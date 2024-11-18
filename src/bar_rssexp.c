/*****************************************************************************
|*			
|*  Copyright		:	(c) 2009 CAS
|*  Filename		:	bar_rssexp.c
|*  Version		:	0.1
|*  Programmer(s)	:	Keum Young-Kwang (gm)
|*  Created		:	2010/01/08
|*  Description		:	barcode RSS Expand routine
|*				
****************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "globals.h"
#include "commbuf.h"
#include "bar_common.h"

#ifdef USE_RSSEXPAND
//RSSExp------------------------------------------------------------
#define K		4
// height
#define SYM_H_EXP 	34
#define PARITY_MOD_EXP 211
#define PARITY_PWR 3   
#define L_PAD 1 // CC left offset
//------------------------------------------------------------------
// not including guard bars
#define ELMNTS 46 // gm include guard bars (46-4)

// RSS-14S row heights
#define ROWS1_H		5
#define ROWS2_H		7

#define L_PADB 10 // RSS14L left pad for ccb
#define L_PADR 5 // RSS-14 left offset
#define R_PADR 7 // RSS-14s right offset

#define CCB4_WIDTH 101 // 4 column cca/b
#define CCB4_ELMNTS 53 // includes qz's

#define MAX_CCB4_ROWS 44	// ccb-4 max rows
#define MAX_CCA2_SIZE 6 // index to 167 in CC2Sizes
#define MAX_CCA3_SIZE 4 // index to 167 in CC3Sizes
#define MAX_CCA4_SIZE 4 // index to 197 in CC4Sizes

#define MAX_CW 176 // ccb-4 max codewords
#define MAX_BYTES 148 // maximum byte mode capacity for ccb4

int *CCSizes; // will point to CCxSizes
ROMDATA int CC4Sizes[] = { 78,108,138,167,197, // cca sizes
			  208,264,352,496,672,840,1016,1184, // ccb sizes
			  0 };

// input chars FNC1 '#' and SYM_SEP '^' also defined in iswhat array
#define FNC1 '#'
#define SYM_SEP 94

#define	NUM_MODE	1
#define	ALNU_MODE	2
#define	ISO_MODE	3
#define ALPH_MODE	4
#define	FINI_MODE	5

#define	IS_NUM		0x1
#define	IS_FNC1		0x2
#define	IS_ALNU		0x4
#define	IS_ISO		0x8
#define	IS_FINI		0x80

struct encodeT {
	INT8U *str;
	int iStr;
	INT8U *bitField;
	int iBit;
	int mode;
	int typeAI;
	int diNum;
	int diAlpha;
};

#ifndef max
#define max(a,b) ((a>b) ? a : b)
#define min(a,b) ((a<b) ? a : b)
#endif

int rowWidth;
int line1;
int linFlag; // tells pack whether linear or cc is being encoded
INT8U ccPattern[MAX_CCB4_ROWS][CCB4_ELMNTS];


ROMDATA INT8U iswhat[256] = { /* byte look up table with IS_XXX bits */
	/* 32 control characters: */
		0x80,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	/* 32 punctuation and numeric characters: (FNC1 = # , symbol separator = ^ ) */
		8,8,8,0xf,0,8,8,8,8,8,0xc,8,0xc,0xc,0xc,0xc,
		0xd,0xd,0xd,0xd,0xd,0xd,0xd,0xd,0xd,0xd,
		8,8,8,8,8,8,
	/* 32 upper case and punctuation characters: */
		0,
		0xc,0xc,0xc,0xc,0xc,0xc,0xc,0xc,0xc,0xc,0xc,0xc,0xc,
		0xc,0xc,0xc,0xc,0xc,0xc,0xc,0xc,0xc,0xc,0xc,0xc,0xc,
		0,0,0,0xc,8,
	/* 32 lower case and punctuation characters: */
		0,
		8,8,8,8,8,8,8,8,8,8,8,8,8,
		8,8,8,8,8,8,8,8,8,8,8,8,8,
		0,0,0,0,0,
	/* extended ASCII */
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};

#define FINDER_SIZE 6
ROMDATA INT8U finders[FINDER_SIZE][3] = {
	{ 1,8,4 },
	{ 3,6,4 },
	{ 3,4,6 },
	{ 3,2,8 },
	{ 2,6,5 },
	{ 2,2,9 } };
	      
ROMDATA INT16S finderSets[10][11] = {
	{ 1,	-1,	0,	 0,	0,	 0,	0,	 0,	0,	 0,	0},
	{ 1,	-2,	2,	 0,	0,	 0,	0,	 0,	0,	 0,	0},
	{ 1,	-3,	2,	-4,	0,	 0,	0,	 0,	0,	 0,	0},
	{ 1,	-5,	2,	-4,	3,	 0,	0,	 0,	0,	 0,	0},
	{ 1,	-5,	2,	-4,	4,	-6,	0,	 0,	0,	 0,	0},
	{ 1,	-5,	2,	-4,	5,	-6,	6,	 0,	0,	 0,	0},
	{ 1,	-1,	2,	-2,	3,	-3,	4,	-4,	0,	 0,	0},
	{ 1,	-1,	2,	-2,	3,	-3,	4,	-5,	5,	 0,	0},
	{ 1,	-1,	2,	-2,	3,	-3,	4,	-5,	6,	-6,	0},
	{ 1,	-1,	2,	-2,	3,	-4,	4,	-5,	5,	-6,	6} };

// element 1 weighting for characters N determined by adjacent finder
ROMDATA INT16U parWts[24] = { 0,1,20,189,193,62,185,113,150,46,76,43,16,109,70,134,148,6,120,79,103,161,55,45 };
/* GF(929) log and antilog tables: */
int gfPwr[928];
int gfLog[929];
int gpa[512];
int gfMul(int a,int b) {

	if ((a == 0) || (b == 0)) return(0);
	return(gfPwr[(gfLog[a] + gfLog[b]) % 928]);
}
void genPoly(int eccSize) {
	int	i, j;

	gpa[0] = 1;
	for (i = 1; i < eccSize+1; i ++) { gpa[i] = 0; }
	for (i = 0; i < eccSize; i++ ) {
		for (j = i; j >= 0; j --) {
			gpa[j+1] = (gpa[j] + gfMul(gpa[j+1], gfPwr[i+1])) % 929;
		}
		gpa[0] = gfMul(gpa[0], gfPwr[i+1]);
	}
	for (i = eccSize-1; i >= 0; i-=2 ) {
		gpa[i] = (929 - gpa[i]) % 929;
	}
}

void genECC(int dsize, int csize, unsigned int sym[]) {
	int i, n, t;

	genPoly(csize);
              
	/* first zero ecc words */              
	for (i = dsize; i < dsize+csize; i++) {
		sym[i] = 0;
	}           
	/* generate check characters */
	for ( n = 0; n < dsize; n++ ) {
		t = (sym[dsize] + sym[n]) % 929;
		for (i = 0; i < csize-1; i++) {
			sym[dsize+i] = (sym[dsize+i+1] + 929 - gfMul(t, gpa[csize-1 - i])) % 929;
		}
		sym[dsize+csize-1] = (929 - gfMul(t, gpa[0])) % 929;
	}
	for (i = dsize; i < dsize+csize; i++) {
		sym[i] = (929 - sym[i]) % 929; 
	}
	return;
}

int getBit(INT8U bitStr[], int bitPos) 
{
	return(((bitStr[bitPos/8] & (0x80>>(bitPos%8))) == 0) ?	0 : 1);
}

