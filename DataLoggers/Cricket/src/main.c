/**
 *
 * Generic startup, calls board_init() to initialize all hardware
 *    loads tables from eeprom
 *    initializes all RF devices
 *    Call individual application e.g. cricket
 *
 */


/*
 * Include header files for all drivers that have been imported from
 * Atmel Software Framework (ASF).
 */
#include <string.h>
#include <asf.h>
#include "Utilities.h"
#include "EepromHandler.h"
#include "RFHandler.h"
#include "pcmsghandler.h"
#include "Applications.h"
#include "crypto.h"
#include "spi_interface.h"

// global data
// generic structure for performing a limited number of actions e.g. flash a led
struct sTimedActions TimedActions[RTC_TOTAL]; // 0 onboard led, 1 hipower led
bool rfActive[RF_ACTUAL_NUMBER_OF_DEVICES];
uint8_t si443regs[0x80];
void RTC_init(void);

uint16_t msgCounter; // starts at repetition count and decrements to zero 
uint16_t gvMsgRepCounter = 0;
uint16_t gvMsgIDCounterTX = 0;
uint16_t gvMsgIDCounterRX = 0;
uint16_t prevGvMsgIDCounter = 0;
bool newMessage = false;
bool sameMessageReceived = false;
uint16_t waitUntilRespond = 0;

uint8_t currentEpromCode = 255;

uint8_t activationCode_1 = 255;
uint8_t activationCode_2 = 255;
uint8_t activationCode_3 = 255;
uint8_t activationCode_4 = 255;

uint8_t GahliliotParams[MAX_GAHLI_FOR_SHALAT][GAHLI_PARAMS];	
uint8_t SlaveGahliliotParams[GAHLI_PARAMS];

uint8 buttonReleased = false;
uint8 buttonIntFlag  = false;


uint8_t activCodeParam    = 255;
uint8_t blinkNrParam      = 255;
uint8_t ledOnTimeParam    = 255;
uint8_t ledOffTimeParam   = 255;
uint8_t listenIntervParam = 255;

volatile uint16_t i2c_wait_flag_count = 0;
bool i2c_wait_finished = false;

bool time_to_read_gps;

//#define FUVARIO

int main (void)
{
	int j;
	uint8_t major,minor; 
	uint16_t eepromaddress;
	RF_ENUM Ret = 0xff;
	reset_cause_t resetCause = reset_cause_get_causes();
	int rftofrequencytable[2] = {RF_CHANNEL0_INDEX,RF_CHANNEL1_INDEX};			
    
	//pmic_init();        for vario
    //board_init();
	//cpu_irq_enable();
	
	pmic_init();	
	sysclk_init();
	irq_initialize_vectors();
	board_init();
	
	msgCounter = 0;
	
#if defined(CRICKET_MASTER) || defined(CRICKET_SLAVE)
	cricket();
#endif

#if defined(ECHO_MASTER) || defined(ECHO_SLAVE)
	echo();
#endif

#if defined(YUVALIT_MASTER) || defined (YUVALIT_SLAVE)
	yuvalit();
#endif

#if defined(BUZZER_MASTER) || defined (BUZZER_SLAVE)
	buzzer();
#endif

#if defined(GAHLILIT_MASTER) || defined (GAHLILIT_SLAVE)
    gahlilit();
#endif

#if defined(TEST)
	tester();
#endif

#if defined(SNIFFER)
	sniffer();
#endif

#ifdef ADC_MASTER
	adc_test();
#endif

#ifdef GPIO_INTERRUPT_TEST
   GpioIntTester();
#endif

#ifdef TRACKER_TEST
  TrackerTest();
#endif

#ifdef VARIO_TEST
   VarioTest();
#endif

	return 1;	// should never get here
}

