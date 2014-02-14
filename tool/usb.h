

namespace usb {

  static void setup() {
    // Enable signals VBUSEN, DMPU
    // TODO: is this really needed as we don't use the pins anyway...
    USB->ROUTE |= USB_ROUTE_VBUSENPEN | USB_ROUTE_DMPUPEN;
  }

}
