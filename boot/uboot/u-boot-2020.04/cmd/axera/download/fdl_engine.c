/*
 * (C) Copyright 2020 AXERA
 *
 * SPDX-License-Identifier:     GPL-2.0+
 */

#include <common.h>
#include <malloc.h>
#include <mmc.h>
#include <mtd.h>
#include <blk.h>
#include <memalign.h>
#include <linux/sizes.h>
#include <linux/string.h>
#include <fdl_engine.h>
#include <fdl_frame.h>
#include <fdl_channel.h>
#include <image-sparse.h>
#include <asm/arch/boot_mode.h>
#include <configs/ax650_common.h>
#include <dm/uclass.h>
#include <dm/device.h>
#include <jffs2/load_kernel.h>
#include <linux/mtd/spi-nor.h>
#include "../../legacy-mtd-utils.h"
#include "../secureboot/secureboot.h"

#define AXERA_DISK_GUID_STR      "12345678-1234-1234-1234-000000000000"

#define SIZE_SPL_IMG_PADDING			(128 * 1024)
#define SIZE_SPL_IMG_SIZE				(111 * 1024)

extern FDL_ChannelHandler_T *g_CurrChannel;
extern int get_part_info(struct blk_desc *dev_desc, const char *name, disk_partition_t * info);
extern void reboot(void);
typedef int (*cmd_handler_t) (fdl_frame_t * pframe, void *arg);

struct fdl_cmdproc {
	//CMD_TYPE cmd;
	cmd_handler_t handler;
	void *arg;
};

extern struct boot_mode_info boot_info_data;

#define CMD_HANDLER(cmd, pframe) cmdproc_tab[cmd - FDL_CMD_TYPE_MIN].handler\
						(pframe, cmdproc_tab[cmd - FDL_CMD_TYPE_MIN].arg)
#define FILE_START_ADDR_SIZE             (sizeof(u64))
#define FILE_TARGET_LEN_SIZE             (sizeof(u64))

char lastXferPart[32] = { '@' };
char lastLoadPart[32] = { '@' };
u32 xferPartCnt = 0;
loff_t partOff = 0;
u_long dl_buf_addr = FDL_BUF_ADDR;
u_long dl_buf_size = FDL_BUF_LEN;
static int sparse_download_enable = 0;

struct sparse_storage sparse;
struct fdl_cmdproc cmdproc_tab[FDL_CMD_TYPE_MAX - FDL_CMD_TYPE_MIN];
struct fdl_file_info g_file_info;
struct fdl_read_info g_read_info;
extern int sparse_info_init(struct sparse_storage *info, const char *name);
extern int write_sparse_img(struct sparse_storage *info, const char *part_name, void *data, ulong *response);
extern void dump_buffer(u64 addr, u64 len);
u32 fdl_checksum32(u32 chksum, u8 const *pdata, u32 len)
{
	u32 i;

	for (i = 0; i < len; i++) {
		chksum += (u8) (*(pdata + i));
	}

	return chksum;
}

static u32 calc_image_checkSum(u8 * buf, u32 len)
{
	u32 chkSum = 0;
	u32 aligned = (len & ~0x3);
	u32 remaining = (len & 0x3);

	for (int i = 0; i < aligned / sizeof(u32); i++) {
		chkSum += ((u32 *) buf)[i];
	}

	for (int i = 0; i < remaining; i++) {
		chkSum += *(buf + aligned + i);
	}

	return chkSum;
}

int repatition_emmc_handle(fdl_partition_t * fdl_part_table, u16 part_count)
{
	/*
	 * add code distinguish emmc/spi nand/nor
	 * if emmc used, dispath to different function, refact later
	 */
	u16 i, j, temp;
	u32 part_table_len;
	ulong blksz;
	disk_partition_t *partitions = NULL;
	struct blk_desc *blk_dev_desc = NULL;
	u64 disk_part_start_lba = 0;
	u64 disk_part_size_lb, disk_part_gap_lb;
	int ret;

	part_table_len = part_count * sizeof(disk_partition_t);
	partitions = (disk_partition_t *) malloc(part_table_len);
	if (partitions == NULL) {
		pr_err("fdl_repartition_handle alloc disk_partition_t error\n");
		return -1;
	}
	memset(partitions, 0, part_table_len);

	blk_dev_desc = blk_get_dev("mmc", EMMC_DEV_ID);
	if (!blk_dev_desc) {
		pr_err("get mmc dev fail\n");
		ret = -1;
		goto RET;
	}
	blksz = blk_dev_desc->blksz;

	for (i = 0; i < part_count; i++) {
		partitions[i].blksz = blksz;
		partitions[i].bootable = 0;
		for (j = 0; j < 31; j++) {
			temp = *(u16 *) (fdl_part_table[i].part_id + j);
			if (!temp)
				break;
			partitions[i].name[j] = (u8) temp;
		}

		if (fdl_part_table[i].sizeof_bytes != 0)
			disk_part_size_lb = PAD_COUNT(fdl_part_table[i].sizeof_bytes, blksz);
		else
			disk_part_size_lb = 0;

		if (fdl_part_table[i].gapof_bytes != 0)
			disk_part_gap_lb = PAD_COUNT(fdl_part_table[i].gapof_bytes, blksz);
		else
			disk_part_gap_lb = 0;

		/* if first partition we start from fixed 1MB of disk */
		if (0 == i)
			disk_part_gap_lb = SZ_1M / blksz;
		/* only the last partition can use the all rest blks */
		if (fdl_part_table[i].part_sizeof_unit == 0xffffffff) {
			if (i == (part_count - 1)) {
				disk_part_size_lb = 0;
			} else {
				pr_err("not the last partition cannot use all rest blks\n");
				ret = -1;
				goto RET;
			}
		}
		disk_part_start_lba += disk_part_gap_lb;
		partitions[i].start = disk_part_start_lba;
		partitions[i].size = disk_part_size_lb;
		disk_part_start_lba += disk_part_size_lb;
#if CONFIG_IS_ENABLED(PARTITION_UUIDS)
#ifdef CONFIG_RANDOM_UUID
		gen_rand_uuid_str(partitions[i].uuid, UUID_STR_FORMAT_STD);
#else
		pr_err("plese set uuid\n");
#endif
#endif
		/* not must need */
#if 0
		partitions[i].sys_ind =;
		partitions[i].type =;
#endif

	}

	ret = gpt_restore(blk_dev_desc, AXERA_DISK_GUID_STR, partitions, part_count);

RET:
	free(partitions);
	return ret;
}

#define PARTITION_BOOT1		1
#define PARTITION_BOOT2		2

int get_emmc_part_info(char *part_name, u64 * addr, u64 * len)
{
	int ret = 0;
	struct blk_desc *blk_dev_desc = NULL;
	disk_partition_t part_info;
#if CONFIG_IS_ENABLED(BLK)
	struct mmc_uclass_priv *upriv;
	struct mmc *mmc;
	(void)mmc;
	(void)upriv;
#endif

	blk_dev_desc = blk_get_dev("mmc", EMMC_DEV_ID);
	if (!blk_dev_desc) {
		printf("get mmc dev fail\n");
		return -1;
	}
#ifdef USE_BOOT_PARTITION
	if (!strcmp(part_name, "spl") || !strcmp(part_name, "uboot")) {
		*addr = 0;
#if CONFIG_IS_ENABLED(BLK)
		if (!blk_dev_desc->bdev) {
			printf("[%s]ERROR: blk_dev_desc->bdev is null\n", __FUNCTION__);
			return -1;
		}
		if (!blk_dev_desc->bdev->parent) {
			printf("[%s]ERROR: blk_dev_desc->bdev->parent is null\n", __FUNCTION__);
			return -1;
		}
		upriv = blk_dev_desc->bdev->parent->uclass_priv;
		if (!upriv || !upriv->mmc) {
			printf("[%s]ERROR: parent->uclass_priv or uclass_priv->mmc is null\n", __FUNCTION__);
			return -1;
		}
		mmc = upriv->mmc;
		*len = mmc->capacity_boot;
#else
		printf("[%s]ERROR: CONFIG_IS_ENABLED(BLK) is not config\n", __FUNCTION__);
		return -1;
#endif
	}
#endif
#ifdef USE_GPT_PARTITON
	ret = part_get_info_by_name(blk_dev_desc, part_name, &part_info);
#else
	/* use bootargs */
	ret = get_part_info(blk_dev_desc, part_name, &part_info);
#endif
	if (ret == -1) {
		printf("%s: get %s partition info fail\n", __FUNCTION__, part_name);
	}
	*addr = (u64)part_info.start * blk_dev_desc->blksz;
	*len = (u64)part_info.size * blk_dev_desc->blksz;
	ret = 0;

	return ret;
}

int get_spinand_part_info(char *part_name, u64 * addr, u64 * len)
{
	int ret = -1;
	int idx;
	u32 busnum = 0;
	loff_t off = 0;
	loff_t size, maxsize;
	struct udevice *dev = NULL;
	struct mtd_info *mtd = NULL;

	ret = uclass_get_device(UCLASS_MTD, busnum, &dev);
	if (ret) {
		printf("uclass_get_device: Invalid bus %d (err=%d)\n", busnum, ret);
		return ret;
	}

	mtd = dev_get_uclass_priv(dev);

	ret = mtd_arg_off(part_name, &idx, &off, &size, &maxsize, MTD_DEV_TYPE_NAND, mtd->size);
	if (ret)
		return ret;

	*addr = off;
	*len = size;

	return ret;
}

int get_spinor_part_info(char *part_name, u64 * addr, u64 * len)
{
	int ret = -1;
	int idx;
	u32 busnum = 0;
	loff_t off = 0;
	loff_t size, maxsize;
	struct udevice *dev = NULL;
	struct mtd_info *mtd = NULL;

	ret = uclass_get_device(UCLASS_SPI_FLASH, busnum, &dev);
	if (ret) {
		printf("uclass_get_device: Invalid bus %d (err=%d)\n", busnum, ret);
		return ret;
	}
	mtd = get_mtd_device_nm("nor0");
	mtd = dev_get_uclass_priv(dev);

	ret = mtd_arg_off(part_name, &idx, &off, &size, &maxsize, MTD_DEV_TYPE_NOR, mtd->size);
	if (ret)
		return ret;

	*addr = off;
	*len = size;

	return ret;
}

int get_ssd_part_info(char *part_name, u64 * addr, u64 * len)
{
	int ret = 0;
	struct blk_desc *blk_dev_desc = NULL;
	disk_partition_t part_info;

	if (!strcmp(part_name, "spl") || !strcmp(part_name, "uboot")
		|| !strcmp(part_name, "uboot_bk") || !strcmp(part_name, "env")) {
		env_set("mtdids", MTDIDS_SPINOR);
		env_set("mtdparts", MTDPARTS_SPINOR);
		return get_spinor_part_info(part_name, addr, len);
	} else {
		blk_dev_desc = blk_get_dev("nvme", 0);
		if (!blk_dev_desc) {
			printf("get nvme dev fail\n");
			return -1;
		}
	}

	/* use bootargs */
	ret = get_part_info(blk_dev_desc, part_name, &part_info);
	if (ret == -1) {
		printf("%s: get %s partition info fail\n", __FUNCTION__, part_name);
	}
	*addr = (u64)part_info.start * blk_dev_desc->blksz;
	*len = (u64)part_info.size * blk_dev_desc->blksz;
	ret = 0;

	return ret;
}

int common_get_part_info(char *part_name, u64 * addr, u64 * len)
{
	int ret = -1;
	switch (boot_info_data.storage_sel) {
	case STORAGE_TYPE_EMMC:
		ret = get_emmc_part_info(part_name, addr, len);
		break;
	case STORAGE_TYPE_NAND:
		ret = get_spinand_part_info(part_name, addr, len);
		break;
	case STORAGE_TYPE_NOR:
		env_set("mtdids", MTDIDS_SPINOR);
		env_set("mtdparts", MTDPARTS_SPINOR);
		ret = get_spinor_part_info(part_name, addr, len);
		break;
	case STORAGE_TYPE_SSD:
		ret = get_ssd_part_info(part_name, addr, len);
		break;
	default:
		break;
	}
	if (0 == ret) {
		printf("%s: part %s, base addr 0x%llX, part size 0x%llX\n", __FUNCTION__, part_name, *addr, *len);
	} else {
		printf("[ERROR]%s: part %s not found\n", __FUNCTION__, part_name);
	}

	return ret;
}

int fdl_read_from_emmc(char *part_name, u64 addr, u64 len, char *buffer)
{
	lbaint_t start_lba, blkcnt_lb;
	lbaint_t rd_blkcnt_lb;
	struct blk_desc *blk_dev_desc = NULL;
	ulong blksz;
	ulong last_len;
#ifdef USE_BOOT_PARTITION
	int ret;
#endif
	//disk_partition_t part_info;
	char *temp_data = NULL;

	blk_dev_desc = blk_get_dev("mmc", EMMC_DEV_ID);
	if (!blk_dev_desc) {
		pr_err("get mmc dev fail\n");
		return -1;
	}
	blksz = blk_dev_desc->blksz;
	if (blksz != 512) {
		printf("%s: blksz 0x%lX is error\n", __FUNCTION__, blksz);
		return -1;
	}
	if (addr % blksz) {
		printf("%s: addr 0x%llX not align\n", __FUNCTION__, addr);
		return -1;
	}
	start_lba = addr / blksz;
	if (len % blksz) {
		blkcnt_lb = len / blksz;
		last_len = len - blkcnt_lb * blksz;
	} else {
		blkcnt_lb = PAD_COUNT(len, blksz);
		last_len = 0;
	}
	printf("[%s]addr 0x%llX, len 0x%llX, start lba 0x%lX, blkcnt 0x%lX, last bytes 0x%lX\n",
	       __FUNCTION__, addr, len, start_lba, blkcnt_lb, last_len);

#ifdef USE_BOOT_PARTITION
	/*
	 * spl and eip bin have backup and store in boot_parttion1
	 * uboot store in boot_partition2
	 * others store in user_partition with gpt table
	 */
	if (!strcmp(part_name, "spl")) {
		ret = blk_dselect_hwpart(blk_dev_desc, PARTITION_BOOT1);
		if (ret) {
			printf("Failed to select h/w partition 1\n");
			return -1;
		}
	} else if (!strcmp(part_name, "uboot")) {
		ret = blk_dselect_hwpart(blk_dev_desc, PARTITION_BOOT2);
		if (ret) {
			printf("Failed to select h/w partition 2\n");
			return -1;
		}
	}
#endif

#if CONFIG_IS_ENABLED(BLK)
	rd_blkcnt_lb = blk_dread(blk_dev_desc, start_lba, blkcnt_lb, (void *)buffer);
	if (rd_blkcnt_lb != blkcnt_lb) {
		printf("some error happend while read from disk\n");
		return -1;
	}
	if (last_len) {
		temp_data = malloc(blksz);
		if (temp_data == NULL) {
			printf("%s malloc %lX bytes fail\n", __FUNCTION__, blksz);
			return -1;
		}
		if (blk_dread(blk_dev_desc, (start_lba + blkcnt_lb), 1, (void *)temp_data) == 1) {
			memcpy((void *)(buffer + blkcnt_lb * blksz), (void *)temp_data, last_len);
		} else {
			printf("some error happend while read from disk\n");
			return -1;
		}
	}
#else
	rd_blkcnt_lb = blk_dev_desc->block_read(blk_dev_desc, start_lba, blkcnt_lb, (void *)buffer);
	if (rd_blkcnt_lb != blkcnt_lb) {
		printf("some error happend while read from disk\n");
		return -1;
	}
	if (last_len) {
		temp_data = malloc(blksz);
		if (temp_data == NULL) {
			printf("%s malloc 0x%lX bytes fail\n", __FUNCTION__, blksz);
			return -1;
		}
		if (blk_dev_desc->block_read(blk_dev_desc, (start_lba + blkcnt_lb), 1, (void *)temp_data) == 1) {
			memcpy((void *)(buffer + blkcnt_lb * blksz), (void *)temp_data, last_len);
		} else {
			printf("some error happend while read from disk\n");
			return -1;
		}
	}
#endif

	return (rd_blkcnt_lb * blksz + last_len);
}

