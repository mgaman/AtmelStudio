/**
 * \file
 *
 * \brief User board configuration template
 *
 */

#ifndef CONF_BOARD_H
#define CONF_BOARD_H
//#define Broadcast  0x7f
//#define SourcePC 0x80;
#define EEPROM_PARAMETER_PAGE 0
#define EEPROM_AESKEY_PAGE 1
#define APPLICATION_TIMER &(TCC0)	// reserved for application use
#define APPLICATION_TIMER_CLOCK SYSCLK_TC0
// ENUMS - never start from zero
enum eProjects { Demo = 1, Firefly, Yuvalit };
	//              message id				opcode							datalength	data
enum eMessageID {	
					GPIO=1,				//GPIO_ON_WITH_ACK, GPIO_OFF_WITH_ACK     0
					ACKMSG,				// ON_ACK, OFF_ACK,NAK              0
					GPIO_START_TOGGLE,	// LONG_LED_ON_NO_ACK               0
					EEPROM_GET,			// start address					1			num. bytes
					EEPROM_PUT,			// start address					num. bytes	data [,data]
					EEPROM_READ,		// start address					num. bytes	data [,data]
					START_ARDUINO,		// none								0
					MSG_SEND_REPEAT,	// none                             0
					NULL_MESSAGE };
enum eOpCode { LED_ON_NO_ACK = 1, LED_OFF_NO_ACK, LONG_LED_ON_NO_ACK, GPIO_ON_WITH_ACK, GPIO_OFF_WITH_ACK, ON_ACK, OFF_ACK,NAK, DONT_CARE };
enum eChannel {UART=1,WIRELESS};
enum eLed {ON_BOARD=1,HIGH_POWER};
enum eGroup {GROUP_1 = 0x11,GROUP_2 = 0x22,GROUP_3 = 0x33};
enum eNode {Broadcast = 0x7f, PCConfig};	
// STRUCTURES
struct sMessage
{
	uint8_t sync1;
	uint8_t sync2;
	uint8_t packetLength;
	uint8_t SourceID;
	uint8_t DestinationID;
	uint8_t MessageId;
	uint8_t OpCode;
	uint8_t frameNumber;
	uint8_t dataLength;
	uint8_t data[33];	// largest data is AES key 32 bytes + cs
};

// UART
#define MMI_UART &USARTD1
#define MMI_BAUDRATE 9600
#define MMI_CHARLENGTH USART_CHSIZE_8BIT_gc
#define MMI_PARITY USART_PMODE_DISABLED_gc
#define MMI_STOPBITS false

#define TICK_RATE 1000

// debug time macros
//#define USE_SIMULATOR	// comment out for real board
#define SKIP_CHECKS // comment out after debugging
//#define USE_RTC	// if time values in eeprom can be expressed with granularity of 1 sec, uses RTC
// if using one shot time counter we cannot get a delay of > 2 secs because PER limited to unsigned short
//#define USE_ONESHOT_TC
#define USE_1K_TICK	// use a free running 1K tick for scheduling apptimer events
#endif // CONF_BOARD_H
