/*
 * AXERA AX650X Host Controller Interface
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <blk.h>
#include <cpu_func.h>
#include <asm/arch/boot_mode.h>
#include <malloc.h>
#include <mtd.h>
#include <image.h>
#include <dm/uclass.h>
#include <dm/device.h>
#include <jffs2/load_kernel.h>
#include <linux/mtd/spi-nor.h>
#include "../../legacy-mtd-utils.h"
#include <common.h>
#include <asm/io.h>
#include <asm/arch/ax650.h>
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
#include <pcie_ax650.h>
#include "../secureboot/secureboot.h"
#include "axera_boot.h"
#include "../cipher/ax_cipher_api.h"

DECLARE_GLOBAL_DATA_PTR;

bl_params_t atf_bl_params;
bl_params_node_t atf_bl33_params;
entry_point_info_t atf_bl33_ep_info;
#ifdef OPTEE_BOOT
bl_params_node_t atf_bl32_params;
entry_point_info_t atf_bl32_ep_info;
#endif

extern int pinmux_init(void);
extern int get_part_info(struct blk_desc *dev_desc, const char *name, disk_partition_t *info);
extern struct boot_mode_info boot_info_data;
typedef void (*atf_boot_fn)(u64 arg0, u64 arg1, u64 arg2, u64 arg3);

/* key_n_header + rsa_key_n + key_e_header + rsa_key_e + 1 */
#define PUB_KEY_ARRAY_MAX_SZ  (397)
int get_boot_image_emmc(char *part_name, void *dest)
{
	int ret = 0;
	disk_partition_t part_info;
	struct blk_desc *blk_dev_desc = NULL;
	uint64_t rd_blkcnt_lb;
	uint8_t hash_digest[32];
	int img_signed, is_sec_enable, key_bits;
	int is_encrypted = 0;
	void *image_load_addr = (void *)HEADER_IMAGE_ADDR;
	struct secboot_image_header *image_header =
						(struct secboot_image_header *)image_load_addr;
#ifdef SECUREBOOT_ENABLE
	int secboot_verify = 1;
#else
	int secboot_verify = env_get_ulong("secureboot_test", 10, 0);
	if (secboot_verify)
		printf("sw secureboot is enabled\n");
#endif

	blk_dev_desc = blk_get_dev("mmc", EMMC_DEV_ID);
	if (!blk_dev_desc) {
		printf("get mmc dev fail\n");
		return -1;
	}

	ret = get_part_info(blk_dev_desc, part_name, &part_info);
	if (ret == -1) {
		printf("%s: get %s partition info fail\n", __FUNCTION__, part_name);
		return -1;
	}

	printf("reading %s image header...\n", part_name);
	rd_blkcnt_lb = blk_dread(blk_dev_desc, part_info.start,
				(sizeof(struct secboot_image_header) + 511) / 512, image_load_addr);
	if (rd_blkcnt_lb != (sizeof(struct secboot_image_header) + 511) / 512) {
		printf("read %s image header failed\n", part_name);
		return -1;
	}

	printf("verify %s image_header ...\n", part_name);
	ret = verify_image_header(image_load_addr, sizeof(struct secboot_image_header));
	if (ret < 0) {
		printf("%s image header verify failed\n", part_name);
		return -1;
	}
	printf("verify %s image_header success\n", part_name);

	printf("reading %s image ...\n", part_name);
	rd_blkcnt_lb = blk_dread(blk_dev_desc, part_info.start,
						(image_header->image_size + 511) / 512, image_load_addr);
	if (rd_blkcnt_lb != ((image_header->image_size + 511) / 512)) {
		printf("%s get %s image fail\n", __func__, part_name);
		return -1;
	}

	is_sec_enable = is_secure_enable();
	if (is_sec_enable)
		secboot_verify = 1;

	if (secboot_verify != 0) {
		img_signed = (image_header->capability & IMG_SIGN) ? 1 : 0;
		is_encrypted = (image_header->capability & IMG_CIPHER_ENABLE) ? 1 : 0;
		key_bits = (image_header->capability & RSA_3072_MODE) ? 3072 : 2048;

		printf("efuse secureboot bit is enable:%d, key_bits:%d\n", is_sec_enable, key_bits);

		if (!img_signed) {
			printf("invalid %s image\n", part_name);
			return -1;
		} else {
			char public_key[PUB_KEY_ARRAY_MAX_SZ] = {0};

			/* copy key_key_n_header and key(2048 or 3072) in bytes */
			memcpy((void *)public_key, (void *)&image_header->pub_key.key_n_header, (4 + key_bits / 8));
			/* copy key_e_header & rsa_key_e*/
			memcpy((void *)public_key + 4 + key_bits / 8, (void *)&image_header->pub_key.key_e_header, 8);
			flush_cache((unsigned long)public_key, sizeof(struct rsa_key));
			if (public_key_verify(public_key, sizeof(struct rsa_key), key_bits) < 0) {
				printf("public key verify failed\n");
				return -1;
			}

			printf("total %s image size = %d\n", part_name, image_header->image_size);
			ret = cipher_sha256_big_packet((char *)image_load_addr + sizeof(struct secboot_image_header), (char *)hash_digest,
										image_header->image_size - sizeof(struct secboot_image_header));
			if (ret < 0) {
				printf("compute %s image hash ERROR: %d!\n", part_name, ret);
				return -1;
			}
			ret = cipher_rsa_verify(public_key, (char *)&image_header->signature.sig_header, (int *)hash_digest, key_bits);
			if(ret < 0) {
				printf("%s image verify failed\n", part_name);
				return -1;
			}
			printf(">>> * %s image verify success! * <<< \n", part_name);
		}
	}
	if (is_encrypted) {
		ret = cipher_aes_ecb_decrypto((unsigned long)((char *)image_load_addr + sizeof(struct secboot_image_header)),
										(unsigned long)dest, image_header->image_size - sizeof(struct secboot_image_header));
		if (ret < 0) {
			printf("decrypto %s image failed\n", part_name);
			return -1;
		}
	} else {
		/* memcpy image to dest addr */
		printf("copy %s image data to dest addr:0x%lx\n", part_name, (unsigned long)dest);
		memcpy(dest, (char *)image_load_addr + sizeof(struct secboot_image_header),
				image_header->image_size - sizeof(struct secboot_image_header));
	}

	return 0;
}

