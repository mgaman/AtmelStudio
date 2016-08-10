/**
 * \file
 *
 * \brief User board definition template
 *
 */

 /* This file is intended to contain definitions and configuration details for
 * features and devices that are available on the board, e.g., frequency and
 * startup time for an external crystal, external memory devices, LED and USART
 * pins.
 */
 /**
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#ifndef USER_BOARD_H
#define USER_BOARD_H

#include <conf_board.h>

// External oscillator settings.
// Uncomment and set correct values if external oscillator is used.

// External oscillator frequency
//#define BOARD_XOSC_HZ          8000000

// External oscillator type.
//!< External clock signal
//#define BOARD_XOSC_TYPE        XOSC_TYPE_EXTERNAL
//!< 32.768 kHz resonator on TOSC
//#define BOARD_XOSC_TYPE        XOSC_TYPE_32KHZ
//!< 0.4 to 16 MHz resonator on XTALS
//#define BOARD_XOSC_TYPE        XOSC_TYPE_XTAL

// External oscillator startup time
//#define BOARD_XOSC_STARTUP_US  500000

//-------------------- TRACKER hardware -------------------------
/*
                                +--------------------+
                         LED1 A0|                    |B0 ADC8
                              A1|                    |B1 ADC9 VBAT
                  WISMO_RESET A2|    ATXMEGA1284U    |B2 POWER_LATCH
                      POW_INT A3|                    |B3
                    CHARGE_ON A4|                    |B4
                   GPS ON/OFF A5|                    |B5
                    WISMO_RDY A6|                    |B6
                 WISMO ON/OFF A7|                    |B7
                                |                    |
                              C0|                    |D0 red power led
                              C1|                    |D1 GPS_WAKEUP
                              C2|                    |D2 UARTD0_RX WS6318
                              C3|                    |D3 UARTD0_TX
          W25Q128FVWIG SPI SS C4|                    |D4
                         MOSI C5|                    |D5 LED2
                         MISO C6|                    |D6 USB D-
                         SCLK C7|                    |D7 USB D+
                                |                    |
       LSM9DS0 MS5611 I2C SDA E0|                    |
                      I2C SCL E1|                    |
            ORG1411 UARTE0_RX E2|                    |
                    UARTE0_TX E3|                    |
                              E4|                    |
                              E5|                    |
                              E6|                    |
                              E7|                    |
                                +--------------------+
*/
  
#define ONBOARD_LED_1		  IOPORT_CREATE_PIN(PORTA,0)
#define ONBOARD_LED_2		  IOPORT_CREATE_PIN(PORTD,5)
#define ACTIVATION_PIN		  3
#define ACTIVATION_BUTTON     IOPORT_CREATE_PIN(PORTA,ACTIVATION_PIN)
#define GPS_ON_OFF_PIN		  IOPORT_CREATE_PIN(PORTA,5)
#define GPS_WAKEUP_PIN        IOPORT_CREATE_PIN(PORTD,1)

#define GSM_ON_OFF_PIN		  IOPORT_CREATE_PIN(PORTA,7)
#define GSM_WAKEUP_PIN        IOPORT_CREATE_PIN(PORTA,6)
#define GSM_RESET_PIN		  IOPORT_CREATE_PIN(PORTA,2)

#define POWER_LATCH	          IOPORT_CREATE_PIN(PORTB,2)

#define SPI_MASTER			SPIC
#define SPI_SS		          IOPORT_CREATE_PIN(PORTC,4)
#define SPI_MISO		      IOPORT_CREATE_PIN(PORTC,5)
#define SPI_MOSI   		      IOPORT_CREATE_PIN(PORTC,6)
#define SPI_CLK               IOPORT_CREATE_PIN(PORTC,7)

#define TWI_MASTER				TWIE
#define TWI_MASTER_PORT			PORTE
#define TWI_SPEED				400000
#define TWI_DATA				IOPORT_CREATE_PIN(PORTE,0)
#define TWI_CLK					IOPORT_CREATE_PIN(PORTE,1)

#define USART_GSM				&USARTD0
#define USART_GSM_RX_PIN		IOPORT_CREATE_PIN(PORTD, 2)
#define USART_GSM_TX_PIN		IOPORT_CREATE_PIN(PORTD, 3)
#define USART_GSM_RX_INTERRUPT	USARTD0_RXC_vect
#define USART_GSM_TX_INTERRUPT	USARTD0_TXC_vect

#define USART_GPS				&USARTE0
#define USART_GPS_RX_PIN		IOPORT_CREATE_PIN(PORTE, 2)
#define USART_GPS_TX_PIN		IOPORT_CREATE_PIN(PORTE, 3)
#define USART_GPS_RX_INTERRUPT	USARTE0_RXC_vect
#define USART_GPS_TX_INTERRUPT	USARTE0_TXC_vect

#define _MAX_RX_FIFO_BUFFER_LENGTH  128 // see ASF documentation

// Timer
#define TIMER_1MS			 &TCC0
#define	TIMER_1MS_PERIOD	 512
#define	TIMER_1MS_RESOLUTION 256000

// prototypes
void tracker(void);
void rtccallback(uint32_t t);
void tc1ms_ovf_interrupt_callback(void);

#define MS5611BA03_SAMPLE_RATE 100  // Hz
#define WS6318_BAUDRATE 115200
#define ORG1411_POWERON_BAUDRATE 4800
#define ORG1411_RUNTIME_BAUDRATE 38400

// define how we are sending data to host
#define SERIAL_CDC
//#define SERIAL_UART
#endif // USER_BOARD_H
