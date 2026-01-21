/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#include "ffmpeg.hpp"
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <exception>
#include <string>
#include <vector>
#include "axcl_rt.h"
#include "elapser.hpp"
#include "nalu_lock_fifo.hpp"
#include "threadx.hpp"
#include "utils/logger.h"

extern "C" {
#include "libavcodec/avcodec.h"
#include "libavcodec/bsf.h" /* ffmpeg 7.1 */
#include "libavformat/avformat.h"
}

template <size_t N>
inline const char *av_err_msg(int err, char (&msg)[N]) {
    av_strerror(err, msg, N);
    return msg;
}

#define FFMPEG_CONTEXT(handle) reinterpret_cast<struct ffmpeg_context *>(handle)

static constexpr const char *ffmpeg_demuxer_attr_frame_rate_control = "ffmpeg.demux.file.frc";
static constexpr const char *ffmpeg_demuxer_attr_file_loop = "ffmpeg.demux.file.loop";
static constexpr const char *ffmpeg_demuxer_attr_total_frame_count = "ffmpeg.demux.total_frame_count";

struct ffmpeg_context {
    std::string url;
    int32_t cookie = -1;
    int32_t device = -1;
    struct stream_info info;
    axcl::threadx demux_thread;
    axcl::threadx dispatch_thread;
    AVFormatContext *avfmt_ctx = nullptr;
    AVBSFContext *avbsf_ctx = nullptr;
    int32_t video_track_id = -1;
    axcl::event eof;

    /* sink and sink userdata */
    stream_sink sink;
    uint64_t userdata = 0;

    /* dispatch fifo */
    nalu_lock_fifo *fifo = nullptr;

    /* attribute */
    bool frame_rate_control = false;
    bool loop = false;
    uint64_t total_count = 0;

/**
 * just for debug:
 * check dispatch put and pop nalu data
 */
//  #define __SAVE_NALU_DATA__
#if defined(__SAVE_NALU_DATA__)
    FILE *fput = nullptr;
    FILE *fpop = nullptr;
    std::vector<std::vector<uint8_t>> put_data;
#endif

    memory_stream mem_stream;
};

static int ffmpeg_init_demuxer(ffmpeg_context *context);
static int ffmpeg_deinit_demuxer(ffmpeg_context *context);
static void ffmpeg_demux_thread(ffmpeg_context *context);
static void ffmpeg_dispatch_thread(ffmpeg_context *context);

int ffmpeg_create_demuxer(ffmpeg_demuxer *demuxer, const char *url, int32_t cookie, int32_t device, stream_sink sink, uint64_t userdata,
                          memory_stream mem_stream) {
    if (!url || device <= 0 || !demuxer) {
        if (demuxer) {
            *demuxer = nullptr;
        }
        SAMPLE_LOG_E("invalid parameters");
        return -EINVAL;
    }

    *demuxer = nullptr;

    ffmpeg_context *context = new (std::nothrow) ffmpeg_context();
    if (!context) {
        SAMPLE_LOG_E("create ffmpeg demux context fail");
        return -EFAULT;
    }

    context->url = url;
    context->cookie = cookie;
    context->device = device;
    context->sink = sink;
    context->userdata = userdata;
    context->mem_stream = mem_stream;

    if (int ret = ffmpeg_init_demuxer(context); 0 != ret) {
        delete context;
        return ret;
    }

    *demuxer = reinterpret_cast<ffmpeg_demuxer>(context);
    return 0;
}

int ffmpeg_destory_demuxer(ffmpeg_demuxer demuxer) {
    ffmpeg_context *context = FFMPEG_CONTEXT(demuxer);
    if (!context) {
        SAMPLE_LOG_E("invalid ffmpeg demux context");
        return -EINVAL;
    }
    if (int ret = ffmpeg_deinit_demuxer(context); 0 != ret) {
        return ret;
    }

    if (context->fifo) {
        delete context->fifo;
    }

    delete context;
    return 0;
}

const struct stream_info *ffmpeg_get_stream_info(ffmpeg_demuxer demuxer) {
    ffmpeg_context *context = reinterpret_cast<ffmpeg_context *>(demuxer);
    if (!context) {
        SAMPLE_LOG_E("invalid context handle");
        return nullptr;
    }

    return &context->info;
}

