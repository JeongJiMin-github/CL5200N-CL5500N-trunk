/*****************************************************************************
|*			
|*  Copyright		:	(c) 2002 CAS
|*  Filename		:	prt_render.c
|*  Version		:	0.1
|*  Programmer(s)	:	Hong Yi-Phyo (HYP)
|*  Created		:	2002/12/10
|*  Description		:	CLP Printer render routine    
|*				
****************************************************************************/

#include <stdio.h>
#include <string.h>
#include "globals.h"
#include "prt_common.h"
#include "prtdrv.h"
#include "prt_render.h"
#include "bar_common.h"
#include "prt_line.h"
#include "prt_font.h"
#include "prt_cnvtable.h"
#include "timer.h"
#ifdef USE_SRAM_PRT_RENERING_BUF
#include "spi_sram.h"
#endif
#ifdef USE_TEST_PRT_RENDERING_TIME
#include "main.h"
extern void MsgOut(HUGEDATA char *);
#endif
#ifdef USE_TEST_TMP_PRTBUF_MAX_SIZE
#include "flash.h"
#include "initial_standard.h"
#endif


//#define USE_PrtPatternHVflip_Funtion //HYP 20060404

ROMDATA INT8U nlmask[] = { 0xff, 0x7f, 0x3f, 0x1f, 0x0f, 0x07, 0x03, 0x01 };
ROMDATA INT8U lmask[] = { 0x00, 0x80, 0xc0, 0xe0, 0xf0, 0xf8, 0xfc, 0xfe };
//sROMDATA INT8U rmask[] = { 0x00, 0x01, 0x03, 0x07, 0x0f, 0x1f, 0x3f, 0x7f };
/*
ROMDATA INT16U nlmask16[] = { 0xffff, 0x7fff, 0x3fff, 0x1fff, 0x0fff, 0x07ff, 0x03ff, 0x01ff,
 							  0x00ff, 0x007f, 0x003f, 0x001f, 0x000f, 0x0007, 0x0003, 0x0001};
ROMDATA INT16U lmask16[] = { 0x0000, 0x8000, 0xc000, 0xe000, 0xf000, 0xf800, 0xfc00, 0xfe00,
 							 0xff00, 0xff80, 0xffc0, 0xffe0, 0xfff0, 0xfff8, 0xfffc, 0xfffe};

*/
ROMDATA INT16U nlmask16[] = { 0xffff, 0xff7f, 0xff3f, 0xff1f, 0xff0f, 0xff07, 0xff03, 0xff01,
 							  0xff00, 0x7f00, 0x3f00, 0x1f00, 0x0f00, 0x0700, 0x0300, 0x0100};
ROMDATA INT16U lmask16[] = { 0x0000, 0x0080, 0x00c0, 0x00e0, 0x00f0, 0x00f8, 0x00fc, 0x00fe,
 							 0x00ff, 0x80ff, 0xc0ff, 0xe0ff, 0xf0ff, 0xf8ff, 0xfcff, 0xfeff};


static ROMDATA INT8U	PrtBarPattern_Table[6][3] = { 	{4,4,4},	 //no guard
							{2,5,4},  //ean13 0 deg
							{1,2,5},  //ean13a 0 deg
							{4,4,4},	 //no guard
							{0,4,4},  //ean13 180 deg
							{0,3,5}}; //ean13a 180 deg

/* HYP 20060404 Erase (non-use function)
void draw_cross(short x,short y, short w)
{
	PrtHLine(x-w,x+w,y,1,1);
	PrtVLine(y-w,y+w,x,1,1);
}
*/

INT8U Inline_Rol8(INT8U ch, INT8U count) {
	switch (count) {
		case 1:
			return ((ch & 0x80) >> 7) | ((ch & 0x7F) << 1);
		case 2:
			return ((ch & 0xC0) >> 6) | ((ch & 0x3F) << 2);
		case 3:
			return ((ch & 0xE0) >> 5) | ((ch & 0x1F) << 3);
		case 4:
			return ((ch & 0xF0) >> 4) | ((ch & 0x0F) << 4);
		case 5:
			return ((ch & 0xF8) >> 3) | ((ch & 0x07) << 5);
		case 6:
			return ((ch & 0xFC) >> 2) | ((ch & 0x03) << 6);
		case 7:
			return ((ch & 0xFE) >> 1) | ((ch & 0x01) << 7);
	}
	return ch;
}

INT8U Inline_Ror8(INT8U ch, INT8U count) {
	switch (count) {
		case 1:
			return ((ch & 0x01) << 7) | ((ch & 0xFE) >> 1);
		case 2:
			return ((ch & 0x03) << 6) | ((ch & 0xFC) >> 2);
		case 3:
			return ((ch & 0x07) << 5) | ((ch & 0xF8) >> 3);
		case 4:
			return ((ch & 0x0F) << 4) | ((ch & 0xF0) >> 4);
		case 5:
			return ((ch & 0x1F) << 3) | ((ch & 0xE0) >> 5);
		case 6:
			return ((ch & 0x3F) << 2) | ((ch & 0xC0) >> 6);
		case 7:
			return ((ch & 0x7F) << 1) | ((ch & 0x80) >> 7);
	}
	return ch;
}

INT16U Inline_Ror16(INT16U ch, INT8U count) {
	switch (count) {
		case 1:
			return ((ch & 0x0001) << 15) | ((ch & 0xFFFE) >> 1);
		case 2:
			return ((ch & 0x0003) << 14) | ((ch & 0xFFFC) >> 2);
		case 3:
			return ((ch & 0x0007) << 13) | ((ch & 0xFFF8) >> 3);
		case 4:
			return ((ch & 0x000F) << 12) | ((ch & 0xFFF0) >> 4);
		case 5:
			return ((ch & 0x001F) << 11) | ((ch & 0xFFE0) >> 5);
		case 6:
			return ((ch & 0x003F) << 10) | ((ch & 0xFFC0) >> 6);
		case 7:
			return ((ch & 0x007F) << 9) | ((ch & 0xFF80) >> 7);
		case 8:
			return ((ch & 0x00FF) << 8) | ((ch & 0xFF00) >> 8);
		case 9:
			return ((ch & 0x01FF) << 7) | ((ch & 0xFE00) >> 9);
		case 10:
			return ((ch & 0x03FF) << 6) | ((ch & 0xFC00) >> 10);
		case 11:
			return ((ch & 0x07FF) << 5) | ((ch & 0xF800) >> 11);
		case 12:
			return ((ch & 0x0FFF) << 4) | ((ch & 0xF000) >> 12);
		case 13:
			return ((ch & 0x1FFF) << 3) | ((ch & 0xE000) >> 13);
		case 14:
			return ((ch & 0x3FFF) << 2) | ((ch & 0xC000) >> 14);
		case 15:
			return ((ch & 0x7FFF) << 1) | ((ch & 0x8000) >> 15);
	}
	return ch;
}

ROMDATA INT8U *Inline_magX(char mx)
{
	switch (mx) {
		default:
		case 1:
			return CnvTableMagX1;
		case 2:
			return *CnvTableMagX2;
		case 3:
			return *CnvTableMagX3;
		case 4:
			return *CnvTableMagX4;
		case 5:
			return *CnvTableMagX5;
	}
}

INT8U	CheckPrePrtAreaBoundary = OFF;//HYP 20060427

BOOLEAN	CheckBoundary(short sx, short sy, short ex, short ey) {
	if (sx<0) return FALSE;
//	if (ex<0) return FALSE;
//	if (sx>=PrtStruct.PrintWidth) return FALSE;
	//HYP 20040723
//	if (ex>=PrtStruct.PrintWidth) return FALSE;
	if(PrtDrvStruct.PrtDirection == NON_INVERT) {
		if (ex>=(PrtStruct.PrintWidth + PrtStruct.NonInvModeTphAlign)) return FALSE;
	} else {
		if (ex>=PrtStruct.PrintWidth) return FALSE;
	}
	
	if (sy<0) return FALSE;
//	if (ey<0) return FALSE;
//	if (sy>=PrtStruct.PrintHeight) return FALSE;

//      if (ey>=PrtStruct.PrintHeight) return FALSE;//HYP 20060427 change to below code
	if ((CheckPrePrtAreaBoundary == ON) && (PrtDrvStruct.PrePrintMode == ENABLE)) {                               
		//HYP 20070629	
//		if (ey>=(PrtStruct.PrintHeight - PrtDrvStruct.PrePrintLength)) return FALSE; 
		if(PrtDrvStruct.PrtDirection == NON_INVERT) {
			if (sy<PrtDrvStruct.PrePrintLength) return FALSE;
			if (ey>=PrtStruct.PrintHeight) return FALSE;
		} else {
			if (ey>=(PrtStruct.PrintHeight - PrtDrvStruct.PrePrintLength)) return FALSE; 
		}
	} else { 
		if (ey>=PrtStruct.PrintHeight) return FALSE;
	}
	return TRUE;
}

//HYP 20040226
short CheckBoundaryErrX(short sx,short ex) {
	if (sx<0) return 0x0fff;
	//HYP 20040723
//	if (ex>=PrtStruct.PrintWidth) return (ex - PrtStruct.PrintWidth);
	if(PrtDrvStruct.PrtDirection == NON_INVERT) {
		if (ex>=(PrtStruct.PrintWidth + PrtStruct.NonInvModeTphAlign)) return (ex - (PrtStruct.PrintWidth + PrtStruct.NonInvModeTphAlign));
	} else {
		if (ex>=PrtStruct.PrintWidth) return (ex - PrtStruct.PrintWidth);
	}
	return 0;
}
//HYP 20040226
short CheckBoundaryErrY(short sy,short ey) {	

	if (sy<0) return 0x0fff;
//	if (ey>=PrtStruct.PrintHeight) return (ey - PrtStruct.PrintHeight);//HYP 20060427 change to below code
	if ((CheckPrePrtAreaBoundary == ON) && (PrtDrvStruct.PrePrintMode == ENABLE)) {
		//HYP 20070629
//		if (ey>=(PrtStruct.PrintHeight - PrtDrvStruct.PrePrintLength)) return (ey - (PrtStruct.PrintHeight - PrtDrvStruct.PrePrintLength));
		if(PrtDrvStruct.PrtDirection == NON_INVERT) {
			if (sy<PrtDrvStruct.PrePrintLength) return 0x0fff;	
			if (ey>=PrtStruct.PrintHeight) return (ey - PrtStruct.PrintHeight);	
		} else {
			if (ey>=(PrtStruct.PrintHeight - PrtDrvStruct.PrePrintLength)) return (ey - (PrtStruct.PrintHeight - PrtDrvStruct.PrePrintLength));
		}
	} else {
		if (ey>=PrtStruct.PrintHeight) return (ey - PrtStruct.PrintHeight);	
	}
	return 0;
}

/*
void RenderTestBarcode(void);
void RenderTestFont(void);

void RenderTest(void)
{

//	HUGEDATA INT8U *font_ptr;
//	HUGEDATA char *p;
//	char string_buf[20];

	PrtStruct.x = 0;
	PrtStruct.y = 0;			
//	PrtStruct.Rot = DEG_0;
//	prt_clear();
//	PrtRectFill(320,40,420,120,WHITE);
 
	 
//	PrtStruct.Rot = DEG_180;
//	PrtStruct.x = 30;
//	PrtStruct.y = 350;			
//	PrtStruct.y = 20;
//	PrtStruct.Opaque = OFF;
//	StrStruct.Typeface = NO_TYPEFACE;
//	StrStruct.Magy = 2;
//	StrStruct.Magx = 2;
//	StrStruct.Justification = CENTER;	 

//	draw_cross(PrtStruct.x,PrtStruct.y,10);	
//	PrtRect(0,0,PrtStruct.PrintableWidth-1,PrtStruct.PrintableHeight-1,1,BLACK);
	BarStruct.BarType = CODE128X;
//	BarStruct.BarWidthMag = 1;
	RenderTestBarcode();

//	draw_cross(PrtStruct.x,PrtStruct.y,10);
//  test draw line
//	for (i=0;i<18;i++) {
//		PrtHLine(100 + i,200 + i,250 + 12 *i ,i%9,1);
//		PrtVLine(110 + i,200 + i,100 + 12 * i,i%9,1);
//	}	
	
	
	RenderTestFont();

//start stop gap test
//	for (i =0; i <40; i++) {
//		PrtRect(2*i,2*i,319-2*i,239-2*i,1,BLACK);
//	}

//	PrtStruct.Rot = DEG_0;
//	StrStruct.Magy = 5;
// render time check on	///////////
//	PrtDrvStruct.SysTimer = 0;
//	PrtDrvStart(PRT_STATE_TM);
	StrStruct.Magy = 1;
	StrStruct.Magx = 1;
///////////////////////////////////
//	Bmptest();
// render time check off //////////
//	PrtDrvStop();
//	sprintf(string_buf, "bm=%d,",PrtDrvStruct.SysTimer); 
//	PutStrTxBuf(&CommBuf, string_buf);
//////////////////////////////////
}
*/

void PrtPattern_Swap(HUGEDATA INT8U *pattern,short pw)
{
	INT8U cc,cb,cbl,cbr;
	short i,spos;
	INT8U  line_buf[LINE_BUF_SIZE];

	cc = pw&0x07;
	spos=pw>>3;
	if (cc) spos++;

	for (i=0; i<spos; i++) {
		line_buf[i]=pattern[i];
	}

	if (cc != 0) {
		for (i=0; i<spos + 1; i++) {
			if ( i == 0 ) {	 // spos>1
				cb=CnvTableMirror[line_buf[spos - i - 1]];
				cb = Inline_Ror8(cb,cc);
				cbl = cb & lmask[cc];
			} else if (i == spos) {
				pattern[i-1] = cbl;
			} else {
				cb=CnvTableMirror[line_buf[spos - i - 1]];
				cb = Inline_Ror8(cb,cc);
				cbr = cb & nlmask[cc];
				pattern[i-1] = cbl | cbr;
				cbl = cb & lmask[cc];
			}
		}
		pattern[i-1]=0x00;//clear word size buffer
	} else {
		for (i=0; i<spos; i++) {
			pattern[i]=CnvTableMirror[line_buf[spos - i - 1]];
		}
		//HYP 20040228
		pattern[i]=0x00;//clear word size buffer
	}		
}

void PrtPattern_Swap_Cpy(HUGEDATA INT8U *dpattern,HUGEDATA INT8U *spattern,short pw)
{
	INT8U cc,cb,cbl,cbr;
	short i,spos;

	cc = pw&0x07;
	spos=pw>>3;
	if (cc) spos++;

	if (cc != 0) {
		for (i=0; i<spos + 1; i++) {
			if ( i == 0 ) {		// spos>0
				cb=CnvTableMirror[spattern[spos - i - 1]];
				cb = Inline_Ror8(cb,cc);
				cbl = cb & lmask[cc];
			} else if (i == spos) {
				dpattern[i-1] = cbl;
			} else {
				cb=CnvTableMirror[spattern[spos - i - 1]];
				cb = Inline_Ror8(cb,cc);
				cbr = cb & nlmask[cc];
				dpattern[i-1] = cbl | cbr;
				cbl = cb & lmask[cc];
			}
		}
		dpattern[i-1]=0x00;//clear word size buffer
	} else {
		for (i=0; i<spos; i++) {
			dpattern[i]=CnvTableMirror[spattern[spos - i - 1]];
		}
		//HYP 20040228
		dpattern[i]=0x00;//clear word size buffer
	}		
}

#ifdef USE_PrtPatternHVflip_Funtion
void PrtPattern_VFlip(HUGEDATA INT8U *pattern,short dwr,short h)
{
	short i,j;
	long  spos,epos;
	INT8U cc;

	spos=0;
	epos=(long)dwr*(h-1);
	for (i=0; i<h/2; i++) {
		for (j=0; j<dwr; j++) {
			cc=pattern[spos+j];
			pattern[spos+j]=pattern[epos+j];
			pattern[epos+j]=cc;
		}
		epos-=dwr;
		spos+=dwr;
	}
}

void PrtPattern_VFlip_Cpy(HUGEDATA INT8U *dpattern,HUGEDATA INT8U *spattern,short dwr,short h)
{
	short i,j;
	long  spos,epos;

	spos=0;
	epos=(long)dwr*(h-1);

	for (i=0; i<h; i++) {
		for (j=0; j<dwr; j++) {
			dpattern[epos+j]=spattern[spos+j];
		}
		epos-=dwr;
		spos+=dwr;
	}
}

void PrtPattern_HFlip_Cpy(HUGEDATA INT8U *dpattern, HUGEDATA INT8U *spattern, short dwr, short w,short h)
{
	short i;
	long  spos;

	spos=0;
	for (i=0; i<h; i++) {
		PrtPattern_Swap_Cpy(&dpattern[spos], &spattern[spos],w);
		spos+=dwr;
	}
}

void PrtPattern_HFlip(HUGEDATA INT8U *pattern, short dwr, short w,short h)
{
	short i;
	long  spos;

	spos=0;
	for (i=0; i<h; i++) {
		PrtPattern_Swap(&pattern[spos],w);
		spos+=dwr;
	}
}

#endif//#ifdef USE_PrtPatternHVflip_Funtion

