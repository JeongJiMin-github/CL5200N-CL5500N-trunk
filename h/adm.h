/*****************************************************************************
|*			
|*  Copyright		:	(c) 2002 CAS
|*  Filename		:	adm.h
|*  Version			:	0.1
|*  Programmer(s)	:	Chung jong kwon (CJK)
|*  Created			:	2003/04/17
|*  Description		:	CLP BASIC LCD global variable define    
|*				
****************************************************************************/

//test svn

#ifndef _ADM_H
#define _ADM_H

// Define Command

#define	ADM_CMD_GET_CAL_ZERO	0x00
#define	ADM_CMD_GET_CAL_SPAN	0x01
#define	ADM_CMD_GET_PARAMETER	0x02
#define	ADM_CMD_GET_CAL_RATIO	0x03
#define	ADM_CMD_GET_CURVE_ZERO	0x04
#define	ADM_CMD_GET_CURVE_MID	0x05
#define	ADM_CMD_GET_CURVE_SPAN	0x06
#define	ADM_CMD_GET_CURVE_RATIO	0x07
#define	ADM_CMD_GET_CURVE_CALC	0x08
#define	ADM_CMD_GET_DUAL	0x09
#define	ADM_CMD_GET_CAPACITY	0x0A
#define	ADM_CMD_GET_INT_RESOL	0x0B
#define	ADM_CMD_GET_OVERLD_R	0x0C
#define	ADM_CMD_GET_INIT_ZR_R	0x0D
#define	ADM_CMD_GET_REZERO_R	0x0E
#define	ADM_CMD_GET_TARESET_R	0x0F
#define	ADM_CMD_GET_NULL_OUT_WT	0x10
#define	ADM_CMD_GET_NULL_OUT_AD1	0x11
#define	ADM_CMD_GET_NULL_OUT_AD2	0x12
#define	ADM_CMD_GET_NULL_OUT_AD3	0x13
#define	ADM_CMD_GET_NULL_OUT_AD4	0x14
#define	ADM_CMD_GET_NULL_OUT_AD5	0x15
#define	ADM_CMD_GET_INI_ZR_FLAG	0x16
#define	ADM_CMD_GET_GRVTY_CAL	0x17
#define	ADM_CMD_GET_GRVTY_USE	0x18
#define	ADM_CMD_GET_W_UNIT_CAL	0x19
#define	ADM_CMD_GET_W_UNIT_USE	0x1A
#define	ADM_CMD_GET_W_UNIT_CUR	0x1B
#define	ADM_CMD_GET_MOTION_BAND	0x1C
#define	ADM_CMD_GET_FILTON_BAND	0x1D
#define	ADM_CMD_GET_FILTOFF_BAND	0x1E
#define	ADM_CMD_GET_FILTER_SIZE	0x1F
#define	ADM_CMD_GET_NULL_START_WT	0x20
#define	ADM_CMD_GET_NULL_RESTART	0x21
#define	ADM_CMD_GET_UPDATETIME1	0x22
#define	ADM_CMD_GET_UPDATETIME2	0x23
#define	ADM_CMD_GET_UPDATETIME3	0x24
#define	ADM_CMD_GET_ZERO	0x25
#define	ADM_CMD_GET_TARE_INT	0x26
#define	ADM_CMD_GET_TARE_EXT	0x27
#define	ADM_CMD_GET_NULL_STOP_WT	0x28
#define	ADM_CMD_GET_TARESET_LB_R	0x29
#define	ADM_CMD_GET_HYST_MID_UP	0x2A
#define	ADM_CMD_GET_HYST_MID_DN	0x2B
#define	ADM_CMD_GET_CALZEROMIN	0x2C
#define	ADM_CMD_GET_CALSPANMIN	0x2D
#define	ADM_CMD_GET_CALZEROMAX	0x2E
#define	ADM_CMD_GET_CALSPANMAX	0x2F
#define	ADM_CMD_GET_ETC	0x30
#define	ADM_CMD_GET_HYST_ZERO	0x31
#define	ADM_CMD_GET_TIME_PERIOD	0x32
//#define	ADM_CMD_GET_CHARGE	0x33
//#define	ADM_CMD_GET_RECHARGE 	0x34
#define	ADM_CMD_GET_MAX_4_CUST	0x33 //Max for Custom Capa.
#define	ADM_CMD_GET_D1_4_CUST 	0x34 //d1 for Custom Capa.
#define	ADM_CMD_GET_BAUDRATE	0x35
#define	ADM_CMD_GET_VERSION	0x36
#define	ADM_CMD_GET_HYST_SPAN	0x37
#define	ADM_CMD_GET_EEP_DATA	0x38
#define	ADM_CMD_GET_NULL_DBG_POINT	0x39
#define	ADM_CMD_GET_NULL_EEP_ERASE	0x3A
#define	ADM_CMD_GET_HYST_ENABLE	0x3B
#define	ADM_CMD_GET_EEP_ADDR	0x3C
#define	ADM_CMD_GET_CHECK_EEP	0x3D
#define	ADM_CMD_GET_INIT_STAT	0x3E
#define	ADM_CMD_GET_NULL_SAVE_SETS	0x3F





