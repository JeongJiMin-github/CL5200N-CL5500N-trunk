/*****************************************************************************
|*			
|*  Copyright		:	(c) 2002 CAS
|*  Filename		:	sensor.h
|*  Version			:	0.1
|*  Programmer(s)	:	Hong Yi-Phyo (HYP)
|*  Created			:	2002/11/11
|*  Description		:	CLP Printer AD(sensor) routine header    
|*				
****************************************************************************/

#ifndef _SENSOR_H
#define _SENSOR_H

#define MAX_FILTER_NUM	1

//#define GAP_SENSOR_MECHANICAL_OFFSET	660    //660(step) * 0.125(mm/step) = 76.5mm
//#define GAP_SENSOR_MECHANICAL_OFFSET	544    //544(step) * 0.125(mm/step) = 68mm
//#define GAP_SENSOR_MECHANICAL_OFFSET	464    //464(step) * 0.125(mm/step) = 58mm
//HYP 20041021 for recent mechanism

#ifdef _CL5500_D_PRINTER_MECH
#define GAP_SENSOR_MECHANICAL_OFFSET	368    //368(step) * 0.125(mm/step) = 46mm,기존에 440이었는데 2011년 11월에 기구 변경으로 9mm이동.
#elif defined (_CL3000_PRINTER_MECH)
#define GAP_SENSOR_MECHANICAL_OFFSET	369    //369(step) * 0.125(mm/step) = 46.125mm  // 실제 직선거리 :45.4mm (CL3000)
#elif defined (_CL5500_PRINTER_MECH)
#define GAP_SENSOR_MECHANICAL_OFFSET	424    //424(step) * 0.125(mm/step) = 53mm)
#else
#define GAP_SENSOR_MECHANICAL_OFFSET	492    //492(step) * 0.125(mm/step) = 61.5mm  // 실제 직선거리 :60.97mm (CL5200J)
#endif

#define GAP_SENSOR_BUF_SIZE				GAP_SENSOR_MECHANICAL_OFFSET+MAX_FORM_LEN+MAX_GAP_LEN+MAX_LEN_ERR+10
#define GAP_SENSOR_DEFAULT_THRESHOLD	0x80	// A/D threshold for form-feed sensing
#define PEEL_SENSOR_DEFAULT_THRESHOLD	0x80	// A/D threshold for hanging-form sensing
//HYP 20040223
#define GAP_OUTOFPAPER_DEFAULT_THRESHOLD		200	// A/D threshold for out of paper (when disable gap check)
#define GAP_OUTOFPAPER_DEFAULT_THRESHOLD_ON_LABEL	255	// A/D threshold for out of paper (when disable gap check)
//#define MIN_GAP_SEN_RANGE				30					
#define MIN_GAP_SEN_RANGE				60					
#pragma pack(4)
typedef struct {
//not save for initial
//	INT8U GapBuf[GAP_SENSOR_BUF_SIZE];
	HUGEDATA INT8U *GapBuf;
	INT16U GapBufInPtr, GapBufOutPtr;
	INT8U GapVal[MAX_FILTER_NUM], PeelVal[MAX_FILTER_NUM], TempVal[MAX_FILTER_NUM], HeadUpVal[MAX_FILTER_NUM];
	INT8U SenFilCount;
					/* Temp = (3 * R) / (R + 27) 	*/
					/* TPHVol = (Real TPH voltage * 1.3 ) / 11.3   ex) 24:2.75	 */
	INT8U GapRealVal, PeelRealVal, TempRealVal, HeadUpRealVal;
	INT8U GapFilVal, PeelFilVal, TempFilVal, HeadUpFilVal;
	// sensor label test routine variable
	INT16U TestGapLength, TestLabelLength;
	char TestGapFlag, TestPreGapFlag;
	char TestGapStartFlag, TestLabelStartFlag;
	INT8U MaxGapValue, MinGapValue, MaxPeelValue, MinPeelValue;
	INT8U MaxGapLastValue, MinGapLastValue, MaxPeelLastValue, MinPeelLastValue;
	INT8U FilterCounter;
//save for initial
	short	GapBufInPtrTrimValue;
	INT8U 	GapThreshold, PeelThreshold;
//HYP 20040223
	INT8U 	GapOutOfPaperThreshold;
	INT8U 	GapOutOfPaperThresholdOnLabel;
} SENSOR_STRUCT;
#pragma pack()
extern HUGEDATA SENSOR_STRUCT SensorStruct;

extern void AdInt(void);
extern void SensorInit(void);
extern void GapBufInit(void);
extern void SensorGapTrim(short trim_value);

#endif /* _SENSOR_H */