
/*
 * AXERA Host Controller Interface
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <cpu_func.h>
#include <dm.h>
#include <errno.h>
#include <malloc.h>
#include <mmc.h>
#include <sdhci.h>
#include <dm.h>
#include <linux/dma-mapping.h>
#include <linux/io.h>
#include <asm/arch/ax650.h>
#define DWC_MSHC_EMMC_BASE                    0x28000000
#define DWC_MSHC_P_VENDOR_SPECIFIC_AREA       0xe8
#define BITS_DWC_MSHC_P_VENDOR_SPECIFIC_AREA_REG_OFFSET_ADDR(x)  (((x) & 0xFFF) << 0)
#define EMMC_CTRL_R                           0x2c
#define BIT_DWC_MSHC_EMMC_CTRL_EMMC_RST_N_OE  BIT(3)
#define BIT_DWC_MSHC_EMMC_CTRL_EMMC_RST_N     BIT(2)

#define SDHCI_AXERA_PHY_BASE		0x300

/* PHY */
#define SDHCI_AXERA_PHY_CNFG	        0x00
#define SDHCI_AXERA_PHY_CMDPAD_CNFG	0x04
#define SDHCI_AXERA_PHY_DATAPAD_CNFG	0x06
#define SDHCI_AXERA_PHY_CLKPAD_CNFG	0x08
#define SDHCI_AXERA_PHY_STBPAD_CNFG	0x0A
#define SDHCI_AXERA_PHY_RSTNPAD_CNFG	0x0C
#define SDHCI_AXERA_PHY_COMMDL_CNFG	0x1C
#define SDHCI_AXERA_PHY_SDCLKDL_CNFG	0x1D
#define SDHCI_AXERA_PHY_SDCLKDL_DC	    0x1E
#define SDHCI_AXERA_PHY_SMPLDL_CNFG	0x20
#define SDHCI_AXERA_PHY_ATDL_CNFG	    0x21

/* PHY_CNFG */
#define BIT_MSHC_PHY_CNFG_PHY_PWRGOOD       BIT(1)
#define BIT_MSHC_PHY_CNFG_PHY_RSTN          BIT(0)

#define SDCTRL_REG_NUM 24
#define SDPHY_REG_NUM  12
static u32 emmc_ctrl_reg_buf[SDCTRL_REG_NUM] = {0xff};
static u32 emmc_phy_reg_buf[SDPHY_REG_NUM] = {0xff};
static void sdhci_dump_host(struct sdhci_host *host)
{
	u32 i;

	for (i = 0; i < SDCTRL_REG_NUM; i++) {
		emmc_ctrl_reg_buf[i] = readl(host->ioaddr + i * 4);
	}
	for (i = 0; i < SDPHY_REG_NUM; i++) {
		emmc_phy_reg_buf[i] = readl(host->ioaddr + SDHCI_AXERA_PHY_BASE + i * 4);
	}
	printf("==================== REG BUF DUMP ===================\n");
	for (i = 0; i < SDCTRL_REG_NUM / 4; i++) {
		printf("%lX ++ %04X: %08X %08X %08X %08X\n", (ulong)host->ioaddr, i * 0x10,
		       emmc_ctrl_reg_buf[i * 4], emmc_ctrl_reg_buf[i * 4 + 1],
		       emmc_ctrl_reg_buf[i * 4 + 2], emmc_ctrl_reg_buf[i * 4 + 3]);
	}
	for (i = 0; i < SDPHY_REG_NUM / 4; i++) {
		printf("%lX ++ %04X: %08X %08X %08X %08X\n", (ulong)host->ioaddr, SDHCI_AXERA_PHY_BASE + i * 0x10,
		       emmc_phy_reg_buf[i * 4], emmc_phy_reg_buf[i * 4 + 1],
		       emmc_phy_reg_buf[i * 4 + 2], emmc_phy_reg_buf[i * 4 + 3]);
	}
	printf("==================== REG UART DUMP ==================\n");
	for (i = 0; i < SDCTRL_REG_NUM / 4; i++) {
		printf("%lX ++ %04X: %08X %08X %08X %08X\n",
		       (ulong)host->ioaddr, i * 0x10,
		       readl(host->ioaddr + i * 0x10), readl(host->ioaddr + i * 0x10 + 0x4),
		       readl(host->ioaddr + i * 0x10 + 0x8), readl(host->ioaddr + i * 0x10 + 0xC));
	}
	for (i = 0; i < SDPHY_REG_NUM / 4; i++) {
		printf("%lX ++ %04X: %08X %08X %08X %08X\n",
		       (ulong)host->ioaddr, SDHCI_AXERA_PHY_BASE + i * 0x10,
		       readl(host->ioaddr + SDHCI_AXERA_PHY_BASE + i * 0x10), readl(host->ioaddr + SDHCI_AXERA_PHY_BASE + i * 0x10 + 0x4),
		       readl(host->ioaddr + SDHCI_AXERA_PHY_BASE + i * 0x10 + 0x8), readl(host->ioaddr + SDHCI_AXERA_PHY_BASE + i * 0x10 + 0xC));
	}
	printf("=====================================================\n");
}

static void sdhci_reset(struct sdhci_host *host, u8 mask)
{
	unsigned long timeout;
	//pr_err("enter sdhci_reset\n");

	/* Wait max 100 ms */
	timeout = 100;
	sdhci_writeb(host, mask, SDHCI_SOFTWARE_RESET);
	while (sdhci_readb(host, SDHCI_SOFTWARE_RESET) & mask) {
		if (timeout == 0) {
			printf("%s: Reset 0x%x never completed.\n", __func__, (int)mask);
			return;
		}
		timeout--;
		udelay(1000);
	}
}

static void sdhci_cmd_done(struct sdhci_host *host, struct mmc_cmd *cmd)
{
	int i;
	if (cmd->resp_type & MMC_RSP_136) {
		/* CRC is stripped so we need to do some shifting. */
		for (i = 0; i < 4; i++) {
			cmd->response[i] = sdhci_readl(host, SDHCI_RESPONSE + (3 - i) * 4) << 8;
			if (i != 3)
				cmd->response[i] |= sdhci_readb(host, SDHCI_RESPONSE + (3 - i) * 4 - 1);
		}
	} else {
		cmd->response[0] = sdhci_readl(host, SDHCI_RESPONSE);
	}
}

static void sdhci_transfer_pio(struct sdhci_host *host, struct mmc_data *data)
{
	int i;
	char *offs;
	for (i = 0; i < data->blocksize; i += 4) {
		offs = data->dest + i;
		if (data->flags == MMC_DATA_READ)
			*(u32 *) offs = sdhci_readl(host, SDHCI_BUFFER);
		else
			sdhci_writel(host, *(u32 *) offs, SDHCI_BUFFER);
	}
}

