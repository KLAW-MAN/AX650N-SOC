#ifndef __AX_AESSIV_H__
#define __AX_AESSIV_H__

typedef struct {
	AX_U8 Key[2 * (256 / 8)];
	AX_U32 KeySize;
	AX_U8 ADBuffer[512];
	AX_U8 *ADList[4];
	unsigned int ADListCount;
	bool Verbose;
} AESSIV_Context;

int AX_CIPHER_AESSIV_Init(AESSIV_Context * context, const bool Verbose);
int AX_CIPHER_AESSIV_SetKey(AESSIV_Context * context, const AX_U8 * key, const size_t keySize);
int AX_CIPHER_AESSIV_SetAD(AESSIV_Context * context, const AX_U8 * AD, const size_t ADSize);
int AX_CIPHER_AESSIV_Encrypt(AESSIV_Context * context, const AX_U8 * inData, const size_t inSize, AX_U8 * outData, size_t * outSize);
int AX_CIPHER_AESSIV_Decrypt(AESSIV_Context * context, const AX_U8 * inData, const size_t inSize, AX_U8 * outData, size_t * outSize);
#endif
