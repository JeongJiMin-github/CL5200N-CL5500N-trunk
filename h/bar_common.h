/*****************************************************************************
|*			
|*  Copyright		:	(c) 2002 CAS
|*  Filename		:	bar_common.h
|*  Version			:	0.1
|*  Programmer(s)	:	Hong Yi-Phyo (HYP)
|*  Created			:	2002/11/11
|*  Description		:	    
|*				
****************************************************************************/

#ifndef _BARCODE_H
#define _BARCODE_H

//HYP 20060404 Define Unused Barcode
//        Barcode                         code size
//#define USE_CODE11	                  0x58E
//#define USE_MSI_PLESSEY	              0xE58
//#define USE_POSTNET                     0x39A
//#define USE_S25_M25_IATA25              0x1090
//#define USE_CODE128ABX                  0x988
//#define USE_UPCE0_UPCE1                 0x1B2E
//#define USE_CODE39FULL                  0x360
//#define USE_CODE93FULL                  0x4A4
//#define USE_EAN13P2_UPCAP2_P5_EAN8P2_P5 0x1A78
//#define USE_CODABAR               0x52C //gm delete for rss barcode 
//////////////////////////////////////////////////////
//  < Barcode Version >
///////////////////////////

//////////////////////////////////////////////////////
//  < Barcode Definition >
//////////////////STANDARD 
#ifdef USE_STANDARD_BARCODE
 //#define USE_RSSEXPAND	//0x5174
 //#define USE_RSS14		//0xE4E
 //#define USE_RSSLIMIT		//0x23BB
#if defined (CL5200_PROJECT) || defined (CL5500_PROJECT)
#define USE_EAN13
#define USE_EAN13SUP5
#define USE_CODABAR               0x52C
#define USE_EAN8
#define USE_CODE128C
#define USE_UPCA
#define USE_CODE39
#define USE_CODE93
#define USE_I2OF5
#else
 #define USE_EAN13
#ifdef USE_JULIAN_DATE
 #define USE_EAN13SUP5
#endif
 //#define USE_CODABAR               0x52C
 //#define USE_EAN8
 #define USE_CODE128C
 #define USE_UPCA
 //#define USE_CODE39
 //#define USE_CODE93
 #define USE_I2OF5
#endif // CL5200_PROJECT
#endif
//////////////////RSS14, RSS Stack, RSS Stack Omni. and RSS Limit
#ifdef USE_RSS_BARCODE
 //#define USE_RSSEXPAND	//0x5174
 #define USE_RSS14			//0xE4E
 #define USE_RSSLIMIT		//0x23BB
 #define USE_EAN13
 #define USE_EAN13SUP5
 //#define USE_CODABAR               0x52C
 //#define USE_EAN8
 //#define USE_CODE128C
 #define USE_UPCA
 //#define USE_CODE39
 //#define USE_CODE93
 //#define USE_I2OF5
#endif
/////////////////RSS EXPAND
#ifdef USE_ONLY_RSS_EXPAND_BARCODE //RSS바코드 사용함 global setting 으로 이동해야 할 것임
 #define USE_RSSEXPAND		//0x5174
 //#define USE_RSS14		//0x2c21
 //#define USE_RSSLIMIT		//0x23BB
 #define USE_EAN13
#ifdef USE_EAN13A5_IN_EXPAND_BARCODE
 #define USE_EAN13SUP5
#endif
 //#define USE_EAN13SUP5
 //#define USE_CODABAR               0x52C
 //#define USE_EAN8
 #define USE_CODE128C		//0xb73
 //#define USE_UPCA
 //#define USE_CODE39
 //#define USE_CODE93
 //#define USE_I2OF5
#endif
//////////////////////////////////////////////////////

#ifdef USE_BARCODE_CODE128X
 #define USE_CODE128ABX                  //0x988
 #ifndef USE_CODE128C
  #define USE_CODE128C
 #endif
#endif

#define MAX_BAR_FIELD_NUM	5

