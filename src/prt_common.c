/*****************************************************************************
|*			
|*  Copyright		:	(c) 2002 CAS
|*  Filename		:	prt_common.c
|*  Version		:	0.1
|*  Programmer(s)	:	Hong Yi-Phyo (HYP)
|*  Created		:	2003/12/16
|*  Description		:	CLP Printer routine    
|*				
****************************************************************************/
#include <stdio.h>
#include "globals.h"
#include "commbuf.h"
#include "prt_common.h"
#include "prtdrv.h"
#include "bar_common.h"
#include "timer.h"
#include "flash.h"
#include "ram.h"
#include "prt_render.h"
#include "prt_font.h"
#include "sensor.h"
#include "prt_interpreter.h"
#include "dsp_common.h"
#include "initial.h"
#include "flash_app.h"
#ifdef USE_SRAM_PRT_RENERING_BUF
#include "spi_sram.h"
#endif
#ifdef USE_TEST_PRT_RENDERING_TIME
#include "main.h"
extern void MsgOut(HUGEDATA char *);
#endif


//HUGEDATA PRT_STRUCT PrtStruct;
NEARDATA PRT_STRUCT PrtStruct;//HYP 20060412 _huge --> _near for small code size
//HUGEDATA short HTabMemory[MAX_HTAB_NUM];
//HUGEDATA short VTabMemory[MAX_VTAB_NUM];


    
// HUGEDATA PRT_FLASH_FONT_HEADER_STRUCT	PrtFlashFontHeaderStruct	_at(PRT_FLASH_FONT_BASE); 
HUGEDATA INT8U *BitmapDataPtr;

//ROMDATA	INT8U default_htab_table[MAX_HTAB_NUM] = {0,4,8,12,16,20,24,28,32,36};
						//1 2 3 4  5  6  7  8  9 10
ROMDATA	INT8U default_htab_table[MAX_HTAB_NUM] = {2,4,6,8,10,12,14,16,18,22};
//ROMDATA	INT8U default_vtab_table[MAX_VTAB_NUM] = {5,10,15,0,0,0,0,0,0,0};

__no_init  HUGEDATA PRT_FLASH_STRUCT 	PrtFlashStruct;

#ifdef USE_MORNITORING_CNT
//extern char MsgBuf[100];
void PrtTphUseData(void)
{
	 INT32U currTphUseData; //mm

	currTphUseData = PrtDrvStruct.TphMemMaxRowNum * 0.125;

// Timer 인터럽트 내에서 SQI 통신 제거	
//	TphUseData.AfterCnt = get_nvram_lparam(NVRAM_MORNITORING_TPH);
//	TphUseData.TotalCnt = get_nvram_lparam(NVRAM_MORNITORING_TPH + 4);

	TphUseData.AfterCnt += currTphUseData;
	TphUseData.TotalCnt += currTphUseData;

// Timer 인터럽트 내에서 SQI 통신 제거
//	set_nvram_lparam(NVRAM_MORNITORING_TPH, TphUseData.TotalCnt);
//	set_nvram_lparam(NVRAM_MORNITORING_TPH + 4, TphUseData.AfterCnt);

    //sprintf(MsgBuf, "CUR%d | %d | TOT%d\r\n", currTphUseData, TphUseData.AfterCnt, TphUseData.TotalCnt);
//	MsgOut(MsgBuf);

    //set_nvram_lparam(INT32U addr,INT32U v);//afterCnt
    //set_nvram_lparam(INT32U addr,INT32U v);//TotalCnt
}

void PrtSaveTphUseData(void)
{
	set_nvram_lparam(NVRAM_MORNITORING_TPH, TphUseData.TotalCnt);
	set_nvram_lparam(NVRAM_MORNITORING_TPH + 4, TphUseData.AfterCnt);
}

