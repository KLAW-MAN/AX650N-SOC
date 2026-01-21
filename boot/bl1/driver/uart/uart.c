#include <cmn.h>
#include "stdarg.h"
#include "early_printf.h"
#include <linux/serial_reg.h>
#define UART0_TXD               0x0C
#define UART0_RXD               0x18
#define PIN_MUX_G6_BASE			0x049bc600
#define UART_MCR_DTR    		0x01    /* DTR   */
#define UART_MCR_RTS    		0x02    /* RTS   */
#define UART_BASE_CLK     		(24000000)
#define UART_BAUDATE      		(115200)
#define UART_LCR_8N1    		0x03
#define UART0_BASE				0x2016000
#define THR_R                   0x0
#define DLL_R                   0x0
#define DLH_R                   0x4
#define FCR_R                   0x8
#define LCR_R                   0xC
#define MCR_R                   0x10
#define LSR_R					0x14
#define UART_LCR_BKSE   		0x80    /* Bank select enable */
#define UART_FCR_FIFO_EN        0x01 	/* Fifo enable */
#define UART_FCR_RXSR           0x02 	/* Receiver soft reset */
#define UART_FCR_TXSR           0x04 	/* Transmitter soft reset */
#define IER_R                   0x4
#define UART_LCRVAL UART_LCR_8N1
#define UART_MCRVAL (UART_MCR_DTR | UART_MCR_RTS)
#define UART_FCR_DEFVAL (UART_FCR_FIFO_EN | UART_FCR_RXSR | UART_FCR_TXSR)

static void empty_check(u64 base, u32 timeout_us)
{
	int status;
	do {
		status = readl(base + LSR_R) & UART_LSR_THRE;
	} while (!status);
}

void uart_putc(u64 base, const char ch, u32 timeout_us)
{
	empty_check(base, timeout_us);
	writel(ch, base + THR_R);
}

void uart_init(u64 base)
{
	u32 baud = (UART_BASE_CLK + UART_BAUDATE/2) / 16 / UART_BAUDATE;
	u32 temp = 0;

	/* uart0 pin config */
	if (base == UART0_BASE) {
		writel(0xC3, (PIN_MUX_G6_BASE + UART0_TXD));
		writel(0xC3, (PIN_MUX_G6_BASE + UART0_RXD));
	}

	writel(0, base + IER_R);
	writel(UART_MCRVAL, base + MCR_R);
	writel(UART_FCR_DEFVAL, base + FCR_R);
	writel(UART_LCRVAL, base + LCR_R);

	/* set baud*/
	temp = readl(base + LCR_R) & ~UART_LCR_BKSE;
	writel(temp | UART_LCR_BKSE, base + LCR_R);
	writel(baud & 0xFF, base + DLL_R);
	writel((baud >> 8) & 0xFF, base + DLH_R);
	writel(temp, base + LCR_R);
}

void early_print_init(void)
{
	uart_init(UART0_BASE);
}

void output_char(char c)
{
	uart_putc(UART0_BASE, c, 5);
}
