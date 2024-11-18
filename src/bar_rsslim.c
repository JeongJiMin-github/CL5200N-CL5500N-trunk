/*****************************************************************************
|*			
|*  Copyright		:	(c) 2009 CAS
|*  Filename		:	bar_rsslim.c
|*  Version		:	0.1
|*  Programmer(s)	:	Keum Young-Kwang (gm)
|*  Created		:	2010/01/05
|*  Description		:	barcode RSS Limit routine
|*				
****************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "globals.h"
#include "commbuf.h"
#include "bar_common.h"

#ifdef USE_RSSLIMIT
//RSSLim------------------------------------------------------------
#define	N	26
#define	K_LIM	7
#define PARITY_MOD_LIM 89
#define SUPL_VAL 2015133531096.
// left char multiplier
#define LEFT_MUL_LIM 2013571.
//------------------------------------------------------------------
// not including guard bars
#define ELMNTS 46 // gm include guard bars (46-4)

// symbol width in modules including guard bars:
#define SYM_W_LIM	74//RSS-14L
#define SYM_H_LIM	10// RSS-14L height

#define L_PADB 10 // RSS14L left pad for ccb

//#define CCB4_WIDTH 101 // 4 column cca/b
//#define CCB4_ELMNTS 53 // includes qz's
//
//#define MAX_CCB4_ROWS 44	// ccb-4 max rows
//#define MAX_CCA2_SIZE 6 // index to 167 in CC2Sizes
//#define MAX_CCA3_SIZE 4 // index to 167 in CC3Sizes
//#define MAX_CCA4_SIZE 4 // index to 197 in CC4Sizes
//

#define max(a,b) ((a>b) ? a : b)
#define min(a,b) ((a<b) ? a : b)

//extern int errFlag;
//extern char MsgBuf[];//???
//extern void MsgOut(HUGEDATA char *str);//???

// stores odd element N & max, even N & max, odd mul, combos
ROMDATA INT32U oddEvenTbl[1*7*6] = { 	/* 26,7 */
	17,6,	9,3,	28,	183064,
	13,5,	13,4,	728,	637000,
	9,3,	17,6,	6454,	180712,
	15,5,	11,4,	203,	490245,
	11,4,	15,5,	2408,	488824,
	19,8,	7,1,	1,	17094,
	7,1,	19,8,	16632,	16632 
};

