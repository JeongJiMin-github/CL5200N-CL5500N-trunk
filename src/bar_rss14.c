/*****************************************************************************
|*			
|*  Copyright		:	(c) 2009 CAS
|*  Filename		:	bar_rss14.c
|*  Version		:	0.1
|*  Programmer(s)	:	Keum Young-Kwang (gm)
|*  Created		:	2009/12/29
|*  Description		:	barcode RSS14 routine (RSS14, RSS14S and RSS14SO)
|*				
****************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "globals.h"
#include "commbuf.h"
#include "bar_common.h"

#ifdef USE_RSS14
//RSS --------------------------------------------------------------
#define K		4
#define	PARITYCHRSIZE	9
#define PARITY_MOD 	79
#define LEFT_MUL 	4537077. 	// left char multiplier
#define	SEMI_MUL	1597		// outside semi-char multipliers
//------------------------------------------------------------------
// not including guard bars
#define ELMNTS 46 // gm include guard bars (46-4)

// symbol width in modules including guard bars:
#define SYM_W		96//RSS-14
#define SYM_H		33// RSS-14 height
// RSS-14 truncated height
#define TRNC_H 		13
// RSS-14S row heights
#define ROWS1_H		5
#define ROWS2_H		7

#define L_PADB 10 // RSS14L left pad for ccb
#define L_PADR 5 // RSS-14 left offset
#define R_PADR 7 // RSS-14s right offset

#define CCB4_WIDTH 101 // 4 column cca/b
#define CCB4_ELMNTS 53 // includes qz's

#define MAX_CCB4_ROWS 44// ccb-4 max rows
#define MAX_CCA2_SIZE 6 // index to 167 in CC2Sizes
#define MAX_CCA3_SIZE 4 // index to 167 in CC3Sizes
#define MAX_CCA4_SIZE 4 // index to 197 in CC4Sizes

#define max(a,b) ((a>b) ? a : b)
#define min(a,b) ((a<b) ? a : b)

// stores even elements N & max, odd N & max, even mul, combos
ROMDATA INT16U tbl154[4*6] = {/* 15,4 */
	10,7,	5,2,	4,	336,
	8,5,	7,4,	20,	700,
	6,3,	9,6,	48,	480,
	4,1,	11,8,	81,	81 
};

// stores odd elements N & max, even N & max, odd mul, combos
ROMDATA INT16U tbl164[5*6] = {/* 16,4 */
	12,8,	4,1,	1,	161,
	10,6,	6,3,	10,	800,
	8,4,	8,5,	34,	1054,
	6,3,	10,6,	70,	700,
	4,1,	12,8,	126,	126 
};

ROMDATA INT8U leftParity[PARITYCHRSIZE * 3] = {
	3,8,2,
	3,5,5,
	3,3,7,
	3,1,9,
	2,7,4,
	2,5,6,
	2,3,8,
	1,5,7,
	1,3,9 
};

ROMDATA INT16U leftWeights[4*K] = {1,3,9,27,2,6,18,54,4,12,36,29,8,24,72,58};
ROMDATA INT16U rightWeights[4*K] = {16,48,65,37,32,17,51,74,64,34,23,69,49,68,46,59};