int ffmpeg_start_demuxer(ffmpeg_demuxer demuxer) {
    ffmpeg_context *context = FFMPEG_CONTEXT(demuxer);
    if (!context) {
        SAMPLE_LOG_E("invalid ffmpeg demux context");
        return -EINVAL;
    }

    char name[16];
    sprintf(name, "dispatch%d", context->cookie);
    context->dispatch_thread.start(name, ffmpeg_dispatch_thread, context);

    sprintf(name, "demux%d", context->cookie);
    context->demux_thread.start(name, ffmpeg_demux_thread, context);
    context->demux_thread.detach();
    return 0;
}

static inline void ffmpeg_stop_dispatch(ffmpeg_context *context) {
    context->dispatch_thread.stop();
}

int ffmpeg_stop_demuxer(ffmpeg_demuxer demuxer) {
    ffmpeg_context *context = FFMPEG_CONTEXT(demuxer);
    if (!context) {
        SAMPLE_LOG_E("invalid ffmpeg demux context");
        return -EINVAL;
    }
    context->demux_thread.stop();
    ffmpeg_stop_dispatch(context);
    context->dispatch_thread.join();
    return 0;
}

static void ffmpeg_demux_eof(ffmpeg_context *context) {
    context->eof.set();
}

int ffmpeg_wait_demuxer_eof(ffmpeg_demuxer demuxer, int32_t timeout) {
    ffmpeg_context *context = FFMPEG_CONTEXT(demuxer);
    if (!context) {
        SAMPLE_LOG_E("invalid ffmpeg demux context");
        return -EINVAL;
    }
    return context->eof.wait(timeout) ? 0 : -1;
}

int ffmpeg_set_demuxer_sink(ffmpeg_demuxer demuxer, stream_sink sink, uint64_t userdata) {
    ffmpeg_context *context = FFMPEG_CONTEXT(demuxer);
    if (!context) {
        SAMPLE_LOG_E("invalid ffmpeg demux context");
        return -EINVAL;
    }
    context->sink = sink;
    context->userdata = userdata;
    return 0;
}

static void ffmpeg_dispatch_thread(ffmpeg_context *context) {
    SAMPLE_LOG_I("[%d] +++", context->cookie);

    /**
     * As sink callback will invoke axcl API, axcl runtime context should be created for dispatch thread.
     * Remind to destory before thread exit.
     */
    axclrtContext runtime;
    if (axclError ret = axclrtCreateContext(&runtime, context->device); AXCL_SUCC != ret) {
        return;
    }

#ifdef __SAVE_NALU_DATA__
    context->fpop = fopen("./fpop.raw", "wb");
#endif

    int32_t ret;
    uint64_t count = 0;
    while (context->dispatch_thread.running() || context->fifo->size() > 0) {
        nalu_data nalu;
        uint32_t total_len = 0;

        // SAMPLE_LOG_I("peek size %d frame %ld +++", context->fifo->size(), count);
        if (ret = context->fifo->peek(nalu, total_len, -1); 0 != ret) {
            if (-EINTR != ret) {
                SAMPLE_LOG_E("[%d] peek from fifo fail, ret = %d", context->cookie, ret);
            }
            break;
        }
        // SAMPLE_LOG_I("peek size %d frame %ld ---", context->fifo->size(), count);

        struct stream_data stream;
        stream.payload = context->info.video.payload;
        stream.cookie = context->cookie;
        stream.video.pts = nalu.pts;
        if (nalu.len2 > 0) {
            stream.video.size = nalu.len + nalu.len2;
            stream.video.data = reinterpret_cast<uint8_t *>(malloc(stream.video.size));
            if (!stream.video.data) {
                SAMPLE_LOG_E("[%d] malloc size %d fail", context->cookie, stream.video.size);
                break;
            }
            memcpy(stream.video.data, nalu.nalu, nalu.len);
            memcpy(stream.video.data + nalu.len, nalu.nalu2, nalu.len2);
        } else {
            stream.video.size = nalu.len;
            stream.video.data = nalu.nalu;
        }

        if (stream.video.size > 0) {
            ++count;

#if defined(__SAVE_NALU_DATA__)
            fwrite((const void *)stream.video.data, 1, stream.video.size, context->fpop);
            if (stream.video.size != context->put_data[count - 1].size()) {
                SAMPLE_LOG_E("frame %ld, size not equal %d != %ld", count, stream.video.size, context->put_data[count - 1].size());
            } else {
                if (0 != memcmp(context->put_data[count - 1].data(), stream.video.data, stream.video.size)) {
                    SAMPLE_LOG_E("frame %ld, size %d, data not equal", count, stream.video.size);
                }
            }
#endif
        }

        context->sink.on_stream_data(&stream, context->userdata);

        /* pop from fifo */
        context->fifo->skip(total_len);

        if (nalu.len2 > 0) {
            free(stream.video.data);
        }
    }

#ifdef __SAVE_NALU_DATA__
    fflush(context->fpop);
    fclose(context->fpop);
#endif

    /* notify eof */
    ffmpeg_demux_eof(context);

    /* destory axcl runtime context */
    axclrtDestroyContext(runtime);
    SAMPLE_LOG_I("[%d] dispatched total %lu frames ---", context->cookie, (unsigned long)count);
}

