/*****************************************************************************
|*			
|*  Copyright		:	(c) 2002 CAS
|*  Filename		:	bar_code25.c
|*  Version			:	0.1
|*  Programmer(s)	:	Hong Yi-Phyo (HYP)
|*  Created			:	2003/01/23
|*  Description		:	    
|*				
****************************************************************************/

#include <stdio.h>
#include <string.h>
#include "globals.h"
#include "commbuf.h"
#include "bar_common.h"

#ifdef USE_I2OF5
ROMDATA char BarI25_Pattern[]={
        0x90,0x00,0x00,0x00, /* INT25 0:start 101100  */
        0xe4,0x00,0x00,0x00, /*       1:stop  111001  */
        0x80,0x00,0x00,0x00, /*       2:bit1      */
        0xe0,0x00,0x00,0x00, /*       3:bit2      */
        0x00,0x00,0x00,0x00, /*       4:zero      */
		  0xff,0xff,0xff,0xff, /*       5:black     */
};

ROMDATA char BarI25_SetPattern[]={
	0x30, /* Pattern 0:00110 [20] == nnwwn */
	0x88, /*         1:10001   */
	0x48, /*         2:01001   */
	0xc0, /*         3:11000   */
	0x28, /*         4:00101   */
	0xa0, /*         5:10100   */
	0x60, /*         6:01100   */
	0x18, /*         7:00011   */
	0x90, /*         8:10010   */
	0x50, /*         9:01010   */
};

ROMDATA char *BarI25_StartPattern[]={ "00","10", };


ROMDATA char *Bar25_Pattern[]={
	"NNWWN","WNNNW","NWNNW","WWNNN","NNWNW",
	"WNWNN","NWWNN","NNNWW","WNNWN","NWNWN",
	"WWN","WNW",			/* start,stop */ //industrial
	"NN","WN",				/* start,stop */ //iata
	"WNNNN","WNNNN",		/* start,stop */ //matrix
};

ROMDATA char Bar25_SubPattern[]={
	0xff,0xff,0x00,0x00,	/* Wide,Narrow bar, space */
	/* limit is 16 bit == 2mm == max bar width */
};

//type = 1 :USS Interleaved 2 of 5 MOD 10 
//type = 2 :German Postal 2 of 5 Leitcode, German Postal 2 of 5 Identcode
//type = 3 :same as s25 
char BarI25_Checksum(HUGEDATA char *code,short leng,char type)
{
	short i,e,o;
	char ck,odd_mul,even_mul;


	e=o=0;
	if(type == 1 || type == 3) {
		odd_mul = 3;
		even_mul = 1;
	} else if (type == 2) {
		odd_mul = 4;
		even_mul = 9;
	} else {
		odd_mul = 0;
		even_mul = 0;
	}
	for (i=leng-1; i>=0; ) {
		if (i>=0) {
			ck=code[i--]-'0';
			e+=ck*odd_mul;
		}
		if (i>=0) {
			ck=code[i--]-'0';
			o+=ck*even_mul;
		}
	}
	e=e+o;
	e=e%10;
///	
	if(type != 3) {
		e = 10-e;
		if(e == 10) e=0;
	}
///
	return e;
}

char Barcode25_Checksum(HUGEDATA char *code,short leng)
{
	short i,e,o;
	char ck;
	e=o=0;
	for (i=leng-1; i>=0; ) {
		if (i>=0) {
			ck=code[i--]-'0';
			e+=ck*3;
		}
		if (i>=0) {
			ck=code[i--]-'0';
			o+=ck;
		}
	}
	e=e+o;
	e=e%10;

	return e+'0';
}


char Barcode25_Verify(HUGEDATA char *text)
{
    INT16S i;

    if (!strlen(text))
	return -1;

    for (i=0; text[i]; i++) {
       	if ((text[i] < '0' || text[i] > '9')) //0x30 ~ 0x39
			return -1;
	}
    return 0;
}

