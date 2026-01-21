/*
 * Copyright (c) 2020 AXERA
 *
 * SPDX-License-Identifier:     GPL-2.0+
 */

#ifndef _AX_EFUSE_API_H_
#define _AX_EFUSE_API_H_
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define EFSC_MAX_BLK 32
typedef enum {
	EFUSE_SUCCESS = 0,
	EFUSE_PARAM_FAIL = 0x80050001,
	EFUSE_INIT_FAIL = 0x80050002,
	EFUSE_READ_FAIL = 0x80050003,
	EFUSE_WRITE_FAIL = 0x80050004,
	EFUSE_MMAP_FAIL = 0x80050005,
} EFUSE_STAUS_E;

int ax_efuse_init(void);
int ax_efuse_deinit(void);
int ax_efuse_read(int blk, int *data);
int ax_efuse_write(int blk, int data);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif
