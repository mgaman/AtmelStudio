/*
 * RFHandler.h
 *
 * Created: 11/19/2013 9:08:40 AM
 *  Author: pc
 */ 

#include "SI4432.h"
#ifndef RFHANDLER_H_
#define RFHANDLER_H_

/*
*/
typedef enum
{
	RF_MSG_SET_ONBOARDLED = 1,
	RF_MSG_SET_HIPOWERLED = 2,
	RF_MSG_YUVAL_ACK,
	RF_MSG_FIREFLY_BLINK,
	RF_MSG_YUVALIT_WIFI,
	RF_CRICKET,
	RF_MSG_GAHLILIT
} RF_MSGID_TYPE_E;

enum {
	TURN_ON = 1,
	TURN_OFF,
	ACK,
	NAK,
	NOOP,
	GO_TO_SLEEP,
	WAKE_UP
};
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*							RF STRUCTS											*/
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
typedef struct
{
	uint8_t			  packetFlags;     // out of encryption
	uint8_t			  packetLen;       // out of encryption
	uint16_t          sourceMAC;
	uint16_t		  targetMAC;
	RF_MSGID_TYPE_E	  msgID;
	uint8_t			  opCode;
    uint16_t          msgRepCounter;   // counter for same message repeated sending
	uint16_t          msgIDCounter;    // counter for each different message sent
#ifdef ENABLE_ENCRYPTION
	uint8_t			  packetLengthBeforeEncrypt;    // header length is 11 bytes - we have place for 5 bytes of data
#endif
}rf_msg_header_t;


typedef struct
{
	rf_msg_header_t      header;
}rf_empty_packet_t;

typedef struct
{
	rf_msg_header_t      header;
	uint8_t              data[50];
}rf_general_packet_t;


typedef union
{
	rf_msg_header_t			header;
	rf_empty_packet_t       packet_empty;
	rf_general_packet_t     packet_data;
}rf_packet_u;

struct sRFdataBuffer
{
	tRFparameters RFparameters;
	uint8_t buffer[_MAX_RECEIVER_BUFFER];
	uint8_t length;	
};

struct sRFutilities
{
	void (*Select)(void);
	void (*Deselect)(void);
	bool (*isInterrupt)(void);
};

