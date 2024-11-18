/*****************************************************************************
|*			
|*  Copyright		:	(c) 2002 CAS
|*  Filename		:	bar_postnet.c
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
#include "bar_common.h"

#ifdef USE_POSTNET
ROMDATA char BarPostnet_Pattern[]={
	0xc0,0x18,0x28,0x30,0x48,
	0x50,0x60,0x88,0x90,0xa0,
	0x80,
};

char  BarPostnet_Checksum(HUGEDATA char *data,short leng)
{
	short i,sum;
	char ck;
	sum=0;
	for (i=0; i<leng; i++) {
		ck=data[i];
		if ((ck>='0') && (ck<='9')) sum += ck-'0';
	}
	sum = sum %10;
	ck=(10-sum) + '0';
	return ck;
}

char BarPostnet_Verify(HUGEDATA char *text)
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

short BarPostnet(HUGEDATA char *data,char chksum,HUGEDATA INT8U *pattern)
/****************************************************************************
 INT25 : N digit , chksum 을 1로 하면 n+1로 전환됨.
 ****************************************************************************/
{
	short slen,q,sz,i;
	char  ch,ck;
	
	if(BarPostnet_Verify(data)) return -1;
	
	slen=strlen(data);
	if (chksum) {
		ck=BarPostnet_Checksum(data,slen);
		*(data + slen)=ck;
		*(data + slen + 1)=NULL;
	}
	q=0;
	BarStruct.p0=BarStruct.p1=0;
	sz=1; BarMembcpy(pattern,q,&BarPostnet_Pattern[10],sz); q+=sz;
	sz=5;
	for (i=0; i<slen; i++) {
		ch=data[i];
//		if ((ch>='0') && (ch<='9')) {
			BarMembcpy(pattern,q,&BarPostnet_Pattern[ch-'0'],sz); q+=sz;
//		}
	}
	if (chksum) {
		BarMembcpy(pattern,q,&BarPostnet_Pattern[ck-'0'],sz); q+=sz;
	}
	sz=1; BarMembcpy(pattern,q,&BarPostnet_Pattern[10],sz); q+=sz;
	BarStruct.p2=0;
	return q;
}

#endif//#ifdef USE_POSTNET