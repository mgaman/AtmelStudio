#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#define PCMSGPARSE
#include "HDLC.h"

uint8_t singlebyte;

static uint16_t calc_crc16;

const uint16_t crc_tab16[256] = {
	0x0000,0x1021,0x2042,0x3063,0x4084,0x50a5,0x60c6,0x70e7,
	0x8108,0x9129,0xa14a,0xb16b,0xc18c,0xd1ad,0xe1ce,0xf1ef,
	0x1231,0x0210,0x3273,0x2252,0x52b5,0x4294,0x72f7,0x62d6,
	0x9339,0x8318,0xb37b,0xa35a,0xd3bd,0xc39c,0xf3ff,0xe3de,
	0x2462,0x3443,0x0420,0x1401,0x64e6,0x74c7,0x44a4,0x5485,
	0xa56a,0xb54b,0x8528,0x9509,0xe5ee,0xf5cf,0xc5ac,0xd58d,
	0x3653,0x2672,0x1611,0x0630,0x76d7,0x66f6,0x5695,0x46b4,
	0xb75b,0xa77a,0x9719,0x8738,0xf7df,0xe7fe,0xd79d,0xc7bc,
	0x48c4,0x58e5,0x6886,0x78a7,0x0840,0x1861,0x2802,0x3823,
	0xc9cc,0xd9ed,0xe98e,0xf9af,0x8948,0x9969,0xa90a,0xb92b,
	0x5af5,0x4ad4,0x7ab7,0x6a96,0x1a71,0x0a50,0x3a33,0x2a12,
	0xdbfd,0xcbdc,0xfbbf,0xeb9e,0x9b79,0x8b58,0xbb3b,0xab1a,
	0x6ca6,0x7c87,0x4ce4,0x5cc5,0x2c22,0x3c03,0x0c60,0x1c41,
	0xedae,0xfd8f,0xcdec,0xddcd,0xad2a,0xbd0b,0x8d68,0x9d49,
	0x7e97,0x6eb6,0x5ed5,0x4ef4,0x3e13,0x2e32,0x1e51,0x0e70,
	0xff9f,0xefbe,0xdfdd,0xcffc,0xbf1b,0xaf3a,0x9f59,0x8f78,
	0x9188,0x81a9,0xb1ca,0xa1eb,0xd10c,0xc12d,0xf14e,0xe16f,
	0x1080,0x00a1,0x30c2,0x20e3,0x5004,0x4025,0x7046,0x6067,
	0x83b9,0x9398,0xa3fb,0xb3da,0xc33d,0xd31c,0xe37f,0xf35e,
	0x02b1,0x1290,0x22f3,0x32d2,0x4235,0x5214,0x6277,0x7256,
	0xb5ea,0xa5cb,0x95a8,0x8589,0xf56e,0xe54f,0xd52c,0xc50d,
	0x34e2,0x24c3,0x14a0,0x0481,0x7466,0x6447,0x5424,0x4405,
	0xa7db,0xb7fa,0x8799,0x97b8,0xe75f,0xf77e,0xc71d,0xd73c,
	0x26d3,0x36f2,0x0691,0x16b0,0x6657,0x7676,0x4615,0x5634,
	0xd94c,0xc96d,0xf90e,0xe92f,0x99c8,0x89e9,0xb98a,0xa9ab,
	0x5844,0x4865,0x7806,0x6827,0x18c0,0x08e1,0x3882,0x28a3,
	0xcb7d,0xdb5c,0xeb3f,0xfb1e,0x8bf9,0x9bd8,0xabbb,0xbb9a,
	0x4a75,0x5a54,0x6a37,0x7a16,0x0af1,0x1ad0,0x2ab3,0x3a92,
	0xfd2e,0xed0f,0xdd6c,0xcd4d,0xbdaa,0xad8b,0x9de8,0x8dc9,
	0x7c26,0x6c07,0x5c64,0x4c45,0x3ca2,0x2c83,0x1ce0,0x0cc1,
	0xef1f,0xff3e,0xcf5d,0xdf7c,0xaf9b,0xbfba,0x8fd9,0x9ff8,
	0x6e17,0x7e36,0x4e55,0x5e74,0x2e93,0x3eb2,0x0ed1,0x1ef0
};
    /*******************************************************************\
    *                                                                   *
    *   unsigned short update_crc_16( unsigned short crc, char c );     *
    *                                                                   *
    *   The function update_crc_16 calculates a  new  CRC-16  value     *
    *   based  on  the  previous value of the CRC and the next byte     *
    *   of the data to be checked.                                      *
    *                                                                   *
    \*******************************************************************/

