/*****************************************************************************
|*			
|*  Copyright		:	(c) 2002 CAS
|*  Filename		:	bar_upcean.c
|*  Version		:	0.1
|*  Programmer(s)	:	Hong Yi-Phyo (HYP)
|*  Created		:	2002/12/24
|*  Description		:	    
|*				
****************************************************************************/

#include <stdio.h>
#include <string.h>
#include "globals.h"
#include "commbuf.h"
#include "bar_common.h"

ROMDATA char *BarUpcEan_Pattern[]={
	"3211", /* UPC: Odd  0:0001101 [0] EAN Code Set A*/
	"2221", /*           1:0011001 */
	"2122", /*           2:0010011 */
	"1411", /*           3:0111101 */
	"1132", /*           4:0100011 */
	"1231", /*           5:0110001 */
	"1114", /*           6:0101111 */
	"1312", /*           7:0111011 */
	"1213", /*           8:0110111 */
	"3112", /*           9:0001011 */

	"1123", /* UPC: Even  0:0100111 EAN Code Set B [10]*/
	"1222", /*           1:0110011 */
	"2212", /*           2:0011011 */
	"1141", /*           3:0100001 */
	"2311", /*           4:0011101 */
	"1321", /*           5:0111001 */
	"4111", /*           6:0000101 */
	"2131", /*           7:0010001 */
	"3121", /*           8:0001001 */
	"2113", /*           9:0010111 */

	"3211", /*      Even 0:1110010 [20] EAN Code Set C */
	"2221", /*           1:1100110 */
	"2122", /*           2:1101100 */
	"1411", /*           3:1000010 */
	"1132", /*           4:1011100 */
	"1231", /*           5:1001110 */
	"1114", /*           6:1010000 */
	"1312", /*           7:1000100 */
	"1213", /*           8:1001000 */
	"3112", /*           9:1110100 */

	"111" ,    /* 101 Left  Margin and GardBar  [30->30] */
	"11111",   /* 01010             Center Bar  	 [34->31] */
	"111" ,    /* 1010 Right GardBar and Margin 	 [38->32] */
	"111" ,    /* 101    ,3bit Left  Margin and GardBar  [42->33] Version E */
	"111111" , /* 010101 ,6bit Right Margin and GardBar  [46->34] Version E */

	"112" ,    /* 1011   ,4bit Left  GardBar for Supplement Code [50->35]   */
	"11" ,     /* 01     ,2bit Delineartor                       [51->36]   */
};

ROMDATA char *BarEan_PatternSet[]={ /* EAN Code Set */
	"AAAAAA","AABABB","AABBAB","AABBBA","ABAABB",
	"ABBAAB","ABBBAA","ABABAB","ABABBA","ABBABA",
};

#ifdef USE_UPCE0_UPCE1
ROMDATA char *BarUpce0_PatternSet[]={ /* C: Even , A: Odd Parity */
	"BBBAAA", /* 0 */
	"BBABAA", /* 1 */
	"BBAABA", /* 2 */
	"BBAAAB", /* 3 */
	"BABBAA", /* 4 */
	"BAABBA", /* 5 */
	"BAAABA", /* 6 */
	"BABABA", /* 7 */
	"BABAAB", /* 8 */
	"BAABAB", /* 9 */
};
ROMDATA char *BarUpce1_PatternSet[]={ /* C: Even , A: Odd Parity */
	"AAABBB", /* 0 */
	"AABABB", /* 1 */
	"AABBAB", /* 2 */
	"AABBBA", /* 3 */
	"ABAABB", /* 4 */
	"ABBAAB", /* 5 */
	"ABBBAA", /* 6 */
	"ABABAB", /* 7 */
	"ABABBA", /* 8 */
	"ABBABA", /* 9 */
};
#endif//#ifdef USE_UPCE0_UPCE1
ROMDATA char *BarUpceS2_PatternSet[]={"AA","AB","BA","BB",};
ROMDATA char *BarUpceS5_PatternSet[]={ /* C: Even , A: Odd Parity */
	"BBAAA", /* 0 */
	"BABAA", /* 1 */
	"BAABA", /* 2 */
	"BAAAB", /* 3 */
	"ABBAA", /* 4 */
	"AABBA", /* 5 */
	"AAABB", /* 6 */
	"ABABA", /* 7 */
	"ABAAB", /* 8 */
	"AABAB", /* 9 */
};

