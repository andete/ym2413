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

void waitCycles(int n)
{
	// !! round-off errors !!
	tick::delay(n / 10);
}

void resetYm2413()
{
	ym::setIC(0);        // activate reset
	waitCycles(200); // keep for 200 cycles (should be at least 80)
	ym::setIC(1);        // deactive reset
	waitCycles(200);
}

void writeRegister(uint8_t reg, uint8_t value) {
	static const int EXTRA = 10; // more time between writes than strictly required

	ym::setData(reg);
	ym::setAddr(0);
	ym::setWE(0);
	waitCycles(1);
	ym::setCS(0);
	waitCycles(1);
	ym::setCS(1);
	ym::setWE(1);

	waitCycles(12-1-1 + EXTRA);
	
	ym::setData(value);
	ym::setAddr(1);
	ym::setWE(0);
	waitCycles(1);
	ym::setCS(0);
	waitCycles(1);
	ym::setCS(1);
	ym::setWE(1);
	
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
	adc0::start();

	resetYm2413();
	// setup custom instrument
	writeRegister(0, 0x20);
	writeRegister(1, 0x23);
	writeRegister(2, 0x3f);
	writeRegister(3, 0x00);
	writeRegister(4, 0xff);
	writeRegister(5, 0xff);
	writeRegister(6, 0x0f);
	writeRegister(7, 0x0f);

	writeRegister(0x10,  171); // frequency (8 lower bits)
	writeRegister(0x30, 0x00); // select custom instrument, maximum volume
	writeRegister(0x20,   28); // write frequency (upper 4 bits), set key-on

	while (true) {
		led::demo();
		//tick::delay(2000);
		//usbcdc::print("Hello, world 2\r\n");
		//tick::delay(2000);

		static const int N = 10000;
		uint16_t buffer[N] __attribute__((aligned(4)));
		for (int i = 0; i < N; ++i) {
			buffer[i] = adc0::getValue();
		}
		usbcdc::write(buffer, sizeof(buffer));
	}
	return 0;
}
