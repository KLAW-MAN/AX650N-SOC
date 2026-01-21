/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#ifndef __OSAL_AX__H__
#define __OSAL_AX__H__

#ifdef __cplusplus
extern "C" {
#endif

#include "osal_type_ax.h"
#include <linux/timer.h>
#include <linux/uaccess.h>
#include <linux/ax_hrtimer.h>
//#include <linux/poll.h>
#include <linux/version.h>
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4, 16, 0))
#include <linux/iversion.h>
#endif

#define OSAL_VERSION_AX "0.50"

#define AX_THREAD_SHOULD_STOP 1
#define AX_TASK_RUNNING			0x0000
#define AX_TASK_INTERRUPTIBLE		0x0001
#define AX_TASK_UNINTERRUPTIBLE		0x0002


#define AX_VERIFY_READ VERIFY_READ
#define AX_VERIFY_WRITE VERIFY_WRITE

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 15, 48))
#define AX_OSAL_USER_access_ok(type, addr, size) access_ok(type, addr, size)
#define AX_OSAL_USER__put_user(x, ptr) __put_user(x, ptr)
#define AX_OSAL_USER__get_user(x, ptr) __get_user(x, ptr)
#endif


//task api
typedef AX_S32(*AX_THREAD_FUNC_T)(AX_VOID *data);

typedef struct AX_TASK {
    AX_VOID *task_struct;
} AX_TASK_T;

struct AX_CURRENT_TASK {
	pid_t pid;
	pid_t tgid;
	void *mm;
	char comm[16];
};

typedef struct AX_TASK_SCHED_PARAM {
    AX_S32 sched_priority;
} AX_TASK_SCHED_PARAM_T;

AX_TASK_T *AX_OSAL_TASK_kthread_run(AX_THREAD_FUNC_T thread, AX_VOID *data, AX_S8 *name);
AX_TASK_T *AX_OSAL_TASK_kthread_create_ex(AX_THREAD_FUNC_T thread, AX_VOID *data, AX_S8 *name, AX_S32 prioirty);
AX_S32 AX_OSAL_TASK_kthread_stop(AX_TASK_T *task, AX_U32 stop_flag);
AX_S32 AX_OSAL_TASK_cond_resched(AX_VOID);
AX_BOOL AX_OSAL_TASK_kthread_should_stop(AX_VOID);

AX_VOID AX_OSAL_set_current_state(AX_S32 state_value);
AX_VOID AX_OSAL_get_current(struct AX_CURRENT_TASK *ax_task);

AX_S32 AX_OSAL_sched_setscheduler(AX_TASK_T *task, AX_S32 policy,const struct AX_TASK_SCHED_PARAM *param);
AX_VOID AX_OSAL_TASK_schedule(void);

//semaphore api
typedef struct AX_SEMAPHORE {
    AX_VOID *sem;
} AX_SEMAPHORE_T;

AX_S32 AX_OSAL_SYNC_sema_init(AX_SEMAPHORE_T *sem, AX_S32 val);
//only for linux kernel
AX_S32 AX_OSAL_SYNC_sema_down_interruptible(AX_SEMAPHORE_T *sem);
AX_S32 AX_OSAL_SYNC_sema_down(AX_SEMAPHORE_T *sem);
AX_S32 AX_OSAL_SYNC_sema_down_timeout(AX_SEMAPHORE_T *sem, AX_LONG timeout);
AX_S32 AX_OSAL_SYNC_sema_down_trylock(AX_SEMAPHORE_T *sem);
AX_VOID AX_OSAL_SYNC_sema_up(AX_SEMAPHORE_T *sem);
AX_VOID AX_OSAL_SYNC_sema_destroy(AX_SEMAPHORE_T *sem);

//mutex api
typedef struct AX_MUTEX {
    AX_VOID *mutex;
} AX_MUTEX_T;

#ifndef CONFIG_DEBUG_MUTEXES
AX_S32 AX_OSAL_SYNC_mutex_init(AX_MUTEX_T *mutex);
#else
AX_VOID *AX_OSAL_DBG_mutex_init(AX_MUTEX_T * mutex);
#define AX_OSAL_SYNC_mutex_init(mutex)                     \
          ({                                               \
		int ret = 0;                               \
		void *p = AX_OSAL_DBG_mutex_init(mutex);   \
		if (p != NULL)                             \
			mutex_init(p);                     \
		else                                       \
			ret = -1;                          \
		ret;                                       \
         })
