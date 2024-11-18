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
#include "key.h"
#include "cal.h"
#include "programmode.h"
#include "rtc.h"
#include "debug.h"
#include "main.h"
#include "initial.h"
#include "flash_app.h"
#include "common.h"
#include "dsp_app.h"
#include "rtc_app.h"
#include "key_typedef.h"
#include "clerk_app.h"
#include "department.h"
#include "mode_sale.h"
#include "mode_main.h"
#include "global_set.h"
#include "plu_manager.h"
#include "adm_app.h"
#include "plu_device.h"

NEARDATA DISPLAY_STRUCT display_parameter;//HYP 20060412 _huge --> _near for small code size

INT8U display_tare_off_flag = 1;

// USA, Canada, Mexico, Standard 사양 triangle 위치 동일
INT8U signTableIndex[DSP_MAX_SYMBOL] = 
{
	21, // SIGN_STABLE
	19, // SIGN_ZERO
	20, // SIGN_TARE
	9, // SIGN_SAVE
	7, // SIGN_AUTO
	11, // SIGN_PREPACK
	14, // SIGN_DISCOUNT
	16, // SIGN_SHIFT
	29, // SIGN_MASTER
	18, // SIGN_NET
	15, // SIGN_LANG
	0, // SIGN_KG
	1, // SIGN_LB
#ifdef USE_USA_UNIT_PRICE_DOT_DISP
	34, // SIGN_PER_G
	32, // SIGN_PER_LB
	33, // SIGN_PER_KG
#else	
	34, // SIGN_PER_KG
	32, // SIGN_PER_LB
	33, // SIGN_PER_G
#endif
	8, // SIGN_CONNECT
	12, // SIGN_CHECKIN
	13, // SIGN_ETH_LED_LINK
	4, // SIGN_USBMEM
	22, // SIGN_BAT1
	24, // SIGN_BAT2
	23, // SIGN_BAT3
	25, // SIGN_ANTENA
	26, // SIGN_ANTENA_LOW
	27, // SIGN_ANTENA_MID
	28, // SIGN_ANTENA_HIGH
#ifdef USE_CTS_FUNCTION
	2, // SIGN_CTS_ALARM
#endif
};