static int32_t ffmpeg_seek_to_begin(ffmpeg_context *context) {
    /**
     * AVSEEK_FLAG_BACKWARD may fail (example: zhuheqiao.mp4), use AVSEEK_FLAG_ANY, but not guarantee seek to I frame
     */
    av_bsf_flush(context->avbsf_ctx);
    int32_t ret = av_seek_frame(context->avfmt_ctx, context->video_track_id, 0, AVSEEK_FLAG_ANY /* AVSEEK_FLAG_BACKWARD */);
    if (ret < 0) {
        char msg[64];
        // SAMPLE_LOG_W("[%d] seek to begin fail, %s, retry once ...", context->cookie, av_err_msg(ret, msg));
        ret = avformat_seek_file(context->avfmt_ctx, context->video_track_id, INT64_MIN, 0, INT64_MAX, AVSEEK_FLAG_BYTE);
        if (ret < 0) {
            SAMPLE_LOG_E("[%d] seek to begin fail, %s", context->cookie, av_err_msg(ret, msg));
            return ret;
        }
    }

    return ret;
}

static void ffmpeg_demux_thread(ffmpeg_context *context) {
    SAMPLE_LOG_I("[%d] +++", context->cookie);

    int ret;
    char msg[64] = {0};
    uint64_t pts = 0;
    uint64_t now = 0;
    uint64_t next_tick = 0;
    const uint64_t interval = 1000000 / context->info.video.fps;

    AVPacket *avpkt = av_packet_alloc();
    if (!avpkt) {
        SAMPLE_LOG_E("[%d] av_packet_alloc() fail!", context->cookie);
        return;
    }

#ifdef __SAVE_NALU_DATA__
    context->fput = fopen("./fput.raw", "wb");
#endif

    context->total_count = 0;
    context->eof.reset();
    while (context->demux_thread.running()) {
        ret = av_read_frame(context->avfmt_ctx, avpkt);
        if (ret < 0) {
            if (AVERROR_EOF == ret) {
                if (context->loop) {
                    if (ret = ffmpeg_seek_to_begin(context); 0 != ret) {
                        break;
                    }

                    continue;
                }

                SAMPLE_LOG_I("[%d] reach eof", context->cookie);
                if (context->sink.on_stream_data) {
                    /* send eof */
                    nalu_data nalu = {};
                    nalu.userdata = context->userdata;
                    if (ret = context->fifo->push(nalu, -1); 0 != ret) {
                        if (-EINTR != ret) {
                            SAMPLE_LOG_E("[%d] push eof to fifo fail, ret = %d", context->cookie, ret);
                        }
                    }
                }

                ffmpeg_stop_dispatch(context);
                break;
            } else {
                SAMPLE_LOG_E("[%d] av_read_frame() fail, %s", context->cookie, av_err_msg(ret, msg));
                break;
            }

        } else {
            if (avpkt->stream_index == context->video_track_id) {
                ret = av_bsf_send_packet(context->avbsf_ctx, avpkt);
                if (ret < 0) {
                    av_packet_unref(avpkt);
                    SAMPLE_LOG_E("[%d] av_bsf_send_packet() fail, %s", context->cookie, av_err_msg(ret, msg));
                    break;
                }

                while (ret >= 0) {
                    ret = av_bsf_receive_packet(context->avbsf_ctx, avpkt);
                    if (ret < 0) {
                        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
                            break;
                        }

                        av_packet_unref(avpkt);
                        SAMPLE_LOG_E("[%d] av_bsf_receive_packet() fail, %s", context->cookie, av_err_msg(ret, msg));

                        ffmpeg_stop_dispatch(context);
                        return;
                    }

                    ++context->total_count;

                    pts += interval;

                    nalu_data nalu = {};
                    nalu.userdata = context->userdata;
                    nalu.pts = pts;
                    nalu.nalu = avpkt->data;
                    nalu.len = avpkt->size;

#ifdef __SAVE_NALU_DATA__
                    fwrite(nalu.nalu, 1, nalu.len, context->fput);
                    context->put_data.emplace_back(nalu.nalu, nalu.nalu + nalu.len);
#endif

                    if (context->frame_rate_control) {
                        /**
                         * Simulate fps by sleeping.
                         * 1. If the host is AX650: Since the CPU frequency is 100Hz, we use the high-precision delay (AX_SYS_Usleep)
                         * provided by the hardware timer64.
                         * 2. If the host is x64: Ensure that a high-precision delay is implemented.
                         *
                         * if now is behind than next, sleep(next - now)
                         * if now is forward  to next, do nothing
                         * # 1          2          3          4          5          6          7  us
                         * | interval |
                         * |----------|----------|----------|----------|----------|----------|
                         * ^    next2                 next4              ^                  us
                         * ^                                             ^
                         * now2                                          now4                us
                         */
                        now = axcl::elapser::ticks();
                        if (context->total_count <= 1) {
                            next_tick = now + interval;
                        } else {
                            if (now < next_tick) {
                                auto delay = next_tick - now;
                                delay -= (delay % 1000); /* truncated to ms */
                                axcl::elapser::ax_usleep((uint32_t)delay);
                            } else {
                                // SAMPLE_LOG_W("[%d] frame %lu: now %lu, expected %lu, delay %lu ==> execced fps interval %lu",
                                //         context->cookie, (unsigned long)context->total_count, (unsigned long)now,
                                //         (unsigned long)next_tick, (unsigned long)(now - next_tick), (unsigned long)interval);
                            }

                            next_tick += interval;
                        }
                    }

                    if (ret = context->fifo->push(nalu, -1); 0 != ret) {
                        SAMPLE_LOG_E("[%d] push frame %lu len %d to fifo fail, ret = %d", context->cookie,
                                     (unsigned long)context->total_count, nalu.len, ret);
                    }
                }
            }

            av_packet_unref(avpkt);
        }
    }

