/*****************************************************************************
|*			
|*  Copyright		:	(c) 2002 CAS
|*  Filename		:	prt_font.c
|*  Version		:	0.1
|*  Programmer(s)	:	Hong Yi-Phyo (HYP)
|*  Created		:	2002/11/11
|*  Description		:	     
|*				
****************************************************************************/
#include <string.h>
#include <stdio.h>
#include "globals.h"
#include "commbuf.h"
#include "bar_common.h"
#include "prt_common.h"
#include "prtdrv.h"
#include "prt_render.h"
#include "prt_font.h"
#include "prt_cnvtable.h"
#include "initial.h"
#include "flash_app.h"
#include "mode_main.h"
#include "prt_line.h"
#include "prt_interpreter.h"
#include "prt_cmdtest.h"
#include "initial_glb.h"

#include "scale_config.h"

INT8U PRT_DEFAULT_FONT_ID;
//HUGEDATA STR_STRUCT  StrStruct;
//HUGEDATA FONT_STRUCT FontStruct;
//HYP 20060412 _huge --> _near for small code size
NEARDATA STR_STRUCT  StrStruct;
NEARDATA FONT_STRUCT FontStruct;

HUGEDATA FONT1_STRUCT Font1Struct[MAX_FONT1_NUM];
HUGEDATA FONT2_STRUCT Font2Struct[MAX_FONT2_NUM];
HUGEDATA STR_FIELD_STRUCT StrFieldStruct[MAX_STR_FIELD_NUM+MAX_BAR_FIELD_NUM];

//HYP 20060424 Fix Byte Align Problem for rendering
//HUGEDATA INT8U	PrtScratchPadMemory[SCRATCH_PAD_SIZE];
//HUGEDATA INT8U	PrtScratchPadFontMemory[SCRATCH_PAD_SIZE_FONT];  //use only font routine
HUGEDATA INT16U	PrtScratchPadMemory[SCRATCH_PAD_SIZE/2];
HUGEDATA INT16U	PrtScratchPadFontMemory[SCRATCH_PAD_SIZE_FONT/2];  //use only font routine
HUGEDATA char	PrtStrBufferMemory[MAX_STR_CHAR_SIZE];
#ifdef USE_QR_ADDTOTAL
HUGEDATA char	PrtBarBufferMemory[MAX_BAR_DATA_BUF_SIZE_QR];
#else
HUGEDATA char	PrtBarBufferMemory[MAX_BAR_DATA_BUF_SIZE];
#endif

#ifdef USE_USA_LABELFORMAT
extern ROMDATA INT8U ImageFormSafeHanding[];
extern ROMDATA INT8U ImageFormTotalPrice[];
extern ROMDATA INT8U ImageFormSavedPrice[];
#endif

//////////////////////////////////////////////////////////////
//for Korean Mode 
//Centi Deg. Font for LCD, PRT
ROMDATA INT8U FontCDeg_DEG0_12x12[2*12] = {
#include "12x12_Prt_949_Wansung_Only_DegC.fnt"
};
ROMDATA INT8U FontCDeg_DEG90_12x12[2*12] = {
#include "12x12_Lcd_949_Wansung_Only_DegC.fnt"
};
ROMDATA INT8U FontCDeg_DEG0_16x16[2*16] = {
#include "16x16_Prt_949_Wansung_Only_DegC.fnt"
};
ROMDATA INT8U FontCDeg_DEG90_16x16[2*16] = {
#include "16x16_Lcd_949_Wansung_Only_DegC.fnt"
};
ROMDATA INT8U FontCDeg_DEG0_24x24[3*24] = {
#include "24x24_Prt_949_Wansung_Only_DegC.fnt"
};
ROMDATA INT8U FontCDeg_DEG90_24x24[3*24] = {
#include "24x24_Lcd_949_Wansung_Only_DegC.fnt"
};

#ifdef USE_KOREAN_IN_GBR	//Pound Font 추가
//POUND. Font for LCD, PRT at PARAMETER 980 set
ROMDATA INT8U FontPOUND_DEG0_12x12[2*12] = {
#include "12x12_Prt_949_Wansung_Only_POUND.fnt"
};
ROMDATA INT8U FontPOUND_DEG90_12x12[2*12] = {
#include "12x12_Lcd_949_Wansung_Only_POUND.fnt"
};
ROMDATA INT8U FontPOUND_DEG0_16x16[2*16] = {
#include "16x16_Prt_949_Wansung_Only_POUND.fnt"
};
ROMDATA INT8U FontPOUND_DEG90_16x16[2*16] = {
#include "16x16_Lcd_949_Wansung_Only_POUND.fnt"
};
ROMDATA INT8U FontPOUND_DEG0_24x24[3*24] = {
#include "24x24_Prt_949_Wansung_Only_POUND.fnt"
};
ROMDATA INT8U FontPOUND_DEG90_24x24[3*24] = {
#include "24x24_Lcd_949_Wansung_Only_POUND.fnt"
};
#endif	//#ifdef USE_KOREAN_IN_GBR

//////////////////////////////////////////////////////////////

//Invalid bit of Font Data must be '0'.
//HYP 20040622
/*
//8x6 font
#define FONT_ADDR_BASIC0		0x102000	//in boot area (Don't erase)
#define FONT_ADDR_BASIC0_SIZE	(256*6+64)
HUGEDATA INT8U DspFontAddrHpf		_at(FONT_ADDR_BASIC0);//[256*6+64]
//ocrb font (only numeric)
#define FONT_ADDR_BASIC1	(FONT_ADDR_BASIC0+FONT_ADDR_BASIC0_SIZE)	
#define FONT_ADDR_BASIC1_SIZE	(10*32*2*3)
HUGEDATA INT8U FontAddrOcrB[10*32*2*3]	_at(FONT_ADDR_BASIC1);
//ocrb font width table
#define FONT_ADDR_BASIC2	(FONT_ADDR_BASIC1+FONT_ADDR_BASIC1_SIZE)
#define FONT_ADDR_BASIC2_SIZE	(30)
HUGEDATA INT8U FontAddrOcrBW[30]		_at(FONT_ADDR_BASIC2);
*/
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
extern ROMDATA INT8U Dsp_Arial_8x6[];
extern ROMDATA INT8U Dsp_Arial_8x16[];
extern ROMDATA INT8U Dsp_Arial_12x8[];
#if defined(USE_ARAB_FONT) || defined(HEBREW_FONT)    //20120914 for CL5200 Arab version by kiho
extern ROMDATA INT8U Dsp_Arial_16x8[];
extern ROMDATA INT8U Dsp_SYSTEM_ENG_8x6[];
#else
extern ROMDATA INT8U Dsp_Arial_20x8[];
#endif
extern ROMDATA INT8U Lcd_3_8x16_1252_Fixedwidth_Normal[];
extern ROMDATA INT8U Lcd_4_12x24_1252_Fixedwidth_Normal[];
#ifdef _USE_LCD32_  // CL5500 DISP
extern ROMDATA INT8U Lcd_4_24x24_1252_Fixedwidth_Normal[];
#endif
#elif defined(_USE_LCD37D_40DOT_)
extern ROMDATA INT8U Dsp_Arial_8x6[];
#endif

#if defined(USE_KOR_FONT) || defined(USE_CHN_FONT)
extern ROMDATA INT8U Dsp_Arial_16x16[];
extern ROMDATA INT8U Prt_3_16x16_1252_Fixedwidth_Normal[];
#endif
extern ROMDATA INT8U Prt_3_8x16_1252_Fixedwidth_Normal[];
extern ROMDATA INT8U Prt_4_12x24_1252_Fixedwidth_Normal[];
extern ROMDATA INT8U Prt_5_6x12_1252_Fixedwidth_Normal[];
#ifdef USE_KOR_FONT
extern ROMDATA INT8U Prt_6_8x24_1252_Fixedwidth_CircleOnly[];
#endif
#ifdef USE_PERSIA_DEFAULT
extern ROMDATA INT8U Prt_6_12x16_1252_Fixedwidth_Normal[];
#endif

extern ROMDATA INT8U Prt_FontAddrOcrB[];
extern ROMDATA INT8U Prt_FontAddrOcrBW[];

HUGEDATA INT8U *FontAddrOcrB	  = (HUGEDATA INT8U *)Prt_FontAddrOcrB;
HUGEDATA INT8U *FontAddrOcrBW	  = (HUGEDATA INT8U *)Prt_FontAddrOcrBW;

#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
HUGEDATA INT8U *DspFontAddrHpf = (HUGEDATA INT8U *)Dsp_Arial_8x6;
HUGEDATA INT8U *DspFontAddr12x8Hpf = (HUGEDATA INT8U *)Dsp_Arial_12x8;
#if defined(USE_ARAB_FONT) || defined(HEBREW_FONT)    //20120914 for CL5200 Arab version by kiho      
HUGEDATA INT8U *DspFontAddr16x8Hpf = (HUGEDATA INT8U *)Dsp_Arial_16x8;
HUGEDATA INT8U *DspFontAddr6x8ENGHpf = (HUGEDATA INT8U *)Dsp_SYSTEM_ENG_8x6;
#else
HUGEDATA INT8U *DspFontAddr20x8Hpf = (HUGEDATA INT8U *)Dsp_Arial_20x8;
#endif
#if defined(USE_CHN_FONT) || defined(USE_KOR_FONT)
HUGEDATA INT8U *DspFontAddr_3_8x16 = (INT8U *)Dsp_Arial_8x16;
#else
HUGEDATA INT8U *DspFontAddr_3_8x16 = (INT8U *)Lcd_3_8x16_1252_Fixedwidth_Normal;
#endif
HUGEDATA INT8U *DspFontAddr_4_12x24 = (INT8U *)Lcd_4_12x24_1252_Fixedwidth_Normal;
#ifdef _USE_LCD32_  // CL5500 DISP
  #ifdef USE_KOR_FONT
HUGEDATA INT8U *DspFontAddr_4_24x24 = (INT8U *)Lcd_4_24x24_1252_Fixedwidth_Normal;
  #endif
#endif
#elif defined(_USE_LCD37D_40DOT_)
HUGEDATA INT8U *DspFontAddrHpf = (HUGEDATA INT8U *)Dsp_Arial_8x6;
#endif
#if defined(USE_KOR_FONT) || defined(USE_CHN_FONT)
  #ifdef USE_BIG5_CHN_FONT
HUGEDATA INT8U *Dsp2byteFontAddrHpf = (HUGEDATA INT8U *)Prt_3_16x16_1252_Fixedwidth_Normal;
  #else
HUGEDATA INT8U *Dsp2byteFontAddrHpf = (HUGEDATA INT8U *)Dsp_Arial_16x16;
  #endif
#endif

#if defined(USE_KOR_FONT) || defined(USE_CHN_FONT)
HUGEDATA INT8U *FontAddr_3_16x16 = (INT8U *)Prt_3_16x16_1252_Fixedwidth_Normal;
#endif
HUGEDATA INT8U *FontAddr_3_8x16 = (INT8U *)Prt_3_8x16_1252_Fixedwidth_Normal;
HUGEDATA INT8U *FontAddr_4_12x24 = (INT8U *)Prt_4_12x24_1252_Fixedwidth_Normal;
HUGEDATA INT8U *FontAddr_5_6x12 = (INT8U *)Prt_5_6x12_1252_Fixedwidth_Normal; 
#if defined(USE_KOR_FONT)
HUGEDATA INT8U *FontAddr_6_8x24 = (INT8U *)Prt_6_8x24_1252_Fixedwidth_CircleOnly;
#endif
#ifdef USE_PERSIA_DEFAULT
HUGEDATA INT8U *FontAddr_6_12x16 = (INT8U *)Prt_6_12x16_1252_Fixedwidth_Normal;
#endif
HUGEDATA INT8U *DspFontAddrHpf;

#ifdef USE_USA_LABELFORMAT
static INT32U image_form_addr(INT16U image_id);
#endif

#ifdef USE_AUSTRALIA_COOL_FUNCTION
extern INT8U reverse_percent;
extern ROMDATA INT8U Image_Form_logo[];
extern ROMDATA INT8U Image_Form_bar[];
void BitmapReverse( INT8U *pattern,short dwr,short w,short h,INT16U size)
{
	INT16U i;
	INT16U reverse_width;
	INT16U reverse_width_byte;	
	INT8U reverse_width_mask;		
	INT8U mask[9] = { 0x00, 0x80, 0xC0, 0xE0, 0xf0, 0xf8, 0xfC, 0xfe, 0xff}; 	

	reverse_width = (w*reverse_percent)/100;
	reverse_width_byte = reverse_width>>3;

	for(i=0; i< size;i++)
	{
		if((i%dwr)<reverse_width_byte)pattern[i] = ~pattern[i];
		if((i%dwr)==reverse_width_byte)
		{
			reverse_width_mask = reverse_width - (reverse_width_byte*8);
			pattern[i] = (~pattern[i] & mask[reverse_width_mask]) | (pattern[i] & ~mask[reverse_width_mask]);
		}
	}
}
#endif

void BitmapOut(INT16U id) //BITMAP graphic file data monochrome, non-compressed
{
	INT32U src_addr,addr,bitmap_point;
	INT16U bmp_address,w,h,dw;
	INT16S    i;
	INT8U bmp_Buf[BITMAP_MAX_SIZE];
#ifdef USE_AUSTRALIA_COOL_FUNCTION	
	INT8U  invertbar[250];

	if(id > 0x20)
	{
		if(id == 0x21)	// Logo
		{
			w = Image_Form_logo[0];
			h = Image_Form_logo[1];
			dw = w>>3;
			if (w%8) dw++;	
			PrtPattern((HUGEDATA INT8U *)&Image_Form_logo[2],dw,(short)w,(short)h,PrtStruct.Rot);
		}
		if(id == 0x22)	// Contain bar
		{
			w = Image_Form_bar[0];
			h = Image_Form_bar[1];			
			dw = w>>3;
			if (w%8) dw++;	
			memset(invertbar, 0, sizeof(invertbar));
			memcpy(invertbar, &Image_Form_bar[2], sizeof(invertbar));
			BitmapReverse(( INT8U *)invertbar,dw,(short)w,(short)h,sizeof(invertbar));
			PrtPattern((HUGEDATA INT8U *)invertbar,dw,(short)w,(short)h,PrtStruct.Rot);
			reverse_percent = 0;
		}
	}
	else
	{
		src_addr  = DFLASH_BASE;
		src_addr += FLASH_BITMAP_AREA;
		addr    = src_addr;
		bmp_address = 99;
		for (i=0; i<14; i++) {
			w=Flash_wread(addr);
			if (w==id) {
				bmp_address=i;
				addr +=2;
				w = Flash_wread(addr);
				addr +=2;
				h = Flash_wread(addr);
				break;
			}
			addr += BITMAP_INFO_SIZE;
		}
		if (bmp_address==99) return;
		dw = w>>3;
		if (w%8) dw++;
		if(PrtStruct.Mode == RECEIPT) {
			PrtStruct.y = 0;
			switch(StrStruct.Justification) {
				default:
				case CENTER:
					PrtStruct.x = (PrtStruct.PrintWidth - w * StrStruct.Magx)>>1;
					break;
				case LEFT:
					PrtStruct.x = 0;
					break;
				case RIGHT:
					PrtStruct.x = PrtStruct.PrintWidth - w * StrStruct.Magx - 1;
					break;
			}
			PrtStruct.ReceiptBmpHeight = h * StrStruct.Magy;
		}
		bitmap_point = bmp_address;
		bitmap_point*= BITMAP_MAX_SIZE;
		bitmap_point+= BITMAP_IMAGE;
		bitmap_point+= src_addr;
		Flash_sread(bitmap_point,bmp_Buf,BITMAP_MAX_SIZE);
		PrtPattern((HUGEDATA INT8U *)bmp_Buf,dw,(short)w,(short)h,PrtStruct.Rot);	//test bmp file is 4 byte * i.
	}
#elif defined(USE_USA_LABELFORMAT)
	INT32U image_size;

	if (status_scale.cur_labelid > 0 && status_scale.cur_labelid <= 45 )
	{
		addr = image_form_addr(id);

		if(addr)
		{
			memcpy(&w, (HUGEDATA char *)addr, 2);
			addr += 2;
			memcpy(&h, (HUGEDATA char *)addr, 2);
			addr += 2;
			
			dw = w>>3;
			if (w%8) dw++;	

			image_size = h * dw;
			memcpy(bmp_Buf, (HUGEDATA char *)addr, image_size);
			PrtPattern((HUGEDATA INT8U *)bmp_Buf,dw,(short)w,(short)h,PrtStruct.Rot);
		}
	}
	else
	{
			
		src_addr  = DFLASH_BASE;
		src_addr += FLASH_BITMAP_AREA;
		addr	= src_addr;
		bmp_address = 99;
		for (i=0; i<14; i++) {
			w=Flash_wread(addr);
			if (w==id) {
				bmp_address=i;
				addr +=2;
				w = Flash_wread(addr);
				addr +=2;
				h = Flash_wread(addr);
				break;
			}
			addr += BITMAP_INFO_SIZE;
		}
		if (bmp_address==99) return;
		dw = w>>3;
		if (w%8) dw++;
		if(PrtStruct.Mode == RECEIPT) {
			PrtStruct.y = 0;
			switch(StrStruct.Justification) {
				default:
				case CENTER:
					PrtStruct.x = (PrtStruct.PrintWidth - w * StrStruct.Magx)>>1;
					break;
				case LEFT:
					PrtStruct.x = 0;
					break;
				case RIGHT:
					PrtStruct.x = PrtStruct.PrintWidth - w * StrStruct.Magx - 1;
					break;
			}
			PrtStruct.ReceiptBmpHeight = h * StrStruct.Magy;
		}
		bitmap_point = bmp_address;
		bitmap_point*= BITMAP_MAX_SIZE;
		bitmap_point+= BITMAP_IMAGE;
		bitmap_point+= src_addr;
		Flash_sread(bitmap_point,bmp_Buf,BITMAP_MAX_SIZE);
		PrtPattern((HUGEDATA INT8U *)bmp_Buf,dw,(short)w,(short)h,PrtStruct.Rot);	//test bmp file is 4 byte * i.
	}
#else	
	src_addr  = DFLASH_BASE;
	src_addr += FLASH_BITMAP_AREA;
	addr    = src_addr;
	bmp_address = 99;
	for (i=0; i<14; i++) {
		w=Flash_wread(addr);
		if (w==id) {
			bmp_address=i;
			addr +=2;
			w = Flash_wread(addr);
			addr +=2;
			h = Flash_wread(addr);
			break;
		}
		addr += BITMAP_INFO_SIZE;
	}
	if (bmp_address==99) return;
	dw = w>>3;
	if (w%8) dw++;
	if(PrtStruct.Mode == RECEIPT) {
		PrtStruct.y = 0;
		switch(StrStruct.Justification) {
			default:
			case CENTER:
				PrtStruct.x = (PrtStruct.PrintWidth - w * StrStruct.Magx)>>1;
				break;
			case LEFT:
				PrtStruct.x = 0;
				break;
			case RIGHT:
				PrtStruct.x = PrtStruct.PrintWidth - w * StrStruct.Magx - 1;
				break;
		}
		PrtStruct.ReceiptBmpHeight = h * StrStruct.Magy;
	}
	bitmap_point = bmp_address;
	bitmap_point*= BITMAP_MAX_SIZE;
	bitmap_point+= BITMAP_IMAGE;
	bitmap_point+= src_addr;
	Flash_sread(bitmap_point,bmp_Buf,BITMAP_MAX_SIZE);
	PrtPattern((HUGEDATA INT8U *)bmp_Buf,dw,(short)w,(short)h,PrtStruct.Rot);	//test bmp file is 4 byte * i.
//	PrtPattern((HUGEDATA INT8U *)bitmap_point,dw,(short)w,(short)h,PrtStruct.Rot);	//test bmp file is 4 byte * i.
#endif
}

INT8U label_defaultload(void)
{
//	INT32U src_addr;
	INT16U id;

//	src_addr  = DFLASH_BASE;
//	src_addr += FLASH_PRINTER_AREA;
//	id=Flash_wread(src_addr+GLOBAL_LABEL_STANDARD);
	id=get_global_wparam(GLOBAL_LABEL_STANDARD);
	return label_load(id);
}

void label_set_defaultid(INT16U id)
{
//	INT32U src_addr;
//	src_addr  = DFLASH_BASE;
//	src_addr += FLASH_PRINTER_AREA;
//	Flash_wwrite(src_addr+GLOBAL_LABEL_STANDARD,id);
	set_global_wparam(GLOBAL_LABEL_STANDARD,id);
}

////////////////////////////////////
// INT8U label_load()
////////////////////////////////////
extern ROMDATA INT8U Label_Form01[];
extern ROMDATA INT8U Label_Form02[];
extern ROMDATA INT8U Label_Form03[];
extern ROMDATA INT8U Label_Form04[];
extern ROMDATA INT8U Label_Form05[];
extern ROMDATA INT8U Label_Form06[];
extern ROMDATA INT8U Label_Form07[];
extern ROMDATA INT8U Label_Form08[];
extern ROMDATA INT8U Label_Form09[];
extern ROMDATA INT8U Label_Form10[];
extern ROMDATA INT8U Label_Form11[];
extern ROMDATA INT8U Label_Form12[];
extern ROMDATA INT8U Label_Form13[];
extern ROMDATA INT8U Label_Form14[];
extern ROMDATA INT8U Label_Form15[];
extern ROMDATA INT8U Label_Form16[];
extern ROMDATA INT8U Label_Form17[];
extern ROMDATA INT8U Label_Form18[];
extern ROMDATA INT8U Label_Form19[];
extern ROMDATA INT8U Label_Form20[];
extern ROMDATA INT8U Label_Form21[];
extern ROMDATA INT8U Label_Form22[];
extern ROMDATA INT8U Label_Form23[];
extern ROMDATA INT8U Label_Form24[];
extern ROMDATA INT8U Label_Form25[];
extern ROMDATA INT8U Label_Form26[];
extern ROMDATA INT8U Label_Form27[];
extern ROMDATA INT8U Label_Form28[];
extern ROMDATA INT8U Label_Form29[];
extern ROMDATA INT8U Label_Form30[];
extern ROMDATA INT8U Label_Form31[];
extern ROMDATA INT8U Label_Form32[];
extern ROMDATA INT8U Label_Form33[];
extern ROMDATA INT8U Label_Form34[];
extern ROMDATA INT8U Label_Form35[];
extern ROMDATA INT8U Label_Form36[];
extern ROMDATA INT8U Label_Form37[];
extern ROMDATA INT8U Label_Form38[];
extern ROMDATA INT8U Label_Form39[];
extern ROMDATA INT8U Label_Form40[];
extern ROMDATA INT8U Label_Form41[];
extern ROMDATA INT8U Label_Form42[];
extern ROMDATA INT8U Label_Form43[];
extern ROMDATA INT8U Label_Form44[];
extern ROMDATA INT8U Label_Form45[];
#ifdef USE_AUSTRALIA_COOL_FUNCTION
extern ROMDATA INT8U Image_Form_logo[];
extern ROMDATA INT8U Image_Form_bar[];
#endif


#if !(ROM_COUNTRY == ROM_RUS)
INT32U label_rom_addr(INT16U id)
{
	INT32U addr;
	addr=0L;
	switch (id) {
#ifdef USE_ENG_LABELFORMAT
		case  1: addr = (INT32U)Label_Form01; break;
		case  2: addr = (INT32U)Label_Form02; break;
		case  3: addr = (INT32U)Label_Form03; break;
		case  4: addr = (INT32U)Label_Form04; break;
		case  5: addr = (INT32U)Label_Form05; break;
		case  6: addr = (INT32U)Label_Form06; break;
		case  7: addr = (INT32U)Label_Form07; break;
		case  8: addr = (INT32U)Label_Form08; break;
		case  9: addr = (INT32U)Label_Form09; break;
		case 10: addr = (INT32U)Label_Form10; break;
		case 11: addr = (INT32U)Label_Form11; break;
		case 12: addr = (INT32U)Label_Form12; break;
		case 13: addr = (INT32U)Label_Form13; break;
		case 14: addr = (INT32U)Label_Form14; break;
		case 15: addr = (INT32U)Label_Form15; break;
	     	case 16: addr = (INT32U)Label_Form16; break;
		case 17: addr = (INT32U)Label_Form17; break;
		case 18: addr = (INT32U)Label_Form18; break;
		case 19: addr = (INT32U)Label_Form19; break;
		case 20: addr = (INT32U)Label_Form20; break;
		case 21: addr = (INT32U)Label_Form21; break;
		case 22: addr = (INT32U)Label_Form22; break;
		case 23: addr = (INT32U)Label_Form23; break;
		case 24: addr = (INT32U)Label_Form24; break;
		case 25: addr = (INT32U)Label_Form25; break;
	     	case 26: addr = (INT32U)Label_Form26; break;
		case 27: addr = (INT32U)Label_Form27; break;
		case 28: addr = (INT32U)Label_Form28; break;
#endif
// 국가에 상관 없는 공통 라벨 포멧(전용 라벨 포멧 사용하는 이란, 호주, 미국,캐나다 제외 
#if !defined(USE_AU_LABELFORMAT) && !defined(USE_IRN_LABELFORMAT) && !defined(USE_USA_LABELFORMAT)
		case 29: addr = (INT32U)Label_Form29; break;
		case 30: addr = (INT32U)Label_Form30; break;
		case 31: addr = (INT32U)Label_Form31; break;
#endif
#ifdef USE_KOR_LABELFORMAT        
		case 32: addr = (INT32U)Label_Form32; break;
		case 33: addr = (INT32U)Label_Form33; break;
		case 34: addr = (INT32U)Label_Form34; break;
		case 35: addr = (INT32U)Label_Form35; break;
	     	case 36: addr = (INT32U)Label_Form36; break;
		case 37: addr = (INT32U)Label_Form37; break;
		case 38: addr = (INT32U)Label_Form38; break;
		case 39: addr = (INT32U)Label_Form39; break;
		case 40: addr = (INT32U)Label_Form40; break;
		case 41: addr = (INT32U)Label_Form41; break;
		case 42: addr = (INT32U)Label_Form42; break;
		case 43: addr = (INT32U)Label_Form43; break;
		case 44: addr = (INT32U)Label_Form44; break;
		case 45: addr = (INT32U)Label_Form45; break;
#endif        
#ifdef USE_CHN_LABELFORMAT
		case  1: addr = (INT32U)Label_Form01; break;
		case  2: addr = (INT32U)Label_Form02; break;
#endif      
#if defined (USE_AU_LABELFORMAT) || defined(USE_IRN_LABELFORMAT) || defined(USE_USA_LABELFORMAT)//전용 라벨 사용 하는 경우
		case  1: addr = (INT32U)Label_Form01; break;
		case  2: addr = (INT32U)Label_Form02; break;
		case  3: addr = (INT32U)Label_Form03; break;
		case  4: addr = (INT32U)Label_Form04; break;
		case  5: addr = (INT32U)Label_Form05; break;
		case  6: addr = (INT32U)Label_Form06; break;
		case  7: addr = (INT32U)Label_Form07; break;
		case  8: addr = (INT32U)Label_Form08; break;
		case  9: addr = (INT32U)Label_Form09; break;
		case 10: addr = (INT32U)Label_Form10; break;
		case 11: addr = (INT32U)Label_Form11; break;
		case 12: addr = (INT32U)Label_Form12; break;
		case 13: addr = (INT32U)Label_Form13; break;
		case 14: addr = (INT32U)Label_Form14; break;
		case 15: addr = (INT32U)Label_Form15; break;
		case 16: addr = (INT32U)Label_Form16; break;
		case 17: addr = (INT32U)Label_Form17; break;
		case 18: addr = (INT32U)Label_Form18; break;
		case 19: addr = (INT32U)Label_Form19; break;
		case 20: addr = (INT32U)Label_Form20; break;
		case 21: addr = (INT32U)Label_Form21; break;
		case 22: addr = (INT32U)Label_Form22; break;
		case 23: addr = (INT32U)Label_Form23; break;
		case 24: addr = (INT32U)Label_Form24; break;
		case 25: addr = (INT32U)Label_Form25; break;
		case 26: addr = (INT32U)Label_Form26; break;
		case 27: addr = (INT32U)Label_Form27; break;
		case 28: addr = (INT32U)Label_Form28; break;
		case 29: addr = (INT32U)Label_Form29; break;
		case 30: addr = (INT32U)Label_Form30; break;
		case 31: addr = (INT32U)Label_Form31; break;
		case 32: addr = (INT32U)Label_Form32; break;
		case 33: addr = (INT32U)Label_Form33; break;
		case 34: addr = (INT32U)Label_Form34; break;
		case 35: addr = (INT32U)Label_Form35; break;
		case 36: addr = (INT32U)Label_Form36; break;
		case 37: addr = (INT32U)Label_Form37; break;
		case 38: addr = (INT32U)Label_Form38; break;
		case 39: addr = (INT32U)Label_Form39; break;
		case 40: addr = (INT32U)Label_Form40; break;
		case 41: addr = (INT32U)Label_Form41; break;
		case 42: addr = (INT32U)Label_Form42; break;
		case 43: addr = (INT32U)Label_Form43; break;
		case 44: addr = (INT32U)Label_Form44; break;
		case 45: addr = (INT32U)Label_Form45; break;
#endif
		default: break;
	}
	return addr;
}
#else
INT32U label_rom_addr(INT16U id)
{
	INT32U addr;
	addr=0L;
	switch (id) {
		case  1: addr = (INT32U)Label_Form01; break;
		case  2: addr = (INT32U)Label_Form02; break;
		case  3: addr = (INT32U)Label_Form03; break;
		case  4: addr = (INT32U)Label_Form04; break;
		//case  5: //addr = (INT32U)Label_Form05; break;
		//case  6: //addr = (INT32U)Label_Form06; break;
		//case  7: //addr = (INT32U)Label_Form07; break;
		//case  8: //addr = (INT32U)Label_Form08; break;
		//case  9: //addr = (INT32U)Label_Form09; break;
		//case 10: //addr = (INT32U)Label_Form10; break;
		//	break;
		case 11: addr = (INT32U)Label_Form11; break;
		case 12: addr = (INT32U)Label_Form12; break;
		case 13: addr = (INT32U)Label_Form13; break;
		case 14: addr = (INT32U)Label_Form14; break;
		case 15: addr = (INT32U)Label_Form15; break;
	     	case 16: addr = (INT32U)Label_Form16; break;
		case 17: addr = (INT32U)Label_Form17; break;
		case 18: addr = (INT32U)Label_Form18; break;
		case 19: addr = (INT32U)Label_Form19; break;
		case 20: addr = (INT32U)Label_Form20; break;
		case 21: addr = (INT32U)Label_Form21; break;
		case 22: addr = (INT32U)Label_Form22; break;
		//case 23: //addr = (INT32U)Label_Form23; break;
		//case 24: //addr = (INT32U)Label_Form24; break;
		//	break;
		case 25: addr = (INT32U)Label_Form25; break;
	     	case 26: addr = (INT32U)Label_Form26; break;
		//case 27: //addr = (INT32U)Label_Form27; break;
		//case 28: //addr = (INT32U)Label_Form28; break;
		//	break;
		case 29: addr = (INT32U)Label_Form29; break;
		//case 30: //addr = (INT32U)Label_Form30; break;
		//	break;
		case 31: addr = (INT32U)Label_Form31; break;
		case 32: addr = (INT32U)Label_Form32; break;
		case 33: addr = (INT32U)Label_Form33; break;
		case 34: addr = (INT32U)Label_Form34; break;
		case 35: addr = (INT32U)Label_Form35; break;
	     	case 36: addr = (INT32U)Label_Form36; break;
		case 37: addr = (INT32U)Label_Form37; break;
		case 38: addr = (INT32U)Label_Form38; break;
		case 39: addr = (INT32U)Label_Form39; break;
		case 40: addr = (INT32U)Label_Form40; break;
		case 41: addr = (INT32U)Label_Form41; break;
		case 42: addr = (INT32U)Label_Form42; break;
		//case 43: addr = (INT32U)Label_Form43; break;
		//case 44: addr = (INT32U)Label_Form44; break;
		//case 45: addr = (INT32U)Label_Form45; break;
		default: break;
	}
	return addr;
}
#endif

#ifdef USE_USA_LABELFORMAT
static INT32U image_form_addr(INT16U image_id)//
{
	INT32U addr;
	addr=0L;
	switch (image_id) {
		case  1: addr = (INT32U)ImageFormSafeHanding; break;
		case  3: addr = (INT32U)ImageFormTotalPrice; break;
		case  5: addr = (INT32U)ImageFormSavedPrice; break;
		default: break;
	}
	return addr;
}
#endif

INT8U label_load(INT16U id)
{
	INT32U src_addr,addr,label_point;
	INT16U label_address,w,label_size, label_idx;
	HUGEDATA char *baddr;
	INT16S    i,cnt;
	INT16U height, width;
	char *lable_addr;
	char label_buf[LABEL_MAX_SIZE];
	struct LABEL_HEADER_INFO info;

	if (PrtStruct.Mode == RECEIPT) return FALSE;
//	char str[32];
	if (id==0) id=1;
	if (id<=MAX_LABEL_ROM_NO) {
		addr=label_rom_addr(id);
		if (addr == 0) return FALSE;
		addr+= PFLASH_DATA_BASE;
		addr += 2;	// width(2)
		memcpy(&height, (HUGEDATA char *)addr, 2);
		addr += 2;	// height(2)
		addr += 16;	// name(16)
		baddr= (HUGEDATA char *)addr;
		memcpy(&label_size,baddr,2);
		addr+=2;
		lable_addr=(char*)addr;
		goto FORM_COMMON;
	}
	if ((id==0) || (id==0xffff)) return FALSE;
// Download Label Format
	label_idx = get_new_labeladdress(id, 1);
	if(label_idx == 999)
	{
		return FALSE;
	}
	
	addr = get_addr_from_label_idx(label_idx, LABEL_AREA_HEADER);

	memset((void*)&info, 0, sizeof(info));
	read_label_header(addr, &info, L_H_I_H_BIT | L_H_I_S_BIT);
	label_size = info.img_size;
	height = info.img_h;

	if (label_size>LABEL_MAX_SIZE) return FALSE;

	addr = get_addr_from_label_idx(label_idx, LABEL_AREA_IMAGE);

	Flash_sread(addr, (INT8U*)label_buf, label_size);

	lable_addr = label_buf;

FORM_COMMON:
	width  = PrtStruct.LabelWidth;
//COMMON:
//	if (width>56) width=56;
	height *= 8;
	if(height != PrtStruct.LabelHeight) {
		//PrtStruct.LabelHeight = height;	// Added by CJK 20040831
		PrtSetLabelSize(width, height);
	}

	PutDataRxBuf(&CommBufPrt,(HUGEDATA char *)lable_addr,label_size);
	status_scale.cur_labelid=id;
	cnt=0;
	while(CheckRxBuf(&CommBufPrt)) {
		CASInterpreter(&CommBufPrt);
		cnt++;
		if (cnt>LABEL_MAX_SIZE) return FALSE;
	}

	return TRUE;
}

INT8U label_clear(void)
{
	Flash_clear(DFLASH_BASE + FLASH_LABEL_AREA, LABEL_CLEAR_DATA, LABEL_CUSTOM_2MB_NUM * LABEL_HEADER_SIZE);
#ifdef USE_LABEL_FORMAT_EXT
	Flash_clear(FLASH_LABEL_FORMAT_EXT_HEADER_AREA, LABEL_CLEAR_DATA, LABEL_CUSTOM_EXT_NUM * LABEL_HEADER_SIZE);
#endif

	return 1;
}

INT8U bitmap_clear(void)
{
	INT32U src_addr;
	INT32U sz;

	src_addr  = DFLASH_BASE;
	src_addr += FLASH_BITMAP_AREA;
	sz      = 14 * BITMAP_INFO_SIZE;
	Flash_clear(src_addr,0,sz);
	return 1;
}

