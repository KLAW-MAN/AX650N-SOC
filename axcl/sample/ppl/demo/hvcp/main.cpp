/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/
#include <signal.h>
#include <string.h>
#include <chrono>
#include <string>
#include "axcl_ppl.h"
#include "axcl_ppl_hvcp_type.h"
#include "cmdline.h"
#include "demux/ffmpeg.hpp"
#include "res_guard.hpp"
#include "threadx.hpp"
#include "utils/logger.h"
#include "utils/person_test_ov2_640x360_2frames.mp4.h"
#include "os.hpp"

static int32_t quit = 0;
static void handler(int s) {
    SAMPLE_LOG_W("\n====================== pid %d caught signal: %d ======================\n", getpid(), s);
    quit = 1;
}

/**
 * @brief Processes video stream nalu frame data after pulling the stream (e.g., via RTSP ... protocols).
 *        Once the nalu frame is received, the axcl_ppl_send_stream API is called to send the nalu frame to VDEC for decoding.
 *
 *        As for example, the nalu frames are obtained by demuxing a local video file (mp4|.264|.265).
 *        There is no frame rate control, and this example uses delays to simulate frame rate control for stream pulling.
 * @param nalu Specifies the nalu frame buffer obtained by demuxing a local video file.
 * @param userdata The bypass value from `axcl_ppl_encoded_stream.userdata`
 */
static void handle_down_streaming_nalu_frame(const struct stream_data *nalu, uint64_t userdata);

/**
 * @brief Callback function to receive the NPU inference output.
 *        The application should avoid performing high-latency operations within this callback function.
 *
 * @param ppl The PPL handle created by `axcl_ppl_create`.
 * @param output The inference result.
 * @param userdata The bypass value from `axcl_ppl_hvcp_param.userdata`.
 */

static void handle_hvcp_output_callback(axcl_ppl ppl, const axcl_ppl_hvcp_output *output, AX_U64 userdata);