#define	ADM_CMD_SET_CAL_ZERO	0x40
#define	ADM_CMD_SET_CAL_SPAN	0x41
#define	ADM_CMD_SET_PARAMETER	0x42
#define	ADM_CMD_SET_CAL_RATIO	0x43
#define	ADM_CMD_SET_CURVE_ZERO	0x44
#define	ADM_CMD_SET_CURVE_MID	0x45
#define	ADM_CMD_SET_CURVE_SPAN	0x46
#define	ADM_CMD_SET_CURVE_RATIO	0x47
#define	ADM_CMD_SET_CURVE_CALC	0x48
#define	ADM_CMD_SET_DUAL	0x49
#define	ADM_CMD_SET_CAPACITY	0x4A
#define	ADM_CMD_SET_INT_RESOL	0x4B
#define	ADM_CMD_SET_OVERLD_R	0x4C
#define	ADM_CMD_SET_INIT_ZR_R	0x4D
#define	ADM_CMD_SET_REZERO_R	0x4E
#define	ADM_CMD_SET_TARESET_R	0x4F
#define	ADM_CMD_SET_OUT_WT	0x50
#define	ADM_CMD_SET_OUT_AD1	0x51
#define	ADM_CMD_SET_OUT_AD2	0x52
#define	ADM_CMD_SET_OUT_AD3	0x53
#define	ADM_CMD_SET_OUT_AD4	0x54
#define	ADM_CMD_SET_OUT_AD5	0x55
#define	ADM_CMD_SET_INI_ZR_FLAG	0x56
#define	ADM_CMD_SET_GRVTY_CAL	0x57
#define	ADM_CMD_SET_GRVTY_USE	0x58
#define	ADM_CMD_SET_W_UNIT_CAL	0x59
#define	ADM_CMD_SET_W_UNIT_USE	0x5A
#define	ADM_CMD_SET_W_UNIT_CUR	0x5B
#define	ADM_CMD_SET_MOTION_BAND	0x5C
#define	ADM_CMD_SET_FILTON_BAND	0x5D
#define	ADM_CMD_SET_FILTOFF_BAND	0x5E
#define	ADM_CMD_SET_FILTER_SIZE	0x5F
#define	ADM_CMD_START_WT	0x60
#define	ADM_CMD_RESTART	0x61
#define	ADM_CMD_SET_UPDATETIME1	0x62
#define	ADM_CMD_SET_UPDATETIME2	0x63
#define	ADM_CMD_SET_UPDATETIME3	0x64
#define	ADM_CMD_SET_ZERO	0x65
#define	ADM_CMD_SET_TARE	0x66
#define	ADM_CMD_SET_PRESET_TARE	0x67
#define	ADM_CMD_STOP_WT	0x68 
#define	ADM_CMD_SET_TARESET_LB_R	0x69
#define	ADM_CMD_SET_HYST_MID_UP	0x6A
#define	ADM_CMD_SET_HYST_MID_DN	0x6B
#define	ADM_CMD_SET_CALZEROMIN	0x6C
#define	ADM_CMD_SET_CALSPANMIN	0x6D
#define	ADM_CMD_SET_CALZEROMAX	0x6E
#define	ADM_CMD_SET_CALSPANMAX	0x6F
#define	ADM_CMD_SET_ETC	0x70
#define	ADM_CMD_SET_HYST_ZERO	0x71
#define	ADM_CMD_SET_TIME_PERIOD	0x72
//#define	ADM_CMD_SET_CHARGE	0x73
//#define	ADM_CMD_SET_RECHARGE 	0x74
#define	ADM_CMD_SET_MAX_4_CUST	0x73 //Max for Custom Capa.
#define	ADM_CMD_SET_D1_4_CUST 	0x74 //d1 for Custom Capa.
#define	ADM_CMD_SET_BAUDRATE	0x75
#define	ADM_CMD_SET_NULL_VERSION	0x76
#define	ADM_CMD_SET_HYST_SPAN	0x77
#define	ADM_CMD_SET_EEP_DATA	0x78
#define	ADM_CMD_SET_NULL_DBG_POINT	0x79
#define	ADM_CMD_SET_EEP_ERASE	0x7A
#define	ADM_CMD_SET_HYST_ENABLE	0x7B
#define	ADM_CMD_SET_EEP_ADDR	0x7C
#define	ADM_CMD_SET_NULL_CHECK_EEP	0x7D
#define	ADM_CMD_INIT	0x7E
#define	ADM_CMD_SAVE_SETS	0x7F


