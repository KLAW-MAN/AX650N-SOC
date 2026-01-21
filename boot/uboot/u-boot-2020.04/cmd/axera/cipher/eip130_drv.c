/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#include <common.h>
#include <malloc.h>
#include <mtd.h>
#include <blk.h>
#include <asm/io.h>
#include <memalign.h>
#include <linux/sizes.h>
#include <linux/string.h>
#include <dm/uclass.h>
#include <dm/device.h>
#include <linux/dma-mapping.h>
#include "ax_base_type.h"
#include "eip130_drv.h"
#include "ax_cipher_api.h"
#include "eip130_fw.h"
#define BOND0_REG (0x550000C)
#define SECURE_BOOT_EN (1 << 26)
#define AP_SYS_BASE                  0x208000
#define HASH_BLK_NUM 8
#define AP_SYS_EB2_WAND              (AP_SYS_BASE + 0x38)
#define AP_SYS_EB2_WOR               (AP_SYS_BASE + 0x3c)
#define AP_SYS_CE_REG                (AP_SYS_BASE + 0xf4)

#define SEC_CLK_RST_BASE    (0x400F000)
#define COMM_SYS_GLB_BASE	(0x4200000)     // aon_sys, chip_top_rf
#define COMMON_CK_RST_BASE	(0x4210000)     // aon_sys, comm_ck_rst_cfg, AON_CLK_RST
#define PUB_CE_BASE			(0x4540000)
#define SEC_CE_BASE			(0x4600000)
#define COMM_SYS_BOND_OPT0	(COMM_SYS_GLB_BASE + 0x20)
#define COMM_SYS_BOND_OPT1	(COMM_SYS_GLB_BASE + 0x24)

#define EIP130_MAILBOX_IN_BASE          (EIP130_REG_BASE)
#define EIP130_MAILBOX_OUT_BASE         (0x0000)
#define EIP130_MAILBOX_SPACING_BYTES    (0x0400)  // Actual mailbox size independent
#define EIP130_FIRMWARE_RAM_BASE        (0x4000 + EIP130_REG_BASE)
#define EIP130_MAILBOX_STAT             (0x3F00 + EIP130_REG_BASE)
#define EIP130_MAILBOX_CTRL             (0x3F00 + EIP130_REG_BASE)
#define EIP130_MAILBOX_LOCKOUT          (0x3F10 + EIP130_REG_BASE)
#define EIP130_MODULE_STATUS            (0x3FE0 + EIP130_REG_BASE)
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
static AX_U64 EIP130_REG_BASE = SEC_CE_BASE;

static int Eip130MailboxLink(const u8 mailboxNr)
{
    AX_U32 setVal;
    AX_U32 getVal;

    if (mailboxNr < 1 || mailboxNr > 8) {
        return AX_CIPHER_OPERATION_FAILED;
    }

    setVal = 4 << ((mailboxNr - 1) * 4);
    writel(setVal, (void *)EIP130_MAILBOX_CTRL);

    getVal = readl((const volatile void *)EIP130_MAILBOX_STAT);
    if ((getVal & setVal) != setVal) {
        return AX_CIPHER_OPERATION_FAILED;
    }
    return AX_CIPHER_SUCCESS;
}
static AX_U32 Eip130ReadModulestatus(void)
{
    return readl((const volatile void *)EIP130_MODULE_STATUS);
}
static void Eip130WriteModuleStatus(AX_U32 value)
{
    writel(value, (volatile void *)EIP130_MODULE_STATUS);
}
static int Eip130MailboxUnlink(int mailboxNr)
{
    AX_U32 setValue;
    AX_U32 getValue;

    if (mailboxNr < 1 || mailboxNr > 8) {
        return AX_CIPHER_OPERATION_FAILED;
    }
    setValue = 8 << ((mailboxNr - 1) * 4);
    writel(setValue, (volatile void *)EIP130_MAILBOX_CTRL);
    getValue = readl((const volatile void *)EIP130_MAILBOX_STAT);
    setValue >>= 1;
    if ((getValue & setValue) != 0) {
        return AX_CIPHER_OPERATION_FAILED;
    }
    return 0;
}
static int Eip130MailboxCanWriteToken(int mailboxNr)
{
    int val;
    AX_U32 bit;
    if (mailboxNr < 1 || mailboxNr > 8) {
        return -1;
    }
    bit = 1 << ((mailboxNr - 1) * 4);
    val = readl((const volatile void *)EIP130_MAILBOX_STAT);
    if ((val & bit) == 0) {
        return 1;
    }
    return 0;
}
static int Eip130MailboxCanReadToken(int mailboxNr)
{
    int val;
    int bit;
    bit = 2 << ((mailboxNr - 1) * 4);
    val = readl((const volatile void *)EIP130_MAILBOX_STAT);
    if ((val & bit) == 0) {
        return 0;
    }
    return 1;
}

static void Eip130WriteU32Array(u64 addr, AX_U32 *data, AX_U32 cnt, int mailboxNr)
{
    AX_U32 *ptr = (AX_U32 *)addr;
    int i;
    for (i = 0; i < cnt; i++) {
        ptr[i] = data[i];
    }
}

