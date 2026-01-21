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
#ifdef WINDOWS
    #include <windows.h>
    #define sleep(x) Sleep((x) * 1000)
#else
    #include <unistd.h>
#endif

// Fix cmdline.h compatibility issues
#ifndef NOMINMAX
#define NOMINMAX
#endif

// Prevent Windows.h from defining min/max macros
#ifdef WINDOWS
    #ifdef max
        #undef max
    #endif
    #ifdef min
        #undef min
    #endif
#endif

#include "axcl_ppl.h"
#include "axcl_ppl_default_venc_rc.h"
#include "axcl_ppl_transcode_type.h"
#include "cmdline.h"
#include "demux/ffmpeg.hpp"
#include "ppl_userdata.hpp"
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
 * @brief Retrieves transcode pipeline parameters from stream information.
 * @param info Stream information, including width, height, payload, and fps.
 * @param h265 Output h264 or h265 stream.
 * @param cb Callback function to handle the transcoded stream processed by VDEC - IVPS - VENC.
 * @param userdata User data for the callback function.
 *                 The entire pipeline will pass the user data to axcl_ppl_encoded_stream_callback_func.
 * @return axcl_ppl_transcode_param
 */

static axcl_ppl_transcode_param get_transcode_ppl_param_from_stream(const struct stream_info *info, bool h265,
                                                                    axcl_ppl_encoded_stream_callback_func cb, uint64_t userdata);

/**
 * @brief Processes video stream nalu frame data after pulling the stream (e.g., via RTSP ... protocols).
 *        Once the nalu frame is received, the axcl_ppl_send_stream API is called to send the nalu frame to VDEC for decoding.
 *
 *        As for example, the nalu frames are obtained by demuxing a local video file (mp4|.264|.265).
 *        There is no frame rate control, and this example uses delays to simulate frame rate control for stream pulling.
 * @param nalu specifies the nalu frame obtained by demuxing a local video file.
 * @param userdata bypass user data to axcl_ppl_encoded_stream.userdata
 */
static void handle_down_streaming_nalu_frame(const struct stream_data *nalu, uint64_t userdata);

/**
 * @brief The axcl_ppl processes transcoded nalu frame by this callback function.
 *        Note: Avoid any high-latency operations within this callback function.
 * @param ppl ppl handle create by axcl_ppl_create
 * @param stream nalu stream encoded by VENC
 * @param userdata bypass user data from axcl_ppl_transcode_param.userdata
 */
static void handle_encoded_stream_callback(axcl_ppl ppl, const axcl_ppl_encoded_stream *stream, AX_U64 userdata);

