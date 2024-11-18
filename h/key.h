/*****************************************************************************
|*
|*  Copyright		:	(c) 2002 CAS
|*  Filename		:	vfd.h
|*  Version			:	0.1
|*  Programmer(s)	:	Kim young il (YIK0101)
|*  Created			:	2003/04/17
|*  Description		:	CLP BASIC LCD global variable define    
|*
 ****************************************************************************/

#ifndef _KEY_H_1
#define _KEY_H_1

#include "globals.h"
#include "peripheral/int/plib_int.h"

#define KEY_DRV_MAX_KEY_NUM	        (8 * 14)
#define KEY_BUF_SIZE	8
#define KEY_DRV_IN_PIN_NUM      8
#define KEY_DRV_OUT_PIN_NUM     14
#define KEY_DRV_PROC_TIME_INTERVAL	1
#define KEY_DRV_VALID_TIME	        40
#define KEY_DRV_LONG_VALID_TIME	    1800//800//3000

#define KEY_IN_1            PLIB_PORTS_PinGet(PORTS_ID_0, PORT_CHANNEL_J, PORTS_BIT_POS_10)//PORTJbits.RJ10
#define KEY_IN_2            PLIB_PORTS_PinGet(PORTS_ID_0, PORT_CHANNEL_J, PORTS_BIT_POS_12)//PORTJbits.RJ12
#define KEY_IN_3            PLIB_PORTS_PinGet(PORTS_ID_0, PORT_CHANNEL_C, PORTS_BIT_POS_1)//PORTCbits.RC1
#define KEY_IN_4            PLIB_PORTS_PinGet(PORTS_ID_0, PORT_CHANNEL_E, PORTS_BIT_POS_7)//PORTEbits.RE7
#define KEY_IN_5            PLIB_PORTS_PinGet(PORTS_ID_0, PORT_CHANNEL_E, PORTS_BIT_POS_6)//PORTEbits.RE6
#define KEY_IN_6            PLIB_PORTS_PinGet(PORTS_ID_0, PORT_CHANNEL_E, PORTS_BIT_POS_5)//PORTEbits.RE5
#define KEY_IN_7            PLIB_PORTS_PinGet(PORTS_ID_0, PORT_CHANNEL_A, PORTS_BIT_POS_5)//PORTAbits.RA5
#define KEY_IN_8            PLIB_PORTS_PinGet(PORTS_ID_0, PORT_CHANNEL_G, PORTS_BIT_POS_15)//PORTGbits.RG15

