#include <common.h>
#include <malloc.h>
#include <mtd.h>
#include <blk.h>
#include <asm/io.h>
#include <memalign.h>
#include <linux/sizes.h>
#include <linux/string.h>
#include <dm/uclass.h>
#include <dm/device.h>
#include <linux/dma-mapping.h>
#include "secureboot.h"
#include "../arch/arm/mach-axera/ax650/ax_efuse_api.h"
#include "../cipher/ax_cipher_api.h"
#include "../cipher/ax_base_type.h"

#define SEC_CLK_RST_BASE    (0x400F000)
#define COMM_SYS_GLB_BASE	(0x4200000)     // aon_sys, chip_top_rf
#define COMMON_CK_RST_BASE	(0x4210000)     // aon_sys, comm_ck_rst_cfg, AON_CLK_RST
#define PUB_CE_BASE			(0x4540000)
#define SEC_CE_BASE			(0x4600000)
#define COMM_SYS_BOND_OPT0	(COMM_SYS_GLB_BASE + 0x20)
#define COMM_SYS_BOND_OPT1	(COMM_SYS_GLB_BASE + 0x24)

#define SECURE_BOOT_EN      (1 << 24)
#define HASH_BLK_NUM        (8)

#define EIP130_REG_BASE                 (PUB_CE_BASE)
#define EIP130_MAILBOX_IN_BASE          (EIP130_REG_BASE)
#define EIP130_MAILBOX_OUT_BASE         (0x0000)
#define EIP130_MAILBOX_SPACING_BYTES    (0x0400) // Actual mailbox size independent
#define EIP130_FIRMWARE_RAM_BASE        (0x4000 + EIP130_REG_BASE)
#define EIP130_MAILBOX_STAT             (0x3F00 + EIP130_REG_BASE)
#define EIP130_MAILBOX_CTRL             (0x3F00 + EIP130_REG_BASE)
#define EIP130_MAILBOX_LOCKOUT          (0x3F10 + EIP130_REG_BASE)
#define EIP130_MODULE_STATUS            (0x3FE0 + EIP130_REG_BASE)
#define EIP130_CRC24_OK                 (1 << 9)
#define EIP130_FIRMWARE_WRITTEN         (1 << 20)
#define EIP130_FIRMWARE_CHECKS_DONE     (1 << 22)
#define EIP130_FIRMWARE_ACCEPTED        (1 << 23)
#define EIP130_FATAL_ERROR              (1 << 31)
#define EIP130_CRC24_BUSY               (1 << 8)
#define EIP130_WAIT_TIMEOUT			    (100000)
#define EIP130_RAM_SIZE                 (0x17800 - 0x4000)

#define COMMON_CK_RST_MUX0                      (COMMON_CK_RST_BASE + 0x0)
#define COMMON_CK_RST_MUX1                      (COMMON_CK_RST_BASE + 0x4)
#define COMMON_CK_RST_EB0                       (COMMON_CK_RST_BASE + 0x8)
#define COMMON_CK_RST_EB1                       (COMMON_CK_RST_BASE + 0xC)
#define COMMON_CK_RST_RST0                      (COMMON_CK_RST_BASE + 0x1C)
#define COMMON_CK_RST_CLK_MUX_1_SET             (COMMON_CK_RST_BASE + 0x30)
#define COMMON_CK_RST_CLK_MUX_1_CLR             (COMMON_CK_RST_BASE + 0x34)
#define COMMON_CK_RST_CLK_EB0_SET               (COMMON_CK_RST_BASE + 0x38)
#define COMMON_CK_RST_CLK_EB0_CLR               (COMMON_CK_RST_BASE + 0x3C)
#define COMMON_CK_RST_CLK_EB1_SET               (COMMON_CK_RST_BASE + 0x40)
#define COMMON_CK_RST_CLK_SW_RST_0_CLR          (COMMON_CK_RST_BASE + 0x64)

