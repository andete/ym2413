#include "ym.hh"
#include "timer0.hh"
//#include "em_cmu.h"
#include "em_gpio.h"
//#include "em_timer.h"

namespace ym {

static const int P_D0    =  0;
static const int P_D1    =  1;
static const int P_D2    =  2;
static const int P_D3    =  3;
static const int P_D4    =  4;
static const int P_D5    =  5;
static const int P_D6    =  6;
static const int P_D7    =  7;
static const int P_A0    =  8;
static const int P_WE    =  9;
static const int P_CS    = 10;
static const int P_IC    = 11;
static const int P_XIN   = 12;
static const int P_XOUT  = 13;
static const int COUNT   = 14;

static const int A0   = 1 << P_A0;
static const int N_WE = 1 << P_WE;
static const int N_CS = 1 << P_CS;
static const int N_IC = 1 << P_IC;

struct PinInfo {
	GPIO_Port_TypeDef port;
	unsigned pin;
	bool negative_logic;
} pins[COUNT] =
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

// Set all (12) output pins to the YM2413 at once.
//
static inline void setPins(uint8_t data, bool a0, bool we, bool cs, bool ic)
{
	uint32_t value = data
	               | (a0 ? A0 : 0)
	               | (we ? 0 : N_WE)
	               | (cs ? 0 : N_CS)
	               | (ic ? 0 : N_IC);
	GPIO->P[gpioPortC].DOUT = value;
}

void reset()
{
	//      data, a0, we,    cs,    ic
	setPins(0,    0,  false, false, true ); // activate IC
	timer0::busyWaitN(200);                 // keep for 200 cycles (should be at least 80)
	setPins(0,    0,  false, false, false); // deactivate IC
}

void writeReg(uint8_t reg, uint8_t value)
{
	static const int EXTRA = 10; // more time between writes than strictly required

	//      data,  a0, we,    cs,    ic
	setPins(reg,   0,  true,  false, false); // setup D7-0, A0, WE
	timer0::busyWaitN(1);
	setPins(reg,   0,  true,  true,  false); // activate CS
	timer0::busyWaitN(1);
	setPins(reg,   0,  false, false, false); // deactivate WE,CS

	timer0::busyWaitN(12-1-1 + EXTRA);

	setPins(value, 1,  true,  false, false); // setup D7-0, A0, WE
	timer0::busyWaitN(1);
	setPins(value, 1,  true,  true,  false); // activate CS
	timer0::busyWaitN(1);
	setPins(value, 1,  false, false, false); // deactivate WE,CS

	timer0::busyWaitN(84-1-1 + EXTRA);
}

/*void start_clock(uint32_t freq)
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
}*/

} // namespace ym