int fdl_read_from_ssd(char *part_name, u64 addr, u64 len, char *buffer)
{
	lbaint_t start_lba, blkcnt_lb;
	lbaint_t rd_blkcnt_lb;
	struct blk_desc *blk_dev_desc = NULL;
	ulong blksz;
	ulong last_len;
#ifdef USE_BOOT_PARTITION
	int ret;
#endif

	if (!strcmp(part_name, "spl") || !strcmp(part_name, "uboot")
		|| !strcmp(part_name, "uboot_bk") || !strcmp(part_name, "env")) {
			return -1;
	}
	//disk_partition_t part_info;
	char *temp_data = NULL;

	blk_dev_desc = blk_get_dev("nvme", 0);
	if (!blk_dev_desc) {
		pr_err("get nvme dev fail\n");
		return -1;
	}
	blksz = blk_dev_desc->blksz;
	if (blksz != 512) {
		printf("%s: blksz 0x%lX is error\n", __FUNCTION__, blksz);
		return -1;
	}
	if (addr % blksz) {
		printf("%s: addr 0x%llX not align\n", __FUNCTION__, addr);
		return -1;
	}
	start_lba = addr / blksz;
	if (len % blksz) {
		blkcnt_lb = len / blksz;
		last_len = len - blkcnt_lb * blksz;
	} else {
		blkcnt_lb = PAD_COUNT(len, blksz);
		last_len = 0;
	}
	printf("[%s]addr 0x%llX, len 0x%llX, start lba 0x%lX, blkcnt 0x%lX, last bytes 0x%lX\n",
	       __FUNCTION__, addr, len, start_lba, blkcnt_lb, last_len);

#ifdef USE_BOOT_PARTITION
	/*
	 * spl and eip bin have backup and store in boot_parttion1
	 * uboot store in boot_partition2
	 * others store in user_partition with gpt table
	 */
	if (!strcmp(part_name, "spl")) {
		ret = blk_dselect_hwpart(blk_dev_desc, PARTITION_BOOT1);
		if (ret) {
			printf("Failed to select h/w partition 1\n");
			return -1;
		}
	} else if (!strcmp(part_name, "uboot")) {
		ret = blk_dselect_hwpart(blk_dev_desc, PARTITION_BOOT2);
		if (ret) {
			printf("Failed to select h/w partition 2\n");
			return -1;
		}
	}
#endif

#if CONFIG_IS_ENABLED(BLK)
	rd_blkcnt_lb = blk_dread(blk_dev_desc, start_lba, blkcnt_lb, (void *)buffer);
	if (rd_blkcnt_lb != blkcnt_lb) {
		printf("some error happend while read from disk\n");
		return -1;
	}
	if (last_len) {
		temp_data = malloc(blksz);
		if (temp_data == NULL) {
			printf("%s malloc %lX bytes fail\n", __FUNCTION__, blksz);
			return -1;
		}
		if (blk_dread(blk_dev_desc, (start_lba + blkcnt_lb), 1, (void *)temp_data) == 1) {
			memcpy((void *)(buffer + blkcnt_lb * blksz), (void *)temp_data, last_len);
		} else {
			printf("some error happend while read from disk\n");
			return -1;
		}
	}
#else
	rd_blkcnt_lb = blk_dev_desc->block_read(blk_dev_desc, start_lba, blkcnt_lb, (void *)buffer);
	if (rd_blkcnt_lb != blkcnt_lb) {
		printf("some error happend while read from disk\n");
		return -1;
	}
	if (last_len) {
		temp_data = malloc(blksz);
		if (temp_data == NULL) {
			printf("%s malloc 0x%lX bytes fail\n", __FUNCTION__, blksz);
			return -1;
		}
		if (blk_dev_desc->block_read(blk_dev_desc, (start_lba + blkcnt_lb), 1, (void *)temp_data) == 1) {
			memcpy((void *)(buffer + blkcnt_lb * blksz), (void *)temp_data, last_len);
		} else {
			printf("some error happend while read from disk\n");
			return -1;
		}
	}
#endif

	return (rd_blkcnt_lb * blksz + last_len);
}

//#define DEBUG_SPARSE_DL_DATA_CHECK
#ifdef DEBUG_SPARSE_DL_DATA_CHECK
#define READ_BACK_CHECK_ADDR		0x100000000
#define READ_BACK_CHECK_SIZE_LIMIT	0x6400000
int emmc_read_back_check(struct blk_desc *blk_dev_desc, lbaint_t start_lba, lbaint_t blkcnt_lb, const void *buffer)
{
	lbaint_t rd_blkcnt_lb;
	ulong blksz;
	char *read_data = NULL;

	blksz = blk_dev_desc->blksz;
	if (blksz != 512) {
		printf("[ERROR]%s: blksz 0x%lX is error\n", __FUNCTION__, blksz);
		return -1;
	}
	if (blkcnt_lb * blksz <= READ_BACK_CHECK_SIZE_LIMIT) {
		rd_blkcnt_lb = blk_dread(blk_dev_desc, start_lba, blkcnt_lb, (void *)READ_BACK_CHECK_ADDR);
		if (rd_blkcnt_lb != blkcnt_lb) {
			printf("%s: some error happend while reading disk\n", __FUNCTION__);
			return -1;
		}
		read_data = (char *)0x430000000;
		if (memcmp((void *)buffer, (void *)read_data, (blkcnt_lb * blksz))) {
			if (blkcnt_lb < 0x20) {
				dump_buffer((u64) buffer, (u64) (blkcnt_lb * blksz));
				printf("\r\n[ERROR]blk_dread: start_lba 0x%lX, blkcnt_lb 0x%lX\n", start_lba,
				       blkcnt_lb);
				dump_buffer((u64) read_data, (u64) (blkcnt_lb * blksz));
			} else {
				printf("\r\n[ERROR]blk_dread: start_lba 0x%lX, blkcnt_lb 0x%lX, buffer addr 0x%llX\n",
				       start_lba, blkcnt_lb, (u64) buffer);
			}
			return -1;
		}
	} else {
		printf("\r\n[ERROR]read back check request size 0x%lX over limit\n", blkcnt_lb * blksz);
	}
	return 0;
}
#endif

int emmc_write(char *part_name, u64 addr, u64 len, char *buffer)
{
	lbaint_t start_lba, blkcnt_lb;
	lbaint_t wr_blkcnt_lb, grp_wr_blkcnt_lb;
	struct blk_desc *blk_dev_desc = NULL;
	ulong blksz;
	ulong last_len;
	ulong grp_blks, unsaved_blks;
	char *temp_data = NULL;

	blk_dev_desc = blk_get_dev("mmc", EMMC_DEV_ID);
	if (!blk_dev_desc) {
		pr_err("get mmc dev fail\n");
		return -1;
	}
	blksz = blk_dev_desc->blksz;
	if (blksz != 512) {
		printf("%s: blksz 0x%lX is error\n", __FUNCTION__, blksz);
		return -1;
	}
	if (addr % blksz) {
		printf("%s: addr 0x%llX not align\n", __FUNCTION__, addr);
		return -1;
	}
	start_lba = (addr / blksz) & 0xffffffff;
	blkcnt_lb = (len / blksz) & 0xffffffff;
	if (len % blksz) {
		last_len = len - blkcnt_lb * blksz;
	} else {
		last_len = 0;
	}
	wr_blkcnt_lb = 0;

	/*
	 * spl and eip bin have backup and store in boot_parttion1
	 * uboot store in boot_partition2
	 * others store in user_partition with gpt table
	 */
#ifdef USE_BOOT_PARTITION
	if (!strcmp(part_name, "spl")) {
		ret = blk_dselect_hwpart(blk_dev_desc, PARTITION_BOOT1);
		if (ret) {
			printf("%s: Failed to select h/w partition 1\n", __FUNCTION__);
			return -1;
		}
	} else if (!strcmp(part_name, "uboot")) {
		ret = blk_dselect_hwpart(blk_dev_desc, PARTITION_BOOT2);
		if (ret) {
			printf("%s: Failed to select h/w partition 2\n", __FUNCTION__);
			return -1;
		}
	}
#endif

#if CONFIG_IS_ENABLED(BLK)
	if ((u64) buffer % 0x1000 == 0) {
		wr_blkcnt_lb = blk_dwrite(blk_dev_desc, start_lba, blkcnt_lb, (void *)buffer);
		if (wr_blkcnt_lb != blkcnt_lb) {
			printf("%s: some error happend while writing disk\n", __FUNCTION__);
			return -1;
		}
	} else {
		temp_data = (char *)DL_WRITE_TEMP_BUF_ADDR;
		unsaved_blks = blkcnt_lb;
		while (unsaved_blks != 0) {
			grp_blks = ((unsaved_blks > SDHCI_ALIGN_BUF_BLKS) ? SDHCI_ALIGN_BUF_BLKS : unsaved_blks);
			memcpy((void *)temp_data, (void *)buffer, (grp_blks * blksz));
			grp_wr_blkcnt_lb =
			    blk_dwrite(blk_dev_desc, (start_lba + wr_blkcnt_lb), grp_blks, (void *)temp_data);
			if (grp_wr_blkcnt_lb != grp_blks) {
				printf("%s: some error happend while writing disk\n", __FUNCTION__);
				return -1;
			}
			unsaved_blks -= grp_blks;
			wr_blkcnt_lb += grp_blks;
			buffer += grp_blks * blksz;
		}
	}
#ifdef DEBUG_SPARSE_DL_DATA_CHECK
	if (emmc_read_back_check(blk_dev_desc, start_lba, blkcnt_lb, (void *)buffer)) {
		printf("[ERROR]%s: emmc_read_back_check fail\n", __FUNCTION__);
		return -1;
	}
#endif
	if (last_len) {
		printf("%s: last %lX bytes\n", __FUNCTION__, last_len);
		temp_data = (char *)DL_WRITE_TEMP_BUF_ADDR;	//malloc(blksz);
		/*if (temp_data == NULL) {
		   printf("%s: malloc %lX bytes fail\n", __FUNCTION__, blksz);
		   return -1;
		   } */

		if (blk_dread(blk_dev_desc, (start_lba + blkcnt_lb), 1, (void *)temp_data) != 1) {
			printf("%s: some error happend while reading disk\n", __FUNCTION__);
			return -1;
		}
		memcpy((void *)temp_data, (void *)(buffer + blkcnt_lb * blksz), last_len);

		if (blk_dwrite(blk_dev_desc, (start_lba + blkcnt_lb), 1, (void *)temp_data) != 1) {
			printf("%s: some error happend while writing disk\n", __FUNCTION__);
			return -1;
		}
	}
#else
	if ((u64) buffer % 0x1000 == 0) {
		wr_blkcnt_lb = blk_dev_desc->block_write(blk_dev_desc, start_lba, blkcnt_lb, (void *)buffer);
		if (wr_blkcnt_lb != blkcnt_lb) {
			printf("%s: some error happend while writing disk\n", __FUNCTION__);
			return -1;
		}
	} else {
		temp_data = (char *)DL_WRITE_TEMP_BUF_ADDR;
		unsaved_blks = blkcnt_lb;
		while (unsaved_blks != 0) {
			grp_blks = ((unsaved_blks > SDHCI_ALIGN_BUF_BLKS) ? SDHCI_ALIGN_BUF_BLKS : unsaved_blks);
			memcpy((void *)temp_data, (void *)buffer, (grp_blks * blksz));
			grp_wr_blkcnt_lb =
			    blk_dev_desc->block_write(blk_dev_desc, (start_lba + wr_blkcnt_lb), grp_blks,
						      (void *)temp_data);
			if (grp_wr_blkcnt_lb != grp_blks) {
				printf("%s: some error happend while writing disk\n", __FUNCTION__);
				return -1;
			}
			unsaved_blks -= grp_blks;
			wr_blkcnt_lb += grp_blks;
			buffer += grp_blks * blksz;
		}
	}

	if (last_len) {
		temp_data = (char *)DL_WRITE_TEMP_BUF_ADDR;	//malloc(blksz);
		/*if (temp_data == NULL) {
		   printf("%s: malloc 0x%lX bytes fail\n", __FUNCTION__, blksz);
		   return -1;
		   } */

		if (blk_dev_desc->block_read(blk_dev_desc, (start_lba + blkcnt_lb), 1, (void *)temp_data) != 1) {
			printf("%s: some error happend while reading disk\n", __FUNCTION__);
			return -1;
		}
		memcpy((void *)temp_data, (void *)(buffer + blkcnt_lb * blksz), last_len);

		if (blk_dev_desc->block_write(blk_dev_desc, (start_lba + blkcnt_lb), 1, (void *)temp_data) != 1) {
			printf("%s: some error happend while writing disk\n", __FUNCTION__);
			return -1;
		}
	}
#endif

	return (wr_blkcnt_lb * blksz + last_len);
}

