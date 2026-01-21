/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#ifndef __DT_BINDINGS_CLOCK_AX650X_H
#define __DT_BINDINGS_CLOCK_AX650X_H


/*comm clk*/
#define AX650X_CLK_SEN_REF1_SEL      0
#define AX650X_CLK_SEN_REF0_SEL      1
#define AX650X_CLK_RISCV_BUS_SEL     2
#define AX650X_CLK_I2S_REF3_SEL      3
#define AX650X_CLK_I2S_REF2_SEL      4
#define AX650X_CLK_I2S_REF1_SEL      5
#define AX650X_CLK_I2S_REF0_SEL      6
#define AX650X_CLK_EPHY_REF_SEL      7
#define AX650X_CLK_DBC_GPIO_SEL      8
#define AX650X_CLK_COMM_WDT_SEL      9
#define AX650X_CLK_COMM_TMR1_SEL     10
#define AX650X_CLK_COMM_TMR0_SEL     11
#define AX650X_CLK_COMM_RTC1_SEL     12
#define AX650X_CLK_COMM_RTC0_SEL     13
#define AX650X_CLK_COMM_IP_SEL       14
#define AX650X_ACLK_TOP_SEL          15
#define AX650X_ACLK_TOP_NN_SEL       16
#define AX650X_CLK_CE_BUS_SEL        17
#define AX650X_ACLK_TOP1_SEL         18
#define AX650X_ACLK_TOP2_SEL         19
#define AX650X_ACLK_TOP3_SEL         20
#define AX650X_PCLK_TOP_SEL          21
#define AX650X_CLK_SEN_REF7_SEL      22
#define AX650X_CLK_SEN_REF6_SEL      23
#define AX650X_CLK_SEN_REF5_SEL      24
#define AX650X_CLK_SEN_REF4_SEL      25
#define AX650X_CLK_SEN_REF3_SEL      26
#define AX650X_CLK_SEN_REF2_SEL      27
#define AX650X_CLK_SEN_REF7          28
#define AX650X_CLK_SEN_REF6          29
#define AX650X_CLK_SEN_REF5          30
#define AX650X_CLK_SEN_REF4          31
#define AX650X_CLK_SEN_REF3          32
#define AX650X_CLK_SEN_REF2          33
#define AX650X_CLK_SEN_REF1          34
#define AX650X_CLK_SEN_REF0          35
#define AX650X_CLK_I2S_REF3          36
#define AX650X_CLK_I2S_REF2          37
#define AX650X_CLK_I2S_REF1          38
#define AX650X_CLK_I2S_REF0          39
#define AX650X_CLK_EPHY_REF          40
#define AX650X_CLK_DEBUG_TLB         41
#define AX650X_CLK_COMM_WDT          42
#define AX650X_CLK_COMM_TMR1         43
#define AX650X_CLK_COMM_TMR0         44
#define AX650X_CLK_COMM_RTC1         45
#define AX650X_CLK_COMM_RTC0         46
#define AX650X_CLK_COMM_ROSC         47
#define AX650X_CLK_COMM_IP           48
#define AX650X_CLK_CE_CNT            49
#define AX650X_PCLK_RTC0             50
#define AX650X_PCLK_PUB_CE           51
#define AX650X_PCLK_PLLC             52
#define AX650X_PCLK_MAILBOX          53
#define AX650X_PCLK_GPIO             54
#define AX650X_PCLK_DFS_MNR          55
#define AX650X_PCLK_BUS_LOCK         56
#define AX650X_PCLK_ADVFS1           57
#define AX650X_PCLK_ADVFS0           58
#define AX650X_CLK_WDT4              59
#define AX650X_CLK_WDT3              60
#define AX650X_CLK_WDT2              61
#define AX650X_CLK_WDT1              62
#define AX650X_CLK_WDT0              63
#define AX650X_CLK_TMR64             64
#define AX650X_CLK_TMR32_3           65
#define AX650X_CLK_TMR32_2           66
#define AX650X_CLK_TMR32_1           67
#define AX650X_CLK_TMR32_0           68
#define AX650X_CLK_THM               69
#define AX650X_CLK_RTC3              70
#define AX650X_CLK_RTC2              71
#define AX650X_CLK_RTC1              72
#define AX650X_CLK_RTC0              73
#define AX650X_CLK_RISCV             74
#define AX650X_CLK_PUB_CE            75
#define AX650X_CLK_PUB_CE_CNT        76
#define AX650X_CLK_PINMUX            77
#define AX650X_CLK_GPIO              78
#define AX650X_CLK_EFS               79
#define AX650X_CLK_ADVFS1            80
#define AX650X_CLK_ADVFS0            81
#define AX650X_PCLK_WDT4             82
#define AX650X_PCLK_WDT3             83
#define AX650X_PCLK_WDT2             84
#define AX650X_PCLK_WDT1             85
#define AX650X_PCLK_WDT0             86
#define AX650X_PCLK_TMR64            87
#define AX650X_PCLK_TMR32_3          88
#define AX650X_PCLK_TMR32_2          89
#define AX650X_PCLK_TMR32_1          90
#define AX650X_PCLK_TMR32_0          91
#define AX650X_PCLK_THM              92
#define AX650X_PCLK_SPINLOCK         93
#define AX650X_PCLK_RTC3             94
#define AX650X_PCLK_RTC2             95
#define AX650X_PCLK_RTC1             96
#define AX650X_CLK_SEN_REF1_DIVN     97
#define AX650X_CLK_SEN_REF0_DIVN     98
#define AX650X_CLK_I2S_REF3_DIVN     99
#define AX650X_CLK_I2S_REF2_DIVN     100
#define AX650X_CLK_I2S_REF1_DIVN     101
#define AX650X_CLK_I2S_REF0_DIVN     102
#define AX650X_CLK_SEN_REF7_DIVN     103
#define AX650X_CLK_SEN_REF6_DIVN     104
#define AX650X_CLK_SEN_REF5_DIVN     105
#define AX650X_CLK_SEN_REF4_DIVN     106
#define AX650X_CLK_SEN_REF3_DIVN     107
#define AX650X_CLK_SEN_REF2_DIVN     108
//PLL
#define AX650X_REF32K   109
#define AX650X_REF24M   110
#define AX650X_CPLL0    111
#define AX650X_CPLL1    112
#define AX650X_DPLL0    113
#define AX650X_DPLL1    114
#define AX650X_NAPLL    115
#define AX650X_VPLL0    116
#define AX650X_VPLL1    117
#define AX650X_APLL0    118
#define AX650X_APLL1    119
#define AX650X_NPLL 120
#define AX650X_MPLL 121
#define AX650X_EPLL 122
#define AX650X_GPLL 123
#define AX650X_DSPLL    124
#define AX650X_NFPLL    125
//PLL_DIV
#define AX650X_CPLL_1600M   126
#define AX650X_CPLL_25M 127
#define AX650X_CPLL_1100M   128
#define AX650X_CPLL_550M    129
#define AX650X_CPLL_27P5M   130
#define AX650X_NPLL_1600M   131
#define AX650X_NPLL_800M    132
#define AX650X_NPLL_800M_90 133
#define AX650X_NPLL_800M_180    134
#define AX650X_NPLL_800M_270    135
#define AX650X_NPLL_400M    136
#define AX650X_NPLL_200M    137
#define AX650X_NPLL_80M 138
#define AX650X_NPLL_20M 139
#define AX650X_VPLL_594M    140
#define AX650X_VPLL1_594M   141
#define AX650X_VPLL_74P25M  142
#define AX650X_VPLL1_74P25M 143
#define AX650X_VPLL_27M 144
#define AX650X_VPLL1_27M    145
#define AX650X_VPLL_198M    146
#define AX650X_VPLL1_198M   147
#define AX650X_VPLL_118P8M  148
#define AX650X_VPLL1_118P8M 149
#define AX650X_VPLL_108M    150
#define AX650X_VPLL1_108M   151
#define AX650X_EPLL_1500M   152
#define AX650X_EPLL_750M    153
#define AX650X_EPLL_750M_90 154
#define AX650X_EPLL_750M_180    155
#define AX650X_EPLL_750M_270    156
#define AX650X_EPLL_187P5M  157
#define AX650X_EPLL_500M    158
#define AX650X_EPLL_500M_60 159
#define AX650X_EPLL_500M_120    160
#define AX650X_EPLL_500M_180    161
#define AX650X_EPLL_250M    162
#define AX650X_EPLL_125M    163
#define AX650X_EPLL_300M    164
#define AX650X_EPLL_150M    165
#define AX650X_EPLL_30M 166
#define AX650X_EPLL_75M 167
#define AX650X_EPLL_100M    168
#define AX650X_EPLL_50M 169
#define AX650X_EPLL_25M 170
#define AX650X_EPLL_20M 171
#define AX650X_EPLL_5M  172
#define AX650X_DPLL_1066M   173
#define AX650X_DPLL1_1066M  174
#define AX650X_DPLL_16P65M  175
#define AX650X_DPLL1_16P65M 176
#define AX650X_GPLL_1248M   177
#define AX650X_GPLL_624M    178
#define AX650X_GPLL_624M_90 179
#define AX650X_GPLL_624M_180    180
#define AX650X_GPLL_624M_270    181
#define AX650X_GPLL_24M 182
#define AX650X_GPLL_12M 183
#define AX650X_GPLL_26M 184
#define AX650X_GPLL_19P2M   185
#define AX650X_GPLL_416M    186
#define AX650X_GPLL_208M    187
#define AX650X_MPLL_1400M   188
#define AX650X_MPLL_700M    189
#define AX650X_MPLL_700M_90 190
#define AX650X_MPLL_700M_180    191
#define AX650X_MPLL_700M_270    192
#define AX650X_MPLL_350M    193
#define AX650X_MPLL_21P875M 194
#define AX650X_XTAL_24M 195
#define AX650X_XTAL_8M  196
#define AX650X_NAPLL_2400M  197
#define AX650X_NAPLL_800M   198
#define AX650X_NAPLL_20M    199
#define AX650X_NFPLL_594M   200
#define AX650X_NFPLL_27M    201
#define AX650X_NFPLL_74P25M 202
#define AX650X_DSPLL_800M   203
#define AX650X_DSPLL_800M_90    204
#define AX650X_DSPLL_800M_180   205
#define AX650X_DSPLL_800M_270   206
#define AX650X_DSPLL_20M    207
#define AX650X_APLL_20P48M  208
#define AX650X_APLL_4096K   209
#define AX650X_APLL_12P288M 210
#define AX650X_APLL_614P4M  211
#define AX650X_APLL1_20P48M 212
#define AX650X_APLL_76P8M   213
#define AX650X_APLL1_4096K  214
#define AX650X_APLL1_12P288M    215
#define AX650X_COMM_NR_CLKS          (AX650X_APLL_12P288M + 1)

