#include <common.h>
#include <dm.h>
#include <dm/pinctrl.h>
#include <dm/uclass-internal.h>
#include <power/regulator.h>
#include <asm/arch/ax650.h>
#include <dm/device-internal.h>
#include <nand.h>
#include <asm/io.h>
#include <asm-generic/gpio.h>
#include <pwm.h>
#include <linux/delay.h>
#include "common/pwm_common.h"
#include <asm/arch/pll_config.h>
#ifdef CONFIG_TARGET_AX650_EMMC
#include <configs/ax650_emmc.h>
#endif
#ifdef CONFIG_TARGET_AX650_XPILOT_6V
#include <configs/ax650_xpilot_6v.h>
#endif
#ifdef CONFIG_TARGET_AX650_NAND
#include <configs/ax650_nand.h>
#endif
#ifdef CONFIG_TARGET_AX650_SSD
#include <configs/ax650_ssd.h>
#endif
#ifdef CONFIG_TARGET_AX650_LP
#include <configs/ax650_lp.h>
#endif
#include <asm/arch/boot_mode.h>
#include "ax_efuse_api.h"

extern void wdt0_enable(bool enable);
extern int pinmux_init(void);
extern void dphyrx_pin_reg_config(void);
extern void ddr_init(void);
extern void early_print_init(void);
extern int early_display_options(void);
#ifdef CONFIG_AXERA_MEMORY_FIREWALL
extern int firewall_init(ulong optee_mem_start, ulong optee_mem_end);
#endif
#define AON_SYS__PLL__BASE_ADDR 0x4E10000
#define CHIP_BOND_BLK	9

static int cpu_clk_configed __attribute__((section(".data"))) = 0;
static int cpu_clk_set __attribute__((section(".data"))) = CPU_CLK_SET;

/* To get the ID of the chip type. */
static unsigned char hanming_decode_74(unsigned char  a)
{
	unsigned char b=0,c=0;
	b = a;
	c = 0;
	if(b&0x40) {
		c = c^0x07 ;
	}
	if(b&0x20) {
		c = c^0x06 ;
	}
	if(b&0x10) {
		c = c^0x05 ;
	}
	if(b&0x08) {
		c = c^0x03 ;
	}
	if(b&0x04) {
		c = c^0x04 ;
	}
	if(b&0x02) {
		c = c^0x02 ;
	}
	if(b&0x01) {
		c = c^0x01 ;
	}
	switch(c) {
	case 0 :
		b = b >> 3 ;
		break ;
	case 1 :
		b = b >> 3 ;
		break ;
	case 2 :
		b = b >> 3 ;
		break ;
	case 3 :
		b = (b^0x08) >> 3 ;
		break ;
	case 4 :
		b = b >> 3 ;
		break ;
	case 5 :
		b = (b^0x10) >> 3 ;
		break ;
	case 6 :
		b = (b^0x20) >> 3 ;
		break ;
	case 7 :
		b = (b^0x40) >> 3 ;
		break ;
	}
	return (b & 0x0F);
}

static unsigned char get_chip_value_from_efuse(void)
{
	int val;
	int ret;
	ret = ax_efuse_init();
	if(ret != EFUSE_SUCCESS) {
		return -1;
	}
	ret = ax_efuse_read(CHIP_BOND_BLK, &val);
	if(ret != EFUSE_SUCCESS) {
		return -1;
	}
	val = val >> 24;
	if((val & 0xff) == 0) {
		printf("In the early chips, there was no write value\n");
		return 0;
	}
	ax_efuse_deinit();
	return (val & 0xff);
}

unsigned char get_chip_type_id(void)
{
	char value;
	value = get_chip_value_from_efuse();
	return hanming_decode_74(value);
}

static void core_clk_set(int clk)
{
	/* Using cpll0 , set cpll0 */
	u32 rdata;
	rdata = readl(CPU_CK_RST_CFG + 0x0);
	rdata = rdata & 0xfe000001;
	rdata = rdata | (0x249249 << 1); // core0-7 switch to epll750m
	writel(rdata, CPU_CK_RST_CFG + 0x0);

	// enable pll pclk & release rst
	rdata = readl(COMM_CK_RST_CFG + 0xc);
	rdata = rdata | (0x1 << 29);
	writel(rdata, COMM_CK_RST_CFG + 0xc);
	rdata = readl(COMM_CK_RST_CFG + 0x1c);
	rdata = rdata & 0xfffff7ff;
	writel(rdata, COMM_CK_RST_CFG + 0x1c);
	if (clk > 1100)
		pll_set(AX_CPLL0, clk);
	else
		pll_set(AX_CPLL0, 1100);
	// core0-7 switch to cpll0 back
	rdata = readl(CPU_CK_RST_CFG + 0x0);
	rdata = rdata & 0xfe000001;
	rdata = rdata | (0x924924 << 1);
	writel(rdata, CPU_CK_RST_CFG + 0x0);
}

