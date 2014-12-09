#ifndef ADC0_HH
#define ADC0_HH

#include <cstdint>

namespace adc0 {

void setup();
void start();
void stop();
uint16_t getValue();

} // namespace adc0

#endif
