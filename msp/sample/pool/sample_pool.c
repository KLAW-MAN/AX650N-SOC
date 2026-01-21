/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#include <unistd.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include "ax_base_type.h"
#include "ax_sys_api.h"

int main(int argc, char *argv[])
{
	AX_S32 ret=0,i=0;
	AX_POOL_FLOORPLAN_T PoolFloorPlan;
	AX_U64 BlkSize;
	AX_BLK BlkId;
	AX_POOL PoolId;
	AX_U64 PhysAddr,MetaPhysAddr;
	AX_VOID *blockVirAddr,*metaVirAddr;

	AX_POOL_CONFIG_T PoolConfig;
	AX_POOL UserPoolId0,UserPoolId1,UserPoolId2;

	printf("sample_pool test begin\n\n");

	AX_SYS_Init();

	/* step 1:exit pool */
	ret = AX_POOL_Exit();

	if (ret) {
		printf("AX_POOL_Exit fail!!Error Code:0x%X\n",ret);
		return -1;
	} else {
		printf("AX_POOL_Exit success!\n");
	}

	/* step 2:set commom pool config */
	memset(&PoolFloorPlan, 0, sizeof(AX_POOL_FLOORPLAN_T));
	PoolFloorPlan.CommPool[0].MetaSize   = 0x2000;
	PoolFloorPlan.CommPool[0].BlkSize   = 1*1024*1024;
	PoolFloorPlan.CommPool[0].BlkCnt    = 5;
	PoolFloorPlan.CommPool[0].CacheMode = POOL_CACHE_MODE_NONCACHE;
	PoolFloorPlan.CommPool[1].MetaSize   = 0x2000;
	PoolFloorPlan.CommPool[1].BlkSize   = 2*1024*1024;
	PoolFloorPlan.CommPool[1].BlkCnt    = 5;
	PoolFloorPlan.CommPool[1].CacheMode = POOL_CACHE_MODE_NONCACHE;
	PoolFloorPlan.CommPool[2].MetaSize   = 0x2000;
	PoolFloorPlan.CommPool[2].BlkSize   = 3*1024*1024;
	PoolFloorPlan.CommPool[2].BlkCnt    = 5;
	PoolFloorPlan.CommPool[2].CacheMode = POOL_CACHE_MODE_NONCACHE;

	/* The default value of PartitionName is anonymous. The partition name must exist when the cmm ko is loaded */
	memset(PoolFloorPlan.CommPool[0].PartitionName, 0, sizeof(PoolFloorPlan.CommPool[0].PartitionName));
	memset(PoolFloorPlan.CommPool[1].PartitionName, 0, sizeof(PoolFloorPlan.CommPool[1].PartitionName));
	memset(PoolFloorPlan.CommPool[2].PartitionName, 0, sizeof(PoolFloorPlan.CommPool[2].PartitionName));
	strcpy((char *)PoolFloorPlan.CommPool[0].PartitionName, "anonymous");
	strcpy((char *)PoolFloorPlan.CommPool[1].PartitionName, "anonymous");
	strcpy((char *)PoolFloorPlan.CommPool[2].PartitionName, "anonymous");

	ret = AX_POOL_SetConfig(&PoolFloorPlan);

	if (ret) {
		printf("AX_POOL_SetConfig fail!Error Code:0x%X\n",ret);
		return -1;
	} else {
		printf("AX_POOL_SetConfig success!\n");
	}

	/* step 3:init pool */
	ret = AX_POOL_Init();
	if (ret) {
		printf("AX_POOL_Init fail!!Error Code:0x%X\n",ret);
		return -1;
	} else {
		printf("AX_POOL_Init success!\n");
	}

	/* step 4:create 3 user pool through AX_POOL_CreatePool */

	/* user_pool_0 :blocksize=1*1024*1024,metasize=0x1000,block count =2 ,noncache */
	memset(&PoolConfig, 0, sizeof(AX_POOL_CONFIG_T));
	PoolConfig.MetaSize = 0x1000;
	PoolConfig.BlkSize = 1*1024*1024;
	PoolConfig.BlkCnt = 2;
	PoolConfig.CacheMode = POOL_CACHE_MODE_NONCACHE;
	memset(PoolConfig.PartitionName,0, sizeof(PoolConfig.PartitionName));
	strcpy((char *)PoolConfig.PartitionName, "anonymous");

	UserPoolId0 = AX_POOL_CreatePool(&PoolConfig);

	if (AX_INVALID_POOLID == UserPoolId0) {
		printf("AX_POOL_CreatePool error!!!\n");
		goto ERROR;
	} else {
		printf("AX_POOL_CreatePool[%d] success\n", UserPoolId0);
	}

	/* user_pool_1:blocksize=2*1024*1024,metasize=0x1000,block count =3 ,noncache */
	memset(&PoolConfig, 0, sizeof(AX_POOL_CONFIG_T));
	PoolConfig.MetaSize = 0x1000;
	PoolConfig.BlkSize = 2*1024*1024;
	PoolConfig.BlkCnt = 3;
	PoolConfig.CacheMode = POOL_CACHE_MODE_NONCACHE;
	memset(PoolConfig.PartitionName,0, sizeof(PoolConfig.PartitionName));
	strcpy((char *)PoolConfig.PartitionName, "anonymous");

	UserPoolId1 = AX_POOL_CreatePool(&PoolConfig);

	if (AX_INVALID_POOLID == UserPoolId1) {
		printf("AX_POOL_CreatePool error!!!\n");
		goto ERROR;
	} else {
		printf("AX_POOL_CreatePool[%d] success\n", UserPoolId1);
	}

	/* user_pool_2:blocksize=3*1024*1024,metasize=0x1000,block count =2 ,cache */
	memset(&PoolConfig, 0, sizeof(AX_POOL_CONFIG_T));
	PoolConfig.MetaSize = 0x1000;
	PoolConfig.BlkSize = 3*1024*1024;
	PoolConfig.BlkCnt = 2;
	PoolConfig.CacheMode = POOL_CACHE_MODE_CACHED;
	memset(PoolConfig.PartitionName,0, sizeof(PoolConfig.PartitionName));
	strcpy((char *)PoolConfig.PartitionName, "anonymous");

	UserPoolId2 = AX_POOL_CreatePool(&PoolConfig);

	if (AX_INVALID_POOLID == UserPoolId2) {
		printf("AX_POOL_CreatePool error!!!\n");
		goto ERROR;
	} else {
		printf("AX_POOL_CreatePool[%d] success\n", UserPoolId2);
	}

	/* step 5:get block */
#if 1
	/* getblock from user_pool_0 */

	BlkSize = 1*1024*1024;//Expected buffer size
	BlkId = AX_POOL_GetBlock(UserPoolId0, BlkSize, NULL);

	if (BlkId == AX_INVALID_BLOCKID) {
		printf("AX_POOL_GetBlock fail!\n");
		goto ERROR;
	} else {
		printf("AX_POOL_GetBlock success!BlkId:0x%X\n", BlkId);
	}
#else
	/* get block from common pool, the first parameter is AX_INVALID_POOLID */

	BlkSize = 3*1024*1024;//Expected buffer size
	BlkId = AX_POOL_GetBlock(AX_INVALID_POOLID, BlkSize, NULL);

	if (BlkId == AX_INVALID_BLOCKID) {
		printf("AX_POOL_GetBlock fail!\n");
		goto ERROR;
	} else {
		printf("AX_POOL_GetBlock success!BlkId:0x%X\n", BlkId);
	}
#endif

	/* step 6:get poolid from blkid */
	PoolId = AX_POOL_Handle2PoolId(BlkId);

	if (PoolId == AX_INVALID_POOLID) {
		printf("AX_POOL_Handle2PoolId fail!\n");
		goto ERROR;
	} else {
		printf("AX_POOL_Handle2PoolId success!(Blockid:0x%X --> PoolId=%d)\n", BlkId, PoolId);
	}

	/* step 7:get block physaddr from blkid */
	PhysAddr = AX_POOL_Handle2PhysAddr(BlkId);

	if (!PhysAddr) {
		printf("AX_POOL_Handle2PhysAddr fail!\n");
		goto ERROR;
	} else {
		printf("AX_POOL_Handle2PhysAddr success!(Blockid:0x%X --> PhyAddr=0x%llx)\n", BlkId, PhysAddr);
	}

	/* step 8:get metadata physaddr from blkid */
	MetaPhysAddr = AX_POOL_Handle2MetaPhysAddr(BlkId);

	if (!MetaPhysAddr) {
		printf("AX_POOL_Handle2MetaPhysAddr fail!\n");
		goto ERROR;
	} else {
		printf("AX_POOL_Handle2MetaPhysAddr success!(Blockid:0x%X --> MetaPhyAddr=0x%llx)\n", BlkId, MetaPhysAddr);
	}

	/* step 9:map the pool */
	ret = AX_POOL_MmapPool(PoolId);

	if (ret) {
		printf("AX_POOL_MmapPool fail!!Error Code:0x%X\n", ret);
		goto ERROR;
	} else {
		printf("AX_POOL_MmapPool success!\n");
	}

	/* step 10:get block viraddr from blkid */
	blockVirAddr = AX_POOL_GetBlockVirAddr(BlkId);

	if (!blockVirAddr) {
		printf("AX_POOL_GetBlockVirAddr fail!\n");
		return -1;
	} else {
		printf("AX_POOL_GetBlockVirAddr success!blockVirAddr=0x%lx\n", (AX_ULONG)blockVirAddr);
	}

	/* Access memory through block virtual address */
	for (i = 0; i < 20; i++) {
		((AX_S32 *)blockVirAddr)[i] = i;
	}

	for(i = 0;i < 20; i++){
		printf("%d,", ((AX_S32 *)blockVirAddr)[i]);
	}
	printf("\n");

	metaVirAddr = AX_POOL_GetMetaVirAddr(BlkId);

	if (!metaVirAddr) {
		printf("AX_POOL_GetMetaVirAddr fail!\n");
		goto ERROR;
	} else {
		printf("AX_POOL_GetMetaVirAddr success!metaVirAddr=0x%lx\n", (AX_ULONG)metaVirAddr);
	}

	/* Access memory through metadata virtual address */
	for (i = 0; i < 20; i++) {
		((AX_S32 *)metaVirAddr)[i] = i*2;
	}

	//read data
	for (i = 0; i < 20; i++) {
		printf("%d,", ((AX_S32 *)metaVirAddr)[i]);
	}
	printf("\n");

	/* step 11:release block */
	ret = AX_POOL_ReleaseBlock(BlkId);

	if (ret) {
		printf("AX_POOL_ReleaseBlock fail!Error Code:0x%X\n", ret);
		goto ERROR;
	} else {
		printf("AX_POOL_ReleaseBlock success!Blockid=0x%X\n", BlkId);
	}

	/* step 12:unmap pool */
	ret = AX_POOL_MunmapPool(PoolId);

	if (ret) {
		printf("AX_POOL_MunmapPool fail!!Error Code:0x%X\n", ret);
		goto ERROR;
	} else {
		printf("AX_POOL_MunmapPool success!\n");
	}

	/* step 13:destroy pool */
	ret = AX_POOL_DestroyPool(UserPoolId2);
	if (ret) {
		printf("AX_POOL_DestroyPool fail!!Error Code:0x%X\n", ret);
		goto ERROR;
	} else {
		printf("AX_POOL_DestroyPool[%d] success!\n", UserPoolId2);
	}

	ret = AX_POOL_DestroyPool(UserPoolId1);
	if (ret) {
		printf("AX_POOL_DestroyPool fail!!Error Code:0x%X\n", ret);
		goto ERROR;
	} else {
		printf("AX_POOL_DestroyPool[%d] success!\n", UserPoolId1);
	}

	ret = AX_POOL_DestroyPool(UserPoolId0);
	if (ret) {
		printf("AX_POOL_DestroyPool fail!!Error Code:0x%X\n", ret);
		goto ERROR;
	} else {
		printf("AX_POOL_DestroyPool[%d] success!\n", UserPoolId0);
	}

	/* step 14:exit pool */
	ret = AX_POOL_Exit();

	if (ret) {
		printf("AX_POOL_Exit fail!!Error Code:0x%X\n", ret);
		return -1;
	} else {
		printf("AX_POOL_Exit success!\n");
	}

	printf("\nsample_pool test end\n");
	AX_SYS_Deinit();
	return 0;

ERROR:
	printf("\nsample_pool test fail!\n");
	ret = AX_POOL_Exit();
	if (ret) {
		printf("AX_POOL_Exit fail!!Error Code:0x%X\n", ret);
		return -1;
	} else {
		printf("AX_POOL_Exit success!\n");
	}
	AX_SYS_Deinit();
	return -1;
}
