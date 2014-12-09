#include "swd.hh"
#include "em_gpio.h"

namespace swd {

void setup()
{
	// enable SWO tracing output
	GPIO->ROUTE |= GPIO_ROUTE_SWOPEN;
}

} // namespace swd
