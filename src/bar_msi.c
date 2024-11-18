/*****************************************************************************
|*			
|*  Copyright		:	(c) 2002 CAS
|*  Filename		:	bar_msi.c
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

#ifdef USE_MSI_PLESSEY
ROMDATA char *BarMsi_Pattern[]={
	"13131313",	/* 0000: 0 1 은 nb, 3 은 wb 에 해당함. */
	"13131331",	/* 0001: 1 */
	"13133113",	/* 0010: 2 */
	"13133131",	/* 0011: 3 */
	"13311313",	/* 0100: 4 */
	"13311331",	/* 0101: 5 */
	"13313113",	/* 0110: 6 */
	"13313131",	/* 0111: 7 */
	"31131313",	/* 1000: 8 */
	"31131331",	/* 1001: 9 */
	"31","131",	/* [110],[1001] start, stop */
};

ROMDATA char *BarPlessey_Pattern[]={
	"13131313",	/* 0000: 0 1 은 nb, 3 은 wb 에 해당함. */
	"31131313",	/* 1000: 1 */
	"13311313",	/* 0100: 2 */
	"31311313",	/* 1100: 3 */
	"13133113",	/* 0010: 4 */
	"31133113",	/* 1010: 5 */
	"13313113",	/* 0110: 6 */
	"31313113",	/* 1110: 7 */
	"13131331",	/* 0001: 8 */
	"31131331",	/* 1001: 9 */
	"13311331", /* 0101: A */
	"31311331", /* 1101: B */
	"13133131", /* 0011: C */
	"31133131", /* 1011: D */
	"13313131", /* 0111: E */
	"31313131", /* 1111: F */
	"31311331",  /* 1101: start */
	"331311313",  /* stopbar + 1100 : stop */
};

//checksum
// 0 = none, 1 = mod10, 2 = mod11, 3 = mod 1010, 4 = mod1110, 5 = mod1011, 6 = mod1111
ROMDATA char BarMsi_CheckTable1[]={ 0, 1, 2, 1, 2, 1, 2 }; 
ROMDATA char BarMsi_CheckTable2[]={ 0, 0, 0, 1, 1, 2, 2 };

char BarMsi_Checksum_M10(HUGEDATA char *text,short l) //,char last
/*******************************
 last=0x00 이면  의미 없음
 0x30 .. 0x39 사이값이면 last 가 text에 추가된 코드로 간주하여 처리함.
 **********************************************************************/
{
	short i,sum;
	char code,o,code2,ck;

	sum=0;
	o=1;
//	if (last>0) o=0;
	for (i=l-1; i>=0; i--) {
        	code = text[i] - '0';
		if (o) {
			code2= code+code;
			sum += code2 + code2/10;
		} else {
			sum += code;
		}
		if (o) o=0; else o=1;
	}

//	if (last>0) {
//		code=(last-'0');
//		code2=code+code;
//		sum+=code2;
//	}
/****	ck = (char)(10-sum%10); ***********/
	sum = (sum + 9) / 10 * 10 - sum;
	ck = sum;
	if (ck==10) ck=0;
/**************
	printf("M10=%d\n",ck);
 **********************************************/
	return ck+'0';
}

char BarMsi_Checksum_M11(HUGEDATA char *code,short l)  //,char last
/*******************************
 last=0x00 이면  의미 없음
 0x30 .. 0x39 사이값이면 last 가 code에 추가된 코드로 간주하여 처리함.
 **********************************************************************/
{
	short i,sum; //,l
	unsigned char weight;

//    l = strlen(code);
	/* Step 1,2 */
	weight=2;
//	if (last>0) weight++;
	sum=0;
	for (i=l-1; i>=0; i--) {
		sum+=(code[i]-'0')*weight;
		weight++;
		if (weight>7) weight=2;
	}
//	if (last>0) {
//		l=(last-'0');
//		l=l+l;
//		sum+=l;
//	}
	/* Step 3 */
	sum = sum % 11;
	/* Step 4 */
	sum = 11 - sum;
	sum = sum % 10;
/**********************
	printf("M11=%d\n",sum);
 ******************************************/
	return sum+'0';
}

char BarMsi_Verify(HUGEDATA char *text)
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