/*cpu clk*/
#define AX650X_PCLK_SEL               0
#define AX650X_GICCLK_SEL             1
#define AX650X_CLK_DSU_SEL            2
#define AX650X_CLK_DSU_PERIPH_SEL     3
#define AX650X_CLK_CPU7_SEL           4
#define AX650X_CLK_CPU6_SEL           5
#define AX650X_CLK_CPU5_SEL           6
#define AX650X_CLK_CPU4_SEL           7
#define AX650X_CLK_CPU3_SEL           8
#define AX650X_CLK_CPU2_SEL           9
#define AX650X_CLK_CPU1_SEL           10
#define AX650X_CLK_CPU0_SEL           11
#define AX650X_ATCLK_SEL              12
#define AX650X_PCLK                   13
#define AX650X_GICCLK                 14
#define AX650X_CLK_DSU_PERIPH         15
#define AX650X_CLK_CPU_ROSC           16
#define AX650X_ATCLK                  17
#define AX650X_CLK_DSU_CG             18
#define AX650X_CPU_NR_CLKS            (AX650X_CLK_DSU_CG + 1)

/*ddr clk*/
#define AX650X_CLK_DDR_SEL         0
#define AX650X_CLK_DDR_GLB_SEL     1
#define AX650X_CLK_DDR_ROSC        2
#define AX650X_CLK_DDR_ORG         3
#define AX650X_CLK_DDR_BW_MNR      4
#define AX650X_PCLK_PERF_MNR4      5
#define AX650X_PCLK_PERF_MNR3      6
#define AX650X_PCLK_PERF_MNR2      7
#define AX650X_PCLK_PERF_MNR1      8
#define AX650X_PCLK_PERF_MNR0      9
#define AX650X_PCLK_DDR_PHY        10
#define AX650X_PCLK_DDR            11
#define AX650X_DFICLK_DDR          12
#define AX650X_CLK_DDR_CORE        13
#define AX650X_CLK_DDR_DIVN        14
#define AX650X_DDR_NR_CLKS         (AX650X_CLK_DDR_DIVN + 1)

