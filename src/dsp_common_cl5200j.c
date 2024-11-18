/*****************************************************************************
 |*
 |*  Copyright		:	(c) 2002 CAS
 |*  Filename		:	dsp_common.c
 |*  Version		:	0.1
 |*  Programmer(s)	:	Cho Dae Kun (chodk)
 |*  Created		:	2003/05/20
 |*  Description		:
 |*
 ****************************************************************************/
#include <stdio.h>
#include <string.h>
#include "globals.h"
#include "commbuf.h"
#include "prt_common.h"
#include "bar_common.h"
#include "prt_font.h"
#include "prtdrv.h"
#include "timer.h"
#include "flash.h"
#include "ram.h"
#include "prt_cnvtable.h"
#include "dsp_common.h"
#include "dsp_app.h"
#include "initial.h"
#include "flash_app.h"
#include "main.h"
#include "initial_glb.h"
#include "dsp_vfd7.h"
#include "dsp_lcd20848.h"
#ifdef _USE_LCD32_
#include "dsp_lcd32.h"
#endif


INT8U DSP_MENU_FONT_ID;
INT8U DSP_PLU_FONT_ID;
INT8U DSP_SYSTEM_FONT_ID;
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
#if defined (USE_ARAB_FONT) || defined (HEBREW_FONT)
INT8U DSP_6_BY_8_ENG_FONT_ID;
#endif
#endif

NEARDATA DSP_STRUCT DspStruct;
HUGEDATA DSP_FONT1_STRUCT DspFont1Struct[MAX_DSP_FONT1_NUM];
HUGEDATA DSP_FONT2_STRUCT DspFont2Struct[MAX_DSP_FONT2_NUM];

#ifdef USE_CHN_FONT
HUGEDATA INT8U DSP_SYSFONT[16*256];	// 8*256  +
void Bit_RotateMCW(INT8U *src,INT16S src_x,INT16S src_y,INT8U *dst,INT16S *dest_x,INT16S *dest_y);
#endif


void wait_sleep(long period)
{
	long di;
	for(di=0; di<period; ++di);
}

POINT point(INT16S y, INT16S x)
{
    POINT p;

    p.y = y;
    p.x = x;
    return p;
}


void Dsp_InitHardware(void)
{
	VFD7_Init();  	//first init LCD-7 for prevent display garbage
	LCD_Init();
	Dsp_Fill_Buffer(0x55);
	Dsp_Diffuse();
	Dsp_Fill_Buffer(0x00);
	Dsp_Diffuse();
}

void Dsp_Write_String(POINT p, HUGEDATA char *str)
{
	if(DspStruct.StrType == ONE_BYTE_FONT_STR) {
#ifdef USE_VIETNAM_FONT
		Cvt_VietnamPreview((INT8U *)str, strlen(str), 0, 0);
		Dsp_Write_1string16(p, VietnamCode_buf);
#elif defined(USE_THAI_FONT)
		Cvt_ThaiPreview((INT8U *)str, strlen(str), 0, 0);		   
		Dsp_Write_1string32(p, ThaiCode_buf);
#else
		Dsp_Write_1string(p,str);
#endif
	} else {
		Dsp_Write_2string(p,str);
	}
}
#ifdef USE_VIETNAM_FONT
void Dsp_Write_1string16(POINT p, HUGEDATA INT16U *str)
{
	INT8U i;

	while((*str) != '\0') {
		if(p.x < 256)
		{
			p.x += Dsp_Write_1Char16(p, (INT16U)*str);
			for (i = 0; i<DspStruct.InterChSp; i++) {
				Dsp_Write_1Char16(p, 0x0020);
				p.x ++;
			}
			str++;
		} else {
			break;
		}
	}
}
#endif

#ifdef USE_THAI_FONT
void Dsp_Write_1string32(POINT p, HUGEDATA INT32U *str)
{
	INT8U i;
		
	while((*str) != '\0') {
		if(p.x < 256)
		{
			p.x += Dsp_Write_1Char32(p, (INT32U)*str);
			for (i = 0; i<DspStruct.InterChSp; i++) {
				Dsp_Write_1Char32(p, 0x00000020);
				p.x ++;
			}
			str++;
		} else {
			break;
		}
	}
}
#endif

#if defined(USE_ARAB_FONT) || defined(HEBREW_FONT)
void Dsp_Write_1stringLong(POINT p, HUGEDATA INT16U *str)
{
    INT8U i;

    while((*str) != 0) {
		if(p.x < 1024)
        {
#ifdef USE_ENG_BASED_ARAB_SUPPORT
			p.x += Dsp_Write_1CharLong(p, *str);
			for (i=0; i<DspStruct.InterChSp; i++) 
			{
				Dsp_Write_1CharLong(p, 0x0020);
				p.x++;
			}
#else

			p.x -= Dsp_Write_1CharLong(p, *str);
            for (i=0; i<DspStruct.InterChSp; i++) {
				Dsp_Write_1CharLong(p, 0x0020);
				p.x--;
            }
#endif
			str++;
		} else {
			break;
		}
    }
	//    sale_display_date(ON);
}
#endif

