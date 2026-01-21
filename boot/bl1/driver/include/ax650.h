/*
 * AXERA AX630X
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef __AX650_H__
#define __AX650_H__

#define GENERIC_TIMER_BASE         0x00140000
#define FLASH_CLK_RST_BASE         0x00110000

#define BOOT_MODE_INFO_ADDR        0x14000000	//env from last stage
#define SD_UPDATE_STATUS_ADDR      0x14000040	//sd update status
#define SD_UPDATE_FINISH           0xAA55BB66
#define SD_UPDATE_FAIL             0xCCDDEEFF

#define USB_UPDATE_STATUS_ADDR      0x14000048	//usb update status
#define USB_UPDATE_FINISH           0xAA55BB66
#define USB_UPDATE_FAIL             0xCCDDEEFF

#define CPU_CK_RST_CFG               0x4516000
#define CPU_CK_RST_CFG_CLK_MUX0      (CPU_CK_RST_CFG + 0x0)
#define CPU_CK_RST_CFG_CLK_MUX0_SET  (CPU_CK_RST_CFG + 0x10)

#define PMU_BASE            0x4C00000
#define PMU_WAKEUP          0x8
#define PMU_WAKEUP1         0xC
#define PMU_WAKEUP_SET      0xEC
#define PMU_WAKEUP_CLR      0xF0
#define PMU_WAKEUP1_SET     0xF4
#define PMU_WAKEUP1_CLR     0xF8

#define AP_GLB_REG_BASE            0x00208000
#define AP_GLB_CLK_EB2_ADDR        (AP_GLB_REG_BASE + 0x34)
#define AP_GLB_MUX_SEL3_ADDR       (AP_GLB_REG_BASE + 0x14)
#define AP_GLB_WDT0_CLK_EB         BIT(27)
#define AP_GLB_CLK_WDT0_SEL_24M    BIT(23)
#define COMM_CK_RST_CFG               0x4210000	//aon_clk_rst
#define COMM_CK_RST_CFG_CLK_MUX0_SET  (COMM_CK_RST_CFG + 0x28)
#define COMM_CK_RST_CFG_CLK_MUX0_CLR  (COMM_CK_RST_CFG + 0x2C)
#define COMM_CK_RST_CFG_CLK_MUX1_SET  (COMM_CK_RST_CFG + 0x30)
#define COMM_CK_RST_CFG_CLK_MUX1_CLR  (COMM_CK_RST_CFG + 0x34)
#define COMM_CK_RST_CFG_CLK_EB1_SET   (COMM_CK_RST_CFG + 0x40)
#define COMM_CK_RST_CFG_CLK_EB1_CLR   (COMM_CK_RST_CFG + 0x44)

#define INIT_CLK_400K				(400000)
#define INIT_CLK_300K				(300000)
#define INIT_CLK_200K				(200000)
#define INIT_CLK_100K				(100000)
#define DEFAULT_SD_CLK				(12000000)
#define DEFAULT_EMMC_CLK 			(12000000)
#define LEGACY_EMMC_CLK 			(25000000)
#define HS_EMMC_CLK 			 	(50000000)
#define HS_SD_CLK 			 	 	(50000000)

#define CLK_EMMC_CLK_CARD_SEL(x)    (((x) & 0x3) << 0)
#define CLK_EMMC_CLK_CARD_DIV(x)    (((x) & 0x1F) << 0)
#define EMMC_CLK_DIV_UPDATE         (0x1 << 5)

#define CLK_SDIO_CLK_CARD_SEL(x)    (((x) & 0x3) << 2)
#define CLK_SDIO_CLK_CARD_DIV(x)    (((x) & 0x1F) << 5)
#define SDIO_CLK_DIV_UPDATE         (0x1 << 10)

#define BIT_DWC_MSHC_CLK_CTRL_CLK_GEN_SELECT          BIT(5)
#define BIT_DWC_MSHC_CLK_CTRL_PLL_ENABLE              BIT(3)
#define BIT_DWC_MSHC_CLK_CTRL_SD_CLK_EN               BIT(2)
#define BIT_DWC_MSHC_CLK_CTRL_INTERNAL_CLK_STABLE     BIT(1)
#define BIT_DWC_MSHC_CLK_CTRL_INTERNAL_CLK_EN         BIT(0)

#define FLASH_SYS_GLB		0x100000
#define FLASH_SYS_CLK_RST_BASE 0x110000
#define FLASH_SYS_CLK_RST_CLK_MUX0_SET (FLASH_SYS_CLK_RST_BASE + 0x18)
#define FLASH_SYS_CLK_RST_CLK_MUX0_CLR (FLASH_SYS_CLK_RST_BASE + 0x1C)
#define FLASH_SYS_CLK_RST_CLK_EB_0_SET (FLASH_SYS_CLK_RST_BASE + 0x20)
#define FLASH_SYS_CLK_RST_CLK_EB_0_CLR (FLASH_SYS_CLK_RST_BASE + 0x24)
#define FLASH_SYS_CLK_RST_CLK_DIV_0_SET (FLASH_SYS_CLK_RST_BASE + 0x30)
#define FLASH_SYS_CLK_RST_CLK_DIV_0_CLR (FLASH_SYS_CLK_RST_BASE + 0x34)

#define PIPE_SYS_GLB                                           0x30000000
#define PIPE_SYS_GLB_SATA0_ADDR                                (PIPE_SYS_GLB + 0xA4)
#define PIPE_SYS_GLB_SATA1_ADDR                                (PIPE_SYS_GLB + 0xA8)
#define PIPE_SYS_GLB_SATA2_ADDR                                (PIPE_SYS_GLB + 0xAC)
#define PIPE_SYS_GLB_SATA3_ADDR                                (PIPE_SYS_GLB + 0xB0)
#define PIPE_SYS_GLB_SATA_SET_ADDR                             (PIPE_SYS_GLB + 0x10A0)
#define PIPE_SYS_GLB_SATA0_SET_ADDR                            (PIPE_SYS_GLB + 0x10A4)
#define PIPE_SYS_GLB_SATA1_SET_ADDR                            (PIPE_SYS_GLB + 0x10A8)
#define PIPE_SYS_GLB_SATA2_SET_ADDR                            (PIPE_SYS_GLB + 0x10AC)
#define PIPE_SYS_GLB_SATA3_SET_ADDR                            (PIPE_SYS_GLB + 0x10B0)
#define PIPE_SYS_GLB_PCIE_CTL0_0_SET                           (PIPE_SYS_GLB + 0x10C0)
#define PIPE_SYS_GLB_PCIE_CTL0_4_SET                           (PIPE_SYS_GLB + 0x10D0)
#define PIPE_SYS_GLB_PCIE_CTL0_0_CLR                           (PIPE_SYS_GLB + 0x20C0)
#define PIPE_SYS_GLB_PCIE_CTL0_4_CLR                           (PIPE_SYS_GLB + 0x20D0)
#define PIPE_SYS_GLB_PCIE_CTL0_16_SET                          (PIPE_SYS_GLB + 0x1100)
#define PIPE_SYS_GLB_PCIE_CTL0_16_CLR                          (PIPE_SYS_GLB + 0x2100)
#define PIPE_SYS_GLB_PCIE_CTL0_19_SET_ADDR                     (PIPE_SYS_GLB + 0x110C)
#define PIPE_SYS_GLB_PCIE_CTL0_19_CLR_ADDR                     (PIPE_SYS_GLB + 0x210C)
#define PIPE_SYS_GLB_PCIE_CTL1_19_SET_ADDR                     (PIPE_SYS_GLB + 0x1194)
#define PIPE_SYS_GLB_APP_LTSSM_ENABLE_SET_LSB                  4
#define PIPE_SYS_GLB_DEVICE_TYPE_CLR_LSB                       0
#define PIPE_SYS_GLB_SLAVE_MISC_INFO_LSB                       21
#define PIPE_SYS_GLB_SATA0_P0_PHY_SPDMODE_LSB                  2
#define PIPE_SYS_GLB_SATA1_P0_PHY_SPDMODE_LSB                  2
#define PIPE_SYS_GLB_SATA2_P0_PHY_SPDMODE_LSB                  2
#define PIPE_SYS_GLB_SATA3_P0_PHY_SPDMODE_LSB                  2
#define PIPE_SYS_GLB_SATA_SET_PMA_CLK_SEL_SET_LSB              1
#define PIPE_SYS_GLB_SATA_SET_PMA_RESET_SEL_SET_LSB            3
#define PIPE_SYS_GLB_SATA0_SET_LED_OE_SET_MSB                  9
#define PIPE_SYS_GLB_SATA1_SET_LED_OE_SET_MSB                  9
#define PIPE_SYS_GLB_SATA2_SET_LED_OE_SET_MSB                  9
#define PIPE_SYS_GLB_SATA3_SET_LED_OE_SET_MSB                  9
#define PIPE_SYS_GLB_PCIE_CTL0_19_SET_BUTTON_RST_N_SET_LSB     14
#define PIPE_SYS_GLB_PCIE_CTL1_19_SET_BUTTON_RST_N_SET_LSB     14
#define PIPE_SYS_GLB_PCIE_CTL0_19_SET_POWER_UP_RST_N_SET_LSB   15
#define PIPE_SYS_GLB_PCIE_CTL1_19_SET_POWER_UP_RST_N_SET_LSB   15

#define PIPE_SYS_CLK_BASE                                      0x30010000
#define PIPE_SYS_CLK_MUX_0                                     (PIPE_SYS_CLK_BASE + 0x0)
#define PIPE_SYS_CLK_MUX_0_SET                                 (PIPE_SYS_CLK_BASE + 0x18)
#define PIPE_SYS_CLK_MUX_0_CLR                                 (PIPE_SYS_CLK_BASE + 0x1C)
#define PIPE_SYS_CLK_EB0_SET                                   (PIPE_SYS_CLK_BASE + 0x20)
#define PIPE_SYS_CLK_EB0_CLR                                   (PIPE_SYS_CLK_BASE + 0x24)
#define PIPE_SYS_CLK_EB1_SET                                   (PIPE_SYS_CLK_BASE + 0x28)
#define PIPE_SYS_CLK_EB1_CLR                                   (PIPE_SYS_CLK_BASE + 0x2C)
#define PIPE_SYS_CLK_DIV_0_SET                                 (PIPE_SYS_CLK_BASE + 0x30)
#define PIPE_SYS_CLK_DIV_0_CLR                                 (PIPE_SYS_CLK_BASE + 0x34)
#define PIPE_SYS_SW_RESET_SET                                  (PIPE_SYS_CLK_BASE + 0x38)
#define PIPE_SYS_SW_RESET_CLR                                  (PIPE_SYS_CLK_BASE + 0x3C)
#define PIPE_SYS_CLK_CLK_PIPE_BUS_SEL_LSB                      6
#define PIPE_SYS_CLK_EB1_ACLK_SATA_EB                          4
#define PIPE_SYS_CLK_SW_RESET_PCIE0_POWER_UP_SW_RST            3
#define PIPE_SYS_CLK_SW_RESET_PCIE0_SW_PERST                   4
#define PIPE_SYS_CLK_SW_RESET_PCIE1_POWER_UP_SW_RST            5
#define PIPE_SYS_CLK_SW_RESET_PCIE1_SW_PERST                   6
#define PIPE_SYS_CLK_SW_RESET_PCIE2_POWER_UP_SW_RST            7
#define PIPE_SYS_CLK_SW_RESET_PCIE2_SW_PERST                   8
#define PIPE_SYS_CLK_SW_RESET_PCIE3_POWER_UP_SW_RST            9
#define PIPE_SYS_CLK_SW_RESET_PCIE3_SW_PERST                   10
#define PIPE_SYS_CLK_SW_RESET_SATA_SW_RST                      11

#define PIPE_PCIE0_PHY                                         0x30300000
#define PIPE_PCIE1_PHY                                         0x30400000
#define PIPE_PCIE2_PHY                                         0x30500000
#define PIPE_PCIE3_PHY                                         0x30600000
#define LANEN_DIG_TX_OVRD_IN_HI_OFFS                           (0x1001 * 4)
#define LANEN_DIG_TX_OVRD_IN_HI_TX_VBOOST_EN_LSB               8
#define LANEN_DIG_TX_OVRD_IN_HI_TX_VBOOST_EN_OVRD_LSB          9
//#define LANEN_DIG_TX_ASIC_IN_OFFS                              (0x1008 * 4)
//#define LANEN_DIG_TX_ASIC_IN_TX_VBOOST_EN_LSB                  11

#define PIPEPHY_GLB                                            0x30700000
#define PIPE_PHY_GLB_PCS_CTL0_0_SET_ADDR                       (PIPEPHY_GLB + 0x1010)
#define PIPE_PHY_GLB_PCS_CTL0_2_SET_ADDR                       (PIPEPHY_GLB + 0x1018)
#define PIPE_PHY_GLB_PCS_CTL0_3_SET_ADDR                       (PIPEPHY_GLB + 0x101C)
#define PIPE_PHY_GLB_PCS_CTL0_4_SET_ADDR                       (PIPEPHY_GLB + 0x1020)
#define PIPE_PHY_GLB_PCS_CTL1_0_SET_ADDR                       (PIPEPHY_GLB + 0x1034)
#define PIPE_PHY_GLB_PCS_CTL1_2_SET_ADDR                       (PIPEPHY_GLB + 0x103C)
#define PIPE_PHY_GLB_PCS_CTL1_3_SET_ADDR                       (PIPEPHY_GLB + 0x1040)
#define PIPE_PHY_GLB_PCS_CTL1_4_SET_ADDR                       (PIPEPHY_GLB + 0x1044)
#define PIPE_PHY_GLB_PCS_CTL0_2_CLR_ADDR                       (PIPEPHY_GLB + 0x2018)
#define PIPE_PHY_GLB_PCS_CTL0_3_CLR_ADDR                       (PIPEPHY_GLB + 0x201C)
#define PIPE_PHY_GLB_PCS_CTL1_2_CLR_ADDR                       (PIPEPHY_GLB + 0x203C)
#define PIPE_PHY_GLB_PCS_CTL1_3_CLR_ADDR                       (PIPEPHY_GLB + 0x2040)
#define PIPE_PHY_GLB_PCS_CTL0_0_ADDR                           (PIPEPHY_GLB + 0x10)
#define PIPE_PHY_GLB_PCS_CTL0_2_PHY0_CSR_TEST_POWERDOWN_LSB    4
#define PIPE_PHY_GLB_PCS_CTL0_3_PHY1_CSR_TEST_POWERDOWN_LSB    4
#define PIPE_PHY_GLB_PCS_CTL1_2_PHY0_CSR_TEST_POWERDOWN_LSB    4
#define PIPE_PHY_GLB_PCS_CTL1_3_PHY1_CSR_TEST_POWERDOWN_LSB    4
#define PIPE_PHY_GLB_PCS_CTL0_2_PHY0_CSR_REF_USE_PAD_LSB       18
#define PIPE_PHY_GLB_PCS_CTL0_3_PHY1_CSR_REF_USE_PAD_LSB       18
#define PIPE_PHY_GLB_PCS_CTL1_2_PHY0_CSR_REF_USE_PAD_LSB       18
#define PIPE_PHY_GLB_PCS_CTL1_3_PHY1_CSR_REF_USE_PAD_LSB       18
#define PIPE_PHY_GLB_PCS_CTL0_2_CLR_PHY0_CSR_PHY_RESET_CLR_LSB 20
#define PIPE_PHY_GLB_PCS_CTL0_3_CLR_PHY1_CSR_PHY_RESET_CLR_LSB 20
#define PIPE_PHY_GLB_PCS_CTL1_2_CLR_PHY0_CSR_PHY_RESET_CLR_LSB 20
#define PIPE_PHY_GLB_PCS_CTL1_3_CLR_PHY1_CSR_PHY_RESET_CLR_LSB 20
#define PIPE_PHY_GLB_PCS_CTL0_2_PHY0_CSR_REF_SSP_EN_LSB        24
#define PIPE_PHY_GLB_PCS_CTL0_3_PHY1_CSR_REF_SSP_EN_LSB        24
#define PIPE_PHY_GLB_PCS_CTL1_2_PHY0_CSR_REF_SSP_EN_LSB        24
#define PIPE_PHY_GLB_PCS_CTL1_3_PHY1_CSR_REF_SSP_EN_LSB        24
#define PIPE_PHY_GLB_PCS_CTL0_4_PIPE0_RESET_N_LSB              0
#define PIPE_PHY_GLB_PCS_CTL1_4_PIPE0_RESET_N_LSB              0

#define PIPEPHY_CK_RST_BASE                                    0x30710000
#define PIPE_PHY_CK_RST_CFG_CLK_EB_0_CLR_ADDR                  (PIPEPHY_CK_RST_BASE + 0x14)
#define PIPE_PHY_CK_RST_CFG_SW_RST_0_CLR_ADDR                  (PIPEPHY_CK_RST_BASE + 0x24)
#define PIPE_PHY_CK_RST_CFG_CLK_EB_0_PIPE_REF_ALT_CLK_EB_LSB   0
#define PIPE_PHY_CK_RST_CFG_SW_RST_0_PCIE0_PHY_SW_PRST_LSB     0
#define PIPE_PHY_CK_RST_CFG_SW_RST_0_PCIE1_PHY_SW_PRST_LSB     1
#define PIPE_PHY_CK_RST_CFG_SW_RST_0_PCIE2_PHY_SW_PRST_LSB     2
#define PIPE_PHY_CK_RST_CFG_SW_RST_0_PCIE3_PHY_SW_PRST_LSB     3

#define COMM_SYS_GLB	    	         0x4200000
#define COMM_ABORT_CFG                  (COMM_SYS_GLB + 0x4C)
#define COMM_ABORT_STATUS               (COMM_SYS_GLB + 0x58)
#define COMM_SYS_DMA_FLASH_DW_SEL0      (COMM_SYS_GLB + 0x3d4)
#define COMM_SYS_DMA_FLASH_DW_SEL1      (COMM_SYS_GLB + 0x3d8)
#define CHIP_RST_SW                     BIT(0)

#define SEN_CK_RST_CFG                  (0x1300C000)
#define SEN_CK_RST_CFG_CLK_MUX_0_SET    (SEN_CK_RST_CFG + 0x10)
#define SEN_CK_RST_CFG_CLK_MUX_0_CLR    (SEN_CK_RST_CFG + 0x14)
#define SEN_CK_RST_CFG_CLK_EB_1_SET     (SEN_CK_RST_CFG + 0x20)
#define SEN_CK_RST_CFG_CLK_SW_RST_0_CLR (SEN_CK_RST_CFG + 0x2C)

#define SENPHY_GLB                      (0x13D80000)
#define SEN_PHY_GLB_CSI_CTRL_EN_CLR     (SENPHY_GLB + 0x1C0)

#define SEN_PHY_CK_RST_CFG              (0x13D84000)
#define SEN_PHY_CK_RST_CFG_CLK_EB_1_SET (SEN_PHY_CK_RST_CFG + 0x14)
#define SEN_PHY_CK_RST_CFG_SW_RST_0_CLR (SEN_PHY_CK_RST_CFG + 0x20)

#define NPU_CK_RST_BASE         (0x16550000)
#define NPU_CK_RST_CLK_EB0      (NPU_CK_RST_BASE + 0x4)
#define NPU_CK_RST_CLK_EB1      (NPU_CK_RST_BASE + 0x8)
#define NPU_CK_RST_CLK_RST      (NPU_CK_RST_BASE + 0xC)
#define NPU_CK_RST_CLK_EB0_SET  (NPU_CK_RST_BASE + 0x28)
#define NPU_CK_RST_CLK_EB0_CLR  (NPU_CK_RST_BASE + 0x2C)

#define PERI_CLK_RST_CFG                (0x2002000)
#define PERI_CLK_RST_CFG_MUX0           (PERI_CLK_RST_CFG + 0x0)
#define PERI_CLK_RST_CFG_MUX0_SET       (PERI_CLK_RST_CFG + 0x38)
#define PERI_CLK_RST_CFG_MUX0_CLR       (PERI_CLK_RST_CFG + 0x3C)
#define PERI_CLK_RST_CFG_EB0_SET        (PERI_CLK_RST_CFG + 0x40)
#define PERI_CLK_RST_CFG_EB1_SET        (PERI_CLK_RST_CFG + 0x48)
#define PERI_CLK_RST_CFG_EB2_SET        (PERI_CLK_RST_CFG + 0x50)
#define PERI_CLK_RST_CFG_EB4_SET        (PERI_CLK_RST_CFG + 0x60)
#define PERI_CLK_RST_CFG_EB5_SET        (PERI_CLK_RST_CFG + 0x68)
#define PERI_CLK_RST_CFG_SW_RST0_CLR    (PERI_CLK_RST_CFG + 0x7c)
#define PERI_CLK_RST_CFG_SW_RST3_CLR    (PERI_CLK_RST_CFG + 0x94)

#define ABORT_WDT4_ALARM BIT(4)
#define ABORT_WDT0_ALARM BIT(3)
#define ABORT_THM_ALARM BIT(1)
#define ABORT_SWRST_ALARM BIT(0)
#define ABORT_WDT4_EN BIT(9)
#define ABORT_WDT4_CLR BIT(8)
#define ABORT_THM_EN BIT(7)
#define ABORT_WDT0_EN BIT(6)
#define ABORT_WDT0_CLR BIT(4)
#define ABORT_THM_CLR BIT(2)
#define ABORT_SWRST_CLR BIT(1)

#define WDT0_BASE                  0x04E05000
#define WDT0_TORR_ADDR             (WDT0_BASE + 0xc)
#define WDT0_TORR_START_ADDR       (WDT0_BASE + 0x18)
#define WDT0_CLK_FREQ              (24000000)

/* SFC pin config */
#define BASE_ADDR_SFC_PIN 						0x05030000
#define OFFSET_SFC_CLK							0x04
#define OFFSET_SFC_MOSI_IO0						0x08
#define OFFSET_SFC_MISO_IO1						0x0C
#define OFFSET_SFC_WP_IO2						0x10
#define OFFSET_SFC_HOLD_IO3						0x14
#define OFFSET_SFC_CSN0							0x18
#define OFFSET_SFC_CSN1							0x1C

/* spi pad select reg */
#define PINMUX_G5_BASE							(0x4250600)
#define PINMUX_G5_MISC_SET						(PINMUX_G5_BASE + 0x8C)

/* i2c pad select reg */
#define I2C_PAD_SELECT_G5_MISC					0x4250688

enum platform_type {
	AX650_HAPS = 0,
	AX650_EMMC,
	AX650_NAND,
	PLATFORM_TYPE_NUM,
};

int get_board_id(void);
unsigned char get_chip_type_id(void);
int setup_boot_mode(void);
void wdt0_enable(u8 enable);
#endif
