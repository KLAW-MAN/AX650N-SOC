# !/usr/bin/env python
# -*- coding:utf-8 -*-
# ******************************************************************************
#
#  Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
#
#  This source file is the property of Axera Semiconductor Co., Ltd. and
#  may not be copied or distributed in any isomorphic form without the prior
#  written consent of Axera Semiconductor Co., Ltd.
#
# ******************************************************************************

import argparse
import os
import sys
import threading
import traceback

BASE_DIR = os.path.dirname(os.path.abspath(__file__))
sys.path.append(BASE_DIR + '/..')
sys.path.append(BASE_DIR + '/../..')

from axclite.axclite_device import AxcliteDevice
from axclite.axclite_context import AxcliteContext
from axclite.axclite_system import axclite_system


def main(device_index):
    device = AxcliteDevice()
    if not device.create(device_index):
        return 1

    device_id = device.device_id
    print(f"device {device_id} is active")

    def worker(device_):
        # 3: create context for work thread
        _context = AxcliteContext()
        _context.create(device_)
        # TODO: add device task here

        # 4: destroy context before thread quit
        _context.destroy()

    t = threading.Thread(target=worker, args=(device_id,))
    t.start()
    t.join()

    device.destroy()
    print(f"device {device_id} is inactive")


if __name__ == '__main__':
    print(f"============== sample runtime started ==============")

    parser = argparse.ArgumentParser()
    parser.add_argument('-d', '--device', type=int, default=0,
                        help="device index from 0 to connected device num - 1")
    parser.add_argument('--json', type=str, default='/usr/bin/axcl/axcl.json', help="axcl.json path")
    args = parser.parse_args()
    device_index = args.device
    json = args.json

    try:
        with axclite_system(json):
            main(device_index)
    except:
        print(sys.exc_info())
        print(traceback.format_exc())

    print("============== sample runtime exited ==============")
