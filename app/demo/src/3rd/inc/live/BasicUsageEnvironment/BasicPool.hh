/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/
#pragma once
#define MAX_POOL_INCREASE_COUNT (8)

// #define __BASIC_POOL_UT__ 1
/*
   A simple auto increased pool which is NOT thread safe.
   The initial blk count is set by blkCount, each time increased blkCount * (2 ** (n-1)),
   The max. increased count is 8, it means max. blk count is blkCount * (2 ** (8-1)),
   for example:
        if initial blkCount is 5,
         1.     5 blk
         2.    10 blk
         3.    20 blk
         4.    40 blk
         5.    80 blk
         6.   160 blk
         7.   320 blk
         8.   640 blk

*/

class BasicPool {
private:
    struct obj {
        struct obj* next; // embedded pointer
    };

public:
    BasicPool(unsigned int blkSize, unsigned int initBlkCnt = 5 /* initial blkCount */);
    ~BasicPool();

    void* borrow();
    void giveback(void* p);

#ifdef __BASIC_POOL_UT__
    unsigned int freeCount() const;
#endif

private:
    void destory();

private:
    struct obj* m_freeList;
    unsigned int m_blkCount;
    unsigned int m_blkSize;
    struct obj* m_chunkList[MAX_POOL_INCREASE_COUNT]; /* store chunk address allocated */

#ifdef __BASIC_POOL_UT__
    unsigned int m_freeCount;
#endif
};

#ifdef __BASIC_POOL_UT__
inline unsigned int BasicPool::freeCount() const;
    return m_freeCount;
}
#endif