short BarI25(HUGEDATA char *data,char chksum,HUGEDATA INT8U *pattern,HUGEDATA char *tdata)
/****************************************************************************
 INT25 : N digit , chksum 을 1로 하면 n+1로 전환됨.
 ****************************************************************************/
{
	char ck;
	short slen,q,sz,i,j,even,id;
	short dlen;
	char  oddCode,evnCode;
	int  oddChar,evnChar;
	unsigned char mask;
	ROMDATA char *cp;
	HUGEDATA char *td;

	if(Barcode25_Verify(data)) return -1;
	
	td = tdata;
	slen=strlen(data);
	dlen=slen;
	if (chksum) {
		ck=BarI25_Checksum(data,slen,chksum);
		dlen++;
	}
	even=dlen%2;
	q=0;
	BarStruct.p0=BarStruct.p1=0;
	/* Start Code */
	cp=BarI25_StartPattern[0];
	for (i=0; i<2; i++) {
		oddCode=cp[i]-'0';
		if (oddCode) sz=BarStruct.wb;
		else sz=BarStruct.nb;
		BarMembcpy(pattern,q,&BarI25_Pattern[20],sz); q+=sz;
		sz=BarStruct.ns;
		BarMembcpy(pattern,q,&BarI25_Pattern[16],sz); q+=sz;
	}

	for (i=0; i<dlen;) {
		if ((i==0) && (even)) {
			oddChar = 0;
			*td = '0';
			td++;
			*td = data[i];
			td++;
			evnChar = data[i++] - '0';
		} else {
			*td = data[i];
			td++;
			oddChar = data[i++] - '0';
			if ((i>=slen) && (chksum)) {
				*td = ck + '0';
				td++;
				evnChar=ck;
				i++;
			} else {
				*td = data[i];
				td++;
				evnChar = data[i++] - '0';
			}
		}
		*td = '\0';
		mask = 0x80;
		for (j=0; j<5; j++) {
			oddCode = BarI25_SetPattern[oddChar]&mask;
			evnCode = BarI25_SetPattern[evnChar]&mask;
			id=20;
			if (oddCode) {
				sz=BarStruct.wb; 
			} else {
				sz=BarStruct.nb;
			}
			BarMembcpy(pattern,q,&BarI25_Pattern[id],sz); q+=sz;
			id=16;
			if (evnCode) {
				sz=BarStruct.ws;
			} else {
				sz=BarStruct.ns;
			}
			BarMembcpy(pattern,q,&BarI25_Pattern[id],sz); q+=sz;
			mask>>=1;
		}
	}
	/* Stop Code */
	cp=BarI25_StartPattern[1];
	for (i=0; i<2; i++) {
		oddCode=cp[i]-'0';
		if (oddCode) sz=BarStruct.wb;
		else sz=BarStruct.nb;
		BarMembcpy(pattern,q,&BarI25_Pattern[20],sz); q+=sz;
		sz=BarStruct.ns;
		BarMembcpy(pattern,q,&BarI25_Pattern[16],sz); q+=sz;
	}
	BarStruct.p2=q;
	return q;
}

#ifdef USE_S25_M25_IATA25
short BarS25(HUGEDATA char *data,char chksum,HUGEDATA INT8U *pattern)
{
	unsigned char ck;
	short slen,q,i,j,id,sz;	 //,id_sub
	char ch;
	ROMDATA char *cp;

	if(Barcode25_Verify(data)) return -1;
	
	slen=strlen(data);
	if (chksum) {
		ck=Barcode25_Checksum(data,slen);
		*(data + slen)=ck;
		*(data + slen + 1)=NULL;
	}
	q=0;
	BarStruct.p0=BarStruct.p1=0;
	/* Start Character */
	cp=Bar25_Pattern[10];
	for (j=0; j<3; j++) {
		ch=cp[j];
		if (ch=='N') sz=BarStruct.nb; else sz=BarStruct.wb;
		BarMembcpy(pattern,q,&Bar25_SubPattern[0],sz); q+=sz;
		BarMembcpy(pattern,q,&Bar25_SubPattern[2],BarStruct.ns); q+=BarStruct.ns;
	}

	for (i=0; i<slen; i++) {
		id=data[i]-'0';
		cp=Bar25_Pattern[id];
		for (j=0; j<5; j++) {
			ch=cp[j];
			if (ch=='N') sz=BarStruct.nb; else sz=BarStruct.wb;
			BarMembcpy(pattern,q,&Bar25_SubPattern[0],sz); q+=sz;
			BarMembcpy(pattern,q,&Bar25_SubPattern[2],BarStruct.ns);
			q+=BarStruct.ns;
		}
	}
	if (chksum) {
		id=ck-'0';
		cp=Bar25_Pattern[id];
		for (j=0; j<5; j++) {
			ch=cp[j];
			if (ch=='N') sz=BarStruct.nb; else sz=BarStruct.wb;
			BarMembcpy(pattern,q,&Bar25_SubPattern[0],sz); q+=sz;
			BarMembcpy(pattern,q,&Bar25_SubPattern[2],BarStruct.ns);
			q+=BarStruct.ns;
		}
	}
	/* STOP */
	cp=Bar25_Pattern[11];
	for (j=0; j<3; j++) {
		ch=cp[j];
		if (ch=='N') sz=BarStruct.nb; else sz=BarStruct.wb;
		BarMembcpy(pattern,q,&Bar25_SubPattern[0],sz); q+=sz;
		BarMembcpy(pattern,q,&Bar25_SubPattern[2],BarStruct.ns); q+=BarStruct.ns;
	}
	BarStruct.p2=q;
        return q;
}

