/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _AX_PRINTK_H
#define _AX_PRINTK_H

#include <linux/stdarg.h>
#include <linux/init.h>
#include <linux/kern_levels.h>
#include <linux/linkage.h>
#include <linux/cache.h>

enum{
    AX_KERN_EMERG	= 0,
    AX_KERN_ALERT	= 1,
    AX_KERN_CRIT	= 2,
    AX_KERN_ERR     = 3,
    AX_KERN_WARNING	= 4,
    AX_KERN_NOTICE	= 5,
    AX_KERN_INFO	= 6,
    AX_KERN_DEBUG	= 7,
};

#define AX_MESSAGE_LOGLEVEL_DEFAULT CONFIG_MESSAGE_LOGLEVEL_DEFAULT

#define AX_CONSOLE_LOGLEVEL_MIN  1
#define AX_CONSOLE_LOGLEVEL_DEFAULT CONFIG_CONSOLE_LOGLEVEL_DEFAULT

#define AX_DEVKMSG_STR_MAX_SIZE 10
extern char ax_devkmsg_log_str[];
struct ctl_table;

struct dev_printk_info;

asmlinkage __printf(5, 0)
int ax_vprintk_emit(const int id, const char* tag, const int level,
                    const struct dev_printk_info *dev_info,
                    const char *fmt, va_list args);

asmlinkage __printf(4, 0)
int ax_vprintk(const int id, const char* tag, const int level, const char *fmt, va_list args);

asmlinkage __printf(4, 5) __cold
int _ax_printk(const int id, const char* tag, const int level, const char *fmt, ...);


__printf(1, 2) __cold int _ax_printk_deferred(const char *fmt, ...);

extern void __ax_printk_safe_enter(void);
extern void __ax_printk_safe_exit(void);

#define ax_printk_deferred_enter __ax_printk_safe_enter
#define ax_printk_deferred_exit __ax_printk_safe_exit

extern int __ax_printk_ratelimit(const char *func);
#define ax_printk_ratelimit() __ax_printk_ratelimit(__func__)
extern bool ax_printk_timed_ratelimit(unsigned long *caller_jiffies,
                                      unsigned int interval_msec);

extern int ax_printk_delay_msec;
extern int ax_dmesg_restrict;

extern int
ax_devkmsg_sysctl_set_loglvl(struct ctl_table *table, int write, void *buf,
                             size_t *lenp, loff_t *ppos);

extern void ax_wake_up_klogd(void);

char *ax_log_buf_addr_get(void);
u32 ax_log_buf_len_get(void);
void __init ax_setup_log_buf(int early);
void ax_printk_trigger_flush(void);
int do_axsyslog(int type, char __user *buf, int len, int source);

#ifdef CONFIG_SMP
extern int __ax_printk_cpu_trylock(void);
extern void __ax_printk_wait_on_cpu_lock(void);
extern void __ax_printk_cpu_unlock(void);

#define ax_printk_cpu_lock_irqsave(flags)       \
    for (;;) {              \
        local_irq_save(flags);      \
        if (__ax_printk_cpu_trylock())  \
            break;          \
        local_irq_restore(flags);   \
        __ax_printk_wait_on_cpu_lock(); \
    }

#define ax_printk_cpu_unlock_irqrestore(flags)  \
    do {                    \
        __ax_printk_cpu_unlock();       \
        local_irq_restore(flags);   \
    } while (0)             \

#else

#define ax_printk_cpu_lock_irqsave(flags) ((void)flags)
#define ax_printk_cpu_unlock_irqrestore(flags) ((void)flags)

#endif /* CONFIG_SMP */

#ifndef pr_fmt
    #define pr_fmt(fmt) fmt
#endif

#define ax_printk_index_wrap(_p_func, id, tag, level, _fmt, ...)                \
    ({                              \
        _p_func(id, tag, level, _fmt, ##__VA_ARGS__);               \
    })

#define ax_printk(id, tag, level, fmt, ...) ax_printk_index_wrap(_ax_printk, id, tag, level, fmt, ##__VA_ARGS__)

#define ax_pr_emerg(id, tag, fmt, ...) \
    ax_printk(id, tag, AX_KERN_EMERG, pr_fmt(fmt), ##__VA_ARGS__)
#define ax_pr_alert(id, tag, fmt, ...) \
    ax_printk(id, tag, AX_KERN_ALERT, pr_fmt(fmt), ##__VA_ARGS__)
#define ax_pr_crit(id, tag, fmt, ...) \
    ax_printk(id, tag, AX_KERN_CRIT, pr_fmt(fmt), ##__VA_ARGS__)
#define ax_pr_err(id, tag, fmt, ...) \
    ax_printk(id, tag, AX_KERN_ERR, pr_fmt(fmt), ##__VA_ARGS__)
#define ax_pr_warn(id, tag, fmt, ...) \
    ax_printk(id, tag, AX_KERN_WARNING, pr_fmt(fmt), ##__VA_ARGS__)
#define ax_pr_notice(id, tag, fmt, ...) \
    ax_printk(id, tag, AX_KERN_NOTICE, pr_fmt(fmt), ##__VA_ARGS__)
#define ax_pr_info(id, tag, fmt, ...) \
    ax_printk(id, tag, AX_KERN_INFO, pr_fmt(fmt), ##__VA_ARGS__)
#define ax_pr_debug(id, tag, fmt, ...) \
    ax_printk(id, tag, AX_KERN_DEBUG, pr_fmt(fmt), ##__VA_ARGS__)

#define ax_printk_default ax_pr_warn

extern const struct file_operations ax_kmsg_fops;

#endif