void Dsp_Write_1string(POINT p,HUGEDATA char *str)
{
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
	INT8U i;
//	INT16U sx, sy;
	INT16S sx, sy;
	char hbits;//, wbits;

	sx = p.x;
	sy = p.y;

	hbits = DspStruct.Hbits1;

	if (DspStruct.PreRot1 == PREROT_ONLY_90DEG)
	{
		hbits = DspStruct.Wbits1;
	}

	Dsp_PrtStructBackup(DSP_PRTSTR_BACKUP);
	while((*str) != '\0') {
		if(p.x < LCD_Y_MARGIN)
		{
			p.x += Dsp_Write_1Char(p,*str);
			for (i=0; i<DspStruct.InterChSp; i++) {
				Dsp_Write_1Char(p,' ');
				p.x ++;
			}
			str++;
		} else {
			break;
		}
	}
	if (DspStruct.DirectDraw)	// use for lcd320240
	{
		Dsp_DirectDraw(sx, sy, p.x - 1, sy + hbits*StrStruct.Magy - 1);
	}
	Dsp_PrtStructBackup(DSP_PRTSTR_RESTORE);
#else
	INT8U i;
	char wbits;

	wbits = display_font_get_width();
	if ((p.x % wbits) != 0) // 한 char가 5x7 char간 space에 걸치지 않도록 조정 (CL5200J)
	{
		p.x /= wbits;
		p.x *= wbits;
	}

	while((*str) != '\0') {
		//		if(p.x < LCD_Y_MARGIN)// && p.x >= 0)
		//		{
#if defined(USE_ARAB_FONT) && defined(_USE_LCD37D_40DOT_)
		p.x += Dsp_Write_1Char_arab(p,*str);
#else
		p.x += Dsp_Write_1Char(p,*str);
#endif
		for (i=0; i<DspStruct.InterChSp; i++) {
#if defined(USE_ARAB_FONT) && defined(_USE_LCD37D_40DOT_)
			p.x += Dsp_Write_1Char_arab(p,' ');
#else
			p.x += Dsp_Write_1Char(p,' ');
#endif
		}
		str++;
		//		} else {
		//			break;
		//		}
	}
	if (DspStruct.DirectDraw)
	{
		Dsp_Diffuse();
	}
#endif
}

void Dsp_Write_2string(POINT p,HUGEDATA char *str)
{
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
	INT8U i;
	CWD cwd;
	INT16U sx, sy;
	char hbits;

	sx = p.x;
	sy = p.y;
	Dsp_PrtStructBackup(DSP_PRTSTR_BACKUP);

	while((*str) != '\0')
	{
		if(p.x < LCD_Y_MARGIN)
		{
			if((INT8U)(*str) < 0x80)			//영어일때...
			{
				p.x += Dsp_Write_1Char(p,*str);
				for(i=0; i<DspStruct.InterChSp; i++)
				{
					Dsp_Write_1Char(p,' ');
					p.x ++;
				}
				str++;
			}
			else										//한글일때...
			{
				cwd.byte.left = (INT8U)(*str);
				//wansung = (temp<<8);
				str++;
				cwd.byte.right = (INT8U)(*str);
				//wansung |= temp;
				p.x += Dsp_Write_2Char(p,cwd.word);
				str++;
			}
		}
		else
		{
			break;
		}
	}
	if (DspStruct.DirectDraw)	// use for lcd320240
	{
		hbits = DspStruct.Hbits1;
		if (DspStruct.PreRot1 == PREROT_ONLY_90DEG)
		{
			hbits = DspStruct.Wbits1;
		}
		Dsp_DirectDraw(sx, sy, p.x - 1, sy + hbits*StrStruct.Magy - 1);
	}
	Dsp_PrtStructBackup(DSP_PRTSTR_RESTORE);
#else
//	INT16U wansung;
	INT8U i;
	CWD cwd;
	char hbits;

	while((*str) != '\0')
	{
		if(p.x < LCD_Y_MARGIN)
		{
			if((INT8U)(*str) < 0x80) 			//영어일때...
			{
				p.x += Dsp_Write_1Char(p,*str);
				for(i=0; i<DspStruct.InterChSp; i++)
				{
					Dsp_Write_1Char(p,' ');
					p.x ++;
				}
				str++;
			}
			else								//한글일때...
			{
				cwd.byte.left = (INT8U)(*str);
				//wansung = (temp<<8);
				str++;
				cwd.byte.right = (INT8U)(*str);
				//wansung |= temp;
				p.x += Dsp_Write_2Char(p,cwd.word);
				str++;
			}
		}
		else
		{
			break;
		}
	}
	if (DspStruct.DirectDraw)	// use for lcd320240
	{
		hbits = DspStruct.Hbits1;
		if (DspStruct.PreRot1 == PREROT_ONLY_90DEG)
		{
			hbits = DspStruct.Wbits1;
		}
		Dsp_Diffuse();
	}
#endif
}