//pattern must be even addr. 
void PrtBarPattern_Hori(HUGEDATA INT8U *pattern,short pw)
{
	short x1,y1,x2,y2,w1,w2,x,y,py1,py2;
	INT8U skip;
//	short shift16;
	long  start_pos16,spos;
	INT8U smask;
	short    dx16,i,j,dw16;
	INT8U	rx16, rw16, nrx16;
	INT16U cb, cbr, cbl, cbn,cbm;
	short mx1,mx2,cx1,cx2,p1,p2,p3,p4;//,dy;
	HUGEDATA INT16U *pattern3,*prt_memory,*prt_memory2;
	short lwidthbyte16;
	char opaque;
	INT8U tflag,type;
	INT8U bar_trans_pattern[MAX_BAR_PATTERN_SIZE];
	INT8U b_bar_hri_pos;
	short px, py;
	short backup_lmargin, backup_tmargin, backup_noninvmodetphalign;
#ifdef USE_SRAM_PRT_RENERING_BUF
    INT32U pSram = 0;
    INT32U pSram2 = 0;
    INT8U pTempSramBuff[2];
#endif



	backup_lmargin = PrtStruct.LMargin;
	backup_tmargin = PrtStruct.TMargin;
		
	px = PrtStruct.x + PrtStruct.LMargin;
	py = PrtStruct.y + PrtStruct.TMargin;	

	//HYP 20040723
	backup_noninvmodetphalign = PrtStruct.NonInvModeTphAlign;
	if(PrtDrvStruct.PrtDirection == NON_INVERT) {
		px = px + PrtStruct.NonInvModeTphAlign;
	}

	// Check Boundary 
	//HYP 20040906
//	if (CheckBoundary(px,py,px+pw,py+BarStruct.FullHeight) == FALSE)
//	if (CheckBoundary(px,py,px+pw,py+BarStruct.FullHeight-1) == FALSE) 
	//HYP 20060427 "+2*BarStruct.QuietZone" is right but assume that the opposite side is blank(white)
	if (CheckBoundary(px,py,px+pw+BarStruct.QuietZone,py+BarStruct.FullHeight-1) == FALSE) {
		return;
	}
	//PrtRectFill() and PrtHLine() use margin
	PrtStruct.LMargin = 0;
	PrtStruct.TMargin = 0;
	//HYP 20040723 
	PrtStruct.NonInvModeTphAlign = 0;

//	dy=BarStruct.FullHeight-BarStruct.BarHeight;
	lwidthbyte16 = PrtStruct.PrintableWidthBytes>>1;
	opaque = PrtStruct.Opaque;

	b_bar_hri_pos = BarStruct.BarHriPos;
//HYP 20040127
	if((BarStruct.BarType <= BAR_HRI_POS_ENABLE_MARGIN) && (BarStruct.GuardType != NO_GUARD)) {
//	if(BarStruct.BarType <= BAR_HRI_POS_ENABLE_MARGIN) {
		BarStruct.BarHriPos = HRI_POS_DOWN; //backup 
	}
	if((PrtStruct.Rot == DEG_0 && BarStruct.BarHriPos == HRI_POS_DOWN) || (PrtStruct.Rot == DEG_180 && BarStruct.BarHriPos == HRI_POS_UP)) {	//0 deg
		if(PrtStruct.Rot == DEG_180) {//HYP 20040127
		//if(BarStruct.BarHriPos == HRI_POS_UP) {
			PrtPattern_Swap(pattern,pw);
		}
		if(BarStruct.GuardType != NO_GUARD) {
		   	y2=py+BarStruct.BarHeight;
		} else {
			y2=py+BarStruct.FullHeight-BarStruct.CharHeight;
		}
		y1=py; 
		x1=px + BarStruct.QuietZone; 
		x2=px+pw+ BarStruct.QuietZone;
		p1 =x1+BarStruct.p1;
		p2 =x1+BarStruct.p2;
		p3 =x1+BarStruct.p3;
		p4 =x1+BarStruct.p4;
		w1 =x1+BarStruct.x1;
		w2 =x1+BarStruct.x2;
		mx1=py;
		mx2=py+(BarStruct.FullHeight-BarStruct.CharHeight);
		cx1=py+BarStruct.CharHeight;
		cx2=y2;
		type = BarStruct.GuardType;
	} else {// ((PrtStruct.Rot == DEG_180 && BarStruct.BarHriPos == HRI_POS_DOWN) || (PrtStruct.Rot == DEG_0 && BarStruct.BarHriPos == HRI_POS_UP)) //180 deg
		if(PrtStruct.Rot == DEG_180) {//HYP 20040127
		//if(BarStruct.BarHriPos == HRI_POS_DOWN) {
			PrtPattern_Swap(pattern,pw);
		}
	   	if(BarStruct.GuardType != NO_GUARD) {
		    y1=py + BarStruct.FullHeight -BarStruct.BarHeight+1; 
		} else {
			y1=py + BarStruct.CharHeight;
		}		
	    y2=py + BarStruct.FullHeight;
    	x1=px+1 + BarStruct.QuietZone; 
    	x2=px+ pw +1 + BarStruct.QuietZone;	
	    p1 =x1+pw-BarStruct.p4;
    	p2 =x1+pw-BarStruct.p3;
	    p3 =x1+pw-BarStruct.p2;
    	p4 =x1+pw-BarStruct.p1;
	    w1 =x1+pw-BarStruct.x2;
    	w2 =x1+pw-BarStruct.x1;
	    mx1=py+BarStruct.CharHeight;
    	mx2=py+ BarStruct.FullHeight;
	    cx1=y1;
    	cx2=y2-BarStruct.CharHeight;
		type = BarStruct.GuardType+3;
	}
	BarStruct.BarHriPos = b_bar_hri_pos;

	if(opaque && (BarStruct.QuietZone != 0)) {
		PrtRectFill(px, py, x1, py+BarStruct.FullHeight,WHITE);	
		PrtRectFill(x2, py, x2+BarStruct.QuietZone, py+BarStruct.FullHeight,WHITE);
	}

//	shift16=x1&0x0f;
	dx16 = x1 >> 4;
	rx16=x1&0x0f;
	dw16=(x2-x1)>>4;
	rw16=(x2-x1)&0x0f;
	if(rw16 != 0) dw16++;

	py1=y1; 
	for(j=0; j<5; j++) {
		if (BarStruct.GuardType != NO_GUARD) {
			tflag=1;
			switch(PrtBarPattern_Table[type][j]) {
				case 0:
					py2=mx1;
					break;
				case 1:
					py2=cx1;
					break;
				case 2:
					tflag=0;
					py2=mx2;
					break;
				case 3:
					tflag=0;
					py2=cx2;
					break;
				case 4:
					tflag=0;
				case 5:
					py2=y2;
					j=5;
					break;
			}
			if (tflag==1)	{	  //y1,mx1,cx1,mx2,cx2,y2
				smask=0x80;
				bar_trans_pattern[(x2-1-x1)>>3] = 0x00; //clear for word operation
				for (x=x1; x<x2; x++) {
					skip=0;
					if (((x>=p1) && (x<p2)) || ((x>=p3) && (x<p4))) {
						if (py1>=mx2) skip=1;
						if (py1<mx1) skip=1;
					}
					if ((x>=w1) && (x<w2)) {
						if (py1<cx1) skip=1;
						if (py1>=cx2) skip=1;
					}		
					if (skip==0) {
						spos=(x-x1)>>3;
						if (pattern[spos]&smask) {
							bar_trans_pattern[spos] |= smask;
						} else {
							bar_trans_pattern[spos] &= ~smask;
						}
					} else {
						spos=(x-x1)>>3;
						bar_trans_pattern[spos] &= ~smask;
					}							
					smask>>=1;
					if (smask==0) smask=0x80;
				}
				bar_trans_pattern[spos+1] = 0x00; //clear for word operation
				pattern3 = (HUGEDATA INT16U *)bar_trans_pattern;
			} else {
				pattern3 = (HUGEDATA INT16U *)pattern;
			}
		} else {
			j =5;
			py1=y1;py2=y2;
			pattern3 = (HUGEDATA INT16U *)pattern;
		}
//y1=py1,y2=py2
		start_pos16 = (long)py1*(lwidthbyte16) + dx16;
    #ifdef USE_SRAM_PRT_RENERING_BUF
        pSram = SramPrtMemory + (start_pos16*2);
    #else //  USE_SRAM_PRT_RENERING_BUF
		prt_memory = ((HUGEDATA INT16U *)PrtStruct.PrtMemory)+start_pos16;
	#endif //  USE_SRAM_PRT_RENERING_BUF
		if (rx16 != 0) {
			nrx16 = 16 - rx16;
			cbl = 0x0000;
			for (i = 0; i < (dw16 + 1); i++) {
			#ifdef USE_SRAM_PRT_RENERING_BUF
                pSram2 = pSram;
            #else
                prt_memory2 = prt_memory;
            #endif
				cb = pattern3[i];
				cb = Inline_Ror16(cb, rx16);
				cbr = nlmask16[rx16] & cb;
			 	if (i == 0) {	// dw > 1
					cbn = cbr;
					cbl = lmask16[rx16] & cb;
					if (opaque) {
						cbm = lmask16[rx16] | cbn;
						for (y=py1; y<py2; y++) {
						#ifdef USE_SRAM_PRT_RENERING_BUF
//	                                    pTempSramBuff = (INT8U *)malloc(2);
                                SramReadData( pSram2, pTempSramBuff, 2 );
                                prt_memory2 = (HUGEDATA INT16U *)pTempSramBuff;
                            #endif
								*prt_memory2 |= cbn;
								*prt_memory2 &= cbm;
                            #ifdef USE_SRAM_PRT_RENERING_BUF
                                SramWriteData( pSram2, pTempSramBuff, 2 );
//	                                    free( pTempSramBuff );
                                pSram2 = pSram2 + (lwidthbyte16*2);
                            #else
                                prt_memory2 = prt_memory2 + lwidthbyte16;
                            #endif
						}
					} else {
						for (y=py1; y<py2; y++) {
    					#ifdef USE_SRAM_PRT_RENERING_BUF
                            SramReadData( pSram2, pTempSramBuff, 2 );
                            prt_memory2 = (HUGEDATA INT16U *)pTempSramBuff;
                        #endif
    						*prt_memory2 |= cbn;
                        #ifdef USE_SRAM_PRT_RENERING_BUF
                            SramWriteData( pSram2, pTempSramBuff, 2 );
                            pSram2 = pSram2 + (lwidthbyte16*2);
                        #else
                            prt_memory2 = prt_memory2 + lwidthbyte16;
                        #endif
						}
					}
				} else if (i < dw16 - 1) {
					cbn = cbl | cbr;
					cbl = lmask16[rx16] & cb;
					if (opaque)	{
						for (y=py1; y<py2; y++) {
					    #ifdef USE_SRAM_PRT_RENERING_BUF
                            SramReadData( pSram2, pTempSramBuff, 2 );
                            prt_memory2 = (HUGEDATA INT16U *)pTempSramBuff;
                        #endif
							*prt_memory2 = cbn;
                        #ifdef USE_SRAM_PRT_RENERING_BUF
                            SramWriteData( pSram2, pTempSramBuff, 2 );
                            pSram2 = pSram2 + (lwidthbyte16*2);
                        #else
                            prt_memory2 = prt_memory2 + lwidthbyte16;
                        #endif
						}
					} else {
						for (y=py1; y<py2; y++) {
						#ifdef USE_SRAM_PRT_RENERING_BUF
                            SramReadData( pSram2, pTempSramBuff, 2 );
                            prt_memory2 = (HUGEDATA INT16U *)pTempSramBuff;
                        #endif
							*prt_memory2 |= cbn;
                        #ifdef USE_SRAM_PRT_RENERING_BUF
                            SramWriteData( pSram2, pTempSramBuff, 2 );
                            pSram2 = pSram2 + (lwidthbyte16*2);
                        #else
                            prt_memory2 = prt_memory2 + lwidthbyte16;
                        #endif
					    }
					}
				} else if (i == dw16 - 1) { // dw > 1
					if((nrx16 >= rw16) && (rw16 != 0)) {
						cbn = cbl | cbr;
						if (opaque)	{
								cbm = nlmask16[rx16+rw16] | cbn;
							for (y=py1; y<py2; y++) {
							#ifdef USE_SRAM_PRT_RENERING_BUF
                                SramReadData( pSram2, pTempSramBuff, 2 );
                                prt_memory2 = (HUGEDATA INT16U *)pTempSramBuff;
                            #endif
								*prt_memory2 |= cbn;
								*prt_memory2 &= cbm;
                            #ifdef USE_SRAM_PRT_RENERING_BUF
                                SramWriteData( pSram2, pTempSramBuff, 2 );
                                pSram2 = pSram2 + (lwidthbyte16*2);
                            #else
                                prt_memory2 = prt_memory2 + lwidthbyte16;
                            #endif
							}
						} else {
							for (y=py1; y<py2; y++) {
							#ifdef USE_SRAM_PRT_RENERING_BUF
                                SramReadData( pSram2, pTempSramBuff, 2 );
                                prt_memory2 = (HUGEDATA INT16U *)pTempSramBuff;
                            #endif
								*prt_memory2 |= cbn;
                            #ifdef USE_SRAM_PRT_RENERING_BUF
                                SramWriteData( pSram2, pTempSramBuff, 2 );
                                pSram2 = pSram2 + (lwidthbyte16*2);
                            #else
                                prt_memory2 = prt_memory2 + lwidthbyte16;
                            #endif
							}		
						}
					} else {
						cbn = cbl | cbr;
						cbl = lmask16[rx16] & cb;
						if (opaque)	{
							for (y=py1; y<py2; y++) {
							#ifdef USE_SRAM_PRT_RENERING_BUF
                                SramReadData( pSram2, pTempSramBuff, 2 );
                                prt_memory2 = (HUGEDATA INT16U *)pTempSramBuff;
                            #endif
								*prt_memory2 = cbn;
                            #ifdef USE_SRAM_PRT_RENERING_BUF
                                SramWriteData( pSram2, pTempSramBuff, 2 );
                                pSram2 = pSram2 + (lwidthbyte16*2);
                            #else
                                prt_memory2 = prt_memory2 + lwidthbyte16;
                            #endif 
							}
						} else {
							for (y=py1; y<py2; y++) {
							#ifdef USE_SRAM_PRT_RENERING_BUF
                                SramReadData( pSram2, pTempSramBuff, 2 );
                                prt_memory2 = (HUGEDATA INT16U *)pTempSramBuff;
                            #endif
								*prt_memory2 |= cbn;
                            #ifdef USE_SRAM_PRT_RENERING_BUF
                                SramWriteData( pSram2, pTempSramBuff, 2 );
                                pSram2 = pSram2 + (lwidthbyte16*2);
                            #else
                                prt_memory2 = prt_memory2 + lwidthbyte16;
                            #endif
							}
						}
					}
				} else if (i == dw16) {
					if((nrx16<rw16) || (rw16 == 0)) {
						cbn = cbl;
						if (opaque) {
							if(rw16 == 0) {
								cbm = nlmask16[rx16] | cbn;
								for (y=py1; y<py2; y++) {
								#ifdef USE_SRAM_PRT_RENERING_BUF
                                    SramReadData( pSram2, pTempSramBuff, 2 );
                                    prt_memory2 = (HUGEDATA INT16U *)pTempSramBuff;
                                #endif
									*prt_memory2 |= cbn; 
									*prt_memory2 &= cbm;
                                #ifdef USE_SRAM_PRT_RENERING_BUF
                                    SramWriteData( pSram2, pTempSramBuff, 2 );
                                    pSram2 = pSram2 + (lwidthbyte16*2);
                                #else
                                    prt_memory2 = prt_memory2 + lwidthbyte16;
                                #endif
								}
							} else {
								cbm = nlmask16[rw16-nrx16] | cbn;
								for (y=py1; y<py2; y++) {
								#ifdef USE_SRAM_PRT_RENERING_BUF
                                    SramReadData( pSram2, pTempSramBuff, 2 );
                                    prt_memory2 = (HUGEDATA INT16U *)pTempSramBuff;
                                #endif
									*prt_memory2 |= cbn; 
									*prt_memory2 &= cbm;
                                #ifdef USE_SRAM_PRT_RENERING_BUF
                                    SramWriteData( pSram2, pTempSramBuff, 2 );
                                    pSram2 = pSram2 + (lwidthbyte16*2);
                                #else
                                    prt_memory2 = prt_memory2 + lwidthbyte16;
                                #endif
								}
							}
						} else {
							for (y=py1; y<py2; y++) {
							#ifdef USE_SRAM_PRT_RENERING_BUF
                                SramReadData( pSram2, pTempSramBuff, 2 );
                                prt_memory2 = (HUGEDATA INT16U *)pTempSramBuff;
                            #endif
								*prt_memory2 |= cbn;
                            #ifdef USE_SRAM_PRT_RENERING_BUF
                                SramWriteData( pSram2, pTempSramBuff, 2 );
                                pSram2 = pSram2 + (lwidthbyte16*2);
                            #else
                                prt_memory2 = prt_memory2 + lwidthbyte16;
                            #endif
							}
						}
					}
				}
			#ifdef USE_SRAM_PRT_RENERING_BUF
                    pSram = pSram + 2;
            #else
				prt_memory++;
            #endif
			}
		} else {
			for (i= 0; i < dw16; i++) {
			#ifdef USE_SRAM_PRT_RENERING_BUF
                pSram2 = pSram;
            #else
                prt_memory2 = prt_memory;
            #endif
				cb = pattern3[i];
				if (opaque) {
					if(i == (dw16-1) && rw16 != 0) {
						cbm = nlmask16[rw16] | cb;
						for (y=py1; y<py2; y++) {
						#ifdef USE_SRAM_PRT_RENERING_BUF
                            SramReadData( pSram2, pTempSramBuff, 2 );
                            prt_memory2 = (HUGEDATA INT16U *)pTempSramBuff;
                        #endif
							*prt_memory2 |= cb;
							*prt_memory2 &= cbm;
                        #ifdef USE_SRAM_PRT_RENERING_BUF
                            SramWriteData( pSram2, pTempSramBuff, 2 );
                            pSram2 = pSram2 + (lwidthbyte16*2);
                        #else
                            prt_memory2 = prt_memory2 + lwidthbyte16;
                        #endif
						}
					} else {
					 	for (y=py1; y<py2; y++) {
						#ifdef USE_SRAM_PRT_RENERING_BUF
                            SramReadData( pSram2, pTempSramBuff, 2 );
                            prt_memory2 = (HUGEDATA INT16U *)pTempSramBuff;
                        #endif
							*prt_memory2 = cb;
                        #ifdef USE_SRAM_PRT_RENERING_BUF
                            SramWriteData( pSram2, pTempSramBuff, 2 );
                            pSram2 = pSram2 + (lwidthbyte16*2);
                        #else
                            prt_memory2 = prt_memory2 + lwidthbyte16;
                        #endif
						}
					}
				} else {
					for (y=py1; y<py2; y++) {
					#ifdef USE_SRAM_PRT_RENERING_BUF
                        SramReadData( pSram2, pTempSramBuff, 2 );
                        prt_memory2 = (HUGEDATA INT16U *)pTempSramBuff;
                    #endif
						*prt_memory2 |= cb;
                    #ifdef USE_SRAM_PRT_RENERING_BUF
                        SramWriteData( pSram2, pTempSramBuff, 2 );
                        pSram2 = pSram2 + (lwidthbyte16*2);
                    #else
                        prt_memory2 = prt_memory2 + lwidthbyte16;
                    #endif
					}
				}
			#ifdef USE_SRAM_PRT_RENERING_BUF
                pSram = pSram + 2;
            #else
				prt_memory++;
            #endif
			}
		}
		py1 = py2;
	}
	if(BarStruct.BearerBarWidth != 0 && BarStruct.BarType != RSSEXP) {
#ifdef USE_BEARER_BAR_ONE_LINE
		if(PrtStruct.Rot == DEG_0)
			PrtHLine(px,x2+BarStruct.QuietZone,y1,BarStruct.BearerBarWidth,BLACK);
		else // 180도 회전일 경우 
			PrtHLine(px,x2+BarStruct.QuietZone,y2-BarStruct.BearerBarWidth,BarStruct.BearerBarWidth,BLACK);
#else // USE_BEARER_BAR_ONE_LINE
		PrtHLine(px,x2+BarStruct.QuietZone,y1,BarStruct.BearerBarWidth,BLACK);
		PrtHLine(px,x2+BarStruct.QuietZone,y2-BarStruct.BearerBarWidth,BarStruct.BearerBarWidth,BLACK);
#endif // USE_BEARER_BAR_ONE_LINE
	}
	PrtStruct.LMargin = backup_lmargin;
	PrtStruct.TMargin = backup_tmargin;
	//HYP 20040723
	PrtStruct.NonInvModeTphAlign = backup_noninvmodetphalign;
}

