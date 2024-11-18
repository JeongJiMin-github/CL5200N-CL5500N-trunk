/*****************************************************************************
|*			
|*  Copyright		:	(c) 2003 CAS
|*  Filename		:	adm.c
|*  Version			:	0.1
|*  Programmer(s)	:	Chung jong kwon (CJK)
|*  Created			:	2003/03/20
|*  Description		:	CLP ad module routine    
|*				
****************************************************************************/

#include <stdio.h>
#include "globals.h"

#include "commbuf.h"
#include "prtdrv.h"
#include "timer.h"
#include "serial.h"
#include "flash.h"
#include "sensor.h"
#include "ram.h"
#include "prt_render.h"
#include "bar_common.h"
#include "prt_font.h"
#include "dsp_common.h"
#include "prt_interpreter.h"

#include "adm.h"
#include "adm_app.h"
#include "main.h"
#include "initial.h"
#include "flash_app.h"
#include "common.h"

#define ADM_EEP_BACKUP_SIZE	0x80//(GLOBAL_AD_CS+1)
#define ADM_BACKUP_ADDR		DFLASH_BASE + FLASH_ADC_AREA
#define ADM_LOCATION_CS		0xfc
#define ADM_LOCATION_INIT_EEP	0xfe //ADDR_INIT_EEP
#define INIT_EEP_COMPLETE		0x49

NEARDATA ADM_GETDATA		ADM_GetData;
NEARDATA ADM_STATUS_STRUCT	ADM_Status;

INT8U ADM_flagDebug = 0;
INT8U 	CurPluWeightUnit;	// Added by CJK 20051025

INT8U ADM_CheckFlashBackup(void);
INT8U ADM_CompareCheckSum(void);
#ifdef USE_ADM_AUTO_RESTORE
void ADM_Init_error_count(void);
void ADM_Eep_error_count(void);
#endif
////////////////////////////////////////////////////////////////////

void ADM_Write(void)
{
	if (CommBufAD.Txing != 1) {
		if(CheckTxBuf(&CommBufAD)) {
			Uart1SendData();
		}
	}
}

void ADM_SendCommand(INT8U cmd, INT32S param, INT8U param_mode)
{
	char Trans_buffer[20];
	
	if(param_mode == 0)
	{
		sprintf(&Trans_buffer[0], "%c%02X%c", 0x01, cmd, 0x04);  
	}
	else
	{
		param %= 100000000L;
		sprintf(&Trans_buffer[0], "%c%02X%08ld%c", 0x01, cmd, param, 0x04);  
	}
	PutStrTxBuf(&CommBufAD, &Trans_buffer[0]);  
	ADM_Write();  
}

INT32S ADM_WaitData(INT8U cmd, INT32S param, INT8U param_mode)		// command를 보내고, command의 처리결과가
{																	// OK가 될때까지 3번 retry
	INT8U i;
	INT16U cur_time;
	
	for(i = 0; i < 3; i++)
	{
		ADM_SendCommand(cmd, param, param_mode);
		cur_time = SysTimer100ms;
		while(1)
		{
			if(ADM_RecvProc())
			{
				if(ADM_GetData.DataMode == ADM_DATAMODE_OK && ADM_GetData.RecvCmd == cmd) 
				{
					return ADM_GetData.RecvData;
				}
			}
			else if((INT16U)(SysTimer100ms - cur_time) > 10)
			{
				break;
			}
		}
	}
	return -1;
}

INT32S ADM_WaitDataCnt(INT8U cmd, INT32S param, INT8U param_mode, INT8U retry)	   // command를 보내고, command의 처리결과가
{																				   // OK가 될때까지 n번 retry
	INT8U i;
	INT16U cur_time;
	
	for(i = 0; i < retry; i++)
	{
		ADM_SendCommand(cmd, param, param_mode);
		cur_time = SysTimer100ms;
		while(1)
		{
			if(ADM_RecvProc())
			{
				if(ADM_GetData.DataMode == ADM_DATAMODE_OK && ADM_GetData.RecvCmd == cmd) 
				{
					return ADM_GetData.RecvData;
				}
			}
			else if((INT16U)(SysTimer100ms - cur_time) > 5)
			{
				break;
			}
		}
	}
	return -1;
}