//#define CE_RESET_INIT
static int hw_img_hash[HASH_BLK_NUM];
#ifdef CE_RESET_INIT
static int eip130_mailbox_link(const u8 mailbox_nr)
{
	u32 set_val;
	u32 get_val;

	if (mailbox_nr < 1 || mailbox_nr > 8) {
		return SECURE_INTERNAL_FAIL;
	}

	set_val = 4 << ((mailbox_nr - 1) * 4);
	writel(set_val, (void *)EIP130_MAILBOX_CTRL);

	get_val = readl((void *)EIP130_MAILBOX_STAT);
	if ((get_val & set_val) != set_val) {
		return SECURE_INTERNAL_FAIL;
	}
	return SECURE_SUCCESS;
}

static int eip130_mailbox_can_write_token(int mailbox_nr)
{
	int val;
	u32 bit;
	if (mailbox_nr < 1 || mailbox_nr > 8) {
		return -1;
	}
	bit = 1 << ((mailbox_nr - 1) * 4);
	val = readl((void *)EIP130_MAILBOX_STAT);
	if ((val & bit) == 0) {
		return 1;
	}
	return 0;
}
#endif
static int eip130_mailbox_can_read_token(int mailbox_nr)
{
	int val;
	int bit;
	bit = 2 << ((mailbox_nr - 1) * 4);
	val = readl((void *)EIP130_MAILBOX_STAT);
	if ((val & bit) == 0) {
		return 0;
	}
	return 1;
}

static void eip130_write_u32_array(u64 addr, u32 *data, u32 cnt,int mailbox_nr)
{
	u32 *ptr = (u32 *)addr;
	int i;
	for (i = 0; i < cnt; i++) {
		ptr[i] = data[i];
	}
}

static void eip130_register_write_mailbox_control(u32 val,int mailbox_nr)
{
	writel(val,(void *)EIP130_MAILBOX_CTRL);
}

static int eip130_mailbox_write_and_submit_token(u32 *command_token_p,int mailbox_nr, int size)
{
	u32 mailbox_addr = EIP130_MAILBOX_IN_BASE;
	if (mailbox_nr < 1 || mailbox_nr > 8) {
		return -1;
	}

	mailbox_addr += (u32)(EIP130_MAILBOX_SPACING_BYTES * (mailbox_nr - 1));
	eip130_write_u32_array(mailbox_addr, command_token_p, size, mailbox_nr);
	eip130_register_write_mailbox_control(1, 1);
	return SECURE_SUCCESS;
}
#ifdef CE_RESET_INIT
static int eip130_register_write_mailbox_lockout(u32 val)
{
	writel(val, (void *)EIP130_MAILBOX_LOCKOUT);
	return SECURE_SUCCESS;
}
#endif

static int eip130_mailbox_read_token(u32 *result_token_p, int mailbox_nr)
{
	if (!eip130_mailbox_can_read_token(mailbox_nr)) {
		return -3;
	}
	eip130_write_u32_array((u64) result_token_p,
			     (u32 *)EIP130_MAILBOX_IN_BASE, 64,
			     mailbox_nr);
	eip130_register_write_mailbox_control(2, 1);

	return 0;
}

static int eip130_waitfor_result_token(int mailbox_nr)
{
	int i = 0;
	// Poll for output token available with sleep
	while (i < EIP130_WAIT_TIMEOUT) {
		if (eip130_mailbox_can_read_token(mailbox_nr)) {
			return SECURE_SUCCESS;
		}
		i++;
	}

	return SECURE_INTERNAL_FAIL;
}

