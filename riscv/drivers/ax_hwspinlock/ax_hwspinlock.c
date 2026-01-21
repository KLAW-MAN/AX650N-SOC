#include "soc.h"
#include "ax_hwspinlock.h"
#include <stdlib.h>
#include <rthw.h>
#include <rtdevice.h>

#define readl(addr)            (*(volatile unsigned int *)(addr))
#define writel(value, addr)    (*(volatile unsigned int *)(addr) = (value))

#define LOCK_ADDR_OFFSET(area_id)   ((AX_HWSPINLOCK_MASTER_ID_RISCV << 8) | (area_id << 3))
#define UNLOCK_ADDR_OFFSET(area_id) ((AX_HWSPINLOCK_MASTER_ID_RISCV << 8) | (area_id << 3) | 0x4)

#define HWSPINLOCK_UNLOCK_VALUE     ((AX_HWSPINLOCK_MASTER_ID_RISCV) << 28 | \
                                     (AX_HWSPINLOCK_MASTER_ID_RISCV) << 24 | \
                                     (AX_HWSPINLOCK_MASTER_ID_RISCV) << 20 | \
                                     (AX_HWSPINLOCK_MASTER_ID_RISCV) << 16 | \
                                     0xffff)

static void ax_hwspinlock_enable()
{
    uint32_t sw_rst_0 = readl(COMMON_RESET_CLK_BASE + SW_RST_0_OFFSET);
    uint32_t clk_eb_2 = readl(COMMON_RESET_CLK_BASE + CLK_EB_2_OFFSET);

    uint32_t hwspinlock_rst = sw_rst_0 & 0x8000000;
    uint32_t hwspinlock_eb = clk_eb_2 & 0x08;

    if (hwspinlock_rst != 0) {
        writel(0x8000000, COMMON_RESET_CLK_BASE + SW_RST_0_CLR_OFFSET);
    }
    if (hwspinlock_eb == 0) {
        writel(0x08, COMMON_RESET_CLK_BASE + CLK_EB_2_SET_OFFSET);
    }
}

void ax_hwspinlock_lock(uint32_t area_id)
{
    uint32_t lock_addr = MC50_HWSPINLOCK_BASE + LOCK_ADDR_OFFSET(area_id);

    while (1) {
        uint32_t value = readl(lock_addr);
        if (value == 0x00000000) {
            break;
        }
    }
}

void ax_hwspinlock_unlock(uint32_t area_id)
{
    uint32_t unlock_addr = MC50_HWSPINLOCK_BASE + UNLOCK_ADDR_OFFSET(area_id);

    writel(HWSPINLOCK_UNLOCK_VALUE, unlock_addr);
}

static int ax_hwspinlock_model_init(void)
{
    ax_hwspinlock_enable();
    return 0;
}

INIT_BOARD_EXPORT(ax_hwspinlock_model_init);
