/**********************************************************************
* $Id$		system_LPC18xx.c			2011-06-02
*//**
* @file		system_LPC18xx.c
* @brief	Cortex-M3 Device System Source File for NXP LPC18xx Series.
* @version	1.0
* @date		02. June. 2011
* @author	NXP MCU SW Application Team
*
* Copyright(C) 2011, NXP Semiconductor
* All rights reserved.
*
***********************************************************************
* Software that is described herein is for illustrative purposes only
* which provides customers with programming information regarding the
* products. This software is supplied "AS IS" without any warranties.
* NXP Semiconductors assumes no responsibility or liability for the
* use of the software, conveys no license or title under any patent,
* copyright, or mask work right to the product. NXP Semiconductors
* reserves the right to make changes in the software without
* notification. NXP Semiconductors also make no representation or
* warranty that such application will be suitable for the specified
* use without further testing or modification.
* Permission to use, copy, modify, and distribute this software and its
* documentation is hereby granted, under NXP Semiconductors'
* relevant copyright in the software, without fee, provided that it
* is used in conjunction with NXP Semiconductors microcontrollers.  This
* copyright, permission, and disclaimer notice must appear in all copies of
* this code.
**********************************************************************/

#include "lpc18xx.h"
#include <wait.h>

/*----------------------------------------------------------------------------
  Define clocks
 *----------------------------------------------------------------------------*/
#define __IRC            (12000000UL)    /* IRC Oscillator frequency          */

/*----------------------------------------------------------------------------
  Clock Variable definitions
 *----------------------------------------------------------------------------*/
uint32_t SystemCoreClock = __IRC;		/*!< System Clock Frequency (Core Clock)*/

extern uint32_t getPC(void);

/**
 * Initialize the system
 *
 * @param  none
 * @return none
 *
 * @brief  Setup the microcontroller system.
 *         Initialize the System.
 */
void SystemInit (void)
{
    SystemCoreClock = __IRC;

	// Enable VTOR register to point to vector table
    SCB->VTOR = 0x10080000;

    LPC_CREG->ETBCFG |= 1;

    wait_init();
}

void system_reset(void)
{
    LPC_WWDT->MOD |= 1UL<<1;
	LPC_WWDT->MOD |= 1UL<<0;
    LPC_WWDT->FEED = 0xAA;
    LPC_WWDT->FEED = 0x55;
}

uint8_t isappmem(uint32_t addr, uint32_t size)
{
    const uint32_t app_off = 0x10000000;
    const uint32_t app_sz  = 0x00018000;

    if(addr >= app_off && addr+size <= app_off+app_sz) return 1;

    return 0;
}

