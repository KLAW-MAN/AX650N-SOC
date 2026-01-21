/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

/**
 * This example shows how to do hardware accelerated encoding. now only support NV12
 * raw file, usage like: sample_venc 1920 1080 input.yuv output.h264
 *
 */

#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#ifdef WINDOWS
#include <io.h>
#include <string>
#include <iostream>
#include "cmdline.h"
#else
#include <unistd.h>
#endif

extern "C" {
#include "libavcodec/avcodec.h"
#include "libavutil/hwcontext.h"
#include "libavutil/opt.h"
#include "libavutil/pixdesc.h"
}
static int width, height;
static AVBufferRef *hw_device_ctx = NULL;

static void print_usage() {
    printf("usage: sample_ffmpeg_venc <args>\n");
    printf("  -c:       encoder name.                     h264_axenc or hevc_axenc, default: h264_axenc\n");
    printf("  -i:       input nv12 yuv file path\n");
    printf("  -w:       width of input yuv file\n");
    printf("  -h:       height of input yuv file\n");
    printf("  -o:       output encoded stream file path   default: ./dump.encoded.nalu\n");
    printf("  -d:       device index                      device index from 0 to connected device num - 1, default: 0\n");
}

static int set_hwframe_ctx(AVCodecContext *ctx, AVBufferRef *hw_device_ctx) {
    AVBufferRef *hw_frames_ref;
    AVHWFramesContext *frames_ctx = NULL;
    int err = 0;

    if (!(hw_frames_ref = av_hwframe_ctx_alloc(hw_device_ctx))) {
        fprintf(stderr, "Failed to create hardware frame context.\n");
        return -1;
    }
    frames_ctx = (AVHWFramesContext *)(hw_frames_ref->data);
    frames_ctx->format = AV_PIX_FMT_AXMM;
    frames_ctx->sw_format = AV_PIX_FMT_NV12;
    frames_ctx->width = width;
    frames_ctx->height = height;
    frames_ctx->initial_pool_size = 20;
    if ((err = av_hwframe_ctx_init(hw_frames_ref)) < 0) {
        char err_buf[AV_ERROR_MAX_STRING_SIZE];
        av_strerror(err, err_buf, AV_ERROR_MAX_STRING_SIZE);
        fprintf(stderr,
                "Failed to initialize hw frame context."
                "Error code: %s\n",
                err_buf);
        av_buffer_unref(&hw_frames_ref);
        return err;
    }
    ctx->hw_frames_ctx = av_buffer_ref(hw_frames_ref);
    if (!ctx->hw_frames_ctx) {
        err = AVERROR(ENOMEM);
    }

    av_buffer_unref(&hw_frames_ref);
    return err;
}

static int encode_write(AVCodecContext *avctx, AVFrame *frame, FILE *fout) {
    int ret = 0;
    AVPacket *enc_pkt;

    if (!(enc_pkt = av_packet_alloc())) {
        return -1;
    }

    if ((ret = avcodec_send_frame(avctx, frame)) < 0) {
        char err_buf[AV_ERROR_MAX_STRING_SIZE];
        av_strerror(ret, err_buf, AV_ERROR_MAX_STRING_SIZE);
        fprintf(stderr, "Error code: %s\n", err_buf);
        goto end;
    }
    while (1) {
        ret = avcodec_receive_packet(avctx, enc_pkt);
        if (ret) {
            break;
        }

        enc_pkt->stream_index = 0;
        (void)fwrite(enc_pkt->data, 1, enc_pkt->size, fout);
        av_packet_unref(enc_pkt);
    }

end:
    av_packet_free(&enc_pkt);
    ret = ((ret == AVERROR(EAGAIN)) ? 0 : -1);
    return ret;
}

