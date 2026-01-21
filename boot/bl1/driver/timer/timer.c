#include "timer.h"
#include "chip_reg.h"

/*
 * this function must be called only once in timer_init(),
 * and we will have 178s under 24Mhz before timer overflow.
 */
static void start_timer()
{
	u32 val;

	/* disable timer */
	val = readl(TIMER_CONTROL_REG);
	val &= ~TIMER_ENABLE;
	writel(val, TIMER_CONTROL_REG);

	/* set max loadcount(178s)*/
	writel(0xffffffff, TIMER_LOADCOUNT);

	/* start timer */
	val = readl(TIMER_CONTROL_REG);
	val |= TIMER_ENABLE;
	writel(val, TIMER_CONTROL_REG);
}

u32 getCurrTime(UNIT time_unit)
{
	u32 time;
	u32 div = 0;

	switch (time_unit) {
	case SEC:
		div = TICK2SEC;
		break;
	case MSEC:
		div = TICK2MSEC;
		break;
	case USEC:
		div = TICK2USEC;
		break;
	}
	time = readl(TIMER_CURRENT_VALUE) / div;

	return time;
}

void delay(u32 s)
{
	u32 start;
	start = getCurrTime(SEC);
	while ((start - getCurrTime(SEC)) <= s);
}

void mdelay(u32 ms)
{
	u32 start;
	start = getCurrTime(MSEC);
	while ((start - getCurrTime(MSEC)) <= ms);
}

void udelay(u32 us)
{
	u32 start;
	start = getCurrTime(USEC);
	while ((start - getCurrTime(USEC)) <= us);
}

void timer_init()
{
	u32 val;

	/* timer0 select 24Mhz */
	writel(BIT(6), PERI_CLK_RST_CFG_MUX0_SET);//0x2002038

	/* timer0 Global clock enable register */
	writel(BIT(2), PERI_CLK_RST_CFG_EB0_SET);

	/* timer0  clk_tmr01_eb Global clock gate enable register */
	writel(BIT(26), PERI_CLK_RST_CFG_EB2_SET);

	/* timer0  plk_tmr0_eb Global clock gate enable register */
	writel(BIT(3), PERI_CLK_RST_CFG_EB5_SET);

	/* timer0 tmr0_sw_rst tmr01_sw_rst Software control reset register */
	writel(BIT(8) | BIT(12), PERI_CLK_RST_CFG_SW_RST3_CLR);

	/* disable timer int and set user_mode */
	val = TIMER_INT_MASK | USER_MODE;
	writel(val, TIMER_CONTROL_REG);

	start_timer();
}
