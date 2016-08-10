/*
 * SpiFlash.c
 *
 * Created: 6/18/2014 4:08:30 PM
 *  Author: User
 */ 


#include <string.h>
#include <stdio.h>
#include <math.h>
#include <asf.h>

#include "SpiFlash.h"
//#include "Utilities.h"
#include "EepromHandler.h"
#include "i2c_bar.h"
#include "ReliableUART.h"
#include "Tracker.h"
#include "Telemetry.h"

 struct spi_device asf_spi_device_conf =
 {
    .id = IOPORT_CREATE_PIN(PORTC, 4)
 };

extern bool write_one_row_to_flash; 
extern volatile uint32_t time_stamp;

//extern uint16_t save_to_eprom_maxval;

extern volatile char     flash_page_buf[];
extern volatile uint16_t flash_page_index;

extern volatile unsigned long NextPageAvailable;
extern volatile unsigned long pages_counter_for_offset_save;
extern volatile unsigned long pagesWrittenToFlash;

extern unsigned long gpsRowsRecorded;
extern unsigned long lsmRowsRecorded;

extern char lsm_ax[6];
extern char lsm_ay[6];
extern char lsm_az[6];

extern char lsm_gx[6];
extern char lsm_gy[6];
extern char lsm_gz[6];

extern char lsm_mx[6];
extern char lsm_my[6];
extern char lsm_mz[6];



//#define FLASH_PAGE_LENGTH 256
//#define PAGES_IN_SECTOR 256
//#define OFFSET_EPROM_ADDRESS 0x400



//**************************
void asf_spi_init_pins(void)
{
	ioport_configure_port_pin(&PORTC, PIN4_bm, IOPORT_INIT_HIGH | IOPORT_DIR_OUTPUT);   //slave select
	ioport_configure_port_pin(&PORTC, PIN5_bm, IOPORT_INIT_HIGH | IOPORT_DIR_OUTPUT);   //mosi
	ioport_configure_port_pin(&PORTC, PIN6_bm, IOPORT_DIR_INPUT);                       //miso
	ioport_configure_port_pin(&PORTC, PIN7_bm, IOPORT_INIT_HIGH | IOPORT_DIR_OUTPUT);   //clock
}


//*****************
void SPI_Init(void)
{
	asf_spi_init_pins();
	
	spi_master_init(&SPIC);
	//spi_master_setup_device(&SPIC, &asf_spi_device_conf, SPI_MODE_0, 1000000, 0);
	spi_master_setup_device(&SPIC, &asf_spi_device_conf, SPI_MODE_0, 104000000, 0);
	spi_enable(&SPIC);
}


//***********************
bool spi_flash_check()
{
	uint8_t retval = 0;	
	uint8_t manuID = 0;
	spi_select_device(&SPIC, &asf_spi_device_conf);	
	for (int i=0; i < 5; i++)
	   manuID = spi_transfer(SPI_FLASH_MANUFACTURER_ID);
	spi_deselect_device(&SPIC, &asf_spi_device_conf);	
	return(manuID == 0xEF);
}


//********************************
uint8_t spi_transfer(uint8_t info)
{
	uint8_t retVal = 0;
	
	SPIC.DATA = info;
	while(!(SPIC.STATUS & SPI_IF_bm));
	retVal = SPIC.DATA;
	
	return(retVal);
}


// wait until chip not busy
//************
void notBusy(void)
{
    spi_select_device(&SPIC, &asf_spi_device_conf);
    spi_transfer(ReadStatus);
    while(spi_transfer(0) & 1){}
    spi_deselect_device(&SPIC, &asf_spi_device_conf);
} 


// enable writing
//****************
void writeEnable(void)
{
    notBusy();
	
    spi_select_device(&SPIC, &asf_spi_device_conf);
    spi_transfer(WriteEnable);
    spi_deselect_device(&SPIC, &asf_spi_device_conf);
}  


// send a command to the EEPROM followed by a 3-uint8_t address
//**************************************************************************
void sendCommandAndAddress (const uint8_t command, const unsigned long addr)
{
    spi_transfer(command);
    spi_transfer((addr >> 16) & 0xFF);
    spi_transfer((addr >> 8)  & 0xFF);
    spi_transfer(addr & 0xFF);
} 


//************************************************************************
void write_to_flash(const unsigned long addr, uint8_t * data, uint8_t len)
{
	writeEnable();
	notBusy();                                                                      // wait until ready
	                                                                      
    spi_select_device(&SPIC, &asf_spi_device_conf);
	sendCommandAndAddress(PageProgram, addr);
	for (; len ; --len)
	   spi_transfer (*data++);
    spi_deselect_device(&SPIC, &asf_spi_device_conf);
	
	notBusy();
} 


