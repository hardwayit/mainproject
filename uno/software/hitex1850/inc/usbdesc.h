/***************************************************************************
* File		usbdesc.h
* Brief		
* Date		27.03.2013
* Author	"Other Technologies", sole proprietorship Pashkova E.A.
*
* Copyright(C) 2013, "Other Technologies", sole proprietorship Pashkova E.A.
* All rights reserved.
***************************************************************************/

#ifndef __USBDESC_H__
#define __USBDESC_H__


#define WBVAL(x) (x & 0xFF),((x >> 8) & 0xFF)
#define DWBVAL(x) (x & 0xFF),((x >> 8) & 0xFF),((x >> 16) & 0xFF),((x >> 24) & 0xFF)

#define USB_DEVICE_DESC_SIZE        (sizeof(USB_DEVICE_DESCRIPTOR))
#define USB_CONFIGUARTION_DESC_SIZE (sizeof(USB_CONFIGURATION_DESCRIPTOR))
#define USB_INTERFACE_DESC_SIZE     (sizeof(USB_INTERFACE_DESCRIPTOR))
#define USB_ENDPOINT_DESC_SIZE      (sizeof(USB_ENDPOINT_DESCRIPTOR))
#define USB_DEVICE_QUALI_SIZE       (sizeof(USB_DEVICE_QUALIFIER_DESCRIPTOR))
#define USB_OTHER_SPEED_CONF_SIZE   (sizeof(USB_OTHER_SPEED_CONFIGURATION))

extern uint8_t USB_DeviceDescriptor[];
extern const uint8_t USB_FSConfigDescriptor[];
extern const uint8_t USB_HSConfigDescriptor[];
extern uint8_t USB_StringDescriptor[];
extern const uint8_t USB_DeviceQualifier[];
extern const uint8_t* USB_FSOtherSpeedConfiguration;
extern const uint8_t* USB_HSOtherSpeedConfiguration;

void usb_set_vidpid(uint16_t vid, uint16_t pid);


#endif  /* __USBDESC_H__ */