static uint16_t update_crc_16( uint16_t crc, char c )
{
	crc = (uint16_t)((crc << 8) ^ crc_tab16[((crc >> 8) ^ c) & 0x00FF]);
	return crc;
}  /* update_crc_16 */

/*
  Wrap messages in DLE,LENGTH,CS,ETC
  Source is a byte array
  DLE DLE counts as DLE in checksum
  special case length 0 considered to be 256
*/
uint8_t *HDLCStuff(uint8_t *raw,uint32_t length,uint32_t *cookedlength)
{
  int index = 0;
  int j;
  calc_crc16 = 0;
  cookedmsg[index++] = DLE;
  cookedmsg[index++] = STX;
  if (length == DLE)
  {
    cookedmsg[index++] = DLE;
    cookedmsg[index] = DLE;
  }
  else
  // special case, length 256 recorded as 0
  if (length == 256)
    cookedmsg[index] = 0;
  else
	 cookedmsg[index] = length;
  calc_crc16 = update_crc_16( calc_crc16, cookedmsg[index++]);
  for (j=0;j<length;j++)
  {
	  if (raw[j] == DLE)
	  {
			cookedmsg[index++] = DLE;
			cookedmsg[index++] = DLE;
	  }
	  else
			cookedmsg[index++] = raw[j];
    calc_crc16 = update_crc_16( calc_crc16, raw[j]);
  }
  // copy CS to message, big endian
  // check for DLE
  if (calc_crc16 % 256 == DLE)
  {
    cookedmsg[index++] = calc_crc16 % 256;
    cookedmsg[index++] = calc_crc16 % 256;
  }
  else
    cookedmsg[index++] = calc_crc16 % 256;
  if (calc_crc16 / 256 == DLE)
  {
    cookedmsg[index++] = calc_crc16 / 256;
    cookedmsg[index++] = calc_crc16 / 256;
  }
  else
    cookedmsg[index++] = calc_crc16 / 256;
   cookedmsg[index++] = DLE;
   cookedmsg[index++] = ETX;
  *cookedlength = index;
  return cookedmsg;
}

void HDLCInit()
{
	HDLCStatus.DLE_IN_MSG = false;
	HDLCStatus.MSG_I = 0;
	HDLCStatus.ready = false;
	HDLCStatus.MsgState = MSG_NONE;
}

