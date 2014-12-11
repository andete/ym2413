#include "led.hh"
#include "tick.hh"

namespace led {

LedInfo pins[NUM] =
{
	{ gpioPortA,  2 }, // LED 0
	{ gpioPortA,  3 }, // LED 1
	{ gpioPortA,  4 }, // LED 2
	{ gpioPortA,  5 }, // LED 3
	{ gpioPortA,  8 }, // LED 4
	{ gpioPortE,  8 }, // LED 5
	{ gpioPortE,  9 }, // LED 6
	{ gpioPortE, 10 }, // LED 7
	{ gpioPortE, 12 }, // LED 8
	{ gpioPortE, 13 }, // LED 9
};

void setup()
{
	for (unsigned i = 0; i < NUM; ++i) {
		GPIO_PinModeSet(pins[i].port, pins[i].pin, gpioModePushPull, 0);
	}
	// maybe do a funky blink pattern on startup
}

void demo()
{
	for (int j = 1; j < 100; ++j) {
		tick::delay(50 + j / 2);
		off((j - 1) % NUM);
		on ( j      % NUM);
	}
	for (int j = 100; j > 0; --j) {
		tick::delay(50 + j / 2);
		off((j + 1) % NUM);
		on ( j      % NUM);
	}
}
void short_demo()
{
	for (int i = 0; i < 10; ++i) off(i);
	for (int i = 0; i < 10; ++i) {
		on(i);
		tick::delay(150);
		off(i);
	}
}

} // namespace led