int ssd_write(char *part_name, u64 addr, u64 len, char *buffer)
{
	lbaint_t start_lba, blkcnt_lb;
	lbaint_t wr_blkcnt_lb, grp_wr_blkcnt_lb;
	struct blk_desc *blk_dev_desc = NULL;
	ulong blksz;
	ulong last_len;
	ulong grp_blks, unsaved_blks;
	char *temp_data = NULL;

	blk_dev_desc = blk_get_dev("nvme", 0);
	if (!blk_dev_desc) {
		pr_err("get nvme dev fail\n");
		return -1;
	}
	blksz = blk_dev_desc->blksz;
	if (blksz != 512) {
		printf("%s: blksz 0x%lX is error\n", __FUNCTION__, blksz);
		return -1;
	}
	if (addr % blksz) {
		printf("%s: addr 0x%llX not align\n", __FUNCTION__, addr);
		return -1;
	}
	start_lba = (addr / blksz) & 0xffffffff;
	blkcnt_lb = (len / blksz) & 0xffffffff;
	if (len % blksz) {
		last_len = len - blkcnt_lb * blksz;
	} else {
		last_len = 0;
	}
	wr_blkcnt_lb = 0;

#if CONFIG_IS_ENABLED(BLK)
	if ((u64) buffer % 0x1000 == 0) {
		wr_blkcnt_lb = blk_dwrite(blk_dev_desc, start_lba, blkcnt_lb, (void *)buffer);
		if (wr_blkcnt_lb != blkcnt_lb) {
			printf("%s: some error happend while writing disk\n", __FUNCTION__);
			return -1;
		}
	} else {
		temp_data = (char *)DL_WRITE_TEMP_BUF_ADDR;
		unsaved_blks = blkcnt_lb;
		while (unsaved_blks != 0) {
			grp_blks = ((unsaved_blks > SDHCI_ALIGN_BUF_BLKS) ? SDHCI_ALIGN_BUF_BLKS : unsaved_blks);
			memcpy((void *)temp_data, (void *)buffer, (grp_blks * blksz));
			grp_wr_blkcnt_lb =
			    blk_dwrite(blk_dev_desc, (start_lba + wr_blkcnt_lb), grp_blks, (void *)temp_data);
			if (grp_wr_blkcnt_lb != grp_blks) {
				printf("%s: some error happend while writing disk\n", __FUNCTION__);
				return -1;
			}
			unsaved_blks -= grp_blks;
			wr_blkcnt_lb += grp_blks;
			buffer += grp_blks * blksz;
		}
	}

	if (last_len) {
		printf("%s: last %lX bytes\n", __FUNCTION__, last_len);
		temp_data = (char *)DL_WRITE_TEMP_BUF_ADDR;	//malloc(blksz);
		/*if (temp_data == NULL) {
		   printf("%s: malloc %lX bytes fail\n", __FUNCTION__, blksz);
		   return -1;
		   } */

		if (blk_dread(blk_dev_desc, (start_lba + blkcnt_lb), 1, (void *)temp_data) != 1) {
			printf("%s: some error happend while reading disk\n", __FUNCTION__);
			return -1;
		}
		memcpy((void *)temp_data, (void *)(buffer + blkcnt_lb * blksz), last_len);

		if (blk_dwrite(blk_dev_desc, (start_lba + blkcnt_lb), 1, (void *)temp_data) != 1) {
			printf("%s: some error happend while writing disk\n", __FUNCTION__);
			return -1;
		}
	}
#else
	if ((u64) buffer % 0x1000 == 0) {
		wr_blkcnt_lb = blk_dev_desc->block_write(blk_dev_desc, start_lba, blkcnt_lb, (void *)buffer);
		if (wr_blkcnt_lb != blkcnt_lb) {
			printf("%s: some error happend while writing disk\n", __FUNCTION__);
			return -1;
		}
	} else {
		temp_data = (char *)DL_WRITE_TEMP_BUF_ADDR;
		unsaved_blks = blkcnt_lb;
		while (unsaved_blks != 0) {
			grp_blks = ((unsaved_blks > SDHCI_ALIGN_BUF_BLKS) ? SDHCI_ALIGN_BUF_BLKS : unsaved_blks);
			memcpy((void *)temp_data, (void *)buffer, (grp_blks * blksz));
			grp_wr_blkcnt_lb =
			    blk_dev_desc->block_write(blk_dev_desc, (start_lba + wr_blkcnt_lb), grp_blks,
						      (void *)temp_data);
			if (grp_wr_blkcnt_lb != grp_blks) {
				printf("%s: some error happend while writing disk\n", __FUNCTION__);
				return -1;
			}
			unsaved_blks -= grp_blks;
			wr_blkcnt_lb += grp_blks;
			buffer += grp_blks * blksz;
		}
	}

	if (last_len) {
		temp_data = (char *)DL_WRITE_TEMP_BUF_ADDR;	//malloc(blksz);
		/*if (temp_data == NULL) {
		   printf("%s: malloc 0x%lX bytes fail\n", __FUNCTION__, blksz);
		   return -1;
		   } */

		if (blk_dev_desc->block_read(blk_dev_desc, (start_lba + blkcnt_lb), 1, (void *)temp_data) != 1) {
			printf("%s: some error happend while reading disk\n", __FUNCTION__);
			return -1;
		}
		memcpy((void *)temp_data, (void *)(buffer + blkcnt_lb * blksz), last_len);

		if (blk_dev_desc->block_write(blk_dev_desc, (start_lba + blkcnt_lb), 1, (void *)temp_data) != 1) {
			printf("%s: some error happend while writing disk\n", __FUNCTION__);
			return -1;
		}
	}
#endif

	return (wr_blkcnt_lb * blksz + last_len);
}

int emmc_erase(char *part_name, u64 addr, u64 len)
{
	lbaint_t start_lba, blkcnt_lb;
	lbaint_t erase_blkcnt_lb;
	struct blk_desc *blk_dev_desc = NULL;
	ulong blksz;
	int ret;

	blk_dev_desc = blk_get_dev("mmc", EMMC_DEV_ID);
	if (!blk_dev_desc) {
		pr_err("get mmc dev fail\n");
		return -1;
	}
	blksz = blk_dev_desc->blksz;
	if (blksz != 512) {
		printf("%s: blksz 0x%lX is error\n", __FUNCTION__, blksz);
		return -1;
	}
	if (addr % blksz) {
		printf("%s: addr 0x%llX not align\n", __FUNCTION__, addr);
		return -1;
	}
	if (len % blksz) {
		printf("%s: len 0x%llX not align\n", __FUNCTION__, len);
		return -1;
	}
	start_lba = addr / blksz;
	blkcnt_lb = len / blksz;
	printf("[%s]addr 0x%llX, len 0x%llX, start lba 0x%lX, blkcnt 0x%lX\n",
	       __FUNCTION__, addr, len, start_lba, blkcnt_lb);
#ifdef USE_BOOT_PARTITION
	/*
	 * spl and eip bin have backup and store in boot_parttion1
	 * uboot store in boot_partition2
	 * others store in user_partition with gpt table
	 */
	if (!strcmp(part_name, "spl")) {
		ret = blk_dselect_hwpart(blk_dev_desc, PARTITION_BOOT1);
		if (ret) {
			printf("%s: Failed to select h/w partition 1\n", __FUNCTION__);
			return -1;
		}
	}
	if (!strcmp(part_name, "uboot")) {
		ret = blk_dselect_hwpart(blk_dev_desc, PARTITION_BOOT2);
		if (ret) {
			printf("%s: Failed to select h/w partition 2\n", __FUNCTION__);
			return -1;
		}
	}
#endif
	/*
	 * all image store in user_partition with bootargs
	 */
	if (!strcmp(part_name, "eraseall")) {
		ret = blk_dselect_hwpart(blk_dev_desc, 0);
		if (ret) {
			printf("%s: Failed to select user area\n", __FUNCTION__);
			return -1;
		}
		start_lba = 0;
		blkcnt_lb = blk_dev_desc->lba;
		printf("%s: start eraseall, start lba 0x%lX, lbacnt 0x%lX\n", __FUNCTION__, start_lba, blkcnt_lb);
	} else {
		ret = blk_dselect_hwpart(blk_dev_desc, 0);
		if (ret) {
			printf("%s: Failed to select user area\n", __FUNCTION__);
			return -1;
		}
	}
#if CONFIG_IS_ENABLED(BLK)
	erase_blkcnt_lb = blk_derase(blk_dev_desc, start_lba, blkcnt_lb);
	if (erase_blkcnt_lb != blkcnt_lb) {
		printf("%s: some error happend while erasing disk\n", __FUNCTION__);
		return -1;
	}
#else
	erase_blkcnt_lb = blk_dev_desc->block_erase(blk_dev_desc, start_lba, blkcnt_lb);
	if (erase_blkcnt_lb != blkcnt_lb) {
		printf("%s: some error happend while erasing disk\n", __FUNCTION__);
		return -1;
	}
#endif

	return 0;
}

#ifdef FDL_BAD_BLKS_SCAN
static void spi_nand_scan_bad_blks(void)
{
	int ret = -1;
	u32 busnum = 0;
	loff_t off;
	struct udevice *dev = NULL;
	struct mtd_info *mtd = NULL;

	ret = uclass_get_device(UCLASS_MTD, busnum, &dev);
	if (ret) {
		pr_err("uclass_get_device: Invalid bus %d (err=%d)\n", busnum, ret);
		return;
	}

	mtd = dev_get_uclass_priv(dev);
	if (!mtd) {
		pr_err("\nno devices available\n");
		return;
	}

	printf("\nbad blocks:\n");
	for (off = 0; off < mtd->size; off += mtd->erasesize) {
		if (mtd_block_isbad(mtd, off))
			printf("%lld: 0x%llx\n", (unsigned long long)off / mtd->erasesize, (unsigned long long)off);
	}
}
#endif

static int spi_nand_write(char *part_name, u64 addr, u64 len, char *buffer)
{

/* 1. mtd spi nand device init */
	u32 ret, dl_buf_off;
	u32 busnum = 0;
	u64 remaining;
	u64 off = 0;
	int idx;
	loff_t maxsize;
	size_t retlen, write_size;
	static int mtdpart_skip_blk_cnt = 0;
	static loff_t size = 0;
	struct udevice *dev = NULL;
	struct mtd_info *mtd = NULL;
	struct erase_info erase_op = { };

	ret = uclass_get_device(UCLASS_MTD, busnum, &dev);
	if (ret) {
		printf("uclass_get_device: Invalid bus %d (err=%d)\n", busnum, ret);
		return ret;
	}

	mtd = dev_get_uclass_priv(dev);

/* 2. first erase partition */
	if (strcmp(lastXferPart, part_name)) {
		erase_op.mtd = mtd;
		erase_op.len = mtd->erasesize;
		erase_op.scrub = 0;

		ret = mtd_arg_off(part_name, &idx, &partOff, &size, &maxsize, MTD_DEV_TYPE_NAND, mtd->size);
		if (ret)
			return ret;
		printf("%s: partiton %s off = 0x%llx size = 0x%llx\n", __func__, part_name, partOff, size);

		remaining = round_up(size, mtd->erasesize);
		erase_op.addr = partOff;

		while (remaining) {
			if (erase_op.addr + mtd->erasesize > partOff + size) {
				pr_err("%s: erase addr 0x%llX len 0x%X over %s part addr 0x%llX size 0x%llX\n",
				       __func__, erase_op.addr, mtd->erasesize, part_name, partOff, size);
				return -1;
			}
			//printf("Erase partition:%s",pfile->part_name);
			ret = mtd_erase(mtd, &erase_op);
			if (ret) {
				if (ret != -EIO) {
					printf("Failure while erasing at offset 0x%llx\n", erase_op.fail_addr);
					return -1;
				}
				pr_err("erase skip bad block @off = 0x%llX size = 0x%X\n", erase_op.addr,
				       mtd->erasesize);
			}
			remaining -= mtd->erasesize;
			erase_op.addr += mtd->erasesize;
		}
		printf("partiton %s erased @off = 0x%llx size = 0x%llx\n", part_name, partOff, size);
	}

/* 3. Loop over to do the actual read/write */
	if (strcmp(lastXferPart, part_name)) {
		xferPartCnt = 0;
		mtdpart_skip_blk_cnt = 0;
		printf("partiton %s write start, mtdpart_skip_blk_cnt=%d\n", part_name, mtdpart_skip_blk_cnt);
	}

	off = partOff + xferPartCnt * dl_buf_size + mtdpart_skip_blk_cnt * mtd->erasesize;	/* FDL_BUF_LEN = 2M */
	if (off % mtd->erasesize) {
		pr_err("nand addr 0x%llX is not block size 0x%X aligned!\n", off, mtd->erasesize);
		return -1;
	}
	xferPartCnt++;

	remaining = len;
	dl_buf_off = 0;
	printf("buf: unsave_recv_len=0x%llX, dl_buf_size=0x%lX\n", len, dl_buf_size);
	while (remaining) {
		if (mtd_block_isbad(mtd, off)) {
			mtdpart_skip_blk_cnt++;
			printf("nand addr 0x%llX, blk %lld is bad, mtdpart_skip_blk_cnt=%d\n", off,
			       off / mtd->erasesize, mtdpart_skip_blk_cnt);
			off += mtd->erasesize;
			continue;
		}

		write_size = (remaining > mtd->erasesize) ? mtd->erasesize : remaining;
		if (off + write_size > partOff + size) {
			pr_err("%s: write addr 0x%llX len 0x%lX over %s part addr 0x%llX size 0x%llX\n",
			       __func__, off, write_size, part_name, partOff, size);
			return -1;
		}
		if (dl_buf_off + write_size > len) {
			pr_err("%s: dl_buf_off 0x%X len 0x%lX over unsave_recv_len 0x%llX\n",
			       __func__, dl_buf_off, write_size, len);
			return -1;
		}

		mtd_write(mtd, off, write_size, &retlen, (u_char *) (buffer + dl_buf_off));
		if (retlen != write_size) {
			pr_err("write partition fail!.\n");
			return -1;
		}
		printf("partiton %s wrote 0x%lX bytes, dl_buf_off 0x%X ==> nand addr 0x%llX\n", part_name, retlen,
		       dl_buf_off, off);
		remaining -= write_size;
		dl_buf_off += write_size;
		off += write_size;
	}

	strcpy(lastXferPart, part_name);
	return dl_buf_off;

}

