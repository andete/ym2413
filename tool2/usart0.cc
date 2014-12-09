#include "usart0.hh"
#include "em_usart.h"

namespace usart0 {

void setup()
{
	// Pin PE11 is configured to Input enabled
	GPIO->P[4].MODEH = (GPIO->P[4].MODEH & ~_GPIO_P_MODEH_MODE11_MASK) | GPIO_P_MODEH_MODE11_INPUT;

	USART_InitAsync_TypeDef init = USART_INITASYNC_DEFAULT;

	init.baudrate     = 31250;
	init.oversampling = usartOVS16;
	init.databits     = usartDatabits8;
	init.parity       = usartNoParity;
	init.stopbits     = usartStopbits1;
	init.mvdis        = 0;
	init.prsRxEnable  = 0;

	USART_InitAsync(USART0, &init);

	/* Enable signal RX */
	USART0->ROUTE |= USART_ROUTE_RXPEN;
}

} // namespace usart0
