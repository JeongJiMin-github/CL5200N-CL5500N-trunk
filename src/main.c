/*****************************************************************************
|*			
|*  Copyright		:	(c) 2002 CAS
|*  Filename		:	main.c
|*  Version			:	0.1
|*  Programmer(s)	:	Chung jong kwon (CJK)
|*  Created			:	2003/03/20
|*  Modify              :       2004/02/25 
|*  Description		:	CLP Printer main routine    
|*				
****************************************************************************/
 
 
#include <stdio.h>
#include "globals.h"

#include "commbuf.h"
#include "prt_common.h"
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
#include "dsp_vfd7.h"
#include "adm.h"
#include "key.h"
#include "cal.h"
#include "prt_cmdtest.h"
#include "programmode.h"
//#include "file.h"
#include "rtc.h"
#include "debug.h"
#include "ethernet.h"
#include "initial.h"
#include "flash_app.h"
#include "mode_main.h"
#include "mode_commun.h"
#include "plu_manager.h"
#include "dsp_app.h"
#include "comm_app.h"
#include "mode_sale.h"
#include "main.h"
#include "caption_app.h"
#include "common.h"
#include "network_command.h"

HUGEDATA BUZZER_STR Buzzer;
HUGEDATA INT16U SysTimer1_25ms;
HUGEDATA INT16U SysTimer10ms;
HUGEDATA INT16U SysTimer100ms;
HUGEDATA INT16U SysTimerMin;
HUGEDATA INT32U SysTimerForDHCP;
#ifdef USE_MORNITORING_CNT
INT8U SysOperatingTime = 0;
INT8U Enable_operate_time_cnt = ON;
REMOTE_STRUCT OperTime;
#endif

DEBUG_TIME_STR DebugTime;
char MsgBuf[100];

void ADM_Receive(void);
void ETH_RecvProc(void);

void uip_log(char *m)
{
  printf("uIP log message: %s\n", m);
}

void software_reset(void)
{
#if defined(USE_REMOTE_FW_DOWNLOAD) || defined(USE_REMOTE_ROM_UPDATE)
	PLIB_ETH_Disable(ETH_ID_0); //Ethernet DeInitialize 할 때 꼭 필요함.
	BSP_DeInitialize();
	SYS_INT_SourceDisable(INT_SOURCE_ETH_1);    
#endif
#ifdef USE_MORNITORING_CNT  
	rtc_save_operated_time();
	PrtSaveTphUseData();
#endif//#ifdef USE_MORNITORING_CNT
	External_Interrupt_Disable(); // Power off Pending
//	if (FtlBackupEnableFlag)
	{
		FtlDataBackupAllDataProgram(OFF);
	}

	External_Interrupt_Enable();
	SYS_RESET_SoftwareReset();
}

void __attribute__((optimize("-O0"))) Delay_100ms(INT16U cnt)
{
	INT16U tic;
	tic=SysTimer100ms;
    while (cnt > (INT16U)(SysTimer100ms-tic));	// Delay
}

void __attribute__((optimize("-O0"))) Delay_100ms_w_TcpIp(INT16U cnt)
{
	INT16U tic;
	tic=SysTimer100ms;
	while (cnt > (INT16U)(SysTimer100ms-tic)) network_common_trans();	// Delay
}

void __attribute__((optimize("-O0"))) Delay_10ms(INT16U cnt)
{
	INT16U tic;
	tic=PrtDrvStruct.Timer10ms;
	while(cnt > (INT16U)(PrtDrvStruct.Timer10ms-tic));		// Delay
}

void __attribute__((optimize("-O0"))) Delay_10ms_w_TcpIp(INT16U cnt)
{
	INT16U tic;
	tic=PrtDrvStruct.Timer10ms;
	while(cnt > (INT16U)(PrtDrvStruct.Timer10ms-tic)) network_common_trans();
}

INT8U Delay_100ms_w_net(INT16U cnt, INT8U use_key, INT8U use_comm)
{
	INT16U tic;
	INT8U Slave_flag, transaction_clerk;
	tic=SysTimer100ms;
	while (cnt > (INT16U)(SysTimer100ms-tic))
	{
		if(use_key)
		{
//			KEY_proc();
			if (KEY_IsKey()) return 1;
		}
		if(use_comm)
		{
			network_check();
		}
	}
	return 0;
}

