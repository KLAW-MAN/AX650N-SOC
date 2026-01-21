#include <cJSON.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "stitch_algo.h"

static int case1(void);
static int case2(void);
static int case3(void);
static int case4(void);

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <vgp_stitching_input_config.json> <output.json>\n", argv[0]);
        return 1;
    }

    const char *input_json = argv[1];
    const char *output_json = argv[2];

    /* load input json config */
    FILE *fp = fopen(input_json, "r");
    if (!fp) {
        perror("open input json fail");
        return 1;
    }

    fseek(fp, 0, SEEK_END);
    long fsize = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    char *buf = malloc(fsize + 1);
    if (fsize != fread(buf, 1, fsize, fp)) {
        perror("read input json fail");
        free(buf);
        fclose(fp);
        return 1;
    }

    buf[fsize] = '\0';
    fclose(fp);
    fp = NULL;

    cJSON *json = cJSON_Parse(buf);
    free(buf);
    if (!json) {
        fprintf(stderr, "parse input JSON fail\n");
        return 1;
    }

    stitch_input_param_t input = {0};
    stitch_output_param_t output;

    input.stitch_total_w = cJSON_GetObjectItem(json, "source_total_width")->valueint;
    input.stitch_total_h = 1080;
    input.box.x = 0;
    input.box.y = 0;
    input.box.w = input.stitch_total_w;
    input.box.h = input.stitch_total_h;
    input.src_w = input.stitch_total_w;
    input.src_h = input.stitch_total_h;
    input.w_parts.part_num = cJSON_GetObjectItem(json, "partition_num")->valueint;
    input.h_parts.part_num = 0;
    for (int n = 0; n < input.w_parts.part_num; n++) {
        char key[64];
        sprintf(key, "disp_width_par%d", n);
        input.w_parts.parts[n] = cJSON_GetObjectItem(json, key)->valueint;
    }

    cJSON_Delete(json);
    json = NULL;

    /* do stitch algorithm */
    if (0 != stitch_algorithm(&input, &output)) {
        return 1;
    }

    /* save into output json file */
    fp = fopen(output_json, "w");
    if (!fp) {
        perror("open output json fail");
        return 1;
    }

    json = cJSON_CreateObject();
    for (int n = 0; n < output.w_meta.part_num; n++) {
        cJSON *part = cJSON_CreateObject();
        cJSON_AddNumberToObject(part, "ori_crop_start", output.w_meta.parts[n].ori_crop_start);
        cJSON_AddNumberToObject(part, "ori_crop_length", output.w_meta.parts[n].ori_crop_length);
        cJSON_AddNumberToObject(part, "output_length", output.w_meta.parts[n].output_length);
        cJSON_AddNumberToObject(part, "output_crop_on_left", output.w_meta.parts[n].output_crop_on_left);
        cJSON_AddNumberToObject(part, "output_crop_on_right", output.w_meta.parts[n].output_crop_on_right);
        cJSON_AddNumberToObject(part, "output_right_end", output.w_meta.parts[n].output_right_end);
        cJSON_AddNumberToObject(part, "scale", output.w_meta.parts[n].scale);

        char key[64];
        sprintf(key, "partition%d", n);
        cJSON_AddItemToObject(json, key, part);
    }

    cJSON_AddNumberToObject(json, "ori_pad_num_end", output.w_meta.ori_pad_num_end);

    char *s = cJSON_Print(json);
    fprintf(fp, "%s\n", s);
    free(s);

    cJSON_Delete(json);
    fclose(fp);

    /* cases */
    if (0 != case1()) {
        return 1;
    }
    if (0 != case2()) {
        return 1;
    }
    if (0 != case3()) {
        return 1;
    }
    if (0 != case4()) {
        return 1;
    }

    printf("===============================================\n");
    printf("=================== success ===================\n");
    printf("===============================================\n");
    return 0;
}

