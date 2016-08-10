/*
 * Endian.h
 *
 * Created: 04-Feb-15 5:03:33 PM
 *  Author: Saba
 */ 


#ifndef ENDIAN_H_
#define ENDIAN_H_

#define SWAP_ENDIAN_4(s) (((uint32_t)s&0xff000000)>>24)+((((uint32_t)s&0xff0000)>>16)<<8)+((((uint32_t)s&0xff00)>>8)<<16)+((((uint32_t)s&0xff))<<24)
#define SWAP_ENDIAN_2(s) (((uint16_t)s&0xff00)>>8) + (((uint16_t)s&0xff)<<8)

enum eEndian {BIG_ENDIAN,LITTLE_ENDIAN};
#endif /* ENDIAN_H_ */