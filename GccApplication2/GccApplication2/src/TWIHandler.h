/*
 * TWIHandler.h
 *
 * Created: 05-Feb-15 10:46:21 AM
 *  Author: Saba
 */ 


#ifndef TWIHANDLER_H_
#define TWIHANDLER_H_

void TWIInit(void);
void TWISlave(void (*handler)(void), uint8_t slave_addr);

#ifdef TWI_MAIN
TWI_Slave_t slave;
twi_options_t m_options;
#else
extern TWI_Slave_t slave;
#endif

#endif /* TWIHANDLER_H_ */