static int eip130_physical_token_exchange(u32 *command_token_p,
				 unsigned int *result_token_p,
				 unsigned int mailbox_nr)
{
	int funcres;
	eip130_mailbox_write_and_submit_token(command_token_p, mailbox_nr, 64);
	do {
		// Wait for the result token to be available
		funcres = eip130_waitfor_result_token(mailbox_nr);
		if (funcres != 0) {
			return SECURE_INTERNAL_FAIL;
		}
		// Read the result token from the OUT mailbox
		funcres = eip130_mailbox_read_token(result_token_p, mailbox_nr);
		if (funcres != 0) {
			return SECURE_INTERNAL_FAIL;
		}
	} while ((command_token_p[0] & 0xffff) != (result_token_p[0] & 0xffff));
	return 0;
}

static u32 result_token[64];
static u32 hash_token[] = {
    0x02009e4d, 0x00000000, 0x00000004, 0x00100000,
    0x00000000, 0x00000004, 0x00000003, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000004, 0x00000000, 0x00000000, 0x00000000,
};

static int eip130_sha256(char *data, char *output, int size)
{
	char *ptr;
	int i;
	int ret;
	// unsigned short token_id;

	// token_id = (hash_token[0] & 0xFFFF) + 1;
	// hash_token[0] = (hash_token[0] & 0xFFFF0000) + token_id;
	hash_token[2] = size;
	hash_token[5] = size;
	hash_token[24] = size;
	hash_token[3] = (unsigned int)((unsigned long)data & 0xFFFFFFFF);
	hash_token[4] = (unsigned int)(((unsigned long)data >> 32) & 0xFFFFFFFF);
	hash_token[6] = 3;
	for (i = 0; i < 8; i++) {
        hash_token[8 + i] = 0;
    }
	ret = eip130_physical_token_exchange((unsigned int *)hash_token, (unsigned int *)result_token, 1);
	if(ret != 0) {
		return ret;
	}
	if(result_token[0] & (1 << 31)){
		return SECURE_HASH_COMPUTE_FAIL;
	}
	ptr = (char *)&result_token[2];
	for (i = 0; i < 32; i++) {
		output[i] = ptr[i];
	}
	return SECURE_SUCCESS;
}

#if 0
static int ax_eip130_sha256(char *data, char *output, int size)
{
    AX_CIPHER_HASH_CTL_S hashCtl;
    AX_CIPHER_HANDLE handle;
    AX_S32 ret;

	hashCtl.hashType = AX_CIPHER_ALGO_HASH_SHA256;
    hashCtl.hmacKey = 0;
    hashCtl.hmackeyLen = 0;
	ret = AX_CIPHER_HashInit(&hashCtl, &handle);
	if (ret != AX_CIPHER_SUCCESS) {
		printf("%s, Hash init failed\n", __func__);
		return -1;
	}
	ret = AX_CIPHER_HashFinal(handle, (unsigned char *)data, size, (unsigned char *)output);
	if (ret != AX_CIPHER_SUCCESS) {
		printf("%s, Hash final failed\n", __func__);
		return -1;
	}

	return 0;
}
#endif

static int eip130_sha256_init(char *data, char *output, int size)
{
	char *ptr;
	int i;
	int ret;
	hash_token[2] = size;
	hash_token[5] = size;
	hash_token[6] = 0x23;
	hash_token[24] = 0;
	hash_token[3] = (unsigned long) data & 0xFFFFFFFF;
	hash_token[4] = (unsigned int)(((unsigned long)data >> 32) & 0xFFFFFFFF);
	for (i = 0; i < 8; i++) {
		hash_token[8 + i] = 0;
	}
	ret = eip130_physical_token_exchange((unsigned int *)hash_token, (unsigned int *)result_token, 1);
	if (ret != 0) {
		return ret;
	}
	if (result_token[0] & (1 << 31)) {
		return SECURE_HASH_COMPUTE_FAIL;
	}
	ptr = (char *)&result_token[2];
	for (i = 0; i < 32; i++) {
		output[i] = ptr[i];
	}
	return SECURE_SUCCESS;
}

