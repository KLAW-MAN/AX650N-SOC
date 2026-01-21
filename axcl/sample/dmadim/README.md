### Sample for device DMA
1. memcpy between two device memories by AXCL_DMA_MemCopy
2. memset device memory to 0xAB by AXCL_DMA_MemCopy
3. checksum by AXCL_DMA_CheckSum
4. crop 1/4 image from (0, 0) by AXCL_DMA_MemCopyXD (AX_DMADIM_2D)

### Usage
```bash
usage: ./axcl_sample_dmadim --image=string --width=unsigned int --height=unsigned int [options] ...
options:
  -d, --device    device index from 0 to connected device num - 1 (unsigned int [=0])
  -i, --image     nv12 image file path (string)
  -w, --width     width of nv12 image (unsigned int)
  -h, --height    height of nv12 image (unsigned int)
      --json      axcl.json path (string [=./axcl.json])
  -?, --help      print this message
```

### Example
```bash
$ ./axcl_sample_dmadim -i 1920x1080.nv12.yuv -w 1920 -h 1080 -d 0
[INFO ][                            main][  30]: ============== V2.26.1 sample started Feb 13 2025 11:10:23 ==============
[INFO ][                            main][  46]: json: ./axcl.json
[INFO ][                            main][  66]: device index: 0, bus number: 129
[INFO ][                        dma_copy][ 119]: dma copy device memory succeed, from 0x14926f000 to 0x14966f000
[INFO ][                      dma_memset][ 139]: dma memset device memory succeed, 0x14926f000 to 0xab
[INFO ][                    dma_checksum][ 166]: dma checksum succeed, checksum = 0xaaa00000
[INFO ][                      dma_copy2d][ 281]: [0] dma memcpy 2D succeed
[INFO ][                      dma_copy2d][ 281]: [1] dma memcpy 2D succeed
[INFO ][                      dma_copy2d][ 308]: ./dma2d_output_image_960x540.nv12 is saved
[INFO ][                      dma_copy2d][ 328]: dma copy2d nv12 image pass
[INFO ][                            main][  82]: ============== V2.26.1 sample exited Feb 13 2025 11:10:23 ==============
```
