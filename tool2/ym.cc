#include "ym.hh"
#include "timer0.hh"
#include "em_cmu.h"
#include "em_gpio.h"
#include "em_timer.h"

namespace ym {

PinInfo pins[COUNT] =
{
	{ gpioPortC,  0, false }, // D0
	{ gpioPortC,  1, false }, // D1
	{ gpioPortC,  2, false }, // D2
	{ gpioPortC,  3, false }, // D3
	{ gpioPortC,  4, false }, // D4
	{ gpioPortC,  5, false }, // D5
	{ gpioPortC,  6, false }, // D6
	{ gpioPortC,  7, false }, // D7
	{ gpioPortC,  8, false }, // A0
	{ gpioPortC,  9, true  }, // ~WE
	{ gpioPortC, 10, true  }, // ~CS
	{ gpioPortC, 11, true  }, // ~IC
	{ gpioPortA,  0, false }, // XIN
	{ gpioPortA,  1, false }, // XOUT
};

void setup()
{
	for (int i = 0; i < COUNT; ++i) {
		GPIO_PinModeSet(pins[i].port, pins[i].pin, gpioModePushPull,
		                pins[i].negative_logic);
	}
}

// todo clever access methods
// as most pins are on gpio port C you can actually write
// to them all in one op

// see http://downloads.energymicro.com/documentation/doxygen/group__GPIO.html#ged42e78c374311828a1eb62fc45dae2d

void start_clock(uint32_t freq)
{
	const uint32_t topValue = CMU_ClockFreqGet(cmuClock_HFPER) / freq;
	TIMER_TopSet(TIMER0, topValue);
	// 50 % duty cycle
	TIMER_CompareSet(TIMER0, 0, topValue / 2);
	TIMER_Enable(TIMER0, true);
}

void stop_clock()
{
	TIMER_Enable(TIMER0, false);
}

} // namespace ym