static int eip130_sha256_update(char *data, char *output, int size, int *digest)
{
	char *ptr;
	int i;
	int ret;
	hash_token[2] = size;
	hash_token[5] = size;
	hash_token[6] = 0x33;
	hash_token[24] = 0;
	hash_token[3] = (unsigned long)data & 0xFFFFFFFF;
	hash_token[4] = (unsigned int)(((unsigned long)data >> 32) & 0xFFFFFFFF);
	for (i = 0; i < 8; i++) {
		hash_token[8 + i] = digest[i];
	}
	ret = eip130_physical_token_exchange((unsigned int *)hash_token, (unsigned int *)result_token, 1);
	if (ret != 0) {
		return ret;
	}
	if (result_token[0] & (1 << 31)) {
		return SECURE_HASH_COMPUTE_FAIL;
	}
	ptr = (char *)&result_token[2];
	for (i = 0; i < 32; i++) {
		output[i] = ptr[i];
	}
	return SECURE_SUCCESS;
}

static int eip130_sha256_final(char *data, char *output, int size, int *digest, int all_size)
{
	char *ptr;
	int i;
	int ret;
	hash_token[2] = size;
	hash_token[5] = size;
	hash_token[6] = 0x13;
	hash_token[24] = all_size;
	hash_token[3] = (unsigned long)data & 0xFFFFFFFF;
	hash_token[4] = (unsigned int)(((unsigned long)data >> 32) & 0xFFFFFFFF);
	for (i = 0; i < 8; i++) {
		hash_token[8 + i] = digest[i];
	}
	ret = eip130_physical_token_exchange((unsigned int *)hash_token, (unsigned int *)result_token, 1);
	if (ret != 0) {
		return ret;
	}
	if (result_token[0] & (1 << 31)) {
		return SECURE_HASH_COMPUTE_FAIL;
	}
	ptr = (char *)&result_token[2];
	for (i = 0; i < 32; i++) {
		output[i] = ptr[i];
	}
	return SECURE_SUCCESS;
}

static int eip130_sha256_big_packet(char *data, char *output, int data_len)
{
	int ret;
	int blk_size = 0x100000;
	int blk_cnt = data_len / blk_size;
	int i;
	if ((data_len % blk_size) || (data_len == 0)) {
		blk_cnt ++;
	}
	if (blk_cnt == 1) {
		ret = eip130_sha256(data, output, data_len);
	} else {
		ret = eip130_sha256_init(data, output, blk_size);
		for (i = 1; i < (blk_cnt - 1); i++) {
			ret = eip130_sha256_update(data + i * blk_size, output, blk_size, (int *)output);
			if (ret < 0) {
				return SECURE_HASH_COMPUTE_FAIL;
			}
		}
		ret = eip130_sha256_final(data + i * blk_size, output, data_len - i * blk_size, (int *)output, data_len);
		if (ret < 0) {
			return SECURE_HASH_COMPUTE_FAIL;
		}
	}
	return SECURE_SUCCESS;
}

