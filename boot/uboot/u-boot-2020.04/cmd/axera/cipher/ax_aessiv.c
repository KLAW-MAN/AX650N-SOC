#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ax_base_type.h"
#include "ax_cipher_api.h"
#include "ax_base_type.h"
#include "eip130_drv.h"
#include "ax_cipher_api.h"
//#include "ax_cipher_token_crypto.h"
#include "ax_aessiv.h"

static void AESSIV_Xor(AX_U8 * buffer, const AX_U8 * XorValue)
{
	int i;
	for (i = 0; i < (128 / 8); i++) {
		buffer[i] ^= XorValue[i];
	}
}

static void AESSIV_BitShiftLeft(AX_U8 * buffer)
{
	int i;
	for (i = 0; i < ((128 / 8) - 1); i++) {
		buffer[i] = (AX_U8) ((buffer[i] << 1) | ((buffer[i + 1] >> 7) & 1));
	}
	buffer[((128 / 8) - 1)] = (AX_U8) (buffer[((128 / 8) - 1)] << 1);
}

static void AESSIV_Dbl(AX_U8 * buffer)
{
	const AX_U8 AESSIV_XorBlock[(128 / 8)] = {
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x87
	};
	bool XorNeeded = ((buffer[0] >> 7) == 1);

	AESSIV_BitShiftLeft(buffer);
	if (XorNeeded) {
		AESSIV_Xor(buffer, AESSIV_XorBlock);
	}
}

static int AESSIV_S2V(AESSIV_Context * context, const AX_U8 * data, const size_t dataSize, AX_U8 * mac)
{
	AX_U8 zeroBlock[(128 / 8)] = {
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	};
	AX_U8 *dataBuf = NULL;
	size_t dataBufSize = 0;
	int status;
	unsigned int i;
	status = AX_CIPHER_EncryptMacBlk(context->Key, context->KeySize / 2, zeroBlock, sizeof(zeroBlock), mac, 16);
	if (status != AX_CIPHER_SUCCESS) {
		AX_CIPHER_ERR("AX_CIPHER_EncryptMacBlk failed, %x\n", status);
		return -1;
	}
	for (i = 0; i < context->ADListCount; i++) {
		AX_U8 localMac[(128 / 8)];
		int ADsize = 0;
		AESSIV_Dbl(mac);
		ADsize = (int)(context->ADList[i + 1] - context->ADList[i]);
		status = AX_CIPHER_EncryptMacBlk(context->Key, context->KeySize / 2, context->ADList[i], ADsize, localMac, 16);
		if (status != AX_CIPHER_SUCCESS) {
			AX_CIPHER_ERR("MAC FAIL\n");
			return -1;
		}
		AESSIV_Xor(mac, localMac);
	}
	if (dataSize >= (128 / 8)) {
		dataBufSize = dataSize;
		dataBuf = malloc(dataBufSize);
		if (dataBuf == NULL) {
			return -1;
		}

		memcpy(dataBuf, data, dataSize);
		AESSIV_Xor(&dataBuf[dataBufSize - 16], mac);
	} else {
		dataBufSize = (128 / 8);
		dataBuf = malloc(dataBufSize);
		if (dataBuf == NULL) {
			return -1;
		}

		AESSIV_Dbl(mac);
		memcpy(dataBuf, data, dataSize);
		dataBuf[dataSize] = 0x80;	// Pad
		for (i = (unsigned int)(dataSize + 1); i < (128 / 8); i++) {
			dataBuf[i] = 0x00;
		}
		AESSIV_Xor(dataBuf, mac);
	}
	status = AX_CIPHER_EncryptMacBlk(context->Key, context->KeySize / 2, dataBuf, dataBufSize, mac, 16);
	if (status != AX_CIPHER_SUCCESS) {
		free(dataBuf);
		return -1;
	}
	free(dataBuf);
	return 0;
}

