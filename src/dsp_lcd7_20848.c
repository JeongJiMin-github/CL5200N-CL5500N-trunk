/**
* @file dsp_lcd7_25d.c
* @attention (c) 2008 CAS
* @version 0.1
* @author 홍의표
* @date 2008/10/31
* @brief lcd(2line,25digit,double body용)에 디스플레이 하기 위한 모듈
*/

#include "globals.h"
#include "dsp_app.h"
#include "prt_font.h"
#include "dsp_common.h"
#include "mode_main.h"
#include "dsp_lcd20848.h"
#include "initial.h"
#include <string.h>

#ifdef _USE_LCD_7_20848_

#define X_COMMA_WIDTH	3
#define X_CHAR_WIDTH	8
#ifdef  USE_DISPLAY_5200_7DIGIT
#define X_BAR_WIDTH		4
#else
#define X_BAR_WIDTH		6
#endif
#define DSP_MAX_DIGIT   26

#if defined (USE_DISPLAY_5200_7DIGIT)
  #ifdef USE_PRICE_TOTAL_8DIGIT
    //#define DISPLAY_CHAR_START_POSITION_WEIGHT				(1+37)
    //#define DISPLAY_CHAR_START_POSITION_UNITPRICE			(1+37+45)
    //#define DISPLAY_CHAR_START_POSITION_TOTALPRICE			(1+37+45+64)
    //#define DISPLAY_CHAR_START_POSITION_TARE				(1)
    #define DISPLAY_CHAR_START_POSITION_WEIGHT				(1+39)
    #define DISPLAY_CHAR_START_POSITION_UNITPRICE			(1+39+45)
    #define DISPLAY_CHAR_START_POSITION_TOTALPRICE			(1+37+45+64)
    #define DISPLAY_CHAR_START_POSITION_TARE				(1)
  #else
    //#define DISPLAY_CHAR_START_POSITION_WEIGHT				(1+37)
    //#define DISPLAY_CHAR_START_POSITION_UNITPRICE			(1+37+45)
    //#define DISPLAY_CHAR_START_POSITION_TOTALPRICE			(1+37+45+64)
    //#define DISPLAY_CHAR_START_POSITION_TARE				(1)
	// unit_price : 7 / total_price : 7
    #define DISPLAY_CHAR_START_POSITION_WEIGHT				(1+39)
    #define DISPLAY_CHAR_START_POSITION_UNITPRICE			(1+39+45)
    #define DISPLAY_CHAR_START_POSITION_TOTALPRICE			(1+39+45+64)
    #define DISPLAY_CHAR_START_POSITION_TARE				(1)
  #endif
#else
  #ifdef USE_DISPLAY_STYLE_3DIGIT			//Weight(0)		Unit(5)		Price(14)
    #if defined(USE_PRICE_UNIT_9DIGIT) && defined (USE_PRICE_TOTAL_9DIGIT)
      #define DISPLAY_CHAR_START_POSITION_WEIGHT				(5)
      #define DISPLAY_CHAR_START_POSITION_UNITPRICE				(5+49)
      #define DISPLAY_CHAR_START_POSITION_TOTALPRICE			(5+49+79)
	#else
      #define DISPLAY_CHAR_START_POSITION_WEIGHT				(1)
      #define DISPLAY_CHAR_START_POSITION_UNITPRICE				(1+57)
      #define DISPLAY_CHAR_START_POSITION_TOTALPRICE			(1+57+81)
	#endif
  #else
    #define DISPLAY_CHAR_START_POSITION_WEIGHT				(1+41)
    #define DISPLAY_CHAR_START_POSITION_UNITPRICE			(1+41+49)
    #define DISPLAY_CHAR_START_POSITION_TOTALPRICE			(1+41+49+57)
    #define DISPLAY_CHAR_START_POSITION_TARE				(1)
  #endif
  #if !defined(USE_PRICE_UNIT_7DIGIT) && defined(USE_PRICE_TOTAL_7DIGIT) // Unit Price 6 , Total Price 7
    #define DISPLAY_CHAR_START_POSITION_WEIGHT				(1+40)
    #define DISPLAY_CHAR_START_POSITION_UNITPRICE			(1+40+48)
    #define DISPLAY_CHAR_START_POSITION_TOTALPRICE			(1+40+48+56)
    #define DISPLAY_CHAR_START_POSITION_TARE				(1)
  #endif
#endif

struct bar_position {
	INT16S weight;
	INT16S unit_price;
	INT16S total_price;
	INT16S tare;
};

struct bar_position_info {
	struct bar_position start;
	struct bar_position end;
};

