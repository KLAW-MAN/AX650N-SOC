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
#include <cpu_func.h>

#include "rtthread.h"



#define DDR_RAM_ADDR_START 	(0x136000000ULL)
#define DDR_RAM_SIZE         	(0x20000000UL)

#define RISCV_PC_ADDR_START	(0x40000000UL)

static void RegWrite(uint64_t addr, uint32_t val)
{
    *(volatile uint32_t*)addr = val;
}

static int _do_riscv_boot(uint64_t ADDR, phys_size_t LEN, phys_addr_t FW_ADDR, phys_size_t FW_LEN)
{

    if (FW_LEN > LEN) {
        printf("%s:%d	fw_len > len\n", __func__, __LINE__);
        return -1;
    }

    uint64_t ramBase = ADDR;
    phys_addr_t aonSysBase = 0x4200000UL;
    phys_addr_t commClkBase = 0x4210000UL;
    phys_addr_t remapBase = 0x4211000UL;

    for (int i = 0; i < (FW_LEN / 4); i++){
        RegWrite(ramBase + i * 4, *((u32*)FW_ADDR + i));
    }
    flush_dcache_all();

    // from mc50_riscv_e907_rtthread/01_code/trace32/start_verify_test_ax650.cmm
    printf("region_sa = 0x%lx\n", RISCV_PC_ADDR_START >> 12);
    printf("region_size = 0x%lx\n", DDR_RAM_SIZE/4/1024-1);
    printf("region_dst_sa = 0x%llx\n", DDR_RAM_ADDR_START >> 12);

    RegWrite(aonSysBase+0x254, RISCV_PC_ADDR_START);
    // set clk_riscv_bus = 500M
    RegWrite(commClkBase+0x28, 0x1UL << 26);
    RegWrite(commClkBase+0x2C, 0x1UL << 25);

    RegWrite(commClkBase+0x28, 0x1UL << 12); // clk_comm_wdt_sel = 1, 24M
    RegWrite(commClkBase+0x40, 0x0080UL);

    RegWrite(commClkBase+0x64, 0x40000UL);

    RegWrite(remapBase+0x00, RISCV_PC_ADDR_START >> 12);
    RegWrite(remapBase+0x04, DDR_RAM_SIZE/4/1024-1);
    RegWrite(remapBase+0x08, DDR_RAM_ADDR_START >> 12);
    RegWrite(remapBase+0x0C, 0x1);

    // use  ax_lookat 0x4210064 -s 0x20000 in linux console
//    RegWrite(commClkBase+0x64, 0x20000UL);
//    RegWrite(0x1E00000C, 0x1UL);

    return 0;
}

int do_riscv_boot(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
    int ret = 0;
    printf("start risc-v boot\n");

    ret = _do_riscv_boot(DDR_RAM_ADDR_START, DDR_RAM_SIZE, (phys_addr_t)rtthread_bin, rtthread_bin_len);

    if (ret == 0) {
        printf("riscv test SUCCESS");
    } else {
        printf("riscv test FAILED");
        return -1;
    }

    return 0;
}

int boot_riscv(void)
{
    printf("to boot risc-v\n");
    return _do_riscv_boot(DDR_RAM_ADDR_START, DDR_RAM_SIZE, (phys_addr_t)rtthread_bin, rtthread_bin_len);
}

U_BOOT_CMD(riscv_boot, 1, 0, do_riscv_boot,
		"risc-v boot", NULL);
