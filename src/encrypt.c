/*****************************************************************************
*                                                                            *
*  --------------------------------- des.c --------------------------------  *
*                                                                            *
*****************************************************************************/

#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "globals.h"
#include "encrypt.h"
#include "common.h"

#ifdef USE_DES_ENCRYPTION


/*****************************************************************************
*                                                                            *
*  Define a mapping for the key transformation.                              *
*                                                                            *
*****************************************************************************/

static const int DesTransform[56] = {

   57, 49, 41, 33, 25, 17,  9,  1, 58, 50, 42, 34, 26, 18,
   10,  2, 59, 51, 43, 35, 27, 19, 11,  3, 60, 52, 44, 36,
   63, 55, 47, 39, 31, 23, 15,  7, 62, 54, 46, 38, 30, 22,
   14,  6, 61, 53, 45, 37, 29, 21, 13,  5, 28, 20, 12,  4

};

/*****************************************************************************
*                                                                            *
*  Define the number of rotations for computing subkeys.                     *
*                                                                            *
*****************************************************************************/

static const int DesRotations[16] = {

   1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1

};

/*****************************************************************************
*                                                                            *
*  Define a mapping for the permuted choice for subkeys.                     *
*                                                                            *
*****************************************************************************/

static const int DesPermuted[48] = {

   14, 17, 11, 24,  1,  5,  3, 28, 15,  6, 21, 10,
   23, 19, 12,  4, 26,  8, 16,  7, 27, 20, 13,  2,
   41, 52, 31, 37, 47, 55, 30, 40, 51, 45, 33, 48,
   44, 49, 39, 56, 34, 53, 46, 42, 50, 36, 29, 32

};

/*****************************************************************************
*                                                                            *
*  Define a mapping for the initial permutation of data blocks.              *
*                                                                            *
*****************************************************************************/

static const int DesInitial[64] = {

   58, 50, 42, 34, 26, 18, 10,  2, 60, 52, 44, 36, 28, 20, 12,  4,
   62, 54, 46, 38, 30, 22, 14,  6, 64, 56, 48, 40, 32, 24, 16,  8,
   57, 49, 41, 33, 25, 17,  9,  1, 59, 51, 43, 35, 27, 19, 11,  3,
   61, 53, 45, 37, 29, 21, 13,  5, 63, 55, 47, 39, 31, 23, 15,  7

};

/*****************************************************************************
*                                                                            *
*  Define a mapping for the expansion permutation of data blocks.            *
*                                                                            *
*****************************************************************************/

static const int DesExpansion[48] = {

   32,  1,  2,  3,  4,  5,  4,  5,  6,  7,  8,  9,
    8,  9, 10, 11, 12, 13, 12, 13, 14, 15, 16, 17,
   16, 17, 18, 19, 20, 21, 20, 21, 22, 23, 24, 25,
   24, 25, 26, 27, 28, 29, 28, 29, 30, 31, 32,  1

};

/*****************************************************************************
*                                                                            *
*  Define tables for the S-box substitutions performed for data blocks.      *
*                                                                            *
*****************************************************************************/

