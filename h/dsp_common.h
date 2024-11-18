/*****************************************************************************
|*			
|*  Copyright		:	(c) 2002 CAS
|*  Filename		:	dsp_common.h
|*  Version			:	0.1
|*  Programmer(s)	:	Cho Dae Kun (chodk)
|*  Created			:	2003/05/20
|*  Description		:	CLP BASIC LCD global variable define    
|*				
 ****************************************************************************/

#ifndef _DSP_COMMON_H
#define _DSP_COMMON_H

#define V256x64	0x01
#define V256x32 0x02
#define LCD		0x03

#define DSP_MAX_PAGE	3
#define DSP_DEFAULT_PAGE	0
#define DSP_ERROR_PAGE		1
#define DSP_PSWD_PAGE		2

#ifdef _USE_LCD32_
#define LCD_X_MARGIN			32
#ifdef _USE_LCD32_7_
#define LCD_Y_MARGIN			160
#else
#define LCD_Y_MARGIN			202
#define LCD_Y_MARGIN_REAL		LCD_Y_MARGIN + 6
#endif
#define DSP_BUF_SIZE			(LCD_Y_MARGIN*(LCD_X_MARGIN/8))	//808
#define DSP_SCRATCH_PAD_SIZE	(LCD_X_MARGIN*(LCD_X_MARGIN/8))	//128
#endif

#if defined (_USE_LCD37D_40DOT_)
//TODO: check
#define LCD_X_MARGIN			8
#define LCD_X_MARGIN_REAL		8
#define LCD_Y_MARGIN			60
#define LCD_Y_MARGIN_REAL		60
#elif defined (_USE_LCD26d_16x96_)
#define LCD_X_MARGIN			16
#define LCD_X_MARGIN_REAL		16
#define LCD_Y_MARGIN			96
#define LCD_Y_MARGIN_REAL		96
#elif defined(_USE_LCD20848_)
#define LCD_X_MARGIN			32
#define LCD_X_MARGIN_REAL		48
#define LCD_Y_MARGIN			202
#define LCD_Y_MARGIN_REAL		208
#endif
#define DSP_BUF_SIZE			(LCD_Y_MARGIN*(LCD_X_MARGIN/8))             // ( 60 * (8/8) = 60 )
#define DSP_SCRATCH_PAD_SIZE	(LCD_X_MARGIN*(LCD_X_MARGIN/8))             // ( 8 * (8/8) = 8 )
#define DSP_BUF_SIZE_REAL		(LCD_Y_MARGIN_REAL*(LCD_X_MARGIN_REAL/8))   // ( 60 * (8/8) = 60 )

#if defined (_USE_LCD20848_) || defined (_USE_LCD32_)
#define MAX_DSP_FONT1_NUM	10//5
#define MAX_DSP_FONT2_NUM	10//5
#else
#define MAX_DSP_FONT1_NUM	1//10//5
#define MAX_DSP_FONT2_NUM	1//10//5
#endif
#define CALLBACK_DISPLAY  14  //se-hyung 20071008

#define DSP_SALE_MODE	0
#define DSP_PGM_MODE	1

// use only for lcd320240
#define DSP_PRTSTR_BACKUP	1
#define DSP_PRTSTR_RESTORE	0
///////////////////

//#define DSP_6x8_FIXED_WIDTH_FONT_ID1	0
//#define DSP_12x24_FIXED_WIDTH_FONT_ID1	1
//#define DSP_16x16_VAR_WIDTH_FONT_ID1	2

// #define DSP_16x16_FIXED_WIDTH_FONT_ID2	DSP_6x8_FIXED_WIDTH_FONT_ID1
// #define DSP_24x24_FIXED_WIDTH_FONT_ID2	DSP_12x24_FIXED_WIDTH_FONT_ID1
#ifdef USE_ARAB_FONT
#ifdef USE_ENG_BASED_ARAB_SUPPORT
   extern INT16U   eng_codetemp[1024];
   extern INT16U   arab_revtemp[1024];
#endif
    extern INT16U 	arab_subtemp[1024];
    extern INT16U  arab_codetemp[1024];
    extern INT8U 	arab_engtemp[1024];
#endif

#ifdef HEBREW_FONT
extern INT16U  	hebrew_codetemp[1024];
extern INT8U 	hebrew_engtemp[1024];
extern INT8U 	hebrew_subtemp[1024];
#endif

#if defined (_USE_LCD20848_) || defined (_USE_LCD32_) //rev2852 아랍관련 수정사항 체크 필요
#if defined (USE_ARAB_FONT) || defined (HEBREW_FONT)
extern INT8U DSP_6_BY_8_ENG_FONT_ID;
#endif
#endif

extern INT8U DSP_MENU_FONT_ID;
extern INT8U DSP_PLU_FONT_ID;
extern INT8U DSP_SYSTEM_FONT_ID;
#pragma pack(4)
typedef struct
{
	unsigned char chX;//0~256 => bit system
	unsigned char chY;//0~7   => charactor system
}CURSOR_ADDR;

typedef struct
{
	unsigned char chX;//0~256 => bit system
	unsigned char scrollY;
}DISP_START_ADDR;

typedef struct
{
    //unsigned short x;
	//unsigned char y;
	INT16S x;
	INT16S y;
}POINT;

typedef struct
{
    unsigned char height;
    unsigned char width;
}SIZE;

