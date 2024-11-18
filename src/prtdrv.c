
/*****************************************************************************
|*			
|*  Copyright		:	(c) 2002 CAS
|*  Filename		:	prtdrv.c
|*  Version		:	0.1
|*  Programmer(s)	:	Hong Yi-Phyo (HYP)
|*  Created		:	2003/03/17
|*  Modified		:	2003/12/16
|*  Modified		:	2004/08/20
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
#include "initial.h"
#include "prt_render.h"
#include "prt_font.h"
#include "sensor.h"
#include "ram.h"
#include "flash.h"
#include "flash_app.h"
#ifdef USE_SRAM_SPI_COMM
#include "spi_sram.h"
#endif
#ifdef USE_TEST_PRT_RENDERING_TIME
#include "main.h"
extern void MsgOut(HUGEDATA char *);
#endif


//HUGEDATA PRTDRV_STRUCT PrtDrvStruct;
PRTDRV_STRUCT PrtDrvStruct;//HYP 20060412 _huge --> _near for small code size

#ifndef USE_SRAM_PRT_RENERING_BUF
#ifdef USE_ONLY_1EA_PRT_RENDERING_BUF
__no_init  HUGEDATA INT8U TphBufC[MAX_FORM_LEN][TPH_ROW_BYTE];//  @ (0x020000 + SRAM_BASE);
#else
__no_init  HUGEDATA INT8U TphBufC[2][MAX_FORM_LEN][TPH_ROW_BYTE];//  @ (0x020000 + SRAM_BASE);
#endif
#endif // USE_SRAM_PRT_RENERING_BUF
void STEP_MOTOR_CON(INT8U  data)
{
#ifndef _CL3000_PRINTER_MECH
  	//REWIND_PH1
	if(data&0x01) PRT_R_PH1_PORT_HIGH;
	else  PRT_R_PH1_PORT_LOW;
  	//REWIND_PH2
	if(data&0x02) PRT_R_PH2_PORT_HIGH;
	else  PRT_R_PH2_PORT_LOW;
  	//REWIND_ENABLE
	if(data&0x04) PRT_R_EN_PORT_HIGH;
	else  PRT_R_EN_PORT_LOW;
#endif
  	//MAIN_PH1
	if(data&0x10) PRT_M_PH1_PORT_HIGH;
	else  PRT_M_PH1_PORT_LOW;
  	//MAIN_PH2
	if(data&0x20) PRT_M_PH2_PORT_HIGH;
	else  PRT_M_PH2_PORT_LOW;
  	//MAIN_ENABLE
	if(data&0x40) PRT_M_EN_PORT_HIGH;
	else  PRT_M_EN_PORT_LOW;
}

//-- Stepmotor address 		: 0x7 WR -- stepping motor
//-- | X |MAIN_EN|MAIN_PH2|MAIN_PH1| X |REWIND_EN|REWIND_PH2|REWIND_PH1|

void PrtDrvSetDirection(BOOLEAN direction)
{
	PrtDrvStruct.PrtDirection = direction;
	if (PrtDrvStruct.PrtDirection == NON_INVERT) {  
		PrtDrvStruct.TphMemRowCount = 0;
		//HYP 20060428 prohibit pre-print when use NON_INVERT
		//for use NON_INVERT label for using receipt 
		//PrtDrvStruct.PrePrintMode = DISABLE;//HYP 20070629
	} else {
		PrtDrvStruct.TphMemRowCount = PrtDrvStruct.TphMemMaxRowNum - 1;
		//HYP 20060428 prohibit pre-print when use NON_INVERT
		//restore direction for label
		//PrtDrvSetPrePrintMode(PrtFlashStruct.PrePrintMode, PrtFlashStruct.PrePrintLength); //HYP 20070629
	}
}

#ifdef USE_FORMFEED_WITH_PRE_PRINT //HYP 20060428
void PrtDrvTphMemoryClear(INT8U flag_FF)
{
    #ifndef USE_SRAM_PRT_RENERING_BUF
	INT16U	count1, count2;
	INT16U	start, end;
//	#ifdef USE_SRAM_PRT_RENERING_BUF
//	        INT32U pSram = SramTphMemory;
//	#else // USE_SRAM_PRT_RENERING_BUF
        HUGEDATA INT16U *p;
        p = (HUGEDATA INT16U *)(PrtDrvStruct.TphMemory);
//	#endif // USE_SRAM_PRT_RENERING_BUF

	if (PrtStruct.Mode == LABEL) {
//		if((PrtDrvStruct.PrePrintMode == ENABLE) && (PrtDrvStruct.PrtDirection == INVERT) && (flag_FF == ON)) {
//			start = 0;
//			end = PrtStruct.PrintableHeight - PrtDrvStruct.PrePrintLength - 1;
		//HYP 20070629
		if((PrtDrvStruct.PrePrintMode == ENABLE) && (flag_FF == ON)) {		
			if (PrtDrvStruct.PrtDirection == NON_INVERT) {
				start = PrtDrvStruct.PrePrintLength;
				end = PrtStruct.PrintableHeight;
			} else {
				start = 0;
				end = PrtStruct.PrintableHeight - PrtDrvStruct.PrePrintLength - 1;
			}
		} else {
			start = 0;
			end = PrtStruct.PrintableHeight;
		}
	} else {
		start = 0;
		end = MAX_RECEIPT_LEN;//MAX_FORM_LEN;//HYP 20070417
	}
    
    #ifdef USE_SRAM_PRT_RENERING_BUF
    SramMemoryClear( pSram+(start*(TPH_ROW_BYTE/2)), (end-start)*(TPH_ROW_BYTE/2) );
    #else //USE_SRAM_PRT_RENERING_BUF
	for (count1 = start; count1 < end; count1++) {
		for (count2 = 0; count2 < (TPH_ROW_BYTE/2); count2++) {
			p[count1*(TPH_ROW_BYTE/2)+count2] = 0x0000;
		}
	}
    #endif //USE_SRAM_PRT_RENERING_BUF
    #endif
}

void PrtDrvTphMemoryCopy(HUGEDATA INT16U *memA, HUGEDATA INT16U *memB,INT8U flag_FF)
{
    #ifndef USE_SRAM_PRT_RENERING_BUF
	INT16U	count1, count2;
	INT16U	start, end;

//	if((PrtDrvStruct.PrePrintMode == ENABLE) && (PrtDrvStruct.PrtDirection == INVERT) && (flag_FF == ON)) {
//		start = 0;
//		end = PrtStruct.PrintableHeight - PrtDrvStruct.PrePrintLength - 1;
	//HYP 20070629
	if((PrtDrvStruct.PrePrintMode == ENABLE) && (flag_FF == ON)) {
		if (PrtDrvStruct.PrtDirection == NON_INVERT) {
			start = PrtDrvStruct.PrePrintLength;
			end = PrtStruct.PrintableHeight;
		} else {
			start = 0;
			end = PrtStruct.PrintableHeight - PrtDrvStruct.PrePrintLength - 1;
		}
	} else {
		start = 0;
		end = PrtStruct.PrintableHeight;
	}
	for (count1 = start; count1 < end; count1++) {
		for (count2 = 0; count2 < (TPH_ROW_BYTE/2); count2++) {
			*memA = *memB;
			memA++;
			memB++;
		}
	}
    #endif
}
#else 
void PrtDrvTphMemoryClear(void)
{
	INT16U	count1, count2;
#ifdef USE_SRAM_PRT_RENERING_BUF
    INT32U pSram = SramTphMemory;
#else // USE_SRAM_PRT_RENERING_BUF
    HUGEDATA INT16U *p;
    p = (HUGEDATA INT16U *)(PrtDrvStruct.TphMemory);
#endif // USE_SRAM_PRT_RENERING_BUF
#ifdef USE_TEST_PRT_RENDERING_TIME
    static INT16U tempCallCount = 0;
    INT16U tic = 0;
    INT16U toc = 0;
    tic = SysTimer1_25ms;
#endif
    
	if (PrtStruct.Mode == LABEL) {
    #ifdef USE_SRAM_PRT_RENERING_BUF
        SramMemoryClear( pSram, PrtStruct.PrintableHeight*(TPH_ROW_BYTE) );
    #else //USE_SRAM_PRT_RENERING_BUF
		for (count1 = 0; count1 < PrtStruct.PrintableHeight; count1++) {
			for (count2 = 0; count2 < (TPH_ROW_BYTE/2); count2++) {
				p[count1*(TPH_ROW_BYTE/2)+count2] = 0x0000;
			}
		}
    #endif //USE_SRAM_PRT_RENERING_BUF
	} else {
	#ifdef USE_SRAM_PRT_RENERING_BUF
        SramMemoryClear( pSram, MAX_RECEIPT_LEN*(TPH_ROW_BYTE) );
    #else //USE_SRAM_PRT_RENERING_BUF
		for (count1 = 0; count1 < MAX_RECEIPT_LEN; count1++) {
			for (count2 = 0; count2 < (TPH_ROW_BYTE/2); count2++) {
				p[count1*(TPH_ROW_BYTE/2)+count2] = 0x0000;
			}
		}
    #endif //USE_SRAM_PRT_RENERING_BUF
	}
    #ifdef USE_TEST_PRT_RENDERING_TIME
    toc = SysTimer1_25ms;
    sprintf(MsgBuf, "***** #%d TphM_C(M:%d / Time:%d)\r\n", tempCallCount++, PrtStruct.Mode, toc-tic);
    MsgOut(MsgBuf);
#endif
}

void PrtDrvTphMemoryCopy(HUGEDATA INT16U *memA, HUGEDATA INT16U *memB)
{
	INT16U	count1, count2;
	for (count1 = 0; count1 < PrtStruct.PrintableHeight; count1++) {
		for (count2 = 0; count2 < (TPH_ROW_BYTE/2); count2++) {
			*memA = *memB;
			memA++;
			memB++;
		}
	}
}
#endif
//	HYP 20040330
//	quality --> speed
//	speed = 0 : 2.50ms(400Hz)
//	speed = 1 : 2.00ms(500Hz)
//	speed = 2 : 1.67ms(600Hz)
//	speed = 3 : 1.43ms(700Hz)
//	speed = 4 : 1.25ms(800Hz)

void PrtDrvSetSpeed(INT8U speed) {

#ifdef PRT_FIXED_SPEED_100
	speed = 4;
#endif

	if(speed > 4) speed = 4;
	PrtDrvStruct.Speed = speed; //0~4 (5 level)
	PrtDrvStruct.Timer0ReloadValue = PrtDrvStruct.Timer0ReloadValueLow + PrtDrvStruct.Timer0ReloadValueInterval*PrtDrvStruct.Speed;
	PrtDrvStruct.Timer0AccelIntervalValue = (PrtDrvStruct.Timer0ReloadValue - PrtDrvStruct.Timer0AccelIntervalStartValue)>>3;
	switch(speed) {
		case 0:
			PrtDrvStruct.SysTimerNum10ms = 4;
			break;
		case 1:
			PrtDrvStruct.SysTimerNum10ms = 5;
			break;
		case 2:
			PrtDrvStruct.SysTimerNum10ms = 6;
			break;
		case 3:
			PrtDrvStruct.SysTimerNum10ms = 7;
			break;
		case 4:
		default:
			PrtDrvStruct.SysTimerNum10ms = 8;
			break;
	}
}

void PrtSensorFlagInit(void) {
	PrtDrvStruct.OutOfPaperFlag = OFF;
	PrtDrvStruct.GapSenErrFlag = OFF;
	PrtDrvStruct.HangingFormFlag = OFF;
	PrtDrvStruct.HeadUpFlag = OFF;
	SensorStruct.MaxGapLastValue = SensorStruct.GapThreshold; 
	SensorStruct.MinGapLastValue =	SensorStruct.GapThreshold;
	SensorStruct.MaxPeelLastValue = SensorStruct.PeelThreshold;
	SensorStruct.MinPeelLastValue = SensorStruct.PeelThreshold;
}	

void PrtDrvInit(void)
{
#ifdef USE_SRAM_PRT_RENERING_BUF
    SramTphMemoryA = SRAM_ADDR_PRT_BASE;
    SramTphMemoryB = SRAM_ADDR_PRT_BASE;
    SramTphMemory = SramTphMemoryA;
    SramPrtMemory = SramTphMemoryA;
    PrtDrvTphMemoryClear( );
#else //USE_SRAM_PRT_RENERING_BUF
    #ifdef USE_ONLY_1EA_PRT_RENDERING_BUF    
	PrtDrvStruct.TphMemoryA = &TphBufC[0][0];	/* TPH MEMORYA */
	PrtDrvStruct.TphMemoryB = &TphBufC[0][0];    /* TPH MEMORYB  Label form */
    #else
	PrtDrvStruct.TphMemoryA = &TphBufC[0][0][0];	/* TPH MEMORYA */
	PrtDrvStruct.TphMemoryB = &TphBufC[1][0][0];    /* TPH MEMORYB  Label form */
    #endif    
    PrtDrvStruct.TphMemory = PrtDrvStruct.TphMemoryA;	/* TPH MEMORY  */
	PrtStruct.PrtMemory = PrtDrvStruct.TphMemoryA;
