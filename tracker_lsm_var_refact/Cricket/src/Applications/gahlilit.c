/*
 * gahlilit.c
 *
 * Created: 2/20/2014 9:34:27 AM
 *  Author: User
 */ 



#include <asf.h>
#include "Utilities.h"
#include "RFHandler.h"
#include "pcdatahandler.h"
#include "ModemPWM.h"
#include "EepromHandler.h"
#include "pcmsghandler.h"

#ifdef GAHLILIT_MASTER
    #include "ModemADC.h"
    #define LOW_VOLTAGE 3240  // threshold is 2.6V =>  (1.3/1.77) * 4096 => 3008  3280 is observed value
#endif



static void HardwareInit(void);
static void SoftwareInit(void);
static void LoadGahliParams(void);


extern uint8_t currentEpromCode;

extern uint8_t activationCode_1;
extern uint8_t activationCode_2;
extern uint8_t activationCode_3;
extern uint8_t activationCode_4;

bool readyToLearn = false;
extern uint16_t msgCounter;
extern bool newMessage;
extern uint16_t gvMsgRepCounter;

extern uint8_t GahliliotParams[MAX_GAHLI_FOR_SHALAT][GAHLI_PARAMS];
extern uint8_t SlaveGahliliotParams[GAHLI_PARAMS];

extern uint16_t gvMsgIDCounterTX,gvMsgIDCounterRX;
extern uint16_t gvMsgRepCounter;

extern fifo_desc_t pc_com_rx_fifo;
extern fifo_desc_t pc_d_com_rx_fifo;

uint8_t oneGahlilitParams[GAHLI_PARAMS];

enum eGahlilMode {GENTERSLEEP,GSLEEPING,GENTERWAKE,GAWAKE};
volatile enum eGahlilMode GahlilMode;

volatile enum sleepmgr_mode sleepmode;

extern uint8 buttonReleased;
extern uint8 buttonIntFlag;

bool usartTest = false;

uint8_t groupNr;
uint8_t parSet;
uint8_t wOne;


extern uint8_t activCodeParam;
extern uint8_t blinkNrParam;
extern uint8_t ledOnTimeParam;
extern uint8_t ledOffTimeParam;
extern uint8_t listenIntervParam;

uint8_t ParamsForSlave[GAHLI_PARAMS-2];


//#define FIRST_TEST
//#define CLEAN_EPROM
//#define BLINKS_FOR_TEST


#define SLAVE_LEDS_FOR_DEBUG
//#define GRISHA_IO_TEST
//#define CONSUMPTION_TEST


RF_ENUM rfFuncResp;

#ifdef GAHLILIT_MASTER
	volatile bool lowvoltage = false;
	volatile bool startSampling = false;
#endif


//------------------------
static void HardwareInit()
{
	#ifdef GAHLILIT_MASTER	
		ONBOARD_LED_OFF;
		
		OK_LED_ON;	
		delay_s(1);
		OK_LED_OFF;		
		
		//RFInitTransmiter(ONE_CHIP_RF_ID);		
	#elif defined(GAHLILIT_SLAVE)	
		HIPOWER_LED_ON;
		delay_s(1);
		HIPOWER_LED_OFF;
		
		ONBOARD_LED_ON;
		delay_s(1);
		ONBOARD_LED_OFF;
		
		GAHLILIT_IR_LED_1_OFF;
		GAHLILIT_IR_LED_2_OFF;
		
	//	GAHLILIT_IR_LED_1_ON;
	//	delay_s(2);
	//	GAHLILIT_IR_LED_1_OFF;
		
	//	GAHLILIT_IR_LED_2_ON;
	//	delay_s(2);
	//	GAHLILIT_IR_LED_2_OFF;			
	#endif
}



