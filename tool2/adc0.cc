#include "adc0.hh"
#include "em_adc.h"

namespace adc0 {

void setup()
{
	// TODO document
	ADC_Init_TypeDef init = ADC_INIT_DEFAULT;

	init.ovsRateSel = adcOvsRateSel2;
	init.lpfMode    = adcLPFilterBypass;
	init.warmUpMode = adcWarmupKeepADCWarm;
	init.timebase   = ADC_TimebaseCalc(0);
	init.prescale   = ADC_PrescaleCalc(7000000, 0);
	init.tailgate   = 0;

	ADC_Init(ADC0, &init);
}

} // namespace adc0