#define CODE128_XTYPE	0
#define CODE128_ATYPE	1
#define CODE128_BTYPE	2
#define CODE128_CTYPE	3
// this number is conserned with Bar_char_width_table[]
#define	EAN13		0
#define	EAN13P2		1
#define	EAN13P5		2
#define	EAN8		3
#define	EAN8P2		4
#define	EAN8P5		5
#define	UPCA		6
#define	UPCAP2		7
#define	UPCAP5		8
#define	UPCE0		9
#define	UPCE0P2		10
#define	UPCE0P5		11
#define	UPCE1		12
#define BAR_HRI_POS_ENABLE_MARGIN	12
#define CODE128A	13
#define CODE128B	14
#define CODE128C	15
#define CODE128X	16
#define CODE93		17
#define CODE93FULL	18
#define CODE11		19
#define BAR_WIDTH_MAG_DIFF_MARGIN 	19
#define CODE39		20
#define CODE39FULL	21
#define	I2OF5		22
#define S2OF5		23	  //Industrial
#define M2OF5		24	  //Matrix
#define IATA2OF5	25	  //International Air Transport Assosiation	 Fixed 17-digit
#define CODABAR		26	  //nw-7
#define MSI			27
#define PLESSEY		28
#define POSTNET		29

//#if defined(USE_RSS_BARCODE) || defined(USE_ONLY_RSS_EXPAND_BARCODE)
#define RSS14		30
#define RSS14T		31
#define RSS14S		32
#define RSS14SO		33
#define RSSLIM		34
#define RSSEXP		35
#define RSSEXPS		36
#ifdef USE_QR_BARCODE //20140902
#define QRCODE		37
#endif
//#endif//#if defined(USE_RSS_BARCODE) || defined(USE_ONLY_RSS_EXPAND_BARCODE)

#define NO_BARCODE	0xff

//include checksum 
#define	MAX_CHAR_EAN13		13
#define	MAX_CHAR_EAN13P2	16 	//include separate space char
#define	MAX_CHAR_EAN13P5	19 	//include separate space char
#define	MAX_CHAR_EAN8		8
#define	MAX_CHAR_EAN8P2		11	//include separate space char
#define	MAX_CHAR_EAN8P5		14	//include separate space char
#define	MAX_CHAR_UPCA		12
#define	MAX_CHAR_UPCAP2		15	//include separate space char
#define	MAX_CHAR_UPCAP5		18	//include separate space char
#define	MAX_CHAR_UPCE0		7
#define	MAX_CHAR_UPCE0P2	10	//include separate space char
#define	MAX_CHAR_UPCE0P5	13	//include separate space char
#define	MAX_CHAR_UPCE1		7
#define	MAX_CHAR_RSS14		14

#ifdef USE_BARCODE_LEN_70
#define MAX_BAR_DATA_BUF_SIZE	70
#elif defined(USE_BARCODE_LEN_90)
#define MAX_BAR_DATA_BUF_SIZE	90
#else
#define MAX_BAR_DATA_BUF_SIZE	50
#endif
#ifdef USE_BARCODE_LEN_90
#define MAX_BAR_PATTERN_SIZE	120//120mm
#else
#define MAX_BAR_PATTERN_SIZE	80//80mm
#endif

#if defined(USE_QR_ADDTOTAL) || defined(USE_CARREFOUR_TRACE_QR_BARCODE) || defined(USE_PRINT_URL_VIA_QR_BARCODE)
#define MAX_BAR_DATA_BUF_SIZE_QR	500
#else
#define MAX_BAR_DATA_BUF_SIZE_QR	MAX_BAR_DATA_BUF_SIZE
#endif
#define	NO_GUARD	   	0
#define	GUARD	   		1
#define EAN13P0_GUARD  	1
#define	EAN13PX_GUARD	2

#define	HRI_POS_DOWN	0
#define	HRI_POS_UP		1

#ifdef USE_DOUBLE_PRINT
#define HRI_FONT_A		0	//12*24
#define HRI_FONT_B		1	//9*17

//error correction level
#define LEVEL_L_	0x30	//7%
#define LEVEL_M_	0x31	//15%
#define LEVEL_Q_	0x32	//25%
#define LEVEL_H_	0x33	//30%

