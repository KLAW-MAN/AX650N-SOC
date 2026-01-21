#include <common.h>
#include <asm/arch/boot_mode.h>
#include <fs.h>
#include <blk.h>
#include <memalign.h>
#include <fat.h>
#include <linux/sizes.h>
#include <asm/io.h>
#include <asm/arch/ax650.h>
#include <image-sparse.h>
#include "../secureboot/secureboot.h"
#include <dm/uclass.h>
#include <dm/device.h>
#include <mtd.h>
#include <mmc.h>
#include "../../legacy-mtd-utils.h"

#include "axera_update.h"

#define INFO_PART_START_STR    "<Partition"
#define PART_DATA_UNIT_STR     "unit"
#define PART_DATA_ID_STR       "id"
#define PART_DATA_SIZE_STR     "size"
#define INFO_PART_END_STR      "</Partitions>"
extern struct boot_mode_info boot_info_data;

static char *extract_val_space(const char *str, const char *key)
{
	char *v, *k;
	char *s, *strcopy;
	char *new = NULL;

	strcopy = strdup(str);
	if (strcopy == NULL)
		return NULL;

	s = strcopy;
	while (s) {
		v = strsep(&s, " ");
		if (!v)
			break;
		k = strsep(&v, "=");
		if (!k)
			break;

		while (*k != 0) {
			if  (strcmp(k, key) == 0) {
				new = strdup(v);
				goto free_ret;
			}
			k++;
		}
	}

free_ret:
	free(strcopy);
	return new;
}

extern struct boot_mode_info boot_info_data;
u64 get_capacity_user(void)
{
	if(boot_info_data.storage_sel == STORAGE_TYPE_EMMC) {
		#if CONFIG_IS_ENABLED(BLK)
		struct mmc_uclass_priv *upriv = NULL;
		struct blk_desc *blk_dev_desc = NULL;
		struct mmc *mmc = NULL;

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
		printf("mmc capacity_user is 0x%llx\n",mmc->capacity_user);
		return mmc->capacity_user;
		#endif
	}
	if(boot_info_data.storage_sel == STORAGE_TYPE_NAND) {
		int ret = -1;
		u32 busnum = 0;
		struct udevice *dev = NULL;
		struct mtd_info *mtd = NULL;

		ret = uclass_get_device(UCLASS_MTD, busnum, &dev);
		if (ret) {
			printf("uclass_get_device: Invalid bus %d (err=%d)\n", busnum, ret);
			return -1;
		}

		mtd = dev_get_uclass_priv(dev);
		printf("nand mtd->size is 0x%llx\n",mtd->size);
		return mtd->size;
	}
	if(boot_info_data.storage_sel == STORAGE_TYPE_SSD) {
		lbaint_t lba;
		struct blk_desc *blk_dev_desc = NULL;

		blk_dev_desc = blk_get_dev("nvme", 0);
		if (!blk_dev_desc) {
			printf("get nvme dev fail\n");
			return -1;
		}

		lba = blk_dev_desc->lba;
		return (lba * blk_dev_desc->blksz);
	}
	return 0;
}