///////////////////////
// Function Name : ADM_WaitResult()
// return value
//	0x00	: No Error(OK)
//	1~0xfe 	: Error Code
//  0xff 	: Communcation Error
INT8U ADM_WaitResult(INT8U cmd, INT32S param, INT8U param_mode)		// command를 보내고 command에 대한 처리결과를 기다림
{
//	INT8U i;
	INT16U cur_time;

	ADM_SendCommand(cmd, param, param_mode);
	cur_time = SysTimer100ms;
	while(1)
	{
		if(ADM_RecvProc())
		{
			if(ADM_GetData.RecvCmd == cmd) 
			{
				if(ADM_GetData.DataMode == ADM_DATAMODE_OK) return 0;
				else if(ADM_GetData.DataMode == ADM_DATAMODE_ERROR) return (INT8U)ADM_GetData.RecvData;
			}
		}
		else if((INT16U)(SysTimer100ms - cur_time) > 10)
		{
			break;
		}
	}
	return 0xff;
}

							
void ADM_Restart(void)
{
	INT32U flag;

	ADM_WaitData(ADM_CMD_RESTART, 0, ADM_PARAM_OFF);					// Restart Adm
	ADM_WaitData(ADM_CMD_START_WT, 0, ADM_PARAM_OFF);					// Start Weight 

	// CJK080324 : ad module은 항상 +tare,-tare를 허용하도록 수정(향후 불필요) 
	flag = ADM_WaitData(ADM_CMD_GET_ETC, 0, ADM_PARAM_OFF);
	flag |= 0xc0;
	ADM_WaitData(ADM_CMD_SET_ETC, (INT32S)(flag&0xff), ADM_PARAM_ON);
}

INT8U ADM_ScanBaudrate(INT8U prev_br, INT8U set_br)
{
	INT8S i;
	INT8U cur_br;
	INT32S status;

	cur_br = 0xff;
	for (i = BAUD_RATE_115200; i >= BAUD_RATE_9600; i--) {
		Delay_100ms(1);
		Uart1Init(i);
		status = ADM_WaitDataCnt(ADM_CMD_GET_VERSION, 0, ADM_PARAM_OFF, 3);	// Version = "1023" + VER[3]
		if (status/1000 == 1023) {
			cur_br = i;
			break;
		}
	}
	// current scale baudrate : cur_br, 
	// current ADM baudrate : cur_br, 
	// orginal scale baudrate : prev_cur

	if (cur_br == 0xff) {			// restore baudrate to prev_br
		Uart1Init(prev_br);
		return FALSE;
	} else {						// set baudrate to set_br
		if(cur_br != set_br) {
			ADM_ChangeBaudrate(set_br);	// set baudrate
			Uart1Init(set_br);
		}
		return TRUE;
	}
}

INT8U ADM_CheckModuleAndGetVersion(void)
{
	INT32U ltemp;

	ltemp = ADM_WaitData(ADM_CMD_GET_VERSION, 0, ADM_PARAM_OFF);	// Version = "1023" + VER[3]
	if (ltemp/1000 != 1023) {
		ltemp = ADM_ScanBaudrate(BAUD_RATE_57600, BAUD_RATE_57600);
		if(!ltemp) {
			return ADM_CHK_ERR_COM;		// 통신에러
		}
		ltemp = ADM_WaitData(ADM_CMD_GET_VERSION, 0, ADM_PARAM_OFF);
		if (ltemp/1000 != 1023) {
			return ADM_CHK_ERR_MODULE;
		}
	}
	// Version Check
	ADM_Status.Version = ltemp%1000;

	ltemp = ADM_WaitData(ADM_CMD_GET_INIT_STAT, 0, ADM_PARAM_OFF);	// 0x59이면 Init 완료
	if (ltemp != 0x49) {
#ifdef USE_ADM_AUTO_RESTORE
		ADM_Init_error_count();
#endif
		return ADM_CHK_ERR_INIT;
	}

	ltemp = ADM_WaitData(ADM_CMD_GET_CHECK_EEP, 0, ADM_PARAM_OFF);	// 0:Err,  1,2:Correct,  3:No Err
	if (ltemp == 0) {	
#ifdef USE_ADM_AUTO_RESTORE
		ADM_Eep_error_count();
#endif
		return ADM_CHK_ERR_EEP;
	}

	return ADM_CHK_SUCCESS;
}