static const int DesSbox[8][4][16] = {

   {
   {14,  4, 13,  1,  2, 15, 11,  8,  3, 10,  6, 12,  5,  9,  0,  7},
   { 0, 15,  7,  4, 14,  2, 13,  1, 10,  6, 12, 11,  9,  5,  3,  8},
   { 4,  1, 14,  8, 13,  6,  2, 11, 15, 12,  9,  7,  3, 10,  5,  0},
   {15, 12,  8,  2,  4,  9,  1,  7,  5, 11,  3, 14, 10,  0,  6, 13},
   },

   {
   {15,  1,  8, 14,  6, 11,  3,  4,  9,  7,  2, 13, 12,  0,  5, 10},
   { 3, 13,  4,  7, 15,  2,  8, 14, 12,  0,  1, 10,  6,  9, 11,  5},
   { 0, 14,  7, 11, 10,  4, 13,  1,  5,  8, 12,  6,  9,  3,  2, 15},
   {13,  8, 10,  1,  3, 15,  4,  2, 11,  6,  7, 12,  0,  5, 14,  9},
   },

   {
   {10,  0,  9, 14,  6,  3, 15,  5,  1, 13, 12,  7, 11,  4,  2,  8},
   {13,  7,  0,  9,  3,  4,  6, 10,  2,  8,  5, 14, 12, 11, 15,  1},
   {13,  6,  4,  9,  8, 15,  3,  0, 11,  1,  2, 12,  5, 10, 14,  7},
   { 1, 10, 13,  0,  6,  9,  8,  7,  4, 15, 14,  3, 11,  5,  2, 12},
   },

   {
   { 7, 13, 14,  3,  0,  6,  9, 10,  1,  2,  8,  5, 11, 12,  4, 15},
   {13,  8, 11,  5,  6, 15,  0,  3,  4,  7,  2, 12,  1, 10, 14,  9},
   {10,  6,  9,  0, 12, 11,  7, 13, 15,  1,  3, 14,  5,  2,  8,  4},
   { 3, 15,  0,  6, 10,  1, 13,  8,  9,  4,  5, 11, 12,  7,  2, 14},
   },

   {
   { 2, 12,  4,  1,  7, 10, 11,  6,  8,  5,  3, 15, 13,  0, 14,  9},
   {14, 11,  2, 12,  4,  7, 13,  1,  5,  0, 15, 10,  3,  9,  8,  6},
   { 4,  2,  1, 11, 10, 13,  7,  8, 15,  9, 12,  5,  6,  3,  0, 14},
   {11,  8, 12,  7,  1, 14,  2, 13,  6, 15,  0,  9, 10,  4,  5,  3},
   },

   {
   {12,  1, 10, 15,  9,  2,  6,  8,  0, 13,  3,  4, 14,  7,  5, 11},
   {10, 15,  4,  2,  7, 12,  9,  5,  6,  1, 13, 14,  0, 11,  3,  8},
   { 9, 14, 15,  5,  2,  8, 12,  3,  7,  0,  4, 10,  1, 13, 11,  6},
   { 4,  3,  2, 12,  9,  5, 15, 10, 11, 14,  1,  7,  6,  0,  8, 13},
   },

   {
   { 4, 11,  2, 14, 15,  0,  8, 13,  3, 12,  9,  7,  5, 10,  6,  1},
   {13,  0, 11,  7,  4,  9,  1, 10, 14,  3,  5, 12,  2, 15,  8,  6},
   { 1,  4, 11, 13, 12,  3,  7, 14, 10, 15,  6,  8,  0,  5,  9,  2},
   { 6, 11, 13,  8,  1,  4, 10,  7,  9,  5,  0, 15, 14,  2,  3, 12},
   },

   {
   {13,  2,  8,  4,  6, 15, 11,  1, 10,  9,  3, 14,  5,  0, 12,  7},
   { 1, 15, 13,  8, 10,  3,  7,  4, 12,  5,  6, 11,  0, 14,  9,  2},
   { 7, 11,  4,  1,  9, 12, 14,  2,  0,  6, 10, 13, 15,  3,  5,  8},
   { 2,  1, 14,  7,  4, 10,  8, 13, 15, 12,  9,  0,  3,  5,  6, 11},
   },

};

/*****************************************************************************
*                                                                            *
*  Define a mapping for the P-box permutation of data blocks.                *
*                                                                            *
*****************************************************************************/

static const int DesPbox[32] = {

   16,  7, 20, 21, 29, 12, 28, 17,  1, 15, 23, 26,  5, 18, 31, 10,
    2,  8, 24, 14, 32, 27,  3,  9, 19, 13, 30,  6, 22, 11,  4, 25

};

/*****************************************************************************
*                                                                            *
*  Define a mapping for the final permutation of data blocks.                *
*                                                                            *
*****************************************************************************/

static const int DesFinal[64] = {

   40,  8, 48, 16, 56, 24, 64, 32, 39,  7, 47, 15, 55, 23, 63, 31,
   38,  6, 46, 14, 54, 22, 62, 30, 37,  5, 45, 13, 53, 21, 61, 29,
   36,  4, 44, 12, 52, 20, 60, 28, 35,  3, 43, 11, 51, 19, 59, 27,
   34,  2, 42, 10, 50, 18, 58, 26, 33,  1, 41,  9, 49, 17, 57, 25

};

/*****************************************************************************
*                                                                            *
*  Define a type for whether to encipher or decipher data.                   *
*                                                                            *
*****************************************************************************/

typedef enum DesEorD_ {encipher, decipher} DesEorD;





/*****************************************************************************
*                                                                            *
*  -------------------------------- bit_get -------------------------------  *
*                                                                            *
*****************************************************************************/

int bit_get(const unsigned char *bits, int pos) {

	unsigned char      mask;

	int                i;

	/*****************************************************************************
	*                                                                            *
	*  Set a mask for the bit to get.                                            *
	*                                                                            *
	*****************************************************************************/

	mask = 0x80;

	for (i = 0; i < (pos % 8); i++)
	   mask = mask >> 1;

	/*****************************************************************************
	*                                                                            *
	*  Get the bit.                                                              *
	*                                                                            *
	*****************************************************************************/

	return (((mask & bits[(int)(pos / 8)]) == mask) ? 1 : 0);

}

/*****************************************************************************
*                                                                            *
*  -------------------------------- bit_set -------------------------------  *
*                                                                            *
*****************************************************************************/

