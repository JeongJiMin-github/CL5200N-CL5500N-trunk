/*****************************************************************************
 *  Copyright		:	(c) 2003 CAS
 *  Filename		:	menu.c
 *  Version		:	0.1
 *  Programmer(s)	:	Chung jong kwon (CJK)
 *  Created		:	2003/09/16
 *  Description		:	CLP Menu routine    
 ****************************************************************************/

#include <stdio.h>
#include <string.h>
#include "globals.h"
#include "prt_font.h"
#include "dsp_common.h"
#include "key.h"
#include "key_typedef.h"
#include "main.h"
#include "prt_common.h"
#include "menu.h"
#include "dsp_vfd7.h"
#include "common.h"
#include "initial.h"
#include "flash.h"
#include "flash_app.h"
#include "cursor.h"
#include "dsp_app.h"

//KI_GLOBAL_STR KeyInSet;
NEARDATA KI_GLOBAL_STR KeyInSet;//HYP 20060412 _huge --> _near for small code size

void cursor_blink(MENU_CURSOR_STR *cursor)
{
	POINT disp_p;
	INT8U disp_pv;
	char ch;
	INT8U backup_directdraw;
	char wbits, hbits;
	
	wbits = display_font_get_width();
	hbits = display_font_get_height();
	backup_directdraw = DspStruct.DirectDraw;
	DspStruct.DirectDraw = 1;
	switch(cursor->mode) {
		case KI_TARE:
			disp_pv = display_parameter.mode_pos[DISPLAY_TARE] + cursor->col + cursor->x_offset;
			break;
		case KI_WEIGHT:
			disp_pv = display_parameter.mode_pos[DISPLAY_WEIGHT] + cursor->col + cursor->x_offset;
			break;
		case KI_UNIT :
			disp_pv = display_parameter.mode_pos[DISPLAY_UNITPRICE] + cursor->col + cursor->x_offset;
			break;
		case KI_TOTAL :
			disp_pv = display_parameter.mode_pos[DISPLAY_PRICE] + cursor->col + cursor->x_offset;
			break;
		case KI_SEGMENT :
			disp_pv = cursor->col + cursor->x_offset;
			break;
		case KI_GRAPHIC :
#if defined(HEBREW_FONT) || defined(USE_ARAB_CONVERT)
			disp_p = point(cursor->y_offset+cursor->line*hbits, LCD_Y_MARGIN_REAL-DspStruct.Hbits1-cursor->x_offset-cursor->col*wbits);
#else
			disp_p = point(cursor->y_offset+cursor->line*hbits, cursor->x_offset+cursor->col*wbits);
#endif
			break;			
	}
	
	if ((SysTimer100ms%5) == cursor->timer) {
		if(cursor->disp_on) {
			if(cursor->mode == KI_GRAPHIC) {
				if(cursor->status == OFF) {
					if(cursor->type == CURSOR_TYPE_INSERT) DspStruct.InsertCur = 1;
					else DspStruct.DeleteCur = 1;
					cursor->status = ON;
				} else {
					//DspStruct.InsertCur = 0;
					//DspStruct.DeleteCur = 0;
					cursor->status = OFF;
				}
				//DspStruct.DirectDraw = 1; 
				
				if(DspStruct.StrType == ONE_BYTE_FONT_STR) {
#ifdef USE_THAI_FONT
	                if((INT8U)(cursor->chw.byte.left) < 0x80) { //Eng
	                	Dsp_Write_1Char(disp_p, cursor->chw.byte.left);
	                } else {
	                    Dsp_Write_1Char32(disp_p, cursor->thai_ch);
	                }
#elif defined(USE_ARAB_FONT) && defined(_USE_LCD37D_40DOT_) // CL5200J Arab version
					Dsp_Write_1Char_arab(disp_p, cursor->chw.byte.left);
#elif defined(USE_ARAB_CONVERT)  //CL5200N Persian version
					if((cursor->chw.words.left) < 0x0080) { //Eng
                            Dsp_Write_1Char(disp_p, cursor->chw.words.left);
                    } else {
                            Dsp_Write_1CharLong(disp_p, cursor->chw.words.left);
                    }
#else
					Dsp_Write_1Char(disp_p, cursor->chw.byte.left);
#endif
				} else {
					if((INT8U)(cursor->chw.byte.left) < 0x80) { //Eng
						Dsp_Write_1Char(disp_p, cursor->chw.byte.left);
					} else { //Kor
						Dsp_Write_2Char(disp_p, cursor->chw.word);
						wbits = display_font_get_width2();
						hbits = display_font_get_height2();
					}
				}
#if defined(_USE_LCD37D_40DOT_) || defined(_USE_LCD26d_16x96_)
				if (DspStruct.DirectDraw)
				{
					Dsp_Diffuse();
				}
#endif
				DspStruct.DirectDraw = backup_directdraw;
				DspStruct.InsertCur = 0;
				DspStruct.DeleteCur = 0;
			} else {
				if(cursor->status == OFF) {
					ch = KeyInSet.char_cursorvfd;	//ch = CURSOR_CHAR;
					if(ch == 0x00) ch = 0x20;
					cursor->status = ON;
				} else {
					ch = cursor->chw.byte.left;
					cursor->status = OFF;
				}
				if(ch == '.' || ch == ',') VFD7_display_char(disp_pv, 0x20);
				VFD7_display_char(disp_pv, ch);
				if(cursor->chw.byte.right == '.' || cursor->chw.byte.right == ',')
					VFD7_display_char(disp_pv, cursor->chw.byte.right);
				VFD7_Diffuse();
			}
			cursor->disp_on = 0;
		}
	} else {
		cursor->disp_on = 1;
	}
	DspStruct.DirectDraw = backup_directdraw; 
}