/*flash clk*/
#define AX650X_CLK_FLASH_SER_SEL         0
#define AX650X_CLK_FLASH_GLB_SEL         1
#define AX650X_CLK_EMMC_LBT_SEL          2
#define AX650X_CCLK_TX_FLS_SDIO_SEL      3
#define AX650X_CCLK_TX_EMMC_SEL          4
#define AX650X_USB2_REF_ALT_CLK          5
#define AX650X_TCLK_FLS_SDIO             6
#define AX650X_TCLK_EMMC                 7
#define AX650X_CLK_USB2_REF              8
#define AX650X_CLK_GTMR                  9
#define AX650X_CLK_FLASH_SER             10
#define AX650X_CLK_EMMC_LBT              11
#define AX650X_CCLK_TX_FLS_SDIO          12
#define AX650X_CCLK_TX_EMMC              13
#define AX650X_BCLK_FLS_SDIO             14
#define AX650X_BCLK_EMMC                 15
#define AX650X_TMCLK_FLS_SDIO            16
#define AX650X_TMCLK_EMMC                17
#define AX650X_PCLK_USB2_PHY             18
#define AX650X_PCLK_SPI                  19
#define AX650X_HCLK_SPI                  20
#define AX650X_HCLK_FLS_SDIO             21
#define AX650X_HCLK_EMMC                 22
#define AX650X_CQETMCLK_FLS_SDIO         23
#define AX650X_CQETMCLK_EMMC             24
#define AX650X_CLK_P_SPI                 25
#define AX650X_CLK_H_SPI                 26
#define AX650X_CLK_DMA                   27
#define AX650X_BUS_CLK_USB2              28
#define AX650X_ACLK_FLS_SDIO             29
#define AX650X_ACLK_EMMC                 30
#define AX650X_CCLK_TX_FLS_SDIO_DIVN     31
#define AX650X_CCLK_TX_EMMC_DIVN         32
#define AX650X_FLASH_NR_CLKS             (AX650X_CCLK_TX_EMMC_DIVN + 1)

