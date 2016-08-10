/*
   Flash device on SPI
*/

#define CHIP_SELECT 10   // for EEPROM
#define BUFFER_ENABLE 9  // for SN54AHC125 buffer
#define PAGE_SIZE 256
//#define SECTOR_SIZE 4096
#define TOTAL_FLASH_SIZE 16777216L // 16MB
#define PAGE_BIT_MAP_START (TOTAL_FLASH_SIZE - 8192) // 8K from the top

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

//
// prototypes
//
void FlashInit(void);
void eraseAllFlash(void);
void notBusy(void);
uint8_t spi_transfer(uint8_t info);
void writeEnable(void);
bool spi_flash_check(void);
void WritePageBufferToFlash(void);
void sendCommandAndAddress (const uint8_t command, const unsigned long addr);
void write_to_flash(uint32_t addr, uint8_t * data, uint8_t len);
void updatePageBitMap(void);
bool flashIsFull(void);
uint32_t GetCurrentPageFromEEPROM(void);
void sendFlashOffsetAndLastRecordsToHost(void);
void read_from_flash(const unsigned long addr, uint8_t * data, unsigned int len);
void SendFlashDataToHost(void);
