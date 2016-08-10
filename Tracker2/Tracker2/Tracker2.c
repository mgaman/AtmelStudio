/*
 * Tracker2.c
 *
 * Created: 3/23/2015 1:57:40 PM
 *  Author: henryd
 */ 


#include <avr/io.h>
#include <asf.h>
#include "Tracker.h"
#include "ReliableUART.h"
#include "LedManager.h"
#include "Telemetry.h"
#include "FlashHandler.h"
#include "ORG1411.h"

extern fifo_desc_t PC_rx_fifo;
// global data
bool request_for_flash_data;
bool memoryFull = false;
bool record_to_flash = false;
bool VarioCalibRcvd = false;
bool LSMCalibRcvd = false;
volatile uint32_t NextPageAvailable;
volatile uint16_t flash_page_index;
uint8_t     flash_page_buf[PAGE_SIZE];
uint32_t pages_counter_for_offset_save;
volatile uint32_t pagesWrittenToFlash;
uint32_t lsmRowsRecorded,varRowsRecorded,GPSRecorded;
volatile uint32_t time_stamp;
bool ConnectedToPC = false;

uint8_t myID = 99;
void tracker(void)
{
	bool gotcommand;
	uint8_t command;
	// kick off 5Hz timer for GPS
	rtc_set_alarm_relative(200);
	while(true)
    {
        // check PC fifo for commands
		gotcommand = (fifo_pull_uint8(&PC_rx_fifo,&command) == FIFO_OK);
		if (gotcommand)
		{
			switch(command)
			{
				case ConnectToTracker:
					SerialWrapSendByte(AckFromTracker);
//					LSM9DS0Reset();
//					MS561101BA03Reset();
					TelemetryReset();
					SerialWrapSendByte(myID);
					ConnectedToPC = true;
					break;
				case GetFlashData:
					request_for_flash_data = true;
					SerialWrapSendByte(AckGetFlashData);
					SendFlashDataToHost();
					break;
				case StopSendingFlashData:
					request_for_flash_data = false;
					SerialWrapSendByte(AckStopFlashData);
					break;
				case VarRadioButton:
				case LSMRadioButton:
				case GPSRadioButton:
				case AllSensorRadioButton:
					break;
				case CleanFlash:
					eraseAllFlash();
	//				LSM9DS0Reset();
		//			MS561101BA03Reset();
					TelemetryReset();
					break;
				case RecordToFlash:
					if (memoryFull)
						SerialWrapSendByte( MemoryFullAck);
					else
					{
						record_to_flash = true;
						LedOn(LED2);
						SerialWrapSendByte(RecordToFlashAck);
					}
					break;
				case StopRecordingToFlash:
					record_to_flash = false;
					LedOff(LED2);
					SerialWrapSendByte(StopRecordingFlashAck);
					break;
				case  WriteOneRowToFlash:
					break;
				case GetFlashOffset:
					sendFlashOffsetAndLastRecordsToHost();
					break;
				case VarioCalibReceived:
					VarioCalibRcvd = true;
					break;
				case LSM96D0CalibReceived:
					LSMCalibRcvd = true;
					break;
				default:
					break;
			}
		}
		ORG1411Handler();
    }
}