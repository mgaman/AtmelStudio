/*
 * crypto.h
 *
 * Created: 12/07/2013 10:28:50
 *  Author: Mounir
 */ 


#ifndef CRYPTO_H_
#define CRYPTO_H_


bool AES_Encrypt(const uint8_t *pData,const uint16_t len,uint8_t *pCipher);
bool AES_Decrypt(const uint8_t *pCipher,const uint16_t len,uint8_t *pData);
void AES_Init(void);

#endif /* CRYPTO_H_ */