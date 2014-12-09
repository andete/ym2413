#ifndef DMA_HH
#define DMA_HH

#include "em_dma.h"

namespace dma {

// list the DMA channel numbers here
static const int LEUART0_DMA_CHANNEL = 0;

extern DMA_DESCRIPTOR_TypeDef control_block[DMA_CHAN_COUNT * 2];

void setup();

} // namespace dma

#endif
