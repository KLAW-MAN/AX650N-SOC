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
            'ddr_num': board.get('ddr_num', 2),
            'rank_num': board.get('rank_num', 2),
            'lpdd4x': board.get('lpdd4x', 1)
        }
        boards[board_id] = config

    return boards if len(boards) > 0 else None


def write_board_config(boards, board_file):
    content = "/*\n* board_id, ddr_num, rank_num, lpdd4x\n*/\n"
    if boards is None:
        with open(board_file, 'w') as f:
            f.write(content)
        return

    for v in boards.values():
        content += f"{{{v['board_id']:8},{v['ddr_num']:8},{v['rank_num']:8},{v['lpdd4x']:8}}},\n"

    # content = content.rstrip(',\n')
    with open(board_file, 'w') as f:
        f.write(content)


if __name__ == '__main__':
    curr_file = os.path.dirname(os.path.abspath(__file__))
    json_file = os.path.normpath(os.path.join(curr_file, '../../../build/projects/AX650_card_board.json'))
    conf_file = os.path.normpath(os.path.join(curr_file, '../driver/ddr/ddr_board_config.h'))
    print(f"\033[92mjson file: {json_file}\033[0m")
    print(f"\033[92mconf file: {conf_file}\033[0m")

    boards = load_board_config(json_file)
    write_board_config(boards, conf_file)
