#include <mm/core_mmu.h>
#include <mm/core_memprot.h>
#include <util.h>
#include <stdlib.h>
#include "ax_base_type.h"
#include "eip130_drv.h"
#include "ax_cipher_api.h"
#include "ax_eip130_fw.h"

#define BOND0_REG (0x550000C)
#define SECURE_BOOT_EN (1 << 26)
#define AP_SYS_BASE                  0x208000
#define HASH_BLK_NUM 8
#define AP_SYS_EB2_WAND              (AP_SYS_BASE + 0x38)
#define AP_SYS_EB2_WOR               (AP_SYS_BASE + 0x3c)
#define AP_SYS_CE_REG                (AP_SYS_BASE + 0xf4)

static unsigned long SEC_CLK_RST_BASE;
static unsigned long COMM_SYS_GLB_BASE;
static unsigned long COMMON_CK_RST_BASE;
static unsigned long addr_map_complete = 0;

#define SEC_CLK_RST_PHY_BASE    (0x400F000UL)
#define COMM_SYS_GLB_PHY_BASE	(0x4200000UL)	// aon_sys, chip_top_rf
#define COMMON_CK_RST_PHY_BASE	(0x4210000UL)	// aon_sys, comm_ck_rst_cfg, AON_CLK_RST

#define PUB_CE_BASE			(0x4540000UL)
#define SEC_CE_BASE			(0x4600000UL)
#define COMM_SYS_BOND_OPT0	(COMM_SYS_GLB_BASE + 0x20)
#define COMM_SYS_BOND_OPT1	(COMM_SYS_GLB_BASE + 0x24)

//#define USE_PUB_CE

#ifdef USE_PUB_CE
#define EIP130_REG_PHY_BASE             (PUB_CE_BASE)
#else
#define EIP130_REG_PHY_BASE				(SEC_CE_BASE)
#endif
static unsigned long EIP130_REG_BASE;

#define EIP130_MAILBOX_IN_BASE          ((unsigned long)EIP130_REG_BASE)
#define EIP130_MAILBOX_OUT_BASE         (0x0000)
#define EIP130_MAILBOX_SPACING_BYTES    (0x0400)	// Actual mailbox size independent
#define EIP130_FIRMWARE_RAM_BASE        (0x4000 + (unsigned long)EIP130_REG_BASE)
#define EIP130_MAILBOX_STAT             (0x3F00 + (unsigned long)EIP130_REG_BASE)
#define EIP130_MAILBOX_CTRL             (0x3F00 + (unsigned long)EIP130_REG_BASE)
#define EIP130_MAILBOX_LOCKOUT          (0x3F10 + (unsigned long)EIP130_REG_BASE)
#define EIP130_MODULE_STATUS            (0x3FE0 + (unsigned long)EIP130_REG_BASE)
#define EIP130_WAIT_TIMEOUT             (10000000)
#define EIP130_RAM_SIZE                 (0x1C000)
#define EIP130_CRC24_BUSY               (1 << 8)
#define EIP130_CRC24_OK                 (1 << 9)
#define EIP130_FIRMWARE_WRITTEN         (1 << 20)
#define EIP130_FIRMWARE_CHECKS_DONE     (1 << 22)
#define EIP130_FIRMWARE_ACCEPTED        (1 << 23)
#define EIP130_FATAL_ERROR              (1 << 31)
#define CRYPTO_OFFICER_ID               (0x4F5A3647)

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

#define SEC_CLK_RST_BASE_clk_eb_0		(SEC_CLK_RST_BASE + 0x00)
#define SEC_CLK_RST_BASE_sw_rst_0		(SEC_CLK_RST_BASE + 0x04)
#define SEC_CLK_RST_BASE_clk_eb_0_set		(SEC_CLK_RST_BASE + 0x08)
#define SEC_CLK_RST_BASE_clk_eb_0_clr		(SEC_CLK_RST_BASE + 0x0C)
#define SEC_CLK_RST_BASE_sw_rst_0_set		(SEC_CLK_RST_BASE + 0x10)
#define SEC_CLK_RST_BASE_sw_rst_0_clr		(SEC_CLK_RST_BASE + 0x14)