void BuzOn(INT8U cnt)
{
#ifdef USE_KEY_BUZZER
	if (cnt == 1) return;

	Buzzer.Times += cnt;
#else
#ifndef USE_BUZZER_OFF_TEST
	Buzzer.Times = cnt;
#endif
#endif
}

void BuzOnAdd(INT8U cnt)
{
	Buzzer.Times += cnt;
}

//void PcInterface(void)
//{
//	if (CheckRxBuf(&CommBufEth)) {
//		PutStrTxBuf_copy(&CommBuf, &CommBufEth);
//	}
//}
void CheckAdInitStatus(void)
{
	POINT p;
	INT8U status, prev_status;
	INT16U cnt_wait;
	char string_buf[34];
	INT8U backup_direct_draw;

	cnt_wait = 0;
	prev_status = 0xff;

	ADM_Restart();

	DspLoadFont1(DSP_PLU_FONT_ID);

	backup_direct_draw = DspStruct.DirectDraw;
	DspStruct.DirectDraw = 0;
	Dsp_Fill_Buffer(0);
	Dsp_Diffuse();
	DspStruct.DirectDraw = 1;
	do {
#ifdef USE_CONTINUE_INIT_ZERO_ERROR
		if(status != ADM_CHK_INIT_ERR_LOAD)		//영점통과에러 경우 AD 상태 체크 X (Error 표시 유지)
		{
			status = ADM_CheckInitWeighing();
		}
#else
		status = ADM_CheckInitWeighing();
#endif
		KEY_Read();
		if(status != ADM_CHK_INIT_FAIL) cnt_wait++;
		if(cnt_wait > 15) {
			cnt_wait = 15;
			p = point(0, 0);
		        //p = point(0, 10);
			if(status != prev_status) {
				memset((INT8U *)string_buf, ' ', 32);
				string_buf[32] = 0;
				Dsp_Write_String(p, string_buf);
				switch(status) {
					case ADM_CHK_INIT_ERR_STABLE :
						caption_get_name(0xE502,string_buf);	//Unstable !!!
						break;
					case ADM_CHK_INIT_ERR_LOAD :
						caption_get_name(0xE503,string_buf);	//Zero Range Error
						break;
					case ADM_CHK_INIT_OK :
						//caption_get_name(0xf143,string_buf);	//Initial OK
						break;
					case ADM_CHK_INIT_EEP_ERR :
						caption_get_name(0xE505,string_buf);	//Invalid AD Module
						break;
					default :
						caption_get_name(0xE504,string_buf);	//Invalid AD Module
						break;
				}
				string_buf[32] = 0;
#ifdef USE_ARAB_CONVERT
                Convert_ArabPreview((INT8U *)string_buf, strlen((char *)string_buf), 0, 0, 0); 
				arab_codetemp[strlen((char *)string_buf)] = 0;
				p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - p.x;
				Dsp_Write_1stringLong(p, arab_codetemp);
#else
				Dsp_Write_String(p, string_buf);
#endif
			}
			prev_status = status;
		}		
	} while(status != ADM_CHK_INIT_OK);

	DspStruct.DirectDraw = backup_direct_draw;
}


#define MAIN_PGM
//#define ETH_JIG_PGM//HYP 20050726
//#define PRT_JIG_PGM//HYP 20060430
//#define ECCENTRICTY_JIG_PGM// CJK 20051117


#if defined(MAIN_PGM)

///////////////////////////////////////////
// Main
void main_app(void)
{
	init_main();
	while(1)
	{
		mode_main();
	}
}
///////////////////////////////////////////
///////////////////////////////////////////

#elif defined(ETH_JIG_PGM)	//#if defined(MAIN_PGM)

///////////////////////////////////////////
/// Ethernet Jig
extern void main_eth_4_jig(void);
void main_app(void)	
{
#include "portcfg.h"
	if (CAL) {	// Cal switch Off
		main_eth_4_jig();
	} else {	// Cal switch On
	}
	init_main();
	mode_main();
}
///////////////////////////////////////////
///////////////////////////////////////////

#elif defined(PRT_JIG_PGM)	//#elif defined(ETH_JIG_PGM)

///////////////////////////////////////////
/// Printer Jig
extern void main_prt_4_jig(void);
void main_app(void)	
{
#include "portcfg.h"
	if (CAL) {	// Cal switch Off
		main_prt_4_jig();
	} else {	// Cal switch On
		init_main();
		mode_main();
	}
}
///////////////////////////////////////////
///////////////////////////////////////////