/*INT8U bitmap_clear(INT16U id)
{
	INT32U src_addr,addr;
	INT16U sz;

	src_addr  = DFLASH_BASE;
	src_addr += FLASH_BITMAP_AREA;
	sz      = 14 * BITMAP_INFO_SIZE;
	if (id==0) {
		Flash_clear(src_addr,0,sz);
	} else if ((id>=1) && (id<=14)) {
		addr = (id-1) * BITMAP_INFO_SIZE;
		addr+=src_addr;
		Flash_clear(addr,0,BITMAP_INFO_SIZE);
	} else
		return 0x81;
	return 1;
}
*/
INT8U FontScanFont(INT8U id, HUGEDATA FONT_FILE_STRUCT   *FontFileStruct) {

	if(FontFileStruct->TypeOfFile[0] != 'F' || FontFileStruct->TypeOfFile[1] != 'T') return 0xff;

	if(FontFileStruct->ChSetType == ONE_BYTE_FONT_STR) {
		Font1Struct[id].FHbits1 = FontFileStruct->FHbits;
		Font1Struct[id].FWBytes1 = FontFileStruct->FWBytes;
		Font1Struct[id].Wbits1 = FontFileStruct->Wbits;
//Insert Fixed Font 20031218
//		if(Font1Struct[id].Wbits1 == 0) {
//        	Font1Struct[id].Wbits1 = Font1Struct[id].FWBytes1 * 8;
//		}
		Font1Struct[id].FixedWbits1 = FontFileStruct->FixedWbits;
		if(Font1Struct[id].Wbits1 == 0) {
        	Font1Struct[id].Wbits1 = FontFileStruct->FixedWbits;
			Font1Struct[id].FixedWidthFlag1 = 0;
		} else {
			Font1Struct[id].FixedWidthFlag1 = 1;
		}
		Font1Struct[id].Hbits1 = FontFileStruct->Hbits;
		Font1Struct[id].Baseline1 = FontFileStruct->Baseline;
		Font1Struct[id].FirstCode1 = (INT8U)(FontFileStruct->FirstCode);
		Font1Struct[id].PtrFont1 = (HUGEDATA INT8U *)FontFileStruct + FontFileStruct->BitmapOffset;
		Font1Struct[id].PtrWidth1 = (HUGEDATA INT8U *)FontFileStruct + FontFileStruct->WidthTableOffset;
		Font1Struct[id].PreRot1 = FontFileStruct->PreRot; 
		Font1Struct[id].TotalCharNum1 = FontFileStruct->TotalCharNum;
		//HYP 20040513
		if (id==0) {
			Font1Struct[id].ExistFont=1;
		} else {
			if(Font1Struct[id].PreRot1 == PREROT_ONLY_90DEG) {
				Font1Struct[id].ExistFont = 0;
			} else {//PREROT_0DEG_90DEG || PREROT_ONLY_0DEG
				Font1Struct[id].ExistFont = 1;
			}
		}
	} else {
		Font2Struct[id].FHbits2 = FontFileStruct->FHbits;
		Font2Struct[id].FWBytes2 = FontFileStruct->FWBytes;
		Font2Struct[id].Wbits2 = FontFileStruct->Wbits;
		if(Font2Struct[id].Wbits2 == 0) {
	        Font2Struct[id].Wbits2 = Font2Struct[id].FWBytes2 * 8;
	    }
	   	Font2Struct[id].Hbits2 = FontFileStruct->Hbits;
		Font2Struct[id].Baseline2 = FontFileStruct->Baseline; 
		Font2Struct[id].FirstCode2 = FontFileStruct->FirstCode;  
		Font2Struct[id].PtrFont2 = (HUGEDATA INT8U *)FontFileStruct + FontFileStruct->BitmapOffset;
		Font2Struct[id].Font2Type = FontFileStruct->ChSetType; 
		Font2Struct[id].PreRot2 = FontFileStruct->PreRot;
		Font2Struct[id].TotalCharNum2 = FontFileStruct->TotalCharNum;
		//HYP 20040513
		if(Font2Struct[id].PreRot2 == PREROT_ONLY_90DEG) {
			Font2Struct[id].ExistFont = 0;
		} else {//PREROT_0DEG_90DEG || PREROT_ONLY_0DEG
			Font2Struct[id].ExistFont = 1;
		}
	}
	return 0;
}

void FontLoadFont1(INT8U id) {
	if(id < MAX_FONT1_NUM) {
		if(Font1Struct[id].ExistFont == 0) {
			id=PRT_DEFAULT_FONT_ID;
		}
		StrStruct.Id1 = id;
		FontStruct.Wbits1 = Font1Struct[id].Wbits1;
		FontStruct.Hbits1 = Font1Struct[id].Hbits1;
		FontStruct.FHbits1 = Font1Struct[id].FHbits1;
		FontStruct.FWBytes1 = Font1Struct[id].FWBytes1;
		FontStruct.Baseline1 = Font1Struct[id].Baseline1;
		FontStruct.FirstCode1 = Font1Struct[id].FirstCode1;
		FontStruct.PtrFont1 = Font1Struct[id].PtrFont1;
		FontStruct.PtrWidth1 = Font1Struct[id].PtrWidth1;
		FontStruct.PreRot1 = Font1Struct[id].PreRot1;
		FontStruct.TotalCharNum1 = Font1Struct[id].TotalCharNum1;
//Insert Fixed Font 20031218
		FontStruct.FixedWbits1 = Font1Struct[id].FixedWbits1;
		FontStruct.FixedWidthFlag1 = Font1Struct[id].FixedWidthFlag1;
		//change feed variable
		PrtDrvStruct.LineFeedLength    = (INT16U)FontStruct.FHbits1*StrStruct.Magy;
//		PrtDrvStruct.ReverseFeedLength = (INT16U)FontStruct.FHbits1*StrStruct.Magy;//Erase HYP 20060308
		PrtDrvStruct.LineFeedVariable = PrtDrvStruct.LineFeedLength;
//		PrtDrvStruct.ReverseFeedVariable = PrtDrvStruct.ReverseFeedLength; ////Erase HYP 20060308
		if(PrtStruct.Mode == RECEIPT) {
			PrtSetHTabPos(PrtFlashStruct.HTabCh);
//			PrtSetVTabPos(PrtFlashStruct.VTabCh);
		}
	}
}

void FontLoadFont2(INT8U id) {
	if(id < MAX_FONT2_NUM) {
		if(Font2Struct[id].ExistFont == 0) {
			id=PRT_DEFAULT_FONT_ID;
		}
		StrStruct.Id2 = id;
		FontStruct.Wbits2 = Font2Struct[id].Wbits2;
		FontStruct.Hbits2 = Font2Struct[id].Hbits2;
		FontStruct.FHbits2 = Font2Struct[id].FHbits2;
		FontStruct.FWBytes2 = Font2Struct[id].FWBytes2; 
		FontStruct.Baseline2 = Font2Struct[id].Baseline2;
		FontStruct.FirstCode2 = Font2Struct[id].FirstCode2;
		FontStruct.PtrFont2 = Font2Struct[id].PtrFont2;
		FontStruct.Font2Type = Font2Struct[id].Font2Type;
		FontStruct.PreRot2 = Font2Struct[id].PreRot2;	
		FontStruct.TotalCharNum2 = Font2Struct[id].TotalCharNum2;		
	}
}

void FontInit(void) //StrStruct.Id1 and StrStruct.Id2 must be predefined.
{
	INT32U addr,pnt,fontaddr;
	INT8U fonttype,fontid,allow_2byte;
	INT16S   i,fontnum;
//	char str[32];
	INT8U id;
#ifdef CPU_PIC32MZ
	Font_getdefaultid();

	FontScanFont(0,(HUGEDATA FONT_FILE_STRUCT   *)DspFontAddrHpf);
//#ifdef _USE_LCD20848_//ck
//	FontScanFont(8,(HUGEDATA FONT_FILE_STRUCT   *)DspFontAddr12x8Hpf);
//	FontScanFont(9,(HUGEDATA FONT_FILE_STRUCT   *)DspFontAddr20x8Hpf);
//#endif
#if defined(USE_KOR_FONT) || defined(USE_CHN_FONT)
	FontScanFont(3,(HUGEDATA FONT_FILE_STRUCT   *)Dsp2byteFontAddrHpf);
	allow_2byte = 1;
//	FontScanFont(0,(HUGEDATA FONT_FILE_STRUCT   *)Dsp2byteFontAddrHpf);
#endif
	FontScanFont(3,(HUGEDATA FONT_FILE_STRUCT   *)FontAddr_3_8x16);
#if defined(USE_KOR_FONT) || defined(USE_CHN_FONT)
	FontScanFont(3,(HUGEDATA FONT_FILE_STRUCT   *)FontAddr_3_16x16);
#endif
	FontScanFont(4,(HUGEDATA FONT_FILE_STRUCT   *)FontAddr_4_12x24);
	FontScanFont(5,(HUGEDATA FONT_FILE_STRUCT   *)FontAddr_5_6x12);
#ifdef USE_PERSIA_DEFAULT
    FontScanFont(6,(HUGEDATA FONT_FILE_STRUCT	*)FontAddr_6_12x16);
#endif
#if defined(USE_KOR_FONT)
	FontScanFont(6,(HUGEDATA FONT_FILE_STRUCT	*)FontAddr_6_8x24);
#endif

	FontLoadFont1(PRT_DEFAULT_FONT_ID);
	if (allow_2byte) FontLoadFont2(PRT_DEFAULT_FONT_ID);
#else
//	DspScanFont(0,(HUGEDATA FONT_FILE_STRUCT   *)DspFontAddrHpf);
	addr  =DFLASH_BASE;
	addr += FLASH_FONT_AREA;
	pnt = addr + FONT_NUMBER;
	fontnum  = (INT16S)Flash_bread(pnt);
	for(id=0; id < MAX_FONT1_NUM; id++) {
		Font1Struct[id].ExistFont = 0;
	}
	for(id=0; id < MAX_FONT2_NUM; id++) {
		Font2Struct[id].ExistFont = 0;
	}
	FontScanFont(0,(HUGEDATA FONT_FILE_STRUCT   *)DspFontAddrHpf);
	Font_getdefaultid();
	allow_2byte=0;
	for (i=0; i<fontnum; i++) {
		pnt = addr + FONT_INFO_POINT+FONT_INFO_SIZE*i;
		fontid  =Flash_bread(pnt);
		fonttype=Flash_bread(pnt+1);
		if (fonttype&0x04) {
			fontaddr = Flash_lread(pnt+2);
			fontaddr+= addr;
			fontaddr+= FONT_IMAGE;
			FontScanFont(fontid,(HUGEDATA FONT_FILE_STRUCT   *)fontaddr);
		}
		if (fonttype&0x01) {
			allow_2byte=1;
		}
	}
	FontLoadFont1(PRT_DEFAULT_FONT_ID);
	if (allow_2byte) FontLoadFont2(PRT_DEFAULT_FONT_ID);
#endif // CPU_PIC32MZ
}

void Font_getdefaultid(void)
{
//	INT32U src_addr;
//
//	src_addr  = DFLASH_BASE;
//	src_addr += FLASH_PRINTER_AREA;
//	PRT_DEFAULT_FONT_ID = Flash_bread(src_addr+PRT_FONT_LABELID);
	PRT_DEFAULT_FONT_ID=3;  //test yoo 100111
    if (PRT_DEFAULT_FONT_ID>0x7f) PRT_DEFAULT_FONT_ID=0;
}

void StrFieldInit(void) {
	INT8U i;
	for(i=0; i < (MAX_STR_FIELD_NUM+MAX_BAR_FIELD_NUM); i++) {
		StrFieldStruct[i].ExistField = 0;
	}
	PrtStruct.CurrentStrFieldNum = 0;//HYP 20060427
}

void StrInit(void) {
	StrStruct.StrHeight = PrtFlashStruct.StrHeight;
	StrStruct.StrWidth = PrtFlashStruct.StrWidth;
	StrStruct.InterChSp = PrtFlashStruct.InterChSp;
	StrStruct.LineSp = PrtFlashStruct.LineSp;
	StrStruct.ChRot = PrtFlashStruct.ChRot;	  //'-' sign ?
	StrStruct.Justification = PrtFlashStruct.Justification;
	StrStruct.Typeface = PrtFlashStruct.Typeface;
	StrStruct.Magy = PrtFlashStruct.Magy;
	StrStruct.Magx = PrtFlashStruct.Magx;
	StrStruct.StrType = PrtFlashStruct.StrType;	
	StrStruct.AutoShrinkx = PrtFlashStruct.AutoShrinkx; 
	StrStruct.WordWrap = PrtFlashStruct.WordWrap;
	StrStruct.VCentering = PrtFlashStruct.VCentering; //Vertical Centering
	StrStruct.StrMirx = PrtFlashStruct.StrMirx;
	StrStruct.EnableExpandByte = PrtFlashStruct.EnableExpandByte; //use for typeface
	StrStruct.Id1 = PrtFlashStruct.Id1;
	StrStruct.Id2 = PrtFlashStruct.Id2;
//Insert Fixed Font 20031218
	StrStruct.UseFixedWidth = 0;
	StrFieldInit();
}

void StrInit4UserLabel(void)//HYP 20060426 for scale user label   ex) print sensor value 
{
	PrtStruct.x = 0;
	PrtStruct.y = 0;
	PrtStruct.Rot = DEG_0;
	StrStruct.StrHeight = PrtFlashStruct.StrHeight;
	StrStruct.StrWidth = PrtFlashStruct.StrWidth;
	StrStruct.InterChSp = PrtFlashStruct.InterChSp;
	StrStruct.LineSp = PrtFlashStruct.LineSp;
	StrStruct.ChRot = PrtFlashStruct.ChRot;	  //'-' sign ?
	StrStruct.Justification = PrtFlashStruct.Justification;
	StrStruct.Typeface = PrtFlashStruct.Typeface;
	StrStruct.Magy = PrtFlashStruct.Magy;
	StrStruct.Magx = PrtFlashStruct.Magx;
	StrStruct.StrType = PrtFlashStruct.StrType;	
	StrStruct.AutoShrinkx = PrtFlashStruct.AutoShrinkx; 
	StrStruct.WordWrap = PrtFlashStruct.WordWrap;
	StrStruct.VCentering = PrtFlashStruct.VCentering; //Vertical Centering
	StrStruct.StrMirx = PrtFlashStruct.StrMirx;
	StrStruct.EnableExpandByte = PrtFlashStruct.EnableExpandByte; //use for typeface
	StrStruct.Id1 = PrtFlashStruct.Id1;
	StrStruct.Id2 = PrtFlashStruct.Id2;
	StrStruct.UseFixedWidth = 0;
}
#ifdef USE_CONTINUOUS_LABEL_WITH_INGREDIENT
extern short ingredient_height_offset;
extern short ingredient_y_pos;
extern INT8U ingredient_height_adjust_flag;
#endif
void FontSaveField(INT8U field_num) {
	
	StrFieldStruct[field_num].sx = PrtStruct.x;
	StrFieldStruct[field_num].sy = PrtStruct.y;	
#ifdef USE_CONTINUOUS_LABEL_WITH_INGREDIENT	
//	if(ingredient_height_adjust_flag)
//	{
//		if(PrtStruct.y>ingredient_y_pos)	StrFieldStruct[field_num].sy += ingredient_height_offset;
//	}
#endif
	StrFieldStruct[field_num].StrRot = PrtStruct.Rot;

#ifdef USE_CONTINUOUS_LABEL_WITH_INGREDIENT	
	if((field_num == FIELD_ID_INGREDIENT_STR) && ingredient_height_adjust_flag)
	{
		StrFieldStruct[field_num].StrHeight += ingredient_height_offset;
	}
	else 
	{
		StrFieldStruct[field_num].StrHeight = StrStruct.StrHeight;
	}
#else
	StrFieldStruct[field_num].StrHeight = StrStruct.StrHeight;
#endif
	StrFieldStruct[field_num].StrWidth = StrStruct.StrWidth;
	StrFieldStruct[field_num].InterChSp = StrStruct.InterChSp;
	StrFieldStruct[field_num].LineSp = StrStruct.LineSp;
	StrFieldStruct[field_num].ChRot = StrStruct.ChRot;	  //'-' sign ?
	StrFieldStruct[field_num].Justification = StrStruct.Justification;
	StrFieldStruct[field_num].Typeface = StrStruct.Typeface;
	StrFieldStruct[field_num].Magy = StrStruct.Magy;
	StrFieldStruct[field_num].Magx = StrStruct.Magx;
	StrFieldStruct[field_num].StrType = StrStruct.StrType;	
	StrFieldStruct[field_num].AutoShrinkx = StrStruct.AutoShrinkx; 
	StrFieldStruct[field_num].WordWrap = StrStruct.WordWrap;
	StrFieldStruct[field_num].VCentering = StrStruct.VCentering; //Vertical Centering

	StrFieldStruct[field_num].StrMirx = StrStruct.StrMirx;
	StrFieldStruct[field_num].EnableExpandByte = StrStruct.EnableExpandByte; //use for typeface

	StrFieldStruct[field_num].Id1 = StrStruct.Id1;
	StrFieldStruct[field_num].Id2 = StrStruct.Id2;
//Insert Fixed Font 20031218
	StrFieldStruct[field_num].UseFixedWidth = StrStruct.UseFixedWidth;
	StrFieldStruct[field_num].ExistField = 1;
}

INT8U FontLoadField(INT8U field_num) {
	if(StrFieldStruct[field_num].ExistField == 0) return 0xff;
	PrtStruct.CurrentStrFieldNum = field_num;//HYP 20040903

	PrtStruct.x = StrFieldStruct[field_num].sx;
	PrtStruct.y = StrFieldStruct[field_num].sy;
	PrtStruct.Rot = StrFieldStruct[field_num].StrRot;

	StrStruct.StrHeight = StrFieldStruct[field_num].StrHeight;
	StrStruct.StrWidth = StrFieldStruct[field_num].StrWidth;
	StrStruct.InterChSp = StrFieldStruct[field_num].InterChSp;
	StrStruct.LineSp = StrFieldStruct[field_num].LineSp;
	StrStruct.ChRot = StrFieldStruct[field_num].ChRot;	  //'-' sign ?
	StrStruct.Justification = StrFieldStruct[field_num].Justification;
	StrStruct.Typeface = StrFieldStruct[field_num].Typeface;
	StrStruct.Magy = StrFieldStruct[field_num].Magy;
	StrStruct.Magx = StrFieldStruct[field_num].Magx;
	StrStruct.StrType = StrFieldStruct[field_num].StrType;	
	StrStruct.AutoShrinkx = StrFieldStruct[field_num].AutoShrinkx; 
	StrStruct.WordWrap = StrFieldStruct[field_num].WordWrap;
	StrStruct.VCentering = StrFieldStruct[field_num].VCentering; //Vertical Centering

	StrStruct.StrMirx = StrFieldStruct[field_num].StrMirx;
	StrStruct.EnableExpandByte = StrFieldStruct[field_num].EnableExpandByte; //use for typeface

	StrStruct.Id1 = StrFieldStruct[field_num].Id1;
	StrStruct.Id2 = StrFieldStruct[field_num].Id2;
//Insert Fixed Font 20031218
	StrStruct.UseFixedWidth = StrFieldStruct[field_num].UseFixedWidth;

	FontLoadFont1(StrStruct.Id1);
	FontLoadFont2(StrStruct.Id2);

	if((PrtDrvStruct.PrtLoadFormFlag == OFF) && (field_num < MAX_STR_FIELD_NUM)) {
#ifdef USE_DISCOUNT_MEMBERSHIP // 할인 기간 인쇄 오류 수정
#else
		if(field_num == FIELD_ID_TRANSACTION_NO) {
			if(PrtStruct.Rot&MASK_DEG_90_270) {
				PrtRectFill(PrtStruct.x,PrtStruct.y,PrtStruct.x+StrStruct.StrHeight,PrtStruct.y+StrStruct.StrWidth,WHITE);
			} else { //DEG_O, DEG_180
				PrtRectFill(PrtStruct.x,PrtStruct.y,PrtStruct.x+StrStruct.StrWidth,PrtStruct.y+StrStruct.StrHeight,WHITE);
			}
		}
#endif
	}

	return 0;
}

//HYP 20040903
INT8U FontEraseFieldArea(INT8U field_num) {
	if(StrFieldStruct[field_num].ExistField == 0) return 0xff;
	PrtStruct.x = StrFieldStruct[field_num].sx;
	PrtStruct.y = StrFieldStruct[field_num].sy;
	PrtStruct.Rot = StrFieldStruct[field_num].StrRot;
	StrStruct.StrHeight = StrFieldStruct[field_num].StrHeight;
	StrStruct.StrWidth = StrFieldStruct[field_num].StrWidth;
	if(field_num < MAX_STR_FIELD_NUM) {
		if(PrtStruct.Rot&MASK_DEG_90_270) {
			PrtRectFill(PrtStruct.x,PrtStruct.y,PrtStruct.x+StrStruct.StrHeight,PrtStruct.y+StrStruct.StrWidth,WHITE);
		} else { //DEG_O, DEG_180
			PrtRectFill(PrtStruct.x,PrtStruct.y,PrtStruct.x+StrStruct.StrWidth,PrtStruct.y+StrStruct.StrHeight,WHITE);
		}
	}
	return 0;
}

//#pragma endoptimize
#ifdef USE_TEXT_EMPHASIS	// ~1 : Italic, ~2 : Bold, ~3 : Underline 강조하는 기능 이후 부터 스페이스 나 엔터가 나오기 전에 강조표현 유지한다 		
	INT8U text_emphasis_state;
	INT8U text_emphasis_loop;
	INT8U text_emphasis_type;	
