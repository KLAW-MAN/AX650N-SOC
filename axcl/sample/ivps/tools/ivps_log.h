/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#ifndef _IVPS_LOG_H_
#define _IVPS_LOG_H_


// #define ALOGD(fmt, ...) printf("\033[1;30;37mDEBUG  :[%s:%d] " fmt "\033[0m\n", __func__, __LINE__, ##__VA_ARGS__) // white
// #define ALOGI(fmt, ...) printf("\033[1;30;32mINFO   :[%s:%d] " fmt "\033[0m\n", __func__, __LINE__, ##__VA_ARGS__) // green
// #define ALOGW(fmt, ...) printf("\033[1;30;33mWARN   :[%s:%d] " fmt "\033[0m\n", __func__, __LINE__, ##__VA_ARGS__) // yellow
// #define ALOGE(fmt, ...) printf("\033[1;30;31mERROR  :[%s:%d] " fmt "\033[0m\n", __func__, __LINE__, ##__VA_ARGS__) // red
// #define ALOGN(fmt, ...) printf("\033[1;30;37mINFO   :[%s:%d] " fmt "\033[0m\n", __func__, __LINE__, ##__VA_ARGS__) // white

#define ivps_err(fmt, ...) printf("\033[1;30;31mERROR  :[%s:%d] " fmt "\033[0m\n", __func__, __LINE__, ##__VA_ARGS__) // red
#define ivps_warn(fmt, ...) printf("\033[1;30;33mWARN   :[%s:%d] " fmt "\033[0m\n", __func__, __LINE__, ##__VA_ARGS__) // yellow

#endif /* _IVPS_LOG_H_ */
