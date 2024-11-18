/*****************************************************************************
|*			
|*  Copyright		:	(c) 2002 CAS
|*  Filename		:	bar_code128.c
|*  Version			:	0.1
|*  Programmer(s)	:	Hong Yi-Phyo (HYP)
|*  Created			:	2003/01/28
|*  Description		:	    
|*				
****************************************************************************/

#include <stdio.h>
#include <string.h>
#include "globals.h"
#include "commbuf.h"
#include "prt_common.h"
#include "bar_common.h"

#ifdef USE_CODE128C
#define START_A 103
#define START_B 104
#define START_C 105
#define STOP    106
#define SHIFT    98 /* only A and B */
#define CODE_A  101 /* only B and C */
#define CODE_B  100 /* only A and C */
#define CODE_C   99 /* only A and B */
#define FUNC_1  102 /* all of them */
#define FUNC_2   97 /* only A and B */
#define FUNC_3   96 /* only A and B */
/* FUNC_4 is CODE_A when in A and CODE_B when in B */


#define CODE_A_CH  ':' /* only B and C */
#define CODE_B_CH  ';' /* only A and C */
#define CODE_C_CH   '<' /* only A and B */
#define SHIFT_CH    '=' /* only A and B */

ROMDATA char BarCode128_2ChTable[] = {				 /* 2-character set code */
	(BAR_2CH_SET_CH - 0x20), FUNC_1, FUNC_2, FUNC_3, 0xff,			/* 01234 */
	(DEL-0x20), START_A, START_B, START_C, STOP, 	/* 56789 */
	CODE_A, CODE_B, CODE_C, SHIFT		 					/* :;<=  */ 
};

ROMDATA char *BarCode128_Pattern[]={	
 /**************************************************************************
 CODE128: 
 **************************************************************************/
	"212222","222122","222221","121223","121322",	   /*  0 ..  4 */
	"131222","122213","122312","132212","221213",	   /*  5 ..  9 */
	"221312","231212","112232","122132","122231",	   /* 10 .. 14 */
	"113222","123122","123221","223211","221132",	   /* 15 .. 19 */
	"221231","213212","223112","312131","311222",	   /* 20 .. 24 */
	"321122","321221","312212","322112","322211",	   /* 25 .. 29 */
	"212123","212321","232121","111323","131123",	   /* 30 .. 34 */
	"131321","112313","132113","132311","211313",	   /* 35 .. 39 */
	"231113","231311","112133","112331","132131",	   /* 40 .. 44 */
	"113123","113321","133121","313121","211331",	   /* 45 .. 49 */
	"231131","213113","213311","213131","311123",	   /* 50 .. 54 */
	"311321","331121","312113","312311","332111",	   /* 55 .. 59 */
	"314111","221411","431111","111224","111422",	   /* 60 .. 64 */
	"121124","121421","141122","141221","112214",	   /* 65 .. 69 */
	"112412","122114","122411","142112","142211",	   /* 70 .. 74 */
	"241211","221114","413111","241112","134111",	   /* 75 .. 79 */
	"111242","121142","121241","114212","124112",	   /* 80 .. 84 */
	"124211","411212","421112","421211","212141",	   /* 85 .. 89 */
	"214121","412121","111143","111341","131141",	   /* 90 .. 94 */
	"114113","114311","411113","411311","113141",	   /* 95 .. 99 */
	"114131","311141","411131",						   /* 100 .. 102 */
	"211412",	   /* 103 Start A */
	"211214",	   /* 104 Start B */
	"211232",	   /* 105 Start C */
	"2331112",	   /* 106 Stop */
	"2"		/* 107       [xx]   Terminate */
};

short Check_2ch_Code(HUGEDATA char *code,short leng,char shift_flag)
{
	short i;
	char c;
	
	if(shift_flag == OFF) {
		for ( i=0; i<leng; i++) {
			c = code[i];
			if (c == BAR_2CH_SET_CH) {
				i++;
				if (i >= leng ) return -1;
				c = code[i];
				if (c>=CODE_A_CH && c<= SHIFT_CH) {
					return i-1;
				}
			}
		}
		return i;
	} else {
		if (code[0]	== 	BAR_2CH_SET_CH) {
			return 2;
		} else {
			return 1;
		}
	}
}	

