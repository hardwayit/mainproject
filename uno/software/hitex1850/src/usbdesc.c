/***************************************************************************
* File		usbdesc.c
* Brief		USB Descriptors
* Date		27.03.2013
* Author	"Other Technologies", sole proprietorship Pashkova E.A.
*
* Copyright(C) 2013, "Other Technologies", sole proprietorship Pashkova E.A.
* All rights reserved.
***************************************************************************/


#include "lpc18xx.h"
#include "lpc_types.h"

#include "usb.h"
#include "usbcfg.h"
#include "usbdesc.h"

#include "msc.h"
#include "mscuser.h"

#define SMART_CARD_CLOCK_IN_KHZ 4000 //this macro used by USB descriptors, SMART_CARD_CLOCK_IN_KHZ =REF_CLOCK_TO_SMART_CARD/1000
#define SMART_CARD_DEFAULT_DATA_RATE 10752 //this macro used by USB descriptors, SMART_CARD_DEFAULT_DATA_RATE = REF_CLOCK_TO_SMART_CARD/372

/* USB Standard Device Descriptor */
uint8_t USB_DeviceDescriptor[] = {
  USB_DEVICE_DESC_SIZE,              /* bLength */
  USB_DEVICE_DESCRIPTOR_TYPE,        /* bDescriptorType */
  WBVAL(0x0200), /* 2.00 */          /* bcdUSB */
  0x00,                              /* bDeviceClass */
  0x00,                              /* bDeviceSubClass */
  0x00,                              /* bDeviceProtocol */
  USB_MAX_PACKET0,                   /* bMaxPacketSize0 */
  WBVAL(0x3241),                     /* idVendor */
  WBVAL(0x9372),                     /* idProduct */
  WBVAL(0x0100), /* 1.00 */          /* bcdDevice */
  0x01,                              /* iManufacturer */
  0x02,                              /* iProduct */
  0x03,                              /* iSerialNumber */
  0x01                               /* bNumConfigurations: one possible configuration*/
};

void usb_set_vidpid(uint16_t vid, uint16_t pid)
{
    *((uint16_t*)&USB_DeviceDescriptor[8]) = vid;
    *((uint16_t*)&USB_DeviceDescriptor[10]) = pid;
}

