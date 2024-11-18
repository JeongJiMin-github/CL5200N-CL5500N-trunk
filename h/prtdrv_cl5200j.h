/*****************************************************************************
|*			
|*  Copyright		:	(c) 2002 CAS
|*  Filename		:	prtdrv_cl5200j.h
|*  Version			:	0.1
|*  Programmer(s)	:	(CJK)
|*  Created			:	2014/11/12
|*  Description		:	CL5200J Printer driver define
|*				
****************************************************************************/

#ifndef _PRTDRV_H
#define _PRTDRV_H

//#define PRT_M_EN_PORT    PORTJbits.RJ13
#define PRT_M_EN_PORT_HIGH    PLIB_PORTS_PinSet( PORTS_ID_0, PORT_CHANNEL_J, PORTS_BIT_POS_13)
#define PRT_M_EN_PORT_LOW    PLIB_PORTS_PinClear( PORTS_ID_0, PORT_CHANNEL_J, PORTS_BIT_POS_13)
//#define PRT_M_PH1_PORT    PORTJbits.RJ15
#define PRT_M_PH1_PORT_HIGH    PLIB_PORTS_PinSet( PORTS_ID_0, PORT_CHANNEL_J, PORTS_BIT_POS_15)
#define PRT_M_PH1_PORT_LOW    PLIB_PORTS_PinClear( PORTS_ID_0, PORT_CHANNEL_J, PORTS_BIT_POS_15)
//#define PRT_M_PH2_PORT    PORTBbits.RB2
#define PRT_M_PH2_PORT_HIGH    PLIB_PORTS_PinSet( PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_2)
#define PRT_M_PH2_PORT_LOW    PLIB_PORTS_PinClear( PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_2)
#ifndef _CL3000_PRINTER_MECH
//#define PRT_R_EN_PORT    PORTJbits.RJ14
#define PRT_R_EN_PORT_HIGH    PLIB_PORTS_PinSet( PORTS_ID_0, PORT_CHANNEL_J, PORTS_BIT_POS_14)
#define PRT_R_EN_PORT_LOW    PLIB_PORTS_PinClear( PORTS_ID_0, PORT_CHANNEL_J, PORTS_BIT_POS_14)
//#define PRT_R_PH1_PORT    PORTAbits.RA10
#define PRT_R_PH1_PORT_HIGH    PLIB_PORTS_PinSet( PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_3)
#define PRT_R_PH1_PORT_LOW    PLIB_PORTS_PinClear( PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_3)
//#define PRT_R_PH2_PORT    PORTBbits.RB1
#define PRT_R_PH2_PORT_HIGH    PLIB_PORTS_PinSet( PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_1)
#define PRT_R_PH2_PORT_LOW    PLIB_PORTS_PinClear( PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_1)
#endif
//#define PRT_LAT_PORT     PORTGbits.RG9
#define PRT_LAT_PORT_HIGH    PLIB_PORTS_PinSet( PORTS_ID_0, PORT_CHANNEL_G, PORTS_BIT_POS_9)
#define PRT_LAT_PORT_LOW    PLIB_PORTS_PinClear( PORTS_ID_0, PORT_CHANNEL_G, PORTS_BIT_POS_9)
//#define PRT_DATA_PORT     PORTCbits.RC4
//#define PRT_DATA_PORT_HIGH    PLIB_PORTS_PinSet( PORTS_ID_0, PORT_CHANNEL_C, PORTS_BIT_POS_4)
//#define PRT_DATA_PORT_LOW    PLIB_PORTS_PinClear( PORTS_ID_0, PORT_CHANNEL_C, PORTS_BIT_POS_4)
//#define PRT_STB1_PORT    PORTAbits.RA0
#define PRT_STB1_PORT_HIGH    PLIB_PORTS_PinSet( PORTS_ID_0, PORT_CHANNEL_A, PORTS_BIT_POS_0)
#define PRT_STB1_PORT_LOW    PLIB_PORTS_PinClear( PORTS_ID_0, PORT_CHANNEL_A, PORTS_BIT_POS_0)
//#define PRT_STB2_PORT    PORTBbits.RB5
#define PRT_STB2_PORT_HIGH    PLIB_PORTS_PinSet( PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_5)
#define PRT_STB2_PORT_LOW    PLIB_PORTS_PinClear( PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_5)
//#define SENSOR_ON_PORT   PORTBbits.RB4
#define SENSOR_ON_PORT_HIGH    PLIB_PORTS_PinSet( PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_4)
#define SENSOR_ON_PORT_LOW    PLIB_PORTS_PinClear( PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_4)
//#define TPH_ON_PORT      PORTKbits.RK0
#define TPH_ON_PORT_HIGH    PLIB_PORTS_PinSet( PORTS_ID_0, PORT_CHANNEL_K, PORTS_BIT_POS_0)
#define TPH_ON_PORT_LOW    PLIB_PORTS_PinClear( PORTS_ID_0, PORT_CHANNEL_K, PORTS_BIT_POS_0)