#define SET_MODUL_SIZE			0x43	//size 셋팅
#define ERROR_CORRECTION_LEVEL	0x45	//오류 정정 레벨 선택
#define STOR_SYMBOL_DATA		0x50	//심볼 저장 영역에 심볼 데이터 저장
#define PRINT_SYMBOL_DATA	 	0x51	//심볼 저장 영역 데이터 인쇄
#endif//USE_DOUBLE_PRINT

#if defined(USE_RSS_BARCODE) || defined(USE_ONLY_RSS_EXPAND_BARCODE)
extern char MsgBuf[];
extern void MsgOut(HUGEDATA char *str);
extern int errFlag;
extern INT8U upperPattern[96/2+2];
extern INT8U lowerPattern[96/2+2];

extern INT8U uppersepPattern[96/2+2];
extern INT8U sepPattern[96/2+2]; // SYM_W = 96
extern INT8U lowersepPattern[96/2+2];

extern INT16U seperatorLen;
extern INT16U upperseperatorLen;
extern INT16U lowerseperatorLen;
//[start]KYK add expanded stack
#define RSSEXP_STACK_MAX_ROW_NUM    6 //KYK, MAX = 11
#define MAX_DBL_SEGS    12 // max double segments
#define ELMNTS_EXP      21 // 2-segments, no guard patterns
#define SYM_W_EXP       49 // 2-segment symbol width in modules, no guard patterns

extern INT8U DataPattern[RSSEXP_STACK_MAX_ROW_NUM][MAX_DBL_SEGS*ELMNTS_EXP+4];
extern INT8U upsegsPattern[RSSEXP_STACK_MAX_ROW_NUM][MAX_DBL_SEGS*SYM_W_EXP+2];
extern INT8U midsegsPattern[RSSEXP_STACK_MAX_ROW_NUM][MAX_DBL_SEGS*SYM_W_EXP+2];
extern INT8U lowsegsPattern[RSSEXP_STACK_MAX_ROW_NUM][MAX_DBL_SEGS*SYM_W_EXP+2];
extern INT16U EXP_upperseperatorLen[RSSEXP_STACK_MAX_ROW_NUM];
extern INT16U EXP_lowerseperatorLen[RSSEXP_STACK_MAX_ROW_NUM];
extern INT16U EXP_midsepLen[RSSEXP_STACK_MAX_ROW_NUM];
extern INT16U cnvSeparator(INT8U data[], INT8U sepPattern[], INT8U startTpye, INT8U leng);
//[end]KYK add expanded stack
#endif//#if defined(USE_RSS_BARCODE) || defined(USE_ONLY_RSS_EXPAND_BARCODE)

//#define USE_RSS_DEBUG//gm 20100122 rss barcode debug 
#ifdef USE_RSS_DEBUG
typedef struct {
	long high;
	long low;
} RSS_BARCODE_TEST_STRUCT;
#endif//#ifdef USE_RSS_DEBUG

#pragma pack(4)
typedef struct {
//not save for initial
	short MaxWidth;			/* Bar Max Width and Height */
	char  ns,nb,ws,wb;		/* Narrow Space,Narrow Black,Wide Space,Wide Black */
	short p0,p1,p2,p3,p4,p5,x1,x2;
	INT8U	HriGapEan; //use only ean upc
	INT8U  	QuietZone;
	INT8U   GuardType;	 //0 = no guard, 1=ean13/upca-, 2=ean13p2/5 upc-ap2/5

//save for initial
	INT8U BarType;
	INT8U BarWidthMag;		/* width mag */
	short BarHeight;		/* Bar Height */
	short FullHeight;
	short CharHeight;

	INT8U	BearerBarWidth;	 //ITF
	INT8U	CheckType;		//check sum type 0=none 0< =check
	INT8U 	BarHriPos; 		//0=down 1=up //exceipt upc ean //must be INT8U

	BOOLEAN PrtHri;			//exceipt upc ean
	BOOLEAN	PrtCheckHri;	//codabar i25 s25 postnet code39 msi code11
	BOOLEAN PrtStartStopHri;  //codabar	code39
	BOOLEAN UseOcrb; 		//exceipt ean upc(use guard)
	BOOLEAN Guard; 			//0 = no guard, 1= guard
}BAR_STRUCT;