int get_part_info_rawdata(struct update_part_info **part_list, char * src, int len)
{
	int ret = -1;
	int count = 0;
	int step = 0;
	int autoresize_flag = 0;
	uint part_size_unit;
	struct update_part_info *pheader = NULL;
	struct update_part_info *curr = NULL;
	struct update_part_info *pbin_info;
	char *tok, *val, *str, *p, *tmpstr;
	u64 capacity_user;
	u64 use_part_size = 0;
	str = src;
	capacity_user = get_capacity_user();
	do {
		if ((0 == step) && !memcmp(str, INFO_PART_START_STR, strlen(INFO_PART_START_STR))) {
			tok = strsep(&str, ">");

			val = extract_val_space(tok, PART_DATA_UNIT_STR);
			if (!val) {
				return -1;
			}
			printf("partinfo start unit val %s\n", val);
			if (strlen(val) <= 2) {
				free(val);
				return -1;
			}
			part_size_unit = *(val+1);
			if ('0' == part_size_unit) {
				part_size_unit = 1024 * 1024;
			}
			else if ('1' == part_size_unit) {
				part_size_unit = 1024 * 512;
			}
			else if ('2' == part_size_unit) {
				part_size_unit = 1024;
			}
			else {
				printf("part size unit not support\n");
				free(val);
				return -1;
			}
			free(val);
			step += 1;
		}

		if (1 == step) {
			if (!memcmp(str, INFO_PART_END_STR, strlen(INFO_PART_END_STR))) {
				if (curr)
					curr->next = NULL;
				step += 1;
				break;
			}

			if (!memcmp(str, INFO_PART_START_STR, strlen(INFO_PART_START_STR))) {
				if (autoresize_flag) {
					printf("[%s]err: auto resize part must be last part\n", __FUNCTION__);
					if (curr)
						curr->next = NULL;
					ret = -1;
					goto free_pbin_node;
				}
				tok = strsep(&str, ">");

				val = extract_val_space(tok, PART_DATA_ID_STR);
				if (!val) {
					ret = -1;
					goto free_pbin_node;
				}
				if (strlen(val) <= 2) {
					free(val);
					ret = -1;
					goto free_pbin_node;
				}
				pbin_info = malloc(sizeof(struct update_part_info));
				if (!pbin_info) {
					free(val);
					ret = -1;
					goto free_pbin_node;
				}
				memset(pbin_info, 0, sizeof(struct update_part_info));
				strncpy(pbin_info->part_name, (val+1), (strlen(val)-2));
				free(val);

				tmpstr = extract_val_space(tok, PART_DATA_SIZE_STR);
				if (!tmpstr) {
					ret = -1;
					goto free_pbin_node;
				}
				if (strlen(tmpstr) <= 2) {
					free(tmpstr);
					ret = -1;
					goto free_pbin_node;
				}
				val = malloc(strlen(tmpstr));
				if (!val) {
					free(tmpstr);
					ret = -1;
					goto free_pbin_node;
				}
				strcpy(val, tmpstr+1);
				free(tmpstr);
				val[strlen(val)-1] = '\0';
				p = val;
				if (!strcmp(val, "0xFFFFFFFF") || !strcmp(val, "0xffffffff") || !strcmp(val, "0XFFFFFFFF") || !strcmp(val, "0Xffffffff")) {
					#if CONFIG_IS_ENABLED(BLK)
						if(capacity_user > 0 && capacity_user != -1)
							pbin_info->part_size = capacity_user - use_part_size;
					#else
						pbin_info->part_size = -1;
					#endif
					autoresize_flag = 1;
				}
				else {
					pbin_info->part_size = ustrtoull(p, &p, 0);
					pbin_info->part_size *= part_size_unit;
				}
				free(val);

				if (!pheader) {
					pheader = pbin_info;
					curr = pbin_info;
				} else {
					curr->next = pbin_info;
					curr = pbin_info;
				}
				count += 1;
#ifdef CONFIG_TARGET_AX650_SSD
				if (!strcmp(curr->part_name, "uboot") || !strcmp(curr->part_name, "uboot_bk")
						|| !strcmp(curr->part_name, "spl") || !strcmp(curr->part_name, "env"))
						continue;
#endif
				use_part_size += pbin_info->part_size;
				printf("curr :%d part %s, size %llX, use_part_size 0x%llX\n", count, curr->part_name, curr->part_size, use_part_size);
			}
		}
	} while (++str < src + len);

	if (step < 2) {
		printf("%s: part info error, end at step %d\n", __FUNCTION__, step);
		ret = -1;
		goto free_pbin_node;
	}

	ret = count;
	*part_list = pheader;
	printf("successfully parsed part info\n");
	return ret;

free_pbin_node:
	while (pheader) {
		pbin_info = pheader;
		pheader = pheader->next;
		free(pbin_info);
	}

	return ret;
}