void PrtLoadTphUseData(void)
{
	TphUseData.AfterCnt = get_nvram_lparam(NVRAM_MORNITORING_TPH);
	TphUseData.TotalCnt = get_nvram_lparam(NVRAM_MORNITORING_TPH + 4);
	TphUseData.InspectDate = get_nvram_lparam(NVRAM_MORNITORING_TPH + 8);
}

void PrtClearTphUseData(INT32U InitDate)
{
	TphUseData.InspectDate = InitDate;
	TphUseData.AfterCnt = 0;
	set_nvram_lparam(NVRAM_MORNITORING_TPH + 4, TphUseData.AfterCnt);
	set_nvram_lparam(NVRAM_MORNITORING_TPH + 8, TphUseData.InspectDate);	
}
#endif

INT8U PrtInit(void)
{
	INT8U re;
	
	re=0;

	Flash_sread(PRT_FLASH_BASE, (HUGEDATA INT8U *)&PrtFlashStruct, PRT_FLASH_SET_STRUCT_SIZE);

	if ((PrtFlashStruct.Mode!=LABEL) && (PrtFlashStruct.Mode!=RECEIPT)) re=1;
	if (PrtFlashStruct.LabelWidth<0  ) re=1;
	if (PrtFlashStruct.LabelWidth>640) re=1;
	if (re) PrtFlashSetDefault();
	return re;
}

