/*
  Common Telemetry stuff
*/

#include <string.h>
#include <math.h>
#include <asf.h>

#include "conf_board.h"
#include "W25Q128FV.h"
#include "Telemetry.h"

extern volatile uint32_t time_stamp;
extern uint8_t myID;
volatile extern uint16_t flash_page_index;
extern char     flash_page_buf[];
extern uint32_t TelemetrySequence;

bool TelemetryWriteToFlash(enum eTelemTypes type, void *rawData)
{
	int length = 0;
	struct sTelemHeader *pHD = (struct sTelemHeader *)rawData;
	pHD->ID = myID;
	pHD->type = type;
	pHD->timestamp = time_stamp;
	switch (*(enum eTelemTypes *)rawData)
	{
		case MID41:
			// we have to calculate actual length
			length = sizeof(struct sMID41_Data); 
			break;
		case LSM9DS0_ALL:
		case LSM9DS0_CALIB:
			length = sizeof(struct sLSM9DS0_Data);
			break;
		case MS561101BA03:
			length = sizeof(struct sMS5611_Data);
			break;
		case MS561101BA03_CALIB:
			length = sizeof(struct sMS5611_Calibration);
			break;
		default:
			return false;		
	}
	// common write to flash
	// check if enough space
	if ((flash_page_index + length) < PAGE_SIZE)
	{
		// copy to buffer
		memcpy(&flash_page_buf[flash_page_index],rawData,length);
		flash_page_index += length;
	}
	else
	{
		// write current page to flash
		WritePageBufferToFlash();
		// copy to buffer
		memset(flash_page_buf,0xff,PAGE_SIZE);
		memcpy(flash_page_buf,rawData,length);
		flash_page_index = length;
	}
	return false;
}

void TelemetryReset()
{
	memset(flash_page_buf,0xff,PAGE_SIZE);
	flash_page_index = 0;	
}