ROMDATA INT8U parityPattern[PARITY_MOD_LIM * 14] = {
	 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 3, 1, 1,
	 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 3, 2, 1, 1,
	 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 3, 1, 1, 1,
	 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 3, 2, 1, 1,
	 1, 1, 1, 1, 1, 1, 1, 2, 1, 2, 3, 1, 1, 1,
	 1, 1, 1, 1, 1, 1, 1, 3, 1, 1, 3, 1, 1, 1,
	 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 3, 2, 1, 1,
	 1, 1, 1, 1, 1, 2, 1, 1, 1, 2, 3, 1, 1, 1,
	 1, 1, 1, 1, 1, 2, 1, 2, 1, 1, 3, 1, 1, 1,
	 1, 1, 1, 1, 1, 3, 1, 1, 1, 1, 3, 1, 1, 1,
	 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 3, 2, 1, 1,
	 1, 1, 1, 2, 1, 1, 1, 1, 1, 2, 3, 1, 1, 1,
	 1, 1, 1, 2, 1, 1, 1, 2, 1, 1, 3, 1, 1, 1,
	 1, 1, 1, 2, 1, 2, 1, 1, 1, 1, 3, 1, 1, 1,
	 1, 1, 1, 3, 1, 1, 1, 1, 1, 1, 3, 1, 1, 1,
	 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 3, 2, 1, 1,
	 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 3, 1, 1, 1,
	 1, 2, 1, 1, 1, 1, 1, 2, 1, 1, 3, 1, 1, 1,
	 1, 2, 1, 1, 1, 2, 1, 1, 1, 1, 3, 1, 1, 1,
	 1, 2, 1, 2, 1, 1, 1, 1, 1, 1, 3, 1, 1, 1,
	 1, 3, 1, 1, 1, 1, 1, 1, 1, 1, 3, 1, 1, 1,
	 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 2, 3, 1, 1,
	 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 1, 1,
	 1, 1, 1, 1, 1, 1, 1, 1, 2, 3, 2, 1, 1, 1,
	 1, 1, 1, 1, 1, 1, 1, 2, 2, 1, 2, 2, 1, 1,
	 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 1, 1, 1,
	 1, 1, 1, 1, 1, 1, 1, 3, 2, 1, 2, 1, 1, 1,
	 1, 1, 1, 1, 1, 2, 1, 1, 2, 1, 2, 2, 1, 1,
	 1, 1, 1, 1, 1, 2, 1, 1, 2, 2, 2, 1, 1, 1,
	 1, 1, 1, 1, 1, 2, 1, 2, 2, 1, 2, 1, 1, 1,
	 1, 1, 1, 1, 1, 3, 1, 1, 2, 1, 2, 1, 1, 1,
	 1, 1, 1, 2, 1, 1, 1, 1, 2, 1, 2, 2, 1, 1,
	 1, 1, 1, 2, 1, 1, 1, 1, 2, 2, 2, 1, 1, 1,
	 1, 1, 1, 2, 1, 1, 1, 2, 2, 1, 2, 1, 1, 1,
	 1, 1, 1, 2, 1, 2, 1, 1, 2, 1, 2, 1, 1, 1,
	 1, 1, 1, 3, 1, 1, 1, 1, 2, 1, 2, 1, 1, 1,
	 1, 2, 1, 1, 1, 1, 1, 1, 2, 1, 2, 2, 1, 1,
	 1, 2, 1, 1, 1, 1, 1, 1, 2, 2, 2, 1, 1, 1,
	 1, 2, 1, 1, 1, 1, 1, 2, 2, 1, 2, 1, 1, 1,
	 1, 2, 1, 1, 1, 2, 1, 1, 2, 1, 2, 1, 1, 1,
	 1, 2, 1, 2, 1, 1, 1, 1, 2, 1, 2, 1, 1, 1,
	 1, 3, 1, 1, 1, 1, 1, 1, 2, 1, 2, 1, 1, 1,
	 1, 1, 1, 1, 1, 1, 1, 1, 3, 1, 1, 3, 1, 1,
	 1, 1, 1, 1, 1, 1, 1, 1, 3, 2, 1, 2, 1, 1,
	 1, 1, 1, 1, 1, 1, 1, 2, 3, 1, 1, 2, 1, 1,
	 1, 1, 1, 2, 1, 1, 1, 1, 3, 1, 1, 2, 1, 1,
	 1, 2, 1, 1, 1, 1, 1, 1, 3, 1, 1, 2, 1, 1,
	 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 2, 3, 1, 1,
	 1, 1, 1, 1, 1, 1, 2, 1, 1, 2, 2, 2, 1, 1,
	 1, 1, 1, 1, 1, 1, 2, 1, 1, 3, 2, 1, 1, 1,
	 1, 1, 1, 1, 1, 1, 2, 2, 1, 1, 2, 2, 1, 1,
	 1, 1, 1, 2, 1, 1, 2, 1, 1, 1, 2, 2, 1, 1,
	 1, 1, 1, 2, 1, 1, 2, 1, 1, 2, 2, 1, 1, 1,
	 1, 1, 1, 2, 1, 1, 2, 2, 1, 1, 2, 1, 1, 1,
	 1, 1, 1, 2, 1, 2, 2, 1, 1, 1, 2, 1, 1, 1,
	 1, 1, 1, 3, 1, 1, 2, 1, 1, 1, 2, 1, 1, 1,
	 1, 2, 1, 1, 1, 1, 2, 1, 1, 1, 2, 2, 1, 1,
	 1, 2, 1, 1, 1, 1, 2, 1, 1, 2, 2, 1, 1, 1,
	 1, 2, 1, 2, 1, 1, 2, 1, 1, 1, 2, 1, 1, 1,
	 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 2, 3, 1, 1,
	 1, 1, 1, 1, 2, 1, 1, 1, 1, 2, 2, 2, 1, 1,
	 1, 1, 1, 1, 2, 1, 1, 1, 1, 3, 2, 1, 1, 1,
	 1, 1, 1, 1, 2, 1, 1, 2, 1, 1, 2, 2, 1, 1,
	 1, 1, 1, 1, 2, 1, 1, 2, 1, 2, 2, 1, 1, 1,
	 1, 1, 1, 1, 2, 2, 1, 1, 1, 1, 2, 2, 1, 1,
	 1, 2, 1, 1, 2, 1, 1, 1, 1, 1, 2, 2, 1, 1,
	 1, 2, 1, 1, 2, 1, 1, 1, 1, 2, 2, 1, 1, 1,
	 1, 2, 1, 1, 2, 1, 1, 2, 1, 1, 2, 1, 1, 1,
	 1, 2, 1, 1, 2, 2, 1, 1, 1, 1, 2, 1, 1, 1,
	 1, 2, 1, 2, 2, 1, 1, 1, 1, 1, 2, 1, 1, 1,
	 1, 3, 1, 1, 2, 1, 1, 1, 1, 1, 2, 1, 1, 1,
	 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 3, 1, 1,
	 1, 1, 2, 1, 1, 1, 1, 1, 1, 2, 2, 2, 1, 1,
	 1, 1, 2, 1, 1, 1, 1, 1, 1, 3, 2, 1, 1, 1,
	 1, 1, 2, 1, 1, 1, 1, 2, 1, 1, 2, 2, 1, 1,
	 1, 1, 2, 1, 1, 1, 1, 2, 1, 2, 2, 1, 1, 1,
	 1, 1, 2, 1, 1, 1, 1, 3, 1, 1, 2, 1, 1, 1,
	 1, 1, 2, 1, 1, 2, 1, 1, 1, 1, 2, 2, 1, 1,
	 1, 1, 2, 1, 1, 2, 1, 1, 1, 2, 2, 1, 1, 1,
	 1, 1, 2, 2, 1, 1, 1, 1, 1, 1, 2, 2, 1, 1,
	 2, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 1, 1,
	 2, 1, 1, 1, 1, 1, 1, 1, 1, 3, 2, 1, 1, 1,
	 2, 1, 1, 1, 1, 1, 1, 2, 1, 1, 2, 2, 1, 1,
	 2, 1, 1, 1, 1, 1, 1, 2, 1, 2, 2, 1, 1, 1,
	 2, 1, 1, 1, 1, 1, 1, 3, 1, 1, 2, 1, 1, 1,
	 2, 1, 1, 1, 1, 2, 1, 1, 1, 2, 2, 1, 1, 1,
	 2, 1, 1, 1, 1, 2, 1, 2, 1, 1, 2, 1, 1, 1,
	 2, 1, 1, 2, 1, 1, 1, 1, 1, 2, 2, 1, 1, 1,
	 2, 1, 1, 1, 1, 1, 1, 1, 2, 2, 1, 2, 1, 1
};

