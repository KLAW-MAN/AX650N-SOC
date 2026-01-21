/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#include <common.h>
#include <asm/arch/boot_mode.h>
#include <asm/arch/ax650.h>
#include <asm/unaligned.h>
#include <asm/io.h>
#include <bmp_layout.h>

#include <splash.h>
#include <lcd.h>
#include "linux/delay.h"
#include <gzip.h>
#include <malloc.h>
#include <mapmem.h>

#include "stb_image.h"
#include "ax_vo.h"
#include "ax_simple_logo.h"

extern int get_part_info(struct blk_desc *dev_desc, const char *name, disk_partition_t * info);

void writeBMPHeader(unsigned char *buffer, int width, int height) {
	int channels = 3;
	int padding = (4 - (width * channels) % 4) % 4;
	int rowSize = width * channels + padding;
	int fileSize = 64 + rowSize * height;

	unsigned char header[64] = {
		0x42, 0x4D, 0, 0, 0, 0, 0, 0, 0, 0, 64, 0, 0, 0, 40, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 24, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
	};

	header[2] = (unsigned char)(fileSize & 0xFF);
	header[3] = (unsigned char)((fileSize >> 8) & 0xFF);
	header[4] = (unsigned char)((fileSize >> 16) & 0xFF);
	header[5] = (unsigned char)((fileSize >> 24) & 0xFF);
	header[18] = (unsigned char)(width & 0xFF);
	header[19] = (unsigned char)((width >> 8) & 0xFF);
	header[20] = (unsigned char)((width >> 16) & 0xFF);
	header[21] = (unsigned char)((width >> 24) & 0xFF);
	header[22] = (unsigned char)(height & 0xFF);
	header[23] = (unsigned char)((height >> 8) & 0xFF);
	header[24] = (unsigned char)((height >> 16) & 0xFF);
	header[25] = (unsigned char)((height >> 24) & 0xFF);
	memcpy(buffer, header, 54);
	/* fill in the remaining header bytes of the file */
	memset(buffer + 54, 0, 10);
}

void resizeImage(int originalHeight, int originalWidth, int newHeight, int newWidth, int channels, unsigned char *originalImage, unsigned char *resizedImage, bool bReverse, bool bSwapRB) {
	int flipVertical = bReverse ? 1 : 0;
	int padding = (4 - (newWidth * channels) % 4) % 4;

	for (int i = 0; i < newHeight; i++) {
		for (int j = 0; j < newWidth; j++) {
			int x = (j * originalWidth + newWidth / 2) / newWidth;
			int y = (i * originalHeight + newHeight / 2) / newHeight;
			for (int c = 0; c < channels; c++) {
				int srcIndex = (y * originalWidth + x) * channels + c;
				int dstIndex = ((flipVertical ? (newHeight - 1 - i) : i) * (newWidth * channels + padding)) + (j * channels) + c;

				if (bSwapRB && (c == 0 || c == 2)) {
					resizedImage[dstIndex] = originalImage[srcIndex + (c == 0 ? 2 : -2)]; // Swap R and B channels
				} else {
					resizedImage[dstIndex] = originalImage[srcIndex];
				}
			}
		}
		for (int p = 0; p < padding; p++) {
			int dstIndex = ((flipVertical ? (newHeight - 1 - i) : i) * (newWidth * channels + padding) + newWidth * channels) + p;
			resizedImage[dstIndex] = 0;
		}
	}
}

