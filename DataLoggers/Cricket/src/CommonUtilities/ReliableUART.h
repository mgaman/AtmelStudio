/*
  For transporting messages up to 256 bytes long, including binary
*/

#define DLE 0x10
#define ETX 0x03

// for wrapping large bundles prior to sending
uint8_t *UartWrap(uint8_t *raw,uint32_t length,uint32_t *cookedlength);
// wrap a single byte and send it
void UartWrapSendByte(USART_t *uart,uint8_t b);
#ifdef PCMSGPARSE
char cookedmsg[520]; // allow for flash page sized messages, assuming not too many DLE
int cookedlength;
#else
extern char cookedmsg[]; // allow for flash page sized messages, assuming not too many DLE
extern int cookedlength;
#endif
