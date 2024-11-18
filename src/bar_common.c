/*****************************************************************************
|*			
|*  Copyright		:	(c) 2002 CAS
|*  Filename		:	bar_common.c
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
#include "prt_common.h"
#include "prtdrv.h"
#include "bar_common.h"
#include "prt_font.h"
#include "bar_common.h"
#include "prt_render.h"
#include "barcode_file.h"
#ifdef USE_QR_BARCODE //20140912
#include "zint.h" //20170421
#include "qr.h"
#include "initial.h"
#endif
#ifdef USE_QR_SIZE_SETTING
#include "prt_app.h"
#endif
#ifdef USE_LOGISTICS_BARCODE
#include "initial.h"
#endif

//HUGEDATA BAR_STRUCT BarStruct;
NEARDATA BAR_STRUCT BarStruct;//HYP 20060412 _huge --> _near for small code size

HUGEDATA BAR_FIELD_STRUCT BarFieldStruct[MAX_BAR_FIELD_NUM];
#ifdef USE_LOGISTICS_BARCODE
static void Bar_str_adjust(char *hri_str, char *bar_str, char *bar_form);
#endif

_inline void Inline_BarReceiptJustification(short r) {
	if(PrtStruct.Mode == RECEIPT) {
		PrtStruct.y = 0;
		//HYP 20040723
		if((PrtStruct.PrintWidth - r - 2*BarStruct.QuietZone) < 0) {
			PrtStruct.x = 0;
			return;
		}

		switch(StrStruct.Justification) {
			default:
			case CENTER:
				PrtStruct.x = (PrtStruct.PrintWidth - r - 2*BarStruct.QuietZone)>>1;
				break;
			case LEFT:
				PrtStruct.x = 0;
				break;
			case RIGHT:
				PrtStruct.x = PrtStruct.PrintWidth - r - 2*BarStruct.QuietZone - 1;
				break;
		}
	}
}
#ifdef USE_RSS14
short PrtBarRSSPattern(INT8U barType, HUGEDATA INT8U* pattern)//mode RSS14, RSS14S, RSS14SO and etc.
{
	HUGEDATA char *cp;
	short orgFullHeight;
	short orgCharHeight;
	short orgX;
	short orgY;
	INT8U rssStackHeight;
	short	i,r;

	switch(barType){
		case RSS14S:
			for (i = 0; i < MAX_BAR_PATTERN_SIZE; i++) pattern[i]=0;  
			cp = (HUGEDATA char *)&upperPattern[0];			    		
			r = BarRSS14MakePattern(cp, 25, 0, pattern, 0);

			orgFullHeight = BarStruct.FullHeight;
			orgCharHeight = BarStruct.CharHeight;
			orgX = PrtStruct.x;

			orgY = PrtStruct.y;


			if (orgCharHeight) BarStruct.FullHeight -= BarStruct.CharHeight;
			BarStruct.FullHeight = (BarStruct.FullHeight*5)/12 - (BarStruct.nb*1)/2;// uppper bar height : lower bar height = 5 : 7

			BarStruct.CharHeight = 0;
			rssStackHeight = 0;
			rssStackHeight = BarStruct.FullHeight;

			Inline_BarReceiptJustification(r);
			
			if (PrtStruct.Rot == DEG_180) PrtStruct.y += orgFullHeight - BarStruct.FullHeight;
			else if (PrtStruct.Rot == DEG_90) PrtStruct.x += orgFullHeight - BarStruct.FullHeight;
			
			PrtBarPattern(pattern,r);
			
			cp = (HUGEDATA char *)&sepPattern[0];

			if (PrtStruct.Rot == DEG_180) PrtStruct.y -= BarStruct.nb*1;
			else if (PrtStruct.Rot == DEG_0) PrtStruct.y += BarStruct.FullHeight;
			else if (PrtStruct.Rot == DEG_90) PrtStruct.x -= BarStruct.nb*1;
			else if (PrtStruct.Rot == DEG_270) PrtStruct.x += BarStruct.FullHeight;
			BarStruct.FullHeight = BarStruct.nb*1;
			rssStackHeight += BarStruct.FullHeight;
								
			for ( i = 0; i < MAX_BAR_PATTERN_SIZE ; i++) pattern[i]=0;  
			r = BarRSS14MakePattern(cp, seperatorLen, 0, pattern, 0);
			Inline_BarReceiptJustification(r);
			PrtBarPattern(pattern,r);
		
			if (PrtStruct.Rot == DEG_180) PrtStruct.y -= (orgFullHeight - rssStackHeight);
			else if (PrtStruct.Rot == DEG_0) PrtStruct.y += BarStruct.FullHeight;
			else if (PrtStruct.Rot == DEG_90) PrtStruct.x -= (orgFullHeight - rssStackHeight);
			else if (PrtStruct.Rot == DEG_270) PrtStruct.x += BarStruct.FullHeight;
			BarStruct.FullHeight = orgFullHeight - rssStackHeight;
			BarStruct.CharHeight = orgCharHeight;
			
			for ( i = 0; i < MAX_BAR_PATTERN_SIZE ; i++) pattern[i]=0;  //inintize

			cp = (HUGEDATA char *)&lowerPattern[0];
			r = BarRSS14MakePattern(cp, 25, 4, pattern, 0);
			Inline_BarReceiptJustification(r);
			PrtBarPattern(pattern,r);

			BarStruct.FullHeight = orgFullHeight;
			PrtStruct.x = orgX;
			PrtStruct.y = orgY;
			break;
		case RSS14SO: 
			for (i = 0; i < MAX_BAR_PATTERN_SIZE; i++) pattern[i]=0;  
			cp = (HUGEDATA char *)&upperPattern[0];
			r = BarRSS14MakePattern(cp, 25, 0, pattern, 0);
			orgFullHeight = BarStruct.FullHeight;
			orgCharHeight = BarStruct.CharHeight;
			orgX = PrtStruct.x;

			orgY = PrtStruct.y;


			if (orgCharHeight) BarStruct.FullHeight -= BarStruct.CharHeight;
			BarStruct.FullHeight = (BarStruct.FullHeight)/2 - (BarStruct.nb*1*3)/2 ; 

			BarStruct.CharHeight = 0;
			rssStackHeight = 0;
			rssStackHeight = BarStruct.FullHeight;
		
			Inline_BarReceiptJustification(r);
			if (PrtStruct.Rot == DEG_180) PrtStruct.y += orgFullHeight - BarStruct.FullHeight;
			else if (PrtStruct.Rot == DEG_90) PrtStruct.x += orgFullHeight - BarStruct.FullHeight;

			PrtBarPattern(pattern,r);

			if (PrtStruct.Rot == DEG_180) PrtStruct.y -= BarStruct.nb*1;
			else if (PrtStruct.Rot == DEG_0) PrtStruct.y += BarStruct.FullHeight;
			else if (PrtStruct.Rot == DEG_90) PrtStruct.x -= BarStruct.nb*1;
			else if (PrtStruct.Rot == DEG_270) PrtStruct.x += BarStruct.FullHeight;
			BarStruct.FullHeight = BarStruct.nb*1;
			rssStackHeight += BarStruct.FullHeight;

			for ( i = 0; i < MAX_BAR_PATTERN_SIZE ; i++) pattern[i]=0;  
		
			cp = (HUGEDATA char *)&uppersepPattern[0];
			r = BarRSS14MakePattern(cp, upperseperatorLen, 0, pattern, 0);
		
			Inline_BarReceiptJustification(r);
			PrtBarPattern(pattern,r);

			if (PrtStruct.Rot == DEG_180) PrtStruct.y -= BarStruct.nb*1;
			else if (PrtStruct.Rot == DEG_0) PrtStruct.y += BarStruct.FullHeight;
			else if (PrtStruct.Rot == DEG_90) PrtStruct.x -= BarStruct.nb*1;
			else if (PrtStruct.Rot == DEG_270) PrtStruct.x += BarStruct.FullHeight;
			BarStruct.FullHeight = BarStruct.nb*1;
			rssStackHeight += BarStruct.FullHeight;

			for ( i = 0; i < MAX_BAR_PATTERN_SIZE ; i++) pattern[i]=0;  
		
			cp = (HUGEDATA char *)&sepPattern[0];
			r = BarRSS14MakePattern(cp, seperatorLen, 0, pattern, 0);
		
			Inline_BarReceiptJustification(r);
			PrtBarPattern(pattern,r);

			if (PrtStruct.Rot == DEG_180) PrtStruct.y -= BarStruct.nb*1;
			else if (PrtStruct.Rot == DEG_0) PrtStruct.y += BarStruct.FullHeight;
			else if (PrtStruct.Rot == DEG_90) PrtStruct.x -= BarStruct.nb*1;
			else if (PrtStruct.Rot == DEG_270) PrtStruct.x += BarStruct.FullHeight;
			BarStruct.FullHeight = BarStruct.nb*1;
			rssStackHeight += BarStruct.FullHeight;

			for ( i = 0; i < MAX_BAR_PATTERN_SIZE ; i++) pattern[i]=0;  
		
			cp = (HUGEDATA char *)&lowersepPattern[0];
			r = BarRSS14MakePattern(cp, lowerseperatorLen, 0, pattern, 0);
		
			Inline_BarReceiptJustification(r);
			PrtBarPattern(pattern,r);

			if (PrtStruct.Rot == DEG_180) PrtStruct.y -= (orgFullHeight - rssStackHeight);
			else if (PrtStruct.Rot == DEG_0) PrtStruct.y += BarStruct.FullHeight;
			else if (PrtStruct.Rot == DEG_90) PrtStruct.x -= (orgFullHeight - rssStackHeight);
			else if (PrtStruct.Rot == DEG_270) PrtStruct.x += BarStruct.FullHeight;
			BarStruct.FullHeight = orgFullHeight - rssStackHeight;
			BarStruct.CharHeight = orgCharHeight;

			for ( i = 0; i < MAX_BAR_PATTERN_SIZE ; i++) pattern[i]=0;  
			
			cp = (HUGEDATA char *)&lowerPattern[0];
			r = BarRSS14MakePattern(cp, 25, 4, pattern, 0);
			
			Inline_BarReceiptJustification(r);
			PrtBarPattern(pattern,r);

			BarStruct.FullHeight = orgFullHeight;
			PrtStruct.x = orgX;
			PrtStruct.y = orgY; 
			break;
		default: 
			break;
	}
	return r;
}
#endif//#ifdef USE_RSS14

#ifdef USE_RSSEXPAND
#ifdef USE_LOGISTICS_BARCODE
short PrtBarRSSExpPattern(INT8U barType, HUGEDATA INT8U* pattern, INT8U lineNo, INT8U charline)//mode RSS_EXPANDED, RSS_EXPANDED_STACK
{
	HUGEDATA char *cp;
	short orgFullHeight;
	short tmpFullHeight;
	short orgCharHeight;
	short orgX;
	short orgY;
	INT8U rssStackHeight;
	short	i,r;
	short max_r;
	INT8U j;
	short rssExpStackRowHeight;
	INT8U startType;

	if(BarStruct.BarWidthMag == 1)BarStruct.CharHeight *=charline/18+1;
	if(BarStruct.BarWidthMag == 2)BarStruct.CharHeight *=charline/38+1;
	if(BarStruct.BarWidthMag == 3)BarStruct.CharHeight *=charline/57+1;
	if(BarStruct.BarWidthMag == 4)BarStruct.CharHeight *=charline/76+1;
#else
short PrtBarRSSExpPattern(INT8U barType,  INT8U* pattern, INT8U lineNo)//mode RSS_EXPANDED, RSS_EXPANDED_STACK
{
	HUGEDATA char *cp;
	short orgFullHeight;
	short tmpFullHeight;
	short orgCharHeight;
	short orgX;
	short orgY;
	INT8U rssStackHeight;
	short	i,r;
	short max_r;
	INT8U j;
	short rssExpStackRowHeight;
	INT8U startType;
#endif

	max_r = 0;
	orgFullHeight = BarStruct.FullHeight;
	orgCharHeight = BarStruct.CharHeight;
	orgX = PrtStruct.x;
	orgY = PrtStruct.y;

	switch(barType){
		case RSSEXP: 
		case RSSEXPS:
			//calcurate gs1 databar expanded stack barcode height
			tmpFullHeight = orgFullHeight;
			if (BarStruct.CharHeight) tmpFullHeight -= BarStruct.CharHeight;
			//[start] for test. we change number to the variable!!
			//rssExpStackRowHeight = (tmpFullHeight - BarStruct.nb*(2-1)*3)/2; //test row is 2
			//rssExpStackRowHeight = (tmpFullHeight - BarStruct.nb*(3-1)*3)/3; //test row is 3
			//rssExpStackRowHeight = (tmpFullHeight - BarStruct.nb*(5-1)*3)/5; //test row is 5
			rssExpStackRowHeight = (tmpFullHeight - BarStruct.nb*(lineNo-1)*3)/lineNo; //test row is 5
			//[end]

			rssStackHeight = rssExpStackRowHeight;
			BarStruct.FullHeight = rssExpStackRowHeight;
			BarStruct.CharHeight = 0;

			//[start] for test. we change number to the variable!!
			//for (j = 0; j < (2-1); j++) //test. row is 2
			//for (j = 0; j < (3-1); j++) //test. row is 3
			//for (j = 0; j < (5-1); j++) //test. row is 5
			for (j = 0; j < (lineNo-1); j++)
			//[end]
			{
				for (i = 0; i < MAX_BAR_PATTERN_SIZE; i++) pattern[i]=0;  
				cp = (HUGEDATA char *)&DataPattern[j];

				if (j % 2) startType = 4; //0: white, 4: black
				else startType = 0;

				r = BarRSS14MakePattern(cp, 225, startType, pattern, 0);
				max_r = r;  //1st barcode width is maximun size
				            

				if (j == 0) 
				{
					if (PrtStruct.Rot == DEG_180) PrtStruct.y += (orgFullHeight-rssExpStackRowHeight);
					else if (PrtStruct.Rot == DEG_90) PrtStruct.x += (orgFullHeight-rssExpStackRowHeight);
				}
				else //atfer 1st barcode (data barcode + upper + middle + lower)
				{
					BarStruct.FullHeight = rssExpStackRowHeight;
					rssStackHeight += BarStruct.FullHeight;
				}

				Inline_BarReceiptJustification(r);
				PrtBarPattern(pattern,r);

				if (PrtStruct.Rot == DEG_180) PrtStruct.y -= BarStruct.nb*1;
				else if (PrtStruct.Rot == DEG_0) PrtStruct.y += BarStruct.FullHeight;
				else if (PrtStruct.Rot == DEG_90) PrtStruct.x -= BarStruct.nb*1;
				else if (PrtStruct.Rot == DEG_270) PrtStruct.x += BarStruct.FullHeight;
				BarStruct.FullHeight = BarStruct.nb*1;
				rssStackHeight += BarStruct.FullHeight;

				for ( i = 0; i < MAX_BAR_PATTERN_SIZE ; i++) pattern[i]=0;  

				cp = (HUGEDATA char *)&upsegsPattern[j];
				r = BarRSS14MakePattern(cp, EXP_upperseperatorLen[j], 0, pattern, 0);

				Inline_BarReceiptJustification(r);
				PrtBarPattern(pattern,r);

				if (PrtStruct.Rot == DEG_180) PrtStruct.y -= BarStruct.nb*1;
				else if (PrtStruct.Rot == DEG_0) PrtStruct.y += BarStruct.nb*1;
				else if (PrtStruct.Rot == DEG_90) PrtStruct.x -= BarStruct.nb*1;
				else if (PrtStruct.Rot == DEG_270) PrtStruct.x += BarStruct.nb*1;                BarStruct.FullHeight = BarStruct.nb*1;
				rssStackHeight += BarStruct.FullHeight;

				for ( i = 0; i < MAX_BAR_PATTERN_SIZE ; i++) pattern[i]=0;  

				cp = (HUGEDATA char *)&midsegsPattern[j];
				r = BarGS1ExpStackSepMakePattern(cp, EXP_midsepLen[j], 0, pattern, 0);//seperatorLen

				Inline_BarReceiptJustification(r);
				PrtBarPattern(pattern,r);

				if (PrtStruct.Rot == DEG_180) PrtStruct.y -= BarStruct.nb*1;
				else if (PrtStruct.Rot == DEG_0) PrtStruct.y += BarStruct.nb*1;
				else if (PrtStruct.Rot == DEG_90) PrtStruct.x -= BarStruct.nb*1;
				else if (PrtStruct.Rot == DEG_270) PrtStruct.x += BarStruct.nb*1;
				BarStruct.FullHeight = BarStruct.nb*1;
				rssStackHeight += BarStruct.FullHeight;

				for ( i = 0; i < MAX_BAR_PATTERN_SIZE ; i++) pattern[i]=0;  

				cp = (HUGEDATA char *)&lowsegsPattern[j];
				r = BarRSS14MakePattern(cp, EXP_lowerseperatorLen[j], 0, pattern, 0);
				if (max_r > r) r = max_r;//when barcode width less than 1st line, r replace max_r

				Inline_BarReceiptJustification(r);
				PrtBarPattern(pattern,r);

				if (PrtStruct.Rot == DEG_180) PrtStruct.y -= rssExpStackRowHeight;//(orgFullHeight - rssStackHeight);
				else if (PrtStruct.Rot == DEG_0) PrtStruct.y += BarStruct.FullHeight;
				else if (PrtStruct.Rot == DEG_90) PrtStruct.x -= rssExpStackRowHeight;//(orgFullHeight - rssStackHeight);
				else if (PrtStruct.Rot == DEG_270) PrtStruct.x += BarStruct.FullHeight;

				//[start] for test. we change number to the variable!!
				//if (j == 0) //last, row is 1                
				//if (j == 0) //last, row is 2                
				//if (j == 1) //last, row is 3                
				//if (j == 3) //last, row is 5
				if (j == lineNo - 2 || (j == 0 && lineNo == 1)) //last
				//[end]
				{
					if (orgCharHeight) 
					{
						if (PrtStruct.Rot == DEG_180) PrtStruct.y -= orgCharHeight;
						else if (PrtStruct.Rot == DEG_90) PrtStruct.x -= orgCharHeight;

						rssExpStackRowHeight += orgCharHeight;                 
						BarStruct.FullHeight = rssExpStackRowHeight;
						BarStruct.CharHeight = orgCharHeight;
					}
					else 
					{
						BarStruct.FullHeight = rssExpStackRowHeight;
					}

					for ( i = 0; i < MAX_BAR_PATTERN_SIZE ; i++) pattern[i]=0;  

					cp = (HUGEDATA char *)&DataPattern[j+1];

					if ((j+1) % 2) startType = 4;
					else startType = 0;

					r = BarRSS14MakePattern(cp, 225, startType, pattern, 0);
					if (max_r > r) r = max_r;//when barcode width less than 1st line, r replace max_r

					Inline_BarReceiptJustification(r);
					PrtBarPattern(pattern,r);
				}
			}
			BarStruct.FullHeight = orgFullHeight;
			PrtStruct.x = orgX;
			PrtStruct.y = orgY; 
			break;
		default: 
			break;
	}
	return r;
}
#endif//#ifdef USE_RSSEXPAND

ROMDATA char Bar_BW_Pattern_Bit[]={0x00,0x00,0x00,0x00,0xff,0xff,0xff,0xff};
#if defined(USE_RSS_BARCODE) || defined(USE_ONLY_RSS_EXPAND_BARCODE)
ROMDATA char Bar_BW_Pattern_Bit_RSS[]={	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
						0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};
#endif//#if defined(USE_RSS_BARCODE) || defined(USE_ONLY_RSS_EXPAND_BARCODE)

void BarMembcpy(HUGEDATA INT8U *dest,short pos,ROMDATA char *source,short sz)
{
	short i,dwpos,swpos;
	INT8U smask,dmask;

	swpos=0; smask=0x80;
	/**********************************************************
	 * 용지 경계에서는 처리하지 않음
	 **********************************************************/
	if (pos>=BarStruct.MaxWidth) return;
	dmask=0x80>>(pos%8); if (dmask==0) dmask=0x80;
	for (i=pos; i<pos+sz; i++) {
		dwpos=i/8;
		if (i>=BarStruct.MaxWidth) break;
		if (source[swpos]&smask) {
			dest[dwpos] |= dmask;

		}
		dmask>>=1; if (dmask==0) { dmask=0x80; dwpos++; }
		smask>>=1; if (smask==0) { smask=0x80; swpos++; }
	}
}

