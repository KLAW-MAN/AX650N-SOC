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

from ctypes import Structure, c_void_p, c_uint32, c_int32, c_uint64, c_char
import os
import sys

BASE_DIR = os.path.dirname(os.path.abspath(__file__))
sys.path.append(BASE_DIR)

from axcl.axcl_base import *
from axcl.utils.axcl_basestructure import *

class axclrtDeviceProperties(BaseStructure):
    """
    .. parsed-literal::

        dict_device_properties = {
            "sw_version": [int],
            "uid": int,
            "pci_domain": int,
            "pci_bus_id": int,
            "pci_device_id": int,
            "temperature": int,
            "total_mem_size": int,
            "free_mem_size": int,
            "total_cmm_size": int,
            "free_cmm_size": int,
            "cpu_loading": int,
            "npu_loading": int,
            "reserved": [int]
        }
    """
    _fields_ = [
        ("swVersion", c_uint8 * 64),
        ("uid", c_uint64),
        ("pciDomain", c_uint32),
        ("pciBusID", c_uint32),
        ("pciDeviceID", c_uint32),
        ("temperature", c_int32),
        ("totalMemSize", c_uint32),
        ("freeMemSize", c_uint32),
        ("totalCmmSize", c_uint32),
        ("freeCmmSize", c_uint32),
        ("cpuLoading", c_uint32),
        ("npuLoading", c_uint32),
        ("reserved", c_uint32 * 32)
    ]

    field_aliases = {
        "swVersion": "sw_version",
        "uid": "uid",
        "pciDomain": "pci_domain",
        "pciBusID": "pci_bus_id",
        "pciDeviceID": "pci_device_id",
        "temperature": "temperature",
        "cpuLoading": "cpu_loading",
        "npuLoading": "npu_loading",
        "totalMemSize": "total_mem_size",
        "freeMemSize": "free_mem_size",
        "totalCmmSize": "total_cmm_size",
        "freeCmmSize": "free_cmm_size",
        "reserved": "reserved"
    }


class axclrtDeviceList(Structure):
    """
    .. parsed-literal::

        dict_device_list = {
            "num": int,
            "devices": [int]
        }
    """
    _fields_ = [
        ("num", c_uint32),
        ("devices", c_int32 * AXCL_MAX_DEVICE_COUNT)
    ]

    field_aliases = {
        "num": "num",
        "devices": "devices"
    }


axclrtMemMallocPolicy = c_int32
"""
    Memory malloc policy

    .. parsed-literal::

        AXCL_MEM_MALLOC_HUGE_FIRST = 0
        AXCL_MEM_MALLOC_HUGE_ONLY = 1
        AXCL_MEM_MALLOC_NORMAL_ONLY = 2
"""
AXCL_MEM_MALLOC_HUGE_FIRST = 0
AXCL_MEM_MALLOC_HUGE_ONLY = 1
AXCL_MEM_MALLOC_NORMAL_ONLY = 2


axclrtMemcpyKind = c_int32
"""
    Memory copy kind

    .. parsed-literal::

        AXCL_MEMCPY_HOST_TO_HOST = 0
        AXCL_MEMCPY_HOST_TO_DEVICE = 1
        AXCL_MEMCPY_DEVICE_TO_HOST = 2
        AXCL_MEMCPY_DEVICE_TO_DEVICE = 3
        AXCL_MEMCPY_HOST_PHY_TO_DEVICE = 4
        AXCL_MEMCPY_DEVICE_TO_HOST_PHY = 5
"""
AXCL_MEMCPY_HOST_TO_HOST = 0
AXCL_MEMCPY_HOST_TO_DEVICE = 1
AXCL_MEMCPY_DEVICE_TO_HOST = 2
AXCL_MEMCPY_DEVICE_TO_DEVICE = 3
AXCL_MEMCPY_HOST_PHY_TO_DEVICE = 4
AXCL_MEMCPY_DEVICE_TO_HOST_PHY = 5