INT8U ADM_ChangeBaudrate(INT8U target_br)
{
	ADM_WaitData(ADM_CMD_STOP_WT, 0, ADM_PARAM_OFF);
	ADM_WaitData(ADM_CMD_SET_BAUDRATE, target_br, ADM_PARAM_ON);
	ADM_WaitData(ADM_CMD_SAVE_SETS, 83419, ADM_PARAM_ON);
	ADM_WaitData(ADM_CMD_RESTART, 0, ADM_PARAM_OFF);
	return 0;
}

void ADM_CheckCsLocation(void)	// Added by CJK 20050808
{
	if(ADM_Status.Version >= 99) {
		ADM_WaitData(ADM_CMD_SET_EEP_ADDR, ADM_LOCATION_CS, ADM_PARAM_ON);
		ADM_Status.EepCsLocation = (INT8U)ADM_WaitData(ADM_CMD_GET_EEP_DATA, 0, ADM_PARAM_OFF);
		//ADM_Status.EepCsLocation = 79;
		if(ADM_Status.EepCsLocation >= 0x80 || ADM_Status.EepCsLocation <= 0x30) ADM_Status.EepCsLocation = 93; // ADM 3.06~ 93 이어야 함. //ADM V1.11 부터 88
	} else if (ADM_Status.Version == 0) {
		ADM_Status.EepCsLocation = 79;
	} else {
		ADM_Status.EepCsLocation = 63;		// Ver0.98이하 : 63, Ver0.99이상:79
	}
}