short BarMembcpyNS(HUGEDATA INT8U *dest,short pos,ROMDATA char *source,char nb,char wb,short sz)
/******************************************************************
   1 bit 의 "0" 은 N, "1" 은 W 
   처음부터 홀수는 Black 짝수는 Space로 표현한다.
 ******************************************************************/
{
	short i,dwpos,swpos,k,w;
	INT8U smask,dmask,eo;

	swpos=0; smask=0x80;
	if (pos>=BarStruct.MaxWidth) return pos;
	k=pos;
	dmask=0x80>>(pos%8); if (dmask==0) dmask=0x80;
	eo=1;
	for (i=pos; i<pos+sz; i++) {
		if (i>=BarStruct.MaxWidth) break;
		if (source[swpos]&smask) {
		    for (w=0; w<wb; w++) {
			if (eo) {
				dwpos=k/8;
				dest[dwpos] |= dmask;
			}
			dmask>>=1; if (dmask==0) dmask=0x80;
			k++;
		    }
		} else {
		    for (w=0; w<nb; w++) {
			if (eo) {
				dwpos=k/8;
				dest[dwpos] |= dmask;
			}
			dmask>>=1; if (dmask==0) dmask=0x80;
			k++;
		    }
		}
		if (eo) eo=0; else eo=1;
		smask>>=1; if (smask==0) { smask=0x80; swpos++; }
	}
	return k;
}

