/*
 * OTG1411.h
 *
 * Created: 04-Feb-15 11:28:11 PM
 *  Author: Saba
 */ 


#ifndef OTG1411_H_
#define OTG1411_H_

#define ORG1411_MASTER_ADDR	 0x60
#define ORG1411_SLAVE_ADDR	 0x62

void On_Off_Pulse(void);
bool ORG1411Reset(void);
void ORG1411SlaveHandler(void) ;
void NMEAParser(uint8_t c);

#endif /* OTG1411_H_ */