static u32 rsa_asset_creat[]= {
	0x170016fe ,0x00000000 ,0x80000004 ,0x00000200,
	0x0000010c ,0x00000000 ,0x00000000
};
static u32 load_asset_cmd[]={
	0x2704751f ,0x00000000 ,0x00015004 ,0x0800010c,
	0x0118bd70 ,0x00000000 ,0x00000000 ,0x00000000,
	0x00000000 ,0x00000000
};
static u32 verify_cmd[64]={
	0x1904357f ,0x00000000 ,0x01400009 ,0x40000020,
	0x00015004 ,0x00000000 ,0x00000000 ,0x01040000,
	0x00000000 ,0x00000000 ,0x00000000 ,0x00000000,
	0x00000000
};
static int eip_rsa_verify(char *key, char *signature, int *hash, int key_bits)
{
	int as_id;
	int ret = 0;
	int i;
	unsigned int result_token[64];
	if (key_bits == 2048) {
		rsa_asset_creat[4] = 0x0000010c;
	} else if (key_bits == 3072) {
		rsa_asset_creat[4] = 0x0000018c;
	}
	ret = eip130_physical_token_exchange((u32 *)rsa_asset_creat, result_token, 1);
	as_id = result_token[1];
	if (ret != 0) {
        printf("rsa asset create failed\n");
		return SECURE_RSA_COMPUTE_FAIL;
	}

	load_asset_cmd[2] = as_id;
	load_asset_cmd[4] = (u64)key & 0xffffffff;
	load_asset_cmd[5] = (u64)key >> 32;
    if (key_bits == 2048) {
		load_asset_cmd[3] = 0x0800010c;
	} else if (key_bits == 3072) {
		load_asset_cmd[3] = 0x0800018c;
	}
	ret = eip130_physical_token_exchange(load_asset_cmd, result_token, 1);
	if (ret != 0) {
        printf("load asset cmd failed\n");
		return SECURE_RSA_COMPUTE_FAIL;
	}

	verify_cmd[4] = as_id;
	verify_cmd[10] = (u64)signature & 0xffffffff;
	verify_cmd[11] = (u64)signature >> 32;
	for (i = 0; i < 8; i++) {
		verify_cmd[12 + i] = hash[i];
	}
    if (key_bits == 2048) {
		verify_cmd[2] = 0x01400009;
        verify_cmd[7] = 0x01040000;
	} else if (key_bits == 3072) {
		verify_cmd[2] = 0x01600009;
    	verify_cmd[7] = 0x01840000;
	}
	ret = eip130_physical_token_exchange(verify_cmd, result_token, 1);
	if (ret != 0) {
        printf("verify cmd failed\n");
		return SECURE_RSA_COMPUTE_FAIL;
	}
	if(result_token[0] & (1 << 31)){
        printf("verify error\n");
		return SECURE_IMG_VERIFY_FAIL;
	}

	return SECURE_SUCCESS;
}
#ifdef CE_RESET_INIT
static int eip130_init(u64 fw_addr, int size)
{
	int i;
	int ret;
	for (i = 1; i < 5; i++) {
		ret = eip130_mailbox_link(i);
		if (ret != 0) {
			return SECURE_INTERNAL_FAIL;
		}
	}
	ret = eip130_mailbox_can_write_token(1);
	if (ret != 1) {
		return SECURE_INTERNAL_FAIL;
	}
	ret = eip130_mailbox_link(1);
	if (ret != SECURE_SUCCESS) {
		return SECURE_INTERNAL_FAIL;
	}
	if (eip130_register_write_mailbox_lockout(0) != SECURE_SUCCESS) {
		return SECURE_INTERNAL_FAIL;
	}
	return SECURE_SUCCESS;
}
#endif
static int img_verify(struct rsa_key *key, char *img, char *sig, size_t size)
{
	int ret;
	unsigned long img_addr;
	unsigned long hash_addr;
	unsigned long key_addr;
	unsigned long sig_addr;
	img_addr = dma_map_single(img, size, DMA_TO_DEVICE);
	hash_addr = dma_map_single(hw_img_hash, sizeof(hw_img_hash), DMA_TO_DEVICE);
	ret = eip130_sha256((char *)img_addr, (char *)hash_addr, size);
	if (ret != 0) {
		return SECURE_HASH_COMPUTE_FAIL;
	}
	key_addr = dma_map_single(key, sizeof(*key), DMA_TO_DEVICE);
	sig_addr = dma_map_single(sig, 64 * 4, DMA_TO_DEVICE);
	ret = eip_rsa_verify((char *)key_addr, (char *)sig_addr,\
                            (int *)hw_img_hash, 2048);
	if (ret != SECURE_SUCCESS) {
		return SECURE_IMG_VERIFY_FAIL;
	}
	return SECURE_SUCCESS;
}
int is_secure_enable(void)
{
	int val;
	val = readl((void *)COMM_SYS_BOND_OPT0);
	if (val & SECURE_BOOT_EN) {
		return 1;
	}
	return 0;
}
static void ce_enable(void)
{
	/* clk_ce_cnt_eb, enalbe ce clks */
	writel(1UL << 0, COMMON_CK_RST_CLK_EB0_SET);
	/* clk_pub_ce_cnt_eb:[5], clk_pub_ce_eb:[6], pclk_pub_ce_eb:[30] */
	writel((3UL << 5) | (1UL << 30), COMMON_CK_RST_CLK_EB1_SET);
	/* release pub ce rst */
	writel(0x1FUL << 12, COMMON_CK_RST_CLK_SW_RST_0_CLR);
	/* sel clk_ce_bus_sel to 500M [25:24] 2'b10: 500M */
	writel(3UL << 24, COMMON_CK_RST_CLK_MUX_1_CLR);
	writel(2UL << 24, COMMON_CK_RST_CLK_MUX_1_SET);
 	/* sel pclk_pub_ce to 187.5M */
	writel(3UL << 12, COMMON_CK_RST_CLK_MUX_1_CLR);
	writel(1UL << 12, COMMON_CK_RST_CLK_MUX_1_SET);
	/* software control reset */
	writel(0x1FUL << 12, COMMON_CK_RST_CLK_SW_RST_0_CLR);
}
static void ce_disable(void)
{
	/*close ce clk*/
	writel(1UL << 0, COMMON_CK_RST_CLK_EB0_CLR);
}
SECURE_STAUS_E rsa_img_verify(struct rsa_key *key, char *img, char *sig, int size)
{
	int ret;
	if(is_secure_enable() == 0) {
		return SECURE_SUCCESS;
	}
	ret = img_verify(key,img,sig,size);
	if (ret != SECURE_SUCCESS) {
		return SECURE_IMG_VERIFY_FAIL;
	}
	return SECURE_SUCCESS;
}
int secure_verify(char *name, struct rsa_key *key, char *img, char *sig, int size)
{
	int ret;
	ce_enable();
	ret = rsa_img_verify(key, img, sig, size);
	ce_disable();
	if(ret != SECURE_SUCCESS) {
		pr_err("check failed %s, ret = %d\n", name, ret);
		return ret;
	}
	return 0;
}

