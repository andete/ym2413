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

namespace usb {

  EFM32_ALIGN(4)
  static const USB_DeviceDescriptor_TypeDef deviceDesc __attribute__ ((aligned(4)))=
  {
    .bLength            = USB_DEVICE_DESCSIZE,
    .bDescriptorType    = USB_DEVICE_DESCRIPTOR,
    .bcdUSB             = 0x0200,
    .bDeviceClass       = USB_CLASS_CDC,
    .bDeviceSubClass    = 0,
    .bDeviceProtocol    = 0,
    .bMaxPacketSize0    = USB_EP0_SIZE,
    .idVendor           = 0x2544,
    .idProduct          = 0x0003,
    .bcdDevice          = 0x0000,
    .iManufacturer      = 1,
    .iProduct           = 2,
    .iSerialNumber      = 0,
    .bNumConfigurations = 1
  };

  #define CONFIG_DESCSIZE ( USB_CONFIG_DESCSIZE                 + \
                          (USB_INTERFACE_DESCSIZE * 2)          + \
                          (USB_ENDPOINT_DESCSIZE * NUM_EP_USED) + \
                          USB_CDC_HEADER_FND_DESCSIZE           + \
                          USB_CDC_CALLMNG_FND_DESCSIZE          + \
                          USB_CDC_ACM_FND_DESCSIZE              + \
                          5 )

  EFM32_ALIGN(4)
  static const uint8_t configDesc[] __attribute__ ((aligned(4)))= {
  /*** Configuration descriptor ***/
    USB_CONFIG_DESCSIZE,    /* bLength                                   */
    USB_CONFIG_DESCRIPTOR,  /* bDescriptorType                           */
    CONFIG_DESCSIZE,        /* wTotalLength (LSB)                        */
    USB_CONFIG_DESCSIZE>>8, /* wTotalLength (MSB)                        */
    2,                      /* bNumInterfaces                            */
    1,                      /* bConfigurationValue                       */
    0,                      /* iConfiguration                            */
    CONFIG_DESC_BM_RESERVED_D7 |   /* bmAttrib: Self powered             */
    CONFIG_DESC_BM_SELFPOWERED,
    CONFIG_DESC_MAXPOWER_mA( 100 ),/* bMaxPower: 100 mA                  */

    /*** Communication Class Interface descriptor (interface no. 0)    ***/
    USB_INTERFACE_DESCSIZE, /* bLength               */
    USB_INTERFACE_DESCRIPTOR,/* bDescriptorType      */
    0,                      /* bInterfaceNumber      */
    0,                      /* bAlternateSetting     */
    1,                      /* bNumEndpoints         */
    USB_CLASS_CDC,          /* bInterfaceClass       */
    USB_CLASS_CDC_ACM,      /* bInterfaceSubClass    */
    0,                      /* bInterfaceProtocol    */
    0,                      /* iInterface            */

    /*** CDC Header Functional descriptor ***/
    USB_CDC_HEADER_FND_DESCSIZE, /* bFunctionLength  */
    USB_CS_INTERFACE_DESCRIPTOR, /* bDescriptorType  */
    USB_CLASS_CDC_HFN,      /* bDescriptorSubtype    */
    0x20,                   /* bcdCDC spec.no LSB    */
    0x01,                   /* bcdCDC spec.no MSB    */

    /*** CDC Call Management Functional descriptor ***/
    USB_CDC_CALLMNG_FND_DESCSIZE, /* bFunctionLength */
    USB_CS_INTERFACE_DESCRIPTOR,  /* bDescriptorType */
    USB_CLASS_CDC_CMNGFN,   /* bDescriptorSubtype    */
    0,                      /* bmCapabilities        */
    1,                      /* bDataInterface        */

    /*** CDC Abstract Control Management Functional descriptor ***/
    USB_CDC_ACM_FND_DESCSIZE, /* bFunctionLength     */
    USB_CS_INTERFACE_DESCRIPTOR, /* bDescriptorType  */
    USB_CLASS_CDC_ACMFN,    /* bDescriptorSubtype    */
    0x02,                   /* bmCapabilities        */
    /* The capabilities that this configuration supports:                   */
    /* D7..D4: RESERVED (Reset to zero)                                     */
    /* D3: 1 - Device supports the notification Network_Connection.         */
    /* D2: 1 - Device supports the request Send_Break                       */
    /* D1: 1 - Device supports the request combination of Set_Line_Coding,  */
    /*         Set_Control_Line_State, Get_Line_Coding, and the             */
    /*         notification Serial_State.                                   */
    /* D0: 1 - Device supports the request combination of Set_Comm_Feature, */
    /*         Clear_Comm_Feature, and Get_Comm_Feature.                    */

    /*** CDC Union Functional descriptor ***/
    5,                      /* bFunctionLength       */
    USB_CS_INTERFACE_DESCRIPTOR, /* bDescriptorType  */
    USB_CLASS_CDC_UNIONFN,  /* bDescriptorSubtype    */
    0,                      /* bControlInterface,      itf. no. 0 */
    1,                      /* bSubordinateInterface0, itf. no. 1 */

    /*** CDC Notification endpoint descriptor ***/
    USB_ENDPOINT_DESCSIZE,  /* bLength               */
    USB_ENDPOINT_DESCRIPTOR,/* bDescriptorType       */
    EP_NOTIFY,              /* bEndpointAddress (IN) */
    USB_EPTYPE_INTR,        /* bmAttributes          */
    BULK_EP_SIZE,           /* wMaxPacketSize (LSB)  */
    0,                      /* wMaxPacketSize (MSB)  */
    0xFF,                   /* bInterval             */

    /*** Data Class Interface descriptor (interface no. 1)                ***/
    USB_INTERFACE_DESCSIZE, /* bLength               */
    USB_INTERFACE_DESCRIPTOR,/* bDescriptorType      */
    1,                      /* bInterfaceNumber      */
    0,                      /* bAlternateSetting     */
    2,                      /* bNumEndpoints         */
    USB_CLASS_CDC_DATA,     /* bInterfaceClass       */
    0,                      /* bInterfaceSubClass    */
    0,                      /* bInterfaceProtocol    */
    0,                      /* iInterface            */

    /*** CDC Data interface endpoint descriptors ***/
    USB_ENDPOINT_DESCSIZE,  /* bLength               */
    USB_ENDPOINT_DESCRIPTOR,/* bDescriptorType       */
    EP_DATA_IN,             /* bEndpointAddress (IN) */
    USB_EPTYPE_BULK,        /* bmAttributes          */
    BULK_EP_SIZE,           /* wMaxPacketSize (LSB)  */
    0,                      /* wMaxPacketSize (MSB)  */
    0,                      /* bInterval             */

    USB_ENDPOINT_DESCSIZE,  /* bLength               */
    USB_ENDPOINT_DESCRIPTOR,/* bDescriptorType       */
    EP_DATA_OUT,            /* bEndpointAddress (OUT)*/
    USB_EPTYPE_BULK,        /* bmAttributes          */
    BULK_EP_SIZE,           /* wMaxPacketSize (LSB)  */
    0,                      /* wMaxPacketSize (MSB)  */
    0                       /* bInterval             */
  };

  STATIC_CONST_STRING_DESC_LANGID( langID, 0x04, 0x09 );
  STATIC_CONST_STRING_DESC( iManufacturer, 'E','n','e','r','g','y',' ',         \
                                         'M','i','c','r','o',' ','A','S', '\0' );
  STATIC_CONST_STRING_DESC( iProduct     , 'E','F','M','3','2',' ','U','S','B', \
                                         ' ','C','D','C',' ','s','e','r','i', \
                                         'a','l',' ','p','o','r','t',' ','d', \
                                         'e','v','i','c','e', '\0' );

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