INT8U DspScanFont(INT8U id, HUGEDATA FONT_FILE_STRUCT   *FontFileStruct) {
#ifdef USE_CHN_FONT
	INT16S sw,sh,fbyte,i,w,h;
	HUGEDATA INT8U *font_ptr,*save_ptr;
#endif
	if(FontFileStruct->TypeOfFile[0] != 'F' || FontFileStruct->TypeOfFile[1] != 'T') return 0xff;

	if(FontFileStruct->ChSetType == ONE_BYTE_FONT_STR) {
		DspFont1Struct[id].FHbits1 = FontFileStruct->FHbits;
		DspFont1Struct[id].FWBytes1 = FontFileStruct->FWBytes;
		DspFont1Struct[id].Wbits1 = FontFileStruct->Wbits;
		if(DspFont1Struct[id].Wbits1 == 0) {
		    DspFont1Struct[id].Wbits1 = DspFont1Struct[id].FWBytes1 * 8;
			DspFont1Struct[id].FixedWidth = 0; ///////
		    DspFont1Struct[id].InterChSp = 1;  //////
		} else {
			DspFont1Struct[id].FixedWidth = 1; ///////
		    DspFont1Struct[id].InterChSp = 0;  //////
		}
		DspFont1Struct[id].Hbits1 = FontFileStruct->Hbits;
		DspFont1Struct[id].FirstCode1 = (INT8U)(FontFileStruct->FirstCode);
		DspFont1Struct[id].PtrFont1 = (HUGEDATA INT8U *)FontFileStruct + FontFileStruct->BitmapOffset;
		DspFont1Struct[id].PtrWidth1 = (HUGEDATA INT8U *)FontFileStruct + FontFileStruct->WidthTableOffset;
		DspFont1Struct[id].PreRot1 = FontFileStruct->PreRot;
		DspFont1Struct[id].TotalCharNum1 = FontFileStruct->TotalCharNum;
#ifdef USE_CHN_FONT
		DspFont1Struct[id].ExistFont=1;
		if (id<3 || id > 5) {
			if(DspFont1Struct[id].PreRot1 == PREROT_ONLY_0DEG) DspFont1Struct[id].ExistFont = 0;
		} else {
			if (id==3) {
				sw = DspFont1Struct[id].Wbits1;
				sh = DspFont1Struct[id].Hbits1;
				fbyte   =DspFont1Struct[id].FHbits1*DspFont1Struct[id].FWBytes1;
				font_ptr=DspFont1Struct[id].PtrFont1;
				save_ptr=(HUGEDATA INT8U *)DSP_SYSFONT;
				for (i=0; i<DspFont1Struct[id].TotalCharNum1; i++) {
					Bit_RotateMCW((INT8U *)font_ptr,(INT16S)sw,(INT16S)sh,
							(INT8U *)save_ptr,(INT16S *)&w,(INT16S *)&h);
					font_ptr+=(long)fbyte;
					save_ptr+=(long)fbyte;
				}
			}
			DspFont1Struct[id].PtrFont1=(HUGEDATA INT8U *)DSP_SYSFONT;
		}
#else
		//HYP 20040513
		if(DspFont1Struct[id].PreRot1 == PREROT_ONLY_0DEG) {
			DspFont1Struct[id].ExistFont = 0;
		} else {//PREROT_0DEG_90DEG || PREROT_ONLY_90DEG
			DspFont1Struct[id].ExistFont = 1;
		}
#endif
	} else {
	  	DspFont2Struct[id].FHbits2 = FontFileStruct->FHbits;
		DspFont2Struct[id].FWBytes2 = FontFileStruct->FWBytes;
		DspFont2Struct[id].Wbits2 = FontFileStruct->Wbits;
		if(DspFont2Struct[id].Wbits2 == 0) {
		    DspFont2Struct[id].Wbits2 = DspFont2Struct[id].FWBytes2 * 8;
		}
		DspFont2Struct[id].Hbits2 = FontFileStruct->Hbits;
		DspFont2Struct[id].FirstCode2 = FontFileStruct->FirstCode;
	    DspFont2Struct[id].Font2Type = FontFileStruct->ChSetType;
	    DspFont2Struct[id].TotalCharNum2 = FontFileStruct->TotalCharNum;
		DspFont2Struct[id].PtrFont2 = (HUGEDATA INT8U *)FontFileStruct + FontFileStruct->BitmapOffset;
	    DspFont2Struct[id].PreRot2 = FontFileStruct->PreRot;
	    DspFont2Struct[id].InterChSp = 1;
#ifdef USE_CHN_FONT
		DspFont2Struct[id].ExistFont=1;
		if (id<3 || id > 5) {
			if(DspFont2Struct[id].PreRot2 == PREROT_ONLY_0DEG) DspFont2Struct[id].ExistFont = 0;
		} else {
		}
#else
		//HYP 20040513
		if(DspFont2Struct[id].PreRot2 == PREROT_ONLY_0DEG) {
			DspFont2Struct[id].ExistFont = 0;
		} else {//PREROT_0DEG_90DEG || PREROT_ONLY_90DEG
			DspFont2Struct[id].ExistFont = 1;
		}
#endif
    }
    return 0;
}