#ifdef USE_UPCE0_UPCE1
ROMDATA char *BarUpce2Upca_Convert[]={
	
	"0AB00000CDE","0AB10000CDE",
	"0AB20000CDE","0ABC00000DE",
	"0ABCD00000E","0ABCDE00005",
	"0ABCDE00006","0ABCDE00007",
	"0ABCDE00008","0ABCDE00009"
};

void BarUpce2Upca(HUGEDATA char *upcE,HUGEDATA char *upcA)
{
	short i;
	char  c;
	i=(upcE[5]-'0');
	strcpy(upcA,BarUpce2Upca_Convert[i]);
	for (i=0; i<11; i++) {
		c=upcA[i];
		if (c>='A') {
			c=upcE[(short)(upcA[i]-'A')];
			upcA[i]=(INT8U)c;
		}
	}
}
#endif//#ifdef USE_UPCE0_UPCE1

#ifdef USE_LOTTEMART_BARCODE_FUNCTION
char Lottemart_GS1_Checksum(HUGEDATA char *data,short leng)
{
	short e,o,i;	//,digit

	e=o=0;

	for (i=leng-1; i>=0; ) 
	{
		if(data[i] == '#')i--;
	     e+=(data[i--]-'0');

		 if(data[i] == '#')i--;
		 if (i>=0) 
		 {
			o += (data[i--]-'0');
	     }
	}

	e=e*3;
	e=e+o;
	e=10-e%10;
	if (e==10) e=0;
	return (char)(e+'0');
}
#endif

char BarUpc_Checksum(HUGEDATA char *data,short leng)
{
	short e,o,i;	//,digit

	e=o=0;

#ifdef 	USE_BARCODE_SHARP_CHECKDIGIT
	for (i=leng-1; i>=0; ) 
	{	
		if(data[i] == '#')	//Indonesia emporium
		{
			e+=3;
			i--;
		}
		else	e+=(data[i--]-'0');
		
	    if (i>=0) 
		{
			if(data[i] == '#')	//Indonesia emporium
			{
				o+=3;
				i--;
			}
			else o += (data[i--]-'0');
	    }
	}
#else
	for (i=leng-1; i>=0; ) {
	     e+=(data[i--]-'0');
	     if (i>=0) {
		o += (data[i--]-'0');
	     }
	}
#endif
	e=e*3;
	e=e+o;
	e=10-e%10;
	if (e==10) e=0;
	return (char)(e+'0');
}
#ifdef USE_EAN13P2_UPCAP2_P5_EAN8P2_P5
short BarUpcEan_Sup2(HUGEDATA char *data,HUGEDATA INT8U *pattern,short pos)
{
	unsigned char c1,c2,ck;
	short fcode,codeset,q,z;
	ROMDATA char *cp;

	c1=(unsigned char)data[0]-'0';
	c2=(unsigned char)data[1]-'0';
	z=(short)c1*10+(short)c2;
	ck=z%4;
	q=pos;
	BarStruct.x1=pos;
	fcode = ck;

	/* Left Hand Bar */
	cp=(ROMDATA char *)BarUpcEan_Pattern[35];
	z=BarMakePattern(cp,3,4,pattern,q); q=z;

	codeset=(BarUpceS2_PatternSet[fcode][0]-'A')*10;

	cp=(ROMDATA char *)BarUpcEan_Pattern[codeset+c1];
	z=BarMakePattern(cp,4,0,pattern,q); q=z;

	cp=(ROMDATA char *)BarUpcEan_Pattern[36];
	z=BarMakePattern(cp,2,0,pattern,q); q=z;

	codeset=(BarUpceS2_PatternSet[fcode][1]-'A')*10;
	cp=(ROMDATA char *)BarUpcEan_Pattern[codeset+c2];

	z=BarMakePattern(cp,4,0,pattern,q); q=z;
	BarStruct.x2=q;
	return q;
}
#endif//#ifdef USE_EAN13P2_UPCAP2_P5_EAN8P2_P5
#ifdef USE_EAN13SUP5
short BarUpcEan_Sup5(HUGEDATA char *data,HUGEDATA INT8U *pattern,short pos)
{
//	unsigned char c;
	short q,i,fcode,codeset,slen,e,o,z;
	ROMDATA char *cp;

	q=pos;
	BarStruct.x1=pos;
	slen=strlen(data);

	e=o=0;
	for (i=slen-1; i>=0; ) {
		o+=(short)(data[i]-'0')*3; i--;
		if (i>=0) {
			e+=(short)(data[i]-'0')*9; i--;
		}
	}
	e = e + o;
	e = e%10;

	/* Left Gard Bar */
	cp=(ROMDATA char *)BarUpcEan_Pattern[35];
	z=BarMakePattern(cp,3,4,pattern,q); q=z;
	fcode = (short)(e);
	for (i=0; i<slen; i++) {
		if (i) {
			/* Seperator Bar */
			cp=(ROMDATA char *)BarUpcEan_Pattern[36];
			z=BarMakePattern(cp,2,0,pattern,q); q=z;
		}
		codeset=(BarUpceS5_PatternSet[fcode][i]-'A')*10;
		cp=(ROMDATA char *)BarUpcEan_Pattern[codeset+data[i]-'0'];
		z=BarMakePattern(cp,4,0,pattern,q); q=z;
	}
	BarStruct.x2=q;
	return q;
}
#endif//#ifdef USE_EAN13SUP5

