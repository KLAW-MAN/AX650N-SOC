#include <linux/dmaengine.h>
#include <linux/async_tx.h>

#define MAX_ID		(16)
#define MAX_TYPE	(2)

static struct dma_chan *chan_buf[MAX_ID][MAX_TYPE] = { NULL };
static int type_buf[MAX_TYPE] = {DMA_XOR, DMA_XOR_VAL};
static int conv_buf[DMA_TX_TYPE_END] = {-1, 0, -1, 1, -1, -1, -1, -1, -1, -1, -1,
					-1, -1, -1, -1, -1};//support DMA_XOR,DMA_XOR_VAL

struct dma_chan *axera_async_tx_find_channel(struct async_submit_ctl *submit,
					     enum dma_transaction_type tx_type)
{
	dma_cap_mask_t mask;
	enum dma_transaction_type i;
	struct dma_async_tx_descriptor *depend_tx = submit->depend_tx;
	int conv_tx_type = conv_buf[tx_type];

	if (tx_type != DMA_XOR && tx_type != DMA_XOR_VAL) {
		return NULL;
	}

	if (submit->id > MAX_ID || submit->id < 1 || conv_tx_type < 0)
		return NULL;

	/* see if we can keep the chain on one channel */
	if (depend_tx &&
	    dma_has_cap(tx_type, depend_tx->chan->device->cap_mask))
		return depend_tx->chan;

	if (IS_ERR_OR_NULL(chan_buf[submit->id][conv_tx_type])) {
		dma_cap_zero(mask);
		dma_cap_set(tx_type, mask);
		chan_buf[submit->id][conv_tx_type] = dma_request_channel(mask, NULL, NULL);
		if (IS_ERR_OR_NULL(chan_buf[submit->id][conv_tx_type])) {
			return NULL;
		}
		for (i = 0; i < MAX_TYPE; i++) {
			if (dma_has_cap(type_buf[i], chan_buf[submit->id][conv_tx_type]->device->cap_mask))
				chan_buf[submit->id][conv_buf[type_buf[i]]] = chan_buf[submit->id][conv_tx_type];
		}
	}
	return chan_buf[submit->id][conv_tx_type];
}
EXPORT_SYMBOL_GPL(axera_async_tx_find_channel);

void axera_async_tx_issue_pending_all(void)
{
	int i;
	enum dma_transaction_type j;
	for (i = 0; i < MAX_ID; i++) {
		for (j = 0; j < MAX_TYPE; j++) {
			if (chan_buf[i][j] != NULL) {
				chan_buf[i][j]->device->device_issue_pending(chan_buf[i][j]);
			}
		}
	}
}
EXPORT_SYMBOL_GPL(axera_async_tx_issue_pending_all);

void axera_async_tx_issue_pending(int id)
{
	enum dma_transaction_type j;

	if (id > MAX_ID)
		return ;

	for (j = 0; j < MAX_TYPE; j++) {
		if (chan_buf[id][j] != NULL) {
			chan_buf[id][j]->device->device_issue_pending(chan_buf[id][j]);
		}
	}
}
EXPORT_SYMBOL_GPL(axera_async_tx_issue_pending);
