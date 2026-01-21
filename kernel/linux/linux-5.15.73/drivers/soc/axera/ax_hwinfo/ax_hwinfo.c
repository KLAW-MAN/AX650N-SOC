#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/io.h>
#include <linux/of.h>
#include <linux/gpio.h>
#include <linux/virtio.h>
#include <linux/uaccess.h>
#include <linux/miscdevice.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/kthread.h>
#include <linux/ioctl.h>
#include <linux/fs.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include "ax_hwinfo.h"
#include <linux/version.h>

static u32 enable_chan;
extern int read_adc_value(u32 enable, struct adc_value *adc_value);

static struct proc_dir_entry *uid_file;
static struct proc_dir_entry *board_id_file;
static struct proc_dir_entry *adp_plate_id_file;
static struct proc_dir_entry *chip_type_file;
static u32 ax_uid_l;
static u32 ax_uid_h;
static u32 board_id;
static u32 adap_plate_id;
static u32 chip_type;

u32 ax_get_chip_type(void);

static const char *chip_type_name[AX650_CHIP_MAX] = {
	[AX650A_CHIP] = "AX650A_CHIP",
	[AX650N_CHIP] = "AX650N_CHIP",
	[AX650C_CHIP] = "AX650C_CHIP",
	[AX750_CHIP] = "AX750_CHIP",
};

static const char * adp_plate_name[ADP_RX_MAX] = {
	[ADP_RX_DPHY_2X4LANE] = "ADP_RX_DPHY_2X4LANE",
	[ADP_RX_DPHY_8X2LANE] = "ADP_RX_DPHY_8X2LANE",
	[ADP_RX_DPHY_4X4LANE] = "ADP_RX_DPHY_4X4LANE",
	[ADP_RX_DPHY_2X8_LVDS_1X16LANE] = "ADP_RX_DPHY_2X8_LVDS_1X16LANE",
	[ADP_RX_DPHY_2X4LANE_N] = "ADP_RX_DPHY_2X4LANE_N",
	[ADP_RX_CPHY_8X2TRIO] = "ADP_RX_CPHY_8X2TRIO",
	[ADP_RX_CPHY_4X3TRIO] = "ADP_RX_CPHY_4X3TRIO",
	[ADP_RX_DVP_1X10BIT_DPHY_1X2LANE_BT601_656_1120] = "ADP_RX_DVP_1X10BIT_DPHY_1X2LANE_BT601_656_1120",
	[ADP_RX_BT1120_2X10BIT] = "ADP_RX_BT1120_2X10BIT",
	[ADP_RX_BT1120_2X8BIT_DPHY_2X2LANE] = "ADP_RX_BT1120_2X8BIT_DPHY_2X2LANE",
	[ADP_RX_BT601_656_2X10BIT_DPHY_2X2LANE] = "ADP_RX_BT601_656_2X10BIT_DPHY_2X2LANE",
	[ADP_RX_BT601_656_2X8BIT_DPHY_4X2LANE] = "ADP_RX_BT601_656_2X8BIT_DPHY_4X2LANE",
	[ADP_RX_DVP_16bit_HDMI_TO_DPHY_2X4LANE] = "ADP_RX_DVP_16bit_HDMI_TO_DPHY_2X4LANE",
	[ADP_RX_BT_TO_CVBS] = "ADP_RX_BT_TO_CVBS",
};

static const char *ax650x_board_id_to_board_name(int id)
{
	static const char *board_name[AX650_BOARD_MAX] = {
		[AX650A_Demo] = "AX650A_Demo",
		[AX650N_Demo] = "AX650N_Demo",
		[AX650A_EVB] = "AX650A_EVB",
		[AX650N_EVB] = "AX650N_EVB",
		[AX650A_SLT] = "AX650A_SLT",
		[AX650N_SLT] = "AX650N_SLT",
		[AX650N_PCIE] = "AX650N_PCIE",
	};

	if (AX650C_CHIP == ax_get_chip_type()) {
		board_name[AX650N_Demo] = "AX650C_Demo";
		board_name[AX650N_PCIE] = "AX650C_PCIE";
	}

	if (id >= 0 && id < AX650_BOARD_MAX) {
		return board_name[id];
	} else {
		return "Unknow board id";
	}
}

