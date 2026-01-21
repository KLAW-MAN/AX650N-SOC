/*
 * AXERA AX650 Controller Interface
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

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
#include "../../legacy-mtd-utils.h"
#include "axera_update.h"
#include <mmc.h>
#include <dm.h>
#include <dm/device-internal.h>
#include <dm/device_compat.h>
#include <dm/lists.h>
#include <linux/compat.h>
#include <asm/io.h>
#include <mapmem.h>
#include <part.h>
#include <fat.h>
#include <fs.h>
#include <rtc.h>
#include <linux/time.h>

#define DUMP_FILE_NAME_LEN 32
#define AXERA_REASON_MASK 0xf98e7c6d

extern struct boot_mode_info boot_info_data;
extern boot_mode_info_t *get_dl_and_boot_info(void);
extern u32 dump_reason;
extern int get_board_id(void);

char dump_file_name[DUMP_FILE_NAME_LEN];
char dump_info_name[DUMP_FILE_NAME_LEN];

struct axera_memory_dump_struct {
	u64 axera_dump_info_addr;
	u64 axera_dump_info_size;
	u64 axera_dump_addr;
	u64 axera_dump_size;
};

#if defined CONFIG_AXERA_MEMORY_DUMP_SD || CONFIG_AXERA_MEMORY_DUMP_EMMC || CONFIG_AXERA_MEMORY_DUMP_USB_STORAGE || CONFIG_AXERA_MEMORY_DUMP_PCIE || CONFIG_AXERA_MEMORY_DUMP_SSD
int last_kernel_dtb_check(struct axera_memory_dump_struct *ax_info)
{
	int offset;
	int len;
	u64 addr, size, memory_addr;
	const u32 *val;
	char *fdt = (char *)DTB_IMAGE_ADDR;

	if (fdt_check_header(fdt)) {
		printf("Invalid device tree header \n");
		return -1;
	}

	offset = fdt_path_offset(fdt, "/reserved-memory/axera_memory_dump@0");
	if (offset < 0)
		printf("reserved_mem error \n");

	val = fdt_getprop(fdt, offset, "reg", &len);
	addr = fdt32_to_cpu(val[0]);
	addr = addr << 32;
	addr |= fdt32_to_cpu(val[1]);

	size = fdt32_to_cpu(val[2]);
	size = size << 32;
	size |= fdt32_to_cpu(val[3]);

	printf("addr = 0x%llx  size = 0x%llx\n", addr, size);

	offset = fdt_path_offset(fdt, "/memory@100000000");
	if (offset < 0)
		printf("memory node error \n");

	val = fdt_getprop(fdt, offset, "reg", &len);
	memory_addr = fdt32_to_cpu(val[0]);
	memory_addr = memory_addr << 32;
	memory_addr |= fdt32_to_cpu(val[1]);
	printf("memory_addr = 0x%llx\n", memory_addr);

	ax_info->axera_dump_info_addr = addr;
	ax_info->axera_dump_info_size = size;
	ax_info->axera_dump_addr = memory_addr;
	ax_info->axera_dump_size = readl(addr+1048)*SZ_1M;
	return 0;
}

static int  get_boot_reason_mask(u32 boot_reason,struct axera_memory_dump_struct  *axera_dump_info)
{
	u32 reason_mask;
	reason_mask = readl(axera_dump_info->axera_dump_info_addr+1024);
        pr_err("boot_reason = 0x%x reason_mask = 0x%x\n",boot_reason,reason_mask);
	return reason_mask;
}

static void  clear_boot_reason_mask(struct axera_memory_dump_struct  *axera_dump_info)
{
	printf("clean reason_mask\n");
        writel(0x0,(axera_dump_info->axera_dump_info_addr+1024));
}

struct dump_tm {
        /*
         * the number of seconds after the minute, normally in the range
         * 0 to 59, but can be up to 60 to allow for leap seconds
         */
	int tm_sec;
	/* the number of minutes after the hour, in the range 0 to 59*/
	int tm_min;
	/* the number of hours past midnight, in the range 0 to 23 */
	int tm_hour;
	/* the day of the month, in the range 1 to 31 */
	int tm_mday;
	/* the number of months since January, in the range 0 to 11 */
	int tm_mon;
	/* the number of years since 1900 */
	long tm_year;
	/* the number of days since Sunday, in the range 0 to 6 */
	int tm_wday;
	/* the number of days since January 1, in the range 0 to 365 */
	int tm_yday;
};
void display_kernel_time(struct axera_memory_dump_struct *axera_dump_info,int flage)
{
	struct dump_tm tm_local;
	memcpy((void *)&tm_local, (void *)(axera_dump_info->axera_dump_info_addr+1088), sizeof(tm_local));
	printf("last kernel panic time: %04ld-%02d-%02d %02d:%02d:%02d\n",tm_local.tm_year+1900, tm_local.tm_mon + 1, tm_local.tm_mday,tm_local.tm_hour, tm_local.tm_min, tm_local.tm_sec);
	memset(dump_file_name,0,DUMP_FILE_NAME_LEN);
	if(flage == 1)
	{
		sprintf(dump_file_name,"vmcore.dump.%04ld%02d%02d%02d%02d%02d.bin",tm_local.tm_year + 1900, tm_local.tm_mon + 1, tm_local.tm_mday,tm_local.tm_hour, tm_local.tm_min, tm_local.tm_sec);
		sprintf(dump_info_name,"vmcore.dump.info.%04ld%02d%02d%02d%02d%02d.bin",tm_local.tm_year + 1900, tm_local.tm_mon + 1, tm_local.tm_mday,tm_local.tm_hour, tm_local.tm_min, tm_local.tm_sec);
	} else {
		sprintf(dump_file_name,"/vmcore.dump.%04ld%02d%02d%02d%02d%02d.bin",tm_local.tm_year + 1900, tm_local.tm_mon + 1, tm_local.tm_mday,tm_local.tm_hour, tm_local.tm_min, tm_local.tm_sec);
		sprintf(dump_info_name,"/vmcore.dump.info.%04ld%02d%02d%02d%02d%02d.bin",tm_local.tm_year + 1900, tm_local.tm_mon + 1, tm_local.tm_mday,tm_local.tm_hour, tm_local.tm_min, tm_local.tm_sec);
	}
}
#endif

