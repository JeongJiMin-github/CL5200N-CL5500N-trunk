/*****************************************************************************
|*			
|*  Copyright		:	(c) 2002 CAS
|*  Filename		:	main.h
|*  Version			:	0.1
|*  Programmer(s)	:	Chung jong kwon (CJK)
|*  Created			:	2003/03/20
|*  Description		:	CLP Printer RAM control routine header    
|*				
****************************************************************************/
#ifndef _MAIN_BUZZER
#define _MAIN_BUZZER

    #include "globals.h"
    #include "ethernet_user.h"
    #include "key.h"	
    #include "commbuf.h"

#ifndef _SOFT_RESET_
#define _SOFT_RESET_

#define _SOFTWARE_RESET  software_reset();

#endif

#pragma pack(4)
typedef struct {
	char Times;
	char Status;
} BUZZER_STR;
 
typedef struct
{
	INT32U Begin;
	INT32U Mid;
	INT32U End;
	INT32U Diff1;
	INT32U Diff2;
	INT8U Flag;
	INT8U Enable;
} DEBUG_TIME_STR; 
#pragma pack()
#endif

#ifndef _MAIN_H
#define _MAIN_H

#define SM_NORMAL_MODE		0
#define SM_MANAGER_MODE		1
#define SM_PREPACK_MODE		2
#define SM_TRAINING_MODE	3
#define SM_WHOLE_MODE		4

#define ASC_STX		0x02
#define ASC_ETX		0x03
#define ASC_CR		0x0d
#define ASC_LF		0x0a

extern char MsgBuf[];
extern DEBUG_TIME_STR  DebugTime;
#ifdef USE_MORNITORING_CNT
extern REMOTE_STRUCT OperTime;
#endif
extern HUGEDATA BUZZER_STR Buzzer;	
//extern HUGEDATA SCALE_CB *pscb;
extern HUGEDATA INT16U SysTimer100ms;
extern HUGEDATA INT16U SysTimer10ms;
extern HUGEDATA INT16U SysTimer1_25ms;
// extern HUGEDATA INT16U SysTimer100msForADM;
extern HUGEDATA INT16U SysTimerMin;
extern HUGEDATA INT32U SysTimerForDHCP;
//extern HUGEDATA INT8U  WeighingState;
//extern HUGEDATA char KeyStart;
#ifdef USE_MORNITORING_CNT
extern INT8U SysOperatingTime;
extern INT8U Enable_operate_time_cnt;
#endif
extern void MsgOut(HUGEDATA char *str);
extern void BuzOn(INT8U cnt);
extern void BuzOnAdd(INT8U cnt);
extern void PutPlu(void);
extern void Delay_100ms(INT16U cnt);
extern void Delay_10ms(INT16U cnt);
extern INT8U Delay_100ms_w_net(INT16U cnt, INT8U use_key, INT8U use_comm);
extern void CheckAdInitStatus(void);
// extern void PcInterface(void);
extern void ErrMode(void);
// extern HUGEDATA DISP_ON_UNION DspOn;

extern void software_reset(void);

#endif 