static int eip130_mailbox_link(const uint8_t mailbox_nr)
{
	uint32_t set_val;
	uint32_t get_val;

	if (mailbox_nr < 1 || mailbox_nr > 8) {
		return AX_CIPHER_OPERATION_FAILED;
	}

	set_val = 4 << ((mailbox_nr - 1) * 4);
	writel(set_val, (void *)EIP130_MAILBOX_CTRL);

	get_val = readl((const volatile void *)EIP130_MAILBOX_STAT);
	if ((get_val & set_val) != set_val) {
		return AX_CIPHER_OPERATION_FAILED;
	}
	return AX_CIPHER_SUCCESS;
}

static uint32_t eip130_read_modulestatus(void)
{
	return readl((const volatile void *)EIP130_MODULE_STATUS);
}

static void eip130_write_module_status(uint32_t value)
{
	writel(value, (volatile void *)EIP130_MODULE_STATUS);
}

static int eip130_mailbox_unlink(int mailbox_nr)
{
	uint32_t set_value;
	uint32_t get_value;

	if (mailbox_nr < 1 || mailbox_nr > 8) {
		return AX_CIPHER_OPERATION_FAILED;
	}
	set_value = 8 << ((mailbox_nr - 1) * 4);
	writel(set_value, (volatile void *)EIP130_MAILBOX_CTRL);
	get_value = readl((const volatile void *)EIP130_MAILBOX_STAT);
	set_value >>= 1;
	if ((get_value & set_value) != 0) {
		return AX_CIPHER_OPERATION_FAILED;
	}
	return 0;
}

static int eip130_mailbox_can_write_token(int mailbox_nr)
{
	int val;
	uint32_t bit;
	if (mailbox_nr < 1 || mailbox_nr > 8) {
		return -1;
	}
	bit = 1 << ((mailbox_nr - 1) * 4);
	val = readl((const volatile void *)EIP130_MAILBOX_STAT);
	if ((val & bit) == 0) {
		return 1;
	}
	return 0;
}

static int eip130_mailbox_can_read_token(int mailbox_nr)
{
	int val;
	int bit;
	bit = 2 << ((mailbox_nr - 1) * 4);
	val = readl((const volatile void *)EIP130_MAILBOX_STAT);
	if ((val & bit) == 0) {
		return 0;
	}
	return 1;
}

static void eip130_write_u32_array(uint64_t addr, uint32_t * data, uint32_t cnt)
{
	uint32_t *ptr = (uint32_t *) addr;
	uint32_t i;
	for (i = 0; i < cnt; i++) {
		ptr[i] = data[i];
	}
}

static void eip130_register_write_mailbox_control(uint32_t val)
{
	writel(val, (void *)EIP130_MAILBOX_CTRL);
}

static int eip130_mailbox_write_and_submit_token(uint32_t * commandToken, int mailbox_nr, int size)
{
	uint64_t mailboxAddr = EIP130_MAILBOX_IN_BASE;
	if (mailbox_nr < 1 || mailbox_nr > 8) {
		return -1;
	}

	mailboxAddr += (uint64_t) (EIP130_MAILBOX_SPACING_BYTES * (mailbox_nr - 1));
	eip130_write_u32_array(mailboxAddr, commandToken, size);
	eip130_register_write_mailbox_control(1);
	return AX_CIPHER_SUCCESS;
}

static int eip130_register_write_mailbox_lockout(uint32_t val)
{
	writel(val, (void *)EIP130_MAILBOX_LOCKOUT);
	return AX_CIPHER_SUCCESS;
}