void PrtBarPattern_Verti(HUGEDATA INT8U *pattern,short pw)
{
	short x1,y1,x2,y2,w1,w2,y;
	long  spos;
	INT8U smask;
	short mx1,mx2,cx1,cx2,p1,p2,p3,p4;//,dy;
	char opaque;
	INT8U b_bar_hri_pos;
	short px, py;
	short backup_lmargin, backup_tmargin, backup_noninvmodetphalign;

	backup_lmargin = PrtStruct.LMargin;
	backup_tmargin = PrtStruct.TMargin;

	px = PrtStruct.x + PrtStruct.LMargin;
	py = PrtStruct.y + PrtStruct.TMargin;

	//HYP 20040723
	backup_noninvmodetphalign = PrtStruct.NonInvModeTphAlign;
	if(PrtDrvStruct.PrtDirection == NON_INVERT) {
		px = px + PrtStruct.NonInvModeTphAlign;
	}
	 
	// Check Boundary
	//HYP 20040906 
//	if (CheckBoundary(px,py,px+BarStruct.FullHeight,py+pw) == FALSE) {
//	if (CheckBoundary(px,py,px+BarStruct.FullHeight-1,py+pw) == FALSE) {
	//HYP 20060427 "+2*BarStruct.QuietZone" is right but assume that the opposite side is blank(white)
	//"+4" is 4 dot(0.5mm) margin for preventing start&stop error (assume)
	//barcode is more critical than string
	if (CheckBoundary(px,py,px+BarStruct.FullHeight-1,py+pw+BarStruct.QuietZone+4) == FALSE) {
		return;
	}
	//PrtRectFill() and PrtHLine() use margin
	PrtStruct.LMargin = 0;
	PrtStruct.TMargin = 0;
	//HYP 20040723 
	PrtStruct.NonInvModeTphAlign = 0;

//	dy=BarStruct.FullHeight-BarStruct.BarHeight;
	opaque = PrtStruct.Opaque;

	b_bar_hri_pos = BarStruct.BarHriPos;
//HYP 20040127
//	if(BarStruct.BarType <= BAR_HRI_POS_ENABLE_MARGIN) {
	if((BarStruct.BarType <= BAR_HRI_POS_ENABLE_MARGIN) && (BarStruct.GuardType != NO_GUARD)) {
		BarStruct.BarHriPos = HRI_POS_DOWN; //backup 
	}
	if((PrtStruct.Rot == DEG_270 && BarStruct.BarHriPos == HRI_POS_DOWN) || (PrtStruct.Rot == DEG_90 && BarStruct.BarHriPos == HRI_POS_UP)) {	//0 deg
		if(PrtStruct.Rot == DEG_270) {//HYP 20040127
		//if(BarStruct.BarHriPos == HRI_POS_DOWN) {
			PrtPattern_Swap(pattern,pw);
		}
		y1=py+1 + BarStruct.QuietZone; 
		y2=py+pw+1 + BarStruct.QuietZone;	
		x1=px; 
		if(BarStruct.GuardType != NO_GUARD) {
#if defined(USE_RSS_BARCODE) || defined(USE_ONLY_RSS_EXPAND_BARCODE) 
			x2=px+BarStruct.BarHeight;
#else
			x2=px+BarStruct.BarHeight-1;
#endif
		} else {
#if defined(USE_RSS_BARCODE) || defined(USE_ONLY_RSS_EXPAND_BARCODE) 
			x2=px+BarStruct.FullHeight-BarStruct.CharHeight;
#else
			x2=px+BarStruct.FullHeight-BarStruct.CharHeight-1;
#endif
		}
		mx1=x1;
#if defined(USE_RSS_BARCODE) || defined(USE_ONLY_RSS_EXPAND_BARCODE) 
		mx2=px+(BarStruct.FullHeight-BarStruct.CharHeight);
#else
		mx2=px+(BarStruct.FullHeight-BarStruct.CharHeight)-1;
#endif
		cx1=px+BarStruct.CharHeight;
		cx2=x2;
		p1 =y1+pw-BarStruct.p4;
		p2 =y1+pw-BarStruct.p3;
		p3 =y1+pw-BarStruct.p2;
		p4 =y1+pw-BarStruct.p1;
		w1 =y1+pw-BarStruct.x2;
		w2 =y1+pw-BarStruct.x1;
	} else {// ((PrtStruct.Rot == DEG_90 && BarStruct.BarHriPos == HRI_POS_DOWN) || (PrtStruct.Rot == DEG_270 && BarStruct.BarHriPos == HRI_POS_UP)) //180 deg
		if(PrtStruct.Rot == DEG_270) {//HYP 20040127
			//if(BarStruct.BarHriPos == HRI_POS_DOWN) {
			//if(BarStruct.BarHriPos == HRI_POS_UP) {//HYP 20040127
			PrtPattern_Swap(pattern,pw);
		}
		y1 =py + BarStruct.QuietZone; 
		y2=py+pw + BarStruct.QuietZone;	
		
		if(BarStruct.GuardType != NO_GUARD) {
			x1 =px+BarStruct.FullHeight-BarStruct.BarHeight+1; 
		} else {
			x1 =px+BarStruct.CharHeight;
		}
		x2=px+BarStruct.FullHeight; 
		mx1=px+BarStruct.FullHeight-(BarStruct.FullHeight-BarStruct.CharHeight)+1;
		mx2=x2;
		cx2=px+BarStruct.FullHeight-BarStruct.CharHeight;
		cx1=x1;
		p1 =BarStruct.p1+y1;
		p2 =BarStruct.p2+y1;
		p3 =BarStruct.p3+y1;
		p4 =BarStruct.p4+y1;
		w1 =BarStruct.x1+y1;
		w2 =BarStruct.x2+y1;
	}
	BarStruct.BarHriPos = b_bar_hri_pos;

	if(opaque && (BarStruct.QuietZone != 0)) {
		PrtRectFill(px, py, px+BarStruct.FullHeight, y1,WHITE);	
		PrtRectFill(px, y2, px+BarStruct.FullHeight, y2+ BarStruct.QuietZone,WHITE);
	}

	smask=0x80;
	for (y=y1; y<y2; y++) {
		spos=(long)(y-y1)>>3;
		if (pattern[spos]&smask) {
		/*
		if (((y>p1) && (y<p2)) || ((y>=p3) && (y<p4))) {
		if(opaque) {
		PrtHLine(x1,mx1,y,1,0);
		}
		PrtHLine(mx1,mx2,y,1,1);
		} else if ((y>=w1) && (y<=w2)) {
		if(opaque) {
		PrtHLine(cx2,x2,y,1,0);
		}
		PrtHLine(cx1,cx2,y,1,1);
		} else {
		PrtHLine(x1,x2,y,1,1);
		}*/
		//HYP 20040127

		if (BarStruct.GuardType != NO_GUARD) {
			if (((y>=p1) && (y<p2)) || ((y>=p3) && (y<p4))) {
				if(opaque) {
					PrtHLine(x1,mx1,y,1,0);
				}
				PrtHLine(mx1,mx2,y,1,1);
			} else if ((y>=w1) && (y<w2)) {
				if(opaque) {
					PrtHLine(cx2,x2,y,1,0);
				}
				PrtHLine(cx1,cx2,y,1,1);
				} else {
					PrtHLine(x1,x2,y,1,1);
				}
			} else {
				PrtHLine(x1,x2,y,1,1);
			}

		} else {
			if(opaque) {
				PrtHLine(x1,x2,y,1,0);
			}
		}
		smask>>=1;
		if (smask==0) smask=0x80;
	}

	if(BarStruct.BearerBarWidth != 0 && BarStruct.BarType != RSSEXP) {
#ifdef USE_BEARER_BAR_ONE_LINE
		if(PrtStruct.Rot == DEG_90)
			PrtVLine(py,y2+BarStruct.QuietZone,x2-BarStruct.BearerBarWidth,BarStruct.BearerBarWidth,BLACK);	
		else // 270도 회전일 경우 
			PrtVLine(py,y2+BarStruct.QuietZone,x1,BarStruct.BearerBarWidth,BLACK);
#else //USE_BEARER_BAR_ONE_LINE
		PrtVLine(py,y2+BarStruct.QuietZone,x1,BarStruct.BearerBarWidth,BLACK);
		PrtVLine(py,y2+BarStruct.QuietZone,x2-BarStruct.BearerBarWidth,BarStruct.BearerBarWidth,BLACK);
#endif //USE_BEARER_BAR_ONE_LINE
	}
	PrtStruct.LMargin = backup_lmargin;
	PrtStruct.TMargin = backup_tmargin;
	//HYP 20040723 
	PrtStruct.NonInvModeTphAlign = backup_noninvmodetphalign;
}


void PrtPattern_Rot90(HUGEDATA INT8U *pattern,short dwr,short w,short h)
{
	INT8U dmask,smask,xmask;//,d1mask;
	short x,y,ix,iy,wx,sx,k;
	long dpos,spos,starty;
	char rx,ry;
	char opaque;
	short px, py;
#ifdef USE_SRAM_PRT_RENERING_BUF
    INT32U pSram = 0;
    INT8U tempSramBuff;
#endif

	px = PrtStruct.x + PrtStruct.LMargin;
	py = PrtStruct.y + PrtStruct.TMargin;
	rx = StrStruct.Magx;
	ry = StrStruct.Magy;

	//HYP 20040723
	if(PrtDrvStruct.PrtDirection == NON_INVERT) {
		px = px + PrtStruct.NonInvModeTphAlign;
	}

	// Check Boundary 
	if (CheckBoundary(px,py,px+h*ry,py+w*rx) == FALSE) {
		//return;
		//HYP 20040226
		h = (h*ry - CheckBoundaryErrX(px,px+h*ry))/ry;
		w = (w*rx - CheckBoundaryErrY(py,py+w*rx))/rx;
		if(w <= 0) return;
		if(h <= 0) return;
	}
	opaque = PrtStruct.Opaque;
	spos  = 0;
	starty  = (long)py*PrtStruct.PrintableWidthBytes;
	k     = (px+h*ry-1);
	x = k&0x07;
	dmask= 0x80>>x;

    #ifdef USE_TEST_PRT_RENDERING_TIME
    tempCount_rot90++;
    #endif
	for (y=0; y<h; y++) {
	    for (iy=0; iy<ry; iy++) {	
			smask=0x80;
 			wx=k>>3;	  
			dpos=starty;
			for (x=0; x<w; x++) {
				sx=x>>3;
				for (ix=0; ix<rx; ix++) {
					if (pattern[spos+sx]&smask) {
                    #ifdef USE_SRAM_PRT_RENERING_BUF
                        pSram = SramPrtMemory + ( dpos+wx );
                        SramReadData_Byte( pSram, &tempSramBuff );
                        tempSramBuff |= dmask;
                        SramWriteData_Byte( pSram, tempSramBuff );
                    #else // USE_SRAM_PRT_RENERING_BUF
						PrtStruct.PrtMemory[dpos+wx] |= dmask;
                    #endif // mUSE_SRAM_PRT_RENERING_BUF
					} else {
						if (opaque) {
							xmask=~dmask;
                        #ifdef USE_SRAM_PRT_RENERING_BUF
                            pSram = SramPrtMemory + ( dpos+wx );
                            SramReadData_Byte( pSram, &tempSramBuff );
                            tempSramBuff &= xmask;
                            SramWriteData_Byte( pSram, tempSramBuff );
                        #else // USE_SRAM_PRT_RENERING_BUF
							PrtStruct.PrtMemory[dpos+wx] &= xmask;
                        #endif // mUSE_SRAM_PRT_RENERING_BUF
						}
					}
					dpos +=PrtStruct.PrintableWidthBytes;
				}
				smask>>=1; if (smask==0) smask=0x80;
			}
			dmask<<=1; if (dmask==0) dmask=0x01;
			k--;
	    }	
		spos +=dwr;
	}
}


void PrtPattern_Rot270(HUGEDATA INT8U *pattern,short dwr,short w,short h)
{
	INT8U dmask,smask,xmask;
	short x,y,ix,iy,wx,sx,k;
	long dpos,spos,starty;
	char rx,ry;
	char opaque;
	short px, py;
#ifdef USE_SRAM_PRT_RENERING_BUF
    INT32U pSram = 0;
    INT8U tempSramBuff;
#endif

	px = PrtStruct.x + PrtStruct.LMargin;
	py = PrtStruct.y + PrtStruct.TMargin;
	rx = StrStruct.Magx;
	ry = StrStruct.Magy;

	//HYP 20040723
	if(PrtDrvStruct.PrtDirection == NON_INVERT) {
		px = px + PrtStruct.NonInvModeTphAlign;
	}

	// Check Boundary 
	if (CheckBoundary(px,py,px+h*ry,py+w*rx) == FALSE) {
		//return;
		//HYP 20040226
		h = (h*ry - CheckBoundaryErrX(px,px+h*ry))/ry;
		w = (w*rx - CheckBoundaryErrY(py,py+w*rx))/rx;
		if(w <= 0) return;
		if(h <= 0) return;
	}
	opaque = PrtStruct.Opaque;
	spos  = 0;
	y = py+w*rx-1; 
	starty  = (long)y*PrtStruct.PrintableWidthBytes;
	k = px;
	x = px&0x07;
	dmask= 0x80>>x;

#ifdef USE_TEST_PRT_RENDERING_TIME
        tempCount_rot270++;
#endif

	for (y=0; y<h; y++) {
	    for (iy=0; iy<ry; iy++) {	  
			smask=0x80;
 			wx=k>>3;		
			dpos=starty;
			for (x=0; x<w; x++) {
				sx=x>>3;
				for (ix=0; ix<rx; ix++) {
					if (pattern[spos+sx]&smask) {
                    #ifdef USE_SRAM_PRT_RENERING_BUF
                        pSram = SramPrtMemory + ( dpos+wx );
                        SramReadData_Byte( pSram, &tempSramBuff );
                        tempSramBuff |= dmask;
                        SramWriteData_Byte( pSram, tempSramBuff );
                    #else // USE_SRAM_PRT_RENERING_BUF
						PrtStruct.PrtMemory[dpos+wx] |= dmask;
                    #endif // USE_SRAM_PRT_RENERING_BUF
					} else {
						if (opaque) {
							xmask=~dmask;
                        #ifdef USE_SRAM_PRT_RENERING_BUF
                            pSram = SramPrtMemory + ( dpos+wx );
                            SramReadData_Byte( pSram, &tempSramBuff );
                            tempSramBuff &= xmask;
                            SramWriteData_Byte( pSram, tempSramBuff );
                        #else // USE_SRAM_PRT_RENERING_BUF
							PrtStruct.PrtMemory[dpos+wx] &= xmask;
                        #endif // mUSE_SRAM_PRT_RENERING_BUF
						}
					}
					dpos -=PrtStruct.PrintableWidthBytes;
				}
				smask>>=1; if (smask==0) smask=0x80;
			}
			dmask>>=1; if (dmask==0) dmask=0x80;
			k++;		
	 	}  
	    spos +=dwr;
	}
}


