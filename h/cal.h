/*****************************************************************************
|*			
|*  Copyright		:	(c) 2002 CAS
|*  Filename		:	cal.h
|*  Version			:	0.1
|*  Programmer(s)	:	Chung jong kwon (CJK)
|*  Created			:	2003/04/17
|*  Description		:	CLP BASIC LCD global variable define    
|*				
****************************************************************************/

#ifndef _CAL_H
#define _CAL_H

#define CAL_SPAN_NORMAL		0
#define CAL_SPAN_PERCENT	1


#define MAX_NUM_GLOBAL_SET	50
#pragma pack(2)
typedef struct {
	INT16U addr;
	INT8U size;
	INT8U type;
} GLOBAL_SET_STRUCT;
#pragma pack()
extern INT32U CalRun_PercentCalSet(INT32U ratio);
extern void CalRun_SpanCal(INT8U mode);
extern void CalRun_FineAdjust(void);
extern void CalRun_Linearize(void);
extern void CalRun_LinearityFineAdjust(void);
extern void CalRun_Hysteresis(void);
extern void CalRun_SetCreep(void);
extern void CalRun_AdFirmware(void);
extern void CalRun_Gravity(void);
extern void CalRun_CapaUnit(void);
extern void CalRun_TestAD(void);
extern void CalRun_TestAD_forUser(void);
//DEL CJK070314	extern void CalRun_AdFilter(void);
extern void CalRun_ZeroTare(void);
extern void CalRun_InitAdFilter(INT8U scale_type);
extern void CalRun_InitADM(INT8U debug_mode, INT8U backup);// Modified by CJK 20050816
//DEL CJK070314	extern void CalRun_SetHardware(void);
extern void CalRun_WeightCal(void);
extern void CalRun_KeyTest(void);
extern void CalRun_DisplayTest(void);
extern void CalRun_CashDrawerTest(void);
extern INT8U CalRun_ClearMemory(INT8U mode);
extern void CalRun_FirmwareVersion(void);
#ifdef USE_MORNITORING_CNT
extern void CalRun_Scale_Info(void);
#endif
extern void CalRun_MemoryInformation(void);
extern void CalRun_ScaleType(void);
extern void CalRun_InitPrinter(INT8U debug_mode);
extern void CalRun_ClearTable(void);
extern void CalRun_ComTest(void);
extern void CalRun_TableDefault(void);
extern INT8U GetBootVersion(INT8U *str);
//extern POINT disp_p;
//extern char disp_string_buf[50];

extern INT8U Cal_flagAdmBackup;
extern INT8U BootVersion[4];
extern INT8U BootVer;
#endif