void DspLoadFont1(INT8U id)
{
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
	DspLoadFont2(id);

	if(id < MAX_DSP_FONT1_NUM) {
		if(DspFont1Struct[id].ExistFont == 0){
			if (id==DSP_SYSTEM_FONT_ID) {
				DSP_SYSTEM_FONT_ID=0;
			}
			id=0;
		}
		DspStruct.Id1 = id;
		DspStruct.Wbits1 = DspFont1Struct[id].Wbits1;
		DspStruct.Hbits1 = DspFont1Struct[id].Hbits1;
		DspStruct.FHbits1 = DspFont1Struct[id].FHbits1;
		DspStruct.FWBytes1 = DspFont1Struct[id].FWBytes1;
		DspStruct.FirstCode1 = DspFont1Struct[id].FirstCode1;
		DspStruct.PtrFont1 = DspFont1Struct[id].PtrFont1;
		DspStruct.PtrWidth1 = DspFont1Struct[id].PtrWidth1;
		DspStruct.PreRot1 = DspFont1Struct[id].PreRot1;
		DspStruct.TotalCharNum1 = DspFont1Struct[id].TotalCharNum1;
		DspStruct.FixedWidth = DspFont1Struct[id].FixedWidth; ///////
		DspStruct.InterChSp = DspFont1Struct[id].InterChSp;  //////
		DspStruct.Magx = 1;
		DspStruct.Magy = 1;
	}
#else
	if (id >= MAX_DSP_FONT1_NUM) id = 0;

	if(id < MAX_DSP_FONT1_NUM) {
		if(DspFont1Struct[id].ExistFont == 0){
			id=0;
		}
		DspStruct.Id1 = id;
		DspStruct.Wbits1 = DspFont1Struct[id].Wbits1;
		DspStruct.Hbits1 = DspFont1Struct[id].Hbits1;
		DspStruct.FHbits1 = DspFont1Struct[id].FHbits1;
		DspStruct.FWBytes1 = DspFont1Struct[id].FWBytes1;
		DspStruct.FirstCode1 = DspFont1Struct[id].FirstCode1;
		DspStruct.PtrFont1 = DspFont1Struct[id].PtrFont1;
		DspStruct.PtrWidth1 = DspFont1Struct[id].PtrWidth1;
		DspStruct.PreRot1 = DspFont1Struct[id].PreRot1;
		DspStruct.TotalCharNum1 = DspFont1Struct[id].TotalCharNum1;
		DspStruct.FixedWidth = DspFont1Struct[id].FixedWidth; ///////
		DspStruct.InterChSp = DspFont1Struct[id].InterChSp;  //////
		DspStruct.Magx = 1;
		DspStruct.Magy = 1;
	}
#endif
}

void DspLoadFont2(INT8U id)
{
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
	if(id < MAX_DSP_FONT2_NUM) {
		if(DspFont2Struct[id].ExistFont == 0){
			id=3;
		}
		DspStruct.Id2 = id;
		DspStruct.Wbits2 = DspFont2Struct[id].Wbits2;
		DspStruct.Hbits2 = DspFont2Struct[id].Hbits2;
		DspStruct.FHbits2 = DspFont2Struct[id].FHbits2;
		DspStruct.FWBytes2 = DspFont2Struct[id].FWBytes2;
		DspStruct.FirstCode2 = DspFont2Struct[id].FirstCode2;
		DspStruct.PtrFont2 = DspFont2Struct[id].PtrFont2;
		DspStruct.Font2Type = DspFont2Struct[id].Font2Type;
		DspStruct.PreRot2 = DspFont2Struct[id].PreRot2;
		DspStruct.TotalCharNum2 = DspFont2Struct[id].TotalCharNum2;
		//DspStruct.FixedWidth = DspFont2Struct[id].FixedWidth; ///////
		DspStruct.InterChSp = DspFont2Struct[id].InterChSp;  //////
		DspStruct.Magx = 1;
		DspStruct.Magy = 1;
	}
#else
	if (id >= MAX_DSP_FONT1_NUM) id = 0;

	if(id < MAX_DSP_FONT2_NUM) {
		if(DspFont2Struct[id].ExistFont == 0){
			id=0;
		}
		DspStruct.Id2 = id;
		DspStruct.Wbits2 = DspFont2Struct[id].Wbits2;
		DspStruct.Hbits2 = DspFont2Struct[id].Hbits2;
		DspStruct.FHbits2 = DspFont2Struct[id].FHbits2;
		DspStruct.FWBytes2 = DspFont2Struct[id].FWBytes2;
		DspStruct.FirstCode2 = DspFont2Struct[id].FirstCode2;
		DspStruct.PtrFont2 = DspFont2Struct[id].PtrFont2;
		DspStruct.Font2Type = DspFont2Struct[id].Font2Type;
		DspStruct.PreRot2 = DspFont2Struct[id].PreRot2;
		DspStruct.TotalCharNum2 = DspFont2Struct[id].TotalCharNum2;
		//DspStruct.FixedWidth = DspFont2Struct[id].FixedWidth; ///////
		DspStruct.InterChSp = DspFont2Struct[id].InterChSp;  //////
		DspStruct.Magx = 1;
		DspStruct.Magy = 1;
	}
#endif
}

