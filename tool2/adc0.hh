#ifndef ADC0_HH
#define ADC0_HH

#include <cstdint>

namespace adc0 {

extern volatile int16_t* volatile fullBufferPtr;

void setup();
void start();
void stop();
uint16_t getValue();

} // namespace adc0

extern "C" void ADC0_IRQHandler();

#endif