//------------------------
static void SoftwareInit()
{
	RepeatMsgID = RF_MSG_GAHLILIT;
	
	GpioPinClear();	
	
	RepeatActionInit();	
	
	#ifdef GAHLILIT_MASTER	
	    GpioPollInit(GROUP_1_BUTTON, PIN_DOWN);
	    GpioPollInit(GROUP_2_BUTTON, PIN_DOWN);
	    GpioPollInit(GROUP_3_BUTTON, PIN_DOWN);
	    GpioPollInit(GROUP_4_BUTTON, PIN_DOWN);	
		
		RFInitReceiver(ONE_CHIP_RF_ID);         
		delay_s(1);
		RFIdle(ONE_CHIP_RF_ID);		
		
		sleepmode = SLEEPMGR_PSAVE;
	
		#ifdef CLEAN_EPROM
		for(i=0; i<=MAX_GAHLI_FOR_SHALAT; i++)
		{
			for (j=0; j<GAHLI_PARAMS; j++)
			{
				wOne = wrParams[i][j];
				WriteEEPROMByte(EEPROM_PARAMETER_PAGE*EEPROM_PAGE_SIZE + i*GROUP_OFFSET + j, 0xff);
			}
		}
		#endif		
		   
	    #ifdef FIRST_TEST
			for(i=0; i<MAX_GAHLI_FOR_SHALAT; i++)			
			{
				for (j=0; j<GAHLI_PARAMS; j++)
				{
					wOne = wrParams[i][j];			
					WriteEEPROMByte(EEPROM_PARAMETER_PAGE*EEPROM_PAGE_SIZE + i*GROUP_OFFSET + j, wOne);
				}
			}		
	    #endif	
		
		for(i=0; i<MAX_GAHLI_FOR_SHALAT; i++)
		{
			for (j=0; j<GAHLI_PARAMS; j++)
			{
				ReadEEPROMByte(EEPROM_PARAMETER_PAGE*EEPROM_PAGE_SIZE + i*GROUP_OFFSET + j, &tmp);
				GahliliotParams[i][j] = tmp;
			}
		}
	#elif defined(GAHLILIT_SLAVE)
	    GahlilMode = GENTERSLEEP; 
		sleepmode = SLEEPMGR_PSAVE;                        
	    GpioPollInit(GAHLILIT_LEARN_BUTTON, PIN_DOWN);			
		LoadGahliParams();			
	#endif	

	gvMsgIDCounterTX = 0;
	gvMsgIDCounterRX = 0;
}


//---------------------------
static void LoadGahliParams()
{
	uint8_t j = 0;
	uint8_t tmp = 0;
	
	for(j=0; j<GAHLI_PARAMS; j++)
	{
		ReadEEPROMByte(EEPROM_PARAMETER_PAGE*EEPROM_PAGE_SIZE + j, &tmp);
		SlaveGahliliotParams[j] = tmp;
	}
	
	activCodeParam    = SlaveGahliliotParams[0];
	blinkNrParam      = SlaveGahliliotParams[1];
	ledOnTimeParam    = SlaveGahliliotParams[2];
	ledOffTimeParam   = SlaveGahliliotParams[3];
	listenIntervParam = SlaveGahliliotParams[4];
	
	if((blinkNrParam & 1) != 0)
	{
		blinkNrParam++;
	}
	
	blinkNrParam = blinkNrParam*2;
}


//------------------------------------------------
void interruptProtectedSleepAndWakeCountersReset()
{
	firefly_slave_sleep_count = 0;
	if (firefly_slave_sleep_count != 0) 
	  firefly_slave_sleep_count = 0;	
	
	firefly_slave_awake_count = 0;
	if  (firefly_slave_awake_count != 0)
   	  firefly_slave_awake_count = 0;
}

