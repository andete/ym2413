#ifndef USBCDC_HH
#define USBCDC_HH

#include "em_usb.h"
#include <cstdint>

// USB CDC code based on silabs/kits/EFM32GG_DK3750/examples/usbdcdc

namespace usbcdc {

void setup();

// Data must be 4-byte aligned and must remain accessable after this call returns!
// After the data has been send an (optional) callback function is called:
//	int callback(USB_Status_TypeDef status, uint32_t xferred, uint32_t remaining)
//	-> should return USB_STATUS_OK
void write(const void* data, uint16_t len, USB_XferCompleteCb_TypeDef callback = nullptr);

} // namespace usbcdc

#endif