static const char *ax750_board_id_to_board_name(int id)
{
#define AX750_BOARD_MAX 2
	static const char *board_name[AX750_BOARD_MAX] = {
		[0] = "AX750_XPILOT_6V_alpha",
		[1] = "AX750_XPILOT_6V_beta",
	};

	if (id >= 0 && id < AX750_BOARD_MAX) {
		return board_name[id];
	} else {
		return "Unknow board id";
	}
}

static int ax_get_uid_from_efuse(void)
{
	int ret;
	void __iomem *efuse0_base = NULL;
	efuse0_base = ioremap(EFSC0_BASE, EFUSE_REG_LEN);
	if (!efuse0_base) {
		//ax_pr_err("remap EFSC0_BASE failed\n");
		return -1;
	}

	ret = ax_efuse_read(efuse0_base, 0, &ax_uid_l);
	if (ret != efuse_success) {
		iounmap(efuse0_base);
		return -1;
	}
	ret = ax_efuse_read(efuse0_base, 1, &ax_uid_h);
	if (ret != efuse_success) {
		iounmap(efuse0_base);
		return -1;
	}
	//ax_pr_info("ax_uid: 0x%x%08x\n", (ax_uid_h>>28), ax_uid_l);

	iounmap(efuse0_base);
	return 1;
}
u32 ax_get_board_id(void)
{
	u32 value;
	void __iomem *board_id_addr = 0;
	board_id_addr = ioremap(COMM_SYS_DUMMY_SW1, 0x4);
	value = readl(board_id_addr);
	board_id = (value & 0xffff0000) >> 16;
	adap_plate_id = (value & 0xffff);
	iounmap(board_id_addr);
	return value;
}
EXPORT_SYMBOL(ax_get_board_id);

u32 ax_get_chip_type(void)
{
	void __iomem *chip_type_addr = 0;
	chip_type_addr = ioremap(COMM_SYS_DUMMY_SW2, 0x4);
	chip_type = readl(chip_type_addr);
	iounmap(chip_type_addr);
	return chip_type;
}
EXPORT_SYMBOL(ax_get_chip_type);

static int ax_uid_proc_show(struct seq_file *m, void *v)
{
	seq_printf(m, "ax_uid: 0x%08x%08x\n", ax_uid_h, ax_uid_l);
	return 0;
}
static int ax_uid_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, ax_uid_proc_show, NULL);
}

static struct proc_ops ax_uid_fops = {
	.proc_open   = ax_uid_proc_open,
	.proc_release = single_release,
	.proc_read   = seq_read,
	.proc_lseek = seq_lseek,
};

static int ax_uid_create_proc_file(void)
{
	uid_file = proc_create_data(AX_UID_NODE_NAME, 0644, NULL, &ax_uid_fops, NULL);
	if (!uid_file) {
		printk("uid proc_create_data failed\n");
		return -ENOMEM;
	}
	return 0;
}

static int ax_board_id_proc_show(struct seq_file *m, void *v)
{
	if (chip_type == AX750_CHIP)
		seq_printf(m, "%s\n", ax750_board_id_to_board_name(board_id));
	else
		seq_printf(m, "%s\n", ax650x_board_id_to_board_name(board_id));
	return 0;
}

static int ax_adp_plate_id_proc_show(struct seq_file *m, void *v)
{
	seq_printf(m, "%s\n", adp_plate_name[adap_plate_id]);
	return 0;
}

static int ax_board_id_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, ax_board_id_proc_show, NULL);
}

static int ax_adp_plate_id_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, ax_adp_plate_id_proc_show, NULL);
}

static struct proc_ops ax_board_id_fops = {
	.proc_open   = ax_board_id_proc_open,
	.proc_release = single_release,
	.proc_read   = seq_read,
	.proc_lseek = seq_lseek,
};

static struct proc_ops ax_adp_plate_id_fops = {
	.proc_open   = ax_adp_plate_id_proc_open,
	.proc_release = single_release,
	.proc_read   = seq_read,
	.proc_lseek = seq_lseek,
};