static int spi_nand_erase(char *part_name, u64 addr, u64 len)
{
	int ret = -1;
	int idx;
	u32 busnum = 0;
	u64 remaining;
	loff_t off = 0;
	loff_t size, maxsize;
	struct udevice *dev = NULL;
	struct mtd_info *mtd = NULL;
	struct erase_info erase_op = { };

	ret = uclass_get_device(UCLASS_MTD, busnum, &dev);
	if (ret) {
		printf("uclass_get_device: Invalid bus %d (err=%d)\n", busnum, ret);
		return ret;
	}

	mtd = dev_get_uclass_priv(dev);

	erase_op.mtd = mtd;
	erase_op.len = mtd->erasesize;
	erase_op.scrub = 0;

	if (!strcmp(part_name, "eraseall")) {
		printf("nand eraseall\n");
		erase_op.addr = 0;
		remaining = mtd->size;
	} else {
		printf("nand erase %s part\n", part_name);
		ret = mtd_arg_off(part_name, &idx, &off, &size, &maxsize, MTD_DEV_TYPE_NAND, mtd->size);
		if (ret)
			return ret;

		if ((addr < off) || (addr + len > off + size)) {
			printf("erase region [0x%llX, 0x%llX] over part region [0x%llX, 0x%llX]\n", addr, (addr + len),
			       off, (off + size));
			return -1;
		}
		remaining = round_up(len, mtd->erasesize);
		erase_op.addr = addr;
	}

	while (remaining) {
		printf("erase phy addr 0x%llX, size 0x%llX\n", erase_op.addr, erase_op.len);
		ret = mtd_erase(mtd, &erase_op);
		if (ret) {
			if (ret != -EIO) {
				printf("Failure while erasing at offset 0x%llx\n", erase_op.fail_addr);
				return -1;
			}
		} else {
			remaining -= mtd->erasesize;
		}
		erase_op.addr += mtd->erasesize;
	}

	return ret;
}

int fdl_read_from_spinand(char * part_name, u64 addr, u64 len, char * buffer)
{
/* 1. mtd spi nand device init */
	int ret;
	u32 load_buf_off;
	u32 busnum = 0;
	loff_t off = 0;
	int idx;
	loff_t maxsize;
	size_t retlen, read_size, remaining;
	static loff_t part_off = 0;
	static loff_t part_size = 0;
	static int mtdpart_skip_blk_cnt = 0;
	struct udevice *dev;
	struct mtd_info *mtd;

	printf("%s: %s, addr 0x%llX, len 0x%llX\n", __func__, part_name, addr, len);
	ret = uclass_get_device(UCLASS_MTD, busnum, &dev);
	if (ret) {
		printf("uclass_get_device: Invalid bus %d (err=%d)\n", busnum, ret);
		return ret;
	}
	mtd = dev_get_uclass_priv(dev);

/* 2. Loop over to do the actual read/write */
	if (strcmp(lastLoadPart, part_name))
	{
		if (addr % mtd->erasesize) {
			pr_err("part addr 0x%llX is not block size 0x%X aligned!\n", addr, mtd->erasesize);
			return -1;
		}
		ret = mtd_arg_off(part_name, &idx, &part_off, &part_size, &maxsize, MTD_DEV_TYPE_NAND, mtd->size);
		if (ret) {
			printf("%s, %d, ret=%d\n",__func__, __LINE__, ret);
			return ret;
		}
		printf("%s: partiton %s off = 0x%llX size = 0x%llX\n", __func__, part_name, part_off, part_size);
		mtdpart_skip_blk_cnt = 0;
	}

	load_buf_off = 0;
	remaining = len;
	if (addr % mtd->erasesize) {
		read_size = ((addr + mtd->erasesize - 1) & ~(mtd->erasesize - 1)) - addr;
		read_size = (remaining > read_size) ? read_size : remaining;
		off = addr + mtdpart_skip_blk_cnt * mtd->erasesize;
		printf("%s: read phy_addr=0x%llX, last len=0x%lX\n", __func__, off, read_size);
		if (off + read_size > part_off + part_size) {
			pr_err("%s: read addr 0x%llX len 0x%lX over part %s addr 0x%llX size 0x%llX\n",
				__func__, off, read_size, part_name, part_off, part_size);
			return -1;
		}
		if (load_buf_off + read_size > dl_buf_size) {
			pr_err("%s: load_buf_off 0x%X len 0x%lX over dl_buf_size 0x%lX\n",
				__func__, load_buf_off, read_size, dl_buf_size);
			return -1;
		}

		mtd_read(mtd, off, read_size, &retlen, (u_char *)(buffer+load_buf_off));
		if (retlen != read_size) {
			pr_err("%s image read fail!\n", part_name);
			return -1;
		}
		printf("partiton %s read 0x%lX bytes, nand addr 0x%llX ==> buf addr 0x%llX\n",
			part_name, retlen, off, (u64)(buffer + load_buf_off));
		remaining -= read_size;
		load_buf_off += read_size;
		off += read_size;
	}
	else {
		off = (loff_t)(addr + mtdpart_skip_blk_cnt * mtd->erasesize); /* FDL_BUF_LEN = 2M */
	}

	printf("%s: read phy_addr=0x%llX, remaining=0x%lX\n", __func__, off, remaining);
	while (remaining) {
		if (mtd_block_isbad(mtd, off)) {
			mtdpart_skip_blk_cnt++;
			printf("nand addr 0x%llX, blk %lld is bad, mtdpart_skip_blk_cnt=%d\n",
				off, (u64)off / mtd->erasesize, mtdpart_skip_blk_cnt);
			off += mtd->erasesize;
			continue;
		}

		read_size = (remaining > mtd->erasesize) ? mtd->erasesize : remaining;
		if (off + read_size > part_off + part_size) {
			pr_err("%s: read addr 0x%llX len 0x%lX over part %s addr 0x%llX size 0x%llX\n",
				__func__, off, read_size, part_name, part_off, part_size);
			return -1;
		}
		if (load_buf_off + read_size > dl_buf_size) {
			pr_err("%s: load_buf_off 0x%X len 0x%lX over dl_buf_size 0x%lX\n",
				__func__, load_buf_off, read_size, dl_buf_size);
			return -1;
		}

		mtd_read(mtd, off, read_size, &retlen, (u_char *)(buffer+load_buf_off));
		if (retlen != read_size) {
			pr_err("%s image read fail!\n", part_name);
			return -1;
		}
		printf("partiton %s read 0x%lX bytes, nand addr 0x%llX ==> buf addr 0x%llX\n",
			part_name, retlen, off, (u64)(buffer + load_buf_off));

		remaining -= read_size;
		load_buf_off += read_size;
		off += read_size;
	}

	strcpy(lastLoadPart, part_name);
	return load_buf_off;
}

static int spi_nor_erase(char *part_name, u64 addr, u64 len)
{
	int ret = -1;
	int idx;
	u32 busnum = 0;
	u64 remaining;
	loff_t off = 0;
	loff_t size, maxsize;
	struct udevice *dev = NULL;
	struct mtd_info *mtd = NULL;
	struct erase_info erase_op = { };
	struct spi_flash *flash;

	ret = uclass_get_device(UCLASS_SPI_FLASH, busnum, &dev);
	if (ret) {
		printf("uclass_get_device: Invalid bus %d (err=%d)\n", busnum, ret);
		return ret;
	}

	mtd = get_mtd_device_nm("nor0");
	flash = dev_get_uclass_priv(dev);

	erase_op.mtd = mtd;
	erase_op.len = mtd->erasesize;
	erase_op.scrub = 0;

	if (!strcmp(part_name, "eraseall")) {
		printf("nor eraseall\n");
		erase_op.addr = 0;
		remaining = flash->size;
		printf("erase size: 0x%llx\n", remaining);
	} else {
		printf("nor erase %s part\n", part_name);
		ret = mtd_arg_off(part_name, &idx, &off, &size, &maxsize, MTD_DEV_TYPE_NOR, flash->size);
		if (ret)
			return ret;

		if ((addr < off) || (addr + len > off + size)) {
			printf("erase region [0x%llX, 0x%llX] over part region [0x%llX, 0x%llX]\n", addr, (addr + len), off, (off + size));
			return -1;
		}
		remaining = round_up(len, mtd->erasesize);
		erase_op.addr = addr;
		printf("erase size: 0x%llx\n", remaining);
	}

	while (remaining) {
		printf("erase phy addr 0x%llX, size 0x%llX\n", erase_op.addr, erase_op.len);
		ret = mtd_erase(mtd, &erase_op);
		if (ret) {
			if (ret != -EIO) {
				printf("Failure while erasing at offset 0x%llx\n", erase_op.fail_addr);
				return -1;
			}
		} else {
			remaining -= mtd->erasesize;
		}
		erase_op.addr += mtd->erasesize;
	}

	return ret;
}

int common_raw_read(char *part_name, u64 addr, u64 len, char *data)
{
	u64 read_len = 0;

	switch (boot_info_data.storage_sel) {
	case STORAGE_TYPE_EMMC:
		read_len = fdl_read_from_emmc(part_name, addr, len, data);
		break;
	case STORAGE_TYPE_NAND:
		read_len = fdl_read_from_spinand(part_name, addr, len, data);
		break;
	case STORAGE_TYPE_NOR:
		break;
	case STORAGE_TYPE_SSD:
		read_len = fdl_read_from_ssd(part_name, addr, len, data);
		break;
	default:
		break;
	}

	return read_len;
}

int common_raw_write(char *part_name, u64 addr, u64 len, char *data)
{
	u64 write_len = 0;

	switch (boot_info_data.storage_sel) {
	case STORAGE_TYPE_EMMC:
		write_len = emmc_write(part_name, addr, len, data);
		break;
	case STORAGE_TYPE_NAND:
		write_len = spi_nand_write(part_name, addr, len, data);
		break;
	case STORAGE_TYPE_NOR:
		break;
	case STORAGE_TYPE_SSD:
		write_len = ssd_write(part_name, addr, len, data);
	default:
		break;
	}

	return write_len;
}

int common_raw_erase(char *part_name, u64 addr, u64 len)
{
	int ret = -1;

	switch (boot_info_data.storage_sel) {
	case STORAGE_TYPE_EMMC:
		ret = emmc_erase(part_name, addr, len);
		break;
	case STORAGE_TYPE_NAND:
		ret = spi_nand_erase(part_name, addr, len);
		break;
	case STORAGE_TYPE_NOR:
		ret = spi_nor_erase(part_name, addr, len);
		break;
	case STORAGE_TYPE_SSD:
		break;
	default:
		break;
	}

	return ret;
}

int fdl_save_to_ddr(struct fdl_file_info *pfile)
{
	static long dtb_cur = DTB_IMAGE_ADDR;
	static long kernel_cur = KERNEL_IMAGE_ADDR;

	if (!strcmp(pfile->part_name, "dtb")) {
		//printf("save dtb\n");
		memcpy((void *)dtb_cur, (void *)pfile->start_addr, pfile->unsave_recv_len);
		dtb_cur += pfile->unsave_recv_len;
	} else if (!strcmp(pfile->part_name, "kernel")) {
		//printf("save kernel\n");
		memcpy((void *)kernel_cur, (void *)pfile->start_addr, pfile->unsave_recv_len);
		kernel_cur += pfile->unsave_recv_len;
	} else {
		printf("don't support saving %s\n", pfile->part_name);
	}
	return 0;
}

int fdl_save_to_emmc(struct fdl_file_info *pfile)
{
	lbaint_t start_lba, blkcnt_lb, base_lba;
	lbaint_t fdl_buf_written_lb;
	lbaint_t wr_blkcnt_lb;
	struct blk_desc *blk_dev_desc = NULL;
	disk_partition_t part_info;
	ulong blksz;
	u32 ret = 0;
#ifdef CONFIG_AXERA_SECURE_BOOT
	struct img_header *img_header;
	struct spl_header *spl_header;
	struct rsa_key *rsa_key;
#endif
	blk_dev_desc = blk_get_dev("mmc", EMMC_DEV_ID);
	if (!blk_dev_desc) {
		pr_err("get mmc dev fail\n");
		return -1;
	}
	blksz = blk_dev_desc->blksz;

	blkcnt_lb = PAD_COUNT(pfile->unsave_recv_len, blksz);
	if (pfile->recv_len > pfile->unsave_recv_len)
		fdl_buf_written_lb = PAD_COUNT(pfile->recv_len - pfile->unsave_recv_len, blksz);
	else
		fdl_buf_written_lb = 0;

	ret = get_part_info(blk_dev_desc, pfile->part_name, &part_info);
	if (ret == -1) {
		printf("%s: get %s partition info fail\n", __FUNCTION__, pfile->part_name);
	}

	if(pfile->recv_len > (u64)part_info.blksz*part_info.size) {
		printf("%s: part: %s: part_info.start = 0x%lx block count = 0x%lx, blksz: %lu\n", __FUNCTION__,pfile->part_name, part_info.start, part_info.size, part_info.blksz);
		pr_err("received file size 0x%llx bytes bigger than partion size 0x%lx bytes\n", pfile->recv_len, part_info.blksz*part_info.size);
		return -1;
	}
	/*
	 * spl and eip bin have backup and store in boot_parttion1
	 * uboot store in boot_partition2
	 * others store in user_partition with gpt table
	 */
	if (!strcmp(pfile->part_name, "spl")) {
		base_lba = part_info.start;
		start_lba = base_lba + fdl_buf_written_lb;
#ifdef CONFIG_AXERA_SECURE_BOOT
		spl_header = (struct spl_header *)pfile->start_addr;
		rsa_key = (struct rsa_key *)&spl_header->key_n_header;
		ret = secure_verify(pfile->part_name, rsa_key, (char *)spl_header + sizeof(*spl_header) * 2,
				    (char *)&spl_header->sig_header, spl_header->img_size);
		if (ret != 0) {
			frame_send_respone(FDL_RESP_SECURE_SIGNATURE_ERR);
			return -1;
		}
		/*verify back spl */
		spl_header = (struct spl_header *)(pfile->start_addr + sizeof(*spl_header));
		rsa_key = (struct rsa_key *)&spl_header->key_n_header;
		ret =
		    secure_verify(pfile->part_name, rsa_key,
				  ((char *)spl_header + SIZE_SPL_IMG_PADDING - sizeof(*spl_header)),
				  (char *)&spl_header->sig_header, spl_header->img_size);
		if (ret != 0) {
			frame_send_respone(FDL_RESP_SECURE_SIGNATURE_ERR);
			return -1;
		}
#endif
#if CONFIG_IS_ENABLED(BLK)
		wr_blkcnt_lb = blk_dwrite(blk_dev_desc, start_lba, blkcnt_lb, (void *)pfile->start_addr);
#else
		wr_blkcnt_lb = blk_dev_desc->block_write(blk_dev_desc, start_lba, blkcnt_lb, (void *)pfile->start_addr);
#endif
		if (wr_blkcnt_lb != blkcnt_lb) {
			pr_err("some error happend while write to disk\n");
			return -1;
		}
	} else if (!strcmp(pfile->part_name, "uboot")) {
#ifdef CONFIG_AXERA_SECURE_BOOT
		img_header = (struct img_header *)pfile->start_addr;
		rsa_key = (struct rsa_key *)&img_header->key_n_header;
		ret = secure_verify(pfile->part_name, rsa_key, (char *)img_header + 0x800,
				    (char *)&img_header->sig_header, img_header->uboot_size);
		if (ret != 0) {
			frame_send_respone(FDL_RESP_SECURE_SIGNATURE_ERR);
			return -1;
		}
#endif
		base_lba = part_info.start;
		start_lba = base_lba + fdl_buf_written_lb;
#if CONFIG_IS_ENABLED(BLK)
		wr_blkcnt_lb = blk_dwrite(blk_dev_desc, start_lba, blkcnt_lb, (void *)pfile->start_addr);
#else
		wr_blkcnt_lb = blk_dev_desc->block_write(blk_dev_desc, start_lba, blkcnt_lb, (void *)pfile->start_addr);
#endif
		if (wr_blkcnt_lb != blkcnt_lb) {
			pr_err("some error happend while write to disk\n");
			return -1;
		}
	} else {
		base_lba = part_info.start;
		start_lba = base_lba + fdl_buf_written_lb;
#if CONFIG_IS_ENABLED(BLK)
		wr_blkcnt_lb = blk_dwrite(blk_dev_desc, start_lba, blkcnt_lb, (void *)pfile->start_addr);
#else
		wr_blkcnt_lb = blk_dev_desc->block_write(blk_dev_desc, start_lba, blkcnt_lb, (void *)pfile->start_addr);
#endif

		if (wr_blkcnt_lb != blkcnt_lb) {
			pr_err("some error happend while write to disk\n");
			return -1;
		}
	}
	return 0;
}