/* USB FSConfiguration Descriptor */
/*   All Descriptors (Configuration, Interface, Endpoint, Class, Vendor */
const uint8_t USB_FSConfigDescriptor[] = {
/* Configuration 1 */
  USB_CONFIGUARTION_DESC_SIZE,       /* bLength */
  USB_CONFIGURATION_DESCRIPTOR_TYPE, /* bDescriptorType */
  WBVAL(                             /* wTotalLength */
    1*USB_CONFIGUARTION_DESC_SIZE +
    1*USB_INTERFACE_DESC_SIZE     +
    2*USB_ENDPOINT_DESC_SIZE      +
    1*USB_INTERFACE_DESC_SIZE     +
    1*0x36                        +
    2*USB_ENDPOINT_DESC_SIZE      +
    1*USB_INTERFACE_DESC_SIZE     +
    2*USB_ENDPOINT_DESC_SIZE
  ),
  0x03,                              /* bNumInterfaces */
  0x01,                              /* bConfigurationValue */
  0x00,                              /* iConfiguration */
  USB_CONFIG_BUS_POWERED,           /* bmAttributes */
  USB_CONFIG_POWER_MA(100),          /* bMaxPower */
/* Interface 0, Alternate Setting 0, MSC Class */
  USB_INTERFACE_DESC_SIZE,           /* bLength */
  USB_INTERFACE_DESCRIPTOR_TYPE,     /* bDescriptorType */
  0x00,                              /* bInterfaceNumber */
  0x00,                              /* bAlternateSetting */
  0x02,                              /* bNumEndpoints */
  USB_DEVICE_CLASS_STORAGE,          /* bInterfaceClass */
  MSC_SUBCLASS_RBC,                 /* bInterfaceSubClass */
  MSC_PROTOCOL_BULK_ONLY,            /* bInterfaceProtocol */
  0x04,                              /* iInterface */
/* Bulk In Endpoint */
  USB_ENDPOINT_DESC_SIZE,            /* bLength */
  USB_ENDPOINT_DESCRIPTOR_TYPE,      /* bDescriptorType */
  MSC_EP_IN,                         /* bEndpointAddress */
  USB_ENDPOINT_TYPE_BULK,            /* bmAttributes */
  WBVAL(64),          /* wMaxPacketSize */
  0,                                 /* bInterval */
/* Bulk Out Endpoint */
  USB_ENDPOINT_DESC_SIZE,            /* bLength */
  USB_ENDPOINT_DESCRIPTOR_TYPE,      /* bDescriptorType */
  MSC_EP_OUT,                        /* bEndpointAddress */
  USB_ENDPOINT_TYPE_BULK,            /* bmAttributes */
  WBVAL(64),                     /* wMaxPacketSize */
  0,                                 /* bInterval */
/* Interface 1, SBI (CCID) */
  USB_INTERFACE_DESC_SIZE,           /* bLength */
  USB_INTERFACE_DESCRIPTOR_TYPE,     /* bDescriptorType */
  0x01,                              /* bInterfaceNumber */
  0x00,                              /* bAlternateSetting */
  0x02,                              /* bNumEndpoints */
  0x0B,                              /* bInterfaceClass */
  0x00,                              /* bInterfaceSubClass */
  0x00,                              /* bInterfaceProtocol */
  0x05,                              /* iInterface */

    /* CCID Class-Specific Descriptor */
    0x36,                   // Size of this descriptor in bytes (bLength)
    0x21,                   // Functional descriptor type (bDescriptorType)
    0x10,0x01,              // CCID Spec Release Number in BCD format -1.1(bcdCCID)
    0x00,                   // Slot number(bMaxSlotIndex)
    0x02,					// 3volt card support (bits or 1=5v, 2=3v, 4=1.8v) (bVoltageSupport)
    0x03,0,0,0,				// Support for T0/T1 Protocol (dwProtocols)
	//0x00,0,0,0,				// Support for T0/T1 Protocol (dwProtocols)
    0xA0, 0x0F, 0x00, 0x00,          // Default clock  (dwDefaultClock)
    0x80, 0x3E, 0x00, 0x00,      	// Maximum clock  (dwMaximumClock)
    0,						// use only default clock freq (bNumClockSupported)
    0x00, 0x2A, 0x00, 0x00, 		// Default Baudrate	10752 (dwDataRate)
    0x90, 0xD0, 0x03, 0x00,		// Max Baudrate 250 kbps (dwMaxDataRate)
    0,						// support all baudrates (bNumDataRatesSupported)
    //254,0,0,0,          	    // For T1, n/a (dwMaxIFSD)
	0,0,0,0,          	    // For T1, n/a (dwMaxIFSD)
    0,0,0,0,				// No Synchronous protocol supported (dwSynchProtocols)
    0,0,0,0,				// mechanical.  no special charachteristics. (dwMechanical)
    0xB6,0,2,0,				// features enabled: automatic parameter configuration, Automatic activation of ICC, Automatic ICC clock frequency change, Automatic baud rate change, Automatic PPS, short apdu exchange (dwFeatures)
//0xB6,0,0,0,				// features enabled: automatic parameter configuration, Automatic activation of ICC, Automatic ICC clock frequency change, Automatic baud rate change, Automatic PPS, short apdu exchange (dwFeatures)
    0x0F,1,0,0,				// max len 200hex (arbitrary) (dwMaxCCIDMessageLength)
    0xFF,					// CCID echoes class of APDU (bClassGetResponse)
    0xFF,					// n/a. extended T0 is not supported (bClassEnvelope)
    0x00,0,					// (wLcdLayout)
    3,						// pin verification/modify support (bPINSupport)
    1,						// simultaneous busy slots (bMaxCCIDBusySlots)

/* Bulk In Endpoint */
  USB_ENDPOINT_DESC_SIZE,            /* bLength */
  USB_ENDPOINT_DESCRIPTOR_TYPE,      /* bDescriptorType */
  0x82,                              /* bEndpointAddress */
  USB_ENDPOINT_TYPE_BULK,            /* bmAttributes */
  WBVAL(64),                         /* wMaxPacketSize */
  0,                                 /* bInterval */
/* Bulk Out Endpoint */
  USB_ENDPOINT_DESC_SIZE,            /* bLength */
  USB_ENDPOINT_DESCRIPTOR_TYPE,      /* bDescriptorType */
  0x02,                              /* bEndpointAddress */
  USB_ENDPOINT_TYPE_BULK,            /* bmAttributes */
  WBVAL(64),                         /* wMaxPacketSize */
  0,                                 /* bInterval */
/* Interface 2, SVI */
  USB_INTERFACE_DESC_SIZE,           /* bLength */
  USB_INTERFACE_DESCRIPTOR_TYPE,     /* bDescriptorType */
  0x02,                              /* bInterfaceNumber */
  0x00,                              /* bAlternateSetting */
  0x02,                              /* bNumEndpoints */
  0xFF,                              /* bInterfaceClass */
  0xFF,                              /* bInterfaceSubClass */
  0x00,                              /* bInterfaceProtocol */
  0x06,                              /* iInterface */
/* Bulk In Endpoint */
  USB_ENDPOINT_DESC_SIZE,            /* bLength */
  USB_ENDPOINT_DESCRIPTOR_TYPE,      /* bDescriptorType */
  0x83,                              /* bEndpointAddress */
  USB_ENDPOINT_TYPE_BULK,            /* bmAttributes */
  WBVAL(64),                         /* wMaxPacketSize */
  0,                                 /* bInterval */
/* Bulk Out Endpoint */
  USB_ENDPOINT_DESC_SIZE,            /* bLength */
  USB_ENDPOINT_DESCRIPTOR_TYPE,      /* bDescriptorType */
  0x03,                              /* bEndpointAddress */
  USB_ENDPOINT_TYPE_BULK,            /* bmAttributes */
  WBVAL(64),                         /* wMaxPacketSize */
  0,                                 /* bInterval */
/* Terminator */
  0                                  /* bLength */
};