#ifdef RFHANDLER_MAIN
struct sRFdataBuffer RFdataBuffer[RF_MAX_NUMBER_OF_DEVICES];
tRFparameters RFdefault[RF_MAX_NUMBER_OF_DEVICES] = {
	//{ // values from S2RF Excel file 433.81Mhz 2400 bps
		//{0x1C	,       0x1D}, // SHOULD  be 1b according to Excel but Munir used 1D so we shall too
		//{0x1D	,       0x40},
		//{0x20	,       0xA1},
		//{0x21	,       0x20},
		//{0x22	,       0x27},
		//{0x23	,       0x52},
		//{0x24	,       0x10},
		//{0x25	,       0x1B},
		//{0x2A	,       0x45},
		//{0x2C	,       0x19},
		//{0x2D	,       0x4	},
		//{0x2E	,       0x2B},
		//{0x30	,       0xAC},
		//{0x32	,       0x0	},
		//{0x33	,       0x2	},
		//{0x34	,       0x4	},
		//{0x35	,       0x22},
		//{0x36	,       0xB5},
		//{0x37	,       0x6B},
		//{0x38	,       0x0	},
		//{0x39	,       0x0	},
		//{0x3A	,       0x0	},
		//{0x3B	,       0x0	},
		//{0x3C	,       0x0	},
		//{0x3D	,       0x0	},
		//{0x3E	,       0x1	},
		//{0x3F	,       0x0	},
		//{0x40	,       0x0	},
		//{0x41	,       0x0	},
		//{0x42	,       0x0	},
		//{0x43	,       0xFF},
		//{0x44	,       0xFF},
		//{0x45	,       0xFF},
		//{0x46	,       0xFF},
		//{0x6E	,       0x13},
		//{0x6F	,       0xA9},
		//{0x70	,       0x2A},
		//{0x71	,       0x2A},
		//{0x72	,       0x30},
		//{0x75	,       0x53},
		//{0x76	,       0x5F},
		//{0x77	,       0x40},
	//},	
	
	////// freq = 433.81 	
	//{
	////	{0x1C,	0x1B},
		//{0x1C,	0x1D},
		//{0x1D,	0x40},
		//{0x20,	0x41},
		//{0x21,	0x60},
		//{0x22,	0x13},
		//{0x23,	0xA9},
		//{0x24,	0x10},
		//{0x25,	0x0A},
		//{0x2A,	0x37},
		//{0x2C,	0x1A},
		//{0x2D,	0x08},
		//{0x2E,	0x2B},
		//{0x30,	0xAC},
		//{0x32,	0x00},
		//{0x33,	0x02},
		//{0x34,	0x04},
		//{0x35,	0x22},
		//{0x36,	0xB5},
		//{0x37,	0x6B},
		//{0x38,	0x00},
		//{0x39,	0x00},
		//{0x3A,	0x00},
		//{0x3B,	0x00},
		//{0x3C,	0x00},
		//{0x3D,	0x00},
		//{0x3E,	0x01},
		//{0x3F,	0x00},
		//{0x40,	0x00},
		//{0x41,	0x00},
		//{0x42,	0x00},
		//{0x43,	0xFF},
		//{0x44,	0xFF},
		//{0x45,	0xFF},
		//{0x46,	0xFF},
		//{0x6E,	0x09},
		//{0x6F,	0xD5},
		//{0x70,	0x2A},
		//{0x71,	0x2A},      // FSK 1200
	  ////{0x71,	0x2B},      // GFSK 1200
		//{0x72,	0x28},
		//{0x75,	0x53},
		//{0x76,	0x5F},
		//{0x77,	0x40},
	//},
		
	//// freq = 494.3  	
	//{
		//{0x1C,	0x1C},
		//{0x1D,	0x40},	
		//{0x20,	0x41},
		//{0x21,	0x60},
		//{0x22,	0x13},
		//{0x23,	0xA9},
		//{0x24,	0x10},
		//{0x25,	0x0A},
		//{0x2A,	0x1F},
		//{0x2C,	0x1A},
		//{0x2D,	0x08},
		//{0x2E,	0x2B},	
		//{0x30,	0xAC},
		//{0x32,	0x00},
		//{0x33,	0x02},
		//{0x34,	0x04},
		//{0x35,	0x22},
		//{0x36,	0xB5},
		//{0x37,	0x6B},
		//{0x38,	0x00},
		//{0x39,	0x00},
		//{0x3A,	0x00},
		//{0x3B,	0x00},
		//{0x3C,	0x00},
		//{0x3D,	0x00},
		//{0x3E,	0x01},
		//{0x3F,	0x00},
		//{0x40,	0x00},
		//{0x41,	0x00},
		//{0x42,	0x00},
		//{0x43,	0xFF},
		//{0x44,	0xFF},
		//{0x45,	0xFF},
		//{0x46,	0xFF},
		//{0x6E,	0x09},
		//{0x6F,	0xD5},
		//{0x70,	0x2A},
		//{0x71,	0x2A},
		//{0x72,	0x28},
		//{0x75,	0x60},
		//{0x76,	0xB2},
		//{0x77,	0xC0},
	//},
		
		
   // Nick's : freq = 433.3, br = 1200
    {		
		{0x1C,	0x16},
		{0x1D,	0x40},
		{0x20,	0x41},
		{0x21,	0x60},
		{0x22,	0x13},
		{0x23,	0xA9},
		{0x24,	0x10},
		{0x25,	0x08},
		{0x2A,	0x26},
		{0x2C,	0x1A},
		{0x2D,	0x08},
		{0x2E,	0x2B},
		{0x30,	0xAC},
		{0x32,	0x00},
		{0x33,	0x02},
		{0x34,	0x04},
		{0x35,	0x22},
		{0x36,	0xB5},
		{0x37,	0x6B},
		{0x38,	0x00},
		{0x39,	0x00},
		{0x3A,	0x00},
		{0x3B,	0x00},			
		{0x3C,	0x00},
		{0x3D,	0x00},
		{0x3E,	0x01},
		{0x3F,	0x00},
		{0x40,	0x00},
		{0x41,	0x00},
		{0x42,	0x00},
		{0x43,	0xFF},
		{0x44,	0xFF},
		{0x45,	0xFF},
		{0x46,	0xFF},
		{0x6E,	0x09},
		{0x6F,	0xD5},
		{0x70,	0x2A},
		{0x71,	0x2A},
		{0x72,	0x30},
		{0x75,	0x53},
		{0x76,	0x5F},
		{0x77,	0x00},
	},
	
	{  // 435Mhz 2400 bps 
		{0x1C	,       0x1b},
		{0x1D	,       0x40},
		{0x20	,       0xA1},
		{0x21	,       0x20},
		{0x22	,       0x27},
		{0x23	,       0x52},
		{0x24	,       0x10},
		{0x25	,       0x20},
		{0x2A	,       0x37},
		{0x2C	,       0x19},
		{0x2D	,       0x4	},
		{0x2E	,       0x2B},
		{0x30	,       0xAC},
		{0x32	,       0x0	},
		{0x33	,       0x2	},
		{0x34	,       0x4	},
		{0x35	,       0x22},
		{0x36	,       0xB5},
		{0x37	,       0x6B},
		{0x38	,       0x0	},
		{0x39	,       0x0	},
		{0x3A	,       0x0	},
		{0x3B	,       0x0	},
		{0x3C	,       0x0	},
		{0x3D	,       0x0	},
		{0x3E	,       0x1	},
		{0x3F	,       0x0	},
		{0x40	,       0x0	},
		{0x41	,       0x0	},
		{0x42	,       0x0	},
		{0x43	,       0xFF},
		{0x44	,       0xFF},
		{0x45	,       0xFF},
		{0x46	,       0xFF},
		{0x6E	,       0x13},
		{0x6F	,       0xA9},
		{0x70	,       0x2A},
		{0x71	,       0x2A},
		{0x72	,       0x28},
		{0x75	,       0x53},
		{0x76	,       0x7D},
		{0x77	,       0x0	}
	},

	};
	
