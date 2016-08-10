/*
 * TWIHandler.c
 *
 * Created: 05-Feb-15 10:46:42 AM
 *  Author: Saba
 */ 

#include <asf.h>
#define TWI_MAIN
#include "TWIHandler.h"

ISR(TWIE_TWIS_vect) {
	TWI_SlaveInterruptHandler(&slave);
}

void TWIInit()
{
	m_options.speed = TWI_SPEED;
	m_options.chip = 0x00;
	m_options.speed_reg = TWI_BAUD(sysclk_get_cpu_hz(), TWI_SPEED);
	TWI_MASTER_PORT.PIN0CTRL = PORT_OPC_WIREDANDPULL_gc;
	TWI_MASTER_PORT.PIN1CTRL = PORT_OPC_WIREDANDPULL_gc;
	sysclk_enable_peripheral_clock(&TWI_SLAVE);	
}

void TWISlave(void (*handler)(void), uint8_t slave_addr)
{
	cpu_irq_disable();
	twi_master_disable(&TWI_MASTER);
	TWI_SlaveInitializeDriver(&slave, &TWI_SLAVE, handler);
	TWI_SlaveInitializeModule(&slave, slave_addr, TWI_SLAVE_INTLVL_MED_gc);
	twi_slave_enable(&TWI_SLAVE);
	cpu_irq_enable();
}