#ifdef CONFIG_SLAVE_PCIE_BOOT
static int dump_memory_to_pcie(u32 boot_reason)
{
	int ret;
	struct axera_memory_dump_struct  axera_dump_info;

	ret = last_kernel_dtb_check(&axera_dump_info);
	if(ret < 0)
		return -1;

	printf("pcie ep memory dump...\n");

	return 0;
}
#endif

#ifdef CONFIG_AXERA_MEMORY_DUMP_USB_STORAGE
static int dump_memory_to_usb_storage(u32 boot_reason)
{
	struct blk_desc *usb_stor_desc = NULL;
	int ret;
	loff_t size;
	unsigned long addr;
	unsigned long count;
	long offset;
	u32 reason_mask;

	void *buf;
	struct axera_memory_dump_struct  axera_dump_info;
	ret = last_kernel_dtb_check(&axera_dump_info);
	if(ret < 0)
		return -1;

	printf("usb-storage memory dump...\n");

	reason_mask = get_boot_reason_mask(boot_reason,&axera_dump_info);
	if((boot_reason != 0) && (reason_mask == AXERA_REASON_MASK)) {

		ret = run_command("usb start", 0);
		if (ret) {
			printf("memory dump usb start error %d\n", ret);
			return -1;
		}

		/* step1 check usb-storage is present */
		usb_stor_desc = blk_get_dev("usb", 0);
		if (NULL == usb_stor_desc) {
			printf("usb-storage is not present, exit dump\n");
			return -1;
		}

		/* we register usb to fatfs */
		if (fat_register_device(usb_stor_desc, 1)) {
			printf("memory dump usb-storage register part1 fat fail, try part0\n");

			if (fat_register_device(usb_stor_desc, 0)) { /* in normal condition, part0 is MBR */
			printf("usb-storage register part0 fat fail, exit usb-storage memory dump\n");
			return -1;
			}
		}

		display_kernel_time(&axera_dump_info,1);

		addr =  axera_dump_info.axera_dump_info_addr;
		count = axera_dump_info.axera_dump_info_size;
		offset = 0;

		buf = map_sysmem(addr, count);
		ret = file_fat_write(dump_info_name, buf, offset, count, &size);
		if (ret < 0) {
			printf("Unable to write vmcore.dump\n");
			return -1;
		}

                unmap_sysmem(buf);


		addr =  axera_dump_info.axera_dump_addr;
		count = axera_dump_info.axera_dump_size;
		offset = 0;

		printf("usb addr = 0x%lx  count = 0x%lx dump_file_name = %s\n",addr,count,dump_file_name);
		buf = map_sysmem(addr, count);
   		ret = file_fat_write(dump_file_name, buf, offset, count, &size);
   		unmap_sysmem(buf);
   		if (ret < 0) {
			printf("file_fat_write failed\n");
           		return -1;
   		}

		ret = run_command("fatls usb 0", 0);
		if (ret) {
			printf("fatls usb 0 failed\n");
		}
		clear_boot_reason_mask(&axera_dump_info);
	}
	return 0;
}
#endif