char BarCode128_Checksum(HUGEDATA char *code,short leng)
{
	short i,j,l,sum;
	unsigned char  c;

    sum=code[0]; 
	l=0;
    for (i=1; i<leng; i++) {
		c=code[i];
		if ((c<0) && (c>105)) return -1;
		j=c*(i);
		sum +=j;
	}
    l = sum % 103;
	return (char)l;
}
#ifdef USE_CODE128ABX
short BarCode128A(HUGEDATA char *data, short slen, HUGEDATA char *trans_data,char start, HUGEDATA char *trans_cdata)
{
	char c;
	short i,j;
	HUGEDATA char *td;

	td = trans_cdata;
	j = 0;
	/* Start Character */
	if (start != 0) {
		trans_data[j] =  START_A;
		j++;
	}
	for (i = 0; i <slen; i++) {
		c = data[i];
		if (c<' ' || c>'_') return -1; //0x20 ~	0x5F
		if (c != BAR_2CH_SET_CH) {
			trans_data[j] =  c - ' '; //0x20
			j++;
			*td++ = c;
		} else {
			i++;
			if (i >= slen ) return -1;
			c = data[i];
			if (c>='0' && c<='4') {   /* "BAR_2CH_SET_CH 0" == BAR_2CH_SET_CH */
				trans_data[j] = BarCode128_2ChTable[c-'0'];	//0x30
				j++;
			} else if (c>='@' && c<='_') {	//0x40 ~ 0x5f
				trans_data[j] = c;
				j++;
			} else {
				return -1;
			}
		}			
	}
	*td = '\0';
	return j;
}

short BarCode128B(HUGEDATA char *data, short slen, HUGEDATA char *trans_data,char start, HUGEDATA char *trans_cdata)
{
	char c;
	short i,j;
	HUGEDATA char *td;

	td = trans_cdata;
	j = 0;
	/* Start Character */
	if (start != 0) {
		trans_data[j] =  START_B;
		j++;
	}

	for (i = 0; i <slen; i++) {
		c = data[i];
		if (c<' ' || c>'~') return -1; //0x20 ~ 0x7e
		if (c != BAR_2CH_SET_CH) {
			trans_data[j] =  c - ' '; //0x20
			j++;
			*td++ = c;
		} else {
			i++;
			if (i >= slen ) return -1;
			c = data[i];
			if (c>='0' && c<='5') {	 //0x30 ~ 0x35	   /* "BAR_2CH_SET_CH 0" == BAR_2CH_SET_CH */
				trans_data[j] = BarCode128_2ChTable[c-'0'];//0x30
				j++;
			} else {
				return -1;
			}
		}			
	}
	*td = '\0';
	return j;
}
#endif//#ifdef USE_CODE128ABX

short BarCode128C(HUGEDATA char *data, short slen, HUGEDATA char *trans_data,char start, HUGEDATA char *trans_cdata)
{	
	unsigned char even;
	short tlen,i,j;
	char right,left;
	HUGEDATA char *td;

//	td = BarData1;
	td = trans_cdata;
	j=0;
	/* Start Character */
	if (start != 0) {
		trans_data[j] =  START_C;
		j++;
	}
	even = slen % 2;
	tlen = slen / 2;
	if (!even) {
		for (i = 0; i <tlen; i++) {
			left = data[2*i] - '0';	//0x30
			*td = left + '0';
			td++;
			if (left<0 || left >9)  return -1;
			right = data[2*i+1] - '0'; //0x30
			*td = right + '0';
			td++;
			if (right<0 || right>9) return -1;
			trans_data[j] = right + left * 10;
			j++;
		}
	} else {
		tlen++;
		right = data[0] - 0x30;
		*td = '0';
		td++;
		*td = right + '0';
		td++;
		if (right<0 || right>9) return -1;
 		trans_data[j] = right;
 		j++;		
		for (i = 1; i <tlen; i++) {
			left = data[2*i-1] - '0'; //0x30
			*td = left + '0';
			td++;
			if (left<0 || left >9) return -1;	
			right = data[2*i] - '0'; //0x30
			*td = right + '0';
			td++;
			if (right<0 || right>9) return -1;	
			trans_data[j] = right + left * 10;
			j++;
		}
	}
	*td = '\0';
	return j;	//include start C 
}

