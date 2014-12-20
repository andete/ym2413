#include "adc0.hh"
#include "led.hh"
#include "timer0.hh"
#include "ym.hh"
#include "em_gpio.h"
#include "em_timer.h"
#include <cstdlib>

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

void playSine(int volume, int freq)
{
	// setup custom instrument
	ym::writeReg(0, 0x20);
	ym::writeReg(1, 0x23);
	ym::writeReg(2, 0x3f);
	ym::writeReg(3, 0x00);
	ym::writeReg(4, 0xff);
	ym::writeReg(5, 0xff);
	ym::writeReg(6, 0x0f);
	ym::writeReg(7, 0x0f);

	ym::writeReg(0x10, freq & 0xff); // frequency (8 lower bits)
	ym::writeReg(0x30, (0 << 16) | (volume & 0x0f)); // custom instrument / volume
	ym::writeReg(0x20, 0x10 | ((freq >> 8) & 0x0f)); // key-on / frequency (upper 4 bits)
}


// The YM2413 can output 9 music channels (or 6 + 5). The channels are output
// in a pattern that repeats every 72 (YM2413) clock cycles. Timer0 ticks for
// every YM2413 clock. Timer1 ticks 72 times slower, that allows to always
// sample the same value in this pattern. Though we still need to select which
// of the 72 clocks we want to sample. And that's what this routines does.
//
// We found out that the 72 cycles are divided in 18 slots of 4 cycles each
// (actually this is also specified in the datasheet). For 3 of the 4 cycles
// in a slot the RO or MO analog output pin has a non-zero value.
//
// This routine tries to setup timer1 so that it has value=0 when the middle
// of the 3 samples of music channel 1 is output on MO.
void syncTimer1()
{
	playSine(0, 0x261); // max volume

	adc0::disableDMA();
	adc0::start0(false); // every YM2413 clock

	while (true) {
		led::short_demo();

		int16_t  sample[72];
		uint16_t timer [72];

		// Capture 72 consecutive YM2413 samples,
		// record the timer1 value for each.
		for (int i = 0; i < 72; ++i) {
			sample[i] = adc0::pollValue();
			timer [i] = TIMER_CounterGet(TIMER1); // 0..71
		}

		// Search the location in the buffer where timer1 had the value 2.
		int p = 0;
		for (/**/; p < 72; ++p) {
			if (timer[p] == 2) break;
		}
		// Once in a while the recorded timer values are off-by-one,
		// don't crash when that happens for value=2.
		if (p == 72) continue; // retry

		// Starting from that location search the first non-zero
		// (ignoring noise) in the sample data.
		int i = 0;
		for (/**/; i < 72; ++i) {
			int s = abs(sample[p]);
			if (s > 100) break; // well-above noise-level
			++p; if (p == 72) p = 0; // circular add
		}
		// It's possible the signal is close to zero, in that case we
		// won't find a clear peak in the signal and we try again
		if (i == 72) continue; // retry

		// The ym2413 signal lasts for 3 consecutive clocks, we
		// searched for the first non-zero sample. Add one to go to the
		// middle of the 3 samples.
		++p; if (p == 72) p = 0; // circular add

		// Look for the value of Timer1 at that middle sample. We
		// want Timer1 to have the value zero at that time. So wait
		// till timer1 has again that value and then reset it.
		uint16_t c = timer[p];
		while (TIMER_CounterGet(TIMER1) != c) {}
		TIMER_CounterSet(TIMER1, 0);

		// Done.
		break;
	}
	adc0::stop();
}

} // namespace ym
