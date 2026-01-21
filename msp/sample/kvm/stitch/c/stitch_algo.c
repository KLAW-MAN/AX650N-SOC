#include "stitch_algo.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PRINT printf
#define SKITCH_ALGO_VERSION ("1.0")

static const int scale_f_bits = 10;
static const int width_align = 2;

typedef enum {
    FLOOR,
    CEILING,
    ROUND,
} ALIGN;

static int align(int var, int align_num, ALIGN align_method) {
    if (var % align_num != 0) {
        int k;
        if (align_method == FLOOR) {
            k = floor(var / (double)align_num);
        } else if (align_method == CEILING) {
            k = ceil(var / (double)align_num);
        } else {
            k = round(var / (double)align_num);
        }
        return k * align_num;
    } else {
        return var;
    }
}

static int do_stitch(int source_total_width, int partition_num, int *disp_width, stitch_output_meta_t *output_config) {
    output_config->part_num = partition_num;

    int disp_width_sum = 0;
    for (int n = 0; n < partition_num; n++) {
        disp_width_sum += disp_width[n];
    }

    double scale = source_total_width / (double)disp_width_sum;
    scale = floor(scale * pow(2, scale_f_bits)) / pow(2, scale_f_bits);

    // partition 0
    int crop_left = 0;
    int offset = 0;
    int crop_right = 0;
    int output_width = disp_width[0] + crop_left;
    int output_width_aligned = align(output_width, width_align, CEILING);
    crop_right = output_width_aligned - output_width;
    while (crop_right % width_align != 0) {
        output_width_aligned += width_align;
        crop_right = output_width_aligned - output_width;
    }

    int source_width_end = floor((output_width - 1 + 1 + 0.5) * scale - 0.5) + offset;
    int source_width = floor((output_width_aligned - 1 + 0.5) * scale - 0.5) + 1 + 1;
    int source_width_aligned = align(source_width, width_align, CEILING);

    output_config->parts[0].ori_crop_start = offset;
    output_config->parts[0].ori_crop_length = source_width_aligned;
    output_config->parts[0].output_length = output_width_aligned;
    output_config->parts[0].output_crop_on_left = crop_left;
    output_config->parts[0].output_crop_on_right = crop_right;
    output_config->parts[0].output_right_end = source_width_end;
    output_config->parts[0].scale = scale;

    // partition 1 ~ n-2
    for (int n = 1; n < partition_num - 1; n++) {
        int extra_left = 0;
        crop_left = ceil((extra_left + 0.5) / scale - 0.5);
        offset = output_config->parts[n - 1].output_right_end - extra_left;

        if (fmod((1 / scale), 2) != 0.0) {
            while (crop_left % width_align != 0 || offset % width_align != 0) {
                extra_left += 1;
                offset -= 1;
                crop_left = ceil((extra_left + 0.5) / scale - 0.5);
            }
        } else {
            if (offset % width_align != 0) {
                extra_left += 1;
                offset -= 1;
                crop_left = ceil((extra_left + 0.5) / scale - 0.5);
            }
            if (crop_left % width_align != 0) {
                crop_left += 1;
            }
        }

        crop_right = 0;
        output_width = disp_width[n] + crop_left;
        output_width_aligned = align(output_width, width_align, CEILING);
        crop_right = output_width_aligned - output_width;
        while (crop_right % width_align != 0) {
            output_width_aligned += width_align;
            crop_right = output_width_aligned - output_width;
        }
        source_width_end = floor((output_width - 1 + 1 + 0.5) * scale - 0.5) + offset;
        source_width = floor((output_width_aligned - 1 + 0.5) * scale - 0.5) + 1 + 1;
        source_width_aligned = align(source_width, width_align, CEILING);

        output_config->parts[n].ori_crop_start = offset;
        output_config->parts[n].ori_crop_length = source_width_aligned;
        output_config->parts[n].output_length = output_width_aligned;
        output_config->parts[n].output_crop_on_left = crop_left;
        output_config->parts[n].output_crop_on_right = crop_right;
        output_config->parts[n].output_right_end = source_width_end;
        output_config->parts[n].scale = scale;
    }

    // partition n-1
    int extra_left = 0;
    crop_left = ceil((extra_left + 0.5) / scale - 0.5);
    offset = output_config->parts[partition_num - 2].output_right_end - extra_left;

    if (fmod((1 / scale), 2) != 0.0) {
        while (crop_left % width_align != 0 || offset % width_align != 0) {
            extra_left += 1;
            offset -= 1;
            crop_left = ceil((extra_left + 0.5) / scale - 0.5);
        }
    } else {
        if (offset % width_align != 0) {
            extra_left += 1;
            offset -= 1;
            crop_left = ceil((extra_left + 0.5) / scale - 0.5);
        }
        if (crop_left % width_align != 0) {
            crop_left += 1;
        }
    }

    output_width = disp_width[partition_num - 1] + crop_left;
    if (offset + (floor((output_width - 1 + 0.5) * scale - 0.5) + 1) > source_total_width) {
        PRINT("There is NOT enough source pixel for last partition to output %d pixels.\n", disp_width[partition_num - 1]);
        return -1;
    }

    output_width_aligned = align(output_width, width_align, CEILING);
    crop_right = output_width_aligned - output_width;
    while (crop_right % width_align != 0) {
        output_width_aligned += width_align;
        crop_right = output_width_aligned - output_width;
    }
    source_width_end = floor((output_width - 1 + 1 + 0.5) * scale - 0.5) + offset;
    source_width = floor((output_width_aligned - 1 + 0.5) * scale - 0.5) + 1 + 1;
    source_width_aligned = align(source_width, width_align, CEILING);
    int pad_num = fmax(offset + source_width_aligned - source_total_width, 0);
    int pad_num_aligned = align(pad_num, width_align, CEILING);

    output_config->parts[partition_num - 1].ori_crop_start = offset;
    output_config->parts[partition_num - 1].ori_crop_length = source_width_aligned;
    output_config->parts[partition_num - 1].output_length = output_width_aligned;
    output_config->parts[partition_num - 1].output_crop_on_left = crop_left;
    output_config->parts[partition_num - 1].output_crop_on_right = crop_right;
    output_config->parts[partition_num - 1].output_right_end = source_width_end;
    output_config->parts[partition_num - 1].scale = scale;

    output_config->ori_pad_num_end = pad_num_aligned;

    return 0;
}