int get_boot_image_ssd(char *part_name, void *dest)
{
	int ret = 0;
	disk_partition_t part_info;
	struct blk_desc *blk_dev_desc = NULL;
	uint64_t rd_blkcnt_lb;
	uint8_t hash_digest[32];
	int img_signed, is_sec_enable, key_bits;
	int is_encrypted = 0;
	void *image_load_addr = (void *)HEADER_IMAGE_ADDR;
	struct secboot_image_header *image_header =
						(struct secboot_image_header *)image_load_addr;
#ifdef SECUREBOOT_ENABLE
	int secboot_verify = 1;
#else
	int secboot_verify = env_get_ulong("secureboot_test", 10, 0);
	if (secboot_verify)
		printf("sw secureboot is enabled\n");
#endif

	blk_dev_desc = blk_get_dev("nvme", 0);
	if (!blk_dev_desc) {
		printf("get nvme dev fail\n");
		return -1;
	}

	ret = get_part_info(blk_dev_desc, part_name, &part_info);
	if (ret == -1) {
		printf("%s: get %s partition info fail\n", __FUNCTION__, part_name);
		return -1;
	}

	printf("reading %s image header...\n", part_name);
	rd_blkcnt_lb = blk_dread(blk_dev_desc, part_info.start,
				(sizeof(struct secboot_image_header) + 511) / 512, image_load_addr);
	if (rd_blkcnt_lb != (sizeof(struct secboot_image_header) + 511) / 512) {
		printf("read %s image header failed\n", part_name);
		return -1;
	}

	printf("verify %s image_header ...\n", part_name);
	ret = verify_image_header(image_load_addr, sizeof(struct secboot_image_header));
	if (ret < 0) {
		printf("%s image header verify failed\n", part_name);
		return -1;
	}
	printf("verify %s image_header success\n", part_name);

	printf("reading %s image ...\n", part_name);
	rd_blkcnt_lb = blk_dread(blk_dev_desc, part_info.start,
						(image_header->image_size + 511) / 512, image_load_addr);
	if (rd_blkcnt_lb != ((image_header->image_size + 511) / 512)) {
		printf("%s get %s image fail\n", __func__, part_name);
		return -1;
	}

	is_sec_enable = is_secure_enable();
	if (is_sec_enable)
		secboot_verify = 1;

	if (secboot_verify != 0) {
		img_signed = (image_header->capability & IMG_SIGN) ? 1 : 0;
		is_encrypted = (image_header->capability & IMG_CIPHER_ENABLE) ? 1 : 0;
		key_bits = (image_header->capability & RSA_3072_MODE) ? 3072 : 2048;

		printf("efuse secureboot bit is enable:%d, key_bits:%d\n", is_sec_enable, key_bits);

		if (!img_signed) {
			printf("invalid %s image\n", part_name);
			return -1;
		} else {
			char public_key[PUB_KEY_ARRAY_MAX_SZ] = {0};

			/* copy key_key_n_header and key(2048 or 3072) in bytes */
			memcpy((void *)public_key, (void *)&image_header->pub_key.key_n_header, (4 + key_bits / 8));
			/* copy key_e_header & rsa_key_e*/
			memcpy((void *)public_key + 4 + key_bits / 8, (void *)&image_header->pub_key.key_e_header, 8);
			flush_cache((unsigned long)public_key, sizeof(struct rsa_key));
			if (public_key_verify(public_key, sizeof(struct rsa_key), key_bits) < 0) {
				printf("public key verify failed\n");
				return -1;
			}

			printf("total %s image size = %d\n", part_name, image_header->image_size);
			ret = cipher_sha256_big_packet((char *)image_load_addr + sizeof(struct secboot_image_header), (char *)hash_digest,
										image_header->image_size - sizeof(struct secboot_image_header));
			if (ret < 0) {
				printf("compute %s image hash ERROR: %d!\n", part_name, ret);
				return -1;
			}
			ret = cipher_rsa_verify(public_key, (char *)&image_header->signature.sig_header, (int *)hash_digest, key_bits);
			if(ret < 0) {
				printf("%s image verify failed\n", part_name);
				return -1;
			}
			printf(">>> * %s image verify success! * <<< \n", part_name);
		}
	}
	if (is_encrypted) {
		ret = cipher_aes_ecb_decrypto((unsigned long)((char *)image_load_addr + sizeof(struct secboot_image_header)),
										(unsigned long)dest, image_header->image_size - sizeof(struct secboot_image_header));
		if (ret < 0) {
			printf("decrypto %s image failed\n", part_name);
			return -1;
		}
	} else {
		/* memcpy image to dest addr */
		printf("copy %s image data to dest addr:0x%lx\n", part_name, (unsigned long)dest);
		memcpy(dest, (char *)image_load_addr + sizeof(struct secboot_image_header),
				image_header->image_size - sizeof(struct secboot_image_header));
	}

	return 0;
}

#ifdef CONFIG_BOOT_OPTIMIZATION
#define BOOTIMG_HEADER_SIZE     64
#define BOOT_MAGIC_SIZE         6
//#define HEADER_IMAGE_ADDR       0x68008000

static void dump_img_header(struct axera_image_header *hdr)
{
	printf("MAGIC:       %s\n", hdr->magic);
	printf("img size:    %d\n", hdr->img_size);
	printf("raw_img_sz: %d\n", hdr->raw_img_sz);
	printf("raw_img_offset: %d\n", hdr->raw_img_offset);
	printf("id:");
	for (int i = 0; i < 32; ++i) {
		printf("%02x ", hdr->id[i]);
	}
	printf("\n");
}

static u32 verifed_image(unsigned char *hash_id, char *addr, unsigned int size)
{
	//todo: Use eip130 driver
	return 0;
}