short BarMakePattern(ROMDATA char *spattern,short sleng,char startbit,HUGEDATA INT8U *pattern,short pos)
/*************************************************************************************
 EAN,UPC,CODE93 등에서 사용함
 *************************************************************************************/
{
	short q,i,ibit,sz;
	char  isz[4];
//	char string_buf[30];

	q=pos;
	ibit=(short)startbit;
	isz[0]=BarStruct.nb;
	isz[1]=BarStruct.ns;
	isz[2]=BarStruct.wb;
	isz[3]=BarStruct.ws;
	if (ibit) ibit=4;
	for (i=0; i<sleng; i++) {
	    sz=isz[spattern[i]-'0'-1];
	    BarMembcpy(pattern,q, (ROMDATA char *)&Bar_BW_Pattern_Bit[ibit],sz); q+=sz;
	    if (ibit) {	ibit=0; } else { ibit=4; }
	}
	return q;
}

#if defined(USE_RSS_BARCODE) || defined(USE_ONLY_RSS_EXPAND_BARCODE)
int errFlag = FALSE;
/**********************************************************************
* getRSSwidths
* routine to generate widths for RSS elements for a given value.
* Calling arguments:
* val = required value
*	n = number of modules
* elements = elements in set (RSS-14 & Expanded = 4; RSS-14 Limited = 7)
*	maxWidth = maximum module width of an element
*	noNarrow = 0 will skip patterns without a narrow element
* Return:
* int widths[] = element widths
************************************************************************/
#define MAX_K	14

int *getRSSwidths(int val, int n, int elements, int maxWidth, int noNarrow)
{
	static int widths[MAX_K];
	int bar;
	int elmWidth;
	int i;
	int mxwElement;
	int subVal, lessVal;
	int narrowMask = 0;

	for (bar = 0; bar < elements-1; bar++)
	{
		for (elmWidth = 1, narrowMask |= (1<<bar);
				 ;
				 elmWidth++, narrowMask &= ~(1<<bar))
		{
			/* get all combinations */
			subVal = combins(n-elmWidth-1, elements-bar-2);
			/* less combinations with no narrow */
			if ((!noNarrow) && (narrowMask == 0) &&
					 (n-elmWidth-(elements-bar-1) >= elements-bar-1))
			{
				subVal -= combins(n-elmWidth-(elements-bar), elements-bar-2);
			}
			/* less combinations with elements > maxVal */
			if (elements-bar-1 > 1)
			{
				lessVal = 0;
				for (mxwElement = n-elmWidth-(elements-bar-2);
						 mxwElement > maxWidth;
						 mxwElement--)
				{
					lessVal += combins(n-elmWidth-mxwElement-1, elements-bar-3);
				}
				subVal -= lessVal * (elements-1-bar);
			}
			else if (n-elmWidth > maxWidth)
			{
				subVal--;
			}
			val -= subVal;
			if (val < 0) break;
		}
		val += subVal;
		n -= elmWidth;
		widths[bar] = elmWidth;
	}
	widths[bar] = n;
	return(widths);
}

/* combins(n,r): returns the number of Combinations of r selected from n:
*		Combinations = n! /( n-r! * r!) */
int combins(int n, int r) 
{
	int i, j;
	int maxDenom, minDenom;
	int val;

	if (n-r > r) {
		minDenom = r;
		maxDenom = n-r;
	}
	else {
		minDenom = n-r;
		maxDenom = r;
	}
	val = 1;
	j = 1;
	for (i = n; i > maxDenom; i--) {
		val *= i;
		if (j <= minDenom) {
			val /= j;
			j++;
		}
	}
	for ( ; j <= minDenom; j++) {
		val /= j;
	}
	return(val);
}

