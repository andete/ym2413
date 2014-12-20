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
	// tune to capture YM2413 channel 0
	TIMER_CounterSet(TIMER0,  0);
	TIMER_CounterSet(TIMER1, 20);

	// setup custom instrument
	ym::writeReg(0, 0x20);
	ym::writeReg(1, 0x23);
	ym::writeReg(2, 0x3f);
	ym::writeReg(3, 0x00);
	ym::writeReg(4, 0xff);
	ym::writeReg(5, 0xff);
	ym::writeReg(6, 0x0f);
	ym::writeReg(7, 0x0f);

	ym::writeReg(0x10, 0x61); // frequency (8 lower bits)
	ym::writeReg(0x30, 0x00); // select custom instrument, maximum volume
	ym::writeReg(0x20, 0x12); // write frequency (upper 4 bits), set key-on

	//ADC_IntEnable(ADC0, ADC_IEN_SINGLEOF);
	//NVIC_EnableIRQ(ADC0_IRQn);

	/*while (true) {
		//led::short_demo();
		led::toggle(8);

		//for (int j = 0; j < 72; ++j) {
		//	TIMER_CounterSet(TIMER0, 0);
		//	TIMER_CounterSet(TIMER1, j);
			static const int N = 20000;
			uint16_t buffer[N] __attribute__((aligned(4)));
			for (int i = 0; i < N; ++i) {
				buffer[i] = adc0::getValue();
				//buffer[i + 1] = TIMER_CounterGet(TIMER1);
			}
		//	buffer[0] = j;
			usbcdc::write(buffer, sizeof(buffer));
		//}
	}*/

	adc0::start();
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