// read len uint8_ts from device
//******************************************************************************
void read_from_flash(const unsigned long addr, uint8_t * data, unsigned int len)
{
	notBusy();                                                                       // wait until ready
	                                                                               
	spi_select_device(&SPIC, &asf_spi_device_conf);
	sendCommandAndAddress(ReadArray, addr);	
	spi_transfer(0);                                                                 // clock in "don't care" uint8_t		
	for ( ; len ; --len)
	   *data++ = spi_transfer(0);
	spi_deselect_device(&SPIC, &asf_spi_device_conf);
}  

#ifndef FLASH_ERASE_CHIP
// erase a 64Kb block of uint8_ts which contains addr
//**********************************************
void erase_flash_block(const unsigned long addr)
{
    writeEnable();
    notBusy(); 
	                                                                               
    spi_select_device(&SPIC, &asf_spi_device_conf);
    sendCommandAndAddress(BlockErase64Kb, addr);         
	spi_deselect_device(&SPIC, &asf_spi_device_conf);
}  


//**********************************************************************
void erase_flash_with_param(uint8_t eraseType, const unsigned long addr)
{
    writeEnable();
    notBusy(); 
	                                                                                
    spi_select_device(&SPIC, &asf_spi_device_conf);
    sendCommandAndAddress(eraseType, addr);
	spi_deselect_device(&SPIC, &asf_spi_device_conf);
}  
#endif


//----------------------------------------------------------------------- HANDLING OFFSET AND RECORDS IN EPROM --------------------------------------------------------
//****************
bool flashIsFull()
{
#ifndef FLASH_ERASE_CHIP
	bool answ = false;
	
	if (NextPageAvailable >= (PAGE_BIT_MAP_START/PAGE_SIZE))   //16711680 - 100
	{
	   memoryFull = true;
	   //usart_putchar(USART_SERIAL_PC, 0x80);
	   answ = true;
	}
	
	return(answ);
#else
	return NextPageAvailable >= (PAGE_BIT_MAP_START/PAGE_SIZE);
#endif
}


#ifndef FLASH_ERASE_CHIP
//***********************
void saveOffsetToEeprom()
{
	 unsigned char byteArray[4];
	
	 byteArray[0] = (int)((NextPageAvailable & 0xFF000000) >> 24 );
	 byteArray[1] = (int)((NextPageAvailable & 0x00FF0000) >> 16 );
	 byteArray[2] = (int)((NextPageAvailable & 0x0000FF00) >> 8 );
	 byteArray[3] = (int)((NextPageAvailable & 0X000000FF));	 
	 
     unsigned long addr = OFFSET_EPROM_ADDRESS;
	 for (int i = 0; i < 4; i++)
	 {
		WriteEEPROMByte(addr, byteArray[i]); 
		addr += 1;
	 }
}


//*************************
void readOffsetFromEeprom()
{
	 uint8_t tmp;
	 unsigned char byteArray[4];	 
	 unsigned long temp;
	 
	 bool no_offset = true;
	 
	 unsigned long addr = OFFSET_EPROM_ADDRESS;
	 for (int i = 0; i < 4; i++)
	 {
		 ReadEEPROMByte(addr, &tmp);
		 byteArray[i] = tmp;	
		 addr += 1;
		 if (tmp != 0xff)
		   no_offset = false;
	 }
	 
	 if  (no_offset)
	    NextPageAvailable = 0;
	 else
	   {		   
		   temp  = (unsigned long)  byteArray[3];
		   temp |= ((unsigned long) byteArray[2]) << 8;
		   temp |= ((unsigned long) byteArray[1]) << 16;
		   temp |= ((unsigned long) byteArray[0]) << 24;
		   
	       NextPageAvailable = temp;
	   }
	   
	 if (flashIsFull())
     {
     }
}

//*************************
void deleteOffsetInEeprom()
{	 
	 uint16_t addr = OFFSET_EPROM_ADDRESS;
	 for (int i = 0; i < 4; i++)
	 {
		WriteEEPROMByte(addr, 0xff); 
		addr += 1;
	 }
}
#endif


#if 0
//*********************************************************************
void saveRecordsToEeprom(unsigned long recVal, unsigned long epromAddr)
{
	 unsigned char byteArray[4];
	
	 byteArray[0] = (int)((recVal & 0xFF000000) >> 24 );
	 byteArray[1] = (int)((recVal & 0x00FF0000) >> 16 );
	 byteArray[2] = (int)((recVal & 0x0000FF00) >> 8 );
	 byteArray[3] = (int)((recVal & 0X000000FF));	 
	 
     unsigned long addr = epromAddr;
	 for (int i = 0; i < 4; i++)
	 {
		WriteEEPROMByte(addr, byteArray[i]); 
		addr += 1;
	 }
}


