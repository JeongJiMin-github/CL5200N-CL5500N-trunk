/*****************************************************************************
|*			
|*  Copyright		:	(c) 2002 CAS
|*  Filename		:	bar_code39.c
|*  Version		:	0.1
|*  Programmer(s)	:	Hong Yi-Phyo (HYP)
|*  Created		:	2003/01/23
|*  Description		:	    
|*				
****************************************************************************/

#include <stdio.h>
#include <string.h>
#include "globals.h"
#include "commbuf.h"
#include "prt_common.h"
#include "bar_common.h"

#ifdef USE_CODE39
ROMDATA char BarCode39_Pattern[]={
	0x62,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /*  0 ..  3 */
	0x54,0x00,0x15,0x00,0x00,0x00,0x00,0x00, /*  4 ..  7 */
	0x00,0x00,0x00,0x00,0x4a,0x00,0x45,0x00, /*  8 .. 11 */
	0x00,0x00,0x42,0x80,0xc2,0x00,0x51,0x00, /* 12 .. 15 */
	0x1a,0x00,0x90,0x80,0x30,0x80,0xb0,0x00, /* 16 .. 19 */
	0x18,0x80,0x98,0x00,0x38,0x00,0x12,0x80, /* 20 .. 23 */
	0x92,0x00,0x32,0x00,0x00,0x00,0x00,0x00, /* 24 .. 27 */
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /* 28 .. 31 */
	0x00,0x00,0x84,0x80,0x24,0x80,0xa4,0x00, /* 32 .. 35 */
	0x0c,0x80,0x8c,0x00,0x2c,0x00,0x06,0x80, /* 36 .. 39 */
	0x86,0x00,0x26,0x00,0x0e,0x00,0x81,0x80, /* 40 .. 43 */
	0x21,0x80,0xa1,0x00,0x09,0x80,0x89,0x00, /* 44 .. 47 */
	0x29,0x00,0x03,0x80,0x83,0x00,0x23,0x00, /* 48 .. 51 */
	0x0b,0x00,0xc0,0x80,0x60,0x80,0xe0,0x00, /* 52 .. 55 */
	0x48,0x80,0xc8,0x00,0x68,0x00,           /* 56 .. 58 */
	0x00,0x00,				 /*       59 */
};

ROMDATA char BarCode39_Set[]={
	/* SP, !, ", #, $, %, &, ', (, ), *, +, ,, -, ., /, */
	   38, 0, 0, 0,39,42, 0, 0, 0, 0,43,41, 0,36,37,40,
	/*  0, 1, 2, 3, 4, 5, 6, 7, 8, 9, :, ;, <, =, >, ?, @, */
	    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 0, 0, 0, 0, 0, 0,
	/*  A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z, */
	   10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,
};

ROMDATA char BarCode39_Unset[]={
	 48, 49, 50, 51, 52, 53, 54, 55, 56, 57,
	 65, 66, 67, 68, 69, 70, 71, 72, 73, 74,
	 75, 76, 77, 78, 79, 80, 81, 82, 83, 84,
	 85, 86, 87, 88, 89, 90, 45, 46, 32, 36,
	 47, 43, 37, 42,  0,
};

/*      00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001111111111111111111111111111 */
/*      00000000001111111111222222222233333333334444444444555555555566666666667777777777888888888899999999990000000000111111111122222222 */
/*      01234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567 */
ROMDATA static char BarCode39_Shift_Set[] =
       "%$$$$$$$$$$$$$$$$$$$$$$$$$$%%%%% ////////////  /          /%%%%%%                          %%%%%%++++++++++++++++++++++++++%%%%%";
ROMDATA static char BarCode39_Shift_Set2[] =
       "UABCDEFGHIJKLMNOPQRSTUVWXYZABCDE ABCDEFGHIJKL  O          ZFGHIJV                          KLMNOWABCDEFGHIJLKMNOPQRSTUVWXYZPQRST";