static int get_output_by_meta(const stitch_input_param_t *input, stitch_output_param_t *output) {
    if (output->w_meta.part_num > 1 && output->h_meta.part_num <= 1) {
        /* horizontal only */
        output->part_num = output->w_meta.part_num;
        for (int i = 0; i < output->part_num; ++i) {
            stitch_output_partiton_t *part = &output->parts[i];
            part->box.x = input->box.x + output->w_meta.parts[i].ori_crop_start;
            part->box.y = input->box.y;
            part->box.w = output->w_meta.parts[i].ori_crop_length;
            part->box.h = input->box.h;
            part->dst_w = output->w_meta.parts[i].output_length;
            part->dst_h = input->stitch_total_h;
            part->w_scale_step = (int)(output->w_meta.parts[i].scale * 1024);
            part->h_scale_step = part->box.h * 1024 / part->dst_h;
            part->overlap.x = output->w_meta.parts[i].output_crop_on_left;
            part->overlap.y = 0;
            part->overlap.w = input->w_parts.parts[i];
            part->overlap.h = part->dst_h;
        }

    } else if (output->w_meta.part_num <= 1 && output->h_meta.part_num > 1) {
        /* vertical only */
        output->part_num = output->h_meta.part_num;
        for (int i = 0; i < output->part_num; ++i) {
            stitch_output_partiton_t *part = &output->parts[i];
            part->box.x = input->box.x;
            part->box.y = input->box.y + output->h_meta.parts[i].ori_crop_start;
            part->box.w = input->box.w;
            part->box.h = output->h_meta.parts[i].ori_crop_length;
            part->dst_w = input->stitch_total_w;
            part->dst_h = output->h_meta.parts[i].output_length;
            part->w_scale_step = part->box.w * 1024 / part->dst_w;
            part->h_scale_step = (int)(output->h_meta.parts[i].scale * 1024);
            part->overlap.x = 0;
            part->overlap.y = output->h_meta.parts[i].output_crop_on_left;
            part->overlap.w = part->dst_w;
            part->overlap.h = input->h_parts.parts[i];
        }
    } else {
        /* horizontal + vertical */
        output->part_num = output->w_meta.part_num * output->h_meta.part_num;
        int part_num = 0;
        for (int j = 0; j < output->h_meta.part_num; ++j) {
            for (int i = 0; i < output->w_meta.part_num; ++i) {
                stitch_output_partiton_t *part = &output->parts[part_num];

                part->box.x = input->box.x + output->w_meta.parts[i].ori_crop_start;
                part->box.y = input->box.y + output->h_meta.parts[j].ori_crop_start;
                part->box.w = output->w_meta.parts[i].ori_crop_length;
                part->box.h = output->h_meta.parts[j].ori_crop_length;
                part->dst_w = output->w_meta.parts[i].output_length;
                part->dst_h = output->h_meta.parts[j].output_length;
                part->w_scale_step = (int)(output->w_meta.parts[i].scale * 1024);
                part->h_scale_step = (int)(output->h_meta.parts[j].scale * 1024);
                part->overlap.x = output->w_meta.parts[i].output_crop_on_left;
                part->overlap.y = output->h_meta.parts[j].output_crop_on_left;
                part->overlap.w = input->w_parts.parts[i];
                part->overlap.h = input->h_parts.parts[j];

                ++part_num;
            }
        }
    }

    return 0;
}

