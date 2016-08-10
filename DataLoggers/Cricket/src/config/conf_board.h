/**
 * \file
 *
 * \brief User board configuration template
 *
 */

#ifndef CONF_BOARD_H
#define CONF_BOARD_H
/*
    32Mhz internal clock
							+---------+
		 push button 0	A0	|ML       | B0
   push button 1/ADCREF A1	|ML       | B1 Select channel 1
		 push button 2	A2	|ML       | B2 IRQ CHANNEL 1
	push button 3 /ADC	A3	|ML       | B3
		  hipower led 1 A4	|L        | B4
		  hipower led 2 A5	|L        | B5
						A6	|         | B6
						A7	|         | B7
							|         |
						C0	|         | D0 spare GPIO
		    hipower LED C1	|SML    LM| D1 spare GPIO
   hipower LED 3/buzzer C2	|SML    LM| D2 IRQ CHANNEL 0
	       onboard LED  C3	|         | D3
	Select channel 0	C4	|         | D4
	
				MOSI    C5	|         | D5
				MISO    C6	|         | D6 UART RX
				SCLK    C7	|         | D7 UART TX
							+---------+
*/

#define _MAX_SI_SETTINGS		50

////USART for PC
#define USART_PC_RX_PIN			IOPORT_CREATE_PIN(PORTD, 6)
#define USART_PC_TX_PIN			IOPORT_CREATE_PIN(PORTD, 7)
#define USART_SERIAL_PC			&USARTD1
#define USART_PC_RX_INTERRUPT	USARTD1_RXC_vect
#define USART_PC_TX_INTERRUPT	USARTD1_TXC_vect

// USART for Modem
#define USART_MODEM_RX_PIN			IOPORT_CREATE_PIN(PORTD, 2)
#define USART_MODEM_TX_PIN			IOPORT_CREATE_PIN(PORTD, 3)
#define USART_SERIAL_MODEM			&USARTD0
#define USART_MODEM_RX_INTERRUPT	USARTD0_RXC_vect
#define USART_MODEM_TX_INTERRUPT	USARTD0_TXC_vect

//#define USART_SERIAL_PC_BAUD           115200
#define USART_SERIAL_MODEM_BAUD        115200            // for Modem / Bluetooth default



#define USART_SERIAL_CHAR_LENGTH         USART_CHSIZE_8BIT_gc
#define USART_SERIAL_PARITY              USART_PMODE_DISABLED_gc

#define USART_SERIAL_STOP_BIT          false

// fifo buffer sizes
#define _MAX_RX_FIFO_BUFFER_LENGTH		128
#define _MAX_TX_FIFO_BUFFER_LENGTH		128
#define _MAX_DEBUG_FIFO_BUFFER_LENGTH	80

// various leds push buttons
//                                                             small medium large
#define ONBOARD_LED			  IOPORT_CREATE_PIN(PORTC,3)     //     X     X       X
#define HIPOWER_LED			  IOPORT_CREATE_PIN(PORTC,1)     //     X     X       X

#define PUSH_BUTTON_2		  IOPORT_CREATE_PIN(PORTA,2)     //           X       X
//#define PUSH_BUTTON_3		  IOPORT_CREATE_PIN(PORTA,3)     //           X       X

#define LTC_MODE PUSH_BUTTON_2
#define LTC_PS   PUSH_BUTTON_3

#define AC_BUZZER			  IOPORT_CREATE_PIN(PORTC,2)     //     X     X       X	// AC uses PWM
#define DC_BUZZER			  IOPORT_CREATE_PIN(PORTC,2)     //     X     X       X	// DC just high or low

#define GAHLILIT_LEARN_BUTTON IOPORT_CREATE_PIN(PORTC,2)


#define HIPOWER_LED_1		  IOPORT_CREATE_PIN(PORTA,4)     //                   X


//-------------------- TRACKER -------------------------
#define TEST_TWI_DATA		  IOPORT_CREATE_PIN(PORTE,0)
#define TEST_TWI_CLK		  IOPORT_CREATE_PIN(PORTE,1)

#define ONBOARD_LED_2		  IOPORT_CREATE_PIN(PORTA,0)
#define ONBOARD_LED_3		  IOPORT_CREATE_PIN(PORTA,1)

#define GPS_ON_OFF_PIN		  IOPORT_CREATE_PIN(PORTA,5) 
#define GPS_WAKEUP_PIN        IOPORT_CREATE_PIN(PORTA,6)

#define MODEM_ON_OFF_PIN	  IOPORT_CREATE_PIN(PORTA,7)
#define MODEM_STATUS_PIN      IOPORT_CREATE_PIN(PORTA,6)