INT8U ADM_Init(void)
{
	INT8U err;
#ifdef USE_CANADA_ZERO_MARK_SETTING
	INT32U ltemp;
#endif

	// Init Variable
	ADM_GetData.ChangeData = 1;
	ADM_GetData.Zero = 0;
	ADM_GetData.Stable = 0;
	ADM_GetData.Tare = 0;
	ADM_GetData.Overload = 0;
	ADM_GetData.RecvData = 0;
	ADM_GetData.Status = 0;

	ADM_Status.Version = 0;
	ADM_Status.Capa = 0;
	ADM_Status.Dual = 0;
	ADM_Status.UseUnit = 0;
	ADM_Status.CalUnit = 0;	
	ADM_Status.EepCsLocation = 63;		// Ver0.98이하 : 63, Ver0.99이상:79
	ADM_flagDebug = 0;						// 0:Enable Debug, 1:Disable Debug

	ADM_WaitData(ADM_CMD_STOP_WT, 0, ADM_PARAM_OFF);
	err = ADM_CheckModuleAndGetVersion();
	ADM_CheckCsLocation();
	switch(err) {
		case ADM_CHK_ERR_COM :
			return err;		// 통신에러
		case ADM_CHK_ERR_MODULE :
			return err;
		case ADM_CHK_ERR_INIT :
#ifdef USE_ADM_AUTO_RESTORE
			BuzOn(3); // buz on for error 03
			if(ADM_CheckFlashBackup()) { // checksum check
				ADM_RestoreEeprom();	// write FLASH -> ADM_EEP
				BuzOn(1); // buz on for restore eeprom complete
				ADM_WaitData(ADM_CMD_SET_EEP_ADDR, ADM_LOCATION_INIT_EEP, ADM_PARAM_ON); // access init eep addr (ADDR_INIT_EEP)
				ADM_WaitData(ADM_CMD_SET_EEP_DATA, INIT_EEP_COMPLETE, ADM_PARAM_ON); // set INIT_EEP_COMPLETE on init eep addr (ADDR_INIT_EEP)
				ADM_SendCommand(ADM_CMD_RESTART, 0, ADM_PARAM_OFF);	// Restart adm
				wait_sleep(0x4ffff);// wait adm restart
				err=0;
			} else {
				BuzOn(7);
				return err;	// 복구 불능
			}
			break;
#else
			return err;
#endif
		case ADM_CHK_ERR_EEP :
			BuzOn(4); // buz on for error 03
			if(ADM_CheckFlashBackup()) { // checksum check
				ADM_RestoreEeprom();	// write FLASH -> ADM_EEP
				ADM_WaitData(ADM_CMD_SET_EEP_ADDR, ADM_LOCATION_INIT_EEP, ADM_PARAM_ON); // access init eep addr (ADDR_INIT_EEP)
				ADM_WaitData(ADM_CMD_SET_EEP_DATA, INIT_EEP_COMPLETE, ADM_PARAM_ON); // set INIT_EEP_COMPLETE on init eep addr (ADDR_INIT_EEP)
				ADM_SendCommand(ADM_CMD_RESTART, 0, ADM_PARAM_OFF);	// Restart adm
				wait_sleep(0x4ffff);// wait adm restart
				err=0;
			} else {
				BuzOn(7);
				return err;	// 복구 불능
			}
			break;
	}

	if(!ADM_CompareCheckSum()) {	// 이 지점에서는 ADM 데이터의 오류가 없다고 가정
		ADM_BackupEeprom();	// ADM_EEP -> FLASH	
	}

	ADM_Status.Version = ADM_WaitData(ADM_CMD_GET_VERSION, 0, ADM_PARAM_OFF) % 1000L;
	ADM_GetData.CurUnit = adm_get_unit_cur();
	ADM_Status.CalUnit = ad_get_unit_cal();
	ADM_Status.UseUnit = ad_get_unit_weigh();//WEIGHT_UNIT_KG;
	ADM_Status.Capa    = ad_get_capa_idx();
	ADM_Status.Dual    = ad_get_multi_interval();
	ADM_Status.WaitingTimes = 0;

	if(ADM_Status.Version >= ADM_CUSTOM_CAPA_VER) {
		ADM_Status.Max1[0] = ADM_WaitData(ADM_CMD_GET_PARAMETER, ADM_SUB_MAX1_KG, ADM_PARAM_ON)%1000000L;
		ADM_Status.Max2[0] = ADM_WaitData(ADM_CMD_GET_PARAMETER, ADM_SUB_MAX2_KG, ADM_PARAM_ON)%1000000L;
		ADM_Status.d1[0] = ADM_WaitData(ADM_CMD_GET_PARAMETER, ADM_SUB_D1_KG, ADM_PARAM_ON)%1000000L;
		ADM_Status.d2[0] = ADM_WaitData(ADM_CMD_GET_PARAMETER, ADM_SUB_D2_KG, ADM_PARAM_ON)%1000000L;

		ADM_Status.Max1[1] = ADM_WaitData(ADM_CMD_GET_PARAMETER, ADM_SUB_MAX1_LB, ADM_PARAM_ON)%1000000L;
		ADM_Status.Max2[1] = ADM_WaitData(ADM_CMD_GET_PARAMETER, ADM_SUB_MAX2_LB, ADM_PARAM_ON)%1000000L;
		ADM_Status.d1[1] = ADM_WaitData(ADM_CMD_GET_PARAMETER, ADM_SUB_D1_LB, ADM_PARAM_ON)%1000000L;
		ADM_Status.d2[1] = ADM_WaitData(ADM_CMD_GET_PARAMETER, ADM_SUB_D2_LB, ADM_PARAM_ON)%1000000L;
	}

	CurPluWeightUnit = ADM_GetData.CurUnit;

#ifdef USE_CANADA_ZERO_MARK_SETTING
	ltemp = ADM_WaitData(ADM_CMD_GET_ETC, 0, ADM_PARAM_OFF);
	ltemp |= 0x10; // Enable Net Zero Mark [Y]
	ltemp &= ~(0x20);// Enable Gross Zero Mark [N]
	ADM_WaitData(ADM_CMD_SET_ETC, ltemp, ADM_PARAM_ON);
#endif
	
	return err;
}				  

							
INT8U ADM_RecvProc(void)
{
	char flagNeg, i, loop;
	INT8U temp, status;
	INT8U rxbuf[2];
	INT32S ltemp;
	INT8U mode, cmd;
	INT8U received;

	status = FALSE;
	received = 0;
	loop = 1;	   
	while(CheckRxBufCount(&CommBufAD, 14) && loop) {
		temp = GetCharRxBuf(&CommBufAD);
		if(temp == 0x01) {
			if(CheckRxBufCount(&CommBufAD, 13)) {
				ltemp = 0;
				rxbuf[0] = GetCharRxBuf(&CommBufAD);
				rxbuf[1] = GetCharRxBuf(&CommBufAD);
				mode = hexstrtobyte((char*)rxbuf);

				rxbuf[0] = GetCharRxBuf(&CommBufAD);
				rxbuf[1] = GetCharRxBuf(&CommBufAD);
				cmd = hexstrtobyte((char*)rxbuf);//

				temp = GetCharRxBuf(&CommBufAD);
				flagNeg = 0;
				if(temp == '-') flagNeg = 1;
				else if(temp == '+') flagNeg = 0;
				else ltemp = (INT32S)(temp - '0');//i = 0;
				for(i = 0; i < 7; i++) {		// string size	check
					temp = GetCharRxBuf(&CommBufAD);
					ltemp *= 10;
					ltemp += (INT32S)(temp - '0');
				}			
				if(flagNeg) ltemp = 0 - ltemp;

				status = TRUE;
				if(GetCharRxBuf(&CommBufAD) != 0x04) {
					status = FALSE;
				}							     
				if(!CheckRxBufCount(&CommBufAD,14) || (mode&0x80)) {	// Recent Data or response of command
					if(status) {
						ADM_GetData.PrevStatus = ADM_GetData.Status;
						ADM_GetData.PrevRecvData = ADM_GetData.RecvData;
						ADM_GetData.PrevStable = ADM_GetData.Stable;
						ADM_GetData.RecvData = ltemp;
						ADM_GetData.RecvCmd = cmd;
						ADM_GetData.Status = mode;
						ADM_GetData.DataMode = ADM_DATAMODE_NONE;
						ADM_GetData.Stable = 0;
						ADM_GetData.Zero = 0;
						ADM_GetData.Tare = 0;
						ADM_GetData.Overload = 0;

						switch(mode&0xf0) {	// DataMode
							case 0x00 :	// kg Value
								ADM_GetData.DataMode = ADM_DATAMODE_WEIGHT_KG;
								ADM_GetData.CurUnit = WEIGHT_UNIT_KG;
								if(get_base_bparam(FLASH_ADC_WUNIT) == WEIGHT_UNIT_G) ADM_GetData.CurUnit = WEIGHT_UNIT_G;
								break;
							case 0x10 :	// C1 Value
								ADM_GetData.DataMode = ADM_DATAMODE_AD1;
								break;
							case 0x20 :
								ADM_GetData.DataMode = ADM_DATAMODE_AD2;
								break;
							case 0x30 :
								ADM_GetData.DataMode = ADM_DATAMODE_AD3;
								break;
							case 0x40 :
								ADM_GetData.DataMode = ADM_DATAMODE_AD4;
								break;
							case 0x50 :	// C5 Value
								ADM_GetData.DataMode = ADM_DATAMODE_AD5;
								break;
							case 0x70 :	// lb Value
								ADM_GetData.DataMode = ADM_DATAMODE_WEIGHT_LB;
								ADM_GetData.CurUnit = WEIGHT_UNIT_LB;
								break;
							case 0x80 :	// Error
								ADM_GetData.DataMode = ADM_DATAMODE_ERROR;
								break;
							case 0x90 :	// Initializing
								ADM_GetData.DataMode = ADM_DATAMODE_INIT_ERR;
								break;
							case 0xA0 :	// EEPROM Error
								ADM_GetData.DataMode = ADM_DATAMODE_EEP_ERR;
								break;
							case 0xf0 :	//	Ok
								ADM_GetData.DataMode = ADM_DATAMODE_OK;
								break;
						}
						if(mode&0x08)	ADM_GetData.Zero = 1;
						if(mode&0x04)	ADM_GetData.Overload = 1;
						if(mode&0x02)	ADM_GetData.Tare = 1;
						if(mode&0x01)	ADM_GetData.Stable = 1;
						if(ADM_GetData.Status != ADM_GetData.PrevStatus || ADM_GetData.PrevRecvData != ADM_GetData.RecvData) {
							ADM_GetData.ChangeData = 1;
						}
						received = 1;
					}
					loop = 0;
				}
				else {
				}
				if(ADM_flagDebug) {
					sprintf(MsgBuf, "<ADM:%d>  Mode:%02X  Cmd:%02X  Data:%7ld\r\n", received, mode, cmd, ltemp);
					MsgOut(MsgBuf);
				}
			} else {
				RewindRxBuf(&CommBufAD, 1);
				loop = 0;
			} // if(CheckRxBufCount(&CommBufAD, 13))
		}
	}		
	return status;
}