INT8U PrtFlashSetDefault(void)
{
	PRT_FLASH_STRUCT prt_flash_struct;
//	HUGEDATA INT8U *p;
	INT8U i;
//	p = (HUGEDATA INT8U *)&prt_flash_struct;
	INT32U p;
	p=PRT_FLASH_BASE;
//PRT_STRUCT
	prt_flash_struct.Mode = LABEL;				//0
//	prt_flash_struct.Mode = RECEIPT;				//0
//	INT8U reserved_byte_align1;					//1
	prt_flash_struct.LabelWidth = TPH_ROW_BYTE*8;
	prt_flash_struct.LabelHeight = 40*8;		//2~5
	prt_flash_struct.ReceiptWidth = TPH_ROW_BYTE*8;
	prt_flash_struct.ReceiptHeight = 20*8;			//6~9

	prt_flash_struct.TMargin = 0;
	prt_flash_struct.BMargin = 0;
	prt_flash_struct.LMargin = 0;
	prt_flash_struct.RMargin = 0;   			//10~17
//	INT8U reserved1[14];				   		//18~31

//PRTDRV_STRUCT
	prt_flash_struct.GapLength = 8*2; //2mm						//32~33
	prt_flash_struct.StbTimeTrimValue = STB_TIME_DIV*10;					//34~35
	prt_flash_struct.Speed = 4;								//36
	prt_flash_struct.PeelEnable = DISABLE;
	prt_flash_struct.LabelGapMode = GAP;
	prt_flash_struct.RewindEnable = ENABLE;					//37~39
	prt_flash_struct.PrePrintMode = ENABLE;					//40
	prt_flash_struct.PrePrintLength = 2*8;		//41
	//HYP 20040330
	prt_flash_struct.ReceiptFeedHeight = 10*8;//10mm			//42-43
//	INT8U reserved2[20];							//44~63


//SENSOR_STRUCT
	prt_flash_struct.GapBufInPtrTrimValue = 0;						//64~65
	prt_flash_struct.GapThreshold = GAP_SENSOR_DEFAULT_THRESHOLD;
	prt_flash_struct.PeelThreshold = PEEL_SENSOR_DEFAULT_THRESHOLD;	//66~67
//HYP 20040223
	prt_flash_struct.GapOutOfPaperThreshold = GAP_OUTOFPAPER_DEFAULT_THRESHOLD;	//68
	prt_flash_struct.GapOutOfPaperThresholdOnLabel = GAP_OUTOFPAPER_DEFAULT_THRESHOLD_ON_LABEL;	//69
//	INT8U reserved3[11]; 											//69~79

//STR_STRUCT
	prt_flash_struct.Id1 = PRT_16x16_VAR_WIDTH_FONT_ID1;
	prt_flash_struct.Id2 = PRT_16x16_FIXED_WIDTH_FONT_ID2;	//80~81
	prt_flash_struct.StrWidth = 400;
	prt_flash_struct.StrHeight = 300;						//82~85
	prt_flash_struct.ChRot = DEG_0;							//86
	prt_flash_struct.LineSp = 4;							//87
	prt_flash_struct.Justification = LEFT;					//88
	prt_flash_struct.InterChSp = 4;							//89
	prt_flash_struct.Typeface = NO_TYPEFACE;				//90
	prt_flash_struct.Magx = 2;
	prt_flash_struct.Magy = 2;								//91~92
	prt_flash_struct.StrMirx = 0;							//93
	prt_flash_struct.WordWrap = ON;							//94
	prt_flash_struct.StrType = TWO_BYTE_FONT_STR;			//95
	prt_flash_struct.AutoShrinkx = OFF;						//96
	prt_flash_struct.VCentering = OFF;						//97
	prt_flash_struct.EnableExpandByte = ON;					//98
	for(i=0; i<MAX_HTAB_NUM; i++) {
		prt_flash_struct.HTabCh[i] = default_htab_table[i];	//99~108
	}
//	for(i=0; i<MAX_VTAB_NUM; i++) {
//		prt_flash_struct.VTabCh[i] = default_vtab_table[i];	//109~118
//	}
//	INT8U reserved4[9];				//119~127

//BAR_STRUCT
	prt_flash_struct.BarType = EAN13;				//128
	prt_flash_struct.BarWidthMag = 2;				//129
	prt_flash_struct.BarHeight = 104;				//130~131
	prt_flash_struct.FullHeight = 120;				//132~133
	prt_flash_struct.CharHeight = 32;				//134~135
	prt_flash_struct.BearerBarWidth = 0;	 		//136
	prt_flash_struct.CheckType = 1;					//137
	prt_flash_struct.BarHriPos = HRI_POS_DOWN;		//138
	prt_flash_struct.PrtHri = ON;					//139
	prt_flash_struct.PrtCheckHri = ON;				//140
	prt_flash_struct.PrtStartStopHri = ON; 			//141
	prt_flash_struct.UseOcrb = OFF; 				//142
	prt_flash_struct.Guard = NO_GUARD;				//143
//	INT8U reserved5[16];			//144~159
//DSP_STRUCT
	prt_flash_struct.DspStrType = TWO_BYTE_FONT_STR;//160
//	INT8U reserved6[95];			//161~255
// save flash
	Flash_swrite(p,(HUGEDATA INT8U *)&prt_flash_struct,PRT_FLASH_SET_STRUCT_SIZE);
	Flash_flush_nvbuffer();//HYP 20040204 FLASH NVRAM

	Flash_sread(PRT_FLASH_BASE, (HUGEDATA INT8U *)&PrtFlashStruct, PRT_FLASH_SET_STRUCT_SIZE);

//        set_prt_bparam(PRT_FONT_MENUID , 0);
//        set_prt_bparam(PRT_FONT_PLUID  , 3);
//        set_prt_bparam(PRT_FONT_LABELID, 3);
#ifdef USE_8_16_FONT_MENU_DISPLAY
	set_global_bparam(GLOBAL_MENU_FONTID	, 3);
#else
	set_global_bparam(GLOBAL_MENU_FONTID	, 2);
#endif
#ifdef USE_CHN_FONT
	set_global_bparam(GLOBAL_PLU_FONTID   	, 3);
#else
	set_global_bparam(GLOBAL_PLU_FONTID   	, 4);
#endif
	set_global_bparam(GLOBAL_LABEL_FONTID 	, 4);

	set_global_bparam(GLOBAL_PRT_TICKET_FONT_SIZE, 0x66 );

//	Flash_bwrite(p+PRT_FONT_PLUID  , 3);
//	Flash_bwrite(p+PRT_FONT_LABELID, 3);
	return TRUE;
}