/*isp clk*/
#define AX650X_CLK_ISP_LS_SEL           0
#define AX650X_CLK_ISP_HS_SEL           1
#define AX650X_CLK_ISP_GLB_SEL          2
#define AX650X_CLK_ISP_DPU_OUT_SEL      3
#define AX650X_CLK_ISP_BUS_SEL          4
#define AX650X_CLK_ISP_ROSC             5
#define AX650X_CLK_ISP_ITS              6
#define AX650X_CLK_ISP_DPU_OUT          7
#define AX650X_CLK_ISP_YUV              8
#define AX650X_CLK_ISP_ITP              9
#define AX650X_CLK_ISP_IFE              10
#define AX650X_CLK_ISP_DPU              11
#define AX650X_CLK_ISP_AXIM             12
#define AX650X_CLK_ISP_DPU_OUT_DIVN     13
#define AX650X_ISP_NR_CLKS              (AX650X_CLK_ISP_DPU_OUT_DIVN + 1)

/*mm clk*/
#define AX650X_SCLK_MM_I2S1_SEL         0
#define AX650X_SCLK_MM_I2S0_SEL         1
#define AX650X_CLK_MM_RGMII_TX_SEL      2
#define AX650X_CLK_MM_GLB_SEL           3
#define AX650X_CLK_MM_DPU_OUT1_SEL      4
#define AX650X_CLK_MM_DPU_OUT0_SEL      5
#define AX650X_CLK_MM_CORE_SEL          6
#define AX650X_CLK_MM_BUS_SEL           7
#define AX650X_SCLK_MM_I2S1             8
#define AX650X_SCLK_MM_I2S0             9
#define AX650X_CLK_MM_RMII_RX           10
#define AX650X_CLK_MM_RMII_PHY          11
#define AX650X_CLK_MM_RGMII_TX          12
#define AX650X_CLK_MM_ISFR              13
#define AX650X_CLK_MM_EMAC_PTP          14
#define AX650X_CLK_MM_DPU_OUT1          15
#define AX650X_CLK_MM_DPU_OUT0          16
#define AX650X_CLK_HDMI_32K             17
#define AX650X_CLK_CSI_ASYNC            18
#define AX650X_PCLK_I2S1                19
#define AX650X_PCLK_I2S0                20
#define AX650X_PCLK_HDMI1               21
#define AX650X_PCLK_HDMI0               22
#define AX650X_PCLK_DSITX               23
#define AX650X_PCLK_CSITX               24
#define AX650X_CLK_JENC                 25
#define AX650X_CLK_JDEC                 26
#define AX650X_CLK_HDMI_VIDEO1          27
#define AX650X_CLK_HDMI_VIDEO0          28
#define AX650X_CLK_HDMI1_ISFR           29
#define AX650X_CLK_HDMI0_ISFR           30
#define AX650X_CLK_DPU1                 31
#define AX650X_CLK_DPU0                 32
#define AX650X_CLK_CMD                  33
#define AX650X_CLK_AXI2CSI              34
#define AX650X_ACLK_MM_EMAC             35
#define AX650X_SCLK_MM_I2S1_DIVN        36
#define AX650X_SCLK_MM_I2S0_DIVN        37
#define AX650X_CLK_MM_RMII_RX_DIVN      38
#define AX650X_CLK_MM_DPU_OUT1_DIVN     39
#define AX650X_CLK_MM_DPU_OUT0_DIVN     40
#define AX650X_MM_NR_CLKS               (AX650X_CLK_MM_DPU_OUT0_DIVN + 1)