#endif //USE_SRAM_PRT_RENERING_BUF

//HYP 20060424 Fix Byte Align Problem for rendering
//	PrtStruct.PrtScratchPad = (HUGEDATA INT8U *)PrtScratchPadMemory;
//	PrtStruct.PrtScratchPadFont = (HUGEDATA INT8U *)PrtScratchPadFontMemory;
	PrtStruct.PrtScratchPad = (HUGEDATA INT8U *)PrtScratchPadMemory;
	PrtStruct.PrtScratchPadFont = (HUGEDATA INT8U *)PrtScratchPadFontMemory;
	PrtStruct.StrBuffer	= PrtStrBufferMemory;
	PrtStruct.BarBuffer	= PrtBarBufferMemory;
//  Init PrtDrv variable
	PrtDrvStruct.MainStepMotorSignal = 0x33;
	PrtDrvStruct.RewindStepMotorSignal = 0x33;
	PrtDrvStruct.RewindVariable = REWIND_LENGTH;
	PrtDrvStruct.ExistBackGround = OFF;
	PrtDrvStruct.StbState = STB_STATE0;
	PrtDrvStruct.PrePrtState = PRE_PRT_STATE0;
	PrtDrvStruct.LabelCopyCount = 0;
	PrtSensorFlagInit();
	//PrtDrvStruct.PrtCompleteFlag = OFF;
	PrtDrvStruct.PrtCompleteFlag = ON;	//HYP 20040820
	PrtDrvStruct.PrtLoadFormFlag = ON;
