#ifndef __AXCL_PCIE_HEADER__
#define __AXCL_PCIE_HEADER__

#include "../include/ax_pcie_msg_ursdev.h"
#include "../include/ax_pcie_dev.h"
#include "../include/ax_pcie_msg_transfer.h"
#include "../include/ax_pcie_proc.h"

#define IOC_AXCL_MAGIC 'A'
#define IOC_AXCL_PORT_MANAGE    _IOWR(IOC_AXCL_MAGIC, 1, struct axcl_device_info)
#define IOC_AXCL_CONN_STATUS    _IOR(IOC_AXCL_MAGIC, 2, struct device_connect_status)
#define IOC_AXCL_HEART_BEATS    _IOW(IOC_AXCL_MAGIC, 3, struct device_heart_packet)
#define IOC_AXCL_DEVICE_LIST    _IOWR(IOC_AXCL_MAGIC, 4, struct device_list_t)
#define IOC_AXCL_WAKEUP_POLL    _IOWR(IOC_AXCL_MAGIC, 5, struct axcl_device_info)
#define IOC_AXCL_DEVICE_RESET	_IOWR(IOC_AXCL_MAGIC, 6, struct axcl_device_info)
#define IOC_AXCL_DEVICE_BOOT    _IOWR(IOC_AXCL_MAGIC, 7, struct axcl_device_info)
#define IOC_AXCL_BUS_INFO       _IOWR(IOC_AXCL_MAGIC, 8, struct axcl_bus_info_t)
#define IOC_AXCL_PID_NUM     	_IOWR(IOC_AXCL_MAGIC, 9, struct axcl_pid_num_t)
#define IOC_AXCL_PID_INFO    	_IOWR(IOC_AXCL_MAGIC, 10, struct axcl_pid_info_t)

#define AXCL_PROCESS_MAX    (64)
#define AXCL_RECV_TIMEOUT   (50000)

#define AXCL_PATH_NAME   "axcl/ax650_card.pac"
#define AXCL_SYSDUMP_NAME   "vmcore_dump"
#define AXCL_NAME_LEN   0xFF
#define MAX_TRANSFER_SIZE   (0x300000)
#define AXCL_MAX_PORT (4)

#define AX_PCIE_ENDPOINT_COMMAND		0x4
#define AX_PCIE_COMMAND_READ			BIT(3)
#define AX_PCIE_COMMAND_WRITE			BIT(4)
#define AX_PCIE_COMMAND_BOOT_REASION		BIT(5)

#define AX_PCIE_ENDPOINT_STATUS			0x8
#define STATUS_READ_SUCCESS			BIT(0)
#define STATUS_READ_FAIL			BIT(1)

#define STATUS_WRITE_SUCCESS			BIT(13)
#define STATUS_WRITE_FAIL			BIT(14)

#define STATUS_COPY_SUCCESS			BIT(4)
#define STATUS_COPY_FAIL			BIT(5)
#define STATUS_IRQ_RAISED			BIT(6)
#define STATUS_SRC_ADDR_INVALID			BIT(7)
#define STATUS_DST_ADDR_INVALID			BIT(8)

#define	BOOT_START_DEVICES			BIT(0)

#define AX_PCIE_ENDPOINT_LOWER_SRC_ADDR		0x0c
#define AX_PCIE_ENDPOINT_UPPER_SRC_ADDR		0x10

#define AX_PCIE_ENDPOINT_LOWER_DST_ADDR		0x14
#define AX_PCIE_ENDPOINT_UPPER_DST_ADDR		0x18

#define AX_PCIE_ENDPOINT_SIZE			0x1c
#define AX_PCIE_ENDPOINT_CHECKSUM		0x20

#define AX_PCIE_ENDPOINT_BOOT_REASON_TYPE	0x24
#define AX_PCIE_ENDPOINT_FINISH_STATUS		0x28

#define AXCL_BASE_PORT  21
#define AXCL_HEARTBEAT_PORT   (10)
#define AXCL_NOTIFY_PORT   (11)

typedef struct _PAC_HEAD_T {
	u32 nMagic;
	u32 nPacVer;
	u64 u64PacSize;
	char szProdName[32];
	char szProdVer[32];
	u32 nFileOffset;
	u32 nFileCount;
	u32 nAuth;
	u32 crc16;
	u32 md5;
} PAC_HEAD_T;

typedef struct _BLOCK_T {
	u64 u64Base;
	u64 u64Size;
	char szPartID[72];
} BLOCK_T;

typedef struct _PAC_FILE_T {
	char szID[32];
	char szType[32];
	char szFile[256];
	u64 u64CodeOffset;
	u64 u64CodeSize;
	BLOCK_T tBlock;
	u32 nFlag;
	u32 nSelect;
	u32 reserved[8];
} PAC_FILE_T;

enum cmd_type {
	AXCL_PORT_CREATE = 1,
	AXCL_PORT_DESTROY = 2,
	AXCL_PORT_CREATE_COMPLETION = 3,
	AXCL_PORT_DESTROY_COMPLETION = 4,
	AXCL_PORT_CREATE_FAIL = 5,
	AXCL_PORT_DESTROY_FAIL = 6,
	AXCL_PORT_REQUEST_USR_PORT = 7,
	AXCL_PORT_RECYCLE_USR_PORT = 8,
};

enum heartbeat_type {
	AXCL_HEARTBEAT_DEAD = 0,
	AXCL_HEARTBEAT_ALIVE = 1,
};

struct device_list_t {
	unsigned int type;	/* 0: pcie */
	unsigned int num;	/* device connected num */
	unsigned int devices[MAX_DEV_NUMBER];	/* ep target id */
};

struct axcl_bus_info_t {
	unsigned int device; /* bus */
	unsigned int domain;
	unsigned int slot;
	unsigned int func;
};

struct axcl_pid_num_t {
	unsigned int device;
	unsigned int num;
};

struct axcl_pid_info_t {
	unsigned int device;
	unsigned int num;
	unsigned long long pid;
};

struct device_heart_packet {
	unsigned int device;
	unsigned int interval;
	unsigned int timestamp;
	unsigned int last_timestamp; /* must be placed at the end of the struct device_heart_packet */
};

struct device_connect_status {
	unsigned int status[AXERA_MAX_MAP_DEV];	/* 0: dead; 1: alive */
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

struct usr_port_info {
	unsigned int port_num;
	unsigned int ports[AXCL_MAX_PORT];
	struct list_head head;
};

struct device_handle_t {
	unsigned int cmd;	/* 1: create; 2: destroy */
	unsigned int device;
	unsigned int pid;
	unsigned int port_num;
	unsigned int ports[AXCL_MAX_PORT];
	struct list_head head;
	struct list_head usr_port_list;
};

struct process_info_t {
	struct list_head head;
	struct list_head dev_list;
	unsigned int pid;
	atomic_t event;
	wait_queue_head_t wait;
};

struct axcl_handle_t {
	struct list_head process_list;
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

extern ax_pcie_msg_handle_t *port_handle[AXERA_MAX_MAP_DEV][MAX_MSG_PORTS];

extern int axcl_pcie_msg_send(struct ax_transfer_handle *handle, void *kbuf,
			      unsigned int count);
extern int axcl_pcie_recv_timeout(ax_pcie_msg_handle_t *handle, void *buf,
				  size_t count, int timeout);
extern void axcl_devices_heartbeat_status_set(unsigned int target,
					      enum heartbeat_type status);
extern enum heartbeat_type axcl_devices_heartbeat_status_get(unsigned int
							     target);
#endif
