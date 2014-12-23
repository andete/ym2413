#include "event.hh"
#include "adc0.hh"
#include "led.hh"
#include "usbcdc.hh"
#include "em_int.h"

namespace event {

static const unsigned BUF_SIZE = 0x10;
static Event buf[BUF_SIZE];
static unsigned rdIdx = 0;
static unsigned wrIdx = 0;


void post(Event event)
{
	__disable_irq();
	postISR(event);
	__enable_irq();
}

void postISR(Event event)
{
	buf[wrIdx] = event;
	wrIdx = (wrIdx + 1) % BUF_SIZE;
	if (wrIdx == rdIdx) {
		postISR(EVENT_OVERFLOW);
	}
}

// Runs in ISR context!
static int usbCallback(USB_Status_TypeDef status, uint32_t /*xferred*/, uint32_t /*remaining*/)
{
	if (status != USB_STATUS_OK) {
		postISR(USB_ERROR);
	}
	// Indicate buffer-space is available again (can be filled with new ADC data).
	adc0::fullBufferPtr = nullptr;
	return USB_STATUS_OK;
}

void loop()
{
	// Enter Cortex-M3 sleep mode (not deep-sleep) on __WFI()
	SCB->SCR &= ~SCB_SCR_SLEEPDEEP_Msk;

	while (true) {
		__disable_irq();
		while (rdIdx != wrIdx) {
			Event event = buf[rdIdx];
			rdIdx = (rdIdx + 1) % BUF_SIZE;
			__enable_irq();

			switch (event) {
			case DMA_DONE:
				// Send buffer to host (over USB).
				usbcdc::write(adc0::fullBufferPtr, adc0::NUM_SAMPLES * sizeof(int16_t),
				              usbCallback);
				led::toggle(8);
				break;

			case ADC_OVERFLOW:
				led::showError(5);
				break;
			case DMA_OVERFLOW:
				led::showError(6);
				break;
			case EVENT_OVERFLOW:
				led::showError(1);
				break;
			case USB_ERROR:
				led::showError(4);
				break;
			}

			__disable_irq();
		}

		// Indicate main loop is still alive.
		led::toggle(9);

		// Enter EM1 sleep mode (peripherals active, CPU sleeping (wakeup by IRQ)).
		__WFI();        // it's ok to call this with interrupts disabled
		__enable_irq(); // at this point the IRQ will be processed
	}
}

} // namespace event