#define SPI_SS		          IOPORT_CREATE_PIN(PORTC,4)
#define SPI_MISO		      IOPORT_CREATE_PIN(PORTC,5)
#define SPI_MOSI   		      IOPORT_CREATE_PIN(PORTC,6)
#define SPI_CLK               IOPORT_CREATE_PIN(PORTC,7)

#define POWER_LATCH	          IOPORT_CREATE_PIN(PORTB,2)	
#define ACTIVATION_BUTTON     IOPORT_CREATE_PIN(PORTA,3)
//------------------------------------------------------



#define HIPOWER_LED_3		AC_BUZZER			
// give meaningful names to GPIO used by different applications

// unused pins
#define UNUSED_PIN_0 IOPORT_CREATE_PIN(PORTA,6)
//#define UNUSED_PIN_1 IOPORT_CREATE_PIN(PORTA,7)
#define UNUSED_PIN_11 IOPORT_CREATE_PIN(PORTB,0)
#define UNUSED_PIN_2 IOPORT_CREATE_PIN(PORTB,3)
#define UNUSED_PIN_3 IOPORT_CREATE_PIN(PORTB,4)
#define UNUSED_PIN_4 IOPORT_CREATE_PIN(PORTB,5)
#define UNUSED_PIN_5 IOPORT_CREATE_PIN(PORTB,6)
#define UNUSED_PIN_6 IOPORT_CREATE_PIN(PORTB,7)
//#define UNUSED_PIN_7 IOPORT_CREATE_PIN(PORTC,0)
//#define UNUSED_PIN_8 IOPORT_CREATE_PIN(PORTD,3)
//#define UNUSED_PIN_9 IOPORT_CREATE_PIN(PORTD,4)
//#define UNUSED_PIN_10 IOPORT_CREATE_PIN(PORTD,5)

// Timer
#define TIMER_1MS			 &TCC0
#define	TIMER_1MS_PERIOD	 512
#define	TIMER_1MS_RESOLUTION 256000

// Useful macros
//------------------------- TRACKER --------------------------------
#define GPS_ON_OFF_PIN_TO_HIGH 	ioport_set_pin_high(GPS_ON_OFF_PIN)
#define GPS_ON_OFF_PIN_TO_LOW 	ioport_set_pin_low(GPS_ON_OFF_PIN)

#define MODEM_ON_OFF_PIN_TO_HIGH 	ioport_set_pin_high(MODEM_ON_OFF_PIN)
#define MODEM_ON_OFF_PIN_TO_LOW 	ioport_set_pin_low(MODEM_ON_OFF_PIN)

#define ONBOARD_LED_2_ON 	 ioport_set_pin_high(ONBOARD_LED_2)
#define ONBOARD_LED_2_OFF 	 ioport_set_pin_low(ONBOARD_LED_2)
#define ONBOARD_LED_2_TOGGLE ioport_toggle_pin(ONBOARD_LED_2)

#define ONBOARD_LED_3_ON 	 ioport_set_pin_high(ONBOARD_LED_3)
#define ONBOARD_LED_3_OFF 	 ioport_set_pin_low(ONBOARD_LED_3)
#define ONBOARD_LED_3_TOGGLE ioport_toggle_pin(ONBOARD_LED_3)


#define SPI_SS_ON 	 ioport_set_pin_high(SPI_SS)
#define SPI_SS_OFF 	 ioport_set_pin_low(SPI_SS)

#define SPI_MOSI_ON  ioport_set_pin_high(SPI_MOSI)
#define SPI_MOSI_OFF ioport_set_pin_low(SPI_MOSI)

#define SPI_CLK_ON 	 ioport_set_pin_high(SPI_CLK)
#define SPI_CLK_OFF  ioport_set_pin_low(SPI_CLK)


#define TX_PC_ON   ioport_set_pin_high(USART_PC_TX_PIN)
#define TX_PC_OFF  ioport_set_pin_low(USART_PC_TX_PIN)

#define RX_PC_ON   ioport_set_pin_high(USART_PC_RX_PIN)
#define RX_PC_OFF  ioport_set_pin_low(USART_PC_RX_PIN)


#define TX_MODEM_ON   ioport_set_pin_high(USART_MODEM_TX_PIN)
#define TX_MODEM_OFF  ioport_set_pin_low(USART_MODEM_TX_PIN)

#define RX_MODEM_ON   ioport_set_pin_high(USART_MODEM_RX_PIN)
#define RX_MODEM_OFF  ioport_set_pin_low(USART_MODEM_RX_PIN)