static int ax_board_id_create_proc_file(void)
{
	board_id_file = proc_create_data(AX_BOARD_ID_NODE_NAME, 0644, NULL, &ax_board_id_fops, NULL);
	if (!board_id_file) {
		printk("board_id proc_create_data failed\n");
		return -ENOMEM;
	}
	adp_plate_id_file = proc_create_data(AX_ADP_PLATE_ID_NODE_NAME, 0644, NULL, &ax_adp_plate_id_fops, NULL);
	if (!adp_plate_id_file) {
		printk("adp_plate_id_file proc_create_data failed\n");
		return -ENOMEM;
	}
	return 0;
}

static int ax_chip_type_proc_show(struct seq_file *m, void *v)
{
	seq_printf(m, "%s\n", chip_type_name[chip_type]);
	return 0;
}
static int ax_chip_type_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, ax_chip_type_proc_show, NULL);
}

static struct proc_ops ax_chip_type_fops = {
	.proc_open   = ax_chip_type_proc_open,
	.proc_release = single_release,
	.proc_read   = seq_read,
	.proc_lseek = seq_lseek,
};

static int ax_chip_type_create_proc_file(void)
{
	chip_type_file = proc_create_data(AX_CHIP_TYPE_NODE_NAME, 0644, NULL, &ax_chip_type_fops, NULL);
	if (!chip_type_file) {
		printk("chip_type proc_create_data failed\n");
		return -ENOMEM;
	}
	return 0;
}


static int adc_open(struct inode *inode, struct file *filp)
{
	pr_debug("enter %s\n", __func__);
	return 0;
}

static long adc_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	int ret = 0;
	struct adc_value adc_value;
	void __user *argp = (void __user *)arg;

	switch (cmd) {
	case SET_ADC_CHAN:
		ret = copy_from_user(&enable_chan, argp, sizeof(u32));
		if (ret != 0) {
			ret = -EFAULT;
		}
		break;
	case GET_ADC_VALUE:
		memset(&adc_value, 0, sizeof(struct adc_value));
		read_adc_value(enable_chan, &adc_value);
		ret = copy_to_user(argp, &adc_value, sizeof(struct adc_value));
		if (ret != 0) {
			ret = -EFAULT;
		}
		enable_chan = 0;
		break;
	default:
		ret = -EINVAL;
	}

	return ret;
}

static const struct file_operations adc_fops = {
	.owner = THIS_MODULE,
	.open = adc_open,
	.unlocked_ioctl = adc_ioctl,
};

static struct miscdevice ax_adc_miscdev = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "axera_adc",
	.fops = &adc_fops
};

static int register_adc_dev(void)
{
	misc_register(&ax_adc_miscdev);
	return 0;
}

static int unregister_adc_dev(void)
{
	misc_deregister(&ax_adc_miscdev);
	return 0;
}

static int ax_hwinfo_probe(struct platform_device *pdev)
{
	ax_board_id_create_proc_file();
	ax_chip_type_create_proc_file();
	ax_uid_create_proc_file();
	ax_get_uid_from_efuse();
	ax_get_board_id();
	ax_get_chip_type();
	register_adc_dev();
	return 0;
}
static int ax_hwinfo_remove(struct platform_device *pdev)
{
	unregister_adc_dev();
	remove_proc_entry(AX_UID_NODE_NAME, NULL);
	remove_proc_entry(AX_CHIP_TYPE_NODE_NAME, NULL);
	remove_proc_entry(AX_ADP_PLATE_ID_NODE_NAME, NULL);
	remove_proc_entry(AX_BOARD_ID_NODE_NAME, NULL);
	return 0;
}
static const struct of_device_id ax_hwinfo_of_match[] = {
	{.compatible = "ax,ax_hwinfo" },
	{ }
};
static struct platform_driver ax_hwinfo_driver = {
	.probe = ax_hwinfo_probe,
	.remove = ax_hwinfo_remove,
	.driver = {
		.name  = KBUILD_MODNAME,
		.of_match_table = ax_hwinfo_of_match,
	},
};
static int __init ax_hwinfo_init(void)
{
	platform_driver_register(&ax_hwinfo_driver);
	return 0;
}
static void ax_hwinfo_exit(void)
{
	platform_driver_unregister(&ax_hwinfo_driver);
}
late_initcall(ax_hwinfo_init);
module_exit(ax_hwinfo_exit);
MODULE_DESCRIPTION("axera ax_hwinfo driver");
MODULE_AUTHOR("Axera Inc.");
MODULE_LICENSE("GPL v2");
