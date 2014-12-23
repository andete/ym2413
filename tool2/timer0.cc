#include "timer0.hh"
#include "em_cmu.h"
#include "em_gpio.h"
#include "em_timer.h"

namespace timer0 {

static unsigned scriptDelay = 0;
static uint16_t* scriptPtr = nullptr;

void setup()
{
	// Enable clock for TIMER0
	CMU_ClockEnable(cmuClock_TIMER0, true);
	CMU_ClockEnable(cmuClock_TIMER1, true);

	// Timer-1, chained to timer-0, overflows every 72 YM2413 ticks
	TIMER_Init_TypeDef timer1Init = TIMER_INIT_DEFAULT;
	timer1Init.clkSel = timerClkSelCascade; // chain to timer-0
	timer1Init.enable = false; // don't start yet
	timer1Init.sync = true;    // start when timer-0 starts
	TIMER_TopSet(TIMER1, 72-1);
	TIMER_Init(TIMER1, &timer1Init);

	// TODO
	// Pin PA0 is configured to Input enabled
	GPIO->P[0].MODEL = (GPIO->P[0].MODEL & ~_GPIO_P_MODEL_MODE0_MASK) | GPIO_P_MODEL_MODE0_INPUT;
	// Pin PA1 is configured to Input enabled
	GPIO->P[0].MODEL = (GPIO->P[0].MODEL & ~_GPIO_P_MODEL_MODE1_MASK) | GPIO_P_MODEL_MODE1_INPUT;

	// set PA0 and PA1 as output
	GPIO_PinModeSet(gpioPortA, 0, gpioModePushPull, 0);
	GPIO_PinModeSet(gpioPortA, 1, gpioModePushPull, 0);

	// Select CC channel parameters
	TIMER_InitCC_TypeDef timerCCInit = {
		.eventCtrl  = timerEventEveryEdge,
		.edge       = timerEdgeBoth, // we don't care about input
		.prsSel     = timerPRSSELCh0, // not used
		.cufoa      = timerOutputActionNone,
		.cofoa      = timerOutputActionNone,
		.cmoa       = timerOutputActionNone,
		.mode       = timerCCModePWM,
		.filter     = false, // no filter
		.prsInput   = false, // no TIMERnCCx input
		.coist      = false, // don't clear output when counter disabled
		.outInvert  = false, // don't invert output
	};

	// Configure CC channel 0
	TIMER_InitCC(TIMER0, 0, &timerCCInit);

	// Route CC0 to location 0 (PA0) and enable pin
	TIMER0->ROUTE |= (TIMER_ROUTE_CC0PEN | TIMER_ROUTE_LOCATION_LOC0);

	// TODO: actual timer setup to create our signal
	// Set Top Value
	const uint32_t PWM_FREQ = 3*1000*1000/10; // 3 Mhz divided by 10
	const uint32_t topValue = CMU_ClockFreqGet(cmuClock_HFPER)/PWM_FREQ;
	TIMER_TopSet(TIMER0, topValue);

	// 50% duty cycle
	TIMER_CompareSet(TIMER0, 0, topValue/2);

	// TODO
	// Select timer parameters
	TIMER_Init_TypeDef timer0Init = {
		.enable     = true,
		.debugRun   = true, // Keep counter during debug halt
		.prescale   = timerPrescale1, // no pre-scaling
		.clkSel     = timerClkSelHFPerClk, // Select HFPER clock
		.count2x    = false, // double count mode
		.ati        = false, //
		.fallAction = timerInputActionNone, // no action
		.riseAction = timerInputActionNone, // no action
		.mode       = timerModeUp, // count up
		.dmaClrAct  = false, // no dma involved
		.quadModeX4 = false, // no quadrature mode
		.oneShot    = false, // keep running
		.sync       = false, // 
	};

	// Enable overflow interrupt
	TIMER_IntEnable(TIMER0, TIMER_IF_OF);
	NVIC_EnableIRQ(TIMER0_IRQn);

	// Configure timer
	TIMER_Init(TIMER0, &timer0Init);
}

void busyWaitN(uint32_t cycles)
{
	uint32_t half = TIMER_TopGet(TIMER0) / 2;
	while (cycles--) {
		// wait till we're in the 2nd half
		while (TIMER_CounterGet(TIMER0) < half) {}
		// wait till we're in the 1st half again
		while (TIMER_CounterGet(TIMER0) >= half) {}
	}
}

void playScript(uint16_t* script)
{
	__disable_irq();
	GPIO->P[gpioPortC].DOUT = script[0];
	scriptDelay             = script[1];
	scriptPtr = script + 2;
	__enable_irq();
}
void stopScript()
{
	__disable_irq();
	scriptDelay = 0;
	__enable_irq();
}
static inline void stepScript()
{
	// Is script running?
	if (scriptDelay == 0) return;

	// Do we need to do something this cycle?
	--scriptDelay;
	if (scriptDelay != 0) return;

	// Write new outputs and set next delay.
	GPIO->P[gpioPortC].DOUT = scriptPtr[0];
	scriptDelay             = scriptPtr[1];
	scriptPtr += 2;
}

} // namespace timer0

// Runs in ISR context!
void TIMER0_IRQHandler()
{
	// Clear flag for TIMER0 overflow interrupt
	TIMER_IntClear(TIMER0, TIMER_IF_OF);

	timer0::stepScript();
}