void bit_set(unsigned char *bits, int pos, int state) {

	unsigned char      mask;

	int                i;

	/*****************************************************************************
	*                                                                            *
	*  Set a mask for the bit to set.                                            *
	*                                                                            *
	*****************************************************************************/

	mask = 0x80;

	for (i = 0; i < (pos % 8); i++)
	   mask = mask >> 1;

	/*****************************************************************************
	*                                                                            *
	*  Set the bit.                                                              *
	*                                                                            *
	*****************************************************************************/

	if (state)
	   bits[pos / 8] = bits[pos / 8] | mask;
	else
	   bits[pos / 8] = bits[pos / 8] & (~mask);

	return;

}

/*****************************************************************************
*                                                                            *
*  -------------------------------- bit_xor -------------------------------  *
*                                                                            *
*****************************************************************************/

void bit_xor(const unsigned char *bits1, const unsigned char *bits2, unsigned char *bitsx, int size) {

	int                i;

	/*****************************************************************************
	*                                                                            *
	*  Compute the bitwise XOR (exclusive OR) of the two buffers.                *
	*                                                                            *
	*****************************************************************************/

	for (i = 0; i < size; i++) {

	   if (bit_get(bits1, i) != bit_get(bits2, i))
	      bit_set(bitsx, i, 1);
	   else
	      bit_set(bitsx, i, 0);

	}

	return;

}

/*****************************************************************************
*                                                                            *
*  ----------------------------- bit_rot_left -----------------------------  *
*                                                                            *
*****************************************************************************/

void bit_rot_left(unsigned char *bits, int size, int count) {

	int                fbit,
	                   lbit,
	                   i,
	                   j;

	/*****************************************************************************
	*                                                                            *
	*  Rotate the buffer to the left the specified number of bits.               *
	*                                                                            *
	*****************************************************************************/

	if (size > 0) {

	   for (j = 0; j < count; j++) {

	      for (i = 0; i <= ((size - 1) / 8); i++) {

	         /********************************************************************
	         *                                                                   *
	         *  Get the bit about to be shifted off the current byte.            *
	         *                                                                   *
	         ********************************************************************/

	         lbit = bit_get(&bits[i], 0);

	         if (i == 0) {

	            /*****************************************************************
	            *                                                                *
	            *  Save the bit shifted off the first byte for later.            *
	            *                                                                *
	            *****************************************************************/

	            fbit = lbit;

	            }

	         else {

	            /*****************************************************************
	            *                                                                *
	            *  Set the rightmost bit of the previous byte to the leftmost    *
	            *  bit about to be shifted off the current byte.                 *
	            *                                                                *
	            *****************************************************************/

	            bit_set(&bits[i - 1], 7, lbit);

	         }

	         /********************************************************************
	         *                                                                   *
	         *  Shift the current byte to the left.                              *
	         *                                                                   *
	         ********************************************************************/

	         bits[i] = bits[i] << 1;

	      }

	      /***********************************************************************
	      *                                                                      *
	      *  Set the rightmost bit of the buffer to the bit shifted off the      *
	      *  first byte.                                                         *
	      *                                                                      *
	      ***********************************************************************/

	      bit_set(bits, size - 1, fbit);

	   }

	}

	return;

}




/*****************************************************************************
*                                                                            *
*  -------------------------------- permute -------------------------------  *
*                                                                            *
*****************************************************************************/

static void permute(unsigned char *bits, const int *mapping, int n) {

	unsigned char      temp[8];

	int                i;

	/*****************************************************************************
	*                                                                            *
	*  Permute the buffer using an n-entry mapping.                              *
	*                                                                            *
	*****************************************************************************/

	memset(temp, 0, (int)ceil(n / 8));

	for (i = 0; i < n; i++)
	   bit_set(temp, i, bit_get(bits, mapping[i] - 1));

	memcpy(bits, temp, (int)ceil(n / 8));

	return;

}


/*****************************************************************************
*                                                                            *
*  ------------------------------- des_main -------------------------------  *
*                                                                            *
*****************************************************************************/