void Dsp_FontSize(char mag_w, char mag_h)
{
	DspStruct.Magx = mag_w;
	DspStruct.Magy = mag_h;
}

#ifdef USE_CHN_FONT
void Bit_RotateMCW(INT8U *src,INT16S src_x,INT16S src_y,INT8U *dst,INT16S *dest_x,INT16S *dest_y)
{
	INT8U	 src_mask,dst_mask;
	INT16S	 src_bw,src_p,src_sp;
	INT16S      dst_x,dst_y,dst_bw,dst_p,dst_mx;
	INT16S	 x,y;

	dst_x=src_y;
	dst_y=src_x;

	dst_bw = dst_x >> 3;
	if (dst_x&0x07) dst_bw++;
	dst_x = dst_bw<<3;
	//==//
	//====
	//////
	*dest_x = dst_x;
	*dest_y = dst_y;

	src_bw=src_x>>3;		// src_bw=src_x / 8;
	if (src_x&0x07) src_bw++;	// if (src_x % 8) src_bw++;
	src_sp  = 0;

	dst_x--;
	dst_mask= 0x80 >> (dst_x&0x07);	// Org
	memset(dst,0x00,(dst_bw*dst_y));
	dst_mx  = dst_bw<<3-1;
	for (y=0; y<src_y; y++) {
		src_mask = 0x80;
		dst_y    = 0;
		dst_p    = (dst_x-y)>>3;
		for (x=0; x<src_x; x++) {
			src_p  = src_sp+(x >> 3);
			if (src[src_p]&src_mask) {
				dst[dst_p+dst_y]|=dst_mask;
			}
			dst_y += dst_bw;
			src_mask>>=1;
			if (src_mask==0) src_mask=0x80;
		}
		src_sp += src_bw;
		dst_mask<<=1;
		if (dst_mask==0) dst_mask=0x01;
	}
}
#endif