/*mm-core clk*/
#define AX650X_CLK_YUV_WDMA1       0
#define AX650X_CLK_YUV_SCALER1     1
#define AX650X_CLK_VGP             2
#define AX650X_CLK_PYRA_LITE       3
#define AX650X_CLK_IVE             4
#define AX650X_CLK_GDC             5
#define AX650X_CLK_CORE_CMD        6
#define AX650X_MM_CORE_NR_CLKS          (AX650X_CLK_CORE_CMD + 1)

/*mm-phy clk*/
#define AX650X_CLK_DPHY_TX_REF     0
#define AX650X_MM_PHY_NR_CLKS          (AX650X_CLK_DPHY_TX_REF + 1)

/*nn clk*/
#define AX650X_CLK_VEC2_SEL       0
#define AX650X_CLK_VEC1_SEL       1
#define AX650X_CLK_VEC0_SEL       2
#define AX650X_CLK_NN_GLB_SEL     3
#define AX650X_CLK_NN_FAB_SEL     4
#define AX650X_CLK_NN_6P_SEL      5
#define AX650X_CLK_CONV5_SEL      6
#define AX650X_CLK_CONV4_SEL      7
#define AX650X_CLK_CONV3_SEL      8
#define AX650X_CLK_CONV2_SEL      9
#define AX650X_CLK_CONV1_SEL      10
#define AX650X_CLK_CONV0_SEL      11
#define AX650X_CLK_NN_ROSC        12
#define AX650X_PCLK_DFS_MNR_NN       13
#define AX650X_CLK_VEC2_TENG      14
#define AX650X_CLK_VEC2_SDMA      15
#define AX650X_CLK_VEC2_CV        16
#define AX650X_CLK_VEC1_TENG      17
#define AX650X_CLK_VEC1_SDMA      18
#define AX650X_CLK_VEC1_CV        19
#define AX650X_CLK_VEC0_TENG      20
#define AX650X_CLK_VEC0_SDMA      21
#define AX650X_CLK_VEC0_CV        22
#define AX650X_CLK_NN_SM          23
#define AX650X_CLK_NN_MAU         24
#define AX650X_CLK_NN_C5          25
#define AX650X_CLK_NN_C4          26
#define AX650X_CLK_NN_C3          27
#define AX650X_CLK_NN_C2          28
#define AX650X_CLK_NN_C1          29
#define AX650X_CLK_NN_C0          30
#define AX650X_CLK_NEUSIGHT       31
#define AX650X_NN_NR_CLKS         (AX650X_CLK_NEUSIGHT + 1)

