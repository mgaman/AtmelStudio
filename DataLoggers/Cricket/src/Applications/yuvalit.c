/*
 * yuvalit.c
 *
 * Created: 12/17/2013 11:36:55 AM
 *  Author: pc
 */ 
#include <asf.h>
#include "Applications.h"
#include "RFHandler.h"
#include "EepromHandler.h"
#include "Utilities.h"
#include "pcmsghandler.h"

#ifdef YUVALIT_MASTER
    #include "ModemADC.h"
 // difference between theoretical value and acutal explained by DeltaV see AVR1300 document chapter 3.3
    #define LOW_VOLTAGE 3240  // threshold is 2.6V =>  (1.3/1.77) * 4096 => 3008  3280 is observed value
#endif

// global data referenced by others
volatile uint16_t sequence;
//extern uint16_t msgCounter;
// local data
RF_ENUM Retx = 0xff;
volatile int k = 0;

enum eYuvalMode {YENTERSLEEP,YSLEEPING,YENTERWAKE,YAWAKE};
volatile enum eYuvalMode YuvalMode;

volatile enum sleepmgr_mode sleepmode;

volatile uint32_t endlisten;
extern uint16_t waitUntilRespond;
extern bool newMessage;

extern uint16_t gvMsgIDCounterTX,gvMsgIDCounterRX;
extern uint16_t gvMsgRepCounter;

#ifdef YUVALIT_MASTER
	volatile bool lowvoltage = false;
	volatile bool startSampling = false;
#endif


extern fifo_desc_t pc_com_rx_fifo;
extern fifo_desc_t pc_com_tx_fifo;

extern uint8_t pc_com_rx_buffer[_MAX_RX_FIFO_BUFFER_LENGTH];
extern uint8_t pc_com_tx_buffer[_MAX_TX_FIFO_BUFFER_LENGTH];


RF_ENUM rfInitRecResp, rfSendPack;


//#define SLAVE_LEDS_FOR_DEBUG


//----------------------------
static void HardwareInit(void)
{
	// make sure all leds off
	ONBOARD_LED_OFF;	// not seen outside of the box
	HIPOWER_LED_OFF;	// master battery
	HIPOWER_LED_1_OFF;	// OK response
	//HIPOWER_LED_2_OFF;	// yuvalit battery
	
	#ifdef YUVALIT_MASTER
		ModemADCinit();
	#endif
}


//----------------------------
static void SoftwareInit(void)
{
	GpioPinClear();
	
	#ifdef YUVALIT_MASTER
		int j;
		GpioPollInit(WIFI_ON_BUTTON, PIN_DOWN);	// wifi on button
		GpioPollInit(WIFI_OFF_BUTTON,PIN_DOWN);	// wifi off button		
	
		RepeatActionInit();
		
	    OK_LED_OFF;
	 	SLAVE_BATTERY_LED_OFF;
		MASTER_BATTERY_LED_OFF;			
	
		RepeatAction(RTC_OK,2,RTC_TICKS_SEC,RTC_TICKS_SEC);                                        
	    RepeatAction(RTC_SPARE0,2,RTC_TICKS_SEC,RTC_TICKS_SEC);                               
	    RepeatAction(RTC_SPARE1,2,RTC_TICKS_SEC,RTC_TICKS_SEC);   
		
		RFInitReceiver(YUVALIT_RF_ID);          // for RF sleep must enter tx or rx before idle else it wont work
		delay_s(1);
		RFIdle(YUVALIT_RF_ID);
		
		lowvoltage = false;
	#endif

	#ifdef YUVALIT_SLAVE
		YuvalMode = YENTERSLEEP;// YENTERWAKE;
		GpioPollInit(YUVALIT_TRIGGER, PIN_CHANGED);	// from arduino to slave
		
	//	HIPOWER_LED_1_ON;
	//	delay_ms(500);
	//	HIPOWER_LED_1_OFF;
	
	//	HIPOWER_LED_2_ON;
	//	delay_ms(500);
	//	HIPOWER_LED_2_OFF;
		
		HIPOWER_LED_ON;
		delay_ms(500);
		HIPOWER_LED_OFF;
	#endif

	sequence = 0;
	gvMsgIDCounterTX = 0;
	gvMsgIDCounterRX = 0;
	sleepmode = SLEEPMGR_PSAVE;
}