void display_temp_setting(void)
{
#ifdef _USE_LCD_7_25_  
	display_parameter.mode_pos[DISPLAY_WEIGHT] = 14;
	display_parameter.mode_len[DISPLAY_WEIGHT] = 5;
	display_parameter.mode_fix[DISPLAY_WEIGHT] = '.';
	display_parameter.mode_dec[DISPLAY_WEIGHT] = 3;

	display_parameter.mode_pos[DISPLAY_UNITPRICE] = 7;
	display_parameter.mode_len[DISPLAY_UNITPRICE] = 6;
	display_parameter.mode_fix[DISPLAY_UNITPRICE] = '.';
	display_parameter.mode_dec[DISPLAY_UNITPRICE] = 2;

	display_parameter.mode_pos[DISPLAY_PRICE] = 20;
	display_parameter.mode_len[DISPLAY_PRICE] = 7;
	display_parameter.mode_fix[DISPLAY_PRICE] = '.';
	display_parameter.mode_dec[DISPLAY_PRICE] = 2;

	display_parameter.mode_pos[DISPLAY_TARE] = 0;
	display_parameter.mode_len[DISPLAY_TARE] = 0;
	display_parameter.mode_fix[DISPLAY_TARE] = 0;
	display_parameter.mode_dec[DISPLAY_TARE] = 0;	
#elif defined (_USE_LCD_7_37D_40DOT_)
	display_parameter.mode_pos[DISPLAY_WEIGHT] = 5;
	display_parameter.mode_len[DISPLAY_WEIGHT] = 5;
	display_parameter.mode_fix[DISPLAY_WEIGHT] = 0;//'.';
	display_parameter.mode_dec[DISPLAY_WEIGHT] = 0;//3;

	display_parameter.mode_pos[DISPLAY_UNITPRICE] = 11;
	display_parameter.mode_len[DISPLAY_UNITPRICE] = 6;
	display_parameter.mode_fix[DISPLAY_UNITPRICE] = 0;//'.';
	display_parameter.mode_dec[DISPLAY_UNITPRICE] = 0;//2;

	display_parameter.mode_pos[DISPLAY_PRICE] = 18;
	display_parameter.mode_len[DISPLAY_PRICE] = 7;
	display_parameter.mode_fix[DISPLAY_PRICE] = 0;//'.';
	display_parameter.mode_dec[DISPLAY_PRICE] = 0;//2;

	display_parameter.mode_pos[DISPLAY_TARE] = 0;
	display_parameter.mode_len[DISPLAY_TARE] = 5;
	display_parameter.mode_fix[DISPLAY_TARE] = 0;
	display_parameter.mode_dec[DISPLAY_TARE] = 0;

	display_parameter.mode_pos[DISPLAY_PLU] = 25;
	display_parameter.mode_len[DISPLAY_PLU] = 12;
	display_parameter.mode_fix[DISPLAY_PLU] = 0;
	display_parameter.mode_dec[DISPLAY_PLU] = 0;
#elif defined (_USE_LCD_7_26d_16x96_)
	display_parameter.mode_pos[DISPLAY_WEIGHT] = 5;
	display_parameter.mode_len[DISPLAY_WEIGHT] = 6;
	display_parameter.mode_fix[DISPLAY_WEIGHT] = 0;//'.';
	display_parameter.mode_dec[DISPLAY_WEIGHT] = 0;//3;

	display_parameter.mode_pos[DISPLAY_UNITPRICE] = 11;
	display_parameter.mode_len[DISPLAY_UNITPRICE] = 7;
	display_parameter.mode_fix[DISPLAY_UNITPRICE] = 0;//'.';
	display_parameter.mode_dec[DISPLAY_UNITPRICE] = 0;//2;

	display_parameter.mode_pos[DISPLAY_PRICE] = 18;
	display_parameter.mode_len[DISPLAY_PRICE] = 8;
	display_parameter.mode_fix[DISPLAY_PRICE] = 0;//'.';
	display_parameter.mode_dec[DISPLAY_PRICE] = 0;//2;

	display_parameter.mode_pos[DISPLAY_TARE] = 0;
	display_parameter.mode_len[DISPLAY_TARE] = 5;
	display_parameter.mode_fix[DISPLAY_TARE] = 0;
	display_parameter.mode_dec[DISPLAY_TARE] = 0;

	display_parameter.mode_pos[DISPLAY_PLU] = 26;
	display_parameter.mode_len[DISPLAY_PLU] = 12;
	display_parameter.mode_fix[DISPLAY_PLU] = 0;
	display_parameter.mode_dec[DISPLAY_PLU] = 0;
#else
	display_parameter.mode_pos[DISPLAY_WEIGHT] = 0;
	display_parameter.mode_len[DISPLAY_WEIGHT] = 5;
	display_parameter.mode_fix[DISPLAY_WEIGHT] = '.';
	display_parameter.mode_dec[DISPLAY_WEIGHT] = 3;

	display_parameter.mode_pos[DISPLAY_UNITPRICE] = 8;
	display_parameter.mode_len[DISPLAY_UNITPRICE] = 6;
	display_parameter.mode_fix[DISPLAY_UNITPRICE] = '.';
	display_parameter.mode_dec[DISPLAY_UNITPRICE] = 2;

	display_parameter.mode_pos[DISPLAY_PRICE] = 17;
	display_parameter.mode_len[DISPLAY_PRICE] = 7;
	display_parameter.mode_fix[DISPLAY_PRICE] = '.';
	display_parameter.mode_dec[DISPLAY_PRICE] = 2;

	display_parameter.mode_pos[DISPLAY_TARE] = 0;
	display_parameter.mode_len[DISPLAY_TARE] = 0;
	display_parameter.mode_fix[DISPLAY_TARE] = 0;
	display_parameter.mode_dec[DISPLAY_TARE] = 0;
#endif
}

