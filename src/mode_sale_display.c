#include <stdio.h>
#include <string.h>

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
#include "adm_app.h"
#include "key.h"
#include "key_typedef.h"
#include "key_function.h"
#include "key_app.h"

#include "cal.h"
#include "prt_cmdtest.h"
#include "programmode.h"
// #include "file.h"
#include "rtc.h"
#include "debug.h"
#include "ethernet.h"
#include "main.h"
#include "initial.h"
#include "flash_app.h"
#include "common.h"

#include "mode_sale.h"
#include "mode_main.h"
#include "mode_commun.h"
#include "mode_error.h"
#include "plu_manager.h"
#include "caption_app.h"
#include "dsp_app.h"
#include "rtc_app.h"
#include "plu_auxcall.h"
#include "discount.h"
#include "initial_glb.h"
#include "ethernet_comm.h"

extern PRICE_TYPE 	Price;
extern DISC_SALE_TYPE	DiscSale;

void sale_display_ethernet_sign(void)
{
  	static INT16U StartTimer100ms = 0;
	  
	if ((INT16U)(SysTimer100ms - StartTimer100ms) < 2) return;
	StartTimer100ms = SysTimer100ms;

#ifdef ETH_LED_INDICATOR_DISABLE
	display_sign_noflush(SIGN_ETH_LED_LINK,0);
#else
	if(ETH_LED_LINK) display_sign_noflush(SIGN_ETH_LED_LINK,1);
	else display_sign_noflush(SIGN_ETH_LED_LINK,0);	
#endif
}

#ifdef USE_CTS_FUNCTION
void sale_display_cts_alarm_sign(void)
{
	static INT16U StartTimer100ms = 0;
	
	if(!CTS_CheckCalibrationDate())
	{
		if ((INT16U)(SysTimer100ms - StartTimer100ms) < 5) return;
		StartTimer100ms = SysTimer100ms;
		
		if (SysTimer100ms / 5 % 2) display_sign_noflush(SIGN_CTS_ALARM, 1);
		else display_sign_noflush(SIGN_CTS_ALARM, 0);
	}
}
#endif

extern INT8U UsbdeviceIsConnected;
extern INT8U UsbHidConnected;
void common_display_sign(void) // sale mode, prog mode 공통
{
#ifdef USE_USB_DATA_ROM_UPDATE
	if(UsbdeviceIsConnected || UsbHidConnected) {
		display_sign_noflush(SIGN_USBMEM,1);
	} else {
		display_sign_noflush(SIGN_USBMEM,0);
	}
#endif
}

