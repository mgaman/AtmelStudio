/*
 * crypto.c
 *
 * Created: 12/07/2013 10:28:37
 *  Author: Mounir
 
 * David - take our private key instead of default
 */ 
#include <string.h>
#include <asf.h>
//#include "Application01/app1.h"
//#include "Application01/app1_settings.h"
//#include "Application01/AES_driver.h"
//#include "aes.h"
#include "crypto.h"
#include "EepromHandler.h"
#include "AES_driver.h"

#define BLOCK_LENGTH		16
#define MAX_DATA_LENGTH		128


/* AES encryption key */
uint8_t key[BLOCK_LENGTH] = {0x94, 0x74, 0xB8, 0xE8, 0xC7, 0x3B, 0xCA, 0x7D,
                             0x28, 0x34, 0x76, 0xAB, 0x38, 0xCF, 0x37, 0xC2};
/* i.e. AES decrytion key. */
uint8_t  lastsubkey[BLOCK_LENGTH];

/* Initialisation vector used during Cipher Block Chaining. */
uint8_t init[BLOCK_LENGTH] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
                              0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};

/* Plain text used during Cipher Block Chaining(contains 3 blocks). */
//uint8_t data_block[BLOCK_LENGTH * BLOCK_COUNT+1] =                        {0x04};


/* Variable used to store ciphertext from Cipher Block Chaining. */
//uint8_t  cipher_block[BLOCK_LENGTH * BLOCK_COUNT];

/* Variable used to store decrypted plaintext from Cipher Block Chaining. */
//uint8_t  block_ans[BLOCK_LENGTH * BLOCK_COUNT];

/* Variable used to check if decrypted answer is equal original data. */
bool success;

void AES_Init(void)
{
	/* Enable the AES clock. */
	
	aes_configure(AES_ENCRYPT, AES_MANUAL, AES_XOR_OFF);
	
	// take our private key
	memcpy((void *)key,ApplicationData.EncryptionKey,BLOCK_LENGTH);
}
	uint8_t pd[MAX_DATA_LENGTH];
	uint8_t pc[MAX_DATA_LENGTH];

bool AES_Encrypt(const uint8_t *pData,const uint16_t len,uint8_t *pCipher)
{	
	bool res = false;
	memset(pd,0,sizeof(pd));
	memcpy((void *)pd,pData,len);
	AES_software_reset();
	uint8_t blocks_count = (len / 16) +1;
	res = AES_CBC_encrypt(pd,pc,key,init,blocks_count); /* MODIFY THE FUNCTION CALL */			
	memcpy((void *)pCipher,pc,len);
	return res;
}



bool AES_Decrypt(const uint8_t *pCipher,const uint16_t len,uint8_t *pData)
{
	/* Before using the AES it is recommended to do an AES software reset to put
	 * the module in known state, in case other parts of your code has accessed
	 * the AES module. */
	bool res = false;
	memset(pc,0,sizeof(pc));
	memcpy((void *)pc,pCipher,len);	
	AES_software_reset();
	AES_lastsubkey_generate(key, lastsubkey);
	uint8_t blocks_count = (len / 16) +1;
	res = AES_CBC_decrypt(pc,pd,lastsubkey,init,blocks_count);; /* MODIFY THE FUNCTION CALL */
	memcpy((void *)pData,pd,len);	
	return res;
	
}