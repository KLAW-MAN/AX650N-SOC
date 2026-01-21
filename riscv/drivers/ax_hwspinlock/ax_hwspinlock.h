#ifndef __AX_HWSPINLOCK_H__
#define __AX_HWSPINLOCK_H__

#include <stdint.h>

#define AX_HWSPINLOCK_MASTER_ID_ARM0    0
#define AX_HWSPINLOCK_MASTER_ID_RISCV   4

#define AX_HWSPINLOCK_AREA_ID_GPIO  12

void ax_hwspinlock_lock(uint32_t area_id);
void ax_hwspinlock_unlock(uint32_t area_id);

#endif // __AX_HWSPINLOCK_H__

