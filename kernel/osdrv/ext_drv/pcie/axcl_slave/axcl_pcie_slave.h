#ifndef __AXCL_PCIE_HEADER__
#define __AXCL_PCIE_HEADER__

#include "../include/ax_pcie_msg_ursdev.h"
#include "../include/ax_pcie_dev.h"
#include "../include/ax_pcie_msg_transfer.h"
#include "../include/ax_pcie_proc.h"

#define AXCL_MAX_PORT (4)

#define AXCL_HEARTBEAT_PORT   (10)
#define AXCL_NOTIFY_PORT   (11)

enum cmd_type {
	AXCL_PORT_CREATE = 1,
	AXCL_PORT_DESTROY = 2,
	AXCL_PORT_CREATE_COMPLETION = 3,
	AXCL_PORT_DESTROY_COMPLETION = 4,
	AXCL_PORT_CREATE_FAIL = 5,
	AXCL_PORT_DESTROY_FAIL = 6,
};

enum heartbeat_type {
	AXCL_HEARTBEAT_DEAD = 0,
	AXCL_HEARTBEAT_ALIVE = 1,
};

struct device_connect_status {
	unsigned int status[AXERA_MAX_MAP_DEV];	/* 0: dead; 1: alive */
};

struct device_heart_packet {
	unsigned int device;
	unsigned int interval;
	unsigned int timestamp;
	unsigned int last_timestamp; /* must be placed at the end of the struct device_heart_packet */
};

struct axcl_device_info {
	unsigned int cmd;	/* 1: create; 2: destroy */
	unsigned int device;
	unsigned int pid;
	unsigned int port_num;
	unsigned int ports[AXCL_MAX_PORT];
	unsigned int dma_buf_size; /* pcie dma buf size */
	unsigned int log_level;    /* device log level */
};

struct ax_mem_list {
	struct list_head head;
	void *data;
	unsigned int data_len;
};

#define AXCL_DEBUG		4
#define AXCL_INFO		3
#define AXCL_ERR		2
#define AXCL_FATAL		1
#define AXCL_CURRENT_LEVEL	2
#define axcl_trace(level, s, params...)	do { \
	if (level <= AXCL_CURRENT_LEVEL)	\
	printk("[%s, %d]: " s "\n", __func__, __LINE__, ##params);	\
} while (0)

#define IOC_AXCL_MAGIC 'A'
#define IOC_AXCL_PORT_MANAGE    _IOWR(IOC_AXCL_MAGIC, 1, struct axcl_device_info)
#define IOC_AXCL_CONN_STATUS    _IOR(IOC_AXCL_MAGIC, 2, struct device_connect_status)
#define IOC_AXCL_HEART_BEATS    _IOW(IOC_AXCL_MAGIC, 3, struct device_heart_packet)

#endif
