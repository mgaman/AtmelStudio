/*
 * TrackerTWI.h
 *
 * Copied from Ofir's test with OTG1411
 */ 



#ifndef I2C_H_
#define I2C_H_

#include <asf.h>

#define TWI_SLAVE        TWIE
#define TWI_MASTER       TWIE
#define TWI_MASTER_PORT  PORTE
#define TWI_SPEED        400000

extern TWI_Slave_t slave;

void BoardSlave(void (*handler)(void), uint8_t slave_addr);
void BoardMaster();
void BoardTWIinit();
void BoardMaster100khz();

#endif /* TRACKERTWI_H_ */