static unsigned int pwr928[69][7];
/* converts bit string to base 928 values, codeWords[0] is highest order */
int encode928(INT8U bitString[], unsigned int codeWords[], int bitLng) 
{

	int i, j, b, bitCnt, cwNdx, cwCnt, cwLng;

	for (cwNdx = cwLng = b = 0; b < bitLng; b += 69, cwNdx += 7) {
		bitCnt = min(bitLng-b, 69);
		cwLng += cwCnt = bitCnt/10 + 1;
		for (i = 0; i < cwCnt; i++) codeWords[cwNdx+i] = 0; /* init 0 */
		for (i = 0; i < bitCnt; i++) {
			if (getBit(bitString, b+bitCnt-i-1)) {
				for (j = 0; j < cwCnt; j++) {
					codeWords[cwNdx+j] += pwr928[i][j+7-cwCnt];
				}
			}
		}
		for (i = cwCnt-1; i > 0; i--) {
			/* add "carries" */
			codeWords[cwNdx+i-1] += codeWords[cwNdx+i]/928;
			codeWords[cwNdx+i] %= 928;
		}
	}
	return(cwLng);
}

#define MAX_CCC_CW 863		// ccc max data codewords
#define MAX_CCC_ROWS 90		// ccc max rows
#define MAX_CCC_BYTES 1033 	// maximum byte mode capacity for ccc

#define MAX_CW 176 	// ccb-4 max codewords
#define MAX_BYTES 148 	// maximum byte mode capacity for ccb4
void putBits(INT8U bitField[], int bitPos, int length, INT16U bits) 
{
	int i, maxBytes;

	if (linFlag == -1) {
		maxBytes = MAX_CCC_BYTES; // CC-C
	}
	else {
		maxBytes = MAX_BYTES; // others
	}
	if ((bitPos+length > maxBytes*8) || (length > 16)) {
//		printf("\nputBits error, %d, %d\n", bitPos, length);
		errFlag = TRUE;
		return;
	}
	for (i = length-1; i >= 0; i--) {
		if ((bits & 1) != 0) {
			bitField[(bitPos+i)/8] |= 0x80 >> ((bitPos+i)%8);
		}
		else {
			bitField[(bitPos+i)/8] &= ~(0x80 >> ((bitPos+i)%8));
		}
		bits >>= 1;
	}
	return;
}

/*
returns number of bits left to closest 2d symbol,
 or -1 if iBit is larger than largest in type and width
 also returns CCSizes index or data char cnt for RSS14E in *size
*/
// globals used by CC-C:
int colCnt; // after set in main, may be decreased by getUnusedBitCnt
int rowCnt; // determined by getUnusedBitCnt
int eccCnt; // determined by getUnusedBitCnt
int getUnusedBitCnt(int iBit, int *size) 
{
	// max data plus ecc codewords for CC-C
	static eccMaxCW[] = { 40+8, 160+16, 320+32, 863+64, 0 };

	int i, byteCnt, cwCnt;

	if (linFlag == 1) { // RSS Expanded
		*size = 0;
		if (iBit <= 252) {
			if ((*size = (iBit + 11) / 12) < 3) {
				*size = 3; // 3 data sym chars minimum
			}
			if ((((*size)+1+rowWidth) % rowWidth) == 1) {
				(*size)++; // last row minimum of 2
			}
			return(*size*12 - iBit);
		}
	}
	else if (linFlag == 0) { // CC-A/B
		for (i = 0; CCSizes[i] != 0; i++) {
			if (iBit <= CCSizes[i]) {
				*size = i;
				return(CCSizes[i] - iBit);
			}
		}
	}
	else if (linFlag == -1) { // CC-C
		*size = FALSE; // size used as error flag for CC-C
		// calculate cwCnt from # of bits
		byteCnt = (iBit+7)/8;
		i = byteCnt/6;
		cwCnt = (byteCnt/6)*5 + byteCnt%6;
		// find # of ecc codewords
		for (i = 0, eccCnt = 8; eccCnt <= 64; i++, eccCnt *= 2) {
			if (cwCnt + eccCnt <= eccMaxCW[i]) {
				break;
			}
		}
		if (eccCnt > 64) {
			return(-1); // too many codewords for CCC
		}

		colCnt++; // preadjust for first decrement in loop
		do {
			colCnt--; // make narrower until satisfies min aspect ratio
			rowCnt = max(3, (1 + 2 + cwCnt + eccCnt + colCnt-1) / colCnt);
			if (rowCnt > MAX_CCC_ROWS) {
				return(-1); // too many rows for CCC
			}
		} while (colCnt + 4 > rowCnt*4);

		if (rowCnt == 3) { // find minimum width if 3 rows, but no less than 4 data)
			colCnt = max(4, (1 + 2 + cwCnt + eccCnt + 2) / 3);
		}

		cwCnt = colCnt*rowCnt - 1 - 2 - eccCnt;
		byteCnt = (cwCnt/5)*6 + cwCnt%5;
		*size = TRUE;
		return(byteCnt*8 - iBit);
	}
	return(-1);
}

int insertPad(struct encodeT *encode) 
{
	int bitCnt, chr, size;

	bitCnt = getUnusedBitCnt(encode->iBit, &size);
	if (bitCnt < 0) {
		return(-1); // too many bits
	}
	while (bitCnt >= 5) {
		putBits(encode->bitField, encode->iBit, 5, 4);
		encode->iBit += 5;
		bitCnt -= 5;
	}
	if (bitCnt > 0) {
		chr = 4 >> (5-bitCnt);
		putBits(encode->bitField, encode->iBit, bitCnt, (INT16U)chr);
		encode->iBit += bitCnt;
	}
	return(size);
}

int procALNU(struct encodeT *encode) 
{

	int chr, i, what, whatN;

	// check next char type
	if ((what = iswhat[(chr = encode->str[encode->iStr])]) == IS_FINI) {
		// end of data
		return(FINI_MODE);
	}
	if ((what & IS_ALNU) == 0) {
		// not a ALNU, latch to ISO
		putBits(encode->bitField, encode->iBit, 5, 4);
		encode->iBit += 5;
		return(ISO_MODE);
	}
	if (((what & IS_NUM) != 0) &&
			(((what | iswhat[encode->str[(encode->iStr)+1]]) & IS_FNC1) == 0)) {
		// next is NUM, look for more
		for (i = 1; i < 6; i++) {
			if ((whatN = iswhat[encode->str[(encode->iStr)+i]]) == IS_FINI) {
				if (i >= 4) {
					// latch numeric if >= 4 numbers at end
					putBits(encode->bitField, encode->iBit, 3, 0);
					encode->iBit += 3;
					return(NUM_MODE);
				}
				else {
					// else continue in ALNU
					break;
				}
			}
			else if ((whatN & IS_NUM) == 0) {
				// stay in ALNU if < 6 digits coming up
				break;
			}
		}
		if (i == 6) {
			// NUM if 6 or more digits coming up
			putBits(encode->bitField, encode->iBit, 3, 0);
			encode->iBit += 3;
			return(NUM_MODE);
		}
	}
	// process char in ALNU mode
	encode->iStr += 1;
	if ((what & IS_NUM) != 0) {
		// FNC1 or 0-9
		if ((what & IS_FNC1) != 0) {
			chr = 0xf;
			encode->mode = NUM_MODE;
		}
		else {
			chr = chr - (int)'0' + 5;
		}
		putBits(encode->bitField, encode->iBit, 5, (INT16U)chr);
		encode->iBit += 5;
	}
	else {
		if (chr == SYM_SEP) {
			// ^ (symbol separator)
			chr = 0x1F;
			encode->mode = NUM_MODE;
		}
		else if (chr >= (int)'A') {
			// A-Z
			chr = chr - (int)'A';
		}
		else if (chr >= ',') {
			// ,-./
			chr = chr - (int)',' + 0x1B;
		}
		else {
			// *
			chr = 0x1A;
		}
		putBits(encode->bitField, encode->iBit, 6, (INT16U)(chr + 0x20));
		encode->iBit += 6;
	}
	return(encode->mode);
}

