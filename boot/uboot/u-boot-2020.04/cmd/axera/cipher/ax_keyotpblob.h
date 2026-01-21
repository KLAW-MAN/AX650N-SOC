#ifndef __AX_KEYOTPBLOB_H__
#define __AX_KEYOTPBLOB_H__

typedef struct {
	AX_VOID *key;
	AX_VOID *label;
	AX_U64 policy;
	AX_S32 keyLen;
	AX_S32 labelSize;
} KEY_OTP_BLOB_CONTEXT;

AX_S32 AX_CIPHER_KeyBlobWrap(KEY_OTP_BLOB_CONTEXT * keyBlobContext, AX_VOID * assetData, AX_S32 assetDataLen, AX_VOID * keyBlob, AX_S32 * keyBlobSize);
AX_S32 AX_CIPHER_KeyBlobUnwrap(KEY_OTP_BLOB_CONTEXT * keyBlobContext, AX_VOID * keyBlob, AX_S32 keyBlobSize, AX_VOID * assetData, AX_S32 * assetDataLen);

#endif