int main(int argc, char *argv[]) {
    int size, err;
    FILE *fin = NULL, *fout = NULL;
    AVFrame *sw_frame = NULL, *hw_frame = NULL;
    AVCodecContext *avctx = NULL;
    const AVCodec *codec = NULL;
    const char *enc_name = "h264_axenc";
    AVDictionary *dict = NULL;
    AVDictionary *opts = NULL;

    const char *input_file = NULL;
    const char *dump_file = "./dump.encoded.nalu";
    int device_index = 0;

#ifdef WINDOWS
    // Parse command line arguments using cmdline
    cmdline::parser parser;
    parser.add<std::string>("codec", 'c', "encoder codec name", false, "h264_axenc");
    parser.add<int>("width", 'w', "frame width", true);
    parser.add<int>("height", 'h', "frame height", true);
    parser.add<std::string>("input", 'i', "input file path", true);
    parser.add<std::string>("output", 'o', "output file path", false, "./dump.encoded.nalu");
    parser.add<int>("device", 'd', "device index", false, 0);

    if (!parser.parse(argc, argv)) {
        print_usage();
        exit(0);
    }

    // Get parsed values
    std::string codec_name = parser.get<std::string>("codec");
    width = parser.get<int>("width");
    height = parser.get<int>("height");
    std::string input_file_str = parser.get<std::string>("input");
    std::string dump_file_str = parser.get<std::string>("output");
    device_index = parser.get<int>("device");

    // Convert to C strings for compatibility
    enc_name = const_cast<char*>(codec_name.c_str());
    input_file = const_cast<char*>(input_file_str.c_str());
    dump_file = const_cast<char*>(dump_file_str.c_str());
#else
    int c;
    int quit = 1;

    while ((c = getopt(argc, argv, "w:h:c:i:o:d:")) != -1) {
        quit = 0;
        switch (c) {
            case 'c':
                enc_name = optarg;
                break;
            case 'w':
                width = atoi(optarg);
                break;
            case 'h':
                height = atoi(optarg);
                break;
            case 'i':
                input_file = optarg;
                break;
            case 'o':
                dump_file = optarg;
                break;
            case 'd':
                device_index = atoi(optarg);
                break;
            default:
                quit = 1;
                break;
        }
    }

    if (quit) {
        print_usage();
        exit(0);
    }
#endif

    if (device_index < 0 || device_index >= 256) {
        fprintf(stderr, "device index %d is out of range[0, 255]\n", device_index);
        return -1;
    } else {
        char value[8];
        sprintf(value, "%d", device_index);
        av_dict_set(&dict, "device_index", value, 0);
    }

    if (width <= 0 || height <= 0) {
        fprintf(stderr, "invalid input width %d or height %d, [-w=width] [-h=height]\n", width, height);
        return -1;
    } else {
        size = width * height;
    }

    if (!input_file || !(fin = fopen(input_file, "rb"))) {
        fprintf(stderr, "fail to open input file %s, %s\n", input_file, strerror(errno));
        return -1;
    }

    if (!(fout = fopen(dump_file, "w+b"))) {
        fprintf(stderr, "fail to open dump file %s, %s\n", dump_file, strerror(errno));
        fclose(fin);
        return -1;
    }

    err = av_dict_set(&dict, "alloc_blk", "1", 0);
    if (0 != err) {
        char err_buf[AV_ERROR_MAX_STRING_SIZE];
        av_strerror(err, err_buf, AV_ERROR_MAX_STRING_SIZE);
        fprintf(stderr, "Failed to set alloc block. Error code: %s\n", err_buf);
        goto close;
    }

    err = av_hwdevice_ctx_create(&hw_device_ctx, AV_HWDEVICE_TYPE_AXMM, NULL, dict, 0);
    if (err < 0) {
        char err_buf[AV_ERROR_MAX_STRING_SIZE];
        av_strerror(err, err_buf, AV_ERROR_MAX_STRING_SIZE);
        fprintf(stderr, "Failed to create a hw device. Error code: %s\n", err_buf);
        goto close;
    }

    if (!(codec = avcodec_find_encoder_by_name(enc_name))) {
        fprintf(stderr, "Could not find encoder.\n");
        err = -1;
        goto close;
    }

    if (!(avctx = avcodec_alloc_context3(codec))) {
        err = AVERROR(ENOMEM);
        goto close;
    }

    avctx->width = width;
    avctx->height = height;
    avctx->time_base.num = 1;
    avctx->time_base.den = 25;
    avctx->framerate.num = 25;
    avctx->framerate.den = 1;
    avctx->sample_aspect_ratio.num = 1;
    avctx->sample_aspect_ratio.den = 1;
    avctx->pix_fmt = AV_PIX_FMT_AXMM;

    av_opt_set(avctx->priv_data, "i_qmin", "12", 0);
    av_opt_set(avctx->priv_data, "i_qmax", "48", 0);
    av_opt_set(avctx, "qmin", "18", 0);
    av_opt_set(avctx, "qmax", "50", 0);

    av_dict_set(&opts, "qmin", "15", 0);
    av_opt_set_dict(avctx, &opts);
    av_dict_free(&opts);

    avctx->hw_device_ctx = av_buffer_ref(hw_device_ctx);
    if (!avctx->hw_device_ctx) {
        fprintf(stderr, "Failed to set hardware device context.\n");
        err = AVERROR(ENOMEM);
        goto close;
    }

    /* set hw_frames_ctx for encoder's AVCodecContext */
    if ((err = set_hwframe_ctx(avctx, hw_device_ctx)) < 0) {
        fprintf(stderr, "Failed to set hwframe context.\n");
        goto close;
    }

    if ((err = avcodec_open2(avctx, codec, NULL)) < 0) {
        char err_buf[AV_ERROR_MAX_STRING_SIZE];
        av_strerror(err, err_buf, AV_ERROR_MAX_STRING_SIZE);
        fprintf(stderr, "Cannot open video encoder codec. Error code: %s\n", err_buf);
        goto close;
    }

    while (1) {
        if (!(sw_frame = av_frame_alloc())) {
            err = AVERROR(ENOMEM);
            goto close;
        }
        /* read data into software frame, and transfer them into hw frame */
        sw_frame->width = width;
        sw_frame->height = height;
        sw_frame->format = AV_PIX_FMT_NV12;
        if ((err = av_frame_get_buffer(sw_frame, 0)) < 0) {
            goto close;
        }

        if ((err = (int)fread((uint8_t *)(sw_frame->data[0]), 1, size, fin)) != size) {
            if (!feof(fin)) {
                fprintf(stderr, "read Y from file failed\n");
            }
            break;
        }

        if ((err = (int)fread((uint8_t *)(sw_frame->data[1]), 1, size / 2, fin)) != size / 2) {
            fprintf(stderr, "read UV from file failed\n");
            break;
        }

        if (!(hw_frame = av_frame_alloc())) {
            err = AVERROR(ENOMEM);
            goto close;
        }

        if ((err = av_hwframe_get_buffer(avctx->hw_frames_ctx, hw_frame, 0)) < 0) {
            char err_buf[AV_ERROR_MAX_STRING_SIZE];
            av_strerror(err, err_buf, AV_ERROR_MAX_STRING_SIZE);
            fprintf(stderr, "Error code: %s.\n", err_buf);
            goto close;
        }

        if (!hw_frame->hw_frames_ctx) {
            err = AVERROR(ENOMEM);
            goto close;
        }

        if ((err = av_hwframe_transfer_data(hw_frame, sw_frame, 0)) < 0) {
            char err_buf[AV_ERROR_MAX_STRING_SIZE];
            av_strerror(err, err_buf, AV_ERROR_MAX_STRING_SIZE);
            fprintf(stderr,
                    "Error while transferring frame data to surface."
                    "Error code: %s.\n",
                    err_buf);
            goto close;
        }

        if ((err = (encode_write(avctx, hw_frame, fout))) < 0) {
            fprintf(stderr, "Failed to encode.\n");
            goto close;
        }

        av_frame_free(&hw_frame);
        av_frame_free(&sw_frame);
    }

    /* flush encoder */
    err = encode_write(avctx, NULL, fout);
    if (err == AVERROR_EOF) {
        err = 0;
    }

close:
    if (fin) {
        fclose(fin);
    }
    if (fout) {
        fclose(fout);
    }
    av_frame_free(&sw_frame);
    av_frame_free(&hw_frame);
    avcodec_free_context(&avctx);
    av_buffer_unref(&hw_device_ctx);

    return err;
}
