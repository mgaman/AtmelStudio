/*
 * EepromHandler.h
 *
 * Created: 11/18/2013 5:45:50 PM
 *  Author: pc
 */ 


#ifndef EEPROMHANDLER_H_
#define EEPROMHANDLER_H_

// Structure of EEPROM data, in this order
struct sAppData {
	uint32_t	MacID;
	uint8_t		EncryptionKey[16];
	uint8_t		MaxTransmissionsNumber;
	uint8_t		IntervalBetweenTransmissions;
	uint32_t	spare1;
	uint32_t	spare2;
};

struct sAppDataCS 
{
	struct sAppData data;
	uint16_t checksum;
};
	
tRFparameters RFparameters;	// configuration data from Munirs S2RF utility

struct sRFdataCS 
{
	tRFparameters data;
	uint16_t checksum;
};
	
#define EEPROM_BASE 10
#define _EEPROM_ADDR_SETTINGS EEPROM_BASE
#define EEPROM_RFDATA_BASE 100

#define EEPROM_ACTIVATION_CODES_BASE 50

#ifdef EEPROM_MAIN
struct sAppData ApplicationData;
//struct sAppData ApplicationDataDefault = {1000,"Encryption Key 0",5,1,0,0};
struct sAppData ApplicationDataDefault = {1000,
	                                     {0x51,0x89,0xba,0xcc,0x04,0x3f,0x9a,0x11,0x88,0xa7,0xd4,0x48,0x3a,0x66,0x2b,0x54},
										  5,1,0,0}; // something less obvious
#else
extern struct sAppData ApplicationData;
extern struct sAppData ApplicationDataDefault;
#endif



// EPROM, arbitrary choice of page, XMEGA4AU has 32 pages of 32 bytes, we need 3 consecutive pages
// structure of activation_codes EEPROM page

#define EEPROM_PARAMETER_PAGE 30
#define GROUP_OFFSET           8

#define ACTIVATION_CODE_OFFSET 0
#define BLINKS_NUMBER_OFFSET   1
#define BLINKS_INTERVAL_OFFSET 2

//#define BLINKS_TIME_OFFSET     3
//#define LISTEN_INTERVAL_OFFSET 4

#define NR_OF_CONTROLED_GAHLI  4

struct gahliParams 
{
	uint8_t	ActivationCode;
	uint8_t	BlinksNumber;
	uint8_t	BlinksInterval;
	uint8_t	BlinksTime;
	uint8_t	ListenTime;
	//uint8_t	spare1;
	//uint8_t	spare2;
};

//typedef uint8_t tRFparameters[_MAX_SI_SETTINGS][RF_MAX_NUMBER_OF_DEVICES];

//struct sAppData ApplicationData;
//struct sAppData ApplicationDataDefault = {1000,"Encryption Key 0",5,1,0,0};
	
	
//struct gahliParams GahliliotParamsDefault[MAX_GAHLI_FOR_SHALAT] = {
																	//{0x0A, 6, 7, 8, 9},
																	//{0x0B,16,17,18,19},
																	//{0x0C,26,27,28,29},
																	//{0x0C,36,37,38,39},
																  //};


// prototypes
void SaveToEEPROM_WithCS(uint16_t addr, uint8_t* p, uint16_t size);
bool ReadFromEEPROM_WithCS(uint16_t addr, uint8_t* p, uint16_t size);

void ReadEEPROMByte(uint16_t Address, uint8_t *Value);
void WriteEEPROMByte(uint16_t Address, uint8_t Value);

void ReadEEPROM(uint16_t Address, uint8_t *Buf, uint16_t Size);
void StoreEEPROM(uint16_t Address, uint8_t *Buf, uint16_t Size);

void ReadEEPROMFromPage(unsigned page, unsigned offset, uint8_t *buffer, unsigned length);
void WriteToEEPROMPage(unsigned page, unsigned offset, uint8_t *buffer, unsigned length);

#endif /* EEPROMHANDLER_H_ */