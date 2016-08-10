/*
 * EepromHandler.c
 *
 * Created: 11/18/2013 5:46:10 PM
 *  Author: pc
 */ 
#include <asf.h>
#define EEPROM_MAIN
#include "RFHandler.h"
#include "EepromHandler.h"


//---------------------------------------------------------------------------------------
void ReadEEPROMFromPage(unsigned page, unsigned offset, uint8_t *buffer, unsigned length)
{
	int i;
	for (i=0;i<length;i++)
   	   buffer[i] = nvm_eeprom_read_byte((page*EEPROM_PAGE_SIZE)+offset+i);
}


//-------------------------------------------------------------------------------------
void WriteToEEPROMPage(unsigned page, unsigned offset, uint8_t *buffer, unsigned length)
{
	int i;
	for (i=0;i<length;i++)
   	  nvm_eeprom_write_byte((page*EEPROM_PAGE_SIZE)+offset+i, buffer[i]);
}


//---------------------------------------------------
void ReadEEPROMByte(uint16_t Address, uint8_t *Value)
{
	*Value = nvm_eeprom_read_byte(Address);
}


// --------------------------------------------------
void WriteEEPROMByte(uint16_t Address, uint8_t Value)
{
	nvm_eeprom_write_byte(Address,Value);
}


//------------------------------------------------------------
void ReadEEPROM(uint16_t Address, uint8_t *Buf, uint16_t Size)
{
//	wdt_reset();
	nvm_eeprom_read_buffer(Address,Buf,Size);
}


// ------------------------------------------------------------
void StoreEEPROM(uint16_t Address, uint8_t *Buf, uint16_t Size)
{
	uint16_t i;
	uint8_t tmp;
	for (i=0;i<Size;i++)
	{
//		wdt_reset();
		ReadEEPROMByte(Address+i,&tmp);
		if (tmp != Buf[i])
		WriteEEPROMByte(Address+i,Buf[i]);
	}
}


//==============================================================
// save data to eeprom with calculated check sum of the total
// data and stored after
void SaveToEEPROM_WithCS(uint16_t addr, uint8_t* p, uint16_t size)
{
	uint32_t cs = crc_io_checksum(p,size,CRC_16BIT);
	StoreEEPROM(addr      ,p,size);
	StoreEEPROM(addr+size ,(uint8_t *)&cs,sizeof(uint16_t));
}


//==============================================================
// read data from eeprom with calculated check sum of the total
// data and compare the saved check sum and the calculated
bool ReadFromEEPROM_WithCS(uint16_t addr, uint8_t* p, uint16_t size)
{
	uint16_t cs2;
//	wdt_reset();
	ReadEEPROM(addr      ,p,size);
	uint32_t cs1 = crc_io_checksum(p,size,CRC_16BIT);
	ReadEEPROM(addr+size ,(uint8_t*)&cs2,sizeof(cs2));
	return ((uint16_t)cs1 == cs2);
}


