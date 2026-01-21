/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#ifndef __rtl9068_H
#define __rtl9068_H

#define rtl9068_NUM_PORTS		8

 /* MDX Master */
typedef enum {
    RTK_MDX_PREAMBLE_0   = 0, /* 0-bit preamble */
    RTK_MDX_PREAMBLE_8   = 1, /* 8-bit preamble */
    RTK_MDX_PREAMBLE_16  = 2, /* 16-bit preamble */
    RTK_MDX_PREAMBLE_32  = 3, /* 32-bit preamble */
    RTK_MDX_PREAMBLE_END
} rtk_mdx_preamble_t;

struct rtl9068_priv;

/* struct rtl9068_info -	This is the main data structure for holding the specific
 *			part for each supported device
 * @sw_setup:		Holding the handler to a device initialization
 * @phy_read:		Holding the way reading PHY port
 * @phy_write:		Holding the way writing PHY port
 * @pad_setup:		Holding the way setting up the bus pad for a certain
 *			MAC port
 * @phy_mode_supported:	Check if the PHY type is being supported on a certain
 *			port
 * @mac_port_validate:	Holding the way to set addition validate type for a
 *			certan MAC port
 * @mac_port_get_state: Holding the way getting the MAC/PCS state for a certain
 *			MAC port
 * @mac_port_config:	Holding the way setting up the PHY attribute to a
 *			certain MAC port
 * @mac_pcs_an_restart	Holding the way restarting PCS autonegotiation for a
 *			certain MAC port
 * @mac_pcs_link_up:	Holding the way setting up the PHY attribute to the pcs
 *			of the certain MAC port
 */
struct rtl9068_info {
	int (*phy_read)(struct rtl9068_priv *priv, int port, int regnum);
	int (*phy_write)(struct rtl9068_priv *priv, int port, int regnum, u16 val);
};

/* struct rtl9068_priv -	This is the main data structure for holding the state
 *			of the driver
 * @dev:		The device pointer
 * @ds:			The pointer to the dsa core structure
 * @bus:		The bus used for the device and built-in PHY
 * @rstc:		The pointer to reset control used by MCM
 * @core_pwr:		The power supplied into the core
 * @io_pwr:		The power supplied into the I/O
 * @reset:		The descriptor for GPIO line tied to its reset pin
 * @mcm:		Flag for distinguishing if standalone IC or module
 *			coupling
 * @ports:		Holding the state among ports
 * @reg_mutex:		The lock for protecting among process accessing
 *			registers
 * @p6_interface	Holding the current port 6 interface
 * @p5_intf_sel:	Holding the current port 5 interface select
 *
 * @irq:		IRQ number of the switch
 * @irq_domain:		IRQ domain of the switch irq_chip
 * @irq_enable:		IRQ enable bits, synced to SYS_INT_EN
 */
struct rtl9068_priv {
	struct udevice           *parent;
	struct udevice		*dev;
	struct mii_dev		*phy_bus;
	const char		*bus;
	const struct rtl9068_info *info;
	unsigned int		id;
};

#endif /* __rtl9068_H */
