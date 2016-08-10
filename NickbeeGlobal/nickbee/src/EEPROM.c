/*
 * EEPROM.c
 *
 * Created: 7/30/2013 2:20:50 PM
 *  Author: pc
 *
 * read/write page of eeprom that contains parameter data
 */ 
#include <string.h>
#include <asf.h>
#include <board.h>
#include <conf_board.h>
#define EEPROM_MAIN
#include "EEPROM.h"

// global data

/*
	do dumb version that also works across page boundaries
*/
void ReadEEPROM(unsigned page,unsigned offset,uint8_t *buffer,unsigned length)
{
	int i;
	for (i=0;i<length;i++)
		buffer[i] = nvm_eeprom_read_byte((page*EEPROM_PAGE_SIZE)+offset+i);
}

void WriteEEPROM(unsigned page,unsigned offset,uint8_t *buffer,unsigned length)
{
	int i;
	for (i=0;i<length;i++)
		nvm_eeprom_write_byte((page*EEPROM_PAGE_SIZE)+offset+i, buffer[i]);
}