// call with str = 13-digit primary, no check digit
int RSS14enc(INT8U string[], INT8U bars[], int ccFlag) 
{
	double data;
	int value;
	int i;
	int elementN, elementMax, parity;
	int leftPar, rightPar;
	long chrValue, chrValSave, semiValue, semiValSave, longNum;
	int iIndex;
	int *widths;

	data = atof((char *)string);
	if (ccFlag) data += 10000000000000.;

	bars[11] = 1; // init fixed patterns
	bars[12] = 1;
	bars[29] = 1;
	bars[30] = 1;

	// calculate left (high order) symbol half value:
	chrValue = chrValSave = (long)(data / LEFT_MUL);

	// determine the 1st (left) character
	// get the 1st char odd elements value
	semiValue = semiValSave = chrValue / SEMI_MUL;

	// get 1st char index into tbl164
	iIndex = 0;
	while (semiValue >= tbl164[iIndex+5]) {
		semiValue -= tbl164[iIndex+5];
		iIndex += 6;
	}

	// get odd elements N and max
	elementN = tbl164[iIndex];
	elementMax = tbl164[iIndex+1];
	longNum = value = semiValue / tbl164[iIndex+4];

	// generate and store odd element widths:
	widths = getRSSwidths(value, elementN, K, elementMax, 1);
	parity = 0l;
//sprintf(MsgBuf, "(1st odd)---\r\n");//???
//MsgOut(MsgBuf);
	for (i = 0; i < K; i++) {
//sprintf(MsgBuf, "%d\r\n", widths[i]);//???
//MsgOut(MsgBuf);
		bars[(i * 2)] = (INT8U)widths[i];
		parity += leftWeights[i * 2] * widths[i];
		parity = parity % PARITY_MOD;
	}

	// calculate even elements value:
	value = semiValue - (tbl164[iIndex+4] * longNum);
	elementN = tbl164[iIndex+2];
	elementMax = tbl164[iIndex+3];

	// generate and store even element widths:
	widths = getRSSwidths(value, elementN, K, elementMax, 0);
//sprintf(MsgBuf, "(1st even)---\r\n");//???
//MsgOut(MsgBuf);
	for (i = 0; i < K; i++) {
//sprintf(MsgBuf, "%d\r\n", widths[i]);//???
//MsgOut(MsgBuf);
		bars[1 + (i * 2)] = (INT8U)widths[i];
		parity += leftWeights[(i * 2) + 1] * widths[i];
		parity = parity % PARITY_MOD;
	}

	// get the 2nd char value
	semiValue = chrValue - (semiValSave * SEMI_MUL);

	// get 2nd char index into tbl154
	iIndex = 0;
	while (semiValue >= tbl154[iIndex+5]) {
		semiValue -= tbl154[iIndex+5];
		iIndex += 6;
	}

	// get even elements N and max
	elementN = tbl154[iIndex];
	elementMax = tbl154[iIndex+1];
	longNum = value = semiValue / tbl154[iIndex+4];

	// generate and store even element widths of the 2nd char:
	widths = getRSSwidths(value, elementN, K, elementMax, 1);
//sprintf(MsgBuf, "(2nd even)---\r\n");//???
//MsgOut(MsgBuf);
	for (i = 0; i < K; i++) {
//sprintf(MsgBuf, "%d\r\n", widths[i]);//???
//MsgOut(MsgBuf);
		bars[19 - (i * 2)] = (INT8U)widths[i];
		parity += leftWeights[(i * 2)+1+8] * widths[i];
		parity = parity % PARITY_MOD;
	}

	// calculate 2nd char odd elements value:
	value = semiValue - (tbl154[iIndex+4] * longNum);
	elementN = tbl154[iIndex+2];
	elementMax = tbl154[iIndex+3];

	// generate and store odd element widths:
	widths = getRSSwidths(value, elementN, K, elementMax, 0);
//sprintf(MsgBuf, "(2nd odd)---\r\n");//???
//MsgOut(MsgBuf);
	for (i = 0; i < K; i++) {
//sprintf(MsgBuf, "%d\r\n", widths[i]);//???
//MsgOut(MsgBuf);
		bars[20 - (i * 2)] = (INT8U)widths[i];
		parity += leftWeights[(i * 2)+8] * widths[i];
		parity = parity % PARITY_MOD;
	}

	// calculate right (low order) symbol half value:
	chrValue = (long)(data - ((double)chrValSave * LEFT_MUL));

	// determine the 3rd character
	// get the 3rd char odd elements value
	semiValue = semiValSave = chrValue / SEMI_MUL;

	// get 3rd char index into tbl164
	iIndex = 0;
	while (semiValue >= tbl164[iIndex+5]) {
		semiValue -= tbl164[iIndex+5];
		iIndex += 6;
	}

	// get odd elements N and max
	elementN = tbl164[iIndex];
	elementMax = tbl164[iIndex+1];
	longNum = value = semiValue / tbl164[iIndex+4];

	// generate and store odd element widths:
	widths = getRSSwidths(value, elementN, K, elementMax, 1);
//sprintf(MsgBuf, "(3rd odd)---\r\n");//???
//MsgOut(MsgBuf);
	for (i = 0; i < K; i++) {
//sprintf(MsgBuf, "%d\r\n", widths[i]);//???
//MsgOut(MsgBuf);

		bars[41 - (i * 2)] = (INT8U)widths[i];
		parity += rightWeights[i * 2] * widths[i];
		parity = parity % PARITY_MOD;
	}

	// calculate even elements value:
	value = semiValue - (tbl164[iIndex+4] * longNum);
	elementN = tbl164[iIndex+2];
	elementMax = tbl164[iIndex+3];

	// generate and store even element widths:
	widths = getRSSwidths(value, elementN, K, elementMax, 0);
//sprintf(MsgBuf, "(3rd even)---\r\n");//???
//MsgOut(MsgBuf);
	for (i = 0; i < K; i++) {
//sprintf(MsgBuf, "%d\r\n", widths[i]);//???
//MsgOut(MsgBuf);

		bars[40 - (i * 2)] = (INT8U)widths[i];
		parity += rightWeights[(i * 2) + 1] * widths[i];
		parity = parity % PARITY_MOD;
	}

	// get the 4th char value
	semiValue = chrValue - (semiValSave * SEMI_MUL);

	// get 4th char index into tbl154
	iIndex = 0;
	while (semiValue >= tbl154[iIndex+5]) {
		semiValue -= tbl154[iIndex+5];
		iIndex += 6;
	}

	// get even elements N and max
	elementN = tbl154[iIndex];
	elementMax = tbl154[iIndex+1];
	longNum = value = semiValue / tbl154[iIndex+4];

	// generate and store even element widths of the 4th char:
	widths = getRSSwidths(value, elementN, K, elementMax, 1);
//sprintf(MsgBuf, "(4th even)---\r\n");//???
//MsgOut(MsgBuf);
	for (i = 0; i < K; i++) {
//sprintf(MsgBuf, "%d\r\n", widths[i]);//???
//MsgOut(MsgBuf);

		bars[22 + (i * 2)] = (INT8U)widths[i];
		parity += rightWeights[(i * 2)+1+8] * widths[i];
		parity = parity % PARITY_MOD;
	}

	// calculate 4th char odd elements value:
	value = semiValue - (tbl154[iIndex+4] * longNum);
	elementN = tbl154[iIndex+2];
	elementMax = tbl154[iIndex+3];

	// generate and store odd element widths:
	widths = getRSSwidths(value, elementN, K, elementMax, 0);
//sprintf(MsgBuf, "(4th odd)---\r\n");//???
//MsgOut(MsgBuf);
	for (i = 0; i < K; i++) {
//sprintf(MsgBuf, "%d\r\n", widths[i]);//???
//MsgOut(MsgBuf);

		bars[21 + (i * 2)] = (INT8U)widths[i];
		parity += rightWeights[(i * 2)+8] * widths[i];
		parity = parity % PARITY_MOD;
	}

	// calculate finders
	if (parity >= 8) {
		parity++; // avoid 0,8 doppelganger
	}
	if (parity >= 72) {
		parity++; // avoid 8,0 doppelganger
	}
	leftPar = parity/9;
	rightPar = parity%9;
	// store left (high order) parity character in the bars:
//sprintf(MsgBuf, "(left parity)---\r\n");//???
//MsgOut(MsgBuf);
	for (i = 0; i < 3; i++) {
//sprintf(MsgBuf, "%d\r\n", leftParity[leftPar * 3 + i]);//???
//MsgOut(MsgBuf);
		bars[8 + i] = leftParity[leftPar * 3 + i];
	}

	// store right (low order) parity character in the spaces:
//sprintf(MsgBuf, "(right parity)---\r\n");//???
//MsgOut(MsgBuf);
	for (i = 0; i < 3; i++) {
//sprintf(MsgBuf, "%d\r\n", leftParity[rightPar * 3 + i]);//???
//MsgOut(MsgBuf);
		bars[33 - i] = leftParity[rightPar * 3 + i];
	}
	return(TRUE);
}
short BarRSS14(HUGEDATA char *data,char chksum, HUGEDATA INT8U *pattern)
{
	INT8U linPattern[ELMNTS];
	INT8U tmpbars[ELMNTS];//gm 091230
   	int i;
   	//int j;
	//int rows;
	int ccFlag;
	//char *ccStr;
	//int rPadl, rPadl1, rPadcc;
	//int symHt;

	HUGEDATA char *cp;
	short q;
	INT8U slen;
	INT8U ck;

	//[start]gm 091229
	//if (params->sym == sRSS14) {
	//	symHt = SYM_H;
	//}
	//else {
	// 	symHt = TRNC_H;
	//}		      	
	ccFlag = FALSE;//gm
	chksum = 0; //gm
	//[end]gm 091229

//[start] gm delete
//     	strcpy(tempStr, "000000000000");	
//	strcat(tempStr, data); //gm params->dataStr
//	strcpy(primaryStr, tempStr + strlen(tempStr) - 13);
//[end] gm delete
	if (RSS14enc((INT8U *)data, linPattern, ccFlag)) {
//		if (errFlag) {
//			printf("\nRSS14 encoding error occurred.");
//			return 0;
//		}
	}
//pattern =  &linPattern;
//for (i = 0; i < ELMNTS; i++) 
//{
//	sprintf(MsgBuf, "%d ", linPattern[i]);//???
//	MsgOut(MsgBuf);
//}
//sprintf(MsgBuf, "\r\n");//???
//MsgOut(MsgBuf);
	
	//[start]gm including left/right guard
	//left guard 
	tmpbars[0] = 1;
	tmpbars[1] = 1;
	for (i=0; i<ELMNTS-4; i++) //strlen(bars)
	{
		tmpbars[2+i] = linPattern[i];
	}
	//right guard
	tmpbars[2+i] = 1;
	tmpbars[2+i+1] = 1;

	for (i=0; i< ELMNTS; i++)
	{
		linPattern[i] = tmpbars[i];
	}
	//[end]gm 

	/* Barcode Bitmap Pattern  */
	cp = (HUGEDATA char *)&linPattern[0];
//for (i = 0; i < ELMNTS; i++) 
//{
//	sprintf(MsgBuf, "%d ", *(cp + i));//???
//	MsgOut(MsgBuf);
//}
//sprintf(MsgBuf, "\r\n");//???
//MsgOut(MsgBuf);
	q = 0;
	q = BarRSS14MakePattern(cp, ELMNTS, 0, pattern, 0);

	//make barcode string with checksum
	slen = strlen(data);
	ck = BarUpc_Checksum(data, slen);
	*(data + slen) = ck;
	*(data + slen + 1) = NULL;

	return q;
}