/*periph clk*/
#define AX650X_SCLK_TDM_M1_SEL         0
#define AX650X_SCLK_TDM_M0_SEL         1
#define AX650X_SCLK_I2S_M1_SEL         2
#define AX650X_SCLK_I2S_M0_SEL         3
#define AX650X_CLK_PERIPH_TMR1_SEL     4
#define AX650X_CLK_PERIPH_TMR0_SEL     5
#define AX650X_CLK_PERIPH_SER_SEL      6
#define AX650X_CLK_PERIPH_GPIO_SEL     7
#define AX650X_CLK_PERIPH_GLB_SEL      8
#define AX650X_SCLK_TDM_M1             9
#define AX650X_SCLK_TDM_M0             10
#define AX650X_SCLK_I2S_M1             11
#define AX650X_SCLK_I2S_M0             12
#define AX650X_CLK_PERIPH_TMR1         13
#define AX650X_CLK_PERIPH_TMR0         14
#define AX650X_CLK_PERIPH_SER          15
#define AX650X_CLK_PERIPH_GPIO         16
#define AX650X_CLK_PWM12               17
#define AX650X_CLK_PWM11               18
#define AX650X_CLK_PWM08               19
#define AX650X_CLK_PWM07               20
#define AX650X_CLK_PWM06               21
#define AX650X_CLK_PWM05               22
#define AX650X_CLK_PWM04               23
#define AX650X_CLK_PWM03               24
#define AX650X_CLK_PWM02               25
#define AX650X_CLK_PWM01               26
#define AX650X_CLK_I2C9                27
#define AX650X_CLK_I2C8                28
#define AX650X_CLK_I2C7                29
#define AX650X_CLK_I2C6                30
#define AX650X_CLK_I2C5                31
#define AX650X_CLK_I2C4                32
#define AX650X_CLK_I2C3                33
#define AX650X_CLK_I2C2                34
#define AX650X_CLK_I2C1                35
#define AX650X_CLK_I2C14               36
#define AX650X_CLK_I2C13               37
#define AX650X_CLK_I2C12               38
#define AX650X_CLK_I2C11               39
#define AX650X_CLK_I2C10               40
#define AX650X_CLK_I2C0                41
#define AX650X_CLK_GPIO6               42
#define AX650X_CLK_GPIO5               43
#define AX650X_CLK_GPIO4               44
#define AX650X_CLK_GPIO3               45
#define AX650X_CLK_GPIO2               46
#define AX650X_CLK_GPIO1               47
#define AX650X_CLK_GPIO0               48
#define AX650X_CLK_TMR12               49
#define AX650X_CLK_TMR11               50
#define AX650X_CLK_TMR04               51
#define AX650X_CLK_TMR03               52
#define AX650X_CLK_TMR02               53
#define AX650X_CLK_TMR01               54
#define AX650X_CLK_SPI_S               55
#define AX650X_CLK_SPI_M3              56
#define AX650X_CLK_SPI_M2              57
#define AX650X_CLK_SPI_M1              58
#define AX650X_CLK_PWM38               59
#define AX650X_CLK_PWM37               60
#define AX650X_CLK_PWM36               61
#define AX650X_CLK_PWM35               62
#define AX650X_CLK_PWM34               63
#define AX650X_CLK_PWM33               64
#define AX650X_CLK_PWM32               65
#define AX650X_CLK_PWM31               66
#define AX650X_CLK_PWM28               67
#define AX650X_CLK_PWM27               68
#define AX650X_CLK_PWM26               69
#define AX650X_CLK_PWM25               70
#define AX650X_CLK_PWM24               71
#define AX650X_CLK_PWM23               72
#define AX650X_CLK_PWM22               73
#define AX650X_CLK_PWM21               74
#define AX650X_CLK_PWM18               75
#define AX650X_CLK_PWM17               76
#define AX650X_CLK_PWM16               77
#define AX650X_CLK_PWM15               78
#define AX650X_CLK_PWM14               79
#define AX650X_CLK_PWM13               80
#define AX650X_PCLK_GPIO1              81
#define AX650X_PCLK_GPIO0              82
#define AX650X_HCLK_SPI_S              83
#define AX650X_CLK_UART9               84
#define AX650X_CLK_UART8               85
#define AX650X_CLK_UART7               86
#define AX650X_CLK_UART6               87
#define AX650X_CLK_UART5               88
#define AX650X_CLK_UART4               89
#define AX650X_CLK_UART3               90
#define AX650X_CLK_UART2               91
#define AX650X_CLK_UART1               92
#define AX650X_CLK_UART10              93
#define AX650X_CLK_UART0               94
#define AX650X_CLK_TMR54               95
#define AX650X_CLK_TMR53               96
#define AX650X_CLK_TMR52               97
#define AX650X_CLK_TMR51               98
#define AX650X_CLK_TMR44               99
#define AX650X_CLK_TMR43               100
#define AX650X_CLK_TMR42               101
#define AX650X_CLK_TMR41               102
#define AX650X_CLK_TMR34               103
#define AX650X_CLK_TMR33               104
#define AX650X_CLK_TMR32               105
#define AX650X_CLK_TMR31               106
#define AX650X_CLK_TMR24               107
#define AX650X_CLK_TMR23               108
#define AX650X_CLK_TMR22               109
#define AX650X_CLK_TMR21               110
#define AX650X_CLK_TMR14               111
#define AX650X_CLK_TMR13               112
#define AX650X_PCLK_TDM_M0             113
#define AX650X_PCLK_SPI_M3             114
#define AX650X_PCLK_SPI_M2             115
#define AX650X_PCLK_SPI_M1             116
#define AX650X_PCLK_PWM3               117
#define AX650X_PCLK_PWM2               118
#define AX650X_PCLK_PWM1               119
#define AX650X_PCLK_PWM0               120
#define AX650X_PCLK_I2S_S1             121
#define AX650X_PCLK_I2S_S0             122
#define AX650X_PCLK_I2S_M1             123
#define AX650X_PCLK_I2S_M0             124
#define AX650X_PCLK_I2C9               125
#define AX650X_PCLK_I2C8               126
#define AX650X_PCLK_I2C7               127
#define AX650X_PCLK_I2C6               128
#define AX650X_PCLK_I2C5               129
#define AX650X_PCLK_I2C4               130
#define AX650X_PCLK_I2C3               131
#define AX650X_PCLK_I2C2               132
#define AX650X_PCLK_I2C1               133
#define AX650X_PCLK_I2C14              134
#define AX650X_PCLK_I2C13              135
#define AX650X_PCLK_I2C12              136
#define AX650X_PCLK_I2C11              137
#define AX650X_PCLK_I2C10              138
#define AX650X_PCLK_I2C0               139
#define AX650X_PCLK_GPIO6              140
#define AX650X_PCLK_GPIO5              141
#define AX650X_PCLK_GPIO4              142
#define AX650X_PCLK_GPIO3              143
#define AX650X_PCLK_GPIO2              144
#define AX650X_PCLK_UART9              145
#define AX650X_PCLK_UART8              146
#define AX650X_PCLK_UART7              147
#define AX650X_PCLK_UART6              148
#define AX650X_PCLK_UART5              149
#define AX650X_PCLK_UART4              150
#define AX650X_PCLK_UART3              151
#define AX650X_PCLK_UART2              152
#define AX650X_PCLK_UART1              153
#define AX650X_PCLK_UART10             154
#define AX650X_PCLK_UART0              155
#define AX650X_PCLK_TMR5               156
#define AX650X_PCLK_TMR4               157
#define AX650X_PCLK_TMR3               158
#define AX650X_PCLK_TMR2               159
#define AX650X_PCLK_TMR1               160
#define AX650X_PCLK_TMR0               161
#define AX650X_PCLK_TDM_S1             162
#define AX650X_PCLK_TDM_S0             163
#define AX650X_PCLK_TDM_M1             164
#define AX650X_SCLK_TDM_M1_DIVN        165
#define AX650X_SCLK_TDM_M0_DIVN        166
#define AX650X_SCLK_I2S_M1_DIVN        167
#define AX650X_SCLK_I2S_M0_DIVN        168
#define AX650X_PERIPH_NR_CLKS          (AX650X_SCLK_I2S_M0_DIVN + 1)