static void dsu_clk_set(int clk)
{
	/* Using cpll1 , set cpll1 */
	u32 rdata;
	// dsu clk switch to gpll24m
	rdata = readl(CPU_CK_RST_CFG + 0x0);
	rdata = rdata & 0xf3ffffff;
	rdata = rdata | (0x1 << 26); // dsu clk switch to epll750m
	writel(rdata, CPU_CK_RST_CFG + 0x0);

	// enable pll pclk & release rst
	rdata = readl(COMM_CK_RST_CFG + 0xc);
	rdata = rdata | (0x1 << 29);
	writel(rdata, COMM_CK_RST_CFG + 0xc);
	rdata = readl(COMM_CK_RST_CFG + 0x1c);
	rdata = rdata & 0xfffff7ff;
	writel(rdata, COMM_CK_RST_CFG + 0x1c);

	if (clk > 770)
		pll_set(AX_CPLL1, clk);
	else
		pll_set(AX_CPLL1, 770);
	// dsu switch to cpll1 back
	rdata = readl(CPU_CK_RST_CFG + 0x0);
	rdata = rdata & 0xf3ffffff;
	rdata = rdata | (0x2 << 26);
	writel(rdata, CPU_CK_RST_CFG + 0x0);
}

int clk_config(void)
{
	int dsu_clk;
	if (cpu_clk_configed) {
		return 0;
	}
#ifdef CFG_CPU_CLK_CMDLINE
	int cpu_clk = CPU_CLK;
	if (cpu_clk > 1100) {
		core_clk_set(cpu_clk);
		dsu_clk = cpu_clk / 2 + cpu_clk / 5; //70%
		dsu_clk_set(dsu_clk);
	} else {
		core_clk_set(1100);
		dsu_clk_set(770);
	}
#else
	/* If CPU_CLK_SET > 1500, AX650A/AX650C CPU CLK set to 1.5GHZ. */
	if ((get_chip_type_id() == AX650A_CHIP) || (get_chip_type_id() == AX650C_CHIP) && (cpu_clk_set > 1500)) {
		cpu_clk_set = 1500;
	}
	core_clk_set(cpu_clk_set);
	dsu_clk = cpu_clk_set / 2 + cpu_clk_set / 5; //70%
	dsu_clk_set(dsu_clk);
#endif
	/* DUMMY_GRP0_REG1_ADDR = AON_SYS__PLL__BASE_ADDR + 0x100 , cpu_08v_pll_sel(DUMMY_GRP0_REG1[0]) = 1 */
	writel(BIT(0), AON_SYS__PLL__BASE_ADDR + 0x100);

	/* iram(pclk_top) to 200M */
	writel(BIT(13), COMM_CK_RST_CFG_CLK_MUX1_SET);
	/* switch flash_sys glb to 350M */
	writel(BIT(5), FLASH_SYS_CLK_RST_CLK_MUX0_CLR);
	writel(BIT(6), FLASH_SYS_CLK_RST_CLK_MUX0_SET);
	/* switch flash_sys clk_flash_ser_sel to 200M */
	writel(BIT(7), FLASH_SYS_CLK_RST_CLK_MUX0_CLR);
	writel(BIT(8), FLASH_SYS_CLK_RST_CLK_MUX0_SET);
	/* switch pipe_sys glb to 100M, clk_pipe_bus_sel to 500M*/
	writel(BIT(6) | BIT(7) | BIT(8), PIPE_SYS_CLK_MUX_0_SET);
	/* aclk_nn to 1600M, aclk_top_sel to 800M */
	writel(GENMASK(5, 0), COMM_CK_RST_CFG_CLK_MUX0_CLR);
	writel(BIT(2) | BIT(5), COMM_CK_RST_CFG_CLK_MUX0_SET);
	/* clk_periph_glb mux select:350M, clk_periph_gpio_sel:gpll_24m
	 * clk_periph_ser_sel:npll_200m
	 * set clk_periph_tmr0 and clk_periph_tmr1 mux select [7:6] to 24M in pwm driver;
	 */
	writel(GENMASK(2, 0), PERI_CLK_RST_CFG_MUX0_SET);
	writel(BIT(5), PERI_CLK_RST_CFG_MUX0_SET);
	/* tmr64 clk set to 24M, set clk eb */
	writel(BIT(12), COMM_CK_RST_CFG_CLK_MUX0_SET);
	writel(BIT(17), COMM_CK_RST_CFG_CLK_EB1_SET);

	/*aclk_top1/2/3_sel*/
	writel(GENMASK(23, 15), COMM_CK_RST_CFG_CLK_MUX1_CLR);
	writel(BIT(17) | BIT(20) | BIT(23), COMM_CK_RST_CFG_CLK_MUX1_SET);

#ifdef CDPHY_PIN_FUNC_MULTIPLEX
	/*pclk_cdphy_rx_eb, cdphy_rx_cfg_clk_eb, cdphy_rx_sw_prst*/
	writel(GENMASK(15, 0), SEN_PHY_CK_RST_CFG_CLK_EB_1_SET);
	writel(GENMASK(7, 0), SEN_PHY_CK_RST_CFG_SW_RST_0_CLR);
#endif

	/*clk_sen_spi_sel, clk_sen_glb_sel*/
	writel(GENMASK(2, 1), SEN_CK_RST_CFG_CLK_MUX_0_CLR);
	writel(BIT(2) | BIT(0), SEN_CK_RST_CFG_CLK_MUX_0_SET);

#ifdef CDPHY_PIN_FUNC_MULTIPLEX
	/*pclk_rx_pinmux_eb, rx_pinmux_sw_prst*/
	writel(GENMASK(13, 10), SEN_CK_RST_CFG_CLK_EB_1_SET);
	writel(GENMASK(13, 10), SEN_CK_RST_CFG_CLK_SW_RST_0_CLR);
#endif

	cpu_clk_configed = 1;

	return 0;
}

