/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#ifndef __AXCL_MODULE_VERSION_H__
#define __AXCL_MODULE_VERSION_H__

#define AXCL_MODULE_VERSION "[AXCL version]: " AXCL_MODULE_NAME " " AXCL_BUILD_VERSION " " __DATE__ " " __TIME__ " " COMPILER_USERNAME

#if defined (WINDOWS)
static const char axcl_module_version[] = AXCL_MODULE_VERSION;
#else
static const char axcl_module_version[] __attribute__((used))  = AXCL_MODULE_VERSION;
#endif

#endif /* __AXCL_MODULE_VERSION_H__ */