//pattern must be even addr.
void PrtPattern_Rot0(HUGEDATA INT8U *pattern,short dwr8,short w,short h)		  //24 x 5 = 120 (signed char -128 ~ 127)
{
	short    ix,iy,dmw16,rmw16,imx16,dw16,rw16;
	long     pos16,ipos16;
	INT8U	dx16,rx16,nrx16;
	INT16U  cbt, cbr, cbl, cbn, cbm;
	ROMDATA INT8U *mag_X, *mag_Xpcbxmx, *mag_Xpcbxmx2;
	char mx,my;
	short i,j,k;//short
	short spos,mlwb16;   //
	short lwidthbyte16;
	char opaque;//
	HUGEDATA INT16U *prt_memory, *p_pattern16,*prt_memory2;
	HUGEDATA INT8U *pattern8,*pattern82;
	CWD	cb;
	char odd;
	INT16U	pattern16[LINE_BUF_SIZE/2],pattern1682[LINE_BUF_SIZE/2];
	short px, py;
#ifdef USE_SRAM_PRT_RENERING_BUF
    INT32U pSram = 0;
    INT32U pSram2 = 0;
    INT8U pTempSramBuff[2];
#endif

	px = PrtStruct.x + PrtStruct.LMargin;
	py = PrtStruct.y + PrtStruct.TMargin;	
	if(w == 0) return; // if opaque is on, blank is printed.
					   // because, if rx != 0, first byte is printed.

	mx = StrStruct.Magx;
	my = StrStruct.Magy;

	//HYP 20040723
	if(PrtDrvStruct.PrtDirection == NON_INVERT) {
		px = px + PrtStruct.NonInvModeTphAlign;
	}

	// Check Boundary 
	if (CheckBoundary(px,py,px+w*mx,py+h*my) == FALSE) {
		//return;
		//HYP 20040226
		w = (w*mx - CheckBoundaryErrX(px,px+w*mx))/mx;
		h = (h*my - CheckBoundaryErrY(py,py+h*my))/my;
		if(w <= 0) return;
		if(h <= 0) return;
	}
    lwidthbyte16 = PrtStruct.PrintableWidthBytes>>1;
	opaque = PrtStruct.Opaque;

	dx16 = px >> 4;
	rx16 = px & 0x0f;

	dmw16 = (w * mx) >>4;
	rmw16 = (w * mx) & 0x0f;
	if (rmw16 != 0) dmw16++;

	dw16 = w  >>4;
	rw16 = w  & 0x0f;
	if (rw16 != 0) dw16++;

	odd = dwr8 & 0x01;

	pattern82 = (HUGEDATA INT8U *)pattern1682;
	pattern8 = (HUGEDATA INT8U *)pattern16;

	pos16=(long)py*lwidthbyte16;

	spos = 0;
	ipos16= pos16 + dx16;
    mlwb16 = my * lwidthbyte16;

#ifdef USE_TEST_PRT_RENDERING_TIME
    tempCount_rot0++;
#endif
    
	if (rx16 != 0) {
		nrx16 = 16 - rx16;
		for (j = 0; j < h; j++) {
			imx16=0;
        #ifdef USE_SRAM_PRT_RENERING_BUF
            pSram = SramPrtMemory + (ipos16*2);
        #else //  USE_SRAM_PRT_RENERING_BUF
			prt_memory = (HUGEDATA INT16U *)PrtStruct.PrtMemory+ipos16;	 //
		#endif //  USE_SRAM_PRT_RENERING_BUF
			cbl = 0x0000;
			if(odd){
				switch(dwr8) {
					case 1:
						pattern82[0]= pattern[spos];  //odd
						pattern82[1]= 0x00;	//	caution!! clear garbage data 
				   		break;
					case 3:
						pattern82[0]= pattern[spos];  //odd
						pattern82[1]= pattern[spos+1];	//even
						pattern82[2]= pattern[spos+2];	//
						pattern82[3]= 0x00;	//
						break;
					default:
						for(k=0;k<dwr8;k++) {
							pattern82[k]= pattern[spos+k];	//even
						}
						pattern82[k]= 0x00;	//
						break;
				}
				p_pattern16 = pattern1682;
			} else {
				p_pattern16 = (HUGEDATA INT16U *)&pattern[spos];
			}
		
		   for (i = 0; i < (dw16 + 1); i++) {
				cb.word = p_pattern16[i];
				switch (mx) {
					case 2:
						pattern16[0] = *((ROMDATA INT16U *)*CnvTableMagX2 + (INT16U)cb.byte.right);
						pattern16[1] = *((ROMDATA INT16U *)*CnvTableMagX2 + (INT16U)cb.byte.left);
						break;
					default:
						mag_X = Inline_magX(mx); //
						mag_Xpcbxmx = mag_X + ((INT16U)cb.byte.right * mx);
						mag_Xpcbxmx2 = mag_X + ((INT16U)cb.byte.left * mx);
						for (ix = 0; ix < mx; ix++){
							pattern8[ix] = mag_Xpcbxmx[ix];
							pattern8[ix+mx] = mag_Xpcbxmx2[ix];
						}
						break;
				}
				for (ix = 0; ix < mx; ix++){
                #ifdef USE_SRAM_PRT_RENERING_BUF
                    pSram2 = pSram;
                #else
                    prt_memory2 = prt_memory;
                #endif
					if(imx16 < dmw16 - 1) {
						cbt = pattern16[ix];							
						cbt = Inline_Ror16(cbt, rx16);
						cbr = nlmask16[rx16] & cbt;
						cbn = cbl | cbr;
				  		cbl = lmask16[rx16] & cbt;
						if (opaque) {
							if(imx16 == 0) {
								cbm = lmask16[rx16] | cbn;
								for (iy = 0; iy < my; iy++) {
                                #ifdef USE_SRAM_PRT_RENERING_BUF
//	                                    pTempSramBuff = (INT8U *)malloc(2);
                                    SramReadData( pSram2, pTempSramBuff, 2 );
                                    prt_memory2 = (HUGEDATA INT16U *)pTempSramBuff;
                                #endif
									*prt_memory2 |= cbn;
									*prt_memory2 &= cbm;
                                #ifdef USE_SRAM_PRT_RENERING_BUF
                                    SramWriteData( pSram2, pTempSramBuff, 2 );
//	                                    free( pTempSramBuff );
                                    pSram2 = pSram2 + (lwidthbyte16*2);
                                #else
                                    prt_memory2 = prt_memory2 + lwidthbyte16;
                                #endif
								}
							} else {
								for (iy = 0; iy < my; iy++) {
                                #ifdef USE_SRAM_PRT_RENERING_BUF
                                    SramReadData( pSram2, pTempSramBuff, 2 );
                                    prt_memory2 = (HUGEDATA INT16U *)pTempSramBuff;
                                #endif
									*prt_memory2 = cbn;
                                #ifdef USE_SRAM_PRT_RENERING_BUF
                                    SramWriteData( pSram2, pTempSramBuff, 2 );
                                    pSram2 = pSram2 + (lwidthbyte16*2);
                                #else
                                    prt_memory2 = prt_memory2 + lwidthbyte16;
                                #endif
								}
							}
						} else {  
							for (iy = 0; iy < my; iy++) {
                            #ifdef USE_SRAM_PRT_RENERING_BUF
                                SramReadData( pSram2, pTempSramBuff, 2 );
                                prt_memory2 = (HUGEDATA INT16U *)pTempSramBuff;
                            #endif
								*prt_memory2 |= cbn;
                            #ifdef USE_SRAM_PRT_RENERING_BUF
                                SramWriteData( pSram2, pTempSramBuff, 2 );
                                pSram2 = pSram2 + (lwidthbyte16*2);
                            #else
                                prt_memory2 = prt_memory2 + lwidthbyte16;
                            #endif
							}
						}		
					} else if(imx16 == dmw16 - 1) {
						if((nrx16>=rmw16) && (rmw16 != 0)) {
							cbt = pattern16[ix];
							cbt = Inline_Ror16(cbt, rx16);
							cbr = nlmask16[rx16] & cbt;
							cbn = cbl | cbr;
							if (opaque) {
								if(imx16 == 0) {
									cbm = lmask16[rx16] | cbn; 
									cbm = nlmask16[rx16+rmw16] | cbm;  
									for (iy = 0; iy < my; iy++) {
                                    #ifdef USE_SRAM_PRT_RENERING_BUF
                                        SramReadData( pSram2, pTempSramBuff, 2 );
                                        prt_memory2 = (HUGEDATA INT16U *)pTempSramBuff;
                                    #endif
										*prt_memory2 |= cbn;
										*prt_memory2 &= cbm;
                                    #ifdef USE_SRAM_PRT_RENERING_BUF
                                        SramWriteData( pSram2, pTempSramBuff, 2 );
                                        pSram2 = pSram2 + (lwidthbyte16*2);
                                    #else
                                        prt_memory2 = prt_memory2 + lwidthbyte16;
                                    #endif
									}
								} else {
									cbm = nlmask16[rx16+rmw16] | cbn;
									for (iy = 0; iy < my; iy++) {
                                    #ifdef USE_SRAM_PRT_RENERING_BUF
                                        SramReadData( pSram2, pTempSramBuff, 2 );
                                        prt_memory2 = (HUGEDATA INT16U *)pTempSramBuff;
                                    #endif
										*prt_memory2 |= cbn;
										*prt_memory2 &= cbm;
                                    #ifdef USE_SRAM_PRT_RENERING_BUF
                                        SramWriteData( pSram2, pTempSramBuff, 2 );
                                        pSram2 = pSram2 + (lwidthbyte16*2);
                                    #else
                                        prt_memory2 = prt_memory2 + lwidthbyte16;
                                    #endif
									}
								}
							} else {	 
								for (iy = 0; iy < my; iy++) {
                                #ifdef USE_SRAM_PRT_RENERING_BUF
                                    SramReadData( pSram2, pTempSramBuff, 2 );
                                    prt_memory2 = (HUGEDATA INT16U *)pTempSramBuff;
                                #endif
									*prt_memory2 |= cbn;
                                #ifdef USE_SRAM_PRT_RENERING_BUF
                                    SramWriteData( pSram2, pTempSramBuff, 2 );
                                    pSram2 = pSram2 + (lwidthbyte16*2);
                                #else
                                    prt_memory2 = prt_memory2 + lwidthbyte16;
                                #endif
								}
							}	 
						} else {
							cbt = pattern16[ix];							
							cbt = Inline_Ror16(cbt, rx16);
							cbr = nlmask16[rx16] & cbt;
							cbn = cbl | cbr;
					  		cbl = lmask16[rx16] & cbt;
 							if (opaque) {	
								if(imx16 == 0) {
									cbm = lmask16[rx16] | cbn;
									for (iy = 0; iy < my; iy++) {
                                    #ifdef USE_SRAM_PRT_RENERING_BUF
                                        SramReadData( pSram2, pTempSramBuff, 2 );
                                        prt_memory2 = (HUGEDATA INT16U *)pTempSramBuff;
                                    #endif
										*prt_memory2 |= cbn;
										*prt_memory2 &= cbm;
                                    #ifdef USE_SRAM_PRT_RENERING_BUF
                                        SramWriteData( pSram2, pTempSramBuff, 2 );
                                        pSram2 = pSram2 + (lwidthbyte16*2);
                                    #else
                                        prt_memory2 = prt_memory2 + lwidthbyte16;
                                    #endif
									}
								} else {
									for (iy = 0; iy < my; iy++) {
                                    #ifdef USE_SRAM_PRT_RENERING_BUF
                                        SramReadData( pSram2, pTempSramBuff, 2 );
                                        prt_memory2 = (HUGEDATA INT16U *)pTempSramBuff;
                                    #endif
										*prt_memory2 = cbn;
                                    #ifdef USE_SRAM_PRT_RENERING_BUF
                                        SramWriteData( pSram2, pTempSramBuff, 2 );
                                        pSram2 = pSram2 + (lwidthbyte16*2);
                                    #else
                                        prt_memory2 = prt_memory2 + lwidthbyte16;
                                    #endif
									}
								}
							} else {	
								for (iy = 0; iy < my; iy++) {
                                #ifdef USE_SRAM_PRT_RENERING_BUF
                                    SramReadData( pSram2, pTempSramBuff, 2 );
                                    prt_memory2 = (HUGEDATA INT16U *)pTempSramBuff;
                                #endif
									*prt_memory2 |= cbn;
                                #ifdef USE_SRAM_PRT_RENERING_BUF
                                    SramWriteData( pSram2, pTempSramBuff, 2 );
                                    pSram2 = pSram2 + (lwidthbyte16*2);
                                #else
                                    prt_memory2 = prt_memory2 + lwidthbyte16;
                                #endif
								}
							}	   
						}
					} else if (imx16 == dmw16) {
						if ((nrx16<rmw16) || (rmw16 == 0)) {
							cbn = cbl ;
							if (opaque) {	
								if(rmw16 == 0) {
									cbm = nlmask16[rx16] | cbn;
									for (iy = 0; iy < my; iy++) {
                                    #ifdef USE_SRAM_PRT_RENERING_BUF
                                        SramReadData( pSram2, pTempSramBuff, 2 );
                                        prt_memory2 = (HUGEDATA INT16U *)pTempSramBuff;
                                    #endif
										*prt_memory2 |= cbn; 
										*prt_memory2 &= cbm;
                                    #ifdef USE_SRAM_PRT_RENERING_BUF
                                        SramWriteData( pSram2, pTempSramBuff, 2 );
                                        pSram2 = pSram2 + (lwidthbyte16*2);
                                    #else
                                        prt_memory2 = prt_memory2 + lwidthbyte16;
                                    #endif
									}
								} else {
									cbm = nlmask16[rmw16-nrx16] | cbn;
									for (iy = 0; iy < my; iy++) {
                                    #ifdef USE_SRAM_PRT_RENERING_BUF
                                        SramReadData( pSram2, pTempSramBuff, 2 );
                                        prt_memory2 = (HUGEDATA INT16U *)pTempSramBuff;
                                    #endif
										*prt_memory2 |= cbn; 
										*prt_memory2 &= cbm;
                                    #ifdef USE_SRAM_PRT_RENERING_BUF
                                        SramWriteData( pSram2, pTempSramBuff, 2 );
                                        pSram2 = pSram2 + (lwidthbyte16*2);
                                    #else
                                        prt_memory2 = prt_memory2 + lwidthbyte16;
                                    #endif
									}
								}
							} else {	
								for (iy = 0; iy < my; iy++) {
                                #ifdef USE_SRAM_PRT_RENERING_BUF
                                    SramReadData( pSram2, pTempSramBuff, 2 );
                                    prt_memory2 = (HUGEDATA INT16U *)pTempSramBuff;
                                #endif
									*prt_memory2 |= cbn;
                                #ifdef USE_SRAM_PRT_RENERING_BUF
                                    SramWriteData( pSram2, pTempSramBuff, 2 );
                                    pSram2 = pSram2 + (lwidthbyte16*2);
                                #else
                                    prt_memory2 = prt_memory2 + lwidthbyte16;
                                #endif
								}
							}		  
						}
						break;
					}						 
					imx16++;
                #ifdef USE_SRAM_PRT_RENERING_BUF
                    pSram = pSram + 2;
                #else
					prt_memory++;
                #endif
				}
			}
            ipos16 = ipos16 + mlwb16;
			spos = spos + dwr8;
		}
	} else {
		for (j = 0; j < h; j++) {
			imx16=0;
        #ifdef USE_SRAM_PRT_RENERING_BUF
            pSram = SramPrtMemory + (ipos16*2);
        #else //  USE_SRAM_PRT_RENERING_BUF
			prt_memory = (HUGEDATA INT16U *)PrtStruct.PrtMemory+ipos16;	 //
		#endif //  USE_SRAM_PRT_RENERING_BUF
			if(odd){
				switch(dwr8) {
					case 1:
						pattern82[0]= pattern[spos];  //odd
						pattern82[1]= 0x00;	//	caution!! clear garbage data 
				   		break;
					case 3:
						pattern82[0]= pattern[spos];  //odd
						pattern82[1]= pattern[spos+1];	//even
						pattern82[2]= pattern[spos+2];	//
						pattern82[3]= 0x00;	//
						break;
					default:
						for(k=0;k<dwr8;k++) {
							pattern82[k]= pattern[spos+k];	//even
						}
						pattern82[k]= 0x00;	//
						break;
				}
				p_pattern16 = pattern1682;
			} else {
				p_pattern16 = (HUGEDATA INT16U *)&pattern[spos];
			}
	
			for (i= 0; i < dw16; i++) {
				cb.word = p_pattern16[i];
				switch (mx) {
					case 2:
						pattern16[0] = *((ROMDATA INT16U *)*CnvTableMagX2 + (INT16U)cb.byte.right);
						pattern16[1] = *((ROMDATA INT16U *)*CnvTableMagX2 + (INT16U)cb.byte.left);
						break;
					default:
						mag_X = Inline_magX(mx); //
						mag_Xpcbxmx = mag_X + ((INT16U)cb.byte.right * mx);
						mag_Xpcbxmx2 = mag_X + ((INT16U)cb.byte.left * mx);
						for (ix = 0; ix < mx; ix++){
							pattern8[ix] = mag_Xpcbxmx[ix];
							pattern8[ix+mx] = mag_Xpcbxmx2[ix];
						}
						break;
				}				
				for (ix = 0; ix < mx; ix++){
  				#ifdef USE_SRAM_PRT_RENERING_BUF
                    pSram2 = pSram;
                #else
                    prt_memory2 = prt_memory;
                #endif
					if (imx16 < dmw16 - 1) {
						cbt = pattern16[ix];
						if (opaque) {
							for (iy = 0; iy < my; iy++) {
                            #ifdef USE_SRAM_PRT_RENERING_BUF
                                SramReadData( pSram2, pTempSramBuff, 2 );
                                prt_memory2 = (HUGEDATA INT16U *)pTempSramBuff;
                            #endif
								*prt_memory2 = cbt;
                            #ifdef USE_SRAM_PRT_RENERING_BUF
                                SramWriteData( pSram2, pTempSramBuff, 2 );
                                pSram2 = pSram2 + (lwidthbyte16*2);
                            #else
                                prt_memory2 = prt_memory2 + lwidthbyte16;
                            #endif
							}
						} else {		  
							for (iy = 0; iy < my; iy++) {
                            #ifdef USE_SRAM_PRT_RENERING_BUF
                                SramReadData( pSram2, pTempSramBuff, 2 );
                                prt_memory2 = (HUGEDATA INT16U *)pTempSramBuff;
                            #endif
								*prt_memory2 |= cbt;
                            #ifdef USE_SRAM_PRT_RENERING_BUF
                                SramWriteData( pSram2, pTempSramBuff, 2 );
                                pSram2 = pSram2 + (lwidthbyte16*2);
                            #else
                                prt_memory2 = prt_memory2 + lwidthbyte16;
                            #endif
							}
						}	  
					} else if (imx16 == dmw16 - 1) {
						cbt = pattern16[ix];
						if (opaque) {
							if(rmw16 != 0) {
								cbm = nlmask16[rmw16] | cbt;
								for (iy = 0; iy < my; iy++) {
                                #ifdef USE_SRAM_PRT_RENERING_BUF
                                    SramReadData( pSram2, pTempSramBuff, 2 );
                                    prt_memory2 = (HUGEDATA INT16U *)pTempSramBuff;
                                #endif
									*prt_memory2 |= cbt;
									*prt_memory2 &= cbm;
                                #ifdef USE_SRAM_PRT_RENERING_BUF
                                    SramWriteData( pSram2, pTempSramBuff, 2 );
                                    pSram2 = pSram2 + (lwidthbyte16*2);
                                #else
                                    prt_memory2 = prt_memory2 + lwidthbyte16;
                                #endif
								}
							} else {
								for (iy = 0; iy < my; iy++) {
                                #ifdef USE_SRAM_PRT_RENERING_BUF
                                    SramReadData( pSram2, pTempSramBuff, 2 );
                                    prt_memory2 = (HUGEDATA INT16U *)pTempSramBuff;
                                #endif
									*prt_memory2 = cbt;
                                #ifdef USE_SRAM_PRT_RENERING_BUF
                                    SramWriteData( pSram2, pTempSramBuff, 2 );
                                    pSram2 = pSram2 + (lwidthbyte16*2);
                                #else
                                    prt_memory2 = prt_memory2 + lwidthbyte16;
                                #endif
								}
							}	 
						} else { 
							for (iy = 0; iy < my; iy++) {
                            #ifdef USE_SRAM_PRT_RENERING_BUF
                                SramReadData( pSram2, pTempSramBuff, 2 );
                                prt_memory2 = (HUGEDATA INT16U *)pTempSramBuff;
                            #endif
								*prt_memory2 |= cbt;
                            #ifdef USE_SRAM_PRT_RENERING_BUF
                                SramWriteData( pSram2, pTempSramBuff, 2 );
                                pSram2 = pSram2 + (lwidthbyte16*2);
                            #else
                                prt_memory2 = prt_memory2 + lwidthbyte16;
                            #endif
							}
						}
						break;	 
					} 
					imx16++;
                #ifdef USE_SRAM_PRT_RENERING_BUF
                    pSram = pSram + 2;
                #else
					prt_memory++;
                #endif
				}
	   		}
			ipos16 = ipos16 + mlwb16;
			spos = spos + dwr8;
		}
	}
}


//pattern must be even addr.
void PrtPattern_Rot0_Mag1(HUGEDATA INT8U *pattern,short dwr8,short w,short h)		  //24 x 5 = 120 (signed char -128 ~ 127)
{
	short    dx16,dw16;
	long     pos16,ipos16;
	INT8U	rx16, rw16,nrx16;// cb,cbr, cbl, cbn, cbm;
	INT16U 	 cb,cbr, cbl, cbn, cbm;
	short i,j,k;//short
	short spos,mlwb16;   //
	char opaque;//
	char odd;
	HUGEDATA INT16U *prt_memory,*pattern16;
	HUGEDATA INT8U *pattern8;
	INT16U	pattern168[LINE_BUF_SIZE/2];
	short px, py;
#ifdef USE_SRAM_PRT_RENERING_BUF
    INT32U pSram = 0;
    INT8U *pTempSramBuff;
    INT32U tempSize = 0;
#endif
#ifdef USE_TEST_TMP_PRTBUF_MAX_SIZE
    static INT32U maxSizeTmpPrtBuf = 0;
#endif

	px = PrtStruct.x + PrtStruct.LMargin;
	py = PrtStruct.y + PrtStruct.TMargin;

	//HYP 20040723
	if(PrtDrvStruct.PrtDirection == NON_INVERT) {
		px = px + PrtStruct.NonInvModeTphAlign;
	}

	// Check Boundary 
	if (CheckBoundary(px,py,px+w,py+h) == FALSE) {
		//return;
		//HYP 20040226
		w = (w - CheckBoundaryErrX(px,px+w));
		h = (h - CheckBoundaryErrY(py,py+h));
		if(w <= 0) return;
		if(h <= 0) return;
	}
	if(w == 0) return; // if opaque is on, blank is printed.
					   // because, if rx != 0, first byte is printed.

	opaque = PrtStruct.Opaque;

 	dx16 = px >> 4;
	rx16 = px & 0x0f;

  	dw16 = w >>4;
	rw16 = w & 0x0f;
	if (rw16 != 0) dw16++;

	odd = dwr8&0x01;
	pattern8 = (HUGEDATA INT8U *)pattern168;

	pos16=(long)py*(PrtStruct.PrintableWidthBytes>>1);

	spos = 0;
	ipos16 = pos16 + dx16;
	mlwb16 = PrtStruct.PrintableWidthBytes>>1;

#ifdef USE_SRAM_PRT_RENERING_BUF
    tempCount_rot0m++;
#endif

	if (rx16 != 0) {
		nrx16 = 16 - rx16;
		for (j = 0; j < h; j++) {
        #ifdef USE_SRAM_PRT_RENERING_BUF
            pSram = SramPrtMemory + (ipos16*2);
            tempSize = 2 * (dw16 + 1);// * 2;
            pTempSramBuff = (char *)malloc(tempSize);
            SramReadData( pSram, pTempSramBuff, tempSize );
            prt_memory = (HUGEDATA INT16U *) pTempSramBuff;
            #ifdef USE_TEST_TMP_PRTBUF_MAX_SIZE
                if( tempSize > maxSizeTmpPrtBuf )
                {
                    maxSizeTmpPrtBuf = tempSize;
                }
            #endif
        #else //  USE_SRAM_PRT_RENERING_BUF
            prt_memory = (HUGEDATA INT16U *)PrtStruct.PrtMemory+ipos16;  //
        #endif //  USE_SRAM_PRT_RENERING_BUF
			cbl = 0x0000;
			if(odd){
				switch(dwr8) {
					case 1:
						pattern8[0]= pattern[spos];
						pattern8[1]= 0x00;	//	caution!! clear garbage data 
				   		break;
					case 3:
						pattern8[0]= pattern[spos];
						pattern8[1]= pattern[spos+1];
						pattern8[2]= pattern[spos+2];
						pattern8[3]= 0x00;	//
						break;
					default:
						for(k=0;k<dwr8;k++) {
							pattern8[k]= pattern[spos+k];
						}
						pattern8[k]= 0x00;	//
						break;
				}
				pattern16 = pattern168;
			} else {
				pattern16 = (HUGEDATA INT16U *)&pattern[spos];
			}
			
		    for (i = 0; i < (dw16 + 1); i++) {
				cb = pattern16[i];	
				if(i < dw16 - 1) {		
					cb = Inline_Ror16(cb, rx16);
					cbr = nlmask16[rx16] & cb;
					cbn = cbl | cbr;
					cbl = lmask16[rx16] & cb;
					if (opaque) {
						if(i == 0) {
							cbm = lmask16[rx16] | cbn;
							*prt_memory |= cbn;
							*prt_memory &= cbm;
						} else {
							*prt_memory = cbn;
						}
					} else {  
						*prt_memory |= cbn;	  
					}		
				} else if(i == dw16 - 1) {
					if((nrx16>=rw16) && (rw16 != 0)) {
						cb = Inline_Ror16(cb, rx16);
						cbr = nlmask16[rx16] & cb;
						cbn = cbl | cbr;
						if (opaque) {
							if(i == 0) {
								cbm = lmask16[rx16] | cbn;
								cbm = nlmask16[rx16+rw16] | cbm;
							} else {
								cbm = nlmask16[rx16+rw16] | cbn;
							}
							*prt_memory |= cbn;
							*prt_memory &= cbm;
						} else {	 
							*prt_memory |= cbn;	  
						}	 
					} else {
						cb = Inline_Ror16(cb, rx16);
						cbr = nlmask16[rx16] & cb;
						cbn = cbl | cbr;
				  		cbl = lmask16[rx16] & cb;
 						if (opaque) {	
							if(i == 0) {
								cbm = lmask16[rx16] | cbn;
								*prt_memory |= cbn;
								*prt_memory &= cbm;
							} else {
								*prt_memory = cbn;	   
							}
						} else {	
							*prt_memory |= cbn;
						}	   
					}
				} else if (i == dw16) {
					if ((nrx16<rw16) || (rw16 == 0)) {
						cbn = cbl ;
						if (opaque) {	
							if(rw16 == 0) {
								cbm = nlmask16[rx16] | cbn;
							} else {
								cbm = nlmask16[rw16-nrx16] | cbn;
							}
							*prt_memory |= cbn; 
							*prt_memory &= cbm;
						} else {	
							*prt_memory |= cbn;	  
						}		  
					}
					break;
				}
                prt_memory++;
			}
			spos = spos + dwr8;
			ipos16 = ipos16 + mlwb16;
        #ifdef USE_SRAM_PRT_RENERING_BUF
            SramWriteData( pSram, pTempSramBuff, tempSize );
            free( pTempSramBuff );
        #endif
		}
	} else {
		for (j = 0; j < h; j++) {
        #ifdef USE_SRAM_PRT_RENERING_BUF
            pSram = SramPrtMemory + (ipos16*2);
            tempSize = 2 * (dw16 + 1);// * 2;
            pTempSramBuff = (char *)malloc(tempSize);
            SramReadData( pSram, pTempSramBuff, tempSize );
            prt_memory = (HUGEDATA INT16U *) pTempSramBuff;
            #ifdef USE_TEST_TMP_PRTBUF_MAX_SIZE
                if( tempSize > maxSizeTmpPrtBuf )
                {
                    maxSizeTmpPrtBuf = tempSize;
                }
            #endif
        #else //  USE_SRAM_PRT_RENERING_BUF
            prt_memory = (HUGEDATA INT16U *)PrtStruct.PrtMemory+ipos16;  //
        #endif //  USE_SRAM_PRT_RENERING_BUF
			if(odd){
				switch(dwr8) {
					case 1:
						pattern8[0]= pattern[spos];
						pattern8[1]= 0x00;	//	caution!! clear garbage data 
				   		break;
					case 3:
						pattern8[0]= pattern[spos];
						pattern8[1]= pattern[spos+1];
						pattern8[2]= pattern[spos+2];
						pattern8[3]= 0x00;	//
						break;
					default:
						for(k=0;k<dwr8;k++) {
							pattern8[k]= pattern[spos+k];
						}
						pattern8[k]= 0x00;	//
						break;
				}
				pattern16 = pattern168;
			} else {
				pattern16 = (HUGEDATA INT16U *)&pattern[spos];
			}
	
			for (i= 0; i < dw16; i++) {
                cb = pattern16[i];	
				if (i < dw16 - 1) {
					if (opaque) {
						*prt_memory = cb;	 
					} else {		  
						*prt_memory |= cb;	
					}	  
				} else if (i == dw16 - 1) {
					if (opaque) {
						if(rw16 != 0) {
							cbm = nlmask16[rw16] | cb;
							*prt_memory |= cb;
							*prt_memory &= cbm;
						} else {
							*prt_memory = cb;	
						}
					} else { 
						*prt_memory |= cb;		 
					}
					break;	 
				} 
                prt_memory++;
	   		}
			spos = spos + dwr8;
			ipos16 = ipos16 + mlwb16;
        #ifdef USE_SRAM_PRT_RENERING_BUF
            SramWriteData( pSram, pTempSramBuff, tempSize );
            free( pTempSramBuff );
        #endif //  USE_SRAM_PRT_RENERING_BUF
		}
	}
    #ifdef USE_TEST_TMP_PRTBUF_MAX_SIZE
        Flash_lwrite( DFLASH_BASE + FLASH_TMP_TEST_RESULT_AREA, maxSizeTmpPrtBuf );
    #endif
}


