// efm32 specific headers
#include "em_adc.h"
#include "em_chip.h"
#include "em_emu.h"
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

	ym::reset();
	ym::syncTimer1();

	ym::playSine(0, 0x261); // max volume

	adc0::enableDMA();
	adc0::start1(true); // sample every 72 clocks

	// Main loop only reacts to events posted from ISRs.
	//   Idea: if the main loop becomes too heavy (too many different
	//     events to check for), then use an event queue.
	while (true) {
		// Sample buffer ready?
		if (int16_t* buf = (int16_t*)adc0::fullBufferPtr) {
			// Send to host over USB
			usbcdc::write(buf, 1024 * sizeof(int16_t));
			// and indicate bufferspace is available again
			//  TODO buffer actually only becomes available when USB has finished sending it
			adc0::fullBufferPtr = NULL;
			led::toggle(8);
		}

		// TODO listen to host commands (send over USB)

		// Has an ISR set an error?
		led::checkError();

		// Indicate main loop is still alive.
		led::toggle(9);

		// Enter EM1 sleep mode (peripherals active, CPU sleeping (wakeup by IRQ)).
		//  TODO this can go wrong when the IRQ triggered between this
		//  point and the point above where we tested the vlaue of e.g.
		//  fullBufferPtr.
		EMU_EnterEM1();
	}
	return 0;
}