static unsigned char* gunzip_logo_img(unsigned long addr, unsigned long *lenp,
					 void **alloc_addr)
{
	void *outAddr = NULL;
	unsigned long len = 0;
	unsigned char *logoData = NULL;
	int ret = 0;

	/*
	 * Decompress  image
	 */
	len = AX_MAX_VO_LOGO_SIZE;
	/* allocate extra 3 bytes for 32-bit-aligned-address + 16 alignment */
	outAddr = malloc(AX_MAX_VO_LOGO_SIZE + 16);
	if (outAddr == NULL) {
		printf("Error: malloc in gunzip failed!\n");
		return NULL;
	}

	/* align to 32-bit-aligned-address + 16 */
	logoData = (unsigned char *)(((uintptr_t)outAddr + 0xf) & ~0xf); //align 16

	ret = gunzip(logoData, AX_MAX_VO_LOGO_SIZE, map_sysmem(addr, 0), &len);
	if (ret != 0) {
		printf("Error: gunzip failed. ret:%d!\n", ret);
		free(outAddr);
		return NULL;
	}

	if (len == AX_MAX_VO_LOGO_SIZE) {
		printf("Image could be truncated. len %ld > AX_MAX_VO_LOGO_SIZE:%d\n",
			len, AX_MAX_VO_LOGO_SIZE);
		free(outAddr);
		return NULL;
	}

	printf("Gzipped logo image detected!\n");
	*alloc_addr = outAddr;
	*lenp = len;
	return logoData;
}


static int emmc_parse_jpg_logo_data(void *imageData_jpg, void *logo_load_addr)
{
	int ret = 0;
	int width, height, channels;
	unsigned char *bmp_data = NULL;
	unsigned char *imageDecData = NULL;
	int newWidth = 0;
	int newHeight = 0;
	int padding = 0;
	int rowSize = 0;
	unsigned char *resized_imageData = NULL;

	if (NULL == imageData_jpg) {
		printf("imageData_jpg == NULL.\n");
		ret = -1;
		goto ERR_RET;
	}

	/* Use the stb_image library to decode JPEG data */
	imageDecData = stbi_load_from_memory(imageData_jpg, AX_MAX_VO_JPG_WIDTH * AX_MAX_VO_JPG_HEIGHT * AX_VO_JPG_CHANNEL, &width, &height, &channels, 0);
	if (!imageDecData) {
		printf("Unable to decode JPEG data\n");
		ret = -1;
		goto ERR_RET_FREE;
	}

	printf("func:%s, jpeg param: width:%d, height:%d, channels:%d.\n", __func__, width, height, channels);
	if (width * height * channels > AX_MAX_VO_JPG_WIDTH * AX_MAX_VO_JPG_HEIGHT * AX_VO_JPG_CHANNEL) {
		printf("jpeg resolution out of range[%d, %d]\n", AX_MAX_VO_JPG_WIDTH, AX_MAX_VO_JPG_HEIGHT);
		ret = -1;
		goto ERR_RET_FREE;
	}

#if 0
	if (((width == 1920) && (height == 1080)) ||
	    ((width == 1280) && (height == 1024)) ||
	    ((width == 1024) && (height == 768))) {
		newWidth = width;
		newHeight = height;
	} else if ((width < AX_MIN_RESIZE_WIDTH) && (height < AX_MIN_RESIZE_HEIGHT)) {
		newWidth = AX_VO_JPEG_ALIGN(width, 16);
		newHeight = AX_VO_JPEG_ALIGN(height, 2);
	} else {
		newWidth = AX_MIN_RESIZE_WIDTH;
		newHeight = AX_MIN_RESIZE_HEIGHT;
	}
#else
	newWidth = AX_VO_JPEG_ALIGN(width, 16);
	newHeight = AX_VO_JPEG_ALIGN(height, 2);
#endif

	padding = (4 - (newWidth * channels) % 4) % 4;
	rowSize = newWidth * channels + padding;
	resized_imageData = (unsigned char *)memalign(0x100000, newHeight * rowSize);
	if (NULL == resized_imageData) {
		printf("resize malloc failed.\n");
		ret = -1;
		goto ERR_RET_FREE;
	}

	printf("resize_image: newWidth:%d, newHeight:%d\n", newWidth, newHeight);
	resizeImage(height, width, newHeight, newWidth, channels, imageDecData, resized_imageData, 0, 1);

	/* Write BMP image data to buffer */
	bmp_data = (unsigned char *)logo_load_addr;
	writeBMPHeader(bmp_data, newWidth, newHeight);
	memcpy(bmp_data + 64, resized_imageData, newWidth * newHeight * AX_RESIZE_CHANNEL + newHeight * ((16 - (newWidth * channels) % 16) % 16));

	printf("func:%s end ++++++++++.\n", __func__);

ERR_RET_FREE:
	if (resized_imageData) {
		free(resized_imageData);
		resized_imageData = NULL;
	}

	if (imageDecData) {
		free(imageDecData);
		imageDecData = NULL;
	}

ERR_RET:
	return ret;
}