static int eip130_mailbox_read_token(uint32_t * resultToken, int mailbox_nr)
{
	if (!eip130_mailbox_can_read_token(mailbox_nr)) {
		return -3;
	}

	eip130_write_u32_array((uint64_t) resultToken, (uint32_t *) EIP130_MAILBOX_IN_BASE, 64);
	eip130_register_write_mailbox_control(2);
	return 0;
}

static int eip130_waitfor_result_token(int mailbox_nr)
{
	int i = 0;
	// Poll for output token available with sleep
	while (i < EIP130_WAIT_TIMEOUT) {
		if (eip130_mailbox_can_read_token(mailbox_nr)) {
			return AX_CIPHER_SUCCESS;
		}
		i++;
	}
	return AX_CIPHER_OPERATION_FAILED;
}

int eip130_physical_token_exchange(Eip130Token_Command_t * commandToken, Eip130Token_Result_t * resultToken, uint32_t mailbox_nr)
{
	int funcres;
	// Set identity in token if not the Provision Random HUK token
	if ((commandToken->W[0] & (0xff << 24)) != (uint32_t) ((EIP130TOKEN_OPCODE_ASSETMANAGEMENT << 24) | (EIP130TOKEN_SUBCODE_PROVISIONRANDOMHUK << 28))) {
		commandToken->W[1] = CRYPTO_OFFICER_ID;
	}
	//PrintCommandData(commandToken);
	eip130_mailbox_write_and_submit_token(&commandToken->W[0], mailbox_nr, 64);
	do {
		// Wait for the result token to be available
		funcres = eip130_waitfor_result_token(mailbox_nr);
		if (funcres != 0) {
			ax_cipher_err("timeout\n");
			return AX_CIPHER_OPERATION_FAILED;
		}
		// Read the result token from the OUT mailbox
		funcres = eip130_mailbox_read_token(&resultToken->W[0], mailbox_nr);
		if (funcres != 0) {
			ax_cipher_err("read token fail\n");
			return AX_CIPHER_OPERATION_FAILED;
		}
	} while ((commandToken->W[0] & 0xffff) != (resultToken->W[0] & 0xffff));
	return 0;
}

static int32_t eip130_firmware_check(void)
{
	uint32_t value = 0;
	do {
		value = eip130_read_modulestatus();
	} while ((value & EIP130_CRC24_BUSY) != 0);
	if (((value & EIP130_CRC24_OK) == 0) || ((value & EIP130_FATAL_ERROR) != 0)) {
		return -3;
	}
	if ((value & EIP130_FIRMWARE_WRITTEN) == 0) {
		return 0;
	}
	// - Check if firmware checks are done & accepted
	if ((value & EIP130_FIRMWARE_CHECKS_DONE) == 0) {
		return 1;
	} else if ((value & EIP130_FIRMWARE_ACCEPTED) != 0) {
		return 2;
	}
	return 0;
}

static int eip130_firmware_load(uint64_t fw_addr, int size)
{
	int i;
	int value;
	int padding;
	int retries = 3;
	int rc;
	int nretries;
	uint32_t *padding_addr;
	int mailbox_nr = 1;
	for (; retries > 0; retries--) {
		rc = eip130_firmware_check();
		if (rc < 0) {
			return AX_CIPHER_INTERNAL_ERROR;
		}
		if (rc == 2) {
			return 0;
		}
		if (rc != 1) {
			rc = eip130_mailbox_write_and_submit_token((uint32_t *) fw_addr, mailbox_nr, 64);
			eip130_mailbox_unlink(mailbox_nr);
			if (rc < 0) {
				return AX_CIPHER_INTERNAL_ERROR;
			}
			eip130_write_u32_array(EIP130_FIRMWARE_RAM_BASE, (uint32_t *) (fw_addr + 256), (size - 256) / 4);
			padding = EIP130_RAM_SIZE - size - 256;
			padding = padding / 4;
			padding_addr = (uint32_t *) (EIP130_FIRMWARE_RAM_BASE + (unsigned long long)(size - 256));
			for (i = 0; i < padding; i++) {
				padding_addr[i] = 0;
			}
			eip130_write_module_status(EIP130_FIRMWARE_WRITTEN);
		}
		value = eip130_read_modulestatus();
		if (((value & EIP130_CRC24_OK) == 0) || ((value & EIP130_FATAL_ERROR) != 0)) {
			return AX_CIPHER_INTERNAL_ERROR;
		}
		if ((value & EIP130_FIRMWARE_WRITTEN) == 0) {
			return AX_CIPHER_INTERNAL_ERROR;
		}
		for (nretries = 0x7FFFFFF; nretries && ((value & EIP130_FIRMWARE_CHECKS_DONE) == 0); nretries--) {
			value = eip130_read_modulestatus();
		}
		if ((value & EIP130_FIRMWARE_CHECKS_DONE) == 0) {
			return AX_CIPHER_INTERNAL_ERROR;
		}
		if ((value & EIP130_FIRMWARE_ACCEPTED) == EIP130_FIRMWARE_ACCEPTED) {
			return 0;
		}
	}
	return AX_CIPHER_INTERNAL_ERROR;
}

