#ifndef ADC0_HH
#define ADC0_HH

#include <cstdint>

namespace adc0 {

extern volatile int16_t* volatile fullBufferPtr;

void setup();
void enableDMA();
void disableDMA();
void start0(bool irq); // every YM2413 clock
void start1(bool irq); // every 72 YM2413 clocks
void stop();
uint16_t pollValue();

} // namespace adc0

extern "C" void ADC0_IRQHandler();

#endif