int update_parts_info(struct update_part_info *pheader)
{
	int i, part_size_KB, part_size_MB;
	int part_count = 0;
	int len = 0;
	struct update_part_info *pcur = pheader;
	char buf[512] = {0};
	char *mmc_parts = NULL;

	while (pcur) {
		part_count++;
		pcur = pcur->next;
	}
	if (0 == part_count) {
		printf("%s: no valid part\n", __func__);
		return -1;
	}
	switch (boot_info_data.storage_sel) {
		case STORAGE_TYPE_EMMC:
			mmc_parts = strstr(BOOTARGS_EMMC, "mmcblk0:");
			len = (mmc_parts - BOOTARGS_EMMC) + strlen("mmcblk0:");
			memcpy(buf, BOOTARGS_EMMC, len);
			break;
		case STORAGE_TYPE_NAND:
			mmc_parts = strstr(BOOTARGS_SPINAND, "spi5.0:");
			len = (mmc_parts - BOOTARGS_SPINAND) + strlen("spi5.0:");
			memcpy(buf, BOOTARGS_SPINAND, len);
			break;
		case STORAGE_TYPE_NOR:
			mmc_parts = strstr(BOOTARGS_SPINOR, "spi5.0:");
			len = (mmc_parts - BOOTARGS_SPINOR) + strlen("spi5.0:");
			memcpy(buf, BOOTARGS_SPINOR, len);
			break;
		case STORAGE_TYPE_SSD:
			mmc_parts = strstr(BOOTARGS_SSD, "nvme0n1:");
			len = (mmc_parts - BOOTARGS_SSD) + strlen("nvme0n1:");
			memcpy(buf, BOOTARGS_SSD, len);
			break;
		default:
			printf("%s: %d, storage_sel %d error\n", __FUNCTION__, __LINE__, boot_info_data.storage_sel);
			break;
	}
	pcur = pheader;
	for (i = 0; i < part_count; i++) {
		if (0 != (pcur->part_size % 1024)) {
			printf("%s: part size 0x%llX must be 1KB aligned.\n", pcur->part_name, pcur->part_size);
			return -1;
		}

		if (0 == (pcur->part_size % (1024 * 1024))) {
			part_size_MB = pcur->part_size / (1024 * 1024);
			printf("%s: %d MB\n", pcur->part_name, part_size_MB);
#ifdef CONFIG_TARGET_AX650_SSD
			if (!strcmp(pcur->part_name, "uboot") || !strcmp(pcur->part_name, "uboot_bk")
				|| !strcmp(pcur->part_name, "env")) {
					pcur = pcur->next;
					continue;
			}
#endif
			len += snprintf(buf + len, PAGE_SIZE-len, "%dM(%s),", part_size_MB, pcur->part_name);
		}
		else {
			part_size_KB = pcur->part_size / 1024;
			printf("%s: %d KB.\n", pcur->part_name, part_size_KB);
#ifdef CONFIG_TARGET_AX650_SSD
			if (!strcmp(pcur->part_name, "uboot") || !strcmp(pcur->part_name, "uboot_bk")
				|| !strcmp(pcur->part_name, "env")) {
					pcur = pcur->next;
					continue;
			}
#endif
			len += snprintf(buf + len, PAGE_SIZE-len, "%dK(%s),", part_size_KB, pcur->part_name);
		}

		pcur = pcur->next;
	}

	buf[strlen(buf)-1] = '\0';

	// printf("buf = %s\n",buf);
	env_set("bootargs",buf);
	return 0;
}

#ifdef VERSION_2_PARSE_XML
int find_update_xml_name(char *xml_name, struct blk_desc *blk_desc)
{
	struct fs_dir_stream *dirs;
	struct fs_dirent *dent;
	char *p = NULL;
	int ret = -1;

	if (fs_set_blk_dev_with_part(blk_desc, 1)) {
		printf("%s fs_set_blk_dev_with_part try part1 failed, try part0\n", __func__);
		if (fs_set_blk_dev_with_part(blk_desc, 0)) {
			printf("%s fs_set_blk_dev_with_part try part0 failed\n", __func__);
			return -1;
		}
	}

	dirs = fs_opendir("/");
	if (!dirs) {
		printf("%s open root directory fail\n", __func__);
		return -1;
	}
	while ((dent = fs_readdir(dirs))) {
		if (dent->type != FS_DT_DIR) {
			if(!strncasecmp(dent->name, XML_PREFIX_STR, strlen(XML_PREFIX_STR))) {
				p = strrchr(dent->name, '.');
				if (p && !strncasecmp(p, XML_POSTFIX_STR, strlen(XML_POSTFIX_STR))) {
					strncpy(xml_name, dent->name, MAX_FILE_NAME_LEN);
					ret = 0;
					break;
				}
			}
		}
	}

	fs_closedir(dirs);
	return ret;
}