static int eip130_init(uint64_t fw_addr, int size)
{
	int32_t i;
	int32_t ret;
	for (i = 1; i < 5; i++) {
		ret = eip130_mailbox_link(i);
		if (ret != 0) {
			return AX_CIPHER_INTERNAL_ERROR;
		}
	}
	ret = eip130_mailbox_can_write_token(1);
	if (ret != 1) {
		return AX_CIPHER_OPERATION_FAILED;
	}
	ret = eip130_firmware_load(fw_addr, size);
	if (ret != 0) {
		return ret;
	}
	ret = eip130_mailbox_link(1);
	if (ret != AX_CIPHER_SUCCESS) {
		return AX_CIPHER_INTERNAL_ERROR;
	}
	if (eip130_register_write_mailbox_lockout(0) != AX_CIPHER_SUCCESS) {
		return AX_CIPHER_OPERATION_FAILED;
	}
	return AX_CIPHER_SUCCESS;
}

static void ce_reset_deassert(void)
{
	// 5: sec_ce_sw_rst
	// 4: sec_ce_sw_prst
	// 3: sec_ce_soft_sw_rst
	// 2: sec_ce_main_sw_rst
	// 1: sec_ce_cnt_sw_rst
	writel((1UL << 2), SEC_CLK_RST_BASE_sw_rst_0_clr);
	writel((1UL << 1) | (1UL << 3) | (1UL << 4) | (1UL << 5), SEC_CLK_RST_BASE_sw_rst_0_clr);
}
static void ce_reset_assert(void)
{
	volatile int i;
	// 5: sec_ce_sw_rst
	// 4: sec_ce_sw_prst
	// 3: sec_ce_soft_sw_rst
	// 2: sec_ce_main_sw_rst
	// 1: sec_ce_cnt_sw_rst
	for(i = 0; i < 20; i++);
	writel((1UL << 2), SEC_CLK_RST_BASE_sw_rst_0_set);
	for(i = 0; i < 20; i++);
	writel((1UL << 1) | (1UL << 3) | (1UL << 4) | (1UL << 5), SEC_CLK_RST_BASE_sw_rst_0_set);
	for(i = 0; i < 20; i++);
}
static void ce_clk_enable(void)
{
	// 0: clk_ce_cnt_eb
	writel((1UL << 0), COMMON_CK_RST_CLK_EB0_SET);

	// 25:24 clk_ce_bus_sel
	writel((1UL << 25), COMMON_CK_RST_CLK_MUX_1_SET);

	// 8: pclk_sec_ce_eb
	// 1: clk_sec_ce_eb
	// 0: clk_sec_ce_cnt_eb
	writel((1UL << 8), SEC_CLK_RST_BASE_clk_eb_0_set);
	writel((1UL << 1) | (1UL << 0), SEC_CLK_RST_BASE_clk_eb_0_set);
}

