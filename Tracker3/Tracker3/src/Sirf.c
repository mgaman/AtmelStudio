/*
   SIRF related structures and functions
*/

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <asf.h>
#include "Sirf.h"

static int crcsize;
static enum eParseState parseState;
static uint32_t index;
uint16_t payloadsize, given_crc16,calc_crc16;
static struct sMID166 MID166msg;
//static struct sMID4 MID4msg;
static struct sMID132 MID132msg;
static struct sMID152 MID152msg;
static struct sMID136 MID136msg;

#define MAX_MSG_LENGTH 256 //_MAX_RX_FIFO_BUFFER_LENGTH
uint8_t payloadbuf[MAX_MSG_LENGTH];

uint16_t SirfCRC(uint8_t *msg,int msgLength, bool bswap)
{
  int ix = 0;
  uint16_t crc = 0;
  while (ix < msgLength)
  {
    crc += msg[ix++];
    crc &= 0x7fff;
  }
  if (bswap)
    return (crc >> 8) | (crc << 8);
  else
    return crc;
}
uint32_t BELE4(uint32_t s)
{
  return ((s&0xff000000)>>24)+(((s&0xff0000)>>16)<<8)+(((s&0xff00)>>8)<<16)+(((s&0xff))<<24);
}
uint16_t BELE2(uint16_t s)
{
  return ((s&0xff00)>>8) + ((s&0xff)<<8);
}

struct sMID166 *MID166(enum eMsgMode mode, uint8_t mid, uint8_t rate)
{
  MID166msg.header.a0 = 0xa0;
  MID166msg.header.a2 = 0xa2;
  MID166msg.header.szmsb = 0;
  MID166msg.header.szlsb = 1+sizeof(struct sMID166) - sizeof(struct sSirfHeader) - sizeof(struct sSirfTrailer); 
  MID166msg.header.MID = 166;
  MID166msg.mode = mode;
  MID166msg.mid = mid;
  MID166msg.rate = rate;
  memset(MID166msg.filler,0,4);
  MID166msg.trailer.crc = SirfCRC(&MID166msg.header.MID,MID166msg.header.szlsb,true);
  MID166msg.trailer.b0 = 0xb0;
  MID166msg.trailer.b3 = 0xb3;
  return &MID166msg;
}

struct sMID132 *MID132(void)
{
  MID132msg.header.a0 = 0xa0;
  MID132msg.header.a2 = 0xa2;
  MID132msg.header.szmsb = 0;
  MID132msg.header.szlsb = 1+sizeof(struct sMID132) - sizeof(struct sSirfHeader) - sizeof(struct sSirfTrailer);
  MID132msg.header.MID = 132;
  MID132msg.filler = 0;
  MID132msg.trailer.crc = SirfCRC(&MID132msg.header.MID,MID132msg.header.szlsb,true);
  MID132msg.trailer.b0 = 0xb0;
  MID132msg.trailer.b3 = 0xb3;
  return &MID132msg;
}

struct sMID152 *MID152(void)
{
  MID152msg.header.a0 = 0xa0;
  MID152msg.header.a2 = 0xa2;
  MID152msg.header.szmsb = 0;
  MID152msg.header.szlsb = 1+sizeof(struct sMID152) - sizeof(struct sSirfHeader) - sizeof(struct sSirfTrailer);  // 2 byte message 
  MID152msg.header.MID = 152;
  MID152msg.filler = 0;
  MID152msg.trailer.crc = SirfCRC(&MID152msg.header.MID,MID152msg.header.szlsb,true);
  MID152msg.trailer.b0 = 0xb0;
  MID152msg.trailer.b3 = 0xb3;
  return &MID152msg;
}

struct sMID136 *MID136()
{
  memset(&MID136msg,0,sizeof(struct sMID136));
  MID136msg.header.a0 = 0xa0;
  MID136msg.header.a2 = 0xa2;
  MID136msg.header.szmsb = 0;
  MID136msg.header.szlsb = 1+sizeof(struct sMID136) - sizeof(struct sSirfHeader) - sizeof(struct sSirfTrailer);  
  MID136msg.header.MID = 136;
  MID136msg.DegradedMode = 4; // no 1SV & 2SV
  MID136msg.PositionCalcMode = 0x1f; //FIVE_HZ_NAV_ENABLED;
  MID136msg.trailer.crc = SirfCRC(&MID136msg.header.MID,MID136msg.header.szlsb,true);
  MID136msg.trailer.b0 = 0xb0;
  MID136msg.trailer.b3 = 0xb3;
  return &MID136msg;
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
      payloadsize = c* 256;
      parseState = SZLSB;
      break;
    case SZLSB:
      payloadbuf[index++] = c;
      payloadsize += c;
      crcsize = payloadsize;
	  if (payloadsize > MAX_MSG_LENGTH)
		parseState = A0;
	  else
		parseState = PAYLOAD;
      break;
    case PAYLOAD:
	  if (index == 4 && c == 6)
		  c++;
      payloadbuf[index++] = c;
      if (--payloadsize == 0)
        parseState = CRC1;
      break;
    case CRC1:
      payloadbuf[index++] = c;
      parseState = CRC2;
      given_crc16 = c*256;
      break;
    case CRC2:
      payloadbuf[index++] = c;
      given_crc16 += c;
      // now calculate CRC of received message
      calc_crc16 = SirfCRC(&payloadbuf[4],crcsize,false);  // return as LE
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

struct sSirfHeader *SirfData()
{
  return (struct sSirfHeader *)payloadbuf;
}
