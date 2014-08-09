#include <lpc_types.h>
#include <lpc18xx.h>
#include <core_cm3.h>


#include <wait.h>

static uint32_t local_time;

void wait_init(void)
{
    local_time = 0;

    SysTick->LOAD = 7200; /* cycle longs 100 us */
    SysTick->VAL = 7200;
    SysTick->CTRL = (1UL<<0) | (1UL<<2);
}

void wait_us(uint32_t us)
{
    int i;

    for(i = 0; i < us/100; i++) while(!(SysTick->CTRL&(1UL<<16)));
}

uint32_t time(void)
{
    return local_time;
}

void SysTick_Handler(void)
{
    local_time++;
}

