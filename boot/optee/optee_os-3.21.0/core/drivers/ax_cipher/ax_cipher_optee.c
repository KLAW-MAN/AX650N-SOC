#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <initcall.h>
#include <io.h>
#include <kernel/spinlock.h>
#include <kernel/tee_time.h>
#include <mm/core_memprot.h>
#include <mm/core_mmu.h>
#include <platform_config.h>
#include <rng_support.h>
#include <trace.h>
#include <types_ext.h>
#include <util.h>
#include "ax_cipher_api.h"

TEE_Result hw_get_random_bytes(void *buf, size_t blen)
{
	uint32_t ret;

	ret = ax_cipher_get_random_number((uint8_t *)buf, (uint32_t)blen);
	if (ret != AX_CIPHER_SUCCESS) {
		EMSG("ax get random error:0x%08x", ret);
		return TEE_ERROR_GENERIC;
	}
	return TEE_SUCCESS;
}

static TEE_Result ax_cipher_init_optee(void)
{
	int ret;
	ret = ax_cipher_init();
	if (ret < 0) {
		EMSG("ax cipher initialized failed");
		return TEE_ERROR_SECURITY;
	}
	//ret = ax_cipher_huk_init();
	if (ret < 0) {
		EMSG("ax cipher hukinit failed");
		return TEE_ERROR_SECURITY;
	}
	return TEE_SUCCESS;
}

service_init(ax_cipher_init_optee);