void Dsp_FontInit(void)
{

	INT32U addr,pnt,fontaddr;
#if defined(USE_CHN_FONT) || defined(_USE_LCD20848_) || defined(_USE_LCD32_)
	INT8U  fontid;
#endif
	INT8U  fonttype;//,allow_2byte;
	INT16S i,fontnum;
	INT8U	id;

	for(id=0; id < MAX_DSP_FONT1_NUM; id++) {
		DspFont1Struct[id].ExistFont = 0;
	}
	for(id=0; id < MAX_DSP_FONT2_NUM; id++) {
		DspFont2Struct[id].ExistFont = 0;
	}

	Dsp_default_fontload();

	DspStruct.StrType = PrtFlashStruct.DspStrType;
	DspStruct.Typeface = 0x00;
	DspStruct.Reverse  = 0;
	DspStruct.DirectDraw = 1;
	DspStruct.InsertCur = 0;
	DspStruct.DeleteCur = 0;

	addr  =DFLASH_BASE;
	addr += FLASH_FONT_AREA;
	pnt = addr + FONT_NUMBER;
	//	allow_2byte=0;
	fontnum  = (INT16S)Flash_bread(pnt);
	if (fontnum > FONT_MAX_NO) fontnum = FONT_MAX_NO;
	// English
	DspScanFont(0,(HUGEDATA FONT_FILE_STRUCT   *)DspFontAddrHpf);	//DspFontAddr8x6Hpf
#if defined (CL5200_PROJECT)
#if defined(USE_KOR_FONT) || defined(USE_CHN_FONT)
	DspScanFont(3,(HUGEDATA FONT_FILE_STRUCT   *)Dsp2byteFontAddrHpf);	// 번호, 값 표시는 6*8로 표시
	DspScanFont(3,(HUGEDATA FONT_FILE_STRUCT   *)DspFontAddr_3_8x16);	//DspFontAddr8x16Hpf
#else
	DspScanFont(3,(HUGEDATA FONT_FILE_STRUCT   *)DspFontAddr_3_8x16);	//DspFontAddr8x16Hpf
	DspScanFont(4,(HUGEDATA FONT_FILE_STRUCT   *)DspFontAddr_4_12x24);	//DspFontAddr12x24Hpf
#endif
	DspScanFont(8,(HUGEDATA FONT_FILE_STRUCT   *)DspFontAddr12x8Hpf);
#if defined(USE_ARAB_FONT) || defined(HEBREW_FONT)    //20120914 for CL5200 Arab version by kiho
	DspScanFont(7,(HUGEDATA FONT_FILE_STRUCT   *)DspFontAddr6x8ENGHpf);
	DspScanFont(9,(HUGEDATA FONT_FILE_STRUCT   *)DspFontAddr16x8Hpf);
#else
	DspScanFont(9,(HUGEDATA FONT_FILE_STRUCT   *)DspFontAddr20x8Hpf);
#endif
#elif defined (CL5500_PROJECT)
#if defined(USE_KOR_FONT) || defined(USE_CHN_FONT)
	DspScanFont(3,(HUGEDATA FONT_FILE_STRUCT   *)Dsp2byteFontAddrHpf);	// 번호, 값 표시는 6*8로 표시
	DspScanFont(3,(HUGEDATA FONT_FILE_STRUCT   *)DspFontAddr_3_8x16);	//DspFontAddr8x16Hpf
	DspScanFont(4,(HUGEDATA FONT_FILE_STRUCT   *)DspFontAddr_4_12x24);	//DspFontAddr12x24Hpf 
  #ifdef USE_KOR_FONT
	DspScanFont(4,(HUGEDATA FONT_FILE_STRUCT   *)DspFontAddr_4_24x24);	//DspFontAddr12x24Hpf 
  #endif
#else
	DspScanFont(3,(HUGEDATA FONT_FILE_STRUCT   *)DspFontAddr_3_8x16);	//DspFontAddr8x16Hpf
	DspScanFont(4,(HUGEDATA FONT_FILE_STRUCT   *)DspFontAddr_4_12x24);	//DspFontAddr12x24Hpf
#endif    
#else
#if defined(USE_KOR_FONT) || defined(USE_CHN_FONT)
	DspScanFont(0,(HUGEDATA FONT_FILE_STRUCT   *)Dsp2byteFontAddrHpf);
#endif
#endif
	for (i=0; i<fontnum; i++) {
		pnt = addr + FONT_INFO_POINT+FONT_INFO_SIZE*i;
//#ifdef _USE_LCD20848_
//		fontid  =Flash_bread(pnt);
//#endif
		fonttype=Flash_bread(pnt+1);
#ifdef USE_CHN_FONT
		fontid  =Flash_bread(pnt);
		if (fontid>=3 && fontid <= 5) {
			if (fonttype&0x04) {
				fontaddr = Flash_lread(pnt+2);
				fontaddr+= addr;
				fontaddr+= FONT_IMAGE;
//#ifdef _USE_LCD20848_
//				DspScanFont(fontid,(HUGEDATA FONT_FILE_STRUCT *)fontaddr);
//#endif
			}
		} else {
			if (fonttype&0x02) {
				fontaddr = Flash_lread(pnt+2);
				fontaddr+= addr;
				fontaddr+= FONT_IMAGE;
//#ifdef _USE_LCD20848_
//				DspScanFont(fontid,(HUGEDATA FONT_FILE_STRUCT *)fontaddr);
//#endif
			}
		}
#else
		if (fonttype&0x02)
		{
			fontaddr = Flash_lread(pnt+2);
			fontaddr+= addr;
			fontaddr+= FONT_IMAGE;
//#ifdef _USE_LCD20848_
//			DspScanFont(fontid,(HUGEDATA FONT_FILE_STRUCT *)fontaddr);
//#endif
		}
#endif
		//		if (fonttype&0x01) allow_2byte=1;
	}
	DspLoadFont1(DSP_MENU_FONT_ID);
#ifdef USE_SINGLE_LINE_GRAPHIC
	Startup.menu_ygap = 0;
	Startup.menu_xgap = 0;
#else
	Startup.menu_ygap = display_font_get_height();
	Startup.menu_xgap = display_font_get_width();
#endif
	Startup.menu_length = Dsp_GetChars();
	if (DSP_MENU_FONT_ID!=DSP_SYSTEM_FONT_ID) {
		DspLoadFont1(DSP_SYSTEM_FONT_ID);
	}
	DspLoadFont1(DSP_PLU_FONT_ID);
}

