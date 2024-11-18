/*****************************************************************************
|*			
|*  Copyright		:	(c) 2006 CAS
|*  Filename		:	prt_jig.c
|*  Version		:	0.1
|*  Programmer(s)	:	Hong Yi-Phyo (HYP)
|*  Created		:	2006/04/20
|*  Description		:	Use only Printer Jig  (comfile)
|*				
****************************************************************************/

/* 
   CL5000 Printer Jig 설명 
   2006년 4월 20일 작성함

   1. Printer PCB & mechanism Test 프로그램 
       1.1 main.c 의 #define ETH_JIG_PGM과 #define PRT_JIG_PGM
	   prt_jig.c의 #define PRT_JIG_PGM2 를 열고 Compile 한다.
     
       1.2 CAL S/W를 누른 상태에서 전원을 켜면 일반 모드로 들어간다.
       1.3 CAL S/W를 누르지 않은 상태에서 전원을 켜면 JIG 모드로 진입한다.
            a. Printer Sensor Calibration(Peel-off, Gap sensor check)
            b. Head-up Sensor
	    c. TPH printing
	    d. Print result label
*/

//#define PRT_JIG_PGM2//HYP 20060430
#ifdef PRT_JIG_PGM2

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

//HYP 20050726
#include "ps2.h"
#include "key_app.h"
#include "ethernet_comm.h"
#include "menu.h"
#include "pgm_app.h"
#include "key_typedef.h"
#include "network_command.h"
#include "prt_cmd.h"
#include "prt_app.h"

//char test_str[]="TEST TEST TEST OK!";
void MENU_MSG_4_jig(INT8U line, ROMDATA char *rstr) 
{
	char str[50];
	POINT disp_p;

	DspStruct.DirectDraw = 1;
	strcpy(str, rstr);
	disp_p = point(line*8, 0);
	Dsp_Write_String(disp_p,str);
//	Dsp_Write_String(disp_p,test_str);
}

extern INT8U PrtFontType;
extern void StrInit4UserLabel(void);
void Print_Result_Label_4_jig(INT8U peel_ok, INT8U gap_ok, INT8U heapup_ok)
{
	char string_buf[50];

	LabCmd_ClearLabel();
	StrInit4UserLabel();	// Added by CJK 20060426
	RcptCmd_SetFontType(0);//0=one byte font, 1=two byte font //SG060313 //HYP 20060315
	RcptCmd_SetFontId(3);
	RcptCmd_IntercharSP(0);
//	LabCmd_SetStrField(40, 80, 200, 428);
	LabCmd_SetStrField(40, 40, 200, 428);
	LabCmd_CharSize(2, 2);
	LabCmd_StrStart();

	sprintf(string_buf, "<Printer Test Result>\n");
	PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));

	if(heapup_ok) {
		sprintf(string_buf, "Head-Up Sensor  : OK\n");
	} else {
		sprintf(string_buf, "Head-Up Sensor  : N.G\n");
	}
	PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));

	if(peel_ok) {
		sprintf(string_buf, "Peel-off Sensor : OK\n");
	} else {
		sprintf(string_buf, "Peel-off Sensor : N.G\n");
	}
	PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));

	if(gap_ok) {
		sprintf(string_buf, "Gap Sensor      : OK\n");
	} else {
		sprintf(string_buf, "Gap Sensor      : N.G\n");
	}
	PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));

	sprintf(string_buf, "TPH Printing    : N/A\n");
	PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));

	LabCmd_StrEnd();
	RcptCmd_TypeNone();
	LabCmd_PrintStart(1);
	PrintListWaitEnd();
}

extern void  printer_sensor_keytest(INT16U *t1,INT16U *t2,INT8U fontid);

