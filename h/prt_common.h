/*****************************************************************************
|*			
|*  Copyright		:	(c) 2002 CAS
|*  Filename		:	prt_common.h
|*  Version			:	0.1
|*  Programmer(s)	:	Hong Yi-Phyo (HYP)
|*  Created			:	2003/03/14
|*  Description		:	CLP Printer global variable define    
|*				
****************************************************************************/

#ifndef _PRT_COMMON_H
#define _PRT_COMMON_H

#include "prt_font.h"

//HYP 20040621
//#define FONT_CON_CH					'['
#define FONT_CON_CH					DC1

//2 bytes commands
#define 	FONT_CON_DEC_MAG			'-' 
#define 	FONT_CON_INC_MAG			'+'
#define 	FONT_CON_TYPE_ITALIC		'I' 
#define 	FONT_CON_TYPE_UNDERLINE		'U'
#define 	FONT_CON_TYPE_THROUGH_LINE	'T'
#define 	FONT_CON_TYPE_SHADOW		'S'
#define 	FONT_CON_TYPE_BOLD			'L'
#define 	FONT_CON_TYPE_REVERSE		'R'
#define 	FONT_CON_TYPE_DTHROUGH_LINE	'H'
#define 	FONT_CON_TYPE_OUTLINE		'O'
#define 	FONT_CON_TYPE_NO_TYPEFACE	'N'
//3 bytes commands
#define 	FONT_CON_DEG				'D'	//+ '0'~'3'
#define 	FONT_CON_MAG_X				'X'	//+ '1'~'5'
#define 	FONT_CON_MAG_Y				'Y'	//+ '1'~'5'
#define 	FONT_CON_FONT1				'A' //+ Font Id1
#define 	FONT_CON_FONT2				'B'	//+ Font Id2

//HYP 20040621
//#define BAR_2CH_SET_CH    	'['  // '[' ~ '_'
/* NULL SOH	STX ETX EOT ENQ ACK BEL BS  HT  LF  VT  FF  CR  SO  SI 			*/
/* @	A	B	C	D	E	F	G	H	I	J	K	L	M	N	O			*/
/* DLE DC1 DC2 DC3 DC4 NAK SYN ETB CAN EM  SUB ESC FC  GS  RS  US  DEL '['	*/
/* P   Q   R   S   T   U   V   W   X   Y   Z   [   \   ]   ^   _   '5' '0' 	*/
/* example : [J = LF  */
#define BAR_2CH_SET_CH    	'^'  // '[' ~ '_'
/* NULL SOH	STX ETX EOT ENQ ACK BEL BS  HT  LF  VT  FF  CR  SO  SI 			*/
/* @	A	B	C	D	E	F	G	H	I	J	K	L	M	N	O			*/
/* DLE DC1 DC2 DC3 DC4 NAK SYN ETB CAN EM  SUB ESC FC  GS  RS  US  DEL '^'	*/
/* P   Q   R   S   T   U   V   W   X   Y   Z   [   \   ]   ^   _   '5' '0' 	*/
/* example : ^J = LF  */

#ifdef USE_BARCODE_LEN_90
#define LINE_BUF_SIZE	120//120mm
#else
#define LINE_BUF_SIZE	80//80mm
#endif

#define RM_COMPLETE		0
#define RM_INIT			1
#define RM_RENDERING	2

#define CI_NOTCOMPLETE	0
#define CI_COMPLETE		1
#define CI_INVALID		2

#define FONT_USE_PRT		0
#define FONT_USE_DSP		1
#define FONT_USE_PRT_DSP	2

//Prt Flash Setting data offset
#define PRT_FLASH_BASE			(DFLASH_BASE + FLASH_PRINTER_AREA)		// modified by cjk
#define PRT_FLASH_SET_STRUCT_SIZE	0x100
#define PRT_FLASH_FONT_STRUCT_SIZE	0x100
#define PRT_FLASH_FONT_BASE		(PRT_FLASH_BASE + PRT_FLASH_SET_STRUCT_SIZE)