// SUB Command(raw)
#define ADM_SCMD_GET_PAR_CREEP_NUM  1
#define ADM_SCMD_GET_PAR_CREEP_ON   2
#define ADM_SCMD_GET_PAR_HANG_TYPE  4
#define ADM_SCMD_GET_PAR_MAX1_KG    20
#define ADM_SCMD_GET_PAR_MAX2_KG    21
#define ADM_SCMD_GET_PAR_D1_KG      22
#define ADM_SCMD_GET_PAR_D2_KG      23
#define ADM_SCMD_GET_PAR_MAX1_LB    24
#define ADM_SCMD_GET_PAR_MAX2_LB    25
#define ADM_SCMD_GET_PAR_D1_LB      26
#define ADM_SCMD_GET_PAR_D2_LB      27
#define ADM_SCMD_GET_CAL_DATE			30
#define ADM_SCMD_GET_OVERLOAD_CNT		31
#define ADM_SCMD_GET_OVERLOAD_SUM_CNT	32
#define ADM_SCMD_GET_WEIGHING_CNT		33
#define ADM_SCMD_GET_WEIGHING_SUM_CNT	34
#define ADM_SCMD_GET_SHOCK_CNT			35
#define ADM_SCMD_GET_SHOCK_SUM_CNT		36


// SUB Command
#define ADM_SUB_CREEP_NUM (ADM_SCMD_GET_PAR_CREEP_NUM * 1000000L)
#define ADM_SUB_CREEP_ON  (ADM_SCMD_GET_PAR_CREEP_ON * 1000000L)
#define ADM_SUB_HANG_TYPE (ADM_SCMD_GET_PAR_HANG_TYPE * 1000000L)
#define ADM_SUB_MAX1_KG (ADM_SCMD_GET_PAR_MAX1_KG * 1000000L)
#define ADM_SUB_MAX2_KG (ADM_SCMD_GET_PAR_MAX2_KG * 1000000L)
#define ADM_SUB_D1_KG   (ADM_SCMD_GET_PAR_D1_KG * 1000000L)
#define ADM_SUB_D2_KG   (ADM_SCMD_GET_PAR_D2_KG * 1000000L)
#define ADM_SUB_MAX1_LB (ADM_SCMD_GET_PAR_MAX1_LB * 1000000L)
#define ADM_SUB_MAX2_LB (ADM_SCMD_GET_PAR_MAX2_LB * 1000000L)
#define ADM_SUB_D1_LB   (ADM_SCMD_GET_PAR_D1_LB * 1000000L)
#define ADM_SUB_D2_LB   (ADM_SCMD_GET_PAR_D2_LB * 1000000L)
#define ADM_SUB_CAL_DATE  (ADM_SCMD_GET_CAL_DATE * 1000000L)
#define ADM_SUB_OVERLOAD_CNT  (ADM_SCMD_GET_OVERLOAD_CNT * 1000000L)
#define ADM_SUB_OVERLOAD_SUM_CNT   (ADM_SCMD_GET_OVERLOAD_SUM_CNT * 1000000L)
#define ADM_SUB_WEIGHING_CNT   (ADM_SCMD_GET_WEIGHING_CNT * 1000000L)
#define ADM_SUB_WEIGHING_SUM_CNT   (ADM_SCMD_GET_WEIGHING_SUM_CNT * 1000000L)
#define ADM_SUB_SHOCK_CNT (ADM_SCMD_GET_SHOCK_CNT * 1000000L)
#define ADM_SUB_SHOCK_SUM_CNT (ADM_SCMD_GET_SHOCK_SUM_CNT * 1000000L)