#ifdef __SAVE_NALU_DATA__
    fflush(context->fput);
    fclose(context->fput);
#endif

    av_packet_free(&avpkt);
    SAMPLE_LOG_I("[%d] demuxed    total %lu frames ---", context->cookie, (unsigned long)context->total_count);
}

static int simple_read_buffer(void *opaque, uint8_t *buf, int buf_size) {
    memory_stream *mem_data = (memory_stream *)opaque;
    if (!mem_data || !buf || buf_size < 0) {
        return AVERROR(EINVAL);
    }

    size_t remaining = mem_data->size - mem_data->pos;
    if (remaining <= 0) {
        return AVERROR_EOF;
    }

    int to_read = (buf_size < (int)remaining) ? buf_size : (int)remaining;
    if (mem_data->pos + to_read > mem_data->size) {
        return AVERROR(EINVAL);
    }

    memcpy(buf, mem_data->data + mem_data->pos, to_read);
    mem_data->pos += to_read;
    return to_read;
}

static int ffmpeg_init_demuxer(ffmpeg_context *context) {
    SAMPLE_LOG_I("[%d] url: %s", context->cookie, context->url.c_str());

    char msg[64] = {0};
    int ret;
    AVIOContext *avio_ctx = NULL;
    context->avfmt_ctx = avformat_alloc_context();
    if (!context->avfmt_ctx) {
        SAMPLE_LOG_E("[%d] avformat_alloc_context() fail.", context->cookie);
        return -EFAULT;
    }

    if (context->url == TEST_MEMORY_VIDEO) {
        memory_stream *mem_stream = (memory_stream *)av_malloc(sizeof(memory_stream));
        if (!mem_stream) {
            SAMPLE_LOG_E("[%d] av_malloc() fail.", context->cookie);
            return -EFAULT;
        }

        mem_stream->data = context->mem_stream.data;
        mem_stream->size = context->mem_stream.size;
        mem_stream->pos  = context->mem_stream.pos;

        unsigned char *buffer = (unsigned char *)av_malloc(32 * 1024);
        if (!buffer) {
            av_free(mem_stream);
            return -EFAULT;
        }

        avio_ctx = avio_alloc_context(buffer, 32 * 1024, 0, mem_stream, simple_read_buffer, NULL, NULL);
        if (!avio_ctx) {
            av_free(buffer);
            av_free(mem_stream);
            return -EFAULT;
        }

        context->avfmt_ctx->pb = avio_ctx;
    }

    do {
        if (avio_ctx) {
            ret = avformat_open_input(&context->avfmt_ctx, NULL, NULL, NULL);
        } else {
            ret = avformat_open_input(&context->avfmt_ctx, context->url.c_str(), NULL, NULL);
        }
        if (ret < 0) {
            SAMPLE_LOG_E("[%d] open %s fail, %s", context->cookie, context->url.c_str(), av_err_msg(ret, msg));
            break;
        }

        ret = avformat_find_stream_info(context->avfmt_ctx, NULL);
        if (ret < 0) {
            SAMPLE_LOG_E("[%d] avformat_find_stream_info() fail, %s", context->cookie, av_err_msg(ret, msg));
            break;
        }

        for (unsigned int i = 0; i < context->avfmt_ctx->nb_streams; i++) {
            if (AVMEDIA_TYPE_VIDEO == context->avfmt_ctx->streams[i]->codecpar->codec_type) {
                context->video_track_id = i;
                break;
            }
        }

        if (-1 == context->video_track_id) {
            ret = -EINVAL;
            SAMPLE_LOG_E("[%d] has no video stream", context->cookie);
            break;
        } else {
            AVStream *avs = context->avfmt_ctx->streams[context->video_track_id];
            switch (avs->codecpar->codec_id) {
                case AV_CODEC_ID_H264:
                    context->info.video.payload = PT_H264;
                    break;
                case AV_CODEC_ID_HEVC:
                    context->info.video.payload = PT_H265;
                    break;
                default:
                    SAMPLE_LOG_E("[%d] unsupport video codec %d!", context->cookie, avs->codecpar->codec_id);
                    break;
            }

            context->info.video.width = avs->codecpar->width;
            context->info.video.height = avs->codecpar->height;
            context->fifo = new nalu_lock_fifo(context->info.video.width * context->info.video.height * 2);

            if (avs->avg_frame_rate.den == 0 || (avs->avg_frame_rate.num == 0 && avs->avg_frame_rate.den == 1)) {
                context->info.video.fps = static_cast<uint32_t>(round(av_q2d(avs->r_frame_rate)));
            } else {
                context->info.video.fps = static_cast<uint32_t>(round(av_q2d(avs->avg_frame_rate)));
            }

            if (0 == context->info.video.fps) {
                context->info.video.fps = 25;
                SAMPLE_LOG_W("[%d] url %s: fps is 0, set to %d fps", context->cookie, context->url.c_str(), context->info.video.fps);
            }

            SAMPLE_LOG_I("[%d] url %s: codec %d, %dx%d, fps %d", context->cookie, context->url.c_str(), context->info.video.payload,
                         context->info.video.width, context->info.video.height, context->info.video.fps);
        }

        if (PT_H264 == context->info.video.payload || PT_H265 == context->info.video.payload) {
            const AVBitStreamFilter *bsf =
                av_bsf_get_by_name((PT_H264 == context->info.video.payload) ? "h264_mp4toannexb" : "hevc_mp4toannexb");
            if (!bsf) {
                ret = -EINVAL;
                SAMPLE_LOG_E("[%d] av_bsf_get_by_name() fail, make sure input is mp4|h264|h265", context->cookie);
                break;
            }

            ret = av_bsf_alloc(bsf, &context->avbsf_ctx);
            if (ret < 0) {
                SAMPLE_LOG_E("[%d] av_bsf_alloc() fail, %s", context->cookie, av_err_msg(ret, msg));
                break;
            }

            ret = avcodec_parameters_copy(context->avbsf_ctx->par_in, context->avfmt_ctx->streams[context->video_track_id]->codecpar);
            if (ret < 0) {
                SAMPLE_LOG_E("[%d] avcodec_parameters_copy() fail, %s", context->cookie, av_err_msg(ret, msg));
                break;
            }

            context->avbsf_ctx->time_base_in = context->avfmt_ctx->streams[context->video_track_id]->time_base;

            ret = av_bsf_init(context->avbsf_ctx);
            if (ret < 0) {
                SAMPLE_LOG_E("[%d] av_bsf_init() fail, %s", context->cookie, av_err_msg(ret, msg));
                break;
            }
        }

        return 0;

    } while (0);

    ffmpeg_deinit_demuxer(context);
    return ret;
}