int fdl_save_to_spinand(struct fdl_file_info *pfile)
{

/* 1. mtd spi nand device init */
	u32 ret, dl_buf_off;
	u32 busnum = 0;
	u64 remaining;
	u64 off = 0;
	int idx;
	loff_t maxsize;
	size_t retlen;
	loff_t write_size;
	static int mtdpart_skip_blk_cnt;
	static loff_t size;

	struct udevice *dev = NULL;
	struct mtd_info *mtd = NULL;
	struct erase_info erase_op = { };

	ret = uclass_get_device(UCLASS_MTD, busnum, &dev);
	if (ret) {
		printf("uclass_get_device: Invalid bus %d (err=%d)\n", busnum, ret);
		return ret;
	}

	mtd = dev_get_uclass_priv(dev);

/* 2. first erase partition */
	if (strcmp(lastXferPart, pfile->part_name)) {
		erase_op.mtd = mtd;
		erase_op.len = mtd->erasesize;
		erase_op.scrub = 0;

		ret = mtd_arg_off(pfile->part_name, &idx, &partOff, &size, &maxsize, MTD_DEV_TYPE_NAND, mtd->size);
		if (ret)
			return ret;
		printf("%s: partiton %s off = 0x%llx size = 0x%llx\n", __func__, pfile->part_name, partOff, size);

		remaining = round_up(size, mtd->erasesize);
		erase_op.addr = partOff;

		while (remaining) {
			if (erase_op.addr + mtd->erasesize > partOff + size) {
				pr_err("%s: erase addr 0x%llX len 0x%X over %s part addr 0x%llX size 0x%llX\n",
				       __func__, erase_op.addr, mtd->erasesize, pfile->part_name, partOff, size);
				return -1;
			}
			//printf("Erase partition:%s",pfile->part_name);
			ret = mtd_erase(mtd, &erase_op);
			if (ret) {
				if (ret != -EIO) {
					printf("Failure while erasing at offset 0x%llx\n", erase_op.fail_addr);
					return -1;
				}
				pr_err("erase skip bad block @off = 0x%llX size = 0x%X\n", erase_op.addr,
				       mtd->erasesize);
			}
			remaining -= mtd->erasesize;
			erase_op.addr += mtd->erasesize;
		}
		printf("partiton %s erased @off = 0x%llx size = 0x%llx\n", pfile->part_name, partOff, size);
	}

/* 3. Loop over to do the actual read/write */
	if (strcmp(lastXferPart, pfile->part_name)) {
		xferPartCnt = 0;
		mtdpart_skip_blk_cnt = 0;
		printf("partiton %s write start, mtdpart_skip_blk_cnt=%d\n", pfile->part_name, mtdpart_skip_blk_cnt);
	}
#if 0
	off = partOff + xferPartCnt * dl_buf_size;	/* FDL_BUF_LEN = 2M */
	xferPartCnt++;

	mtd_write(mtd, off, pfile->unsave_recv_len, &retlen, (u_char *) pfile->start_addr);
	if (retlen != pfile->unsave_recv_len) {
		printf("write partition fail!.\n");
		return -1;
	}
	printf("partiton %s write @off = 0x%llx size = 0x%x\n", pfile->part_name, off, pfile->unsave_recv_len);
#else
	off = partOff + xferPartCnt * dl_buf_size + mtdpart_skip_blk_cnt * mtd->erasesize;	/* FDL_BUF_LEN = 2M */
	if (off % mtd->erasesize) {
		pr_err("nand addr 0x%llX is not block size 0x%X aligned!\n", off, mtd->erasesize);
		return -1;
	}
	xferPartCnt++;

	remaining = pfile->unsave_recv_len;
	dl_buf_off = 0;
	printf("buf: unsave_recv_len=0x%llX, maxsize=0x%lX\n", pfile->unsave_recv_len, dl_buf_size);
	while (remaining) {
		if (mtd_block_isbad(mtd, off)) {
			mtdpart_skip_blk_cnt++;
			printf("nand addr 0x%llX, blk %lld is bad, mtdpart_skip_blk_cnt=%d\n", off,
			       off / mtd->erasesize, mtdpart_skip_blk_cnt);
			off += mtd->erasesize;
			continue;
		}

		write_size = (remaining > mtd->erasesize) ? mtd->erasesize : remaining;
		if (off + write_size > partOff + size) {
			pr_err("%s: write addr 0x%llX len 0x%llX over %s part addr 0x%llX size 0x%llX\n",
			       __func__, off, write_size, pfile->part_name, partOff, size);
			return -1;
		}
		if (dl_buf_off + write_size > pfile->unsave_recv_len) {
			pr_err("%s: dl_buf_off 0x%X len 0x%llX over unsave_recv_len 0x%llX\n",
			       __func__, dl_buf_off, write_size, pfile->unsave_recv_len);
			return -1;
		}

		mtd_write(mtd, off, write_size, &retlen, (u_char *) (pfile->start_addr + dl_buf_off));
		if (retlen != write_size) {
			pr_err("write partition fail!.\n");
			return -1;
		}
		printf("partiton %s wrote 0x%lX bytes, dl_buf_off 0x%X ==> nand addr 0x%llX\n", pfile->part_name,
		       retlen, dl_buf_off, off);
		remaining -= write_size;
		dl_buf_off += write_size;
		off += write_size;
	}
#endif

	strcpy(lastXferPart, pfile->part_name);
	return 0;

}

int fdl_save_to_spinor(struct fdl_file_info *pfile)
{
	/* 1. mtd spi nand device init */
	u32 ret;
	u32 busnum = 0;
	u64 remaining, off = 0;
	int idx;
	loff_t size, maxsize;
	size_t retlen;
	struct udevice *dev;
	struct mtd_info *mtd;
	struct erase_info erase_op = { };
	struct spi_flash *flash;

	ret = uclass_get_device(UCLASS_SPI_FLASH, busnum, &dev);
	if (ret) {
		printf("uclass_get_device: Invalid bus %d (err=%d)\n", busnum, ret);
		return ret;
	}
	mtd = get_mtd_device_nm("nor0");
	flash = dev_get_uclass_priv(dev);

	/* 2. first erase partition */
	if (strcmp(lastXferPart, pfile->part_name)) {
		erase_op.mtd = mtd;
		erase_op.len = mtd->erasesize;
		erase_op.scrub = 0;

		ret = mtd_arg_off(pfile->part_name, &idx, &partOff, &size, &maxsize, MTD_DEV_TYPE_NOR, flash->size);
		if (ret)
			return ret;

		remaining = round_up(size, mtd->erasesize);
		erase_op.addr = partOff;

		while (remaining) {
			ret = mtd_erase(mtd, &erase_op);
			if (ret) {
				if (ret != -EIO) {
					printf("Failure while erasing at offset 0x%llx\n", erase_op.fail_addr);
					return -1;
				}
			} else {
				remaining -= mtd->erasesize;
			}
			erase_op.addr += mtd->erasesize;
		}
		printf("partiton %s erased @off = 0x%llx size = 0x%llx\n", pfile->part_name, partOff, size);
	}

	/* 3. Loop over to do the actual read/write */
	if (strcmp(lastXferPart, pfile->part_name))
		xferPartCnt = 0;

	off = partOff + xferPartCnt * dl_buf_size;
	xferPartCnt++;

	mtd_write(mtd, off, pfile->unsave_recv_len, &retlen, (u_char *) pfile->start_addr);
	if (retlen != pfile->unsave_recv_len) {
		printf("write partition fail!.\n");
		return -1;
	}
	printf("partiton %s write @off = 0x%llx size = 0x%llx\n", pfile->part_name, off, pfile->unsave_recv_len);

	strcpy(lastXferPart, pfile->part_name);
	return 0;
}

int fdl_save_to_ssd(struct fdl_file_info *pfile)
{
	lbaint_t start_lba, blkcnt_lb, base_lba;
	lbaint_t fdl_buf_written_lb;
	lbaint_t wr_blkcnt_lb;
	struct blk_desc *blk_dev_desc = NULL;
	disk_partition_t part_info;
	ulong blksz;
	u32 ret = 0;
#ifdef CONFIG_AXERA_SECURE_BOOT
	struct img_header *img_header;
	struct spl_header *spl_header;
	struct rsa_key *rsa_key;
#endif

	if (!strcmp(pfile->part_name, "spl") || !strcmp(pfile->part_name, "uboot")
			|| !strcmp(pfile->part_name, "uboot_bk") || !strcmp(pfile->part_name, "env")) {
		return fdl_save_to_spinor(pfile);
	} else {

		blk_dev_desc = blk_get_dev("nvme", 0);
		if (!blk_dev_desc) {
			pr_err("get nvme dev fail\n");
			return -1;
		}
	}
	blksz = blk_dev_desc->blksz;

	blkcnt_lb = PAD_COUNT(pfile->unsave_recv_len, blksz);
	if (pfile->recv_len > pfile->unsave_recv_len)
		fdl_buf_written_lb = PAD_COUNT(pfile->recv_len - pfile->unsave_recv_len, blksz);
	else
		fdl_buf_written_lb = 0;

	ret = get_part_info(blk_dev_desc, pfile->part_name, &part_info);
	if (ret == -1) {
		printf("%s: get %s partition info fail\n", __FUNCTION__, pfile->part_name);
	}

	if(pfile->recv_len > (u64)part_info.blksz*part_info.size) {
		printf("%s: part: %s: part_info.start = 0x%lx block count = 0x%lx, blksz: %lu\n", __FUNCTION__,pfile->part_name, part_info.start, part_info.size, part_info.blksz);
		pr_err("received file size 0x%llx bytes bigger than partion size 0x%lx bytes\n", pfile->recv_len, part_info.blksz*part_info.size);
		return -1;
	}
	/*
	 * spl and eip bin have backup and store in boot_parttion1
	 * uboot store in boot_partition2
	 * others store in user_partition with gpt table
	 */
	if (!strcmp(pfile->part_name, "spl")) {
		base_lba = part_info.start;
		start_lba = base_lba + fdl_buf_written_lb;
#ifdef CONFIG_AXERA_SECURE_BOOT
		spl_header = (struct spl_header *)pfile->start_addr;
		rsa_key = (struct rsa_key *)&spl_header->key_n_header;
		ret = secure_verify(pfile->part_name, rsa_key, (char *)spl_header + sizeof(*spl_header) * 2,
				    (char *)&spl_header->sig_header, spl_header->img_size);
		if (ret != 0) {
			frame_send_respone(FDL_RESP_SECURE_SIGNATURE_ERR);
			return -1;
		}
		/*verify back spl */
		spl_header = (struct spl_header *)(pfile->start_addr + sizeof(*spl_header));
		rsa_key = (struct rsa_key *)&spl_header->key_n_header;
		ret =
		    secure_verify(pfile->part_name, rsa_key,
				  ((char *)spl_header + SIZE_SPL_IMG_PADDING - sizeof(*spl_header)),
				  (char *)&spl_header->sig_header, spl_header->img_size);
		if (ret != 0) {
			frame_send_respone(FDL_RESP_SECURE_SIGNATURE_ERR);
			return -1;
		}
#endif
#if CONFIG_IS_ENABLED(BLK)
		wr_blkcnt_lb = blk_dwrite(blk_dev_desc, start_lba, blkcnt_lb, (void *)pfile->start_addr);
#else
		wr_blkcnt_lb = blk_dev_desc->block_write(blk_dev_desc, start_lba, blkcnt_lb, (void *)pfile->start_addr);
#endif
		if (wr_blkcnt_lb != blkcnt_lb) {
			pr_err("some error happend while write to disk\n");
			return -1;
		}
	} else if (!strcmp(pfile->part_name, "uboot")) {
#ifdef CONFIG_AXERA_SECURE_BOOT
		img_header = (struct img_header *)pfile->start_addr;
		rsa_key = (struct rsa_key *)&img_header->key_n_header;
		ret = secure_verify(pfile->part_name, rsa_key, (char *)img_header + 0x800,
				    (char *)&img_header->sig_header, img_header->uboot_size);
		if (ret != 0) {
			frame_send_respone(FDL_RESP_SECURE_SIGNATURE_ERR);
			return -1;
		}
#endif
		base_lba = part_info.start;
		start_lba = base_lba + fdl_buf_written_lb;
#if CONFIG_IS_ENABLED(BLK)
		wr_blkcnt_lb = blk_dwrite(blk_dev_desc, start_lba, blkcnt_lb, (void *)pfile->start_addr);
#else
		wr_blkcnt_lb = blk_dev_desc->block_write(blk_dev_desc, start_lba, blkcnt_lb, (void *)pfile->start_addr);
#endif
		if (wr_blkcnt_lb != blkcnt_lb) {
			pr_err("some error happend while write to disk\n");
			return -1;
		}
	} else {
		base_lba = part_info.start;
		start_lba = base_lba + fdl_buf_written_lb;
#if CONFIG_IS_ENABLED(BLK)
		wr_blkcnt_lb = blk_dwrite(blk_dev_desc, start_lba, blkcnt_lb, (void *)pfile->start_addr);
#else
		wr_blkcnt_lb = blk_dev_desc->block_write(blk_dev_desc, start_lba, blkcnt_lb, (void *)pfile->start_addr);
#endif

		if (wr_blkcnt_lb != blkcnt_lb) {
			pr_err("some error happend while write to disk\n");
			return -1;
		}
	}
	return 0;
}