INT8U ADM_CheckInitWeighing(void)
{
	INT8U status;

	status = ADM_CHK_INIT_FAIL;
	if(ADM_RecvProc()) {
		status = ADM_CHK_INIT_OK;
		if(ADM_GetData.DataMode == ADM_DATAMODE_INIT_ERR) {
			if(!ADM_GetData.Stable) {
				status = ADM_CHK_INIT_ERR_STABLE;
			}
			else if(ADM_GetData.Overload) {
				status = ADM_CHK_INIT_ERR_LOAD;
			}
			else {
				status = ADM_CHK_INIT_FAIL;
			}
		}
		else if(ADM_GetData.DataMode == ADM_DATAMODE_EEP_ERR) {
			status = ADM_CHK_INIT_EEP_ERR;
		}
	}
	return status;
}

INT8U ADM_BackupEeprom(void)
{
	INT16U i;
	INT8U buf[ADM_EEP_BACKUP_SIZE];

	if(ADM_Status.Version == 0) return FALSE;	// not initialize module or not exist

	ADM_WaitData(ADM_CMD_SET_EEP_ADDR, 0, ADM_PARAM_ON);
	for(i = 0; i < ADM_Status.EepCsLocation+1; i++) {
		buf[i] = (INT8U)ADM_WaitData(ADM_CMD_GET_EEP_DATA, 0, ADM_PARAM_OFF);
	}
	Flash_swrite(ADM_BACKUP_ADDR, (HUGEDATA INT8U *)buf, ADM_Status.EepCsLocation+1);
	return TRUE;
}