#elif defined(ECCENTRICTY_JIG_PGM)	//#elif defined(PRT_JIG_PGM)

///////////////////////////////////////////
// Eccentricity	Jig
#include "key_typedef.h"
void main_app(void)
{
#include "portcfg.h"
	init_main();
	//mode_main();
	display_alloff_primary_indication();
	VFD7_Diffuse();
	while(KeyDrv.CnvCode != KP_ESC)	CalRun_TestAD();
}
///////////////////////////////////////////
///////////////////////////////////////////

#endif	//#elif defined(ECCENTRICTY_JIG_PGM)

int main(void)
{
	main_app();
	
	return 0;
}

/* Exception Handler Variables */
/* Static in case exception condition would stop auto variable from being created */
static unsigned int _excep_code;
static unsigned int _excep_addr;

#define EXCCODE_INTERRUPT				      0x00
#define EXCCODE_TLB_MODIFY				      0x01
#define EXCCODE_TLB_ERROR_LOAD                0x02
#define EXCCODE_TLB_ERROR_STORE			      0x03
#define EXCCODE_ADDRESS_ERROR_LOAD		      0x04
#define EXCCODE_ADDRESS_ERROR_STORE			  0x05
#define EXCCODE_BUS_ERROR_FETCH 			  0x06
#define EXCCODE_BUS_ERROR_STORE 			  0x07
#define EXCCODE_SYSCALL_ERROR   			  0x08
#define EXCCODE_BREAK_POINT_ERROR   		  0x09
#define EXCCODE_RESERVED_INSTRUCT_ERROR		  0x0A
#define EXCCODE_COPPROCESSOR_UNSTABLE		  0x0B
#define EXCCODE_ARITHMETIC_OVERFLOW 		  0x0C
#define EXCCODE_TRAP_EXCEPTION				  0x0D
#define EXCCODE_TLB_READ_INHIBIT			  0x13
#define EXCCODE_TLB_EXECUTE_INHIBIT			  0x14
#define EXCCODE_REF_WATCH_ERROR 			  0x17
#define EXCCODE_MACHINE_CHECK    			  0x18
#define EXCCODE_DSP_ASE_STATE_DISABLE		  0x1A

void __attribute__((nomips16)) _on_reset(void)
{

}
void __attribute__((nomips16)) _general_exception_handler(void)
{
    // Mask off the ExcCode Field from the Cause Register
    // Refer to FRM Section-50 CPU - DS61192
    _excep_code=(_CP0_GET_CAUSE() & 0x0000007C) >> 2;
    _excep_addr=_CP0_GET_EPC();

    // Disable Interrupts
    _CP0_SET_STATUS(_CP0_GET_STATUS()&0xFFFFFFE);
    // Examine _excep_code to identify the type of exception
    // Examine _excep_addr to find the address that caused the exception
    // Perform an indication of the exception

	switch(_excep_code)
	{
		case EXCCODE_INTERRUPT:					while(1); break;
		case EXCCODE_TLB_MODIFY:				while(1); break;
		case EXCCODE_TLB_ERROR_LOAD:			while(1); break;
		case EXCCODE_TLB_ERROR_STORE:			while(1); break;
		case EXCCODE_ADDRESS_ERROR_LOAD:		while(1); break;
		case EXCCODE_ADDRESS_ERROR_STORE:		while(1); break;
		case EXCCODE_BUS_ERROR_FETCH:			while(1); break;
		case EXCCODE_BUS_ERROR_STORE:			while(1); break;
		case EXCCODE_SYSCALL_ERROR:				while(1); break;
		case EXCCODE_BREAK_POINT_ERROR:			while(1); break;
		case EXCCODE_RESERVED_INSTRUCT_ERROR:	while(1); break;
		case EXCCODE_COPPROCESSOR_UNSTABLE: 	while(1); break;
		case EXCCODE_ARITHMETIC_OVERFLOW:		while(1); break;
		case EXCCODE_TRAP_EXCEPTION:			while(1); break;
		case EXCCODE_TLB_READ_INHIBIT:			while(1); break;
		case EXCCODE_TLB_EXECUTE_INHIBIT:		while(1); break;
		case EXCCODE_REF_WATCH_ERROR:			while(1); break;
		case EXCCODE_MACHINE_CHECK:				while(1); break;
		case EXCCODE_DSP_ASE_STATE_DISABLE:		while(1); break;
		default:								while(1); break;
	}
}