extern int ext4fs_memory_dump(struct blk_desc *fs_dev_desc, unsigned long int addr, unsigned long int size,
		unsigned long int info_addr,unsigned long int info_size,char *filename,char *info_name);
#ifdef CONFIG_AXERA_MEMORY_DUMP_EMMC
static int dump_memory_to_emmc(u32 boot_reason)
{
	int ret;
	u32 reason_mask;
	struct axera_memory_dump_struct  axera_dump_info;
	struct blk_desc *emmc_desc;
	ret = last_kernel_dtb_check(&axera_dump_info);
	if(ret < 0)
		return -1;
	printf("emmc  memory dump...\n");
	reason_mask = get_boot_reason_mask(boot_reason,&axera_dump_info);
	if((boot_reason != 0) && (reason_mask == AXERA_REASON_MASK)) {

		/* step1 check emmc is present */
 		emmc_desc = blk_get_dev("mmc", EMMC_DEV_ID);
 		if (NULL == emmc_desc) {
         		printf("memory dump sd is not present, exit emmc dump\n");
         		return -1;
 		}
		display_kernel_time(&axera_dump_info,0);

		ext4fs_memory_dump(emmc_desc,axera_dump_info.axera_dump_addr,axera_dump_info.axera_dump_size,
			axera_dump_info.axera_dump_info_addr,axera_dump_info.axera_dump_info_size,dump_file_name,dump_info_name);

		clear_boot_reason_mask(&axera_dump_info);
		return 0;
	}
	return 0;
}
#endif

#ifdef CONFIG_AXERA_MEMORY_DUMP_SSD
static int dump_memory_to_ssd(u32 boot_reason)
{
	int ret;
	u32 reason_mask;
	struct axera_memory_dump_struct  axera_dump_info;
	struct blk_desc *ssd_desc;
	ret = last_kernel_dtb_check(&axera_dump_info);
	if(ret < 0)
		return -1;
	printf("ssd  memory dump...\n");
	reason_mask = get_boot_reason_mask(boot_reason,&axera_dump_info);
	if((boot_reason != 0) && (reason_mask == AXERA_REASON_MASK)) {

		/* step1 check ssd is present */
		ssd_desc = blk_get_dev("nvme", 0);

		if (NULL == ssd_desc) {
			printf("memory dump sd is not present, exit ssd dump\n");
			return -1;
		}

		display_kernel_time(&axera_dump_info,0);

		ext4fs_memory_dump(ssd_desc,axera_dump_info.axera_dump_addr,axera_dump_info.axera_dump_size,
			axera_dump_info.axera_dump_info_addr,axera_dump_info.axera_dump_info_size,dump_file_name,dump_info_name);

		clear_boot_reason_mask(&axera_dump_info);
	}

	return 0;
}
#endif

