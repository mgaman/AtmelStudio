/*
  For transporting messages up to 256 bytes long, including binary
*/

#define DLE 0x10
#define STX 0x02
#define ETX 0x03

// for wrapping large bundles prior to sending
uint8_t *SerialWrap(uint8_t *raw,uint32_t length,uint32_t *cookedlength);
// wrap a single byte and send it
#if defined (SERIAL_UART)
void SerialWrapSendByte(USART_t *uart,uint8_t b);
#elif defined(SERIAL_CDC)
void SerialWrapSendByte(uint8_t b);
#else
#error Define Serial Method
#endif

#ifdef PCMSGPARSE
uint8_t cookedmsg[520]; // allow for flash page sized messages, assuming not too many DLE
int cookedlength;
#else
extern uint8_t cookedmsg[]; // allow for flash page sized messages, assuming not too many DLE
extern int cookedlength;
#endif
