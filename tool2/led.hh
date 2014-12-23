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

void setup();
void demo();
void short_demo();


// Show a 4-bit error status.
void showError(int err);


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
