/*****************************************************************************
|*
|*  Copyright		:	(c) 2002 CAS
|*  Filename		:	sensor.c
|*  Version			:	0.1
|*  Programmer(s)	:	Hong Yi-Phyo (HYP)
|*  Created			:	2002/11/11
|*  Description		:	CLP Printer AD(sensor) routine
|*
****************************************************************************/

#include <stdio.h>
#include "globals.h"
#include "commbuf.h"
#include "prt_common.h"
#include "prtdrv.h"
#include "sensor.h"
#include "timer.h"
#include "flash.h"
#include "initial.h"
#include "flash_app.h"


#define MAX_STB_TIME 580

HUGEDATA SENSOR_STRUCT SensorStruct;
INT8U GapBufMemory[GAP_SENSOR_BUF_SIZE];

ROMDATA INT16U StbTimeTable[0x100]={
	34,
	34,34,34,34,34,34,34,34,34,34,
	34,34,34,34,34,34,34,34,34,34,
	34,34,34,34,34,
	34,34,34,34,34,
	34,34,34,34,34,
	36,55,74,92,109,  //   0~   40 (70~ )


	123,136,149,162,174,
	185,193,200,208,216,
	220,223,226,229,232,235,238,240,		//  41~   58 (60~70)

	241,242,243,244,245,246,247,248,249,250,	//  59-   68 (50~60)
	251,252,253,254,255,256,257,258,259,260,

	261,262,263,264,265,266,267,268,269,270,
	271,272,272,							//  69 -  91 (40~50)
	273,273,274,274,275,275,276,276,277,277,
	278,278,279,279,280,280,281,281,282,282,

	282,282,283,283,284,284,285,285,286,286,
	287,									//  92 - 122 (30~40)

	287,288,288,289,289,290,290,291,291,292,
	292,293,
	293,294,294,295,295,296,296,297,
	297,298,298,299,299,					// 123 - 147 (20~30)

	300,301,302,303,304,305,306,307,308,309,
 	310,311,312,312,313,314,315,316,317,318,
	319,320,321,322,323,324,325,326,		// 148 ~ 175 (10~20)

	328,330,332,334,336,338,340,342,345,347,
	349,351,
	354,357,360,363,366,369,372,375,
	378,381,384,387,390,					// 176 ~ 200 (0~10)

	394,398,401,404,410,413,416,419,422,425,
	428,431,434,437,440,443,446,449,		// 201 ~ 218 (-10 ~ 0)

	452,455,458,461,464,467,470,473,476,479,
	482,485,488,491,494,497,500,504,506,508,
	510,511,512,513,514,515,516,
//	0xF0CD,0xF0C5,0xF0BD,0xF0B5,0xF0AD,0xF0A5,0xF09D,0xF095,0xF000,0xF000	// 219 ~ 255 ( ~ -10)
	34,34,34,34,34,34,34,34,34,34  //Temp Sensor error
};

void SensorGapTrim(short trim_value) {
	short i;

	if(trim_value >= 0) {
		for (i= 0; i < trim_value; i++) {
			if (SensorStruct.GapBufOutPtr >= (GAP_SENSOR_BUF_SIZE-1)) SensorStruct.GapBufOutPtr = 0;
			else SensorStruct.GapBufOutPtr++;
		}
	} else {
		for (i= trim_value; i < 0; i++) {
			if (SensorStruct.GapBufOutPtr == 0) SensorStruct.GapBufOutPtr = (GAP_SENSOR_BUF_SIZE-1);
			else SensorStruct.GapBufOutPtr--;
		}
	}
}

