// efm32 specific headers
#include "em_chip.h"
//#include "em_device.h"
//#include "em_int.h"
#include "em_timer.h"

// own code
#include "dma.hh"
#include "tick.hh"
#include "clock.hh"
#include "adc0.hh"
#include "leuart0.hh"
#include "swd.hh"
#include "usart0.hh"
#include "led.hh"
#include "timer0.hh"
#include "usbcdc.hh"
#include "ym.hh"

static void setup()
{
	// Chip errata
	CHIP_Init();

	// setup all subsystems
	tick::setup();
	clock::setup();
	dma::setup();
	adc0::setup();
	leuart0::setup();
	timer0::setup();
	swd::setup();
	usart0::setup();
	led::setup();
	ym::setup();
	usbcdc::setup();
}

int main()
{
	setup();

	tick::delay(100);
	adc0::start();

	ym::reset();
	// setup custom instrument
	ym::writeReg(0, 0x20);
	ym::writeReg(1, 0x23);
	ym::writeReg(2, 0x3f);
	ym::writeReg(3, 0x00);
	ym::writeReg(4, 0xff);
	ym::writeReg(5, 0xff);
	ym::writeReg(6, 0x0f);
	ym::writeReg(7, 0x0f);

	ym::writeReg(0x10,  171); // frequency (8 lower bits)
	ym::writeReg(0x30, 0x00); // select custom instrument, maximum volume
	ym::writeReg(0x20,   28); // write frequency (upper 4 bits), set key-on

	while (true) {
		led::short_demo();
		//tick::delay(2000);
		//usbcdc::print("Hello, world 2\r\n");
		//tick::delay(2000);

		static const int N = 10000;
		uint16_t buffer[N] __attribute__((aligned(4)));
		for (int i = 0; i < N; ++i) {
			buffer[i] = adc0::getValue();
			//buffer[i + 1] = TIMER_CounterGet(TIMER0);
			//buffer[i + 1] = TIMER_CounterGet(TIMER1);
		}
		usbcdc::write(buffer, sizeof(buffer));
	}
	return 0;
}
