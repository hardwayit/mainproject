#include "lpc18xx_spi.h"
#include "lpc18xx.h" //????????? ???????????? ?????? LPC_SSP0, ??????? ???????? ?????????((LPC_SSPn_Type*) LPC_SSP0_BASE)
#include "wait.h"


/* NOTE
	typedef struct {                        //< (@ 0x400xx000) SSPn Structure
  __IO uint32_t CR0;                        //< (@ 0x400xx000) Control Register 0. Selects the serial clock rate, bus type, and data size.
  __IO uint32_t CR1;                        //< (@ 0x400xx004) Control Register 1. Selects master/slave and other modes.
  __IO uint32_t DR;                         //< (@ 0x400xx008) Data Register. Writes fill the transmit FIFO, and reads empty the receive FIFO. 
  __I  uint32_t SR;                         //< (@ 0x400xx00C) Status Register
  __IO uint32_t CPSR;                       //< (@ 0x400xx010) Clock Prescale Register
  __IO uint32_t IMSC;                       //< (@ 0x400xx014) Interrupt Mask Set and Clear Register
  __I  uint32_t RIS;                        //< (@ 0x400xx018) Raw Interrupt Status Register
  __I  uint32_t MIS;                        //< (@ 0x400xx01C) Masked Interrupt Status Register
  __O  uint32_t ICR;                        //< (@ 0x400xx020) SSPICR Interrupt Clear Register
  __IO uint32_t DMACR;                      //< (@ 0x400xx024) SSPn DMA control register
} LPC_SSPn_Type;
*/


void spi_xfer(uint8_t* data_out, uint8_t* data_in, uint32_t count)
{
	int i;
	//wait untill the SSP) controller isn't BUSY and/or transmit FIFO is empty (the 4th bit = 0)
	while( (( LPC_SSP0->SR ) & (1UL<<4) ) );
	
	// make sure the Receive FIFO is empty (the 2nd bit = 0)
	while ( (LPC_SSP0->SR ) & 0x4 )  data_in[0] = (uint8_t) LPC_SSP0->DR;

	for(i = 0; i<count; i++)
	{
		/*sending*/
		if (data_out != NULL )LPC_SSP0->DR = (uint16_t)data_out[i];
		else LPC_SSP0->DR = 0;

		while(!(LPC_SSP0->SR & (1UL<<2)));
		
		/*reading*/
	    if(data_in != NULL) data_in[i] = (uint8_t) LPC_SSP0->DR;
	   		
		//wait untill the SSP0 controller isn't BUSY and/or transmit FIFO is empty (the 4th bit = 0)
	    while( (( LPC_SSP0->SR ) & 0x10 ) );
	}
}
uint32_t spi_get_status(void)
{
	return LPC_SSP0->SR;
}

