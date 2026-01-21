#include "spinor.h"
#include "dw_spi.h"
#include "chip_reg.h"

static int spinor_qmode = !!((SYS_IRAM_BASE + 0x8) & (1 << 3));

static int spinor_read_data(u32 addr, u32 len, u8 *buf)
{
	struct spi_mem_op op = SPINOR_READ_DATA_OP(spinor_qmode, addr, buf, len);
	u32 remaining = len;
	int ret;

	while (remaining) {
		op.data.nbytes = remaining;
		ret = spi_mem_adjust_op_size(&op);
		if (ret)
			return ret;

		ret = spi_mem_exec_op(&op);
		if (ret)
		{
			return ret;
		}

		op.addr.val += op.data.nbytes;
		remaining -= op.data.nbytes;
		op.data.buf.in += op.data.nbytes;
	}

	return len;
}

int spinor_read(u32 addr, u32 len, u8 *buf)
{
	return spinor_read_data(addr, len, buf);
}