static int calc_check_sum(int *data, int size)
{
	int count = size / 4;
	int i, sum = 0;
	for (i = 0; i < count; i++) {
		sum += *(data + i);
	}
	return sum;
}

/* HEADER_MAGIC: "AXERA!" */
int verify_image_header(struct secboot_image_header *img_hdr, int size)
{
	if (strncmp((char *)img_hdr->magic, "AXERA!", 6) != 0) {
		printf("%s: boot image header magic is error\n", __FUNCTION__);
		return -1;
	}
	int check_sum = calc_check_sum((int *)&img_hdr->capability, size - 4);
	if (check_sum != img_hdr->check_sum) {
		printf("image_header verify failed\n");
		return -1;
	}
	return 0;
}

static int secboot_rsa2048_hash[HASH_BLK_NUM] = {
	0x78ddfe9e, 0x698a9288, 0xccad1548, 0x5ca04574,
	0x9f1aa945, 0xf57eb822, 0xb807db67, 0x29140398
};

static int secboot_rsa3072_hash[HASH_BLK_NUM] = {
	0x6c1dda30, 0x53cd22c1, 0x97beb790, 0xaaeda709,
	0xef49ed74, 0xec82d080, 0x90175efb, 0x637dd4e5
};

int public_key_verify(char *key_addr, int size, int key_bits)
{
	int ret;
	int i;
	int hw_key_hash[HASH_BLK_NUM];
	int *secboot_hash_val ;

	if (key_bits == 2048) {
		secboot_hash_val = secboot_rsa2048_hash;
	} else if (key_bits == 3072) {
		secboot_hash_val = secboot_rsa3072_hash;
	} else {
		printf("secboot rsa key bits not supported\n");
        return SECURE_CIPHER_FAIL;
	}

	ret = eip130_sha256((char *)key_addr, (char *)hw_key_hash, size);
	if (ret != 0) {
		printf("eip130_sha256 failed\n");
		return SECURE_HASH_COMPUTE_FAIL;
	}
	for (i = 0; i < HASH_BLK_NUM; i++) {
		if (hw_key_hash[i] != secboot_hash_val[i]) {
			return SECURE_HASH_COMPUTE_FAIL;
		}
	}
	return SECURE_SUCCESS;
}

