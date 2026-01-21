#ifndef MC50_E907_INTERRUPT_H
#define MC50_E907_INTERRUPT_H

#include <core_rv32.h>
#include <rthw.h>
#include <stdbool.h>

struct MC50_RISCV_INT_REG
{
    __IOM uint32_t int_mask;
    __OM uint32_t int_mask_set;
    __OM uint32_t int_mask_clr;

    __IOM uint32_t int_clr;
    __OM uint32_t int_clr_set;
    __OM uint32_t int_clr_clr;

    __I uint32_t int_raw;
    __I uint32_t int_sta;

    __IOM uint32_t int_mask_riscv0;
    __OM uint32_t int_mask_riscv0_set;
    __OM uint32_t int_mask_riscv0_clr;
    __IOM uint32_t int_mask_riscv1;
    __OM uint32_t int_mask_riscv1_set;
    __OM uint32_t int_mask_riscv1_clr;
    __IOM uint32_t int_mask_riscv2;
    __OM uint32_t int_mask_riscv2_set;
    __OM uint32_t int_mask_riscv2_clr;
    __IOM uint32_t int_mask_riscv3;
    __OM uint32_t int_mask_riscv3_set;
    __OM uint32_t int_mask_riscv3_clr;

    __IOM uint32_t int_clr_riscv0;
    __OM uint32_t int_clr_riscv0_set;
    __OM uint32_t int_clr_riscv0_clr;
    __IOM uint32_t int_clr_riscv1;
    __OM uint32_t int_clr_riscv1_set;
    __OM uint32_t int_clr_riscv1_clr;
    __IOM uint32_t int_clr_riscv2;
    __OM uint32_t int_clr_riscv2_set;
    __OM uint32_t int_clr_riscv2_clr;
    __IOM uint32_t int_clr_riscv3;
    __OM uint32_t int_clr_riscv3_set;
    __OM uint32_t int_clr_riscv3_clr;

    __I uint32_t int_raw_riscv0;
    __I uint32_t int_raw_riscv1;
    __I uint32_t int_raw_riscv2;
    __I uint32_t int_raw_riscv3;

    __I uint32_t int_sta_riscv0;
    __I uint32_t int_sta_riscv1;
    __I uint32_t int_sta_riscv2;
    __I uint32_t int_sta_riscv3;

    __I uint32_t riscv_reset_status;
};

