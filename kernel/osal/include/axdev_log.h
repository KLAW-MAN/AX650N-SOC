#ifndef _LINUX_AXDEV_LOG_H_
#define _LINUX_AXDEV_LOG_H_

#include <linux/printk.h>

#define AXDEV_LOG_EMERG(fmt,...) \
    pr_emerg("[AXDEV][KO][%s][%d]: "fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__);

#define AXDEV_LOG_ALERT(fmt,...) \
    pr_alert("[AXDEV][KO][%s][%d]: "fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__);

#define AXDEV_LOG_CRIT(fmt,...) \
    pr_crit("[AXDEV][KO][%s][%d]: "fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__);

#define AXDEV_LOG_ERR(fmt,...) \
    pr_err("[AXDEV][KO][%s][%d]: "fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__);

#define AXDEV_LOG_WARN(fmt,...) \
    pr_warning("[AXDEV][KO][%s][%d]: "fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__);

#define AXDEV_LOG_NOTICE(fmt,...) \
    pr_notice("[AXDEV][KO][%s][%d]: "fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__);

#define AXDEV_LOG_INFO(fmt,...) \
    pr_info("[AXDEV][KO][%s][%d]: "fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__);

#define AXDEV_LOG_DEBUG(fmt,...) \
    pr_debug("[AXDEV][KO][%s][%d]: "fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__);

#define AXDEV_LOG_ERROR     AXDEV_LOG_ERR
#endif