#define	NON_INVERT	0
#define	INVERT		1

#define GAP		1
#define NO_GAP	0

#define GAP_STATUS_LABEL		2
#define GAP_STATUS_GAP			1
#define GAP_STATUS_OUTOFPAPER	0

#define TPH_ROW_BYTE	 56		 //56 mm width TPH
#define L6219 //if uni-polar step motor, this define must be off
//#define TIMER0_ONOFF_CONTROL
#define PRT_FIXED_SPEED_100
#define PRT_REVERSE_FEED

#define PEEL_SENSOR_REFLECTIVE

#ifdef USE_BARCODE_LEN_90
#define MAX_BAR_WIDTH 120	// Max Barcode Width : 120mm
#else
#define MAX_BAR_WIDTH 80	// Max Barcode Width : 80mm
#endif
//#define USE_RTC_TRCHA

//HYP 20060428 when press FormFeed Key 
//if this option is defined, Label is printed in pre-print area is printing(previous shop name, ...)
//if this option is not defined, blank label is printing.
//now not complete, when load label, shop data field must be rendered 
//and when shop date change, reload label(with shop rendering)
//#define USE_FORMFEED_WITH_PRE_PRINT

//#define TPH_STB_ON_AT_THE_SAME_TIME
//HYP 20060424
//#define STB_TIME_DIV	50
#define STB_TIME_DIV	13 //13*20 = max. 260ms
#define PCB_REV04

#define	TPH_ON		TPH_ON_PORT_HIGH;
#define	TPH_OFF		TPH_ON_PORT_LOW;
#define	SENSOR_ON	SENSOR_ON_PORT_LOW;
#define	SENSOR_OFF	SENSOR_ON_PORT_HIGH;

#define	PEEL_ON		SENSOR_ON;
#define	PEEL_OFF	SENSOR_OFF;
#define	GAP_ON		SENSOR_ON;
#define	GAP_OFF		SENSOR_OFF;

#define CASH_OFF				PLIB_PORTS_PinClear (PORTS_ID_0, PORT_CHANNEL_D, PORTS_BIT_POS_9 )
#define CASH_ON					PLIB_PORTS_PinSet (PORTS_ID_0, PORT_CHANNEL_D, PORTS_BIT_POS_9 )

#define CASH_SEN_OPEN		0
#define CASH_SEN_CLOSE		1			

//#define LATCH		PRT_LAT_PORT
//#define TPHSTB1		PRT_STB1_PORT
//#define TPHSTB2		PRT_STB2_PORT

extern INT8U IsHeadupState(void);
#define HEADUP_THREADHOLD	128
#define HEADUP	IsHeadupState()
#define HEADUP_PORT PLIB_PORTS_PinGet(PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_0)//PORTBbits.RB0

