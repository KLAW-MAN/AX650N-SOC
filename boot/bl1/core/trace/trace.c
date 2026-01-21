#include "trace.h"
#include "cmn.h"

static const char * log_id_to_string[LOG_PRINT_MAX] = {
	[LOG_DDR_INIT] = "D",
	[LOG_PCIE_BOOT] = "P",
	[LOG_PCIE_EP_INIT] = "EI",
	[LOG_PCIE_DMA_READ] = "R",
	[LOG_PCIE_READ_FAIL] = "F",
	[LOG_PCIE_READ_SUC] = "S",
	[LOG_PCIE_LINK] = "L",
};

void log_print_str(const char * string)
{
	char * ch = (char *)string;

	while('\0' != *ch) {
		writel(*ch, UART0_BASE + THR_R);
		writel(*ch, UART1_BASE + THR_R);
		ch++;
	}
}

void log_print(LOG_PRINT_ID id)
{
	if ((id < 0) || (id >= LOG_PRINT_MAX))
		return;
	log_print_str(log_id_to_string[id]);
}

