/*****************************************************************************
 |*
 |*  Copyright		:	(c) 2002 CAS
 |*  Filename		:	dsp_lcd32.c
 |*  Version		:	0.1
 |*  Programmer(s)	:	Hong Yi-Phyo (HYP)
 |*  Created		:	2003/04/14
 |*  Description		:
 |*
 ****************************************************************************/

#include <string.h>
#include <stdio.h>
#include "globals.h"

#if defined(_USE_LCD37D_40DOT_) || defined(_USE_LCD26d_16x96_)

#include "commbuf.h"
#include "prt_common.h"
#include "prtdrv.h"
#include "bar_common.h"
#include "prt_font.h"
#include "dsp_common.h"
#include "timer.h"
#include "flash.h"
#include "ram.h"
#include "prt_cnvtable.h"
#include "dsp_lcd37d_40dot.h"
#include "dsp_lcd26d_16x96.h"
#include "main.h"
#include "initial.h"
#include "initial_glb.h"
#include "mode_main.h"
#include "dsp_vfd7.h"

#ifdef USE_CHN_FONT
HUGEDATA INT8U LCD_rot[72];
extern void Bit_RotateMCW(INT8U *src,INT16S src_x,INT16S src_y,INT8U *dst,INT16S *dest_x,INT16S *dest_y);
#endif

HUGEDATA INT8U DspScratchPad[DSP_SCRATCH_PAD_SIZE];            // 8
HUGEDATA INT8U DspConPortValue;

void LCD_Write_data(unsigned short addr, char cbyte);


void Delay_10(void){ //40 us
	INT32U i;
	for(i=0; i<0x30; i++);
}

void Delay_10us(void)
{
	INT32U i;
	for(i=0; i<0x75; i++); //test 100119
}

/********************************************************
 Reset (Initialization)
 a. Display Off
 b. Display Start Line : First Line
 c. Satatic Drive : Off
 d. Column Address Counter : Address 0
 e. Page Address Register : Page 0
 f. Select Duty : 1/32
 g. Aelect ADC : Forword (ADC Command D0 = '0',ADC Status Flag = '1')
 h. Read Modify Write : Off
 ********************************************************/
void LCD_Reset(void)
{
}

void LCD_Write_send(char a, char byte, char cs)
{
}

void LCD_Write_data(unsigned short addr, char byte)
{
}

void Dsp_Clear(void)
{
}


INT8U KeyFlagForDispRst = TRUE;//HYP 20060703

INT16U DspAutoTimer100ms=0;
void Dsp_AUTOP_Diffuse(void)
{
}

#ifdef USE_CL3000_LCD_ESD_TEST
extern INT8U Disp_reset_flag;
#endif
void Dsp_Diffuse(void)
{
	//VFD7_Diffuse();
	DSP_LCD7_char_diffuse();
#ifdef USE_CL3000_LCD_ESD_TEST
	static INT16U dspTimer100ms = 0;
	if (Disp_reset_flag)
	{
		if (10 < (INT16U)(SysTimer100ms - dspTimer100ms)) {//1000 ms Reflash
			dspTimer100ms = SysTimer100ms;
			VFD7_Reset(Disp_reset_flag);
		}
	}
#endif
}