typedef struct {
	INT8U BarType;
	short bx,by;
	char BarRot;
	INT8U BarWidthMag;	/* width mag */
	short BarHeight;		/* Bar Height */
	short FullHeight;
	short CharHeight;

	INT8U	BearerBarWidth;	 //ITF
	INT8U	CheckType;
	INT8U 	BarHriPos; //0=down 1=up //exceipt upc ean //must be INT8U

	BOOLEAN PrtHri;	//exceipt upc ean
	BOOLEAN	PrtCheckHri;	//codabar i25 s25 postnet code39 msi code11
	BOOLEAN PrtStartStopHri;  //codabar	code39
	BOOLEAN UseOcrb; //ean upc
	BOOLEAN Guard; //0 = no guard, 1= guard
	BOOLEAN ExistField;
}BAR_FIELD_STRUCT;
#pragma pack()
//extern HUGEDATA BAR_STRUCT BarStruct;
extern NEARDATA BAR_STRUCT BarStruct;//HYP 20060412 _huge --> _near for small code size

extern void BarcodeOut(HUGEDATA char *BarString);
extern void BarMembcpy(HUGEDATA INT8U *dest,short pos,ROMDATA char *source,short sz);
extern short BarMembcpyNS(HUGEDATA INT8U *dest,short pos,ROMDATA char *source,char nb,char ns,short sz);
extern short BarMakePattern(ROMDATA char *spattern,short sleng,char startbit,HUGEDATA INT8U *pattern,short pos);
extern void BarInit(void);
extern void BarFieldInit(void);
extern void BarSaveField(INT8U field_num);
extern INT8U BarLoadField(INT8U field_num);
extern void BarSetWidthMag(INT8U wmag);
extern void BarSetBarType(INT8U bar_type);
extern void BarSetMaxWidth(short w);
extern void BarSetNW(char nb,char wb);
extern void BarSetNSWS(char nb,char ns,char wb,char ws);
extern void BarSetBarHeight(short h);
extern void BarSetCharHeight(short h);
extern void BarSetFullHeight(short h);
extern void BarSetGuard(INT8U guard);

