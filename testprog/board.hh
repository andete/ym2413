// efm32 specific headers
#include "em_device.h"
#include "em_chip.h"
#include "em_adc.h"
#include "em_cmu.h"
#include "em_usart.h"
#include "em_leuart.h"
#include "em_dma.h"
#include "em_gpio.h"
#include "em_usb.h"
#include "em_int.h"
#include "em_timer.h"
#include "efm32gg_uart.h"

// own code
#include "dma.hh"
#include "tick.hh"
#include "clock.hh"
#include "adc0.hh"
#include "leuart0.hh"
#include "timer0.hh"
#include "swd.hh"
#include "usart0.hh"
#include "led.hh"
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

static inline void delay(int millisec)
{
	tick::delay(millisec);
}

static inline void setLed(int num, bool status)
{
	if (status) {
		led::on(num);
	} else {
		led::off(num);
	}
}

static inline void printUsb(const char* message)
{
	usbcdc::print(message);
}

static void end()
{
	while (true) {}
}


// write /IC (false->active, true->not active)
static inline void setIC(bool ic)
{
	if (ic) {
		ym::high(YM_IC);
	} else {
		ym::low (YM_IC);
	}
}

// write /CS (false->active, true->not active)
static inline void setCS(bool cs)
{
	if (cs) {
		ym::high(YM_CS);
	} else {
		ym::low (YM_CS);
	}
}

// write /CS (false->active, true->not active)
static inline void setWE(bool we)
{
	if (we) {
		ym::high(YM_WE);
	} else {
		ym::low (YM_WE);
	}
}

// write XOUT/XIN
//  true  -> positive edge
//  false -> negative edge
static inline void setClk(bool clk)
{
	// TODO should we write XOUT or XIN, or both?
	if (clk) {
		ym::high(YM_XOUT);
	} else {
		ym::low (YM_XOUT);
	}
}

// write D7-D0
static inline void setData(int data)
{
	// TODO write 8 bits at-a-time
	for (int i = 0; i < 8; ++i) {
		if (data & (1 << i)) {
			ym::high(YM_D0 + i);
		} else {
			ym::low (YM_D0 + i);
		}
	}
}

// write A0
static inline void setAddr(int addr)
{
	if (addr) {
		ym::high(YM_A0);
	} else {
		ym::low (YM_A0);
	}
}

short sampleMO()
{
	return 0; // TODO
}

short sampleRO()
{
	return 0; // TODO
}

void sendDataUsb(const uint8_t* buf, uint16_t len)
{
	usbcdc::write(buf, len);
}