#define KEY_OUT_1_H   PLIB_PORTS_PinSet( PORTS_ID_0, PORT_CHANNEL_E, PORTS_BIT_POS_4)//KEY_OUT_1, RE4
#define KEY_OUT_1_L   PLIB_PORTS_PinClear( PORTS_ID_0, PORT_CHANNEL_E, PORTS_BIT_POS_4)
#define KEY_OUT_2_H   PLIB_PORTS_PinSet( PORTS_ID_0, PORT_CHANNEL_E, PORTS_BIT_POS_3)//KEY_OUT_2, RE3
#define KEY_OUT_2_L   PLIB_PORTS_PinClear( PORTS_ID_0, PORT_CHANNEL_E, PORTS_BIT_POS_3)
#define KEY_OUT_3_H   PLIB_PORTS_PinSet( PORTS_ID_0, PORT_CHANNEL_E, PORTS_BIT_POS_2)//KEY_OUT_3, RE2
#define KEY_OUT_3_L   PLIB_PORTS_PinClear( PORTS_ID_0, PORT_CHANNEL_E, PORTS_BIT_POS_2)
#define KEY_OUT_4_H   PLIB_PORTS_PinSet( PORTS_ID_0, PORT_CHANNEL_E, PORTS_BIT_POS_1)//KEY_OUT_4, RE1
#define KEY_OUT_4_L   PLIB_PORTS_PinClear( PORTS_ID_0, PORT_CHANNEL_E, PORTS_BIT_POS_1)
#define KEY_OUT_5_H   PLIB_PORTS_PinSet( PORTS_ID_0, PORT_CHANNEL_E, PORTS_BIT_POS_0)//KEY_OUT_5, RE0
#define KEY_OUT_5_L   PLIB_PORTS_PinClear( PORTS_ID_0, PORT_CHANNEL_E, PORTS_BIT_POS_0)
#define KEY_OUT_6_H   PLIB_PORTS_PinSet( PORTS_ID_0, PORT_CHANNEL_J, PORTS_BIT_POS_7)//KEY_OUT_6, RJ7
#define KEY_OUT_6_L   PLIB_PORTS_PinClear( PORTS_ID_0, PORT_CHANNEL_J, PORTS_BIT_POS_7)
#define KEY_OUT_7_H   PLIB_PORTS_PinSet( PORTS_ID_0, PORT_CHANNEL_J, PORTS_BIT_POS_6)//KEY_OUT_7, RJ6
#define KEY_OUT_7_L   PLIB_PORTS_PinClear( PORTS_ID_0, PORT_CHANNEL_J, PORTS_BIT_POS_6)
#define KEY_OUT_8_H   PLIB_PORTS_PinSet( PORTS_ID_0, PORT_CHANNEL_J, PORTS_BIT_POS_5)//KEY_OUT_8, RJ5
#define KEY_OUT_8_L   PLIB_PORTS_PinClear( PORTS_ID_0, PORT_CHANNEL_J, PORTS_BIT_POS_5)
#define KEY_OUT_9_H   PLIB_PORTS_PinSet( PORTS_ID_0, PORT_CHANNEL_J, PORTS_BIT_POS_4)//KEY_OUT_9, RJ4
#define KEY_OUT_9_L   PLIB_PORTS_PinClear( PORTS_ID_0, PORT_CHANNEL_J, PORTS_BIT_POS_4)
#define KEY_OUT_10_H   PLIB_PORTS_PinSet( PORTS_ID_0, PORT_CHANNEL_G, PORTS_BIT_POS_0)//KEY_OUT_10, RG0
#define KEY_OUT_10_L   PLIB_PORTS_PinClear( PORTS_ID_0, PORT_CHANNEL_G, PORTS_BIT_POS_0)
#define KEY_OUT_11_H   PLIB_PORTS_PinSet( PORTS_ID_0, PORT_CHANNEL_G, PORTS_BIT_POS_1)//KEY_OUT_11, RG1
#define KEY_OUT_11_L   PLIB_PORTS_PinClear( PORTS_ID_0, PORT_CHANNEL_G, PORTS_BIT_POS_1)
#define KEY_OUT_12_H   PLIB_PORTS_PinSet( PORTS_ID_0, PORT_CHANNEL_K, PORTS_BIT_POS_7)//KEY_OUT_12, RK7
#define KEY_OUT_12_L   PLIB_PORTS_PinClear( PORTS_ID_0, PORT_CHANNEL_K, PORTS_BIT_POS_7)
#define KEY_OUT_13_H   PLIB_PORTS_PinSet( PORTS_ID_0, PORT_CHANNEL_J, PORTS_BIT_POS_3)//KEY_OUT_13, RJ3
#define KEY_OUT_13_L   PLIB_PORTS_PinClear( PORTS_ID_0, PORT_CHANNEL_J, PORTS_BIT_POS_3)
#define KEY_OUT_14_H   PLIB_PORTS_PinSet( PORTS_ID_0, PORT_CHANNEL_J, PORTS_BIT_POS_2)//KEY_OUT_14, RJ2
#define KEY_OUT_14_L   PLIB_PORTS_PinClear( PORTS_ID_0, PORT_CHANNEL_J, PORTS_BIT_POS_2)

#define KEY_CHECK_TIMES	5
#define KEY_DOUBLE_TIMES 10		// for Human
//#define KEY_DOUBLE_TIMES 50	// for Robot

