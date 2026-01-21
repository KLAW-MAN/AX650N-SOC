Description:
CMM interface reference use case

cmd：
/opt/bin/sample_cmm


Test item description:
ax_mem_cmm_test_001：test MemAlloc/MemFree

ax_mem_cmm_test_002：test AX_SYS_MemAllocCached/AX_SYS_MemFree

ax_mem_cmm_test_003：test AX_SYS_GetBlockInfoByVirt/AX_SYS_GetBlockInfoByPhy non-cached VirtAddr

ax_mem_cmm_test_004：test AX_SYS_Mmap/AX_SYS_Munmap

ax_mem_cmm_test_005：test AX_SYS_MmapCache/AX_SYS_MflushCache/AX_SYS_Munmap

ax_mem_cmm_test_006：test AX_SYS_MmapCache/AX_SYS_MinvalidateCache/AX_SYS_Munmap

ax_mem_cmm_test_007：test AX_SYS_MflushCache

ax_mem_cmm_test_008：test AX_SYS_MinvalidateCache

ax_mem_cmm_test_009：test AX_SYS_MflushCache with offset address case 1

ax_mem_cmm_test_010：test AX_SYS_MflushCache with offset address case 2

ax_mem_cmm_test_011：test AX_SYS_MflushCache with offset address case 3

ax_mem_cmm_test_012：This case expects to return failure!!! test AX_SYS_MfushCache with offset address case 4

ax_mem_cmm_test_013：test AX_SYS_MmapCache/AX_SYS_MflushCache/AX_SYS_Munmap with offset address

ax_mem_cmm_test_014：This case expects to return failure!!! test AX_SYS_MmapCache/AX_SYS_MflushCache/AX_SYS_Munmap with offset address

ax_mem_cmm_test_015：test AX_SYS_MmapCache/AX_SYS_MflushCache/AX_SYS_Munmap Unmanaged mode

ax_mem_cmm_test_016：test AX_SYS_MmapCache/AX_SYS_MinvalidateCache/AX_SYS_Munmap Hosting mode

ax_mem_cmm_test_017：test AX_SYS_GetBlockInfoByVirt/AX_SYS_GetBlockInfoByPhy with cache virtual address

ax_mem_cmm_test_018：test AX_SYS_GetBlockInfoByVirt/AX_SYS_GetBlockInfoByPhy with virtual address from AX_SYS_Mmap

ax_mem_cmm_test_019：test AX_SYS_GetBlockInfoByVirt/AX_SYS_GetBlockInfoByPhy with virtual address from AX_SYS_MmapCache

ax_mem_cmm_test_020：test AX_SYS_Mmap/AX_SYS_MmapCache map a single VB in the pool.

ax_mem_cmm_test_021：test AX_SYS_MmapFast

ax_mem_cmm_test_022：test AX_SYS_MmapCacheFast

ax_mem_cmm_test_023：test AX_SYS_MmapCacheFast/AX_SYS_MflushCache/AX_SYS_Munmap

ax_mem_cmm_test_024：test AX_SYS_MmapCacheFast/AX_SYS_MinvalidateCache/AX_SYS_Munmap

Note: 
ax_mem_cmm_test_012 and ax_mem_cmm_test_014 are special cases that are intentionally constructed. 
We expect the result they return to be fail. The purpose is to reversely verify whether the flush interface 
is really effective.Sometimes a single run may not be able to detect a fail, so you can test it several times.