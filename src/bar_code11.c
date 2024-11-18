/*****************************************************************************
|*			
|*  Copyright		:	(c) 2002 CAS
|*  Filename		:	barc_ode11.c
|*  Version			:	0.1
|*  Programmer(s)	:	Hong Yi-Phyo (HYP)
|*  Created			:	2003/01/23
|*  Description		:	barcode code11 routine    
|*				
****************************************************************************/

#include <stdio.h>
#include <string.h>
#include "globals.h"
#include "commbuf.h"
#include "bar_common.h"

#ifdef USE_CODE11

ROMDATA char BarCode11_Pattern[]={
	0x08,0x88,0x48,0xc0,0x28,0xa0,0x60,0x18,0x90,0x80,
	0x20,	/* Dash */
	0x30,	/* Start and Stop */
	0x00,	/* Inter Space    */
};

char BarCode11_Checksum_CK(HUGEDATA char *code,short leng,INT8U *k)
{
	short i,sum_c,sum_k,c1,k1;
	char ck;

	sum_c=0;
	sum_k=0;
	k1=2;
	c1=1;
	for (i=leng-1; i>=0; i--) {
		ck=code[i];
		if (code[i]=='-') ck=10; else ck=ck-'0';
		sum_c+=ck*c1;
		sum_k+=ck*k1;
		if(c1 == 10) c1 = 1;
		else c1++;
		if(k1 == 9) k1 = 1;
		else k1++;
	}
	sum_c=sum_c%11;
	sum_k=sum_k + sum_c;
	sum_k=sum_k%11;

	if (sum_k==10) *k   ='-'; else *k   =sum_k+'0';
	if (sum_c==10) sum_c='-'; else sum_c=sum_c+'0';
	return sum_c;
}

char BarCode11_Verify(HUGEDATA char *text)
{
    INT16S i;

    if (!strlen(text))
	return -1;

    for (i=0; text[i]; i++) {
       	if ((text[i] < '0' || text[i] > '9') && (text[i] != '-')) //0x30 ~ 0x39
			return -1;
	}
    return 0;
}

short BarCode11(HUGEDATA char *data,char chksum,HUGEDATA INT8U *pattern)
{
	INT8U ck,ck_k;
	short slen,q,i,id,sz,z;

	if(BarCode11_Verify(data)) return -1;

	slen=strlen(data);
	if (chksum) {
		ck=BarCode11_Checksum_CK(data,slen,&ck_k);
	}
	q=0;
	BarStruct.p0=BarStruct.p1=0;
	/* Start Character */
	sz=6;
	z=BarMembcpyNS(pattern,q,&BarCode11_Pattern[11],BarStruct.nb,BarStruct.wb,sz); q=z;
	for (i=0; i<slen; i++) {
		id=data[i]-'0';
		if (data[i]=='-') id=10;
		z=BarMembcpyNS(pattern,q,&BarCode11_Pattern[id],BarStruct.nb,BarStruct.wb,sz); q=z;
	}
	if (chksum>=1) {
//		sz=5;
//		if (chksum>=2) sz=6;
		sz =6;
		id=ck-'0'; if (ck=='-') id=10;
		z=BarMembcpyNS(pattern,q,&BarCode11_Pattern[id],BarStruct.nb,BarStruct.wb,sz); q=z;
		*(data + slen)=ck;
		*(data + slen + 1)=NULL;
	}
	if (chksum>=2) {
//		sz=5;
		sz=6;
		id=ck_k-'0'; if (ck_k=='-') id=10;
		z=BarMembcpyNS(pattern,q,&BarCode11_Pattern[id],BarStruct.nb,BarStruct.wb,sz); q=z;
		*(data + slen + 1)=ck_k;
		*(data + slen + 2)=NULL;
	}
	sz=5;
	z=BarMembcpyNS(pattern,q,&BarCode11_Pattern[11],BarStruct.nb,BarStruct.wb,sz); q=z;
	BarStruct.p2=q;
	return q;
}

#endif //#ifdef USE_CODE11