#ifdef USE_SRILANKA_ZERO
#define INIT_ZERO_VALUE 3
#elif defined (USE_CHN_CART_SCALE)
#define INIT_ZERO_VALUE 20
#else
#define INIT_ZERO_VALUE 10
#endif

#ifdef USE_SRILANKA_ZERO
#define REZERO_VALUE 3
#elif defined (USE_CHN_CART_SCALE)
#define REZERO_VALUE 10
#else
#define REZERO_VALUE 2
#endif

							  
#define		ADM_PARAM_OFF	0
#define		ADM_PARAM_ON	1

#define ADM_M_GRV_AT_CAL	0
#define ADM_M_GRV_AT_USE	1

#define ADM_M_UNIT_AT_CAL	0
#define ADM_M_UNIT_AT_USE	1

#define ADM_CHK_SUCCESS			0
#define ADM_CHK_ERR_COM			1
#define ADM_CHK_ERR_MODULE		2
#define ADM_CHK_ERR_INIT		3	
#define ADM_CHK_ERR_EEP			4

#define WEIGHT_UNIT_KG	0
#define WEIGHT_UNIT_LB	1
#define WEIGHT_UNIT_G	2

#define ADM_INT_RESOLUTION	60000

//#define ADM_MAX_CAPA_ID 5
#define ADM_MAX_CAPA_ID 10	// 0~9: fixed capa, 10:custom capa
#define ADM_CUSTOM_CAPA_ID 10

#define	ADM_CUSTOM_CAPA_VER	204	//CL5200/CL5500 기준 (CL5200N/CL5500N은 V3.06 이하 버전의 TPND로 생산한 적 없음)


//##typedef enum
//##{
//##	ADM_DATAMODE_WEIGHT_KG,
//##	ADM_DATAMODE_C1,
//##	ADM_DATAMODE_D2,
//##	ADM_DATAMODE_D3,
//##	ADM_DATAMODE_D4,
//##	ADM_DATAMODE_C5,
//##	ADM_DATAMODE_EMPTY,
//##	ADM_DATAMODE_WEIGHT_LB,	
//##	ADM_DATAMODE_OK = 0x10,
//##	ADM_DATAMODE_ERROR,	   
//##	ADM_DATAMODE_INIT_ERR,
//##	ADM_DATAMODE_EEP_ERR,
//##	ADM_DATAMODE_NONE = 0xFF
//##} ADM_GETDATAMODE;