#if CONFIG_IS_ENABLED(MMC_SDHCI_ADMA)
static void sdhci_adma_desc(struct sdhci_host *host, dma_addr_t dma_addr, u16 len, bool end)
{
	struct sdhci_adma_desc *desc;
	u8 attr;

	desc = &host->adma_desc_table[host->desc_slot];

	attr = ADMA_DESC_ATTR_VALID | ADMA_DESC_TRANSFER_DATA;
	if (!end)
		host->desc_slot++;
	else
		attr |= ADMA_DESC_ATTR_END;

	desc->attr = attr;
	desc->len = len;
	desc->reserved = 0;
	desc->addr_lo = lower_32_bits(dma_addr);
#ifdef CONFIG_DMA_ADDR_T_64BIT
	desc->addr_hi = upper_32_bits(dma_addr);
#endif
}

static void sdhci_prepare_adma_table(struct sdhci_host *host, struct mmc_data *data)
{
	uint trans_bytes = data->blocksize * data->blocks;
	uint desc_count = DIV_ROUND_UP(trans_bytes, ADMA_MAX_LEN);
	int i = desc_count;
	dma_addr_t dma_addr = host->start_addr;
	unsigned int first_len;

	host->desc_slot = 0;

	while (--i) {
		if (((dma_addr & (SDHCI_ADMA_BOUNDARY_SIZE - 1)) + ADMA_MAX_LEN) > SDHCI_ADMA_BOUNDARY_SIZE) {
			first_len = SDHCI_ADMA_BOUNDARY_SIZE - (dma_addr & (SDHCI_ADMA_BOUNDARY_SIZE - 1));
			sdhci_adma_desc(host, dma_addr, first_len, false);
			sdhci_adma_desc(host, (dma_addr + first_len), (ADMA_MAX_LEN - first_len), false);
		} else {
			sdhci_adma_desc(host, dma_addr, ADMA_MAX_LEN, false);
		}
		dma_addr += ADMA_MAX_LEN;
		trans_bytes -= ADMA_MAX_LEN;
	}

	if (((dma_addr & (SDHCI_ADMA_BOUNDARY_SIZE - 1)) + trans_bytes) > SDHCI_ADMA_BOUNDARY_SIZE) {
		first_len = SDHCI_ADMA_BOUNDARY_SIZE - (dma_addr & (SDHCI_ADMA_BOUNDARY_SIZE - 1));
		sdhci_adma_desc(host, dma_addr, first_len, false);
		sdhci_adma_desc(host, (dma_addr + first_len), (trans_bytes - first_len), true);
	} else {
		sdhci_adma_desc(host, dma_addr, trans_bytes, true);
	}

	flush_cache((dma_addr_t) host->adma_desc_table,
	            ROUND(desc_count * sizeof(struct sdhci_adma_desc), ARCH_DMA_MINALIGN));
}
#elif defined(CONFIG_MMC_SDHCI_SDMA)
static void sdhci_prepare_adma_table(struct sdhci_host *host, struct mmc_data *data)
{
}
#endif
#if (defined(CONFIG_MMC_SDHCI_SDMA) || CONFIG_IS_ENABLED(MMC_SDHCI_ADMA))
static void sdhci_prepare_dma(struct sdhci_host *host, struct mmc_data *data, int *is_aligned, int trans_bytes)
{
	unsigned char ctrl;
	void *buf;

	if (data->flags == MMC_DATA_READ)
		buf = data->dest;
	else
		buf = (void *)data->src;

	ctrl = sdhci_readb(host, SDHCI_HOST_CONTROL);
	ctrl &= ~SDHCI_CTRL_DMA_MASK;
	if (host->flags & USE_ADMA64)
		ctrl |= SDHCI_CTRL_ADMA64;
	else if (host->flags & USE_ADMA)
		ctrl |= SDHCI_CTRL_ADMA32;
	sdhci_writeb(host, ctrl, SDHCI_HOST_CONTROL);

	if (host->flags & USE_SDMA &&
	    (host->force_align_buffer ||
	     (host->quirks & SDHCI_QUIRK_32BIT_DMA_ADDR && ((unsigned long)buf & 0x7) != 0x0))) {
		*is_aligned = 0;
		if (data->flags != MMC_DATA_READ)
			memcpy(host->align_buffer, buf, trans_bytes);
		buf = host->align_buffer;
	}

	host->start_addr = dma_map_single(buf, trans_bytes, mmc_get_dma_dir(data));

	if (host->flags & USE_SDMA) {
		sdhci_writel(host, host->start_addr, SDHCI_DMA_ADDRESS);
	} else if (host->flags & (USE_ADMA | USE_ADMA64)) {
		sdhci_prepare_adma_table(host, data);

		sdhci_writel(host, lower_32_bits(host->adma_addr), SDHCI_ADMA_ADDRESS);
		if (host->flags & USE_ADMA64)
			sdhci_writel(host, upper_32_bits(host->adma_addr), SDHCI_ADMA_ADDRESS_HI);
	}
}
#else
static void sdhci_prepare_dma(struct sdhci_host *host, struct mmc_data *data, int *is_aligned, int trans_bytes)
{
}
#endif
static int sdhci_transfer_data(struct sdhci_host *host, struct mmc_data *data)
{
	dma_addr_t start_addr = host->start_addr;
	unsigned int stat, rdy, mask, timeout, block = 0;
	bool transfer_done = false;

	timeout = 1000000;
	rdy = SDHCI_INT_SPACE_AVAIL | SDHCI_INT_DATA_AVAIL;
	mask = SDHCI_DATA_AVAILABLE | SDHCI_SPACE_AVAILABLE;
	do {
		stat = sdhci_readl(host, SDHCI_INT_STATUS);
		if (stat & SDHCI_INT_ERROR) {
			sdhci_dump_host(host);
			printf("%s: Error detected in status(0x%X)!\n", __func__, stat);
			return -EIO;
		}
		if (!transfer_done && (stat & rdy)) {
			if (!(sdhci_readl(host, SDHCI_PRESENT_STATE) & mask))
				continue;
			sdhci_writel(host, rdy, SDHCI_INT_STATUS);
			sdhci_transfer_pio(host, data);
			data->dest += data->blocksize;
			if (++block >= data->blocks) {
				/* Keep looping until the SDHCI_INT_DATA_END is
				 * cleared, even if we finished sending all the
				 * blocks.
				 */
				transfer_done = true;
				continue;
			}
		}
		if ((host->flags & USE_DMA) && !transfer_done && (stat & SDHCI_INT_DMA_END)) {
			sdhci_writel(host, SDHCI_INT_DMA_END, SDHCI_INT_STATUS);
			if (host->flags & USE_SDMA) {
				start_addr &= ~(SDHCI_DEFAULT_BOUNDARY_SIZE - 1);
				start_addr += SDHCI_DEFAULT_BOUNDARY_SIZE;
				sdhci_writel(host, start_addr, SDHCI_DMA_ADDRESS);
			}
		}
		if (timeout-- > 0)
			udelay(10);
		else {
			printf("%s: Transfer data timeout\n", __func__);
			return -ETIMEDOUT;
		}
	} while (!(stat & SDHCI_INT_DATA_END));

	dma_unmap_single(host->start_addr, data->blocks * data->blocksize, mmc_get_dma_dir(data));

	return 0;
}