int procNUM(struct encodeT *encode) 
{
	int bitCnt, char1, char2, what1, what2, i;

	// check first char type
	if ((what1 = iswhat[(char1 = encode->str[encode->iStr])]) == IS_FINI) {
		// end of data
		bitCnt = getUnusedBitCnt(encode->iBit, &i);
		if (bitCnt > 4) {
			bitCnt = 4;
		}
		if (bitCnt > 0) {
			// insert full or partial 0000 alnu latch for pad
			putBits(encode->bitField, encode->iBit, bitCnt, 0);
			encode->iBit += bitCnt;
		}
		return(FINI_MODE);
	}
	if ((what1 & IS_NUM) == 0) {
		// first not a "number", latch to ALNU
		putBits(encode->bitField, encode->iBit, 4, 0);
		encode->iBit += 4;
		return(ALNU_MODE);
	}
	// check 2nd char type
	if (((what2 = iswhat[(char2 = encode->str[(encode->iStr)+1])]) == IS_FINI) &&
			((what1 & IS_FNC1) == 0)) {
		// single digit left, check for nearly at end of bits
		encode->iStr += 1;
		bitCnt = getUnusedBitCnt(encode->iBit, &i);
		if ((bitCnt >= 4) && (bitCnt < 7)) {
			// less than 7 bits, encode a bcd+1
			putBits(encode->bitField, encode->iBit, 4, (INT16U)(char1+1 -(int)'0'));
			bitCnt -= 4;
			if (bitCnt > 0) {
				// 0 or 00 final pad
				putBits(encode->bitField, (encode->iBit)+4, bitCnt, 0);
			}
			encode->iBit += 4 + bitCnt;
		}
		else {
			// encode as digit & FNC1
			putBits(encode->bitField, encode->iBit, 7, (INT16U)(((char1-(int)'0') * 11) + 10 + 8));
			encode->iBit += 7;
			bitCnt -= 7;
			if ((bitCnt > 4) || (bitCnt < 0)) {
				bitCnt = 4; // either not near end or ran over to upper
			}
			if (bitCnt > 0) {
				// insert full or partial 0000 alnu latch
				putBits(encode->bitField, encode->iBit, bitCnt, 0);
				encode->iBit += bitCnt;
			}
		}
		return(FINI_MODE);
	}
	if (((what1 & what2 & IS_FNC1) != 0) || ((what2 & IS_NUM) == 0)) {
		// dbl FNC1 or 2nd char not a digit, latch to alnu
		putBits(encode->bitField, encode->iBit, 4, 0);
		encode->iBit += 4;
		return(ALNU_MODE);
	}
	else {
		// both "digits", encode as 7-bits
		encode->iStr += 2;
		if ((what1 & IS_FNC1) != 0) {
			char1 = 10;
		}
		else {
			char1 -= (int)'0';
		}
		if ((what2 & IS_FNC1) != 0) {
			char2 = 10;
		}
		else {
			char2 -= (int)'0';
		}
		putBits(encode->bitField, encode->iBit, 7, (INT16U)((char1 * 11) + char2 + 8));
		encode->iBit += 7;
		return(NUM_MODE);
	}
}
enum {
	AIx,
	AIdummy, // not used
	AI21,
	AI8004
};

#define isupper(c) ((c) >= 'A' && (c) <= 'Z')  //gm add
int procALPH(struct encodeT *encode) 
{
	int i;

	// check next char type
	if (isupper(encode->str[encode->iStr])) {
		// alpha
		putBits(encode->bitField, encode->iBit, 5, encode->str[encode->iStr]-65);
		encode->iBit += 5;
		encode->iStr += 1;
	}
	else if (isdigit(encode->str[encode->iStr])) {
		// number
		putBits(encode->bitField, encode->iBit, 6, encode->str[encode->iStr]+4);
		encode->iBit += 6;
		encode->iStr += 1;
	}
	else if (encode->str[encode->iStr] == FNC1) {
		// FNC1
		putBits(encode->bitField, encode->iBit, 5, 31);
		encode->iBit += 5;
		encode->iStr += 1;
		encode->mode = NUM_MODE;
	}
	else {
		// '\0'
		i = getUnusedBitCnt(encode->iBit, &i);
		if (i > 5) {
			i = 5; // i is minimum of 5 or unused bit count
		}
		putBits(encode->bitField, encode->iBit, i, 31);
		encode->iBit += i;
		encode->mode = NUM_MODE;
	}
	return(encode->mode);
}

int check2DData(INT8U dataStr[]) {
	int i;

	for (i = 0; iswhat[dataStr[i]] != 0x80; i++) {
		if (iswhat[dataStr[i]] == 0) {
			return(i); // error, unsupported character
		}
	}
	return(0);
}

// Fills in elements for a symbol character given a element array and
// and a symbol char value. Updates the parity *weight. Will fill in
// the array forward or reverse order for odd or even characters.
// Returns the updated parity.
// odd elements N & max, even N & max, odd mul, combos:
//static int tbl174[5*6] = {
ROMDATA int tbl174[5*6] = {/* 17,4 */ 
	12,7,	5,2,	4,	348,
	10,5,	7,4,	20,	1040,
	8,4,	9,5,	52,	1560,
	6,3,	11,6,	104,1040,
	4,1,	13,8,	204,204 };

int symCharPat(INT8U bars[], int symValue, int parity, int weight, int forwardFlag) 
{
	int i, value, saveVal;
	int elementN, elementMax;
	int wgtOdd, wgtEven;
	int iIndex;
	int *widths;

	wgtOdd = weight;
	wgtEven = (weight * 3) % PARITY_MOD_EXP;

	// get sym char val index into tbl174
	iIndex = 0;
	while (symValue >= tbl174[iIndex+5]) {
		symValue -= tbl174[iIndex+5];
		iIndex += 6;
	}

	// get odd elements N and max
	elementN = tbl174[iIndex];
	elementMax = tbl174[iIndex+1];
	saveVal = value = symValue / tbl174[iIndex+4];

	// generate and store odd element widths:
	widths = getRSSwidths(value, elementN, K, elementMax, 0);
	for (i = 0; i < 4; i++) {
		if (forwardFlag) {
			bars[i*2] = (INT8U)widths[i]; // store in 0,2,4,6
		}
		else {
			bars[7 - i*2] = (INT8U)widths[i]; // else in 7,5,3,1
		}
		parity += wgtOdd * widths[i];
		parity = parity % PARITY_MOD_EXP;
		wgtOdd = (wgtOdd * 9) % PARITY_MOD_EXP;
	}

	// calculate even elements value:
	value = symValue - (tbl174[iIndex+4] * saveVal);
	elementN = tbl174[iIndex+2];
	elementMax = tbl174[iIndex+3];

	// generate and store even element widths:
	widths = getRSSwidths(value, elementN, K, elementMax, 1);
	for (i = 0; i < 4; i++) {
		if (forwardFlag) {
			bars[1 + i*2] = (INT8U)widths[i]; // store in 1,3,5,7
		}
		else {
			bars[6 - i*2] = (INT8U)widths[i]; // else in 6,4,2,0
		}
		parity += wgtEven * widths[i];
		parity = parity % PARITY_MOD_EXP;
		wgtEven = (wgtEven * 9) % PARITY_MOD_EXP;
	}
	return(parity);
}