typedef enum
{
	ADM_DATAMODE_WEIGHT_KG,
	ADM_DATAMODE_AD1,
	ADM_DATAMODE_AD2,
	ADM_DATAMODE_AD3,
	ADM_DATAMODE_AD4,
	ADM_DATAMODE_AD5,
	ADM_DATAMODE_EMPTY,
	ADM_DATAMODE_WEIGHT_LB,	
	ADM_DATAMODE_OK = 0x10,
	ADM_DATAMODE_ERROR,	   
	ADM_DATAMODE_INIT_ERR,
	ADM_DATAMODE_EEP_ERR,
	ADM_DATAMODE_NONE = 0xFF
} ADM_GETDATAMODE;

typedef enum
{
	ADM_CHK_INIT_OK,
	ADM_CHK_INIT_ERR_STABLE,
	ADM_CHK_INIT_ERR_LOAD,
	ADM_CHK_INIT_EEP_ERR,
	ADM_CHK_INIT_FAIL
} ADM_CHECKINIT;

#pragma pack(4)
typedef struct
{
//	INT16U Version;
	INT32S RecvData;
	INT32S PrevRecvData;
	INT8U RecvCmd;
	INT8U WeightMode;
	INT8U DataMode;
	INT8U Stable;
	INT8U PrevStable;
	INT8U Zero;
	INT8U Tare;
	INT8U Overload;
	INT8U Status;
	INT8U PrevStatus;
	INT8U ChangeData;
	INT8U CurUnit;	// 0:kg, 1:lb, 2:g
} ADM_GETDATA;


typedef struct
{
	INT16U Version;
	INT8U Capa;
	INT8U Dual;
	INT8U CalUnit;	// 0:kg, 1:lb
	INT8U UseUnit;	// 0:kg, 1:lb
//	INT8U InhibitDigit;
	INT8U EepCsLocation;
	INT8U WaitingTimes;
	INT32S Max1[2]; // Dual일 때 아랫구간 Max. Single일 때 Max2와 동일
	INT32S Max2[2]; // Dual일 때 윗구간 Max
	INT32S d1[2];   // Dual일 때 아랫구간 한눈의 값. Single일 때 d2와 동일
	INT32S d2[2];   // Dual일 때 윗구간 한눈의 값
	INT8U WtDec;
} ADM_STATUS_STRUCT;

typedef struct
{
	INT8U Cmd;
	INT8U ParamMode;
	INT32S Param;
} ADM_SENDDATA;
#pragma pack()
//extern ADM_GETDATA ADM_GetData;
//extern ADM_STATUS_STRUCT ADM_Status;
//HYP 20060412
extern NEARDATA ADM_GETDATA ADM_GetData;
extern NEARDATA ADM_STATUS_STRUCT ADM_Status;

extern INT8U ADM_flagDebug;
extern INT8U 	CurPluWeightUnit;	// Added by CJK 20051025

extern void ADM_Process(void);
extern INT8U ADM_Init(void);
extern void ADM_SendCommand(INT8U cmd, INT32S param, INT8U param_mode);
extern void ADM_ParseData(void);
extern INT8U ADM_RecvProc(void);
extern INT32S ADM_WaitData(INT8U cmd, INT32S param, INT8U param_mode);
extern INT32S ADM_WaitDataCnt(INT8U cmd, INT32S param, INT8U param_mode, INT8U retry);
extern INT8U ADM_WaitResult(INT8U cmd, INT32S param, INT8U param_mode);
extern INT8U ADM_CheckInitWeighing(void);
extern void ADM_Restart(void);
extern INT8U ADM_ChangeBaudrate(INT8U target_br);
extern INT8U ADM_BackupEeprom(void);
extern INT8U ADM_RestoreEeprom(void);
extern INT8U ADM_ScanBaudrate(INT8U prev_br, INT8U set_br);
extern void ADM_ChangeByteFlash(INT32U ad_addr, INT8U data);
extern void ADM_ChangeWordFlash(INT32U ad_addr, INT16U data);
extern void ADM_ChangeLongFlash(INT32U ad_addr, INT32U data);
extern INT8U ADM_CheckModuleAndGetVersion(void);
extern void ADM_CheckCsLocation(void);



#endif








