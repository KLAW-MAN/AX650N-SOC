/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <common.h>
#include "ax_base_type.h"
#include "ax_cipher_api.h"

static int do_huk_test(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
	int ret;
	int i;
	int data0[128 / 32];
	int data1[128 / 32];
	if (AX_CIPHER_Init() < 0) {
		AX_CIPHER_ERR("AX_CIPHER_Init failed\n");
		return -1;
	}

	ret = AX_CIPHER_HukInit();
	if(ret < 0) {
		AX_CIPHER_ERR("AX_CIPHER_HukInit fail %x\n", ret);
		return -1;
	}
	memset(data0, 0, sizeof(data0));
	ret = AX_CIPHER_HukRead((AX_U8 *)data0, 16);
	if(ret < 0) {
		AX_CIPHER_ERR("HUK read fail %x\n", ret);
		return -1;
	}
	AX_CIPHER_ERR("first times read HUK val is:\n");
	AX_CIPHER_ERR("%x, %x, %x, %x\n", data0[0], data0[1], data0[2], data0[3]);
	for(i = 0; i < 1000; i++) {
		memset(data1, 0, sizeof(data1));
		ret = AX_CIPHER_HukRead((AX_U8 *)data1, 32);
		if(ret < 0) {
			AX_CIPHER_ERR("HUK read fail %x, times: %d\n", ret, i);
			return -1;
		}
		if(memcmp(data0, data1, sizeof(data1)) != 0) {
			AX_CIPHER_ERR("read compare fail:\n");
			AX_CIPHER_ERR("%x, %x, %x, %x\n", data1[0], data1[1], data1[2], data1[3]);
			return -1;
		}
	}
	AX_CIPHER_ERR("HUK Test PASS!\n");
	return 0;
}
U_BOOT_CMD(ax_huk_test, 1, 1, do_huk_test, "AX huk Test", "for example: ax_huk_test");
