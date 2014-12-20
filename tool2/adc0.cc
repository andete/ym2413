#include "adc0.hh"
#include "led.hh"
#include "dma.hh"
#include "em_adc.h"
#include "em_dma.h"
#include "em_prs.h"

namespace adc0 {

volatile int16_t* volatile fullBufferPtr = NULL;

static const unsigned NUM_SAMPLES = 1024;
volatile int16_t buffer1[NUM_SAMPLES]; // ADC/DMA ping-pong between these
volatile int16_t buffer2[NUM_SAMPLES]; //  two buffers

DMA_CB_TypeDef cb; // DMA callback structure

static void dmaTransferComplete(unsigned channel, bool primary, void* /*user*/)
{
	if (fullBufferPtr != NULL) {
		// buffer hasn't been send yet over USB
		led::error(6);
	}
	fullBufferPtr = primary ? buffer1 : buffer2;

	// Re-activate the DMA
	DMA_RefreshPingPong(
		channel,
		primary,
		false,           // use burst
		NULL,            // no new dst address
		NULL,            // no new src address
		NUM_SAMPLES - 1, // size of this chunk
		false);          // do not stop after this chunk
	led::toggle(0);
}

static void setupDMA()
{
	// setup callback function
	cb.cbFunc = dmaTransferComplete;
	cb.userPtr = NULL;

	// setup channel
	DMA_CfgChannel_TypeDef chnlCfg;
	chnlCfg.highPri   = false;
	chnlCfg.enableInt = true;
	chnlCfg.select    = DMAREQ_ADC0_SINGLE;
	chnlCfg.cb        = &cb;
	DMA_CfgChannel(dma::ADC0_CHANNEL, &chnlCfg);

	// Setting up channel descriptor
	DMA_CfgDescr_TypeDef descrCfg;
	descrCfg.dstInc  = dmaDataInc2;    // advance 16-bit in dst
	descrCfg.srcInc  = dmaDataIncNone; // always read same register
	descrCfg.size    = dmaDataSize2;   // 16-bit items
	descrCfg.arbRate = dmaArbitrate1;  // 
	descrCfg.hprot   = 0;
	DMA_CfgDescr(dma::ADC0_CHANNEL, true,  &descrCfg);
	DMA_CfgDescr(dma::ADC0_CHANNEL, false, &descrCfg);
}

void enableDMA()
{
	// enable ping-pong transfer
	DMA_ActivatePingPong(
		dma::ADC0_CHANNEL,          // channel
		false,                      // don't use use burst
		(void*)buffer1,             // primary dst
		(void*)&(ADC0->SINGLEDATA), // primary src
		NUM_SAMPLES - 1,            // 
		(void*)buffer2,             // alternate dst
		(void*)&(ADC0->SINGLEDATA), // alternate src
		NUM_SAMPLES - 1);           //
}

void disableDMA()
{
	//DMA_ChannelEnable(dma::ADC0_CHANNEL, false);
	DMA->CHENC = 1 << dma::ADC0_CHANNEL;
}

void setup()
{
	setupDMA();

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
	init_single.input      = adcSingleInpCh0Ch1; // RO-MO
	init_single.diff       = true;
	init_single.prsEnable  = true;         // use PRS
	init_single.leftAdjust = false;
	init_single.rep        = false;        // wait for next PRS event
	ADC_InitSingle(ADC0, &init_single);
}

static void irqOnOverflow(bool enable)
{
	if (enable) {
		// Enable ADC single overflow interrupt to indicate lost samples.
		ADC_IntClear(ADC0, ADC_IFC_SINGLEOF);
		ADC_IntEnable(ADC0, ADC_IEN_SINGLEOF);
		NVIC_EnableIRQ(ADC0_IRQn);
	} else {
		ADC_IntDisable(ADC0, ADC_IEN_SINGLEOF);
		NVIC_DisableIRQ(ADC0_IRQn);
	}
}

void start0(bool irq)
{
	// Start ADC by routing the output of timer0 to the trigger of the ADC

	irqOnOverflow(irq);

	// Route timer 0 to PRS channel 5
	PRS_SourceSignalSet(
		5,                             // channel 5
		PRS_CH_CTRL_SOURCESEL_TIMER0,  // timer0
		PRS_CH_CTRL_SIGSEL_TIMER0OF,   // overflow
		prsEdgeOff);                   // leave signal as is.
}

void start1(bool irq)
{
	// Start ADC by routing the output of timer1 to the trigger of the ADC

	irqOnOverflow(irq);

	// Route timer 1 to PRS channel 5
	PRS_SourceSignalSet(
		5,                             // channel 5
		PRS_CH_CTRL_SOURCESEL_TIMER1,  // timer1
		PRS_CH_CTRL_SIGSEL_TIMER1OF,   // overflow
		prsEdgeOff);                   // leave signal as is.
}

void stop()
{
	// Disconnect the ADC trigger input
	PRS_SourceSignalSet(
		5,                             // channel 5
		PRS_CH_CTRL_SOURCESEL_NONE,    // none
		PRS_CH_CTRL_SIGSEL_TIMER1OF,   // -
		prsEdgeOff);                   // -

	//ADC_Reset(ADC0);
}

uint16_t pollValue()
{
	// wait till data ready
	while ((ADC0->STATUS & ADC_STATUS_SINGLEDV) == 0) {}
	return ADC0->SINGLEDATA;
}


} // namespace adc0

void ADC0_IRQHandler()
{
	// Clear interrupt flag
	ADC_IntClear(ADC0, ADC_IFC_SINGLEOF);
	adc0::stop();

	// ERROR: ADC Result overflow has occured.
	// This indicates that the DMA is not able to keep up with the ADC
	// sample rate and that a samples has been written to the ADC result
	// registers before the DMA was able to fetch the previous result.
	// TODO report error outside of ISR
	led::error(5);
}
