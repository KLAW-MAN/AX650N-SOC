#include <common.h>
#include <asm/armv8/mmu.h>
#include <asm/io.h>
#include <sdhci.h>
#include <malloc.h>
#include <asm/arch/ax650.h>
#include <asm/arch/boot_mode.h>

#define SDIO_REG_BASE_PHY 0x28001000
#define SD_REG_BASE_PHY 0x30E00000
#define EMMC_REG_BASE_PHY 0x28000000
#define REG_SIZE 0x1000

#define SDHCI_AXERA_PHY_BASE           0x300
/* PHY */
#define SDHCI_AXERA_PHY_CNFG           (0x00 + SDHCI_AXERA_PHY_BASE)
#define SDHCI_AXERA_PHY_CMDPAD_CNFG    (0x04 + SDHCI_AXERA_PHY_BASE)
#define SDHCI_AXERA_PHY_DATAPAD_CNFG   (0x06 + SDHCI_AXERA_PHY_BASE)
#define SDHCI_AXERA_PHY_CLKPAD_CNFG    (0x08 + SDHCI_AXERA_PHY_BASE)
#define SDHCI_AXERA_PHY_STBPAD_CNFG    (0x0A + SDHCI_AXERA_PHY_BASE)
#define SDHCI_AXERA_PHY_RSTNPAD_CNFG   (0x0C + SDHCI_AXERA_PHY_BASE)
#define SDHCI_AXERA_PHY_COMMDL_CNFG    (0x1C + SDHCI_AXERA_PHY_BASE)
#define SDHCI_AXERA_PHY_SDCLKDL_CNFG   (0x1D + SDHCI_AXERA_PHY_BASE)
#define SDHCI_AXERA_PHY_SDCLKDL_DC     (0x1E + SDHCI_AXERA_PHY_BASE)
#define SDHCI_AXERA_PHY_SMPLDL_CNFG    (0x20 + SDHCI_AXERA_PHY_BASE)
#define SDHCI_AXERA_PHY_ATDL_CNFG      (0x21 + SDHCI_AXERA_PHY_BASE)
#define SDHCI_AXERA_PHY_DLL_CTRL       (0x24 + SDHCI_AXERA_PHY_BASE)
#define SDHCI_AXERA_PHY_DLL_CNFG1      (0x25 + SDHCI_AXERA_PHY_BASE)
#define SDHCI_AXERA_PHY_DLL_CNFG2      (0x26 + SDHCI_AXERA_PHY_BASE)
#define SDHCI_AXERA_PHY_DLLDL_CNFG     (0x28 + SDHCI_AXERA_PHY_BASE)
#define SDHCI_AXERA_PHY_DLL_OFFSET     (0x29 + SDHCI_AXERA_PHY_BASE)
#define SDHCI_AXERA_PHY_DLLLBT_CNFG    (0x2C + SDHCI_AXERA_PHY_BASE)
#define SDHCI_AXERA_PHY_DLL_STATUS     (0x2E + SDHCI_AXERA_PHY_BASE)
#define SDHCI_AXERA_PHY_DLLDBG_MLKDC   (0x30 + SDHCI_AXERA_PHY_BASE)


#define READ_TEST_SIZE  0x800 //2M
#define READ_TEST_ADDR1 0x100000000
#define READ_TEST_ADDR2 (READ_TEST_ADDR1 + READ_TEST_SIZE)
#define READ_TEST_ADDR3 (READ_TEST_ADDR2 + READ_TEST_SIZE)
#define READ_TEST_ADDR4 (READ_TEST_ADDR3 + READ_TEST_SIZE)

static int curr_device = 0;
static u32 reg_handle_mem;
static struct mmc *init_mmc_device(int dev, bool force_init)
{
	struct mmc *mmc;
	mmc = find_mmc_device(dev);
	if (!mmc) {
		printf("no mmc device at slot %x\n", dev);
		return NULL;
	}

	if (!mmc_getcd(mmc))
		force_init = true;

	if (force_init)
		mmc->has_init = 0;
	if (mmc_init(mmc))
		return NULL;

#ifdef CONFIG_BLOCK_CACHE
	struct blk_desc *bd = mmc_get_blk_desc(mmc);
	blkcache_invalidate(bd->if_type, bd->devnum);
#endif

	return mmc;
}

static int do_mmc_read(u32 blk, u32 cnt, void *addr)
{
	struct mmc *mmc;
	u32 n;

	mmc = init_mmc_device(curr_device, false);
	if (!mmc)
		return CMD_RET_FAILURE;

	printf("\nMMC read: dev # %d, block # %d, count %d ... ",
	       curr_device, blk, cnt);

	n = blk_dread(mmc_get_blk_desc(mmc), blk, cnt, addr);
	printf("%d blocks read: %s\n", n, (n == cnt) ? "OK" : "ERROR");

	return (n == cnt) ? CMD_RET_SUCCESS : CMD_RET_FAILURE;
}

