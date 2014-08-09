#ifndef LPC18XX_SPI_H
#define LPC18XX_SPI_H

#include "lpc_types.h"

void spi_cs_low(void);
void spi_cs_high(void);
void spi_xfer(uint8_t* data_out, uint8_t* data_in, uint32_t count);

uint32_t spi_get_status(void);
void spinor_int();

#endif