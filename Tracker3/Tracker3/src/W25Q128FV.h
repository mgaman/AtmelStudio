/*
	W25Q128FVEIG  registers and instructions
*/

#define CHIP_SELECT 10   // for flash
#define BUFFER_ENABLE 9  // for SN54AHC125 buffer
#define PAGE_SIZE 256
#define TOTAL_FLASH_SIZE 16777216L // 16MB
#define PAGE_BIT_MAP_START (TOTAL_FLASH_SIZE - 8192) // 8K from the top

// AT25DF041A flash commands

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
#define WINBOND_MANUF_ID 0xEF
#define WINBOND_DEVICE_ID 0x17

bool W25Q128Init(void);
void W25Q128EraseAllFlash(void);
void WritePageBufferToFlash(void);
uint32_t GetNextAvailablePage(void);
bool flashIsFull(void);
//void sendFlashOffsetAndLastRecordsToHost(void);
void SendFlashDataToHost(void);

uint8_t spi_transfer(uint8_t);
void notBusy(void);
void writeEnable(void);
void sendCommandAndAddress (const uint8_t command, const unsigned long addr);
void read_from_flash(const unsigned long addr, uint8_t * data, unsigned int len);
void write_to_flash(const unsigned long addr, uint8_t * data, uint8_t len);
void updatePageBitMap(void);