void Dsp_default_fontload(void)
{
#ifdef USE_8_16_FONT_MENU_DISPLAY	//CL5200 Iran Display
	DSP_MENU_FONT_ID  = 3;
#else
	DSP_MENU_FONT_ID  = get_global_bparam(GLOBAL_MENU_FONTID);
//	DSP_MENU_FONT_ID  = 3;
#endif
#ifdef CL5200_PROJECT
#if defined(USE_ARAB_FONT) || defined(HEBREW_FONT)    //20120914 for CL5200 Arab version by kiho
	DSP_PLU_FONT_ID   = get_global_bparam(GLOBAL_PLU_FONTID);
	DSP_6_BY_8_ENG_FONT_ID = 7;
#else
	DSP_PLU_FONT_ID   = 3;
#endif
#elif defined(CL5500_PROJECT)
	DSP_PLU_FONT_ID   = get_global_bparam(GLOBAL_PLU_FONTID);
#else
//		DSP_PLU_FONT_ID   = get_global_bparam(GLOBAL_PLU_FONTID);
    DSP_PLU_FONT_ID   = 0;
#endif
//	DSP_SYSTEM_FONT_ID = 2;
    DSP_SYSTEM_FONT_ID = 0;

}

#ifndef USE_CHN_FONT
/* ------------------------------------------------------- */
/*	Font Processing Table for Korean Character	   */
/* ------------------------------------------------------- */
#define CHO		0
#define JUNG	1
#define JONG	2

extern ROMDATA INT8U bul_cntA[3];	//벌수 8x4x4
extern ROMDATA INT8U bul_cntB[3];	//벌수 	7x2x2
extern ROMDATA INT8U chr_cnt[3];	//문자수

extern ROMDATA INT8U table_cho[21];
extern ROMDATA INT8U table_joong[30];
extern ROMDATA INT8U table_jong[30];
extern ROMDATA INT8U bul_choA1[22];
extern ROMDATA INT8U bul_choA2[22];
extern ROMDATA INT8U bul_jungA1[20];
extern ROMDATA INT8U bul_jungA2[20];
extern ROMDATA INT8U bul_jongA[22];
extern ROMDATA INT8U bul_choB1[22];
extern ROMDATA INT8U bul_choB2[22];
extern ROMDATA INT8U bul_jungB1[20];
extern ROMDATA INT8U bul_jungB2[20];
extern ROMDATA INT8U bul_jongB[22];

extern ROMDATA	INT8U	X_16x16Pattern[32];
extern ROMDATA	INT8U	X_24x24Pattern[72];

void Dsp_HanJohabPattern(HUGEDATA INT8U *Pattern,INT16U Kcode, char rot)
{
  	INT8U cho_5bit, jung_5bit, jong_5bit,i;
	INT8U cho_bul, jung_bul, jong_bul, jong_flag,font_bytes;
	INT16U character,pre_rot_font_offset;
	HUGEDATA INT8U	*font_ptr;
	ROMDATA INT8U *bul_cnt,*bul_cho1,*bul_cho2,*bul_jung1,*bul_jung2,*bul_jong;

  	font_bytes = DspStruct.FHbits2 * DspStruct.FWBytes2;

	//HYP 20040512
	if(Kcode == 0xffff) {
		if(DspStruct.FHbits2 == 16){ //Use only 24x24 and 16x16
			bul_cnt=X_16x16Pattern;
		} else {
			bul_cnt=X_24x24Pattern;
		}
		for(i=0; i<font_bytes; i++) {
			Pattern[i]=bul_cnt[i];
		}
		return;
	}

  	cho_5bit = table_cho[(Kcode >> 10) & 0x001F];		/* get cho, joong, jong 5 bit */
  	jung_5bit = table_joong[(Kcode >> 5) & 0x001F];
  	jong_5bit = table_jong[Kcode & 0x001F];

	switch(DspStruct.Font2Type) {
		case 2:   //8x4x4
			bul_cnt = bul_cntA;
			bul_cho1 = bul_choA1;
			bul_cho2 = bul_choA2;
			bul_jung1 = bul_jungA1;
			bul_jung2 = bul_jungA2;
			bul_jong = bul_jongA;
			break;
		case 3:   //7x2x2
			bul_cnt = bul_cntB;
			bul_cho1 = bul_choB1;
			bul_cho2 = bul_choB2;
			bul_jung1 = bul_jungB1;
			bul_jung2 = bul_jungB2;
			bul_jong = bul_jongB;
			break;
		default:
			return;
	}

   	if(jong_5bit == 0){
  	  jong_flag = 0;
      	  cho_bul = bul_cho1[jung_5bit];
      	  jung_bul = bul_jung1[cho_5bit];
  	}
  	else {
   	  jong_flag = 1;
      	  cho_bul = bul_cho2[jung_5bit];
  	  jung_bul = bul_jung2[cho_5bit];
          jong_bul = bul_jong[jung_5bit];
        }

	if (rot == 2) {
		pre_rot_font_offset = 0;
	} else {
		pre_rot_font_offset = DspStruct.TotalCharNum2;
//		pre_rot_font_offset = bul_cnt[CHO]*chr_cnt[CHO] + bul_cnt[JUNG]*chr_cnt[JUNG] +bul_cnt[JONG]*chr_cnt[JONG];
	}

// cho sung
  	character = cho_bul*chr_cnt[CHO] + cho_5bit + pre_rot_font_offset;
  	font_ptr = DspStruct.PtrFont2 + (INT32U)character * font_bytes;
  	for(i = 0; i < font_bytes; i++) {
	   	Pattern[i] = font_ptr[i];
	}
// jung sung
	character = bul_cnt[CHO]*chr_cnt[CHO] + jung_bul*chr_cnt[JUNG] + jung_5bit + pre_rot_font_offset;
  	font_ptr = DspStruct.PtrFont2 + (INT32U)character * font_bytes;
  	for(i = 0; i < font_bytes; i++) {
	 	Pattern[i] |= font_ptr[i];
	}
// jong sung
	if(jong_flag == 1)	{
		character = bul_cnt[CHO]*chr_cnt[CHO] + bul_cnt[JUNG]*chr_cnt[JUNG] +jong_bul*chr_cnt[JONG] + jong_5bit + pre_rot_font_offset;
		font_ptr = DspStruct.PtrFont2 + (INT32U)character * font_bytes;
      	for(i = 0; i < font_bytes; i++) {
      	  	Pattern[i] |= font_ptr[i];
	}
    }
}

