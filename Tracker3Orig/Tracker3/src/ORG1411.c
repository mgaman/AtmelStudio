/*
  ORG1411 related
*/

#include <asf.h>
#include <stdio.h>
#include <string.h>
#include "ORG1411.h"
#include "LedManager.h"
#include "sirf.h"

// pedantic prototypes
void power_toggle(void);
void myWaitMs(unsigned int);

extern volatile bool rtcdone;
extern fifo_desc_t GPS_rx_fifo;
extern usart_rs232_options_t USART_GPS_OPTIONS;

static usart_rs232_options_t USART_GPS_OPTIONS_2 =
{
	.baudrate   = ORG1411_RUNTIME_BAUDRATE,
	.charlength = USART_CHSIZE_8BIT_gc,
	.paritytype = USART_PMODE_DISABLED_gc,
	.stopbits   = 1
};

//enum eParseState {BEFORE_DOLLAR,AFTER_DOLLAR,AFTER_ASTERISK,AFTER_CR} SentenceBuild;
// ORG1411 wakes up in NMEA mode, 4800 baud rate, send this string to change to OSF mode @ 115200 baud 
//char *SIRF115200 = "$PSRF100,0,115200,8,1,0*04\r\n";  // MUST have \r\n
char *SIRF38400	 = "$PSRF100,0,38400,8,1,0*3C\r\n";
//char *SIRF4800   = "$PSRF100,0,4800,8,1,0*0F\r\n";
//char *NMEA38400  = "$PSRF100,1,38400,8,1,0*3D\r\n";

void myWaitMs(unsigned int msecs)
{
	rtcdone = false;
	rtc_set_alarm_relative(msecs);
	while (!rtcdone);
	
}
void power_toggle()
{
	ioport_set_pin_dir(GPS_ON_OFF_PIN, IOPORT_DIR_OUTPUT);
	ioport_set_pin_level(GPS_ON_OFF_PIN, HIGH);
//	rtcdone = false;
//	rtc_set_alarm_relative(90);
//	while (!rtcdone);
	myWaitMs(90);
	ioport_set_pin_level(GPS_ON_OFF_PIN, LOW);
//	rtcdone = false;
//	rtc_set_alarm_relative(1000);
//	while (!rtcdone);
	myWaitMs(1000);
}

bool ORG1410_Init()
{
	bool level;
	uint8_t *pSirfMessage;
	int i;
	ioport_set_pin_dir(GPS_WAKEUP_PIN, IOPORT_DIR_INPUT);
	power_toggle();
	//ioport_set_pin_mode(GPS_WAKEUP, IOPORT_MODE_PULLUP);
	level = ioport_get_pin_level(GPS_WAKEUP_PIN);
	while (level == LOW)
	{
		power_toggle();
		level = ioport_get_pin_level(GPS_WAKEUP_PIN);
	}
	// SentenceBuild = BEFORE_DOLLAR;
	// change to SIRF mode @ 38400
	char *temp = SIRF38400;
	while (*temp)
		usart_putchar(USART_GPS,*temp++);
	// wait a decent time then change USART to 38400
    usart_tx_disable(USART_GPS);
    usart_rx_disable(USART_GPS);
	myWaitMs(1000);
    if (usart_init_rs232(USART_GPS, &USART_GPS_OPTIONS_2) != true)
		while (true){}
	// turn off all messages (this disables poll, nav parameters, poll software version
    usart_rx_disable(USART_GPS);
	pSirfMessage = (uint8_t *)MID166(ENABLEDISABLEALL,0,0);
	for (i=0;i<sizeof(struct sMID166);i++)
		usart_putchar(USART_GPS,*pSirfMessage++);
	myWaitMs(1000);
	// set 5Mhz mode
	pSirfMessage = (uint8_t *)MID136();
	for (i=0;i<sizeof(struct sMID136);i++)
		usart_putchar(USART_GPS,*pSirfMessage++);
	myWaitMs(1000);
	// initialize the parser
	SirfParseInit();
	// create a poll 41 message
	pSirfMessage = (uint8_t *)MID166(POLLONE,41,0);
	for (i=0;i<sizeof(struct sMID166);i++)
		usart_putchar(USART_GPS,*pSirfMessage++);
    usart_rx_enable(USART_GPS);
	return true;
}

void ORG1411_Test()
{
	uint8_t gps_byte = 0xff;
	struct sSirfHeader *pSirfHeader;
	struct sMID41* pMID41;
	// copy from GPS FIFO to CDC
	if (!fifo_is_empty(&GPS_rx_fifo))
	{
		fifo_pull_uint8(&GPS_rx_fifo,&gps_byte);
		udi_cdc_putc(gps_byte);
		if (SirfBlockReady(gps_byte))
		{
			pSirfHeader = SirfData();
			switch (pSirfHeader->MID)
			{
				case 41:
					pMID41 = (struct sMID41 *)pSirfHeader;
					if (pMID41->NavValid == 0)
					{
						LedToggle(LED1);
					}
			}
		}
	}
}