int get_axera_boot_image_spinand(char **image, char **dtb)
{
	u32 ret, load_buf_off;
	u32 busnum = 0;
	loff_t off = 0;
	loff_t read_addr;
	int idx;
	loff_t size, maxsize;
	loff_t read_size, remaining, image_size, dtb_size;
	size_t retlen;
	struct udevice *dev;
	struct mtd_info *mtd;
	char *header_load_addr = (char *)HEADER_IMAGE_ADDR;
	char *kernel_load_addr = (char *)KERNEL_IMAGE_ADDR;
	char *dtb_load_addr = (char *)DTB_IMAGE_ADDR;
	struct axera_image_header *hdr;
	const char *x_kernel = "kernel";
	const char *x_dtb = "dtb";

#ifdef CONFIG_AXERA_SUPPORT_AB_OTA
	char *bootpart = env_get("bootpart");

	if (bootpart == NULL) {
		x_kernel = "kernel";
		x_dtb = "dtb";
		env_set("bootargs", BOOTARGS_SPINAND_A);
	} else {
		if (strcmp(bootpart, "B") == 0) {
			x_kernel = "kernel_B";
			x_dtb = "dtb_B";
			env_set("bootargs", BOOTARGS_SPINAND_B);
		} else {
			x_kernel = "kernel";
			x_dtb = "dtb";
			env_set("bootargs", BOOTARGS_SPINAND_A);
		}
	}
#endif

	ret = uclass_get_device(UCLASS_MTD, busnum, &dev);
	if (ret) {
		printf("uclass_get_device: Invalid bus %d (err=%d)\n", busnum, ret);
		return ret;
	}
	mtd = dev_get_uclass_priv(dev);
	printf("reading DTB and BOOT image ...\n");

	ret = mtd_arg_off(x_dtb, &idx, &off, &size, &maxsize, MTD_DEV_TYPE_NAND, mtd->size);
	if (ret) {
		printf("%s: get dtb partition info fail, ret=%d\n", __func__, ret);
		return -1;
	}

	if ((u64)off % mtd->erasesize) {
		printf("nand addr 0x%llX is not block size 0x%X aligned!\n", off, mtd->erasesize);
		return -1;
	}
	dtb_size = 0;
	load_buf_off = 0;
	read_addr = off;
	remaining = size;
	//printf("%s: read_addr=0x%llX, remaining=0x%X\n", __func__, read_addr, remaining);
	while (remaining) {
		if (mtd_block_isbad(mtd, read_addr)) {
			printf("nand addr 0x%llX, blk %lld is bad\n", read_addr, (u64) read_addr / mtd->erasesize);
			read_addr += mtd->erasesize;
			continue;
		}

		read_size = (remaining > mtd->erasesize) ? mtd->erasesize : remaining;
		if (read_addr + read_size > off + size) {
			printf("%s: read addr 0x%llX len 0x%llX over kernel-dtb part addr 0x%llX size 0x%llX\n",
			       __func__, read_addr, read_size, off, size);
			return -1;
		}

		mtd_read(mtd, read_addr, read_size, &retlen, (u_char *) (dtb_load_addr + load_buf_off));
		if (retlen != read_size) {
			printf("dtb image read fail!\n");
			return -1;
		}
		//printf("partiton kernel-dtb read 0x%X bytes, nand addr 0x%llX ==> buf addr 0x%X\n",
		//retlen, read_addr, (u32)(kernel_load_addr + load_buf_off));

		if (0 == dtb_size) {
			if (fdt_check_header(dtb_load_addr) != 0) {
				printf("image is not a fdt: %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X\n",
				     dtb_load_addr[0x0], dtb_load_addr[0x1], dtb_load_addr[0x2], dtb_load_addr[0x3],
				     dtb_load_addr[0x4], dtb_load_addr[0x5], dtb_load_addr[0x6], dtb_load_addr[0x7],
				     dtb_load_addr[0x8], dtb_load_addr[0x9], dtb_load_addr[0xA], dtb_load_addr[0xB],
				     dtb_load_addr[0xC], dtb_load_addr[0xD], dtb_load_addr[0xE], dtb_load_addr[0xF]);
				return -1;
			}
			dtb_size = fdt_totalsize(dtb_load_addr);
			if (dtb_size < read_size) {
				remaining = 0;
				printf("kernel dtb size 0x%llX\n", dtb_size);
				break;
			}
			remaining = dtb_size;
			printf("warning: kernel dtb size 0x%X, update valid size 0x%llX\n", fdt_totalsize(dtb_load_addr), remaining);
		}
		remaining -= read_size;
		load_buf_off += read_size;
		read_addr += read_size;
	}

	printf("reading bootimg header...\n");
	ret = mtd_arg_off(x_kernel, &idx, &off, &size, &maxsize, MTD_DEV_TYPE_NAND, mtd->size);
	if (ret) {
		printf("%s: get kernel partition info fail, ret=%d\n", __func__, ret);
		return -1;
	}

	if ((u64)off % mtd->erasesize) {
		printf("nand addr 0x%llX is not block size 0x%X aligned!\n", off, mtd->erasesize);
		return -1;
	}
	image_size = 0;
	load_buf_off = 0;
	read_addr = off;
	remaining = size;
	//printf("%s: read_addr=0x%llX, remaining=0x%X\n", __func__, read_addr, remaining);
	while (remaining) {
		if (mtd_block_isbad(mtd, read_addr)) {
			printf("nand addr 0x%llX, blk %lld is bad\n", read_addr, (u64) read_addr / mtd->erasesize);
			read_addr += mtd->erasesize;
			continue;
		}

		read_size = (remaining > mtd->erasesize) ? mtd->erasesize : remaining;
		if (read_addr + read_size > off + size) {
			printf("%s: read addr 0x%llX len 0x%llX over kernel part addr 0x%llX size 0x%llX\n",
			       __func__, read_addr, read_size, off, size);
			return -1;
		}

		mtd_read(mtd, read_addr, read_size, &retlen, (u_char *) (header_load_addr + load_buf_off));
		if (retlen != read_size) {
			printf("kernel image read fail!\n");
			return -1;
		}
		//printf("partiton kernel read 0x%X bytes, nand addr 0x%llX ==> buf addr 0x%X\n",
		//retlen, read_addr, (u32)(kernel_load_addr + load_buf_off));

		if (0 == image_size) {
			hdr = (struct axera_image_header *)header_load_addr;
			if (strncmp((char *)hdr->magic, HEADER_MAGIC, BOOT_MAGIC_SIZE) != 0) {
				printf("%s: boot image header magic is error!\n", __func__);
				printf("DATA: %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X\n",
				     kernel_load_addr[0x0], kernel_load_addr[0x1], kernel_load_addr[0x2],
				     kernel_load_addr[0x3], kernel_load_addr[0x4], kernel_load_addr[0x5],
				     kernel_load_addr[0x6], kernel_load_addr[0x7], kernel_load_addr[0x8],
				     kernel_load_addr[0x9], kernel_load_addr[0xA], kernel_load_addr[0xB],
				     kernel_load_addr[0xC], kernel_load_addr[0xD], kernel_load_addr[0xE],
				     kernel_load_addr[0xF]);
				return -1;
			}
			dump_img_header(hdr);
			image_size = hdr->img_size;
			if (image_size < read_size) {
				printf("Wrong Image Size: total size 0x%llX is smaller than first read size 0x%llX\n",
				     image_size, read_size);
				return -1;
			}
			if (verifed_image(hdr->id, header_load_addr + hdr->raw_img_offset, hdr->raw_img_sz) != 0) {
				printf("%s: verifed image failed!!\n", __func__);
				return -1;
			}
			remaining = image_size;
			printf("raw_img_offset 0x%X, raw_img_sz 0x%X, update valid image size 0x%llX\n",
			       hdr->raw_img_offset, hdr->raw_img_sz, remaining);
		}
		remaining -= read_size;
		load_buf_off += read_size;
		read_addr += read_size;
	}
	memcpy(kernel_load_addr, header_load_addr + hdr->raw_img_offset, hdr->raw_img_sz);
	printf("load kernel image addr = 0x%lx,load dtb image addr = 0x%lx\n", (ulong) kernel_load_addr,
	       (ulong) dtb_load_addr);
	*image = kernel_load_addr;
	*dtb = dtb_load_addr;
	return 0;
}
#endif