//pattern must be even addr.
void PrtPattern_Rot180(HUGEDATA INT8U *pattern,short dwr8,short w,short h)		  //24 x 5 = 120 (signed char -128 ~ 127)
{
	short    ix,iy,dmw16,rmw16,imx16,dw16,rw16;
	long     pos16,ipos16;
	INT8U	dx16,rx16,nrx16;
	INT16U  cbt, cbr, cbl, cbn, cbm;
	ROMDATA INT8U *mag_X, *mag_Xpcbxmx, *mag_Xpcbxmx2;
	INT8U	tmppattern[LINE_BUF_SIZE];	  //maximun 60mm pad
	char mx,my;
	short i,j,k;//short
	short rspos,mlwb16;   //
	short lwidthbyte16;
	char opaque;//
	HUGEDATA INT16U *prt_memory, *p_pattern16,*prt_memory2;
	HUGEDATA INT8U *pattern8,*pattern82;
	CWD	cb;
	char odd;
	INT16U	pattern16[LINE_BUF_SIZE/2],pattern1682[LINE_BUF_SIZE/2];
	short px, py;
#ifdef USE_SRAM_PRT_RENERING_BUF
    INT32U pSram = 0;
    INT32U pSram2 = 0;
    INT8U pTempSramBuff[2];
#endif

	px = PrtStruct.x + PrtStruct.LMargin;
	py = PrtStruct.y + PrtStruct.TMargin;	
	if(w == 0) return; 
	
	mx = StrStruct.Magx;
	my = StrStruct.Magy;

	//HYP 20040723
	if(PrtDrvStruct.PrtDirection == NON_INVERT) {
		px = px + PrtStruct.NonInvModeTphAlign;
	}
	// Check Boundary 
	if (CheckBoundary(px,py,px+w*mx,py+h*my) == FALSE) {
		//return;
		//HYP 20040226
		w = (w*mx - CheckBoundaryErrX(px,px+w*mx))/mx;
		h = (h*my - CheckBoundaryErrY(py,py+h*my))/my;
		if(w <= 0) return;
		if(h <= 0) return;
	}
	lwidthbyte16 = PrtStruct.PrintableWidthBytes>>1;
	opaque = PrtStruct.Opaque;

	dx16 = px >> 4;
	rx16 = px & 0x0f;

	dmw16 = (w * mx) >>4;
	rmw16 = (w * mx) & 0x0f;
	if (rmw16 != 0) dmw16++;

	dw16 = w  >>4;
	rw16 = w  & 0x0f;
	if (rw16 != 0) dw16++;

	odd = dwr8 & 0x01;
	pattern82 = (HUGEDATA INT8U *)pattern1682;
	pattern8 = (HUGEDATA INT8U *)pattern16;
	pos16=(long)py*lwidthbyte16;

	ipos16= pos16 + dx16;
	mlwb16 = my * lwidthbyte16;
	rspos = (h-1)*dwr8;

#ifdef USE_TEST_PRT_RENDERING_TIME
    tempCount_rot180++;
#endif

	if (rx16 != 0) {
		nrx16 = 16 - rx16;
		for (j = 0; j < h; j++) {
			imx16=0;
        #ifdef USE_SRAM_PRT_RENERING_BUF
            pSram = SramPrtMemory + (ipos16*2);
        #else //  USE_SRAM_PRT_RENERING_BUF
			prt_memory = (HUGEDATA INT16U *)PrtStruct.PrtMemory + ipos16;
        #endif //  USE_SRAM_PRT_RENERING_BUF
			cbl = 0x0000;
			PrtPattern_Swap_Cpy(tmppattern, &pattern[rspos], w);
			if(odd){
				switch(dwr8) {
					case 1:
						pattern82[0]= tmppattern[0];
						pattern82[1]= 0x00;	//	caution!! clear garbage data 
				   		break;
					case 3:
						pattern82[0]= tmppattern[0];
						pattern82[1]= tmppattern[1];
						pattern82[2]= tmppattern[2];
						pattern82[3]= 0x00;	//
						break;
					default:
						for(k=0;k<dwr8;k++) {
							pattern82[k]= tmppattern[k];
						}
						pattern82[k]= 0x00;	//
						break;
				}					
				p_pattern16 = pattern1682;
			} else {
				p_pattern16 = (HUGEDATA INT16U *)&tmppattern[0];
			}
		   for (i = 0; i < (dw16 + 1); i++) {
				cb.word = p_pattern16[i];
				switch (mx) {
					case 2:
						pattern16[0] = *((ROMDATA INT16U *)*CnvTableMagX2 + (INT16U)cb.byte.right);
						pattern16[1] = *((ROMDATA INT16U *)*CnvTableMagX2 + (INT16U)cb.byte.left);
						break;
					default:
						mag_X = Inline_magX(mx); //
						mag_Xpcbxmx = mag_X + ((INT16U)cb.byte.right * mx);
						mag_Xpcbxmx2 = mag_X + ((INT16U)cb.byte.left * mx);
						for (ix = 0; ix < mx; ix++){
							pattern8[ix] = mag_Xpcbxmx[ix];
							pattern8[ix+mx] = mag_Xpcbxmx2[ix];
						}
						break;
				}
				for (ix = 0; ix < mx; ix++){
				#ifdef USE_SRAM_PRT_RENERING_BUF
                    pSram2 = pSram;
                #else
                    prt_memory2 = prt_memory;
                #endif
					if(imx16 < dmw16 - 1) {
						cbt = pattern16[ix];							
						cbt = Inline_Ror16(cbt, rx16);
						cbr = nlmask16[rx16] & cbt;
						cbn = cbl | cbr;
				  		cbl = lmask16[rx16] & cbt;
						if (opaque) {
							if(imx16 == 0) {
								cbm = lmask16[rx16] | cbn;
								for (iy = 0; iy < my; iy++) {
								#ifdef USE_SRAM_PRT_RENERING_BUF
//	                                    pTempSramBuff = (INT8U *)malloc(2);
                                    SramReadData( pSram2, pTempSramBuff, 2 );
                                    prt_memory2 = (HUGEDATA INT16U *)pTempSramBuff;
                                #endif
									*prt_memory2 |= cbn;
									*prt_memory2 &= cbm;
                                #ifdef USE_SRAM_PRT_RENERING_BUF
                                    SramWriteData( pSram2, pTempSramBuff, 2 );
//	                                    free( pTempSramBuff );
                                    pSram2 = pSram2 + (lwidthbyte16*2);
                                #else
                                    prt_memory2 = prt_memory2 + lwidthbyte16;
                                #endif
								}
							} else {
								for (iy = 0; iy < my; iy++) {
								#ifdef USE_SRAM_PRT_RENERING_BUF
                                    SramReadData( pSram2, pTempSramBuff, 2 );
                                    prt_memory2 = (HUGEDATA INT16U *)pTempSramBuff;
                                #endif
									*prt_memory2 = cbn;
                                #ifdef USE_SRAM_PRT_RENERING_BUF
                                    SramWriteData( pSram2, pTempSramBuff, 2 );
                                    pSram2 = pSram2 + (lwidthbyte16*2);
                                #else
                                    prt_memory2 = prt_memory2 + lwidthbyte16;
                                #endif
								}
							}
						} else {  
							for (iy = 0; iy < my; iy++) {
							#ifdef USE_SRAM_PRT_RENERING_BUF
                                SramReadData( pSram2, pTempSramBuff, 2 );
                                prt_memory2 = (HUGEDATA INT16U *)pTempSramBuff;
                            #endif
								*prt_memory2 |= cbn;
                            #ifdef USE_SRAM_PRT_RENERING_BUF
                                SramWriteData( pSram2, pTempSramBuff, 2 );
                                pSram2 = pSram2 + (lwidthbyte16*2);
                            #else
                                prt_memory2 = prt_memory2 + lwidthbyte16;
                            #endif
							}
						}		
					} else if(imx16 == dmw16 - 1) {
						if((nrx16>=rmw16) && (rmw16 != 0)) {
							cbt = pattern16[ix];
							cbt = Inline_Ror16(cbt, rx16);
							cbr = nlmask16[rx16] & cbt;
							cbn = cbl | cbr;
							if (opaque) {
								if(imx16 == 0) {
									cbm = lmask16[rx16] | cbn;
									cbm = nlmask16[rx16+rmw16] | cbm;
									for (iy = 0; iy < my; iy++) {
									#ifdef USE_SRAM_PRT_RENERING_BUF
                                        SramReadData( pSram2, pTempSramBuff, 2 );
                                        prt_memory2 = (HUGEDATA INT16U *)pTempSramBuff;
                                    #endif
										*prt_memory2 |= cbn;
										*prt_memory2 &= cbm;
                                    #ifdef USE_SRAM_PRT_RENERING_BUF
                                        SramWriteData( pSram2, pTempSramBuff, 2 );
                                        pSram2 = pSram2 + (lwidthbyte16*2);
                                    #else
                                        prt_memory2 = prt_memory2 + lwidthbyte16;
                                    #endif
									}
								} else {
									cbm = nlmask16[rx16+rmw16] | cbn;
									for (iy = 0; iy < my; iy++) {
									#ifdef USE_SRAM_PRT_RENERING_BUF
                                        SramReadData( pSram2, pTempSramBuff, 2 );
                                        prt_memory2 = (HUGEDATA INT16U *)pTempSramBuff;
                                    #endif
										*prt_memory2 |= cbn;
										*prt_memory2 &= cbm;
                                    #ifdef USE_SRAM_PRT_RENERING_BUF
                                        SramWriteData( pSram2, pTempSramBuff, 2 );
                                        pSram2 = pSram2 + (lwidthbyte16*2);
                                    #else
                                        prt_memory2 = prt_memory2 + lwidthbyte16;
                                    #endif
									}
								}
							} else {	 
								for (iy = 0; iy < my; iy++) {
								#ifdef USE_SRAM_PRT_RENERING_BUF
                                    SramReadData( pSram2, pTempSramBuff, 2 );
                                    prt_memory2 = (HUGEDATA INT16U *)pTempSramBuff;
                                #endif
									*prt_memory2 |= cbn;
                                #ifdef USE_SRAM_PRT_RENERING_BUF
                                    SramWriteData( pSram2, pTempSramBuff, 2 );
                                    pSram2 = pSram2 + (lwidthbyte16*2);
                                #else
                                    prt_memory2 = prt_memory2 + lwidthbyte16;
                                #endif
								}
							}	 
						} else {
							cbt = pattern16[ix];							
							cbt = Inline_Ror16(cbt, rx16);
							cbr = nlmask16[rx16] & cbt;
							cbn = cbl | cbr;
					  		cbl = lmask16[rx16] & cbt;
 							if (opaque) {	
								if(imx16 == 0) {
									cbm = lmask16[rx16] | cbn;
									for (iy = 0; iy < my; iy++) {
									#ifdef USE_SRAM_PRT_RENERING_BUF
                                        SramReadData( pSram2, pTempSramBuff, 2 );
                                        prt_memory2 = (HUGEDATA INT16U *)pTempSramBuff;
                                    #endif
										*prt_memory2 |= cbn;
										*prt_memory2 &= cbm;
                                    #ifdef USE_SRAM_PRT_RENERING_BUF
                                        SramWriteData( pSram2, pTempSramBuff, 2 );
                                        pSram2 = pSram2 + (lwidthbyte16*2);
                                    #else
                                        prt_memory2 = prt_memory2 + lwidthbyte16;
                                    #endif
									}
								} else {
									for (iy = 0; iy < my; iy++) {
									#ifdef USE_SRAM_PRT_RENERING_BUF
                                        SramReadData( pSram2, pTempSramBuff, 2 );
                                        prt_memory2 = (HUGEDATA INT16U *)pTempSramBuff;
                                    #endif
										*prt_memory2 = cbn;
                                    #ifdef USE_SRAM_PRT_RENERING_BUF
                                        SramWriteData( pSram2, pTempSramBuff, 2 );
                                        pSram2 = pSram2 + (lwidthbyte16*2);
                                    #else
                                        prt_memory2 = prt_memory2 + lwidthbyte16;
                                    #endif	   
									}
								}
							} else {	
								for (iy = 0; iy < my; iy++) {
								#ifdef USE_SRAM_PRT_RENERING_BUF
                                    SramReadData( pSram2, pTempSramBuff, 2 );
                                    prt_memory2 = (HUGEDATA INT16U *)pTempSramBuff;
                                #endif
									*prt_memory2 |= cbn;
                                #ifdef USE_SRAM_PRT_RENERING_BUF
                                    SramWriteData( pSram2, pTempSramBuff, 2 );
                                    pSram2 = pSram2 + (lwidthbyte16*2);
                                #else
                                    prt_memory2 = prt_memory2 + lwidthbyte16;
                                #endif
								}
							}	   
						}
					} else if (imx16 == dmw16) {
						if ((nrx16<rmw16) || (rmw16 == 0)) {
							cbn = cbl ;
							if (opaque) {	
								if(rmw16 == 0) {
									cbm = nlmask16[rx16] | cbn;
									for (iy = 0; iy < my; iy++) {
									#ifdef USE_SRAM_PRT_RENERING_BUF
                                        SramReadData( pSram2, pTempSramBuff, 2 );
                                        prt_memory2 = (HUGEDATA INT16U *)pTempSramBuff;
                                    #endif
										*prt_memory2 |= cbn; 
										*prt_memory2 &= cbm;
                                    #ifdef USE_SRAM_PRT_RENERING_BUF
                                        SramWriteData( pSram2, pTempSramBuff, 2 );
                                        pSram2 = pSram2 + (lwidthbyte16*2);
                                    #else
                                        prt_memory2 = prt_memory2 + lwidthbyte16;
                                    #endif
									}
								} else {
									cbm = nlmask16[rmw16-nrx16] | cbn;
									for (iy = 0; iy < my; iy++) {								
								    #ifdef USE_SRAM_PRT_RENERING_BUF
                                        SramReadData( pSram2, pTempSramBuff, 2 );
                                        prt_memory2 = (HUGEDATA INT16U *)pTempSramBuff;
                                    #endif
										*prt_memory2 |= cbn; 
										*prt_memory2 &= cbm;
                                    #ifdef USE_SRAM_PRT_RENERING_BUF
                                        SramWriteData( pSram2, pTempSramBuff, 2 );
                                        pSram2 = pSram2 + (lwidthbyte16*2);
                                    #else
                                        prt_memory2 = prt_memory2 + lwidthbyte16;
                                    #endif
									}
								}
							} else {	
								for (iy = 0; iy < my; iy++) {
								#ifdef USE_SRAM_PRT_RENERING_BUF
                                    SramReadData( pSram2, pTempSramBuff, 2 );
                                    prt_memory2 = (HUGEDATA INT16U *)pTempSramBuff;
                                #endif
									*prt_memory2 |= cbn;
                                #ifdef USE_SRAM_PRT_RENERING_BUF
                                    SramWriteData( pSram2, pTempSramBuff, 2 );
                                    pSram2 = pSram2 + (lwidthbyte16*2);
                                #else
                                    prt_memory2 = prt_memory2 + lwidthbyte16;
                                #endif
								}
							}		  
						}
						break;
					}						 
					imx16++;
				#ifdef USE_SRAM_PRT_RENERING_BUF
                    pSram = pSram + 2;
                #else
					prt_memory++;
                #endif
				}
			}
            ipos16 = ipos16 + mlwb16;
			rspos = rspos - dwr8;
		}
	} else {
		for (j = 0; j < h; j++) {
			imx16=0;
        #ifdef USE_SRAM_PRT_RENERING_BUF
            pSram = SramPrtMemory + (ipos16*2);
        #else //  USE_SRAM_PRT_RENERING_BUF
			prt_memory = (HUGEDATA INT16U *)PrtStruct.PrtMemory+ipos16;	 //
		#endif //  USE_SRAM_PRT_RENERING_BUF
			PrtPattern_Swap_Cpy(tmppattern, &pattern[rspos], w);
			if(odd){
				switch(dwr8) {
					case 1:
						pattern82[0]= tmppattern[0];
						pattern82[1]= 0x00;	//	caution!! clear garbage data 
				   		break;
					case 3:
						pattern82[0]= tmppattern[0];
						pattern82[1]= tmppattern[1];
						pattern82[2]= tmppattern[2];
						pattern82[3]= 0x00;	//
						break;
					default:
						for(k=0;k<dwr8;k++) {
							pattern82[k]= tmppattern[k];
						}
						pattern82[k]= 0x00;	//
						break;
				}
				p_pattern16 = pattern1682;
			} else {
				p_pattern16 = (HUGEDATA INT16U *)&tmppattern[0];
			}

			for (i= 0; i < dw16; i++) {
				cb.word = p_pattern16[i];
				switch (mx) {
					case 2:
						pattern16[0] = *((ROMDATA INT16U *)*CnvTableMagX2 + (INT16U)cb.byte.right);
						pattern16[1] = *((ROMDATA INT16U *)*CnvTableMagX2 + (INT16U)cb.byte.left);
						break;
					default:
						mag_X = Inline_magX(mx); //
						mag_Xpcbxmx = mag_X + ((INT16U)cb.byte.right * mx);
						mag_Xpcbxmx2 = mag_X + ((INT16U)cb.byte.left * mx);
						for (ix = 0; ix < mx; ix++){
							pattern8[ix] = mag_Xpcbxmx[ix];
							pattern8[ix+mx] = mag_Xpcbxmx2[ix];
						}
						break;
				}				
				for (ix = 0; ix < mx; ix++){
				#ifdef USE_SRAM_PRT_RENERING_BUF
                    pSram2 = pSram;
                #else
                    prt_memory2 = prt_memory;
                #endif
					if (imx16 < dmw16 - 1) {
						cbt = pattern16[ix];
						if (opaque) {
							for (iy = 0; iy < my; iy++) {
							#ifdef USE_SRAM_PRT_RENERING_BUF
                                SramReadData( pSram2, pTempSramBuff, 2 );
                                prt_memory2 = (HUGEDATA INT16U *)pTempSramBuff;
                            #endif
								*prt_memory2 = cbt;
                            #ifdef USE_SRAM_PRT_RENERING_BUF
                                SramWriteData( pSram2, pTempSramBuff, 2 );
                                pSram2 = pSram2 + (lwidthbyte16*2);
                            #else
                                prt_memory2 = prt_memory2 + lwidthbyte16;
                            #endif
							}
						} else {		  
							for (iy = 0; iy < my; iy++) {
							#ifdef USE_SRAM_PRT_RENERING_BUF
                                SramReadData( pSram2, pTempSramBuff, 2 );
                                prt_memory2 = (HUGEDATA INT16U *)pTempSramBuff;
                            #endif
								*prt_memory2 |= cbt;
                            #ifdef USE_SRAM_PRT_RENERING_BUF
                                SramWriteData( pSram2, pTempSramBuff, 2 );
                                pSram2 = pSram2 + (lwidthbyte16*2);
                            #else
                                prt_memory2 = prt_memory2 + lwidthbyte16;
                            #endif
							}
						}	  
					} else if (imx16 == dmw16 - 1) {
						cbt = pattern16[ix];
						if (opaque) {
							if(rmw16 != 0) {
								cbm = nlmask16[rmw16] | cbt;
								for (iy = 0; iy < my; iy++) {
								#ifdef USE_SRAM_PRT_RENERING_BUF
                                    SramReadData( pSram2, pTempSramBuff, 2 );
                                    prt_memory2 = (HUGEDATA INT16U *)pTempSramBuff;
                                #endif
									*prt_memory2 |= cbt;
									*prt_memory2 &= cbm;
                                #ifdef USE_SRAM_PRT_RENERING_BUF
                                    SramWriteData( pSram2, pTempSramBuff, 2 );
                                    pSram2 = pSram2 + (lwidthbyte16*2);
                                #else
                                    prt_memory2 = prt_memory2 + lwidthbyte16;
                                #endif
								}
							} else {
								for (iy = 0; iy < my; iy++) {
								#ifdef USE_SRAM_PRT_RENERING_BUF
                                    SramReadData( pSram2, pTempSramBuff, 2 );
                                    prt_memory2 = (HUGEDATA INT16U *)pTempSramBuff;
                                #endif
									*prt_memory2 = cbt;
                                #ifdef USE_SRAM_PRT_RENERING_BUF
                                    SramWriteData( pSram2, pTempSramBuff, 2 );
                                    pSram2 = pSram2 + (lwidthbyte16*2);
                                #else
                                    prt_memory2 = prt_memory2 + lwidthbyte16;
                                #endif
								}
							}	 
						} else { 
							for (iy = 0; iy < my; iy++) {
							#ifdef USE_SRAM_PRT_RENERING_BUF
                                SramReadData( pSram2, pTempSramBuff, 2 );
                                prt_memory2 = (HUGEDATA INT16U *)pTempSramBuff;
                            #endif
								*prt_memory2 |= cbt;
                            #ifdef USE_SRAM_PRT_RENERING_BUF
                                SramWriteData( pSram2, pTempSramBuff, 2 );
                                pSram2 = pSram2 + (lwidthbyte16*2);
                            #else
                                prt_memory2 = prt_memory2 + lwidthbyte16;
                            #endif
							}
						}
						break;	 
					} 
					imx16++;
				#ifdef USE_SRAM_PRT_RENERING_BUF
                    pSram = pSram + 2;
                #else
					prt_memory++;
                #endif
				}
	   		}
            ipos16 = ipos16 + mlwb16;
            rspos = rspos - dwr8;
		}
	}
}

