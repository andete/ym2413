/* -*- Mode: C; tab-width: 2; indent-tabs-mode: nil -*- */

// USB CDC code based on silabs/kits/EFM32GG_DK3750/examples/usbdcdc

// Define USB endpoint addresses
#define EP_DATA_OUT       0x01  /* Endpoint for USB data reception.       */
#define EP_DATA_IN        0x81  /* Endpoint for USB data transmission.    */
#define EP_NOTIFY         0x82  /* The notification endpoint (not used).  */

#define BULK_EP_SIZE     USB_MAX_EP_SIZE  /* This is the max. ep size.    */
#define USB_RX_BUF_SIZ   BULK_EP_SIZE /* Packet size when receiving on USB*/
#define USB_TX_BUF_SIZ   127    /* Packet size when transmitting on USB.  */

/* Calculate a timeout in ms corresponding to 5 char times on current     */
/* baudrate. Minimum timeout is set to 10 ms.                             */
#define RX_TIMEOUT    EFM32_MAX(10U, 50000 / (cdcLineCoding.dwDTERate))

/* The serial port LINE CODING data structure, used to carry information  */
/* about serial port baudrate, parity etc. between host and device.       */
EFM32_PACK_START(1)
typedef struct
{
  uint32_t dwDTERate;               /** Baudrate                            */
  uint8_t  bCharFormat;             /** Stop bits, 0=1 1=1.5 2=2            */
  uint8_t  bParityType;             /** 0=None 1=Odd 2=Even 3=Mark 4=Space  */
  uint8_t  bDataBits;               /** 5, 6, 7, 8 or 16                    */
  uint8_t  dummy;                   /** To ensure size is a multiple of 4 bytes.*/
} __attribute__ ((packed)) cdcLineCoding_TypeDef;
EFM32_PACK_END()


// prototypes
namespace usb {

  static int  UsbDataReceived(USB_Status_TypeDef status, uint32_t xferred,
                            uint32_t remaining);
  static void DmaSetup(void);
  static int  LineCodingReceived(USB_Status_TypeDef status,
                               uint32_t xferred,
                               uint32_t remaining);
  static void SerialPortInit(void);
  static int  SetupCmd(const USB_Setup_TypeDef *setup);
  static void StateChange(USBD_State_TypeDef oldState,
                        USBD_State_TypeDef newState);
  static void UartRxTimeout(void);

}

#include "usb_descriptors.h"

namespace usb {

  static const void * const strings[] = {
    &langID,
    &iManufacturer,
    &iProduct,
  };

/* Endpoint buffer sizes */
/* 1 = single buffer, 2 = double buffering, 3 = triple buffering ...  */
/* Use double buffering on the BULK endpoints.                        */
  static const uint8_t bufferingMultiplier[ NUM_EP_USED + 1 ] = { 1, 1, 2, 2 };

  static const USBD_Callbacks_TypeDef callbacks = {
    .usbReset        = NULL,
    .usbStateChange  = usb::StateChange,
    .setupCmd        = usb::SetupCmd,
    .isSelfPowered   = NULL,
    .sofInt          = NULL
  };

  static USBD_Init_TypeDef initstruct = {
    .deviceDescriptor    = &deviceDesc,
    .configDescriptor    = configDesc,
    .stringDescriptors   = strings,
    .numberOfStrings     = sizeof(strings)/sizeof(void*),
    .bufferingMultiplier = bufferingMultiplier,
    .callbacks           = &callbacks,
    .reserved            = 0,
  };

  static void setup() {
      
    // Enable signals VBUSEN, DMPU
    // TODO: is this really needed as we don't use the pins anyway...
    USB->ROUTE |= USB_ROUTE_VBUSENPEN | USB_ROUTE_DMPUPEN;
  }

}