int get_kernel_dtb_image_emmc(char **image, char **dtb)
{
	u64 rd_blkcnt_lb_kernel;
	u64 rd_blkcnt_lb_dtb;
	struct blk_desc *blk_dev_desc = NULL;
	disk_partition_t part_info;
	u32 ret = 0;
	char *kernel_load_addr = (char *)KERNEL_IMAGE_ADDR;
	char *dtb_load_addr = (char *)DTB_IMAGE_ADDR;
	const char *x_kernel = "kernel";
	const char *x_dtb = "dtb";

#ifdef CONFIG_AXERA_SUPPORT_AB_OTA
	char *bootpart = env_get("bootpart");

	if (bootpart == NULL) {
		x_kernel = "kernel";
		x_dtb = "dtb";
		env_set("bootargs", BOOTARGS_EMMC_A);
	} else {
		if (strcmp(bootpart, "B") == 0) {
			x_kernel = "kernel_B";
			x_dtb = "dtb_B";
			env_set("bootargs", BOOTARGS_EMMC_B);
		} else {
			x_kernel = "kernel";
			x_dtb = "dtb";
			env_set("bootargs", BOOTARGS_EMMC_A);
		}
	}
#endif

	blk_dev_desc = blk_get_dev("mmc", EMMC_DEV_ID);
	if (!blk_dev_desc) {
		printf("get mmc dev fail\n");
		return -1;
	}
	printf("reading DTB and KERNEL image from emmc ...\n");
	/* spl 512K,uboot 1M,env 512k,dtb 1M,kernel 32M */

	ret = get_part_info(blk_dev_desc, x_dtb, &part_info);
	if (ret == -1) {
		printf("%s: get dtb partition info fail\n", __FUNCTION__);
	}

	rd_blkcnt_lb_dtb = blk_dread(blk_dev_desc, part_info.start, part_info.size, dtb_load_addr);
	if (rd_blkcnt_lb_dtb != part_info.size) {
		printf("do_axera_boot get dtb image fail\n");
		return -1;
	}
	ret = get_part_info(blk_dev_desc, x_kernel, &part_info);
	if (ret == -1) {
		printf("%s: get kernel partition info fail\n", __FUNCTION__);
	}

	rd_blkcnt_lb_kernel = blk_dread(blk_dev_desc, part_info.start, part_info.size, kernel_load_addr);
	if (rd_blkcnt_lb_kernel != part_info.size) {
		printf("do_axera_boot get kernel image fail\n");
		return -1;
	}

	printf("load kernel image addr = 0x%llx,load dtb image addr = 0x%llx\n", (u64) kernel_load_addr,
	       (u64) dtb_load_addr);

	*image = kernel_load_addr;
	*dtb = dtb_load_addr;
	return 0;

}

int get_kernel_dtb_image_ssd(char **image, char **dtb)
{
	u64 rd_blkcnt_lb_kernel;
	u64 rd_blkcnt_lb_dtb;
	struct blk_desc *blk_dev_desc = NULL;
	disk_partition_t part_info;
	u32 ret = 0;
	char *kernel_load_addr = (char *)KERNEL_IMAGE_ADDR;
	char *dtb_load_addr = (char *)DTB_IMAGE_ADDR;

	blk_dev_desc = blk_get_dev("nvme", 0);
	if (!blk_dev_desc) {
		printf("get nvme dev fail\n");
		return -1;
	}
	printf("reading DTB and KERNEL image from emmc ...\n");
	/* spl 512K,uboot 1M,env 512k,dtb 1M,kernel 32M */

	ret = get_part_info(blk_dev_desc, "dtb", &part_info);
	if (ret == -1) {
		printf("%s: get dtb partition info fail\n", __FUNCTION__);
	}

	rd_blkcnt_lb_dtb = blk_dread(blk_dev_desc, part_info.start, part_info.size, dtb_load_addr);
	if (rd_blkcnt_lb_dtb != part_info.size) {
		printf("do_axera_boot get dtb image fail\n");
		return -1;
	}
	ret = get_part_info(blk_dev_desc, "kernel", &part_info);
	if (ret == -1) {
		printf("%s: get kernel partition info fail\n", __FUNCTION__);
	}

	rd_blkcnt_lb_kernel = blk_dread(blk_dev_desc, part_info.start, part_info.size, kernel_load_addr);
	if (rd_blkcnt_lb_kernel != part_info.size) {
		printf("do_axera_boot get kernel image fail\n");
		return -1;
	}

	printf("load kernel image addr = 0x%llx,load dtb image addr = 0x%llx\n", (u64) kernel_load_addr,
	       (u64) dtb_load_addr);

	*image = kernel_load_addr;
	*dtb = dtb_load_addr;
	return 0;

}

int get_kernel_image_nor(char **image)
{
	u32 ret;
	u32 busnum = 0;
	loff_t off = 0;
	int idx;
	loff_t size, maxsize;
	size_t retlen;
	struct udevice *dev;
	struct mtd_info *mtd;
	struct spi_flash *flash;
	char *pdata = NULL;

	ret = uclass_get_device(UCLASS_SPI_FLASH, busnum, &dev);
	if (ret) {
		printf("uclass_get_device: Invalid bus %d (err=%d)\n", busnum, ret);
		return ret;
	}
	mtd = get_mtd_device_nm("nor0");
	flash = dev_get_uclass_priv(dev);

	ret = mtd_arg_off("kernel", &idx, &off, &size, &maxsize, MTD_DEV_TYPE_NOR, flash->size);
	if (ret)
		return ret;

	pdata = (char *)memalign(0x100000, size);
	if (!pdata) {
		pr_err("cannot malloc buf for image\n");
		return -1;
	}

	printf("reading kernel image from SPI NOR Flash ...\n");
	mtd_read(mtd, off, size, &retlen, (u_char *) pdata);
	if (retlen != size)
		printf("kernel image read fail!\n");

	*image = pdata;
	return 0;
}

int get_kernel_dtb_image_nor(char **image, char **dtb)
{
	u32 ret;
	u32 busnum = 0;
	loff_t off = 0;
	int idx;
	loff_t size, maxsize;
	size_t retlen;
	struct udevice *dev;
	struct mtd_info *mtd;
	struct spi_flash *flash;
	char *pdata_kernel = (char *)KERNEL_IMAGE_ADDR;
	char *pdata_kernel_dtb = (char *)DTB_IMAGE_ADDR;

	ret = uclass_get_device(UCLASS_SPI_FLASH, busnum, &dev);
	if (ret) {
		printf("uclass_get_device: Invalid bus %d (err=%d)\n", busnum, ret);
		return ret;
	}
	mtd = get_mtd_device_nm("nor0");
	flash = dev_get_uclass_priv(dev);

	ret = mtd_arg_off("kernel", &idx, &off, &size, &maxsize, MTD_DEV_TYPE_NOR, flash->size);
	if (ret) {
		printf("%s, %d, ret=%d\n", __func__, __LINE__, ret);
		return ret;
	}

	printf("reading kernel image size 0x%llX from SPI NOR Flash ...\n", size);
	mtd_read(mtd, off, size, &retlen, (u_char *) pdata_kernel);
	if (retlen != size) {
		printf("kernel image read fail!\n");
		return -1;
	}
	*image = pdata_kernel;

	ret = mtd_arg_off("kernel-dtb", &idx, &off, &size, &maxsize, MTD_DEV_TYPE_NOR, flash->size);
	if (ret) {
		printf("%s, %d, ret=%d\n", __func__, __LINE__, ret);
		return ret;
	}

	printf("reading kernel-dtb image size 0x%llX from SPI NOR Flash ...\n", size);
	mtd_read(mtd, off, size, &retlen, (u_char *) pdata_kernel_dtb);
	if (retlen != size) {
		printf("kernel dtb read fail!\n");
		return -1;
	}
	*dtb = pdata_kernel_dtb;

	return 0;
}