static const struct bar_position_info bar_pos_info = {
	.start = {
		.weight 		= DISPLAY_CHAR_START_POSITION_WEIGHT - 4,
		.unit_price 	= DISPLAY_CHAR_START_POSITION_UNITPRICE - 4,
		.total_price 	= DISPLAY_CHAR_START_POSITION_TOTALPRICE - 4,
		.tare 			= 0,
	},
	.end = {
		.weight 		= DISPLAY_CHAR_START_POSITION_UNITPRICE - 4,
		.unit_price 	= DISPLAY_CHAR_START_POSITION_TOTALPRICE - 4,
		.total_price	= 208,
		.tare 			= DISPLAY_CHAR_START_POSITION_WEIGHT - 4,
	},
};


/////////////////////////////////////////////////

#define DSP_STR_BUF_SIZE	16

#define DSP_MAX_DIGIT   26
#define DSP_MAX_CHAR	12
#define PRICE_DIGIT     7

#define WEIGHT_DIGIT          5
#define UNIT_DIGIT			6
#define PT_DIGIT              4

#define PT_DISPLAY_START      1//0//4
#define WEIGHT_DISPLAY_START  13//5//5
#define UNIT_DISPLAY_START    7//11//6

#define DIFFUSE_ALL		0
#define DIFFUSE_7SEG	1
#define DIFFUSE_12CHAR	2

#define PRICE_DISPLAY_START   19//17//7
#define PMSG_DIGIT		12
#define PMSG_DISPLAY_START   24//12

#define DOT_TYPE_COMMA	0x80

INT8U conPortStatus = 0;
INT8U datPortStatus = 0;

INT8U DSP_Memory[DSP_MAX_DIGIT*2+DSP_MAX_CHAR*5];
INT8U DSP_Memory_blink[DSP_MAX_DIGIT*2+DSP_MAX_CHAR*5];
INT8U DSP_Memory_capture[DSP_MAX_DIGIT*2+DSP_MAX_CHAR*5];

INT8U	DspPtrFont1;

INT8U capture_flag;
INT8U blink_flag;
INT8U blink_spos;
INT8U blink_epos;
INT8U *DSP_buf_ptr;
INT8U   UseSaleModeEditFlag=0;


#define DSP_ON	1
#define DSP_OFF	0

#define WMSG_DIGIT			UNIT_DIGIT
#define WMSG_DISPLAY_START	UNIT_DISPLAY_START
#define UMSG_DIGIT			PRICE_DIGIT
#define UMSG_DISPLAY_START	PRICE_DISPLAY_START


const char typeDp1[13][6] = {"%0ld", "%1ld", "%2ld", "%3ld", "%4ld", "%5ld", "%6ld", "%7ld", "%8ld", "%9ld", "%10ld", "%11ld", "%12ld"};
const char typeDp2[13][7] = {"%00ld", "%01ld", "%02ld", "%03ld", "%04ld", "%05ld", "%06ld", "%07ld", "%08ld", "%09ld", "%010ld", "%011ld", "%012ld"};
const char typeDpHex[13][6] = {"%00x", "%01x", "%02x", "%03x", "%04x", "%05x", "%06x", "%07x", "%08x", "%09x", "%10x", "%11x", "%12x"};

#define __set_display_start_end_bar_pos_by_mode(mode, start_pos, end_pos) \
	do { \
		switch (mode) { \
			case DISPLAY_WEIGHT: \
				start_pos 	= bar_pos_info.start.weight; \
				end_pos     = bar_pos_info.end.weight; \
				break; \
			case DISPLAY_UNITPRICE: \
				start_pos 	= bar_pos_info.start.unit_price; \
				end_pos     = bar_pos_info.end.unit_price; \
				break; \
			case DISPLAY_PRICE: \
				start_pos 	= bar_pos_info.start.total_price; \
				end_pos     = bar_pos_info.end.total_price; \
				break; \
			case DISPLAY_TARE: \
				start_pos 	= bar_pos_info.start.tare; \
				end_pos     = bar_pos_info.end.tare; \
				break; \
			case DISPLAY_UNDEFINED: \
			default: \
				return; \
		} \
	} while(0)

#define __display_ch_based_y_zero(ch, x_pos, width) { \
	POINT disp_p; \
	disp_p = point(0, x_pos); \
	Dsp_Write_1Char_7seg(disp_p, ch, width); \
}

#define __display_bar(end_pos) \
	__display_ch_based_y_zero('|', end_pos - 2, 3) /* 각 자리(Total Price) 시작 위치에 앞 구역과 구분하는 구분자 삽입 | */

