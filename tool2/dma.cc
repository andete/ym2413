#include "dma.hh"

namespace dma {

// DMA control block, must be aligned to 256.
DMA_DESCRIPTOR_TypeDef control_block[DMA_CHAN_COUNT * 2] __attribute__ ((aligned(256)));

static DMA_Init_TypeDef dmaInit =
{
	.hprot        = 0,             // No descriptor protection
	.controlBlock = control_block, // DMA control block alligned to 256
};

void setup()
{
	DMA_Init(&dmaInit);
}

} // namespace dma
