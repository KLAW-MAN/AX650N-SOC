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
#include <cpu_func.h>
#include "../../legacy-mtd-utils.h"
#include "axera_update.h"
#include "../boot/axera_boot.h"

extern struct boot_mode_info boot_info_data;
extern int atf_boot_prepare(char *kimg_addr, char *dtb_addr);
extern bl_params_t atf_bl_params;
extern bootm_headers_t atf_boot_images;

#define READ_IMG_SIZE (5 * 1024 *1024)
#define SD_BOOT_IMAGE_ADDR CONFIG_SYS_SDRAM_BASE

int do_sd_boot(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	char boot_cmd[30];
	int reserve_img_size, read_img_size;
	int i, j;
	void (*atf_boot)(u64 arg0, u64 arg1, u64 arg2, u64 arg3);
	struct secboot_image_header *atf_img = NULL;
	struct secboot_image_header *boot_img = NULL;
	unsigned int img_header_size = sizeof(struct secboot_image_header);
#ifdef OPTEE_BOOT
	struct secboot_image_header *optee_img = NULL;
#endif
	printf("now enter sd boot, SD_BOOT_IMAGE_ADDR:0x%lx\n",SD_BOOT_IMAGE_ADDR);
	if (boot_info_data.mode == SD_BOOT_MODE) {
		env_set("bootargs", BOOTARGS_SD);
		sprintf(boot_cmd, "fatload mmc 1:1 0x%lx kernel.img 0x%x",SD_BOOT_IMAGE_ADDR, img_header_size);
		run_command_list(boot_cmd, -1, 0);
		boot_img = (struct secboot_image_header *)SD_BOOT_IMAGE_ADDR;
		i = (boot_img->image_size - img_header_size) / READ_IMG_SIZE;
		read_img_size = READ_IMG_SIZE; //5M
		reserve_img_size = boot_img->image_size - img_header_size;
		j = 0;
		memset((void *)KERNEL_IMAGE_ADDR, 0, boot_img->image_size - img_header_size);
		memset((void *)DTB_IMAGE_ADDR, 0, 1024 * 1024);

		while (i--) {
			memset(boot_cmd, 0, sizeof(boot_cmd));
			memset((void *)SD_BOOT_IMAGE_ADDR, 0, read_img_size);

			sprintf(boot_cmd, "fatload mmc 1:1 0x%lx kernel.img 0x%lx 0x%x", SD_BOOT_IMAGE_ADDR, read_img_size, img_header_size + read_img_size * j);
			run_command_list(boot_cmd, -1, 0);
			reserve_img_size = reserve_img_size - read_img_size;
			memmove((void *)KERNEL_IMAGE_ADDR + read_img_size * j, (void *)SD_BOOT_IMAGE_ADDR, read_img_size);
			j++;
			if(reserve_img_size / READ_IMG_SIZE == 0) {
				read_img_size = reserve_img_size;
				memset( (void *)SD_BOOT_IMAGE_ADDR, 0, read_img_size);
				sprintf(boot_cmd, "fatload mmc 1:1 0x%lx kernel.img 0x%x 0x%x", SD_BOOT_IMAGE_ADDR, read_img_size, img_header_size + READ_IMG_SIZE * j);
				run_command_list(boot_cmd, -1, 0);
				memmove((void *)KERNEL_IMAGE_ADDR + READ_IMG_SIZE * j, (void *)SD_BOOT_IMAGE_ADDR, read_img_size);
			}
		}
		printf("sd boot:kernel img read finish\n");

		memset(boot_cmd, 0, sizeof(boot_cmd));
		memset( (void *)SD_BOOT_IMAGE_ADDR, 0, READ_IMG_SIZE);
		sprintf(boot_cmd, "fatload mmc 1:1 0x%lx dtb.img ", SD_BOOT_IMAGE_ADDR);
		run_command_list(boot_cmd, -1, 0);
		memmove((void *)DTB_IMAGE_ADDR, (void *)SD_BOOT_IMAGE_ADDR + img_header_size, 512 * 1024);
		printf("sd boot:dtb img read finish\n");

		memset(boot_cmd, 0, sizeof(boot_cmd));
		memset( (void *)SD_BOOT_IMAGE_ADDR, 0, READ_IMG_SIZE);
		sprintf(boot_cmd, "fatload mmc 1:1 0x%lx atf.img", SD_BOOT_IMAGE_ADDR);
		run_command_list(boot_cmd, -1, 0);
		atf_img = (struct secboot_image_header *)SD_BOOT_IMAGE_ADDR;
		printf("sd boot: atf_img->img_size: 0x%x\n", atf_img->image_size);

		memset((void *)ATF_IMAGE_ADDR, 0, atf_img->image_size - img_header_size);
		memmove((void *)ATF_IMAGE_ADDR, (void *)SD_BOOT_IMAGE_ADDR + img_header_size, atf_img->image_size - img_header_size);
		printf("sd boot:atf img read finish\n");

#ifdef OPTEE_BOOT
		memset(boot_cmd, 0, sizeof(boot_cmd));
		memset( (void *)SD_BOOT_IMAGE_ADDR, 0, READ_IMG_SIZE);
		sprintf(boot_cmd, "fatload mmc 1:1 0x%lx optee.img", SD_BOOT_IMAGE_ADDR);
		run_command_list(boot_cmd, -1, 0);
		optee_img = (struct secboot_image_header *)SD_BOOT_IMAGE_ADDR;
		printf("sd boot: optee_img->img_size: 0x%x\n",optee_img->image_size);

		memset((void *)OPTEE_IMAGE_ADDR, 0, optee_img->image_size - img_header_size);
		memmove((void *)OPTEE_IMAGE_ADDR, (void *)SD_BOOT_IMAGE_ADDR + img_header_size, optee_img->image_size - img_header_size);
		printf("sd boot:optee img read finish\n");
#endif

		atf_boot = (void *)ATF_IMAGE_ADDR;
		if (!atf_boot_prepare((char *)(KERNEL_IMAGE_ADDR), (char *)DTB_IMAGE_ADDR)) {
			printf("now enter atf boot\n");
			dcache_disable();
			atf_boot((u64)&atf_bl_params, 0, (u64)atf_boot_images.ft_addr,
						ARM_BL31_PLAT_PARAM_VAL);
		}
	}

	return 0;
}

U_BOOT_CMD(sd_boot, 1, 0, do_sd_boot,
	   "sd boot", "axera enter sd boot mode\n" "it is used for sd boot to kernel\n");