//PRT_STRUCT
#define FLASH_OFFSET_Mode						0
#define FLASH_OFFSET_LabelWidth					2
#define FLASH_OFFSET_LabelHeight				4
#define FLASH_OFFSET_ReceiptWidth				6
#define FLASH_OFFSET_ReceiptHeight				8
#define FLASH_OFFSET_TMargin					10
#define FLASH_OFFSET_BMargin					12
#define FLASH_OFFSET_LMargin					14
#define FLASH_OFFSET_RMargin	   				16
//PRTDRV_STRUCT
#define FLASH_OFFSET_GapLength					32
#define FLASH_OFFSET_StbTimeTrimValue			34
#define FLASH_OFFSET_Speed						36
#define FLASH_OFFSET_PeelEnable					37
#define FLASH_OFFSET_LabelGapMode				38
#define FLASH_OFFSET_RewindEnable				39
#define FLASH_OFFSET_PrePrintMode				40
#define FLASH_OFFSET_PrePrintLength				41
#define FLASH_OFFSET_ReceiptFeedHeight			42

//SENSOR_STRUCT
#define FLASH_OFFSET_GapBufInPtrTrimValue		64
#define FLASH_OFFSET_GapThreshold				66
#define FLASH_OFFSET_PeelThreshold				67

#define FLASH_OFFSET_GapOutOfPaperThreshold		68
#define FLASH_OFFSET_GapOutOfPaperThresholdOnLabel	69
#define FLASH_OFFSET_GapBufInPtrReverseTrimValue	70

#define FLASH_OFFSET_Id1						80
#define FLASH_OFFSET_Id2						81
#define FLASH_OFFSET_StrWidth					82
#define FLASH_OFFSET_StrHeight					84
#define FLASH_OFFSET_ChRot						86
#define FLASH_OFFSET_LineSp						87
#define FLASH_OFFSET_Justification				88
#define FLASH_OFFSET_InterChSp					89
#define FLASH_OFFSET_Typeface					90
#define FLASH_OFFSET_Magx						91
#define FLASH_OFFSET_Magy						92
#define FLASH_OFFSET_StrMirx					93
#define FLASH_OFFSET_WordWrap					94
#define FLASH_OFFSET_StrType					95	// byte
#define FLASH_OFFSET_AutoShrinkx				96
#define FLASH_OFFSET_VCentering					97
#define FLASH_OFFSET_EnableExpandByte			98
#define FLASH_OFFSET_HTabCh				   		99
#define FLASH_OFFSET_VTabCh						109
#define FLASH_OFFSET_BarType					128
#define FLASH_OFFSET_BarWidthMag				129
#define FLASH_OFFSET_BarHeight					130
#define FLASH_OFFSET_FullHeight					132
#define FLASH_OFFSET_CharHeight					134
#define FLASH_OFFSET_BearerBarWidth		 		136
#define FLASH_OFFSET_CheckType					137
#define FLASH_OFFSET_BarHriPos	 				138
#define FLASH_OFFSET_PrtHri						139
#define FLASH_OFFSET_PrtCheckHri				140
#define FLASH_OFFSET_PrtStartStopHri  			141
#define FLASH_OFFSET_UseOcrb 					142
#define FLASH_OFFSET_Guard	 					143
#define FLASH_OFFSET_DspStrType					160			// byte

#define FLASH_OFFSET_BootStruct					208
#pragma pack(2)
typedef struct {
	INT8U  MAC[6];          // 0~5
	INT8U  IP[4];           // 6~9
	INT8U  VersionStr[16];  // 10~25 // Model(2)+Year(2)+Month(2)+Country(3)+ScaleType(1)+SubVer(2)+Ver(3)
	INT8U  DealerVer[4];    // 26~29 // "(I)"
	INT8U  ConutryVer[3];   // 30~32 // "DE"
	INT8U  Reserved[15];    // 33~47
} BOOT_STRUCT;