void Dsp_HanSpecialPattern(HUGEDATA INT8U *ScratchPad,INT16U Kcode, char rot)
{
	INT8U i,font_bytes;
	ROMDATA INT8U *FontPtr;

  	font_bytes = DspStruct.FHbits2 * DspStruct.FWBytes2;

	if(Kcode == 0xA1C9) {						//℃ in cp949
		if (rot == DEG_0) {
			if(DspStruct.FHbits2 == 12){ 		//Use only 24x24, 16x16, 12x12
				FontPtr=FontCDeg_DEG0_12x12;	//Display font도 ptr font에서 참조?
			} else if(DspStruct.FHbits2 == 16) {
				FontPtr=FontCDeg_DEG0_16x16;
			} else {
				FontPtr=FontCDeg_DEG0_24x24;
			}
		} else {								//DEG_90
			if(DspStruct.FHbits2 == 12){ 		//Use only 24x24, 16x16, 12x12
				FontPtr=FontCDeg_DEG90_12x12;
			} else if(DspStruct.FHbits2 == 16) {
				FontPtr=FontCDeg_DEG90_16x16;
			} else {
				FontPtr=FontCDeg_DEG90_24x24;
			}
		}
		for(i=0; i<font_bytes; i++) {
			ScratchPad[i]=FontPtr[i];
		}
	}
  #ifdef USE_KOREAN_IN_GBR
	else if((Kcode == 0xA1CC)&&(get_global_bparam(GLOBAL_SALE_SETUP_FLAG4)&0x08))	//￡ in cp949
	{
		if (rot == DEG_0)
		{
			if(DspStruct.FHbits2 == 12)
			{			
				FontPtr=FontPOUND_DEG0_12x12;		
			} 
			else if(DspStruct.FHbits2 == 16) 
			{
				FontPtr=FontPOUND_DEG0_16x16;
			} 
			else 
			{
				FontPtr=FontPOUND_DEG0_24x24;
			} 
		} 
		else	//DEG_90
		{
			if(DspStruct.FHbits2 == 12)
			{ 
				FontPtr=FontPOUND_DEG90_12x12;
			} 
			else if(DspStruct.FHbits2 == 16) 
			{
				FontPtr=FontPOUND_DEG90_16x16;
			} 
			else 
			{
				FontPtr=FontPOUND_DEG90_24x24;
			} 
		}
		for(i=0; i<font_bytes; i++) 
		{
			ScratchPad[i]=FontPtr[i];
		}
	}
  #endif		//#ifdef USE_KOREAN_IN_GBR    
}
#endif		//#ifndef USE_CHN_FONT

INT16U Dsp_GetChars(void)
{
	INT16U r;
	char wbits;

	wbits = display_font_get_width();

	if ((DspStruct.InterChSp + wbits) == 0) return 1;
	r = (LCD_Y_MARGIN / (DspStruct.InterChSp + wbits));

    if (r > 53) r = 53;
	return r;
}


void Dsp_SetPage(INT8U page)
{
	if((page >= 0) && (page < DSP_MAX_PAGE)) {
		DspStruct.Page = page;
	}
}

extern INT8U scroll_effect;
void Dsp_ChangeMode(INT8U mode)
{
	switch (mode)
	{
		case DSP_SALE_MODE:
			//Dsp_Fill_Buffer(0x00);
			//Dsp_Diffuse();
			scroll_effect = 0;
#ifdef _USE_LCD20848_ //ck
			UseSaleModeEditFlag = 0;
#endif
			LCD_DrawBackgroundSaleMode();
			break;
		case DSP_PGM_MODE:
			//Dsp_Fill_Buffer(0x00);
			//Dsp_Diffuse();
#ifdef _USE_LCD20848_ //ck
			UseSaleModeEditFlag = 1;
#endif
			LCD_DrawBackgroundProgramMode();
			break;
	}
}