// gets the next 12 bit sym char from bit string
int getVal12(INT8U bitString[], int symNdx) 
{
	int val, ndx;

	ndx = symNdx*3/2; // index into bitString
	if (symNdx & 1) {
		// odd sym char count, so val = 4+8 bits
		val = (int)bitString[ndx] & 0xF;
		val = (val << 8) + (int)bitString[ndx+1];
	}
	else {
		// even sym char count, so val = 8+4 bits
		val = (int)bitString[ndx];
		val = (val << 4) + ((int)bitString[ndx+1] >> 4);
	}
	return(val);
}

// looks for '^' (symbol separator) in string and returns char index iff found
int isSymbolSepatator(INT8U string[]) 
{
	int i;

	for (i = 0; i < (int)strlen(string); i++) {
		if (string[i] == '^') {
			return(i);
		}
	}
	return(0);
}

INT16U yymmdd(INT8U str[]) 
{
	INT16U val;

	val = ((INT16U)(str[0]-'0')*10 + (INT16U)(str[1]-'0')) * 384; // YY					
	val += ((INT16U)(str[2]-'0')*10 + (INT16U)(str[3]-'0') - 1) * 32; // MM					
	val += (INT16U)(str[4]-'0')*10 + (INT16U)(str[5]-'0'); // DD
					
	return(val);
}

// converts 13 digits to 44 bits
void cnv13 (INT8U str[], int *iStr, INT8U bitField[], int *iBit) 
{
	int i;

	putBits(bitField, *iBit, 4, (INT16U)(str[*iStr] - '0')); // high order 4 bits
	*iBit += 4;
	*iStr += 1;
	for (i = 0; i < 4 ; i++) {
		putBits(bitField, *iBit, 10, (INT16U)((INT16U)(str[*iStr] - '0')*100 +
							(str[*iStr+1] - '0')*10 +
							str[*iStr+2] - '0')); // 10 bit groups bits
		*iBit += 10;
		*iStr += 3;
	}
	return;
}

// converts 12 digits to 40 bits
void cnv12 (INT8U str[], int *iStr, INT8U bitField[], int *iBit) 
{
	int i;

	for (i = 0; i < 4 ; i++) {
		putBits(bitField, *iBit, 10, (INT16U)((INT16U)(str[*iStr] - '0')*100 +
							(str[*iStr+1] - '0')*10 +
							str[*iStr+2] - '0')); // 10 bit groups bits
		*iBit += 10;
		*iStr += 3;
	}
	return;
}

int doLinMethods(INT8U str[], int *iStr, INT8U bitField[], int *iBit) 
{

	INT16U bits;
	long weight;
	char numStr[10];

	if (strlen(str) >= 26) {
		strncpy(numStr, &str[20], 6); // possible weight field
		numStr[6] = '\0';
	}
	// look for AI 01
	if (strlen(str)>=16 && str[0]=='0' && str[1]=='1') {

		// look for fixed length with AI 01[9] + 3103[0-32767]
		if (str[2]=='9' && (strlen(str)==16+10) &&
			str[16]=='3' && str[17]=='1' && str[18]=='0' && str[19]=='3' &&
			(weight=atol(numStr))<=32767L) {
			// method 0100, AI's 01 + 3103
			putBits(bitField, *iBit, 4, 4); // write method
			*iBit += 4;
			*iStr += 3; // skip AI 01 and PI 9
			cnv12(str, iStr, bitField, iBit); // write PID-12
			putBits(bitField, *iBit, 15, (INT16U)weight); // write weight
			*iBit += 15;
			*iStr += 1+10; // skip check digit & jump weight field
		}

		// look for fixed length with AI 01[9] + 3202[0-009999]
		else if (str[2]=='9' && (strlen(str)==16+10) &&
			str[16]=='3' && str[17]=='2' && str[18]=='0' && str[19]=='2' &&
			(weight=atol(numStr))<=9999L) {
			// method 0101, AI's 01 + 3202
			putBits(bitField, *iBit, 4, 5); // write method
			*iBit += 4;
			*iStr += 3; // skip AI 01 and PI 9
			cnv12(str, iStr, bitField, iBit); // write PID-12
			putBits(bitField, *iBit, 15, (INT16U)weight); // write weight
			*iBit += 15;
			*iStr += 1+10; // skip check digit & jump weight field
		}

		// look for fixed length with AI 01[9] + 3203[0-022767]
		else if (str[2]=='9' && (strlen(str)==16+10) &&
			str[16]=='3' && str[17]=='2' && str[18]=='0' && str[19]=='3' &&
			(weight=atol(numStr))<=22767L) {
			// method 0101, AI's 01 + 3203
			putBits(bitField, *iBit, 4, 5); // write method
			*iBit += 4;
			*iStr += 3; // skip AI 01 and PI 9
			cnv12(str, iStr, bitField, iBit); // write PID-12
			putBits(bitField, *iBit, 15, (INT16U)weight+10000); // write weight
			*iBit += 15;
			*iStr += 1+10; // skip check digit & jump weight field
		}

		// look for AI 01[9] + 392[0-3]
		else if (str[2]=='9' && (strlen(str)>=16+4+1) &&
			str[16]=='3' && str[17]=='9' && str[18]=='2' &&
			(str[19]>='0' && str[19]<='3')) {
			// method 01100, AI's 01 + 392x + G.P.
			putBits(bitField, *iBit, 5+2, 0x0C<<2); // write method + 2 VLS bits
			*iBit += 5+2;
			*iStr += 3; // skip AI 01 and PI 9
			cnv12(str, iStr, bitField, iBit); // write PID-12
			putBits(bitField, *iBit, 2, (INT16U)(str[19]-'0')); // write D.P.
			*iBit += 2;
			*iStr += 1+4; // skip check digit & jump price AI
		}

		// look for AI 01[9] + 393[0-3]
		else if (str[2]=='9' && (strlen(str)>=16+4+3+1) &&
			str[16]=='3' && str[17]=='9' && str[18]=='3' &&
			(str[19]>='0' && str[19]<='3')) {
			// method 01101, AI's 01 + 393x[NNN] + G.P.
			putBits(bitField, *iBit, 5+2, 0x0D<<2); // write method + 2 VLS bits
			*iBit += 5+2;
			*iStr += 3; // skip AI 01 and PI 9
			cnv12(str, iStr, bitField, iBit); // write PID-12
			putBits(bitField, *iBit, 2, (INT16U)(str[19]-'0')); // write D.P.
			*iBit += 2;
			*iStr += 1+4; // skip check digit & jump price AI
			strncpy(numStr, &str[20], 3); // ISO country code
			numStr[3] = '\0';
			putBits(bitField, *iBit, 10, atoi(numStr)); // write ISO c.c.
			*iBit += 10;
			*iStr += 3; // jump ISO country code
		}

		// look for fixed length with AI 01[9] + 310x/320x[0-099999]
		else if (str[2]=='9' && (strlen(str)==16+10) &&
			str[16]=='3' && (str[17]=='1' || str[17]=='2') && str[18]=='0' &&
			(weight=atol(numStr))<=99999L) {
			// methods 0111000-0111001, AI's 01 + 3x0x no date
			bits = 0x38+(str[17]-'1');
			putBits(bitField, *iBit, 7, bits); // write method
			*iBit += 7;
			*iStr += 3; // skip AI 01 and PI 9
			cnv12(str, iStr, bitField, iBit); // write PID-12
			weight = weight + ((long)(str[19] - '0') * 100000L); // decimal digit
			putBits(bitField, *iBit, 4, (INT16U)(weight>>16)); // write weight
			putBits(bitField, *iBit+4, 16, (INT16U)(weight&0xFFFF));
			*iBit += 20;
			*iStr += 1+10; // jump check digit and weight field
			putBits(bitField, *iBit, 16, (INT16U)38400); // write no date
			*iBit += 16;
		}

		// look for fixed length + AI 01[9] + 310x/320x[0-099999] + 11/13/15/17
		else if (str[2]=='9' && strlen(str)==16+10+8 &&
			str[16]=='3' && (str[17]=='1' || str[17]=='2') && str[18]=='0' &&
			(weight=atol(numStr))<=99999L &&
			str[26]=='1' &&
			(str[27]=='1' || str[27]=='3' || str[27]=='5' || str[27]=='7')) {
			// methods 0111000-0111111, AI's 01 + 3x0x + 1x
			bits = 0x38+(str[27]-'1')+(str[17] - '1');
			putBits(bitField, *iBit, 7, bits); // write method
			*iBit += 7;
			*iStr += 3; // skip AI 01 and PI 9
			cnv12(str, iStr, bitField, iBit); // write PID-12
			weight = weight + ((long)(str[19] - '0') * 100000L); // decimal digit
			putBits(bitField, *iBit, 4, (INT16U)(weight>>16)); // write weight
			putBits(bitField, *iBit+4, 16, (INT16U)(weight&0xFFFF));
			*iBit += 20;
			*iStr += 11; // jump check digit & weight field
			putBits(bitField, *iBit, 16, yymmdd(&str[*iStr+2])); // write date
			*iBit += 16;
			*iStr += 8; // date field
		}
		else {
			// method 1 (plus 2-bit variable lng sym bit fld), AI 01
			putBits(bitField, *iBit, 1+2, 1<<2); // write method + 2 VLS bits
			*iBit += 1+2;
			*iStr += 2;
			cnv13(str, iStr, bitField, iBit);
			*iStr += 1; //skip check digit
		}
	}
	else {
		// method 00 (plus 2-bit variable lng sym bit fld), not AI 01
		putBits(bitField, *iBit, 2+2, 0); // write method + 2 VLSB bits
		*iBit += 2+2;
	}
	return(NUM_MODE);
}