void cursor_clear(MENU_CURSOR_STR *cursor)
{
	POINT disp_p;
	INT8U disp_pv;
	char ch;
	INT8U backup_directdraw;
	char wbits, hbits;
	
	wbits = display_font_get_width();
	hbits = display_font_get_height();
	switch(cursor->mode) {
		case KI_TARE:
			disp_pv = display_parameter.mode_pos[DISPLAY_TARE] + cursor->col + cursor->x_offset;
			break;
		case KI_WEIGHT:
			disp_pv = display_parameter.mode_pos[DISPLAY_WEIGHT] + cursor->col + cursor->x_offset;
			break;
		case KI_UNIT :
			disp_pv = display_parameter.mode_pos[DISPLAY_UNITPRICE] + cursor->col + cursor->x_offset;
			break;
		case KI_TOTAL :
			disp_pv = display_parameter.mode_pos[DISPLAY_PRICE] + cursor->col + cursor->x_offset;
			break;
		case KI_SEGMENT :
			disp_pv = cursor->col + cursor->x_offset;
			break;
		case KI_GRAPHIC :
#if defined(USE_ARAB_CONVERT) || defined(HEBREW_FONT)
			disp_p = point(cursor->y_offset+cursor->line*hbits, LCD_Y_MARGIN_REAL-DspStruct.Hbits1-cursor->x_offset-cursor->col*wbits);
#else
			disp_p = point(cursor->y_offset+cursor->line*hbits, cursor->x_offset+cursor->col*wbits);
#endif
			break;			
	}
	
	if(cursor->mode == KI_GRAPHIC) {
		DspStruct.InsertCur = 0;
		DspStruct.DeleteCur = 0;
		
		backup_directdraw = DspStruct.DirectDraw;
		DspStruct.DirectDraw = 1;
		
		if(DspStruct.StrType == ONE_BYTE_FONT_STR) {
#if defined(USE_ARAB_FONT) && defined(_USE_LCD37D_40DOT_)  // CL5200J Arab version
			Dsp_Write_1Char_arab(disp_p, cursor->chw.byte.left);
#elif defined USE_ARAB_CONVERT //CL5200N persian version
		Dsp_Write_1CharLong(disp_p, cursor->chw.words.left);
#else
			Dsp_Write_1Char(disp_p, cursor->chw.byte.left);
#endif
		} else {
			if((INT8U)(cursor->chw.byte.left) < 0x80) { //Eng
				Dsp_Write_1Char(disp_p, cursor->chw.byte.left);
			} else { //Kor
				Dsp_Write_2Char(disp_p, cursor->chw.word);
				wbits = display_font_get_width2();
				hbits = display_font_get_height2();
			}
		} 
		DspStruct.DirectDraw = backup_directdraw;
	} else {
		ch = cursor->chw.byte.left;
		if(ch == '.' || ch == ',') VFD7_display_char(disp_pv, 0x20);
		VFD7_display_char(disp_pv, ch);
		VFD7_Diffuse();
	}
}

void cursor_init(MENU_CURSOR_STR *cursor, INT8U mode, INT8U y, INT16U x)
{
	cursor->y_offset = y;
	cursor->x_offset = x;
	cursor->mode = mode;
}

void cursor_ready(MENU_CURSOR_STR *cursor, INT8U cursortype, INT8U line, INT16U col, INT8U ch1, INT8U ch2)
{
	cursor->type = cursortype;
	cursor->line = line;
	cursor->col = col;
	cursor->chw.byte.left = ch1;
	cursor->chw.byte.right = ch2;
#ifdef USE_ARAB_CONVERT
	cursor->chw.words.left = ch1;
	cursor->chw.words.right = ch2;
#endif	
	cursor->timer = (SysTimer100ms+1)%5;
	cursor->disp_on = 1;
	cursor->status = OFF;
}