#endif
void Font_Typeface(HUGEDATA INT8U *pattern,short dwr,short w,short h,char rot);
#if defined(USE_ARAB_FONT) || defined(HEBREW_FONT)
void  FontStrOut(HUGEDATA char *ch_ptr) 
{
	short cx,cy,crx,cry,bits_sum,i;
	short by[30],str_left_margin[30],str_right_margin[30],whiteSpace,whitespace_V;
	CWDARB		chw;
	char rot,wbits1,hbits1,wbits2,hbits2,wbits,hbits,nwbits,nhbits,justification,sbits;
	char line_idx,line_end,loop,spbytes,rx,ry;
//	HUGEDATA char *p,*p_max;/////htab
	HUGEDATA INT16U *p,*p_max;
	char wordwrap, p_sum, /*baseline_height,*/ctrl_ch,max_line;//HYP 20050224 erase baseline_height
	short MbaselineH,MbaselineL,baselineL,baselineH,baseline1,baseline2,line_height;
	short sx,sy;
	HUGEDATA INT8U *fontw_ptr;
	char variable_init_flag, magx_backup;
	char ch_rot_backup;/////////rot
	INT8U	sphbits;
	short	ncx;////////htab
	INT8U	htab_pos_count;	////////htab
	short	max_cx;////////htab
	BOOLEAN updata_mag;
	INT8U id1_backup, id2_backup;
	//char string_buf[30];	
	//20040209 HYP
	short backup_x,backup_y;
	backup_x = PrtStruct.x;
	backup_y = PrtStruct.y;
	variable_init_flag = 1;
	loop = TRUE;	
	magx_backup = StrStruct.Magx;	 //for auto shrink
	ch_rot_backup = StrStruct.ChRot;  /////////rot
	id1_backup = StrStruct.Id1;
	id2_backup = StrStruct.Id2;

	if(*ch_ptr == '\0') return;
#ifdef USE_ARAB_FONT
 #ifdef USE_ENG_BASED_ARAB_SUPPORT
	Convert_ArabPreview((INT8U*)ch_ptr, strlen(ch_ptr), 0, 0, 1); //make 2 BYTE data of arab_codetemp[1024]
 #else
	Convert_ArabPreview((INT8U*)ch_ptr, strlen(ch_ptr), 0, 0, 0); //make 2 BYTE data of arab_codetemp[1024]
 #endif
#elif defined(HEBREW_FONT)
	Convert_HebrewPreview((INT8U*)ch_ptr, strlen(ch_ptr), 0, 0); //make 2 BYTE data of arab_codetemp[1024]
#endif

#ifdef USE_PRINT_LTR_FOR_ARAB
	// arab_codetemp에 아랍어문자 존재 여부 확인
	// 아랍어가 있을 경우 : arab_codetemp - 변경없음
	// 아랍어가 있을 경우 : arab_codetemp - 아랍 문자순서 적용 전 데이터로 변경
	INT8U IsArabFlag = FALSE;
	IsArabFlag = ChangeToOnlyEnglishBuffer((INT8U*)ch_ptr);
#endif

#ifdef USE_ARABIC_NUMBER
	if((PrtStruct.CurrentStrFieldNum == FIELD_ID_2ND_TOTALPRICE) ||	// Arabic Total Price
	   (PrtStruct.CurrentStrFieldNum == FIELD_ID_2ND_UNIT_PRICE) ||	// Arabic Unit Price
	   (PrtStruct.CurrentStrFieldNum == FIELD_ID_2ND_NET_WEIGHT) ||	// Arabic Gross Weight	 
	   (PrtStruct.CurrentStrFieldNum == FIELD_ID_NET_WEIGHT)     ||	// Arabic Weight  
	   (PrtStruct.CurrentStrFieldNum == FIELD_ID_NEW_QUANTITY))     // Arabic Quantity
	   {
			ConvertArabianNumeralToArabic(arab_codetemp);
	   }
#endif //USE_ARABIC_NUMBER
	//if (mode) Convert_ArabPreview((INT8U*)ch_ptr, strlen(ch_ptr)); //make 2 BYTE data of arab_codetemp[1024]
	//else {
	//	for (i=0; i<strlen(ch_ptr); i++) {
	//		arab_codetemp[i]=(INT16U)ch_ptr[i];
	//	}
	//}   
#ifndef USE_ENG_BASED_ARAB_SUPPORT
  #ifdef USE_PRINT_LTR_FOR_ARAB
	if(!IsArabFlag)
		StrStruct.StrMirx = 0;	// arab_codetemp에 아랍어가 없는 경우
	else
  #endif
		StrStruct.StrMirx = 1; // for arab test, mirror 
#endif

	while (loop) {
		if(variable_init_flag == 1) {
			variable_init_flag = 0;

			sx = PrtStruct.x;
			sy = PrtStruct.y;
			spbytes = StrStruct.InterChSp>>3;
			if(StrStruct.InterChSp&0x07) spbytes++;			
#ifdef USE_ARAB_FONT
 #ifdef USE_ENG_BASED_ARAB_SUPPORT
			p = eng_codetemp; //for arab version
 #else
			p = arab_codetemp; //for arab version
 #endif
#elif defined(HEBREW_FONT)
			p = hebrew_codetemp; //for arab version
#endif
			wordwrap = StrStruct.WordWrap;
			cx = 0;								// rel. character pos x
			cy = 0;								// rel. character pos y
			justification = StrStruct.Justification;
			if(StrStruct.StrMirx) { //mirror
				if(justification == RIGHT) {
					justification = LEFT;
				} else if(justification == LEFT) {
					justification = RIGHT;
				}
			}
//////////////////////////////////load font1,2 start
			rx = StrStruct.Magx;
			ry = StrStruct.Magy;	
			wbits2 = FontStruct.Wbits2*rx;	  	// mag. width bits of 2 bytes font
			hbits2 = FontStruct.Hbits2*ry;		// mag. height bits of 2 bytes font
			wbits1 = FontStruct.Wbits1*rx;		// mag. width bits of 1 bytes font
			hbits1 = FontStruct.Hbits1*ry;		// mag. height bits of 1 bytes font
			wbits = wbits1;		// mag. width inc. bits of 1 str line
			hbits = hbits1;		// mag. height inc. bits of 1 str line
			nwbits = wbits1;		// mag. width inc. bits of 1 str line
			nhbits = hbits1;		// mag. height inc. bits of 1 str line
			baseline2 = FontStruct.Baseline2*ry;
			baseline1 = FontStruct.Baseline1*ry;
//Insert Fixed Font 20031218
//			fontw_ptr = FontStruct.PtrWidth1 + ((INT8U)0x20-FontStruct.FirstCode1);
//			sbits = *(fontw_ptr) * rx;
			if(FontStruct.FixedWidthFlag1 == 1 || StrStruct.UseFixedWidth == 1) {
				sbits = FontStruct.FixedWbits1 * rx;
			} else {
				fontw_ptr = FontStruct.PtrWidth1 + ((INT16U)0x20-FontStruct.FirstCode1);
				sbits = *(fontw_ptr) * rx;
			}
////////////////////////////
//////////////////////////////////load font1,2 end
			str_left_margin[0] = 0;
			str_right_margin[0]=0;
			line_idx = 0;
			htab_pos_count = 0;	 ////////htab
			max_cx = 0;	 ////////htab
			ncx = 0;  ///////htab
			line_end = FALSE;
			by[0] = 0;
			baselineL = 0;
			baselineH = 0;

			p_sum = 0;
			bits_sum = 0;
			MbaselineH = 0;
			MbaselineL = 0;
			line_height = 0;
		}	/*  Init end  */

 //justification and baseline check
		ctrl_ch = OFF;
		chw.words.left  = *p;
		chw.words.right = *(p+1);//HYP 20040128
		if(chw.words.left == _CR) {
			if(*(p+1) == LF) {
				p++;
			}
			chw.words.left = LF;
		}
		if (StrStruct.StrType == TWO_BYTE_FONT_STR && chw.words.left > 0x80) {
			p = p + 2;
			wbits = wbits2;		
			hbits = hbits2;
			baselineL = baseline2;
			baselineH = hbits2 - baseline2;
			p_sum = p_sum + 2;
		} else {
        		p++;
			if (chw.words.left == FONT_CON_CH) {
				ctrl_ch = ON;
				updata_mag = OFF;
				switch (*p) {
					///////////rot
					case FONT_CON_DEG:
						p++;
						switch(*p) {
							case '0':
								StrStruct.ChRot = DEG_0;
								break;
							case '1':
								StrStruct.ChRot = DEG_90;
								break;
							case '2':
								StrStruct.ChRot = DEG_180;
								break;
							case '3':
								StrStruct.ChRot = DEG_270;
								break;
						}
						break;
					case FONT_CON_MAG_X:
						p++;
						switch(*p) {
							case '1':
								rx = 1;
								break;
							case '2':
								rx = 2;
								break;
							case '3':
								rx = 3;
								break;
							case '4':
								rx = 4;
								break;
							case '5':
								rx = 5;
								break;
						}
						updata_mag = ON;
						break;
					case FONT_CON_MAG_Y:
						p++;
						switch(*p) {
							case '1':
								ry = 1;
								break;
							case '2':
								ry = 2;
								break;
							case '3':
								ry = 3;
								break;
							case '4':
								ry = 4;
								break;
							case '5':
								ry = 5;
								break;
						}
						updata_mag = ON;
						break;
					case FONT_CON_INC_MAG:
						if(rx < 5) {
							rx++;
						}
						if(ry < 5) {
							ry++;
						}
						updata_mag = ON;
						break;
					case FONT_CON_DEC_MAG:
						if(rx > 1) {
							rx--;
						}
						if(ry > 1) {
							ry--;
						}
						updata_mag = ON;
						break;
					case FONT_CON_FONT2:
						p++;
						FontLoadFont2((INT16U)*p - '0');
						updata_mag = ON;
						break;
					case FONT_CON_FONT1:
						p++;
//sprintf(gmsg, "[%04x,%04x]", (INT16U)*p, (INT8U)*p);
//MsgOut(gmsg);
						FontLoadFont1((INT16U)*p - '0');
						updata_mag = ON;
						break;
				//HYP 20040621
				//FONT_CON_CH > 0x20
				//	case '0':
				//		ctrl_ch = OFF;
				//		break;
				}
				if(updata_mag == ON) {
					baseline2 = FontStruct.Baseline2*ry;
					baseline1 = FontStruct.Baseline1*ry;
					wbits2 = FontStruct.Wbits2*rx;	  	// mag. width bits of 2 bytes font
					hbits2 = FontStruct.Hbits2*ry;		// mag. height bits of 2 bytes font
					wbits1 = FontStruct.Wbits1*rx;		// mag. width bits of 1 bytes font
					hbits1 = FontStruct.Hbits1*ry;		// mag. height bits of 1 bytes font
					fontw_ptr = FontStruct.PtrWidth1 + ((INT16U)0x20-FontStruct.FirstCode1);
				}
				p++;
			}
			if(chw.words.left < (INT16U)FontStruct.FirstCode1) {
				//HYP 20050224
				//wbits1 = 0;
				if (StrStruct.ChRot&MASK_DEG_90_270) {
					hbits1 = 0;
				} else {
					wbits1 = 0;
				}
				ctrl_ch = ON;  
			} else {
//Insert Fixed Font 20031218					  
//				fontw_ptr = FontStruct.PtrWidth1 + ((INT8U)chw.words.left-FontStruct.FirstCode1);
//				wbits1 = *(fontw_ptr) * rx;
				if(FontStruct.FixedWidthFlag1 == 1 || StrStruct.UseFixedWidth == 1) {
					wbits1 = FontStruct.FixedWbits1 * rx;
				} else {
					fontw_ptr = FontStruct.PtrWidth1 + ((INT16U)chw.words.left-FontStruct.FirstCode1);
					wbits1 = *(fontw_ptr) * rx;
				}
				//HYP 20050224
				hbits1= FontStruct.Hbits1*ry;
////////////////////////////
			}
			wbits = wbits1;		
			hbits = hbits1;
			//HYP 20050224
			//baselineH = hbits1 - baseline1;
			//baselineL = baseline1;
			if (StrStruct.ChRot&MASK_DEG_90_270) {
				baselineH = wbits1;
				baselineL = 0;
			} else {
				baselineH = hbits1 - baseline1;
				baselineL = baseline1;
			}
			if(chw.words.left != 0x20) {
				if ((ctrl_ch == ON) || (chw.words.left == FONT_CON_CH)) {
					p_sum = 0; //FONT_CON_CH < 0x20
				} else {
					p_sum = p_sum + 1;
				}
				if(PrtStruct.Mode == RECEIPT && chw.words.left == HT) {	////////htab
					if(max_cx < cx) max_cx = cx;
					cx = StrStruct.HTab[htab_pos_count]; ///htab 
					htab_pos_count++;
					if(max_cx < cx) max_cx = cx;
				}
			} else {
				if(FontStruct.FixedWidthFlag1 == 1 || StrStruct.UseFixedWidth == 1) {
					sbits = FontStruct.FixedWbits1 * rx;
				} else {
					sbits = *(fontw_ptr) * rx;/////////////////////////
				}
				p_sum = 0;
			}
		}
		if (StrStruct.StrType == TWO_BYTE_FONT_STR && (INT8U)(*p) > 0x80) {	//you must cast!!! -> 만약 버그있을 시 형변환 문제 테스트해볼 것
			nwbits = wbits2;		
			nhbits = hbits2;
		} else {
			if((INT16U)*p < FontStruct.FirstCode1) {//FONT_CON_CH < 0x20
				nwbits = 0;
				nhbits = 0; ///////rot
			} else {
				if(FontStruct.FixedWidthFlag1 == 1 || StrStruct.UseFixedWidth == 1) {
					nwbits = FontStruct.FixedWbits1 * rx;
				} else {
					fontw_ptr = FontStruct.PtrWidth1 + ((INT16U)*p-FontStruct.FirstCode1);
					nwbits = *(fontw_ptr) * rx;
				}
////////////////////////////
				//HYP 20050224
				//nhbits = hbits1;
				nhbits = FontStruct.Hbits1*ry;
			}
			if(PrtStruct.Mode == RECEIPT && (INT16U)*p == HT) {	////////htab
				ncx = StrStruct.HTab[htab_pos_count]; ////
			}
		}
		if((StrStruct.ChRot&MASK_DEG_90_270) && (chw.words.left != 0x20)) {
			if (MbaselineH < baselineH) {
				MbaselineH = baselineH;
			}
			if (MbaselineL < baselineL) {
				MbaselineL = baselineL;
			}			
		   	if ((chw.words.left != LF) && (chw.words.left != '\0')) {
			   	if (((cx != 0) || (chw.words.left != 0x20)) && (ctrl_ch == OFF)) {
			   		cx = cx + hbits; 
					cx = cx + StrStruct.InterChSp;
				}
				if (chw.words.left == 0x20 || ctrl_ch == ON) {
					bits_sum = 0;
				} else {
					bits_sum = bits_sum + hbits + StrStruct.InterChSp;
				}
			}
			if((cx+nhbits > StrStruct.StrWidth) || (ncx > StrStruct.StrWidth) || (chw.words.left == LF) || (chw.words.left == '\0')) {/////htab
				if(PrtStruct.Mode != RECEIPT) {/////////htab
					if (wordwrap == ON) {
						if((chw.words.left != 0x20) && (*p != 0x20) && (*p != HT) && (chw.words.left != LF) && (chw.words.left != '\0') && (cx != bits_sum)) {////htab
							cx = cx - bits_sum ;
							p = p - p_sum;
							if (cx != 0) {
								if (*(p-1) == 0x20){   ///htab
									cx = cx - sbits - 2*StrStruct.InterChSp;
									p--;
								} else {
									cx = cx - StrStruct.InterChSp;
								}
							}
					 	} else {
							if (cx != 0) {
								if (chw.words.left == 0x20){
									cx = cx - sbits - 2*StrStruct.InterChSp;
									p--;
								} else { 
									cx = cx - StrStruct.InterChSp;
								}
							}
						}
					} else {
						if (cx != 0) {
							if (chw.words.left == 0x20) {
								cx = cx - sbits - 2*StrStruct.InterChSp;
								p--;
							} else {
								cx = cx - StrStruct.InterChSp;
							}
						}
					}
					p_sum = 0;
					bits_sum = 0;
					by[line_idx] = cy + MbaselineH;
					line_height = MbaselineH + MbaselineL;
					cy = cy + line_height;
					max_line = line_idx;
					if(cy > StrStruct.StrHeight) {
						PrtStruct.StrAreaOverFlag = TRUE;//HYP 20040903	not applied to PC program (PrtRender)
						whiteSpace = 0;
						loop = FALSE;
					} else {
						whiteSpace = StrStruct.StrWidth - cx;
						//HYP 20040903
						//if(*p == '\0') {
						if((*p == '\0') || (chw.words.left == '\0')) {
							loop=FALSE;
							max_line++;
						}
					}
					cy = cy + StrStruct.LineSp;
					MbaselineH = 0;
					MbaselineL = 0;
					cx = 0;
				} else {  //////PrtStruct.Mode == RECEIPT
					by[line_idx] = cy + MbaselineH;
					PrtStruct.ReceiptLineHeight = MbaselineH + MbaselineL;
					if(PrtStruct.ReceiptLineHeight <= StrStruct.StrHeight) {
						if(max_cx < cx) max_cx = cx;
						whiteSpace = StrStruct.StrWidth - max_cx + StrStruct.InterChSp;
						max_line = 1;
						p_max = p;	////////
					} else {
						max_line = 0;
						PrtStruct.ReceiptLineHeight = 0;
					}				
					loop = FALSE;
				}
				line_end=TRUE;
			}
		} else {
			if (MbaselineH < baselineH) {
				MbaselineH = baselineH;
			}
			if (MbaselineL < baselineL) {
				MbaselineL = baselineL;
			}
			if ((chw.words.left != LF) && (chw.words.left != '\0')) {
				if (((cx != 0) || (chw.words.left != 0x20)) && (ctrl_ch == OFF)) {
					cx = cx + wbits; 
					cx = cx + StrStruct.InterChSp;
				}
				if (chw.words.left == 0x20 || ctrl_ch == ON) {
					bits_sum = 0;
				} else {
					bits_sum = bits_sum + wbits + StrStruct.InterChSp;
				}
			}
			if((cx+nwbits > StrStruct.StrWidth) || (ncx > StrStruct.StrWidth) || (chw.words.left == LF) || (chw.words.left == '\0')) {/////htab
				if(PrtStruct.Mode != RECEIPT) {/////////htab
					if (wordwrap == ON) {
						if((chw.words.left != 0x20) && (*p != 0x20) && (*p != HT) && (chw.words.left != LF) && (chw.words.left != '\0') && (cx != bits_sum)) {///htab
							cx = cx - bits_sum ;
							p = p - p_sum;				   
							if (cx != 0) {
								if (*(p-1) == 0x20){
									cx = cx - sbits - 2*StrStruct.InterChSp;
									p--;
								} else {
									cx = cx - StrStruct.InterChSp;
								}
							}
					 	} else {
							if (cx != 0) {
								if (chw.words.left == 0x20){
									cx = cx - sbits - 2*StrStruct.InterChSp;
									p--;
								} else { 
									cx = cx - StrStruct.InterChSp;
								}
							}
						}
					} else {
						if (cx != 0) {
							if (chw.words.left == 0x20) {
								cx = cx - sbits - 2*StrStruct.InterChSp;
								p--;
							} else { 
								cx = cx - StrStruct.InterChSp;
							}
						}
					}
					p_sum = 0;
					bits_sum = 0;
					by[line_idx] = cy + MbaselineH;
					line_height = MbaselineH + MbaselineL;
					cy = cy + line_height;
					max_line = line_idx;
					if(cy > StrStruct.StrHeight) {
						if(StrStruct.AutoShrinkx == ON) {	//auto shrink mode
							if(StrStruct.Magx != 1) {
								variable_init_flag = 1;		//for auto shrink
								loop = TRUE;
								StrStruct.Magx--;
							} else {
								PrtStruct.StrAreaOverFlag = TRUE;//HYP 20040903	not applied to PC program (PrtRender)
								whiteSpace = 0;
								loop = FALSE;
								cy = cy - StrStruct.LineSp - StrStruct.LineSp - line_height;//for vertical centering
						    }
						} else {
							PrtStruct.StrAreaOverFlag = TRUE;//HYP 20040903	not applied to PC program (PrtRender)
							whiteSpace = 0;
							loop = FALSE;
							cy = cy - StrStruct.LineSp - StrStruct.LineSp - line_height;//for vertical centering
						}
					} else {
						whiteSpace = StrStruct.StrWidth - cx;	  
						//HYP 20040903
						//if(*p == '\0') 
						if((*p == '\0') || (chw.words.left == '\0')){
							loop=FALSE;
							max_line++;
						}
					}
					cy = cy + StrStruct.LineSp;
					MbaselineH = 0;
					MbaselineL = 0;
					cx = 0;
				} else { //PrtStruct.Mode == RECEIPT
					by[line_idx] = cy + MbaselineH;
					PrtStruct.ReceiptLineHeight =  MbaselineH + MbaselineL;
					if(PrtStruct.ReceiptLineHeight <= StrStruct.StrHeight) {
						if(max_cx < cx) max_cx = cx;
						whiteSpace = StrStruct.StrWidth - max_cx + StrStruct.InterChSp;
						max_line = 1;
						p_max = p;	////////
					} else {
						max_line = 0;
						PrtStruct.ReceiptLineHeight = 0;
					}
					loop = FALSE;
				}
				line_end=TRUE;
			}
		}
		if(PrtStruct.Mode == LABEL)	//VCentering은 RECIEPT인 경우에는 사용하지 않음.R_Interprerter없음.
		{
			if((StrStruct.VCentering == ON) && (loop==FALSE)) {	  //Vertical Centering ,change StrStruct.StrHeight
				whitespace_V = StrStruct.StrHeight - cy;
				if (whitespace_V > 0) {
					whitespace_V = 	whitespace_V>>1;
					switch(PrtStruct.Rot) {
						case DEG_0: 
							sy = sy + whitespace_V;
							StrStruct.StrHeight = cy;
							break;
						case DEG_90:
							sx = sx + whitespace_V;
							StrStruct.StrHeight = cy;
							break;
						case DEG_180: 
							sy = sy + whitespace_V;
							StrStruct.StrHeight = cy;
							break;
						case DEG_270:
							sx = sx + whitespace_V;
							StrStruct.StrHeight = cy;
							break;
					}
				}
			}
		}
		if(line_end==TRUE) {
			line_end = FALSE;
			switch (justification) {		// apply justification control
				case CENTER:
					str_left_margin[line_idx] = whiteSpace / 2;
					str_right_margin[line_idx] = whiteSpace / 2;
					break;
				case RIGHT:
					//HYP 20040226///////////////
					if(PrtStruct.Mode == RECEIPT) {
						if(StrStruct.Typeface & ITALIC) {	
							whiteSpace = whiteSpace - ((line_height/StrStruct.Magy)/6)*StrStruct.Magx; 
						} 
						if(StrStruct.Typeface & BOLD) {	
							whiteSpace = whiteSpace - StrStruct.Magx;
						}
						if(StrStruct.Typeface & OUTLINE) {	
							whiteSpace = whiteSpace - StrStruct.Magx;
						}
						if(whiteSpace < 0) whiteSpace = 0; 
					}
					//////////////////////////////
					str_left_margin[line_idx] = whiteSpace;
					str_right_margin[line_idx] = 0;
					break;
				case LEFT:
				default:	// to Left
					str_left_margin[line_idx] = 0;
					str_right_margin[line_idx] = whiteSpace;
					break;
			}
			line_idx++;
		}
	}
	//start rendering

	StrStruct.ChRot = ch_rot_backup;  /////////rot
	FontLoadFont1(id1_backup);
	FontLoadFont2(id2_backup);

	baseline2 = FontStruct.Baseline2*StrStruct.Magy;
	baseline1 = FontStruct.Baseline1*StrStruct.Magy;
	wbits2 = FontStruct.Wbits2*StrStruct.Magx;	  	// mag. width bits of 2 bytes font
	hbits2 = FontStruct.Hbits2*StrStruct.Magy;		// mag. height bits of 2 bytes font
	wbits1 = FontStruct.Wbits1*StrStruct.Magx;		// mag. width bits of 1 bytes font
	hbits1 = FontStruct.Hbits1*StrStruct.Magy;		// mag. height bits of 1 bytes font

	wbits = wbits1;		// mag. width inc. bits of 1 str line
	hbits = hbits1;		// mag. height inc. bits of 1 str line
	nwbits = wbits1;		// mag. width inc. bits of 1 str line
	nhbits = hbits1;		// mag. height inc. bits of 1 str line
	//	baseline_height = 0;//HYP 20050224 erase baseline_height
#ifdef USE_ARAB_FONT
 #ifdef USE_ENG_BASED_ARAB_SUPPORT
	p = eng_codetemp; //for arab version
 #else
	p = arab_codetemp; //for arab version
 #endif
#elif defined(HEBREW_FONT)
	p = hebrew_codetemp; //for arab version
#endif
	cx = str_left_margin[0];
		
	// rel. character pos x
	cy = 0;
	loop = TRUE;
	line_idx = 0;
	htab_pos_count = 0;	 ////////htab
	if(line_idx >= max_line) {
		loop = FALSE;
	}

	while (loop) {
		ctrl_ch = OFF;
		chw.words.left = *p;
		if(chw.words.left == _CR) {
			if(*(p+1) == LF) {
				p++;
			}
			chw.words.left = LF;
		}
		if (StrStruct.StrType == TWO_BYTE_FONT_STR && chw.words.left > 0x80) {
			if(StrStruct.ChRot & MASK_DEG_90_270) {	//chw.words.left != 0x20
				cy = by[line_idx] - wbits2;
			} else {
				cy = by[line_idx] - hbits2 + baseline2;
			}
			chw.words.right = *(p+1);
			rot = PrtStruct.Rot + StrStruct.ChRot; //chw.words.left != 0x20
	    		switch(PrtStruct.Rot) {
				case DEG_0:
					if(StrStruct.StrMirx) {
						crx = sx + StrStruct.StrWidth - cx;
						cry = sy + cy;
						if(rot&MASK_DEG_90_270){
							crx = crx - hbits2;
						} else {
							crx = crx - wbits2;
						}	
					} else {
						crx = sx + cx;
						cry = sy + cy;
					}
					PrtStruct.x = crx;
					PrtStruct.y = cry;
					FontDCharOut(chw.word, rot);
					break;
				case DEG_90:
					if(StrStruct.StrMirx) {
						crx = sx + StrStruct.StrHeight - cy;
						cry = sy + StrStruct.StrWidth - cx;
						if(rot&MASK_DEG_90_270){
							crx = crx - hbits2;
							cry = cry - wbits2;
						} else {
							crx = crx - wbits2;
							cry = cry - hbits2;
						}					
					} else {
						crx = sx + StrStruct.StrHeight - cy;
						cry = sy + cx;
						if(rot&MASK_DEG_90_270){
							crx = crx - hbits2;
						} else {
							crx = crx - wbits2;
						}
					}
					PrtStruct.x = crx;
					PrtStruct.y = cry;
					FontDCharOut(chw.word, rot);						
					break;
				case DEG_180:
					if(StrStruct.StrMirx) {
						crx = sx + cx;
						cry = sy + StrStruct.StrHeight - cy;
						if(rot&MASK_DEG_90_270){
							cry = cry - wbits2;
						} else {
							cry = cry - hbits2;
						}
					} else {
						crx = sx + StrStruct.StrWidth - cx;
						cry = sy + StrStruct.StrHeight - cy;
						if(rot&MASK_DEG_90_270){
							cry = cry - wbits2;
							crx = crx - hbits2;
						} else {
							cry = cry - hbits2;
							crx = crx - wbits2;
						}
					}
					PrtStruct.x = crx;
					PrtStruct.y = cry;
					FontDCharOut(chw.word, rot);
					break;
				case DEG_270:
					if(StrStruct.StrMirx) {
						crx = sx + cy;
						cry = sy + cx;					
					} else {
						crx = sx + cy;
						cry = sy + StrStruct.StrWidth - cx;
						if(rot&MASK_DEG_90_270){
							cry = cry - wbits2;
						} else {
							cry = cry - hbits2;
						}
					}
					PrtStruct.x = crx;
					PrtStruct.y = cry;						
					FontDCharOut(chw.word, rot);
					break;
			}				
	       		p = p + 2;
			wbits = wbits2;		
			hbits = hbits2;
		} else {
   	    		p++;
			if (chw.words.left == FONT_CON_CH) {
				ctrl_ch = ON;
				switch (*p)	{
					/////////rot
					case FONT_CON_DEG:
						p++;
						switch(*p) {
							case '0':
								StrStruct.ChRot = DEG_0;
								break;
							case '1':
								StrStruct.ChRot = DEG_90;
								break;
							case '2':
								StrStruct.ChRot = DEG_180;
								break;
							case '3':
								StrStruct.ChRot = DEG_270;
								break;
						}
						break;
					//Typeface
					case FONT_CON_TYPE_ITALIC:
						StrStruct.Typeface |= ITALIC;
						break;
					case FONT_CON_TYPE_UNDERLINE:
						StrStruct.Typeface |= UNDERLINE;
						break;
					case FONT_CON_TYPE_THROUGH_LINE:
						StrStruct.Typeface |= THROUGH_LINE;
						break;
					//20040209 HYP
					//case FONT_CON_TYPE_SHADOW:
					//	StrStruct.Typeface |= SHADOW;
					//	break;
					case FONT_CON_TYPE_BOLD:
						StrStruct.Typeface |= BOLD;
						break;
					//20040209 HYP
					//case FONT_CON_TYPE_REVERSE:
					//	StrStruct.Typeface |= REVERSE;
					//	break;
					case FONT_CON_TYPE_DTHROUGH_LINE:
						StrStruct.Typeface |= DTHROUGH_LINE;
						break;
					case FONT_CON_TYPE_OUTLINE:
						StrStruct.Typeface |= OUTLINE;
						break;
					case FONT_CON_TYPE_NO_TYPEFACE:
						StrStruct.Typeface = NO_TYPEFACE;
						break;
					case FONT_CON_MAG_X:
						p++;
						switch(*p) {
							case '1':
								StrStruct.Magx = 1;
								break;
							case '2':
								StrStruct.Magx = 2;
								break;
							case '3':
								StrStruct.Magx = 3;
								break;
							case '4':
								StrStruct.Magx = 4;
								break;
							case '5':
								StrStruct.Magx = 5;
								break;
						}
						break;
					case FONT_CON_MAG_Y:
						p++;
						switch(*p) {
							case '1':
								StrStruct.Magy = 1;
								break;
							case '2':
								StrStruct.Magy = 2;
								break;
							case '3':
								StrStruct.Magy = 3;
								break;
							case '4':
								StrStruct.Magy = 4;
								break;
							case '5':
								StrStruct.Magy = 5;
								break;
						}
						break;
					case FONT_CON_INC_MAG:
						if(StrStruct.Magx < 5) {
							StrStruct.Magx++;
						}
						if(StrStruct.Magy < 5) {
							StrStruct.Magy++;
						}
						break;
					case FONT_CON_DEC_MAG:
						if(StrStruct.Magx > 1) {
							StrStruct.Magx--;
						}
						if(StrStruct.Magy > 1) {
							StrStruct.Magy--;
						}
						break;
					case FONT_CON_FONT2:
						p++;
						FontLoadFont2(*p - '0');
						break;
					case FONT_CON_FONT1:
						p++;
						FontLoadFont1(*p - '0');
						break;
				//HYP 20040621
				//FONT_CON_CH > 0x20
				//	case '0':
				//		ctrl_ch = OFF;
				//		break;
				}
				p++;
				baseline2 = FontStruct.Baseline2*StrStruct.Magy;
				baseline1 = FontStruct.Baseline1*StrStruct.Magy;
				wbits2 = FontStruct.Wbits2*StrStruct.Magx;	  	// mag. width bits of 2 bytes font
				hbits2 = FontStruct.Hbits2*StrStruct.Magy;		// mag. height bits of 2 bytes font
				wbits1 = FontStruct.Wbits1*StrStruct.Magx;		// mag. width bits of 1 bytes font
				hbits1 = FontStruct.Hbits1*StrStruct.Magy;		// mag. height bits of 1 bytes font
			}
			if(chw.words.left < (INT16U)FontStruct.FirstCode1) {
				//HYP 20050224
				//wbits1 = 0;
				if (StrStruct.ChRot&MASK_DEG_90_270) {
					hbits1 = 0;
				} else {
					wbits1 = 0;
				}				
				ctrl_ch = ON;  
			} else {
//Insert Fixed Font 20031218
//				fontw_ptr = FontStruct.PtrWidth1 + ((INT8U)chw.words.left-FontStruct.FirstCode1);
//				wbits1 = *(fontw_ptr) * StrStruct.Magx;
				if(FontStruct.FixedWidthFlag1 == 1 || StrStruct.UseFixedWidth == 1) {
					wbits1 = FontStruct.FixedWbits1 * StrStruct.Magx;
				} else {
					fontw_ptr = FontStruct.PtrWidth1 + ((INT16U)chw.words.left-FontStruct.FirstCode1);
					wbits1 = *(fontw_ptr) * StrStruct.Magx;
				}
				//HYP 20050224
				hbits1=FontStruct.Hbits1*StrStruct.Magy;				
////////////////////////////
			}
			if((StrStruct.ChRot & MASK_DEG_90_270) && (chw.words.left != 0x20)) { 
				cy = by[line_idx] - wbits1;
			} else {
				cy = by[line_idx] - hbits1 + baseline1;
			}
			if(PrtStruct.Mode == RECEIPT && chw.words.left == HT) {	////////htab
				cx = str_left_margin[line_idx] + StrStruct.HTab[htab_pos_count]; ///htab
				htab_pos_count++;
			}
 			if((chw.words.left != LF) && ((cx != str_left_margin[line_idx]) || (chw.words.left != 0x20)) && (ctrl_ch == OFF)) {
				if((StrStruct.ChRot & MASK_DEG_90_270) && (chw.words.left == 0x20)) {
					rot = PrtStruct.Rot;
				} else {
					rot = PrtStruct.Rot + StrStruct.ChRot;
				}
 			   	switch(PrtStruct.Rot) {
					case DEG_0:
						if(StrStruct.StrMirx) {
							crx = sx + StrStruct.StrWidth - cx;
							cry = sy + cy;
							if(rot&MASK_DEG_90_270){
								crx = crx - hbits1;
							} else {
								crx = crx - wbits1;
							}	
						} else {
							crx = sx + cx;
							cry = sy + cy;
						}
						PrtStruct.x = crx;
						PrtStruct.y = cry;
						FontCharOut(chw.words.left, rot);
						break;
					case DEG_90:
						if(StrStruct.StrMirx) {
							crx = sx + StrStruct.StrHeight - cy;
							cry = sy + StrStruct.StrWidth - cx;
							if(rot&MASK_DEG_90_270){
								crx = crx - hbits1;
								cry = cry - wbits1;
							} else {
								crx = crx - wbits1;
								cry = cry - hbits1;
							}					
						} else {
							crx = sx + StrStruct.StrHeight - cy;
							cry = sy + cx;
							if(rot&MASK_DEG_90_270){
								crx = crx - hbits1;
							} else {
								crx = crx - wbits1;
							}
						}
						PrtStruct.x = crx;
						PrtStruct.y = cry;
						FontCharOut(chw.words.left, rot);						
						break;
					case DEG_180:
						if(StrStruct.StrMirx) {
							crx = sx + cx;
							cry = sy + StrStruct.StrHeight - cy;
							if(rot&MASK_DEG_90_270){
								cry = cry - wbits1;
							} else {
								cry = cry - hbits1;
							}
						} else {
							crx = sx + StrStruct.StrWidth - cx;
							cry = sy + StrStruct.StrHeight - cy;
							if(rot&MASK_DEG_90_270){
								cry = cry - wbits1;
								crx = crx - hbits1;
							} else {
								cry = cry - hbits1;
								crx = crx - wbits1;
							}
						}
						PrtStruct.x = crx;
						PrtStruct.y = cry;
						FontCharOut(chw.words.left, rot);
						break;
					case DEG_270:
						if(StrStruct.StrMirx) {
							crx = sx + cy;
							cry = sy + cx;					
						} else {
							crx = sx + cy;
							cry = sy + StrStruct.StrWidth - cx;
							if(rot&MASK_DEG_90_270){
								cry = cry - wbits1;
							} else {
								cry = cry - hbits1;
							}
						}
						PrtStruct.x = crx;
						PrtStruct.y = cry;
						FontCharOut(chw.words.left, rot);
						break;
				}
			}
			wbits = wbits1;		
			hbits = hbits1;
		}
		if (StrStruct.StrType == TWO_BYTE_FONT_STR && (INT8U)(*p) > 0x80) {
			nwbits = wbits2;		
			nhbits = hbits2;
		} else {
//			if(((INT8U)*p < FontStruct.FirstCode1) || ((INT8U)*p == FONT_CON_CH && (INT8U)*(p+1) != FONT_CON_CH)) {
//HYP 20040621
//			if(((INT8U)*p < FontStruct.FirstCode1) || ((INT8U)*p == FONT_CON_CH && (INT8U)*(p+1) != '0')) {//FONT_CON_CH > 0x20 //HYP 20040127
			if(*p < (INT16U)FontStruct.FirstCode1) {//FONT_CON_CH < 0x20
//HYP 20040621
				nwbits = 0;
				nhbits = 0;///////rot
//HYP 20040611
/*
				if(*p == LF) {//if interchcp==0, 1 char out before LF.
					nwbits = 1;
					nhbits = 1;///////rot
				} else {
					nwbits = 0;
					nhbits = 0;///////rot
				}
*/

			} else {
//Insert Fixed Font 20031218
//				fontw_ptr = FontStruct.PtrWidth1 + ((INT8U)*p-FontStruct.FirstCode1);
//				nwbits = *(fontw_ptr) * StrStruct.Magx;
				if(FontStruct.FixedWidthFlag1 == 1 || StrStruct.UseFixedWidth == 1) {
					nwbits = FontStruct.FixedWbits1 * StrStruct.Magx;
				} else {
					fontw_ptr = FontStruct.PtrWidth1 + ((INT16U)*p-FontStruct.FirstCode1);
					nwbits = *(fontw_ptr) * StrStruct.Magx;
				}
//////////////////////////// 
				//HYP 20050224
				//nhbits = hbits1;
				nhbits=FontStruct.Hbits1*StrStruct.Magy;				
			}
		}
		// increase rel ch pos include ch rot
			
		if((StrStruct.ChRot&MASK_DEG_90_270) && (chw.words.left != 0x20)) {
//HYP 20050224 erase
//			if (line_height < wbits) {
//				line_height = wbits;
//			}
//HYP 20040621
//	    	if ((chw.words.left != LF) && ((cx != str_left_margin[line_idx]) || (chw.words.left != 0x20)) && (ctrl_ch == OFF)) {///htab
//			   	cx = cx + hbits; 
//				cx = cx + StrStruct.InterChSp;
//HYP 20040807 	//if interchcp==0, 1 char out before LF.
//	    	if ((chw.words.left != LF) && ((cx != str_left_margin[line_idx]) || (chw.words.left != 0x20))) {///htab
	    	if ((cx != str_left_margin[line_idx]) || (chw.words.left != 0x20)) {///htab
				if(ctrl_ch == OFF) {
					cx = cx + hbits; 
					cx = cx + StrStruct.InterChSp;
				}
				if((cx + nhbits) > (StrStruct.StrWidth - str_right_margin[line_idx]) && (PrtStruct.Mode != RECEIPT || p >= p_max)) {/////htab
					line_idx++;
					cx = str_left_margin[line_idx];
					//HYP 20050224
					//while ((cx + hbits) > (StrStruct.StrWidth - str_right_margin[line_idx])) {
					while ((cx + nhbits) > (StrStruct.StrWidth - str_right_margin[line_idx])) {
						line_idx++;
						if(line_idx >= max_line) {
							loop = FALSE;
							break;
						}
						cx = str_left_margin[line_idx];
					}
					if(line_idx >= max_line) {
						loop = FALSE;
					}
				} else {
					if (((PrtStruct.Opaque == ON) || (StrStruct.Typeface != NO_TYPEFACE))) {
						if ((cx < (StrStruct.StrWidth - str_right_margin[line_idx])) && (StrStruct.InterChSp != 0)) {
							sphbits = wbits/StrStruct.Magy;
							for ( i = 0; i < (spbytes * sphbits); i++) {
								PrtStruct.PrtScratchPad[i] = 0x00;
							}
							Font_Typeface(PrtStruct.PrtScratchPad,spbytes,StrStruct.InterChSp,sphbits,DEG_0);
 					   		switch(PrtStruct.Rot + StrStruct.StrMirx * 2) {
								case DEG_0:
								case DEG_360:
									PrtStruct.x = crx + hbits;
									PrtStruct.y = cry;
									break;
								case DEG_90:
								case DEG_450:
									PrtStruct.x = crx;
									PrtStruct.y = cry + hbits;
									break;
								case DEG_180:
								case DEG_540:
									PrtStruct.x = crx - StrStruct.InterChSp;
									PrtStruct.y = cry;
									break;
								case DEG_270:
								case DEG_630:
									PrtStruct.x = crx;
									PrtStruct.y = cry - StrStruct.InterChSp;
									break;
							}
							rx = StrStruct.Magx; //backup
							StrStruct.Magx = 1;
				   	 		PrtPattern(PrtStruct.PrtScratchPad,spbytes,StrStruct.InterChSp,sphbits,PrtStruct.Rot);
							StrStruct.Magx = rx;	
	  					}
					}
				}
			}
		} else {
//HYP 20050224 erase
//			if (line_height < hbits) {
//				line_height = hbits;
//			}
//HYP 20040621
//			if ((chw.words.left != LF) && ((cx != str_left_margin[line_idx]) || (chw.words.left != 0x20)) && (ctrl_ch == OFF)) {//////htab
//				cx = cx + wbits;
//				cx = cx + StrStruct.InterChSp;
//HYP 20040807 	//if interchcp==0, 1 char out before LF.
//			if ((chw.words.left != LF) && ((cx != str_left_margin[line_idx]) || (chw.words.left != 0x20))) {//////htab
			if ((cx != str_left_margin[line_idx]) || (chw.words.left != 0x20)) {//////htab
				if(ctrl_ch == OFF) {
					cx = cx + wbits;
					cx = cx + StrStruct.InterChSp;
				}
				if((cx + nwbits) > (StrStruct.StrWidth - str_right_margin[line_idx]) && (PrtStruct.Mode != RECEIPT || p >= p_max)) { //////htab
					line_idx++;
					cx = str_left_margin[line_idx];
					//HYP 20050224
					//while ((cx + wbits) > (StrStruct.StrWidth - str_right_margin[line_idx])) {
					while ((cx + nwbits) > (StrStruct.StrWidth - str_right_margin[line_idx])) {
						line_idx++;
						if(line_idx >= max_line) {
							loop = FALSE;
							break;
						}
						cx = str_left_margin[line_idx];
					}
					if(line_idx >= max_line) {
						loop = FALSE;
					}
				} else {
					if (((PrtStruct.Opaque == ON) || (StrStruct.Typeface != NO_TYPEFACE))) {
						if ((cx < (StrStruct.StrWidth - str_right_margin[line_idx])) && (StrStruct.InterChSp != 0)) {
							sphbits = hbits/StrStruct.Magy;
							for ( i = 0; i < (spbytes * sphbits); i++) {
								PrtStruct.PrtScratchPad[i] = 0x00;   
							}
							Font_Typeface(PrtStruct.PrtScratchPad,spbytes,StrStruct.InterChSp,sphbits,DEG_0);
 			   				switch(PrtStruct.Rot + StrStruct.StrMirx * 2) {
								case DEG_0:
								case DEG_360:
									PrtStruct.x = crx + wbits;
									PrtStruct.y = cry;
									break;
								case DEG_90:
								case DEG_450:
									PrtStruct.x = crx;
									PrtStruct.y = cry + wbits;
									break;
								case DEG_180:
								case DEG_540:
									PrtStruct.x = crx - StrStruct.InterChSp;
									PrtStruct.y = cry;
									break;
								case DEG_270:
								case DEG_630:
									PrtStruct.x = crx;
									PrtStruct.y = cry - StrStruct.InterChSp;
									break;
							}
							rx = StrStruct.Magx; //backup
							StrStruct.Magx = 1;
				  	  		PrtPattern(PrtStruct.PrtScratchPad,spbytes,StrStruct.InterChSp,sphbits,PrtStruct.Rot);
							StrStruct.Magx = rx;	
						}
					}
				}
			}
		}
		if(*p == '\0') loop=FALSE;
    }
	StrStruct.Magx = magx_backup;	 //for auto shrink
	//20040209 HYP
	PrtStruct.x = backup_x;
	PrtStruct.y = backup_y;
	PrtPattern_Str_Area_Typeface();
}
#else //USE_ARAB_FONT
#ifdef USE_VIETNAM_FONT
void  FontStrOut(HUGEDATA char *ch_ptr) 
{
	short cx,cy,crx,cry,bits_sum,i;
	short by[30],str_left_margin[30],str_right_margin[30],whiteSpace,whitespace_V;
	char rot,wbits1,hbits1,wbits2,hbits2,wbits,hbits,nwbits,nhbits,justification,sbits;
	char line_idx,line_end,loop,spbytes,rx,ry;
	CWDVIET		chw;
	HUGEDATA INT16U *p, *p_max;
	INT16U position_counter;
	INT16U strnumber, calculatestrnum;
	char wordwrap, p_sum, /*baseline_height,*/ctrl_ch,max_line;//HYP 20050224 erase baseline_height
	short MbaselineH,MbaselineL,baselineL,baselineH,baseline1,baseline2,line_height;
	short sx,sy;
	HUGEDATA INT8U *fontw_ptr;
	char variable_init_flag, magx_backup;
	char ch_rot_backup;/////////rot
	INT8U	sphbits;
	short	ncx;////////htab
	INT8U	htab_pos_count;	////////htab
	short	max_cx;////////htab
	BOOLEAN updata_mag;
	INT8U id1_backup, id2_backup;
//	char string_buf[30];
	//20040209 HYP
	short backup_x,backup_y;
	backup_x = PrtStruct.x;
	backup_y = PrtStruct.y;

	variable_init_flag = 1;
	loop = TRUE;	
	magx_backup = StrStruct.Magx;	 //for auto shrink
	ch_rot_backup = StrStruct.ChRot;  /////////rot
	id1_backup = StrStruct.Id1;
	id2_backup = StrStruct.Id2;

	if(*ch_ptr == '\0') return;
	strnumber = strlen(ch_ptr);
	calculatestrnum = Cvt_VietnamPreview((INT8U*)ch_ptr, strnumber, strnumber-1, 0); //gm 081103
	while (loop) {
		if(variable_init_flag == 1) {
			variable_init_flag = 0;

			sx = PrtStruct.x;
			sy = PrtStruct.y;
			spbytes = StrStruct.InterChSp>>3;
			if(StrStruct.InterChSp&0x07) spbytes++;
			p = VietnamCode_buf;
			wordwrap = StrStruct.WordWrap;
			cx = 0;								// rel. character pos x
			cy = 0;								// rel. character pos y
			justification = StrStruct.Justification;
			if(StrStruct.StrMirx) { //mirror
				if(justification == RIGHT) {
					justification = LEFT;
				} else if(justification == LEFT) {
					justification = RIGHT;
				}
			}
//////////////////////////////////load font1,2 start
			rx = StrStruct.Magx;
			ry = StrStruct.Magy;	
			wbits2 = FontStruct.Wbits2*rx;	  	// mag. width bits of 2 bytes font
			hbits2 = FontStruct.Hbits2*ry;		// mag. height bits of 2 bytes font
			wbits1 = FontStruct.Wbits1*rx;		// mag. width bits of 1 bytes font
			hbits1 = FontStruct.Hbits1*ry;		// mag. height bits of 1 bytes font
			wbits = wbits1;		// mag. width inc. bits of 1 str line
			hbits = hbits1;		// mag. height inc. bits of 1 str line
			nwbits = wbits1;		// mag. width inc. bits of 1 str line
			nhbits = hbits1;		// mag. height inc. bits of 1 str line
			baseline2 = FontStruct.Baseline2*ry;
			baseline1 = FontStruct.Baseline1*ry;
//Insert Fixed Font 20031218
//			fontw_ptr = FontStruct.PtrWidth1 + ((INT8U)0x20-FontStruct.FirstCode1);
//			sbits = *(fontw_ptr) * rx;
			if(FontStruct.FixedWidthFlag1 == 1 || StrStruct.UseFixedWidth == 1) {
				sbits = FontStruct.FixedWbits1 * rx;
			} else {
				fontw_ptr = FontStruct.PtrWidth1 + ((INT16U)0x20-FontStruct.FirstCode1);
				sbits = *(fontw_ptr) * rx;
			}
////////////////////////////
//////////////////////////////////load font1,2 end
			str_left_margin[0] = 0;
			str_right_margin[0]=0;
			line_idx = 0;
			htab_pos_count = 0;	 ////////htab
			max_cx = 0;	 ////////htab
			ncx = 0;  ///////htab
			line_end = FALSE;
			by[0] = 0;
			baselineL = 0;
			baselineH = 0;

			p_sum = 0;
			bits_sum = 0;
			MbaselineH = 0;
			MbaselineL = 0;
			line_height = 0;
		}	/*  Init end  */

 //justification and baseline check
		ctrl_ch = OFF;
		chw.vietnam.left = *p;
		chw.vietnam.right = *(p+1);
		if((INT16U)chw.vietnam.left == _CR) {
			if(*(p+1) == LF) {
				p++;
			}
			chw.vietnam.left = LF;
		}
		if (StrStruct.StrType == TWO_BYTE_FONT_STR && (INT16U)chw.vietnam.left > 0x80) {
			p = p + 2;
			wbits = wbits2;		
			hbits = hbits2;
			baselineL = baseline2;
			baselineH = hbits2 - baseline2;
			p_sum = p_sum + 2;
		} else {
			p++;
			if ((INT16U)chw.vietnam.left == FONT_CON_CH) {
				ctrl_ch = ON;
				updata_mag = OFF;
				switch (*p) {
					///////////rot
					case FONT_CON_DEG:
						p++;
						switch(*p) {
							case '0':
								StrStruct.ChRot = DEG_0;
								break;
							case '1':
								StrStruct.ChRot = DEG_90;
								break;
							case '2':
								StrStruct.ChRot = DEG_180;
								break;
							case '3':
								StrStruct.ChRot = DEG_270;
								break;
						}
						break;
					case FONT_CON_MAG_X:
						p++;
						switch(*p) {
							case '1':
								rx = 1;
								break;
							case '2':
								rx = 2;
								break;
							case '3':
								rx = 3;
								break;
							case '4':
								rx = 4;
								break;
							case '5':
								rx = 5;
								break;
						}
						updata_mag = ON;
						break;
					case FONT_CON_MAG_Y:
						p++;
						switch(*p) {
							case '1':
								ry = 1;
								break;
							case '2':
								ry = 2;
								break;
							case '3':
								ry = 3;
								break;
							case '4':
								ry = 4;
								break;
							case '5':
								ry = 5;
								break;
						}
						updata_mag = ON;
						break;
					case FONT_CON_INC_MAG:
						if(rx < 5) {
							rx++;
						}
						if(ry < 5) {
							ry++;
						}
						updata_mag = ON;
						break;
					case FONT_CON_DEC_MAG:
						if(rx > 1) {
							rx--;
						}
						if(ry > 1) {
							ry--;
						}
						updata_mag = ON;
						break;
					case FONT_CON_FONT2:
						p++;
						FontLoadFont2(*p - '0');
						updata_mag = ON;
						break;
					case FONT_CON_FONT1:
						p++;
						FontLoadFont1(*p - '0');
						updata_mag = ON;
						break;
				//HYP 20040621
				//FONT_CON_CH > 0x20
				//	case '0':
				//		ctrl_ch = OFF;
				//		break;
				}
				if(updata_mag == ON) {
					baseline2 = FontStruct.Baseline2*ry;
					baseline1 = FontStruct.Baseline1*ry;
					wbits2 = FontStruct.Wbits2*rx;	  	// mag. width bits of 2 bytes font
					hbits2 = FontStruct.Hbits2*ry;		// mag. height bits of 2 bytes font
					wbits1 = FontStruct.Wbits1*rx;		// mag. width bits of 1 bytes font
					hbits1 = FontStruct.Hbits1*ry;		// mag. height bits of 1 bytes font
					fontw_ptr = FontStruct.PtrWidth1 + ((INT16U)0x20-FontStruct.FirstCode1);
				}
				p++;
			}
			//HYP 20071130
			//if((INT8U)chw.byte.left < FontStruct.FirstCode1) {
			if(chw.vietnam.left < (INT16U)0x20) {
				//HYP 20050224
				//wbits1 = 0;
				if (StrStruct.ChRot&MASK_DEG_90_270) {
					hbits1 = 0;
				} else {
					wbits1 = 0;
				}
				ctrl_ch = ON;  
			} else {
//Insert Fixed Font 20031218
//				fontw_ptr = FontStruct.PtrWidth1 + ((INT8U)chw.byte.left-FontStruct.FirstCode1);
//				wbits1 = *(fontw_ptr) * rx;
				if(FontStruct.FixedWidthFlag1 == 1 || StrStruct.UseFixedWidth == 1) {
					wbits1 = FontStruct.FixedWbits1 * rx;
				} else {
					fontw_ptr = FontStruct.PtrWidth1 + ((INT16U)chw.vietnam.left-FontStruct.FirstCode1);
					wbits1 = *(fontw_ptr) * rx;
				}
				//HYP 20050224
				hbits1= FontStruct.Hbits1*ry;
////////////////////////////
			}
			wbits = wbits1;		
			hbits = hbits1;
			//HYP 20050224
			//baselineH = hbits1 - baseline1;
			//baselineL = baseline1;
			if (StrStruct.ChRot&MASK_DEG_90_270) {
				baselineH = wbits1;
				baselineL = 0;
			} else {
				baselineH = hbits1 - baseline1;
				baselineL = baseline1;
			}
			if((INT16U)chw.vietnam.left != 0x20) {
//				if ((ctrl_ch == ON) || (chw.byte.left == FONT_CON_CH)) {
//					p_sum = 0;	  //////////////////
//				} else {
//					p_sum = p_sum + 1;
//				}
				//HYP 20040128
				if ((ctrl_ch == ON) || ((INT16U)chw.vietnam.left == FONT_CON_CH)) {
//HYP 20040621
					//FONT_CON_CH > 0x20
//					if(chw.byte.right == '0') {
//						p_sum = p_sum + 2;
//					} else {
//						p_sum = 0;
//					}
					p_sum = 0; //FONT_CON_CH < 0x20
				} else {
					p_sum = p_sum + 1;
				}
				if(PrtStruct.Mode == RECEIPT && (INT16U)chw.vietnam.left == HT) {////////htab
					if(max_cx < cx) max_cx = cx;
					cx = StrStruct.HTab[htab_pos_count]; ///htab 
					htab_pos_count++;
					if(max_cx < cx) max_cx = cx;
				}
			} else {
//Insert Fixed Font 20031218
//				sbits = *(fontw_ptr) * rx;/////////////////////////
				if(FontStruct.FixedWidthFlag1 == 1 || StrStruct.UseFixedWidth == 1) {
					sbits = FontStruct.FixedWbits1 * rx;
				} else {
					sbits = *(fontw_ptr) * rx;/////////////////////////
				}
////////////////////////////
				p_sum = 0;
			}
		}
		if (StrStruct.StrType == TWO_BYTE_FONT_STR && (INT16U)(*p) > 0x80) {	//you must cast!!!
			nwbits = wbits2;		
			nhbits = hbits2;
		} else {
//			if(((INT8U)*p < FontStruct.FirstCode1) || ((INT8U)*p == FONT_CON_CH && (INT8U)*(p+1) != FONT_CON_CH)) {
//HYP 20040621
//			if(((INT8U)*p < FontStruct.FirstCode1) || ((INT8U)*p == FONT_CON_CH && (INT8U)*(p+1) != '0')) {//FONT_CON_CH > 0x20 //HYP 20040127
			//HYP 20071130
			//if((INT8U)*p < FontStruct.FirstCode1) {//FONT_CON_CH < 0x20
			if((INT16U)*p < 0x20) {//FONT_CON_CH < 0x20
				nwbits = 0;
				nhbits = 0; ///////rot
			} else {
//Insert Fixed Font 20031218
//				fontw_ptr = FontStruct.PtrWidth1 + ((INT8U)*p-FontStruct.FirstCode1);
//				nwbits = *(fontw_ptr) * rx;
				if(FontStruct.FixedWidthFlag1 == 1 || StrStruct.UseFixedWidth == 1) {
					nwbits = FontStruct.FixedWbits1 * rx;
				} else {
					fontw_ptr = FontStruct.PtrWidth1 + ((INT16U)*p-FontStruct.FirstCode1);
					nwbits = *(fontw_ptr) * rx;
				}
////////////////////////////
				//HYP 20050224
				//nhbits = hbits1;
				nhbits = FontStruct.Hbits1*ry;
			}
			if(PrtStruct.Mode == RECEIPT && (INT16U)*p == HT) {	////////htab
				ncx = StrStruct.HTab[htab_pos_count]; ////
			}
		}
		if((StrStruct.ChRot&MASK_DEG_90_270) && ((INT16U)chw.vietnam.left != 0x20)) {
			//HYP 20050224
			//if (MbaselineH < wbits) {
		    //	MbaselineH = wbits;
			//}
			if (MbaselineH < baselineH) {
				MbaselineH = baselineH;
			}
			if (MbaselineL < baselineL) {
				MbaselineL = baselineL;
			}			
		   	if (((INT16U)chw.vietnam.left != LF) && ((INT16U)chw.vietnam.left != '\0')) {
			   	if (((cx != 0) || ((INT16U)chw.vietnam.left != 0x20)) && (ctrl_ch == OFF)) {
			   		cx = cx + hbits; 
					cx = cx + StrStruct.InterChSp;
				}
				if ((INT16U)chw.vietnam.left == 0x20 || ctrl_ch == ON) {
					bits_sum = 0;
				} else {
					bits_sum = bits_sum + hbits + StrStruct.InterChSp;
				}
			}
			if((cx+nhbits > StrStruct.StrWidth) || (ncx > StrStruct.StrWidth) || ((INT16U)chw.vietnam.left == LF) || ((INT16U)chw.vietnam.left == '\0')) {/////htab
				if(PrtStruct.Mode != RECEIPT) {/////////htab
					if (wordwrap == ON) {
						if(((INT16U)chw.vietnam.left != 0x20) && ((INT16U)*p != 0x20) && ((INT16U)*p != HT) && ((INT16U)chw.vietnam.left != LF) && ((INT16U)chw.vietnam.left != '\0') && (cx != bits_sum)) {////htab
							cx = cx - bits_sum ;
							p = p - p_sum;
							if (cx != 0) {
								if (*(p-1) == 0x20){   ///htab
									cx = cx - sbits - 2*StrStruct.InterChSp;
									p--;
								} else {
									cx = cx - StrStruct.InterChSp;
								}
							}
					 	} else {
							if (cx != 0) {
								if ((INT16U)chw.vietnam.left == 0x20){
									cx = cx - sbits - 2*StrStruct.InterChSp;
									p--;
								} else { 
									cx = cx - StrStruct.InterChSp;
								}
							}
						}
					} else {
						if (cx != 0) {
							if ((INT16U)chw.vietnam.left == 0x20){
								cx = cx - sbits - 2*StrStruct.InterChSp;
								p--;
							} else {
								cx = cx - StrStruct.InterChSp;
							}
						}
					}
					p_sum = 0;
					bits_sum = 0;
					by[line_idx] = cy + MbaselineH;
					line_height = MbaselineH + MbaselineL;
					cy = cy + line_height;
					max_line = line_idx;
					if(cy > StrStruct.StrHeight) {
						PrtStruct.StrAreaOverFlag = TRUE;//HYP 20040903	not applied to PC program (PrtRender)
						whiteSpace = 0;
						loop = FALSE;
					} else {
						whiteSpace = StrStruct.StrWidth - cx;
						//HYP 20040903
						//if(*p == '\0') {
						if(((INT16U)*p == '\0') || ((INT16U)chw.vietnam.left == '\0')) {
							loop=FALSE;
							max_line++;
						}
					}
					cy = cy + StrStruct.LineSp;
					MbaselineH = 0;
					MbaselineL = 0;
					cx = 0;
				} else {  //////PrtStruct.Mode == RECEIPT
					by[line_idx] = cy + MbaselineH;
					PrtStruct.ReceiptLineHeight = MbaselineH + MbaselineL;
					if(PrtStruct.ReceiptLineHeight <= StrStruct.StrHeight) {
						if(max_cx < cx) max_cx = cx;
						whiteSpace = StrStruct.StrWidth - max_cx + StrStruct.InterChSp;
						max_line = 1;
						p_max = p;	////////
					} else {
						max_line = 0;
						PrtStruct.ReceiptLineHeight = 0;
					}				
					loop = FALSE;
				}
				line_end=TRUE;
			}
		} else {
			if (MbaselineH < baselineH) {
				MbaselineH = baselineH;
			}
			if (MbaselineL < baselineL) {
				MbaselineL = baselineL;
			}
			if (((INT16U)chw.vietnam.left != LF) && ((INT16U)chw.vietnam.left != '\0')) {//gm 081020
				if (((cx != 0) || ((INT16U)chw.vietnam.left != 0x20)) && (ctrl_ch == OFF)) {//gm 081020
					cx = cx + wbits; 
					cx = cx + StrStruct.InterChSp;
				}
				if ((INT16U)chw.vietnam.left == 0x20 || ctrl_ch == ON) {
					bits_sum = 0;
				} else {
					bits_sum = bits_sum + wbits + StrStruct.InterChSp;
				}
			}
			if((cx+nwbits > StrStruct.StrWidth) || (ncx > StrStruct.StrWidth) || ((INT16U)chw.vietnam.left == LF) || ((INT16U)chw.vietnam.left == '\0')) {/////htab
				if(PrtStruct.Mode != RECEIPT) {/////////htab
					if (wordwrap == ON) {
						if(((INT16U)chw.vietnam.left != 0x20) && ((INT16U)*p != 0x20) && ((INT16U)*p != HT) && ((INT16U)chw.vietnam.left != LF) && ((INT16U)chw.vietnam.left != '\0') && (cx != bits_sum)) {///htab
							cx = cx - bits_sum ;
							p = p - p_sum;				   
							if (cx != 0) {
								if ((INT16U)*(p-1) == 0x20){
									cx = cx - sbits - 2*StrStruct.InterChSp;
									p--;
								} else {
									cx = cx - StrStruct.InterChSp;
								}
							}
					 	} else {
							if (cx != 0) {
								if (chw.vietnam.left == 0x20){
									cx = cx - sbits - 2*StrStruct.InterChSp;
									p--;
								} else { 
									cx = cx - StrStruct.InterChSp;
								}
							}
						}
					} else {
						if (cx != 0) {
							if (chw.vietnam.left == 0x20) {
								cx = cx - sbits - 2*StrStruct.InterChSp;
								p--;
							} else { 
								cx = cx - StrStruct.InterChSp;
							}
						}
					}
					p_sum = 0;
					bits_sum = 0;
					by[line_idx] = cy + MbaselineH;
					line_height = MbaselineH + MbaselineL;
					cy = cy + line_height;
					max_line = line_idx;
					if(cy > StrStruct.StrHeight) {
						if(StrStruct.AutoShrinkx == ON) {	//auto shrink mode
							if(StrStruct.Magx != 1) {
								variable_init_flag = 1;		//for auto shrink
								loop = TRUE;
								StrStruct.Magx--;
							} else {
								PrtStruct.StrAreaOverFlag = TRUE;//HYP 20040903	not applied to PC program (PrtRender)
								whiteSpace = 0;
								loop = FALSE;
								cy = cy - StrStruct.LineSp - StrStruct.LineSp - line_height;//for vertical centering
						    }
						} else {
							PrtStruct.StrAreaOverFlag = TRUE;//HYP 20040903	not applied to PC program (PrtRender)
							whiteSpace = 0;
							loop = FALSE;
							cy = cy - StrStruct.LineSp - StrStruct.LineSp - line_height;//for vertical centering
						}
					} else {
						whiteSpace = StrStruct.StrWidth - cx;	  
						//HYP 20040903
						//if(*p == '\0') 
						if(((INT16U)*p == '\0') || ((INT16U)chw.vietnam.left == '\0')){
							loop=FALSE;
							max_line++;
						}
					}
					cy = cy + StrStruct.LineSp;
					MbaselineH = 0;
					MbaselineL = 0;
					cx = 0;
				} else { //PrtStruct.Mode == RECEIPT
					by[line_idx] = cy + MbaselineH;
					PrtStruct.ReceiptLineHeight =  MbaselineH + MbaselineL;
					if(PrtStruct.ReceiptLineHeight <= StrStruct.StrHeight) {
						if(max_cx < cx) max_cx = cx;
						whiteSpace = StrStruct.StrWidth - max_cx + StrStruct.InterChSp;
						max_line = 1;
						p_max = p;	////////
					} else {
						max_line = 0;
						PrtStruct.ReceiptLineHeight = 0;
					}
					loop = FALSE;
				}
				line_end=TRUE;
			}
		}
		if(PrtStruct.Mode == LABEL)	//VCentering은 RECIEPT인 경우에는 사용하지 않음.R_Interprerter없음.
		{
			if((StrStruct.VCentering == ON) && (loop==FALSE)) {	  //Vertical Centering ,change StrStruct.StrHeight
			whitespace_V = StrStruct.StrHeight - cy;
			if (whitespace_V > 0) {
				whitespace_V = 	whitespace_V>>1;
				switch(PrtStruct.Rot) {
					case DEG_0: 
						sy = sy + whitespace_V;
						StrStruct.StrHeight = cy;
						break;
					case DEG_90:
						sx = sx + whitespace_V;
						StrStruct.StrHeight = cy;
						break;
					case DEG_180: 
						sy = sy + whitespace_V;
						StrStruct.StrHeight = cy;
						break;
					case DEG_270:
						sx = sx + whitespace_V;
						StrStruct.StrHeight = cy;
						break;
					}
				}
			}
		}
		if(line_end==TRUE) {
			line_end = FALSE;
			switch (justification) {		// apply justification control
				case CENTER:
					str_left_margin[line_idx] = whiteSpace / 2;
					str_right_margin[line_idx] = whiteSpace / 2;
					break;
				case RIGHT:
					//HYP 20040226///////////////
					if(PrtStruct.Mode == RECEIPT) {
						if(StrStruct.Typeface & ITALIC) {	
							whiteSpace = whiteSpace - ((line_height/StrStruct.Magy)/6)*StrStruct.Magx; 
						} 
						if(StrStruct.Typeface & BOLD) {	
							whiteSpace = whiteSpace - StrStruct.Magx;
						}
						if(StrStruct.Typeface & OUTLINE) {	
							whiteSpace = whiteSpace - StrStruct.Magx;
						}
						if(whiteSpace < 0) whiteSpace = 0; 
					}
					//////////////////////////////
					str_left_margin[line_idx] = whiteSpace;
					str_right_margin[line_idx] = 0;
					break;
				case LEFT:
				default:	// to Left
					str_left_margin[line_idx] = 0;
					str_right_margin[line_idx] = whiteSpace;
					break;
			}
			line_idx++;
		}
	}
	//start rendering

	StrStruct.ChRot = ch_rot_backup;  /////////rot
	FontLoadFont1(id1_backup);
	FontLoadFont2(id2_backup);

	baseline2 = FontStruct.Baseline2*StrStruct.Magy;
	baseline1 = FontStruct.Baseline1*StrStruct.Magy;
	wbits2 = FontStruct.Wbits2*StrStruct.Magx;	  	// mag. width bits of 2 bytes font
	hbits2 = FontStruct.Hbits2*StrStruct.Magy;		// mag. height bits of 2 bytes font
	wbits1 = FontStruct.Wbits1*StrStruct.Magx;		// mag. width bits of 1 bytes font
	hbits1 = FontStruct.Hbits1*StrStruct.Magy;		// mag. height bits of 1 bytes font

	wbits = wbits1;		// mag. width inc. bits of 1 str line
	hbits = hbits1;		// mag. height inc. bits of 1 str line
	nwbits = wbits1;		// mag. width inc. bits of 1 str line
	nhbits = hbits1;		// mag. height inc. bits of 1 str line
//	baseline_height = 0;//HYP 20050224 erase baseline_height
	p = VietnamCode_buf;
	position_counter = 0;
	cx = str_left_margin[0];
		
	// rel. character pos x
	cy = 0;
	loop = TRUE;
	line_idx = 0;
	htab_pos_count = 0;	 ////////htab
	if(line_idx >= max_line) {
		loop = FALSE;
	}

	while (loop) {
		ctrl_ch = OFF;
		chw.vietnam.left = p[position_counter];
		if((INT16U)chw.vietnam.left == _CR) {
			if((INT16U)p[position_counter+1] == LF) {
				position_counter++;
			}
			chw.vietnam.left = LF;
		}
		if (StrStruct.StrType == TWO_BYTE_FONT_STR && (INT16U)chw.vietnam.left > 0x80) {
			if(StrStruct.ChRot & MASK_DEG_90_270) {	//chw.byte.left != 0x20
				cy = by[line_idx] - wbits2;
			} else {
				cy = by[line_idx] - hbits2 + baseline2;
			}
			chw.vietnam.right = p[position_counter+1];
			rot = PrtStruct.Rot + StrStruct.ChRot; //chw.byte.left != 0x20
	    	switch(PrtStruct.Rot) {
				case DEG_0:
					if(StrStruct.StrMirx) {
						crx = sx + StrStruct.StrWidth - cx;
						cry = sy + cy;
						if(rot&MASK_DEG_90_270){
							crx = crx - hbits2;
						} else {
							crx = crx - wbits2;
						}	
					} else {
						crx = sx + cx;
						cry = sy + cy;
					}
					PrtStruct.x = crx;
					PrtStruct.y = cry;
					FontDCharOut((INT16U)chw.words,rot);
					break;
				case DEG_90:
					if(StrStruct.StrMirx) {
						crx = sx + StrStruct.StrHeight - cy;
						cry = sy + StrStruct.StrWidth - cx;
						if(rot&MASK_DEG_90_270){
							crx = crx - hbits2;
							cry = cry - wbits2;
						} else {
							crx = crx - wbits2;
							cry = cry - hbits2;
						}					
					} else {
						crx = sx + StrStruct.StrHeight - cy;
						cry = sy + cx;
						if(rot&MASK_DEG_90_270){
							crx = crx - hbits2;
						} else {
							crx = crx - wbits2;
						}
					}
					PrtStruct.x = crx;
					PrtStruct.y = cry;
					FontDCharOut((INT16U)chw.words,rot);
					break;
				case DEG_180:
					if(StrStruct.StrMirx) {
						crx = sx + cx;
						cry = sy + StrStruct.StrHeight - cy;
						if(rot&MASK_DEG_90_270){
							cry = cry - wbits2;
						} else {
							cry = cry - hbits2;
						}
					} else {
						crx = sx + StrStruct.StrWidth - cx;
						cry = sy + StrStruct.StrHeight - cy;
						if(rot&MASK_DEG_90_270){
							cry = cry - wbits2;
							crx = crx - hbits2;
						} else {
							cry = cry - hbits2;
							crx = crx - wbits2;
						}
					}
					PrtStruct.x = crx;
					PrtStruct.y = cry;
					FontDCharOut((INT16U)chw.words,rot);
					break;
				case DEG_270:
					if(StrStruct.StrMirx) {
						crx = sx + cy;
						cry = sy + cx;					
					} else {
						crx = sx + cy;
						cry = sy + StrStruct.StrWidth - cx;
						if(rot&MASK_DEG_90_270){
							cry = cry - wbits2;
						} else {
							cry = cry - hbits2;
						}
					}
					PrtStruct.x = crx;
					PrtStruct.y = cry;						
					FontDCharOut((INT16U)chw.words,rot);
					break;
			}				
			position_counter = position_counter + 2;
			wbits = wbits2;		
			hbits = hbits2;
		} else {
			position_counter++;
			if ((INT16U)chw.vietnam.left == FONT_CON_CH) {
				ctrl_ch = ON;
				switch (p[position_counter]) {
					/////////rot
					case FONT_CON_DEG:
						position_counter++;
						switch(p[position_counter]) {
							case '0':
								StrStruct.ChRot = DEG_0;
								break;
							case '1':
								StrStruct.ChRot = DEG_90;
								break;
							case '2':
								StrStruct.ChRot = DEG_180;
								break;
							case '3':
								StrStruct.ChRot = DEG_270;
								break;
						}
						break;
					//Typeface
					case FONT_CON_TYPE_ITALIC:
						StrStruct.Typeface |= ITALIC;
						break;
					case FONT_CON_TYPE_UNDERLINE:
						StrStruct.Typeface |= UNDERLINE;
						break;
					case FONT_CON_TYPE_THROUGH_LINE:
						StrStruct.Typeface |= THROUGH_LINE;
						break;
					case FONT_CON_TYPE_BOLD:
						StrStruct.Typeface |= BOLD;
						break;
					case FONT_CON_TYPE_DTHROUGH_LINE:
						StrStruct.Typeface |= DTHROUGH_LINE;
						break;
					case FONT_CON_TYPE_OUTLINE:
						StrStruct.Typeface |= OUTLINE;
						break;
					case FONT_CON_TYPE_NO_TYPEFACE:
						StrStruct.Typeface = NO_TYPEFACE;
						break;
					case FONT_CON_MAG_X:
						position_counter++;
						switch(p[position_counter]) {
							case '1':
								StrStruct.Magx = 1;
								break;
							case '2':
								StrStruct.Magx = 2;
								break;
							case '3':
								StrStruct.Magx = 3;
								break;
							case '4':
								StrStruct.Magx = 4;
								break;
							case '5':
								StrStruct.Magx = 5;
								break;
						}
						break;
					case FONT_CON_MAG_Y:
						position_counter++;
						switch(p[position_counter]) {
							case '1':
								StrStruct.Magy = 1;
								break;
							case '2':
								StrStruct.Magy = 2;
								break;
							case '3':
								StrStruct.Magy = 3;
								break;
							case '4':
								StrStruct.Magy = 4;
								break;
							case '5':
								StrStruct.Magy = 5;
								break;
						}
						break;
					case FONT_CON_INC_MAG:
						if(StrStruct.Magx < 5) {
							StrStruct.Magx++;
						}
						if(StrStruct.Magy < 5) {
							StrStruct.Magy++;
						}
						break;
					case FONT_CON_DEC_MAG:
						if(StrStruct.Magx > 1) {
							StrStruct.Magx--;
						}
						if(StrStruct.Magy > 1) {
							StrStruct.Magy--;
						}
						break;
					case FONT_CON_FONT2:
						position_counter++;
						FontLoadFont2(p[position_counter] - '0');
						break;
					case FONT_CON_FONT1:
						position_counter++;
						FontLoadFont1(p[position_counter] - '0');
						break;
				}
				position_counter++;
				baseline2 = FontStruct.Baseline2*StrStruct.Magy;
				baseline1 = FontStruct.Baseline1*StrStruct.Magy;
				wbits2 = FontStruct.Wbits2*StrStruct.Magx;	  	// mag. width bits of 2 bytes font
				hbits2 = FontStruct.Hbits2*StrStruct.Magy;		// mag. height bits of 2 bytes font
				wbits1 = FontStruct.Wbits1*StrStruct.Magx;		// mag. width bits of 1 bytes font
				hbits1 = FontStruct.Hbits1*StrStruct.Magy;		// mag. height bits of 1 bytes font
			}
			if((INT16U)chw.vietnam.left < 0x20) {
				if (StrStruct.ChRot&MASK_DEG_90_270) {
					hbits1 = 0;
				} else {
					wbits1 = 0;
				}				
				ctrl_ch = ON;  
			} else {
//Insert Fixed Font 20031218
				if(FontStruct.FixedWidthFlag1 == 1 || StrStruct.UseFixedWidth == 1) {
					wbits1 = FontStruct.FixedWbits1 * StrStruct.Magx;
				} else {
					fontw_ptr = FontStruct.PtrWidth1 + ((INT16U)chw.vietnam.left-FontStruct.FirstCode1);
					wbits1 = *(fontw_ptr) * StrStruct.Magx;
				}
				hbits1=FontStruct.Hbits1*StrStruct.Magy;				
////////////////////////////
			}
			if((StrStruct.ChRot & MASK_DEG_90_270) && ((INT16U)chw.vietnam.left != 0x20)) {
				cy = by[line_idx] - wbits1;
			} else {
				cy = by[line_idx] - hbits1 + baseline1;
			}
			if(PrtStruct.Mode == RECEIPT && (INT16U)chw.vietnam.left == HT) {	////////htab
				cx = str_left_margin[line_idx] + StrStruct.HTab[htab_pos_count]; ///htab
				htab_pos_count++;
			}
 			if(((INT16U)chw.vietnam.left != LF) && ((cx != str_left_margin[line_idx]) || ((INT16U)chw.vietnam.left != 0x20)) && (ctrl_ch == OFF)) {
				if((StrStruct.ChRot & MASK_DEG_90_270) && ((INT16U)chw.vietnam.left == 0x20)) {
					rot = PrtStruct.Rot;
				} else {
					rot = PrtStruct.Rot + StrStruct.ChRot;
				}
 			   	switch(PrtStruct.Rot) {
					case DEG_0:
						if(StrStruct.StrMirx) {
							crx = sx + StrStruct.StrWidth - cx;
							cry = sy + cy;
							if(rot&MASK_DEG_90_270){
								crx = crx - hbits1;
							} else {
								crx = crx - wbits1;
							}	
						} else {
							crx = sx + cx;
							cry = sy + cy;
						}
						PrtStruct.x = crx;
						PrtStruct.y = cry;
						FontCharOut(chw.vietnam.left, rot);
						break;
					case DEG_90:
						if(StrStruct.StrMirx) {
							crx = sx + StrStruct.StrHeight - cy;
							cry = sy + StrStruct.StrWidth - cx;
							if(rot&MASK_DEG_90_270){
								crx = crx - hbits1;
								cry = cry - wbits1;
							} else {
								crx = crx - wbits1;
								cry = cry - hbits1;
							}					
						} else {
							crx = sx + StrStruct.StrHeight - cy;
							cry = sy + cx;
							if(rot&MASK_DEG_90_270){
								crx = crx - hbits1;
							} else {
								crx = crx - wbits1;
							}
						}
						PrtStruct.x = crx;
						PrtStruct.y = cry;
						FontCharOut(chw.vietnam.left, rot);
						break;
					case DEG_180:
						if(StrStruct.StrMirx) {
							crx = sx + cx;
							cry = sy + StrStruct.StrHeight - cy;
							if(rot&MASK_DEG_90_270){
								cry = cry - wbits1;
							} else {
								cry = cry - hbits1;
							}
						} else {
							crx = sx + StrStruct.StrWidth - cx;
							cry = sy + StrStruct.StrHeight - cy;
							if(rot&MASK_DEG_90_270){
								cry = cry - wbits1;
								crx = crx - hbits1;
							} else {
								cry = cry - hbits1;
								crx = crx - wbits1;
							}
						}
						PrtStruct.x = crx;
						PrtStruct.y = cry;
						FontCharOut(chw.vietnam.left, rot);
						break;
					case DEG_270:
						if(StrStruct.StrMirx) {
							crx = sx + cy;
							cry = sy + cx;					
						} else {
							crx = sx + cy;
							cry = sy + StrStruct.StrWidth - cx;
							if(rot&MASK_DEG_90_270){
								cry = cry - wbits1;
							} else {
								cry = cry - hbits1;
							}
						}
						PrtStruct.x = crx;
						PrtStruct.y = cry;						
						FontCharOut(chw.vietnam.left, rot);
						break;
				}
			}
			wbits = wbits1;		
			hbits = hbits1;
		}
		if (StrStruct.StrType == TWO_BYTE_FONT_STR && (INT16U)(p[position_counter]) > 0x80) {
			nwbits = wbits2;		
			nhbits = hbits2;
		} else {
			if((INT16U)*p < 0x20) {//FONT_CON_CH < 0x20
				nwbits = 0;
				nhbits = 0;///////rot
			} else {
//Insert Fixed Font 20031218
				if(FontStruct.FixedWidthFlag1 == 1 || StrStruct.UseFixedWidth == 1) {
					nwbits = FontStruct.FixedWbits1 * StrStruct.Magx;
				} else {
					fontw_ptr = FontStruct.PtrWidth1 + ((INT16U)*p-FontStruct.FirstCode1);
					nwbits = *(fontw_ptr) * StrStruct.Magx;
				}
//////////////////////////// 
				nhbits=FontStruct.Hbits1*StrStruct.Magy;				
			}
		}
		// increase rel ch pos include ch rot
			
		if((StrStruct.ChRot&MASK_DEG_90_270) && ((INT16U)chw.vietnam.left != 0x20)) {
//HYP 20040807 	//if interchcp==0, 1 char out before LF.
	    	if ((cx != str_left_margin[line_idx]) || ((INT16U)chw.vietnam.left != 0x20)) {///htab
 				if(ctrl_ch == OFF) {
					cx = cx + hbits; 
					cx = cx + StrStruct.InterChSp;
				}
				if((cx + nhbits) > (StrStruct.StrWidth - str_right_margin[line_idx]) && (PrtStruct.Mode != RECEIPT || &p[position_counter] >= p_max)) {/////htab
					line_idx++;
					cx = str_left_margin[line_idx];
					while ((cx + nhbits) > (StrStruct.StrWidth - str_right_margin[line_idx])) {
						line_idx++;
						if(line_idx >= max_line) {
							loop = FALSE;
							break;
						}
						cx = str_left_margin[line_idx];
					}
					if(line_idx >= max_line) {
						loop = FALSE;
					}
				} else {
					if (((PrtStruct.Opaque == ON) || (StrStruct.Typeface != NO_TYPEFACE))) {
						if ((cx < (StrStruct.StrWidth - str_right_margin[line_idx])) && (StrStruct.InterChSp != 0)) {
							sphbits = wbits/StrStruct.Magy;
							for ( i = 0; i < (spbytes * sphbits); i++) {
								PrtStruct.PrtScratchPad[i] = 0x00;
							}
							Font_Typeface(PrtStruct.PrtScratchPad,spbytes,StrStruct.InterChSp,sphbits,DEG_0);
 					   		switch(PrtStruct.Rot + StrStruct.StrMirx * 2) {
								case DEG_0:
								case DEG_360:
									PrtStruct.x = crx + hbits;
									PrtStruct.y = cry;
									break;
								case DEG_90:
								case DEG_450:
									PrtStruct.x = crx;
									PrtStruct.y = cry + hbits;
									break;
								case DEG_180:
								case DEG_540:
									PrtStruct.x = crx - StrStruct.InterChSp;
									PrtStruct.y = cry;
									break;
								case DEG_270:
								case DEG_630:
									PrtStruct.x = crx;
									PrtStruct.y = cry - StrStruct.InterChSp;
									break;
							}
							rx = StrStruct.Magx; //backup
							StrStruct.Magx = 1;
				   	 		PrtPattern(PrtStruct.PrtScratchPad,spbytes,StrStruct.InterChSp,sphbits,PrtStruct.Rot);
							StrStruct.Magx = rx;	
	  					}
					}
				}
			}
		} else {
			if ((cx != str_left_margin[line_idx]) || ((INT16U)chw.vietnam.left != 0x20)) {//////htab
				if(ctrl_ch == OFF) {
					cx = cx + wbits;
					cx = cx + StrStruct.InterChSp;
				}
				if((cx + nwbits) > (StrStruct.StrWidth - str_right_margin[line_idx]) && (PrtStruct.Mode != RECEIPT || &p[position_counter] >= p_max))  {//////htab
					line_idx++;
					cx = str_left_margin[line_idx];
					while ((cx + nwbits) > (StrStruct.StrWidth - str_right_margin[line_idx])) {
						line_idx++;
						if(line_idx >= max_line) {
							loop = FALSE;
							break;
						}
						cx = str_left_margin[line_idx];
					}
					if(line_idx >= max_line) {
						loop = FALSE;
					}
				} else {
					if (((PrtStruct.Opaque == ON) || (StrStruct.Typeface != NO_TYPEFACE))) {
						if ((cx < (StrStruct.StrWidth - str_right_margin[line_idx])) && (StrStruct.InterChSp != 0)) {
							sphbits = hbits/StrStruct.Magy;
							for ( i = 0; i < (spbytes * sphbits); i++) {
								PrtStruct.PrtScratchPad[i] = 0x00;   
							}
							Font_Typeface(PrtStruct.PrtScratchPad,spbytes,StrStruct.InterChSp,sphbits,DEG_0);
 			   				switch(PrtStruct.Rot + StrStruct.StrMirx * 2) {
								case DEG_0:
								case DEG_360:
									PrtStruct.x = crx + wbits;
									PrtStruct.y = cry;
									break;
								case DEG_90:
								case DEG_450:
									PrtStruct.x = crx;
									PrtStruct.y = cry + wbits;
									break;
								case DEG_180:
								case DEG_540:
									PrtStruct.x = crx - StrStruct.InterChSp;
									PrtStruct.y = cry;
									break;
								case DEG_270:
								case DEG_630:
									PrtStruct.x = crx;
									PrtStruct.y = cry - StrStruct.InterChSp;
									break;
							}
							rx = StrStruct.Magx; //backup
							StrStruct.Magx = 1;
				  	  		PrtPattern(PrtStruct.PrtScratchPad,spbytes,StrStruct.InterChSp,sphbits,PrtStruct.Rot);
							StrStruct.Magx = rx;	
						}
					}
				}
			}
		}
		if((INT16U)p[position_counter] == '\0' || position_counter == (calculatestrnum+1))
		{
			loop=FALSE;
		}
	}
	StrStruct.Magx = magx_backup;	 //for auto shrink
	PrtStruct.x = backup_x;
	PrtStruct.y = backup_y;
	PrtPattern_Str_Area_Typeface();
}
#elif defined(USE_THAI_FONT)
void  FontStrOut(HUGEDATA char *ch_ptr) 
{
	short cx,cy,crx,cry,bits_sum,i;
	short by[30],str_left_margin[30],str_right_margin[30],whiteSpace,whitespace_V;
	char rot,wbits1,hbits1,wbits2,hbits2,wbits,hbits,nwbits,nhbits,justification,sbits;
	char line_idx,line_end,loop,spbytes,rx,ry;
	CWD chw;
        INT32U chw_thai_left, chw_thai_right;
	INT32U *p, *p_max;
	INT16U position_counter;	
	INT16U strnumber;
	INT16U calculatestrnum;
	char wordwrap, p_sum, /*baseline_height,*/ctrl_ch,max_line;//HYP 20050224 erase baseline_height
	short MbaselineH,MbaselineL,baselineL,baselineH,baseline1,baseline2,line_height;
	short sx,sy;
	INT8U *fontw_ptr;
	char variable_init_flag, magx_backup;
	char ch_rot_backup;/////////rot
	INT8U	sphbits;
	short	ncx;////////htab
	INT8U	htab_pos_count;	////////htab
	short	max_cx;////////htab
	BOOLEAN updata_mag;
	INT8U id1_backup, id2_backup;
//	char string_buf[30];
	//20040209 HYP
	short backup_x,backup_y;
	backup_x = PrtStruct.x;
	backup_y = PrtStruct.y;

	variable_init_flag = 1;
	loop = TRUE;	
	magx_backup = StrStruct.Magx;	 //for auto shrink
	ch_rot_backup = StrStruct.ChRot;  /////////rot
	id1_backup = StrStruct.Id1;
	id2_backup = StrStruct.Id2;

	if(*ch_ptr == '\0') return;
	strnumber = strlen(ch_ptr);
	calculatestrnum = Cvt_ThaiPreview((INT8U*)ch_ptr, strnumber, strnumber-1, 0); //gm 081103
	while (loop) {
		if(variable_init_flag == 1) {
			variable_init_flag = 0;

			sx = PrtStruct.x;
			sy = PrtStruct.y;
			spbytes = StrStruct.InterChSp>>3;
			if(StrStruct.InterChSp&0x07) spbytes++;
			p = ThaiCode_buf;
			wordwrap = StrStruct.WordWrap;
			cx = 0;								// rel. character pos x
			cy = 0;								// rel. character pos y
			justification = StrStruct.Justification;
			if(StrStruct.StrMirx) { //mirror
				if(justification == RIGHT) {
					justification = LEFT;
				} else if(justification == LEFT) {
					justification = RIGHT;
				}
			}
//////////////////////////////////load font1,2 start
			rx = StrStruct.Magx;
			ry = StrStruct.Magy;	
			wbits2 = FontStruct.Wbits2*rx;	  	// mag. width bits of 2 bytes font
			hbits2 = FontStruct.Hbits2*ry;		// mag. height bits of 2 bytes font
			wbits1 = FontStruct.Wbits1*rx;		// mag. width bits of 1 bytes font
			hbits1 = FontStruct.Hbits1*ry;		// mag. height bits of 1 bytes font
			wbits = wbits1;		// mag. width inc. bits of 1 str line
			hbits = hbits1;		// mag. height inc. bits of 1 str line
			nwbits = wbits1;		// mag. width inc. bits of 1 str line
			nhbits = hbits1;		// mag. height inc. bits of 1 str line
			baseline2 = FontStruct.Baseline2*ry;
			baseline1 = FontStruct.Baseline1*ry;
//Insert Fixed Font 20031218
//			fontw_ptr = FontStruct.PtrWidth1 + ((INT8U)0x20-FontStruct.FirstCode1);
//			sbits = *(fontw_ptr) * rx;
			if(FontStruct.FixedWidthFlag1 == 1 || StrStruct.UseFixedWidth == 1) {
				sbits = FontStruct.FixedWbits1 * rx;
			} else {
				fontw_ptr = FontStruct.PtrWidth1 + ((INT16U)0x20-FontStruct.FirstCode1);
				sbits = *(fontw_ptr) * rx;
			}
////////////////////////////
//////////////////////////////////load font1,2 end
			str_left_margin[0] = 0;
			str_right_margin[0]=0;
			line_idx = 0;
			htab_pos_count = 0;	 ////////htab
			max_cx = 0;	 ////////htab
			ncx = 0;  ///////htab
			line_end = FALSE;
			by[0] = 0;
			baselineL = 0;
			baselineH = 0;

			p_sum = 0;
			bits_sum = 0;
			MbaselineH = 0;
			MbaselineL = 0;
			line_height = 0;
		}	/*  Init end  */

 //justification and baseline check
		ctrl_ch = OFF;
		chw_thai_left = *p;
		chw_thai_right = *(p+1);
                chw.byte.left = (INT8U)*p;
                chw.byte.right = (INT8U)*(p+1);
 		if((INT16U)chw_thai_left == _CR) {
			if(*(p+1) == LF) {
				p++;
			}
			chw_thai_left = LF;
		}
		if (StrStruct.StrType == TWO_BYTE_FONT_STR && (INT16U)chw_thai_left > 0x80) {
			p = p + 2;
			wbits = wbits2;		
			hbits = hbits2;
			baselineL = baseline2;
			baselineH = hbits2 - baseline2;
			p_sum = p_sum + 2;
		} else {
			p++;
			if ((INT16U)chw_thai_left == FONT_CON_CH) {
				ctrl_ch = ON;
				updata_mag = OFF;
				switch (*p) {
					///////////rot
					case FONT_CON_DEG:
						p++;
						switch(*p) {
							case '0':
								StrStruct.ChRot = DEG_0;
								break;
							case '1':
								StrStruct.ChRot = DEG_90;
								break;
							case '2':
								StrStruct.ChRot = DEG_180;
								break;
							case '3':
								StrStruct.ChRot = DEG_270;
								break;
						}
						break;
					case FONT_CON_MAG_X:
						p++;
						switch(*p) {
							case '1':
								rx = 1;
								break;
							case '2':
								rx = 2;
								break;
							case '3':
								rx = 3;
								break;
							case '4':
								rx = 4;
								break;
							case '5':
								rx = 5;
								break;
						}
						updata_mag = ON;
						break;
					case FONT_CON_MAG_Y:
						p++;
						switch(*p) {
							case '1':
								ry = 1;
								break;
							case '2':
								ry = 2;
								break;
							case '3':
								ry = 3;
								break;
							case '4':
								ry = 4;
								break;
							case '5':
								ry = 5;
								break;
						}
						updata_mag = ON;
						break;
					case FONT_CON_INC_MAG:
						if(rx < 5) {
							rx++;
						}
						if(ry < 5) {
							ry++;
						}
						updata_mag = ON;
						break;
					case FONT_CON_DEC_MAG:
						if(rx > 1) {
							rx--;
						}
						if(ry > 1) {
							ry--;
						}
						updata_mag = ON;
						break;
					case FONT_CON_FONT2:
						p++;
						FontLoadFont2(*p - '0');
						updata_mag = ON;
						break;
					case FONT_CON_FONT1:
						p++;
						FontLoadFont1(*p - '0');
						updata_mag = ON;
						break;
				//HYP 20040621
				//FONT_CON_CH > 0x20
				//	case '0':
				//		ctrl_ch = OFF;
				//		break;
				}
				if(updata_mag == ON) {
					baseline2 = FontStruct.Baseline2*ry;
					baseline1 = FontStruct.Baseline1*ry;
					wbits2 = FontStruct.Wbits2*rx;	  	// mag. width bits of 2 bytes font
					hbits2 = FontStruct.Hbits2*ry;		// mag. height bits of 2 bytes font
					wbits1 = FontStruct.Wbits1*rx;		// mag. width bits of 1 bytes font
					hbits1 = FontStruct.Hbits1*ry;		// mag. height bits of 1 bytes font
					fontw_ptr = FontStruct.PtrWidth1 + ((INT16U)0x20-FontStruct.FirstCode1);
				}
				p++;
			}
			//HYP 20071130
			//if((INT8U)chw.byte.left < FontStruct.FirstCode1) {
//			if((INT8U)chw_thai_left < 0x20) {
			if(0) {//wcm151110
				//HYP 20050224
				//wbits1 = 0;
				if (StrStruct.ChRot&MASK_DEG_90_270) {
					hbits1 = 0;
				} else {
					wbits1 = 0;
				}
				ctrl_ch = ON;  
			} else {
//Insert Fixed Font 20031218
//				fontw_ptr = FontStruct.PtrWidth1 + ((INT8U)chw.byte.left-FontStruct.FirstCode1);
//				wbits1 = *(fontw_ptr) * rx;
				if(FontStruct.FixedWidthFlag1 == 1 || StrStruct.UseFixedWidth == 1) {
					wbits1 = FontStruct.FixedWbits1 * rx;
				} else {
					fontw_ptr = FontStruct.PtrWidth1 + ((INT16U)chw_thai_left-FontStruct.FirstCode1);
					wbits1 = *(fontw_ptr) * rx;
				}
				//HYP 20050224
				hbits1= FontStruct.Hbits1*ry;
////////////////////////////
			}
			wbits = wbits1;		
			hbits = hbits1;
			//HYP 20050224
			//baselineH = hbits1 - baseline1;
			//baselineL = baseline1;
			if (StrStruct.ChRot&MASK_DEG_90_270) {
				baselineH = wbits1;
				baselineL = 0;
			} else {
				baselineH = hbits1 - baseline1;
				baselineL = baseline1;
			}
			if((INT16U)chw_thai_left != 0x20) {
//				if ((ctrl_ch == ON) || (chw.byte.left == FONT_CON_CH)) {
//					p_sum = 0;	  //////////////////
//				} else {
//					p_sum = p_sum + 1;
//				}
				//HYP 20040128
				if ((ctrl_ch == ON) || ((INT16U)chw_thai_left == FONT_CON_CH)) {
//HYP 20040621
					//FONT_CON_CH > 0x20
//					if(chw.byte.right == '0') {
//						p_sum = p_sum + 2;
//					} else {
//						p_sum = 0;
//					}
					p_sum = 0; //FONT_CON_CH < 0x20
				} else {
					p_sum = p_sum + 1;
				}
				if(PrtStruct.Mode == RECEIPT && (INT16U)chw_thai_left == HT) {////////htab
					if(max_cx < cx) max_cx = cx;
					cx = StrStruct.HTab[htab_pos_count]; ///htab 
					htab_pos_count++;
					if(max_cx < cx) max_cx = cx;
				}
			} else {
//Insert Fixed Font 20031218
//				sbits = *(fontw_ptr) * rx;/////////////////////////
				if(FontStruct.FixedWidthFlag1 == 1 || StrStruct.UseFixedWidth == 1) {
					sbits = FontStruct.FixedWbits1 * rx;
				} else {
					sbits = *(fontw_ptr) * rx;/////////////////////////
				}
////////////////////////////
				p_sum = 0;
			}
		}
		if (StrStruct.StrType == TWO_BYTE_FONT_STR && (INT16U)(*p) > 0x80) {	//you must cast!!!
			nwbits = wbits2;		
			nhbits = hbits2;
		} else {
//			if(((INT8U)*p < FontStruct.FirstCode1) || ((INT8U)*p == FONT_CON_CH && (INT8U)*(p+1) != FONT_CON_CH)) {
//HYP 20040621
//			if(((INT8U)*p < FontStruct.FirstCode1) || ((INT8U)*p == FONT_CON_CH && (INT8U)*(p+1) != '0')) {//FONT_CON_CH > 0x20 //HYP 20040127
			//HYP 20071130
			//if((INT8U)*p < FontStruct.FirstCode1) {//FONT_CON_CH < 0x20
			if((INT16U)*p < 0x20) {//FONT_CON_CH < 0x20
				nwbits = 0;
				nhbits = 0; ///////rot
			} else {
//Insert Fixed Font 20031218
//				fontw_ptr = FontStruct.PtrWidth1 + ((INT8U)*p-FontStruct.FirstCode1);
//				nwbits = *(fontw_ptr) * rx;
				if(FontStruct.FixedWidthFlag1 == 1 || StrStruct.UseFixedWidth == 1) {
					nwbits = FontStruct.FixedWbits1 * rx;
				} else {
					fontw_ptr = FontStruct.PtrWidth1 + ((INT16U)*p-FontStruct.FirstCode1);
					nwbits = *(fontw_ptr) * rx;
				}
////////////////////////////
				//HYP 20050224
				//nhbits = hbits1;
				nhbits = FontStruct.Hbits1*ry;
			}
			if(PrtStruct.Mode == RECEIPT && (INT16U)*p == HT) {	////////htab
				ncx = StrStruct.HTab[htab_pos_count]; ////
			}
		}
		if((StrStruct.ChRot&MASK_DEG_90_270) && ((INT16U)chw_thai_left != 0x20)) {
			//HYP 20050224
			//if (MbaselineH < wbits) {
		    //	MbaselineH = wbits;
			//}
			if (MbaselineH < baselineH) {
				MbaselineH = baselineH;
			}
			if (MbaselineL < baselineL) {
				MbaselineL = baselineL;
			}			
		   	if (((INT16U)chw_thai_left != LF) && ((INT16U)chw_thai_left != '\0')) {
			   	if (((cx != 0) || ((INT16U)chw_thai_left != 0x20)) && (ctrl_ch == OFF)) {
			   		cx = cx + hbits; 
					cx = cx + StrStruct.InterChSp;
				}
				if ((INT16U)chw_thai_left == 0x20 || ctrl_ch == ON) {
					bits_sum = 0;
				} else {
					bits_sum = bits_sum + hbits + StrStruct.InterChSp;
				}
			}
			if((cx+nhbits > StrStruct.StrWidth) || (ncx > StrStruct.StrWidth) || ((INT16U)chw_thai_left == LF) || ((INT16U)chw_thai_left == '\0')) {/////htab
				if(PrtStruct.Mode != RECEIPT) {/////////htab
					if (wordwrap == ON) {
						if(((INT16U)chw_thai_left != 0x20) && ((INT16U)*p != 0x20) && ((INT16U)*p != HT) && ((INT16U)chw_thai_left != LF) && ((INT16U)chw_thai_left != '\0') && (cx != bits_sum)) {////htab
							cx = cx - bits_sum ;
							p = p - p_sum;
							if (cx != 0) {
								if (*(p-1) == 0x20){   ///htab
									cx = cx - sbits - 2*StrStruct.InterChSp;
									p--;
								} else {
									cx = cx - StrStruct.InterChSp;
								}
							}
					 	} else {
							if (cx != 0) {
								if ((INT16U)chw_thai_left == 0x20){
									cx = cx - sbits - 2*StrStruct.InterChSp;
									p--;
								} else { 
									cx = cx - StrStruct.InterChSp;
								}
							}
						}
					} else {
						if (cx != 0) {
							if ((INT16U)chw_thai_left == 0x20){
								cx = cx - sbits - 2*StrStruct.InterChSp;
								p--;
							} else {
								cx = cx - StrStruct.InterChSp;
							}
						}
					}
					p_sum = 0;
					bits_sum = 0;
					by[line_idx] = cy + MbaselineH;
					line_height = MbaselineH + MbaselineL;
					cy = cy + line_height;
					max_line = line_idx;
					if(cy > StrStruct.StrHeight) {
						PrtStruct.StrAreaOverFlag = TRUE;//HYP 20040903	not applied to PC program (PrtRender)
						whiteSpace = 0;
						loop = FALSE;
					} else {
						whiteSpace = StrStruct.StrWidth - cx;
						//HYP 20040903
						//if(*p == '\0') {
						if(((INT16U)*p == '\0') || ((INT16U)chw_thai_left == '\0')) {
							loop=FALSE;
							max_line++;
						}
					}
					cy = cy + StrStruct.LineSp;
					MbaselineH = 0;
					MbaselineL = 0;
					cx = 0;
				} else {  //////PrtStruct.Mode == RECEIPT
					by[line_idx] = cy + MbaselineH;
					PrtStruct.ReceiptLineHeight = MbaselineH + MbaselineL;
					if(PrtStruct.ReceiptLineHeight <= StrStruct.StrHeight) {
						if(max_cx < cx) max_cx = cx;
						whiteSpace = StrStruct.StrWidth - max_cx + StrStruct.InterChSp;
						max_line = 1;
						p_max = p;	////////
					} else {
						max_line = 0;
						PrtStruct.ReceiptLineHeight = 0;
					}				
					loop = FALSE;
				}
				line_end=TRUE;
			}
		} else {
			if (MbaselineH < baselineH) {
				MbaselineH = baselineH;
			}
			if (MbaselineL < baselineL) {
				MbaselineL = baselineL;
			}
			if (((INT16U)chw_thai_left != LF) && ((INT16U)chw_thai_left != '\0')) {//gm 081020
				if (((cx != 0) || ((INT16U)chw_thai_left != 0x20)) && (ctrl_ch == OFF)) {//gm 081020
					cx = cx + wbits; 
					cx = cx + StrStruct.InterChSp;
				}
				if ((INT16U)chw_thai_left == 0x20 || ctrl_ch == ON) {
					bits_sum = 0;
				} else {
					bits_sum = bits_sum + wbits + StrStruct.InterChSp;
				}
			}
			if((cx+nwbits > StrStruct.StrWidth) || (ncx > StrStruct.StrWidth) || ((INT16U)chw_thai_left == LF) || ((INT16U)chw_thai_left == '\0')) {/////htab
				if(PrtStruct.Mode != RECEIPT) {/////////htab
					if (wordwrap == ON) {
						if(((INT16U)chw_thai_left != 0x20) && ((INT16U)*p != 0x20) && ((INT16U)*p != HT) && ((INT16U)chw_thai_left != LF) && ((INT16U)chw_thai_left != '\0') && (cx != bits_sum)) {///htab
							cx = cx - bits_sum ;
							p = p - p_sum;				   
							if (cx != 0) {
								if ((INT16U)*(p-1) == 0x20){
									cx = cx - sbits - 2*StrStruct.InterChSp;
									p--;
								} else {
									cx = cx - StrStruct.InterChSp;
								}
							}
					 	} else {
							if (cx != 0) {
								if (chw_thai_left == 0x20){
									cx = cx - sbits - 2*StrStruct.InterChSp;
									p--;
								} else { 
									cx = cx - StrStruct.InterChSp;
								}
							}
						}
					} else {
						if (cx != 0) {
							if (chw_thai_left == 0x20) {
								cx = cx - sbits - 2*StrStruct.InterChSp;
								p--;
							} else { 
								cx = cx - StrStruct.InterChSp;
							}
						}
					}
					p_sum = 0;
					bits_sum = 0;
					by[line_idx] = cy + MbaselineH;
					line_height = MbaselineH + MbaselineL;
					cy = cy + line_height;
					max_line = line_idx;
					if(cy > StrStruct.StrHeight) {
						if(StrStruct.AutoShrinkx == ON) {	//auto shrink mode
							if(StrStruct.Magx != 1) {
								variable_init_flag = 1;		//for auto shrink
								loop = TRUE;
								StrStruct.Magx--;
							} else {
								PrtStruct.StrAreaOverFlag = TRUE;//HYP 20040903	not applied to PC program (PrtRender)
								whiteSpace = 0;
								loop = FALSE;
								cy = cy - StrStruct.LineSp - StrStruct.LineSp - line_height;//for vertical centering
						    }
						} else {
							PrtStruct.StrAreaOverFlag = TRUE;//HYP 20040903	not applied to PC program (PrtRender)
							whiteSpace = 0;
							loop = FALSE;
							cy = cy - StrStruct.LineSp - StrStruct.LineSp - line_height;//for vertical centering
						}
					} else {
						whiteSpace = StrStruct.StrWidth - cx;	  
						//HYP 20040903
						//if(*p == '\0') 
						if(((INT16U)*p == '\0') || ((INT16U)chw_thai_left == '\0')){
							loop=FALSE;
							max_line++;
						}
					}
					cy = cy + StrStruct.LineSp;
					MbaselineH = 0;
					MbaselineL = 0;
					cx = 0;
				} else { //PrtStruct.Mode == RECEIPT
					by[line_idx] = cy + MbaselineH;
					PrtStruct.ReceiptLineHeight =  MbaselineH + MbaselineL;
					if(PrtStruct.ReceiptLineHeight <= StrStruct.StrHeight) {
						if(max_cx < cx) max_cx = cx;
						whiteSpace = StrStruct.StrWidth - max_cx + StrStruct.InterChSp;
						max_line = 1;
						p_max = p;	////////
					} else {
						max_line = 0;
						PrtStruct.ReceiptLineHeight = 0;
					}
					loop = FALSE;
				}
				line_end=TRUE;
			}
		}
		if(PrtStruct.Mode == LABEL)	//VCentering은 RECIEPT인 경우에는 사용하지 않음.R_Interprerter없음.
		{
			if((StrStruct.VCentering == ON) && (loop==FALSE)) {	  //Vertical Centering ,change StrStruct.StrHeight
			whitespace_V = StrStruct.StrHeight - cy;
			if (whitespace_V > 0) {
				whitespace_V = 	whitespace_V>>1;
				switch(PrtStruct.Rot) {
					case DEG_0: 
						sy = sy + whitespace_V;
						StrStruct.StrHeight = cy;
						break;
					case DEG_90:
						sx = sx + whitespace_V;
						StrStruct.StrHeight = cy;
						break;
					case DEG_180: 
						sy = sy + whitespace_V;
						StrStruct.StrHeight = cy;
						break;
					case DEG_270:
						sx = sx + whitespace_V;
						StrStruct.StrHeight = cy;
						break;
					}
				}
			}
		}
		if(line_end==TRUE) {
			line_end = FALSE;
			switch (justification) {		// apply justification control
				case CENTER:
					str_left_margin[line_idx] = whiteSpace / 2;
					str_right_margin[line_idx] = whiteSpace / 2;
					break;
				case RIGHT:
					//HYP 20040226///////////////
					if(PrtStruct.Mode == RECEIPT) {
						if(StrStruct.Typeface & ITALIC) {	
							whiteSpace = whiteSpace - ((line_height/StrStruct.Magy)/6)*StrStruct.Magx; 
						} 
						if(StrStruct.Typeface & BOLD) {	
							whiteSpace = whiteSpace - StrStruct.Magx;
						}
						if(StrStruct.Typeface & OUTLINE) {	
							whiteSpace = whiteSpace - StrStruct.Magx;
						}
						if(whiteSpace < 0) whiteSpace = 0; 
					}
					//////////////////////////////
					str_left_margin[line_idx] = whiteSpace;
					str_right_margin[line_idx] = 0;
					break;
				case LEFT:
				default:	// to Left
					str_left_margin[line_idx] = 0;
					str_right_margin[line_idx] = whiteSpace;
					break;
			}
			line_idx++;
		}
	}
	//start rendering

	StrStruct.ChRot = ch_rot_backup;  /////////rot
	FontLoadFont1(id1_backup);
	FontLoadFont2(id2_backup);

	baseline2 = FontStruct.Baseline2*StrStruct.Magy;
	baseline1 = FontStruct.Baseline1*StrStruct.Magy;
	wbits2 = FontStruct.Wbits2*StrStruct.Magx;	  	// mag. width bits of 2 bytes font
	hbits2 = FontStruct.Hbits2*StrStruct.Magy;		// mag. height bits of 2 bytes font
	wbits1 = FontStruct.Wbits1*StrStruct.Magx;		// mag. width bits of 1 bytes font
	hbits1 = FontStruct.Hbits1*StrStruct.Magy;		// mag. height bits of 1 bytes font

	wbits = wbits1;		// mag. width inc. bits of 1 str line
	hbits = hbits1;		// mag. height inc. bits of 1 str line
	nwbits = wbits1;		// mag. width inc. bits of 1 str line
	nhbits = hbits1;		// mag. height inc. bits of 1 str line
//	baseline_height = 0;//HYP 20050224 erase baseline_height
	p = ThaiCode_buf;
	position_counter = 0;
	cx = str_left_margin[0];
		
	// rel. character pos x
	cy = 0;
	loop = TRUE;
	line_idx = 0;
	htab_pos_count = 0;	 ////////htab
	if(line_idx >= max_line) {
		loop = FALSE;
	}

	while (loop) {
		ctrl_ch = OFF;
		chw_thai_left = p[position_counter];
 		if((INT16U)chw_thai_left == _CR) {
			if((INT16U)p[position_counter+1] == LF) {
				position_counter++;
			}
			chw_thai_left = LF;
		}
		if (StrStruct.StrType == TWO_BYTE_FONT_STR && (INT16U)chw_thai_left > 0x80) {
			if(StrStruct.ChRot & MASK_DEG_90_270) {	//chw.byte.left != 0x20
				cy = by[line_idx] - wbits2;
			} else {
				cy = by[line_idx] - hbits2 + baseline2;
			}
			chw_thai_right = p[position_counter+1];			
			rot = PrtStruct.Rot + StrStruct.ChRot; //chw.byte.left != 0x20
	    	switch(PrtStruct.Rot) {
				case DEG_0:
					if(StrStruct.StrMirx) {
						crx = sx + StrStruct.StrWidth - cx;
						cry = sy + cy;
						if(rot&MASK_DEG_90_270){
							crx = crx - hbits2;
						} else {
							crx = crx - wbits2;
						}	
					} else {
						crx = sx + cx;
						cry = sy + cy;
					}
					PrtStruct.x = crx;
					PrtStruct.y = cry;
					FontDCharOut((INT16U)chw.word,rot);
					break;
				case DEG_90:
					if(StrStruct.StrMirx) {
						crx = sx + StrStruct.StrHeight - cy;
						cry = sy + StrStruct.StrWidth - cx;
						if(rot&MASK_DEG_90_270){
							crx = crx - hbits2;
							cry = cry - wbits2;
						} else {
							crx = crx - wbits2;
							cry = cry - hbits2;
						}					
					} else {
						crx = sx + StrStruct.StrHeight - cy;
						cry = sy + cx;
						if(rot&MASK_DEG_90_270){
							crx = crx - hbits2;
						} else {
							crx = crx - wbits2;
						}
					}
					PrtStruct.x = crx;
					PrtStruct.y = cry;
					FontDCharOut((INT16U)chw.word,rot);
					break;
				case DEG_180:
					if(StrStruct.StrMirx) {
						crx = sx + cx;
						cry = sy + StrStruct.StrHeight - cy;
						if(rot&MASK_DEG_90_270){
							cry = cry - wbits2;
						} else {
							cry = cry - hbits2;
						}
					} else {
						crx = sx + StrStruct.StrWidth - cx;
						cry = sy + StrStruct.StrHeight - cy;
						if(rot&MASK_DEG_90_270){
							cry = cry - wbits2;
							crx = crx - hbits2;
						} else {
							cry = cry - hbits2;
							crx = crx - wbits2;
						}
					}
					PrtStruct.x = crx;
					PrtStruct.y = cry;
					FontDCharOut((INT16U)chw.word,rot);
					break;
				case DEG_270:
					if(StrStruct.StrMirx) {
						crx = sx + cy;
						cry = sy + cx;					
					} else {
						crx = sx + cy;
						cry = sy + StrStruct.StrWidth - cx;
						if(rot&MASK_DEG_90_270){
							cry = cry - wbits2;
						} else {
							cry = cry - hbits2;
						}
					}
					PrtStruct.x = crx;
					PrtStruct.y = cry;						
					FontDCharOut((INT16U)chw.word,rot);
					break;
			}				
			position_counter = position_counter + 2;	       	
			wbits = wbits2;		
			hbits = hbits2;
		} else {
			position_counter++;
			if ((INT16U)chw_thai_left == FONT_CON_CH) {
				ctrl_ch = ON;
				switch (p[position_counter]) {
					/////////rot
					case FONT_CON_DEG:
						position_counter++;
						switch(p[position_counter]) {
							case '0':
								StrStruct.ChRot = DEG_0;
								break;
							case '1':
								StrStruct.ChRot = DEG_90;
								break;
							case '2':
								StrStruct.ChRot = DEG_180;
								break;
							case '3':
								StrStruct.ChRot = DEG_270;
								break;
						}
						break;
					//Typeface
					case FONT_CON_TYPE_ITALIC:
						StrStruct.Typeface |= ITALIC;
						break;
					case FONT_CON_TYPE_UNDERLINE:
						StrStruct.Typeface |= UNDERLINE;
						break;
					case FONT_CON_TYPE_THROUGH_LINE:
						StrStruct.Typeface |= THROUGH_LINE;
						break;
					case FONT_CON_TYPE_BOLD:
						StrStruct.Typeface |= BOLD;
						break;
					case FONT_CON_TYPE_DTHROUGH_LINE:
						StrStruct.Typeface |= DTHROUGH_LINE;
						break;
					case FONT_CON_TYPE_OUTLINE:
						StrStruct.Typeface |= OUTLINE;
						break;
					case FONT_CON_TYPE_NO_TYPEFACE:
						StrStruct.Typeface = NO_TYPEFACE;
						break;
					case FONT_CON_MAG_X:
						position_counter++;
						switch(p[position_counter]) {
							case '1':
								StrStruct.Magx = 1;
								break;
							case '2':
								StrStruct.Magx = 2;
								break;
							case '3':
								StrStruct.Magx = 3;
								break;
							case '4':
								StrStruct.Magx = 4;
								break;
							case '5':
								StrStruct.Magx = 5;
								break;
						}
						break;
					case FONT_CON_MAG_Y:
						position_counter++;
						switch(p[position_counter]) {
							case '1':
								StrStruct.Magy = 1;
								break;
							case '2':
								StrStruct.Magy = 2;
								break;
							case '3':
								StrStruct.Magy = 3;
								break;
							case '4':
								StrStruct.Magy = 4;
								break;
							case '5':
								StrStruct.Magy = 5;
								break;
						}
						break;
					case FONT_CON_INC_MAG:
						if(StrStruct.Magx < 5) {
							StrStruct.Magx++;
						}
						if(StrStruct.Magy < 5) {
							StrStruct.Magy++;
						}
						break;
					case FONT_CON_DEC_MAG:
						if(StrStruct.Magx > 1) {
							StrStruct.Magx--;
						}
						if(StrStruct.Magy > 1) {
							StrStruct.Magy--;
						}
						break;
					case FONT_CON_FONT2:
						position_counter++;
						FontLoadFont2(p[position_counter] - '0');
						break;
					case FONT_CON_FONT1:
						position_counter++;
						FontLoadFont1(p[position_counter] - '0');
						break;
				}
				position_counter++;
				baseline2 = FontStruct.Baseline2*StrStruct.Magy;
				baseline1 = FontStruct.Baseline1*StrStruct.Magy;
				wbits2 = FontStruct.Wbits2*StrStruct.Magx;	  	// mag. width bits of 2 bytes font
				hbits2 = FontStruct.Hbits2*StrStruct.Magy;		// mag. height bits of 2 bytes font
				wbits1 = FontStruct.Wbits1*StrStruct.Magx;		// mag. width bits of 1 bytes font
				hbits1 = FontStruct.Hbits1*StrStruct.Magy;		// mag. height bits of 1 bytes font
			}
			if((INT16U)chw_thai_left < 0x20) {
				if (StrStruct.ChRot&MASK_DEG_90_270) {
					hbits1 = 0;
				} else {
					wbits1 = 0;
				}				
				ctrl_ch = ON;  
			} else {
//Insert Fixed Font 20031218
				if(FontStruct.FixedWidthFlag1 == 1 || StrStruct.UseFixedWidth == 1) {
					wbits1 = FontStruct.FixedWbits1 * StrStruct.Magx;
				} else {
					fontw_ptr = FontStruct.PtrWidth1 + ((INT16U)chw_thai_left-FontStruct.FirstCode1);
					wbits1 = *(fontw_ptr) * StrStruct.Magx;
				}
				hbits1=FontStruct.Hbits1*StrStruct.Magy;				
			}
			if((StrStruct.ChRot & MASK_DEG_90_270) && ((INT16U)chw_thai_left != 0x20)) {
				cy = by[line_idx] - wbits1;
			} else {
				cy = by[line_idx] - hbits1 + baseline1;
			}
			if(PrtStruct.Mode == RECEIPT && (INT16U)chw_thai_left == HT) {	////////htab
				cx = str_left_margin[line_idx] + StrStruct.HTab[htab_pos_count]; ///htab
				htab_pos_count++;
			}
 			if(((INT16U)chw_thai_left != LF) && ((cx != str_left_margin[line_idx]) || ((INT16U)chw_thai_left != 0x20)) && (ctrl_ch == OFF)) {
				if((StrStruct.ChRot & MASK_DEG_90_270) && ((INT16U)chw_thai_left == 0x20)) {
					rot = PrtStruct.Rot;
				} else {
					rot = PrtStruct.Rot + StrStruct.ChRot;
				}
 			   	switch(PrtStruct.Rot) {
					case DEG_0:
						if(StrStruct.StrMirx) {
							crx = sx + StrStruct.StrWidth - cx;
							cry = sy + cy;
							if(rot&MASK_DEG_90_270){
								crx = crx - hbits1;
							} else {
								crx = crx - wbits1;
							}	
						} else {
							crx = sx + cx;
							cry = sy + cy;
						}
						PrtStruct.x = crx;
						PrtStruct.y = cry;
						FontThaiCharOut(chw_thai_left, rot);
						break;
					case DEG_90:
						if(StrStruct.StrMirx) {
							crx = sx + StrStruct.StrHeight - cy;
							cry = sy + StrStruct.StrWidth - cx;
							if(rot&MASK_DEG_90_270){
								crx = crx - hbits1;
								cry = cry - wbits1;
							} else {
								crx = crx - wbits1;
								cry = cry - hbits1;
							}					
						} else {
							crx = sx + StrStruct.StrHeight - cy;
							cry = sy + cx;
							if(rot&MASK_DEG_90_270){
								crx = crx - hbits1;
							} else {
								crx = crx - wbits1;
							}
						}
						PrtStruct.x = crx;
						PrtStruct.y = cry;
						FontThaiCharOut(chw_thai_left, rot);
						break;
					case DEG_180:
						if(StrStruct.StrMirx) {
							crx = sx + cx;
							cry = sy + StrStruct.StrHeight - cy;
							if(rot&MASK_DEG_90_270){
								cry = cry - wbits1;
							} else {
								cry = cry - hbits1;
							}
						} else {
							crx = sx + StrStruct.StrWidth - cx;
							cry = sy + StrStruct.StrHeight - cy;
							if(rot&MASK_DEG_90_270){
								cry = cry - wbits1;
								crx = crx - hbits1;
							} else {
								cry = cry - hbits1;
								crx = crx - wbits1;
							}
						}
						PrtStruct.x = crx;
						PrtStruct.y = cry;
						FontThaiCharOut(chw_thai_left, rot);
						break;
					case DEG_270:
						if(StrStruct.StrMirx) {
							crx = sx + cy;
							cry = sy + cx;					
						} else {
							crx = sx + cy;
							cry = sy + StrStruct.StrWidth - cx;
							if(rot&MASK_DEG_90_270){
								cry = cry - wbits1;
							} else {
								cry = cry - hbits1;
							}
						}
						PrtStruct.x = crx;
						PrtStruct.y = cry;						
						FontThaiCharOut(chw_thai_left, rot);
						break;
				}
			}
			wbits = wbits1;		
			hbits = hbits1;
		}
		if (StrStruct.StrType == TWO_BYTE_FONT_STR && (INT16U)(p[position_counter]) > 0x80) {
			nwbits = wbits2;		
			nhbits = hbits2;
		} else {
			if((INT16U)*p < 0x20) {//FONT_CON_CH < 0x20
				nwbits = 0;
				nhbits = 0;///////rot
			} else {
//Insert Fixed Font 20031218
				if(FontStruct.FixedWidthFlag1 == 1 || StrStruct.UseFixedWidth == 1) {
					nwbits = FontStruct.FixedWbits1 * StrStruct.Magx;
				} else {
					fontw_ptr = FontStruct.PtrWidth1 + ((INT16U)p[position_counter]-FontStruct.FirstCode1);
					nwbits = *(fontw_ptr) * StrStruct.Magx;
				}
				nhbits=FontStruct.Hbits1*StrStruct.Magy;				
			}
		}
		// increase rel ch pos include ch rot
			
		if((StrStruct.ChRot&MASK_DEG_90_270) && ((INT16U)chw_thai_left != 0x20)) {
//HYP 20040807 	//if interchcp==0, 1 char out before LF.
	    	if ((cx != str_left_margin[line_idx]) || ((INT16U)chw_thai_left != 0x20)) {///htab
 				if(ctrl_ch == OFF) {
					cx = cx + hbits; 
					cx = cx + StrStruct.InterChSp;
				}
				if((cx + nhbits) > (StrStruct.StrWidth - str_right_margin[line_idx]) && (PrtStruct.Mode != RECEIPT || &p[position_counter] >= p_max)) {/////htab
					line_idx++;
					cx = str_left_margin[line_idx];
					while ((cx + nhbits) > (StrStruct.StrWidth - str_right_margin[line_idx])) {
						line_idx++;
						if(line_idx >= max_line) {
							loop = FALSE;
							break;
						}
						cx = str_left_margin[line_idx];
					}
					if(line_idx >= max_line) {
						loop = FALSE;
					}
				} else {
					if (((PrtStruct.Opaque == ON) || (StrStruct.Typeface != NO_TYPEFACE))) {
						if ((cx < (StrStruct.StrWidth - str_right_margin[line_idx])) && (StrStruct.InterChSp != 0)) {
							sphbits = wbits/StrStruct.Magy;
							for ( i = 0; i < (spbytes * sphbits); i++) {
								PrtStruct.PrtScratchPad[i] = 0x00;
							}
							Font_Typeface(PrtStruct.PrtScratchPad,spbytes,StrStruct.InterChSp,sphbits,DEG_0);
 					   		switch(PrtStruct.Rot + StrStruct.StrMirx * 2) {
								case DEG_0:
								case DEG_360:
									PrtStruct.x = crx + hbits;
									PrtStruct.y = cry;
									break;
								case DEG_90:
								case DEG_450:
									PrtStruct.x = crx;
									PrtStruct.y = cry + hbits;
									break;
								case DEG_180:
								case DEG_540:
									PrtStruct.x = crx - StrStruct.InterChSp;
									PrtStruct.y = cry;
									break;
								case DEG_270:
								case DEG_630:
									PrtStruct.x = crx;
									PrtStruct.y = cry - StrStruct.InterChSp;
									break;
							}
							rx = StrStruct.Magx; //backup
							StrStruct.Magx = 1;
				   	 		PrtPattern(PrtStruct.PrtScratchPad,spbytes,StrStruct.InterChSp,sphbits,PrtStruct.Rot);
							StrStruct.Magx = rx;	
	  					}
					}
				}
			}
		} else {
			if ((cx != str_left_margin[line_idx]) || ((INT16U)chw_thai_left != 0x20)) {//////htab
				if(ctrl_ch == OFF) {
					cx = cx + wbits;
					cx = cx + StrStruct.InterChSp;
				}
				if((cx + nwbits) > (StrStruct.StrWidth - str_right_margin[line_idx]) && (PrtStruct.Mode != RECEIPT || &p[position_counter] >= p_max))  {//////htab				
					line_idx++;
					cx = str_left_margin[line_idx];
					while ((cx + nwbits) > (StrStruct.StrWidth - str_right_margin[line_idx])) {
						line_idx++;
						if(line_idx >= max_line) {
							loop = FALSE;
							break;
						}
						cx = str_left_margin[line_idx];
					}
					if(line_idx >= max_line) {
						loop = FALSE;
					}
				} else {
					if (((PrtStruct.Opaque == ON) || (StrStruct.Typeface != NO_TYPEFACE))) {
						if ((cx < (StrStruct.StrWidth - str_right_margin[line_idx])) && (StrStruct.InterChSp != 0)) {
							sphbits = hbits/StrStruct.Magy;
							for ( i = 0; i < (spbytes * sphbits); i++) {
								PrtStruct.PrtScratchPad[i] = 0x00;   
							}
							Font_Typeface(PrtStruct.PrtScratchPad,spbytes,StrStruct.InterChSp,sphbits,DEG_0);
 			   				switch(PrtStruct.Rot + StrStruct.StrMirx * 2) {
								case DEG_0:
								case DEG_360:
									PrtStruct.x = crx + wbits;
									PrtStruct.y = cry;
									break;
								case DEG_90:
								case DEG_450:
									PrtStruct.x = crx;
									PrtStruct.y = cry + wbits;
									break;
								case DEG_180:
								case DEG_540:
									PrtStruct.x = crx - StrStruct.InterChSp;
									PrtStruct.y = cry;
									break;
								case DEG_270:
								case DEG_630:
									PrtStruct.x = crx;
									PrtStruct.y = cry - StrStruct.InterChSp;
									break;
							}
							rx = StrStruct.Magx; //backup
							StrStruct.Magx = 1;
				  	  		PrtPattern(PrtStruct.PrtScratchPad,spbytes,StrStruct.InterChSp,sphbits,PrtStruct.Rot);
							StrStruct.Magx = rx;	
						}
					}
				}
			}
		}
		if((INT16U)p[position_counter] == '\0' || position_counter == (calculatestrnum+1))
		{
			loop=FALSE;
		}
    }
	StrStruct.Magx = magx_backup;	 //for auto shrink
	PrtStruct.x = backup_x;
	PrtStruct.y = backup_y;
	PrtPattern_Str_Area_Typeface();
}
#else
void  FontStrOut(HUGEDATA char *ch_ptr) 
{
	short cx,cy,crx,cry,bits_sum,i;
	short by[100],str_left_margin[100],str_right_margin[100],whiteSpace,whitespace_V;
	char rot,wbits1,hbits1,wbits2,hbits2,wbits,hbits,nwbits,nhbits,justification,sbits;
	INT8U line_idx;
	char line_end,loop,spbytes,rx,ry;
	CWD		chw;	
	HUGEDATA char *p,*p_max;/////htab
	char wordwrap, p_sum, /*baseline_height,*/ctrl_ch,max_line;//HYP 20050224 erase baseline_height
	short MbaselineH,MbaselineL,baselineL,baselineH,baseline1,baseline2,line_height;
	short sx,sy;
	HUGEDATA INT8U *fontw_ptr;
	char variable_init_flag, magx_backup;
	char ch_rot_backup;/////////rot
	INT8U	sphbits;
	short	ncx;////////htab
	INT8U	htab_pos_count;	////////htab
	short	max_cx;////////htab
	BOOLEAN updata_mag;
	INT8U id1_backup, id2_backup;
//	char string_buf[30];
	//20040209 HYP
	short backup_x,backup_y;
	backup_x = PrtStruct.x;
	backup_y = PrtStruct.y;

	variable_init_flag = 1;
	loop = TRUE;	
	magx_backup = StrStruct.Magx;	 //for auto shrink
	ch_rot_backup = StrStruct.ChRot;  /////////rot
	id1_backup = StrStruct.Id1;
	id2_backup = StrStruct.Id2;

	if(*ch_ptr == '\0') return;
	while (loop) {
		if(variable_init_flag == 1) {
			variable_init_flag = 0;

			sx = PrtStruct.x;
			sy = PrtStruct.y;
			spbytes = StrStruct.InterChSp>>3;
			if(StrStruct.InterChSp&0x07) spbytes++;
			p = ch_ptr;
			wordwrap = StrStruct.WordWrap;
			cx = 0;								// rel. character pos x
			cy = 0;								// rel. character pos y
			justification = StrStruct.Justification;
			if(StrStruct.StrMirx) { //mirror
				if(justification == RIGHT) {
					justification = LEFT;
				} else if(justification == LEFT) {
					justification = RIGHT;
				}
			}
//////////////////////////////////load font1,2 start
			rx = StrStruct.Magx;
			ry = StrStruct.Magy;	
			wbits2 = FontStruct.Wbits2*rx;	  	// mag. width bits of 2 bytes font
			hbits2 = FontStruct.Hbits2*ry;		// mag. height bits of 2 bytes font
			wbits1 = FontStruct.Wbits1*rx;		// mag. width bits of 1 bytes font
			hbits1 = FontStruct.Hbits1*ry;		// mag. height bits of 1 bytes font
			wbits = wbits1;		// mag. width inc. bits of 1 str line
			hbits = hbits1;		// mag. height inc. bits of 1 str line
			nwbits = wbits1;		// mag. width inc. bits of 1 str line
			nhbits = hbits1;		// mag. height inc. bits of 1 str line
			baseline2 = FontStruct.Baseline2*ry;
			baseline1 = FontStruct.Baseline1*ry;
//Insert Fixed Font 20031218
//			fontw_ptr = FontStruct.PtrWidth1 + ((INT8U)0x20-FontStruct.FirstCode1);
//			sbits = *(fontw_ptr) * rx;
			if(FontStruct.FixedWidthFlag1 == 1 || StrStruct.UseFixedWidth == 1) {
				sbits = FontStruct.FixedWbits1 * rx;
			} else {
				fontw_ptr = FontStruct.PtrWidth1 + ((INT8U)0x20-FontStruct.FirstCode1);
				sbits = *(fontw_ptr) * rx;
			}
////////////////////////////
//////////////////////////////////load font1,2 end
			str_left_margin[0] = 0;
			str_right_margin[0]=0;
			line_idx = 0;
			htab_pos_count = 0;	 ////////htab
			max_cx = 0;	 ////////htab
			ncx = 0;  ///////htab
			line_end = FALSE;
			by[0] = 0;
			baselineL = 0;
			baselineH = 0;

			p_sum = 0;
			bits_sum = 0;
			MbaselineH = 0;
			MbaselineL = 0;
			line_height = 0;
#ifdef USE_TEXT_EMPHASIS	// ~1 : Italic, ~2 : Bold, ~3 : Underline 강조하는 기능 이후 부터 스페이스 나 엔터가 나오기 전에 강조표현 유지한다 		
			text_emphasis_state = 0;
#endif
			
		}	/*  Init end  */

 //justification and baseline check
		ctrl_ch = OFF;
		chw.byte.left = *p;
		chw.byte.right = *(p+1);//HYP 20040128
		if(chw.byte.left == _CR) {
			if(*(p+1) == LF) {
				p++;
			}
			chw.byte.left = LF;
		}
#ifdef USE_TEXT_EMPHASIS	// ~1 : Italic, ~2 : Bold, ~3 : Underline 강조하는 기능 이후 부터 스페이스 나 엔터가 나오기 전에 강조표현 유지한다 		
		// type 0 : // ~1 : Italic, ~2 : Bold, ~3 : Underline 강조하는 기능 이후 부터 스페이스 나 엔터가 나오기 전에 강조표현 유지한다 
		text_emphasis_loop=1;
		while(text_emphasis_loop)
		{
			switch(text_emphasis_state)
				{
					case 0 : //Init ,Start State
						switch(*p)
						{
							case '~': // Start State
								text_emphasis_state = 1;
								p++;
								break;
							default : // Not Command, Set normal
								text_emphasis_loop=0; //Parsing End 							
								break;
						}
						break;
					case 1 : // move to normal state
						switch(*p)
						{
							case '1': // Italic
							case '2': // Bold
							case '3': // Underline
								p++;
								chw.byte.left = *p;
								text_emphasis_state = 2;
								break;
							case LF: 
							case 0x20: 
								p--;
								text_emphasis_state = 0;
								text_emphasis_loop=0; //Parsing End
								break;								
							default : // Not Command, Set normal
								p--;							
								text_emphasis_state = 0;
								text_emphasis_loop=0; //Parsing End 							
								break;
						}
						break;
			
					case 2 : // Normal state, keep setting
						switch(*p)
						{
							case '~': 
								text_emphasis_state=3;
								p++;
								break;
							case LF: 
							case 0x20: 
								text_emphasis_state = 0;								
								text_emphasis_loop=0; //Parsing End
								break;
							default : // Not Command, Set normal
								text_emphasis_loop=0; //Parsing End
								break;

						}
						break;
					case 3 : // Normal state, keep setting						
						switch(*p)
						{
							case '1': // Italic
							case '2': // Bold
							case '3': // Underline
								p++;
								chw.byte.left = *p;
								text_emphasis_state = 2;
								break;
							case LF: 
							case 0x20: 
								p--;
								text_emphasis_state = 0;
								text_emphasis_loop=0; //Parsing End 							
								break;
							default : // Not Command, Set normal
								p--;							
								text_emphasis_state = 2;
								text_emphasis_loop=0; //Parsing End 								
								break;
						}
						break;
					default : 
						text_emphasis_state = 0;
						text_emphasis_loop=0; //Parsing End 							
						break;
							
				}
		}
		// type 1 : // ~ : Italic, * : Bold, _ : Underline 강조하는 기능 이후 부터 스페이스 나 엔터가 나오기 전에 강조표현 유지한다 , 다시 심볼이 나오면 강조표현 삭제 
#endif

		
		if (StrStruct.StrType == TWO_BYTE_FONT_STR && chw.byte.left > 0x80) {
			p = p + 2;
			wbits = wbits2;		
			hbits = hbits2;
			baselineL = baseline2;
			baselineH = hbits2 - baseline2;
			p_sum = p_sum + 2;
		} else {
			p++;
			if (chw.byte.left == FONT_CON_CH) {
				ctrl_ch = ON;
				updata_mag = OFF;
				switch (*p) {
					///////////rot
					case FONT_CON_DEG:
						p++;
						switch(*p) {
							case '0':
								StrStruct.ChRot = DEG_0;
								break;
							case '1':
								StrStruct.ChRot = DEG_90;
								break;
							case '2':
								StrStruct.ChRot = DEG_180;
								break;
							case '3':
								StrStruct.ChRot = DEG_270;
								break;
						}
						break;
					case FONT_CON_MAG_X:
						p++;
						switch(*p) {
							case '1':
								rx = 1;
								break;
							case '2':
								rx = 2;
								break;
							case '3':
								rx = 3;
								break;
							case '4':
								rx = 4;
								break;
							case '5':
								rx = 5;
								break;
						}
						updata_mag = ON;
						break;
					case FONT_CON_MAG_Y:
						p++;
						switch(*p) {
							case '1':
								ry = 1;
								break;
							case '2':
								ry = 2;
								break;
							case '3':
								ry = 3;
								break;
							case '4':
								ry = 4;
								break;
							case '5':
								ry = 5;
								break;
						}
						updata_mag = ON;
						break;
					case FONT_CON_INC_MAG:
						if(rx < 5) {
							rx++;
						}
						if(ry < 5) {
							ry++;
						}
						updata_mag = ON;
						break;
					case FONT_CON_DEC_MAG:
						if(rx > 1) {
							rx--;
						}
						if(ry > 1) {
							ry--;
						}
						updata_mag = ON;
						break;
					case FONT_CON_FONT2:
						p++;
						FontLoadFont2(*p - '0');
						updata_mag = ON;
						break;
					case FONT_CON_FONT1:
						p++;
						FontLoadFont1(*p - '0');
						updata_mag = ON;
						break;
				//HYP 20040621
				//FONT_CON_CH > 0x20
				//	case '0':
				//		ctrl_ch = OFF;
				//		break;
				}
				if(updata_mag == ON) {
					baseline2 = FontStruct.Baseline2*ry;
					baseline1 = FontStruct.Baseline1*ry;
					wbits2 = FontStruct.Wbits2*rx;	  	// mag. width bits of 2 bytes font
					hbits2 = FontStruct.Hbits2*ry;		// mag. height bits of 2 bytes font
					wbits1 = FontStruct.Wbits1*rx;		// mag. width bits of 1 bytes font
					hbits1 = FontStruct.Hbits1*ry;		// mag. height bits of 1 bytes font
					fontw_ptr = FontStruct.PtrWidth1 + ((INT8U)0x20-FontStruct.FirstCode1);
				}
				p++;
			}
			//HYP 20071130
			//if((INT8U)chw.byte.left < FontStruct.FirstCode1) {
			if((INT8U)chw.byte.left < 0x20) {
				//HYP 20050224
				//wbits1 = 0;
				if (StrStruct.ChRot&MASK_DEG_90_270) {
					hbits1 = 0;
				} else {
					wbits1 = 0;
				}
				ctrl_ch = ON;  
			} else {
//Insert Fixed Font 20031218
//				fontw_ptr = FontStruct.PtrWidth1 + ((INT8U)chw.byte.left-FontStruct.FirstCode1);
//				wbits1 = *(fontw_ptr) * rx;
				if(FontStruct.FixedWidthFlag1 == 1 || StrStruct.UseFixedWidth == 1) {
					wbits1 = FontStruct.FixedWbits1 * rx;
				} else {
					fontw_ptr = FontStruct.PtrWidth1 + ((INT8U)chw.byte.left-FontStruct.FirstCode1);
					wbits1 = *(fontw_ptr) * rx;
				}
				//HYP 20050224
				hbits1= FontStruct.Hbits1*ry;
////////////////////////////
			}
			wbits = wbits1;		
			hbits = hbits1;
			//HYP 20050224
			//baselineH = hbits1 - baseline1;
			//baselineL = baseline1;
			if (StrStruct.ChRot&MASK_DEG_90_270) {
				baselineH = wbits1;
				baselineL = 0;
			} else {
				baselineH = hbits1 - baseline1;
				baselineL = baseline1;
			}
			if(chw.byte.left != 0x20) {
//				if ((ctrl_ch == ON) || (chw.byte.left == FONT_CON_CH)) {
//					p_sum = 0;	  //////////////////
//				} else {
//					p_sum = p_sum + 1;
//				}
				//HYP 20040128
				if ((ctrl_ch == ON) || (chw.byte.left == FONT_CON_CH)) {
//HYP 20040621
					//FONT_CON_CH > 0x20
//					if(chw.byte.right == '0') {
//						p_sum = p_sum + 2;
//					} else {
//						p_sum = 0;
//					}
					p_sum = 0; //FONT_CON_CH < 0x20
				} else {
					p_sum = p_sum + 1;
				}

				if(PrtStruct.Mode == RECEIPT && (INT8U)chw.byte.left == HT) {	////////htab
					if(max_cx < cx) max_cx = cx;
					cx = StrStruct.HTab[htab_pos_count]; ///htab 
					htab_pos_count++;
					if(max_cx < cx) max_cx = cx;
				}
			} else {
//Insert Fixed Font 20031218
//				sbits = *(fontw_ptr) * rx;/////////////////////////
				if(FontStruct.FixedWidthFlag1 == 1 || StrStruct.UseFixedWidth == 1) {
					sbits = FontStruct.FixedWbits1 * rx;
				} else {
					sbits = *(fontw_ptr) * rx;/////////////////////////
				}
////////////////////////////
				p_sum = 0;
			}
		}
		if (StrStruct.StrType == TWO_BYTE_FONT_STR && (INT8U)(*p) > 0x80) {	//you must cast!!!
			nwbits = wbits2;		
			nhbits = hbits2;
		} else {
//			if(((INT8U)*p < FontStruct.FirstCode1) || ((INT8U)*p == FONT_CON_CH && (INT8U)*(p+1) != FONT_CON_CH)) {
//HYP 20040621
//			if(((INT8U)*p < FontStruct.FirstCode1) || ((INT8U)*p == FONT_CON_CH && (INT8U)*(p+1) != '0')) {//FONT_CON_CH > 0x20 //HYP 20040127
			//HYP 20071130
			//if((INT8U)*p < FontStruct.FirstCode1) {//FONT_CON_CH < 0x20
			if((INT8U)*p < 0x20) {//FONT_CON_CH < 0x20
				nwbits = 0;
				nhbits = 0; ///////rot
			} else {
//Insert Fixed Font 20031218
//				fontw_ptr = FontStruct.PtrWidth1 + ((INT8U)*p-FontStruct.FirstCode1);
//				nwbits = *(fontw_ptr) * rx;
				if(FontStruct.FixedWidthFlag1 == 1 || StrStruct.UseFixedWidth == 1) {
					nwbits = FontStruct.FixedWbits1 * rx;
				} else {
					fontw_ptr = FontStruct.PtrWidth1 + ((INT8U)*p-FontStruct.FirstCode1);
					nwbits = *(fontw_ptr) * rx;
				}
////////////////////////////
				//HYP 20050224
				//nhbits = hbits1;
				nhbits = FontStruct.Hbits1*ry;
			}
			if(PrtStruct.Mode == RECEIPT && (INT8U)*p == HT) {	////////htab
				ncx = StrStruct.HTab[htab_pos_count]; ////
			}
		}
		if((StrStruct.ChRot&MASK_DEG_90_270) && (chw.byte.left != 0x20)) {
			//HYP 20050224
			//if (MbaselineH < wbits) {
		    //	MbaselineH = wbits;
			//}
			if (MbaselineH < baselineH) {
				MbaselineH = baselineH;
			}
			if (MbaselineL < baselineL) {
				MbaselineL = baselineL;
			}			
		   	if ((chw.byte.left != LF) && (chw.byte.left != '\0')) {
			   	if (((cx != 0) || (chw.byte.left != 0x20)) && (ctrl_ch == OFF)) {
			   		cx = cx + hbits; 
					cx = cx + StrStruct.InterChSp;
				}
				if (chw.byte.left == 0x20 || ctrl_ch == ON) {
					bits_sum = 0;
				} else {
					bits_sum = bits_sum + hbits + StrStruct.InterChSp;
				}
			}
			if((cx+nhbits > StrStruct.StrWidth) || (ncx > StrStruct.StrWidth) || (chw.byte.left == LF) || (chw.byte.left == '\0')) {/////htab
				if(PrtStruct.Mode != RECEIPT) {/////////htab
					if (wordwrap == ON) {
						if((chw.byte.left != 0x20) && (*p != 0x20) && (*p != HT) && (chw.byte.left != LF) && (chw.byte.left != '\0') && (cx != bits_sum)) {////htab
							cx = cx - bits_sum ;
							p = p - p_sum;
							if (cx != 0) {
								if (*(p-1) == 0x20){   ///htab
									cx = cx - sbits - 2*StrStruct.InterChSp;
									p--;
								} else {
									cx = cx - StrStruct.InterChSp;
								}
							}
					 	} else {
							if (cx != 0) {
								if (chw.byte.left == 0x20){
									cx = cx - sbits - 2*StrStruct.InterChSp;
									p--;
								} else { 
									cx = cx - StrStruct.InterChSp;
								}
							}
						}
					} else {
						if (cx != 0) {
							if (chw.byte.left == 0x20) {
								cx = cx - sbits - 2*StrStruct.InterChSp;
								p--;
							} else {
								cx = cx - StrStruct.InterChSp;
							}
						}
					}
					p_sum = 0;
					bits_sum = 0;
					by[line_idx] = cy + MbaselineH;
					line_height = MbaselineH + MbaselineL;
					cy = cy + line_height;
					max_line = line_idx;
					if(cy > StrStruct.StrHeight) {
						PrtStruct.StrAreaOverFlag = TRUE;//HYP 20040903	not applied to PC program (PrtRender)
						whiteSpace = 0;
						loop = FALSE;
					} else {
						whiteSpace = StrStruct.StrWidth - cx;
						//HYP 20040903
						//if(*p == '\0') {
						if((*p == '\0') || (chw.byte.left == '\0')) {
							loop=FALSE;
							max_line++;
						}
					}
					cy = cy + StrStruct.LineSp;
					MbaselineH = 0;
					MbaselineL = 0;
					cx = 0;
				} else {  //////PrtStruct.Mode == RECEIPT
					by[line_idx] = cy + MbaselineH;
					PrtStruct.ReceiptLineHeight = MbaselineH + MbaselineL;
					if(PrtStruct.ReceiptLineHeight <= StrStruct.StrHeight) {
						if(max_cx < cx) max_cx = cx;
						whiteSpace = StrStruct.StrWidth - max_cx + StrStruct.InterChSp;
						max_line = 1;
						p_max = p;	////////
					} else {
						max_line = 0;
						PrtStruct.ReceiptLineHeight = 0;
					}				
					loop = FALSE;
				}
				line_end=TRUE;
			}
		} else {
			if (MbaselineH < baselineH) {
				MbaselineH = baselineH;
			}
			if (MbaselineL < baselineL) {
				MbaselineL = baselineL;
			}
			if ((chw.byte.left != LF) && (chw.byte.left != '\0')) {
				if (((cx != 0) || (chw.byte.left != 0x20)) && (ctrl_ch == OFF)) {
					cx = cx + wbits; 
					cx = cx + StrStruct.InterChSp;
				}
				if (chw.byte.left == 0x20 || ctrl_ch == ON) {
					bits_sum = 0;
				} else {
					bits_sum = bits_sum + wbits + StrStruct.InterChSp;
				}
			}
			if((cx+nwbits > StrStruct.StrWidth) || (ncx > StrStruct.StrWidth) || (chw.byte.left == LF) || (chw.byte.left == '\0')) {/////htab
				if(PrtStruct.Mode != RECEIPT) {/////////htab
					if (wordwrap == ON) {
						if((chw.byte.left != 0x20) && (*p != 0x20) && (*p != HT) && (chw.byte.left != LF) && (chw.byte.left != '\0') && (cx != bits_sum)) {///htab
							cx = cx - bits_sum ;
							p = p - p_sum;				   
							if (cx != 0) {
								if (*(p-1) == 0x20){
									cx = cx - sbits - 2*StrStruct.InterChSp;
									p--;
								} else {
									cx = cx - StrStruct.InterChSp;
								}
							}
					 	} else {
							if (cx != 0) {
								if (chw.byte.left == 0x20){
									cx = cx - sbits - 2*StrStruct.InterChSp;
									p--;
								} else { 
									cx = cx - StrStruct.InterChSp;
								}
							}
						}
					} else {
						if (cx != 0) {
							if (chw.byte.left == 0x20) {
								cx = cx - sbits - 2*StrStruct.InterChSp;
								p--;
							} else { 
								cx = cx - StrStruct.InterChSp;
							}
						}
					}
					p_sum = 0;
					bits_sum = 0;
					by[line_idx] = cy + MbaselineH;
					line_height = MbaselineH + MbaselineL;
					cy = cy + line_height;
					max_line = line_idx;
					if(cy > StrStruct.StrHeight) {
						if(StrStruct.AutoShrinkx == ON) {	//auto shrink mode
							if(StrStruct.Magx != 1) {
								variable_init_flag = 1;		//for auto shrink
								loop = TRUE;
								StrStruct.Magx--;
							} else {
								PrtStruct.StrAreaOverFlag = TRUE;//HYP 20040903	not applied to PC program (PrtRender)
								whiteSpace = 0;
								loop = FALSE;
								cy = cy - StrStruct.LineSp - StrStruct.LineSp - line_height;//for vertical centering
						    }
						} else {
							PrtStruct.StrAreaOverFlag = TRUE;//HYP 20040903	not applied to PC program (PrtRender)
							whiteSpace = 0;
							loop = FALSE;
							cy = cy - StrStruct.LineSp - StrStruct.LineSp - line_height;//for vertical centering
						}
					} else {
						whiteSpace = StrStruct.StrWidth - cx;	  
						//HYP 20040903
						//if(*p == '\0') 
						if((*p == '\0') || (chw.byte.left == '\0')){
							loop=FALSE;
							max_line++;
						}
					}
					cy = cy + StrStruct.LineSp;
					MbaselineH = 0;
					MbaselineL = 0;
					cx = 0;
				} else { //PrtStruct.Mode == RECEIPT
					by[line_idx] = cy + MbaselineH;
					PrtStruct.ReceiptLineHeight =  MbaselineH + MbaselineL;
					if(PrtStruct.ReceiptLineHeight <= StrStruct.StrHeight) {
						if(max_cx < cx) max_cx = cx;
						whiteSpace = StrStruct.StrWidth - max_cx + StrStruct.InterChSp;
						max_line = 1;
						p_max = p;	////////
					} else {
						max_line = 0;
						PrtStruct.ReceiptLineHeight = 0;
					}
					loop = FALSE;
				}
				line_end=TRUE;
			}
		}
		if(PrtStruct.Mode == LABEL)	//VCentering은 RECIEPT인 경우에는 사용하지 않음.R_Interprerter없음.
		{
			if((StrStruct.VCentering == ON) && (loop==FALSE)) {	  //Vertical Centering ,change StrStruct.StrHeight
			whitespace_V = StrStruct.StrHeight - cy;
			if (whitespace_V > 0) {
				whitespace_V = 	whitespace_V>>1;
				switch(PrtStruct.Rot) {
					case DEG_0: 
						sy = sy + whitespace_V;
						StrStruct.StrHeight = cy;
						break;
					case DEG_90:
						sx = sx + whitespace_V;
						StrStruct.StrHeight = cy;
						break;
					case DEG_180: 
						sy = sy + whitespace_V;
						StrStruct.StrHeight = cy;
						break;
					case DEG_270:
						sx = sx + whitespace_V;
						StrStruct.StrHeight = cy;
						break;
					}
				}
			}
		}
		if(line_end==TRUE) {
			line_end = FALSE;
			switch (justification) {		// apply justification control
				case CENTER:
					str_left_margin[line_idx] = whiteSpace / 2;
					str_right_margin[line_idx] = whiteSpace / 2;
					break;
				case RIGHT:
					//HYP 20040226///////////////
					if(PrtStruct.Mode == RECEIPT) {
						if(StrStruct.Typeface & ITALIC) {	
							whiteSpace = whiteSpace - ((line_height/StrStruct.Magy)/6)*StrStruct.Magx; 
						} 
						if(StrStruct.Typeface & BOLD) {	
							whiteSpace = whiteSpace - StrStruct.Magx;
						}
						if(StrStruct.Typeface & OUTLINE) {	
							whiteSpace = whiteSpace - StrStruct.Magx;
						}
						if(whiteSpace < 0) whiteSpace = 0; 
					}
					//////////////////////////////
					str_left_margin[line_idx] = whiteSpace;
					str_right_margin[line_idx] = 0;
					break;
				case LEFT:
				default:	// to Left
					str_left_margin[line_idx] = 0;
					str_right_margin[line_idx] = whiteSpace;
					break;
			}
			line_idx++;
		}
	}
	//start rendering

	StrStruct.ChRot = ch_rot_backup;  /////////rot
	FontLoadFont1(id1_backup);
	FontLoadFont2(id2_backup);

	baseline2 = FontStruct.Baseline2*StrStruct.Magy;
	baseline1 = FontStruct.Baseline1*StrStruct.Magy;
	wbits2 = FontStruct.Wbits2*StrStruct.Magx;	  	// mag. width bits of 2 bytes font
	hbits2 = FontStruct.Hbits2*StrStruct.Magy;		// mag. height bits of 2 bytes font
	wbits1 = FontStruct.Wbits1*StrStruct.Magx;		// mag. width bits of 1 bytes font
	hbits1 = FontStruct.Hbits1*StrStruct.Magy;		// mag. height bits of 1 bytes font

	wbits = wbits1;		// mag. width inc. bits of 1 str line
	hbits = hbits1;		// mag. height inc. bits of 1 str line
	nwbits = wbits1;		// mag. width inc. bits of 1 str line
	nhbits = hbits1;		// mag. height inc. bits of 1 str line
//	baseline_height = 0;//HYP 20050224 erase baseline_height
	p = ch_ptr;
	cx = str_left_margin[0];
		
	// rel. character pos x
	cy = 0;
	loop = TRUE;
	line_idx = 0;
	htab_pos_count = 0;	 ////////htab
	if(line_idx >= max_line) {
		loop = FALSE;
	}

	while (loop) {
		ctrl_ch = OFF;
		chw.byte.left = *p;

		if(chw.byte.left == _CR) {
			if(*(p+1) == LF) {
				p++;
			}
			chw.byte.left = LF;
		}
#ifdef USE_TEXT_EMPHASIS	// ~1 : Italic, ~2 : Bold, ~3 : Underline 강조하는 기능 이후 부터 스페이스 나 엔터가 나오기 전에 강조표현 유지한다 
		// type 0 : // ~1 : Italic, ~2 : Bold, ~3 : Underline 강조하는 기능 이후 부터 스페이스 나 엔터가 나오기 전에 강조표현 유지한다 
		text_emphasis_loop=1;
		while(text_emphasis_loop)
		{
			switch(text_emphasis_state)
				{
					case 0 : //Init ,Start State
						switch(*p)
						{
							case '~': // Start State
								text_emphasis_state = 1;
								p++;
								break;
							default : // Not Command, Set normal
								text_emphasis_loop=0; //Parsing End								
								break;
						}
						break;
					case 1 : // move to normal state
						switch(*p)
						{
							case '1': // Italic
								StrStruct.Typeface |= ITALIC;
								p++;
								chw.byte.left = *p;
								text_emphasis_state = 2;
								break;
							case '2': // Bold
								StrStruct.Typeface |= BOLD;
								p++;
								chw.byte.left = *p;
								text_emphasis_state = 2;
								break;
							case '3': // Underline
								StrStruct.Typeface |= UNDERLINE;
								p++;
								chw.byte.left = *p;
								text_emphasis_state = 2;
								break;
							case LF: 
							case 0x20: 
								p--;
								text_emphasis_state = 0;
								text_emphasis_loop=0; //Parsing End
								break;								
							default : // Not Command, Set normal
								p--;							
								text_emphasis_state = 0;
								text_emphasis_loop=0; //Parsing End								
								break;
						}
						break;
			
					case 2 : // Normal state, keep setting
						switch(*p)
						{
							case '~': 
								text_emphasis_state=3;
								p++;
								break;
							case LF: 
							case 0x20: 
								StrStruct.Typeface = 0x00;
								text_emphasis_state = 0;								
								text_emphasis_loop=0; //Parsing End
								break;
							default : // Not Command, Set normal
								text_emphasis_loop=0; //Parsing End
								break;

						}
						break;
					case 3 : // Normal state, keep setting						
						switch(*p)
						{
							case '1': // Italic
								StrStruct.Typeface |= ITALIC;
								p++;
								chw.byte.left = *p;
								text_emphasis_state = 2;
								break;
							case '2': // Bold
								StrStruct.Typeface |= BOLD;
								p++;
								chw.byte.left = *p;
								text_emphasis_state = 2;
								break;
							case '3': // Underline
								StrStruct.Typeface |= UNDERLINE;
								p++;
								chw.byte.left = *p;
								text_emphasis_state = 2;
								break;
							case LF: 
							case 0x20: 
								p--;
								StrStruct.Typeface = 0x00;
								text_emphasis_state = 0;
								text_emphasis_loop=0; //Parsing End								
								break;
							default : // Not Command, Set normal
								p--;							
								text_emphasis_state = 2;
								text_emphasis_loop=0; //Parsing End									
								break;
						}
						break;
					default : 
						StrStruct.Typeface = 0x00;
						text_emphasis_state = 0;
						text_emphasis_loop=0; //Parsing End 							
						break;
							
				}
		}
		// type 1 : // ~ : Italic, * : Bold, _ : Underline 강조하는 기능 이후 부터 스페이스 나 엔터가 나오기 전에 강조표현 유지한다 , 다시 심볼이 나오면 강조표현 삭제 

#endif
		if (StrStruct.StrType == TWO_BYTE_FONT_STR && chw.byte.left > 0x80) {
			if(StrStruct.ChRot & MASK_DEG_90_270) {	//chw.byte.left != 0x20
				cy = by[line_idx] - wbits2;
			} else {
				cy = by[line_idx] - hbits2 + baseline2;
			}
			chw.byte.right = *(p+1);
			rot = PrtStruct.Rot + StrStruct.ChRot; //chw.byte.left != 0x20
	    	switch(PrtStruct.Rot) {
				case DEG_0:
					if(StrStruct.StrMirx) {
						crx = sx + StrStruct.StrWidth - cx;
						cry = sy + cy;
						if(rot&MASK_DEG_90_270){
							crx = crx - hbits2;
						} else {
							crx = crx - wbits2;
						}	
					} else {
						crx = sx + cx;
						cry = sy + cy;
					}
					PrtStruct.x = crx;
					PrtStruct.y = cry;
					FontDCharOut(chw.word,rot);
					break;
				case DEG_90:
					if(StrStruct.StrMirx) {
						crx = sx + StrStruct.StrHeight - cy;
						cry = sy + StrStruct.StrWidth - cx;
						if(rot&MASK_DEG_90_270){
							crx = crx - hbits2;
							cry = cry - wbits2;
						} else {
							crx = crx - wbits2;
							cry = cry - hbits2;
						}					
					} else {
						crx = sx + StrStruct.StrHeight - cy;
						cry = sy + cx;
						if(rot&MASK_DEG_90_270){
							crx = crx - hbits2;
						} else {
							crx = crx - wbits2;
						}
					}
					PrtStruct.x = crx;
					PrtStruct.y = cry;
					FontDCharOut(chw.word,rot);						
					break;
				case DEG_180:
					if(StrStruct.StrMirx) {
						crx = sx + cx;
						cry = sy + StrStruct.StrHeight - cy;
						if(rot&MASK_DEG_90_270){
							cry = cry - wbits2;
						} else {
							cry = cry - hbits2;
						}
					} else {
						crx = sx + StrStruct.StrWidth - cx;
						cry = sy + StrStruct.StrHeight - cy;
						if(rot&MASK_DEG_90_270){
							cry = cry - wbits2;
							crx = crx - hbits2;
						} else {
							cry = cry - hbits2;
							crx = crx - wbits2;
						}
					}
					PrtStruct.x = crx;
					PrtStruct.y = cry;
					FontDCharOut(chw.word, rot);
					break;
				case DEG_270:
					if(StrStruct.StrMirx) {
						crx = sx + cy;
						cry = sy + cx;					
					} else {
						crx = sx + cy;
						cry = sy + StrStruct.StrWidth - cx;
						if(rot&MASK_DEG_90_270){
							cry = cry - wbits2;
						} else {
							cry = cry - hbits2;
						}
					}
					PrtStruct.x = crx;
					PrtStruct.y = cry;						
					FontDCharOut(chw.word, rot);
					break;
			}				
	       	p = p + 2;
			wbits = wbits2;		
			hbits = hbits2;
		} else {
   	    	p++;
			if (chw.byte.left == FONT_CON_CH) {
				ctrl_ch = ON;
				switch (*p)	{
					/////////rot
					case FONT_CON_DEG:
						p++;
						switch(*p) {
							case '0':
								StrStruct.ChRot = DEG_0;
								break;
							case '1':
								StrStruct.ChRot = DEG_90;
								break;
							case '2':
								StrStruct.ChRot = DEG_180;
								break;
							case '3':
								StrStruct.ChRot = DEG_270;
								break;
						}
						break;
					//Typeface
					case FONT_CON_TYPE_ITALIC:
						StrStruct.Typeface |= ITALIC;
						break;
					case FONT_CON_TYPE_UNDERLINE:
						StrStruct.Typeface |= UNDERLINE;
						break;
					case FONT_CON_TYPE_THROUGH_LINE:
						StrStruct.Typeface |= THROUGH_LINE;
						break;
					//20040209 HYP
					//case FONT_CON_TYPE_SHADOW:
					//	StrStruct.Typeface |= SHADOW;
					//	break;
					case FONT_CON_TYPE_BOLD:
						StrStruct.Typeface |= BOLD;
						break;
					//20040209 HYP
					//case FONT_CON_TYPE_REVERSE:
					//	StrStruct.Typeface |= REVERSE;
					//	break;
					case FONT_CON_TYPE_DTHROUGH_LINE:
						StrStruct.Typeface |= DTHROUGH_LINE;
						break;
					case FONT_CON_TYPE_OUTLINE:
						StrStruct.Typeface |= OUTLINE;
						break;
					case FONT_CON_TYPE_NO_TYPEFACE:
						StrStruct.Typeface = NO_TYPEFACE;
						break;
					case FONT_CON_MAG_X:
						p++;
						switch(*p) {
							case '1':
								StrStruct.Magx = 1;
								break;
							case '2':
								StrStruct.Magx = 2;
								break;
							case '3':
								StrStruct.Magx = 3;
								break;
							case '4':
								StrStruct.Magx = 4;
								break;
							case '5':
								StrStruct.Magx = 5;
								break;
						}
						break;
					case FONT_CON_MAG_Y:
						p++;
						switch(*p) {
							case '1':
								StrStruct.Magy = 1;
								break;
							case '2':
								StrStruct.Magy = 2;
								break;
							case '3':
								StrStruct.Magy = 3;
								break;
							case '4':
								StrStruct.Magy = 4;
								break;
							case '5':
								StrStruct.Magy = 5;
								break;
						}
						break;
					case FONT_CON_INC_MAG:
						if(StrStruct.Magx < 5) {
							StrStruct.Magx++;
						}
						if(StrStruct.Magy < 5) {
							StrStruct.Magy++;
						}
						break;
					case FONT_CON_DEC_MAG:
						if(StrStruct.Magx > 1) {
							StrStruct.Magx--;
						}
						if(StrStruct.Magy > 1) {
							StrStruct.Magy--;
						}
						break;
					case FONT_CON_FONT2:
						p++;
						FontLoadFont2(*p - '0');
						break;
					case FONT_CON_FONT1:
						p++;
						FontLoadFont1(*p - '0');
						break;
				//HYP 20040621
				//FONT_CON_CH > 0x20
				//	case '0':
				//		ctrl_ch = OFF;
				//		break;
				}
				p++;
				baseline2 = FontStruct.Baseline2*StrStruct.Magy;
				baseline1 = FontStruct.Baseline1*StrStruct.Magy;
				wbits2 = FontStruct.Wbits2*StrStruct.Magx;	  	// mag. width bits of 2 bytes font
				hbits2 = FontStruct.Hbits2*StrStruct.Magy;		// mag. height bits of 2 bytes font
				wbits1 = FontStruct.Wbits1*StrStruct.Magx;		// mag. width bits of 1 bytes font
				hbits1 = FontStruct.Hbits1*StrStruct.Magy;		// mag. height bits of 1 bytes font
			}
			//HYP 20071130
			//if((INT8U)chw.byte.left < FontStruct.FirstCode1) {
			if((INT8U)chw.byte.left < 0x20) {
				//HYP 20050224
				//wbits1 = 0;
				if (StrStruct.ChRot&MASK_DEG_90_270) {
					hbits1 = 0;
				} else {
					wbits1 = 0;
				}				
				ctrl_ch = ON;  
			} else {
//Insert Fixed Font 20031218
//				fontw_ptr = FontStruct.PtrWidth1 + ((INT8U)chw.byte.left-FontStruct.FirstCode1);
//				wbits1 = *(fontw_ptr) * StrStruct.Magx;
				if(FontStruct.FixedWidthFlag1 == 1 || StrStruct.UseFixedWidth == 1) {
					wbits1 = FontStruct.FixedWbits1 * StrStruct.Magx;
				} else {
					fontw_ptr = FontStruct.PtrWidth1 + ((INT8U)chw.byte.left-FontStruct.FirstCode1);
					wbits1 = *(fontw_ptr) * StrStruct.Magx;
				}
				//HYP 20050224
				hbits1=FontStruct.Hbits1*StrStruct.Magy;				
////////////////////////////
			}
			if((StrStruct.ChRot & MASK_DEG_90_270) && (chw.byte.left != 0x20)) { 
				cy = by[line_idx] - wbits1;
			} else {
				cy = by[line_idx] - hbits1 + baseline1;
			}
			if(PrtStruct.Mode == RECEIPT && (INT8U)chw.byte.left == HT) {	////////htab
				cx = str_left_margin[line_idx] + StrStruct.HTab[htab_pos_count]; ///htab
				htab_pos_count++;
			}
 			if((chw.byte.left != LF) && ((cx != str_left_margin[line_idx]) || (chw.byte.left != 0x20)) && (ctrl_ch == OFF)) {
				if((StrStruct.ChRot & MASK_DEG_90_270) && (chw.byte.left == 0x20)) {
					rot = PrtStruct.Rot;
				} else {
					rot = PrtStruct.Rot + StrStruct.ChRot;
				}
 			   	switch(PrtStruct.Rot) {
					case DEG_0:
						if(StrStruct.StrMirx) {
							crx = sx + StrStruct.StrWidth - cx;
							cry = sy + cy;
							if(rot&MASK_DEG_90_270){
								crx = crx - hbits1;
							} else {
								crx = crx - wbits1;
							}	
						} else {
							crx = sx + cx;
							cry = sy + cy;
						}
						PrtStruct.x = crx;
						PrtStruct.y = cry;
						FontCharOut(chw.byte.left, rot);
						break;
					case DEG_90:
						if(StrStruct.StrMirx) {
							crx = sx + StrStruct.StrHeight - cy;
							cry = sy + StrStruct.StrWidth - cx;
							if(rot&MASK_DEG_90_270){
								crx = crx - hbits1;
								cry = cry - wbits1;
							} else {
								crx = crx - wbits1;
								cry = cry - hbits1;
							}					
						} else {
							crx = sx + StrStruct.StrHeight - cy;
							cry = sy + cx;
							if(rot&MASK_DEG_90_270){
								crx = crx - hbits1;
							} else {
								crx = crx - wbits1;
							}
						}
						PrtStruct.x = crx;
						PrtStruct.y = cry;
						FontCharOut(chw.byte.left, rot);						
						break;
					case DEG_180:
						if(StrStruct.StrMirx) {
							crx = sx + cx;
							cry = sy + StrStruct.StrHeight - cy;
							if(rot&MASK_DEG_90_270){
								cry = cry - wbits1;
							} else {
								cry = cry - hbits1;
							}
						} else {
							crx = sx + StrStruct.StrWidth - cx;
							cry = sy + StrStruct.StrHeight - cy;
							if(rot&MASK_DEG_90_270){
								cry = cry - wbits1;
								crx = crx - hbits1;
							} else {
								cry = cry - hbits1;
								crx = crx - wbits1;
							}
						}
						PrtStruct.x = crx;
						PrtStruct.y = cry;
						FontCharOut(chw.byte.left, rot);
						break;
					case DEG_270:
						if(StrStruct.StrMirx) {
							crx = sx + cy;
							cry = sy + cx;					
						} else {
							crx = sx + cy;
							cry = sy + StrStruct.StrWidth - cx;
							if(rot&MASK_DEG_90_270){
								cry = cry - wbits1;
							} else {
								cry = cry - hbits1;
							}
						}
						PrtStruct.x = crx;
						PrtStruct.y = cry;						
						FontCharOut(chw.byte.left, rot);
						break;
				}
			}
			wbits = wbits1;		
			hbits = hbits1;
		}
		if (StrStruct.StrType == TWO_BYTE_FONT_STR && (INT8U)(*p) > 0x80) {
			nwbits = wbits2;		
			nhbits = hbits2;
		} else {
//			if(((INT8U)*p < FontStruct.FirstCode1) || ((INT8U)*p == FONT_CON_CH && (INT8U)*(p+1) != FONT_CON_CH)) {
//HYP 20040621
//			if(((INT8U)*p < FontStruct.FirstCode1) || ((INT8U)*p == FONT_CON_CH && (INT8U)*(p+1) != '0')) {//FONT_CON_CH > 0x20 //HYP 20040127
			//HYP 20071130
			//if((INT8U)*p < FontStruct.FirstCode1) {//FONT_CON_CH < 0x20
			if((INT8U)*p < 0x20) {//FONT_CON_CH < 0x20
//HYP 20040621
				nwbits = 0;
				nhbits = 0;///////rot
//HYP 20040611
/*
				if(*p == LF) {//if interchcp==0, 1 char out before LF.
					nwbits = 1;
					nhbits = 1;///////rot
				} else {
					nwbits = 0;
					nhbits = 0;///////rot
				}
*/

			} else {
//Insert Fixed Font 20031218
//				fontw_ptr = FontStruct.PtrWidth1 + ((INT8U)*p-FontStruct.FirstCode1);
//				nwbits = *(fontw_ptr) * StrStruct.Magx;
				if(FontStruct.FixedWidthFlag1 == 1 || StrStruct.UseFixedWidth == 1) {
					nwbits = FontStruct.FixedWbits1 * StrStruct.Magx;
				} else {
					fontw_ptr = FontStruct.PtrWidth1 + ((INT8U)*p-FontStruct.FirstCode1);
					nwbits = *(fontw_ptr) * StrStruct.Magx;
				}
//////////////////////////// 
				//HYP 20050224
				//nhbits = hbits1;
				nhbits=FontStruct.Hbits1*StrStruct.Magy;				
			}
		}
		// increase rel ch pos include ch rot
			
		if((StrStruct.ChRot&MASK_DEG_90_270) && (chw.byte.left != 0x20)) {
//HYP 20050224 erase
//			if (line_height < wbits) {
//				line_height = wbits;
//			}
//HYP 20040621
//	    	if ((chw.byte.left != LF) && ((cx != str_left_margin[line_idx]) || (chw.byte.left != 0x20)) && (ctrl_ch == OFF)) {///htab
//			   	cx = cx + hbits; 
//				cx = cx + StrStruct.InterChSp;
//HYP 20040807 	//if interchcp==0, 1 char out before LF.
//	    	if ((chw.byte.left != LF) && ((cx != str_left_margin[line_idx]) || (chw.byte.left != 0x20))) {///htab
	    	if ((cx != str_left_margin[line_idx]) || (chw.byte.left != 0x20)) {///htab
				if(ctrl_ch == OFF) {
					cx = cx + hbits; 
					cx = cx + StrStruct.InterChSp;
				}
				if((cx + nhbits) > (StrStruct.StrWidth - str_right_margin[line_idx]) && (PrtStruct.Mode != RECEIPT || p >= p_max)) {/////htab
					line_idx++;
					cx = str_left_margin[line_idx];
					//HYP 20050224
					//while ((cx + hbits) > (StrStruct.StrWidth - str_right_margin[line_idx])) {
					while ((cx + nhbits) > (StrStruct.StrWidth - str_right_margin[line_idx])) {
						line_idx++;
						if(line_idx >= max_line) {
							loop = FALSE;
							break;
						}
						cx = str_left_margin[line_idx];
					}
					if(line_idx >= max_line) {
						loop = FALSE;
					}
				} else {
					if (((PrtStruct.Opaque == ON) || (StrStruct.Typeface != NO_TYPEFACE))) {
						if ((cx < (StrStruct.StrWidth - str_right_margin[line_idx])) && (StrStruct.InterChSp != 0)) {
							sphbits = wbits/StrStruct.Magy;
							for ( i = 0; i < (spbytes * sphbits); i++) {
								PrtStruct.PrtScratchPad[i] = 0x00;
							}
							Font_Typeface(PrtStruct.PrtScratchPad,spbytes,StrStruct.InterChSp,sphbits,DEG_0);
 					   		switch(PrtStruct.Rot + StrStruct.StrMirx * 2) {
								case DEG_0:
								case DEG_360:
									PrtStruct.x = crx + hbits;
									PrtStruct.y = cry;
									break;
								case DEG_90:
								case DEG_450:
									PrtStruct.x = crx;
									PrtStruct.y = cry + hbits;
									break;
								case DEG_180:
								case DEG_540:
									PrtStruct.x = crx - StrStruct.InterChSp;
									PrtStruct.y = cry;
									break;
								case DEG_270:
								case DEG_630:
									PrtStruct.x = crx;
									PrtStruct.y = cry - StrStruct.InterChSp;
									break;
							}
							rx = StrStruct.Magx; //backup
							StrStruct.Magx = 1;
				   	 		PrtPattern(PrtStruct.PrtScratchPad,spbytes,StrStruct.InterChSp,sphbits,PrtStruct.Rot);
							StrStruct.Magx = rx;	
	  					}
					}
				}
			}
		} else {
//HYP 20050224 erase
//			if (line_height < hbits) {
//				line_height = hbits;
//			}
//HYP 20040621
//			if ((chw.byte.left != LF) && ((cx != str_left_margin[line_idx]) || (chw.byte.left != 0x20)) && (ctrl_ch == OFF)) {//////htab
//				cx = cx + wbits;
//				cx = cx + StrStruct.InterChSp;
//HYP 20040807 	//if interchcp==0, 1 char out before LF.
//			if ((chw.byte.left != LF) && ((cx != str_left_margin[line_idx]) || (chw.byte.left != 0x20))) {//////htab
			if ((cx != str_left_margin[line_idx]) || (chw.byte.left != 0x20)) {//////htab
				if(ctrl_ch == OFF) {
					cx = cx + wbits;
					cx = cx + StrStruct.InterChSp;
				}
				if((cx + nwbits) > (StrStruct.StrWidth - str_right_margin[line_idx]) && (PrtStruct.Mode != RECEIPT || p >= p_max)) { //////htab
					line_idx++;
					cx = str_left_margin[line_idx];
					//HYP 20050224
					//while ((cx + wbits) > (StrStruct.StrWidth - str_right_margin[line_idx])) {
					while ((cx + nwbits) > (StrStruct.StrWidth - str_right_margin[line_idx])) {
						line_idx++;
						if(line_idx >= max_line) {
							loop = FALSE;
							break;
						}
						cx = str_left_margin[line_idx];
					}
					if(line_idx >= max_line) {
						loop = FALSE;
					}
				} else {
					if (((PrtStruct.Opaque == ON) || (StrStruct.Typeface != NO_TYPEFACE))) {
						if ((cx < (StrStruct.StrWidth - str_right_margin[line_idx])) && (StrStruct.InterChSp != 0)) {
							sphbits = hbits/StrStruct.Magy;
							for ( i = 0; i < (spbytes * sphbits); i++) {
								PrtStruct.PrtScratchPad[i] = 0x00;   
							}
							Font_Typeface(PrtStruct.PrtScratchPad,spbytes,StrStruct.InterChSp,sphbits,DEG_0);
 			   				switch(PrtStruct.Rot + StrStruct.StrMirx * 2) {
								case DEG_0:
								case DEG_360:
									PrtStruct.x = crx + wbits;
									PrtStruct.y = cry;
									break;
								case DEG_90:
								case DEG_450:
									PrtStruct.x = crx;
									PrtStruct.y = cry + wbits;
									break;
								case DEG_180:
								case DEG_540:
									PrtStruct.x = crx - StrStruct.InterChSp;
									PrtStruct.y = cry;
									break;
								case DEG_270:
								case DEG_630:
									PrtStruct.x = crx;
									PrtStruct.y = cry - StrStruct.InterChSp;
									break;
							}
							rx = StrStruct.Magx; //backup
							StrStruct.Magx = 1;
				  	  		PrtPattern(PrtStruct.PrtScratchPad,spbytes,StrStruct.InterChSp,sphbits,PrtStruct.Rot);
							StrStruct.Magx = rx;	
						}
					}
				}
			}
		}
		if(*p == '\0') loop=FALSE;
    }
	StrStruct.Magx = magx_backup;	 //for auto shrink
	//20040209 HYP
	PrtStruct.x = backup_x;
	PrtStruct.y = backup_y;
	PrtPattern_Str_Area_Typeface();
}
#endif //USE_VIETNAM_FONT
#endif //USE_ARAB_FONT

