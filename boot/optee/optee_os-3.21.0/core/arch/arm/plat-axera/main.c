// SPDX-License-Identifier: BSD-2-Clause
/*
 *
 */

#include <console.h>
#include <drivers/gic.h>
#include <drivers/serial8250_uart.h>
#include <kernel/boot.h>
#include <kernel/interrupt.h>
#include <kernel/panic.h>
#include <mm/core_memprot.h>
#include <platform_config.h>
#include <trace.h>
#include <io.h>
#include <stdio.h>
#include <string.h>
#include <tee/entry_fast.h>
#include <tee/entry_std.h>
#include <kernel/tee_common_otp.h>

register_phys_mem_pgdir(MEM_AREA_IO_NSEC,
			ROUNDDOWN(CONSOLE_UART_BASE, CORE_MMU_PGDIR_SIZE),
			CORE_MMU_PGDIR_SIZE);

register_phys_mem_pgdir(MEM_AREA_IO_SEC,
			ROUNDDOWN(GIC_BASE, CORE_MMU_PGDIR_SIZE),
			CORE_MMU_PGDIR_SIZE);

register_phys_mem_pgdir(MEM_AREA_IO_SEC,
			ROUNDDOWN(GIC_BASE + GICD_OFFSET, CORE_MMU_PGDIR_SIZE),
			CORE_MMU_PGDIR_SIZE);

register_phys_mem_pgdir(MEM_AREA_IO_NSEC,
			ROUNDDOWN(EFUSE0_BASE, CORE_MMU_PGDIR_SIZE),
			CORE_MMU_PGDIR_SIZE);

static struct gic_data gic_data;
static struct serial8250_uart_data console_data;

void console_init(void)
{
	serial8250_uart_init(&console_data, CONSOLE_UART_BASE,
			     CONSOLE_UART_CLK_IN_HZ, CONSOLE_BAUDRATE);
	register_serial_console(&console_data.chip);
}

void main_init_gic(void)
{
	gic_init_base_addr(&gic_data, GIC_BASE + GICC_OFFSET,
			   GIC_BASE + GICD_OFFSET);

	itr_init(&gic_data.chip);
}

void itr_core_handler(void)
{
	gic_it_handle(&gic_data);
}
int tee_otp_get_die_id(uint8_t *buffer, size_t len)
{
	static uint64_t uid;
	size_t i;
	static uint64_t uid_read = 0;
	vaddr_t efuse0_base;
	uint64_t tmp;
	char *ptr = (char *)&uid;
	if(!uid_read) {
		efuse0_base = (vaddr_t)phys_to_virt(EFUSE0_BASE, MEM_AREA_IO_NSEC, EFUSE0_SZ);
		uid = io_read32(efuse0_base + EFSC_SHADOW_0);
		tmp = io_read32(efuse0_base + EFSC_SHADOW_0 + 4);
		tmp <<= 32;
		uid |= tmp;
		uid_read = 1;
	}
	for (i = 0; i < len; i++) {
		buffer[i] = ptr[i % 8];
	}

	return 0;
}

#ifdef CFG_HW_UNQ_KEY_REQUEST
#include <kernel/tee_common_otp.h>
extern int ax_cipher_huk_read(unsigned char * huk, unsigned int count);
TEE_Result tee_otp_get_hw_unique_key(struct tee_hw_unique_key *hwkey)
{
	TEE_Result res;
	int ret = 0;
	static uint8_t hw_unq_key[sizeof(hwkey->data)] __aligned(64);
	static int hw_unq_key_read = 0;
	if(hw_unq_key_read == 0) {
		ret = ax_cipher_huk_read(hw_unq_key, sizeof(hwkey->data));
	}
	if (ret < 0) {
		EMSG("\nUnique key is not fetched from the platform.");
		//memcpy(&hwkey->data[0], hw_unq_key, sizeof(hwkey->data));
		memset(&hwkey->data[0], 0, sizeof(hwkey->data));
		hw_unq_key_read = 1;
		res = TEE_SUCCESS;
	} else {
		memcpy(&hwkey->data[0], hw_unq_key, sizeof(hwkey->data));
		hw_unq_key_read = 1;
		res = TEE_SUCCESS;
	}
	return res;
}
#endif