#ifdef CONFIG_AXERA_MEMORY_DUMP_SD
static int dump_memory_to_sd(u32 boot_reason)
{
	u32 reason_mask;
	struct udevice *dev;
	char *mmc_type;
	loff_t size;
	unsigned long addr;
	unsigned long count;
	long offset;

	void *buf;

	int ret;
	struct blk_desc *sd_desc = NULL;

	struct axera_memory_dump_struct  axera_dump_info;
	ret = last_kernel_dtb_check(&axera_dump_info);
	if(ret < 0)
		return -1;

	reason_mask = get_boot_reason_mask(boot_reason,&axera_dump_info);

	if((boot_reason != 0) && (reason_mask == AXERA_REASON_MASK)) {

		/* step1 check sd is present */
		sd_desc = blk_get_dev("mmc", SD_DEV_ID);
		if (NULL == sd_desc) {
        		printf("memory dump sd is not present, exit sd update\n");
        		return -1;
		}

		/* we register fat device */
		if (fat_register_device(sd_desc, 0)) {
        		printf("memory dump sd register part0 fat fail, try part1\n");

        		if (fat_register_device(sd_desc, 1)) {
                		printf("sd register part1 fat fail, exit sd update\n");
                		return -1;
        		}
		}

		display_kernel_time(&axera_dump_info,1);
                for (uclass_first_device(UCLASS_MMC, &dev);dev;uclass_next_device(&dev)) {
			struct mmc *m = mmc_get_mmc_dev(dev);
			if (m->has_init)
				mmc_type = IS_SD(m) ? "SD" : "eMMC";
			else
				mmc_type = NULL;

			if(!strcmp(mmc_type,"eMMC")) {
				continue;
			}

			if (mmc_type) {
				printf("%s: %d", m->cfg->name, mmc_get_blk_desc(m)->devnum);
				printf(" (%s)\n", mmc_type);
			} else {
				pr_err("SD card no found,memory dump failed\n");
				return -1;
			}
		}

		addr =  axera_dump_info.axera_dump_info_addr;
		count = axera_dump_info.axera_dump_info_size;
		offset = 0;

		buf = map_sysmem(addr, count);
		ret = file_fat_write(dump_info_name, buf, offset, count, &size);
		if (ret < 0) {
			printf("Unable to write %s, ret=%d\n", dump_info_name, ret);
			return -1;
		}

                unmap_sysmem(buf);

                addr =  axera_dump_info.axera_dump_addr;
                count = axera_dump_info.axera_dump_size;

		printf("sd card dump....\n");
		buf = map_sysmem(addr, count);
		ret = file_fat_write(dump_file_name, buf, offset, count, &size);
		if (ret < 0) {
			printf("Unable to write\n");
			return -1;
		}

		unmap_sysmem(buf);
		printf("%llu bytes written\n", size);
		clear_boot_reason_mask(&axera_dump_info);
		return 0;
	}
	return 0;
}
#endif