INT8U Chess_DEFAULT[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
INT8U Chess_0x00[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
INT8U Chess_0xFF[] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
void LCD_Check(unsigned char x, unsigned char y)
{
}

void LCD_Init(void)
{
    INT8U i;

	Dsp_Clear();	// 위치 변경 : display on 후 바로 clear함
	Dsp_FontInit();

	for (i=0; i<DSP_MAX_PAGE; i++) {
		Dsp_SetPage(i);
		Dsp_Fill_Buffer(0x00);
	}
	Dsp_SetPage(DSP_DEFAULT_PAGE);
}

ROMDATA	unsigned short mask[] = {0xff00,0xfe01,0xfc03,0xf807,0xf00f,0xe01f,0xc03f,0x807f};
ROMDATA	unsigned char heightmask[] = {0xff,0xfe,0xfc,0xf8,0xf0,0xe0,0xc0,0x80,0x00};

void LCD_BufferWrite(POINT p, SIZE m, char type, HUGEDATA unsigned char* ptr)
{
	unsigned short startaddr, curaddr, i, j;
	unsigned char offset, heightB, limit, data;
	char fsize;
	word2byteunion b;

//Check boundry
	if(p.y < 0 || p.y > (LCD_X_MARGIN - m.height)) return;
//	if(p.x < 0 || p.x > (LCD_Y_MARGIN - m.width)) return;
//HYP 20050614
	if(p.x < 0) return;
	if((p.x + m.width)> LCD_Y_MARGIN) {
		//return;
		if (p.x < LCD_Y_MARGIN) {
			m.width = LCD_Y_MARGIN - p.x;
		} else {
			return;
		}
	}

	offset = (p.y%8);
	heightB = (type & 0x0f);
	startaddr = (p.x * (LCD_X_MARGIN / 8) + p.y / 8);

	for(i=0; i<m.width; i++)
	{
		fsize = m.height;
		curaddr = startaddr;
		for(j=heightB-1; j>=0; j--)
		{
			if(fsize>8) limit = 8;
			else if(fsize<=0) break;
			else limit = fsize;
			b.twobyte.l_byte = DspBuf[DspStruct.Page][curaddr];
			b.twobyte.h_byte = DspBuf[DspStruct.Page][curaddr+1];
			b.aword &= (mask[offset]|(heightmask[limit]<<(8-offset)));
			data = ptr[i*heightB+j];
			//if (DspStruct.Reverse) data = (~data);
//Insert(delete) curser
		if (DspStruct.InsertCur && i < 2) data ^= 0xff;
			if (DspStruct.DeleteCur) data = (~data);
			b.aword |= ((data&(~heightmask[limit]))<<offset);
			DspBuf[DspStruct.Page][curaddr] = b.twobyte.l_byte;
			DspBuf[DspStruct.Page][curaddr+1] = b.twobyte.h_byte;
			if(DspStruct.DirectDraw)
			{
				LCD_Write_data(curaddr,DspBuf[DspStruct.Page][curaddr]);
			}
            fsize -= 8;
			curaddr++;
			if(fsize<8)fsize = 0;//test yoo 010107
		}
	//HYP 20040921
//	if(DspStruct.DirectDraw)
	if((DspStruct.DirectDraw) && (offset != 0))
	{
		LCD_Write_data(curaddr,DspBuf[DspStruct.Page][curaddr]);
	}
		startaddr += (LCD_X_MARGIN / 8);
	}
}

extern void VFD7_display_char_graphic(INT16U x, char cData);
INT8U Dsp_Write_1Char(POINT p, char c)
{
	SIZE m;
	HUGEDATA unsigned char *font_ptr;
	HUGEDATA	INT8U *fontw_ptr;
	short font_bytes;

#ifdef USE_CHN_FONT
	INT16S w,fw;
#endif
//	PutCharTxBuf(&CommBuf,c);//////////////////
	if ((unsigned char)c<DspStruct.FirstCode1) c=DspStruct.FirstCode1;

	fontw_ptr = DspStruct.PtrWidth1 + ((unsigned char)c-DspStruct.FirstCode1);
	font_bytes = DspStruct.FHbits1*DspStruct.FWBytes1;
#ifdef USE_CHN_FONT
	if (DspStruct.Id1==3) {
//	font_bytes=16;
		font_ptr = DspStruct.PtrFont1 + ((INT8U)c - DspStruct.FirstCode1)*font_bytes;
	} else {
	if(DspStruct.PreRot1 == PREROT_ONLY_90DEG) {
			font_ptr = DspStruct.PtrFont1 + ((INT8U)c - DspStruct.FirstCode1)*font_bytes;
	} else {//PREROT_0DEG_90DEG
		font_ptr = DspStruct.PtrFont1 + (DspStruct.TotalCharNum1 + (INT8U)c - DspStruct.FirstCode1)*font_bytes;
	}
	}
#else
	if(DspStruct.PreRot1 == PREROT_ONLY_90DEG) {
		font_ptr = DspStruct.PtrFont1 + ((INT8U)c - DspStruct.FirstCode1)*font_bytes;
	} else {//PREROT_0DEG_90DEG
		font_ptr = DspStruct.PtrFont1 + (DspStruct.TotalCharNum1 + (INT8U)c - DspStruct.FirstCode1)*font_bytes;
	}
#endif
	m.height = DspStruct.Wbits1;
	if(DspStruct.FixedWidth == 0)
	{
		m.width = *fontw_ptr;
	}
	else
	{
		m.width = DspStruct.Hbits1;
	}
#ifdef _USE_LCD37D_40DOT_
// CL5200J의 Font width를 5로 제한
	if (m.width > LCD_CHAR_FONT_WIDTH)
		m.width = LCD_CHAR_FONT_WIDTH;
#endif
#ifdef USE_CHN_FONT
	if (DspStruct.Id1>=3) {
	w=DspStruct.Hbits1;
	fw=w>>3;
	if (w&0x07) fw++;
	m.width  = DspStruct.Wbits1;
	m.height = DspStruct.Hbits1;
	LCD_BufferWrite(p,m,fw,font_ptr);
	} else {
	LCD_BufferWrite(p,m,DspStruct.FWBytes1,font_ptr);
	}
#else
	LCD_BufferWrite(p,m,DspStruct.FWBytes1,font_ptr);
#endif
	return m.width;
}

#if defined(USE_ARAB_FONT) && defined(_USE_LCD37D_40DOT_)
INT8U Dsp_Write_1Char_arab(POINT p, char c)
{
	INT8U width = LCD_CHAR_FONT_WIDTH; // character 수

	if ((c >= 0x00) && (c < 0x80))
	{
		VFD7_display_char_graphic(p.x, c);
	}
	else
	{
		VFD7_display_char_graphic(p.x, '^');
	}
	return width;
}
#endif

#ifdef USE_VIETNAM_FONT
INT8U Dsp_Write_1Char16(POINT p, INT16U c)
{
    SIZE m;
    HUGEDATA 	INT8U *font_ptr;
    HUGEDATA	INT8U *fontw_ptr;
    short font_bytes;

	//	PutCharTxBuf(&CommBuf,c);//////////////////
    fontw_ptr = DspStruct.PtrWidth1 + ((INT16U)c-DspStruct.FirstCode1);
    font_bytes = DspStruct.FHbits1*DspStruct.FWBytes1;
    if(DspStruct.PreRot1 == PREROT_ONLY_90DEG) {
        font_ptr = DspStruct.PtrFont1 + ((INT16U)c - DspStruct.FirstCode1)*font_bytes;
    } else {//PREROT_0DEG_90DEG
        font_ptr = DspStruct.PtrFont1 + (DspStruct.TotalCharNum1 + (INT16U)c - DspStruct.FirstCode1)*font_bytes;
    }

    m.height = DspStruct.Wbits1;
    if(DspStruct.FixedWidth == 0)
    {
        m.width = *fontw_ptr;
    }
    else
    {
        m.width = DspStruct.Hbits1;
    }
    LCD_BufferWrite(p,m,DspStruct.FWBytes1,font_ptr);
    return m.width;
}
#endif

#if defined(USE_ARAB_FONT) || defined(HEBREW_FONT)
INT8U Dsp_Write_1CharLong(POINT p, INT16U c)
{
    SIZE m;
    HUGEDATA 	INT8U *font_ptr;
    HUGEDATA	INT8U *fontw_ptr;
    short font_bytes;

	//	PutCharTxBuf(&CommBuf,c);//////////////////
    fontw_ptr = DspStruct.PtrWidth1 + ((INT16U)c-DspStruct.FirstCode1);
    font_bytes = DspStruct.FHbits1*DspStruct.FWBytes1;
    if(DspStruct.PreRot1 == PREROT_ONLY_90DEG) {
        font_ptr = DspStruct.PtrFont1 + ((INT16U)c - DspStruct.FirstCode1)*font_bytes;
    } else {//PREROT_0DEG_90DEG
        font_ptr = DspStruct.PtrFont1 + (DspStruct.TotalCharNum1 + (INT16U)c - DspStruct.FirstCode1)*font_bytes;
    }

    m.height = DspStruct.Wbits1;
    if(DspStruct.FixedWidth == 0)
    {
        m.width = *fontw_ptr;
    }
    else
    {
        m.width = DspStruct.Hbits1;
    }
	LCD_BufferWrite(p,m,DspStruct.FWBytes1,font_ptr);
    return m.width;
}
#endif


INT8U Dsp_Write_2Char(POINT p, INT16U chw)
{
	CWD offset;
#ifdef USE_CHN_FONT
    INT16S   w,h,fw;
#endif
    SIZE m;
    short font_bytes;
    HUGEDATA INT8U *font_ptr;

    m.height = DspStruct.Hbits2;
    m.width = DspStruct.Wbits2;

    //Use only Korean 2byte font
    //if you want to display China font, consider font map
    offset.word = chw - DspStruct.FirstCode2;// relative
#ifdef USE_BIG5_CHN_FONT
	if (offset.byte.left >= 0x26) offset.byte.left -= 0x02; 	//skip for 0xc740 ~ 0xc8ff
	if (offset.byte.right >= 0x60) offset.byte.right -= 0x20; 	//skip for 0x0080 ~ 0x009f
	offset.word = ((INT16U)offset.byte.left * 0xa0) + offset.byte.right;	//
#else
	offset.word = ((INT16U)offset.byte.left * 0x60) + offset.byte.right;	//
#endif

    //HYP 20040511
    //Dsp_HanJohabPattern(DspScratchPad,HanWan2JohabConvTable[offset.word],DspStruct.PreRot2);
    //m.height = DspStruct.Hbits2;
    //m.width = DspStruct.Wbits2;
    //LCD_BufferWrite(p,m,DspStruct.FWBytes2,DspScratchPad);
    if (DspStruct.Font2Type == TWO_BYTE_FONT_WAN) {//wansung
	font_bytes = DspStruct.FHbits2 * DspStruct.FWBytes2;
	if(offset.word < DspStruct.TotalCharNum2) {
#ifdef USE_CHN_FONT
	        font_ptr = DspStruct.PtrFont2 + (INT32U)offset.word * font_bytes;
		Bit_RotateMCW((INT8U *)font_ptr,(INT16S)DspStruct.Wbits2,(INT16S)DspStruct.Hbits2,
			      (INT8U *)LCD_rot,(INT16S *)&w,(INT16S *)&h);
		fw=w>>3;
		if (w&0x07) fw++;
		m.width  = w;
		m.height = h;
		LCD_BufferWrite(p,m,fw,LCD_rot);
#else
		if(DspStruct.PreRot2 == PREROT_ONLY_90DEG) {
		        font_ptr = DspStruct.PtrFont2 + (INT32U)offset.word * font_bytes;
		} else {//PREROT_0DEG_90DEG
		        font_ptr = DspStruct.PtrFont2 + (DspStruct.TotalCharNum2 + (INT32U)offset.word) * font_bytes;
		}
		LCD_BufferWrite(p,m,DspStruct.FWBytes2,font_ptr);
#endif
	} else {
#ifndef USE_CHN_FONT
		//HYP 20050304
		//Dsp_HanJohabPattern(DspScratchPad,0xffff,0);//output 'X' pattern(only 16x16, 24x24)
		if(((chw == 0xA1C9) && (Startup.country==COUNTRY_KR))||((chw == 0xA1CC)&&(get_global_bparam(GLOBAL_SALE_SETUP_FLAG4)&0x08)))
		{
			Dsp_HanSpecialPattern(DspScratchPad,chw,DspStruct.PreRot2);
		} else {
			Dsp_HanJohabPattern(DspScratchPad,0xffff,0);//output 'X' pattern(only 16x16, 24x24)
		}
		LCD_BufferWrite(p,m,DspStruct.FWBytes2,DspScratchPad);
#endif
	}
    } else {//johab
#ifndef USE_CHN_FONT
	if((chw > 0xA4A0) && (chw <0xA4BF)) {//cho sung for automata
		Dsp_HanJohabPattern(DspScratchPad,HanWan2JohabConvTable_Chosung[(INT8U)(chw-0xA4A1)],DspStruct.PreRot2);
	} else {
		if(offset.word < (2350+50)) {//HYP 20040513 conv table max value
			Dsp_HanJohabPattern(DspScratchPad,HanWan2JohabConvTable[offset.word],DspStruct.PreRot2);
		} else {
			//HYP 20050304
			//Dsp_HanJohabPattern(DspScratchPad,0xffff,0);//output 'X' pattern(only 16x16, 24x24)
		if(((chw == 0xA1C9) && (Startup.country==COUNTRY_KR))||((chw == 0xA1CC)&&(get_global_bparam(GLOBAL_SALE_SETUP_FLAG4)&0x08))) {
				Dsp_HanSpecialPattern(DspScratchPad,chw,DspStruct.PreRot2);
			} else {
				Dsp_HanJohabPattern(DspScratchPad,0xffff,0);//output 'X' pattern(only 16x16, 24x24)
			}
		}
	}
	LCD_BufferWrite(p,m,DspStruct.FWBytes2,DspScratchPad);
#endif
	}
	return m.width;
}

void Dsp_Fill_Buffer(unsigned char x)
{
	VFD7_Fill_Buffer_CHAR(x);
}

void LCD_DrawBackgroundSaleMode(void)
{
	Dsp_Fill_Buffer(0x00);
	Dsp_Diffuse();
}

void LCD_DrawBackgroundProgramMode(void)
{
	Dsp_Fill_Buffer(0x00);
	Dsp_Diffuse();
}

void LCD_DrawBackgroundScrollMsg(void)
{
	Dsp_Fill_Buffer(0x00);
	Dsp_Diffuse();
}

void Dsp_ReDraw(short sx, short sy, short ex, short ey)
{
}

#endif



