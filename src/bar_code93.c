/*****************************************************************************
|*			
|*  Copyright		:	(c) 2002 CAS
|*  Filename		:	bar_code93.c
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

#ifdef USE_CODE93
#define START_STOP 47
#define EXTEND_DOLLAR   43
#define EXTEND_PERCENT   44
#define EXTEND_SLASH   45
#define EXTEND_PLUS   46
/*      00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001111111111111111111111111111 */
/*      00000000001111111111222222222233333333334444444444555555555566666666667777777777888888888899999999990000000000111111111122222222 */
/*      01234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567 */
ROMDATA char BarCode93_Shift_Set[] =
       "%$$$$$$$$$$$$$$$$$$$$$$$$$$%%%%% ///  ///// /             /%%%%%%                          %%%%%%++++++++++++++++++++++++++%%%%%";
ROMDATA char BarCode93_Shift_Set2[] =
       "UABCDEFGHIJKLMNOPQRSTUVWXYZABCDE ABC  FGHIJ L             ZFGHIJV                          KLMNOWABCDEFGHIJLKMNOPQRSTUVWXYZPQRST";
ROMDATA char *BarCode93_Pattern[]= {
	"131112", "111213", "111312", "111411", "121113",  /*  0  -  4 */
	"121212", "121311", "111114", "131211", "141111",  /*  5  -  9 */
	"211113", "211212", "211311", "221112", "221211",  /*  A  -  E */
	"231111", "112113", "112212", "112311", "122112",  /*  F  -  J */
	"132111", "111123", "111222", "111321", "121122",  /*  K  -  O */
	"131121", "212112", "212211", "211122", "211221",  /*  P  -  T */
	"221121", "222111", "112122", "112221", "122121",  /*  U  -  Y */
	"123111", "121131", "311112", "311211", "321111",  /*  Z  -  $ */
	"112131", "113121", "211131", "121221", "312111",  /*  /  - (%) */
	"311121", "122211", "111141",                       /* (/) -  (terminator) */
	"1"
};

ROMDATA char BarCode93_Set[]={
  /* SP,  !,  ",  #,  $,  %,  &,  ',  (,  ),  *,  +,  ,,  -,  .,  /, */
	 38,  0,  0,  0, 39, 42,  0,  0,  0,  0,  0, 41,  0, 36, 37, 40,
  /*  0,  1,  2,  3,  4,  5,  6,  7,  8, 9,  :,  ;,  <,  =,  >,  ?,  @, */
	  0,  1,  2,  3,  4,  5,  6,  7,  8, 9,  0,  0,  0,  0,  0,  0,	 0,
  /*  A,  B,  C,  D,  E,  F,  G,  H,  I,  J,  K,  L,  M,  N,  O,    */
	  10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24,
  /*  P,  Q,  R,  S,  T,  U,  V,  W,  X,  Y,  Z, */
	 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35,
};

char BarCode93_Unset[]={
	 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 65, 66, 67, 68, 69, 70,
	 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86,
	 87, 88, 89, 90, 45, 46, 32, 36, 47, 43, 37,
};

char BarCode93_Checksum_CK(HUGEDATA char *code,short leng,INT8U *k)
{
	short i,sum_c,sum_k,c1,k1;
	char ck;
	sum_c=0;
	sum_k=0;
	k1=2;
	c1=1;
	for (i=leng-1; i>=0; i--) {
		ck=code[i];
		sum_c+=ck*c1;
		sum_k+=ck*k1;
		k1++; c1++;
		if(k1>15) k1=1;
		if(c1>20) c1=1;
	}
	sum_c=sum_c%47;
	sum_k=sum_k + sum_c;
	sum_k=sum_k%47;
	*k=sum_k;
	return sum_c;
}

