/*
 * SpiFlash.h
 *
 * Created: 6/18/2014 4:08:58 PM
 *  Author: User
 */ 


#ifndef SPIFLASH_H_
#define SPIFLASH_H_

#define CHIP_SELECT 10   // for EEPROM
#define BUFFER_ENABLE 9  // for SN54AHC125 buffer
#define PAGE_SIZE 256
//#define SECTOR_SIZE 4096
#define TOTAL_FLASH_SIZE 16777216L // 16MB
#define PAGE_BIT_MAP_START (TOTAL_FLASH_SIZE - 8192) // 8K from the top

//#define MEMORY_SIZE 16711680     //16777216

// AT25DF041A EEPROM commands

// reading
#define ReadArray             0x0B
#define ReadArrayLowFrequency 0x03

// programming
#define BlockErase4Kb       0x20
#define BlockErase32Kb      0x52
#define BlockErase64Kb      0xD8
#define ChipErase           0x60
#define PageProgram         0x02
#define SequentialProgram   0xAD

// protection
#define WriteEnable           0x06
#define WriteDisable          0x04
#define ProtectSector         0x36
#define UnProtectSector       0x39
#define ReadSectorProtection  0x3C

// status
#define ReadStatus 0x05
#define WriteStatus 0x01

// miscellaneous
#define ReadManufacturer     0x9F
#define DeepPowerDown        0xB9
#define ResumeFromPowerDown  0xAB


#define	SPI_FLASH_MANUFACTURER_ID  0x90
#define	SPI_FLASH_UNIQUE_ID        0x4B

//#define FLASH_BLOCK_LENTGH 256


//uint16_t offset;

extern bool memoryFull;

bool request_for_flash_data;

//
//struct spi_device spi_device_conf =
//{
	//.id = IOPORT_CREATE_PIN(PORTD, 4)
//};

 //struct spi_device asf_spi_device_conf =
 //{
    //.id = IOPORT_CREATE_PIN(PORTD, 4 /*1*/)
 //};

uint8_t spi_transfer(uint8_t dataAddress);
bool spi_flash_check(void);
void SPI_Init(void);

void write_to_flash(const unsigned long addr, uint8_t * data, uint8_t len);
void read_from_flash(const unsigned long addr, uint8_t * data, unsigned int len);
void erase_flash_block(const unsigned long addr);
void eraseFlashWithParam(uint8_t eraseType, const unsigned long addr);

void WriteSensorsDataToFlash(void);

//void readOffsetFromEeprom(void);
uint32_t GetCurrentPageFromEEPROM(void);
void eraseAllFlash(void);

void saveOffsetToEeprom(void);
void saveRecordsToEeprom(unsigned long recVal, unsigned long epromAddr);
unsigned long readRecordsFromEeprom(unsigned long epromAddr);
void sendFlashOffsetAndLastRecordsToHost(void);
void updatePageBitMap(void);

void WriteGpsDataToFlash(void);
void WriteLsmDataToFlash(void);
void WriteVarioDataToFlash(void);

void SendFlashDataToHost(void);
void readSampleFlashData(void);

void deleteOffsetInEeprom(void);
void deleteOffsetAndFlash(void);
void cleanRecordsInEeprom(void);

void WritePageBufferToFlash(void);

void flashReadWriteTest(void);

void readOffsetFromFlash(void);

#define GPS_REC_ADDR 0x330
#define LSM_REC_ADDR 0x350
#define VAR_REC_ADDR 0x370

#endif /* SPIFLASH_H_ */