#ifdef USE_PB_BATTERY
#define AC_CHECK	Gpio4->Bit.b5
//#define AC_CHECK	0	// for test
#endif

#define CAL_PORT PLIB_PORTS_PinGet(PORTS_ID_0, PORT_CHANNEL_A, PORTS_BIT_POS_9)//PORTAbits.RA9

//Ethernet 100Base Led
#define   ETH_LED_100  PLIB_PORTS_PinGet(PORTS_ID_0, PORT_CHANNEL_H, PORTS_BIT_POS_15)//PORTAbits.RH15
//Ethernet Link Led
#define   ETH_LED_LINK   PLIB_PORTS_PinGet(PORTS_ID_0, PORT_CHANNEL_H, PORTS_BIT_POS_14)//PORTAbits.RH14

//#define BUZZER  PORTDbits.RD13
#define BUZZER_HIGH   PLIB_PORTS_PinSet( PORTS_ID_0, PORT_CHANNEL_D, PORTS_BIT_POS_13)
#define BUZZER_LOW    PLIB_PORTS_PinClear( PORTS_ID_0, PORT_CHANNEL_D, PORTS_BIT_POS_13)

extern INT8U IsCalState(void);
#define CAL		(IsCalState() ? 0 : 1)

#define PEEL_OFF_AD_PORT	ADRSH0
#define TEMP_AD_PORT		ADRSH1
#define GAP_SENSOR_AD_PORT	ADRSH2

#define LABEL				0
#define RECEIPT				1

#define MAX_LABEL_COUNT_IN_SENSOR	5

#ifdef USE_ONLY_1EA_PRT_RENDERING_BUF
    #ifdef USE_SRAM_PRT_RENERING_BUF
        #ifdef USE_CONTINUOUS_LABEL_WITH_INGREDIENT
#define MAX_FORM_LEN 	2320     //3200(step) * 0.125(mm/step) = 400mm
        #else
#define MAX_FORM_LEN 	2320     // 2320(290mm MaxSize of SRAM) //1600(200mm H44960) //1200(150mm H72000) //960(120mm H72000 QR-ADD_30 OK)//640(80mm H72000)//1600
        #endif                   // 640(step) * 0.125(mm/step) = 80mm
    #else //  USE_SRAM_PRT_RENERING_BUF
        #ifdef USE_CONTINUOUS_LABEL_WITH_INGREDIENT
#define MAX_FORM_LEN 	640		//3200(step) * 0.125(mm/step) = 400mm
    #else  // USE_CONTINUOUS_LABEL_WITH_INGREDIENT
#define MAX_FORM_LEN 	640     // 2320(290mm MaxSize of SRAM) //1600(200mm H44960) //1200(150mm H72000) //960(120mm H72000 QR-ADD_30 OK)//640(80mm H72000)//1600
    #endif // USE_CONTINUOUS_LABEL_WITH_INGREDIENT                          //640(step) * 0.125(mm/step) = 80mm
    #endif //  USE_SRAM_PRT_RENERING_BUF
#define MAX_RECEIPT_LEN		MAX_FORM_LEN//2800//960//480		//2800=350mm
#else

#ifdef USE_CONTINUOUS_LABEL_WITH_INGREDIENT
#define MAX_FORM_LEN 	640		//3200(step) * 0.125(mm/step) = 400mm
#else
#define MAX_FORM_LEN 	640//1600		//1600(step) * 0.125(mm/step) = 200mm
#endif
#define MAX_RECEIPT_LEN		MAX_FORM_LEN//2800//960//480		//2800=350mm

#endif//#ifdef USE_ONLY_1EA_PRT_RENDERING_BUF

#define MAX_GAP_LEN 	80			//80(step) * 0.125(mm/step) = 10mm
#define MAX_LEN_ERR		64			//64(step) * 0.125(mm/step) = 8mm
#define MAX_CHECK_FEED_LEN	(3*(MAX_FORM_LEN+MAX_GAP_LEN)+MAX_LEN_ERR)
#define MAX_CHECK_FEED_LEN_NO_GAP	320	//40mm

