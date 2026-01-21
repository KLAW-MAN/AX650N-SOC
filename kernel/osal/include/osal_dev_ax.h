/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#ifndef __OSAL_DEV_AX__H__
#define __OSAL_DEV_AX__H__

#ifdef __cplusplus
extern "C" {
#endif
#include "osal_type_ax.h"
#include "osal_ax.h"

//device
#define AX_OSAL_L_PTE_VALID     0
#define AX_OSAL_L_PTE_PRESENT   1
#define AX_OSAL_L_PTE_YOUNG     2
#define AX_OSAL_L_PTE_DIRTY     3

#define AX_OSAL_L_PTE_MT_DEV_SHARED     4
#define AX_OSAL_L_PTE_MT_DEV_NONSHARED  5
#define AX_OSAL_L_PTE_MT_DEV_WC         6
#define AX_OSAL_L_PTE_MT_DEV_CACHED     7

#define AX_OSAL_SEEK_SET 0
#define AX_OSAL_SEEK_CUR 1
#define AX_OSAL_SEEK_END 2

#define AXERA_RESOURCE_NAME 32

enum AX_DEVFREQ_TIMER {
        AX_DEVFREQ_TIMER_DEFERRABLE = 0,
        AX_DEVFREQ_TIMER_DELAYED,
        AX_DEVFREQ_TIMER_NUM,
};

struct AX_DEVFREQ_DEV_STATUS {
        /* both since the last measure */
        AX_ULONG total_time;
        AX_ULONG busy_time;
        AX_ULONG current_frequency;
        void *private_data;
};


struct AX_DEVFREQ_DEV_PROFILE {
        AX_ULONG initial_freq;
        AX_S32 polling_ms;
        enum AX_DEVFREQ_TIMER timer;
        bool is_cooling_device;

        AX_S32 (*target)(AX_VOID *dev,AX_ULONG rate,AX_ULONG volt);
        AX_S32 (*get_dev_status)(AX_VOID *dev,
                              struct AX_DEVFREQ_DEV_STATUS *stat);
        AX_ULONG (*get_cur_freq)(AX_VOID *dev);
        AX_VOID (*exit)(AX_VOID *dev);

        AX_ULONG *freq_table;
        AX_S32 max_state;
};


struct AXERA_RESOURCE {
    AX_U64 start;
    AX_U64 end;
    AX_U8 name[AXERA_RESOURCE_NAME];
    AX_U64 flags;
    AX_U64 desc;
};

struct AX_OF_DEVICE_ID  {
    AX_U8    name[32];
    AX_U8    type[32];
    AX_U8    compatible[128];
    const AX_VOID *data;
};

struct AX_PLATFORM_DEVICE_DRIVER {
    const AX_U8      *name;
#if defined(CONFIG_OF)
    const AX_VOID   *of_match_table;
#endif
};


struct AX_PLATFORM_DRIVER {
    AX_S32 (*probe)(AX_VOID *);
    AX_S32 (*remove)(AX_VOID *);
    AX_VOID (*shutdown)(AX_VOID *);
    AX_S32 (*suspend)(AX_VOID *);
    AX_S32 (*resume)(AX_VOID *);
    struct AX_PLATFORM_DEVICE_DRIVER driver;
    AX_VOID *axera_ptr;
};


typedef struct AX_POLL {
    AX_VOID *poll_table;
    AX_VOID *data;
    AX_VOID *wait; /*AX_WAIT, only support one poll, read or write*/
} AX_POLL_T;

typedef struct AX_DEV {
    AX_S8 name[48];
    AX_VOID *dev;
    AX_S32 minor;
    struct AX_FILEOPS *fops;
    struct AX_PMOPS *osal_pmops;
    AX_VOID *private_data;
    struct AX_POLL dev_poll;
    struct AX_WAIT dev_wait;
} AX_DEV_T;

typedef struct AX_VM {
    AX_VOID *vm;
} AX_VM_T;

typedef struct AX_DEV_PRIVATE_DATA {
    struct AX_DEV *dev;
    void *data;
    struct AX_POLL table;
    int f_ref_cnt;
    unsigned int f_flags;		//todo: this parameter will be deleted after the driver modification is completed
    struct file *file;
} AX_DEV_PRIVATE_DATA_T;

#define AX_OSAL_DEV_PRI_DATA(pri_data) ((( struct AX_DEV_PRIVATE_DATA  *)pri_data)->data)

typedef struct AX_FILEOPS {
    AX_S32(*open)(AX_VOID *private_data);
    AX_S32(*read)(AX_S8 *buf, AX_S32 size, AX_LONG *offset, AX_VOID *private_data);
    AX_S32(*write)(const AX_S8 *buf, AX_S32 size, AX_LONG *offset, AX_VOID *private_data);
    AX_LONG(*llseek)(AX_LONG offset, AX_S32 whence, AX_VOID *private_data);
    AX_S32(*release)(AX_VOID *private_data);
    AX_LONG(*unlocked_ioctl)(AX_U32 cmd, AX_ULONG arg, AX_VOID *private_data);
    AX_U32(*poll)(AX_POLL_T *osal_poll, AX_VOID *private_data);

    /*******RTT not support mmap*************/
    AX_S32 (*mmap) (AX_VM_T *vm, AX_ULONG start, AX_ULONG end, AX_ULONG vm_pgoff, AX_VOID *private_data);
} AX_FILEOPS_T;

typedef struct AX_PMOPS {
    AX_S32(*pm_suspend)(AX_DEV_T *dev);
    AX_S32(*pm_resume)(AX_DEV_T *dev);
    AX_S32(*pm_suspend_late)(AX_DEV_T *dev);
    AX_S32(*pm_resume_early)(AX_DEV_T *dev);

    /*******RTT not support the other pm ops*************/
    AX_S32 (*pm_prepare)(AX_DEV_T *dev);
    AX_VOID (*pm_complete)(AX_DEV_T *dev);
    AX_S32 (*pm_freeze)(AX_DEV_T *dev);
    AX_S32 (*pm_thaw)(AX_DEV_T *dev);
    AX_S32 (*pm_poweroff)(AX_DEV_T *dev);
    AX_S32 (*pm_restore)(AX_DEV_T *dev);
    AX_S32 (*pm_freeze_late)(AX_DEV_T *dev);
    AX_S32 (*pm_thaw_early)(AX_DEV_T *dev);
    AX_S32 (*pm_poweroff_late)(AX_DEV_T *dev);
    AX_S32 (*pm_restore_early)(AX_DEV_T *dev);
    AX_S32 (*pm_suspend_noirq)(AX_DEV_T *dev);
    AX_S32 (*pm_resume_noirq)(AX_DEV_T *dev);
    AX_S32 (*pm_freeze_noirq)(AX_DEV_T *dev);
    AX_S32 (*pm_thaw_noirq)(AX_DEV_T *dev);
    AX_S32 (*pm_poweroff_noirq)(AX_DEV_T *dev);
    AX_S32 (*pm_restore_noirq)(AX_DEV_T *dev);
} AX_OSAL_PMOPS_T;

AX_DEV_T *AX_OSAL_DEV_createdev(AX_S8 *name);
AX_S32 AX_OSAL_DEV_destroydev(AX_DEV_T *ax_dev);
AX_S32 AX_OSAL_DEV_device_register(AX_DEV_T *ax_dev);
AX_VOID AX_OSAL_DEV_device_unregister(AX_DEV_T *ax_dev);
//only for linux kernel
AX_VOID AX_OSAL_DEV_poll_wait(AX_POLL_T *table, AX_WAIT_T *wait);
//only for linux kernel
AX_VOID AX_OSAL_DEV_pgprot_noncached(AX_VM_T *vm);
//only for linux kernel
AX_VOID AX_OSAL_DEV_pgprot_cached(AX_VM_T *vm);
//only for linux kernel
AX_VOID AX_OSAL_DEV_pgprot_writecombine(AX_VM_T *vm);
//only for linux kernel
AX_VOID AX_OSAL_DEV_pgprot_stronglyordered(AX_VM_T *vm);
//only for linux kernel
AX_S32 AX_OSAL_DEV_remap_pfn_range(AX_VM_T *vm, AX_ULONG addr, AX_ULONG pfn, AX_ULONG size);
//only for linux kernel
AX_S32 AX_OSAL_DEV_io_remap_pfn_range(AX_VM_T *vm, AX_ULONG addr, AX_ULONG pfn, AX_ULONG size);

//addr translate
//only for linux kernel
AX_VOID *AX_OSAL_DEV_ioremap(AX_ULONG phys_addr, AX_ULONG size);
//only for linux kernel
AX_VOID *AX_OSAL_DEV_ioremap_nocache(AX_ULONG phys_addr, AX_ULONG size);
//only for linux kernel
AX_VOID *AX_OSAL_DEV_ioremap_cache(AX_ULONG phys_addr, AX_ULONG size);
//only for linux kernel
AX_VOID AX_OSAL_DEV_iounmap(AX_VOID *addr);
AX_ULONG AX_OSAL_DEV_copy_from_user(AX_VOID *to, const AX_VOID *from, AX_ULONG n);
AX_ULONG AX_OSAL_DEV_copy_to_user(AX_VOID *to, const AX_VOID *from, AX_ULONG n);

AX_ULONG AX_OSAL_DEV_usr_virt_to_phys(AX_ULONG virt);
AX_ULONG AX_OSAL_DEV_kernel_virt_to_phys(void *virt);
//cache api
AX_VOID AX_OSAL_DEV_invalidate_dcache_area(AX_VOID  *addr, AX_S32 size);
AX_VOID AX_OSAL_DEV_flush_dcache_area(AX_VOID *kvirt, AX_ULONG length);
//only for linux kernel
AX_VOID AX_OSAL_DEV_flush_dcache_all(AX_VOID);

AX_VOID AX_OSAL_DEV_outer_dcache_area(AX_U64 phys_addr_start, AX_U64 phys_addr_end);

//interrupt api
typedef AX_S32(*AX_IRQ_HANDLER_T)(AX_S32, AX_VOID *);
AX_S32 AX_OSAL_DEV_request_threaded_irq(AX_U32 irq, AX_IRQ_HANDLER_T handler, AX_IRQ_HANDLER_T thread_fn,
                                        const AX_S8 *name, AX_VOID *dev);
AX_S32 AX_OSAL_DEV_request_threaded_irq_ex(AX_U32 irq, AX_IRQ_HANDLER_T handler, AX_IRQ_HANDLER_T thread_fn,
                                        AX_ULONG flags, const AX_S8 *name, AX_VOID *dev);

const AX_VOID *AX_OSAL_DEV_free_irq(AX_U32 irq, AX_VOID *dev);
AX_S32 AX_OSAL_DEV_in_interrupt(AX_VOID);

AX_VOID AX_OSAL_DEV_enable_irq(AX_U32 irq);
AX_VOID AX_OSAL_DEV_disable_irq(AX_U32 irq);
AX_VOID AX_OSAL_DEV_disable_irq_nosync(AX_U32 irq);

enum AX_OSAL_irqchip_irq_state {
	AX_OSAL_IRQCHIP_STATE_PENDING,		/* Is interrupt pending? */
	AX_OSAL_IRQCHIP_STATE_ACTIVE,		/* Is interrupt in progress? */
	AX_OSAL_IRQCHIP_STATE_MASKED,		/* Is interrupt masked? */
	AX_OSAL_IRQCHIP_STATE_LINE_LEVEL,	/* Is IRQ line high? */
};

AX_S32 AX_OSAL_DEV_irq_get_irqchip_state(AX_U32 irq, enum AX_OSAL_irqchip_irq_state which,
			  AX_S32 *state);

AX_S32 AX_OSAL_DEV_irq_set_irqchip_state(AX_U32 irq, enum AX_OSAL_irqchip_irq_state which,
			  AX_S32 val);


/*device framework(platform) API*/
AX_S32 AX_OSAL_DEV_platform_driver_register(AX_VOID *drv);
AX_VOID AX_OSAL_DEV_platform_driver_unregister(AX_VOID *drv);
AX_S32 AX_OSAL_DEV_platform_get_resource_byname(AX_VOID *dev, AX_U32 type, const AX_S8 *name,struct AXERA_RESOURCE *res);
AX_S32 AX_OSAL_DEV_platform_get_resource(AX_VOID *dev, AX_U32 type, AX_U32 num,struct AXERA_RESOURCE *res);
AX_S32 AX_OSAL_DEV_platform_get_irq(AX_VOID *dev, AX_U32 num);
AX_S32 AX_OSAL_DEV_platform_get_irq_byname(AX_VOID *dev, const AX_S8 *name);
AX_ULONG AX_OSAL_DEV_resource_size(const struct AXERA_RESOURCE *res);
AX_VOID *AX_OSAL_DEV_platform_get_drvdata(AX_VOID *pdev);
AX_VOID AX_OSAL_DEV_platform_set_drvdata(AX_VOID *pdev,AX_VOID *data);
AX_S32 AX_OSAL_DEV_platform_irq_count(AX_VOID *dev);
AX_VOID  *AX_OSAL_DEV_to_platform_device(AX_VOID * dev);
AX_VOID  *AX_OSAL_DEV_to_platform_driver(AX_VOID * drv);

AX_S32 AX_OSAL_DEV_of_property_read_string(AX_VOID *pdev,const char *propname,const char **out_string);
#if 0
const AX_VOID *AX_OSAL_DEV_of_get_property(AX_VOID *pdev, const AX_S8 *name,AX_S32 *lenp);
AX_S32 AX_OSAL_DEV_of_property_read_string_array(AX_VOID *pdev,const AX_S8 *propname,const char **out_strs,AX_U64 sz);
AX_S32 AX_OSAL_DEV_of_property_count_strings(AX_VOID *pdev,const AX_S8 *propname);
AX_S32 AX_OSAL_DEV_of_property_read_string_index(AX_VOID *pdev,const AX_S8 *propname,AX_S32 index, const char **output);
#endif
bool AX_OSAL_DEV_of_property_read_bool(AX_VOID *pdev,const AX_S8 *propname);
#if 0
AX_S32 AX_OSAL_DEV_of_property_read_u8(AX_VOID *pdev,const AX_S8 *propname,AX_S8 *out_value);
AX_S32 AX_OSAL_DEV_of_property_read_u16(AX_VOID *pdev,const AX_S8 *propname,AX_U16 *out_value);
#endif
AX_S32 AX_OSAL_DEV_of_property_read_u32(AX_VOID *pdev,const char *propname,AX_U32 *out_value);
AX_S32 AX_OSAL_DEV_of_property_read_s32(AX_VOID *pdev,const AX_S8 *propname,AX_S32 *out_value);



AX_VOID *AX_OSAL_DEV_devm_clk_get(AX_VOID *pdev, const char *id);
AX_VOID AX_OSAL_DEV_devm_clk_put(AX_VOID *pdev,AX_VOID *pclk);
AX_VOID AX_OSAL_DEV_clk_disable(AX_VOID *pclk);
AX_S32 AX_OSAL_DEV_clk_enable(AX_VOID *pclk);
bool AX_OSAL_DEV_clk_is_enabled(AX_VOID * pclk);
AX_S32 AX_OSAL_DEV_clk_prepare_enable(AX_VOID *pclk);
AX_S32 AX_OSAL_DEV_clk_set_rate(AX_VOID *pclk, AX_ULONG rate);
AX_ULONG AX_OSAL_DEV_clk_get_rate(AX_VOID *pclk);
AX_VOID AX_OSAL_DEV_clk_disable_unprepare(AX_VOID *pclk);


AX_S32 AX_OSAL_DEV_i2c_write(unsigned char i2c_dev, unsigned char dev_addr,
                         unsigned int reg_addr, unsigned int reg_addr_num,
                         unsigned int data, unsigned int data_byte_num);

AX_S32 AX_OSAL_DEV_i2c_read(unsigned char i2c_dev, unsigned char dev_addr,
                        unsigned int reg_addr, unsigned int reg_addr_num,
                        unsigned int *pRegData, unsigned int data_byte_num);

AX_S32 AX_OSAL_DEV_i2c_dev_init(AX_VOID);
AX_VOID AX_OSAL_DEV_i2c_dev_exit(AX_VOID);

AX_S32 AX_OSAL_DEV_spi_dev_read(u32 spi_bus, u32 spi_csn, u8 devid, u8 addrL, u8 addrH, u8 *data);
AX_S32 AX_OSAL_DEV_spi_dev_write(u32 spi_bus, u32 spi_csn, u8 devid, u8 addrL, u8 addrH, u8 data, u32 len);
AX_S32 AX_OSAL_DEV_spi_dev_init(AX_VOID);


AX_VOID *AX_OSAL_DEV_devm_reset_control_get_optional(AX_VOID *pdev, const char *id, int flag);
AX_S32  AX_OSAL_DEV_reset_control_assert(AX_VOID *rstc);
AX_S32  AX_OSAL_DEV_reset_control_deassert(AX_VOID *rstc);

#include <linux/soc/axera/ax_bw_limiter.h>
AX_S32 AX_OSAL_DEV_bwlimiter_register_with_clk(unsigned int sub_sys_bw, AX_VOID *pclk);
AX_S32 AX_OSAL_DEV_bwlimiter_register_with_val(unsigned int sub_sys_bw, AX_U32 clk);
AX_S32 AX_OSAL_DEV_bwlimiter_unregister(unsigned int sub_sys_bw, AX_VOID *pclk);
AX_S32 AX_OSAL_DEV_bwlimiter_refresh_limiter(unsigned int sub_sys_bw);

AX_S32 AX_OSAL_DEV_pm_opp_of_add_table(AX_VOID *pdev);
AX_VOID AX_OSAL_DEV_pm_opp_of_remove_table(AX_VOID *pdev);
AX_S32 AX_OSAL_DEV_devm_devfreq_add_device(AX_VOID *pdev,struct AX_DEVFREQ_DEV_PROFILE *ax_profile,const char *governor_name,void *data);

#ifdef __cplusplus
}
#endif

#endif /*__OSAL_DEV_AX__H__*/