#define __display_reverse_check_boundary_calc(x_pos, start_pos, width) \
	(x_pos - width <= start_pos)

#define __display_reverse_x_pos_update(x_pos, width) \
	x_pos -= width; \

#define __display_reverse_ch_x_pos_update(x_pos, start_pos, ch, width) {\
	__display_reverse_x_pos_update(x_pos, width); \
	__display_ch_based_y_zero(ch, x_pos, width); \
}

#define __display_reverse_fill_bar_remain_area(x_pos, start_pos) { \
	while(--x_pos > start_pos) { \
		__display_ch_based_y_zero(0x20, x_pos, 1); \
	} \
}

static inline INT8U __check_if_boundary_is_over
(char *dataString, char size, INT16S start_pos, INT16S end_pos, INT16S x_pos)
{
	while (size) {
		char ch = dataString[--size];

		if (ch == '.' || ch == ',') {
			if (__display_reverse_check_boundary_calc(x_pos, start_pos, X_COMMA_WIDTH)) {
				return TRUE;
			}
			__display_reverse_x_pos_update(x_pos, X_COMMA_WIDTH);
		} else if (ch == 0x20) {
			break;
		} else {
			if (__display_reverse_check_boundary_calc(x_pos, start_pos, X_CHAR_WIDTH)) {
				return TRUE;
			}
			__display_reverse_x_pos_update(x_pos, X_CHAR_WIDTH);
		}
	}
	return FALSE;
}

static inline void __display_reverse(char *dataString, char size, INT16S start_pos, INT16S end_pos)
{
#if defined (USE_DISPLAY_5200_7DIGIT)
	/* 디스플레이 표시 공간이 부족하여 end bar position에 한 칸 가깝게 표시하기 위함 */
	INT16S x_pos = end_pos;
#else
	INT16S x_pos = end_pos - 1;
#endif

	if (__check_if_boundary_is_over(dataString, size, start_pos, end_pos, x_pos)) {
		memset(dataString, '-', size); \
		dataString[size] = 0; \
	}

	// strlen 방식으로 변경 필요??
	while (size) {
		char ch = dataString[--size];
		
		if (ch == '.' || ch == ',') {
			if (__display_reverse_check_boundary_calc(x_pos, start_pos, X_COMMA_WIDTH))
				break;
			__display_reverse_ch_x_pos_update(x_pos, start_pos, ch, X_COMMA_WIDTH);
		} else {
			if (__display_reverse_check_boundary_calc(x_pos, start_pos, X_CHAR_WIDTH))
				break;
			__display_reverse_ch_x_pos_update(x_pos, start_pos, ch, X_CHAR_WIDTH);
		}
	}

	__display_reverse_fill_bar_remain_area(x_pos, start_pos);
}

static inline void __display_string_reverse(char *dataString, char size, INT8U mode)
{
	INT16S start_pos, end_pos;

	__set_display_start_end_bar_pos_by_mode(mode, start_pos, end_pos);
	__display_bar(end_pos);
	__display_reverse(dataString, size, start_pos, end_pos);
}

static inline void __display_string(char *dataString, char size, INT16S x)
{
	INT8U i;
	POINT disp_p;
	INT8U dot_flag;

	i=0;

	dot_flag = 0;
	while((*(dataString) != '\0') && (i<size))
	//while((*(dataString) != '\0'))
	{
		disp_p = point(0,x);

		if(*dataString == '.' || *dataString == ',')
		{
			Dsp_Write_1Char_7seg(disp_p, *dataString, X_COMMA_WIDTH);
			x = x + X_COMMA_WIDTH;
			dot_flag = 1;
			size++;
		}
		else
		{
			Dsp_Write_1Char_7seg(disp_p, *dataString, X_CHAR_WIDTH);
			x = x + X_CHAR_WIDTH;
		}

		dataString++;
		i++;
	}
	if (dot_flag == 0)
	{
		disp_p = point(0,x);
		Dsp_Write_1Char_7seg(disp_p, 0x20, X_COMMA_WIDTH);		
	}
}

INT16U DSP7_DATA;

void DISPLAY_DATA(unsigned char data)
{
	INT16U DSP7_TEMP_DATA = 0;       
	
        DSP7_TEMP_DATA = data;
        DSP7_DATA = (DSP7_TEMP_DATA<<8)| (DSP7_DATA & 0x00FF);
		Delay_10();
}