/* HYP 20060407 Erase non-use function
INT8U PrtFlashSetCurrent(void)
{
	PRT_FLASH_STRUCT prt_flash_struct;
//	HUGEDATA INT8U *p;
	INT8U i;
	INT32U p;

	p=PRT_FLASH_BASE;

//	p = (HUGEDATA INT8U *)&prt_flash_struct;

//PRT_STRUCT
	prt_flash_struct.Mode = PrtStruct.Mode;						//0
//	INT8U reserved_byte_align1;									//1
	prt_flash_struct.LabelWidth = PrtStruct.LabelWidth;
	prt_flash_struct.LabelHeight = PrtStruct.LabelHeight;		//2~5
	prt_flash_struct.ReceiptWidth = PrtStruct.ReceiptWidth;
	prt_flash_struct.ReceiptHeight = PrtStruct.ReceiptHeight;	//6~9

	prt_flash_struct.TMargin = PrtStruct.TMargin;
	prt_flash_struct.BMargin = PrtStruct.BMargin;
	prt_flash_struct.LMargin = PrtStruct.LMargin;
	prt_flash_struct.RMargin = PrtStruct.RMargin;   			//10~17
//	INT8U reserved1[14];				   						//18~31

//PRTDRV_STRUCT
	prt_flash_struct.GapLength = PrtDrvStruct.GapLength; //2mm		   	//32~33
	prt_flash_struct.StbTimeTrimValue = PrtDrvStruct.StbTimeTrimValue; 	//34~35
	prt_flash_struct.Speed = PrtDrvStruct.Speed;					//36
	prt_flash_struct.PeelEnable = PrtDrvStruct.PeelEnable;
	prt_flash_struct.LabelGapMode = PrtDrvStruct.LabelGapMode;
	prt_flash_struct.RewindEnable = PrtDrvStruct.RewindEnable;		//37~39
	prt_flash_struct.PrePrintMode = PrtDrvStruct.PrePrintMode;		//40
	prt_flash_struct.PrePrintLength = PrtDrvStruct.PrePrintLength;		//41
	//HYP 20040330
	prt_flash_struct.ReceiptFeedHeight = PrtDrvStruct.ReceiptFeedHeight;	//42-43
//	INT8U reserved2[20];							//44~63

//SENSOR_STRUCT
	prt_flash_struct.GapBufInPtrTrimValue = SensorStruct.GapBufInPtrTrimValue;	//64~65
	prt_flash_struct.GapThreshold = SensorStruct.GapThreshold;
	prt_flash_struct.PeelThreshold = SensorStruct.PeelThreshold;				//66~67
//HYP 20040223
	prt_flash_struct.GapOutOfPaperThreshold = SensorStruct.GapOutOfPaperThreshold;	//68
//	INT8U reserved3[11]; 														//68~79

//STR_STRUCT
	prt_flash_struct.Id1 = StrStruct.Id1;
	prt_flash_struct.Id2 = StrStruct.Id2;							//80~81
	prt_flash_struct.StrWidth = StrStruct.StrWidth;
	prt_flash_struct.StrHeight = StrStruct.StrHeight;				//82~85
	prt_flash_struct.ChRot = StrStruct.ChRot;						//86
	prt_flash_struct.LineSp = StrStruct.LineSp;						//87
	prt_flash_struct.Justification = StrStruct.Justification;		//88
	prt_flash_struct.InterChSp = StrStruct.InterChSp;				//89
	prt_flash_struct.Typeface = StrStruct.Typeface;					//90
	prt_flash_struct.Magx = StrStruct.Magx;
	prt_flash_struct.Magy = StrStruct.Magy;							//91~92
	prt_flash_struct.StrMirx = StrStruct.StrMirx;					//93
	prt_flash_struct.WordWrap = StrStruct.WordWrap;					//94
	prt_flash_struct.StrType = StrStruct.StrType;					//95
	prt_flash_struct.AutoShrinkx = StrStruct.AutoShrinkx;			//96
	prt_flash_struct.VCentering = StrStruct.VCentering;				//97
	prt_flash_struct.EnableExpandByte = StrStruct.EnableExpandByte;	//98
	for(i=0; i<MAX_HTAB_NUM; i++) {
		prt_flash_struct.HTabCh[i] = StrStruct.HTabCh[i];			//99~108
	}
//	for(i=0; i<MAX_VTAB_NUM; i++) {
//		prt_flash_struct.VTabCh[i] = StrStruct.VTabCh[i];			//109~118
//	}
//	INT8U reserved4[9];												//119~127

//BAR_STRUCT
	prt_flash_struct.BarType = BarStruct.BarType;					//128
	prt_flash_struct.BarWidthMag = BarStruct.BarWidthMag;			//129
	prt_flash_struct.BarHeight = BarStruct.BarHeight;				//130~131
	prt_flash_struct.FullHeight = BarStruct.FullHeight;				//132~133
	prt_flash_struct.CharHeight = BarStruct.CharHeight;				//134~135
	prt_flash_struct.BearerBarWidth = BarStruct.BearerBarWidth;		//136
	prt_flash_struct.CheckType = BarStruct.CheckType;				//137
	prt_flash_struct.BarHriPos = BarStruct.BarHriPos;				//138
	prt_flash_struct.PrtHri = BarStruct.PrtHri;						//139
	prt_flash_struct.PrtCheckHri = BarStruct.PrtCheckHri;			//140
	prt_flash_struct.PrtStartStopHri = BarStruct.PrtStartStopHri; 	//141
	prt_flash_struct.UseOcrb = BarStruct.UseOcrb; 					//142
	prt_flash_struct.Guard = BarStruct.Guard;						//143
//	INT8U reserved5[16];											//144~159
//DSP_STRUCT
	prt_flash_struct.DspStrType = DspStruct.StrType;	//160
//	INT8U reserved6[95];								//161~255
// save flash
	Flash_swrite(p,(HUGEDATA INT8U *)&prt_flash_struct,PRT_FLASH_SET_STRUCT_SIZE);
	Flash_flush_nvbuffer();//HYP 20040204 FLASH NVRAM
	return TRUE;
}
*/
void PrtSetPos(short x,short y)
{
	PrtStruct.x=x;
	PrtStruct.y=y;
}

