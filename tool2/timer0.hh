#ifndef TIMER0_HH
#define TIMER0_HH

#include <cstdint>

namespace timer0 {

void setup();
void busyWaitN(uint32_t cycles);

using PlayCallback = void (*)();
void playScript(uint16_t* script, PlayCallback callback);
void stopScript();

} // namespace timer0

//extern "C" void TIMER0_IRQHandler();

#endif