int get_kernel_dtb_image_spinand(char **image, char **dtb)
{
	u32 ret, load_buf_off;
	u32 busnum = 0;
	loff_t off = 0;
	loff_t read_addr;
	int idx;
	loff_t size, maxsize;
	loff_t read_size, remaining, image_size, dtb_size;
	size_t retlen;
	struct udevice *dev;
	struct mtd_info *mtd;
	char *kernel_load_addr = (char *)KERNEL_IMAGE_ADDR;
	char *dtb_load_addr = (char *)DTB_IMAGE_ADDR;
	const image_header_t *hdr;
	const char *x_kernel = "kernel";
	const char *x_dtb = "kernel-dtb";

#ifdef CONFIG_AXERA_SUPPORT_AB_OTA
	char *bootpart = env_get("bootpart");

	if (bootpart == NULL) {
		x_kernel = "kernel";
		x_dtb = "kernel-dtb";
		env_set("bootargs", BOOTARGS_SPINAND_A);
	} else {
		if (strcmp(bootpart, "B") == 0) {
			x_kernel = "kernel_B";
			x_dtb = "kernel-dtb_B";
			env_set("bootargs", BOOTARGS_SPINAND_B);
		} else {
			x_kernel = "kernel";
			x_dtb = "kernel-dtb";
			env_set("bootargs", BOOTARGS_SPINAND_A);
		}
	}
#endif

	ret = uclass_get_device(UCLASS_MTD, busnum, &dev);
	if (ret) {
		printf("uclass_get_device: Invalid bus %d (err=%d)\n", busnum, ret);
		return ret;
	}
	mtd = dev_get_uclass_priv(dev);
/* for kernel */
	ret = mtd_arg_off(x_kernel, &idx, &off, &size, &maxsize, MTD_DEV_TYPE_NAND, mtd->size);
	if (ret) {
		printf("%s, %d, ret=%d\n", __func__, __LINE__, ret);
		return ret;
	}

	printf("reading %s image from SPI NAND Flash ...\n", x_kernel);
#if 0
	mtd_read(mtd, off, size, &retlen, (u_char *) kernel_load_addr);
	if (retlen != size) {
		printf("kernel image read fail!\n");
		return -1;
	}
#else
	if ((u64) off % mtd->erasesize) {
		pr_err("nand addr 0x%llX is not block size 0x%X aligned!\n", off, mtd->erasesize);
		return -1;
	}
	image_size = 0;
	load_buf_off = 0;
	read_addr = off;
	remaining = size;
	//printf("%s: read_addr=0x%llX, remaining=0x%X\n", __func__, read_addr, remaining);
	while (remaining) {
		if (mtd_block_isbad(mtd, read_addr)) {
			printf("nand addr 0x%llX, blk %lld is bad\n", read_addr, (u64) read_addr / mtd->erasesize);
			read_addr += mtd->erasesize;
			continue;
		}

		read_size = (remaining > mtd->erasesize) ? mtd->erasesize : remaining;
		if (read_addr + read_size > off + size) {
			pr_err("%s: read addr 0x%llX len 0x%llX over kernel part addr 0x%llX size 0x%llX\n",
			       __func__, read_addr, read_size, off, size);
			return -1;
		}

		mtd_read(mtd, read_addr, read_size, &retlen, (u_char *) (kernel_load_addr + load_buf_off));
		if (retlen != read_size) {
			pr_err("kernel image read fail!\n");
			return -1;
		}
		//printf("partiton kernel read 0x%X bytes, nand addr 0x%llX ==> buf addr 0x%X\n",
		//retlen, read_addr, (u32)(kernel_load_addr + load_buf_off));

		if (0 == image_size) {
			hdr = (const image_header_t *)kernel_load_addr;
			if (!image_check_magic(hdr)) {
				pr_err
				    ("Wrong Image Format: %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X\n",
				     kernel_load_addr[0x0], kernel_load_addr[0x1], kernel_load_addr[0x2],
				     kernel_load_addr[0x3], kernel_load_addr[0x4], kernel_load_addr[0x5],
				     kernel_load_addr[0x6], kernel_load_addr[0x7], kernel_load_addr[0x8],
				     kernel_load_addr[0x9], kernel_load_addr[0xA], kernel_load_addr[0xB],
				     kernel_load_addr[0xC], kernel_load_addr[0xD], kernel_load_addr[0xE],
				     kernel_load_addr[0xF]);
				return -1;
			}
			image_size = image_get_header_size() + image_get_data_size(hdr);
			if (image_size < read_size) {
				pr_err
				    ("Wrong Image Size: hdr size 0x%X, data size 0x%X, total size 0x%llX is smaller than first read size 0x%llX\n",
				     image_get_header_size(), image_get_data_size(hdr), image_size, read_size);
				return -1;
			}
			remaining = image_size;
			printf("kernel image hdr size 0x%X, data size 0x%X, update valid size 0x%llX\n",
			       image_get_header_size(), image_get_data_size(hdr), remaining);
		}
		remaining -= read_size;
		load_buf_off += read_size;
		read_addr += read_size;
	}
#endif
	*image = kernel_load_addr;

/* for kernel-dtb */
	ret = mtd_arg_off(x_dtb, &idx, &off, &size, &maxsize, MTD_DEV_TYPE_NAND, mtd->size);
	if (ret) {
		printf("%s, %d, ret=%d\n", __func__, __LINE__, ret);
		return ret;
	}

	printf("reading %s image from SPI NAND Flash ...\n", x_dtb);
#if 0
	mtd_read(mtd, off, size, &retlen, (u_char *) dtb_load_addr);
	if (retlen != size) {
		printf("kernel image-dtb read fail!\n");
		return -1;
	}
#else
	if ((u64) off % mtd->erasesize) {
		pr_err("nand addr 0x%llX is not block size 0x%X aligned!\n", off, mtd->erasesize);
		return -1;
	}
	dtb_size = 0;
	load_buf_off = 0;
	read_addr = off;
	remaining = size;
	//printf("%s: read_addr=0x%llX, remaining=0x%X\n", __func__, read_addr, remaining);
	while (remaining) {
		if (mtd_block_isbad(mtd, read_addr)) {
			printf("nand addr 0x%llX, blk %lld is bad\n", read_addr, (u64) read_addr / mtd->erasesize);
			read_addr += mtd->erasesize;
			continue;
		}

		read_size = (remaining > mtd->erasesize) ? mtd->erasesize : remaining;
		if (read_addr + read_size > off + size) {
			pr_err("%s: read addr 0x%llX len 0x%llX over kernel-dtb part addr 0x%llX size 0x%llX\n",
			       __func__, read_addr, read_size, off, size);
			return -1;
		}

		mtd_read(mtd, read_addr, read_size, &retlen, (u_char *) (dtb_load_addr + load_buf_off));
		if (retlen != read_size) {
			pr_err("kernel image read fail!\n");
			return -1;
		}
		//printf("partiton kernel-dtb read 0x%X bytes, nand addr 0x%llX ==> buf addr 0x%X\n",
		//retlen, read_addr, (u32)(kernel_load_addr + load_buf_off));

		if (0 == dtb_size) {
			if (fdt_check_header(dtb_load_addr) != 0) {
				pr_err
				    ("image is not a fdt: %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X\n",
				     dtb_load_addr[0x0], dtb_load_addr[0x1], dtb_load_addr[0x2], dtb_load_addr[0x3],
				     dtb_load_addr[0x4], dtb_load_addr[0x5], dtb_load_addr[0x6], dtb_load_addr[0x7],
				     dtb_load_addr[0x8], dtb_load_addr[0x9], dtb_load_addr[0xA], dtb_load_addr[0xB],
				     dtb_load_addr[0xC], dtb_load_addr[0xD], dtb_load_addr[0xE], dtb_load_addr[0xF]);
				return -1;
			}
			dtb_size = fdt_totalsize(dtb_load_addr);
			if (dtb_size < read_size) {
				remaining = 0;
				printf("kernel dtb size 0x%llX\n", dtb_size);
				break;
			}
			remaining = dtb_size;
			printf("warning: kernel dtb size 0x%X, update valid size 0x%llX\n", fdt_totalsize(dtb_load_addr),
			       remaining);
		}
		remaining -= read_size;
		load_buf_off += read_size;
		read_addr += read_size;
	}
#endif

	*dtb = dtb_load_addr;

	return 0;

}

