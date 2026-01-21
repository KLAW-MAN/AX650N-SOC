#pragma once

typedef enum {
    BL_DDR_PORT0 = 0,
    BL_SYSVDEC,
    BL_STSVENC,
    BL_SATA0,
    BL_DMA_XOR,
    BL_DMA0,
    BL_DMA1,
    BL_PCIE0,
    BL_PCIE1,
    BL_MM_DPU_0,
    BL_MM_DPU_1,
    BL_MM_AXI2CSITX,
    BL_MM_CDMA,
    BL_MM_VGP,
    BL_MM_IVE,
    BL_MM_PYRA,
    BL_MM_GDC,
    BL_MM_YUVWDMA,
    BL_VDSP0DMA,
    BL_VDSP1DMA,
    BL_NPU_VEC_0,
    BL_NPU_VEC_1,
    BL_NPU_VEC_2,
    BL_NPU_INTERLEAVE,
    BL_ISP_IFE_M1,
    BL_ISP_IFE_WDAM_M1,
    BL_ISP_ITP_TDP0,
    BL_ISP_ITP_TDP1,
    BL_ISP_ITP_M1,
    BL_ISP_PIPE_VPP,
    BL_ISP_YUV_M1,
    BL_NUM_MAX
} SUB_SYS_BW_LIMITERS;

int ax_bw_limiter_register_with_clk(SUB_SYS_BW_LIMITERS sub_sys_bw, void *clk);
int ax_bw_limiter_register_with_val(SUB_SYS_BW_LIMITERS sub_sys_bw, u32 work_clk);
int ax_bw_limiter_unregister(SUB_SYS_BW_LIMITERS sub_sys_bw, void *clk);
int ax_bw_limiter_refresh_limiter(SUB_SYS_BW_LIMITERS sub_sys_bw);
