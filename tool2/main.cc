// efm32 specific headers
#include "em_chip.h"
//#include "em_device.h"
//#include "em_int.h"
#include "em_timer.h"
#include "em_adc.h"

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

	ym::reset();
	ym::syncTimer1();

	ym::playSine(0, 0x261); // max volume

	adc0::enableDMA();
	adc0::start1(true); // sample every 72 clocks
	led::toggle(9);

	while (true) {
		led::toggle(8);

		// TODO use ISR instead of polling
		// wait till buffer full
		int16_t* buf;
		do {
			buf = (int16_t*)adc0::fullBufferPtr;
		} while (!buf);

		usbcdc::write(buf, 1024 * 2);

		adc0::fullBufferPtr = NULL;
	}
	return 0;
}
