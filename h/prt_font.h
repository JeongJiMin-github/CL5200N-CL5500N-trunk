/*****************************************************************************
|*			
|*  Copyright		:	(c) 2002 CAS
|*  Filename		:	prt_font.h
|*  Version			:	0.1
|*  Programmer(s)	:	Hong Yi-Phyo (HYP)
|*  Created			:	2002/11/11
|*  Description		:	    
|*				
****************************************************************************/

#ifndef _FONT_H
#define _FONT_H

#include "bar_common.h"

#define MAX_STR_FIELD_NUM	240//200	// by CJK for Total Label Field
#define	LEFT		0
#define	RIGHT		1
#define CENTER		2
#define	TWO_BYTE_FONT_STR	1
#define	ONE_BYTE_FONT_STR	0

#define	NO_TYPEFACE		0x00
#define ITALIC 			0x10
#define OUTLINE			0x08	
#define UNDERLINE		0x01
#define THROUGH_LINE	0x02
#define	DTHROUGH_LINE	0x04
#define BOLD			0x40
#define SHADOW			0x20
#define REVERSE			0x80

#define	ONE_BYTE_FONT			0
#define	TWO_BYTE_FONT_WAN		1
#define	TWO_BYTE_FONT_JO8X4X4	2
#define	TWO_BYTE_FONT_JO7X2X2	3  

#define PREROT_ONLY_0DEG		0
#define PREROT_0DEG_90DEG		1
#define PREROT_ONLY_90DEG		2

#define	DEG_0	0
#define	DEG_90	1
#define	DEG_180	2
#define	DEG_270	3
#define	DEG_360	4
#define	DEG_450	5
#define	DEG_540	6
#define	DEG_630	7
#define	MASK_DEG_90_270	0x01 

#define MAX_FONT1_NUM	10
#define MAX_FONT2_NUM	10

#define OCRB_FONT_ID	MAX_FONT1_NUM - 1

#ifdef USE_PERSIA_DEFAULT
#define DEFAULT_RRT_FONT_ID	6
#else
#define DEFAULT_RRT_FONT_ID	3
#endif

#define SCRATCH_PAD_SIZE	80*8*50 //1line*60mm
#define SCRATCH_PAD_SIZE_FONT	6*48 //48x48 font (max)
#define MAX_STR_CHAR_SIZE	2560//2000
#define MAX_HTAB_NUM		10
#define MAX_VTAB_NUM		10

#define PRT_16x16_VAR_WIDTH_FONT_ID1	0	//16x16 one byte variable font
#define PRT_24x24_VAR_WIDTH_FONT_ID1	1	//24x24 one byte variable font
#define PRT_16x16_FIXED_WIDTH_FONT_ID2	0	//16x16 two byte fixed font
#define PRT_24x24_FIXED_WIDTH_FONT_ID2	1	//24x24 two byte fixed font
#pragma pack(2)
typedef struct {
	INT8U TypeOfFile[2]; 					//0~1
	INT8U Version[2];						//2~3
	INT8U Name[20];							//4~23
	INT16U	BitmapOffset;					//24~25
	INT16U	WidthTableOffset;				//26~27
	char	FHbits;							//28
	char	FWBytes;						//29
	char	Hbits;							//30
	char 	Wbits;	//0: variable width		//31
	char 	Baseline;						//32
	char 	PreRot;							//33
	char 	PreFlip;						//34
	char 	ChSetType;						//35
	char 	DefaultStrRot;					//36
	char 	DefaultStrDir;					//37
	INT16U	DefaultCh;						//38~39
	INT16U	FirstCode;						//40~41
	INT16U	TotalCharNum;					//42~43
//Insert Fixed Font 20031218
//	char 	SpWidth;						//44
	char 	FixedWbits;						//44
	char 	StrikeRowIdx;					//45
	INT8U	reserved[18];					//46~63
}FONT_FILE_STRUCT;
#pragma pack()
#pragma pack(4)
typedef struct {
	HUGEDATA INT8U *PtrFont1, *PtrFont2;
	HUGEDATA INT8U *PtrWidth1;
	INT8U	Font2Type;
	char FHbits1, FHbits2;
	char FWBytes1, FWBytes2;
	char Hbits1, Hbits2;
	char Wbits1, Wbits2;
	char PreHFlip1, PreHFlip2;  //font1
	char PreRot1, PreRot2;  //font2
	INT16U	TotalCharNum1;
	INT16U	TotalCharNum2; 
	char Baseline1, Baseline2;
	INT8U FirstCode1;
	INT16U FirstCode2;
//Insert Fixed Font 20031218
	char  FixedWbits1;
	BOOLEAN FixedWidthFlag1;						
//	char StrikeRowIdx;	//font1,2
//	char ChSetType1, ChSetType2;
//	char DefaultStrRot, DefaultStrDir;	//font1,2
//	char DefaultCh;	//font1,2
//	char HighDCode;		//font2
}FONT_STRUCT;