#ifdef USE_UPCA
short BarUpca(HUGEDATA char *data,char chksum,HUGEDATA INT8U *pattern)
/****************************************************************************
 UPC_A : 11 or 12 digit , 11 일때는 chksum 을 1로 하면 12로 전환됨.
 ****************************************************************************/
{
	char ck;
	ROMDATA char *cp;
	short slen,q,i,id,z;//,sz

	slen=strlen(data);
	if (chksum) {
	    ck=BarUpc_Checksum(data,slen);
		*(data + slen)=ck;
		*(data + slen + 1)=NUL;
	}
	/* Start Pattern  */
	q=0;
	BarStruct.p0=0;
	cp=(ROMDATA char *)BarUpcEan_Pattern[30];
	z=BarMakePattern(cp,3,4,pattern,q); q=z;
//	BarStruct.p1=q;
	/* Left  Pattern : Odd  Parity */
	for (i=0; i<6; i++) {
		id=data[i]-'0';
		cp=(ROMDATA char *)BarUpcEan_Pattern[id];
		z=BarMakePattern(cp,4,0,pattern,q); q=z;
		if(i==0)BarStruct.p1=q; 
	}
	/* Center Bar */
	BarStruct.p2=q;
	cp=(ROMDATA char *)BarUpcEan_Pattern[31];
	z=BarMakePattern(cp,5,0,pattern,q); q=z;
	BarStruct.p3=q;
	/* Right Pattern : Even Parity */
	for (i=6; i<slen; i++) {
		id=data[i]-'0';
		cp=(ROMDATA char *)BarUpcEan_Pattern[id];
		z=BarMakePattern(cp,4,4,pattern,q); q=z;
		if(i==10)BarStruct.p4=q;
	}
	/* Check Sum */
	if (chksum) {
		BarStruct.p4=q;
		id=ck-'0';
		cp=(ROMDATA char *)BarUpcEan_Pattern[id];
		z=BarMakePattern(cp,4,4,pattern,q); q=z;
	}
	/* End Bar */
//	BarStruct.p4=q;
	cp=(ROMDATA char *)BarUpcEan_Pattern[32];
	z=BarMakePattern(cp,3,4,pattern,q); q=z;
	BarStruct.p5=q;
	return q;
}
#endif//#ifdef USE_UPCA
#ifdef USE_EAN13P2_UPCAP2_P5_EAN8P2_P5
short BarUpcaSup2(HUGEDATA char *upc,HUGEDATA char *sup2,char chksum,HUGEDATA INT8U *pattern)
{
	short r,z;
	r=BarUpca(upc,chksum,pattern);
//	r+=BarStruct.ns*6;
	r+=BarStruct.nb*9;
	z=BarUpcEan_Sup2(sup2,pattern,r);
	return z;
}

