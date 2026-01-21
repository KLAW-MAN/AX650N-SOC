/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-08-20     zx.chen      CSI Core Peripheral Access Layer Header File for
 *                             CSKYSOC Device Series
 */

#ifndef _SOC_H_
#define _SOC_H_

#include <stdint.h>
#include <csi_core.h>
#include "mc50_e907_soc.h"

#ifdef __cplusplus
extern "C" {
#endif

/* -------------------------  Interrupt Number Definition  ------------------------ */
typedef enum IRQn {
    NMI_EXPn                        =   -2,      /* NMI Exception */
    /* ----------------------  SmartL Specific Interrupt Numbers  --------------------- */
    Machine_Software_IRQn           =   3,      /* Machine software interrupt */
    User_Timer_IRQn                 =   4,      /* User timer interrupt */
    Supervisor_Timer_IRQn           =   5,      /* Supervisor timer interrupt */
    CORET_IRQn                      =   7,      /* core Timer Interrupt */
    Machine_External_IRQn           =   11,     /* Machine external interrupt */
#ifdef MC50_E907
    ALL_PERIPH_IRQn                 =   16,     /* uart Interrupt */
#else
    UART_IRQn                       =   21,     /* uart Interrupt */
#endif
}IRQn_Type;

#ifdef __cplusplus
}
#endif

#endif  /* _SOC_H_ */