typedef struct {
	HUGEDATA char *Name1;
	HUGEDATA INT8U *PtrFont1;
	HUGEDATA INT8U *PtrWidth1;
	char FHbits1, FWBytes1;
	char Hbits1, Wbits1;
	char PreHFlip1, PreRot1;  //font1
	INT16U	TotalCharNum1;
	char Baseline1;
	char ChSetType1;
	INT8U FirstCode1;
//Insert Fixed Font 20031218
	char  FixedWbits1;
	BOOLEAN FixedWidthFlag1;
//	char DefaultStrRot, DefaultStrDir;	//font1,2
//	char DefaultCh;	//font1,2
//	char StrikeRowIdx;	//font1,2
	BOOLEAN ExistFont;
}FONT1_STRUCT;

typedef struct {
	HUGEDATA char *Name2;
	HUGEDATA INT8U *PtrFont2;
	INT8U	Font2Type;
	char FHbits2, FWBytes2;
	char Hbits2, Wbits2;
	char PreHFlip2, PreRot2;  //font2
	INT16U	TotalCharNum2; 
	char Baseline2; 
	char ChSetType2;
	INT16U FirstCode2;
//	char DefaultStrRot, DefaultStrDir;	//font1,2
//	char DefaultCh;	//font1,2
//	char HighDCode;		//font2
//	char StrikeRowIdx;	//font1,2
	BOOLEAN ExistFont;
}FONT2_STRUCT;

typedef struct {
//not save for initial
	short HTab[MAX_HTAB_NUM];   //tab position use only receipt mode
//	short VTab[MAX_VTAB_NUM];	//tab position use only receipt mode
//save for initial
	INT8U Id1, Id2;	//Font ID1,2
	short StrWidth, StrHeight;
	char ChRot;
	char LineSp;
	char Justification;
	char InterChSp;
	char Typeface;
	char Magx,Magy;
	INT8U StrMirx;//, StrMiry;
	BOOLEAN WordWrap;
	BOOLEAN StrType;
	BOOLEAN	AutoShrinkx;
	BOOLEAN VCentering;
	BOOLEAN EnableExpandByte;
//Insert Fixed Font 20031218
	BOOLEAN UseFixedWidth;
	INT8U HTabCh[MAX_HTAB_NUM];   //tab position use only receipt mode
//	INT8U VTabCh[MAX_VTAB_NUM];	//tab position use only receipt mode
}STR_STRUCT;

typedef struct {
	char StrRot;
	short sx, sy;
	INT8U Id1, Id2;	//Font ID1,2
	short StrWidth, StrHeight;
	char ChRot;
	char LineSp;
	char Justification;
	char InterChSp;
	char Typeface;
	char Magx,Magy;
	INT8U StrMirx;//, StrMiry;
	BOOLEAN WordWrap;
	BOOLEAN StrType;
	BOOLEAN	AutoShrinkx;
	BOOLEAN VCentering;
	BOOLEAN EnableExpandByte;
//Insert Fixed Font 20031218
	BOOLEAN UseFixedWidth;
	BOOLEAN ExistField;
}STR_FIELD_STRUCT;
#pragma pack()
//for Korean Mode 
//Centi Deg. Font for LCD, PRT
extern ROMDATA INT8U FontCDeg_DEG0_12x12[2*12];
extern ROMDATA INT8U FontCDeg_DEG90_12x12[2*12];
extern ROMDATA INT8U FontCDeg_DEG0_16x16[2*16];
extern ROMDATA INT8U FontCDeg_DEG90_16x16[2*16];
extern ROMDATA INT8U FontCDeg_DEG0_24x24[3*24];
extern ROMDATA INT8U FontCDeg_DEG90_24x24[3*24];
//POUND Font for LCD, PRT(PARAMETER 980 set)
extern ROMDATA INT8U FontPOUND_DEG0_12x12[2*12];
extern ROMDATA INT8U FontPOUND_DEG90_12x12[2*12];
extern ROMDATA INT8U FontPOUND_DEG0_16x16[2*16];
extern ROMDATA INT8U FontPOUND_DEG90_16x16[2*16];
extern ROMDATA INT8U FontPOUND_DEG0_24x24[3*24];
extern ROMDATA INT8U FontPOUND_DEG90_24x24[3*24];

//extern HUGEDATA STR_STRUCT StrStruct;
//extern HUGEDATA FONT_STRUCT FontStruct;
//HYP 20060412 _huge --> _near for small code size
extern NEARDATA STR_STRUCT  StrStruct;
extern NEARDATA FONT_STRUCT FontStruct;
extern HUGEDATA STR_FIELD_STRUCT StrFieldStruct[MAX_STR_FIELD_NUM+MAX_BAR_FIELD_NUM];

extern HUGEDATA FONT1_STRUCT Font1Struct[MAX_FONT1_NUM];
extern HUGEDATA FONT2_STRUCT Font2Struct[MAX_FONT2_NUM];

