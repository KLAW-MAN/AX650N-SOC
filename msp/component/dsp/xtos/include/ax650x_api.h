/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#ifndef _INC_AX630X_API_H_
#define _INC_AX630X_API_H_

unsigned int ax_dsp_base_ddr_config(void);
unsigned int ax_cpu_addr_to_dsp_addr(unsigned long long addr);
unsigned long long ax_dsp_addr_to_idma_addr(unsigned int addr);
unsigned long long ax_dsp_addr_to_cpu_addr(unsigned int addr);
void ax_dsp_log_config(unsigned int uart_id);
void ax_dsp_memlog_config(unsigned int enable);
int AX_Mailbox_Init(void);
int AX_Mailbox_Read(uint32_t *data, uint32_t size);
int AX_Mailbox_Write(int mid, uint32_t *data, uint32_t size);
int AX_Mailbox_Read_Fromid(int mid, uint32_t *data, uint32_t size, int block);
int AX_Mailbox_Deinit(void);
int AX_Mailbox_Start(void);

typedef struct {
    AX_U32 bypass;
} AX_FBCDC_PARAM;

typedef struct {
    AX_U32 zero_bits_cnt;
    AX_U32 comp_level;
    AX_U32 pixel_format;
    AX_U32 tile_width;
    //below is not necessary, can be computed or fetched
    AX_U32  layer_no;
    AX_U32  start_position_h_layer;
    AX_U32  start_position_l_layer;
    AX_U32  end_position_h_layer;
    AX_U32  end_position_l_layer;
    AX_U32  hor_subimage_subtile_num_layer;
    AX_U32  hor_subtile_num_layer;
    AX_U32  subimage_subtile_start_pos_x_layer;
    AX_U32  ver_tile_num_layer;
} AX_DSP_FBCDC_PARAM;

AX_FBCDC_PARAM *AX_VDSP_FBC_GetInfo(void);
AX_FBCDC_PARAM *AX_VDSP_FBDC_GetInfo(void);
//AX_S32 AX_VDSP_CopyData_FBCDC(AX_U64 dst, AX_U64 src, AX_S32 s32Size);
//AX_S32 AX_VDSP_Copy2DAddr_FBCDC(AX_U64 dst, AX_U64 src, AX_S32 s32Size);
AX_S32 AX_VDSP_CheckFbcdc(AX_U64 dst, AX_U64 src, AX_S32 size);
void AX_VDSP_Update_Fbcdc_Para(void);
AX_S32 AX_VDSP_FBCisEnabled();
AX_S32 AX_VDSP_FBDCisEnabled();
AX_S32 AX_DSP_SetFBC(AX_DSP_FBCDC_PARAM *param);
AX_S32 AX_DSP_SetFBDC(AX_DSP_FBCDC_PARAM *param);
AX_S32 AX_DSP_DisableFBC(void);
AX_S32 AX_DSP_DisableFBDC(void);
AX_S32 AX_DSP_EnableFBC(void);
AX_S32 AX_DSP_EnableFBDC(void);

typedef enum
{
    AX_COMPRESS_MODE_NONE = 0,   /* no compress */
    AX_COMPRESS_MODE_LOSSLESS,
    AX_COMPRESS_MODE_LOSSY,
    AX_COMPRESS_MODE_BUTT
} AX_COMPRESS_MODE_E;

typedef struct axFRAME_COMPRESS_INFO_T {
    AX_COMPRESS_MODE_E enCompressMode;
    AX_U32    u32CompressLevel;
} AX_FRAME_COMPRESS_INFO_T;

