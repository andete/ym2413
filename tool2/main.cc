// efm32 specific headers
#include "em_chip.h"
//#include "em_device.h"
//#include "em_int.h"

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
#include "board.hh" // TEMP HACK

void waitCycles(int n)
{
	// !! round-off errors !!
	tick::delay(n / 10);
}

void resetYm2413()
{
	setIC(0);        // activate reset
	waitCycles(200); // keep for 200 cycles (should be at least 80)
	setIC(1);        // deactive reset
	waitCycles(200);
}

void writeRegister(int reg, int value) {
	static const int EXTRA = 10; // more time between writes than strictly required

	setData(reg);
	setAddr(0);
	setWE(0);
	waitCycles(1);
	setCS(0);
	waitCycles(1);
	setCS(1);
	setWE(1);

	waitCycles(12-1-1 + EXTRA);
	
	setData(value);
	setAddr(1);
	setWE(0);
	waitCycles(1);
	setCS(0);
	waitCycles(1);
	setCS(1);
	setWE(1);
	
	waitCycles(84-1-1 + EXTRA);
}


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

	resetYm2413();
	writeRegister(16, 171); // frequency (8 lower bits)
	writeRegister(48,  64); // select instrument (piano), volume (maximum)
	// alternatives: 16 -> violin, 32 -> guitar
	//               48 -> piano, 64->flute, ...
	writeRegister(32,  28); // write frequency (upper 4 bits), set key-on

	while (true) {
		led::demo();
		tick::delay(2000);
		usbcdc::print("Hello, world 2\r\n");
		tick::delay(2000);
	}
	return 0;
}