int get_logo_from_emmc(unsigned char *logo_load_addr)
{
	u64 rd_blkcnt_lb_logo;
	struct blk_desc *blk_dev_desc = NULL;
	disk_partition_t part_info;
	u32 ret = 0;

	blk_dev_desc = blk_get_dev("mmc", EMMC_DEV_ID);
	if (!blk_dev_desc) {
		printf("get mmc dev fail\n");
		return -1;
	}
	printf("reading splash logo image++ ...\n");

	ret = get_part_info(blk_dev_desc,"logo", &part_info);
	if(ret == -1) {
		printf("%s: get logo partition info fail\n", __FUNCTION__);
		return -1;
	}

	rd_blkcnt_lb_logo = blk_dread(blk_dev_desc, part_info.start,
						part_info.size, logo_load_addr);
	if (rd_blkcnt_lb_logo != part_info.size) {
		printf("get_logo_from_emmc get logo image fail++ rd_blkcnt_lb_logo %llx part_info.size: %lx\n", rd_blkcnt_lb_logo, part_info.size);
		return -1;
	}

	printf("load logo image addr = 0x%llx\n",(u64)logo_load_addr);
	return 0;
}

static int fdt_fixup_logo_reserved_mem(int dev, u64 addr, u64 size, void *fdt)
{
	int ret, offset, parent_offset;
	const uint32_t devid = cpu_to_fdt32(dev);
	const uint64_t reserved_addr = cpu_to_fdt64(addr);
	const uint64_t reserved_size = cpu_to_fdt64(size);
	char name[128];

	parent_offset = fdt_path_offset(fdt, "/reserved-memory/");
	if (parent_offset < 0) {
		VO_ERROR("reserved_mem not found\n");
		return -EINVAL;
	}

	sprintf(name, "boot_logo_reserved@%llx", addr);

	offset = fdt_add_subnode(fdt, parent_offset, name);
	if (offset < 0) {
		VO_ERROR("add %s to reserved_mem failed, ret = %d\n", name, offset);
		return offset;
	}

	ret = fdt_setprop(fdt, offset, "compatible", "boot_logo", 9);
	if (ret) {
		VO_ERROR("set compatible-prop to %s failed, ret = %d\n", name, ret);
		goto exit;
	}

	ret = fdt_setprop(fdt, offset, "id", &devid, sizeof(devid));
	if (ret) {
		VO_ERROR("set id-prop to %s failed, ret = %d\n", name, ret);
		goto exit;
	}

	ret = fdt_setprop(fdt, offset, "reg", &reserved_addr, sizeof(reserved_addr));
	if (ret) {
		VO_ERROR("set reg-prop to %s failed, ret = %d\n", name, ret);
		goto exit;
	}

	ret = fdt_appendprop(fdt, offset, "reg", &reserved_size, sizeof(reserved_size));
	if (ret) {
		VO_ERROR("append reg-prop to %s failed, ret = %d\n", name, ret);
		goto exit;
	}

exit:
	if (ret)
		fdt_del_node(fdt, offset);

	VO_INFO("add %s node %s\n", name, ret ? "failed" : "success");

	return ret;
}

void fdt_fixup_logo_info(int dev, u64 addr, u64 size, void *fdt)
{
	if (dev >= VO_NR) {
		VO_ERROR("dev(%d) invalid\n", dev);
		return;
	}

	if (!fdt || fdt_check_header(fdt)) {
		VO_ERROR("device tree invalid\n");
		return;
	}

	if (!fdt_fixup_vo_init_mode(dev, fdt))
		fdt_fixup_logo_reserved_mem(dev, addr, size, fdt);
}