short BarRSS14MakePattern(HUGEDATA char *spattern, short sleng, char startbit, HUGEDATA INT8U *pattern, short pos)
/*************************************************************************************
 RSS14 등에서 사용함
 *************************************************************************************/
{
	short q,i,ibit,sz;

	q = pos;
	ibit = (short)startbit;
	
	if (ibit) ibit=16;
	for (i=0; i<sleng; i++) {
	    sz = spattern[i]*BarStruct.nb;

	    BarMembcpy(pattern, q, (ROMDATA char *)&Bar_BW_Pattern_Bit_RSS[ibit], sz); q += sz;

	    if (ibit) ibit=0;
	    else ibit=16;
	}
	return q;
}
short BarGS1ExpStackSepMakePattern(HUGEDATA char *spattern, short sleng, char startbit, HUGEDATA INT8U *pattern, short pos)
/*************************************************************************************
GS1 databar Expand Stack : middle seperator 
 *************************************************************************************/
{
	short q,i,ibit,sz;

	q = pos;
	ibit = (short)startbit;

	if (ibit) ibit=16;

	for (i=0; i<sleng; i++) {
		sz = spattern[i]*BarStruct.nb;        

		if ( (i >= 0 && i <= 3) || ( (i>= (sleng-4) && i <= (sleng -1)) ) )
		{
			BarMembcpy(pattern, q, (ROMDATA char *)&Bar_BW_Pattern_Bit_RSS[startbit], sz); q += sz;          
		}
		else
		{
			BarMembcpy(pattern, q, (ROMDATA char *)&Bar_BW_Pattern_Bit_RSS[ibit], sz); q += sz;
		}

		if (ibit) ibit=0;
		else ibit=16;
	}
	return q;
}
#endif//#if defined(USE_RSS_BARCODE) || defined(USE_ONLY_RSS_EXPAND_BARCODE)

void BarFieldInit(void)
{
	INT8U i;

	for(i=0; i < MAX_BAR_FIELD_NUM; i++) {
		BarFieldStruct[i].ExistField = 0;
	}
	PrtStruct.CurrentBarFieldNum = 0;//HYP 20060427
}

void BarInit(void)
{

	BarStruct.FullHeight=0;
	BarStruct.CharHeight=0;
	//HYP 20040127
	BarStruct.BarHeight=0;

	BarSetMaxWidth(MAX_BAR_WIDTH*8);

	BarSetFullHeight(PrtFlashStruct.FullHeight);
	BarSetBarHeight(PrtFlashStruct.BarHeight);
	BarSetCharHeight(PrtFlashStruct.CharHeight);
	BarSetBarType(PrtFlashStruct.BarType);
	BarSetWidthMag(PrtFlashStruct.BarWidthMag);
	BarSetGuard(PrtFlashStruct.Guard);
	BarStruct.BearerBarWidth = PrtFlashStruct.BearerBarWidth;
	BarStruct.CheckType = PrtFlashStruct.CheckType;
	BarStruct.PrtHri = PrtFlashStruct.PrtHri;
	BarStruct.UseOcrb = PrtFlashStruct.UseOcrb;
	BarStruct.BarHriPos = PrtFlashStruct.BarHriPos;
	BarStruct.PrtCheckHri = PrtFlashStruct.PrtCheckHri;
	BarStruct.PrtStartStopHri = PrtFlashStruct.PrtStartStopHri;
	BarFieldInit();
}
#ifdef USE_CONTINUOUS_LABEL_WITH_INGREDIENT
extern short ingredient_height_offset;
extern short ingredient_y_pos;
extern INT8U ingredient_height_adjust_flag;
#endif
void BarSaveField(INT8U field_num) {
	FontSaveField(MAX_STR_FIELD_NUM+field_num);

	BarFieldStruct[field_num].bx = PrtStruct.x;
	BarFieldStruct[field_num].by = PrtStruct.y;	
#ifdef 	USE_CONTINUOUS_LABEL_WITH_INGREDIENT
//	if(ingredient_height_adjust_flag)
//	{
//		if(PrtStruct.y>ingredient_y_pos)	BarFieldStruct[field_num].by += ingredient_height_offset;
//	}
#endif
	BarFieldStruct[field_num].BarRot = PrtStruct.Rot;

	BarFieldStruct[field_num].FullHeight = BarStruct.FullHeight;
	BarFieldStruct[field_num].BarHeight = BarStruct.BarHeight;
	BarFieldStruct[field_num].CharHeight = BarStruct.CharHeight;

	BarFieldStruct[field_num].BarType = BarStruct.BarType;
	BarFieldStruct[field_num].BarWidthMag = BarStruct.BarWidthMag;

	BarFieldStruct[field_num].Guard = BarStruct.Guard;
	BarFieldStruct[field_num].BearerBarWidth = BarStruct.BearerBarWidth;
	BarFieldStruct[field_num].CheckType = BarStruct.CheckType;
	BarFieldStruct[field_num].PrtHri = BarStruct.PrtHri;
	BarFieldStruct[field_num].UseOcrb = BarStruct.UseOcrb;
	BarFieldStruct[field_num].BarHriPos = BarStruct.BarHriPos;
	BarFieldStruct[field_num].PrtCheckHri = BarStruct.PrtCheckHri;
	BarFieldStruct[field_num].PrtStartStopHri = BarStruct.PrtStartStopHri;

	BarFieldStruct[field_num].ExistField = 1;

}

INT8U BarLoadField(INT8U field_num) {
	if(FontLoadField(MAX_STR_FIELD_NUM+field_num) == 0xff) return 0xff;
	if(BarFieldStruct[field_num].ExistField == 0) return 0xff;

	PrtStruct.CurrentBarFieldNum = field_num;//HYP 20060427

	PrtStruct.x = BarFieldStruct[field_num].bx;
	PrtStruct.y = BarFieldStruct[field_num].by;
	PrtStruct.Rot = BarFieldStruct[field_num].BarRot;

	BarStruct.FullHeight = BarFieldStruct[field_num].FullHeight;
	BarStruct.BarHeight = BarFieldStruct[field_num].BarHeight;
	BarStruct.CharHeight = BarFieldStruct[field_num].CharHeight;

	BarSetBarType(BarFieldStruct[field_num].BarType);
	BarSetWidthMag(BarFieldStruct[field_num].BarWidthMag);
	BarSetGuard(BarFieldStruct[field_num].Guard);
	BarStruct.BearerBarWidth = BarFieldStruct[field_num].BearerBarWidth;
	BarStruct.CheckType = BarFieldStruct[field_num].CheckType;
	BarStruct.PrtHri = BarFieldStruct[field_num].PrtHri;
	BarStruct.UseOcrb = BarFieldStruct[field_num].UseOcrb;
	BarStruct.BarHriPos = BarFieldStruct[field_num].BarHriPos;
	BarStruct.PrtCheckHri = BarFieldStruct[field_num].PrtCheckHri;
	BarStruct.PrtStartStopHri = BarFieldStruct[field_num].PrtStartStopHri;

	return 0;
}


ROMDATA	INT8U BarEanWMag[5][4] = {{0,0,0,0}, 	//not use
							{1,2,3,4}, {2,4,6,8}, {3,6,9,12}, {4,8,12,16}}; //1:2
ROMDATA	INT8U BarITFWMag[12][2] = {{0,0}, 		//not use
							{1,2}, {2,4}, {3,6}, {4,8}, 	 	//1:2
							{1,3}, {2,6}, {3,9}, {4,12},	//1:3
							{2,5}, {3,8}, {4,10}};			//1:2.5

void BarSetWidthMag(INT8U wmag) {
	BarStruct.BarWidthMag = wmag;
	if(wmag != 0) {  //user define width
		if(BarStruct.BarType > BAR_WIDTH_MAG_DIFF_MARGIN) {
			BarSetNW(BarITFWMag[wmag][0],BarITFWMag[wmag][1]);
		} else {
			BarSetNSWS(BarEanWMag[wmag][0],BarEanWMag[wmag][1],BarEanWMag[wmag][2],BarEanWMag[wmag][3]);
		}
	}
//	if(	BarStruct.QuietZone != 0) {
		BarStruct.QuietZone = 9*BarStruct.nb;
//	}
}

void BarSetBarType(INT8U bar_type) 
{
	BarStruct.BarType = bar_type;
	BarSetWidthMag(BarStruct.BarWidthMag);
	BarSetGuard(BarStruct.Guard);
	//HYP 20040127
	BarStruct.p0=0;
	BarStruct.p1=0;
	BarStruct.p2=0;
	BarStruct.p3=0;
	BarStruct.p4=0;
	BarStruct.p5=0;
	BarStruct.x1=0;
	BarStruct.x2=0;
}

void BarSetGuard(INT8U guard) {
	if(guard == NO_GUARD) {
		BarStruct.Guard = NO_GUARD;
		BarStruct.GuardType = NO_GUARD;
	} else {
		BarStruct.Guard = GUARD;
		switch(BarStruct.BarType) {
			case EAN13:
			case EAN8:
			case UPCA:
			case UPCE0:
			case UPCE1:
				BarStruct.GuardType = EAN13P0_GUARD;
				break;
			case EAN13P2:
			case EAN13P5:
			case EAN8P2:
			case EAN8P5:
			case UPCAP2:
			case UPCAP5:
			case UPCE0P2:
			case UPCE0P5:
				BarStruct.GuardType = EAN13PX_GUARD;
				break;
			default:
				//RSS14 serises don't need guard.
				BarStruct.GuardType = NO_GUARD;
				break;
		}
	}
}