int fdl_save_to_storage(struct fdl_file_info *pfile)
{
	int ret = 0;

	switch (boot_info_data.storage_sel) {
	case STORAGE_TYPE_EMMC:
		ret = fdl_save_to_emmc(pfile);
		break;
	case STORAGE_TYPE_NAND:
		ret = fdl_save_to_spinand(pfile);
		break;
	case STORAGE_TYPE_NOR:
		ret = fdl_save_to_spinor(pfile);
		break;
	case STORAGE_TYPE_SSD:
		ret = fdl_save_to_ssd(pfile);
		break;
	case STORAGE_TYPE_UNKNOWN:
		ret = fdl_save_to_ddr(pfile);
		break;
	default:
		ret = -1;
		break;
	}

	return ret;
}

int fdl_repartition_handle(fdl_partition_t * fdl_part_table, u16 part_count)
{
	int ret;

	switch (boot_info_data.storage_sel) {
	case STORAGE_TYPE_EMMC:
		ret = repatition_emmc_handle(fdl_part_table, part_count);
		break;
	case STORAGE_TYPE_NAND:
		//ret = repartion_nand_handle(fdl_part_table, part_count);
		ret = 0;	//temp
		break;
	case STORAGE_TYPE_NOR:
		//ret = repartion_nor_handle(fdl_part_table, part_count);
		ret = 0;
		break;
	default:
		ret = -1;
		break;
	}

	return ret;
}

#define MAX_PATTERN_NUM           (6)
#if defined (CONFIG_AXERA_AX620)
#define SPI_BASE_ADDR             (0x8000000)
#elif defined (CONFIG_AXERA_AX630X)
#define SPI_BASE_ADDR             (0x80000000)
#elif defined (CONFIG_AXERA_AX650)
#define SPI_BASE_ADDR             (0x20000000)
#else
@error
#endif
#define SPI_SSIENR_ADDR           (SPI_BASE_ADDR + 0x8)
#define SPI_BAURD_ADDR            (SPI_BASE_ADDR + 0x14)
#define SPI_RX_SAMPLE_DELAY_ADDR  (SPI_BASE_ADDR + 0xf0)

#ifdef SPI_RX_SAMPLE_DLY_SCAN
static int spi_pattern_data_set(u32 * pbuf, u32 pattern_size, u32 pattern_val)
{
	int i;

	if (NULL == pbuf) {
		pr_err("%s: data buf is null\n", __func__);
		return -1;
	}
	if (pattern_size % 4) {
		pr_err("%s: pattern_size 0x%x not 4 bytes aligned\n", __func__, pattern_size);
		return -1;
	}

	for (i = 0; i < pattern_size / 4; i++) {
		pbuf[i] = pattern_val;
	}
	return 0;
}

static int spi_pattern_data_check(u32 * pbuf, u32 pattern_size, u32 pattern_val)
{
	int i;

	if (NULL == pbuf) {
		pr_err("%s: data buf is null\n", __func__);
		return -1;
	}
	if (pattern_size % 4) {
		pr_err("%s: pattern_size 0x%x not 4 bytes aligned\n", __func__, pattern_size);
		return -1;
	}

	for (i = 0; i < pattern_size / 4; i++) {
		if (pattern_val != pbuf[i]) {
			pr_err("addr 0x%lx, 0x%x != 0x%x\n", (unsigned long)&pbuf[i], pbuf[i], pattern_val);
			return -1;
		}
	}
	//printf("pattern 0x%x pass\n", pattern_val);
	return 0;
}

//#define SPI_RX_SAMPLE_SCAN_FIRST_WRITE
static u32 spi_rx_sample_delay_scan(int baurd)
{
	int ret = -1;
	u32 i, delay;
	u32 busnum = 0;
	size_t retlen;
	u32 spi_pattern[MAX_PATTERN_NUM] = {0, 0xffffffff, 0x5a5a5a5a, 0x0f0f0f0f, 0x12345678, 0x98765432};
	u_char * pbuf = NULL;
#ifdef SPI_RX_SAMPLE_SCAN_FIRST_WRITE
	struct erase_info erase_op;
#endif
	struct udevice *dev = NULL;
	struct mtd_info *mtd = NULL;

	printf("======== %s start ===========\n", __func__);
	ret = uclass_get_device(UCLASS_MTD, busnum, &dev);
	if (ret) {
		pr_err("uclass_get_device: Invalid bus %d (err=%d)\n", busnum, ret);
		return 0;
	}

	mtd = dev_get_uclass_priv(dev);
	if (!mtd) {
		pr_err("no devices available\n");
		return 0;
	}

	if (MAX_PATTERN_NUM * mtd->writesize > mtd->erasesize) {
		pr_err("scan size over nand block0\n");
		return 0;
	}

	pbuf = memalign(0x100000, MAX_PATTERN_NUM * mtd->writesize);
	if (NULL == pbuf) {
		pr_err("no mem space\n");
		return 0;
	}
	printf("malloc 0x%x bytes at addr 0x%lx for scan\n", MAX_PATTERN_NUM * mtd->writesize, (unsigned long)pbuf);
	for (i = 0; i < MAX_PATTERN_NUM; i++) {
		if (spi_pattern_data_set((u32 *)(pbuf + i * mtd->writesize), mtd->writesize, spi_pattern[i])) {
			pr_err("pattern set fail\n");
			return 0;
		}
	}
#ifdef SPI_RX_SAMPLE_SCAN_FIRST_WRITE
	erase_op.mtd = mtd;
	erase_op.len = mtd->erasesize;
	erase_op.scrub = 0;
	erase_op.addr = 0;
	ret = mtd_erase(mtd, &erase_op);
	if (ret) {
		if (ret != -EIO) {
			pr_err("Failure while erasing at offset 0x%llx\n",
				erase_op.fail_addr);
			return 0;
		}
		pr_err("erase @off = 0x%llX size = 0x%X return %d\n", erase_op.addr, mtd->erasesize, -EIO);
	}

	mtd_write(mtd, 0, MAX_PATTERN_NUM * mtd->writesize, &retlen, pbuf);
	if (retlen != MAX_PATTERN_NUM * mtd->writesize) {
		pr_err("write partition fail!.\n");
		return 0;
	}
#endif
	// pose edge rx sample delay scan
	for (delay = 0; delay < 2 * baurd; delay++) {
		*((u32 *)SPI_SSIENR_ADDR) = 0;
		udelay(100);
		*((u32 *)SPI_RX_SAMPLE_DELAY_ADDR) = delay;
		*((u32 *)SPI_SSIENR_ADDR) = 1;
		udelay(100);
		printf("0x%x=0x%x, 0x%x=0x%x, 0x%x=0x%x:\n",
			SPI_SSIENR_ADDR, *((u32 *)SPI_SSIENR_ADDR),
			SPI_BAURD_ADDR, *((u32 *)SPI_BAURD_ADDR),
			SPI_RX_SAMPLE_DELAY_ADDR, *((u32 *)SPI_RX_SAMPLE_DELAY_ADDR));
		memset((void *)pbuf, 0xee, MAX_PATTERN_NUM * mtd->writesize);
		mtd_read(mtd, 0, MAX_PATTERN_NUM * mtd->writesize, &retlen, pbuf);
		if (retlen != MAX_PATTERN_NUM * mtd->writesize) {
			pr_err("read fail!\n");
			return 0;
		}
		for (i = 0; i < MAX_PATTERN_NUM; i++) {
			if (spi_pattern_data_check((u32 *)(pbuf + i * mtd->writesize), mtd->writesize, spi_pattern[i])) {
				pr_err("pattern check fail\n");
				break;
			}
		}
		if (MAX_PATTERN_NUM == i)
			printf("pos edge: delay=%d pass\n", delay);
	}
	// neg edge rx sample delay scan
	for (delay = 0; delay < 2 * baurd; delay++) {
		*((u32 *)SPI_SSIENR_ADDR) = 0;
		udelay(100);
		*((u32 *)SPI_RX_SAMPLE_DELAY_ADDR) = 0x10000 | delay;
		*((u32 *)SPI_SSIENR_ADDR) = 1;
		udelay(100);
		printf("0x%x=0x%x, 0x%x=0x%x, 0x%x=0x%x:\n",
			SPI_SSIENR_ADDR, *((u32 *)SPI_SSIENR_ADDR),
			SPI_BAURD_ADDR, *((u32 *)SPI_BAURD_ADDR),
			SPI_RX_SAMPLE_DELAY_ADDR, *((u32 *)SPI_RX_SAMPLE_DELAY_ADDR));
		memset((void *)pbuf, 0xee, MAX_PATTERN_NUM * mtd->writesize);
		mtd_read(mtd, 0, MAX_PATTERN_NUM * mtd->writesize, &retlen, pbuf);
		if (retlen != MAX_PATTERN_NUM * mtd->writesize) {
			pr_err("read fail!\n");
			return 0;
		}
		for (i = 0; i < MAX_PATTERN_NUM; i++) {
			if (spi_pattern_data_check((u32 *)(pbuf + i * mtd->writesize), mtd->writesize, spi_pattern[i])) {
				pr_err("pattern check fail\n");
				break;
			}
		}
		if (MAX_PATTERN_NUM == i)
			printf("neg edge: delay=%d pass\n", delay);
	}

	free(pbuf);
	pbuf = NULL;
	printf("======== %s complete ========\n", __func__);
	return 0;
}
#endif

#define CMD_START_DATALEN_WITH_CHKSUM     0x5C
#define CMD_START_DATALEN_NO_CHKSUM       0x58
#define CMD_START_IMG_SIZE_OFF            72
#define CMD_START_IMG_CHKSUM_OFF          88
int cmd_start_transfer(fdl_frame_t * pframe, void *arg)
{
	int i;
	u64 temp;
	u16 ch16;
	struct fdl_file_info *pfile = (struct fdl_file_info *)arg;

	dl_buf_addr = FDL_BUF_ADDR;
	dl_buf_size = FDL_BUF_LEN;

	if (dl_buf_size == 0) {
		//error("fdl buf len is 0 bytes, cannot receive data\n");
		return -1;
	}
	memset((void *)pfile, 0, sizeof(struct fdl_file_info));

	/*
	 *                           cmd_start_trnafer format
	 *      | header | data_len | cmd | data area | checksum |
	 *      | 4B | 4B | 4B | par_id 72B + size 8B + rsv 8 + chksum 4B(op) | 2bytes |
	 */
	if (pframe->data_len == CMD_START_DATALEN_WITH_CHKSUM) {
		pfile->chksum_flag = 1;
		pfile->target_chksum = *(u32 *) (pframe->data + CMD_START_IMG_CHKSUM_OFF);
	} else if (pframe->data_len == CMD_START_DATALEN_NO_CHKSUM) {
		pfile->chksum_flag = 0;
		pfile->target_chksum = 0;
	} else {
		//error("cmd_start_tranfer format error\n");
		frame_send_respone(FDL_RESP_INVLID_CMD);
		return -1;
	}

	pfile->start_addr = dl_buf_addr;
	pfile->curr_addr = dl_buf_addr;

	for (i = 0; i < 31; i++) {
		ch16 = *(u16 *) (pframe->data + 2 * i);
		if (!ch16)
			break;
		pfile->part_name[i] = (char)ch16;
	}

	temp = *(u32 *) (pframe->data + CMD_START_IMG_SIZE_OFF + 4);
	temp = temp << 32;
	pfile->target_len = *(u32 *) (pframe->data + CMD_START_IMG_SIZE_OFF) | temp;	//combined to 64 bit data

	pfile->recv_len = 0;
	pfile->unsave_recv_len = 0;
	pfile->recv_chksum = 0;
	pfile->full_img_size = pfile->target_len;	// full pkg img size
	u64 part_addr, part_size = 0;
	if(0 == common_get_part_info(pfile->part_name, &part_addr, &part_size))
	{
		if(pfile->full_img_size <= part_size)
		{
			frame_send_respone(FDL_RESP_ACK);
		}
		else
		{
			printf("part name: %s, full_img_size: 0x%llx bigger than part size: 0x%llx, update failed!\n", pfile->part_name, pfile->full_img_size, part_size);
			frame_send_respone(FDL_RESP_SIZE_ERR);
			return -1;
		}
	}
	else
	{
		frame_send_respone(FDL_RESP_ACK);
	}
	return 0;
}