static int ffmpeg_deinit_demuxer(ffmpeg_context *context) {
    if (context->avfmt_ctx) {
        AVIOContext *avio_ctx = context->avfmt_ctx->pb;
        if (context->mem_stream.data && context->mem_stream.size > 0 && avio_ctx) {
            memory_stream *mem_stream = (memory_stream *)avio_ctx->opaque;
            avformat_close_input(&context->avfmt_ctx);
            if (mem_stream) {
                av_free(mem_stream);
            }
            avio_context_free(&avio_ctx);
        } else {
            avformat_close_input(&context->avfmt_ctx);
        }

        /*  avformat_close_input will free ctx
            http://ffmpeg.org/doxygen/trunk/demux_8c_source.html
        */
        // avformat_free_context(avfmt_ctx);
        context->avfmt_ctx = nullptr;
    }

    if (context->fifo) {
        delete context->fifo;
        context->fifo = nullptr;
    }

    return 0;
}

int ffmpeg_set_demuxer_attr(ffmpeg_demuxer demuxer, const char *name, const void *attr) {
    if (!name) {
        SAMPLE_LOG_E("nil attribute name");
        return -EINVAL;
    }

    ffmpeg_context *context = FFMPEG_CONTEXT(demuxer);
    if (!context) {
        SAMPLE_LOG_E("invalid ffmpeg demux context");
        return -EINVAL;
    }
    if (0 == strcmp(name, ffmpeg_demuxer_attr_frame_rate_control)) {
        context->frame_rate_control = (1 == *(reinterpret_cast<const int32_t *>(attr))) ? true : false;
        SAMPLE_LOG_I("[%d] set %s to %d", context->cookie, ffmpeg_demuxer_attr_frame_rate_control, context->frame_rate_control);
    } else if (0 == strcmp(name, ffmpeg_demuxer_attr_file_loop)) {
        context->loop = (1 == *(reinterpret_cast<const int32_t *>(attr))) ? true : false;
        SAMPLE_LOG_I("[%d] set %s to %d", context->cookie, ffmpeg_demuxer_attr_file_loop, context->loop);
    } else {
        SAMPLE_LOG_E("[%d] unsupport attribute %s", context->cookie, name);
        return -EINVAL;
    }

    return 0;
}

int ffmpeg_get_demuxer_attr(ffmpeg_demuxer demuxer, const char *name, void *attr) {
    if (!name) {
        SAMPLE_LOG_E("nil attribute name");
        return -EINVAL;
    }

    ffmpeg_context *context = FFMPEG_CONTEXT(demuxer);
    if (!context) {
        SAMPLE_LOG_E("invalid ffmpeg demux context");
        return -EINVAL;
    }
    if (0 == strcmp(name, ffmpeg_demuxer_attr_total_frame_count)) {
        *(reinterpret_cast<uint64_t *>(attr)) = context->total_count;
    } else {
        SAMPLE_LOG_E("[%d] unsupport attribute %s", context->cookie, name);
        return -EINVAL;
    }

    return 0;
}