//	PrtDrvStruct.PrtLoadFormFlag = OFF;

	BarInit();
	StrInit();
	FontInit();  //StrInit > fontInit > PrtLoadModeSetting
	PrtLoadModeSetting(PrtFlashStruct.Mode);
#ifdef USE_INTERNAL_ADC
    DRV_ADC0_Open();
#endif
}

void PrtDrvSetPrePrintMode(INT8U pre_prt_mode, INT8U pre_prt_length) {
	PrtDrvStruct.PrePrintMode = pre_prt_mode;
	if (pre_prt_length > PRE_PRINT_AREA_LENGTH_MAX) {
		PrtDrvStruct.PrePrintLength = PRE_PRINT_AREA_LENGTH_MAX;
	} else {
		PrtDrvStruct.PrePrintLength = pre_prt_length;
	}
	PrtDrvStruct.PrePrtState = PRE_PRT_STATE0;
	//HYP 20040330
	if (PrtDrvStruct.LabelGapMode == NO_GAP) {
		PrtDrvStruct.PrePrintMode = DISABLE;
	}
//	GapBufInit();
	//HYP 20060428 prohibit pre-print when use NON_INVERT
	//PrtDrvStruct.PrtDirection = INVERT;//HYP 20070629
}

//HYP 20040330
void PrtDrvSetLabelGapMode(INT8U label_gap_mode) {
	PrtDrvStruct.LabelGapMode = label_gap_mode;
	if((PrtFlashStruct.PrePrintMode == ENABLE) && (PrtDrvStruct.LabelGapMode == GAP)) {
		PrtDrvSetPrePrintMode(ENABLE, PrtFlashStruct.PrePrintLength);
	} else {
		PrtDrvSetPrePrintMode(DISABLE, PrtFlashStruct.PrePrintLength);//HYP 20070629
	}
}