int stitch_algorithm(const stitch_input_param_t *input, stitch_output_param_t *output) {
    if (!input || !output) {
        PRINT("nil input or output\n");
        return -1;
    }

    memset(output, 0, sizeof(stitch_output_param_t));

    if (input->src_w <= 0 || input->src_h <= 0 || input->box.x < 0 || input->box.y < 0 || input->box.w < 0 || input->box.h < 0 ||
        (input->box.x + input->box.w) > input->src_w || (input->box.y + input->box.h) > input->src_h || input->stitch_total_w <= 0 ||
        input->stitch_total_h <= 0) {
        PRINT("invalid input image parameters! src: %d x %d, box: [(%d, %d) %d x %d] stitch %d x %d\n", input->src_w, input->src_h,
              input->box.x, input->box.y, input->box.w, input->box.h, input->stitch_total_w, input->stitch_total_h);
        return -1;
    }

    if (input->w_parts.part_num < 0 || input->w_parts.part_num > MAX_STITCH_PARTITION_NUM || input->h_parts.part_num < 0 ||
        input->h_parts.part_num > MAX_STITCH_PARTITION_NUM || (0 == input->w_parts.part_num && 0 == input->h_parts.part_num)) {
        PRINT("invalid input partiton parameters! w %d, h %d\n", input->w_parts.part_num, input->h_parts.part_num);
        return -1;
    }

    if (input->w_parts.part_num > 1) {
        for (int i = 0; i < input->w_parts.part_num; ++i) {
            if (input->w_parts.parts[i] <= 0) {
                PRINT("invalid input horizontal partiton parameters! partiton[%d] = %d\n", i, input->w_parts.parts[i]);
                return -1;
            }
        }
    }

    if (input->h_parts.part_num > 1) {
        for (int i = 0; i < input->h_parts.part_num; ++i) {
            if (input->h_parts.parts[i] <= 0) {
                PRINT("invalid input vertical partiton parameters! partiton[%d] = %d\n", i, input->h_parts.parts[i]);
                return -1;
            }
        }
    }

    if (input->w_parts.part_num > 1) {
        if (0 != do_stitch(input->stitch_total_w, input->w_parts.part_num, (int *)&input->w_parts.parts[0], &output->w_meta)) {
            return -1;
        }
    }

    if (input->h_parts.part_num > 1) {
        if (0 != do_stitch(input->stitch_total_h, input->h_parts.part_num, (int *)&input->h_parts.parts[0], &output->h_meta)) {
            return -1;
        }
    }

    return get_output_by_meta(input, output);
}

void stitch_dump_output(const stitch_output_param_t *output) {
    PRINT("skitch algorithm version: %s\n", SKITCH_ALGO_VERSION);

    const stitch_output_meta_t *meta[2] = {&output->w_meta, &output->h_meta};
    for (int i = 0; i < 2; ++i) {
        if (meta[i]->part_num > 0) {
            PRINT("meta[%d]:\n", i);
            for (int j = 0; j < meta[i]->part_num; ++j) {
                PRINT("  partition%d: {\n", j);
                PRINT("     ori_crop_start: %d\n", meta[i]->parts[j].ori_crop_start);
                PRINT("     ori_crop_length: %d\n", meta[i]->parts[j].ori_crop_length);
                PRINT("     output_length: %d\n", meta[i]->parts[j].output_length);
                PRINT("     output_crop_on_left: %d\n", meta[i]->parts[j].output_crop_on_left);
                PRINT("     output_crop_on_right: %d\n", meta[i]->parts[j].output_crop_on_right);
                PRINT("     output_right_end: %d\n", meta[i]->parts[j].output_right_end);
                PRINT("     scale: %f\n", meta[i]->parts[j].scale);
                PRINT("  }\n");
            }
            PRINT("  ori_pad_num_end: %d\n", meta[i]->ori_pad_num_end);
        }
    }

    PRINT("\noutput: %d\n", output->part_num);
    for (int i = 0; i < output->part_num; ++i) {
        PRINT("  [%d] => [(%4d, %4d) %4d x %4d] -> [(0, 0) %4d x %4d] scale_w: %4d, scaler_h: %4d  overlap: [(%4d, %4d) %4d x %4d]\n", i,
              output->parts[i].box.x, output->parts[i].box.y, output->parts[i].box.w, output->parts[i].box.h, output->parts[i].dst_w,
              output->parts[i].dst_h, output->parts[i].w_scale_step, output->parts[i].h_scale_step, output->parts[i].overlap.x,
              output->parts[i].overlap.y, output->parts[i].overlap.w, output->parts[i].overlap.h);
    }
}
