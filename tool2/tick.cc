#include "tick.hh"
#include "em_cmu.h"

namespace tick {

volatile uint32_t jiffy = 0;

void setup()
{
	// Setup SysTick Timer for 1 msec interrupts
	if (SysTick_Config(CMU_ClockFreqGet(cmuClock_CORE) / 1000)) {
		while (1) {}
	}
}

} // namespace tick


void SysTick_Handler()
{
	++tick::jiffy; // increment counter necessary in delay()
}
