#ifndef __PMU_GLB_H__
#define __PMU_GLB_H__

#include <ax650_def.h>

// REGISTER : INT_RAW

#define PMU_GLB_INT_RAW_ADDR      (PMU_GLB_BASE_ADDR + 0xC4)

// REGISTER : INT_RAW1

#define PMU_GLB_INT_RAW1_ADDR      (PMU_GLB_BASE_ADDR + 0xC8)

// REGISTER : INT_RAW2

#define PMU_GLB_INT_RAW2_ADDR      (PMU_GLB_BASE_ADDR + 0xCC)

// REGISTER : SLP_EN_SET

#define PMU_GLB_SLP_EN_SET_ADDR      (PMU_GLB_BASE_ADDR + 0xDC)
#define BIT_PMU_SLP_EN_CPU7_SET      BIT(11)
#define BIT_PMU_SLP_EN_CPU6_SET      BIT(10)
#define BIT_PMU_SLP_EN_CPU5_SET      BIT(9)
#define BIT_PMU_SLP_EN_CPU4_SET      BIT(8)
#define BIT_PMU_SLP_EN_CPU3_SET      BIT(7)
#define BIT_PMU_SLP_EN_CPU2_SET      BIT(6)
#define BIT_PMU_SLP_EN_CPU1_SET      BIT(5)
#define BIT_PMU_SLP_EN_CPU0_SET      BIT(4)
#define BIT_PMU_SLP_EN_VDSP_SET      BIT(3)
#define BIT_PMU_SLP_EN_VDEC_SET      BIT(2)
#define BIT_PMU_SLP_EN_VENC1_SET     BIT(1)
#define BIT_PMU_SLP_EN_VENC0_SET     BIT(0)

// REGISTER : SLP_EN1_SET

#define PMU_GLB_SLP_EN1_SET_ADDR      (PMU_GLB_BASE_ADDR + 0xE4)
#define BIT_PMU_SLP_EN_SEN_SET        BIT(18)
#define BIT_PMU_SLP_EN_ISP_SET        BIT(17)
#define BIT_PMU_SLP_EN_CPU_SET        BIT(16)
#define BIT_PMU_SLP_EN_NPU_SET        BIT(15)
#define BIT_PMU_SLP_EN_N8_SET         BIT(14)
#define BIT_PMU_SLP_EN_N7_SET         BIT(13)
#define BIT_PMU_SLP_EN_N6_SET         BIT(12)
#define BIT_PMU_SLP_EN_N5_SET         BIT(11)
#define BIT_PMU_SLP_EN_N4_SET         BIT(10)
#define BIT_PMU_SLP_EN_N3_SET         BIT(9)
#define BIT_PMU_SLP_EN_N2_SET         BIT(8)
#define BIT_PMU_SLP_EN_N1_SET         BIT(7)
#define BIT_PMU_SLP_EN_N0_SET         BIT(6)
#define BIT_PMU_SLP_EN_MM_SET         BIT(5)
#define BIT_PMU_SLP_EN_PERIP_SET      BIT(4)
#define BIT_PMU_SLP_EN_PIPE_SET       BIT(3)
#define BIT_PMU_SLP_EN_FLASH_SET      BIT(2)
#define BIT_PMU_SLP_EN_DDR1_SET       BIT(1)
#define BIT_PMU_SLP_EN_DDR0_SET       BIT(0)

// REGISTER : SLP_EN_CLR

#define PMU_GLB_SLP_EN_CLR_ADDR      (PMU_GLB_BASE_ADDR + 0xE0)

// REGISTER : SLP_EN1_CLR

#define PMU_GLB_SLP_EN1_CLR_ADDR      (PMU_GLB_BASE_ADDR + 0xE8)


// REGISTER : PWR_STATE0

#define PMU_GLB_PWR_STATE0_ADDR      (PMU_GLB_BASE_ADDR + 0x18)

// REGISTER : PWR_STATE1

#define PMU_GLB_PWR_STATE1_ADDR      (PMU_GLB_BASE_ADDR + 0x1C)

// REGISTER : PWR_STATE2

#define PMU_GLB_PWR_STATE2_ADDR      (PMU_GLB_BASE_ADDR + 0x20)


// REGISTER : PWR_STATE3

#define PMU_GLB_PWR_STATE3_ADDR      (PMU_GLB_BASE_ADDR + 0x24)

// REGISTER : WAKUP_SET

#define PMU_GLB_WAKUP_SET_ADDR      (PMU_GLB_BASE_ADDR + 0xEC)

// REGISTER : WAKUP_CLR

#define PMU_GLB_WAKUP_CLR_ADDR      (PMU_GLB_BASE_ADDR + 0xF0)

// REGISTER : WAKUP1_SET

#define PMU_GLB_WAKUP1_SET_ADDR      (PMU_GLB_BASE_ADDR + 0xF4)

// REGISTER : WAKUP1_CLR

#define PMU_GLB_WAKUP1_CLR_ADDR      (PMU_GLB_BASE_ADDR + 0xF8)

// REGISTER : PWR_WAIT0

#define PMU_GLB_PWR_WAIT0_ADDR      (PMU_GLB_BASE_ADDR + 0x28)

// REGISTER : PWR_WAIT1

#define PMU_GLB_PWR_WAIT1_ADDR      (PMU_GLB_BASE_ADDR + 0x2C)

// REGISTER : PWR_WAIT2

#define PMU_GLB_PWR_WAIT2_ADDR      (PMU_GLB_BASE_ADDR + 0x30)

// REGISTER : PWR_WAIT3

#define PMU_GLB_PWR_WAIT3_ADDR      (PMU_GLB_BASE_ADDR + 0x34)

// REGISTER : PWR_WAIT4

#define PMU_GLB_PWR_WAIT4_ADDR      (PMU_GLB_BASE_ADDR + 0x38)

// REGISTER : PWR_WAITON0

#define PMU_GLB_PWR_WAITON0_ADDR      (PMU_GLB_BASE_ADDR + 0x3C)

// REGISTER : PWR_WAITON1

#define PMU_GLB_PWR_WAITON1_ADDR      (PMU_GLB_BASE_ADDR + 0x40)

// REGISTER : PWR_WAITON2

#define PMU_GLB_PWR_WAITON2_ADDR      (PMU_GLB_BASE_ADDR + 0x44)

// REGISTER : PWR_WAITON3

#define PMU_GLB_PWR_WAITON3_ADDR      (PMU_GLB_BASE_ADDR + 0x48)

// REGISTER : PWR_WAITON4

#define PMU_GLB_PWR_WAITON4_ADDR      (PMU_GLB_BASE_ADDR + 0x4C)

// REGISTER : SLP_EN

#define PMU_GLB_SLP_EN_ADDR      (PMU_GLB_BASE_ADDR + 0x0)
#define PMU_GLB_SLP_EN_CPU7_POS      11
#define PMU_GLB_SLP_EN_CPU6_POS      10
#define PMU_GLB_SLP_EN_CPU5_POS      9
#define PMU_GLB_SLP_EN_CPU4_POS      8
#define PMU_GLB_SLP_EN_CPU3_POS      7
#define PMU_GLB_SLP_EN_CPU2_POS      6
#define PMU_GLB_SLP_EN_CPU1_POS      5
#define PMU_GLB_SLP_EN_CPU0_POS      4
#define PMU_GLB_SLP_EN_DSP_POS      3
#define PMU_GLB_SLP_EN_VDEC_POS      2
#define PMU_GLB_SLP_EN_VENC1_POS      1
#define PMU_GLB_SLP_EN_VENC0_POS      0

// REGISTER : SLP_EN1

#define PMU_GLB_SLP_EN1_ADDR      (PMU_GLB_BASE_ADDR + 0x4)
#define PMU_GLB_SLP_EN1_SEN_POS      18
#define PMU_GLB_SLP_EN1_ISP_POS      17
#define PMU_GLB_SLP_EN1_CPU_POS      16
#define PMU_GLB_SLP_EN1_NPU_POS      15
#define PMU_GLB_SLP_EN1_N8_POS      14
#define PMU_GLB_SLP_EN1_N7_POS      13
#define PMU_GLB_SLP_EN1_N6_POS      12
#define PMU_GLB_SLP_EN1_N5_POS      11
#define PMU_GLB_SLP_EN1_N4_POS      10
#define PMU_GLB_SLP_EN1_N3_POS      9
#define PMU_GLB_SLP_EN1_N2_POS      8
#define PMU_GLB_SLP_EN1_N1_POS      7
#define PMU_GLB_SLP_EN1_N0_POS      6
#define PMU_GLB_SLP_EN1_MM_POS      5
#define PMU_GLB_SLP_EN1_PERIP_POS      4
#define PMU_GLB_SLP_EN1_PIPE_POS      3
#define PMU_GLB_SLP_EN1_FLASH_POS      2
#define PMU_GLB_SLP_EN1_DDR1_POS      1
#define PMU_GLB_SLP_EN1_DDR0_POS      0

//========================below I may delete after==============================

// REGISTER : PWR_BYPASS

#define PMU_GLB_PWR_BYPASS_ADDR      PMU_GLB_BASE_ADDR + 0x10
#define PMU_GLB_PWR_BYPASS_RSTVAL    0x0
#define PMU_GLB_PWR_BYPASS_ALL1      0x7FFF
#define PMU_GLB_PWR_BYPASS_ALL0      0x0
#define PMU_GLB_PWR_BYPASS_ALL5A     0x5A5A
#define PMU_GLB_PWR_BYPASS_ALLA5     0x25A5

// FIELD IN REGISTER : PWR_BYPASS

#define PMU_GLB_PWR_BYPASS_ISP_POS      14
#define PMU_GLB_PWR_BYPASS_ISP_LEN      1
#define PMU_GLB_PWR_BYPASS_ISP_MASK     0x00004000
#define PMU_GLB_PWR_BYPASS_ISP_VAL(w)   (((w) & 0x00004000) >> 14)
#define PMU_GLB_PWR_BYPASS_ISP_SET(w)   w |= ( 0x1 << 14)
#define PMU_GLB_PWR_BYPASS_ISP_CLR(w)   w &= ~( 0x1 << 14)

#define PMU_GLB_PWR_BYPASS_CPU_POS      13
#define PMU_GLB_PWR_BYPASS_CPU_LEN      1
#define PMU_GLB_PWR_BYPASS_CPU_MASK     0x00002000
#define PMU_GLB_PWR_BYPASS_CPU_VAL(w)   (((w) & 0x00002000) >> 13)
#define PMU_GLB_PWR_BYPASS_CPU_SET(w)   w |= ( 0x1 << 13)
#define PMU_GLB_PWR_BYPASS_CPU_CLR(w)   w &= ~( 0x1 << 13)

#define PMU_GLB_PWR_BYPASS_NPU_POS      12
#define PMU_GLB_PWR_BYPASS_NPU_LEN      1
#define PMU_GLB_PWR_BYPASS_NPU_MASK     0x00001000
#define PMU_GLB_PWR_BYPASS_NPU_VAL(w)   (((w) & 0x00001000) >> 12)
#define PMU_GLB_PWR_BYPASS_NPU_SET(w)   w |= ( 0x1 << 12)
#define PMU_GLB_PWR_BYPASS_NPU_CLR(w)   w &= ~( 0x1 << 12)

#define PMU_GLB_PWR_BYPASS_CPU7_POS      11
#define PMU_GLB_PWR_BYPASS_CPU7_LEN      1
#define PMU_GLB_PWR_BYPASS_CPU7_MASK     0x00000800
#define PMU_GLB_PWR_BYPASS_CPU7_VAL(w)   (((w) & 0x00000800) >> 11)
#define PMU_GLB_PWR_BYPASS_CPU7_SET(w)   w |= ( 0x1 << 11)
#define PMU_GLB_PWR_BYPASS_CPU7_CLR(w)   w &= ~( 0x1 << 11)

#define PMU_GLB_PWR_BYPASS_CPU6_POS      10
#define PMU_GLB_PWR_BYPASS_CPU6_LEN      1
#define PMU_GLB_PWR_BYPASS_CPU6_MASK     0x00000400
#define PMU_GLB_PWR_BYPASS_CPU6_VAL(w)   (((w) & 0x00000400) >> 10)
#define PMU_GLB_PWR_BYPASS_CPU6_SET(w)   w |= ( 0x1 << 10)
#define PMU_GLB_PWR_BYPASS_CPU6_CLR(w)   w &= ~( 0x1 << 10)

#define PMU_GLB_PWR_BYPASS_CPU5_POS      9
#define PMU_GLB_PWR_BYPASS_CPU5_LEN      1
#define PMU_GLB_PWR_BYPASS_CPU5_MASK     0x00000200
#define PMU_GLB_PWR_BYPASS_CPU5_VAL(w)   (((w) & 0x00000200) >> 9)
#define PMU_GLB_PWR_BYPASS_CPU5_SET(w)   w |= ( 0x1 << 9)
#define PMU_GLB_PWR_BYPASS_CPU5_CLR(w)   w &= ~( 0x1 << 9)

#define PMU_GLB_PWR_BYPASS_CPU4_POS      8
#define PMU_GLB_PWR_BYPASS_CPU4_LEN      1
#define PMU_GLB_PWR_BYPASS_CPU4_MASK     0x00000100
#define PMU_GLB_PWR_BYPASS_CPU4_VAL(w)   (((w) & 0x00000100) >> 8)
#define PMU_GLB_PWR_BYPASS_CPU4_SET(w)   w |= ( 0x1 << 8)
#define PMU_GLB_PWR_BYPASS_CPU4_CLR(w)   w &= ~( 0x1 << 8)

#define PMU_GLB_PWR_BYPASS_CPU3_POS      7
#define PMU_GLB_PWR_BYPASS_CPU3_LEN      1
#define PMU_GLB_PWR_BYPASS_CPU3_MASK     0x00000080
#define PMU_GLB_PWR_BYPASS_CPU3_VAL(w)   (((w) & 0x00000080) >> 7)
#define PMU_GLB_PWR_BYPASS_CPU3_SET(w)   w |= ( 0x1 << 7)
#define PMU_GLB_PWR_BYPASS_CPU3_CLR(w)   w &= ~( 0x1 << 7)

#define PMU_GLB_PWR_BYPASS_CPU2_POS      6
#define PMU_GLB_PWR_BYPASS_CPU2_LEN      1
#define PMU_GLB_PWR_BYPASS_CPU2_MASK     0x00000040
#define PMU_GLB_PWR_BYPASS_CPU2_VAL(w)   (((w) & 0x00000040) >> 6)
#define PMU_GLB_PWR_BYPASS_CPU2_SET(w)   w |= ( 0x1 << 6)
#define PMU_GLB_PWR_BYPASS_CPU2_CLR(w)   w &= ~( 0x1 << 6)

#define PMU_GLB_PWR_BYPASS_CPU1_POS      5
#define PMU_GLB_PWR_BYPASS_CPU1_LEN      1
#define PMU_GLB_PWR_BYPASS_CPU1_MASK     0x00000020
#define PMU_GLB_PWR_BYPASS_CPU1_VAL(w)   (((w) & 0x00000020) >> 5)
#define PMU_GLB_PWR_BYPASS_CPU1_SET(w)   w |= ( 0x1 << 5)
#define PMU_GLB_PWR_BYPASS_CPU1_CLR(w)   w &= ~( 0x1 << 5)

#define PMU_GLB_PWR_BYPASS_CPU0_POS      4
#define PMU_GLB_PWR_BYPASS_CPU0_LEN      1
#define PMU_GLB_PWR_BYPASS_CPU0_MASK     0x00000010
#define PMU_GLB_PWR_BYPASS_CPU0_VAL(w)   (((w) & 0x00000010) >> 4)
#define PMU_GLB_PWR_BYPASS_CPU0_SET(w)   w |= ( 0x1 << 4)
#define PMU_GLB_PWR_BYPASS_CPU0_CLR(w)   w &= ~( 0x1 << 4)

#define PMU_GLB_PWR_BYPASS_DSP_POS      3
#define PMU_GLB_PWR_BYPASS_DSP_LEN      1
#define PMU_GLB_PWR_BYPASS_DSP_MASK     0x00000008
#define PMU_GLB_PWR_BYPASS_DSP_VAL(w)   (((w) & 0x00000008) >> 3)
#define PMU_GLB_PWR_BYPASS_DSP_SET(w)   w |= ( 0x1 << 3)
#define PMU_GLB_PWR_BYPASS_DSP_CLR(w)   w &= ~( 0x1 << 3)

#define PMU_GLB_PWR_BYPASS_VDEC_POS      2
#define PMU_GLB_PWR_BYPASS_VDEC_LEN      1
#define PMU_GLB_PWR_BYPASS_VDEC_MASK     0x00000004
#define PMU_GLB_PWR_BYPASS_VDEC_VAL(w)   (((w) & 0x00000004) >> 2)
#define PMU_GLB_PWR_BYPASS_VDEC_SET(w)   w |= ( 0x1 << 2)
#define PMU_GLB_PWR_BYPASS_VDEC_CLR(w)   w &= ~( 0x1 << 2)

#define PMU_GLB_PWR_BYPASS_VENC1_POS      1
#define PMU_GLB_PWR_BYPASS_VENC1_LEN      1
#define PMU_GLB_PWR_BYPASS_VENC1_MASK     0x00000002
#define PMU_GLB_PWR_BYPASS_VENC1_VAL(w)   (((w) & 0x00000002) >> 1)
#define PMU_GLB_PWR_BYPASS_VENC1_SET(w)   w |= ( 0x1 << 1)
#define PMU_GLB_PWR_BYPASS_VENC1_CLR(w)   w &= ~( 0x1 << 1)

#define PMU_GLB_PWR_BYPASS_VENC0_POS      0
#define PMU_GLB_PWR_BYPASS_VENC0_LEN      1
#define PMU_GLB_PWR_BYPASS_VENC0_MASK     0x00000001
#define PMU_GLB_PWR_BYPASS_VENC0_VAL(w)   (((w) & 0x00000001) >> 0)
#define PMU_GLB_PWR_BYPASS_VENC0_SET(w)   w |= ( 0x1 << 0)
#define PMU_GLB_PWR_BYPASS_VENC0_CLR(w)   w &= ~( 0x1 << 0)

// REGISTER : PWR_BYPASS1

#define PMU_GLB_PWR_BYPASS1_ADDR      PMU_GLB_BASE_ADDR + 0x14
#define PMU_GLB_PWR_BYPASS1_RSTVAL    0x0
#define PMU_GLB_PWR_BYPASS1_ALL1      0x7FF
#define PMU_GLB_PWR_BYPASS1_ALL0      0x0
#define PMU_GLB_PWR_BYPASS1_ALL5A     0x25A
#define PMU_GLB_PWR_BYPASS1_ALLA5     0x5A5

// FIELD IN REGISTER : PWR_BYPASS1

#define PMU_GLB_PWR_BYPASS1_N8_POS      10
#define PMU_GLB_PWR_BYPASS1_N8_LEN      1
#define PMU_GLB_PWR_BYPASS1_N8_MASK     0x00000400
#define PMU_GLB_PWR_BYPASS1_N8_VAL(w)   (((w) & 0x00000400) >> 10)
#define PMU_GLB_PWR_BYPASS1_N8_SET(w)   w |= ( 0x1 << 10)
#define PMU_GLB_PWR_BYPASS1_N8_CLR(w)   w &= ~( 0x1 << 10)

#define PMU_GLB_PWR_BYPASS1_N7_POS      9
#define PMU_GLB_PWR_BYPASS1_N7_LEN      1
#define PMU_GLB_PWR_BYPASS1_N7_MASK     0x00000200
#define PMU_GLB_PWR_BYPASS1_N7_VAL(w)   (((w) & 0x00000200) >> 9)
#define PMU_GLB_PWR_BYPASS1_N7_SET(w)   w |= ( 0x1 << 9)
#define PMU_GLB_PWR_BYPASS1_N7_CLR(w)   w &= ~( 0x1 << 9)

#define PMU_GLB_PWR_BYPASS1_N6_POS      8
#define PMU_GLB_PWR_BYPASS1_N6_LEN      1
#define PMU_GLB_PWR_BYPASS1_N6_MASK     0x00000100
#define PMU_GLB_PWR_BYPASS1_N6_VAL(w)   (((w) & 0x00000100) >> 8)
#define PMU_GLB_PWR_BYPASS1_N6_SET(w)   w |= ( 0x1 << 8)
#define PMU_GLB_PWR_BYPASS1_N6_CLR(w)   w &= ~( 0x1 << 8)

#define PMU_GLB_PWR_BYPASS1_N5_POS      7
#define PMU_GLB_PWR_BYPASS1_N5_LEN      1
#define PMU_GLB_PWR_BYPASS1_N5_MASK     0x00000080
#define PMU_GLB_PWR_BYPASS1_N5_VAL(w)   (((w) & 0x00000080) >> 7)
#define PMU_GLB_PWR_BYPASS1_N5_SET(w)   w |= ( 0x1 << 7)
#define PMU_GLB_PWR_BYPASS1_N5_CLR(w)   w &= ~( 0x1 << 7)

#define PMU_GLB_PWR_BYPASS1_N4_POS      6
#define PMU_GLB_PWR_BYPASS1_N4_LEN      1
#define PMU_GLB_PWR_BYPASS1_N4_MASK     0x00000040
#define PMU_GLB_PWR_BYPASS1_N4_VAL(w)   (((w) & 0x00000040) >> 6)
#define PMU_GLB_PWR_BYPASS1_N4_SET(w)   w |= ( 0x1 << 6)
#define PMU_GLB_PWR_BYPASS1_N4_CLR(w)   w &= ~( 0x1 << 6)

#define PMU_GLB_PWR_BYPASS1_N3_POS      5
#define PMU_GLB_PWR_BYPASS1_N3_LEN      1
#define PMU_GLB_PWR_BYPASS1_N3_MASK     0x00000020
#define PMU_GLB_PWR_BYPASS1_N3_VAL(w)   (((w) & 0x00000020) >> 5)
#define PMU_GLB_PWR_BYPASS1_N3_SET(w)   w |= ( 0x1 << 5)
#define PMU_GLB_PWR_BYPASS1_N3_CLR(w)   w &= ~( 0x1 << 5)

#define PMU_GLB_PWR_BYPASS1_N2_POS      4
#define PMU_GLB_PWR_BYPASS1_N2_LEN      1
#define PMU_GLB_PWR_BYPASS1_N2_MASK     0x00000010
#define PMU_GLB_PWR_BYPASS1_N2_VAL(w)   (((w) & 0x00000010) >> 4)
#define PMU_GLB_PWR_BYPASS1_N2_SET(w)   w |= ( 0x1 << 4)
#define PMU_GLB_PWR_BYPASS1_N2_CLR(w)   w &= ~( 0x1 << 4)

#define PMU_GLB_PWR_BYPASS1_N1_POS      3
#define PMU_GLB_PWR_BYPASS1_N1_LEN      1
#define PMU_GLB_PWR_BYPASS1_N1_MASK     0x00000008
#define PMU_GLB_PWR_BYPASS1_N1_VAL(w)   (((w) & 0x00000008) >> 3)
#define PMU_GLB_PWR_BYPASS1_N1_SET(w)   w |= ( 0x1 << 3)
#define PMU_GLB_PWR_BYPASS1_N1_CLR(w)   w &= ~( 0x1 << 3)

#define PMU_GLB_PWR_BYPASS1_N0_POS      2
#define PMU_GLB_PWR_BYPASS1_N0_LEN      1
#define PMU_GLB_PWR_BYPASS1_N0_MASK     0x00000004
#define PMU_GLB_PWR_BYPASS1_N0_VAL(w)   (((w) & 0x00000004) >> 2)
#define PMU_GLB_PWR_BYPASS1_N0_SET(w)   w |= ( 0x1 << 2)
#define PMU_GLB_PWR_BYPASS1_N0_CLR(w)   w &= ~( 0x1 << 2)

#define PMU_GLB_PWR_BYPASS1_MM_POS      1
#define PMU_GLB_PWR_BYPASS1_MM_LEN      1
#define PMU_GLB_PWR_BYPASS1_MM_MASK     0x00000002
#define PMU_GLB_PWR_BYPASS1_MM_VAL(w)   (((w) & 0x00000002) >> 1)
#define PMU_GLB_PWR_BYPASS1_MM_SET(w)   w |= ( 0x1 << 1)
#define PMU_GLB_PWR_BYPASS1_MM_CLR(w)   w &= ~( 0x1 << 1)

#define PMU_GLB_PWR_BYPASS1_PIPE_POS      0
#define PMU_GLB_PWR_BYPASS1_PIPE_LEN      1
#define PMU_GLB_PWR_BYPASS1_PIPE_MASK     0x00000001
#define PMU_GLB_PWR_BYPASS1_PIPE_VAL(w)   (((w) & 0x00000001) >> 0)
#define PMU_GLB_PWR_BYPASS1_PIPE_SET(w)   w |= ( 0x1 << 0)
#define PMU_GLB_PWR_BYPASS1_PIPE_CLR(w)   w &= ~( 0x1 << 0)

// REGISTER : SLP_FRC_EN

#define PMU_GLB_SLP_FRC_EN_ADDR      PMU_GLB_BASE_ADDR + 0x50
#define PMU_GLB_SLP_FRC_EN_RSTVAL    0x0
#define PMU_GLB_SLP_FRC_EN_ALL1      0xFFF
#define PMU_GLB_SLP_FRC_EN_ALL0      0x0
#define PMU_GLB_SLP_FRC_EN_ALL5A     0xA5A
#define PMU_GLB_SLP_FRC_EN_ALLA5     0x5A5

// FIELD IN REGISTER : SLP_FRC_EN

#define PMU_GLB_SLP_FRC_EN_CPU7_POS      11
#define PMU_GLB_SLP_FRC_EN_CPU7_LEN      1
#define PMU_GLB_SLP_FRC_EN_CPU7_MASK     0x00000800
#define PMU_GLB_SLP_FRC_EN_CPU7_VAL(w)   (((w) & 0x00000800) >> 11)
#define PMU_GLB_SLP_FRC_EN_CPU7_SET(w)   w |= ( 0x1 << 11)
#define PMU_GLB_SLP_FRC_EN_CPU7_CLR(w)   w &= ~( 0x1 << 11)

#define PMU_GLB_SLP_FRC_EN_CPU6_POS      10
#define PMU_GLB_SLP_FRC_EN_CPU6_LEN      1
#define PMU_GLB_SLP_FRC_EN_CPU6_MASK     0x00000400
#define PMU_GLB_SLP_FRC_EN_CPU6_VAL(w)   (((w) & 0x00000400) >> 10)
#define PMU_GLB_SLP_FRC_EN_CPU6_SET(w)   w |= ( 0x1 << 10)
#define PMU_GLB_SLP_FRC_EN_CPU6_CLR(w)   w &= ~( 0x1 << 10)

#define PMU_GLB_SLP_FRC_EN_CPU5_POS      9
#define PMU_GLB_SLP_FRC_EN_CPU5_LEN      1
#define PMU_GLB_SLP_FRC_EN_CPU5_MASK     0x00000200
#define PMU_GLB_SLP_FRC_EN_CPU5_VAL(w)   (((w) & 0x00000200) >> 9)
#define PMU_GLB_SLP_FRC_EN_CPU5_SET(w)   w |= ( 0x1 << 9)
#define PMU_GLB_SLP_FRC_EN_CPU5_CLR(w)   w &= ~( 0x1 << 9)

#define PMU_GLB_SLP_FRC_EN_CPU4_POS      8
#define PMU_GLB_SLP_FRC_EN_CPU4_LEN      1
#define PMU_GLB_SLP_FRC_EN_CPU4_MASK     0x00000100
#define PMU_GLB_SLP_FRC_EN_CPU4_VAL(w)   (((w) & 0x00000100) >> 8)
#define PMU_GLB_SLP_FRC_EN_CPU4_SET(w)   w |= ( 0x1 << 8)
#define PMU_GLB_SLP_FRC_EN_CPU4_CLR(w)   w &= ~( 0x1 << 8)

#define PMU_GLB_SLP_FRC_EN_CPU3_POS      7
#define PMU_GLB_SLP_FRC_EN_CPU3_LEN      1
#define PMU_GLB_SLP_FRC_EN_CPU3_MASK     0x00000080
#define PMU_GLB_SLP_FRC_EN_CPU3_VAL(w)   (((w) & 0x00000080) >> 7)
#define PMU_GLB_SLP_FRC_EN_CPU3_SET(w)   w |= ( 0x1 << 7)
#define PMU_GLB_SLP_FRC_EN_CPU3_CLR(w)   w &= ~( 0x1 << 7)

#define PMU_GLB_SLP_FRC_EN_CPU2_POS      6
#define PMU_GLB_SLP_FRC_EN_CPU2_LEN      1
#define PMU_GLB_SLP_FRC_EN_CPU2_MASK     0x00000040
#define PMU_GLB_SLP_FRC_EN_CPU2_VAL(w)   (((w) & 0x00000040) >> 6)
#define PMU_GLB_SLP_FRC_EN_CPU2_SET(w)   w |= ( 0x1 << 6)
#define PMU_GLB_SLP_FRC_EN_CPU2_CLR(w)   w &= ~( 0x1 << 6)

#define PMU_GLB_SLP_FRC_EN_CPU1_POS      5
#define PMU_GLB_SLP_FRC_EN_CPU1_LEN      1
#define PMU_GLB_SLP_FRC_EN_CPU1_MASK     0x00000020
#define PMU_GLB_SLP_FRC_EN_CPU1_VAL(w)   (((w) & 0x00000020) >> 5)
#define PMU_GLB_SLP_FRC_EN_CPU1_SET(w)   w |= ( 0x1 << 5)
#define PMU_GLB_SLP_FRC_EN_CPU1_CLR(w)   w &= ~( 0x1 << 5)

#define PMU_GLB_SLP_FRC_EN_CPU0_POS      4
#define PMU_GLB_SLP_FRC_EN_CPU0_LEN      1
#define PMU_GLB_SLP_FRC_EN_CPU0_MASK     0x00000010
#define PMU_GLB_SLP_FRC_EN_CPU0_VAL(w)   (((w) & 0x00000010) >> 4)
#define PMU_GLB_SLP_FRC_EN_CPU0_SET(w)   w |= ( 0x1 << 4)
#define PMU_GLB_SLP_FRC_EN_CPU0_CLR(w)   w &= ~( 0x1 << 4)

#define PMU_GLB_SLP_FRC_EN_DSP_POS      3
#define PMU_GLB_SLP_FRC_EN_DSP_LEN      1
#define PMU_GLB_SLP_FRC_EN_DSP_MASK     0x00000008
#define PMU_GLB_SLP_FRC_EN_DSP_VAL(w)   (((w) & 0x00000008) >> 3)
#define PMU_GLB_SLP_FRC_EN_DSP_SET(w)   w |= ( 0x1 << 3)
#define PMU_GLB_SLP_FRC_EN_DSP_CLR(w)   w &= ~( 0x1 << 3)

#define PMU_GLB_SLP_FRC_EN_VDEC_POS      2
#define PMU_GLB_SLP_FRC_EN_VDEC_LEN      1
#define PMU_GLB_SLP_FRC_EN_VDEC_MASK     0x00000004
#define PMU_GLB_SLP_FRC_EN_VDEC_VAL(w)   (((w) & 0x00000004) >> 2)
#define PMU_GLB_SLP_FRC_EN_VDEC_SET(w)   w |= ( 0x1 << 2)
#define PMU_GLB_SLP_FRC_EN_VDEC_CLR(w)   w &= ~( 0x1 << 2)

#define PMU_GLB_SLP_FRC_EN_VENC1_POS      1
#define PMU_GLB_SLP_FRC_EN_VENC1_LEN      1
#define PMU_GLB_SLP_FRC_EN_VENC1_MASK     0x00000002
#define PMU_GLB_SLP_FRC_EN_VENC1_VAL(w)   (((w) & 0x00000002) >> 1)
#define PMU_GLB_SLP_FRC_EN_VENC1_SET(w)   w |= ( 0x1 << 1)
#define PMU_GLB_SLP_FRC_EN_VENC1_CLR(w)   w &= ~( 0x1 << 1)

#define PMU_GLB_SLP_FRC_EN_VENC0_POS      0
#define PMU_GLB_SLP_FRC_EN_VENC0_LEN      1
#define PMU_GLB_SLP_FRC_EN_VENC0_MASK     0x00000001
#define PMU_GLB_SLP_FRC_EN_VENC0_VAL(w)   (((w) & 0x00000001) >> 0)
#define PMU_GLB_SLP_FRC_EN_VENC0_SET(w)   w |= ( 0x1 << 0)
#define PMU_GLB_SLP_FRC_EN_VENC0_CLR(w)   w &= ~( 0x1 << 0)

// REGISTER : SLP_FRC_EN1

#define PMU_GLB_SLP_FRC_EN1_ADDR      PMU_GLB_BASE_ADDR + 0x54
#define PMU_GLB_SLP_FRC_EN1_RSTVAL    0x0
#define PMU_GLB_SLP_FRC_EN1_ALL1      0x7FFFF
#define PMU_GLB_SLP_FRC_EN1_ALL0      0x0
#define PMU_GLB_SLP_FRC_EN1_ALL5A     0x25A5A
#define PMU_GLB_SLP_FRC_EN1_ALLA5     0x5A5A5

// FIELD IN REGISTER : SLP_FRC_EN1

#define PMU_GLB_SLP_FRC_EN1_SEN_POS      18
#define PMU_GLB_SLP_FRC_EN1_SEN_LEN      1
#define PMU_GLB_SLP_FRC_EN1_SEN_MASK     0x00040000
#define PMU_GLB_SLP_FRC_EN1_SEN_VAL(w)   (((w) & 0x00040000) >> 18)
#define PMU_GLB_SLP_FRC_EN1_SEN_SET(w)   w |= ( 0x1 << 18)
#define PMU_GLB_SLP_FRC_EN1_SEN_CLR(w)   w &= ~( 0x1 << 18)

#define PMU_GLB_SLP_FRC_EN1_ISP_POS      17
#define PMU_GLB_SLP_FRC_EN1_ISP_LEN      1
#define PMU_GLB_SLP_FRC_EN1_ISP_MASK     0x00020000
#define PMU_GLB_SLP_FRC_EN1_ISP_VAL(w)   (((w) & 0x00020000) >> 17)
#define PMU_GLB_SLP_FRC_EN1_ISP_SET(w)   w |= ( 0x1 << 17)
#define PMU_GLB_SLP_FRC_EN1_ISP_CLR(w)   w &= ~( 0x1 << 17)

#define PMU_GLB_SLP_FRC_EN1_CPU_POS      16
#define PMU_GLB_SLP_FRC_EN1_CPU_LEN      1
#define PMU_GLB_SLP_FRC_EN1_CPU_MASK     0x00010000
#define PMU_GLB_SLP_FRC_EN1_CPU_VAL(w)   (((w) & 0x00010000) >> 16)
#define PMU_GLB_SLP_FRC_EN1_CPU_SET(w)   w |= ( 0x1 << 16)
#define PMU_GLB_SLP_FRC_EN1_CPU_CLR(w)   w &= ~( 0x1 << 16)

#define PMU_GLB_SLP_FRC_EN1_NPU_POS      15
#define PMU_GLB_SLP_FRC_EN1_NPU_LEN      1
#define PMU_GLB_SLP_FRC_EN1_NPU_MASK     0x00008000
#define PMU_GLB_SLP_FRC_EN1_NPU_VAL(w)   (((w) & 0x00008000) >> 15)
#define PMU_GLB_SLP_FRC_EN1_NPU_SET(w)   w |= ( 0x1 << 15)
#define PMU_GLB_SLP_FRC_EN1_NPU_CLR(w)   w &= ~( 0x1 << 15)

#define PMU_GLB_SLP_FRC_EN1_N8_POS      14
#define PMU_GLB_SLP_FRC_EN1_N8_LEN      1
#define PMU_GLB_SLP_FRC_EN1_N8_MASK     0x00004000
#define PMU_GLB_SLP_FRC_EN1_N8_VAL(w)   (((w) & 0x00004000) >> 14)
#define PMU_GLB_SLP_FRC_EN1_N8_SET(w)   w |= ( 0x1 << 14)
#define PMU_GLB_SLP_FRC_EN1_N8_CLR(w)   w &= ~( 0x1 << 14)

#define PMU_GLB_SLP_FRC_EN1_N7_POS      13
#define PMU_GLB_SLP_FRC_EN1_N7_LEN      1
#define PMU_GLB_SLP_FRC_EN1_N7_MASK     0x00002000
#define PMU_GLB_SLP_FRC_EN1_N7_VAL(w)   (((w) & 0x00002000) >> 13)
#define PMU_GLB_SLP_FRC_EN1_N7_SET(w)   w |= ( 0x1 << 13)
#define PMU_GLB_SLP_FRC_EN1_N7_CLR(w)   w &= ~( 0x1 << 13)

#define PMU_GLB_SLP_FRC_EN1_N6_POS      12
#define PMU_GLB_SLP_FRC_EN1_N6_LEN      1
#define PMU_GLB_SLP_FRC_EN1_N6_MASK     0x00001000
#define PMU_GLB_SLP_FRC_EN1_N6_VAL(w)   (((w) & 0x00001000) >> 12)
#define PMU_GLB_SLP_FRC_EN1_N6_SET(w)   w |= ( 0x1 << 12)
#define PMU_GLB_SLP_FRC_EN1_N6_CLR(w)   w &= ~( 0x1 << 12)

#define PMU_GLB_SLP_FRC_EN1_N5_POS      11
#define PMU_GLB_SLP_FRC_EN1_N5_LEN      1
#define PMU_GLB_SLP_FRC_EN1_N5_MASK     0x00000800
#define PMU_GLB_SLP_FRC_EN1_N5_VAL(w)   (((w) & 0x00000800) >> 11)
#define PMU_GLB_SLP_FRC_EN1_N5_SET(w)   w |= ( 0x1 << 11)
#define PMU_GLB_SLP_FRC_EN1_N5_CLR(w)   w &= ~( 0x1 << 11)

#define PMU_GLB_SLP_FRC_EN1_N4_POS      10
#define PMU_GLB_SLP_FRC_EN1_N4_LEN      1
#define PMU_GLB_SLP_FRC_EN1_N4_MASK     0x00000400
#define PMU_GLB_SLP_FRC_EN1_N4_VAL(w)   (((w) & 0x00000400) >> 10)
#define PMU_GLB_SLP_FRC_EN1_N4_SET(w)   w |= ( 0x1 << 10)
#define PMU_GLB_SLP_FRC_EN1_N4_CLR(w)   w &= ~( 0x1 << 10)

#define PMU_GLB_SLP_FRC_EN1_N3_POS      9
#define PMU_GLB_SLP_FRC_EN1_N3_LEN      1
#define PMU_GLB_SLP_FRC_EN1_N3_MASK     0x00000200
#define PMU_GLB_SLP_FRC_EN1_N3_VAL(w)   (((w) & 0x00000200) >> 9)
#define PMU_GLB_SLP_FRC_EN1_N3_SET(w)   w |= ( 0x1 << 9)
#define PMU_GLB_SLP_FRC_EN1_N3_CLR(w)   w &= ~( 0x1 << 9)

#define PMU_GLB_SLP_FRC_EN1_N2_POS      8
#define PMU_GLB_SLP_FRC_EN1_N2_LEN      1
#define PMU_GLB_SLP_FRC_EN1_N2_MASK     0x00000100
#define PMU_GLB_SLP_FRC_EN1_N2_VAL(w)   (((w) & 0x00000100) >> 8)
#define PMU_GLB_SLP_FRC_EN1_N2_SET(w)   w |= ( 0x1 << 8)
#define PMU_GLB_SLP_FRC_EN1_N2_CLR(w)   w &= ~( 0x1 << 8)

#define PMU_GLB_SLP_FRC_EN1_N1_POS      7
#define PMU_GLB_SLP_FRC_EN1_N1_LEN      1
#define PMU_GLB_SLP_FRC_EN1_N1_MASK     0x00000080
#define PMU_GLB_SLP_FRC_EN1_N1_VAL(w)   (((w) & 0x00000080) >> 7)
#define PMU_GLB_SLP_FRC_EN1_N1_SET(w)   w |= ( 0x1 << 7)
#define PMU_GLB_SLP_FRC_EN1_N1_CLR(w)   w &= ~( 0x1 << 7)

#define PMU_GLB_SLP_FRC_EN1_N0_POS      6
#define PMU_GLB_SLP_FRC_EN1_N0_LEN      1
#define PMU_GLB_SLP_FRC_EN1_N0_MASK     0x00000040
#define PMU_GLB_SLP_FRC_EN1_N0_VAL(w)   (((w) & 0x00000040) >> 6)
#define PMU_GLB_SLP_FRC_EN1_N0_SET(w)   w |= ( 0x1 << 6)
#define PMU_GLB_SLP_FRC_EN1_N0_CLR(w)   w &= ~( 0x1 << 6)

#define PMU_GLB_SLP_FRC_EN1_MM_POS      5
#define PMU_GLB_SLP_FRC_EN1_MM_LEN      1
#define PMU_GLB_SLP_FRC_EN1_MM_MASK     0x00000020
#define PMU_GLB_SLP_FRC_EN1_MM_VAL(w)   (((w) & 0x00000020) >> 5)
#define PMU_GLB_SLP_FRC_EN1_MM_SET(w)   w |= ( 0x1 << 5)
#define PMU_GLB_SLP_FRC_EN1_MM_CLR(w)   w &= ~( 0x1 << 5)

#define PMU_GLB_SLP_FRC_EN1_PERIP_POS      4
#define PMU_GLB_SLP_FRC_EN1_PERIP_LEN      1
#define PMU_GLB_SLP_FRC_EN1_PERIP_MASK     0x00000010
#define PMU_GLB_SLP_FRC_EN1_PERIP_VAL(w)   (((w) & 0x00000010) >> 4)
#define PMU_GLB_SLP_FRC_EN1_PERIP_SET(w)   w |= ( 0x1 << 4)
#define PMU_GLB_SLP_FRC_EN1_PERIP_CLR(w)   w &= ~( 0x1 << 4)

#define PMU_GLB_SLP_FRC_EN1_PIPE_POS      3
#define PMU_GLB_SLP_FRC_EN1_PIPE_LEN      1
#define PMU_GLB_SLP_FRC_EN1_PIPE_MASK     0x00000008
#define PMU_GLB_SLP_FRC_EN1_PIPE_VAL(w)   (((w) & 0x00000008) >> 3)
#define PMU_GLB_SLP_FRC_EN1_PIPE_SET(w)   w |= ( 0x1 << 3)
#define PMU_GLB_SLP_FRC_EN1_PIPE_CLR(w)   w &= ~( 0x1 << 3)

#define PMU_GLB_SLP_FRC_EN1_FLASH_POS      2
#define PMU_GLB_SLP_FRC_EN1_FLASH_LEN      1
#define PMU_GLB_SLP_FRC_EN1_FLASH_MASK     0x00000004
#define PMU_GLB_SLP_FRC_EN1_FLASH_VAL(w)   (((w) & 0x00000004) >> 2)
#define PMU_GLB_SLP_FRC_EN1_FLASH_SET(w)   w |= ( 0x1 << 2)
#define PMU_GLB_SLP_FRC_EN1_FLASH_CLR(w)   w &= ~( 0x1 << 2)

#define PMU_GLB_SLP_FRC_EN1_DDR1_POS      1
#define PMU_GLB_SLP_FRC_EN1_DDR1_LEN      1
#define PMU_GLB_SLP_FRC_EN1_DDR1_MASK     0x00000002
#define PMU_GLB_SLP_FRC_EN1_DDR1_VAL(w)   (((w) & 0x00000002) >> 1)
#define PMU_GLB_SLP_FRC_EN1_DDR1_SET(w)   w |= ( 0x1 << 1)
#define PMU_GLB_SLP_FRC_EN1_DDR1_CLR(w)   w &= ~( 0x1 << 1)

#define PMU_GLB_SLP_FRC_EN1_DDR0_POS      0
#define PMU_GLB_SLP_FRC_EN1_DDR0_LEN      1
#define PMU_GLB_SLP_FRC_EN1_DDR0_MASK     0x00000001
#define PMU_GLB_SLP_FRC_EN1_DDR0_VAL(w)   (((w) & 0x00000001) >> 0)
#define PMU_GLB_SLP_FRC_EN1_DDR0_SET(w)   w |= ( 0x1 << 0)
#define PMU_GLB_SLP_FRC_EN1_DDR0_CLR(w)   w &= ~( 0x1 << 0)

// REGISTER : SLP_FRC_SW

#define PMU_GLB_SLP_FRC_SW_ADDR      PMU_GLB_BASE_ADDR + 0x58
#define PMU_GLB_SLP_FRC_SW_RSTVAL    0x0
#define PMU_GLB_SLP_FRC_SW_ALL1      0xFFF
#define PMU_GLB_SLP_FRC_SW_ALL0      0x0
#define PMU_GLB_SLP_FRC_SW_ALL5A     0xA5A
#define PMU_GLB_SLP_FRC_SW_ALLA5     0x5A5

// FIELD IN REGISTER : SLP_FRC_SW

#define PMU_GLB_SLP_FRC_SW_CPU7_POS      11
#define PMU_GLB_SLP_FRC_SW_CPU7_LEN      1
#define PMU_GLB_SLP_FRC_SW_CPU7_MASK     0x00000800
#define PMU_GLB_SLP_FRC_SW_CPU7_VAL(w)   (((w) & 0x00000800) >> 11)
#define PMU_GLB_SLP_FRC_SW_CPU7_SET(w)   w |= ( 0x1 << 11)
#define PMU_GLB_SLP_FRC_SW_CPU7_CLR(w)   w &= ~( 0x1 << 11)

#define PMU_GLB_SLP_FRC_SW_CPU6_POS      10
#define PMU_GLB_SLP_FRC_SW_CPU6_LEN      1
#define PMU_GLB_SLP_FRC_SW_CPU6_MASK     0x00000400
#define PMU_GLB_SLP_FRC_SW_CPU6_VAL(w)   (((w) & 0x00000400) >> 10)
#define PMU_GLB_SLP_FRC_SW_CPU6_SET(w)   w |= ( 0x1 << 10)
#define PMU_GLB_SLP_FRC_SW_CPU6_CLR(w)   w &= ~( 0x1 << 10)

#define PMU_GLB_SLP_FRC_SW_CPU5_POS      9
#define PMU_GLB_SLP_FRC_SW_CPU5_LEN      1
#define PMU_GLB_SLP_FRC_SW_CPU5_MASK     0x00000200
#define PMU_GLB_SLP_FRC_SW_CPU5_VAL(w)   (((w) & 0x00000200) >> 9)
#define PMU_GLB_SLP_FRC_SW_CPU5_SET(w)   w |= ( 0x1 << 9)
#define PMU_GLB_SLP_FRC_SW_CPU5_CLR(w)   w &= ~( 0x1 << 9)

#define PMU_GLB_SLP_FRC_SW_CPU4_POS      8
#define PMU_GLB_SLP_FRC_SW_CPU4_LEN      1
#define PMU_GLB_SLP_FRC_SW_CPU4_MASK     0x00000100
#define PMU_GLB_SLP_FRC_SW_CPU4_VAL(w)   (((w) & 0x00000100) >> 8)
#define PMU_GLB_SLP_FRC_SW_CPU4_SET(w)   w |= ( 0x1 << 8)
#define PMU_GLB_SLP_FRC_SW_CPU4_CLR(w)   w &= ~( 0x1 << 8)

#define PMU_GLB_SLP_FRC_SW_CPU3_POS      7
#define PMU_GLB_SLP_FRC_SW_CPU3_LEN      1
#define PMU_GLB_SLP_FRC_SW_CPU3_MASK     0x00000080
#define PMU_GLB_SLP_FRC_SW_CPU3_VAL(w)   (((w) & 0x00000080) >> 7)
#define PMU_GLB_SLP_FRC_SW_CPU3_SET(w)   w |= ( 0x1 << 7)
#define PMU_GLB_SLP_FRC_SW_CPU3_CLR(w)   w &= ~( 0x1 << 7)

#define PMU_GLB_SLP_FRC_SW_CPU2_POS      6
#define PMU_GLB_SLP_FRC_SW_CPU2_LEN      1
#define PMU_GLB_SLP_FRC_SW_CPU2_MASK     0x00000040
#define PMU_GLB_SLP_FRC_SW_CPU2_VAL(w)   (((w) & 0x00000040) >> 6)
#define PMU_GLB_SLP_FRC_SW_CPU2_SET(w)   w |= ( 0x1 << 6)
#define PMU_GLB_SLP_FRC_SW_CPU2_CLR(w)   w &= ~( 0x1 << 6)

#define PMU_GLB_SLP_FRC_SW_CPU1_POS      5
#define PMU_GLB_SLP_FRC_SW_CPU1_LEN      1
#define PMU_GLB_SLP_FRC_SW_CPU1_MASK     0x00000020
#define PMU_GLB_SLP_FRC_SW_CPU1_VAL(w)   (((w) & 0x00000020) >> 5)
#define PMU_GLB_SLP_FRC_SW_CPU1_SET(w)   w |= ( 0x1 << 5)
#define PMU_GLB_SLP_FRC_SW_CPU1_CLR(w)   w &= ~( 0x1 << 5)

#define PMU_GLB_SLP_FRC_SW_CPU0_POS      4
#define PMU_GLB_SLP_FRC_SW_CPU0_LEN      1
#define PMU_GLB_SLP_FRC_SW_CPU0_MASK     0x00000010
#define PMU_GLB_SLP_FRC_SW_CPU0_VAL(w)   (((w) & 0x00000010) >> 4)
#define PMU_GLB_SLP_FRC_SW_CPU0_SET(w)   w |= ( 0x1 << 4)
#define PMU_GLB_SLP_FRC_SW_CPU0_CLR(w)   w &= ~( 0x1 << 4)

#define PMU_GLB_SLP_FRC_SW_DSP_POS      3
#define PMU_GLB_SLP_FRC_SW_DSP_LEN      1
#define PMU_GLB_SLP_FRC_SW_DSP_MASK     0x00000008
#define PMU_GLB_SLP_FRC_SW_DSP_VAL(w)   (((w) & 0x00000008) >> 3)
#define PMU_GLB_SLP_FRC_SW_DSP_SET(w)   w |= ( 0x1 << 3)
#define PMU_GLB_SLP_FRC_SW_DSP_CLR(w)   w &= ~( 0x1 << 3)

#define PMU_GLB_SLP_FRC_SW_VDEC_POS      2
#define PMU_GLB_SLP_FRC_SW_VDEC_LEN      1
#define PMU_GLB_SLP_FRC_SW_VDEC_MASK     0x00000004
#define PMU_GLB_SLP_FRC_SW_VDEC_VAL(w)   (((w) & 0x00000004) >> 2)
#define PMU_GLB_SLP_FRC_SW_VDEC_SET(w)   w |= ( 0x1 << 2)
#define PMU_GLB_SLP_FRC_SW_VDEC_CLR(w)   w &= ~( 0x1 << 2)

#define PMU_GLB_SLP_FRC_SW_VENC1_POS      1
#define PMU_GLB_SLP_FRC_SW_VENC1_LEN      1
#define PMU_GLB_SLP_FRC_SW_VENC1_MASK     0x00000002
#define PMU_GLB_SLP_FRC_SW_VENC1_VAL(w)   (((w) & 0x00000002) >> 1)
#define PMU_GLB_SLP_FRC_SW_VENC1_SET(w)   w |= ( 0x1 << 1)
#define PMU_GLB_SLP_FRC_SW_VENC1_CLR(w)   w &= ~( 0x1 << 1)

#define PMU_GLB_SLP_FRC_SW_VENC0_POS      0
#define PMU_GLB_SLP_FRC_SW_VENC0_LEN      1
#define PMU_GLB_SLP_FRC_SW_VENC0_MASK     0x00000001
#define PMU_GLB_SLP_FRC_SW_VENC0_VAL(w)   (((w) & 0x00000001) >> 0)
#define PMU_GLB_SLP_FRC_SW_VENC0_SET(w)   w |= ( 0x1 << 0)
#define PMU_GLB_SLP_FRC_SW_VENC0_CLR(w)   w &= ~( 0x1 << 0)

// REGISTER : SLP_FRC_SW1

#define PMU_GLB_SLP_FRC_SW1_ADDR      PMU_GLB_BASE_ADDR + 0x5C
#define PMU_GLB_SLP_FRC_SW1_RSTVAL    0x0
#define PMU_GLB_SLP_FRC_SW1_ALL1      0x7FFFF
#define PMU_GLB_SLP_FRC_SW1_ALL0      0x0
#define PMU_GLB_SLP_FRC_SW1_ALL5A     0x25A5A
#define PMU_GLB_SLP_FRC_SW1_ALLA5     0x5A5A5

// FIELD IN REGISTER : SLP_FRC_SW1

#define PMU_GLB_SLP_FRC_SW1_SEN_POS      18
#define PMU_GLB_SLP_FRC_SW1_SEN_LEN      1
#define PMU_GLB_SLP_FRC_SW1_SEN_MASK     0x00040000
#define PMU_GLB_SLP_FRC_SW1_SEN_VAL(w)   (((w) & 0x00040000) >> 18)
#define PMU_GLB_SLP_FRC_SW1_SEN_SET(w)   w |= ( 0x1 << 18)
#define PMU_GLB_SLP_FRC_SW1_SEN_CLR(w)   w &= ~( 0x1 << 18)

#define PMU_GLB_SLP_FRC_SW1_ISP_POS      17
#define PMU_GLB_SLP_FRC_SW1_ISP_LEN      1
#define PMU_GLB_SLP_FRC_SW1_ISP_MASK     0x00020000
#define PMU_GLB_SLP_FRC_SW1_ISP_VAL(w)   (((w) & 0x00020000) >> 17)
#define PMU_GLB_SLP_FRC_SW1_ISP_SET(w)   w |= ( 0x1 << 17)
#define PMU_GLB_SLP_FRC_SW1_ISP_CLR(w)   w &= ~( 0x1 << 17)

#define PMU_GLB_SLP_FRC_SW1_CPU_POS      16
#define PMU_GLB_SLP_FRC_SW1_CPU_LEN      1
#define PMU_GLB_SLP_FRC_SW1_CPU_MASK     0x00010000
#define PMU_GLB_SLP_FRC_SW1_CPU_VAL(w)   (((w) & 0x00010000) >> 16)
#define PMU_GLB_SLP_FRC_SW1_CPU_SET(w)   w |= ( 0x1 << 16)
#define PMU_GLB_SLP_FRC_SW1_CPU_CLR(w)   w &= ~( 0x1 << 16)

#define PMU_GLB_SLP_FRC_SW1_NPU_POS      15
#define PMU_GLB_SLP_FRC_SW1_NPU_LEN      1
#define PMU_GLB_SLP_FRC_SW1_NPU_MASK     0x00008000
#define PMU_GLB_SLP_FRC_SW1_NPU_VAL(w)   (((w) & 0x00008000) >> 15)
#define PMU_GLB_SLP_FRC_SW1_NPU_SET(w)   w |= ( 0x1 << 15)
#define PMU_GLB_SLP_FRC_SW1_NPU_CLR(w)   w &= ~( 0x1 << 15)

#define PMU_GLB_SLP_FRC_SW1_N8_POS      14
#define PMU_GLB_SLP_FRC_SW1_N8_LEN      1
#define PMU_GLB_SLP_FRC_SW1_N8_MASK     0x00004000
#define PMU_GLB_SLP_FRC_SW1_N8_VAL(w)   (((w) & 0x00004000) >> 14)
#define PMU_GLB_SLP_FRC_SW1_N8_SET(w)   w |= ( 0x1 << 14)
#define PMU_GLB_SLP_FRC_SW1_N8_CLR(w)   w &= ~( 0x1 << 14)

#define PMU_GLB_SLP_FRC_SW1_N7_POS      13
#define PMU_GLB_SLP_FRC_SW1_N7_LEN      1
#define PMU_GLB_SLP_FRC_SW1_N7_MASK     0x00002000
#define PMU_GLB_SLP_FRC_SW1_N7_VAL(w)   (((w) & 0x00002000) >> 13)
#define PMU_GLB_SLP_FRC_SW1_N7_SET(w)   w |= ( 0x1 << 13)
#define PMU_GLB_SLP_FRC_SW1_N7_CLR(w)   w &= ~( 0x1 << 13)

#define PMU_GLB_SLP_FRC_SW1_N6_POS      12
#define PMU_GLB_SLP_FRC_SW1_N6_LEN      1
#define PMU_GLB_SLP_FRC_SW1_N6_MASK     0x00001000
#define PMU_GLB_SLP_FRC_SW1_N6_VAL(w)   (((w) & 0x00001000) >> 12)
#define PMU_GLB_SLP_FRC_SW1_N6_SET(w)   w |= ( 0x1 << 12)
#define PMU_GLB_SLP_FRC_SW1_N6_CLR(w)   w &= ~( 0x1 << 12)

#define PMU_GLB_SLP_FRC_SW1_N5_POS      11
#define PMU_GLB_SLP_FRC_SW1_N5_LEN      1
#define PMU_GLB_SLP_FRC_SW1_N5_MASK     0x00000800
#define PMU_GLB_SLP_FRC_SW1_N5_VAL(w)   (((w) & 0x00000800) >> 11)
#define PMU_GLB_SLP_FRC_SW1_N5_SET(w)   w |= ( 0x1 << 11)
#define PMU_GLB_SLP_FRC_SW1_N5_CLR(w)   w &= ~( 0x1 << 11)

#define PMU_GLB_SLP_FRC_SW1_N4_POS      10
#define PMU_GLB_SLP_FRC_SW1_N4_LEN      1
#define PMU_GLB_SLP_FRC_SW1_N4_MASK     0x00000400
#define PMU_GLB_SLP_FRC_SW1_N4_VAL(w)   (((w) & 0x00000400) >> 10)
#define PMU_GLB_SLP_FRC_SW1_N4_SET(w)   w |= ( 0x1 << 10)
#define PMU_GLB_SLP_FRC_SW1_N4_CLR(w)   w &= ~( 0x1 << 10)

#define PMU_GLB_SLP_FRC_SW1_N3_POS      9
#define PMU_GLB_SLP_FRC_SW1_N3_LEN      1
#define PMU_GLB_SLP_FRC_SW1_N3_MASK     0x00000200
#define PMU_GLB_SLP_FRC_SW1_N3_VAL(w)   (((w) & 0x00000200) >> 9)
#define PMU_GLB_SLP_FRC_SW1_N3_SET(w)   w |= ( 0x1 << 9)
#define PMU_GLB_SLP_FRC_SW1_N3_CLR(w)   w &= ~( 0x1 << 9)

#define PMU_GLB_SLP_FRC_SW1_N2_POS      8
#define PMU_GLB_SLP_FRC_SW1_N2_LEN      1
#define PMU_GLB_SLP_FRC_SW1_N2_MASK     0x00000100
#define PMU_GLB_SLP_FRC_SW1_N2_VAL(w)   (((w) & 0x00000100) >> 8)
#define PMU_GLB_SLP_FRC_SW1_N2_SET(w)   w |= ( 0x1 << 8)
#define PMU_GLB_SLP_FRC_SW1_N2_CLR(w)   w &= ~( 0x1 << 8)

#define PMU_GLB_SLP_FRC_SW1_N1_POS      7
#define PMU_GLB_SLP_FRC_SW1_N1_LEN      1
#define PMU_GLB_SLP_FRC_SW1_N1_MASK     0x00000080
#define PMU_GLB_SLP_FRC_SW1_N1_VAL(w)   (((w) & 0x00000080) >> 7)
#define PMU_GLB_SLP_FRC_SW1_N1_SET(w)   w |= ( 0x1 << 7)
#define PMU_GLB_SLP_FRC_SW1_N1_CLR(w)   w &= ~( 0x1 << 7)

#define PMU_GLB_SLP_FRC_SW1_N0_POS      6
#define PMU_GLB_SLP_FRC_SW1_N0_LEN      1
#define PMU_GLB_SLP_FRC_SW1_N0_MASK     0x00000040
#define PMU_GLB_SLP_FRC_SW1_N0_VAL(w)   (((w) & 0x00000040) >> 6)
#define PMU_GLB_SLP_FRC_SW1_N0_SET(w)   w |= ( 0x1 << 6)
#define PMU_GLB_SLP_FRC_SW1_N0_CLR(w)   w &= ~( 0x1 << 6)

#define PMU_GLB_SLP_FRC_SW1_MM_POS      5
#define PMU_GLB_SLP_FRC_SW1_MM_LEN      1
#define PMU_GLB_SLP_FRC_SW1_MM_MASK     0x00000020
#define PMU_GLB_SLP_FRC_SW1_MM_VAL(w)   (((w) & 0x00000020) >> 5)
#define PMU_GLB_SLP_FRC_SW1_MM_SET(w)   w |= ( 0x1 << 5)
#define PMU_GLB_SLP_FRC_SW1_MM_CLR(w)   w &= ~( 0x1 << 5)

#define PMU_GLB_SLP_FRC_SW1_PERIP_POS      4
#define PMU_GLB_SLP_FRC_SW1_PERIP_LEN      1
#define PMU_GLB_SLP_FRC_SW1_PERIP_MASK     0x00000010
#define PMU_GLB_SLP_FRC_SW1_PERIP_VAL(w)   (((w) & 0x00000010) >> 4)
#define PMU_GLB_SLP_FRC_SW1_PERIP_SET(w)   w |= ( 0x1 << 4)
#define PMU_GLB_SLP_FRC_SW1_PERIP_CLR(w)   w &= ~( 0x1 << 4)

#define PMU_GLB_SLP_FRC_SW1_PIPE_POS      3
#define PMU_GLB_SLP_FRC_SW1_PIPE_LEN      1
#define PMU_GLB_SLP_FRC_SW1_PIPE_MASK     0x00000008
#define PMU_GLB_SLP_FRC_SW1_PIPE_VAL(w)   (((w) & 0x00000008) >> 3)
#define PMU_GLB_SLP_FRC_SW1_PIPE_SET(w)   w |= ( 0x1 << 3)
#define PMU_GLB_SLP_FRC_SW1_PIPE_CLR(w)   w &= ~( 0x1 << 3)

#define PMU_GLB_SLP_FRC_SW1_FLASH_POS      2
#define PMU_GLB_SLP_FRC_SW1_FLASH_LEN      1
#define PMU_GLB_SLP_FRC_SW1_FLASH_MASK     0x00000004
#define PMU_GLB_SLP_FRC_SW1_FLASH_VAL(w)   (((w) & 0x00000004) >> 2)
#define PMU_GLB_SLP_FRC_SW1_FLASH_SET(w)   w |= ( 0x1 << 2)
#define PMU_GLB_SLP_FRC_SW1_FLASH_CLR(w)   w &= ~( 0x1 << 2)

#define PMU_GLB_SLP_FRC_SW1_DDR1_POS      1
#define PMU_GLB_SLP_FRC_SW1_DDR1_LEN      1
#define PMU_GLB_SLP_FRC_SW1_DDR1_MASK     0x00000002
#define PMU_GLB_SLP_FRC_SW1_DDR1_VAL(w)   (((w) & 0x00000002) >> 1)
#define PMU_GLB_SLP_FRC_SW1_DDR1_SET(w)   w |= ( 0x1 << 1)
#define PMU_GLB_SLP_FRC_SW1_DDR1_CLR(w)   w &= ~( 0x1 << 1)

#define PMU_GLB_SLP_FRC_SW1_DDR0_POS      0
#define PMU_GLB_SLP_FRC_SW1_DDR0_LEN      1
#define PMU_GLB_SLP_FRC_SW1_DDR0_MASK     0x00000001
#define PMU_GLB_SLP_FRC_SW1_DDR0_VAL(w)   (((w) & 0x00000001) >> 0)
#define PMU_GLB_SLP_FRC_SW1_DDR0_SET(w)   w |= ( 0x1 << 0)
#define PMU_GLB_SLP_FRC_SW1_DDR0_CLR(w)   w &= ~( 0x1 << 0)

// REGISTER : PWROFF_FRC_EN

#define PMU_GLB_PWROFF_FRC_EN_ADDR      PMU_GLB_BASE_ADDR + 0x60
#define PMU_GLB_PWROFF_FRC_EN_RSTVAL    0x0
#define PMU_GLB_PWROFF_FRC_EN_ALL1      0x7FFF
#define PMU_GLB_PWROFF_FRC_EN_ALL0      0x0
#define PMU_GLB_PWROFF_FRC_EN_ALL5A     0x5A5A
#define PMU_GLB_PWROFF_FRC_EN_ALLA5     0x25A5

// FIELD IN REGISTER : PWROFF_FRC_EN

#define PMU_GLB_PWROFF_FRC_EN_ISP_POS      14
#define PMU_GLB_PWROFF_FRC_EN_ISP_LEN      1
#define PMU_GLB_PWROFF_FRC_EN_ISP_MASK     0x00004000
#define PMU_GLB_PWROFF_FRC_EN_ISP_VAL(w)   (((w) & 0x00004000) >> 14)
#define PMU_GLB_PWROFF_FRC_EN_ISP_SET(w)   w |= ( 0x1 << 14)
#define PMU_GLB_PWROFF_FRC_EN_ISP_CLR(w)   w &= ~( 0x1 << 14)

#define PMU_GLB_PWROFF_FRC_EN_CPU_POS      13
#define PMU_GLB_PWROFF_FRC_EN_CPU_LEN      1
#define PMU_GLB_PWROFF_FRC_EN_CPU_MASK     0x00002000
#define PMU_GLB_PWROFF_FRC_EN_CPU_VAL(w)   (((w) & 0x00002000) >> 13)
#define PMU_GLB_PWROFF_FRC_EN_CPU_SET(w)   w |= ( 0x1 << 13)
#define PMU_GLB_PWROFF_FRC_EN_CPU_CLR(w)   w &= ~( 0x1 << 13)

#define PMU_GLB_PWROFF_FRC_EN_NPU_POS      12
#define PMU_GLB_PWROFF_FRC_EN_NPU_LEN      1
#define PMU_GLB_PWROFF_FRC_EN_NPU_MASK     0x00001000
#define PMU_GLB_PWROFF_FRC_EN_NPU_VAL(w)   (((w) & 0x00001000) >> 12)
#define PMU_GLB_PWROFF_FRC_EN_NPU_SET(w)   w |= ( 0x1 << 12)
#define PMU_GLB_PWROFF_FRC_EN_NPU_CLR(w)   w &= ~( 0x1 << 12)

#define PMU_GLB_PWROFF_FRC_EN_CPU7_POS      11
#define PMU_GLB_PWROFF_FRC_EN_CPU7_LEN      1
#define PMU_GLB_PWROFF_FRC_EN_CPU7_MASK     0x00000800
#define PMU_GLB_PWROFF_FRC_EN_CPU7_VAL(w)   (((w) & 0x00000800) >> 11)
#define PMU_GLB_PWROFF_FRC_EN_CPU7_SET(w)   w |= ( 0x1 << 11)
#define PMU_GLB_PWROFF_FRC_EN_CPU7_CLR(w)   w &= ~( 0x1 << 11)

#define PMU_GLB_PWROFF_FRC_EN_CPU6_POS      10
#define PMU_GLB_PWROFF_FRC_EN_CPU6_LEN      1
#define PMU_GLB_PWROFF_FRC_EN_CPU6_MASK     0x00000400
#define PMU_GLB_PWROFF_FRC_EN_CPU6_VAL(w)   (((w) & 0x00000400) >> 10)
#define PMU_GLB_PWROFF_FRC_EN_CPU6_SET(w)   w |= ( 0x1 << 10)
#define PMU_GLB_PWROFF_FRC_EN_CPU6_CLR(w)   w &= ~( 0x1 << 10)

#define PMU_GLB_PWROFF_FRC_EN_CPU5_POS      9
#define PMU_GLB_PWROFF_FRC_EN_CPU5_LEN      1
#define PMU_GLB_PWROFF_FRC_EN_CPU5_MASK     0x00000200
#define PMU_GLB_PWROFF_FRC_EN_CPU5_VAL(w)   (((w) & 0x00000200) >> 9)
#define PMU_GLB_PWROFF_FRC_EN_CPU5_SET(w)   w |= ( 0x1 << 9)
#define PMU_GLB_PWROFF_FRC_EN_CPU5_CLR(w)   w &= ~( 0x1 << 9)

#define PMU_GLB_PWROFF_FRC_EN_CPU4_POS      8
#define PMU_GLB_PWROFF_FRC_EN_CPU4_LEN      1
#define PMU_GLB_PWROFF_FRC_EN_CPU4_MASK     0x00000100
#define PMU_GLB_PWROFF_FRC_EN_CPU4_VAL(w)   (((w) & 0x00000100) >> 8)
#define PMU_GLB_PWROFF_FRC_EN_CPU4_SET(w)   w |= ( 0x1 << 8)
#define PMU_GLB_PWROFF_FRC_EN_CPU4_CLR(w)   w &= ~( 0x1 << 8)

#define PMU_GLB_PWROFF_FRC_EN_CPU3_POS      7
#define PMU_GLB_PWROFF_FRC_EN_CPU3_LEN      1
#define PMU_GLB_PWROFF_FRC_EN_CPU3_MASK     0x00000080
#define PMU_GLB_PWROFF_FRC_EN_CPU3_VAL(w)   (((w) & 0x00000080) >> 7)
#define PMU_GLB_PWROFF_FRC_EN_CPU3_SET(w)   w |= ( 0x1 << 7)
#define PMU_GLB_PWROFF_FRC_EN_CPU3_CLR(w)   w &= ~( 0x1 << 7)

#define PMU_GLB_PWROFF_FRC_EN_CPU2_POS      6
#define PMU_GLB_PWROFF_FRC_EN_CPU2_LEN      1
#define PMU_GLB_PWROFF_FRC_EN_CPU2_MASK     0x00000040
#define PMU_GLB_PWROFF_FRC_EN_CPU2_VAL(w)   (((w) & 0x00000040) >> 6)
#define PMU_GLB_PWROFF_FRC_EN_CPU2_SET(w)   w |= ( 0x1 << 6)
#define PMU_GLB_PWROFF_FRC_EN_CPU2_CLR(w)   w &= ~( 0x1 << 6)

#define PMU_GLB_PWROFF_FRC_EN_CPU1_POS      5
#define PMU_GLB_PWROFF_FRC_EN_CPU1_LEN      1
#define PMU_GLB_PWROFF_FRC_EN_CPU1_MASK     0x00000020
#define PMU_GLB_PWROFF_FRC_EN_CPU1_VAL(w)   (((w) & 0x00000020) >> 5)
#define PMU_GLB_PWROFF_FRC_EN_CPU1_SET(w)   w |= ( 0x1 << 5)
#define PMU_GLB_PWROFF_FRC_EN_CPU1_CLR(w)   w &= ~( 0x1 << 5)

#define PMU_GLB_PWROFF_FRC_EN_CPU0_POS      4
#define PMU_GLB_PWROFF_FRC_EN_CPU0_LEN      1
#define PMU_GLB_PWROFF_FRC_EN_CPU0_MASK     0x00000010
#define PMU_GLB_PWROFF_FRC_EN_CPU0_VAL(w)   (((w) & 0x00000010) >> 4)
#define PMU_GLB_PWROFF_FRC_EN_CPU0_SET(w)   w |= ( 0x1 << 4)
#define PMU_GLB_PWROFF_FRC_EN_CPU0_CLR(w)   w &= ~( 0x1 << 4)

#define PMU_GLB_PWROFF_FRC_EN_DSP_POS      3
#define PMU_GLB_PWROFF_FRC_EN_DSP_LEN      1
#define PMU_GLB_PWROFF_FRC_EN_DSP_MASK     0x00000008
#define PMU_GLB_PWROFF_FRC_EN_DSP_VAL(w)   (((w) & 0x00000008) >> 3)
#define PMU_GLB_PWROFF_FRC_EN_DSP_SET(w)   w |= ( 0x1 << 3)
#define PMU_GLB_PWROFF_FRC_EN_DSP_CLR(w)   w &= ~( 0x1 << 3)

#define PMU_GLB_PWROFF_FRC_EN_VDEC_POS      2
#define PMU_GLB_PWROFF_FRC_EN_VDEC_LEN      1
#define PMU_GLB_PWROFF_FRC_EN_VDEC_MASK     0x00000004
#define PMU_GLB_PWROFF_FRC_EN_VDEC_VAL(w)   (((w) & 0x00000004) >> 2)
#define PMU_GLB_PWROFF_FRC_EN_VDEC_SET(w)   w |= ( 0x1 << 2)
#define PMU_GLB_PWROFF_FRC_EN_VDEC_CLR(w)   w &= ~( 0x1 << 2)

#define PMU_GLB_PWROFF_FRC_EN_VENC1_POS      1
#define PMU_GLB_PWROFF_FRC_EN_VENC1_LEN      1
#define PMU_GLB_PWROFF_FRC_EN_VENC1_MASK     0x00000002
#define PMU_GLB_PWROFF_FRC_EN_VENC1_VAL(w)   (((w) & 0x00000002) >> 1)
#define PMU_GLB_PWROFF_FRC_EN_VENC1_SET(w)   w |= ( 0x1 << 1)
#define PMU_GLB_PWROFF_FRC_EN_VENC1_CLR(w)   w &= ~( 0x1 << 1)

#define PMU_GLB_PWROFF_FRC_EN_VENC0_POS      0
#define PMU_GLB_PWROFF_FRC_EN_VENC0_LEN      1
#define PMU_GLB_PWROFF_FRC_EN_VENC0_MASK     0x00000001
#define PMU_GLB_PWROFF_FRC_EN_VENC0_VAL(w)   (((w) & 0x00000001) >> 0)
#define PMU_GLB_PWROFF_FRC_EN_VENC0_SET(w)   w |= ( 0x1 << 0)
#define PMU_GLB_PWROFF_FRC_EN_VENC0_CLR(w)   w &= ~( 0x1 << 0)

// REGISTER : PWROFF_FRC_EN1

#define PMU_GLB_PWROFF_FRC_EN1_ADDR      PMU_GLB_BASE_ADDR + 0x64
#define PMU_GLB_PWROFF_FRC_EN1_RSTVAL    0x0
#define PMU_GLB_PWROFF_FRC_EN1_ALL1      0x7FF
#define PMU_GLB_PWROFF_FRC_EN1_ALL0      0x0
#define PMU_GLB_PWROFF_FRC_EN1_ALL5A     0x25A
#define PMU_GLB_PWROFF_FRC_EN1_ALLA5     0x5A5

// FIELD IN REGISTER : PWROFF_FRC_EN1

#define PMU_GLB_PWROFF_FRC_EN1_N8_POS      10
#define PMU_GLB_PWROFF_FRC_EN1_N8_LEN      1
#define PMU_GLB_PWROFF_FRC_EN1_N8_MASK     0x00000400
#define PMU_GLB_PWROFF_FRC_EN1_N8_VAL(w)   (((w) & 0x00000400) >> 10)
#define PMU_GLB_PWROFF_FRC_EN1_N8_SET(w)   w |= ( 0x1 << 10)
#define PMU_GLB_PWROFF_FRC_EN1_N8_CLR(w)   w &= ~( 0x1 << 10)

#define PMU_GLB_PWROFF_FRC_EN1_N7_POS      9
#define PMU_GLB_PWROFF_FRC_EN1_N7_LEN      1
#define PMU_GLB_PWROFF_FRC_EN1_N7_MASK     0x00000200
#define PMU_GLB_PWROFF_FRC_EN1_N7_VAL(w)   (((w) & 0x00000200) >> 9)
#define PMU_GLB_PWROFF_FRC_EN1_N7_SET(w)   w |= ( 0x1 << 9)
#define PMU_GLB_PWROFF_FRC_EN1_N7_CLR(w)   w &= ~( 0x1 << 9)

#define PMU_GLB_PWROFF_FRC_EN1_N6_POS      8
#define PMU_GLB_PWROFF_FRC_EN1_N6_LEN      1
#define PMU_GLB_PWROFF_FRC_EN1_N6_MASK     0x00000100
#define PMU_GLB_PWROFF_FRC_EN1_N6_VAL(w)   (((w) & 0x00000100) >> 8)
#define PMU_GLB_PWROFF_FRC_EN1_N6_SET(w)   w |= ( 0x1 << 8)
#define PMU_GLB_PWROFF_FRC_EN1_N6_CLR(w)   w &= ~( 0x1 << 8)

#define PMU_GLB_PWROFF_FRC_EN1_N5_POS      7
#define PMU_GLB_PWROFF_FRC_EN1_N5_LEN      1
#define PMU_GLB_PWROFF_FRC_EN1_N5_MASK     0x00000080
#define PMU_GLB_PWROFF_FRC_EN1_N5_VAL(w)   (((w) & 0x00000080) >> 7)
#define PMU_GLB_PWROFF_FRC_EN1_N5_SET(w)   w |= ( 0x1 << 7)
#define PMU_GLB_PWROFF_FRC_EN1_N5_CLR(w)   w &= ~( 0x1 << 7)

#define PMU_GLB_PWROFF_FRC_EN1_N4_POS      6
#define PMU_GLB_PWROFF_FRC_EN1_N4_LEN      1
#define PMU_GLB_PWROFF_FRC_EN1_N4_MASK     0x00000040
#define PMU_GLB_PWROFF_FRC_EN1_N4_VAL(w)   (((w) & 0x00000040) >> 6)
#define PMU_GLB_PWROFF_FRC_EN1_N4_SET(w)   w |= ( 0x1 << 6)
#define PMU_GLB_PWROFF_FRC_EN1_N4_CLR(w)   w &= ~( 0x1 << 6)

#define PMU_GLB_PWROFF_FRC_EN1_N3_POS      5
#define PMU_GLB_PWROFF_FRC_EN1_N3_LEN      1
#define PMU_GLB_PWROFF_FRC_EN1_N3_MASK     0x00000020
#define PMU_GLB_PWROFF_FRC_EN1_N3_VAL(w)   (((w) & 0x00000020) >> 5)
#define PMU_GLB_PWROFF_FRC_EN1_N3_SET(w)   w |= ( 0x1 << 5)
#define PMU_GLB_PWROFF_FRC_EN1_N3_CLR(w)   w &= ~( 0x1 << 5)

#define PMU_GLB_PWROFF_FRC_EN1_N2_POS      4
#define PMU_GLB_PWROFF_FRC_EN1_N2_LEN      1
#define PMU_GLB_PWROFF_FRC_EN1_N2_MASK     0x00000010
#define PMU_GLB_PWROFF_FRC_EN1_N2_VAL(w)   (((w) & 0x00000010) >> 4)
#define PMU_GLB_PWROFF_FRC_EN1_N2_SET(w)   w |= ( 0x1 << 4)
#define PMU_GLB_PWROFF_FRC_EN1_N2_CLR(w)   w &= ~( 0x1 << 4)

#define PMU_GLB_PWROFF_FRC_EN1_N1_POS      3
#define PMU_GLB_PWROFF_FRC_EN1_N1_LEN      1
#define PMU_GLB_PWROFF_FRC_EN1_N1_MASK     0x00000008
#define PMU_GLB_PWROFF_FRC_EN1_N1_VAL(w)   (((w) & 0x00000008) >> 3)
#define PMU_GLB_PWROFF_FRC_EN1_N1_SET(w)   w |= ( 0x1 << 3)
#define PMU_GLB_PWROFF_FRC_EN1_N1_CLR(w)   w &= ~( 0x1 << 3)

#define PMU_GLB_PWROFF_FRC_EN1_N0_POS      2
#define PMU_GLB_PWROFF_FRC_EN1_N0_LEN      1
#define PMU_GLB_PWROFF_FRC_EN1_N0_MASK     0x00000004
#define PMU_GLB_PWROFF_FRC_EN1_N0_VAL(w)   (((w) & 0x00000004) >> 2)
#define PMU_GLB_PWROFF_FRC_EN1_N0_SET(w)   w |= ( 0x1 << 2)
#define PMU_GLB_PWROFF_FRC_EN1_N0_CLR(w)   w &= ~( 0x1 << 2)

#define PMU_GLB_PWROFF_FRC_EN1_MM_POS      1
#define PMU_GLB_PWROFF_FRC_EN1_MM_LEN      1
#define PMU_GLB_PWROFF_FRC_EN1_MM_MASK     0x00000002
#define PMU_GLB_PWROFF_FRC_EN1_MM_VAL(w)   (((w) & 0x00000002) >> 1)
#define PMU_GLB_PWROFF_FRC_EN1_MM_SET(w)   w |= ( 0x1 << 1)
#define PMU_GLB_PWROFF_FRC_EN1_MM_CLR(w)   w &= ~( 0x1 << 1)

#define PMU_GLB_PWROFF_FRC_EN1_PIPE_POS      0
#define PMU_GLB_PWROFF_FRC_EN1_PIPE_LEN      1
#define PMU_GLB_PWROFF_FRC_EN1_PIPE_MASK     0x00000001
#define PMU_GLB_PWROFF_FRC_EN1_PIPE_VAL(w)   (((w) & 0x00000001) >> 0)
#define PMU_GLB_PWROFF_FRC_EN1_PIPE_SET(w)   w |= ( 0x1 << 0)
#define PMU_GLB_PWROFF_FRC_EN1_PIPE_CLR(w)   w &= ~( 0x1 << 0)

// REGISTER : PWROFF_FRC_SW

#define PMU_GLB_PWROFF_FRC_SW_ADDR      PMU_GLB_BASE_ADDR + 0x68
#define PMU_GLB_PWROFF_FRC_SW_RSTVAL    0x0
#define PMU_GLB_PWROFF_FRC_SW_ALL1      0x7FFF
#define PMU_GLB_PWROFF_FRC_SW_ALL0      0x0
#define PMU_GLB_PWROFF_FRC_SW_ALL5A     0x5A5A
#define PMU_GLB_PWROFF_FRC_SW_ALLA5     0x25A5

// FIELD IN REGISTER : PWROFF_FRC_SW

#define PMU_GLB_PWROFF_FRC_SW_ISP_POS      14
#define PMU_GLB_PWROFF_FRC_SW_ISP_LEN      1
#define PMU_GLB_PWROFF_FRC_SW_ISP_MASK     0x00004000
#define PMU_GLB_PWROFF_FRC_SW_ISP_VAL(w)   (((w) & 0x00004000) >> 14)
#define PMU_GLB_PWROFF_FRC_SW_ISP_SET(w)   w |= ( 0x1 << 14)
#define PMU_GLB_PWROFF_FRC_SW_ISP_CLR(w)   w &= ~( 0x1 << 14)

#define PMU_GLB_PWROFF_FRC_SW_CPU_POS      13
#define PMU_GLB_PWROFF_FRC_SW_CPU_LEN      1
#define PMU_GLB_PWROFF_FRC_SW_CPU_MASK     0x00002000
#define PMU_GLB_PWROFF_FRC_SW_CPU_VAL(w)   (((w) & 0x00002000) >> 13)
#define PMU_GLB_PWROFF_FRC_SW_CPU_SET(w)   w |= ( 0x1 << 13)
#define PMU_GLB_PWROFF_FRC_SW_CPU_CLR(w)   w &= ~( 0x1 << 13)

#define PMU_GLB_PWROFF_FRC_SW_NPU_POS      12
#define PMU_GLB_PWROFF_FRC_SW_NPU_LEN      1
#define PMU_GLB_PWROFF_FRC_SW_NPU_MASK     0x00001000
#define PMU_GLB_PWROFF_FRC_SW_NPU_VAL(w)   (((w) & 0x00001000) >> 12)
#define PMU_GLB_PWROFF_FRC_SW_NPU_SET(w)   w |= ( 0x1 << 12)
#define PMU_GLB_PWROFF_FRC_SW_NPU_CLR(w)   w &= ~( 0x1 << 12)

#define PMU_GLB_PWROFF_FRC_SW_CPU7_POS      11
#define PMU_GLB_PWROFF_FRC_SW_CPU7_LEN      1
#define PMU_GLB_PWROFF_FRC_SW_CPU7_MASK     0x00000800
#define PMU_GLB_PWROFF_FRC_SW_CPU7_VAL(w)   (((w) & 0x00000800) >> 11)
#define PMU_GLB_PWROFF_FRC_SW_CPU7_SET(w)   w |= ( 0x1 << 11)
#define PMU_GLB_PWROFF_FRC_SW_CPU7_CLR(w)   w &= ~( 0x1 << 11)

#define PMU_GLB_PWROFF_FRC_SW_CPU6_POS      10
#define PMU_GLB_PWROFF_FRC_SW_CPU6_LEN      1
#define PMU_GLB_PWROFF_FRC_SW_CPU6_MASK     0x00000400
#define PMU_GLB_PWROFF_FRC_SW_CPU6_VAL(w)   (((w) & 0x00000400) >> 10)
#define PMU_GLB_PWROFF_FRC_SW_CPU6_SET(w)   w |= ( 0x1 << 10)
#define PMU_GLB_PWROFF_FRC_SW_CPU6_CLR(w)   w &= ~( 0x1 << 10)

#define PMU_GLB_PWROFF_FRC_SW_CPU5_POS      9
#define PMU_GLB_PWROFF_FRC_SW_CPU5_LEN      1
#define PMU_GLB_PWROFF_FRC_SW_CPU5_MASK     0x00000200
#define PMU_GLB_PWROFF_FRC_SW_CPU5_VAL(w)   (((w) & 0x00000200) >> 9)
#define PMU_GLB_PWROFF_FRC_SW_CPU5_SET(w)   w |= ( 0x1 << 9)
#define PMU_GLB_PWROFF_FRC_SW_CPU5_CLR(w)   w &= ~( 0x1 << 9)

#define PMU_GLB_PWROFF_FRC_SW_CPU4_POS      8
#define PMU_GLB_PWROFF_FRC_SW_CPU4_LEN      1
#define PMU_GLB_PWROFF_FRC_SW_CPU4_MASK     0x00000100
#define PMU_GLB_PWROFF_FRC_SW_CPU4_VAL(w)   (((w) & 0x00000100) >> 8)
#define PMU_GLB_PWROFF_FRC_SW_CPU4_SET(w)   w |= ( 0x1 << 8)
#define PMU_GLB_PWROFF_FRC_SW_CPU4_CLR(w)   w &= ~( 0x1 << 8)

#define PMU_GLB_PWROFF_FRC_SW_CPU3_POS      7
#define PMU_GLB_PWROFF_FRC_SW_CPU3_LEN      1
#define PMU_GLB_PWROFF_FRC_SW_CPU3_MASK     0x00000080
#define PMU_GLB_PWROFF_FRC_SW_CPU3_VAL(w)   (((w) & 0x00000080) >> 7)
#define PMU_GLB_PWROFF_FRC_SW_CPU3_SET(w)   w |= ( 0x1 << 7)
#define PMU_GLB_PWROFF_FRC_SW_CPU3_CLR(w)   w &= ~( 0x1 << 7)

#define PMU_GLB_PWROFF_FRC_SW_CPU2_POS      6
#define PMU_GLB_PWROFF_FRC_SW_CPU2_LEN      1
#define PMU_GLB_PWROFF_FRC_SW_CPU2_MASK     0x00000040
#define PMU_GLB_PWROFF_FRC_SW_CPU2_VAL(w)   (((w) & 0x00000040) >> 6)
#define PMU_GLB_PWROFF_FRC_SW_CPU2_SET(w)   w |= ( 0x1 << 6)
#define PMU_GLB_PWROFF_FRC_SW_CPU2_CLR(w)   w &= ~( 0x1 << 6)

#define PMU_GLB_PWROFF_FRC_SW_CPU1_POS      5
#define PMU_GLB_PWROFF_FRC_SW_CPU1_LEN      1
#define PMU_GLB_PWROFF_FRC_SW_CPU1_MASK     0x00000020
#define PMU_GLB_PWROFF_FRC_SW_CPU1_VAL(w)   (((w) & 0x00000020) >> 5)
#define PMU_GLB_PWROFF_FRC_SW_CPU1_SET(w)   w |= ( 0x1 << 5)
#define PMU_GLB_PWROFF_FRC_SW_CPU1_CLR(w)   w &= ~( 0x1 << 5)

#define PMU_GLB_PWROFF_FRC_SW_CPU0_POS      4
#define PMU_GLB_PWROFF_FRC_SW_CPU0_LEN      1
#define PMU_GLB_PWROFF_FRC_SW_CPU0_MASK     0x00000010
#define PMU_GLB_PWROFF_FRC_SW_CPU0_VAL(w)   (((w) & 0x00000010) >> 4)
#define PMU_GLB_PWROFF_FRC_SW_CPU0_SET(w)   w |= ( 0x1 << 4)
#define PMU_GLB_PWROFF_FRC_SW_CPU0_CLR(w)   w &= ~( 0x1 << 4)

#define PMU_GLB_PWROFF_FRC_SW_DSP_POS      3
#define PMU_GLB_PWROFF_FRC_SW_DSP_LEN      1
#define PMU_GLB_PWROFF_FRC_SW_DSP_MASK     0x00000008
#define PMU_GLB_PWROFF_FRC_SW_DSP_VAL(w)   (((w) & 0x00000008) >> 3)
#define PMU_GLB_PWROFF_FRC_SW_DSP_SET(w)   w |= ( 0x1 << 3)
#define PMU_GLB_PWROFF_FRC_SW_DSP_CLR(w)   w &= ~( 0x1 << 3)

#define PMU_GLB_PWROFF_FRC_SW_VDEC_POS      2
#define PMU_GLB_PWROFF_FRC_SW_VDEC_LEN      1
#define PMU_GLB_PWROFF_FRC_SW_VDEC_MASK     0x00000004
#define PMU_GLB_PWROFF_FRC_SW_VDEC_VAL(w)   (((w) & 0x00000004) >> 2)
#define PMU_GLB_PWROFF_FRC_SW_VDEC_SET(w)   w |= ( 0x1 << 2)
#define PMU_GLB_PWROFF_FRC_SW_VDEC_CLR(w)   w &= ~( 0x1 << 2)

#define PMU_GLB_PWROFF_FRC_SW_VENC1_POS      1
#define PMU_GLB_PWROFF_FRC_SW_VENC1_LEN      1
#define PMU_GLB_PWROFF_FRC_SW_VENC1_MASK     0x00000002
#define PMU_GLB_PWROFF_FRC_SW_VENC1_VAL(w)   (((w) & 0x00000002) >> 1)
#define PMU_GLB_PWROFF_FRC_SW_VENC1_SET(w)   w |= ( 0x1 << 1)
#define PMU_GLB_PWROFF_FRC_SW_VENC1_CLR(w)   w &= ~( 0x1 << 1)

#define PMU_GLB_PWROFF_FRC_SW_VENC0_POS      0
#define PMU_GLB_PWROFF_FRC_SW_VENC0_LEN      1
#define PMU_GLB_PWROFF_FRC_SW_VENC0_MASK     0x00000001
#define PMU_GLB_PWROFF_FRC_SW_VENC0_VAL(w)   (((w) & 0x00000001) >> 0)
#define PMU_GLB_PWROFF_FRC_SW_VENC0_SET(w)   w |= ( 0x1 << 0)
#define PMU_GLB_PWROFF_FRC_SW_VENC0_CLR(w)   w &= ~( 0x1 << 0)

// REGISTER : PWROFF_FRC_SW1

#define PMU_GLB_PWROFF_FRC_SW1_ADDR      PMU_GLB_BASE_ADDR + 0x6C
#define PMU_GLB_PWROFF_FRC_SW1_RSTVAL    0x0
#define PMU_GLB_PWROFF_FRC_SW1_ALL1      0x7FF
#define PMU_GLB_PWROFF_FRC_SW1_ALL0      0x0
#define PMU_GLB_PWROFF_FRC_SW1_ALL5A     0x25A
#define PMU_GLB_PWROFF_FRC_SW1_ALLA5     0x5A5

// FIELD IN REGISTER : PWROFF_FRC_SW1

#define PMU_GLB_PWROFF_FRC_SW1_N8_POS      10
#define PMU_GLB_PWROFF_FRC_SW1_N8_LEN      1
#define PMU_GLB_PWROFF_FRC_SW1_N8_MASK     0x00000400
#define PMU_GLB_PWROFF_FRC_SW1_N8_VAL(w)   (((w) & 0x00000400) >> 10)
#define PMU_GLB_PWROFF_FRC_SW1_N8_SET(w)   w |= ( 0x1 << 10)
#define PMU_GLB_PWROFF_FRC_SW1_N8_CLR(w)   w &= ~( 0x1 << 10)

#define PMU_GLB_PWROFF_FRC_SW1_N7_POS      9
#define PMU_GLB_PWROFF_FRC_SW1_N7_LEN      1
#define PMU_GLB_PWROFF_FRC_SW1_N7_MASK     0x00000200
#define PMU_GLB_PWROFF_FRC_SW1_N7_VAL(w)   (((w) & 0x00000200) >> 9)
#define PMU_GLB_PWROFF_FRC_SW1_N7_SET(w)   w |= ( 0x1 << 9)
#define PMU_GLB_PWROFF_FRC_SW1_N7_CLR(w)   w &= ~( 0x1 << 9)

#define PMU_GLB_PWROFF_FRC_SW1_N6_POS      8
#define PMU_GLB_PWROFF_FRC_SW1_N6_LEN      1
#define PMU_GLB_PWROFF_FRC_SW1_N6_MASK     0x00000100
#define PMU_GLB_PWROFF_FRC_SW1_N6_VAL(w)   (((w) & 0x00000100) >> 8)
#define PMU_GLB_PWROFF_FRC_SW1_N6_SET(w)   w |= ( 0x1 << 8)
#define PMU_GLB_PWROFF_FRC_SW1_N6_CLR(w)   w &= ~( 0x1 << 8)

#define PMU_GLB_PWROFF_FRC_SW1_N5_POS      7
#define PMU_GLB_PWROFF_FRC_SW1_N5_LEN      1
#define PMU_GLB_PWROFF_FRC_SW1_N5_MASK     0x00000080
#define PMU_GLB_PWROFF_FRC_SW1_N5_VAL(w)   (((w) & 0x00000080) >> 7)
#define PMU_GLB_PWROFF_FRC_SW1_N5_SET(w)   w |= ( 0x1 << 7)
#define PMU_GLB_PWROFF_FRC_SW1_N5_CLR(w)   w &= ~( 0x1 << 7)

#define PMU_GLB_PWROFF_FRC_SW1_N4_POS      6
#define PMU_GLB_PWROFF_FRC_SW1_N4_LEN      1
#define PMU_GLB_PWROFF_FRC_SW1_N4_MASK     0x00000040
#define PMU_GLB_PWROFF_FRC_SW1_N4_VAL(w)   (((w) & 0x00000040) >> 6)
#define PMU_GLB_PWROFF_FRC_SW1_N4_SET(w)   w |= ( 0x1 << 6)
#define PMU_GLB_PWROFF_FRC_SW1_N4_CLR(w)   w &= ~( 0x1 << 6)

#define PMU_GLB_PWROFF_FRC_SW1_N3_POS      5
#define PMU_GLB_PWROFF_FRC_SW1_N3_LEN      1
#define PMU_GLB_PWROFF_FRC_SW1_N3_MASK     0x00000020
#define PMU_GLB_PWROFF_FRC_SW1_N3_VAL(w)   (((w) & 0x00000020) >> 5)
#define PMU_GLB_PWROFF_FRC_SW1_N3_SET(w)   w |= ( 0x1 << 5)
#define PMU_GLB_PWROFF_FRC_SW1_N3_CLR(w)   w &= ~( 0x1 << 5)

#define PMU_GLB_PWROFF_FRC_SW1_N2_POS      4
#define PMU_GLB_PWROFF_FRC_SW1_N2_LEN      1
#define PMU_GLB_PWROFF_FRC_SW1_N2_MASK     0x00000010
#define PMU_GLB_PWROFF_FRC_SW1_N2_VAL(w)   (((w) & 0x00000010) >> 4)
#define PMU_GLB_PWROFF_FRC_SW1_N2_SET(w)   w |= ( 0x1 << 4)
#define PMU_GLB_PWROFF_FRC_SW1_N2_CLR(w)   w &= ~( 0x1 << 4)

#define PMU_GLB_PWROFF_FRC_SW1_N1_POS      3
#define PMU_GLB_PWROFF_FRC_SW1_N1_LEN      1
#define PMU_GLB_PWROFF_FRC_SW1_N1_MASK     0x00000008
#define PMU_GLB_PWROFF_FRC_SW1_N1_VAL(w)   (((w) & 0x00000008) >> 3)
#define PMU_GLB_PWROFF_FRC_SW1_N1_SET(w)   w |= ( 0x1 << 3)
#define PMU_GLB_PWROFF_FRC_SW1_N1_CLR(w)   w &= ~( 0x1 << 3)

#define PMU_GLB_PWROFF_FRC_SW1_N0_POS      2
#define PMU_GLB_PWROFF_FRC_SW1_N0_LEN      1
#define PMU_GLB_PWROFF_FRC_SW1_N0_MASK     0x00000004
#define PMU_GLB_PWROFF_FRC_SW1_N0_VAL(w)   (((w) & 0x00000004) >> 2)
#define PMU_GLB_PWROFF_FRC_SW1_N0_SET(w)   w |= ( 0x1 << 2)
#define PMU_GLB_PWROFF_FRC_SW1_N0_CLR(w)   w &= ~( 0x1 << 2)

#define PMU_GLB_PWROFF_FRC_SW1_MM_POS      1
#define PMU_GLB_PWROFF_FRC_SW1_MM_LEN      1
#define PMU_GLB_PWROFF_FRC_SW1_MM_MASK     0x00000002
#define PMU_GLB_PWROFF_FRC_SW1_MM_VAL(w)   (((w) & 0x00000002) >> 1)
#define PMU_GLB_PWROFF_FRC_SW1_MM_SET(w)   w |= ( 0x1 << 1)
#define PMU_GLB_PWROFF_FRC_SW1_MM_CLR(w)   w &= ~( 0x1 << 1)

#define PMU_GLB_PWROFF_FRC_SW1_PIPE_POS      0
#define PMU_GLB_PWROFF_FRC_SW1_PIPE_LEN      1
#define PMU_GLB_PWROFF_FRC_SW1_PIPE_MASK     0x00000001
#define PMU_GLB_PWROFF_FRC_SW1_PIPE_VAL(w)   (((w) & 0x00000001) >> 0)
#define PMU_GLB_PWROFF_FRC_SW1_PIPE_SET(w)   w |= ( 0x1 << 0)
#define PMU_GLB_PWROFF_FRC_SW1_PIPE_CLR(w)   w &= ~( 0x1 << 0)

// REGISTER : ISO_FRC_EN

#define PMU_GLB_ISO_FRC_EN_ADDR      PMU_GLB_BASE_ADDR + 0x70
#define PMU_GLB_ISO_FRC_EN_RSTVAL    0x0
#define PMU_GLB_ISO_FRC_EN_ALL1      0x7FFF
#define PMU_GLB_ISO_FRC_EN_ALL0      0x0
#define PMU_GLB_ISO_FRC_EN_ALL5A     0x5A5A
#define PMU_GLB_ISO_FRC_EN_ALLA5     0x25A5

// FIELD IN REGISTER : ISO_FRC_EN

#define PMU_GLB_ISO_FRC_EN_ISP_POS      14
#define PMU_GLB_ISO_FRC_EN_ISP_LEN      1
#define PMU_GLB_ISO_FRC_EN_ISP_MASK     0x00004000
#define PMU_GLB_ISO_FRC_EN_ISP_VAL(w)   (((w) & 0x00004000) >> 14)
#define PMU_GLB_ISO_FRC_EN_ISP_SET(w)   w |= ( 0x1 << 14)
#define PMU_GLB_ISO_FRC_EN_ISP_CLR(w)   w &= ~( 0x1 << 14)

#define PMU_GLB_ISO_FRC_EN_CPU_POS      13
#define PMU_GLB_ISO_FRC_EN_CPU_LEN      1
#define PMU_GLB_ISO_FRC_EN_CPU_MASK     0x00002000
#define PMU_GLB_ISO_FRC_EN_CPU_VAL(w)   (((w) & 0x00002000) >> 13)
#define PMU_GLB_ISO_FRC_EN_CPU_SET(w)   w |= ( 0x1 << 13)
#define PMU_GLB_ISO_FRC_EN_CPU_CLR(w)   w &= ~( 0x1 << 13)

#define PMU_GLB_ISO_FRC_EN_NPU_POS      12
#define PMU_GLB_ISO_FRC_EN_NPU_LEN      1
#define PMU_GLB_ISO_FRC_EN_NPU_MASK     0x00001000
#define PMU_GLB_ISO_FRC_EN_NPU_VAL(w)   (((w) & 0x00001000) >> 12)
#define PMU_GLB_ISO_FRC_EN_NPU_SET(w)   w |= ( 0x1 << 12)
#define PMU_GLB_ISO_FRC_EN_NPU_CLR(w)   w &= ~( 0x1 << 12)

#define PMU_GLB_ISO_FRC_EN_CPU7_POS      11
#define PMU_GLB_ISO_FRC_EN_CPU7_LEN      1
#define PMU_GLB_ISO_FRC_EN_CPU7_MASK     0x00000800
#define PMU_GLB_ISO_FRC_EN_CPU7_VAL(w)   (((w) & 0x00000800) >> 11)
#define PMU_GLB_ISO_FRC_EN_CPU7_SET(w)   w |= ( 0x1 << 11)
#define PMU_GLB_ISO_FRC_EN_CPU7_CLR(w)   w &= ~( 0x1 << 11)

#define PMU_GLB_ISO_FRC_EN_CPU6_POS      10
#define PMU_GLB_ISO_FRC_EN_CPU6_LEN      1
#define PMU_GLB_ISO_FRC_EN_CPU6_MASK     0x00000400
#define PMU_GLB_ISO_FRC_EN_CPU6_VAL(w)   (((w) & 0x00000400) >> 10)
#define PMU_GLB_ISO_FRC_EN_CPU6_SET(w)   w |= ( 0x1 << 10)
#define PMU_GLB_ISO_FRC_EN_CPU6_CLR(w)   w &= ~( 0x1 << 10)

#define PMU_GLB_ISO_FRC_EN_CPU5_POS      9
#define PMU_GLB_ISO_FRC_EN_CPU5_LEN      1
#define PMU_GLB_ISO_FRC_EN_CPU5_MASK     0x00000200
#define PMU_GLB_ISO_FRC_EN_CPU5_VAL(w)   (((w) & 0x00000200) >> 9)
#define PMU_GLB_ISO_FRC_EN_CPU5_SET(w)   w |= ( 0x1 << 9)
#define PMU_GLB_ISO_FRC_EN_CPU5_CLR(w)   w &= ~( 0x1 << 9)

#define PMU_GLB_ISO_FRC_EN_CPU4_POS      8
#define PMU_GLB_ISO_FRC_EN_CPU4_LEN      1
#define PMU_GLB_ISO_FRC_EN_CPU4_MASK     0x00000100
#define PMU_GLB_ISO_FRC_EN_CPU4_VAL(w)   (((w) & 0x00000100) >> 8)
#define PMU_GLB_ISO_FRC_EN_CPU4_SET(w)   w |= ( 0x1 << 8)
#define PMU_GLB_ISO_FRC_EN_CPU4_CLR(w)   w &= ~( 0x1 << 8)

#define PMU_GLB_ISO_FRC_EN_CPU3_POS      7
#define PMU_GLB_ISO_FRC_EN_CPU3_LEN      1
#define PMU_GLB_ISO_FRC_EN_CPU3_MASK     0x00000080
#define PMU_GLB_ISO_FRC_EN_CPU3_VAL(w)   (((w) & 0x00000080) >> 7)
#define PMU_GLB_ISO_FRC_EN_CPU3_SET(w)   w |= ( 0x1 << 7)
#define PMU_GLB_ISO_FRC_EN_CPU3_CLR(w)   w &= ~( 0x1 << 7)

#define PMU_GLB_ISO_FRC_EN_CPU2_POS      6
#define PMU_GLB_ISO_FRC_EN_CPU2_LEN      1
#define PMU_GLB_ISO_FRC_EN_CPU2_MASK     0x00000040
#define PMU_GLB_ISO_FRC_EN_CPU2_VAL(w)   (((w) & 0x00000040) >> 6)
#define PMU_GLB_ISO_FRC_EN_CPU2_SET(w)   w |= ( 0x1 << 6)
#define PMU_GLB_ISO_FRC_EN_CPU2_CLR(w)   w &= ~( 0x1 << 6)

#define PMU_GLB_ISO_FRC_EN_CPU1_POS      5
#define PMU_GLB_ISO_FRC_EN_CPU1_LEN      1
#define PMU_GLB_ISO_FRC_EN_CPU1_MASK     0x00000020
#define PMU_GLB_ISO_FRC_EN_CPU1_VAL(w)   (((w) & 0x00000020) >> 5)
#define PMU_GLB_ISO_FRC_EN_CPU1_SET(w)   w |= ( 0x1 << 5)
#define PMU_GLB_ISO_FRC_EN_CPU1_CLR(w)   w &= ~( 0x1 << 5)

#define PMU_GLB_ISO_FRC_EN_CPU0_POS      4
#define PMU_GLB_ISO_FRC_EN_CPU0_LEN      1
#define PMU_GLB_ISO_FRC_EN_CPU0_MASK     0x00000010
#define PMU_GLB_ISO_FRC_EN_CPU0_VAL(w)   (((w) & 0x00000010) >> 4)
#define PMU_GLB_ISO_FRC_EN_CPU0_SET(w)   w |= ( 0x1 << 4)
#define PMU_GLB_ISO_FRC_EN_CPU0_CLR(w)   w &= ~( 0x1 << 4)

#define PMU_GLB_ISO_FRC_EN_DSP_POS      3
#define PMU_GLB_ISO_FRC_EN_DSP_LEN      1
#define PMU_GLB_ISO_FRC_EN_DSP_MASK     0x00000008
#define PMU_GLB_ISO_FRC_EN_DSP_VAL(w)   (((w) & 0x00000008) >> 3)
#define PMU_GLB_ISO_FRC_EN_DSP_SET(w)   w |= ( 0x1 << 3)
#define PMU_GLB_ISO_FRC_EN_DSP_CLR(w)   w &= ~( 0x1 << 3)

#define PMU_GLB_ISO_FRC_EN_VDEC_POS      2
#define PMU_GLB_ISO_FRC_EN_VDEC_LEN      1
#define PMU_GLB_ISO_FRC_EN_VDEC_MASK     0x00000004
#define PMU_GLB_ISO_FRC_EN_VDEC_VAL(w)   (((w) & 0x00000004) >> 2)
#define PMU_GLB_ISO_FRC_EN_VDEC_SET(w)   w |= ( 0x1 << 2)
#define PMU_GLB_ISO_FRC_EN_VDEC_CLR(w)   w &= ~( 0x1 << 2)

#define PMU_GLB_ISO_FRC_EN_VENC1_POS      1
#define PMU_GLB_ISO_FRC_EN_VENC1_LEN      1
#define PMU_GLB_ISO_FRC_EN_VENC1_MASK     0x00000002
#define PMU_GLB_ISO_FRC_EN_VENC1_VAL(w)   (((w) & 0x00000002) >> 1)
#define PMU_GLB_ISO_FRC_EN_VENC1_SET(w)   w |= ( 0x1 << 1)
#define PMU_GLB_ISO_FRC_EN_VENC1_CLR(w)   w &= ~( 0x1 << 1)

#define PMU_GLB_ISO_FRC_EN_VENC0_POS      0
#define PMU_GLB_ISO_FRC_EN_VENC0_LEN      1
#define PMU_GLB_ISO_FRC_EN_VENC0_MASK     0x00000001
#define PMU_GLB_ISO_FRC_EN_VENC0_VAL(w)   (((w) & 0x00000001) >> 0)
#define PMU_GLB_ISO_FRC_EN_VENC0_SET(w)   w |= ( 0x1 << 0)
#define PMU_GLB_ISO_FRC_EN_VENC0_CLR(w)   w &= ~( 0x1 << 0)

// REGISTER : ISO_FRC_EN1

#define PMU_GLB_ISO_FRC_EN1_ADDR      PMU_GLB_BASE_ADDR + 0x74
#define PMU_GLB_ISO_FRC_EN1_RSTVAL    0x0
#define PMU_GLB_ISO_FRC_EN1_ALL1      0x7FF
#define PMU_GLB_ISO_FRC_EN1_ALL0      0x0
#define PMU_GLB_ISO_FRC_EN1_ALL5A     0x25A
#define PMU_GLB_ISO_FRC_EN1_ALLA5     0x5A5

// FIELD IN REGISTER : ISO_FRC_EN1

#define PMU_GLB_ISO_FRC_EN1_N8_POS      10
#define PMU_GLB_ISO_FRC_EN1_N8_LEN      1
#define PMU_GLB_ISO_FRC_EN1_N8_MASK     0x00000400
#define PMU_GLB_ISO_FRC_EN1_N8_VAL(w)   (((w) & 0x00000400) >> 10)
#define PMU_GLB_ISO_FRC_EN1_N8_SET(w)   w |= ( 0x1 << 10)
#define PMU_GLB_ISO_FRC_EN1_N8_CLR(w)   w &= ~( 0x1 << 10)

#define PMU_GLB_ISO_FRC_EN1_N7_POS      9
#define PMU_GLB_ISO_FRC_EN1_N7_LEN      1
#define PMU_GLB_ISO_FRC_EN1_N7_MASK     0x00000200
#define PMU_GLB_ISO_FRC_EN1_N7_VAL(w)   (((w) & 0x00000200) >> 9)
#define PMU_GLB_ISO_FRC_EN1_N7_SET(w)   w |= ( 0x1 << 9)
#define PMU_GLB_ISO_FRC_EN1_N7_CLR(w)   w &= ~( 0x1 << 9)

#define PMU_GLB_ISO_FRC_EN1_N6_POS      8
#define PMU_GLB_ISO_FRC_EN1_N6_LEN      1
#define PMU_GLB_ISO_FRC_EN1_N6_MASK     0x00000100
#define PMU_GLB_ISO_FRC_EN1_N6_VAL(w)   (((w) & 0x00000100) >> 8)
#define PMU_GLB_ISO_FRC_EN1_N6_SET(w)   w |= ( 0x1 << 8)
#define PMU_GLB_ISO_FRC_EN1_N6_CLR(w)   w &= ~( 0x1 << 8)

#define PMU_GLB_ISO_FRC_EN1_N5_POS      7
#define PMU_GLB_ISO_FRC_EN1_N5_LEN      1
#define PMU_GLB_ISO_FRC_EN1_N5_MASK     0x00000080
#define PMU_GLB_ISO_FRC_EN1_N5_VAL(w)   (((w) & 0x00000080) >> 7)
#define PMU_GLB_ISO_FRC_EN1_N5_SET(w)   w |= ( 0x1 << 7)
#define PMU_GLB_ISO_FRC_EN1_N5_CLR(w)   w &= ~( 0x1 << 7)

#define PMU_GLB_ISO_FRC_EN1_N4_POS      6
#define PMU_GLB_ISO_FRC_EN1_N4_LEN      1
#define PMU_GLB_ISO_FRC_EN1_N4_MASK     0x00000040
#define PMU_GLB_ISO_FRC_EN1_N4_VAL(w)   (((w) & 0x00000040) >> 6)
#define PMU_GLB_ISO_FRC_EN1_N4_SET(w)   w |= ( 0x1 << 6)
#define PMU_GLB_ISO_FRC_EN1_N4_CLR(w)   w &= ~( 0x1 << 6)

#define PMU_GLB_ISO_FRC_EN1_N3_POS      5
#define PMU_GLB_ISO_FRC_EN1_N3_LEN      1
#define PMU_GLB_ISO_FRC_EN1_N3_MASK     0x00000020
#define PMU_GLB_ISO_FRC_EN1_N3_VAL(w)   (((w) & 0x00000020) >> 5)
#define PMU_GLB_ISO_FRC_EN1_N3_SET(w)   w |= ( 0x1 << 5)
#define PMU_GLB_ISO_FRC_EN1_N3_CLR(w)   w &= ~( 0x1 << 5)

#define PMU_GLB_ISO_FRC_EN1_N2_POS      4
#define PMU_GLB_ISO_FRC_EN1_N2_LEN      1
#define PMU_GLB_ISO_FRC_EN1_N2_MASK     0x00000010
#define PMU_GLB_ISO_FRC_EN1_N2_VAL(w)   (((w) & 0x00000010) >> 4)
#define PMU_GLB_ISO_FRC_EN1_N2_SET(w)   w |= ( 0x1 << 4)
#define PMU_GLB_ISO_FRC_EN1_N2_CLR(w)   w &= ~( 0x1 << 4)

#define PMU_GLB_ISO_FRC_EN1_N1_POS      3
#define PMU_GLB_ISO_FRC_EN1_N1_LEN      1
#define PMU_GLB_ISO_FRC_EN1_N1_MASK     0x00000008
#define PMU_GLB_ISO_FRC_EN1_N1_VAL(w)   (((w) & 0x00000008) >> 3)
#define PMU_GLB_ISO_FRC_EN1_N1_SET(w)   w |= ( 0x1 << 3)
#define PMU_GLB_ISO_FRC_EN1_N1_CLR(w)   w &= ~( 0x1 << 3)

#define PMU_GLB_ISO_FRC_EN1_N0_POS      2
#define PMU_GLB_ISO_FRC_EN1_N0_LEN      1
#define PMU_GLB_ISO_FRC_EN1_N0_MASK     0x00000004
#define PMU_GLB_ISO_FRC_EN1_N0_VAL(w)   (((w) & 0x00000004) >> 2)
#define PMU_GLB_ISO_FRC_EN1_N0_SET(w)   w |= ( 0x1 << 2)
#define PMU_GLB_ISO_FRC_EN1_N0_CLR(w)   w &= ~( 0x1 << 2)

#define PMU_GLB_ISO_FRC_EN1_MM_POS      1
#define PMU_GLB_ISO_FRC_EN1_MM_LEN      1
#define PMU_GLB_ISO_FRC_EN1_MM_MASK     0x00000002
#define PMU_GLB_ISO_FRC_EN1_MM_VAL(w)   (((w) & 0x00000002) >> 1)
#define PMU_GLB_ISO_FRC_EN1_MM_SET(w)   w |= ( 0x1 << 1)
#define PMU_GLB_ISO_FRC_EN1_MM_CLR(w)   w &= ~( 0x1 << 1)

#define PMU_GLB_ISO_FRC_EN1_PIPE_POS      0
#define PMU_GLB_ISO_FRC_EN1_PIPE_LEN      1
#define PMU_GLB_ISO_FRC_EN1_PIPE_MASK     0x00000001
#define PMU_GLB_ISO_FRC_EN1_PIPE_VAL(w)   (((w) & 0x00000001) >> 0)
#define PMU_GLB_ISO_FRC_EN1_PIPE_SET(w)   w |= ( 0x1 << 0)
#define PMU_GLB_ISO_FRC_EN1_PIPE_CLR(w)   w &= ~( 0x1 << 0)

// REGISTER : ISO_FRC_SW

#define PMU_GLB_ISO_FRC_SW_ADDR      PMU_GLB_BASE_ADDR + 0x78
#define PMU_GLB_ISO_FRC_SW_RSTVAL    0x0
#define PMU_GLB_ISO_FRC_SW_ALL1      0x7FFF
#define PMU_GLB_ISO_FRC_SW_ALL0      0x0
#define PMU_GLB_ISO_FRC_SW_ALL5A     0x5A5A
#define PMU_GLB_ISO_FRC_SW_ALLA5     0x25A5

// FIELD IN REGISTER : ISO_FRC_SW

#define PMU_GLB_ISO_FRC_SW_ISP_POS      14
#define PMU_GLB_ISO_FRC_SW_ISP_LEN      1
#define PMU_GLB_ISO_FRC_SW_ISP_MASK     0x00004000
#define PMU_GLB_ISO_FRC_SW_ISP_VAL(w)   (((w) & 0x00004000) >> 14)
#define PMU_GLB_ISO_FRC_SW_ISP_SET(w)   w |= ( 0x1 << 14)
#define PMU_GLB_ISO_FRC_SW_ISP_CLR(w)   w &= ~( 0x1 << 14)

#define PMU_GLB_ISO_FRC_SW_CPU_POS      13
#define PMU_GLB_ISO_FRC_SW_CPU_LEN      1
#define PMU_GLB_ISO_FRC_SW_CPU_MASK     0x00002000
#define PMU_GLB_ISO_FRC_SW_CPU_VAL(w)   (((w) & 0x00002000) >> 13)
#define PMU_GLB_ISO_FRC_SW_CPU_SET(w)   w |= ( 0x1 << 13)
#define PMU_GLB_ISO_FRC_SW_CPU_CLR(w)   w &= ~( 0x1 << 13)

#define PMU_GLB_ISO_FRC_SW_NPU_POS      12
#define PMU_GLB_ISO_FRC_SW_NPU_LEN      1
#define PMU_GLB_ISO_FRC_SW_NPU_MASK     0x00001000
#define PMU_GLB_ISO_FRC_SW_NPU_VAL(w)   (((w) & 0x00001000) >> 12)
#define PMU_GLB_ISO_FRC_SW_NPU_SET(w)   w |= ( 0x1 << 12)
#define PMU_GLB_ISO_FRC_SW_NPU_CLR(w)   w &= ~( 0x1 << 12)

#define PMU_GLB_ISO_FRC_SW_CPU7_POS      11
#define PMU_GLB_ISO_FRC_SW_CPU7_LEN      1
#define PMU_GLB_ISO_FRC_SW_CPU7_MASK     0x00000800
#define PMU_GLB_ISO_FRC_SW_CPU7_VAL(w)   (((w) & 0x00000800) >> 11)
#define PMU_GLB_ISO_FRC_SW_CPU7_SET(w)   w |= ( 0x1 << 11)
#define PMU_GLB_ISO_FRC_SW_CPU7_CLR(w)   w &= ~( 0x1 << 11)

#define PMU_GLB_ISO_FRC_SW_CPU6_POS      10
#define PMU_GLB_ISO_FRC_SW_CPU6_LEN      1
#define PMU_GLB_ISO_FRC_SW_CPU6_MASK     0x00000400
#define PMU_GLB_ISO_FRC_SW_CPU6_VAL(w)   (((w) & 0x00000400) >> 10)
#define PMU_GLB_ISO_FRC_SW_CPU6_SET(w)   w |= ( 0x1 << 10)
#define PMU_GLB_ISO_FRC_SW_CPU6_CLR(w)   w &= ~( 0x1 << 10)

#define PMU_GLB_ISO_FRC_SW_CPU5_POS      9
#define PMU_GLB_ISO_FRC_SW_CPU5_LEN      1
#define PMU_GLB_ISO_FRC_SW_CPU5_MASK     0x00000200
#define PMU_GLB_ISO_FRC_SW_CPU5_VAL(w)   (((w) & 0x00000200) >> 9)
#define PMU_GLB_ISO_FRC_SW_CPU5_SET(w)   w |= ( 0x1 << 9)
#define PMU_GLB_ISO_FRC_SW_CPU5_CLR(w)   w &= ~( 0x1 << 9)

#define PMU_GLB_ISO_FRC_SW_CPU4_POS      8
#define PMU_GLB_ISO_FRC_SW_CPU4_LEN      1
#define PMU_GLB_ISO_FRC_SW_CPU4_MASK     0x00000100
#define PMU_GLB_ISO_FRC_SW_CPU4_VAL(w)   (((w) & 0x00000100) >> 8)
#define PMU_GLB_ISO_FRC_SW_CPU4_SET(w)   w |= ( 0x1 << 8)
#define PMU_GLB_ISO_FRC_SW_CPU4_CLR(w)   w &= ~( 0x1 << 8)

#define PMU_GLB_ISO_FRC_SW_CPU3_POS      7
#define PMU_GLB_ISO_FRC_SW_CPU3_LEN      1
#define PMU_GLB_ISO_FRC_SW_CPU3_MASK     0x00000080
#define PMU_GLB_ISO_FRC_SW_CPU3_VAL(w)   (((w) & 0x00000080) >> 7)
#define PMU_GLB_ISO_FRC_SW_CPU3_SET(w)   w |= ( 0x1 << 7)
#define PMU_GLB_ISO_FRC_SW_CPU3_CLR(w)   w &= ~( 0x1 << 7)

#define PMU_GLB_ISO_FRC_SW_CPU2_POS      6
#define PMU_GLB_ISO_FRC_SW_CPU2_LEN      1
#define PMU_GLB_ISO_FRC_SW_CPU2_MASK     0x00000040
#define PMU_GLB_ISO_FRC_SW_CPU2_VAL(w)   (((w) & 0x00000040) >> 6)
#define PMU_GLB_ISO_FRC_SW_CPU2_SET(w)   w |= ( 0x1 << 6)
#define PMU_GLB_ISO_FRC_SW_CPU2_CLR(w)   w &= ~( 0x1 << 6)

#define PMU_GLB_ISO_FRC_SW_CPU1_POS      5
#define PMU_GLB_ISO_FRC_SW_CPU1_LEN      1
#define PMU_GLB_ISO_FRC_SW_CPU1_MASK     0x00000020
#define PMU_GLB_ISO_FRC_SW_CPU1_VAL(w)   (((w) & 0x00000020) >> 5)
#define PMU_GLB_ISO_FRC_SW_CPU1_SET(w)   w |= ( 0x1 << 5)
#define PMU_GLB_ISO_FRC_SW_CPU1_CLR(w)   w &= ~( 0x1 << 5)

#define PMU_GLB_ISO_FRC_SW_CPU0_POS      4
#define PMU_GLB_ISO_FRC_SW_CPU0_LEN      1
#define PMU_GLB_ISO_FRC_SW_CPU0_MASK     0x00000010
#define PMU_GLB_ISO_FRC_SW_CPU0_VAL(w)   (((w) & 0x00000010) >> 4)
#define PMU_GLB_ISO_FRC_SW_CPU0_SET(w)   w |= ( 0x1 << 4)
#define PMU_GLB_ISO_FRC_SW_CPU0_CLR(w)   w &= ~( 0x1 << 4)

#define PMU_GLB_ISO_FRC_SW_DSP_POS      3
#define PMU_GLB_ISO_FRC_SW_DSP_LEN      1
#define PMU_GLB_ISO_FRC_SW_DSP_MASK     0x00000008
#define PMU_GLB_ISO_FRC_SW_DSP_VAL(w)   (((w) & 0x00000008) >> 3)
#define PMU_GLB_ISO_FRC_SW_DSP_SET(w)   w |= ( 0x1 << 3)
#define PMU_GLB_ISO_FRC_SW_DSP_CLR(w)   w &= ~( 0x1 << 3)

#define PMU_GLB_ISO_FRC_SW_VDEC_POS      2
#define PMU_GLB_ISO_FRC_SW_VDEC_LEN      1
#define PMU_GLB_ISO_FRC_SW_VDEC_MASK     0x00000004
#define PMU_GLB_ISO_FRC_SW_VDEC_VAL(w)   (((w) & 0x00000004) >> 2)
#define PMU_GLB_ISO_FRC_SW_VDEC_SET(w)   w |= ( 0x1 << 2)
#define PMU_GLB_ISO_FRC_SW_VDEC_CLR(w)   w &= ~( 0x1 << 2)

#define PMU_GLB_ISO_FRC_SW_VENC1_POS      1
#define PMU_GLB_ISO_FRC_SW_VENC1_LEN      1
#define PMU_GLB_ISO_FRC_SW_VENC1_MASK     0x00000002
#define PMU_GLB_ISO_FRC_SW_VENC1_VAL(w)   (((w) & 0x00000002) >> 1)
#define PMU_GLB_ISO_FRC_SW_VENC1_SET(w)   w |= ( 0x1 << 1)
#define PMU_GLB_ISO_FRC_SW_VENC1_CLR(w)   w &= ~( 0x1 << 1)

#define PMU_GLB_ISO_FRC_SW_VENC0_POS      0
#define PMU_GLB_ISO_FRC_SW_VENC0_LEN      1
#define PMU_GLB_ISO_FRC_SW_VENC0_MASK     0x00000001
#define PMU_GLB_ISO_FRC_SW_VENC0_VAL(w)   (((w) & 0x00000001) >> 0)
#define PMU_GLB_ISO_FRC_SW_VENC0_SET(w)   w |= ( 0x1 << 0)
#define PMU_GLB_ISO_FRC_SW_VENC0_CLR(w)   w &= ~( 0x1 << 0)

// REGISTER : ISO_FRC_SW1

#define PMU_GLB_ISO_FRC_SW1_ADDR      PMU_GLB_BASE_ADDR + 0x7C
#define PMU_GLB_ISO_FRC_SW1_RSTVAL    0x0
#define PMU_GLB_ISO_FRC_SW1_ALL1      0x7FF
#define PMU_GLB_ISO_FRC_SW1_ALL0      0x0
#define PMU_GLB_ISO_FRC_SW1_ALL5A     0x25A
#define PMU_GLB_ISO_FRC_SW1_ALLA5     0x5A5

// FIELD IN REGISTER : ISO_FRC_SW1

#define PMU_GLB_ISO_FRC_SW1_N8_POS      10
#define PMU_GLB_ISO_FRC_SW1_N8_LEN      1
#define PMU_GLB_ISO_FRC_SW1_N8_MASK     0x00000400
#define PMU_GLB_ISO_FRC_SW1_N8_VAL(w)   (((w) & 0x00000400) >> 10)
#define PMU_GLB_ISO_FRC_SW1_N8_SET(w)   w |= ( 0x1 << 10)
#define PMU_GLB_ISO_FRC_SW1_N8_CLR(w)   w &= ~( 0x1 << 10)

#define PMU_GLB_ISO_FRC_SW1_N7_POS      9
#define PMU_GLB_ISO_FRC_SW1_N7_LEN      1
#define PMU_GLB_ISO_FRC_SW1_N7_MASK     0x00000200
#define PMU_GLB_ISO_FRC_SW1_N7_VAL(w)   (((w) & 0x00000200) >> 9)
#define PMU_GLB_ISO_FRC_SW1_N7_SET(w)   w |= ( 0x1 << 9)
#define PMU_GLB_ISO_FRC_SW1_N7_CLR(w)   w &= ~( 0x1 << 9)

#define PMU_GLB_ISO_FRC_SW1_N6_POS      8
#define PMU_GLB_ISO_FRC_SW1_N6_LEN      1
#define PMU_GLB_ISO_FRC_SW1_N6_MASK     0x00000100
#define PMU_GLB_ISO_FRC_SW1_N6_VAL(w)   (((w) & 0x00000100) >> 8)
#define PMU_GLB_ISO_FRC_SW1_N6_SET(w)   w |= ( 0x1 << 8)
#define PMU_GLB_ISO_FRC_SW1_N6_CLR(w)   w &= ~( 0x1 << 8)

#define PMU_GLB_ISO_FRC_SW1_N5_POS      7
#define PMU_GLB_ISO_FRC_SW1_N5_LEN      1
#define PMU_GLB_ISO_FRC_SW1_N5_MASK     0x00000080
#define PMU_GLB_ISO_FRC_SW1_N5_VAL(w)   (((w) & 0x00000080) >> 7)
#define PMU_GLB_ISO_FRC_SW1_N5_SET(w)   w |= ( 0x1 << 7)
#define PMU_GLB_ISO_FRC_SW1_N5_CLR(w)   w &= ~( 0x1 << 7)

#define PMU_GLB_ISO_FRC_SW1_N4_POS      6
#define PMU_GLB_ISO_FRC_SW1_N4_LEN      1
#define PMU_GLB_ISO_FRC_SW1_N4_MASK     0x00000040
#define PMU_GLB_ISO_FRC_SW1_N4_VAL(w)   (((w) & 0x00000040) >> 6)
#define PMU_GLB_ISO_FRC_SW1_N4_SET(w)   w |= ( 0x1 << 6)
#define PMU_GLB_ISO_FRC_SW1_N4_CLR(w)   w &= ~( 0x1 << 6)

#define PMU_GLB_ISO_FRC_SW1_N3_POS      5
#define PMU_GLB_ISO_FRC_SW1_N3_LEN      1
#define PMU_GLB_ISO_FRC_SW1_N3_MASK     0x00000020
#define PMU_GLB_ISO_FRC_SW1_N3_VAL(w)   (((w) & 0x00000020) >> 5)
#define PMU_GLB_ISO_FRC_SW1_N3_SET(w)   w |= ( 0x1 << 5)
#define PMU_GLB_ISO_FRC_SW1_N3_CLR(w)   w &= ~( 0x1 << 5)

#define PMU_GLB_ISO_FRC_SW1_N2_POS      4
#define PMU_GLB_ISO_FRC_SW1_N2_LEN      1
#define PMU_GLB_ISO_FRC_SW1_N2_MASK     0x00000010
#define PMU_GLB_ISO_FRC_SW1_N2_VAL(w)   (((w) & 0x00000010) >> 4)
#define PMU_GLB_ISO_FRC_SW1_N2_SET(w)   w |= ( 0x1 << 4)
#define PMU_GLB_ISO_FRC_SW1_N2_CLR(w)   w &= ~( 0x1 << 4)

#define PMU_GLB_ISO_FRC_SW1_N1_POS      3
#define PMU_GLB_ISO_FRC_SW1_N1_LEN      1
#define PMU_GLB_ISO_FRC_SW1_N1_MASK     0x00000008
#define PMU_GLB_ISO_FRC_SW1_N1_VAL(w)   (((w) & 0x00000008) >> 3)
#define PMU_GLB_ISO_FRC_SW1_N1_SET(w)   w |= ( 0x1 << 3)
#define PMU_GLB_ISO_FRC_SW1_N1_CLR(w)   w &= ~( 0x1 << 3)

#define PMU_GLB_ISO_FRC_SW1_N0_POS      2
#define PMU_GLB_ISO_FRC_SW1_N0_LEN      1
#define PMU_GLB_ISO_FRC_SW1_N0_MASK     0x00000004
#define PMU_GLB_ISO_FRC_SW1_N0_VAL(w)   (((w) & 0x00000004) >> 2)
#define PMU_GLB_ISO_FRC_SW1_N0_SET(w)   w |= ( 0x1 << 2)
#define PMU_GLB_ISO_FRC_SW1_N0_CLR(w)   w &= ~( 0x1 << 2)

#define PMU_GLB_ISO_FRC_SW1_MM_POS      1
#define PMU_GLB_ISO_FRC_SW1_MM_LEN      1
#define PMU_GLB_ISO_FRC_SW1_MM_MASK     0x00000002
#define PMU_GLB_ISO_FRC_SW1_MM_VAL(w)   (((w) & 0x00000002) >> 1)
#define PMU_GLB_ISO_FRC_SW1_MM_SET(w)   w |= ( 0x1 << 1)
#define PMU_GLB_ISO_FRC_SW1_MM_CLR(w)   w &= ~( 0x1 << 1)

#define PMU_GLB_ISO_FRC_SW1_PIPE_POS      0
#define PMU_GLB_ISO_FRC_SW1_PIPE_LEN      1
#define PMU_GLB_ISO_FRC_SW1_PIPE_MASK     0x00000001
#define PMU_GLB_ISO_FRC_SW1_PIPE_VAL(w)   (((w) & 0x00000001) >> 0)
#define PMU_GLB_ISO_FRC_SW1_PIPE_SET(w)   w |= ( 0x1 << 0)
#define PMU_GLB_ISO_FRC_SW1_PIPE_CLR(w)   w &= ~( 0x1 << 0)

// REGISTER : CLK_FRC_EN

#define PMU_GLB_CLK_FRC_EN_ADDR      PMU_GLB_BASE_ADDR + 0x80
#define PMU_GLB_CLK_FRC_EN_RSTVAL    0x0
#define PMU_GLB_CLK_FRC_EN_ALL1      0xFFF
#define PMU_GLB_CLK_FRC_EN_ALL0      0x0
#define PMU_GLB_CLK_FRC_EN_ALL5A     0xA5A
#define PMU_GLB_CLK_FRC_EN_ALLA5     0x5A5

// FIELD IN REGISTER : CLK_FRC_EN

#define PMU_GLB_CLK_FRC_EN_CPU7_POS      11
#define PMU_GLB_CLK_FRC_EN_CPU7_LEN      1
#define PMU_GLB_CLK_FRC_EN_CPU7_MASK     0x00000800
#define PMU_GLB_CLK_FRC_EN_CPU7_VAL(w)   (((w) & 0x00000800) >> 11)
#define PMU_GLB_CLK_FRC_EN_CPU7_SET(w)   w |= ( 0x1 << 11)
#define PMU_GLB_CLK_FRC_EN_CPU7_CLR(w)   w &= ~( 0x1 << 11)

#define PMU_GLB_CLK_FRC_EN_CPU6_POS      10
#define PMU_GLB_CLK_FRC_EN_CPU6_LEN      1
#define PMU_GLB_CLK_FRC_EN_CPU6_MASK     0x00000400
#define PMU_GLB_CLK_FRC_EN_CPU6_VAL(w)   (((w) & 0x00000400) >> 10)
#define PMU_GLB_CLK_FRC_EN_CPU6_SET(w)   w |= ( 0x1 << 10)
#define PMU_GLB_CLK_FRC_EN_CPU6_CLR(w)   w &= ~( 0x1 << 10)

#define PMU_GLB_CLK_FRC_EN_CPU5_POS      9
#define PMU_GLB_CLK_FRC_EN_CPU5_LEN      1
#define PMU_GLB_CLK_FRC_EN_CPU5_MASK     0x00000200
#define PMU_GLB_CLK_FRC_EN_CPU5_VAL(w)   (((w) & 0x00000200) >> 9)
#define PMU_GLB_CLK_FRC_EN_CPU5_SET(w)   w |= ( 0x1 << 9)
#define PMU_GLB_CLK_FRC_EN_CPU5_CLR(w)   w &= ~( 0x1 << 9)

#define PMU_GLB_CLK_FRC_EN_CPU4_POS      8
#define PMU_GLB_CLK_FRC_EN_CPU4_LEN      1
#define PMU_GLB_CLK_FRC_EN_CPU4_MASK     0x00000100
#define PMU_GLB_CLK_FRC_EN_CPU4_VAL(w)   (((w) & 0x00000100) >> 8)
#define PMU_GLB_CLK_FRC_EN_CPU4_SET(w)   w |= ( 0x1 << 8)
#define PMU_GLB_CLK_FRC_EN_CPU4_CLR(w)   w &= ~( 0x1 << 8)

#define PMU_GLB_CLK_FRC_EN_CPU3_POS      7
#define PMU_GLB_CLK_FRC_EN_CPU3_LEN      1
#define PMU_GLB_CLK_FRC_EN_CPU3_MASK     0x00000080
#define PMU_GLB_CLK_FRC_EN_CPU3_VAL(w)   (((w) & 0x00000080) >> 7)
#define PMU_GLB_CLK_FRC_EN_CPU3_SET(w)   w |= ( 0x1 << 7)
#define PMU_GLB_CLK_FRC_EN_CPU3_CLR(w)   w &= ~( 0x1 << 7)

#define PMU_GLB_CLK_FRC_EN_CPU2_POS      6
#define PMU_GLB_CLK_FRC_EN_CPU2_LEN      1
#define PMU_GLB_CLK_FRC_EN_CPU2_MASK     0x00000040
#define PMU_GLB_CLK_FRC_EN_CPU2_VAL(w)   (((w) & 0x00000040) >> 6)
#define PMU_GLB_CLK_FRC_EN_CPU2_SET(w)   w |= ( 0x1 << 6)
#define PMU_GLB_CLK_FRC_EN_CPU2_CLR(w)   w &= ~( 0x1 << 6)

#define PMU_GLB_CLK_FRC_EN_CPU1_POS      5
#define PMU_GLB_CLK_FRC_EN_CPU1_LEN      1
#define PMU_GLB_CLK_FRC_EN_CPU1_MASK     0x00000020
#define PMU_GLB_CLK_FRC_EN_CPU1_VAL(w)   (((w) & 0x00000020) >> 5)
#define PMU_GLB_CLK_FRC_EN_CPU1_SET(w)   w |= ( 0x1 << 5)
#define PMU_GLB_CLK_FRC_EN_CPU1_CLR(w)   w &= ~( 0x1 << 5)

#define PMU_GLB_CLK_FRC_EN_CPU0_POS      4
#define PMU_GLB_CLK_FRC_EN_CPU0_LEN      1
#define PMU_GLB_CLK_FRC_EN_CPU0_MASK     0x00000010
#define PMU_GLB_CLK_FRC_EN_CPU0_VAL(w)   (((w) & 0x00000010) >> 4)
#define PMU_GLB_CLK_FRC_EN_CPU0_SET(w)   w |= ( 0x1 << 4)
#define PMU_GLB_CLK_FRC_EN_CPU0_CLR(w)   w &= ~( 0x1 << 4)

#define PMU_GLB_CLK_FRC_EN_DSP_POS      3
#define PMU_GLB_CLK_FRC_EN_DSP_LEN      1
#define PMU_GLB_CLK_FRC_EN_DSP_MASK     0x00000008
#define PMU_GLB_CLK_FRC_EN_DSP_VAL(w)   (((w) & 0x00000008) >> 3)
#define PMU_GLB_CLK_FRC_EN_DSP_SET(w)   w |= ( 0x1 << 3)
#define PMU_GLB_CLK_FRC_EN_DSP_CLR(w)   w &= ~( 0x1 << 3)

#define PMU_GLB_CLK_FRC_EN_VDEC_POS      2
#define PMU_GLB_CLK_FRC_EN_VDEC_LEN      1
#define PMU_GLB_CLK_FRC_EN_VDEC_MASK     0x00000004
#define PMU_GLB_CLK_FRC_EN_VDEC_VAL(w)   (((w) & 0x00000004) >> 2)
#define PMU_GLB_CLK_FRC_EN_VDEC_SET(w)   w |= ( 0x1 << 2)
#define PMU_GLB_CLK_FRC_EN_VDEC_CLR(w)   w &= ~( 0x1 << 2)

#define PMU_GLB_CLK_FRC_EN_VENC1_POS      1
#define PMU_GLB_CLK_FRC_EN_VENC1_LEN      1
#define PMU_GLB_CLK_FRC_EN_VENC1_MASK     0x00000002
#define PMU_GLB_CLK_FRC_EN_VENC1_VAL(w)   (((w) & 0x00000002) >> 1)
#define PMU_GLB_CLK_FRC_EN_VENC1_SET(w)   w |= ( 0x1 << 1)
#define PMU_GLB_CLK_FRC_EN_VENC1_CLR(w)   w &= ~( 0x1 << 1)

#define PMU_GLB_CLK_FRC_EN_VENC0_POS      0
#define PMU_GLB_CLK_FRC_EN_VENC0_LEN      1
#define PMU_GLB_CLK_FRC_EN_VENC0_MASK     0x00000001
#define PMU_GLB_CLK_FRC_EN_VENC0_VAL(w)   (((w) & 0x00000001) >> 0)
#define PMU_GLB_CLK_FRC_EN_VENC0_SET(w)   w |= ( 0x1 << 0)
#define PMU_GLB_CLK_FRC_EN_VENC0_CLR(w)   w &= ~( 0x1 << 0)

// REGISTER : CLK_FRC_EN1

#define PMU_GLB_CLK_FRC_EN1_ADDR      PMU_GLB_BASE_ADDR + 0x84
#define PMU_GLB_CLK_FRC_EN1_RSTVAL    0x0
#define PMU_GLB_CLK_FRC_EN1_ALL1      0x7FFFF
#define PMU_GLB_CLK_FRC_EN1_ALL0      0x0
#define PMU_GLB_CLK_FRC_EN1_ALL5A     0x25A5A
#define PMU_GLB_CLK_FRC_EN1_ALLA5     0x5A5A5

// FIELD IN REGISTER : CLK_FRC_EN1

#define PMU_GLB_CLK_FRC_EN1_SEN_POS      18
#define PMU_GLB_CLK_FRC_EN1_SEN_LEN      1
#define PMU_GLB_CLK_FRC_EN1_SEN_MASK     0x00040000
#define PMU_GLB_CLK_FRC_EN1_SEN_VAL(w)   (((w) & 0x00040000) >> 18)
#define PMU_GLB_CLK_FRC_EN1_SEN_SET(w)   w |= ( 0x1 << 18)
#define PMU_GLB_CLK_FRC_EN1_SEN_CLR(w)   w &= ~( 0x1 << 18)

#define PMU_GLB_CLK_FRC_EN1_ISP_POS      17
#define PMU_GLB_CLK_FRC_EN1_ISP_LEN      1
#define PMU_GLB_CLK_FRC_EN1_ISP_MASK     0x00020000
#define PMU_GLB_CLK_FRC_EN1_ISP_VAL(w)   (((w) & 0x00020000) >> 17)
#define PMU_GLB_CLK_FRC_EN1_ISP_SET(w)   w |= ( 0x1 << 17)
#define PMU_GLB_CLK_FRC_EN1_ISP_CLR(w)   w &= ~( 0x1 << 17)

#define PMU_GLB_CLK_FRC_EN1_CPU_POS      16
#define PMU_GLB_CLK_FRC_EN1_CPU_LEN      1
#define PMU_GLB_CLK_FRC_EN1_CPU_MASK     0x00010000
#define PMU_GLB_CLK_FRC_EN1_CPU_VAL(w)   (((w) & 0x00010000) >> 16)
#define PMU_GLB_CLK_FRC_EN1_CPU_SET(w)   w |= ( 0x1 << 16)
#define PMU_GLB_CLK_FRC_EN1_CPU_CLR(w)   w &= ~( 0x1 << 16)

#define PMU_GLB_CLK_FRC_EN1_NPU_POS      15
#define PMU_GLB_CLK_FRC_EN1_NPU_LEN      1
#define PMU_GLB_CLK_FRC_EN1_NPU_MASK     0x00008000
#define PMU_GLB_CLK_FRC_EN1_NPU_VAL(w)   (((w) & 0x00008000) >> 15)
#define PMU_GLB_CLK_FRC_EN1_NPU_SET(w)   w |= ( 0x1 << 15)
#define PMU_GLB_CLK_FRC_EN1_NPU_CLR(w)   w &= ~( 0x1 << 15)

#define PMU_GLB_CLK_FRC_EN1_N8_POS      14
#define PMU_GLB_CLK_FRC_EN1_N8_LEN      1
#define PMU_GLB_CLK_FRC_EN1_N8_MASK     0x00004000
#define PMU_GLB_CLK_FRC_EN1_N8_VAL(w)   (((w) & 0x00004000) >> 14)
#define PMU_GLB_CLK_FRC_EN1_N8_SET(w)   w |= ( 0x1 << 14)
#define PMU_GLB_CLK_FRC_EN1_N8_CLR(w)   w &= ~( 0x1 << 14)

#define PMU_GLB_CLK_FRC_EN1_N7_POS      13
#define PMU_GLB_CLK_FRC_EN1_N7_LEN      1
#define PMU_GLB_CLK_FRC_EN1_N7_MASK     0x00002000
#define PMU_GLB_CLK_FRC_EN1_N7_VAL(w)   (((w) & 0x00002000) >> 13)
#define PMU_GLB_CLK_FRC_EN1_N7_SET(w)   w |= ( 0x1 << 13)
#define PMU_GLB_CLK_FRC_EN1_N7_CLR(w)   w &= ~( 0x1 << 13)

#define PMU_GLB_CLK_FRC_EN1_N6_POS      12
#define PMU_GLB_CLK_FRC_EN1_N6_LEN      1
#define PMU_GLB_CLK_FRC_EN1_N6_MASK     0x00001000
#define PMU_GLB_CLK_FRC_EN1_N6_VAL(w)   (((w) & 0x00001000) >> 12)
#define PMU_GLB_CLK_FRC_EN1_N6_SET(w)   w |= ( 0x1 << 12)
#define PMU_GLB_CLK_FRC_EN1_N6_CLR(w)   w &= ~( 0x1 << 12)

#define PMU_GLB_CLK_FRC_EN1_N5_POS      11
#define PMU_GLB_CLK_FRC_EN1_N5_LEN      1
#define PMU_GLB_CLK_FRC_EN1_N5_MASK     0x00000800
#define PMU_GLB_CLK_FRC_EN1_N5_VAL(w)   (((w) & 0x00000800) >> 11)
#define PMU_GLB_CLK_FRC_EN1_N5_SET(w)   w |= ( 0x1 << 11)
#define PMU_GLB_CLK_FRC_EN1_N5_CLR(w)   w &= ~( 0x1 << 11)

#define PMU_GLB_CLK_FRC_EN1_N4_POS      10
#define PMU_GLB_CLK_FRC_EN1_N4_LEN      1
#define PMU_GLB_CLK_FRC_EN1_N4_MASK     0x00000400
#define PMU_GLB_CLK_FRC_EN1_N4_VAL(w)   (((w) & 0x00000400) >> 10)
#define PMU_GLB_CLK_FRC_EN1_N4_SET(w)   w |= ( 0x1 << 10)
#define PMU_GLB_CLK_FRC_EN1_N4_CLR(w)   w &= ~( 0x1 << 10)

#define PMU_GLB_CLK_FRC_EN1_N3_POS      9
#define PMU_GLB_CLK_FRC_EN1_N3_LEN      1
#define PMU_GLB_CLK_FRC_EN1_N3_MASK     0x00000200
#define PMU_GLB_CLK_FRC_EN1_N3_VAL(w)   (((w) & 0x00000200) >> 9)
#define PMU_GLB_CLK_FRC_EN1_N3_SET(w)   w |= ( 0x1 << 9)
#define PMU_GLB_CLK_FRC_EN1_N3_CLR(w)   w &= ~( 0x1 << 9)

#define PMU_GLB_CLK_FRC_EN1_N2_POS      8
#define PMU_GLB_CLK_FRC_EN1_N2_LEN      1
#define PMU_GLB_CLK_FRC_EN1_N2_MASK     0x00000100
#define PMU_GLB_CLK_FRC_EN1_N2_VAL(w)   (((w) & 0x00000100) >> 8)
#define PMU_GLB_CLK_FRC_EN1_N2_SET(w)   w |= ( 0x1 << 8)
#define PMU_GLB_CLK_FRC_EN1_N2_CLR(w)   w &= ~( 0x1 << 8)

#define PMU_GLB_CLK_FRC_EN1_N1_POS      7
#define PMU_GLB_CLK_FRC_EN1_N1_LEN      1
#define PMU_GLB_CLK_FRC_EN1_N1_MASK     0x00000080
#define PMU_GLB_CLK_FRC_EN1_N1_VAL(w)   (((w) & 0x00000080) >> 7)
#define PMU_GLB_CLK_FRC_EN1_N1_SET(w)   w |= ( 0x1 << 7)
#define PMU_GLB_CLK_FRC_EN1_N1_CLR(w)   w &= ~( 0x1 << 7)

#define PMU_GLB_CLK_FRC_EN1_N0_POS      6
#define PMU_GLB_CLK_FRC_EN1_N0_LEN      1
#define PMU_GLB_CLK_FRC_EN1_N0_MASK     0x00000040
#define PMU_GLB_CLK_FRC_EN1_N0_VAL(w)   (((w) & 0x00000040) >> 6)
#define PMU_GLB_CLK_FRC_EN1_N0_SET(w)   w |= ( 0x1 << 6)
#define PMU_GLB_CLK_FRC_EN1_N0_CLR(w)   w &= ~( 0x1 << 6)

#define PMU_GLB_CLK_FRC_EN1_MM_POS      5
#define PMU_GLB_CLK_FRC_EN1_MM_LEN      1
#define PMU_GLB_CLK_FRC_EN1_MM_MASK     0x00000020
#define PMU_GLB_CLK_FRC_EN1_MM_VAL(w)   (((w) & 0x00000020) >> 5)
#define PMU_GLB_CLK_FRC_EN1_MM_SET(w)   w |= ( 0x1 << 5)
#define PMU_GLB_CLK_FRC_EN1_MM_CLR(w)   w &= ~( 0x1 << 5)

#define PMU_GLB_CLK_FRC_EN1_PERIP_POS      4
#define PMU_GLB_CLK_FRC_EN1_PERIP_LEN      1
#define PMU_GLB_CLK_FRC_EN1_PERIP_MASK     0x00000010
#define PMU_GLB_CLK_FRC_EN1_PERIP_VAL(w)   (((w) & 0x00000010) >> 4)
#define PMU_GLB_CLK_FRC_EN1_PERIP_SET(w)   w |= ( 0x1 << 4)
#define PMU_GLB_CLK_FRC_EN1_PERIP_CLR(w)   w &= ~( 0x1 << 4)

#define PMU_GLB_CLK_FRC_EN1_PIPE_POS      3
#define PMU_GLB_CLK_FRC_EN1_PIPE_LEN      1
#define PMU_GLB_CLK_FRC_EN1_PIPE_MASK     0x00000008
#define PMU_GLB_CLK_FRC_EN1_PIPE_VAL(w)   (((w) & 0x00000008) >> 3)
#define PMU_GLB_CLK_FRC_EN1_PIPE_SET(w)   w |= ( 0x1 << 3)
#define PMU_GLB_CLK_FRC_EN1_PIPE_CLR(w)   w &= ~( 0x1 << 3)

#define PMU_GLB_CLK_FRC_EN1_FLASH_POS      2
#define PMU_GLB_CLK_FRC_EN1_FLASH_LEN      1
#define PMU_GLB_CLK_FRC_EN1_FLASH_MASK     0x00000004
#define PMU_GLB_CLK_FRC_EN1_FLASH_VAL(w)   (((w) & 0x00000004) >> 2)
#define PMU_GLB_CLK_FRC_EN1_FLASH_SET(w)   w |= ( 0x1 << 2)
#define PMU_GLB_CLK_FRC_EN1_FLASH_CLR(w)   w &= ~( 0x1 << 2)

#define PMU_GLB_CLK_FRC_EN1_DDR1_POS      1
#define PMU_GLB_CLK_FRC_EN1_DDR1_LEN      1
#define PMU_GLB_CLK_FRC_EN1_DDR1_MASK     0x00000002
#define PMU_GLB_CLK_FRC_EN1_DDR1_VAL(w)   (((w) & 0x00000002) >> 1)
#define PMU_GLB_CLK_FRC_EN1_DDR1_SET(w)   w |= ( 0x1 << 1)
#define PMU_GLB_CLK_FRC_EN1_DDR1_CLR(w)   w &= ~( 0x1 << 1)

#define PMU_GLB_CLK_FRC_EN1_DDR0_POS      0
#define PMU_GLB_CLK_FRC_EN1_DDR0_LEN      1
#define PMU_GLB_CLK_FRC_EN1_DDR0_MASK     0x00000001
#define PMU_GLB_CLK_FRC_EN1_DDR0_VAL(w)   (((w) & 0x00000001) >> 0)
#define PMU_GLB_CLK_FRC_EN1_DDR0_SET(w)   w |= ( 0x1 << 0)
#define PMU_GLB_CLK_FRC_EN1_DDR0_CLR(w)   w &= ~( 0x1 << 0)

// REGISTER : CLK_FRC_SW

#define PMU_GLB_CLK_FRC_SW_ADDR      PMU_GLB_BASE_ADDR + 0x88
#define PMU_GLB_CLK_FRC_SW_RSTVAL    0x0
#define PMU_GLB_CLK_FRC_SW_ALL1      0xFFF
#define PMU_GLB_CLK_FRC_SW_ALL0      0x0
#define PMU_GLB_CLK_FRC_SW_ALL5A     0xA5A
#define PMU_GLB_CLK_FRC_SW_ALLA5     0x5A5

// FIELD IN REGISTER : CLK_FRC_SW

#define PMU_GLB_CLK_FRC_SW_CPU7_POS      11
#define PMU_GLB_CLK_FRC_SW_CPU7_LEN      1
#define PMU_GLB_CLK_FRC_SW_CPU7_MASK     0x00000800
#define PMU_GLB_CLK_FRC_SW_CPU7_VAL(w)   (((w) & 0x00000800) >> 11)
#define PMU_GLB_CLK_FRC_SW_CPU7_SET(w)   w |= ( 0x1 << 11)
#define PMU_GLB_CLK_FRC_SW_CPU7_CLR(w)   w &= ~( 0x1 << 11)

#define PMU_GLB_CLK_FRC_SW_CPU6_POS      10
#define PMU_GLB_CLK_FRC_SW_CPU6_LEN      1
#define PMU_GLB_CLK_FRC_SW_CPU6_MASK     0x00000400
#define PMU_GLB_CLK_FRC_SW_CPU6_VAL(w)   (((w) & 0x00000400) >> 10)
#define PMU_GLB_CLK_FRC_SW_CPU6_SET(w)   w |= ( 0x1 << 10)
#define PMU_GLB_CLK_FRC_SW_CPU6_CLR(w)   w &= ~( 0x1 << 10)

#define PMU_GLB_CLK_FRC_SW_CPU5_POS      9
#define PMU_GLB_CLK_FRC_SW_CPU5_LEN      1
#define PMU_GLB_CLK_FRC_SW_CPU5_MASK     0x00000200
#define PMU_GLB_CLK_FRC_SW_CPU5_VAL(w)   (((w) & 0x00000200) >> 9)
#define PMU_GLB_CLK_FRC_SW_CPU5_SET(w)   w |= ( 0x1 << 9)
#define PMU_GLB_CLK_FRC_SW_CPU5_CLR(w)   w &= ~( 0x1 << 9)

#define PMU_GLB_CLK_FRC_SW_CPU4_POS      8
#define PMU_GLB_CLK_FRC_SW_CPU4_LEN      1
#define PMU_GLB_CLK_FRC_SW_CPU4_MASK     0x00000100
#define PMU_GLB_CLK_FRC_SW_CPU4_VAL(w)   (((w) & 0x00000100) >> 8)
#define PMU_GLB_CLK_FRC_SW_CPU4_SET(w)   w |= ( 0x1 << 8)
#define PMU_GLB_CLK_FRC_SW_CPU4_CLR(w)   w &= ~( 0x1 << 8)

#define PMU_GLB_CLK_FRC_SW_CPU3_POS      7
#define PMU_GLB_CLK_FRC_SW_CPU3_LEN      1
#define PMU_GLB_CLK_FRC_SW_CPU3_MASK     0x00000080
#define PMU_GLB_CLK_FRC_SW_CPU3_VAL(w)   (((w) & 0x00000080) >> 7)
#define PMU_GLB_CLK_FRC_SW_CPU3_SET(w)   w |= ( 0x1 << 7)
#define PMU_GLB_CLK_FRC_SW_CPU3_CLR(w)   w &= ~( 0x1 << 7)

#define PMU_GLB_CLK_FRC_SW_CPU2_POS      6
#define PMU_GLB_CLK_FRC_SW_CPU2_LEN      1
#define PMU_GLB_CLK_FRC_SW_CPU2_MASK     0x00000040
#define PMU_GLB_CLK_FRC_SW_CPU2_VAL(w)   (((w) & 0x00000040) >> 6)
#define PMU_GLB_CLK_FRC_SW_CPU2_SET(w)   w |= ( 0x1 << 6)
#define PMU_GLB_CLK_FRC_SW_CPU2_CLR(w)   w &= ~( 0x1 << 6)

#define PMU_GLB_CLK_FRC_SW_CPU1_POS      5
#define PMU_GLB_CLK_FRC_SW_CPU1_LEN      1
#define PMU_GLB_CLK_FRC_SW_CPU1_MASK     0x00000020
#define PMU_GLB_CLK_FRC_SW_CPU1_VAL(w)   (((w) & 0x00000020) >> 5)
#define PMU_GLB_CLK_FRC_SW_CPU1_SET(w)   w |= ( 0x1 << 5)
#define PMU_GLB_CLK_FRC_SW_CPU1_CLR(w)   w &= ~( 0x1 << 5)

#define PMU_GLB_CLK_FRC_SW_CPU0_POS      4
#define PMU_GLB_CLK_FRC_SW_CPU0_LEN      1
#define PMU_GLB_CLK_FRC_SW_CPU0_MASK     0x00000010
#define PMU_GLB_CLK_FRC_SW_CPU0_VAL(w)   (((w) & 0x00000010) >> 4)
#define PMU_GLB_CLK_FRC_SW_CPU0_SET(w)   w |= ( 0x1 << 4)
#define PMU_GLB_CLK_FRC_SW_CPU0_CLR(w)   w &= ~( 0x1 << 4)

#define PMU_GLB_CLK_FRC_SW_DSP_POS      3
#define PMU_GLB_CLK_FRC_SW_DSP_LEN      1
#define PMU_GLB_CLK_FRC_SW_DSP_MASK     0x00000008
#define PMU_GLB_CLK_FRC_SW_DSP_VAL(w)   (((w) & 0x00000008) >> 3)
#define PMU_GLB_CLK_FRC_SW_DSP_SET(w)   w |= ( 0x1 << 3)
#define PMU_GLB_CLK_FRC_SW_DSP_CLR(w)   w &= ~( 0x1 << 3)

#define PMU_GLB_CLK_FRC_SW_VDEC_POS      2
#define PMU_GLB_CLK_FRC_SW_VDEC_LEN      1
#define PMU_GLB_CLK_FRC_SW_VDEC_MASK     0x00000004
#define PMU_GLB_CLK_FRC_SW_VDEC_VAL(w)   (((w) & 0x00000004) >> 2)
#define PMU_GLB_CLK_FRC_SW_VDEC_SET(w)   w |= ( 0x1 << 2)
#define PMU_GLB_CLK_FRC_SW_VDEC_CLR(w)   w &= ~( 0x1 << 2)

#define PMU_GLB_CLK_FRC_SW_VENC1_POS      1
#define PMU_GLB_CLK_FRC_SW_VENC1_LEN      1
#define PMU_GLB_CLK_FRC_SW_VENC1_MASK     0x00000002
#define PMU_GLB_CLK_FRC_SW_VENC1_VAL(w)   (((w) & 0x00000002) >> 1)
#define PMU_GLB_CLK_FRC_SW_VENC1_SET(w)   w |= ( 0x1 << 1)
#define PMU_GLB_CLK_FRC_SW_VENC1_CLR(w)   w &= ~( 0x1 << 1)

#define PMU_GLB_CLK_FRC_SW_VENC0_POS      0
#define PMU_GLB_CLK_FRC_SW_VENC0_LEN      1
#define PMU_GLB_CLK_FRC_SW_VENC0_MASK     0x00000001
#define PMU_GLB_CLK_FRC_SW_VENC0_VAL(w)   (((w) & 0x00000001) >> 0)
#define PMU_GLB_CLK_FRC_SW_VENC0_SET(w)   w |= ( 0x1 << 0)
#define PMU_GLB_CLK_FRC_SW_VENC0_CLR(w)   w &= ~( 0x1 << 0)

// REGISTER : CLK_FRC_SW1

#define PMU_GLB_CLK_FRC_SW1_ADDR      PMU_GLB_BASE_ADDR + 0x8C
#define PMU_GLB_CLK_FRC_SW1_RSTVAL    0x0
#define PMU_GLB_CLK_FRC_SW1_ALL1      0x7FFFF
#define PMU_GLB_CLK_FRC_SW1_ALL0      0x0
#define PMU_GLB_CLK_FRC_SW1_ALL5A     0x25A5A
#define PMU_GLB_CLK_FRC_SW1_ALLA5     0x5A5A5

// FIELD IN REGISTER : CLK_FRC_SW1

#define PMU_GLB_CLK_FRC_SW1_SEN_POS      18
#define PMU_GLB_CLK_FRC_SW1_SEN_LEN      1
#define PMU_GLB_CLK_FRC_SW1_SEN_MASK     0x00040000
#define PMU_GLB_CLK_FRC_SW1_SEN_VAL(w)   (((w) & 0x00040000) >> 18)
#define PMU_GLB_CLK_FRC_SW1_SEN_SET(w)   w |= ( 0x1 << 18)
#define PMU_GLB_CLK_FRC_SW1_SEN_CLR(w)   w &= ~( 0x1 << 18)

#define PMU_GLB_CLK_FRC_SW1_ISP_POS      17
#define PMU_GLB_CLK_FRC_SW1_ISP_LEN      1
#define PMU_GLB_CLK_FRC_SW1_ISP_MASK     0x00020000
#define PMU_GLB_CLK_FRC_SW1_ISP_VAL(w)   (((w) & 0x00020000) >> 17)
#define PMU_GLB_CLK_FRC_SW1_ISP_SET(w)   w |= ( 0x1 << 17)
#define PMU_GLB_CLK_FRC_SW1_ISP_CLR(w)   w &= ~( 0x1 << 17)

#define PMU_GLB_CLK_FRC_SW1_CPU_POS      16
#define PMU_GLB_CLK_FRC_SW1_CPU_LEN      1
#define PMU_GLB_CLK_FRC_SW1_CPU_MASK     0x00010000
#define PMU_GLB_CLK_FRC_SW1_CPU_VAL(w)   (((w) & 0x00010000) >> 16)
#define PMU_GLB_CLK_FRC_SW1_CPU_SET(w)   w |= ( 0x1 << 16)
#define PMU_GLB_CLK_FRC_SW1_CPU_CLR(w)   w &= ~( 0x1 << 16)

#define PMU_GLB_CLK_FRC_SW1_NPU_POS      15
#define PMU_GLB_CLK_FRC_SW1_NPU_LEN      1
#define PMU_GLB_CLK_FRC_SW1_NPU_MASK     0x00008000
#define PMU_GLB_CLK_FRC_SW1_NPU_VAL(w)   (((w) & 0x00008000) >> 15)
#define PMU_GLB_CLK_FRC_SW1_NPU_SET(w)   w |= ( 0x1 << 15)
#define PMU_GLB_CLK_FRC_SW1_NPU_CLR(w)   w &= ~( 0x1 << 15)

#define PMU_GLB_CLK_FRC_SW1_N8_POS      14
#define PMU_GLB_CLK_FRC_SW1_N8_LEN      1
#define PMU_GLB_CLK_FRC_SW1_N8_MASK     0x00004000
#define PMU_GLB_CLK_FRC_SW1_N8_VAL(w)   (((w) & 0x00004000) >> 14)
#define PMU_GLB_CLK_FRC_SW1_N8_SET(w)   w |= ( 0x1 << 14)
#define PMU_GLB_CLK_FRC_SW1_N8_CLR(w)   w &= ~( 0x1 << 14)

#define PMU_GLB_CLK_FRC_SW1_N7_POS      13
#define PMU_GLB_CLK_FRC_SW1_N7_LEN      1
#define PMU_GLB_CLK_FRC_SW1_N7_MASK     0x00002000
#define PMU_GLB_CLK_FRC_SW1_N7_VAL(w)   (((w) & 0x00002000) >> 13)
#define PMU_GLB_CLK_FRC_SW1_N7_SET(w)   w |= ( 0x1 << 13)
#define PMU_GLB_CLK_FRC_SW1_N7_CLR(w)   w &= ~( 0x1 << 13)

#define PMU_GLB_CLK_FRC_SW1_N6_POS      12
#define PMU_GLB_CLK_FRC_SW1_N6_LEN      1
#define PMU_GLB_CLK_FRC_SW1_N6_MASK     0x00001000
#define PMU_GLB_CLK_FRC_SW1_N6_VAL(w)   (((w) & 0x00001000) >> 12)
#define PMU_GLB_CLK_FRC_SW1_N6_SET(w)   w |= ( 0x1 << 12)
#define PMU_GLB_CLK_FRC_SW1_N6_CLR(w)   w &= ~( 0x1 << 12)

#define PMU_GLB_CLK_FRC_SW1_N5_POS      11
#define PMU_GLB_CLK_FRC_SW1_N5_LEN      1
#define PMU_GLB_CLK_FRC_SW1_N5_MASK     0x00000800
#define PMU_GLB_CLK_FRC_SW1_N5_VAL(w)   (((w) & 0x00000800) >> 11)
#define PMU_GLB_CLK_FRC_SW1_N5_SET(w)   w |= ( 0x1 << 11)
#define PMU_GLB_CLK_FRC_SW1_N5_CLR(w)   w &= ~( 0x1 << 11)

#define PMU_GLB_CLK_FRC_SW1_N4_POS      10
#define PMU_GLB_CLK_FRC_SW1_N4_LEN      1
#define PMU_GLB_CLK_FRC_SW1_N4_MASK     0x00000400
#define PMU_GLB_CLK_FRC_SW1_N4_VAL(w)   (((w) & 0x00000400) >> 10)
#define PMU_GLB_CLK_FRC_SW1_N4_SET(w)   w |= ( 0x1 << 10)
#define PMU_GLB_CLK_FRC_SW1_N4_CLR(w)   w &= ~( 0x1 << 10)

#define PMU_GLB_CLK_FRC_SW1_N3_POS      9
#define PMU_GLB_CLK_FRC_SW1_N3_LEN      1
#define PMU_GLB_CLK_FRC_SW1_N3_MASK     0x00000200
#define PMU_GLB_CLK_FRC_SW1_N3_VAL(w)   (((w) & 0x00000200) >> 9)
#define PMU_GLB_CLK_FRC_SW1_N3_SET(w)   w |= ( 0x1 << 9)
#define PMU_GLB_CLK_FRC_SW1_N3_CLR(w)   w &= ~( 0x1 << 9)

#define PMU_GLB_CLK_FRC_SW1_N2_POS      8
#define PMU_GLB_CLK_FRC_SW1_N2_LEN      1
#define PMU_GLB_CLK_FRC_SW1_N2_MASK     0x00000100
#define PMU_GLB_CLK_FRC_SW1_N2_VAL(w)   (((w) & 0x00000100) >> 8)
#define PMU_GLB_CLK_FRC_SW1_N2_SET(w)   w |= ( 0x1 << 8)
#define PMU_GLB_CLK_FRC_SW1_N2_CLR(w)   w &= ~( 0x1 << 8)

#define PMU_GLB_CLK_FRC_SW1_N1_POS      7
#define PMU_GLB_CLK_FRC_SW1_N1_LEN      1
#define PMU_GLB_CLK_FRC_SW1_N1_MASK     0x00000080
#define PMU_GLB_CLK_FRC_SW1_N1_VAL(w)   (((w) & 0x00000080) >> 7)
#define PMU_GLB_CLK_FRC_SW1_N1_SET(w)   w |= ( 0x1 << 7)
#define PMU_GLB_CLK_FRC_SW1_N1_CLR(w)   w &= ~( 0x1 << 7)

#define PMU_GLB_CLK_FRC_SW1_N0_POS      6
#define PMU_GLB_CLK_FRC_SW1_N0_LEN      1
#define PMU_GLB_CLK_FRC_SW1_N0_MASK     0x00000040
#define PMU_GLB_CLK_FRC_SW1_N0_VAL(w)   (((w) & 0x00000040) >> 6)
#define PMU_GLB_CLK_FRC_SW1_N0_SET(w)   w |= ( 0x1 << 6)
#define PMU_GLB_CLK_FRC_SW1_N0_CLR(w)   w &= ~( 0x1 << 6)

#define PMU_GLB_CLK_FRC_SW1_MM_POS      5
#define PMU_GLB_CLK_FRC_SW1_MM_LEN      1
#define PMU_GLB_CLK_FRC_SW1_MM_MASK     0x00000020
#define PMU_GLB_CLK_FRC_SW1_MM_VAL(w)   (((w) & 0x00000020) >> 5)
#define PMU_GLB_CLK_FRC_SW1_MM_SET(w)   w |= ( 0x1 << 5)
#define PMU_GLB_CLK_FRC_SW1_MM_CLR(w)   w &= ~( 0x1 << 5)

#define PMU_GLB_CLK_FRC_SW1_PERIP_POS      4
#define PMU_GLB_CLK_FRC_SW1_PERIP_LEN      1
#define PMU_GLB_CLK_FRC_SW1_PERIP_MASK     0x00000010
#define PMU_GLB_CLK_FRC_SW1_PERIP_VAL(w)   (((w) & 0x00000010) >> 4)
#define PMU_GLB_CLK_FRC_SW1_PERIP_SET(w)   w |= ( 0x1 << 4)
#define PMU_GLB_CLK_FRC_SW1_PERIP_CLR(w)   w &= ~( 0x1 << 4)

#define PMU_GLB_CLK_FRC_SW1_PIPE_POS      3
#define PMU_GLB_CLK_FRC_SW1_PIPE_LEN      1
#define PMU_GLB_CLK_FRC_SW1_PIPE_MASK     0x00000008
#define PMU_GLB_CLK_FRC_SW1_PIPE_VAL(w)   (((w) & 0x00000008) >> 3)
#define PMU_GLB_CLK_FRC_SW1_PIPE_SET(w)   w |= ( 0x1 << 3)
#define PMU_GLB_CLK_FRC_SW1_PIPE_CLR(w)   w &= ~( 0x1 << 3)

#define PMU_GLB_CLK_FRC_SW1_FLASH_POS      2
#define PMU_GLB_CLK_FRC_SW1_FLASH_LEN      1
#define PMU_GLB_CLK_FRC_SW1_FLASH_MASK     0x00000004
#define PMU_GLB_CLK_FRC_SW1_FLASH_VAL(w)   (((w) & 0x00000004) >> 2)
#define PMU_GLB_CLK_FRC_SW1_FLASH_SET(w)   w |= ( 0x1 << 2)
#define PMU_GLB_CLK_FRC_SW1_FLASH_CLR(w)   w &= ~( 0x1 << 2)

#define PMU_GLB_CLK_FRC_SW1_DDR1_POS      1
#define PMU_GLB_CLK_FRC_SW1_DDR1_LEN      1
#define PMU_GLB_CLK_FRC_SW1_DDR1_MASK     0x00000002
#define PMU_GLB_CLK_FRC_SW1_DDR1_VAL(w)   (((w) & 0x00000002) >> 1)
#define PMU_GLB_CLK_FRC_SW1_DDR1_SET(w)   w |= ( 0x1 << 1)
#define PMU_GLB_CLK_FRC_SW1_DDR1_CLR(w)   w &= ~( 0x1 << 1)

#define PMU_GLB_CLK_FRC_SW1_DDR0_POS      0
#define PMU_GLB_CLK_FRC_SW1_DDR0_LEN      1
#define PMU_GLB_CLK_FRC_SW1_DDR0_MASK     0x00000001
#define PMU_GLB_CLK_FRC_SW1_DDR0_VAL(w)   (((w) & 0x00000001) >> 0)
#define PMU_GLB_CLK_FRC_SW1_DDR0_SET(w)   w |= ( 0x1 << 0)
#define PMU_GLB_CLK_FRC_SW1_DDR0_CLR(w)   w &= ~( 0x1 << 0)

// REGISTER : RST_FRC_EN

#define PMU_GLB_RST_FRC_EN_ADDR      PMU_GLB_BASE_ADDR + 0x90
#define PMU_GLB_RST_FRC_EN_RSTVAL    0x0
#define PMU_GLB_RST_FRC_EN_ALL1      0xFFF
#define PMU_GLB_RST_FRC_EN_ALL0      0x0
#define PMU_GLB_RST_FRC_EN_ALL5A     0xA5A
#define PMU_GLB_RST_FRC_EN_ALLA5     0x5A5

// FIELD IN REGISTER : RST_FRC_EN

#define PMU_GLB_RST_FRC_EN_CPU7_POS      11
#define PMU_GLB_RST_FRC_EN_CPU7_LEN      1
#define PMU_GLB_RST_FRC_EN_CPU7_MASK     0x00000800
#define PMU_GLB_RST_FRC_EN_CPU7_VAL(w)   (((w) & 0x00000800) >> 11)
#define PMU_GLB_RST_FRC_EN_CPU7_SET(w)   w |= ( 0x1 << 11)
#define PMU_GLB_RST_FRC_EN_CPU7_CLR(w)   w &= ~( 0x1 << 11)

#define PMU_GLB_RST_FRC_EN_CPU6_POS      10
#define PMU_GLB_RST_FRC_EN_CPU6_LEN      1
#define PMU_GLB_RST_FRC_EN_CPU6_MASK     0x00000400
#define PMU_GLB_RST_FRC_EN_CPU6_VAL(w)   (((w) & 0x00000400) >> 10)
#define PMU_GLB_RST_FRC_EN_CPU6_SET(w)   w |= ( 0x1 << 10)
#define PMU_GLB_RST_FRC_EN_CPU6_CLR(w)   w &= ~( 0x1 << 10)

#define PMU_GLB_RST_FRC_EN_CPU5_POS      9
#define PMU_GLB_RST_FRC_EN_CPU5_LEN      1
#define PMU_GLB_RST_FRC_EN_CPU5_MASK     0x00000200
#define PMU_GLB_RST_FRC_EN_CPU5_VAL(w)   (((w) & 0x00000200) >> 9)
#define PMU_GLB_RST_FRC_EN_CPU5_SET(w)   w |= ( 0x1 << 9)
#define PMU_GLB_RST_FRC_EN_CPU5_CLR(w)   w &= ~( 0x1 << 9)

#define PMU_GLB_RST_FRC_EN_CPU4_POS      8
#define PMU_GLB_RST_FRC_EN_CPU4_LEN      1
#define PMU_GLB_RST_FRC_EN_CPU4_MASK     0x00000100
#define PMU_GLB_RST_FRC_EN_CPU4_VAL(w)   (((w) & 0x00000100) >> 8)
#define PMU_GLB_RST_FRC_EN_CPU4_SET(w)   w |= ( 0x1 << 8)
#define PMU_GLB_RST_FRC_EN_CPU4_CLR(w)   w &= ~( 0x1 << 8)

#define PMU_GLB_RST_FRC_EN_CPU3_POS      7
#define PMU_GLB_RST_FRC_EN_CPU3_LEN      1
#define PMU_GLB_RST_FRC_EN_CPU3_MASK     0x00000080
#define PMU_GLB_RST_FRC_EN_CPU3_VAL(w)   (((w) & 0x00000080) >> 7)
#define PMU_GLB_RST_FRC_EN_CPU3_SET(w)   w |= ( 0x1 << 7)
#define PMU_GLB_RST_FRC_EN_CPU3_CLR(w)   w &= ~( 0x1 << 7)

#define PMU_GLB_RST_FRC_EN_CPU2_POS      6
#define PMU_GLB_RST_FRC_EN_CPU2_LEN      1
#define PMU_GLB_RST_FRC_EN_CPU2_MASK     0x00000040
#define PMU_GLB_RST_FRC_EN_CPU2_VAL(w)   (((w) & 0x00000040) >> 6)
#define PMU_GLB_RST_FRC_EN_CPU2_SET(w)   w |= ( 0x1 << 6)
#define PMU_GLB_RST_FRC_EN_CPU2_CLR(w)   w &= ~( 0x1 << 6)

#define PMU_GLB_RST_FRC_EN_CPU1_POS      5
#define PMU_GLB_RST_FRC_EN_CPU1_LEN      1
#define PMU_GLB_RST_FRC_EN_CPU1_MASK     0x00000020
#define PMU_GLB_RST_FRC_EN_CPU1_VAL(w)   (((w) & 0x00000020) >> 5)
#define PMU_GLB_RST_FRC_EN_CPU1_SET(w)   w |= ( 0x1 << 5)
#define PMU_GLB_RST_FRC_EN_CPU1_CLR(w)   w &= ~( 0x1 << 5)

#define PMU_GLB_RST_FRC_EN_CPU0_POS      4
#define PMU_GLB_RST_FRC_EN_CPU0_LEN      1
#define PMU_GLB_RST_FRC_EN_CPU0_MASK     0x00000010
#define PMU_GLB_RST_FRC_EN_CPU0_VAL(w)   (((w) & 0x00000010) >> 4)
#define PMU_GLB_RST_FRC_EN_CPU0_SET(w)   w |= ( 0x1 << 4)
#define PMU_GLB_RST_FRC_EN_CPU0_CLR(w)   w &= ~( 0x1 << 4)

#define PMU_GLB_RST_FRC_EN_DSP_POS      3
#define PMU_GLB_RST_FRC_EN_DSP_LEN      1
#define PMU_GLB_RST_FRC_EN_DSP_MASK     0x00000008
#define PMU_GLB_RST_FRC_EN_DSP_VAL(w)   (((w) & 0x00000008) >> 3)
#define PMU_GLB_RST_FRC_EN_DSP_SET(w)   w |= ( 0x1 << 3)
#define PMU_GLB_RST_FRC_EN_DSP_CLR(w)   w &= ~( 0x1 << 3)

#define PMU_GLB_RST_FRC_EN_VDEC_POS      2
#define PMU_GLB_RST_FRC_EN_VDEC_LEN      1
#define PMU_GLB_RST_FRC_EN_VDEC_MASK     0x00000004
#define PMU_GLB_RST_FRC_EN_VDEC_VAL(w)   (((w) & 0x00000004) >> 2)
#define PMU_GLB_RST_FRC_EN_VDEC_SET(w)   w |= ( 0x1 << 2)
#define PMU_GLB_RST_FRC_EN_VDEC_CLR(w)   w &= ~( 0x1 << 2)

#define PMU_GLB_RST_FRC_EN_VENC1_POS      1
#define PMU_GLB_RST_FRC_EN_VENC1_LEN      1
#define PMU_GLB_RST_FRC_EN_VENC1_MASK     0x00000002
#define PMU_GLB_RST_FRC_EN_VENC1_VAL(w)   (((w) & 0x00000002) >> 1)
#define PMU_GLB_RST_FRC_EN_VENC1_SET(w)   w |= ( 0x1 << 1)
#define PMU_GLB_RST_FRC_EN_VENC1_CLR(w)   w &= ~( 0x1 << 1)

#define PMU_GLB_RST_FRC_EN_VENC0_POS      0
#define PMU_GLB_RST_FRC_EN_VENC0_LEN      1
#define PMU_GLB_RST_FRC_EN_VENC0_MASK     0x00000001
#define PMU_GLB_RST_FRC_EN_VENC0_VAL(w)   (((w) & 0x00000001) >> 0)
#define PMU_GLB_RST_FRC_EN_VENC0_SET(w)   w |= ( 0x1 << 0)
#define PMU_GLB_RST_FRC_EN_VENC0_CLR(w)   w &= ~( 0x1 << 0)

// REGISTER : RST_FRC_EN1

#define PMU_GLB_RST_FRC_EN1_ADDR      PMU_GLB_BASE_ADDR + 0x94
#define PMU_GLB_RST_FRC_EN1_RSTVAL    0x0
#define PMU_GLB_RST_FRC_EN1_ALL1      0x7FFFF
#define PMU_GLB_RST_FRC_EN1_ALL0      0x0
#define PMU_GLB_RST_FRC_EN1_ALL5A     0x25A5A
#define PMU_GLB_RST_FRC_EN1_ALLA5     0x5A5A5

// FIELD IN REGISTER : RST_FRC_EN1

#define PMU_GLB_RST_FRC_EN1_SEN_POS      18
#define PMU_GLB_RST_FRC_EN1_SEN_LEN      1
#define PMU_GLB_RST_FRC_EN1_SEN_MASK     0x00040000
#define PMU_GLB_RST_FRC_EN1_SEN_VAL(w)   (((w) & 0x00040000) >> 18)
#define PMU_GLB_RST_FRC_EN1_SEN_SET(w)   w |= ( 0x1 << 18)
#define PMU_GLB_RST_FRC_EN1_SEN_CLR(w)   w &= ~( 0x1 << 18)

#define PMU_GLB_RST_FRC_EN1_ISP_POS      17
#define PMU_GLB_RST_FRC_EN1_ISP_LEN      1
#define PMU_GLB_RST_FRC_EN1_ISP_MASK     0x00020000
#define PMU_GLB_RST_FRC_EN1_ISP_VAL(w)   (((w) & 0x00020000) >> 17)
#define PMU_GLB_RST_FRC_EN1_ISP_SET(w)   w |= ( 0x1 << 17)
#define PMU_GLB_RST_FRC_EN1_ISP_CLR(w)   w &= ~( 0x1 << 17)

#define PMU_GLB_RST_FRC_EN1_CPU_POS      16
#define PMU_GLB_RST_FRC_EN1_CPU_LEN      1
#define PMU_GLB_RST_FRC_EN1_CPU_MASK     0x00010000
#define PMU_GLB_RST_FRC_EN1_CPU_VAL(w)   (((w) & 0x00010000) >> 16)
#define PMU_GLB_RST_FRC_EN1_CPU_SET(w)   w |= ( 0x1 << 16)
#define PMU_GLB_RST_FRC_EN1_CPU_CLR(w)   w &= ~( 0x1 << 16)

#define PMU_GLB_RST_FRC_EN1_NPU_POS      15
#define PMU_GLB_RST_FRC_EN1_NPU_LEN      1
#define PMU_GLB_RST_FRC_EN1_NPU_MASK     0x00008000
#define PMU_GLB_RST_FRC_EN1_NPU_VAL(w)   (((w) & 0x00008000) >> 15)
#define PMU_GLB_RST_FRC_EN1_NPU_SET(w)   w |= ( 0x1 << 15)
#define PMU_GLB_RST_FRC_EN1_NPU_CLR(w)   w &= ~( 0x1 << 15)

#define PMU_GLB_RST_FRC_EN1_N8_POS      14
#define PMU_GLB_RST_FRC_EN1_N8_LEN      1
#define PMU_GLB_RST_FRC_EN1_N8_MASK     0x00004000
#define PMU_GLB_RST_FRC_EN1_N8_VAL(w)   (((w) & 0x00004000) >> 14)
#define PMU_GLB_RST_FRC_EN1_N8_SET(w)   w |= ( 0x1 << 14)
#define PMU_GLB_RST_FRC_EN1_N8_CLR(w)   w &= ~( 0x1 << 14)

#define PMU_GLB_RST_FRC_EN1_N7_POS      13
#define PMU_GLB_RST_FRC_EN1_N7_LEN      1
#define PMU_GLB_RST_FRC_EN1_N7_MASK     0x00002000
#define PMU_GLB_RST_FRC_EN1_N7_VAL(w)   (((w) & 0x00002000) >> 13)
#define PMU_GLB_RST_FRC_EN1_N7_SET(w)   w |= ( 0x1 << 13)
#define PMU_GLB_RST_FRC_EN1_N7_CLR(w)   w &= ~( 0x1 << 13)

#define PMU_GLB_RST_FRC_EN1_N6_POS      12
#define PMU_GLB_RST_FRC_EN1_N6_LEN      1
#define PMU_GLB_RST_FRC_EN1_N6_MASK     0x00001000
#define PMU_GLB_RST_FRC_EN1_N6_VAL(w)   (((w) & 0x00001000) >> 12)
#define PMU_GLB_RST_FRC_EN1_N6_SET(w)   w |= ( 0x1 << 12)
#define PMU_GLB_RST_FRC_EN1_N6_CLR(w)   w &= ~( 0x1 << 12)

#define PMU_GLB_RST_FRC_EN1_N5_POS      11
#define PMU_GLB_RST_FRC_EN1_N5_LEN      1
#define PMU_GLB_RST_FRC_EN1_N5_MASK     0x00000800
#define PMU_GLB_RST_FRC_EN1_N5_VAL(w)   (((w) & 0x00000800) >> 11)
#define PMU_GLB_RST_FRC_EN1_N5_SET(w)   w |= ( 0x1 << 11)
#define PMU_GLB_RST_FRC_EN1_N5_CLR(w)   w &= ~( 0x1 << 11)

#define PMU_GLB_RST_FRC_EN1_N4_POS      10
#define PMU_GLB_RST_FRC_EN1_N4_LEN      1
#define PMU_GLB_RST_FRC_EN1_N4_MASK     0x00000400
#define PMU_GLB_RST_FRC_EN1_N4_VAL(w)   (((w) & 0x00000400) >> 10)
#define PMU_GLB_RST_FRC_EN1_N4_SET(w)   w |= ( 0x1 << 10)
#define PMU_GLB_RST_FRC_EN1_N4_CLR(w)   w &= ~( 0x1 << 10)

#define PMU_GLB_RST_FRC_EN1_N3_POS      9
#define PMU_GLB_RST_FRC_EN1_N3_LEN      1
#define PMU_GLB_RST_FRC_EN1_N3_MASK     0x00000200
#define PMU_GLB_RST_FRC_EN1_N3_VAL(w)   (((w) & 0x00000200) >> 9)
#define PMU_GLB_RST_FRC_EN1_N3_SET(w)   w |= ( 0x1 << 9)
#define PMU_GLB_RST_FRC_EN1_N3_CLR(w)   w &= ~( 0x1 << 9)

#define PMU_GLB_RST_FRC_EN1_N2_POS      8
#define PMU_GLB_RST_FRC_EN1_N2_LEN      1
#define PMU_GLB_RST_FRC_EN1_N2_MASK     0x00000100
#define PMU_GLB_RST_FRC_EN1_N2_VAL(w)   (((w) & 0x00000100) >> 8)
#define PMU_GLB_RST_FRC_EN1_N2_SET(w)   w |= ( 0x1 << 8)
#define PMU_GLB_RST_FRC_EN1_N2_CLR(w)   w &= ~( 0x1 << 8)

#define PMU_GLB_RST_FRC_EN1_N1_POS      7
#define PMU_GLB_RST_FRC_EN1_N1_LEN      1
#define PMU_GLB_RST_FRC_EN1_N1_MASK     0x00000080
#define PMU_GLB_RST_FRC_EN1_N1_VAL(w)   (((w) & 0x00000080) >> 7)
#define PMU_GLB_RST_FRC_EN1_N1_SET(w)   w |= ( 0x1 << 7)
#define PMU_GLB_RST_FRC_EN1_N1_CLR(w)   w &= ~( 0x1 << 7)

#define PMU_GLB_RST_FRC_EN1_N0_POS      6
#define PMU_GLB_RST_FRC_EN1_N0_LEN      1
#define PMU_GLB_RST_FRC_EN1_N0_MASK     0x00000040
#define PMU_GLB_RST_FRC_EN1_N0_VAL(w)   (((w) & 0x00000040) >> 6)
#define PMU_GLB_RST_FRC_EN1_N0_SET(w)   w |= ( 0x1 << 6)
#define PMU_GLB_RST_FRC_EN1_N0_CLR(w)   w &= ~( 0x1 << 6)

#define PMU_GLB_RST_FRC_EN1_MM_POS      5
#define PMU_GLB_RST_FRC_EN1_MM_LEN      1
#define PMU_GLB_RST_FRC_EN1_MM_MASK     0x00000020
#define PMU_GLB_RST_FRC_EN1_MM_VAL(w)   (((w) & 0x00000020) >> 5)
#define PMU_GLB_RST_FRC_EN1_MM_SET(w)   w |= ( 0x1 << 5)
#define PMU_GLB_RST_FRC_EN1_MM_CLR(w)   w &= ~( 0x1 << 5)

#define PMU_GLB_RST_FRC_EN1_PERIP_POS      4
#define PMU_GLB_RST_FRC_EN1_PERIP_LEN      1
#define PMU_GLB_RST_FRC_EN1_PERIP_MASK     0x00000010
#define PMU_GLB_RST_FRC_EN1_PERIP_VAL(w)   (((w) & 0x00000010) >> 4)
#define PMU_GLB_RST_FRC_EN1_PERIP_SET(w)   w |= ( 0x1 << 4)
#define PMU_GLB_RST_FRC_EN1_PERIP_CLR(w)   w &= ~( 0x1 << 4)

#define PMU_GLB_RST_FRC_EN1_PIPE_POS      3
#define PMU_GLB_RST_FRC_EN1_PIPE_LEN      1
#define PMU_GLB_RST_FRC_EN1_PIPE_MASK     0x00000008
#define PMU_GLB_RST_FRC_EN1_PIPE_VAL(w)   (((w) & 0x00000008) >> 3)
#define PMU_GLB_RST_FRC_EN1_PIPE_SET(w)   w |= ( 0x1 << 3)
#define PMU_GLB_RST_FRC_EN1_PIPE_CLR(w)   w &= ~( 0x1 << 3)

#define PMU_GLB_RST_FRC_EN1_FLASH_POS      2
#define PMU_GLB_RST_FRC_EN1_FLASH_LEN      1
#define PMU_GLB_RST_FRC_EN1_FLASH_MASK     0x00000004
#define PMU_GLB_RST_FRC_EN1_FLASH_VAL(w)   (((w) & 0x00000004) >> 2)
#define PMU_GLB_RST_FRC_EN1_FLASH_SET(w)   w |= ( 0x1 << 2)
#define PMU_GLB_RST_FRC_EN1_FLASH_CLR(w)   w &= ~( 0x1 << 2)

#define PMU_GLB_RST_FRC_EN1_DDR1_POS      1
#define PMU_GLB_RST_FRC_EN1_DDR1_LEN      1
#define PMU_GLB_RST_FRC_EN1_DDR1_MASK     0x00000002
#define PMU_GLB_RST_FRC_EN1_DDR1_VAL(w)   (((w) & 0x00000002) >> 1)
#define PMU_GLB_RST_FRC_EN1_DDR1_SET(w)   w |= ( 0x1 << 1)
#define PMU_GLB_RST_FRC_EN1_DDR1_CLR(w)   w &= ~( 0x1 << 1)

#define PMU_GLB_RST_FRC_EN1_DDR0_POS      0
#define PMU_GLB_RST_FRC_EN1_DDR0_LEN      1
#define PMU_GLB_RST_FRC_EN1_DDR0_MASK     0x00000001
#define PMU_GLB_RST_FRC_EN1_DDR0_VAL(w)   (((w) & 0x00000001) >> 0)
#define PMU_GLB_RST_FRC_EN1_DDR0_SET(w)   w |= ( 0x1 << 0)
#define PMU_GLB_RST_FRC_EN1_DDR0_CLR(w)   w &= ~( 0x1 << 0)

// REGISTER : RST_FRC_SW

#define PMU_GLB_RST_FRC_SW_ADDR      PMU_GLB_BASE_ADDR + 0x98
#define PMU_GLB_RST_FRC_SW_RSTVAL    0x0
#define PMU_GLB_RST_FRC_SW_ALL1      0xFFF
#define PMU_GLB_RST_FRC_SW_ALL0      0x0
#define PMU_GLB_RST_FRC_SW_ALL5A     0xA5A
#define PMU_GLB_RST_FRC_SW_ALLA5     0x5A5

// FIELD IN REGISTER : RST_FRC_SW

#define PMU_GLB_RST_FRC_SW_CPU7_POS      11
#define PMU_GLB_RST_FRC_SW_CPU7_LEN      1
#define PMU_GLB_RST_FRC_SW_CPU7_MASK     0x00000800
#define PMU_GLB_RST_FRC_SW_CPU7_VAL(w)   (((w) & 0x00000800) >> 11)
#define PMU_GLB_RST_FRC_SW_CPU7_SET(w)   w |= ( 0x1 << 11)
#define PMU_GLB_RST_FRC_SW_CPU7_CLR(w)   w &= ~( 0x1 << 11)

#define PMU_GLB_RST_FRC_SW_CPU6_POS      10
#define PMU_GLB_RST_FRC_SW_CPU6_LEN      1
#define PMU_GLB_RST_FRC_SW_CPU6_MASK     0x00000400
#define PMU_GLB_RST_FRC_SW_CPU6_VAL(w)   (((w) & 0x00000400) >> 10)
#define PMU_GLB_RST_FRC_SW_CPU6_SET(w)   w |= ( 0x1 << 10)
#define PMU_GLB_RST_FRC_SW_CPU6_CLR(w)   w &= ~( 0x1 << 10)

#define PMU_GLB_RST_FRC_SW_CPU5_POS      9
#define PMU_GLB_RST_FRC_SW_CPU5_LEN      1
#define PMU_GLB_RST_FRC_SW_CPU5_MASK     0x00000200
#define PMU_GLB_RST_FRC_SW_CPU5_VAL(w)   (((w) & 0x00000200) >> 9)
#define PMU_GLB_RST_FRC_SW_CPU5_SET(w)   w |= ( 0x1 << 9)
#define PMU_GLB_RST_FRC_SW_CPU5_CLR(w)   w &= ~( 0x1 << 9)

#define PMU_GLB_RST_FRC_SW_CPU4_POS      8
#define PMU_GLB_RST_FRC_SW_CPU4_LEN      1
#define PMU_GLB_RST_FRC_SW_CPU4_MASK     0x00000100
#define PMU_GLB_RST_FRC_SW_CPU4_VAL(w)   (((w) & 0x00000100) >> 8)
#define PMU_GLB_RST_FRC_SW_CPU4_SET(w)   w |= ( 0x1 << 8)
#define PMU_GLB_RST_FRC_SW_CPU4_CLR(w)   w &= ~( 0x1 << 8)

#define PMU_GLB_RST_FRC_SW_CPU3_POS      7
#define PMU_GLB_RST_FRC_SW_CPU3_LEN      1
#define PMU_GLB_RST_FRC_SW_CPU3_MASK     0x00000080
#define PMU_GLB_RST_FRC_SW_CPU3_VAL(w)   (((w) & 0x00000080) >> 7)
#define PMU_GLB_RST_FRC_SW_CPU3_SET(w)   w |= ( 0x1 << 7)
#define PMU_GLB_RST_FRC_SW_CPU3_CLR(w)   w &= ~( 0x1 << 7)

#define PMU_GLB_RST_FRC_SW_CPU2_POS      6
#define PMU_GLB_RST_FRC_SW_CPU2_LEN      1
#define PMU_GLB_RST_FRC_SW_CPU2_MASK     0x00000040
#define PMU_GLB_RST_FRC_SW_CPU2_VAL(w)   (((w) & 0x00000040) >> 6)
#define PMU_GLB_RST_FRC_SW_CPU2_SET(w)   w |= ( 0x1 << 6)
#define PMU_GLB_RST_FRC_SW_CPU2_CLR(w)   w &= ~( 0x1 << 6)

#define PMU_GLB_RST_FRC_SW_CPU1_POS      5
#define PMU_GLB_RST_FRC_SW_CPU1_LEN      1
#define PMU_GLB_RST_FRC_SW_CPU1_MASK     0x00000020
#define PMU_GLB_RST_FRC_SW_CPU1_VAL(w)   (((w) & 0x00000020) >> 5)
#define PMU_GLB_RST_FRC_SW_CPU1_SET(w)   w |= ( 0x1 << 5)
#define PMU_GLB_RST_FRC_SW_CPU1_CLR(w)   w &= ~( 0x1 << 5)

#define PMU_GLB_RST_FRC_SW_CPU0_POS      4
#define PMU_GLB_RST_FRC_SW_CPU0_LEN      1
#define PMU_GLB_RST_FRC_SW_CPU0_MASK     0x00000010
#define PMU_GLB_RST_FRC_SW_CPU0_VAL(w)   (((w) & 0x00000010) >> 4)
#define PMU_GLB_RST_FRC_SW_CPU0_SET(w)   w |= ( 0x1 << 4)
#define PMU_GLB_RST_FRC_SW_CPU0_CLR(w)   w &= ~( 0x1 << 4)

#define PMU_GLB_RST_FRC_SW_DSP_POS      3
#define PMU_GLB_RST_FRC_SW_DSP_LEN      1
#define PMU_GLB_RST_FRC_SW_DSP_MASK     0x00000008
#define PMU_GLB_RST_FRC_SW_DSP_VAL(w)   (((w) & 0x00000008) >> 3)
#define PMU_GLB_RST_FRC_SW_DSP_SET(w)   w |= ( 0x1 << 3)
#define PMU_GLB_RST_FRC_SW_DSP_CLR(w)   w &= ~( 0x1 << 3)

#define PMU_GLB_RST_FRC_SW_VDEC_POS      2
#define PMU_GLB_RST_FRC_SW_VDEC_LEN      1
#define PMU_GLB_RST_FRC_SW_VDEC_MASK     0x00000004
#define PMU_GLB_RST_FRC_SW_VDEC_VAL(w)   (((w) & 0x00000004) >> 2)
#define PMU_GLB_RST_FRC_SW_VDEC_SET(w)   w |= ( 0x1 << 2)
#define PMU_GLB_RST_FRC_SW_VDEC_CLR(w)   w &= ~( 0x1 << 2)

#define PMU_GLB_RST_FRC_SW_VENC1_POS      1
#define PMU_GLB_RST_FRC_SW_VENC1_LEN      1
#define PMU_GLB_RST_FRC_SW_VENC1_MASK     0x00000002
#define PMU_GLB_RST_FRC_SW_VENC1_VAL(w)   (((w) & 0x00000002) >> 1)
#define PMU_GLB_RST_FRC_SW_VENC1_SET(w)   w |= ( 0x1 << 1)
#define PMU_GLB_RST_FRC_SW_VENC1_CLR(w)   w &= ~( 0x1 << 1)

#define PMU_GLB_RST_FRC_SW_VENC0_POS      0
#define PMU_GLB_RST_FRC_SW_VENC0_LEN      1
#define PMU_GLB_RST_FRC_SW_VENC0_MASK     0x00000001
#define PMU_GLB_RST_FRC_SW_VENC0_VAL(w)   (((w) & 0x00000001) >> 0)
#define PMU_GLB_RST_FRC_SW_VENC0_SET(w)   w |= ( 0x1 << 0)
#define PMU_GLB_RST_FRC_SW_VENC0_CLR(w)   w &= ~( 0x1 << 0)

// REGISTER : RST_FRC_SW1

#define PMU_GLB_RST_FRC_SW1_ADDR      PMU_GLB_BASE_ADDR + 0x9C
#define PMU_GLB_RST_FRC_SW1_RSTVAL    0x0
#define PMU_GLB_RST_FRC_SW1_ALL1      0x7FFFF
#define PMU_GLB_RST_FRC_SW1_ALL0      0x0
#define PMU_GLB_RST_FRC_SW1_ALL5A     0x25A5A
#define PMU_GLB_RST_FRC_SW1_ALLA5     0x5A5A5

// FIELD IN REGISTER : RST_FRC_SW1

#define PMU_GLB_RST_FRC_SW1_SEN_POS      18
#define PMU_GLB_RST_FRC_SW1_SEN_LEN      1
#define PMU_GLB_RST_FRC_SW1_SEN_MASK     0x00040000
#define PMU_GLB_RST_FRC_SW1_SEN_VAL(w)   (((w) & 0x00040000) >> 18)
#define PMU_GLB_RST_FRC_SW1_SEN_SET(w)   w |= ( 0x1 << 18)
#define PMU_GLB_RST_FRC_SW1_SEN_CLR(w)   w &= ~( 0x1 << 18)

#define PMU_GLB_RST_FRC_SW1_ISP_POS      17
#define PMU_GLB_RST_FRC_SW1_ISP_LEN      1
#define PMU_GLB_RST_FRC_SW1_ISP_MASK     0x00020000
#define PMU_GLB_RST_FRC_SW1_ISP_VAL(w)   (((w) & 0x00020000) >> 17)
#define PMU_GLB_RST_FRC_SW1_ISP_SET(w)   w |= ( 0x1 << 17)
#define PMU_GLB_RST_FRC_SW1_ISP_CLR(w)   w &= ~( 0x1 << 17)

#define PMU_GLB_RST_FRC_SW1_CPU_POS      16
#define PMU_GLB_RST_FRC_SW1_CPU_LEN      1
#define PMU_GLB_RST_FRC_SW1_CPU_MASK     0x00010000
#define PMU_GLB_RST_FRC_SW1_CPU_VAL(w)   (((w) & 0x00010000) >> 16)
#define PMU_GLB_RST_FRC_SW1_CPU_SET(w)   w |= ( 0x1 << 16)
#define PMU_GLB_RST_FRC_SW1_CPU_CLR(w)   w &= ~( 0x1 << 16)

#define PMU_GLB_RST_FRC_SW1_NPU_POS      15
#define PMU_GLB_RST_FRC_SW1_NPU_LEN      1
#define PMU_GLB_RST_FRC_SW1_NPU_MASK     0x00008000
#define PMU_GLB_RST_FRC_SW1_NPU_VAL(w)   (((w) & 0x00008000) >> 15)
#define PMU_GLB_RST_FRC_SW1_NPU_SET(w)   w |= ( 0x1 << 15)
#define PMU_GLB_RST_FRC_SW1_NPU_CLR(w)   w &= ~( 0x1 << 15)

#define PMU_GLB_RST_FRC_SW1_N8_POS      14
#define PMU_GLB_RST_FRC_SW1_N8_LEN      1
#define PMU_GLB_RST_FRC_SW1_N8_MASK     0x00004000
#define PMU_GLB_RST_FRC_SW1_N8_VAL(w)   (((w) & 0x00004000) >> 14)
#define PMU_GLB_RST_FRC_SW1_N8_SET(w)   w |= ( 0x1 << 14)
#define PMU_GLB_RST_FRC_SW1_N8_CLR(w)   w &= ~( 0x1 << 14)

#define PMU_GLB_RST_FRC_SW1_N7_POS      13
#define PMU_GLB_RST_FRC_SW1_N7_LEN      1
#define PMU_GLB_RST_FRC_SW1_N7_MASK     0x00002000
#define PMU_GLB_RST_FRC_SW1_N7_VAL(w)   (((w) & 0x00002000) >> 13)
#define PMU_GLB_RST_FRC_SW1_N7_SET(w)   w |= ( 0x1 << 13)
#define PMU_GLB_RST_FRC_SW1_N7_CLR(w)   w &= ~( 0x1 << 13)

#define PMU_GLB_RST_FRC_SW1_N6_POS      12
#define PMU_GLB_RST_FRC_SW1_N6_LEN      1
#define PMU_GLB_RST_FRC_SW1_N6_MASK     0x00001000
#define PMU_GLB_RST_FRC_SW1_N6_VAL(w)   (((w) & 0x00001000) >> 12)
#define PMU_GLB_RST_FRC_SW1_N6_SET(w)   w |= ( 0x1 << 12)
#define PMU_GLB_RST_FRC_SW1_N6_CLR(w)   w &= ~( 0x1 << 12)

#define PMU_GLB_RST_FRC_SW1_N5_POS      11
#define PMU_GLB_RST_FRC_SW1_N5_LEN      1
#define PMU_GLB_RST_FRC_SW1_N5_MASK     0x00000800
#define PMU_GLB_RST_FRC_SW1_N5_VAL(w)   (((w) & 0x00000800) >> 11)
#define PMU_GLB_RST_FRC_SW1_N5_SET(w)   w |= ( 0x1 << 11)
#define PMU_GLB_RST_FRC_SW1_N5_CLR(w)   w &= ~( 0x1 << 11)

#define PMU_GLB_RST_FRC_SW1_N4_POS      10
#define PMU_GLB_RST_FRC_SW1_N4_LEN      1
#define PMU_GLB_RST_FRC_SW1_N4_MASK     0x00000400
#define PMU_GLB_RST_FRC_SW1_N4_VAL(w)   (((w) & 0x00000400) >> 10)
#define PMU_GLB_RST_FRC_SW1_N4_SET(w)   w |= ( 0x1 << 10)
#define PMU_GLB_RST_FRC_SW1_N4_CLR(w)   w &= ~( 0x1 << 10)

#define PMU_GLB_RST_FRC_SW1_N3_POS      9
#define PMU_GLB_RST_FRC_SW1_N3_LEN      1
#define PMU_GLB_RST_FRC_SW1_N3_MASK     0x00000200
#define PMU_GLB_RST_FRC_SW1_N3_VAL(w)   (((w) & 0x00000200) >> 9)
#define PMU_GLB_RST_FRC_SW1_N3_SET(w)   w |= ( 0x1 << 9)
#define PMU_GLB_RST_FRC_SW1_N3_CLR(w)   w &= ~( 0x1 << 9)

#define PMU_GLB_RST_FRC_SW1_N2_POS      8
#define PMU_GLB_RST_FRC_SW1_N2_LEN      1
#define PMU_GLB_RST_FRC_SW1_N2_MASK     0x00000100
#define PMU_GLB_RST_FRC_SW1_N2_VAL(w)   (((w) & 0x00000100) >> 8)
#define PMU_GLB_RST_FRC_SW1_N2_SET(w)   w |= ( 0x1 << 8)
#define PMU_GLB_RST_FRC_SW1_N2_CLR(w)   w &= ~( 0x1 << 8)

#define PMU_GLB_RST_FRC_SW1_N1_POS      7
#define PMU_GLB_RST_FRC_SW1_N1_LEN      1
#define PMU_GLB_RST_FRC_SW1_N1_MASK     0x00000080
#define PMU_GLB_RST_FRC_SW1_N1_VAL(w)   (((w) & 0x00000080) >> 7)
#define PMU_GLB_RST_FRC_SW1_N1_SET(w)   w |= ( 0x1 << 7)
#define PMU_GLB_RST_FRC_SW1_N1_CLR(w)   w &= ~( 0x1 << 7)

#define PMU_GLB_RST_FRC_SW1_N0_POS      6
#define PMU_GLB_RST_FRC_SW1_N0_LEN      1
#define PMU_GLB_RST_FRC_SW1_N0_MASK     0x00000040
#define PMU_GLB_RST_FRC_SW1_N0_VAL(w)   (((w) & 0x00000040) >> 6)
#define PMU_GLB_RST_FRC_SW1_N0_SET(w)   w |= ( 0x1 << 6)
#define PMU_GLB_RST_FRC_SW1_N0_CLR(w)   w &= ~( 0x1 << 6)

#define PMU_GLB_RST_FRC_SW1_MM_POS      5
#define PMU_GLB_RST_FRC_SW1_MM_LEN      1
#define PMU_GLB_RST_FRC_SW1_MM_MASK     0x00000020
#define PMU_GLB_RST_FRC_SW1_MM_VAL(w)   (((w) & 0x00000020) >> 5)
#define PMU_GLB_RST_FRC_SW1_MM_SET(w)   w |= ( 0x1 << 5)
#define PMU_GLB_RST_FRC_SW1_MM_CLR(w)   w &= ~( 0x1 << 5)

#define PMU_GLB_RST_FRC_SW1_PERIP_POS      4
#define PMU_GLB_RST_FRC_SW1_PERIP_LEN      1
#define PMU_GLB_RST_FRC_SW1_PERIP_MASK     0x00000010
#define PMU_GLB_RST_FRC_SW1_PERIP_VAL(w)   (((w) & 0x00000010) >> 4)
#define PMU_GLB_RST_FRC_SW1_PERIP_SET(w)   w |= ( 0x1 << 4)
#define PMU_GLB_RST_FRC_SW1_PERIP_CLR(w)   w &= ~( 0x1 << 4)

#define PMU_GLB_RST_FRC_SW1_PIPE_POS      3
#define PMU_GLB_RST_FRC_SW1_PIPE_LEN      1
#define PMU_GLB_RST_FRC_SW1_PIPE_MASK     0x00000008
#define PMU_GLB_RST_FRC_SW1_PIPE_VAL(w)   (((w) & 0x00000008) >> 3)
#define PMU_GLB_RST_FRC_SW1_PIPE_SET(w)   w |= ( 0x1 << 3)
#define PMU_GLB_RST_FRC_SW1_PIPE_CLR(w)   w &= ~( 0x1 << 3)

#define PMU_GLB_RST_FRC_SW1_FLASH_POS      2
#define PMU_GLB_RST_FRC_SW1_FLASH_LEN      1
#define PMU_GLB_RST_FRC_SW1_FLASH_MASK     0x00000004
#define PMU_GLB_RST_FRC_SW1_FLASH_VAL(w)   (((w) & 0x00000004) >> 2)
#define PMU_GLB_RST_FRC_SW1_FLASH_SET(w)   w |= ( 0x1 << 2)
#define PMU_GLB_RST_FRC_SW1_FLASH_CLR(w)   w &= ~( 0x1 << 2)

#define PMU_GLB_RST_FRC_SW1_DDR1_POS      1
#define PMU_GLB_RST_FRC_SW1_DDR1_LEN      1
#define PMU_GLB_RST_FRC_SW1_DDR1_MASK     0x00000002
#define PMU_GLB_RST_FRC_SW1_DDR1_VAL(w)   (((w) & 0x00000002) >> 1)
#define PMU_GLB_RST_FRC_SW1_DDR1_SET(w)   w |= ( 0x1 << 1)
#define PMU_GLB_RST_FRC_SW1_DDR1_CLR(w)   w &= ~( 0x1 << 1)

#define PMU_GLB_RST_FRC_SW1_DDR0_POS      0
#define PMU_GLB_RST_FRC_SW1_DDR0_LEN      1
#define PMU_GLB_RST_FRC_SW1_DDR0_MASK     0x00000001
#define PMU_GLB_RST_FRC_SW1_DDR0_VAL(w)   (((w) & 0x00000001) >> 0)
#define PMU_GLB_RST_FRC_SW1_DDR0_SET(w)   w |= ( 0x1 << 0)
#define PMU_GLB_RST_FRC_SW1_DDR0_CLR(w)   w &= ~( 0x1 << 0)

// REGISTER : LIGHT_SLEEP_FRC

#define PMU_GLB_LIGHT_SLEEP_FRC_ADDR      PMU_GLB_BASE_ADDR + 0xA0
#define PMU_GLB_LIGHT_SLEEP_FRC_RSTVAL    0x0
#define PMU_GLB_LIGHT_SLEEP_FRC_ALL1      0x1
#define PMU_GLB_LIGHT_SLEEP_FRC_ALL0      0x0
#define PMU_GLB_LIGHT_SLEEP_FRC_ALL5A     0x0
#define PMU_GLB_LIGHT_SLEEP_FRC_ALLA5     0x1

// FIELD IN REGISTER : LIGHT_SLEEP_FRC

#define PMU_GLB_LIGHT_SLEEP_FRC_EN_POS      0
#define PMU_GLB_LIGHT_SLEEP_FRC_EN_LEN      1
#define PMU_GLB_LIGHT_SLEEP_FRC_EN_MASK     0x00000001
#define PMU_GLB_LIGHT_SLEEP_FRC_EN_VAL(w)   (((w) & 0x00000001) >> 0)
#define PMU_GLB_LIGHT_SLEEP_FRC_EN_SET(w)   w |= ( 0x1 << 0)
#define PMU_GLB_LIGHT_SLEEP_FRC_EN_CLR(w)   w &= ~( 0x1 << 0)

// REGISTER : LIGHT_SLEEP_SW

#define PMU_GLB_LIGHT_SLEEP_SW_ADDR      PMU_GLB_BASE_ADDR + 0xA4
#define PMU_GLB_LIGHT_SLEEP_SW_RSTVAL    0x0
#define PMU_GLB_LIGHT_SLEEP_SW_ALL1      0x1
#define PMU_GLB_LIGHT_SLEEP_SW_ALL0      0x0
#define PMU_GLB_LIGHT_SLEEP_SW_ALL5A     0x0
#define PMU_GLB_LIGHT_SLEEP_SW_ALLA5     0x1

// FIELD IN REGISTER : LIGHT_SLEEP_SW

#define PMU_GLB_LIGHT_SLEEP_SW_EN_POS      0
#define PMU_GLB_LIGHT_SLEEP_SW_EN_LEN      1
#define PMU_GLB_LIGHT_SLEEP_SW_EN_MASK     0x00000001
#define PMU_GLB_LIGHT_SLEEP_SW_EN_VAL(w)   (((w) & 0x00000001) >> 0)
#define PMU_GLB_LIGHT_SLEEP_SW_EN_SET(w)   w |= ( 0x1 << 0)
#define PMU_GLB_LIGHT_SLEEP_SW_EN_CLR(w)   w &= ~( 0x1 << 0)

// REGISTER : MEM_REPAIRE

#define PMU_GLB_MEM_REPAIRE_ADDR      PMU_GLB_BASE_ADDR + 0xA8
#define PMU_GLB_MEM_REPAIRE_RSTVAL    0x0
#define PMU_GLB_MEM_REPAIRE_ALL1      0x3
#define PMU_GLB_MEM_REPAIRE_ALL0      0x0
#define PMU_GLB_MEM_REPAIRE_ALL5A     0x2
#define PMU_GLB_MEM_REPAIRE_ALLA5     0x1

// FIELD IN REGISTER : MEM_REPAIRE

#define PMU_GLB_MEM_REPAIRE_BIHR_POS      1
#define PMU_GLB_MEM_REPAIRE_BIHR_LEN      1
#define PMU_GLB_MEM_REPAIRE_BIHR_MASK     0x00000002
#define PMU_GLB_MEM_REPAIRE_BIHR_VAL(w)   (((w) & 0x00000002) >> 1)
#define PMU_GLB_MEM_REPAIRE_BIHR_SET(w)   w |= ( 0x1 << 1)
#define PMU_GLB_MEM_REPAIRE_BIHR_CLR(w)   w &= ~( 0x1 << 1)

#define PMU_GLB_MEM_REPAIRE_BISR_POS      0
#define PMU_GLB_MEM_REPAIRE_BISR_LEN      1
#define PMU_GLB_MEM_REPAIRE_BISR_MASK     0x00000001
#define PMU_GLB_MEM_REPAIRE_BISR_VAL(w)   (((w) & 0x00000001) >> 0)
#define PMU_GLB_MEM_REPAIRE_BISR_SET(w)   w |= ( 0x1 << 0)
#define PMU_GLB_MEM_REPAIRE_BISR_CLR(w)   w &= ~( 0x1 << 0)

// REGISTER : INT_MASK

#define PMU_GLB_INT_MASK_ADDR      PMU_GLB_BASE_ADDR + 0xAC
#define PMU_GLB_INT_MASK_RSTVAL    0x0
#define PMU_GLB_INT_MASK_ALL1      0xFFFFFFFF
#define PMU_GLB_INT_MASK_ALL0      0x0
#define PMU_GLB_INT_MASK_ALL5A     0x5A5A5A5A
#define PMU_GLB_INT_MASK_ALLA5     0xA5A5A5A5

// FIELD IN REGISTER : INT_MASK

#define PMU_GLB_INT_MASK_VAL_POS      0
#define PMU_GLB_INT_MASK_VAL_LEN      32
#define PMU_GLB_INT_MASK_VAL_MASK     0xFFFFFFFF
#define PMU_GLB_INT_MASK_VAL_VAL(w)   (((w) & 0xFFFFFFFF) >> 0)
#define PMU_GLB_INT_MASK_VAL_SET(w)   w |= ( 0xFFFFFFFF << 0)
#define PMU_GLB_INT_MASK_VAL_CLR(w)   w &= ~( 0xFFFFFFFF << 0)

// REGISTER : INT_MASK1

#define PMU_GLB_INT_MASK1_ADDR      PMU_GLB_BASE_ADDR + 0xB0
#define PMU_GLB_INT_MASK1_RSTVAL    0x0
#define PMU_GLB_INT_MASK1_ALL1      0xFFFFFFFF
#define PMU_GLB_INT_MASK1_ALL0      0x0
#define PMU_GLB_INT_MASK1_ALL5A     0x5A5A5A5A
#define PMU_GLB_INT_MASK1_ALLA5     0xA5A5A5A5

// FIELD IN REGISTER : INT_MASK1

#define PMU_GLB_INT_MASK1_VAL_POS      0
#define PMU_GLB_INT_MASK1_VAL_LEN      32
#define PMU_GLB_INT_MASK1_VAL_MASK     0xFFFFFFFF
#define PMU_GLB_INT_MASK1_VAL_VAL(w)   (((w) & 0xFFFFFFFF) >> 0)
#define PMU_GLB_INT_MASK1_VAL_SET(w)   w |= ( 0xFFFFFFFF << 0)
#define PMU_GLB_INT_MASK1_VAL_CLR(w)   w &= ~( 0xFFFFFFFF << 0)

// REGISTER : INT_MASK2

#define PMU_GLB_INT_MASK2_ADDR      PMU_GLB_BASE_ADDR + 0xB4
#define PMU_GLB_INT_MASK2_RSTVAL    0x0
#define PMU_GLB_INT_MASK2_ALL1      0xF
#define PMU_GLB_INT_MASK2_ALL0      0x0
#define PMU_GLB_INT_MASK2_ALL5A     0xA
#define PMU_GLB_INT_MASK2_ALLA5     0x5

// FIELD IN REGISTER : INT_MASK2

#define PMU_GLB_INT_MASK2_VAL_POS      0
#define PMU_GLB_INT_MASK2_VAL_LEN      4
#define PMU_GLB_INT_MASK2_VAL_MASK     0x0000000F
#define PMU_GLB_INT_MASK2_VAL_VAL(w)   (((w) & 0x0000000F) >> 0)
#define PMU_GLB_INT_MASK2_VAL_SET(w)   w |= ( 0xF << 0)
#define PMU_GLB_INT_MASK2_VAL_CLR(w)   w &= ~( 0xF << 0)

// REGISTER : INT_CLR

#define PMU_GLB_INT_CLR_ADDR      PMU_GLB_BASE_ADDR + 0xB8
#define PMU_GLB_INT_CLR_RSTVAL    0x0
#define PMU_GLB_INT_CLR_ALL1      0x0
#define PMU_GLB_INT_CLR_ALL0      0x0
#define PMU_GLB_INT_CLR_ALL5A     0x0
#define PMU_GLB_INT_CLR_ALLA5     0x0

// FIELD IN REGISTER : INT_CLR

#define PMU_GLB_INT_CLR_VAL_POS      0
#define PMU_GLB_INT_CLR_VAL_LEN      32
#define PMU_GLB_INT_CLR_VAL_MASK     0xFFFFFFFF
#define PMU_GLB_INT_CLR_VAL_VAL(w)   (((w) & 0xFFFFFFFF) >> 0)
#define PMU_GLB_INT_CLR_VAL_SET(w)   w |= ( 0xFFFFFFFF << 0)
#define PMU_GLB_INT_CLR_VAL_CLR(w)   w &= ~( 0xFFFFFFFF << 0)

// REGISTER : INT_CLR1

#define PMU_GLB_INT_CLR1_ADDR      PMU_GLB_BASE_ADDR + 0xBC
#define PMU_GLB_INT_CLR1_RSTVAL    0x0
#define PMU_GLB_INT_CLR1_ALL1      0x0
#define PMU_GLB_INT_CLR1_ALL0      0x0
#define PMU_GLB_INT_CLR1_ALL5A     0x0
#define PMU_GLB_INT_CLR1_ALLA5     0x0

// FIELD IN REGISTER : INT_CLR1

#define PMU_GLB_INT_CLR1_VAL_POS      0
#define PMU_GLB_INT_CLR1_VAL_LEN      32
#define PMU_GLB_INT_CLR1_VAL_MASK     0xFFFFFFFF
#define PMU_GLB_INT_CLR1_VAL_VAL(w)   (((w) & 0xFFFFFFFF) >> 0)
#define PMU_GLB_INT_CLR1_VAL_SET(w)   w |= ( 0xFFFFFFFF << 0)
#define PMU_GLB_INT_CLR1_VAL_CLR(w)   w &= ~( 0xFFFFFFFF << 0)

// REGISTER : INT_CLR2

#define PMU_GLB_INT_CLR2_ADDR      PMU_GLB_BASE_ADDR + 0xC0
#define PMU_GLB_INT_CLR2_RSTVAL    0x0
#define PMU_GLB_INT_CLR2_ALL1      0x0
#define PMU_GLB_INT_CLR2_ALL0      0x0
#define PMU_GLB_INT_CLR2_ALL5A     0x0
#define PMU_GLB_INT_CLR2_ALLA5     0x0

// FIELD IN REGISTER : INT_CLR2

#define PMU_GLB_INT_CLR2_VAL_POS      0
#define PMU_GLB_INT_CLR2_VAL_LEN      4
#define PMU_GLB_INT_CLR2_VAL_MASK     0x0000000F
#define PMU_GLB_INT_CLR2_VAL_VAL(w)   (((w) & 0x0000000F) >> 0)
#define PMU_GLB_INT_CLR2_VAL_SET(w)   w |= ( 0xF << 0)
#define PMU_GLB_INT_CLR2_VAL_CLR(w)   w &= ~( 0xF << 0)

// REGISTER : INT_STA

#define PMU_GLB_INT_STA_ADDR      PMU_GLB_BASE_ADDR + 0xD0
#define PMU_GLB_INT_STA_RSTVAL    0x0
#define PMU_GLB_INT_STA_ALL1      0xFFFFFFFF
#define PMU_GLB_INT_STA_ALL0      0x0
#define PMU_GLB_INT_STA_ALL5A     0x5A5A5A5A
#define PMU_GLB_INT_STA_ALLA5     0xA5A5A5A5

// FIELD IN REGISTER : INT_STA

#define PMU_GLB_INT_STA_VAL_POS      0
#define PMU_GLB_INT_STA_VAL_LEN      32
#define PMU_GLB_INT_STA_VAL_MASK     0xFFFFFFFF
#define PMU_GLB_INT_STA_VAL_VAL(w)   (((w) & 0xFFFFFFFF) >> 0)
#define PMU_GLB_INT_STA_VAL_SET(w)   w |= ( 0xFFFFFFFF << 0)
#define PMU_GLB_INT_STA_VAL_CLR(w)   w &= ~( 0xFFFFFFFF << 0)

// REGISTER : INT_STA1

#define PMU_GLB_INT_STA1_ADDR      PMU_GLB_BASE_ADDR + 0xD4
#define PMU_GLB_INT_STA1_RSTVAL    0x0
#define PMU_GLB_INT_STA1_ALL1      0xFFFFFFFF
#define PMU_GLB_INT_STA1_ALL0      0x0
#define PMU_GLB_INT_STA1_ALL5A     0x5A5A5A5A
#define PMU_GLB_INT_STA1_ALLA5     0xA5A5A5A5

// FIELD IN REGISTER : INT_STA1

#define PMU_GLB_INT_STA1_VAL_POS      0
#define PMU_GLB_INT_STA1_VAL_LEN      32
#define PMU_GLB_INT_STA1_VAL_MASK     0xFFFFFFFF
#define PMU_GLB_INT_STA1_VAL_VAL(w)   (((w) & 0xFFFFFFFF) >> 0)
#define PMU_GLB_INT_STA1_VAL_SET(w)   w |= ( 0xFFFFFFFF << 0)
#define PMU_GLB_INT_STA1_VAL_CLR(w)   w &= ~( 0xFFFFFFFF << 0)

// REGISTER : INT_STA2

#define PMU_GLB_INT_STA2_ADDR      PMU_GLB_BASE_ADDR + 0xD8
#define PMU_GLB_INT_STA2_RSTVAL    0x0
#define PMU_GLB_INT_STA2_ALL1      0xF
#define PMU_GLB_INT_STA2_ALL0      0x0
#define PMU_GLB_INT_STA2_ALL5A     0xA
#define PMU_GLB_INT_STA2_ALLA5     0x5

// FIELD IN REGISTER : INT_STA2

#define PMU_GLB_INT_STA2_VAL_POS      0
#define PMU_GLB_INT_STA2_VAL_LEN      4
#define PMU_GLB_INT_STA2_VAL_MASK     0x0000000F
#define PMU_GLB_INT_STA2_VAL_VAL(w)   (((w) & 0x0000000F) >> 0)
#define PMU_GLB_INT_STA2_VAL_SET(w)   w |= ( 0xF << 0)
#define PMU_GLB_INT_STA2_VAL_CLR(w)   w &= ~( 0xF << 0)

// REGISTER : PWR_BYPASS_SET

#define PMU_GLB_PWR_BYPASS_SET_ADDR      PMU_GLB_BASE_ADDR + 0xFC
#define PMU_GLB_PWR_BYPASS_SET_RSTVAL    0x0
#define PMU_GLB_PWR_BYPASS_SET_ALL1      0x0
#define PMU_GLB_PWR_BYPASS_SET_ALL0      0x0
#define PMU_GLB_PWR_BYPASS_SET_ALL5A     0x0
#define PMU_GLB_PWR_BYPASS_SET_ALLA5     0x0

// FIELD IN REGISTER : PWR_BYPASS_SET

#define PMU_GLB_PWR_BYPASS_SET_ISP_SET_POS      14
#define PMU_GLB_PWR_BYPASS_SET_ISP_SET_LEN      1
#define PMU_GLB_PWR_BYPASS_SET_ISP_SET_MASK     0x00004000
#define PMU_GLB_PWR_BYPASS_SET_ISP_SET_VAL(w)   (((w) & 0x00004000) >> 14)
#define PMU_GLB_PWR_BYPASS_SET_ISP_SET_SET(w)   w |= ( 0x1 << 14)
#define PMU_GLB_PWR_BYPASS_SET_ISP_SET_CLR(w)   w &= ~( 0x1 << 14)

#define PMU_GLB_PWR_BYPASS_SET_CPU_SET_POS      13
#define PMU_GLB_PWR_BYPASS_SET_CPU_SET_LEN      1
#define PMU_GLB_PWR_BYPASS_SET_CPU_SET_MASK     0x00002000
#define PMU_GLB_PWR_BYPASS_SET_CPU_SET_VAL(w)   (((w) & 0x00002000) >> 13)
#define PMU_GLB_PWR_BYPASS_SET_CPU_SET_SET(w)   w |= ( 0x1 << 13)
#define PMU_GLB_PWR_BYPASS_SET_CPU_SET_CLR(w)   w &= ~( 0x1 << 13)

#define PMU_GLB_PWR_BYPASS_SET_NPU_SET_POS      12
#define PMU_GLB_PWR_BYPASS_SET_NPU_SET_LEN      1
#define PMU_GLB_PWR_BYPASS_SET_NPU_SET_MASK     0x00001000
#define PMU_GLB_PWR_BYPASS_SET_NPU_SET_VAL(w)   (((w) & 0x00001000) >> 12)
#define PMU_GLB_PWR_BYPASS_SET_NPU_SET_SET(w)   w |= ( 0x1 << 12)
#define PMU_GLB_PWR_BYPASS_SET_NPU_SET_CLR(w)   w &= ~( 0x1 << 12)

#define PMU_GLB_PWR_BYPASS_SET_CPU7_SET_POS      11
#define PMU_GLB_PWR_BYPASS_SET_CPU7_SET_LEN      1
#define PMU_GLB_PWR_BYPASS_SET_CPU7_SET_MASK     0x00000800
#define PMU_GLB_PWR_BYPASS_SET_CPU7_SET_VAL(w)   (((w) & 0x00000800) >> 11)
#define PMU_GLB_PWR_BYPASS_SET_CPU7_SET_SET(w)   w |= ( 0x1 << 11)
#define PMU_GLB_PWR_BYPASS_SET_CPU7_SET_CLR(w)   w &= ~( 0x1 << 11)

#define PMU_GLB_PWR_BYPASS_SET_CPU6_SET_POS      10
#define PMU_GLB_PWR_BYPASS_SET_CPU6_SET_LEN      1
#define PMU_GLB_PWR_BYPASS_SET_CPU6_SET_MASK     0x00000400
#define PMU_GLB_PWR_BYPASS_SET_CPU6_SET_VAL(w)   (((w) & 0x00000400) >> 10)
#define PMU_GLB_PWR_BYPASS_SET_CPU6_SET_SET(w)   w |= ( 0x1 << 10)
#define PMU_GLB_PWR_BYPASS_SET_CPU6_SET_CLR(w)   w &= ~( 0x1 << 10)

#define PMU_GLB_PWR_BYPASS_SET_CPU5_SET_POS      9
#define PMU_GLB_PWR_BYPASS_SET_CPU5_SET_LEN      1
#define PMU_GLB_PWR_BYPASS_SET_CPU5_SET_MASK     0x00000200
#define PMU_GLB_PWR_BYPASS_SET_CPU5_SET_VAL(w)   (((w) & 0x00000200) >> 9)
#define PMU_GLB_PWR_BYPASS_SET_CPU5_SET_SET(w)   w |= ( 0x1 << 9)
#define PMU_GLB_PWR_BYPASS_SET_CPU5_SET_CLR(w)   w &= ~( 0x1 << 9)

#define PMU_GLB_PWR_BYPASS_SET_CPU4_SET_POS      8
#define PMU_GLB_PWR_BYPASS_SET_CPU4_SET_LEN      1
#define PMU_GLB_PWR_BYPASS_SET_CPU4_SET_MASK     0x00000100
#define PMU_GLB_PWR_BYPASS_SET_CPU4_SET_VAL(w)   (((w) & 0x00000100) >> 8)
#define PMU_GLB_PWR_BYPASS_SET_CPU4_SET_SET(w)   w |= ( 0x1 << 8)
#define PMU_GLB_PWR_BYPASS_SET_CPU4_SET_CLR(w)   w &= ~( 0x1 << 8)

#define PMU_GLB_PWR_BYPASS_SET_CPU3_SET_POS      7
#define PMU_GLB_PWR_BYPASS_SET_CPU3_SET_LEN      1
#define PMU_GLB_PWR_BYPASS_SET_CPU3_SET_MASK     0x00000080
#define PMU_GLB_PWR_BYPASS_SET_CPU3_SET_VAL(w)   (((w) & 0x00000080) >> 7)
#define PMU_GLB_PWR_BYPASS_SET_CPU3_SET_SET(w)   w |= ( 0x1 << 7)
#define PMU_GLB_PWR_BYPASS_SET_CPU3_SET_CLR(w)   w &= ~( 0x1 << 7)

#define PMU_GLB_PWR_BYPASS_SET_CPU2_SET_POS      6
#define PMU_GLB_PWR_BYPASS_SET_CPU2_SET_LEN      1
#define PMU_GLB_PWR_BYPASS_SET_CPU2_SET_MASK     0x00000040
#define PMU_GLB_PWR_BYPASS_SET_CPU2_SET_VAL(w)   (((w) & 0x00000040) >> 6)
#define PMU_GLB_PWR_BYPASS_SET_CPU2_SET_SET(w)   w |= ( 0x1 << 6)
#define PMU_GLB_PWR_BYPASS_SET_CPU2_SET_CLR(w)   w &= ~( 0x1 << 6)

#define PMU_GLB_PWR_BYPASS_SET_CPU1_SET_POS      5
#define PMU_GLB_PWR_BYPASS_SET_CPU1_SET_LEN      1
#define PMU_GLB_PWR_BYPASS_SET_CPU1_SET_MASK     0x00000020
#define PMU_GLB_PWR_BYPASS_SET_CPU1_SET_VAL(w)   (((w) & 0x00000020) >> 5)
#define PMU_GLB_PWR_BYPASS_SET_CPU1_SET_SET(w)   w |= ( 0x1 << 5)
#define PMU_GLB_PWR_BYPASS_SET_CPU1_SET_CLR(w)   w &= ~( 0x1 << 5)

#define PMU_GLB_PWR_BYPASS_SET_CPU0_SET_POS      4
#define PMU_GLB_PWR_BYPASS_SET_CPU0_SET_LEN      1
#define PMU_GLB_PWR_BYPASS_SET_CPU0_SET_MASK     0x00000010
#define PMU_GLB_PWR_BYPASS_SET_CPU0_SET_VAL(w)   (((w) & 0x00000010) >> 4)
#define PMU_GLB_PWR_BYPASS_SET_CPU0_SET_SET(w)   w |= ( 0x1 << 4)
#define PMU_GLB_PWR_BYPASS_SET_CPU0_SET_CLR(w)   w &= ~( 0x1 << 4)

#define PMU_GLB_PWR_BYPASS_SET_DSP_SET_POS      3
#define PMU_GLB_PWR_BYPASS_SET_DSP_SET_LEN      1
#define PMU_GLB_PWR_BYPASS_SET_DSP_SET_MASK     0x00000008
#define PMU_GLB_PWR_BYPASS_SET_DSP_SET_VAL(w)   (((w) & 0x00000008) >> 3)
#define PMU_GLB_PWR_BYPASS_SET_DSP_SET_SET(w)   w |= ( 0x1 << 3)
#define PMU_GLB_PWR_BYPASS_SET_DSP_SET_CLR(w)   w &= ~( 0x1 << 3)

#define PMU_GLB_PWR_BYPASS_SET_VDEC_SET_POS      2
#define PMU_GLB_PWR_BYPASS_SET_VDEC_SET_LEN      1
#define PMU_GLB_PWR_BYPASS_SET_VDEC_SET_MASK     0x00000004
#define PMU_GLB_PWR_BYPASS_SET_VDEC_SET_VAL(w)   (((w) & 0x00000004) >> 2)
#define PMU_GLB_PWR_BYPASS_SET_VDEC_SET_SET(w)   w |= ( 0x1 << 2)
#define PMU_GLB_PWR_BYPASS_SET_VDEC_SET_CLR(w)   w &= ~( 0x1 << 2)

#define PMU_GLB_PWR_BYPASS_SET_VENC1_SET_POS      1
#define PMU_GLB_PWR_BYPASS_SET_VENC1_SET_LEN      1
#define PMU_GLB_PWR_BYPASS_SET_VENC1_SET_MASK     0x00000002
#define PMU_GLB_PWR_BYPASS_SET_VENC1_SET_VAL(w)   (((w) & 0x00000002) >> 1)
#define PMU_GLB_PWR_BYPASS_SET_VENC1_SET_SET(w)   w |= ( 0x1 << 1)
#define PMU_GLB_PWR_BYPASS_SET_VENC1_SET_CLR(w)   w &= ~( 0x1 << 1)

#define PMU_GLB_PWR_BYPASS_SET_VENC0_SET_POS      0
#define PMU_GLB_PWR_BYPASS_SET_VENC0_SET_LEN      1
#define PMU_GLB_PWR_BYPASS_SET_VENC0_SET_MASK     0x00000001
#define PMU_GLB_PWR_BYPASS_SET_VENC0_SET_VAL(w)   (((w) & 0x00000001) >> 0)
#define PMU_GLB_PWR_BYPASS_SET_VENC0_SET_SET(w)   w |= ( 0x1 << 0)
#define PMU_GLB_PWR_BYPASS_SET_VENC0_SET_CLR(w)   w &= ~( 0x1 << 0)

// REGISTER : PWR_BYPASS_CLR

#define PMU_GLB_PWR_BYPASS_CLR_ADDR      PMU_GLB_BASE_ADDR + 0x100
#define PMU_GLB_PWR_BYPASS_CLR_RSTVAL    0x0
#define PMU_GLB_PWR_BYPASS_CLR_ALL1      0x0
#define PMU_GLB_PWR_BYPASS_CLR_ALL0      0x0
#define PMU_GLB_PWR_BYPASS_CLR_ALL5A     0x0
#define PMU_GLB_PWR_BYPASS_CLR_ALLA5     0x0

// FIELD IN REGISTER : PWR_BYPASS_CLR

#define PMU_GLB_PWR_BYPASS_CLR_ISP_CLR_POS      14
#define PMU_GLB_PWR_BYPASS_CLR_ISP_CLR_LEN      1
#define PMU_GLB_PWR_BYPASS_CLR_ISP_CLR_MASK     0x00004000
#define PMU_GLB_PWR_BYPASS_CLR_ISP_CLR_VAL(w)   (((w) & 0x00004000) >> 14)
#define PMU_GLB_PWR_BYPASS_CLR_ISP_CLR_SET(w)   w |= ( 0x1 << 14)
#define PMU_GLB_PWR_BYPASS_CLR_ISP_CLR_CLR(w)   w &= ~( 0x1 << 14)

#define PMU_GLB_PWR_BYPASS_CLR_CPU_CLR_POS      13
#define PMU_GLB_PWR_BYPASS_CLR_CPU_CLR_LEN      1
#define PMU_GLB_PWR_BYPASS_CLR_CPU_CLR_MASK     0x00002000
#define PMU_GLB_PWR_BYPASS_CLR_CPU_CLR_VAL(w)   (((w) & 0x00002000) >> 13)
#define PMU_GLB_PWR_BYPASS_CLR_CPU_CLR_SET(w)   w |= ( 0x1 << 13)
#define PMU_GLB_PWR_BYPASS_CLR_CPU_CLR_CLR(w)   w &= ~( 0x1 << 13)

#define PMU_GLB_PWR_BYPASS_CLR_NPU_CLR_POS      12
#define PMU_GLB_PWR_BYPASS_CLR_NPU_CLR_LEN      1
#define PMU_GLB_PWR_BYPASS_CLR_NPU_CLR_MASK     0x00001000
#define PMU_GLB_PWR_BYPASS_CLR_NPU_CLR_VAL(w)   (((w) & 0x00001000) >> 12)
#define PMU_GLB_PWR_BYPASS_CLR_NPU_CLR_SET(w)   w |= ( 0x1 << 12)
#define PMU_GLB_PWR_BYPASS_CLR_NPU_CLR_CLR(w)   w &= ~( 0x1 << 12)

#define PMU_GLB_PWR_BYPASS_CLR_CPU7_CLR_POS      11
#define PMU_GLB_PWR_BYPASS_CLR_CPU7_CLR_LEN      1
#define PMU_GLB_PWR_BYPASS_CLR_CPU7_CLR_MASK     0x00000800
#define PMU_GLB_PWR_BYPASS_CLR_CPU7_CLR_VAL(w)   (((w) & 0x00000800) >> 11)
#define PMU_GLB_PWR_BYPASS_CLR_CPU7_CLR_SET(w)   w |= ( 0x1 << 11)
#define PMU_GLB_PWR_BYPASS_CLR_CPU7_CLR_CLR(w)   w &= ~( 0x1 << 11)

#define PMU_GLB_PWR_BYPASS_CLR_CPU6_CLR_POS      10
#define PMU_GLB_PWR_BYPASS_CLR_CPU6_CLR_LEN      1
#define PMU_GLB_PWR_BYPASS_CLR_CPU6_CLR_MASK     0x00000400
#define PMU_GLB_PWR_BYPASS_CLR_CPU6_CLR_VAL(w)   (((w) & 0x00000400) >> 10)
#define PMU_GLB_PWR_BYPASS_CLR_CPU6_CLR_SET(w)   w |= ( 0x1 << 10)
#define PMU_GLB_PWR_BYPASS_CLR_CPU6_CLR_CLR(w)   w &= ~( 0x1 << 10)

#define PMU_GLB_PWR_BYPASS_CLR_CPU5_CLR_POS      9
#define PMU_GLB_PWR_BYPASS_CLR_CPU5_CLR_LEN      1
#define PMU_GLB_PWR_BYPASS_CLR_CPU5_CLR_MASK     0x00000200
#define PMU_GLB_PWR_BYPASS_CLR_CPU5_CLR_VAL(w)   (((w) & 0x00000200) >> 9)
#define PMU_GLB_PWR_BYPASS_CLR_CPU5_CLR_SET(w)   w |= ( 0x1 << 9)
#define PMU_GLB_PWR_BYPASS_CLR_CPU5_CLR_CLR(w)   w &= ~( 0x1 << 9)

#define PMU_GLB_PWR_BYPASS_CLR_CPU4_CLR_POS      8
#define PMU_GLB_PWR_BYPASS_CLR_CPU4_CLR_LEN      1
#define PMU_GLB_PWR_BYPASS_CLR_CPU4_CLR_MASK     0x00000100
#define PMU_GLB_PWR_BYPASS_CLR_CPU4_CLR_VAL(w)   (((w) & 0x00000100) >> 8)
#define PMU_GLB_PWR_BYPASS_CLR_CPU4_CLR_SET(w)   w |= ( 0x1 << 8)
#define PMU_GLB_PWR_BYPASS_CLR_CPU4_CLR_CLR(w)   w &= ~( 0x1 << 8)

#define PMU_GLB_PWR_BYPASS_CLR_CPU3_CLR_POS      7
#define PMU_GLB_PWR_BYPASS_CLR_CPU3_CLR_LEN      1
#define PMU_GLB_PWR_BYPASS_CLR_CPU3_CLR_MASK     0x00000080
#define PMU_GLB_PWR_BYPASS_CLR_CPU3_CLR_VAL(w)   (((w) & 0x00000080) >> 7)
#define PMU_GLB_PWR_BYPASS_CLR_CPU3_CLR_SET(w)   w |= ( 0x1 << 7)
#define PMU_GLB_PWR_BYPASS_CLR_CPU3_CLR_CLR(w)   w &= ~( 0x1 << 7)

#define PMU_GLB_PWR_BYPASS_CLR_CPU2_CLR_POS      6
#define PMU_GLB_PWR_BYPASS_CLR_CPU2_CLR_LEN      1
#define PMU_GLB_PWR_BYPASS_CLR_CPU2_CLR_MASK     0x00000040
#define PMU_GLB_PWR_BYPASS_CLR_CPU2_CLR_VAL(w)   (((w) & 0x00000040) >> 6)
#define PMU_GLB_PWR_BYPASS_CLR_CPU2_CLR_SET(w)   w |= ( 0x1 << 6)
#define PMU_GLB_PWR_BYPASS_CLR_CPU2_CLR_CLR(w)   w &= ~( 0x1 << 6)

#define PMU_GLB_PWR_BYPASS_CLR_CPU1_CLR_POS      5
#define PMU_GLB_PWR_BYPASS_CLR_CPU1_CLR_LEN      1
#define PMU_GLB_PWR_BYPASS_CLR_CPU1_CLR_MASK     0x00000020
#define PMU_GLB_PWR_BYPASS_CLR_CPU1_CLR_VAL(w)   (((w) & 0x00000020) >> 5)
#define PMU_GLB_PWR_BYPASS_CLR_CPU1_CLR_SET(w)   w |= ( 0x1 << 5)
#define PMU_GLB_PWR_BYPASS_CLR_CPU1_CLR_CLR(w)   w &= ~( 0x1 << 5)

#define PMU_GLB_PWR_BYPASS_CLR_CPU0_CLR_POS      4
#define PMU_GLB_PWR_BYPASS_CLR_CPU0_CLR_LEN      1
#define PMU_GLB_PWR_BYPASS_CLR_CPU0_CLR_MASK     0x00000010
#define PMU_GLB_PWR_BYPASS_CLR_CPU0_CLR_VAL(w)   (((w) & 0x00000010) >> 4)
#define PMU_GLB_PWR_BYPASS_CLR_CPU0_CLR_SET(w)   w |= ( 0x1 << 4)
#define PMU_GLB_PWR_BYPASS_CLR_CPU0_CLR_CLR(w)   w &= ~( 0x1 << 4)

#define PMU_GLB_PWR_BYPASS_CLR_DSP_CLR_POS      3
#define PMU_GLB_PWR_BYPASS_CLR_DSP_CLR_LEN      1
#define PMU_GLB_PWR_BYPASS_CLR_DSP_CLR_MASK     0x00000008
#define PMU_GLB_PWR_BYPASS_CLR_DSP_CLR_VAL(w)   (((w) & 0x00000008) >> 3)
#define PMU_GLB_PWR_BYPASS_CLR_DSP_CLR_SET(w)   w |= ( 0x1 << 3)
#define PMU_GLB_PWR_BYPASS_CLR_DSP_CLR_CLR(w)   w &= ~( 0x1 << 3)

#define PMU_GLB_PWR_BYPASS_CLR_VDEC_CLR_POS      2
#define PMU_GLB_PWR_BYPASS_CLR_VDEC_CLR_LEN      1
#define PMU_GLB_PWR_BYPASS_CLR_VDEC_CLR_MASK     0x00000004
#define PMU_GLB_PWR_BYPASS_CLR_VDEC_CLR_VAL(w)   (((w) & 0x00000004) >> 2)
#define PMU_GLB_PWR_BYPASS_CLR_VDEC_CLR_SET(w)   w |= ( 0x1 << 2)
#define PMU_GLB_PWR_BYPASS_CLR_VDEC_CLR_CLR(w)   w &= ~( 0x1 << 2)

#define PMU_GLB_PWR_BYPASS_CLR_VENC1_CLR_POS      1
#define PMU_GLB_PWR_BYPASS_CLR_VENC1_CLR_LEN      1
#define PMU_GLB_PWR_BYPASS_CLR_VENC1_CLR_MASK     0x00000002
#define PMU_GLB_PWR_BYPASS_CLR_VENC1_CLR_VAL(w)   (((w) & 0x00000002) >> 1)
#define PMU_GLB_PWR_BYPASS_CLR_VENC1_CLR_SET(w)   w |= ( 0x1 << 1)
#define PMU_GLB_PWR_BYPASS_CLR_VENC1_CLR_CLR(w)   w &= ~( 0x1 << 1)

#define PMU_GLB_PWR_BYPASS_CLR_VENC0_CLR_POS      0
#define PMU_GLB_PWR_BYPASS_CLR_VENC0_CLR_LEN      1
#define PMU_GLB_PWR_BYPASS_CLR_VENC0_CLR_MASK     0x00000001
#define PMU_GLB_PWR_BYPASS_CLR_VENC0_CLR_VAL(w)   (((w) & 0x00000001) >> 0)
#define PMU_GLB_PWR_BYPASS_CLR_VENC0_CLR_SET(w)   w |= ( 0x1 << 0)
#define PMU_GLB_PWR_BYPASS_CLR_VENC0_CLR_CLR(w)   w &= ~( 0x1 << 0)

// REGISTER : PWR_BYPASS1_SET

#define PMU_GLB_PWR_BYPASS1_SET_ADDR      PMU_GLB_BASE_ADDR + 0x104
#define PMU_GLB_PWR_BYPASS1_SET_RSTVAL    0x0
#define PMU_GLB_PWR_BYPASS1_SET_ALL1      0x0
#define PMU_GLB_PWR_BYPASS1_SET_ALL0      0x0
#define PMU_GLB_PWR_BYPASS1_SET_ALL5A     0x0
#define PMU_GLB_PWR_BYPASS1_SET_ALLA5     0x0

// FIELD IN REGISTER : PWR_BYPASS1_SET

#define PMU_GLB_PWR_BYPASS1_SET_N8_SET_POS      10
#define PMU_GLB_PWR_BYPASS1_SET_N8_SET_LEN      1
#define PMU_GLB_PWR_BYPASS1_SET_N8_SET_MASK     0x00000400
#define PMU_GLB_PWR_BYPASS1_SET_N8_SET_VAL(w)   (((w) & 0x00000400) >> 10)
#define PMU_GLB_PWR_BYPASS1_SET_N8_SET_SET(w)   w |= ( 0x1 << 10)
#define PMU_GLB_PWR_BYPASS1_SET_N8_SET_CLR(w)   w &= ~( 0x1 << 10)

#define PMU_GLB_PWR_BYPASS1_SET_N7_SET_POS      9
#define PMU_GLB_PWR_BYPASS1_SET_N7_SET_LEN      1
#define PMU_GLB_PWR_BYPASS1_SET_N7_SET_MASK     0x00000200
#define PMU_GLB_PWR_BYPASS1_SET_N7_SET_VAL(w)   (((w) & 0x00000200) >> 9)
#define PMU_GLB_PWR_BYPASS1_SET_N7_SET_SET(w)   w |= ( 0x1 << 9)
#define PMU_GLB_PWR_BYPASS1_SET_N7_SET_CLR(w)   w &= ~( 0x1 << 9)

#define PMU_GLB_PWR_BYPASS1_SET_N6_SET_POS      8
#define PMU_GLB_PWR_BYPASS1_SET_N6_SET_LEN      1
#define PMU_GLB_PWR_BYPASS1_SET_N6_SET_MASK     0x00000100
#define PMU_GLB_PWR_BYPASS1_SET_N6_SET_VAL(w)   (((w) & 0x00000100) >> 8)
#define PMU_GLB_PWR_BYPASS1_SET_N6_SET_SET(w)   w |= ( 0x1 << 8)
#define PMU_GLB_PWR_BYPASS1_SET_N6_SET_CLR(w)   w &= ~( 0x1 << 8)

#define PMU_GLB_PWR_BYPASS1_SET_N5_SET_POS      7
#define PMU_GLB_PWR_BYPASS1_SET_N5_SET_LEN      1
#define PMU_GLB_PWR_BYPASS1_SET_N5_SET_MASK     0x00000080
#define PMU_GLB_PWR_BYPASS1_SET_N5_SET_VAL(w)   (((w) & 0x00000080) >> 7)
#define PMU_GLB_PWR_BYPASS1_SET_N5_SET_SET(w)   w |= ( 0x1 << 7)
#define PMU_GLB_PWR_BYPASS1_SET_N5_SET_CLR(w)   w &= ~( 0x1 << 7)

#define PMU_GLB_PWR_BYPASS1_SET_N4_SET_POS      6
#define PMU_GLB_PWR_BYPASS1_SET_N4_SET_LEN      1
#define PMU_GLB_PWR_BYPASS1_SET_N4_SET_MASK     0x00000040
#define PMU_GLB_PWR_BYPASS1_SET_N4_SET_VAL(w)   (((w) & 0x00000040) >> 6)
#define PMU_GLB_PWR_BYPASS1_SET_N4_SET_SET(w)   w |= ( 0x1 << 6)
#define PMU_GLB_PWR_BYPASS1_SET_N4_SET_CLR(w)   w &= ~( 0x1 << 6)

#define PMU_GLB_PWR_BYPASS1_SET_N3_SET_POS      5
#define PMU_GLB_PWR_BYPASS1_SET_N3_SET_LEN      1
#define PMU_GLB_PWR_BYPASS1_SET_N3_SET_MASK     0x00000020
#define PMU_GLB_PWR_BYPASS1_SET_N3_SET_VAL(w)   (((w) & 0x00000020) >> 5)
#define PMU_GLB_PWR_BYPASS1_SET_N3_SET_SET(w)   w |= ( 0x1 << 5)
#define PMU_GLB_PWR_BYPASS1_SET_N3_SET_CLR(w)   w &= ~( 0x1 << 5)

#define PMU_GLB_PWR_BYPASS1_SET_N2_SET_POS      4
#define PMU_GLB_PWR_BYPASS1_SET_N2_SET_LEN      1
#define PMU_GLB_PWR_BYPASS1_SET_N2_SET_MASK     0x00000010
#define PMU_GLB_PWR_BYPASS1_SET_N2_SET_VAL(w)   (((w) & 0x00000010) >> 4)
#define PMU_GLB_PWR_BYPASS1_SET_N2_SET_SET(w)   w |= ( 0x1 << 4)
#define PMU_GLB_PWR_BYPASS1_SET_N2_SET_CLR(w)   w &= ~( 0x1 << 4)

#define PMU_GLB_PWR_BYPASS1_SET_N1_SET_POS      3
#define PMU_GLB_PWR_BYPASS1_SET_N1_SET_LEN      1
#define PMU_GLB_PWR_BYPASS1_SET_N1_SET_MASK     0x00000008
#define PMU_GLB_PWR_BYPASS1_SET_N1_SET_VAL(w)   (((w) & 0x00000008) >> 3)
#define PMU_GLB_PWR_BYPASS1_SET_N1_SET_SET(w)   w |= ( 0x1 << 3)
#define PMU_GLB_PWR_BYPASS1_SET_N1_SET_CLR(w)   w &= ~( 0x1 << 3)

#define PMU_GLB_PWR_BYPASS1_SET_N0_SET_POS      2
#define PMU_GLB_PWR_BYPASS1_SET_N0_SET_LEN      1
#define PMU_GLB_PWR_BYPASS1_SET_N0_SET_MASK     0x00000004
#define PMU_GLB_PWR_BYPASS1_SET_N0_SET_VAL(w)   (((w) & 0x00000004) >> 2)
#define PMU_GLB_PWR_BYPASS1_SET_N0_SET_SET(w)   w |= ( 0x1 << 2)
#define PMU_GLB_PWR_BYPASS1_SET_N0_SET_CLR(w)   w &= ~( 0x1 << 2)

#define PMU_GLB_PWR_BYPASS1_SET_MM_SET_POS      1
#define PMU_GLB_PWR_BYPASS1_SET_MM_SET_LEN      1
#define PMU_GLB_PWR_BYPASS1_SET_MM_SET_MASK     0x00000002
#define PMU_GLB_PWR_BYPASS1_SET_MM_SET_VAL(w)   (((w) & 0x00000002) >> 1)
#define PMU_GLB_PWR_BYPASS1_SET_MM_SET_SET(w)   w |= ( 0x1 << 1)
#define PMU_GLB_PWR_BYPASS1_SET_MM_SET_CLR(w)   w &= ~( 0x1 << 1)

#define PMU_GLB_PWR_BYPASS1_SET_PIPE_SET_POS      0
#define PMU_GLB_PWR_BYPASS1_SET_PIPE_SET_LEN      1
#define PMU_GLB_PWR_BYPASS1_SET_PIPE_SET_MASK     0x00000001
#define PMU_GLB_PWR_BYPASS1_SET_PIPE_SET_VAL(w)   (((w) & 0x00000001) >> 0)
#define PMU_GLB_PWR_BYPASS1_SET_PIPE_SET_SET(w)   w |= ( 0x1 << 0)
#define PMU_GLB_PWR_BYPASS1_SET_PIPE_SET_CLR(w)   w &= ~( 0x1 << 0)

// REGISTER : PWR_BYPASS1_CLR

#define PMU_GLB_PWR_BYPASS1_CLR_ADDR      PMU_GLB_BASE_ADDR + 0x108
#define PMU_GLB_PWR_BYPASS1_CLR_RSTVAL    0x0
#define PMU_GLB_PWR_BYPASS1_CLR_ALL1      0x0
#define PMU_GLB_PWR_BYPASS1_CLR_ALL0      0x0
#define PMU_GLB_PWR_BYPASS1_CLR_ALL5A     0x0
#define PMU_GLB_PWR_BYPASS1_CLR_ALLA5     0x0

// FIELD IN REGISTER : PWR_BYPASS1_CLR

#define PMU_GLB_PWR_BYPASS1_CLR_N8_CLR_POS      10
#define PMU_GLB_PWR_BYPASS1_CLR_N8_CLR_LEN      1
#define PMU_GLB_PWR_BYPASS1_CLR_N8_CLR_MASK     0x00000400
#define PMU_GLB_PWR_BYPASS1_CLR_N8_CLR_VAL(w)   (((w) & 0x00000400) >> 10)
#define PMU_GLB_PWR_BYPASS1_CLR_N8_CLR_SET(w)   w |= ( 0x1 << 10)
#define PMU_GLB_PWR_BYPASS1_CLR_N8_CLR_CLR(w)   w &= ~( 0x1 << 10)

#define PMU_GLB_PWR_BYPASS1_CLR_N7_CLR_POS      9
#define PMU_GLB_PWR_BYPASS1_CLR_N7_CLR_LEN      1
#define PMU_GLB_PWR_BYPASS1_CLR_N7_CLR_MASK     0x00000200
#define PMU_GLB_PWR_BYPASS1_CLR_N7_CLR_VAL(w)   (((w) & 0x00000200) >> 9)
#define PMU_GLB_PWR_BYPASS1_CLR_N7_CLR_SET(w)   w |= ( 0x1 << 9)
#define PMU_GLB_PWR_BYPASS1_CLR_N7_CLR_CLR(w)   w &= ~( 0x1 << 9)

#define PMU_GLB_PWR_BYPASS1_CLR_N6_CLR_POS      8
#define PMU_GLB_PWR_BYPASS1_CLR_N6_CLR_LEN      1
#define PMU_GLB_PWR_BYPASS1_CLR_N6_CLR_MASK     0x00000100
#define PMU_GLB_PWR_BYPASS1_CLR_N6_CLR_VAL(w)   (((w) & 0x00000100) >> 8)
#define PMU_GLB_PWR_BYPASS1_CLR_N6_CLR_SET(w)   w |= ( 0x1 << 8)
#define PMU_GLB_PWR_BYPASS1_CLR_N6_CLR_CLR(w)   w &= ~( 0x1 << 8)

#define PMU_GLB_PWR_BYPASS1_CLR_N5_CLR_POS      7
#define PMU_GLB_PWR_BYPASS1_CLR_N5_CLR_LEN      1
#define PMU_GLB_PWR_BYPASS1_CLR_N5_CLR_MASK     0x00000080
#define PMU_GLB_PWR_BYPASS1_CLR_N5_CLR_VAL(w)   (((w) & 0x00000080) >> 7)
#define PMU_GLB_PWR_BYPASS1_CLR_N5_CLR_SET(w)   w |= ( 0x1 << 7)
#define PMU_GLB_PWR_BYPASS1_CLR_N5_CLR_CLR(w)   w &= ~( 0x1 << 7)

#define PMU_GLB_PWR_BYPASS1_CLR_N4_CLR_POS      6
#define PMU_GLB_PWR_BYPASS1_CLR_N4_CLR_LEN      1
#define PMU_GLB_PWR_BYPASS1_CLR_N4_CLR_MASK     0x00000040
#define PMU_GLB_PWR_BYPASS1_CLR_N4_CLR_VAL(w)   (((w) & 0x00000040) >> 6)
#define PMU_GLB_PWR_BYPASS1_CLR_N4_CLR_SET(w)   w |= ( 0x1 << 6)
#define PMU_GLB_PWR_BYPASS1_CLR_N4_CLR_CLR(w)   w &= ~( 0x1 << 6)

#define PMU_GLB_PWR_BYPASS1_CLR_N3_CLR_POS      5
#define PMU_GLB_PWR_BYPASS1_CLR_N3_CLR_LEN      1
#define PMU_GLB_PWR_BYPASS1_CLR_N3_CLR_MASK     0x00000020
#define PMU_GLB_PWR_BYPASS1_CLR_N3_CLR_VAL(w)   (((w) & 0x00000020) >> 5)
#define PMU_GLB_PWR_BYPASS1_CLR_N3_CLR_SET(w)   w |= ( 0x1 << 5)
#define PMU_GLB_PWR_BYPASS1_CLR_N3_CLR_CLR(w)   w &= ~( 0x1 << 5)

#define PMU_GLB_PWR_BYPASS1_CLR_N2_CLR_POS      4
#define PMU_GLB_PWR_BYPASS1_CLR_N2_CLR_LEN      1
#define PMU_GLB_PWR_BYPASS1_CLR_N2_CLR_MASK     0x00000010
#define PMU_GLB_PWR_BYPASS1_CLR_N2_CLR_VAL(w)   (((w) & 0x00000010) >> 4)
#define PMU_GLB_PWR_BYPASS1_CLR_N2_CLR_SET(w)   w |= ( 0x1 << 4)
#define PMU_GLB_PWR_BYPASS1_CLR_N2_CLR_CLR(w)   w &= ~( 0x1 << 4)

#define PMU_GLB_PWR_BYPASS1_CLR_N1_CLR_POS      3
#define PMU_GLB_PWR_BYPASS1_CLR_N1_CLR_LEN      1
#define PMU_GLB_PWR_BYPASS1_CLR_N1_CLR_MASK     0x00000008
#define PMU_GLB_PWR_BYPASS1_CLR_N1_CLR_VAL(w)   (((w) & 0x00000008) >> 3)
#define PMU_GLB_PWR_BYPASS1_CLR_N1_CLR_SET(w)   w |= ( 0x1 << 3)
#define PMU_GLB_PWR_BYPASS1_CLR_N1_CLR_CLR(w)   w &= ~( 0x1 << 3)

#define PMU_GLB_PWR_BYPASS1_CLR_N0_CLR_POS      2
#define PMU_GLB_PWR_BYPASS1_CLR_N0_CLR_LEN      1
#define PMU_GLB_PWR_BYPASS1_CLR_N0_CLR_MASK     0x00000004
#define PMU_GLB_PWR_BYPASS1_CLR_N0_CLR_VAL(w)   (((w) & 0x00000004) >> 2)
#define PMU_GLB_PWR_BYPASS1_CLR_N0_CLR_SET(w)   w |= ( 0x1 << 2)
#define PMU_GLB_PWR_BYPASS1_CLR_N0_CLR_CLR(w)   w &= ~( 0x1 << 2)

#define PMU_GLB_PWR_BYPASS1_CLR_MM_CLR_POS      1
#define PMU_GLB_PWR_BYPASS1_CLR_MM_CLR_LEN      1
#define PMU_GLB_PWR_BYPASS1_CLR_MM_CLR_MASK     0x00000002
#define PMU_GLB_PWR_BYPASS1_CLR_MM_CLR_VAL(w)   (((w) & 0x00000002) >> 1)
#define PMU_GLB_PWR_BYPASS1_CLR_MM_CLR_SET(w)   w |= ( 0x1 << 1)
#define PMU_GLB_PWR_BYPASS1_CLR_MM_CLR_CLR(w)   w &= ~( 0x1 << 1)

#define PMU_GLB_PWR_BYPASS1_CLR_PIPE_CLR_POS      0
#define PMU_GLB_PWR_BYPASS1_CLR_PIPE_CLR_LEN      1
#define PMU_GLB_PWR_BYPASS1_CLR_PIPE_CLR_MASK     0x00000001
#define PMU_GLB_PWR_BYPASS1_CLR_PIPE_CLR_VAL(w)   (((w) & 0x00000001) >> 0)
#define PMU_GLB_PWR_BYPASS1_CLR_PIPE_CLR_SET(w)   w |= ( 0x1 << 0)
#define PMU_GLB_PWR_BYPASS1_CLR_PIPE_CLR_CLR(w)   w &= ~( 0x1 << 0)

// REGISTER : PWR_STATE0_SET

#define PMU_GLB_PWR_STATE0_SET_ADDR      PMU_GLB_BASE_ADDR + 0x10C
#define PMU_GLB_PWR_STATE0_SET_RSTVAL    0x0
#define PMU_GLB_PWR_STATE0_SET_ALL1      0x0
#define PMU_GLB_PWR_STATE0_SET_ALL0      0x0
#define PMU_GLB_PWR_STATE0_SET_ALL5A     0x0
#define PMU_GLB_PWR_STATE0_SET_ALLA5     0x0

// FIELD IN REGISTER : PWR_STATE0_SET

#define PMU_GLB_PWR_STATE0_SET_ISP_SET_POS      24
#define PMU_GLB_PWR_STATE0_SET_ISP_SET_LEN      4
#define PMU_GLB_PWR_STATE0_SET_ISP_SET_MASK     0x0F000000
#define PMU_GLB_PWR_STATE0_SET_ISP_SET_VAL(w)   (((w) & 0x0F000000) >> 24)
#define PMU_GLB_PWR_STATE0_SET_ISP_SET_SET(w)   w |= ( 0xF << 24)
#define PMU_GLB_PWR_STATE0_SET_ISP_SET_CLR(w)   w &= ~( 0xF << 24)

#define PMU_GLB_PWR_STATE0_SET_CPU_SET_POS      20
#define PMU_GLB_PWR_STATE0_SET_CPU_SET_LEN      4
#define PMU_GLB_PWR_STATE0_SET_CPU_SET_MASK     0x00F00000
#define PMU_GLB_PWR_STATE0_SET_CPU_SET_VAL(w)   (((w) & 0x00F00000) >> 20)
#define PMU_GLB_PWR_STATE0_SET_CPU_SET_SET(w)   w |= ( 0xF << 20)
#define PMU_GLB_PWR_STATE0_SET_CPU_SET_CLR(w)   w &= ~( 0xF << 20)

#define PMU_GLB_PWR_STATE0_SET_NPU_SET_POS      16
#define PMU_GLB_PWR_STATE0_SET_NPU_SET_LEN      4
#define PMU_GLB_PWR_STATE0_SET_NPU_SET_MASK     0x000F0000
#define PMU_GLB_PWR_STATE0_SET_NPU_SET_VAL(w)   (((w) & 0x000F0000) >> 16)
#define PMU_GLB_PWR_STATE0_SET_NPU_SET_SET(w)   w |= ( 0xF << 16)
#define PMU_GLB_PWR_STATE0_SET_NPU_SET_CLR(w)   w &= ~( 0xF << 16)

#define PMU_GLB_PWR_STATE0_SET_DSP_SET_POS      12
#define PMU_GLB_PWR_STATE0_SET_DSP_SET_LEN      4
#define PMU_GLB_PWR_STATE0_SET_DSP_SET_MASK     0x0000F000
#define PMU_GLB_PWR_STATE0_SET_DSP_SET_VAL(w)   (((w) & 0x0000F000) >> 12)
#define PMU_GLB_PWR_STATE0_SET_DSP_SET_SET(w)   w |= ( 0xF << 12)
#define PMU_GLB_PWR_STATE0_SET_DSP_SET_CLR(w)   w &= ~( 0xF << 12)

#define PMU_GLB_PWR_STATE0_SET_VDEC_SET_POS      8
#define PMU_GLB_PWR_STATE0_SET_VDEC_SET_LEN      4
#define PMU_GLB_PWR_STATE0_SET_VDEC_SET_MASK     0x00000F00
#define PMU_GLB_PWR_STATE0_SET_VDEC_SET_VAL(w)   (((w) & 0x00000F00) >> 8)
#define PMU_GLB_PWR_STATE0_SET_VDEC_SET_SET(w)   w |= ( 0xF << 8)
#define PMU_GLB_PWR_STATE0_SET_VDEC_SET_CLR(w)   w &= ~( 0xF << 8)

#define PMU_GLB_PWR_STATE0_SET_VENC1_SET_POS      4
#define PMU_GLB_PWR_STATE0_SET_VENC1_SET_LEN      4
#define PMU_GLB_PWR_STATE0_SET_VENC1_SET_MASK     0x000000F0
#define PMU_GLB_PWR_STATE0_SET_VENC1_SET_VAL(w)   (((w) & 0x000000F0) >> 4)
#define PMU_GLB_PWR_STATE0_SET_VENC1_SET_SET(w)   w |= ( 0xF << 4)
#define PMU_GLB_PWR_STATE0_SET_VENC1_SET_CLR(w)   w &= ~( 0xF << 4)

#define PMU_GLB_PWR_STATE0_SET_VENC0_SET_POS      0
#define PMU_GLB_PWR_STATE0_SET_VENC0_SET_LEN      4
#define PMU_GLB_PWR_STATE0_SET_VENC0_SET_MASK     0x0000000F
#define PMU_GLB_PWR_STATE0_SET_VENC0_SET_VAL(w)   (((w) & 0x0000000F) >> 0)
#define PMU_GLB_PWR_STATE0_SET_VENC0_SET_SET(w)   w |= ( 0xF << 0)
#define PMU_GLB_PWR_STATE0_SET_VENC0_SET_CLR(w)   w &= ~( 0xF << 0)

// REGISTER : PWR_STATE0_CLR

#define PMU_GLB_PWR_STATE0_CLR_ADDR      PMU_GLB_BASE_ADDR + 0x110
#define PMU_GLB_PWR_STATE0_CLR_RSTVAL    0x0
#define PMU_GLB_PWR_STATE0_CLR_ALL1      0x0
#define PMU_GLB_PWR_STATE0_CLR_ALL0      0x0
#define PMU_GLB_PWR_STATE0_CLR_ALL5A     0x0
#define PMU_GLB_PWR_STATE0_CLR_ALLA5     0x0

// FIELD IN REGISTER : PWR_STATE0_CLR

#define PMU_GLB_PWR_STATE0_CLR_ISP_CLR_POS      24
#define PMU_GLB_PWR_STATE0_CLR_ISP_CLR_LEN      4
#define PMU_GLB_PWR_STATE0_CLR_ISP_CLR_MASK     0x0F000000
#define PMU_GLB_PWR_STATE0_CLR_ISP_CLR_VAL(w)   (((w) & 0x0F000000) >> 24)
#define PMU_GLB_PWR_STATE0_CLR_ISP_CLR_SET(w)   w |= ( 0xF << 24)
#define PMU_GLB_PWR_STATE0_CLR_ISP_CLR_CLR(w)   w &= ~( 0xF << 24)

#define PMU_GLB_PWR_STATE0_CLR_CPU_CLR_POS      20
#define PMU_GLB_PWR_STATE0_CLR_CPU_CLR_LEN      4
#define PMU_GLB_PWR_STATE0_CLR_CPU_CLR_MASK     0x00F00000
#define PMU_GLB_PWR_STATE0_CLR_CPU_CLR_VAL(w)   (((w) & 0x00F00000) >> 20)
#define PMU_GLB_PWR_STATE0_CLR_CPU_CLR_SET(w)   w |= ( 0xF << 20)
#define PMU_GLB_PWR_STATE0_CLR_CPU_CLR_CLR(w)   w &= ~( 0xF << 20)

#define PMU_GLB_PWR_STATE0_CLR_NPU_CLR_POS      16
#define PMU_GLB_PWR_STATE0_CLR_NPU_CLR_LEN      4
#define PMU_GLB_PWR_STATE0_CLR_NPU_CLR_MASK     0x000F0000
#define PMU_GLB_PWR_STATE0_CLR_NPU_CLR_VAL(w)   (((w) & 0x000F0000) >> 16)
#define PMU_GLB_PWR_STATE0_CLR_NPU_CLR_SET(w)   w |= ( 0xF << 16)
#define PMU_GLB_PWR_STATE0_CLR_NPU_CLR_CLR(w)   w &= ~( 0xF << 16)

#define PMU_GLB_PWR_STATE0_CLR_DSP_CLR_POS      12
#define PMU_GLB_PWR_STATE0_CLR_DSP_CLR_LEN      4
#define PMU_GLB_PWR_STATE0_CLR_DSP_CLR_MASK     0x0000F000
#define PMU_GLB_PWR_STATE0_CLR_DSP_CLR_VAL(w)   (((w) & 0x0000F000) >> 12)
#define PMU_GLB_PWR_STATE0_CLR_DSP_CLR_SET(w)   w |= ( 0xF << 12)
#define PMU_GLB_PWR_STATE0_CLR_DSP_CLR_CLR(w)   w &= ~( 0xF << 12)

#define PMU_GLB_PWR_STATE0_CLR_VDEC_CLR_POS      8
#define PMU_GLB_PWR_STATE0_CLR_VDEC_CLR_LEN      4
#define PMU_GLB_PWR_STATE0_CLR_VDEC_CLR_MASK     0x00000F00
#define PMU_GLB_PWR_STATE0_CLR_VDEC_CLR_VAL(w)   (((w) & 0x00000F00) >> 8)
#define PMU_GLB_PWR_STATE0_CLR_VDEC_CLR_SET(w)   w |= ( 0xF << 8)
#define PMU_GLB_PWR_STATE0_CLR_VDEC_CLR_CLR(w)   w &= ~( 0xF << 8)

#define PMU_GLB_PWR_STATE0_CLR_VENC1_CLR_POS      4
#define PMU_GLB_PWR_STATE0_CLR_VENC1_CLR_LEN      4
#define PMU_GLB_PWR_STATE0_CLR_VENC1_CLR_MASK     0x000000F0
#define PMU_GLB_PWR_STATE0_CLR_VENC1_CLR_VAL(w)   (((w) & 0x000000F0) >> 4)
#define PMU_GLB_PWR_STATE0_CLR_VENC1_CLR_SET(w)   w |= ( 0xF << 4)
#define PMU_GLB_PWR_STATE0_CLR_VENC1_CLR_CLR(w)   w &= ~( 0xF << 4)

#define PMU_GLB_PWR_STATE0_CLR_VENC0_CLR_POS      0
#define PMU_GLB_PWR_STATE0_CLR_VENC0_CLR_LEN      4
#define PMU_GLB_PWR_STATE0_CLR_VENC0_CLR_MASK     0x0000000F
#define PMU_GLB_PWR_STATE0_CLR_VENC0_CLR_VAL(w)   (((w) & 0x0000000F) >> 0)
#define PMU_GLB_PWR_STATE0_CLR_VENC0_CLR_SET(w)   w |= ( 0xF << 0)
#define PMU_GLB_PWR_STATE0_CLR_VENC0_CLR_CLR(w)   w &= ~( 0xF << 0)

// REGISTER : PWR_STATE1_SET

#define PMU_GLB_PWR_STATE1_SET_ADDR      PMU_GLB_BASE_ADDR + 0x114
#define PMU_GLB_PWR_STATE1_SET_RSTVAL    0x0
#define PMU_GLB_PWR_STATE1_SET_ALL1      0x0
#define PMU_GLB_PWR_STATE1_SET_ALL0      0x0
#define PMU_GLB_PWR_STATE1_SET_ALL5A     0x0
#define PMU_GLB_PWR_STATE1_SET_ALLA5     0x0

// FIELD IN REGISTER : PWR_STATE1_SET

#define PMU_GLB_PWR_STATE1_SET_CPU7_SET_POS      28
#define PMU_GLB_PWR_STATE1_SET_CPU7_SET_LEN      4
#define PMU_GLB_PWR_STATE1_SET_CPU7_SET_MASK     0xF0000000
#define PMU_GLB_PWR_STATE1_SET_CPU7_SET_VAL(w)   (((w) & 0xF0000000) >> 28)
#define PMU_GLB_PWR_STATE1_SET_CPU7_SET_SET(w)   w |= ( 0xF << 28)
#define PMU_GLB_PWR_STATE1_SET_CPU7_SET_CLR(w)   w &= ~( 0xF << 28)

#define PMU_GLB_PWR_STATE1_SET_CPU6_SET_POS      24
#define PMU_GLB_PWR_STATE1_SET_CPU6_SET_LEN      4
#define PMU_GLB_PWR_STATE1_SET_CPU6_SET_MASK     0x0F000000
#define PMU_GLB_PWR_STATE1_SET_CPU6_SET_VAL(w)   (((w) & 0x0F000000) >> 24)
#define PMU_GLB_PWR_STATE1_SET_CPU6_SET_SET(w)   w |= ( 0xF << 24)
#define PMU_GLB_PWR_STATE1_SET_CPU6_SET_CLR(w)   w &= ~( 0xF << 24)

#define PMU_GLB_PWR_STATE1_SET_CPU5_SET_POS      20
#define PMU_GLB_PWR_STATE1_SET_CPU5_SET_LEN      4
#define PMU_GLB_PWR_STATE1_SET_CPU5_SET_MASK     0x00F00000
#define PMU_GLB_PWR_STATE1_SET_CPU5_SET_VAL(w)   (((w) & 0x00F00000) >> 20)
#define PMU_GLB_PWR_STATE1_SET_CPU5_SET_SET(w)   w |= ( 0xF << 20)
#define PMU_GLB_PWR_STATE1_SET_CPU5_SET_CLR(w)   w &= ~( 0xF << 20)

#define PMU_GLB_PWR_STATE1_SET_CPU4_SET_POS      16
#define PMU_GLB_PWR_STATE1_SET_CPU4_SET_LEN      4
#define PMU_GLB_PWR_STATE1_SET_CPU4_SET_MASK     0x000F0000
#define PMU_GLB_PWR_STATE1_SET_CPU4_SET_VAL(w)   (((w) & 0x000F0000) >> 16)
#define PMU_GLB_PWR_STATE1_SET_CPU4_SET_SET(w)   w |= ( 0xF << 16)
#define PMU_GLB_PWR_STATE1_SET_CPU4_SET_CLR(w)   w &= ~( 0xF << 16)

#define PMU_GLB_PWR_STATE1_SET_CPU3_SET_POS      12
#define PMU_GLB_PWR_STATE1_SET_CPU3_SET_LEN      4
#define PMU_GLB_PWR_STATE1_SET_CPU3_SET_MASK     0x0000F000
#define PMU_GLB_PWR_STATE1_SET_CPU3_SET_VAL(w)   (((w) & 0x0000F000) >> 12)
#define PMU_GLB_PWR_STATE1_SET_CPU3_SET_SET(w)   w |= ( 0xF << 12)
#define PMU_GLB_PWR_STATE1_SET_CPU3_SET_CLR(w)   w &= ~( 0xF << 12)

#define PMU_GLB_PWR_STATE1_SET_CPU2_SET_POS      8
#define PMU_GLB_PWR_STATE1_SET_CPU2_SET_LEN      4
#define PMU_GLB_PWR_STATE1_SET_CPU2_SET_MASK     0x00000F00
#define PMU_GLB_PWR_STATE1_SET_CPU2_SET_VAL(w)   (((w) & 0x00000F00) >> 8)
#define PMU_GLB_PWR_STATE1_SET_CPU2_SET_SET(w)   w |= ( 0xF << 8)
#define PMU_GLB_PWR_STATE1_SET_CPU2_SET_CLR(w)   w &= ~( 0xF << 8)

#define PMU_GLB_PWR_STATE1_SET_CPU1_SET_POS      4
#define PMU_GLB_PWR_STATE1_SET_CPU1_SET_LEN      4
#define PMU_GLB_PWR_STATE1_SET_CPU1_SET_MASK     0x000000F0
#define PMU_GLB_PWR_STATE1_SET_CPU1_SET_VAL(w)   (((w) & 0x000000F0) >> 4)
#define PMU_GLB_PWR_STATE1_SET_CPU1_SET_SET(w)   w |= ( 0xF << 4)
#define PMU_GLB_PWR_STATE1_SET_CPU1_SET_CLR(w)   w &= ~( 0xF << 4)

#define PMU_GLB_PWR_STATE1_SET_CPU0_SET_POS      0
#define PMU_GLB_PWR_STATE1_SET_CPU0_SET_LEN      4
#define PMU_GLB_PWR_STATE1_SET_CPU0_SET_MASK     0x0000000F
#define PMU_GLB_PWR_STATE1_SET_CPU0_SET_VAL(w)   (((w) & 0x0000000F) >> 0)
#define PMU_GLB_PWR_STATE1_SET_CPU0_SET_SET(w)   w |= ( 0xF << 0)
#define PMU_GLB_PWR_STATE1_SET_CPU0_SET_CLR(w)   w &= ~( 0xF << 0)

// REGISTER : PWR_STATE1_CLR

#define PMU_GLB_PWR_STATE1_CLR_ADDR      PMU_GLB_BASE_ADDR + 0x118
#define PMU_GLB_PWR_STATE1_CLR_RSTVAL    0x0
#define PMU_GLB_PWR_STATE1_CLR_ALL1      0x0
#define PMU_GLB_PWR_STATE1_CLR_ALL0      0x0
#define PMU_GLB_PWR_STATE1_CLR_ALL5A     0x0
#define PMU_GLB_PWR_STATE1_CLR_ALLA5     0x0

// FIELD IN REGISTER : PWR_STATE1_CLR

#define PMU_GLB_PWR_STATE1_CLR_CPU7_CLR_POS      28
#define PMU_GLB_PWR_STATE1_CLR_CPU7_CLR_LEN      4
#define PMU_GLB_PWR_STATE1_CLR_CPU7_CLR_MASK     0xF0000000
#define PMU_GLB_PWR_STATE1_CLR_CPU7_CLR_VAL(w)   (((w) & 0xF0000000) >> 28)
#define PMU_GLB_PWR_STATE1_CLR_CPU7_CLR_SET(w)   w |= ( 0xF << 28)
#define PMU_GLB_PWR_STATE1_CLR_CPU7_CLR_CLR(w)   w &= ~( 0xF << 28)

#define PMU_GLB_PWR_STATE1_CLR_CPU6_CLR_POS      24
#define PMU_GLB_PWR_STATE1_CLR_CPU6_CLR_LEN      4
#define PMU_GLB_PWR_STATE1_CLR_CPU6_CLR_MASK     0x0F000000
#define PMU_GLB_PWR_STATE1_CLR_CPU6_CLR_VAL(w)   (((w) & 0x0F000000) >> 24)
#define PMU_GLB_PWR_STATE1_CLR_CPU6_CLR_SET(w)   w |= ( 0xF << 24)
#define PMU_GLB_PWR_STATE1_CLR_CPU6_CLR_CLR(w)   w &= ~( 0xF << 24)

#define PMU_GLB_PWR_STATE1_CLR_CPU5_CLR_POS      20
#define PMU_GLB_PWR_STATE1_CLR_CPU5_CLR_LEN      4
#define PMU_GLB_PWR_STATE1_CLR_CPU5_CLR_MASK     0x00F00000
#define PMU_GLB_PWR_STATE1_CLR_CPU5_CLR_VAL(w)   (((w) & 0x00F00000) >> 20)
#define PMU_GLB_PWR_STATE1_CLR_CPU5_CLR_SET(w)   w |= ( 0xF << 20)
#define PMU_GLB_PWR_STATE1_CLR_CPU5_CLR_CLR(w)   w &= ~( 0xF << 20)

#define PMU_GLB_PWR_STATE1_CLR_CPU4_CLR_POS      16
#define PMU_GLB_PWR_STATE1_CLR_CPU4_CLR_LEN      4
#define PMU_GLB_PWR_STATE1_CLR_CPU4_CLR_MASK     0x000F0000
#define PMU_GLB_PWR_STATE1_CLR_CPU4_CLR_VAL(w)   (((w) & 0x000F0000) >> 16)
#define PMU_GLB_PWR_STATE1_CLR_CPU4_CLR_SET(w)   w |= ( 0xF << 16)
#define PMU_GLB_PWR_STATE1_CLR_CPU4_CLR_CLR(w)   w &= ~( 0xF << 16)

#define PMU_GLB_PWR_STATE1_CLR_CPU3_CLR_POS      12
#define PMU_GLB_PWR_STATE1_CLR_CPU3_CLR_LEN      4
#define PMU_GLB_PWR_STATE1_CLR_CPU3_CLR_MASK     0x0000F000
#define PMU_GLB_PWR_STATE1_CLR_CPU3_CLR_VAL(w)   (((w) & 0x0000F000) >> 12)
#define PMU_GLB_PWR_STATE1_CLR_CPU3_CLR_SET(w)   w |= ( 0xF << 12)
#define PMU_GLB_PWR_STATE1_CLR_CPU3_CLR_CLR(w)   w &= ~( 0xF << 12)

#define PMU_GLB_PWR_STATE1_CLR_CPU2_CLR_POS      8
#define PMU_GLB_PWR_STATE1_CLR_CPU2_CLR_LEN      4
#define PMU_GLB_PWR_STATE1_CLR_CPU2_CLR_MASK     0x00000F00
#define PMU_GLB_PWR_STATE1_CLR_CPU2_CLR_VAL(w)   (((w) & 0x00000F00) >> 8)
#define PMU_GLB_PWR_STATE1_CLR_CPU2_CLR_SET(w)   w |= ( 0xF << 8)
#define PMU_GLB_PWR_STATE1_CLR_CPU2_CLR_CLR(w)   w &= ~( 0xF << 8)

#define PMU_GLB_PWR_STATE1_CLR_CPU1_CLR_POS      4
#define PMU_GLB_PWR_STATE1_CLR_CPU1_CLR_LEN      4
#define PMU_GLB_PWR_STATE1_CLR_CPU1_CLR_MASK     0x000000F0
#define PMU_GLB_PWR_STATE1_CLR_CPU1_CLR_VAL(w)   (((w) & 0x000000F0) >> 4)
#define PMU_GLB_PWR_STATE1_CLR_CPU1_CLR_SET(w)   w |= ( 0xF << 4)
#define PMU_GLB_PWR_STATE1_CLR_CPU1_CLR_CLR(w)   w &= ~( 0xF << 4)

#define PMU_GLB_PWR_STATE1_CLR_CPU0_CLR_POS      0
#define PMU_GLB_PWR_STATE1_CLR_CPU0_CLR_LEN      4
#define PMU_GLB_PWR_STATE1_CLR_CPU0_CLR_MASK     0x0000000F
#define PMU_GLB_PWR_STATE1_CLR_CPU0_CLR_VAL(w)   (((w) & 0x0000000F) >> 0)
#define PMU_GLB_PWR_STATE1_CLR_CPU0_CLR_SET(w)   w |= ( 0xF << 0)
#define PMU_GLB_PWR_STATE1_CLR_CPU0_CLR_CLR(w)   w &= ~( 0xF << 0)

// REGISTER : PWR_STATE2_SET

#define PMU_GLB_PWR_STATE2_SET_ADDR      PMU_GLB_BASE_ADDR + 0x11C
#define PMU_GLB_PWR_STATE2_SET_RSTVAL    0x0
#define PMU_GLB_PWR_STATE2_SET_ALL1      0x0
#define PMU_GLB_PWR_STATE2_SET_ALL0      0x0
#define PMU_GLB_PWR_STATE2_SET_ALL5A     0x0
#define PMU_GLB_PWR_STATE2_SET_ALLA5     0x0

// FIELD IN REGISTER : PWR_STATE2_SET

#define PMU_GLB_PWR_STATE2_SET_FLASH_SET_POS      28
#define PMU_GLB_PWR_STATE2_SET_FLASH_SET_LEN      4
#define PMU_GLB_PWR_STATE2_SET_FLASH_SET_MASK     0xF0000000
#define PMU_GLB_PWR_STATE2_SET_FLASH_SET_VAL(w)   (((w) & 0xF0000000) >> 28)
#define PMU_GLB_PWR_STATE2_SET_FLASH_SET_SET(w)   w |= ( 0xF << 28)
#define PMU_GLB_PWR_STATE2_SET_FLASH_SET_CLR(w)   w &= ~( 0xF << 28)

#define PMU_GLB_PWR_STATE2_SET_DDR0_SET_POS      24
#define PMU_GLB_PWR_STATE2_SET_DDR0_SET_LEN      4
#define PMU_GLB_PWR_STATE2_SET_DDR0_SET_MASK     0x0F000000
#define PMU_GLB_PWR_STATE2_SET_DDR0_SET_VAL(w)   (((w) & 0x0F000000) >> 24)
#define PMU_GLB_PWR_STATE2_SET_DDR0_SET_SET(w)   w |= ( 0xF << 24)
#define PMU_GLB_PWR_STATE2_SET_DDR0_SET_CLR(w)   w &= ~( 0xF << 24)

#define PMU_GLB_PWR_STATE2_SET_DDR1_SET_POS      20
#define PMU_GLB_PWR_STATE2_SET_DDR1_SET_LEN      4
#define PMU_GLB_PWR_STATE2_SET_DDR1_SET_MASK     0x00F00000
#define PMU_GLB_PWR_STATE2_SET_DDR1_SET_VAL(w)   (((w) & 0x00F00000) >> 20)
#define PMU_GLB_PWR_STATE2_SET_DDR1_SET_SET(w)   w |= ( 0xF << 20)
#define PMU_GLB_PWR_STATE2_SET_DDR1_SET_CLR(w)   w &= ~( 0xF << 20)

#define PMU_GLB_PWR_STATE2_SET_PERIP_SET_POS      16
#define PMU_GLB_PWR_STATE2_SET_PERIP_SET_LEN      4
#define PMU_GLB_PWR_STATE2_SET_PERIP_SET_MASK     0x000F0000
#define PMU_GLB_PWR_STATE2_SET_PERIP_SET_VAL(w)   (((w) & 0x000F0000) >> 16)
#define PMU_GLB_PWR_STATE2_SET_PERIP_SET_SET(w)   w |= ( 0xF << 16)
#define PMU_GLB_PWR_STATE2_SET_PERIP_SET_CLR(w)   w &= ~( 0xF << 16)

#define PMU_GLB_PWR_STATE2_SET_SEN_SET_POS      12
#define PMU_GLB_PWR_STATE2_SET_SEN_SET_LEN      4
#define PMU_GLB_PWR_STATE2_SET_SEN_SET_MASK     0x0000F000
#define PMU_GLB_PWR_STATE2_SET_SEN_SET_VAL(w)   (((w) & 0x0000F000) >> 12)
#define PMU_GLB_PWR_STATE2_SET_SEN_SET_SET(w)   w |= ( 0xF << 12)
#define PMU_GLB_PWR_STATE2_SET_SEN_SET_CLR(w)   w &= ~( 0xF << 12)

#define PMU_GLB_PWR_STATE2_SET_N8_SET_POS      8
#define PMU_GLB_PWR_STATE2_SET_N8_SET_LEN      4
#define PMU_GLB_PWR_STATE2_SET_N8_SET_MASK     0x00000F00
#define PMU_GLB_PWR_STATE2_SET_N8_SET_VAL(w)   (((w) & 0x00000F00) >> 8)
#define PMU_GLB_PWR_STATE2_SET_N8_SET_SET(w)   w |= ( 0xF << 8)
#define PMU_GLB_PWR_STATE2_SET_N8_SET_CLR(w)   w &= ~( 0xF << 8)

#define PMU_GLB_PWR_STATE2_SET_MM_SET_POS      4
#define PMU_GLB_PWR_STATE2_SET_MM_SET_LEN      4
#define PMU_GLB_PWR_STATE2_SET_MM_SET_MASK     0x000000F0
#define PMU_GLB_PWR_STATE2_SET_MM_SET_VAL(w)   (((w) & 0x000000F0) >> 4)
#define PMU_GLB_PWR_STATE2_SET_MM_SET_SET(w)   w |= ( 0xF << 4)
#define PMU_GLB_PWR_STATE2_SET_MM_SET_CLR(w)   w &= ~( 0xF << 4)

#define PMU_GLB_PWR_STATE2_SET_PIPE_SET_POS      0
#define PMU_GLB_PWR_STATE2_SET_PIPE_SET_LEN      4
#define PMU_GLB_PWR_STATE2_SET_PIPE_SET_MASK     0x0000000F
#define PMU_GLB_PWR_STATE2_SET_PIPE_SET_VAL(w)   (((w) & 0x0000000F) >> 0)
#define PMU_GLB_PWR_STATE2_SET_PIPE_SET_SET(w)   w |= ( 0xF << 0)
#define PMU_GLB_PWR_STATE2_SET_PIPE_SET_CLR(w)   w &= ~( 0xF << 0)

// REGISTER : PWR_STATE2_CLR

#define PMU_GLB_PWR_STATE2_CLR_ADDR      PMU_GLB_BASE_ADDR + 0x120
#define PMU_GLB_PWR_STATE2_CLR_RSTVAL    0x0
#define PMU_GLB_PWR_STATE2_CLR_ALL1      0x0
#define PMU_GLB_PWR_STATE2_CLR_ALL0      0x0
#define PMU_GLB_PWR_STATE2_CLR_ALL5A     0x0
#define PMU_GLB_PWR_STATE2_CLR_ALLA5     0x0

// FIELD IN REGISTER : PWR_STATE2_CLR

#define PMU_GLB_PWR_STATE2_CLR_FLASH_CLR_POS      28
#define PMU_GLB_PWR_STATE2_CLR_FLASH_CLR_LEN      4
#define PMU_GLB_PWR_STATE2_CLR_FLASH_CLR_MASK     0xF0000000
#define PMU_GLB_PWR_STATE2_CLR_FLASH_CLR_VAL(w)   (((w) & 0xF0000000) >> 28)
#define PMU_GLB_PWR_STATE2_CLR_FLASH_CLR_SET(w)   w |= ( 0xF << 28)
#define PMU_GLB_PWR_STATE2_CLR_FLASH_CLR_CLR(w)   w &= ~( 0xF << 28)

#define PMU_GLB_PWR_STATE2_CLR_DDR0_CLR_POS      24
#define PMU_GLB_PWR_STATE2_CLR_DDR0_CLR_LEN      4
#define PMU_GLB_PWR_STATE2_CLR_DDR0_CLR_MASK     0x0F000000
#define PMU_GLB_PWR_STATE2_CLR_DDR0_CLR_VAL(w)   (((w) & 0x0F000000) >> 24)
#define PMU_GLB_PWR_STATE2_CLR_DDR0_CLR_SET(w)   w |= ( 0xF << 24)
#define PMU_GLB_PWR_STATE2_CLR_DDR0_CLR_CLR(w)   w &= ~( 0xF << 24)

#define PMU_GLB_PWR_STATE2_CLR_DDR1_CLR_POS      20
#define PMU_GLB_PWR_STATE2_CLR_DDR1_CLR_LEN      4
#define PMU_GLB_PWR_STATE2_CLR_DDR1_CLR_MASK     0x00F00000
#define PMU_GLB_PWR_STATE2_CLR_DDR1_CLR_VAL(w)   (((w) & 0x00F00000) >> 20)
#define PMU_GLB_PWR_STATE2_CLR_DDR1_CLR_SET(w)   w |= ( 0xF << 20)
#define PMU_GLB_PWR_STATE2_CLR_DDR1_CLR_CLR(w)   w &= ~( 0xF << 20)

#define PMU_GLB_PWR_STATE2_CLR_PERIP_CLR_POS      16
#define PMU_GLB_PWR_STATE2_CLR_PERIP_CLR_LEN      4
#define PMU_GLB_PWR_STATE2_CLR_PERIP_CLR_MASK     0x000F0000
#define PMU_GLB_PWR_STATE2_CLR_PERIP_CLR_VAL(w)   (((w) & 0x000F0000) >> 16)
#define PMU_GLB_PWR_STATE2_CLR_PERIP_CLR_SET(w)   w |= ( 0xF << 16)
#define PMU_GLB_PWR_STATE2_CLR_PERIP_CLR_CLR(w)   w &= ~( 0xF << 16)

#define PMU_GLB_PWR_STATE2_CLR_SEN_CLR_POS      12
#define PMU_GLB_PWR_STATE2_CLR_SEN_CLR_LEN      4
#define PMU_GLB_PWR_STATE2_CLR_SEN_CLR_MASK     0x0000F000
#define PMU_GLB_PWR_STATE2_CLR_SEN_CLR_VAL(w)   (((w) & 0x0000F000) >> 12)
#define PMU_GLB_PWR_STATE2_CLR_SEN_CLR_SET(w)   w |= ( 0xF << 12)
#define PMU_GLB_PWR_STATE2_CLR_SEN_CLR_CLR(w)   w &= ~( 0xF << 12)

#define PMU_GLB_PWR_STATE2_CLR_N8_CLR_POS      8
#define PMU_GLB_PWR_STATE2_CLR_N8_CLR_LEN      4
#define PMU_GLB_PWR_STATE2_CLR_N8_CLR_MASK     0x00000F00
#define PMU_GLB_PWR_STATE2_CLR_N8_CLR_VAL(w)   (((w) & 0x00000F00) >> 8)
#define PMU_GLB_PWR_STATE2_CLR_N8_CLR_SET(w)   w |= ( 0xF << 8)
#define PMU_GLB_PWR_STATE2_CLR_N8_CLR_CLR(w)   w &= ~( 0xF << 8)

#define PMU_GLB_PWR_STATE2_CLR_MM_CLR_POS      4
#define PMU_GLB_PWR_STATE2_CLR_MM_CLR_LEN      4
#define PMU_GLB_PWR_STATE2_CLR_MM_CLR_MASK     0x000000F0
#define PMU_GLB_PWR_STATE2_CLR_MM_CLR_VAL(w)   (((w) & 0x000000F0) >> 4)
#define PMU_GLB_PWR_STATE2_CLR_MM_CLR_SET(w)   w |= ( 0xF << 4)
#define PMU_GLB_PWR_STATE2_CLR_MM_CLR_CLR(w)   w &= ~( 0xF << 4)

#define PMU_GLB_PWR_STATE2_CLR_PIPE_CLR_POS      0
#define PMU_GLB_PWR_STATE2_CLR_PIPE_CLR_LEN      4
#define PMU_GLB_PWR_STATE2_CLR_PIPE_CLR_MASK     0x0000000F
#define PMU_GLB_PWR_STATE2_CLR_PIPE_CLR_VAL(w)   (((w) & 0x0000000F) >> 0)
#define PMU_GLB_PWR_STATE2_CLR_PIPE_CLR_SET(w)   w |= ( 0xF << 0)
#define PMU_GLB_PWR_STATE2_CLR_PIPE_CLR_CLR(w)   w &= ~( 0xF << 0)

// REGISTER : PWR_STATE3_SET

#define PMU_GLB_PWR_STATE3_SET_ADDR      PMU_GLB_BASE_ADDR + 0x124
#define PMU_GLB_PWR_STATE3_SET_RSTVAL    0x0
#define PMU_GLB_PWR_STATE3_SET_ALL1      0x0
#define PMU_GLB_PWR_STATE3_SET_ALL0      0x0
#define PMU_GLB_PWR_STATE3_SET_ALL5A     0x0
#define PMU_GLB_PWR_STATE3_SET_ALLA5     0x0

// FIELD IN REGISTER : PWR_STATE3_SET

#define PMU_GLB_PWR_STATE3_SET_N7_SET_POS      28
#define PMU_GLB_PWR_STATE3_SET_N7_SET_LEN      4
#define PMU_GLB_PWR_STATE3_SET_N7_SET_MASK     0xF0000000
#define PMU_GLB_PWR_STATE3_SET_N7_SET_VAL(w)   (((w) & 0xF0000000) >> 28)
#define PMU_GLB_PWR_STATE3_SET_N7_SET_SET(w)   w |= ( 0xF << 28)
#define PMU_GLB_PWR_STATE3_SET_N7_SET_CLR(w)   w &= ~( 0xF << 28)

#define PMU_GLB_PWR_STATE3_SET_N6_SET_POS      24
#define PMU_GLB_PWR_STATE3_SET_N6_SET_LEN      4
#define PMU_GLB_PWR_STATE3_SET_N6_SET_MASK     0x0F000000
#define PMU_GLB_PWR_STATE3_SET_N6_SET_VAL(w)   (((w) & 0x0F000000) >> 24)
#define PMU_GLB_PWR_STATE3_SET_N6_SET_SET(w)   w |= ( 0xF << 24)
#define PMU_GLB_PWR_STATE3_SET_N6_SET_CLR(w)   w &= ~( 0xF << 24)

#define PMU_GLB_PWR_STATE3_SET_N5_SET_POS      20
#define PMU_GLB_PWR_STATE3_SET_N5_SET_LEN      4
#define PMU_GLB_PWR_STATE3_SET_N5_SET_MASK     0x00F00000
#define PMU_GLB_PWR_STATE3_SET_N5_SET_VAL(w)   (((w) & 0x00F00000) >> 20)
#define PMU_GLB_PWR_STATE3_SET_N5_SET_SET(w)   w |= ( 0xF << 20)
#define PMU_GLB_PWR_STATE3_SET_N5_SET_CLR(w)   w &= ~( 0xF << 20)

#define PMU_GLB_PWR_STATE3_SET_N4_SET_POS      16
#define PMU_GLB_PWR_STATE3_SET_N4_SET_LEN      4
#define PMU_GLB_PWR_STATE3_SET_N4_SET_MASK     0x000F0000
#define PMU_GLB_PWR_STATE3_SET_N4_SET_VAL(w)   (((w) & 0x000F0000) >> 16)
#define PMU_GLB_PWR_STATE3_SET_N4_SET_SET(w)   w |= ( 0xF << 16)
#define PMU_GLB_PWR_STATE3_SET_N4_SET_CLR(w)   w &= ~( 0xF << 16)

#define PMU_GLB_PWR_STATE3_SET_N3_SET_POS      12
#define PMU_GLB_PWR_STATE3_SET_N3_SET_LEN      4
#define PMU_GLB_PWR_STATE3_SET_N3_SET_MASK     0x0000F000
#define PMU_GLB_PWR_STATE3_SET_N3_SET_VAL(w)   (((w) & 0x0000F000) >> 12)
#define PMU_GLB_PWR_STATE3_SET_N3_SET_SET(w)   w |= ( 0xF << 12)
#define PMU_GLB_PWR_STATE3_SET_N3_SET_CLR(w)   w &= ~( 0xF << 12)

#define PMU_GLB_PWR_STATE3_SET_N2_SET_POS      8
#define PMU_GLB_PWR_STATE3_SET_N2_SET_LEN      4
#define PMU_GLB_PWR_STATE3_SET_N2_SET_MASK     0x00000F00
#define PMU_GLB_PWR_STATE3_SET_N2_SET_VAL(w)   (((w) & 0x00000F00) >> 8)
#define PMU_GLB_PWR_STATE3_SET_N2_SET_SET(w)   w |= ( 0xF << 8)
#define PMU_GLB_PWR_STATE3_SET_N2_SET_CLR(w)   w &= ~( 0xF << 8)

#define PMU_GLB_PWR_STATE3_SET_N1_SET_POS      4
#define PMU_GLB_PWR_STATE3_SET_N1_SET_LEN      4
#define PMU_GLB_PWR_STATE3_SET_N1_SET_MASK     0x000000F0
#define PMU_GLB_PWR_STATE3_SET_N1_SET_VAL(w)   (((w) & 0x000000F0) >> 4)
#define PMU_GLB_PWR_STATE3_SET_N1_SET_SET(w)   w |= ( 0xF << 4)
#define PMU_GLB_PWR_STATE3_SET_N1_SET_CLR(w)   w &= ~( 0xF << 4)

#define PMU_GLB_PWR_STATE3_SET_N0_SET_POS      0
#define PMU_GLB_PWR_STATE3_SET_N0_SET_LEN      4
#define PMU_GLB_PWR_STATE3_SET_N0_SET_MASK     0x0000000F
#define PMU_GLB_PWR_STATE3_SET_N0_SET_VAL(w)   (((w) & 0x0000000F) >> 0)
#define PMU_GLB_PWR_STATE3_SET_N0_SET_SET(w)   w |= ( 0xF << 0)
#define PMU_GLB_PWR_STATE3_SET_N0_SET_CLR(w)   w &= ~( 0xF << 0)

// REGISTER : PWR_STATE3_CLR

#define PMU_GLB_PWR_STATE3_CLR_ADDR      PMU_GLB_BASE_ADDR + 0x128
#define PMU_GLB_PWR_STATE3_CLR_RSTVAL    0x0
#define PMU_GLB_PWR_STATE3_CLR_ALL1      0x0
#define PMU_GLB_PWR_STATE3_CLR_ALL0      0x0
#define PMU_GLB_PWR_STATE3_CLR_ALL5A     0x0
#define PMU_GLB_PWR_STATE3_CLR_ALLA5     0x0

// FIELD IN REGISTER : PWR_STATE3_CLR

#define PMU_GLB_PWR_STATE3_CLR_N7_CLR_POS      28
#define PMU_GLB_PWR_STATE3_CLR_N7_CLR_LEN      4
#define PMU_GLB_PWR_STATE3_CLR_N7_CLR_MASK     0xF0000000
#define PMU_GLB_PWR_STATE3_CLR_N7_CLR_VAL(w)   (((w) & 0xF0000000) >> 28)
#define PMU_GLB_PWR_STATE3_CLR_N7_CLR_SET(w)   w |= ( 0xF << 28)
#define PMU_GLB_PWR_STATE3_CLR_N7_CLR_CLR(w)   w &= ~( 0xF << 28)

#define PMU_GLB_PWR_STATE3_CLR_N6_CLR_POS      24
#define PMU_GLB_PWR_STATE3_CLR_N6_CLR_LEN      4
#define PMU_GLB_PWR_STATE3_CLR_N6_CLR_MASK     0x0F000000
#define PMU_GLB_PWR_STATE3_CLR_N6_CLR_VAL(w)   (((w) & 0x0F000000) >> 24)
#define PMU_GLB_PWR_STATE3_CLR_N6_CLR_SET(w)   w |= ( 0xF << 24)
#define PMU_GLB_PWR_STATE3_CLR_N6_CLR_CLR(w)   w &= ~( 0xF << 24)

#define PMU_GLB_PWR_STATE3_CLR_N5_CLR_POS      20
#define PMU_GLB_PWR_STATE3_CLR_N5_CLR_LEN      4
#define PMU_GLB_PWR_STATE3_CLR_N5_CLR_MASK     0x00F00000
#define PMU_GLB_PWR_STATE3_CLR_N5_CLR_VAL(w)   (((w) & 0x00F00000) >> 20)
#define PMU_GLB_PWR_STATE3_CLR_N5_CLR_SET(w)   w |= ( 0xF << 20)
#define PMU_GLB_PWR_STATE3_CLR_N5_CLR_CLR(w)   w &= ~( 0xF << 20)

#define PMU_GLB_PWR_STATE3_CLR_N4_CLR_POS      16
#define PMU_GLB_PWR_STATE3_CLR_N4_CLR_LEN      4
#define PMU_GLB_PWR_STATE3_CLR_N4_CLR_MASK     0x000F0000
#define PMU_GLB_PWR_STATE3_CLR_N4_CLR_VAL(w)   (((w) & 0x000F0000) >> 16)
#define PMU_GLB_PWR_STATE3_CLR_N4_CLR_SET(w)   w |= ( 0xF << 16)
#define PMU_GLB_PWR_STATE3_CLR_N4_CLR_CLR(w)   w &= ~( 0xF << 16)

#define PMU_GLB_PWR_STATE3_CLR_N3_CLR_POS      12
#define PMU_GLB_PWR_STATE3_CLR_N3_CLR_LEN      4
#define PMU_GLB_PWR_STATE3_CLR_N3_CLR_MASK     0x0000F000
#define PMU_GLB_PWR_STATE3_CLR_N3_CLR_VAL(w)   (((w) & 0x0000F000) >> 12)
#define PMU_GLB_PWR_STATE3_CLR_N3_CLR_SET(w)   w |= ( 0xF << 12)
#define PMU_GLB_PWR_STATE3_CLR_N3_CLR_CLR(w)   w &= ~( 0xF << 12)

#define PMU_GLB_PWR_STATE3_CLR_N2_CLR_POS      8
#define PMU_GLB_PWR_STATE3_CLR_N2_CLR_LEN      4
#define PMU_GLB_PWR_STATE3_CLR_N2_CLR_MASK     0x00000F00
#define PMU_GLB_PWR_STATE3_CLR_N2_CLR_VAL(w)   (((w) & 0x00000F00) >> 8)
#define PMU_GLB_PWR_STATE3_CLR_N2_CLR_SET(w)   w |= ( 0xF << 8)
#define PMU_GLB_PWR_STATE3_CLR_N2_CLR_CLR(w)   w &= ~( 0xF << 8)

#define PMU_GLB_PWR_STATE3_CLR_N1_CLR_POS      4
#define PMU_GLB_PWR_STATE3_CLR_N1_CLR_LEN      4
#define PMU_GLB_PWR_STATE3_CLR_N1_CLR_MASK     0x000000F0
#define PMU_GLB_PWR_STATE3_CLR_N1_CLR_VAL(w)   (((w) & 0x000000F0) >> 4)
#define PMU_GLB_PWR_STATE3_CLR_N1_CLR_SET(w)   w |= ( 0xF << 4)
#define PMU_GLB_PWR_STATE3_CLR_N1_CLR_CLR(w)   w &= ~( 0xF << 4)

#define PMU_GLB_PWR_STATE3_CLR_N0_CLR_POS      0
#define PMU_GLB_PWR_STATE3_CLR_N0_CLR_LEN      4
#define PMU_GLB_PWR_STATE3_CLR_N0_CLR_MASK     0x0000000F
#define PMU_GLB_PWR_STATE3_CLR_N0_CLR_VAL(w)   (((w) & 0x0000000F) >> 0)
#define PMU_GLB_PWR_STATE3_CLR_N0_CLR_SET(w)   w |= ( 0xF << 0)
#define PMU_GLB_PWR_STATE3_CLR_N0_CLR_CLR(w)   w &= ~( 0xF << 0)

// REGISTER : PWR_WAIT0_SET

#define PMU_GLB_PWR_WAIT0_SET_ADDR      PMU_GLB_BASE_ADDR + 0x12C
#define PMU_GLB_PWR_WAIT0_SET_RSTVAL    0x0
#define PMU_GLB_PWR_WAIT0_SET_ALL1      0x0
#define PMU_GLB_PWR_WAIT0_SET_ALL0      0x0
#define PMU_GLB_PWR_WAIT0_SET_ALL5A     0x0
#define PMU_GLB_PWR_WAIT0_SET_ALLA5     0x0

// FIELD IN REGISTER : PWR_WAIT0_SET

#define PMU_GLB_PWR_WAIT0_SET_DSP_SET_POS      24
#define PMU_GLB_PWR_WAIT0_SET_DSP_SET_LEN      8
#define PMU_GLB_PWR_WAIT0_SET_DSP_SET_MASK     0xFF000000
#define PMU_GLB_PWR_WAIT0_SET_DSP_SET_VAL(w)   (((w) & 0xFF000000) >> 24)
#define PMU_GLB_PWR_WAIT0_SET_DSP_SET_SET(w)   w |= ( 0xFF << 24)
#define PMU_GLB_PWR_WAIT0_SET_DSP_SET_CLR(w)   w &= ~( 0xFF << 24)

#define PMU_GLB_PWR_WAIT0_SET_VDEC_SET_POS      16
#define PMU_GLB_PWR_WAIT0_SET_VDEC_SET_LEN      8
#define PMU_GLB_PWR_WAIT0_SET_VDEC_SET_MASK     0x00FF0000
#define PMU_GLB_PWR_WAIT0_SET_VDEC_SET_VAL(w)   (((w) & 0x00FF0000) >> 16)
#define PMU_GLB_PWR_WAIT0_SET_VDEC_SET_SET(w)   w |= ( 0xFF << 16)
#define PMU_GLB_PWR_WAIT0_SET_VDEC_SET_CLR(w)   w &= ~( 0xFF << 16)

#define PMU_GLB_PWR_WAIT0_SET_VENC1_SET_POS      8
#define PMU_GLB_PWR_WAIT0_SET_VENC1_SET_LEN      8
#define PMU_GLB_PWR_WAIT0_SET_VENC1_SET_MASK     0x0000FF00
#define PMU_GLB_PWR_WAIT0_SET_VENC1_SET_VAL(w)   (((w) & 0x0000FF00) >> 8)
#define PMU_GLB_PWR_WAIT0_SET_VENC1_SET_SET(w)   w |= ( 0xFF << 8)
#define PMU_GLB_PWR_WAIT0_SET_VENC1_SET_CLR(w)   w &= ~( 0xFF << 8)

#define PMU_GLB_PWR_WAIT0_SET_VENC0_SET_POS      0
#define PMU_GLB_PWR_WAIT0_SET_VENC0_SET_LEN      8
#define PMU_GLB_PWR_WAIT0_SET_VENC0_SET_MASK     0x000000FF
#define PMU_GLB_PWR_WAIT0_SET_VENC0_SET_VAL(w)   (((w) & 0x000000FF) >> 0)
#define PMU_GLB_PWR_WAIT0_SET_VENC0_SET_SET(w)   w |= ( 0xFF << 0)
#define PMU_GLB_PWR_WAIT0_SET_VENC0_SET_CLR(w)   w &= ~( 0xFF << 0)

// REGISTER : PWR_WAIT0_CLR

#define PMU_GLB_PWR_WAIT0_CLR_ADDR      PMU_GLB_BASE_ADDR + 0x130
#define PMU_GLB_PWR_WAIT0_CLR_RSTVAL    0x0
#define PMU_GLB_PWR_WAIT0_CLR_ALL1      0x0
#define PMU_GLB_PWR_WAIT0_CLR_ALL0      0x0
#define PMU_GLB_PWR_WAIT0_CLR_ALL5A     0x0
#define PMU_GLB_PWR_WAIT0_CLR_ALLA5     0x0

// FIELD IN REGISTER : PWR_WAIT0_CLR

#define PMU_GLB_PWR_WAIT0_CLR_DSP_CLR_POS      24
#define PMU_GLB_PWR_WAIT0_CLR_DSP_CLR_LEN      8
#define PMU_GLB_PWR_WAIT0_CLR_DSP_CLR_MASK     0xFF000000
#define PMU_GLB_PWR_WAIT0_CLR_DSP_CLR_VAL(w)   (((w) & 0xFF000000) >> 24)
#define PMU_GLB_PWR_WAIT0_CLR_DSP_CLR_SET(w)   w |= ( 0xFF << 24)
#define PMU_GLB_PWR_WAIT0_CLR_DSP_CLR_CLR(w)   w &= ~( 0xFF << 24)

#define PMU_GLB_PWR_WAIT0_CLR_VDEC_CLR_POS      16
#define PMU_GLB_PWR_WAIT0_CLR_VDEC_CLR_LEN      8
#define PMU_GLB_PWR_WAIT0_CLR_VDEC_CLR_MASK     0x00FF0000
#define PMU_GLB_PWR_WAIT0_CLR_VDEC_CLR_VAL(w)   (((w) & 0x00FF0000) >> 16)
#define PMU_GLB_PWR_WAIT0_CLR_VDEC_CLR_SET(w)   w |= ( 0xFF << 16)
#define PMU_GLB_PWR_WAIT0_CLR_VDEC_CLR_CLR(w)   w &= ~( 0xFF << 16)

#define PMU_GLB_PWR_WAIT0_CLR_VENC1_CLR_POS      8
#define PMU_GLB_PWR_WAIT0_CLR_VENC1_CLR_LEN      8
#define PMU_GLB_PWR_WAIT0_CLR_VENC1_CLR_MASK     0x0000FF00
#define PMU_GLB_PWR_WAIT0_CLR_VENC1_CLR_VAL(w)   (((w) & 0x0000FF00) >> 8)
#define PMU_GLB_PWR_WAIT0_CLR_VENC1_CLR_SET(w)   w |= ( 0xFF << 8)
#define PMU_GLB_PWR_WAIT0_CLR_VENC1_CLR_CLR(w)   w &= ~( 0xFF << 8)

#define PMU_GLB_PWR_WAIT0_CLR_VENC0_CLR_POS      0
#define PMU_GLB_PWR_WAIT0_CLR_VENC0_CLR_LEN      8
#define PMU_GLB_PWR_WAIT0_CLR_VENC0_CLR_MASK     0x000000FF
#define PMU_GLB_PWR_WAIT0_CLR_VENC0_CLR_VAL(w)   (((w) & 0x000000FF) >> 0)
#define PMU_GLB_PWR_WAIT0_CLR_VENC0_CLR_SET(w)   w |= ( 0xFF << 0)
#define PMU_GLB_PWR_WAIT0_CLR_VENC0_CLR_CLR(w)   w &= ~( 0xFF << 0)

// REGISTER : PWR_WAIT1_SET

#define PMU_GLB_PWR_WAIT1_SET_ADDR      PMU_GLB_BASE_ADDR + 0x134
#define PMU_GLB_PWR_WAIT1_SET_RSTVAL    0x0
#define PMU_GLB_PWR_WAIT1_SET_ALL1      0x0
#define PMU_GLB_PWR_WAIT1_SET_ALL0      0x0
#define PMU_GLB_PWR_WAIT1_SET_ALL5A     0x0
#define PMU_GLB_PWR_WAIT1_SET_ALLA5     0x0

// FIELD IN REGISTER : PWR_WAIT1_SET

#define PMU_GLB_PWR_WAIT1_SET_CPU7_SET_POS      24
#define PMU_GLB_PWR_WAIT1_SET_CPU7_SET_LEN      8
#define PMU_GLB_PWR_WAIT1_SET_CPU7_SET_MASK     0xFF000000
#define PMU_GLB_PWR_WAIT1_SET_CPU7_SET_VAL(w)   (((w) & 0xFF000000) >> 24)
#define PMU_GLB_PWR_WAIT1_SET_CPU7_SET_SET(w)   w |= ( 0xFF << 24)
#define PMU_GLB_PWR_WAIT1_SET_CPU7_SET_CLR(w)   w &= ~( 0xFF << 24)

#define PMU_GLB_PWR_WAIT1_SET_CPU6_SET_POS      16
#define PMU_GLB_PWR_WAIT1_SET_CPU6_SET_LEN      8
#define PMU_GLB_PWR_WAIT1_SET_CPU6_SET_MASK     0x00FF0000
#define PMU_GLB_PWR_WAIT1_SET_CPU6_SET_VAL(w)   (((w) & 0x00FF0000) >> 16)
#define PMU_GLB_PWR_WAIT1_SET_CPU6_SET_SET(w)   w |= ( 0xFF << 16)
#define PMU_GLB_PWR_WAIT1_SET_CPU6_SET_CLR(w)   w &= ~( 0xFF << 16)

#define PMU_GLB_PWR_WAIT1_SET_CPU5_SET_POS      8
#define PMU_GLB_PWR_WAIT1_SET_CPU5_SET_LEN      8
#define PMU_GLB_PWR_WAIT1_SET_CPU5_SET_MASK     0x0000FF00
#define PMU_GLB_PWR_WAIT1_SET_CPU5_SET_VAL(w)   (((w) & 0x0000FF00) >> 8)
#define PMU_GLB_PWR_WAIT1_SET_CPU5_SET_SET(w)   w |= ( 0xFF << 8)
#define PMU_GLB_PWR_WAIT1_SET_CPU5_SET_CLR(w)   w &= ~( 0xFF << 8)

#define PMU_GLB_PWR_WAIT1_SET_CPU4_SET_POS      0
#define PMU_GLB_PWR_WAIT1_SET_CPU4_SET_LEN      8
#define PMU_GLB_PWR_WAIT1_SET_CPU4_SET_MASK     0x000000FF
#define PMU_GLB_PWR_WAIT1_SET_CPU4_SET_VAL(w)   (((w) & 0x000000FF) >> 0)
#define PMU_GLB_PWR_WAIT1_SET_CPU4_SET_SET(w)   w |= ( 0xFF << 0)
#define PMU_GLB_PWR_WAIT1_SET_CPU4_SET_CLR(w)   w &= ~( 0xFF << 0)

// REGISTER : PWR_WAIT1_CLR

#define PMU_GLB_PWR_WAIT1_CLR_ADDR      PMU_GLB_BASE_ADDR + 0x138
#define PMU_GLB_PWR_WAIT1_CLR_RSTVAL    0x0
#define PMU_GLB_PWR_WAIT1_CLR_ALL1      0x0
#define PMU_GLB_PWR_WAIT1_CLR_ALL0      0x0
#define PMU_GLB_PWR_WAIT1_CLR_ALL5A     0x0
#define PMU_GLB_PWR_WAIT1_CLR_ALLA5     0x0

// FIELD IN REGISTER : PWR_WAIT1_CLR

#define PMU_GLB_PWR_WAIT1_CLR_CPU7_CLR_POS      24
#define PMU_GLB_PWR_WAIT1_CLR_CPU7_CLR_LEN      8
#define PMU_GLB_PWR_WAIT1_CLR_CPU7_CLR_MASK     0xFF000000
#define PMU_GLB_PWR_WAIT1_CLR_CPU7_CLR_VAL(w)   (((w) & 0xFF000000) >> 24)
#define PMU_GLB_PWR_WAIT1_CLR_CPU7_CLR_SET(w)   w |= ( 0xFF << 24)
#define PMU_GLB_PWR_WAIT1_CLR_CPU7_CLR_CLR(w)   w &= ~( 0xFF << 24)

#define PMU_GLB_PWR_WAIT1_CLR_CPU6_CLR_POS      16
#define PMU_GLB_PWR_WAIT1_CLR_CPU6_CLR_LEN      8
#define PMU_GLB_PWR_WAIT1_CLR_CPU6_CLR_MASK     0x00FF0000
#define PMU_GLB_PWR_WAIT1_CLR_CPU6_CLR_VAL(w)   (((w) & 0x00FF0000) >> 16)
#define PMU_GLB_PWR_WAIT1_CLR_CPU6_CLR_SET(w)   w |= ( 0xFF << 16)
#define PMU_GLB_PWR_WAIT1_CLR_CPU6_CLR_CLR(w)   w &= ~( 0xFF << 16)

#define PMU_GLB_PWR_WAIT1_CLR_CPU5_CLR_POS      8
#define PMU_GLB_PWR_WAIT1_CLR_CPU5_CLR_LEN      8
#define PMU_GLB_PWR_WAIT1_CLR_CPU5_CLR_MASK     0x0000FF00
#define PMU_GLB_PWR_WAIT1_CLR_CPU5_CLR_VAL(w)   (((w) & 0x0000FF00) >> 8)
#define PMU_GLB_PWR_WAIT1_CLR_CPU5_CLR_SET(w)   w |= ( 0xFF << 8)
#define PMU_GLB_PWR_WAIT1_CLR_CPU5_CLR_CLR(w)   w &= ~( 0xFF << 8)

#define PMU_GLB_PWR_WAIT1_CLR_CPU4_CLR_POS      0
#define PMU_GLB_PWR_WAIT1_CLR_CPU4_CLR_LEN      8
#define PMU_GLB_PWR_WAIT1_CLR_CPU4_CLR_MASK     0x000000FF
#define PMU_GLB_PWR_WAIT1_CLR_CPU4_CLR_VAL(w)   (((w) & 0x000000FF) >> 0)
#define PMU_GLB_PWR_WAIT1_CLR_CPU4_CLR_SET(w)   w |= ( 0xFF << 0)
#define PMU_GLB_PWR_WAIT1_CLR_CPU4_CLR_CLR(w)   w &= ~( 0xFF << 0)

// REGISTER : PWR_WAIT2_SET

#define PMU_GLB_PWR_WAIT2_SET_ADDR      PMU_GLB_BASE_ADDR + 0x13C
#define PMU_GLB_PWR_WAIT2_SET_RSTVAL    0x0
#define PMU_GLB_PWR_WAIT2_SET_ALL1      0x0
#define PMU_GLB_PWR_WAIT2_SET_ALL0      0x0
#define PMU_GLB_PWR_WAIT2_SET_ALL5A     0x0
#define PMU_GLB_PWR_WAIT2_SET_ALLA5     0x0

// FIELD IN REGISTER : PWR_WAIT2_SET

#define PMU_GLB_PWR_WAIT2_SET_CPU3_SET_POS      24
#define PMU_GLB_PWR_WAIT2_SET_CPU3_SET_LEN      8
#define PMU_GLB_PWR_WAIT2_SET_CPU3_SET_MASK     0xFF000000
#define PMU_GLB_PWR_WAIT2_SET_CPU3_SET_VAL(w)   (((w) & 0xFF000000) >> 24)
#define PMU_GLB_PWR_WAIT2_SET_CPU3_SET_SET(w)   w |= ( 0xFF << 24)
#define PMU_GLB_PWR_WAIT2_SET_CPU3_SET_CLR(w)   w &= ~( 0xFF << 24)

#define PMU_GLB_PWR_WAIT2_SET_CPU2_SET_POS      16
#define PMU_GLB_PWR_WAIT2_SET_CPU2_SET_LEN      8
#define PMU_GLB_PWR_WAIT2_SET_CPU2_SET_MASK     0x00FF0000
#define PMU_GLB_PWR_WAIT2_SET_CPU2_SET_VAL(w)   (((w) & 0x00FF0000) >> 16)
#define PMU_GLB_PWR_WAIT2_SET_CPU2_SET_SET(w)   w |= ( 0xFF << 16)
#define PMU_GLB_PWR_WAIT2_SET_CPU2_SET_CLR(w)   w &= ~( 0xFF << 16)

#define PMU_GLB_PWR_WAIT2_SET_CPU1_SET_POS      8
#define PMU_GLB_PWR_WAIT2_SET_CPU1_SET_LEN      8
#define PMU_GLB_PWR_WAIT2_SET_CPU1_SET_MASK     0x0000FF00
#define PMU_GLB_PWR_WAIT2_SET_CPU1_SET_VAL(w)   (((w) & 0x0000FF00) >> 8)
#define PMU_GLB_PWR_WAIT2_SET_CPU1_SET_SET(w)   w |= ( 0xFF << 8)
#define PMU_GLB_PWR_WAIT2_SET_CPU1_SET_CLR(w)   w &= ~( 0xFF << 8)

#define PMU_GLB_PWR_WAIT2_SET_CPU0_SET_POS      0
#define PMU_GLB_PWR_WAIT2_SET_CPU0_SET_LEN      8
#define PMU_GLB_PWR_WAIT2_SET_CPU0_SET_MASK     0x000000FF
#define PMU_GLB_PWR_WAIT2_SET_CPU0_SET_VAL(w)   (((w) & 0x000000FF) >> 0)
#define PMU_GLB_PWR_WAIT2_SET_CPU0_SET_SET(w)   w |= ( 0xFF << 0)
#define PMU_GLB_PWR_WAIT2_SET_CPU0_SET_CLR(w)   w &= ~( 0xFF << 0)

// REGISTER : PWR_WAIT2_CLR

#define PMU_GLB_PWR_WAIT2_CLR_ADDR      PMU_GLB_BASE_ADDR + 0x140
#define PMU_GLB_PWR_WAIT2_CLR_RSTVAL    0x0
#define PMU_GLB_PWR_WAIT2_CLR_ALL1      0x0
#define PMU_GLB_PWR_WAIT2_CLR_ALL0      0x0
#define PMU_GLB_PWR_WAIT2_CLR_ALL5A     0x0
#define PMU_GLB_PWR_WAIT2_CLR_ALLA5     0x0

// FIELD IN REGISTER : PWR_WAIT2_CLR

#define PMU_GLB_PWR_WAIT2_CLR_CPU3_CLR_POS      24
#define PMU_GLB_PWR_WAIT2_CLR_CPU3_CLR_LEN      8
#define PMU_GLB_PWR_WAIT2_CLR_CPU3_CLR_MASK     0xFF000000
#define PMU_GLB_PWR_WAIT2_CLR_CPU3_CLR_VAL(w)   (((w) & 0xFF000000) >> 24)
#define PMU_GLB_PWR_WAIT2_CLR_CPU3_CLR_SET(w)   w |= ( 0xFF << 24)
#define PMU_GLB_PWR_WAIT2_CLR_CPU3_CLR_CLR(w)   w &= ~( 0xFF << 24)

#define PMU_GLB_PWR_WAIT2_CLR_CPU2_CLR_POS      16
#define PMU_GLB_PWR_WAIT2_CLR_CPU2_CLR_LEN      8
#define PMU_GLB_PWR_WAIT2_CLR_CPU2_CLR_MASK     0x00FF0000
#define PMU_GLB_PWR_WAIT2_CLR_CPU2_CLR_VAL(w)   (((w) & 0x00FF0000) >> 16)
#define PMU_GLB_PWR_WAIT2_CLR_CPU2_CLR_SET(w)   w |= ( 0xFF << 16)
#define PMU_GLB_PWR_WAIT2_CLR_CPU2_CLR_CLR(w)   w &= ~( 0xFF << 16)

#define PMU_GLB_PWR_WAIT2_CLR_CPU1_CLR_POS      8
#define PMU_GLB_PWR_WAIT2_CLR_CPU1_CLR_LEN      8
#define PMU_GLB_PWR_WAIT2_CLR_CPU1_CLR_MASK     0x0000FF00
#define PMU_GLB_PWR_WAIT2_CLR_CPU1_CLR_VAL(w)   (((w) & 0x0000FF00) >> 8)
#define PMU_GLB_PWR_WAIT2_CLR_CPU1_CLR_SET(w)   w |= ( 0xFF << 8)
#define PMU_GLB_PWR_WAIT2_CLR_CPU1_CLR_CLR(w)   w &= ~( 0xFF << 8)

#define PMU_GLB_PWR_WAIT2_CLR_CPU0_CLR_POS      0
#define PMU_GLB_PWR_WAIT2_CLR_CPU0_CLR_LEN      8
#define PMU_GLB_PWR_WAIT2_CLR_CPU0_CLR_MASK     0x000000FF
#define PMU_GLB_PWR_WAIT2_CLR_CPU0_CLR_VAL(w)   (((w) & 0x000000FF) >> 0)
#define PMU_GLB_PWR_WAIT2_CLR_CPU0_CLR_SET(w)   w |= ( 0xFF << 0)
#define PMU_GLB_PWR_WAIT2_CLR_CPU0_CLR_CLR(w)   w &= ~( 0xFF << 0)

// REGISTER : PWR_WAIT3_SET

#define PMU_GLB_PWR_WAIT3_SET_ADDR      PMU_GLB_BASE_ADDR + 0x144
#define PMU_GLB_PWR_WAIT3_SET_RSTVAL    0x0
#define PMU_GLB_PWR_WAIT3_SET_ALL1      0x0
#define PMU_GLB_PWR_WAIT3_SET_ALL0      0x0
#define PMU_GLB_PWR_WAIT3_SET_ALL5A     0x0
#define PMU_GLB_PWR_WAIT3_SET_ALLA5     0x0

// FIELD IN REGISTER : PWR_WAIT3_SET

#define PMU_GLB_PWR_WAIT3_SET_ISP_SET_POS      16
#define PMU_GLB_PWR_WAIT3_SET_ISP_SET_LEN      8
#define PMU_GLB_PWR_WAIT3_SET_ISP_SET_MASK     0x00FF0000
#define PMU_GLB_PWR_WAIT3_SET_ISP_SET_VAL(w)   (((w) & 0x00FF0000) >> 16)
#define PMU_GLB_PWR_WAIT3_SET_ISP_SET_SET(w)   w |= ( 0xFF << 16)
#define PMU_GLB_PWR_WAIT3_SET_ISP_SET_CLR(w)   w &= ~( 0xFF << 16)

#define PMU_GLB_PWR_WAIT3_SET_CPU_SET_POS      8
#define PMU_GLB_PWR_WAIT3_SET_CPU_SET_LEN      8
#define PMU_GLB_PWR_WAIT3_SET_CPU_SET_MASK     0x0000FF00
#define PMU_GLB_PWR_WAIT3_SET_CPU_SET_VAL(w)   (((w) & 0x0000FF00) >> 8)
#define PMU_GLB_PWR_WAIT3_SET_CPU_SET_SET(w)   w |= ( 0xFF << 8)
#define PMU_GLB_PWR_WAIT3_SET_CPU_SET_CLR(w)   w &= ~( 0xFF << 8)

#define PMU_GLB_PWR_WAIT3_SET_NPU_SET_POS      0
#define PMU_GLB_PWR_WAIT3_SET_NPU_SET_LEN      8
#define PMU_GLB_PWR_WAIT3_SET_NPU_SET_MASK     0x000000FF
#define PMU_GLB_PWR_WAIT3_SET_NPU_SET_VAL(w)   (((w) & 0x000000FF) >> 0)
#define PMU_GLB_PWR_WAIT3_SET_NPU_SET_SET(w)   w |= ( 0xFF << 0)
#define PMU_GLB_PWR_WAIT3_SET_NPU_SET_CLR(w)   w &= ~( 0xFF << 0)

// REGISTER : PWR_WAIT3_CLR

#define PMU_GLB_PWR_WAIT3_CLR_ADDR      PMU_GLB_BASE_ADDR + 0x148
#define PMU_GLB_PWR_WAIT3_CLR_RSTVAL    0x0
#define PMU_GLB_PWR_WAIT3_CLR_ALL1      0x0
#define PMU_GLB_PWR_WAIT3_CLR_ALL0      0x0
#define PMU_GLB_PWR_WAIT3_CLR_ALL5A     0x0
#define PMU_GLB_PWR_WAIT3_CLR_ALLA5     0x0

// FIELD IN REGISTER : PWR_WAIT3_CLR

#define PMU_GLB_PWR_WAIT3_CLR_ISP_CLR_POS      16
#define PMU_GLB_PWR_WAIT3_CLR_ISP_CLR_LEN      8
#define PMU_GLB_PWR_WAIT3_CLR_ISP_CLR_MASK     0x00FF0000
#define PMU_GLB_PWR_WAIT3_CLR_ISP_CLR_VAL(w)   (((w) & 0x00FF0000) >> 16)
#define PMU_GLB_PWR_WAIT3_CLR_ISP_CLR_SET(w)   w |= ( 0xFF << 16)
#define PMU_GLB_PWR_WAIT3_CLR_ISP_CLR_CLR(w)   w &= ~( 0xFF << 16)

#define PMU_GLB_PWR_WAIT3_CLR_CPU_CLR_POS      8
#define PMU_GLB_PWR_WAIT3_CLR_CPU_CLR_LEN      8
#define PMU_GLB_PWR_WAIT3_CLR_CPU_CLR_MASK     0x0000FF00
#define PMU_GLB_PWR_WAIT3_CLR_CPU_CLR_VAL(w)   (((w) & 0x0000FF00) >> 8)
#define PMU_GLB_PWR_WAIT3_CLR_CPU_CLR_SET(w)   w |= ( 0xFF << 8)
#define PMU_GLB_PWR_WAIT3_CLR_CPU_CLR_CLR(w)   w &= ~( 0xFF << 8)

#define PMU_GLB_PWR_WAIT3_CLR_NPU_CLR_POS      0
#define PMU_GLB_PWR_WAIT3_CLR_NPU_CLR_LEN      8
#define PMU_GLB_PWR_WAIT3_CLR_NPU_CLR_MASK     0x000000FF
#define PMU_GLB_PWR_WAIT3_CLR_NPU_CLR_VAL(w)   (((w) & 0x000000FF) >> 0)
#define PMU_GLB_PWR_WAIT3_CLR_NPU_CLR_SET(w)   w |= ( 0xFF << 0)
#define PMU_GLB_PWR_WAIT3_CLR_NPU_CLR_CLR(w)   w &= ~( 0xFF << 0)

// REGISTER : PWR_WAIT4_SET

#define PMU_GLB_PWR_WAIT4_SET_ADDR      PMU_GLB_BASE_ADDR + 0x14C
#define PMU_GLB_PWR_WAIT4_SET_RSTVAL    0x0
#define PMU_GLB_PWR_WAIT4_SET_ALL1      0x0
#define PMU_GLB_PWR_WAIT4_SET_ALL0      0x0
#define PMU_GLB_PWR_WAIT4_SET_ALL5A     0x0
#define PMU_GLB_PWR_WAIT4_SET_ALLA5     0x0

// FIELD IN REGISTER : PWR_WAIT4_SET

#define PMU_GLB_PWR_WAIT4_SET_NN_CORE_SET_POS      16
#define PMU_GLB_PWR_WAIT4_SET_NN_CORE_SET_LEN      8
#define PMU_GLB_PWR_WAIT4_SET_NN_CORE_SET_MASK     0x00FF0000
#define PMU_GLB_PWR_WAIT4_SET_NN_CORE_SET_VAL(w)   (((w) & 0x00FF0000) >> 16)
#define PMU_GLB_PWR_WAIT4_SET_NN_CORE_SET_SET(w)   w |= ( 0xFF << 16)
#define PMU_GLB_PWR_WAIT4_SET_NN_CORE_SET_CLR(w)   w &= ~( 0xFF << 16)

#define PMU_GLB_PWR_WAIT4_SET_PIPE_SET_POS      8
#define PMU_GLB_PWR_WAIT4_SET_PIPE_SET_LEN      8
#define PMU_GLB_PWR_WAIT4_SET_PIPE_SET_MASK     0x0000FF00
#define PMU_GLB_PWR_WAIT4_SET_PIPE_SET_VAL(w)   (((w) & 0x0000FF00) >> 8)
#define PMU_GLB_PWR_WAIT4_SET_PIPE_SET_SET(w)   w |= ( 0xFF << 8)
#define PMU_GLB_PWR_WAIT4_SET_PIPE_SET_CLR(w)   w &= ~( 0xFF << 8)

#define PMU_GLB_PWR_WAIT4_SET_MM_SET_POS      0
#define PMU_GLB_PWR_WAIT4_SET_MM_SET_LEN      8
#define PMU_GLB_PWR_WAIT4_SET_MM_SET_MASK     0x000000FF
#define PMU_GLB_PWR_WAIT4_SET_MM_SET_VAL(w)   (((w) & 0x000000FF) >> 0)
#define PMU_GLB_PWR_WAIT4_SET_MM_SET_SET(w)   w |= ( 0xFF << 0)
#define PMU_GLB_PWR_WAIT4_SET_MM_SET_CLR(w)   w &= ~( 0xFF << 0)

// REGISTER : PWR_WAIT4_CLR

#define PMU_GLB_PWR_WAIT4_CLR_ADDR      PMU_GLB_BASE_ADDR + 0x150
#define PMU_GLB_PWR_WAIT4_CLR_RSTVAL    0x0
#define PMU_GLB_PWR_WAIT4_CLR_ALL1      0x0
#define PMU_GLB_PWR_WAIT4_CLR_ALL0      0x0
#define PMU_GLB_PWR_WAIT4_CLR_ALL5A     0x0
#define PMU_GLB_PWR_WAIT4_CLR_ALLA5     0x0

// FIELD IN REGISTER : PWR_WAIT4_CLR

#define PMU_GLB_PWR_WAIT4_CLR_NN_CORE_CLR_POS      16
#define PMU_GLB_PWR_WAIT4_CLR_NN_CORE_CLR_LEN      8
#define PMU_GLB_PWR_WAIT4_CLR_NN_CORE_CLR_MASK     0x00FF0000
#define PMU_GLB_PWR_WAIT4_CLR_NN_CORE_CLR_VAL(w)   (((w) & 0x00FF0000) >> 16)
#define PMU_GLB_PWR_WAIT4_CLR_NN_CORE_CLR_SET(w)   w |= ( 0xFF << 16)
#define PMU_GLB_PWR_WAIT4_CLR_NN_CORE_CLR_CLR(w)   w &= ~( 0xFF << 16)

#define PMU_GLB_PWR_WAIT4_CLR_PIPE_CLR_POS      8
#define PMU_GLB_PWR_WAIT4_CLR_PIPE_CLR_LEN      8
#define PMU_GLB_PWR_WAIT4_CLR_PIPE_CLR_MASK     0x0000FF00
#define PMU_GLB_PWR_WAIT4_CLR_PIPE_CLR_VAL(w)   (((w) & 0x0000FF00) >> 8)
#define PMU_GLB_PWR_WAIT4_CLR_PIPE_CLR_SET(w)   w |= ( 0xFF << 8)
#define PMU_GLB_PWR_WAIT4_CLR_PIPE_CLR_CLR(w)   w &= ~( 0xFF << 8)

#define PMU_GLB_PWR_WAIT4_CLR_MM_CLR_POS      0
#define PMU_GLB_PWR_WAIT4_CLR_MM_CLR_LEN      8
#define PMU_GLB_PWR_WAIT4_CLR_MM_CLR_MASK     0x000000FF
#define PMU_GLB_PWR_WAIT4_CLR_MM_CLR_VAL(w)   (((w) & 0x000000FF) >> 0)
#define PMU_GLB_PWR_WAIT4_CLR_MM_CLR_SET(w)   w |= ( 0xFF << 0)
#define PMU_GLB_PWR_WAIT4_CLR_MM_CLR_CLR(w)   w &= ~( 0xFF << 0)

// REGISTER : PWR_WAITON0_SET

#define PMU_GLB_PWR_WAITON0_SET_ADDR      PMU_GLB_BASE_ADDR + 0x154
#define PMU_GLB_PWR_WAITON0_SET_RSTVAL    0x0
#define PMU_GLB_PWR_WAITON0_SET_ALL1      0x0
#define PMU_GLB_PWR_WAITON0_SET_ALL0      0x0
#define PMU_GLB_PWR_WAITON0_SET_ALL5A     0x0
#define PMU_GLB_PWR_WAITON0_SET_ALLA5     0x0

// FIELD IN REGISTER : PWR_WAITON0_SET

#define PMU_GLB_PWR_WAITON0_SET_DSP_SET_POS      24
#define PMU_GLB_PWR_WAITON0_SET_DSP_SET_LEN      8
#define PMU_GLB_PWR_WAITON0_SET_DSP_SET_MASK     0xFF000000
#define PMU_GLB_PWR_WAITON0_SET_DSP_SET_VAL(w)   (((w) & 0xFF000000) >> 24)
#define PMU_GLB_PWR_WAITON0_SET_DSP_SET_SET(w)   w |= ( 0xFF << 24)
#define PMU_GLB_PWR_WAITON0_SET_DSP_SET_CLR(w)   w &= ~( 0xFF << 24)

#define PMU_GLB_PWR_WAITON0_SET_VDEC_SET_POS      16
#define PMU_GLB_PWR_WAITON0_SET_VDEC_SET_LEN      8
#define PMU_GLB_PWR_WAITON0_SET_VDEC_SET_MASK     0x00FF0000
#define PMU_GLB_PWR_WAITON0_SET_VDEC_SET_VAL(w)   (((w) & 0x00FF0000) >> 16)
#define PMU_GLB_PWR_WAITON0_SET_VDEC_SET_SET(w)   w |= ( 0xFF << 16)
#define PMU_GLB_PWR_WAITON0_SET_VDEC_SET_CLR(w)   w &= ~( 0xFF << 16)

#define PMU_GLB_PWR_WAITON0_SET_VENC1_SET_POS      8
#define PMU_GLB_PWR_WAITON0_SET_VENC1_SET_LEN      8
#define PMU_GLB_PWR_WAITON0_SET_VENC1_SET_MASK     0x0000FF00
#define PMU_GLB_PWR_WAITON0_SET_VENC1_SET_VAL(w)   (((w) & 0x0000FF00) >> 8)
#define PMU_GLB_PWR_WAITON0_SET_VENC1_SET_SET(w)   w |= ( 0xFF << 8)
#define PMU_GLB_PWR_WAITON0_SET_VENC1_SET_CLR(w)   w &= ~( 0xFF << 8)

#define PMU_GLB_PWR_WAITON0_SET_VENC0_SET_POS      0
#define PMU_GLB_PWR_WAITON0_SET_VENC0_SET_LEN      8
#define PMU_GLB_PWR_WAITON0_SET_VENC0_SET_MASK     0x000000FF
#define PMU_GLB_PWR_WAITON0_SET_VENC0_SET_VAL(w)   (((w) & 0x000000FF) >> 0)
#define PMU_GLB_PWR_WAITON0_SET_VENC0_SET_SET(w)   w |= ( 0xFF << 0)
#define PMU_GLB_PWR_WAITON0_SET_VENC0_SET_CLR(w)   w &= ~( 0xFF << 0)

// REGISTER : PWR_WAITON0_CLR

#define PMU_GLB_PWR_WAITON0_CLR_ADDR      PMU_GLB_BASE_ADDR + 0x158
#define PMU_GLB_PWR_WAITON0_CLR_RSTVAL    0x0
#define PMU_GLB_PWR_WAITON0_CLR_ALL1      0x0
#define PMU_GLB_PWR_WAITON0_CLR_ALL0      0x0
#define PMU_GLB_PWR_WAITON0_CLR_ALL5A     0x0
#define PMU_GLB_PWR_WAITON0_CLR_ALLA5     0x0

// FIELD IN REGISTER : PWR_WAITON0_CLR

#define PMU_GLB_PWR_WAITON0_CLR_DSP_CLR_POS      24
#define PMU_GLB_PWR_WAITON0_CLR_DSP_CLR_LEN      8
#define PMU_GLB_PWR_WAITON0_CLR_DSP_CLR_MASK     0xFF000000
#define PMU_GLB_PWR_WAITON0_CLR_DSP_CLR_VAL(w)   (((w) & 0xFF000000) >> 24)
#define PMU_GLB_PWR_WAITON0_CLR_DSP_CLR_SET(w)   w |= ( 0xFF << 24)
#define PMU_GLB_PWR_WAITON0_CLR_DSP_CLR_CLR(w)   w &= ~( 0xFF << 24)

#define PMU_GLB_PWR_WAITON0_CLR_VDEC_CLR_POS      16
#define PMU_GLB_PWR_WAITON0_CLR_VDEC_CLR_LEN      8
#define PMU_GLB_PWR_WAITON0_CLR_VDEC_CLR_MASK     0x00FF0000
#define PMU_GLB_PWR_WAITON0_CLR_VDEC_CLR_VAL(w)   (((w) & 0x00FF0000) >> 16)
#define PMU_GLB_PWR_WAITON0_CLR_VDEC_CLR_SET(w)   w |= ( 0xFF << 16)
#define PMU_GLB_PWR_WAITON0_CLR_VDEC_CLR_CLR(w)   w &= ~( 0xFF << 16)

#define PMU_GLB_PWR_WAITON0_CLR_VENC1_CLR_POS      8
#define PMU_GLB_PWR_WAITON0_CLR_VENC1_CLR_LEN      8
#define PMU_GLB_PWR_WAITON0_CLR_VENC1_CLR_MASK     0x0000FF00
#define PMU_GLB_PWR_WAITON0_CLR_VENC1_CLR_VAL(w)   (((w) & 0x0000FF00) >> 8)
#define PMU_GLB_PWR_WAITON0_CLR_VENC1_CLR_SET(w)   w |= ( 0xFF << 8)
#define PMU_GLB_PWR_WAITON0_CLR_VENC1_CLR_CLR(w)   w &= ~( 0xFF << 8)

#define PMU_GLB_PWR_WAITON0_CLR_VENC0_CLR_POS      0
#define PMU_GLB_PWR_WAITON0_CLR_VENC0_CLR_LEN      8
#define PMU_GLB_PWR_WAITON0_CLR_VENC0_CLR_MASK     0x000000FF
#define PMU_GLB_PWR_WAITON0_CLR_VENC0_CLR_VAL(w)   (((w) & 0x000000FF) >> 0)
#define PMU_GLB_PWR_WAITON0_CLR_VENC0_CLR_SET(w)   w |= ( 0xFF << 0)
#define PMU_GLB_PWR_WAITON0_CLR_VENC0_CLR_CLR(w)   w &= ~( 0xFF << 0)

// REGISTER : PWR_WAITON1_SET

#define PMU_GLB_PWR_WAITON1_SET_ADDR      PMU_GLB_BASE_ADDR + 0x15C
#define PMU_GLB_PWR_WAITON1_SET_RSTVAL    0x0
#define PMU_GLB_PWR_WAITON1_SET_ALL1      0x0
#define PMU_GLB_PWR_WAITON1_SET_ALL0      0x0
#define PMU_GLB_PWR_WAITON1_SET_ALL5A     0x0
#define PMU_GLB_PWR_WAITON1_SET_ALLA5     0x0

// FIELD IN REGISTER : PWR_WAITON1_SET

#define PMU_GLB_PWR_WAITON1_SET_CPU7_SET_POS      24
#define PMU_GLB_PWR_WAITON1_SET_CPU7_SET_LEN      8
#define PMU_GLB_PWR_WAITON1_SET_CPU7_SET_MASK     0xFF000000
#define PMU_GLB_PWR_WAITON1_SET_CPU7_SET_VAL(w)   (((w) & 0xFF000000) >> 24)
#define PMU_GLB_PWR_WAITON1_SET_CPU7_SET_SET(w)   w |= ( 0xFF << 24)
#define PMU_GLB_PWR_WAITON1_SET_CPU7_SET_CLR(w)   w &= ~( 0xFF << 24)

#define PMU_GLB_PWR_WAITON1_SET_CPU6_SET_POS      16
#define PMU_GLB_PWR_WAITON1_SET_CPU6_SET_LEN      8
#define PMU_GLB_PWR_WAITON1_SET_CPU6_SET_MASK     0x00FF0000
#define PMU_GLB_PWR_WAITON1_SET_CPU6_SET_VAL(w)   (((w) & 0x00FF0000) >> 16)
#define PMU_GLB_PWR_WAITON1_SET_CPU6_SET_SET(w)   w |= ( 0xFF << 16)
#define PMU_GLB_PWR_WAITON1_SET_CPU6_SET_CLR(w)   w &= ~( 0xFF << 16)

#define PMU_GLB_PWR_WAITON1_SET_CPU5_SET_POS      8
#define PMU_GLB_PWR_WAITON1_SET_CPU5_SET_LEN      8
#define PMU_GLB_PWR_WAITON1_SET_CPU5_SET_MASK     0x0000FF00
#define PMU_GLB_PWR_WAITON1_SET_CPU5_SET_VAL(w)   (((w) & 0x0000FF00) >> 8)
#define PMU_GLB_PWR_WAITON1_SET_CPU5_SET_SET(w)   w |= ( 0xFF << 8)
#define PMU_GLB_PWR_WAITON1_SET_CPU5_SET_CLR(w)   w &= ~( 0xFF << 8)

#define PMU_GLB_PWR_WAITON1_SET_CPU4_SET_POS      0
#define PMU_GLB_PWR_WAITON1_SET_CPU4_SET_LEN      8
#define PMU_GLB_PWR_WAITON1_SET_CPU4_SET_MASK     0x000000FF
#define PMU_GLB_PWR_WAITON1_SET_CPU4_SET_VAL(w)   (((w) & 0x000000FF) >> 0)
#define PMU_GLB_PWR_WAITON1_SET_CPU4_SET_SET(w)   w |= ( 0xFF << 0)
#define PMU_GLB_PWR_WAITON1_SET_CPU4_SET_CLR(w)   w &= ~( 0xFF << 0)

// REGISTER : PWR_WAITON1_CLR

#define PMU_GLB_PWR_WAITON1_CLR_ADDR      PMU_GLB_BASE_ADDR + 0x160
#define PMU_GLB_PWR_WAITON1_CLR_RSTVAL    0x0
#define PMU_GLB_PWR_WAITON1_CLR_ALL1      0x0
#define PMU_GLB_PWR_WAITON1_CLR_ALL0      0x0
#define PMU_GLB_PWR_WAITON1_CLR_ALL5A     0x0
#define PMU_GLB_PWR_WAITON1_CLR_ALLA5     0x0

// FIELD IN REGISTER : PWR_WAITON1_CLR

#define PMU_GLB_PWR_WAITON1_CLR_CPU7_CLR_POS      24
#define PMU_GLB_PWR_WAITON1_CLR_CPU7_CLR_LEN      8
#define PMU_GLB_PWR_WAITON1_CLR_CPU7_CLR_MASK     0xFF000000
#define PMU_GLB_PWR_WAITON1_CLR_CPU7_CLR_VAL(w)   (((w) & 0xFF000000) >> 24)
#define PMU_GLB_PWR_WAITON1_CLR_CPU7_CLR_SET(w)   w |= ( 0xFF << 24)
#define PMU_GLB_PWR_WAITON1_CLR_CPU7_CLR_CLR(w)   w &= ~( 0xFF << 24)

#define PMU_GLB_PWR_WAITON1_CLR_CPU6_CLR_POS      16
#define PMU_GLB_PWR_WAITON1_CLR_CPU6_CLR_LEN      8
#define PMU_GLB_PWR_WAITON1_CLR_CPU6_CLR_MASK     0x00FF0000
#define PMU_GLB_PWR_WAITON1_CLR_CPU6_CLR_VAL(w)   (((w) & 0x00FF0000) >> 16)
#define PMU_GLB_PWR_WAITON1_CLR_CPU6_CLR_SET(w)   w |= ( 0xFF << 16)
#define PMU_GLB_PWR_WAITON1_CLR_CPU6_CLR_CLR(w)   w &= ~( 0xFF << 16)

#define PMU_GLB_PWR_WAITON1_CLR_CPU5_CLR_POS      8
#define PMU_GLB_PWR_WAITON1_CLR_CPU5_CLR_LEN      8
#define PMU_GLB_PWR_WAITON1_CLR_CPU5_CLR_MASK     0x0000FF00
#define PMU_GLB_PWR_WAITON1_CLR_CPU5_CLR_VAL(w)   (((w) & 0x0000FF00) >> 8)
#define PMU_GLB_PWR_WAITON1_CLR_CPU5_CLR_SET(w)   w |= ( 0xFF << 8)
#define PMU_GLB_PWR_WAITON1_CLR_CPU5_CLR_CLR(w)   w &= ~( 0xFF << 8)

#define PMU_GLB_PWR_WAITON1_CLR_CPU4_CLR_POS      0
#define PMU_GLB_PWR_WAITON1_CLR_CPU4_CLR_LEN      8
#define PMU_GLB_PWR_WAITON1_CLR_CPU4_CLR_MASK     0x000000FF
#define PMU_GLB_PWR_WAITON1_CLR_CPU4_CLR_VAL(w)   (((w) & 0x000000FF) >> 0)
#define PMU_GLB_PWR_WAITON1_CLR_CPU4_CLR_SET(w)   w |= ( 0xFF << 0)
#define PMU_GLB_PWR_WAITON1_CLR_CPU4_CLR_CLR(w)   w &= ~( 0xFF << 0)

// REGISTER : PWR_WAITON2_SET

#define PMU_GLB_PWR_WAITON2_SET_ADDR      PMU_GLB_BASE_ADDR + 0x164
#define PMU_GLB_PWR_WAITON2_SET_RSTVAL    0x0
#define PMU_GLB_PWR_WAITON2_SET_ALL1      0x0
#define PMU_GLB_PWR_WAITON2_SET_ALL0      0x0
#define PMU_GLB_PWR_WAITON2_SET_ALL5A     0x0
#define PMU_GLB_PWR_WAITON2_SET_ALLA5     0x0

// FIELD IN REGISTER : PWR_WAITON2_SET

#define PMU_GLB_PWR_WAITON2_SET_CPU3_SET_POS      24
#define PMU_GLB_PWR_WAITON2_SET_CPU3_SET_LEN      8
#define PMU_GLB_PWR_WAITON2_SET_CPU3_SET_MASK     0xFF000000
#define PMU_GLB_PWR_WAITON2_SET_CPU3_SET_VAL(w)   (((w) & 0xFF000000) >> 24)
#define PMU_GLB_PWR_WAITON2_SET_CPU3_SET_SET(w)   w |= ( 0xFF << 24)
#define PMU_GLB_PWR_WAITON2_SET_CPU3_SET_CLR(w)   w &= ~( 0xFF << 24)

#define PMU_GLB_PWR_WAITON2_SET_CPU2_SET_POS      16
#define PMU_GLB_PWR_WAITON2_SET_CPU2_SET_LEN      8
#define PMU_GLB_PWR_WAITON2_SET_CPU2_SET_MASK     0x00FF0000
#define PMU_GLB_PWR_WAITON2_SET_CPU2_SET_VAL(w)   (((w) & 0x00FF0000) >> 16)
#define PMU_GLB_PWR_WAITON2_SET_CPU2_SET_SET(w)   w |= ( 0xFF << 16)
#define PMU_GLB_PWR_WAITON2_SET_CPU2_SET_CLR(w)   w &= ~( 0xFF << 16)

#define PMU_GLB_PWR_WAITON2_SET_CPU1_SET_POS      8
#define PMU_GLB_PWR_WAITON2_SET_CPU1_SET_LEN      8
#define PMU_GLB_PWR_WAITON2_SET_CPU1_SET_MASK     0x0000FF00
#define PMU_GLB_PWR_WAITON2_SET_CPU1_SET_VAL(w)   (((w) & 0x0000FF00) >> 8)
#define PMU_GLB_PWR_WAITON2_SET_CPU1_SET_SET(w)   w |= ( 0xFF << 8)
#define PMU_GLB_PWR_WAITON2_SET_CPU1_SET_CLR(w)   w &= ~( 0xFF << 8)

#define PMU_GLB_PWR_WAITON2_SET_CPU0_SET_POS      0
#define PMU_GLB_PWR_WAITON2_SET_CPU0_SET_LEN      8
#define PMU_GLB_PWR_WAITON2_SET_CPU0_SET_MASK     0x000000FF
#define PMU_GLB_PWR_WAITON2_SET_CPU0_SET_VAL(w)   (((w) & 0x000000FF) >> 0)
#define PMU_GLB_PWR_WAITON2_SET_CPU0_SET_SET(w)   w |= ( 0xFF << 0)
#define PMU_GLB_PWR_WAITON2_SET_CPU0_SET_CLR(w)   w &= ~( 0xFF << 0)

// REGISTER : PWR_WAITON2_CLR

#define PMU_GLB_PWR_WAITON2_CLR_ADDR      PMU_GLB_BASE_ADDR + 0x168
#define PMU_GLB_PWR_WAITON2_CLR_RSTVAL    0x0
#define PMU_GLB_PWR_WAITON2_CLR_ALL1      0x0
#define PMU_GLB_PWR_WAITON2_CLR_ALL0      0x0
#define PMU_GLB_PWR_WAITON2_CLR_ALL5A     0x0
#define PMU_GLB_PWR_WAITON2_CLR_ALLA5     0x0

// FIELD IN REGISTER : PWR_WAITON2_CLR

#define PMU_GLB_PWR_WAITON2_CLR_CPU3_CLR_POS      24
#define PMU_GLB_PWR_WAITON2_CLR_CPU3_CLR_LEN      8
#define PMU_GLB_PWR_WAITON2_CLR_CPU3_CLR_MASK     0xFF000000
#define PMU_GLB_PWR_WAITON2_CLR_CPU3_CLR_VAL(w)   (((w) & 0xFF000000) >> 24)
#define PMU_GLB_PWR_WAITON2_CLR_CPU3_CLR_SET(w)   w |= ( 0xFF << 24)
#define PMU_GLB_PWR_WAITON2_CLR_CPU3_CLR_CLR(w)   w &= ~( 0xFF << 24)

#define PMU_GLB_PWR_WAITON2_CLR_CPU2_CLR_POS      16
#define PMU_GLB_PWR_WAITON2_CLR_CPU2_CLR_LEN      8
#define PMU_GLB_PWR_WAITON2_CLR_CPU2_CLR_MASK     0x00FF0000
#define PMU_GLB_PWR_WAITON2_CLR_CPU2_CLR_VAL(w)   (((w) & 0x00FF0000) >> 16)
#define PMU_GLB_PWR_WAITON2_CLR_CPU2_CLR_SET(w)   w |= ( 0xFF << 16)
#define PMU_GLB_PWR_WAITON2_CLR_CPU2_CLR_CLR(w)   w &= ~( 0xFF << 16)

#define PMU_GLB_PWR_WAITON2_CLR_CPU1_CLR_POS      8
#define PMU_GLB_PWR_WAITON2_CLR_CPU1_CLR_LEN      8
#define PMU_GLB_PWR_WAITON2_CLR_CPU1_CLR_MASK     0x0000FF00
#define PMU_GLB_PWR_WAITON2_CLR_CPU1_CLR_VAL(w)   (((w) & 0x0000FF00) >> 8)
#define PMU_GLB_PWR_WAITON2_CLR_CPU1_CLR_SET(w)   w |= ( 0xFF << 8)
#define PMU_GLB_PWR_WAITON2_CLR_CPU1_CLR_CLR(w)   w &= ~( 0xFF << 8)

#define PMU_GLB_PWR_WAITON2_CLR_CPU0_CLR_POS      0
#define PMU_GLB_PWR_WAITON2_CLR_CPU0_CLR_LEN      8
#define PMU_GLB_PWR_WAITON2_CLR_CPU0_CLR_MASK     0x000000FF
#define PMU_GLB_PWR_WAITON2_CLR_CPU0_CLR_VAL(w)   (((w) & 0x000000FF) >> 0)
#define PMU_GLB_PWR_WAITON2_CLR_CPU0_CLR_SET(w)   w |= ( 0xFF << 0)
#define PMU_GLB_PWR_WAITON2_CLR_CPU0_CLR_CLR(w)   w &= ~( 0xFF << 0)

// REGISTER : PWR_WAITON3_SET

#define PMU_GLB_PWR_WAITON3_SET_ADDR      PMU_GLB_BASE_ADDR + 0x16C
#define PMU_GLB_PWR_WAITON3_SET_RSTVAL    0x0
#define PMU_GLB_PWR_WAITON3_SET_ALL1      0x0
#define PMU_GLB_PWR_WAITON3_SET_ALL0      0x0
#define PMU_GLB_PWR_WAITON3_SET_ALL5A     0x0
#define PMU_GLB_PWR_WAITON3_SET_ALLA5     0x0

// FIELD IN REGISTER : PWR_WAITON3_SET

#define PMU_GLB_PWR_WAITON3_SET_ISP_SET_POS      16
#define PMU_GLB_PWR_WAITON3_SET_ISP_SET_LEN      8
#define PMU_GLB_PWR_WAITON3_SET_ISP_SET_MASK     0x00FF0000
#define PMU_GLB_PWR_WAITON3_SET_ISP_SET_VAL(w)   (((w) & 0x00FF0000) >> 16)
#define PMU_GLB_PWR_WAITON3_SET_ISP_SET_SET(w)   w |= ( 0xFF << 16)
#define PMU_GLB_PWR_WAITON3_SET_ISP_SET_CLR(w)   w &= ~( 0xFF << 16)

#define PMU_GLB_PWR_WAITON3_SET_CPU_SET_POS      8
#define PMU_GLB_PWR_WAITON3_SET_CPU_SET_LEN      8
#define PMU_GLB_PWR_WAITON3_SET_CPU_SET_MASK     0x0000FF00
#define PMU_GLB_PWR_WAITON3_SET_CPU_SET_VAL(w)   (((w) & 0x0000FF00) >> 8)
#define PMU_GLB_PWR_WAITON3_SET_CPU_SET_SET(w)   w |= ( 0xFF << 8)
#define PMU_GLB_PWR_WAITON3_SET_CPU_SET_CLR(w)   w &= ~( 0xFF << 8)

#define PMU_GLB_PWR_WAITON3_SET_NPU_SET_POS      0
#define PMU_GLB_PWR_WAITON3_SET_NPU_SET_LEN      8
#define PMU_GLB_PWR_WAITON3_SET_NPU_SET_MASK     0x000000FF
#define PMU_GLB_PWR_WAITON3_SET_NPU_SET_VAL(w)   (((w) & 0x000000FF) >> 0)
#define PMU_GLB_PWR_WAITON3_SET_NPU_SET_SET(w)   w |= ( 0xFF << 0)
#define PMU_GLB_PWR_WAITON3_SET_NPU_SET_CLR(w)   w &= ~( 0xFF << 0)

// REGISTER : PWR_WAITON3_CLR

#define PMU_GLB_PWR_WAITON3_CLR_ADDR      PMU_GLB_BASE_ADDR + 0x170
#define PMU_GLB_PWR_WAITON3_CLR_RSTVAL    0x0
#define PMU_GLB_PWR_WAITON3_CLR_ALL1      0x0
#define PMU_GLB_PWR_WAITON3_CLR_ALL0      0x0
#define PMU_GLB_PWR_WAITON3_CLR_ALL5A     0x0
#define PMU_GLB_PWR_WAITON3_CLR_ALLA5     0x0

// FIELD IN REGISTER : PWR_WAITON3_CLR

#define PMU_GLB_PWR_WAITON3_CLR_ISP_CLR_POS      16
#define PMU_GLB_PWR_WAITON3_CLR_ISP_CLR_LEN      8
#define PMU_GLB_PWR_WAITON3_CLR_ISP_CLR_MASK     0x00FF0000
#define PMU_GLB_PWR_WAITON3_CLR_ISP_CLR_VAL(w)   (((w) & 0x00FF0000) >> 16)
#define PMU_GLB_PWR_WAITON3_CLR_ISP_CLR_SET(w)   w |= ( 0xFF << 16)
#define PMU_GLB_PWR_WAITON3_CLR_ISP_CLR_CLR(w)   w &= ~( 0xFF << 16)

#define PMU_GLB_PWR_WAITON3_CLR_CPU_CLR_POS      8
#define PMU_GLB_PWR_WAITON3_CLR_CPU_CLR_LEN      8
#define PMU_GLB_PWR_WAITON3_CLR_CPU_CLR_MASK     0x0000FF00
#define PMU_GLB_PWR_WAITON3_CLR_CPU_CLR_VAL(w)   (((w) & 0x0000FF00) >> 8)
#define PMU_GLB_PWR_WAITON3_CLR_CPU_CLR_SET(w)   w |= ( 0xFF << 8)
#define PMU_GLB_PWR_WAITON3_CLR_CPU_CLR_CLR(w)   w &= ~( 0xFF << 8)

#define PMU_GLB_PWR_WAITON3_CLR_NPU_CLR_POS      0
#define PMU_GLB_PWR_WAITON3_CLR_NPU_CLR_LEN      8
#define PMU_GLB_PWR_WAITON3_CLR_NPU_CLR_MASK     0x000000FF
#define PMU_GLB_PWR_WAITON3_CLR_NPU_CLR_VAL(w)   (((w) & 0x000000FF) >> 0)
#define PMU_GLB_PWR_WAITON3_CLR_NPU_CLR_SET(w)   w |= ( 0xFF << 0)
#define PMU_GLB_PWR_WAITON3_CLR_NPU_CLR_CLR(w)   w &= ~( 0xFF << 0)

// REGISTER : PWR_WAITON4_SET

#define PMU_GLB_PWR_WAITON4_SET_ADDR      PMU_GLB_BASE_ADDR + 0x174
#define PMU_GLB_PWR_WAITON4_SET_RSTVAL    0x0
#define PMU_GLB_PWR_WAITON4_SET_ALL1      0x0
#define PMU_GLB_PWR_WAITON4_SET_ALL0      0x0
#define PMU_GLB_PWR_WAITON4_SET_ALL5A     0x0
#define PMU_GLB_PWR_WAITON4_SET_ALLA5     0x0

// FIELD IN REGISTER : PWR_WAITON4_SET

#define PMU_GLB_PWR_WAITON4_SET_NN_CORE_SET_POS      16
#define PMU_GLB_PWR_WAITON4_SET_NN_CORE_SET_LEN      8
#define PMU_GLB_PWR_WAITON4_SET_NN_CORE_SET_MASK     0x00FF0000
#define PMU_GLB_PWR_WAITON4_SET_NN_CORE_SET_VAL(w)   (((w) & 0x00FF0000) >> 16)
#define PMU_GLB_PWR_WAITON4_SET_NN_CORE_SET_SET(w)   w |= ( 0xFF << 16)
#define PMU_GLB_PWR_WAITON4_SET_NN_CORE_SET_CLR(w)   w &= ~( 0xFF << 16)

#define PMU_GLB_PWR_WAITON4_SET_PIPE_SET_POS      8
#define PMU_GLB_PWR_WAITON4_SET_PIPE_SET_LEN      8
#define PMU_GLB_PWR_WAITON4_SET_PIPE_SET_MASK     0x0000FF00
#define PMU_GLB_PWR_WAITON4_SET_PIPE_SET_VAL(w)   (((w) & 0x0000FF00) >> 8)
#define PMU_GLB_PWR_WAITON4_SET_PIPE_SET_SET(w)   w |= ( 0xFF << 8)
#define PMU_GLB_PWR_WAITON4_SET_PIPE_SET_CLR(w)   w &= ~( 0xFF << 8)

#define PMU_GLB_PWR_WAITON4_SET_MM_SET_POS      0
#define PMU_GLB_PWR_WAITON4_SET_MM_SET_LEN      8
#define PMU_GLB_PWR_WAITON4_SET_MM_SET_MASK     0x000000FF
#define PMU_GLB_PWR_WAITON4_SET_MM_SET_VAL(w)   (((w) & 0x000000FF) >> 0)
#define PMU_GLB_PWR_WAITON4_SET_MM_SET_SET(w)   w |= ( 0xFF << 0)
#define PMU_GLB_PWR_WAITON4_SET_MM_SET_CLR(w)   w &= ~( 0xFF << 0)

// REGISTER : PWR_WAITON4_CLR

#define PMU_GLB_PWR_WAITON4_CLR_ADDR      PMU_GLB_BASE_ADDR + 0x178
#define PMU_GLB_PWR_WAITON4_CLR_RSTVAL    0x0
#define PMU_GLB_PWR_WAITON4_CLR_ALL1      0x0
#define PMU_GLB_PWR_WAITON4_CLR_ALL0      0x0
#define PMU_GLB_PWR_WAITON4_CLR_ALL5A     0x0
#define PMU_GLB_PWR_WAITON4_CLR_ALLA5     0x0

// FIELD IN REGISTER : PWR_WAITON4_CLR

#define PMU_GLB_PWR_WAITON4_CLR_NN_CORE_CLR_POS      16
#define PMU_GLB_PWR_WAITON4_CLR_NN_CORE_CLR_LEN      8
#define PMU_GLB_PWR_WAITON4_CLR_NN_CORE_CLR_MASK     0x00FF0000
#define PMU_GLB_PWR_WAITON4_CLR_NN_CORE_CLR_VAL(w)   (((w) & 0x00FF0000) >> 16)
#define PMU_GLB_PWR_WAITON4_CLR_NN_CORE_CLR_SET(w)   w |= ( 0xFF << 16)
#define PMU_GLB_PWR_WAITON4_CLR_NN_CORE_CLR_CLR(w)   w &= ~( 0xFF << 16)

#define PMU_GLB_PWR_WAITON4_CLR_PIPE_CLR_POS      8
#define PMU_GLB_PWR_WAITON4_CLR_PIPE_CLR_LEN      8
#define PMU_GLB_PWR_WAITON4_CLR_PIPE_CLR_MASK     0x0000FF00
#define PMU_GLB_PWR_WAITON4_CLR_PIPE_CLR_VAL(w)   (((w) & 0x0000FF00) >> 8)
#define PMU_GLB_PWR_WAITON4_CLR_PIPE_CLR_SET(w)   w |= ( 0xFF << 8)
#define PMU_GLB_PWR_WAITON4_CLR_PIPE_CLR_CLR(w)   w &= ~( 0xFF << 8)

#define PMU_GLB_PWR_WAITON4_CLR_MM_CLR_POS      0
#define PMU_GLB_PWR_WAITON4_CLR_MM_CLR_LEN      8
#define PMU_GLB_PWR_WAITON4_CLR_MM_CLR_MASK     0x000000FF
#define PMU_GLB_PWR_WAITON4_CLR_MM_CLR_VAL(w)   (((w) & 0x000000FF) >> 0)
#define PMU_GLB_PWR_WAITON4_CLR_MM_CLR_SET(w)   w |= ( 0xFF << 0)
#define PMU_GLB_PWR_WAITON4_CLR_MM_CLR_CLR(w)   w &= ~( 0xFF << 0)

// REGISTER : SLP_FRC_EN_SET

#define PMU_GLB_SLP_FRC_EN_SET_ADDR      PMU_GLB_BASE_ADDR + 0x17C
#define PMU_GLB_SLP_FRC_EN_SET_RSTVAL    0x0
#define PMU_GLB_SLP_FRC_EN_SET_ALL1      0x0
#define PMU_GLB_SLP_FRC_EN_SET_ALL0      0x0
#define PMU_GLB_SLP_FRC_EN_SET_ALL5A     0x0
#define PMU_GLB_SLP_FRC_EN_SET_ALLA5     0x0

// FIELD IN REGISTER : SLP_FRC_EN_SET

#define PMU_GLB_SLP_FRC_EN_SET_CPU7_SET_POS      11
#define PMU_GLB_SLP_FRC_EN_SET_CPU7_SET_LEN      1
#define PMU_GLB_SLP_FRC_EN_SET_CPU7_SET_MASK     0x00000800
#define PMU_GLB_SLP_FRC_EN_SET_CPU7_SET_VAL(w)   (((w) & 0x00000800) >> 11)
#define PMU_GLB_SLP_FRC_EN_SET_CPU7_SET_SET(w)   w |= ( 0x1 << 11)
#define PMU_GLB_SLP_FRC_EN_SET_CPU7_SET_CLR(w)   w &= ~( 0x1 << 11)

#define PMU_GLB_SLP_FRC_EN_SET_CPU6_SET_POS      10
#define PMU_GLB_SLP_FRC_EN_SET_CPU6_SET_LEN      1
#define PMU_GLB_SLP_FRC_EN_SET_CPU6_SET_MASK     0x00000400
#define PMU_GLB_SLP_FRC_EN_SET_CPU6_SET_VAL(w)   (((w) & 0x00000400) >> 10)
#define PMU_GLB_SLP_FRC_EN_SET_CPU6_SET_SET(w)   w |= ( 0x1 << 10)
#define PMU_GLB_SLP_FRC_EN_SET_CPU6_SET_CLR(w)   w &= ~( 0x1 << 10)

#define PMU_GLB_SLP_FRC_EN_SET_CPU5_SET_POS      9
#define PMU_GLB_SLP_FRC_EN_SET_CPU5_SET_LEN      1
#define PMU_GLB_SLP_FRC_EN_SET_CPU5_SET_MASK     0x00000200
#define PMU_GLB_SLP_FRC_EN_SET_CPU5_SET_VAL(w)   (((w) & 0x00000200) >> 9)
#define PMU_GLB_SLP_FRC_EN_SET_CPU5_SET_SET(w)   w |= ( 0x1 << 9)
#define PMU_GLB_SLP_FRC_EN_SET_CPU5_SET_CLR(w)   w &= ~( 0x1 << 9)

#define PMU_GLB_SLP_FRC_EN_SET_CPU4_SET_POS      8
#define PMU_GLB_SLP_FRC_EN_SET_CPU4_SET_LEN      1
#define PMU_GLB_SLP_FRC_EN_SET_CPU4_SET_MASK     0x00000100
#define PMU_GLB_SLP_FRC_EN_SET_CPU4_SET_VAL(w)   (((w) & 0x00000100) >> 8)
#define PMU_GLB_SLP_FRC_EN_SET_CPU4_SET_SET(w)   w |= ( 0x1 << 8)
#define PMU_GLB_SLP_FRC_EN_SET_CPU4_SET_CLR(w)   w &= ~( 0x1 << 8)

#define PMU_GLB_SLP_FRC_EN_SET_CPU3_SET_POS      7
#define PMU_GLB_SLP_FRC_EN_SET_CPU3_SET_LEN      1
#define PMU_GLB_SLP_FRC_EN_SET_CPU3_SET_MASK     0x00000080
#define PMU_GLB_SLP_FRC_EN_SET_CPU3_SET_VAL(w)   (((w) & 0x00000080) >> 7)
#define PMU_GLB_SLP_FRC_EN_SET_CPU3_SET_SET(w)   w |= ( 0x1 << 7)
#define PMU_GLB_SLP_FRC_EN_SET_CPU3_SET_CLR(w)   w &= ~( 0x1 << 7)

#define PMU_GLB_SLP_FRC_EN_SET_CPU2_SET_POS      6
#define PMU_GLB_SLP_FRC_EN_SET_CPU2_SET_LEN      1
#define PMU_GLB_SLP_FRC_EN_SET_CPU2_SET_MASK     0x00000040
#define PMU_GLB_SLP_FRC_EN_SET_CPU2_SET_VAL(w)   (((w) & 0x00000040) >> 6)
#define PMU_GLB_SLP_FRC_EN_SET_CPU2_SET_SET(w)   w |= ( 0x1 << 6)
#define PMU_GLB_SLP_FRC_EN_SET_CPU2_SET_CLR(w)   w &= ~( 0x1 << 6)

#define PMU_GLB_SLP_FRC_EN_SET_CPU1_SET_POS      5
#define PMU_GLB_SLP_FRC_EN_SET_CPU1_SET_LEN      1
#define PMU_GLB_SLP_FRC_EN_SET_CPU1_SET_MASK     0x00000020
#define PMU_GLB_SLP_FRC_EN_SET_CPU1_SET_VAL(w)   (((w) & 0x00000020) >> 5)
#define PMU_GLB_SLP_FRC_EN_SET_CPU1_SET_SET(w)   w |= ( 0x1 << 5)
#define PMU_GLB_SLP_FRC_EN_SET_CPU1_SET_CLR(w)   w &= ~( 0x1 << 5)

#define PMU_GLB_SLP_FRC_EN_SET_CPU0_SET_POS      4
#define PMU_GLB_SLP_FRC_EN_SET_CPU0_SET_LEN      1
#define PMU_GLB_SLP_FRC_EN_SET_CPU0_SET_MASK     0x00000010
#define PMU_GLB_SLP_FRC_EN_SET_CPU0_SET_VAL(w)   (((w) & 0x00000010) >> 4)
#define PMU_GLB_SLP_FRC_EN_SET_CPU0_SET_SET(w)   w |= ( 0x1 << 4)
#define PMU_GLB_SLP_FRC_EN_SET_CPU0_SET_CLR(w)   w &= ~( 0x1 << 4)

#define PMU_GLB_SLP_FRC_EN_SET_DSP_SET_POS      3
#define PMU_GLB_SLP_FRC_EN_SET_DSP_SET_LEN      1
#define PMU_GLB_SLP_FRC_EN_SET_DSP_SET_MASK     0x00000008
#define PMU_GLB_SLP_FRC_EN_SET_DSP_SET_VAL(w)   (((w) & 0x00000008) >> 3)
#define PMU_GLB_SLP_FRC_EN_SET_DSP_SET_SET(w)   w |= ( 0x1 << 3)
#define PMU_GLB_SLP_FRC_EN_SET_DSP_SET_CLR(w)   w &= ~( 0x1 << 3)

#define PMU_GLB_SLP_FRC_EN_SET_VDEC_SET_POS      2
#define PMU_GLB_SLP_FRC_EN_SET_VDEC_SET_LEN      1
#define PMU_GLB_SLP_FRC_EN_SET_VDEC_SET_MASK     0x00000004
#define PMU_GLB_SLP_FRC_EN_SET_VDEC_SET_VAL(w)   (((w) & 0x00000004) >> 2)
#define PMU_GLB_SLP_FRC_EN_SET_VDEC_SET_SET(w)   w |= ( 0x1 << 2)
#define PMU_GLB_SLP_FRC_EN_SET_VDEC_SET_CLR(w)   w &= ~( 0x1 << 2)

#define PMU_GLB_SLP_FRC_EN_SET_VENC1_SET_POS      1
#define PMU_GLB_SLP_FRC_EN_SET_VENC1_SET_LEN      1
#define PMU_GLB_SLP_FRC_EN_SET_VENC1_SET_MASK     0x00000002
#define PMU_GLB_SLP_FRC_EN_SET_VENC1_SET_VAL(w)   (((w) & 0x00000002) >> 1)
#define PMU_GLB_SLP_FRC_EN_SET_VENC1_SET_SET(w)   w |= ( 0x1 << 1)
#define PMU_GLB_SLP_FRC_EN_SET_VENC1_SET_CLR(w)   w &= ~( 0x1 << 1)

#define PMU_GLB_SLP_FRC_EN_SET_VENC0_SET_POS      0
#define PMU_GLB_SLP_FRC_EN_SET_VENC0_SET_LEN      1
#define PMU_GLB_SLP_FRC_EN_SET_VENC0_SET_MASK     0x00000001
#define PMU_GLB_SLP_FRC_EN_SET_VENC0_SET_VAL(w)   (((w) & 0x00000001) >> 0)
#define PMU_GLB_SLP_FRC_EN_SET_VENC0_SET_SET(w)   w |= ( 0x1 << 0)
#define PMU_GLB_SLP_FRC_EN_SET_VENC0_SET_CLR(w)   w &= ~( 0x1 << 0)

// REGISTER : SLP_FRC_EN_CLR

#define PMU_GLB_SLP_FRC_EN_CLR_ADDR      PMU_GLB_BASE_ADDR + 0x180
#define PMU_GLB_SLP_FRC_EN_CLR_RSTVAL    0x0
#define PMU_GLB_SLP_FRC_EN_CLR_ALL1      0x0
#define PMU_GLB_SLP_FRC_EN_CLR_ALL0      0x0
#define PMU_GLB_SLP_FRC_EN_CLR_ALL5A     0x0
#define PMU_GLB_SLP_FRC_EN_CLR_ALLA5     0x0

// FIELD IN REGISTER : SLP_FRC_EN_CLR

#define PMU_GLB_SLP_FRC_EN_CLR_CPU7_CLR_POS      11
#define PMU_GLB_SLP_FRC_EN_CLR_CPU7_CLR_LEN      1
#define PMU_GLB_SLP_FRC_EN_CLR_CPU7_CLR_MASK     0x00000800
#define PMU_GLB_SLP_FRC_EN_CLR_CPU7_CLR_VAL(w)   (((w) & 0x00000800) >> 11)
#define PMU_GLB_SLP_FRC_EN_CLR_CPU7_CLR_SET(w)   w |= ( 0x1 << 11)
#define PMU_GLB_SLP_FRC_EN_CLR_CPU7_CLR_CLR(w)   w &= ~( 0x1 << 11)

#define PMU_GLB_SLP_FRC_EN_CLR_CPU6_CLR_POS      10
#define PMU_GLB_SLP_FRC_EN_CLR_CPU6_CLR_LEN      1
#define PMU_GLB_SLP_FRC_EN_CLR_CPU6_CLR_MASK     0x00000400
#define PMU_GLB_SLP_FRC_EN_CLR_CPU6_CLR_VAL(w)   (((w) & 0x00000400) >> 10)
#define PMU_GLB_SLP_FRC_EN_CLR_CPU6_CLR_SET(w)   w |= ( 0x1 << 10)
#define PMU_GLB_SLP_FRC_EN_CLR_CPU6_CLR_CLR(w)   w &= ~( 0x1 << 10)

#define PMU_GLB_SLP_FRC_EN_CLR_CPU5_CLR_POS      9
#define PMU_GLB_SLP_FRC_EN_CLR_CPU5_CLR_LEN      1
#define PMU_GLB_SLP_FRC_EN_CLR_CPU5_CLR_MASK     0x00000200
#define PMU_GLB_SLP_FRC_EN_CLR_CPU5_CLR_VAL(w)   (((w) & 0x00000200) >> 9)
#define PMU_GLB_SLP_FRC_EN_CLR_CPU5_CLR_SET(w)   w |= ( 0x1 << 9)
#define PMU_GLB_SLP_FRC_EN_CLR_CPU5_CLR_CLR(w)   w &= ~( 0x1 << 9)

#define PMU_GLB_SLP_FRC_EN_CLR_CPU4_CLR_POS      8
#define PMU_GLB_SLP_FRC_EN_CLR_CPU4_CLR_LEN      1
#define PMU_GLB_SLP_FRC_EN_CLR_CPU4_CLR_MASK     0x00000100
#define PMU_GLB_SLP_FRC_EN_CLR_CPU4_CLR_VAL(w)   (((w) & 0x00000100) >> 8)
#define PMU_GLB_SLP_FRC_EN_CLR_CPU4_CLR_SET(w)   w |= ( 0x1 << 8)
#define PMU_GLB_SLP_FRC_EN_CLR_CPU4_CLR_CLR(w)   w &= ~( 0x1 << 8)

#define PMU_GLB_SLP_FRC_EN_CLR_CPU3_CLR_POS      7
#define PMU_GLB_SLP_FRC_EN_CLR_CPU3_CLR_LEN      1
#define PMU_GLB_SLP_FRC_EN_CLR_CPU3_CLR_MASK     0x00000080
#define PMU_GLB_SLP_FRC_EN_CLR_CPU3_CLR_VAL(w)   (((w) & 0x00000080) >> 7)
#define PMU_GLB_SLP_FRC_EN_CLR_CPU3_CLR_SET(w)   w |= ( 0x1 << 7)
#define PMU_GLB_SLP_FRC_EN_CLR_CPU3_CLR_CLR(w)   w &= ~( 0x1 << 7)

#define PMU_GLB_SLP_FRC_EN_CLR_CPU2_CLR_POS      6
#define PMU_GLB_SLP_FRC_EN_CLR_CPU2_CLR_LEN      1
#define PMU_GLB_SLP_FRC_EN_CLR_CPU2_CLR_MASK     0x00000040
#define PMU_GLB_SLP_FRC_EN_CLR_CPU2_CLR_VAL(w)   (((w) & 0x00000040) >> 6)
#define PMU_GLB_SLP_FRC_EN_CLR_CPU2_CLR_SET(w)   w |= ( 0x1 << 6)
#define PMU_GLB_SLP_FRC_EN_CLR_CPU2_CLR_CLR(w)   w &= ~( 0x1 << 6)

#define PMU_GLB_SLP_FRC_EN_CLR_CPU1_CLR_POS      5
#define PMU_GLB_SLP_FRC_EN_CLR_CPU1_CLR_LEN      1
#define PMU_GLB_SLP_FRC_EN_CLR_CPU1_CLR_MASK     0x00000020
#define PMU_GLB_SLP_FRC_EN_CLR_CPU1_CLR_VAL(w)   (((w) & 0x00000020) >> 5)
#define PMU_GLB_SLP_FRC_EN_CLR_CPU1_CLR_SET(w)   w |= ( 0x1 << 5)
#define PMU_GLB_SLP_FRC_EN_CLR_CPU1_CLR_CLR(w)   w &= ~( 0x1 << 5)

#define PMU_GLB_SLP_FRC_EN_CLR_CPU0_CLR_POS      4
#define PMU_GLB_SLP_FRC_EN_CLR_CPU0_CLR_LEN      1
#define PMU_GLB_SLP_FRC_EN_CLR_CPU0_CLR_MASK     0x00000010
#define PMU_GLB_SLP_FRC_EN_CLR_CPU0_CLR_VAL(w)   (((w) & 0x00000010) >> 4)
#define PMU_GLB_SLP_FRC_EN_CLR_CPU0_CLR_SET(w)   w |= ( 0x1 << 4)
#define PMU_GLB_SLP_FRC_EN_CLR_CPU0_CLR_CLR(w)   w &= ~( 0x1 << 4)

#define PMU_GLB_SLP_FRC_EN_CLR_DSP_CLR_POS      3
#define PMU_GLB_SLP_FRC_EN_CLR_DSP_CLR_LEN      1
#define PMU_GLB_SLP_FRC_EN_CLR_DSP_CLR_MASK     0x00000008
#define PMU_GLB_SLP_FRC_EN_CLR_DSP_CLR_VAL(w)   (((w) & 0x00000008) >> 3)
#define PMU_GLB_SLP_FRC_EN_CLR_DSP_CLR_SET(w)   w |= ( 0x1 << 3)
#define PMU_GLB_SLP_FRC_EN_CLR_DSP_CLR_CLR(w)   w &= ~( 0x1 << 3)

#define PMU_GLB_SLP_FRC_EN_CLR_VDEC_CLR_POS      2
#define PMU_GLB_SLP_FRC_EN_CLR_VDEC_CLR_LEN      1
#define PMU_GLB_SLP_FRC_EN_CLR_VDEC_CLR_MASK     0x00000004
#define PMU_GLB_SLP_FRC_EN_CLR_VDEC_CLR_VAL(w)   (((w) & 0x00000004) >> 2)
#define PMU_GLB_SLP_FRC_EN_CLR_VDEC_CLR_SET(w)   w |= ( 0x1 << 2)
#define PMU_GLB_SLP_FRC_EN_CLR_VDEC_CLR_CLR(w)   w &= ~( 0x1 << 2)

#define PMU_GLB_SLP_FRC_EN_CLR_VENC1_CLR_POS      1
#define PMU_GLB_SLP_FRC_EN_CLR_VENC1_CLR_LEN      1
#define PMU_GLB_SLP_FRC_EN_CLR_VENC1_CLR_MASK     0x00000002
#define PMU_GLB_SLP_FRC_EN_CLR_VENC1_CLR_VAL(w)   (((w) & 0x00000002) >> 1)
#define PMU_GLB_SLP_FRC_EN_CLR_VENC1_CLR_SET(w)   w |= ( 0x1 << 1)
#define PMU_GLB_SLP_FRC_EN_CLR_VENC1_CLR_CLR(w)   w &= ~( 0x1 << 1)

#define PMU_GLB_SLP_FRC_EN_CLR_VENC0_CLR_POS      0
#define PMU_GLB_SLP_FRC_EN_CLR_VENC0_CLR_LEN      1
#define PMU_GLB_SLP_FRC_EN_CLR_VENC0_CLR_MASK     0x00000001
#define PMU_GLB_SLP_FRC_EN_CLR_VENC0_CLR_VAL(w)   (((w) & 0x00000001) >> 0)
#define PMU_GLB_SLP_FRC_EN_CLR_VENC0_CLR_SET(w)   w |= ( 0x1 << 0)
#define PMU_GLB_SLP_FRC_EN_CLR_VENC0_CLR_CLR(w)   w &= ~( 0x1 << 0)

// REGISTER : SLP_FRC_EN1_SET

#define PMU_GLB_SLP_FRC_EN1_SET_ADDR      PMU_GLB_BASE_ADDR + 0x184
#define PMU_GLB_SLP_FRC_EN1_SET_RSTVAL    0x0
#define PMU_GLB_SLP_FRC_EN1_SET_ALL1      0x0
#define PMU_GLB_SLP_FRC_EN1_SET_ALL0      0x0
#define PMU_GLB_SLP_FRC_EN1_SET_ALL5A     0x0
#define PMU_GLB_SLP_FRC_EN1_SET_ALLA5     0x0

// FIELD IN REGISTER : SLP_FRC_EN1_SET

#define PMU_GLB_SLP_FRC_EN1_SET_SEN_SET_POS      18
#define PMU_GLB_SLP_FRC_EN1_SET_SEN_SET_LEN      1
#define PMU_GLB_SLP_FRC_EN1_SET_SEN_SET_MASK     0x00040000
#define PMU_GLB_SLP_FRC_EN1_SET_SEN_SET_VAL(w)   (((w) & 0x00040000) >> 18)
#define PMU_GLB_SLP_FRC_EN1_SET_SEN_SET_SET(w)   w |= ( 0x1 << 18)
#define PMU_GLB_SLP_FRC_EN1_SET_SEN_SET_CLR(w)   w &= ~( 0x1 << 18)

#define PMU_GLB_SLP_FRC_EN1_SET_ISP_SET_POS      17
#define PMU_GLB_SLP_FRC_EN1_SET_ISP_SET_LEN      1
#define PMU_GLB_SLP_FRC_EN1_SET_ISP_SET_MASK     0x00020000
#define PMU_GLB_SLP_FRC_EN1_SET_ISP_SET_VAL(w)   (((w) & 0x00020000) >> 17)
#define PMU_GLB_SLP_FRC_EN1_SET_ISP_SET_SET(w)   w |= ( 0x1 << 17)
#define PMU_GLB_SLP_FRC_EN1_SET_ISP_SET_CLR(w)   w &= ~( 0x1 << 17)

#define PMU_GLB_SLP_FRC_EN1_SET_CPU_SET_POS      16
#define PMU_GLB_SLP_FRC_EN1_SET_CPU_SET_LEN      1
#define PMU_GLB_SLP_FRC_EN1_SET_CPU_SET_MASK     0x00010000
#define PMU_GLB_SLP_FRC_EN1_SET_CPU_SET_VAL(w)   (((w) & 0x00010000) >> 16)
#define PMU_GLB_SLP_FRC_EN1_SET_CPU_SET_SET(w)   w |= ( 0x1 << 16)
#define PMU_GLB_SLP_FRC_EN1_SET_CPU_SET_CLR(w)   w &= ~( 0x1 << 16)

#define PMU_GLB_SLP_FRC_EN1_SET_NPU_SET_POS      15
#define PMU_GLB_SLP_FRC_EN1_SET_NPU_SET_LEN      1
#define PMU_GLB_SLP_FRC_EN1_SET_NPU_SET_MASK     0x00008000
#define PMU_GLB_SLP_FRC_EN1_SET_NPU_SET_VAL(w)   (((w) & 0x00008000) >> 15)
#define PMU_GLB_SLP_FRC_EN1_SET_NPU_SET_SET(w)   w |= ( 0x1 << 15)
#define PMU_GLB_SLP_FRC_EN1_SET_NPU_SET_CLR(w)   w &= ~( 0x1 << 15)

#define PMU_GLB_SLP_FRC_EN1_SET_N8_SET_POS      14
#define PMU_GLB_SLP_FRC_EN1_SET_N8_SET_LEN      1
#define PMU_GLB_SLP_FRC_EN1_SET_N8_SET_MASK     0x00004000
#define PMU_GLB_SLP_FRC_EN1_SET_N8_SET_VAL(w)   (((w) & 0x00004000) >> 14)
#define PMU_GLB_SLP_FRC_EN1_SET_N8_SET_SET(w)   w |= ( 0x1 << 14)
#define PMU_GLB_SLP_FRC_EN1_SET_N8_SET_CLR(w)   w &= ~( 0x1 << 14)

#define PMU_GLB_SLP_FRC_EN1_SET_N7_SET_POS      13
#define PMU_GLB_SLP_FRC_EN1_SET_N7_SET_LEN      1
#define PMU_GLB_SLP_FRC_EN1_SET_N7_SET_MASK     0x00002000
#define PMU_GLB_SLP_FRC_EN1_SET_N7_SET_VAL(w)   (((w) & 0x00002000) >> 13)
#define PMU_GLB_SLP_FRC_EN1_SET_N7_SET_SET(w)   w |= ( 0x1 << 13)
#define PMU_GLB_SLP_FRC_EN1_SET_N7_SET_CLR(w)   w &= ~( 0x1 << 13)

#define PMU_GLB_SLP_FRC_EN1_SET_N6_SET_POS      12
#define PMU_GLB_SLP_FRC_EN1_SET_N6_SET_LEN      1
#define PMU_GLB_SLP_FRC_EN1_SET_N6_SET_MASK     0x00001000
#define PMU_GLB_SLP_FRC_EN1_SET_N6_SET_VAL(w)   (((w) & 0x00001000) >> 12)
#define PMU_GLB_SLP_FRC_EN1_SET_N6_SET_SET(w)   w |= ( 0x1 << 12)
#define PMU_GLB_SLP_FRC_EN1_SET_N6_SET_CLR(w)   w &= ~( 0x1 << 12)

#define PMU_GLB_SLP_FRC_EN1_SET_N5_SET_POS      11
#define PMU_GLB_SLP_FRC_EN1_SET_N5_SET_LEN      1
#define PMU_GLB_SLP_FRC_EN1_SET_N5_SET_MASK     0x00000800
#define PMU_GLB_SLP_FRC_EN1_SET_N5_SET_VAL(w)   (((w) & 0x00000800) >> 11)
#define PMU_GLB_SLP_FRC_EN1_SET_N5_SET_SET(w)   w |= ( 0x1 << 11)
#define PMU_GLB_SLP_FRC_EN1_SET_N5_SET_CLR(w)   w &= ~( 0x1 << 11)

#define PMU_GLB_SLP_FRC_EN1_SET_N4_SET_POS      10
#define PMU_GLB_SLP_FRC_EN1_SET_N4_SET_LEN      1
#define PMU_GLB_SLP_FRC_EN1_SET_N4_SET_MASK     0x00000400
#define PMU_GLB_SLP_FRC_EN1_SET_N4_SET_VAL(w)   (((w) & 0x00000400) >> 10)
#define PMU_GLB_SLP_FRC_EN1_SET_N4_SET_SET(w)   w |= ( 0x1 << 10)
#define PMU_GLB_SLP_FRC_EN1_SET_N4_SET_CLR(w)   w &= ~( 0x1 << 10)

#define PMU_GLB_SLP_FRC_EN1_SET_N3_SET_POS      9
#define PMU_GLB_SLP_FRC_EN1_SET_N3_SET_LEN      1
#define PMU_GLB_SLP_FRC_EN1_SET_N3_SET_MASK     0x00000200
#define PMU_GLB_SLP_FRC_EN1_SET_N3_SET_VAL(w)   (((w) & 0x00000200) >> 9)
#define PMU_GLB_SLP_FRC_EN1_SET_N3_SET_SET(w)   w |= ( 0x1 << 9)
#define PMU_GLB_SLP_FRC_EN1_SET_N3_SET_CLR(w)   w &= ~( 0x1 << 9)

#define PMU_GLB_SLP_FRC_EN1_SET_N2_SET_POS      8
#define PMU_GLB_SLP_FRC_EN1_SET_N2_SET_LEN      1
#define PMU_GLB_SLP_FRC_EN1_SET_N2_SET_MASK     0x00000100
#define PMU_GLB_SLP_FRC_EN1_SET_N2_SET_VAL(w)   (((w) & 0x00000100) >> 8)
#define PMU_GLB_SLP_FRC_EN1_SET_N2_SET_SET(w)   w |= ( 0x1 << 8)
#define PMU_GLB_SLP_FRC_EN1_SET_N2_SET_CLR(w)   w &= ~( 0x1 << 8)

#define PMU_GLB_SLP_FRC_EN1_SET_N1_SET_POS      7
#define PMU_GLB_SLP_FRC_EN1_SET_N1_SET_LEN      1
#define PMU_GLB_SLP_FRC_EN1_SET_N1_SET_MASK     0x00000080
#define PMU_GLB_SLP_FRC_EN1_SET_N1_SET_VAL(w)   (((w) & 0x00000080) >> 7)
#define PMU_GLB_SLP_FRC_EN1_SET_N1_SET_SET(w)   w |= ( 0x1 << 7)
#define PMU_GLB_SLP_FRC_EN1_SET_N1_SET_CLR(w)   w &= ~( 0x1 << 7)

#define PMU_GLB_SLP_FRC_EN1_SET_N0_SET_POS      6
#define PMU_GLB_SLP_FRC_EN1_SET_N0_SET_LEN      1
#define PMU_GLB_SLP_FRC_EN1_SET_N0_SET_MASK     0x00000040
#define PMU_GLB_SLP_FRC_EN1_SET_N0_SET_VAL(w)   (((w) & 0x00000040) >> 6)
#define PMU_GLB_SLP_FRC_EN1_SET_N0_SET_SET(w)   w |= ( 0x1 << 6)
#define PMU_GLB_SLP_FRC_EN1_SET_N0_SET_CLR(w)   w &= ~( 0x1 << 6)

#define PMU_GLB_SLP_FRC_EN1_SET_MM_SET_POS      5
#define PMU_GLB_SLP_FRC_EN1_SET_MM_SET_LEN      1
#define PMU_GLB_SLP_FRC_EN1_SET_MM_SET_MASK     0x00000020
#define PMU_GLB_SLP_FRC_EN1_SET_MM_SET_VAL(w)   (((w) & 0x00000020) >> 5)
#define PMU_GLB_SLP_FRC_EN1_SET_MM_SET_SET(w)   w |= ( 0x1 << 5)
#define PMU_GLB_SLP_FRC_EN1_SET_MM_SET_CLR(w)   w &= ~( 0x1 << 5)

#define PMU_GLB_SLP_FRC_EN1_SET_PERIP_SET_POS      4
#define PMU_GLB_SLP_FRC_EN1_SET_PERIP_SET_LEN      1
#define PMU_GLB_SLP_FRC_EN1_SET_PERIP_SET_MASK     0x00000010
#define PMU_GLB_SLP_FRC_EN1_SET_PERIP_SET_VAL(w)   (((w) & 0x00000010) >> 4)
#define PMU_GLB_SLP_FRC_EN1_SET_PERIP_SET_SET(w)   w |= ( 0x1 << 4)
#define PMU_GLB_SLP_FRC_EN1_SET_PERIP_SET_CLR(w)   w &= ~( 0x1 << 4)

#define PMU_GLB_SLP_FRC_EN1_SET_PIPE_SET_POS      3
#define PMU_GLB_SLP_FRC_EN1_SET_PIPE_SET_LEN      1
#define PMU_GLB_SLP_FRC_EN1_SET_PIPE_SET_MASK     0x00000008
#define PMU_GLB_SLP_FRC_EN1_SET_PIPE_SET_VAL(w)   (((w) & 0x00000008) >> 3)
#define PMU_GLB_SLP_FRC_EN1_SET_PIPE_SET_SET(w)   w |= ( 0x1 << 3)
#define PMU_GLB_SLP_FRC_EN1_SET_PIPE_SET_CLR(w)   w &= ~( 0x1 << 3)

#define PMU_GLB_SLP_FRC_EN1_SET_FLASH_SET_POS      2
#define PMU_GLB_SLP_FRC_EN1_SET_FLASH_SET_LEN      1
#define PMU_GLB_SLP_FRC_EN1_SET_FLASH_SET_MASK     0x00000004
#define PMU_GLB_SLP_FRC_EN1_SET_FLASH_SET_VAL(w)   (((w) & 0x00000004) >> 2)
#define PMU_GLB_SLP_FRC_EN1_SET_FLASH_SET_SET(w)   w |= ( 0x1 << 2)
#define PMU_GLB_SLP_FRC_EN1_SET_FLASH_SET_CLR(w)   w &= ~( 0x1 << 2)

#define PMU_GLB_SLP_FRC_EN1_SET_DDR1_SET_POS      1
#define PMU_GLB_SLP_FRC_EN1_SET_DDR1_SET_LEN      1
#define PMU_GLB_SLP_FRC_EN1_SET_DDR1_SET_MASK     0x00000002
#define PMU_GLB_SLP_FRC_EN1_SET_DDR1_SET_VAL(w)   (((w) & 0x00000002) >> 1)
#define PMU_GLB_SLP_FRC_EN1_SET_DDR1_SET_SET(w)   w |= ( 0x1 << 1)
#define PMU_GLB_SLP_FRC_EN1_SET_DDR1_SET_CLR(w)   w &= ~( 0x1 << 1)

#define PMU_GLB_SLP_FRC_EN1_SET_DDR0_SET_POS      0
#define PMU_GLB_SLP_FRC_EN1_SET_DDR0_SET_LEN      1
#define PMU_GLB_SLP_FRC_EN1_SET_DDR0_SET_MASK     0x00000001
#define PMU_GLB_SLP_FRC_EN1_SET_DDR0_SET_VAL(w)   (((w) & 0x00000001) >> 0)
#define PMU_GLB_SLP_FRC_EN1_SET_DDR0_SET_SET(w)   w |= ( 0x1 << 0)
#define PMU_GLB_SLP_FRC_EN1_SET_DDR0_SET_CLR(w)   w &= ~( 0x1 << 0)

// REGISTER : SLP_FRC_EN1_CLR

#define PMU_GLB_SLP_FRC_EN1_CLR_ADDR      PMU_GLB_BASE_ADDR + 0x188
#define PMU_GLB_SLP_FRC_EN1_CLR_RSTVAL    0x0
#define PMU_GLB_SLP_FRC_EN1_CLR_ALL1      0x0
#define PMU_GLB_SLP_FRC_EN1_CLR_ALL0      0x0
#define PMU_GLB_SLP_FRC_EN1_CLR_ALL5A     0x0
#define PMU_GLB_SLP_FRC_EN1_CLR_ALLA5     0x0

// FIELD IN REGISTER : SLP_FRC_EN1_CLR

#define PMU_GLB_SLP_FRC_EN1_CLR_SEN_CLR_POS      18
#define PMU_GLB_SLP_FRC_EN1_CLR_SEN_CLR_LEN      1
#define PMU_GLB_SLP_FRC_EN1_CLR_SEN_CLR_MASK     0x00040000
#define PMU_GLB_SLP_FRC_EN1_CLR_SEN_CLR_VAL(w)   (((w) & 0x00040000) >> 18)
#define PMU_GLB_SLP_FRC_EN1_CLR_SEN_CLR_SET(w)   w |= ( 0x1 << 18)
#define PMU_GLB_SLP_FRC_EN1_CLR_SEN_CLR_CLR(w)   w &= ~( 0x1 << 18)

#define PMU_GLB_SLP_FRC_EN1_CLR_ISP_CLR_POS      17
#define PMU_GLB_SLP_FRC_EN1_CLR_ISP_CLR_LEN      1
#define PMU_GLB_SLP_FRC_EN1_CLR_ISP_CLR_MASK     0x00020000
#define PMU_GLB_SLP_FRC_EN1_CLR_ISP_CLR_VAL(w)   (((w) & 0x00020000) >> 17)
#define PMU_GLB_SLP_FRC_EN1_CLR_ISP_CLR_SET(w)   w |= ( 0x1 << 17)
#define PMU_GLB_SLP_FRC_EN1_CLR_ISP_CLR_CLR(w)   w &= ~( 0x1 << 17)

#define PMU_GLB_SLP_FRC_EN1_CLR_CPU_CLR_POS      16
#define PMU_GLB_SLP_FRC_EN1_CLR_CPU_CLR_LEN      1
#define PMU_GLB_SLP_FRC_EN1_CLR_CPU_CLR_MASK     0x00010000
#define PMU_GLB_SLP_FRC_EN1_CLR_CPU_CLR_VAL(w)   (((w) & 0x00010000) >> 16)
#define PMU_GLB_SLP_FRC_EN1_CLR_CPU_CLR_SET(w)   w |= ( 0x1 << 16)
#define PMU_GLB_SLP_FRC_EN1_CLR_CPU_CLR_CLR(w)   w &= ~( 0x1 << 16)

#define PMU_GLB_SLP_FRC_EN1_CLR_NPU_CLR_POS      15
#define PMU_GLB_SLP_FRC_EN1_CLR_NPU_CLR_LEN      1
#define PMU_GLB_SLP_FRC_EN1_CLR_NPU_CLR_MASK     0x00008000
#define PMU_GLB_SLP_FRC_EN1_CLR_NPU_CLR_VAL(w)   (((w) & 0x00008000) >> 15)
#define PMU_GLB_SLP_FRC_EN1_CLR_NPU_CLR_SET(w)   w |= ( 0x1 << 15)
#define PMU_GLB_SLP_FRC_EN1_CLR_NPU_CLR_CLR(w)   w &= ~( 0x1 << 15)

#define PMU_GLB_SLP_FRC_EN1_CLR_N8_CLR_POS      14
#define PMU_GLB_SLP_FRC_EN1_CLR_N8_CLR_LEN      1
#define PMU_GLB_SLP_FRC_EN1_CLR_N8_CLR_MASK     0x00004000
#define PMU_GLB_SLP_FRC_EN1_CLR_N8_CLR_VAL(w)   (((w) & 0x00004000) >> 14)
#define PMU_GLB_SLP_FRC_EN1_CLR_N8_CLR_SET(w)   w |= ( 0x1 << 14)
#define PMU_GLB_SLP_FRC_EN1_CLR_N8_CLR_CLR(w)   w &= ~( 0x1 << 14)

#define PMU_GLB_SLP_FRC_EN1_CLR_N7_CLR_POS      13
#define PMU_GLB_SLP_FRC_EN1_CLR_N7_CLR_LEN      1
#define PMU_GLB_SLP_FRC_EN1_CLR_N7_CLR_MASK     0x00002000
#define PMU_GLB_SLP_FRC_EN1_CLR_N7_CLR_VAL(w)   (((w) & 0x00002000) >> 13)
#define PMU_GLB_SLP_FRC_EN1_CLR_N7_CLR_SET(w)   w |= ( 0x1 << 13)
#define PMU_GLB_SLP_FRC_EN1_CLR_N7_CLR_CLR(w)   w &= ~( 0x1 << 13)

#define PMU_GLB_SLP_FRC_EN1_CLR_N6_CLR_POS      12
#define PMU_GLB_SLP_FRC_EN1_CLR_N6_CLR_LEN      1
#define PMU_GLB_SLP_FRC_EN1_CLR_N6_CLR_MASK     0x00001000
#define PMU_GLB_SLP_FRC_EN1_CLR_N6_CLR_VAL(w)   (((w) & 0x00001000) >> 12)
#define PMU_GLB_SLP_FRC_EN1_CLR_N6_CLR_SET(w)   w |= ( 0x1 << 12)
#define PMU_GLB_SLP_FRC_EN1_CLR_N6_CLR_CLR(w)   w &= ~( 0x1 << 12)

#define PMU_GLB_SLP_FRC_EN1_CLR_N5_CLR_POS      11
#define PMU_GLB_SLP_FRC_EN1_CLR_N5_CLR_LEN      1
#define PMU_GLB_SLP_FRC_EN1_CLR_N5_CLR_MASK     0x00000800
#define PMU_GLB_SLP_FRC_EN1_CLR_N5_CLR_VAL(w)   (((w) & 0x00000800) >> 11)
#define PMU_GLB_SLP_FRC_EN1_CLR_N5_CLR_SET(w)   w |= ( 0x1 << 11)
#define PMU_GLB_SLP_FRC_EN1_CLR_N5_CLR_CLR(w)   w &= ~( 0x1 << 11)

#define PMU_GLB_SLP_FRC_EN1_CLR_N4_CLR_POS      10
#define PMU_GLB_SLP_FRC_EN1_CLR_N4_CLR_LEN      1
#define PMU_GLB_SLP_FRC_EN1_CLR_N4_CLR_MASK     0x00000400
#define PMU_GLB_SLP_FRC_EN1_CLR_N4_CLR_VAL(w)   (((w) & 0x00000400) >> 10)
#define PMU_GLB_SLP_FRC_EN1_CLR_N4_CLR_SET(w)   w |= ( 0x1 << 10)
#define PMU_GLB_SLP_FRC_EN1_CLR_N4_CLR_CLR(w)   w &= ~( 0x1 << 10)

#define PMU_GLB_SLP_FRC_EN1_CLR_N3_CLR_POS      9
#define PMU_GLB_SLP_FRC_EN1_CLR_N3_CLR_LEN      1
#define PMU_GLB_SLP_FRC_EN1_CLR_N3_CLR_MASK     0x00000200
#define PMU_GLB_SLP_FRC_EN1_CLR_N3_CLR_VAL(w)   (((w) & 0x00000200) >> 9)
#define PMU_GLB_SLP_FRC_EN1_CLR_N3_CLR_SET(w)   w |= ( 0x1 << 9)
#define PMU_GLB_SLP_FRC_EN1_CLR_N3_CLR_CLR(w)   w &= ~( 0x1 << 9)

#define PMU_GLB_SLP_FRC_EN1_CLR_N2_CLR_POS      8
#define PMU_GLB_SLP_FRC_EN1_CLR_N2_CLR_LEN      1
#define PMU_GLB_SLP_FRC_EN1_CLR_N2_CLR_MASK     0x00000100
#define PMU_GLB_SLP_FRC_EN1_CLR_N2_CLR_VAL(w)   (((w) & 0x00000100) >> 8)
#define PMU_GLB_SLP_FRC_EN1_CLR_N2_CLR_SET(w)   w |= ( 0x1 << 8)
#define PMU_GLB_SLP_FRC_EN1_CLR_N2_CLR_CLR(w)   w &= ~( 0x1 << 8)

#define PMU_GLB_SLP_FRC_EN1_CLR_N1_CLR_POS      7
#define PMU_GLB_SLP_FRC_EN1_CLR_N1_CLR_LEN      1
#define PMU_GLB_SLP_FRC_EN1_CLR_N1_CLR_MASK     0x00000080
#define PMU_GLB_SLP_FRC_EN1_CLR_N1_CLR_VAL(w)   (((w) & 0x00000080) >> 7)
#define PMU_GLB_SLP_FRC_EN1_CLR_N1_CLR_SET(w)   w |= ( 0x1 << 7)
#define PMU_GLB_SLP_FRC_EN1_CLR_N1_CLR_CLR(w)   w &= ~( 0x1 << 7)

#define PMU_GLB_SLP_FRC_EN1_CLR_N0_CLR_POS      6
#define PMU_GLB_SLP_FRC_EN1_CLR_N0_CLR_LEN      1
#define PMU_GLB_SLP_FRC_EN1_CLR_N0_CLR_MASK     0x00000040
#define PMU_GLB_SLP_FRC_EN1_CLR_N0_CLR_VAL(w)   (((w) & 0x00000040) >> 6)
#define PMU_GLB_SLP_FRC_EN1_CLR_N0_CLR_SET(w)   w |= ( 0x1 << 6)
#define PMU_GLB_SLP_FRC_EN1_CLR_N0_CLR_CLR(w)   w &= ~( 0x1 << 6)

#define PMU_GLB_SLP_FRC_EN1_CLR_MM_CLR_POS      5
#define PMU_GLB_SLP_FRC_EN1_CLR_MM_CLR_LEN      1
#define PMU_GLB_SLP_FRC_EN1_CLR_MM_CLR_MASK     0x00000020
#define PMU_GLB_SLP_FRC_EN1_CLR_MM_CLR_VAL(w)   (((w) & 0x00000020) >> 5)
#define PMU_GLB_SLP_FRC_EN1_CLR_MM_CLR_SET(w)   w |= ( 0x1 << 5)
#define PMU_GLB_SLP_FRC_EN1_CLR_MM_CLR_CLR(w)   w &= ~( 0x1 << 5)

#define PMU_GLB_SLP_FRC_EN1_CLR_PERIP_CLR_POS      4
#define PMU_GLB_SLP_FRC_EN1_CLR_PERIP_CLR_LEN      1
#define PMU_GLB_SLP_FRC_EN1_CLR_PERIP_CLR_MASK     0x00000010
#define PMU_GLB_SLP_FRC_EN1_CLR_PERIP_CLR_VAL(w)   (((w) & 0x00000010) >> 4)
#define PMU_GLB_SLP_FRC_EN1_CLR_PERIP_CLR_SET(w)   w |= ( 0x1 << 4)
#define PMU_GLB_SLP_FRC_EN1_CLR_PERIP_CLR_CLR(w)   w &= ~( 0x1 << 4)

#define PMU_GLB_SLP_FRC_EN1_CLR_PIPE_CLR_POS      3
#define PMU_GLB_SLP_FRC_EN1_CLR_PIPE_CLR_LEN      1
#define PMU_GLB_SLP_FRC_EN1_CLR_PIPE_CLR_MASK     0x00000008
#define PMU_GLB_SLP_FRC_EN1_CLR_PIPE_CLR_VAL(w)   (((w) & 0x00000008) >> 3)
#define PMU_GLB_SLP_FRC_EN1_CLR_PIPE_CLR_SET(w)   w |= ( 0x1 << 3)
#define PMU_GLB_SLP_FRC_EN1_CLR_PIPE_CLR_CLR(w)   w &= ~( 0x1 << 3)

#define PMU_GLB_SLP_FRC_EN1_CLR_FLASH_CLR_POS      2
#define PMU_GLB_SLP_FRC_EN1_CLR_FLASH_CLR_LEN      1
#define PMU_GLB_SLP_FRC_EN1_CLR_FLASH_CLR_MASK     0x00000004
#define PMU_GLB_SLP_FRC_EN1_CLR_FLASH_CLR_VAL(w)   (((w) & 0x00000004) >> 2)
#define PMU_GLB_SLP_FRC_EN1_CLR_FLASH_CLR_SET(w)   w |= ( 0x1 << 2)
#define PMU_GLB_SLP_FRC_EN1_CLR_FLASH_CLR_CLR(w)   w &= ~( 0x1 << 2)

#define PMU_GLB_SLP_FRC_EN1_CLR_DDR1_CLR_POS      1
#define PMU_GLB_SLP_FRC_EN1_CLR_DDR1_CLR_LEN      1
#define PMU_GLB_SLP_FRC_EN1_CLR_DDR1_CLR_MASK     0x00000002
#define PMU_GLB_SLP_FRC_EN1_CLR_DDR1_CLR_VAL(w)   (((w) & 0x00000002) >> 1)
#define PMU_GLB_SLP_FRC_EN1_CLR_DDR1_CLR_SET(w)   w |= ( 0x1 << 1)
#define PMU_GLB_SLP_FRC_EN1_CLR_DDR1_CLR_CLR(w)   w &= ~( 0x1 << 1)

#define PMU_GLB_SLP_FRC_EN1_CLR_DDR0_CLR_POS      0
#define PMU_GLB_SLP_FRC_EN1_CLR_DDR0_CLR_LEN      1
#define PMU_GLB_SLP_FRC_EN1_CLR_DDR0_CLR_MASK     0x00000001
#define PMU_GLB_SLP_FRC_EN1_CLR_DDR0_CLR_VAL(w)   (((w) & 0x00000001) >> 0)
#define PMU_GLB_SLP_FRC_EN1_CLR_DDR0_CLR_SET(w)   w |= ( 0x1 << 0)
#define PMU_GLB_SLP_FRC_EN1_CLR_DDR0_CLR_CLR(w)   w &= ~( 0x1 << 0)

// REGISTER : SLP_FRC_SW_SET

#define PMU_GLB_SLP_FRC_SW_SET_ADDR      PMU_GLB_BASE_ADDR + 0x18C
#define PMU_GLB_SLP_FRC_SW_SET_RSTVAL    0x0
#define PMU_GLB_SLP_FRC_SW_SET_ALL1      0x0
#define PMU_GLB_SLP_FRC_SW_SET_ALL0      0x0
#define PMU_GLB_SLP_FRC_SW_SET_ALL5A     0x0
#define PMU_GLB_SLP_FRC_SW_SET_ALLA5     0x0

// FIELD IN REGISTER : SLP_FRC_SW_SET

#define PMU_GLB_SLP_FRC_SW_SET_CPU7_SET_POS      11
#define PMU_GLB_SLP_FRC_SW_SET_CPU7_SET_LEN      1
#define PMU_GLB_SLP_FRC_SW_SET_CPU7_SET_MASK     0x00000800
#define PMU_GLB_SLP_FRC_SW_SET_CPU7_SET_VAL(w)   (((w) & 0x00000800) >> 11)
#define PMU_GLB_SLP_FRC_SW_SET_CPU7_SET_SET(w)   w |= ( 0x1 << 11)
#define PMU_GLB_SLP_FRC_SW_SET_CPU7_SET_CLR(w)   w &= ~( 0x1 << 11)

#define PMU_GLB_SLP_FRC_SW_SET_CPU6_SET_POS      10
#define PMU_GLB_SLP_FRC_SW_SET_CPU6_SET_LEN      1
#define PMU_GLB_SLP_FRC_SW_SET_CPU6_SET_MASK     0x00000400
#define PMU_GLB_SLP_FRC_SW_SET_CPU6_SET_VAL(w)   (((w) & 0x00000400) >> 10)
#define PMU_GLB_SLP_FRC_SW_SET_CPU6_SET_SET(w)   w |= ( 0x1 << 10)
#define PMU_GLB_SLP_FRC_SW_SET_CPU6_SET_CLR(w)   w &= ~( 0x1 << 10)

#define PMU_GLB_SLP_FRC_SW_SET_CPU5_SET_POS      9
#define PMU_GLB_SLP_FRC_SW_SET_CPU5_SET_LEN      1
#define PMU_GLB_SLP_FRC_SW_SET_CPU5_SET_MASK     0x00000200
#define PMU_GLB_SLP_FRC_SW_SET_CPU5_SET_VAL(w)   (((w) & 0x00000200) >> 9)
#define PMU_GLB_SLP_FRC_SW_SET_CPU5_SET_SET(w)   w |= ( 0x1 << 9)
#define PMU_GLB_SLP_FRC_SW_SET_CPU5_SET_CLR(w)   w &= ~( 0x1 << 9)

#define PMU_GLB_SLP_FRC_SW_SET_CPU4_SET_POS      8
#define PMU_GLB_SLP_FRC_SW_SET_CPU4_SET_LEN      1
#define PMU_GLB_SLP_FRC_SW_SET_CPU4_SET_MASK     0x00000100
#define PMU_GLB_SLP_FRC_SW_SET_CPU4_SET_VAL(w)   (((w) & 0x00000100) >> 8)
#define PMU_GLB_SLP_FRC_SW_SET_CPU4_SET_SET(w)   w |= ( 0x1 << 8)
#define PMU_GLB_SLP_FRC_SW_SET_CPU4_SET_CLR(w)   w &= ~( 0x1 << 8)

#define PMU_GLB_SLP_FRC_SW_SET_CPU3_SET_POS      7
#define PMU_GLB_SLP_FRC_SW_SET_CPU3_SET_LEN      1
#define PMU_GLB_SLP_FRC_SW_SET_CPU3_SET_MASK     0x00000080
#define PMU_GLB_SLP_FRC_SW_SET_CPU3_SET_VAL(w)   (((w) & 0x00000080) >> 7)
#define PMU_GLB_SLP_FRC_SW_SET_CPU3_SET_SET(w)   w |= ( 0x1 << 7)
#define PMU_GLB_SLP_FRC_SW_SET_CPU3_SET_CLR(w)   w &= ~( 0x1 << 7)

#define PMU_GLB_SLP_FRC_SW_SET_CPU2_SET_POS      6
#define PMU_GLB_SLP_FRC_SW_SET_CPU2_SET_LEN      1
#define PMU_GLB_SLP_FRC_SW_SET_CPU2_SET_MASK     0x00000040
#define PMU_GLB_SLP_FRC_SW_SET_CPU2_SET_VAL(w)   (((w) & 0x00000040) >> 6)
#define PMU_GLB_SLP_FRC_SW_SET_CPU2_SET_SET(w)   w |= ( 0x1 << 6)
#define PMU_GLB_SLP_FRC_SW_SET_CPU2_SET_CLR(w)   w &= ~( 0x1 << 6)

#define PMU_GLB_SLP_FRC_SW_SET_CPU1_SET_POS      5
#define PMU_GLB_SLP_FRC_SW_SET_CPU1_SET_LEN      1
#define PMU_GLB_SLP_FRC_SW_SET_CPU1_SET_MASK     0x00000020
#define PMU_GLB_SLP_FRC_SW_SET_CPU1_SET_VAL(w)   (((w) & 0x00000020) >> 5)
#define PMU_GLB_SLP_FRC_SW_SET_CPU1_SET_SET(w)   w |= ( 0x1 << 5)
#define PMU_GLB_SLP_FRC_SW_SET_CPU1_SET_CLR(w)   w &= ~( 0x1 << 5)

#define PMU_GLB_SLP_FRC_SW_SET_CPU0_SET_POS      4
#define PMU_GLB_SLP_FRC_SW_SET_CPU0_SET_LEN      1
#define PMU_GLB_SLP_FRC_SW_SET_CPU0_SET_MASK     0x00000010
#define PMU_GLB_SLP_FRC_SW_SET_CPU0_SET_VAL(w)   (((w) & 0x00000010) >> 4)
#define PMU_GLB_SLP_FRC_SW_SET_CPU0_SET_SET(w)   w |= ( 0x1 << 4)
#define PMU_GLB_SLP_FRC_SW_SET_CPU0_SET_CLR(w)   w &= ~( 0x1 << 4)

#define PMU_GLB_SLP_FRC_SW_SET_DSP_SET_POS      3
#define PMU_GLB_SLP_FRC_SW_SET_DSP_SET_LEN      1
#define PMU_GLB_SLP_FRC_SW_SET_DSP_SET_MASK     0x00000008
#define PMU_GLB_SLP_FRC_SW_SET_DSP_SET_VAL(w)   (((w) & 0x00000008) >> 3)
#define PMU_GLB_SLP_FRC_SW_SET_DSP_SET_SET(w)   w |= ( 0x1 << 3)
#define PMU_GLB_SLP_FRC_SW_SET_DSP_SET_CLR(w)   w &= ~( 0x1 << 3)

#define PMU_GLB_SLP_FRC_SW_SET_VDEC_SET_POS      2
#define PMU_GLB_SLP_FRC_SW_SET_VDEC_SET_LEN      1
#define PMU_GLB_SLP_FRC_SW_SET_VDEC_SET_MASK     0x00000004
#define PMU_GLB_SLP_FRC_SW_SET_VDEC_SET_VAL(w)   (((w) & 0x00000004) >> 2)
#define PMU_GLB_SLP_FRC_SW_SET_VDEC_SET_SET(w)   w |= ( 0x1 << 2)
#define PMU_GLB_SLP_FRC_SW_SET_VDEC_SET_CLR(w)   w &= ~( 0x1 << 2)

#define PMU_GLB_SLP_FRC_SW_SET_VENC1_SET_POS      1
#define PMU_GLB_SLP_FRC_SW_SET_VENC1_SET_LEN      1
#define PMU_GLB_SLP_FRC_SW_SET_VENC1_SET_MASK     0x00000002
#define PMU_GLB_SLP_FRC_SW_SET_VENC1_SET_VAL(w)   (((w) & 0x00000002) >> 1)
#define PMU_GLB_SLP_FRC_SW_SET_VENC1_SET_SET(w)   w |= ( 0x1 << 1)
#define PMU_GLB_SLP_FRC_SW_SET_VENC1_SET_CLR(w)   w &= ~( 0x1 << 1)

#define PMU_GLB_SLP_FRC_SW_SET_VENC0_SET_POS      0
#define PMU_GLB_SLP_FRC_SW_SET_VENC0_SET_LEN      1
#define PMU_GLB_SLP_FRC_SW_SET_VENC0_SET_MASK     0x00000001
#define PMU_GLB_SLP_FRC_SW_SET_VENC0_SET_VAL(w)   (((w) & 0x00000001) >> 0)
#define PMU_GLB_SLP_FRC_SW_SET_VENC0_SET_SET(w)   w |= ( 0x1 << 0)
#define PMU_GLB_SLP_FRC_SW_SET_VENC0_SET_CLR(w)   w &= ~( 0x1 << 0)

// REGISTER : SLP_FRC_SW_CLR

#define PMU_GLB_SLP_FRC_SW_CLR_ADDR      PMU_GLB_BASE_ADDR + 0x190
#define PMU_GLB_SLP_FRC_SW_CLR_RSTVAL    0x0
#define PMU_GLB_SLP_FRC_SW_CLR_ALL1      0x0
#define PMU_GLB_SLP_FRC_SW_CLR_ALL0      0x0
#define PMU_GLB_SLP_FRC_SW_CLR_ALL5A     0x0
#define PMU_GLB_SLP_FRC_SW_CLR_ALLA5     0x0

// FIELD IN REGISTER : SLP_FRC_SW_CLR

#define PMU_GLB_SLP_FRC_SW_CLR_CPU7_CLR_POS      11
#define PMU_GLB_SLP_FRC_SW_CLR_CPU7_CLR_LEN      1
#define PMU_GLB_SLP_FRC_SW_CLR_CPU7_CLR_MASK     0x00000800
#define PMU_GLB_SLP_FRC_SW_CLR_CPU7_CLR_VAL(w)   (((w) & 0x00000800) >> 11)
#define PMU_GLB_SLP_FRC_SW_CLR_CPU7_CLR_SET(w)   w |= ( 0x1 << 11)
#define PMU_GLB_SLP_FRC_SW_CLR_CPU7_CLR_CLR(w)   w &= ~( 0x1 << 11)

#define PMU_GLB_SLP_FRC_SW_CLR_CPU6_CLR_POS      10
#define PMU_GLB_SLP_FRC_SW_CLR_CPU6_CLR_LEN      1
#define PMU_GLB_SLP_FRC_SW_CLR_CPU6_CLR_MASK     0x00000400
#define PMU_GLB_SLP_FRC_SW_CLR_CPU6_CLR_VAL(w)   (((w) & 0x00000400) >> 10)
#define PMU_GLB_SLP_FRC_SW_CLR_CPU6_CLR_SET(w)   w |= ( 0x1 << 10)
#define PMU_GLB_SLP_FRC_SW_CLR_CPU6_CLR_CLR(w)   w &= ~( 0x1 << 10)

#define PMU_GLB_SLP_FRC_SW_CLR_CPU5_CLR_POS      9
#define PMU_GLB_SLP_FRC_SW_CLR_CPU5_CLR_LEN      1
#define PMU_GLB_SLP_FRC_SW_CLR_CPU5_CLR_MASK     0x00000200
#define PMU_GLB_SLP_FRC_SW_CLR_CPU5_CLR_VAL(w)   (((w) & 0x00000200) >> 9)
#define PMU_GLB_SLP_FRC_SW_CLR_CPU5_CLR_SET(w)   w |= ( 0x1 << 9)
#define PMU_GLB_SLP_FRC_SW_CLR_CPU5_CLR_CLR(w)   w &= ~( 0x1 << 9)

#define PMU_GLB_SLP_FRC_SW_CLR_CPU4_CLR_POS      8
#define PMU_GLB_SLP_FRC_SW_CLR_CPU4_CLR_LEN      1
#define PMU_GLB_SLP_FRC_SW_CLR_CPU4_CLR_MASK     0x00000100
#define PMU_GLB_SLP_FRC_SW_CLR_CPU4_CLR_VAL(w)   (((w) & 0x00000100) >> 8)
#define PMU_GLB_SLP_FRC_SW_CLR_CPU4_CLR_SET(w)   w |= ( 0x1 << 8)
#define PMU_GLB_SLP_FRC_SW_CLR_CPU4_CLR_CLR(w)   w &= ~( 0x1 << 8)

#define PMU_GLB_SLP_FRC_SW_CLR_CPU3_CLR_POS      7
#define PMU_GLB_SLP_FRC_SW_CLR_CPU3_CLR_LEN      1
#define PMU_GLB_SLP_FRC_SW_CLR_CPU3_CLR_MASK     0x00000080
#define PMU_GLB_SLP_FRC_SW_CLR_CPU3_CLR_VAL(w)   (((w) & 0x00000080) >> 7)
#define PMU_GLB_SLP_FRC_SW_CLR_CPU3_CLR_SET(w)   w |= ( 0x1 << 7)
#define PMU_GLB_SLP_FRC_SW_CLR_CPU3_CLR_CLR(w)   w &= ~( 0x1 << 7)

#define PMU_GLB_SLP_FRC_SW_CLR_CPU2_CLR_POS      6
#define PMU_GLB_SLP_FRC_SW_CLR_CPU2_CLR_LEN      1
#define PMU_GLB_SLP_FRC_SW_CLR_CPU2_CLR_MASK     0x00000040
#define PMU_GLB_SLP_FRC_SW_CLR_CPU2_CLR_VAL(w)   (((w) & 0x00000040) >> 6)
#define PMU_GLB_SLP_FRC_SW_CLR_CPU2_CLR_SET(w)   w |= ( 0x1 << 6)
#define PMU_GLB_SLP_FRC_SW_CLR_CPU2_CLR_CLR(w)   w &= ~( 0x1 << 6)

#define PMU_GLB_SLP_FRC_SW_CLR_CPU1_CLR_POS      5
#define PMU_GLB_SLP_FRC_SW_CLR_CPU1_CLR_LEN      1
#define PMU_GLB_SLP_FRC_SW_CLR_CPU1_CLR_MASK     0x00000020
#define PMU_GLB_SLP_FRC_SW_CLR_CPU1_CLR_VAL(w)   (((w) & 0x00000020) >> 5)
#define PMU_GLB_SLP_FRC_SW_CLR_CPU1_CLR_SET(w)   w |= ( 0x1 << 5)
#define PMU_GLB_SLP_FRC_SW_CLR_CPU1_CLR_CLR(w)   w &= ~( 0x1 << 5)

#define PMU_GLB_SLP_FRC_SW_CLR_CPU0_CLR_POS      4
#define PMU_GLB_SLP_FRC_SW_CLR_CPU0_CLR_LEN      1
#define PMU_GLB_SLP_FRC_SW_CLR_CPU0_CLR_MASK     0x00000010
#define PMU_GLB_SLP_FRC_SW_CLR_CPU0_CLR_VAL(w)   (((w) & 0x00000010) >> 4)
#define PMU_GLB_SLP_FRC_SW_CLR_CPU0_CLR_SET(w)   w |= ( 0x1 << 4)
#define PMU_GLB_SLP_FRC_SW_CLR_CPU0_CLR_CLR(w)   w &= ~( 0x1 << 4)

#define PMU_GLB_SLP_FRC_SW_CLR_DSP_CLR_POS      3
#define PMU_GLB_SLP_FRC_SW_CLR_DSP_CLR_LEN      1
#define PMU_GLB_SLP_FRC_SW_CLR_DSP_CLR_MASK     0x00000008
#define PMU_GLB_SLP_FRC_SW_CLR_DSP_CLR_VAL(w)   (((w) & 0x00000008) >> 3)
#define PMU_GLB_SLP_FRC_SW_CLR_DSP_CLR_SET(w)   w |= ( 0x1 << 3)
#define PMU_GLB_SLP_FRC_SW_CLR_DSP_CLR_CLR(w)   w &= ~( 0x1 << 3)

#define PMU_GLB_SLP_FRC_SW_CLR_VDEC_CLR_POS      2
#define PMU_GLB_SLP_FRC_SW_CLR_VDEC_CLR_LEN      1
#define PMU_GLB_SLP_FRC_SW_CLR_VDEC_CLR_MASK     0x00000004
#define PMU_GLB_SLP_FRC_SW_CLR_VDEC_CLR_VAL(w)   (((w) & 0x00000004) >> 2)
#define PMU_GLB_SLP_FRC_SW_CLR_VDEC_CLR_SET(w)   w |= ( 0x1 << 2)
#define PMU_GLB_SLP_FRC_SW_CLR_VDEC_CLR_CLR(w)   w &= ~( 0x1 << 2)

#define PMU_GLB_SLP_FRC_SW_CLR_VENC1_CLR_POS      1
#define PMU_GLB_SLP_FRC_SW_CLR_VENC1_CLR_LEN      1
#define PMU_GLB_SLP_FRC_SW_CLR_VENC1_CLR_MASK     0x00000002
#define PMU_GLB_SLP_FRC_SW_CLR_VENC1_CLR_VAL(w)   (((w) & 0x00000002) >> 1)
#define PMU_GLB_SLP_FRC_SW_CLR_VENC1_CLR_SET(w)   w |= ( 0x1 << 1)
#define PMU_GLB_SLP_FRC_SW_CLR_VENC1_CLR_CLR(w)   w &= ~( 0x1 << 1)

#define PMU_GLB_SLP_FRC_SW_CLR_VENC0_CLR_POS      0
#define PMU_GLB_SLP_FRC_SW_CLR_VENC0_CLR_LEN      1
#define PMU_GLB_SLP_FRC_SW_CLR_VENC0_CLR_MASK     0x00000001
#define PMU_GLB_SLP_FRC_SW_CLR_VENC0_CLR_VAL(w)   (((w) & 0x00000001) >> 0)
#define PMU_GLB_SLP_FRC_SW_CLR_VENC0_CLR_SET(w)   w |= ( 0x1 << 0)
#define PMU_GLB_SLP_FRC_SW_CLR_VENC0_CLR_CLR(w)   w &= ~( 0x1 << 0)

// REGISTER : SLP_FRC_SW1_SET

#define PMU_GLB_SLP_FRC_SW1_SET_ADDR      PMU_GLB_BASE_ADDR + 0x194
#define PMU_GLB_SLP_FRC_SW1_SET_RSTVAL    0x0
#define PMU_GLB_SLP_FRC_SW1_SET_ALL1      0x0
#define PMU_GLB_SLP_FRC_SW1_SET_ALL0      0x0
#define PMU_GLB_SLP_FRC_SW1_SET_ALL5A     0x0
#define PMU_GLB_SLP_FRC_SW1_SET_ALLA5     0x0

// FIELD IN REGISTER : SLP_FRC_SW1_SET

#define PMU_GLB_SLP_FRC_SW1_SET_SEN_SET_POS      18
#define PMU_GLB_SLP_FRC_SW1_SET_SEN_SET_LEN      1
#define PMU_GLB_SLP_FRC_SW1_SET_SEN_SET_MASK     0x00040000
#define PMU_GLB_SLP_FRC_SW1_SET_SEN_SET_VAL(w)   (((w) & 0x00040000) >> 18)
#define PMU_GLB_SLP_FRC_SW1_SET_SEN_SET_SET(w)   w |= ( 0x1 << 18)
#define PMU_GLB_SLP_FRC_SW1_SET_SEN_SET_CLR(w)   w &= ~( 0x1 << 18)

#define PMU_GLB_SLP_FRC_SW1_SET_ISP_SET_POS      17
#define PMU_GLB_SLP_FRC_SW1_SET_ISP_SET_LEN      1
#define PMU_GLB_SLP_FRC_SW1_SET_ISP_SET_MASK     0x00020000
#define PMU_GLB_SLP_FRC_SW1_SET_ISP_SET_VAL(w)   (((w) & 0x00020000) >> 17)
#define PMU_GLB_SLP_FRC_SW1_SET_ISP_SET_SET(w)   w |= ( 0x1 << 17)
#define PMU_GLB_SLP_FRC_SW1_SET_ISP_SET_CLR(w)   w &= ~( 0x1 << 17)

#define PMU_GLB_SLP_FRC_SW1_SET_CPU_SET_POS      16
#define PMU_GLB_SLP_FRC_SW1_SET_CPU_SET_LEN      1
#define PMU_GLB_SLP_FRC_SW1_SET_CPU_SET_MASK     0x00010000
#define PMU_GLB_SLP_FRC_SW1_SET_CPU_SET_VAL(w)   (((w) & 0x00010000) >> 16)
#define PMU_GLB_SLP_FRC_SW1_SET_CPU_SET_SET(w)   w |= ( 0x1 << 16)
#define PMU_GLB_SLP_FRC_SW1_SET_CPU_SET_CLR(w)   w &= ~( 0x1 << 16)

#define PMU_GLB_SLP_FRC_SW1_SET_NPU_SET_POS      15
#define PMU_GLB_SLP_FRC_SW1_SET_NPU_SET_LEN      1
#define PMU_GLB_SLP_FRC_SW1_SET_NPU_SET_MASK     0x00008000
#define PMU_GLB_SLP_FRC_SW1_SET_NPU_SET_VAL(w)   (((w) & 0x00008000) >> 15)
#define PMU_GLB_SLP_FRC_SW1_SET_NPU_SET_SET(w)   w |= ( 0x1 << 15)
#define PMU_GLB_SLP_FRC_SW1_SET_NPU_SET_CLR(w)   w &= ~( 0x1 << 15)

#define PMU_GLB_SLP_FRC_SW1_SET_N8_SET_POS      14
#define PMU_GLB_SLP_FRC_SW1_SET_N8_SET_LEN      1
#define PMU_GLB_SLP_FRC_SW1_SET_N8_SET_MASK     0x00004000
#define PMU_GLB_SLP_FRC_SW1_SET_N8_SET_VAL(w)   (((w) & 0x00004000) >> 14)
#define PMU_GLB_SLP_FRC_SW1_SET_N8_SET_SET(w)   w |= ( 0x1 << 14)
#define PMU_GLB_SLP_FRC_SW1_SET_N8_SET_CLR(w)   w &= ~( 0x1 << 14)

#define PMU_GLB_SLP_FRC_SW1_SET_N7_SET_POS      13
#define PMU_GLB_SLP_FRC_SW1_SET_N7_SET_LEN      1
#define PMU_GLB_SLP_FRC_SW1_SET_N7_SET_MASK     0x00002000
#define PMU_GLB_SLP_FRC_SW1_SET_N7_SET_VAL(w)   (((w) & 0x00002000) >> 13)
#define PMU_GLB_SLP_FRC_SW1_SET_N7_SET_SET(w)   w |= ( 0x1 << 13)
#define PMU_GLB_SLP_FRC_SW1_SET_N7_SET_CLR(w)   w &= ~( 0x1 << 13)

#define PMU_GLB_SLP_FRC_SW1_SET_N6_SET_POS      12
#define PMU_GLB_SLP_FRC_SW1_SET_N6_SET_LEN      1
#define PMU_GLB_SLP_FRC_SW1_SET_N6_SET_MASK     0x00001000
#define PMU_GLB_SLP_FRC_SW1_SET_N6_SET_VAL(w)   (((w) & 0x00001000) >> 12)
#define PMU_GLB_SLP_FRC_SW1_SET_N6_SET_SET(w)   w |= ( 0x1 << 12)
#define PMU_GLB_SLP_FRC_SW1_SET_N6_SET_CLR(w)   w &= ~( 0x1 << 12)

#define PMU_GLB_SLP_FRC_SW1_SET_N5_SET_POS      11
#define PMU_GLB_SLP_FRC_SW1_SET_N5_SET_LEN      1
#define PMU_GLB_SLP_FRC_SW1_SET_N5_SET_MASK     0x00000800
#define PMU_GLB_SLP_FRC_SW1_SET_N5_SET_VAL(w)   (((w) & 0x00000800) >> 11)
#define PMU_GLB_SLP_FRC_SW1_SET_N5_SET_SET(w)   w |= ( 0x1 << 11)
#define PMU_GLB_SLP_FRC_SW1_SET_N5_SET_CLR(w)   w &= ~( 0x1 << 11)

#define PMU_GLB_SLP_FRC_SW1_SET_N4_SET_POS      10
#define PMU_GLB_SLP_FRC_SW1_SET_N4_SET_LEN      1
#define PMU_GLB_SLP_FRC_SW1_SET_N4_SET_MASK     0x00000400
#define PMU_GLB_SLP_FRC_SW1_SET_N4_SET_VAL(w)   (((w) & 0x00000400) >> 10)
#define PMU_GLB_SLP_FRC_SW1_SET_N4_SET_SET(w)   w |= ( 0x1 << 10)
#define PMU_GLB_SLP_FRC_SW1_SET_N4_SET_CLR(w)   w &= ~( 0x1 << 10)

#define PMU_GLB_SLP_FRC_SW1_SET_N3_SET_POS      9
#define PMU_GLB_SLP_FRC_SW1_SET_N3_SET_LEN      1
#define PMU_GLB_SLP_FRC_SW1_SET_N3_SET_MASK     0x00000200
#define PMU_GLB_SLP_FRC_SW1_SET_N3_SET_VAL(w)   (((w) & 0x00000200) >> 9)
#define PMU_GLB_SLP_FRC_SW1_SET_N3_SET_SET(w)   w |= ( 0x1 << 9)
#define PMU_GLB_SLP_FRC_SW1_SET_N3_SET_CLR(w)   w &= ~( 0x1 << 9)

#define PMU_GLB_SLP_FRC_SW1_SET_N2_SET_POS      8
#define PMU_GLB_SLP_FRC_SW1_SET_N2_SET_LEN      1
#define PMU_GLB_SLP_FRC_SW1_SET_N2_SET_MASK     0x00000100
#define PMU_GLB_SLP_FRC_SW1_SET_N2_SET_VAL(w)   (((w) & 0x00000100) >> 8)
#define PMU_GLB_SLP_FRC_SW1_SET_N2_SET_SET(w)   w |= ( 0x1 << 8)
#define PMU_GLB_SLP_FRC_SW1_SET_N2_SET_CLR(w)   w &= ~( 0x1 << 8)

#define PMU_GLB_SLP_FRC_SW1_SET_N1_SET_POS      7
#define PMU_GLB_SLP_FRC_SW1_SET_N1_SET_LEN      1
#define PMU_GLB_SLP_FRC_SW1_SET_N1_SET_MASK     0x00000080
#define PMU_GLB_SLP_FRC_SW1_SET_N1_SET_VAL(w)   (((w) & 0x00000080) >> 7)
#define PMU_GLB_SLP_FRC_SW1_SET_N1_SET_SET(w)   w |= ( 0x1 << 7)
#define PMU_GLB_SLP_FRC_SW1_SET_N1_SET_CLR(w)   w &= ~( 0x1 << 7)

#define PMU_GLB_SLP_FRC_SW1_SET_N0_SET_POS      6
#define PMU_GLB_SLP_FRC_SW1_SET_N0_SET_LEN      1
#define PMU_GLB_SLP_FRC_SW1_SET_N0_SET_MASK     0x00000040
#define PMU_GLB_SLP_FRC_SW1_SET_N0_SET_VAL(w)   (((w) & 0x00000040) >> 6)
#define PMU_GLB_SLP_FRC_SW1_SET_N0_SET_SET(w)   w |= ( 0x1 << 6)
#define PMU_GLB_SLP_FRC_SW1_SET_N0_SET_CLR(w)   w &= ~( 0x1 << 6)

#define PMU_GLB_SLP_FRC_SW1_SET_MM_SET_POS      5
#define PMU_GLB_SLP_FRC_SW1_SET_MM_SET_LEN      1
#define PMU_GLB_SLP_FRC_SW1_SET_MM_SET_MASK     0x00000020
#define PMU_GLB_SLP_FRC_SW1_SET_MM_SET_VAL(w)   (((w) & 0x00000020) >> 5)
#define PMU_GLB_SLP_FRC_SW1_SET_MM_SET_SET(w)   w |= ( 0x1 << 5)
#define PMU_GLB_SLP_FRC_SW1_SET_MM_SET_CLR(w)   w &= ~( 0x1 << 5)

#define PMU_GLB_SLP_FRC_SW1_SET_PERIP_SET_POS      4
#define PMU_GLB_SLP_FRC_SW1_SET_PERIP_SET_LEN      1
#define PMU_GLB_SLP_FRC_SW1_SET_PERIP_SET_MASK     0x00000010
#define PMU_GLB_SLP_FRC_SW1_SET_PERIP_SET_VAL(w)   (((w) & 0x00000010) >> 4)
#define PMU_GLB_SLP_FRC_SW1_SET_PERIP_SET_SET(w)   w |= ( 0x1 << 4)
#define PMU_GLB_SLP_FRC_SW1_SET_PERIP_SET_CLR(w)   w &= ~( 0x1 << 4)

#define PMU_GLB_SLP_FRC_SW1_SET_PIPE_SET_POS      3
#define PMU_GLB_SLP_FRC_SW1_SET_PIPE_SET_LEN      1
#define PMU_GLB_SLP_FRC_SW1_SET_PIPE_SET_MASK     0x00000008
#define PMU_GLB_SLP_FRC_SW1_SET_PIPE_SET_VAL(w)   (((w) & 0x00000008) >> 3)
#define PMU_GLB_SLP_FRC_SW1_SET_PIPE_SET_SET(w)   w |= ( 0x1 << 3)
#define PMU_GLB_SLP_FRC_SW1_SET_PIPE_SET_CLR(w)   w &= ~( 0x1 << 3)

#define PMU_GLB_SLP_FRC_SW1_SET_FLASH_SET_POS      2
#define PMU_GLB_SLP_FRC_SW1_SET_FLASH_SET_LEN      1
#define PMU_GLB_SLP_FRC_SW1_SET_FLASH_SET_MASK     0x00000004
#define PMU_GLB_SLP_FRC_SW1_SET_FLASH_SET_VAL(w)   (((w) & 0x00000004) >> 2)
#define PMU_GLB_SLP_FRC_SW1_SET_FLASH_SET_SET(w)   w |= ( 0x1 << 2)
#define PMU_GLB_SLP_FRC_SW1_SET_FLASH_SET_CLR(w)   w &= ~( 0x1 << 2)

#define PMU_GLB_SLP_FRC_SW1_SET_DDR1_SET_POS      1
#define PMU_GLB_SLP_FRC_SW1_SET_DDR1_SET_LEN      1
#define PMU_GLB_SLP_FRC_SW1_SET_DDR1_SET_MASK     0x00000002
#define PMU_GLB_SLP_FRC_SW1_SET_DDR1_SET_VAL(w)   (((w) & 0x00000002) >> 1)
#define PMU_GLB_SLP_FRC_SW1_SET_DDR1_SET_SET(w)   w |= ( 0x1 << 1)
#define PMU_GLB_SLP_FRC_SW1_SET_DDR1_SET_CLR(w)   w &= ~( 0x1 << 1)

#define PMU_GLB_SLP_FRC_SW1_SET_DDR0_SET_POS      0
#define PMU_GLB_SLP_FRC_SW1_SET_DDR0_SET_LEN      1
#define PMU_GLB_SLP_FRC_SW1_SET_DDR0_SET_MASK     0x00000001
#define PMU_GLB_SLP_FRC_SW1_SET_DDR0_SET_VAL(w)   (((w) & 0x00000001) >> 0)
#define PMU_GLB_SLP_FRC_SW1_SET_DDR0_SET_SET(w)   w |= ( 0x1 << 0)
#define PMU_GLB_SLP_FRC_SW1_SET_DDR0_SET_CLR(w)   w &= ~( 0x1 << 0)

// REGISTER : SLP_FRC_SW1_CLR

#define PMU_GLB_SLP_FRC_SW1_CLR_ADDR      PMU_GLB_BASE_ADDR + 0x198
#define PMU_GLB_SLP_FRC_SW1_CLR_RSTVAL    0x0
#define PMU_GLB_SLP_FRC_SW1_CLR_ALL1      0x0
#define PMU_GLB_SLP_FRC_SW1_CLR_ALL0      0x0
#define PMU_GLB_SLP_FRC_SW1_CLR_ALL5A     0x0
#define PMU_GLB_SLP_FRC_SW1_CLR_ALLA5     0x0

// FIELD IN REGISTER : SLP_FRC_SW1_CLR

#define PMU_GLB_SLP_FRC_SW1_CLR_SEN_CLR_POS      18
#define PMU_GLB_SLP_FRC_SW1_CLR_SEN_CLR_LEN      1
#define PMU_GLB_SLP_FRC_SW1_CLR_SEN_CLR_MASK     0x00040000
#define PMU_GLB_SLP_FRC_SW1_CLR_SEN_CLR_VAL(w)   (((w) & 0x00040000) >> 18)
#define PMU_GLB_SLP_FRC_SW1_CLR_SEN_CLR_SET(w)   w |= ( 0x1 << 18)
#define PMU_GLB_SLP_FRC_SW1_CLR_SEN_CLR_CLR(w)   w &= ~( 0x1 << 18)

#define PMU_GLB_SLP_FRC_SW1_CLR_ISP_CLR_POS      17
#define PMU_GLB_SLP_FRC_SW1_CLR_ISP_CLR_LEN      1
#define PMU_GLB_SLP_FRC_SW1_CLR_ISP_CLR_MASK     0x00020000
#define PMU_GLB_SLP_FRC_SW1_CLR_ISP_CLR_VAL(w)   (((w) & 0x00020000) >> 17)
#define PMU_GLB_SLP_FRC_SW1_CLR_ISP_CLR_SET(w)   w |= ( 0x1 << 17)
#define PMU_GLB_SLP_FRC_SW1_CLR_ISP_CLR_CLR(w)   w &= ~( 0x1 << 17)

#define PMU_GLB_SLP_FRC_SW1_CLR_CPU_CLR_POS      16
#define PMU_GLB_SLP_FRC_SW1_CLR_CPU_CLR_LEN      1
#define PMU_GLB_SLP_FRC_SW1_CLR_CPU_CLR_MASK     0x00010000
#define PMU_GLB_SLP_FRC_SW1_CLR_CPU_CLR_VAL(w)   (((w) & 0x00010000) >> 16)
#define PMU_GLB_SLP_FRC_SW1_CLR_CPU_CLR_SET(w)   w |= ( 0x1 << 16)
#define PMU_GLB_SLP_FRC_SW1_CLR_CPU_CLR_CLR(w)   w &= ~( 0x1 << 16)

#define PMU_GLB_SLP_FRC_SW1_CLR_NPU_CLR_POS      15
#define PMU_GLB_SLP_FRC_SW1_CLR_NPU_CLR_LEN      1
#define PMU_GLB_SLP_FRC_SW1_CLR_NPU_CLR_MASK     0x00008000
#define PMU_GLB_SLP_FRC_SW1_CLR_NPU_CLR_VAL(w)   (((w) & 0x00008000) >> 15)
#define PMU_GLB_SLP_FRC_SW1_CLR_NPU_CLR_SET(w)   w |= ( 0x1 << 15)
#define PMU_GLB_SLP_FRC_SW1_CLR_NPU_CLR_CLR(w)   w &= ~( 0x1 << 15)

#define PMU_GLB_SLP_FRC_SW1_CLR_N8_CLR_POS      14
#define PMU_GLB_SLP_FRC_SW1_CLR_N8_CLR_LEN      1
#define PMU_GLB_SLP_FRC_SW1_CLR_N8_CLR_MASK     0x00004000
#define PMU_GLB_SLP_FRC_SW1_CLR_N8_CLR_VAL(w)   (((w) & 0x00004000) >> 14)
#define PMU_GLB_SLP_FRC_SW1_CLR_N8_CLR_SET(w)   w |= ( 0x1 << 14)
#define PMU_GLB_SLP_FRC_SW1_CLR_N8_CLR_CLR(w)   w &= ~( 0x1 << 14)

#define PMU_GLB_SLP_FRC_SW1_CLR_N7_CLR_POS      13
#define PMU_GLB_SLP_FRC_SW1_CLR_N7_CLR_LEN      1
#define PMU_GLB_SLP_FRC_SW1_CLR_N7_CLR_MASK     0x00002000
#define PMU_GLB_SLP_FRC_SW1_CLR_N7_CLR_VAL(w)   (((w) & 0x00002000) >> 13)
#define PMU_GLB_SLP_FRC_SW1_CLR_N7_CLR_SET(w)   w |= ( 0x1 << 13)
#define PMU_GLB_SLP_FRC_SW1_CLR_N7_CLR_CLR(w)   w &= ~( 0x1 << 13)

#define PMU_GLB_SLP_FRC_SW1_CLR_N6_CLR_POS      12
#define PMU_GLB_SLP_FRC_SW1_CLR_N6_CLR_LEN      1
#define PMU_GLB_SLP_FRC_SW1_CLR_N6_CLR_MASK     0x00001000
#define PMU_GLB_SLP_FRC_SW1_CLR_N6_CLR_VAL(w)   (((w) & 0x00001000) >> 12)
#define PMU_GLB_SLP_FRC_SW1_CLR_N6_CLR_SET(w)   w |= ( 0x1 << 12)
#define PMU_GLB_SLP_FRC_SW1_CLR_N6_CLR_CLR(w)   w &= ~( 0x1 << 12)

#define PMU_GLB_SLP_FRC_SW1_CLR_N5_CLR_POS      11
#define PMU_GLB_SLP_FRC_SW1_CLR_N5_CLR_LEN      1
#define PMU_GLB_SLP_FRC_SW1_CLR_N5_CLR_MASK     0x00000800
#define PMU_GLB_SLP_FRC_SW1_CLR_N5_CLR_VAL(w)   (((w) & 0x00000800) >> 11)
#define PMU_GLB_SLP_FRC_SW1_CLR_N5_CLR_SET(w)   w |= ( 0x1 << 11)
#define PMU_GLB_SLP_FRC_SW1_CLR_N5_CLR_CLR(w)   w &= ~( 0x1 << 11)

#define PMU_GLB_SLP_FRC_SW1_CLR_N4_CLR_POS      10
#define PMU_GLB_SLP_FRC_SW1_CLR_N4_CLR_LEN      1
#define PMU_GLB_SLP_FRC_SW1_CLR_N4_CLR_MASK     0x00000400
#define PMU_GLB_SLP_FRC_SW1_CLR_N4_CLR_VAL(w)   (((w) & 0x00000400) >> 10)
#define PMU_GLB_SLP_FRC_SW1_CLR_N4_CLR_SET(w)   w |= ( 0x1 << 10)
#define PMU_GLB_SLP_FRC_SW1_CLR_N4_CLR_CLR(w)   w &= ~( 0x1 << 10)

#define PMU_GLB_SLP_FRC_SW1_CLR_N3_CLR_POS      9
#define PMU_GLB_SLP_FRC_SW1_CLR_N3_CLR_LEN      1
#define PMU_GLB_SLP_FRC_SW1_CLR_N3_CLR_MASK     0x00000200
#define PMU_GLB_SLP_FRC_SW1_CLR_N3_CLR_VAL(w)   (((w) & 0x00000200) >> 9)
#define PMU_GLB_SLP_FRC_SW1_CLR_N3_CLR_SET(w)   w |= ( 0x1 << 9)
#define PMU_GLB_SLP_FRC_SW1_CLR_N3_CLR_CLR(w)   w &= ~( 0x1 << 9)

#define PMU_GLB_SLP_FRC_SW1_CLR_N2_CLR_POS      8
#define PMU_GLB_SLP_FRC_SW1_CLR_N2_CLR_LEN      1
#define PMU_GLB_SLP_FRC_SW1_CLR_N2_CLR_MASK     0x00000100
#define PMU_GLB_SLP_FRC_SW1_CLR_N2_CLR_VAL(w)   (((w) & 0x00000100) >> 8)
#define PMU_GLB_SLP_FRC_SW1_CLR_N2_CLR_SET(w)   w |= ( 0x1 << 8)
#define PMU_GLB_SLP_FRC_SW1_CLR_N2_CLR_CLR(w)   w &= ~( 0x1 << 8)

#define PMU_GLB_SLP_FRC_SW1_CLR_N1_CLR_POS      7
#define PMU_GLB_SLP_FRC_SW1_CLR_N1_CLR_LEN      1
#define PMU_GLB_SLP_FRC_SW1_CLR_N1_CLR_MASK     0x00000080
#define PMU_GLB_SLP_FRC_SW1_CLR_N1_CLR_VAL(w)   (((w) & 0x00000080) >> 7)
#define PMU_GLB_SLP_FRC_SW1_CLR_N1_CLR_SET(w)   w |= ( 0x1 << 7)
#define PMU_GLB_SLP_FRC_SW1_CLR_N1_CLR_CLR(w)   w &= ~( 0x1 << 7)

#define PMU_GLB_SLP_FRC_SW1_CLR_N0_CLR_POS      6
#define PMU_GLB_SLP_FRC_SW1_CLR_N0_CLR_LEN      1
#define PMU_GLB_SLP_FRC_SW1_CLR_N0_CLR_MASK     0x00000040
#define PMU_GLB_SLP_FRC_SW1_CLR_N0_CLR_VAL(w)   (((w) & 0x00000040) >> 6)
#define PMU_GLB_SLP_FRC_SW1_CLR_N0_CLR_SET(w)   w |= ( 0x1 << 6)
#define PMU_GLB_SLP_FRC_SW1_CLR_N0_CLR_CLR(w)   w &= ~( 0x1 << 6)

#define PMU_GLB_SLP_FRC_SW1_CLR_MM_CLR_POS      5
#define PMU_GLB_SLP_FRC_SW1_CLR_MM_CLR_LEN      1
#define PMU_GLB_SLP_FRC_SW1_CLR_MM_CLR_MASK     0x00000020
#define PMU_GLB_SLP_FRC_SW1_CLR_MM_CLR_VAL(w)   (((w) & 0x00000020) >> 5)
#define PMU_GLB_SLP_FRC_SW1_CLR_MM_CLR_SET(w)   w |= ( 0x1 << 5)
#define PMU_GLB_SLP_FRC_SW1_CLR_MM_CLR_CLR(w)   w &= ~( 0x1 << 5)

#define PMU_GLB_SLP_FRC_SW1_CLR_PERIP_CLR_POS      4
#define PMU_GLB_SLP_FRC_SW1_CLR_PERIP_CLR_LEN      1
#define PMU_GLB_SLP_FRC_SW1_CLR_PERIP_CLR_MASK     0x00000010
#define PMU_GLB_SLP_FRC_SW1_CLR_PERIP_CLR_VAL(w)   (((w) & 0x00000010) >> 4)
#define PMU_GLB_SLP_FRC_SW1_CLR_PERIP_CLR_SET(w)   w |= ( 0x1 << 4)
#define PMU_GLB_SLP_FRC_SW1_CLR_PERIP_CLR_CLR(w)   w &= ~( 0x1 << 4)

#define PMU_GLB_SLP_FRC_SW1_CLR_PIPE_CLR_POS      3
#define PMU_GLB_SLP_FRC_SW1_CLR_PIPE_CLR_LEN      1
#define PMU_GLB_SLP_FRC_SW1_CLR_PIPE_CLR_MASK     0x00000008
#define PMU_GLB_SLP_FRC_SW1_CLR_PIPE_CLR_VAL(w)   (((w) & 0x00000008) >> 3)
#define PMU_GLB_SLP_FRC_SW1_CLR_PIPE_CLR_SET(w)   w |= ( 0x1 << 3)
#define PMU_GLB_SLP_FRC_SW1_CLR_PIPE_CLR_CLR(w)   w &= ~( 0x1 << 3)

#define PMU_GLB_SLP_FRC_SW1_CLR_FLASH_CLR_POS      2
#define PMU_GLB_SLP_FRC_SW1_CLR_FLASH_CLR_LEN      1
#define PMU_GLB_SLP_FRC_SW1_CLR_FLASH_CLR_MASK     0x00000004
#define PMU_GLB_SLP_FRC_SW1_CLR_FLASH_CLR_VAL(w)   (((w) & 0x00000004) >> 2)
#define PMU_GLB_SLP_FRC_SW1_CLR_FLASH_CLR_SET(w)   w |= ( 0x1 << 2)
#define PMU_GLB_SLP_FRC_SW1_CLR_FLASH_CLR_CLR(w)   w &= ~( 0x1 << 2)

#define PMU_GLB_SLP_FRC_SW1_CLR_DDR1_CLR_POS      1
#define PMU_GLB_SLP_FRC_SW1_CLR_DDR1_CLR_LEN      1
#define PMU_GLB_SLP_FRC_SW1_CLR_DDR1_CLR_MASK     0x00000002
#define PMU_GLB_SLP_FRC_SW1_CLR_DDR1_CLR_VAL(w)   (((w) & 0x00000002) >> 1)
#define PMU_GLB_SLP_FRC_SW1_CLR_DDR1_CLR_SET(w)   w |= ( 0x1 << 1)
#define PMU_GLB_SLP_FRC_SW1_CLR_DDR1_CLR_CLR(w)   w &= ~( 0x1 << 1)

#define PMU_GLB_SLP_FRC_SW1_CLR_DDR0_CLR_POS      0
#define PMU_GLB_SLP_FRC_SW1_CLR_DDR0_CLR_LEN      1
#define PMU_GLB_SLP_FRC_SW1_CLR_DDR0_CLR_MASK     0x00000001
#define PMU_GLB_SLP_FRC_SW1_CLR_DDR0_CLR_VAL(w)   (((w) & 0x00000001) >> 0)
#define PMU_GLB_SLP_FRC_SW1_CLR_DDR0_CLR_SET(w)   w |= ( 0x1 << 0)
#define PMU_GLB_SLP_FRC_SW1_CLR_DDR0_CLR_CLR(w)   w &= ~( 0x1 << 0)

// REGISTER : PWROFF_FRC_EN_SET

#define PMU_GLB_PWROFF_FRC_EN_SET_ADDR      PMU_GLB_BASE_ADDR + 0x19C
#define PMU_GLB_PWROFF_FRC_EN_SET_RSTVAL    0x0
#define PMU_GLB_PWROFF_FRC_EN_SET_ALL1      0x0
#define PMU_GLB_PWROFF_FRC_EN_SET_ALL0      0x0
#define PMU_GLB_PWROFF_FRC_EN_SET_ALL5A     0x0
#define PMU_GLB_PWROFF_FRC_EN_SET_ALLA5     0x0

// FIELD IN REGISTER : PWROFF_FRC_EN_SET

#define PMU_GLB_PWROFF_FRC_EN_SET_ISP_SET_POS      14
#define PMU_GLB_PWROFF_FRC_EN_SET_ISP_SET_LEN      1
#define PMU_GLB_PWROFF_FRC_EN_SET_ISP_SET_MASK     0x00004000
#define PMU_GLB_PWROFF_FRC_EN_SET_ISP_SET_VAL(w)   (((w) & 0x00004000) >> 14)
#define PMU_GLB_PWROFF_FRC_EN_SET_ISP_SET_SET(w)   w |= ( 0x1 << 14)
#define PMU_GLB_PWROFF_FRC_EN_SET_ISP_SET_CLR(w)   w &= ~( 0x1 << 14)

#define PMU_GLB_PWROFF_FRC_EN_SET_CPU_SET_POS      13
#define PMU_GLB_PWROFF_FRC_EN_SET_CPU_SET_LEN      1
#define PMU_GLB_PWROFF_FRC_EN_SET_CPU_SET_MASK     0x00002000
#define PMU_GLB_PWROFF_FRC_EN_SET_CPU_SET_VAL(w)   (((w) & 0x00002000) >> 13)
#define PMU_GLB_PWROFF_FRC_EN_SET_CPU_SET_SET(w)   w |= ( 0x1 << 13)
#define PMU_GLB_PWROFF_FRC_EN_SET_CPU_SET_CLR(w)   w &= ~( 0x1 << 13)

#define PMU_GLB_PWROFF_FRC_EN_SET_NPU_SET_POS      12
#define PMU_GLB_PWROFF_FRC_EN_SET_NPU_SET_LEN      1
#define PMU_GLB_PWROFF_FRC_EN_SET_NPU_SET_MASK     0x00001000
#define PMU_GLB_PWROFF_FRC_EN_SET_NPU_SET_VAL(w)   (((w) & 0x00001000) >> 12)
#define PMU_GLB_PWROFF_FRC_EN_SET_NPU_SET_SET(w)   w |= ( 0x1 << 12)
#define PMU_GLB_PWROFF_FRC_EN_SET_NPU_SET_CLR(w)   w &= ~( 0x1 << 12)

#define PMU_GLB_PWROFF_FRC_EN_SET_CPU7_SET_POS      11
#define PMU_GLB_PWROFF_FRC_EN_SET_CPU7_SET_LEN      1
#define PMU_GLB_PWROFF_FRC_EN_SET_CPU7_SET_MASK     0x00000800
#define PMU_GLB_PWROFF_FRC_EN_SET_CPU7_SET_VAL(w)   (((w) & 0x00000800) >> 11)
#define PMU_GLB_PWROFF_FRC_EN_SET_CPU7_SET_SET(w)   w |= ( 0x1 << 11)
#define PMU_GLB_PWROFF_FRC_EN_SET_CPU7_SET_CLR(w)   w &= ~( 0x1 << 11)

#define PMU_GLB_PWROFF_FRC_EN_SET_CPU6_SET_POS      10
#define PMU_GLB_PWROFF_FRC_EN_SET_CPU6_SET_LEN      1
#define PMU_GLB_PWROFF_FRC_EN_SET_CPU6_SET_MASK     0x00000400
#define PMU_GLB_PWROFF_FRC_EN_SET_CPU6_SET_VAL(w)   (((w) & 0x00000400) >> 10)
#define PMU_GLB_PWROFF_FRC_EN_SET_CPU6_SET_SET(w)   w |= ( 0x1 << 10)
#define PMU_GLB_PWROFF_FRC_EN_SET_CPU6_SET_CLR(w)   w &= ~( 0x1 << 10)

#define PMU_GLB_PWROFF_FRC_EN_SET_CPU5_SET_POS      9
#define PMU_GLB_PWROFF_FRC_EN_SET_CPU5_SET_LEN      1
#define PMU_GLB_PWROFF_FRC_EN_SET_CPU5_SET_MASK     0x00000200
#define PMU_GLB_PWROFF_FRC_EN_SET_CPU5_SET_VAL(w)   (((w) & 0x00000200) >> 9)
#define PMU_GLB_PWROFF_FRC_EN_SET_CPU5_SET_SET(w)   w |= ( 0x1 << 9)
#define PMU_GLB_PWROFF_FRC_EN_SET_CPU5_SET_CLR(w)   w &= ~( 0x1 << 9)

#define PMU_GLB_PWROFF_FRC_EN_SET_CPU4_SET_POS      8
#define PMU_GLB_PWROFF_FRC_EN_SET_CPU4_SET_LEN      1
#define PMU_GLB_PWROFF_FRC_EN_SET_CPU4_SET_MASK     0x00000100
#define PMU_GLB_PWROFF_FRC_EN_SET_CPU4_SET_VAL(w)   (((w) & 0x00000100) >> 8)
#define PMU_GLB_PWROFF_FRC_EN_SET_CPU4_SET_SET(w)   w |= ( 0x1 << 8)
#define PMU_GLB_PWROFF_FRC_EN_SET_CPU4_SET_CLR(w)   w &= ~( 0x1 << 8)

#define PMU_GLB_PWROFF_FRC_EN_SET_CPU3_SET_POS      7
#define PMU_GLB_PWROFF_FRC_EN_SET_CPU3_SET_LEN      1
#define PMU_GLB_PWROFF_FRC_EN_SET_CPU3_SET_MASK     0x00000080
#define PMU_GLB_PWROFF_FRC_EN_SET_CPU3_SET_VAL(w)   (((w) & 0x00000080) >> 7)
#define PMU_GLB_PWROFF_FRC_EN_SET_CPU3_SET_SET(w)   w |= ( 0x1 << 7)
#define PMU_GLB_PWROFF_FRC_EN_SET_CPU3_SET_CLR(w)   w &= ~( 0x1 << 7)

#define PMU_GLB_PWROFF_FRC_EN_SET_CPU2_SET_POS      6
#define PMU_GLB_PWROFF_FRC_EN_SET_CPU2_SET_LEN      1
#define PMU_GLB_PWROFF_FRC_EN_SET_CPU2_SET_MASK     0x00000040
#define PMU_GLB_PWROFF_FRC_EN_SET_CPU2_SET_VAL(w)   (((w) & 0x00000040) >> 6)
#define PMU_GLB_PWROFF_FRC_EN_SET_CPU2_SET_SET(w)   w |= ( 0x1 << 6)
#define PMU_GLB_PWROFF_FRC_EN_SET_CPU2_SET_CLR(w)   w &= ~( 0x1 << 6)

#define PMU_GLB_PWROFF_FRC_EN_SET_CPU1_SET_POS      5
#define PMU_GLB_PWROFF_FRC_EN_SET_CPU1_SET_LEN      1
#define PMU_GLB_PWROFF_FRC_EN_SET_CPU1_SET_MASK     0x00000020
#define PMU_GLB_PWROFF_FRC_EN_SET_CPU1_SET_VAL(w)   (((w) & 0x00000020) >> 5)
#define PMU_GLB_PWROFF_FRC_EN_SET_CPU1_SET_SET(w)   w |= ( 0x1 << 5)
#define PMU_GLB_PWROFF_FRC_EN_SET_CPU1_SET_CLR(w)   w &= ~( 0x1 << 5)

#define PMU_GLB_PWROFF_FRC_EN_SET_CPU0_SET_POS      4
#define PMU_GLB_PWROFF_FRC_EN_SET_CPU0_SET_LEN      1
#define PMU_GLB_PWROFF_FRC_EN_SET_CPU0_SET_MASK     0x00000010
#define PMU_GLB_PWROFF_FRC_EN_SET_CPU0_SET_VAL(w)   (((w) & 0x00000010) >> 4)
#define PMU_GLB_PWROFF_FRC_EN_SET_CPU0_SET_SET(w)   w |= ( 0x1 << 4)
#define PMU_GLB_PWROFF_FRC_EN_SET_CPU0_SET_CLR(w)   w &= ~( 0x1 << 4)

#define PMU_GLB_PWROFF_FRC_EN_SET_DSP_SET_POS      3
#define PMU_GLB_PWROFF_FRC_EN_SET_DSP_SET_LEN      1
#define PMU_GLB_PWROFF_FRC_EN_SET_DSP_SET_MASK     0x00000008
#define PMU_GLB_PWROFF_FRC_EN_SET_DSP_SET_VAL(w)   (((w) & 0x00000008) >> 3)
#define PMU_GLB_PWROFF_FRC_EN_SET_DSP_SET_SET(w)   w |= ( 0x1 << 3)
#define PMU_GLB_PWROFF_FRC_EN_SET_DSP_SET_CLR(w)   w &= ~( 0x1 << 3)

#define PMU_GLB_PWROFF_FRC_EN_SET_VDEC_SET_POS      2
#define PMU_GLB_PWROFF_FRC_EN_SET_VDEC_SET_LEN      1
#define PMU_GLB_PWROFF_FRC_EN_SET_VDEC_SET_MASK     0x00000004
#define PMU_GLB_PWROFF_FRC_EN_SET_VDEC_SET_VAL(w)   (((w) & 0x00000004) >> 2)
#define PMU_GLB_PWROFF_FRC_EN_SET_VDEC_SET_SET(w)   w |= ( 0x1 << 2)
#define PMU_GLB_PWROFF_FRC_EN_SET_VDEC_SET_CLR(w)   w &= ~( 0x1 << 2)

#define PMU_GLB_PWROFF_FRC_EN_SET_VENC1_SET_POS      1
#define PMU_GLB_PWROFF_FRC_EN_SET_VENC1_SET_LEN      1
#define PMU_GLB_PWROFF_FRC_EN_SET_VENC1_SET_MASK     0x00000002
#define PMU_GLB_PWROFF_FRC_EN_SET_VENC1_SET_VAL(w)   (((w) & 0x00000002) >> 1)
#define PMU_GLB_PWROFF_FRC_EN_SET_VENC1_SET_SET(w)   w |= ( 0x1 << 1)
#define PMU_GLB_PWROFF_FRC_EN_SET_VENC1_SET_CLR(w)   w &= ~( 0x1 << 1)

#define PMU_GLB_PWROFF_FRC_EN_SET_VENC0_SET_POS      0
#define PMU_GLB_PWROFF_FRC_EN_SET_VENC0_SET_LEN      1
#define PMU_GLB_PWROFF_FRC_EN_SET_VENC0_SET_MASK     0x00000001
#define PMU_GLB_PWROFF_FRC_EN_SET_VENC0_SET_VAL(w)   (((w) & 0x00000001) >> 0)
#define PMU_GLB_PWROFF_FRC_EN_SET_VENC0_SET_SET(w)   w |= ( 0x1 << 0)
#define PMU_GLB_PWROFF_FRC_EN_SET_VENC0_SET_CLR(w)   w &= ~( 0x1 << 0)

// REGISTER : PWROFF_FRC_EN_CLR

#define PMU_GLB_PWROFF_FRC_EN_CLR_ADDR      PMU_GLB_BASE_ADDR + 0x1A0
#define PMU_GLB_PWROFF_FRC_EN_CLR_RSTVAL    0x0
#define PMU_GLB_PWROFF_FRC_EN_CLR_ALL1      0x0
#define PMU_GLB_PWROFF_FRC_EN_CLR_ALL0      0x0
#define PMU_GLB_PWROFF_FRC_EN_CLR_ALL5A     0x0
#define PMU_GLB_PWROFF_FRC_EN_CLR_ALLA5     0x0

// FIELD IN REGISTER : PWROFF_FRC_EN_CLR

#define PMU_GLB_PWROFF_FRC_EN_CLR_ISP_CLR_POS      14
#define PMU_GLB_PWROFF_FRC_EN_CLR_ISP_CLR_LEN      1
#define PMU_GLB_PWROFF_FRC_EN_CLR_ISP_CLR_MASK     0x00004000
#define PMU_GLB_PWROFF_FRC_EN_CLR_ISP_CLR_VAL(w)   (((w) & 0x00004000) >> 14)
#define PMU_GLB_PWROFF_FRC_EN_CLR_ISP_CLR_SET(w)   w |= ( 0x1 << 14)
#define PMU_GLB_PWROFF_FRC_EN_CLR_ISP_CLR_CLR(w)   w &= ~( 0x1 << 14)

#define PMU_GLB_PWROFF_FRC_EN_CLR_CPU_CLR_POS      13
#define PMU_GLB_PWROFF_FRC_EN_CLR_CPU_CLR_LEN      1
#define PMU_GLB_PWROFF_FRC_EN_CLR_CPU_CLR_MASK     0x00002000
#define PMU_GLB_PWROFF_FRC_EN_CLR_CPU_CLR_VAL(w)   (((w) & 0x00002000) >> 13)
#define PMU_GLB_PWROFF_FRC_EN_CLR_CPU_CLR_SET(w)   w |= ( 0x1 << 13)
#define PMU_GLB_PWROFF_FRC_EN_CLR_CPU_CLR_CLR(w)   w &= ~( 0x1 << 13)

#define PMU_GLB_PWROFF_FRC_EN_CLR_NPU_CLR_POS      12
#define PMU_GLB_PWROFF_FRC_EN_CLR_NPU_CLR_LEN      1
#define PMU_GLB_PWROFF_FRC_EN_CLR_NPU_CLR_MASK     0x00001000
#define PMU_GLB_PWROFF_FRC_EN_CLR_NPU_CLR_VAL(w)   (((w) & 0x00001000) >> 12)
#define PMU_GLB_PWROFF_FRC_EN_CLR_NPU_CLR_SET(w)   w |= ( 0x1 << 12)
#define PMU_GLB_PWROFF_FRC_EN_CLR_NPU_CLR_CLR(w)   w &= ~( 0x1 << 12)

#define PMU_GLB_PWROFF_FRC_EN_CLR_CPU7_CLR_POS      11
#define PMU_GLB_PWROFF_FRC_EN_CLR_CPU7_CLR_LEN      1
#define PMU_GLB_PWROFF_FRC_EN_CLR_CPU7_CLR_MASK     0x00000800
#define PMU_GLB_PWROFF_FRC_EN_CLR_CPU7_CLR_VAL(w)   (((w) & 0x00000800) >> 11)
#define PMU_GLB_PWROFF_FRC_EN_CLR_CPU7_CLR_SET(w)   w |= ( 0x1 << 11)
#define PMU_GLB_PWROFF_FRC_EN_CLR_CPU7_CLR_CLR(w)   w &= ~( 0x1 << 11)

#define PMU_GLB_PWROFF_FRC_EN_CLR_CPU6_CLR_POS      10
#define PMU_GLB_PWROFF_FRC_EN_CLR_CPU6_CLR_LEN      1
#define PMU_GLB_PWROFF_FRC_EN_CLR_CPU6_CLR_MASK     0x00000400
#define PMU_GLB_PWROFF_FRC_EN_CLR_CPU6_CLR_VAL(w)   (((w) & 0x00000400) >> 10)
#define PMU_GLB_PWROFF_FRC_EN_CLR_CPU6_CLR_SET(w)   w |= ( 0x1 << 10)
#define PMU_GLB_PWROFF_FRC_EN_CLR_CPU6_CLR_CLR(w)   w &= ~( 0x1 << 10)

#define PMU_GLB_PWROFF_FRC_EN_CLR_CPU5_CLR_POS      9
#define PMU_GLB_PWROFF_FRC_EN_CLR_CPU5_CLR_LEN      1
#define PMU_GLB_PWROFF_FRC_EN_CLR_CPU5_CLR_MASK     0x00000200
#define PMU_GLB_PWROFF_FRC_EN_CLR_CPU5_CLR_VAL(w)   (((w) & 0x00000200) >> 9)
#define PMU_GLB_PWROFF_FRC_EN_CLR_CPU5_CLR_SET(w)   w |= ( 0x1 << 9)
#define PMU_GLB_PWROFF_FRC_EN_CLR_CPU5_CLR_CLR(w)   w &= ~( 0x1 << 9)

#define PMU_GLB_PWROFF_FRC_EN_CLR_CPU4_CLR_POS      8
#define PMU_GLB_PWROFF_FRC_EN_CLR_CPU4_CLR_LEN      1
#define PMU_GLB_PWROFF_FRC_EN_CLR_CPU4_CLR_MASK     0x00000100
#define PMU_GLB_PWROFF_FRC_EN_CLR_CPU4_CLR_VAL(w)   (((w) & 0x00000100) >> 8)
#define PMU_GLB_PWROFF_FRC_EN_CLR_CPU4_CLR_SET(w)   w |= ( 0x1 << 8)
#define PMU_GLB_PWROFF_FRC_EN_CLR_CPU4_CLR_CLR(w)   w &= ~( 0x1 << 8)

#define PMU_GLB_PWROFF_FRC_EN_CLR_CPU3_CLR_POS      7
#define PMU_GLB_PWROFF_FRC_EN_CLR_CPU3_CLR_LEN      1
#define PMU_GLB_PWROFF_FRC_EN_CLR_CPU3_CLR_MASK     0x00000080
#define PMU_GLB_PWROFF_FRC_EN_CLR_CPU3_CLR_VAL(w)   (((w) & 0x00000080) >> 7)
#define PMU_GLB_PWROFF_FRC_EN_CLR_CPU3_CLR_SET(w)   w |= ( 0x1 << 7)
#define PMU_GLB_PWROFF_FRC_EN_CLR_CPU3_CLR_CLR(w)   w &= ~( 0x1 << 7)

#define PMU_GLB_PWROFF_FRC_EN_CLR_CPU2_CLR_POS      6
#define PMU_GLB_PWROFF_FRC_EN_CLR_CPU2_CLR_LEN      1
#define PMU_GLB_PWROFF_FRC_EN_CLR_CPU2_CLR_MASK     0x00000040
#define PMU_GLB_PWROFF_FRC_EN_CLR_CPU2_CLR_VAL(w)   (((w) & 0x00000040) >> 6)
#define PMU_GLB_PWROFF_FRC_EN_CLR_CPU2_CLR_SET(w)   w |= ( 0x1 << 6)
#define PMU_GLB_PWROFF_FRC_EN_CLR_CPU2_CLR_CLR(w)   w &= ~( 0x1 << 6)

#define PMU_GLB_PWROFF_FRC_EN_CLR_CPU1_CLR_POS      5
#define PMU_GLB_PWROFF_FRC_EN_CLR_CPU1_CLR_LEN      1
#define PMU_GLB_PWROFF_FRC_EN_CLR_CPU1_CLR_MASK     0x00000020
#define PMU_GLB_PWROFF_FRC_EN_CLR_CPU1_CLR_VAL(w)   (((w) & 0x00000020) >> 5)
#define PMU_GLB_PWROFF_FRC_EN_CLR_CPU1_CLR_SET(w)   w |= ( 0x1 << 5)
#define PMU_GLB_PWROFF_FRC_EN_CLR_CPU1_CLR_CLR(w)   w &= ~( 0x1 << 5)

#define PMU_GLB_PWROFF_FRC_EN_CLR_CPU0_CLR_POS      4
#define PMU_GLB_PWROFF_FRC_EN_CLR_CPU0_CLR_LEN      1
#define PMU_GLB_PWROFF_FRC_EN_CLR_CPU0_CLR_MASK     0x00000010
#define PMU_GLB_PWROFF_FRC_EN_CLR_CPU0_CLR_VAL(w)   (((w) & 0x00000010) >> 4)
#define PMU_GLB_PWROFF_FRC_EN_CLR_CPU0_CLR_SET(w)   w |= ( 0x1 << 4)
#define PMU_GLB_PWROFF_FRC_EN_CLR_CPU0_CLR_CLR(w)   w &= ~( 0x1 << 4)

#define PMU_GLB_PWROFF_FRC_EN_CLR_DSP_CLR_POS      3
#define PMU_GLB_PWROFF_FRC_EN_CLR_DSP_CLR_LEN      1
#define PMU_GLB_PWROFF_FRC_EN_CLR_DSP_CLR_MASK     0x00000008
#define PMU_GLB_PWROFF_FRC_EN_CLR_DSP_CLR_VAL(w)   (((w) & 0x00000008) >> 3)
#define PMU_GLB_PWROFF_FRC_EN_CLR_DSP_CLR_SET(w)   w |= ( 0x1 << 3)
#define PMU_GLB_PWROFF_FRC_EN_CLR_DSP_CLR_CLR(w)   w &= ~( 0x1 << 3)

#define PMU_GLB_PWROFF_FRC_EN_CLR_VDEC_CLR_POS      2
#define PMU_GLB_PWROFF_FRC_EN_CLR_VDEC_CLR_LEN      1
#define PMU_GLB_PWROFF_FRC_EN_CLR_VDEC_CLR_MASK     0x00000004
#define PMU_GLB_PWROFF_FRC_EN_CLR_VDEC_CLR_VAL(w)   (((w) & 0x00000004) >> 2)
#define PMU_GLB_PWROFF_FRC_EN_CLR_VDEC_CLR_SET(w)   w |= ( 0x1 << 2)
#define PMU_GLB_PWROFF_FRC_EN_CLR_VDEC_CLR_CLR(w)   w &= ~( 0x1 << 2)

#define PMU_GLB_PWROFF_FRC_EN_CLR_VENC1_CLR_POS      1
#define PMU_GLB_PWROFF_FRC_EN_CLR_VENC1_CLR_LEN      1
#define PMU_GLB_PWROFF_FRC_EN_CLR_VENC1_CLR_MASK     0x00000002
#define PMU_GLB_PWROFF_FRC_EN_CLR_VENC1_CLR_VAL(w)   (((w) & 0x00000002) >> 1)
#define PMU_GLB_PWROFF_FRC_EN_CLR_VENC1_CLR_SET(w)   w |= ( 0x1 << 1)
#define PMU_GLB_PWROFF_FRC_EN_CLR_VENC1_CLR_CLR(w)   w &= ~( 0x1 << 1)

#define PMU_GLB_PWROFF_FRC_EN_CLR_VENC0_CLR_POS      0
#define PMU_GLB_PWROFF_FRC_EN_CLR_VENC0_CLR_LEN      1
#define PMU_GLB_PWROFF_FRC_EN_CLR_VENC0_CLR_MASK     0x00000001
#define PMU_GLB_PWROFF_FRC_EN_CLR_VENC0_CLR_VAL(w)   (((w) & 0x00000001) >> 0)
#define PMU_GLB_PWROFF_FRC_EN_CLR_VENC0_CLR_SET(w)   w |= ( 0x1 << 0)
#define PMU_GLB_PWROFF_FRC_EN_CLR_VENC0_CLR_CLR(w)   w &= ~( 0x1 << 0)

// REGISTER : PWROFF_FRC_EN1_SET

#define PMU_GLB_PWROFF_FRC_EN1_SET_ADDR      PMU_GLB_BASE_ADDR + 0x1A4
#define PMU_GLB_PWROFF_FRC_EN1_SET_RSTVAL    0x0
#define PMU_GLB_PWROFF_FRC_EN1_SET_ALL1      0x0
#define PMU_GLB_PWROFF_FRC_EN1_SET_ALL0      0x0
#define PMU_GLB_PWROFF_FRC_EN1_SET_ALL5A     0x0
#define PMU_GLB_PWROFF_FRC_EN1_SET_ALLA5     0x0

// FIELD IN REGISTER : PWROFF_FRC_EN1_SET

#define PMU_GLB_PWROFF_FRC_EN1_SET_N8_SET_POS      10
#define PMU_GLB_PWROFF_FRC_EN1_SET_N8_SET_LEN      1
#define PMU_GLB_PWROFF_FRC_EN1_SET_N8_SET_MASK     0x00000400
#define PMU_GLB_PWROFF_FRC_EN1_SET_N8_SET_VAL(w)   (((w) & 0x00000400) >> 10)
#define PMU_GLB_PWROFF_FRC_EN1_SET_N8_SET_SET(w)   w |= ( 0x1 << 10)
#define PMU_GLB_PWROFF_FRC_EN1_SET_N8_SET_CLR(w)   w &= ~( 0x1 << 10)

#define PMU_GLB_PWROFF_FRC_EN1_SET_N7_SET_POS      9
#define PMU_GLB_PWROFF_FRC_EN1_SET_N7_SET_LEN      1
#define PMU_GLB_PWROFF_FRC_EN1_SET_N7_SET_MASK     0x00000200
#define PMU_GLB_PWROFF_FRC_EN1_SET_N7_SET_VAL(w)   (((w) & 0x00000200) >> 9)
#define PMU_GLB_PWROFF_FRC_EN1_SET_N7_SET_SET(w)   w |= ( 0x1 << 9)
#define PMU_GLB_PWROFF_FRC_EN1_SET_N7_SET_CLR(w)   w &= ~( 0x1 << 9)

#define PMU_GLB_PWROFF_FRC_EN1_SET_N6_SET_POS      8
#define PMU_GLB_PWROFF_FRC_EN1_SET_N6_SET_LEN      1
#define PMU_GLB_PWROFF_FRC_EN1_SET_N6_SET_MASK     0x00000100
#define PMU_GLB_PWROFF_FRC_EN1_SET_N6_SET_VAL(w)   (((w) & 0x00000100) >> 8)
#define PMU_GLB_PWROFF_FRC_EN1_SET_N6_SET_SET(w)   w |= ( 0x1 << 8)
#define PMU_GLB_PWROFF_FRC_EN1_SET_N6_SET_CLR(w)   w &= ~( 0x1 << 8)

#define PMU_GLB_PWROFF_FRC_EN1_SET_N5_SET_POS      7
#define PMU_GLB_PWROFF_FRC_EN1_SET_N5_SET_LEN      1
#define PMU_GLB_PWROFF_FRC_EN1_SET_N5_SET_MASK     0x00000080
#define PMU_GLB_PWROFF_FRC_EN1_SET_N5_SET_VAL(w)   (((w) & 0x00000080) >> 7)
#define PMU_GLB_PWROFF_FRC_EN1_SET_N5_SET_SET(w)   w |= ( 0x1 << 7)
#define PMU_GLB_PWROFF_FRC_EN1_SET_N5_SET_CLR(w)   w &= ~( 0x1 << 7)

#define PMU_GLB_PWROFF_FRC_EN1_SET_N4_SET_POS      6
#define PMU_GLB_PWROFF_FRC_EN1_SET_N4_SET_LEN      1
#define PMU_GLB_PWROFF_FRC_EN1_SET_N4_SET_MASK     0x00000040
#define PMU_GLB_PWROFF_FRC_EN1_SET_N4_SET_VAL(w)   (((w) & 0x00000040) >> 6)
#define PMU_GLB_PWROFF_FRC_EN1_SET_N4_SET_SET(w)   w |= ( 0x1 << 6)
#define PMU_GLB_PWROFF_FRC_EN1_SET_N4_SET_CLR(w)   w &= ~( 0x1 << 6)

#define PMU_GLB_PWROFF_FRC_EN1_SET_N3_SET_POS      5
#define PMU_GLB_PWROFF_FRC_EN1_SET_N3_SET_LEN      1
#define PMU_GLB_PWROFF_FRC_EN1_SET_N3_SET_MASK     0x00000020
#define PMU_GLB_PWROFF_FRC_EN1_SET_N3_SET_VAL(w)   (((w) & 0x00000020) >> 5)
#define PMU_GLB_PWROFF_FRC_EN1_SET_N3_SET_SET(w)   w |= ( 0x1 << 5)
#define PMU_GLB_PWROFF_FRC_EN1_SET_N3_SET_CLR(w)   w &= ~( 0x1 << 5)

#define PMU_GLB_PWROFF_FRC_EN1_SET_N2_SET_POS      4
#define PMU_GLB_PWROFF_FRC_EN1_SET_N2_SET_LEN      1
#define PMU_GLB_PWROFF_FRC_EN1_SET_N2_SET_MASK     0x00000010
#define PMU_GLB_PWROFF_FRC_EN1_SET_N2_SET_VAL(w)   (((w) & 0x00000010) >> 4)
#define PMU_GLB_PWROFF_FRC_EN1_SET_N2_SET_SET(w)   w |= ( 0x1 << 4)
#define PMU_GLB_PWROFF_FRC_EN1_SET_N2_SET_CLR(w)   w &= ~( 0x1 << 4)

#define PMU_GLB_PWROFF_FRC_EN1_SET_N1_SET_POS      3
#define PMU_GLB_PWROFF_FRC_EN1_SET_N1_SET_LEN      1
#define PMU_GLB_PWROFF_FRC_EN1_SET_N1_SET_MASK     0x00000008
#define PMU_GLB_PWROFF_FRC_EN1_SET_N1_SET_VAL(w)   (((w) & 0x00000008) >> 3)
#define PMU_GLB_PWROFF_FRC_EN1_SET_N1_SET_SET(w)   w |= ( 0x1 << 3)
#define PMU_GLB_PWROFF_FRC_EN1_SET_N1_SET_CLR(w)   w &= ~( 0x1 << 3)

#define PMU_GLB_PWROFF_FRC_EN1_SET_N0_SET_POS      2
#define PMU_GLB_PWROFF_FRC_EN1_SET_N0_SET_LEN      1
#define PMU_GLB_PWROFF_FRC_EN1_SET_N0_SET_MASK     0x00000004
#define PMU_GLB_PWROFF_FRC_EN1_SET_N0_SET_VAL(w)   (((w) & 0x00000004) >> 2)
#define PMU_GLB_PWROFF_FRC_EN1_SET_N0_SET_SET(w)   w |= ( 0x1 << 2)
#define PMU_GLB_PWROFF_FRC_EN1_SET_N0_SET_CLR(w)   w &= ~( 0x1 << 2)

#define PMU_GLB_PWROFF_FRC_EN1_SET_MM_SET_POS      1
#define PMU_GLB_PWROFF_FRC_EN1_SET_MM_SET_LEN      1
#define PMU_GLB_PWROFF_FRC_EN1_SET_MM_SET_MASK     0x00000002
#define PMU_GLB_PWROFF_FRC_EN1_SET_MM_SET_VAL(w)   (((w) & 0x00000002) >> 1)
#define PMU_GLB_PWROFF_FRC_EN1_SET_MM_SET_SET(w)   w |= ( 0x1 << 1)
#define PMU_GLB_PWROFF_FRC_EN1_SET_MM_SET_CLR(w)   w &= ~( 0x1 << 1)

#define PMU_GLB_PWROFF_FRC_EN1_SET_PIPE_SET_POS      0
#define PMU_GLB_PWROFF_FRC_EN1_SET_PIPE_SET_LEN      1
#define PMU_GLB_PWROFF_FRC_EN1_SET_PIPE_SET_MASK     0x00000001
#define PMU_GLB_PWROFF_FRC_EN1_SET_PIPE_SET_VAL(w)   (((w) & 0x00000001) >> 0)
#define PMU_GLB_PWROFF_FRC_EN1_SET_PIPE_SET_SET(w)   w |= ( 0x1 << 0)
#define PMU_GLB_PWROFF_FRC_EN1_SET_PIPE_SET_CLR(w)   w &= ~( 0x1 << 0)

// REGISTER : PWROFF_FRC_EN1_CLR

#define PMU_GLB_PWROFF_FRC_EN1_CLR_ADDR      PMU_GLB_BASE_ADDR + 0x1A8
#define PMU_GLB_PWROFF_FRC_EN1_CLR_RSTVAL    0x0
#define PMU_GLB_PWROFF_FRC_EN1_CLR_ALL1      0x0
#define PMU_GLB_PWROFF_FRC_EN1_CLR_ALL0      0x0
#define PMU_GLB_PWROFF_FRC_EN1_CLR_ALL5A     0x0
#define PMU_GLB_PWROFF_FRC_EN1_CLR_ALLA5     0x0

// FIELD IN REGISTER : PWROFF_FRC_EN1_CLR

#define PMU_GLB_PWROFF_FRC_EN1_CLR_N8_CLR_POS      10
#define PMU_GLB_PWROFF_FRC_EN1_CLR_N8_CLR_LEN      1
#define PMU_GLB_PWROFF_FRC_EN1_CLR_N8_CLR_MASK     0x00000400
#define PMU_GLB_PWROFF_FRC_EN1_CLR_N8_CLR_VAL(w)   (((w) & 0x00000400) >> 10)
#define PMU_GLB_PWROFF_FRC_EN1_CLR_N8_CLR_SET(w)   w |= ( 0x1 << 10)
#define PMU_GLB_PWROFF_FRC_EN1_CLR_N8_CLR_CLR(w)   w &= ~( 0x1 << 10)

#define PMU_GLB_PWROFF_FRC_EN1_CLR_N7_CLR_POS      9
#define PMU_GLB_PWROFF_FRC_EN1_CLR_N7_CLR_LEN      1
#define PMU_GLB_PWROFF_FRC_EN1_CLR_N7_CLR_MASK     0x00000200
#define PMU_GLB_PWROFF_FRC_EN1_CLR_N7_CLR_VAL(w)   (((w) & 0x00000200) >> 9)
#define PMU_GLB_PWROFF_FRC_EN1_CLR_N7_CLR_SET(w)   w |= ( 0x1 << 9)
#define PMU_GLB_PWROFF_FRC_EN1_CLR_N7_CLR_CLR(w)   w &= ~( 0x1 << 9)

#define PMU_GLB_PWROFF_FRC_EN1_CLR_N6_CLR_POS      8
#define PMU_GLB_PWROFF_FRC_EN1_CLR_N6_CLR_LEN      1
#define PMU_GLB_PWROFF_FRC_EN1_CLR_N6_CLR_MASK     0x00000100
#define PMU_GLB_PWROFF_FRC_EN1_CLR_N6_CLR_VAL(w)   (((w) & 0x00000100) >> 8)
#define PMU_GLB_PWROFF_FRC_EN1_CLR_N6_CLR_SET(w)   w |= ( 0x1 << 8)
#define PMU_GLB_PWROFF_FRC_EN1_CLR_N6_CLR_CLR(w)   w &= ~( 0x1 << 8)

#define PMU_GLB_PWROFF_FRC_EN1_CLR_N5_CLR_POS      7
#define PMU_GLB_PWROFF_FRC_EN1_CLR_N5_CLR_LEN      1
#define PMU_GLB_PWROFF_FRC_EN1_CLR_N5_CLR_MASK     0x00000080
#define PMU_GLB_PWROFF_FRC_EN1_CLR_N5_CLR_VAL(w)   (((w) & 0x00000080) >> 7)
#define PMU_GLB_PWROFF_FRC_EN1_CLR_N5_CLR_SET(w)   w |= ( 0x1 << 7)
#define PMU_GLB_PWROFF_FRC_EN1_CLR_N5_CLR_CLR(w)   w &= ~( 0x1 << 7)

#define PMU_GLB_PWROFF_FRC_EN1_CLR_N4_CLR_POS      6
#define PMU_GLB_PWROFF_FRC_EN1_CLR_N4_CLR_LEN      1
#define PMU_GLB_PWROFF_FRC_EN1_CLR_N4_CLR_MASK     0x00000040
#define PMU_GLB_PWROFF_FRC_EN1_CLR_N4_CLR_VAL(w)   (((w) & 0x00000040) >> 6)
#define PMU_GLB_PWROFF_FRC_EN1_CLR_N4_CLR_SET(w)   w |= ( 0x1 << 6)
#define PMU_GLB_PWROFF_FRC_EN1_CLR_N4_CLR_CLR(w)   w &= ~( 0x1 << 6)

#define PMU_GLB_PWROFF_FRC_EN1_CLR_N3_CLR_POS      5
#define PMU_GLB_PWROFF_FRC_EN1_CLR_N3_CLR_LEN      1
#define PMU_GLB_PWROFF_FRC_EN1_CLR_N3_CLR_MASK     0x00000020
#define PMU_GLB_PWROFF_FRC_EN1_CLR_N3_CLR_VAL(w)   (((w) & 0x00000020) >> 5)
#define PMU_GLB_PWROFF_FRC_EN1_CLR_N3_CLR_SET(w)   w |= ( 0x1 << 5)
#define PMU_GLB_PWROFF_FRC_EN1_CLR_N3_CLR_CLR(w)   w &= ~( 0x1 << 5)

#define PMU_GLB_PWROFF_FRC_EN1_CLR_N2_CLR_POS      4
#define PMU_GLB_PWROFF_FRC_EN1_CLR_N2_CLR_LEN      1
#define PMU_GLB_PWROFF_FRC_EN1_CLR_N2_CLR_MASK     0x00000010
#define PMU_GLB_PWROFF_FRC_EN1_CLR_N2_CLR_VAL(w)   (((w) & 0x00000010) >> 4)
#define PMU_GLB_PWROFF_FRC_EN1_CLR_N2_CLR_SET(w)   w |= ( 0x1 << 4)
#define PMU_GLB_PWROFF_FRC_EN1_CLR_N2_CLR_CLR(w)   w &= ~( 0x1 << 4)

#define PMU_GLB_PWROFF_FRC_EN1_CLR_N1_CLR_POS      3
#define PMU_GLB_PWROFF_FRC_EN1_CLR_N1_CLR_LEN      1
#define PMU_GLB_PWROFF_FRC_EN1_CLR_N1_CLR_MASK     0x00000008
#define PMU_GLB_PWROFF_FRC_EN1_CLR_N1_CLR_VAL(w)   (((w) & 0x00000008) >> 3)
#define PMU_GLB_PWROFF_FRC_EN1_CLR_N1_CLR_SET(w)   w |= ( 0x1 << 3)
#define PMU_GLB_PWROFF_FRC_EN1_CLR_N1_CLR_CLR(w)   w &= ~( 0x1 << 3)

#define PMU_GLB_PWROFF_FRC_EN1_CLR_N0_CLR_POS      2
#define PMU_GLB_PWROFF_FRC_EN1_CLR_N0_CLR_LEN      1
#define PMU_GLB_PWROFF_FRC_EN1_CLR_N0_CLR_MASK     0x00000004
#define PMU_GLB_PWROFF_FRC_EN1_CLR_N0_CLR_VAL(w)   (((w) & 0x00000004) >> 2)
#define PMU_GLB_PWROFF_FRC_EN1_CLR_N0_CLR_SET(w)   w |= ( 0x1 << 2)
#define PMU_GLB_PWROFF_FRC_EN1_CLR_N0_CLR_CLR(w)   w &= ~( 0x1 << 2)

#define PMU_GLB_PWROFF_FRC_EN1_CLR_MM_CLR_POS      1
#define PMU_GLB_PWROFF_FRC_EN1_CLR_MM_CLR_LEN      1
#define PMU_GLB_PWROFF_FRC_EN1_CLR_MM_CLR_MASK     0x00000002
#define PMU_GLB_PWROFF_FRC_EN1_CLR_MM_CLR_VAL(w)   (((w) & 0x00000002) >> 1)
#define PMU_GLB_PWROFF_FRC_EN1_CLR_MM_CLR_SET(w)   w |= ( 0x1 << 1)
#define PMU_GLB_PWROFF_FRC_EN1_CLR_MM_CLR_CLR(w)   w &= ~( 0x1 << 1)

#define PMU_GLB_PWROFF_FRC_EN1_CLR_PIPE_CLR_POS      0
#define PMU_GLB_PWROFF_FRC_EN1_CLR_PIPE_CLR_LEN      1
#define PMU_GLB_PWROFF_FRC_EN1_CLR_PIPE_CLR_MASK     0x00000001
#define PMU_GLB_PWROFF_FRC_EN1_CLR_PIPE_CLR_VAL(w)   (((w) & 0x00000001) >> 0)
#define PMU_GLB_PWROFF_FRC_EN1_CLR_PIPE_CLR_SET(w)   w |= ( 0x1 << 0)
#define PMU_GLB_PWROFF_FRC_EN1_CLR_PIPE_CLR_CLR(w)   w &= ~( 0x1 << 0)

// REGISTER : PWROFF_FRC_SW_SET

#define PMU_GLB_PWROFF_FRC_SW_SET_ADDR      PMU_GLB_BASE_ADDR + 0x1AC
#define PMU_GLB_PWROFF_FRC_SW_SET_RSTVAL    0x0
#define PMU_GLB_PWROFF_FRC_SW_SET_ALL1      0x0
#define PMU_GLB_PWROFF_FRC_SW_SET_ALL0      0x0
#define PMU_GLB_PWROFF_FRC_SW_SET_ALL5A     0x0
#define PMU_GLB_PWROFF_FRC_SW_SET_ALLA5     0x0

// FIELD IN REGISTER : PWROFF_FRC_SW_SET

#define PMU_GLB_PWROFF_FRC_SW_SET_ISP_SET_POS      14
#define PMU_GLB_PWROFF_FRC_SW_SET_ISP_SET_LEN      1
#define PMU_GLB_PWROFF_FRC_SW_SET_ISP_SET_MASK     0x00004000
#define PMU_GLB_PWROFF_FRC_SW_SET_ISP_SET_VAL(w)   (((w) & 0x00004000) >> 14)
#define PMU_GLB_PWROFF_FRC_SW_SET_ISP_SET_SET(w)   w |= ( 0x1 << 14)
#define PMU_GLB_PWROFF_FRC_SW_SET_ISP_SET_CLR(w)   w &= ~( 0x1 << 14)

#define PMU_GLB_PWROFF_FRC_SW_SET_CPU_SET_POS      13
#define PMU_GLB_PWROFF_FRC_SW_SET_CPU_SET_LEN      1
#define PMU_GLB_PWROFF_FRC_SW_SET_CPU_SET_MASK     0x00002000
#define PMU_GLB_PWROFF_FRC_SW_SET_CPU_SET_VAL(w)   (((w) & 0x00002000) >> 13)
#define PMU_GLB_PWROFF_FRC_SW_SET_CPU_SET_SET(w)   w |= ( 0x1 << 13)
#define PMU_GLB_PWROFF_FRC_SW_SET_CPU_SET_CLR(w)   w &= ~( 0x1 << 13)

#define PMU_GLB_PWROFF_FRC_SW_SET_NPU_SET_POS      12
#define PMU_GLB_PWROFF_FRC_SW_SET_NPU_SET_LEN      1
#define PMU_GLB_PWROFF_FRC_SW_SET_NPU_SET_MASK     0x00001000
#define PMU_GLB_PWROFF_FRC_SW_SET_NPU_SET_VAL(w)   (((w) & 0x00001000) >> 12)
#define PMU_GLB_PWROFF_FRC_SW_SET_NPU_SET_SET(w)   w |= ( 0x1 << 12)
#define PMU_GLB_PWROFF_FRC_SW_SET_NPU_SET_CLR(w)   w &= ~( 0x1 << 12)

#define PMU_GLB_PWROFF_FRC_SW_SET_CPU7_SET_POS      11
#define PMU_GLB_PWROFF_FRC_SW_SET_CPU7_SET_LEN      1
#define PMU_GLB_PWROFF_FRC_SW_SET_CPU7_SET_MASK     0x00000800
#define PMU_GLB_PWROFF_FRC_SW_SET_CPU7_SET_VAL(w)   (((w) & 0x00000800) >> 11)
#define PMU_GLB_PWROFF_FRC_SW_SET_CPU7_SET_SET(w)   w |= ( 0x1 << 11)
#define PMU_GLB_PWROFF_FRC_SW_SET_CPU7_SET_CLR(w)   w &= ~( 0x1 << 11)

#define PMU_GLB_PWROFF_FRC_SW_SET_CPU6_SET_POS      10
#define PMU_GLB_PWROFF_FRC_SW_SET_CPU6_SET_LEN      1
#define PMU_GLB_PWROFF_FRC_SW_SET_CPU6_SET_MASK     0x00000400
#define PMU_GLB_PWROFF_FRC_SW_SET_CPU6_SET_VAL(w)   (((w) & 0x00000400) >> 10)
#define PMU_GLB_PWROFF_FRC_SW_SET_CPU6_SET_SET(w)   w |= ( 0x1 << 10)
#define PMU_GLB_PWROFF_FRC_SW_SET_CPU6_SET_CLR(w)   w &= ~( 0x1 << 10)

#define PMU_GLB_PWROFF_FRC_SW_SET_CPU5_SET_POS      9
#define PMU_GLB_PWROFF_FRC_SW_SET_CPU5_SET_LEN      1
#define PMU_GLB_PWROFF_FRC_SW_SET_CPU5_SET_MASK     0x00000200
#define PMU_GLB_PWROFF_FRC_SW_SET_CPU5_SET_VAL(w)   (((w) & 0x00000200) >> 9)
#define PMU_GLB_PWROFF_FRC_SW_SET_CPU5_SET_SET(w)   w |= ( 0x1 << 9)
#define PMU_GLB_PWROFF_FRC_SW_SET_CPU5_SET_CLR(w)   w &= ~( 0x1 << 9)

#define PMU_GLB_PWROFF_FRC_SW_SET_CPU4_SET_POS      8
#define PMU_GLB_PWROFF_FRC_SW_SET_CPU4_SET_LEN      1
#define PMU_GLB_PWROFF_FRC_SW_SET_CPU4_SET_MASK     0x00000100
#define PMU_GLB_PWROFF_FRC_SW_SET_CPU4_SET_VAL(w)   (((w) & 0x00000100) >> 8)
#define PMU_GLB_PWROFF_FRC_SW_SET_CPU4_SET_SET(w)   w |= ( 0x1 << 8)
#define PMU_GLB_PWROFF_FRC_SW_SET_CPU4_SET_CLR(w)   w &= ~( 0x1 << 8)

#define PMU_GLB_PWROFF_FRC_SW_SET_CPU3_SET_POS      7
#define PMU_GLB_PWROFF_FRC_SW_SET_CPU3_SET_LEN      1
#define PMU_GLB_PWROFF_FRC_SW_SET_CPU3_SET_MASK     0x00000080
#define PMU_GLB_PWROFF_FRC_SW_SET_CPU3_SET_VAL(w)   (((w) & 0x00000080) >> 7)
#define PMU_GLB_PWROFF_FRC_SW_SET_CPU3_SET_SET(w)   w |= ( 0x1 << 7)
#define PMU_GLB_PWROFF_FRC_SW_SET_CPU3_SET_CLR(w)   w &= ~( 0x1 << 7)

#define PMU_GLB_PWROFF_FRC_SW_SET_CPU2_SET_POS      6
#define PMU_GLB_PWROFF_FRC_SW_SET_CPU2_SET_LEN      1
#define PMU_GLB_PWROFF_FRC_SW_SET_CPU2_SET_MASK     0x00000040
#define PMU_GLB_PWROFF_FRC_SW_SET_CPU2_SET_VAL(w)   (((w) & 0x00000040) >> 6)
#define PMU_GLB_PWROFF_FRC_SW_SET_CPU2_SET_SET(w)   w |= ( 0x1 << 6)
#define PMU_GLB_PWROFF_FRC_SW_SET_CPU2_SET_CLR(w)   w &= ~( 0x1 << 6)

#define PMU_GLB_PWROFF_FRC_SW_SET_CPU1_SET_POS      5
#define PMU_GLB_PWROFF_FRC_SW_SET_CPU1_SET_LEN      1
#define PMU_GLB_PWROFF_FRC_SW_SET_CPU1_SET_MASK     0x00000020
#define PMU_GLB_PWROFF_FRC_SW_SET_CPU1_SET_VAL(w)   (((w) & 0x00000020) >> 5)
#define PMU_GLB_PWROFF_FRC_SW_SET_CPU1_SET_SET(w)   w |= ( 0x1 << 5)
#define PMU_GLB_PWROFF_FRC_SW_SET_CPU1_SET_CLR(w)   w &= ~( 0x1 << 5)

#define PMU_GLB_PWROFF_FRC_SW_SET_CPU0_SET_POS      4
#define PMU_GLB_PWROFF_FRC_SW_SET_CPU0_SET_LEN      1
#define PMU_GLB_PWROFF_FRC_SW_SET_CPU0_SET_MASK     0x00000010
#define PMU_GLB_PWROFF_FRC_SW_SET_CPU0_SET_VAL(w)   (((w) & 0x00000010) >> 4)
#define PMU_GLB_PWROFF_FRC_SW_SET_CPU0_SET_SET(w)   w |= ( 0x1 << 4)
#define PMU_GLB_PWROFF_FRC_SW_SET_CPU0_SET_CLR(w)   w &= ~( 0x1 << 4)

#define PMU_GLB_PWROFF_FRC_SW_SET_DSP_SET_POS      3
#define PMU_GLB_PWROFF_FRC_SW_SET_DSP_SET_LEN      1
#define PMU_GLB_PWROFF_FRC_SW_SET_DSP_SET_MASK     0x00000008
#define PMU_GLB_PWROFF_FRC_SW_SET_DSP_SET_VAL(w)   (((w) & 0x00000008) >> 3)
#define PMU_GLB_PWROFF_FRC_SW_SET_DSP_SET_SET(w)   w |= ( 0x1 << 3)
#define PMU_GLB_PWROFF_FRC_SW_SET_DSP_SET_CLR(w)   w &= ~( 0x1 << 3)

#define PMU_GLB_PWROFF_FRC_SW_SET_VDEC_SET_POS      2
#define PMU_GLB_PWROFF_FRC_SW_SET_VDEC_SET_LEN      1
#define PMU_GLB_PWROFF_FRC_SW_SET_VDEC_SET_MASK     0x00000004
#define PMU_GLB_PWROFF_FRC_SW_SET_VDEC_SET_VAL(w)   (((w) & 0x00000004) >> 2)
#define PMU_GLB_PWROFF_FRC_SW_SET_VDEC_SET_SET(w)   w |= ( 0x1 << 2)
#define PMU_GLB_PWROFF_FRC_SW_SET_VDEC_SET_CLR(w)   w &= ~( 0x1 << 2)

#define PMU_GLB_PWROFF_FRC_SW_SET_VENC1_SET_POS      1
#define PMU_GLB_PWROFF_FRC_SW_SET_VENC1_SET_LEN      1
#define PMU_GLB_PWROFF_FRC_SW_SET_VENC1_SET_MASK     0x00000002
#define PMU_GLB_PWROFF_FRC_SW_SET_VENC1_SET_VAL(w)   (((w) & 0x00000002) >> 1)
#define PMU_GLB_PWROFF_FRC_SW_SET_VENC1_SET_SET(w)   w |= ( 0x1 << 1)
#define PMU_GLB_PWROFF_FRC_SW_SET_VENC1_SET_CLR(w)   w &= ~( 0x1 << 1)

#define PMU_GLB_PWROFF_FRC_SW_SET_VENC0_SET_POS      0
#define PMU_GLB_PWROFF_FRC_SW_SET_VENC0_SET_LEN      1
#define PMU_GLB_PWROFF_FRC_SW_SET_VENC0_SET_MASK     0x00000001
#define PMU_GLB_PWROFF_FRC_SW_SET_VENC0_SET_VAL(w)   (((w) & 0x00000001) >> 0)
#define PMU_GLB_PWROFF_FRC_SW_SET_VENC0_SET_SET(w)   w |= ( 0x1 << 0)
#define PMU_GLB_PWROFF_FRC_SW_SET_VENC0_SET_CLR(w)   w &= ~( 0x1 << 0)

// REGISTER : PWROFF_FRC_SW_CLR

#define PMU_GLB_PWROFF_FRC_SW_CLR_ADDR      PMU_GLB_BASE_ADDR + 0x1B0
#define PMU_GLB_PWROFF_FRC_SW_CLR_RSTVAL    0x0
#define PMU_GLB_PWROFF_FRC_SW_CLR_ALL1      0x0
#define PMU_GLB_PWROFF_FRC_SW_CLR_ALL0      0x0
#define PMU_GLB_PWROFF_FRC_SW_CLR_ALL5A     0x0
#define PMU_GLB_PWROFF_FRC_SW_CLR_ALLA5     0x0

// FIELD IN REGISTER : PWROFF_FRC_SW_CLR

#define PMU_GLB_PWROFF_FRC_SW_CLR_ISP_CLR_POS      14
#define PMU_GLB_PWROFF_FRC_SW_CLR_ISP_CLR_LEN      1
#define PMU_GLB_PWROFF_FRC_SW_CLR_ISP_CLR_MASK     0x00004000
#define PMU_GLB_PWROFF_FRC_SW_CLR_ISP_CLR_VAL(w)   (((w) & 0x00004000) >> 14)
#define PMU_GLB_PWROFF_FRC_SW_CLR_ISP_CLR_SET(w)   w |= ( 0x1 << 14)
#define PMU_GLB_PWROFF_FRC_SW_CLR_ISP_CLR_CLR(w)   w &= ~( 0x1 << 14)

#define PMU_GLB_PWROFF_FRC_SW_CLR_CPU_CLR_POS      13
#define PMU_GLB_PWROFF_FRC_SW_CLR_CPU_CLR_LEN      1
#define PMU_GLB_PWROFF_FRC_SW_CLR_CPU_CLR_MASK     0x00002000
#define PMU_GLB_PWROFF_FRC_SW_CLR_CPU_CLR_VAL(w)   (((w) & 0x00002000) >> 13)
#define PMU_GLB_PWROFF_FRC_SW_CLR_CPU_CLR_SET(w)   w |= ( 0x1 << 13)
#define PMU_GLB_PWROFF_FRC_SW_CLR_CPU_CLR_CLR(w)   w &= ~( 0x1 << 13)

#define PMU_GLB_PWROFF_FRC_SW_CLR_NPU_CLR_POS      12
#define PMU_GLB_PWROFF_FRC_SW_CLR_NPU_CLR_LEN      1
#define PMU_GLB_PWROFF_FRC_SW_CLR_NPU_CLR_MASK     0x00001000
#define PMU_GLB_PWROFF_FRC_SW_CLR_NPU_CLR_VAL(w)   (((w) & 0x00001000) >> 12)
#define PMU_GLB_PWROFF_FRC_SW_CLR_NPU_CLR_SET(w)   w |= ( 0x1 << 12)
#define PMU_GLB_PWROFF_FRC_SW_CLR_NPU_CLR_CLR(w)   w &= ~( 0x1 << 12)

#define PMU_GLB_PWROFF_FRC_SW_CLR_CPU7_CLR_POS      11
#define PMU_GLB_PWROFF_FRC_SW_CLR_CPU7_CLR_LEN      1
#define PMU_GLB_PWROFF_FRC_SW_CLR_CPU7_CLR_MASK     0x00000800
#define PMU_GLB_PWROFF_FRC_SW_CLR_CPU7_CLR_VAL(w)   (((w) & 0x00000800) >> 11)
#define PMU_GLB_PWROFF_FRC_SW_CLR_CPU7_CLR_SET(w)   w |= ( 0x1 << 11)
#define PMU_GLB_PWROFF_FRC_SW_CLR_CPU7_CLR_CLR(w)   w &= ~( 0x1 << 11)

#define PMU_GLB_PWROFF_FRC_SW_CLR_CPU6_CLR_POS      10
#define PMU_GLB_PWROFF_FRC_SW_CLR_CPU6_CLR_LEN      1
#define PMU_GLB_PWROFF_FRC_SW_CLR_CPU6_CLR_MASK     0x00000400
#define PMU_GLB_PWROFF_FRC_SW_CLR_CPU6_CLR_VAL(w)   (((w) & 0x00000400) >> 10)
#define PMU_GLB_PWROFF_FRC_SW_CLR_CPU6_CLR_SET(w)   w |= ( 0x1 << 10)
#define PMU_GLB_PWROFF_FRC_SW_CLR_CPU6_CLR_CLR(w)   w &= ~( 0x1 << 10)

#define PMU_GLB_PWROFF_FRC_SW_CLR_CPU5_CLR_POS      9
#define PMU_GLB_PWROFF_FRC_SW_CLR_CPU5_CLR_LEN      1
#define PMU_GLB_PWROFF_FRC_SW_CLR_CPU5_CLR_MASK     0x00000200
#define PMU_GLB_PWROFF_FRC_SW_CLR_CPU5_CLR_VAL(w)   (((w) & 0x00000200) >> 9)
#define PMU_GLB_PWROFF_FRC_SW_CLR_CPU5_CLR_SET(w)   w |= ( 0x1 << 9)
#define PMU_GLB_PWROFF_FRC_SW_CLR_CPU5_CLR_CLR(w)   w &= ~( 0x1 << 9)

#define PMU_GLB_PWROFF_FRC_SW_CLR_CPU4_CLR_POS      8
#define PMU_GLB_PWROFF_FRC_SW_CLR_CPU4_CLR_LEN      1
#define PMU_GLB_PWROFF_FRC_SW_CLR_CPU4_CLR_MASK     0x00000100
#define PMU_GLB_PWROFF_FRC_SW_CLR_CPU4_CLR_VAL(w)   (((w) & 0x00000100) >> 8)
#define PMU_GLB_PWROFF_FRC_SW_CLR_CPU4_CLR_SET(w)   w |= ( 0x1 << 8)
#define PMU_GLB_PWROFF_FRC_SW_CLR_CPU4_CLR_CLR(w)   w &= ~( 0x1 << 8)

#define PMU_GLB_PWROFF_FRC_SW_CLR_CPU3_CLR_POS      7
#define PMU_GLB_PWROFF_FRC_SW_CLR_CPU3_CLR_LEN      1
#define PMU_GLB_PWROFF_FRC_SW_CLR_CPU3_CLR_MASK     0x00000080
#define PMU_GLB_PWROFF_FRC_SW_CLR_CPU3_CLR_VAL(w)   (((w) & 0x00000080) >> 7)
#define PMU_GLB_PWROFF_FRC_SW_CLR_CPU3_CLR_SET(w)   w |= ( 0x1 << 7)
#define PMU_GLB_PWROFF_FRC_SW_CLR_CPU3_CLR_CLR(w)   w &= ~( 0x1 << 7)

#define PMU_GLB_PWROFF_FRC_SW_CLR_CPU2_CLR_POS      6
#define PMU_GLB_PWROFF_FRC_SW_CLR_CPU2_CLR_LEN      1
#define PMU_GLB_PWROFF_FRC_SW_CLR_CPU2_CLR_MASK     0x00000040
#define PMU_GLB_PWROFF_FRC_SW_CLR_CPU2_CLR_VAL(w)   (((w) & 0x00000040) >> 6)
#define PMU_GLB_PWROFF_FRC_SW_CLR_CPU2_CLR_SET(w)   w |= ( 0x1 << 6)
#define PMU_GLB_PWROFF_FRC_SW_CLR_CPU2_CLR_CLR(w)   w &= ~( 0x1 << 6)

#define PMU_GLB_PWROFF_FRC_SW_CLR_CPU1_CLR_POS      5
#define PMU_GLB_PWROFF_FRC_SW_CLR_CPU1_CLR_LEN      1
#define PMU_GLB_PWROFF_FRC_SW_CLR_CPU1_CLR_MASK     0x00000020
#define PMU_GLB_PWROFF_FRC_SW_CLR_CPU1_CLR_VAL(w)   (((w) & 0x00000020) >> 5)
#define PMU_GLB_PWROFF_FRC_SW_CLR_CPU1_CLR_SET(w)   w |= ( 0x1 << 5)
#define PMU_GLB_PWROFF_FRC_SW_CLR_CPU1_CLR_CLR(w)   w &= ~( 0x1 << 5)

#define PMU_GLB_PWROFF_FRC_SW_CLR_CPU0_CLR_POS      4
#define PMU_GLB_PWROFF_FRC_SW_CLR_CPU0_CLR_LEN      1
#define PMU_GLB_PWROFF_FRC_SW_CLR_CPU0_CLR_MASK     0x00000010
#define PMU_GLB_PWROFF_FRC_SW_CLR_CPU0_CLR_VAL(w)   (((w) & 0x00000010) >> 4)
#define PMU_GLB_PWROFF_FRC_SW_CLR_CPU0_CLR_SET(w)   w |= ( 0x1 << 4)
#define PMU_GLB_PWROFF_FRC_SW_CLR_CPU0_CLR_CLR(w)   w &= ~( 0x1 << 4)

#define PMU_GLB_PWROFF_FRC_SW_CLR_DSP_CLR_POS      3
#define PMU_GLB_PWROFF_FRC_SW_CLR_DSP_CLR_LEN      1
#define PMU_GLB_PWROFF_FRC_SW_CLR_DSP_CLR_MASK     0x00000008
#define PMU_GLB_PWROFF_FRC_SW_CLR_DSP_CLR_VAL(w)   (((w) & 0x00000008) >> 3)
#define PMU_GLB_PWROFF_FRC_SW_CLR_DSP_CLR_SET(w)   w |= ( 0x1 << 3)
#define PMU_GLB_PWROFF_FRC_SW_CLR_DSP_CLR_CLR(w)   w &= ~( 0x1 << 3)

#define PMU_GLB_PWROFF_FRC_SW_CLR_VDEC_CLR_POS      2
#define PMU_GLB_PWROFF_FRC_SW_CLR_VDEC_CLR_LEN      1
#define PMU_GLB_PWROFF_FRC_SW_CLR_VDEC_CLR_MASK     0x00000004
#define PMU_GLB_PWROFF_FRC_SW_CLR_VDEC_CLR_VAL(w)   (((w) & 0x00000004) >> 2)
#define PMU_GLB_PWROFF_FRC_SW_CLR_VDEC_CLR_SET(w)   w |= ( 0x1 << 2)
#define PMU_GLB_PWROFF_FRC_SW_CLR_VDEC_CLR_CLR(w)   w &= ~( 0x1 << 2)

#define PMU_GLB_PWROFF_FRC_SW_CLR_VENC1_CLR_POS      1
#define PMU_GLB_PWROFF_FRC_SW_CLR_VENC1_CLR_LEN      1
#define PMU_GLB_PWROFF_FRC_SW_CLR_VENC1_CLR_MASK     0x00000002
#define PMU_GLB_PWROFF_FRC_SW_CLR_VENC1_CLR_VAL(w)   (((w) & 0x00000002) >> 1)
#define PMU_GLB_PWROFF_FRC_SW_CLR_VENC1_CLR_SET(w)   w |= ( 0x1 << 1)
#define PMU_GLB_PWROFF_FRC_SW_CLR_VENC1_CLR_CLR(w)   w &= ~( 0x1 << 1)

#define PMU_GLB_PWROFF_FRC_SW_CLR_VENC0_CLR_POS      0
#define PMU_GLB_PWROFF_FRC_SW_CLR_VENC0_CLR_LEN      1
#define PMU_GLB_PWROFF_FRC_SW_CLR_VENC0_CLR_MASK     0x00000001
#define PMU_GLB_PWROFF_FRC_SW_CLR_VENC0_CLR_VAL(w)   (((w) & 0x00000001) >> 0)
#define PMU_GLB_PWROFF_FRC_SW_CLR_VENC0_CLR_SET(w)   w |= ( 0x1 << 0)
#define PMU_GLB_PWROFF_FRC_SW_CLR_VENC0_CLR_CLR(w)   w &= ~( 0x1 << 0)

// REGISTER : PWROFF_FRC_SW1_SET

#define PMU_GLB_PWROFF_FRC_SW1_SET_ADDR      PMU_GLB_BASE_ADDR + 0x1B4
#define PMU_GLB_PWROFF_FRC_SW1_SET_RSTVAL    0x0
#define PMU_GLB_PWROFF_FRC_SW1_SET_ALL1      0x0
#define PMU_GLB_PWROFF_FRC_SW1_SET_ALL0      0x0
#define PMU_GLB_PWROFF_FRC_SW1_SET_ALL5A     0x0
#define PMU_GLB_PWROFF_FRC_SW1_SET_ALLA5     0x0

// FIELD IN REGISTER : PWROFF_FRC_SW1_SET

#define PMU_GLB_PWROFF_FRC_SW1_SET_N8_SET_POS      10
#define PMU_GLB_PWROFF_FRC_SW1_SET_N8_SET_LEN      1
#define PMU_GLB_PWROFF_FRC_SW1_SET_N8_SET_MASK     0x00000400
#define PMU_GLB_PWROFF_FRC_SW1_SET_N8_SET_VAL(w)   (((w) & 0x00000400) >> 10)
#define PMU_GLB_PWROFF_FRC_SW1_SET_N8_SET_SET(w)   w |= ( 0x1 << 10)
#define PMU_GLB_PWROFF_FRC_SW1_SET_N8_SET_CLR(w)   w &= ~( 0x1 << 10)

#define PMU_GLB_PWROFF_FRC_SW1_SET_N7_SET_POS      9
#define PMU_GLB_PWROFF_FRC_SW1_SET_N7_SET_LEN      1
#define PMU_GLB_PWROFF_FRC_SW1_SET_N7_SET_MASK     0x00000200
#define PMU_GLB_PWROFF_FRC_SW1_SET_N7_SET_VAL(w)   (((w) & 0x00000200) >> 9)
#define PMU_GLB_PWROFF_FRC_SW1_SET_N7_SET_SET(w)   w |= ( 0x1 << 9)
#define PMU_GLB_PWROFF_FRC_SW1_SET_N7_SET_CLR(w)   w &= ~( 0x1 << 9)

#define PMU_GLB_PWROFF_FRC_SW1_SET_N6_SET_POS      8
#define PMU_GLB_PWROFF_FRC_SW1_SET_N6_SET_LEN      1
#define PMU_GLB_PWROFF_FRC_SW1_SET_N6_SET_MASK     0x00000100
#define PMU_GLB_PWROFF_FRC_SW1_SET_N6_SET_VAL(w)   (((w) & 0x00000100) >> 8)
#define PMU_GLB_PWROFF_FRC_SW1_SET_N6_SET_SET(w)   w |= ( 0x1 << 8)
#define PMU_GLB_PWROFF_FRC_SW1_SET_N6_SET_CLR(w)   w &= ~( 0x1 << 8)

#define PMU_GLB_PWROFF_FRC_SW1_SET_N5_SET_POS      7
#define PMU_GLB_PWROFF_FRC_SW1_SET_N5_SET_LEN      1
#define PMU_GLB_PWROFF_FRC_SW1_SET_N5_SET_MASK     0x00000080
#define PMU_GLB_PWROFF_FRC_SW1_SET_N5_SET_VAL(w)   (((w) & 0x00000080) >> 7)
#define PMU_GLB_PWROFF_FRC_SW1_SET_N5_SET_SET(w)   w |= ( 0x1 << 7)
#define PMU_GLB_PWROFF_FRC_SW1_SET_N5_SET_CLR(w)   w &= ~( 0x1 << 7)

#define PMU_GLB_PWROFF_FRC_SW1_SET_N4_SET_POS      6
#define PMU_GLB_PWROFF_FRC_SW1_SET_N4_SET_LEN      1
#define PMU_GLB_PWROFF_FRC_SW1_SET_N4_SET_MASK     0x00000040
#define PMU_GLB_PWROFF_FRC_SW1_SET_N4_SET_VAL(w)   (((w) & 0x00000040) >> 6)
#define PMU_GLB_PWROFF_FRC_SW1_SET_N4_SET_SET(w)   w |= ( 0x1 << 6)
#define PMU_GLB_PWROFF_FRC_SW1_SET_N4_SET_CLR(w)   w &= ~( 0x1 << 6)

#define PMU_GLB_PWROFF_FRC_SW1_SET_N3_SET_POS      5
#define PMU_GLB_PWROFF_FRC_SW1_SET_N3_SET_LEN      1
#define PMU_GLB_PWROFF_FRC_SW1_SET_N3_SET_MASK     0x00000020
#define PMU_GLB_PWROFF_FRC_SW1_SET_N3_SET_VAL(w)   (((w) & 0x00000020) >> 5)
#define PMU_GLB_PWROFF_FRC_SW1_SET_N3_SET_SET(w)   w |= ( 0x1 << 5)
#define PMU_GLB_PWROFF_FRC_SW1_SET_N3_SET_CLR(w)   w &= ~( 0x1 << 5)

#define PMU_GLB_PWROFF_FRC_SW1_SET_N2_SET_POS      4
#define PMU_GLB_PWROFF_FRC_SW1_SET_N2_SET_LEN      1
#define PMU_GLB_PWROFF_FRC_SW1_SET_N2_SET_MASK     0x00000010
#define PMU_GLB_PWROFF_FRC_SW1_SET_N2_SET_VAL(w)   (((w) & 0x00000010) >> 4)
#define PMU_GLB_PWROFF_FRC_SW1_SET_N2_SET_SET(w)   w |= ( 0x1 << 4)
#define PMU_GLB_PWROFF_FRC_SW1_SET_N2_SET_CLR(w)   w &= ~( 0x1 << 4)

#define PMU_GLB_PWROFF_FRC_SW1_SET_N1_SET_POS      3
#define PMU_GLB_PWROFF_FRC_SW1_SET_N1_SET_LEN      1
#define PMU_GLB_PWROFF_FRC_SW1_SET_N1_SET_MASK     0x00000008
#define PMU_GLB_PWROFF_FRC_SW1_SET_N1_SET_VAL(w)   (((w) & 0x00000008) >> 3)
#define PMU_GLB_PWROFF_FRC_SW1_SET_N1_SET_SET(w)   w |= ( 0x1 << 3)
#define PMU_GLB_PWROFF_FRC_SW1_SET_N1_SET_CLR(w)   w &= ~( 0x1 << 3)

#define PMU_GLB_PWROFF_FRC_SW1_SET_N0_SET_POS      2
#define PMU_GLB_PWROFF_FRC_SW1_SET_N0_SET_LEN      1
#define PMU_GLB_PWROFF_FRC_SW1_SET_N0_SET_MASK     0x00000004
#define PMU_GLB_PWROFF_FRC_SW1_SET_N0_SET_VAL(w)   (((w) & 0x00000004) >> 2)
#define PMU_GLB_PWROFF_FRC_SW1_SET_N0_SET_SET(w)   w |= ( 0x1 << 2)
#define PMU_GLB_PWROFF_FRC_SW1_SET_N0_SET_CLR(w)   w &= ~( 0x1 << 2)

#define PMU_GLB_PWROFF_FRC_SW1_SET_MM_SET_POS      1
#define PMU_GLB_PWROFF_FRC_SW1_SET_MM_SET_LEN      1
#define PMU_GLB_PWROFF_FRC_SW1_SET_MM_SET_MASK     0x00000002
#define PMU_GLB_PWROFF_FRC_SW1_SET_MM_SET_VAL(w)   (((w) & 0x00000002) >> 1)
#define PMU_GLB_PWROFF_FRC_SW1_SET_MM_SET_SET(w)   w |= ( 0x1 << 1)
#define PMU_GLB_PWROFF_FRC_SW1_SET_MM_SET_CLR(w)   w &= ~( 0x1 << 1)

#define PMU_GLB_PWROFF_FRC_SW1_SET_PIPE_SET_POS      0
#define PMU_GLB_PWROFF_FRC_SW1_SET_PIPE_SET_LEN      1
#define PMU_GLB_PWROFF_FRC_SW1_SET_PIPE_SET_MASK     0x00000001
#define PMU_GLB_PWROFF_FRC_SW1_SET_PIPE_SET_VAL(w)   (((w) & 0x00000001) >> 0)
#define PMU_GLB_PWROFF_FRC_SW1_SET_PIPE_SET_SET(w)   w |= ( 0x1 << 0)
#define PMU_GLB_PWROFF_FRC_SW1_SET_PIPE_SET_CLR(w)   w &= ~( 0x1 << 0)

// REGISTER : PWROFF_FRC_SW1_CLR

#define PMU_GLB_PWROFF_FRC_SW1_CLR_ADDR      PMU_GLB_BASE_ADDR + 0x1B8
#define PMU_GLB_PWROFF_FRC_SW1_CLR_RSTVAL    0x0
#define PMU_GLB_PWROFF_FRC_SW1_CLR_ALL1      0x0
#define PMU_GLB_PWROFF_FRC_SW1_CLR_ALL0      0x0
#define PMU_GLB_PWROFF_FRC_SW1_CLR_ALL5A     0x0
#define PMU_GLB_PWROFF_FRC_SW1_CLR_ALLA5     0x0

// FIELD IN REGISTER : PWROFF_FRC_SW1_CLR

#define PMU_GLB_PWROFF_FRC_SW1_CLR_N8_CLR_POS      10
#define PMU_GLB_PWROFF_FRC_SW1_CLR_N8_CLR_LEN      1
#define PMU_GLB_PWROFF_FRC_SW1_CLR_N8_CLR_MASK     0x00000400
#define PMU_GLB_PWROFF_FRC_SW1_CLR_N8_CLR_VAL(w)   (((w) & 0x00000400) >> 10)
#define PMU_GLB_PWROFF_FRC_SW1_CLR_N8_CLR_SET(w)   w |= ( 0x1 << 10)
#define PMU_GLB_PWROFF_FRC_SW1_CLR_N8_CLR_CLR(w)   w &= ~( 0x1 << 10)

#define PMU_GLB_PWROFF_FRC_SW1_CLR_N7_CLR_POS      9
#define PMU_GLB_PWROFF_FRC_SW1_CLR_N7_CLR_LEN      1
#define PMU_GLB_PWROFF_FRC_SW1_CLR_N7_CLR_MASK     0x00000200
#define PMU_GLB_PWROFF_FRC_SW1_CLR_N7_CLR_VAL(w)   (((w) & 0x00000200) >> 9)
#define PMU_GLB_PWROFF_FRC_SW1_CLR_N7_CLR_SET(w)   w |= ( 0x1 << 9)
#define PMU_GLB_PWROFF_FRC_SW1_CLR_N7_CLR_CLR(w)   w &= ~( 0x1 << 9)

#define PMU_GLB_PWROFF_FRC_SW1_CLR_N6_CLR_POS      8
#define PMU_GLB_PWROFF_FRC_SW1_CLR_N6_CLR_LEN      1
#define PMU_GLB_PWROFF_FRC_SW1_CLR_N6_CLR_MASK     0x00000100
#define PMU_GLB_PWROFF_FRC_SW1_CLR_N6_CLR_VAL(w)   (((w) & 0x00000100) >> 8)
#define PMU_GLB_PWROFF_FRC_SW1_CLR_N6_CLR_SET(w)   w |= ( 0x1 << 8)
#define PMU_GLB_PWROFF_FRC_SW1_CLR_N6_CLR_CLR(w)   w &= ~( 0x1 << 8)

#define PMU_GLB_PWROFF_FRC_SW1_CLR_N5_CLR_POS      7
#define PMU_GLB_PWROFF_FRC_SW1_CLR_N5_CLR_LEN      1
#define PMU_GLB_PWROFF_FRC_SW1_CLR_N5_CLR_MASK     0x00000080
#define PMU_GLB_PWROFF_FRC_SW1_CLR_N5_CLR_VAL(w)   (((w) & 0x00000080) >> 7)
#define PMU_GLB_PWROFF_FRC_SW1_CLR_N5_CLR_SET(w)   w |= ( 0x1 << 7)
#define PMU_GLB_PWROFF_FRC_SW1_CLR_N5_CLR_CLR(w)   w &= ~( 0x1 << 7)

#define PMU_GLB_PWROFF_FRC_SW1_CLR_N4_CLR_POS      6
#define PMU_GLB_PWROFF_FRC_SW1_CLR_N4_CLR_LEN      1
#define PMU_GLB_PWROFF_FRC_SW1_CLR_N4_CLR_MASK     0x00000040
#define PMU_GLB_PWROFF_FRC_SW1_CLR_N4_CLR_VAL(w)   (((w) & 0x00000040) >> 6)
#define PMU_GLB_PWROFF_FRC_SW1_CLR_N4_CLR_SET(w)   w |= ( 0x1 << 6)
#define PMU_GLB_PWROFF_FRC_SW1_CLR_N4_CLR_CLR(w)   w &= ~( 0x1 << 6)

#define PMU_GLB_PWROFF_FRC_SW1_CLR_N3_CLR_POS      5
#define PMU_GLB_PWROFF_FRC_SW1_CLR_N3_CLR_LEN      1
#define PMU_GLB_PWROFF_FRC_SW1_CLR_N3_CLR_MASK     0x00000020
#define PMU_GLB_PWROFF_FRC_SW1_CLR_N3_CLR_VAL(w)   (((w) & 0x00000020) >> 5)
#define PMU_GLB_PWROFF_FRC_SW1_CLR_N3_CLR_SET(w)   w |= ( 0x1 << 5)
#define PMU_GLB_PWROFF_FRC_SW1_CLR_N3_CLR_CLR(w)   w &= ~( 0x1 << 5)

#define PMU_GLB_PWROFF_FRC_SW1_CLR_N2_CLR_POS      4
#define PMU_GLB_PWROFF_FRC_SW1_CLR_N2_CLR_LEN      1
#define PMU_GLB_PWROFF_FRC_SW1_CLR_N2_CLR_MASK     0x00000010
#define PMU_GLB_PWROFF_FRC_SW1_CLR_N2_CLR_VAL(w)   (((w) & 0x00000010) >> 4)
#define PMU_GLB_PWROFF_FRC_SW1_CLR_N2_CLR_SET(w)   w |= ( 0x1 << 4)
#define PMU_GLB_PWROFF_FRC_SW1_CLR_N2_CLR_CLR(w)   w &= ~( 0x1 << 4)

#define PMU_GLB_PWROFF_FRC_SW1_CLR_N1_CLR_POS      3
#define PMU_GLB_PWROFF_FRC_SW1_CLR_N1_CLR_LEN      1
#define PMU_GLB_PWROFF_FRC_SW1_CLR_N1_CLR_MASK     0x00000008
#define PMU_GLB_PWROFF_FRC_SW1_CLR_N1_CLR_VAL(w)   (((w) & 0x00000008) >> 3)
#define PMU_GLB_PWROFF_FRC_SW1_CLR_N1_CLR_SET(w)   w |= ( 0x1 << 3)
#define PMU_GLB_PWROFF_FRC_SW1_CLR_N1_CLR_CLR(w)   w &= ~( 0x1 << 3)

#define PMU_GLB_PWROFF_FRC_SW1_CLR_N0_CLR_POS      2
#define PMU_GLB_PWROFF_FRC_SW1_CLR_N0_CLR_LEN      1
#define PMU_GLB_PWROFF_FRC_SW1_CLR_N0_CLR_MASK     0x00000004
#define PMU_GLB_PWROFF_FRC_SW1_CLR_N0_CLR_VAL(w)   (((w) & 0x00000004) >> 2)
#define PMU_GLB_PWROFF_FRC_SW1_CLR_N0_CLR_SET(w)   w |= ( 0x1 << 2)
#define PMU_GLB_PWROFF_FRC_SW1_CLR_N0_CLR_CLR(w)   w &= ~( 0x1 << 2)

#define PMU_GLB_PWROFF_FRC_SW1_CLR_MM_CLR_POS      1
#define PMU_GLB_PWROFF_FRC_SW1_CLR_MM_CLR_LEN      1
#define PMU_GLB_PWROFF_FRC_SW1_CLR_MM_CLR_MASK     0x00000002
#define PMU_GLB_PWROFF_FRC_SW1_CLR_MM_CLR_VAL(w)   (((w) & 0x00000002) >> 1)
#define PMU_GLB_PWROFF_FRC_SW1_CLR_MM_CLR_SET(w)   w |= ( 0x1 << 1)
#define PMU_GLB_PWROFF_FRC_SW1_CLR_MM_CLR_CLR(w)   w &= ~( 0x1 << 1)

#define PMU_GLB_PWROFF_FRC_SW1_CLR_PIPE_CLR_POS      0
#define PMU_GLB_PWROFF_FRC_SW1_CLR_PIPE_CLR_LEN      1
#define PMU_GLB_PWROFF_FRC_SW1_CLR_PIPE_CLR_MASK     0x00000001
#define PMU_GLB_PWROFF_FRC_SW1_CLR_PIPE_CLR_VAL(w)   (((w) & 0x00000001) >> 0)
#define PMU_GLB_PWROFF_FRC_SW1_CLR_PIPE_CLR_SET(w)   w |= ( 0x1 << 0)
#define PMU_GLB_PWROFF_FRC_SW1_CLR_PIPE_CLR_CLR(w)   w &= ~( 0x1 << 0)

// REGISTER : ISO_FRC_EN_SET

#define PMU_GLB_ISO_FRC_EN_SET_ADDR      PMU_GLB_BASE_ADDR + 0x1BC
#define PMU_GLB_ISO_FRC_EN_SET_RSTVAL    0x0
#define PMU_GLB_ISO_FRC_EN_SET_ALL1      0x0
#define PMU_GLB_ISO_FRC_EN_SET_ALL0      0x0
#define PMU_GLB_ISO_FRC_EN_SET_ALL5A     0x0
#define PMU_GLB_ISO_FRC_EN_SET_ALLA5     0x0

// FIELD IN REGISTER : ISO_FRC_EN_SET

#define PMU_GLB_ISO_FRC_EN_SET_ISP_SET_POS      14
#define PMU_GLB_ISO_FRC_EN_SET_ISP_SET_LEN      1
#define PMU_GLB_ISO_FRC_EN_SET_ISP_SET_MASK     0x00004000
#define PMU_GLB_ISO_FRC_EN_SET_ISP_SET_VAL(w)   (((w) & 0x00004000) >> 14)
#define PMU_GLB_ISO_FRC_EN_SET_ISP_SET_SET(w)   w |= ( 0x1 << 14)
#define PMU_GLB_ISO_FRC_EN_SET_ISP_SET_CLR(w)   w &= ~( 0x1 << 14)

#define PMU_GLB_ISO_FRC_EN_SET_CPU_SET_POS      13
#define PMU_GLB_ISO_FRC_EN_SET_CPU_SET_LEN      1
#define PMU_GLB_ISO_FRC_EN_SET_CPU_SET_MASK     0x00002000
#define PMU_GLB_ISO_FRC_EN_SET_CPU_SET_VAL(w)   (((w) & 0x00002000) >> 13)
#define PMU_GLB_ISO_FRC_EN_SET_CPU_SET_SET(w)   w |= ( 0x1 << 13)
#define PMU_GLB_ISO_FRC_EN_SET_CPU_SET_CLR(w)   w &= ~( 0x1 << 13)

#define PMU_GLB_ISO_FRC_EN_SET_NPU_SET_POS      12
#define PMU_GLB_ISO_FRC_EN_SET_NPU_SET_LEN      1
#define PMU_GLB_ISO_FRC_EN_SET_NPU_SET_MASK     0x00001000
#define PMU_GLB_ISO_FRC_EN_SET_NPU_SET_VAL(w)   (((w) & 0x00001000) >> 12)
#define PMU_GLB_ISO_FRC_EN_SET_NPU_SET_SET(w)   w |= ( 0x1 << 12)
#define PMU_GLB_ISO_FRC_EN_SET_NPU_SET_CLR(w)   w &= ~( 0x1 << 12)

#define PMU_GLB_ISO_FRC_EN_SET_CPU7_SET_POS      11
#define PMU_GLB_ISO_FRC_EN_SET_CPU7_SET_LEN      1
#define PMU_GLB_ISO_FRC_EN_SET_CPU7_SET_MASK     0x00000800
#define PMU_GLB_ISO_FRC_EN_SET_CPU7_SET_VAL(w)   (((w) & 0x00000800) >> 11)
#define PMU_GLB_ISO_FRC_EN_SET_CPU7_SET_SET(w)   w |= ( 0x1 << 11)
#define PMU_GLB_ISO_FRC_EN_SET_CPU7_SET_CLR(w)   w &= ~( 0x1 << 11)

#define PMU_GLB_ISO_FRC_EN_SET_CPU6_SET_POS      10
#define PMU_GLB_ISO_FRC_EN_SET_CPU6_SET_LEN      1
#define PMU_GLB_ISO_FRC_EN_SET_CPU6_SET_MASK     0x00000400
#define PMU_GLB_ISO_FRC_EN_SET_CPU6_SET_VAL(w)   (((w) & 0x00000400) >> 10)
#define PMU_GLB_ISO_FRC_EN_SET_CPU6_SET_SET(w)   w |= ( 0x1 << 10)
#define PMU_GLB_ISO_FRC_EN_SET_CPU6_SET_CLR(w)   w &= ~( 0x1 << 10)

#define PMU_GLB_ISO_FRC_EN_SET_CPU5_SET_POS      9
#define PMU_GLB_ISO_FRC_EN_SET_CPU5_SET_LEN      1
#define PMU_GLB_ISO_FRC_EN_SET_CPU5_SET_MASK     0x00000200
#define PMU_GLB_ISO_FRC_EN_SET_CPU5_SET_VAL(w)   (((w) & 0x00000200) >> 9)
#define PMU_GLB_ISO_FRC_EN_SET_CPU5_SET_SET(w)   w |= ( 0x1 << 9)
#define PMU_GLB_ISO_FRC_EN_SET_CPU5_SET_CLR(w)   w &= ~( 0x1 << 9)

#define PMU_GLB_ISO_FRC_EN_SET_CPU4_SET_POS      8
#define PMU_GLB_ISO_FRC_EN_SET_CPU4_SET_LEN      1
#define PMU_GLB_ISO_FRC_EN_SET_CPU4_SET_MASK     0x00000100
#define PMU_GLB_ISO_FRC_EN_SET_CPU4_SET_VAL(w)   (((w) & 0x00000100) >> 8)
#define PMU_GLB_ISO_FRC_EN_SET_CPU4_SET_SET(w)   w |= ( 0x1 << 8)
#define PMU_GLB_ISO_FRC_EN_SET_CPU4_SET_CLR(w)   w &= ~( 0x1 << 8)

#define PMU_GLB_ISO_FRC_EN_SET_CPU3_SET_POS      7
#define PMU_GLB_ISO_FRC_EN_SET_CPU3_SET_LEN      1
#define PMU_GLB_ISO_FRC_EN_SET_CPU3_SET_MASK     0x00000080
#define PMU_GLB_ISO_FRC_EN_SET_CPU3_SET_VAL(w)   (((w) & 0x00000080) >> 7)
#define PMU_GLB_ISO_FRC_EN_SET_CPU3_SET_SET(w)   w |= ( 0x1 << 7)
#define PMU_GLB_ISO_FRC_EN_SET_CPU3_SET_CLR(w)   w &= ~( 0x1 << 7)

#define PMU_GLB_ISO_FRC_EN_SET_CPU2_SET_POS      6
#define PMU_GLB_ISO_FRC_EN_SET_CPU2_SET_LEN      1
#define PMU_GLB_ISO_FRC_EN_SET_CPU2_SET_MASK     0x00000040
#define PMU_GLB_ISO_FRC_EN_SET_CPU2_SET_VAL(w)   (((w) & 0x00000040) >> 6)
#define PMU_GLB_ISO_FRC_EN_SET_CPU2_SET_SET(w)   w |= ( 0x1 << 6)
#define PMU_GLB_ISO_FRC_EN_SET_CPU2_SET_CLR(w)   w &= ~( 0x1 << 6)

#define PMU_GLB_ISO_FRC_EN_SET_CPU1_SET_POS      5
#define PMU_GLB_ISO_FRC_EN_SET_CPU1_SET_LEN      1
#define PMU_GLB_ISO_FRC_EN_SET_CPU1_SET_MASK     0x00000020
#define PMU_GLB_ISO_FRC_EN_SET_CPU1_SET_VAL(w)   (((w) & 0x00000020) >> 5)
#define PMU_GLB_ISO_FRC_EN_SET_CPU1_SET_SET(w)   w |= ( 0x1 << 5)
#define PMU_GLB_ISO_FRC_EN_SET_CPU1_SET_CLR(w)   w &= ~( 0x1 << 5)

#define PMU_GLB_ISO_FRC_EN_SET_CPU0_SET_POS      4
#define PMU_GLB_ISO_FRC_EN_SET_CPU0_SET_LEN      1
#define PMU_GLB_ISO_FRC_EN_SET_CPU0_SET_MASK     0x00000010
#define PMU_GLB_ISO_FRC_EN_SET_CPU0_SET_VAL(w)   (((w) & 0x00000010) >> 4)
#define PMU_GLB_ISO_FRC_EN_SET_CPU0_SET_SET(w)   w |= ( 0x1 << 4)
#define PMU_GLB_ISO_FRC_EN_SET_CPU0_SET_CLR(w)   w &= ~( 0x1 << 4)

#define PMU_GLB_ISO_FRC_EN_SET_DSP_SET_POS      3
#define PMU_GLB_ISO_FRC_EN_SET_DSP_SET_LEN      1
#define PMU_GLB_ISO_FRC_EN_SET_DSP_SET_MASK     0x00000008
#define PMU_GLB_ISO_FRC_EN_SET_DSP_SET_VAL(w)   (((w) & 0x00000008) >> 3)
#define PMU_GLB_ISO_FRC_EN_SET_DSP_SET_SET(w)   w |= ( 0x1 << 3)
#define PMU_GLB_ISO_FRC_EN_SET_DSP_SET_CLR(w)   w &= ~( 0x1 << 3)

#define PMU_GLB_ISO_FRC_EN_SET_VDEC_SET_POS      2
#define PMU_GLB_ISO_FRC_EN_SET_VDEC_SET_LEN      1
#define PMU_GLB_ISO_FRC_EN_SET_VDEC_SET_MASK     0x00000004
#define PMU_GLB_ISO_FRC_EN_SET_VDEC_SET_VAL(w)   (((w) & 0x00000004) >> 2)
#define PMU_GLB_ISO_FRC_EN_SET_VDEC_SET_SET(w)   w |= ( 0x1 << 2)
#define PMU_GLB_ISO_FRC_EN_SET_VDEC_SET_CLR(w)   w &= ~( 0x1 << 2)

#define PMU_GLB_ISO_FRC_EN_SET_VENC1_SET_POS      1
#define PMU_GLB_ISO_FRC_EN_SET_VENC1_SET_LEN      1
#define PMU_GLB_ISO_FRC_EN_SET_VENC1_SET_MASK     0x00000002
#define PMU_GLB_ISO_FRC_EN_SET_VENC1_SET_VAL(w)   (((w) & 0x00000002) >> 1)
#define PMU_GLB_ISO_FRC_EN_SET_VENC1_SET_SET(w)   w |= ( 0x1 << 1)
#define PMU_GLB_ISO_FRC_EN_SET_VENC1_SET_CLR(w)   w &= ~( 0x1 << 1)

#define PMU_GLB_ISO_FRC_EN_SET_VENC0_SET_POS      0
#define PMU_GLB_ISO_FRC_EN_SET_VENC0_SET_LEN      1
#define PMU_GLB_ISO_FRC_EN_SET_VENC0_SET_MASK     0x00000001
#define PMU_GLB_ISO_FRC_EN_SET_VENC0_SET_VAL(w)   (((w) & 0x00000001) >> 0)
#define PMU_GLB_ISO_FRC_EN_SET_VENC0_SET_SET(w)   w |= ( 0x1 << 0)
#define PMU_GLB_ISO_FRC_EN_SET_VENC0_SET_CLR(w)   w &= ~( 0x1 << 0)

// REGISTER : ISO_FRC_EN_CLR

#define PMU_GLB_ISO_FRC_EN_CLR_ADDR      PMU_GLB_BASE_ADDR + 0x1C0
#define PMU_GLB_ISO_FRC_EN_CLR_RSTVAL    0x0
#define PMU_GLB_ISO_FRC_EN_CLR_ALL1      0x0
#define PMU_GLB_ISO_FRC_EN_CLR_ALL0      0x0
#define PMU_GLB_ISO_FRC_EN_CLR_ALL5A     0x0
#define PMU_GLB_ISO_FRC_EN_CLR_ALLA5     0x0

// FIELD IN REGISTER : ISO_FRC_EN_CLR

#define PMU_GLB_ISO_FRC_EN_CLR_ISP_CLR_POS      14
#define PMU_GLB_ISO_FRC_EN_CLR_ISP_CLR_LEN      1
#define PMU_GLB_ISO_FRC_EN_CLR_ISP_CLR_MASK     0x00004000
#define PMU_GLB_ISO_FRC_EN_CLR_ISP_CLR_VAL(w)   (((w) & 0x00004000) >> 14)
#define PMU_GLB_ISO_FRC_EN_CLR_ISP_CLR_SET(w)   w |= ( 0x1 << 14)
#define PMU_GLB_ISO_FRC_EN_CLR_ISP_CLR_CLR(w)   w &= ~( 0x1 << 14)

#define PMU_GLB_ISO_FRC_EN_CLR_CPU_CLR_POS      13
#define PMU_GLB_ISO_FRC_EN_CLR_CPU_CLR_LEN      1
#define PMU_GLB_ISO_FRC_EN_CLR_CPU_CLR_MASK     0x00002000
#define PMU_GLB_ISO_FRC_EN_CLR_CPU_CLR_VAL(w)   (((w) & 0x00002000) >> 13)
#define PMU_GLB_ISO_FRC_EN_CLR_CPU_CLR_SET(w)   w |= ( 0x1 << 13)
#define PMU_GLB_ISO_FRC_EN_CLR_CPU_CLR_CLR(w)   w &= ~( 0x1 << 13)

#define PMU_GLB_ISO_FRC_EN_CLR_NPU_CLR_POS      12
#define PMU_GLB_ISO_FRC_EN_CLR_NPU_CLR_LEN      1
#define PMU_GLB_ISO_FRC_EN_CLR_NPU_CLR_MASK     0x00001000
#define PMU_GLB_ISO_FRC_EN_CLR_NPU_CLR_VAL(w)   (((w) & 0x00001000) >> 12)
#define PMU_GLB_ISO_FRC_EN_CLR_NPU_CLR_SET(w)   w |= ( 0x1 << 12)
#define PMU_GLB_ISO_FRC_EN_CLR_NPU_CLR_CLR(w)   w &= ~( 0x1 << 12)

#define PMU_GLB_ISO_FRC_EN_CLR_CPU7_CLR_POS      11
#define PMU_GLB_ISO_FRC_EN_CLR_CPU7_CLR_LEN      1
#define PMU_GLB_ISO_FRC_EN_CLR_CPU7_CLR_MASK     0x00000800
#define PMU_GLB_ISO_FRC_EN_CLR_CPU7_CLR_VAL(w)   (((w) & 0x00000800) >> 11)
#define PMU_GLB_ISO_FRC_EN_CLR_CPU7_CLR_SET(w)   w |= ( 0x1 << 11)
#define PMU_GLB_ISO_FRC_EN_CLR_CPU7_CLR_CLR(w)   w &= ~( 0x1 << 11)

#define PMU_GLB_ISO_FRC_EN_CLR_CPU6_CLR_POS      10
#define PMU_GLB_ISO_FRC_EN_CLR_CPU6_CLR_LEN      1
#define PMU_GLB_ISO_FRC_EN_CLR_CPU6_CLR_MASK     0x00000400
#define PMU_GLB_ISO_FRC_EN_CLR_CPU6_CLR_VAL(w)   (((w) & 0x00000400) >> 10)
#define PMU_GLB_ISO_FRC_EN_CLR_CPU6_CLR_SET(w)   w |= ( 0x1 << 10)
#define PMU_GLB_ISO_FRC_EN_CLR_CPU6_CLR_CLR(w)   w &= ~( 0x1 << 10)

#define PMU_GLB_ISO_FRC_EN_CLR_CPU5_CLR_POS      9
#define PMU_GLB_ISO_FRC_EN_CLR_CPU5_CLR_LEN      1
#define PMU_GLB_ISO_FRC_EN_CLR_CPU5_CLR_MASK     0x00000200
#define PMU_GLB_ISO_FRC_EN_CLR_CPU5_CLR_VAL(w)   (((w) & 0x00000200) >> 9)
#define PMU_GLB_ISO_FRC_EN_CLR_CPU5_CLR_SET(w)   w |= ( 0x1 << 9)
#define PMU_GLB_ISO_FRC_EN_CLR_CPU5_CLR_CLR(w)   w &= ~( 0x1 << 9)

#define PMU_GLB_ISO_FRC_EN_CLR_CPU4_CLR_POS      8
#define PMU_GLB_ISO_FRC_EN_CLR_CPU4_CLR_LEN      1
#define PMU_GLB_ISO_FRC_EN_CLR_CPU4_CLR_MASK     0x00000100
#define PMU_GLB_ISO_FRC_EN_CLR_CPU4_CLR_VAL(w)   (((w) & 0x00000100) >> 8)
#define PMU_GLB_ISO_FRC_EN_CLR_CPU4_CLR_SET(w)   w |= ( 0x1 << 8)
#define PMU_GLB_ISO_FRC_EN_CLR_CPU4_CLR_CLR(w)   w &= ~( 0x1 << 8)

#define PMU_GLB_ISO_FRC_EN_CLR_CPU3_CLR_POS      7
#define PMU_GLB_ISO_FRC_EN_CLR_CPU3_CLR_LEN      1
#define PMU_GLB_ISO_FRC_EN_CLR_CPU3_CLR_MASK     0x00000080
#define PMU_GLB_ISO_FRC_EN_CLR_CPU3_CLR_VAL(w)   (((w) & 0x00000080) >> 7)
#define PMU_GLB_ISO_FRC_EN_CLR_CPU3_CLR_SET(w)   w |= ( 0x1 << 7)
#define PMU_GLB_ISO_FRC_EN_CLR_CPU3_CLR_CLR(w)   w &= ~( 0x1 << 7)

#define PMU_GLB_ISO_FRC_EN_CLR_CPU2_CLR_POS      6
#define PMU_GLB_ISO_FRC_EN_CLR_CPU2_CLR_LEN      1
#define PMU_GLB_ISO_FRC_EN_CLR_CPU2_CLR_MASK     0x00000040
#define PMU_GLB_ISO_FRC_EN_CLR_CPU2_CLR_VAL(w)   (((w) & 0x00000040) >> 6)
#define PMU_GLB_ISO_FRC_EN_CLR_CPU2_CLR_SET(w)   w |= ( 0x1 << 6)
#define PMU_GLB_ISO_FRC_EN_CLR_CPU2_CLR_CLR(w)   w &= ~( 0x1 << 6)

#define PMU_GLB_ISO_FRC_EN_CLR_CPU1_CLR_POS      5
#define PMU_GLB_ISO_FRC_EN_CLR_CPU1_CLR_LEN      1
#define PMU_GLB_ISO_FRC_EN_CLR_CPU1_CLR_MASK     0x00000020
#define PMU_GLB_ISO_FRC_EN_CLR_CPU1_CLR_VAL(w)   (((w) & 0x00000020) >> 5)
#define PMU_GLB_ISO_FRC_EN_CLR_CPU1_CLR_SET(w)   w |= ( 0x1 << 5)
#define PMU_GLB_ISO_FRC_EN_CLR_CPU1_CLR_CLR(w)   w &= ~( 0x1 << 5)

#define PMU_GLB_ISO_FRC_EN_CLR_CPU0_CLR_POS      4
#define PMU_GLB_ISO_FRC_EN_CLR_CPU0_CLR_LEN      1
#define PMU_GLB_ISO_FRC_EN_CLR_CPU0_CLR_MASK     0x00000010
#define PMU_GLB_ISO_FRC_EN_CLR_CPU0_CLR_VAL(w)   (((w) & 0x00000010) >> 4)
#define PMU_GLB_ISO_FRC_EN_CLR_CPU0_CLR_SET(w)   w |= ( 0x1 << 4)
#define PMU_GLB_ISO_FRC_EN_CLR_CPU0_CLR_CLR(w)   w &= ~( 0x1 << 4)

#define PMU_GLB_ISO_FRC_EN_CLR_DSP_CLR_POS      3
#define PMU_GLB_ISO_FRC_EN_CLR_DSP_CLR_LEN      1
#define PMU_GLB_ISO_FRC_EN_CLR_DSP_CLR_MASK     0x00000008
#define PMU_GLB_ISO_FRC_EN_CLR_DSP_CLR_VAL(w)   (((w) & 0x00000008) >> 3)
#define PMU_GLB_ISO_FRC_EN_CLR_DSP_CLR_SET(w)   w |= ( 0x1 << 3)
#define PMU_GLB_ISO_FRC_EN_CLR_DSP_CLR_CLR(w)   w &= ~( 0x1 << 3)

#define PMU_GLB_ISO_FRC_EN_CLR_VDEC_CLR_POS      2
#define PMU_GLB_ISO_FRC_EN_CLR_VDEC_CLR_LEN      1
#define PMU_GLB_ISO_FRC_EN_CLR_VDEC_CLR_MASK     0x00000004
#define PMU_GLB_ISO_FRC_EN_CLR_VDEC_CLR_VAL(w)   (((w) & 0x00000004) >> 2)
#define PMU_GLB_ISO_FRC_EN_CLR_VDEC_CLR_SET(w)   w |= ( 0x1 << 2)
#define PMU_GLB_ISO_FRC_EN_CLR_VDEC_CLR_CLR(w)   w &= ~( 0x1 << 2)

#define PMU_GLB_ISO_FRC_EN_CLR_VENC1_CLR_POS      1
#define PMU_GLB_ISO_FRC_EN_CLR_VENC1_CLR_LEN      1
#define PMU_GLB_ISO_FRC_EN_CLR_VENC1_CLR_MASK     0x00000002
#define PMU_GLB_ISO_FRC_EN_CLR_VENC1_CLR_VAL(w)   (((w) & 0x00000002) >> 1)
#define PMU_GLB_ISO_FRC_EN_CLR_VENC1_CLR_SET(w)   w |= ( 0x1 << 1)
#define PMU_GLB_ISO_FRC_EN_CLR_VENC1_CLR_CLR(w)   w &= ~( 0x1 << 1)

#define PMU_GLB_ISO_FRC_EN_CLR_VENC0_CLR_POS      0
#define PMU_GLB_ISO_FRC_EN_CLR_VENC0_CLR_LEN      1
#define PMU_GLB_ISO_FRC_EN_CLR_VENC0_CLR_MASK     0x00000001
#define PMU_GLB_ISO_FRC_EN_CLR_VENC0_CLR_VAL(w)   (((w) & 0x00000001) >> 0)
#define PMU_GLB_ISO_FRC_EN_CLR_VENC0_CLR_SET(w)   w |= ( 0x1 << 0)
#define PMU_GLB_ISO_FRC_EN_CLR_VENC0_CLR_CLR(w)   w &= ~( 0x1 << 0)

// REGISTER : ISO_FRC_EN1_SET

#define PMU_GLB_ISO_FRC_EN1_SET_ADDR      PMU_GLB_BASE_ADDR + 0x1C4
#define PMU_GLB_ISO_FRC_EN1_SET_RSTVAL    0x0
#define PMU_GLB_ISO_FRC_EN1_SET_ALL1      0x0
#define PMU_GLB_ISO_FRC_EN1_SET_ALL0      0x0
#define PMU_GLB_ISO_FRC_EN1_SET_ALL5A     0x0
#define PMU_GLB_ISO_FRC_EN1_SET_ALLA5     0x0

// FIELD IN REGISTER : ISO_FRC_EN1_SET

#define PMU_GLB_ISO_FRC_EN1_SET_N8_SET_POS      10
#define PMU_GLB_ISO_FRC_EN1_SET_N8_SET_LEN      1
#define PMU_GLB_ISO_FRC_EN1_SET_N8_SET_MASK     0x00000400
#define PMU_GLB_ISO_FRC_EN1_SET_N8_SET_VAL(w)   (((w) & 0x00000400) >> 10)
#define PMU_GLB_ISO_FRC_EN1_SET_N8_SET_SET(w)   w |= ( 0x1 << 10)
#define PMU_GLB_ISO_FRC_EN1_SET_N8_SET_CLR(w)   w &= ~( 0x1 << 10)

#define PMU_GLB_ISO_FRC_EN1_SET_N7_SET_POS      9
#define PMU_GLB_ISO_FRC_EN1_SET_N7_SET_LEN      1
#define PMU_GLB_ISO_FRC_EN1_SET_N7_SET_MASK     0x00000200
#define PMU_GLB_ISO_FRC_EN1_SET_N7_SET_VAL(w)   (((w) & 0x00000200) >> 9)
#define PMU_GLB_ISO_FRC_EN1_SET_N7_SET_SET(w)   w |= ( 0x1 << 9)
#define PMU_GLB_ISO_FRC_EN1_SET_N7_SET_CLR(w)   w &= ~( 0x1 << 9)

#define PMU_GLB_ISO_FRC_EN1_SET_N6_SET_POS      8
#define PMU_GLB_ISO_FRC_EN1_SET_N6_SET_LEN      1
#define PMU_GLB_ISO_FRC_EN1_SET_N6_SET_MASK     0x00000100
#define PMU_GLB_ISO_FRC_EN1_SET_N6_SET_VAL(w)   (((w) & 0x00000100) >> 8)
#define PMU_GLB_ISO_FRC_EN1_SET_N6_SET_SET(w)   w |= ( 0x1 << 8)
#define PMU_GLB_ISO_FRC_EN1_SET_N6_SET_CLR(w)   w &= ~( 0x1 << 8)

#define PMU_GLB_ISO_FRC_EN1_SET_N5_SET_POS      7
#define PMU_GLB_ISO_FRC_EN1_SET_N5_SET_LEN      1
#define PMU_GLB_ISO_FRC_EN1_SET_N5_SET_MASK     0x00000080
#define PMU_GLB_ISO_FRC_EN1_SET_N5_SET_VAL(w)   (((w) & 0x00000080) >> 7)
#define PMU_GLB_ISO_FRC_EN1_SET_N5_SET_SET(w)   w |= ( 0x1 << 7)
#define PMU_GLB_ISO_FRC_EN1_SET_N5_SET_CLR(w)   w &= ~( 0x1 << 7)

#define PMU_GLB_ISO_FRC_EN1_SET_N4_SET_POS      6
#define PMU_GLB_ISO_FRC_EN1_SET_N4_SET_LEN      1
#define PMU_GLB_ISO_FRC_EN1_SET_N4_SET_MASK     0x00000040
#define PMU_GLB_ISO_FRC_EN1_SET_N4_SET_VAL(w)   (((w) & 0x00000040) >> 6)
#define PMU_GLB_ISO_FRC_EN1_SET_N4_SET_SET(w)   w |= ( 0x1 << 6)
#define PMU_GLB_ISO_FRC_EN1_SET_N4_SET_CLR(w)   w &= ~( 0x1 << 6)

#define PMU_GLB_ISO_FRC_EN1_SET_N3_SET_POS      5
#define PMU_GLB_ISO_FRC_EN1_SET_N3_SET_LEN      1
#define PMU_GLB_ISO_FRC_EN1_SET_N3_SET_MASK     0x00000020
#define PMU_GLB_ISO_FRC_EN1_SET_N3_SET_VAL(w)   (((w) & 0x00000020) >> 5)
#define PMU_GLB_ISO_FRC_EN1_SET_N3_SET_SET(w)   w |= ( 0x1 << 5)
#define PMU_GLB_ISO_FRC_EN1_SET_N3_SET_CLR(w)   w &= ~( 0x1 << 5)

#define PMU_GLB_ISO_FRC_EN1_SET_N2_SET_POS      4
#define PMU_GLB_ISO_FRC_EN1_SET_N2_SET_LEN      1
#define PMU_GLB_ISO_FRC_EN1_SET_N2_SET_MASK     0x00000010
#define PMU_GLB_ISO_FRC_EN1_SET_N2_SET_VAL(w)   (((w) & 0x00000010) >> 4)
#define PMU_GLB_ISO_FRC_EN1_SET_N2_SET_SET(w)   w |= ( 0x1 << 4)
#define PMU_GLB_ISO_FRC_EN1_SET_N2_SET_CLR(w)   w &= ~( 0x1 << 4)

#define PMU_GLB_ISO_FRC_EN1_SET_N1_SET_POS      3
#define PMU_GLB_ISO_FRC_EN1_SET_N1_SET_LEN      1
#define PMU_GLB_ISO_FRC_EN1_SET_N1_SET_MASK     0x00000008
#define PMU_GLB_ISO_FRC_EN1_SET_N1_SET_VAL(w)   (((w) & 0x00000008) >> 3)
#define PMU_GLB_ISO_FRC_EN1_SET_N1_SET_SET(w)   w |= ( 0x1 << 3)
#define PMU_GLB_ISO_FRC_EN1_SET_N1_SET_CLR(w)   w &= ~( 0x1 << 3)

#define PMU_GLB_ISO_FRC_EN1_SET_N0_SET_POS      2
#define PMU_GLB_ISO_FRC_EN1_SET_N0_SET_LEN      1
#define PMU_GLB_ISO_FRC_EN1_SET_N0_SET_MASK     0x00000004
#define PMU_GLB_ISO_FRC_EN1_SET_N0_SET_VAL(w)   (((w) & 0x00000004) >> 2)
#define PMU_GLB_ISO_FRC_EN1_SET_N0_SET_SET(w)   w |= ( 0x1 << 2)
#define PMU_GLB_ISO_FRC_EN1_SET_N0_SET_CLR(w)   w &= ~( 0x1 << 2)

#define PMU_GLB_ISO_FRC_EN1_SET_MM_SET_POS      1
#define PMU_GLB_ISO_FRC_EN1_SET_MM_SET_LEN      1
#define PMU_GLB_ISO_FRC_EN1_SET_MM_SET_MASK     0x00000002
#define PMU_GLB_ISO_FRC_EN1_SET_MM_SET_VAL(w)   (((w) & 0x00000002) >> 1)
#define PMU_GLB_ISO_FRC_EN1_SET_MM_SET_SET(w)   w |= ( 0x1 << 1)
#define PMU_GLB_ISO_FRC_EN1_SET_MM_SET_CLR(w)   w &= ~( 0x1 << 1)

#define PMU_GLB_ISO_FRC_EN1_SET_PIPE_SET_POS      0
#define PMU_GLB_ISO_FRC_EN1_SET_PIPE_SET_LEN      1
#define PMU_GLB_ISO_FRC_EN1_SET_PIPE_SET_MASK     0x00000001
#define PMU_GLB_ISO_FRC_EN1_SET_PIPE_SET_VAL(w)   (((w) & 0x00000001) >> 0)
#define PMU_GLB_ISO_FRC_EN1_SET_PIPE_SET_SET(w)   w |= ( 0x1 << 0)
#define PMU_GLB_ISO_FRC_EN1_SET_PIPE_SET_CLR(w)   w &= ~( 0x1 << 0)

// REGISTER : ISO_FRC_EN1_CLR

#define PMU_GLB_ISO_FRC_EN1_CLR_ADDR      PMU_GLB_BASE_ADDR + 0x1C8
#define PMU_GLB_ISO_FRC_EN1_CLR_RSTVAL    0x0
#define PMU_GLB_ISO_FRC_EN1_CLR_ALL1      0x0
#define PMU_GLB_ISO_FRC_EN1_CLR_ALL0      0x0
#define PMU_GLB_ISO_FRC_EN1_CLR_ALL5A     0x0
#define PMU_GLB_ISO_FRC_EN1_CLR_ALLA5     0x0

// FIELD IN REGISTER : ISO_FRC_EN1_CLR

#define PMU_GLB_ISO_FRC_EN1_CLR_N8_CLR_POS      10
#define PMU_GLB_ISO_FRC_EN1_CLR_N8_CLR_LEN      1
#define PMU_GLB_ISO_FRC_EN1_CLR_N8_CLR_MASK     0x00000400
#define PMU_GLB_ISO_FRC_EN1_CLR_N8_CLR_VAL(w)   (((w) & 0x00000400) >> 10)
#define PMU_GLB_ISO_FRC_EN1_CLR_N8_CLR_SET(w)   w |= ( 0x1 << 10)
#define PMU_GLB_ISO_FRC_EN1_CLR_N8_CLR_CLR(w)   w &= ~( 0x1 << 10)

#define PMU_GLB_ISO_FRC_EN1_CLR_N7_CLR_POS      9
#define PMU_GLB_ISO_FRC_EN1_CLR_N7_CLR_LEN      1
#define PMU_GLB_ISO_FRC_EN1_CLR_N7_CLR_MASK     0x00000200
#define PMU_GLB_ISO_FRC_EN1_CLR_N7_CLR_VAL(w)   (((w) & 0x00000200) >> 9)
#define PMU_GLB_ISO_FRC_EN1_CLR_N7_CLR_SET(w)   w |= ( 0x1 << 9)
#define PMU_GLB_ISO_FRC_EN1_CLR_N7_CLR_CLR(w)   w &= ~( 0x1 << 9)

#define PMU_GLB_ISO_FRC_EN1_CLR_N6_CLR_POS      8
#define PMU_GLB_ISO_FRC_EN1_CLR_N6_CLR_LEN      1
#define PMU_GLB_ISO_FRC_EN1_CLR_N6_CLR_MASK     0x00000100
#define PMU_GLB_ISO_FRC_EN1_CLR_N6_CLR_VAL(w)   (((w) & 0x00000100) >> 8)
#define PMU_GLB_ISO_FRC_EN1_CLR_N6_CLR_SET(w)   w |= ( 0x1 << 8)
#define PMU_GLB_ISO_FRC_EN1_CLR_N6_CLR_CLR(w)   w &= ~( 0x1 << 8)

#define PMU_GLB_ISO_FRC_EN1_CLR_N5_CLR_POS      7
#define PMU_GLB_ISO_FRC_EN1_CLR_N5_CLR_LEN      1
#define PMU_GLB_ISO_FRC_EN1_CLR_N5_CLR_MASK     0x00000080
#define PMU_GLB_ISO_FRC_EN1_CLR_N5_CLR_VAL(w)   (((w) & 0x00000080) >> 7)
#define PMU_GLB_ISO_FRC_EN1_CLR_N5_CLR_SET(w)   w |= ( 0x1 << 7)
#define PMU_GLB_ISO_FRC_EN1_CLR_N5_CLR_CLR(w)   w &= ~( 0x1 << 7)

#define PMU_GLB_ISO_FRC_EN1_CLR_N4_CLR_POS      6
#define PMU_GLB_ISO_FRC_EN1_CLR_N4_CLR_LEN      1
#define PMU_GLB_ISO_FRC_EN1_CLR_N4_CLR_MASK     0x00000040
#define PMU_GLB_ISO_FRC_EN1_CLR_N4_CLR_VAL(w)   (((w) & 0x00000040) >> 6)
#define PMU_GLB_ISO_FRC_EN1_CLR_N4_CLR_SET(w)   w |= ( 0x1 << 6)
#define PMU_GLB_ISO_FRC_EN1_CLR_N4_CLR_CLR(w)   w &= ~( 0x1 << 6)

#define PMU_GLB_ISO_FRC_EN1_CLR_N3_CLR_POS      5
#define PMU_GLB_ISO_FRC_EN1_CLR_N3_CLR_LEN      1
#define PMU_GLB_ISO_FRC_EN1_CLR_N3_CLR_MASK     0x00000020
#define PMU_GLB_ISO_FRC_EN1_CLR_N3_CLR_VAL(w)   (((w) & 0x00000020) >> 5)
#define PMU_GLB_ISO_FRC_EN1_CLR_N3_CLR_SET(w)   w |= ( 0x1 << 5)
#define PMU_GLB_ISO_FRC_EN1_CLR_N3_CLR_CLR(w)   w &= ~( 0x1 << 5)

#define PMU_GLB_ISO_FRC_EN1_CLR_N2_CLR_POS      4
#define PMU_GLB_ISO_FRC_EN1_CLR_N2_CLR_LEN      1
#define PMU_GLB_ISO_FRC_EN1_CLR_N2_CLR_MASK     0x00000010
#define PMU_GLB_ISO_FRC_EN1_CLR_N2_CLR_VAL(w)   (((w) & 0x00000010) >> 4)
#define PMU_GLB_ISO_FRC_EN1_CLR_N2_CLR_SET(w)   w |= ( 0x1 << 4)
#define PMU_GLB_ISO_FRC_EN1_CLR_N2_CLR_CLR(w)   w &= ~( 0x1 << 4)

#define PMU_GLB_ISO_FRC_EN1_CLR_N1_CLR_POS      3
#define PMU_GLB_ISO_FRC_EN1_CLR_N1_CLR_LEN      1
#define PMU_GLB_ISO_FRC_EN1_CLR_N1_CLR_MASK     0x00000008
#define PMU_GLB_ISO_FRC_EN1_CLR_N1_CLR_VAL(w)   (((w) & 0x00000008) >> 3)
#define PMU_GLB_ISO_FRC_EN1_CLR_N1_CLR_SET(w)   w |= ( 0x1 << 3)
#define PMU_GLB_ISO_FRC_EN1_CLR_N1_CLR_CLR(w)   w &= ~( 0x1 << 3)

#define PMU_GLB_ISO_FRC_EN1_CLR_N0_CLR_POS      2
#define PMU_GLB_ISO_FRC_EN1_CLR_N0_CLR_LEN      1
#define PMU_GLB_ISO_FRC_EN1_CLR_N0_CLR_MASK     0x00000004
#define PMU_GLB_ISO_FRC_EN1_CLR_N0_CLR_VAL(w)   (((w) & 0x00000004) >> 2)
#define PMU_GLB_ISO_FRC_EN1_CLR_N0_CLR_SET(w)   w |= ( 0x1 << 2)
#define PMU_GLB_ISO_FRC_EN1_CLR_N0_CLR_CLR(w)   w &= ~( 0x1 << 2)

#define PMU_GLB_ISO_FRC_EN1_CLR_MM_CLR_POS      1
#define PMU_GLB_ISO_FRC_EN1_CLR_MM_CLR_LEN      1
#define PMU_GLB_ISO_FRC_EN1_CLR_MM_CLR_MASK     0x00000002
#define PMU_GLB_ISO_FRC_EN1_CLR_MM_CLR_VAL(w)   (((w) & 0x00000002) >> 1)
#define PMU_GLB_ISO_FRC_EN1_CLR_MM_CLR_SET(w)   w |= ( 0x1 << 1)
#define PMU_GLB_ISO_FRC_EN1_CLR_MM_CLR_CLR(w)   w &= ~( 0x1 << 1)

#define PMU_GLB_ISO_FRC_EN1_CLR_PIPE_CLR_POS      0
#define PMU_GLB_ISO_FRC_EN1_CLR_PIPE_CLR_LEN      1
#define PMU_GLB_ISO_FRC_EN1_CLR_PIPE_CLR_MASK     0x00000001
#define PMU_GLB_ISO_FRC_EN1_CLR_PIPE_CLR_VAL(w)   (((w) & 0x00000001) >> 0)
#define PMU_GLB_ISO_FRC_EN1_CLR_PIPE_CLR_SET(w)   w |= ( 0x1 << 0)
#define PMU_GLB_ISO_FRC_EN1_CLR_PIPE_CLR_CLR(w)   w &= ~( 0x1 << 0)

// REGISTER : ISO_FRC_SW_SET

#define PMU_GLB_ISO_FRC_SW_SET_ADDR      PMU_GLB_BASE_ADDR + 0x1CC
#define PMU_GLB_ISO_FRC_SW_SET_RSTVAL    0x0
#define PMU_GLB_ISO_FRC_SW_SET_ALL1      0x0
#define PMU_GLB_ISO_FRC_SW_SET_ALL0      0x0
#define PMU_GLB_ISO_FRC_SW_SET_ALL5A     0x0
#define PMU_GLB_ISO_FRC_SW_SET_ALLA5     0x0

// FIELD IN REGISTER : ISO_FRC_SW_SET

#define PMU_GLB_ISO_FRC_SW_SET_ISP_SET_POS      14
#define PMU_GLB_ISO_FRC_SW_SET_ISP_SET_LEN      1
#define PMU_GLB_ISO_FRC_SW_SET_ISP_SET_MASK     0x00004000
#define PMU_GLB_ISO_FRC_SW_SET_ISP_SET_VAL(w)   (((w) & 0x00004000) >> 14)
#define PMU_GLB_ISO_FRC_SW_SET_ISP_SET_SET(w)   w |= ( 0x1 << 14)
#define PMU_GLB_ISO_FRC_SW_SET_ISP_SET_CLR(w)   w &= ~( 0x1 << 14)

#define PMU_GLB_ISO_FRC_SW_SET_CPU_SET_POS      13
#define PMU_GLB_ISO_FRC_SW_SET_CPU_SET_LEN      1
#define PMU_GLB_ISO_FRC_SW_SET_CPU_SET_MASK     0x00002000
#define PMU_GLB_ISO_FRC_SW_SET_CPU_SET_VAL(w)   (((w) & 0x00002000) >> 13)
#define PMU_GLB_ISO_FRC_SW_SET_CPU_SET_SET(w)   w |= ( 0x1 << 13)
#define PMU_GLB_ISO_FRC_SW_SET_CPU_SET_CLR(w)   w &= ~( 0x1 << 13)

#define PMU_GLB_ISO_FRC_SW_SET_NPU_SET_POS      12
#define PMU_GLB_ISO_FRC_SW_SET_NPU_SET_LEN      1
#define PMU_GLB_ISO_FRC_SW_SET_NPU_SET_MASK     0x00001000
#define PMU_GLB_ISO_FRC_SW_SET_NPU_SET_VAL(w)   (((w) & 0x00001000) >> 12)
#define PMU_GLB_ISO_FRC_SW_SET_NPU_SET_SET(w)   w |= ( 0x1 << 12)
#define PMU_GLB_ISO_FRC_SW_SET_NPU_SET_CLR(w)   w &= ~( 0x1 << 12)

#define PMU_GLB_ISO_FRC_SW_SET_CPU7_SET_POS      11
#define PMU_GLB_ISO_FRC_SW_SET_CPU7_SET_LEN      1
#define PMU_GLB_ISO_FRC_SW_SET_CPU7_SET_MASK     0x00000800
#define PMU_GLB_ISO_FRC_SW_SET_CPU7_SET_VAL(w)   (((w) & 0x00000800) >> 11)
#define PMU_GLB_ISO_FRC_SW_SET_CPU7_SET_SET(w)   w |= ( 0x1 << 11)
#define PMU_GLB_ISO_FRC_SW_SET_CPU7_SET_CLR(w)   w &= ~( 0x1 << 11)

#define PMU_GLB_ISO_FRC_SW_SET_CPU6_SET_POS      10
#define PMU_GLB_ISO_FRC_SW_SET_CPU6_SET_LEN      1
#define PMU_GLB_ISO_FRC_SW_SET_CPU6_SET_MASK     0x00000400
#define PMU_GLB_ISO_FRC_SW_SET_CPU6_SET_VAL(w)   (((w) & 0x00000400) >> 10)
#define PMU_GLB_ISO_FRC_SW_SET_CPU6_SET_SET(w)   w |= ( 0x1 << 10)
#define PMU_GLB_ISO_FRC_SW_SET_CPU6_SET_CLR(w)   w &= ~( 0x1 << 10)

#define PMU_GLB_ISO_FRC_SW_SET_CPU5_SET_POS      9
#define PMU_GLB_ISO_FRC_SW_SET_CPU5_SET_LEN      1
#define PMU_GLB_ISO_FRC_SW_SET_CPU5_SET_MASK     0x00000200
#define PMU_GLB_ISO_FRC_SW_SET_CPU5_SET_VAL(w)   (((w) & 0x00000200) >> 9)
#define PMU_GLB_ISO_FRC_SW_SET_CPU5_SET_SET(w)   w |= ( 0x1 << 9)
#define PMU_GLB_ISO_FRC_SW_SET_CPU5_SET_CLR(w)   w &= ~( 0x1 << 9)

#define PMU_GLB_ISO_FRC_SW_SET_CPU4_SET_POS      8
#define PMU_GLB_ISO_FRC_SW_SET_CPU4_SET_LEN      1
#define PMU_GLB_ISO_FRC_SW_SET_CPU4_SET_MASK     0x00000100
#define PMU_GLB_ISO_FRC_SW_SET_CPU4_SET_VAL(w)   (((w) & 0x00000100) >> 8)
#define PMU_GLB_ISO_FRC_SW_SET_CPU4_SET_SET(w)   w |= ( 0x1 << 8)
#define PMU_GLB_ISO_FRC_SW_SET_CPU4_SET_CLR(w)   w &= ~( 0x1 << 8)

#define PMU_GLB_ISO_FRC_SW_SET_CPU3_SET_POS      7
#define PMU_GLB_ISO_FRC_SW_SET_CPU3_SET_LEN      1
#define PMU_GLB_ISO_FRC_SW_SET_CPU3_SET_MASK     0x00000080
#define PMU_GLB_ISO_FRC_SW_SET_CPU3_SET_VAL(w)   (((w) & 0x00000080) >> 7)
#define PMU_GLB_ISO_FRC_SW_SET_CPU3_SET_SET(w)   w |= ( 0x1 << 7)
#define PMU_GLB_ISO_FRC_SW_SET_CPU3_SET_CLR(w)   w &= ~( 0x1 << 7)

#define PMU_GLB_ISO_FRC_SW_SET_CPU2_SET_POS      6
#define PMU_GLB_ISO_FRC_SW_SET_CPU2_SET_LEN      1
#define PMU_GLB_ISO_FRC_SW_SET_CPU2_SET_MASK     0x00000040
#define PMU_GLB_ISO_FRC_SW_SET_CPU2_SET_VAL(w)   (((w) & 0x00000040) >> 6)
#define PMU_GLB_ISO_FRC_SW_SET_CPU2_SET_SET(w)   w |= ( 0x1 << 6)
#define PMU_GLB_ISO_FRC_SW_SET_CPU2_SET_CLR(w)   w &= ~( 0x1 << 6)

#define PMU_GLB_ISO_FRC_SW_SET_CPU1_SET_POS      5
#define PMU_GLB_ISO_FRC_SW_SET_CPU1_SET_LEN      1
#define PMU_GLB_ISO_FRC_SW_SET_CPU1_SET_MASK     0x00000020
#define PMU_GLB_ISO_FRC_SW_SET_CPU1_SET_VAL(w)   (((w) & 0x00000020) >> 5)
#define PMU_GLB_ISO_FRC_SW_SET_CPU1_SET_SET(w)   w |= ( 0x1 << 5)
#define PMU_GLB_ISO_FRC_SW_SET_CPU1_SET_CLR(w)   w &= ~( 0x1 << 5)

#define PMU_GLB_ISO_FRC_SW_SET_CPU0_SET_POS      4
#define PMU_GLB_ISO_FRC_SW_SET_CPU0_SET_LEN      1
#define PMU_GLB_ISO_FRC_SW_SET_CPU0_SET_MASK     0x00000010
#define PMU_GLB_ISO_FRC_SW_SET_CPU0_SET_VAL(w)   (((w) & 0x00000010) >> 4)
#define PMU_GLB_ISO_FRC_SW_SET_CPU0_SET_SET(w)   w |= ( 0x1 << 4)
#define PMU_GLB_ISO_FRC_SW_SET_CPU0_SET_CLR(w)   w &= ~( 0x1 << 4)

#define PMU_GLB_ISO_FRC_SW_SET_DSP_SET_POS      3
#define PMU_GLB_ISO_FRC_SW_SET_DSP_SET_LEN      1
#define PMU_GLB_ISO_FRC_SW_SET_DSP_SET_MASK     0x00000008
#define PMU_GLB_ISO_FRC_SW_SET_DSP_SET_VAL(w)   (((w) & 0x00000008) >> 3)
#define PMU_GLB_ISO_FRC_SW_SET_DSP_SET_SET(w)   w |= ( 0x1 << 3)
#define PMU_GLB_ISO_FRC_SW_SET_DSP_SET_CLR(w)   w &= ~( 0x1 << 3)

#define PMU_GLB_ISO_FRC_SW_SET_VDEC_SET_POS      2
#define PMU_GLB_ISO_FRC_SW_SET_VDEC_SET_LEN      1
#define PMU_GLB_ISO_FRC_SW_SET_VDEC_SET_MASK     0x00000004
#define PMU_GLB_ISO_FRC_SW_SET_VDEC_SET_VAL(w)   (((w) & 0x00000004) >> 2)
#define PMU_GLB_ISO_FRC_SW_SET_VDEC_SET_SET(w)   w |= ( 0x1 << 2)
#define PMU_GLB_ISO_FRC_SW_SET_VDEC_SET_CLR(w)   w &= ~( 0x1 << 2)

#define PMU_GLB_ISO_FRC_SW_SET_VENC1_SET_POS      1
#define PMU_GLB_ISO_FRC_SW_SET_VENC1_SET_LEN      1
#define PMU_GLB_ISO_FRC_SW_SET_VENC1_SET_MASK     0x00000002
#define PMU_GLB_ISO_FRC_SW_SET_VENC1_SET_VAL(w)   (((w) & 0x00000002) >> 1)
#define PMU_GLB_ISO_FRC_SW_SET_VENC1_SET_SET(w)   w |= ( 0x1 << 1)
#define PMU_GLB_ISO_FRC_SW_SET_VENC1_SET_CLR(w)   w &= ~( 0x1 << 1)

#define PMU_GLB_ISO_FRC_SW_SET_VENC0_SET_POS      0
#define PMU_GLB_ISO_FRC_SW_SET_VENC0_SET_LEN      1
#define PMU_GLB_ISO_FRC_SW_SET_VENC0_SET_MASK     0x00000001
#define PMU_GLB_ISO_FRC_SW_SET_VENC0_SET_VAL(w)   (((w) & 0x00000001) >> 0)
#define PMU_GLB_ISO_FRC_SW_SET_VENC0_SET_SET(w)   w |= ( 0x1 << 0)
#define PMU_GLB_ISO_FRC_SW_SET_VENC0_SET_CLR(w)   w &= ~( 0x1 << 0)

// REGISTER : ISO_FRC_SW_CLR

#define PMU_GLB_ISO_FRC_SW_CLR_ADDR      PMU_GLB_BASE_ADDR + 0x1D0
#define PMU_GLB_ISO_FRC_SW_CLR_RSTVAL    0x0
#define PMU_GLB_ISO_FRC_SW_CLR_ALL1      0x0
#define PMU_GLB_ISO_FRC_SW_CLR_ALL0      0x0
#define PMU_GLB_ISO_FRC_SW_CLR_ALL5A     0x0
#define PMU_GLB_ISO_FRC_SW_CLR_ALLA5     0x0

// FIELD IN REGISTER : ISO_FRC_SW_CLR

#define PMU_GLB_ISO_FRC_SW_CLR_ISP_CLR_POS      14
#define PMU_GLB_ISO_FRC_SW_CLR_ISP_CLR_LEN      1
#define PMU_GLB_ISO_FRC_SW_CLR_ISP_CLR_MASK     0x00004000
#define PMU_GLB_ISO_FRC_SW_CLR_ISP_CLR_VAL(w)   (((w) & 0x00004000) >> 14)
#define PMU_GLB_ISO_FRC_SW_CLR_ISP_CLR_SET(w)   w |= ( 0x1 << 14)
#define PMU_GLB_ISO_FRC_SW_CLR_ISP_CLR_CLR(w)   w &= ~( 0x1 << 14)

#define PMU_GLB_ISO_FRC_SW_CLR_CPU_CLR_POS      13
#define PMU_GLB_ISO_FRC_SW_CLR_CPU_CLR_LEN      1
#define PMU_GLB_ISO_FRC_SW_CLR_CPU_CLR_MASK     0x00002000
#define PMU_GLB_ISO_FRC_SW_CLR_CPU_CLR_VAL(w)   (((w) & 0x00002000) >> 13)
#define PMU_GLB_ISO_FRC_SW_CLR_CPU_CLR_SET(w)   w |= ( 0x1 << 13)
#define PMU_GLB_ISO_FRC_SW_CLR_CPU_CLR_CLR(w)   w &= ~( 0x1 << 13)

#define PMU_GLB_ISO_FRC_SW_CLR_NPU_CLR_POS      12
#define PMU_GLB_ISO_FRC_SW_CLR_NPU_CLR_LEN      1
#define PMU_GLB_ISO_FRC_SW_CLR_NPU_CLR_MASK     0x00001000
#define PMU_GLB_ISO_FRC_SW_CLR_NPU_CLR_VAL(w)   (((w) & 0x00001000) >> 12)
#define PMU_GLB_ISO_FRC_SW_CLR_NPU_CLR_SET(w)   w |= ( 0x1 << 12)
#define PMU_GLB_ISO_FRC_SW_CLR_NPU_CLR_CLR(w)   w &= ~( 0x1 << 12)

#define PMU_GLB_ISO_FRC_SW_CLR_CPU7_CLR_POS      11
#define PMU_GLB_ISO_FRC_SW_CLR_CPU7_CLR_LEN      1
#define PMU_GLB_ISO_FRC_SW_CLR_CPU7_CLR_MASK     0x00000800
#define PMU_GLB_ISO_FRC_SW_CLR_CPU7_CLR_VAL(w)   (((w) & 0x00000800) >> 11)
#define PMU_GLB_ISO_FRC_SW_CLR_CPU7_CLR_SET(w)   w |= ( 0x1 << 11)
#define PMU_GLB_ISO_FRC_SW_CLR_CPU7_CLR_CLR(w)   w &= ~( 0x1 << 11)

#define PMU_GLB_ISO_FRC_SW_CLR_CPU6_CLR_POS      10
#define PMU_GLB_ISO_FRC_SW_CLR_CPU6_CLR_LEN      1
#define PMU_GLB_ISO_FRC_SW_CLR_CPU6_CLR_MASK     0x00000400
#define PMU_GLB_ISO_FRC_SW_CLR_CPU6_CLR_VAL(w)   (((w) & 0x00000400) >> 10)
#define PMU_GLB_ISO_FRC_SW_CLR_CPU6_CLR_SET(w)   w |= ( 0x1 << 10)
#define PMU_GLB_ISO_FRC_SW_CLR_CPU6_CLR_CLR(w)   w &= ~( 0x1 << 10)

#define PMU_GLB_ISO_FRC_SW_CLR_CPU5_CLR_POS      9
#define PMU_GLB_ISO_FRC_SW_CLR_CPU5_CLR_LEN      1
#define PMU_GLB_ISO_FRC_SW_CLR_CPU5_CLR_MASK     0x00000200
#define PMU_GLB_ISO_FRC_SW_CLR_CPU5_CLR_VAL(w)   (((w) & 0x00000200) >> 9)
#define PMU_GLB_ISO_FRC_SW_CLR_CPU5_CLR_SET(w)   w |= ( 0x1 << 9)
#define PMU_GLB_ISO_FRC_SW_CLR_CPU5_CLR_CLR(w)   w &= ~( 0x1 << 9)

#define PMU_GLB_ISO_FRC_SW_CLR_CPU4_CLR_POS      8
#define PMU_GLB_ISO_FRC_SW_CLR_CPU4_CLR_LEN      1
#define PMU_GLB_ISO_FRC_SW_CLR_CPU4_CLR_MASK     0x00000100
#define PMU_GLB_ISO_FRC_SW_CLR_CPU4_CLR_VAL(w)   (((w) & 0x00000100) >> 8)
#define PMU_GLB_ISO_FRC_SW_CLR_CPU4_CLR_SET(w)   w |= ( 0x1 << 8)
#define PMU_GLB_ISO_FRC_SW_CLR_CPU4_CLR_CLR(w)   w &= ~( 0x1 << 8)

#define PMU_GLB_ISO_FRC_SW_CLR_CPU3_CLR_POS      7
#define PMU_GLB_ISO_FRC_SW_CLR_CPU3_CLR_LEN      1
#define PMU_GLB_ISO_FRC_SW_CLR_CPU3_CLR_MASK     0x00000080
#define PMU_GLB_ISO_FRC_SW_CLR_CPU3_CLR_VAL(w)   (((w) & 0x00000080) >> 7)
#define PMU_GLB_ISO_FRC_SW_CLR_CPU3_CLR_SET(w)   w |= ( 0x1 << 7)
#define PMU_GLB_ISO_FRC_SW_CLR_CPU3_CLR_CLR(w)   w &= ~( 0x1 << 7)

#define PMU_GLB_ISO_FRC_SW_CLR_CPU2_CLR_POS      6
#define PMU_GLB_ISO_FRC_SW_CLR_CPU2_CLR_LEN      1
#define PMU_GLB_ISO_FRC_SW_CLR_CPU2_CLR_MASK     0x00000040
#define PMU_GLB_ISO_FRC_SW_CLR_CPU2_CLR_VAL(w)   (((w) & 0x00000040) >> 6)
#define PMU_GLB_ISO_FRC_SW_CLR_CPU2_CLR_SET(w)   w |= ( 0x1 << 6)
#define PMU_GLB_ISO_FRC_SW_CLR_CPU2_CLR_CLR(w)   w &= ~( 0x1 << 6)

#define PMU_GLB_ISO_FRC_SW_CLR_CPU1_CLR_POS      5
#define PMU_GLB_ISO_FRC_SW_CLR_CPU1_CLR_LEN      1
#define PMU_GLB_ISO_FRC_SW_CLR_CPU1_CLR_MASK     0x00000020
#define PMU_GLB_ISO_FRC_SW_CLR_CPU1_CLR_VAL(w)   (((w) & 0x00000020) >> 5)
#define PMU_GLB_ISO_FRC_SW_CLR_CPU1_CLR_SET(w)   w |= ( 0x1 << 5)
#define PMU_GLB_ISO_FRC_SW_CLR_CPU1_CLR_CLR(w)   w &= ~( 0x1 << 5)

#define PMU_GLB_ISO_FRC_SW_CLR_CPU0_CLR_POS      4
#define PMU_GLB_ISO_FRC_SW_CLR_CPU0_CLR_LEN      1
#define PMU_GLB_ISO_FRC_SW_CLR_CPU0_CLR_MASK     0x00000010
#define PMU_GLB_ISO_FRC_SW_CLR_CPU0_CLR_VAL(w)   (((w) & 0x00000010) >> 4)
#define PMU_GLB_ISO_FRC_SW_CLR_CPU0_CLR_SET(w)   w |= ( 0x1 << 4)
#define PMU_GLB_ISO_FRC_SW_CLR_CPU0_CLR_CLR(w)   w &= ~( 0x1 << 4)

#define PMU_GLB_ISO_FRC_SW_CLR_DSP_CLR_POS      3
#define PMU_GLB_ISO_FRC_SW_CLR_DSP_CLR_LEN      1
#define PMU_GLB_ISO_FRC_SW_CLR_DSP_CLR_MASK     0x00000008
#define PMU_GLB_ISO_FRC_SW_CLR_DSP_CLR_VAL(w)   (((w) & 0x00000008) >> 3)
#define PMU_GLB_ISO_FRC_SW_CLR_DSP_CLR_SET(w)   w |= ( 0x1 << 3)
#define PMU_GLB_ISO_FRC_SW_CLR_DSP_CLR_CLR(w)   w &= ~( 0x1 << 3)

#define PMU_GLB_ISO_FRC_SW_CLR_VDEC_CLR_POS      2
#define PMU_GLB_ISO_FRC_SW_CLR_VDEC_CLR_LEN      1
#define PMU_GLB_ISO_FRC_SW_CLR_VDEC_CLR_MASK     0x00000004
#define PMU_GLB_ISO_FRC_SW_CLR_VDEC_CLR_VAL(w)   (((w) & 0x00000004) >> 2)
#define PMU_GLB_ISO_FRC_SW_CLR_VDEC_CLR_SET(w)   w |= ( 0x1 << 2)
#define PMU_GLB_ISO_FRC_SW_CLR_VDEC_CLR_CLR(w)   w &= ~( 0x1 << 2)

#define PMU_GLB_ISO_FRC_SW_CLR_VENC1_CLR_POS      1
#define PMU_GLB_ISO_FRC_SW_CLR_VENC1_CLR_LEN      1
#define PMU_GLB_ISO_FRC_SW_CLR_VENC1_CLR_MASK     0x00000002
#define PMU_GLB_ISO_FRC_SW_CLR_VENC1_CLR_VAL(w)   (((w) & 0x00000002) >> 1)
#define PMU_GLB_ISO_FRC_SW_CLR_VENC1_CLR_SET(w)   w |= ( 0x1 << 1)
#define PMU_GLB_ISO_FRC_SW_CLR_VENC1_CLR_CLR(w)   w &= ~( 0x1 << 1)

#define PMU_GLB_ISO_FRC_SW_CLR_VENC0_CLR_POS      0
#define PMU_GLB_ISO_FRC_SW_CLR_VENC0_CLR_LEN      1
#define PMU_GLB_ISO_FRC_SW_CLR_VENC0_CLR_MASK     0x00000001
#define PMU_GLB_ISO_FRC_SW_CLR_VENC0_CLR_VAL(w)   (((w) & 0x00000001) >> 0)
#define PMU_GLB_ISO_FRC_SW_CLR_VENC0_CLR_SET(w)   w |= ( 0x1 << 0)
#define PMU_GLB_ISO_FRC_SW_CLR_VENC0_CLR_CLR(w)   w &= ~( 0x1 << 0)

// REGISTER : ISO_FRC_SW1_SET

#define PMU_GLB_ISO_FRC_SW1_SET_ADDR      PMU_GLB_BASE_ADDR + 0x1D4
#define PMU_GLB_ISO_FRC_SW1_SET_RSTVAL    0x0
#define PMU_GLB_ISO_FRC_SW1_SET_ALL1      0x0
#define PMU_GLB_ISO_FRC_SW1_SET_ALL0      0x0
#define PMU_GLB_ISO_FRC_SW1_SET_ALL5A     0x0
#define PMU_GLB_ISO_FRC_SW1_SET_ALLA5     0x0

// FIELD IN REGISTER : ISO_FRC_SW1_SET

#define PMU_GLB_ISO_FRC_SW1_SET_N8_SET_POS      10
#define PMU_GLB_ISO_FRC_SW1_SET_N8_SET_LEN      1
#define PMU_GLB_ISO_FRC_SW1_SET_N8_SET_MASK     0x00000400
#define PMU_GLB_ISO_FRC_SW1_SET_N8_SET_VAL(w)   (((w) & 0x00000400) >> 10)
#define PMU_GLB_ISO_FRC_SW1_SET_N8_SET_SET(w)   w |= ( 0x1 << 10)
#define PMU_GLB_ISO_FRC_SW1_SET_N8_SET_CLR(w)   w &= ~( 0x1 << 10)

#define PMU_GLB_ISO_FRC_SW1_SET_N7_SET_POS      9
#define PMU_GLB_ISO_FRC_SW1_SET_N7_SET_LEN      1
#define PMU_GLB_ISO_FRC_SW1_SET_N7_SET_MASK     0x00000200
#define PMU_GLB_ISO_FRC_SW1_SET_N7_SET_VAL(w)   (((w) & 0x00000200) >> 9)
#define PMU_GLB_ISO_FRC_SW1_SET_N7_SET_SET(w)   w |= ( 0x1 << 9)
#define PMU_GLB_ISO_FRC_SW1_SET_N7_SET_CLR(w)   w &= ~( 0x1 << 9)

#define PMU_GLB_ISO_FRC_SW1_SET_N6_SET_POS      8
#define PMU_GLB_ISO_FRC_SW1_SET_N6_SET_LEN      1
#define PMU_GLB_ISO_FRC_SW1_SET_N6_SET_MASK     0x00000100
#define PMU_GLB_ISO_FRC_SW1_SET_N6_SET_VAL(w)   (((w) & 0x00000100) >> 8)
#define PMU_GLB_ISO_FRC_SW1_SET_N6_SET_SET(w)   w |= ( 0x1 << 8)
#define PMU_GLB_ISO_FRC_SW1_SET_N6_SET_CLR(w)   w &= ~( 0x1 << 8)

#define PMU_GLB_ISO_FRC_SW1_SET_N5_SET_POS      7
#define PMU_GLB_ISO_FRC_SW1_SET_N5_SET_LEN      1
#define PMU_GLB_ISO_FRC_SW1_SET_N5_SET_MASK     0x00000080
#define PMU_GLB_ISO_FRC_SW1_SET_N5_SET_VAL(w)   (((w) & 0x00000080) >> 7)
#define PMU_GLB_ISO_FRC_SW1_SET_N5_SET_SET(w)   w |= ( 0x1 << 7)
#define PMU_GLB_ISO_FRC_SW1_SET_N5_SET_CLR(w)   w &= ~( 0x1 << 7)

#define PMU_GLB_ISO_FRC_SW1_SET_N4_SET_POS      6
#define PMU_GLB_ISO_FRC_SW1_SET_N4_SET_LEN      1
#define PMU_GLB_ISO_FRC_SW1_SET_N4_SET_MASK     0x00000040
#define PMU_GLB_ISO_FRC_SW1_SET_N4_SET_VAL(w)   (((w) & 0x00000040) >> 6)
#define PMU_GLB_ISO_FRC_SW1_SET_N4_SET_SET(w)   w |= ( 0x1 << 6)
#define PMU_GLB_ISO_FRC_SW1_SET_N4_SET_CLR(w)   w &= ~( 0x1 << 6)

#define PMU_GLB_ISO_FRC_SW1_SET_N3_SET_POS      5
#define PMU_GLB_ISO_FRC_SW1_SET_N3_SET_LEN      1
#define PMU_GLB_ISO_FRC_SW1_SET_N3_SET_MASK     0x00000020
#define PMU_GLB_ISO_FRC_SW1_SET_N3_SET_VAL(w)   (((w) & 0x00000020) >> 5)
#define PMU_GLB_ISO_FRC_SW1_SET_N3_SET_SET(w)   w |= ( 0x1 << 5)
#define PMU_GLB_ISO_FRC_SW1_SET_N3_SET_CLR(w)   w &= ~( 0x1 << 5)

#define PMU_GLB_ISO_FRC_SW1_SET_N2_SET_POS      4
#define PMU_GLB_ISO_FRC_SW1_SET_N2_SET_LEN      1
#define PMU_GLB_ISO_FRC_SW1_SET_N2_SET_MASK     0x00000010
#define PMU_GLB_ISO_FRC_SW1_SET_N2_SET_VAL(w)   (((w) & 0x00000010) >> 4)
#define PMU_GLB_ISO_FRC_SW1_SET_N2_SET_SET(w)   w |= ( 0x1 << 4)
#define PMU_GLB_ISO_FRC_SW1_SET_N2_SET_CLR(w)   w &= ~( 0x1 << 4)

#define PMU_GLB_ISO_FRC_SW1_SET_N1_SET_POS      3
#define PMU_GLB_ISO_FRC_SW1_SET_N1_SET_LEN      1
#define PMU_GLB_ISO_FRC_SW1_SET_N1_SET_MASK     0x00000008
#define PMU_GLB_ISO_FRC_SW1_SET_N1_SET_VAL(w)   (((w) & 0x00000008) >> 3)
#define PMU_GLB_ISO_FRC_SW1_SET_N1_SET_SET(w)   w |= ( 0x1 << 3)
#define PMU_GLB_ISO_FRC_SW1_SET_N1_SET_CLR(w)   w &= ~( 0x1 << 3)

#define PMU_GLB_ISO_FRC_SW1_SET_N0_SET_POS      2
#define PMU_GLB_ISO_FRC_SW1_SET_N0_SET_LEN      1
#define PMU_GLB_ISO_FRC_SW1_SET_N0_SET_MASK     0x00000004
#define PMU_GLB_ISO_FRC_SW1_SET_N0_SET_VAL(w)   (((w) & 0x00000004) >> 2)
#define PMU_GLB_ISO_FRC_SW1_SET_N0_SET_SET(w)   w |= ( 0x1 << 2)
#define PMU_GLB_ISO_FRC_SW1_SET_N0_SET_CLR(w)   w &= ~( 0x1 << 2)

#define PMU_GLB_ISO_FRC_SW1_SET_MM_SET_POS      1
#define PMU_GLB_ISO_FRC_SW1_SET_MM_SET_LEN      1
#define PMU_GLB_ISO_FRC_SW1_SET_MM_SET_MASK     0x00000002
#define PMU_GLB_ISO_FRC_SW1_SET_MM_SET_VAL(w)   (((w) & 0x00000002) >> 1)
#define PMU_GLB_ISO_FRC_SW1_SET_MM_SET_SET(w)   w |= ( 0x1 << 1)
#define PMU_GLB_ISO_FRC_SW1_SET_MM_SET_CLR(w)   w &= ~( 0x1 << 1)

#define PMU_GLB_ISO_FRC_SW1_SET_PIPE_SET_POS      0
#define PMU_GLB_ISO_FRC_SW1_SET_PIPE_SET_LEN      1
#define PMU_GLB_ISO_FRC_SW1_SET_PIPE_SET_MASK     0x00000001
#define PMU_GLB_ISO_FRC_SW1_SET_PIPE_SET_VAL(w)   (((w) & 0x00000001) >> 0)
#define PMU_GLB_ISO_FRC_SW1_SET_PIPE_SET_SET(w)   w |= ( 0x1 << 0)
#define PMU_GLB_ISO_FRC_SW1_SET_PIPE_SET_CLR(w)   w &= ~( 0x1 << 0)

// REGISTER : ISO_FRC_SW1_CLR

#define PMU_GLB_ISO_FRC_SW1_CLR_ADDR      PMU_GLB_BASE_ADDR + 0x1D8
#define PMU_GLB_ISO_FRC_SW1_CLR_RSTVAL    0x0
#define PMU_GLB_ISO_FRC_SW1_CLR_ALL1      0x0
#define PMU_GLB_ISO_FRC_SW1_CLR_ALL0      0x0
#define PMU_GLB_ISO_FRC_SW1_CLR_ALL5A     0x0
#define PMU_GLB_ISO_FRC_SW1_CLR_ALLA5     0x0

// FIELD IN REGISTER : ISO_FRC_SW1_CLR

#define PMU_GLB_ISO_FRC_SW1_CLR_N8_CLR_POS      10
#define PMU_GLB_ISO_FRC_SW1_CLR_N8_CLR_LEN      1
#define PMU_GLB_ISO_FRC_SW1_CLR_N8_CLR_MASK     0x00000400
#define PMU_GLB_ISO_FRC_SW1_CLR_N8_CLR_VAL(w)   (((w) & 0x00000400) >> 10)
#define PMU_GLB_ISO_FRC_SW1_CLR_N8_CLR_SET(w)   w |= ( 0x1 << 10)
#define PMU_GLB_ISO_FRC_SW1_CLR_N8_CLR_CLR(w)   w &= ~( 0x1 << 10)

#define PMU_GLB_ISO_FRC_SW1_CLR_N7_CLR_POS      9
#define PMU_GLB_ISO_FRC_SW1_CLR_N7_CLR_LEN      1
#define PMU_GLB_ISO_FRC_SW1_CLR_N7_CLR_MASK     0x00000200
#define PMU_GLB_ISO_FRC_SW1_CLR_N7_CLR_VAL(w)   (((w) & 0x00000200) >> 9)
#define PMU_GLB_ISO_FRC_SW1_CLR_N7_CLR_SET(w)   w |= ( 0x1 << 9)
#define PMU_GLB_ISO_FRC_SW1_CLR_N7_CLR_CLR(w)   w &= ~( 0x1 << 9)

#define PMU_GLB_ISO_FRC_SW1_CLR_N6_CLR_POS      8
#define PMU_GLB_ISO_FRC_SW1_CLR_N6_CLR_LEN      1
#define PMU_GLB_ISO_FRC_SW1_CLR_N6_CLR_MASK     0x00000100
#define PMU_GLB_ISO_FRC_SW1_CLR_N6_CLR_VAL(w)   (((w) & 0x00000100) >> 8)
#define PMU_GLB_ISO_FRC_SW1_CLR_N6_CLR_SET(w)   w |= ( 0x1 << 8)
#define PMU_GLB_ISO_FRC_SW1_CLR_N6_CLR_CLR(w)   w &= ~( 0x1 << 8)

#define PMU_GLB_ISO_FRC_SW1_CLR_N5_CLR_POS      7
#define PMU_GLB_ISO_FRC_SW1_CLR_N5_CLR_LEN      1
#define PMU_GLB_ISO_FRC_SW1_CLR_N5_CLR_MASK     0x00000080
#define PMU_GLB_ISO_FRC_SW1_CLR_N5_CLR_VAL(w)   (((w) & 0x00000080) >> 7)
#define PMU_GLB_ISO_FRC_SW1_CLR_N5_CLR_SET(w)   w |= ( 0x1 << 7)
#define PMU_GLB_ISO_FRC_SW1_CLR_N5_CLR_CLR(w)   w &= ~( 0x1 << 7)

#define PMU_GLB_ISO_FRC_SW1_CLR_N4_CLR_POS      6
#define PMU_GLB_ISO_FRC_SW1_CLR_N4_CLR_LEN      1
#define PMU_GLB_ISO_FRC_SW1_CLR_N4_CLR_MASK     0x00000040
#define PMU_GLB_ISO_FRC_SW1_CLR_N4_CLR_VAL(w)   (((w) & 0x00000040) >> 6)
#define PMU_GLB_ISO_FRC_SW1_CLR_N4_CLR_SET(w)   w |= ( 0x1 << 6)
#define PMU_GLB_ISO_FRC_SW1_CLR_N4_CLR_CLR(w)   w &= ~( 0x1 << 6)

#define PMU_GLB_ISO_FRC_SW1_CLR_N3_CLR_POS      5
#define PMU_GLB_ISO_FRC_SW1_CLR_N3_CLR_LEN      1
#define PMU_GLB_ISO_FRC_SW1_CLR_N3_CLR_MASK     0x00000020
#define PMU_GLB_ISO_FRC_SW1_CLR_N3_CLR_VAL(w)   (((w) & 0x00000020) >> 5)
#define PMU_GLB_ISO_FRC_SW1_CLR_N3_CLR_SET(w)   w |= ( 0x1 << 5)
#define PMU_GLB_ISO_FRC_SW1_CLR_N3_CLR_CLR(w)   w &= ~( 0x1 << 5)

#define PMU_GLB_ISO_FRC_SW1_CLR_N2_CLR_POS      4
#define PMU_GLB_ISO_FRC_SW1_CLR_N2_CLR_LEN      1
#define PMU_GLB_ISO_FRC_SW1_CLR_N2_CLR_MASK     0x00000010
#define PMU_GLB_ISO_FRC_SW1_CLR_N2_CLR_VAL(w)   (((w) & 0x00000010) >> 4)
#define PMU_GLB_ISO_FRC_SW1_CLR_N2_CLR_SET(w)   w |= ( 0x1 << 4)
#define PMU_GLB_ISO_FRC_SW1_CLR_N2_CLR_CLR(w)   w &= ~( 0x1 << 4)

#define PMU_GLB_ISO_FRC_SW1_CLR_N1_CLR_POS      3
#define PMU_GLB_ISO_FRC_SW1_CLR_N1_CLR_LEN      1
#define PMU_GLB_ISO_FRC_SW1_CLR_N1_CLR_MASK     0x00000008
#define PMU_GLB_ISO_FRC_SW1_CLR_N1_CLR_VAL(w)   (((w) & 0x00000008) >> 3)
#define PMU_GLB_ISO_FRC_SW1_CLR_N1_CLR_SET(w)   w |= ( 0x1 << 3)
#define PMU_GLB_ISO_FRC_SW1_CLR_N1_CLR_CLR(w)   w &= ~( 0x1 << 3)

#define PMU_GLB_ISO_FRC_SW1_CLR_N0_CLR_POS      2
#define PMU_GLB_ISO_FRC_SW1_CLR_N0_CLR_LEN      1
#define PMU_GLB_ISO_FRC_SW1_CLR_N0_CLR_MASK     0x00000004
#define PMU_GLB_ISO_FRC_SW1_CLR_N0_CLR_VAL(w)   (((w) & 0x00000004) >> 2)
#define PMU_GLB_ISO_FRC_SW1_CLR_N0_CLR_SET(w)   w |= ( 0x1 << 2)
#define PMU_GLB_ISO_FRC_SW1_CLR_N0_CLR_CLR(w)   w &= ~( 0x1 << 2)

#define PMU_GLB_ISO_FRC_SW1_CLR_MM_CLR_POS      1
#define PMU_GLB_ISO_FRC_SW1_CLR_MM_CLR_LEN      1
#define PMU_GLB_ISO_FRC_SW1_CLR_MM_CLR_MASK     0x00000002
#define PMU_GLB_ISO_FRC_SW1_CLR_MM_CLR_VAL(w)   (((w) & 0x00000002) >> 1)
#define PMU_GLB_ISO_FRC_SW1_CLR_MM_CLR_SET(w)   w |= ( 0x1 << 1)
#define PMU_GLB_ISO_FRC_SW1_CLR_MM_CLR_CLR(w)   w &= ~( 0x1 << 1)

#define PMU_GLB_ISO_FRC_SW1_CLR_PIPE_CLR_POS      0
#define PMU_GLB_ISO_FRC_SW1_CLR_PIPE_CLR_LEN      1
#define PMU_GLB_ISO_FRC_SW1_CLR_PIPE_CLR_MASK     0x00000001
#define PMU_GLB_ISO_FRC_SW1_CLR_PIPE_CLR_VAL(w)   (((w) & 0x00000001) >> 0)
#define PMU_GLB_ISO_FRC_SW1_CLR_PIPE_CLR_SET(w)   w |= ( 0x1 << 0)
#define PMU_GLB_ISO_FRC_SW1_CLR_PIPE_CLR_CLR(w)   w &= ~( 0x1 << 0)

// REGISTER : CLK_FRC_EN_SET

#define PMU_GLB_CLK_FRC_EN_SET_ADDR      PMU_GLB_BASE_ADDR + 0x1DC
#define PMU_GLB_CLK_FRC_EN_SET_RSTVAL    0x0
#define PMU_GLB_CLK_FRC_EN_SET_ALL1      0x0
#define PMU_GLB_CLK_FRC_EN_SET_ALL0      0x0
#define PMU_GLB_CLK_FRC_EN_SET_ALL5A     0x0
#define PMU_GLB_CLK_FRC_EN_SET_ALLA5     0x0

// FIELD IN REGISTER : CLK_FRC_EN_SET

#define PMU_GLB_CLK_FRC_EN_SET_CPU7_SET_POS      11
#define PMU_GLB_CLK_FRC_EN_SET_CPU7_SET_LEN      1
#define PMU_GLB_CLK_FRC_EN_SET_CPU7_SET_MASK     0x00000800
#define PMU_GLB_CLK_FRC_EN_SET_CPU7_SET_VAL(w)   (((w) & 0x00000800) >> 11)
#define PMU_GLB_CLK_FRC_EN_SET_CPU7_SET_SET(w)   w |= ( 0x1 << 11)
#define PMU_GLB_CLK_FRC_EN_SET_CPU7_SET_CLR(w)   w &= ~( 0x1 << 11)

#define PMU_GLB_CLK_FRC_EN_SET_CPU6_SET_POS      10
#define PMU_GLB_CLK_FRC_EN_SET_CPU6_SET_LEN      1
#define PMU_GLB_CLK_FRC_EN_SET_CPU6_SET_MASK     0x00000400
#define PMU_GLB_CLK_FRC_EN_SET_CPU6_SET_VAL(w)   (((w) & 0x00000400) >> 10)
#define PMU_GLB_CLK_FRC_EN_SET_CPU6_SET_SET(w)   w |= ( 0x1 << 10)
#define PMU_GLB_CLK_FRC_EN_SET_CPU6_SET_CLR(w)   w &= ~( 0x1 << 10)

#define PMU_GLB_CLK_FRC_EN_SET_CPU5_SET_POS      9
#define PMU_GLB_CLK_FRC_EN_SET_CPU5_SET_LEN      1
#define PMU_GLB_CLK_FRC_EN_SET_CPU5_SET_MASK     0x00000200
#define PMU_GLB_CLK_FRC_EN_SET_CPU5_SET_VAL(w)   (((w) & 0x00000200) >> 9)
#define PMU_GLB_CLK_FRC_EN_SET_CPU5_SET_SET(w)   w |= ( 0x1 << 9)
#define PMU_GLB_CLK_FRC_EN_SET_CPU5_SET_CLR(w)   w &= ~( 0x1 << 9)

#define PMU_GLB_CLK_FRC_EN_SET_CPU4_SET_POS      8
#define PMU_GLB_CLK_FRC_EN_SET_CPU4_SET_LEN      1
#define PMU_GLB_CLK_FRC_EN_SET_CPU4_SET_MASK     0x00000100
#define PMU_GLB_CLK_FRC_EN_SET_CPU4_SET_VAL(w)   (((w) & 0x00000100) >> 8)
#define PMU_GLB_CLK_FRC_EN_SET_CPU4_SET_SET(w)   w |= ( 0x1 << 8)
#define PMU_GLB_CLK_FRC_EN_SET_CPU4_SET_CLR(w)   w &= ~( 0x1 << 8)

#define PMU_GLB_CLK_FRC_EN_SET_CPU3_SET_POS      7
#define PMU_GLB_CLK_FRC_EN_SET_CPU3_SET_LEN      1
#define PMU_GLB_CLK_FRC_EN_SET_CPU3_SET_MASK     0x00000080
#define PMU_GLB_CLK_FRC_EN_SET_CPU3_SET_VAL(w)   (((w) & 0x00000080) >> 7)
#define PMU_GLB_CLK_FRC_EN_SET_CPU3_SET_SET(w)   w |= ( 0x1 << 7)
#define PMU_GLB_CLK_FRC_EN_SET_CPU3_SET_CLR(w)   w &= ~( 0x1 << 7)

#define PMU_GLB_CLK_FRC_EN_SET_CPU2_SET_POS      6
#define PMU_GLB_CLK_FRC_EN_SET_CPU2_SET_LEN      1
#define PMU_GLB_CLK_FRC_EN_SET_CPU2_SET_MASK     0x00000040
#define PMU_GLB_CLK_FRC_EN_SET_CPU2_SET_VAL(w)   (((w) & 0x00000040) >> 6)
#define PMU_GLB_CLK_FRC_EN_SET_CPU2_SET_SET(w)   w |= ( 0x1 << 6)
#define PMU_GLB_CLK_FRC_EN_SET_CPU2_SET_CLR(w)   w &= ~( 0x1 << 6)

#define PMU_GLB_CLK_FRC_EN_SET_CPU1_SET_POS      5
#define PMU_GLB_CLK_FRC_EN_SET_CPU1_SET_LEN      1
#define PMU_GLB_CLK_FRC_EN_SET_CPU1_SET_MASK     0x00000020
#define PMU_GLB_CLK_FRC_EN_SET_CPU1_SET_VAL(w)   (((w) & 0x00000020) >> 5)
#define PMU_GLB_CLK_FRC_EN_SET_CPU1_SET_SET(w)   w |= ( 0x1 << 5)
#define PMU_GLB_CLK_FRC_EN_SET_CPU1_SET_CLR(w)   w &= ~( 0x1 << 5)

#define PMU_GLB_CLK_FRC_EN_SET_CPU0_SET_POS      4
#define PMU_GLB_CLK_FRC_EN_SET_CPU0_SET_LEN      1
#define PMU_GLB_CLK_FRC_EN_SET_CPU0_SET_MASK     0x00000010
#define PMU_GLB_CLK_FRC_EN_SET_CPU0_SET_VAL(w)   (((w) & 0x00000010) >> 4)
#define PMU_GLB_CLK_FRC_EN_SET_CPU0_SET_SET(w)   w |= ( 0x1 << 4)
#define PMU_GLB_CLK_FRC_EN_SET_CPU0_SET_CLR(w)   w &= ~( 0x1 << 4)

#define PMU_GLB_CLK_FRC_EN_SET_DSP_SET_POS      3
#define PMU_GLB_CLK_FRC_EN_SET_DSP_SET_LEN      1
#define PMU_GLB_CLK_FRC_EN_SET_DSP_SET_MASK     0x00000008
#define PMU_GLB_CLK_FRC_EN_SET_DSP_SET_VAL(w)   (((w) & 0x00000008) >> 3)
#define PMU_GLB_CLK_FRC_EN_SET_DSP_SET_SET(w)   w |= ( 0x1 << 3)
#define PMU_GLB_CLK_FRC_EN_SET_DSP_SET_CLR(w)   w &= ~( 0x1 << 3)

#define PMU_GLB_CLK_FRC_EN_SET_VDEC_SET_POS      2
#define PMU_GLB_CLK_FRC_EN_SET_VDEC_SET_LEN      1
#define PMU_GLB_CLK_FRC_EN_SET_VDEC_SET_MASK     0x00000004
#define PMU_GLB_CLK_FRC_EN_SET_VDEC_SET_VAL(w)   (((w) & 0x00000004) >> 2)
#define PMU_GLB_CLK_FRC_EN_SET_VDEC_SET_SET(w)   w |= ( 0x1 << 2)
#define PMU_GLB_CLK_FRC_EN_SET_VDEC_SET_CLR(w)   w &= ~( 0x1 << 2)

#define PMU_GLB_CLK_FRC_EN_SET_VENC1_SET_POS      1
#define PMU_GLB_CLK_FRC_EN_SET_VENC1_SET_LEN      1
#define PMU_GLB_CLK_FRC_EN_SET_VENC1_SET_MASK     0x00000002
#define PMU_GLB_CLK_FRC_EN_SET_VENC1_SET_VAL(w)   (((w) & 0x00000002) >> 1)
#define PMU_GLB_CLK_FRC_EN_SET_VENC1_SET_SET(w)   w |= ( 0x1 << 1)
#define PMU_GLB_CLK_FRC_EN_SET_VENC1_SET_CLR(w)   w &= ~( 0x1 << 1)

#define PMU_GLB_CLK_FRC_EN_SET_VENC0_SET_POS      0
#define PMU_GLB_CLK_FRC_EN_SET_VENC0_SET_LEN      1
#define PMU_GLB_CLK_FRC_EN_SET_VENC0_SET_MASK     0x00000001
#define PMU_GLB_CLK_FRC_EN_SET_VENC0_SET_VAL(w)   (((w) & 0x00000001) >> 0)
#define PMU_GLB_CLK_FRC_EN_SET_VENC0_SET_SET(w)   w |= ( 0x1 << 0)
#define PMU_GLB_CLK_FRC_EN_SET_VENC0_SET_CLR(w)   w &= ~( 0x1 << 0)

// REGISTER : CLK_FRC_EN_CLR

#define PMU_GLB_CLK_FRC_EN_CLR_ADDR      PMU_GLB_BASE_ADDR + 0x1E0
#define PMU_GLB_CLK_FRC_EN_CLR_RSTVAL    0x0
#define PMU_GLB_CLK_FRC_EN_CLR_ALL1      0x0
#define PMU_GLB_CLK_FRC_EN_CLR_ALL0      0x0
#define PMU_GLB_CLK_FRC_EN_CLR_ALL5A     0x0
#define PMU_GLB_CLK_FRC_EN_CLR_ALLA5     0x0

// FIELD IN REGISTER : CLK_FRC_EN_CLR

#define PMU_GLB_CLK_FRC_EN_CLR_CPU7_CLR_POS      11
#define PMU_GLB_CLK_FRC_EN_CLR_CPU7_CLR_LEN      1
#define PMU_GLB_CLK_FRC_EN_CLR_CPU7_CLR_MASK     0x00000800
#define PMU_GLB_CLK_FRC_EN_CLR_CPU7_CLR_VAL(w)   (((w) & 0x00000800) >> 11)
#define PMU_GLB_CLK_FRC_EN_CLR_CPU7_CLR_SET(w)   w |= ( 0x1 << 11)
#define PMU_GLB_CLK_FRC_EN_CLR_CPU7_CLR_CLR(w)   w &= ~( 0x1 << 11)

#define PMU_GLB_CLK_FRC_EN_CLR_CPU6_CLR_POS      10
#define PMU_GLB_CLK_FRC_EN_CLR_CPU6_CLR_LEN      1
#define PMU_GLB_CLK_FRC_EN_CLR_CPU6_CLR_MASK     0x00000400
#define PMU_GLB_CLK_FRC_EN_CLR_CPU6_CLR_VAL(w)   (((w) & 0x00000400) >> 10)
#define PMU_GLB_CLK_FRC_EN_CLR_CPU6_CLR_SET(w)   w |= ( 0x1 << 10)
#define PMU_GLB_CLK_FRC_EN_CLR_CPU6_CLR_CLR(w)   w &= ~( 0x1 << 10)

#define PMU_GLB_CLK_FRC_EN_CLR_CPU5_CLR_POS      9
#define PMU_GLB_CLK_FRC_EN_CLR_CPU5_CLR_LEN      1
#define PMU_GLB_CLK_FRC_EN_CLR_CPU5_CLR_MASK     0x00000200
#define PMU_GLB_CLK_FRC_EN_CLR_CPU5_CLR_VAL(w)   (((w) & 0x00000200) >> 9)
#define PMU_GLB_CLK_FRC_EN_CLR_CPU5_CLR_SET(w)   w |= ( 0x1 << 9)
#define PMU_GLB_CLK_FRC_EN_CLR_CPU5_CLR_CLR(w)   w &= ~( 0x1 << 9)

#define PMU_GLB_CLK_FRC_EN_CLR_CPU4_CLR_POS      8
#define PMU_GLB_CLK_FRC_EN_CLR_CPU4_CLR_LEN      1
#define PMU_GLB_CLK_FRC_EN_CLR_CPU4_CLR_MASK     0x00000100
#define PMU_GLB_CLK_FRC_EN_CLR_CPU4_CLR_VAL(w)   (((w) & 0x00000100) >> 8)
#define PMU_GLB_CLK_FRC_EN_CLR_CPU4_CLR_SET(w)   w |= ( 0x1 << 8)
#define PMU_GLB_CLK_FRC_EN_CLR_CPU4_CLR_CLR(w)   w &= ~( 0x1 << 8)

#define PMU_GLB_CLK_FRC_EN_CLR_CPU3_CLR_POS      7
#define PMU_GLB_CLK_FRC_EN_CLR_CPU3_CLR_LEN      1
#define PMU_GLB_CLK_FRC_EN_CLR_CPU3_CLR_MASK     0x00000080
#define PMU_GLB_CLK_FRC_EN_CLR_CPU3_CLR_VAL(w)   (((w) & 0x00000080) >> 7)
#define PMU_GLB_CLK_FRC_EN_CLR_CPU3_CLR_SET(w)   w |= ( 0x1 << 7)
#define PMU_GLB_CLK_FRC_EN_CLR_CPU3_CLR_CLR(w)   w &= ~( 0x1 << 7)

#define PMU_GLB_CLK_FRC_EN_CLR_CPU2_CLR_POS      6
#define PMU_GLB_CLK_FRC_EN_CLR_CPU2_CLR_LEN      1
#define PMU_GLB_CLK_FRC_EN_CLR_CPU2_CLR_MASK     0x00000040
#define PMU_GLB_CLK_FRC_EN_CLR_CPU2_CLR_VAL(w)   (((w) & 0x00000040) >> 6)
#define PMU_GLB_CLK_FRC_EN_CLR_CPU2_CLR_SET(w)   w |= ( 0x1 << 6)
#define PMU_GLB_CLK_FRC_EN_CLR_CPU2_CLR_CLR(w)   w &= ~( 0x1 << 6)

#define PMU_GLB_CLK_FRC_EN_CLR_CPU1_CLR_POS      5
#define PMU_GLB_CLK_FRC_EN_CLR_CPU1_CLR_LEN      1
#define PMU_GLB_CLK_FRC_EN_CLR_CPU1_CLR_MASK     0x00000020
#define PMU_GLB_CLK_FRC_EN_CLR_CPU1_CLR_VAL(w)   (((w) & 0x00000020) >> 5)
#define PMU_GLB_CLK_FRC_EN_CLR_CPU1_CLR_SET(w)   w |= ( 0x1 << 5)
#define PMU_GLB_CLK_FRC_EN_CLR_CPU1_CLR_CLR(w)   w &= ~( 0x1 << 5)

#define PMU_GLB_CLK_FRC_EN_CLR_CPU0_CLR_POS      4
#define PMU_GLB_CLK_FRC_EN_CLR_CPU0_CLR_LEN      1
#define PMU_GLB_CLK_FRC_EN_CLR_CPU0_CLR_MASK     0x00000010
#define PMU_GLB_CLK_FRC_EN_CLR_CPU0_CLR_VAL(w)   (((w) & 0x00000010) >> 4)
#define PMU_GLB_CLK_FRC_EN_CLR_CPU0_CLR_SET(w)   w |= ( 0x1 << 4)
#define PMU_GLB_CLK_FRC_EN_CLR_CPU0_CLR_CLR(w)   w &= ~( 0x1 << 4)

#define PMU_GLB_CLK_FRC_EN_CLR_DSP_CLR_POS      3
#define PMU_GLB_CLK_FRC_EN_CLR_DSP_CLR_LEN      1
#define PMU_GLB_CLK_FRC_EN_CLR_DSP_CLR_MASK     0x00000008
#define PMU_GLB_CLK_FRC_EN_CLR_DSP_CLR_VAL(w)   (((w) & 0x00000008) >> 3)
#define PMU_GLB_CLK_FRC_EN_CLR_DSP_CLR_SET(w)   w |= ( 0x1 << 3)
#define PMU_GLB_CLK_FRC_EN_CLR_DSP_CLR_CLR(w)   w &= ~( 0x1 << 3)

#define PMU_GLB_CLK_FRC_EN_CLR_VDEC_CLR_POS      2
#define PMU_GLB_CLK_FRC_EN_CLR_VDEC_CLR_LEN      1
#define PMU_GLB_CLK_FRC_EN_CLR_VDEC_CLR_MASK     0x00000004
#define PMU_GLB_CLK_FRC_EN_CLR_VDEC_CLR_VAL(w)   (((w) & 0x00000004) >> 2)
#define PMU_GLB_CLK_FRC_EN_CLR_VDEC_CLR_SET(w)   w |= ( 0x1 << 2)
#define PMU_GLB_CLK_FRC_EN_CLR_VDEC_CLR_CLR(w)   w &= ~( 0x1 << 2)

#define PMU_GLB_CLK_FRC_EN_CLR_VENC1_CLR_POS      1
#define PMU_GLB_CLK_FRC_EN_CLR_VENC1_CLR_LEN      1
#define PMU_GLB_CLK_FRC_EN_CLR_VENC1_CLR_MASK     0x00000002
#define PMU_GLB_CLK_FRC_EN_CLR_VENC1_CLR_VAL(w)   (((w) & 0x00000002) >> 1)
#define PMU_GLB_CLK_FRC_EN_CLR_VENC1_CLR_SET(w)   w |= ( 0x1 << 1)
#define PMU_GLB_CLK_FRC_EN_CLR_VENC1_CLR_CLR(w)   w &= ~( 0x1 << 1)

#define PMU_GLB_CLK_FRC_EN_CLR_VENC0_CLR_POS      0
#define PMU_GLB_CLK_FRC_EN_CLR_VENC0_CLR_LEN      1
#define PMU_GLB_CLK_FRC_EN_CLR_VENC0_CLR_MASK     0x00000001
#define PMU_GLB_CLK_FRC_EN_CLR_VENC0_CLR_VAL(w)   (((w) & 0x00000001) >> 0)
#define PMU_GLB_CLK_FRC_EN_CLR_VENC0_CLR_SET(w)   w |= ( 0x1 << 0)
#define PMU_GLB_CLK_FRC_EN_CLR_VENC0_CLR_CLR(w)   w &= ~( 0x1 << 0)

// REGISTER : CLK_FRC_EN1_SET

#define PMU_GLB_CLK_FRC_EN1_SET_ADDR      PMU_GLB_BASE_ADDR + 0x1E4
#define PMU_GLB_CLK_FRC_EN1_SET_RSTVAL    0x0
#define PMU_GLB_CLK_FRC_EN1_SET_ALL1      0x0
#define PMU_GLB_CLK_FRC_EN1_SET_ALL0      0x0
#define PMU_GLB_CLK_FRC_EN1_SET_ALL5A     0x0
#define PMU_GLB_CLK_FRC_EN1_SET_ALLA5     0x0

// FIELD IN REGISTER : CLK_FRC_EN1_SET

#define PMU_GLB_CLK_FRC_EN1_SET_SEN_SET_POS      18
#define PMU_GLB_CLK_FRC_EN1_SET_SEN_SET_LEN      1
#define PMU_GLB_CLK_FRC_EN1_SET_SEN_SET_MASK     0x00040000
#define PMU_GLB_CLK_FRC_EN1_SET_SEN_SET_VAL(w)   (((w) & 0x00040000) >> 18)
#define PMU_GLB_CLK_FRC_EN1_SET_SEN_SET_SET(w)   w |= ( 0x1 << 18)
#define PMU_GLB_CLK_FRC_EN1_SET_SEN_SET_CLR(w)   w &= ~( 0x1 << 18)

#define PMU_GLB_CLK_FRC_EN1_SET_ISP_SET_POS      17
#define PMU_GLB_CLK_FRC_EN1_SET_ISP_SET_LEN      1
#define PMU_GLB_CLK_FRC_EN1_SET_ISP_SET_MASK     0x00020000
#define PMU_GLB_CLK_FRC_EN1_SET_ISP_SET_VAL(w)   (((w) & 0x00020000) >> 17)
#define PMU_GLB_CLK_FRC_EN1_SET_ISP_SET_SET(w)   w |= ( 0x1 << 17)
#define PMU_GLB_CLK_FRC_EN1_SET_ISP_SET_CLR(w)   w &= ~( 0x1 << 17)

#define PMU_GLB_CLK_FRC_EN1_SET_CPU_SET_POS      16
#define PMU_GLB_CLK_FRC_EN1_SET_CPU_SET_LEN      1
#define PMU_GLB_CLK_FRC_EN1_SET_CPU_SET_MASK     0x00010000
#define PMU_GLB_CLK_FRC_EN1_SET_CPU_SET_VAL(w)   (((w) & 0x00010000) >> 16)
#define PMU_GLB_CLK_FRC_EN1_SET_CPU_SET_SET(w)   w |= ( 0x1 << 16)
#define PMU_GLB_CLK_FRC_EN1_SET_CPU_SET_CLR(w)   w &= ~( 0x1 << 16)

#define PMU_GLB_CLK_FRC_EN1_SET_NPU_SET_POS      15
#define PMU_GLB_CLK_FRC_EN1_SET_NPU_SET_LEN      1
#define PMU_GLB_CLK_FRC_EN1_SET_NPU_SET_MASK     0x00008000
#define PMU_GLB_CLK_FRC_EN1_SET_NPU_SET_VAL(w)   (((w) & 0x00008000) >> 15)
#define PMU_GLB_CLK_FRC_EN1_SET_NPU_SET_SET(w)   w |= ( 0x1 << 15)
#define PMU_GLB_CLK_FRC_EN1_SET_NPU_SET_CLR(w)   w &= ~( 0x1 << 15)

#define PMU_GLB_CLK_FRC_EN1_SET_N8_SET_POS      14
#define PMU_GLB_CLK_FRC_EN1_SET_N8_SET_LEN      1
#define PMU_GLB_CLK_FRC_EN1_SET_N8_SET_MASK     0x00004000
#define PMU_GLB_CLK_FRC_EN1_SET_N8_SET_VAL(w)   (((w) & 0x00004000) >> 14)
#define PMU_GLB_CLK_FRC_EN1_SET_N8_SET_SET(w)   w |= ( 0x1 << 14)
#define PMU_GLB_CLK_FRC_EN1_SET_N8_SET_CLR(w)   w &= ~( 0x1 << 14)

#define PMU_GLB_CLK_FRC_EN1_SET_N7_SET_POS      13
#define PMU_GLB_CLK_FRC_EN1_SET_N7_SET_LEN      1
#define PMU_GLB_CLK_FRC_EN1_SET_N7_SET_MASK     0x00002000
#define PMU_GLB_CLK_FRC_EN1_SET_N7_SET_VAL(w)   (((w) & 0x00002000) >> 13)
#define PMU_GLB_CLK_FRC_EN1_SET_N7_SET_SET(w)   w |= ( 0x1 << 13)
#define PMU_GLB_CLK_FRC_EN1_SET_N7_SET_CLR(w)   w &= ~( 0x1 << 13)

#define PMU_GLB_CLK_FRC_EN1_SET_N6_SET_POS      12
#define PMU_GLB_CLK_FRC_EN1_SET_N6_SET_LEN      1
#define PMU_GLB_CLK_FRC_EN1_SET_N6_SET_MASK     0x00001000
#define PMU_GLB_CLK_FRC_EN1_SET_N6_SET_VAL(w)   (((w) & 0x00001000) >> 12)
#define PMU_GLB_CLK_FRC_EN1_SET_N6_SET_SET(w)   w |= ( 0x1 << 12)
#define PMU_GLB_CLK_FRC_EN1_SET_N6_SET_CLR(w)   w &= ~( 0x1 << 12)

#define PMU_GLB_CLK_FRC_EN1_SET_N5_SET_POS      11
#define PMU_GLB_CLK_FRC_EN1_SET_N5_SET_LEN      1
#define PMU_GLB_CLK_FRC_EN1_SET_N5_SET_MASK     0x00000800
#define PMU_GLB_CLK_FRC_EN1_SET_N5_SET_VAL(w)   (((w) & 0x00000800) >> 11)
#define PMU_GLB_CLK_FRC_EN1_SET_N5_SET_SET(w)   w |= ( 0x1 << 11)
#define PMU_GLB_CLK_FRC_EN1_SET_N5_SET_CLR(w)   w &= ~( 0x1 << 11)

#define PMU_GLB_CLK_FRC_EN1_SET_N4_SET_POS      10
#define PMU_GLB_CLK_FRC_EN1_SET_N4_SET_LEN      1
#define PMU_GLB_CLK_FRC_EN1_SET_N4_SET_MASK     0x00000400
#define PMU_GLB_CLK_FRC_EN1_SET_N4_SET_VAL(w)   (((w) & 0x00000400) >> 10)
#define PMU_GLB_CLK_FRC_EN1_SET_N4_SET_SET(w)   w |= ( 0x1 << 10)
#define PMU_GLB_CLK_FRC_EN1_SET_N4_SET_CLR(w)   w &= ~( 0x1 << 10)

#define PMU_GLB_CLK_FRC_EN1_SET_N3_SET_POS      9
#define PMU_GLB_CLK_FRC_EN1_SET_N3_SET_LEN      1
#define PMU_GLB_CLK_FRC_EN1_SET_N3_SET_MASK     0x00000200
#define PMU_GLB_CLK_FRC_EN1_SET_N3_SET_VAL(w)   (((w) & 0x00000200) >> 9)
#define PMU_GLB_CLK_FRC_EN1_SET_N3_SET_SET(w)   w |= ( 0x1 << 9)
#define PMU_GLB_CLK_FRC_EN1_SET_N3_SET_CLR(w)   w &= ~( 0x1 << 9)

#define PMU_GLB_CLK_FRC_EN1_SET_N2_SET_POS      8
#define PMU_GLB_CLK_FRC_EN1_SET_N2_SET_LEN      1
#define PMU_GLB_CLK_FRC_EN1_SET_N2_SET_MASK     0x00000100
#define PMU_GLB_CLK_FRC_EN1_SET_N2_SET_VAL(w)   (((w) & 0x00000100) >> 8)
#define PMU_GLB_CLK_FRC_EN1_SET_N2_SET_SET(w)   w |= ( 0x1 << 8)
#define PMU_GLB_CLK_FRC_EN1_SET_N2_SET_CLR(w)   w &= ~( 0x1 << 8)

#define PMU_GLB_CLK_FRC_EN1_SET_N1_SET_POS      7
#define PMU_GLB_CLK_FRC_EN1_SET_N1_SET_LEN      1
#define PMU_GLB_CLK_FRC_EN1_SET_N1_SET_MASK     0x00000080
#define PMU_GLB_CLK_FRC_EN1_SET_N1_SET_VAL(w)   (((w) & 0x00000080) >> 7)
#define PMU_GLB_CLK_FRC_EN1_SET_N1_SET_SET(w)   w |= ( 0x1 << 7)
#define PMU_GLB_CLK_FRC_EN1_SET_N1_SET_CLR(w)   w &= ~( 0x1 << 7)

#define PMU_GLB_CLK_FRC_EN1_SET_N0_SET_POS      6
#define PMU_GLB_CLK_FRC_EN1_SET_N0_SET_LEN      1
#define PMU_GLB_CLK_FRC_EN1_SET_N0_SET_MASK     0x00000040
#define PMU_GLB_CLK_FRC_EN1_SET_N0_SET_VAL(w)   (((w) & 0x00000040) >> 6)
#define PMU_GLB_CLK_FRC_EN1_SET_N0_SET_SET(w)   w |= ( 0x1 << 6)
#define PMU_GLB_CLK_FRC_EN1_SET_N0_SET_CLR(w)   w &= ~( 0x1 << 6)

#define PMU_GLB_CLK_FRC_EN1_SET_MM_SET_POS      5
#define PMU_GLB_CLK_FRC_EN1_SET_MM_SET_LEN      1
#define PMU_GLB_CLK_FRC_EN1_SET_MM_SET_MASK     0x00000020
#define PMU_GLB_CLK_FRC_EN1_SET_MM_SET_VAL(w)   (((w) & 0x00000020) >> 5)
#define PMU_GLB_CLK_FRC_EN1_SET_MM_SET_SET(w)   w |= ( 0x1 << 5)
#define PMU_GLB_CLK_FRC_EN1_SET_MM_SET_CLR(w)   w &= ~( 0x1 << 5)

#define PMU_GLB_CLK_FRC_EN1_SET_PERIP_SET_POS      4
#define PMU_GLB_CLK_FRC_EN1_SET_PERIP_SET_LEN      1
#define PMU_GLB_CLK_FRC_EN1_SET_PERIP_SET_MASK     0x00000010
#define PMU_GLB_CLK_FRC_EN1_SET_PERIP_SET_VAL(w)   (((w) & 0x00000010) >> 4)
#define PMU_GLB_CLK_FRC_EN1_SET_PERIP_SET_SET(w)   w |= ( 0x1 << 4)
#define PMU_GLB_CLK_FRC_EN1_SET_PERIP_SET_CLR(w)   w &= ~( 0x1 << 4)

#define PMU_GLB_CLK_FRC_EN1_SET_PIPE_SET_POS      3
#define PMU_GLB_CLK_FRC_EN1_SET_PIPE_SET_LEN      1
#define PMU_GLB_CLK_FRC_EN1_SET_PIPE_SET_MASK     0x00000008
#define PMU_GLB_CLK_FRC_EN1_SET_PIPE_SET_VAL(w)   (((w) & 0x00000008) >> 3)
#define PMU_GLB_CLK_FRC_EN1_SET_PIPE_SET_SET(w)   w |= ( 0x1 << 3)
#define PMU_GLB_CLK_FRC_EN1_SET_PIPE_SET_CLR(w)   w &= ~( 0x1 << 3)

#define PMU_GLB_CLK_FRC_EN1_SET_FLASH_SET_POS      2
#define PMU_GLB_CLK_FRC_EN1_SET_FLASH_SET_LEN      1
#define PMU_GLB_CLK_FRC_EN1_SET_FLASH_SET_MASK     0x00000004
#define PMU_GLB_CLK_FRC_EN1_SET_FLASH_SET_VAL(w)   (((w) & 0x00000004) >> 2)
#define PMU_GLB_CLK_FRC_EN1_SET_FLASH_SET_SET(w)   w |= ( 0x1 << 2)
#define PMU_GLB_CLK_FRC_EN1_SET_FLASH_SET_CLR(w)   w &= ~( 0x1 << 2)

#define PMU_GLB_CLK_FRC_EN1_SET_DDR1_SET_POS      1
#define PMU_GLB_CLK_FRC_EN1_SET_DDR1_SET_LEN      1
#define PMU_GLB_CLK_FRC_EN1_SET_DDR1_SET_MASK     0x00000002
#define PMU_GLB_CLK_FRC_EN1_SET_DDR1_SET_VAL(w)   (((w) & 0x00000002) >> 1)
#define PMU_GLB_CLK_FRC_EN1_SET_DDR1_SET_SET(w)   w |= ( 0x1 << 1)
#define PMU_GLB_CLK_FRC_EN1_SET_DDR1_SET_CLR(w)   w &= ~( 0x1 << 1)

#define PMU_GLB_CLK_FRC_EN1_SET_DDR0_SET_POS      0
#define PMU_GLB_CLK_FRC_EN1_SET_DDR0_SET_LEN      1
#define PMU_GLB_CLK_FRC_EN1_SET_DDR0_SET_MASK     0x00000001
#define PMU_GLB_CLK_FRC_EN1_SET_DDR0_SET_VAL(w)   (((w) & 0x00000001) >> 0)
#define PMU_GLB_CLK_FRC_EN1_SET_DDR0_SET_SET(w)   w |= ( 0x1 << 0)
#define PMU_GLB_CLK_FRC_EN1_SET_DDR0_SET_CLR(w)   w &= ~( 0x1 << 0)

// REGISTER : CLK_FRC_EN1_CLR

#define PMU_GLB_CLK_FRC_EN1_CLR_ADDR      PMU_GLB_BASE_ADDR + 0x1E8
#define PMU_GLB_CLK_FRC_EN1_CLR_RSTVAL    0x0
#define PMU_GLB_CLK_FRC_EN1_CLR_ALL1      0x0
#define PMU_GLB_CLK_FRC_EN1_CLR_ALL0      0x0
#define PMU_GLB_CLK_FRC_EN1_CLR_ALL5A     0x0
#define PMU_GLB_CLK_FRC_EN1_CLR_ALLA5     0x0

// FIELD IN REGISTER : CLK_FRC_EN1_CLR

#define PMU_GLB_CLK_FRC_EN1_CLR_SEN_CLR_POS      18
#define PMU_GLB_CLK_FRC_EN1_CLR_SEN_CLR_LEN      1
#define PMU_GLB_CLK_FRC_EN1_CLR_SEN_CLR_MASK     0x00040000
#define PMU_GLB_CLK_FRC_EN1_CLR_SEN_CLR_VAL(w)   (((w) & 0x00040000) >> 18)
#define PMU_GLB_CLK_FRC_EN1_CLR_SEN_CLR_SET(w)   w |= ( 0x1 << 18)
#define PMU_GLB_CLK_FRC_EN1_CLR_SEN_CLR_CLR(w)   w &= ~( 0x1 << 18)

#define PMU_GLB_CLK_FRC_EN1_CLR_ISP_CLR_POS      17
#define PMU_GLB_CLK_FRC_EN1_CLR_ISP_CLR_LEN      1
#define PMU_GLB_CLK_FRC_EN1_CLR_ISP_CLR_MASK     0x00020000
#define PMU_GLB_CLK_FRC_EN1_CLR_ISP_CLR_VAL(w)   (((w) & 0x00020000) >> 17)
#define PMU_GLB_CLK_FRC_EN1_CLR_ISP_CLR_SET(w)   w |= ( 0x1 << 17)
#define PMU_GLB_CLK_FRC_EN1_CLR_ISP_CLR_CLR(w)   w &= ~( 0x1 << 17)

#define PMU_GLB_CLK_FRC_EN1_CLR_CPU_CLR_POS      16
#define PMU_GLB_CLK_FRC_EN1_CLR_CPU_CLR_LEN      1
#define PMU_GLB_CLK_FRC_EN1_CLR_CPU_CLR_MASK     0x00010000
#define PMU_GLB_CLK_FRC_EN1_CLR_CPU_CLR_VAL(w)   (((w) & 0x00010000) >> 16)
#define PMU_GLB_CLK_FRC_EN1_CLR_CPU_CLR_SET(w)   w |= ( 0x1 << 16)
#define PMU_GLB_CLK_FRC_EN1_CLR_CPU_CLR_CLR(w)   w &= ~( 0x1 << 16)

#define PMU_GLB_CLK_FRC_EN1_CLR_NPU_CLR_POS      15
#define PMU_GLB_CLK_FRC_EN1_CLR_NPU_CLR_LEN      1
#define PMU_GLB_CLK_FRC_EN1_CLR_NPU_CLR_MASK     0x00008000
#define PMU_GLB_CLK_FRC_EN1_CLR_NPU_CLR_VAL(w)   (((w) & 0x00008000) >> 15)
#define PMU_GLB_CLK_FRC_EN1_CLR_NPU_CLR_SET(w)   w |= ( 0x1 << 15)
#define PMU_GLB_CLK_FRC_EN1_CLR_NPU_CLR_CLR(w)   w &= ~( 0x1 << 15)

#define PMU_GLB_CLK_FRC_EN1_CLR_N8_CLR_POS      14
#define PMU_GLB_CLK_FRC_EN1_CLR_N8_CLR_LEN      1
#define PMU_GLB_CLK_FRC_EN1_CLR_N8_CLR_MASK     0x00004000
#define PMU_GLB_CLK_FRC_EN1_CLR_N8_CLR_VAL(w)   (((w) & 0x00004000) >> 14)
#define PMU_GLB_CLK_FRC_EN1_CLR_N8_CLR_SET(w)   w |= ( 0x1 << 14)
#define PMU_GLB_CLK_FRC_EN1_CLR_N8_CLR_CLR(w)   w &= ~( 0x1 << 14)

#define PMU_GLB_CLK_FRC_EN1_CLR_N7_CLR_POS      13
#define PMU_GLB_CLK_FRC_EN1_CLR_N7_CLR_LEN      1
#define PMU_GLB_CLK_FRC_EN1_CLR_N7_CLR_MASK     0x00002000
#define PMU_GLB_CLK_FRC_EN1_CLR_N7_CLR_VAL(w)   (((w) & 0x00002000) >> 13)
#define PMU_GLB_CLK_FRC_EN1_CLR_N7_CLR_SET(w)   w |= ( 0x1 << 13)
#define PMU_GLB_CLK_FRC_EN1_CLR_N7_CLR_CLR(w)   w &= ~( 0x1 << 13)

#define PMU_GLB_CLK_FRC_EN1_CLR_N6_CLR_POS      12
#define PMU_GLB_CLK_FRC_EN1_CLR_N6_CLR_LEN      1
#define PMU_GLB_CLK_FRC_EN1_CLR_N6_CLR_MASK     0x00001000
#define PMU_GLB_CLK_FRC_EN1_CLR_N6_CLR_VAL(w)   (((w) & 0x00001000) >> 12)
#define PMU_GLB_CLK_FRC_EN1_CLR_N6_CLR_SET(w)   w |= ( 0x1 << 12)
#define PMU_GLB_CLK_FRC_EN1_CLR_N6_CLR_CLR(w)   w &= ~( 0x1 << 12)

#define PMU_GLB_CLK_FRC_EN1_CLR_N5_CLR_POS      11
#define PMU_GLB_CLK_FRC_EN1_CLR_N5_CLR_LEN      1
#define PMU_GLB_CLK_FRC_EN1_CLR_N5_CLR_MASK     0x00000800
#define PMU_GLB_CLK_FRC_EN1_CLR_N5_CLR_VAL(w)   (((w) & 0x00000800) >> 11)
#define PMU_GLB_CLK_FRC_EN1_CLR_N5_CLR_SET(w)   w |= ( 0x1 << 11)
#define PMU_GLB_CLK_FRC_EN1_CLR_N5_CLR_CLR(w)   w &= ~( 0x1 << 11)

#define PMU_GLB_CLK_FRC_EN1_CLR_N4_CLR_POS      10
#define PMU_GLB_CLK_FRC_EN1_CLR_N4_CLR_LEN      1
#define PMU_GLB_CLK_FRC_EN1_CLR_N4_CLR_MASK     0x00000400
#define PMU_GLB_CLK_FRC_EN1_CLR_N4_CLR_VAL(w)   (((w) & 0x00000400) >> 10)
#define PMU_GLB_CLK_FRC_EN1_CLR_N4_CLR_SET(w)   w |= ( 0x1 << 10)
#define PMU_GLB_CLK_FRC_EN1_CLR_N4_CLR_CLR(w)   w &= ~( 0x1 << 10)

#define PMU_GLB_CLK_FRC_EN1_CLR_N3_CLR_POS      9
#define PMU_GLB_CLK_FRC_EN1_CLR_N3_CLR_LEN      1
#define PMU_GLB_CLK_FRC_EN1_CLR_N3_CLR_MASK     0x00000200
#define PMU_GLB_CLK_FRC_EN1_CLR_N3_CLR_VAL(w)   (((w) & 0x00000200) >> 9)
#define PMU_GLB_CLK_FRC_EN1_CLR_N3_CLR_SET(w)   w |= ( 0x1 << 9)
#define PMU_GLB_CLK_FRC_EN1_CLR_N3_CLR_CLR(w)   w &= ~( 0x1 << 9)

#define PMU_GLB_CLK_FRC_EN1_CLR_N2_CLR_POS      8
#define PMU_GLB_CLK_FRC_EN1_CLR_N2_CLR_LEN      1
#define PMU_GLB_CLK_FRC_EN1_CLR_N2_CLR_MASK     0x00000100
#define PMU_GLB_CLK_FRC_EN1_CLR_N2_CLR_VAL(w)   (((w) & 0x00000100) >> 8)
#define PMU_GLB_CLK_FRC_EN1_CLR_N2_CLR_SET(w)   w |= ( 0x1 << 8)
#define PMU_GLB_CLK_FRC_EN1_CLR_N2_CLR_CLR(w)   w &= ~( 0x1 << 8)

#define PMU_GLB_CLK_FRC_EN1_CLR_N1_CLR_POS      7
#define PMU_GLB_CLK_FRC_EN1_CLR_N1_CLR_LEN      1
#define PMU_GLB_CLK_FRC_EN1_CLR_N1_CLR_MASK     0x00000080
#define PMU_GLB_CLK_FRC_EN1_CLR_N1_CLR_VAL(w)   (((w) & 0x00000080) >> 7)
#define PMU_GLB_CLK_FRC_EN1_CLR_N1_CLR_SET(w)   w |= ( 0x1 << 7)
#define PMU_GLB_CLK_FRC_EN1_CLR_N1_CLR_CLR(w)   w &= ~( 0x1 << 7)

#define PMU_GLB_CLK_FRC_EN1_CLR_N0_CLR_POS      6
#define PMU_GLB_CLK_FRC_EN1_CLR_N0_CLR_LEN      1
#define PMU_GLB_CLK_FRC_EN1_CLR_N0_CLR_MASK     0x00000040
#define PMU_GLB_CLK_FRC_EN1_CLR_N0_CLR_VAL(w)   (((w) & 0x00000040) >> 6)
#define PMU_GLB_CLK_FRC_EN1_CLR_N0_CLR_SET(w)   w |= ( 0x1 << 6)
#define PMU_GLB_CLK_FRC_EN1_CLR_N0_CLR_CLR(w)   w &= ~( 0x1 << 6)

#define PMU_GLB_CLK_FRC_EN1_CLR_MM_CLR_POS      5
#define PMU_GLB_CLK_FRC_EN1_CLR_MM_CLR_LEN      1
#define PMU_GLB_CLK_FRC_EN1_CLR_MM_CLR_MASK     0x00000020
#define PMU_GLB_CLK_FRC_EN1_CLR_MM_CLR_VAL(w)   (((w) & 0x00000020) >> 5)
#define PMU_GLB_CLK_FRC_EN1_CLR_MM_CLR_SET(w)   w |= ( 0x1 << 5)
#define PMU_GLB_CLK_FRC_EN1_CLR_MM_CLR_CLR(w)   w &= ~( 0x1 << 5)

#define PMU_GLB_CLK_FRC_EN1_CLR_PERIP_CLR_POS      4
#define PMU_GLB_CLK_FRC_EN1_CLR_PERIP_CLR_LEN      1
#define PMU_GLB_CLK_FRC_EN1_CLR_PERIP_CLR_MASK     0x00000010
#define PMU_GLB_CLK_FRC_EN1_CLR_PERIP_CLR_VAL(w)   (((w) & 0x00000010) >> 4)
#define PMU_GLB_CLK_FRC_EN1_CLR_PERIP_CLR_SET(w)   w |= ( 0x1 << 4)
#define PMU_GLB_CLK_FRC_EN1_CLR_PERIP_CLR_CLR(w)   w &= ~( 0x1 << 4)

#define PMU_GLB_CLK_FRC_EN1_CLR_PIPE_CLR_POS      3
#define PMU_GLB_CLK_FRC_EN1_CLR_PIPE_CLR_LEN      1
#define PMU_GLB_CLK_FRC_EN1_CLR_PIPE_CLR_MASK     0x00000008
#define PMU_GLB_CLK_FRC_EN1_CLR_PIPE_CLR_VAL(w)   (((w) & 0x00000008) >> 3)
#define PMU_GLB_CLK_FRC_EN1_CLR_PIPE_CLR_SET(w)   w |= ( 0x1 << 3)
#define PMU_GLB_CLK_FRC_EN1_CLR_PIPE_CLR_CLR(w)   w &= ~( 0x1 << 3)

#define PMU_GLB_CLK_FRC_EN1_CLR_FLASH_CLR_POS      2
#define PMU_GLB_CLK_FRC_EN1_CLR_FLASH_CLR_LEN      1
#define PMU_GLB_CLK_FRC_EN1_CLR_FLASH_CLR_MASK     0x00000004
#define PMU_GLB_CLK_FRC_EN1_CLR_FLASH_CLR_VAL(w)   (((w) & 0x00000004) >> 2)
#define PMU_GLB_CLK_FRC_EN1_CLR_FLASH_CLR_SET(w)   w |= ( 0x1 << 2)
#define PMU_GLB_CLK_FRC_EN1_CLR_FLASH_CLR_CLR(w)   w &= ~( 0x1 << 2)

#define PMU_GLB_CLK_FRC_EN1_CLR_DDR1_CLR_POS      1
#define PMU_GLB_CLK_FRC_EN1_CLR_DDR1_CLR_LEN      1
#define PMU_GLB_CLK_FRC_EN1_CLR_DDR1_CLR_MASK     0x00000002
#define PMU_GLB_CLK_FRC_EN1_CLR_DDR1_CLR_VAL(w)   (((w) & 0x00000002) >> 1)
#define PMU_GLB_CLK_FRC_EN1_CLR_DDR1_CLR_SET(w)   w |= ( 0x1 << 1)
#define PMU_GLB_CLK_FRC_EN1_CLR_DDR1_CLR_CLR(w)   w &= ~( 0x1 << 1)

#define PMU_GLB_CLK_FRC_EN1_CLR_DDR0_CLR_POS      0
#define PMU_GLB_CLK_FRC_EN1_CLR_DDR0_CLR_LEN      1
#define PMU_GLB_CLK_FRC_EN1_CLR_DDR0_CLR_MASK     0x00000001
#define PMU_GLB_CLK_FRC_EN1_CLR_DDR0_CLR_VAL(w)   (((w) & 0x00000001) >> 0)
#define PMU_GLB_CLK_FRC_EN1_CLR_DDR0_CLR_SET(w)   w |= ( 0x1 << 0)
#define PMU_GLB_CLK_FRC_EN1_CLR_DDR0_CLR_CLR(w)   w &= ~( 0x1 << 0)

// REGISTER : CLK_FRC_SW_SET

#define PMU_GLB_CLK_FRC_SW_SET_ADDR      PMU_GLB_BASE_ADDR + 0x1EC
#define PMU_GLB_CLK_FRC_SW_SET_RSTVAL    0x0
#define PMU_GLB_CLK_FRC_SW_SET_ALL1      0x0
#define PMU_GLB_CLK_FRC_SW_SET_ALL0      0x0
#define PMU_GLB_CLK_FRC_SW_SET_ALL5A     0x0
#define PMU_GLB_CLK_FRC_SW_SET_ALLA5     0x0

// FIELD IN REGISTER : CLK_FRC_SW_SET

#define PMU_GLB_CLK_FRC_SW_SET_CPU7_SET_POS      11
#define PMU_GLB_CLK_FRC_SW_SET_CPU7_SET_LEN      1
#define PMU_GLB_CLK_FRC_SW_SET_CPU7_SET_MASK     0x00000800
#define PMU_GLB_CLK_FRC_SW_SET_CPU7_SET_VAL(w)   (((w) & 0x00000800) >> 11)
#define PMU_GLB_CLK_FRC_SW_SET_CPU7_SET_SET(w)   w |= ( 0x1 << 11)
#define PMU_GLB_CLK_FRC_SW_SET_CPU7_SET_CLR(w)   w &= ~( 0x1 << 11)

#define PMU_GLB_CLK_FRC_SW_SET_CPU6_SET_POS      10
#define PMU_GLB_CLK_FRC_SW_SET_CPU6_SET_LEN      1
#define PMU_GLB_CLK_FRC_SW_SET_CPU6_SET_MASK     0x00000400
#define PMU_GLB_CLK_FRC_SW_SET_CPU6_SET_VAL(w)   (((w) & 0x00000400) >> 10)
#define PMU_GLB_CLK_FRC_SW_SET_CPU6_SET_SET(w)   w |= ( 0x1 << 10)
#define PMU_GLB_CLK_FRC_SW_SET_CPU6_SET_CLR(w)   w &= ~( 0x1 << 10)

#define PMU_GLB_CLK_FRC_SW_SET_CPU5_SET_POS      9
#define PMU_GLB_CLK_FRC_SW_SET_CPU5_SET_LEN      1
#define PMU_GLB_CLK_FRC_SW_SET_CPU5_SET_MASK     0x00000200
#define PMU_GLB_CLK_FRC_SW_SET_CPU5_SET_VAL(w)   (((w) & 0x00000200) >> 9)
#define PMU_GLB_CLK_FRC_SW_SET_CPU5_SET_SET(w)   w |= ( 0x1 << 9)
#define PMU_GLB_CLK_FRC_SW_SET_CPU5_SET_CLR(w)   w &= ~( 0x1 << 9)

#define PMU_GLB_CLK_FRC_SW_SET_CPU4_SET_POS      8
#define PMU_GLB_CLK_FRC_SW_SET_CPU4_SET_LEN      1
#define PMU_GLB_CLK_FRC_SW_SET_CPU4_SET_MASK     0x00000100
#define PMU_GLB_CLK_FRC_SW_SET_CPU4_SET_VAL(w)   (((w) & 0x00000100) >> 8)
#define PMU_GLB_CLK_FRC_SW_SET_CPU4_SET_SET(w)   w |= ( 0x1 << 8)
#define PMU_GLB_CLK_FRC_SW_SET_CPU4_SET_CLR(w)   w &= ~( 0x1 << 8)

#define PMU_GLB_CLK_FRC_SW_SET_CPU3_SET_POS      7
#define PMU_GLB_CLK_FRC_SW_SET_CPU3_SET_LEN      1
#define PMU_GLB_CLK_FRC_SW_SET_CPU3_SET_MASK     0x00000080
#define PMU_GLB_CLK_FRC_SW_SET_CPU3_SET_VAL(w)   (((w) & 0x00000080) >> 7)
#define PMU_GLB_CLK_FRC_SW_SET_CPU3_SET_SET(w)   w |= ( 0x1 << 7)
#define PMU_GLB_CLK_FRC_SW_SET_CPU3_SET_CLR(w)   w &= ~( 0x1 << 7)

#define PMU_GLB_CLK_FRC_SW_SET_CPU2_SET_POS      6
#define PMU_GLB_CLK_FRC_SW_SET_CPU2_SET_LEN      1
#define PMU_GLB_CLK_FRC_SW_SET_CPU2_SET_MASK     0x00000040
#define PMU_GLB_CLK_FRC_SW_SET_CPU2_SET_VAL(w)   (((w) & 0x00000040) >> 6)
#define PMU_GLB_CLK_FRC_SW_SET_CPU2_SET_SET(w)   w |= ( 0x1 << 6)
#define PMU_GLB_CLK_FRC_SW_SET_CPU2_SET_CLR(w)   w &= ~( 0x1 << 6)

#define PMU_GLB_CLK_FRC_SW_SET_CPU1_SET_POS      5
#define PMU_GLB_CLK_FRC_SW_SET_CPU1_SET_LEN      1
#define PMU_GLB_CLK_FRC_SW_SET_CPU1_SET_MASK     0x00000020
#define PMU_GLB_CLK_FRC_SW_SET_CPU1_SET_VAL(w)   (((w) & 0x00000020) >> 5)
#define PMU_GLB_CLK_FRC_SW_SET_CPU1_SET_SET(w)   w |= ( 0x1 << 5)
#define PMU_GLB_CLK_FRC_SW_SET_CPU1_SET_CLR(w)   w &= ~( 0x1 << 5)

#define PMU_GLB_CLK_FRC_SW_SET_CPU0_SET_POS      4
#define PMU_GLB_CLK_FRC_SW_SET_CPU0_SET_LEN      1
#define PMU_GLB_CLK_FRC_SW_SET_CPU0_SET_MASK     0x00000010
#define PMU_GLB_CLK_FRC_SW_SET_CPU0_SET_VAL(w)   (((w) & 0x00000010) >> 4)
#define PMU_GLB_CLK_FRC_SW_SET_CPU0_SET_SET(w)   w |= ( 0x1 << 4)
#define PMU_GLB_CLK_FRC_SW_SET_CPU0_SET_CLR(w)   w &= ~( 0x1 << 4)

#define PMU_GLB_CLK_FRC_SW_SET_DSP_SET_POS      3
#define PMU_GLB_CLK_FRC_SW_SET_DSP_SET_LEN      1
#define PMU_GLB_CLK_FRC_SW_SET_DSP_SET_MASK     0x00000008
#define PMU_GLB_CLK_FRC_SW_SET_DSP_SET_VAL(w)   (((w) & 0x00000008) >> 3)
#define PMU_GLB_CLK_FRC_SW_SET_DSP_SET_SET(w)   w |= ( 0x1 << 3)
#define PMU_GLB_CLK_FRC_SW_SET_DSP_SET_CLR(w)   w &= ~( 0x1 << 3)

#define PMU_GLB_CLK_FRC_SW_SET_VDEC_SET_POS      2
#define PMU_GLB_CLK_FRC_SW_SET_VDEC_SET_LEN      1
#define PMU_GLB_CLK_FRC_SW_SET_VDEC_SET_MASK     0x00000004
#define PMU_GLB_CLK_FRC_SW_SET_VDEC_SET_VAL(w)   (((w) & 0x00000004) >> 2)
#define PMU_GLB_CLK_FRC_SW_SET_VDEC_SET_SET(w)   w |= ( 0x1 << 2)
#define PMU_GLB_CLK_FRC_SW_SET_VDEC_SET_CLR(w)   w &= ~( 0x1 << 2)

#define PMU_GLB_CLK_FRC_SW_SET_VENC1_SET_POS      1
#define PMU_GLB_CLK_FRC_SW_SET_VENC1_SET_LEN      1
#define PMU_GLB_CLK_FRC_SW_SET_VENC1_SET_MASK     0x00000002
#define PMU_GLB_CLK_FRC_SW_SET_VENC1_SET_VAL(w)   (((w) & 0x00000002) >> 1)
#define PMU_GLB_CLK_FRC_SW_SET_VENC1_SET_SET(w)   w |= ( 0x1 << 1)
#define PMU_GLB_CLK_FRC_SW_SET_VENC1_SET_CLR(w)   w &= ~( 0x1 << 1)

#define PMU_GLB_CLK_FRC_SW_SET_VENC0_SET_POS      0
#define PMU_GLB_CLK_FRC_SW_SET_VENC0_SET_LEN      1
#define PMU_GLB_CLK_FRC_SW_SET_VENC0_SET_MASK     0x00000001
#define PMU_GLB_CLK_FRC_SW_SET_VENC0_SET_VAL(w)   (((w) & 0x00000001) >> 0)
#define PMU_GLB_CLK_FRC_SW_SET_VENC0_SET_SET(w)   w |= ( 0x1 << 0)
#define PMU_GLB_CLK_FRC_SW_SET_VENC0_SET_CLR(w)   w &= ~( 0x1 << 0)

// REGISTER : CLK_FRC_SW_CLR

#define PMU_GLB_CLK_FRC_SW_CLR_ADDR      PMU_GLB_BASE_ADDR + 0x1F0
#define PMU_GLB_CLK_FRC_SW_CLR_RSTVAL    0x0
#define PMU_GLB_CLK_FRC_SW_CLR_ALL1      0x0
#define PMU_GLB_CLK_FRC_SW_CLR_ALL0      0x0
#define PMU_GLB_CLK_FRC_SW_CLR_ALL5A     0x0
#define PMU_GLB_CLK_FRC_SW_CLR_ALLA5     0x0

// FIELD IN REGISTER : CLK_FRC_SW_CLR

#define PMU_GLB_CLK_FRC_SW_CLR_CPU7_CLR_POS      11
#define PMU_GLB_CLK_FRC_SW_CLR_CPU7_CLR_LEN      1
#define PMU_GLB_CLK_FRC_SW_CLR_CPU7_CLR_MASK     0x00000800
#define PMU_GLB_CLK_FRC_SW_CLR_CPU7_CLR_VAL(w)   (((w) & 0x00000800) >> 11)
#define PMU_GLB_CLK_FRC_SW_CLR_CPU7_CLR_SET(w)   w |= ( 0x1 << 11)
#define PMU_GLB_CLK_FRC_SW_CLR_CPU7_CLR_CLR(w)   w &= ~( 0x1 << 11)

#define PMU_GLB_CLK_FRC_SW_CLR_CPU6_CLR_POS      10
#define PMU_GLB_CLK_FRC_SW_CLR_CPU6_CLR_LEN      1
#define PMU_GLB_CLK_FRC_SW_CLR_CPU6_CLR_MASK     0x00000400
#define PMU_GLB_CLK_FRC_SW_CLR_CPU6_CLR_VAL(w)   (((w) & 0x00000400) >> 10)
#define PMU_GLB_CLK_FRC_SW_CLR_CPU6_CLR_SET(w)   w |= ( 0x1 << 10)
#define PMU_GLB_CLK_FRC_SW_CLR_CPU6_CLR_CLR(w)   w &= ~( 0x1 << 10)

#define PMU_GLB_CLK_FRC_SW_CLR_CPU5_CLR_POS      9
#define PMU_GLB_CLK_FRC_SW_CLR_CPU5_CLR_LEN      1
#define PMU_GLB_CLK_FRC_SW_CLR_CPU5_CLR_MASK     0x00000200
#define PMU_GLB_CLK_FRC_SW_CLR_CPU5_CLR_VAL(w)   (((w) & 0x00000200) >> 9)
#define PMU_GLB_CLK_FRC_SW_CLR_CPU5_CLR_SET(w)   w |= ( 0x1 << 9)
#define PMU_GLB_CLK_FRC_SW_CLR_CPU5_CLR_CLR(w)   w &= ~( 0x1 << 9)

#define PMU_GLB_CLK_FRC_SW_CLR_CPU4_CLR_POS      8
#define PMU_GLB_CLK_FRC_SW_CLR_CPU4_CLR_LEN      1
#define PMU_GLB_CLK_FRC_SW_CLR_CPU4_CLR_MASK     0x00000100
#define PMU_GLB_CLK_FRC_SW_CLR_CPU4_CLR_VAL(w)   (((w) & 0x00000100) >> 8)
#define PMU_GLB_CLK_FRC_SW_CLR_CPU4_CLR_SET(w)   w |= ( 0x1 << 8)
#define PMU_GLB_CLK_FRC_SW_CLR_CPU4_CLR_CLR(w)   w &= ~( 0x1 << 8)

#define PMU_GLB_CLK_FRC_SW_CLR_CPU3_CLR_POS      7
#define PMU_GLB_CLK_FRC_SW_CLR_CPU3_CLR_LEN      1
#define PMU_GLB_CLK_FRC_SW_CLR_CPU3_CLR_MASK     0x00000080
#define PMU_GLB_CLK_FRC_SW_CLR_CPU3_CLR_VAL(w)   (((w) & 0x00000080) >> 7)
#define PMU_GLB_CLK_FRC_SW_CLR_CPU3_CLR_SET(w)   w |= ( 0x1 << 7)
#define PMU_GLB_CLK_FRC_SW_CLR_CPU3_CLR_CLR(w)   w &= ~( 0x1 << 7)

#define PMU_GLB_CLK_FRC_SW_CLR_CPU2_CLR_POS      6
#define PMU_GLB_CLK_FRC_SW_CLR_CPU2_CLR_LEN      1
#define PMU_GLB_CLK_FRC_SW_CLR_CPU2_CLR_MASK     0x00000040
#define PMU_GLB_CLK_FRC_SW_CLR_CPU2_CLR_VAL(w)   (((w) & 0x00000040) >> 6)
#define PMU_GLB_CLK_FRC_SW_CLR_CPU2_CLR_SET(w)   w |= ( 0x1 << 6)
#define PMU_GLB_CLK_FRC_SW_CLR_CPU2_CLR_CLR(w)   w &= ~( 0x1 << 6)

#define PMU_GLB_CLK_FRC_SW_CLR_CPU1_CLR_POS      5
#define PMU_GLB_CLK_FRC_SW_CLR_CPU1_CLR_LEN      1
#define PMU_GLB_CLK_FRC_SW_CLR_CPU1_CLR_MASK     0x00000020
#define PMU_GLB_CLK_FRC_SW_CLR_CPU1_CLR_VAL(w)   (((w) & 0x00000020) >> 5)
#define PMU_GLB_CLK_FRC_SW_CLR_CPU1_CLR_SET(w)   w |= ( 0x1 << 5)
#define PMU_GLB_CLK_FRC_SW_CLR_CPU1_CLR_CLR(w)   w &= ~( 0x1 << 5)

#define PMU_GLB_CLK_FRC_SW_CLR_CPU0_CLR_POS      4
#define PMU_GLB_CLK_FRC_SW_CLR_CPU0_CLR_LEN      1
#define PMU_GLB_CLK_FRC_SW_CLR_CPU0_CLR_MASK     0x00000010
#define PMU_GLB_CLK_FRC_SW_CLR_CPU0_CLR_VAL(w)   (((w) & 0x00000010) >> 4)
#define PMU_GLB_CLK_FRC_SW_CLR_CPU0_CLR_SET(w)   w |= ( 0x1 << 4)
#define PMU_GLB_CLK_FRC_SW_CLR_CPU0_CLR_CLR(w)   w &= ~( 0x1 << 4)

#define PMU_GLB_CLK_FRC_SW_CLR_DSP_CLR_POS      3
#define PMU_GLB_CLK_FRC_SW_CLR_DSP_CLR_LEN      1
#define PMU_GLB_CLK_FRC_SW_CLR_DSP_CLR_MASK     0x00000008
#define PMU_GLB_CLK_FRC_SW_CLR_DSP_CLR_VAL(w)   (((w) & 0x00000008) >> 3)
#define PMU_GLB_CLK_FRC_SW_CLR_DSP_CLR_SET(w)   w |= ( 0x1 << 3)
#define PMU_GLB_CLK_FRC_SW_CLR_DSP_CLR_CLR(w)   w &= ~( 0x1 << 3)

#define PMU_GLB_CLK_FRC_SW_CLR_VDEC_CLR_POS      2
#define PMU_GLB_CLK_FRC_SW_CLR_VDEC_CLR_LEN      1
#define PMU_GLB_CLK_FRC_SW_CLR_VDEC_CLR_MASK     0x00000004
#define PMU_GLB_CLK_FRC_SW_CLR_VDEC_CLR_VAL(w)   (((w) & 0x00000004) >> 2)
#define PMU_GLB_CLK_FRC_SW_CLR_VDEC_CLR_SET(w)   w |= ( 0x1 << 2)
#define PMU_GLB_CLK_FRC_SW_CLR_VDEC_CLR_CLR(w)   w &= ~( 0x1 << 2)

#define PMU_GLB_CLK_FRC_SW_CLR_VENC1_CLR_POS      1
#define PMU_GLB_CLK_FRC_SW_CLR_VENC1_CLR_LEN      1
#define PMU_GLB_CLK_FRC_SW_CLR_VENC1_CLR_MASK     0x00000002
#define PMU_GLB_CLK_FRC_SW_CLR_VENC1_CLR_VAL(w)   (((w) & 0x00000002) >> 1)
#define PMU_GLB_CLK_FRC_SW_CLR_VENC1_CLR_SET(w)   w |= ( 0x1 << 1)
#define PMU_GLB_CLK_FRC_SW_CLR_VENC1_CLR_CLR(w)   w &= ~( 0x1 << 1)

#define PMU_GLB_CLK_FRC_SW_CLR_VENC0_CLR_POS      0
#define PMU_GLB_CLK_FRC_SW_CLR_VENC0_CLR_LEN      1
#define PMU_GLB_CLK_FRC_SW_CLR_VENC0_CLR_MASK     0x00000001
#define PMU_GLB_CLK_FRC_SW_CLR_VENC0_CLR_VAL(w)   (((w) & 0x00000001) >> 0)
#define PMU_GLB_CLK_FRC_SW_CLR_VENC0_CLR_SET(w)   w |= ( 0x1 << 0)
#define PMU_GLB_CLK_FRC_SW_CLR_VENC0_CLR_CLR(w)   w &= ~( 0x1 << 0)

// REGISTER : CLK_FRC_SW1_SET

#define PMU_GLB_CLK_FRC_SW1_SET_ADDR      PMU_GLB_BASE_ADDR + 0x1F4
#define PMU_GLB_CLK_FRC_SW1_SET_RSTVAL    0x0
#define PMU_GLB_CLK_FRC_SW1_SET_ALL1      0x0
#define PMU_GLB_CLK_FRC_SW1_SET_ALL0      0x0
#define PMU_GLB_CLK_FRC_SW1_SET_ALL5A     0x0
#define PMU_GLB_CLK_FRC_SW1_SET_ALLA5     0x0

// FIELD IN REGISTER : CLK_FRC_SW1_SET

#define PMU_GLB_CLK_FRC_SW1_SET_SEN_SET_POS      18
#define PMU_GLB_CLK_FRC_SW1_SET_SEN_SET_LEN      1
#define PMU_GLB_CLK_FRC_SW1_SET_SEN_SET_MASK     0x00040000
#define PMU_GLB_CLK_FRC_SW1_SET_SEN_SET_VAL(w)   (((w) & 0x00040000) >> 18)
#define PMU_GLB_CLK_FRC_SW1_SET_SEN_SET_SET(w)   w |= ( 0x1 << 18)
#define PMU_GLB_CLK_FRC_SW1_SET_SEN_SET_CLR(w)   w &= ~( 0x1 << 18)

#define PMU_GLB_CLK_FRC_SW1_SET_ISP_SET_POS      17
#define PMU_GLB_CLK_FRC_SW1_SET_ISP_SET_LEN      1
#define PMU_GLB_CLK_FRC_SW1_SET_ISP_SET_MASK     0x00020000
#define PMU_GLB_CLK_FRC_SW1_SET_ISP_SET_VAL(w)   (((w) & 0x00020000) >> 17)
#define PMU_GLB_CLK_FRC_SW1_SET_ISP_SET_SET(w)   w |= ( 0x1 << 17)
#define PMU_GLB_CLK_FRC_SW1_SET_ISP_SET_CLR(w)   w &= ~( 0x1 << 17)

#define PMU_GLB_CLK_FRC_SW1_SET_CPU_SET_POS      16
#define PMU_GLB_CLK_FRC_SW1_SET_CPU_SET_LEN      1
#define PMU_GLB_CLK_FRC_SW1_SET_CPU_SET_MASK     0x00010000
#define PMU_GLB_CLK_FRC_SW1_SET_CPU_SET_VAL(w)   (((w) & 0x00010000) >> 16)
#define PMU_GLB_CLK_FRC_SW1_SET_CPU_SET_SET(w)   w |= ( 0x1 << 16)
#define PMU_GLB_CLK_FRC_SW1_SET_CPU_SET_CLR(w)   w &= ~( 0x1 << 16)

#define PMU_GLB_CLK_FRC_SW1_SET_NPU_SET_POS      15
#define PMU_GLB_CLK_FRC_SW1_SET_NPU_SET_LEN      1
#define PMU_GLB_CLK_FRC_SW1_SET_NPU_SET_MASK     0x00008000
#define PMU_GLB_CLK_FRC_SW1_SET_NPU_SET_VAL(w)   (((w) & 0x00008000) >> 15)
#define PMU_GLB_CLK_FRC_SW1_SET_NPU_SET_SET(w)   w |= ( 0x1 << 15)
#define PMU_GLB_CLK_FRC_SW1_SET_NPU_SET_CLR(w)   w &= ~( 0x1 << 15)

#define PMU_GLB_CLK_FRC_SW1_SET_N8_SET_POS      14
#define PMU_GLB_CLK_FRC_SW1_SET_N8_SET_LEN      1
#define PMU_GLB_CLK_FRC_SW1_SET_N8_SET_MASK     0x00004000
#define PMU_GLB_CLK_FRC_SW1_SET_N8_SET_VAL(w)   (((w) & 0x00004000) >> 14)
#define PMU_GLB_CLK_FRC_SW1_SET_N8_SET_SET(w)   w |= ( 0x1 << 14)
#define PMU_GLB_CLK_FRC_SW1_SET_N8_SET_CLR(w)   w &= ~( 0x1 << 14)

#define PMU_GLB_CLK_FRC_SW1_SET_N7_SET_POS      13
#define PMU_GLB_CLK_FRC_SW1_SET_N7_SET_LEN      1
#define PMU_GLB_CLK_FRC_SW1_SET_N7_SET_MASK     0x00002000
#define PMU_GLB_CLK_FRC_SW1_SET_N7_SET_VAL(w)   (((w) & 0x00002000) >> 13)
#define PMU_GLB_CLK_FRC_SW1_SET_N7_SET_SET(w)   w |= ( 0x1 << 13)
#define PMU_GLB_CLK_FRC_SW1_SET_N7_SET_CLR(w)   w &= ~( 0x1 << 13)

#define PMU_GLB_CLK_FRC_SW1_SET_N6_SET_POS      12
#define PMU_GLB_CLK_FRC_SW1_SET_N6_SET_LEN      1
#define PMU_GLB_CLK_FRC_SW1_SET_N6_SET_MASK     0x00001000
#define PMU_GLB_CLK_FRC_SW1_SET_N6_SET_VAL(w)   (((w) & 0x00001000) >> 12)
#define PMU_GLB_CLK_FRC_SW1_SET_N6_SET_SET(w)   w |= ( 0x1 << 12)
#define PMU_GLB_CLK_FRC_SW1_SET_N6_SET_CLR(w)   w &= ~( 0x1 << 12)

#define PMU_GLB_CLK_FRC_SW1_SET_N5_SET_POS      11
#define PMU_GLB_CLK_FRC_SW1_SET_N5_SET_LEN      1
#define PMU_GLB_CLK_FRC_SW1_SET_N5_SET_MASK     0x00000800
#define PMU_GLB_CLK_FRC_SW1_SET_N5_SET_VAL(w)   (((w) & 0x00000800) >> 11)
#define PMU_GLB_CLK_FRC_SW1_SET_N5_SET_SET(w)   w |= ( 0x1 << 11)
#define PMU_GLB_CLK_FRC_SW1_SET_N5_SET_CLR(w)   w &= ~( 0x1 << 11)

#define PMU_GLB_CLK_FRC_SW1_SET_N4_SET_POS      10
#define PMU_GLB_CLK_FRC_SW1_SET_N4_SET_LEN      1
#define PMU_GLB_CLK_FRC_SW1_SET_N4_SET_MASK     0x00000400
#define PMU_GLB_CLK_FRC_SW1_SET_N4_SET_VAL(w)   (((w) & 0x00000400) >> 10)
#define PMU_GLB_CLK_FRC_SW1_SET_N4_SET_SET(w)   w |= ( 0x1 << 10)
#define PMU_GLB_CLK_FRC_SW1_SET_N4_SET_CLR(w)   w &= ~( 0x1 << 10)

#define PMU_GLB_CLK_FRC_SW1_SET_N3_SET_POS      9
#define PMU_GLB_CLK_FRC_SW1_SET_N3_SET_LEN      1
#define PMU_GLB_CLK_FRC_SW1_SET_N3_SET_MASK     0x00000200
#define PMU_GLB_CLK_FRC_SW1_SET_N3_SET_VAL(w)   (((w) & 0x00000200) >> 9)
#define PMU_GLB_CLK_FRC_SW1_SET_N3_SET_SET(w)   w |= ( 0x1 << 9)
#define PMU_GLB_CLK_FRC_SW1_SET_N3_SET_CLR(w)   w &= ~( 0x1 << 9)

#define PMU_GLB_CLK_FRC_SW1_SET_N2_SET_POS      8
#define PMU_GLB_CLK_FRC_SW1_SET_N2_SET_LEN      1
#define PMU_GLB_CLK_FRC_SW1_SET_N2_SET_MASK     0x00000100
#define PMU_GLB_CLK_FRC_SW1_SET_N2_SET_VAL(w)   (((w) & 0x00000100) >> 8)
#define PMU_GLB_CLK_FRC_SW1_SET_N2_SET_SET(w)   w |= ( 0x1 << 8)
#define PMU_GLB_CLK_FRC_SW1_SET_N2_SET_CLR(w)   w &= ~( 0x1 << 8)

#define PMU_GLB_CLK_FRC_SW1_SET_N1_SET_POS      7
#define PMU_GLB_CLK_FRC_SW1_SET_N1_SET_LEN      1
#define PMU_GLB_CLK_FRC_SW1_SET_N1_SET_MASK     0x00000080
#define PMU_GLB_CLK_FRC_SW1_SET_N1_SET_VAL(w)   (((w) & 0x00000080) >> 7)
#define PMU_GLB_CLK_FRC_SW1_SET_N1_SET_SET(w)   w |= ( 0x1 << 7)
#define PMU_GLB_CLK_FRC_SW1_SET_N1_SET_CLR(w)   w &= ~( 0x1 << 7)

#define PMU_GLB_CLK_FRC_SW1_SET_N0_SET_POS      6
#define PMU_GLB_CLK_FRC_SW1_SET_N0_SET_LEN      1
#define PMU_GLB_CLK_FRC_SW1_SET_N0_SET_MASK     0x00000040
#define PMU_GLB_CLK_FRC_SW1_SET_N0_SET_VAL(w)   (((w) & 0x00000040) >> 6)
#define PMU_GLB_CLK_FRC_SW1_SET_N0_SET_SET(w)   w |= ( 0x1 << 6)
#define PMU_GLB_CLK_FRC_SW1_SET_N0_SET_CLR(w)   w &= ~( 0x1 << 6)

#define PMU_GLB_CLK_FRC_SW1_SET_MM_SET_POS      5
#define PMU_GLB_CLK_FRC_SW1_SET_MM_SET_LEN      1
#define PMU_GLB_CLK_FRC_SW1_SET_MM_SET_MASK     0x00000020
#define PMU_GLB_CLK_FRC_SW1_SET_MM_SET_VAL(w)   (((w) & 0x00000020) >> 5)
#define PMU_GLB_CLK_FRC_SW1_SET_MM_SET_SET(w)   w |= ( 0x1 << 5)
#define PMU_GLB_CLK_FRC_SW1_SET_MM_SET_CLR(w)   w &= ~( 0x1 << 5)

#define PMU_GLB_CLK_FRC_SW1_SET_PERIP_SET_POS      4
#define PMU_GLB_CLK_FRC_SW1_SET_PERIP_SET_LEN      1
#define PMU_GLB_CLK_FRC_SW1_SET_PERIP_SET_MASK     0x00000010
#define PMU_GLB_CLK_FRC_SW1_SET_PERIP_SET_VAL(w)   (((w) & 0x00000010) >> 4)
#define PMU_GLB_CLK_FRC_SW1_SET_PERIP_SET_SET(w)   w |= ( 0x1 << 4)
#define PMU_GLB_CLK_FRC_SW1_SET_PERIP_SET_CLR(w)   w &= ~( 0x1 << 4)

#define PMU_GLB_CLK_FRC_SW1_SET_PIPE_SET_POS      3
#define PMU_GLB_CLK_FRC_SW1_SET_PIPE_SET_LEN      1
#define PMU_GLB_CLK_FRC_SW1_SET_PIPE_SET_MASK     0x00000008
#define PMU_GLB_CLK_FRC_SW1_SET_PIPE_SET_VAL(w)   (((w) & 0x00000008) >> 3)
#define PMU_GLB_CLK_FRC_SW1_SET_PIPE_SET_SET(w)   w |= ( 0x1 << 3)
#define PMU_GLB_CLK_FRC_SW1_SET_PIPE_SET_CLR(w)   w &= ~( 0x1 << 3)

#define PMU_GLB_CLK_FRC_SW1_SET_FLASH_SET_POS      2
#define PMU_GLB_CLK_FRC_SW1_SET_FLASH_SET_LEN      1
#define PMU_GLB_CLK_FRC_SW1_SET_FLASH_SET_MASK     0x00000004
#define PMU_GLB_CLK_FRC_SW1_SET_FLASH_SET_VAL(w)   (((w) & 0x00000004) >> 2)
#define PMU_GLB_CLK_FRC_SW1_SET_FLASH_SET_SET(w)   w |= ( 0x1 << 2)
#define PMU_GLB_CLK_FRC_SW1_SET_FLASH_SET_CLR(w)   w &= ~( 0x1 << 2)

#define PMU_GLB_CLK_FRC_SW1_SET_DDR1_SET_POS      1
#define PMU_GLB_CLK_FRC_SW1_SET_DDR1_SET_LEN      1
#define PMU_GLB_CLK_FRC_SW1_SET_DDR1_SET_MASK     0x00000002
#define PMU_GLB_CLK_FRC_SW1_SET_DDR1_SET_VAL(w)   (((w) & 0x00000002) >> 1)
#define PMU_GLB_CLK_FRC_SW1_SET_DDR1_SET_SET(w)   w |= ( 0x1 << 1)
#define PMU_GLB_CLK_FRC_SW1_SET_DDR1_SET_CLR(w)   w &= ~( 0x1 << 1)

#define PMU_GLB_CLK_FRC_SW1_SET_DDR0_SET_POS      0
#define PMU_GLB_CLK_FRC_SW1_SET_DDR0_SET_LEN      1
#define PMU_GLB_CLK_FRC_SW1_SET_DDR0_SET_MASK     0x00000001
#define PMU_GLB_CLK_FRC_SW1_SET_DDR0_SET_VAL(w)   (((w) & 0x00000001) >> 0)
#define PMU_GLB_CLK_FRC_SW1_SET_DDR0_SET_SET(w)   w |= ( 0x1 << 0)
#define PMU_GLB_CLK_FRC_SW1_SET_DDR0_SET_CLR(w)   w &= ~( 0x1 << 0)

// REGISTER : CLK_FRC_SW1_CLR

#define PMU_GLB_CLK_FRC_SW1_CLR_ADDR      PMU_GLB_BASE_ADDR + 0x1F8
#define PMU_GLB_CLK_FRC_SW1_CLR_RSTVAL    0x0
#define PMU_GLB_CLK_FRC_SW1_CLR_ALL1      0x0
#define PMU_GLB_CLK_FRC_SW1_CLR_ALL0      0x0
#define PMU_GLB_CLK_FRC_SW1_CLR_ALL5A     0x0
#define PMU_GLB_CLK_FRC_SW1_CLR_ALLA5     0x0

// FIELD IN REGISTER : CLK_FRC_SW1_CLR

#define PMU_GLB_CLK_FRC_SW1_CLR_SEN_CLR_POS      18
#define PMU_GLB_CLK_FRC_SW1_CLR_SEN_CLR_LEN      1
#define PMU_GLB_CLK_FRC_SW1_CLR_SEN_CLR_MASK     0x00040000
#define PMU_GLB_CLK_FRC_SW1_CLR_SEN_CLR_VAL(w)   (((w) & 0x00040000) >> 18)
#define PMU_GLB_CLK_FRC_SW1_CLR_SEN_CLR_SET(w)   w |= ( 0x1 << 18)
#define PMU_GLB_CLK_FRC_SW1_CLR_SEN_CLR_CLR(w)   w &= ~( 0x1 << 18)

#define PMU_GLB_CLK_FRC_SW1_CLR_ISP_CLR_POS      17
#define PMU_GLB_CLK_FRC_SW1_CLR_ISP_CLR_LEN      1
#define PMU_GLB_CLK_FRC_SW1_CLR_ISP_CLR_MASK     0x00020000
#define PMU_GLB_CLK_FRC_SW1_CLR_ISP_CLR_VAL(w)   (((w) & 0x00020000) >> 17)
#define PMU_GLB_CLK_FRC_SW1_CLR_ISP_CLR_SET(w)   w |= ( 0x1 << 17)
#define PMU_GLB_CLK_FRC_SW1_CLR_ISP_CLR_CLR(w)   w &= ~( 0x1 << 17)

#define PMU_GLB_CLK_FRC_SW1_CLR_CPU_CLR_POS      16
#define PMU_GLB_CLK_FRC_SW1_CLR_CPU_CLR_LEN      1
#define PMU_GLB_CLK_FRC_SW1_CLR_CPU_CLR_MASK     0x00010000
#define PMU_GLB_CLK_FRC_SW1_CLR_CPU_CLR_VAL(w)   (((w) & 0x00010000) >> 16)
#define PMU_GLB_CLK_FRC_SW1_CLR_CPU_CLR_SET(w)   w |= ( 0x1 << 16)
#define PMU_GLB_CLK_FRC_SW1_CLR_CPU_CLR_CLR(w)   w &= ~( 0x1 << 16)

#define PMU_GLB_CLK_FRC_SW1_CLR_NPU_CLR_POS      15
#define PMU_GLB_CLK_FRC_SW1_CLR_NPU_CLR_LEN      1
#define PMU_GLB_CLK_FRC_SW1_CLR_NPU_CLR_MASK     0x00008000
#define PMU_GLB_CLK_FRC_SW1_CLR_NPU_CLR_VAL(w)   (((w) & 0x00008000) >> 15)
#define PMU_GLB_CLK_FRC_SW1_CLR_NPU_CLR_SET(w)   w |= ( 0x1 << 15)
#define PMU_GLB_CLK_FRC_SW1_CLR_NPU_CLR_CLR(w)   w &= ~( 0x1 << 15)

#define PMU_GLB_CLK_FRC_SW1_CLR_N8_CLR_POS      14
#define PMU_GLB_CLK_FRC_SW1_CLR_N8_CLR_LEN      1
#define PMU_GLB_CLK_FRC_SW1_CLR_N8_CLR_MASK     0x00004000
#define PMU_GLB_CLK_FRC_SW1_CLR_N8_CLR_VAL(w)   (((w) & 0x00004000) >> 14)
#define PMU_GLB_CLK_FRC_SW1_CLR_N8_CLR_SET(w)   w |= ( 0x1 << 14)
#define PMU_GLB_CLK_FRC_SW1_CLR_N8_CLR_CLR(w)   w &= ~( 0x1 << 14)

#define PMU_GLB_CLK_FRC_SW1_CLR_N7_CLR_POS      13
#define PMU_GLB_CLK_FRC_SW1_CLR_N7_CLR_LEN      1
#define PMU_GLB_CLK_FRC_SW1_CLR_N7_CLR_MASK     0x00002000
#define PMU_GLB_CLK_FRC_SW1_CLR_N7_CLR_VAL(w)   (((w) & 0x00002000) >> 13)
#define PMU_GLB_CLK_FRC_SW1_CLR_N7_CLR_SET(w)   w |= ( 0x1 << 13)
#define PMU_GLB_CLK_FRC_SW1_CLR_N7_CLR_CLR(w)   w &= ~( 0x1 << 13)

#define PMU_GLB_CLK_FRC_SW1_CLR_N6_CLR_POS      12
#define PMU_GLB_CLK_FRC_SW1_CLR_N6_CLR_LEN      1
#define PMU_GLB_CLK_FRC_SW1_CLR_N6_CLR_MASK     0x00001000
#define PMU_GLB_CLK_FRC_SW1_CLR_N6_CLR_VAL(w)   (((w) & 0x00001000) >> 12)
#define PMU_GLB_CLK_FRC_SW1_CLR_N6_CLR_SET(w)   w |= ( 0x1 << 12)
#define PMU_GLB_CLK_FRC_SW1_CLR_N6_CLR_CLR(w)   w &= ~( 0x1 << 12)

#define PMU_GLB_CLK_FRC_SW1_CLR_N5_CLR_POS      11
#define PMU_GLB_CLK_FRC_SW1_CLR_N5_CLR_LEN      1
#define PMU_GLB_CLK_FRC_SW1_CLR_N5_CLR_MASK     0x00000800
#define PMU_GLB_CLK_FRC_SW1_CLR_N5_CLR_VAL(w)   (((w) & 0x00000800) >> 11)
#define PMU_GLB_CLK_FRC_SW1_CLR_N5_CLR_SET(w)   w |= ( 0x1 << 11)
#define PMU_GLB_CLK_FRC_SW1_CLR_N5_CLR_CLR(w)   w &= ~( 0x1 << 11)

#define PMU_GLB_CLK_FRC_SW1_CLR_N4_CLR_POS      10
#define PMU_GLB_CLK_FRC_SW1_CLR_N4_CLR_LEN      1
#define PMU_GLB_CLK_FRC_SW1_CLR_N4_CLR_MASK     0x00000400
#define PMU_GLB_CLK_FRC_SW1_CLR_N4_CLR_VAL(w)   (((w) & 0x00000400) >> 10)
#define PMU_GLB_CLK_FRC_SW1_CLR_N4_CLR_SET(w)   w |= ( 0x1 << 10)
#define PMU_GLB_CLK_FRC_SW1_CLR_N4_CLR_CLR(w)   w &= ~( 0x1 << 10)

#define PMU_GLB_CLK_FRC_SW1_CLR_N3_CLR_POS      9
#define PMU_GLB_CLK_FRC_SW1_CLR_N3_CLR_LEN      1
#define PMU_GLB_CLK_FRC_SW1_CLR_N3_CLR_MASK     0x00000200
#define PMU_GLB_CLK_FRC_SW1_CLR_N3_CLR_VAL(w)   (((w) & 0x00000200) >> 9)
#define PMU_GLB_CLK_FRC_SW1_CLR_N3_CLR_SET(w)   w |= ( 0x1 << 9)
#define PMU_GLB_CLK_FRC_SW1_CLR_N3_CLR_CLR(w)   w &= ~( 0x1 << 9)

#define PMU_GLB_CLK_FRC_SW1_CLR_N2_CLR_POS      8
#define PMU_GLB_CLK_FRC_SW1_CLR_N2_CLR_LEN      1
#define PMU_GLB_CLK_FRC_SW1_CLR_N2_CLR_MASK     0x00000100
#define PMU_GLB_CLK_FRC_SW1_CLR_N2_CLR_VAL(w)   (((w) & 0x00000100) >> 8)
#define PMU_GLB_CLK_FRC_SW1_CLR_N2_CLR_SET(w)   w |= ( 0x1 << 8)
#define PMU_GLB_CLK_FRC_SW1_CLR_N2_CLR_CLR(w)   w &= ~( 0x1 << 8)

#define PMU_GLB_CLK_FRC_SW1_CLR_N1_CLR_POS      7
#define PMU_GLB_CLK_FRC_SW1_CLR_N1_CLR_LEN      1
#define PMU_GLB_CLK_FRC_SW1_CLR_N1_CLR_MASK     0x00000080
#define PMU_GLB_CLK_FRC_SW1_CLR_N1_CLR_VAL(w)   (((w) & 0x00000080) >> 7)
#define PMU_GLB_CLK_FRC_SW1_CLR_N1_CLR_SET(w)   w |= ( 0x1 << 7)
#define PMU_GLB_CLK_FRC_SW1_CLR_N1_CLR_CLR(w)   w &= ~( 0x1 << 7)

#define PMU_GLB_CLK_FRC_SW1_CLR_N0_CLR_POS      6
#define PMU_GLB_CLK_FRC_SW1_CLR_N0_CLR_LEN      1
#define PMU_GLB_CLK_FRC_SW1_CLR_N0_CLR_MASK     0x00000040
#define PMU_GLB_CLK_FRC_SW1_CLR_N0_CLR_VAL(w)   (((w) & 0x00000040) >> 6)
#define PMU_GLB_CLK_FRC_SW1_CLR_N0_CLR_SET(w)   w |= ( 0x1 << 6)
#define PMU_GLB_CLK_FRC_SW1_CLR_N0_CLR_CLR(w)   w &= ~( 0x1 << 6)

#define PMU_GLB_CLK_FRC_SW1_CLR_MM_CLR_POS      5
#define PMU_GLB_CLK_FRC_SW1_CLR_MM_CLR_LEN      1
#define PMU_GLB_CLK_FRC_SW1_CLR_MM_CLR_MASK     0x00000020
#define PMU_GLB_CLK_FRC_SW1_CLR_MM_CLR_VAL(w)   (((w) & 0x00000020) >> 5)
#define PMU_GLB_CLK_FRC_SW1_CLR_MM_CLR_SET(w)   w |= ( 0x1 << 5)
#define PMU_GLB_CLK_FRC_SW1_CLR_MM_CLR_CLR(w)   w &= ~( 0x1 << 5)

#define PMU_GLB_CLK_FRC_SW1_CLR_PERIP_CLR_POS      4
#define PMU_GLB_CLK_FRC_SW1_CLR_PERIP_CLR_LEN      1
#define PMU_GLB_CLK_FRC_SW1_CLR_PERIP_CLR_MASK     0x00000010
#define PMU_GLB_CLK_FRC_SW1_CLR_PERIP_CLR_VAL(w)   (((w) & 0x00000010) >> 4)
#define PMU_GLB_CLK_FRC_SW1_CLR_PERIP_CLR_SET(w)   w |= ( 0x1 << 4)
#define PMU_GLB_CLK_FRC_SW1_CLR_PERIP_CLR_CLR(w)   w &= ~( 0x1 << 4)

#define PMU_GLB_CLK_FRC_SW1_CLR_PIPE_CLR_POS      3
#define PMU_GLB_CLK_FRC_SW1_CLR_PIPE_CLR_LEN      1
#define PMU_GLB_CLK_FRC_SW1_CLR_PIPE_CLR_MASK     0x00000008
#define PMU_GLB_CLK_FRC_SW1_CLR_PIPE_CLR_VAL(w)   (((w) & 0x00000008) >> 3)
#define PMU_GLB_CLK_FRC_SW1_CLR_PIPE_CLR_SET(w)   w |= ( 0x1 << 3)
#define PMU_GLB_CLK_FRC_SW1_CLR_PIPE_CLR_CLR(w)   w &= ~( 0x1 << 3)

#define PMU_GLB_CLK_FRC_SW1_CLR_FLASH_CLR_POS      2
#define PMU_GLB_CLK_FRC_SW1_CLR_FLASH_CLR_LEN      1
#define PMU_GLB_CLK_FRC_SW1_CLR_FLASH_CLR_MASK     0x00000004
#define PMU_GLB_CLK_FRC_SW1_CLR_FLASH_CLR_VAL(w)   (((w) & 0x00000004) >> 2)
#define PMU_GLB_CLK_FRC_SW1_CLR_FLASH_CLR_SET(w)   w |= ( 0x1 << 2)
#define PMU_GLB_CLK_FRC_SW1_CLR_FLASH_CLR_CLR(w)   w &= ~( 0x1 << 2)

#define PMU_GLB_CLK_FRC_SW1_CLR_DDR1_CLR_POS      1
#define PMU_GLB_CLK_FRC_SW1_CLR_DDR1_CLR_LEN      1
#define PMU_GLB_CLK_FRC_SW1_CLR_DDR1_CLR_MASK     0x00000002
#define PMU_GLB_CLK_FRC_SW1_CLR_DDR1_CLR_VAL(w)   (((w) & 0x00000002) >> 1)
#define PMU_GLB_CLK_FRC_SW1_CLR_DDR1_CLR_SET(w)   w |= ( 0x1 << 1)
#define PMU_GLB_CLK_FRC_SW1_CLR_DDR1_CLR_CLR(w)   w &= ~( 0x1 << 1)

#define PMU_GLB_CLK_FRC_SW1_CLR_DDR0_CLR_POS      0
#define PMU_GLB_CLK_FRC_SW1_CLR_DDR0_CLR_LEN      1
#define PMU_GLB_CLK_FRC_SW1_CLR_DDR0_CLR_MASK     0x00000001
#define PMU_GLB_CLK_FRC_SW1_CLR_DDR0_CLR_VAL(w)   (((w) & 0x00000001) >> 0)
#define PMU_GLB_CLK_FRC_SW1_CLR_DDR0_CLR_SET(w)   w |= ( 0x1 << 0)
#define PMU_GLB_CLK_FRC_SW1_CLR_DDR0_CLR_CLR(w)   w &= ~( 0x1 << 0)

// REGISTER : RST_FRC_EN_SET

#define PMU_GLB_RST_FRC_EN_SET_ADDR      PMU_GLB_BASE_ADDR + 0x1FC
#define PMU_GLB_RST_FRC_EN_SET_RSTVAL    0x0
#define PMU_GLB_RST_FRC_EN_SET_ALL1      0x0
#define PMU_GLB_RST_FRC_EN_SET_ALL0      0x0
#define PMU_GLB_RST_FRC_EN_SET_ALL5A     0x0
#define PMU_GLB_RST_FRC_EN_SET_ALLA5     0x0

// FIELD IN REGISTER : RST_FRC_EN_SET

#define PMU_GLB_RST_FRC_EN_SET_CPU7_SET_POS      11
#define PMU_GLB_RST_FRC_EN_SET_CPU7_SET_LEN      1
#define PMU_GLB_RST_FRC_EN_SET_CPU7_SET_MASK     0x00000800
#define PMU_GLB_RST_FRC_EN_SET_CPU7_SET_VAL(w)   (((w) & 0x00000800) >> 11)
#define PMU_GLB_RST_FRC_EN_SET_CPU7_SET_SET(w)   w |= ( 0x1 << 11)
#define PMU_GLB_RST_FRC_EN_SET_CPU7_SET_CLR(w)   w &= ~( 0x1 << 11)

#define PMU_GLB_RST_FRC_EN_SET_CPU6_SET_POS      10
#define PMU_GLB_RST_FRC_EN_SET_CPU6_SET_LEN      1
#define PMU_GLB_RST_FRC_EN_SET_CPU6_SET_MASK     0x00000400
#define PMU_GLB_RST_FRC_EN_SET_CPU6_SET_VAL(w)   (((w) & 0x00000400) >> 10)
#define PMU_GLB_RST_FRC_EN_SET_CPU6_SET_SET(w)   w |= ( 0x1 << 10)
#define PMU_GLB_RST_FRC_EN_SET_CPU6_SET_CLR(w)   w &= ~( 0x1 << 10)

#define PMU_GLB_RST_FRC_EN_SET_CPU5_SET_POS      9
#define PMU_GLB_RST_FRC_EN_SET_CPU5_SET_LEN      1
#define PMU_GLB_RST_FRC_EN_SET_CPU5_SET_MASK     0x00000200
#define PMU_GLB_RST_FRC_EN_SET_CPU5_SET_VAL(w)   (((w) & 0x00000200) >> 9)
#define PMU_GLB_RST_FRC_EN_SET_CPU5_SET_SET(w)   w |= ( 0x1 << 9)
#define PMU_GLB_RST_FRC_EN_SET_CPU5_SET_CLR(w)   w &= ~( 0x1 << 9)

#define PMU_GLB_RST_FRC_EN_SET_CPU4_SET_POS      8
#define PMU_GLB_RST_FRC_EN_SET_CPU4_SET_LEN      1
#define PMU_GLB_RST_FRC_EN_SET_CPU4_SET_MASK     0x00000100
#define PMU_GLB_RST_FRC_EN_SET_CPU4_SET_VAL(w)   (((w) & 0x00000100) >> 8)
#define PMU_GLB_RST_FRC_EN_SET_CPU4_SET_SET(w)   w |= ( 0x1 << 8)
#define PMU_GLB_RST_FRC_EN_SET_CPU4_SET_CLR(w)   w &= ~( 0x1 << 8)

#define PMU_GLB_RST_FRC_EN_SET_CPU3_SET_POS      7
#define PMU_GLB_RST_FRC_EN_SET_CPU3_SET_LEN      1
#define PMU_GLB_RST_FRC_EN_SET_CPU3_SET_MASK     0x00000080
#define PMU_GLB_RST_FRC_EN_SET_CPU3_SET_VAL(w)   (((w) & 0x00000080) >> 7)
#define PMU_GLB_RST_FRC_EN_SET_CPU3_SET_SET(w)   w |= ( 0x1 << 7)
#define PMU_GLB_RST_FRC_EN_SET_CPU3_SET_CLR(w)   w &= ~( 0x1 << 7)

#define PMU_GLB_RST_FRC_EN_SET_CPU2_SET_POS      6
#define PMU_GLB_RST_FRC_EN_SET_CPU2_SET_LEN      1
#define PMU_GLB_RST_FRC_EN_SET_CPU2_SET_MASK     0x00000040
#define PMU_GLB_RST_FRC_EN_SET_CPU2_SET_VAL(w)   (((w) & 0x00000040) >> 6)
#define PMU_GLB_RST_FRC_EN_SET_CPU2_SET_SET(w)   w |= ( 0x1 << 6)
#define PMU_GLB_RST_FRC_EN_SET_CPU2_SET_CLR(w)   w &= ~( 0x1 << 6)

#define PMU_GLB_RST_FRC_EN_SET_CPU1_SET_POS      5
#define PMU_GLB_RST_FRC_EN_SET_CPU1_SET_LEN      1
#define PMU_GLB_RST_FRC_EN_SET_CPU1_SET_MASK     0x00000020
#define PMU_GLB_RST_FRC_EN_SET_CPU1_SET_VAL(w)   (((w) & 0x00000020) >> 5)
#define PMU_GLB_RST_FRC_EN_SET_CPU1_SET_SET(w)   w |= ( 0x1 << 5)
#define PMU_GLB_RST_FRC_EN_SET_CPU1_SET_CLR(w)   w &= ~( 0x1 << 5)

#define PMU_GLB_RST_FRC_EN_SET_CPU0_SET_POS      4
#define PMU_GLB_RST_FRC_EN_SET_CPU0_SET_LEN      1
#define PMU_GLB_RST_FRC_EN_SET_CPU0_SET_MASK     0x00000010
#define PMU_GLB_RST_FRC_EN_SET_CPU0_SET_VAL(w)   (((w) & 0x00000010) >> 4)
#define PMU_GLB_RST_FRC_EN_SET_CPU0_SET_SET(w)   w |= ( 0x1 << 4)
#define PMU_GLB_RST_FRC_EN_SET_CPU0_SET_CLR(w)   w &= ~( 0x1 << 4)

#define PMU_GLB_RST_FRC_EN_SET_DSP_SET_POS      3
#define PMU_GLB_RST_FRC_EN_SET_DSP_SET_LEN      1
#define PMU_GLB_RST_FRC_EN_SET_DSP_SET_MASK     0x00000008
#define PMU_GLB_RST_FRC_EN_SET_DSP_SET_VAL(w)   (((w) & 0x00000008) >> 3)
#define PMU_GLB_RST_FRC_EN_SET_DSP_SET_SET(w)   w |= ( 0x1 << 3)
#define PMU_GLB_RST_FRC_EN_SET_DSP_SET_CLR(w)   w &= ~( 0x1 << 3)

#define PMU_GLB_RST_FRC_EN_SET_VDEC_SET_POS      2
#define PMU_GLB_RST_FRC_EN_SET_VDEC_SET_LEN      1
#define PMU_GLB_RST_FRC_EN_SET_VDEC_SET_MASK     0x00000004
#define PMU_GLB_RST_FRC_EN_SET_VDEC_SET_VAL(w)   (((w) & 0x00000004) >> 2)
#define PMU_GLB_RST_FRC_EN_SET_VDEC_SET_SET(w)   w |= ( 0x1 << 2)
#define PMU_GLB_RST_FRC_EN_SET_VDEC_SET_CLR(w)   w &= ~( 0x1 << 2)

#define PMU_GLB_RST_FRC_EN_SET_VENC1_SET_POS      1
#define PMU_GLB_RST_FRC_EN_SET_VENC1_SET_LEN      1
#define PMU_GLB_RST_FRC_EN_SET_VENC1_SET_MASK     0x00000002
#define PMU_GLB_RST_FRC_EN_SET_VENC1_SET_VAL(w)   (((w) & 0x00000002) >> 1)
#define PMU_GLB_RST_FRC_EN_SET_VENC1_SET_SET(w)   w |= ( 0x1 << 1)
#define PMU_GLB_RST_FRC_EN_SET_VENC1_SET_CLR(w)   w &= ~( 0x1 << 1)

#define PMU_GLB_RST_FRC_EN_SET_VENC0_SET_POS      0
#define PMU_GLB_RST_FRC_EN_SET_VENC0_SET_LEN      1
#define PMU_GLB_RST_FRC_EN_SET_VENC0_SET_MASK     0x00000001
#define PMU_GLB_RST_FRC_EN_SET_VENC0_SET_VAL(w)   (((w) & 0x00000001) >> 0)
#define PMU_GLB_RST_FRC_EN_SET_VENC0_SET_SET(w)   w |= ( 0x1 << 0)
#define PMU_GLB_RST_FRC_EN_SET_VENC0_SET_CLR(w)   w &= ~( 0x1 << 0)

// REGISTER : RST_FRC_EN_CLR

#define PMU_GLB_RST_FRC_EN_CLR_ADDR      PMU_GLB_BASE_ADDR + 0x200
#define PMU_GLB_RST_FRC_EN_CLR_RSTVAL    0x0
#define PMU_GLB_RST_FRC_EN_CLR_ALL1      0x0
#define PMU_GLB_RST_FRC_EN_CLR_ALL0      0x0
#define PMU_GLB_RST_FRC_EN_CLR_ALL5A     0x0
#define PMU_GLB_RST_FRC_EN_CLR_ALLA5     0x0

// FIELD IN REGISTER : RST_FRC_EN_CLR

#define PMU_GLB_RST_FRC_EN_CLR_CPU7_CLR_POS      11
#define PMU_GLB_RST_FRC_EN_CLR_CPU7_CLR_LEN      1
#define PMU_GLB_RST_FRC_EN_CLR_CPU7_CLR_MASK     0x00000800
#define PMU_GLB_RST_FRC_EN_CLR_CPU7_CLR_VAL(w)   (((w) & 0x00000800) >> 11)
#define PMU_GLB_RST_FRC_EN_CLR_CPU7_CLR_SET(w)   w |= ( 0x1 << 11)
#define PMU_GLB_RST_FRC_EN_CLR_CPU7_CLR_CLR(w)   w &= ~( 0x1 << 11)

#define PMU_GLB_RST_FRC_EN_CLR_CPU6_CLR_POS      10
#define PMU_GLB_RST_FRC_EN_CLR_CPU6_CLR_LEN      1
#define PMU_GLB_RST_FRC_EN_CLR_CPU6_CLR_MASK     0x00000400
#define PMU_GLB_RST_FRC_EN_CLR_CPU6_CLR_VAL(w)   (((w) & 0x00000400) >> 10)
#define PMU_GLB_RST_FRC_EN_CLR_CPU6_CLR_SET(w)   w |= ( 0x1 << 10)
#define PMU_GLB_RST_FRC_EN_CLR_CPU6_CLR_CLR(w)   w &= ~( 0x1 << 10)

#define PMU_GLB_RST_FRC_EN_CLR_CPU5_CLR_POS      9
#define PMU_GLB_RST_FRC_EN_CLR_CPU5_CLR_LEN      1
#define PMU_GLB_RST_FRC_EN_CLR_CPU5_CLR_MASK     0x00000200
#define PMU_GLB_RST_FRC_EN_CLR_CPU5_CLR_VAL(w)   (((w) & 0x00000200) >> 9)
#define PMU_GLB_RST_FRC_EN_CLR_CPU5_CLR_SET(w)   w |= ( 0x1 << 9)
#define PMU_GLB_RST_FRC_EN_CLR_CPU5_CLR_CLR(w)   w &= ~( 0x1 << 9)

#define PMU_GLB_RST_FRC_EN_CLR_CPU4_CLR_POS      8
#define PMU_GLB_RST_FRC_EN_CLR_CPU4_CLR_LEN      1
#define PMU_GLB_RST_FRC_EN_CLR_CPU4_CLR_MASK     0x00000100
#define PMU_GLB_RST_FRC_EN_CLR_CPU4_CLR_VAL(w)   (((w) & 0x00000100) >> 8)
#define PMU_GLB_RST_FRC_EN_CLR_CPU4_CLR_SET(w)   w |= ( 0x1 << 8)
#define PMU_GLB_RST_FRC_EN_CLR_CPU4_CLR_CLR(w)   w &= ~( 0x1 << 8)

#define PMU_GLB_RST_FRC_EN_CLR_CPU3_CLR_POS      7
#define PMU_GLB_RST_FRC_EN_CLR_CPU3_CLR_LEN      1
#define PMU_GLB_RST_FRC_EN_CLR_CPU3_CLR_MASK     0x00000080
#define PMU_GLB_RST_FRC_EN_CLR_CPU3_CLR_VAL(w)   (((w) & 0x00000080) >> 7)
#define PMU_GLB_RST_FRC_EN_CLR_CPU3_CLR_SET(w)   w |= ( 0x1 << 7)
#define PMU_GLB_RST_FRC_EN_CLR_CPU3_CLR_CLR(w)   w &= ~( 0x1 << 7)

#define PMU_GLB_RST_FRC_EN_CLR_CPU2_CLR_POS      6
#define PMU_GLB_RST_FRC_EN_CLR_CPU2_CLR_LEN      1
#define PMU_GLB_RST_FRC_EN_CLR_CPU2_CLR_MASK     0x00000040
#define PMU_GLB_RST_FRC_EN_CLR_CPU2_CLR_VAL(w)   (((w) & 0x00000040) >> 6)
#define PMU_GLB_RST_FRC_EN_CLR_CPU2_CLR_SET(w)   w |= ( 0x1 << 6)
#define PMU_GLB_RST_FRC_EN_CLR_CPU2_CLR_CLR(w)   w &= ~( 0x1 << 6)

#define PMU_GLB_RST_FRC_EN_CLR_CPU1_CLR_POS      5
#define PMU_GLB_RST_FRC_EN_CLR_CPU1_CLR_LEN      1
#define PMU_GLB_RST_FRC_EN_CLR_CPU1_CLR_MASK     0x00000020
#define PMU_GLB_RST_FRC_EN_CLR_CPU1_CLR_VAL(w)   (((w) & 0x00000020) >> 5)
#define PMU_GLB_RST_FRC_EN_CLR_CPU1_CLR_SET(w)   w |= ( 0x1 << 5)
#define PMU_GLB_RST_FRC_EN_CLR_CPU1_CLR_CLR(w)   w &= ~( 0x1 << 5)

#define PMU_GLB_RST_FRC_EN_CLR_CPU0_CLR_POS      4
#define PMU_GLB_RST_FRC_EN_CLR_CPU0_CLR_LEN      1
#define PMU_GLB_RST_FRC_EN_CLR_CPU0_CLR_MASK     0x00000010
#define PMU_GLB_RST_FRC_EN_CLR_CPU0_CLR_VAL(w)   (((w) & 0x00000010) >> 4)
#define PMU_GLB_RST_FRC_EN_CLR_CPU0_CLR_SET(w)   w |= ( 0x1 << 4)
#define PMU_GLB_RST_FRC_EN_CLR_CPU0_CLR_CLR(w)   w &= ~( 0x1 << 4)

#define PMU_GLB_RST_FRC_EN_CLR_DSP_CLR_POS      3
#define PMU_GLB_RST_FRC_EN_CLR_DSP_CLR_LEN      1
#define PMU_GLB_RST_FRC_EN_CLR_DSP_CLR_MASK     0x00000008
#define PMU_GLB_RST_FRC_EN_CLR_DSP_CLR_VAL(w)   (((w) & 0x00000008) >> 3)
#define PMU_GLB_RST_FRC_EN_CLR_DSP_CLR_SET(w)   w |= ( 0x1 << 3)
#define PMU_GLB_RST_FRC_EN_CLR_DSP_CLR_CLR(w)   w &= ~( 0x1 << 3)

#define PMU_GLB_RST_FRC_EN_CLR_VDEC_CLR_POS      2
#define PMU_GLB_RST_FRC_EN_CLR_VDEC_CLR_LEN      1
#define PMU_GLB_RST_FRC_EN_CLR_VDEC_CLR_MASK     0x00000004
#define PMU_GLB_RST_FRC_EN_CLR_VDEC_CLR_VAL(w)   (((w) & 0x00000004) >> 2)
#define PMU_GLB_RST_FRC_EN_CLR_VDEC_CLR_SET(w)   w |= ( 0x1 << 2)
#define PMU_GLB_RST_FRC_EN_CLR_VDEC_CLR_CLR(w)   w &= ~( 0x1 << 2)

#define PMU_GLB_RST_FRC_EN_CLR_VENC1_CLR_POS      1
#define PMU_GLB_RST_FRC_EN_CLR_VENC1_CLR_LEN      1
#define PMU_GLB_RST_FRC_EN_CLR_VENC1_CLR_MASK     0x00000002
#define PMU_GLB_RST_FRC_EN_CLR_VENC1_CLR_VAL(w)   (((w) & 0x00000002) >> 1)
#define PMU_GLB_RST_FRC_EN_CLR_VENC1_CLR_SET(w)   w |= ( 0x1 << 1)
#define PMU_GLB_RST_FRC_EN_CLR_VENC1_CLR_CLR(w)   w &= ~( 0x1 << 1)

#define PMU_GLB_RST_FRC_EN_CLR_VENC0_CLR_POS      0
#define PMU_GLB_RST_FRC_EN_CLR_VENC0_CLR_LEN      1
#define PMU_GLB_RST_FRC_EN_CLR_VENC0_CLR_MASK     0x00000001
#define PMU_GLB_RST_FRC_EN_CLR_VENC0_CLR_VAL(w)   (((w) & 0x00000001) >> 0)
#define PMU_GLB_RST_FRC_EN_CLR_VENC0_CLR_SET(w)   w |= ( 0x1 << 0)
#define PMU_GLB_RST_FRC_EN_CLR_VENC0_CLR_CLR(w)   w &= ~( 0x1 << 0)

// REGISTER : RST_FRC_EN1_SET

#define PMU_GLB_RST_FRC_EN1_SET_ADDR      PMU_GLB_BASE_ADDR + 0x204
#define PMU_GLB_RST_FRC_EN1_SET_RSTVAL    0x0
#define PMU_GLB_RST_FRC_EN1_SET_ALL1      0x0
#define PMU_GLB_RST_FRC_EN1_SET_ALL0      0x0
#define PMU_GLB_RST_FRC_EN1_SET_ALL5A     0x0
#define PMU_GLB_RST_FRC_EN1_SET_ALLA5     0x0

// FIELD IN REGISTER : RST_FRC_EN1_SET

#define PMU_GLB_RST_FRC_EN1_SET_SEN_SET_POS      18
#define PMU_GLB_RST_FRC_EN1_SET_SEN_SET_LEN      1
#define PMU_GLB_RST_FRC_EN1_SET_SEN_SET_MASK     0x00040000
#define PMU_GLB_RST_FRC_EN1_SET_SEN_SET_VAL(w)   (((w) & 0x00040000) >> 18)
#define PMU_GLB_RST_FRC_EN1_SET_SEN_SET_SET(w)   w |= ( 0x1 << 18)
#define PMU_GLB_RST_FRC_EN1_SET_SEN_SET_CLR(w)   w &= ~( 0x1 << 18)

#define PMU_GLB_RST_FRC_EN1_SET_ISP_SET_POS      17
#define PMU_GLB_RST_FRC_EN1_SET_ISP_SET_LEN      1
#define PMU_GLB_RST_FRC_EN1_SET_ISP_SET_MASK     0x00020000
#define PMU_GLB_RST_FRC_EN1_SET_ISP_SET_VAL(w)   (((w) & 0x00020000) >> 17)
#define PMU_GLB_RST_FRC_EN1_SET_ISP_SET_SET(w)   w |= ( 0x1 << 17)
#define PMU_GLB_RST_FRC_EN1_SET_ISP_SET_CLR(w)   w &= ~( 0x1 << 17)

#define PMU_GLB_RST_FRC_EN1_SET_CPU_SET_POS      16
#define PMU_GLB_RST_FRC_EN1_SET_CPU_SET_LEN      1
#define PMU_GLB_RST_FRC_EN1_SET_CPU_SET_MASK     0x00010000
#define PMU_GLB_RST_FRC_EN1_SET_CPU_SET_VAL(w)   (((w) & 0x00010000) >> 16)
#define PMU_GLB_RST_FRC_EN1_SET_CPU_SET_SET(w)   w |= ( 0x1 << 16)
#define PMU_GLB_RST_FRC_EN1_SET_CPU_SET_CLR(w)   w &= ~( 0x1 << 16)

#define PMU_GLB_RST_FRC_EN1_SET_NPU_SET_POS      15
#define PMU_GLB_RST_FRC_EN1_SET_NPU_SET_LEN      1
#define PMU_GLB_RST_FRC_EN1_SET_NPU_SET_MASK     0x00008000
#define PMU_GLB_RST_FRC_EN1_SET_NPU_SET_VAL(w)   (((w) & 0x00008000) >> 15)
#define PMU_GLB_RST_FRC_EN1_SET_NPU_SET_SET(w)   w |= ( 0x1 << 15)
#define PMU_GLB_RST_FRC_EN1_SET_NPU_SET_CLR(w)   w &= ~( 0x1 << 15)

#define PMU_GLB_RST_FRC_EN1_SET_N8_SET_POS      14
#define PMU_GLB_RST_FRC_EN1_SET_N8_SET_LEN      1
#define PMU_GLB_RST_FRC_EN1_SET_N8_SET_MASK     0x00004000
#define PMU_GLB_RST_FRC_EN1_SET_N8_SET_VAL(w)   (((w) & 0x00004000) >> 14)
#define PMU_GLB_RST_FRC_EN1_SET_N8_SET_SET(w)   w |= ( 0x1 << 14)
#define PMU_GLB_RST_FRC_EN1_SET_N8_SET_CLR(w)   w &= ~( 0x1 << 14)

#define PMU_GLB_RST_FRC_EN1_SET_N7_SET_POS      13
#define PMU_GLB_RST_FRC_EN1_SET_N7_SET_LEN      1
#define PMU_GLB_RST_FRC_EN1_SET_N7_SET_MASK     0x00002000
#define PMU_GLB_RST_FRC_EN1_SET_N7_SET_VAL(w)   (((w) & 0x00002000) >> 13)
#define PMU_GLB_RST_FRC_EN1_SET_N7_SET_SET(w)   w |= ( 0x1 << 13)
#define PMU_GLB_RST_FRC_EN1_SET_N7_SET_CLR(w)   w &= ~( 0x1 << 13)

#define PMU_GLB_RST_FRC_EN1_SET_N6_SET_POS      12
#define PMU_GLB_RST_FRC_EN1_SET_N6_SET_LEN      1
#define PMU_GLB_RST_FRC_EN1_SET_N6_SET_MASK     0x00001000
#define PMU_GLB_RST_FRC_EN1_SET_N6_SET_VAL(w)   (((w) & 0x00001000) >> 12)
#define PMU_GLB_RST_FRC_EN1_SET_N6_SET_SET(w)   w |= ( 0x1 << 12)
#define PMU_GLB_RST_FRC_EN1_SET_N6_SET_CLR(w)   w &= ~( 0x1 << 12)

#define PMU_GLB_RST_FRC_EN1_SET_N5_SET_POS      11
#define PMU_GLB_RST_FRC_EN1_SET_N5_SET_LEN      1
#define PMU_GLB_RST_FRC_EN1_SET_N5_SET_MASK     0x00000800
#define PMU_GLB_RST_FRC_EN1_SET_N5_SET_VAL(w)   (((w) & 0x00000800) >> 11)
#define PMU_GLB_RST_FRC_EN1_SET_N5_SET_SET(w)   w |= ( 0x1 << 11)
#define PMU_GLB_RST_FRC_EN1_SET_N5_SET_CLR(w)   w &= ~( 0x1 << 11)

#define PMU_GLB_RST_FRC_EN1_SET_N4_SET_POS      10
#define PMU_GLB_RST_FRC_EN1_SET_N4_SET_LEN      1
#define PMU_GLB_RST_FRC_EN1_SET_N4_SET_MASK     0x00000400
#define PMU_GLB_RST_FRC_EN1_SET_N4_SET_VAL(w)   (((w) & 0x00000400) >> 10)
#define PMU_GLB_RST_FRC_EN1_SET_N4_SET_SET(w)   w |= ( 0x1 << 10)
#define PMU_GLB_RST_FRC_EN1_SET_N4_SET_CLR(w)   w &= ~( 0x1 << 10)

#define PMU_GLB_RST_FRC_EN1_SET_N3_SET_POS      9
#define PMU_GLB_RST_FRC_EN1_SET_N3_SET_LEN      1
#define PMU_GLB_RST_FRC_EN1_SET_N3_SET_MASK     0x00000200
#define PMU_GLB_RST_FRC_EN1_SET_N3_SET_VAL(w)   (((w) & 0x00000200) >> 9)
#define PMU_GLB_RST_FRC_EN1_SET_N3_SET_SET(w)   w |= ( 0x1 << 9)
#define PMU_GLB_RST_FRC_EN1_SET_N3_SET_CLR(w)   w &= ~( 0x1 << 9)

#define PMU_GLB_RST_FRC_EN1_SET_N2_SET_POS      8
#define PMU_GLB_RST_FRC_EN1_SET_N2_SET_LEN      1
#define PMU_GLB_RST_FRC_EN1_SET_N2_SET_MASK     0x00000100
#define PMU_GLB_RST_FRC_EN1_SET_N2_SET_VAL(w)   (((w) & 0x00000100) >> 8)
#define PMU_GLB_RST_FRC_EN1_SET_N2_SET_SET(w)   w |= ( 0x1 << 8)
#define PMU_GLB_RST_FRC_EN1_SET_N2_SET_CLR(w)   w &= ~( 0x1 << 8)

#define PMU_GLB_RST_FRC_EN1_SET_N1_SET_POS      7
#define PMU_GLB_RST_FRC_EN1_SET_N1_SET_LEN      1
#define PMU_GLB_RST_FRC_EN1_SET_N1_SET_MASK     0x00000080
#define PMU_GLB_RST_FRC_EN1_SET_N1_SET_VAL(w)   (((w) & 0x00000080) >> 7)
#define PMU_GLB_RST_FRC_EN1_SET_N1_SET_SET(w)   w |= ( 0x1 << 7)
#define PMU_GLB_RST_FRC_EN1_SET_N1_SET_CLR(w)   w &= ~( 0x1 << 7)

#define PMU_GLB_RST_FRC_EN1_SET_N0_SET_POS      6
#define PMU_GLB_RST_FRC_EN1_SET_N0_SET_LEN      1
#define PMU_GLB_RST_FRC_EN1_SET_N0_SET_MASK     0x00000040
#define PMU_GLB_RST_FRC_EN1_SET_N0_SET_VAL(w)   (((w) & 0x00000040) >> 6)
#define PMU_GLB_RST_FRC_EN1_SET_N0_SET_SET(w)   w |= ( 0x1 << 6)
#define PMU_GLB_RST_FRC_EN1_SET_N0_SET_CLR(w)   w &= ~( 0x1 << 6)

#define PMU_GLB_RST_FRC_EN1_SET_MM_SET_POS      5
#define PMU_GLB_RST_FRC_EN1_SET_MM_SET_LEN      1
#define PMU_GLB_RST_FRC_EN1_SET_MM_SET_MASK     0x00000020
#define PMU_GLB_RST_FRC_EN1_SET_MM_SET_VAL(w)   (((w) & 0x00000020) >> 5)
#define PMU_GLB_RST_FRC_EN1_SET_MM_SET_SET(w)   w |= ( 0x1 << 5)
#define PMU_GLB_RST_FRC_EN1_SET_MM_SET_CLR(w)   w &= ~( 0x1 << 5)

#define PMU_GLB_RST_FRC_EN1_SET_PERIP_SET_POS      4
#define PMU_GLB_RST_FRC_EN1_SET_PERIP_SET_LEN      1
#define PMU_GLB_RST_FRC_EN1_SET_PERIP_SET_MASK     0x00000010
#define PMU_GLB_RST_FRC_EN1_SET_PERIP_SET_VAL(w)   (((w) & 0x00000010) >> 4)
#define PMU_GLB_RST_FRC_EN1_SET_PERIP_SET_SET(w)   w |= ( 0x1 << 4)
#define PMU_GLB_RST_FRC_EN1_SET_PERIP_SET_CLR(w)   w &= ~( 0x1 << 4)

#define PMU_GLB_RST_FRC_EN1_SET_PIPE_SET_POS      3
#define PMU_GLB_RST_FRC_EN1_SET_PIPE_SET_LEN      1
#define PMU_GLB_RST_FRC_EN1_SET_PIPE_SET_MASK     0x00000008
#define PMU_GLB_RST_FRC_EN1_SET_PIPE_SET_VAL(w)   (((w) & 0x00000008) >> 3)
#define PMU_GLB_RST_FRC_EN1_SET_PIPE_SET_SET(w)   w |= ( 0x1 << 3)
#define PMU_GLB_RST_FRC_EN1_SET_PIPE_SET_CLR(w)   w &= ~( 0x1 << 3)

#define PMU_GLB_RST_FRC_EN1_SET_FLASH_SET_POS      2
#define PMU_GLB_RST_FRC_EN1_SET_FLASH_SET_LEN      1
#define PMU_GLB_RST_FRC_EN1_SET_FLASH_SET_MASK     0x00000004
#define PMU_GLB_RST_FRC_EN1_SET_FLASH_SET_VAL(w)   (((w) & 0x00000004) >> 2)
#define PMU_GLB_RST_FRC_EN1_SET_FLASH_SET_SET(w)   w |= ( 0x1 << 2)
#define PMU_GLB_RST_FRC_EN1_SET_FLASH_SET_CLR(w)   w &= ~( 0x1 << 2)

#define PMU_GLB_RST_FRC_EN1_SET_DDR1_SET_POS      1
#define PMU_GLB_RST_FRC_EN1_SET_DDR1_SET_LEN      1
#define PMU_GLB_RST_FRC_EN1_SET_DDR1_SET_MASK     0x00000002
#define PMU_GLB_RST_FRC_EN1_SET_DDR1_SET_VAL(w)   (((w) & 0x00000002) >> 1)
#define PMU_GLB_RST_FRC_EN1_SET_DDR1_SET_SET(w)   w |= ( 0x1 << 1)
#define PMU_GLB_RST_FRC_EN1_SET_DDR1_SET_CLR(w)   w &= ~( 0x1 << 1)

#define PMU_GLB_RST_FRC_EN1_SET_DDR0_SET_POS      0
#define PMU_GLB_RST_FRC_EN1_SET_DDR0_SET_LEN      1
#define PMU_GLB_RST_FRC_EN1_SET_DDR0_SET_MASK     0x00000001
#define PMU_GLB_RST_FRC_EN1_SET_DDR0_SET_VAL(w)   (((w) & 0x00000001) >> 0)
#define PMU_GLB_RST_FRC_EN1_SET_DDR0_SET_SET(w)   w |= ( 0x1 << 0)
#define PMU_GLB_RST_FRC_EN1_SET_DDR0_SET_CLR(w)   w &= ~( 0x1 << 0)

// REGISTER : RST_FRC_EN1_CLR

#define PMU_GLB_RST_FRC_EN1_CLR_ADDR      PMU_GLB_BASE_ADDR + 0x208
#define PMU_GLB_RST_FRC_EN1_CLR_RSTVAL    0x0
#define PMU_GLB_RST_FRC_EN1_CLR_ALL1      0x0
#define PMU_GLB_RST_FRC_EN1_CLR_ALL0      0x0
#define PMU_GLB_RST_FRC_EN1_CLR_ALL5A     0x0
#define PMU_GLB_RST_FRC_EN1_CLR_ALLA5     0x0

// FIELD IN REGISTER : RST_FRC_EN1_CLR

#define PMU_GLB_RST_FRC_EN1_CLR_SEN_CLR_POS      18
#define PMU_GLB_RST_FRC_EN1_CLR_SEN_CLR_LEN      1
#define PMU_GLB_RST_FRC_EN1_CLR_SEN_CLR_MASK     0x00040000
#define PMU_GLB_RST_FRC_EN1_CLR_SEN_CLR_VAL(w)   (((w) & 0x00040000) >> 18)
#define PMU_GLB_RST_FRC_EN1_CLR_SEN_CLR_SET(w)   w |= ( 0x1 << 18)
#define PMU_GLB_RST_FRC_EN1_CLR_SEN_CLR_CLR(w)   w &= ~( 0x1 << 18)

#define PMU_GLB_RST_FRC_EN1_CLR_ISP_CLR_POS      17
#define PMU_GLB_RST_FRC_EN1_CLR_ISP_CLR_LEN      1
#define PMU_GLB_RST_FRC_EN1_CLR_ISP_CLR_MASK     0x00020000
#define PMU_GLB_RST_FRC_EN1_CLR_ISP_CLR_VAL(w)   (((w) & 0x00020000) >> 17)
#define PMU_GLB_RST_FRC_EN1_CLR_ISP_CLR_SET(w)   w |= ( 0x1 << 17)
#define PMU_GLB_RST_FRC_EN1_CLR_ISP_CLR_CLR(w)   w &= ~( 0x1 << 17)

#define PMU_GLB_RST_FRC_EN1_CLR_CPU_CLR_POS      16
#define PMU_GLB_RST_FRC_EN1_CLR_CPU_CLR_LEN      1
#define PMU_GLB_RST_FRC_EN1_CLR_CPU_CLR_MASK     0x00010000
#define PMU_GLB_RST_FRC_EN1_CLR_CPU_CLR_VAL(w)   (((w) & 0x00010000) >> 16)
#define PMU_GLB_RST_FRC_EN1_CLR_CPU_CLR_SET(w)   w |= ( 0x1 << 16)
#define PMU_GLB_RST_FRC_EN1_CLR_CPU_CLR_CLR(w)   w &= ~( 0x1 << 16)

#define PMU_GLB_RST_FRC_EN1_CLR_NPU_CLR_POS      15
#define PMU_GLB_RST_FRC_EN1_CLR_NPU_CLR_LEN      1
#define PMU_GLB_RST_FRC_EN1_CLR_NPU_CLR_MASK     0x00008000
#define PMU_GLB_RST_FRC_EN1_CLR_NPU_CLR_VAL(w)   (((w) & 0x00008000) >> 15)
#define PMU_GLB_RST_FRC_EN1_CLR_NPU_CLR_SET(w)   w |= ( 0x1 << 15)
#define PMU_GLB_RST_FRC_EN1_CLR_NPU_CLR_CLR(w)   w &= ~( 0x1 << 15)

#define PMU_GLB_RST_FRC_EN1_CLR_N8_CLR_POS      14
#define PMU_GLB_RST_FRC_EN1_CLR_N8_CLR_LEN      1
#define PMU_GLB_RST_FRC_EN1_CLR_N8_CLR_MASK     0x00004000
#define PMU_GLB_RST_FRC_EN1_CLR_N8_CLR_VAL(w)   (((w) & 0x00004000) >> 14)
#define PMU_GLB_RST_FRC_EN1_CLR_N8_CLR_SET(w)   w |= ( 0x1 << 14)
#define PMU_GLB_RST_FRC_EN1_CLR_N8_CLR_CLR(w)   w &= ~( 0x1 << 14)

#define PMU_GLB_RST_FRC_EN1_CLR_N7_CLR_POS      13
#define PMU_GLB_RST_FRC_EN1_CLR_N7_CLR_LEN      1
#define PMU_GLB_RST_FRC_EN1_CLR_N7_CLR_MASK     0x00002000
#define PMU_GLB_RST_FRC_EN1_CLR_N7_CLR_VAL(w)   (((w) & 0x00002000) >> 13)
#define PMU_GLB_RST_FRC_EN1_CLR_N7_CLR_SET(w)   w |= ( 0x1 << 13)
#define PMU_GLB_RST_FRC_EN1_CLR_N7_CLR_CLR(w)   w &= ~( 0x1 << 13)

#define PMU_GLB_RST_FRC_EN1_CLR_N6_CLR_POS      12
#define PMU_GLB_RST_FRC_EN1_CLR_N6_CLR_LEN      1
#define PMU_GLB_RST_FRC_EN1_CLR_N6_CLR_MASK     0x00001000
#define PMU_GLB_RST_FRC_EN1_CLR_N6_CLR_VAL(w)   (((w) & 0x00001000) >> 12)
#define PMU_GLB_RST_FRC_EN1_CLR_N6_CLR_SET(w)   w |= ( 0x1 << 12)
#define PMU_GLB_RST_FRC_EN1_CLR_N6_CLR_CLR(w)   w &= ~( 0x1 << 12)

#define PMU_GLB_RST_FRC_EN1_CLR_N5_CLR_POS      11
#define PMU_GLB_RST_FRC_EN1_CLR_N5_CLR_LEN      1
#define PMU_GLB_RST_FRC_EN1_CLR_N5_CLR_MASK     0x00000800
#define PMU_GLB_RST_FRC_EN1_CLR_N5_CLR_VAL(w)   (((w) & 0x00000800) >> 11)
#define PMU_GLB_RST_FRC_EN1_CLR_N5_CLR_SET(w)   w |= ( 0x1 << 11)
#define PMU_GLB_RST_FRC_EN1_CLR_N5_CLR_CLR(w)   w &= ~( 0x1 << 11)

#define PMU_GLB_RST_FRC_EN1_CLR_N4_CLR_POS      10
#define PMU_GLB_RST_FRC_EN1_CLR_N4_CLR_LEN      1
#define PMU_GLB_RST_FRC_EN1_CLR_N4_CLR_MASK     0x00000400
#define PMU_GLB_RST_FRC_EN1_CLR_N4_CLR_VAL(w)   (((w) & 0x00000400) >> 10)
#define PMU_GLB_RST_FRC_EN1_CLR_N4_CLR_SET(w)   w |= ( 0x1 << 10)
#define PMU_GLB_RST_FRC_EN1_CLR_N4_CLR_CLR(w)   w &= ~( 0x1 << 10)

#define PMU_GLB_RST_FRC_EN1_CLR_N3_CLR_POS      9
#define PMU_GLB_RST_FRC_EN1_CLR_N3_CLR_LEN      1
#define PMU_GLB_RST_FRC_EN1_CLR_N3_CLR_MASK     0x00000200
#define PMU_GLB_RST_FRC_EN1_CLR_N3_CLR_VAL(w)   (((w) & 0x00000200) >> 9)
#define PMU_GLB_RST_FRC_EN1_CLR_N3_CLR_SET(w)   w |= ( 0x1 << 9)
#define PMU_GLB_RST_FRC_EN1_CLR_N3_CLR_CLR(w)   w &= ~( 0x1 << 9)

#define PMU_GLB_RST_FRC_EN1_CLR_N2_CLR_POS      8
#define PMU_GLB_RST_FRC_EN1_CLR_N2_CLR_LEN      1
#define PMU_GLB_RST_FRC_EN1_CLR_N2_CLR_MASK     0x00000100
#define PMU_GLB_RST_FRC_EN1_CLR_N2_CLR_VAL(w)   (((w) & 0x00000100) >> 8)
#define PMU_GLB_RST_FRC_EN1_CLR_N2_CLR_SET(w)   w |= ( 0x1 << 8)
#define PMU_GLB_RST_FRC_EN1_CLR_N2_CLR_CLR(w)   w &= ~( 0x1 << 8)

#define PMU_GLB_RST_FRC_EN1_CLR_N1_CLR_POS      7
#define PMU_GLB_RST_FRC_EN1_CLR_N1_CLR_LEN      1
#define PMU_GLB_RST_FRC_EN1_CLR_N1_CLR_MASK     0x00000080
#define PMU_GLB_RST_FRC_EN1_CLR_N1_CLR_VAL(w)   (((w) & 0x00000080) >> 7)
#define PMU_GLB_RST_FRC_EN1_CLR_N1_CLR_SET(w)   w |= ( 0x1 << 7)
#define PMU_GLB_RST_FRC_EN1_CLR_N1_CLR_CLR(w)   w &= ~( 0x1 << 7)

#define PMU_GLB_RST_FRC_EN1_CLR_N0_CLR_POS      6
#define PMU_GLB_RST_FRC_EN1_CLR_N0_CLR_LEN      1
#define PMU_GLB_RST_FRC_EN1_CLR_N0_CLR_MASK     0x00000040
#define PMU_GLB_RST_FRC_EN1_CLR_N0_CLR_VAL(w)   (((w) & 0x00000040) >> 6)
#define PMU_GLB_RST_FRC_EN1_CLR_N0_CLR_SET(w)   w |= ( 0x1 << 6)
#define PMU_GLB_RST_FRC_EN1_CLR_N0_CLR_CLR(w)   w &= ~( 0x1 << 6)

#define PMU_GLB_RST_FRC_EN1_CLR_MM_CLR_POS      5
#define PMU_GLB_RST_FRC_EN1_CLR_MM_CLR_LEN      1
#define PMU_GLB_RST_FRC_EN1_CLR_MM_CLR_MASK     0x00000020
#define PMU_GLB_RST_FRC_EN1_CLR_MM_CLR_VAL(w)   (((w) & 0x00000020) >> 5)
#define PMU_GLB_RST_FRC_EN1_CLR_MM_CLR_SET(w)   w |= ( 0x1 << 5)
#define PMU_GLB_RST_FRC_EN1_CLR_MM_CLR_CLR(w)   w &= ~( 0x1 << 5)

#define PMU_GLB_RST_FRC_EN1_CLR_PERIP_CLR_POS      4
#define PMU_GLB_RST_FRC_EN1_CLR_PERIP_CLR_LEN      1
#define PMU_GLB_RST_FRC_EN1_CLR_PERIP_CLR_MASK     0x00000010
#define PMU_GLB_RST_FRC_EN1_CLR_PERIP_CLR_VAL(w)   (((w) & 0x00000010) >> 4)
#define PMU_GLB_RST_FRC_EN1_CLR_PERIP_CLR_SET(w)   w |= ( 0x1 << 4)
#define PMU_GLB_RST_FRC_EN1_CLR_PERIP_CLR_CLR(w)   w &= ~( 0x1 << 4)

#define PMU_GLB_RST_FRC_EN1_CLR_PIPE_CLR_POS      3
#define PMU_GLB_RST_FRC_EN1_CLR_PIPE_CLR_LEN      1
#define PMU_GLB_RST_FRC_EN1_CLR_PIPE_CLR_MASK     0x00000008
#define PMU_GLB_RST_FRC_EN1_CLR_PIPE_CLR_VAL(w)   (((w) & 0x00000008) >> 3)
#define PMU_GLB_RST_FRC_EN1_CLR_PIPE_CLR_SET(w)   w |= ( 0x1 << 3)
#define PMU_GLB_RST_FRC_EN1_CLR_PIPE_CLR_CLR(w)   w &= ~( 0x1 << 3)

#define PMU_GLB_RST_FRC_EN1_CLR_FLASH_CLR_POS      2
#define PMU_GLB_RST_FRC_EN1_CLR_FLASH_CLR_LEN      1
#define PMU_GLB_RST_FRC_EN1_CLR_FLASH_CLR_MASK     0x00000004
#define PMU_GLB_RST_FRC_EN1_CLR_FLASH_CLR_VAL(w)   (((w) & 0x00000004) >> 2)
#define PMU_GLB_RST_FRC_EN1_CLR_FLASH_CLR_SET(w)   w |= ( 0x1 << 2)
#define PMU_GLB_RST_FRC_EN1_CLR_FLASH_CLR_CLR(w)   w &= ~( 0x1 << 2)

#define PMU_GLB_RST_FRC_EN1_CLR_DDR1_CLR_POS      1
#define PMU_GLB_RST_FRC_EN1_CLR_DDR1_CLR_LEN      1
#define PMU_GLB_RST_FRC_EN1_CLR_DDR1_CLR_MASK     0x00000002
#define PMU_GLB_RST_FRC_EN1_CLR_DDR1_CLR_VAL(w)   (((w) & 0x00000002) >> 1)
#define PMU_GLB_RST_FRC_EN1_CLR_DDR1_CLR_SET(w)   w |= ( 0x1 << 1)
#define PMU_GLB_RST_FRC_EN1_CLR_DDR1_CLR_CLR(w)   w &= ~( 0x1 << 1)

#define PMU_GLB_RST_FRC_EN1_CLR_DDR0_CLR_POS      0
#define PMU_GLB_RST_FRC_EN1_CLR_DDR0_CLR_LEN      1
#define PMU_GLB_RST_FRC_EN1_CLR_DDR0_CLR_MASK     0x00000001
#define PMU_GLB_RST_FRC_EN1_CLR_DDR0_CLR_VAL(w)   (((w) & 0x00000001) >> 0)
#define PMU_GLB_RST_FRC_EN1_CLR_DDR0_CLR_SET(w)   w |= ( 0x1 << 0)
#define PMU_GLB_RST_FRC_EN1_CLR_DDR0_CLR_CLR(w)   w &= ~( 0x1 << 0)

// REGISTER : RST_FRC_SW_SET

#define PMU_GLB_RST_FRC_SW_SET_ADDR      PMU_GLB_BASE_ADDR + 0x20C
#define PMU_GLB_RST_FRC_SW_SET_RSTVAL    0x0
#define PMU_GLB_RST_FRC_SW_SET_ALL1      0x0
#define PMU_GLB_RST_FRC_SW_SET_ALL0      0x0
#define PMU_GLB_RST_FRC_SW_SET_ALL5A     0x0
#define PMU_GLB_RST_FRC_SW_SET_ALLA5     0x0

// FIELD IN REGISTER : RST_FRC_SW_SET

#define PMU_GLB_RST_FRC_SW_SET_CPU7_SET_POS      11
#define PMU_GLB_RST_FRC_SW_SET_CPU7_SET_LEN      1
#define PMU_GLB_RST_FRC_SW_SET_CPU7_SET_MASK     0x00000800
#define PMU_GLB_RST_FRC_SW_SET_CPU7_SET_VAL(w)   (((w) & 0x00000800) >> 11)
#define PMU_GLB_RST_FRC_SW_SET_CPU7_SET_SET(w)   w |= ( 0x1 << 11)
#define PMU_GLB_RST_FRC_SW_SET_CPU7_SET_CLR(w)   w &= ~( 0x1 << 11)

#define PMU_GLB_RST_FRC_SW_SET_CPU6_SET_POS      10
#define PMU_GLB_RST_FRC_SW_SET_CPU6_SET_LEN      1
#define PMU_GLB_RST_FRC_SW_SET_CPU6_SET_MASK     0x00000400
#define PMU_GLB_RST_FRC_SW_SET_CPU6_SET_VAL(w)   (((w) & 0x00000400) >> 10)
#define PMU_GLB_RST_FRC_SW_SET_CPU6_SET_SET(w)   w |= ( 0x1 << 10)
#define PMU_GLB_RST_FRC_SW_SET_CPU6_SET_CLR(w)   w &= ~( 0x1 << 10)

#define PMU_GLB_RST_FRC_SW_SET_CPU5_SET_POS      9
#define PMU_GLB_RST_FRC_SW_SET_CPU5_SET_LEN      1
#define PMU_GLB_RST_FRC_SW_SET_CPU5_SET_MASK     0x00000200
#define PMU_GLB_RST_FRC_SW_SET_CPU5_SET_VAL(w)   (((w) & 0x00000200) >> 9)
#define PMU_GLB_RST_FRC_SW_SET_CPU5_SET_SET(w)   w |= ( 0x1 << 9)
#define PMU_GLB_RST_FRC_SW_SET_CPU5_SET_CLR(w)   w &= ~( 0x1 << 9)

#define PMU_GLB_RST_FRC_SW_SET_CPU4_SET_POS      8
#define PMU_GLB_RST_FRC_SW_SET_CPU4_SET_LEN      1
#define PMU_GLB_RST_FRC_SW_SET_CPU4_SET_MASK     0x00000100
#define PMU_GLB_RST_FRC_SW_SET_CPU4_SET_VAL(w)   (((w) & 0x00000100) >> 8)
#define PMU_GLB_RST_FRC_SW_SET_CPU4_SET_SET(w)   w |= ( 0x1 << 8)
#define PMU_GLB_RST_FRC_SW_SET_CPU4_SET_CLR(w)   w &= ~( 0x1 << 8)

#define PMU_GLB_RST_FRC_SW_SET_CPU3_SET_POS      7
#define PMU_GLB_RST_FRC_SW_SET_CPU3_SET_LEN      1
#define PMU_GLB_RST_FRC_SW_SET_CPU3_SET_MASK     0x00000080
#define PMU_GLB_RST_FRC_SW_SET_CPU3_SET_VAL(w)   (((w) & 0x00000080) >> 7)
#define PMU_GLB_RST_FRC_SW_SET_CPU3_SET_SET(w)   w |= ( 0x1 << 7)
#define PMU_GLB_RST_FRC_SW_SET_CPU3_SET_CLR(w)   w &= ~( 0x1 << 7)

#define PMU_GLB_RST_FRC_SW_SET_CPU2_SET_POS      6
#define PMU_GLB_RST_FRC_SW_SET_CPU2_SET_LEN      1
#define PMU_GLB_RST_FRC_SW_SET_CPU2_SET_MASK     0x00000040
#define PMU_GLB_RST_FRC_SW_SET_CPU2_SET_VAL(w)   (((w) & 0x00000040) >> 6)
#define PMU_GLB_RST_FRC_SW_SET_CPU2_SET_SET(w)   w |= ( 0x1 << 6)
#define PMU_GLB_RST_FRC_SW_SET_CPU2_SET_CLR(w)   w &= ~( 0x1 << 6)

#define PMU_GLB_RST_FRC_SW_SET_CPU1_SET_POS      5
#define PMU_GLB_RST_FRC_SW_SET_CPU1_SET_LEN      1
#define PMU_GLB_RST_FRC_SW_SET_CPU1_SET_MASK     0x00000020
#define PMU_GLB_RST_FRC_SW_SET_CPU1_SET_VAL(w)   (((w) & 0x00000020) >> 5)
#define PMU_GLB_RST_FRC_SW_SET_CPU1_SET_SET(w)   w |= ( 0x1 << 5)
#define PMU_GLB_RST_FRC_SW_SET_CPU1_SET_CLR(w)   w &= ~( 0x1 << 5)

#define PMU_GLB_RST_FRC_SW_SET_CPU0_SET_POS      4
#define PMU_GLB_RST_FRC_SW_SET_CPU0_SET_LEN      1
#define PMU_GLB_RST_FRC_SW_SET_CPU0_SET_MASK     0x00000010
#define PMU_GLB_RST_FRC_SW_SET_CPU0_SET_VAL(w)   (((w) & 0x00000010) >> 4)
#define PMU_GLB_RST_FRC_SW_SET_CPU0_SET_SET(w)   w |= ( 0x1 << 4)
#define PMU_GLB_RST_FRC_SW_SET_CPU0_SET_CLR(w)   w &= ~( 0x1 << 4)

#define PMU_GLB_RST_FRC_SW_SET_DSP_SET_POS      3
#define PMU_GLB_RST_FRC_SW_SET_DSP_SET_LEN      1
#define PMU_GLB_RST_FRC_SW_SET_DSP_SET_MASK     0x00000008
#define PMU_GLB_RST_FRC_SW_SET_DSP_SET_VAL(w)   (((w) & 0x00000008) >> 3)
#define PMU_GLB_RST_FRC_SW_SET_DSP_SET_SET(w)   w |= ( 0x1 << 3)
#define PMU_GLB_RST_FRC_SW_SET_DSP_SET_CLR(w)   w &= ~( 0x1 << 3)

#define PMU_GLB_RST_FRC_SW_SET_VDEC_SET_POS      2
#define PMU_GLB_RST_FRC_SW_SET_VDEC_SET_LEN      1
#define PMU_GLB_RST_FRC_SW_SET_VDEC_SET_MASK     0x00000004
#define PMU_GLB_RST_FRC_SW_SET_VDEC_SET_VAL(w)   (((w) & 0x00000004) >> 2)
#define PMU_GLB_RST_FRC_SW_SET_VDEC_SET_SET(w)   w |= ( 0x1 << 2)
#define PMU_GLB_RST_FRC_SW_SET_VDEC_SET_CLR(w)   w &= ~( 0x1 << 2)

#define PMU_GLB_RST_FRC_SW_SET_VENC1_SET_POS      1
#define PMU_GLB_RST_FRC_SW_SET_VENC1_SET_LEN      1
#define PMU_GLB_RST_FRC_SW_SET_VENC1_SET_MASK     0x00000002
#define PMU_GLB_RST_FRC_SW_SET_VENC1_SET_VAL(w)   (((w) & 0x00000002) >> 1)
#define PMU_GLB_RST_FRC_SW_SET_VENC1_SET_SET(w)   w |= ( 0x1 << 1)
#define PMU_GLB_RST_FRC_SW_SET_VENC1_SET_CLR(w)   w &= ~( 0x1 << 1)

#define PMU_GLB_RST_FRC_SW_SET_VENC0_SET_POS      0
#define PMU_GLB_RST_FRC_SW_SET_VENC0_SET_LEN      1
#define PMU_GLB_RST_FRC_SW_SET_VENC0_SET_MASK     0x00000001
#define PMU_GLB_RST_FRC_SW_SET_VENC0_SET_VAL(w)   (((w) & 0x00000001) >> 0)
#define PMU_GLB_RST_FRC_SW_SET_VENC0_SET_SET(w)   w |= ( 0x1 << 0)
#define PMU_GLB_RST_FRC_SW_SET_VENC0_SET_CLR(w)   w &= ~( 0x1 << 0)

// REGISTER : RST_FRC_SW_CLR

#define PMU_GLB_RST_FRC_SW_CLR_ADDR      PMU_GLB_BASE_ADDR + 0x210
#define PMU_GLB_RST_FRC_SW_CLR_RSTVAL    0x0
#define PMU_GLB_RST_FRC_SW_CLR_ALL1      0x0
#define PMU_GLB_RST_FRC_SW_CLR_ALL0      0x0
#define PMU_GLB_RST_FRC_SW_CLR_ALL5A     0x0
#define PMU_GLB_RST_FRC_SW_CLR_ALLA5     0x0

// FIELD IN REGISTER : RST_FRC_SW_CLR

#define PMU_GLB_RST_FRC_SW_CLR_CPU7_CLR_POS      11
#define PMU_GLB_RST_FRC_SW_CLR_CPU7_CLR_LEN      1
#define PMU_GLB_RST_FRC_SW_CLR_CPU7_CLR_MASK     0x00000800
#define PMU_GLB_RST_FRC_SW_CLR_CPU7_CLR_VAL(w)   (((w) & 0x00000800) >> 11)
#define PMU_GLB_RST_FRC_SW_CLR_CPU7_CLR_SET(w)   w |= ( 0x1 << 11)
#define PMU_GLB_RST_FRC_SW_CLR_CPU7_CLR_CLR(w)   w &= ~( 0x1 << 11)

#define PMU_GLB_RST_FRC_SW_CLR_CPU6_CLR_POS      10
#define PMU_GLB_RST_FRC_SW_CLR_CPU6_CLR_LEN      1
#define PMU_GLB_RST_FRC_SW_CLR_CPU6_CLR_MASK     0x00000400
#define PMU_GLB_RST_FRC_SW_CLR_CPU6_CLR_VAL(w)   (((w) & 0x00000400) >> 10)
#define PMU_GLB_RST_FRC_SW_CLR_CPU6_CLR_SET(w)   w |= ( 0x1 << 10)
#define PMU_GLB_RST_FRC_SW_CLR_CPU6_CLR_CLR(w)   w &= ~( 0x1 << 10)

#define PMU_GLB_RST_FRC_SW_CLR_CPU5_CLR_POS      9
#define PMU_GLB_RST_FRC_SW_CLR_CPU5_CLR_LEN      1
#define PMU_GLB_RST_FRC_SW_CLR_CPU5_CLR_MASK     0x00000200
#define PMU_GLB_RST_FRC_SW_CLR_CPU5_CLR_VAL(w)   (((w) & 0x00000200) >> 9)
#define PMU_GLB_RST_FRC_SW_CLR_CPU5_CLR_SET(w)   w |= ( 0x1 << 9)
#define PMU_GLB_RST_FRC_SW_CLR_CPU5_CLR_CLR(w)   w &= ~( 0x1 << 9)

#define PMU_GLB_RST_FRC_SW_CLR_CPU4_CLR_POS      8
#define PMU_GLB_RST_FRC_SW_CLR_CPU4_CLR_LEN      1
#define PMU_GLB_RST_FRC_SW_CLR_CPU4_CLR_MASK     0x00000100
#define PMU_GLB_RST_FRC_SW_CLR_CPU4_CLR_VAL(w)   (((w) & 0x00000100) >> 8)
#define PMU_GLB_RST_FRC_SW_CLR_CPU4_CLR_SET(w)   w |= ( 0x1 << 8)
#define PMU_GLB_RST_FRC_SW_CLR_CPU4_CLR_CLR(w)   w &= ~( 0x1 << 8)

#define PMU_GLB_RST_FRC_SW_CLR_CPU3_CLR_POS      7
#define PMU_GLB_RST_FRC_SW_CLR_CPU3_CLR_LEN      1
#define PMU_GLB_RST_FRC_SW_CLR_CPU3_CLR_MASK     0x00000080
#define PMU_GLB_RST_FRC_SW_CLR_CPU3_CLR_VAL(w)   (((w) & 0x00000080) >> 7)
#define PMU_GLB_RST_FRC_SW_CLR_CPU3_CLR_SET(w)   w |= ( 0x1 << 7)
#define PMU_GLB_RST_FRC_SW_CLR_CPU3_CLR_CLR(w)   w &= ~( 0x1 << 7)

#define PMU_GLB_RST_FRC_SW_CLR_CPU2_CLR_POS      6
#define PMU_GLB_RST_FRC_SW_CLR_CPU2_CLR_LEN      1
#define PMU_GLB_RST_FRC_SW_CLR_CPU2_CLR_MASK     0x00000040
#define PMU_GLB_RST_FRC_SW_CLR_CPU2_CLR_VAL(w)   (((w) & 0x00000040) >> 6)
#define PMU_GLB_RST_FRC_SW_CLR_CPU2_CLR_SET(w)   w |= ( 0x1 << 6)
#define PMU_GLB_RST_FRC_SW_CLR_CPU2_CLR_CLR(w)   w &= ~( 0x1 << 6)

#define PMU_GLB_RST_FRC_SW_CLR_CPU1_CLR_POS      5
#define PMU_GLB_RST_FRC_SW_CLR_CPU1_CLR_LEN      1
#define PMU_GLB_RST_FRC_SW_CLR_CPU1_CLR_MASK     0x00000020
#define PMU_GLB_RST_FRC_SW_CLR_CPU1_CLR_VAL(w)   (((w) & 0x00000020) >> 5)
#define PMU_GLB_RST_FRC_SW_CLR_CPU1_CLR_SET(w)   w |= ( 0x1 << 5)
#define PMU_GLB_RST_FRC_SW_CLR_CPU1_CLR_CLR(w)   w &= ~( 0x1 << 5)

#define PMU_GLB_RST_FRC_SW_CLR_CPU0_CLR_POS      4
#define PMU_GLB_RST_FRC_SW_CLR_CPU0_CLR_LEN      1
#define PMU_GLB_RST_FRC_SW_CLR_CPU0_CLR_MASK     0x00000010
#define PMU_GLB_RST_FRC_SW_CLR_CPU0_CLR_VAL(w)   (((w) & 0x00000010) >> 4)
#define PMU_GLB_RST_FRC_SW_CLR_CPU0_CLR_SET(w)   w |= ( 0x1 << 4)
#define PMU_GLB_RST_FRC_SW_CLR_CPU0_CLR_CLR(w)   w &= ~( 0x1 << 4)

#define PMU_GLB_RST_FRC_SW_CLR_DSP_CLR_POS      3
#define PMU_GLB_RST_FRC_SW_CLR_DSP_CLR_LEN      1
#define PMU_GLB_RST_FRC_SW_CLR_DSP_CLR_MASK     0x00000008
#define PMU_GLB_RST_FRC_SW_CLR_DSP_CLR_VAL(w)   (((w) & 0x00000008) >> 3)
#define PMU_GLB_RST_FRC_SW_CLR_DSP_CLR_SET(w)   w |= ( 0x1 << 3)
#define PMU_GLB_RST_FRC_SW_CLR_DSP_CLR_CLR(w)   w &= ~( 0x1 << 3)

#define PMU_GLB_RST_FRC_SW_CLR_VDEC_CLR_POS      2
#define PMU_GLB_RST_FRC_SW_CLR_VDEC_CLR_LEN      1
#define PMU_GLB_RST_FRC_SW_CLR_VDEC_CLR_MASK     0x00000004
#define PMU_GLB_RST_FRC_SW_CLR_VDEC_CLR_VAL(w)   (((w) & 0x00000004) >> 2)
#define PMU_GLB_RST_FRC_SW_CLR_VDEC_CLR_SET(w)   w |= ( 0x1 << 2)
#define PMU_GLB_RST_FRC_SW_CLR_VDEC_CLR_CLR(w)   w &= ~( 0x1 << 2)

#define PMU_GLB_RST_FRC_SW_CLR_VENC1_CLR_POS      1
#define PMU_GLB_RST_FRC_SW_CLR_VENC1_CLR_LEN      1
#define PMU_GLB_RST_FRC_SW_CLR_VENC1_CLR_MASK     0x00000002
#define PMU_GLB_RST_FRC_SW_CLR_VENC1_CLR_VAL(w)   (((w) & 0x00000002) >> 1)
#define PMU_GLB_RST_FRC_SW_CLR_VENC1_CLR_SET(w)   w |= ( 0x1 << 1)
#define PMU_GLB_RST_FRC_SW_CLR_VENC1_CLR_CLR(w)   w &= ~( 0x1 << 1)

#define PMU_GLB_RST_FRC_SW_CLR_VENC0_CLR_POS      0
#define PMU_GLB_RST_FRC_SW_CLR_VENC0_CLR_LEN      1
#define PMU_GLB_RST_FRC_SW_CLR_VENC0_CLR_MASK     0x00000001
#define PMU_GLB_RST_FRC_SW_CLR_VENC0_CLR_VAL(w)   (((w) & 0x00000001) >> 0)
#define PMU_GLB_RST_FRC_SW_CLR_VENC0_CLR_SET(w)   w |= ( 0x1 << 0)
#define PMU_GLB_RST_FRC_SW_CLR_VENC0_CLR_CLR(w)   w &= ~( 0x1 << 0)

// REGISTER : RST_FRC_SW1_SET

#define PMU_GLB_RST_FRC_SW1_SET_ADDR      PMU_GLB_BASE_ADDR + 0x214
#define PMU_GLB_RST_FRC_SW1_SET_RSTVAL    0x0
#define PMU_GLB_RST_FRC_SW1_SET_ALL1      0x0
#define PMU_GLB_RST_FRC_SW1_SET_ALL0      0x0
#define PMU_GLB_RST_FRC_SW1_SET_ALL5A     0x0
#define PMU_GLB_RST_FRC_SW1_SET_ALLA5     0x0

// FIELD IN REGISTER : RST_FRC_SW1_SET

#define PMU_GLB_RST_FRC_SW1_SET_SEN_SET_POS      18
#define PMU_GLB_RST_FRC_SW1_SET_SEN_SET_LEN      1
#define PMU_GLB_RST_FRC_SW1_SET_SEN_SET_MASK     0x00040000
#define PMU_GLB_RST_FRC_SW1_SET_SEN_SET_VAL(w)   (((w) & 0x00040000) >> 18)
#define PMU_GLB_RST_FRC_SW1_SET_SEN_SET_SET(w)   w |= ( 0x1 << 18)
#define PMU_GLB_RST_FRC_SW1_SET_SEN_SET_CLR(w)   w &= ~( 0x1 << 18)

#define PMU_GLB_RST_FRC_SW1_SET_ISP_SET_POS      17
#define PMU_GLB_RST_FRC_SW1_SET_ISP_SET_LEN      1
#define PMU_GLB_RST_FRC_SW1_SET_ISP_SET_MASK     0x00020000
#define PMU_GLB_RST_FRC_SW1_SET_ISP_SET_VAL(w)   (((w) & 0x00020000) >> 17)
#define PMU_GLB_RST_FRC_SW1_SET_ISP_SET_SET(w)   w |= ( 0x1 << 17)
#define PMU_GLB_RST_FRC_SW1_SET_ISP_SET_CLR(w)   w &= ~( 0x1 << 17)

#define PMU_GLB_RST_FRC_SW1_SET_CPU_SET_POS      16
#define PMU_GLB_RST_FRC_SW1_SET_CPU_SET_LEN      1
#define PMU_GLB_RST_FRC_SW1_SET_CPU_SET_MASK     0x00010000
#define PMU_GLB_RST_FRC_SW1_SET_CPU_SET_VAL(w)   (((w) & 0x00010000) >> 16)
#define PMU_GLB_RST_FRC_SW1_SET_CPU_SET_SET(w)   w |= ( 0x1 << 16)
#define PMU_GLB_RST_FRC_SW1_SET_CPU_SET_CLR(w)   w &= ~( 0x1 << 16)

#define PMU_GLB_RST_FRC_SW1_SET_NPU_SET_POS      15
#define PMU_GLB_RST_FRC_SW1_SET_NPU_SET_LEN      1
#define PMU_GLB_RST_FRC_SW1_SET_NPU_SET_MASK     0x00008000
#define PMU_GLB_RST_FRC_SW1_SET_NPU_SET_VAL(w)   (((w) & 0x00008000) >> 15)
#define PMU_GLB_RST_FRC_SW1_SET_NPU_SET_SET(w)   w |= ( 0x1 << 15)
#define PMU_GLB_RST_FRC_SW1_SET_NPU_SET_CLR(w)   w &= ~( 0x1 << 15)

#define PMU_GLB_RST_FRC_SW1_SET_N8_SET_POS      14
#define PMU_GLB_RST_FRC_SW1_SET_N8_SET_LEN      1
#define PMU_GLB_RST_FRC_SW1_SET_N8_SET_MASK     0x00004000
#define PMU_GLB_RST_FRC_SW1_SET_N8_SET_VAL(w)   (((w) & 0x00004000) >> 14)
#define PMU_GLB_RST_FRC_SW1_SET_N8_SET_SET(w)   w |= ( 0x1 << 14)
#define PMU_GLB_RST_FRC_SW1_SET_N8_SET_CLR(w)   w &= ~( 0x1 << 14)

#define PMU_GLB_RST_FRC_SW1_SET_N7_SET_POS      13
#define PMU_GLB_RST_FRC_SW1_SET_N7_SET_LEN      1
#define PMU_GLB_RST_FRC_SW1_SET_N7_SET_MASK     0x00002000
#define PMU_GLB_RST_FRC_SW1_SET_N7_SET_VAL(w)   (((w) & 0x00002000) >> 13)
#define PMU_GLB_RST_FRC_SW1_SET_N7_SET_SET(w)   w |= ( 0x1 << 13)
#define PMU_GLB_RST_FRC_SW1_SET_N7_SET_CLR(w)   w &= ~( 0x1 << 13)

#define PMU_GLB_RST_FRC_SW1_SET_N6_SET_POS      12
#define PMU_GLB_RST_FRC_SW1_SET_N6_SET_LEN      1
#define PMU_GLB_RST_FRC_SW1_SET_N6_SET_MASK     0x00001000
#define PMU_GLB_RST_FRC_SW1_SET_N6_SET_VAL(w)   (((w) & 0x00001000) >> 12)
#define PMU_GLB_RST_FRC_SW1_SET_N6_SET_SET(w)   w |= ( 0x1 << 12)
#define PMU_GLB_RST_FRC_SW1_SET_N6_SET_CLR(w)   w &= ~( 0x1 << 12)

#define PMU_GLB_RST_FRC_SW1_SET_N5_SET_POS      11
#define PMU_GLB_RST_FRC_SW1_SET_N5_SET_LEN      1
#define PMU_GLB_RST_FRC_SW1_SET_N5_SET_MASK     0x00000800
#define PMU_GLB_RST_FRC_SW1_SET_N5_SET_VAL(w)   (((w) & 0x00000800) >> 11)
#define PMU_GLB_RST_FRC_SW1_SET_N5_SET_SET(w)   w |= ( 0x1 << 11)
#define PMU_GLB_RST_FRC_SW1_SET_N5_SET_CLR(w)   w &= ~( 0x1 << 11)

#define PMU_GLB_RST_FRC_SW1_SET_N4_SET_POS      10
#define PMU_GLB_RST_FRC_SW1_SET_N4_SET_LEN      1
#define PMU_GLB_RST_FRC_SW1_SET_N4_SET_MASK     0x00000400
#define PMU_GLB_RST_FRC_SW1_SET_N4_SET_VAL(w)   (((w) & 0x00000400) >> 10)
#define PMU_GLB_RST_FRC_SW1_SET_N4_SET_SET(w)   w |= ( 0x1 << 10)
#define PMU_GLB_RST_FRC_SW1_SET_N4_SET_CLR(w)   w &= ~( 0x1 << 10)

#define PMU_GLB_RST_FRC_SW1_SET_N3_SET_POS      9
#define PMU_GLB_RST_FRC_SW1_SET_N3_SET_LEN      1
#define PMU_GLB_RST_FRC_SW1_SET_N3_SET_MASK     0x00000200
#define PMU_GLB_RST_FRC_SW1_SET_N3_SET_VAL(w)   (((w) & 0x00000200) >> 9)
#define PMU_GLB_RST_FRC_SW1_SET_N3_SET_SET(w)   w |= ( 0x1 << 9)
#define PMU_GLB_RST_FRC_SW1_SET_N3_SET_CLR(w)   w &= ~( 0x1 << 9)

#define PMU_GLB_RST_FRC_SW1_SET_N2_SET_POS      8
#define PMU_GLB_RST_FRC_SW1_SET_N2_SET_LEN      1
#define PMU_GLB_RST_FRC_SW1_SET_N2_SET_MASK     0x00000100
#define PMU_GLB_RST_FRC_SW1_SET_N2_SET_VAL(w)   (((w) & 0x00000100) >> 8)
#define PMU_GLB_RST_FRC_SW1_SET_N2_SET_SET(w)   w |= ( 0x1 << 8)
#define PMU_GLB_RST_FRC_SW1_SET_N2_SET_CLR(w)   w &= ~( 0x1 << 8)

#define PMU_GLB_RST_FRC_SW1_SET_N1_SET_POS      7
#define PMU_GLB_RST_FRC_SW1_SET_N1_SET_LEN      1
#define PMU_GLB_RST_FRC_SW1_SET_N1_SET_MASK     0x00000080
#define PMU_GLB_RST_FRC_SW1_SET_N1_SET_VAL(w)   (((w) & 0x00000080) >> 7)
#define PMU_GLB_RST_FRC_SW1_SET_N1_SET_SET(w)   w |= ( 0x1 << 7)
#define PMU_GLB_RST_FRC_SW1_SET_N1_SET_CLR(w)   w &= ~( 0x1 << 7)

#define PMU_GLB_RST_FRC_SW1_SET_N0_SET_POS      6
#define PMU_GLB_RST_FRC_SW1_SET_N0_SET_LEN      1
#define PMU_GLB_RST_FRC_SW1_SET_N0_SET_MASK     0x00000040
#define PMU_GLB_RST_FRC_SW1_SET_N0_SET_VAL(w)   (((w) & 0x00000040) >> 6)
#define PMU_GLB_RST_FRC_SW1_SET_N0_SET_SET(w)   w |= ( 0x1 << 6)
#define PMU_GLB_RST_FRC_SW1_SET_N0_SET_CLR(w)   w &= ~( 0x1 << 6)

#define PMU_GLB_RST_FRC_SW1_SET_MM_SET_POS      5
#define PMU_GLB_RST_FRC_SW1_SET_MM_SET_LEN      1
#define PMU_GLB_RST_FRC_SW1_SET_MM_SET_MASK     0x00000020
#define PMU_GLB_RST_FRC_SW1_SET_MM_SET_VAL(w)   (((w) & 0x00000020) >> 5)
#define PMU_GLB_RST_FRC_SW1_SET_MM_SET_SET(w)   w |= ( 0x1 << 5)
#define PMU_GLB_RST_FRC_SW1_SET_MM_SET_CLR(w)   w &= ~( 0x1 << 5)

#define PMU_GLB_RST_FRC_SW1_SET_PERIP_SET_POS      4
#define PMU_GLB_RST_FRC_SW1_SET_PERIP_SET_LEN      1
#define PMU_GLB_RST_FRC_SW1_SET_PERIP_SET_MASK     0x00000010
#define PMU_GLB_RST_FRC_SW1_SET_PERIP_SET_VAL(w)   (((w) & 0x00000010) >> 4)
#define PMU_GLB_RST_FRC_SW1_SET_PERIP_SET_SET(w)   w |= ( 0x1 << 4)
#define PMU_GLB_RST_FRC_SW1_SET_PERIP_SET_CLR(w)   w &= ~( 0x1 << 4)

#define PMU_GLB_RST_FRC_SW1_SET_PIPE_SET_POS      3
#define PMU_GLB_RST_FRC_SW1_SET_PIPE_SET_LEN      1
#define PMU_GLB_RST_FRC_SW1_SET_PIPE_SET_MASK     0x00000008
#define PMU_GLB_RST_FRC_SW1_SET_PIPE_SET_VAL(w)   (((w) & 0x00000008) >> 3)
#define PMU_GLB_RST_FRC_SW1_SET_PIPE_SET_SET(w)   w |= ( 0x1 << 3)
#define PMU_GLB_RST_FRC_SW1_SET_PIPE_SET_CLR(w)   w &= ~( 0x1 << 3)

#define PMU_GLB_RST_FRC_SW1_SET_FLASH_SET_POS      2
#define PMU_GLB_RST_FRC_SW1_SET_FLASH_SET_LEN      1
#define PMU_GLB_RST_FRC_SW1_SET_FLASH_SET_MASK     0x00000004
#define PMU_GLB_RST_FRC_SW1_SET_FLASH_SET_VAL(w)   (((w) & 0x00000004) >> 2)
#define PMU_GLB_RST_FRC_SW1_SET_FLASH_SET_SET(w)   w |= ( 0x1 << 2)
#define PMU_GLB_RST_FRC_SW1_SET_FLASH_SET_CLR(w)   w &= ~( 0x1 << 2)

#define PMU_GLB_RST_FRC_SW1_SET_DDR1_SET_POS      1
#define PMU_GLB_RST_FRC_SW1_SET_DDR1_SET_LEN      1
#define PMU_GLB_RST_FRC_SW1_SET_DDR1_SET_MASK     0x00000002
#define PMU_GLB_RST_FRC_SW1_SET_DDR1_SET_VAL(w)   (((w) & 0x00000002) >> 1)
#define PMU_GLB_RST_FRC_SW1_SET_DDR1_SET_SET(w)   w |= ( 0x1 << 1)
#define PMU_GLB_RST_FRC_SW1_SET_DDR1_SET_CLR(w)   w &= ~( 0x1 << 1)

#define PMU_GLB_RST_FRC_SW1_SET_DDR0_SET_POS      0
#define PMU_GLB_RST_FRC_SW1_SET_DDR0_SET_LEN      1
#define PMU_GLB_RST_FRC_SW1_SET_DDR0_SET_MASK     0x00000001
#define PMU_GLB_RST_FRC_SW1_SET_DDR0_SET_VAL(w)   (((w) & 0x00000001) >> 0)
#define PMU_GLB_RST_FRC_SW1_SET_DDR0_SET_SET(w)   w |= ( 0x1 << 0)
#define PMU_GLB_RST_FRC_SW1_SET_DDR0_SET_CLR(w)   w &= ~( 0x1 << 0)

// REGISTER : RST_FRC_SW1_CLR

#define PMU_GLB_RST_FRC_SW1_CLR_ADDR      PMU_GLB_BASE_ADDR + 0x218
#define PMU_GLB_RST_FRC_SW1_CLR_RSTVAL    0x0
#define PMU_GLB_RST_FRC_SW1_CLR_ALL1      0x0
#define PMU_GLB_RST_FRC_SW1_CLR_ALL0      0x0
#define PMU_GLB_RST_FRC_SW1_CLR_ALL5A     0x0
#define PMU_GLB_RST_FRC_SW1_CLR_ALLA5     0x0

// FIELD IN REGISTER : RST_FRC_SW1_CLR

#define PMU_GLB_RST_FRC_SW1_CLR_SEN_CLR_POS      18
#define PMU_GLB_RST_FRC_SW1_CLR_SEN_CLR_LEN      1
#define PMU_GLB_RST_FRC_SW1_CLR_SEN_CLR_MASK     0x00040000
#define PMU_GLB_RST_FRC_SW1_CLR_SEN_CLR_VAL(w)   (((w) & 0x00040000) >> 18)
#define PMU_GLB_RST_FRC_SW1_CLR_SEN_CLR_SET(w)   w |= ( 0x1 << 18)
#define PMU_GLB_RST_FRC_SW1_CLR_SEN_CLR_CLR(w)   w &= ~( 0x1 << 18)

#define PMU_GLB_RST_FRC_SW1_CLR_ISP_CLR_POS      17
#define PMU_GLB_RST_FRC_SW1_CLR_ISP_CLR_LEN      1
#define PMU_GLB_RST_FRC_SW1_CLR_ISP_CLR_MASK     0x00020000
#define PMU_GLB_RST_FRC_SW1_CLR_ISP_CLR_VAL(w)   (((w) & 0x00020000) >> 17)
#define PMU_GLB_RST_FRC_SW1_CLR_ISP_CLR_SET(w)   w |= ( 0x1 << 17)
#define PMU_GLB_RST_FRC_SW1_CLR_ISP_CLR_CLR(w)   w &= ~( 0x1 << 17)

#define PMU_GLB_RST_FRC_SW1_CLR_CPU_CLR_POS      16
#define PMU_GLB_RST_FRC_SW1_CLR_CPU_CLR_LEN      1
#define PMU_GLB_RST_FRC_SW1_CLR_CPU_CLR_MASK     0x00010000
#define PMU_GLB_RST_FRC_SW1_CLR_CPU_CLR_VAL(w)   (((w) & 0x00010000) >> 16)
#define PMU_GLB_RST_FRC_SW1_CLR_CPU_CLR_SET(w)   w |= ( 0x1 << 16)
#define PMU_GLB_RST_FRC_SW1_CLR_CPU_CLR_CLR(w)   w &= ~( 0x1 << 16)

#define PMU_GLB_RST_FRC_SW1_CLR_NPU_CLR_POS      15
#define PMU_GLB_RST_FRC_SW1_CLR_NPU_CLR_LEN      1
#define PMU_GLB_RST_FRC_SW1_CLR_NPU_CLR_MASK     0x00008000
#define PMU_GLB_RST_FRC_SW1_CLR_NPU_CLR_VAL(w)   (((w) & 0x00008000) >> 15)
#define PMU_GLB_RST_FRC_SW1_CLR_NPU_CLR_SET(w)   w |= ( 0x1 << 15)
#define PMU_GLB_RST_FRC_SW1_CLR_NPU_CLR_CLR(w)   w &= ~( 0x1 << 15)

#define PMU_GLB_RST_FRC_SW1_CLR_N8_CLR_POS      14
#define PMU_GLB_RST_FRC_SW1_CLR_N8_CLR_LEN      1
#define PMU_GLB_RST_FRC_SW1_CLR_N8_CLR_MASK     0x00004000
#define PMU_GLB_RST_FRC_SW1_CLR_N8_CLR_VAL(w)   (((w) & 0x00004000) >> 14)
#define PMU_GLB_RST_FRC_SW1_CLR_N8_CLR_SET(w)   w |= ( 0x1 << 14)
#define PMU_GLB_RST_FRC_SW1_CLR_N8_CLR_CLR(w)   w &= ~( 0x1 << 14)

#define PMU_GLB_RST_FRC_SW1_CLR_N7_CLR_POS      13
#define PMU_GLB_RST_FRC_SW1_CLR_N7_CLR_LEN      1
#define PMU_GLB_RST_FRC_SW1_CLR_N7_CLR_MASK     0x00002000
#define PMU_GLB_RST_FRC_SW1_CLR_N7_CLR_VAL(w)   (((w) & 0x00002000) >> 13)
#define PMU_GLB_RST_FRC_SW1_CLR_N7_CLR_SET(w)   w |= ( 0x1 << 13)
#define PMU_GLB_RST_FRC_SW1_CLR_N7_CLR_CLR(w)   w &= ~( 0x1 << 13)

#define PMU_GLB_RST_FRC_SW1_CLR_N6_CLR_POS      12
#define PMU_GLB_RST_FRC_SW1_CLR_N6_CLR_LEN      1
#define PMU_GLB_RST_FRC_SW1_CLR_N6_CLR_MASK     0x00001000
#define PMU_GLB_RST_FRC_SW1_CLR_N6_CLR_VAL(w)   (((w) & 0x00001000) >> 12)
#define PMU_GLB_RST_FRC_SW1_CLR_N6_CLR_SET(w)   w |= ( 0x1 << 12)
#define PMU_GLB_RST_FRC_SW1_CLR_N6_CLR_CLR(w)   w &= ~( 0x1 << 12)

#define PMU_GLB_RST_FRC_SW1_CLR_N5_CLR_POS      11
#define PMU_GLB_RST_FRC_SW1_CLR_N5_CLR_LEN      1
#define PMU_GLB_RST_FRC_SW1_CLR_N5_CLR_MASK     0x00000800
#define PMU_GLB_RST_FRC_SW1_CLR_N5_CLR_VAL(w)   (((w) & 0x00000800) >> 11)
#define PMU_GLB_RST_FRC_SW1_CLR_N5_CLR_SET(w)   w |= ( 0x1 << 11)
#define PMU_GLB_RST_FRC_SW1_CLR_N5_CLR_CLR(w)   w &= ~( 0x1 << 11)

#define PMU_GLB_RST_FRC_SW1_CLR_N4_CLR_POS      10
#define PMU_GLB_RST_FRC_SW1_CLR_N4_CLR_LEN      1
#define PMU_GLB_RST_FRC_SW1_CLR_N4_CLR_MASK     0x00000400
#define PMU_GLB_RST_FRC_SW1_CLR_N4_CLR_VAL(w)   (((w) & 0x00000400) >> 10)
#define PMU_GLB_RST_FRC_SW1_CLR_N4_CLR_SET(w)   w |= ( 0x1 << 10)
#define PMU_GLB_RST_FRC_SW1_CLR_N4_CLR_CLR(w)   w &= ~( 0x1 << 10)

#define PMU_GLB_RST_FRC_SW1_CLR_N3_CLR_POS      9
#define PMU_GLB_RST_FRC_SW1_CLR_N3_CLR_LEN      1
#define PMU_GLB_RST_FRC_SW1_CLR_N3_CLR_MASK     0x00000200
#define PMU_GLB_RST_FRC_SW1_CLR_N3_CLR_VAL(w)   (((w) & 0x00000200) >> 9)
#define PMU_GLB_RST_FRC_SW1_CLR_N3_CLR_SET(w)   w |= ( 0x1 << 9)
#define PMU_GLB_RST_FRC_SW1_CLR_N3_CLR_CLR(w)   w &= ~( 0x1 << 9)

#define PMU_GLB_RST_FRC_SW1_CLR_N2_CLR_POS      8
#define PMU_GLB_RST_FRC_SW1_CLR_N2_CLR_LEN      1
#define PMU_GLB_RST_FRC_SW1_CLR_N2_CLR_MASK     0x00000100
#define PMU_GLB_RST_FRC_SW1_CLR_N2_CLR_VAL(w)   (((w) & 0x00000100) >> 8)
#define PMU_GLB_RST_FRC_SW1_CLR_N2_CLR_SET(w)   w |= ( 0x1 << 8)
#define PMU_GLB_RST_FRC_SW1_CLR_N2_CLR_CLR(w)   w &= ~( 0x1 << 8)

#define PMU_GLB_RST_FRC_SW1_CLR_N1_CLR_POS      7
#define PMU_GLB_RST_FRC_SW1_CLR_N1_CLR_LEN      1
#define PMU_GLB_RST_FRC_SW1_CLR_N1_CLR_MASK     0x00000080
#define PMU_GLB_RST_FRC_SW1_CLR_N1_CLR_VAL(w)   (((w) & 0x00000080) >> 7)
#define PMU_GLB_RST_FRC_SW1_CLR_N1_CLR_SET(w)   w |= ( 0x1 << 7)
#define PMU_GLB_RST_FRC_SW1_CLR_N1_CLR_CLR(w)   w &= ~( 0x1 << 7)

#define PMU_GLB_RST_FRC_SW1_CLR_N0_CLR_POS      6
#define PMU_GLB_RST_FRC_SW1_CLR_N0_CLR_LEN      1
#define PMU_GLB_RST_FRC_SW1_CLR_N0_CLR_MASK     0x00000040
#define PMU_GLB_RST_FRC_SW1_CLR_N0_CLR_VAL(w)   (((w) & 0x00000040) >> 6)
#define PMU_GLB_RST_FRC_SW1_CLR_N0_CLR_SET(w)   w |= ( 0x1 << 6)
#define PMU_GLB_RST_FRC_SW1_CLR_N0_CLR_CLR(w)   w &= ~( 0x1 << 6)

#define PMU_GLB_RST_FRC_SW1_CLR_MM_CLR_POS      5
#define PMU_GLB_RST_FRC_SW1_CLR_MM_CLR_LEN      1
#define PMU_GLB_RST_FRC_SW1_CLR_MM_CLR_MASK     0x00000020
#define PMU_GLB_RST_FRC_SW1_CLR_MM_CLR_VAL(w)   (((w) & 0x00000020) >> 5)
#define PMU_GLB_RST_FRC_SW1_CLR_MM_CLR_SET(w)   w |= ( 0x1 << 5)
#define PMU_GLB_RST_FRC_SW1_CLR_MM_CLR_CLR(w)   w &= ~( 0x1 << 5)

#define PMU_GLB_RST_FRC_SW1_CLR_PERIP_CLR_POS      4
#define PMU_GLB_RST_FRC_SW1_CLR_PERIP_CLR_LEN      1
#define PMU_GLB_RST_FRC_SW1_CLR_PERIP_CLR_MASK     0x00000010
#define PMU_GLB_RST_FRC_SW1_CLR_PERIP_CLR_VAL(w)   (((w) & 0x00000010) >> 4)
#define PMU_GLB_RST_FRC_SW1_CLR_PERIP_CLR_SET(w)   w |= ( 0x1 << 4)
#define PMU_GLB_RST_FRC_SW1_CLR_PERIP_CLR_CLR(w)   w &= ~( 0x1 << 4)

#define PMU_GLB_RST_FRC_SW1_CLR_PIPE_CLR_POS      3
#define PMU_GLB_RST_FRC_SW1_CLR_PIPE_CLR_LEN      1
#define PMU_GLB_RST_FRC_SW1_CLR_PIPE_CLR_MASK     0x00000008
#define PMU_GLB_RST_FRC_SW1_CLR_PIPE_CLR_VAL(w)   (((w) & 0x00000008) >> 3)
#define PMU_GLB_RST_FRC_SW1_CLR_PIPE_CLR_SET(w)   w |= ( 0x1 << 3)
#define PMU_GLB_RST_FRC_SW1_CLR_PIPE_CLR_CLR(w)   w &= ~( 0x1 << 3)

#define PMU_GLB_RST_FRC_SW1_CLR_FLASH_CLR_POS      2
#define PMU_GLB_RST_FRC_SW1_CLR_FLASH_CLR_LEN      1
#define PMU_GLB_RST_FRC_SW1_CLR_FLASH_CLR_MASK     0x00000004
#define PMU_GLB_RST_FRC_SW1_CLR_FLASH_CLR_VAL(w)   (((w) & 0x00000004) >> 2)
#define PMU_GLB_RST_FRC_SW1_CLR_FLASH_CLR_SET(w)   w |= ( 0x1 << 2)
#define PMU_GLB_RST_FRC_SW1_CLR_FLASH_CLR_CLR(w)   w &= ~( 0x1 << 2)

#define PMU_GLB_RST_FRC_SW1_CLR_DDR1_CLR_POS      1
#define PMU_GLB_RST_FRC_SW1_CLR_DDR1_CLR_LEN      1
#define PMU_GLB_RST_FRC_SW1_CLR_DDR1_CLR_MASK     0x00000002
#define PMU_GLB_RST_FRC_SW1_CLR_DDR1_CLR_VAL(w)   (((w) & 0x00000002) >> 1)
#define PMU_GLB_RST_FRC_SW1_CLR_DDR1_CLR_SET(w)   w |= ( 0x1 << 1)
#define PMU_GLB_RST_FRC_SW1_CLR_DDR1_CLR_CLR(w)   w &= ~( 0x1 << 1)

#define PMU_GLB_RST_FRC_SW1_CLR_DDR0_CLR_POS      0
#define PMU_GLB_RST_FRC_SW1_CLR_DDR0_CLR_LEN      1
#define PMU_GLB_RST_FRC_SW1_CLR_DDR0_CLR_MASK     0x00000001
#define PMU_GLB_RST_FRC_SW1_CLR_DDR0_CLR_VAL(w)   (((w) & 0x00000001) >> 0)
#define PMU_GLB_RST_FRC_SW1_CLR_DDR0_CLR_SET(w)   w |= ( 0x1 << 0)
#define PMU_GLB_RST_FRC_SW1_CLR_DDR0_CLR_CLR(w)   w &= ~( 0x1 << 0)

// REGISTER : LIGHT_SLEEP_FRC_SET

#define PMU_GLB_LIGHT_SLEEP_FRC_SET_ADDR      PMU_GLB_BASE_ADDR + 0x21C
#define PMU_GLB_LIGHT_SLEEP_FRC_SET_RSTVAL    0x0
#define PMU_GLB_LIGHT_SLEEP_FRC_SET_ALL1      0x0
#define PMU_GLB_LIGHT_SLEEP_FRC_SET_ALL0      0x0
#define PMU_GLB_LIGHT_SLEEP_FRC_SET_ALL5A     0x0
#define PMU_GLB_LIGHT_SLEEP_FRC_SET_ALLA5     0x0

// FIELD IN REGISTER : LIGHT_SLEEP_FRC_SET

#define PMU_GLB_LIGHT_SLEEP_FRC_SET_EN_SET_POS      0
#define PMU_GLB_LIGHT_SLEEP_FRC_SET_EN_SET_LEN      1
#define PMU_GLB_LIGHT_SLEEP_FRC_SET_EN_SET_MASK     0x00000001
#define PMU_GLB_LIGHT_SLEEP_FRC_SET_EN_SET_VAL(w)   (((w) & 0x00000001) >> 0)
#define PMU_GLB_LIGHT_SLEEP_FRC_SET_EN_SET_SET(w)   w |= ( 0x1 << 0)
#define PMU_GLB_LIGHT_SLEEP_FRC_SET_EN_SET_CLR(w)   w &= ~( 0x1 << 0)

// REGISTER : LIGHT_SLEEP_FRC_CLR

#define PMU_GLB_LIGHT_SLEEP_FRC_CLR_ADDR      PMU_GLB_BASE_ADDR + 0x220
#define PMU_GLB_LIGHT_SLEEP_FRC_CLR_RSTVAL    0x0
#define PMU_GLB_LIGHT_SLEEP_FRC_CLR_ALL1      0x0
#define PMU_GLB_LIGHT_SLEEP_FRC_CLR_ALL0      0x0
#define PMU_GLB_LIGHT_SLEEP_FRC_CLR_ALL5A     0x0
#define PMU_GLB_LIGHT_SLEEP_FRC_CLR_ALLA5     0x0

// FIELD IN REGISTER : LIGHT_SLEEP_FRC_CLR

#define PMU_GLB_LIGHT_SLEEP_FRC_CLR_EN_CLR_POS      0
#define PMU_GLB_LIGHT_SLEEP_FRC_CLR_EN_CLR_LEN      1
#define PMU_GLB_LIGHT_SLEEP_FRC_CLR_EN_CLR_MASK     0x00000001
#define PMU_GLB_LIGHT_SLEEP_FRC_CLR_EN_CLR_VAL(w)   (((w) & 0x00000001) >> 0)
#define PMU_GLB_LIGHT_SLEEP_FRC_CLR_EN_CLR_SET(w)   w |= ( 0x1 << 0)
#define PMU_GLB_LIGHT_SLEEP_FRC_CLR_EN_CLR_CLR(w)   w &= ~( 0x1 << 0)

// REGISTER : LIGHT_SLEEP_SW_SET

#define PMU_GLB_LIGHT_SLEEP_SW_SET_ADDR      PMU_GLB_BASE_ADDR + 0x224
#define PMU_GLB_LIGHT_SLEEP_SW_SET_RSTVAL    0x0
#define PMU_GLB_LIGHT_SLEEP_SW_SET_ALL1      0x0
#define PMU_GLB_LIGHT_SLEEP_SW_SET_ALL0      0x0
#define PMU_GLB_LIGHT_SLEEP_SW_SET_ALL5A     0x0
#define PMU_GLB_LIGHT_SLEEP_SW_SET_ALLA5     0x0

// FIELD IN REGISTER : LIGHT_SLEEP_SW_SET

#define PMU_GLB_LIGHT_SLEEP_SW_SET_EN_SET_POS      0
#define PMU_GLB_LIGHT_SLEEP_SW_SET_EN_SET_LEN      1
#define PMU_GLB_LIGHT_SLEEP_SW_SET_EN_SET_MASK     0x00000001
#define PMU_GLB_LIGHT_SLEEP_SW_SET_EN_SET_VAL(w)   (((w) & 0x00000001) >> 0)
#define PMU_GLB_LIGHT_SLEEP_SW_SET_EN_SET_SET(w)   w |= ( 0x1 << 0)
#define PMU_GLB_LIGHT_SLEEP_SW_SET_EN_SET_CLR(w)   w &= ~( 0x1 << 0)

// REGISTER : LIGHT_SLEEP_SW_CLR

#define PMU_GLB_LIGHT_SLEEP_SW_CLR_ADDR      PMU_GLB_BASE_ADDR + 0x228
#define PMU_GLB_LIGHT_SLEEP_SW_CLR_RSTVAL    0x0
#define PMU_GLB_LIGHT_SLEEP_SW_CLR_ALL1      0x0
#define PMU_GLB_LIGHT_SLEEP_SW_CLR_ALL0      0x0
#define PMU_GLB_LIGHT_SLEEP_SW_CLR_ALL5A     0x0
#define PMU_GLB_LIGHT_SLEEP_SW_CLR_ALLA5     0x0

// FIELD IN REGISTER : LIGHT_SLEEP_SW_CLR

#define PMU_GLB_LIGHT_SLEEP_SW_CLR_EN_CLR_POS      0
#define PMU_GLB_LIGHT_SLEEP_SW_CLR_EN_CLR_LEN      1
#define PMU_GLB_LIGHT_SLEEP_SW_CLR_EN_CLR_MASK     0x00000001
#define PMU_GLB_LIGHT_SLEEP_SW_CLR_EN_CLR_VAL(w)   (((w) & 0x00000001) >> 0)
#define PMU_GLB_LIGHT_SLEEP_SW_CLR_EN_CLR_SET(w)   w |= ( 0x1 << 0)
#define PMU_GLB_LIGHT_SLEEP_SW_CLR_EN_CLR_CLR(w)   w &= ~( 0x1 << 0)

// REGISTER : MEM_REPAIRE_SET

#define PMU_GLB_MEM_REPAIRE_SET_ADDR      PMU_GLB_BASE_ADDR + 0x22C
#define PMU_GLB_MEM_REPAIRE_SET_RSTVAL    0x0
#define PMU_GLB_MEM_REPAIRE_SET_ALL1      0x0
#define PMU_GLB_MEM_REPAIRE_SET_ALL0      0x0
#define PMU_GLB_MEM_REPAIRE_SET_ALL5A     0x0
#define PMU_GLB_MEM_REPAIRE_SET_ALLA5     0x0

// FIELD IN REGISTER : MEM_REPAIRE_SET

#define PMU_GLB_MEM_REPAIRE_SET_BIHR_SET_POS      1
#define PMU_GLB_MEM_REPAIRE_SET_BIHR_SET_LEN      1
#define PMU_GLB_MEM_REPAIRE_SET_BIHR_SET_MASK     0x00000002
#define PMU_GLB_MEM_REPAIRE_SET_BIHR_SET_VAL(w)   (((w) & 0x00000002) >> 1)
#define PMU_GLB_MEM_REPAIRE_SET_BIHR_SET_SET(w)   w |= ( 0x1 << 1)
#define PMU_GLB_MEM_REPAIRE_SET_BIHR_SET_CLR(w)   w &= ~( 0x1 << 1)

#define PMU_GLB_MEM_REPAIRE_SET_BISR_SET_POS      0
#define PMU_GLB_MEM_REPAIRE_SET_BISR_SET_LEN      1
#define PMU_GLB_MEM_REPAIRE_SET_BISR_SET_MASK     0x00000001
#define PMU_GLB_MEM_REPAIRE_SET_BISR_SET_VAL(w)   (((w) & 0x00000001) >> 0)
#define PMU_GLB_MEM_REPAIRE_SET_BISR_SET_SET(w)   w |= ( 0x1 << 0)
#define PMU_GLB_MEM_REPAIRE_SET_BISR_SET_CLR(w)   w &= ~( 0x1 << 0)

// REGISTER : MEM_REPAIRE_CLR

#define PMU_GLB_MEM_REPAIRE_CLR_ADDR      PMU_GLB_BASE_ADDR + 0x230
#define PMU_GLB_MEM_REPAIRE_CLR_RSTVAL    0x0
#define PMU_GLB_MEM_REPAIRE_CLR_ALL1      0x0
#define PMU_GLB_MEM_REPAIRE_CLR_ALL0      0x0
#define PMU_GLB_MEM_REPAIRE_CLR_ALL5A     0x0
#define PMU_GLB_MEM_REPAIRE_CLR_ALLA5     0x0

// FIELD IN REGISTER : MEM_REPAIRE_CLR

#define PMU_GLB_MEM_REPAIRE_CLR_BIHR_CLR_POS      1
#define PMU_GLB_MEM_REPAIRE_CLR_BIHR_CLR_LEN      1
#define PMU_GLB_MEM_REPAIRE_CLR_BIHR_CLR_MASK     0x00000002
#define PMU_GLB_MEM_REPAIRE_CLR_BIHR_CLR_VAL(w)   (((w) & 0x00000002) >> 1)
#define PMU_GLB_MEM_REPAIRE_CLR_BIHR_CLR_SET(w)   w |= ( 0x1 << 1)
#define PMU_GLB_MEM_REPAIRE_CLR_BIHR_CLR_CLR(w)   w &= ~( 0x1 << 1)

#define PMU_GLB_MEM_REPAIRE_CLR_BISR_CLR_POS      0
#define PMU_GLB_MEM_REPAIRE_CLR_BISR_CLR_LEN      1
#define PMU_GLB_MEM_REPAIRE_CLR_BISR_CLR_MASK     0x00000001
#define PMU_GLB_MEM_REPAIRE_CLR_BISR_CLR_VAL(w)   (((w) & 0x00000001) >> 0)
#define PMU_GLB_MEM_REPAIRE_CLR_BISR_CLR_SET(w)   w |= ( 0x1 << 0)
#define PMU_GLB_MEM_REPAIRE_CLR_BISR_CLR_CLR(w)   w &= ~( 0x1 << 0)

// REGISTER : INT_MASK_SET

#define PMU_GLB_INT_MASK_SET_ADDR      PMU_GLB_BASE_ADDR + 0x234
#define PMU_GLB_INT_MASK_SET_RSTVAL    0x0
#define PMU_GLB_INT_MASK_SET_ALL1      0x0
#define PMU_GLB_INT_MASK_SET_ALL0      0x0
#define PMU_GLB_INT_MASK_SET_ALL5A     0x0
#define PMU_GLB_INT_MASK_SET_ALLA5     0x0

// FIELD IN REGISTER : INT_MASK_SET

#define PMU_GLB_INT_MASK_SET_VAL_SET_POS      0
#define PMU_GLB_INT_MASK_SET_VAL_SET_LEN      32
#define PMU_GLB_INT_MASK_SET_VAL_SET_MASK     0xFFFFFFFF
#define PMU_GLB_INT_MASK_SET_VAL_SET_VAL(w)   (((w) & 0xFFFFFFFF) >> 0)
#define PMU_GLB_INT_MASK_SET_VAL_SET_SET(w)   w |= ( 0xFFFFFFFF << 0)
#define PMU_GLB_INT_MASK_SET_VAL_SET_CLR(w)   w &= ~( 0xFFFFFFFF << 0)

// REGISTER : INT_MASK_CLR

#define PMU_GLB_INT_MASK_CLR_ADDR      PMU_GLB_BASE_ADDR + 0x238
#define PMU_GLB_INT_MASK_CLR_RSTVAL    0x0
#define PMU_GLB_INT_MASK_CLR_ALL1      0x0
#define PMU_GLB_INT_MASK_CLR_ALL0      0x0
#define PMU_GLB_INT_MASK_CLR_ALL5A     0x0
#define PMU_GLB_INT_MASK_CLR_ALLA5     0x0

// FIELD IN REGISTER : INT_MASK_CLR

#define PMU_GLB_INT_MASK_CLR_VAL_CLR_POS      0
#define PMU_GLB_INT_MASK_CLR_VAL_CLR_LEN      32
#define PMU_GLB_INT_MASK_CLR_VAL_CLR_MASK     0xFFFFFFFF
#define PMU_GLB_INT_MASK_CLR_VAL_CLR_VAL(w)   (((w) & 0xFFFFFFFF) >> 0)
#define PMU_GLB_INT_MASK_CLR_VAL_CLR_SET(w)   w |= ( 0xFFFFFFFF << 0)
#define PMU_GLB_INT_MASK_CLR_VAL_CLR_CLR(w)   w &= ~( 0xFFFFFFFF << 0)

// REGISTER : INT_MASK1_SET

#define PMU_GLB_INT_MASK1_SET_ADDR      PMU_GLB_BASE_ADDR + 0x23C
#define PMU_GLB_INT_MASK1_SET_RSTVAL    0x0
#define PMU_GLB_INT_MASK1_SET_ALL1      0x0
#define PMU_GLB_INT_MASK1_SET_ALL0      0x0
#define PMU_GLB_INT_MASK1_SET_ALL5A     0x0
#define PMU_GLB_INT_MASK1_SET_ALLA5     0x0

// FIELD IN REGISTER : INT_MASK1_SET

#define PMU_GLB_INT_MASK1_SET_VAL_SET_POS      0
#define PMU_GLB_INT_MASK1_SET_VAL_SET_LEN      32
#define PMU_GLB_INT_MASK1_SET_VAL_SET_MASK     0xFFFFFFFF
#define PMU_GLB_INT_MASK1_SET_VAL_SET_VAL(w)   (((w) & 0xFFFFFFFF) >> 0)
#define PMU_GLB_INT_MASK1_SET_VAL_SET_SET(w)   w |= ( 0xFFFFFFFF << 0)
#define PMU_GLB_INT_MASK1_SET_VAL_SET_CLR(w)   w &= ~( 0xFFFFFFFF << 0)

// REGISTER : INT_MASK1_CLR

#define PMU_GLB_INT_MASK1_CLR_ADDR      PMU_GLB_BASE_ADDR + 0x240
#define PMU_GLB_INT_MASK1_CLR_RSTVAL    0x0
#define PMU_GLB_INT_MASK1_CLR_ALL1      0x0
#define PMU_GLB_INT_MASK1_CLR_ALL0      0x0
#define PMU_GLB_INT_MASK1_CLR_ALL5A     0x0
#define PMU_GLB_INT_MASK1_CLR_ALLA5     0x0

// FIELD IN REGISTER : INT_MASK1_CLR

#define PMU_GLB_INT_MASK1_CLR_VAL_CLR_POS      0
#define PMU_GLB_INT_MASK1_CLR_VAL_CLR_LEN      32
#define PMU_GLB_INT_MASK1_CLR_VAL_CLR_MASK     0xFFFFFFFF
#define PMU_GLB_INT_MASK1_CLR_VAL_CLR_VAL(w)   (((w) & 0xFFFFFFFF) >> 0)
#define PMU_GLB_INT_MASK1_CLR_VAL_CLR_SET(w)   w |= ( 0xFFFFFFFF << 0)
#define PMU_GLB_INT_MASK1_CLR_VAL_CLR_CLR(w)   w &= ~( 0xFFFFFFFF << 0)

// REGISTER : INT_MASK2_SET

#define PMU_GLB_INT_MASK2_SET_ADDR      PMU_GLB_BASE_ADDR + 0x244
#define PMU_GLB_INT_MASK2_SET_RSTVAL    0x0
#define PMU_GLB_INT_MASK2_SET_ALL1      0x0
#define PMU_GLB_INT_MASK2_SET_ALL0      0x0
#define PMU_GLB_INT_MASK2_SET_ALL5A     0x0
#define PMU_GLB_INT_MASK2_SET_ALLA5     0x0

// FIELD IN REGISTER : INT_MASK2_SET

#define PMU_GLB_INT_MASK2_SET_VAL_SET_POS      0
#define PMU_GLB_INT_MASK2_SET_VAL_SET_LEN      4
#define PMU_GLB_INT_MASK2_SET_VAL_SET_MASK     0x0000000F
#define PMU_GLB_INT_MASK2_SET_VAL_SET_VAL(w)   (((w) & 0x0000000F) >> 0)
#define PMU_GLB_INT_MASK2_SET_VAL_SET_SET(w)   w |= ( 0xF << 0)
#define PMU_GLB_INT_MASK2_SET_VAL_SET_CLR(w)   w &= ~( 0xF << 0)

// REGISTER : INT_MASK2_CLR

#define PMU_GLB_INT_MASK2_CLR_ADDR      PMU_GLB_BASE_ADDR + 0x248
#define PMU_GLB_INT_MASK2_CLR_RSTVAL    0x0
#define PMU_GLB_INT_MASK2_CLR_ALL1      0x0
#define PMU_GLB_INT_MASK2_CLR_ALL0      0x0
#define PMU_GLB_INT_MASK2_CLR_ALL5A     0x0
#define PMU_GLB_INT_MASK2_CLR_ALLA5     0x0

// FIELD IN REGISTER : INT_MASK2_CLR

#define PMU_GLB_INT_MASK2_CLR_VAL_CLR_POS      0
#define PMU_GLB_INT_MASK2_CLR_VAL_CLR_LEN      4
#define PMU_GLB_INT_MASK2_CLR_VAL_CLR_MASK     0x0000000F
#define PMU_GLB_INT_MASK2_CLR_VAL_CLR_VAL(w)   (((w) & 0x0000000F) >> 0)
#define PMU_GLB_INT_MASK2_CLR_VAL_CLR_SET(w)   w |= ( 0xF << 0)
#define PMU_GLB_INT_MASK2_CLR_VAL_CLR_CLR(w)   w &= ~( 0xF << 0)

// REGISTER : INT_CLR_SET

#define PMU_GLB_INT_CLR_SET_ADDR      PMU_GLB_BASE_ADDR + 0x24C
#define PMU_GLB_INT_CLR_SET_RSTVAL    0x0
#define PMU_GLB_INT_CLR_SET_ALL1      0x0
#define PMU_GLB_INT_CLR_SET_ALL0      0x0
#define PMU_GLB_INT_CLR_SET_ALL5A     0x0
#define PMU_GLB_INT_CLR_SET_ALLA5     0x0

// FIELD IN REGISTER : INT_CLR_SET

#define PMU_GLB_INT_CLR_SET_VAL_SET_POS      0
#define PMU_GLB_INT_CLR_SET_VAL_SET_LEN      32
#define PMU_GLB_INT_CLR_SET_VAL_SET_MASK     0xFFFFFFFF
#define PMU_GLB_INT_CLR_SET_VAL_SET_VAL(w)   (((w) & 0xFFFFFFFF) >> 0)
#define PMU_GLB_INT_CLR_SET_VAL_SET_SET(w)   w |= ( 0xFFFFFFFF << 0)
#define PMU_GLB_INT_CLR_SET_VAL_SET_CLR(w)   w &= ~( 0xFFFFFFFF << 0)

// REGISTER : INT_CLR_CLR

#define PMU_GLB_INT_CLR_CLR_ADDR      PMU_GLB_BASE_ADDR + 0x250
#define PMU_GLB_INT_CLR_CLR_RSTVAL    0x0
#define PMU_GLB_INT_CLR_CLR_ALL1      0x0
#define PMU_GLB_INT_CLR_CLR_ALL0      0x0
#define PMU_GLB_INT_CLR_CLR_ALL5A     0x0
#define PMU_GLB_INT_CLR_CLR_ALLA5     0x0

// FIELD IN REGISTER : INT_CLR_CLR

#define PMU_GLB_INT_CLR_CLR_VAL_CLR_POS      0
#define PMU_GLB_INT_CLR_CLR_VAL_CLR_LEN      32
#define PMU_GLB_INT_CLR_CLR_VAL_CLR_MASK     0xFFFFFFFF
#define PMU_GLB_INT_CLR_CLR_VAL_CLR_VAL(w)   (((w) & 0xFFFFFFFF) >> 0)
#define PMU_GLB_INT_CLR_CLR_VAL_CLR_SET(w)   w |= ( 0xFFFFFFFF << 0)
#define PMU_GLB_INT_CLR_CLR_VAL_CLR_CLR(w)   w &= ~( 0xFFFFFFFF << 0)

// REGISTER : INT_CLR1_SET

#define PMU_GLB_INT_CLR1_SET_ADDR      PMU_GLB_BASE_ADDR + 0x254
#define PMU_GLB_INT_CLR1_SET_RSTVAL    0x0
#define PMU_GLB_INT_CLR1_SET_ALL1      0x0
#define PMU_GLB_INT_CLR1_SET_ALL0      0x0
#define PMU_GLB_INT_CLR1_SET_ALL5A     0x0
#define PMU_GLB_INT_CLR1_SET_ALLA5     0x0

// FIELD IN REGISTER : INT_CLR1_SET

#define PMU_GLB_INT_CLR1_SET_VAL_SET_POS      0
#define PMU_GLB_INT_CLR1_SET_VAL_SET_LEN      32
#define PMU_GLB_INT_CLR1_SET_VAL_SET_MASK     0xFFFFFFFF
#define PMU_GLB_INT_CLR1_SET_VAL_SET_VAL(w)   (((w) & 0xFFFFFFFF) >> 0)
#define PMU_GLB_INT_CLR1_SET_VAL_SET_SET(w)   w |= ( 0xFFFFFFFF << 0)
#define PMU_GLB_INT_CLR1_SET_VAL_SET_CLR(w)   w &= ~( 0xFFFFFFFF << 0)

// REGISTER : INT_CLR1_CLR

#define PMU_GLB_INT_CLR1_CLR_ADDR      PMU_GLB_BASE_ADDR + 0x258
#define PMU_GLB_INT_CLR1_CLR_RSTVAL    0x0
#define PMU_GLB_INT_CLR1_CLR_ALL1      0x0
#define PMU_GLB_INT_CLR1_CLR_ALL0      0x0
#define PMU_GLB_INT_CLR1_CLR_ALL5A     0x0
#define PMU_GLB_INT_CLR1_CLR_ALLA5     0x0

// FIELD IN REGISTER : INT_CLR1_CLR

#define PMU_GLB_INT_CLR1_CLR_VAL_CLR_POS      0
#define PMU_GLB_INT_CLR1_CLR_VAL_CLR_LEN      32
#define PMU_GLB_INT_CLR1_CLR_VAL_CLR_MASK     0xFFFFFFFF
#define PMU_GLB_INT_CLR1_CLR_VAL_CLR_VAL(w)   (((w) & 0xFFFFFFFF) >> 0)
#define PMU_GLB_INT_CLR1_CLR_VAL_CLR_SET(w)   w |= ( 0xFFFFFFFF << 0)
#define PMU_GLB_INT_CLR1_CLR_VAL_CLR_CLR(w)   w &= ~( 0xFFFFFFFF << 0)

// REGISTER : INT_CLR2_SET

#define PMU_GLB_INT_CLR2_SET_ADDR      PMU_GLB_BASE_ADDR + 0x25C
#define PMU_GLB_INT_CLR2_SET_RSTVAL    0x0
#define PMU_GLB_INT_CLR2_SET_ALL1      0x0
#define PMU_GLB_INT_CLR2_SET_ALL0      0x0
#define PMU_GLB_INT_CLR2_SET_ALL5A     0x0
#define PMU_GLB_INT_CLR2_SET_ALLA5     0x0

// FIELD IN REGISTER : INT_CLR2_SET

#define PMU_GLB_INT_CLR2_SET_VAL_SET_POS      0
#define PMU_GLB_INT_CLR2_SET_VAL_SET_LEN      4
#define PMU_GLB_INT_CLR2_SET_VAL_SET_MASK     0x0000000F
#define PMU_GLB_INT_CLR2_SET_VAL_SET_VAL(w)   (((w) & 0x0000000F) >> 0)
#define PMU_GLB_INT_CLR2_SET_VAL_SET_SET(w)   w |= ( 0xF << 0)
#define PMU_GLB_INT_CLR2_SET_VAL_SET_CLR(w)   w &= ~( 0xF << 0)

// REGISTER : INT_CLR2_CLR

#define PMU_GLB_INT_CLR2_CLR_ADDR      PMU_GLB_BASE_ADDR + 0x260
#define PMU_GLB_INT_CLR2_CLR_RSTVAL    0x0
#define PMU_GLB_INT_CLR2_CLR_ALL1      0x0
#define PMU_GLB_INT_CLR2_CLR_ALL0      0x0
#define PMU_GLB_INT_CLR2_CLR_ALL5A     0x0
#define PMU_GLB_INT_CLR2_CLR_ALLA5     0x0

// FIELD IN REGISTER : INT_CLR2_CLR

#define PMU_GLB_INT_CLR2_CLR_VAL_CLR_POS      0
#define PMU_GLB_INT_CLR2_CLR_VAL_CLR_LEN      4
#define PMU_GLB_INT_CLR2_CLR_VAL_CLR_MASK     0x0000000F
#define PMU_GLB_INT_CLR2_CLR_VAL_CLR_VAL(w)   (((w) & 0x0000000F) >> 0)
#define PMU_GLB_INT_CLR2_CLR_VAL_CLR_SET(w)   w |= ( 0xF << 0)
#define PMU_GLB_INT_CLR2_CLR_VAL_CLR_CLR(w)   w &= ~( 0xF << 0)

// REGISTER : INT_RAW_SET

#define PMU_GLB_INT_RAW_SET_ADDR      PMU_GLB_BASE_ADDR + 0x264
#define PMU_GLB_INT_RAW_SET_RSTVAL    0x0
#define PMU_GLB_INT_RAW_SET_ALL1      0x0
#define PMU_GLB_INT_RAW_SET_ALL0      0x0
#define PMU_GLB_INT_RAW_SET_ALL5A     0x0
#define PMU_GLB_INT_RAW_SET_ALLA5     0x0

// FIELD IN REGISTER : INT_RAW_SET

#define PMU_GLB_INT_RAW_SET_VAL_SET_POS      0
#define PMU_GLB_INT_RAW_SET_VAL_SET_LEN      32
#define PMU_GLB_INT_RAW_SET_VAL_SET_MASK     0xFFFFFFFF
#define PMU_GLB_INT_RAW_SET_VAL_SET_VAL(w)   (((w) & 0xFFFFFFFF) >> 0)
#define PMU_GLB_INT_RAW_SET_VAL_SET_SET(w)   w |= ( 0xFFFFFFFF << 0)
#define PMU_GLB_INT_RAW_SET_VAL_SET_CLR(w)   w &= ~( 0xFFFFFFFF << 0)

// REGISTER : INT_RAW_CLR

#define PMU_GLB_INT_RAW_CLR_ADDR      PMU_GLB_BASE_ADDR + 0x268
#define PMU_GLB_INT_RAW_CLR_RSTVAL    0x0
#define PMU_GLB_INT_RAW_CLR_ALL1      0x0
#define PMU_GLB_INT_RAW_CLR_ALL0      0x0
#define PMU_GLB_INT_RAW_CLR_ALL5A     0x0
#define PMU_GLB_INT_RAW_CLR_ALLA5     0x0

// FIELD IN REGISTER : INT_RAW_CLR

#define PMU_GLB_INT_RAW_CLR_VAL_CLR_POS      0
#define PMU_GLB_INT_RAW_CLR_VAL_CLR_LEN      32
#define PMU_GLB_INT_RAW_CLR_VAL_CLR_MASK     0xFFFFFFFF
#define PMU_GLB_INT_RAW_CLR_VAL_CLR_VAL(w)   (((w) & 0xFFFFFFFF) >> 0)
#define PMU_GLB_INT_RAW_CLR_VAL_CLR_SET(w)   w |= ( 0xFFFFFFFF << 0)
#define PMU_GLB_INT_RAW_CLR_VAL_CLR_CLR(w)   w &= ~( 0xFFFFFFFF << 0)

// REGISTER : INT_RAW1_SET

#define PMU_GLB_INT_RAW1_SET_ADDR      PMU_GLB_BASE_ADDR + 0x26C
#define PMU_GLB_INT_RAW1_SET_RSTVAL    0x0
#define PMU_GLB_INT_RAW1_SET_ALL1      0x0
#define PMU_GLB_INT_RAW1_SET_ALL0      0x0
#define PMU_GLB_INT_RAW1_SET_ALL5A     0x0
#define PMU_GLB_INT_RAW1_SET_ALLA5     0x0

// FIELD IN REGISTER : INT_RAW1_SET

#define PMU_GLB_INT_RAW1_SET_VAL_SET_POS      0
#define PMU_GLB_INT_RAW1_SET_VAL_SET_LEN      32
#define PMU_GLB_INT_RAW1_SET_VAL_SET_MASK     0xFFFFFFFF
#define PMU_GLB_INT_RAW1_SET_VAL_SET_VAL(w)   (((w) & 0xFFFFFFFF) >> 0)
#define PMU_GLB_INT_RAW1_SET_VAL_SET_SET(w)   w |= ( 0xFFFFFFFF << 0)
#define PMU_GLB_INT_RAW1_SET_VAL_SET_CLR(w)   w &= ~( 0xFFFFFFFF << 0)

// REGISTER : INT_RAW1_CLR

#define PMU_GLB_INT_RAW1_CLR_ADDR      PMU_GLB_BASE_ADDR + 0x270
#define PMU_GLB_INT_RAW1_CLR_RSTVAL    0x0
#define PMU_GLB_INT_RAW1_CLR_ALL1      0x0
#define PMU_GLB_INT_RAW1_CLR_ALL0      0x0
#define PMU_GLB_INT_RAW1_CLR_ALL5A     0x0
#define PMU_GLB_INT_RAW1_CLR_ALLA5     0x0

// FIELD IN REGISTER : INT_RAW1_CLR

#define PMU_GLB_INT_RAW1_CLR_VAL_CLR_POS      0
#define PMU_GLB_INT_RAW1_CLR_VAL_CLR_LEN      32
#define PMU_GLB_INT_RAW1_CLR_VAL_CLR_MASK     0xFFFFFFFF
#define PMU_GLB_INT_RAW1_CLR_VAL_CLR_VAL(w)   (((w) & 0xFFFFFFFF) >> 0)
#define PMU_GLB_INT_RAW1_CLR_VAL_CLR_SET(w)   w |= ( 0xFFFFFFFF << 0)
#define PMU_GLB_INT_RAW1_CLR_VAL_CLR_CLR(w)   w &= ~( 0xFFFFFFFF << 0)

// REGISTER : INT_RAW2_SET

#define PMU_GLB_INT_RAW2_SET_ADDR      PMU_GLB_BASE_ADDR + 0x274
#define PMU_GLB_INT_RAW2_SET_RSTVAL    0x0
#define PMU_GLB_INT_RAW2_SET_ALL1      0x0
#define PMU_GLB_INT_RAW2_SET_ALL0      0x0
#define PMU_GLB_INT_RAW2_SET_ALL5A     0x0
#define PMU_GLB_INT_RAW2_SET_ALLA5     0x0

// FIELD IN REGISTER : INT_RAW2_SET

#define PMU_GLB_INT_RAW2_SET_VAL_SET_POS      0
#define PMU_GLB_INT_RAW2_SET_VAL_SET_LEN      4
#define PMU_GLB_INT_RAW2_SET_VAL_SET_MASK     0x0000000F
#define PMU_GLB_INT_RAW2_SET_VAL_SET_VAL(w)   (((w) & 0x0000000F) >> 0)
#define PMU_GLB_INT_RAW2_SET_VAL_SET_SET(w)   w |= ( 0xF << 0)
#define PMU_GLB_INT_RAW2_SET_VAL_SET_CLR(w)   w &= ~( 0xF << 0)

// REGISTER : INT_RAW2_CLR

#define PMU_GLB_INT_RAW2_CLR_ADDR      PMU_GLB_BASE_ADDR + 0x278
#define PMU_GLB_INT_RAW2_CLR_RSTVAL    0x0
#define PMU_GLB_INT_RAW2_CLR_ALL1      0x0
#define PMU_GLB_INT_RAW2_CLR_ALL0      0x0
#define PMU_GLB_INT_RAW2_CLR_ALL5A     0x0
#define PMU_GLB_INT_RAW2_CLR_ALLA5     0x0

// FIELD IN REGISTER : INT_RAW2_CLR

#define PMU_GLB_INT_RAW2_CLR_VAL_CLR_POS      0
#define PMU_GLB_INT_RAW2_CLR_VAL_CLR_LEN      4
#define PMU_GLB_INT_RAW2_CLR_VAL_CLR_MASK     0x0000000F
#define PMU_GLB_INT_RAW2_CLR_VAL_CLR_VAL(w)   (((w) & 0x0000000F) >> 0)
#define PMU_GLB_INT_RAW2_CLR_VAL_CLR_SET(w)   w |= ( 0xF << 0)
#define PMU_GLB_INT_RAW2_CLR_VAL_CLR_CLR(w)   w &= ~( 0xF << 0)

// REGISTER : INT_STA_SET

#define PMU_GLB_INT_STA_SET_ADDR      PMU_GLB_BASE_ADDR + 0x27C
#define PMU_GLB_INT_STA_SET_RSTVAL    0x0
#define PMU_GLB_INT_STA_SET_ALL1      0x0
#define PMU_GLB_INT_STA_SET_ALL0      0x0
#define PMU_GLB_INT_STA_SET_ALL5A     0x0
#define PMU_GLB_INT_STA_SET_ALLA5     0x0

// FIELD IN REGISTER : INT_STA_SET

#define PMU_GLB_INT_STA_SET_VAL_SET_POS      0
#define PMU_GLB_INT_STA_SET_VAL_SET_LEN      32
#define PMU_GLB_INT_STA_SET_VAL_SET_MASK     0xFFFFFFFF
#define PMU_GLB_INT_STA_SET_VAL_SET_VAL(w)   (((w) & 0xFFFFFFFF) >> 0)
#define PMU_GLB_INT_STA_SET_VAL_SET_SET(w)   w |= ( 0xFFFFFFFF << 0)
#define PMU_GLB_INT_STA_SET_VAL_SET_CLR(w)   w &= ~( 0xFFFFFFFF << 0)

// REGISTER : INT_STA_CLR

#define PMU_GLB_INT_STA_CLR_ADDR      PMU_GLB_BASE_ADDR + 0x280
#define PMU_GLB_INT_STA_CLR_RSTVAL    0x0
#define PMU_GLB_INT_STA_CLR_ALL1      0x0
#define PMU_GLB_INT_STA_CLR_ALL0      0x0
#define PMU_GLB_INT_STA_CLR_ALL5A     0x0
#define PMU_GLB_INT_STA_CLR_ALLA5     0x0

// FIELD IN REGISTER : INT_STA_CLR

#define PMU_GLB_INT_STA_CLR_VAL_CLR_POS      0
#define PMU_GLB_INT_STA_CLR_VAL_CLR_LEN      32
#define PMU_GLB_INT_STA_CLR_VAL_CLR_MASK     0xFFFFFFFF
#define PMU_GLB_INT_STA_CLR_VAL_CLR_VAL(w)   (((w) & 0xFFFFFFFF) >> 0)
#define PMU_GLB_INT_STA_CLR_VAL_CLR_SET(w)   w |= ( 0xFFFFFFFF << 0)
#define PMU_GLB_INT_STA_CLR_VAL_CLR_CLR(w)   w &= ~( 0xFFFFFFFF << 0)

// REGISTER : INT_STA1_SET

#define PMU_GLB_INT_STA1_SET_ADDR      PMU_GLB_BASE_ADDR + 0x284
#define PMU_GLB_INT_STA1_SET_RSTVAL    0x0
#define PMU_GLB_INT_STA1_SET_ALL1      0x0
#define PMU_GLB_INT_STA1_SET_ALL0      0x0
#define PMU_GLB_INT_STA1_SET_ALL5A     0x0
#define PMU_GLB_INT_STA1_SET_ALLA5     0x0

// FIELD IN REGISTER : INT_STA1_SET

#define PMU_GLB_INT_STA1_SET_VAL_SET_POS      0
#define PMU_GLB_INT_STA1_SET_VAL_SET_LEN      32
#define PMU_GLB_INT_STA1_SET_VAL_SET_MASK     0xFFFFFFFF
#define PMU_GLB_INT_STA1_SET_VAL_SET_VAL(w)   (((w) & 0xFFFFFFFF) >> 0)
#define PMU_GLB_INT_STA1_SET_VAL_SET_SET(w)   w |= ( 0xFFFFFFFF << 0)
#define PMU_GLB_INT_STA1_SET_VAL_SET_CLR(w)   w &= ~( 0xFFFFFFFF << 0)

// REGISTER : INT_STA1_CLR

#define PMU_GLB_INT_STA1_CLR_ADDR      PMU_GLB_BASE_ADDR + 0x288
#define PMU_GLB_INT_STA1_CLR_RSTVAL    0x0
#define PMU_GLB_INT_STA1_CLR_ALL1      0x0
#define PMU_GLB_INT_STA1_CLR_ALL0      0x0
#define PMU_GLB_INT_STA1_CLR_ALL5A     0x0
#define PMU_GLB_INT_STA1_CLR_ALLA5     0x0

// FIELD IN REGISTER : INT_STA1_CLR

#define PMU_GLB_INT_STA1_CLR_VAL_CLR_POS      0
#define PMU_GLB_INT_STA1_CLR_VAL_CLR_LEN      32
#define PMU_GLB_INT_STA1_CLR_VAL_CLR_MASK     0xFFFFFFFF
#define PMU_GLB_INT_STA1_CLR_VAL_CLR_VAL(w)   (((w) & 0xFFFFFFFF) >> 0)
#define PMU_GLB_INT_STA1_CLR_VAL_CLR_SET(w)   w |= ( 0xFFFFFFFF << 0)
#define PMU_GLB_INT_STA1_CLR_VAL_CLR_CLR(w)   w &= ~( 0xFFFFFFFF << 0)

// REGISTER : INT_STA2_SET

#define PMU_GLB_INT_STA2_SET_ADDR      PMU_GLB_BASE_ADDR + 0x28C
#define PMU_GLB_INT_STA2_SET_RSTVAL    0x0
#define PMU_GLB_INT_STA2_SET_ALL1      0x0
#define PMU_GLB_INT_STA2_SET_ALL0      0x0
#define PMU_GLB_INT_STA2_SET_ALL5A     0x0
#define PMU_GLB_INT_STA2_SET_ALLA5     0x0

// FIELD IN REGISTER : INT_STA2_SET

#define PMU_GLB_INT_STA2_SET_VAL_SET_POS      0
#define PMU_GLB_INT_STA2_SET_VAL_SET_LEN      4
#define PMU_GLB_INT_STA2_SET_VAL_SET_MASK     0x0000000F
#define PMU_GLB_INT_STA2_SET_VAL_SET_VAL(w)   (((w) & 0x0000000F) >> 0)
#define PMU_GLB_INT_STA2_SET_VAL_SET_SET(w)   w |= ( 0xF << 0)
#define PMU_GLB_INT_STA2_SET_VAL_SET_CLR(w)   w &= ~( 0xF << 0)

// REGISTER : INT_STA2_CLR

#define PMU_GLB_INT_STA2_CLR_ADDR      PMU_GLB_BASE_ADDR + 0x290
#define PMU_GLB_INT_STA2_CLR_RSTVAL    0x0
#define PMU_GLB_INT_STA2_CLR_ALL1      0x0
#define PMU_GLB_INT_STA2_CLR_ALL0      0x0
#define PMU_GLB_INT_STA2_CLR_ALL5A     0x0
#define PMU_GLB_INT_STA2_CLR_ALLA5     0x0

// FIELD IN REGISTER : INT_STA2_CLR

#define PMU_GLB_INT_STA2_CLR_VAL_CLR_POS      0
#define PMU_GLB_INT_STA2_CLR_VAL_CLR_LEN      4
#define PMU_GLB_INT_STA2_CLR_VAL_CLR_MASK     0x0000000F
#define PMU_GLB_INT_STA2_CLR_VAL_CLR_VAL(w)   (((w) & 0x0000000F) >> 0)
#define PMU_GLB_INT_STA2_CLR_VAL_CLR_SET(w)   w |= ( 0xF << 0)
#define PMU_GLB_INT_STA2_CLR_VAL_CLR_CLR(w)   w &= ~( 0xF << 0)

#endif
