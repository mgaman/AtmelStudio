/*
 * Application.c
 *
 * Created: 9/29/2013 10:20:09 AM
 *  Author: pc
 */ 
#include <string.h>
#include <asf.h>
#include "EEPROM.h"
#include "Application.h"
#include "Parser.h"
// localdata
uint8_t sourcenodeid;
struct sMessage mymessage;
uint16_t SendMessageCount,LocalMessageCount;
uint8_t blinkcount;
// Firefly may be sending messages to 3 groups simultaneously so we need to keep info
// on all 3
uint8_t repeatMessageLength[3],repeatcount[3];
uint32_t repeatAlarmTime[3];
bool repeatBusy[3];
struct sMessage FireflyMessage[3];
enum eMessageID timerReason;
bool OnBoardLedState;
// prototypes
void LedSet(enum eLed,bool state);
#ifdef USE_1K_TICK
void AppTimerTick(void);
volatile uint32_t tickcount,tickAlarm;
volatile bool appTimeBusy;
#endif
#ifdef USE_RTC
void AppTimerCallback(uint32_t time);
#else
void AppTimerCallback(void);
#endif
void FireflyInitiateSend(uint8_t group);
void DemoMessageSend(bool ledon);

/*
	API main to application
*/
void appInit()
{
	int j;
	switch (eeprom.project)
	{
		case Demo:
			break;
		case Firefly:
			SendMessageCount = 0;
			LocalMessageCount = 0;
			break;
		case Yuvalit:
			break;
	}
	timerReason = NULL_MESSAGE;
	for (j=0;j<3;j++)
	{
		repeatBusy [j]= false;
		repeatAlarmTime[j] = UINT32_MAX;
	}
#ifdef USE_RTC
	rtc_set_callback(AppTimerCallback);
#elif defined(USE_1K_TICK) 
	tickcount = 0;
	appTimeBusy = false;
#endif
}
/*
	Called every time the apptimer expires
	Further action depends on the reason that the appTimer was set
*/
void appTimerExpired(uint32_t when)
{
	int length,index;
	switch(timerReason)
	{
		case GPIO_START_TOGGLE:
			if (--blinkcount)
			{
				appTimerStart(OnBoardLedState ? eeprom.FireflyBlinkOffPeriod : eeprom.FireflyBlinkOnPeriod);
				OnBoardLedState = !OnBoardLedState;
				LedSet(ON_BOARD,OnBoardLedState);					
			}
			else
			{
				appTimeBusy = false;
				timerReason = NULL_MESSAGE;	// stop further calls
			}
			break;
		case START_ARDUINO:
		// no uart input was received so send a nak message
			LedSet(HIGH_POWER,false);
			length = shortMessage(sourcenodeid,ACKMSG,NAK);
			msgTx(UART,(uint8_t *)&mymessage,length);
			appTimeBusy = false;
			timerReason = NULL_MESSAGE;	// stop further calls
			break;
		case MSG_SEND_REPEAT:
			// the problem here is to keep track of which timer expired for which stream
			index = when == repeatAlarmTime[0] ? 0 :(when == repeatAlarmTime[1] ? 1 : 2);
			if (--repeatcount[index] > 0)
			{
				msgTx(UART,(uint8_t*)&FireflyMessage[index],repeatMessageLength[index]);
				appTimerStart(eeprom.MessageRepeatDelay);
				repeatAlarmTime[index] = tickAlarm;
			}
			else
				repeatBusy[index] = false;
			break;
		default:
			length = 0;	// for breakpoint
			break;
	}
}
/*
	GPIO event filtered first by pin number then application
	Could do the other way round but whats the difference
*/
void gpioEvent(ioport_pin_t gpioNumber,bool value)
{
	int length;
	switch (gpioNumber)
	{
		case PUSH_BUTTON_1:
			switch (eeprom.project)
			{
				case Demo:
					if (!value)
					{
						// button down, ignore button up
						// create a led off message and send it via wireless
						DemoMessageSend(false);
					}
					break;
				case Firefly:
					if (!value && !eeprom.IamRemote)	// remote doesnt send messages
						FireflyInitiateSend(GROUP_1);	
					break;				
				case Yuvalit:
					if (!value)
					{
						// button down, ignore button up
						// create a start arduino message and send it via wireless
						// add message counter for security
						++SendMessageCount;
						length = longMessage(Broadcast,START_ARDUINO,DONT_CARE,sizeof(uint16_t),(uint8_t *)&SendMessageCount);
						msgTx(UART,(uint8_t*)&mymessage,length);
					}
					break;
			}
			break;
		case PUSH_BUTTON_2:
			switch (eeprom.project)
			{
				case Demo:
					if (!value)
					{
						// button down, ignore button up
						// create a led on message and send it via wireless
						DemoMessageSend(true);
					}
					break;	
				case Firefly:
					if (!value && !eeprom.IamRemote)
						FireflyInitiateSend(GROUP_2);
					break;
			}
			break;
		case PUSH_BUTTON_3:
			switch(eeprom.project)
			{
				case Firefly:
					if (!value && !eeprom.IamRemote)
						FireflyInitiateSend(GROUP_3);
					break;
				default:
					break;
			}
			break;
		default:
			length++; // for breakpoint
			break;
	}
}
void msgRx(enum eChannel channel,struct sMessage *msg)
{
	int length;
	uint8_t bbuf[30];
	// if we get this far checksum is ok
	// check that message is for me
	sourcenodeid = msg->SourceID;
	// if a message came from outside source may need to turn off application timer
	if (timerReason == START_ARDUINO)
		appTimerStop();
	if (msg->DestinationID == Broadcast || msg->DestinationID == eeprom.NodeID)
	{
		// if message from PC this is for all projects
		if (sourcenodeid == PCConfig)
		{
			switch (msg->MessageId)
			{
				case EEPROM_GET:
					if (msg->dataLength == 1)	// sanity check
					{
						ReadEEPROM(EEPROM_PARAMETER_PAGE,msg->OpCode,bbuf,msg->data[0]);
						length = longMessage(PCConfig,EEPROM_READ,msg->OpCode,msg->data[0],bbuf);
						msgTx(UART,(uint8_t*)&mymessage,length);
					}
					break;
				case EEPROM_PUT:
					if (msg->dataLength > 0)
						WriteEEPROM(EEPROM_PARAMETER_PAGE,msg->OpCode,msg->data,msg->dataLength);
					break;
			}
		}
		else switch(eeprom.project)
		{
			case Demo:
				switch (msg->MessageId)
				{
					case GPIO:
						switch (msg->OpCode)
						{
							case GPIO_ON_WITH_ACK:
							case GPIO_OFF_WITH_ACK:
								if (msg->OpCode == GPIO_ON_WITH_ACK)
									LedSet(ON_BOARD,true);
								else
									LedSet(ON_BOARD,false);
								length = shortMessage(sourcenodeid,ACKMSG,msg->OpCode == GPIO_ON_WITH_ACK ? ON_ACK : OFF_ACK);
								msgTx(UART,(uint8_t*)&mymessage,length);
								break;
						}
						break;
					case ACKMSG:
						switch (msg->OpCode)
						{
							case ON_ACK:
								LedSet(ON_BOARD,true);
								break;
							case OFF_ACK:
								LedSet(ON_BOARD,false);
								break;
						}
						break;
				}
				break;
			case Firefly:
				if (gpio_pin_is_low(PUSH_BUTTON_1))
				{
					// setting group number if button is down
					eeprom.FireflyGroup = msg->OpCode;
					WriteEEPROM(EEPROM_PARAMETER_PAGE,offsetof(struct sEEProm,FireflyGroup),(uint8_t*)&eeprom.FireflyGroup,1);
				}
				else if (msg->OpCode == eeprom.FireflyGroup)
				{
					// only react to a message for my group
					switch (msg->MessageId)
					{
						case GPIO_START_TOGGLE:
							// check that message counter had increased
							if (msg->dataLength == sizeof(uint16_t))
							{
								uint16_t temp = msg->data[0] + (msg->data[1]<<8);
#ifdef SKIP_CHECKS
								if (true)
#else
								if (temp > LocalMessageCount)
#endif
								{
									LocalMessageCount = temp;
									blinkcount = eeprom.FireflyBlinkCount*2; // count on/off separately
									timerReason = msg->MessageId;
									OnBoardLedState = true;
									LedSet(ON_BOARD_LED,OnBoardLedState);
									appTimerStart(eeprom.FireflyBlinkOffPeriod);
								}
							}
							break;
					}					
				}
				break;
			case Yuvalit:
				switch (msg->MessageId)
				{
					case START_ARDUINO:
						// check that message counter had increased
						if (msg->dataLength == sizeof(uint16_t))
						{
							uint16_t temp = msg->data[0] + (msg->data[1]<<8);
#ifdef SKIP_CHECKS
							if (true)
#else
							if (temp > LocalMessageCount)
#endif
							{
								LocalMessageCount = temp;
								LedSet(HIGH_POWER,true);
								timerReason = msg->MessageId;
								appTimerStart(eeprom.TxTimeout);
							}
						}
						break;					
				}
				break;
			default:
				break;
		}
	}
};
void msgSent(enum eChannel c)
{
	if (eeprom.project == Yuvalit && c == WIRELESS)
		appTimerStop();	
};