short BarM25(HUGEDATA char *data,char chksum,HUGEDATA INT8U *pattern)
{
	unsigned char ck;
	short slen,q,i,j,id,sz;	 //,id_sub
	char ch;
	ROMDATA char *cp;

	if(Barcode25_Verify(data)) return -1;
	
	slen=strlen(data);
	if (chksum) {
		ck=Barcode25_Checksum(data,slen);
		*(data + slen)=ck;
		*(data + slen + 1)=NULL;
	}
	q=0;
	BarStruct.p0=BarStruct.p1=0;
	/* Start Character */
	cp=Bar25_Pattern[14];
	for (j=0; j<5; j++) { 
		ch=cp[j];
		if (ch=='N') sz=BarStruct.nb; else sz=BarStruct.wb;
		if (j&0x01) {
			BarMembcpy(pattern,q,&Bar25_SubPattern[2],sz); q+=sz;
		} else {
			BarMembcpy(pattern,q,&Bar25_SubPattern[0],sz); q+=sz;
		}
	}
	BarMembcpy(pattern,q,&Bar25_SubPattern[2],BarStruct.ns); q+=BarStruct.ns;

	for (i=0; i<slen; i++) {
		id=data[i]-'0';
		cp=Bar25_Pattern[id];
		for (j=0; j<5; j++) {
			ch=cp[j];
			if (ch=='N') sz=BarStruct.nb; else sz=BarStruct.wb;
			if (j&0x01) {
				BarMembcpy(pattern,q,&Bar25_SubPattern[2],sz); q+=sz;
			} else {
				BarMembcpy(pattern,q,&Bar25_SubPattern[0],sz); q+=sz;
			}
		}
		BarMembcpy(pattern,q,&Bar25_SubPattern[2],BarStruct.ns); q+=BarStruct.ns;
	}
	if (chksum) {
		id=ck-'0';
		cp=Bar25_Pattern[id];
		for (j=0; j<5; j++) {
			ch=cp[j];
			if (ch=='N') sz=BarStruct.nb; else sz=BarStruct.wb;
			if (j&0x01) {
				BarMembcpy(pattern,q,&Bar25_SubPattern[2],sz); q+=sz;
			} else {
				BarMembcpy(pattern,q,&Bar25_SubPattern[0],sz); q+=sz;
			}
		}
		BarMembcpy(pattern,q,&Bar25_SubPattern[2],BarStruct.ns); q+=BarStruct.ns;
	}
	/* STOP */
	cp=Bar25_Pattern[15];
	for (j=0; j<5; j++) { 
		ch=cp[j];
		if (ch=='N') sz=BarStruct.nb; else sz=BarStruct.wb;
		if (j&0x01) {
			BarMembcpy(pattern,q,&Bar25_SubPattern[2],sz); q+=sz;
		} else {
			BarMembcpy(pattern,q,&Bar25_SubPattern[0],sz); q+=sz;
		}
	}
	BarMembcpy(pattern,q,&Bar25_SubPattern[2],BarStruct.ns); q+=BarStruct.ns;
	BarStruct.p2=q;
        return q;
}

short BarIATA25(HUGEDATA char *data,char chksum,HUGEDATA INT8U *pattern)
{
	unsigned char ck;
	short slen,q,i,j,id,sz;	 //,id_sub
	char ch;
	ROMDATA char *cp;

	if(Barcode25_Verify(data)) return -1;
	
	slen=strlen(data);
	if (chksum) {
		ck=Barcode25_Checksum(data,slen);
		*(data + slen)=ck;
		*(data + slen + 1)=NULL;
	}
	q=0;
	BarStruct.p0=BarStruct.p1=0;
	/* Start Character */
	cp=Bar25_Pattern[12];
	for (j=0; j<2; j++) { 
		ch=cp[j];
		if (ch=='N') sz=BarStruct.nb; else sz=BarStruct.wb;
		BarMembcpy(pattern,q,&Bar25_SubPattern[0],sz); q+=sz;
		BarMembcpy(pattern,q,&Bar25_SubPattern[2],BarStruct.ns); q+=BarStruct.ns;
	}

	for (i=0; i<slen; i++) {
		id=data[i]-'0';
		cp=Bar25_Pattern[id];
		for (j=0; j<5; j++) {
			ch=cp[j];
			if (ch=='N') sz=BarStruct.nb; else sz=BarStruct.wb;
			BarMembcpy(pattern,q,&Bar25_SubPattern[0],sz); q+=sz;
			BarMembcpy(pattern,q,&Bar25_SubPattern[2],BarStruct.ns);
			q+=BarStruct.ns;
		}
	}
	if (chksum) {
		id=ck-'0';
		cp=Bar25_Pattern[id];
		for (j=0; j<5; j++) {
			ch=cp[j];
			if (ch=='N') sz=BarStruct.nb; else sz=BarStruct.wb;
			BarMembcpy(pattern,q,&Bar25_SubPattern[0],sz); q+=sz;
			BarMembcpy(pattern,q,&Bar25_SubPattern[2],BarStruct.ns);
			q+=BarStruct.ns;
		}
	}
	/* STOP */
	cp=Bar25_Pattern[13];
	for (j=0; j<2; j++) { 
		ch=cp[j];
		if (ch=='N') sz=BarStruct.nb; else sz=BarStruct.wb;
		BarMembcpy(pattern,q,&Bar25_SubPattern[0],sz); q+=sz;
		BarMembcpy(pattern,q,&Bar25_SubPattern[2],BarStruct.ns); q+=BarStruct.ns;
	}
	BarStruct.p2=q;
        return q;
}

#endif//#ifdef USE_S25_M25_IATA25
#endif//#ifdef USE_I2OF5