#define PRE_PRINT_AREA_LENGTH_MAX	(8*10)

#define REWIND_LENGTH	680   //680(step) * 0.125(mm/step) = 85mm
#pragma pack(4)
typedef struct {
//not save for initial
	HUGEDATA INT8U *TphMemory;	/* TPH MEMORY */
	HUGEDATA INT8U *TphMemoryA;	/* TPH MEMORYA*/
	HUGEDATA INT8U *TphMemoryB;	/* TPH MEMORYB*/
	INT8U PrtState, StbState, PrePrtState;
	INT16U TphMemMaxRowNum,	PrevTphMemMaxRowNum; //do not use for rendering
	//TphMemMaxRowNum == PrtStruct.PrintableHeight(Label Mode) but != (Receipt Mode) 
	INT16U TphMemRowCount;//, TphMemRowByteCount;
	INT8U 	MainStepMotorSignal, RewindStepMotorSignal;
	INT16U Timer0ReloadValueLow, Timer0ReloadValueHigh, Timer0ReloadValueInterval;
	INT16U Timer0AccelIntervalValue, Timer0AccelIntervalStartValue;
	INT16U Timer0ReloadValue, PCATimerReloadValue, PCATimerReloadValue2;
	INT16U SysTimer;
 	INT16U FormFeedLength, LineFeedLength, ReverseFeedLength;
	INT16U FormFeedVariable, LineFeedVariable, ReverseFeedVariable, RewindVariable;
	//HYP 20040330
	INT16U ReceiptFeedVariable;
	INT16U LabelCopyCount;
	INT16U FindGapFeedCount, FindLabelFeedCount;
	INT8U SysTimerNum10ms;
	INT16U Timer10ms;
	BOOLEAN Timer0OnOff;
	BOOLEAN	HangingFormFlag, OutOfPaperFlag, HeadUpFlag;
	BOOLEAN	GapTraceFlag;
	BOOLEAN	GapSenErrFlag;
	BOOLEAN ExistBackGround;
	BOOLEAN PrtDirection;
	BOOLEAN PrtCompleteFlag;
	BOOLEAN PrtLoadFormFlag;//Copy label background image when complete a printing job (in label mode)
	INT8U GapSensorStatus;//HYP 20101021    
//save for initial
	INT16U GapLength;
	INT16U StbTimeTrimValue;
	INT8U Speed;
	BOOLEAN PeelEnable, LabelGapMode, RewindEnable;
	BOOLEAN PrePrintMode;	//FGD DISABLE
	INT8U	PrePrintLength;
	//HYP 20040330
	INT16U ReceiptFeedHeight;
}PRTDRV_STRUCT;
#pragma pack()

extern PRTDRV_STRUCT PrtDrvStruct;//HYP 20060412 _huge --> _near for small code size

extern void STEP_MOTOR_CON(INT8U  data);
extern void PrtDrvInit(void);
extern void PrtSensorFlagInit(void);
extern void PortInit(void);
extern void PrtDrvStart(INT8U	state);
extern void PrtDrvStop(void); 

#ifdef USE_FORMFEED_WITH_PRE_PRINT
extern void PrtDrvTphMemoryCopy(HUGEDATA INT16U *memA, HUGEDATA INT16U *memB,INT8U flag_FF);
extern void PrtDrvTphMemoryClear(INT8U flag_FF);
#else	
extern void PrtDrvTphMemoryCopy(HUGEDATA INT16U *memA, HUGEDATA INT16U *memB);
extern void PrtDrvTphMemoryClear(void);
#endif
extern void PrtDrvSetDirection(BOOLEAN direction);
extern void PrtDrvSetSpeed(INT8U speed);
extern void PrtDrvSetPrePrintMode(INT8U pre_mode, INT8U pre_length);

#endif //_PRTDRV_H