void display_init(void)
{
	INT8U decimal_sign;

	decimal_sign = get_global_bparam(GLOBAL_DECIMAL_SIGN);
	if ((decimal_sign != '.') && (decimal_sign != ',')) decimal_sign = '.';

    display_parameter.mode_pos[DISPLAY_WEIGHT] = get_global_bparam(GLOBAL_DISPLAY_WEIGHTPOS);
	display_parameter.mode_len[DISPLAY_WEIGHT] = get_global_bparam(GLOBAL_DISPLAY_WEIGHTLENGTH);
	display_parameter.mode_fix[DISPLAY_WEIGHT] = decimal_sign;
	display_parameter.mode_dec[DISPLAY_WEIGHT] = get_global_bparam(GLOBAL_DISPLAY_WEIGHTDECIMAL);


	display_parameter.mode_pos[DISPLAY_UNITPRICE] = get_global_bparam(GLOBAL_DISPLAY_UNITPRICEPOS);
	display_parameter.mode_len[DISPLAY_UNITPRICE] = get_global_bparam(GLOBAL_DISPLAY_UNITPRICELENGTH);
	display_parameter.mode_fix[DISPLAY_UNITPRICE] = decimal_sign;
	display_parameter.mode_dec[DISPLAY_UNITPRICE] = get_global_bparam(GLOBAL_AREA_PRICE_DECIMALPOINT);

	display_parameter.mode_pos[DISPLAY_PRICE] = get_global_bparam(GLOBAL_DISPLAY_PRICEPOS);
	display_parameter.mode_len[DISPLAY_PRICE] = get_global_bparam(GLOBAL_DISPLAY_PRICELENGTH);
	display_parameter.mode_fix[DISPLAY_PRICE] = decimal_sign;
	display_parameter.mode_dec[DISPLAY_PRICE] = get_global_bparam(GLOBAL_AREA_PRICE_DECIMALPOINT);

	display_parameter.mode_pos[DISPLAY_TARE] = get_global_bparam(GLOBAL_DISPLAY_TAREPOS);
	display_parameter.mode_len[DISPLAY_TARE] = get_global_bparam(GLOBAL_DISPLAY_TARELENGTH);
	display_parameter.mode_fix[DISPLAY_TARE] = decimal_sign;
	display_parameter.mode_dec[DISPLAY_TARE] = get_global_bparam(GLOBAL_DISPLAY_WEIGHTDECIMAL);
}

extern INT8U check_plu_weightunit(INT8U *num);
INT8U get_decimal_pt(INT8U mode, INT16U temp)	//0:cur, 1:use, 2:cal, 3:fix, 4:plu, 5:unitweight, 6:curpluweightunit
{
	INT8U uw, unit;
	INT16U sz;

	switch (mode) 
	{
		default:
		case 0: // Cur Weight Unit
			unit = ADM_GetData.CurUnit;
			break;
		case 1: // Use Weight Unit
			unit = ADM_Status.UseUnit;
			break;
		case 2: // Cal Weight Unit
			unit = ADM_Status.CalUnit;
			break;
		case 3: // Fix Weight Unit
			unit = (INT8U)temp;
			break;
		case 4: // PLU Weight Unit
			if (temp) 
			{
				plu_get_field(temp-1, PTYPE_UNIT_WEIGHT, (INT8U *)&uw, (INT16S *)&sz, sizeof(uw));
				unit = check_plu_weightunit(&uw);
				if(uw == 0xff) unit = ADM_GetData.CurUnit;
			} 
			else 
			{
				unit = ADM_GetData.CurUnit;
			}
			break;
		case 5: // Unit Weight Id
			uw = (INT8U)temp;
			unit = check_plu_weightunit(&uw);
			if (uw == 0xff) unit = ADM_GetData.CurUnit;
			break;
		case 6: // Cur PLU Weight Unit
			unit = CurPluWeightUnit;
			break;
	}
	unit = get_weight_decimal(ADM_Status.Capa, ADM_Status.Dual, unit);
	return unit;
}

void display_set_weightunit_change(INT8U dec)
{
	if (display_parameter.mode_dec[DISPLAY_WEIGHT] == dec) return;	// dec가 같을 때는 Display 업데이트 안함
	display_parameter.mode_dec[DISPLAY_WEIGHT] = dec;
	display_parameter.mode_dec[DISPLAY_TARE] = dec;
	sale_display_update(UPDATE_TARE|UPDATE_WEIGHT|UPDATE_PRICE);

}

void display_allon_primary_indication(void)
{
	VFD7_Fill_Buffer(0xff);
}

void display_alloff_primary_indication(void)
{
	VFD7_Fill_Buffer(0x00);
}

void display_use_allon_digit(INT8U ch)
{
	INT16S i;
	static INT8U allOn = ON;

	if (allOn) 
	{
		display_allon_primary_indication();
		allOn = OFF;
	} 
	else 
	{
		display_alloff_primary_indication();
		allOn = ON;
	}
	for (i = 0; i < DSP_MAX_DIGIT; i++)
	{
		VFD7_display_char(i, ch);
		if (allOn == OFF) 
		{
			VFD7_display_char(i, ',');
		}
	}
	VFD7_Diffuse();
}

