/*
   SIRF related structures and functions
*/

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "Endian.h"
#include "Sirflib.h"

static int crcsize;
static enum eParseState parseState;
static unsigned index;
uint16_t payloadsize, given_crc16,calc_crc16;
static struct sMID166 MID166msg;
static struct sMID4 MID4msg;

#define MAX_MSG_LENGTH 200
uint8_t payloadbuf[MAX_MSG_LENGTH];

uint16_t SirfCRC(uint8_t *msg,int msgLength, enum eEndian finalEndian)
{
  int index = 0;
  uint16_t crc = 0;
  while (index < msgLength)
  {
    crc += msg[index++];
    crc &= 0x7ffff;
  }
  if (finalEndian == LITTLE_ENDIAN)
    return SWAP_ENDIAN_2(crc);
  else
    return crc;
}

struct sMID166 *MID166(enum eMsgMode mode, uint8_t mid, uint8_t rate)
{
  MID166msg.header.a0 = 0xa0;
  MID166msg.header.a2 = 0xa2;
  MID166msg.header.szmsb = 0;
  MID166msg.header.szlsb = 8;  // 8 byte message 
  MID166msg.MID = 166;
  MID166msg.mode = mode;
  MID166msg.mid = mid;
  MID166msg.rate = rate;
  memset(MID166msg.filler,0,4);
  MID166msg.trailer.crc = SirfCRC(&MID166msg.MID,8,true);
  MID166msg.trailer.b0 = 0xb0;
  MID166msg.trailer.b3 = 0xb3;
  return &MID166msg;
}

void SirfParseInit(void)
{
  parseState = A0;
}

bool SirfBlockReady(uint8_t c)
{
  bool rc = false;
  switch (parseState)
  {
    case A0:
      if (c == 0xA0)
      {
        index = 0;
        payloadbuf[index++] = 0xA0;
        parseState = A2;
      }
      break;
    case A2:
      if (c == 0xA2)
      {
          payloadbuf[index++] = 0xA2;
          parseState = SZMSB;
      }
      else
           parseState = A0;
      break;
    case SZMSB:
      payloadbuf[index++] = c;
      payloadsize = c* 256;  // save payloadsize as little-endian
      parseState = SZLSB;
      break;
    case SZLSB:
      payloadbuf[index++] = c;
      payloadsize += c;
      crcsize = payloadsize;
      parseState = PAYLOAD;
      break;
    case PAYLOAD:
      payloadbuf[index++] = c;
      if (--payloadsize == 0)
        parseState = CRC1;
      break;
    case CRC1:
      payloadbuf[index++] = c;
      parseState = CRC2;
      given_crc16 = c;
      break;
    case CRC2:
      payloadbuf[index++] = c;
      given_crc16 += (c*256);
      // now calculate CRC of received message
      calc_crc16 = SirfCRC(&payloadbuf[4],crcsize,BIG_ENDIAN);  // compare as bigendian
      if (given_crc16 == calc_crc16)
        parseState = B0;
      else
        parseState = A0;	// bail out if wrong CRC
      break;
    case B0:
      payloadbuf[index++] = c;
      if (c == 0xB0)
        parseState = B3;
      else
        parseState = A0;
      break;
    case B3:
      payloadbuf[index++] = c;
      if (c == 0xB3)
        rc = true;
      parseState = A0;
      break;
  }
  return rc;
}
