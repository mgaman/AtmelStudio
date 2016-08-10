/*
 * OTG1411.c
 *
 * Created: 30-Jan-15 9:20:52 AM
 *  Author: Saba
 */ 


//#include <avr/io.h>
#include <asf.h>
#include <string.h>
#include <stdio.h>
#include "LedManager.h"
#include "sirf.h"

void On_Off(void);
void ORG1411Reset(void);
void ORG1411main(void);

extern volatile bool rtcdone;
extern volatile bool cdc_up;
fifo_desc_t i2c_slave_rx_fifo;
uint8_t		 i2c_slave_rx_buffer[128];
bool mid41active;

TWI_Slave_t slave;
twi_options_t m_options;
twi_package_t packet;

char *SIRF115200 = "$PSRF100,0,115200,8,1,0*04\r\n";  // MUST have \r\n
struct sMID166 *pMID166;
struct sMID136 *pMID136;
char printbuf[100];
uint8_t c;
struct sSirfHeader *pSirfHeader;
struct sMID41 *pMID41;
uint32_t ferrcount;

void slaveProcess(void)
{
	// copy data to fifo
	if (mid41active)
	{
	//	if (cdc_up)
	//		udi_cdc_putc(slave.receivedData[slave.bytesReceived]);
		if (fifo_push_uint8(&i2c_slave_rx_fifo,(uint32_t)slave.receivedData[slave.bytesReceived]) != FIFO_OK)
			ferrcount++;
	}
	slave.bytesReceived = 0;	
}
	
ISR(TWIE_TWIS_vect) {
	TWI_SlaveInterruptHandler(&slave);
}

void On_Off()
{
	// see Hornet ORG14xx datasheet 7.2.2
	ioport_set_pin_level(GPS_ON_OFF_PIN,HIGH);
	rtcdone = false;
	rtc_set_alarm_relative(90);
	while (!rtcdone);
	ioport_set_pin_level(GPS_ON_OFF_PIN,LOW);
	rtcdone = false;
	rtc_set_alarm_relative(1000);
	while (!rtcdone);
}

void ORG1411Reset()
{
	bool hibernate;
	// bring gps to known state see Hornet ORG14xx datasheet Chapter 9.1
	// on_off pulse alternately switches gps on/off but as we don't know its original state
	// force it into power off then power on
	hibernate = ioport_get_pin_level(GPS_WAKEUP_PIN) == HIGH;
	if (!hibernate)
		while (!hibernate)
		{
			On_Off();
			hibernate = ioport_get_pin_level(GPS_WAKEUP_PIN) == HIGH;	
		}
	while (hibernate)
	{
		On_Off();
		hibernate = ioport_get_pin_level(GPS_WAKEUP_PIN) == HIGH;
	}
}

void TWIMasterMode(TWI_t *twi)
{
	cpu_irq_disable();
	twi_slave_disable(twi);
	twi_master_init(twi, &m_options);
	twi_master_enable(twi);
	cpu_irq_enable();	
}

void TWISlaveMode(TWI_t *twi,void (*handler)(void),uint8_t selfAddress)
{
	cpu_irq_disable();
	twi_master_disable(twi);
	TWI_SlaveInitializeDriver(&slave, twi, handler);
	TWI_SlaveInitializeModule(&slave, selfAddress, TWI_SLAVE_INTLVL_MED_gc);
	memset(slave.receivedData,0,TWIS_RECEIVE_BUFFER_SIZE);
	twi_slave_enable(twi);
	cpu_irq_enable();
}

