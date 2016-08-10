/*
    MS5611 definitions
*/

#define CSHIGH   // comment out if not set high
#ifdef CSHIGH
#define MS5611_SLAVE_ADDRESS 0x76
#else
#define MS5611_SLAVE_ADDRESS 0x77
#endif
//
//  commands
//
#define CMD_RESET 0x1E // ADC reset command
#define CMD_ADC_READ 0x00 // ADC read command
#define CMD_ADC_CONV 0x40 // ADC conversion command
#define CMD_ADC_D1 0x00 // ADC D1 conversion
#define CMD_ADC_D2 0x10 // ADC D2 conversion
#define CMD_ADC_256 0x00 // ADC OSR=256
#define CMD_ADC_512 0x02 // ADC OSR=512
#define CMD_ADC_1024 0x04 // ADC OSR=1024
#define CMD_ADC_2048 0x06 // ADC OSR=2048
#define CMD_ADC_4096 0x08 // ADC OSR=4096
#define CMD_PROM_RD 0xA0 // Prom read command


//
//  prototypes
//
bool MS561101BA03Reset(void);
bool MS561101BA03Ready;
uint32_t MS561101BA03ReadADC(uint8_t command);
void MS561101BA03FormatAltimeter(uint32_t D1,uint32_t D2,double *Temp,double *Press);