/*
 * No command will be sent by driver if card is busy, so driver must wait
 * for card ready state.
 * Every time when card is busy after timeout then (last) timeout value will be
 * increased twice but only if it doesn't exceed global defined maximum.
 * Each function call will use last timeout value.
 */
#define SDHCI_CMD_MAX_TIMEOUT			3200
#define SDHCI_CMD_DEFAULT_TIMEOUT		100
#define SDHCI_READ_STATUS_TIMEOUT		1000

#ifdef CONFIG_DM_MMC
static int sdhci_send_command(struct udevice *dev, struct mmc_cmd *cmd, struct mmc_data *data)
{
	struct mmc *mmc = mmc_get_mmc_dev(dev);

#else
static int sdhci_send_command(struct mmc *mmc, struct mmc_cmd *cmd, struct mmc_data *data)
{
#endif
	struct sdhci_host *host = mmc->priv;
	unsigned int stat = 0;
	int ret = 0;
	int trans_bytes = 0, is_aligned = 1;
	u32 mask, flags, mode;
	unsigned int time = 0;
	int mmc_dev = mmc_get_blk_desc(mmc)->devnum;
	ulong start = get_timer(0);

	host->start_addr = 0;
	/* Timeout unit - ms */
	static unsigned int cmd_timeout = SDHCI_CMD_DEFAULT_TIMEOUT;

	mask = SDHCI_CMD_INHIBIT | SDHCI_DATA_INHIBIT;

	/* We shouldn't wait for data inihibit for stop commands, even
	   though they might use busy signaling */
	if (cmd->cmdidx == MMC_CMD_STOP_TRANSMISSION ||
	    ((cmd->cmdidx == MMC_CMD_SEND_TUNING_BLOCK || cmd->cmdidx == MMC_CMD_SEND_TUNING_BLOCK_HS200) && !data))
		mask &= ~SDHCI_DATA_INHIBIT;

	while (sdhci_readl(host, SDHCI_PRESENT_STATE) & mask) {
		if (time >= cmd_timeout) {
			printf("%s: MMC: %d busy ", __func__, mmc_dev);
			if (2 * cmd_timeout <= SDHCI_CMD_MAX_TIMEOUT) {
				cmd_timeout += cmd_timeout;
				printf("timeout increasing to: %u ms.\n", cmd_timeout);
			} else {
				sdhci_dump_host(host);
				puts("timeout.\n");
				return -ECOMM;
			}
		}
		time++;
		udelay(1000);
	}

	sdhci_writel(host, SDHCI_INT_ALL_MASK, SDHCI_INT_STATUS);

	mask = SDHCI_INT_RESPONSE;
	if ((cmd->cmdidx == MMC_CMD_SEND_TUNING_BLOCK || cmd->cmdidx == MMC_CMD_SEND_TUNING_BLOCK_HS200) && !data)
		mask = SDHCI_INT_DATA_AVAIL;

	if (!(cmd->resp_type & MMC_RSP_PRESENT))
		flags = SDHCI_CMD_RESP_NONE;
	else if (cmd->resp_type & MMC_RSP_136)
		flags = SDHCI_CMD_RESP_LONG;
	else if (cmd->resp_type & MMC_RSP_BUSY) {
		flags = SDHCI_CMD_RESP_SHORT_BUSY;
		if (data)
			mask |= SDHCI_INT_DATA_END;
	} else
		flags = SDHCI_CMD_RESP_SHORT;

	if (cmd->resp_type & MMC_RSP_CRC)
		flags |= SDHCI_CMD_CRC;
	if (cmd->resp_type & MMC_RSP_OPCODE)
		flags |= SDHCI_CMD_INDEX;
	if (data || cmd->cmdidx == MMC_CMD_SEND_TUNING_BLOCK || cmd->cmdidx == MMC_CMD_SEND_TUNING_BLOCK_HS200)
		flags |= SDHCI_CMD_DATA;

	/* Set Transfer mode regarding to data flag */
	if (data) {
		sdhci_writeb(host, 0xe, SDHCI_TIMEOUT_CONTROL);
		mode = SDHCI_TRNS_BLK_CNT_EN;
		trans_bytes = data->blocks * data->blocksize;
		if (data->blocks > 1)
			mode |= SDHCI_TRNS_MULTI;

		if (data->flags == MMC_DATA_READ)
			mode |= SDHCI_TRNS_READ;

		if (host->flags & USE_DMA) {
			mode |= SDHCI_TRNS_DMA;
			sdhci_prepare_dma(host, data, &is_aligned, trans_bytes);
		}

		sdhci_writew(host, SDHCI_MAKE_BLKSZ(SDHCI_DEFAULT_BOUNDARY_ARG, data->blocksize), SDHCI_BLOCK_SIZE);
		sdhci_writew(host, data->blocks, SDHCI_BLOCK_COUNT);
		sdhci_writew(host, mode, SDHCI_TRANSFER_MODE);
	} else if (cmd->resp_type & MMC_RSP_BUSY) {
		sdhci_writeb(host, 0xe, SDHCI_TIMEOUT_CONTROL);
	}

	sdhci_writel(host, cmd->cmdarg, SDHCI_ARGUMENT);
	sdhci_writew(host, SDHCI_MAKE_CMD(cmd->cmdidx, flags), SDHCI_COMMAND);
	start = get_timer(0);
	do {
		stat = sdhci_readl(host, SDHCI_INT_STATUS);
		if (stat & SDHCI_INT_ERROR)
			break;

		if (get_timer(start) >= SDHCI_READ_STATUS_TIMEOUT) {
			if (host->quirks & SDHCI_QUIRK_BROKEN_R1B) {
				return 0;
			} else {
				sdhci_dump_host(host);
				printf("%s: Timeout for status update!\n", __func__);
				return -ETIMEDOUT;
			}
		}
	} while ((stat & mask) != mask);

	if ((stat & (SDHCI_INT_ERROR | mask)) == mask) {
		sdhci_cmd_done(host, cmd);
		sdhci_writel(host, mask, SDHCI_INT_STATUS);
	} else
		ret = -1;

	if (!ret && data)
		ret = sdhci_transfer_data(host, data);

	if (host->quirks & SDHCI_QUIRK_WAIT_SEND_CMD)
		udelay(1000);

	stat = sdhci_readl(host, SDHCI_INT_STATUS);

	sdhci_writel(host, SDHCI_INT_ALL_MASK, SDHCI_INT_STATUS);
	if (!ret) {
		if ((host->quirks & SDHCI_QUIRK_32BIT_DMA_ADDR) && !is_aligned && (data->flags == MMC_DATA_READ))
			memcpy(data->dest, host->align_buffer, trans_bytes);
		return 0;
	}

	sdhci_reset(host, SDHCI_RESET_CMD);
	sdhci_reset(host, SDHCI_RESET_DATA);
	if (stat & SDHCI_INT_TIMEOUT)
		return -ETIMEDOUT;
	else
		return -ECOMM;
}

#if defined(CONFIG_DM_MMC) && defined(MMC_SUPPORTS_TUNING)
static int sdhci_execute_tuning(struct udevice *dev, uint opcode)
{
	int err;
	struct mmc *mmc = mmc_get_mmc_dev(dev);
	struct sdhci_host *host = mmc->priv;

	debug("%s\n", __func__);

	if (host->ops && host->ops->platform_execute_tuning) {
		err = host->ops->platform_execute_tuning(mmc, opcode);
		if (err)
			return err;
		return 0;
	}
	return 0;
}
#endif

#define SDHCI_MAX_DIV	30
#define SDHCI_AXERA_EXTDLY_EN 0x1
int sdhci_set_clock(struct mmc *mmc, unsigned int clock)
{
	struct sdhci_host *host = mmc->priv;
	int clk_sel = 0, div = 0;
	u16 val = 0;
	u8 set_extdly = 0;
	unsigned int timeout;
	/* Wait max 20 ms */
	timeout = 200;
	while (sdhci_readl(host, SDHCI_PRESENT_STATE) & (SDHCI_CMD_INHIBIT | SDHCI_DATA_INHIBIT)) {
		if (timeout == 0) {
			printf("%s: Timeout to wait cmd & data inhibit\n", __func__);
			return -EBUSY;
		}

		timeout--;
		udelay(100);
	}
	sdhci_writew(host, 0, SDHCI_CLOCK_CONTROL);	//stop clk
	if (clock == 0)
		return 0;
	if (mmc->cfg->host_caps & MMC_CAP_NONREMOVABLE) {
		switch (clock) {
		case INIT_CLK_400K:
		case INIT_CLK_300K:
			clk_sel = 0;	//source gpll_24M/2
			div = 0xf;	//12M/30
			break;
		case INIT_CLK_200K:
		case INIT_CLK_100K:
			clk_sel = 0;
			div = 0x1E;	//12M/60
			break;
		}
		/* set clock to 25M */
		if (clock <= 26000000 && clock > 400000) {
			clk_sel = 2;	//source npll_400M/2
			div = 4;	//200M/8
		}
		/* set clock to 50M */
		if (clock <= 52000000 && clock > 26000000) {
			clk_sel = 2;	//source npll_400M/2
			div = 2;	//200M/4
		}
		/* set clock to 200M */
		if (clock > 52000000) {
			clk_sel = 2;	//source npll_400M/2
			div = 0;	//200M/1
		}

		writel(BIT(2), FLASH_SYS_CLK_RST_CLK_EB_0_CLR);	//close cclk_tx_emmc_eb
		writel(CLK_EMMC_CLK_CARD_SEL(0x3), FLASH_SYS_CLK_RST_CLK_MUX0_CLR);	//clr clk source bit[1:0]
		writel(CLK_EMMC_CLK_CARD_SEL(clk_sel), FLASH_SYS_CLK_RST_CLK_MUX0_SET);	//Select clk source bit[1:0]
		writel(BIT(2), FLASH_SYS_CLK_RST_CLK_EB_0_SET);	//open cclk_tx_emmc_eb
		writel(GENMASK(4, 0), FLASH_SYS_CLK_RST_CLK_DIV_0_CLR);	//clear bit[4:0]
		writel(CLK_EMMC_CLK_CARD_DIV(div), FLASH_SYS_CLK_RST_CLK_DIV_0_SET);	//set div
		writel(EMMC_CLK_DIV_UPDATE, FLASH_SYS_CLK_RST_CLK_DIV_0_SET);
		writel(EMMC_CLK_DIV_UPDATE, FLASH_SYS_CLK_RST_CLK_DIV_0_CLR);

	} else {		//SD
		switch (clock) {
		case INIT_CLK_400K:
		case INIT_CLK_300K:
			clk_sel = 0;	//source gpll_24M/2
			div = 0xf;	//12M/30
			break;
		case INIT_CLK_200K:
		case INIT_CLK_100K:
			clk_sel = 0;
			div = 0x1E;	//12M/60
			break;
		}
		/* set clock to 26M */
		if (clock <= 25000000 && clock > 400000) {
			clk_sel = 2;	//source gpll_416m/2
			div = 4;	//208M/8
		}
		/* set clock to 52M */
		if (clock <= 52000000 && clock > 25000000) {
			clk_sel = 2;	//source gpll_416m/2
			div = 2;	//208M/4
		}
		/* set clock to 208M */
		if (clock > 52000000) {
			clk_sel = 2;	//source gpll_416m/2
			div = 0;	//208M/1
		}
		writel(BIT(2), PIPE_SYS_CLK_EB0_CLR);	//close cclk_tx_pp_sdio_eb
		writel(CLK_SDIO_CLK_CARD_SEL(0x3), PIPE_SYS_CLK_MUX_0_CLR);	//clr clk source bit[3:2]
		writel(CLK_SDIO_CLK_CARD_SEL(clk_sel), PIPE_SYS_CLK_MUX_0_SET);	//Select clk source bit[3:2]
		writel(BIT(2), PIPE_SYS_CLK_EB0_SET);	//open cclk_tx_pp_sdio_eb
		writel(GENMASK(9, 5), PIPE_SYS_CLK_DIV_0_CLR);	//clear bit[9:5]
		writel(CLK_SDIO_CLK_CARD_DIV(div), PIPE_SYS_CLK_DIV_0_SET);
		writel(SDIO_CLK_DIV_UPDATE, PIPE_SYS_CLK_DIV_0_SET);
		writel(SDIO_CLK_DIV_UPDATE, PIPE_SYS_CLK_DIV_0_CLR);
	}

	val |= BIT_DWC_MSHC_CLK_CTRL_INTERNAL_CLK_EN;
	sdhci_writew(host, val, SDHCI_CLOCK_CONTROL);
	//check stable wait max 150ms
	timeout = 150;
	while (!((clock = sdhci_readw(host, SDHCI_CLOCK_CONTROL))
	         & SDHCI_CLOCK_INT_STABLE)) {
		if (timeout == 0) {
			printf("%s: Internal clock never stabilised.\n", __func__);
			return -EBUSY;
		}
		timeout--;
		udelay(1000);
	}

	//if not timeout then set pll_enable
	val |= BIT_DWC_MSHC_CLK_CTRL_PLL_ENABLE;
	sdhci_writew(host, val, SDHCI_CLOCK_CONTROL);

	//check stable wait max 150ms
	timeout = 150;
	while (!((clock = sdhci_readw(host, SDHCI_CLOCK_CONTROL))
	         & SDHCI_CLOCK_INT_STABLE)) {
		if (timeout == 0) {
			printf("%s: Internal clock never stabilised.\n", __func__);
			return -EBUSY;
		}
		timeout--;
		udelay(1000);
	}

	if (mmc->selected_mode == SD_HS) {
		set_extdly = sdhci_readb(host, SDHCI_AXERA_PHY_BASE + SDHCI_AXERA_PHY_SDCLKDL_CNFG);
		set_extdly |= SDHCI_AXERA_EXTDLY_EN;
		sdhci_writeb(host, set_extdly, SDHCI_AXERA_PHY_SDCLKDL_CNFG);
	}
	//enable clk
	val |= BIT_DWC_MSHC_CLK_CTRL_SD_CLK_EN;
	sdhci_writew(host, val, SDHCI_CLOCK_CONTROL);
	return 0;
}

static void sdhci_set_power(struct sdhci_host *host, unsigned short power)
{
	u8 pwr = 0;

	if (power != (unsigned short)-1) {
		switch (1 << power) {
		case MMC_VDD_165_195:
			pwr = SDHCI_POWER_180;
			break;
		case MMC_VDD_29_30:
		case MMC_VDD_30_31:
			pwr = SDHCI_POWER_300;
			break;
		case MMC_VDD_32_33:
		case MMC_VDD_33_34:
			pwr = SDHCI_POWER_330;
			break;
		}
	}

	if (pwr == 0) {
		sdhci_writeb(host, 0, SDHCI_POWER_CONTROL);
		return;
	}

	pwr |= SDHCI_POWER_ON;

	sdhci_writeb(host, pwr, SDHCI_POWER_CONTROL);
}

void sdhci_set_uhs_timing(struct sdhci_host *host)
{
	struct mmc *mmc = host->mmc;
	u32 reg;

	reg = sdhci_readw(host, SDHCI_HOST_CONTROL2);
	reg &= ~SDHCI_CTRL_UHS_MASK;

	switch (mmc->selected_mode) {
	case MMC_HS_52:
		reg |= SDHCI_CTRL_UHS_SDR25;
		break;
	case UHS_SDR50:
		reg |= SDHCI_CTRL_UHS_SDR50;
		break;
	case UHS_DDR50:
	case MMC_DDR_52:
		reg |= SDHCI_CTRL_UHS_DDR50;
		break;
	case UHS_SDR104:
	case MMC_HS_200:
		reg |= SDHCI_CTRL_UHS_SDR104;
		break;
	case SD_HS:
		reg |= SDHCI_CTRL_UHS_SDR25;
		break;
	default:
		reg |= SDHCI_CTRL_UHS_SDR12;
	}
	sdhci_writew(host, reg, SDHCI_HOST_CONTROL2);
}

#ifdef CONFIG_DM_MMC
static int sdhci_set_ios(struct udevice *dev)
{
	struct mmc *mmc = mmc_get_mmc_dev(dev);
#else
static int sdhci_set_ios(struct mmc *mmc)
{
#endif
	u32 ctrl;
	struct sdhci_host *host = mmc->priv;

	if (host->ops && host->ops->set_control_reg)
		host->ops->set_control_reg(host);

	if (mmc->clock != host->clock)
		sdhci_set_clock(mmc, mmc->clock);

	if (mmc->clk_disable)
		sdhci_set_clock(mmc, 0);

	/* Set bus width */
	ctrl = sdhci_readb(host, SDHCI_HOST_CONTROL);
	if (mmc->bus_width == 8) {
		ctrl &= ~SDHCI_CTRL_4BITBUS;
		if ((SDHCI_GET_VERSION(host) >= SDHCI_SPEC_300) || (host->quirks & SDHCI_QUIRK_USE_WIDE8))
			ctrl |= SDHCI_CTRL_8BITBUS;
	} else {
		if ((SDHCI_GET_VERSION(host) >= SDHCI_SPEC_300) || (host->quirks & SDHCI_QUIRK_USE_WIDE8))
			ctrl &= ~SDHCI_CTRL_8BITBUS;
		if (mmc->bus_width == 4)
			ctrl |= SDHCI_CTRL_4BITBUS;
		else
			ctrl &= ~SDHCI_CTRL_4BITBUS;
	}

	if (mmc->clock > 26000000)
		ctrl |= SDHCI_CTRL_HISPD;
	else
		ctrl &= ~SDHCI_CTRL_HISPD;

	if ((host->quirks & SDHCI_QUIRK_NO_HISPD_BIT) || (host->quirks & SDHCI_QUIRK_BROKEN_HISPD_MODE))
		ctrl &= ~SDHCI_CTRL_HISPD;

	sdhci_writeb(host, ctrl, SDHCI_HOST_CONTROL);
	sdhci_set_uhs_timing(host);
	/* If available, call the driver specific "post" set_ios() function */
	if (host->ops && host->ops->set_ios_post)
		return host->ops->set_ios_post(host);

	return 0;
}

static int sdhci_init(struct mmc *mmc)
{
	struct sdhci_host *host = mmc->priv;

	pr_err("enter sdhci_init\n");
#if CONFIG_IS_ENABLED(DM_MMC) && CONFIG_IS_ENABLED(DM_GPIO)
	struct udevice *dev = mmc->dev;

	gpio_request_by_name(dev, "cd-gpios", 0, &host->cd_gpio, GPIOD_IS_IN);
#endif

	sdhci_reset(host, SDHCI_RESET_ALL);

#if defined(CONFIG_FIXED_SDHCI_ALIGNED_BUFFER)
	host->align_buffer = (void *)CONFIG_FIXED_SDHCI_ALIGNED_BUFFER;
	/*
	 * Always use this bounce-buffer when CONFIG_FIXED_SDHCI_ALIGNED_BUFFER
	 * is defined.
	 */
	host->force_align_buffer = true;
#else
	if (host->quirks & SDHCI_QUIRK_32BIT_DMA_ADDR) {
		host->align_buffer = memalign(8, 512 * 1024);
		if (!host->align_buffer) {
			printf("%s: Aligned buffer alloc failed!!!\n", __func__);
			return -ENOMEM;
		}
	}
#endif

	sdhci_set_power(host, fls(mmc->cfg->voltages) - 1);

	if (host->ops && host->ops->get_cd)
		host->ops->get_cd(host);

	/* Enable only interrupts served by the SD controller */
	sdhci_writel(host, SDHCI_INT_DATA_MASK | SDHCI_INT_CMD_MASK, SDHCI_INT_ENABLE);
	/* Mask all sdhci interrupt sources */
	sdhci_writel(host, 0x0, SDHCI_SIGNAL_ENABLE);

	return 0;
}

#ifdef CONFIG_DM_MMC
int sdhci_probe(struct udevice *dev)
{
	struct mmc *mmc = mmc_get_mmc_dev(dev);

	return sdhci_init(mmc);
}

static int sdhci_deferred_probe(struct udevice *dev)
{
	int err;
	struct mmc *mmc = mmc_get_mmc_dev(dev);
	struct sdhci_host *host = mmc->priv;

	if (host->ops && host->ops->deferred_probe) {
		err = host->ops->deferred_probe(host);
		if (err)
			return err;
	}
	return 0;
}

static int sdhci_get_cd(struct udevice *dev)
{
	struct mmc *mmc = mmc_get_mmc_dev(dev);
	struct sdhci_host *host = mmc->priv;
	int value;

	/* If nonremovable, assume that the card is always present. */
	if (mmc->cfg->host_caps & MMC_CAP_NONREMOVABLE)
		return 1;
	/* If polling, assume that the card is always present. */
	if (mmc->cfg->host_caps & MMC_CAP_NEEDS_POLL)
		return 1;

#if CONFIG_IS_ENABLED(DM_GPIO)
	value = dm_gpio_get_value(&host->cd_gpio);
	if (value >= 0) {
		if (mmc->cfg->host_caps & MMC_CAP_CD_ACTIVE_HIGH)
			return !value;
		else
			return value;
	}
#endif
	value = ! !(sdhci_readl(host, SDHCI_PRESENT_STATE) & SDHCI_CARD_PRESENT);
	if (mmc->cfg->host_caps & MMC_CAP_CD_ACTIVE_HIGH)
		return !value;
	else
		return value;
}

const struct dm_mmc_ops sdhci_ops = {
	.send_cmd = sdhci_send_command,
	.set_ios = sdhci_set_ios,
	.get_cd = sdhci_get_cd,
	.deferred_probe = sdhci_deferred_probe,
#ifdef MMC_SUPPORTS_TUNING
	.execute_tuning = sdhci_execute_tuning,
#endif
};
#else
static const struct mmc_ops sdhci_ops = {
	.send_cmd = sdhci_send_command,
	.set_ios = sdhci_set_ios,
	.init = sdhci_init,
};
#endif

int sdhci_setup_cfg(struct mmc_config *cfg, struct sdhci_host *host, u32 f_max, u32 f_min)
{
	u32 caps, caps_1 = 0;

#if CONFIG_IS_ENABLED(DM_MMC)
	u64 dt_caps, dt_caps_mask;

	dt_caps_mask = dev_read_u64_default(host->mmc->dev, "sdhci-caps-mask", 0);
	dt_caps = dev_read_u64_default(host->mmc->dev, "sdhci-caps", 0);
	caps = ~(u32) dt_caps_mask & sdhci_readl(host, SDHCI_CAPABILITIES);
	caps |= (u32) dt_caps;
#else
	caps = sdhci_readl(host, SDHCI_CAPABILITIES);
#endif
	//printf("%s, caps: 0x%x\n", __func__, caps);

#ifdef CONFIG_MMC_SDHCI_SDMA
	if (!(caps & SDHCI_CAN_DO_SDMA)) {
		printf("%s: Your controller doesn't support SDMA!!\n", __func__);
		return -EINVAL;
	}

	host->flags |= USE_SDMA;
#endif
#if CONFIG_IS_ENABLED(MMC_SDHCI_ADMA)
	if (!(caps & SDHCI_CAN_DO_ADMA2)) {
		printf("%s: Your controller doesn't support SDMA!!\n", __func__);
		return -EINVAL;
	}
	host->adma_desc_table = memalign(ARCH_DMA_MINALIGN, ADMA_TABLE_SZ);

	host->adma_addr = (dma_addr_t) host->adma_desc_table;
#ifdef CONFIG_DMA_ADDR_T_64BIT
	host->flags |= USE_ADMA64;
#else
	host->flags |= USE_ADMA;
#endif
#endif
	if (host->quirks & SDHCI_QUIRK_REG32_RW)
		host->version = sdhci_readl(host, SDHCI_HOST_VERSION - 2) >> 16;
	else
		host->version = sdhci_readw(host, SDHCI_HOST_VERSION);

	cfg->name = host->name;
#ifndef CONFIG_DM_MMC
	cfg->ops = &sdhci_ops;
#endif

	/* Check whether the clock multiplier is supported or not */
	if (SDHCI_GET_VERSION(host) >= SDHCI_SPEC_300) {
#if CONFIG_IS_ENABLED(DM_MMC)
		caps_1 = ~(u32) (dt_caps_mask >> 32) & sdhci_readl(host, SDHCI_CAPABILITIES_1);
		caps_1 |= (u32) (dt_caps >> 32);
#else
		caps_1 = sdhci_readl(host, SDHCI_CAPABILITIES_1);
#endif
		debug("%s, caps_1: 0x%x\n", __func__, caps_1);
		host->clk_mul = (caps_1 & SDHCI_CLOCK_MUL_MASK) >> SDHCI_CLOCK_MUL_SHIFT;
	}

	if (host->max_clk == 0) {
		if (SDHCI_GET_VERSION(host) >= SDHCI_SPEC_300)
			host->max_clk = (caps & SDHCI_CLOCK_V3_BASE_MASK) >> SDHCI_CLOCK_BASE_SHIFT;
		else
			host->max_clk = (caps & SDHCI_CLOCK_BASE_MASK) >> SDHCI_CLOCK_BASE_SHIFT;
		host->max_clk *= 1000000;
		if (host->clk_mul)
			host->max_clk *= host->clk_mul;
	}
	if (host->max_clk == 0) {
		printf("%s: Hardware doesn't specify base clock frequency\n", __func__);
		return -EINVAL;
	}
	if (f_max && (f_max < host->max_clk))
		cfg->f_max = f_max;
	else
		cfg->f_max = host->max_clk;
	if (f_min)
		cfg->f_min = f_min;
	else {
		if (SDHCI_GET_VERSION(host) >= SDHCI_SPEC_300)
			cfg->f_min = cfg->f_max / SDHCI_MAX_DIV_SPEC_300;
		else
			cfg->f_min = cfg->f_max / SDHCI_MAX_DIV_SPEC_200;
	}
	host->timeout_clk = (caps & SDHCI_TIMEOUT_CLK_MASK) >> SDHCI_TIMEOUT_CLK_SHIFT;
	if (caps & SDHCI_TIMEOUT_CLK_UNIT)
		host->timeout_clk *= 1000;
	host->mmc->max_busy_timeout = 1 << 27;
	host->mmc->max_busy_timeout /= host->timeout_clk;
	//printf("\r\n%s: sdhost timeout clk %u KHz, max_busy_timeout %u ms\n",
	//      __FUNCTION__, host->timeout_clk, host->mmc->max_busy_timeout);
	cfg->voltages = 0;
	if (caps & SDHCI_CAN_VDD_330)
		cfg->voltages |= MMC_VDD_32_33 | MMC_VDD_33_34;
	if (caps & SDHCI_CAN_VDD_300)
		cfg->voltages |= MMC_VDD_29_30 | MMC_VDD_30_31;
	if (caps & SDHCI_CAN_VDD_180)
		cfg->voltages |= MMC_VDD_165_195;

	if (host->quirks & SDHCI_QUIRK_BROKEN_VOLTAGE)
		cfg->voltages |= host->voltages;

	cfg->host_caps |= MMC_MODE_HS | MMC_MODE_HS_52MHz | MMC_MODE_4BIT;

	/* Since Host Controller Version3.0 */
	if (SDHCI_GET_VERSION(host) >= SDHCI_SPEC_300) {
		if (!(caps & SDHCI_CAN_DO_8BIT))
			cfg->host_caps &= ~MMC_MODE_8BIT;
	}

	if (host->quirks & SDHCI_QUIRK_BROKEN_HISPD_MODE) {
		cfg->host_caps &= ~MMC_MODE_HS;
		cfg->host_caps &= ~MMC_MODE_HS_52MHz;
	}

	if (!(cfg->voltages & MMC_VDD_165_195) || (host->quirks & SDHCI_QUIRK_NO_1_8_V))
		caps_1 &= ~(SDHCI_SUPPORT_SDR104 | SDHCI_SUPPORT_SDR50 | SDHCI_SUPPORT_DDR50);

	if (caps_1 & (SDHCI_SUPPORT_SDR104 | SDHCI_SUPPORT_SDR50 | SDHCI_SUPPORT_DDR50))
		cfg->host_caps |= MMC_CAP(UHS_SDR12) | MMC_CAP(UHS_SDR25);

	if (caps_1 & SDHCI_SUPPORT_SDR104) {
		cfg->host_caps |= MMC_CAP(UHS_SDR104) | MMC_CAP(UHS_SDR50);
		/*
		 * SD3.0: SDR104 is supported so (for eMMC) the caps2
		 * field can be promoted to support HS200.
		 */
		cfg->host_caps |= MMC_CAP(MMC_HS_200);
	} else if (caps_1 & SDHCI_SUPPORT_SDR50) {
		cfg->host_caps |= MMC_CAP(UHS_SDR50);
	}

	if (caps_1 & SDHCI_SUPPORT_DDR50)
		cfg->host_caps |= MMC_CAP(UHS_DDR50);

	if (host->host_caps)
		cfg->host_caps |= host->host_caps;

	cfg->b_max = CONFIG_SYS_MMC_MAX_BLK_COUNT;

	return 0;
}

#ifdef CONFIG_BLK
int sdhci_bind(struct udevice *dev, struct mmc *mmc, struct mmc_config *cfg)
{
	return mmc_bind(dev, mmc, cfg);
}
#else
int add_sdhci(struct sdhci_host *host, u32 f_max, u32 f_min)
{
	int ret;

	ret = sdhci_setup_cfg(&host->cfg, host, f_max, f_min);
	if (ret)
		return ret;

	host->mmc = mmc_create(&host->cfg, host);
	if (host->mmc == NULL) {
		printf("%s: mmc create fail!\n", __func__);
		return -ENOMEM;
	}

	return 0;
}
#endif

struct sdhci_axera_plat {
	struct mmc_config cfg;
	struct mmc mmc;
};

struct sdhci_axera_phy_cfg {
	const char *property;
	u8 addr;
	u8 reg_sz;
};

static void sdhci_axera_emmc_reset_n_signal(struct sdhci_host *host)
{
	u16 temp_u16, vendor_ptr;

	if (DWC_MSHC_EMMC_BASE != (u64)host->ioaddr)
		return;

	vendor_ptr = sdhci_readw(host, DWC_MSHC_P_VENDOR_SPECIFIC_AREA); //0xe8
	//printf("reg read 0x%llx, val 0x%x\n", (u64)(host->ioaddr + DWC_MSHC_P_VENDOR_SPECIFIC_AREA), vendor_ptr);
	vendor_ptr &= BITS_DWC_MSHC_P_VENDOR_SPECIFIC_AREA_REG_OFFSET_ADDR(0xFFF);
	temp_u16 = sdhci_readw(host, vendor_ptr + EMMC_CTRL_R);
	//printf("reg read 0x%llx, val 0x%x\n", (u64)(host->ioaddr + vendor_ptr + EMMC_CTRL_R), temp_u16);
	//rst_n output enable
	if (0 == (temp_u16 & BIT_DWC_MSHC_EMMC_CTRL_EMMC_RST_N_OE)) {
		temp_u16 |= BIT_DWC_MSHC_EMMC_CTRL_EMMC_RST_N_OE; //bit3
		sdhci_writew(host, temp_u16, vendor_ptr + EMMC_CTRL_R);
		pr_err("reg write 0x%llx, val 0x%x\n", (u64)(host->ioaddr + vendor_ptr + EMMC_CTRL_R), temp_u16);
	}

	//generate rising edge to rst
	temp_u16 &= ~BIT_DWC_MSHC_EMMC_CTRL_EMMC_RST_N;
	sdhci_writew(host, temp_u16, vendor_ptr + EMMC_CTRL_R);
	//printf("reg write 0x%llx, val 0x%x\n", (u64)(host->ioaddr + vendor_ptr + EMMC_CTRL_R), temp_u16);
	udelay(100);
	temp_u16 |= BIT_DWC_MSHC_EMMC_CTRL_EMMC_RST_N;
	sdhci_writew(host, temp_u16, vendor_ptr + EMMC_CTRL_R);
	//printf("reg write 0x%llx, val 0x%x\n", (u64)(host->ioaddr + vendor_ptr + EMMC_CTRL_R), temp_u16);
	//as spec card need longest 1ms
	udelay(1000);
}

static const struct sdhci_axera_phy_cfg sdhci_axera_phy_cfgs[] = {
	{"axera,phy-cnfg", SDHCI_AXERA_PHY_CNFG, sizeof(u32)}
	,
	{"axera,phy-cmdpad-cnfg", SDHCI_AXERA_PHY_CMDPAD_CNFG, sizeof(u16)}
	,
	{"axera,phy-datapad-cnfg", SDHCI_AXERA_PHY_DATAPAD_CNFG, sizeof(u16)}
	,
	{"axera,phy-clkpad-cnfg", SDHCI_AXERA_PHY_CLKPAD_CNFG, sizeof(u16)}
	,
	{"axera,phy-stbpad-cnfg", SDHCI_AXERA_PHY_STBPAD_CNFG, sizeof(u16)}
	,
	{"axera,phy-rstnpad-cnfg", SDHCI_AXERA_PHY_RSTNPAD_CNFG, sizeof(u16)}
	,
	{"axera,phy-commdl-cnfg", SDHCI_AXERA_PHY_COMMDL_CNFG, sizeof(u8)}
	,
	{"axera,phy-sdclkdl-cnfg", SDHCI_AXERA_PHY_SDCLKDL_CNFG, sizeof(u8)}
	,
	{"axera,phy-sdclkdl-dc", SDHCI_AXERA_PHY_SDCLKDL_DC, sizeof(u8)}
	,
	{"axera,phy-smpldl-cnfg", SDHCI_AXERA_PHY_SMPLDL_CNFG, sizeof(u8)}
	,
	{"axera,phy-atdl-cnfg", SDHCI_AXERA_PHY_ATDL_CNFG, sizeof(u8)}
	,
};

static int sdhci_axera_phy_init(struct udevice *dev)
{
	u8 temp_u8;
	u16 temp_u16;
	u32 temp_u32, prop_val;
	int ret, i;
	int cnt = 10;
	struct sdhci_host *host = dev_get_priv(dev);
	unsigned long phy_base = (unsigned long)host->ioaddr + SDHCI_AXERA_PHY_BASE;

	do {
		udelay(100);
		temp_u32 = readl(phy_base + SDHCI_AXERA_PHY_CNFG);
	} while (!(temp_u32 & BIT_MSHC_PHY_CNFG_PHY_PWRGOOD) && (cnt--));

	if (cnt <= 0) {
		pr_err("ax650x phy pwrgood is timeout\n");
		return -1;
	}

	for (i = 0; i < ARRAY_SIZE(sdhci_axera_phy_cfgs); i++) {
		ret = dev_read_u32(dev, sdhci_axera_phy_cfgs[i].property, &prop_val);
		if (ret)
			continue;

		switch (sdhci_axera_phy_cfgs[i].reg_sz) {
		case 1:
			temp_u8 = prop_val & 0xFF;
			writeb(temp_u8, phy_base + sdhci_axera_phy_cfgs[i].addr);
			break;
		case 2:
			temp_u16 = prop_val & 0xFFFF;
			writew(temp_u16, phy_base + sdhci_axera_phy_cfgs[i].addr);
			break;
		case 4:
			writel(prop_val, phy_base + sdhci_axera_phy_cfgs[i].addr);
			break;
		default:
			ret = -1;
			return ret;
		}
	}

	udelay(10);

	//deassert reset
	temp_u32 = readl(phy_base + SDHCI_AXERA_PHY_CNFG);
	temp_u32 |= BIT_MSHC_PHY_CNFG_PHY_RSTN;
	writel(temp_u32, phy_base + SDHCI_AXERA_PHY_CNFG);

	sdhci_axera_emmc_reset_n_signal(host);

	return 0;
}

static int sdhci_axera_bind(struct udevice *dev)
{
	struct sdhci_axera_plat *plat = dev_get_platdata(dev);

	return sdhci_bind(dev, &plat->mmc, &plat->cfg);
}

static int sdhci_axera_probe(struct udevice *dev)
{
	struct mmc_uclass_priv *upriv = dev_get_uclass_priv(dev);
	struct sdhci_axera_plat *plat = dev_get_platdata(dev);
	struct sdhci_host *host = dev_get_priv(dev);
	fdt_addr_t base;
	int ret;
	u16 reg;

	base = devfdt_get_addr(dev);
	if (base == FDT_ADDR_T_NONE)
		return -EINVAL;

	host->ioaddr = (void *)base;
	host->name = dev->name;

	ret = mmc_of_parse(dev, &plat->cfg);
	if (ret)
		return ret;

	host->quirks = SDHCI_QUIRK_WAIT_SEND_CMD;
	if (dev_read_bool(dev, "no-1-8-v"))
		host->quirks |= SDHCI_QUIRK_NO_1_8_V;
	host->mmc = &plat->mmc;
	host->mmc->dev = dev;
	/* max_freq 52Mhz, min_freq 400Khz in uboot */
	ret = sdhci_setup_cfg(&plat->cfg, host, 52000000, 400000);
	if (ret)
		return ret;

	upriv->mmc = &plat->mmc;
	host->mmc->priv = host;

	ret = sdhci_probe(dev);
	if (ret)
		return ret;

	if (plat->cfg.host_caps & MMC_CAP_NONREMOVABLE) {
		reg = sdhci_readw(host, SDHCI_HOST_CONTROL2);
		/* eMMC support 3.3V and 1.8V */
		if (!dev_read_bool(dev, "no-1-8-v"))
			reg |= SDHCI_CTRL_VDD_180;
		sdhci_writew(host, reg, SDHCI_HOST_CONTROL2);
	}
	pr_err("sdhci_axera_probe all sdhci_axera_phy_init\n");

	//phy init after sdhci_probe or phy will reset
	return sdhci_axera_phy_init(dev);
}

static const struct udevice_id sdhci_axera_match[] = {
	{.compatible = "axera,ax650-sdhc"},
};

U_BOOT_DRIVER(sdhci_axera) = {
	.name = "sdhci-axera",.id = UCLASS_MMC,.of_match = sdhci_axera_match,.bind = sdhci_axera_bind,.probe =
	sdhci_axera_probe,.priv_auto_alloc_size = sizeof(struct sdhci_host),.platdata_auto_alloc_size =
	sizeof(struct sdhci_axera_plat),.ops = &sdhci_ops,
};
