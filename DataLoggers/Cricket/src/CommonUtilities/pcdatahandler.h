#ifndef _PCDATA_HANDLER_H_
#define _PCDATA_HANDLER_H_
#include "fifo.h"



/*-------------------------------------------------------*/
// PC Message header struct
/*-------------------------------------------------------*/
typedef struct
{
	uint8_t     PacketLength; // complete packet length
	uint8_t     SourceID;     // message source ID
	uint8_t     DestID;       // message destination ID
	uint8_t		MsgID;        // message ID
	uint8_t     OpCode;       // Message code
	uint8_t     FrameNumber;
	uint8_t     DataLength;   // data length in Message

}PC_PACKET_HEADER;


/*-------------------------------------------------------*/
// Data Handler result type 
/*-------------------------------------------------------*/
typedef enum {
	MSG_OK,
	MSG_NOT_COMPLETED,
	MSG_ERR,
}MSG_RES;

//////////////////////////////////////////////////////////////////////////
/************************************************************************/
/*  PCMessagesHandler : is a pointer to the function that will handel   */
/*						a valid message after it was decoded		    */
/************************************************************************/
extern void (*LibPCMessagesHandler)(uint8_t* pMessage);

//////////////////////////////////////////////////////////////////////////
/************************************************************************/
/*  PCSendMsg : is a pointer to the function that will send			    */
/*				data to  pc											    */
/************************************************************************/
extern bool (*LibPCSendMsg)(uint8_t* Buffer,uint8_t Len);



//////////////////////////////////////////////////////////////////////////
/************************************************************************/
/*  HandleReceivedPCData : function that will decode data received from */
/*						   PC, if the message is valid it will call		*/
/*						   PCMessagesHandler							*/	
/************************************************************************/
MSG_RES LibHandleReceivedPCData(fifo_desc_t *pStack);

//////////////////////////////////////////////////////////////////////////
/************************************************************************/
/*  PCPacketTransmit : function will encode data and send it to PC		*/
/************************************************************************/
bool LibPCPacketTransmit(uint8_t Dest,uint8_t FrameNumber,uint8_t MsgID,uint8_t OpCode, uint8_t *p, uint8_t len);

#endif