//**********************************************************
unsigned long readRecordsFromEeprom(unsigned long epromAddr)
{
	 uint8_t tmp;
	 unsigned char byteArray[4];	 
	 unsigned long temp;
	 
	 unsigned long answ = 0;
	 	 
	 bool no_rec = true;
	 
	 unsigned long addr = epromAddr;
	 for (int i = 0; i < 4; i++)
	 {
		 ReadEEPROMByte(addr, &tmp);
		 byteArray[i] = tmp;	
		 addr += 1;
		 if (tmp != 0xff)
		   no_rec = false;
	 }
	 
	 if  (no_rec)
	    answ = 0;
	 else
	   {
		   temp  = (unsigned long)  byteArray[3];
		   temp |= ((unsigned long) byteArray[2]) << 8;
		   temp |= ((unsigned long) byteArray[1]) << 16;
		   temp |= ((unsigned long) byteArray[0]) << 24;
		   
	       answ = temp;
	   }
	   
	 return(answ);  
}


//*************************
void cleanRecordsInEeprom()
{	 
	 uint16_t addr = 0;
	 
	 addr = GPS_REC_ADDR;
	 for (int i = 0; i < 4; i++)
	 {
		WriteEEPROMByte(addr, 0xff); 
		addr += 1;
	 }
	 
	 addr = LSM_REC_ADDR;
	 for (int i = 0; i < 4; i++)
	 {
		WriteEEPROMByte(addr, 0xff); 
		addr += 1;
	 }
	 
	 addr = VAR_REC_ADDR;
	 for (int i = 0; i < 4; i++)
	 {
		WriteEEPROMByte(addr, 0xff); 
		addr += 1;
	 } 
}


#ifndef FLASH_ERASE_CHIP
//*************************	
void deleteOffsetAndFlash()
{	
	 cleanRecordsInEeprom();
	 deleteOffsetInEeprom();
	 readOffsetFromEeprom();
	 
	 erase_flash_block(0);
	 erase_flash_block(16711680);
	 
	 NextPageAvailable = 0;
	 
	 //char fromFlash[FLASH_BLOCK_LENTGH];
	 //for (int ff = 0; ff < 10; ff++)
	 //{
		 //readEEPROM(flashOffset, fromFlash, sizeof(fromFlash));
		 //flashOffset += FLASH_BLOCK_LENTGH;
	 //}
}
#endif

//----------------------------------------------------------------------- HANDLING OFFSET AND RECORDS IN FLASH --------------------------------------------------------

//**********************
void saveOffsetToFlash()
{
    char flash_offset_buf[8];
    
    for (int ff = 0; ff < 8; ff++)
       flash_offset_buf[ff] = 0x20;
    
    ltoa(NextPageAvailable, flash_offset_buf, 10);	
	erase_flash_block(16711680);    
    write_to_flash(16711680, flash_offset_buf, sizeof(flash_offset_buf));   	
}


//*************************
void readOffsetFromFlash()
{
	char offsetBuf[8];
  	
 	read_from_flash(16711680, offsetBuf, sizeof(offsetBuf));	  
  	NextPageAvailable = atol(offsetBuf);
 }


//************************
void readSampleFlashData()
{
	unsigned long addre = 256;
	char fromFlash[256];
	
	for (unsigned long ff = 0; ff < 10; ff++)
	{
		read_from_flash(addre, fromFlash, sizeof(fromFlash));
		addre += sizeof(fromFlash);
	}
}
#endif