/*
	API application to main
*/
#ifdef USE_RTC
void AppTimerCallback(uint32_t time)
#else
void AppTimerCallback(void)
#endif
{
#ifdef USE_ONESHOT_TC
	tc_set_overflow_interrupt_level(APPLICATION_TIMER,TC_INT_LVL_OFF);
	tc_set_overflow_interrupt_callback(APPLICATION_TIMER,NULL);	// cancel callback
#endif
#if defined(USE_RTC) || defined (USE_ONESHOT_TC)
	appTimerExpired();
#endif
}

void appTimerStart(unsigned sec)
{
#ifdef USE_ONESHOT_TC
	volatile uint32_t periphrate; 
	volatile uint32_t factor;
	tc_write_clock_source(APPLICATION_TIMER,TC_CLKSEL_DIV1024_gc);
	periphrate = tc_get_resolution(APPLICATION_TIMER);
	tc_set_overflow_interrupt_level(APPLICATION_TIMER,TC_INT_LVL_LO);
	tc_set_overflow_interrupt_callback(APPLICATION_TIMER,AppTimerCallback);
	factor = (periphrate * sec) / 1000; 
	// factor must be < 65K
	tc_write_period(APPLICATION_TIMER,factor);  // tune this number for best result
#elif defined(USE_RTC)
	rtc_set_alarm_relative(sec);
#elif defined(USE_1K_TICK)
	appTimeBusy = true;
	tickAlarm = tickcount + sec;
#endif
}