static u32 bmp_bpix2vo_fmt(u32 bpix)
{
	if (bpix == 16)
		return AX_VO_FORMAT_RGB565;
	else if (bpix == 24)
		return AX_VO_FORMAT_RGB888;
	else
		return AX_VO_FORMAT_ARGB8888;
}

extern struct simple_panel_cfg g_panel_cfg;
extern u32 g_vdev_id;
extern u32 g_out_type;
extern u32 g_fixed_sync;
extern int get_part_info(struct blk_desc *dev_desc, const char *name, disk_partition_t * info);
int ax_bootlogo_show(void)
{
	int ret = 0;
	unsigned long width, height;
	unsigned data_offs, bmp_bpix;
	void *logo_load_addr = (void *)LOGO_IMAGE_LOAD_ADDR;
	boot_mode_info_t *boot_mode = (boot_mode_info_t *) BOOT_MODE_INFO_ADDR;
	struct display_info dp_info = {0};
	struct bmp_image *bmp;
	unsigned char *imageData_logo = NULL;
	unsigned char *imageData_jpg = NULL;
	unsigned char *imageData_gz = NULL;
	AX_VO_LOGO_FMT_E inLogoFmt = AX_VO_LOGO_FMT_NONE;
	AX_VO_LOGO_FMT_E outLogoFmt = AX_VO_LOGO_FMT_NONE;
	unsigned long gzLen = 0;
	void *gz_alloc_addr = NULL;
	u32 display_x = 0;
	u32 display_y = 0;
	u32 vdev = g_vdev_id;
	u32 type = g_out_type;
	u32 sync = g_fixed_sync;

	memset((void *)boot_mode, 0, sizeof(boot_mode_info_t));
	boot_mode->magic = BOOT_MODE_ENV_MAGIC;
	boot_mode->dl_channel = readl(COMM_SYS_DUMMY_SW5);

	if (boot_mode->dl_channel != DL_CHAN_UART1 &&
	    boot_mode->dl_channel != DL_CHAN_USB &&
	    boot_mode->dl_channel != DL_CHAN_SD) {

		imageData_logo = memalign(AX_VO_LOGO_ALIGN_SIZE, AX_MAX_VO_LOGO_SIZE);
		if (NULL == imageData_logo) {
			printf("jpeg malloc failed.\n");
			return -1;
		}

		ret = get_logo_from_emmc(imageData_logo);
		if (ret < 0) {
			printf("fail to read logo from emmc partition\n");
			ret = -1;
			goto ERR_RET;
		}

		bmp = (struct bmp_image *)imageData_logo;
		if (imageData_logo[0] == 0x1F && (imageData_logo[1] == 0x8B)) {
			inLogoFmt = AX_VO_LOGO_FMT_GZ;
			printf("%s inLogoFmt: %s\n", __func__, "FMT_GZ");
		} else if ((imageData_logo[0] == 0xFF) && (imageData_logo[1] == 0xD8)) {
			outLogoFmt = inLogoFmt = AX_VO_LOGO_FMT_JPEG;
			imageData_jpg = imageData_logo;
			printf("%s inLogoFmt: %s\n", __func__, "FMT_JPEG");
		} else if ((bmp->header.signature[0]=='B') && (bmp->header.signature[1]=='M')) {
			outLogoFmt = inLogoFmt = AX_VO_LOGO_FMT_BMP;
			memcpy(logo_load_addr, imageData_logo, AX_MAX_VO_LOGO_SIZE);
			printf("%s inLogoFmt: %s\n", __func__, "FMT_BMP");
		} else {
			printf("invalid logo fmt. logo header: 0x%x, 0x%x\n", imageData_logo[0], imageData_logo[1]);
			ret = -1;
			goto ERR_RET;
		}


		if (inLogoFmt == AX_VO_LOGO_FMT_GZ) {
			imageData_gz = gunzip_logo_img((unsigned long)imageData_logo, &gzLen, &gz_alloc_addr);
			if (imageData_gz == NULL) {
				printf("gunzip_logo_img failed\n");
				ret = -1;
				goto ERR_RET;
			}

			bmp = (struct bmp_image *)imageData_gz;
			if ((imageData_gz[0] == 0xFF) && (imageData_gz[1] == 0xD8)) {
				outLogoFmt = AX_VO_LOGO_FMT_JPEG;
				imageData_jpg = imageData_gz;
				printf("%s outLogoFmt: %s\n", __func__, "FMT_JPEG");
			} else if ((bmp->header.signature[0]=='B') && (bmp->header.signature[1]=='M')) {
				outLogoFmt = AX_VO_LOGO_FMT_BMP;
				memcpy(logo_load_addr, imageData_gz, gzLen);
				printf("%s outLogoFmt: %s\n", __func__, "FMT_BMP");
			} else {
				printf("invalid logo fmt. logo header: 0x%x, 0x%x\n", imageData_logo[0], imageData_logo[1]);
				ret = -1;
				goto ERR_RET;
			}
		}


		if (outLogoFmt == AX_VO_LOGO_FMT_JPEG) {
			ret = emmc_parse_jpg_logo_data(imageData_jpg, logo_load_addr);
			if (ret < 0) {
				printf("%s failed emmc_parse_jpg_logo_data\n", __func__);
				ret = -1;
				goto ERR_RET;
			}
		}

		bmp = (struct bmp_image *)logo_load_addr;
		if (!((bmp->header.signature[0]=='B') && (bmp->header.signature[1]=='M'))) {
			printf("%s bmp invalid\n", __func__);
			ret = -1;
			goto ERR_RET;
		}

		width = get_unaligned_le32(&bmp->header.width);
		height = get_unaligned_le32(&bmp->header.height);
		bmp_bpix = get_unaligned_le16(&bmp->header.bit_count);
		data_offs = get_unaligned_le16(&bmp->header.data_offset);
		printf("%s bmp info: [%ld, %ld, %d, %d]\n", __func__, width, height, bmp_bpix, data_offs);

		if ((bmp_bpix != 16) && (bmp_bpix != 24) && (bmp_bpix != 32)) {
			printf("%s bmp bpix(%d) invalid\n", __func__, bmp_bpix);
			ret = -1;
			goto ERR_RET;
		}

		dp_info.img_width = width;
		dp_info.img_height = height;
		dp_info.img_stride = width * (bmp_bpix >> 3);
		dp_info.img_fmt = bmp_bpix2vo_fmt(bmp_bpix);
		dp_info.img_addr = (u64)logo_load_addr + data_offs;

		if (width < AX_MIN_RESIZE_WIDTH)
			display_x = ((AX_MIN_RESIZE_WIDTH - width) / 2) & (~0x1);
		if (height < AX_MIN_RESIZE_HEIGHT)
			display_y = ((AX_MIN_RESIZE_HEIGHT - height) / 2) & (~0x1);

		dp_info.display_x = display_x;
		dp_info.display_y = display_y;
		dp_info.display_addr = LOGO_SHOW_BUFFER;

		if (sync >= AX_VO_OUTPUT_BUTT) {
			if ((dp_info.img_width == 1920) && (dp_info.img_height == 1080)) {
				sync = AX_VO_OUTPUT_1080P60;
			} else if ((dp_info.img_width == 1280) && (dp_info.img_height == 1024)) {
				sync = AX_VO_OUTPUT_1280x1024_60;
			} else if ((dp_info.img_width <= 1024) && (dp_info.img_height <= 768)) {
				sync = AX_VO_OUTPUT_1024x768_60;
			} else {
				printf("%s unsupported resolution(%dx%d)\n", __func__, dp_info.img_width, dp_info.img_height);
				ret = -1;
				goto ERR_RET;
			}
		}

		dp_info.priv = &g_panel_cfg;

		ret = ax_start_vo(vdev, type, sync, &dp_info);
	}

ERR_RET:
	if (ret)
		printf("%s failed to show logo\n", __func__);
	if (imageData_logo) {
		free(imageData_logo);
		imageData_logo = NULL;
	}

	if (gz_alloc_addr) {
		free(gz_alloc_addr);
		imageData_gz = NULL;
	}

	return ret;
}

void ax_bootlogo_handle(void)
{
	ax_bootlogo_show();
}