int get_kernel_image_spinand(char **image, char **dtb)
{
	u32 ret;
	u32 busnum = 0;
	loff_t off = 0;
	int idx;
	loff_t size, maxsize;
	size_t retlen;
	struct udevice *dev;
	struct mtd_info *mtd;
	char *pdata = NULL;

	ret = uclass_get_device(UCLASS_MTD, busnum, &dev);
	if (ret) {
		printf("uclass_get_device: Invalid bus %d (err=%d)\n", busnum, ret);
		return ret;
	}
	mtd = dev_get_uclass_priv(dev);

	ret = mtd_arg_off("kernel", &idx, &off, &size, &maxsize, MTD_DEV_TYPE_NAND, mtd->size);
	if (ret) {
		printf("%s, %d, ret=%d\n", __func__, __LINE__, ret);
		return ret;
	}

	pdata = (char *)memalign(0x100000, size);
	if (!pdata) {
		printf("cannot malloc buf for image\n");
		return -1;
	}

	printf("reading kernel image from SPI NAND Flash ...\n");
	mtd_read(mtd, off, size, &retlen, (u_char *) pdata);
	if (retlen != size) {
		printf("kernel image read fail!\n");
		return -1;
	}
	*image = pdata;
	return 0;

}

int get_boot_image_pcie(char **image, char **dtb)
{
	int ret = 0;
	struct pci_msg_reg *reg = (struct pci_msg_reg *)PCIE_MSG_BASE;
	char *kernel_load_addr = (char *)KERNEL_IMAGE_ADDR;
	char *dtb_load_addr = (char *)DTB_IMAGE_ADDR;

	printf("Reading image ...");
	wdt0_enable(0);
	ret = polling_remote_command(reg);
	if(ret < 0) {
		wdt0_enable(1);
		printf("image read fail\n");
		return -1;
	} else {
		*dtb = dtb_load_addr;
		*image = kernel_load_addr;
	}
	wdt0_enable(1);
	return 0;
}


int get_atf_image_nor(char **atf_image, u64 kernel_addr)
{
	//todo
	return 0;
}

int get_atf_image_pcie(char **atf_image, u64 kernel_addr)
{
	char *header_load_addr = (char *)HEADER_IMAGE_ADDR;
	char *atf_load_addr = (char *)ATF_IMAGE_ADDR;
	const axera_image_header_t *hdr = (axera_image_header_t *)header_load_addr;

	memcpy(atf_load_addr, header_load_addr + hdr->raw_img_offset, hdr->raw_img_sz);
	printf("reading atf-img header end\n");

	*atf_image = atf_load_addr;

	return 0;
}