ROMDATA INT16U leftWeightsLimit[2*K_LIM] = {1,3,9,27,81,65,17,51,64,14,42,37,22,66};
ROMDATA INT16U rightWeightsLimit[2*K_LIM] = {20,60,2,6,18,54,73,41,34,13,39,28,84,74};

// call with str = 13-digit primary, no check digit
int RSSLimEnc(INT8U string[], INT8U bars[], int ccFlag) 
{
	double data; 
	int value;
	int i;
	int elementN, elementMax, parity;
	long chrValue, chrValSave, longNum;
	int iIndex;
	int *widths;
	INT8U tmpbars[ELMNTS];//gm 091230

	data = atof((char *)string);
	if (data > 1999999999999.) {
		return(FALSE); // item number too large
	}
	if (ccFlag) data += SUPL_VAL;
	
	// calculate left (high order) symbol half value:
	chrValue = chrValSave = (long)(data / LEFT_MUL_LIM);

	// get 1st char index into oddEvenTbl
	iIndex = 0;
	while (chrValue >= oddEvenTbl[iIndex+5]) {
		chrValue -= oddEvenTbl[iIndex+5];
		iIndex += 6;
	}

	// get odd elements N and max
	elementN = (int)oddEvenTbl[iIndex];
	elementMax = (int)oddEvenTbl[iIndex+1];
	longNum = value = chrValue / oddEvenTbl[iIndex+4];

	// generate and store odd element widths:
	widths = getRSSwidths(value, elementN, K_LIM, elementMax, 1);
//sprintf(MsgBuf, "(1st odd)---\r\n");//???
//MsgOut(MsgBuf);
	parity = 0l;
	for (i = 0; i < K_LIM; i++) {
//sprintf(MsgBuf, "%d\r\n", widths[i]);//???
//MsgOut(MsgBuf);
		bars[(i*2)] = (INT8U)widths[i];
		parity += leftWeightsLimit[i * 2] * widths[i];
		parity = parity % PARITY_MOD_LIM;		
	}

	// calculate even elements value:
	value = chrValue - (oddEvenTbl[iIndex+4] * longNum);
	elementN = (int)oddEvenTbl[iIndex+2];
	elementMax = (int)oddEvenTbl[iIndex+3];

	// generate and store even element widths:
	widths = getRSSwidths(value, elementN, K_LIM, elementMax, 0);
//sprintf(MsgBuf, "(1st even)---\r\n");//???
//MsgOut(MsgBuf);
	for (i = 0; i < K_LIM; i++) {
//sprintf(MsgBuf, "%d\r\n", widths[i]);//???
//MsgOut(MsgBuf);
		bars[(i*2)+1] = (INT8U)widths[i];
		parity += leftWeightsLimit[(i * 2) + 1] * widths[i];
		parity = parity % PARITY_MOD_LIM;
	}

	// calculate right (low order) symbol half value:
	chrValue = (long)(data - ((double)chrValSave * LEFT_MUL_LIM));

	// get 2nd char index into oddEvenTbl
	iIndex = 0;
	while (chrValue >= oddEvenTbl[iIndex+5]) {
		chrValue -= oddEvenTbl[iIndex+5];
		iIndex += 6;
	}

	// get odd elements N and max
	elementN = (int)oddEvenTbl[iIndex];
	elementMax = (int)oddEvenTbl[iIndex+1];
	longNum = value = chrValue / oddEvenTbl[iIndex+4];

	// generate and store odd element widths:
	widths = getRSSwidths(value, elementN, K_LIM, elementMax, 1);
//sprintf(MsgBuf, "(2nd odd)---\r\n");//???
//MsgOut(MsgBuf);
	for (i = 0; i < K_LIM; i++) {
//sprintf(MsgBuf, "%d\r\n", widths[i]);//???
//MsgOut(MsgBuf);
		bars[(i*2)+28] = (INT8U)widths[i];
		parity += rightWeightsLimit[i * 2] * widths[i];
		parity = parity % PARITY_MOD_LIM;
	}

	// calculate even elements value:
	value = chrValue - (oddEvenTbl[iIndex+4] * longNum);
	elementN = (int)oddEvenTbl[iIndex+2];
	elementMax = (int)oddEvenTbl[iIndex+3];

	// generate and store even element widths:
	widths = getRSSwidths(value, elementN, K_LIM, elementMax, 0);
//sprintf(MsgBuf, "(2nd even)---\r\n");//???
//MsgOut(MsgBuf);
	for (i = 0; i < K_LIM; i++) {
//sprintf(MsgBuf, "%d\r\n", widths[i]);//???
//MsgOut(MsgBuf);
		bars[(i*2)+1+28] = (INT8U)widths[i];
		parity += rightWeightsLimit[(i * 2) + 1] * widths[i];
		parity = parity % PARITY_MOD_LIM;
	}

	// store parity character in bars[]:
	for (i = 0; i < 14; i++) {
		bars[14 + i] = parityPattern[(parity * 14) + i];
	}
//[start]gm including left/right guard
	//left guard 
	tmpbars[0] = 1;
	tmpbars[1] = 1;
	for (i=0; i<ELMNTS-4; i++) //strlen(bars)
	{
		tmpbars[2+i] = bars[i];
	}
	//right guard
	tmpbars[2+i] = 1;
	tmpbars[2+i+1] = 1;

	for (i=0; i< ELMNTS; i++)
	{
		bars[i] = tmpbars[i];
	}
	//[end]gm
	return(TRUE);
}

