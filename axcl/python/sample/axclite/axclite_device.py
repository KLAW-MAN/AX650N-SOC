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
import axcl
from axclite.axclite_resource import AxcliteResource


class AxcliteDevice(AxcliteResource):
    def __init__(self):
        super().__init__(self.__class__.__name__)
        self._device_id = 0
        self._inited = False

    @property
    def device_id(self):
        return self._device_id

    def create(self, device_index) -> bool:
        devices, ret = axcl.rt.get_device_list()
        if ret != axcl.AXCL_SUCC:
            print(f"get connected the list of devices fail, ret = 0x{ret&0xFFFFFFFF:x}")
            return False

        device_num = len(devices)
        if device_num == 0:
            print("no device is connected")
            return False

        if device_index >= device_num:
            print(f"device index: {device_index} is out of connected device number {device_num}")
            return False

        self._device_id = devices[device_index]
        print(f"device index: {device_index}, bus number: {self._device_id}")

        ret = axcl.rt.set_device(self._device_id)
        if axcl.AXCL_SUCC != ret:
            print(f"set device(id: {self._device_id}) fail, ret = 0x{ret&0xFFFFFFFF:x}")
            return False

        self._inited = True
        return True

    def destroy(self):
        if self._inited:
            axcl.rt.reset_device(self._device_id)
            self._inited = False
            self._device_id = 0