void display_tare(INT8U mode)
{
	char data[16];

	if (mode == 0) 
	{
		if (status_run.run_mode == RUN_SALE) 
		{
			memset(data, 0x20, display_parameter.mode_len[mode]);
			data[display_parameter.mode_len[mode]] = 0;
			display_string(mode, (INT8U *)data);
		}
	} 
	display_tare_off_flag = mode;
}

void display_ulongsimple(INT8U mode, long idata)
{
	if (display_parameter.mode_len[mode] == 0) return;
	display_ulong(mode,idata,display_parameter.mode_dec[mode]);
}

void display_ulong(INT8U mode, long idata, INT8U decimalpoint)
{
	char sdata[16];
	INT16S  sign;
	INT16S  i;
	INT16S  pos;
	INT16S  temp_pos;
	long disp_data; 
	INT8U	temp;
	// 1000단위 구분자 사용
	INT8U decpt_cnt;
	INT8U segmentChar;
	INT32U max, temp32;
	INT8U useSeperator;
	INT8U max_cnt;

	useSeperator = get_global_bparam(GLOBAL_PRICESIGN_1000);
	useSeperator &= 0x01;
	if (useSeperator)	// 1000단위 구분자 사용
	{
 #if defined(COUNTRY_INDONESIA) || defined(USE_INDONESIA_EMPORIUM)
		segmentChar = '.';
 #else
		segmentChar = get_global_bparam(GLOBAL_SALE_SETUP_FLAG20) & 0x01;	//parameter 747

	 	if(segmentChar == 1)
			segmentChar = '.';
		else
			segmentChar = ',';
 #endif
	decpt_cnt = 0;
	}
	
	sign = 0;
	if ((display_tare_off_flag == 0) && (mode == DISPLAY_TARE)) return;

	if (idata < 0) 
	{ 
		sign = 1;
		idata = -idata; 
	}
	disp_data = idata;
	pos = display_parameter.mode_len[mode];
	if (pos == 0) return;

	char total_size;

	if (decimalpoint) 
	{
		pos++;
	}

	if (useSeperator) {
		INT32S digit; /* 소수점을 제외한 자릿수 */
		INT32S separator_count; /* separator 총 개수 */

		digit = display_parameter.mode_len[mode] - decimalpoint;
		separator_count = (digit - 1) / 3;

		if (separator_count < 0) {
			/* 현재는 마이너스가 될 가능성은 없다 */
			separator_count = 0;
		}

		pos += separator_count;
	}
	total_size = pos;

	memset((INT8U *)sdata, 0x20, pos);
	sdata[pos] = 0;
	pos--;
	temp_pos = pos;

	for (i = 0; i < decimalpoint; i++) 
	{
		if (pos < 0) goto ERR;
		sdata[pos] = (idata % 10) + '0';
		idata = idata / 10;
		pos--;
	}

	if (decimalpoint) 
	{
		if (pos < 0) goto ERR;
		sdata[pos] = display_parameter.mode_fix[mode];
		pos--;
	}
	
	if(useSeperator)	// 1000단위 구분자 사용
		max_cnt = 16;
	else
		max_cnt = 10;
	
	for (i=decimalpoint; i<max_cnt; i++)
	{
		if (pos<0) goto ERR;
		
		if (useSeperator) {
			if (decpt_cnt && (decpt_cnt % 3 == 0)) {
				sdata[pos] = segmentChar;
				pos--;
			}
			decpt_cnt++;
		}
		
		sdata[pos]=(idata%10)+'0';
		idata     = idata/10;
		if (idata==0) break;
		pos--;
	}

	if (sign) 
	{
		if (pos > 0) sdata[--pos] = '-';
		else goto ERR;
	}
	if (!status_scale.display_dummydigit)
	{
		if((mode == DISPLAY_WEIGHT || mode == DISPLAY_TARE) && decimalpoint > 0 && status_run.run_mode == RUN_SALE) 
		{
			if(ADM_Status.Dual == 1 && ad_get_max_capa(ADM_Status.Capa, CurPluWeightUnit, 0) == 30000) 	// 30kg, 30lb
			{
				if(disp_data >= 15000) 
				{
					sdata[temp_pos] = ' ';
				}
			}
			//SG070419
			temp = get_global_bparam(GLOBAL_WEIGHT_ROUND);
			temp = (temp >> 4) & 0x03;
			for(i=0; i<temp; i++)
			{
				if(i>decimalpoint) break;
				sdata[temp_pos-i] = ' ';		
			}
		}
	}
	
	////////////////////////
	VFD7_display_string((HUGEDATA char *)sdata, display_parameter.mode_pos[mode], total_size, mode, TRUE);

	return;
ERR:
	BuzOnAdd(1);
}