void SensorInit(void)
{
/*
    ADMOD = 0;					// single scan mode
    ADRES = 0;					// Set to 8-bit operation
	ADCFG = 0x0b;	  //ad speed 4.87 usec
	ADCS0 = 1;	   //peel off
	ADCS1 = 1;	   //CLP Printer : SENSE	BASIC :temp
	ADCS2 = 1;	   //Gap
#ifdef _CLP_PRINTER_
	ADCS3 = 1;	  //CLP Printer : temp
#endif
    IPA3 = IPA3 & 0xF0;
    IPA3 = IPA3 | 0x01;
	ADINT = 0;
	EAD = 1;
*/
	Flash_sread(PRT_FLASH_BASE, (HUGEDATA INT8U *)&PrtFlashStruct, PRT_FLASH_SET_STRUCT_SIZE);

	SensorStruct.GapThreshold = PrtFlashStruct.GapThreshold;
	SensorStruct.PeelThreshold = PrtFlashStruct.PeelThreshold;
//HYp 20040223
	SensorStruct.GapOutOfPaperThreshold = PrtFlashStruct.GapOutOfPaperThreshold;
	SensorStruct.GapOutOfPaperThresholdOnLabel = PrtFlashStruct.GapOutOfPaperThresholdOnLabel;
//	SensorStruct.SenFilCount = 0;
	SensorStruct.FilterCounter = 0;
// Initialize gap buffer
	GapBufInit();
}

INT16U PreFindGapFeedCount;

INT8U ADRSH0;
INT8U ADRSH1;
INT8U ADRSH2;

// China Cart Scale
#ifdef USE_PB_BATTERY
#define MAX_BAT_AD_FILTER_NUM	40

#define BAT_MAX_REF_VOLTAGE 3.3
#define BAT_MAX_AD_COUNT 1024
#define BAT_AD_CUT_COUNT 1	// 1bit
#define BAT_VOLT_DIVIDE 11	// (100kohm+10kohm)/10kohm
extern INT16U BatVoltVal;

void Read_BatAd(void)
{
	INT8U count;
	INT16U bat_sum;
	static INT16U batVoltBuf[MAX_BAT_AD_FILTER_NUM];
	static INT16U filterCounter = 0;

	bat_sum = 0;

	batVoltBuf[filterCounter] = ADC_GetConversionValue(ADC_Channel_4)>>0;//BATTERY

	filterCounter++;
	if(filterCounter < MAX_BAT_AD_FILTER_NUM) {
//	 	ADSST = 1; //if you control AD start and stop, use this variable!!
//		ADC_ConversionCmd(ADC_Conversion_Start);
	} else {
		filterCounter = 0;
		for (count = 0; count < MAX_BAT_AD_FILTER_NUM; count++) {
			bat_sum = bat_sum + batVoltBuf[count];
		}
		BatVoltVal = (INT16U)(bat_sum / MAX_BAT_AD_FILTER_NUM);
	}
}

float ConvertBatVolatage(INT16U ad)
{
	float temp;

	temp = (float)ad;
	temp *= BAT_MAX_REF_VOLTAGE;
	temp /= (float)(BAT_MAX_AD_COUNT/BAT_AD_CUT_COUNT);
	temp *= (float)BAT_VOLT_DIVIDE; // (100kohm+10kohm)/10kohm
	return temp;
}
#endif

#define PRT_AD_GAP           0
#define PRT_AD_PEEL          1
#define PRT_AD_TEMP          2
//#define PRT_AD_HEADUP        3
#define PRT_AD_MAX           3

#ifdef USE_INTERNAL_ADC
#define PRT_AD_GAP_PIN       ADCHS_AN29
#define PRT_AD_PEEL_PIN      ADCHS_AN7
#define PRT_AD_TEMP_PIN      ADCHS_AN8
#endif

#ifdef USE_EXTERNAL_ADC
#define DIN_START            0
#define DIN_END              5
#define DOUT_CONV_START      5
#define DOUT_DATA_START      16
#define MAX_CLK              26

#define CS_HIGH PLIB_PORTS_PinSet( PORTS_ID_0, PORT_CHANNEL_D, PORTS_BIT_POS_14);
#define CS_LOW  PLIB_PORTS_PinClear( PORTS_ID_0, PORT_CHANNEL_D, PORTS_BIT_POS_14);
#define DI_HIGH PLIB_PORTS_PinSet( PORTS_ID_0, PORT_CHANNEL_F, PORTS_BIT_POS_2);
#define DI_LOW  PLIB_PORTS_PinClear( PORTS_ID_0, PORT_CHANNEL_F, PORTS_BIT_POS_2);
#define SCLK_HIGH   PLIB_PORTS_PinSet( PORTS_ID_0, PORT_CHANNEL_D, PORTS_BIT_POS_15);
#define SCLK_LOW    PLIB_PORTS_PinClear( PORTS_ID_0, PORT_CHANNEL_D, PORTS_BIT_POS_15);
#define DOUT        PLIB_PORTS_PinGet(PORTS_ID_0, PORT_CHANNEL_F, PORTS_BIT_POS_8);
#endif