static int des_main(const unsigned char *source, unsigned char *target, const unsigned char *key, DesEorD direction, int len) {

	static unsigned char subkeys[16][7];

	unsigned char        temp[8],
	                     target_temp[8],
	                     lkey[4],
	                     rkey[4],
	                     lblk[6],
	                     rblk[6],
	                     fblk[6],
	                     xblk[6],
	                     sblk;

	int                  row,
	                     col,
	                     i,
	                     j,
	                     k,
	                     p,
	                     strSize,
	                     dataSize,
	                     tgtIndex,
	                     l;

	/*****************************************************************************
	*                                                                            *
	*  If key is NULL, use the subkeys as computed in a previous call.           *
	*                                                                            *
	*****************************************************************************/

	if (key != NULL) {

	   /**************************************************************************
	   *                                                                         *
	   *  Make a local copy of the key.                                          *
	   *                                                                         *
	   **************************************************************************/

	   memcpy(temp, key, 8);

	   /**************************************************************************
	   *                                                                         *
	   *  Permute and compress the key into 56 bits.                             *
	   *                                                                         *
	   **************************************************************************/

	   permute(temp, DesTransform, 56);

	   /**************************************************************************
	   *                                                                         *
	   *  Split the key into two 28-bit blocks.                                  *
	   *                                                                         *
	   **************************************************************************/

	   memset(lkey, 0, 4);
	   memset(rkey, 0, 4);

	   for (j = 0; j < 28; j++)
	      bit_set(lkey, j, bit_get(temp, j));

	   for (j = 0; j < 28; j++)
	      bit_set(rkey, j, bit_get(temp, j + 28));

	   /**************************************************************************
	   *                                                                         *
	   *  Compute the subkeys for each round.                                    *
	   *                                                                         *
	   **************************************************************************/

	   for (i = 0; i < 16; i++) {

	      /***********************************************************************
	      *                                                                      *
	      *  Rotate each block according to its round.                           *
	      *                                                                      *
	      ***********************************************************************/

	      bit_rot_left(lkey, 28, DesRotations[i]);
	      bit_rot_left(rkey, 28, DesRotations[i]);

	      /***********************************************************************
	      *                                                                      *
	      *  Concatenate the blocks into a single subkey.                        *
	      *                                                                      *
	      ***********************************************************************/

	      for (j = 0; j < 28; j++)
	         bit_set(subkeys[i], j, bit_get(lkey, j));

	      for (j = 0; j < 28; j++)
	         bit_set(subkeys[i], j + 28, bit_get(rkey, j));

	      /***********************************************************************
	      *                                                                      *
	      *  Do the permuted choice permutation.                                 *
	      *                                                                      *
	      ***********************************************************************/

	      permute(subkeys[i], DesPermuted, 48);

	   }

	}

	//strSize = strlen(source);
	strSize = len;
	dataSize = strSize/8;
	if (strSize%8) dataSize++;

	for(l=0, tgtIndex=0; l<dataSize; l++){

		/*****************************************************************************
		*                                                                            *
		*  Make a local copy of the source text.                                     *
		*                                                                            *
		*****************************************************************************/
		
		memcpy(temp, source+tgtIndex, 8);
		
		/*****************************************************************************
		*                                                                            *
		*  Do the initial permutation.                                               *
		*                                                                            *
		*****************************************************************************/
		
		permute(temp, DesInitial, 64);
		
		/*****************************************************************************
		*                                                                            *
		*  Split the source text into a left and right block of 32 bits.             *
		*                                                                            *
		*****************************************************************************/
		
		memcpy(lblk, &temp[0], 4);
		memcpy(rblk, &temp[4], 4);
		
		/*****************************************************************************
		*                                                                            *
		*  Encipher or decipher the source text.                                     *
		*                                                                            *
		*****************************************************************************/
		
		for (i = 0; i < 16; i++) {
		
		   /**************************************************************************
		   *                                                                         *
		   *  Begin the computation of f.                                            *
		   *                                                                         *
		   **************************************************************************/
		
		   memcpy(fblk, rblk, 4);
		
		   /**************************************************************************
		   *                                                                         *
		   *  Permute and expand the copy of the right block into 48 bits.           *
		   *                                                                         *
		   **************************************************************************/
		
		   permute(fblk, DesExpansion, 48);
		
		   /**************************************************************************
		   *                                                                         *
		   *  Apply the appropriate subkey for the round.                            *
		   *                                                                         *
		   **************************************************************************/
		
		   if (direction == encipher) {
		
		      /***********************************************************************
		      *                                                                      *
		      *  For enciphering, subkeys are applied in increasing order.           *
		      *                                                                      *
		      ***********************************************************************/
		
		      bit_xor(fblk, subkeys[i], xblk, 48);
		      memcpy(fblk, xblk, 6);
		
		      }
		
		   else {
		
		      /***********************************************************************
		      *                                                                      *
		      *  For deciphering, subkeys are applied in decreasing order.           *
		      *                                                                      *
		      ***********************************************************************/
		
		      bit_xor(fblk, subkeys[15 - i], xblk, 48);
		      memcpy(fblk, xblk, 6);
		
		   }
		
		   /**************************************************************************
		   *                                                                         *
		   *  Do the S-box substitutions.                                            *
		   *                                                                         *
		   **************************************************************************/
		
		   p = 0;
		
		   for (j = 0; j < 8; j++) {
		
		      /***********************************************************************
		      *                                                                      *
		      *  Compute a row and column into the S-box tables.                     *
		      *                                                                      *
		      ***********************************************************************/
		
		      row = (bit_get(fblk, (j * 6)+0) * 2) + (bit_get(fblk, (j * 6)+5) * 1);
		      col = (bit_get(fblk, (j * 6)+1) * 8) + (bit_get(fblk, (j * 6)+2) * 4) +
		            (bit_get(fblk, (j * 6)+3) * 2) + (bit_get(fblk, (j * 6)+4) * 1);
		
		      /***********************************************************************
		      *                                                                      *
		      *  Do the S-box substitution for the current six-bit block.            *
		      *                                                                      *
		      ***********************************************************************/
		
		      sblk = (unsigned char)DesSbox[j][row][col];
		
		      for (k = 4; k < 8; k++) {
		
		         bit_set(fblk, p, bit_get(&sblk, k));
		         p++;
		
		      }
		
		   }
		
		   /**************************************************************************
		   *                                                                         *
		   *  Do the P-box permutation to complete f.                                *
		   *                                                                         *
		   **************************************************************************/
		
		   permute(fblk, DesPbox, 32);
		
		   /**************************************************************************
		   *                                                                         *
		   *  Compute the XOR of the left block and f.                               *
		   *                                                                         *
		   **************************************************************************/
		
		   bit_xor(lblk, fblk, xblk, 32);
		
		   /**************************************************************************
		   *                                                                         *
		   *  Set the left block for the round.                                      *
		   *                                                                         *
		   **************************************************************************/
		
		   memcpy(lblk, rblk, 4);
		
		   /**************************************************************************
		   *                                                                         *
		   *  Set the right block for the round.                                     *
		   *                                                                         *
		   **************************************************************************/
		
		   memcpy(rblk, xblk, 4);
		
		}
		
		/*****************************************************************************
		*                                                                            *
		*  Set the target text to the rejoined final right and left blocks.          *
		*                                                                            *
		*****************************************************************************/
		
		memcpy(&target_temp[0], rblk, 4);
		memcpy(&target_temp[4], lblk, 4);
		
		/*****************************************************************************
		*                                                                            *
		*  Do the final permutation.                                                 *
		*                                                                            *
		*****************************************************************************/
		
		permute(target_temp, DesFinal, 64);
		
		memcpy((target + tgtIndex), target_temp, 8);
		tgtIndex += 8;

	}
	target[tgtIndex] = 0;

	return 0;

}