int get_atf_image_spinand(char **atf_image, u64 kernel_addr)
{
	u32 ret, load_buf_off;
	u32 busnum = 0;
	loff_t off = 0;
	loff_t read_addr;
	int idx;
	loff_t size, maxsize;
	loff_t read_size, remaining, atf_size;
	size_t retlen;
	struct udevice *dev;
	struct mtd_info *mtd;
	char *header_load_addr = (char *)HEADER_IMAGE_ADDR;
	char *atf_load_addr = (char *)ATF_IMAGE_ADDR;
	const axera_image_header_t *hdr;
	const char *x_atf = "atf";

#ifdef CONFIG_AXERA_SUPPORT_AB_OTA
	char *bootpart = env_get("bootpart");

	if (bootpart == NULL) {
		x_atf = "atf";
	} else {
		if (strcmp(bootpart, "B") == 0) {
			x_atf = "atf_B";
		} else {
			x_atf = "atf";
		}
	}
#endif

	ret = uclass_get_device(UCLASS_MTD, busnum, &dev);
	if (ret) {
		printf("uclass_get_device: Invalid bus %d (err=%d)\n", busnum, ret);
		return ret;
	}
	mtd = dev_get_uclass_priv(dev);

	printf("reading atf-img header begin\n");
	ret = mtd_arg_off(x_atf, &idx, &off, &size, &maxsize, MTD_DEV_TYPE_NAND, mtd->size);
	if (ret) {
		printf("%s, %d, ret=%d\n", __func__, __LINE__, ret);
		return ret;
	}

	printf("reading atf image from SPI NAND Flash ...\n");
	if ((u64) off % mtd->erasesize) {
		pr_err("nand addr 0x%llX is not block size 0x%X aligned!\n", off, mtd->erasesize);
		return -1;
	}
	atf_size = 0;
	load_buf_off = 0;
	read_addr = off;
	remaining = size;
	printf("%s: read_addr=0x%llX, remaining=0x%llX\n", __func__, read_addr, remaining);
	while (remaining) {
		if (mtd_block_isbad(mtd, read_addr)) {
			printf("nand addr 0x%llX, blk %lld is bad\n", read_addr, (u64) read_addr / mtd->erasesize);
			read_addr += mtd->erasesize;
			continue;
		}

		read_size = (remaining > mtd->erasesize) ? mtd->erasesize : remaining;
		if (read_addr + read_size > off + size) {
			pr_err("%s: read addr 0x%llX len 0x%llX over kernel part addr 0x%llX size 0x%llX\n",
			       __func__, read_addr, read_size, off, size);
			return -1;
		}

		mtd_read(mtd, read_addr, read_size, &retlen, (u_char *) (header_load_addr + load_buf_off));
		if (retlen != read_size) {
			pr_err("kernel image read fail!\n");
			return -1;
		}
		//printf("partiton kernel read 0x%X bytes, nand addr 0x%llX ==> buf addr 0x%X\n",
		//retlen, read_addr, (u32)(kernel_load_addr + load_buf_off));

		if (0 == atf_size) {
			hdr = (axera_image_header_t *)header_load_addr;
			if (strncmp((char *)hdr->magic, HEADER_MAGIC, AXIMG_HEADER_SIZE) != 0) {
				printf("%s: atf-img header magic is error!\n", __FUNCTION__);
				printf("[ATF]Wrong Image Format: %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X\n",
				     atf_load_addr[0x0], atf_load_addr[0x1], atf_load_addr[0x2], atf_load_addr[0x3],
				     atf_load_addr[0x4], atf_load_addr[0x5], atf_load_addr[0x6], atf_load_addr[0x7],
				     atf_load_addr[0x8], atf_load_addr[0x9], atf_load_addr[0xA], atf_load_addr[0xB],
				     atf_load_addr[0xC], atf_load_addr[0xD], atf_load_addr[0xE], atf_load_addr[0xF]);
				return -1;
			}
			atf_size = hdr->img_size;
			if (atf_size < read_size) {
				remaining = 0;
				printf("atf image size 0x%llX\n", atf_size);
				break;
			}
			remaining = atf_size;
			printf("warning: atf image size 0x%X, update valid size 0x%llX\n", hdr->img_size, remaining);
		}
		remaining -= read_size;
		load_buf_off += read_size;
		read_addr += read_size;
	}
	memcpy(atf_load_addr, header_load_addr + hdr->raw_img_offset, hdr->raw_img_sz);
	printf("reading atf-img header end\n");

	*atf_image = atf_load_addr;

	return 0;
}


bootm_headers_t atf_boot_images;

#ifdef CONFIG_LMB
static void atf_boot_start_lmb(bootm_headers_t *images)
{
	ulong mem_start;
	phys_size_t mem_size;

	mem_start = env_get_bootm_low();
	mem_size = env_get_bootm_size();

	lmb_init_and_reserve_range(&images->lmb, (phys_addr_t)mem_start,
				   mem_size, NULL);
}
#else
#define lmb_reserve(lmb, base, size)
static inline void boot_start_lmb(bootm_headers_t *images) { }
#endif

int atf_boot_prepare(char *kimg_addr, char *dtb_addr)
{
	ulong relocated_addr;
	ulong image_size;
	int ret;
	ulong ld = (ulong)kimg_addr;

	memset((void *)&atf_boot_images, 0, sizeof(atf_boot_images));
	atf_boot_start_lmb(&atf_boot_images);

	/* kernel image handle */
	ret = booti_setup(ld, &relocated_addr, &image_size, false);
	if (ret != 0)
		return -1;

	if (relocated_addr != ld) {
		printf("moving kernel image from 0x%lx to 0x%lx\n", ld, relocated_addr);
		memmove((void *)relocated_addr, (void *)ld, image_size);
	}

	atf_boot_images.ep = relocated_addr;
	atf_boot_images.os.start = relocated_addr;
	atf_boot_images.os.end = relocated_addr + image_size;

	lmb_reserve(&atf_boot_images.lmb, atf_boot_images.ep, le32_to_cpu(image_size));

	/* linux dtb handle */
	atf_boot_images.ft_addr = dtb_addr;
	atf_boot_images.ft_len = fdt_totalsize(dtb_addr);
#ifdef CONFIG_OF_LIBFDT
	if (image_setup_linux(&atf_boot_images)) {
		printf("fdt creation failed! hanging...");
		return -1;
	}
#endif

#ifdef CONFIG_VIDEO_AXERA
	extern void fdt_fixup_logo_info(int dev, u64 addr, u64 size, void *fdt);

	fdt_fixup_logo_info(LOGO_SHOW_DEVID, LOGO_SHOW_BUFFER, LOGO_SHOW_BUF_SIZE, atf_boot_images.ft_addr);
#endif
	if (atf_boot_images.ft_addr != dtb_addr) {
		printf("moving dtb image from 0x%lx to 0x%lx, fdt_totalsize:%u\n",
				(ulong)atf_boot_images.ft_addr, (ulong)dtb_addr,
				fdt_totalsize(atf_boot_images.ft_addr));
		memmove((void *)dtb_addr, (void *)atf_boot_images.ft_addr,
						fdt_totalsize(atf_boot_images.ft_addr));

		atf_boot_images.ft_addr = dtb_addr;
	}
	printf("aftre handle, kernel_addr:%lx, fdt_addr:%lx\n",
			atf_boot_images.ep, (ulong)atf_boot_images.ft_addr);

	/* atf boot params */
	atf_bl_params.h.type = PARAM_BL_PARAMS;
	atf_bl_params.h.version = VERSION_2;
	atf_bl_params.head = &atf_bl33_params;

	atf_bl33_params.image_id = BL33_IMAGE_ID;
#ifdef OPTEE_BOOT
	atf_bl33_params.next_params_info = &atf_bl32_params;
#else
	atf_bl33_params.next_params_info = NULL;
#endif
	atf_bl33_params.ep_info = &atf_bl33_ep_info;

	atf_bl33_ep_info.pc = atf_boot_images.ep;
	atf_bl33_ep_info.spsr = SPSR_64(MODE_EL1, MODE_SP_ELX, DISABLE_ALL_EXCEPTIONS);
	SET_PARAM_HEAD(&atf_bl33_ep_info, PARAM_EP, VERSION_1, EP_NON_SECURE);

#ifdef OPTEE_BOOT
	atf_bl32_params.image_id = BL32_IMAGE_ID;
	atf_bl32_params.next_params_info = NULL;
	atf_bl32_params.ep_info = &atf_bl32_ep_info;

	atf_bl32_ep_info.pc = OPTEE_IMAGE_ADDR;
	atf_bl32_ep_info.spsr = SPSR_64(MODE_EL1, MODE_SP_ELX, DISABLE_ALL_EXCEPTIONS);
	SET_PARAM_HEAD(&atf_bl32_ep_info, PARAM_EP, VERSION_1, EP_SECURE);
#endif
	return 0;
}