static int case1(void) {
    printf("\n================ %s: ================\n", __func__);
    stitch_input_param_t input = {0};
    stitch_output_param_t output;
    input.src_w = 1920;
    input.src_h = 1080;
    input.box.x = 0;
    input.box.y = 0;
    input.box.w = 1920;
    input.box.h = 1080;
    input.stitch_total_w = 1920;
    input.stitch_total_h = 1080;
    input.w_parts.part_num = 2;
    input.w_parts.parts[0] = 1720;
    input.w_parts.parts[1] = 600;
    input.h_parts.part_num = 0;

    if (0 != stitch_algorithm(&input, &output)) {
        return 1;
    }

    stitch_dump_output(&output);

    /* check result */
    const stitch_output_partiton_t expected[] = {
        {
            /* f0 */
            .box =
                {
                    .x = 0,
                    .y = 0,
                    .w = 1424,
                    .h = 1080,
                },
            .dst_w = 1720,
            .dst_h = 1080,
            .w_scale_step = 847,
            .h_scale_step = 1024,
            .overlap =
                {
                    .x = 0,
                    .y = 0,
                    .w = 1720,
                    .h = 1080,
                },
        },
        {
            /* f1 */
            .box =
                {
                    .x = 1416,
                    .y = 0,
                    .w = 504,
                    .h = 1080,
                },
            .dst_w = 608,
            .dst_h = 1080,
            .w_scale_step = 847,
            .h_scale_step = 1024,
            .overlap =
                {
                    .x = 8,
                    .y = 0,
                    .w = 600,
                    .h = 1080,
                },
        },
    };

    for (int i = 0; i < 2; ++i) {
        if (0 != memcmp(&output.parts[i], &expected[i], sizeof(stitch_output_partiton_t))) {
            printf("compare result fail, partiton = %d\n", i);
            return -1;
        }
    }

    return 0;
}

static int case2(void) {
    printf("\n================ %s: ================\n", __func__);
    stitch_input_param_t input = {0};
    stitch_output_param_t output;
    input.src_w = 1920;
    input.src_h = 1080;
    input.box.x = 0;
    input.box.y = 664;
    input.box.w = 1920;
    input.box.h = 416;
    input.stitch_total_w = 1920;
    input.stitch_total_h = 430;
    input.w_parts.part_num = 2;
    input.w_parts.parts[0] = 1720;
    input.w_parts.parts[1] = 1892;
    input.h_parts.part_num = 0;

    if (0 != stitch_algorithm(&input, &output)) {
        return 1;
    }

    stitch_dump_output(&output);

    /* check result */
    const stitch_output_partiton_t expected[] = {
        {
            /* f0 */
            .box =
                {
                    .x = 0,
                    .y = 664,
                    .w = 914,
                    .h = 416,
                },
            .dst_w = 1720,
            .dst_h = 430,
            .w_scale_step = 544,
            .h_scale_step = 990,
            .overlap =
                {
                    .x = 0,
                    .y = 0,
                    .w = 1720,
                    .h = 430,
                },
        },
        {
            /* f1 */
            .box =
                {
                    .x = 908,
                    .y = 664,
                    .w = 1012,
                    .h = 416,
                },
            .dst_w = 1902,
            .dst_h = 430,
            .w_scale_step = 544,
            .h_scale_step = 990,
            .overlap =
                {
                    .x = 10,
                    .y = 0,
                    .w = 1892,
                    .h = 430,
                },
        },
    };

    for (int i = 0; i < 2; ++i) {
        if (0 != memcmp(&output.parts[i], &expected[i], sizeof(stitch_output_partiton_t))) {
            printf("compare result fail, partiton = %d\n", i);
            return -1;
        }
    }

    return 0;
}