//#pragma optimize 2
//If StrStruct(FontStruct) is _near, Don't use optimize level 2

#ifdef USE_VIETNAM_FONT
void FontCharOut(INT16U ch, char rot)
#else
#if defined(USE_ARAB_FONT) || defined(HEBREW_FONT)
void FontCharOut(INT16U ch, char rot)
#else
void FontCharOut(char ch,char rot)
#endif
#endif
{
	short    i,j,k;
	HUGEDATA INT8U *font_ptr;
	char wbits,hbits,wbytes;
#ifdef USE_VIETNAM_FONT
	INT16U offset;
#else
#if defined(USE_ARAB_FONT) || defined(HEBREW_FONT)
	INT16U offset;
#else
	INT8U offset;
#endif
#endif
	HUGEDATA INT8U *fontw_ptr;
	INT8U	font_bytes;
	char	b_magx,b_magy;
	char	expand_wbyte_flag;

	/* Check Boundary */
//	if (x<0) return;
//	if (y<0) return;
//	if (y>=PrtStruct.PrintableHeight) {
//		y=PrtStruct.PrintableHeight-1;
//	}
	

	//HYP 20041101
	//exceipt UNDERLINE,THROUGH_LINE,DTHROUGH_LINE
#ifdef USE_VIETNAM_FONT
	if((PrtStruct.Mode == RECEIPT) && ((INT16U)ch == 0x20)) {
#else
	if((PrtStruct.Mode == RECEIPT) && (ch == 0x20)) {
#endif
		if ((StrStruct.Typeface&0x07) == 0x00) return;
	}

	hbits = FontStruct.Hbits1;
	wbytes = FontStruct.FWBytes1;
	font_bytes = FontStruct.FHbits1 * FontStruct.FWBytes1;

//	Font location & Font ID is fixed for testing  (font_ptr, FontStruct.PtrFont1)
#ifdef USE_VIETNAM_FONT
	offset = (INT16U)ch - FontStruct.FirstCode1;
	if((INT16U)ch < FontStruct.FirstCode1) {
#else
#if defined(USE_ARAB_FONT) || defined(HEBREW_FONT)
	offset = ch - FontStruct.FirstCode1;
	if (ch < FontStruct.FirstCode1) {
#else
	offset = (INT8U)ch - FontStruct.FirstCode1;
	if((INT8U)ch < FontStruct.FirstCode1) {
#endif
#endif
		wbits = 0;
	} else {
//Insert Fixed Font 20031218
//		fontw_ptr = FontStruct.PtrWidth1 + offset;//include font data offset
//		wbits = *(fontw_ptr);
		if(FontStruct.FixedWidthFlag1 == 1 || StrStruct.UseFixedWidth == 1) {
			wbits = FontStruct.FixedWbits1;
		} else {
			fontw_ptr = FontStruct.PtrWidth1 + offset;//include font data offset
			wbits = *(fontw_ptr);
		}
////////////////////////////
	}

	if((rot & MASK_DEG_90_270)&&(StrStruct.Typeface == NO_TYPEFACE)&&(FontStruct.PreRot1 == PREROT_0DEG_90DEG)) {
		b_magx = StrStruct.Magx;   //
		b_magy = StrStruct.Magy;   //
		StrStruct.Magx = b_magy;   //
		StrStruct.Magy = b_magx;   // 
		font_ptr = FontStruct.PtrFont1 + ((INT32U)offset + FontStruct.TotalCharNum1) * font_bytes;//rotate font
		PrtPattern(font_ptr,FontStruct.FWBytes1,hbits,wbits,rot-FontStruct.PreRot1);
		StrStruct.Magx = b_magx;   //
		StrStruct.Magy = b_magy;   //
	} else {
		font_ptr = FontStruct.PtrFont1 + (INT32U)offset * font_bytes;//
		if (StrStruct.Typeface == NO_TYPEFACE) {
			PrtPattern(font_ptr,wbytes,wbits,hbits,rot);
		} else {
			expand_wbyte_flag = 0;
			if(StrStruct.Typeface & ITALIC) {	
				wbits=wbits + hbits/6;
			} 
			if(StrStruct.Typeface & BOLD) {	
				wbits++;
			}
			if(StrStruct.Typeface & OUTLINE) {	
				wbits++;
			}
			if(wbits > (wbytes<<3)) {
				if(StrStruct.EnableExpandByte == ON) {
					wbytes++;
					expand_wbyte_flag = 1;
				} else {
					wbits = wbytes<<3;
				}
			}
			if(expand_wbyte_flag == 0) {
				for ( i = 0; i < (wbytes * hbits); i++) {
					PrtStruct.PrtScratchPad[i] = font_ptr[i];	   
				}
				if(ch == 0x20) {
					Font_Typeface(PrtStruct.PrtScratchPad,wbytes,wbits,hbits,DEG_0);
				} else {
					Font_Typeface(PrtStruct.PrtScratchPad,wbytes,wbits,hbits,StrStruct.ChRot);
				}
				PrtPattern(PrtStruct.PrtScratchPad,wbytes,wbits,hbits,rot);
			} else { // expand 1 wbyte 
				k = 0;
				for ( i = 0; i <hbits; i++) {
					for (j=0; j<(wbytes-1); j++) {
						PrtStruct.PrtScratchPad[i*wbytes+j] = font_ptr[k];
						k++;	   
					}
					PrtStruct.PrtScratchPad[i*wbytes+j] = 0x00;
				}
				if(ch == 0x20) {
					Font_Typeface(PrtStruct.PrtScratchPad,wbytes,wbits,hbits,DEG_0);
				} else {
					Font_Typeface(PrtStruct.PrtScratchPad,wbytes,wbits,hbits,StrStruct.ChRot);
				}
				PrtPattern(PrtStruct.PrtScratchPad,wbytes,wbits,hbits,rot);
			}
		}
	}
}

#ifdef USE_THAI_FONT
extern INT8U IsThaiUpperVowel(INT8U ch);
extern INT8U IsThaiToneMark(INT8U ch);
extern INT8U ThaiCP874Cnv(INT8U ch);

void FontThaiCharOut(INT32U ch, char rot)
{
	short    i,j,k;
	INT8U *font_ptr;
	char wbits,hbits,wbytes;
	INT8U offset;

	INT8U *fontw_ptr;
	INT8U	font_bytes;
	char	b_magx,b_magy;
	char	expand_wbyte_flag;

        INT8U *thai_ptr;
        INT16U tone_mark_font_offset;
           
        thai_ptr = (INT8U *)&ch;
        tone_mark_font_offset = 0;
    
	if((PrtStruct.Mode == RECEIPT) && (thai_ptr[0] == 0x20)) {
		if ((StrStruct.Typeface&0x07) == 0x00) return;
	}

	hbits = FontStruct.Hbits1;
	wbytes = FontStruct.FWBytes1;
	font_bytes = FontStruct.FHbits1 * FontStruct.FWBytes1;

	offset = (INT8U)thai_ptr[0] - FontStruct.FirstCode1;
	if((INT8U)thai_ptr[0] < FontStruct.FirstCode1) {
		wbits = 0;
	} else {
		if(FontStruct.FixedWidthFlag1 == 1 || StrStruct.UseFixedWidth == 1) {
			wbits = FontStruct.FixedWbits1;
		} else {
			fontw_ptr = FontStruct.PtrWidth1 + offset;//include font data offset
			wbits = *(fontw_ptr);
		}
	}

        //Make Thai Font Start/////////////////////////////////////////////////////////////////////////////
      	if((rot & MASK_DEG_90_270) && (FontStruct.PreRot1 == PREROT_0DEG_90DEG)) {
            font_ptr = FontStruct.PtrFont1 + (FontStruct.TotalCharNum1 + (INT16U)thai_ptr[0] - FontStruct.FirstCode1)*font_bytes;
        } else {//PREROT_0DEG_90DEG
            font_ptr = FontStruct.PtrFont1 + ((INT16U)thai_ptr[0] - FontStruct.FirstCode1)*font_bytes;
        }
       
        for(i = 0; i < font_bytes; i++) {
            PrtStruct.PrtScratchPadFont[i] = font_ptr[i];
        }
        
        thai_ptr[1] = ThaiCP874Cnv(thai_ptr[1]);
        if (thai_ptr[1] != 0x00)
        {
            if((rot & MASK_DEG_90_270) && (FontStruct.PreRot1 == PREROT_0DEG_90DEG)) {
                font_ptr = FontStruct.PtrFont1 + (FontStruct.TotalCharNum1 + (INT16U)thai_ptr[1] - FontStruct.FirstCode1)*font_bytes;
            } else {//PREROT_0DEG_90DEG
                font_ptr = FontStruct.PtrFont1 + ((INT16U)thai_ptr[1] - FontStruct.FirstCode1)*font_bytes;
            }
            for(i = 0; i < font_bytes; i++) {
                PrtStruct.PrtScratchPadFont[i] |= font_ptr[i];
            }      
        }
        
        thai_ptr[2] = ThaiCP874Cnv(thai_ptr[2]);
        if (IsThaiUpperVowel(thai_ptr[1]) && IsThaiToneMark(thai_ptr[2])) tone_mark_font_offset = 0x50;
        
        if (thai_ptr[2] != 0x00)
        {
            if((rot & MASK_DEG_90_270) && (FontStruct.PreRot1 == PREROT_0DEG_90DEG)) {
                font_ptr = FontStruct.PtrFont1 + (FontStruct.TotalCharNum1 + (INT16U)thai_ptr[2] - tone_mark_font_offset - FontStruct.FirstCode1)*font_bytes;
            } else {//PREROT_0DEG_90DEG
                font_ptr = FontStruct.PtrFont1 + ((INT16U)thai_ptr[2] - tone_mark_font_offset - FontStruct.FirstCode1)*font_bytes;
            }
            for(i = 0; i < font_bytes; i++) {
                PrtStruct.PrtScratchPadFont[i] |= font_ptr[i];
            }      
        }
        
        tone_mark_font_offset = 0x50;
        
        thai_ptr[3] = ThaiCP874Cnv(thai_ptr[3]);
        if (thai_ptr[3] != 0x00)
        {
            if((rot & MASK_DEG_90_270) && (FontStruct.PreRot1 == PREROT_0DEG_90DEG)) {
                font_ptr = FontStruct.PtrFont1 + (FontStruct.TotalCharNum1 + (INT16U)thai_ptr[3] - tone_mark_font_offset - FontStruct.FirstCode1)*font_bytes;
            } else {//PREROT_0DEG_90DEG
                font_ptr = FontStruct.PtrFont1 + ((INT16U)thai_ptr[3] - tone_mark_font_offset - FontStruct.FirstCode1)*font_bytes;//0x50 -> font offset (upper area)
            }
            for(i = 0; i < font_bytes; i++) {
                PrtStruct.PrtScratchPadFont[i] |= font_ptr[i];
            }      
        }  
        
        font_ptr = PrtStruct.PrtScratchPadFont;
        //Make Thai Font End///////////////////////////////////////////////////////////////////////////////
    
          
	if((rot & MASK_DEG_90_270)&&(StrStruct.Typeface == NO_TYPEFACE)&&(FontStruct.PreRot1 == PREROT_0DEG_90DEG)) {
		b_magx = StrStruct.Magx;   //
		b_magy = StrStruct.Magy;   //
		StrStruct.Magx = b_magy;   //
		StrStruct.Magy = b_magx;   // 
		//font_ptr = FontStruct.PtrFont1 + ((INT32U)offset + FontStruct.TotalCharNum1) * font_bytes;//rotate font
		PrtPattern(font_ptr,FontStruct.FWBytes1,hbits,wbits,rot-FontStruct.PreRot1);
		StrStruct.Magx = b_magx;   //
		StrStruct.Magy = b_magy;   //
	} else {
		//font_ptr = FontStruct.PtrFont1 + (INT32U)offset * font_bytes;//
		if (StrStruct.Typeface == NO_TYPEFACE) {
			PrtPattern(font_ptr,wbytes,wbits,hbits,rot);
		} else {
			expand_wbyte_flag = 0;
			if(StrStruct.Typeface & ITALIC) {	
				wbits=wbits + hbits/6;
			} 
			if(StrStruct.Typeface & BOLD) {	
				wbits++;
			}
			if(StrStruct.Typeface & OUTLINE) {	
				wbits++;
			}
			if(wbits > (wbytes<<3)) {
				if(StrStruct.EnableExpandByte == ON) {
					wbytes++;
					expand_wbyte_flag = 1;
				} else {
					wbits = wbytes<<3;
				}
			}
			if(expand_wbyte_flag == 0) {
				for ( i = 0; i < (wbytes * hbits); i++) {
					PrtStruct.PrtScratchPad[i] = font_ptr[i];	   
				}
				if(thai_ptr[0] == 0x20) {
					Font_Typeface(PrtStruct.PrtScratchPad,wbytes,wbits,hbits,DEG_0);
				} else {
					Font_Typeface(PrtStruct.PrtScratchPad,wbytes,wbits,hbits,StrStruct.ChRot);
				}
				PrtPattern(PrtStruct.PrtScratchPad,wbytes,wbits,hbits,rot);
			} else { // expand 1 wbyte 
				k = 0;
				for ( i = 0; i <hbits; i++) {
					for (j=0; j<(wbytes-1); j++) {
						PrtStruct.PrtScratchPad[i*wbytes+j] = font_ptr[k];
						k++;	   
					}
					PrtStruct.PrtScratchPad[i*wbytes+j] = 0x00;
				}
				if(thai_ptr[0] == 0x20) {
					Font_Typeface(PrtStruct.PrtScratchPad,wbytes,wbits,hbits,DEG_0);
				} else {
					Font_Typeface(PrtStruct.PrtScratchPad,wbytes,wbits,hbits,StrStruct.ChRot);
				}
				PrtPattern(PrtStruct.PrtScratchPad,wbytes,wbits,hbits,rot);
			}
		}
	}
}
#endif

void FontDCharOut(INT16U chw, char rot)
{
	short    i,j,k;	
	CWD		offset;
	HUGEDATA INT8U	*font_ptr;
	char 	wbits, hbits, wbytes;
	INT8U	font_bytes;
	char	b_magx,b_magy;
	char	expand_wbyte_flag;
	char 	invalid_code_flag;

	invalid_code_flag=0;

	/* Check Boundary */
//	if (x<0) return;
//	if (y<0) return;
//	if (y>=PrtStruct.PrintableHeight) {
//		y=PrtStruct.PrintableHeight-1;
//	}
	wbits = FontStruct.Wbits2;
	hbits = FontStruct.Hbits2;
	wbytes = FontStruct.FWBytes2;
	font_bytes = FontStruct.FHbits2 * FontStruct.FWBytes2;

	//Use only Korean 2byte font 
	//if you want to print China font, consider font map
	offset.word = chw - FontStruct.FirstCode2;// relative
#ifdef USE_BIG5_CHN_FONT
	if (offset.byte.left >= 0x26) offset.byte.left -= 0x02; 	//skip for 0xc740 ~ 0xc8ff 
	if (offset.byte.right >= 0x60) offset.byte.right -= 0x20; 	//skip for 0x0080 ~ 0x009f
	offset.word = ((INT16U)offset.byte.left * 0xa0) + offset.byte.right;	//
#else
	offset.word = ((INT16U)offset.byte.left * 0x60) + offset.byte.right;	//
#endif
	//HYP 20040513
	//if((rot & MASK_DEG_90_270)&&(StrStruct.Typeface == NO_TYPEFACE)&&(FontStruct.PreRot2)&&(FontStruct.Font2Type != TWO_BYTE_FONT_WAN)) {
	if((rot & MASK_DEG_90_270)&&(StrStruct.Typeface == NO_TYPEFACE)&&(FontStruct.PreRot2 == PREROT_0DEG_90DEG)) {
		if (FontStruct.Font2Type == TWO_BYTE_FONT_WAN) {//wansung
			if(offset.word < FontStruct.TotalCharNum2) {
				font_ptr = FontStruct.PtrFont2 + ((INT32U)offset.word +FontStruct.TotalCharNum2)* font_bytes;
			} else {
				invalid_code_flag=1;				
				//HYP 20050304
				//HanJohabPattern(PrtStruct.PrtScratchPad,0xffff,0);//output 'X' pattern(only 16x16, 24x24)
#ifndef USE_CHN_FONT
				if(((chw == 0xA1C9) && (Startup.country==COUNTRY_KR))||((chw == 0xA1CC)&&(get_global_bparam(GLOBAL_SALE_SETUP_FLAG4)&0x08))) {
					HanSpecialPattern(PrtStruct.PrtScratchPad,chw,FontStruct.PreRot2);
				} else {
					HanJohabPattern(PrtStruct.PrtScratchPad,0xffff,0);//output 'X' pattern(only 16x16, 24x24)
				}
#endif
			}
#ifndef USE_CHN_FONT
		} else {
			if(offset.word < (2350+50)) {//HYP 20040513 conv table max value 
				HanJohabPattern(PrtStruct.PrtScratchPad,HanWan2JohabConvTable[offset.word],FontStruct.PreRot2);
			} else {
				//HYP 20050304
				//HanJohabPattern(PrtStruct.PrtScratchPad,0xffff,0);//output 'X' pattern(only 16x16, 24x24)
				if(((chw == 0xA1C9) && (Startup.country==COUNTRY_KR))||((chw == 0xA1CC)&&(get_global_bparam(GLOBAL_SALE_SETUP_FLAG4)&0x08))) {
					HanSpecialPattern(PrtStruct.PrtScratchPad,chw,FontStruct.PreRot2);
				} else {
					HanJohabPattern(PrtStruct.PrtScratchPad,0xffff,0);//output 'X' pattern(only 16x16, 24x24)
				}
			}
#endif
		}
		b_magx = StrStruct.Magx;   //
		b_magy = StrStruct.Magy;   //
		StrStruct.Magx = b_magy;   //
		StrStruct.Magy = b_magx;   //
		if((FontStruct.Font2Type == TWO_BYTE_FONT_WAN) && (invalid_code_flag == 0)) {	  //wansung
			PrtPattern(font_ptr,FontStruct.FWBytes2,hbits,wbits,rot-FontStruct.PreRot2);
		} else { 					  //Johab
			PrtPattern(PrtStruct.PrtScratchPad,FontStruct.FWBytes2,hbits,wbits,rot-FontStruct.PreRot2);
		}
		StrStruct.Magx = b_magx;   //
		StrStruct.Magy = b_magy;   //
	} else { 
		if (FontStruct.Font2Type == TWO_BYTE_FONT_WAN) {//wansung
			if(offset.word < FontStruct.TotalCharNum2) {
				font_ptr = FontStruct.PtrFont2 + (INT32U)offset.word * font_bytes;
			} else {
				invalid_code_flag=1;
				//HYP 20050304
				//HanJohabPattern(PrtStruct.PrtScratchPad,0xffff,0);//output 'X' pattern(only 16x16, 24x24)
#ifndef USE_CHN_FONT
			if(((chw == 0xA1C9) && (Startup.country==COUNTRY_KR))||((chw == 0xA1CC)&&(get_global_bparam(GLOBAL_SALE_SETUP_FLAG4)&0x08))) {
					HanSpecialPattern(PrtStruct.PrtScratchPad,chw,0);
				} else {
					HanJohabPattern(PrtStruct.PrtScratchPad,0xffff,0);//output 'X' pattern(only 16x16, 24x24)
				}
#endif
			}
		} else {//johab
#ifndef USE_CHN_FONT
			if(offset.word < (2350+50)) {//HYP 20040513 conv table max value
				HanJohabPattern(PrtStruct.PrtScratchPad,HanWan2JohabConvTable[offset.word],0);
			} else {
				//HYP 20050304
				//HanJohabPattern(PrtStruct.PrtScratchPad,0xffff,0);//output 'X' pattern(only 16x16, 24x24)
				if(((chw == 0xA1C9) && (Startup.country==COUNTRY_KR))||((chw == 0xA1CC)&&(get_global_bparam(GLOBAL_SALE_SETUP_FLAG4)&0x08))) {
					HanSpecialPattern(PrtStruct.PrtScratchPad,chw,0);
				} else {
					HanJohabPattern(PrtStruct.PrtScratchPad,0xffff,0);//output 'X' pattern(only 16x16, 24x24)
				}
			}
#endif
		}
		if (StrStruct.Typeface != NO_TYPEFACE) {
			expand_wbyte_flag = 0;
			if(StrStruct.Typeface & ITALIC) {	
				wbits=wbits + hbits/6; 
			} 
			if(StrStruct.Typeface & BOLD) {	
				wbits++;
			}
			if(StrStruct.Typeface & OUTLINE) {	
				wbits++;
			}
			if(wbits > (wbytes<<3)) {
				if(StrStruct.EnableExpandByte == ON) {
					wbytes++;
					expand_wbyte_flag = 1;
				} else {
					wbits = wbytes<<3;
				}
			}
			if((FontStruct.Font2Type == TWO_BYTE_FONT_WAN) && (invalid_code_flag == 0)) {	 //wansung
				if(expand_wbyte_flag == 0) {
					for ( i = 0; i < (wbytes * hbits); i++) {
						PrtStruct.PrtScratchPad[i] = font_ptr[i];	   
					}
					Font_Typeface(PrtStruct.PrtScratchPad,wbytes,wbits,hbits,StrStruct.ChRot);
					PrtPattern(PrtStruct.PrtScratchPad,wbytes,wbits,hbits,rot);
				} else { // expand 1 wbyte 
					k = 0;
					for ( i = 0; i <hbits; i++) {
						for (j=0; j<(wbytes-1); j++) {
							PrtStruct.PrtScratchPad[i*wbytes+j] = font_ptr[k];
							k++;	   
						}
						PrtStruct.PrtScratchPad[i*wbytes+j] = 0x00;
					}
					Font_Typeface(PrtStruct.PrtScratchPad,wbytes,wbits,hbits,StrStruct.ChRot);
					PrtPattern(PrtStruct.PrtScratchPad,wbytes,wbits,hbits,rot);
				}
			} else {  //johab
				if(expand_wbyte_flag == 0) {  
					Font_Typeface(PrtStruct.PrtScratchPad,wbytes,wbits,hbits,StrStruct.ChRot);
					PrtPattern(PrtStruct.PrtScratchPad,wbytes,wbits,hbits,rot);
				} else {  //expand 1 wbyte
					k = 0;
					for ( i = 0; i <hbits; i++) {
						for (j=0; j<(wbytes-1); j++) {
							PrtStruct.PrtScratchPadFont[i*wbytes+j] = PrtStruct.PrtScratchPad[k];
							k++;	   
						}
						PrtStruct.PrtScratchPadFont[i*wbytes+j] = 0x00;
					}
					Font_Typeface(PrtStruct.PrtScratchPadFont,wbytes,wbits,hbits,StrStruct.ChRot);
					PrtPattern(PrtStruct.PrtScratchPadFont,wbytes,wbits,hbits,rot);
				}
			}
		} else {
			if((FontStruct.Font2Type == TWO_BYTE_FONT_WAN) && (invalid_code_flag ==0)) {	  //wansung
				PrtPattern(font_ptr,wbytes,wbits,hbits,rot);
			} else {						  //Johab
				PrtPattern(PrtStruct.PrtScratchPad,wbytes,wbits,hbits,rot);
			}
		}
	}
}


void Font_Underline(HUGEDATA INT8U *pattern,short dwr,short w,short h,char rot)
{
	short wb,pos,i,rw;
	INT8U mask;

	if(rot&MASK_DEG_90_270) {
		if(rot == DEG_90) {
			pos=w-1;
		} else {
			pos=0;
		}
		wb=pos>>3;
		rw=pos&0x07;
		mask = 0x80 >> rw;
		for (i=0; i<h; i++) {
			pattern[dwr*i + wb] |= mask;
		}
	} else {
		wb=w>>3;
		rw=w&0x07;
		if (rw) wb++;

		if(rot == DEG_0) {
			pos=(h-1)*dwr;
		} else {
			pos=0;
		}
		for (i=0; i<wb-1; i++) {
			pattern[pos+i]=0xff;
		}
		//i =  wb-1
		if(rw != 0) {
			pattern[pos+i] = lmask[rw];
		} else {
			pattern[pos+i]=0xff;
		}	
	}
}

#define THROUGH_LINE_WIDTH	2

void Font_Throughline(HUGEDATA INT8U *pattern,short dwr,short w,short h,char rot)
{
	short wb,pos,i,rw;
	INT8U mask;
	short j;

//	pos = FontStruct.StrikeRowIdx;
	if(rot&MASK_DEG_90_270) {
//		pos=(w-1)>>1;
//		wb=pos>>3;
//		rw=pos&0x07;
//		mask = 0x80 >> rw;
//		for (i=0; i<h; i++) {
//			pattern[dwr*i + wb] |= mask;
//		}
		pos=(w-1)>>1;
		for(j = 0; j < THROUGH_LINE_WIDTH; j++) {
			wb=(pos+j)>>3;
			rw=(pos+j)&0x07;
			mask = 0x80 >> rw;
			for (i=0; i<h; i++) {
				pattern[dwr*i + wb] |= mask;
			} 		 
		}
	} else {
//		pos=(h>>1);
//		pos = pos*dwr;
//
//		wb=w>>3;
//		rw=w&0x07;
//		if (rw) wb++;
//		
//		for (i=0; i<wb-1; i++) {
//			pattern[pos+i]=0xff;
//		}
//		//i =  wb-1
//		if(rw != 0) {
//			pattern[pos+i] = lmask[rw];
//		} else {
//			pattern[pos+i]=0xff;
//		}

		wb=w>>3;
		rw=w&0x07;
		if (rw) wb++;
		
		for(j = 0; j < THROUGH_LINE_WIDTH; j++) {
			pos=(h>>1);
			pos = pos*dwr;
			pos = pos+j*dwr;
			for (i=0; i<wb-1; i++) {
				pattern[pos+i]=0xff;
			}
			//i =  wb-1
			if(rw != 0) {
				pattern[pos+i] = lmask[rw];
			} else {
				pattern[pos+i]=0xff;
			}	
		}
	}
}

void Font_DThroughline(HUGEDATA INT8U *pattern,short dwr,short w,short h,char rot)
{
	short wb,pos,i,rw;
	INT8U mask;
	short j;

//	pos = FontStruct.StrikeRowIdx;
	if(rot&MASK_DEG_90_270) {
//		pos=(w-1)>>3;
//		pos=pos*2;
//		wb=pos>>3;
//		rw=pos&0x07;
//		mask = 0x80 >> rw;
//		for (i=0; i<h; i++) {
//			pattern[dwr*i + wb] |= mask;
//		}
//		pos=(w-1)>>3;
//		pos=pos*5;
//		wb=pos>>3;
//		rw=pos&0x07;
//		mask = 0x80 >> rw;
//		for (i=0; i<h; i++) {
//			pattern[dwr*i + wb] |= mask;
//		}
 
		pos=(w-1)>>3;
		pos=pos*2;

		for(j = 0; j < THROUGH_LINE_WIDTH; j++) {
			wb=(pos+j)>>3;
			rw=(pos+j)&0x07;
			mask = 0x80 >> rw;
			for (i=0; i<h; i++) {
				pattern[dwr*i + wb] |= mask;
			} 
		}

		pos=(w-1)>>3;
		pos=pos*4;

		for(j = 0; j < THROUGH_LINE_WIDTH; j++) {
			wb=(pos+j)>>3;
			rw=(pos+j)&0x07;
			mask = 0x80 >> rw;
			for (i=0; i<h; i++) {
				pattern[dwr*i + wb] |= mask;
			} 
		}
	} else {
//		wb=w>>3;
//		rw=w&0x07;
//		if (rw) wb++;
//
//		pos=h/8;
//		pos=dwr*pos*2;
//		
//		for (i=0; i<wb-1; i++) {
//			pattern[pos+i]=0xff;
//		}
//		//i =  wb-1
//		if(rw != 0) {
//			pattern[pos+i] = lmask[rw];
//		} else {
//			pattern[pos+i]=0xff;
//		}
//			
//		pos=h/8;
//		pos=dwr*pos*5;
//		for (i=0; i<wb-1; i++) {
//			pattern[pos+i]=0xff;
//		}
//		//i =  wb-1
//		if(rw != 0) {
//			pattern[pos+i] = lmask[rw];
//		} else {
//			pattern[pos+i]=0xff;
//		}
		wb=w>>3;
		rw=w&0x07;
		if (rw) wb++;

		for(j=0; j<	THROUGH_LINE_WIDTH; j++){
			pos=h/8;
			pos=dwr*pos*2;
			pos=pos+j*dwr;
			for (i=0; i<wb-1; i++) {
				pattern[pos+i]=0xff;
			}
			//i =  wb-1
			if(rw != 0) {
				pattern[pos+i] = lmask[rw];
			} else {
				pattern[pos+i]=0xff;
			}
		}
			
		for(j=0; j<	THROUGH_LINE_WIDTH; j++){
			pos=h/8;
			pos=dwr*pos*4;
			pos=pos+j*dwr;
			for (i=0; i<wb-1; i++) {
				pattern[pos+i]=0xff;
			}
			//i =  wb-1
			if(rw != 0) {
				pattern[pos+i] = lmask[rw];
			} else {
				pattern[pos+i]=0xff;
			}
		}	
	}
}

//20040209 HYP
/*
void Font_Reverse(HUGEDATA INT8U *pattern,short dwr,short w,short h)
{
	short wb,i,j,k,rw;	

	wb=w/8;
	rw=w%8;
	if (rw) wb++;

	k=0;
	for (i=0; i<h; i++) {
	    for (j=0; j<dwr; j++) {
			if (j < wb -1) {
				pattern[k]=~pattern[k];
			} else if(j == wb - 1) {
				if (rw != 0) {
					pattern[k]=lmask[rw] & ~pattern[k];
				} else {
					pattern[k]=~pattern[k];
				}
			} 
			k++;	
		}
	}
}
*/
//20040209 HYP
/*
void Font_Shadow( HUGEDATA INT8U *dest,short dwr,short w,short  height)
{
	short wb,i,j,q,rw;
	INT8U mask;

	wb=w/8;
	rw=w%8;
	if (rw) wb++;

	q=0;
	for (i=0; i<height; i++) {
		if ((i%2)==0) mask=0xaa; 
	    else mask=0x55;
	    for (j=0; j<dwr; j++) {
			if (j < wb -1) {
				*(dest + q) |=mask;
			} else if(j == wb - 1) {
				if (rw != 0) {
					*(dest + q) =lmask[rw] & (*(dest + q) | mask);
				} else {
					*(dest + q) |=mask;
				}
			}	
			q++;
	    }
	}

}
*/

void Font_Bold(HUGEDATA INT8U *dest,short dwr,short  height)
{
	short i,j;

	for (i=0; i<height; i++) {
		for(j=(dwr-1); j>0; j--) {
			dest[i*dwr+j] |= (dest[i*dwr+j] >> 1);
			dest[i*dwr+j] |= (dest[i*dwr+j-1] << 7);
		}
		dest[i*dwr] |= dest[i*dwr] >> 1;
	}
}

void Font_Italic(HUGEDATA INT8U *dest,short dwr, short height)
{
	short i,j,k,l;
	HUGEDATA INT8U *temp;

	j=height/6;
	k=0;
	for (i=0; i<height; i++)
	{
		temp = dest+dwr*i;
		for (l=dwr-1; l>0; l--) 
		{
			*(temp+l)=((*(temp+l-1))<<(8-j) ) | (*(temp+l))>>j;
		}
		*(temp)=(*(temp))>>j;
		k++;
		if (k==6) { j--; k=0;}
	}
}

void Font_Outline(HUGEDATA INT8U *pattern,short dwr,short height)
{
	short i,q;
	INT8U temp[288];
	
	q=dwr*height;
	memcpy( temp,pattern,q); 
	for (i=0; i<q; i++) {
	    pattern[i] |= temp[i] >>1;
	    pattern[i] |= temp[i] <<1;
	}
	for (i=0; i<q-dwr; i++) pattern[i+dwr] |= temp[i];
	for (i=dwr; i<q;   i++) pattern[i-dwr] |= temp[i];
	for (i=0;      i<q;   i++) pattern[i       ] ^= temp[i];
}

/*
void Font_Threed(HUGEDATA INT8U *pattern,short dwr,short height)
{
	short i,q;
	INT8U temp[288];
	
	q=dwr*height;
	memcpy( temp,pattern,q);	
	for (i=0; i<q       ; i++) pattern[    i   ] |= temp[i]>>1;
	for (i=0; i<q-dwr; i++) pattern[i+dwr] |= temp[i];
	for (i=0; i<q-dwr; i++) pattern[    i   ] ^= temp[i];
}
*/


void Font_Typeface(HUGEDATA INT8U *pattern,short dwr,short w,short h,char rot)
{
	char typeface;

	typeface = StrStruct.Typeface;

	if (typeface==NO_TYPEFACE) return;
	if (typeface&ITALIC) { // Italic 
		Font_Italic(pattern,dwr,h);
	}
	if (typeface&OUTLINE) {
		Font_Outline(pattern,dwr,h);
	}
	if (typeface&UNDERLINE) { // Underline
		Font_Underline(pattern,dwr,w,h,rot);
	}
	if (typeface&THROUGH_LINE) { // Throughline
		Font_Throughline(pattern,dwr,w,h,rot);
	}
	if (typeface&DTHROUGH_LINE) { // DThroughline
		Font_DThroughline(pattern,dwr,w,h,rot);
	}
	if (typeface&BOLD) { // Bold
		Font_Bold(pattern,dwr,h);	
	}
//20040209 HYP
/*
 	if (typeface&SHADOW) { // Shadow
		Font_Shadow(pattern,dwr,w,h);
	}
	if (typeface&REVERSE) { // Reverse
		Font_Reverse(pattern,dwr,w,h);
	}
*/
/*
	if (typeface&0x20) {
		Font_Threed(pattern,dwr,h);
	}
*/
}
#ifndef USE_CHN_FONT
/* ------------------------------------------------------- */
/*	Font Processing Table for Korean Character	   */
/* ------------------------------------------------------- */
#define CHO		0
#define JUNG	1
#define JONG	2

/*							초성, 중성, 종성 				*/
ROMDATA INT8U bul_cntA[3] = {   8,    4,    4};	//벌수 8x4x4
ROMDATA INT8U bul_cntB[3] = {   7,    2,    2};	//벌수 	7x2x2
ROMDATA INT8U chr_cnt[3] = {  20,   22,   28};	//문자수  

ROMDATA INT8U table_cho[21] =
/*	 x,fill,ㄱ,ㄲ,ㄴ,ㄷ,ㄸ,ㄹ,ㅁ,ㅂ,ㅃ,ㅅ,ㅆ,ㅇ,ㅈ,ㅉ,ㅊ,ㅋ,ㅌ,ㅍ,ㅎ  */
	{0,   0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,16,17,18,19};
ROMDATA INT8U table_joong[30] =
/*   x,x,fill,ㅏ,ㅐ,ㅑ,ㅒ,ㅓ,x,x,ㅔ,ㅕ,ㅖ,ㅗ,ㅘ,ㅙ,x,x,ㅚ,ㅛ,ㅜ,ㅝ,ㅞ,ㅟ,x,x,ㅠ,ㅡ,ㅢ,ㅣ  */
	{0,0,   0, 1, 2, 3, 4, 5,0,0, 6, 7, 8, 9,10,11,0,0,12,13,14,15,16,17,0,0,18,19,20,21};
ROMDATA INT8U table_jong[30] =
/*   x,fill,ㄱ,ㄲ,ㄳ,ㄴ,ㄵ,ㄶ,ㄷ,ㄹ,ㄺ,ㄻ,ㄼ,ㄽ,ㄾ,ㄿ,ㅀ,ㅁ,x,ㅂ,ㅄ,ㅅ,ㅆ,ㅇ,ㅈ,ㅊ,ㅋ,ㅌ,ㅍ,ㅎ  */
	{0,   0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,16,0,17,18,19,20,21,22,23,24,25,26,27};
// 8x4x4 type = 1
ROMDATA INT8U bul_choA1[22] =
/*  종성이 없을 경우, 중성에 따른 초성*/
/*   fill,ㅏ,ㅐ,ㅑ,ㅒ,ㅓ,ㅔ,ㅕ,ㅖ,ㅗ,ㅘ,ㅙ,ㅚ,ㅛ,ㅜ,ㅝ,ㅞ,ㅟ,ㅠ,ㅡ,ㅢ,ㅣ  */
	{   0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 3, 3, 3, 1, 2, 4, 4, 4, 2, 1, 3, 0};
ROMDATA INT8U bul_choA2[22] =
/*  종성이 있을 경우, 중성에 따른 초성*/
/*   fill,ㅏ,ㅐ,ㅑ,ㅒ,ㅓ,ㅔ,ㅕ,ㅖ,ㅗ,ㅘ,ㅙ,ㅚ,ㅛ,ㅜ,ㅝ,ㅞ,ㅟ,ㅠ,ㅡ,ㅢ,ㅣ  */
	{   0, 5, 5, 5, 5, 5, 5, 5, 5, 6, 7, 7, 7, 6, 6, 7, 7, 7, 6, 6, 7, 5};
ROMDATA INT8U bul_jungA1[20] =
/*  종성이 없을 경우, 초성에 따른 중성*/
/*   fill,ㄱ,ㄲ,ㄴ,ㄷ,ㄸ,ㄹ,ㅁ,ㅂ,ㅃ,ㅅ,ㅆ,ㅇ,ㅈ,ㅉ,ㅊ,ㅋ,ㅌ,ㅍ,ㅎ  */
	{   0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1};
ROMDATA INT8U bul_jungA2[20] =
/*  종성이 있을 경우, 초성에 따른 중성*/
/*   fill,ㄱ,ㄲ,ㄴ,ㄷ,ㄸ,ㄹ,ㅁ,ㅂ,ㅃ,ㅅ,ㅆ,ㅇ,ㅈ,ㅉ,ㅊ,ㅋ,ㅌ,ㅍ,ㅎ  */
	{   0, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2, 3, 3, 3};
ROMDATA INT8U bul_jongA[22] =
/*  중성에 따른 종성  */
/*   fill,ㅏ,ㅐ,ㅑ,ㅒ,ㅓ,ㅔ,ㅕ,ㅖ,ㅗ,ㅘ,ㅙ,ㅚ,ㅛ,ㅜ,ㅝ,ㅞ,ㅟ,ㅠ,ㅡ,ㅢ,ㅣ  */
	{   0, 0, 2, 0, 2, 1, 2, 1, 2, 3, 0, 2, 1, 3, 3, 1, 2, 1, 3, 3, 1, 1};
// 7x2x2 type = 2
ROMDATA INT8U bul_choB1[22] =
/*  종성이 없을 경우, 중성에 따른 초성*/
/*   fill,ㅏ,ㅐ,ㅑ,ㅒ,ㅓ,ㅔ,ㅕ,ㅖ,ㅗ,ㅘ,ㅙ,ㅚ,ㅛ,ㅜ,ㅝ,ㅞ,ㅟ,ㅠ,ㅡ,ㅢ,ㅣ  */
	{   0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 3, 3, 3, 1, 2, 3, 3, 3, 2, 1, 3, 0};
ROMDATA INT8U bul_choB2[22] =
/*  종성이 있을 경우, 중성에 따른 초성*/
/*   fill,ㅏ,ㅐ,ㅑ,ㅒ,ㅓ,ㅔ,ㅕ,ㅖ,ㅗ,ㅘ,ㅙ,ㅚ,ㅛ,ㅜ,ㅝ,ㅞ,ㅟ,ㅠ,ㅡ,ㅢ,ㅣ  */
	{   0, 4, 4, 4, 4, 4, 4, 4, 4, 5, 6, 6, 6, 5, 5, 6, 6, 6, 5, 5, 6, 4};
ROMDATA INT8U bul_jungB1[20] =
/*  종성이 없을 경우, 초성에 따른 중성*/
/*   fill,ㄱ,ㄲ,ㄴ,ㄷ,ㄸ,ㄹ,ㅁ,ㅂ,ㅃ,ㅅ,ㅆ,ㅇ,ㅈ,ㅉ,ㅊ,ㅋ,ㅌ,ㅍ,ㅎ  */
	{   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
ROMDATA INT8U bul_jungB2[20] =
/*  종성이 있을 경우, 초성에 따른 중성*/
/*   fill,ㄱ,ㄲ,ㄴ,ㄷ,ㄸ,ㄹ,ㅁ,ㅂ,ㅃ,ㅅ,ㅆ,ㅇ,ㅈ,ㅉ,ㅊ,ㅋ,ㅌ,ㅍ,ㅎ  */
	{   0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
ROMDATA INT8U bul_jongB[22] =
/*  중성에 따른 종성  */
/*   fill,ㅏ,ㅐ,ㅑ,ㅒ,ㅓ,ㅔ,ㅕ,ㅖ,ㅗ,ㅘ,ㅙ,ㅚ,ㅛ,ㅜ,ㅝ,ㅞ,ㅟ,ㅠ,ㅡ,ㅢ,ㅣ  */
	{   0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0};

/* 8 * 4 * 4벌
 초성
      초성 1벌 : 받침없는 'ㅏㅐㅑㅒㅓㅔㅕㅖㅣ' 와 결합
      초성 2벌 : 받침없는 'ㅗㅛㅡ'
      초성 3벌 : 받침없는 'ㅜㅠ'
      초성 4벌 : 받침없는 'ㅘㅙㅚㅢ'
      초성 5벌 : 받침없는 'ㅝㅞㅟ'
      초성 6벌 : 받침있는 'ㅏㅐㅑㅒㅓㅔㅕㅖㅣ' 와 결합
      초성 7벌 : 받침있는 'ㅗㅛㅜㅠㅡ'
      초성 8벌 : 받침있는 'ㅘㅙㅚㅢㅝㅞㅟ'
 중성
      중성 1벌 : 받침없는 'ㄱㅋ' 와 결합
      중성 2벌 : 받침없는 'ㄱㅋ' 이외의 자음
      중성 3벌 : 받침있는 'ㄱㅋ' 와 결합
      중성 4벌 : 받침있는 'ㄱㅋ' 이외의 자음
 종성
      종성 1벌 : 중성 'ㅏㅑㅘ' 와 결합
      종성 2벌 : 중성 'ㅓㅕㅚㅝㅟㅢㅣ'
      종성 3벌 : 중성 'ㅐㅒㅔㅖㅙㅞ'
      종성 4벌 : 중성 'ㅗㅛㅜㅠㅡ'
*/
/* 7 * 2 * 2벌	???????Is it correct???
 초성
      초성 1벌 : 받침없는 'ㅏㅐㅑㅒㅓㅔㅕㅖㅣ' 와 결합
      초성 2벌 : 받침없는 'ㅗㅛㅡ'
      초성 3벌 : 받침없는 'ㅜㅠ'
      초성 4벌 : 받침없는 'ㅘㅙㅚㅢㅝㅞㅟ'
      초성 5벌 : 받침있는 'ㅏㅐㅑㅒㅓㅔㅕㅖㅣ' 와 결합
      초성 6벌 : 받침있는 'ㅗㅛㅜㅠㅡ'
      초성 7벌 : 받침있는 'ㅘㅙㅚㅢㅝㅞㅟ'
 중성
      중성 1벌 : 받침없는 
      중성 2벌 : 받침있는 
 종성
      종성 1벌 : 중성 'ㅏㅐㅑㅒㅓㅔㅕㅖㅣㅘㅙㅚㅢㅝㅞㅟ' 와 결합 
      종성 2벌 : 중성 'ㅗㅛㅜㅠㅡ' 와 결합
*/

ROMDATA	INT8U	X_12x12Pattern[24]=
{
0x00,0x00,0x7F,0xE0,0x40,0x20,0x40,0x20,0x40,0x20,0x40,0x20,0x40,0x20,0x40,0x20,
0x40,0x20,0x40,0x20,0x7F,0xE0,0x00,0x00
};

ROMDATA	INT8U	X_16x16Pattern[32]=
{
0x00,0x00,0x7F,0xFE,0x40,0x02,0x40,0x02,0x40,0x02,0x40,0x02,0x40,0x02,0x40,0x02,
0x40,0x02,0x40,0x02,0x40,0x02,0x40,0x02,0x40,0x02,0x40,0x02,0x7F,0xFE,0x00,0x00
};

ROMDATA	INT8U	X_24x24Pattern[72]=
{
0x00,0x00,0x00,0x7F,0xFF,0xFE,0x40,0x00,0x02,0x40,0x00,0x02,0x40,0x00,0x02,0x40,0x00,0x02,
0x40,0x00,0x02,0x40,0x00,0x02,0x40,0x00,0x02,0x40,0x00,0x02,0x40,0x00,0x02,0x40,0x00,0x02,
0x40,0x00,0x02,0x40,0x00,0x02,0x40,0x00,0x02,0x40,0x00,0x02,0x40,0x00,0x02,0x40,0x00,0x02,
0x40,0x00,0x02,0x40,0x00,0x02,0x40,0x00,0x02,0x40,0x00,0x02,0x7F,0xFF,0xFE,0x00,0x00,0x00
};

void HanJohabPattern(HUGEDATA INT8U *ScratchPad,INT16U Kcode, char rot)
{ 
  	INT8U cho_5bit, jung_5bit, jong_5bit,i;
	INT8U cho_bul, jung_bul, jong_bul, jong_flag,font_bytes;
	INT16U character,pre_rot_font_offset;
	HUGEDATA INT8U	*font_ptr;
	ROMDATA INT8U *bul_cnt,*bul_cho1,*bul_cho2,*bul_jung1,*bul_jung2,*bul_jong;

  	font_bytes = FontStruct.FHbits2 * FontStruct.FWBytes2;

	//HYP 20040513
	if(Kcode == 0xffff) {
		//HYP 20040527
		if(FontStruct.FHbits2 == 12){ //Use only 24x24, 16x16, 12x12
			bul_cnt=X_12x12Pattern;
		} else if(FontStruct.FHbits2 == 16) {
			bul_cnt=X_16x16Pattern;
		} else {
			bul_cnt=X_24x24Pattern;
		} 
		for(i=0; i<font_bytes; i++) {
			ScratchPad[i]=bul_cnt[i];
		}
		return;
	}
	  
  	cho_5bit = table_cho[(Kcode >> 10) & 0x001F];		/* get cho, joong, jong 5 bit */
  	jung_5bit = table_joong[(Kcode >> 5) & 0x001F];
  	jong_5bit = table_jong[Kcode & 0x001F];

	switch(FontStruct.Font2Type) {
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

   	if(jong_5bit == 0)	{
  		jong_flag = 0;
      	cho_bul = bul_cho1[jung_5bit];
      	jung_bul = bul_jung1[cho_5bit];
  	} else { 
   		jong_flag = 1;
      	cho_bul = bul_cho2[jung_5bit];
  	    jung_bul = bul_jung2[cho_5bit];
   		jong_bul = bul_jong[jung_5bit];
    }

	if (rot == DEG_0) {
		pre_rot_font_offset = 0;
	} else {
		pre_rot_font_offset = FontStruct.TotalCharNum2;
//		pre_rot_font_offset = bul_cnt[CHO]*chr_cnt[CHO] + bul_cnt[JUNG]*chr_cnt[JUNG] +bul_cnt[JONG]*chr_cnt[JONG];
	}

// cho sung
  	character = cho_bul*chr_cnt[CHO] + cho_5bit + pre_rot_font_offset;			
  	font_ptr = FontStruct.PtrFont2 + (INT32U)character * font_bytes;
  	for(i = 0; i < font_bytes; i++) {
	   	ScratchPad[i] = font_ptr[i];
	}
// jung sung
	character = bul_cnt[CHO]*chr_cnt[CHO] + jung_bul*chr_cnt[JUNG] + jung_5bit + pre_rot_font_offset;		
  	font_ptr = FontStruct.PtrFont2 + (INT32U)character * font_bytes;
  	for(i = 0; i < font_bytes; i++) {
	 	ScratchPad[i] |= font_ptr[i];
	}
// jong sung
	if(jong_flag == 1)	{
		character = bul_cnt[CHO]*chr_cnt[CHO] + bul_cnt[JUNG]*chr_cnt[JUNG] +jong_bul*chr_cnt[JONG] + jong_5bit + pre_rot_font_offset;
		font_ptr = FontStruct.PtrFont2 + (INT32U)character * font_bytes;
      	for(i = 0; i < font_bytes; i++) {
      	  	ScratchPad[i] |= font_ptr[i];
		}
    }
}

void HanSpecialPattern(HUGEDATA INT8U *ScratchPad,INT16U Kcode, char rot)
{ 
	INT8U i,font_bytes;
	ROMDATA INT8U *FontPtr;

  	font_bytes = FontStruct.FHbits2 * FontStruct.FWBytes2;

	if(Kcode == 0xA1C9) {					//℃ in cp949

		if (rot == DEG_0) {
			if(FontStruct.FHbits2 == 12){ //Use only 24x24, 16x16, 12x12
				FontPtr=FontCDeg_DEG0_12x12;
			} else if(FontStruct.FHbits2 == 16) {
				FontPtr=FontCDeg_DEG0_16x16;
			} else {
				FontPtr=FontCDeg_DEG0_24x24;
			} 
		} else {//DEG_90
			if(FontStruct.FHbits2 == 12){ //Use only 24x24, 16x16, 12x12
				FontPtr=FontCDeg_DEG90_12x12;
			} else if(FontStruct.FHbits2 == 16) {
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
			if(FontStruct.FHbits2 == 12)
			{ 			
				FontPtr=FontPOUND_DEG0_12x12;		
			} 
			else if(FontStruct.FHbits2 == 16) 
			{
				FontPtr=FontPOUND_DEG0_16x16;
			} 
			else 
			{
				FontPtr=FontPOUND_DEG0_24x24;
			} 
		} 
		else 	//DEG_90
		{
			if(FontStruct.FHbits2 == 12)
			{ 
				FontPtr=FontPOUND_DEG90_12x12;
			} 
			else if(FontStruct.FHbits2 == 16) 
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
#endif	//#ifdef USE_KOREAN_IN_GBR
}
#endif	//#ifndef USE_CHN_FONT


#include "bar_common.h"
ROMDATA INT8U OcrB_Hbits_table[] = {16,14,10};
ROMDATA INT8U Bar_char_width_table[] = 
/*  0		1		2		3	4		5		6	7		8	9		10		11		12	*/
/*	EAN13 EAN13P2 EAN13P5 EAN8 EAN8P2 EAN8P5 UPCA UPCAP2 UPCAP5 UPCE0 UPCE0P2 UPCE0P5 UPCE1 */
{	7,		7,		7,		7,	7,		7,		7,	7,		7,	7,		7,		7,		7,
/*  13		14		15		16			17		18		19		20		21		   22	   */
/*  CODE128A CODE128B CODE128C CODE128X CODE93 CODE93FULL CODE11  CODE39 CODE39FULL   */
	11,		 11,	  5,	   	11,		9,		9,		   7,		12,	 12,	 	
/*	23	24		25		26		27		28		29	*/
/*  I2OF5  S2OF5 M2OF5 IATA2OF5 CODABAR MSI	 PLESSEY   POSTNET*/
	7,	   12,	 12,   12, 		9,		12,	 20, 		7};

//void FontLoadOcrB(void) {	 //only numeric
//HYP 20050528
void FontLoadOcrB(short bwidth, short char_num) {	 //only numeric
	char i,j,end_flag,bi,bj;
	INT8U char_width;
//	char string_buf[30];

	Font1Struct[OCRB_FONT_ID].Wbits1 = 16;
	Font1Struct[OCRB_FONT_ID].FHbits1 = 16;
	Font1Struct[OCRB_FONT_ID].FWBytes1 = 2;
	Font1Struct[OCRB_FONT_ID].Baseline1 = 0;
	Font1Struct[OCRB_FONT_ID].FirstCode1 = 0x30;
	Font1Struct[OCRB_FONT_ID].PreRot1 = PREROT_ONLY_0DEG;
	Font1Struct[OCRB_FONT_ID].TotalCharNum1 = 10;
	Font1Struct[OCRB_FONT_ID].ExistFont = 1;

	end_flag = 0;
	//char_width = Bar_char_width_table[BarStruct.BarType]*(INT8U)BarStruct.nb;
	//HYP 20050528
	if(bwidth == 0) {//EAN,UPC
		char_width = Bar_char_width_table[BarStruct.BarType]*(INT8U)BarStruct.nb;
	} else {
		char_width = bwidth/char_num;
	}

	StrStruct.Magx = 1;
	StrStruct.Magy = 1;
	StrStruct.InterChSp = 0;
	bi = 1;
	bj = 2;

	for (j=0;j<3;j++) {
		for(i=3;i>0;i--) {
			if(*(FontAddrOcrBW + j*10)*i <= char_width) {
				end_flag = 1;
				bi = i;
				bj = j;
			 	break;
			}
	   	}
	   	if(end_flag == 1) {
	   		break;
	   	}
	}
  	Font1Struct[OCRB_FONT_ID].PtrFont1 = FontAddrOcrB +bj * (10*32);
  	Font1Struct[OCRB_FONT_ID].PtrWidth1 = FontAddrOcrBW +bj * 10;	
	
//Insert Fixed Font 20031218
	Font1Struct[OCRB_FONT_ID].FixedWbits1 = *(Font1Struct[OCRB_FONT_ID].PtrWidth1);
	Font1Struct[OCRB_FONT_ID].FixedWidthFlag1 = 0;

  	StrStruct.Magx = bi;
	Font1Struct[OCRB_FONT_ID].Hbits1 = OcrB_Hbits_table[(INT8U)bj];
  	StrStruct.Magy = BarStruct.CharHeight/Font1Struct[OCRB_FONT_ID].Hbits1;
	if((FontStruct.Hbits1 * StrStruct.Magy) < StrStruct.StrHeight) {
		StrStruct.StrHeight = Font1Struct[OCRB_FONT_ID].Hbits1 * StrStruct.Magy;
	}
	StrStruct.InterChSp = char_width - (*(Font1Struct[OCRB_FONT_ID].PtrWidth1)*bi);//20040209
//	StrStruct.InterChSp = char_width - *(Font1Struct[OCRB_FONT_ID].PtrWidth1);
	FontLoadFont1(OCRB_FONT_ID);
}

//Insert Fixed Font 20031218
//only fixed width
//INT8U FontLoadHri(void)
//HYP 20050528
INT8U FontLoadHri(short bwidth, short char_num)
{
//	char i,j,mx,my;
	char mx,my;
	INT8U char_width;
// char string_buf[30];

	//char_width = Bar_char_width_table[BarStruct.BarType]*(INT8U)BarStruct.nb;
	//HYP 20050528
	char_width = bwidth/char_num;
	// width
  	mx = char_width/FontStruct.FixedWbits1;
	// height
  	my = BarStruct.CharHeight/FontStruct.Hbits1;
	if((FontStruct.Hbits1 * my) < StrStruct.StrHeight) {
		StrStruct.StrHeight = FontStruct.Hbits1 * my;
	}
	if(mx == 0 || my == 0) return 0xff;
	//interch space
	StrStruct.InterChSp = char_width - (FontStruct.FixedWbits1*mx);//20040209
//	StrStruct.InterChSp = char_width - FontStruct.FixedWbits1;
	StrStruct.Magx = mx;
	StrStruct.Magy = my;
	return 0;
}

