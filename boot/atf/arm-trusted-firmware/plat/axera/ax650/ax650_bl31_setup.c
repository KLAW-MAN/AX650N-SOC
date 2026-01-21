/*
 * Copyright (c) 2015-2021, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <assert.h>

#include <arch.h>
#include <arch_helpers.h>
#include <common/bl_common.h>
#include <common/debug.h>
#include <drivers/console.h>
#include <lib/debugfs.h>
#include <lib/extensions/ras.h>
#if ENABLE_RME
#include <lib/gpt_rme/gpt_rme.h>
#endif
#include <lib/mmio.h>
#include <lib/xlat_tables/xlat_tables_compat.h>
#include <ax650_def.h>
#include <plat/common/platform.h>
#include <platform_def.h>
#include <drivers/ti/uart/uart_16550.h>
#include <plat_ax650.h>
#include <ax650_pwrc.h>
/*
 * Placeholder variables for copying the arguments that have been passed to
 * BL31 from BL2.
 */
static entry_point_info_t bl32_image_ep_info;
static entry_point_info_t bl33_image_ep_info;
#if ENABLE_RME
static entry_point_info_t rmm_image_ep_info;
#endif
static console_t console;

#if test
#define MAP_BL31_TOTAL		MAP_REGION_FLAT(			\
					BL31_START,			\
					BL31_END - BL31_START,		\
					MT_CODE | MT_RW | EL3_PAS)
#else
#define MAP_BL31_TOTAL		MAP_REGION_FLAT(			\
					AON_SYS_IRAME_BASE,			\
					AON_SYS_IRAME_SIZE,		\
					MT_CODE | MT_RW | EL3_PAS)
#endif

/*******************************************************************************
 * Return a pointer to the 'entry_point_info' structure of the next image for the
 * security state specified. BL33 corresponds to the non-secure image type
 * while BL32 corresponds to the secure image type. A NULL pointer is returned
 * if the image does not exist.
 ******************************************************************************/
struct entry_point_info *bl31_plat_get_next_image_ep_info(uint32_t type)
{
	entry_point_info_t *next_image_info;

	assert(sec_state_is_valid(type));
	if (type == NON_SECURE) {
		next_image_info = &bl33_image_ep_info;
	}
#if ENABLE_RME
	else if (type == REALM) {
		next_image_info = &rmm_image_ep_info;
	}
#endif
	else {
		next_image_info = &bl32_image_ep_info;
	}

	/*
	 * None of the images on the ARM development platforms can have 0x0
	 * as the entrypoint
	 */
	if (next_image_info->pc)
		return next_image_info;
	else
		return NULL;
}

uintptr_t ax650_get_ns_image_entrypoint(void)
{
#ifdef PRELOADED_BL33_BASE
	return PRELOADED_BL33_BASE;
#else
	return 0ULL;
#endif
}

uint32_t arm_get_spsr_for_bl33_entry(void)
{
	unsigned int mode;
	uint32_t spsr;

	/* Figure out what mode we enter the non-secure world in */
	mode = (el_implemented(2) != EL_IMPL_NONE) ? MODE_EL2 : MODE_EL1;

	/*
	 * TODO: Consider the possibility of specifying the SPSR in
	 * the FIP ToC and allowing the platform to have a say as
	 * well.
	 */
	spsr = SPSR_64((uint64_t)mode, MODE_SP_ELX, DISABLE_ALL_EXCEPTIONS);
	return spsr;
}

/*******************************************************************************
 * Perform any BL31 early platform setup common to ARM standard platforms.
 * Here is an opportunity to copy parameters passed by the calling EL (S-EL1
 * in BL2 & EL3 in BL1) before they are lost (potentially). This needs to be
 * done before the MMU is initialized so that the memory layout can be used
 * while creating page tables. BL2 has flushed this information to memory, so
 * we are guaranteed to pick up good data.
 ******************************************************************************/
