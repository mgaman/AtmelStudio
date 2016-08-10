#ifndef SI4432_h
#define SI4432_h



#define _MAX_TRANCEIVERS	 4
#define _MAX_RECEIVER_BUFFER 64


typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;




//RF stack enumerations
typedef enum _RF_ENUM
{
	RF_OK				=	0x00,			//function response parameters
	RF_ERROR_TIMING		=	0x01,
	RF_ERROR_PARAMETER	=	0x02,
	RF_PACKET_RECEIVED	= 	0x03,
	RF_NO_PACKET		=	0x04,
	RF_CRC_ERROR		=   0x05,
	RF_TX_MODE          =   0x0A,
	RF_RX_MODE          =   0x0B,
	
} RF_ENUM;

// Si443x registers
#define R_DEVICE_TYPE 0x0
#define R_DEVICE_VERSION 0x1
#define R_DEVICE_STATUS 0x2
#define R_OP_CTRL_1 0X7
#define R_OP_CTRL_2 0X8
#define R_RSSI 0x26





/*Init functions for handling the SPI interface*/
extern uint8_t (*pbSpiReadWrite)(uint8_t data_in);
extern void (*vDelay_MS)(uint32_t ms);

/************************************************************************************************************************************************/
// Function RF_Init
// Arguments		: ============================================================================================================================
// uint8 id						: index of the transceiver, 0-3
// uint8 Settings[]				: Settings array
// uint8_t SettingsCount		: Number of Settings in settings array
// uint8_t *pBuffer				: allocated buffer to hold received data
// uint8_t BufferSize			: allocated buffer size to hold received data
// uint8_t *pDataLength			: pointer to variable that will hold the last received packet length
// uint32_t *pmscounter			: pointer to the ms timer variable
// bool (*IsInterrupt)(void)	: pointer to function that returns the interrupt pin status
// void (*vChipSelect)(void)	: pointer to function that select the cs for the trancsceiver
// void (*pHandler)(uint8_t *Buffer,uint8_t Length) : pointer to function that will be called when valid packet is received
// Return			: ============================================================================================================================
// RF_ENUM according to chip init status
/************************************************************************************************************************************************/
RF_ENUM RF_Init(uint8 id, uint8 Settings[], uint8_t SettingsCount, uint8_t *pBuffer, uint8_t BufferSize, uint8_t *pDataLength, uint32_t *pmscounter,
bool (*IsInterrupt)(void),
void (*vChipSelect)(void),
void (*vChipUnSelect)(void),
void (*pHandler)(uint8_t deviceId,uint8_t *Buffer,uint8_t Length));
/************************************************************************************************************************************************/
// RFTransmit
// Arguments		: ============================================================================================================================
// uint8 id			: index of the transceiver, 0-3
// uint8 * packet	: packet to transmit
// uint8 length		: packet length to transmit
// Return			: ============================================================================================================================
// RF_ENUM according to transmit status
/************************************************************************************************************************************************/
RF_ENUM RFTransmit(uint8 id,uint8 * packet, uint8 length);

/************************************************************************************************************************************************/
// RFIdle
// Arguments		: ============================================================================================================================
// uint8 id			: index of the transceiver, 0-3
// Return			: ============================================================================================================================
// RF_ENUM
/************************************************************************************************************************************************/
RF_ENUM RFIdle(uint8 id);


/************************************************************************************************************************************************/
// RFInitReceiver		Set Transceiver to receive mode
// Arguments		: ============================================================================================================================
// uint8 id			: index of the transceiver, 0-3
// Return			: ============================================================================================================================
// RF_ENUM
/************************************************************************************************************************************************/
RF_ENUM RFInitReceiver(uint8 id);

/************************************************************************************************************************************************/
// RFReceiveMode_Handler		Set Transceiver to receive mode
// Arguments		: ============================================================================================================================
// uint8 ReceiverID	: index of the transceiver, 0-3
// Return			: ============================================================================================================================
// void
/************************************************************************************************************************************************/
void RFReceiveMode_Handler(uint8 ReceiverID);

/************************************************************************************************************************************************/
// RFInitTransmiter		Set Transceiver to transmitter mode
// Arguments		: ============================================================================================================================
// uint8 id			: index of the transceiver, 0-3
// Return			: ============================================================================================================================
// void
/************************************************************************************************************************************************/
void RFInitTransmiter(uint8 id);


/************************************************************************************************************************************************/
// SPI2RF_Write/SPI2RF_Read :		Write or Read set of registers
// Arguments		: ============================================================================================================================
// uint8 index		: index of the transceiver, 0-3
// uint8_t addr		: Register address to start reading or writing
// uint8_t *data	: pointer to byte array of values to be written or to store read values
// uint8_t len		: length of data to read/write
// Return			: ============================================================================================================================
// bool				: true = operation succeeded
/************************************************************************************************************************************************/
bool SPI2RF_Write(uint8_t index,uint8_t addr,uint8_t *data,uint8_t len);
bool SPI2RF_Read(uint8_t index,uint8_t addr,uint8_t *data,uint8_t len);
/************************************************************************************************************************************************/

/************************************************************************************************************************************************/
void GetLibVersion(uint8_t *pVerMajor, uint8_t *pVerMinor);
/************************************************************************************************************************************************/

#endif