#ifdef CONFIG_TARGET_AX650_XPILOT_6V
#define XPILOT_6V_DTB_OFFSET    0X40000
#endif

int do_axera_boot(cmd_tbl_t * cmdtp, int flag, int argc, char *const argv[])
{
	int ret = 0;
	char *img_addr = NULL;
	char *dtb_addr = NULL;
	atf_boot_fn atf_boot;
	char *x_kernel = "kernel";
	char *x_dtb = "dtb";
	char *x_atf = "atf";
	char *x_optee = "optee";

	pr_err("enter do_axera_boot\n");
	/* Turn on watchdog before booting to kernel. */
	wdt0_enable(1);
#ifndef CONFIG_SLAVE_PCIE_BOOT
	ret = AX_CIPHER_Init();
	if (ret != 0) {
		printf("cipher init failed\n");
		return SECURE_CIPHER_FAIL;
	}
#endif
#ifdef CONFIG_SLAVE_PCIE_BOOT
	boot_info_data.storage_sel = STORAGE_TYPE_PCIE;
#endif

	switch (boot_info_data.storage_sel) {
	case STORAGE_TYPE_EMMC:
		printf("boot from emmc\n");
#ifdef CONFIG_AXERA_SUPPORT_AB_OTA
		char *bootpart = env_get("bootpart");

		if (bootpart == NULL) {
			x_kernel = "kernel";
			x_dtb = "dtb";
			x_atf = "atf";
			x_optee = "optee";
			env_set("bootargs", BOOTARGS_EMMC_A);
		} else {
			if (strcmp(bootpart, "B") == 0) {
				x_kernel = "kernel_B";
				x_dtb = "dtb_B";
				x_atf = "atf_B";
				x_optee = "optee_B";
				env_set("bootargs", BOOTARGS_EMMC_B);
			} else {
				x_kernel = "kernel";
				x_dtb = "dtb";
				x_atf = "atf";
				x_optee = "optee";
				env_set("bootargs", BOOTARGS_EMMC_A);
			}
		}
#endif

#ifdef CONFIG_BOOT_OPTIMIZATION
#ifdef SUPPORT_RECOVERY
		char *bootable = env_get("bootable");
		char *bootcount = env_get("bootcount");
		if ((!strcmp(bootable, "recovery")) || (!strcmp(bootcount, "1"))) {
			printf("From recovery boot\n");
			x_kernel = "recovery";
			x_dtb = "recovery_dtb";
			env_set("bootargs", RECOVERY_BOOTARGS);
		}
		env_set("bootcount", "1");
#endif
		ret =get_boot_image_emmc(x_dtb, (void *)DTB_IMAGE_ADDR);
		if (!ret)
			dtb_addr = (char *)DTB_IMAGE_ADDR;

		if (!ret)
			ret =get_boot_image_emmc(x_kernel, (void *)KERNEL_IMAGE_ADDR);
		if (!ret)
			img_addr = (char *)KERNEL_IMAGE_ADDR;
#else
		ret = get_kernel_dtb_image_emmc(&img_addr, &dtb_addr);
#endif
		if (!ret)
			ret = get_boot_image_emmc(x_atf, (void *)ATF_IMAGE_ADDR);

#ifdef OPTEE_BOOT
		if (!ret)
			ret = get_boot_image_emmc(x_optee, (void *)OPTEE_IMAGE_ADDR);
#endif
		break;
	case STORAGE_TYPE_NAND:
		env_set("bootargs", BOOTARGS_SPINAND);
#ifdef CONFIG_BOOT_OPTIMIZATION
		ret = get_axera_boot_image_spinand(&img_addr, &dtb_addr);
#else
		ret = get_kernel_dtb_image_spinand(&img_addr, &dtb_addr);
#endif
		if (!ret)
			ret = get_atf_image_spinand((char **)&atf_boot, (u64)img_addr);
		break;
	case STORAGE_TYPE_NOR:
		env_set("mtdids", MTDIDS_SPINOR);
		env_set("mtdparts", MTDPARTS_SPINOR);
		env_set("bootargs", BOOTARGS_SPINOR);
		ret = get_kernel_dtb_image_nor(&img_addr, &dtb_addr);
		if (!ret)
			ret = get_atf_image_nor((char **)&atf_boot, (u64)img_addr);
		break;
	case STORAGE_TYPE_PCIE:
		printf("boot from pcie\n");
		env_set("bootargs", BOOTARGS_PCIE);
		ret = get_boot_image_pcie(&img_addr, &dtb_addr);
		if (!ret)
			ret = get_atf_image_pcie((char **)&atf_boot, (u64)img_addr);
		break;
	case STORAGE_TYPE_SSD:
		printf("boot from ssd\n");
#ifdef CONFIG_BOOT_OPTIMIZATION
		ret =get_boot_image_ssd("dtb", (void *)DTB_IMAGE_ADDR);
		if (!ret)
			dtb_addr = (char *)DTB_IMAGE_ADDR;

		if (!ret)
			ret =get_boot_image_ssd("kernel", (void *)KERNEL_IMAGE_ADDR);
		if (!ret)
			img_addr = (char *)KERNEL_IMAGE_ADDR;
#else
		ret = get_kernel_dtb_image_ssd(&img_addr, &dtb_addr);
#endif
		if (!ret)
			ret = get_boot_image_ssd("atf", (void *)ATF_IMAGE_ADDR);

#ifdef OPTEE_BOOT
		if (!ret)
			ret = get_boot_image_ssd("optee", (void *)OPTEE_IMAGE_ADDR);
#endif
		break;
	default:
		pr_err("unknown storage type selected\n");
		ret = -1;
		break;
	}
#ifndef CONFIG_SLAVE_PCIE_BOOT
	AX_CIPHER_DeInit();
#endif

	if (ret) {
		pr_err("get start image fail\n");
		return ret;
	}

#ifdef CONFIG_TARGET_AX650_XPILOT_6V
	/* xpilot_6v project alpha dtb offset=0,beta dtb offset=0x40000 */
	dtb_addr = dtb_addr + XPILOT_6V_DTB_OFFSET * get_board_id();
#endif

	if (!ret && !atf_boot_prepare(img_addr, dtb_addr)) {
		printf("now enter atf boot\n");
		dcache_disable();
		atf_boot = (atf_boot_fn)ATF_IMAGE_ADDR;
		atf_boot((u64)&atf_bl_params, 0, (u64)atf_boot_images.ft_addr,
					ARM_BL31_PLAT_PARAM_VAL);
	}

	//in fact will not come here
	pr_err("unknown error when boot from atf\n");
	while (1);

	return ret;
}

U_BOOT_CMD(axera_boot, 1, 0, do_axera_boot,
	   "axera boot", "axera enter normal boot mode\n" "it is used for axera boot to kernel\n");