//----------------
void yuvalit(void)
{
	bool messageToSend;
	uint32_t replyTime;
	
	//bool messageReceived = false;
	uint32_t last_wifi_ON;
	bool wifi_on_sent = false;
	
	bool goodAnswer = false;
	
	#ifdef YUVALIT_MASTER
		int j;
		uint32_t sample;
		#define NUM_SAMPLES 15
		uint8 adcCounts = 0;
	
	#elif defined(YUVALIT_SLAVE)
		uint8_t uartRepeats;
	#endif

	HardwareInit();
	SoftwareInit();	
	
	while (true)
	{
		int repeatCount;
		//volatile uint8_t rfvalue,j;
		//RF_ENUM rfcode;
		
		sleepmgr_init();
		sleepmgr_lock_mode(sleepmode);
		sleepmgr_enter_sleep();
		
		messageToSend = false;
		
		/* exit when RTC ticks, master checks push buttons for action, slave checks trigger pin
		 both master and slave work in different patterns as the master is always the initiator of wireless messages
		 slave only reacts
		 MASTER
				check if button pushed
				NO - nothing to do
				YES - prepare message
					enter RFxmit mode
					xmit message for 10 seconds
						message includes a decrementing counter needed by slave
					enter RFrcv mode
					listen for 2 seconds
					if message rcvd, process it
					enter RFidle mode
		SLAVE
				check if trigger raised
				NO - nothing to do
				YES - enter RFrcv mode
					- set sleepmode to IDLE
					- test for new message up to 10 seconds
					- if message arrives, forward by UART to Arduino -- repeat 10x at 200msec intervals
					- wait for Arduino reply
					- wait for remainder of 10 second period
					- enter RFxmit mode
					- send xmit back to master
					- set sleepmode to PSAVE
		*/	
		
		
   #ifdef YUVALIT_MASTER        
		if (!lowvoltage)                           // master battery test
		{
			sample = 0;
			for (j=0; j<NUM_SAMPLES; j++)
				sample += ModemADCinput();
				
			sample /= NUM_SAMPLES;
			if (!startSampling)
				startSampling = ++adcCounts > 30;	
				
			if (startSampling)
			{
				if (!lowvoltage && (sample < LOW_VOLTAGE))
				{
					lowvoltage = true;						

					#ifndef MASTER_LEDS_FOR_DEBUG
					    MASTER_BATTERY_LED_ON;	                                                              // 4 secs off, 1 sec on, repetitions = 0 means forever    
					    RepeatAction(RTC_SPARE1, RTC_FOREVER, 4*RTC_TICKS_SEC,RTC_TICKS_SEC);
					#endif
				}
			}
		}		
	
		if (GpioPoll(WIFI_ON_BUTTON))
		{
			last_wifi_ON = rtcTicks;
			wifi_on_sent = true;			
			messageToSend = true;
			RepeatMsgID = RF_MSG_YUVALIT_WIFI;
			RepeatOpCode = TURN_ON;			
		}
		
		else if (GpioPoll(WIFI_OFF_BUTTON))
		{
			bool do_work = false;
			if (wifi_on_sent)
			{
				if (rtcTicks - last_wifi_ON > ON_OFF_TIMEOUT) 
			    {
					do_work = true;
				}
			}
			else
			{
				do_work = true;
			}
			
			if (do_work)
			{		
				messageToSend = true;
				RepeatMsgID = RF_MSG_YUVALIT_WIFI;
				RepeatOpCode = TURN_OFF;
				OK_LED_OFF;				
			}
		}
		
		if (messageToSend)
		{
			#ifdef MASTER_LEDS_FOR_DEBUG
			   SLAVE_BATTERY_LED_OFF;
			   MASTER_BATTERY_LED_OFF;
			#endif			
			
			messageToSend = false;	

			RFInitTransmiter(YUVALIT_RF_ID);	
		  	   
			OK_LED_ON;
			repeatCount = 140;	                  // for baud=1200 => aprox 200 ms for message.                 
			newMessage = true;
			gvMsgRepCounter = repeatCount;
			
			for (; repeatCount>0; repeatCount--)
			{
				RFSendPacket(YUVALIT_RF_ID, RepeatMsgID, RepeatOpCode, 0xffff, NULL, 0);
				//uint8_t txPowVal = ReadRFRegister(YUVALIT_RF_ID, 109/* 0x6d*/);
			}				
		   // OK_LED_OFF;                                                                       // It will be ON until it receives message from Slave or the period of RF_Receive ends
			
			RF_ENUM rfResp = RFInitReceiver(YUVALIT_RF_ID);                                     // now listen for a response
			
			ONBOARD_LED_ON;			
						
			endlisten = ms1Ticks + 5000;
			decodedPacket.header.packetLen = 0;
			
		    goodAnswer = false;
			
			uint32_t k;
			for (k = 0; k < 45000; k++)
			{
				RFReceiveMode_Handler(YUVALIT_RF_ID);
				if (decodedPacket.header.packetLen != 0)                                        // answer received from Slave
				{
				 // got a packet, handle it
					switch (decodedPacket.header.msgID)
					{
						case RF_MSG_YUVALIT_WIFI:
							/* opcode is encoded thus
								bit 0 1 +ve acknowledge to masters message
									  0	-ve acknowledge to masters message
								bit 1 1 yuvalit battery low
								      0 yuvalit battery high
							*/
														
							if (decodedPacket.header.opCode & 1)
							{
								goodAnswer = true;
								
								OK_LED_ON;
								RepeatAction(RTC_OK, 35, RTC_TICKS_SEC, RTC_TICKS_SEC);						
							}
							else
							if (decodedPacket.header.opCode & 2)
							{
							    goodAnswer = true;
							   
							    OK_LED_ON;
							    RepeatAction(RTC_OK, 35, RTC_TICKS_SEC, RTC_TICKS_SEC);
								
							    #ifndef MASTER_LEDS_FOR_DEBUG			   
								   SLAVE_BATTERY_LED_ON;                                           
								   RepeatAction(RTC_SPARE0, 35, RTC_TICKS_SEC, RTC_TICKS_SEC);  
				    			#endif
							}
							else
							{
								#ifdef MASTER_LEDS_FOR_DEBUG
								   SLAVE_BATTERY_LED_ON;
								   MASTER_BATTERY_LED_OFF;
								#endif
							}							
						break;
						
						default:
							#ifdef MASTER_LEDS_FOR_DEBUG
							   SLAVE_BATTERY_LED_OFF;
							   MASTER_BATTERY_LED_ON;
							#endif
						break;
					}
				}
				
				if (goodAnswer)
				  break;
				  
				delay_ms(1);
			}
			
			ONBOARD_LED_OFF;
			RFIdle(YUVALIT_RF_ID);
			
			// turn off led if no answer received
		//	if (decodedPacket.header.packetLen == 0)
			if (!goodAnswer)			
			   OK_LED_OFF;
		}
		
		
   #elif defined(YUVALIT_SLAVE)
		if (GpioPoll(YUVALIT_TRIGGER))
		{
			if (gpio_pin_is_low(YUVALIT_TRIGGER))  // changed from high
			{
				YuvalMode = YENTERWAKE;
			}
			else
			{
				YuvalMode = YENTERSLEEP;
			}
		}
		
		switch (YuvalMode)
		{
			case YENTERSLEEP:
				RFIdle(YUVALIT_RF_ID);
				YuvalMode = YSLEEPING;
				sleepmode = SLEEPMGR_PSAVE;
				
				#ifdef SLAVE_LEDS_FOR_DEBUG
				   ONBOARD_LED_OFF;	
				#endif
				
				break;
				
			case YENTERWAKE:
				rfInitRecResp = RFInitReceiver(YUVALIT_RF_ID);
				if (rfInitRecResp != RF_OK)
				{
 					 WriteEEPROMByte(28*EEPROM_PAGE_SIZE, (uint8_t)rfInitRecResp);						
					 while (true)
					 {
					   HIPOWER_LED_TOGGLE;
					   delay_s(2);
					 }
				}
				
				YuvalMode = YAWAKE;
				sleepmode = SLEEPMGR_IDLE;
				
				#ifdef SLAVE_LEDS_FOR_DEBUG
				   ONBOARD_LED_ON;
				#endif	
				
				break;
				
			case YSLEEPING:
			case YAWAKE:
				break;
		}
		

		if (YuvalMode == YAWAKE)
		{
			decodedPacket.header.packetLen = 0;
			RFReceiveMode_Handler(YUVALIT_RF_ID);
			
		 // got a packet, handle it
			messageToSend = false;
			if (decodedPacket.header.packetLen != 0)
			{
				switch (decodedPacket.header.msgID)
				{
					case RF_MSG_YUVALIT_WIFI:
				     
						RFIdle(YUVALIT_RF_ID);						// save energy
						
						// forward message via uart
						// Ofir requested that this message be repeated up to 10x every 200msec in which case we 
						// cannot risk going back to sleep mode, so do a loop here
						
						replyTime = ms1Ticks + waitUntilRespond;

                        bool toON = true;
						messageToSend = false;
						uartRepeats = 500;						
									
						fifo_flush(&pc_com_rx_fifo);
						fifo_flush(&pc_com_tx_fifo);
						
						while (!messageToSend && uartRepeats-->0)
						{
							int j;
							for (j=0; j<5; j++)
							{
								LibPCPacketTransmit(0,0,decodedPacket.header.msgID,decodedPacket.header.opCode,NULL,0);			
								delay_ms(500);
								
								#ifdef SLAVE_LEDS_FOR_DEBUG
							        ONBOARD_LED_TOGGLE;
							    #endif								
							}				
					
							pcmessage.PacketLength = 0;
							LibHandleReceivedPCData(&pc_com_rx_fifo);
							if (pcmessage.PacketLength != 0)							
							{
								switch (pcmessage.MsgID)
								{
									case MSG_YUVALIT_STATE:
									   RepeatMsgID = RF_MSG_YUVALIT_WIFI;
									   RepeatOpCode = pcmessage.OpCode;
									   
								       messageToSend = true;
									   #ifdef SLAVE_LEDS_FOR_DEBUG
									 	  HIPOWER_LED_ON;
									   #endif	
									break;
									
									default:
									break;
								}
						
						
								if (messageToSend)
								{
						 		 // hang around until time to send
									#ifdef SLAVE_LEDS_FOR_DEBUG
									   ONBOARD_LED_ON;
									#endif
									while (ms1Ticks < replyTime)
									{
									
									} 
									#ifdef SLAVE_LEDS_FOR_DEBUG
									   ONBOARD_LED_OFF;
									#endif															
						
									RFInitTransmiter(YUVALIT_RF_ID);
								
									repeatCount = 100;	                        // 230msec each packet
									newMessage = true;
									gvMsgRepCounter = repeatCount;
									ONBOARD_LED_OFF;
									for (; repeatCount>0; repeatCount--)
									{
							  			rfSendPack = RFSendPacket(0,RepeatMsgID,RepeatOpCode,0xffff,NULL,0);	
										if (rfSendPack == RF_OK)  	
										{
											 #ifdef SLAVE_LEDS_FOR_DEBUG
												 if  (toON)
												 {
													 ONBOARD_LED_ON;
													 toON = false;
												 }
												 else
												 {
													 ONBOARD_LED_OFF;
													 toON = true;
												 }
											 #endif												
										}							  
								        else
										{
											ONBOARD_LED_OFF;							
										}
																			
										delay_ms(50);
									}
									
									break; 								
								}
							}
		
							delay_ms(500);
							
							if (messageToSend)
							{
							   memset(&pcmessage, 0, sizeof(pcmessage));
							   break;
							}
						}
						
						RFInitReceiver(YUVALIT_RF_ID);
						
						#ifdef SLAVE_LEDS_FOR_DEBUG
					    	ONBOARD_LED_OFF;
							HIPOWER_LED_OFF;
						#endif
					break;
					
					default:
					break;
				}
			}
		}
   #endif
	}
}