INT8U upperPattern[SYM_W/2+2];
INT8U lowerPattern[SYM_W/2+2];
INT8U uppersepPattern[SYM_W/2+2];
INT8U sepPattern[SYM_W/2+2];
INT8U lowersepPattern[SYM_W/2+2];

INT16U seperatorLen;
INT16U upperseperatorLen;
INT16U lowerseperatorLen;
// RSS14 Stacked row separator pattern routine
INT16U separator14S(INT8U data[], INT8U sepPattern[])
{
	int i, j, k, lNdx, rNdx, sNdx, lWidth, rWidth, matchWidth;

	sepPattern[0] = sepPattern[1] = 1; // start with old SB guard in separator
	lNdx = 0; // left (top) element index
	rNdx = (ELMNTS-4)/2; //modify, ELMNTS/2 -> (ELMNTS-4)/2 right (bottom) element index 
	sNdx = 2; // separator element index
	lWidth = rWidth = matchWidth = 0;
	for (i = 0; i < SYM_W/2 - 2; i++, lWidth--, rWidth--) {
		if (lWidth == 0) {
			lWidth = data[lNdx++]; //prints->pattern[lNdx++] -> data[lNdx++], next left element width
		}
		if (rWidth == 0) {
			rWidth = data[rNdx++]; // prints->pattern[rNdx++] -> data[rNdx++], next left element width
		}
		if (((lNdx - rNdx) & 1) == 1) {
			// top and bottom rows are opposite colors here
			if (matchWidth > 0) {
				// same to opposite, terminate complimentary element
				sepPattern[sNdx++] = matchWidth;
				matchWidth = 0;
			}
			sepPattern[sNdx++] = 1; // 1X elements separate opposite colors
		}
		else {
			if (matchWidth == 0) {
				// opposite to same
				if (((lNdx - sNdx) & 1) == 0) {
					// seperator is opposite color, start new element
					matchWidth = 1;
				}
				else {
					// wrong color, extend previous narrow to match color
					sNdx--;
					matchWidth = 2;
				}
			}
			else {
				// same to same, see if colors reversed
				if (((lNdx - sNdx) & 1) == 1) {
					// yes, terminate previous color
					sepPattern[sNdx++] = matchWidth;
					matchWidth = 1;
				}
				else {
					// no, add to current color
					matchWidth++;
				}
			}
		}
	}
	sepPattern[sNdx] = sepPattern[sNdx+1] = 1; // old right guard pattern

	// insert 4X min space on each end
	for (i = k = 0; k <= 4; k += sepPattern[i], i++);
	if ((i&1)==0) {
		sepPattern[0] = 4;
		sepPattern[1] = k-4;
		j = 2;
	}
	else {
		sepPattern[0] = k;
		j = 1;
	}
	for ( ; i < sNdx+2; i++, j++) {
		sepPattern[j] = sepPattern[i];
	}
	for (j--, k = 0; k <= 4; k += sepPattern[j], j--);
	if ((j&1)==0) {
		j += 2;
		sepPattern[j-1] = k-4;
		sepPattern[j] = 4;
	}
	else {
		j++;
		sepPattern[j] = k;
	}
	return j+1;
}

