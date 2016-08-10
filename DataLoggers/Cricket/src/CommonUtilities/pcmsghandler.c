/*=======================================================================================
* Project		: RFTerminal
* Version		:
* Author		: Mounir Shmordok
* Company		: AL Hadas Technologies
* Date			: 1-2-2011
* Modify		: add your modify log
*
*======================================================================================*/




#include <string.h>
#include <stdio.h>
#include <asf.h>
#define UARTHANDLER_MAIN
#include "pcmsghandler.h"
#include "pcdatahandler.h"
#include "EepromHandler.h"
#include "RFHandler.h"


extern fifo_desc_t pc_com_rx_fifo;
extern uint8_t		pc_com_rx_buffer[];
extern fifo_desc_t pc_com_tx_fifo;
extern uint8_t		pc_com_tx_buffer[];
extern fifo_desc_t pc_com_tx_debug_fifo;
extern uint8_t		pc_com_tx_debug_buffer[];

uint8_t eeprombuf[32];	// for read/write app data
void PCMessagesHandler_Init(void)
{
	LibPCMessagesHandler = PCRecievedMessagesHandler; // init function that will be called when a valid message received
	LibPCSendMsg		  = PCSendMessage;				   // init function that will transmit the packets to PC	
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Handle PC messages received from UART or From Network
// MsgSource indicates where the message received from in order to know if it should be forwarded
// or handled and Ack PC
void PCRecievedMessagesHandler(uint8_t* pMessage)
{
	memset(&pcmessage, 0, sizeof(pcmessage));
	
#if 0
	PC_PACKET_HEADER RXHeader;     
    bool messagetosend = false;
	int channel;
	uint16_t *pEepromDataOffset;
	
	memcpy(&RXHeader,pMessage,sizeof(RXHeader));	
       
	uint8_t DataStartIndex = sizeof(RXHeader);      // index of data length
	switch (RXHeader.MsgID)
	{
		case MSG_SHAKEHANDE://==================================================================================================================
			LibPCPacketTransmit(RXHeader.SourceID, RXHeader.FrameNumber,RXHeader.MsgID,RXHeader.OpCode,0,0);
			break;
		case MSG_VERSION:{//=====================================================================================================================		
			version_t version = {.major = VER_MAJOR, .minor = VER_MINOR, .revision = VER_REV};
			LibPCPacketTransmit(RXHeader.SourceID, RXHeader.FrameNumber,RXHeader.MsgID,RXHeader.OpCode,(uint8_t*)&version,sizeof(version));	
			}
			break;
	
		case MSG_RESET://=======================================================================================================================
			if (RXHeader.OpCode == 0)
			{
				LibPCPacketTransmit(RXHeader.SourceID, RXHeader.FrameNumber,RXHeader.MsgID,RXHeader.OpCode,0,0);
				reset_do_soft_reset();
			}                 
			break;
		case MSG_SETTINGS:
			 if (RXHeader.OpCode == 1 && RXHeader.DataLength == sizeof(ApplicationData))
			 {						  
				  memcpy(&ApplicationData,&pMessage[DataStartIndex],RXHeader.DataLength);
				  SaveToEEPROM_WithCS(_EEPROM_ADDR_SETTINGS,(uint8_t*)&ApplicationData,sizeof(ApplicationData));
			 }			 
			 LibPCPacketTransmit(RXHeader.SourceID, RXHeader.FrameNumber,RXHeader.MsgID,RXHeader.OpCode,(uint8_t*)&ApplicationData,sizeof(ApplicationData));
			break;
		case MSG_RF_SETTINGS:
			 channel = RXHeader.OpCode & 0x03;
			 if ((RXHeader.OpCode & 0x80) == 0x80) // change for multiple devices
			 {	
				//write
				memset((uint8_t*)&RFdataBuffer[channel].RFparameters,0,sizeof(tRFparameters));			  
				memcpy((uint8_t*)&RFdataBuffer[channel],&pMessage[DataStartIndex],RXHeader.DataLength);
				SaveToEEPROM_WithCS(EEPROM_RFDATA_BASE + (channel * sizeof(struct sRFdataCS)),(uint8_t*)&RFdataBuffer[channel],sizeof(tRFparameters));
			 }
			 else if ((RXHeader.OpCode & 0x80) == 0)
			 {
				 // read
			//	 RFSettingsRestoreDefaults(channel);
			//	 RFSettingsSaveToEEprom(channel);
			 }	
			LibPCPacketTransmit(RXHeader.SourceID, RXHeader.FrameNumber,RXHeader.MsgID,RXHeader.OpCode,
					(uint8_t*)&RFdataBuffer[channel].RFparameters,sizeof(tRFparameters));
			break;
			
			case EEPROM_GET:
				// we do a trick here, opcode & frame number are combined to give short offset address
				pEepromDataOffset = (uint16_t *)&RXHeader.OpCode;
				// check that data length is as expected
				if (RXHeader.DataLength == 1)
				{
					if (!ReadFromEEPROM_WithCS(*pEepromDataOffset,&eeprombuf[sizeof(uint16_t)],pMessage[DataStartIndex]))
						memset(eeprombuf,0xFF,sizeof(eeprombuf));	// clear buffer if bad checksum
					// send data back, first 2 bytes of data are offset in eeprom
					*(uint16_t *)eeprombuf = *pEepromDataOffset;
					LibPCPacketTransmit(RXHeader.DestID,0,EEPROM_DATA,0,eeprombuf,pMessage[DataStartIndex]+sizeof(uint16_t));
				}
				break;
				
			case EEPROM_PUT:
				// we do a trick here, opcode & frame number are combined to give short offset address
				pEepromDataOffset = (uint16_t *)&RXHeader.OpCode;
				SaveToEEPROM_WithCS(*pEepromDataOffset,&pMessage[DataStartIndex],RXHeader.DataLength);
				break;
		default:
			break;
	}
	
	if (messagetosend)
	{
		++LocalMessageCounter;
		// set up for repetition
		RepeatDataLength = sizeof(uint16_t);
		memcpy(RepeatData,&LocalMessageCounter,sizeof(uint16_t));
		MessageRepeatCount = MSG_REPEAT_COUNT;
		MessageRepeatTimeout = MSG_REPEAT_DELAY;
		cpu_irq_disable();
#ifdef MSG_REPEAT_ENFORCE
		MessageRepeatActive = true;
#else
		MessageRepeatActive = false;
#endif
		MessageToSend = true;
		cpu_irq_enable();		
	}
#else
    memcpy(&pcmessage,pMessage,sizeof(pcmessage));	
  //memmove(&pcmessage,pMessage,sizeof(pcmessage));
#endif
}
	

void UART_StartTransmition()
{
	uint8_t tmp = 0xff;
	if (fifo_is_empty(&pc_com_tx_fifo) == false)
	{
		fifo_pull_uint8(&pc_com_tx_fifo,&tmp);
		usart_putchar(USART_SERIAL_PC,tmp);
	}		
	else if (fifo_is_empty(&pc_com_tx_debug_fifo) == false)
	{
		fifo_pull_uint8(&pc_com_tx_debug_fifo,&tmp);
		usart_putchar(USART_SERIAL_PC,tmp);	
	}		
}

bool PCSendMessage(uint8_t* Buffer,uint8_t Len)
{
	for(uint8_t i =0 ; i< Len ; i++)
	  if (fifo_push_uint8(& pc_com_tx_fifo,Buffer[i]) != FIFO_OK)
		  return false;
		  
		  
	//if ((UartTXStack[UartIndex_PC],Buffer,Len) == 0) return false;
	   UART_StartTransmition();  
        return true;

}