/* USB HSConfiguration Descriptor */
/*   All Descriptors (Configuration, Interface, Endpoint, Class, Vendor */
const uint8_t USB_HSConfigDescriptor[] = {
  /* Configuration 1 */
  USB_CONFIGUARTION_DESC_SIZE,       /* bLength */
  USB_CONFIGURATION_DESCRIPTOR_TYPE, /* bDescriptorType */
  WBVAL(                             /* wTotalLength */
    1*USB_CONFIGUARTION_DESC_SIZE +
    1*USB_INTERFACE_DESC_SIZE     +
    2*USB_ENDPOINT_DESC_SIZE      +
    1*USB_INTERFACE_DESC_SIZE     +
    1*0x36                        +
    2*USB_ENDPOINT_DESC_SIZE      +
    1*USB_INTERFACE_DESC_SIZE     +
    2*USB_ENDPOINT_DESC_SIZE
  ),
  0x03,                              /* bNumInterfaces */
  0x01,                              /* bConfigurationValue */
  0x00,                              /* iConfiguration */
  USB_CONFIG_BUS_POWERED /*|*/       /* bmAttributes */
  /*USB_CONFIG_REMOTE_WAKEUP*/,
  USB_CONFIG_POWER_MA(500),          /* bMaxPower */
  /* Interface 0, Alternate Setting 0, MSC Class */
  USB_INTERFACE_DESC_SIZE,           /* bLength */
  USB_INTERFACE_DESCRIPTOR_TYPE,     /* bDescriptorType */
  0x00,                              /* bInterfaceNumber */
  0x00,                              /* bAlternateSetting */
  0x02,                              /* bNumEndpoints */
  USB_DEVICE_CLASS_STORAGE,          /* bInterfaceClass */
  MSC_SUBCLASS_SCSI,                 /* bInterfaceSubClass */
  MSC_PROTOCOL_BULK_ONLY,            /* bInterfaceProtocol */
  0x04,                              /* iInterface */
  /* Bulk In Endpoint */
  USB_ENDPOINT_DESC_SIZE,            /* bLength */
  USB_ENDPOINT_DESCRIPTOR_TYPE,      /* bDescriptorType */
  MSC_EP_IN,                         /* bEndpointAddress */
  USB_ENDPOINT_TYPE_BULK,            /* bmAttributes */
  WBVAL(128),                     /* wMaxPacketSize */
  0,                                 /* bInterval */
  /* Bulk Out Endpoint */
  USB_ENDPOINT_DESC_SIZE,            /* bLength */
  USB_ENDPOINT_DESCRIPTOR_TYPE,      /* bDescriptorType */
  MSC_EP_OUT,                        /* bEndpointAddress */
  USB_ENDPOINT_TYPE_BULK,            /* bmAttributes */
  WBVAL(128),                     /* wMaxPacketSize */
  0,                                 /* bInterval */
/* Interface 1, SBI (CCID) */
  USB_INTERFACE_DESC_SIZE,           /* bLength */
  USB_INTERFACE_DESCRIPTOR_TYPE,     /* bDescriptorType */
  0x01,                              /* bInterfaceNumber */
  0x00,                              /* bAlternateSetting */
  0x02,                              /* bNumEndpoints */
  0x0B,                              /* bInterfaceClass */
  0x00,                              /* bInterfaceSubClass */
  0x00,                              /* bInterfaceProtocol */
  0x05,                              /* iInterface */

    /* CCID Class-Specific Descriptor */
    0x36,                   // Size of this descriptor in bytes (bLength)
    0x21,                   // Functional descriptor type (bDescriptorType)
    0x10,0x01,              // CCID Spec Release Number in BCD format -1.1(bcdCCID)
    0x00,                   // Slot number(bMaxSlotIndex)
    0x02,					// 3volt card support (bits or 1=5v, 2=3v, 4=1.8v) (bVoltageSupport)
    0x03,0,0,0,				// Support for T0/T1 Protocol (dwProtocols)
	//0x00,0,0,0,				// Support for T0/T1 Protocol (dwProtocols)
    DWBVAL((unsigned long)SMART_CARD_CLOCK_IN_KHZ),          // Default clock  (dwDefaultClock)
    DWBVAL((unsigned long)SMART_CARD_CLOCK_IN_KHZ),      	// Maximum clock  (dwMaximumClock)
    0,						// use only default clock freq (bNumClockSupported)
    DWBVAL((unsigned long)SMART_CARD_DEFAULT_DATA_RATE), 		// Default Baudrate	10752 (dwDataRate)
    DWBVAL(250000),		// Max Baudrate 250 kbps (dwMaxDataRate)
    0,						// support all baudrates (bNumDataRatesSupported)
    //254,0,0,0,          	    // For T1, n/a (dwMaxIFSD)
	0,0,0,0,          	    // For T1, n/a (dwMaxIFSD)
    0,0,0,0,				// No Synchronous protocol supported (dwSynchProtocols)
    0,0,0,0,				// mechanical.  no special charachteristics. (dwMechanical)
    0xB6,0,2,0,				// features enabled: automatic parameter configuration, Automatic activation of ICC, Automatic ICC clock frequency change, Automatic baud rate change, Automatic PPS, short apdu exchange (dwFeatures)
//0xB6,0,0,0,				// features enabled: automatic parameter configuration, Automatic activation of ICC, Automatic ICC clock frequency change, Automatic baud rate change, Automatic PPS, short apdu exchange (dwFeatures)
    0x0F,1,0,0,				// max len 200hex (arbitrary) (dwMaxCCIDMessageLength)
    0xFF,					// CCID echoes class of APDU (bClassGetResponse)
    0xFF,					// n/a. extended T0 is not supported (bClassEnvelope)
    0x00,0,					// (wLcdLayout)
    3,						// pin verification/modify support (bPINSupport)
    1,						// simultaneous busy slots (bMaxCCIDBusySlots)

/* Bulk In Endpoint */
  USB_ENDPOINT_DESC_SIZE,            /* bLength */
  USB_ENDPOINT_DESCRIPTOR_TYPE,      /* bDescriptorType */
  0x82,                              /* bEndpointAddress */
  USB_ENDPOINT_TYPE_BULK,            /* bmAttributes */
  WBVAL(64),                         /* wMaxPacketSize */
  0,                                 /* bInterval */
/* Bulk Out Endpoint */
  USB_ENDPOINT_DESC_SIZE,            /* bLength */
  USB_ENDPOINT_DESCRIPTOR_TYPE,      /* bDescriptorType */
  0x02,                              /* bEndpointAddress */
  USB_ENDPOINT_TYPE_BULK,            /* bmAttributes */
  WBVAL(64),                         /* wMaxPacketSize */
  0,                                 /* bInterval */
/* Interface 2, SVI */
  USB_INTERFACE_DESC_SIZE,           /* bLength */
  USB_INTERFACE_DESCRIPTOR_TYPE,     /* bDescriptorType */
  0x02,                              /* bInterfaceNumber */
  0x00,                              /* bAlternateSetting */
  0x02,                              /* bNumEndpoints */
  0xFF,                              /* bInterfaceClass */
  0xFF,                              /* bInterfaceSubClass */
  0x00,                              /* bInterfaceProtocol */
  0x06,                              /* iInterface */
/* Bulk In Endpoint */
  USB_ENDPOINT_DESC_SIZE,            /* bLength */
  USB_ENDPOINT_DESCRIPTOR_TYPE,      /* bDescriptorType */
  0x84,                              /* bEndpointAddress */
  USB_ENDPOINT_TYPE_BULK,            /* bmAttributes */
  WBVAL(64),                         /* wMaxPacketSize */
  0,                                 /* bInterval */
/* Bulk Out Endpoint */
  USB_ENDPOINT_DESC_SIZE,            /* bLength */
  USB_ENDPOINT_DESCRIPTOR_TYPE,      /* bDescriptorType */
  0x04,                              /* bEndpointAddress */
  USB_ENDPOINT_TYPE_BULK,            /* bmAttributes */
  WBVAL(64),                         /* wMaxPacketSize */
  0,                                 /* bInterval */
  /* Terminator */
  0                                  /* bLength */
};

