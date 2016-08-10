/*
 * DMA.c
 *
 * Created: 8/7/2013 8:16:20 AM
 *  Author: pc
 */ 
#include <asf.h>
#include "ModemDMA.h"

#define DMA_BUFFER_SIZE (sizeof(uint16_t))	// 1 ADC result
static void dma_transfer_done(enum dma_channel_status);

uint16_t volatile adc_samples[DMA_BUFFER_SIZE/2];

static struct dma_channel_config dmach_conf;
static volatile unsigned dmacount = 0;

static void dma_transfer_done(enum dma_channel_status status)
{
	dmacount++;
}
/*
	Hook up DMA transfer to ADC CH2 completion
	Copy 4 words from CH0RES
*/
void dma_init()
{
	memset(&dmach_conf, 0, sizeof(dmach_conf));
	dma_disable();
	dma_channel_reset(DMA_CHANNEL_ADC);
	
	dma_channel_set_burst_length(&dmach_conf, DMA_CH_BURSTLEN_8BYTE_gc);
	dma_channel_set_transfer_count(&dmach_conf, DMA_BUFFER_SIZE);

	dma_channel_set_src_reload_mode(&dmach_conf,DMA_CH_SRCRELOAD_TRANSACTION_gc);
	dma_channel_set_dest_reload_mode(&dmach_conf,DMA_CH_DESTRELOAD_TRANSACTION_gc);

	dma_channel_set_src_dir_mode(&dmach_conf, DMA_CH_SRCDIR_INC_gc);
	dma_channel_set_source_address(&dmach_conf,(uint16_t)(uintptr_t)&ADCA.CH2RES);

	dma_channel_set_dest_dir_mode(&dmach_conf, DMA_CH_DESTDIR_INC_gc);
	dma_channel_set_destination_address(&dmach_conf,(uint16_t)(uintptr_t)adc_samples);

	dma_channel_set_trigger_source(&dmach_conf, DMA_CH_TRIGSRC_ADCA_CH2_gc);	// ADCA CH0 complete

	dma_channel_set_single_shot(&dmach_conf);
	
	dma_enable();

	dma_set_callback(DMA_CHANNEL_ADC, dma_transfer_done);
	
	dma_channel_set_interrupt_level(&dmach_conf, DMA_INT_LVL_LO);

	dma_channel_write_config(DMA_CHANNEL_ADC, &dmach_conf);
	dma_channel_enable(DMA_CHANNEL_ADC);
}
