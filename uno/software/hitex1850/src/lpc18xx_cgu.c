/**********************************************************************
* $Id$		lpc18xx_cgu.c		2011-06-02
*//**
* @file		lpc18xx_cgu.c
* @brief	Contains all functions support for Clock Generation and Control
* 			firmware library on LPC18xx
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
**********************************************************************/

/* Peripheral group ----------------------------------------------------------- */
/** @addtogroup CGU
 * @{
 */

/* Includes ------------------------------------------------------------------- */
#include "lpc_types.h"
#include "lpc18xx_scu.h"
#include "lpc18xx_cgu.h"


uint32_t CGU_Init(void)
{
    volatile uint32_t i;
    uint32_t psel, nsel, msel;

    LPC_CGU->XTAL_OSC_CTRL &= ~(1<<2);

	LPC_CGU->XTAL_OSC_CTRL &= ~CGU_CTRL_EN_MASK;

    /*Delay for stable clock*/
    for(i = 0;i<1000000;i++);

    LPC_CGU->PLL1_CTRL = (LPC_CGU->PLL1_CTRL&(~CGU_CTRL_SRC_MASK)) | (CGU_CLKSRC_XTAL_OSC<<24) | CGU_CTRL_AUTOBLOCK_MASK;

    for(i = 0;i<100;i++);

    psel = 0x03;
    nsel = 0x03;
    msel = 0xFF;

	LPC_CGU->PLL1_CTRL &= ~(CGU_PLL1_FBSEL_MASK |
									CGU_PLL1_BYPASS_MASK |
									CGU_PLL1_DIRECT_MASK |
									(psel<<8) | (msel<<16) | (nsel<<12));

    for(i = 0;i<100;i++);

    LPC_CGU->PLL1_CTRL |= (5UL<<16) | (1UL<<8) | (0UL<<12) | CGU_PLL1_FBSEL_MASK;// nsel = 0, msel = 5, psel = 1

    for(i = 0;i<100;i++);

    LPC_CGU->PLL1_CTRL &= ~CGU_CTRL_EN_MASK;

    /*if PLL is selected, check if it is locked */
    while((LPC_CGU->PLL1_STAT&1) == 0x0);

    /*post check lock status */
    if(!(LPC_CGU->PLL1_STAT&1)) while(1);

    LPC_CGU->BASE_M3_CLK = (LPC_CGU->BASE_M3_CLK&(~CGU_CTRL_SRC_MASK)) | (CGU_CLKSRC_PLL1<<24) | CGU_CTRL_AUTOBLOCK_MASK;

	return 0;
}

/* --------------------------------- End Of File ------------------------------ */