/* USB String Descriptor (optional) */
uint8_t USB_StringDescriptor[] =
{
  /* Index 0x00: LANGID Codes */
  0x04,                              /* bLength */
  USB_STRING_DESCRIPTOR_TYPE,        /* bDescriptorType */
  WBVAL(0x0409), /* US English */    /* wLANGID */
  /* Index 0x01: Manufacturer */
  (18*2 + 2),                        /* bLength (13 Char + Type + lenght) */
  USB_STRING_DESCRIPTOR_TYPE,        /* bDescriptorType */
  'N', 0,
  'X', 0,
  'P', 0,
  ' ', 0,
  'S', 0,
  'e', 0,
  'm', 0,
  'i', 0,
  'c', 0,
  'o', 0,
  'n', 0,
  'd', 0,
  'u', 0,
  'c', 0,
  't', 0,
  'o', 0,
  'r', 0,
  's', 0,
  /* Index 0x02: Product */
  (14*2 + 2),                        /* bLength (13 Char + Type + lenght) */
  USB_STRING_DESCRIPTOR_TYPE,        /* bDescriptorType */
  '5', 0,
  'M', 0,
  '1', 0,
  '3', 0,
  'X', 0,
  'P', 0,
  '1', 0,
  '8', 0,
  '1', 0,
  'E', 0,
  '1', 0,
  ' ', 0,
  ' ', 0,
  ' ', 0,
  /* Index 0x03: Serial Number */
  (13*2 + 2),                        /* bLength (13 Char + Type + lenght) */
  USB_STRING_DESCRIPTOR_TYPE,        /* bDescriptorType */
  'A', 0,
  'B', 0,
  'C', 0,
  'D', 0,
  '1', 0,
  '2', 0,
  '3', 0,
  '4', 0,
  '5', 0,
  '6', 0,
  '7', 0,
  '8', 0,
  '9', 0,
  /* Index 0x04: Interface 0, Alternate Setting 0 */
  (6*2 + 2),                        /* bLength (13 Char + Type + lenght) */
  USB_STRING_DESCRIPTOR_TYPE,        /* bDescriptorType */
  'M', 0,
  'e', 0,
  'm', 0,
  'o', 0,
  'r', 0,
  'y', 0,
  /* Index 0x05: Interface 1, Alternate Setting 0 */
  (3*2 + 2),                        /* bLength (13 Char + Type + lenght) */
  USB_STRING_DESCRIPTOR_TYPE,        /* bDescriptorType */
  'S', 0,
  'B', 0,
  'I', 0,
  /* Index 0x06: Interface 1, Alternate Setting 0 */
  (3*2 + 2),                        /* bLength (13 Char + Type + lenght) */
  USB_STRING_DESCRIPTOR_TYPE,        /* bDescriptorType */
  'S', 0,
  'V', 0,
  'I', 0,
};

