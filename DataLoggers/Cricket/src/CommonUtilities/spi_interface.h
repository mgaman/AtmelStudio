/*
 * spi_interface.h
 *
 * Created: 30/11/2012 15:26:41
 *  Author: Mounir
 */ 


#ifndef SPI_INTERFACE_H_
#define SPI_INTERFACE_H_
#include "compiler.h"
#include "spi_master.h"


extern volatile uint16_t g_uSpiMSCounter;

void	spi_init(void);
uint8_t bSpi_SendDataNoResp(uint8_t bDataInLength, uint8_t *pbDataIn);
uint8_t bSpi_SendDataGetResp(uint8_t bDataOutLength, uint8_t *pbDataOut);
uint8_t bSpiReadWrite(uint8_t data_in);
void Delay_MS(uint32_t ms);

#endif /* SPI_INTERFACE_H_ */