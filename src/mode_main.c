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

#include "mode_main.h"
#include "mode_sale.h"
#include "mode_program.h"
#include "mode_calibration.h"
#include "mode_error.h"

//STATUS_RUN   status_run;
//STATUS_SCALE status_scale;
//HYP 20060412 _huge --> _near for small code size
NEARDATA STATUS_RUN   status_run;
NEARDATA STATUS_SCALE status_scale;
#if defined(USE_REMOTE_FW_DOWNLOAD) || defined(USE_REMOTE_ROM_UPDATE)      
int UpdateFwPercnet = 0;
INT16U UpdateFwTimer = 0;
#endif

#include "mode_main.h"	    
extern void debug_Factory(void);

void mode_main_init()
{
	memset((INT8U*)&status_run,0,sizeof(STATUS_RUN));
	if (CAL) {
		status_run.run_mode=RUN_SALE;
		mode_sale_init();
		KEY_SetMode(0);
	} else {
#if defined(USE_CTS_FUNCTION)
		if (CtsStruct.CtsEnable)
		{
			status_run.run_mode = RUN_WAITING_CAL;
		}
		else
		{
			status_run.run_mode = RUN_CALIBRATION;
		}
#else
		status_run.run_mode=RUN_CALIBRATION;
#endif
		mode_sale_init();
		KEY_SetMode(1);
	}
}

void mode_main(void)
{
	INT8U loop;
#if defined(USE_REMOTE_FW_DOWNLOAD) || defined(USE_REMOTE_ROM_UPDATE)    
    static INT16U run_premode = 0;
    static int preUpdatePercnet = 0;
    char tempChar[32] = {0,};
    POINT disp_p; 
    char old_font;
#endif
	loop = TRUE;
	mode_main_init();
	CheckAdInitStatus();
	while (loop) {
//		switch(status_run.run_mode) {
//			case RUN_SALE :
//				break;
//			case RUN_PROGRAM:
//				break;
//			case RUN_CALIBRATION:
//				break;
//			case RUN_DEBUG:
//				break;
//		}
		switch(status_run.run_mode) {
			case RUN_SALE :
#if defined(USE_REMOTE_FW_DOWNLOAD) || defined(USE_REMOTE_ROM_UPDATE)                
                if (run_premode == RUN_FW_UPDATE) {
                    UpdateFwTimer = SysTimer100ms;
                }
                run_premode = RUN_SALE;
#endif                
				mode_sale();
				break;
			case RUN_PROGRAM:
//#ifdef USE_CHN_FONT
//				DspStruct.StrType = ONE_BYTE_FONT_STR;
//				mode_program();
//				DspStruct.StrType = TWO_BYTE_FONT_STR;
//#else
				mode_program();
//#endif
				break;
#ifdef USE_CTS_FUNCTION
			case RUN_WAITING_CAL:
				Uart0Init(BAUD_RATE_115200);
				CommBufInit(&CommBuf,COMM_TYPE_HOST,0);
				mode_waiting_cal();
				break;
#endif
			case RUN_CALIBRATION:
#ifndef USE_CTS_FUNCTION
				// Added by CJK
				Uart0Init(BAUD_RATE_115200);
				CommBufInit(&CommBuf,COMM_TYPE_HOST,0);//HYP 20041222
				///////////////
#endif
				mode_calibration();
				// Added by CJK
				Uart0Init(get_global_bparam(GLOBAL_AREA_RS232C_BAUD));
				CommBufInit(&CommBuf,COMM_TYPE_HOST,0);//HYP 20041222
				///////////////
				break;
			case RUN_DEBUG:
				DebugTerminal(&CommBuf);
				break;
			case RUN_BACKUP:
				DebugBackup();
				break;
			case RUN_FJIG:
				debug_Factory();
				break;
			case RUN_FJIG_EXIT:
				status_run.run_mode=RUN_SALE;
				break;
			case RUN_ERROR:
				mode_error();
				break;
			case RUN_RESTART:
				loop = FALSE;
				break;
#if defined(USE_REMOTE_FW_DOWNLOAD) || defined(USE_REMOTE_ROM_UPDATE)               
            case RUN_FW_UPDATE:
                old_font = DspStruct.Id1;
                DspLoadFont1(DSP_SMALL_FONT_ID);                
                if (run_premode == RUN_SALE) {
                   //Dwin_loading_msg(ON, tempChar);
                   UpdateFwTimer = SysTimer100ms;
                }
                
                if ((UpdateFwPercnet - preUpdatePercnet) >= 1) {
                    disp_p = point(0,0);
                    sprintf(tempChar, "PLEASE WAIT...%d%%", UpdateFwPercnet);
                    Dsp_Write_String(disp_p, (HUGEDATA char *)tempChar);
                    Dsp_Diffuse();                    
                }
                preUpdatePercnet = UpdateFwPercnet;
                
                if((INT16U)(SysTimer100ms - UpdateFwTimer) > 30) {
                    status_run.run_mode = RUN_SALE;
                }    
                run_premode = RUN_FW_UPDATE;
                network_check();
                DspLoadFont1(old_font);  
                break;
#endif                
		}
	}
}
