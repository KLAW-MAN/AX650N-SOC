#include "cmn.h"
#include "dw_spi.h"
#include "spinor.h"
#include "chip_reg.h"
#include "early_printf.h"

static u32 ssi_version = SPI_VERSION_102A;

static inline u32 dw_read(u32 offset)
{
	return readl((DW_SPI_BASE + offset));
}

static inline void dw_write(u32 offset, u32 val)
{
	writel(val, (DW_SPI_BASE + offset));
}

static void spi_enable_chip(int enable)
{
	dw_write(DW_SPI_SSIENR, (enable ? 1 : 0));
}

static void dw_writer(struct dw_poll_priv *priv)
{
	/* get the max entries we can fill into tx fifo */
	u32 dw = priv->fifowidth >> 3;
	u32 tx_left = priv->entries;
	u32 tx_avail = DW_SPI_FIFO_LEN/dw - dw_read(DW_SPI_TXFLR);
	u32 tx_max = tx_left > tx_avail ? tx_avail : tx_left;
	priv->entries = tx_left - tx_max;

	while (tx_max--) {
		if (priv->fifowidth == DFS_8_BIT)
			dw_write(DW_SPI_DR, (*(u8 *)(priv->tx)));
		else
			dw_write(DW_SPI_DR, (*(u32 *)(priv->tx)));
		priv->tx += dw;
	}
}

static void dw_reader(struct dw_poll_priv *priv)
{
	/* get the max entries we should read out of rx fifo */
	u32 dw = priv->fifowidth >> 3;
	u32 rx_left = priv->entries;
	u32 rx_avail = dw_read(DW_SPI_RXFLR);
	u32 rx_max = rx_left > rx_avail ? rx_avail : rx_left;
	priv->entries = rx_left - rx_max;

	while (rx_max--) {
		if (priv->fifowidth == DFS_8_BIT)
			*(u8 *)(priv->rx) = dw_read(DW_SPI_DR);
		else
			*(u32 *)(priv->rx) = dw_read(DW_SPI_DR);
		priv->rx += dw;
	}
}

static int poll_transfer(struct dw_poll_priv *priv)
{
	if (priv->rx)
		/* dummy word to trigger RXONLY transfer */
		dw_write(DW_SPI_DR, 0xffffffff);
	do {
		if (priv->tx) {
			dw_writer(priv);
		} else if (priv->rx){
			dw_reader(priv);
		} else {
			return -1;
		}
	} while (priv->entries);

	return 0;
}

static void external_cs_manage(u32 on)
{
	u32 val;

	//value
	if (on) {//high
		val = readl((GPIO3_BASE + PORTA_DR));
		val |= SPI_CS_GPIO(CURR_CS);
		writel(val, (GPIO3_BASE + PORTA_DR));
	} else {//low
		val = readl((GPIO3_BASE + PORTA_DR));
		val &= ~ SPI_CS_GPIO(CURR_CS);
		writel(val, (GPIO3_BASE + PORTA_DR));
	}
}