//pattern must be even addr.
void PrtPattern_Rot180_Mag1(HUGEDATA INT8U *pattern,short dwr8,short w,short h)		  //24 x 5 = 120 (signed char -128 ~ 127)
{
	short    dx16,dw16;
	long     pos16,ipos16;
	INT8U	rx16, rw16,nrx16;// cb,cbr, cbl, cbn, cbm;
	INT16U 	 cb,cbr, cbl, cbn, cbm;
	short i,j,k;//short
	short rspos,mlwb16;   //
	char opaque;//
	char odd;
	INT8U	tmppattern[LINE_BUF_SIZE];	  //maximun 60mm pad
	HUGEDATA INT16U *prt_memory,*pattern16;
	HUGEDATA INT8U *pattern8;
	INT16U	pattern168[LINE_BUF_SIZE/2];
	short px, py;
#ifdef USE_SRAM_PRT_RENERING_BUF
    INT32U pSram = 0;
    INT8U *pTempSramBuff;
    INT32U tempSize = 0;
#endif
#ifdef USE_TEST_TMP_PRTBUF_MAX_SIZE
        static INT32U maxSizeTmpPrtBuf = 0;
#endif


	px = PrtStruct.x + PrtStruct.LMargin;
	py = PrtStruct.y + PrtStruct.TMargin;

	//HYP 20040723
	if(PrtDrvStruct.PrtDirection == NON_INVERT) {
		px = px + PrtStruct.NonInvModeTphAlign;
	}
	// Check Boundary 
	if (CheckBoundary(px,py,px+w,py+h) == FALSE) {
		//return;
		//HYP 20040226
		w = (w - CheckBoundaryErrX(px,px+w));
		h = (h - CheckBoundaryErrY(py,py+h));
		if(w <= 0) return;
		if(h <= 0) return;
	}
	if(w == 0) return; 
	
	opaque = PrtStruct.Opaque;

 	dx16 = px >> 4;
	rx16 = px & 0x0f;

  	dw16 = w >>4;
	rw16 = w & 0x0f;
	if (rw16 != 0) dw16++;

	odd = dwr8&0x01;
	pattern8 = (HUGEDATA INT8U *)pattern168;

	pos16=(long)py*(PrtStruct.PrintableWidthBytes>>1);
	ipos16 = pos16 + dx16;
	mlwb16 = PrtStruct.PrintableWidthBytes>>1;
	rspos = (h-1)*dwr8;

#ifdef USE_TEST_PRT_RENDERING_TIME
    tempCount_rot180m++;
#endif

	if (rx16 != 0) {
		nrx16 = 16 - rx16;
		for (j = 0; j < h; j++) {
        #ifdef USE_SRAM_PRT_RENERING_BUF
            pSram = SramPrtMemory + (ipos16*2);
            tempSize = 2 * (dw16 + 1);
            pTempSramBuff = (char *)malloc(tempSize);
            SramReadData( pSram, pTempSramBuff, tempSize );
            prt_memory = (HUGEDATA INT16U *) pTempSramBuff;
            #ifdef USE_TEST_TMP_PRTBUF_MAX_SIZE
                if( tempSize > maxSizeTmpPrtBuf )
                {
                    maxSizeTmpPrtBuf = tempSize;
                }
            #endif
        #else //  USE_SRAM_PRT_RENERING_BUF
			prt_memory = (HUGEDATA INT16U *)PrtStruct.PrtMemory + ipos16;
        #endif //  USE_SRAM_PRT_RENERING_BUF
			PrtPattern_Swap_Cpy(tmppattern, &pattern[rspos], w);	 //
			cbl = 0x0000;
			if(odd){
				switch(dwr8) {
					case 1:
						pattern8[0]= tmppattern[0];
						pattern8[1]= 0x00;	//	caution!! clear garbage data 
				   		break;
					case 3:
						pattern8[0]= tmppattern[0];
						pattern8[1]= tmppattern[1];
						pattern8[2]= tmppattern[2];
						pattern8[3]= 0x00;	//
						break;
					default:
						for(k=0;k<dwr8;k++) {
							pattern8[k]= tmppattern[k];
						}
						pattern8[k]= 0x00;	//
						break;
				}
				pattern16 = pattern168;
			} else {
				pattern16 = (HUGEDATA INT16U *)&tmppattern[0];
			}
			
		   for (i = 0; i < (dw16 + 1); i++) {
				cb = pattern16[i];	
				if(i < dw16 - 1) {		
					cb = Inline_Ror16(cb, rx16);
					cbr = nlmask16[rx16] & cb;
					cbn = cbl | cbr;
					cbl = lmask16[rx16] & cb;
					if (opaque) {
						if(i == 0) {
							cbm = lmask16[rx16] | cbn;
							*prt_memory |= cbn;
							*prt_memory &= cbm;
						} else {
							*prt_memory = cbn;
						}
					} else {  
						*prt_memory |= cbn;	  
					}		
				} else if(i == dw16 - 1) {
					if((nrx16>=rw16) && (rw16 != 0)) {
						cb = Inline_Ror16(cb, rx16);
						cbr = nlmask16[rx16] & cb;
						cbn = cbl | cbr;
						if (opaque) {
							if(i == 0) {
								cbm = lmask16[rx16] | cbn;
								cbm = nlmask16[rx16+rw16] | cbm;
							} else {
								cbm = nlmask16[rx16+rw16] | cbn;
							}
							*prt_memory |= cbn;
							*prt_memory &= cbm;
						} else {	 
							*prt_memory |= cbn;	  
						}	 
					} else {
						cb = Inline_Ror16(cb, rx16);
						cbr = nlmask16[rx16] & cb;
						cbn = cbl | cbr;
				  		cbl = lmask16[rx16] & cb;
 						if (opaque) {	
							if(i == 0) {
								cbm = lmask16[rx16] | cbn;
								*prt_memory |= cbn;
								*prt_memory &= cbm;
							} else {
								*prt_memory = cbn;	   
							}
						} else {	
							*prt_memory |= cbn;
						}	   
					}
				} else if (i == dw16) {
					if ((nrx16<rw16) || (rw16 == 0)) {
						cbn = cbl ;
						if (opaque) {	
							if(rw16 == 0) {
								cbm = nlmask16[rx16] | cbn;
							} else {
								cbm = nlmask16[rw16-nrx16] | cbn;
							}
							*prt_memory |= cbn; 
							*prt_memory &= cbm;
						} else {	
							*prt_memory |= cbn;	  
						}		  
					}
					break;
				}
				prt_memory++;
			}
			rspos = rspos - dwr8;
			ipos16 = ipos16 + mlwb16;
        #ifdef USE_SRAM_PRT_RENERING_BUF
            SramWriteData( pSram, pTempSramBuff, tempSize );
            free( pTempSramBuff );
        #endif
		}
	} else {
		for (j = 0; j < h; j++) {
        #ifdef USE_SRAM_PRT_RENERING_BUF
            pSram = SramPrtMemory + (ipos16*2);
            tempSize = 2 * (dw16+1);
            pTempSramBuff = (char *)malloc(tempSize);
            SramReadData( pSram, pTempSramBuff, tempSize );
            prt_memory = (HUGEDATA INT16U *) pTempSramBuff;
            #ifdef USE_TEST_TMP_PRTBUF_MAX_SIZE
                if( tempSize > maxSizeTmpPrtBuf )
                {
                    maxSizeTmpPrtBuf = tempSize;
                }
            #endif
        #else //  USE_SRAM_PRT_RENERING_BUF
 			prt_memory = (HUGEDATA INT16U *)PrtStruct.PrtMemory + ipos16;
        #endif //  USE_SRAM_PRT_RENERING_BUF
			PrtPattern_Swap_Cpy(tmppattern, &pattern[rspos], w);	 //
			if(odd){
				switch(dwr8) {
					case 1:
						pattern8[0]= tmppattern[0];
						pattern8[1]= 0x00;	//	caution!! clear garbage data 
				   		break;
					case 3:
						pattern8[0]= tmppattern[0];
						pattern8[1]= tmppattern[1];
						pattern8[2]= tmppattern[2];
						pattern8[3]= 0x00;	//
						break;
					default:
						for(k=0;k<dwr8;k++) {
							pattern8[k]= tmppattern[k];
						}
						pattern8[k]= 0x00;	//
						break;
				}
				pattern16 = pattern168;
			} else {
				pattern16 = (HUGEDATA INT16U *)&tmppattern[0];
			}

			for (i= 0; i < dw16; i++) {
				cb = pattern16[i];	
				if (i < dw16 - 1) {
					if (opaque) {
						*prt_memory = cb;	 
					} else {		  
						*prt_memory |= cb;	
					}	  
				} else if (i == dw16 - 1) {
					if (opaque) {
						if(rw16 != 0) {
							cbm = nlmask16[rw16] | cb;
							*prt_memory |= cb;
							*prt_memory &= cbm;
						} else {
							*prt_memory = cb;	
						}
					} else { 
						*prt_memory |= cb;		 
					}
					break;	 
				}
				prt_memory++;
	   		}
			rspos = rspos - dwr8;
			ipos16 = ipos16 + mlwb16;
        #ifdef USE_SRAM_PRT_RENERING_BUF
            SramWriteData( pSram, pTempSramBuff, tempSize );
            free( pTempSramBuff );
        #endif //  USE_SRAM_PRT_RENERING_BUF
		}
	}
#ifdef USE_TEST_TMP_PRTBUF_MAX_SIZE
        Flash_lwrite( DFLASH_BASE + FLASH_TMP_TEST_RESULT_AREA + 4, maxSizeTmpPrtBuf );
#endif
}

#ifdef USE_PrtPatternHVflip_Funtion
//pattern must be even addr. 
void PrtPatternVflip_Rot0(HUGEDATA INT8U *pattern,short dwr8,short w,short h)
{
	short    ix,iy,dmw16,rmw16,imx16,dw16,rw16;
	long     pos16,ipos16;
	INT8U	dx16,rx16,nrx16;
	INT16U  cbt, cbr, cbl, cbn, cbm;
	ROMDATA INT8U *mag_X, *mag_Xpcbxmx, *mag_Xpcbxmx2;
	char mx,my;
	short i,j,k;//short or char??????
	short rspos,mlwb16;   //
	short lwidthbyte16;
	char opaque;//
	HUGEDATA INT16U *prt_memory, *p_pattern16,*prt_memory2;
	HUGEDATA INT8U *pattern8,*pattern82;
	CWD	cb;
	char odd;
	INT16U	pattern16[LINE_BUF_SIZE],pattern1682[LINE_BUF_SIZE];
	short px, py;

	px = PrtStruct.x + PrtStruct.LMargin;
	py = PrtStruct.y + PrtStruct.TMargin;	
	if(w == 0) return; 

	mx = StrStruct.Magx;
	my = StrStruct.Magy;

	//HYP 20040723
	if(PrtDrvStruct.PrtDirection == NON_INVERT) {
		px = px + PrtStruct.NonInvModeTphAlign;
	}

	// Check Boundary 
	if (CheckBoundary(px,py,px+w*mx,py+h*my) == FALSE) {
		//return;
		//HYP 20040226
		w = (w*mx - CheckBoundaryErrX(px,px+w*mx))/mx;
		h = (h*my - CheckBoundaryErrY(py,py+h*my))/my;
		if(w <= 0) return;
		if(h <= 0) return;
	}
	lwidthbyte16 = PrtStruct.PrintableWidthBytes>>1;
	opaque = PrtStruct.Opaque;

	dx16 = px >> 4;
	rx16 = px & 0x0f;

	dmw16 = (w * mx) >>4;
	rmw16 = (w * mx) & 0x0f;
	if (rmw16 != 0) dmw16++;

	dw16 = w  >>4;
	rw16 = w  & 0x0f;
	if (rw16 != 0) dw16++;

	odd = dwr8 & 0x01;
	pattern82 = (HUGEDATA INT8U *)pattern1682;
	pattern8 = (HUGEDATA INT8U *)pattern16;

	pos16=(long)py*lwidthbyte16;

	mag_X = Inline_magX(mx);

	ipos16= pos16 + dx16;
	mlwb16 = my * lwidthbyte16;
	rspos = (h-1)*dwr8;

	if (rx16 != 0) {
		nrx16 = 16 - rx16;
		for (j = 0; j < h; j++) {
			imx16=0;  	  
			prt_memory = (HUGEDATA INT16U *)PrtStruct.PrtMemory + ipos16;
			cbl = 0x0000;
			if(odd){
				switch(dwr8) {
					case 1:
						pattern82[0]= pattern[rspos];  //odd
						pattern82[1]= 0x00;	//	caution!! clear garbage data 
				   		break;
					case 3:
						pattern82[0]= pattern[rspos];  //odd
						pattern82[1]= pattern[rspos+1];	//even
						pattern82[2]= pattern[rspos+2];	//
						pattern82[3]= 0x00;	//
						break;
					default:
						for(k=0;k<dwr8;k++) {
							pattern82[k]= pattern[rspos+k];	//even
						}
						pattern82[k]= 0x00;	//
						break;
				}
				p_pattern16 = pattern1682;
			} else {
				p_pattern16 = (HUGEDATA INT16U *)&pattern[rspos];
			}
		
		   for (i = 0; i < (dw16 + 1); i++) {
				cb.word = p_pattern16[i];
				switch (mx) {
					case 2:
						pattern16[0] = *((ROMDATA INT16U *)*CnvTableMagX2 + (INT16U)cb.byte.right);
						pattern16[1] = *((ROMDATA INT16U *)*CnvTableMagX2 + (INT16U)cb.byte.left);
						break;
					default:
						mag_Xpcbxmx = mag_X + ((INT16U)cb.byte.right * mx);
						mag_Xpcbxmx2 = mag_X + ((INT16U)cb.byte.left * mx);
						for (ix = 0; ix < mx; ix++){
							pattern8[ix] = mag_Xpcbxmx[ix];
							pattern8[ix+mx] = mag_Xpcbxmx2[ix];
						}
						break;
				}
				for (ix = 0; ix < mx; ix++){
					prt_memory2 = prt_memory;
					if(imx16 < dmw16 - 1) {
						cbt = pattern16[ix];							
						cbt = Inline_Ror16(cbt, rx16);
						cbr = nlmask16[rx16] & cbt;
						cbn = cbl | cbr;
				  		cbl = lmask16[rx16] & cbt;
						if (opaque) {
							if(imx16 == 0) {
								for (iy = 0; iy < my; iy++) {
									cbm = lmask16[rx16] | cbn;
									*prt_memory2 |= cbn;
									*prt_memory2 &= cbm;
									prt_memory2 = prt_memory2 +lwidthbyte16;
								}
							} else {
								for (iy = 0; iy < my; iy++) {
										*prt_memory2 = cbn;
										prt_memory2 = prt_memory2 +lwidthbyte16;
								}
							}
						} else {  
							for (iy = 0; iy < my; iy++) {
								*prt_memory2 |= cbn;	  
								prt_memory2 = prt_memory2 +lwidthbyte16;
							}
						}		
					} else if(imx16 == dmw16 - 1) {
						if((nrx16>=rmw16) && (rmw16 != 0)) {
							cbt = pattern16[ix];
							cbt = Inline_Ror16(cbt, rx16);
							cbr = nlmask16[rx16] & cbt;
							cbn = cbl | cbr;
							if (opaque) {
								if(imx16 == 0) {
									for (iy = 0; iy < my; iy++) {
										cbm = lmask16[rx16] | cbn;
										cbm = nlmask16[rx16+rmw16] | cbm;
										*prt_memory2 |= cbn;
										*prt_memory2 &= cbm;
										prt_memory2 = prt_memory2 +lwidthbyte16;
									}
								} else {
									for (iy = 0; iy < my; iy++) {
										cbm = nlmask16[rx16+rmw16] | cbn;
										*prt_memory2 |= cbn;
										*prt_memory2 &= cbm;
										prt_memory2 = prt_memory2 +lwidthbyte16;
									}
								}
							} else {	 
								for (iy = 0; iy < my; iy++) {
									*prt_memory2 |= cbn;	  
									prt_memory2 = prt_memory2 +lwidthbyte16;
								}
							}	 
						} else {
							cbt = pattern16[ix];							
							cbt = Inline_Ror16(cbt, rx16);
							cbr = nlmask16[rx16] & cbt;
							cbn = cbl | cbr;
					  		cbl = lmask16[rx16] & cbt;
 							if (opaque) {	
								if(imx16 == 0) {
									for (iy = 0; iy < my; iy++) {
										cbm = lmask16[rx16] | cbn;
										*prt_memory2 |= cbn;
										*prt_memory2 &= cbm;
										prt_memory2 = prt_memory2 +lwidthbyte16;
									}
								} else {
									for (iy = 0; iy < my; iy++) {
										*prt_memory2 = cbn;
										prt_memory2 = prt_memory2 +lwidthbyte16;	   
									}
								}
							} else {	
								for (iy = 0; iy < my; iy++) {
									*prt_memory2 |= cbn;
									prt_memory2 = prt_memory2 +lwidthbyte16;
								}
							}	   
						}
					} else if (imx16 == dmw16) {
						if ((nrx16<rmw16) || (rmw16 == 0)) {
							cbn = cbl ;
							if (opaque) {	
								if(rmw16 == 0) {
									for (iy = 0; iy < my; iy++) {
										cbm = nlmask16[rx16] | cbn;
										*prt_memory2 |= cbn; 
										*prt_memory2 &= cbm;
										prt_memory2 = prt_memory2 +lwidthbyte16;
									}
								} else {
									for (iy = 0; iy < my; iy++) {								
										cbm = nlmask16[rmw16-nrx16] | cbn;
										*prt_memory2 |= cbn; 
										*prt_memory2 &= cbm;
										prt_memory2 = prt_memory2 +lwidthbyte16;
									}
								}
							} else {	
								for (iy = 0; iy < my; iy++) {
									*prt_memory2 |= cbn;	  
									prt_memory2 = prt_memory2 +lwidthbyte16;
								}
							}		  
						}
						break;
					}						 
					imx16++;
					prt_memory++;
				}
			}
			rspos = rspos - dwr8;
			ipos16 = ipos16 + mlwb16;
		}
	} else {
		for (j = 0; j < h; j++) {
			imx16=0;	  
			prt_memory = (HUGEDATA INT16U *)PrtStruct.PrtMemory + ipos16;
			if(odd){
				switch(dwr8) {
					case 1:
						pattern82[0]= pattern[rspos];  //odd
						pattern82[1]= 0x00;	//	caution!! clear garbage data 
				   		break;
					case 3:
						pattern82[0]= pattern[rspos];  //odd
						pattern82[1]= pattern[rspos+1];	//even
						pattern82[2]= pattern[rspos+2];	//
						pattern82[3]= 0x00;	//
						break;
					default:
						for(k=0;k<dwr8;k++) {
							pattern82[k]= pattern[rspos+k];	//even
						}
						pattern82[k]= 0x00;	//
						break;
				}
				p_pattern16 = pattern1682;
			} else {
				p_pattern16 = (HUGEDATA INT16U *)&pattern[rspos];
			}

			for (i= 0; i < dw16; i++) {
				cb.word = p_pattern16[i];
				switch (mx) {
					case 2:
						pattern16[0] = *((ROMDATA INT16U *)*CnvTableMagX2 + (INT16U)cb.byte.right);
						pattern16[1] = *((ROMDATA INT16U *)*CnvTableMagX2 + (INT16U)cb.byte.left);
						break;
					default:
						mag_Xpcbxmx = mag_X + ((INT16U)cb.byte.right * mx);
						mag_Xpcbxmx2 = mag_X + ((INT16U)cb.byte.left * mx);
						for (ix = 0; ix < mx; ix++){
							pattern8[ix] = mag_Xpcbxmx[ix];
							pattern8[ix+mx] = mag_Xpcbxmx2[ix];
						}
						break;
				}				
				for (ix = 0; ix < mx; ix++){
					prt_memory2 = prt_memory;
					if (imx16 < dmw16 - 1) {
						cbt = pattern16[ix];
						if (opaque) {
							for (iy = 0; iy < my; iy++) {
								*prt_memory2 = cbt;	 
								prt_memory2 = prt_memory2 +lwidthbyte16;
							}
						} else {		  
							for (iy = 0; iy < my; iy++) {
								*prt_memory2 |= cbt;	
								prt_memory2 = prt_memory2 +lwidthbyte16;
							}
						}	  
					} else if (imx16 == dmw16 - 1) {
						cbt = pattern16[ix];
						if (opaque) {
							if(rmw16 != 0) {
								for (iy = 0; iy < my; iy++) {
									cbm = nlmask16[rmw16] | cbt;
									*prt_memory2 |= cbt;
									*prt_memory2 &= cbm;
									prt_memory2 = prt_memory2 +lwidthbyte16;
								}
							} else {
								for (iy = 0; iy < my; iy++) {
									*prt_memory2 = cbt;
									prt_memory2 = prt_memory2 +lwidthbyte16;
								}
							}	 
						} else { 
							for (iy = 0; iy < my; iy++) {
								*prt_memory2 |= cbt;		 
								prt_memory2 = prt_memory2 +lwidthbyte16;
							}
						}
						break;	 
					} 
					imx16++;
			   		prt_memory++; 
				}
	   		}
			rspos = rspos - dwr8;
			ipos16 = ipos16 + mlwb16;
		}
	}
}
#endif//#ifdef USE_PrtPatternHVflip_Funtion

