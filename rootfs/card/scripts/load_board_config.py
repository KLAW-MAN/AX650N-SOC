#!/usr/bin/env python3
# -*- coding:utf-8 -*-

import json
import os


def load_board_config(json_file):
    with open(json_file, 'r') as f:
        content = json.load(f)

    if 'boards' not in content:
        return None

    boards = {}
    for board in content['boards']:
        if 'board_id' not in board:
            print(f"\033[91mmissing board_id in {json_file}\033[0m")
            continue

        board_id = board['board_id']
        config = {
            'board_id': board_id,
            'cmm': board.get('cmm', 'anonymous,0,0x148000000,3072M')
        }
        boards[board_id] = config

    return boards if len(boards) > 0 else None


def write_board_config(boards, board_file):
    content = """# This file defined the cmm config for each board, the format is:
#    board id = cmm config
#
# `cmm config` should follow the sys ko rules of below:
#    partitonname,flag,phyaddr_start,partitionsize[:partitonname,flag,phyaddr_start, partitionsize]
#
#  When insmod the sys ko, `auto_load_all_drv.sh` will get the board id from `COMM_SYS_DUMMY_SW1` register at first.
#  Then try to find the cmm configuration of current board id in this file.
#  1. If board id is not defined in this file:
#     - if `CMM_POOL_PARAM` is defined in `build/projects/AX650_card.mak`, used the configuration defined by `CMM_POOL_PARAM`
#     - otherwise, used the default configuration defined by `DEFAULT_CMM_POOL_CONFIG` in `auto_load_all_drv.sh`.
#  2. If board id is defined in this file: used the configuration of specified board id.
#  3. Refer to `auto_load_all_drv.sh` for more details.
#
#
# Example:
#   1 = anonymous,0,0x148000000,7040M
"""
    if boards is None:
        with open(board_file, 'w') as f:
            f.write(content)
        return

    for v in boards.values():
        content += f"{v['board_id']} = {v['cmm']}\n"

    with open(board_file, 'w') as f:
        f.write(content)


if __name__ == '__main__':
    curr_file = os.path.dirname(os.path.abspath(__file__))
    json_file = os.path.normpath(os.path.join(curr_file, '../../../build/projects/AX650_card_board.json'))
    conf_file = os.path.normpath(os.path.join(curr_file, '../rootfs/soc/scripts/board_cmm.conf'))
    print(f"\033[92mjson file: {json_file}\033[0m")
    print(f"\033[92mconf file: {conf_file}\033[0m")

    boards = load_board_config(json_file)
    write_board_config(boards, conf_file)