static void Eip130RegisterWriteMailboxControl(AX_U32 val, int mailboxNr)
{
    writel(val, (void *)EIP130_MAILBOX_CTRL);
}

static int Eip130MailboxWriteAndSubmitToken(AX_U32 *commandToken, int mailboxNr, int size)
{
    AX_U32 mailboxAddr = EIP130_MAILBOX_IN_BASE;
    if (mailboxNr < 1 || mailboxNr > 8) {
        return -1;
    }

    mailboxAddr += (AX_U32)(EIP130_MAILBOX_SPACING_BYTES * (mailboxNr - 1));
    Eip130WriteU32Array(mailboxAddr, commandToken, size, mailboxNr);
    Eip130RegisterWriteMailboxControl(1, 1);
    return AX_CIPHER_SUCCESS;
}
static int Eip130RegisterWriteMailboxLockout(AX_U32 val)
{
    writel(val, (void *)EIP130_MAILBOX_LOCKOUT);
    return AX_CIPHER_SUCCESS;
}

static int Eip130MailboxReadToken(AX_U32 *resultToken, int mailboxNr)
{
    if (!Eip130MailboxCanReadToken(mailboxNr)) {
        return -3;
    }
    Eip130WriteU32Array((u64) resultToken, (AX_U32 *)EIP130_MAILBOX_IN_BASE, 64, mailboxNr);
    Eip130RegisterWriteMailboxControl(2, 1);
    return 0;
}