typedef enum MC50_IRQn {
    INT_REQ_RISCV_RSP_ERR                       =   0,
    INT_REQ_COMMON_MAIL4                        =   1,

    INT_REQ_COMMON_RTC0                         =   2,
    INT_REQ_COMMON_RTC1                         =   3,
    INT_REQ_COMMON_RTC2                         =   4,
    INT_REQ_COMMON_RTC3                         =   5,

    INT_REQ_COMMON_WDT3                         =   6,
    INT_REQ_NPU_SYNC_MANAGER_REQ_0              =   7,
    INT_REQ_COMMON_TMR64                        =   8,
    INT_REQ_COMMON_QPIO                         =   9,
    INT_REQ_COMMON_PCHN                         =   10,

    INT_REQ_REERVED0                            =   11,

    INT_REQ_COMMON_IRAM_W                       =   12,
    INT_REQ_COMMON_IRAM_R                       =   13,
    INT_REQ_DDR0_ddr                            =   14,
    INT_REQ_NPU_SYNC_MANAGER_REQ_1              =   15,
    INT_REQ_DDR1_ddr                            =   16,
    INT_REQ_NPU_SYNC_MANAGER_REQ_2              =   17,

    INT_REQ_FLASH_DW_DMAC                       =   18,

    INT_REQ_ISP_IFE2DSP                         =   19,
    INT_REQ_ISP_ITP2DSP                         =   20,
    INT_REQ_ISP_DISPC2DSP                       =   21,

    INT_REQ_MM_CORE_DSP                         =   22,
    INT_REQ_MM_DSI_TX                           =   23,
    INT_REQ_MM_CSI_TX                           =   24,
    INT_REQ_MM_HDMI0                            =   25,
    INT_REQ_MM_HDMI1                            =   26,
    INT_REQ_MM_JPEG_ENC                         =   27,
    INT_REQ_MM_JPEG_DEC                         =   28,

    INT_REQ_NPU_SYNC_MANAGER_REQ_3              =   29,
    INT_REQ_NPU_SYNC_MANAGER_REQ_ERR            =   30,

    INT_REQ_PERIPH_APB_SSI_M1                   =   31,
    INT_REQ_PERIPH_APB_SSI_M2                   =   32,
    INT_REQ_PERIPH_APB_SSI_M3                   =   33,
    INT_REQ_PERIPH_APB_SSI_S                    =   34,

    INT_REQ_PERIPH_I2C0                         =   35,
    INT_REQ_PERIPH_I2C1                         =   36,
    INT_REQ_PERIPH_I2C2                         =   37,
    INT_REQ_PERIPH_I2C3                         =   38,
    INT_REQ_PERIPH_I2C4                         =   39,
    INT_REQ_PERIPH_I2C5                         =   40,
    INT_REQ_PERIPH_I2C6                         =   41,
    INT_REQ_PERIPH_I2C7                         =   42,
    INT_REQ_PERIPH_I2C8                         =   43,
    INT_REQ_PERIPH_I2C9                         =   44,
    INT_REQ_PERIPH_I2C10                        =   45,
    INT_REQ_PERIPH_I2C11                        =   46,
    INT_REQ_PERIPH_I2C12                        =   47,
    INT_REQ_PERIPH_I2C13                        =   48,
    INT_REQ_PERIPH_I2C14                        =   49,

    INT_REQ_PERIPH_UART0                        =   50,
    INT_REQ_PERIPH_UART1                        =   51,
    INT_REQ_PERIPH_UART2                        =   52,
    INT_REQ_PERIPH_UART3                        =   53,
    INT_REQ_PERIPH_UART4                        =   54,
    INT_REQ_PERIPH_UART5                        =   55,
    INT_REQ_PERIPH_UART6                        =   56,
    INT_REQ_PERIPH_UART7                        =   57,
    INT_REQ_PERIPH_UART8                        =   58,
    INT_REQ_PERIPH_UART9                        =   59,
    INT_REQ_PERIPH_UART10                       =   60,

    INT_REQ_PERIPH_TMR0                         =   61,
    INT_REQ_PERIPH_TMR1                         =   62,
    INT_REQ_PERIPH_TMR2                         =   63,
    INT_REQ_PERIPH_TMR3                         =   64,
    INT_REQ_PERIPH_TMR4                         =   65,
    INT_REQ_PERIPH_TMR5                         =   66,

    INT_REQ_PERIPH_PWM0                         =   67,
    INT_REQ_PERIPH_PWM1                         =   68,
    INT_REQ_PERIPH_PWM2                         =   69,
    INT_REQ_PERIPH_PWM3                         =   70,

    INT_REQ_ISP_YUV2DSP                         =   71,

    INT_REQ_REERVED1                            =   72,

    INT_REQ_PERIPH_GPIO0                        =   73,
    INT_REQ_PERIPH_GPIO1                        =   74,
    INT_REQ_PERIPH_GPIO2                        =   75,
    INT_REQ_PERIPH_GPIO3                        =   76,
    INT_REQ_PERIPH_GPIO4                        =   77,
    INT_REQ_PERIPH_GPIO5                        =   78,
    INT_REQ_PERIPH_GPIO6                        =   79,

    INT_REQ_PERIPH_I2S_S0                       =   80,
    INT_REQ_PERIPH_I2S_S1                       =   81,
    INT_REQ_PERIPH_I2S_M0                       =   82,
    INT_REQ_PERIPH_I2S_M1                       =   83,
    INT_REQ_PERIPH_TDM_S0                       =   84,
    INT_REQ_PERIPH_TDM_S1                       =   85,
    INT_REQ_PERIPH_TDM_M0                       =   86,
    INT_REQ_PERIPH_TDM_M1                       =   87,

    INT_REQ_REERVED2                            =   88,

    INT_REQ_PIPE_DMAC0                          =   89,

    INT_REQ_REERVED3                            =   90,

    INT_REQ_PIPE_DMAC_XOR                       =   91,
    INT_REQ_PIPE_SATA                           =   92,
    INT_REQ_PIPE_PCIE0                          =   93,
    INT_REQ_PIPE_PCIE1                          =   94,

    INT_REQ_SEN_CSI2RX0                         =   95,
    INT_REQ_SEN_CSI2RX1                         =   96,
    INT_REQ_SEN_CSI2RX2                         =   97,
    INT_REQ_SEN_CSI2RX3                         =   98,
    INT_REQ_SEN_CSI2RX4                         =   99,
    INT_REQ_SEN_CSI2RX5                         =  100,
    INT_REQ_SEN_CSI2RX6                         =  101,
    INT_REQ_SEN_CSI2RX7                         =  102,
    INT_REQ_SEN_CSI2RX8                         =  103,
    INT_REQ_SEN_CSI2RX9                         =  104,

    INT_REQ_SEN_SPI_MASTER                      =  105,

    INT_REQ_VDEC_VC8000D_0                      =  106,
    INT_REQ_VDEC_VC8000D_1                      =  107,

    INT_REQ_VENC0_VC8000E                       =  108,
    INT_REQ_VENC1_VC8000E                       =  109,

    INT_REQ_MM_HDMI0_OINTERRUPTWAKEUP           =  110,
    INT_REQ_MM_HDMI1_OINTERRUPTWAKEUP           =  111,

    INT_REQ_MAX,
} MC50_RISCV_IRQn_Type;


#define RISCV_INT_REG_BASE (0x4200000UL+0x2D8)

rt_isr_handler_t mc50_e907_interrupt_install(int vector, rt_isr_handler_t handler,void *param, const char *name);
void mc50_e907_interrupt_umask(int vector);
void mc50_e907_interrupt_mask(int vector);
int mc50_e907_interrupt_get_mask(int vector, bool *mask_state);
#endif
