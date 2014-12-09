#ifndef YM_HH
#define YM_HH

#include "em_gpio.h"

namespace ym {

static const int D0    =  0;
static const int D1    =  1;
static const int D2    =  2;
static const int D3    =  3;
static const int D4    =  4;
static const int D5    =  5;
static const int D6    =  6;
static const int D7    =  7;
static const int A0    =  8;
static const int WE    =  9;
static const int CS    = 10;
static const int IC    = 11;
static const int XIN   = 12;
static const int XOUT  = 13;
static const int COUNT = 14;

struct PinInfo {
	GPIO_Port_TypeDef port;
	unsigned pin;
	bool negative_logic;
};
extern PinInfo pins[COUNT];

void setup();

// todo clever access methods
// as most pins are on gpio port C you can actually write
// to them all in one op

// see http://downloads.energymicro.com/documentation/doxygen/group__GPIO.html#ged42e78c374311828a1eb62fc45dae2d

inline void high(const uint8_t n)
{
	GPIO_PinOutSet(pins[n].port, pins[n].pin);
}

inline void low(const uint8_t n)
{
	GPIO_PinOutClear(pins[n].port, pins[n].pin);
}

void start_clock(uint32_t freq = 3579545);
void stop_clock();

} // namespace ym

#endif