extern void prt_delay(INT8U cnt);

INT16U sensorVal[PRT_AD_MAX];

INT8U AdcCmdTable[4] = //ch0, ch1, ch2, ch3
{
	0x03, 0x13, 0x0b, 0x1b
};

#ifdef USE_EXTERNAL_ADC
void __attribute__((optimize("-O0"))) sensorAdcGet(INT8U type)
{
    INT32U i;
    INT32U result = 0;
    //u32 port;
    INT32U tmp = 0, tmp2 = 0;

    tmp = AdcCmdTable[type];

    CS_LOW;
    for (i = DIN_START; i < DIN_END; i++) //DATA IN
    {
        //delay_1us(1);
	SCLK_LOW;
        //delay_1us(1);
	if ( (tmp >> i) & 0x01)
	{
            DI_HIGH;
	}
	else
	{
	    DI_LOW;
	}
        prt_delay(10);//delay_1us(1);
        SCLK_HIGH;
        prt_delay(10);//delay_1us(1);
    }
///*****************************************************************************
/*
 *  Single-ended ADC CH0 : 1X00 (AdcCmdTable ch 0 : 00011) >> & 11000
 *  Single-ended ADC CH1 : 1X01 (AdcCmdTable ch 1 : 10011) >> & 11001
 *  Single-ended ADC CH2 : 1X10 (AdcCmdTable ch 2 : 01011) >> & 11010
 *  Single-ended ADC CH3 : 1X11 (AdcCmdTable ch 3 : 11011) >> & 11011
*/
//****************************************************************************/
    DI_LOW; //datasheet need don't care.

    for (i = DOUT_CONV_START; i < MAX_CLK; i++)
    {
	SCLK_LOW;

        if (i >= DOUT_DATA_START && i < MAX_CLK)
        {
            tmp2 = DOUT;
            result |= (tmp2 & 0x01) << (i-DOUT_DATA_START); //LSB
        }
        prt_delay(10);//delay_1us(1);
	SCLK_HIGH;
        prt_delay(10);//delay_1us(1);
    }
    CS_HIGH;

    if (result != 0)
    {
        sensorVal[type] = result;
    }
}
#elif defined (USE_INTERNAL_ADC)
void __attribute__((optimize("-O0"))) sensorAdcGet(INT8U type)
{
    INT32U result = 0;

    switch ( type )
    {
        case PRT_AD_GAP:
        {
            if ( DRV_ADC_SamplesAvailable(PRT_AD_GAP_PIN) )
            {
                result = DRV_ADC_SamplesRead(PRT_AD_GAP_PIN);
        	}
            break;
        }
        case PRT_AD_PEEL:
        {
            if ( DRV_ADC_SamplesAvailable(PRT_AD_PEEL_PIN) )
            {
                result = DRV_ADC_SamplesRead(PRT_AD_PEEL_PIN);
        	}
            break;
        }
        case PRT_AD_TEMP:
        {
            if ( DRV_ADC_SamplesAvailable(PRT_AD_TEMP_PIN) )
            {
                result = DRV_ADC_SamplesRead(PRT_AD_TEMP_PIN);
        	}
            break;
        }
    }

    if (result != 0)
    {
        sensorVal[type] = result;
    }
}
#endif      // #ifdef USE_EXTERNAL_ADC #elif USE_INTERNAL_ADC

