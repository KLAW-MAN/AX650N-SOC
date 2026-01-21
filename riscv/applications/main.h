/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#ifndef __MAIN_H__
#define __MAIN_H__

#include <rtdef.h>

int sync_sig_app(void);
int lowpower_app(void);

/* ========================================================================================= */
/* ================  List all the user INIT_APP_EXPORT here, then the apps  ================ */
/* ================  will be executed in order.                             ================ */
/* ================  For example:                                           ================ */
/* ================              INIT_APP_EXPORT(qs_sensor);                ================ */
/* ================              INIT_APP_EXPORT(read_rootfs);              ================ */
/* ================  then read_rootfs will be executed before sensor_test.  ================ */
/* ================  Notes: The code order is oppsite to execution order    ================ */
/* ========================================================================================= */
INIT_APP_EXPORT(lowpower_app);
INIT_APP_EXPORT(sync_sig_app);

#endif //__MAIN_H__
