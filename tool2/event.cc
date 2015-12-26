#include "event.hh"
#include "adc0.hh"
#include "led.hh"
#include "timer0.hh"
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
	led::toggle(5); // TODO after a while this callback doesn't trigger anymore !!!
	                //  UPDATE: happens when host is not listening to the send data
	return USB_STATUS_OK;
}

enum class State { S0, E0, R0, R1, R2, R3 };
static State state = State::S0;
static uint8_t echoData[2]  = { 0x00, 0x80 };
static uint8_t startData[2] = { 0xD3, 0x80 };
static uint8_t stopData[2]  = { 0xF3, 0x80 };
static uint8_t reg1Data[2]  = { 0xD2, 0x80 };
static uint8_t reg2Data[2]  = { 0xF2, 0x80 };

static const unsigned MAX_SCRIPT_SIZE = 4 * 1024;
static union {
	uint8_t scriptData[MAX_SCRIPT_SIZE];
	uint16_t data16;
};
static unsigned scriptIdx = 0;

static void playCallback()
{
	usbcdc::write(reg2Data, sizeof(reg2Data));
}

static void usbRx(uint8_t data)
{
	switch (state) {
	case State::S0:
		if (data == 'A') {
			led::toggle(1);
		} else if (data == 'B') {
			led::toggle(2);
		} else if (data == 'C') {
			led::toggle(3);
		} else if (data == 'S') {
			usbcdc::write(startData, sizeof(startData));
			adc0::start1(true);
		} else if (data == 's') {
			usbcdc::write(stopData, sizeof(stopData));
			adc0::stop();
		} else if (data == 'E') {
			state = State::E0;
		} else if (data == 'R') {
			scriptIdx = 0;
			state = State::R0;
		}
		break;
	case State::E0:
		echoData[0] = data;
		usbcdc::write(echoData, sizeof(echoData));
		state = State::S0;
		break;
	case State::R0:
		scriptData[scriptIdx++] = data;
		state = State::R1;
		break;
	case State::R1:
		scriptData[scriptIdx++] = data;
		state = State::R2;
		break;
	case State::R2:
		scriptData[scriptIdx++] = data;
		state = State::R3;
		break;
	case State::R3:
		scriptData[scriptIdx++] = data;
		if ((scriptData[scriptIdx - 1] == 0) &&
		    (scriptData[scriptIdx - 2] == 0)) {
			usbcdc::write(reg1Data, sizeof(reg1Data));
			timer0::playScript(&data16, playCallback);
			state = State::S0;
		} else {
			state = State::R0;
		}
		break;
	}
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
				adc0::fullBufferPtr = nullptr; // TODO should (only) be done in usbCallback
				led::toggle(8);
				break;

			case USB_RX:
				uint8_t data;
				while (usbcdc::pollRead(data)) {
					usbRx(data);
				}
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
			case USB_RX_OVERFLOW:
				led::showError(7);
				break;
			case USB_TX_OVERFLOW:
				led::showError(2);
				break;
			default:
				// unknown event type
				led::showError(3);
				break;
			}

			// Indicate main loop is still alive.
			led::toggle(9);

			__disable_irq();
		}

		// Enter EM1 sleep mode (peripherals active, CPU sleeping (wakeup by IRQ)).
		__WFI();        // it's ok to call this with interrupts disabled
		__enable_irq(); // at this point the IRQ will be processed
	}
}

} // namespace event
