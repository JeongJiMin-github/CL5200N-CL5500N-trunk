/*****************************************************************************
|*			
|*  Copyright		:	(c) 2004 CAS
|*  Filename		:	ps2.h
|*  Version			:	0.1
|*  Programmer(s)	:	Hong Yi-Phyo (HYP)
|*  Created			:	2004/01/12
|*  Description		:	CLP ps2 routine header    
|*				
****************************************************************************/

#ifndef _PS2_H
#define _PS2_H

#define PSW_KEY_TABLE_MAX_NUM	0x90
#define PS2_KEY_MAX_NUM		140

#define PS2_BUFFER_LEN	100
#define PS2_RAW_BUFFER_LEN	500
#pragma pack(4)
typedef struct{
	INT16U Buffer[PS2_BUFFER_LEN];
	INT8U  RawBuffer[PS2_RAW_BUFFER_LEN];
	INT8U  Pop;
	INT8U  Push;
	INT8U  RawPop;
	INT8U  RawPush;
} PS2_BUF_STRUCT;
#pragma pack()
extern ROMDATA INT16U prog_ps2_code_setS_kor[PS2_KEY_MAX_NUM];//HYP 20060427
extern ROMDATA INT16U prog_ps2_code_setS_kor_shifted[PS2_KEY_MAX_NUM];//HYP 20060427
extern ROMDATA INT16U prog_ps2_code_setA_kor[PS2_KEY_MAX_NUM];
extern ROMDATA INT16U prog_ps2_code_setA_kor_shifted[PS2_KEY_MAX_NUM];
extern ROMDATA INT16U prog_ps2_code_setB_kor[PS2_KEY_MAX_NUM];
extern ROMDATA INT16U prog_ps2_code_setB_kor_shifted[PS2_KEY_MAX_NUM];

extern ROMDATA INT16U prog_ps2_code_setS_self[PS2_KEY_MAX_NUM]; //SG071121


extern PS2_BUF_STRUCT PS2Buf;

extern void PS2_RawWrite(INT8U code);
//extern void PS2_Write(INT16U code);
extern INT8U PS2_IsRawKey(void);
extern void PS2_InitKeyBuf(void);
extern INT16U PS2_InKey(void);
extern INT8U PS2_IsKey(void);
extern void PS2decode(INT8U sc); 
extern INT16U PS2_InRawKey(void);

#endif /* _PS2_H */
