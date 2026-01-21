#include <rthw.h>
#include <rtthread.h>
#include <rtdevice.h>
#include <finsh.h>
#include "board.h"
#include "drv_mailbox.h"
#include "mc50_e907_interrupt.h"
#include "ax_log.h"
#include "ax_common.h"
#include "mailbox_cmd.h"

typedef struct {
    uint8_t id;
    void *data;
    mbox_callback_t callback;
    rt_list_t list;
} mbox_callback_info_t;

#define NUM_CHANS                    32

#define AX_MAILBOX_MASTER_ID_ARM0    0
#define AX_MAILBOX_MASTER_ID_RISCV   4

#define INT_STATUS_REG_OFFSET        ((0x3UL << 8) | (AX_MAILBOX_MASTER_ID_RISCV << 4) | (0x00UL << 2))
#define INT_CLEAR_REG_OFFSET         ((0x3UL << 8) | (AX_MAILBOX_MASTER_ID_RISCV << 4) | (0x01UL << 2))
#define REG_QUERY_REG_OFFSET         ((0x3UL << 8) | (AX_MAILBOX_MASTER_ID_RISCV << 4) | (0x02UL << 2))
#define REG_LOCK_REG_OFFSET          ((0x3UL << 8) | (AX_MAILBOX_MASTER_ID_RISCV << 4) | (0x03UL << 2))

#define INFO_REG_X(x)                ((0x1UL << 8) | ((x) << 2)) // G = 1
#define DATA_REG_X(x)                ((0x0UL << 8) | ((x) << 2)) // G = 0

#define RISCV_MAILBOX_TEST_MAGIC    "test_for_mailbox"

static rt_list_t callback_list_head;

__STATIC_INLINE void writel(unsigned int value, unsigned int addr)
{
    *(volatile uint32_t*)addr = value;
}

 __STATIC_INLINE uint32_t readl(unsigned int addr)
{
    return *(volatile uint32_t*)addr;
}

void mailbox_clk_enable(void)
{
    writel(1UL<<28, COMMON_RESET_CLK_BASE+CLK_EB_1_SET_OFFSET);
}

void mailbox_clk_disable(void)
{
    writel(1UL<<28, COMMON_RESET_CLK_BASE+CLK_EB_1_CLR_OFFSET);
}

void mailbox_reset(void)
{
    writel(1UL<<10, COMMON_RESET_CLK_BASE+SW_RST_0_SET_OFFSET);
    ax_mdelay(1);
    writel(1UL<<10, COMMON_RESET_CLK_BASE+SW_RST_0_CLR_OFFSET);
}

static int mbox_flush_cache(mbox_msg_t *msg, void *data)
{
    csi_dcache_clean();
    return 0;
}

static int mbox_test_callback(mbox_msg_t *msg, void *data)
{
    mbox_msg_t msg_fb;
    if (rt_strcmp((char *)msg->data, RISCV_MAILBOX_TEST_MAGIC) == 0) {
        msg_fb.id = AX_MBOX_CMD_FB_ARM_TEST_OK;
    } else {
        msg_fb.id = AX_MBOX_CMD_FB_ARM_TEST_FAIL;
        AX_LOG_ERROR("test data [%s] error", (char *)msg->data);
    }
    mbox_auto_send_message(&msg_fb);

    return 0;
}

static int mbox_handle_msg(mbox_msg_t *msg)
{
    mbox_callback_info_t *info = NULL;
    mbox_callback_info_t *pos;
    rt_list_for_each_entry(pos, &callback_list_head, list) {
        if (pos->id == msg->id) {
            info = pos;
            info->callback(msg, info->data);
            break;
        }
    }

    if (info == NULL) {
        AX_LOG_ERROR("callback id[0x%x] is not registered", msg->id);
        return -1;
    }

    return 0;
}

