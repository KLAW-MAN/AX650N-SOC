/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#pragma once
#include <stddef.h>
#include <stdint.h>
#include "ax_global_type.h"

#define TEST_MEMORY_VIDEO "test.memory.video"

struct video_stream {
    uint8_t *data;
    uint32_t size;
    uint64_t pts;
    uint64_t seq_num;
};

struct audio_stream {};

struct stream_data {
    int32_t cookie;
    AX_PAYLOAD_TYPE_E payload;
    union {
        struct video_stream video;
        struct audio_stream audio;
    };
};

struct stream_info {
    struct video_info {
        AX_PAYLOAD_TYPE_E payload;
        uint32_t width;
        uint32_t height;
        uint32_t fps;
    } video;

    struct audio_info {
        AX_PAYLOAD_TYPE_E payload;
        uint32_t bps;
        uint32_t sample_rate;
        uint32_t sample_bits;
    } audio;
};

typedef struct {
    uint8_t *data;
    size_t size;
    size_t pos;
} memory_stream;

typedef struct {
    void (*on_stream_data)(const struct stream_data *data, uint64_t userdata);
} stream_sink;

typedef void *ffmpeg_demuxer;

/**
 * demux mp4 video to raw h264 or h265 nalu frame.
 * raw h264 or h265 also supported.
 */
int ffmpeg_create_demuxer(ffmpeg_demuxer *demuxer, const char *url, int32_t cookie, int32_t device, stream_sink sink, uint64_t userdata, memory_stream mem_stream);
int ffmpeg_destory_demuxer(ffmpeg_demuxer demuxer);

const struct stream_info *ffmpeg_get_stream_info(ffmpeg_demuxer demuxer);

int ffmpeg_start_demuxer(ffmpeg_demuxer demuxer);
int ffmpeg_stop_demuxer(ffmpeg_demuxer demuxer);

int ffmpeg_wait_demuxer_eof(ffmpeg_demuxer demuxer, int32_t timeout);

int ffmpeg_set_demuxer_sink(ffmpeg_demuxer demuxer, stream_sink sink, uint64_t userdata);

/**
 * @brief set attribute
 * @param demuxer handle created by ffmpeg_create_demuxer
 * @param name attribute string name
 * @param attr attribute value
 *              name                                         attr
 *                                                   type            value
 *  ffmpeg.demux.file.frc                    [W]   int32_t   1: enable, 0: disable(default)
 *  ffmpeg.demux.file.loop                   [W]   int32_t   1: loop, 0: once(default)
 *  ffmpeg.demux.total_frame_count           [R]   uint64_t
 */
int ffmpeg_set_demuxer_attr(ffmpeg_demuxer demuxer, const char *name, const void *attr);
int ffmpeg_get_demuxer_attr(ffmpeg_demuxer demuxer, const char *name, void *attr);