typedef enum
{
    AX_FORMAT_INVALID                               = -1,

    /* YUV400 8 bit */
    AX_FORMAT_YUV400                                = 0x0,      /* Y...          */

    /* YUV420 8 bit */
    AX_FORMAT_YUV420_PLANAR                         = 0x1,      /* YYYY... UUUU... VVVV...   I420/YU12  */
    AX_FORMAT_YUV420_PLANAR_VU                      = 0x2,      /* YYYY... VVVV... UUUU...  YV12 */
    AX_FORMAT_YUV420_SEMIPLANAR                     = 0x3,      /* YYYY... UVUVUV...       NV12  */
    AX_FORMAT_YUV420_SEMIPLANAR_VU                  = 0x4,      /* YYYY... VUVUVU...      NV21  */

    /* YUV422 8 bit */
    AX_FORMAT_YUV422_PLANAR                         = 0x8,      /* YYYY... UUUU... VVVV...   I422  */
    AX_FORMAT_YUV422_PLANAR_VU                      = 0x9,      /* YYYY... VVVV... UUUU...  YV16 */
    AX_FORMAT_YUV422_SEMIPLANAR                     = 0xA,      /* YYYY... UVUVUV...       NV61  */
    AX_FORMAT_YUV422_SEMIPLANAR_VU                  = 0xB,      /* YYYY... VUVUVU...      NV16  */
    AX_FORMAT_YUV422_INTERLEAVED_YUVY               = 0xC,      /* YUVYYUVY...           YUVY   */
    AX_FORMAT_YUV422_INTERLEAVED_YUYV               = 0xD,      /* YUYVYUYV...           YUYV   */
    AX_FORMAT_YUV422_INTERLEAVED_UYVY               = 0xE,      /* UYVYUYVY...           UYVY   */
    AX_FORMAT_YUV422_INTERLEAVED_VYUY               = 0xF,      /* VYUYVYUY...           VYUY   */
    AX_FORMAT_YUV422_INTERLEAVED_YVYU               = 0x10,     /* VYUYVYUY...           YVYU   */

    /* YUV444 8 bit */
    AX_FORMAT_YUV444_PLANAR                         = 0x14,     /* YYYY... UUUU... VVVV...   I444  */
    AX_FORMAT_YUV444_PLANAR_VU                      = 0x15,     /* YYYY... VVVV... UUUU...  YV24 */
    AX_FORMAT_YUV444_SEMIPLANAR                     = 0x16,     /* YYYY... UVUVUV...       NV24  */
    AX_FORMAT_YUV444_SEMIPLANAR_VU                  = 0x17,     /* YYYY... VUVUVU...      NV42  */
    AX_FORMAT_YUV444_PACKED                         = 0x18,          /* YUV YUV YUV ...          */

    /* YUV 10 bit */
    AX_FORMAT_YUV400_10BIT                          = 0x20,
    AX_FORMAT_YUV420_PLANAR_10BIT_UV_PACKED_4Y5B    = 0x24,     /* YYYY... UUUU... VVVV... , 4 Y pixels in 5 bytes, UV packed  */
    AX_FORMAT_YUV420_PLANAR_10BIT_I010              = 0x25,     /*  16 bit pixel, low 10bits valid, high 6 bits invalid */
    AX_FORMAT_YUV420_SEMIPLANAR_10BIT_P101010       = 0x28,     /* YYYY... UVUVUV... ,  Y/U/V 4 pixels in 5 bytes  */
    AX_FORMAT_YUV420_SEMIPLANAR_10BIT_P010          = 0x2A,     /* 16 bit pixel, high 10bits valid, low 6 bits invalid */
    AX_FORMAT_YUV420_SEMIPLANAR_10BIT_P016          = 0x2C,     /* 16 bit pixel, low 10bits valid, high 6 bits invalid */
    AX_FORMAT_YUV420_SEMIPLANAR_10BIT_I016          = 0x2E,     /* 16 bit pixel, high 10bits valid, low 6 bits invalid */
    AX_FORMAT_YUV420_SEMIPLANAR_10BIT_12P16B        = 0x2F,     /* 12 pixels in 16bytes, low 120bits valid, high 8 bits invalid */
    AX_FORMAT_YUV444_PACKED_10BIT_P010              = 0x30,     /* YUV YUV YUV ... , 16 bit pixel, high 10bits valid, low 6 bits invalid  */
    AX_FORMAT_YUV444_PACKED_10BIT_P101010           = 0x32,     /* YUV YUV YUV ... , 4 pixels storage in 5 bytes */
    AX_FORMAT_YUV422_SEMIPLANAR_10BIT_P101010       = 0x33,     /* YYYY... UVUVUV... ,  Y/U/V 4 pixels in 5 bytes */
    AX_FORMAT_YUV422_SEMIPLANAR_10BIT_P010          = 0x34,     /* 16 bit pixel, high 10bits valid, low 6 bits invalid */

    /* BAYER RAW */
    AX_FORMAT_BAYER_RAW_8BPP                        = 0x80,
    AX_FORMAT_BAYER_RAW_10BPP                       = 0x81,
    AX_FORMAT_BAYER_RAW_12BPP                       = 0x82,
    AX_FORMAT_BAYER_RAW_14BPP                       = 0x83,
    AX_FORMAT_BAYER_RAW_16BPP                       = 0x84,
    AX_FORMAT_BAYER_RAW_10BPP_PACKED                = 0x85,
    AX_FORMAT_BAYER_RAW_12BPP_PACKED                = 0x86,
    AX_FORMAT_BAYER_RAW_14BPP_PACKED                = 0x87,

    /* RGB Format */
    AX_FORMAT_RGB565                                = 0xA0,      /* BGRBGR..., RGB565 16bpp */
    AX_FORMAT_RGB888                                = 0xA1,      /* BGRBGR..., RGB888 24bpp */
    AX_FORMAT_KRGB444                               = 0xA2,
    AX_FORMAT_KRGB555                               = 0xA3,
    AX_FORMAT_KRGB888                               = 0xA4,
    AX_FORMAT_BGR888                                = 0xA5,      /* RGBRGB..., BGR888 32bpp */
    AX_FORMAT_BGR565                                = 0xA6,      /* RGBRGB..., BGR565 16bpp */

    AX_FORMAT_ARGB4444                              = 0xC5,      /* BGRABGRA..., ARGB4444 16bpp */
    AX_FORMAT_ARGB1555                              = 0xC6,      /* BGRABGRA..., ARGB1555 16bpp */
    AX_FORMAT_ARGB8888                              = 0xC7,      /* BGRABGRA..., ARGB8888 32bpp */
    AX_FORMAT_ARGB8565                              = 0xC8,      /* BGRABGRA..., ARGB8565 24bpp */
    AX_FORMAT_RGBA8888                              = 0xC9,      /* ABGRABGR..., RGBA8888 32bpp */
    AX_FORMAT_RGBA5551                              = 0xCA,      /* ABGRABGR..., RGBA5551 16bpp */
    AX_FORMAT_RGBA4444                              = 0xCB,      /* ABGRABGR..., RGBA4444 16bpp */
    AX_FORMAT_RGBA5658                              = 0xCC,      /* ABGRABGR..., RGBA5658 24bpp */
    AX_FORMAT_ABGR4444                              = 0xCD,      /* RGBARGBA..., ABGR4444 16bpp */
    AX_FORMAT_ABGR1555                              = 0xCE,      /* RGBARGBA..., ABGR1555 16bpp */
    AX_FORMAT_ABGR8888                              = 0xCF,      /* RGBARGBA..., ABGR8888 32bpp */
    AX_FORMAT_ABGR8565                              = 0xD0,      /* RGBARGBA..., ABGR8565 24bpp */
    AX_FORMAT_BGRA8888                              = 0xD1,      /* ARGBARGB..., BGRA8888 32bpp */
    AX_FORMAT_BGRA5551                              = 0xD2,      /* ARGBARGB..., BGRA5551 16bpp */
    AX_FORMAT_BGRA4444                              = 0xD3,      /* ARGBARGB..., BGRA4444 16bpp */
    AX_FORMAT_BGRA5658                              = 0xD4,      /* ARGBARGB..., BGRA5658 24bpp */

    AX_FORMAT_BITMAP                                = 0xE0,

    AX_FORMAT_MAX
} AX_IMG_FORMAT_E;

extern uint32_t _procid;
#endif