static void mailbox_irq_handler(int irqno, void *param)
{
    uint32_t status;

    status = readl(MC50_MAILBOX_BASE + INT_STATUS_REG_OFFSET);
    writel(status & 0xFUL, MC50_MAILBOX_BASE + INT_CLEAR_REG_OFFSET);

    uint32_t channel = (status >> 16) & 0x3FUL;
    uint32_t info = readl(MC50_MAILBOX_BASE + INFO_REG_X(channel));

    uint32_t send_id = (info >> 24) & 0xFUL;
    uint32_t size = info & 0xFFFFFFUL;

    static mbox_msg_t msg = {.id = 0xff};
    uint32_t *ptr = (uint32_t *)&msg;
    switch (send_id) {
    case AX_MAILBOX_MASTER_ID_ARM0:
        for(int i = 0; i < (size / 4); i++) {
            ptr[i] = readl(MC50_MAILBOX_BASE + DATA_REG_X(channel));
        }
        mbox_handle_msg(&msg);
        break;
    default:
        AX_LOG_ERROR("undefined master id %u", send_id);
    }
}

static int mbox_send_message(int channel, void *data, rt_size_t size)
{
    if (size != 32) {
        rt_kprintf("message size error, size[%u]\n", size);
        return -1;
    }
    for (int i = 0; i < (size / 4); i++) {
        writel(((uint32_t*)data)[i], MC50_MAILBOX_BASE + DATA_REG_X(channel));
    }

    uint32_t info = (AX_MAILBOX_MASTER_ID_ARM0 << 28) | (AX_MAILBOX_MASTER_ID_RISCV << 24) | size;
    writel(info, MC50_MAILBOX_BASE + INFO_REG_X(channel));

    return size;
}

static int mbox_request_channel(void)
{
    uint32_t channel = readl(MC50_MAILBOX_BASE + REG_QUERY_REG_OFFSET);
    if (channel == 0xFFFFFFFFUL) {
        return -1;
    }

    return channel;
}

int mbox_auto_send_message(mbox_msg_t *msg)
{
    if (msg == NULL) {
        AX_LOG_ERROR("params error");
        return -1;
    }

    int channel = mbox_request_channel();
    if (channel == -1) {
        AX_LOG_ERROR("request mailbox channel fail");
        return -1;
    }
    mbox_send_message(channel, msg, 32);

    return 0;
}

int mbox_register_callback(mbox_callback_t callback, void *data, uint8_t id)
{
    rt_base_t level = rt_spin_lock_irqsave("lock");
    mbox_callback_info_t *pos;
    rt_list_for_each_entry(pos, &callback_list_head, list) {
        if (pos->id == id) {
            rt_spin_unlock_irqrestore("lock", level);
            AX_LOG_ERROR("callback id %u has aready exist", id);
            return -1;
        }
    }

    mbox_callback_info_t *info = rt_malloc(sizeof(mbox_callback_info_t));
    info->callback = callback;
    info->data = data;
    info->id = id;
    rt_list_insert_before(&callback_list_head, &info->list);

    rt_spin_unlock_irqrestore("lock", level);
    return 0;
}

int mbox_unregister_callback(uint8_t id)
{
    mbox_callback_info_t *info = RT_NULL;
    mbox_callback_info_t *pos, *tmp;

    rt_base_t level = rt_spin_lock_irqsave("lock");
    rt_list_for_each_entry_safe(pos, tmp, &callback_list_head, list) {
        if (pos->id == id) {
            info = pos;
            rt_list_remove(&info->list);
            break;
        }
    }

    if (info != NULL) {
        rt_free(info);
    } else {
        rt_spin_unlock_irqrestore("lock", level);
        AX_LOG_ERROR("callback id %u is not exist", id);
        return -1;
    }

    rt_spin_unlock_irqrestore("lock", level);
    return 0;
}

static int mc50_mailbox_init(void)
{
    rt_list_init(&callback_list_head);

    mc50_e907_interrupt_install(INT_REQ_COMMON_MAIL4, mailbox_irq_handler, NULL, "mailbox");
    mc50_e907_interrupt_umask(INT_REQ_COMMON_MAIL4);

    int r =  mbox_register_callback(mbox_flush_cache, RT_NULL, AX_MBOX_CMD_FLUSH_CACHE);
    if (r != 0) {
        AX_LOG_ERROR("register mbox_flush_cache fail");
    }
    r = mbox_register_callback(mbox_test_callback, RT_NULL, AX_MBOX_CMD_TEST);
    if (r != 0) {
        AX_LOG_ERROR("register test callback fail");
    }

    return 0;
}

INIT_DEVICE_EXPORT(mc50_mailbox_init);