void main_prt_menu_4_jig(void)
{
	INT16U result,temp[2];
	char string_buf[10];
	INT8U peel_ok, gap_ok, heapup_ok, loop, hstep;

	peel_ok=0; gap_ok=0; heapup_ok=0;
	DspLoadFont1(2);//DSP_SYSTEM_FONT_ID

//Headup Test
	MENU_MSG_4_jig(0,"1.Head-Up Sensor Test  :");
	MENU_MSG_4_jig(3,"On&Off Printer Head S/W   ");
	 
	loop=1;
	hstep = 0;
	KEY_SetMode(1);
	while(loop) {
		string_buf[0] = GS;
		string_buf[1] = 'V';
		PutDataRxBuf(&CommBufPrt, string_buf, 2);

//		while(CheckRxBuf(&CommBufPrt)) {
//			CASInterpreter(&CommBufPrt);
//		}
//		while(PrtDrvStruct.PrtState != PRT_STATE0I);
		PrintListWaitEndWithoutComm();

		if(PrtDrvStruct.HeadUpFlag == ON) hstep=1;
		if(PrtDrvStruct.HeadUpFlag == OFF && hstep==1) {
			loop=0;
			heapup_ok=1;
		}
		if(KEY_Read()) {
			BuzOn(1);
			loop=0;
		}
	}
	if(heapup_ok == 0) {
		MENU_MSG_4_jig(0,"1.Head-Up Sensor Test  : N.G");
	} else {
		MENU_MSG_4_jig(0,"1.Head-Up Sensor Test  : OK");
	}

//Peel & Gap Test
	MENU_MSG_4_jig(1,"2.Peel&Gap Sensor Test :");
	MENU_MSG_4_jig(3,"Press PRINT Key for Step 1");
	
	KEY_SetMode(1);
/*
	while(!KEY_Read());
	BuzOn(1);
	result = KP_ENTER;//Any Key
*/
	Delay_100ms(5);

	printer_sensor_keytest(&temp[0],&temp[1],3);
//	if(SensorStruct.PeelThreshold>50) {
		if((SensorStruct.MaxPeelValue - SensorStruct.MinPeelValue) > 80){
			peel_ok=1;
		}
//	}
//	if(SensorStruct.GapThreshold>50) {
		if((SensorStruct.MaxGapValue - SensorStruct.MinGapValue) > 80){
			gap_ok=1;
		}
//	}

	if(peel_ok == 0 || gap_ok == 0) {
		MENU_MSG_4_jig(1,"2.Peel&Gap Sensor Test : N.G");
	} else {
		MENU_MSG_4_jig(1,"2.Peel&Gap Sensor Test : OK");
	}

//Motor & Tph Test
//	MENU_MSG_4_jig(2,"3.TPH Printing Test    : N/A");
	MENU_MSG_4_jig(2,"3.TPH Printing Test         ");
	MENU_MSG_4_jig(3,"Press PRINT Key for Step 3");
	while(!KEY_Read());
	string_buf[0] = ESC;
	string_buf[1] = 'T';
	string_buf[2] = ESC;
	string_buf[3] = '!';
	string_buf[4] = 0x01;
	PutDataRxBuf(&CommBufPrt, string_buf, 5);
//	while(CheckRxBuf(&CommBufPrt)) {
//		CASInterpreter(&CommBufPrt);
//	}
//	while(PrtDrvStruct.PrtState != PRT_STATE0I);
	PrintListWaitEndWithoutComm();
//Print Result Label
	MENU_MSG_4_jig(3,"Press PRINT Key for Report");

//20091104 printer jig 기능 추가	
KEY_INPUT:
	while(!KEY_Read()) ;
	switch(KeyDrv.CnvCode) 
	{
		case KP_TEST :
			string_buf[0] = ESC;
			string_buf[1] = 'T';
			string_buf[2] = ESC;
			string_buf[3] = '!';
			string_buf[4] = 0x01;
			PutDataRxBuf(&CommBufPrt, string_buf, 5);
//			while(CheckRxBuf(&CommBufPrt)) {
//				CASInterpreter(&CommBufPrt);
//			}
//			while(PrtDrvStruct.PrtState != PRT_STATE0I);
			PrintListWaitEndWithoutComm();
			goto KEY_INPUT;
		default :
			Print_Result_Label_4_jig(peel_ok, gap_ok, heapup_ok);
			break;
	}

//END
	MENU_MSG_4_jig(3,"Please Power Off&On for Next Test");
	while(1);
}

void main_prt_4_jig(void) 
{
	INT8U i;//HYP 20041222

	BUZZER_HIGH;    
	PortInit();
	TimerInit();
	SensorInit();
	//memset((char *)&Startup,0,sizeof(Startup));
	Uart0Init(BAUD_RATE_115200);	// PC
	CommBufInit(&CommBuf,COMM_TYPE_HOST,0);
	Flash_init_nvbuffer();
	Uart1Init(BAUD_RATE_57600); 	// ADM
	CommBufInit(&CommBufAD,COMM_TYPE_AD,0);
	CommBufInit(&CommBufKey,COMM_TYPE_KEY,0);	
	CommBufInit(&CommBufUsb,COMM_TYPE_USB,0);	
	CpldInit();
	PrtInit();

	PrtDrvInit();
	CommBufInit(&CommBufPrt,COMM_TYPE_PRT,0);
	
	RTCInit();
	CommBufInit(&CommBufEth,COMM_TYPE_ETH,0);
	for(i=0; i<ETH_BUF_MAX_NUM; i++) {
		CommBufInit(&(CommBufEthData[i]),COMM_TYPE_ETHDATA,i);	 //Ethernet
	}
	Buzzer.Status = 0;
	Buzzer.Times = 0;
  	PrtDrvStruct.SysTimer = 0;
	PrtDrvStart(PRT_STATE_RW);//Initial prtdrv state
	Timer0Start(); //this function is pollowed after PrtDrvStart(PRT_STATE_RW);
 	PSWH &= 0xf0;	// lower interrupt priority 

	BUZZER_LOW;    
	//VFD7_Init();

	CommBufInit(&CommBuf,COMM_TYPE_HOST,0);////HYP 20041222
	Dsp_InitHardware();
	//LCD_Init();

	KEY_Init();
	PS2_InitKeyBuf();
	keyin_Init();

	serial_init();

	network_param_init();
	main_prt_menu_4_jig();
}
#endif//#ifdef PRT_JIG_PGM2 