static unsigned int aes_token[] = {
	0x01009e4d, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

static int aes_decrypt(int *key, unsigned long src_addr, unsigned long dst_addr, int size)
{
	int ret;
	int i;
	aes_token[2] = size;
	aes_token[3] = (u32)(src_addr & 0xffffffff);
	aes_token[4] = (u32)((src_addr>>32) & 0xffffffff);
	aes_token[5] = size;
	aes_token[6] = dst_addr & 0xffffffff;
	aes_token[7] = (u32)((dst_addr>>32) & 0xffffffff);
	aes_token[8] = size;
	aes_token[11] = (3 << 16);//256bit
	for (i = 0; i < 8; i++) {
		aes_token[17 + i] = key[i];
	}
	ret = eip130_physical_token_exchange((u32 *)aes_token, (u32 *)result_token, 1);
	if (ret != 0) {
		return ret;
	}
	return 0;
}

static int get_aes_key(int *key)
{
	int i;
	int ret;
	ret = ax_efuse_init();
	if (ret != 0) {
		printf("efuse init failed\n");
		return SECURE_EFUSE_FAIL;
	}
	for(i = 0; i < 8; i++) {
		ret = ax_efuse_read(AESKEY_BLK_START + i, &key[i]);
		if (ret != SECURE_SUCCESS) {
			return SECURE_EFUSE_FAIL;
		}
	}
	return 0;
}

int cipher_sha256(char *data, char *output, int size)
{
	return eip130_sha256(data, output, size);
}

int cipher_rsa_verify(char *public_key, char *signature, int *hash, int key_bits)
{
	return eip_rsa_verify(public_key, signature, hash, key_bits);
}

int cipher_sha256_big_packet(char *data, char *output, int size)
{
	return eip130_sha256_big_packet(data, output, size);
}

int cipher_aes_ecb_decrypto(unsigned long src_addr, unsigned long dst_addr, int size)
{
	int ret;
	int blk_cnt, blk_size;
	void *data = NULL;
	char *src = (char *)src_addr;
	char *dst = (char *)dst_addr;
	int aes_key[8];
	ret = get_aes_key(aes_key);
	if (ret < 0) {
		printf("get aes key failed\n");
		return ret;
	}

	if (size > AX_CIPHER_CRYPTO_MAX_SIZE) {
		blk_size = AX_CIPHER_CRYPTO_MAX_SIZE;
	} else {
		blk_size = size;
	}
	blk_cnt = size / blk_size;
	data = malloc(blk_size);
	while(blk_cnt--) {
		memcpy(data, src, blk_size);
		flush_cache((unsigned long)src, blk_size);
		ret = aes_decrypt(aes_key, (unsigned long)src, (unsigned long)dst, blk_size);
		if (ret < 0) {
			printf("erro \n");
			free(data);
			return ret;
		}
		src += blk_size;
		dst += blk_size;
		size -= blk_size;
	}
	if (size) {
		memcpy(data, src, size);
		flush_cache((unsigned long)src, size);
		ret = aes_decrypt(aes_key, (unsigned long)src, (unsigned long)dst, size);
		if (ret < 0) {
			printf("erro \n");
			free(data);
			return ret;
		}
	}
	free(data);
	return ret;
}