short BarUpcaSup5(HUGEDATA char *upc,HUGEDATA char *sup5,char chksum,HUGEDATA INT8U *pattern)
{
	short r,z;
	r=BarUpca(upc,chksum,pattern);
//	r+=BarStruct.ns*6;
	r+=BarStruct.nb*9;
	z=BarUpcEan_Sup5(sup5,pattern,r);
	return z;
}
#endif//#ifdef USE_EAN13P2_UPCAP2_P5_EAN8P2_P5

#ifdef USE_UPCE0_UPCE1
short BarUpce0(HUGEDATA char *data,HUGEDATA INT8U *pattern)
/****************************************************************************
 UPC_E : 6 Digit , 내부에 처음 '0' 과 Checksum이 Pattern내부에 포함된다.
        "123456" -> "0 123456 5" 
 ****************************************************************************/
{
	char ck;
	ROMDATA char *cp;
	short slen,q,z,i;//,id;
	short fcode,codeset;
	char  upcA[15];

	BarStruct.p0=0;
	/* Check Sum 을 구하기 위해 UPCE 의 데이타를  UPCA로 변경 */
	BarUpce2Upca(data,upcA);
	slen=strlen(upcA);
	/* Check Sum 을 확보 UPCE 의 데이타를  UPCA로 변경        */
	ck=BarUpc_Checksum(upcA,slen);
	slen=strlen(data);
//
	*(data + slen)=ck;
	*(data + slen + 1)=NULL;
//
	/* Start Pattern  */
	q=0;
	cp=(ROMDATA char *)BarUpcEan_Pattern[33];
	z=BarMakePattern(cp,3,4,pattern,q); q=z;
	BarStruct.p1=q;

	fcode = (short)(ck-'0');
	for (i=0; i<slen; i++) {
		codeset=(BarUpce0_PatternSet[fcode][i]-'A')*10;
		cp=(ROMDATA char *)BarUpcEan_Pattern[codeset+(data[i]-'0')];
		z=BarMakePattern(cp,4,0,pattern,q); q=z;
	}
	/* End Bar */
	BarStruct.p2=q;
	cp=(ROMDATA char *)BarUpcEan_Pattern[34];
	z=BarMakePattern(cp,6,0,pattern,q); q=z;
	BarStruct.p3=q;
	BarStruct.p4=q;
	BarStruct.p5=q;

	return q;
}

short BarUpce0Sup2(HUGEDATA char *upc,HUGEDATA char *sup2,HUGEDATA INT8U *pattern)
{
	short r,z;
	r=BarUpce0(upc,pattern);
//	r+=BarStruct.ns*6;
	r+=BarStruct.nb*9;
	z=BarUpcEan_Sup2(sup2,pattern,r);
	return z;
}

short BarUpce0Sup5(HUGEDATA char *upc,HUGEDATA char *sup5,HUGEDATA INT8U *pattern)
{
	short r,z;
	r=BarUpce0(upc,pattern);
//	r+=BarStruct.ns*6;
	r+=BarStruct.nb*9;
	z=BarUpcEan_Sup5(sup5,pattern,r);
	return z;
}


