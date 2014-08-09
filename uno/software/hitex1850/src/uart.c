#include <lpc_types.h>
#include <lpc18xx_cgu.h>
#include <lpc18xx_scu.h>
#include <lpc18xx.h>

void uart_init(void)
{
    NVIC_DisableIRQ(USART0_IRQn);
    LPC_CGU->BASE_UART0_CLK = (LPC_CGU->BASE_UART0_CLK&(~CGU_CTRL_SRC_MASK)) | (CGU_CLKSRC_XTAL_OSC<<24) | CGU_CTRL_AUTOBLOCK_MASK;

    scu_pinmux(2, 0, MD_PDN, 1);
    scu_pinmux(6, 5, MD_PLN|MD_EZI|MD_ZI, 2);

    LPC_USART0->LCR |= 0x80;
    LPC_USART0->DLL = 0x04;
    LPC_USART0->DLM = 0x00;

    LPC_USART0->LCR &= ~0x80;
    LPC_USART0->IER = 0x00;
    LPC_USART0->FCR = 0x00;
    LPC_USART0->LCR = 0x03;
    LPC_USART0->SCR = 0x00;
    LPC_USART0->ACR = 0x06;
    LPC_USART0->FDR = 0x85;
    LPC_USART0->OSR = 0xF0;
    LPC_USART0->SCICTRL = 0x00;
    LPC_USART0->RS485CTRL = 0x00;
    LPC_USART0->RS485ADRMATCH = 0x00;
    LPC_USART0->RS485DLY = 0x00;
    LPC_USART0->SYNCCTRL = 0x00;
    LPC_USART0->TER = 0x01;
}

void uart_putch(char ch)
{
    LPC_USART0->THR = ch;

    while(!(LPC_USART0->LSR & 1UL<<5));
}

void uart_send(uint8_t* buf, uint32_t size)
{
    while(size--) uart_putch(*buf++);
}

void uart_puts(char* s)
{
    while(*s) uart_putch(*s++);
}