//DEL070704extern void sale_over_write(char *str);
#ifdef USE_PB_BATTERY
extern INT8U BatterySign1;
extern INT8U BatterySign2;
extern INT8U BatterySign3;
#endif
void sale_display_sign(void)
{
	INT16U chk;
   
	//chg_flag = get_global_bparam(GLOBAL_SALE_SETUP_FLAG8) & 0x40;	//parameter 723
	chk = UPDATE_STATUS;

	if (status_scale.cur_displayon & chk)
	{
		status_scale.cur_displayon &= (~chk);

		if ((ADM_GetData.Tare) || Price.PercentTareWeight) {
			display_sign_noflush(SIGN_TARE,1);
		} else {
			display_sign_noflush(SIGN_TARE,0);
		}
		if (ADM_GetData.Zero) {
			display_sign_noflush(SIGN_ZERO,1);
		} else {
			display_sign_noflush(SIGN_ZERO,0);
		}
		if(ADM_GetData.Stable) {
			display_sign_noflush(SIGN_STABLE,1);
		} else {
			display_sign_noflush(SIGN_STABLE,0);
		}
	}

	chk = UPDATE_MODE;
	if (status_scale.cur_displayon & chk)
	{
		status_scale.cur_displayon &= (~chk);      
		if (ethernet_list.status==1) {
			display_sign_noflush(SIGN_MASTER,1);
		}

        
		if (Operation.wAuto) {
			display_sign_noflush(SIGN_AUTO,1);
		} else {
			display_sign_noflush(SIGN_AUTO,0);
		}
		if (Operation.wSave) {
			display_sign_noflush(SIGN_SAVE,1);
		} else {
			display_sign_noflush(SIGN_SAVE,0);
		}
#if defined(USE_GSMART_TRACE_FUNCTION) || defined(USE_EMART_DEFAULT)
		if (Operation.wPrepack) {
			display_sign_noflush(SIGN_AUTO,1);//GS Mart
		} else {
			display_sign_noflush(SIGN_AUTO,0); //GS Mart
		}
#else
		if (Operation.wPrepack) {
			display_sign_noflush(SIGN_PREPACK,1);
		} else {
			display_sign_noflush(SIGN_PREPACK,0);
		}
#endif
		if(KeyDrv.Shifted) {
			display_sign_noflush(SIGN_SHIFT,1);
		} else {
			display_sign_noflush(SIGN_SHIFT,0);
		}
		if (DiscSale.saleDiscountType) {
			display_sign_noflush(SIGN_DISCOUNT,1);
		} else {
			display_sign_noflush(SIGN_DISCOUNT,0);
		}
#ifdef USE_PB_BATTERY
		if (BatterySign1) {
			display_sign_noflush(SIGN_BAT1,1);
		} else {
			display_sign_noflush(SIGN_BAT1,0);
		}
		if (BatterySign2) {
			display_sign_noflush(SIGN_BAT2,1);
		} else {
			display_sign_noflush(SIGN_BAT2,0);
		}
		if (BatterySign3) {
			display_sign_noflush(SIGN_BAT3,1);
		} else {
			display_sign_noflush(SIGN_BAT3,0);
		}
#endif

		display_sign_noflush(SIGN_LANG,0);
		//if(Startup.country == COUNTRY_US) {
		if (GlbOper.TriangleType == 1 || GlbOper.TriangleType == 2) //1:USA,CANADA, 2:Mexico
		{
			if (ADM_GetData.CurUnit == WEIGHT_UNIT_KG) 
			{
				display_sign_noflush(SIGN_KG,1);
				display_sign_noflush(SIGN_LB,0);
				display_sign_noflush(SIGN_PER_LB,0);
				if(status_scale.cur_weightunit == 100)		//100g triangle  Display(USA)
				{
					display_sign_noflush(SIGN_PER_KG,0);
					display_sign_noflush(SIGN_PER_G,1);
				}
				else
				{
					display_sign_noflush(SIGN_PER_KG,1);
					display_sign_noflush(SIGN_PER_G,0);
				}
			}
			else	//LB 
			{
				display_sign_noflush(SIGN_KG,0);
				display_sign_noflush(SIGN_LB,1);
				display_sign_noflush(SIGN_PER_KG,0);
				display_sign_noflush(SIGN_PER_LB,1);
				display_sign_noflush(SIGN_PER_G,0);
			}
		}
		else
		{
			if(network_status.connection) {
				display_sign_noflush(SIGN_CONNECT,1);
			} else {
				display_sign_noflush(SIGN_CONNECT,0);
			}

#ifdef USE_EMART_NETWORK
			if(Operation.checkin) {
				display_sign_noflush(SIGN_CHECKIN,1);
			} else {
				display_sign_noflush(SIGN_CHECKIN,0);
			}
#endif
		}   
	}
}


void sale_display_lock(INT8U lock)
{
	static  INT8U o_lock=0;
	INT8U	y_gab=8;				// 폰트의 y 크기
	POINT 	disp_p;
	INT8U	str[32];

	if (o_lock!=lock) {
	   if (lock) {
		display_alloff_primary_indication();
		//caption_get_name(0xf207,(char*)str);
		sprintf((char *)str,"LOCK");
		display_string(DISPLAY_WEIGHT   ,str);
		sprintf((char *)str,"-----");
		display_string(DISPLAY_UNITPRICE,str);
		display_string(DISPLAY_TARE     ,str);
		display_string(DISPLAY_PRICE    ,str);
		disp_p = point( 1*y_gab, 0);
		caption_get_name(0xf208,(char*)str);
		DspStruct.DirectDraw = 1;
		Dsp_Fill_Buffer(0);
		DspLoadFont1(DSP_PLU_FONT_ID);
		Dsp_Write_String(disp_p,(HUGEDATA char *)str);
	   } else {
		status_scale.cur_displayon=0x0FFF;
	   }
	   Dsp_Diffuse();
	   o_lock=lock;
	}
//	Dsp_SetPage(DSP_DEFAULT_PAGE);
}