void BarSetMaxWidth(short w)
{
	BarStruct.MaxWidth = w;
}

void BarSetNW(char nb,char wb)
{
	BarStruct.nb=nb;
	BarStruct.ns=nb;
	BarStruct.wb=wb;
	BarStruct.ws=wb;
}

void BarSetNSWS(char nb,char ns,char wb,char ws)
{
	BarStruct.nb=nb;
	BarStruct.ns=ns;
	BarStruct.wb=wb;
	BarStruct.ws=ws;
	BarStruct.HriGapEan = 2*BarStruct.nb;
}

void BarSetBarHeight(short h)
{
	BarStruct.BarHeight=h;
	if (BarStruct.FullHeight<h) {
		BarStruct.FullHeight=h;
		BarStruct.CharHeight=0;
	}
}

void BarSetCharHeight(short h)
{
	BarStruct.CharHeight=h;
	if (BarStruct.BarHeight<=BarStruct.CharHeight) {
		BarStruct.CharHeight=0;
		BarStruct.BarHeight =h;
	}
}

void BarSetFullHeight(short h)
{
	BarStruct.FullHeight=h;
	if (BarStruct.BarHeight>h) {
		BarStruct.BarHeight =h;
		BarStruct.CharHeight=0;
	}
	if (BarStruct.BarHeight==0) {
		BarStruct.BarHeight =h;
		BarStruct.CharHeight=0;
	}
}

//SG071012
extern INT8U GlbBarcodeData_Field1[BARCODE_FORMAT_LEN+1];
void GetFinalBarcodeDataF1(char *barStr1,char *barStr2)
{

	INT8U len1;
	INT8U len2;
	INT8U k;

	if(PrtStruct.CurrentBarFieldNum != 1) return;
	len1 = strlen(barStr1);
	if( len1 > sizeof(GlbBarcodeData_Field1) ) len1 = sizeof(GlbBarcodeData_Field1);
	k =0;
	memcpy(&GlbBarcodeData_Field1[k], barStr1, len1);	//SG071012. TEST ONLY
	k += len1;
	GlbBarcodeData_Field1[k]=0;
	if(barStr2)
	{
		len2 = strlen(barStr2);
		if( len2 > sizeof(GlbBarcodeData_Field1) - len1 ) len2 = sizeof(GlbBarcodeData_Field1) - len1;
		memcpy(&GlbBarcodeData_Field1[k], barStr2, len2);	
		k += len2;
		GlbBarcodeData_Field1[k]=0;
	} 
}
#ifdef USE_RSS_DEBUG
extern RSS_BARCODE_TEST_STRUCT rssBarStr;//gm 100115 check for rss barocde
#endif

#ifdef USE_LOGISTICS_BARCODE
void Bar_str_adjust(char *hri_str, char *bar_str, char *bar_form)
{
	char bar_temp[MAX_BAR_DATA_BUF_SIZE];
	char hri_temp[MAX_BAR_DATA_BUF_SIZE];	
	char bar_pos, cnt, tmp_cnt, hri_cnt;	
	INT8U use_idbracket;
	use_idbracket = get_global_bparam(GLOBAL_SALE_SETUP_FLAG19);
	use_idbracket = (use_idbracket>>1)& 0x01; 	//param 526, barcode Use ID bracket on/off

	bar_pos = strlen(bar_form);
	tmp_cnt = 0;
	hri_cnt = 0;
	for(cnt=0;cnt<bar_pos;cnt++)
	{
		if(bar_form[cnt] == '#')
		{
			bar_temp[tmp_cnt++] = bar_form[cnt];
		}
		else if(bar_form[cnt] == '(' || bar_form[cnt] == ')')
		{
			if(use_idbracket)hri_temp[hri_cnt++] = bar_form[cnt];
		}
		else
		{
			bar_temp[tmp_cnt++] = bar_form[cnt];
			hri_temp[hri_cnt++] = bar_form[cnt];			
		}
	}
	bar_temp[tmp_cnt]= 0;
	hri_temp[hri_cnt]= 0;	
	strcpy(bar_str, bar_temp);
	strcpy(hri_str, hri_temp);	
}
#endif