#ifdef USE_CODE128ABX
short BarCode128X(HUGEDATA char *data, short slen, HUGEDATA char *trans_data, HUGEDATA char *trans_cdata)
{
	char c, type, shift_flag;
	short i,j,stlen,ttlen,k;

	shift_flag = OFF;
	j = 0;
	k = 0;
	c = data[0];
	if (c != BAR_2CH_SET_CH) {
			return -1;
	} else {
		c = data[1];
		if (c>='6' && c<='8') {	//0x36 ~ 0x38	   /* "BAR_2CH_SET_CH 0" == BAR_2CH_SET_CH */
			trans_data[j] = BarCode128_2ChTable[c-'0'];	//0x30
			type = trans_data[j];
			j++;
		} else {
			return -1;
		}
	}
	for (i = 2; i <slen; i++) {
		stlen = Check_2ch_Code(&data[i],slen-i,shift_flag);
		if(stlen == -1) return -1;
		switch(type) {
			case START_A:
				type = CODE_A;
			case CODE_A:
				ttlen = BarCode128A(&data[i], stlen, &trans_data[j],0,&trans_cdata[k]);
				k += strlen(&trans_cdata[k]);
				if(ttlen == -1) return -1;
				if (shift_flag == ON) {
					type = CODE_B;
				}
				break;

			case START_B:
				type = CODE_B;
			case CODE_B:
				ttlen = BarCode128B(&data[i], stlen, &trans_data[j],0,&trans_cdata[k]);
				k += strlen(&trans_cdata[k]);
				if(ttlen == -1) return -1;
				if (shift_flag == ON) {
					type = CODE_A;
				}
				break;

			case START_C:
				type = CODE_C;
			case CODE_C:
				ttlen = BarCode128C(&data[i], stlen, &trans_data[j],0,&trans_cdata[k]);
				k += strlen(&trans_cdata[k]);
				if(ttlen == -1) return -1;
				break;
		}
		j = j + ttlen;
		i = i + stlen;
		if (i < slen) {
			if (shift_flag == OFF) {
				c = data[i];
				if (c == BAR_2CH_SET_CH) {
					i++;
					if (i >= slen ) return -1;
					c = data[i];
					if (c>=CODE_A_CH && c<= SHIFT_CH) {
						switch(c) {
							case CODE_A_CH:
								type = CODE_A;
								trans_data[j] = CODE_A;
								j++;
								break;
							case CODE_B_CH:
								type = CODE_B;
								trans_data[j] = CODE_B;
								j++;
								break;
							case CODE_C_CH:
								type = CODE_C;
								trans_data[j] = CODE_C;
								j++;
								break;
							case SHIFT_CH:
								shift_flag = ON;
								trans_data[j] = SHIFT;
								j++;
								if (type == CODE_A) type = CODE_B;
								else if (type == CODE_B) type = CODE_A;
								else return -1;
						}
					} else {
						return -1;
					}
				} else {
					return -1;
				}
			} else {
				i--;
				shift_flag = OFF;
			}
		}
	}
	return j;
}