void BarRSS14S(HUGEDATA char *data, char chksum)
{
	INT8U linPattern[ELMNTS];
  	INT8U tmpbars[ELMNTS];//gm 091230

	int i;
	//int j;
	//int rows;
	int ccFlag;
	//char *ccStr;
	//HUGEDATA char *cp;
	INT8U slen;
	INT8U ck;
	
	ccFlag = FALSE;//gm
	chksum = 0;
	if (RSS14enc((INT8U *)data, linPattern, ccFlag)) {
		if (errFlag) {
			printf("\nRSS14 encoding error occurred.");
			return;
		}
	}
//sprintf(MsgBuf, "%s\r\n", data);//???
//MsgOut(MsgBuf);
	// RSS14S upper row
	memcpy(&upperPattern[0], &linPattern[0], 21); //ELMNTS/2 - 2 
//[start]gm including left/right guard
	//left guard 
	tmpbars[0] = 1;
	tmpbars[1] = 1;
	for (i = 0; i < 21; i++) //strlen(bars)
	{
		tmpbars[2+i] = upperPattern[i];
	}
	//right guard
	tmpbars[2+i] = 1;
	tmpbars[2+i+1] = 1;

	for (i=0; i< 25; i++) // ELMNTS/2 + 2
	{
		upperPattern[i] = tmpbars[i];
	}
//[end]gm
//for (i = 0; i < 25; i++) 
//{
//	sprintf(MsgBuf, "%d ", upperPattern[i]);//???
//	MsgOut(MsgBuf);
//}
//sprintf(MsgBuf, "\r\n");//???
//MsgOut(MsgBuf);
	// RSS14S separator pattern
	seperatorLen = 0;
	seperatorLen = separator14S(linPattern, sepPattern);
//for (i = 0; i < seperatorLen; i++) 
//{
//	sprintf(MsgBuf, "%d ", sepPattern[i]);//???
//	MsgOut(MsgBuf);
//}
//sprintf(MsgBuf, "(len=%d)\r\n", seperatorLen);//???
//MsgOut(MsgBuf);

	// RSS14S lower row
	memcpy(&lowerPattern[0], &linPattern[ELMNTS/2-2], ELMNTS/2-2);
//[start]gm including left/right guard
	//left guard 
	tmpbars[0] = 1;
	tmpbars[1] = 1;
	for (i = 0; i < ELMNTS/2-2; i++) //strlen(bars)
	{
		tmpbars[2+i] = lowerPattern[i];
	}
	//right guard
	tmpbars[2+i] = 1;
	tmpbars[2+i+1] = 1;

	for (i=0; i< ELMNTS/2+2; i++)
	{
		lowerPattern[i] = tmpbars[i];
	}
//[end]gm
//for (i = 0; i < ELMNTS/2+2; i++) 
//{
////	lowerPattern[i] = linPattern[ELMNTS/2 + i];
//	sprintf(MsgBuf, "%d ", lowerPattern[i]);//???
//	MsgOut(MsgBuf);
//}
//sprintf(MsgBuf, "\r\n");//???
//MsgOut(MsgBuf);

	//make barcode string with checksum
	slen = strlen(data);
	ck = BarUpc_Checksum(data, slen);
	*(data + slen) = ck;
	*(data + slen + 1) = NULL;
}