int procISO(struct encodeT *encode) 
{
	int chr, i, what, whatN, numCnt;

	// check next char type
	if ((what = iswhat[(chr = encode->str[encode->iStr])]) == IS_FINI) {
		// end of data
		return(FINI_MODE);
	}
	numCnt = 0;
	if (((what & IS_ALNU) != 0) && ((what & IS_FNC1) == 0)) {
		// next is ALNU (& not FNC1), look 9 for more ALNU
		if ((what & IS_NUM) != 0) {
			// also count leading "digits"
			numCnt = 1;
		}
		for (i = 1; i < 10; i++) {
			whatN = iswhat[encode->str[(encode->iStr)+i]];
			if (whatN == IS_FINI) {
				if ((numCnt >= 4) || (numCnt <= -4)) {
					// latch numeric if >= 4 numbers at end
					putBits(encode->bitField, encode->iBit, 3, 0);
					encode->iBit += 3;
					return(NUM_MODE);
				}
				if (i >= 5) {
					// latch ALNU if >= 5 alphanumbers at end
					putBits(encode->bitField, encode->iBit, 5, 4);
					encode->iBit += 5;
					return(ALNU_MODE);
				}
				else {
					break;
				}
			}
			if ((whatN & IS_NUM) != 0) {
				if (numCnt > 0) {
					// count leading diits
					numCnt++;
				}
			}
			else if (numCnt > 0) {
				// stop counting if not a digit
				numCnt = -numCnt;
			}
			if ((whatN & IS_ALNU) == 0) {
				break;
			}
		}
		if (i == 10) {
			if ((numCnt >= 4) || (numCnt <= -4)) {
				// latch numeric if >= 4 numbers follow & no ISO chars in next 10
				putBits(encode->bitField, encode->iBit, 3, 0);
				encode->iBit += 3;
				return(NUM_MODE);
			}
			else {
				// latch ALNU if no ISO only chars in next 10
				putBits(encode->bitField, encode->iBit, 5, 4);
				encode->iBit += 5;
				return(ALNU_MODE);
			}
		}
	}
	// process char in ISO mode
	encode->iStr += 1;
	if ((what & IS_NUM) != 0) {
		// FNC1 or 0-9
		if ((what & IS_FNC1) != 0) {
			chr = 0xF;
			encode->mode = NUM_MODE;
		}
		else {
			chr = chr - (int)'0' + 5;
		}
		putBits(encode->bitField, encode->iBit, 5, (INT16U)chr);
		encode->iBit += 5;
	}
	else if ((chr >= (int)'A') && (chr <= (int)'Z')) {
		// A-Z
		chr = chr - (int)'A' + 0x40;
		putBits(encode->bitField, encode->iBit, 7, (INT16U)chr);
		encode->iBit += 7;
	}
	else if ((chr >= (int)'a') && (chr <= (int)'z')) {
		// a-z
		chr = chr - (int)'a' + 0x5A;
		putBits(encode->bitField, encode->iBit, 7, (INT16U)chr);
		encode->iBit += 7;
	}
	else {
		if (chr == 32) {
			chr = 0xFC; // sp
		}
		else if (chr == SYM_SEP) {
			chr = 0xFD; // ^ (symbol separator)
			encode->mode = NUM_MODE;
		}
		else if (chr == 95) {
			chr = 0xFB; // _
		}
		else if (chr >= 58) {
			chr = chr - 58 + 0xF5; // :-?
		}
		else if (chr >= 37) {
			chr = chr - 37 + 0xEA; // %-/
		}
		else {
			chr = chr - 33 + 0xE8; // !-"
		}
		putBits(encode->bitField, encode->iBit, 8, (INT16U)chr);
		encode->iBit += 8;
	}
	return(encode->mode);
}
int testAI90(struct encodeT *encode) 
{
	INT8U chr;

	// possible method "11"
	// get DI number - diNum, DI alpha - diAlpha, and start of data - ndx
	encode->diNum = -1;
	if (isupper(encode->str[encode->iStr+2])) {
		encode->diAlpha = encode->str[encode->iStr+2];
		encode->diNum = 0;
		encode->iStr += 3;
	}
	else if (isdigit(encode->str[encode->iStr+2]) &&
			isupper(encode->str[encode->iStr+3])) {
		encode->diAlpha = encode->str[encode->iStr+3];
		encode->diNum = encode->str[encode->iStr+2] - '0';
		encode->iStr += 4;
	}
	else if (isdigit(encode->str[encode->iStr+2]) &&
				isdigit(encode->str[encode->iStr+3]) &&
				isupper(encode->str[encode->iStr+4])) {
		encode->diAlpha = encode->str[encode->iStr+4];
		chr = encode->str[encode->iStr+4];
		encode->str[encode->iStr+4] = '\0';
		encode->diNum = atoi(&encode->str[encode->iStr+2]);
		encode->str[encode->iStr+4] = chr;
		encode->iStr += 5;
	}
	else if (isdigit(encode->str[encode->iStr+2]) &&
						isdigit(encode->str[encode->iStr+3]) &&
						isdigit(encode->str[encode->iStr+4]) &&
						isupper(encode->str[encode->iStr+5])) {
		encode->diAlpha = encode->str[encode->iStr+5];
		chr = encode->str[encode->iStr+5];
		encode->str[encode->iStr+5] = '\0';
		encode->diNum = atoi(&encode->str[encode->iStr+2]);
		encode->str[encode->iStr+5] = chr;
		encode->iStr += 6;
	}
	return(encode->diNum >= 0);
}