typedef struct {
//PRT_STRUCT
	INT8U Mode;									//0
	INT8U reserved_byte_align;					//1
	short LabelWidth, LabelHeight;				//2~5
	short ReceiptWidth, ReceiptHeight;			//6~9
	short TMargin, BMargin, LMargin, RMargin;   //10~17
	INT8U reserved1[14];							//18~31
//PRTDRV_STRUCT
	INT16U GapLength;											//32~33
	INT16U StbTimeTrimValue;									//34~35
	INT8U Speed;												//36
	BOOLEAN PeelEnable, LabelGapMode, RewindEnable;				//37~39
	BOOLEAN PrePrintMode;										//40
	INT8U	PrePrintLength;										//41
	//HYP 20040330
	INT16U ReceiptFeedHeight;									//42-43
	INT8U reserved2[20];										//44~63
//SENSOR_STRUCT
	short GapBufInPtrTrimValue;			//64~65
	INT8U GapThreshold, PeelThreshold;	//66~67
//HYP 20040223
	INT8U GapOutOfPaperThreshold;		//68
	INT8U GapOutOfPaperThresholdOnLabel;	//69
	//INT8U reservedDummy[1]; 		//69
	INT16U GapBufInPtrReverseTrimValue;	//70~71
	INT8U reserved3[8]; 			//72~79
//STR_STRUCT
	INT8U Id1, Id2;					//80~81
	short StrWidth, StrHeight;		//82~85
	char ChRot;						//86
	char LineSp;					//87
	char Justification;				//88
	char InterChSp;					//89
	char Typeface;					//90
	char Magx,Magy;					//91~92
	INT8U StrMirx;					//93
	BOOLEAN WordWrap;				//94
	BOOLEAN StrType;				//95
	BOOLEAN	AutoShrinkx;			//96
	BOOLEAN VCentering;				//97
	BOOLEAN EnableExpandByte;		//98
	INT8U HTabCh[MAX_HTAB_NUM];   	//99~108
	INT8U VTabCh[MAX_VTAB_NUM];		//109~118
	INT8U reserved4[9];				//119~127
//BAR_STRUCT
	INT8U BarType;					//128
	INT8U BarWidthMag;				//129
	short BarHeight;				//130~131
	short FullHeight;				//132~133
	short CharHeight;				//134~135
	INT8U	BearerBarWidth;	 		//136
	INT8U	CheckType;				//137
	INT8U 	BarHriPos; 				//138
	BOOLEAN PrtHri;					//139
	BOOLEAN	PrtCheckHri;			//140
	BOOLEAN PrtStartStopHri;  		//141
	BOOLEAN UseOcrb; 				//142
	BOOLEAN Guard; 					//143
	INT8U reserved5[16];			//144~159
//DSP_STRUCT
	char DspStrType;				//160
	INT8U reserved6[47];			//161~207
	// Boot FW update시 사용할 영역(IP, MAC, Version...)
	BOOT_STRUCT BootStruct;			//208~255
}PRT_FLASH_STRUCT;

