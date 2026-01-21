#include <stdlib.h>
#include <rthw.h>
#include <rtdevice.h>
#include <stdlib.h>
#include "ax_hwspinlock.h"

static void print_usage()
{
    rt_kprintf("usage: hwspinlock_test\n"
               "    0: lock\n"
               "    1: unlock\n"
               "    2: lock/unlock with int disable\n\n");
}

static void hwspinlock_test(rt_int32_t argc, char* argv[])
{
    if (argc != 2) {
        print_usage();
        return;
    }

    rt_base_t level;
    int mode = atoi(argv[1]);
    switch (mode) {
    case 0:
        ax_hwspinlock_lock(AX_HWSPINLOCK_AREA_ID_GPIO);
        rt_kprintf("master id %u, area id %u, locked\n",
            AX_HWSPINLOCK_MASTER_ID_RISCV, AX_HWSPINLOCK_AREA_ID_GPIO);
        break;
    case 1:
        ax_hwspinlock_unlock(AX_HWSPINLOCK_AREA_ID_GPIO);
        rt_kprintf("master id %u, area id %u, unlocked\n",
            AX_HWSPINLOCK_MASTER_ID_RISCV, AX_HWSPINLOCK_AREA_ID_GPIO);
        break;
    case 2:
        level = rt_hw_interrupt_disable();
        ax_hwspinlock_lock(AX_HWSPINLOCK_AREA_ID_GPIO);
        rt_kprintf("master id %u, area id %u, locked and interrupt disbaled wait 10 seconds\n",
            AX_HWSPINLOCK_MASTER_ID_RISCV, AX_HWSPINLOCK_AREA_ID_GPIO);

        volatile uint32_t intl = 0;
        volatile uint32_t ext = 0;
        for (ext = 0; ext < 1; ext++) {
            for (intl = 0; intl < 0x3fffffff; intl++) {
            }
        }

        ax_hwspinlock_unlock(AX_HWSPINLOCK_AREA_ID_GPIO);
        rt_hw_interrupt_enable(level);
        rt_kprintf("master id %u, area id %u, unlocked\n",
            AX_HWSPINLOCK_MASTER_ID_RISCV, AX_HWSPINLOCK_AREA_ID_GPIO);
        break;
    default:
        print_usage();
    }
}

MSH_CMD_EXPORT(hwspinlock_test, lock or unlock [-h] to get help);