void ocm_clk_config(void)
{
	//epll_750m
	writel(GENMASK(11, 9), NPU_CK_RST_CLK_EB0_CLR);
	writel(GENMASK(10, 9), NPU_CK_RST_CLK_EB0_SET);
}

static int thm_reset_enable(void)
{
	u32 temp;
	temp = readl(COMM_ABORT_CFG);
	temp |= (1 << 7);
	writel(temp, (u32)COMM_ABORT_CFG);
	return 0;
}

static void pwm_volt_config(enum platform_type type)
{
#ifdef CONFIG_PWM_AXERA

	/* PWM clock set */
	pwm_clk_config();
	/*
		VDDCORE settings, frequence is 240KHZ, duty is 50%, Vout is 0.8V,
		use PWM05 generate PWM signal.
	*/
	pwm_config(type, VDDCORE, 240, 80);
	udelay(500);
	/*
		VDDCPU settings, frequence is 240KHZ, duty is 50%, Vout is 0.8V
		use PWM02 generate PWM signal.
	*/
#ifdef CFG_CPU_CLK_CMDLINE
	int cpu_clk = CPU_CLK;
	if (cpu_clk > 1100)
		pwm_config(type, VDDCPU, 240, 100);
	else
		pwm_config(type, VDDCPU, 240, 80);
#else
	if (cpu_clk_set > 1100)
		pwm_config(type, VDDCPU, 240, 100);
	else
		pwm_config(type, VDDCPU, 240, 80);
#endif
	udelay(500);
	/*
		NPU settings ,frequence is 240KHZ, duty is 50%, Vout is 0.6V
		use PWM01 generate PWM signal.
	*/
	if (get_chip_type_id() == AX650A_CHIP)
		pwm_config(type, NNVDD, 240, 62);
	else
		pwm_config(type, NNVDD, 240, 60);
#endif
	mdelay(3);

}

/* function: pmu_init()
* CPU0-CPU , dspvdec venc1 venc0 ,N0-N8 WAKEUP
* sen, isp, cpu, npu, mm, perip, pipe, flash, ddr1 ,ddr0 WAKEUP
*/
static void pmu_init(void)
{
	/* cpu1-cpu7 not wakeup in bringup, after setting. */
	writel(GENMASK(4, 0), PMU_BASE + PMU_WAKEUP_SET);
	writel(GENMASK(18, 0), PMU_BASE + PMU_WAKEUP1_SET);

	/* The chip is required to come back through the interrupt status judgment, temporarily using delay */
	mdelay(5);
	writel(GENMASK(4, 0), PMU_BASE + PMU_WAKEUP_CLR);
	writel(GENMASK(18, 0), PMU_BASE + PMU_WAKEUP1_CLR);
}

int chip_init(enum platform_type type)
{
#ifdef CONFIG_SLAVE_PCIE_BOOT
	u32 dl_mode;
#endif
	early_display_options();
#if defined(OPTEE_BOOT) && defined(CONFIG_AXERA_MEMORY_FIREWALL)
	firewall_init(OPTEE_IMAGE_ADDR, OPTEE_IMAGE_ADDR + OPTEE_RESERVED_SIZE - OPTEE_SHMEM_SIZE - 1);
#endif
	timer_init();
	pmu_init();
	wdt0_enable(0);
#ifndef CONFIG_AXERA_ENV_BOARD_ID
	/* pinmux init */
	pinmux_init();
#endif
	pwm_volt_config(type);
	clk_config();
	ocm_clk_config();
	thm_reset_enable();
	early_print_init();
#ifdef CDPHY_PIN_FUNC_MULTIPLEX
	dphyrx_pin_reg_config();
#endif

#ifdef CONFIG_SLAVE_PCIE_BOOT
	dl_mode = readl(COMM_SYS_DUMMY_SW5);
	if (dl_mode == DL_CHAN_UART1 || dl_mode == DL_CHAN_UART0 || dl_mode == DL_CHAN_USB)
#endif
	ddr_init();
	return 0;
}