/*pipe clk*/
#define AX650X_CLK_PP_RGMII_TX_SEL      0
#define AX650X_CLK_PIPE_GLB_SEL         1
#define AX650X_CLK_PIPE_BUS_SEL         2
#define AX650X_CLK1_USB_SUSPEND_SEL     3
#define AX650X_CLK0_USB_SUSPEND_SEL     4
#define AX650X_CCLK_TX_PP_SDIO_SEL      5
#define AX650X_AUX_CLK_PCIE_SEL         6
#define AX650X_TCLK_PP_SDIO             7
#define AX650X_CLK_USB_REF              8
#define AX650X_CLK_PP_RMII_RX           9
#define AX650X_CLK_PP_RMII_PHY          10
#define AX650X_CLK_PP_RGMII_TX          11
#define AX650X_CLK_PP_EMAC_PTP          12
#define AX650X_CLK1_USB_SUSPEND         13
#define AX650X_CLK0_USB_SUSPEND         14
#define AX650X_CCLK_TX_PP_SDIO          15
#define AX650X_BCLK_PP_SDIO             16
#define AX650X_AUX_CLK_PCIE             17
#define AX650X_TMCLK_PP_SDIO            18
#define AX650X_HCLK_PP_SDIO             19
#define AX650X_CQETMCLK_PP_SDIO         20
#define AX650X_CLK_USB1_REF             21
#define AX650X_CLK_USB0_REF             22
#define AX650X_CLK_DMA2                 23
#define AX650X_CLK_DMA0                 24
#define AX650X_BUS_CLK_USB1             25
#define AX650X_BUS_CLK_USB0             26
#define AX650X_AUX_CLK_PCIE1            27
#define AX650X_AUX_CLK_PCIE0            28
#define AX650X_ACLK_SATA                29
#define AX650X_ACLK_PP_SDIO             30
#define AX650X_ACLK_PP_EMAC             31
#define AX650X_ACLK_PCIE1               32
#define AX650X_ACLK_PCIE0               33
#define AX650X_CLK_PP_RMII_RX_DIVN      34
#define AX650X_CCLK_TX_PP_SDIO_DIVN     35
#define AX650X_AUX_CLK_PCIE_DIVN        36
#define AX650X_PIPE_NR_CLKS             (AX650X_AUX_CLK_PCIE_DIVN + 1)

/*pipe-phy clk*/
#define AX650X_USB1_REF_ALT_CLK     0
#define AX650X_USB0_REF_ALT_CLK     1
#define AX650X_PIPE_REF_ALT_CLK     2
#define AX650X_PCLK_USB1_PHY        3
#define AX650X_PCLK_USB0_PHY        4
#define AX650X_PCLK_PCIE3_PHY       5
#define AX650X_PCLK_PCIE2_PHY       6
#define AX650X_PCLK_PCIE1_PHY       7
#define AX650X_PCLK_PCIE0_PHY       8
#define AX650X_PIPE_PHY_NR_CLKS         (AX650X_PCLK_PCIE0_PHY + 1)

