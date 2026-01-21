#
# AXERA
#
# SPDX-License-Identifier:	GPL-2.0+
# https://spdx.org/licenses

ifeq ($(debugkconfig), yes)
EXTRA_CFLAGS   +=  -DAXERA_DEBUG_KCONFIG
endif

# download
obj-$(CONFIG_CMD_AXERA_DOWNLOAD) += download/download.o
obj-$(CONFIG_CMD_AXERA_DOWNLOAD) += download/fdl_channel.o
obj-$(CONFIG_CMD_AXERA_DOWNLOAD) += download/fdl_channel_serial.o
obj-$(CONFIG_CMD_AXERA_DOWNLOAD) += download/fdl_engine.o
obj-$(CONFIG_CMD_AXERA_DOWNLOAD) += download/fdl_frame.o
obj-$(CONFIG_CMD_AXERA_DOWNLOAD) += download/fdl_usb_driver.o
obj-$(CONFIG_CMD_AXERA_DOWNLOAD) += download/fdl_channel_usb.o
obj-$(CONFIG_CMD_AXERA_DOWNLOAD) += download/sparse_img.o
obj-$(CONFIG_AXERA_SECURE_BOOT) += secureboot/secureboot.o

# memtest
obj-$(CONFIG_CMD_AXERA_MEMTEST)  += memtest/tests.o
obj-$(CONFIG_CMD_AXERA_MEMTEST)  += memtest/memtester.o

# ddr dfs
obj-$(CONFIG_CMD_AXERA_BOOT) += ddr_dfs/ddr_dfs.o

# boot os
obj-$(CONFIG_CMD_AXERA_BOOT) += boot/axera_boot.o

# sd update
obj-$(CONFIG_CMD_AXERA_SDUPDATE) += sd_update/sd_update.o
obj-$(CONFIG_CMD_AXERA_SDBOOT) += sd_boot/sd_boot.o
obj-$(CONFIG_CMD_AXERA_TFTP_OTA) += tftp_update/axera_ota.o
obj-$(CONFIG_CMD_AXERA_UPDATE) += update/axera_update.o
obj-y += setup_boot/setup_boot.o
obj-$(CONFIG_CMD_AXERA_CIPHER) += cipher/eip130_drv.o cipher/ax_cipher_api.o cipher/sample_cipher.o cipher/sample_hash.o cipher/sample_trng.o cipher/sample_rsa.o
obj-$(CONFIG_CMD_AXERA_CIPHER) += cipher/ax_keyotpblob.o cipher/ax_aessiv.o cipher/sample_huk.o
obj-$(CONFIG_CMD_AXERA_EMMC_SCAN) += emmc_scan/emmc_scan.o

obj-$(CONFIG_CMD_AXERA_BOOT_RISCV) += riscv/boot_riscv.o
obj-$(CONFIG_CMD_AXERA_USB_STOR_UPDATE) += usb_stor_update/usb_storage_update.o
