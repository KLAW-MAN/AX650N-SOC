/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

OUTPUT_FORMAT("elf64-littleaarch64", "elf64-littleaarch64", "elf64-littleaarch64")
OUTPUT_ARCH(aarch64)
ENTRY(_start)

MEMORY
{
	iRam (rw) : ORIGIN = 0x04800400, LENGTH = 0x1f000
}

SECTIONS
{
	.text :
	{
		*start.o
		*(.text)
	} > iRam

	. = ALIGN(4);
	.rodata :
	{
		*(.rodata)
		*(.rodata.str1.4)
		. = ALIGN(4);
	} > iRam

	. = ALIGN(4);
	.data :
	{
		_data_start = . ;
		*(.data)
		. = ALIGN(4);
		_data_end = . ;
	} > iRam

	. = ALIGN(4);
	_bss_start = . ;
	.bss :
	{
		*(.bss)
		. = ALIGN(4);
	} > iRam
	_bss_end = . ;

	. = ALIGN(4);
	.stack 0x4818800:
	{
		_stacktop = . ;
	} > iRam
}
