/* -*- Mode: C; tab-width: 2; indent-tabs-mode: nil -*- */

/* DMA control block, must be aligned to 256. */
DMA_DESCRIPTOR_TypeDef dma_control_block[DMA_CHAN_COUNT * 2] __attribute__ ((aligned(256)));

namespace dma {

  DMA_Init_TypeDef dmaInit = {
    .hprot        = 0,                 // No descriptor protection
    .controlBlock = dma_control_block, // DMA control block alligned to 256
  };

  static void setup() {

    DMA_Init(&dmaInit);

  }

}