static int dw_spi_xfer(unsigned int bitlen, const void *dout, void *din,
				unsigned int flags, unsigned int fifowidth, unsigned int qmode)
{
	const u8 *tx = dout;
	u8 *rx = din;
	int ret = 0;
	u32 cr0 = 0;
	u32 spicr0 = 0;
	u32 txftlr = 0;
	u32 rxftlr = 0;
	u32 status, txEmpty, busy;
	int timeout = RX_TIMEOUT;
	u32 tmode = SPI_TMOD_TO;
	u32 nbytes;
	u32 entries;
	struct dw_poll_priv priv;
	/* txrx_dlr and burst_len must keep identical,
	 currently configured as half fifo depth */
#ifdef SPI_MST_USE_DMA
	u32 txrx_dlr = SPI_MST_DMA_TXRX_DLR;
	u32 burst_len = DMAC_BURST_TRANS_LEN_32;
	u32 dma_width = DMAC_TRANS_WIDTH_8;

	if (fifowidth == DFS_32_BIT)
		dma_width = DMAC_TRANS_WIDTH_32;
#endif

	/* spi core configured to do 8 bit transfers */
	if (bitlen % 8 || fifowidth % 8 || bitlen % fifowidth) {
		return -1;
	}
	nbytes = bitlen >> 3;
	entries = nbytes / (fifowidth >> 3);

	/* Start the transaction if necessary. */
	if (flags & SPI_XFER_BEGIN)
		external_cs_manage(0);

	cr0 = ((fifowidth - 1) << SPI_DFS_OFFSET) | (SPI_FRF_SPI << SPI_FRF_OFFSET);
	if (qmode) {
		cr0 |= SPI_QAUD_MODE << SPI_SPI_FRF_OFFSET;
		spicr0 = SPI_CLK_STRETCH_EN << SPI_CLK_STRETCH_EN_OFFSET;
		rxftlr = SPI_MST_DMA_TXRX_DLR + 1;
		if(tx) {
			spicr0 |= SPI_BOTH_IN_SPIFRF_MODE << SPI_TRANS_TYPE_OFFSET;
			if (fifowidth == DFS_32_BIT)
				spicr0 |= SPI_ADDR_L_32_BITS << SPI_ADDR_L_OFFSET;
			else
				spicr0 |= SPI_ADDR_L_8_BITS << SPI_ADDR_L_OFFSET;

			txftlr = 1 << SPI_TXFTHR_OFFSET;//bug fix
		}
	}

	if (tx)
		tmode = SPI_TMOD_TO;
	else if (rx)
		tmode = SPI_TMOD_RO;

	cr0 &= ~SPI_TMOD_MASK;
	cr0 |= (tmode << SPI_TMOD_OFFSET);

	/* Disable controller before writing control registers */
	spi_enable_chip(0);
	/* Reprogram cr0 only if changed */
	if (dw_read(DW_SPI_CTRL0) != cr0)
		dw_write(DW_SPI_CTRL0, cr0);

	if (qmode && dw_read(DW_SPI_SPI_CTRL0) != spicr0)
		dw_write(DW_SPI_SPI_CTRL0, spicr0);

	dw_write(DW_SPI_TXFLTR, txftlr);
	dw_write(DW_SPI_RXFLTR, rxftlr);

	if (rx)
		dw_write(DW_SPI_CTRL1, entries-1);

	if (SPI_VERSION_103A != ssi_version) {
		/* Enable controller after writing control registers */
		spi_enable_chip(1);
	}

#ifdef SPI_MST_USE_DMA
	if (entries >= txrx_dlr) {
		if (SPI_VERSION_103A == ssi_version) {
			if (tx) {
				dw_write(DW_SPI_DMATDLR, txrx_dlr);
				dw_write(DW_SPI_DMACR, 2);
				spi_enable_chip(1);
				axi_dma_xfer_start(DMAC_CHAN1, (u64)tx, (DW_SPI_BASE + DW_SPI_DR), nbytes,
					dma_width, dma_width, burst_len, DMA_MEM_TO_DEV, SSI_DMA_TX_REQ);
			}
			else {
				axi_dma_xfer_start(DMAC_CHAN1, (DW_SPI_BASE + DW_SPI_DR), (u64)rx, nbytes,
					dma_width, dma_width, burst_len, DMA_DEV_TO_MEM, SSI_DMA_RX_REQ);
				dw_write(DW_SPI_DMARDLR, txrx_dlr-1);
				dw_write(DW_SPI_DMACR, 1);
				spi_enable_chip(1);
				/* dummy word to trigger RXONLY transfer */
				dw_write(DW_SPI_DR, 0xffffffff);
			}

			ret = axi_dma_wait_xfer_done(DMAC_CHAN1);
		} else {
			if (tx) {
				axi_dma_xfer_start(DMAC_CHAN0, (u64)tx, (DW_SPI_BASE + DW_SPI_DR), nbytes,
				dma_width, dma_width, burst_len, DMA_MEM_TO_DEV, SSI_DMA_TX_REQ);
				dw_write(DW_SPI_DMATDLR, txrx_dlr);
				dw_write(DW_SPI_DMACR, 2);
			}
			else {
				axi_dma_xfer_start(DMAC_CHAN0, (DW_SPI_BASE + DW_SPI_DR), (u64)rx, nbytes,
				dma_width, dma_width, burst_len, DMA_DEV_TO_MEM, SSI_DMA_RX_REQ);
				dw_write(DW_SPI_DMARDLR, txrx_dlr-1);
				dw_write(DW_SPI_DMACR, 1);
				/* dummy word to trigger RXONLY transfer */
				dw_write(DW_SPI_DR, 0xffffffff);
			}

			ret = axi_dma_wait_xfer_done(DMAC_CHAN0);
			dw_write(DW_SPI_DMACR, 0);
		}
	} else
	{
		UNUSED(txrx_dlr);
		UNUSED(burst_len);
		UNUSED(dma_width);
#endif
		if (SPI_VERSION_103A == ssi_version) {
			/* Enable controller after writing control registers */
			spi_enable_chip(1);
		}
		priv.entries = entries;
		priv.fifowidth = fifowidth;
		priv.tx = (void *)tx;
		priv.rx = rx;
		ret = poll_transfer(&priv);
#ifdef SPI_MST_USE_DMA
	}
#endif

	/* Wait for current transmit operation to complete. */
	do {
		status = dw_read(DW_SPI_SR);
		txEmpty = status & SR_TF_EMPT;
		busy = status & SR_BUSY;
	} while (!txEmpty || (busy && timeout--));

	if (timeout < 0)
		ret = -1;
#ifdef SPI_MST_USE_DMA
	else if ((SPI_VERSION_103A == ssi_version) && (entries >= txrx_dlr)) {
		spi_enable_chip(0);
		dw_write(DW_SPI_DMACR, 0);
		spi_enable_chip(1);
	}
#endif

	/* Stop the transaction if necessary */
	if (flags & SPI_XFER_END)
		external_cs_manage(1);

	return ret;
}