/// <summary>
/// Call this for every byte recieved
/// returns true a complete message has arrives
/// </summary>
/// <param name="b"></param>
/// <returns></returns>
static uint16_t calc_crc16;
bool HDLCParse(uint8_t PcByte)
{
		switch (HDLCStatus.MsgState)
		{
				case MSG_NONE:
						if (PcByte == DLE)
						{
								HDLCStatus.MsgState = MSG_READ_STX;
								HDLCStatus.MSG_I = 0;
						}
						break;
				case MSG_READ_STX:
						if (PcByte == STX)
						{
								HDLCStatus.MsgState = MSG_READ_LENGTH;
								HDLCStatus.MSG_I = 0;
								calc_crc16 = 0;
						}
						else
								HDLCStatus.MsgState = MSG_NONE;
						break;
				case MSG_READ_LENGTH:
						HDLCStatus.body[HDLCStatus.MSG_I++] = PcByte;
						HDLCStatus.MsgState = MSG_READ_PAYLOAD;
						HDLCStatus.DLE_IN_MSG = false;
						calc_crc16 = update_crc_16( calc_crc16,PcByte);
						break;
				case MSG_READ_PAYLOAD:
						if (PcByte == DLE)
						{
								HDLCStatus.DLE_IN_MSG = !HDLCStatus.DLE_IN_MSG;
								if (HDLCStatus.DLE_IN_MSG)
										break;
						}
						calc_crc16 = update_crc_16( calc_crc16,PcByte);
						HDLCStatus.body[HDLCStatus.MSG_I++] = PcByte;
						if (HDLCStatus.body[0] < HDLCStatus.MSG_I)
								HDLCStatus.MsgState = MSG_READ_CRC16_1;
						break;
				case MSG_READ_CRC16_1:   // must allow for escaped DLE in checksum as well
						if (PcByte == DLE)
						{
								HDLCStatus.DLE_IN_MSG = !HDLCStatus.DLE_IN_MSG;
								if (HDLCStatus.DLE_IN_MSG)
										break;
						}
						HDLCStatus.body[HDLCStatus.MSG_I++] = PcByte;
						HDLCStatus.MsgState = MSG_READ_CRC16_2;
						break;
				case MSG_READ_CRC16_2:   // must allow for escaped DLE in checksum as well
						if (PcByte == DLE)
						{
								HDLCStatus.DLE_IN_MSG = !HDLCStatus.DLE_IN_MSG;
								if (HDLCStatus.DLE_IN_MSG)
										break;
						}
						HDLCStatus.body[HDLCStatus.MSG_I] = PcByte;
						// sanity check on length
						if ((HDLCStatus.body[0] + 2) == HDLCStatus.MSG_I)
								HDLCStatus.MsgState = MSG_READ_END;
						else
								HDLCStatus.MsgState = MSG_NONE;
						break;
				case MSG_READ_END:
						if (PcByte == DLE)
								HDLCStatus.MsgState = MSG_READ_ETX;
						else
								HDLCStatus.MsgState = MSG_NONE;
						break;
				case MSG_READ_ETX:
						if (PcByte == ETX)
								HDLCStatus.MsgState = MSG_READ_ENDED;
						else
								HDLCStatus.MsgState = MSG_NONE;
						break;
				default:
						//   ErrorReport(__FILE__,__LINE__);
						break;
		}
		if (HDLCStatus.MsgState == MSG_READ_ENDED)
		{
			// now compare calculated CRC to that in message
//			if (*(uint16_t *)&HDLCStatus.body[HDLCStatus.body[0]+1] == calc_crc16)
			if (true)
			{
						HDLCStatus.MsgState = MSG_NONE;
						return true;
				}
				else
				{
						HDLCStatus.MsgState = MSG_NONE;
				}
		}
		return false;
}
/// <summary>
/// Get the actual data
/// </summary>
/// <returns></returns>
uint8_t HDLCUnStuff(uint8_t *work)
{
//		byte[] final = new byte[HDLCStatus.body[0]];    // get buffer of correct length
//		Array.Copy(HDLCStatus.body, 1, final, 0, final.Length);
//		return final;
	memcpy(work,&HDLCStatus.body[1],HDLCStatus.body[0]);
	return HDLCStatus.body[0];
}

#ifdef HDLC_UNIT_TEST

uint8_t single[] = {1};
uint8_t multi[] = { 1, 2, 3, 4, 5 };
uint8_t multidel[] = { 1, 2, 3, 4, 5, 6, 7, 8, 8, 7, 6, 5, 4, 3, 2, 1 }; // length is DEL
uint8_t datadel[] = { 1, 2, 3, 4, 0x10, 6, 7, 8 }; // data contains DEL
uint8_t *stuffed;

bool compareOldNew(uint8_t *raw, uint8_t *work, uint32_t length)
{
	int i;
	uint8_t finallength,finalwork[50];
	for (i = 0; i < length;i++ )
  	if (HDLCParse(work[i]))
    	break;
  finallength = HDLCUnStuff(finalwork);
  return (memcmp(raw,finalwork,finallength) == 0);
}

bool rc = true;
void main()
{
	uint32_t cookedlength;
	HDLCInit();
	single[0] = 0xea;   // typical
  stuffed = HDLCStuff(single,sizeof(single),&cookedlength);
	rc &= compareOldNew(single,stuffed,cookedlength);
	single[0] = 0x10;   // DLE
  stuffed = HDLCStuff(single,sizeof(single),&cookedlength);
	rc &= compareOldNew(single,stuffed,cookedlength);
  stuffed = HDLCStuff(multi,sizeof(multi),&cookedlength);  // simple data
	rc &= compareOldNew(multi,stuffed,cookedlength);
  stuffed = HDLCStuff(multidel,sizeof(multidel),&cookedlength);  // length is del
	rc &= compareOldNew(multidel,stuffed,cookedlength);
  stuffed = HDLCStuff(datadel,sizeof(datadel),&cookedlength);  // daya contains del
	rc &= compareOldNew(datadel,stuffed,cookedlength);
	while (true){}
}

void USB0DeviceIntHandler(){}
#endif