void __init ax650_bl31_early_platform_setup(void *from_bl2, uintptr_t soc_fw_config,
				uintptr_t hw_config, void *plat_params_from_bl2)
{
	/* Initialize the console to provide early debug support */
	console_16550_register(UART0_BASE, AX650_UART_CLOCK,
			       AX650_BAUDRATE, &console);

#if RESET_TO_BL31
	/* There are no parameters from BL2 if BL31 is a reset vector */
	assert(from_bl2 == NULL);
	assert(plat_params_from_bl2 == NULL);

# ifdef BL32_BASE
	/* Populate entry point information for BL32 */
	SET_PARAM_HEAD(&bl32_image_ep_info,
				PARAM_EP,
				VERSION_1,
				0);
	SET_SECURITY_STATE(bl32_image_ep_info.h.attr, SECURE);
	bl32_image_ep_info.pc = BL32_BASE;
	bl32_image_ep_info.spsr = 0;

#if defined(SPD_spmd)
	/* SPM (hafnium in secure world) expects SPM Core manifest base address
	 * in x0, which in !RESET_TO_BL31 case loaded after base of non shared
	 * SRAM(after 4KB offset of SRAM). But in RESET_TO_BL31 case all non
	 * shared SRAM is allocated to BL31, so to avoid overwriting of manifest
	 * keep it in the last page.
	 */
	bl32_image_ep_info.args.arg0 = ARM_TRUSTED_SRAM_BASE +
				PLAT_ARM_TRUSTED_SRAM_SIZE - PAGE_SIZE;
#endif

# endif /* BL32_BASE */

	/* Populate entry point information for BL33 */
	SET_PARAM_HEAD(&bl33_image_ep_info,
				PARAM_EP,
				VERSION_1,
				0);
	/*
	 * Tell BL31 where the non-trusted software image
	 * is located and the entry state information
	 */
	bl33_image_ep_info.pc = ax650_get_ns_image_entrypoint();

	bl33_image_ep_info.spsr = arm_get_spsr_for_bl33_entry();
	SET_SECURITY_STATE(bl33_image_ep_info.h.attr, NON_SECURE);

#else /* RESET_TO_BL31 */

	/*
	 * In debug builds, we pass a special value in 'plat_params_from_bl2'
	 * to verify platform parameters from BL2 to BL31.
	 * In release builds, it's not used.
	 */
	assert(((unsigned long long)plat_params_from_bl2) ==
		ARM_BL31_PLAT_PARAM_VAL);

	/*
	 * Check params passed from BL2 should not be NULL,
	 */
	bl_params_t *params_from_bl2 = (bl_params_t *)from_bl2;
	assert(params_from_bl2 != NULL);
	assert(params_from_bl2->h.type == PARAM_BL_PARAMS);
	assert(params_from_bl2->h.version >= VERSION_2);

	bl_params_node_t *bl_params = params_from_bl2->head;

	/*
	 * Copy BL33, BL32 and RMM (if present), entry point information.
	 * They are stored in Secure RAM, in BL2's address space.
	 */
	while (bl_params != NULL) {
		if (bl_params->image_id == BL32_IMAGE_ID) {
			bl32_image_ep_info = *bl_params->ep_info;
		}
#if ENABLE_RME
		else if (bl_params->image_id == RMM_IMAGE_ID) {
			rmm_image_ep_info = *bl_params->ep_info;
		}
#endif
		else if (bl_params->image_id == BL33_IMAGE_ID) {
			bl33_image_ep_info = *bl_params->ep_info;
		}

		bl_params = bl_params->next_params_info;
	}

	if (bl33_image_ep_info.pc == 0U)
		panic();
#if ENABLE_RME
	if (rmm_image_ep_info.pc == 0U)
		panic();
#endif
#endif /* RESET_TO_BL31 */

# if ARM_LINUX_KERNEL_AS_BL33
	/*
	 * According to the file ``Documentation/arm64/booting.txt`` of the
	 * Linux kernel tree, Linux expects the physical address of the device
	 * tree blob (DTB) in x0, while x1-x3 are reserved for future use and
	 * must be 0.
	 * Repurpose the option to load Hafnium hypervisor in the normal world.
	 * It expects its manifest address in x0. This is essentially the linux
	 * dts (passed to the primary VM) by adding 'hypervisor' and chosen
	 * nodes specifying the Hypervisor configuration.
	 */
#if RESET_TO_BL31
	bl33_image_ep_info.args.arg0 = (u_register_t)ARM_PRELOADED_DTB_BASE;
#else
	bl33_image_ep_info.args.arg0 = (u_register_t)hw_config;
#endif
	bl33_image_ep_info.args.arg1 = 0U;
	bl33_image_ep_info.args.arg2 = 0U;
	bl33_image_ep_info.args.arg3 = 0U;
# endif
}

void bl31_early_platform_setup2(u_register_t arg0, u_register_t arg1,
		u_register_t arg2, u_register_t arg3)
{
	ax650_bl31_early_platform_setup((void *)arg0, arg1, arg2, (void *)arg3);
}

