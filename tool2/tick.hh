#ifndef TICK_HH
#define TICK_HH

#include <cstdint>

namespace tick {

extern volatile uint32_t jiffy;

void setup();

inline void delay(const uint32_t delay_ms)
{
	uint32_t start = jiffy;
	while ((jiffy - start) < delay_ms) {}
}

} // namespace tick

// systick interrupt handler
extern "C" void SysTick_Handler();

#endif