/*****************************************************************************
*                                                                            *
*  ----------------------------- des_encrypt -----------------------------  *
*                                                                            *
*****************************************************************************/

void des_encrypt(const unsigned char *plaintext, unsigned char *ciphertext, const unsigned char *key, int len) 
{

	des_main(plaintext, ciphertext, key, encipher, len);
	
	return;

}

/*****************************************************************************
*                                                                            *
*  ----------------------------- des_decrypt -----------------------------  *
*                                                                            *
*****************************************************************************/

void des_decrypt(const unsigned char *ciphertext, unsigned char *plaintext, const unsigned char *key, int len) 
{

	des_main(ciphertext, plaintext, key, decipher, len);
	
	return;

}





 
static const char MimeBase64[] = {
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
    'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
    'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
    'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
    'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
    'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
    'w', 'x', 'y', 'z', '0', '1', '2', '3',
    '4', '5', '6', '7', '8', '9', '+', '/'
};
 
static int DecodeMimeBase64[256] = {
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* 00-0F */
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* 10-1F */
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,62,-1,-1,-1,63,  /* 20-2F */
    52,53,54,55,56,57,58,59,60,61,-1,-1,-1,-1,-1,-1,  /* 30-3F */
    -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,  /* 40-4F */
    15,16,17,18,19,20,21,22,23,24,25,-1,-1,-1,-1,-1,  /* 50-5F */
    -1,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,  /* 60-6F */
    41,42,43,44,45,46,47,48,49,50,51,-1,-1,-1,-1,-1,  /* 70-7F */
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* 80-8F */
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* 90-9F */
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* A0-AF */
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* B0-BF */
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* C0-CF */
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* D0-DF */
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* E0-EF */
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1   /* F0-FF */
};
 
typedef union{
    struct{
        unsigned char c1,c2,c3;
    };
    struct{
        unsigned int e1:6,e2:6,e3:6,e4:6;
    };
} BF;
 
int endian = 0; // little : 0, big : 1
 
