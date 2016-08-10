/*
    loop 
	reads data from the PC FIFO and passes it to the WS6318 FIFO
	reads data from the WS6318 FIFO and passes it to the PC FIFO
	
*/

#include <asf.h>
#include "WS6318.h"

extern fifo_desc_t GSM_rx_fifo;
extern fifo_desc_t PC_rx_fifo;
extern volatile bool rtcdone;
volatile bool cdc_up;
volatile bool PCrecvd = false;
// modem strings
char *APN = "modem.orange.net.il";
char *HostIP = "gpsarena.zickel.net";
int HostPort = 80;
char *URI = "get /addlocation.php?timestamp=%04d-%02d-%02d%%20%02d:%02d:%02d&lat=%d.%d&lon=%d.%d&alt=%d&id=%s HTTP/1.1\n\
Host: %s\n\n\003"; // add date, time, latitude , longitude, altitude, ID
void power_toggle_WS6318(void);

char *modemstrings[] = {
	"AT+WIPCFG=1",  //START IP STACK
	"AT+WIPBR=1,6",   //open GPRS
//	"AT+WIPBR=2,6,11,\"uwap.orange.co.il\"",  // 3g portal
	"AT+WIPBR=2,6,11,%s",  // APN goes here
	"AT+WIPBR=4,6,0",  //start gprs bearer
//	"AT+WIPBR=3,6,15",  //get local ip address
	"AT+WIPCREATE=2,1,%s,%d",   //create tcp client on socket 1 for host and port
	"AT+WIPDATA=2,1,1",   //open tcp socket 1 in uart mode
	"AT+WIPCLOSE=2,1",
	"AT+WIPCFG=1"};  //stop IP STACK

void WS6318_Test()
{
	uint8_t gsm_byte = 0xff;
	// copy from GSM FIFO to PC BUT ONLY after 1st char from terminal
	while (PCrecvd == true && !fifo_is_empty(&GSM_rx_fifo))
	{
		fifo_pull_uint8(&GSM_rx_fifo,&gsm_byte);
		if (cdc_up == true)
			udi_cdc_putc(gsm_byte);
	}	
	// copy from PC FIFO to GSM
	while (!fifo_is_empty(&PC_rx_fifo))
	{
		fifo_pull_uint8(&PC_rx_fifo,&gsm_byte);
		PCrecvd = true;
		usart_putchar(USART_GSM,gsm_byte);
	}
}

void power_toggle_WS6318(void)
{
	ioport_set_pin_level(GSM_RESET_PIN, LOW);
	ioport_set_pin_level(GSM_ON_OFF_PIN, HIGH);
	rtcdone = false;
	rtc_set_alarm_relative(250);
	while (!rtcdone);
//	delay_ms(250);
	ioport_set_pin_level(GSM_ON_OFF_PIN, LOW);
	rtcdone = false;
	rtc_set_alarm_relative(50);
	while (!rtcdone);
//	delay_ms(50);
	ioport_set_pin_level(GSM_RESET_PIN, HIGH);
	rtcdone = false;
	rtc_set_alarm_relative(600);
	while (!rtcdone);
//	delay_ms(600);
}

// see datasheet 5.1.3.1
char *AT = "AT\n";
bool WS6318Init()
{
    power_toggle_WS6318();
	while (ioport_get_pin_level(GSM_WAKEUP_PIN) == LOW){}
	return true;
}