static boot_mode_t sysdump_mode(void)
{
	wdt0_enable(0);
#ifdef CONFIG_AXERA_MEMORY_DUMP_PCIE
	if (dump_reason & (1 << 3)) {
		//boot_mode->mode = SYSDUMP_MODE;
		if (dump_memory_to_pcie(dump_reason) < 0) {
			printf("axera memoey pcie failed\n");
		}
		goto dump_ok;
	}

#endif

	if (dump_reason & (1 << 3)) {
#ifdef CONFIG_AXERA_MEMORY_DUMP_SD
		//boot_mode->mode = SYSDUMP_MODE;
		if (dump_memory_to_sd(dump_reason) < 0) {
			printf("axera memoey dump sd failed\n");
		} else {
			goto dump_ok;
		}
#endif
#ifdef CONFIG_AXERA_MEMORY_DUMP_USB_STORAGE

		if (dump_memory_to_usb_storage(dump_reason) < 0) {
			printf("axera memoey dump usb_storage failed\n");
		} else {
			goto dump_ok;
		}
#endif
#ifdef CONFIG_AXERA_MEMORY_DUMP_EMMC
		if (dump_memory_to_emmc(dump_reason) < 0) {
			printf("axera memoey dump emmc failed\n");
		} else {
			goto dump_ok;
		}
#endif

#ifdef CONFIG_AXERA_MEMORY_DUMP_SSD
		if (dump_memory_to_ssd(dump_reason) < 0) {
			printf("axera memoey dump ssd failed\n");
		} else {
			goto dump_ok;
		}
#endif

	}

#if defined CONFIG_AXERA_MEMORY_DUMP_SD || CONFIG_AXERA_MEMORY_DUMP_EMMC || CONFIG_AXERA_MEMORY_DUMP_USB_STORAGE || CONFIG_AXERA_MEMORY_DUMP_PCIE || CONFIG_AXERA_MEMORY_DUMP_SSD
dump_ok:
#endif
	wdt0_enable(1);

	return CMD_UNDEFINED_MODE;
}

static boot_mode_t sd_update_mode(void)
{
	struct blk_desc *sd_desc = NULL;
	int sd_is_present = 0;

	sd_desc = blk_get_dev("mmc", SD_DEV_ID);
	if (NULL == sd_desc)
		printf("sd card is not present\n");
	else
		sd_is_present = 1;
#ifdef CONFIG_CMD_AXERA_SDUPDATE
	if (sd_is_present) {
#if defined (CONFIG_ENV_IS_IN_MMC) || defined (CONFIG_ENV_IS_IN_NAND)
		char *update_status = env_get("sdupdate");

		if (!strcmp(update_status, "fail")) {
			printf("last time sd update fail, may need check\n");
			return CMD_UNDEFINED_MODE;
		} else if (strcmp(update_status, "finish")) {
			if (boot_info_data.mode == NORMAL_BOOT_MODE || boot_info_data.mode == SD_BOOT_MODE) {
				env_set("bootcmd", "sd_update");
				printf("env sdupdate is %s, enter sd update mode\n", update_status);
				boot_info_data.mode = SD_UPDATE_MODE;
				return SD_UPDATE_MODE;
			}
		}
#endif
	} else {
		return CMD_UNDEFINED_MODE;
	}
#endif
	return CMD_UNDEFINED_MODE;
}

static boot_mode_t usb_update_mode(void)
{
	if (boot_info_data.mode == USB_UPDATE_MODE) {
		env_set("bootdelay", "0");
		env_set("bootcmd", "download");
		printf("enter usb download mode\n");
		wdt0_enable(0);
		return USB_UPDATE_MODE;
	}
	return CMD_UNDEFINED_MODE;
}

static boot_mode_t uart_update_mode(void)
{
	if (boot_info_data.mode == UART_UPDATE_MODE) {
		env_set("bootdelay", "0");
		env_set("bootcmd", "download");
		printf("enter uart download mode\n");
		wdt0_enable(0);
		return UART_UPDATE_MODE;
	}
	return CMD_UNDEFINED_MODE;
}

static boot_mode_t tftp_update_mode(void)
{
#if defined (CONFIG_CMD_AXERA_TFTP_OTA) && (defined (CONFIG_ENV_IS_IN_MMC) || defined (CONFIG_ENV_IS_IN_NAND))
	if ((boot_info_data.mode == NORMAL_BOOT_MODE) && (NULL != env_get("ota_ready"))) {
		printf("env ota_ready=%s\n", env_get("ota_ready"));
		if (!strcmp(env_get("ota_ready"), "true")) {
			env_set("bootcmd", "axera_ota");
			printf("enter tftp ota update\n");
			wdt0_enable(0);
			boot_info_data.mode = TFTP_UPDATE_MODE;
			return TFTP_UPDATE_MODE;
		} else if (!strcmp(env_get("ota_ready"), "retry")) {
			env_set("bootcmd", "axera_ota");
			printf("retry tftp ota update\n");
			wdt0_enable(0);
			boot_info_data.mode = TFTP_UPDATE_MODE;
			return TFTP_UPDATE_MODE;
		}
	}
#endif
	return CMD_UNDEFINED_MODE;
}