void ORG1411main()
{
	int length;
	mid41active = false;
	ORG1411Reset();
	// if we get here turn on led
	LedOn(LED1);
	// GPS is now in default state, I2C Master NMEA
	// set up fifo to receive slave data
	fifo_init(&i2c_slave_rx_fifo,i2c_slave_rx_buffer,sizeof(i2c_slave_rx_buffer));
	// Setup TWI slave & just let it run, data goes into fifo
	// loop in main empties fifo & sends to serial port	
	m_options.speed = TWI_SPEED;
	m_options.chip = 0;
	m_options.speed_reg = TWI_BAUD(sysclk_get_cpu_hz(), TWI_SPEED);
	TWI_PORT_GPS.PIN0CTRL = PORT_OPC_WIREDANDPULL_gc;
	TWI_PORT_GPS.PIN1CTRL = PORT_OPC_WIREDANDPULL_gc;
	sysclk_enable_peripheral_clock(&TWI_ORG1411);  // same clock for master & slave
	// set up Atmel as a slave device
	TWISlaveMode(&TWI_ORG1411,slaveProcess,GPS_AS_MASTER_ADDR);
	// put ORG1411 into SIRF mode
	// wait until I2C master bus is idle, then send a PSRF100 message
	packet.chip = GPS_AS_SLAVE_ADDR;
	packet.addr_length = 0;
	packet.buffer = SIRF115200;
	packet.length = strlen(SIRF115200);
	packet.no_wait = false; 
	TWIMasterMode(&TWI_ORG1411);
	twi_master_write(&TWI_ORG1411,&packet);
	TWISlaveMode(&TWI_ORG1411,slaveProcess,GPS_AS_MASTER_ADDR);
	delay_s(1);
	// turn off all messages (this disables poll nav parameters, poll software version
	pMID166 = MID166(ENABLEDISABLEALL,0,0);
	packet.buffer = pMID166;
	packet.length = sizeof(struct sMID166);
	TWIMasterMode(&TWI_ORG1411);
	twi_master_write(&TWI_ORG1411,&packet);
	TWISlaveMode(&TWI_ORG1411,slaveProcess,GPS_AS_MASTER_ADDR);
	delay_s(1);
	// set 5Mhz mode
	pMID136 = MID136();
	packet.chip = GPS_AS_SLAVE_ADDR;
	packet.addr_length = 0;
	packet.buffer = pMID136;
	packet.length = sizeof(struct sMID136);
	packet.no_wait = false;
	TWIMasterMode(&TWI_ORG1411);
	twi_master_write(&TWI_ORG1411,&packet);
	TWISlaveMode(&TWI_ORG1411,slaveProcess,GPS_AS_MASTER_ADDR);
	delay_s(1);
	// create a poll 41 message
	pMID166 = MID166(POLLONE,41,0);
	// initialize the parser
	SirfParseInit();
	// kick off the timer
	rtcdone = false;
	mid41active = true;
	rtc_set_alarm_relative(200);
	// pool at 5Hz
	while (true)
	{
		bool gotac;
		cpu_irq_disable();
		gotac = fifo_pull_uint8(&i2c_slave_rx_fifo,&c) == FIFO_OK;
		cpu_irq_enable();
		if (gotac)
		{
			if (SirfBlockReady(c))
			{
				pSirfHeader = SirfData();
				switch (pSirfHeader->MID)
				{
					case 41:
//						LedToggle(LED1);
						pMID41 = (struct sMID41 *)pSirfHeader;
						if (pMID41->NavValid == 0)
						{
							LedToggle(LED1);
							length = sprintf(printbuf,"%ld,%ld,%ld\r\n",
								BELE4(pMID41->Longitude),
								BELE4(pMID41->Latitude),
								BELE4(pMID41->AltitudeFromMSL));
							if (cdc_up)
							{
								LedToggle(LED2);
								udi_cdc_write_buf(printbuf,length);
							}
						}
						break;
					default:
						break;
				}
			}
		}
		if (rtcdone)
		{
//			LedToggle(LED2);
			packet.chip = GPS_AS_SLAVE_ADDR;
			packet.addr_length = 0;
			packet.buffer = pMID166;
			packet.length = sizeof(struct sMID166);
			packet.no_wait = false;
			TWIMasterMode(&TWI_ORG1411);
			twi_master_write(&TWI_ORG1411,&packet);
			TWISlaveMode(&TWI_ORG1411,slaveProcess,GPS_AS_MASTER_ADDR);
			rtcdone = false;
			rtc_set_alarm_relative(200);
		}
	}
}
