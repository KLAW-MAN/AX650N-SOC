/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Copyright (c) 2022 AXERA.
 *
 */
{ 0x1005f * 2, 0x55f}, //DWC_DDRPHYA_DBYTE0_TxSlewRate_b0_p0
{ 0x1015f * 2, 0x55f}, //DWC_DDRPHYA_DBYTE0_TxSlewRate_b1_p0
{ 0x1105f * 2, 0x55f}, //DWC_DDRPHYA_DBYTE1_TxSlewRate_b0_p0
{ 0x1115f * 2, 0x55f}, //DWC_DDRPHYA_DBYTE1_TxSlewRate_b1_p0
{ 0x1205f * 2, 0x55f}, //DWC_DDRPHYA_DBYTE2_TxSlewRate_b0_p0
{ 0x1215f * 2, 0x55f}, //DWC_DDRPHYA_DBYTE2_TxSlewRate_b1_p0
{ 0x1305f * 2, 0x55f}, //DWC_DDRPHYA_DBYTE3_TxSlewRate_b0_p0
{ 0x1315f * 2, 0x55f}, //DWC_DDRPHYA_DBYTE3_TxSlewRate_b1_p0
{ 0x11005f * 2, 0x55f}, // DWC_DDRPHYA_DBYTE0_p1_TxSlewRate_b0_p1
{ 0x11015f * 2, 0x55f}, // DWC_DDRPHYA_DBYTE0_p1_TxSlewRate_b1_p1
{ 0x11105f * 2, 0x55f}, // DWC_DDRPHYA_DBYTE1_p1_TxSlewRate_b0_p1
{ 0x11115f * 2, 0x55f}, // DWC_DDRPHYA_DBYTE1_p1_TxSlewRate_b1_p1
{ 0x11205f * 2, 0x55f}, // DWC_DDRPHYA_DBYTE2_p1_TxSlewRate_b0_p1
{ 0x11215f * 2, 0x55f}, // DWC_DDRPHYA_DBYTE2_p1_TxSlewRate_b1_p1
{ 0x11305f * 2, 0x55f}, // DWC_DDRPHYA_DBYTE3_p1_TxSlewRate_b0_p1
{ 0x11315f * 2, 0x55f}, // DWC_DDRPHYA_DBYTE3_p1_TxSlewRate_b1_p1
{ 0x21005f * 2, 0x55f}, // DWC_DDRPHYA_DBYTE0_p2_TxSlewRate_b0_p2
{ 0x21015f * 2, 0x55f}, // DWC_DDRPHYA_DBYTE0_p2_TxSlewRate_b1_p2
{ 0x21105f * 2, 0x55f}, // DWC_DDRPHYA_DBYTE1_p2_TxSlewRate_b0_p2
{ 0x21115f * 2, 0x55f}, // DWC_DDRPHYA_DBYTE1_p2_TxSlewRate_b1_p2
{ 0x21205f * 2, 0x55f}, // DWC_DDRPHYA_DBYTE2_p2_TxSlewRate_b0_p2
{ 0x21215f * 2, 0x55f}, // DWC_DDRPHYA_DBYTE2_p2_TxSlewRate_b1_p2
{ 0x21305f * 2, 0x55f}, // DWC_DDRPHYA_DBYTE3_p2_TxSlewRate_b0_p2
{ 0x21315f * 2, 0x55f}, // DWC_DDRPHYA_DBYTE3_p2_TxSlewRate_b1_p2
{ 0x0055 * 2, 0x19f}, // DWC_DDRPHYA_ANIB0_ATxSlewRate
{ 0x1055 * 2, 0x19f}, // DWC_DDRPHYA_ANIB1_ATxSlewRate
{ 0x2055 * 2, 0x19f}, // DWC_DDRPHYA_ANIB2_ATxSlewRate
{ 0x3055 * 2, 0x19f}, // DWC_DDRPHYA_ANIB3_ATxSlewRate
{ 0x4055 * 2, 0x19f}, // DWC_DDRPHYA_ANIB4_ATxSlewRate
{ 0x5055 * 2, 0x19f}, // DWC_DDRPHYA_ANIB5_ATxSlewRate
{ 0x200c3 * 2, 0x0   }, // DWC_DDRPHYA_MASTER0_PllPwrDn
#if DDR_CFG_2133
{ 0x200c5 * 2, 0xa   }, // DWC_DDRPHYA_MASTER0_PllCtrl2_p0
#endif
#if DDR_CFG_3200
{ 0x200c5 * 2, 0x19},
{ 0x2200c5 * 2, 0xa},
#endif
#if DDR_CFG_3733
{ 0x200c5 * 2, 0x19},
{ 0x2200c5 * 2, 0xa},
#endif
#if DDR_CFG_4266
{ 0x200c5 * 2, 0x18}, //DWC_DDRPHYA_MASTER0_PllCtrl2_p0
{ 0x1200c5 * 2, 0x19},//DWC_DDRPHYA_MASTER0_p1_PllCtrl2_p1
{ 0x2200c5 * 2, 0xa}, //DWC_DDRPHYA_MASTER0_p2_PllCtrl2_p2
#endif
{ 0x1200c5 * 2, 0x19},
{ 0x2002e * 2, 0x2},
{ 0x12002e * 2, 0x2 }, // DWC_DDRPHYA_MASTER0_p1_ARdPtrInitVal_p1
{ 0x22002e * 2, 0x2 }, // DWC_DDRPHYA_MASTER0_p2_ARdPtrInitVal_p2
{ 0x90204 * 2, 0x0},
{ 0x190204 * 2, 0x0}, // DWC_DDRPHYA_INITENG0_p1_Seq0BGPR4_p1
{ 0x290204 * 2, 0x0}, // DWC_DDRPHYA_INITENG0_p2_Seq0BGPR4_p2
{ 0x20024 * 2, 0x1a3}, // DWC_DDRPHYA_MASTER0_p2_DqsPreambleControl_p2 [7]LP4SttcPrebridgeRXEn
{ 0x120024 * 2, 0x1a3}, // DWC_DDRPHYA_MASTER0_p2_DqsPreambleControl_p2 [7]LP4SttcPrebridgeRXEn
{ 0x220024 * 2, 0x1a3}, // DWC_DDRPHYA_MASTER0_p2_DqsPreambleControl_p2 [7]LP4SttcPrebridgeRXEn
{ 0x2003a * 2, 0x2}, //DWC_DDRPHYA_MASTER0_DbyteDllModeCntrl
{ 0x2007d * 2, 0x212}, //DWC_DDRPHYA_MASTER0_DllLockParam_p0
{ 0x2007c * 2, 0x61}, //DWC_DDRPHYA_MASTER0_DllGainCtl_p0
{ 0x12003a * 2, 0x2}, //DWC_DDRPHYA_MASTER0_DbyteDllModeCntrl_p1
{ 0x12007d * 2, 0x212}, // DWC_DDRPHYA_MASTER0_p1_DllLockParam_p1
{ 0x12007c * 2, 0x61}, // DWC_DDRPHYA_MASTER0_p1_DllGainCtl_p1
{ 0x22003a * 2, 0x2}, //DWC_DDRPHYA_MASTER0_DbyteDllModeCntrl_p2
{ 0x22007d * 2, 0x212}, // DWC_DDRPHYA_MASTER0_p2_DllLockParam_p2
{ 0x22007c * 2, 0x61}, // DWC_DDRPHYA_MASTER0_p2_DllGainCtl_p2
{ 0x20056 * 2, 0x3},
{ 0x120056 * 2, 0x3}, // DWC_DDRPHYA_MASTER0_p1_ProcOdtTimeCtl_p1
{ 0x220056 * 2, 0x3}, // DWC_DDRPHYA_MASTER0_p2_ProcOdtTimeCtl_p2
{ 0x1004d * 2, 0x600},
{ 0x1014d * 2, 0x600},
{ 0x1104d * 2, 0x600},
{ 0x1114d * 2, 0x600},
{ 0x1204d * 2, 0x600},
{ 0x1214d * 2, 0x600},
{ 0x1304d * 2, 0x600},
{ 0x1314d * 2, 0x600},
{ 0x11004d * 2, 0x600}, // DWC_DDRPHYA_DBYTE0_p1_TxOdtDrvStren_b0_p1
{ 0x11014d * 2, 0x600}, // DWC_DDRPHYA_DBYTE0_p1_TxOdtDrvStren_b1_p1
{ 0x11104d * 2, 0x600}, // DWC_DDRPHYA_DBYTE1_p1_TxOdtDrvStren_b0_p1
{ 0x11114d * 2, 0x600}, // DWC_DDRPHYA_DBYTE1_p1_TxOdtDrvStren_b1_p1
{ 0x11204d * 2, 0x600}, // DWC_DDRPHYA_DBYTE2_p1_TxOdtDrvStren_b0_p1
{ 0x11214d * 2, 0x600}, // DWC_DDRPHYA_DBYTE2_p1_TxOdtDrvStren_b1_p1
{ 0x11304d * 2, 0x600}, // DWC_DDRPHYA_DBYTE3_p1_TxOdtDrvStren_b0_p1
{ 0x11314d * 2, 0x600}, // DWC_DDRPHYA_DBYTE3_p1_TxOdtDrvStren_b1_p1
{ 0x21004d * 2, 0x600}, // DWC_DDRPHYA_DBYTE0_p2_TxOdtDrvStren_b0_p2
{ 0x21014d * 2, 0x600}, // DWC_DDRPHYA_DBYTE0_p2_TxOdtDrvStren_b1_p2
{ 0x21104d * 2, 0x600}, // DWC_DDRPHYA_DBYTE1_p2_TxOdtDrvStren_b0_p2
{ 0x21114d * 2, 0x600}, // DWC_DDRPHYA_DBYTE1_p2_TxOdtDrvStren_b1_p2
{ 0x21204d * 2, 0x600}, // DWC_DDRPHYA_DBYTE2_p2_TxOdtDrvStren_b0_p2
{ 0x21214d * 2, 0x600}, // DWC_DDRPHYA_DBYTE2_p2_TxOdtDrvStren_b1_p2
{ 0x21304d * 2, 0x600}, // DWC_DDRPHYA_DBYTE3_p2_TxOdtDrvStren_b0_p2
{ 0x21314d * 2, 0x600}, // DWC_DDRPHYA_DBYTE3_p2_TxOdtDrvStren_b1_p2
{ 0x10049 * 2, 0xe00},
{ 0x10149 * 2, 0xe00},
{ 0x11049 * 2, 0xe00},
{ 0x11149 * 2, 0xe00},
{ 0x12049 * 2, 0xe00},
{ 0x12149 * 2, 0xe00},
{ 0x13049 * 2, 0xe00},
{ 0x13149 * 2, 0xe00},
{ 0x110049 * 2, 0xe00}, // DWC_DDRPHYA_DBYTE0_p1_TxImpedanceCtrl1_b0_p1
{ 0x110149 * 2, 0xe00}, // DWC_DDRPHYA_DBYTE0_p1_TxImpedanceCtrl1_b1_p1
{ 0x111049 * 2, 0xe00}, // DWC_DDRPHYA_DBYTE1_p1_TxImpedanceCtrl1_b0_p1
{ 0x111149 * 2, 0xe00}, // DWC_DDRPHYA_DBYTE1_p1_TxImpedanceCtrl1_b1_p1
{ 0x112049 * 2, 0xe00}, // DWC_DDRPHYA_DBYTE2_p1_TxImpedanceCtrl1_b0_p1
{ 0x112149 * 2, 0xe00}, // DWC_DDRPHYA_DBYTE2_p1_TxImpedanceCtrl1_b1_p1
{ 0x113049 * 2, 0xe00}, // DWC_DDRPHYA_DBYTE3_p1_TxImpedanceCtrl1_b0_p1
{ 0x113149 * 2, 0xe00}, // DWC_DDRPHYA_DBYTE3_p1_TxImpedanceCtrl1_b1_p1
{ 0x210049 * 2, 0xe00}, // DWC_DDRPHYA_DBYTE0_p2_TxImpedanceCtrl1_b0_p2
{ 0x210149 * 2, 0xe00}, // DWC_DDRPHYA_DBYTE0_p2_TxImpedanceCtrl1_b1_p2
{ 0x211049 * 2, 0xe00}, // DWC_DDRPHYA_DBYTE1_p2_TxImpedanceCtrl1_b0_p2
{ 0x211149 * 2, 0xe00}, // DWC_DDRPHYA_DBYTE1_p2_TxImpedanceCtrl1_b1_p2
{ 0x212049 * 2, 0xe00}, // DWC_DDRPHYA_DBYTE2_p2_TxImpedanceCtrl1_b0_p2
{ 0x212149 * 2, 0xe00}, // DWC_DDRPHYA_DBYTE2_p2_TxImpedanceCtrl1_b1_p2
{ 0x213049 * 2, 0xe00}, // DWC_DDRPHYA_DBYTE3_p2_TxImpedanceCtrl1_b0_p2
{ 0x213149 * 2, 0xe00}, // DWC_DDRPHYA_DBYTE3_p2_TxImpedanceCtrl1_b1_p2
{ 0x00043 * 2, 0x60},
{ 0x01043 * 2, 0x60},
{ 0x02043 * 2, 0x60},
{ 0x03043 * 2, 0x60},
{ 0x04043 * 2, 0x60},
{ 0x05043 * 2, 0x60},
{ 0x20018 * 2, 0x3},
{ 0x20075 * 2, 0x4},
{ 0x20050 * 2, 0x0},
{ 0x2009b * 2, 0x2},
#if DDR_CFG_2133
{ 0x20008 * 2, 0x215 }, // DWC_DDRPHYA_MASTER0_CalUclkInfo_p0
#endif
#if DDR_CFG_3200
{ 0x20008 * 2, 0x320},
{ 0x220008 * 2, 0x215}, // DWC_DDRPHYA_MASTER0_p2_CalUclkInfo_p1
#endif
#if DDR_CFG_3733
{ 0x20008 * 2, 0x3a5},
{ 0x220008 * 2, 0x215}, // DWC_DDRPHYA_MASTER0_p2_CalUclkInfo_p1
#endif
#if DDR_CFG_4266
{ 0x20008 * 2, 0x42b},
{ 0x120008 * 2, 0x320}, // DWC_DDRPHYA_MASTER0_p1_CalUclkInfo_p1
{ 0x220008 * 2, 0x215}, // DWC_DDRPHYA_MASTER0_p2_CalUclkInfo_p2
#endif
{ 0x20088 * 2, 0x9},
//{ 0x200b2 * 2, 0x104},
{ 0x10043 * 2, 0x5a1},
{ 0x10143 * 2, 0x5a1},
{ 0x11043 * 2, 0x5a1},
{ 0x11143 * 2, 0x5a1},
{ 0x12043 * 2, 0x5a1},
{ 0x12143 * 2, 0x5a1},
{ 0x13043 * 2, 0x5a1},
{ 0x13143 * 2, 0x5a1},
//{ 0x1200b2 * 2, 0x104}, // DWC_DDRPHYA_MASTER0_p1_VrefInGlobal_p1
{ 0x110043 * 2, 0x5a1}, // DWC_DDRPHYA_DBYTE0_p1_DqDqsRcvCntrl_b0_p1
{ 0x110143 * 2, 0x5a1}, // DWC_DDRPHYA_DBYTE0_p1_DqDqsRcvCntrl_b1_p1
{ 0x111043 * 2, 0x5a1}, // DWC_DDRPHYA_DBYTE1_p1_DqDqsRcvCntrl_b0_p1
{ 0x111143 * 2, 0x5a1}, // DWC_DDRPHYA_DBYTE1_p1_DqDqsRcvCntrl_b1_p1
{ 0x112043 * 2, 0x5a1}, // DWC_DDRPHYA_DBYTE2_p1_DqDqsRcvCntrl_b0_p1
{ 0x112143 * 2, 0x5a1}, // DWC_DDRPHYA_DBYTE2_p1_DqDqsRcvCntrl_b1_p1
{ 0x113043 * 2, 0x5a1}, // DWC_DDRPHYA_DBYTE3_p1_DqDqsRcvCntrl_b0_p1
{ 0x113143 * 2, 0x5a1}, // DWC_DDRPHYA_DBYTE3_p1_DqDqsRcvCntrl_b1_p1
//{ 0x2200b2 * 2, 0x104}, // DWC_DDRPHYA_MASTER0_p2_VrefInGlobal_p2
{ 0x210043 * 2, 0x5a1}, // DWC_DDRPHYA_DBYTE0_p2_DqDqsRcvCntrl_b0_p2
{ 0x210143 * 2, 0x5a1}, // DWC_DDRPHYA_DBYTE0_p2_DqDqsRcvCntrl_b1_p2
{ 0x211043 * 2, 0x5a1}, // DWC_DDRPHYA_DBYTE1_p2_DqDqsRcvCntrl_b0_p2
{ 0x211143 * 2, 0x5a1}, // DWC_DDRPHYA_DBYTE1_p2_DqDqsRcvCntrl_b1_p2
{ 0x212043 * 2, 0x5a1}, // DWC_DDRPHYA_DBYTE2_p2_DqDqsRcvCntrl_b0_p2
{ 0x212143 * 2, 0x5a1}, // DWC_DDRPHYA_DBYTE2_p2_DqDqsRcvCntrl_b1_p2
{ 0x213043 * 2, 0x5a1}, // DWC_DDRPHYA_DBYTE3_p2_DqDqsRcvCntrl_b0_p2
{ 0x213143 * 2, 0x5a1}, // DWC_DDRPHYA_DBYTE3_p2_DqDqsRcvCntrl_b1_p2
{ 0x200fa * 2, 0x1},
{ 0x1200fa * 2, 0x1}, // DWC_DDRPHYA_MASTER0_p1_DfiFreqRatio_p1
{ 0x2200fa * 2, 0x1}, // DWC_DDRPHYA_MASTER0_p2_DfiFreqRatio_p2
{ 0x20019 * 2, 0x1},
{ 0x120019 * 2, 0x1}, // DWC_DDRPHYA_MASTER0_p1_TristateModeCA_p1
{ 0x220019 * 2, 0x1}, // DWC_DDRPHYA_MASTER0_p2_TristateModeCA_p2
{ 0x200f0 * 2, 0x0},
{ 0x200f1 * 2, 0x0},
{ 0x200f2 * 2, 0x4444},
{ 0x200f3 * 2, 0x8888},
{ 0x200f4 * 2, 0x5555},
{ 0x200f5 * 2, 0x0},
{ 0x200f6 * 2, 0x0},
{ 0x200f7 * 2, 0xf000},
/* { 0x1004a * 2, 0x500},
{ 0x1104a * 2, 0x500},
{ 0x1204a * 2, 0x500},
{ 0x1304a * 2, 0x500}, */
{ 0x20025 * 2, 0x0},
{ 0x2002d * 2, 0x1},
{ 0x12002d * 2, 0x1}, // DWC_DDRPHYA_MASTER0_p1_DMIPinPresent_p1
{ 0x22002d * 2, 0x1}, // DWC_DDRPHYA_MASTER0_p2_DMIPinPresent_p2
{ 0x2002c * 2, 0x0},