#ifdef USE_KOR_FONT
extern INT16U convert_euckr_to_utf8(INT8U *src);
#endif
#ifdef USE_GB2312_CHN_FONT
extern INT16U convert_gb2312_to_utf8(INT8U *src);
#endif
void BarcodeOut(HUGEDATA char *BarString) {		
	char BarConvString1[MAX_BAR_DATA_BUF_SIZE];
	char BarConvString2[MAX_BAR_DATA_BUF_SIZE];
	INT8U pattern[MAX_BAR_PATTERN_SIZE];
	short	i,r;
#ifdef USE_RSSEXPAND
	INT8U lineNo;
#endif
#ifdef USE_LOGISTICS_BARCODE
	INT8U barline_no;
	barline_no = get_global_bparam(GLOBAL_SALE_SETUP_FLAG19);
	barline_no = (barline_no>>2)& 0x03;	//param 527, barcode line limit
#endif
//caution!!! you must erase bar pattern buffer first.
//because barcode render routine is OR func.
	if(*BarString == '\0') return;

#ifdef USE_RSS_DEBUG
sprintf(BarString, "%04ld%09ld\r\n", rssBarStr.high, rssBarStr.low);//gm 100115 check for rss barocde
MsgOut(BarString);
#endif
	for ( i = 0; i < MAX_BAR_PATTERN_SIZE ; i++) pattern[i]=0;  
	switch(BarStruct.BarType) {
#ifdef USE_EAN13
		case EAN13:
			if(BarStruct.CheckType == 0) {
				if(strlen(BarString) != MAX_CHAR_EAN13) return;
			} else {
				if(strlen(BarString) != MAX_CHAR_EAN13-1) return;
			}
			r=BarEan13(BarString,BarStruct.CheckType,pattern);
			Inline_BarReceiptJustification(r);
			PrtBarPattern(pattern,r);
			BarHriEan13(BarString);
			GetFinalBarcodeDataF1(BarString, NULL); 
			break;	
#endif//#ifdef USE_EAN13SUP5
#ifdef USE_EAN13P2_UPCAP2_P5_EAN8P2_P5				
		case EAN13P2:
			if(BarStruct.CheckType == 0) {
				if(strlen(BarString) != MAX_CHAR_EAN13P2) return;
			} else {
				if(strlen(BarString) != MAX_CHAR_EAN13P2-1) return;
			}
			BarEanHriStrConv(BarConvString1,BarConvString2,BarString);
			r=BarEan13Sup2(BarConvString1,BarConvString2,BarStruct.CheckType,pattern);
			Inline_BarReceiptJustification(r);
			PrtBarPattern(pattern,r);
			BarHriEan13A(BarConvString1,BarConvString2);
			GetFinalBarcodeDataF1(BarConvString1,BarConvString2); 
 			break;
#endif//#ifdef USE_EAN13P2_UPCAP2_P5_EAN8P2_P5
#ifdef USE_EAN13SUP5
		case EAN13P5:
			if(BarStruct.CheckType == 0) {
				if(strlen(BarString) != MAX_CHAR_EAN13P5) return;
			} else {
				if(strlen(BarString) != MAX_CHAR_EAN13P5-1) return;
			}
			BarEanHriStrConv(BarConvString1,BarConvString2,BarString);
			r=BarEan13Sup5(BarConvString1,BarConvString2,BarStruct.CheckType,pattern);
			Inline_BarReceiptJustification(r);
			PrtBarPattern(pattern,r);
			BarHriEan13A(BarConvString1,BarConvString2);
			GetFinalBarcodeDataF1(BarConvString1,BarConvString2); 
			break;
#endif//#ifdef USE_EAN13SUP5
#ifdef USE_EAN8
		case EAN8:
			if(BarStruct.CheckType == 0) {
				if(strlen(BarString) != MAX_CHAR_EAN8) return;
			} else {
				if(strlen(BarString) != MAX_CHAR_EAN8-1) return;
			}
			r=BarEan8(BarString,BarStruct.CheckType,pattern);
			Inline_BarReceiptJustification(r);
			PrtBarPattern(pattern,r);
			BarHriEan8(BarString);	   		
			GetFinalBarcodeDataF1(BarString, NULL); 
			break;
#endif//#ifdef USE_EAN8
#ifdef USE_EAN13P2_UPCAP2_P5_EAN8P2_P5
		case EAN8P2:
			if(BarStruct.CheckType == 0) {
				if(strlen(BarString) != MAX_CHAR_EAN8P2) return;
			} else {
				if(strlen(BarString) != MAX_CHAR_EAN8P2-1) return;
			}
			BarEanHriStrConv(BarConvString1,BarConvString2,BarString);
			r=BarEan8Sup2(BarConvString1,BarConvString2,BarStruct.CheckType,pattern);	
			Inline_BarReceiptJustification(r);
			PrtBarPattern(pattern,r);
			BarHriEan8A(BarConvString1,BarConvString2);   
			GetFinalBarcodeDataF1(BarConvString1,BarConvString2); 
			break;
		case EAN8P5:
			if(BarStruct.CheckType == 0) {
				if(strlen(BarString) != MAX_CHAR_EAN8P5) return;
			} else {
				if(strlen(BarString) != MAX_CHAR_EAN8P5-1) return;
			}
			BarEanHriStrConv(BarConvString1,BarConvString2,BarString);
			r=BarEan8Sup5(BarConvString1,BarConvString2,BarStruct.CheckType,pattern);	
			Inline_BarReceiptJustification(r);
			PrtBarPattern(pattern,r);
			BarHriEan8A(BarConvString1,BarConvString2);   
			GetFinalBarcodeDataF1(BarConvString1,BarConvString2); 
			break;
#endif//#ifdef USE_EAN13P2_UPCAP2_P5_EAN8P2_P5
#ifdef USE_UPCA
		case UPCA:
			if(BarStruct.CheckType == 0) {
				if(strlen(BarString) != MAX_CHAR_UPCA) return;
			} else {
				if(strlen(BarString) != MAX_CHAR_UPCA-1) return;
			}
			r=BarUpca(BarString,BarStruct.CheckType,pattern);
			Inline_BarReceiptJustification(r);
			PrtBarPattern(pattern,r);
			BarHriUpca(BarString);
			GetFinalBarcodeDataF1(BarString,NULL); 
			break;
#endif//#ifdef USE_UPCA
#ifdef USE_EAN13P2_UPCAP2_P5_EAN8P2_P5
	    	case UPCAP2:
			if(BarStruct.CheckType == 0) {
				if(strlen(BarString) != MAX_CHAR_UPCAP2) return;
			} else {
				if(strlen(BarString) != MAX_CHAR_UPCAP2-1) return;
			}
			BarEanHriStrConv(BarConvString1,BarConvString2,BarString);
			r=BarUpcaSup2(BarConvString1,BarConvString2,BarStruct.CheckType,pattern);	
			Inline_BarReceiptJustification(r);
			PrtBarPattern(pattern,r);
			BarHriUpcaA(BarConvString1,BarConvString2);
			GetFinalBarcodeDataF1(BarConvString1,BarConvString2); 
			break;
		case UPCAP5:
			if(BarStruct.CheckType == 0) {
				if(strlen(BarString) != MAX_CHAR_UPCAP5) return;
			} else {
				if(strlen(BarString) != MAX_CHAR_UPCAP5-1) return;
			}
			BarEanHriStrConv(BarConvString1,BarConvString2,BarString);
			r=BarUpcaSup5(BarConvString1,BarConvString2,BarStruct.CheckType,pattern);
			Inline_BarReceiptJustification(r);
			PrtBarPattern(pattern,r);
			BarHriUpcaA(BarConvString1,BarConvString2);
			GetFinalBarcodeDataF1(BarConvString1,BarConvString2); 
			break;
#endif//#ifdef USE_EAN13P2_UPCAP2_P5_EAN8P2_P5
#ifdef USE_UPCE0_UPCE1
		case UPCE0:
			if(BarStruct.CheckType == 0) {
				if(strlen(BarString) != MAX_CHAR_UPCE0) return;
			} else {
				if(strlen(BarString) != MAX_CHAR_UPCE0-1) return;
			}
			r=BarUpce0(BarString,pattern);
			Inline_BarReceiptJustification(r);
			PrtBarPattern(pattern,r);
			BarHriUpce(BarString);	   
			GetFinalBarcodeDataF1(BarString,NULL); 
			break;
		case UPCE0P2:
			if(BarStruct.CheckType == 0) {
				if(strlen(BarString) != MAX_CHAR_UPCE0P2) return;
			} else {
				if(strlen(BarString) != MAX_CHAR_UPCE0P2-1) return;
			}
			BarEanHriStrConv(BarConvString1,BarConvString2,BarString);
			r=BarUpce0Sup2(BarConvString1,BarConvString2,pattern);	
			Inline_BarReceiptJustification(r);
			PrtBarPattern(pattern,r);
			BarHriUpceA(BarConvString1,BarConvString2);	
			GetFinalBarcodeDataF1(BarConvString1,BarConvString2); 
			break;
		case UPCE0P5:
			if(BarStruct.CheckType == 0) {
				if(strlen(BarString) != MAX_CHAR_UPCE0P5) return;
			} else {
				if(strlen(BarString) != MAX_CHAR_UPCE0P5-1) return;
			}
			BarEanHriStrConv(BarConvString1,BarConvString2,BarString);
			r=BarUpce0Sup5(BarConvString1,BarConvString2,pattern);
			Inline_BarReceiptJustification(r);
			PrtBarPattern(pattern,r);
			BarHriUpceA(BarConvString1,BarConvString2);   
			GetFinalBarcodeDataF1(BarConvString1,BarConvString2); 
			break;
		case UPCE1:
			if(BarStruct.CheckType == 0) {
				if(strlen(BarString) != MAX_CHAR_UPCE1) return;
			} else {
				if(strlen(BarString) != MAX_CHAR_UPCE1-1) return;
			}
			r=BarUpce1(BarString,pattern);	
			Inline_BarReceiptJustification(r);
			PrtBarPattern(pattern,r);
			BarHriUpce(BarString);	  
			GetFinalBarcodeDataF1(BarString,NULL); 
			break;
#endif//#ifdef USE_UPCE0_UPCE1
#ifdef USE_I2OF5
		case I2OF5: 
			r=BarI25(BarString,BarStruct.CheckType,pattern,BarConvString1);  //BarConvString1 = conv. data
			Inline_BarReceiptJustification(r);
			PrtBarPattern(pattern,r);
			BarHri(BarConvString1,r);
			GetFinalBarcodeDataF1(BarConvString1,NULL); 
			break;
#endif//#ifdef USE_I2OF5

#ifdef USE_CODE128ABX
		case CODE128A:  
  #ifdef USE_LOGISTICS_BARCODE
			Bar_str_adjust(BarConvString2, BarConvString1, BarString);
			r=BarCode128(BarConvString1,BarStruct.CheckType,pattern,CODE128_ATYPE,BarConvString1);
			Inline_BarReceiptJustification(r);
			PrtBarPattern(pattern,r);
			BarHri(BarConvString2,r);
			GetFinalBarcodeDataF1(BarConvString1,NULL);
  #else
			//BarHriCtrlStrConv(BarConvString1,BarString);		 
			r=BarCode128(BarString,BarStruct.CheckType,pattern,CODE128_ATYPE,BarConvString1);
			Inline_BarReceiptJustification(r);
			PrtBarPattern(pattern,r);
			BarHri(BarConvString1,r);
			GetFinalBarcodeDataF1(BarConvString1,NULL); 
  #endif			
			break;
		case CODE128B:  
  #ifdef USE_LOGISTICS_BARCODE
			Bar_str_adjust(BarConvString2, BarConvString1, BarString);
			r=BarCode128(BarConvString1,BarStruct.CheckType,pattern,CODE128_BTYPE,BarConvString1);
			Inline_BarReceiptJustification(r);
			PrtBarPattern(pattern,r);
			BarHri(BarConvString2,r);
			GetFinalBarcodeDataF1(BarConvString1,NULL);
  #else		
			//BarHriCtrlStrConv(BarConvString1,BarString);
			r=BarCode128(BarString,BarStruct.CheckType,pattern,CODE128_BTYPE,BarConvString1);
			Inline_BarReceiptJustification(r);
			PrtBarPattern(pattern,r);
			BarHri(BarConvString1,r);
			GetFinalBarcodeDataF1(BarConvString1,NULL); 
  #endif			
			break;
		case CODE128X:  
  #ifdef USE_LOGISTICS_BARCODE
			Bar_str_adjust(BarConvString2, BarConvString1, BarString);
			r=BarCode128(BarConvString1,BarStruct.CheckType,pattern,CODE128_XTYPE,BarConvString1);
			Inline_BarReceiptJustification(r);
			PrtBarPattern(pattern,r);
			BarHri(BarConvString2,r);
			GetFinalBarcodeDataF1(BarConvString1,NULL);
  #else		
			//BarHriCtrlStrConv(BarConvString1,BarString);
			r=BarCode128(BarString,BarStruct.CheckType,pattern,CODE128_XTYPE,BarConvString1); //notice!! BarData1 = conv data if typeC
			Inline_BarReceiptJustification(r);
			PrtBarPattern(pattern,r);
			BarHri(BarConvString1,r);
			GetFinalBarcodeDataF1(BarConvString1,NULL); 
  #endif			
			break;
#endif//#ifdef USE_CODE128ABX
#ifdef USE_CODE128C
		case CODE128C:  
			r=BarCode128(BarString,BarStruct.CheckType,pattern,CODE128_CTYPE,BarConvString1); //notice!! BarData1 = conv data if typeC
			Inline_BarReceiptJustification(r);
			PrtBarPattern(pattern,r);
			BarHri(BarConvString1,r);
			GetFinalBarcodeDataF1(BarConvString1,NULL); 
			break;
#endif//#ifdef USE_CODE128C
#ifdef USE_RSS14
		case RSS14:
			if (BarStruct.CheckType == 0) 
			{
				if (strlen(BarString) != MAX_CHAR_RSS14) return;
			} 
			else 
			{
				if (strlen(BarString) != MAX_CHAR_RSS14-1) return;
			}
			r = BarRSS14(BarString, BarStruct.CheckType, pattern);//for rss14 standard
			Inline_BarReceiptJustification(r);
			PrtBarPattern(pattern,r);
			BarHri(BarString,r);
			GetFinalBarcodeDataF1(BarString,NULL); 	
			break;
		case RSS14S:
			if (BarStruct.CheckType == 0) 
			{
				if (strlen(BarString) != MAX_CHAR_RSS14) return;
			} 
			else 
			{
				if (strlen(BarString) != MAX_CHAR_RSS14-1) return;
			}			
			BarRSS14S(BarString, BarStruct.CheckType);//for rss14 stack
			r = PrtBarRSSPattern(RSS14S, pattern);//mode RSS14, RSS14S, RSS14SO and etc.
			BarHri(BarString,r);
			GetFinalBarcodeDataF1(BarString,NULL); 
			break;
		case RSS14SO: 
			if (BarStruct.CheckType == 0) 
			{
				if (strlen(BarString) != MAX_CHAR_RSS14) return;
			} 
			else 
			{
				if (strlen(BarString) != MAX_CHAR_RSS14-1) return;
			}			
			BarRSS14SO(BarString, BarStruct.CheckType);//gm rss14 stack omni test 
			r = PrtBarRSSPattern(RSS14SO, pattern);//mode RSS14, RSS14S, RSS14SO and etc.
			BarHri(BarString,r);
			GetFinalBarcodeDataF1(BarString,NULL); 
			break;
#endif //#ifdef USE_RSS14
#ifdef USE_RSSLIMIT
		case RSSLIM:
			if (BarStruct.CheckType == 0) 
			{
				if (strlen(BarString) != MAX_CHAR_RSS14) return;
			} 
			else 
			{
				if (strlen(BarString) != MAX_CHAR_RSS14-1) return;
			}			
			r = BarRSSLimit(BarString, BarStruct.CheckType, pattern);
			Inline_BarReceiptJustification(r);
			PrtBarPattern(pattern,r);
			BarHri(BarString,r);
			GetFinalBarcodeDataF1(BarString,NULL); 
			break;
#endif //#ifdef USE_RSSLIMIT
#ifdef USE_RSSEXPAND
		case RSSEXP:  
//BarString[13] = 0; //check for rss barcode
 #ifdef USE_LOGISTICS_BARCODE
			switch( barline_no)
			{		
				case 1: // line limit 1(CODE128X)
					BarStruct.BarType = CODE128X;
					/*
					if(BarStruct.BarWidthMag ==2)
					{
						BarStruct.ns = 4;
						BarStruct.nb = 2;
						BarStruct.ws = 8;
						BarStruct.wb = 6;
					}
					if(BarStruct.BarWidthMag ==1)
					{
						BarStruct.ns = 2;
						BarStruct.nb = 1;
						BarStruct.ws = 4;
						BarStruct.wb = 3;
					}
					*/
					BarStruct.ns = 2*BarStruct.BarWidthMag;
					BarStruct.nb = 1*BarStruct.BarWidthMag;
					BarStruct.ws = 4*BarStruct.BarWidthMag;
					BarStruct.wb = 3*BarStruct.BarWidthMag;
					
					Bar_str_adjust(BarConvString2, BarConvString1, BarString);
					r=BarCode128(BarConvString1,BarStruct.CheckType,pattern,CODE128_XTYPE,BarConvString1);
					Inline_BarReceiptJustification(r);
					PrtBarPattern(pattern,r);
					BarHri(BarConvString2,r);
					GetFinalBarcodeDataF1(BarConvString1,NULL);
					break;
				case 2: // line limit 2 barcode size limit
				case 3: // line limit 3 barcode size limit				
					Bar_str_adjust(BarConvString2, BarConvString1, BarString);				
					BarRSSExp(BarConvString1, pattern, &lineNo); //KYK test RSSEXPS
					if(lineNo == 0) break;
					if(lineNo > barline_no) break;					
					r = PrtBarRSSExpPattern(RSSEXP, pattern, lineNo, strlen(BarConvString2));
					BarHri(BarConvString2,r);
					GetFinalBarcodeDataF1(BarString,NULL);
					break;
				default:
					Bar_str_adjust(BarConvString2, BarConvString1, BarString);
					BarRSSExp(BarConvString1, pattern, &lineNo); //KYK test RSSEXPS
					if(lineNo == 0) break;
					r = PrtBarRSSExpPattern(RSSEXP, pattern, lineNo, strlen(BarConvString2));
					BarHri(BarConvString2,r);
					GetFinalBarcodeDataF1(BarString,NULL);
					break;
			}
 #else
			BarRSSExp(BarString, pattern, &lineNo); //KYK test RSSEXPS
			if(lineNo == 0)
				break;
			r = PrtBarRSSExpPattern(RSSEXP, pattern, lineNo);
			BarHri(BarString,r);
			GetFinalBarcodeDataF1(BarString,NULL);
 #endif			
			break;
//		case RSSEXPS: //gm 100114 미완성 (진행중)
//sprintf(MsgBuf, "[RSS Expand Stack]%s\r\n", BarString);//???
//MsgOut(MsgBuf);
//			r = BarRSSExp(BarString, pattern); //gm RSS expand stack
//			Inline_BarReceiptJustification(r);
//			PrtBarPattern(pattern,r);
//			//BarHri(BarString,r);
//			GetFinalBarcodeDataF1(BarString,NULL); 
//			break;
#endif//#ifdef USE_RSSEXPAND
#ifdef USE_CODE39
		case CODE39: 
			r=BarCode39(BarString,BarStruct.CheckType,pattern);
			Inline_BarReceiptJustification(r);
			PrtBarPattern(pattern,r);
			BarHri(BarString,r);
			GetFinalBarcodeDataF1(BarString,NULL); 
			break;
#endif//#ifndef USE_CODE39
#ifdef USE_CODE39FULL
		case CODE39FULL: 
			BarHriCtrlStrConv(BarConvString1,BarString);
			r=BarCode39e(BarString,BarStruct.CheckType,pattern);
			Inline_BarReceiptJustification(r);
			PrtBarPattern(pattern,r);
			BarHri(BarConvString1,r);
			GetFinalBarcodeDataF1(BarConvString1,NULL); 
			break;
#endif//#ifdef USE_CODE39FULL
#ifdef USE_CODE93
		case CODE93:  
			r=BarCode93(BarString,BarStruct.CheckType,pattern);
			Inline_BarReceiptJustification(r);
			PrtBarPattern(pattern,r);
			BarHri(BarString,r);
			GetFinalBarcodeDataF1(BarString,NULL); 
			break;
#endif//#ifndef USE_CODE93
#ifdef USE_CODE93FULL
		case CODE93FULL: 
			BarHriCtrlStrConv(BarConvString1,BarString);
			r=BarCode93e(BarString,BarStruct.CheckType,pattern);
			Inline_BarReceiptJustification(r);
			PrtBarPattern(pattern,r);
			BarHri(BarConvString1,r);
			GetFinalBarcodeDataF1(BarConvString1,NULL); 
			break;
#endif//#ifdef USE_CODE93FULL
#ifdef USE_CODABAR
		case CODABAR: 
			r=BarCodabar(BarString,BarStruct.CheckType,pattern);
			Inline_BarReceiptJustification(r);
			PrtBarPattern(pattern,r);
			BarHri(BarString,r);
			GetFinalBarcodeDataF1(BarString,NULL); 
			break;
#endif//#ifdef USE_CODABAR
#ifdef USE_MSI_PLESSEY
		case MSI: 
			r=BarMsi(BarString,BarStruct.CheckType,pattern); // 10 (4)
			Inline_BarReceiptJustification(r);
			PrtBarPattern(pattern,r);
			BarHri(BarString,r);
			GetFinalBarcodeDataF1(BarString,NULL); 
			break;
		case PLESSEY: 
			r=BarPlessey(BarString,BarStruct.CheckType,pattern);
			Inline_BarReceiptJustification(r);
			PrtBarPattern(pattern,r);			   
			BarHri(BarString,r);
			GetFinalBarcodeDataF1(BarString,NULL); 
			break;
#endif//#ifdef USE_MSI_PLESSEY
#ifdef USE_S25_M25_IATA25
		case S2OF5: 
			r=BarS25(BarString,BarStruct.CheckType,pattern);	//Standard 2 of 5 = Industrial 2 of 5   
			Inline_BarReceiptJustification(r);
			PrtBarPattern(pattern,r);
			BarHri(BarString,r);
			GetFinalBarcodeDataF1(BarString,NULL); 
			break;
		case M2OF5:
			r=BarM25(BarString,BarStruct.CheckType,pattern);	//Standard 2 of 5 = Industrial 2 of 5   
			Inline_BarReceiptJustification(r);
			PrtBarPattern(pattern,r);
			BarHri(BarString,r);
			GetFinalBarcodeDataF1(BarString,NULL); 
			break;
		case IATA2OF5:
			r=BarIATA25(BarString,BarStruct.CheckType,pattern);	//Standard 2 of 5 = Industrial 2 of 5   
			Inline_BarReceiptJustification(r);
			PrtBarPattern(pattern,r);
			BarHri(BarString,r);
			GetFinalBarcodeDataF1(BarString,NULL); 
			break;
#endif//#ifdef USE_S25_M25_IATA25
#ifdef USE_CODE11
 		case CODE11: 
			r=BarCode11(BarString,BarStruct.CheckType,pattern);
			Inline_BarReceiptJustification(r);
			PrtBarPattern(pattern,r);
			BarHri(BarString,r);
			GetFinalBarcodeDataF1(BarString,NULL); 
			break;
#endif//#ifdef USE_CODE11
#ifdef USE_POSTNET
		case POSTNET: 
			r=BarPostnet(BarString,BarStruct.CheckType,pattern);
			Inline_BarReceiptJustification(r);
			PrtPostbarPattern(pattern,r);			   //??
			BarHri(BarString,r);
			GetFinalBarcodeDataF1(BarString,NULL); 
			break;
#endif//#ifdef USE_POSTNET
#ifdef USE_QR_BARCODE //20140902
		case QRCODE:
  #ifdef USE_GB2312_CHN_FONT
			convert_gb2312_to_utf8(BarString);
  #elif defined(USE_KOR_FONT)
			convert_euckr_to_utf8(BarString);
  #endif

  #ifdef USE_QR_SIZE_SETTING 
			if(QrAddTotalFlag == 1 && PrtPaperType == PRINT_MODE_TICKET)
				BarQR(BARCODE_QRCODE, BarString, 0, GetQRCodeSzie());
			else
  #endif		
				BarQR(BARCODE_QRCODE, BarString, 0, BarStruct.BarWidthMag);
			break;
#endif//#ifdef USE_QR_BARCODE
	}
}

