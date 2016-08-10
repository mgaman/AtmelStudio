/*
 * buzzer.c
 *
 * Created: 2/16/2014 3:45:29 PM
 *  Author: pc
 
 The only spec we have is a mail from Dov 16/2/2014
                                              3.	???? ??????
 ????? ?? ????? ?????? ???? ???? ??? ????? ???? ?????? ????
 ????? ????? ?? ?????? ?????? ?? ???? ??? ?????? ????? ????.
 ?? ???? ???? ???? ?????? ????.
 ???? ????? :??????? 100 ???.

		button down - send buzzer start message (aka led on)
		button up - send buzzer stop message (aka led off)
		No sleep mode
		
		Assuming small/medium modem
		Slave only receives, no transmit
 */ 

#include <asf.h>
#include "Utilities.h"
#include "RFHandler.h"
#include "pcdatahandler.h"
#include "ModemPWM.h"
#include "conf_board.h"

void buzzer(void);
static void HardwareInit(void);
static void SoftwareInit(void);
#define NORMALLY_CLOSED


//------------------------
static void HardwareInit()
{
	#ifdef BUZZER_MASTER
		GpioPollInit(BUZZER_ON_OFF_BUTTON,PIN_CHANGED);
		ONBOARD_LED_OFF;
		RFInitTransmiter(BUZZER_RF_ID);
	#elif defined(BUZZER_SLAVE)
		RFInitReceiver(BUZZER_RF_ID);
		HIPOWER_LED_OFF;
		ONBOARD_LED_OFF;
	
		PwmInit(0,PWM_CH_C,1000);	//AC buzzer is PC2
	#endif
}


//------------------------
static void SoftwareInit()
{
	#ifdef BUZZER_MASTER
		RepeatMsgID = RF_MSG_SET_HIPOWERLED;
	#elif defined(BUZZER_SLAVE)
	#endif	
}


//-----------
void buzzer()
{
	//RF_ENUM Ret;
	int rfnum = BUZZER_RF_ID;
	
	HardwareInit();
	SoftwareInit();
	
	rfnum++;
	
	while (true)
	{
		#ifdef BUZZER_MASTER
			if (GpioPoll(BUZZER_ON_OFF_BUTTON))
			{				
			#ifdef NORMALLY_CLOSED			
				if (ioport_pin_is_low(BUZZER_ON_OFF_BUTTON))
			#else
				if (ioport_pin_is_high(BUZZER_ON_OFF_BUTTON))
			#endif		
				{
					RepeatOpCode = TURN_ON;
					ONBOARD_LED_ON;
				}
				else
				{
					RepeatOpCode = TURN_OFF;
					ONBOARD_LED_OFF;
				}
				Ret = RFSendPacket(BUZZER_RF_ID,RepeatMsgID,RepeatOpCode,0xffff,NULL,0);
				if (Ret != RF_OK)
				{
					// what?
					Ret++;	// just for breakpoint
				}
		    }
			
        #elif defined(BUZZER_SLAVE)
		   decodedPacket.header.packetLen = 0;
		   RFReceiveMode_Handler(BUZZER_RF_ID);
		   if (decodedPacket.header.packetLen != 0)
		   {
			  switch (decodedPacket.header.msgID)
			  {
				case RF_MSG_SET_HIPOWERLED:
					switch (decodedPacket.header.opCode)
					{
						case TURN_ON:
							BUZZER_ON;
							ONBOARD_LED_ON;
							HIPOWER_LED_ON;
							PwmStart(0,50);		// 50% duty cycle
						break;
							
						case TURN_OFF:
							BUZZER_OFF;
							ONBOARD_LED_OFF;
							HIPOWER_LED_OFF;
							PwmStop(0);
						break;
					}
				break;
				
				default:
				break;
			  }
		   }
        #endif		
	}
}