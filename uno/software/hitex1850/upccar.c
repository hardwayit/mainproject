#include <string.h>
#include "lpc18xx.h"
#include "lpc_types.h"
#include "lpc18xx_scu.h"
#include <stdio.h>

#include "usb.h"
#include "usbcfg.h"
#include "usbhw.h"
#include "usbcore.h"
#include "usbuser.h"

#include "lpc18xx_cgu.h"
#include "lpc18xx_libcfg.h"

#include "wait.h"


extern DQH_T ep_QH[EP_NUM_MAX];
extern DTD_T ep_TD[EP_NUM_MAX];
extern uint32_t DevStatusFS2HS;

extern void USB_EndPoint0 (uint32_t event);


int main (void)
{
    LPC_USBDRV_INIT_T usb_cb;
    SystemInit();
    CGU_Init();

    memset((void*)&usb_cb, 0, sizeof(LPC_USBDRV_INIT_T));
    usb_cb.USB_Reset_Event = USB_Reset_Event;
    usb_cb.USB_P_EP[0] = USB_EndPoint0;
    usb_cb.USB_P_EP[1] = USB_EndPoint1;
    usb_cb.USB_P_EP[2] = USB_EndPoint2;
    usb_cb.USB_P_EP[3] = USB_EndPoint3;
    usb_cb.USB_P_EP[4] = USB_EndPoint4;
    usb_cb.ep0_maxp = USB_MAX_PACKET0;

    uart_init();
    spilcd_init();


    __enable_irq();

    USB_Init(&usb_cb);

    NVIC_DisableIRQ(USB0_IRQn);

    NVIC_SetPriority(USB0_IRQn, (1UL<<3) | 2);

    NVIC_EnableIRQ(USB0_IRQn);

    USB_Connect(TRUE);

    while(1)
    {
    }
}

void system_blink_critical_error(uint32_t errcode)
{
    char chs[10], i = 0, j;

    __disable_irq();

    memset(chs, 0xFF, 10);

    led_set(0, 0);

    wait_us(1000000);

    while(errcode)
    {
        chs[i++] = errcode%10;
        errcode /= 10;
    }

    while(1)
    {
        for(i = 0; chs[i] < 10; i++)
        {
            for(j = 0; j < chs[i]; j++)
            {
                led_set(0, 1);
                wait_us(300000);
                led_set(0, 0);
                wait_us(500000);
            }

            wait_us(1000000);
        }

        wait_us(5000000);
    }
}

void system_get_serial(uint32_t* dst)
{
    memset(dst, 0xAA, 16);
}

void default_exception_handler(void)
{
}

void SVCHandler_main(unsigned int * svc_args)
{    
}

#ifdef  DEBUG
void check_failed(uint8_t *file, uint32_t line)
{
	/* User can add his own implementation to report the file name and line number,
	 ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

	/* Infinite loop */
	while(1);
}
#endif

