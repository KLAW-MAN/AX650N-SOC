#ifndef _AX_SECUREBOOT_H_
#define _AX_SECUREBOOT_H_

/* img_header capbility bit field, follow the romcode */
#define MMC_BUSWIDTH_4				(1 << 0)
#define MMC_BUSWIDTH_8				(1 << 1)
#define SPI_NAND_BUSWIDTH_4			(1 << 2)
#define SPI_NOR_BUSWIDTH_4			(1 << 3)
#define SPI_SLV_BUSWIDTH_RX_TX_1	(1 << 4)
#define SPI_SLV_BUSWIDTH_4			(1 << 5)
#define SDCARD_BUSWIDTH_4			(1 << 6)
#define SDIO_BUSWIDTH_4				(1 << 7)
#define IMG_CIPHER_ENABLE			(1 << 8)
#define CPU_CLK_CONFIG				(1 << 9)
#define RSA_3072_MODE				(1 << 10)
#define IMG_BAK_ENABLE				(1 << 11)
#define FW_BAK_ENABLE				(1 << 12)
#define IMG_CHECK_ENABLE			(1 << 13) //to enable image checksum verify, if check fail, read the backup from flash
#define FW_CHECK_ENABLE				(1 << 14) //to enable FW image checksum verify, if check fail, read the backup from flash
#define NPU_OCM_USE 				(1 << 15)
#define CE_FW_CP_DMA_USE 			(1 << 16)
#define IMG_SIGN					(1 << 17)

#define PUBKEY_HASH_BLK_START   (14)
#define AESKEY_BLK_START        (22)
#define AX_CIPHER_CRYPTO_MAX_SIZE (0xFFF00)

typedef enum {
	SECURE_SUCCESS = 0,
	SECURE_HASH_COMPUTE_FAIL = -1,
	SECURE_RSA_COMPUTE_FAIL = -2,
	SECURE_IMG_VERIFY_FAIL = -3,
	SECURE_INTERNAL_FAIL = -4,
	SECURE_EFUSE_FAIL = -5,
	SECURE_CIPHER_FAIL = -6,
} SECURE_STAUS_E;

struct img_header{
	u32 check_sum;//header chksum
	u32 magic_data;//should be 0x55543322
	u32 capability;
	u32 uboot_size;
	u32 cipher_en;
	u32 key_len;
	u32 reserved0[5];
	u32 key_n_header;
	u32 rsa_key_n[96];
	u32 key_e_header;
	u32 rsa_key_e;
	u32 sig_header;
	u32 signature[96];
	u32 reserved1[49];
};
struct spl_header{
	u32 check_sum;//0x00
	u32 magic_data;//0x04, should be 0x55543322
	u32 capability;
	u32 img_size;
	u32 fw_size;
	u32 img_check_sum;
	u32 fw_check_sum;
	/*
	 * [1:0]: nand page_size, page_size = 2^(nand_nor_cfg[1:0] + 11), 2KB ~ 16KB
	 * [4:2]: nand block_size, block_size = 2^(nand_nor_cfg[4:2] + 16), 64KB ~ 8MB
	 * [6:5]: nand oob_size, oob_size = 2^(nand_nor_cfg[6:5] + 6), 64Bytes ~ 512Bytes
	 * [7]: nand bad block marker size, if nand_nor_cfg[7] is 0, the bad block marker
	 * is 1 byte, if nand_type[7] is 1, bad block marker is 2 bytes.
	 * [15: 8]: nor QE read status register cmd
	 * [23:16]: nor QE write status register cmd
	 * [31:24]: nor QE bit offset
	 */
	u32 nand_nor_cfg;
	u32 boot_bak_flash_addr;
	u32 fw_flash_addr;
	u32 fw_bak_flash_addr;
	u32 key_n_header;
	u32 rsa_key_n[96];
	u32 key_e_header;
	u32 rsa_key_e;
	u32 sig_header;
	u32 signature[96];
	u32 reserved[49];
};
struct rsa_key {
	u32 key_n_header;
	u32 rsa_key_n[96];
	u32 key_e_header;
	u32 rsa_key_e;
};
struct sig_struct {
    u32 sig_header;
    u32 signature[96];
};

#if 0
struct multi_image_info {
    u8 	magic[6];
    u32 image_size;     // image file actual size
    u8 	partition_num;
    u32 partition_size[32];
    u32 partition_offset[32];
    u8  hash_digest[32]; // image data hash
};
struct multi_image_header {
    u32 check_sum; // header check sum
    u32 capability;
    struct multi_image_info mul_img_info;
    struct rsa_key public_key;
    struct sig_struct img_signature;
};
#endif
struct secboot_image_header {
	u32 check_sum;
	u32 capability;
	u8 magic[6];
	u32 image_size;
	u8 hash_digest[32];
	struct rsa_key pub_key;
	struct sig_struct signature;
	u8 reserved[188];
};

int secure_verify(char *name, struct rsa_key *key, char *img, char *sig, int size);

int is_secure_enable(void);
int verify_image_header(struct secboot_image_header *img_hdr, int size);
int public_key_verify(char *key_addr, int size, int key_bits);
int cipher_sha256(char *data, char *output, int size);
int cipher_rsa_verify(char *public_key, char *signature, int *hash, int key_bits);
int cipher_sha256_big_packet(char *data, char *output, int size);
int cipher_aes_ecb_decrypto(unsigned long src_addr, unsigned long dst_addr, int size);

#endif

