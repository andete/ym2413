// efm32 specific headers
#include "em_adc.h"
#include "em_chip.h"
#include "em_emu.h"
#include "em_timer.h"

// own code
#include "dma.hh"
#include "event.hh"
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

	// Enter event handling loop
	event::loop(); // does not return
	return 0;
}
