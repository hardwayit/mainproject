/***************************************************************************
* File		usbuser.h
* Brief		
* Date		27.03.2013
* Author	"Other Technologies", sole proprietorship Pashkova E.A.
*
* Copyright(C) 2013, "Other Technologies", sole proprietorship Pashkova E.A.
* All rights reserved.
***************************************************************************/

#ifndef __USBUSER_H__
#define __USBUSER_H__


/* USB Device Events Callback Functions */
extern void USB_Power_Event     (uint32_t power);
extern void USB_Reset_Event     (void);
extern void USB_Suspend_Event   (void);
extern void USB_Resume_Event    (void);
extern void USB_WakeUp_Event    (void);
extern void USB_SOF_Event       (void);
extern void USB_Error_Event     (uint32_t error);

/* USB Endpoint Events Callback Pointers */
extern void (* const USB_P_EP[USB_EP_NUM])(uint32_t event);

/* USB Endpoint Events Callback Functions */
extern void USB_EndPoint0  (uint32_t event);
extern void USB_EndPoint1  (uint32_t event);
extern void USB_EndPoint2  (uint32_t event);
extern void USB_EndPoint3  (uint32_t event);
extern void USB_EndPoint4  (uint32_t event);
extern void USB_EndPoint5  (uint32_t event);

/* USB Core Events Callback Functions */
extern void USB_Configure_Event (void);
extern void USB_Interface_Event (void);
extern void USB_Feature_Event   (void);


#endif  /* __USBUSER_H__ */