static int update_parse_xml(struct update_part_info **bin_info, struct blk_desc *blk_desc)
{
	int ret = 0;
	int is_uboot_found = 0;
	loff_t read_len, size;
	struct update_part_info *pheader = NULL;
	struct update_part_info *pcurr = NULL;
	struct update_part_info *pbin_info;
	char *part_data = NULL;
	char file_name[MAX_FILE_NAME_LEN] = {0};
	char xml_name[MAX_FILE_NAME_LEN] = {0};

	if(find_update_xml_name(xml_name, blk_desc)) {
		printf("update config file is not exist, exit update\n");
		return -1;
	}

	printf("\nstart parse %s...\n", xml_name);
	if (!fat_size(xml_name, &size)) {
		part_data = (char *)malloc(size + 1);
		if (!part_data) {
			printf("update malloc part_data space fail, exit update\n");
			return -1;
		}
		memset(part_data, 0, size + 1);
	} else {
			printf("get xml file size fail, exit update\n");
			return -1;
	}

	ret = fat_read_file(xml_name, part_data, 0, size, &read_len);
	if (ret || size != read_len) {
		printf("read config file fail, exit update\n");
		goto free_data;
	}

	if (get_part_info_rawdata(&pheader, part_data, size) < 0) {
		printf("%s: part info error\n", __FUNCTION__);
		ret = -1;
		goto free_part_list;
	}

	pcurr = pheader;
	while (pcurr) {
		if (!strcmp(pcurr->part_name, "uboot")) {
			is_uboot_found = 1;
		}

		pcurr = pcurr->next;
	}

	if (0 == is_uboot_found) {
		pbin_info = malloc(sizeof(struct update_part_info));
		if (!pbin_info) {
			ret = -1;
			goto free_part_list;
		}
		memset(pbin_info, 0, sizeof(struct update_part_info));
		strcpy(pbin_info->part_name, "uboot");
		pbin_info->part_size = UBOOT_MAX_SIZE;
		pbin_info->next = pheader;
		pheader = pbin_info;
		pbin_info = NULL;
	}

	pcurr = pheader;
	while (pcurr) {
		strcpy(file_name, pcurr->part_name);
		if (!strcmp(pcurr->part_name, "uboot")) {
			strcat(file_name, ".bin");
		}
		else {
			strcat(file_name, ".img");
		}

		if (fat_exists(file_name)) {
			strcpy(pcurr->file_name, file_name);
		}
		else {
			strcpy(pcurr->file_name, "none");
		}
		printf("part:%s, updating file:%s\n", pcurr->part_name, pcurr->file_name);
		memset(file_name, 0, sizeof(file_name));
		pcurr = pcurr->next;
	}

	*bin_info = pheader;
	printf("successfully parsed image file\n\n");

	goto free_data;

free_part_list:
	while (pheader) {
		pbin_info = pheader;
		pheader = pheader->next;
		free(pbin_info);
	}

free_data:
	free(part_data);

	return ret;
}

int update_parse_part_info(struct update_part_info **bin_info, struct blk_desc *blk_desc)
{
	int ret = 0;

	if (SD_UPDATE_MODE == boot_info_data.mode  ||
		boot_info_data.mode == USB_UPDATE_MODE ||
		boot_info_data.mode == NORMAL_BOOT_MODE
		) {
		ret = update_parse_xml(bin_info, blk_desc);
		printf("%s: fs loop ret %d\n", __FUNCTION__, ret);
	}

	return ret;
}
#endif