uint8_t *USB_String_Serial = &USB_StringDescriptor[74];

uint8_t usb_set_serial(const uint8_t* serial)
{
    uint8_t i;

    if(serial == NULL) return 1;

    for(i = 0; i < 13; i++)
        if(!((serial[i] >= '0' && serial[i] <= '9') || (serial[i] >= 'A' && serial[i] <= 'F'))) return 2;

    for(i = 0; i < 13; i++) USB_String_Serial[i*2] = serial[i];

    return 0;
}

/* USB Device Qualifier */
const uint8_t USB_DeviceQualifier[] = {
  USB_DEVICE_QUALI_SIZE,             	/* bLength */
  USB_DEVICE_QUALIFIER_DESCRIPTOR_TYPE,	/* bDescriptorType */
  WBVAL(0x0200), /* 2.00 */          /* bcdUSB */
  0x00,                              /* bDeviceClass */
  0x00,                              /* bDeviceSubClass */
  0x00,                              /* bDeviceProtocol */
  USB_MAX_PACKET0,                   /* bMaxPacketSize0 */
  0x01,                              /* bNumOtherSpeedConfigurations */
  0x00                               /* bReserved */
};

const uint8_t* USB_FSOtherSpeedConfiguration = USB_FSConfigDescriptor;
const uint8_t* USB_HSOtherSpeedConfiguration = USB_HSConfigDescriptor;