//------------
void gahlilit()
{
	uint8_t j;
	
    #ifdef GAHLILIT_MASTER
		int jj;
		uint32_t sample;
		#define NUM_SAMPLES 10
		uint8 adcCounts = 0;
	#endif
	
	HardwareInit();
	SoftwareInit();	

	uint8_t local_sleep_count = 0;
	uint8_t local_awake_count = 0;	
	
	firefly_blinking_flag = false;	
	firefly_blink_timeout_count = 0;
	
	uint8_t firefly_blinking_timeout = 0;
	
	bool ready_for_new_command = true;
	uint8_t local_blinks_count = 0;
	
	while (true)
	{	
       #ifdef GAHLILIT_MASTER	
	      if (!firefly_master_is_waiting_for_pc)
		  {
			 sleepmgr_init();
			 sleepmgr_lock_mode(sleepmode);
			 sleepmgr_enter_sleep();
		  }  
		  
		  messageToSend = false;
	  		  
	      if (GpioPoll(GROUP_1_BUTTON))
          {
	          messageToSend = true;
	          RepeatOpCode = TURN_ON;
			  indx = 0;			
          }
          else 
		  if (GpioPoll(GROUP_2_BUTTON))
          {
			  messageToSend = true;
			  RepeatOpCode = TURN_ON;
			  indx = 1;
		  }
		  else
		  if (GpioPoll(GROUP_3_BUTTON))
		  {
			  messageToSend = true;
			  RepeatOpCode = TURN_ON;
			  indx = 2;
		  }
		  else
		  if (GpioPoll(GROUP_4_BUTTON))
		  {
			 messageToSend = true;
			 RepeatOpCode = TURN_ON;
			 indx = 3;
    	  }
		
		//----------------------------------------------------
      	  if (messageToSend)
	      {
    		 messageToSend = false;
			 
			 if (firefly_master_is_waiting_for_pc)
			   firefly_master_is_waiting_for_pc = false;
			 
			 RFInitTransmiter(ONE_CHIP_RF_ID);	

    		 uint8_t activaCode = GahliliotParams[indx][0];
			 uint8_t fBlinksNum = GahliliotParams[indx][1];
			  
			 for (j=2; j<GAHLI_PARAMS; j++)
			    ParamsForSlave[j-2] = GahliliotParams[indx][j];			 
		    
		     OK_LED_ON;
			 repeatCount = 43;	                  
		     newMessage = true;
		     gvMsgRepCounter = repeatCount;						    				
		     for (; repeatCount>0; repeatCount--)
			 {
		        RFSendPacket(ONE_CHIP_RF_ID, RepeatMsgID, activaCode, fBlinksNum, &ParamsForSlave, sizeof(ParamsForSlave));
			 }
		     OK_LED_OFF;
			 
			 RFIdle(ONE_CHIP_RF_ID);
		  }
		  
		  
		//-------------------------------------------------  
		  pcmessage.PacketLength = 0;
		  LibHandleReceivedPCData(&pc_com_rx_fifo);
		  if (pcmessage.PacketLength != 0)
		  {
			  uint8_t GroupAndParSet = pcmessage.SourceID;
			  switch (GroupAndParSet)
			  {
				 case 11 : groupNr = 1; parSet = 1; break;
				 case 12 : groupNr = 1; parSet = 2; break;
                 case 21  :groupNr = 2; parSet = 1; break;
				 case 22 : groupNr = 2; parSet = 2; break;
				 case 31 : groupNr = 3; parSet = 1; break;
				 case 32 : groupNr = 3; parSet = 2; break;
				 case 41 : groupNr = 4; parSet = 1; break;
				 case 42 : groupNr = 4; parSet = 2; break;
				 
				 default : groupNr = 1; parSet = 1; break;
			  }
			  
			  if (parSet == 1) 
			  {				  
				  activCodeParam    = pcmessage.DestID;
				  blinkNrParam      = pcmessage.MsgID;
				  ledOnTimeParam    = pcmessage.OpCode;
			  }
			  
			  if (parSet == 2)
			  {
				  ledOffTimeParam   = pcmessage.DestID;
				  listenIntervParam = pcmessage.MsgID;		 
				  
				  oneGahlilitParams[0] = activCodeParam;
				  oneGahlilitParams[1] = blinkNrParam;
				  oneGahlilitParams[2] = ledOnTimeParam;
				  oneGahlilitParams[3] = ledOffTimeParam;
				  oneGahlilitParams[4] = 255;               //listenIntervParam;				  
			  
				  for (j=0; j<GAHLI_PARAMS; j++)
				  {
					  wOne = oneGahlilitParams[j];
					  WriteEEPROMByte(EEPROM_PARAMETER_PAGE*EEPROM_PAGE_SIZE + (groupNr-1)*GROUP_OFFSET + j, wOne);  
				  }		
				  
				  for(i=0; i<MAX_GAHLI_FOR_SHALAT; i++)
				  {
					  for (j=0; j<GAHLI_PARAMS; j++)
					  {
						  ReadEEPROMByte(EEPROM_PARAMETER_PAGE*EEPROM_PAGE_SIZE + i*GROUP_OFFSET + j, &tmp);
						  GahliliotParams[i][j] = tmp;
					  }
				  }		
				  
				  usart_putchar(USART_SERIAL_PC, 252);		  
	  
			      OK_LED_ON;
			      delay_ms(500);
			      OK_LED_OFF;			   
			  }
		  }
	  
     #elif defined(GAHLILIT_SLAVE)	  
	     sleepmgr_init();
	     sleepmgr_lock_mode(sleepmode);
	     sleepmgr_enter_sleep();
	 
	     readyToLearn = true;
	     if (ioport_pin_is_low(GAHLILIT_LEARN_BUTTON))
		 {
	  	    readyToLearn = true;
			  
			 #ifdef GRISHA_IO_TEST
			     RepeatAction(RTC_HIPOWER, 6, 1*RTC_TICKS_SEC, 1*RTC_TICKS_SEC);	
				
				 RepeatAction(RTC_IR_1, 6, 1*RTC_TICKS_SEC, 1*RTC_TICKS_SEC);
				 RepeatAction(RTC_IR_2, 6, 1*RTC_TICKS_SEC, 1*RTC_TICKS_SEC);					
			 #endif  
			 
			 
			 #ifdef CONSUMPTION_TEST
			    GAHLILIT_IR_LED_1_OFF;
				GAHLILIT_IR_LED_2_OFF;
				
				while(true)
				{
				    GAHLILIT_IR_LED_1_ON;
				    delay_s(5);
				    GAHLILIT_IR_LED_2_ON;
					delay_s(5);
					GAHLILIT_IR_LED_1_OFF;
					GAHLILIT_IR_LED_2_OFF;
					
					delay_s(5);
				}
			 #endif
		 }
	     else
		    readyToLearn = false;	 
	 
	  // cpu_irq_disable();	 	   	
		 local_sleep_count = firefly_slave_sleep_count;                                         //  I do not want to disable interrupts => double read of the volatile shared data
		 if (local_sleep_count != firefly_slave_sleep_count)
			local_sleep_count = firefly_slave_sleep_count;
		
		 local_awake_count = firefly_slave_awake_count;
		 if (local_awake_count != firefly_slave_awake_count)
			local_awake_count = firefly_slave_awake_count; 
	  // cpu_irq_enable();	 
	    
	   
		 if (local_sleep_count >= MAX_GAHLIL_SLEEP_CNT)
		 {
			 firefly_slave_is_sleeping = false;
			 interruptProtectedSleepAndWakeCountersReset();			 
			 GahlilMode = GENTERWAKE;			 
		 }
		 
		 if (local_awake_count >= MAX_GAHLIL_AWAKE_CNT)
		 {
			 firefly_slave_is_sleeping = true;			 
			 interruptProtectedSleepAndWakeCountersReset();		   
			 GahlilMode = GENTERSLEEP;
			 
			 #ifdef SLAVE_LEDS_FOR_DEBUG
			     ONBOARD_LED_OFF;
			 #endif
		 }
 
	
		 switch (GahlilMode)
		 {
			 case GENTERSLEEP:
				 RFIdle(ONE_CHIP_RF_ID);							 
				 GahlilMode = GSLEEPING;
				 sleepmode = SLEEPMGR_PSAVE;	
			 break;
		 
			 case GENTERWAKE:			 
			     if (firefly_blinking_flag)
				 {
			        local_blinks_count = firefly_blink_timeout_count;
			        if (local_blinks_count != firefly_blink_timeout_count)
			          local_blinks_count = firefly_blink_timeout_count;	   
				   
				    ready_for_new_command = false;
					if (local_blinks_count >= firefly_blinking_timeout)
				    {
						firefly_blinking_flag = false;
						ready_for_new_command = true;
						
						firefly_blink_timeout_count = 0;
						firefly_blinking_timeout    = 0;     
					}
					else
					{
						ONBOARD_LED_OFF;
					}
				 }
				 else
				 {
					ready_for_new_command = true; 					
				 }
			
			     if (ready_for_new_command)
				 {			
					GAHLILIT_IR_LED_1_OFF;
					GAHLILIT_IR_LED_2_OFF;
					
					RF_ENUM res =  RFInitReceiver(ONE_CHIP_RF_ID);
					if (res == RF_OK)
					{
						#ifdef SLAVE_LEDS_FOR_DEBUG
						    ONBOARD_LED_ON;
						#endif
					}
					GahlilMode = GAWAKE;
					sleepmode = SLEEPMGR_IDLE; 									
				 }		 
			 break;
		 
			 case GSLEEPING:
			 case GAWAKE:
			 break;
		 }	 

		 if (GahlilMode == GAWAKE)
		 { 			
			decodedPacket.header.packetLen = 0;
			RFReceiveMode_Handler(ONE_CHIP_RF_ID);
			if (decodedPacket.header.packetLen != 0)
			{
				ONBOARD_LED_OFF;
				switch (decodedPacket.header.msgID)
				{
					case RF_MSG_GAHLILIT:
					   RFIdle(ONE_CHIP_RF_ID);
					  
					   if (readyToLearn)
					   {			   
						   oneGahlilitParams[0] = decodedPacket.header.opCode;
						   oneGahlilitParams[1] = decodedPacket.header.targetMAC;
						   oneGahlilitParams[2] = decodedPacket.packet_data.data[0];
						   oneGahlilitParams[3] = decodedPacket.packet_data.data[1];
						   oneGahlilitParams[4] = decodedPacket.packet_data.data[2];
					   
						   for (j=0; j<GAHLI_PARAMS; j++)
						   {
							   wOne = oneGahlilitParams[j];
							   WriteEEPROMByte(EEPROM_PARAMETER_PAGE*EEPROM_PAGE_SIZE + j, wOne); 
						   }		   	   
	
						   LoadGahliParams();							   
						   RepeatAction(RTC_HIPOWER, 4, RTC_TICKS_SEC, RTC_TICKS_SEC); 
					   }
					   else
					   {
						   if (decodedPacket.header.opCode == SlaveGahliliotParams[0])
						   {
							   firefly_blinking_flag = true;
							   ready_for_new_command = false;
							   
							   firefly_blink_timeout_count = 0;						
						
							   #ifdef BLINKS_FOR_TEST
							       RepeatAction(RTC_HIPOWER, blinkNrParam, ledOnTimeParam*RTC_TICKS_SEC, ledOffTimeParam*RTC_TICKS_SEC);
							   #else
								   RepeatAction(RTC_IR_1, blinkNrParam, ledOnTimeParam*RTC_TICKS_SEC, ledOffTimeParam*RTC_TICKS_SEC);
								   RepeatAction(RTC_IR_2, blinkNrParam, ledOnTimeParam*RTC_TICKS_SEC, ledOffTimeParam*RTC_TICKS_SEC);
							   #endif	
							   
							 //  firefly_blinking_timeout = ((blinkNrParam/2)-1)*(ledOnTimeParam + ledOffTimeParam);	
							 firefly_blinking_timeout = (blinkNrParam/2)*(ledOnTimeParam + ledOffTimeParam);							   
						   }
					   }

					   firefly_slave_is_sleeping = true;
					   interruptProtectedSleepAndWakeCountersReset();
					   sleepmode = SLEEPMGR_PSAVE;
					   
					break;			 
				}
   		     }
		 }
      #endif		
	}
}



// RepeatAction(RTC_IR_1, 60, 2*RTC_TICKS_SEC, 4*RTC_TICKS_SEC);
// RepeatAction(RTC_IR_2, 60, 2*RTC_TICKS_SEC, 4*RTC_TICKS_SEC);	
// firefly_blinking_timeout = 30*6



//if (buttonIntFlag)
//{
//buttonIntFlag = false;
//if (buttonReleased)
//{
//readyToLearn = false;
//ONBOARD_LED_OFF;
//}
//else
//{
//readyToLearn = true;
//ONBOARD_LED_ON;
//}
//}


//uint8_t wrParams[MAX_GAHLI_FOR_SHALAT][GAHLI_PARAMS] = {
//{0x0A, 1, 1, 1, 1},
//{0x0B, 2, 2, 2, 2},
//{0x0C, 3, 3, 3, 3},
//{0x0C, 4, 4, 4, 4},
//};
//
//uint8_t rdParams[MAX_GAHLI_FOR_SHALAT][MAX_GAHLI_FOR_SHALAT];