#endif

AX_S32 AX_OSAL_SYNC_mutex_lock(AX_MUTEX_T *mutex);
//only for linux kernel
AX_S32 AX_OSAL_SYNC_mutex_lock_interruptible(AX_MUTEX_T *mutex);
AX_S32 AX_OSAL_SYNC_mutex_trylock(AX_MUTEX_T *mutex);
AX_VOID AX_OSAL_SYNC_mutex_unlock(AX_MUTEX_T *mutex);
AX_VOID AX_OSAL_SYNC_mutex_destroy(AX_MUTEX_T *mutex);
#define DEFINE_AX_OSAL_SYNC_MUTEX(mutex_name) \
	DEFINE_MUTEX(osal##mutex_name); \
	AX_MUTEX_T mutex_name = { \
			.mutex = &osal##mutex_name, \
		}

typedef struct AX_RT_MUTEX {
    void *rt_mutex;
} AX_RT_MUTEX_T;

#ifdef CONFIG_DEBUG_RT_MUTEXES
int AX_OSAL_DBG_rt_mutex_init(AX_RT_MUTEX_T * rt_mutex,struct lock_class_key *__key);
# define AX_OSAL_SYNC_rt_mutex_init(rt_mutex)                    \
({                                                               \
        int __ret = 0;                                           \
        static struct lock_class_key __key;                      \
        __ret = AX_OSAL_DBG_rt_mutex_init(rt_mutex,&__key);      \
        if(__ret < 0)                                            \
                __ret = __ret;                                   \
        __ret;                                                   \
})

#else
int AX_OSAL_SYNC_rt_mutex_init(AX_RT_MUTEX_T * mutex);
#endif

void AX_OSAL_SYNC_rt_mutex_lock(AX_RT_MUTEX_T * mutex);
void AX_OSAL_SYNC_rt_mutex_unlock(AX_RT_MUTEX_T * mutex);
void AX_OSAL_SYNC_rt_mutex_destroy(AX_RT_MUTEX_T * mutex);


//spin lock api
typedef struct AX_SPINLOCK {
    AX_VOID *lock;
} AX_SPINLOCK_T;


#ifndef CONFIG_DEBUG_SPINLOCK
AX_S32 AX_OSAL_SYNC_spin_lock_init(AX_SPINLOCK_T *lock);
#else
AX_VOID *AX_OSAL_DBG_spin_lock_init(AX_SPINLOCK_T *lock);

#define AX_OSAL_SYNC_spin_lock_init(lock)                     \
	({                                                    \
		int ret = 0;                                  \
		void *p = AX_OSAL_DBG_spin_lock_init(lock);   \
		if(p != NULL)                                 \
			spin_lock_init(p);                    \
		else                                          \
			ret = -1;                             \
		ret;                                          \
	})
#endif

AX_VOID AX_OSAL_SYNC_spin_lock(AX_SPINLOCK_T *lock);
AX_S32 AX_OSAL_SYNC_spin_trylock(AX_SPINLOCK_T *lock);
AX_VOID AX_OSAL_SYNC_spin_unlock(AX_SPINLOCK_T *lock);
AX_VOID AX_OSAL_SYNC_spin_lock_irqsave(AX_SPINLOCK_T *lock, AX_U32 *flags);
AX_VOID AX_OSAL_SYNC_spin_unlock_irqrestore(AX_SPINLOCK_T *lock, AX_U32 *flags);
AX_VOID AX_OSAL_SYNC_spinLock_destory(AX_SPINLOCK_T *lock);

#define DEFINE_AX_OSAL_SYNC_SPINLOCK(lock_name) \
	DEFINE_SPINLOCK(osal##lock_name); \
	AX_SPINLOCK_T lock_name = { \
		   .lock = &osal##lock_name};

//atomic api
typedef struct AX_ATOMIC {
    AX_VOID *atomic;
} AX_ATOMIC_T;

AX_S32 AX_OSAL_SYNC_atomic_init(AX_ATOMIC_T *atomic);
AX_S32 AX_OSAL_SYNC_atomic_read(AX_ATOMIC_T *atomic);
AX_VOID AX_OSAL_SYNC_atomic_set(AX_ATOMIC_T *atomic, AX_S32 val);
AX_S32 AX_OSAL_SYNC_atomic_inc_return(AX_ATOMIC_T *atomic);
AX_S32 AX_OSAL_SYNC_atomic_dec_return(AX_ATOMIC_T *atomic);
AX_S32 AX_OSAL_SYNC_atomic_cmpxchg(AX_ATOMIC_T *atomic, AX_S32 old, AX_S32 new);
AX_BOOL AX_OSAL_SYNC_atomic_try_cmpxchg(AX_ATOMIC_T * atomic, AX_S32 *old, AX_S32 new);
AX_S32 AX_OSAL_SYNC_atomic_fetch_add_ge(AX_ATOMIC_T * atomic, AX_S32 add, AX_S32 used);
AX_VOID AX_OSAL_SYNC_atomic_destroy(AX_ATOMIC_T *atomic);


//barrier api
AX_VOID AX_OSAL_SYNC_mb(AX_VOID);
AX_VOID AX_OSAL_SYNC_rmb(AX_VOID);
AX_VOID AX_OSAL_SYNC_wmb(AX_VOID);
AX_VOID AX_OSAL_SYNC_isb(AX_VOID);
AX_VOID AX_OSAL_SYNC_dsb(AX_VOID);
AX_VOID AX_OSAL_SYNC_dmb(AX_VOID);


//workqueue api
typedef struct AX_WORK {
    AX_VOID *work;
    AX_VOID(*func)(struct AX_WORK *work);
} AX_WORK_T;
typedef AX_VOID(*AX_WORK_FUNC_T)(AX_WORK_T *work);

AX_S32 AX_OSAL_SYNC_init_work(AX_WORK_T *work, AX_WORK_FUNC_T func);
AX_S32 AX_OSAL_SYNC_schedule_work(AX_WORK_T *work);
AX_VOID AX_OSAL_SYNC_destroy_work(AX_WORK_T *work);

AX_S32 AX_OSAL_SYNC_init_delayed_work(AX_WORK_T *work, AX_WORK_FUNC_T func);
AX_S32 AX_OSAL_SYNC_schedule_delayed_work(AX_WORK_T *work, AX_ULONG delay);
AX_VOID AX_OSAL_SYNC_destroy_delayed_work(AX_WORK_T *work);
AX_S32 AX_OSAL_SYNC_cancel_delayed_work(AX_WORK_T *osal_work);
AX_S32 AX_OSAL_SYNC_cancel_delayed_work_sync(AX_WORK_T *osal_work);

//waitqueue api
#define AX_OSAL_SYNC_INTERRUPTIBLE  0
#define AX_OSAL_SYNC_UNINTERRUPTIBLE    1

#define DEFINE_AX_OSAL_SYNC_WAIT_QUEUE_HEAD(wq_name) \
	DECLARE_WAIT_QUEUE_HEAD(osal##wq_name); \
	AX_WAIT_T wq_name = { \
		   .wait = &osal##wq_name};
typedef AX_S32(*AX_WAIT_COND_FUNC_T)(AX_VOID *param);

typedef struct AX_WAIT {
    AX_VOID *wait;
} AX_WAIT_T;
AX_S32 AX_OSAL_SYNC_waitqueue_init(AX_WAIT_T *wait);
AX_U32 AX_OSAL_SYNC_wait_uninterruptible(AX_WAIT_T *wait, AX_WAIT_COND_FUNC_T func, AX_VOID *param);
//only for linux kernel
AX_U32 AX_OSAL_SYNC_wait_interruptible(AX_WAIT_T *wait, AX_WAIT_COND_FUNC_T func, AX_VOID *param);
AX_U32 AX_OSAL_SYNC_wait_uninterruptible_timeout(AX_WAIT_T *wait, AX_WAIT_COND_FUNC_T func, AX_VOID *param,
        AX_ULONG timeout);
//only for linux kernel
AX_U32 AX_OSAL_SYNC_wait_interruptible_timeout(AX_WAIT_T *wait, AX_WAIT_COND_FUNC_T func, AX_VOID *param,
        AX_ULONG timeout);
AX_VOID AX_OSAL_SYNC_wakeup(AX_WAIT_T *wait, AX_VOID *key);
AX_VOID AX_OSAL_SYNC_wake_up_interruptible(AX_WAIT_T *osal_wait, AX_VOID *key);
AX_VOID AX_OSAL_SYNC_wake_up_interruptible_all(AX_WAIT_T *wait, AX_VOID *key);

AX_VOID AX_OSAL_SYNC_wait_destroy(AX_WAIT_T *wait);

/*wait event*/

#define AX_OSAL_SYNC_wait_event_interruptible(wait, func, param) \
		({									\
			int __ret = 0;							\
			for (;;){						   \
				if(func(param)){					   \
					__ret = 0;					\
					break;					  \
				}\
				__ret = AX_OSAL_SYNC_wait_interruptible(wait, (func), param);	\
				if(__ret < 0){				  \
					if(__ret == -2) {  \
						__ret = 0;   \
					}  \
					break;  \
				}   \
			}									 \
			__ret;									 \
		})

#define AX_OSAL_SYNC_wait_event_interruptible_timeout(wait, func, param, timeout) \
	({									\
		int __ret = timeout;						  \
									   \
		if ((func(param)) && !timeout) \
		{ \
		__ret = 1; \
		} \
											  \
		for (;;) {							\
			if (func(param))					   \
			{\
				break;					  \
			}\
			__ret = AX_OSAL_SYNC_wait_interruptible_timeout(wait, (func), param, __ret);	 \
			if(__ret < 0)	\
			{\
				break;  \
			}\
			if(!__ret && !func(param))  \
			{\
				__ret = -ETIMEDOUT;   \
				break;  \
			}\
		}									\
		__ret;									 \
	})

#define AX_OSAL_SYNC_wait_event(wait, func, param) \
({                                  \
	int __ret = 0;                          \
	for (;;){                          \
		if(func(param)){                       \
			__ret = 0;                  \
			break;                    \
		}\
		__ret = AX_OSAL_SYNC_wait_uninterruptible(wait, (func), param);   \
		if(__ret < 0)  \
		{ \
			break;  \
		} \
	}                                    \
	__ret;                                   \
})

#define AX_OSAL_SYNC_wait_event_timeout(wait, func, param, timeout) \
({                                  \
	int __ret = timeout;                        \
	if ((func(param)) && !timeout) \
	{ \
		__ret = 1; \
	} \
	for (;;) {                          \
		if (func(param))                       \
		{\
			break;                    \
		}\
		__ret = AX_OSAL_SYNC_wait_uninterruptible_timeout(wait, (func), param, __ret);   \
		if(!__ret && !func(param))   \
		{ \
			__ret = -ETIMEDOUT;   \
			break;                    \
		} \
	}                                   \
	__ret;                                   \
})

//timer api
typedef struct AX_TIMER {
    struct timer_list timer;
    AX_VOID(*function)(AX_VOID *);
    AX_LONG data;
} AX_TIMER_T;

typedef struct AX_TIMERVAL {
    AX_LONG tv_sec;
    AX_LONG tv_usec;
} AX_TIMERVAL_T;

typedef struct AX_TIMERSPEC64 {
    AX_S64 tv_sec;
    AX_LONG tv_nsec;
} AX_TIMERSPEC64_T;

typedef struct AX_RTC_TIMER {
    AX_S32 tm_sec;
    AX_S32 tm_min;
    AX_S32 tm_hour;
    AX_S32 tm_mday;
    AX_S32 tm_mon;
    AX_S32 tm_year;
    AX_S32 tm_wday;
    AX_S32 tm_yday;
    AX_S32 tm_isdst;
} AX_RTC_TIMER_T;

#define AX_OSAL_HRTIMER_NORESTART  0      /* Timer is not restarted */
#define AX_OSAL_HRTIMER_RESTART   1        /* Timer must be restarted */

AX_LONG AX_OSAL_DEV_usleep(unsigned long use);
AX_VOID *AX_OSAL_DEV_hrtimer_alloc(unsigned long use,int (*function)(void *),void *private);
AX_VOID AX_OSAL_DEV_hrtimer_destroy(void *timer);
AX_S32 AX_OSAL_DEV_hrtimer_start(void *timer);
AX_S32 AX_OSAL_DEV_hrtimer_stop(void *timer);

AX_TIMER_T *AX_OSAL_TMR_alloc_timers(void (*function)(void *), AX_LONG data);
AX_S32 AX_OSAL_TMR_init_timers(AX_TIMER_T *timer);
AX_U32 AX_OSAL_TMR_mod_timer(AX_TIMER_T *timer, AX_ULONG interval);
AX_U32 AX_OSAL_TMR_del_timer(AX_TIMER_T *timer);
AX_S32 AX_OSAL_TMR_destory_timer(AX_TIMER_T *timer);
AX_ULONG AX_OSAL_TM_msleep(AX_U32 msecs);
AX_VOID AX_OSAL_TM_udelay(AX_U32 usecs);
AX_VOID AX_OSAL_TM_mdelay(AX_U32 msecs);
AX_LONG  AX_OSAL_TM_msecs_to_jiffies(AX_LONG ax_msecs);
AX_U32 AX_OSAL_TM_jiffies_to_msecs(AX_VOID);
AX_U64 AX_OSAL_TM_sched_clock(AX_VOID);
AX_U64 AX_OSAL_TM_get_microseconds(AX_VOID);
//only for linux kernel
AX_VOID AX_OSAL_TM_do_gettimeofday(AX_TIMERVAL_T *tm);
AX_VOID AX_OSAL_TM_do_settimeofday(AX_TIMERVAL_T *tm);
//only for linux kernel
AX_VOID AX_OSAL_TM_rtc_time_to_tm(AX_ULONG time, AX_RTC_TIMER_T *tm);
//only for linux kernel
AX_VOID AX_OSAL_TM_rtc_tm_to_time(AX_RTC_TIMER_T *tm, AX_ULONG *time);
AX_VOID AX_OSAL_TM_get_jiffies(AX_U64 *pjiffies);
//only for linux kernel
AX_S32 AX_OSAL_TM_rtc_valid_tm(AX_RTC_TIMER_T *tm);
AX_U64 AX_OSAL_TM_get_microsecond(void);
AX_VOID AX_OSAL_TM_hrtimer_mdelay(AX_U32 msecs);
AX_VOID AX_OSAL_TM_hrtimer_udelay(AX_U64 usecs);
AX_VOID AX_OSAL_TM_ktime_get_real_ts64( AX_TIMERSPEC64_T *tm);

//kmalloc , memory
#define AX_OSAL_GFP_ATOMIC  0
#define AX_OSAL_GFP_KERNEL  1

AX_VOID *AX_OSAL_MEM_kmalloc(AX_SIZE_T size, AX_U32 osal_gfp_flag);
AX_VOID *AX_OSAL_MEM_kzalloc(AX_SIZE_T size, AX_U32 osal_gfp_flag);
AX_VOID AX_OSAL_MEM_kfree(const AX_VOID *addr);
AX_VOID *AX_OSAL_MEM_vmalloc(AX_SIZE_T size);
AX_VOID AX_OSAL_MEM_vfree(const AX_VOID *addr);
AX_S32 AX_OSAL_MEM_VirtAddrIsValid(AX_ULONG vm_start, AX_ULONG vm_end);
AX_S32 AX_OSAL_MEM_AddrMunmap(AX_ULONG start, AX_SIZE_T size);
AX_ULONG AX_OSAL_MEM_AddrMmap(AX_VOID *file, AX_ULONG addr,
	AX_ULONG len, AX_ULONG prot, AX_ULONG flag, AX_ULONG offset);

AX_VOID *ax_os_mem_kmalloc(AX_S32 id, AX_SIZE_T size, AX_U32 flag);
AX_VOID *ax_os_mem_kzalloc(AX_S32 id, AX_SIZE_T size, AX_U32 flag);
AX_VOID ax_os_mem_kfree(AX_S32 id, const AX_VOID *addr);
AX_VOID *ax_os_mem_vmalloc(AX_S32 id, AX_SIZE_T size);
AX_VOID ax_os_mem_vfree(AX_S32 id, const AX_VOID *addr);
AX_S32 ax_os_release_reserved_mem(AX_ULONG phy_start, AX_SIZE_T size, const AX_S8 *s);

//file system , only for linux kernel
#define AX_OSAL_O_RDONLY         00
#define AX_OSAL_O_WRONLY         01
#define AX_OSAL_O_RDWR           02

#define AX_OSAL_O_CREAT        0100
#define AX_OSAL_O_EXCL         0200
#define AX_OSAL_O_NOCTTY       0400
#define AX_OSAL_O_TRUNC       01000
#define AX_OSAL_O_APPEND      02000
#define AX_OSAL_O_NONBLOCK    04000
#define AX_OSAL_O_DSYNC      010000
#define AX_OSAL_O_SYNC     04010000
#define AX_OSAL_O_RSYNC    04010000
#define AX_OSAL_O_BINARY    0100000
#define AX_OSAL_O_DIRECTORY 0200000
#define AX_OSAL_O_NOFOLLOW  0400000
#define AX_OSAL_O_CLOEXEC  02000000

AX_VOID *AX_OSAL_FS_filp_open(const AX_S8 *filename, AX_S32 flags, AX_S32 mode);
AX_VOID AX_OSAL_FS_filp_close(AX_VOID * filp);
AX_S32 AX_OSAL_FS_filp_write(AX_S8 *buf, AX_S32 len, AX_VOID * filp);
AX_S32 AX_OSAL_FS_filp_read(AX_S8 *buf, AX_S32 len, AX_VOID * filp);
AX_S32 AX_OSAL_FS_FILP_llseek(AX_S32 whence, AX_S32 offset, AX_VOID * file);

/*module init&exit API*/
/*IMPORTANT: 'module init&exit' not support to compat OS, later we will fixed, because we may change RTT sourcecode*/

/*Linux init module */
#include <linux/module.h>
#include <linux/mm.h>
#include <linux/mman.h>
#include <linux/fcntl.h>
#include <linux/of.h>
#include <linux/irqreturn.h>

#define AX_OSAL_module_initcall(fn) module_init(fn);
#define AX_OSAL_module_exit(fn) module_exit(fn);

/*IO write/read*/
#define AX_OSAL_IO_writel(v, x) (*((volatile int *)(x)) = (v))
#define AX_OSAL_IO_readl(x) (*((volatile int *)(x)))

void AX_OSAL_IO_bit_set(AX_U32 *bitmap, AX_S32 pos);
void AX_OSAL_IO_bit_clean(AX_U32 *bitmap, AX_S32 pos);

/* srcu api */
typedef struct AX_OSAL_srcu_struct {
	AX_VOID *ssp;
} AX_OSAL_srcu_struct_t;

#ifndef CONFIG_DEBUG_LOCK_ALLOC
AX_S32 AX_OSAL_init_srcu_struct(AX_OSAL_srcu_struct_t *ssp);
#else
#include <linux/slab.h>
AX_VOID *AX_OSAL_DBG_init_srcu_struct(AX_OSAL_srcu_struct_t *ssp);
#define AX_OSAL_init_srcu_struct(ssp)				\
	({							\
		AX_S32 ret = 0;					\
		AX_VOID *p = AX_OSAL_DBG_init_srcu_struct(ssp);	\
		if (p != NULL) {				\
			ret = init_srcu_struct(p);		\
			if (ret != 0) {				\
				kfree(p);			\
				ssp->ssp = NULL;		\
			}					\
		} else {					\
			ret = -1;				\
		}						\
		ret;						\
	})
#endif
AX_VOID AX_OSAL_cleanup_srcu_struct(AX_OSAL_srcu_struct_t *ssp);
AX_S32 AX_OSAL_srcu_read_lock(AX_OSAL_srcu_struct_t *ssp);
AX_VOID AX_OSAL_srcu_read_unlock(AX_OSAL_srcu_struct_t *ssp, AX_S32 idx);
AX_VOID AX_OSAL_synchronize_srcu(AX_OSAL_srcu_struct_t *ssp);

/* percpu api */
#define AX_OSAL___percpu __percpu
#define AX_OSAL_for_each_possible_cpu for_each_possible_cpu
#define AX_OSAL_alloc_percpu alloc_percpu
#define AX_OSAL_this_cpu_ptr this_cpu_ptr
#define AX_OSAL_get_cpu_ptr get_cpu_ptr
#define AX_OSAL_put_cpu_ptr put_cpu_ptr
#define AX_OSAL_per_cpu_ptr per_cpu_ptr
#define AX_OSAL_free_percpu free_percpu

/* prefetch api */
AX_VOID AX_OSAL_prefetch(AX_VOID *addr);
AX_VOID AX_OSAL_prefetchw(AX_VOID *addr);

#ifdef __cplusplus
}
#endif

#endif /*__OSAL_AX__H__*/