void encodeAI90(struct encodeT *encode) 
{
	while ((encode->str[encode->iStr-1] != FNC1) &&
					(encode->str[encode->iStr-2] != FNC1) &&
					(encode->str[encode->iStr-1] != SYM_SEP) &&
					(encode->mode != FINI_MODE)) {
		switch (encode->mode) {
		case NUM_MODE: {
			if (encode->str[encode->iStr] == FNC1) {
				if (encode->typeAI == AI21) {
					// move up char after "21" in case it is needed for NUM_MODE
					encode->str[encode->iStr+1] = encode->str[encode->iStr+3];
				}
				else if (encode->typeAI == AI8004) {
					// move up char after "8004" in case it is needed for NUM_MODE
					encode->str[encode->iStr+1] = encode->str[encode->iStr+5];
				}
			}
			encode->mode = procNUM(encode);
			break;
		}
		case ALNU_MODE: {
			encode->mode = procALNU(encode);
			break;
		}
		case ISO_MODE: {
			encode->mode = procISO(encode);
			break;
		}
		case ALPH_MODE: {
			encode->mode = procALPH(encode);
			break;
		}
		default: {
//			printf("\nmode error");
			errFlag = TRUE;
			return;
		} } /* end of case */
	}
	return;
}

void nextAI(struct encodeT *encode) 
{
	if (encode->str[encode->iStr+0] == '2' &&
			encode->str[encode->iStr+1] == '1') {
		encode->typeAI = AI21;
		encode->iStr += 2;
	}
	else if (encode->str[encode->iStr+0] == '8' &&
			encode->str[encode->iStr+1] == '0' &&
			encode->str[encode->iStr+2] == '0' &&
			encode->str[encode->iStr+3] == '4') {
		encode->typeAI = AI8004;
		encode->iStr += 4;
	}
	else {
		encode->typeAI = AIx; // not AI 21 or 8004
	}
	return;
}

void procAI90(struct encodeT *encode) 
{

	int i, j, k;
	int alLessNu;
	int diNum1, diAlpha1;
	static INT8U alphaTbl[] = "BDHIJKLNPQRSTVWZ"; // strlen must be 16

	// method "11", look ahead to find best compaction scheme
	j = 10000; // 10000: initial flag for non-numeric index
	alLessNu = 0; // upper-case - digit, < -9000 if non-alnu seen
	for (i = encode->iStr; i < strlen(encode->str); i++) {
		if ((encode->str[i] == FNC1) || (encode->str[i] == SYM_SEP)) {
			break; // found it
		}
		if (j == 10000 && !isdigit(encode->str[i])) {
			j = i; // save first non-numeric index
		}
		if (isdigit(encode->str[i])) {
			alLessNu--;
		}
		else if (isupper(encode->str[i])) {
			alLessNu++;
		}
		else {
			alLessNu = -10000; // flag that char not digit or upper seen
		}
	}
	diNum1 = encode->diNum; // save in case nextAI called
	diAlpha1 = encode->diAlpha; // save in case nextAI called
	if (encode->str[i] == FNC1) {
		k = encode->iStr;
		encode->iStr = i+1;
		nextAI(encode);
		encode->iStr = k;
	}
	else {
		encode->typeAI = AIx;
	}
	// do encodation bit(s)
	if (alLessNu > 0) {
		putBits(encode->bitField, encode->iBit, 2, 3); // 11: alpha encoding
		encode->iBit += 2;
		encode->mode = ALPH_MODE;
	}
	else if (i > j && j-encode->iStr < 4) {
		putBits(encode->bitField, encode->iBit, 1, 0); // 0: alphanumeric encoding
		encode->iBit += 1;
		encode->mode = ALNU_MODE;
	}
	else {
		putBits(encode->bitField, encode->iBit, 2, 2); // 10: numeric encoding
		encode->iBit += 2;
		encode->mode = NUM_MODE;
	}
	// next AI is 1 or 2 bit field
	if (encode->typeAI == AIx) {
		putBits(encode->bitField, encode->iBit, 1, 0); // 0: not AI 21 or 8004
		encode->iBit += 1;
	}
	else { // 10: AI 21 or 11: AI 8004
		putBits(encode->bitField, encode->iBit, 2, encode->typeAI);
		encode->iBit += 2;
	}
	for (j = 0; j < 16; j++) {
		if (diAlpha1 == alphaTbl[j]) {
			break;
		}
	}
	if (diNum1 < 31 && j < 16) {
		putBits(encode->bitField, encode->iBit, 5, diNum1); // DI number < 31
		putBits(encode->bitField,
				encode->iBit+5, 4, j); // DI alpha from alphaTbl
		encode->iBit += 9;
	}
	else {
		putBits(encode->bitField, encode->iBit, 5, 31);
		putBits(encode->bitField,
				encode->iBit+5, 10, diNum1); // DI number >= 31
		putBits(encode->bitField,
				encode->iBit+15, 5, diAlpha1-65); // or alpha not in table
		encode->iBit += 20;
	}
	encodeAI90(encode);
	if (encode->typeAI == AI21) {
		encode->iStr += 2; // skip "21"
	}
	else if (encode->typeAI == AI8004) {
		encode->iStr += 4; // skip "8004"
	}
	return;
}

int doMethods(struct encodeT *encode) 
{
	INT16U bits;
	int iStr1;

	if (strlen(encode->str) >= 8 && encode->str[0] == '1' &&
				(encode->str[1] == '1' || encode->str[1] == '7')) {
		// method "10"
		putBits(encode->bitField, 0, 2, 2); // mfg/exp date-lot encodation method bit flag 10
		bits = ((INT16U)(encode->str[2]-'0')*10 +
				(INT16U)(encode->str[3]-'0')) * 384; // YY
		bits += ((INT16U)(encode->str[4]-'0')*10 +
				(INT16U)(encode->str[5]-'0') - 1) * 32; // MM
		bits += (INT16U)(encode->str[6]-'0')*10 +
				(INT16U)(encode->str[7]-'0'); // DD
		putBits(encode->bitField, 2, 16, bits); // date packed data
		putBits(encode->bitField, 2+16, 1,
				(INT16U)((encode->str[1] == '1') ? 0 : 1)); // 0/1 bit for AI 11/17
		if (encode->str[8] == '1' && encode->str[9] == '0' &&
				encode->str[10] != '#') {
			encode->iStr = 2+6+2; // lot data follows
			encode->iBit = 2+16+1;
		}
		else {
			encode->str[7] = '#'; // insert FNC1 to indicate no lot
			encode->iStr = 2+6-1;
			encode->iBit = 2+16+1;
		}
		return(NUM_MODE);
	}
	if (encode->str[0] == '9' &&
			encode->str[1] == '0' &&
			testAI90(encode)) {
		putBits(encode->bitField, encode->iBit, 2, 3); // method 11
		encode->iBit += 2;
		procAI90(encode);
	}
	else {
		// method 0
		putBits(encode->bitField, 0, 1, 0); // g.p. encodation method bit flag 0
		encode->iBit = 1;
		encode->mode = NUM_MODE;
		encode->iStr = 0;
	}
	return(encode->mode);
}

