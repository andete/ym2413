#include "timer0.hh"
#include "em_cmu.h"
#include "em_gpio.h"
#include "em_timer.h"

namespace timer0 {

void setup()
{
	// Enable clock for TIMER0
	CMU_ClockEnable(cmuClock_TIMER0, true);

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
	TIMER_Init_TypeDef timerInit = {
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

	// TODO; we may want to have interrupts later to do 
	// some handling
	// Enable overflow interrupt
	//TIMER_IntEnable(TIMER0, TIMER_IF_OF);

	// Enable TIMER0 interrupt vector in NVIC
	//NVIC_EnableIRQ(TIMER0_IRQn);

	// Configure timer
	TIMER_Init(TIMER0, &timerInit);
}

} // namespace timer0

void TIMER0_IRQHandler()
{
	uint32_t compareValue;

	/* Clear flag for TIMER0 overflow interrupt */
	TIMER_IntClear(TIMER0, TIMER_IF_OF);

	compareValue = TIMER_CaptureGet(TIMER0, 0);
	/* increment duty-cycle or reset if reached TOP value */
	if (compareValue == TIMER_TopGet(TIMER0)) {
		TIMER_CompareBufSet(TIMER0, 0, 0);
	} else {
		TIMER_CompareBufSet(TIMER0, 0, ++compareValue);
	}
}