typedef struct {
	INT8U	Id;
	INT8U	UseType;	//0 = PRT, 1=DSP, 2 = PRT + DSP
	INT32U	Offset;		// font file start addr offset
}PRT_FLASH_FONT_STRUCT;
#pragma pack()
#define MAX_FONT_NUM	40
//typedef struct {
//	INT8U	FontNum;				//0 font number
//	INT8U reserved_byte_align;		//1
//	PRT_FLASH_FONT_STRUCT	PrtFlashFontStruct[MAX_FONT_NUM];//2~241  //6*40 = 240
//	INT8U reserved[14];			//242~255
//}PRT_FLASH_FONT_HEADER_STRUCT;
#pragma pack(4)
typedef struct {
//not save for initial
	short PrintableWidth,PrintableHeight;		/* Width and Height */
	short PrintableWidthBytes;		/* Width as bytes */
	short PrintWidth, PrintHeight; //applied with margin value
	HUGEDATA INT8U *PrtMemory;	/* PRINTER MEMORY */
	HUGEDATA INT8U	*PrtScratchPad;
	HUGEDATA INT8U	*PrtScratchPadFont;
	HUGEDATA char	*StrBuffer, *BarBuffer;
	INT16U	StrBufferCount;
	INT32U PrtHimem;	/* PRINTER Max Memory */
	INT8U ReceiptLineHeight;
	short ReceiptBmpHeight;
	INT8U ReceiptModeState;
	short x,y;			/* Cursor Position */
	char  Rot;
	char  Opaque;
	//HYP 20040723
	short NonInvModeTphAlign; //HYP 20040723 when non-invert mode, if label or receipt size is smaller than TPH_ROW_BYTE, err
	INT8U StrAreaOverFlag;//HYP 20040903
	INT8U CurrentStrFieldNum;//HYP 20040903 CurrentFieldNum ->CurrentStrFieldNum
	INT8U CurrentBarFieldNum;//HYP 20040903
//save for initial
	INT8U Mode;
	short LabelWidth, LabelHeight;
	short ReceiptWidth, ReceiptHeight;//HYP 20040330 ReceiptHeight is only used for feed key
	short TMargin, BMargin, LMargin, RMargin; /* Top/Bottom/Left/Right Margin */
}PRT_STRUCT;
#pragma pack()
typedef union {		// useful for accessing bytes of words
	INT16U	word;
	struct {
		INT8U	right;
		INT8U	left;
	} byte;
#ifdef USE_ARAB_CONVERT
    struct {
        INT16U  right;
        INT16U  left;
    } words;  // 커서 위에 이란어 위치할 경우 글자 틀어지는 문제 개선을 위해 추가
#endif
} CWD;
#ifdef USE_VIETNAM_FONT
//[start]gm 081209, for vietnam
typedef union {		// useful for accessing bytes of words
	INT32U words;
	struct {
		INT16U	left;
		INT16U	right;	
	} vietnam;
	//W2BYTE   byte;
}CWDVIET; 
//[end]gm 081209, for vietnam
#endif
#if defined(USE_ARAB_FONT) || defined(HEBREW_FONT)
typedef union {
	INT32U word;
	struct {
		INT16U right;
		INT16U left;
	} words;
	struct {
		INT8U  right_dummy;
		INT8U  right;
		INT8U  left_dummy;
		INT8U  left;
	} byte;
} CWDARB;
#endif
#ifdef USE_MORNITORING_CNT
REMOTE_STRUCT TphUseData;
#endif
//extern HUGEDATA PRT_STRUCT PrtStruct;
extern NEARDATA PRT_STRUCT PrtStruct;//HYP 20060412 _huge --> _near for small code size
extern HUGEDATA PRT_FLASH_STRUCT 	PrtFlashStruct;
// extern HUGEDATA PRT_FLASH_FONT_HEADER_STRUCT	PrtFlashFontHeaderStruct;
extern HUGEDATA INT8U *BitmapDataPtr;

extern INT8U PrtFlashSetDefault(void);
extern INT8U PrtFlashSetCurrent(void);
extern void PrtTphMemoryCopy(HUGEDATA INT16U *memA, HUGEDATA INT16U *memB);
extern void	PrtMemoryClear(void);
extern void PrtSetPos(short x,short y);
extern void PrtSetRot(char rot);
extern void PrtSetLabelWidth(short width);
extern void PrtSetLabelHeight(short height);
extern void PrtSetLabelSize(short width,short height);
extern void PrtSetReceiptWidth(short width);
extern void PrtSetReceiptHeight(short height);		//HYP 20040330	
extern void PrtSetReceiptFeedHeight(short height);	//HYP 20040330
extern void PrtSetMargin(short t_margin, short b_margin, short l_margin, short r_margin);
extern void PrtSetHTabPos(HUGEDATA INT8U *htab_pos);
//extern void PrtSetVTabPos(HUGEDATA INT8U *vtab_pos);
extern void PrtDrvSetLabelGapMode(INT8U gap_mode);
extern void PrtLoadModeSetting(INT8U mode);
extern INT8U PrtInit(void);

#endif //_PRT_COMMON_H