/*******************************************************************************
 * Perform any BL31 platform runtime setup prior to BL31 exit common to ARM
 * standard platforms
 * Perform BL31 platform setup
 ******************************************************************************/
void ax650_bl31_plat_runtime_setup(void)
{
#if 0
	console_switch_state(CONSOLE_FLAG_RUNTIME);

	/* Initialize the runtime console */
	arm_console_runtime_init();

#if RECLAIM_INIT_CODE
	arm_free_init_memory();
#endif

#if PLAT_RO_XLAT_TABLES
	arm_xlat_make_tables_readonly();
#endif
#endif
}

void bl31_platform_setup(void)
{
	/* Initialize the GIC driver, cpu and distributor interfaces */
	plat_ax650_gic_driver_init();
	plat_ax650_gic_init();

	/* Enable and initialize the System level generic timer */
	mmio_write_32(FLASH_SW_RST_0_CLR, BIT_GTIMR_SW_RST_CLR);
	mmio_write_32(FLASH_CLK_EB_0_SET, BIT_CLK_GTMR_EB_SET);
	mmio_write_32(GTMR_FID, SYS_COUNTER_FREQ);
	mmio_write_32(GTMR_CNTCR, BIT_GTMR_EN);

	/* Initialize power controller before setting up topology */
	plat_ax650_pwrc_setup();
}

void bl31_plat_runtime_setup(void)
{
}

const mmap_region_t plat_ax650_mmap[] = {
	//MAP_REGION_FLAT(AON_SYS_BASE, AON_SYS_SIZE,
	//	MT_DEVICE | MT_RW | MT_NS),
	MAP_REGION_FLAT(AON_SYS_BF_IRAME_BASE, AON_SYS_BF_IRAME_SIZE,
		MT_DEVICE | MT_RW | MT_NS),
	MAP_REGION_FLAT(GIC400_BASE, GIC400_SIZE,
		MT_DEVICE | MT_RW | MT_SECURE),
	MAP_REGION_FLAT(AON_SYS_AF_IRAME_BASE, AON_SYS_AF_IRAME_SIZE,
		MT_DEVICE | MT_RW | MT_NS),
	MAP_REGION_FLAT(PERI_SYS_BASE, PERI_SYS_SIZE,
		MT_DEVICE | MT_RW | MT_NS),
	MAP_REGION_FLAT(FLASH1_SYS_BASE, FLASH1_SYS_SIZE,
		MT_DEVICE | MT_RW | MT_NS),
	MAP_REGION_FLAT(DDR0_SYS_BASE, DDR0_SYS_SIZE,
		MT_DEVICE | MT_RW | MT_NS),
	MAP_REGION_FLAT(DDR0_PHY_BASE, DDR0_PHY_SIZE,
		MT_DEVICE | MT_RW | MT_NS),
	MAP_REGION_FLAT(DDR1_SYS_BASE, DDR1_SYS_SIZE,
		MT_DEVICE | MT_RW | MT_NS),
	MAP_REGION_FLAT(DDR1_PHY_BASE, DDR1_PHY_SIZE,
		MT_DEVICE | MT_RW | MT_NS),
	{0},
};

/*******************************************************************************
 * Perform the very early platform specific architectural setup shared between
 * ARM standard platforms. This only does basic initialization. Later
 * architectural setup (bl31_arch_setup()) does not do anything platform
 * specific.
 ******************************************************************************/
static void ax650_bl31_plat_arch_setup(void)
{
#if 1   //todo check more
	const mmap_region_t bl_regions[] = {
		MAP_REGION_FLAT(AON_SYS_IRAME_BASE,  AON_SYS_IRAME_SIZE, MT_MEMORY | MT_RW | MT_SECURE),
		MAP_REGION_FLAT(BL_CODE_BASE, BL_CODE_END - BL_CODE_BASE, MT_CODE | MT_RO | MT_SECURE),
		MAP_REGION_FLAT(BL_RO_DATA_BASE, BL_RO_DATA_END - BL_RO_DATA_BASE, MT_RO_DATA | MT_RO | MT_SECURE),
		{0}
	};

	setup_page_tables(bl_regions, plat_ax650_mmap);

	enable_mmu_el3(0);
#endif
}

void bl31_plat_arch_setup(void)
{
	ax650_bl31_plat_arch_setup();
}
