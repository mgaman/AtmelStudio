/*
 * EEPROM.h
 *
 * Created: 7/30/2013 2:34:26 PM
 *  Author: pc
 */ 


#ifndef EEPROM_H_
#define EEPROM_H_
struct sEEProm
{
	// common to all nodes
	uint8_t NodeID;
	uint16_t MacAddress;
	uint8_t project;
	uint8_t IamRemote;	// set 0 for master controller, not zero for remotes
	uint8_t MessageRepeatCount;
	uint16_t MessageRepeatDelay;	// millsecs
	uint16_t WakingPeriod;
	uint16_t SleepingPeriod;
	// firefly specific
	uint16_t FireflyBlinkOnPeriod;	// millsecs
	uint16_t FireflyBlinkOffPeriod;	// millsecs
	uint8_t FireflyBlinkCount;
	uint8_t FireflyGroup;
	// yuvalit specific
	uint16_t TxTimeout;				// millsecs
};

void ReadEEPROM(unsigned page,unsigned offset,uint8_t *buffer,unsigned length);
void WriteEEPROM(unsigned page,unsigned offset,uint8_t *buffer,unsigned length);

#ifdef EEPROM_MAIN
volatile struct sEEProm eeprom;
#else
extern volatile struct sEEProm eeprom;
#endif

#endif /* EEPROM_H_ */