void sensorProc()
{
	INT8U i;

    #ifdef USE_INTERNAL_ADC
    DRV_ADC_Start();
    #endif
	for (i = 0; i < PRT_AD_MAX; i++)
	{
		sensorAdcGet(i);
	}
    #ifdef USE_INTERNAL_ADC
    DRV_ADC_Stop();
    #endif
}
extern char MsgBuf[];
void AdInt(void)
{
	INT8U count;
	INT16U peel_sum, gap_sum, temp_sum;

//if (ADC_GetFlagStatus(ADC_FLAG_ECV) != RESET){	//check adc-complete //non interrupt type test

#ifdef USE_PB_BATTERY
	if (SensorStruct.FilterCounter == 0)
	{
		Read_BatAd();	// read Bat AD
		if (PrtDrvStruct.PrtState == PRT_STATE0I)
		{
			ADC_ClearFlag(ADC_FLAG_ECV);
			return;
		}
	}
#endif

	peel_sum = 0;
	gap_sum = 0;
	temp_sum = 0;

	sensorProc();
ADC_RESTART:
#ifdef USE_INTERNAL_ADC
    // 기존 외부 ADC경우 10bit Output을 2bit shift 연산하여 1 Byte로 사용하였지만
    // EFG 내부 ADC의 경우 12bit 결과를 출력하여 4bit shift 연산을 수행
	SensorStruct.GapRealVal = (INT8U)(sensorVal[PRT_AD_GAP]>>4);
	SensorStruct.PeelRealVal = (INT8U)(sensorVal[PRT_AD_PEEL]>>4);
	SensorStruct.TempRealVal = (INT8U)(sensorVal[PRT_AD_TEMP]>>4);

	SensorStruct.GapVal[SensorStruct.FilterCounter] = (INT8U)(sensorVal[PRT_AD_GAP]>>4);
	SensorStruct.PeelVal[SensorStruct.FilterCounter] = (INT8U)(sensorVal[PRT_AD_PEEL]>>4);
	SensorStruct.TempVal[SensorStruct.FilterCounter] = (INT8U)(sensorVal[PRT_AD_TEMP]>>4);
#elif defined (USE_EXTERNAL_ADC)
	SensorStruct.GapRealVal = (INT8U)(sensorVal[PRT_AD_GAP]>>2);
	SensorStruct.PeelRealVal = (INT8U)(sensorVal[PRT_AD_PEEL]>>2);
	SensorStruct.TempRealVal = (INT8U)(sensorVal[PRT_AD_TEMP]>>2);
	//SensorStruct.HeadUpRealVal = (INT8U)(sensorVal[PRT_AD_HEADUP]>>2);

	SensorStruct.GapVal[SensorStruct.FilterCounter] = (INT8U)(sensorVal[PRT_AD_GAP]>>2);
	SensorStruct.PeelVal[SensorStruct.FilterCounter] = (INT8U)(sensorVal[PRT_AD_PEEL]>>2);
	SensorStruct.TempVal[SensorStruct.FilterCounter] = (INT8U)(sensorVal[PRT_AD_TEMP]>>2);
	//SensorStruct.HeadUpVal[SensorStruct.FilterCounter] = (INT8U)(sensorVal[PRT_AD_HEADUP]>>2);
#endif

	SensorStruct.FilterCounter++;
	if(SensorStruct.FilterCounter < MAX_FILTER_NUM) {
//	 	ADSST = 1; //if you control AD start and stop, use this variable!!
		goto ADC_RESTART;
	} else {
		SensorStruct.FilterCounter = 0;
		//HYP 20040223
		for (count = 0; count < MAX_FILTER_NUM; count++) {
			peel_sum = peel_sum + SensorStruct.PeelVal[count];
			gap_sum = gap_sum + SensorStruct.GapVal[count];
			temp_sum = temp_sum + SensorStruct.TempVal[count];
		}

		SensorStruct.PeelFilVal = (INT8U)(peel_sum / MAX_FILTER_NUM);
		SensorStruct.GapFilVal = (INT8U)(gap_sum / MAX_FILTER_NUM);
		SensorStruct.TempFilVal = (INT8U)(temp_sum / MAX_FILTER_NUM);
//sprintf(MsgBuf, "HeadVal=%d\r\n", SensorStruct.HeadUpFilVal);
//PutStrTxBuf(&CommBuf, MsgBuf);

		//HYP 20040223
		if (PrtDrvStruct.LabelGapMode == NO_GAP) {
			if (SensorStruct.GapFilVal > SensorStruct.GapOutOfPaperThreshold) {
				PrtDrvStruct.OutOfPaperFlag = ON;
			} else {
				PrtDrvStruct.OutOfPaperFlag = OFF;
			}
		}

		//hardware limit = 540us
		PrtDrvStruct.PCATimerReloadValue = StbTimeTable[SensorStruct.TempFilVal];
		PrtDrvStruct.PCATimerReloadValue = PrtDrvStruct.PCATimerReloadValue + PrtDrvStruct.StbTimeTrimValue;//increase

		//HYP 20040811
//		PrtDrvStruct.PCATimerReloadValue2 = (PrtDrvStruct.PCATimerReloadValue * 32) / 29; //CL5000
		PrtDrvStruct.PCATimerReloadValue2 = PrtDrvStruct.PCATimerReloadValue;//SMPS goldmac, samick (chess)
		if(PrtDrvStruct.PCATimerReloadValue2 > MAX_STB_TIME) PrtDrvStruct.PCATimerReloadValue2=MAX_STB_TIME;

		//HYP 20040513 stb time margin
		if(PrtDrvStruct.PCATimerReloadValue > MAX_STB_TIME) PrtDrvStruct.PCATimerReloadValue=MAX_STB_TIME;

		if(SensorStruct.GapFilVal>SensorStruct.MaxGapLastValue) {
			SensorStruct.MaxGapLastValue = SensorStruct.GapFilVal;
		}
		if(SensorStruct.GapFilVal<SensorStruct.MinGapLastValue) {
			SensorStruct.MinGapLastValue = SensorStruct.GapFilVal;
		}
		if(SensorStruct.PeelFilVal>SensorStruct.MaxPeelLastValue) {
			SensorStruct.MaxPeelLastValue = SensorStruct.PeelFilVal;
		}
		if(SensorStruct.PeelFilVal<SensorStruct.MinPeelLastValue) {
			SensorStruct.MinPeelLastValue = SensorStruct.PeelFilVal;
		}

		if(PrtDrvStruct.PrtState == PRT_STATE_SC) {
			//HYP 20040223
			if(SensorStruct.GapFilVal>SensorStruct.MaxGapValue) {
				SensorStruct.MaxGapValue = SensorStruct.GapFilVal;
			}
			if(SensorStruct.GapFilVal<SensorStruct.MinGapValue) {
				SensorStruct.MinGapValue = SensorStruct.GapFilVal;
			}
			if(SensorStruct.PeelFilVal>SensorStruct.MaxPeelValue) {
				SensorStruct.MaxPeelValue = SensorStruct.PeelFilVal;
			}
			if(SensorStruct.PeelFilVal<SensorStruct.MinPeelValue) {
				SensorStruct.MinPeelValue = SensorStruct.PeelFilVal;
			}

			if(SensorStruct.GapFilVal >= SensorStruct.MaxGapValue - MIN_GAP_SEN_RANGE) {	   //gap
				SensorStruct.TestPreGapFlag = SensorStruct.TestGapFlag;
				SensorStruct.TestGapFlag = 1;
				if(SensorStruct.TestGapFlag != SensorStruct.TestPreGapFlag && SensorStruct.TestGapStartFlag == 0) {
					SensorStruct.TestGapStartFlag = 1;
				}
				if(SensorStruct.TestLabelStartFlag == 1) {
					SensorStruct.TestLabelStartFlag = 2;
				}
			} else if(SensorStruct.GapFilVal <= SensorStruct.MinGapValue + MIN_GAP_SEN_RANGE) {	   //gap
				SensorStruct.TestPreGapFlag = SensorStruct.TestGapFlag;
				SensorStruct.TestGapFlag = 0;	 												//label
				if(SensorStruct.TestGapFlag != SensorStruct.TestPreGapFlag && SensorStruct.TestLabelStartFlag == 0) {
					SensorStruct.TestLabelStartFlag = 1;
				}
				if(SensorStruct.TestGapStartFlag == 1) {
					SensorStruct.TestGapStartFlag = 2;
				}
			}
		} else if (PrtDrvStruct.PrtState == PRT_STATE_LC){
			if(SensorStruct.GapFilVal >= SensorStruct.GapThreshold) {	   //gap
				SensorStruct.TestPreGapFlag = SensorStruct.TestGapFlag;
				SensorStruct.TestGapFlag = 1;
				if(SensorStruct.TestGapFlag != SensorStruct.TestPreGapFlag && SensorStruct.TestGapStartFlag == 0) {
					SensorStruct.TestGapLength = 0;
					SensorStruct.TestGapStartFlag = 1;
				}
				if(SensorStruct.TestGapStartFlag == 1) {
					SensorStruct.TestGapLength++;
				}
				if(SensorStruct.TestLabelStartFlag == 1) {
					SensorStruct.TestLabelStartFlag = 2;
				}
			} else {
				SensorStruct.TestPreGapFlag = SensorStruct.TestGapFlag;
				SensorStruct.TestGapFlag = 0;	 												//label
				if(SensorStruct.TestGapFlag != SensorStruct.TestPreGapFlag && SensorStruct.TestLabelStartFlag == 0) {
					SensorStruct.TestLabelLength = 0;
					SensorStruct.TestLabelStartFlag = 1;
				}
				if(SensorStruct.TestLabelStartFlag == 1) {
					SensorStruct.TestLabelLength++;
				}
				if(SensorStruct.TestGapStartFlag == 1) {
					SensorStruct.TestGapStartFlag = 2;
				}
			}
		}

	}
//}
}