void DISPLAY_CON(unsigned char data)
{
	INT16U DSP7_TEMP_DATA = 0;		 
		
	DSP7_TEMP_DATA = (data|0x20);
	DSP7_DATA = (DSP7_TEMP_DATA)| (DSP7_DATA & 0xFF00);
	Delay_10();
}

void DSP_LCD7_diffuse(void)
{
//	ht162XWriteAllData(DIFFUSE_ALL);
//	DSP_LCD7_diffuse_by_type(DIFFUSE_ALL);
}
void DSP_clear_all(void)
{
	memset(DSP_Memory, 0x00, sizeof(DSP_Memory));
	DSP_LCD7_diffuse();
}

void VFD7_write_byte(char Data)
{

}        

void VFD7_write_cmd(char Data, INT8U stb)	// Local Command
{     
	
} 
 

void VFD7_display_On(void)
{
	
} 

void VFD7_display_Off(void)
{
	
}

void VFD7_display_bright(INT8U bright)//1~4 level
{

}


void VFD7_Init(void)
{
	LCD_Init();
}

void VFD7_display_char(INT8U nDigitPos, char c)
{
	POINT disp_p;
	
	if (c == ',') return;
	
	if((status_run.run_mode == RUN_SALE)&&(UseSaleModeEditFlag==0))
        {
		DspLoadFont1(9);
	} else {
		DspLoadFont1(8);
	}
	
	disp_p = point(0,nDigitPos*X_CHAR_WIDTH);
	Dsp_Write_1Char_7seg(disp_p,c,X_CHAR_WIDTH);
	
	DspLoadFont1(DSP_MENU_FONT_ID);	

}

void VFD7_display_ucomma(INT8U nDigitPos, INT8U onoff)
{
	
}

void DSP_set_triangle(INT8U pos, INT8U OnOff)
{
	LCD_BufferWriteTriangle(pos, OnOff);
}

void VFD7_display_triangle(INT8U nDigitPos, INT8U onoff)
{
	LCD_BufferWriteTriangle(nDigitPos,onoff);
}

void VFD7_display_string(HUGEDATA char *dataString, INT8U nDigitPos, char size, INT8U mode, BOOLEAN reverse)
{
	INT16S	x;
	INT8U backup_directdraw;
	INT8U useSeperator;
	INT8U bar_width;
	unsigned char d_position[4];

	backup_directdraw = DspStruct.DirectDraw;
#ifdef USE_DISPLAY_OPT_CL5200
	DspStruct.DirectDraw = 1;
#else
	DspStruct.DirectDraw = 0;
#endif

	useSeperator = get_global_bparam(GLOBAL_PRICESIGN_1000);
	useSeperator &= 0x01;

	if((status_run.run_mode == RUN_SALE)&&(UseSaleModeEditFlag==0)){
		DspLoadFont1(9);
	} else {
		DspLoadFont1(8);
	}
	
	switch(nDigitPos)
	{
		default:
			x = nDigitPos*8;
			break;
	}
	
	if (reverse && mode != DISPLAY_UNDEFINED) {
		__display_string_reverse(dataString, size, mode);
	} else {
		__display_string(dataString, size, x);
	}
	
	DspStruct.DirectDraw = backup_directdraw;
	
	DspLoadFont1(DSP_MENU_FONT_ID);
}

void VFD7_Diffuse(void)
{
#ifndef USE_DISPLAY_OPT_CL5200
	Dsp_Diffuse();	
#endif
}

void DSP_LCD7_Fill_Buffer(unsigned char x)
{
	
	INT8U i;
	POINT disp_p;
	INT8U triangle_on;
	INT8U backup_directdraw;

	backup_directdraw = DspStruct.DirectDraw;
	DspStruct.DirectDraw = 1;
	
	if (x == 0xff) triangle_on = 1;
	else triangle_on = 0;
	
	if (x == 0xff) x = '8';
	if (x == 0x00) x = ' ';

	if((status_run.run_mode == RUN_SALE)&&(UseSaleModeEditFlag==0)) {
		DspLoadFont1(9);
	} else {
		DspLoadFont1(8);
	}
	
	for (i = 0; i < 26; i++)
	{
		disp_p = point(0,i*8);
		Dsp_Write_1Char_7seg(disp_p, x, 8);
		LCD_BufferWriteTriangle(i, triangle_on);
	}

	DspStruct.DirectDraw = backup_directdraw;
	
	DspLoadFont1(DSP_MENU_FONT_ID);	
}

void VFD7_Fill_Buffer(unsigned char x)
{
	DSP_LCD7_Fill_Buffer(x);
}

void VFD7_save_restore(INT8U x)
{
}

#endif