void BarRSS14SO(HUGEDATA char *data, char chksum)
{
	INT8U linPattern[ELMNTS];
	//INT8U sepPattern[SYM_W/2+2];
	INT8U chexPattern[SYM_W/2+2];
  	INT8U tmpbars[ELMNTS];//gm 091230

	//char primaryStr[14+1];
	//char tempStr[28+1];

	int i;
	//int j;
	//int rows;
	int ccFlag;
	//char *ccStr;

	INT8U startPrtType; 
	INT8U leng;
	INT8U slen;
	INT8U ck;

	for (i = 0; i < SYM_W/2+2; i++) chexPattern[i] = 1; // chex = all 1X elements
	chexPattern[0] = 5; // wide space on left
	chexPattern[SYM_W/2+1-7] = 4; // wide space on right

	ccFlag = FALSE;//gm
	chksum = 0; //gm
//	if (strlen(params->dataStr) > 13) {
//		errFlag = TRUE;
//		printf("\nprimary data exceeds 13 digits");
//		return;
//	}

//	strcpy(tempStr, "000000000000");
//	strcat(tempStr, bar_buf); //gm params->dataStr	
//	strcpy(primaryStr, tempStr + strlen(tempStr) - 13);

	if (RSS14enc((INT8U *)data, linPattern, ccFlag)) {
//		if (errFlag) {
//			printf("\nRSS14 encoding error occurred.");
//			return;
//		}
	}
//sprintf(MsgBuf, "%s\r\n", data);//???
//MsgOut(MsgBuf);

	// RSS14SO upper row
	//prints.pattern = linPattern;
	//prints.height = params->pixMult*SYM_H;
//[start]gm including left/right guard
	memcpy(&upperPattern[0], &linPattern[0], ELMNTS/2 - 2); //ELMNTS/2 - 2 	
	//left guard 
	tmpbars[0] = 1;
	tmpbars[1] = 1;
	for (i = 0; i < ELMNTS/2 - 2; i++) //strlen(bars)
	{
		tmpbars[2+i] = upperPattern[i];
	}
	//right guard
	tmpbars[2+i] = 1;
	tmpbars[2+i+1] = 1;

	for (i=0; i< ELMNTS/2 + 2 ; i++) // ELMNTS/2 + 2
	{
		upperPattern[i] = tmpbars[i];
	}
//[end]gm
//for (i = 0; i < ELMNTS/2 + 2 ; i++) 
//{
//	sprintf(MsgBuf, "%d ", upperPattern[i]);//???
//	MsgOut(MsgBuf);
//}
//sprintf(MsgBuf, "\r\n");//???
//MsgOut(MsgBuf);

	// RSS14SO upper row separator pattern
	startPrtType = TRUE; //gm add
	leng = ELMNTS/2 - 2;
	upperseperatorLen = 0;
	upperseperatorLen = cnvSeparator(linPattern, uppersepPattern, startPrtType, leng);
//for (i = 0; i < upperseperatorLen; i++) 
//{
//	sprintf(MsgBuf, "%d ", uppersepPattern[i]);
//	MsgOut(MsgBuf);
//}
//sprintf(MsgBuf, "uppersep_len=%d\r\n", upperseperatorLen);//???
//MsgOut(MsgBuf);

	// chex pattern
	//for (i = 0; i < SYM_W/2+2-7; i++) sepPattern[i] = chexPattern[i]; // chex = all 1X elements
	memcpy(&sepPattern[0], &chexPattern[0], SYM_W/2+2-7);
	seperatorLen = SYM_W/2+2-7;
//for (i = 0; i < SYM_W/2+2-7; i++) 
//{
//	sprintf(MsgBuf, "%d ", sepPattern[i]);
//	MsgOut(MsgBuf);
//}
//sprintf(MsgBuf, "\r\n");//???
//MsgOut(MsgBuf);

	// RSS14SO lower row separator pattern
	startPrtType = FALSE; //gm add
	lowerseperatorLen = 0;
	lowerseperatorLen = cnvSeparator(&linPattern[21], lowersepPattern, startPrtType, leng);
//for (i = 0; i < lowerseperatorLen; i++) 
//{
//	sprintf(MsgBuf, "%d ", lowersepPattern[i]);
//	MsgOut(MsgBuf);
//}
//sprintf(MsgBuf, "lowersep_len=%d\r\n", lowerseperatorLen);//???
//MsgOut(MsgBuf);

	// RSS14SO lower row
	memcpy(&lowerPattern[0], &linPattern[ELMNTS/2-2], ELMNTS/2-2);//ELMNTS/2 
//[start]gm including left/right guard
	//left guard 
	tmpbars[0] = 1;
	tmpbars[1] = 1;
	for (i = 0; i < ELMNTS/2-2; i++) //strlen(bars)
	{
		tmpbars[2+i] = lowerPattern[i];
	}
	//right guard
	tmpbars[2+i] = 1;
	tmpbars[2+i+1] = 1;

	for (i=0; i< ELMNTS/2+2; i++) // ELMNTS/2 + 2
	{
		lowerPattern[i] = tmpbars[i];
	}
//[end]gm
//for (i = 0; i < ELMNTS/2+2; i++) 
//{
//	sprintf(MsgBuf, "%d ", lowerPattern[i]);//???
//	MsgOut(MsgBuf);
//}
//sprintf(MsgBuf, "\r\n");//???
//MsgOut(MsgBuf);

	//make barcode string with checksum
	slen = strlen(data);
	ck = BarUpc_Checksum(data, slen);
	*(data + slen) = ck;
	*(data + slen + 1) = NULL;
}			    
#endif//#ifdef USE_RSS14
