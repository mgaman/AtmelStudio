/*
 * i2c.c
 *
 * Created: 01/12/2014 21:18:30
 *  Author: Ofir
 */ 

#include "TrackerTWI.h"
#include <asf.h>

TWI_Slave_t slave;
twi_options_t m_options;

ISR(TWIE_TWIS_vect) {
	TWI_SlaveInterruptHandler(&slave);
}

void BoardTWIinit()
{
	irq_initialize_vectors();
	m_options.speed = TWI_SPEED;
	m_options.chip = 0x00;
	m_options.speed_reg = TWI_BAUD(sysclk_get_cpu_hz(), TWI_SPEED);
	TWI_MASTER_PORT.PIN0CTRL = PORT_OPC_WIREDANDPULL_gc;
	TWI_MASTER_PORT.PIN1CTRL = PORT_OPC_WIREDANDPULL_gc;
	sysclk_enable_peripheral_clock(&TWI_SLAVE);
}

void BoardSlave(void (*handler)(void), uint8_t slave_addr)
{
	cpu_irq_disable();
	twi_master_disable(&TWI_MASTER);
	TWI_SlaveInitializeDriver(&slave, &TWI_SLAVE, handler);
	TWI_SlaveInitializeModule(&slave, slave_addr, TWI_SLAVE_INTLVL_MED_gc);
	for (uint8_t i = 0; i < TWIS_RECEIVE_BUFFER_SIZE; i++) {
		slave.receivedData[i] = 0;
	}
	cpu_irq_enable();
}

void BoardMaster()
{
	cpu_irq_disable();
	twi_slave_disable(&TWI_MASTER);
	twi_master_init(&TWI_MASTER, &m_options);
	twi_master_enable(&TWI_MASTER);
	cpu_irq_enable();
}

void BoardMaster100khz()
{
	// change bus speed to 100khz
	cpu_irq_disable();
	twi_slave_disable(&TWI_MASTER);
	m_options.speed = 100000;
	m_options.speed_reg = TWI_BAUD(sysclk_get_cpu_hz(), 100000);
	twi_master_init(&TWI_MASTER, &m_options);
	twi_master_enable(&TWI_MASTER);
	cpu_irq_enable();
}