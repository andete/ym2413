#ifndef LED_HH
#define LED_HH

#include "em_gpio.h"

namespace led {

static const unsigned NUM = 10;

struct LedInfo {
	GPIO_Port_TypeDef port;
	unsigned pin;
};
extern LedInfo pins[NUM];

extern volatile int newError;

void setup();
void demo();
void short_demo();


// Show a 4 bit error status.
// This part is very fast (can be called from an ISR). The new status is only set from the main loop.
inline void setError(int err)
{
	newError = err;
}

// Immediately show a new error status
void showError(int err);

// Second half of the setError() routine, should be called periodically from the main loop.
inline void checkError()
{
	if (newError) {
		showError(newError);
		newError = 0;
	}
}



inline void on(const uint8_t led)
{
	GPIO_PinOutSet(pins[led].port, pins[led].pin);
}

inline void off(const uint8_t led)
{
	GPIO_PinOutClear(pins[led].port, pins[led].pin);
}

inline void toggle(const uint8_t led)
{
	GPIO_PinOutToggle(pins[led].port, pins[led].pin);
}

} // namespace led

#endif