void GapBufInit(void)
{
	short count;
	short length,out_ptr,sensor_width;
	INT8U state;
	INT32U 	input_data;

	SensorStruct.GapBufInPtr = GAP_SENSOR_MECHANICAL_OFFSET-1;
	SensorStruct.GapBuf = GapBufMemory;
	SensorStruct.GapBufOutPtr = 0;
	SensorStruct.GapBufInPtrTrimValue = PrtFlashStruct.GapBufInPtrTrimValue;
	SensorGapTrim(SensorStruct.GapBufInPtrTrimValue);
	input_data = Flash_wread(PRT_FLASH_BASE + FLASH_OFFSET_GapBufInPtrReverseTrimValue);
	PrtDrvStruct.ReverseFeedLength = (INT16U)input_data;

	out_ptr = SensorStruct.GapBufOutPtr;
	if(SensorStruct.GapBufInPtr >= SensorStruct.GapBufOutPtr) {
		sensor_width = SensorStruct.GapBufInPtr - SensorStruct.GapBufOutPtr;
	} else {
		sensor_width = SensorStruct.GapBufInPtr + ((GAP_SENSOR_BUF_SIZE-1) - SensorStruct.GapBufOutPtr);
	}
	//HYP 20040223
//	if(PrtFlashStruct.Mode == LABEL) {
	if((PrtFlashStruct.Mode == LABEL) && (PrtFlashStruct.LabelGapMode == GAP)) {
		state = 0;
		if(PrtFlashStruct.PrePrintMode == ON) {
			length = PrtFlashStruct.LabelHeight - PrtFlashStruct.PrePrintLength;
		} else {
			length = PrtFlashStruct.LabelHeight;
		}

		for (count = 0 ; count < sensor_width; count++) {
			if (state == 0) {//label
				SensorStruct.GapBuf[out_ptr] = 0x00;
				if (out_ptr >= (GAP_SENSOR_BUF_SIZE-1)) out_ptr = 0;
				else out_ptr++;

				if (length <= count) {
					state = 1;
					length = length + PrtFlashStruct.GapLength;
				}
			} else {//gap
				SensorStruct.GapBuf[out_ptr] = 0xff;
				if (out_ptr >= (GAP_SENSOR_BUF_SIZE-1)) out_ptr = 0;
				else out_ptr++;
				if (length <= count) {
					state = 0;
					length = length + PrtFlashStruct.LabelHeight;
				}
			}
		}
	} else {//PrtStruct.Mode == RECEIPT || PrtFlashStruct.LabelGapMode != GAP
		for (count = 0; count < SensorStruct.GapBufInPtr; count++) {
			SensorStruct.GapBuf[count] = 0x00;
		}
	}

}