char BarCode93_Verify(HUGEDATA char *text)
{
    INT16S i;

    if (!strlen(text))
	return -1;

    for (i=0; text[i]; i++) {
       	if (text[i] < ' ' || text[i] > 'Z')	//0x20 ~ 0x5a
			return -1;
       	if (!BarCode93_Set[text[i]-0x20] && (text[i] != '0'))
           	return -1;
	}
    return 0;
}
#ifdef USE_CODE93FULL
char BarCode93e_Verify(HUGEDATA char *text)
{
    INT16S i;

    if (!strlen(text))
	return -1;

    for (i=0; text[i]; i++) {
       	if (text[i] < ' ' || text[i] > '~') //0x20 ~ 0x7e
			return -1;
	}
    return 0;
}
#endif//#ifdef USE_CODE93FULL
short BarCode93t(HUGEDATA char *data,char chksum,HUGEDATA INT8U *pattern,short slen)
{
	INT8U ck,ck_k;
	short q,i,id,z;
	ROMDATA char *cp;

//	slen=strlen(data);
	if (chksum) {
		ck=BarCode93_Checksum_CK(data,slen,&ck_k);
	}
	q=0;
	BarStruct.p0=BarStruct.p1=0;
	/* Start Character */
	id=START_STOP;
	cp=(ROMDATA char *)BarCode93_Pattern[id];
	z=BarMakePattern(cp,6,4,pattern,q); q=z;
	for (i=0; i<slen; i++) {
		id=data[i];
		cp=(ROMDATA char *)BarCode93_Pattern[id];
		z=BarMakePattern(cp,6,4,pattern,q); q=z;
	}
//HYP 20040621
/*
	if (chksum>=1) {
		id=ck;
		cp=(ROMDATA char *)BarCode93_Pattern[id];
		z=BarMakePattern(cp,6,4,pattern,q); q=z;
//		*(data + slen)=BarCode93_Unset[ck];
//		*(data + slen + 1)=NULL;
	}
	if (chksum>=2) {
		id=ck_k;
		cp=(ROMDATA char *)BarCode93_Pattern[id];
		z=BarMakePattern(cp,6,4,pattern,q); q=z;
//		*(data + slen + 1)=BarCode93_Unset[ck];
//		*(data + slen + 2)=NULL;
	}
*/
	if (chksum) {
		id=ck;
		cp=(ROMDATA char *)BarCode93_Pattern[id];
		z=BarMakePattern(cp,6,4,pattern,q); q=z;

		id=ck_k;
		cp=(ROMDATA char *)BarCode93_Pattern[id];
		z=BarMakePattern(cp,6,4,pattern,q); q=z;
	}
	/* Stop Character */
	id=START_STOP;
	cp=(ROMDATA char *)BarCode93_Pattern[id];

	z=BarMakePattern(cp,6,4,pattern,q); q=z;
	/* Terminate Bar */
	cp=(ROMDATA char *)BarCode93_Pattern[48];
	z=BarMakePattern(cp,1,4,pattern,q); q=z;
	BarStruct.p2=q;
        return q;
}

#ifdef USE_CODE93FULL
short BarCode93e(HUGEDATA char *data,char chksum,HUGEDATA INT8U *pattern)
{
	short q,i,j,slen;
	char c,tdata[30];

	if(BarCode93e_Verify(data)) return -1;
	
	j = 0;
	slen=strlen(data);
	for (i = 0; i <slen; i++) {
		c = data[i];
		if (c>=' ' && c<='Z') {	//0x20 ~ 0x5a
			if(BarCode93_Set[c-' '] != 0 || c == '0') { //0x20 0x30
				tdata[j] = BarCode93_Set[c-' ']; //0x20
				j++;
			} else {
		   		switch(BarCode93_Shift_Set[c]) {
        	     	case '$':
            	    	tdata[j] = EXTEND_DOLLAR;
                		break;
	             	case '%':
    	            	tdata[j] = EXTEND_PERCENT;
        	        	break;
            	 	case '/':
                 		tdata[j] = EXTEND_SLASH;
	                	break;
    	         	case '+':
        	        	tdata[j] = EXTEND_PLUS;
            	    	break;
	             	default:
        	        	break;
	        	}
				j++;
				tdata[j] = BarCode93_Set[BarCode93_Shift_Set2[c]-' ']; //0x20
				j++;
			}
		} else if (c>='[' && c<= '~') { //0x5b ~ 0x7e
			if (c == BAR_2CH_SET_CH) {
				i++;
				if (i >= slen ) return -1;
				c = data[i];
				if (c>='@' && c<='_') {  //0x40 ~ 0x5f
					c = c - '@'; //0x40
				} else if (c == '0') { //0x30
					c = BAR_2CH_SET_CH;
				} else if (c == '5') { //0x35
					c = DEL;  //0x7f
				} else {
					return -1;
				}
			}
		   	switch(BarCode93_Shift_Set[c]) {
        	     case '$':
            	    tdata[j] = EXTEND_DOLLAR;
                	break;
	             case '%':
    	            tdata[j] = EXTEND_PERCENT;
        	        break;
            	 case '/':
                 	tdata[j] = EXTEND_SLASH;
	                break;
    	         case '+':
        	        tdata[j] = EXTEND_PLUS;
            	    break;
	             default:
        	        break;
	        }
			j++;
			tdata[j] =BarCode93_Set[BarCode93_Shift_Set2[c]-' ']; //0x20
			j++;
		} else {
			return -1;
		}			
	}
	q = BarCode93t(tdata,chksum,pattern,j);
    return q;
}
#endif//#ifdef USE_CODE93FULL

short BarCode93(HUGEDATA char *data,char chksum,HUGEDATA INT8U *pattern)
{
	short q,i,j,slen;
	char c,tdata[MAX_BAR_DATA_BUF_SIZE];

	if(BarCode93_Verify(data)) return -1;

	j = 0;
	slen=strlen(data);
	for (i = 0; i <slen; i++) {
		c = data[i];
		if (c>=' ' && c<='Z') {   //0x20 ~ 0x5a
			if(BarCode93_Set[c-' '] != 0 || c == '0') { //0x20 0x30
				tdata[j] = BarCode93_Set[c-' ']; //0x20
				j++;
			} else {
				return -1;
			}
		}			
	}
	q = BarCode93t(tdata,chksum,pattern,j);
    return q;
}
#endif//#ifdef USE_CODE93


