#include "adc0.hh"
#include "em_adc.h"
#include "em_prs.h"

namespace adc0 {

void setup()
{

	// Route timer 1 to PRS channel 5
	PRS_SourceSignalSet(
		5,                             // channel 5
		PRS_CH_CTRL_SOURCESEL_TIMER1,  // timer 1
		PRS_CH_CTRL_SIGSEL_TIMER1OF,   // overflow
		prsEdgeOff);                   // leave signal as is.

	ADC_Init_TypeDef init = ADC_INIT_DEFAULT;
	init.ovsRateSel = adcOvsRateSel2;
	init.lpfMode    = adcLPFilterBypass;
	init.warmUpMode = adcWarmupKeepADCWarm;
	init.timebase   = ADC_TimebaseCalc(0);
	init.prescale   = 4-1; // ADC_CLK = 48MHz/4 = 12MHz
	init.tailgate   = 0;
	ADC_Init(ADC0, &init);

	ADC_InitSingle_TypeDef init_single = ADC_INITSINGLE_DEFAULT;
	init_single.prsSel     = adcPRSSELCh5; // channel 5
	init_single.acqTime    = adcAcqTime1;  // 1 cycle
	init_single.reference  = adcRefVDD;    // Buffered VDD reference
	init_single.resolution = adcRes12Bit;  // 12 bit
	init_single.input      = adcSingleInpCh0Ch1; // MO
	init_single.diff       = true;
	init_single.prsEnable  = true;         // use PRS
	init_single.leftAdjust = false;
	init_single.rep        = false;        // wait for next PRS event
	ADC_InitSingle(ADC0, &init_single);
}

void start()
{
	// not needed when using PRS?
	//ADC_Start(ADC0, adcStartSingle);
}

void stop()
{
	ADC_Reset(ADC0);
}

uint16_t getValue()
{
	// wait till data ready
	while ((ADC0->STATUS & ADC_STATUS_SINGLEDV) == 0) {}
	return ADC0->SINGLEDATA;
}

} // namespace adc0