char BarCode39_Checksum(HUGEDATA char *code,short leng)
{
	short i,j,l,sum,l2;
	unsigned char  c;

        sum=0;
	l2=0;
        for (i=0; i<leng; i++) {
		c=code[i];
//		if ((c<0) && (c>0x5a)) return -1;
		j=BarCode39_Set[c-' '];	//0x20
		sum +=j;
        }
        l = sum%43;
	return (char)BarCode39_Unset[l];
}

char BarCode39_Verify(HUGEDATA char *text)
{
    INT16S i;

    if (!strlen(text))
	return -1;

    for (i=0; text[i]; i++) {
       	if (text[i] < ' ' || text[i] > 'Z')	//0x20 ~ 0x5A
			return -1;
       	if (!BarCode39_Set[text[i]-0x20] && (text[i] != '0'))
           	return -1;
	}
    return 0;
}

short BarCode39(HUGEDATA char *data,char chksum,HUGEDATA INT8U *pattern)
{
	short z,q,i,sz,r,slen;
	unsigned ck,c;

	if(BarCode39_Verify(data)) return -1;

	slen=strlen(data);
	if (chksum) {
	    ck=BarCode39_Checksum(data,slen);
		*(data + slen)=ck;
		*(data + slen + 1)=NUL;
	}
        q=0;
	BarStruct.p0=BarStruct.p1=0;
        sz=10;
	c='*';							 	
	r=c-0x20;
	r<<=1;
	z=BarMembcpyNS(pattern,q,&BarCode39_Pattern[r],BarStruct.nb,BarStruct.wb,sz); q=z;
        for (i=0; i<slen; i++) {
		c=data[i];
//		if ((c<0) && (c>0x5a)) return -1;
		r=c-0x20;
		r<<=1;
		z=BarMembcpyNS(pattern,q,&BarCode39_Pattern[r],BarStruct.nb,BarStruct.wb,sz); q=z;
        }
	if (chksum) {
		c=ck;
//		if ((c<0) && (c>0x5a)) return -1;
		r=c-0x20;
		r<<=1;
		z=BarMembcpyNS(pattern,q,&BarCode39_Pattern[r],BarStruct.nb,BarStruct.wb,sz); q=z;
	}
	c='*';
	r=c-0x20;
	r<<=1;
    sz=9;
	z=BarMembcpyNS(pattern,q,&BarCode39_Pattern[r],BarStruct.nb,BarStruct.wb,sz); q=z;
	BarStruct.p2=q;
        return q;
}

#ifdef USE_CODE39FULL
short BarCode39e(HUGEDATA char *data,char chksum,HUGEDATA INT8U *pattern)
{
	short q,i,j,slen;
	char c,tdata[MAX_BAR_DATA_BUF_SIZE],ck;

	j = 0;
	slen=strlen(data);
	for (i = 0; i <slen; i++) {
		c = data[i];
		if (c>=' ' && c<='Z') {	//0x20 ~ 0x5a
			if(BarCode39_Set[c-' '] != 0 || c == '0') { //0x20,0x30
				tdata[j] = c;
				j++;
			} else {
				tdata[j] = BarCode39_Shift_Set[c];
				j++;
				tdata[j] = BarCode39_Shift_Set2[c];
				j++;
			}
		} else if (c>='[' && c<= '~') {  //0x5b ~ 0x7e
			if (c == BAR_2CH_SET_CH) {
				i++;
				if (i >= slen ) return -1;
				c = data[i];
				if (c>='@' && c<='_') {	//0x40 ~ 0x5f
					c = c - '@'; //0x40
				} else if (c == '0') {
					    c = BAR_2CH_SET_CH;
				} else if (c == '5') {
						c = DEL;  //0x7f
				} else {
						return -1;
				}
			}
			tdata[j] = BarCode39_Shift_Set[c];
			j++;
			tdata[j] = BarCode39_Shift_Set2[c];
			j++;
		} else {
			return -1;
		}			
	}
	tdata[j] = NULL;
	if (chksum) {
	    ck=BarCode39_Checksum(tdata,j);
		*(data + slen)=ck;
		*(data + slen + 1)=NULL;
	}
	q = BarCode39(tdata,chksum,pattern);
    return q;
}
#endif//#ifdef USE_CODE39FULL
#endif//#ifdef USE_CODE39