//20040209 HYP
void PrtPattern_Str_Area_Typeface(void)
{
	short    dx16,dw16;
	long     pos16,ipos16;
	INT8U	rx16, rw16,rxw16;
	INT16U 	 cb;
	short i,j;//short
	short mlwb16;   //
	HUGEDATA INT16U *prt_memory;
	short px, py;

	short dwr8, w, h;
	INT16U mask16;
	INT8U typeface;
#ifdef USE_SRAM_PRT_RENERING_BUF
    INT32U pSram = 0;
    INT8U *pTempSramBuff;
    INT32U tempSize = 0;
    INT8U tempCount = 0;
#endif
#ifdef USE_TEST_TMP_PRTBUF_MAX_SIZE
    static INT32U maxSizeTmpPrtBuf = 0;
#endif

	typeface = StrStruct.Typeface;
	if(!(typeface&REVERSE) && !(typeface&SHADOW)) return;

	px = PrtStruct.x + PrtStruct.LMargin;
	py = PrtStruct.y + PrtStruct.TMargin;

	if(PrtStruct.Rot & MASK_DEG_90_270) {
		h = StrStruct.StrWidth;
		w = StrStruct.StrHeight;
	} else {
		w = StrStruct.StrWidth;
		h = StrStruct.StrHeight;
	}

	if((w == 0) || (h == 0)) return; // if opaque is on, blank is printed.
					   // because, if rx != 0, first byte is printed.

	//HYP 20040723
	if(PrtDrvStruct.PrtDirection == NON_INVERT) {
		px = px + PrtStruct.NonInvModeTphAlign;
	}

	// Check Boundary 
	if (CheckBoundary(px,py,px+w-1,py+h-1) == FALSE) {
		return;
	}

	if(PrtStruct.Mode == LABEL) {
		if(PrtStruct.Rot & MASK_DEG_90_270) {

				if(StrStruct.Typeface & BOLD) {	
					h = h + StrStruct.Magx;
				}
				if(StrStruct.Typeface & OUTLINE) {	
					h = h + StrStruct.Magx;
				}
		} else {
				if(StrStruct.Typeface & BOLD) {	
					w = w + StrStruct.Magx;
				}
				if(StrStruct.Typeface & OUTLINE) {	
					w = w + StrStruct.Magx;
				}
		}
	}

	dwr8 = w/8;
	if (w%8) dwr8++;

 	dx16 = px >> 4;
	rx16 = px & 0x0f;

  	dw16 = w >>4;
	rw16 = w & 0x0f;

	if (rw16 != 0) {
		dw16++;
	} else {
		if(rx16 !=0) dw16++;
	}

	if (rw16 > (16-rx16)) {
		dw16++;
		rxw16 = rw16 - (16-rx16);
	} else {
		rxw16 = rx16+rw16;
		if(rxw16 == 0) dw16++;
		if(rxw16 == 16) {
			dw16++;
			rxw16=0;
		}
	}
	pos16=(long)py*(PrtStruct.PrintableWidthBytes>>1);
	ipos16 = pos16 + dx16;
	mlwb16 = PrtStruct.PrintableWidthBytes>>1;

	for (j = 0; j < h; j++) {
    #ifdef USE_SRAM_PRT_RENERING_BUF
        pSram = SramPrtMemory + (ipos16*2);
        tempSize = dw16*2;
        pTempSramBuff = (char *)malloc(tempSize);
        SramReadData( pSram, pTempSramBuff, tempSize );
        prt_memory = (INT16U *) pTempSramBuff;
    #ifdef USE_TEST_TMP_PRTBUF_MAX_SIZE
            if( tempSize > maxSizeTmpPrtBuf )
            {
                maxSizeTmpPrtBuf = tempSize;
            }
    #endif
    #else //  USE_SRAM_PRT_RENERING_BUF
 		prt_memory = (HUGEDATA INT16U *)PrtStruct.PrtMemory + ipos16;
    #endif //  USE_SRAM_PRT_RENERING_BUF
	
		if(j%2) mask16 = 0xaaaa;
		else mask16 = 0x5555;
		for (i= 0; i < dw16; i++) {
			cb = *prt_memory;
			if(dw16 == 1) {//if(i == 0 && i == dw16 - 1) {	// Added by CJK20070625
				if(typeface & SHADOW) cb = (cb & (lmask16[rx16] | nlmask16[rxw16])) | ((cb | mask16) & (nlmask16[rx16] & lmask16[rxw16]));
				if(typeface & REVERSE) cb = (cb & (lmask16[rx16] | nlmask16[rxw16])) | ((~cb) & (nlmask16[rx16] & lmask16[rxw16]));
			} else if(i == 0) {
				if(typeface & SHADOW) cb = (cb & lmask16[rx16]) | ((cb | mask16) & nlmask16[rx16]);
				if(typeface & REVERSE) cb = (cb & lmask16[rx16]) | ((~cb) & nlmask16[rx16]);
			} else if(i == dw16 - 1) {
				if(typeface & SHADOW) cb = (cb & nlmask16[rxw16]) | ((cb | mask16) & lmask16[rxw16]);
				if(typeface & REVERSE) cb = (cb & nlmask16[rxw16]) | ((~cb) & lmask16[rxw16]);
			} else {
				if(typeface & SHADOW) cb = cb | mask16;
				if(typeface & REVERSE) cb = ~cb;
			}
			*prt_memory = cb;	 
			prt_memory++;
	   	}
    #ifdef USE_SRAM_PRT_RENERING_BUF
        SramWriteData( pSram, pTempSramBuff, tempSize );
        free( pTempSramBuff );
    #endif
		ipos16 = ipos16 + mlwb16;
	}
#ifdef USE_TEST_TMP_PRTBUF_MAX_SIZE
        Flash_lwrite( DFLASH_BASE + FLASH_TMP_TEST_RESULT_AREA + 8, maxSizeTmpPrtBuf );
#endif
}

void PrtBarPattern(HUGEDATA INT8U *pattern,short pw)
{
	if(pw == -1) return;

//	if((PrtDrvStruct.PrtLoadFormFlag == OFF) || (PrtDrvStruct.PrePrintMode == ON)) {
	if(PrtDrvStruct.PrtLoadFormFlag == OFF) {
		if(PrtStruct.Rot&MASK_DEG_90_270) {
			PrtRectFill(PrtStruct.x,PrtStruct.y,PrtStruct.x+BarStruct.FullHeight,PrtStruct.y+pw+2*BarStruct.QuietZone,WHITE);
		} else { //DEG_O, DEG_180
			PrtRectFill(PrtStruct.x,PrtStruct.y,PrtStruct.x+pw+2*BarStruct.QuietZone,PrtStruct.y+BarStruct.FullHeight,WHITE);
		}
	}

	switch (PrtStruct.Rot) {
	    case DEG_0:
	   	case DEG_180: 
			PrtBarPattern_Hori(pattern,pw);
			break;
		case DEG_90:
		case DEG_270: 
			PrtBarPattern_Verti(pattern,pw);
		    break;
	}
}

void PrtPattern(HUGEDATA INT8U *pattern,short dwr,short w,short h,char rot)
{
	switch (rot) {
		case DEG_0: 
		case DEG_360:
			if(StrStruct.Magx!=1 || StrStruct.Magy!=1)	{
				PrtPattern_Rot0(pattern,dwr,w,h);
			} else {
				PrtPattern_Rot0_Mag1(pattern,dwr,w,h);
			}
			break;
		case DEG_90:
		case DEG_450:
			PrtPattern_Rot90(pattern,dwr,w,h); 
			break;
		case DEG_180:				 
		case DEG_540:
			if(StrStruct.Magx!=1 || StrStruct.Magy!=1)	{
				PrtPattern_Rot180(pattern,dwr,w,h);
			} else {
				PrtPattern_Rot180_Mag1(pattern,dwr,w,h); 
			}
			break;
		case DEG_270:
		case DEG_630:
			PrtPattern_Rot270(pattern,dwr,w,h); 
			break;
	}
}


#define	VFLIP_ROT_0	10
#ifdef USE_PrtPatternHVflip_Funtion
void PrtPatternHVflip(HUGEDATA INT8U *pattern,short dwr,short w,short h,char rot,char hv) //Max size is PrtScratchPad size
{
	HUGEDATA INT8U	*p;

	if(hv == 0) {   // vflip
		if(rot != DEG_0 && rot != DEG_360) {
			PrtPattern_VFlip_Cpy(PrtStruct.PrtScratchPad, pattern,dwr, h);						
			p = PrtStruct.PrtScratchPad;
		} else {
			p = pattern;
			rot = VFLIP_ROT_0;
		}
	} else {   // hflip
			PrtPattern_HFlip_Cpy(PrtStruct.PrtScratchPad, pattern, dwr, w, h);						
			p = PrtStruct.PrtScratchPad;
	}

	switch (rot) {
		case DEG_0: 
		case DEG_360:
			if(StrStruct.Magx!=1 || StrStruct.Magy!=1)	{
				PrtPattern_Rot0(p,dwr,w,h);
			} else {
				PrtPattern_Rot0_Mag1(p,dwr,w,h);
			}
			break;
		case DEG_90:
		case DEG_450:
			PrtPattern_Rot90(p,dwr,w,h); 
			break;
		case DEG_180:				 
		case DEG_540:
			if(StrStruct.Magx!=1 || StrStruct.Magy!=1)	{
				PrtPattern_Rot180(p,dwr,w,h);
			} else {
				PrtPattern_Rot180_Mag1(p,dwr,w,h); 
			}
			break;
		case DEG_270:
		case DEG_630:
			PrtPattern_Rot270(p,dwr,w,h); 
			break;
		case VFLIP_ROT_0: 
			PrtPatternVflip_Rot0(p,dwr,w,h);
			break;
	}
}
#endif//#ifdef USE_PrtPatternHVflip_Funtion
//caution! this func not tested
#ifdef USE_POSTNET
void PrtPostbarPattern(HUGEDATA INT8U *pattern,short pw)
{
	short x,p,c,y,shift;
	short x_max;
	long  dpos,start_pos,spos,yhalf;
	INT8U smask,dmask;
	short px, py;

	px = PrtStruct.x + PrtStruct.LMargin;
	py = PrtStruct.y + PrtStruct.TMargin;

	//HYP 20040723
	if(PrtDrvStruct.PrtDirection == NON_INVERT) {
		px = px + PrtStruct.NonInvModeTphAlign;
	}

	x_max=px+pw;
	start_pos=(long)py*PrtStruct.PrintableWidthBytes;
	shift=px%8;
	yhalf=py+(BarStruct.BarHeight-BarStruct.nb);
	for (y=py; y<py+BarStruct.BarHeight; y++) {
		dmask=0x80;
		dmask>>=shift;
		if (dmask==0) dmask=0x80;
		smask=0x80;
		p=px;
		for (x=px; x<x_max; x++) {
			spos=(x-px)/8;
			if (x>=PrtStruct.PrintableWidth) break;
			if (pattern[spos]&smask) {
			     for (c=0; c<BarStruct.wb; c++) {
				dpos=start_pos+p/8;
			PrtStruct.PrtMemory[dpos] |= dmask;
				dmask>>=1; if (dmask==0) dmask=0x80;
				p++;
			     }
			} else {
				if (y>=yhalf) {
				     for (c=0; c<BarStruct.wb; c++) {
					dpos=start_pos+p/8;
					PrtStruct.PrtMemory[dpos] |= dmask;
					dmask>>=1; if (dmask==0) dmask=0x80;
					p++;
				     }
				} else {
				     for (c=0; c<BarStruct.wb; c++) {
					dmask>>=1; if (dmask==0) dmask=0x80;
					p++;
				     }
				}
			}
			smask>>=1; if (smask==0) smask=0x80;
			for (c=0; c<BarStruct.ns; c++) {
				dmask>>=1; if (dmask==0) dmask=0x80;
				p++;
			}
		}
		start_pos+=PrtStruct.PrintableWidthBytes;
	}
}
#endif//#ifdef USE_POSTNET
/*
void RenderTestBarcode(void){
	char string_buf[30];

/// render time check on	///////////
//	PrtDrvStruct.SysTimer = 0;
//	PrtDrvStart(PRT_STATE_TM);
///////////////////////////////////

	switch(BarStruct.BarType) {
		case EAN13:
			BarStruct.GuardType = EAN13P0_GUARD;
//			sprintf(string_buf, "880123456789");
			sprintf(string_buf, "1145380007801");
			BarStruct.CheckType =0;//for test
			BarcodeOut(string_buf);
			break;		
		case EAN13P2:
			BarStruct.GuardType = EAN13PX_GUARD;
			sprintf(string_buf, "541234567890 12");
			BarcodeOut(string_buf);
 			break;
		case EAN13P5:
			BarStruct.GuardType = EAN13PX_GUARD;
			sprintf(string_buf, "541234567890 12345");
			BarcodeOut(string_buf);
			break;
		case EAN8:
			sprintf(string_buf, "4067808");
			BarcodeOut(string_buf);	   		
			break;
		case EAN8P2:
			sprintf(string_buf, "5412345 34");
			BarcodeOut(string_buf);   
			break;
		case EAN8P5:
			BarStruct.GuardType = EAN13PX_GUARD;
			sprintf(string_buf, "5412345 12345");
			BarcodeOut(string_buf);   
			break;
		case UPCA:
			sprintf(string_buf, "14498374877");  
			BarcodeOut(string_buf);
			break;
	    case UPCAP2:
			sprintf(string_buf, "14498374877 12");
			BarcodeOut(string_buf);
			break;
		case UPCAP5:
			BarStruct.GuardType = EAN13PX_GUARD;
			sprintf(string_buf, "14498374877 12345");
			BarcodeOut(string_buf);
			break;
		case UPCE0:
			sprintf(string_buf, "848509");
			BarcodeOut(string_buf);	   
			break;
		case UPCE0P2:
			sprintf(string_buf, "848509 34");
			BarcodeOut(string_buf);	
			break;
		case UPCE0P5:
			sprintf(string_buf, "848509 12345");
			BarcodeOut(string_buf);   
			break;
		case UPCE1:
			sprintf(string_buf, "848509");
			BarcodeOut(string_buf);	  
			break;
		case I2OF5: 
			BarSetNSWS(2,3,5,6);
  			BarStruct.BearerBarWidth = 3*BarStruct.nb;
			BarStruct.CheckType = 2;
			BarStruct.PrtCheckHri = ON;
			sprintf(string_buf, "12345678901");
			BarcodeOut(string_buf);
			break;
		case CODE128A:  
			sprintf(string_buf, "01A[0[J[M56");
			BarcodeOut(string_buf);
			break;
		case CODE128B:  
			sprintf(string_buf, "01A[0bc56");
			BarcodeOut(string_buf);
			break;
		case CODE128C:  
			sprintf(string_buf, "113098000100200000");
			BarcodeOut(string_buf);
			break;
		case CODE128X:  
			sprintf(string_buf, "[6A[0[=a[0BC[;ab[=[Jc");
			BarcodeOut(string_buf);
			break;
		case CODE39: 
			BarSetNW(2,4);
			BarStruct.PrtCheckHri = OFF;
			BarStruct.CheckType = 1;	
			BarStruct.PrtStartStopHri = OFF;
			sprintf(string_buf, "TEST39");
			BarcodeOut(string_buf);
			break;
		case CODE39FULL: 
			BarSetNW(2,4);
			BarStruct.PrtCheckHri = OFF;
			BarStruct.CheckType = 1;	
			BarStruct.PrtStartStopHri = ON;
			sprintf(string_buf, "[0Test![J");
			BarcodeOut(string_buf);
			break;
		case CODE93:  
			BarStruct.CheckType = 2;
			BarStruct.PrtCheckHri = ON;
			sprintf(string_buf, "TEST93");
			BarcodeOut(string_buf);
			break;
		case CODE93FULL: 
			BarStruct.CheckType = 2;
			BarStruct.PrtCheckHri = ON;	
			sprintf(string_buf, "[0Test![J");
			BarcodeOut(string_buf);
			break;
		case CODABAR: 
			BarSetNW(2,4);
			BarStruct.PrtCheckHri = ON;
			BarStruct.CheckType = 1;	
			BarStruct.PrtStartStopHri = ON;
			BarStruct.PrtHri = ON;
			sprintf(string_buf, "A378590B");
			BarcodeOut(string_buf);
			break;
		case MSI: 
			BarSetNW(2,4);
//checksum
// 0 = none, 1 = mod10, 2 = mod11, 3 = mod 1010, 4 = mod1110, 5 = mod1011, 6 = mod1111
			BarStruct.CheckType = 1;
			BarStruct.PrtCheckHri = ON;
//			sprintf(string_buf, "943457842");
			sprintf(string_buf, "11476925");
			BarcodeOut(string_buf);
			break;
		case S2OF5: 
			BarSetNW(2,4);
			BarStruct.PrtCheckHri = OFF;
			sprintf(string_buf, "123456");
			BarcodeOut(string_buf);
			break;
 		case CODE11: 
			BarSetNW(2,4);
			BarStruct.CheckType = 2;
			BarStruct.PrtCheckHri = ON;
			sprintf(string_buf, "123456-789012");
			BarcodeOut(string_buf);
			break;
		case M2OF5:
			BarSetNW(2,4);
			BarStruct.PrtCheckHri = OFF;
			sprintf(string_buf, "123456");
			BarcodeOut(string_buf);
			break;
		case PLESSEY: 
			BarStruct.CheckType = 1;
			BarStruct.PrtCheckHri = OFF; 
			sprintf(string_buf, "AB41589636");
			BarcodeOut(string_buf);
			break;
		case IATA2OF5:
			BarSetNW(2,4);
			BarStruct.PrtCheckHri = OFF;
			sprintf(string_buf, "123456");
			BarcodeOut(string_buf);
			break;
		case POSTNET: 
			BarSetNSWS(10,2,3,6);
			sprintf(string_buf, "801221905");
			BarcodeOut(string_buf);
			break;
	}
// render time check off /////////
//	PrtDrvStop();
//	sprintf(string_buf, "ba=%d,",PrtDrvStruct.SysTimer); 
//	PutStrTxBuf(&CommBuf, string_buf);
//////////////////////////////////
}

void RenderTestFont(void){
	char string_buf[300];
//	short i;
//	sprintf(string_buf, "가나다라마바사아자차카타파하[[각낙난간한학? [[\n abcdefghijklmnopqrstuvwxyz[[abcdefghijklmnopqrstuvwxyz\n ABCDEFGHIJKLMNOPQRSTUVWXYZ[[ABCDEFGHIJKLMNOPQRSTUVWXYZ\n");
//	sprintf(string_buf, "가 나[B다 라[A마[[가 나[B다라[A마[[가나[B[[\n abc0[D012[C2klmabc0klmabc0[D01abc0[D012[C2klmabc0klmabc0[D2[C2klm\n");
//	sprintf(string_buf, "가\n타 파\nAB C\nDEF G\ndflasefjasle\nfelfsifjselfij\nsflefsiefj\nsdwefsefsefsefsef\nfefsefsefs\nend");
//	sprintf(string_buf, "ab cd  ef gh   ijkl    mnopq   rstu\nvwxy    zabcdefghijkl\nmnopqrstuvwxyzabcdefghi\njklmnopqrstuvwxyzabcd\nefghijklmnopqrstuvw\nxyzABCDEFGHI\nJKLMNOPQRSTUV\nWXYZ[ABCDE\nFGHIJKLMNOPQ\nRSTUVWXYZ\nabcdefghijklmnopqrstu\nvwxyzabcdefghijkl\nmnopqrstuvwxyzabcdefghi");
//	sprintf(string_buf, "??????????????[[??????????????[[\n abcdefghijklmnopqrstuvwxyz[[abcdefghijklmnopqrstuvwxyz\n ABCDEFGHIJKLMNOPQRSTUVWXYZ[[ABCDEFGHIJKLMNOPQRSTUVWXYZ\n");	 //johab
//	sprintf(string_buf, "가나다라마바사아자차카타파하[[각낙난간한학?가나다라마바사아자차카타파하[[각낙난간한학?가나다라마바사아자차카타파하[[각낙난간한학?가나다라마바사아자차카타파하[[각낙난간한학?");
//	sprintf(string_buf, "洪宜杓 [B 홍의표 [A abcABC");
//	sprintf(string_buf, "가 나[B다 라[A마[[가\n가 나[B다 라[A마\n사아자차카타\nabcdefghij\nbcdefghi\n가나다라마바\n사아자차카타\nbcdefghi\n");
//	sprintf(string_buf, "가[+나[+다[+\n라[-\n마[-바[-사");
//	sprintf(string_buf, "가[+[+[+[+[+나[-[-[-[-[-[-다\nab]+cd]-e");
//	sprintf(string_buf, "[+[+[+[+[+A[-[-[-[-[-[-B\nab[+[+[+cd[-e");
//	sprintf(string_buf, "ab cd  ef gh   ijkl    mnopq   rstu\nvwxy    zabcdefghijkl\n");
//	sprintf(string_buf, "가 나[B다 라[A마[[\x0d\x0d\x0a가 나[B다\t\t\t\t\t\t\t\t라[A마[[가나\x0d\x0d\x0a[B[[abc0[D012[C2klmab\x0d\x0d\x0a");
//	sprintf(string_buf, "가 [1나[0[B다 [2라[0[A마[[가\n가 [3나[0[B다 [0라[0[A마\n사아자차카타\nabc[1def[0ghij\nbcdefghi\n가나다");
//	sprintf(string_buf, "ABCDE\tEFGHI\tJKLM\tABCDE\taaa");
//	sprintf(string_buf, "ABCDE\t한글");
//	sprintf(string_buf, "가 나[B1다 라[B0마[[가 나[B1다라[B0마[[가나[B2[[\n abc0[A1012[A02klmabc0klmabc0[A101abc0[A012[A12k\n");
	sprintf(string_buf, "가 [I[D1나[B1[X3다[D0[X2[I라[B0[Y3마[0[Y2가 나[B1다라[B0마[0가나[B2[[\n abc0[A1012[A02klmab[D1c0k[D0lmabc0[A101abc0[A012[A12k\n");

//
//	for(i=0x80;i<=0xff;i++) {
//		string_buf[i-0x80]=i;
//	}
//	string_buf[i-0x80]=0x00;
//
	PrtStruct.x	= 10;
	PrtStruct.y = 200;
	StrStruct.StrHeight = 300;
	StrStruct.StrWidth = 400;
//	StrStruct.Typeface = BOLD;
//	StrStruct.Typeface = NO_TYPEFACE;
//	PrtStruct.Rot = DEG_90;
//	StrStruct.ChRot = DEG_0;

	switch(PrtStruct.Rot) {
		case 0:
		case 2:
	PrtRect(PrtStruct.x,PrtStruct.y,PrtStruct.x+StrStruct.StrWidth,PrtStruct.y+StrStruct.StrHeight,1,BLACK);
			break;
		case 1:
		case 3:
	PrtRect(PrtStruct.x,PrtStruct.y,PrtStruct.x+StrStruct.StrHeight,PrtStruct.y+StrStruct.StrWidth,1,BLACK);
			break;
	}
//	PrtStruct.Mode = RECEIPT;
//	StrStruct.Justification = LEFT;
// render time check on	///////////
//	PrtDrvStruct.SysTimer = 0;
//	PrtDrvStart(PRT_STATE_TM);
///////////////////////////////////

//	for (i=0; i<20; i++) {
//	PrtStruct.x	= 10;
//	PrtStruct.y = 100 + i*(PrtStruct.ReceiptLineHeight + StrStruct.LineSp); 
	FontStrOut(string_buf);
//	}
// render time check off //////////
//	PrtDrvStop();
//	sprintf(string_buf, "te=%d,he=%d,",PrtDrvStruct.SysTimer,PrtStruct.ReceiptLineHeight); 
//	PutStrTxBuf(&CommBuf, string_buf);
//////////////////////////////////
}
*/
/*
void PrtTestLabel(char *plu_name, INT32U weight, INT32U unit, INT32U price){
	char string_buf[100];
        
        StrStruct.Magx = 2;
        StrStruct.Magy = 2;
        StrStruct.Justification = 0;
        
	//plu name
	PrtStruct.x	= 10;
	PrtStruct.y = 8;
	StrStruct.StrHeight = 40;
	StrStruct.StrWidth = 8*54;
	FontStrOut(plu_name);
	//weight
	PrtStruct.x	= 10;
	PrtStruct.y = 40;
	StrStruct.StrHeight = 10*8;
	StrStruct.StrWidth = 8*54;	
	sprintf(string_buf, "WEIGHT = %06d kg", weight); 
	FontStrOut(string_buf);

	//unit
	PrtStruct.x	= 10;
	PrtStruct.y = 72;
	StrStruct.StrHeight = 10*8;
	StrStruct.StrWidth = 8*54;	
	sprintf(string_buf, "UNIT = %06d $/kg", unit); 
	FontStrOut(string_buf);	
	
	//price
	PrtStruct.x	= 10;
	PrtStruct.y = 104;
	StrStruct.StrHeight = 10*8;
	StrStruct.StrWidth = 8*54;	
	sprintf(string_buf, "PRICE = %06d $", price); 
	FontStrOut(string_buf);	
        
	PrtStruct.x	= 10;
	PrtStruct.y = 136;
        StrStruct.Magx = 1;
        StrStruct.Magy = 2;
        sprintf(string_buf, "881234567890");
        StrStruct.Justification = 2;
        BarcodeOut(string_buf);
}
*/
void	PrtTestPattern(void)
{
    INT16U count1, count2, test_buffer;
    short h;

//pattern clear
/*
		test_buffer = 0x0000;
		for (count1 = 0; count1 < PrtStruct.PrintableHeight; count1++) {
			for (count2 = 0; count2 < (TPH_ROW_BYTE/2); count2++) {
				PrtDrvStruct.TphMemory[count1 * TPH_ROW_BYTE + count2*2] = (INT8U)test_buffer;
				PrtDrvStruct.TphMemory[count1 * TPH_ROW_BYTE + count2*2+1] = (INT8U)test_buffer;
			}
		}
*/
//	Test pattern 
		test_buffer = 0xff00;
		//HYP 20040223
		if(PrtStruct.Mode == LABEL) {
			h = PrtStruct.PrintableHeight;
		} else {//PrtStruct.Mode == RECEIPT
			h = (40*8);
			PrtDrvStruct.TphMemMaxRowNum = h;
		}
		for (count1 = 0; count1 < h; count1++) {
			//HYP20040811 white space at top/bottom
			if(count1 <= 8*1) test_buffer = 0x0000;
			if(count1 > (h-8*1)) test_buffer = 0x0000;
			if(count1 == (8*1+1)) test_buffer = 0xff00;
			for (count2 = 0; count2 < (TPH_ROW_BYTE/2); count2++) {
                #ifdef USE_SRAM_PRT_RENERING_BUF
                SramWriteData_Byte( SramTphMemory + (count1*TPH_ROW_BYTE + count2*2), (INT8U)test_buffer );
                #else
				PrtDrvStruct.TphMemory[count1 * TPH_ROW_BYTE + count2*2] = (INT8U)test_buffer;
                #endif
				test_buffer = Inline_Ror16(test_buffer,8);
                #ifdef USE_SRAM_PRT_RENERING_BUF
                SramWriteData_Byte( SramTphMemory + (count1*TPH_ROW_BYTE + count2*2) + 1, (INT8U)test_buffer );
                #else
				PrtDrvStruct.TphMemory[count1 * TPH_ROW_BYTE + count2*2+1] = (INT8U)test_buffer;
                #endif
				test_buffer = Inline_Ror16(test_buffer,8);
			}
			if (count1 % 8 == 0) {
		    		if(count1 < (4*8)) {
		    			test_buffer = Inline_Ror16(test_buffer,8);
				} else if(count1 < (8*8)) {
		    			test_buffer = Inline_Ror16(test_buffer,10);
				} else if(count1 < (12*8)) {
		    			test_buffer = Inline_Ror16(test_buffer,10);
				} else if(count1 < (16*8)) {
		    			test_buffer = Inline_Ror16(test_buffer,10);
				} else if(count1 < (20*8)) {
		    			test_buffer = Inline_Ror16(test_buffer,10);
				} else if(count1 < h) {
		    			test_buffer = Inline_Ror16(test_buffer,8);
		    		}					
	    		}
		}
}
//HYP 20050216
void	PrtTestHPattern(void)
{
    	INT16U count1, count2,test_buffer;
	char string_buf[300];

//pattern clear
		test_buffer = 0x0000;
        #ifdef USE_SRAM_PRT_RENERING_BUF
        SramMemoryClear( SramTphMemory, PrtStruct.PrintableHeight*(TPH_ROW_BYTE) );
        #else
		for (count1 = 0; count1 < PrtStruct.PrintableHeight; count1++) {
			for (count2 = 0; count2 < (TPH_ROW_BYTE/2); count2++) {
				PrtDrvStruct.TphMemory[count1 * TPH_ROW_BYTE + count2*2] = (INT8U)test_buffer;
				PrtDrvStruct.TphMemory[count1 * TPH_ROW_BYTE + count2*2+1] = (INT8U)test_buffer;
			}
		}
        #endif //USE_SRAM_PRT_RENERING_BUF
        
//Test 'H' pattern
	sprintf(string_buf,\
/*
         1         2	     3	       4	 5	   6	     7	       8	 9	   0	
1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890
*/
"HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH\
HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH\
HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH"); //290 chars
	PrtStruct.x = 0;
	PrtStruct.y = 0;
	StrStruct.StrHeight = PrtStruct.PrintHeight;
	StrStruct.StrWidth = PrtStruct.PrintWidth;
	StrStruct.LineSp = 0;
	StrStruct.InterChSp = 0;
	StrStruct.Typeface = NO_TYPEFACE;
	PrtStruct.Rot = DEG_0;
	StrStruct.ChRot = DEG_0;
	StrStruct.Magx = 2;
	StrStruct.Magy = 2;
	FontLoadFont1(3);
	FontStrOut(string_buf);
}