#define LTC_MODE_ON   ioport_set_pin_high(LTC_MODE)
#define LTC_MODE_OFF  ioport_set_pin_low(LTC_MODE)

#define LTC_PS_ON   ioport_set_pin_high(LTC_PS)
#define LTC_PS_OFF  ioport_set_pin_low(LTC_PS)


#define MODEM_STATUS_FOR_PIN_TEST_ON   ioport_set_pin_high(MODEM_STATUS_PIN)
#define MODEM_STATUS_FOR_PIN_TEST_OFF  ioport_set_pin_low(MODEM_STATUS_PIN)
//------------------------------------------------------------------


#define ONBOARD_LED_OFF		ioport_set_pin_high(ONBOARD_LED)
#define ONBOARD_LED_ON		ioport_set_pin_low(ONBOARD_LED)
#define ONBOARD_LED_TOGGLE	ioport_toggle_pin(ONBOARD_LED)

#define HIPOWER_LED_OFF		ioport_set_pin_low(HIPOWER_LED)
#define HIPOWER_LED_ON		ioport_set_pin_high(HIPOWER_LED)

#define HIPOWER_LED_TOGGLE	ioport_toggle_pin(HIPOWER_LED)
#define HIPOWER_LED_1_OFF	ioport_set_pin_low(HIPOWER_LED_1)
#define HIPOWER_LED_1_ON	ioport_set_pin_high(HIPOWER_LED_1)
#define HIPOWER_LED_1_TOGGLE ioport_toggle_pin(HIPOWER_LED_1)
#define HIPOWER_LED_2_OFF	ioport_set_pin_low(HIPOWER_LED_2)
#define HIPOWER_LED_2_ON	ioport_set_pin_high(HIPOWER_LED_2)
#define HIPOWER_LED_2_TOGGLE ioport_toggle_pin(HIPOWER_LED_2)

#define GAHLILIT_IR_LED_1_ON  ioport_set_pin_high(GAHLILIT_IR_LED_1)
#define GAHLILIT_IR_LED_1_OFF ioport_set_pin_low(GAHLILIT_IR_LED_1)

#define GAHLILIT_IR_LED_2_ON  ioport_set_pin_high(GAHLILIT_IR_LED_2)
#define GAHLILIT_IR_LED_2_OFF ioport_set_pin_low(GAHLILIT_IR_LED_2)

#define BUZZER_OFF   ioport_set_pin_high(DC_BUZZER)
#define BUZZER_ON    ioport_set_pin_low(DC_BUZZER)


enum eRTCevents {RTC_ONBOARD,RTC_HIPOWER,RTC_HIPOWER1,RTC_HIPOWER2,RTC_SPARE0,RTC_SPARE1,RTC_IR_1,RTC_IR_2,RTC_TOTAL};
#define RTC_FOREVER 0

struct sTimedActions
{
	bool active;
	uint16_t repetitions_left;
	bool endless;
	bool period0;				// if in period 0 or 1
	uint16_t ticks_left;		// of current period
	uint16_t ticks_period_0;	// length of period 0
	uint16_t ticks_period_1;	// length of period 1
	void (*function)(void);		// common action to take
};


//TWI_t * trcTwi = &TWIE;

#define MSG_REPEAT_COUNT 5
#define MSG_REPEAT_DELAY 2 // RTC ticks

//#define RF_MAX_NUMBER_OF_DEVICES 2	// 1 on small, medium, 2 on large
//#define RF_ACTUAL_NUMBER_OF_DEVICES 2
//typedef uint8_t tRFparameters[_MAX_SI_SETTINGS][RF_MAX_NUMBER_OF_DEVICES];

#define ENABLE_ENCRYPTION

//#define MASTER_LEDS_FOR_DEBUG

// App specific stuff
//#define WIFI_ON_BUTTON		PUSH_BUTTON_1
//#define WIFI_OFF_BUTTON		PUSH_BUTTON_2

#define WIFI_ON_BUTTON		PUSH_BUTTON_2      // changed according to requests document
#define WIFI_OFF_BUTTON		PUSH_BUTTON_1      // changed according to requests document


#define WIFI_ON_BUTTON		PUSH_BUTTON_2      
#define WIFI_OFF_BUTTON		PUSH_BUTTON_1 


//#define GROUP_1_BUTTON      PUSH_BUTTON_0

#define GROUP_1_BUTTON      SPARE_GPIO_0

#define GROUP_2_BUTTON      PUSH_BUTTON_1
#define GROUP_3_BUTTON      PUSH_BUTTON_2
//#define GROUP_4_BUTTON      PUSH_BUTTON_3
#define GROUP_4_BUTTON      SPARE_GPIO_1


