/*
 * Copyright (c) 2022 AXERA in AX650 project.
 *
 * SPDX-License-Identifier:     GPL-2.0+
 */
#include <common.h>

#define AX750_BOARD_MAX 2
static const char * board_name[AX750_BOARD_MAX] = {
	[0] = "AX750A_XPILOT_6V_alpha",
	[1] = "AX750A_XPILOT_6V_beta",
};

const char* board_id_to_name(int id)
{
	if (id >= 0 && id < AX750_BOARD_MAX) {
		return board_name[id];
	}
	return "Unknow board ID";
}
