/*****************************************************************************
|*			
|*  Copyright		:	(c) 2002 CAS
|*  Filename		:	bar_codabar.c
|*  Version			:	0.1
|*  Programmer(s)	:	Hong Yi-Phyo (HYP)
|*  Created			:	2003/01/23
|*  Description		:	barcode codabar routine    
|*				
****************************************************************************/

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "globals.h"
#include "commbuf.h"
#include "bar_common.h"

#ifdef USE_CODABAR
/*****************************************************************************
|*								VARIABLES
****************************************************************************/
ROMDATA char BarCodabar_Pattern[]={
	0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /*  0 ..  4 [36~ ] $%&'( */
	0x00,0x00,0x00,0x00,0x2a,0x00,0x00,0x00,0x18,0x00, /*  5 ..  9        )*+,- */
	0xa8,0x00,0xa2,0x00,0x06,0x00,0x0c,0x00,0x12,0x00, /* 10 .. 14        ./012 */
	0xc0,0x00,0x24,0x00,0x84,0x00,0x42,0x00,0x48,0x00, /* 15 .. 19 */
	0x60,0x00,0x90,0x00,0x8a,0x00,				   /* 20 .. 22 */
	0x34,0x00,0x52,0x00,0x16,0x00,0x1c,0x00,	   /* A,B,C,D [44. 46. 48. 50] */
	0x00,0x00,};					   /* Interspace [52] */
/****************************************
	Width Encoding size=7 + space
 ****************************************/
ROMDATA char BarCodabar_Set[]={
	/* SP, !, ", #, $, %, &, ', (, ), *, +, ,, -, ., /, */
	   0, 0, 0, 0,  0,11, 0, 0, 0, 0,18,15, 0,10,14,13,
	/*  0, 1, 2, 3, 4, 5, 6, 7, 8, 9, :, ;, <, =, >, ?, @, */
	    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 12, 0, 0, 0, 0, 0, 0,
	/*  A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z, */
	   16,17,18,19,19, 0, 0, 0, 0, 0, 0, 0, 0,17, 0, 0, 0, 0, 0,16, 0, 0, 0, 0, 0, 0
};

ROMDATA char BarCodabar_Unset[]={
	/*  0, 1, 2, 3, 4, 5, 6, 7, 8, 9 */
	 48, 49, 50, 51, 52, 53, 54, 55, 56, 57,
  /*  -,  $,  :,  /,  .,  +,  A,  B,  C,  D */
	 45, 36, 58, 47, 46, 43, 65, 66, 67, 68
};						/*	 T,  N,  *,	 E  */

/*****************************************************************************
|*  DESCRIPTION : calculate codabar checksum
|*
|*  ARGUMENTS 	: codabar data ptr, length of data    
|*
|*	RETURNS		: codabar checksum
|*
|*  NOTES		: none
|*
****************************************************************************/
char BarCodabar_Checksum(HUGEDATA char *code,short leng)
{
	short i,j,l,sum;
	unsigned char  c;

    sum=0;
    for (i=0; i<leng; i++) {
		c=code[i];
		j=BarCodabar_Set[c-' ']; //0x20
		sum +=j;
    }
    l = 16 - sum%16;
	return (char)BarCodabar_Unset[l];
}

char BarCodabar_Verify(HUGEDATA char *text)
{
    INT16S i;

    if (!strlen(text))
	return -1;

    for (i=0; text[i]; i++) {
       	if (text[i] < ' ' || text[i] > 'Z')	//0x20 ~ 0x5a
			return -1;
       	if (!BarCodabar_Set[text[i]-0x20] && (text[i] != '0'))
           	return -1;
	}
    return 0;
}

short BarCodabar(HUGEDATA char *data,char chksum,HUGEDATA INT8U *pattern)
{
	char ck, tmp;
	short slen,q,i,id,sz,z;

	if(BarCodabar_Verify(data)) return -1;

	slen=strlen(data);
	if (chksum) {
	    ck=BarCodabar_Checksum(data,slen);
		tmp = *(data + slen - 1);
		*(data + slen - 1)=ck;
		*(data + slen )=tmp;
		*(data + slen + 1)=NUL;
		slen++;
	}
	q=0;
	BarStruct.p0=BarStruct.p1=0;
	sz=8;
	for (i=0; i<slen-1; i++) {
		if (data[i]>='A') id=data[i]-'A'+23; else id=data[i]-'$';
		id<<=1;
		z=BarMembcpyNS(pattern,q,&BarCodabar_Pattern[id],BarStruct.nb,BarStruct.wb,sz); q=z;
	}
	if (data[i]>='A') id=data[i]-'A'+23;
	else id=data[i]-'$';
	id<<=1;
	sz=7;
	z=BarMembcpyNS(pattern,q,&BarCodabar_Pattern[id],BarStruct.nb,BarStruct.wb,sz); q=z;
	BarStruct.p2=q;
    return q;
}
#endif//#ifdef USE_CODABAR