#if defined(USE_RSS_BARCODE) || defined(USE_ONLY_RSS_EXPAND_BARCODE)
// copies pattern for separator adding 9 narrow elements inside each finder
INT16U cnvSeparator(INT8U data[], INT8U sepPattern[], INT8U startTpye, INT8U leng)
{
	int i, j, k;

	for (i = 0, k = 2; k <= 4; k += data[i], i++);
	//if ((prints->whtFirst && (i&1)==1) || (!prints->whtFirst && (i&1)==0)) {
	if ((startTpye && (i&1)==1) || (!startTpye && (i&1)==0)) {
		sepPattern[0] = 4;
		sepPattern[1] = k-4;
		j = 2;
	}
	else {
		sepPattern[0] = k;
		j = 1;
	}
	for ( ; i < leng; i++, j++) { //prints->elmCnt -> leng
		sepPattern[j] = data[i];
		if (data[i] +data[i+1] + data[i+2] == 13) {
			if ((j&1)==1) {
				// finder is light/dark/light
				for (k = 0; k < data[i]; k++) {
					sepPattern[j+k] = 1; // bwbw... over light
				}
				j += k-1;
				if ((k&1) == 0) {
					i++;
					sepPattern[j] += data[i]; // trailing w for e1, append to next w
				}
				else {
					i++;
					j++;
					sepPattern[j] = data[i]; // trailing b, next is w e2
				}
				i++;
				j++;
				for (k = 0; k < data[i]; k++) {
					sepPattern[j+k] = 1; // bwbw... over light e3
				}
				j += k-1;
				if ((k&1) == 0) {
					i++;
					sepPattern[j] += data[i]; // trailing w for e3, append to next w
				}
				else {
					i++;
					j++;
					sepPattern[j] = data[i]; // trailing b for e3, next is w
				}
			}
			else {
				// finder is dark/light/dark
				i++;
				if (data[i] > 1) {
					j++;
					for (k = 0; k < data[i]; k++) {
						sepPattern[j+k] = 1; // bwbw... over light e2
					}
					j += k-1;
					if ((k&1) == 0) {
						i++;
						sepPattern[j] += data[i]; // trailing w for e2, append to next w
					}
					else {
						i++;
						j++;
						sepPattern[j] = data[i]; // trailing b for e2, next is w
					}
				}
				else {
					i++;
					sepPattern[j] = 10; // 1X light e2 (val=3), so w/b/w = 10/1/2
					sepPattern[j+1] = 1;
					sepPattern[j+2] = 2;
					j+=2;
				}
			}
		}
	}
	k = 2;
	j--;
	for ( ; k <= 4; k += sepPattern[j], j--);
	if ((j&1)==0) {
		j += 2;
		sepPattern[j-1] = k-4;
		sepPattern[j] = 4;
	}
	else {
		j++;
		sepPattern[j] = k;
	}
	//prntSep.elmCnt = j+1;
	//return(&prntSep);
	return j+1;
}
#endif