#define DEBUG_SPARSE_DL_ERASE_PART
int cmd_transfer_data(fdl_frame_t * pframe, void *arg)
{
	ulong free_buf_len;
	int ret, retlen;
	u32 checksum_en = 0;
	u32 chksum_flag = 0;
	u32 checksum = 0;
	struct fdl_file_info *pfile = (struct fdl_file_info *)arg;
#ifdef DEBUG_SPARSE_DL_ERASE_PART
	ulong sparse_buf_point, sparse_buf_last;
	u64 part_addr, part_size;
#endif

	pfile->target_len = *(u32 *) (pframe->data);	//1 pkg img size
	/*
	 * if the FDL buf size is too small
	 */
	if (pfile->target_len > dl_buf_size) {
		pr_err("cmd_transfer error, frame size is big than buf size\n");
		return -1;
	}

	if (pfile->recv_len + pfile->target_len > pfile->full_img_size) {
		pr_err("cmd_transfer error, recive data size mismatch\n");
		frame_send_respone(FDL_RESP_SIZE_ERR);
		return -1;
	}
	/* fisrt pkg
	 * 0x5C6D8E9F (pframe->data_len n) (pframe->cmd_index 0x00000002) (pframe->data size(4) + chksum_en(4) + chksum(4))     (checksum XXXX)
	 */
	checksum_en = *(u32 *) (pframe->data + 4);
	if (checksum_en == 1) {
		checksum = *(u32 *) (pframe->data + 8);	//1 pkg img chksum
		chksum_flag = 1;
	} else if (checksum_en == 0) {
		checksum = 0;
		chksum_flag = 0;
	}
	/* two resp pkg
	 * Header(4)    Payload Data Len(4)     CMD(4)  Payload Data(0) Check Sum(2)
	 */
	ret = frame_send_respone(FDL_RESP_ACK);
	if (ret)
		return ret;

	/* three resp pkg
	 * IMAGE RAW DATA
	 */
	retlen = g_CurrChannel->read(g_CurrChannel, (u8 *) pframe->data, pfile->target_len);	//read data to pframe->data (max 64K)
	if (retlen != pfile->target_len) {
		printf("read retlen error, retlen = %d,pfile->target_len = %lld\n", retlen, pfile->target_len);
		return -1;
	}
	/* Verify a package of img */
	if (chksum_flag && checksum != calc_image_checkSum((u8 *) pframe->data, pfile->target_len)) {
		ret = frame_send_respone(FDL_RESP_VERIFY_CHEKSUM_ERROR);
		if (ret)
			return ret;
	}

	if ((0 == pfile->recv_len) && is_sparse_image((void *)pframe->data)) {
		dl_buf_addr = SPARSE_IMAGE_BUF_ADDR;
		dl_buf_size = SPARSE_IMAGE_BUF_LEN;
		pfile->start_addr = dl_buf_addr;
		pfile->curr_addr = dl_buf_addr;
		sparse_download_enable = 1;
		if (sparse_info_init(&sparse, pfile->part_name)) {
			frame_send_respone(FDL_RESP_DEST_ERR);
			return -1;
		}
		printf("%s: part %s is sparse format, buf 0x%lX, size 0x%lX\n", __FUNCTION__, pfile->part_name,
		       dl_buf_addr, dl_buf_size);
#ifdef DEBUG_SPARSE_DL_ERASE_PART
		if (!common_get_part_info(pfile->part_name, &part_addr, &part_size)) {
			printf("%s: erase part %s, addr 0x%llX, size 0x%llX\n", __FUNCTION__, pfile->part_name,
			       part_addr, part_size);
			common_raw_erase(pfile->part_name, part_addr, part_size);
		}
#endif
	}
	if (pfile->unsave_recv_len + pfile->target_len > dl_buf_size) {
		/*
		 * the buf is 4KB size bundary
		 * fill some frame data to make buf full and then save to storage
		 */
		free_buf_len = dl_buf_size - pfile->unsave_recv_len;
		memcpy((void *)pfile->curr_addr, (void *)pframe->data, free_buf_len);
		pfile->unsave_recv_len = dl_buf_size;
		pfile->recv_len += free_buf_len;

		if (sparse_download_enable) {
			ret = write_sparse_img(&sparse, pfile->part_name, (void *)pfile->start_addr, &sparse_buf_point);
			if (ret || (sparse_buf_point < dl_buf_addr) || (sparse_buf_point > (dl_buf_addr + dl_buf_size))) {
				printf("cmd_transfer_data write sparse image fail, ret %d, end point addr 0x%lX\n", ret,
				       sparse_buf_point);
				frame_send_respone(FDL_RESP_DEST_ERR);
				return -2;
			}
			sparse_buf_last = dl_buf_addr + dl_buf_size - sparse_buf_point;
			if ((sparse_buf_last + pfile->target_len - free_buf_len) > dl_buf_size) {
				printf("%s: write sparse error, chunk size over 0x%llX\n", __FUNCTION__,
				       (dl_buf_size - pfile->target_len));
				frame_send_respone(FDL_RESP_SIZE_ERR);
				return -2;
			}
			if (sparse_buf_last) {
				printf("%s: move last buffer 0x%lX, size 0x%lX to 0x%lX\n", __FUNCTION__,
				       sparse_buf_point, sparse_buf_last, dl_buf_addr);
				memcpy((void *)dl_buf_addr, (void *)(ulong) sparse_buf_point, (ulong) sparse_buf_last);
			}
			/* reset the curr_addr unsave_recv_len */
			pfile->curr_addr = dl_buf_addr + sparse_buf_last;
			pfile->unsave_recv_len = sparse_buf_last;
			memcpy((void *)pfile->curr_addr, (void *)(pframe->data + free_buf_len),
			       (pfile->target_len - free_buf_len));
			pfile->unsave_recv_len += pfile->target_len - free_buf_len;
			pfile->curr_addr += pfile->target_len - free_buf_len;
			pfile->recv_len += pfile->target_len - free_buf_len;
		} else {
			ret = fdl_save_to_storage(pfile);

			if (ret) {
				pr_err("cmd_transfer_data save data to storage fail\n");
				frame_send_respone(FDL_RESP_DEST_ERR);
				return -2;
			}
			/* reset the curr_addr unsave_recv_len */
			pfile->curr_addr = dl_buf_addr;
			memcpy((void *)pfile->curr_addr, (void *)(pframe->data + free_buf_len),
			       (pfile->target_len - free_buf_len));
			pfile->unsave_recv_len = pfile->target_len - free_buf_len;
			pfile->curr_addr += pfile->target_len - free_buf_len;
			pfile->recv_len += pfile->target_len - free_buf_len;
		}

	} else {
		memcpy((void *)pfile->curr_addr, (void *)pframe->data, pfile->target_len);
		pfile->curr_addr += pfile->target_len;
		pfile->unsave_recv_len += pfile->target_len;
		pfile->recv_len += pfile->target_len;
	}

	if (pfile->chksum_flag)
		pfile->recv_chksum = fdl_checksum32(pfile->recv_chksum, pframe->data, pfile->target_len);

	/* four resp pkg
	 * Header(4)    Payload Data Len(4)     CMD(4)  Payload Data(0) Check Sum(2)
	 */
	ret = frame_send_respone(FDL_RESP_ACK);
	if (ret)
		return ret;

	return 0;
}

int cmd_transfer_end(fdl_frame_t * pframe, void *arg)
{
	int ret;
	//u32 img_chksum;
	struct fdl_file_info *pfile = (struct fdl_file_info *)arg;
	if (pfile->chksum_flag && (pfile->recv_chksum != pfile->target_chksum)) {
		//error("transfer_end img checksum error\n");
		frame_send_respone(FDL_RESP_VERIFY_CHEKSUM_ERROR);
		return -1;
	}

	if (pfile->recv_len != pfile->full_img_size) {
		//error("transfer_end img size mismatch error\n");
		frame_send_respone(FDL_RESP_SIZE_ERR);
		return -1;
	}

	printf("download image ok\n");

	if (pfile->unsave_recv_len) {
		if (sparse_download_enable) {
			ret = write_sparse_img(&sparse, pfile->part_name, (void *)pfile->start_addr, NULL);
			if (ret) {
				printf("transfer_end write sparse image fail\n");
				frame_send_respone(FDL_RESP_DEST_ERR);
				return -2;
			}
			sparse_download_enable = 0;
		} else {
			ret = fdl_save_to_storage(pfile);
			if (ret) {
				//error("transfer_end save data to storage fail\n");
				frame_send_respone(FDL_RESP_DEST_ERR);
				return -2;
			}
		}
	}

	frame_send_respone(FDL_RESP_ACK);
	return 0;
}

#define REPARTITION_PART_ENTRY_LEN      88
#define REPARTITION_PART_SIZE_OFF       72
#define REPARTITION_PART_GAP_OFF        80
#define FDL_PART_HEADER_LEN             8

extern struct boot_mode_info boot_info_data;
int cmd_repartition(fdl_frame_t * pframe, void *arg)
{
	if(boot_info_data.storage_sel != STORAGE_TYPE_EMMC && boot_info_data.storage_sel != STORAGE_TYPE_SSD) {
		frame_send_respone(FDL_RESP_ACK);
		return 0;
	}
	u16 i;
	u32 part_table_len;
	u64 temp;
	int len = 0, ret = 0;
	fdl_partition_header_t fdl_part_header;
	fdl_partition_t *fdl_part_table;
	u64 capacity_user;
	char *mmc_parts = NULL;
#if CONFIG_IS_ENABLED(BLK)
	struct mmc_uclass_priv *upriv;
	struct blk_desc *blk_dev_desc = NULL;
	struct mmc *mmc;
	(void)mmc;
	(void)upriv;
	u64 use_part_size = 0;
#endif
	void *fdl_part_cur;
	char buf[512] = { 0 };
	memcpy((void *)&fdl_part_header, (void *)pframe->data, FDL_PART_HEADER_LEN);
#ifdef CONFIG_TARGET_AX650_SSD
	int j;
	char *part_name;
	blk_dev_desc = blk_get_dev("nvme", 0);
	if (!blk_dev_desc) {
			printf("get nvme dev fail\n");
			return -1;
	}
	capacity_user = blk_dev_desc->lba * blk_dev_desc->blksz;
	part_name = malloc(32);
#else
#if CONFIG_IS_ENABLED(BLK)
	blk_dev_desc = blk_get_dev("mmc", EMMC_DEV_ID);
	if (!blk_dev_desc) {
		printf("get mmc dev fail\n");
		return -1;
	}
	upriv = blk_dev_desc->bdev->parent->uclass_priv;
	if (!upriv || !upriv->mmc) {
		printf("[%s]ERROR: parent->uclass_priv or uclass_priv->mmc is null\n", __FUNCTION__);
		return -1;
	}
	mmc = upriv->mmc;
	capacity_user = mmc->capacity_user;
	printf("mmc capacity_user is 0x%llx\n", mmc->capacity_user);
#endif
#endif

	if (fdl_part_header.dwMagic != PARTITION_HEADER_MAGIC) {
		pr_err("cmd_repartition partition header magic error\n");
		return -1;
	}

	part_table_len = fdl_part_header.part_count * sizeof(struct fdl_partition);
	fdl_part_table = (fdl_partition_t *) malloc(part_table_len);
	if (fdl_part_table == NULL) {
		return -1;
	}
	memset(fdl_part_table, 0, part_table_len);


	switch (boot_info_data.storage_sel) {
		case STORAGE_TYPE_EMMC:
			mmc_parts = strstr(BOOTARGS_EMMC, "mmcblk0:");
			len = (mmc_parts - BOOTARGS_EMMC) + strlen("mmcblk0:");
			memcpy(buf, BOOTARGS_EMMC, len);
			break;
		case STORAGE_TYPE_NAND:
			mmc_parts = strstr(BOOTARGS_SPINAND, "spi4.0:");
			len = (mmc_parts - BOOTARGS_SPINAND) + strlen("spi4.0:");
			memcpy(buf, BOOTARGS_SPINAND, len);
			break;
		case STORAGE_TYPE_NOR:
			mmc_parts = strstr(BOOTARGS_SPINOR, "spi4.0:");
			len = (mmc_parts - BOOTARGS_SPINOR) + strlen("spi4.0:");
			memcpy(buf, BOOTARGS_SPINOR, len);
			break;
		case STORAGE_TYPE_SSD:
			mmc_parts = strstr(BOOTARGS_SSD, "nvme0n1:");
			len = (mmc_parts - BOOTARGS_SSD) + strlen("nvme0n1:");
			memcpy(buf, BOOTARGS_SSD, len);
			break;
		default:
			ret = -1;
			printf("%s: %d, storage_sel %d error\n", __FUNCTION__, __LINE__, boot_info_data.storage_sel);
			break;
	}
	if(ret)
		frame_send_respone(FDL_RESP_INVALID_PARTITION);

	for (i = 0; i < fdl_part_header.part_count; i++) {
		fdl_part_cur = (void *)(pframe->data + FDL_PART_HEADER_LEN + i * REPARTITION_PART_ENTRY_LEN);

		memcpy((void *)fdl_part_table[i].part_id, (void *)fdl_part_cur, CMD_PART_ID_SIZE);
#ifdef CONFIG_TARGET_AX650_SSD
		for (j = 0; j < 32; j++) {
			part_name[j] = fdl_part_table[i].part_id[j];
		}

		if (!strcmp(part_name, "spl") || !strcmp(part_name, "uboot")
			|| !strcmp(part_name, "uboot_bk") || !strcmp(part_name, "env")) {
			continue;
		}
#endif
		temp = *(u32 *) (fdl_part_cur + REPARTITION_PART_SIZE_OFF + 4);
		temp = temp << 32;
		temp = *(u32 *) (fdl_part_cur + REPARTITION_PART_SIZE_OFF) | temp;
		fdl_part_table[i].part_sizeof_unit = temp;

		temp = *(u32 *) (fdl_part_cur + REPARTITION_PART_GAP_OFF + 4);
		temp = temp << 32;
		temp = *(u32 *) (fdl_part_cur + REPARTITION_PART_GAP_OFF) | temp;
		fdl_part_table[i].part_gapof_unit = temp;

		switch (fdl_part_header.size_unit) {
		case 0:
			fdl_part_table[i].sizeof_bytes = fdl_part_table[i].part_sizeof_unit * SZ_1M;
			fdl_part_table[i].gapof_bytes = fdl_part_table[i].part_gapof_unit * SZ_1M;
			break;
		case 1:
			fdl_part_table[i].sizeof_bytes = fdl_part_table[i].part_sizeof_unit * SZ_512K;
			fdl_part_table[i].gapof_bytes = fdl_part_table[i].part_gapof_unit * SZ_512K;
			break;
		case 2:
			fdl_part_table[i].sizeof_bytes = fdl_part_table[i].part_sizeof_unit * SZ_1K;
			fdl_part_table[i].gapof_bytes = fdl_part_table[i].part_gapof_unit * SZ_1K;
			break;
		case 3:
			fdl_part_table[i].sizeof_bytes = fdl_part_table[i].part_sizeof_unit * SZ_1;
			fdl_part_table[i].gapof_bytes = fdl_part_table[i].part_gapof_unit * SZ_1;
			break;
		default:
			fdl_part_table[i].sizeof_bytes = fdl_part_table[i].part_sizeof_unit * SZ_1M;
			fdl_part_table[i].gapof_bytes = fdl_part_table[i].part_gapof_unit * SZ_1M;
			break;
		}
#if CONFIG_IS_ENABLED(BLK)
		/* only the last partition can use the all rest blks */
		if (fdl_part_table[i].part_sizeof_unit == 0xffffffff) {
			if (i == (fdl_part_header.part_count - 1)) {
				if (fdl_part_header.size_unit == 3) {
					pr_err("The minimum partition unit is KByte, and bytes are not supported.\n");
					return -1;
				}
				if (fdl_part_header.size_unit == 0) {
					fdl_part_table[i].part_sizeof_unit = (capacity_user - use_part_size) / 1024 / 1024;	//M
				}
				if (fdl_part_header.size_unit == 1) {
					fdl_part_table[i].part_sizeof_unit =
					    (capacity_user - use_part_size) / 1024 / 512;
				}
				if (fdl_part_header.size_unit == 2) {
					fdl_part_table[i].part_sizeof_unit = (capacity_user - use_part_size) / 1024;	//K
				}

			} else {
				pr_err("not the last partition cannot use all rest blks\n");
				return -1;
			}
		} else {
			use_part_size += fdl_part_table[i].sizeof_bytes;
		}

		if (fdl_part_header.size_unit == 0)
			len += snprintf(buf + len, PAGE_SIZE - len, "%lldM(%ls),", fdl_part_table[i].part_sizeof_unit,
				     fdl_part_table[i].part_id);
		if (fdl_part_header.size_unit == 1)
			len += snprintf(buf + len, PAGE_SIZE - len, "%lldK(%ls),",
				     fdl_part_table[i].part_sizeof_unit * SZ_512, fdl_part_table[i].part_id);
		if (fdl_part_header.size_unit == 2)
			len += snprintf(buf + len, PAGE_SIZE - len, "%lldK(%ls),", fdl_part_table[i].part_sizeof_unit,
				     fdl_part_table[i].part_id);
#endif
	}
	/* remove , */
	buf[strlen(buf) - 1] = '\0';
	env_set("bootargs",buf);
	env_save(); //for set bootargs to env
#ifdef CONFIG_TARGET_AX650_SSD
	free(part_name);
#endif
	free(fdl_part_table);
	frame_send_respone(FDL_RESP_ACK);
	return 0;
}