static int do_mmc_write(u32 blk, u32 cnt, void *addr)
{
	struct mmc *mmc;
	u32  n;

	mmc = init_mmc_device(curr_device, false);
	if (!mmc)
		return CMD_RET_FAILURE;

	printf("\nMMC write: dev # %d, block # %d, count %d ... ",
	       curr_device, blk, cnt);

	if (mmc_getwp(mmc) == 1) {
		printf("Error: card is write protected!\n");
		return CMD_RET_FAILURE;
	}
	n = blk_dwrite(mmc_get_blk_desc(mmc), blk, cnt, addr);
	printf("%d blocks written: %s\n", n, (n == cnt) ? "OK" : "ERROR");

	return (n == cnt) ? CMD_RET_SUCCESS : CMD_RET_FAILURE;
}


static void read_phy_reg(void)
{
	printf("SDHCI_AXERA_PHY_COMMDL_CNFG: 0x%x\n", readb((u64)reg_handle_mem + SDHCI_AXERA_PHY_COMMDL_CNFG));
	printf("SDHCI_AXERA_PHY_SDCLKDL_CNFG: 0x%x\n", readb((u64)reg_handle_mem + SDHCI_AXERA_PHY_SDCLKDL_CNFG));
	printf("SDHCI_AXERA_PHY_SDCLKDL_DC: 0x%x\n", readb((u64)reg_handle_mem + SDHCI_AXERA_PHY_SDCLKDL_DC));
	printf("SDHCI_AXERA_PHY_SMPLDL_CNFG: 0x%x\n", readb((u64)reg_handle_mem + SDHCI_AXERA_PHY_SMPLDL_CNFG));
	printf("SDHCI_AXERA_PHY_ATDL_CNFG: 0x%x\n", readb((u64)reg_handle_mem + SDHCI_AXERA_PHY_ATDL_CNFG));
}

#define SDHCI_CLOCK_CONTROL 0x2C
static void clk_set(int enable)
{
	int clk;
	clk = readw((u64)reg_handle_mem + SDHCI_CLOCK_CONTROL);
	if (enable)
		clk |= SDHCI_CLOCK_CARD_EN;
	else
		clk &= ~SDHCI_CLOCK_CARD_EN;
	writew(clk, (u64)reg_handle_mem + SDHCI_CLOCK_CONTROL);
}
static void emmc_scan_phy_set_write(int fix_delay, int i)
{
	clk_set(0);
	writeb(0x0, (u64)reg_handle_mem + SDHCI_AXERA_PHY_COMMDL_CNFG);
	writeb(fix_delay, (u64)reg_handle_mem + SDHCI_AXERA_PHY_SDCLKDL_CNFG);
	writeb(i, (u64)reg_handle_mem + SDHCI_AXERA_PHY_SDCLKDL_DC);
	writeb(0x8, (u64)reg_handle_mem + SDHCI_AXERA_PHY_SMPLDL_CNFG);
	writeb(0xC, (u64)reg_handle_mem + SDHCI_AXERA_PHY_ATDL_CNFG);
	clk_set(1);
}

static void emmc_run_test(void)
{
	/* test address */
	void *read_addr1 = (void *)READ_TEST_ADDR1;
	void *read_addr2 = (void *)READ_TEST_ADDR2;
	void *read_addr3 = (void *)READ_TEST_ADDR3;
	void *read_addr4 = (void *)READ_TEST_ADDR4;
	printf("Note: This test program may damage the uboot and param partitions. If it is damaged, please burn the version again.\n");
	/* read 1M data in uboot partition to test addr */
	do_mmc_read(0x0, READ_TEST_SIZE, read_addr1);
	do_mmc_read(0x0, READ_TEST_SIZE, read_addr2);
	if(0 == memcmp(read_addr1, read_addr2, READ_TEST_SIZE)) {
		printf("read data compare success\n");
	} else {
		printf("read data compare failed\n");
	}
	/* read/write 1M data in param partition to test addr */
	do_mmc_write(0x2000, READ_TEST_SIZE, read_addr1);
	do_mmc_read(0x2000, READ_TEST_SIZE, read_addr3);
	do_mmc_write(0x2000, READ_TEST_SIZE, read_addr2);
	do_mmc_read(0x2000, READ_TEST_SIZE, read_addr4);
	if(0 == memcmp(read_addr3, read_addr4, READ_TEST_SIZE)) {
		printf("write data and read data compare success\n");
	} else {
		printf("write data and read data compare failed\n");
	}
}

int do_emmc_scan(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	int i = 0, j = 0;
	reg_handle_mem = EMMC_REG_BASE_PHY;
	printf("enter do_emmc_scan_test\n");
	for (j = 0; j < 2; j++) {
		for (i = 0; i < 128; i++) {
			emmc_scan_phy_set_write(j, i);
			read_phy_reg();
			printf("================step:%d\n", i);
			emmc_run_test();
		}
	}
	read_phy_reg();
	return 0;
}

U_BOOT_CMD(
    emmc_scan, 4, 0, do_emmc_scan,
    "integrated emmc scan",
    "<addr> <len> [loops]"
);