INT8U ADM_RestoreEeprom(void)
{
	INT16U i;
	INT8U buf[ADM_EEP_BACKUP_SIZE];

	if(ADM_Status.Version == 0) return FALSE;	// not initialize module or not exist

	Flash_sread(ADM_BACKUP_ADDR, (HUGEDATA INT8U *)buf, ADM_Status.EepCsLocation+1);

	ADM_WaitData(ADM_CMD_SET_EEP_ADDR, 0, ADM_PARAM_ON);
	for(i = 0; i < ADM_Status.EepCsLocation+1; i++)
	{
		ADM_WaitData(ADM_CMD_SET_EEP_DATA, buf[i], ADM_PARAM_ON);
	}
	return TRUE;
}

INT8U ADM_CheckFlashBackup(void)
{
	INT8U bcc, c;
	INT8U i;

	if(ADM_Status.Version == 0) return FALSE;	// not initialize module or not exist

	bcc = 0;
	for(i = 0; i < ADM_Status.EepCsLocation; i++) {
		c = Flash_bread(ADM_BACKUP_ADDR+i);
		bcc ^= c;
	}
	c = Flash_bread(ADM_BACKUP_ADDR	+ ADM_Status.EepCsLocation);
	if(bcc == c) {
		return TRUE;
	} else {
		return FALSE;
	}
}

