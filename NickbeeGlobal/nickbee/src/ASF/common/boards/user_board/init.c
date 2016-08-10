/**
 * \file
 *
 * \brief User board initialization template
 *
 */

#include <asf.h>
#include <board.h>
#include <conf_board.h>
#include "EEPROM.h"
#include "Application.h"
// global data
static usart_rs232_options_t UOPS = {
	.baudrate = MMI_BAUDRATE,
	.charlength = MMI_CHARLENGTH,
	.paritytype = MMI_PARITY,
	.stopbits = MMI_STOPBITS
};
// prototypes
void AppTimerCallback(void);
void AppTimerTick(void);

void board_init(void)
{
#if defined (USE_1K_TICK)
	volatile uint32_t periphrate;
	volatile uint32_t factor;
#endif
	ioport_init();
	ioport_set_pin_dir(PUSH_BUTTON_1,IOPORT_DIR_INPUT);
	ioport_set_pin_mode(PUSH_BUTTON_1,IOPORT_MODE_PULLUP);
	ioport_set_pin_sense_mode(PUSH_BUTTON_1,IOPORT_SENSE_FALLING);
	ioport_set_pin_dir(PUSH_BUTTON_2,IOPORT_DIR_INPUT);
	ioport_set_pin_mode(PUSH_BUTTON_2,IOPORT_MODE_PULLUP);
	ioport_set_pin_sense_mode(PUSH_BUTTON_2,IOPORT_SENSE_FALLING);
	ioport_set_pin_dir(PUSH_BUTTON_3,IOPORT_DIR_INPUT);
//	ioport_set_pin_mode(PUSH_BUTTON_3,IOPORT_MODE_PULLUP);
//	ioport_set_pin_mode(PUSH_BUTTON_3,IOPORT_MODE_PULLDOWN);
	ioport_set_pin_sense_mode(PUSH_BUTTON_3,IOPORT_SENSE_BOTHEDGES);
//	ioport_set_pin_sense_mode(PUSH_BUTTON_3,IOPORT_SENSE_FALLING);
	ioport_set_pin_dir(ON_BOARD_LED,IOPORT_DIR_OUTPUT);
	ioport_set_pin_mode(ON_BOARD_LED,IOPORT_MODE_PULLUP); // led off
	gpio_set_pin_high(ON_BOARD_LED);
	ioport_set_pin_dir(HIGH_POWER_LED,IOPORT_DIR_OUTPUT);
	ioport_set_pin_mode(HIGH_POWER_LED,IOPORT_MODE_PULLUP); // led off
	gpio_set_pin_high(HIGH_POWER_LED);	
	ioport_set_pin_dir(POWER_LATCH,IOPORT_DIR_OUTPUT);
	ioport_set_pin_mode(POWER_LATCH,IOPORT_MODE_PULLUP); // led off
	gpio_set_pin_high(POWER_LATCH);
	
	ioport_set_pin_dir(LED0,IOPORT_DIR_OUTPUT);
	ioport_set_pin_mode(LED0,IOPORT_MODE_PULLDOWN); // led off
	gpio_set_pin_high(LED0);

	ioport_set_pin_dir(LED1,IOPORT_DIR_OUTPUT);
	ioport_set_pin_mode(LED1,IOPORT_MODE_PULLDOWN); // led off
	gpio_set_pin_high(LED1);
	pmic_init();
	// setup interrupt mask for 3 push buttons
	PORTA.INT0MASK |= 1<<1;	// GPIO_1
	PORTA.INT1MASK |= 1<<1;	// GPIO_1
	PORTA.INTCTRL |= PORT_INT0LVL0_bm << PORT_INT0LVL_gp;
	PORTA.INT0MASK |= 1<<2; // GPIO_2
	PORTA.INT1MASK |= 1<<2;	// GPIO_2
	PORTA.INTCTRL |= PORT_INT1LVL0_bm << PORT_INT1LVL_gp;
	PORTA.INT0MASK |= 1<<3;	// GPIO_3
	PORTA.INT1MASK |= 1<<3;	// GPIO_3
	PORTA.INTCTRL |= PORT_INT0LVL0_bm << PORT_INT0LVL_gp;
	PORTA.INTCTRL |= PORT_INT0LVL1_bm << PORT_INT1LVL_gp;
	// usart
	ioport_set_pin_dir(IOPORT_CREATE_PIN(PORTD,6),IOPORT_DIR_INPUT);  //RX
	ioport_set_pin_dir(IOPORT_CREATE_PIN(PORTD,7),IOPORT_DIR_OUTPUT);  //TX
//	sysclk_enable_module(SYSCLK_PORT_D,PR_USART1_bm);
	usart_init_rs232(MMI_UART,&UOPS);
	usart_set_rx_interrupt_level(MMI_UART,USART_INT_LVL_MED);
	usart_set_tx_interrupt_level(MMI_UART,USART_INT_LVL_OFF);
#ifdef USE_RTC
	rtc_init();
#elif defined (USE_1K_TICK)
 // set up a free running 1K tick
	sysclk_enable_module(SYSCLK_PORT_C, APPLICATION_TIMER_CLOCK);
	tc_set_wgm(APPLICATION_TIMER,TC_WG_NORMAL);
	tc_write_clock_source(APPLICATION_TIMER,TC_CLKSEL_DIV8_gc);
	periphrate = tc_get_resolution(APPLICATION_TIMER);
	tc_set_overflow_interrupt_level(APPLICATION_TIMER,TC_INT_LVL_LO);
	tc_set_overflow_interrupt_callback(APPLICATION_TIMER,AppTimerTick);
	factor = periphrate / 1000;
	// factor must be < 65K
	tc_write_period(APPLICATION_TIMER,factor);  // tune this number for best result
#endif
}
// Interrupt vectors
volatile int jj;
extern struct sMessage mymessage;
uint8_t CheckSum(uint8_t *buff,int length);
int  shortMessage(uint8_t dest,enum eMessageID msg,enum eOpCode opcode);
volatile uint16_t mcounter = 0;

ISR(PORTA_INT1_vect)
{
	if (!ioport_get_value(PUSH_BUTTON_1))
	gpioEvent(PUSH_BUTTON_1,false);
	else if (!ioport_get_value(PUSH_BUTTON_2))
	gpioEvent(PUSH_BUTTON_2,false);
	if (!ioport_get_value(PUSH_BUTTON_3))
	gpioEvent(PUSH_BUTTON_3,false);
}
ISR(PORTA_INT0_vect)
{
	if (!ioport_get_value(PUSH_BUTTON_1))
		gpioEvent(PUSH_BUTTON_1,false);
	else if (!ioport_get_value(PUSH_BUTTON_2))
		gpioEvent(PUSH_BUTTON_2,false);
	if (!ioport_get_value(PUSH_BUTTON_3))
		gpioEvent(PUSH_BUTTON_3,false);
}
