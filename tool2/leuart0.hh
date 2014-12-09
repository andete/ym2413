#ifndef LEUART_HH
#define LEUART_HH

#include "dma.hh"
#include "em_gpio.h"
#include "em_leuart.h"

namespace leuart0 {

// extra serial interface for debugging

// PD4: TX
// PD5: RX

void setup();

} // namespace leuart0

extern "C" void LEUART0_IRQHandler();

#endif