/*pllc clk*/

/*sec clk*/
#define AX650X_PCLK_SEC_WDT       0
#define AX650X_PCLK_SEC_UART      1
#define AX650X_PCLK_SEC_TMR1      2
#define AX650X_PCLK_SEC_TMR0      3
#define AX650X_PCLK_SEC_GPIO      4
#define AX650X_PCLK_SEC_CE        5
#define AX650X_PCLK_FIREWALL0     6
#define AX650X_CLK_SEC_WDT        7
#define AX650X_CLK_SEC_UART       8
#define AX650X_CLK_SEC_TMR1       9
#define AX650X_CLK_SEC_TMR0       10
#define AX650X_CLK_SEC_GPIO       11
#define AX650X_CLK_SEC_CE         12
#define AX650X_CLK_SEC_CE_CNT     13
#define AX650X_SEC_NR_CLKS        (AX650X_CLK_SEC_CE_CNT + 1)

/*sen clk*/
#define AX650X_CLK_SEN_SPI_SEL       0
#define AX650X_CLK_SEN_GLB_SEL       1
#define AX650X_CLK_SEN_SPI           2
#define AX650X_PCLK_SEN_SPI          3
#define AX650X_PCLK_RX3_PINMUX       4
#define AX650X_PCLK_RX2_PINMUX       5
#define AX650X_PCLK_RX1_PINMUX       6
#define AX650X_PCLK_RX0_PINMUX       7
#define AX650X_PCLK_DPHY_CSI_RX1     8
#define AX650X_PCLK_DPHY_CSI_RX0     9
#define AX650X_PCLK_CSI_RX7          10
#define AX650X_PCLK_CSI_RX6          11
#define AX650X_PCLK_CSI_RX5          12
#define AX650X_PCLK_CSI_RX4          13
#define AX650X_PCLK_CSI_RX3          14
#define AX650X_PCLK_CSI_RX2          15
#define AX650X_PCLK_CSI_RX1          16
#define AX650X_PCLK_CSI_RX0          17
#define AX650X_SEN_NR_CLKS           (AX650X_PCLK_CSI_RX0 + 1)

/*sen clk*/
#define AX650X_RXPHY_TXCLKESC        0
#define AX650X_PCLK_CDPHY_RX7        1
#define AX650X_PCLK_CDPHY_RX6        2
#define AX650X_PCLK_CDPHY_RX5        3
#define AX650X_PCLK_CDPHY_RX4        4
#define AX650X_PCLK_CDPHY_RX3        5
#define AX650X_PCLK_CDPHY_RX2        6
#define AX650X_PCLK_CDPHY_RX1        7
#define AX650X_PCLK_CDPHY_RX0        8
#define AX650X_CDPHY_RX7_CFG_CLK     9
#define AX650X_CDPHY_RX6_CFG_CLK     10
#define AX650X_CDPHY_RX5_CFG_CLK     11
#define AX650X_CDPHY_RX4_CFG_CLK     12
#define AX650X_CDPHY_RX3_CFG_CLK     13
#define AX650X_CDPHY_RX2_CFG_CLK     14
#define AX650X_CDPHY_RX1_CFG_CLK     15
#define AX650X_CDPHY_RX0_CFG_CLK     16
#define AX650X_SEN_PHY_NR_CLKS           (AX650X_CDPHY_RX0_CFG_CLK + 1)

/*vdec clk*/
#define AX650X_CLK_VDEC_BUS_SEL     0
#define AX650X_CLK_VDEC_ROSC        1
#define AX650X_CLK_VDEC1            2
#define AX650X_CLK_VDEC0            3
#define AX650X_VDEC_NR_CLKS         (AX650X_CLK_VDEC0 + 1)

/*vdsp clk*/
#define AX650X_CLK_VDSP_GLB_SEL     0
#define AX650X_CLK_VDSP_BUS_SEL     1
#define AX650X_VDSP_NR_CLKS         (AX650X_CLK_VDSP_BUS_SEL + 1)

/*venc clk*/
#define AX650X_CLK_VENC_BUS_SEL      0
#define AX650X_CLK_VENC1_BUS_SEL     1
#define AX650X_CLK_VENC0_BUS_SEL     2
#define AX650X_CLK_VENC_ROSC         3
#define AX650X_CLK_VENC1             4
#define AX650X_CLK_VENC0             5
#define AX650X_VENC_NR_CLKS          (AX650X_CLK_VENC0 + 1)

/*pllc clk*/
#define AX650X_CLK_VPLL0    0
#define AX650X_CLK_VPLL1    1
#define AX650X_CLK_NFPLL    2

#endif