int pack(INT8U str[], INT8U bitField[] ) 
{
	struct encodeT encode;
	int i;

	encode.str = str;
	encode.bitField = bitField;
	encode.iStr = encode.iBit = 0;

	if (linFlag == 1) {
		encode.iBit++; // skip composite link bit if linear component
		encode.mode = doLinMethods(encode.str, &encode.iStr, encode.bitField, &encode.iBit);
	}
	else {
		encode.mode = doMethods(&encode);
	}
	while (encode.mode != FINI_MODE) {
		switch (encode.mode) {
			case NUM_MODE: 
				encode.mode = procNUM(&encode);
				break;			
			case ALNU_MODE: 
				encode.mode = procALNU(&encode);
				break;			
			case ISO_MODE: 
				encode.mode = procISO(&encode);
				break;
			default: 
//				printf("\nmode error");
				errFlag = TRUE;
				return(-1);			 
		} /* end of case */
	}
  if (linFlag == -1) { // CC-C
		if (!insertPad(&encode)) { // will return FALSE if error
			printf("symbol too big\n");
			return(-1);
		}
		return(encode.iBit/8); // no error, return number of data bytes
	}
	else { // CC-A/B or RSS Exp
		return(insertPad(&encode));
	}
}

// convert AI string to bar widths in dbl segments
int RSS14Eenc(INT8U string[], INT8U bars[MAX_DBL_SEGS][ELMNTS_EXP], int ccFlag) 
{
	int i, j;
	int parity, weight;
	int symValue;
	int size, fndrNdx, fndrSetNdx;
	INT8U bitField[MAX_DBL_SEGS*3];

	linFlag = 1;
	parity = 0;

//[start]gm
//	if (((i=check2DData(string)) != 0) || ((i=isSymbolSepatator(string)) != 0)) {
//		printf("\nillegal character in RSS Expanded data = '%c'", string[i]);
//		errFlag = TRUE;
//		return(0);
//	}
//[end]gm
	putBits(bitField, 0, 1, (INT16U)ccFlag); // 2D linkage bit
	size = pack(string, bitField);
	if (size < 0) {
//		printf("\ndata error");
		errFlag = TRUE;
		return(0);
	}

	// note size is # of data chars, not segments
	if ((bitField[0]&0x40) == 0x40) {
		// method 1, insert variable length symbol bit field
		bitField[0] |= (((size+1)&1)<<5) + ((size > 13)?0x10:0);
	}
	if ((bitField[0]&0x60) == 0) {
		// method 00, insert variable length symbol bit field
		bitField[0] |= (((size+1)&1)<<4) + ((size > 13)?8:0);
	}
	if ((bitField[0]&0x71) == 0x30) {
		// method 01100/01101, insert variable length symbol bit field
		bitField[0] |= (((size+1)&1)<<1) + ((size > 13)?1:0);
	}
	fndrSetNdx = (size - 2) / 2;

	for (i = 0; i < (size+2)/2; i++) { // loop through all dbl segments
		fndrNdx = finderSets[fndrSetNdx][i];
		// fill left data char in dbl segment if not first (check char)
		j = (fndrNdx >= 0) ? fndrNdx*2 : -fndrNdx*2+1;
		if (i > 0) {
			weight = parWts[2*(j-2)];
			symValue = getVal12(bitField, i*2-1);
			parity = symCharPat(&bars[i][0], symValue, parity, weight, TRUE);
		}
		// fill finder for dbl segment
		if (fndrNdx < 0) { // reversed finder
			bars[i][12] = finders[-fndrNdx-1][0];
			bars[i][11] = finders[-fndrNdx-1][1];
			bars[i][10] = finders[-fndrNdx-1][2];
			bars[i][9] = 1;
			bars[i][8] = 1;
		}
		else { // forward finder
			bars[i][8] = finders[fndrNdx-1][0];
			bars[i][9] = finders[fndrNdx-1][1];
			bars[i][10] = finders[fndrNdx-1][2];
			bars[i][11] = 1;
			bars[i][12] = 1;
		}
		// fill right data char in dbl segment if it exists
		if (size > i*2) {
			weight = parWts[2*(j-2)+1];
			symValue = getVal12(bitField, i*2);
			parity = symCharPat(&bars[i][8+5], symValue, parity, weight, FALSE);
		}
	}
	// fill in first parity char
	symCharPat(bars[0], (size-3)*PARITY_MOD_EXP + parity, 0, weight, TRUE);
	return(size+1);
}

