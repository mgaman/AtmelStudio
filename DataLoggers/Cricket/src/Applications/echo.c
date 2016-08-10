/*
 * echo.c
 *
 * Created: 11/25/2013 11:47:06 AM
 *  Author: pc
 *  Master sends message to slave. 
 *  Slave receives message, executes and echoes to master
 *  Master receives message, executes
 */ 

#include <asf.h>
#include "Applications.h"
#include "RFHandler.h"
#include "Utilities.h"

rf_packet_u decodedPacket;
uint16_t sequence;
extern uint16_t msgCounter;

static void HardwareInit(void)
{
}
static void SoftwareInit(void)
{
	RepeatActionInit();
#ifdef ECHO_MASTER
	GpioPollInit(PUSH_BUTTON_1,PIN_DOWN);
#endif	
#ifdef ECHO_SLAVE
	RFInitReceiver(0);
#endif
	msgCounter = 0;
	sequence = 0;
}

void echo(void)
{
	HardwareInit();
	SoftwareInit();
	while (true)
	{
        #ifdef ECHO_MASTER
			if (GpioPoll(PUSH_BUTTON_1))
			{
				RFInitTransmiter(0);
				RFSendPacket(0,RF_MSG_SET_ONBOARDLED,0,0xffff,&sequence,sizeof(uint16_t));
				sequence++;
				RFInitReceiver(0);	// wait for reply
			}
        #endif
		
		decodedPacket.header.packetLen = 0;
		RFReceiveMode_Handler(0);
		if (decodedPacket.header.packetLen != 0)
		{
			// we got a packet
			switch (decodedPacket.header.msgID)
			{
				case RF_MSG_SET_ONBOARDLED:
					RepeatAction(RTC_ONBOARD,10,1,1/*,OnBoardToggle*/);
					break;
				default:
					break;
			}
			
            #ifdef ECHO_SLAVE
				RFInitTransmiter(0);	// echo back to master
				RFSendPacket(0,RF_MSG_SET_ONBOARDLED,0,0xffff,&sequence,sizeof(uint16_t));
				sequence++;
				RFInitReceiver(0);
            #endif
			
            #ifdef ECHO_MASTER
			    RFIdle(0);
            #endif
		}
	}
}