short BarMsi(HUGEDATA char *data,char chksum,HUGEDATA INT8U *pattern)
{
	unsigned char ck;
	short slen,q,i,id,z;
	ROMDATA char  *cp;
	char chksum1, chksum2;
	
	if(BarMsi_Verify(data)) return -1;
	
	slen=strlen(data);
	q=0;
	BarStruct.p0=BarStruct.p1=0;
	cp=(ROMDATA char *)BarMsi_Pattern[10];
	z=BarMakePattern(cp,2,4,pattern,q); q=z;

	for (i=0; i<slen; i++) {
		id=data[i]-'0';
		cp=(ROMDATA char *)BarMsi_Pattern[id];
		z=BarMakePattern(cp,8,4,pattern,q); q=z;
	}
	if (chksum) {
		chksum1 = BarMsi_CheckTable1[chksum];
		chksum2 = BarMsi_CheckTable2[chksum];
	   if (chksum1 == 1) 
		ck=BarMsi_Checksum_M10(data,slen);//,0
	   else
		ck=BarMsi_Checksum_M11(data,slen);//,0

		*(data + slen)=ck;
		*(data + slen + 1)=NULL;

	   id=ck-'0';
	   cp=(ROMDATA char *)BarMsi_Pattern[id];
	   z=BarMakePattern(cp,8,4,pattern,q); q=z;
	   if (chksum2) {
		if (chksum2==1)
		    ck=BarMsi_Checksum_M10(data,slen+1);//,ck
		else
		    ck=BarMsi_Checksum_M11(data,slen+1); //,ck

		*(data + slen + 1)=ck;
		*(data + slen + 2)=NULL;

		id=ck-'0';
		cp=(ROMDATA char *)BarMsi_Pattern[id];
		z=BarMakePattern(cp,8,4,pattern,q); q=z;
	   }
	}
	cp=(ROMDATA char *)BarMsi_Pattern[11];
	z=BarMakePattern(cp,3,4,pattern,q); q=z;
		BarStruct.p2=q;
        return q;
}

char BarPlessey_Checksum(HUGEDATA char *text,short leng) //,char last
/*******************************
 last=0x00 이면  의미 없음
 0x30 .. 0x39 사이값이면 last 가 text에 추가된 코드로 간주하여 처리함.
 **********************************************************************/
{
	short i,j;
	char code,ck;
	static char check[9] = {1,1,1,1,0,1,0,0,1};
	char checkptr[4*MAX_BAR_DATA_BUF_SIZE];

	for (i=0; i<leng; i++) {
		if(text[i] >= 'A' && text[i] <= 'F') {
			code = text[i] - 'A' + 10;
		} else {
			code = text[i] - '0';
		}
		checkptr[4*i] = code & 1;
		checkptr[4*i+1] = (code >> 1) & 1;
		checkptr[4*i+2] = (code >> 2) & 1;
		checkptr[4*i+3] = (code >> 3) & 1;
	}
	leng = leng * 4;
	for(i=0;i<9;i++) { 
		checkptr[leng+i] = 0;
	}

	for (i=0; i < leng; i++) {
        if (checkptr[i])
                for (j = 0; j < 9; j++)
                        checkptr[i+j] ^= check[j];
    }
	ck = 0x00;
	ck |= (checkptr[leng ] & 1)<<4;
	ck |= (checkptr[leng + 1] & 1)<<5;
	ck |= (checkptr[leng + 2] & 1)<<6;
	ck |= (checkptr[leng + 3] & 1)<<7;
	ck |= (checkptr[leng + 4] & 1);
	ck |= (checkptr[leng + 5] & 1)<<1;
	ck |= (checkptr[leng + 6] & 1)<<2;
	ck |= (checkptr[leng + 7] & 1)<<3;

	return ck;
}

char BarPlessey_Verify(HUGEDATA char *text)
{
    INT16S i;

    if (!strlen(text))
	return -1;

    for (i=0; text[i]; i++) {
       	if ((text[i] < '0' || text[i] > '9')) //0x30 ~ 0x39
			if (text[i] < 'A' || text[i] > 'F') //0x30 ~ 0x39
				return -1;
	}
    return 0;
}

short BarPlessey(HUGEDATA char *data,char chksum,HUGEDATA INT8U *pattern)
{
	unsigned char ck;
	short slen,q,i,id,z;
	ROMDATA char  *cp;
	char chksum1, chksum2;
	
	if(BarPlessey_Verify(data)) return -1;
	
	slen=strlen(data);
	q=0;
	BarStruct.p0=BarStruct.p1=0;
	cp=(ROMDATA char *)BarPlessey_Pattern[16];
	z=BarMakePattern(cp,8,4,pattern,q); q=z;

	for (i=0; i<slen; i++) {
		if(data[i] >= 'A' && data[i] <= 'F') {
			id=data[i]-'A'+10;
		} else {
			id=data[i]-'0';
		}
		cp=(ROMDATA char *)BarPlessey_Pattern[id];
		z=BarMakePattern(cp,8,4,pattern,q); q=z;
	}

	if (chksum) {
		ck = BarPlessey_Checksum(data,slen);
		chksum1 = (ck & 0xf0)>>4;
		chksum2 = (ck & 0x0f);

		if(chksum1>=10) {
			data[slen] = chksum1 + 'A' - 10;
		} else {
			data[slen] = chksum1 + '0';
		}
		if(chksum2>=10) {
			data[slen + 1] = chksum2 + 'A' - 10;
		} else {
			data[slen + 1] = chksum2 + '0';
		}
		data[slen + 2] = NULL; 

	   cp=(ROMDATA char *)BarPlessey_Pattern[chksum1];
	   z=BarMakePattern(cp,8,4,pattern,q); q=z;
		cp=(ROMDATA char *)BarPlessey_Pattern[chksum2];
		z=BarMakePattern(cp,8,4,pattern,q); q=z;
	}

	cp=(ROMDATA char *)BarPlessey_Pattern[17];
	z=BarMakePattern(cp,9,4,pattern,q); q=z;
		BarStruct.p2=q;
        return q;
}

#endif//#ifdef USE_MSI_PLESSEY