void base64e(INT8U *src, INT8U *result, INT16U length){
    int i, j = 0;
    BF temp;
#ifdef DES_ENCRYPTION
    INT8U string_buf[64];
    INT8U string_buf2[64];
#endif
 
    if(endian == 0){ // little endian(intel)
        for(i = 0 ; i < length ; i = i+3, j = j+4){
            temp.c3 = src[i];
            if((i+1) > length) temp.c2 = 0x00;
            else temp.c2 = src[i+1];
            if((i+2) > length) temp.c1 = 0x00;
            else temp.c1 = src[i+2];
 
            result[j]   = MimeBase64[temp.e4];
            result[j+1] = MimeBase64[temp.e3];
            result[j+2] = MimeBase64[temp.e2];
            result[j+3] = MimeBase64[temp.e1];
 
            if((i+2) > length) result[j+2] = '=';
            if((i+3) > length) result[j+3] = '=';
        }
    } else { // big endian(sun)
        for(i = 0 ; i < length ; i = i+3, j = j+4){
            temp.c1 = src[i];
            if((i+1) > length) temp.c2 = 0x00;
            else temp.c2 = src[i+1];
            if((i+2) > length) temp.c3 = 0x00;
            else temp.c3 = src[i+2];
 
            result[j]   = MimeBase64[temp.e4];
            result[j+1] = MimeBase64[temp.e3];
            result[j+2] = MimeBase64[temp.e2];
            result[j+3] = MimeBase64[temp.e1];
 
            if((i+2) > length) result[j+2] = '=';
            if((i+3) > length) result[j+3] = '=';
        }
    }
    result[j] = 0;
#ifdef DES_ENCRYPTION
    hex2hexstr(src, string_buf2, length);
    string_buf2[(length)*2] = 0;
    sprintf(string_buf, " decrypt(hex)=[%s]\r\n", string_buf2);
    MsgOut(string_buf);
    
    sprintf(string_buf, " base64e(asc)=[%s]\r\n", result);
    MsgOut(string_buf);
#endif
}
 
void base64d(INT8U *src, INT8U *result, INT16U *length){
    int i, j = 0, src_length, blank = 0;
    BF temp;
#ifdef DES_ENCRYPTION
    INT8U string_buf[64];
    INT8U string_buf2[64];
#endif
 
    src_length = strlen(src);
 
    if(endian == 0){ // little endian(intel)
        for(i = 0 ; i < src_length ; i = i+4, j = j+3){
            temp.e4 = DecodeMimeBase64[src[i]];
            temp.e3 = DecodeMimeBase64[src[i+1]];
            if(src[i+2] == '='){
                temp.e2 = 0x00;
                blank++;
            } else temp.e2 = DecodeMimeBase64[src[i+2]];
            if(src[i+3] == '='){
                temp.e1 = 0x00;
                blank++;
            } else temp.e1 = DecodeMimeBase64[src[i+3]];
 
            result[j]   = temp.c3;
            result[j+1] = temp.c2;
            result[j+2] = temp.c1;
        }
    } else { // big endian(sun)
        for(i = 0 ; i < src_length ; i = i+4, j = j+3){
            temp.e4 = DecodeMimeBase64[src[i]];
            temp.e3 = DecodeMimeBase64[src[i+1]];
            if(src[i+2] == '='){
                temp.e2 = 0x00;
                blank++;
            } else temp.e2 = DecodeMimeBase64[src[i+2]];
            if(src[i+3] == '='){
                temp.e1 = 0x00;
                blank++;
            } else temp.e1 = DecodeMimeBase64[src[i+3]];
 
            result[j]   = temp.c1;
            result[j+1] = temp.c2;
            result[j+2] = temp.c3;
        }
    }
    *length = j-blank;
#ifdef DES_ENCRYPTION
    hex2hexstr(src, string_buf2, *length);
    string_buf2[(*length)*2] = 0;
    sprintf(string_buf, " encrypt(hex)=[%s]\r\n", string_buf2);
    MsgOut(string_buf);
    
    hex2hexstr(result, string_buf2, *length);
    string_buf2[(*length)*2] = 0;
    sprintf(string_buf, " base64d(hex)=(%d)[%s]\r\n", *length, string_buf2);
    MsgOut(string_buf);
#endif
}



// PKCS5Padding
//  If numberOfBytes(clearText) mod 8 == 7, PM = M + 0x01
//  If numberOfBytes(clearText) mod 8 == 6, PM = M + 0x0202
//  If numberOfBytes(clearText) mod 8 == 5, PM = M + 0x030303
//  ...
//  If numberOfBytes(clearText) mod 8 == 0, PM = M + 0x0808080808080808
void pkcs5padding(unsigned char *src, int direction)
{
	int len;
	unsigned char temp;
#ifdef DES_ENCRYPTION
	INT8U string_buf[64];
#endif
	
	len = strlen(src);
	
	if (direction == 0) // padding
	{
		temp = len % 8;
		memset(src + len, 8 - temp, 8 - temp);
		src[len + 8 - temp] = 0;
#ifdef DES_ENCRYPTION
		sprintf(string_buf, " paddingE(str)=[%s]\r\n", src);
		MsgOut(string_buf);
#endif
	}
	else // reverse
	{
		temp = src[len - 1];
		if (temp < 1 || temp > 8) return;
		memset(src + len - temp, 0, temp);
#ifdef DES_ENCRYPTION
		sprintf(string_buf, " paddingD(str)=[%s]\r\n", src);
		MsgOut(string_buf);
#endif
	}
}


