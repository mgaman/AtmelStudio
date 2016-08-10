/*
 * spi_interface.c
 *
 * Created: 30/11/2012 15:26:28
 *  Author: Mounir
 */ 
//#include "Application01/app1_defines.h"
#include <asf.h>
#include "spi_interface.h"
#include "SI4432.h"
#include "delay.h"


void Delay_MS(uint32_t ms)
{
	delay_ms(ms);
}

void spi_init(void)
{	
	struct spi_device spi_device_conf;

	spi_master_init(RF_SPI);
	spi_master_setup_device(RF_SPI, &spi_device_conf, SPI_MODE_0,
	1000000, 0);
	spi_enable(RF_SPI);	
	pbSpiReadWrite = bSpiReadWrite;
	vDelay_MS = Delay_MS;	
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  +
  + FUNCTION NAME:  uint8_t bSpi_SendDataNoResp(uint8_t bDataInLength, uint8_t *pbDataIn)
  +
  + DESCRIPTION:    send data over SPI no response expected
  +
  + INPUT:          bDataInLength - length of data
  +                 *pbDataIn     - pointer to the data
  +
  + RETURN:         None
  +
  + NOTES:          None
  +
  +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
uint8_t bSpi_SendDataNoResp(uint8_t bDataInLength, uint8_t *pbDataIn)
{
  
  spi_write_packet(RF_SPI,pbDataIn,bDataInLength);
  return 0;
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  +
  + FUNCTION NAME:  uint8_t bSpi_SendDataGetResp(uint8_t bDataOutLength, uint8_t *pbDataOut)
  +
  + DESCRIPTION:    send dummy data over SPI and get the response
  +
  + INPUT:          bDataOutLength  - length of data to be read
  +                 *pbDataOut      - response
  +
  + RETURN:         None
  +
  + NOTES:          None
  +
  +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
uint8_t bSpi_SendDataGetResp(uint8_t bDataOutLength, uint8_t *pbDataOut)   // Send data, get the response
{ 
 spi_read_packet(RF_SPI,pbDataOut,bDataOutLength); 
  return 0;
}



uint8_t bSpiReadWrite(uint8_t data_in)
{
	//SPIF1 = 0;                          // cleat SPIF
	//SPI1DAT = data_in;					//write data into the SPI register
	spi_put(RF_SPI,data_in);
	while( spi_is_tx_ok(RF_SPI) == false);					//wait for sending the data
	//SPIF1 = 0;							//clear interrupt flag
	return spi_get(RF_SPI);						//read received bytes
}