#ifdef USE_QR_BARCODE
const unsigned char bit_pos[8] = {0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01};
int BarQR (int symbol, INT8U* input, int show_hrt, INT8U scale)
{
	struct zint_symbol *my_symbol;
	INT16U w,h,dw;
	int error = 0;
	int ret;
	int i = 0;
	int bitmap_size;
	char bitmap[BITMAP_MAX_SIZE];
	char* bitmap_conv;
	INT8U backup_StrMagx, backup_StrMagy;
		
	my_symbol = ZBarcode_Create();
	if(my_symbol == NULL) {
		ret = -1;
		goto EXIT;
	}
	my_symbol->symbology = symbol;
	my_symbol->show_hrt = show_hrt;
	my_symbol->scale = scale;
	my_symbol->option_1 = LEVEL_L; // 기본 복원레벨
	
	error = ZBarcode_Encode(my_symbol, input, strlen(input));
	
	if(error != 0)
		printf("%s\n", my_symbol->errtxt);
	//if(error > ERROR_INVALID_OPTION) {
	if(error > ZINT_ERROR_INVALID_OPTION) {
		ret = -1;
		goto EXIT;
	}

	w = my_symbol->width;
	h = my_symbol->rows;
	dw = w >> 3;
	if(my_symbol->width % 8) dw++;
	if(PrtStruct.Mode == RECEIPT) {
		PrtStruct.y = 0;
		switch(StrStruct.Justification) {
			default:
			case CENTER:
				PrtStruct.x = (PrtStruct.PrintWidth - w * scale)>>1;
				break;
			case LEFT:
				PrtStruct.x = 0;
				break;
			case RIGHT:
				PrtStruct.x = PrtStruct.PrintWidth - w * scale - 1;
				break;
		}
		PrtStruct.ReceiptBmpHeight = h * StrStruct.Magy;
	}
	
	bitmap_size = dw * 8 * h;
	if(dw * h > BITMAP_MAX_SIZE) { 
		ret = -2;
		goto EXIT;
	}
	
	memset (bitmap, 0, sizeof(bitmap));

	int j = 0;
	j = 0;
	for(j = 0; j < h; j++)
    {
        for(i = 0 ; i < w; i++)
        {
            if (module_is_set(my_symbol, j, i))
            {
                bitmap[j*dw + i/8] |= bit_pos[i%8];
            } 
        }
	}
	
	backup_StrMagx = StrStruct.Magx;
    backup_StrMagy = StrStruct.Magy;
    
    StrStruct.Magx = scale;
	StrStruct.Magy = scale;
	
	PrtPattern((INT8U* )bitmap,dw,(short)w,(short)h,PrtStruct.Rot);
	ret = 0;

	StrStruct.Magx = backup_StrMagx;
    StrStruct.Magy = backup_StrMagy;
EXIT:
	ZBarcode_Delete(my_symbol);
	return ret;
}
#endif

#ifdef USE_QR_SIZE_SETTING
INT8U GetQRCodeSzie(void)
{
	INT8U qr_size;
	qr_size = get_global_bparam(GLOBAL_SALE_SETUP_FLAG21);
	qr_size &= 0x07;

	return qr_size; 
}
#endif