// EPROM, arbitrary choice of page, XMEGA4AU has 32 pages of 32 bytes, we need 3 consecutive pages
//#define EEPROM_PARAMETER_PAGE 30
    

#define YUVALIT_TRIGGER		PUSH_BUTTON_1
#define OK_LED				HIPOWER_LED
#define OK_LED_ON			HIPOWER_LED_ON
#define OK_LED_OFF			HIPOWER_LED_OFF
#define OK_LED_TOGGLE		HIPOWER_LED_TOGGLE

#define SLAVE_BATTERY_LED			SPARE_GPIO_0
#define SLAVE_BATTERY_LED_ON		ioport_set_pin_high(SLAVE_BATTERY_LED)
#define SLAVE_BATTERY_LED_OFF		ioport_set_pin_low(SLAVE_BATTERY_LED)
#define SLAVE_BATTERY_LED_TOGGLE	ioport_toggle_pin(SLAVE_BATTERY_LED)

#define MASTER_BATTERY_LED			SPARE_GPIO_1
#define MASTER_BATTERY_LED_ON		ioport_set_pin_high(MASTER_BATTERY_LED)
#define MASTER_BATTERY_LED_OFF		ioport_set_pin_low(MASTER_BATTERY_LED)
#define MASTER_BATTERY_LED_TOGGLE	ioport_toggle_pin(MASTER_BATTERY_LED)

#define RTC_OK	RTC_HIPOWER
#define RTC_SLAVE	RTC_HIPOWER1


//#define RF_MAX_NUMBER_OF_DEVICES    2	                                      // 1 on small, medium, 2 on large

#define RF_MAX_NUMBER_OF_DEVICES    2
#define RF_ACTUAL_NUMBER_OF_DEVICES 1
typedef uint8_t tRFparameters[_MAX_SI_SETTINGS][RF_MAX_NUMBER_OF_DEVICES];


#define MAX_GAHLI_FOR_SHALAT 4
#define GAHLI_PARAMS 5


//if (RF_ACTUAL_NUMBER_OF_DEVICES == 2)
//{
	//#define YUVALIT_RF_ID 1
//}
//else
//{
	//#define YUVALIT_RF_ID 0
//}


#define YUVALIT_RF_ID 0

#define BUZZER_RF_ID      0     // to compile gahlilit
#define RF_CHANNEL0_INDEX 0     // to compile gahlilit
#define RF_CHANNEL1_INDEX 1     // to compile gahlilit

#define NO_LEARN_CODE 255

#define ONE_CHIP_RF_ID 0

//#define YUVALIT_VOLTAGE_SAMPLE_PIN PUSH_BUTTON_3

#define	ADC_CHANNEL		ADC_CH2	 
//#define VOLTAGE_SAMPLE_PIN	PUSH_BUTTON_3 // sample voltage via PA3
#define ADC_CHANNEL_PIN ADCCH_POS_PIN3		// sample voltage via PA3
#define A_REF			PUSH_BUTTON_0

#define CRICKET_BUZZER_ON_BUTTON		PUSH_BUTTON_0
#define CRICKET_BUZZER_OFF_BUTTON		PUSH_BUTTON_3

#define ON_OFF_TIMEOUT 30

#define MAX_GAHLIL_SLEEP_CNT   10
#define MAX_GAHLIL_AWAKE_CNT    1

#define GAHLIL_MASTER_WAIT_FOR_PC_PERIOD 600


// buzzer app definitions
#if defined(BUZZER_MASTER) || defined(BUZZER_SLAVE)
#define BUZZER_ON_OFF_BUTTON PUSH_BUTTON_3	
	#if RF_ACTUAL_NUMBER_OF_DEVICES == 1 // small or medium board
	#define BUZZER_RF_ID 0
	#define RF_CHANNEL0_INDEX 0 // 433.3 Mhz
	#define RF_CHANNEL1_INDEX 1
	#else
	#define BUZZER_RF_ID 1		// on large board use 2nd device because that has an antenna attached
	#define RF_CHANNEL0_INDEX 1
	#define RF_CHANNEL1_INDEX 0 // 433.3 Mhz
	#endif
#else
#define BUZZER_ON_OFF_BUTTON PUSH_BUTTON_0
#endif





//#define CRICKET_SLEEP_BUTTON		    PUSH_BUTTON_2
//#define CRICKET_WAKE_BUTTON		    PUSH_BUTTON_3
#if defined(SNIFFER) || defined(BUZZER_SLAVE)
// sniffer wants to see all messages, including repeats
#define IGNORE_MESSAGE_SEQUENCING
#endif
#endif // CONF_BOARD_H