short BarUpce1(HUGEDATA char *data,HUGEDATA INT8U *pattern)
/****************************************************************************
 UPC_E1 : UPC_E0 과의 차이는 Pattern 의 차이
 ****************************************************************************/
{
	char ck;
	ROMDATA char *cp;
	short slen,q,z,i;//,id;
	short fcode,codeset;
	char  upcA[15];

	/* Check Sum 을 구하기 위해 UPCE 의 데이타를  UPCA로 변경 */
	BarUpce2Upca(data,upcA);
	slen=strlen(upcA);
	/* Check Sum 을 확보 UPCE 의 데이타를  UPCA로 변경        */
	ck=BarUpc_Checksum(upcA,slen);
	slen=strlen(data);
//
	*(data + slen)=ck;
	*(data + slen + 1)=NULL;
//

	/* Start Pattern  */
	q=0;
	BarStruct.p0=0;
	cp=(ROMDATA char *)BarUpcEan_Pattern[33];
	z=BarMakePattern(cp,3,4,pattern,q); q=z;
	BarStruct.p1=q;

	fcode = (short)(ck-'0');
	for (i=0; i<slen; i++) {
		codeset=(BarUpce1_PatternSet[fcode][i]-'A')*10;
		cp=(ROMDATA char *)BarUpcEan_Pattern[codeset+(data[i]-'0')];
		z=BarMakePattern(cp,4,0,pattern,q); q=z;
	}
	BarStruct.p2=q;
	/* End Bar */
	cp=(ROMDATA char *)BarUpcEan_Pattern[34];
	z=BarMakePattern(cp,6,0,pattern,q); q=z;
	BarStruct.p3=q;
	BarStruct.p4=q;
	BarStruct.p5=q;
	return q;
}
#endif//#ifdef USE_UPCE0_UPCE1
#ifdef USE_EAN13
short BarEan13(HUGEDATA char *data,char chksum,HUGEDATA INT8U *pattern)
/****************************************************************************
 EAN13 : 12 or 13 digit , 12 일때는 chksum 을 1로 하면 13로 전환됨.
 ****************************************************************************/
{
	char ck;
	ROMDATA char *cp;
	short slen,q,z,i,id,codeset,fcode;
//	char string_buf[30];

	slen=strlen(data);
	if (chksum) {
	    ck=BarUpc_Checksum(data,slen);
		*(data + slen)=ck;
		*(data + slen + 1)=NUL;
	}
	/* Start Pattern  */
	q=0;
	BarStruct.p0=0;
	cp=(ROMDATA char *)BarUpcEan_Pattern[30];
	z=BarMakePattern(cp,3,4,pattern,q); q=z;
	BarStruct.p1=q;
	/* Left  Pattern : Odd  Parity */
	fcode = (short)(data[0]-'0');
	for (i=1; i<7; i++) {
		id=data[i]-'0';
		codeset=(BarEan_PatternSet[fcode][i-1]-'A')*10;
		cp=(ROMDATA char *)BarUpcEan_Pattern[codeset+(data[i]-'0')];
		z=BarMakePattern(cp,4,0,pattern,q); q=z;
	}
	BarStruct.p2=q;
	/* Center Bar */
	cp=(ROMDATA char *)BarUpcEan_Pattern[31];
	z=BarMakePattern(cp,5,0,pattern,q); q=z;
	BarStruct.p3=q;

	/* Right Pattern : Even Parity */
	for (i=7; i<slen; i++) {
		id=data[i]-'0'+20;
		cp=(ROMDATA char *)BarUpcEan_Pattern[id];
		z=BarMakePattern(cp,4,4,pattern,q); q=z;
	}
	/* Check Sum */
	if (chksum) {
		id=ck-'0'+20;
		cp=(ROMDATA char *)BarUpcEan_Pattern[id];
		z=BarMakePattern(cp,4,4,pattern,q); q=z;
	}
	/* End Bar */
	BarStruct.p4=q;
	cp=(ROMDATA char *)BarUpcEan_Pattern[32];
	z=BarMakePattern(cp,3,4,pattern,q); q=z;
	BarStruct.p5=q;
	return q;
}
#endif//#ifdef USE_EAN13
#ifdef USE_EAN13P2_UPCAP2_P5_EAN8P2_P5
short BarEan13Sup2(HUGEDATA char *ean13,HUGEDATA char *sup2,char chksum,HUGEDATA INT8U *pattern)
{
	short r,z;
	r=BarEan13(ean13,chksum,pattern);
//	r+=BarStruct.ns*6;
	r+=BarStruct.nb*9;

	z=BarUpcEan_Sup2(sup2,pattern,r);
	return z;
}
#endif//#ifdef USE_EAN13P2_UPCAP2_P5_EAN8P2_P5
#ifdef USE_EAN13SUP5
short BarEan13Sup5(HUGEDATA char *ean13,HUGEDATA char *sup5,char chksum,HUGEDATA INT8U *pattern)
{
	short r,z;
	r=BarEan13(ean13,chksum,pattern);
//	r+=BarStruct.ns*6;
	r+=BarStruct.nb*9;
	z=BarUpcEan_Sup5(sup5,pattern,r);
	return z;
}
#endif//#ifdef USE_EAN13SUP5
#ifdef USE_EAN8
short BarEan8(HUGEDATA char *data,char chksum,HUGEDATA INT8U *pattern)
/****************************************************************************
 EAN8 : 7 or 8 digit , 7 일때는 chksum 을 1로 하면 8로 전환됨.
 ****************************************************************************/
{
	char ck;
	ROMDATA char *cp;
	short slen,q,z,i,id;

	slen=strlen(data);
	if (chksum) {
	    ck=BarUpc_Checksum(data,slen);
		*(data + slen)=ck;
		*(data + slen + 1)=NUL;
	}
	/* Start Pattern  */
	q=0;
	BarStruct.p0=0;
	cp=(ROMDATA char *)BarUpcEan_Pattern[30];
	z=BarMakePattern(cp,3,4,pattern,q); q=z;
	BarStruct.p1=q;
	/* Left  Pattern : Odd  Parity */
	for (i=0; i<4; i++) {
		id=data[i]-'0';
		cp=(ROMDATA char *)BarUpcEan_Pattern[id];
		z=BarMakePattern(cp,4,0,pattern,q); q=z;
	}
	BarStruct.p2=q;
	/* Center Bar */
	cp=(ROMDATA char *)BarUpcEan_Pattern[31];
	z=BarMakePattern(cp,5,0,pattern,q); q=z;
	BarStruct.p3=q;

	/* Right Pattern : Even Parity */
	for (i=4; i<slen; i++) {
		id=data[i]-'0'+20;
		cp=(ROMDATA char *)BarUpcEan_Pattern[id];
		z=BarMakePattern(cp,4,4,pattern,q); q=z;
	}
	/* Check Sum */
	if (chksum) {
		id=ck-'0'+20;
		cp=(ROMDATA char *)BarUpcEan_Pattern[id];
		z=BarMakePattern(cp,4,4,pattern,q); q=z;
	}
	/* End Bar */
	BarStruct.p4=q;
	cp=(ROMDATA char *)BarUpcEan_Pattern[32];
	z=BarMakePattern(cp,3,4,pattern,q); q=z;
	BarStruct.p5=q;
	return q;
}
#endif//#ifdef USE_EAN8
#ifdef USE_EAN13P2_UPCAP2_P5_EAN8P2_P5
short BarEan8Sup2(HUGEDATA char *ean8,HUGEDATA char *sup2,char chksum,HUGEDATA INT8U *pattern)
{
	short r,z;
	r=BarEan8(ean8,chksum,pattern);
//	r+=BarStruct.ns*6;
	r+=BarStruct.nb*9;
	z=BarUpcEan_Sup2(sup2,pattern,r);
	return z;
}

short BarEan8Sup5(HUGEDATA char *ean8,HUGEDATA char *sup2,char chksum,HUGEDATA INT8U *pattern)
{
	short r,z;
	r=BarEan8(ean8,chksum,pattern);
//	r+=BarStruct.ns*6;
	r+=BarStruct.nb*9;
	z=BarUpcEan_Sup5(sup2,pattern,r);
	return z;
}
#endif//#ifdef USE_EAN13P2_UPCAP2_P5_EAN8P2_P5