static int case3(void) {
    printf("\n================ %s: ================\n", __func__);
    stitch_input_param_t input = {0};
    stitch_output_param_t output;
    input.src_w = 1920;
    input.src_h = 1080;
    input.box.x = 0;
    input.box.y = 0;
    input.box.w = 1920;
    input.box.h = 1080;
    input.stitch_total_w = 1920;
    input.stitch_total_h = 1080;
    input.w_parts.part_num = 3;
    input.w_parts.parts[0] = 1280;
    input.w_parts.parts[1] = 1280;
    input.w_parts.parts[2] = 1280;
    input.h_parts.part_num = 3;
    input.h_parts.parts[0] = 720;
    input.h_parts.parts[1] = 720;
    input.h_parts.parts[2] = 720;

    if (0 != stitch_algorithm(&input, &output)) {
        return 1;
    }

    stitch_dump_output(&output);

    /* check result */
    const stitch_output_partiton_t expected[] = {
        {
            /* f0 */
            .box =
                {
                    .x = 0,
                    .y = 0,
                    .w = 642,
                    .h = 362,
                },
            .dst_w = 1280,
            .dst_h = 720,
            .w_scale_step = 512,
            .h_scale_step = 512,
            .overlap =
                {
                    .x = 0,
                    .y = 0,
                    .w = 1280,
                    .h = 720,
                },
        },
        {
            /* f1 */
            .box =
                {
                    .x = 638,
                    .y = 0,
                    .w = 644,
                    .h = 362,
                },
            .dst_w = 1284,
            .dst_h = 720,
            .w_scale_step = 512,
            .h_scale_step = 512,
            .overlap =
                {
                    .x = 4,
                    .y = 0,
                    .w = 1280,
                    .h = 720,
                },
        },
        {
            /* f2 */
            .box =
                {
                    .x = 1278,
                    .y = 0,
                    .w = 644,
                    .h = 362,
                },
            .dst_w = 1284,
            .dst_h = 720,
            .w_scale_step = 512,
            .h_scale_step = 512,
            .overlap =
                {
                    .x = 4,
                    .y = 0,
                    .w = 1280,
                    .h = 720,
                },
        },
        {
            /* f3 */
            .box =
                {
                    .x = 0,
                    .y = 358,
                    .w = 642,
                    .h = 364,
                },
            .dst_w = 1280,
            .dst_h = 724,
            .w_scale_step = 512,
            .h_scale_step = 512,
            .overlap =
                {
                    .x = 0,
                    .y = 4,
                    .w = 1280,
                    .h = 720,
                },
        },
        {
            /* f4 */
            .box =
                {
                    .x = 638,
                    .y = 358,
                    .w = 644,
                    .h = 364,
                },
            .dst_w = 1284,
            .dst_h = 724,
            .w_scale_step = 512,
            .h_scale_step = 512,
            .overlap =
                {
                    .x = 4,
                    .y = 4,
                    .w = 1280,
                    .h = 720,
                },
        },
        {
            /* f5 */
            .box =
                {
                    .x = 1278,
                    .y = 358,
                    .w = 644,
                    .h = 364,
                },
            .dst_w = 1284,
            .dst_h = 724,
            .w_scale_step = 512,
            .h_scale_step = 512,
            .overlap =
                {
                    .x = 4,
                    .y = 4,
                    .w = 1280,
                    .h = 720,
                },
        },
        {
            /* f6 */
            .box =
                {
                    .x = 0,
                    .y = 718,
                    .w = 642,
                    .h = 364,
                },
            .dst_w = 1280,
            .dst_h = 724,
            .w_scale_step = 512,
            .h_scale_step = 512,
            .overlap =
                {
                    .x = 0,
                    .y = 4,
                    .w = 1280,
                    .h = 720,
                },
        },
        {
            /* f7 */
            .box =
                {
                    .x = 638,
                    .y = 718,
                    .w = 644,
                    .h = 364,
                },
            .dst_w = 1284,
            .dst_h = 724,
            .w_scale_step = 512,
            .h_scale_step = 512,
            .overlap =
                {
                    .x = 4,
                    .y = 4,
                    .w = 1280,
                    .h = 720,
                },
        },
        {
            /* f8 */
            .box =
                {
                    .x = 1278,
                    .y = 718,
                    .w = 644,
                    .h = 364,
                },
            .dst_w = 1284,
            .dst_h = 724,
            .w_scale_step = 512,
            .h_scale_step = 512,
            .overlap =
                {
                    .x = 4,
                    .y = 4,
                    .w = 1280,
                    .h = 720,
                },
        },
    };

    for (int i = 0; i < 9; ++i) {
        if (0 != memcmp(&output.parts[i], &expected[i], sizeof(stitch_output_partiton_t))) {
            printf("compare result fail, partiton = %d\n", i);
            return -1;
        }
    }

    return 0;
}