static boot_mode_t sdboot_mode(void)
{
	if (boot_info_data.mode == SD_BOOT_MODE) {
		env_set("bootcmd", "sd_boot");
		printf("enter sd boot mode\n");
		return SD_BOOT_MODE;
	}
	return CMD_UNDEFINED_MODE;
}

static boot_mode_t usb_stor_mode(void)
{
#if defined (CONFIG_CMD_AXERA_USB_STOR_UPDATE) && (defined (CONFIG_ENV_IS_IN_MMC) || defined (CONFIG_ENV_IS_IN_NAND))
	if ((boot_info_data.mode == NORMAL_BOOT_MODE) && (NULL != env_get("usbupdate"))) {
		char * update_status = env_get("usbupdate");
		struct blk_desc *usb_stor_desc = NULL;
		int ret;

		if (!strcmp(update_status, "retry")) {
			ret = run_command("usb start", 0);
			if (ret) {
				printf("usb start error %d\n", ret);
				return CMD_UNDEFINED_MODE;
			}

			usb_stor_desc = blk_get_dev("usb", 0);
			if (NULL == usb_stor_desc) {
				printf("usb-storage is not present\n");
				return CMD_UNDEFINED_MODE;
			}

			env_set("bootcmd", "usb_storage_update");
			wdt0_enable(0);
			printf("env usbupdate is %s, enter usb update mode\n", update_status);
			boot_info_data.mode = USB_STOR_MODE;
			return USB_STOR_MODE;
		}
	}
#endif
	return CMD_UNDEFINED_MODE;
}

static s_boot_func_array boot_func_array[BOOTMODE_FUN_NUM] = {
	sysdump_mode,
	sd_update_mode,
	usb_update_mode,
	uart_update_mode,
	tftp_update_mode,
	usb_stor_mode,
	sdboot_mode,
	0,
};

static void update_cmdline(void)
{
	char *boot_reason_cmd = NULL;
	char *board_id_cmd = NULL;
	char * bootargs;
	int board_id = get_board_id();
	u32 boot_reason = dump_reason;

	bootargs = env_get("bootargs");
	if(NULL == bootargs)
		return;
	boot_reason_cmd = strstr(bootargs, "boot_reason");
	if(!boot_reason_cmd) {
		pr_err("boot_reason not find bootargs\n");
		return;
	}
	if (boot_reason >= 0 && boot_reason <= 9) {
		boot_reason_cmd[14] = boot_reason + '0';
	}

	board_id_cmd = strstr(bootargs, "board_id");
	if(!board_id_cmd) {
		pr_err("board_id not find bootargs\n");
		return;
	}
	if (board_id >= 0 && board_id <= 9) {
		board_id_cmd[11] = board_id + '0';
	}
	if (board_id >= 10 && board_id <= 15) {
		board_id_cmd[11] = board_id + 'W';
	}
	env_set("bootargs", bootargs);
}

int setup_boot_mode(void)
{
	int i = 0;
	boot_mode_t boot_mode;
	struct boot_mode_info *const boot_info = get_dl_and_boot_info();

	if (boot_info->magic != BOOT_MODE_ENV_MAGIC) {
		printf("boot_mode magic error\n");
		return -1;
	}
	memcpy(&boot_info_data, boot_info, sizeof(boot_mode_info_t));

	for (i = 0; i < BOOTMODE_FUN_NUM - 1; i++) {
		if (0 == boot_func_array[i]) {
			env_set("bootcmd", "axera_boot");
			printf("enter normal boot mode\n");
			break;
		}
		boot_mode = boot_func_array[i] ();
		if (CMD_UNDEFINED_MODE == boot_mode) {
			continue;
		} else {
			printf("get boot mode in boot func array[%d]\n", i);
			break;
		}
	}
	update_cmdline();
	printf("boot_info_data.mode = %d\n", boot_info_data.mode);
	return 0;
}
