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
#include "ax_base_type.h"
#include "ax_cipher_api.h"
#include "ax_aessiv.h"
#include "ax_keyotpblob.h"

static bool g_Verbose = true;
static AESSIV_Context g_AESSIVContext;

static int EIP130_KeyBlobWrap(KEY_OTP_BLOB_CONTEXT * keyBlobContext, AX_VOID * assetData, AX_S32 assetDataLen, AX_VOID * keyBlob, AX_S32 * keyBlobSize)
{
	int ret = -1;
	size_t OutputSize = assetDataLen + 128 / 8;
	char label[256];

	ret = AX_CIPHER_AESSIV_Init(&g_AESSIVContext, g_Verbose);
	if (ret != 0) {
		AX_CIPHER_ERR("KeyBlob setup failed (Init)\n");
		return -1;
	}

	ret = AX_CIPHER_AESSIV_SetKey(&g_AESSIVContext, keyBlobContext->key, keyBlobContext->keyLen);
	if (ret != 0) {
		AX_CIPHER_ERR("KeyBlob setup failed (Key)\n");
		return -1;
	}

	memcpy(label, keyBlobContext->label, keyBlobContext->labelSize);
	memcpy(label + keyBlobContext->labelSize, &keyBlobContext->policy, 8);
	ret = AX_CIPHER_AESSIV_SetAD(&g_AESSIVContext, (const AX_U8 *)label, keyBlobContext->labelSize + 8);
	if (ret != 0) {
		AX_CIPHER_ERR("KeyBlob setup failed (AD)\n");
		return -1;
	}

	ret = AX_CIPHER_AESSIV_Encrypt(&g_AESSIVContext, assetData, assetDataLen, keyBlob, &OutputSize);
	if (ret != 0) {
		AX_CIPHER_ERR("KeyBlob generation failed\n");
		return -1;
	}
	*keyBlobSize = OutputSize;
	return 0;
}

static int EIP130_KeyBlobUnwrap(KEY_OTP_BLOB_CONTEXT * keyBlobContext, AX_VOID * keyBlob, AX_S32 keyBlobSize, AX_VOID * assetData, AX_S32 * assetDataLen)
{
	int ret = -1;
	size_t OutputSize = keyBlobSize - 128 / 8;
	char label[256];
	if ((keyBlobContext->labelSize + 8) > (AX_S32) sizeof(label)) {
		AX_CIPHER_ERR("EIP130_KeyBlobUnwrap kblob size is too large\n");
		return -1;
	}
	ret = AX_CIPHER_AESSIV_Init(&g_AESSIVContext, g_Verbose);
	if (ret != 0) {
		AX_CIPHER_ERR("KeyBlob setup failed (Init)\n");
		return -1;
	}

	ret = AX_CIPHER_AESSIV_SetKey(&g_AESSIVContext, keyBlobContext->key, keyBlobContext->keyLen);
	if (ret != 0) {
		AX_CIPHER_ERR("KeyBlob setup failed (Key)\n");
		return -1;
	}
	memcpy(label, keyBlobContext->label, keyBlobContext->labelSize);
	memcpy(label + keyBlobContext->labelSize, &keyBlobContext->policy, 8);
	ret = AX_CIPHER_AESSIV_SetAD(&g_AESSIVContext, (const AX_U8 *)label, keyBlobContext->labelSize + 8);
	if (ret != 0) {
		AX_CIPHER_ERR("KeyBlob setup failed (AD)\n");
		return -1;
	}
	ret = AX_CIPHER_AESSIV_Decrypt(&g_AESSIVContext, keyBlob, keyBlobSize, assetData, &OutputSize);
	if (ret != 0) {
		AX_CIPHER_ERR("KeyBlob Decrypt failed\n");
		return -1;
	}
	*assetDataLen = OutputSize;
	return 0;
}

AX_S32 AX_CIPHER_KeyBlobWrap(KEY_OTP_BLOB_CONTEXT * keyBlobContext, AX_VOID * assetData, AX_S32 assetDataLen, AX_VOID * keyBlob, AX_S32 * keyBlobSize)
{
	if (!keyBlobContext->key || !keyBlobContext->label || !keyBlob || !assetData || !assetDataLen) {
		return -1;
	}
	EIP130_KeyBlobWrap(keyBlobContext, assetData, assetDataLen, keyBlob, keyBlobSize);
	return 0;
}

AX_S32 AX_CIPHER_KeyBlobUnwrap(KEY_OTP_BLOB_CONTEXT * keyBlobContext, AX_VOID * keyBlob, AX_S32 keyBlobSize, AX_VOID * assetData, AX_S32 * assetDataLen)
{
	if (!keyBlobContext->key || !keyBlobContext->label || !keyBlob || !assetData || !assetDataLen) {
		return -1;
	}
	EIP130_KeyBlobUnwrap(keyBlobContext, keyBlob, keyBlobSize, assetData, assetDataLen);
	return 0;
}