int Eip130WaitforResultToken(int mailboxNr)
{
    int i = 0;
    // Poll for output token available with sleep
    while (i < EIP130_WAIT_TIMEOUT) {
        if (Eip130MailboxCanReadToken(mailboxNr)) {
            return AX_CIPHER_SUCCESS;
        }
        i++;
    }
    return AX_CIPHER_OPERATION_FAILED;
}
int Eip130PhysicalTokenExchange(AX_U32 *commandToken,
                                AX_U32 *resultToken,
                                AX_U32 mailboxNr)
{
    int funcres;
    // Set identity in token if not the Provision Random HUK token
    if ((commandToken[0] & (0xff << 24)) !=
        (AX_U32)((EIP130TOKEN_OPCODE_ASSETMANAGEMENT     << 24) |
                 (EIP130TOKEN_SUBCODE_PROVISIONRANDOMHUK << 28))) {
        commandToken[1] = CRYPTO_OFFICER_ID;
    }
    //PrintCommandData(commandToken);
    Eip130MailboxWriteAndSubmitToken(commandToken, mailboxNr, 64);
    do {
        // Wait for the result token to be available
        funcres = Eip130WaitforResultToken(mailboxNr);
        if (funcres != 0) {
            printf("timeout\n");
            return AX_CIPHER_OPERATION_FAILED;
        }
        // Read the result token from the OUT mailbox
        funcres = Eip130MailboxReadToken(resultToken, mailboxNr);
        if (funcres != 0) {
            printf("read token fail\n");
            return AX_CIPHER_OPERATION_FAILED;
        }
    } while ((commandToken[0] & 0xffff) != (resultToken[0] & 0xffff));
    return 0;
}
static AX_S32 EIP130_FirmwareCheck(void)
{
    AX_U32 value = 0;
    do {
        value = Eip130ReadModulestatus();
    } while ((value & EIP130_CRC24_BUSY) != 0);
    if (((value & EIP130_CRC24_OK) == 0) ||
        ((value & EIP130_FATAL_ERROR) != 0)) {
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
static int Eip130Firmwareload(u64 fw_addr, int size)
{
    int i;
    int value;
    int padding;
    int retries = 3;
    int rc;
    int nretries;
    AX_U32 *padding_addr;
    int mailboxNr = 1;
    for (; retries > 0; retries--) {
        rc = EIP130_FirmwareCheck();
        if (rc < 0) {
            return AX_CIPHER_INTERNAL_ERROR;
        }
        if (rc == 2) {
            return 0;
        }
        if (rc != 1) {
            rc = Eip130MailboxWriteAndSubmitToken((AX_U32 *)fw_addr, mailboxNr, 64);
            Eip130MailboxUnlink(mailboxNr);
            if (rc < 0) {
                return AX_CIPHER_INTERNAL_ERROR;
            }
            Eip130WriteU32Array(EIP130_FIRMWARE_RAM_BASE, (AX_U32 *)(fw_addr + 256), (size - 256) / 4, 1);
            padding = EIP130_RAM_SIZE - size - 256;
            padding = padding / 4;
            padding_addr = (AX_U32 *)(EIP130_FIRMWARE_RAM_BASE + (unsigned long long)(size - 256));
            for (i = 0; i < padding; i++) {
                padding_addr[i] = 0;
            }
            Eip130WriteModuleStatus(EIP130_FIRMWARE_WRITTEN);
        }
        value = Eip130ReadModulestatus();
        if (((value & EIP130_CRC24_OK) == 0) ||
            ((value & EIP130_FATAL_ERROR) != 0)) {
            return AX_CIPHER_INTERNAL_ERROR;
        }
        if ((value & EIP130_FIRMWARE_WRITTEN) == 0) {
            return AX_CIPHER_INTERNAL_ERROR;
        }
        for (nretries = 0x7FFFFFF; nretries && ((value & EIP130_FIRMWARE_CHECKS_DONE) == 0); nretries--) {
            value = Eip130ReadModulestatus();
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

static int Eip130Init(AX_ADDR fw_addr, int size)
{
    AX_S32 i;
    AX_S32 ret;
    for (i = 1; i < 5; i++) {
        ret = Eip130MailboxLink(i);
        if (ret != 0) {
            return AX_CIPHER_INTERNAL_ERROR;
        }
    }
    ret = Eip130MailboxCanWriteToken(1);
    if (ret != 1) {
        return AX_CIPHER_OPERATION_FAILED;
    }
    ret = Eip130Firmwareload(fw_addr, size);
    if (ret != 0) {
        return ret;
    }
    ret = Eip130MailboxLink(1);
    if (ret != AX_CIPHER_SUCCESS) {
        return AX_CIPHER_INTERNAL_ERROR;
    }
    if (Eip130RegisterWriteMailboxLockout(0) != AX_CIPHER_SUCCESS) {
        return AX_CIPHER_OPERATION_FAILED;
    }
    return AX_CIPHER_SUCCESS;
}

static void CeEnable(void)
{
    //PUB_CE init
    /* clk_ce_cnt_eb, enalbe ce clks */
    writel(1UL << 0, COMMON_CK_RST_CLK_EB0_SET);
    /* clk_pub_ce_cnt_eb:[5], clk_pub_ce_eb:[6], pclk_pub_ce_eb:[30] */
    writel((3UL << 5) | (1UL << 30), COMMON_CK_RST_CLK_EB1_SET);
    /* sel clk_ce_bus_sel to 500M [25:24] 2'b10: 500M */
    writel(3UL << 24, COMMON_CK_RST_CLK_MUX_1_CLR);
    writel(2UL << 24, COMMON_CK_RST_CLK_MUX_1_SET);
    /* sel pclk_pub_ce to 187.5M */
    writel(3UL << 12, COMMON_CK_RST_CLK_MUX_1_CLR);
    writel(1UL << 12, COMMON_CK_RST_CLK_MUX_1_SET);
    /* software control reset */
    writel(0x1UL << 13, COMMON_CK_RST_CLK_SW_RST_0_CLR);
    writel(0x1DUL << 12, COMMON_CK_RST_CLK_SW_RST_0_CLR);
    //SEC_CE init
    // 0: clk_ce_cnt_eb
    writel((1UL << 0), COMMON_CK_RST_CLK_EB0_SET);

    // 25:24 clk_ce_bus_sel
    writel((1UL << 25), COMMON_CK_RST_CLK_MUX_1_SET);

    // 8: pclk_sec_ce_eb
    // 1: clk_sec_ce_eb
    // 0: clk_sec_ce_cnt_eb
    writel((1UL << 8), SEC_CLK_RST_BASE_clk_eb_0_set);
    writel((1UL << 1) | (1UL << 0), SEC_CLK_RST_BASE_clk_eb_0_set);


    // 5: sec_ce_sw_rst
    // 4: sec_ce_sw_prst
    // 3: sec_ce_soft_sw_rst
    // 2: sec_ce_main_sw_rst
    // 1: sec_ce_cnt_sw_rst

    writel((1UL << 2), SEC_CLK_RST_BASE_sw_rst_0_clr);
    writel((1UL << 1) | (1UL << 3) | (1UL << 4) | (1UL << 5), SEC_CLK_RST_BASE_sw_rst_0_clr);
}
void CeDisable(void)
{
    /*close ce clk*/
    writel(1UL << 0, COMMON_CK_RST_CLK_EB0_CLR);
}
int CeInit(void)
{
    int ret;
    CeEnable();
    EIP130_REG_BASE = PUB_CE_BASE;
    ret = Eip130Init((AX_ADDR)eip130_firmware, sizeof(eip130_firmware));
    if (ret != AX_CIPHER_SUCCESS) {
        return -1;
    }
    EIP130_REG_BASE = SEC_CE_BASE;
    ret = Eip130Init((AX_ADDR)eip130_firmware, sizeof(eip130_firmware));
    if (ret != AX_CIPHER_SUCCESS) {
        return -1;
    }
    return 0;
}
int CeMailboxUnlink(void)
{
    EIP130_REG_BASE = PUB_CE_BASE;
    for (int i = 1; i < 5; i++) {
        int ret = Eip130MailboxUnlink(i);
        if (ret != 0) {
            return AX_CIPHER_INTERNAL_ERROR;
        }
    }
    EIP130_REG_BASE = SEC_CE_BASE;
    for (int i = 1; i < 5; i++) {
        int ret = Eip130MailboxUnlink(i);
        if (ret != 0) {
            return AX_CIPHER_INTERNAL_ERROR;
        }
    }
    return AX_CIPHER_SUCCESS;
}