INT8U ADM_CompareCheckSum(void)
{
	INT8U f_bcc, a_bcc;

	if(ADM_Status.Version == 0) return FALSE;	// not initialize module or not exist

	f_bcc = Flash_bread(ADM_BACKUP_ADDR + ADM_Status.EepCsLocation);

	ADM_WaitData(ADM_CMD_SET_EEP_ADDR, ADM_Status.EepCsLocation, ADM_PARAM_ON);
	a_bcc = (INT8U)ADM_WaitData(ADM_CMD_GET_EEP_DATA, 0, ADM_PARAM_OFF);
	if(f_bcc == a_bcc) {
		return TRUE;
	} else {
		return FALSE;
	}
}

void ADM_ChangeByteFlash(INT32U ad_addr, INT8U data)
{
	INT8U bcc, c;

	bcc = Flash_bread(ADM_BACKUP_ADDR + ADM_Status.EepCsLocation);
	c = Flash_bread(ADM_BACKUP_ADDR+ad_addr);
	bcc ^= c;
	bcc ^= data;
	Flash_bwrite(ADM_BACKUP_ADDR + ad_addr, data);
	Flash_bwrite(ADM_BACKUP_ADDR + ADM_Status.EepCsLocation, bcc);
}

void ADM_ChangeWordFlash(INT32U ad_addr, INT16U data)
{
	INT8U dbyte;

	dbyte = (INT8U)(data&0x00ff);
	ADM_ChangeByteFlash(ad_addr+1, dbyte);
	data >>= 8;
	dbyte = (INT8U)(data&0x00ff);
	ADM_ChangeByteFlash(ad_addr, dbyte);
}

void ADM_ChangeLongFlash(INT32U ad_addr, INT32U data)
{
	INT8U dbyte;

	dbyte = (INT8U)(data&0x000000ff);
	ADM_ChangeByteFlash(ad_addr+3, dbyte);
	data >>= 8;
	dbyte = (INT8U)(data&0x000000ff);
	ADM_ChangeByteFlash(ad_addr+2, dbyte);
	data >>= 8;
	dbyte = (INT8U)(data&0x000000ff);
	ADM_ChangeByteFlash(ad_addr+1, dbyte);
	data >>= 8;
	dbyte = (INT8U)(data&0x000000ff);
	ADM_ChangeByteFlash(ad_addr, dbyte);
}

#ifdef USE_ADM_AUTO_RESTORE
void ADM_Init_error_count(void)
{
	INT8U init_err_cnt;
	init_err_cnt = Flash_bread(DFLASH_BASE + FLASH_ADM_INIT_ERROR_COUNT);
	if(init_err_cnt == 0xff) init_err_cnt = 0;
	init_err_cnt++;
	Flash_bwrite(DFLASH_BASE + FLASH_ADM_INIT_ERROR_COUNT, init_err_cnt);
}
void ADM_Eep_error_count(void)
{
	INT8U eep_err_cnt;
	eep_err_cnt = Flash_bread(DFLASH_BASE + FLASH_ADM_EEP_ERROR_COUNT);
	if(eep_err_cnt == 0xff) eep_err_cnt = 0;
	eep_err_cnt++;
	Flash_bwrite(DFLASH_BASE + FLASH_ADM_EEP_ERROR_COUNT, eep_err_cnt);
}
#endif