//HYP 20060424 Fix Byte Align Problem for rendering
//extern HUGEDATA INT8U	PrtScratchPadMemory[SCRATCH_PAD_SIZE];
//extern HUGEDATA INT8U	PrtScratchPadFontMemory[SCRATCH_PAD_SIZE_FONT];
extern HUGEDATA INT16U	PrtScratchPadMemory[SCRATCH_PAD_SIZE/2];
extern HUGEDATA INT16U	PrtScratchPadFontMemory[SCRATCH_PAD_SIZE_FONT/2];

extern HUGEDATA char	PrtStrBufferMemory[MAX_STR_CHAR_SIZE];
#ifdef USE_QR_ADDTOTAL
extern HUGEDATA char	PrtBarBufferMemory[MAX_BAR_DATA_BUF_SIZE_QR];
#else
extern HUGEDATA char	PrtBarBufferMemory[MAX_BAR_DATA_BUF_SIZE];
#endif
#ifdef USE_ARAB_FONT
 #ifdef USE_ENG_BASED_ARAB_SUPPORT
 extern INT16U  eng_codetemp[1024];
 #else
 extern INT16U  arab_codetemp[1024];
 #endif
#endif
#ifdef HEBREW_FONT
extern INT16U  	hebrew_codetemp[1024];
#endif

//HYP 20040622
extern HUGEDATA INT8U *DspFontAddrHpf;
extern HUGEDATA INT8U *Dsp2byteFontAddrHpf;
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
extern HUGEDATA INT8U *DspFontAddr_3_8x16;
extern HUGEDATA INT8U *DspFontAddr_4_12x24;
  #ifdef _USE_LCD32_ 			// CL5500 DISP에만 적용
    #ifdef USE_KOR_FONT			// 한글폰트
extern HUGEDATA INT8U *DspFontAddr_4_24x24;
	#endif
  #endif
extern HUGEDATA INT8U *DspFontAddr12x8Hpf;
extern HUGEDATA INT8U *DspFontAddr20x8Hpf;
extern HUGEDATA INT8U *DspFontAddr16x8Hpf;  //20120914 for CL5200 Arab version by kiho
#if defined(USE_ARAB_FONT) || defined(HEBREW_FONT)
extern HUGEDATA INT8U *DspFontAddr6x8ENGHpf;
#endif
#endif

#ifdef HEBREW_FONT
extern INT16U Convert_HebrewPreview(INT8U *str, INT16U leng, INT16U cur_pt, INT8U firstInputFlag);
#endif

extern void FontStrOut(HUGEDATA char *buf);
#ifdef USE_VIETNAM_FONT
extern INT16U VietnamCode_buf[1024];
extern INT16U Cvt_VietnamPreview(INT8U *str, INT16U leng, INT16U cur_pt, INT8U mode);
extern void FontCharOut(INT16U ch, char rot);
#elif defined(USE_THAI_FONT)
extern INT32U ThaiCode_buf[1024];
extern INT16U Cvt_ThaiPreview(INT8U *str, INT16U leng, INT16U cur_pt, INT8U mode);
extern INT8U Cvt_ThaiStrLength(INT8U *str);
extern void FontThaiCharOut(INT32U ch, char rot);
#else
#if defined(USE_ARAB_FONT) || defined(HEBREW_FONT)
extern void FontCharOut(INT16U ch, char rot);
#else
extern void FontCharOut(char ch,char rot);
#endif
#endif
extern void FontDCharOut(INT16U chw,char rot);
extern void FontInit(void);
extern void StrInit(void);
extern void StrFieldInit(void);
//extern void FontLoadOcrB(void);
//HYP 20050528
extern void FontLoadOcrB(short bwidth, short char_num);
//Insert Fixed Font 20031218
//extern INT8U FontLoadHri(void);
//HYP 20050528
extern INT8U FontLoadHri(short bwidth, short char_num);
extern INT8U FontScanFont(INT8U id, HUGEDATA FONT_FILE_STRUCT   *FontFileStruct);
extern void FontLoadFont1(INT8U id);
extern void FontLoadFont2(INT8U id);
extern void FontSaveField(INT8U field_num);
extern INT8U FontLoadField(INT8U field_num);
//HYP 20040903
extern INT8U FontEraseFieldArea(INT8U field_num);
extern void BitmapOut(INT16U id);
extern void Font_getdefaultid(void);
extern INT8U label_load(INT16U id);
extern INT8U label_defaultload(void);
extern void label_set_defaultid(INT16U id);
extern INT8U label_clear(void);
//INT8U bitmap_clear(INT16U id);
extern INT8U bitmap_clear(void);

extern void HanJohabPattern(HUGEDATA INT8U *ScratchPad,INT16U Kcode, char rot);
extern void HanSpecialPattern(HUGEDATA INT8U *ScratchPad,INT16U Kcode, char rot);

#endif /* _FONT_H */
