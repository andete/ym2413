#include "usbcdc.hh"
#include "event.hh"
#include "efm32gg_uart.h"

// USB CDC code based on silabs/kits/EFM32GG_DK3750/examples/usbdcdc

namespace usbcdc {

// Define USB endpoint addresses
#define EP_DATA_OUT       0x01  // Endpoint for USB data reception.
#define EP_DATA_IN        0x81  // Endpoint for USB data transmission.
#define EP_NOTIFY         0x82  // The notification endpoint (not used).

#define BULK_EP_SIZE     USB_MAX_EP_SIZE  // This is the max. ep size.
#define USB_RX_BUF_SIZ   BULK_EP_SIZE // Packet size when receiving on USB
#define USB_TX_BUF_SIZ   127    // Packet size when transmitting on USB.

// Calculate a timeout in ms corresponding to 5 char times on current
// baudrate. Minimum timeout is set to 10 ms.
#define RX_TIMEOUT    EFM32_MAX(10U, 50000 / (cdcLineCoding.dwDTERate))

// The serial port LINE CODING data structure, used to carry information
// about serial port baudrate, parity etc. between host and device.
EFM32_PACK_START(1)
typedef struct
{
	uint32_t dwDTERate;               // Baudrate
	uint8_t  bCharFormat;             // Stop bits, 0=1 1=1.5 2=2
	uint8_t  bParityType;             // 0=None 1=Odd 2=Even 3=Mark 4=Space
	uint8_t  bDataBits;               // 5, 6, 7, 8 or 16
	uint8_t  dummy;                   // To ensure size is a multiple of 4 bytes.
} __attribute__ ((packed)) cdcLineCoding_TypeDef;
EFM32_PACK_END()


// prototypes
static int data_received(USB_Status_TypeDef status,
                         uint32_t xferred,
                         uint32_t remaining);

static int line_coding_received(USB_Status_TypeDef status,
                                uint32_t xferred,
                                uint32_t remaining);

static int setup_cmd(const USB_Setup_TypeDef* setup);

static void state_change(USBD_State_TypeDef oldState,
                         USBD_State_TypeDef newState);

EFM32_ALIGN(4)
static const USB_DeviceDescriptor_TypeDef deviceDesc __attribute__ ((aligned(4))) = {
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

#define CONFIG_DESCSIZE (USB_CONFIG_DESCSIZE  + \
	(USB_INTERFACE_DESCSIZE * 2)          + \
	(USB_ENDPOINT_DESCSIZE * NUM_EP_USED) + \
	USB_CDC_HEADER_FND_DESCSIZE           + \
	USB_CDC_CALLMNG_FND_DESCSIZE          + \
	USB_CDC_ACM_FND_DESCSIZE              + \
	5)

EFM32_ALIGN(4)
static const uint8_t configDesc[] __attribute__ ((aligned(4))) = {
	// Configuration descriptor
	USB_CONFIG_DESCSIZE,    // bLength
	USB_CONFIG_DESCRIPTOR,  // bDescriptorType
	CONFIG_DESCSIZE,        // wTotalLength (LSB)
	USB_CONFIG_DESCSIZE>>8, // wTotalLength (MSB)
	2,                      // bNumInterfaces
	1,                      // bConfigurationValue
	0,                      // iConfiguration
	CONFIG_DESC_BM_RESERVED_D7 |   // bmAttrib: Self powered
		CONFIG_DESC_BM_SELFPOWERED,
	CONFIG_DESC_MAXPOWER_mA( 100 ),// bMaxPower: 100 mA

	// Communication Class Interface descriptor (interface no. 0)
	USB_INTERFACE_DESCSIZE, // bLength
	USB_INTERFACE_DESCRIPTOR,// bDescriptorType
	0,                      // bInterfaceNumber
	0,                      // bAlternateSetting
	1,                      // bNumEndpoints
	USB_CLASS_CDC,          // bInterfaceClass
	USB_CLASS_CDC_ACM,      // bInterfaceSubClass
	0,                      // bInterfaceProtocol
	0,                      // iInterface

	// CDC Header Functional descriptor
	USB_CDC_HEADER_FND_DESCSIZE, // bFunctionLength
	USB_CS_INTERFACE_DESCRIPTOR, // bDescriptorType
	USB_CLASS_CDC_HFN,      // bDescriptorSubtype
	0x20,                   // bcdCDC spec.no LSB
	0x01,                   // bcdCDC spec.no MSB

	// CDC Call Management Functional descriptor
	USB_CDC_CALLMNG_FND_DESCSIZE, // bFunctionLength
	USB_CS_INTERFACE_DESCRIPTOR,  // bDescriptorType
	USB_CLASS_CDC_CMNGFN,   // bDescriptorSubtype
	0,                      // bmCapabilities
	1,                      // bDataInterface

	// CDC Abstract Control Management Functional descriptor
	USB_CDC_ACM_FND_DESCSIZE, // bFunctionLength
	USB_CS_INTERFACE_DESCRIPTOR, // bDescriptorType
	USB_CLASS_CDC_ACMFN,    // bDescriptorSubtype
	0x02,                   // bmCapabilities
	// The capabilities that this configuration supports:
	// D7..D4: RESERVED (Reset to zero)
	// D3: 1 - Device supports the notification Network_Connection.
	// D2: 1 - Device supports the request Send_Break
	// D1: 1 - Device supports the request combination of Set_Line_Coding,
	//         Set_Control_Line_State, Get_Line_Coding, and the
	//         notification Serial_State.
	// D0: 1 - Device supports the request combination of Set_Comm_Feature,
	//         Clear_Comm_Feature, and Get_Comm_Feature.

	// CDC Union Functional descriptor
	5,                      // bFunctionLength
	USB_CS_INTERFACE_DESCRIPTOR, // bDescriptorType
	USB_CLASS_CDC_UNIONFN,  // bDescriptorSubtype
	0,                      // bControlInterface,      itf. no. 0
	1,                      // bSubordinateInterface0, itf. no. 1

	// CDC Notification endpoint descriptor
	USB_ENDPOINT_DESCSIZE,  // bLength
	USB_ENDPOINT_DESCRIPTOR,// bDescriptorType
	EP_NOTIFY,              // bEndpointAddress (IN)
	USB_EPTYPE_INTR,        // bmAttributes
	BULK_EP_SIZE,           // wMaxPacketSize (LSB)
	0,                      // wMaxPacketSize (MSB)
	0xFF,                   // bInterval

	// Data Class Interface descriptor (interface no. 1)
	USB_INTERFACE_DESCSIZE, // bLength
	USB_INTERFACE_DESCRIPTOR,// bDescriptorType
	1,                      // bInterfaceNumber
	0,                      // bAlternateSetting
	2,                      // bNumEndpoints
	USB_CLASS_CDC_DATA,     // bInterfaceClass
	0,                      // bInterfaceSubClass
	0,                      // bInterfaceProtocol
	0,                      // iInterface

	// CDC Data interface endpoint descriptors
	USB_ENDPOINT_DESCSIZE,  // bLength
	USB_ENDPOINT_DESCRIPTOR,// bDescriptorType
	EP_DATA_IN,             // bEndpointAddress (IN)
	USB_EPTYPE_BULK,        // bmAttributes
	BULK_EP_SIZE,           // wMaxPacketSize (LSB)
	0,                      // wMaxPacketSize (MSB)
	0,                      // bInterval

	USB_ENDPOINT_DESCSIZE,  // bLength
	USB_ENDPOINT_DESCRIPTOR,// bDescriptorType
	EP_DATA_OUT,            // bEndpointAddress (OUT)
	USB_EPTYPE_BULK,        // bmAttributes
	BULK_EP_SIZE,           // wMaxPacketSize (LSB)
	0,                      // wMaxPacketSize (MSB)
	0                       // bInterval
};

STATIC_CONST_STRING_DESC_LANGID(langID, 0x04, 0x09);
STATIC_CONST_STRING_DESC(iManufacturer,
	'E','n','e','r','g','y',' ', 'M','i','c','r','o',' ','A','S', '\0');
STATIC_CONST_STRING_DESC(iProduct,
	'E','F','M','3','2',' ','U','S','B', ' ','C','D','C',' ','s','e','r','i', \
	'a','l',' ','p','o','r','t',' ','d', 'e','v','i','c','e', '\0');

static const void* const strings[] = {
	&langID,
	&iManufacturer,
	&iProduct,
};

// Endpoint buffer sizes
// 1 = single buffer, 2 = double buffering, 3 = triple buffering ...
// Use double buffering on the BULK endpoints.
static const uint8_t bufferingMultiplier[NUM_EP_USED + 1] = { 1, 1, 2, 2 };

static const USBD_Callbacks_TypeDef callbacks = {
	.usbReset        = nullptr,
	.usbStateChange  = usbcdc::state_change,
	.setupCmd        = usbcdc::setup_cmd,
	.isSelfPowered   = nullptr,
	.sofInt          = nullptr
};

static USBD_Init_TypeDef initstruct = {
	.deviceDescriptor    = &deviceDesc,
	.configDescriptor    = configDesc,
	.stringDescriptors   = strings,
	.numberOfStrings     = sizeof(strings) / sizeof(void*),
	.bufferingMultiplier = bufferingMultiplier,
	.callbacks           = &callbacks,
	.reserved            = 0,
};

// The LineCoding variable must be 4-byte aligned as it is used as USB
// transmit and receive buffer
EFM32_ALIGN(4)
EFM32_PACK_START(1)
static cdcLineCoding_TypeDef __attribute__ ((aligned(4))) cdcLineCoding = {
	115200, 0, 0, 8, 0
};
EFM32_PACK_END()


void setup()
{
	// Enable signals VBUSEN, DMPU
	// TODO: is this really needed as we don't use the pins anyway...
	USB->ROUTE |= USB_ROUTE_VBUSENPEN | USB_ROUTE_DMPUPEN;

	USBD_Init(&initstruct);

	// When using a debugger it is practical to uncomment the following three
	// lines to force host to re-enumerate the device.
	USBD_Disconnect();
	USBTIMER_DelayMs(1000);
	USBD_Connect();
}


STATIC_UBUF(usbRxBuffer, USB_RX_BUF_SIZ); // USB receive buffer
static const unsigned USB_BUF_SIZE = 256;
uint8_t usbBuffer[USB_BUF_SIZE];
unsigned usbRdIdx = 0;
unsigned usbWrIdx = 0;

///////////////////////////////////////////////////////////////////////////////
// @brief Called whenever a new data packet is received on USB
// @param[in] status    Transfer status code.
// @param[in] xferred   Number of bytes transferred.
// @param[in] remaining Number of bytes not transferred.
// @return USB_STATUS_OK.
// Runs in ISR context! (??)
static int data_received(USB_Status_TypeDef status,
                         uint32_t xferred,
                         uint32_t /*remaining*/)
{
	if ((status == USB_STATUS_OK) && (xferred > 0)) {
		// simple byte-per-byte copy,
		// TODO improve later
		for (uint32_t i = 0; i < xferred; ++i) {
			usbBuffer[usbWrIdx] = usbRxBuffer[i];
			++usbWrIdx; if (usbWrIdx == USB_BUF_SIZE) usbWrIdx = 0;
			if (usbWrIdx == usbRdIdx) {
				event::postISR(event::USB_RX_OVERFLOW);
			}
		}
		event::postISR(event::USB_RX);

		USBD_Read(EP_DATA_OUT, const_cast<uint8_t*>(usbRxBuffer),
		          USB_RX_BUF_SIZ, data_received);
	}
	return USB_STATUS_OK;
}

bool pollRead(uint8_t& output)
{
	__disable_irq();
	if (usbRdIdx == usbWrIdx) {
		__enable_irq();
		return false;
	}
	output = usbBuffer[usbRdIdx];
	++usbRdIdx; if (usbRdIdx == USB_BUF_SIZE) usbRdIdx = 0;
	__enable_irq();
	return true;
}

///////////////////////////////////////////////////////////////////////////////
// @brief
//   Callback function called each time the USB device state is changed.
//   Starts CDC operation when device has been configured by USB host.
// @param[in] oldState The device state the device has just left.
// @param[in] newState The new device state.
static void state_change(USBD_State_TypeDef oldState,
                         USBD_State_TypeDef newState)
{
	if (newState == USBD_STATE_CONFIGURED) {
		// We have been configured, start CDC functionality!
		if (oldState == USBD_STATE_SUSPENDED) {
			// Resume?
		}

		// Start receiving data from USB host.
		usbRdIdx = 0;
		usbWrIdx = 0;
		USBD_Read(EP_DATA_OUT, const_cast<uint8_t*>(usbRxBuffer),
		          USB_RX_BUF_SIZ, data_received);

	} else if ((oldState == USBD_STATE_CONFIGURED) &&
			(newState != USBD_STATE_SUSPENDED)) {
		// We have been de-configured, stop CDC functionality
		USBTIMER_Stop(0);
		DMA->CHENC = 3;     // Stop DMA channels 0 and 1.

	} else if (newState == USBD_STATE_SUSPENDED) {
		// We have been suspended, stop CDC functionality
		// Reduce current consumption to below 2.5 mA.
		USBTIMER_Stop(0);
		DMA->CHENC = 3;     // Stop DMA channels 0 and 1.
	}
}

///////////////////////////////////////////////////////////////////////////////
// @brief
//   Handle USB setup commands. Implements CDC class specific commands.
//
// @param[in] setup Pointer to the setup packet received.
//
// @return USB_STATUS_OK if command accepted.
//         USB_STATUS_REQ_UNHANDLED when command is unknown, the USB device
//         stack will handle the request.
static int setup_cmd(const USB_Setup_TypeDef* setup)
{
	int retVal = USB_STATUS_REQ_UNHANDLED;

	if ((setup->Type == USB_SETUP_TYPE_CLASS) &&
	    (setup->Recipient == USB_SETUP_RECIPIENT_INTERFACE)) {
		switch (setup->bRequest) {
		case USB_CDC_GETLINECODING:
			if ((setup->wValue == 0) &&
			    (setup->wIndex == 0) &&  // Interface no.
			    (setup->wLength == 7) && // Length of cdcLineCoding
			    (setup->Direction == USB_SETUP_DIR_IN)) {
				// Send current settings to USB host.
				USBD_Write(0, &cdcLineCoding, 7, nullptr);
				retVal = USB_STATUS_OK;
			}
			break;

		case USB_CDC_SETLINECODING:
			if ((setup->wValue == 0) &&
			    (setup->wIndex == 0) &&  // Interface no.
			    (setup->wLength == 7) && // Length of cdcLineCoding
			    (setup->Direction != USB_SETUP_DIR_IN)) {
				// Get new settings from USB host.
				USBD_Read(0, &cdcLineCoding, 7, line_coding_received);
				retVal = USB_STATUS_OK;
			}
			break;

		case USB_CDC_SETCTRLLINESTATE:
			if ((setup->wIndex == 0) &&  // Interface no.
			    (setup->wLength == 0)) { // No data
				// Do nothing (Non compliant behaviour!!)
				retVal = USB_STATUS_OK;
			}
			break;
		}
	}
	return retVal;
}

///////////////////////////////////////////////////////////////////////////////
// @brief
//   Callback function called when the data stage of a CDC_SET_LINECODING
//   setup command has completed.
//
// @param[in] status    Transfer status code.
// @param[in] xferred   Number of bytes transferred.
// @param[in] remaining Number of bytes not transferred.
//
// @return USB_STATUS_OK if data accepted.
//         USB_STATUS_REQ_ERR if data calls for modes we can not support.
static int line_coding_received(USB_Status_TypeDef status,
                                uint32_t xferred,
                                uint32_t /*remaining*/)
{
	uint32_t frame = 0;

	// We have received new serial port communication settings from USB host
	if ((status == USB_STATUS_OK) && (xferred == 7)) {
		// Check bDataBits, valid values are: 5, 6, 7, 8 or 16 bits
		if (cdcLineCoding.bDataBits == 5) {
			frame |= UART_FRAME_DATABITS_FIVE;
		} else if (cdcLineCoding.bDataBits == 6) {
			frame |= UART_FRAME_DATABITS_SIX;
		} else if (cdcLineCoding.bDataBits == 7) {
			frame |= UART_FRAME_DATABITS_SEVEN;
		} else if (cdcLineCoding.bDataBits == 8) {
			frame |= UART_FRAME_DATABITS_EIGHT;
		} else if (cdcLineCoding.bDataBits == 16) {
			frame |= UART_FRAME_DATABITS_SIXTEEN;
		} else {
			return USB_STATUS_REQ_ERR;
		}

		// Check bParityType, valid values are: 0=None 1=Odd 2=Even 3=Mark 4=Space
		if (cdcLineCoding.bParityType == 0) {
			frame |= UART_FRAME_PARITY_NONE;
		} else if (cdcLineCoding.bParityType == 1) {
			frame |= UART_FRAME_PARITY_ODD;
		} else if (cdcLineCoding.bParityType == 2) {
			frame |= UART_FRAME_PARITY_EVEN;
		} else if (cdcLineCoding.bParityType == 3) {
			return USB_STATUS_REQ_ERR;
		} else if (cdcLineCoding.bParityType == 4) {
			return USB_STATUS_REQ_ERR;
		} else {
			return USB_STATUS_REQ_ERR;
		}

		// Check bCharFormat, valid values are: 0=1 1=1.5 2=2 stop bits
		if (cdcLineCoding.bCharFormat == 0) {
			frame |= UART_FRAME_STOPBITS_ONE;
		} else if (cdcLineCoding.bCharFormat == 1) {
			frame |= UART_FRAME_STOPBITS_ONEANDAHALF;
		} else if (cdcLineCoding.bCharFormat == 2) {
			frame |= UART_FRAME_STOPBITS_TWO;
		} else {
			return USB_STATUS_REQ_ERR;
		}

		// we don't have an USART1
#if 0
		// Program new UART baudrate etc.
		UART1->FRAME = frame;
		USART_BaudrateAsyncSet(UART1, 0, cdcLineCoding.dwDTERate, usartOVS16);
#endif
		return USB_STATUS_OK;
	}
	return USB_STATUS_REQ_ERR;
}

void write(const void* data, uint16_t len, USB_XferCompleteCb_TypeDef callback)
{
	USBD_Write(EP_DATA_IN, const_cast<void*>(data), len, callback);
}

} // namespace usbcdc
