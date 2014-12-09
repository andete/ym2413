#ifndef USBCDC_HH
#define USBCDC_HH

#include <cstdint>

// USB CDC code based on silabs/kits/EFM32GG_DK3750/examples/usbdcdc

namespace usbcdc {

void setup();

// data _must_ be 4-byte aligned!
// data needs to be accessable after this call returns!
void write(const void* data, uint16_t len);

// data _must_ be 4-byte aligned!
// data needs to be accessable after this call returns!
// TODO deal with this in a smart way to hide these details
// from external code!
void print(const char* str);

} // namespace usbcdc

#endif
