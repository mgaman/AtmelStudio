/*
  For transporting messages of any length, including binary
*/

#define DLE 0x10
#define STX 0x02
#define ETX 0x03
enum  _MsgState {
            MSG_NONE,
            MSG_READ_STX,
            MSG_READ_LENGTH,
            MSG_READ_PAYLOAD,
            MSG_READ_CRC16_1,
            MSG_READ_CRC16_2,
            MSG_READ_END,
            MSG_READ_ENDED,
            MSG_READ_ETX,
            MSG_READY
        };

struct eMessage
{
	uint8_t body[500];
	bool ready;
	enum _MsgState MsgState;
	int MSG_I;
	bool DLE_IN_MSG;
} ;
// for wrapping large bundles prior to sending
uint8_t *HDLCStuff(uint8_t *raw,uint32_t length,uint32_t *cookedlength);
void HDLCInit(void);
// add data to parser, return true when a stuffed message has arrived
bool HDLCParse(uint8_t PcByte);
// copy unstuffed message to work area, which must be big enough
uint8_t HDLCUnStuff(uint8_t *work);
#ifdef PCMSGPARSE
uint8_t cookedmsg[520]; // allow for flash page sized messages, assuming not too many DLE
int cookedlength;
struct eMessage HDLCStatus;
#else
extern uint8_t cookedmsg[]; // allow for flash page sized messages, assuming not too many DLE
extern int cookedlength;
extern struct eMessage HDLCStatus;
#endif