INT16U EXP_upperseperatorLen[RSSEXP_STACK_MAX_ROW_NUM];
INT16U EXP_lowerseperatorLen[RSSEXP_STACK_MAX_ROW_NUM];
INT16U EXP_midsepLen[RSSEXP_STACK_MAX_ROW_NUM];	
INT8U DataPattern[RSSEXP_STACK_MAX_ROW_NUM][MAX_DBL_SEGS*ELMNTS_EXP+4];
INT8U upsegsPattern[RSSEXP_STACK_MAX_ROW_NUM][MAX_DBL_SEGS*SYM_W_EXP+2];
INT8U midsegsPattern[RSSEXP_STACK_MAX_ROW_NUM][MAX_DBL_SEGS*SYM_W_EXP+2];
INT8U lowsegsPattern[RSSEXP_STACK_MAX_ROW_NUM][MAX_DBL_SEGS*SYM_W_EXP+2];
short BarRSSExp(HUGEDATA char *data, HUGEDATA INT8U *pattern, INT8U *line)
{
	INT8U linPattern[MAX_DBL_SEGS*ELMNTS_EXP+4];
	INT8U tmpbars[MAX_DBL_SEGS*ELMNTS_EXP+4];
	INT8U chexPattern[MAX_DBL_SEGS*SYM_W_EXP+2];
	INT8U dblPattern[MAX_DBL_SEGS][ELMNTS_EXP];

    int i, j, k;
    int rows, ccFlag;
    int segs, lNdx, lNdx1, lMods, lHeight;
    int evenRow, rev;
    int sepCnt;
    int chexSize;
    char *ccStr;
    int rPadl1, rPadcc;
    int tmpVal;

    INT8U startPrtType; 
    int leng;
      
    int lineNo;
    int lastsegs;
    int INdx_array[RSSEXP_STACK_MAX_ROW_NUM];
    INT8U reverseDirectFlag;
    INT8U tmpBuf[MAX_DBL_SEGS*ELMNTS_EXP+4];
    
    INT8U tmpSegWidth; 
    int segmentNo;//KYK, 1st segment's number
    
    HUGEDATA char *cp;
    short q;
    int maxlNdx;
    
    maxlNdx = 0;

	for (i=0; i<RSSEXP_STACK_MAX_ROW_NUM; i++)
	{
		for(j=0;j<MAX_DBL_SEGS*ELMNTS_EXP+4; j++)
		{
			DataPattern[i][j] = 0x00;
			upsegsPattern[i][j] = 0x00;
			midsegsPattern[i][j] = 0x00;
			lowsegsPattern[i][j] = 0x00;
		}
	}
	for (j = 0; j < (MAX_DBL_SEGS*ELMNTS_EXP)+4; j++)
	{
		tmpBuf[j] = 0;
	}

	for (j = 0; j < RSSEXP_STACK_MAX_ROW_NUM; j++)
	{
		EXP_upperseperatorLen[j] = 0;
		EXP_lowerseperatorLen[j] = 0;
		EXP_midsepLen[j] = 0;
	}
    
	errFlag = FALSE;
	ccFlag = FALSE;//2D flag
	//tmpSegWidth = 22; //when using rss expand stacked, we should modify here.
	tmpSegWidth = BarStruct.BearerBarWidth;
	if (tmpSegWidth <= 0 || tmpSegWidth > 20) tmpSegWidth = 6;
	rowWidth = tmpSegWidth; // save for getUnusedBitCnt
	segmentNo = tmpSegWidth; //KYK, test: 1st segment is 6.
	if ((segs = RSS14Eenc((INT8U *)data, dblPattern, ccFlag)) > 0) 
    {
		if (segmentNo % 2) 
		{ 
			//printf("error: segment no is odd\r\n");
			return;
		}

		if (segmentNo > segs ) 
		{ 
			//printf("error: segment no is over\r\n");
			return;
		}
		reverseDirectFlag = 0;
		if ((segmentNo % 4) == 0 && (segmentNo <= 20)) //even line's direction is reverse.
		{
			reverseDirectFlag = 1;
		}

		lineNo = 1;
		lastsegs = segmentNo;
		for (i = 0; i < RSSEXP_STACK_MAX_ROW_NUM; i++)//line is 11
		{
			if ((segs - (i*segmentNo)) > segmentNo) ;//continue;
			else if ((segs - (i*segmentNo)) == segmentNo) break;
			else 
			{
				lastsegs = segs - (i*segmentNo);
				break;
			}
			lineNo++;
		}
		*line = lineNo;
		//printf("lines no= %d, last seg= %d\r\n", lineNo, lastsegs);
//		if (errFlag) {
//			printf("\nRSS Exp encoding error occurred.");
//			return;
//		}

		for (k = 0; k < lineNo; k++)
		{
			lNdx = 0;
			if ( k == (lineNo-1)) //last line
			{
				for (i = segmentNo*k; i < segs-1; i += 2) {
					for (j = 0; j < 8+5+8; j++) { // copy double segments
						DataPattern[k][lNdx++] = dblPattern[i/2][j];
					}
				}
				if (i ==  segs-1) {
					for (j = 0; j < 8+5; j++) { // copy last odd segment if one exists
						DataPattern[k][lNdx++] = dblPattern[i/2][j];
					}
				}
				j = (lastsegs <= tmpSegWidth) ? lastsegs : tmpSegWidth;//????
				i = (lastsegs+j-1)/j; // number of linear rows
				//lHeight =params->pixMult*i*SYM_H_EXP + params->sepHt*(i-1)*3;
                lHeight = 1*i*SYM_H_EXP + 1*(i-1)*3;//KYK temp
				lNdx = (j/2)*(8+5+8) + (j&1)*(8+5);
				INdx_array[k] = lNdx;//length
				lMods = 2 + (j/2)*(17+15+17) + (j&1)*(17+15) + 2;
			}
			else //처음부터 마지막 전 라인까지
			{
				for (i = segmentNo*k; i < segmentNo*(k+1)-1; i += 2) {
					for (j = 0; j < 8+5+8; j++) { // copy double segments
						DataPattern[k][lNdx++] = dblPattern[i/2][j];
					}
				}
				if (i == segmentNo*(k+1)-1) {
					for (j = 0; j < 8+5; j++) { // copy last odd segment if one exists
						DataPattern[k][lNdx++] = dblPattern[i/2][j];
					}
				}
				j = (segmentNo <= tmpSegWidth) ? segmentNo : tmpSegWidth;//????
				i = (segmentNo+j-1)/j; // number of linear rows
				//lHeight =params->pixMult*i*SYM_H_EXP + params->sepHt*(i-1)*3;
				lHeight = 1*i*SYM_H_EXP + 1*(i-1)*3;//KYK temp
                lNdx = (j/2)*(8+5+8) + (j&1)*(8+5);
				INdx_array[k] = lNdx;//length
				lMods = 2 + (j/2)*(17+15+17) + (j&1)*(17+15) + 2;
				if (maxlNdx < lNdx) maxlNdx = lNdx; //find the maximum barcode width
			}
            if(reverseDirectFlag)
            {
                if (k % 2)
                {
                    for (i = 0; i < lNdx; i++)
                    {
                        tmpBuf[i] = DataPattern[k][(lNdx-1)-i];
                    }
                    for (i = 0; i < lNdx; i++)
                    {
                        DataPattern[k][i] = tmpBuf[i];
                    }
                }
            }

            memset(tmpbars, 0, (MAX_DBL_SEGS*ELMNTS_EXP)+4);//????

            //left guard
            tmpbars[0] = 1;
            tmpbars[1] = 1;
            
            //barcode body
            memcpy(&tmpbars[2], DataPattern[k], maxlNdx);
            
            //right guard
            tmpbars[lNdx+2] = 1;
            tmpbars[lNdx+2+1] = 1;
            
            for (i=0; i<maxlNdx+4; i++) { DataPattern[k][i] = tmpbars[i]; }

			// set up checkered seperator pattern and print structure
			for (i = 0; i < MAX_DBL_SEGS*SYM_W_EXP+2; i++) {
				chexPattern[i] = 1; // chex = all 1X elements
			}
			rPadcc = lMods - L_PAD - CCB4_WIDTH;

			tmpVal = 0;
			for (i = 0; i < INdx_array[k]; i++) {
				tmpVal += DataPattern[k][i];
				//printf("%d", DataPattern[k][i]);
			}
			//printf("\r\nlNdx=%d, tmpVal=%d\n", lNdx, tmpVal);
		}
/////////////////////////////////////////// end of DataPattern
/////////////////////////////////////////// start of seperate Pattern
		for (k = 0; k < lineNo-1; k++)
		{
			if (k % 2) startPrtType = FALSE;
			else startPrtType = TRUE;
			
			leng = INdx_array[k];
			EXP_upperseperatorLen[k] = cnvSeparator(&DataPattern[k][2], upsegsPattern[k], startPrtType, leng);

			tmpVal = 0;
			for (i = 0; i < EXP_upperseperatorLen[k]; i++) 
			{
				tmpVal += upsegsPattern[k][i];
				//printf("%d", upsegsPattern[k][i]);
			}
			//printf("\r\nlNdx=%d, tmpVal=%d\n", upperseperatorLen, tmpVal);

			EXP_midsepLen[k] = tmpVal;
			for (i = 0; i < EXP_midsepLen[k]; i++) 
			{
				midsegsPattern[k][i] = chexPattern[i];
			}
			//printf("012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789\r\n");
			tmpVal = 0;
			for (i = 0; i < EXP_midsepLen[k]; i++) //SYM_W/2+2-7
			{
				tmpVal += midsegsPattern[k][i];
				//printf("%d", midsegsPattern[k][i]);
			}
			//printf("\r\nlNdx=%d, tmpVal=%d\n", EXP_midsepLen, tmpVal);
			
			if (k % 2) startPrtType = TRUE;
			else startPrtType = FALSE;
			
			leng = INdx_array[k+1];
			EXP_lowerseperatorLen[k] = cnvSeparator(&DataPattern[k+1][2], lowsegsPattern[k], startPrtType, leng);
			tmpVal = 0;
			for (i = 0; i < EXP_lowerseperatorLen[k]; i++) 
			{
				tmpVal += lowsegsPattern[k][i];
				//printf("%d", lowsegsPattern[k][i]);
			}
			//printf("\r\nlNdx=%d, tmpVal=%d\n", lowerseperatorLen, tmpVal);
		}
	}
    return 1;
}
#endif//#ifdef USE_RSSEXPAND