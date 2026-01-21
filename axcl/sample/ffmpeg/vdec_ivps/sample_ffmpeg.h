/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#ifndef __SAMPLE_FFMPEG_H__
#define __SAMPLE_FFMPEG_H__

#include <stdio.h>
#include <stdlib.h>

#include "libavfilter/avfilter.h"
#include "libavfilter/buffersink.h"
#include "libavfilter/buffersrc.h"

#include "libavutil/pixdesc.h"
#include "libavutil/pixfmt.h"


#ifdef __cplusplus
extern "C" {
#endif


typedef struct InputFilterOptions {
    int64_t             trim_start_us;
    int64_t             trim_end_us;

    uint8_t            *name;

    /* When IFILTER_FLAG_CFR is set, the stream is guaranteed to be CFR with
     * this framerate.
     *
     * Otherwise, this is an estimate that should not be relied upon to be
     * accurate */
    AVRational          framerate;

    unsigned            crop_top;
    unsigned            crop_bottom;
    unsigned            crop_left;
    unsigned            crop_right;

    int                 sub2video_width;
    int                 sub2video_height;

    // a combination of IFILTER_FLAG_*
    unsigned            flags;

    AVFrame            *fallback;
} InputFilterOptions;

typedef struct InputFilter {
    struct FilterGraph *graph;
    uint8_t            *name;
} InputFilter;

typedef struct OutputFilter {
    const AVClass       *class;

    struct FilterGraph  *graph;
    uint8_t             *name;

    /* for filters that are not yet bound to an output stream,
     * this stores the output linklabel, if any */
    int                  bound;
    uint8_t             *linklabel;

    char                *apad;

    enum AVMediaType     type;
} OutputFilter;

typedef struct FilterGraph {
    const AVClass *class;
    int            index;

    InputFilter   **inputs;
    int          nb_inputs;
    OutputFilter **outputs;
    int         nb_outputs;
} FilterGraph;

typedef struct OutputFilterPriv {
    OutputFilter            ofilter;

    int                     index;

    void                   *log_parent;
    char                    log_name[32];

    char                   *name;

    AVFilterContext        *filter;

    /* desired output stream properties */
    int                     format;
    int                     width, height;
    int                     sample_rate;
    AVChannelLayout         ch_layout;
    enum AVColorSpace       color_space;
    enum AVColorRange       color_range;

    // time base in which the output is sent to our downstream
    // does not need to match the filtersink's timebase
    AVRational              tb_out;
    // at least one frame with the above timebase was sent
    // to our downstream, so it cannot change anymore
    int                     tb_out_locked;

    AVRational              sample_aspect_ratio;

    AVDictionary           *sws_opts;
    AVDictionary           *swr_opts;

    // those are only set if no format is specified and the encoder gives us multiple options
    // They point directly to the relevant lists of the encoder.
    const int              *formats;
    const AVChannelLayout  *ch_layouts;
    const int              *sample_rates;
    const enum AVColorSpace *color_spaces;
    const enum AVColorRange *color_ranges;

    AVRational              enc_timebase;
    int64_t                 trim_start_us;
    int64_t                 trim_duration_us;
    // offset for output timestamps, in AV_TIME_BASE_Q
    int64_t                 ts_offset;
    int64_t                 next_pts;

    unsigned                flags;
} OutputFilterPriv;

typedef struct InputFilterPriv {
    InputFilter         ifilter;

    InputFilterOptions  opts;

    int                 index;

    AVFilterContext    *filter;

    // used to hold submitted input
    AVFrame            *frame;

    /* for filters that are not yet bound to an input stream,
     * this stores the input linklabel, if any */
    uint8_t            *linklabel;

    // filter data type
    enum AVMediaType    type;
    // source data type: AVMEDIA_TYPE_SUBTITLE for sub2video,
    // same as type otherwise
    enum AVMediaType    type_src;

    int                 eof;
    int                 bound;

    // parameters configured for this input
    int                 format;

    int                 width, height;
    AVRational          sample_aspect_ratio;
    enum AVColorSpace   color_space;
    enum AVColorRange   color_range;

    int                 sample_rate;
    AVChannelLayout     ch_layout;

    AVRational          time_base;

    AVBufferRef        *hw_frames_ctx;

    int                 displaymatrix_present;
    int                 displaymatrix_applied;
    int32_t             displaymatrix[9];

    struct {
        AVFrame *frame;

        int64_t last_pts;
        int64_t end_pts;

        ///< marks if sub2video_update should force an initialization
        unsigned int initialize;
    } sub2video;
} InputFilterPriv;

#ifdef __cplusplus
}
#endif
#endif
