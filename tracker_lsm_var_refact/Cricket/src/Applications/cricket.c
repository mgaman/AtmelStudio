/*
 * cricket.c
 *
 * Created: 11/20/2013 8:03:21 AM
 *  Author: pc
 */ 
#include <asf.h>
#include "Utilities.h"
#include "RFHandler.h"
#include "pcdatahandler.h"

void cricket(void);
void setTimerToInterrupt(void);

extern fifo_desc_t pc_com_rx_fifo;
extern fifo_desc_t pc_d_com_rx_fifo;

#define CRICKET_SLEEP_MSECS 5000	// length of sleep cycle of cricket slave
#define CRICKET_RF_DEVICE 0

extern uint16_t msgCounter;
extern bool newMessage;
bool led_on;



static void HardwareInit(void)
{
//	ONBOARD_LED_OFF;	// not seen outside of the box
	//HIPOWER_LED_OFF;	// master battery
//	BUZZER_OFF;         // ?? (PORTC,2)
}



static void SoftwareInit(void)
{
	RepeatActionInit();
//	sleepmgr_init();	// without this lock counters overflow to zero and PSAVE is lower than PDOWN
	
	#ifdef CRICKET_MASTER
		sleepmgr_lock_mode(SLEEPMGR_ACTIVE);
		GpioPollInit(CRICKET_BUZZER_ON_BUTTON, PIN_DOWN);
		GpioPollInit(CRICKET_BUZZER_OFF_BUTTON,PIN_DOWN);
	
		//GpioPollInit(CRICKET_SLEEP_BUTTON,PIN_DOWN);
		//GpioPollInit(CRICKET_WAKE_BUTTON,PIN_DOWN);
	#endif


	#ifdef CRICKET_SLAVE
	 //     sleepmgr_lock_mode(SLEEPMGR_ACTIVE);
	   // sleepmgr_lock_mode(SLEEPMGR_PSAVE);   // when it will enter in sleep_mode it enters in SLEEPMGR_PSAVE - only RTC works
		RFInitReceiver(0);
	#endif
}



void cricket(void)
{
	//bool SendMessage;
	int j;
	//RF_ENUM Ret;
	//bool slave_led_to_blink = false;
	
	HardwareInit();
	SoftwareInit();
	
	while (true)
	{
		#ifdef CRICKET_MASTER
			SendMessage = false;
			if (GpioPoll(CRICKET_BUZZER_ON_BUTTON))
			{
				SendMessage = true;
				RepeatMsgID = RF_CRICKET;
				RepeatOpCode = TURN_ON;
			}
			
			else 
			if (GpioPoll(CRICKET_BUZZER_OFF_BUTTON))
			{
				SendMessage = true;
				RepeatMsgID = RF_CRICKET;
				RepeatOpCode = TURN_OFF;
			}
			
			//else if (GpioPoll(CRICKET_WAKE_BUTTON))
			//{
			//   SendMessage = true;
			//   RepeatMsgID = RF_CRICKET;
			//   RepeatOpCode = WAKE_UP;
			//}
			
			//else if (GpioPoll(CRICKET_SLEEP_BUTTON))
			//{
			//SendMessage = true;
			//RepeatMsgID = RF_CRICKET;
			//RepeatOpCode = GO_TO_SLEEP;
			//}
			
			
			// if there is a message to send, do it here in a closed loop and assume 150 msecs per message
			if (SendMessage)
			{
				ONBOARD_LED_TOGGLE;
				msgCounter = CRICKET_SLEEP_MSECS/150;                                 //slave wakes up each second => msgCounter is OK
			
				RFInitTransmiter(CRICKET_RF_DEVICE);
				
				newMessage = true;
				for (j=0; j<msgCounter; j++)
					RFSendPacket(0, RepeatMsgID, RepeatOpCode, 0xffff, NULL, 0);
					
				RFIdle(CRICKET_RF_DEVICE);
			}
			

		//if (GpioPoll(PUSH_BUTTON_1))
		//{
			//RFInitTransmiter(0);
			//RFSendPacket(0,RF_MSG_SET_ONBOARDLED,0,0xffff,&sequence,sizeof(uint16_t));
			//sequence++;
			//RFInitReceiver(0);	// wait for reply
		//}
         #elif defined(CRICKET_SLAVE)
          // RFInitReceiver(CRICKET_RF_DEVICE);
		  
		     if ((slave_led_to_blink) && (ms1Ticks >= 5000))
			 {
				ms1Ticks = 0;					
				HIPOWER_LED_TOGGLE; 
				RepeatAction(RTC_SLAVE, 1, RTC_TICKS_SEC, RTC_TICKS_SEC);
				
				//HIPOWER_LED_ON;
				//delay_ms(200);
				//HIPOWER_LED_OFF;
			 }
         
			 decodedPacket.header.packetLen = 0;         
			 RFReceiveMode_Handler(0);
			 if (decodedPacket.header.packetLen != 0)
			 {
				 // got a packet, handle it
				 switch (decodedPacket.header.msgID)
				 {
					 case RF_CRICKET:
						 switch (decodedPacket.header.opCode)
						 {
							 case TURN_ON:
								 HIPOWER_LED_ON;
						     break;
         
							 case TURN_OFF:
								HIPOWER_LED_OFF;
							 break;
         
							 case WAKE_UP:
					  		    sleepmgr_lock_mode(SLEEPMGR_ACTIVE);      // definition of sleep mode at the next sleepmgr_enter_sleep()
                                
								ms1Ticks = 0;
								slave_led_to_blink = true;
								
								HIPOWER_LED_TOGGLE;
								RepeatAction(RTC_SLAVE, 1, RTC_TICKS_SEC, RTC_TICKS_SEC);
								
								//HIPOWER_LED_ON;								
								//delay_ms(200);
								//HIPOWER_LED_OFF;
							 break;
         
							 case GO_TO_SLEEP:		 
							    slave_led_to_blink = false;
								
								// definition of sleep mode at the next sleepmgr_enter_sleep()
								sleepmgr_lock_mode(SLEEPMGR_PSAVE);         
								HIPOWER_LED_OFF;
							 break;
         
							 default:
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

