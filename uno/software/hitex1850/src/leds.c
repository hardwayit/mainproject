#include <lpc18xx_gpio.h>
#include <lpc18xx_gpio.h>

#define LED_PORTNUM_1 5
#define LED_BITNUM_1  4
#define LED_PORTNUM_2 5
#define LED_BITNUM_2  5

void gpio_set_value(uint32_t portnum, uint32_t bitnum, uint8_t value)
{
    if(value > 0) GPIO_SetValue(portnum, 1UL<<bitnum);
    else GPIO_ClearValue(portnum, 1UL<<bitnum);
}

void gpio_set_dir(uint32_t portnum, uint32_t bitnum, uint8_t value)
{
    GPIO_SetDir(portnum, 1UL<<bitnum, 1);
}

void led_set(uint8_t led, uint8_t value)
{
   uint32_t portnum, bitnum;

   scu_pinmux(2, 4, 0, 4);
   scu_pinmux(2, 5, 0, 4);

   switch(led)
   {
   case 0: portnum = LED_PORTNUM_1; bitnum = LED_BITNUM_1; break;
   case 1: portnum = LED_PORTNUM_2; bitnum = LED_BITNUM_2; break;
   default: 
       return;
   }

   switch(value)
   {
   case 0:
       gpio_set_dir(portnum, bitnum, 1);
       gpio_set_value(portnum, bitnum, value);
       break;
   case 1:
       gpio_set_dir(portnum, bitnum, 1);
       gpio_set_value(portnum, bitnum, value);
       break;
   case 2:
       gpio_set_dir(portnum, bitnum, 1);
       gpio_set_value(portnum, bitnum, 0);
       break;
   }
}