/*
void main_des_base64(void)
{

	pkcs5padding(src, 0);
	
	des_encipher(src, encrypt, key); 
	   
	len = strlen(encrypt);
	base64e(encrypt, enc_base64, len);

	len = strlen(enc_base64);
	base64d(enc_base64, dec_base64, &len);

	des_decipher(dec_base64, decrypt, key);

	pkcs5padding(src, 1);

}
*/

#ifdef USE_TEST_ENCRYPT

INT8U EncryptPattern[][25] = {
	"QCv6DENh", "FSp4kd/+b+cg9rioqCsmhw==",
	"6z<q&W", "JHpFz5PjtS0=",
	"mq.o9T", "h6TNmdNYGcg=",
	"Y'l", "m3wjwwZ3tK0=",
	"g[=z,6S>g", "BufITsZQedE5UTRyIsQOHw==",
	"(t!dPua`", "P9pZwwY8q0gg9rioqCsmhw==",
	"p4-'", "nSkkW1PyqIw=",
	"W7[", "WLvoGDJ1PhI=",
	"!5JMJ[Gw", "8DjpGG81bzYg9rioqCsmhw==",
	"KQI.D<St&", "qH9mjZ1BilsscCQizVvo4g==",
	"oH_=5", "2nXNqQ43gEc=",
	"'QG6Nta", "FvbTynrXMt4=",
	"S;cKYKpg", "bBdAK42HLj0g9rioqCsmhw==",
	"4)XvS", "XhZmusdD83c=",
	"7f$", "P0TPi1Khr30=",

	"1=\\D;MG", "xO46homo318=",
	"=}uN*Wc", "EqDIqvM+qUA=",
	"68L}LZ", "ltLY5MB3gkw=",
	"nuHD<$u", "LsUIo3x0wC0=",
	"b.R!5", "5x4OWb3aEQA=",
	"gZ%ZZ", "WNbZu9HFX/U=",
	"OzNo$aD$<", "e22BVUz0L/K8FBpjf36VlA==",
	"&)}%'", "MDOytuKy5p8=",
	"?J/SQ", "h+sz4cZivnM=",
	"-`D(L2A_g", "FDUWMm9Vi5k5UTRyIsQOHw==",
	"U/}]B%R.", "QHX4S+rRm6sg9rioqCsmhw==",
	"A[<t%", "nKNVHwnPhuI=",
	"b2Rp'-v$f", "Om4ccgSAVLMoavIJSd2d1g==",
	":<%7", "AKYu+NiEPYY=",
	"gb2*", "V1O3j1mH3Qs=",
	"bO=B:", "hne0b5apXEQ=",
	"mdLM$", "D8IAnGsKeBc=",
	"A&_4EhY", "rApbqNhwrEY=",
	"]dVZVa", "CPir9PyPeso=",
	"=_g", "WfOZM+r09Ok=",
	"K#'zJuFh(", "eSt/UJljE/92XkpUlk05gw==",
	",nW", "fIDR3gZwdBI=",

	"19z>\\_", "5pC0iBFOS9A=",
	"]n+", "nXThUp9A/gU=",
	"o]^G]", "IIuXvlmYx6U=",
	"7KC2|_", "PgtyqYx1NDk=",
	"P6N;#[", "lJwYdYIxw7E=",
	"=1p^w7", "7k/WnAQhNS4=",
	"P#n", "F73NGNvptSI=",
	"kCMF", "ttg6l15+/cA=",
	"E94dv4(q", "qiBPNONB3QMg9rioqCsmhw==",
	"/-|", "fDnBjns/CjU=",
	"{,Dd%", "pLdcYpNCb7Y=",
	"Oq/", "K0wBRNKkSf8=",
	"|%A", "whjpqWSf7lY=",
	"TVCy7)", "gXxfliCtBhU=",
	"_xM.Yh]", "9KCcosRkQp4=",
	"bE?8L", "P54hYUrLyCA=",
	">.:", "sCky44+l2v8=",
	"#[RU#q6", "3XMyiam6Nlg=",
	"p4$jS%U", "ODDH3NOLXIM=",
	"0[H1mv&t", "ry1nhvYfkoEg9rioqCsmhw==",

	"zoB\\`X;L", "d0d7EJl67tcg9rioqCsmhw==",
	"-o92$WN-P", "/U5lPddg058Yub4bpBRuhA==",
	"u'Zcc", "ODsV+Gl+43k=",
	"KynKK2", "Wlyq5DsTQUQ=",
	"u|7g", "TZq+zoxnjYE=",
	"<:(/h", "7EEqIdi7zmM=",
	"ro'Ng)[b", "VdKGdiYK6KYg9rioqCsmhw==",
	"4(6=(1", "k8SkpWB/fvA=",
	".:N[e", "XDZIkqSyxrk=",
	"vJL", "hLy1/VT9J00=",
	"5FY", "RxOQ1c8U8Hg=",
	"}},H@a-Q#", "C+smpV1dos5W6Z4Nx2y9nw==",
	"[*nd6B3g", "A517W60I8JYg9rioqCsmhw==",
	"bz/t7PHga", "7j/8XToCgd/TZwhlLhX7UQ==",
	"U9F#${A&K", "ie7R3arQH7WvyZ5ZPNrtBA==",
	"L!7", "RKDxA5lQ2WM=",
	"jw/aUm", "WyRUqC4h3BE=",
	"pFK/_/Y3>", "FwKlIOZQM69t35wKP104GQ==",
	"]xT6<", "lgTwzLjLOfs=",
	"on4-", "KUH/GQY7MUM=",
	">`>.W!?nZ", "7HdS9QM9nAQgx8jdQjeQ0Q==",
	"NTq7", "bJaf1Nr8jeQ=",
	"k*d}$", "0mGLowwvieE=",

	"2S5\\#!Nwv", "RRzLcY9z53QialsymA1oDA==",
	"#PJC4", "ZB+vTLJZOKU=",
	"S9Zpwi", "G1eZg/YfVeA=",
	"<8m41P4", "OBkZ+1ufhCE=",
	"Ti/deW9E", "m8UdqQ2XNh4g9rioqCsmhw==",
	",d7V", "8PYJcv4X+do=",
	"w3OD7", "ls8U9a/601A=",
	"8Gn$*FHZz", "AIYnYi2hSwVan60fNhFulA==",
	"C!ns<Z(", "qwirFMdnzFQ=",
	"Z-muVv3}", "3EDrUrIZRY4g9rioqCsmhw==",
	"O<sr:hB", "vtkFK1I35gM=",
	"S+H", "DrnRSo3LU2A=",
	"Q?lAL", "Jp/f56QcMCY=",
	"f>`a4VW", "0+UI3sqMzDw=",
	"_fm)\"Rg6", "IzTSRsQ7sMIg9rioqCsmhw==",
	"YG\"T", "7JM3eNYyMbM=",
	")D&3\"", "n3I1jNVynzA=",
	"x|g\".&}>", "Qv6VK4hj0fYg9rioqCsmhw==",
	"",""
};

