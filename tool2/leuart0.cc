#include "leuart0.hh"
#include "dma.hh"
#include "em_gpio.h"
#include "em_leuart.h"

namespace leuart0 {

// extra serial interface for debugging

// PD4: TX
// PD5: RX

static const int BUF_MAX = 255;
static char rxbuf[BUF_MAX] = { 0 };

LEUART_Init_TypeDef init =
{
	.enable   = leuartEnableRx, // Activate data reception on LEUn_RX pin.
	.refFreq  = 0, // Inherit the clock frequenzy from the LEUART clock source
	.baudrate = 9600,
	.databits = leuartDatabits8,
	.parity   = leuartNoParity,
	.stopbits = leuartStopbits1,
};

DMA_CfgChannel_TypeDef chnlCfg =
{
	.highPri   = false, // Normal priority
	.enableInt = false, // No interupt enabled for callback functions
	// Set LEUART0 RX data available as source of DMA signals:
	.select    = DMAREQ_LEUART0_RXDATAV,
	.cb        = NULL,
};

DMA_CfgDescr_TypeDef descrCfg =
{
	.dstInc  = dmaDataInc1,    // Increment destination address by one byte
	.srcInc  = dmaDataIncNone, // Do no increment source address
	.size    = dmaDataSize1,   // Data size is one byte
	.arbRate = dmaArbitrate1,  // Rearbitrate for each byte recieved
	.hprot   = 0,              // No read/write source protection
};

static inline void _activate_dma()
{
	DMA_ActivateBasic(
		dma::LEUART0_DMA_CHANNEL,// Activate channel selected
		true,                    // Use primary descriptor
		false,                   // No DMA burst
		(void*)&rxbuf,           // Destination address
		(void*)&LEUART0->RXDATA, // Source address
		BUF_MAX - 1);            // Size of buffer minus1
}
static inline void _reactivate_dma()
{
	DMA_ActivateBasic(
		dma::LEUART0_DMA_CHANNEL,// Activate channel selected
		true,                    // Use primary descriptor
		false,                   // No DMA burst
		NULL,                    // keep
		NULL,                    // keep
		BUF_MAX - 1);            // Size of buffer minus1
}

void setup()
{
	GPIO_PinModeSet(gpioPortD, 4, gpioModePushPull, 1);
	GPIO_PinModeSet(gpioPortD, 5, gpioModeInput, 0);
	// To avoid false start, configure output LEU0_TX as high on PD4
	GPIO_PinOutSet(gpioPortD, 4);

	// Enable signals TX, RX at location 0
	LEUART0->ROUTE |= LEUART_ROUTE_TXPEN | LEUART_ROUTE_RXPEN |
	                  LEUART_ROUTE_LOCATION_LOC0;

	// setup RX DMA
	DMA_CfgChannel(dma::LEUART0_DMA_CHANNEL, &chnlCfg);
	DMA_CfgDescr(dma::LEUART0_DMA_CHANNEL, true, &descrCfg);

	// Starting the transfer. Using Basic Mode
	_activate_dma();

	// trigger on newline
	LEUART0->SIGFRAME = '\n';
	// Enable LEUART Signal Frame Interrupt
	LEUART_IntEnable(LEUART0, LEUART_IEN_SIGF);
	// Enable LEUART interrupt vector
	NVIC_EnableIRQ(LEUART0_IRQn);
	// Make sure the LEUART wakes up the DMA on RX data
	LEUART0->CTRL = LEUART_CTRL_RXDMAWU;
}

} // namespace leuart0

void LEUART0_IRQHandler()
{
	// Store and reset pending interupts
	const uint32_t leuartif = LEUART_IntGet(LEUART0);
	LEUART_IntClear(LEUART0, leuartif);

	// if signal frame interrupt
	if (leuartif & LEUART_IF_SIGF) {
		// Zero-terminate rx buffer
		const uint32_t len = leuart0::BUF_MAX - 1 - ((dma::control_block->CTRL >> 4) & 0x3FF);
		leuart0::rxbuf[len - 1] = 0;

		// TODO handle received message

		leuart0::_reactivate_dma();
	}
}