void PrtSetRot(char rot)
{
	PrtStruct.Rot=rot;
}

void PrtMemoryClear(void)
{
	INT16U	count1, count2;
#ifdef USE_SRAM_PRT_RENERING_BUF
    INT32U pSram = SramPrtMemory;
#else // USE_SRAM_PRT_RENERING_BUF
	HUGEDATA INT16U *p;
	p = (HUGEDATA INT16U *)(PrtStruct.PrtMemory);
#endif // USE_SRAM_PRT_RENERING_BUF
#ifdef USE_TEST_PRT_RENDERING_TIME
    static INT16U tempCallCount = 0;
//      INT16U tic = 0;
//      INT16U toc = 0;
//      tic = SysTimer1_25ms;
    sprintf(MsgBuf, "****** #%d PrtM_C(M:%d)\r\n", tempCallCount++, PrtStruct.Mode);
    MsgOut(MsgBuf);
#endif


	if(PrtStruct.Mode == LABEL) {
    #ifdef USE_SRAM_PRT_RENERING_BUF
        SramMemoryClear( pSram, PrtStruct.PrintableHeight*(TPH_ROW_BYTE) );
    #else // USE_SRAM_PRT_RENERING_BUF
		for (count1 = 0; count1 < PrtStruct.PrintableHeight; count1++) {
			for (count2 = 0; count2 < (TPH_ROW_BYTE/2); count2++) {
				p[count1*(TPH_ROW_BYTE/2)+count2] = 0x0000;
			}
		}
    #endif // USE_SRAM_PRT_RENERING_BUF
	} else{
	#ifdef USE_SRAM_PRT_RENERING_BUF
        SramMemoryClear( pSram, MAX_FORM_LEN*(TPH_ROW_BYTE) );
    #else // USE_SRAM_PRT_RENERING_BUF
		for (count1 = 0; count1 < MAX_FORM_LEN; count1++) {
			for (count2 = 0; count2 < (TPH_ROW_BYTE/2); count2++) {
				p[count1*(TPH_ROW_BYTE/2)+count2] = 0x0000;
			}
		}
    #endif // USE_SRAM_PRT_RENERING_BUF
	}
}

