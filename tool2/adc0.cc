#include "adc0.hh"
#include "em_adc.h"

namespace adc0 {

void setup()
{
	ADC_Init_TypeDef init = ADC_INIT_DEFAULT;
	init.ovsRateSel = adcOvsRateSel2;
	init.lpfMode    = adcLPFilterBypass;
	init.warmUpMode = adcWarmupKeepADCWarm;
	init.timebase   = ADC_TimebaseCalc(0);
	init.prescale   = ADC_PrescaleCalc(7000000, 0);
	init.tailgate   = 0;
	ADC_Init(ADC0, &init);

	ADC_InitSingle_TypeDef init_single = ADC_INITSINGLE_DEFAULT;
	init_single.prsSel     = adcPRSSELCh0; // don't care
	init_single.acqTime    = adcAcqTime1;  // 1 cycle
	init_single.reference  = adcRefVDD;    // Buffered VDD reference
	init_single.resolution = adcRes12Bit;  // 12 bit
	init_single.input      = adcSingleInpCh1; // MO
	init_single.diff       = false;
	init_single.prsEnable  = false;
	init_single.leftAdjust = false;
	init_single.rep        = true;
	ADC_InitSingle(ADC0, &init_single);
}

void start()
{
	ADC_Start(ADC0, adcStartSingle);
}

void stop()
{
	ADC_Reset(ADC0);
}

uint16_t getValue()
{
	// wait till data ready
	while ((ADC0->STATUS & ADC_STATUS_SINGLEDV) == 0) {}

	return ADC_DataSingleGet(ADC0);
}

} // namespace adc0