INT8U KeyFlagForDispRst = TRUE;//HYP 20060703
void display_primary_diffuse(void) // VFD7_Diffuse() 대체
{
#if defined (_USE_LCD20848_) || defined (_USE_LCD32_)
	unsigned short i,j;
  #ifdef USE_CL5200_DISP_CONTINUOUS_REFRESH
	int check_refresh = 0;
  #endif

/////////////////////////////////HYP 20050407 for electro-static-discharge & Burst
	static INT16U DspTimer100ms=0;//HYP 20050411

	if(KeyFlagForDispRst == TRUE) {//HYP 20060703 Reset when key is pressed
		DspTimer100ms = SysTimer100ms;
		KeyFlagForDispRst = FALSE;
#if defined (_USE_LCD20848_)
		LCD_Reset();
		LCD_Write_send(LCD_CMD,0x3f,LCD_C123); //display on
#endif		
	} else {
  #ifndef USE_CL5200_DISP_CONTINUOUS_REFRESH
		if(10 < (INT16U)(SysTimer100ms - DspTimer100ms))		//1000 ms Refresh
		{
			DspTimer100ms = SysTimer100ms;
  #else
		if(Disp_refresh_flag == TRUE)							//continous refresh parameter ON
		{
			if( 1 < (INT16U)(SysTimer100ms - DspTimer100ms) )	//continous refresh ( frequence: 100ms )
			{
				//DspTimer100ms = SysTimer100ms;
				check_refresh = 1;
			}
		}
		else													//continous refresh parameter OFF
		{
			if(10 < (INT16U)(SysTimer100ms - DspTimer100ms)) 	// 1000ms refresh
			{//1000 ms Reflash
				//DspTimer100ms = SysTimer100ms;
				check_refresh = 1;
			}
		}	
  #endif         
		if(check_refresh)
		{
			check_refresh = OFF;
			//HYP 20060703
#if defined (_USE_LCD20848_)
			LCD_Reset();  //test 100119
			LCD_Write_send(LCD_CMD,0xc0,LCD_C123); //display start line 0
			LCD_Write_send(LCD_CMD,0xb8,LCD_C123); //SET PAGE ADDRESS 0
			LCD_Write_send(LCD_CMD,0x40,LCD_C123); //SET COLUMN ADDRESS 0
			LCD_Write_send(LCD_CMD,0x3f,LCD_C123); //display on
#endif
		}
	}
  #ifndef USE_CL5200_DISP_CONTINUOUS_REFRESH
	if(10 < (INT16U)(SysTimer100ms - DspTimer100ms))		//1000 ms Refresh
	{
		DspTimer100ms = SysTimer100ms;
  #else
    #ifdef USE_CERT_DISP_CONTINUOUS_REFRESH
		if(Disp_refresh_flag == TRUE)							//continous refresh parameter ON
		{
			check_refresh = 1;
		}
	#else
		if(Disp_refresh_flag == TRUE)							//continous refresh parameter ON
		{
			if( 1 < (INT16U)(SysTimer100ms - DspTimer100ms) )	//continous refresh ( frequence: 100ms )
			{
				DspTimer100ms = SysTimer100ms;
				check_refresh = 1;
			}
		}
		else													//continous refresh parameter OFF
		{
			if(10 < (INT16U)(SysTimer100ms - DspTimer100ms)) 	// 1000ms refresh
			{//1000 ms Reflash
				DspTimer100ms = SysTimer100ms;
				check_refresh = 1;
			}
		}
    #endif  // USE_CERT_DISP_CONTINUOUS_REFRESH	
		if(check_refresh)
		{
			check_refresh = OFF;
  #endif  // USE_CL5200_DISP_CONTINUOUS_REFRESH
#ifdef _USE_LCD20848_
		for(i=0;i<6;i++)
		{
			LCD_Write_send(LCD_CMD,0xb8+i,LCD_C1);
			LCD_Write_send(LCD_CMD,0x40,LCD_C1);

			for(j=0;j<64;j++) {
				LCD_Write_send(LCD_DAT,DspBuf[DspStruct.Page][i+6*j],LCD_C1);
			}
		}
		for(i=0;i<6;i++)
		{
			LCD_Write_send(LCD_CMD,0xb8+i,LCD_C2);
			LCD_Write_send(LCD_CMD,0x40,LCD_C2);

			for(j=0;j<64;j++) {
				LCD_Write_send(LCD_DAT,DspBuf[DspStruct.Page][i+6*(j+64)],LCD_C2);
			}
		}
		for(i=0;i<6;i++)
		{
			LCD_Write_send(LCD_CMD,0xb8+i,LCD_C3);
			LCD_Write_send(LCD_CMD,0x40,LCD_C3);

			for(j=0;j<64;j++) {
				LCD_Write_send(LCD_DAT,DspBuf[DspStruct.Page][i+6*(j+128)],LCD_C3);
			}
		}
		for(i=0;i<6;i++)
		{
			LCD_Write_send(LCD_CMD,0xb8+i,LCD_C4);
			LCD_Write_send(LCD_CMD,0x40,LCD_C4);

			for(j=0;j<(LCD_Y_MARGIN_REAL-192);j++) {
				LCD_Write_send(LCD_DAT,DspBuf[DspStruct.Page][i+6*(j+192)],LCD_C4);
			}
		}	
	}
#elif defined (_USE_LCD_7_24_)  || defined (_USE_LCD_7_25_)
	}
	VFD7_Diffuse();
#endif	
#else//#if defined (_USE_LCD20848_) || defined (_USE_LCD32_)
	VFD7_Diffuse();
#endif//#if defined (_USE_LCD20848_) || defined (_USE_LCD32_)
}

void init_lcdchess(void)
{
	static INT8U cc = 0xff;

	LCD_Check(cc, ~cc);
	cc ^= 0xff;
}

void display_clear(INT8U mode)
{
	char data[10];

	memset((INT8U *)data, 0x20, 10);
	data[9] = 0;

	VFD7_display_string((HUGEDATA char *)data, display_parameter.mode_pos[mode], (char)display_parameter.mode_len[mode], mode, TRUE);
}


void display_lcd_clear(void)
{
	Dsp_Fill_Buffer(0);
	Dsp_Diffuse();
}

void display_lcd_clear_buf(void)
{
	Dsp_Fill_Buffer(0);
}

void display_lcd_diffuse(void)
{
	Dsp_Diffuse();
}

void display_string(INT8U mode, INT8U *data)
{
	INT16S l;

	l = strlen((char *)data);
	if (l > display_parameter.mode_len[mode]) l = display_parameter.mode_len[mode];
	if (display_parameter.mode_len[mode] <= 0) return;
#if defined(_USE_LCD37D_40DOT_) || defined(_USE_LCD26d_16x96_)
	if (display_parameter.mode_len[mode] > 12) return; //plu digit = 12
#else
	if (display_parameter.mode_len[mode] > 10) return;
#endif
	// 저울 디스플레이 문구는 FALSE로 기존처럼 앞에서부터 출력
	VFD7_display_string((HUGEDATA char *)data, display_parameter.mode_pos[mode], (char)l, mode, FALSE);
}

void display_string_pos(INT8U pos, INT8U *data)
{
	INT16S len;

	len  = strlen((char*)data);
	if (len > DSP_MAX_DIGIT - pos) len = DSP_MAX_DIGIT - pos;
	if (len <= 0) return;
	if (pos > DSP_MAX_DIGIT) return;

	VFD7_display_string((char*)data, pos, len, DISPLAY_UNDEFINED, FALSE);
}

void display_string_pos_clear(INT8U pos, INT16S len)
{
	char data[DSP_MAX_DIGIT+1];

	if (len > DSP_MAX_DIGIT - pos) len = DSP_MAX_DIGIT - pos;
	if (len <= 0) return;
	if (pos > DSP_MAX_DIGIT) return;

	memset((INT8U *)data, 0x20, len);
	data[len] = 0;

	VFD7_display_string(data, pos, len, DISPLAY_UNDEFINED, FALSE);
}

void display_sign_noflush(INT8U sign, INT8U onoff)
{
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
	INT32U pnt;
	INT8U  pos;

	if (sign >= GLOBAL_DISPLAY_INDICATOR_NO) return;

	pnt = DFLASH_BASE;
	pnt += FLASH_GLOBAL_AREA;
	pnt += GLOBAL_DISPLAY_INDICATORSIGN;
	pnt += sign;
	pos = Flash_bread(pnt);
	if (pos == 0xff) return;
#else	
	INT8U  pos;

	if (sign >= DSP_MAX_SYMBOL) return;

	pos = signTableIndex[sign];
#endif
 #if defined (_USE_LCD_7_25_)
	VFD7_display_triangle(sign, onoff);
 #else
	VFD7_display_triangle(pos, onoff);
 #endif
}
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
void display_set_sign(INT8U sign,INT8U pos)
{
	INT32U pnt;

	if (sign >= GLOBAL_DISPLAY_INDICATOR_NO) return;

	pnt = DFLASH_BASE;
	pnt += FLASH_GLOBAL_AREA;
	pnt += GLOBAL_DISPLAY_INDICATORSIGN;
	pnt += sign;
	Flash_bwrite(pnt, pos);
}
#endif
char display_font_get_width(void)
{
#if defined(_USE_LCD37D_40DOT_)
	return LCD_CHAR_FONT_WIDTH;
#else
	char wbits;

	wbits = DspStruct.Wbits1;
	if (DspStruct.PreRot1 == PREROT_ONLY_90DEG)
	{
		wbits = DspStruct.Hbits1;
	}
	return (DspStruct.InterChSp + wbits);
#endif
}

char display_font_get_height(void)
{
#if defined(_USE_LCD37D_40DOT_)
	return LCD_CHAR_FONT_HEIGHT;
#else
	char hbits;

	hbits = DspStruct.Hbits1;
	if (DspStruct.PreRot1 == PREROT_ONLY_90DEG)
	{
		hbits = DspStruct.Wbits1;
	}
	return hbits;
#endif
}

char display_font_get_width2(void)
{
	return (DspStruct.InterChSp + DspStruct.Hbits2);
}

char display_font_get_height2(void)
{
	return DspStruct.Wbits2;
}

extern HUGEDATA DSP_FONT1_STRUCT DspFont1Struct[MAX_DSP_FONT1_NUM];
extern HUGEDATA DSP_FONT2_STRUCT DspFont2Struct[MAX_DSP_FONT2_NUM];

char display_font_get_width_id(INT8U fontid, INT8U mag)
{
	return (DspFont1Struct[fontid].InterChSp + DspFont1Struct[fontid].Hbits1);
}

char display_font_get_height_id(INT8U fontid, INT8U mag)
{
	return DspFont1Struct[fontid].Wbits1;
}

void display_save_restore(INT8U mode)
{
	VFD7_save_restore(mode);
}

INT16S display_date_check(INT16S mode)
{
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
	if (mode == DISPLAY_DATE_DEPT) 
	{
		if (Startup.disable_dept) mode = DISPLAY_DATE_PLUNO;
	}
	if(Operation.operationClerk != OPER_NORMAL)
	{
		if (mode >= DISPLAY_DATE_ITEM) mode = DISPLAY_DATE_CLEAR;
	}
	else
	{
		if (mode >= DISPLAY_DATE_CLERK)	mode = DISPLAY_DATE_CLEAR;
	}
#else
	if (mode == DISPLAY_DATE_DEPT) 
	{
		if (Startup.disable_dept) mode++;
	}
	if (mode == DISPLAY_DATE_CLERK) 
	{
		if (!(status_scale.restrict & FUNC_CLERK)) mode++;
		else if (Operation.operationClerk == OPER_NORMAL) mode++;
	}
	if (mode == DISPLAY_DATE_OPENCLERK) 
	{
		if (!(status_scale.restrict & FUNC_CLERK)) mode++;
		else if (Operation.operationClerk == OPER_NORMAL) mode++;
	}
#endif
	return mode;
}


extern INT8U	DisplayModePrev;
void display_date_change(INT16S mode, INT16S saveFlag)
{
	if(!saveFlag) 
	{
		if(DisplayModePrev)
		{
			mode = DisplayModePrev;
			DisplayModePrev = 0;
		}
		else
		{
			mode = get_nvram_bparam(NVRAM_DISPLAY_DATETIME_MODE);
		}
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
		mode++;		
		if (mode == DISPLAY_DATE_CLERK) 
		{
			if (!(status_scale.restrict & FUNC_CLERK)) mode++;
		}
		if (mode == DISPLAY_DATE_DEPT) 
		{
			if (Startup.disable_dept) mode++;
		}
#else
		if (DateTimeDispTime) mode++; // 표시되고 있을 때만 모드가 변환 되도록.
#endif
	}
	mode = display_date_check(mode);
	mode %= DISPLAY_DATE_MAX;
	if(mode != get_nvram_bparam(NVRAM_DISPLAY_DATETIME_MODE))
	{
		set_nvram_bparam(NVRAM_DISPLAY_DATETIME_MODE, mode);
	}
	sale_display_date(ON);
}

extern INT16S PutNSmallString(INT16S y, INT16S x, char *data, INT16S n);
INT16U MsgDisplayTime = 0;
INT16U DateTimeDispTime = 0;
INT8U  MsgShort = 0;
INT8S  MsgDisplayBuf[34];
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
INT16U MsgPLUDisplayTime = 0;	//Added by JJANG 20080220
INT16U MsgRDisplayTime = 0;
INT8U  MsgWeight = 0;		//Added by JJANG 20080401
INT8S  MsgDisplayRightBuf[34];
#else
INT16U Msg7SegDispTime = 0;
#endif
void ClearDisp7(void)
{
	display_clear(DISPLAY_TARE);
	display_clear(DISPLAY_WEIGHT);
	display_clear(DISPLAY_UNITPRICE);
	display_clear(DISPLAY_PRICE);
	VFD7_Diffuse();
}

extern INT8U  FlagDisplayIndividual;
INT16S DisplayMsg(char *msg)
{

	MsgShort = OFF;
	MsgDisplayTime = SysTimer100ms;
	PutString(DSP_MSG_Y, DSP_MSG_X, msg, DSP_MSG_FONT_ID, DSP_MSG_MAGY, DSP_MSG_MAGX, MAX_MSG_CHAR);
	strcpy((char*)MsgDisplayBuf, msg);
	return 0;
}
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
INT16S DisplayMsgRight(char *msg)
{
	INT8U old_page;

	MsgRDisplayTime = SysTimer100ms;
	old_page = DspStruct.Page;
	Dsp_SetPage(DSP_ERROR_PAGE);
	PutString(DSP_MSG_R_Y, DSP_MSG_R_X, msg, DSP_MSG_R_FONT_ID, DSP_MSG_R_MAGY, DSP_MSG_R_MAGX, MAX_MSG_R_CHAR);
	Dsp_SetPage(old_page);
	strcpy((char*)MsgDisplayRightBuf, msg);
	return 0;
}
#endif
INT16S DisplayMsgShort(char *msg)
{
	MsgShort = ON;
	MsgDisplayTime = SysTimer100ms;
	PutString(DSP_MSG_Y, DSP_MSG_X, msg, DSP_MSG_FONT_ID, DSP_MSG_MAGY, DSP_MSG_MAGX, MAX_MSG_CHAR);
	strcpy((char*)MsgDisplayBuf, msg);
	return 0;
}

//Added by JJANG 20080220	PARAMETER 613
#define MAX_M_MSG_CHAR 25
extern INT16S PutNMediumString(INT16S y, INT16S x,char *data, INT16S n);
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
INT16S DisplayMsgTransaction(char *msg)
{
	INT8U old_page;
	MsgPLUDisplayTime = SysTimer100ms;
	old_page = DspStruct.Page;	
	Dsp_SetPage(DSP_ERROR_PAGE);
	Dsp_Fill_Buffer(0);
	PutNMediumString(1, 0, msg, MAX_M_MSG_CHAR);
	Dsp_Diffuse();							 
	Dsp_SetPage(old_page);
	return 0;
}
#endif
//Added by JJANG 20080401
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
INT16S DisplayWeightMsg(char *msg)
{
	INT8U old_page;

	MsgWeight = ON;
	MsgDisplayTime = SysTimer100ms;
	old_page = DspStruct.Page;	
	Dsp_SetPage(DSP_ERROR_PAGE);
	PutString(DSP_MSG_Y, DSP_MSG_X, msg, DSP_MSG_FONT_ID, DSP_MSG_MAGY, DSP_MSG_MAGX, MAX_MSG_CHAR_LONG);
	Dsp_SetPage(old_page);
	return 0;
}
#endif
void DisplayMsgReset(void)
{
	MsgDisplayTime = 0;
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
	MsgPLUDisplayTime= 0;	//Added by JJANG 20080220
	MsgRDisplayTime = 0;
	MsgWeight = 0;		//Modified by JJANG 20080401
	sale_display_update(UPDATE_TITLE|UPDATE_TITLE2|UPDATE_RIDE);
#else
	Msg7SegDispTime = 0;
	sale_display_update(UPDATE_ALL);
#endif
}