void PrtSetHTabPos(HUGEDATA INT8U *htab_pos)
{
	INT8U	i;
	short 	s_pos, e_pos;
//	char string_buf[30];
	for (i=0; i < MAX_HTAB_NUM; i++) {
		s_pos = (short)FontStruct.Wbits1 * (short)htab_pos[i] * (short)StrStruct.Magx;
		e_pos = s_pos + (short)FontStruct.Wbits1 * (short)StrStruct.Magx;
		if(e_pos < PrtStruct.PrintWidth) {
			StrStruct.HTab[i] = s_pos;
			StrStruct.HTabCh[i] = htab_pos[i];
		} else {
			StrStruct.HTab[i] = 0;
			StrStruct.HTabCh[i] = 0;
		}
//		sprintf(string_buf, "t1=%d,",StrStruct.HTab[i]); /////
//		PutStrTxBuf(&CommBuf,string_buf);//////
	}
}
/*
void PrtSetVTabPos(HUGEDATA INT8U *vtab_pos)
{
	INT8U	i;
	char string_buf[30];
	short s_pos, e_pos;

	for (i=0; i < MAX_VTAB_NUM; i++) {
		s_pos = (short)FontStruct.Hbits1 * (short)vtab_pos[i] * (short)StrStruct.Magy;
		e_pos = s_pos + (short)FontStruct.Hbits1 * (short)StrStruct.Magy;
		if(e_pos < PrtStruct.PrintHeight) {
			StrStruct.VTab[i] = s_pos;
			StrStruct.VTabCh[i] = vtab_pos[i];
		} else {
			StrStruct.VTab[i] = 0;
			StrStruct.VTabCh[i] = 0;
		}
		sprintf(string_buf, "vt1=%d,",StrStruct.VTab[i]); /////
		PutStrTxBuf(&CommBuf,string_buf);//////
	}
}
*/

void PrtSetLabelWidth(short width) {
	
	if(width > (TPH_ROW_BYTE*8) ) width = TPH_ROW_BYTE*8;
	PrtStruct.LabelWidth = width;
	//HYP 20040723 
	PrtStruct.NonInvModeTphAlign = (TPH_ROW_BYTE*8) - width;

//	PrtStruct.PrintableWidth =PrtStruct.LabelWidth;
//	PrtStruct.PrintableWidthBytes=width/8;
    PrtStruct.PrintableWidth = TPH_ROW_BYTE*8;  //physical memory is constant.
	PrtStruct.PrintableWidthBytes=TPH_ROW_BYTE;
	if (width%8) PrtStruct.PrintableWidthBytes++;
	PrtStruct.PrtHimem=PrtStruct.PrintableWidthBytes;
//	PrtStruct.PrintWidth = PrtStruct.PrintableWidth - PrtStruct.LMargin - PrtStruct.RMargin;
	PrtStruct.PrintWidth = PrtStruct.LabelWidth - PrtStruct.LMargin - PrtStruct.RMargin;
}