//****************************************
void sendFlashOffsetAndLastRecordsToHost()
{
	uint32_t cookedlength;
	uint8_t *cm;
	int i;
    char printbuf[50];	
	unsigned long gpsrec = 0;
	snprintf(printbuf,50,"%lu;%lu;%lu;%lu;",NextPageAvailable,gpsrec,lsmRowsRecorded,varRowsRecorded);	
	UartWrapSendByte(USART_SERIAL_PC, StartFlashOffset);
	cm = UartWrap(printbuf,strlen(printbuf),&cookedlength);	
	for (i=0; i<cookedlength;i++)
		usart_putchar(USART_SERIAL_PC,*cm++);
	UartWrapSendByte(USART_SERIAL_PC, EndFlashOffset);
}
//************************
void SendFlashDataToHost()
{
	uint32_t addre = 0;
	char fromFlash[PAGE_SIZE];
	uint32_t cookedlength;
	uint8_t *cm;
	int i,offset,length;
	struct sTelemHeader *pTH;
	bool valid;
    while (addre < NextPageAvailable)		
	{
		valid = true;
		read_from_flash((addre*PAGE_SIZE), fromFlash, PAGE_SIZE);
		offset = 0;
		while (valid && offset < PAGE_SIZE)  // no more data
		{
			pTH = (struct sTelemHeader *)&fromFlash[offset];
			switch (pTH->type)
			{
				case LSM9DS0_ALL:
				case LSM9DS0_CALIB:
					length = sizeof(struct sLSM9DS0_Data);
					break;
				case MS561101BA03:
					length = sizeof(struct sMS5611_Data );
					break;
				case MS561101BA03_CALIB:
					length = sizeof(struct sMS5611_Calibration );
					break;
				default:
					valid = false;  // bail out
					break;				
			}
			if (valid)
			{
				UartWrapSendByte(USART_SERIAL_PC, StartFlashDataToHost);
				cm = UartWrap(&fromFlash[offset],length,&cookedlength);
				for (i=0;i<cookedlength;i++)
					usart_putchar(USART_SERIAL_PC,*cm++);
				UartWrapSendByte(USART_SERIAL_PC, EndFlashDataToHost);	
			}
			offset += length;			
		}
		
		UartWrapSendByte(USART_SERIAL_PC,FlashPageCountToHost);
		cm = UartWrap(&addre,sizeof(uint32_t),&cookedlength);
		for (i=0;i<cookedlength;i++)
			usart_putchar(USART_SERIAL_PC,*cm++);
		addre++;
		
		if (!request_for_flash_data)
		  break;
		  
		delay_ms(1);  
	}
	
	request_for_flash_data = false;
	UartWrapSendByte(USART_SERIAL_PC, NoMoreFlashData);         
}
/*
	64K pages in 16MB, use highest 8K in flash memory for pages used
	1 bit per page, 1 means available, 0 used
	After erase of all flash this bit map is initialized to all available
*/

void updatePageBitMap()
{
	// Calculate byte offset and bit offset of next byte to write, turning 1 to 0
	// this technique coped from Miznach, CriticalError routines
	uint32_t byte_offset = NextPageAvailable/8;
	unsigned char mask = ~(1<<(NextPageAvailable % 8));  
	write_to_flash(PAGE_BIT_MAP_START + byte_offset,&mask,1);
	NextPageAvailable++;
}

uint32_t GetCurrentPageFromEEPROM()
{
	uint32_t count = 0;
	uint8_t bitmap = 0;
	int index = 0;
	while (bitmap != 0xff)
	{
		// read next byte from map and increment the count 
		read_from_flash(PAGE_BIT_MAP_START + index++,&bitmap,1);
		switch (bitmap)
		{
			case 0x00:
				count += 8;
				break;
			case 0xfe:
				count += 1;
				break;
			case 0xfc:
				count += 2;
				break;
			case 0xf8:
				count += 3;
				break;
			case 0xf0:
				count += 4;
				break;
			case 0xe0:
				count += 5;
				break;
			case 0xc0:
				count += 6;
				break;
			case 0x80:
				count += 7;
				break;
		}
	}
	return count;
}

//***************************
void WritePageBufferToFlash()
{
	if (flashIsFull())
	  return;	  
#ifndef FLASH_ERASE_CHIP 
	if (pagesWrittenToFlash == 256 /*PAGES_IN_SECTOR*/)                               
	{
	   erase_flash_block(LastPageUsed);
	   pagesWrittenToFlash = 0;	 	   
    }
#endif		   
	write_to_flash(NextPageAvailable * PAGE_SIZE, flash_page_buf, 255 /*(uint8_t)FLASH_PAGE_SIZE*/);    
	pagesWrittenToFlash++;  
	pages_counter_for_offset_save++;
	
	UartWrapSendByte(USART_SERIAL_PC, TelemBlockToFLash);
	
#ifndef FLASH_ERASE_CHIP
	clean_flash_buff();
	
	if (pages_counter_for_offset_save >= 100)    
	{
		pages_counter_for_offset_save = 0;
		
	  //saveOffsetToEeprom();
		saveOffsetToFlash();
	}	
#else
	updatePageBitMap();
#endif
}

// Better to erase all flash before recording data 
// rather than erasing block block while we are working
void eraseAllFlash()
{
    writeEnable();
    notBusy();
    
    spi_select_device(&SPIC, &asf_spi_device_conf);
    spi_transfer(ChipErase);
    spi_deselect_device(&SPIC, &asf_spi_device_conf);
    notBusy();
   	NextPageAvailable = 0;
	UartWrapSendByte(USART_SERIAL_PC, CleanFlashAck); // send this AFTER chip erase
}
