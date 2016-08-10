/*
 * RFHandler.c
 *
 * Created: 11/19/2013 9:08:57 AM
 *  Author: pc
 */ 
#include <string.h>
#include <asf.h>
#define RFHANDLER_MAIN
#include "RFHandler.h"
#include "EepromHandler.h"

#ifdef ENABLE_ENCRYPTION
#define BLOCK_LENGTH 16
#include "crypto.h"
#endif

//extern uint16_t msgCounter;

extern rf_packet_u decodedPacket;
extern uint16_t gvMsgRepCounter;
extern uint16_t gvMsgIDCounterTX,gvMsgIDCounterRX;
extern uint16_t prevGvMsgIDCounter;

extern bool newMessage;
extern bool sameMessageReceived;

extern uint16_t waitUntilRespond;


unsigned long truecounter[2],falsecounter[2];

volatile RF_ENUM rfResp;


//-----------------------
void vChipSelectRF0(void)
{
	//ioport_set_pin_low(SI4332_NSEL_CHANNEL0);
}


//-------------------------
void vChipUnSelectRF0(void)
{
	//ioport_set_pin_high(SI4332_NSEL_CHANNEL0);
}


bool IsInterruptedRF0(void)
{
	//bool b =  ioport_pin_is_low(SI4332_NIRQ_CHANNEL0);
	//if (b)
		//truecounter[0]++;
	//else
		//falsecounter[0]++;
	//return b;
}


//-----------------------
void vChipSelectRF1(void)
{
	//ioport_set_pin_low(SI4332_NSEL_CHANNEL1);
}


//-------------------------
void vChipUnSelectRF1(void)
{
	//ioport_set_pin_high(SI4332_NSEL_CHANNEL1);
}


bool IsInterruptedRF1(void)
{
	bool b; // = ioport_pin_is_low(SI4332_NIRQ_CHANNEL1);
	if (b)
		truecounter[1]++;
	else
		falsecounter[1]++;
	return b;
}



struct sRFutilities RFutilities[RF_MAX_NUMBER_OF_DEVICES] = 
{
	{vChipSelectRF0,vChipUnSelectRF0,IsInterruptedRF0},
	{vChipSelectRF1,vChipUnSelectRF1,IsInterruptedRF1},
	};
	
#ifdef ENABLE_ENCRYPTION
uint8_t plain_data[BLOCK_LENGTH];
uint8_t encrypted_data[BLOCK_LENGTH];
#endif




//------------------------------------------------------------------------
void RFHandleReceivedPacket(uint8_t device, uint8_t *pBuffer, uint8_t len)
{
	rf_packet_u *packet = (rf_packet_u*)(pBuffer);
	
//	bool SendMessage = false;	// local to this func
//	uint16_t sequence;
//	int j;
//	uint8_t *cp;

	if (!packet)
		return;
		
	#ifdef ENABLE_ENCRYPTION
		memset(plain_data, 0, sizeof(t_data));
		
		if  (packet->header.packetLen-2 > sizeof(plain_data))
		  return;	 

		AES_Decrypt((uint8_t *)&packet->header.sourceMAC,packet->header.packetLen-2,plain_data);
		memcpy((void *)&packet->header.sourceMAC,plain_data,packet->header.packetLen-2);
		
	 // check that data checksum is OK
		packet->header.packetLen = packet->header.packetLengthBeforeEncrypt; // restore what we had before adding checksum
		uint32_t cs = crc_io_checksum(&packet->header.packetFlags,packet->header.packetLengthBeforeEncrypt,CRC_16BIT);
		if ((uint8_t)cs != *(uint8_t *)&packet->packet_data.data[packet->header.packetLengthBeforeEncrypt - sizeof(rf_msg_header_t)])
		{
			return;
		}

	#endif
	
	// is this message intended for me ?
	#if 0
	if (!(packet->header.targetMAC == TARGET_BROADCAST || packet->header.targetMAC == ApplicationData.MacID))
		return;
	#endif
	
	
#ifdef IGNORE_MESSAGE_SEQUENCING
	memcpy((void *)&decodedPacket, packet, sizeof(rf_packet_u));
#else
	if (packet->header.msgIDCounter == 1)
	{
		prevGvMsgIDCounter = 0;
	}
	
	if (packet->header.msgIDCounter > prevGvMsgIDCounter)
	{
       //sameMessageReceived = true;		
		waitUntilRespond = (packet->header.msgRepCounter * 235 /*150*/);                    //150 for baud=2400 and 235 for baud=1200
		memcpy((void *)&decodedPacket, packet, sizeof(rf_packet_u));
		prevGvMsgIDCounter = packet->header.msgIDCounter;
	}
#endif	
	return;
}



//----------------------------------------------------------------------------------------------------------------------------------
RF_ENUM RFSendPacket(uint8_t device, RF_MSGID_TYPE_E msg, uint8_t opcode, uint16_t targetMac, uint8_t *pDataToSend, uint8_t DataLen)
{
	uint8_t delta = 0;
	rf_general_packet_t packet;
	packet.header.packetLen = sizeof(rf_msg_header_t) + DataLen;
	packet.header.msgID = msg;
	
	if (newMessage)
	{
	   packet.header.msgIDCounter = ++gvMsgIDCounterTX;
	   packet.header.msgRepCounter = gvMsgRepCounter--;
	   newMessage = false;
	}
	else
	{
	   packet.header.msgIDCounter = gvMsgIDCounterTX;
	   packet.header.msgRepCounter = gvMsgRepCounter--;
	}
	

	packet.header.opCode = opcode;
	
	packet.header.sourceMAC = ApplicationData.MacID;
	packet.header.targetMAC = targetMac;
	packet.header.packetFlags = 0;
	
	memset(packet.data,0,sizeof(packet.data));
	memcpy((void *)packet.data,pDataToSend,DataLen);
	
	// calculate checksum of data and add to the end
	#ifdef ENABLE_ENCRYPTION
	    packet.header.packetLengthBeforeEncrypt = packet.header.packetLen;
	#endif
	
	uint32_t cs = crc_io_checksum(&packet.header.packetFlags,packet.header.packetLen,CRC_16BIT);
	// place this checksum after the data
	// encrypted data area must be a multiple of 16 bytes
	*(uint16_t *)&packet.data[DataLen] = cs;

	packet.header.packetLen += sizeof(uint16_t);
	
	#ifdef ENABLE_ENCRYPTION
		delta = AES_DATA_SIZE % (packet.header.packetLen - 2);
		memset(encrypted_data,0,sizeof(t_data));
		AES_Encrypt((uint8_t *)&packet.header.sourceMAC,packet.header.packetLen-2 + delta,encrypted_data);
		// modify packet to take all of the 16 bytes of encoded data
		memcpy((void *)&packet.header.sourceMAC,encrypted_data,packet.header.packetLen -2 + delta);
	#endif
	
	packet.header.packetLen += delta;
	//	RFInitTransmiter(device);
	rfResp = RFTransmit(device, (uint8_t*)&packet, packet.header.packetLen);
	
	//return RFTransmit(device, (uint8_t*)&packet, packet.header.packetLen);
	
	return rfResp;
}