uint16_t LocalMessageCounter,ReceivedMessageCounter;
volatile bool MessageRepeatActive;
//volatile bool MessageToSend;
volatile uint16_t MessageRepeatCount;
volatile uint8_t MessageRepeatTimeout;
volatile uint8_t RepeatMsgID;
volatile uint8_t RepeatOpCode;
volatile uint8_t RepeatDataLength;
volatile uint8_t RepeatData[20];
rf_packet_u decodedPacket;
#else
extern struct sRFdataBuffer RFdataBuffer[];
extern tRFparameters RFdefault[];
extern uint16_t LocalMessageCounter,ReceivedMessageCounter;
extern volatile bool MessageRepeatActive;
//extern volatile bool MessageToSend;
extern volatile uint16_t MessageRepeatCount;
extern volatile uint8_t MessageRepeatTimeout;
extern volatile uint8_t RepeatMsgID;
extern volatile uint8_t RepeatOpCode;
extern volatile uint8_t RepeatDataLength;
extern volatile uint8_t RepeatData[];
extern struct sRFutilities RFutilities[];
extern rf_packet_u decodedPacket;
#endif

// prototypes
RF_ENUM RFSendPacket(uint8_t device,RF_MSGID_TYPE_E msgid,uint8_t opcode,uint16_t target,uint8_t * buffer,uint8_t length);
void RFHandleReceivedPacket(uint8_t device,uint8_t *pBuffer,uint8_t len);
void vChipSelectRF0(void);
void vChipUnSelectRF0(void);
bool IsInterruptedRF0(void);
void vChipSelectRF1(void);
void vChipUnSelectRF1(void);
bool IsInterruptedRF1(void);

#endif /* RFHANDLER_H_ */