int main(int argc, char *argv[]) {
    const int32_t pid = static_cast<int32_t>(getpid());
    SAMPLE_LOG_I("============== %s sample started %s %s pid %d ==============\n", AXCL_BUILD_VERSION, __DATE__, __TIME__, pid);
    signal(SIGINT, handler);

    cmdline::parser a;
    a.add<std::string>("url", 'i', "mp4|.264|.265 file path or test.memory.video", true);
    a.add<std::string>("model", 'm', "model path", true);
    a.add<uint32_t>("device", 'd', "device index from 0 to connected device num - 1", false, 0,
                    cmdline::range(0, AXCL_MAX_DEVICE_COUNT - 1));
    a.add<int32_t>("vnpu", 'v', "type of Visual-NPU inited {0=Disable, 1=STD, 2=BigLittle, 3=LittleBig}", false, 0, cmdline::range(0, 3));
    a.add<int32_t>("affinity", 'a', "npu affinity when running a model", false, 0b111, cmdline::range(1, 0b111));
    a.add<int32_t>("loop", '\0', "1: loop demux for local file  0: no loop(default)", false, 0, cmdline::oneof(0, 1));
    a.add<std::string>("json", '\0', "axcl.json path", false, "./axcl.json");

    a.parse_check(argc, argv);

    const std::string url = a.get<std::string>("url");
    const uint32_t device_index = a.get<uint32_t>("device");
    const std::string model_path = a.get<std::string>("model");
    const int32_t loop = a.get<int32_t>("loop");
    const std::string json = a.get<std::string>("json");
    const int32_t vnpu = a.get<int32_t>("vnpu");
    const int32_t affinity = a.get<int32_t>("affinity");

    memory_stream mem_stream = {0};
    if (url == TEST_MEMORY_VIDEO) {
        mem_stream.data = (uint8_t *)&person_test_ov2_640x360_2frames_mp4[0];
        mem_stream.size = (size_t)person_test_ov2_640x360_2frames_mp4_len;
    }

    axclError ret;
    axcl_ppl ppl;

    /**
     * @brief Initialize system runtime:
     *     1. axclInit
     *     2. axclrtSetDevice
     *     3. initialize media modules
     */
    axcl_ppl_init_param init_param = {};
    init_param.json = json.c_str();
    init_param.device_index = device_index;
    init_param.modules = AXCL_LITE_VDEC | AXCL_LITE_IVPS;
    init_param.vdec_clk = AX_VDEC_HWCLK_624M;
    init_param.max_vdec_grp = 32;
    if (ret = axcl_ppl_init(&init_param); AXCL_SUCC != ret) {
        SAMPLE_LOG_E("axcl_ppl_init(device index %d) fail, ret = 0x%x", device_index, ret);
        return 1;
    }

    const int32_t device_id = init_param.device_id;
    SAMPLE_LOG_I("pid: %d, device index: %d, bus number: %d", pid, init_param.device_index, init_param.device_id);

    /**
     * @brief Create ffmpeg demuxer
     *        Use RAII to auto release resource of demuxer by `res_guard` template class
     *    1. Probe stream
     *    2. Retrieve stream information such as width, height, payload, fps.
     */
    res_guard<ffmpeg_demuxer> demuxer_holder(
        [&url, device_id, pid, mem_stream]() -> ffmpeg_demuxer {
            ffmpeg_demuxer handle;
            ffmpeg_create_demuxer(&handle, url.c_str(), pid, device_id, {}, 0, mem_stream);
            return handle;
        },
        [](ffmpeg_demuxer &handle) {
            if (handle) {
                ffmpeg_destory_demuxer(handle);
            }
        });

    ffmpeg_demuxer demuxer = demuxer_holder.get();
    if (!demuxer) {
        axcl_ppl_deinit();
        return 1;
    } else {
        constexpr int32_t frc = 1; /* enable fps control */
        ffmpeg_set_demuxer_attr(demuxer, "ffmpeg.demux.file.frc", (const void *)&frc);
        ffmpeg_set_demuxer_attr(demuxer, "ffmpeg.demux.file.loop", (const void *)&loop);
    }

    /**
     * @brief Configures the PPL parameters and creates a PPL instance.
     *
     * 1. Configures the video decoder attributes based on the FFmpeg probed stream information.
     * 2. Registers the callback for handling model inference outputs.
     */
    axcl_ppl_hvcp_param hvcp_ppl_param = {};
    /* codec configuration */
    hvcp_ppl_param.vdec.payload = ffmpeg_get_stream_info(demuxer)->video.payload;
    hvcp_ppl_param.vdec.width = ffmpeg_get_stream_info(demuxer)->video.width;
    hvcp_ppl_param.vdec.height = ffmpeg_get_stream_info(demuxer)->video.height;
    hvcp_ppl_param.vdec.output_order = AX_VDEC_OUTPUT_ORDER_DISP;
    hvcp_ppl_param.vdec.display_mode = AX_VDEC_DISPLAY_MODE_PREVIEW;

    strcpy(hvcp_ppl_param.hvcp_param.model_path, model_path.c_str());
    hvcp_ppl_param.hvcp_param.vnpu_kind = vnpu;
    hvcp_ppl_param.hvcp_param.affinity = affinity;
    hvcp_ppl_param.hvcp_param.callback = handle_hvcp_output_callback;
    hvcp_ppl_param.hvcp_param.userdata = 0;

    axcl_ppl_param ppl_param = {};
    ppl_param.ppl = AXCL_PPL_HVCP;
    ppl_param.param = (void *)&hvcp_ppl_param;
    if (ret = axcl_ppl_create(&ppl, &ppl_param); AXCL_SUCC != ret) {
        SAMPLE_LOG_E("axcl_ppl_create(device id %d) fail, ret = 0x%x", device_id, ret);
        axcl_ppl_deinit();
        return 1;
    }

    /* Register AnnexB nalu frame handler: send to video decoder */
    ffmpeg_set_demuxer_sink(demuxer, {handle_down_streaming_nalu_frame}, reinterpret_cast<uint64_t>(ppl));

    /**
     * @brief Start transcode ppl and then start to demux.
     */
    if (ret = axcl_ppl_start(ppl); AXCL_SUCC != ret) {
        axcl_ppl_destroy(ppl);
        axcl_ppl_deinit();
        return 1;
    }

    ffmpeg_start_demuxer(demuxer);

    /**
     * transcoding ...
     */
    while (!quit) {
        std::this_thread::sleep_for(std::chrono::seconds(1));

        if (0 == ffmpeg_wait_demuxer_eof(demuxer, 0)) {
            /**
             * @brief wait eof of local stream file
             */
            SAMPLE_LOG_I("ffmpeg (pid %d) demux eof", static_cast<uint32_t>(getpid()));
            break;
        }
    }

    /**
     * @brief Stop ppl and ffmpeg demuxer.
     */
    axcl_ppl_stop(ppl);
    ffmpeg_stop_demuxer(demuxer);

    /**
     * @brief Destroy ppl and deinitialize system.
     */
    axcl_ppl_destroy(ppl);
    axcl_ppl_deinit();

    uint64_t frame_count = 0;
    ffmpeg_get_demuxer_attr(demuxer, "ffmpeg.demux.total_frame_count", &frame_count);
    SAMPLE_LOG_I("total frames: %lu", (unsigned long)frame_count);
    SAMPLE_LOG_I("============== %s sample exited %s %s pid %d ==============\n", AXCL_BUILD_VERSION, __DATE__, __TIME__, pid);
    return 0;
}

static void handle_down_streaming_nalu_frame(const struct stream_data *nalu, uint64_t userdata) {
    axcl_ppl ppl = reinterpret_cast<axcl_ppl>(userdata);

    axcl_ppl_input_stream stream;
    stream.nalu = nalu->video.data;
    stream.nalu_len = nalu->video.size;
    stream.pts = nalu->video.pts;
    stream.userdata = userdata;
    if (axclError ret = axcl_ppl_send_stream(ppl, &stream, 1000); AXCL_SUCC != ret) {
        if (AXCL_ERR_LITE_PPL_NOT_STARTED != ret) {
            SAMPLE_LOG_E("axcl_ppl_send_stream() fail, ret = 0x%x", ret);
        }
    }
}

static void handle_hvcp_output_callback(axcl_ppl ppl, const axcl_ppl_hvcp_output *output, AX_U64 userdata) {
    SAMPLE_LOG_I("frame %8lld: hvcp count %3d", output->frame_id, output->count);
#if 0
    /* print detail boxes */
    for (AX_U32 i = 0; i < output->count; ++i) {
        const hvcp_box &box = output->item[i].box;
        SAMPLE_LOG_I("    %2d: x = %4d, y = %4d, w = %4d, h = %4d, confidence %f", i, box.x, box.y, box.w, box.h,
                     output->item[i].confidence);
    }
#endif
}
