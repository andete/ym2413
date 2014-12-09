#include "clock.hh"
#include "em_cmu.h"

namespace clock {

// all clocks are enabled together
void setup()
{
	// Use crystal oscillator for HFXO
	CMU->CTRL |= CMU_CTRL_HFXOMODE_XTAL;

	// HFXO setup
	CMU->CTRL = (CMU->CTRL & ~_CMU_CTRL_HFXOBOOST_MASK) | CMU_CTRL_HFXOBOOST_100PCENT;

	// Enable HFXO as high frequency clock, HFCLK
	CMU_ClockSelectSet(cmuClock_HF,cmuSelect_HFXO);

	// Enable LFRCO oscillator
	CMU_OscillatorEnable(cmuOsc_LFRCO, true, true);

	// Select LFRCO as clock source for LFBCLK
	CMU_ClockSelectSet(cmuClock_LFB, cmuSelect_LFRCO);

	// Enable GPIO clock
	CMU_ClockEnable(cmuClock_GPIO, true);

	// Enable clock for ADC0
	CMU_ClockEnable(cmuClock_ADC0, true);

	// Enable clock for DMA
	CMU_ClockEnable(cmuClock_DMA, true);

	// Enable clock for LEUART0
	CMU_ClockEnable(cmuClock_LEUART0, true);

	// Enable clock for TIMER0
	CMU_ClockEnable(cmuClock_TIMER0, true);
}

} // namespace clock