void appTimerStop()
{
#ifdef USE_ONESHOT_TC
	tc_set_overflow_interrupt_callback(APPLICATION_TIMER,NULL);	// cancel callback
	tc_set_overflow_interrupt_level(APPLICATION_TIMER,TC_INT_LVL_OFF);
#elif defined(USE_1K_TICK)
	appTimeBusy = false;
#endif
}

void msgTx(enum eChannel channel,uint8_t *msg,int length)
{
	int j;
	uint8_t *cp = (uint8_t*)msg;
	switch (channel)
	{
		case UART:
			for (j=0;j<length;j++)
				usart_putchar(MMI_UART,*cp++);
			break;
		default:
			break;
	}
};
/*
	Utility functions
	checksum is simple sum of all bytes
*/
uint8_t CheckSum(uint8_t *buff,int length)
{
	volatile uint8_t cs = 0;
	int j;
	for (j=0;j<length;j++)
		cs += buff[j];
	return cs;
}

int shortMessage(uint8_t dest,enum eMessageID msg,enum eOpCode opcode)
{
	mymessage.sync1 = SYNC_CHAR;
	mymessage.sync2 = SYNC_CHAR;
	mymessage.packetLength = 6;
	mymessage.SourceID = eeprom.NodeID;
	mymessage.DestinationID = dest;
	mymessage.MessageId = msg;
	mymessage.OpCode = opcode;
	mymessage.frameNumber = 0;
	mymessage.dataLength = 0;
	// checksum simple addition of bytes from packet length
	*(mymessage.data + mymessage.dataLength) = CheckSum((uint8_t *)&mymessage.packetLength,mymessage.packetLength+1);
	return 10;	// basic message 7 + 2 sync + cs
};
int longMessage(uint8_t dest,enum eMessageID msg,enum eOpCode opcode,unsigned datalength,uint8_t *data)
{
	mymessage.sync1 = SYNC_CHAR;
	mymessage.sync2 = SYNC_CHAR;
	mymessage.packetLength = 6 + datalength;
	mymessage.SourceID = eeprom.NodeID;
	mymessage.DestinationID = dest;
	mymessage.MessageId = msg;
	mymessage.OpCode = opcode;
	mymessage.frameNumber = 0;
	mymessage.dataLength = datalength;
	memcpy(mymessage.data,data,datalength);
	*(mymessage.data + mymessage.dataLength) = CheckSum((uint8_t *)&mymessage.packetLength,mymessage.packetLength+1);
	return 10 + datalength;
};
void LedSet(enum eLed led,bool state)
{
	switch (led)
	{
		case ON_BOARD:
			if (state)
				gpio_set_pin_low(ON_BOARD_LED);
			else
				gpio_set_pin_high(ON_BOARD_LED);
				break;
		case HIGH_POWER:
			if (state)
				gpio_set_pin_low(HIGH_POWER_LED);
			else
				gpio_set_pin_high(HIGH_POWER_LED);
			break;
	}
}
#ifdef USE_1K_TICK
void AppTimerTick()
{
	tickcount++;
	if ((appTimeBusy && tickAlarm == tickcount) ||
	    (tickcount == repeatAlarmTime[0] || tickcount == repeatAlarmTime[1] || tickcount == repeatAlarmTime[2]))
		appTimerExpired(tickcount);
}
#endif
/*
	Common code for initiating a Firefly message. 
	Send once with appropriate group number.
	ISSUE If we are already sending a repeat message for 1 group can I send another message right away
	or should I ignore button presses until complete.
*/
void FireflyInitiateSend(uint8_t group)
{
	// create start toggle message, opcode is group code
	// add message counter for security
	int index;
	index = (group == GROUP_1) ? 0 : ((group == GROUP_2) ? 1 : 2);
	if (!repeatBusy[index])
	{
		++SendMessageCount;
		repeatMessageLength[index] = longMessage(Broadcast,GPIO_START_TOGGLE,group,sizeof(uint16_t),(uint8_t *)&SendMessageCount);
		FireflyMessage[index] = mymessage;
//		msgTx(UART,(uint8_t*)&FireflyMessage[index],repeatMessageLength[index]);
		// set up message send repetition
		timerReason = MSG_SEND_REPEAT;
		repeatcount[index] = eeprom.MessageRepeatCount;
		appTimerStart(eeprom.MessageRepeatDelay);
		repeatAlarmTime[index] = tickAlarm;
		repeatBusy[index] = true;	
		msgTx(UART,(uint8_t*)&FireflyMessage[index],repeatMessageLength[index]);
	}
}
/*
	Common code for Demo message, just send led on or off
*/
void DemoMessageSend(bool ledon)
{
	int length;
	// button down, ignore button up
	// create a led off message and send it via wireless
	length = shortMessage(Broadcast,GPIO,ledon ? GPIO_ON_WITH_ACK : GPIO_OFF_WITH_ACK);
	msgTx(UART,(uint8_t*)&mymessage,length);	
}
