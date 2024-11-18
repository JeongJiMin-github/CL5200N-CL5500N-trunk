#include <stdio.h>
#include "globals.h"
#include "serial.h"
#include "flash.h"

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
#include "main.h"
#include "initial.h"
#include "flash_app.h"
#include "common.h"
#include "caption_app.h"
#include "mode_calibration.h"
#include "mode_main.h"
#include "mode_error.h"
#include "mode_commun.h"
#include "key_function.h"
#include "dsp_app.h"

void message_error(POINT disp_p)
{
	char buffer[64];
	INT8U backup_directdraw;

	sprintf((char*)buffer, "caption err");
	backup_directdraw = DspStruct.DirectDraw;
	DspStruct.DirectDraw = 1;
#ifdef HEBREW_FONT
	Convert_HebrewPreview((INT8U *)buffer, strlen(buffer), 0, 0);
	hebrew_codetemp[strlen(buffer)] = 0;
	Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
	Convert_ArabPreview((INT8U *)buffer, strlen(buffer), 0, 0, 0); 
	arab_codetemp[strlen(buffer)] = 0;
	Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
	Dsp_Write_String(disp_p,(HUGEDATA char *)buffer);
#endif
	DspStruct.DirectDraw = backup_directdraw;
}

void mode_error(void)
{
	POINT p;

	DspStruct.DirectDraw = 0;
	Dsp_Fill_Buffer(0);

	DspLoadFont1(DSP_PLU_FONT_ID);
	p = point(10, 10);
	sprintf(MsgBuf,"Error!!");
#ifdef HEBREW_FONT
	Convert_HebrewPreview((INT8U *)MsgBuf, strlen(MsgBuf), 0, 0);
	hebrew_codetemp[strlen(MsgBuf)] = 0;
	p.x = 202 - p.x;
	Dsp_Write_1stringLong(p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
	Convert_ArabPreview((INT8U *)MsgBuf, strlen(MsgBuf), 0, 0, 0); 
	arab_codetemp[strlen(MsgBuf)] = 0;
	p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - p.x;
	Dsp_Write_1stringLong(p, arab_codetemp);
#else
	Dsp_Write_String(p,MsgBuf);
#endif
	Dsp_Diffuse();
	DspStruct.DirectDraw = 1;

	while(1) {
		serial_proc(&CommBuf);
	}
}

//extern INT16S DisplayMsg(char *msg);
void mode_error_disp(INT16U errcode,INT8U action)
{
//	POINT p;
	char  msg_buf[33];
	INT32U err_addr;
	if (!action) {
		if (errcode) BuzOn(1);
//		DspLoadFont1(DSP_MENU_FONT_ID);
//		p = point(0,0);
		memset((INT8U *)msg_buf,0x20,32);
		if (errcode) {
			err_addr=caption_get_name(errcode,msg_buf);
			if (!err_addr) {
				sprintf(msg_buf,"E:%04X",errcode);
			}
		}
		msg_buf[22]=0;
		//Dsp_Write_String(p,msg_buf);
		//PutNSmallString(0,0,msg_buf,33);//
		DisplayMsg(msg_buf);
	} else {
		BuzOn(2);
	}
}

//DEL071206void warning_message(INT16U w,char *msg)
//DEL071206{
//DEL071206	INT8U	str[32];
//DEL071206	INT8U	y,old_font;				// 폰트의 y 크기
//DEL071206	POINT 	disp_p;
//DEL071206
//DEL071206	caption_get_name(w,(char*)str);
//DEL071206	switch (status_run.run_mode) {
//DEL071206	    case RUN_SALE: 
//DEL071206		if (msg) {
//DEL071206			goto DEF;
//DEL071206		} else {
//DEL071206			DisplayMsg((char *)str);
//DEL071206		}
//DEL071206	    	break;
//DEL071206	    default      :
//DEL071206DEF:		old_font=DspStruct.Id1;
//DEL071206		if (DspStruct.Page!=DSP_ERROR_PAGE) {
//DEL071206			Dsp_SetPage(DSP_ERROR_PAGE);
//DEL071206			DspLoadFont1(DSP_MENU_FONT_ID);
//DEL071206			y=1;
//DEL071206			if (msg) y=0;
//DEL071206			disp_p = point( y*Startup.menu_ygap, 0);
//DEL071206			caption_get_name(w,(char*)str);
//DEL071206			Dsp_Write_String(disp_p,(HUGEDATA char *)str);
//DEL071206			if (msg) {
//DEL071206				y++;
//DEL071206				disp_p = point( y*Startup.menu_ygap, 0);
//DEL071206//				caption_get_name(w,str);
//DEL071206				Dsp_Write_String(disp_p,(HUGEDATA char *)msg);
//DEL071206			}
//DEL071206			Dsp_Diffuse();
//DEL071206		} else {
//DEL071206		//
//DEL071206		}
//DEL071206		Delay_100ms(10);
//DEL071206		Dsp_SetPage(DSP_DEFAULT_PAGE);
//DEL071206		DspLoadFont1(old_font);
//DEL071206		Dsp_Diffuse();
//DEL071206	}
//DEL071206}
