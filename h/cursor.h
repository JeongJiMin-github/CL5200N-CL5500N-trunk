/****************************************************************************
 *
 *  Copyright		:	(c) 2002 CAS
 *  Filename		:	menu.h
 *  Version			:	0.1
 *  Programmer(s)	:	Chung jong kwon (CJK)
 *  Created			:	2003/09/16
 *  Description		:	CLP menu header
 *
****************************************************************************/

#ifndef _MENU_CURSOR_H__
#define _MENU_CURSOR_H__

#define CURSOR_TYPE_INSERT	0
#define CURSOR_TYPE_DELETE	1
#pragma pack(4)
typedef struct {
	INT8U type;
	INT8U line;
	INT16U col;
	INT16U timer;
	CWD chw;
#ifdef USE_THAI_FONT
	INT32U thai_ch;
#endif        
	INT8U mode;
	INT16U x_offset;
	INT8U y_offset;
	INT8U disp_on;
	INT8U status;
} MENU_CURSOR_STR;

typedef struct {
	char char_security;
	char char_decimal;
	char char_unit1000;
	char char_cursorvfd;
	char char_fixed;
	char char_yes;
	char char_no;
	char language_type;
} KI_GLOBAL_STR;
#pragma pack()

//extern KI_GLOBAL_STR KeyInSet;
extern NEARDATA KI_GLOBAL_STR KeyInSet;//HYP 20060412 _huge --> _near for small code size

extern void cursor_blink(MENU_CURSOR_STR *cursor);
extern void cursor_clear(MENU_CURSOR_STR *cursor);
extern void cursor_init(MENU_CURSOR_STR *cursor, INT8U mode, INT8U y, INT16U x);
extern void cursor_ready(MENU_CURSOR_STR *cursor, INT8U cursortype, INT8U line, INT16U col, INT8U ch1, INT8U ch2);
#endif
