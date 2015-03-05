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

uint16_t sineScript[] = {
	// R#0x00 <- 0x20
	0xC00,  1,
	0x800,  1,
	0xE00, 12-2,
	0xD20,  1,
	0x920,  1,
	0xF20, 84-2,
	// R#0x01 <- 0x20
	0xC01,  1,
	0x801,  1,
	0xE01, 12-2,
	0xD20,  1,
	0x920,  1,
	0xF20, 84-2,
	// R#0x02 <- 0x3F
	0xC02,  1,
	0x802,  1,
	0xE02, 12-2,
	0xD3F,  1,
	0x93F,  1,
	0xF3F, 84-2,
	// R#0x03 <- 0x00
	0xC03,  1,
	0x803,  1,
	0xE03, 12-2,
	0xD00,  1,
	0x900,  1,
	0xF00, 84-2,
	// R#0x04 <- 0xFF
	0xC04,  1,
	0x804,  1,
	0xE04, 12-2,
	0xDFF,  1,
	0x9FF,  1,
	0xFFF, 84-2,
	// R#0x05 <- 0xFF
	0xC05,  1,
	0x805,  1,
	0xE05, 12-2,
	0xDFF,  1,
	0x9FF,  1,
	0xFFF, 84-2,
	// R#0x06 <- 0x0F
	0xC06,  1,
	0x806,  1,
	0xE06, 12-2,
	0xD0F,  1,
	0x90F,  1,
	0xF0F, 84-2,
	// R#0x07 <- 0x0F
	0xC07,  1,
	0x807,  1,
	0xE07, 12-2,
	0xD0F,  1,
	0x90F,  1,
	0xF0F, 84-2,
	// R#0x10 <- 0x61
	0xC10,  1,
	0x810,  1,
	0xE10, 12-2,
	0xD61,  1,
	0x961,  1,
	0xF61, 84-2,
	// R#0x30 <- 0x00
	0xC30,  1,
	0x830,  1,
	0xE30, 12-2,
	0xD00,  1,
	0x900,  1,
	0xF00, 84-2,
	// R#0x20 <- 0x12
	0xC20,  1,
	0x820,  1,
	0xE20, 12-2,
	0xD12,  1,
	0x912,  1,
	0xF12, 84-2,
	// end
	0xF00, 0
};
uint16_t pianoScript[] = {
	// R#0x10 <- 0x61
	0xC10,  1,
	0x810,  1,
	0xE10, 12-2,
	0xD61,  1,
	0x961,  1,
	0xF61, 84-2,
	// R#0x30 <- 0x30
	0xC30,  1,
	0x830,  1,
	0xE30, 12-2,
	0xD30,  1,
	0x930,  1,
	0xF30, 84-2,
	// R#0x20 <- 0x12
	0xC20,  1,
	0x820,  1,
	0xE20, 12-2,
	0xD12,  1,
	0x912,  1,
	0xF12, 84-2,
	// end
	0xF00, 0
};

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
	adc0::callibrateOffset();

	ym::reset();
	ym::syncTimer1();

	//ym::playSine(0, 0x261); // max volume
	//timer0::playScript(sineScript, nullptr); // constant (max) amplitude
	//timer0::playScript(pianoScript); // amplitude decays over time

	adc0::enableDMA();
	//adc0::start1(true); // sample every 72 clocks

	// Enter event handling loop
	event::loop(); // does not return
	return 0;
}
