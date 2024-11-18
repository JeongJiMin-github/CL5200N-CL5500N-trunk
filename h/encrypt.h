/*****************************************************************************
|*		       
|*  Copyright		:	(c) 2013 CAS
|*  Filename		:	encrypt.h
|*  Version		:	1.0
|*  Programmer(s)		:	cjk
|*  Created		:	2013/03/18
|*  Description		:	
|*              				
****************************************************************************/

#ifdef USE_DES_ENCRYPTION

extern void des_encrypt(const unsigned char *plaintext, unsigned char *ciphertext, const unsigned char *key, int len);
extern void des_decrypt(const unsigned char *ciphertext, unsigned char *plaintext, const unsigned char *key, int len);


extern void base64e(INT8U *src, INT8U *result, INT16U length);
extern void base64d(INT8U *src, INT8U *result, INT16U *length);

extern void pkcs5padding(unsigned char *src, int direction);

#endif