void PrtLoadModeSetting(INT8U mode) {
    INT32U  input_data;
#ifdef USE_TEST_PRT_RENDERING_TIME
    sprintf( MsgBuf, "///LoadModeSetting\r\n" );
    MsgOut( MsgBuf );
#endif


	PrtDrvStruct.StbTimeTrimValue = PrtFlashStruct.StbTimeTrimValue;
	PrtDrvSetPrePrintMode(PrtFlashStruct.PrePrintMode, PrtFlashStruct.PrePrintLength);
	PrtDrvSetSpeed(PrtFlashStruct.Speed); //speed and timer 0
#ifdef USE_SRAM_PRT_RENERING_BUF
    SramPrtMemory = SramTphMemoryA;
#else
	PrtStruct.PrtMemory = PrtDrvStruct.TphMemoryA;//HYP 20061128 correct PrtMemoryClear() is over memory in receipt
#endif

	if(mode == LABEL) {
		PrtStruct.Mode = LABEL;
//		PrtStruct.PrtMemory = PrtDrvStruct.TphMemoryA;//HYP 20061128 move up
		PrtSetLabelSize(PrtFlashStruct.LabelWidth,PrtFlashStruct.LabelHeight);
		//HYP 20040330
		//PrtDrvStruct.LabelGapMode = PrtFlashStruct.LabelGapMode;
		PrtDrvSetLabelGapMode(PrtFlashStruct.LabelGapMode);
		PrtDrvStruct.RewindEnable = PrtFlashStruct.RewindEnable;
//		PrtDrvSetDirection(NON_INVERT);
		PrtDrvSetDirection(INVERT);	   //only label mode
	} else {
		PrtStruct.Mode = RECEIPT;
		PrtStruct.StrBufferCount = 0;
		PrtStruct.ReceiptModeState = RM_COMPLETE;
		PrtSetReceiptWidth(PrtFlashStruct.ReceiptWidth);
		PrtSetReceiptHeight(PrtFlashStruct.ReceiptHeight);		//HYP 20040330
		PrtSetReceiptFeedHeight(PrtFlashStruct.ReceiptFeedHeight);	//HYP 20040330
		//HYP 20040330
		//PrtDrvStruct.LabelGapMode = NO_GAP;
		PrtDrvSetLabelGapMode(PrtFlashStruct.LabelGapMode);
		PrtDrvStruct.RewindEnable = PrtFlashStruct.RewindEnable;
//		PrtDrvSetLabelGapMode(NO_GAP);
//		PrtDrvStruct.RewindEnable = DISABLE;
// 	this value is saved when font1 loading
//		PrtSetHTabPos(PrtFlashStruct.HTabCh);
//		PrtSetVTabPos(PrtFlashStruct.VTabCh);
		PrtDrvSetDirection(NON_INVERT);
		PrtDrvSetPrePrintMode(DISABLE, PrtFlashStruct.PrePrintLength);//HYP 20070629
	}
	PrtDrvStop();
	PrtDrvStruct.PeelEnable = PrtFlashStruct.PeelEnable;

//Init Prt variable 	
	PrtStruct.Opaque = OFF;

	PrtSetPos(0,0);
	PrtSetRot(DEG_0);

	PrtSetMargin(PrtFlashStruct.TMargin,PrtFlashStruct.BMargin,PrtFlashStruct.LMargin,PrtFlashStruct.RMargin);
	PrtMemoryClear();
#ifdef USE_FORMFEED_WITH_PRE_PRINT //HYP 20060428
	PrtDrvTphMemoryClear(OFF);
#else
	PrtDrvTphMemoryClear();
#endif
	//Init feed variable
	PrtDrvStruct.FormFeedVariable = PrtDrvStruct.FormFeedLength;
	PrtDrvStruct.GapLength = PrtFlashStruct.GapLength;
// 	this value is saved when font1 loading
//	PrtDrvStruct.LineFeedLength    = (INT16U)FontStruct.FHbits1*StrStruct.Magy;
//	PrtDrvStruct.ReverseFeedLength = (INT16U)FontStruct.FHbits1*StrStruct.Magy;
//	PrtDrvStruct.LineFeedVariable = PrtDrvStruct.LineFeedLength;
//	PrtDrvStruct.ReverseFeedVariable = PrtDrvStruct.ReverseFeedLength;
	//Insert HYP 20060308
//	PrtDrvStruct.ReverseFeedLength = (INT16U)REVERSE_FEED_LENGTH;
	input_data = Flash_wread(PRT_FLASH_BASE + FLASH_OFFSET_GapBufInPtrReverseTrimValue);
	PrtDrvStruct.ReverseFeedLength = (INT16U)input_data;
	PrtDrvStruct.ReverseFeedVariable = PrtDrvStruct.ReverseFeedLength;
}

