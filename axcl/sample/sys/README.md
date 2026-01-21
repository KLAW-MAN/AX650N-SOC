### Description
This module is the sample code of the SYS module provided by the SDK package, which is convenient for customers to quickly understand and master the use of SYS related interfaces.

The code demonstrates the following functions:
1. Non-cache type CMM memory application and release
2. Cache type CMM memory application and release
3. Common pool creation and use
4. User pool creation and use
5. Binding relationship creation and query.


### Usage
```bash
options:
  -d, --device    device index from 0 to connected device num - 1 (unsigned int [=0])
  -?, --help      print this message
```

### Example
```bash
$ ./axcl_sample_sys -d 0
[INFO ][                            main][  35]: json: ./axcl.json
[INFO ][                            main][  55]: device index: 0, bus number: 129
[INFO ][           sample_sys_alloc_free][  82]: sys_alloc_free begin
[INFO ][           sample_sys_alloc_free][  91]: alloc PhyAddr= 0x14926f000,pVirAddr=0xffff82a3c000
[INFO ][           sample_sys_alloc_free][  91]: alloc PhyAddr= 0x14936f000,pVirAddr=0xffff8293c000
[INFO ][           sample_sys_alloc_free][  91]: alloc PhyAddr= 0x14946f000,pVirAddr=0xffff8283c000
[INFO ][           sample_sys_alloc_free][  91]: alloc PhyAddr= 0x14956f000,pVirAddr=0xffff8273c000
[INFO ][           sample_sys_alloc_free][  91]: alloc PhyAddr= 0x14966f000,pVirAddr=0xffff8263c000
[INFO ][           sample_sys_alloc_free][  91]: alloc PhyAddr= 0x14976f000,pVirAddr=0xffff8253c000
[INFO ][           sample_sys_alloc_free][  91]: alloc PhyAddr= 0x14986f000,pVirAddr=0xffff8243c000
[INFO ][           sample_sys_alloc_free][  91]: alloc PhyAddr= 0x14996f000,pVirAddr=0xffff8233c000
[INFO ][           sample_sys_alloc_free][  91]: alloc PhyAddr= 0x149a6f000,pVirAddr=0xffff8223c000
[INFO ][           sample_sys_alloc_free][  91]: alloc PhyAddr= 0x149b6f000,pVirAddr=0xffff8213c000
[INFO ][           sample_sys_alloc_free][ 100]: free PhyAddr= 0x14926f000,pVirAddr=0xffff82a3c000
[INFO ][           sample_sys_alloc_free][ 100]: free PhyAddr= 0x14936f000,pVirAddr=0xffff8293c000
[INFO ][           sample_sys_alloc_free][ 100]: free PhyAddr= 0x14946f000,pVirAddr=0xffff8283c000
[INFO ][           sample_sys_alloc_free][ 100]: free PhyAddr= 0x14956f000,pVirAddr=0xffff8273c000
[INFO ][           sample_sys_alloc_free][ 100]: free PhyAddr= 0x14966f000,pVirAddr=0xffff8263c000
[INFO ][           sample_sys_alloc_free][ 100]: free PhyAddr= 0x14976f000,pVirAddr=0xffff8253c000
[INFO ][           sample_sys_alloc_free][ 100]: free PhyAddr= 0x14986f000,pVirAddr=0xffff8243c000
[INFO ][           sample_sys_alloc_free][ 100]: free PhyAddr= 0x14996f000,pVirAddr=0xffff8233c000
[INFO ][           sample_sys_alloc_free][ 100]: free PhyAddr= 0x149a6f000,pVirAddr=0xffff8223c000
[INFO ][           sample_sys_alloc_free][ 100]: free PhyAddr= 0x149b6f000,pVirAddr=0xffff8213c000
[INFO ][           sample_sys_alloc_free][ 103]: sys_alloc_free end success
[INFO ][     sample_sys_alloc_cache_free][ 115]: sys_alloc_cache_free begin
[INFO ][     sample_sys_alloc_cache_free][ 124]: alloc PhyAddr= 0x14926f000,pVirAddr=0xffff82a3c000
[INFO ][     sample_sys_alloc_cache_free][ 124]: alloc PhyAddr= 0x14936f000,pVirAddr=0xffff8293c000
[INFO ][     sample_sys_alloc_cache_free][ 124]: alloc PhyAddr= 0x14946f000,pVirAddr=0xffff8283c000
[INFO ][     sample_sys_alloc_cache_free][ 124]: alloc PhyAddr= 0x14956f000,pVirAddr=0xffff8273c000
[INFO ][     sample_sys_alloc_cache_free][ 124]: alloc PhyAddr= 0x14966f000,pVirAddr=0xffff8263c000
[INFO ][     sample_sys_alloc_cache_free][ 124]: alloc PhyAddr= 0x14976f000,pVirAddr=0xffff8253c000
[INFO ][     sample_sys_alloc_cache_free][ 124]: alloc PhyAddr= 0x14986f000,pVirAddr=0xffff8243c000
[INFO ][     sample_sys_alloc_cache_free][ 124]: alloc PhyAddr= 0x14996f000,pVirAddr=0xffff8233c000
[INFO ][     sample_sys_alloc_cache_free][ 124]: alloc PhyAddr= 0x149a6f000,pVirAddr=0xffff8223c000
[INFO ][     sample_sys_alloc_cache_free][ 124]: alloc PhyAddr= 0x149b6f000,pVirAddr=0xffff8213c000
[INFO ][     sample_sys_alloc_cache_free][ 133]: free PhyAddr= 0x14926f000,pVirAddr=0xffff82a3c000
[INFO ][     sample_sys_alloc_cache_free][ 133]: free PhyAddr= 0x14936f000,pVirAddr=0xffff8293c000
[INFO ][     sample_sys_alloc_cache_free][ 133]: free PhyAddr= 0x14946f000,pVirAddr=0xffff8283c000
[INFO ][     sample_sys_alloc_cache_free][ 133]: free PhyAddr= 0x14956f000,pVirAddr=0xffff8273c000
[INFO ][     sample_sys_alloc_cache_free][ 133]: free PhyAddr= 0x14966f000,pVirAddr=0xffff8263c000
[INFO ][     sample_sys_alloc_cache_free][ 133]: free PhyAddr= 0x14976f000,pVirAddr=0xffff8253c000
[INFO ][     sample_sys_alloc_cache_free][ 133]: free PhyAddr= 0x14986f000,pVirAddr=0xffff8243c000
[INFO ][     sample_sys_alloc_cache_free][ 133]: free PhyAddr= 0x14996f000,pVirAddr=0xffff8233c000
[INFO ][     sample_sys_alloc_cache_free][ 133]: free PhyAddr= 0x149a6f000,pVirAddr=0xffff8223c000
[INFO ][     sample_sys_alloc_cache_free][ 133]: free PhyAddr= 0x149b6f000,pVirAddr=0xffff8213c000
[INFO ][     sample_sys_alloc_cache_free][ 136]: sys_alloc_cache_free end success
[INFO ][          sample_sys_commom_pool][ 148]: sys_commom_pool begin
[INFO ][          sample_sys_commom_pool][ 157]: AXCL_SYS_Init success!
[INFO ][          sample_sys_commom_pool][ 167]: AXCL_POOL_Exit success!
[INFO ][          sample_sys_commom_pool][ 199]: AXCL_POOL_SetConfig success!
[INFO ][          sample_sys_commom_pool][ 208]: AXCL_POOL_Init success!
[INFO ][          sample_sys_commom_pool][ 222]: AXCL_POOL_GetBlock success!BlkId:0x5E002000
[INFO ][          sample_sys_commom_pool][ 233]: AXCL_POOL_Handle2PoolId success!(Blockid:0x5E002000 --> PoolId=2)
[INFO ][          sample_sys_commom_pool][ 244]: AXCL_POOL_Handle2PhysAddr success!(Blockid:0x5E002000 --> PhyAddr=0x14ad8d000)
[INFO ][          sample_sys_commom_pool][ 255]: AXCL_POOL_Handle2MetaPhysAddr success!(Blockid:0x5E002000 --> MetaPhyAddr=0x14a18b000)
[INFO ][          sample_sys_commom_pool][ 265]: AXCL_POOL_ReleaseBlock success!Blockid=0x5e002000
[INFO ][          sample_sys_commom_pool][ 275]: AXCL_POOL_Exit success!
[INFO ][          sample_sys_commom_pool][ 285]: AXCL_SYS_Deinit success!
[INFO ][          sample_sys_commom_pool][ 288]: sys_commom_pool end success!
[INFO ][         sample_sys_private_pool][ 310]: sys_private_pool begin
[INFO ][         sample_sys_private_pool][ 319]: AXCL_SYS_Init success!
[INFO ][         sample_sys_private_pool][ 329]: AXCL_POOL_Exit success!
[INFO ][         sample_sys_private_pool][ 349]: AXCL_POOL_CreatePool[0] success
[INFO ][         sample_sys_private_pool][ 367]: AXCL_POOL_CreatePool[1] success
[INFO ][         sample_sys_private_pool][ 378]: AXCL_POOL_GetBlock success!BlkId:0x5E001000
[INFO ][         sample_sys_private_pool][ 389]: AXCL_POOL_Handle2PoolId success!(Blockid:0x5E001000 --> PoolId=1)
[INFO ][         sample_sys_private_pool][ 400]: AX_POOL_Handle2PhysAddr success!(Blockid:0x5E001000 --> PhyAddr=0x149879000)
[INFO ][         sample_sys_private_pool][ 411]: AXCL_POOL_Handle2MetaPhysAddr success!(Blockid:0x5E001000 --> MetaPhyAddr=0x149477000)
[INFO ][         sample_sys_private_pool][ 421]: AXCL_POOL_ReleaseBlock success!Blockid=0x5e001000
[INFO ][         sample_sys_private_pool][ 430]: AXCL_POOL_DestroyPool[1] success!
[INFO ][         sample_sys_private_pool][ 438]: AXCL_POOL_DestroyPool[0] success!
[INFO ][         sample_sys_private_pool][ 448]: AXCL_SYS_Deinit success!
[INFO ][         sample_sys_private_pool][ 451]: sys_private_pool end success!
[INFO ][                 sample_sys_link][ 472]: sample_sys_link begin
[INFO ][                 sample_sys_link][ 487]: AXCL_SYS_Init success!
[INFO ][                 sample_sys_link][ 554]: AXCL_SYS_Deinit success!
[INFO ][                 sample_sys_link][ 557]: sample_sys_link end success!
```