static int case4(void) {
    printf("\n================ %s: ================\n", __func__);
    stitch_input_param_t input = {0};
    stitch_output_param_t output;
    input.src_w = 1920;
    input.src_h = 1080;
    input.box.x = 0;
    input.box.y = 0;
    input.box.w = 1920;
    input.box.h = 1080;
    input.stitch_total_w = 1920;
    input.stitch_total_h = 1080;
    input.w_parts.part_num = 3;
    input.w_parts.parts[0] = 600;
    input.w_parts.parts[1] = 600;
    input.w_parts.parts[2] = 600;
    input.h_parts.part_num = 3;
    input.h_parts.parts[0] = 320;
    input.h_parts.parts[1] = 320;
    input.h_parts.parts[2] = 320;

    if (0 != stitch_algorithm(&input, &output)) {
        return 1;
    }

    stitch_dump_output(&output);

    /* check result */
    const stitch_output_partiton_t expected[] = {
        {
            /* f0 */
            .box =
                {
                    .x = 0,
                    .y = 0,
                    .w = 640,
                    .h = 360,
                },
            .dst_w = 600,
            .dst_h = 320,
            .w_scale_step = 1092,
            .h_scale_step = 1152,
            .overlap =
                {
                    .x = 0,
                    .y = 0,
                    .w = 600,
                    .h = 320,
                },
        },
        {
            /* f1 */
            .box =
                {
                    .x = 622,
                    .y = 0,
                    .w = 658,
                    .h = 360,
                },
            .dst_w = 616,
            .dst_h = 320,
            .w_scale_step = 1092,
            .h_scale_step = 1152,
            .overlap =
                {
                    .x = 16,
                    .y = 0,
                    .w = 600,
                    .h = 320,
                },
        },
        {
            /* f2 */
            .box =
                {
                    .x = 1278,
                    .y = 0,
                    .w = 640,
                    .h = 360,
                },
            .dst_w = 600,
            .dst_h = 320,
            .w_scale_step = 1092,
            .h_scale_step = 1152,
            .overlap =
                {
                    .x = 0,
                    .y = 0,
                    .w = 600,
                    .h = 320,
                },
        },
        {
            /* f3 */
            .box =
                {
                    .x = 0,
                    .y = 360,
                    .w = 640,
                    .h = 360,
                },
            .dst_w = 600,
            .dst_h = 320,
            .w_scale_step = 1092,
            .h_scale_step = 1152,
            .overlap =
                {
                    .x = 0,
                    .y = 0,
                    .w = 600,
                    .h = 320,
                },
        },
        {
            /* f4 */
            .box =
                {
                    .x = 622,
                    .y = 360,
                    .w = 658,
                    .h = 360,
                },
            .dst_w = 616,
            .dst_h = 320,
            .w_scale_step = 1092,
            .h_scale_step = 1152,
            .overlap =
                {
                    .x = 16,
                    .y = 0,
                    .w = 600,
                    .h = 320,
                },
        },
        {
            /* f5 */
            .box =
                {
                    .x = 1278,
                    .y = 360,
                    .w = 640,
                    .h = 360,
                },
            .dst_w = 600,
            .dst_h = 320,
            .w_scale_step = 1092,
            .h_scale_step = 1152,
            .overlap =
                {
                    .x = 0,
                    .y = 0,
                    .w = 600,
                    .h = 320,
                },
        },
        {
            /* f6 */
            .box =
                {
                    .x = 0,
                    .y = 720,
                    .w = 640,
                    .h = 360,
                },
            .dst_w = 600,
            .dst_h = 320,
            .w_scale_step = 1092,
            .h_scale_step = 1152,
            .overlap =
                {
                    .x = 0,
                    .y = 0,
                    .w = 600,
                    .h = 320,
                },
        },
        {
            /* f7 */
            .box =
                {
                    .x = 622,
                    .y = 720,
                    .w = 658,
                    .h = 360,
                },
            .dst_w = 616,
            .dst_h = 320,
            .w_scale_step = 1092,
            .h_scale_step = 1152,
            .overlap =
                {
                    .x = 16,
                    .y = 0,
                    .w = 600,
                    .h = 320,
                },
        },
        {
            /* f8 */
            .box =
                {
                    .x = 1278,
                    .y = 720,
                    .w = 640,
                    .h = 360,
                },
            .dst_w = 600,
            .dst_h = 320,
            .w_scale_step = 1092,
            .h_scale_step = 1152,
            .overlap =
                {
                    .x = 0,
                    .y = 0,
                    .w = 600,
                    .h = 320,
                },
        },
    };

    for (int i = 0; i < 9; ++i) {
        if (0 != memcmp(&output.parts[i], &expected[i], sizeof(stitch_output_partiton_t))) {
            printf("compare result fail, partiton = %d\n", i);
            return -1;
        }
    }

    return 0;
}