short BarRSSLimit(HUGEDATA char *data,char chksum, HUGEDATA INT8U *pattern)
{
	INT8U linPattern[ELMNTS];

	//char primaryStr[14+1];
	//char tempStr[28+1];

	//int i, j;
	//int rows;
	int ccFlag;
	//char *ccStr;
	char ck;
	HUGEDATA char *cp;
	short slen,q;


	ccFlag = FALSE;//gm
	chksum = 0;
	if (RSSLimEnc((INT8U *)data, linPattern, ccFlag)) {
//		if (errFlag) {
//			printf("\nRSS Limited encoding error occurred.");
//			return;
//		}
//for (i = 0; i < ELMNTS; i++) 
//{
//	sprintf(MsgBuf, "%d ", linPattern[i]);//???
//	MsgOut(MsgBuf);
//}
//sprintf(MsgBuf, "\r\n");//???
//MsgOut(MsgBuf);
		cp = (HUGEDATA char *)&linPattern[0];
		q = 0;
		q = BarRSS14MakePattern(cp, ELMNTS, 0, pattern, 0);
	}
	else {
		printf("\nerror occurred, exiting.");
		//return;
	}
	//make barcode string with checksum
	slen = strlen(data);
	ck = BarUpc_Checksum(data, slen);
	*(data + slen) = ck;
	*(data + slen + 1) = NULL;

	return q;
}
#endif//#ifdef USE_RSSLIMIT