int main(int argc, char *argv[]) {
    const int32_t pid = static_cast<int32_t>(getpid());
    SAMPLE_LOG_I("============== %s sample started %s %s pid %d ==============\n", AXCL_BUILD_VERSION, __DATE__, __TIME__, pid);
    signal(SIGINT, handler);

    cmdline::parser a;
    a.add<std::string>("url", 'i', "mp4|.264|.265| file path or test.memory.video", true);
    a.add<uint32_t>("device", 'd', "device index from 0 to connected device num - 1", false, 0,
                    cmdline::range(0, AXCL_MAX_DEVICE_COUNT - 1));
    a.add<uint32_t>("width", 'w', "output width, 0 means same as input", false, 0);
    a.add<uint32_t>("height", 'h', "output height, 0 means same as input", false, 0);
    a.add<std::string>("codec", '\0', "encoded codec: [h264 | h265] (default: h265)", false, "h265");
    a.add<std::string>("json", '\0', "axcl.json path", false, "./axcl.json");
    a.add<int32_t>("loop", '\0', "1: loop demux for local file  0: no loop(default)", false, 0, cmdline::oneof(0, 1));
    a.add<std::string>("dump", '\0', "dump file path", false, "");
    a.add<uint32_t>("hwclk", '\0', "decoder hw clk, 0: 624M, 1: 500M, 2: 400M(default)", false, 2, cmdline::oneof(0, 1, 2));
    a.add("ut", '\0', "unittest");
    a.parse_check(argc, argv);
    const std::string url = a.get<std::string>("url");
    const uint32_t device_index = a.get<uint32_t>("device");
    uint32_t width = a.get<uint32_t>("width");
    uint32_t height = a.get<uint32_t>("height");
    const std::string codec = a.get<std::string>("codec");
    const std::string json = a.get<std::string>("json");
    int32_t loop = a.get<int32_t>("loop");
    const std::string dump = a.get<std::string>("dump");
    if (dump != "") {
        SAMPLE_LOG_W("if enable dump, disable loop automatically");
        loop = 0;
    }
    const bool is_ut = a.exist("ut");

    memory_stream mem_stream = {0};
    if (url == TEST_MEMORY_VIDEO) {
        width = 640;
        height = 360;
        mem_stream.data = (uint8_t *)&person_test_ov2_640x360_2frames_mp4[0];
        mem_stream.size = (size_t)person_test_ov2_640x360_2frames_mp4_len;
    }

    axclError ret;
    axcl_ppl ppl;
    std::unique_ptr<ppl_user_data, decltype(&ppl_user_data::destroy)> ppl_data(ppl_user_data::alloc(dump.c_str()), &ppl_user_data::destroy);
    ppl_user_data *ppl_info = ppl_data.get();

    /**
     * @brief Initialize system runtime:
     *     1. axclInit
     *     2. axclrtSetDevice
     *     3. initialize media modules
     * @param AXCL_LITE_DEFAULT means VDEC + IVPS + VENC modules
     */
    axcl_ppl_init_param init_param;
    memset(&init_param, 0, sizeof(init_param));
    init_param.json = json.c_str();
    init_param.device_index = device_index;
    init_param.modules = AXCL_LITE_DEFAULT;
    /**
     * vdec -> venc, max performance is 8K30fps, so we can reduce vdec freq to save power consumption
     * If wants high performance of decoder, set high freq. [400000000 | 500000000 |624000000]
     * */
    init_param.vdec_clk = static_cast<AX_VDEC_HWCLK_E>(a.get<uint32_t>("hwclk"));
    /**
     *  !!!
     *  should >= the number of processes
     */
    init_param.max_vdec_grp = 32;
    init_param.max_venc_thd = 1;
    if (ret = axcl_ppl_init(&init_param); AXCL_SUCC != ret) {
        SAMPLE_LOG_E("axcl_ppl_init(device index %d) fail, ret = 0x%x", device_index, ret);
        return 1;
    }

    const int32_t device = init_param.device_id;
    SAMPLE_LOG_I("pid: %d, device index: %d, bus number: %d", pid, init_param.device_index, init_param.device_id);

    /**
     * @brief Create ffmpeg demuxer
     *        Use RAII to auto release resource of demuxer by res_guard template class
     *    1. Probe stream
     *    2. Retrieve stream information such as width, height, payload, fps.
     */
    res_guard<ffmpeg_demuxer> demuxer_holder(
        [&url, device, pid, mem_stream]() -> ffmpeg_demuxer {
            ffmpeg_demuxer handle;
            ffmpeg_create_demuxer(&handle, url.c_str(), pid, device, {}, 0, mem_stream);
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
    }

    constexpr int32_t active_fps = 1;
    ffmpeg_set_demuxer_attr(demuxer, "ffmpeg.demux.file.frc", (const void *)&active_fps);
    ffmpeg_set_demuxer_attr(demuxer, "ffmpeg.demux.file.loop", (const void *)&loop);
    const struct stream_info *info = ffmpeg_get_stream_info(demuxer);

    /**
     * @brief Config input and output parameters (axcl_ppl_transcode_param) of AXCL_PPL_TRANSCODE ppl,
     *       and then create PPL (VDEC - IVPS - VENC)
     */
    axcl_ppl_transcode_param transcode_param = get_transcode_ppl_param_from_stream(
        ffmpeg_get_stream_info(demuxer), 0 == codec.compare("h265"), handle_encoded_stream_callback, reinterpret_cast<uint64_t>(ppl_info));
    if (width > 0 && height > 0) {
        transcode_param.venc.width = width;
        transcode_param.venc.height = height;
    }

    axcl_ppl_param ppl_param;
    memset(&ppl_param, 0, sizeof(ppl_param));
    ppl_param.ppl = AXCL_PPL_TRANSCODE;
    ppl_param.param = (void *)&transcode_param;
    if (ret = axcl_ppl_create(&ppl, &ppl_param); AXCL_SUCC != ret) {
        SAMPLE_LOG_E("axcl_ppl_create(device %d) fail, ret = 0x%x", device, ret);
        axcl_ppl_deinit();
        return 1;
    } else {
        /**
         * @brief Regist callback to handle ffmpeg demux thread
         * @param handle_down_streaming_nalu_frame callback to handle nalu frame
         * @param ppl_info callback user data
         */
        ppl_info->ppl = ppl;
        ppl_info->device = device;
        ppl_info->fps = info->video.fps;

        /* below attributes are for debug or print log, not mandatory */
        axcl_ppl_get_attr(ppl, "axcl.ppl.transcode.vdec.grp", reinterpret_cast<void *>(&ppl_info->vdec));
        axcl_ppl_get_attr(ppl, "axcl.ppl.transcode.ivps.grp", reinterpret_cast<void *>(&ppl_info->ivps));
        axcl_ppl_get_attr(ppl, "axcl.ppl.transcode.venc.chn", reinterpret_cast<void *>(&ppl_info->venc));
        SAMPLE_LOG_I("pid %d: [vdec %02d] - [ivps %02d] - [venc %02d]", pid, ppl_info->vdec, ppl_info->ivps, ppl_info->venc);

        uint32_t vdec_blk_cnt;
        uint32_t vdec_out_depth;
        uint32_t ivps_in_depth;
        uint32_t ivps_out_depth;
        uint32_t ivps_blk_cnt;
        uint32_t ivps_engine;
        uint32_t venc_in_depth;
        uint32_t venc_out_depth;
        axcl_ppl_get_attr(ppl, "axcl.ppl.transcode.vdec.blk.cnt", reinterpret_cast<void *>(&vdec_blk_cnt));
        axcl_ppl_get_attr(ppl, "axcl.ppl.transcode.vdec.out.depth", reinterpret_cast<void *>(&vdec_out_depth));
        axcl_ppl_get_attr(ppl, "axcl.ppl.transcode.ivps.in.depth", reinterpret_cast<void *>(&ivps_in_depth));
        axcl_ppl_get_attr(ppl, "axcl.ppl.transcode.ivps.out.depth", reinterpret_cast<void *>(&ivps_out_depth));
        axcl_ppl_get_attr(ppl, "axcl.ppl.transcode.ivps.blk.cnt", reinterpret_cast<void *>(&ivps_blk_cnt));
        axcl_ppl_get_attr(ppl, "axcl.ppl.transcode.ivps.engine", reinterpret_cast<void *>(&ivps_engine));
        axcl_ppl_get_attr(ppl, "axcl.ppl.transcode.venc.in.depth", reinterpret_cast<void *>(&venc_in_depth));
        axcl_ppl_get_attr(ppl, "axcl.ppl.transcode.venc.out.depth", reinterpret_cast<void *>(&venc_out_depth));
        SAMPLE_LOG_I("pid %d: VDEC attr ==> blk cnt: %d, fifo depth: out %d", pid, vdec_blk_cnt, vdec_out_depth);
        SAMPLE_LOG_I("pid %d: IVPS attr ==> blk cnt: %d, fifo depth: in %d, out %d, engine %d", pid, ivps_blk_cnt, ivps_in_depth,
                     ivps_out_depth, ivps_engine);
        SAMPLE_LOG_I("pid %d: VENC attr ==> fifo depth: in %d, out %d", pid, venc_in_depth, venc_out_depth);

        if (is_ut) {
            int32_t wait_timeout = 2000;
            axcl_ppl_set_attr(ppl, "axcl.ppl.transcode.venc.stop.wait", reinterpret_cast<const void *>(&wait_timeout));
        }

        ffmpeg_set_demuxer_sink(demuxer, {handle_down_streaming_nalu_frame}, reinterpret_cast<uint64_t>(ppl_info));
    }

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

    if (is_ut) {
        uint64_t demux_frame_count = 0;
        ffmpeg_get_demuxer_attr(demuxer, "ffmpeg.demux.total_frame_count", &demux_frame_count);
        if (demux_frame_count > 0) {
            if (demux_frame_count < ppl_info->frame_count.load() - 1) {
                SAMPLE_LOG_E("(pid %d) transcoded frame count %lu is not equal to demux frame count %lu", static_cast<uint32_t>(getpid()),
                             (unsigned long)ppl_info->frame_count.load(), (unsigned long)demux_frame_count);
                while (1) {
                    sleep(1);
                }
            } else {
                SAMPLE_LOG_I("(pid %d) transcoded frame count %lu, demux frame count %lu", static_cast<uint32_t>(getpid()),
                             (unsigned long)ppl_info->frame_count.load(), (unsigned long)demux_frame_count);
            }
        } else {
            SAMPLE_LOG_E("(pid %d) demux frame count is 0", static_cast<uint32_t>(getpid()));
        }
    }

    /**
     * @brief Destroy ppl and deinitialize system.
     */
    axcl_ppl_destroy(ppl);
    axcl_ppl_deinit();

    SAMPLE_LOG_I("total transcoded frames: %lu", (unsigned long)ppl_info->frame_count.load());
    SAMPLE_LOG_I("============== %s sample exited %s %s pid %d ==============\n", AXCL_BUILD_VERSION, __DATE__, __TIME__, pid);
    return 0;
}

#if 0
static void sample_down_streaming_thread(axcl_ppl ppl, int32_t device) {
    SAMPLE_LOG_I("+++");

    /* thread runtime context is needed */
    axclError ret;
    axclrtContext context;
    ret = axclrtCreateContext(&context, device);
    if (AXCL_SUCC != ret) {
        SAMPLE_LOG_E("axclrtCreateContext(device %d) fail, ret = 0x%x", device, ret);
        return;
    }

    constexpr int32_t timeout = 1000;
    axcl_ppl_input_stream stream = {0};
    while (!quit) {
        /**
         * TODO: fill axcl_ppl_input_stream
         */

        ret = axcl_ppl_send_stream(ppl, &stream, timeout);
        if (AXCL_SUCC != ret) {
            SAMPLE_LOG_E("axcl_ppl_send_stream(device %d) fail, ret = 0x%x", device, ret);
            return;
        }
    }

    axclrtDestroyContext(context);
    SAMPLE_LOG_I("---");
}
#endif

static axcl_ppl_transcode_param get_transcode_ppl_param_from_stream(const struct stream_info *info, bool h265,
                                                                    axcl_ppl_encoded_stream_callback_func cb, uint64_t userdata) {
    axcl_ppl_transcode_param transcode_param;
    memset(&transcode_param, 0, sizeof(transcode_param));
    transcode_param.vdec.payload = info->video.payload;
    transcode_param.vdec.width = info->video.width;
    transcode_param.vdec.height = info->video.height;
    transcode_param.vdec.output_order = AX_VDEC_OUTPUT_ORDER_DEC;

    /**
     * AX_VDEC_DISPLAY_MODE_PREVIEW :  pull-streaming such as RTSP
     * AX_VDEC_DISPLAY_MODE_PLAYBACK:  local stream file
     */
    transcode_param.vdec.display_mode = AX_VDEC_DISPLAY_MODE_PLAYBACK;

    transcode_param.venc.width = info->video.width;
    transcode_param.venc.height = info->video.height;
    if (h265) {
        transcode_param.venc.payload = PT_H265;
        transcode_param.venc.profile = AX_VENC_HEVC_MAIN_PROFILE;
        transcode_param.venc.level = AX_VENC_HEVC_LEVEL_5_2;
    } else {
        transcode_param.venc.payload = PT_H264;
        transcode_param.venc.profile = AX_VENC_H264_MAIN_PROFILE;
        transcode_param.venc.level = AX_VENC_H264_LEVEL_5_2;
    }

    transcode_param.venc.gop.enGopMode = AX_VENC_GOPMODE_NORMALP;

    /* simply use default rc, replace with requirement */
    if (h265) {
        transcode_param.venc.rc = axcl_default_rc_h265_cbr_1080p_4096kbps();
    } else {
        transcode_param.venc.rc = axcl_default_rc_h264_cbr_1080p_4096kbps();
    }
    transcode_param.venc.rc.stH265Cbr.u32Gop = info->video.fps * 2;
    transcode_param.venc.rc.stFrameRate.fSrcFrameRate = (AX_F32)info->video.fps;
    transcode_param.venc.rc.stFrameRate.fDstFrameRate = (AX_F32)info->video.fps;

    transcode_param.cb = cb;
    transcode_param.userdata = userdata;

    return transcode_param;
}

static void handle_down_streaming_nalu_frame(const struct stream_data *nalu, uint64_t userdata) {
    ppl_user_data *ppl_info = reinterpret_cast<ppl_user_data *>(userdata);

    axcl_ppl_input_stream stream;
    stream.nalu = nalu->video.data;
    stream.nalu_len = nalu->video.size;
    stream.pts = nalu->video.pts;
    stream.userdata = userdata;
    if (axclError ret = axcl_ppl_send_stream(ppl_info->ppl, &stream, 1000); AXCL_SUCC != ret) {
        if (AXCL_ERR_LITE_PPL_NOT_STARTED != ret) {
            SAMPLE_LOG_E("axcl_ppl_send_stream(pid: %d) fail, ret = 0x%x", static_cast<uint32_t>(ppl_info->pid), ret);
        }
    }
}

static void handle_encoded_stream_callback(axcl_ppl ppl, const axcl_ppl_encoded_stream *stream, AX_U64 userdata) {
    ppl_user_data *ppl_info = reinterpret_cast<ppl_user_data *>(userdata);
    /**
     * Avoid any high-latency operations within this callback function.
     */
    //  SAMPLE_LOG_I("received encoded nalu size %d", stream->stPack.u32Len);
    if (ppl_info->fp) {
        fwrite(stream->stPack.pu8Addr, 1, stream->stPack.u32Len, ppl_info->fp);
    }

    ppl_info->frame_count++;
}