typedef struct {
	HUGEDATA char *Name1;
	HUGEDATA INT8U *PtrFont1;
	HUGEDATA INT8U *PtrWidth1;
	char FHbits1, FWBytes1;
	char Hbits1, Wbits1;
	char PreHFlip1, PreRot1;  //font1
	INT16U	TotalCharNum1;
	char ChSetType1;
	INT8U FirstCode1;
	char InterChSp;
	char FixedWidth;
	BOOLEAN ExistFont;
}DSP_FONT1_STRUCT;

typedef struct {
	HUGEDATA char *Name2;
	HUGEDATA INT8U *PtrFont2;
	INT8U	Font2Type;
	char FHbits2, FWBytes2;
	char Hbits2, Wbits2;
	char PreHFlip2, PreRot2;  //font2
	INT16U	TotalCharNum2; 
	char ChSetType2;
	INT16U FirstCode2;
	char InterChSp;
	char FixedWidth;
	BOOLEAN ExistFont;
}DSP_FONT2_STRUCT;

typedef struct {
//not save for initial
	char Id1,Id2;
	HUGEDATA char *Name1,*Name2;
	HUGEDATA INT8U *PtrFont1;
	HUGEDATA INT8U *PtrFont2;
	HUGEDATA INT8U *PtrWidth1;
	INT8U	Font2Type; //0=1 byte font 1=2 byte font,wansung0, 2=Johab 8x4x4, 3=Johab 7x2x2
	char FHbits1, FWBytes1;
	char FHbits2, FWBytes2;
	char Hbits1, Wbits1;
	char Hbits2, Wbits2;
	char PreHFlip1, PreRot1;  //font1
	char PreHFlip2, PreRot2;  //font1
	INT8U	Page;
	INT16U	TotalCharNum1;
	INT16U	TotalCharNum2; 
	char ChSetType1, ChSetType2;
	INT8U FirstCode1;
	INT16U FirstCode2;
	char DefaultCh;	//font1,2
	char HighDCode;		//font2
	char FixedWidth;
	char InterChSp;
	char Typeface;
	char Reverse;
	char DirectDraw;
	char InsertCur;
	char DeleteCur;
//save for initial
	char StrType;
	char Magx,Magy;
}DSP_STRUCT;
#pragma pack()

extern NEARDATA DSP_STRUCT DspStruct;
#ifdef _USE_LCD20848_ //ck
extern HUGEDATA INT8U DspBuf[DSP_MAX_PAGE][DSP_BUF_SIZE_REAL];
extern HUGEDATA INT8U DspScratchPad[DSP_SCRATCH_PAD_SIZE];
#else
extern HUGEDATA INT8U DspBuf[DSP_MAX_PAGE][DSP_BUF_SIZE];
extern HUGEDATA INT8U DspScratchPad[DSP_SCRATCH_PAD_SIZE];
#endif
extern POINT point(INT16S,INT16S);

extern void Dsp_InitHardware(void);
#if defined(USE_ARAB_FONT) || defined(HEBREW_FONT)
extern void Dsp_Write_1stringLong(POINT,HUGEDATA INT16U *str);
#endif
extern void Dsp_Write_1string(POINT,HUGEDATA char *str);
#ifdef USE_VIETNAM_FONT
extern void Dsp_Write_1string16(POINT p,HUGEDATA INT16U *str);
#endif
#ifdef USE_THAI_FONT
extern void Dsp_Write_1string32(POINT p, HUGEDATA INT32U *str);
#endif
extern void Dsp_Write_2string(POINT,HUGEDATA char *str);
extern void Dsp_Write_String(POINT,HUGEDATA char *str);
extern void Dsp_Diffuse(void);
extern void Dsp_Clear(void);
#ifdef USE_PLU_SCROLLING
extern void Dsp_Fill_Buffer_without_TOP_Msg(unsigned char x);
#endif
extern void Dsp_Fill_Buffer(unsigned char x);
extern void LCD_Put_1string(POINT, HUGEDATA char *strData);
extern void LCD_Put_2string(POINT, HUGEDATA char *strData);
extern void LCD_Put_String(POINT, HUGEDATA char *strData);

extern void wait_sleep(long period);

extern void Dsp_FontInit(void);
extern void Dsp_HanJohabPattern(HUGEDATA INT8U *Pattern,INT16U Kcode, char rot);
extern void Dsp_HanSpecialPattern(HUGEDATA INT8U *ScratchPad,INT16U Kcode, char rot);//HYP 20050304

extern INT8U DspScanFont(INT8U id, HUGEDATA FONT_FILE_STRUCT   *FontFileStruct);
extern void DspLoadFont1(INT8U id);
extern void DspLoadFont2(INT8U id);
extern void Dsp_FontSize(char mag_w, char mag_h);
extern INT16U Dsp_GetChars(void);
extern void Dsp_default_fontload(void);
//extern void wait_test_command(void);
extern void Dsp_SetPage(INT8U page);
extern void Dsp_ChangeMode(INT8U mode);

extern void Dsp_ReDraw(short sx, short sy, short ex, short ey);

extern void LCD_DrawBackgroundSaleMode(void);
extern void LCD_DrawBackgroundProgramMode(void);
extern void LCD_DrawBackgroundScrollMsg(void);

#include "dsp_lcd37d_40dot.h"
#include "dsp_lcd26d_16x96.h"
#include "dsp_lcd20848.h"
#ifdef _USE_LCD32_
#include "dsp_lcd32.h"
#endif//#ifdef _USE_LCD32_
#endif