#define	KEY_BUFFER_LEN	4
#define KEY_MAX_TABLE_SIZE		0x100

#define KEY_NORMAL_TABLE_ADDR			(DFLASH_BASE+FLASH_KEY_AREA)//0x3f0000
#define KEY_NORMAL_TABLE_SHIFT_ADDR		(KEY_NORMAL_TABLE_ADDR + KEY_MAX_TABLE_SIZE)
//#define KEY_PROG_TABLE_ADDR				(KEY_NORMAL_TABLE_SHIFT_ADDR + KEY_MAX_TABLE_SIZE)
//#define KEY_PROG_TABLE_SHIFT_ADDR		(KEY_PROG_TABLE_ADDR + KEY_MAX_TABLE_SIZE)
#endif

#ifndef _KEY_H_KEY_EXTERN
#define _KEY_H_KEY_EXTERN
#pragma pack(4)
typedef struct{
	INT8U  Buffer[KEY_BUFFER_LEN];
	INT8U  Buzzer[KEY_BUFFER_LEN];
	INT8U  Pop;
	INT8U  Push;
	INT8U  WriteFlag;
	INT8U  IsConvCodeFlag[KEY_BUFFER_LEN];
	INT16U ConvBuffer[KEY_BUFFER_LEN];
} KEY_BUF_STRUCT;

//yoo 091230
typedef struct {
	unsigned short ldad;
	unsigned short svad;
	unsigned char buf[32];
}KEY_BUF_TEMP_STRUCT;
#pragma pack()
enum KEY_MODE_TYPE {
	E_KEY_MODE_TYPE_SALE,
	E_KEY_MODE_TYPE_PROG,
};

#ifdef USE_FUNCTION_KEY_BLOCK
typedef struct
{
	INT8U	keycode;
	INT8U	flag;
} KEY_ONOFF_STRUCT;

extern KEY_ONOFF_STRUCT KeyOnoff[18];
#endif

extern KEY_BUF_TEMP_STRUCT KeyBufTemp;  //yoo 091230

extern void KeyBufInit(void);
extern void KeyBufPutChar(INT8U byte);
extern INT8U KeyBufGetChar(void);
extern INT8U KeyBufCheck(void);
extern void keyOutProc(INT16U H_L);

extern INT8U KEY_ErrorChk(long a);
extern void  KEY_Scan(void);
extern void  KEY_Write(INT16U rawcode,INT8U bz, INT8U isconvcodeflag);
extern void  KEY_InitKeyBuf(void);
extern void  KEY_Init(void);
extern void  KEY_InitTable(void);
#ifdef USE_FUNCTION_KEY_BLOCK
extern void  KEY_InitKeyOnoff(void);
#endif

extern INT8U KEY_InKey(void);
extern INT8U KEY_IsKey(void);
//extern INT8U KEY_Check(void);
extern INT8U KEY_Read(void);
extern void  KEY_SetKey(INT8U mode, INT8U position, INT16U key_id);
extern void  KEY_CopySelfKeyTable(void);  //SG060922
extern void  KEY_CopyKeyTable(INT16U country);
extern INT8U KEY_CnvTable(INT8U oldType, INT8U newType, INT8U tableSelect);
extern void  KEY_SetMode(INT8U mode);
extern INT8U KEY_GetMode(void);
extern INT16U KEY_getrawcode_from_plukey(INT16U key, INT8U *level);
extern void KEY_Set_Shift(INT8U onoff);
extern void KEY_Set_Caps(void);
extern INT16U KEY_getnormalkey(INT16U rawkey, INT8U level);
extern INT8U	UseScanner;
#ifdef USE_TRACE_STANDALONE
extern INT8U 	ModeIndiv;
extern INT16U MaxIndividualNo;
#endif
#ifdef USE_LONGPRESS_KEY_OVERRIDE
extern INT8U keyLongkeyIn;
#endif
#endif
