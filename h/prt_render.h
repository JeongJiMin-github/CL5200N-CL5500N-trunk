/*****************************************************************************
|*			
|*  Copyright		:	(c) 2002 CAS
|*  Filename		:	prt_render.h
|*  Version			:	0.1
|*  Programmer(s)	:	Hong Yi-Phyo (HYP)
|*  Created			:	2002/12/10
|*  Description		:	    
|*				
****************************************************************************/

#ifndef _RENDER_H
#define _RENDER_H

#include "globals.h"

//HYP 20060427 str, bar field is ignored in pre-print area(except shop data) if pre-print mode is on
//fixed str, fixed barcode, bitmap, line is accepted
//#define USE_ONLY_SHOPNAME_PRE_PRT_AREA

extern ROMDATA INT8U nlmask[];
extern ROMDATA INT8U lmask[];
extern ROMDATA INT16U nlmask16[];
extern ROMDATA INT16U lmask16[];

extern INT8U	CheckPrePrtAreaBoundary;

//extern void RenderTest(void);
extern void PrtBarPattern(HUGEDATA INT8U *pattern,short pw);
extern void PrtPostbarPattern(HUGEDATA INT8U *pattern,short pw);
extern void PrtPattern(HUGEDATA INT8U *pattern,short dwr,short w,short h,char rot);
extern void PrtPatternHVflip(HUGEDATA INT8U *pattern,short dwr,short w,short h,char rot,char hv);
//20040209 HYP
extern void PrtPattern_Str_Area_Typeface(void);
extern void PrtTestPattern(void);
extern BOOLEAN	CheckBoundary(short sx, short sy, short ex, short ey) ;
//HYP 20050216
extern void PrtTestHPattern(void);

#ifdef USE_AUSTRALIA_COOL_FUNCTION
// 프린트 가능한 X, Y 좌표
#define PRINT_POSITION_X_MIN 0 
#define PRINT_POSITION_Y_MIN 0 

// CoOL을 그렸을 때 프린트 영역 범위 내에 있으면 Y, 없으면 N
enum COOL_PRINT_ENABLE
{
	COOL_PRINT_ENABLE_N,
	COOL_PRINT_ENABLE_Y,
};

// CoOL 이미지(로고, Ingredient Bar) 너비, 높이 구조체
struct COOL_IMAGE_INFO
{
	const INT8U logo_width;	
	const INT8U logo_height;	
	const INT8U bar_width;	
	const INT8U bar_height;	
};

// CoOL 기능 관련 프린터 좌표를 조정하기 위한 구조체
struct COOL_CONTROL_PRT_POINT
{
	// 원점 좌표
	short origin_x; 
	short origin_y;

	// CoOL을 그릴 때 원점 좌표가 조정될 경우 사용하는 좌표
	short origin_switch_x;
	short origin_switch_y;
	
	// 회전했을 때 기준이 되는 좌표
	short control_x;
	short control_y;

	// CoOL 사각형 영역에 대한 너비와 높이
	short width;
	short height;

	// CoOL 좌표 계산 margin
	const short image_margin;

	// 좌표 계산을 하고나서 프린트 가능한 범위를 설정했는지의 여부
	enum COOL_PRINT_ENABLE print_enable;
	
	struct COOL_IMAGE_INFO image_info;
};

#define COOL_REVERSE_PERCENT_VALUE ((PrtStruct.StrBuffer[3]-'0') * 100 + (PrtStruct.StrBuffer[4]-'0') * 10 +(PrtStruct.StrBuffer[5]-'0') * 1)

extern void cool_set_control_prt_point(struct COOL_CONTROL_PRT_POINT*);
extern void cool_print_rect_outline(const struct COOL_CONTROL_PRT_POINT*);
extern void cool_print_logo_bar(const struct COOL_CONTROL_PRT_POINT*);
extern void cool_print_bar(const struct COOL_CONTROL_PRT_POINT*);
extern void cool_print_font_str(const struct COOL_CONTROL_PRT_POINT*, const char, const char);
#endif
#endif /* _RENDER_H */
