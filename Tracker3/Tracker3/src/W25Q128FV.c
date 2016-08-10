/*
  W25Q128FV handler
  Connected to SPIC
  
*/

#include <asf.h>
#include "W25Q128FV.h"
#include "Telemetry.h"
//#include "ReliableUART.h"
#include "HDLC.h"
#include "Tracker.h"

extern volatile unsigned long NextPageAvailable;
extern volatile uint32_t pagesWrittenToFlash;
//extern uint32_t pages_counter_for_offset_save;
extern uint8_t    flash_page_buf[];
extern bool request_for_flash_data;
extern uint32_t lsmRowsRecorded,MS5611RowsRecorded,GPSRecorded;
void CDCWrapSendByte(uint8_t);
void sendFlashOffsetAndLastRecordsToHost(void);

struct spi_device asf_spi_device_conf =
{
	.id = IOPORT_CREATE_PIN(PORTC, 4)
};

/*
  Sanity check on manufacturer
*/
uint8_t spi_transfer(uint8_t info)
{
	uint8_t retVal = 0;
	
	SPI_MASTER.DATA = info;
	while(!(SPI_MASTER.STATUS & SPI_IF_bm));
	retVal = SPI_MASTER.DATA;
	
	return(retVal);
}

uint8_t manuID = 0;
uint8_t deviceID = 0;
bool W25Q128Init(void)
{
	spi_master_init(&SPI_MASTER);
	spi_master_setup_device(&SPI_MASTER, &asf_spi_device_conf, SPI_MODE_0, 104000000, 0);
	spi_enable(&SPI_MASTER);
	spi_select_device(&SPI_MASTER, &asf_spi_device_conf);
	for (int i=0; i < 5; i++)
		manuID = spi_transfer(SPI_FLASH_MANUFACTURER_ID);
	deviceID = spi_transfer(SPI_FLASH_MANUFACTURER_ID);
	spi_deselect_device(&SPI_MASTER, &asf_spi_device_conf);
	return (manuID == WINBOND_MANUF_ID && deviceID == WINBOND_DEVICE_ID);
}
// wait until chip not busy
//************
void notBusy()
{
    spi_select_device(&SPI_MASTER, &asf_spi_device_conf);
    spi_transfer(ReadStatus);
    while(spi_transfer(0) & 1){}
    spi_deselect_device(&SPI_MASTER, &asf_spi_device_conf);
} 


// enable writing
//****************
void writeEnable()
{
    notBusy();
    spi_select_device(&SPI_MASTER, &asf_spi_device_conf);
    spi_transfer(WriteEnable);
    spi_deselect_device(&SPI_MASTER, &asf_spi_device_conf);
}  


// Better to erase all flash before recording data
// rather than erasing block block while we are working
void W25Q128EraseAllFlash()
{
	writeEnable();
	notBusy();
	spi_select_device(&SPI_MASTER, &asf_spi_device_conf);
	spi_transfer(ChipErase);
	spi_deselect_device(&SPI_MASTER, &asf_spi_device_conf);
	notBusy();
	NextPageAvailable = 0;
	CDCWrapSendByte(CleanFlashAck); // send this AFTER chip erase
}
bool flashIsFull()
{
	return NextPageAvailable >= (PAGE_BIT_MAP_START/PAGE_SIZE);
}

// send a command to the flash followed by a 3-uint8_t address
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
	                                                                      
    spi_select_device(&SPI_MASTER, &asf_spi_device_conf);
	sendCommandAndAddress(PageProgram, addr);
	for (; len ; --len)
	   spi_transfer (*data++);
    spi_deselect_device(&SPI_MASTER, &asf_spi_device_conf);	
	notBusy();
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
void WritePageBufferToFlash()
{
	if (flashIsFull())
		return;
	write_to_flash(NextPageAvailable * PAGE_SIZE, flash_page_buf, 255 /*(uint8_t)FLASH_PAGE_SIZE*/);
	pagesWrittenToFlash++;
	//pages_counter_for_offset_save++;
	
	CDCWrapSendByte(TelemBlockToFLash);
	updatePageBitMap();
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

uint32_t GetNextAvailablePage()
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
//************************
void SendFlashDataToHost()
{
	uint32_t addre = 0;
	uint8_t fromFlash[PAGE_SIZE];
	uint32_t cookedlength;
	uint8_t *cm;
	int offset,length;
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
				case MID41:
					length = sizeof(struct sMID41_Data);
					break;
				case LSM9DS0_CALIB:
					length = sizeof(struct sLSM9SD0_Calib);
					break;
				case LSM9DS0_ALL:
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
				CDCWrapSendByte(StartFlashDataToHost);
//				cm = SerialWrap(&fromFlash[offset],length,&cookedlength);
				cm = HDLCStuff(&fromFlash[offset],length,&cookedlength);
				udi_cdc_write_buf(cm,cookedlength);
				CDCWrapSendByte(EndFlashDataToHost);	
			}
			offset += length;			
		}
		
		CDCWrapSendByte(FlashPageCountToHost);
//		cm = SerialWrap(&addre,sizeof(uint32_t),&cookedlength);
		cm = HDLCStuff((uint8_t *)&addre,sizeof(uint32_t),&cookedlength);
		udi_cdc_write_buf(cm,cookedlength);
		addre++;
		
		if (!request_for_flash_data)
		  break;
		  
		delay_ms(1);  
	}
	request_for_flash_data = false;
	CDCWrapSendByte(NoMoreFlashData);         
}

//****************************************
static uint8_t *cm;
void sendFlashOffsetAndLastRecordsToHost()
{
	uint32_t cookedlength;
    uint32_t printbuf[4];	
	int i;
	printbuf[0] = NextPageAvailable;
	printbuf[1] = GPSRecorded;
	printbuf[2] = lsmRowsRecorded;
	printbuf[3] = MS5611RowsRecorded;
	CDCWrapSendByte(StartFlashOffset);
//	cm = SerialWrap(printbuf,sizeof(printbuf),&cookedlength);
	cm = HDLCStuff((uint8_t *)printbuf,sizeof(printbuf),&cookedlength);
#if 0
	udi_cdc_write_buf(cm,cookedlength);
#else
	for (i=0;i<cookedlength;i++)
		udi_cdc_putc(*cm++);
#endif
	CDCWrapSendByte(EndFlashOffset);
}
