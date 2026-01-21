typedef enum {
	AX_DMADIM_MODE_OFFSET = 0,
	AX_DMADIM_1D = AX_DMADIM_MODE_OFFSET,
	AX_DMADIM_2D,
	AX_DMADIM_3D,
	AX_DMADIM_4D,
	AX_DMADIM_MEMORY_INIT,
	AX_DMADIM_CHECKSUM,
} ax_dmadim_xfer_mode_t;

typedef enum {
	DMADIM_ENDIAN_DEF = 0,
	DMADIM_ENDIAN_32B,
	DMADIM_ENDIAN_16B,
	DMADIM_ENDIAN_MAX = DMADIM_ENDIAN_16B
} ax_dmadim_endian_t;

typedef struct {
	u64 paddr;
	u32 imgw;
	u32 stride[3];
} dim_info_t;

typedef struct {
	u16 ntiles[3];
	dim_info_t src_info;
	dim_info_t dst_info;
} ax_dmadim_xd_desc_t;

/**
 * @brief
 * @param dma_callback
 * The first parameter is customized by the customer, and the second parameter
 * is the return value of DMA processing, with a return value of 1 indicating
 * success.
 * @param arg
 * The first parameter of the dma_callback function
 */
int ax_dmadim_xfer(u64 src_addr, u64 dest_addr, u32 size,
		   int (*dma_callback) (void *, unsigned int), void *arg);
int ax_dmadim_xfer_crop(ax_dmadim_xd_desc_t *xd_desc, u32 lli_num,
			ax_dmadim_xfer_mode_t dma_mode, ax_dmadim_endian_t endian,
			int (*dma_callback) (void *, unsigned int), void *arg);
int ax_dmadim_xfer_sync(u64 src_addr, u64 dest_addr, u32 size);
int ax_dmadim_xfer_crop_sync(ax_dmadim_xd_desc_t *xd_desc, u32 lli_num,
			ax_dmadim_xfer_mode_t dma_mode, ax_dmadim_endian_t endian);