void PortInit(void)
{
#ifndef CPU_PIC32MZ
	LATCH	= 1;
	TPHSTB1	= 0;
	TPHSTB2	= 0;
	TPH_OFF;
#ifdef L6219
	STEP_MOTOR_CON(0xff); //L6219
#else
	DCM_OFF;
	STEP_MOTOR_CON(0x0f); //L293D & uni-polar
#endif
#endif //#ifdef CPU_PIC32MZ
}

void PrtDrvStop(void) {
	PrtDrvStruct.PrtState = PRT_STATE0I;
#ifdef TIMER0_ONOFF_CONTROL
	PrtDrvStruct.Timer0OnOff = OFF;
#endif
	if (PrtDrvStruct.PrtDirection == NON_INVERT) {
		PrtDrvStruct.TphMemRowCount = 0;
	} else {
		PrtDrvStruct.TphMemRowCount = PrtDrvStruct.TphMemMaxRowNum - 1;
	}							  
#ifdef L6219
	STEP_MOTOR_CON(0xff); //L6219
#else
	STEP_MOTOR_CON(0x0f); //L293D & uni-polar
	DCM_OFF;
#endif  
	TPH_OFF;
   	PEEL_OFF;
	GAP_OFF;
}

void PrtDrvStart(INT8U	state) {
	PrtDrvStruct.PrtState = state;
#ifdef TIMER0_ONOFF_CONTROL
	PrtDrvStruct.Timer0OnOff = ON;
	TL0 = PrtDrvStruct.Timer0ReloadValueHigh & 0xff;		/* set timer 0 reload value */
	TH0 = PrtDrvStruct.Timer0ReloadValueHigh >> 8;
	TIMER0_PRT_START;			/* start timer 0 ???*/
#endif
}