// 시작은 CODE128C로 시작
// Numeric이 아닐 경우 CODE128B로 변경
// 연속 2char가 Numeric이면 CODE128C로 변경
void BarCode128X_StrConv(HUGEDATA char *dest,HUGEDATA char *src)
{
	HUGEDATA char *p1, *p2;
	char temp;
	INT8U mode_alpha;
	INT8U odd;

	p1 = dest;
	p2 = src;

	mode_alpha = OFF;
	odd = 0;

	*p1++ = BAR_2CH_SET_CH;
	*p1++ = '8';	// code128c
	while(1) {
		if (mode_alpha)
		{
			if (*p2 >= '0' && *p2 <= '9')
			{
				if (*(p2+1) >= '0' && *(p2+1) <= '9')
				{
					*p1++ = BAR_2CH_SET_CH;
					*p1++ = CODE_C_CH;	// code128c
					mode_alpha = OFF;
					odd = 1;
				}
			}
			else
			{
				if (*p2 == '\0') break;
			}
		}
		else
		{
			odd ^= 1;
			if (*p2 >= '0' && *p2 <= '9')
			{
			}
			else
			{
				if (!odd)
				{
					p1--;
					temp = *p1;
					*p1++ = BAR_2CH_SET_CH;
					*p1++ = CODE_B_CH;	// code128b
					*p1++ = temp;
					if (*p2 == '\0') break;
				}
				else
				{
					if (*p2 == '\0') break;
					*p1++ = BAR_2CH_SET_CH;
					*p1++ = CODE_B_CH;	// code128b
				}
				mode_alpha = ON;
			}
		}
			
		*p1 = *p2;

		p1++;
		p2++;
	}
	*p1 = '\0';
}
#endif//#ifdef USE_CODE128ABX
short BarCode128(HUGEDATA char *data,char chksum,HUGEDATA INT8U *pattern, char code_type, HUGEDATA char *trans_cdata)
{
	unsigned char ck;
	short slen,tlen,q,i,id,z;
	ROMDATA char *cp;
	char trans_data[MAX_BAR_DATA_BUF_SIZE];
#ifdef USE_CODE128ABX
	char temp_data[MAX_BAR_DATA_BUF_SIZE*2];	//code128x용 버퍼
#endif

	slen = strlen(data);
    
    switch (code_type) {
#ifdef USE_CODE128ABX
		case CODE128_ATYPE:
			tlen = BarCode128A(data,slen,trans_data,1,trans_cdata);
			if (chksum) {
				ck=BarCode128_Checksum(trans_data,tlen);
			}
			break;
		case CODE128_BTYPE:
			tlen = BarCode128B(data,slen,trans_data,1,trans_cdata);
			if (chksum) {
				ck=BarCode128_Checksum(trans_data,tlen);
			}
			break;
		case CODE128_XTYPE:
			BarCode128X_StrConv(temp_data, data);
			slen = strlen(temp_data);
			tlen = BarCode128X(temp_data,slen,trans_data,trans_cdata);
			if (chksum) {
				ck = BarCode128_Checksum(trans_data,tlen);
 			}
			break;
#endif//#ifdef USE_CODE128ABX
		case CODE128_CTYPE:
			tlen = BarCode128C(data,slen,trans_data,1,trans_cdata);
			if (chksum) {
				ck = BarCode128_Checksum(trans_data,tlen);
 			}			
			break;
	}
	
	if(tlen == -1) return -1;

	q=0;
	BarStruct.p0=BarStruct.p1=0;

	for (i=0; i<tlen; i++) {
		id=trans_data[i];
		cp=(ROMDATA char *)BarCode128_Pattern[id];
		z=BarMakePattern(cp,6,4,pattern,q); q=z;
	}
	if (chksum == 1) {
		id=ck;
		cp=(ROMDATA char *)BarCode128_Pattern[id];
		z=BarMakePattern(cp,6,4,pattern,q); q=z;
//		*(data + slen)=ck;
//		*(data + slen + 1)=NULL;
	}
	/* Stop Character */
	cp=(ROMDATA char *)BarCode128_Pattern[106];

	z=BarMakePattern(cp,6,4,pattern,q); q=z;
	/* Terminate Bar */
	cp=(ROMDATA char *)BarCode128_Pattern[107];
	z=BarMakePattern(cp,1,4,pattern,q); q=z;
	BarStruct.p2=q;
    return q;
}
#endif//#ifdef USE_CODE128C
