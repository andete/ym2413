#ifndef TIMER0_HH
#define TIMER0_HH

#include <cstdint>

namespace timer0 {

void setup();
void busyWaitN(uint32_t cycles);

} // namespace timer0

//extern "C" void TIMER0_IRQHandler();

#endif