#ifdef USE_AUSTRALIA_COOL_FUNCTION
extern INT8U reverse_percent;
extern ROMDATA INT8U Image_Form_logo[];
extern ROMDATA INT8U Image_Form_bar[];

/**
 * @brief  다음에 문자열 인쇄하기 위한 위치 조정   
 * @param  ccpp
 *          - CoOL 기능 관련 프린터 좌표를 조정하기 위한 구조체
 * @return void
 * @remark
 */
static void cool_set_str_position(const struct COOL_CONTROL_PRT_POINT *ccpp)
{
	StrStruct.StrWidth -=	ccpp->image_info.logo_width;
	
	switch(PrtStruct.Rot)
	{
		case DEG_0:
			PrtStruct.x = ccpp->origin_x + ccpp->image_info.logo_width;
			PrtStruct.y = ccpp->origin_y;
			break;
		case DEG_90:
			PrtStruct.x  = ccpp->origin_switch_x;
			PrtStruct.y  = ccpp->origin_switch_y + ccpp->image_info.logo_width;
			break;
		case DEG_180:
			PrtStruct.x  = ccpp->origin_switch_x + (ccpp->image_info.logo_width - ccpp->image_info.bar_width) / 2;
			PrtStruct.y  = ccpp->origin_switch_y;
			break;
		case DEG_270:
			PrtStruct.x  = ccpp->origin_switch_x;
			PrtStruct.y  = ccpp->origin_switch_y + (ccpp->image_info.logo_width - ccpp->image_info.bar_width) / 2;
			break;
	}
	return;
}

/**
 * @brief  사각형 최소 길이 이상으로 결정하기 위한 함수 
 * @param  ccpp
 *          - CoOL 기능 관련 프린터 좌표를 조정하기 위한 구조체
 * @return void
 * @remark
 *          - 사용자가 라벨에디터에서 사각형 라인을 그릴 때 길이가 최소 길이 이상이면 그 값을 따르고
 *            최소 길이보다 작게 그릴 경우 최소 길이로 설정.
 */
static short cool_set_rect_length(const struct COOL_CONTROL_PRT_POINT *ccpp)
{
	const short min_length      = ccpp->image_info.logo_height + ccpp->image_info.bar_height + ccpp->image_margin * 2;
	const short original_length = StrStruct.StrHeight + ccpp->image_margin * 2;

	return original_length < min_length ? min_length : original_length;
}

/**
 * @brief   
 * @param  ccpp
 *          - CoOL 기능 관련 프린터 좌표를 조정하기 위한 구조체
 * @return void
 * @remark
 */
void cool_set_control_prt_point(struct COOL_CONTROL_PRT_POINT *ccpp)
{

	ccpp->origin_x = ccpp->origin_switch_x = PrtStruct.x;
	ccpp->origin_y = ccpp->origin_switch_y = PrtStruct.y;
	ccpp->print_enable = COOL_PRINT_ENABLE_Y;

	switch(PrtStruct.Rot)
	{
		case DEG_0:
			ccpp->control_x = ccpp->origin_x;
			ccpp->control_y = ccpp->origin_y;

			ccpp->width   = StrStruct.StrWidth;
			ccpp->height  = StrStruct.StrHeight = cool_set_rect_length(ccpp);

			if(ccpp->control_y + ccpp->height > PrtStruct.PrintableHeight)
			{
				ccpp->print_enable = COOL_PRINT_ENABLE_N;
			}
			break;
		case DEG_90:
			ccpp->control_x = ccpp->origin_x + StrStruct.StrHeight;
			ccpp->control_y = ccpp->origin_y;

			ccpp->width   = StrStruct.StrHeight = cool_set_rect_length(ccpp);
			ccpp->height  = StrStruct.StrWidth;

			if(ccpp->control_x - ccpp->width < PRINT_POSITION_X_MIN)
			{
				ccpp->print_enable = COOL_PRINT_ENABLE_N;
			}
			else
			{
				ccpp->origin_switch_x = ccpp->control_x - ccpp->width;
			}
			break;
		case DEG_180:
			ccpp->control_x = ccpp->origin_x + StrStruct.StrWidth;
			ccpp->control_y = ccpp->origin_y + StrStruct.StrHeight;

			ccpp->width   = StrStruct.StrWidth;
			ccpp->height  = StrStruct.StrHeight = cool_set_rect_length(ccpp);

			if(ccpp->control_y - ccpp->height < PRINT_POSITION_Y_MIN)
			{
				ccpp->print_enable = COOL_PRINT_ENABLE_N;
			}
			else
			{
				ccpp->origin_switch_y = ccpp->control_y - ccpp->height;
			}
			break;
		case DEG_270:
			ccpp->control_x = ccpp->origin_x;
			ccpp->control_y = ccpp->origin_y + StrStruct.StrWidth;

			ccpp->width   = StrStruct.StrHeight = cool_set_rect_length(ccpp);
			ccpp->height  = StrStruct.StrWidth;

			if(ccpp->origin_x + ccpp->width > PrtStruct.PrintableWidth)
			{
				ccpp->print_enable = COOL_PRINT_ENABLE_N;
			}
			break;
		default:
			break;
	}
	PrtStruct.x = ccpp->origin_switch_x;
	PrtStruct.y = ccpp->origin_switch_y;

	return;
}

/**
 * @brief  CoOL 기능 관련(Field : 50) - Rotate 각도에 따라 사각형 그리는 함수
 * @param  ccpp
 *          - CoOL 기능 관련 프린터 좌표를 조정하기 위한 구조체
 * @return void
 * @remark
 */
void cool_print_rect_outline(const struct COOL_CONTROL_PRT_POINT *ccpp)
{
	if(ccpp->print_enable == COOL_PRINT_ENABLE_N)
	{
		return;
	}
	PrtRect(ccpp->origin_switch_x, ccpp->origin_switch_y, ccpp->origin_switch_x + ccpp->width, ccpp->origin_switch_y + ccpp->height, 3, BLACK);

	return;
}

/**
 * @brief  CoOL 기능 관련(Field : 50) - Rotate 각도에 따라 로고 & 하단 바(bar) 그리는 함수 
 * @param  ccpp
 *          - CoOL 기능 관련 프린터 좌표를 조정하기 위한 구조체
 * @return void
 * @remark
 */
void cool_print_logo_bar(const struct COOL_CONTROL_PRT_POINT *ccpp)
{
	const short logo_bar_width_diff   = ccpp->image_info.logo_width - ccpp->image_info.bar_width;
	const short logo_bar_height_total = ccpp->image_info.logo_height + ccpp->image_info.bar_height;

	reverse_percent = COOL_REVERSE_PERCENT_VALUE;

	if(ccpp->print_enable == COOL_PRINT_ENABLE_N)
	{
		return;
	}

	PrtStruct.x =	ccpp->control_x;
	PrtStruct.y =	ccpp->control_y;
	
	switch(PrtStruct.Rot)
	{
		case DEG_0:
			// Print LOGO
			PrtStruct.x += ccpp->image_margin;
			PrtStruct.y += (ccpp->height - logo_bar_height_total) / 2;
			BitmapOut(0x21);

			// Print Bar				
			PrtStruct.x +=	logo_bar_width_diff / 2;
			PrtStruct.y +=	ccpp->image_info.logo_height;
			BitmapOut(0x22);
			break;
		case DEG_90:
			// Print LOGO
			PrtStruct.x -= (((ccpp->width + logo_bar_height_total) / 2) - ccpp->image_info.bar_height);
			PrtStruct.y += ccpp->image_margin;
			BitmapOut(0x21);

			// Print Bar
			PrtStruct.x -= ccpp->image_info.bar_height;
			PrtStruct.y += logo_bar_width_diff / 2;
			BitmapOut(0x22);
			break;
		case DEG_180:
			// Print LOGO
			PrtStruct.x -= (ccpp->image_margin + ccpp->image_info.logo_width);
			PrtStruct.y -= (((ccpp->height + logo_bar_height_total) / 2) - ccpp->image_info.bar_height);
			BitmapOut(0x21);

			// Print Bar
			PrtStruct.x += logo_bar_width_diff / 2;
			PrtStruct.y -= ccpp->image_info.bar_height;
			BitmapOut(0x22);
			break;
		case DEG_270:
			// Print LOGO
			PrtStruct.x += (ccpp->width - logo_bar_height_total) / 2;
			PrtStruct.y -= (ccpp->image_margin + ccpp->image_info.logo_width);
			BitmapOut(0x21);

			// Print Bar
			PrtStruct.x += ccpp->image_info.logo_height;
			PrtStruct.y += logo_bar_width_diff / 2;
			BitmapOut(0x22);
			break;
	}
	cool_set_str_position(ccpp);

	return;
}

/**
 * @brief  CoOL 기능 관련(Field : 50) - Rotate 각도에 따라 하단 바(bar)만 그리는 함수 
 * @param  ccpp
 *          - CoOL 기능 관련 프린터 좌표를 조정하기 위한 구조체
 * @return void
 * @remark
 */
void cool_print_bar(const struct COOL_CONTROL_PRT_POINT *ccpp)
{
	const short logo_bar_width_diff = ccpp->image_info.logo_width - ccpp->image_info.bar_width;

	reverse_percent = COOL_REVERSE_PERCENT_VALUE;

	if(ccpp->print_enable == COOL_PRINT_ENABLE_N)
	{
		return;
	}

	PrtStruct.x = ccpp->control_x;
	PrtStruct.y = ccpp->control_y;

	switch(PrtStruct.Rot)
	{
		case DEG_0:
			PrtStruct.x += (logo_bar_width_diff / 2 + ccpp->image_margin);
			PrtStruct.y += (ccpp->height - ccpp->image_info.bar_height) / 2;
			break;
		case DEG_90:
			PrtStruct.x -= (ccpp->width + ccpp->image_info.bar_height) / 2;
			PrtStruct.y += (logo_bar_width_diff / 2 + ccpp->image_margin);
			break;
		case DEG_180:
			PrtStruct.x -= (ccpp->image_margin + ccpp->image_info.logo_width - logo_bar_width_diff / 2);
			PrtStruct.y -= (ccpp->height + ccpp->image_info.bar_height) / 2;
			break;
		case DEG_270:
			PrtStruct.x += (ccpp->width - ccpp->image_info.bar_height) / 2;
			PrtStruct.y -= (ccpp->image_info.logo_width + ccpp->image_margin - logo_bar_width_diff / 2);
			break;
		default:
			break;
	}
	// Print Bar								
	BitmapOut(0x22);

	cool_set_str_position(ccpp);

	return;
}

/**
 * @brief  CoOL 기능 관련(Field : 50) - Rotate 각도에 따라 Font Str 그리는 함수 
 * @param  ccpp
 *          - CoOL 기능 관련 프린터 좌표를 조정하기 위한 구조체
 * @return void
 * @remark
 */
void cool_print_font_str(const struct COOL_CONTROL_PRT_POINT *ccpp, const char backup_magx, const char backup_magy)
{
	StrStruct.StrWidth -= ccpp->image_margin*2;
	StrStruct.StrHeight-= ccpp->image_margin*2;

	PrtStruct.x +=  ccpp->image_margin;
	PrtStruct.y +=	ccpp->image_margin;

	StrStruct.Magx = backup_magx;
	StrStruct.Magy = backup_magy;

	if(ccpp->print_enable == COOL_PRINT_ENABLE_Y)
	{
		FontStrOut(&PrtStruct.StrBuffer[6]);
	}

	return;
}

/**
 * @brief  field_id가 50번일 때 처리하는 함수 
 * @param  void
 * @return void
 * @remark
 */
void cool_field_id_bonus_point_func(void)
{
	const short image_margin = 4;
	char backup_magx, backup_magy;

	struct COOL_CONTROL_PRT_POINT ccpp = {
		.image_margin = image_margin,
		.image_info = { Image_Form_logo[0], Image_Form_logo[1], Image_Form_bar[0], Image_Form_bar[1] }
	};

	if(PrtStruct.StrBuffer[0]){
		backup_magx = StrStruct.Magx;
		backup_magy = StrStruct.Magy;
		StrStruct.Magx = 1;
		StrStruct.Magy = 1;

		cool_set_control_prt_point(&ccpp);

		// Print Outline
		cool_print_rect_outline(&ccpp);

		if(PrtStruct.StrBuffer[0] == '1') // Print Logo and Bar
		{
			cool_print_logo_bar(&ccpp);

		}
		else if(PrtStruct.StrBuffer[1] == '1') // Print Bar
		{
			cool_print_bar(&ccpp);
		}
		
		// Print Text								
		cool_print_font_str(&ccpp, backup_magx, backup_magy);
	}
	
	return;
}
#endif