int spi_mem_adjust_op_size(struct spi_mem_op *op)
{
	if (op->data.dir == SPI_MEM_DATA_IN) {
		op->data.nbytes = op->data.nbytes <= MAX_READ_SIZE ? op->data.nbytes : MAX_READ_SIZE;
	} else {
		op->data.nbytes = op->data.nbytes <= MAX_WRITE_SIZE ? op->data.nbytes : MAX_WRITE_SIZE;
	}

	if (!op->data.nbytes)
		return -1;

	return 0;
}

int spi_mem_exec_op(const struct spi_mem_op *op)
{
	unsigned int pos = 0;
	const u8 *tx_buf = NULL;
	u8 *rx_buf = NULL;
	u32 fifowidth = 8;
	u32 qmode = 0;
	int op_len;
	u32 flag;
	int ret;
	int i;

	if (op->data.nbytes) {
		if (op->data.dir == SPI_MEM_DATA_IN)
			rx_buf = op->data.buf.in;
		else
			tx_buf = op->data.buf.out;
	}

	op_len = sizeof(op->cmd.opcode) + op->addr.nbytes + op->dummy.nbytes;
	u8 op_buf[op_len];

	op_buf[pos++] = op->cmd.opcode;

	if (op->addr.nbytes) {
		for (i = 0; i < op->addr.nbytes; i++)
			op_buf[pos + i] = op->addr.val >>
				(8 * (op->addr.nbytes - i - 1));
		pos += op->addr.nbytes;
	}

	if (op->dummy.nbytes) {
		for (i = 0; i < op->dummy.nbytes; i++)
			op_buf[pos + i] = 0xff;
		//pos += op->dummy.nbytes;
	}

	/* 1st transfer: opcode + address + dummy cycles */
	flag = SPI_XFER_BEGIN;
	/* Make sure to set END bit if no tx or rx data follow */
	if (!tx_buf && !rx_buf)
		flag |= SPI_XFER_END;

	ret = dw_spi_xfer(op_len * 8, op_buf, NULL, flag, fifowidth, qmode);
	if (ret)
		return ret;

	/* 2nd transfer: rx or tx data path */
	if (tx_buf || rx_buf) {
		if (op->data.buswidth == 4)
			qmode = 1;
		ret = dw_spi_xfer(op->data.nbytes * 8, tx_buf, rx_buf, SPI_XFER_END, fifowidth, qmode);
		if (ret)
			return ret;
	}

#if 0
	for (i = 0; i < pos; i++)
		early_printf("%02x ", op_buf[i]);
	early_printf("| [%dB %s] ",
	      tx_buf || rx_buf ? op->data.nbytes : 0,
	      tx_buf || rx_buf ? (tx_buf ? "out" : "in") : "-");
	for (i = 0; i < op->data.nbytes; i++)
		early_printf("%02x ", tx_buf ? tx_buf[i] : rx_buf[i]);
	early_printf("[ret %d]\n", ret);
#endif

	return 0;
}