int cmd_reboot(fdl_frame_t * pframe, void *arg)
{
	frame_send_respone(FDL_RESP_ACK);
	boot_info_data.mode = NORMAL_BOOT_MODE; //fake normal boot
	run_command_list("axera_boot", -1, 0);
	return 0;
}

int cmd_read_start(fdl_frame_t * pframe, void *arg)
{
	int i;
	u64 temp;
	u16 ch16;
	struct fdl_read_info *pfile = (struct fdl_read_info *)arg;

	printf("%s magic_num 0x%X, data_len 0x%X, cmd_index 0x%X\n", __FUNCTION__,
	       pframe->magic_num, pframe->data_len, pframe->cmd_index);

	dl_buf_addr = FDL_BUF_ADDR;
	dl_buf_size = FDL_BUF_LEN;
	if (dl_buf_size == 0) {
		//error("fdl buf len is 0 bytes, cannot receive data\n");
		return -1;
	}
	memset((void *)pfile, 0, sizeof(struct fdl_read_info));

	/*
	 *                           cmd_start_trnafer format
	 *      | header | data_len | cmd | data area | checksum |
	 *      | 4B | 4B | 4B | par_id 72B + size 8B + rsv 8 + chksum 4B(op) | 2bytes |
	 */
	for (i = 0; i < 31; i++) {
		ch16 = *(u16 *) (pframe->data + 2 * i);
		if (!ch16)
			break;
		pfile->part_name[i] = (char)ch16;
	}

	if (!pfile->part_name) {
		printf("%s: part name is null\n", __FUNCTION__);
		return -1;
	}

	if (0 != common_get_part_info(pfile->part_name, &pfile->part_base_addr, &pfile->part_size)) {
		frame_send_respone(FDL_RESP_INVALID_PARTITION);
		return -1;
	}
	printf("%s: part name %s, base addr 0x%llX, part size 0x%llX\n", __FUNCTION__,
	       pfile->part_name, pfile->part_base_addr, pfile->part_size);

	if (pframe->data_len == CMD_START_DATALEN_WITH_CHKSUM) {
		pfile->chksum_flag = 1;
	} else if (pframe->data_len == CMD_START_DATALEN_NO_CHKSUM) {
		pfile->chksum_flag = 0;
		pfile->send_chksum = 0;
	} else {
		//error("cmd_start_tranfer format error\n");
		frame_send_respone(FDL_RESP_INVLID_CMD);
		return -1;
	}

	temp = *(u32 *) (pframe->data + CMD_START_IMG_SIZE_OFF + 4);
	temp = temp << 32;
	pfile->request_len = *(u32 *) (pframe->data + CMD_START_IMG_SIZE_OFF) | temp;	//combined to 64 bit data
	printf("%s: request len 0x%llX, chksum_flag 0x%X\n", __FUNCTION__, pfile->request_len, pfile->chksum_flag);
	if (pfile->request_len > pfile->part_size) {
		printf("exceed partition size\n");
		frame_send_respone(FDL_RESP_SIZE_ERR);
		return -1;
	}

	pfile->buf_addr = dl_buf_addr;
	pfile->buf_point = dl_buf_addr;
	pfile->buf_len = 0;
	pfile->buf_send_len = 0;
	pfile->total_send_len = 0;
	pfile->part_read_size = 0;

	frame_send_respone(FDL_RESP_ACK);

	return 0;
}

int cmd_read_data(fdl_frame_t * pframe, void *arg)
{
	u64 buf_last_len, buf_free_len;
	u64 part_last_len, read_len;
	u64 temp;
	//u64 pkg_offset;
	u32 pkg_len;
	struct fdl_read_info *pfile = (struct fdl_read_info *)arg;
	pkg_len = *(u32 *) (pframe->data + 8);
	temp = *(u32 *) (pframe->data + 4);
	temp = temp << 32;

	if (pfile->buf_send_len + pkg_len > pfile->buf_len) {
		if (pfile->buf_point != pfile->buf_addr) {
			if (pfile->buf_len > pfile->buf_send_len) {
				buf_last_len = pfile->buf_len - pfile->buf_send_len;
			} else {
				printf("ERROR: buf_send_len 0x%lX over buf_len 0x%lX\n", pfile->buf_send_len,
				       pfile->buf_len);
				return -1;
			}
			memcpy((void *)pfile->buf_addr, (void *)pfile->buf_point, buf_last_len);
			pfile->buf_len = buf_last_len;
			pfile->buf_send_len = 0;
			printf
			    ("[Move tail buf_point 0x%lX to buf_addr 0x%lX] buf_len 0x%lX, buf_send_len 0x%lX, total_send_len 0x%llX, part_read_size 0x%llX\n",
			     pfile->buf_point, pfile->buf_addr, pfile->buf_len, pfile->buf_send_len,
			     pfile->total_send_len, pfile->part_read_size);
			pfile->buf_point = pfile->buf_addr;
		}

		if (pfile->buf_point == pfile->buf_addr) {
			if (pfile->buf_len >= dl_buf_size) {
				printf("ERROR: unsend buf len 0x%lX over buf limit size 0x%lX\n", pfile->buf_len,
				       dl_buf_size);
			} else {
				buf_free_len = dl_buf_size - pfile->buf_len;

				if (pfile->part_read_size >= pfile->request_len) {
					printf("ERROR: read size 0x%llX over request len 0x%llX\n",
					       pfile->part_read_size, pfile->request_len);
					return -1;
				}
				part_last_len = pfile->request_len - pfile->part_read_size;

				temp = (part_last_len < buf_free_len) ? part_last_len : buf_free_len;
				read_len =
				    common_raw_read(pfile->part_name, (pfile->part_base_addr + pfile->part_read_size),
						    temp, (char *)(pfile->buf_point + pfile->buf_len));
				if (read_len != temp) {
					printf("ERROR: result read len 0x%llX, read size 0x%llx\n", read_len, temp);
					return -1;
				}
				printf("[Read 0x%llX bytes to addr 0x%lX] ", temp, (pfile->buf_point + pfile->buf_len));
				pfile->part_read_size += temp;
				pfile->buf_len += temp;
				printf
				    ("buf_point 0x%lX, buf_len 0x%lX, buf_send_len 0x%lX, total_send_len 0x%llX, part_read_size 0x%llX\n",
				     pfile->buf_point, pfile->buf_len, pfile->buf_send_len, pfile->total_send_len,
				     pfile->part_read_size);
			}
		}
	}

	if (pfile->buf_send_len + pkg_len <= pfile->buf_len) {
		frame_send_data(FDL_RESP_FLASH_DATA, (char *)pfile->buf_point, pkg_len);
		pfile->buf_send_len += pkg_len;
		pfile->buf_point += pkg_len;
		pfile->total_send_len += pkg_len;
		if (pfile->buf_len == pfile->buf_send_len) {
			pfile->buf_point = pfile->buf_addr;
			pfile->buf_len = 0;
			pfile->buf_send_len = 0;
		}
		if (pfile->total_send_len == pfile->request_len)
			printf
			    ("after send frame buf_addr 0x%lX, buf_point 0x%lX, buf_len 0x%lX, buf_send_len 0x%lX, total_send_len 0x%llX, part_read_size 0x%llX\n",
			     pfile->buf_addr, pfile->buf_point, pfile->buf_len, pfile->buf_send_len,
			     pfile->total_send_len, pfile->part_read_size);
	}

	return 0;
}

int cmd_read_end(fdl_frame_t * pframe, void *arg)
{
	printf("%s magic_num 0x%X, data_len 0x%X, cmd_index 0x%X\n", __FUNCTION__,
	       pframe->magic_num, pframe->data_len, pframe->cmd_index);

	frame_send_respone(FDL_RESP_ACK);
	return 0;
}

int cmd_erase(fdl_frame_t * pframe, void *arg)
{
	int i;
	u64 temp, flag, size, part_addr, part_size;
	u16 ch16;
	char name[36] = { 0 };
	printf("%s: magic_num 0x%X, data_len 0x%X, cmd_index 0x%X\n", __FUNCTION__,
	       pframe->magic_num, pframe->data_len, pframe->cmd_index);

	temp = *(u32 *) (pframe->data + 4);
	temp = temp << 32;
	flag = *(u32 *) (pframe->data) | temp;	//combined to 64 bit data

	if (flag == 1) {
		printf("%s: flag 0x%llX\n", __FUNCTION__, flag);
		if (common_raw_erase("eraseall", 0, 0)) {
			frame_send_respone(FDL_RESP_OPERATION_FAIL);
			return -1;
		}
	} else {
		for (i = 0; i < 35; i++) {
			ch16 = *(u16 *) (pframe->data + sizeof(flag) + 2 * i);
			if (!ch16)
				break;
			name[i] = (char)ch16;
		}

		temp = *(u32 *) (pframe->data + sizeof(flag) + sizeof(name) * sizeof(ch16) + 4);
		temp = temp << 32;
		size = *(u32 *) (pframe->data + sizeof(flag) + sizeof(name) * sizeof(ch16)) | temp;	//combined to 64 bit data
		printf("%s: flag 0x%llX, part %s, erase size 0x%llX\n", __FUNCTION__, flag, name, size);

		if (0 != common_get_part_info(name, &part_addr, &part_size)) {
			frame_send_respone(FDL_RESP_DEST_ERR);
			return -1;
		}

		if (size > part_size) {
			frame_send_respone(FDL_RESP_SIZE_ERR);
			return -1;
		}
		if (size == 0) {
			size = part_size;
		}
		if (common_raw_erase(name, part_addr, size)) {
			frame_send_respone(FDL_RESP_OPERATION_FAIL);
			return -1;
		}
	}

	frame_send_respone(FDL_RESP_ACK);
	env_load();
	return 0;
}

int cmd_set_baudrate(fdl_frame_t * pframe, void *arg)
{
	int ret;
	u32 baudrate_new;

	baudrate_new = *((u32 *) pframe->data);
	frame_send_respone(FDL_RESP_ACK);
	ret = g_CurrChannel->setbaudrate(g_CurrChannel, baudrate_new);
	if (ret)
		while (1) ;

	return 0;
}

int cmd_connect(fdl_frame_t * pframe, void *arg)
{
	int ret;

	ret = frame_send_respone(FDL_RESP_ACK);
	if (ret)
		return ret;

	return 0;
}

static void cmdproc_register(CMD_TYPE cmd, cmd_handler_t handler, void *arg)
{
	//cmdproc_tab[cmd - FDL_CMD_TYPE_MIN].cmd = cmd;
	cmdproc_tab[cmd - FDL_CMD_TYPE_MIN].handler = handler;
	cmdproc_tab[cmd - FDL_CMD_TYPE_MIN].arg = arg;
}

void fdl_dl_init(void)
{
	memset(cmdproc_tab, 0, sizeof(cmdproc_tab));

	cmdproc_register(FDL_CMD_CONNECT, cmd_connect, 0);
	cmdproc_register(FDL_CMD_START_TRANSFER, cmd_start_transfer, &g_file_info);
	cmdproc_register(FDL_CMD_TRANSFERRING_DATA, cmd_transfer_data, &g_file_info);
	cmdproc_register(FDL_CMD_START_TRANSFER_END, cmd_transfer_end, &g_file_info);
	cmdproc_register(FDL_CMD_REPATITION, cmd_repartition, 0);
	cmdproc_register(FDL_CMD_REBOOT, cmd_reboot, 0);
	cmdproc_register(FDL_CMD_START_READ_FLASH, cmd_read_start, &g_read_info);
	cmdproc_register(FDL_CMD_READING_FLASH, cmd_read_data, &g_read_info);
	cmdproc_register(FDL_CMD_END_READ_FLASH, cmd_read_end, &g_read_info);
	cmdproc_register(FDL_CMD_ERASE_FLASH, cmd_erase, 0);
	cmdproc_register(FDL_CMD_CHG_BAUDRATE, cmd_set_baudrate, 0);
	//others add later
#ifdef FDL_BAD_BLKS_SCAN
	spi_nand_scan_bad_blks();
#endif
#ifdef SPI_RX_SAMPLE_DLY_SCAN
	spi_rx_sample_delay_scan(4);
#endif
}

void fdl_dl_entry(void)
{
	int ret;
	CMD_TYPE cmd;
	fdl_frame_t *pframe;
	while (1) {
		pframe = frame_get();
		if (!pframe)
			while (1) ;

		cmd = pframe->cmd_index;
		ret = CMD_HANDLER(cmd, pframe);
		if (ret)
			return;
	}
}