static int AESSIV_AESCTR(AESSIV_Context * context, AX_U8 * IV, const AX_U8 * inData, const size_t inSize, AX_U8 * outData)
{
	int status;
	size_t InSize;
	AX_U8 tmpIV[(128 / 8)];
	AX_CIPHER_HANDLE handler;
	AX_CIPHER_CTRL_S ctrl;
	memcpy(tmpIV, IV, sizeof(tmpIV));
	tmpIV[8] &= 0x7F;
	tmpIV[12] &= 0x7F;
	ctrl.alg = AX_CIPHER_ALGO_CIPHER_AES;
	ctrl.workMode = AX_CIPHER_MODE_CIPHER_CTR;
	ctrl.pKey = (AX_U8 *) context->Key + (context->KeySize / 2);
	ctrl.keySize = context->KeySize / 2;
	ctrl.pIV = (AX_U8 *) tmpIV;
	status = AX_CIPHER_CreateHandle(&handler, &ctrl);
	if (status != AX_CIPHER_SUCCESS) {
		AX_CIPHER_ERR("AX_CIPHER_CreateHandle\n");
		return -1;
	}

	InSize = (inSize + ((128 / 8) - 1)) & (size_t) ~ ((128 / 8) - 1);
	if (InSize == inSize) {
		status = AX_CIPHER_Encrypt(handler, (AX_U8 *) inData, outData, InSize);
	} else {
		AX_U8 *inDataBuf;
		AX_U8 *outDataBuf;

		inDataBuf = malloc(InSize);
		if (inDataBuf == NULL) {
			AX_CIPHER_DestroyHandle(handler);
			return -1;
		}
		outDataBuf = malloc(InSize);
		if (outDataBuf == NULL) {
			free(inDataBuf);
			AX_CIPHER_DestroyHandle(handler);
			return -1;
		}

		memcpy(inDataBuf, inData, inSize);
		if ((InSize - inSize) > 0) {
			memset(&inDataBuf[inSize], 0, (InSize - inSize));
		}
		status = AX_CIPHER_Encrypt(handler, (AX_U8 *) inData, outData, InSize);
		if (status == AX_CIPHER_SUCCESS) {
			memcpy(outData, outDataBuf, inSize);
		}

		free(inDataBuf);
		free(outDataBuf);
	}
	if (status != AX_CIPHER_SUCCESS) {
		AX_CIPHER_DestroyHandle(handler);
		return -1;
	}
	AX_CIPHER_DestroyHandle(handler);
	return 0;
}

int AX_CIPHER_AESSIV_Init(AESSIV_Context * context, const bool Verbose)
{
	memset(context, 0, sizeof(AESSIV_Context));
	context->Verbose = Verbose;
	return 0;
}

int AX_CIPHER_AESSIV_SetKey(AESSIV_Context * context, const AX_U8 * key, const size_t KeySize)
{
	if (KeySize > sizeof(context->Key)) {
		return -1;
	}

	memcpy(context->Key, key, KeySize);
	context->KeySize = (unsigned int)KeySize;
	return 0;
}

int AX_CIPHER_AESSIV_SetAD(AESSIV_Context * context, const AX_U8 * AD, const size_t ADSize)
{
	AX_U8 *begin = NULL;

	if (context->ADListCount == 0) {
		if (ADSize > sizeof(context->ADBuffer)) {
			return -1;
		}

		begin = context->ADBuffer;
		context->ADList[context->ADListCount] = begin;
	} else {
		begin = context->ADList[context->ADListCount];
		if (ADSize > (sizeof(context->ADBuffer) - (unsigned int)(begin - context->ADBuffer))) {
			return -1;
		}
		if (context->ADListCount == ((sizeof(context->ADList) / sizeof(context->ADList[0])) - 1)) {
			return -1;
		}
	}
	memcpy(begin, AD, ADSize);
	context->ADListCount++;
	context->ADList[context->ADListCount] = begin + ADSize;

	return 0;
}

int AX_CIPHER_AESSIV_Encrypt(AESSIV_Context * context, const AX_U8 * inData, const size_t inSize, AX_U8 * outData, size_t * outSize)
{
	AX_U8 V[(128 / 8)];

	if ((inSize + (128 / 8)) > *outSize) {
		return -1;
	}

	if (AESSIV_S2V(context, inData, inSize, V) < 0) {
		return -1;
	}

	if (AESSIV_AESCTR(context, V, inData, inSize, &outData[(128 / 8)])) {
		return -1;
	}

	memcpy(outData, V, (128 / 8));
	*outSize = inSize + (128 / 8);
	return 0;
}

int AX_CIPHER_AESSIV_Decrypt(AESSIV_Context * context, const AX_U8 * inData, const size_t inSize, AX_U8 * outData, size_t * outSize)
{
	AX_U8 V[(128 / 8)];
	AX_U8 *C;
	AX_U8 *C1;
	*outSize = inSize - sizeof(V);
	C = malloc(inSize - sizeof(V));
	if (!C) {
		AX_CIPHER_ERR("AESSIV_Decrypt malloc C failed\n");
		return -1;
	}
	C1 = malloc(inSize);
	if (!C1) {
		free(C);
		AX_CIPHER_ERR("AESSIV_Decrypt malloc C1 failed\n");
		return -1;
	}
	memcpy(V, inData, sizeof(V));
	memcpy(C, inData + sizeof(V), *outSize);
	if (AESSIV_AESCTR(context, V, C, *outSize, C1) < 0) {
		free(C);
		free(C1);
		AX_CIPHER_ERR("inSize: %ld\n", (long int)inSize);
		return -1;
	}
	memcpy(outData, C1, *outSize);
	free(C);
	free(C1);
	return 0;
}