extern char MsgBuf[];
void TestEncryption(void)
{
	INT8U encrypt[32];
	INT8U decrypt[32];
	INT8U padding[32];
	INT8U enc_base64[32];
	INT8U scaleKey[9];
	INT16U len;
	INT16U i;
	INT8U key[] = {"352C8A61254F758B"};
	//INT8U key[] = {"D01D6B93C304140B"};
	
	for (i = 0; i < 8; i++)
	{
		scaleKey[i] = hexstrtobyte(&key[i*2]);
	}
	
	sprintf(MsgBuf, "\r\n\r\n<< Start >>\r\n");
	MsgOut(MsgBuf);

	for (i = 0; i < 100; i++)
	{
		if (EncryptPattern[i*2][0] == 0) break;

		// password decryption
		len = strlen((char*)EncryptPattern[i*2+1]);
		base64d(EncryptPattern[i*2+1], encrypt, &len);
		des_decipher(encrypt, decrypt, scaleKey, len); 
		pkcs5padding(decrypt, 1); // reverse padding
		len = strlen(decrypt);
		if (data_compare((INT8U*)EncryptPattern[i*2], (INT8U*)decrypt, len) != 0)
		{
			sprintf(MsgBuf, "[D%2d] = [%s], [%s] ", i, EncryptPattern[i*2+1], EncryptPattern[i*2]);
			MsgOut(MsgBuf);		
			sprintf(MsgBuf, "<%s>\r\n\r\n", decrypt);
			MsgOut(MsgBuf);	
		}
		
		// password 암호화 함수
		len = strlen((char*)EncryptPattern[i*2]);
		memcpy(padding, EncryptPattern[i*2], len);
		padding[len] = 0;
		pkcs5padding(padding, 0);
		len = strlen((char*)padding);
		des_encipher(padding, encrypt, scaleKey, len); 
		base64e(encrypt, enc_base64, len);
		if (data_compare((INT8U*)EncryptPattern[i*2+1], (INT8U*)enc_base64, len) != 0)
		{
			sprintf(MsgBuf, "[E%2d] = [%s], [%s] ", i, EncryptPattern[i*2], EncryptPattern[i*2+1]);
			MsgOut(MsgBuf);		
			sprintf(MsgBuf, "<%s>\r\n\r\n", enc_base64);
			MsgOut(MsgBuf);	
		}
	}
	
}
#endif
#endif