void PrtSetLabelHeight(short height) {
#ifndef USE_CONTINUOUS_LABEL_WITH_INGREDIENT	
	if(height > MAX_FORM_LEN ) height = MAX_FORM_LEN;
#endif	
	PrtStruct.LabelHeight = height;

	PrtStruct.PrintableHeight=PrtStruct.LabelHeight;
	PrtStruct.PrtHimem=PrtStruct.PrtHimem*height;
	PrtStruct.PrintHeight = PrtStruct.PrintableHeight - PrtStruct.TMargin - PrtStruct.BMargin;
	if(PrtStruct.Mode == LABEL) {
		PrtDrvStruct.TphMemMaxRowNum = PrtStruct.LabelHeight;//only label mode
	} else {
		PrtDrvStruct.TphMemMaxRowNum = 0;
	}
	//change formfeedlength
	PrtDrvStruct.FormFeedLength = (INT16U)PrtStruct.LabelHeight;
	//HYP 20040921
	if (PrtDrvStruct.PrtDirection == NON_INVERT) {
		PrtDrvStruct.TphMemRowCount = 0;
	} else {
		PrtDrvStruct.TphMemRowCount = PrtDrvStruct.TphMemMaxRowNum - 1;
	}	
}

void PrtSetReceiptWidth(short width) {
	
	if(width > (TPH_ROW_BYTE*8) ) width = TPH_ROW_BYTE*8;	
	PrtStruct.ReceiptWidth = width;
	//HYP 20040723 
	PrtStruct.NonInvModeTphAlign = (TPH_ROW_BYTE*8) - width;

//	PrtStruct.PrintableWidth =PrtStruct.ReceiptWidth;
//	PrtStruct.PrintableWidthBytes=width/8;
    PrtStruct.PrintableWidth = TPH_ROW_BYTE*8;  //physical memory is constant.
	PrtStruct.PrintableWidthBytes=TPH_ROW_BYTE;

	if (width%8) PrtStruct.PrintableWidthBytes++;
//	PrtStruct.PrintWidth = PrtStruct.PrintableWidth - PrtStruct.LMargin - PrtStruct.RMargin;
	PrtStruct.PrintWidth = PrtStruct.ReceiptWidth - PrtStruct.LMargin - PrtStruct.RMargin;

	GapBufInit();
}

//HYP 20040330
void PrtSetReceiptHeight(short height) {
	
	//HYP 20041130
	//if(height > MAX_FORM_LEN ) height = MAX_FORM_LEN;
	if(height > MAX_RECEIPT_LEN ) height = MAX_RECEIPT_LEN;
	PrtStruct.ReceiptHeight = height;
	//change formfeedlength
	PrtDrvStruct.FormFeedLength = (INT16U)PrtStruct.ReceiptHeight;
}
//HYP 20040330
void PrtSetReceiptFeedHeight(short height) {
	
	//HYP 20041130
	//if(height > MAX_FORM_LEN ) height = MAX_FORM_LEN;
	if(height > MAX_RECEIPT_LEN ) height = MAX_RECEIPT_LEN;
	PrtDrvStruct.ReceiptFeedHeight = height;
}

void PrtSetLabelSize(short width,short height)
{	 
	PrtSetLabelWidth(width);
	PrtSetLabelHeight(height);
	GapBufInit();
}

void PrtSetMargin(short t_margin, short b_margin, short l_margin, short r_margin) {
	PrtStruct.TMargin = t_margin;
	PrtStruct.BMargin = b_margin;
	PrtStruct.LMargin = l_margin;
	PrtStruct.RMargin = r_margin;
//	PrtStruct.PrintWidth = PrtStruct.PrintableWidth - PrtStruct.LMargin - PrtStruct.RMargin;
	if(PrtStruct.Mode == LABEL) {
		PrtStruct.PrintWidth = PrtStruct.LabelWidth - PrtStruct.LMargin - PrtStruct.RMargin;
	} else {
		PrtStruct.PrintWidth = PrtStruct.ReceiptWidth - PrtStruct.LMargin - PrtStruct.RMargin;
	}
	PrtStruct.PrintHeight = PrtStruct.PrintableHeight - PrtStruct.TMargin - PrtStruct.BMargin;
}