extern short BarS25(HUGEDATA char *data,char chksum,HUGEDATA INT8U *pattern);
extern short BarIATA25(HUGEDATA char *data,char chksum,HUGEDATA INT8U *pattern);
extern short BarM25(HUGEDATA char *data,char chksum,HUGEDATA INT8U *pattern);
extern short BarPostnet(HUGEDATA char *data,char chksum,HUGEDATA INT8U *pattern);
extern short BarMsi(HUGEDATA char *data,char chksum,HUGEDATA INT8U *pattern);
extern short BarPlessey(HUGEDATA char *data,char chksum,HUGEDATA INT8U *pattern);
extern short BarI25(HUGEDATA char *data,char chksum,HUGEDATA INT8U *pattern, HUGEDATA char *tdata);
extern short BarUpce0(HUGEDATA char *data,HUGEDATA INT8U *pattern);
extern short BarUpce0Sup2(HUGEDATA char *upc,HUGEDATA char *sup2,HUGEDATA INT8U *pattern);
extern short BarUpce0Sup5(HUGEDATA char *upc,HUGEDATA char *sup2,HUGEDATA INT8U *pattern);
extern short BarUpce1(HUGEDATA char *data,HUGEDATA INT8U *pattern);
extern short BarUpca(HUGEDATA char *data,char chksum,HUGEDATA INT8U *pattern);
extern short BarUpcaSup2(HUGEDATA char *upc,HUGEDATA char *sup2,char chksum,HUGEDATA INT8U *pattern);
extern short BarUpcaSup5(HUGEDATA char *upc,HUGEDATA char *sup5,char chksum,HUGEDATA INT8U *pattern);
extern short BarEan8(HUGEDATA char *data,char chksum,HUGEDATA INT8U *pattern);
extern short BarEan8Sup2(HUGEDATA char *ean8,HUGEDATA char *sup2,char chksum,HUGEDATA INT8U *pattern);
extern short BarEan8Sup5(HUGEDATA char *ean8,HUGEDATA char *sup2,char chksum,HUGEDATA INT8U *pattern);
extern short BarEan13(HUGEDATA char *data,char chksum,HUGEDATA INT8U *pattern);
extern short BarEan13Sup2(HUGEDATA char *ean13,HUGEDATA char *sup2,char chksum,HUGEDATA INT8U *pattern);
extern short BarEan13Sup5(HUGEDATA char *ean13,HUGEDATA char *sup5,char chksum,HUGEDATA INT8U *pattern);
extern short BarCodabar(HUGEDATA char *data,char chksum,HUGEDATA INT8U *pattern);
extern short BarCode11(HUGEDATA char *data,char chksum,HUGEDATA INT8U *pattern);
extern short BarCode128(HUGEDATA char *data,char chksum,HUGEDATA INT8U *pattern, char code_type, HUGEDATA char *trans_cdata);
extern short BarCode39e(HUGEDATA char *data,char chksum,HUGEDATA INT8U *pattern);
extern short BarCode93e(HUGEDATA char *data,char chksum,HUGEDATA INT8U *pattern);
extern short BarCode39(HUGEDATA char *data,char chksum,HUGEDATA INT8U *pattern);
extern short BarCode93(HUGEDATA char *data,char chksum,HUGEDATA INT8U *pattern);
#if defined(USE_RSS_BARCODE) || defined(USE_ONLY_RSS_EXPAND_BARCODE)
extern short BarRSS14MakePattern(HUGEDATA char *spattern,short sleng,char startbit,HUGEDATA INT8U *pattern,short pos);
extern short BarGS1ExpStackSepMakePattern(HUGEDATA char *spattern,short sleng,char startbit,HUGEDATA INT8U *pattern,short pos);
extern char BarUpc_Checksum(HUGEDATA char *data,short leng);
#ifdef USE_LOTTEMART_BARCODE_FUNCTION
extern char Lottemart_GS1_Checksum(HUGEDATA char *data,short leng);
#endif
extern short BarRSS14(HUGEDATA char *data,char chksum, HUGEDATA INT8U *pattern);
extern short BarRSSLimit(HUGEDATA char *data,char chksum,HUGEDATA INT8U *pattern);
extern void BarRSS14S(HUGEDATA char *data, char chksum);
extern void BarRSS14SO(HUGEDATA char *data, char chksum);
extern short BarRSSExp(HUGEDATA char *data, HUGEDATA INT8U *pattern, INT8U *lineNo);
extern int *getRSSwidths(int val, int n, int elements, int maxWidth, int noNarrow);
extern int combins(int n, int r);
#endif//#if defined(USE_RSS_BARCODE) || defined(USE_ONLY_RSS_EXPAND_BARCODE)
				  
extern void BarHriEan13A(HUGEDATA char *HriData, HUGEDATA char *HriData2);
extern void BarHriEan13(HUGEDATA char *HriData);
extern void BarHriEan8A(HUGEDATA char *HriData, HUGEDATA char *HriData2);
extern void BarHriEan8(HUGEDATA char *HriData);
extern void BarHriUpcaA(HUGEDATA char *HriData, HUGEDATA char *HriData2);
extern void BarHriUpca(HUGEDATA char *HriData);
extern void BarHriUpceA(HUGEDATA char *HriData, HUGEDATA char *HriData2);
extern void BarHriUpce(HUGEDATA char *HriData);
extern void BarHri(HUGEDATA char *HriData, short pw);
extern void BarHriCtrlStrConv(HUGEDATA char *BarData1,HUGEDATA char *BarData2);
extern void BarEanHriStrConv(HUGEDATA char *dest1,HUGEDATA char *dest2,HUGEDATA char *src);
extern void Prt_strcpy(HUGEDATA char *dest, HUGEDATA char *src, char num);
#ifdef USE_QR_BARCODE
extern int BarQR (int symbol, INT8U* input, int show_hrt, INT8U scale); //20170421
#endif
#ifdef USE_QR_SIZE_SETTING
extern INT8U PrtPaperType;
extern INT8U QrAddTotalFlag;
extern INT8U GetQRCodeSzie(void);
#endif
#endif /* _BAR_CODE_H */
