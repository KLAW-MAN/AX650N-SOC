/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#ifndef _IVPS_MD5_H_
#define _IVPS_MD5_H_
#include <openssl/md5.h>

typedef struct _IVPS_MD5_RESULT {
	AX_U64 nLow;
	AX_U64 nHigh;
} IVPS_MD5_RESULT_T;

typedef struct _IVPS_MD5_INFO {
	char arrCaseId[32];
	int nGrpId;
	int nChnId;
	char md5_expected[33];
} IVPS_MD5_INFO_T;


static IVPS_MD5_INFO_T gMD5Info[] =
{
	/* BASE */
	{"MM_IVPS_BASE_0001", 0, 0, "0000"},
	{"MM_IVPS_BASE_0002", 0, 0, "0000"},
	/* PPL */
	{"MM_IVPS_PPL_0001", 1, 0, "6201410f7b4b984e3e2965cab804e033"},
	{"MM_IVPS_PPL_0002", 1, 0, "2bc7a5f1465336de73c6c30cb87efd93"},
	{"MM_IVPS_PPL_0003", 1, 0, "0265cbc24e2e56776ec514ece0a3e09a"},
	{"MM_IVPS_PPL_0004", 1, 0, "ac8c0cd10fbafd3063624f86b4eb7f64"},
	/* {"MM_IVPS_PPL_0005", 1, 0, "000"}, */
	{"MM_IVPS_PPL_0006", 1, 0, "4ce5e7df81245aa0e3974952bc0a05de"},
	{"MM_IVPS_PPL_0007", 0, 0, "00047575aedef1dfe29373fb6d1d57bb"},
	{"MM_IVPS_PPL_0009", 1, 0, "a3761ec16065826d3e6e71b5ec9fbe25"},
	{"MM_IVPS_PPL_00100", 1, 1, "9144776dfe0c6d66461735a465bba077"},
	{"MM_IVPS_PPL_00111", 1, 1, "f31938060bdc59cf0d9fa31e826f6842"},
	{"MM_IVPS_PPL_0012", 1, 0, "d00bb0ee674132c7479cf3589b910f29"},
	{"MM_IVPS_PPL_0013", 1, 0, "9144776dfe0c6d66461735a465bba077"},
	{"MM_IVPS_PPL_0014", 1, 0, "f31938060bdc59cf0d9fa31e826f6842"},
	{"MM_IVPS_PPL_0015", 1, 0, "902255660de11952a76eb125220e526d"},
	{"MM_IVPS_PPL_0016", 1, 0, "936d69cb04831d06528d3c397f96a265"},
	{"MM_IVPS_PPL_0017", 1, 0, "6a9ba0ec262577c004912c70dd567ba0"},
	{"MM_IVPS_PPL_0018", 1, 0, "73c836f7dc0e7df98a4bc170b4bf68a9"},
	/* {"MM_IVPS_PPL_0019", 1, 0, "00000"}, */
	/* POOL */
	/* {"MM_IVPS_POOL_0001", 0, 0, "0000"}, */
	/* {"MM_IVPS_POOL_0002", 0, 0, "0000"}, */
	/* {"MM_IVPS_POOL_0003", 0, 0, "0000"}, */
	/* RGN */
	{"MM_IVPS_RGN_0001", 1, 0, "a7b8a93ffc4b5543e76b90c907a04fce"},
	{"MM_IVPS_RGN_0002", 1, 0, "9138e5144000b123720e4f19a1d02503"},
	{"MM_IVPS_RGN_0003", 1, 0, "6b489c1d9500d385ac6f5efbfc2f2da0"},
	{"MM_IVPS_RGN_0004", 1, 0, "212615383ac727b4b7b06a914eee4850"},
	/* SYNC */
	{"MM_IVPS_SYNC_0001", 0, 0, "401014437726f05e8a6e77099e3a3ac6"},
	{"MM_IVPS_SYNC_0002", 0, 0, "401014437726f05e8a6e77099e3a3ac6"},
	{"MM_IVPS_SYNC_0003", 0, 0, "401014437726f05e8a6e77099e3a3ac6"},
	{"MM_IVPS_SYNC_0101", 0, 0, "7d47dd500360615a2f53e80002c2b5a1"},
	{"MM_IVPS_SYNC_0102", 0, 0, "7d47dd500360615a2f53e80002c2b5a1"},
	{"MM_IVPS_SYNC_0103", 0, 0, "7d47dd500360615a2f53e80002c2b5a1"},
	{"MM_IVPS_SYNC_0201", 0, 0, "a5a01869e3afff11b39e531d85a86445"},
	{"MM_IVPS_SYNC_0202", 0, 0, "4a4592837653ec78ca2ba66180d1e6d4"},
	{"MM_IVPS_SYNC_0203", 0, 0, "ed78656822c4d3002916f3fda02f37d2"},
	{"MM_IVPS_SYNC_0301", 0, 0, "3725f43dd097048208d252cb5f443f0b"},
	{"MM_IVPS_SYNC_0401", 0, 0, "fb2b19bebc3aa4f9a1e1e42bee6460fc"},
	{"MM_IVPS_SYNC_0402", 0, 0, "79972115f4b76b56013fced2411ffac9"},
	{"MM_IVPS_SYNC_0501", 0, 0, "38bcb3348f500283558f45e6df86c424"},
	{"MM_IVPS_SYNC_0501", 0, 1, "09bf09e5362163ebc96990331d82f23c"},
	{"MM_IVPS_SYNC_0502", 0, 0, "85e823b85b491b5d2c28371303823704"},
	{"MM_IVPS_SYNC_0503", 0, 0, "85e823b85b491b5d2c28371303823704"},
	{"MM_IVPS_SYNC_0601", 0, 0, "a791e2a99ea39b5477006999af38f1a5"},
	{"MM_IVPS_SYNC_0602", 0, 0, "66c38a879b0630962a91d50f7cb4c71d"},
	{"MM_IVPS_SYNC_0603", 0, 0, "66c38a879b0630962a91d50f7cb4c71d"},
	{"MM_IVPS_SYNC_0604", 0, 0, "de32b68b3efbfce9cbf7c707693668a8"},
	{"MM_IVPS_SYNC_0605", 0, 0, "fbeff674460a1b06408649b463b79745"},
	{"MM_IVPS_SYNC_0606", 0, 0, "e24203f6c47d2cdd5a5f8a13cb22ede6"},
	{"MM_IVPS_SYNC_0607", 0, 0, "65be60384d4a1126ddeaf1a6639163ed"},
	{"MM_IVPS_SYNC_0608", 0, 0, "6c3c7829fbf0ac8e9eb59b608d6fd23e"},
	{"MM_IVPS_SYNC_0609", 0, 0, "7b12a02e7626295427186c69f84fea1f"},
	{"MM_IVPS_SYNC_0610", 0, 0, "07d52fcc7de37e5ffc1e1b718a86961a"},
	{"MM_IVPS_SYNC_0611", 0, 0, "e299306fe54477807e1261212e583ff2"},
	{"MM_IVPS_SYNC_0612", 0, 0, "7b292f07cd6f40dc740e6958d925619c"},

	/* DEWARP */
	{"MM_IVPS_DEWARP_0001", 0, 0, "0000"},
	{"MM_IVPS_DEWARP_0002", 0, 0, "0000"},
	{"MM_IVPS_DEWARP_0003", 0, 0, "0000"},
	{"MM_IVPS_DEWARP_0004", 0, 0, "0000"},
	{"MM_IVPS_DEWARP_0005", 0, 0, "0000"},
	{"MM_IVPS_DEWARP_0006", 0, 0, "0000"},
};

#endif /* _IVPS_MD5_H_ */