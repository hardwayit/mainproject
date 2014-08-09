#include "spilcd.h"
#include "lpc18xx.h"
#include "lpc18xx_scu.h"
#include "lpc18xx_cgu.h"
#include "lpc18xx_spi.h"
#include "logo.h"
#include <wait.h>

uint8_t logo[512];

void spilcd_init()
{
    uint8_t cmd;
    uint16_t i, j;

	LPC_CGU->BASE_SSP0_CLK = (LPC_CGU->BASE_SSP0_CLK&(~CGU_CTRL_SRC_MASK)) | (CGU_CLKSRC_PLL1<<24) | CGU_CTRL_AUTOBLOCK_MASK;
	
	/* Configure SSP0 pins*/
	scu_pinmux(0xF,0,MD_PLN_FAST,FUNC0);	// connected to SCL/SCLK	func0=SSP0 SCK0
 	scu_pinmux(0xF,3,MD_PLN_FAST,FUNC2);	// connected to MOSI		func5=SSP0 MOSI0
 	scu_pinmux(0xD,0,MD_PLN_FAST,FUNC4);	// connected to LCD_RST    func4=GPIO6.14
 	scu_pinmux(0x2,5,MD_PLN_FAST,FUNC4);	// connected to LCD_A0     func4=GPIO5.5
 	scu_pinmux(0xF,1,MD_PLN_FAST,FUNC2);	// connected to SSEL		func4=GPIO5.11

 	scu_pinmux(0xF,2,MD_PLN_FAST,FUNC2);	// connected to SO 		func5=SSP0 MISO0

 	scu_pinmux(0xC,11,MD_PLN_FAST,FUNC4);	// connected to MUX0		func4=GPIO6.10
 	scu_pinmux(0xC,12,MD_PLN_FAST,FUNC4);	// connected to MUX1		func4=GPIO6.11

    gpio_set_dir(6, 10, 1); gpio_set_value(6, 10, 0);
    gpio_set_dir(6, 11, 1); gpio_set_value(6, 11, 0);

    gpio_set_dir(5, 11, 1);

    gpio_set_dir(5, 5, 1); gpio_set_value(5, 5, 0);
    gpio_set_dir(6, 14, 1); gpio_set_value(6, 14, 0);
 
	LPC_SSP0->CR0 	= 0xFFC7 ; //0:3=0x7 - 8 of bits in frame, 4:5 = 0 - SPI, 6:7 = 0 - CPOL CPHA, 8:15 = 15 SCR, 16:32 - N/A
	LPC_SSP0->CR1 	= 0x2 ; //4:31 - N/A
	LPC_SSP0->CPSR 	= 2;
	LPC_SSP0->IMSC 	= 0;
	LPC_SSP0->DMACR = 0;

    for(j = 0; j < 4; j++)
        for(i = 0; i < 128; i++)
        {
            logo[j*128 + i] = 0;
            logo[j*128 + i] |= ((logo_bits[i/8 + j*128 + 16*0]&(1<<(i%8)))>0?1:0)<<0;
            logo[j*128 + i] |= ((logo_bits[i/8 + j*128 + 16*1]&(1<<(i%8)))>0?1:0)<<1;
            logo[j*128 + i] |= ((logo_bits[i/8 + j*128 + 16*2]&(1<<(i%8)))>0?1:0)<<2;
            logo[j*128 + i] |= ((logo_bits[i/8 + j*128 + 16*3]&(1<<(i%8)))>0?1:0)<<3;
            logo[j*128 + i] |= ((logo_bits[i/8 + j*128 + 16*4]&(1<<(i%8)))>0?1:0)<<4;
            logo[j*128 + i] |= ((logo_bits[i/8 + j*128 + 16*5]&(1<<(i%8)))>0?1:0)<<5;
            logo[j*128 + i] |= ((logo_bits[i/8 + j*128 + 16*6]&(1<<(i%8)))>0?1:0)<<6;
            logo[j*128 + i] |= ((logo_bits[i/8 + j*128 + 16*7]&(1<<(i%8)))>0?1:0)<<7;
        }


    gpio_set_value(6, 14, 1);
    wait_us(100);

    spilcd_cmd(0xA0);
    spilcd_cmd(0xAE);
    spilcd_cmd(0xC8);
    spilcd_cmd(0xA2);
    spilcd_cmd(0x2F);
    spilcd_cmd(0x21);
    spilcd_cmd(0x81);
    spilcd_cmd(0x2F);

    //spilcd_cmd(0xAF);
    //spilcd_cmd(0xA7);
    spilcd_cmd(0xA4);
    //spilcd_cmd(0xAF);

    spilcd_cmd(0xA6);
    spilcd_cmd(0xAF);

    spilcd_cmd(0x40);

    for(i = 0; i < 4; i++)
    {
        spilcd_cmd(0xB0+i);
        spilcd_cmd(0x10);
        spilcd_cmd(0x00);
        for(j = 0; j < 128; j++) spilcd_dat(logo[i*128+j]);
    }
}

void spi_cs_low(void)
{
    gpio_set_value(0xF, 1, 0);
}
void spi_cs_high(void)
{
    gpio_set_value(0xF, 1, 1);
}

void spilcd_cmd(uint8_t cmd)
{
    spi_cs_low();
    gpio_set_value(5, 5, 0);

    spi_xfer(&cmd, NULL, 1);
    spi_cs_high();
}

void spilcd_dat(uint8_t dat)
{
    spi_cs_low();
    gpio_set_value(5, 5, 1);

    spi_xfer(&dat, NULL, 1);
    spi_cs_high();
}