void ce_disable(void)
{
	/*close ce clk */
	writel(1UL << 0, COMMON_CK_RST_CLK_EB0_CLR);
}
static int ce_addr_map(void)
{
	if(addr_map_complete)
		return 0;
	if (!core_mmu_add_mapping(MEM_AREA_IO_SEC, COMMON_CK_RST_PHY_BASE, 0x1000)) {
		ax_cipher_err("ce init map failed %lx\n", COMMON_CK_RST_PHY_BASE);
		return -1;
	}
	COMMON_CK_RST_BASE = (vaddr_t) phys_to_virt(COMMON_CK_RST_PHY_BASE, MEM_AREA_IO_SEC, 0x1000);
	if (!COMMON_CK_RST_BASE) {
		ax_cipher_err("phys_to_virt failed %lx\n", COMMON_CK_RST_PHY_BASE);
		return -1;
	}
	if (!core_mmu_add_mapping(MEM_AREA_IO_SEC, EIP130_REG_PHY_BASE, 0x1000)) {
		ax_cipher_err("ce init map failed %lx\n", EIP130_REG_PHY_BASE);
		return -1;
	}
	EIP130_REG_BASE = (vaddr_t) phys_to_virt(EIP130_REG_PHY_BASE, MEM_AREA_IO_SEC, 0x1000);
	if (!EIP130_REG_BASE) {
		ax_cipher_err("phys_to_virt failed %lx\n", EIP130_REG_PHY_BASE);
		return -1;
	}

	if (!core_mmu_add_mapping(MEM_AREA_IO_SEC, SEC_CLK_RST_PHY_BASE, 0x100000)) {
		ax_cipher_err("ce init map failed %lx\n", EIP130_REG_PHY_BASE);
		return -1;
	}
	SEC_CLK_RST_BASE = (vaddr_t) phys_to_virt(SEC_CLK_RST_PHY_BASE, MEM_AREA_IO_SEC, 0x100000);
	if (!SEC_CLK_RST_BASE) {
		ax_cipher_err("phys_to_virt failed %lx\n", SEC_CLK_RST_PHY_BASE);
		return -1;
	}
	if (!core_mmu_add_mapping(MEM_AREA_IO_SEC, COMM_SYS_GLB_PHY_BASE, 0x1000)) {
		ax_cipher_err("ce init map failed %lx\n", EIP130_REG_PHY_BASE);
		return -1;
	}
	COMM_SYS_GLB_BASE = (vaddr_t) phys_to_virt(COMM_SYS_GLB_PHY_BASE, MEM_AREA_IO_SEC, 0x1000);
	if (!COMM_SYS_GLB_BASE) {
		ax_cipher_err("phys_to_virt failed %lx\n", COMM_SYS_GLB_PHY_BASE);
		return -1;
	}
	ax_cipher_info("SEC_CE Init COMMON_CK_RST_BASE: %lx, EIP130_REG_BASE: %lx,SEC_CLK_RST_BASE: %lx, COMM_SYS_GLB_BASE: %lx\n", COMMON_CK_RST_BASE, EIP130_REG_BASE, SEC_CLK_RST_BASE,
	       COMM_SYS_GLB_BASE);
	addr_map_complete = 1;
	return 0;
}

int ce_init(void)
{
	int ret;
	ret = ce_addr_map();
	if (ret < 0) {
		ax_cipher_err("ce_init failed\n");
		return -1;
	}

	ce_clk_enable();
	ce_reset_assert();
	ce_reset_deassert();
	ret = eip130_init((uint64_t) eip130_firmware, sizeof(eip130_firmware));
	if (ret != AX_CIPHER_SUCCESS) {
		return -1;
	}
	return 0;
}
int ce_reinit(void)
{
	int ret;
	ce_reset_assert();
	ce_reset_deassert();
	ret = eip130_init((uint64_t) eip130_firmware, sizeof(eip130_firmware));
	if (ret != AX_CIPHER_SUCCESS) {
		return -1;
	}
	return 0;
}
