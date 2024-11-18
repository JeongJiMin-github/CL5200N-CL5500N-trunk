/*****************************************************************************
|*		       
|*  Copyright		:	(c) 2003 CAS
|*  Filename		:	pcs_unit.c
|*  Version			:	v 0.9
|*  Programmer(s)	:	getwing (Jae-Ok Kim)
|*  Created			:	2003/12/23
|*  Description		:	define PCS unit
|*              				
****************************************************************************/


#include <stdio.h>
#include <string.h>
#include "globals.h"

#include "commbuf.h"
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
#include "bar_common.h"
#include "prt_interpreter.h"

#include "rtc.h"

#include "adm.h"
#include "main.h"
#include "initial.h"
#include "key.h"
#include "key_typedef.h"
#include "key_function.h"
#include "mode_main.h"
#include "caption_app.h"
#include "common.h"
#include "prt_common.h"
#include "menu.h"
#include "flash_app.h"
#include "dsp_app.h"
#include "dsp_vfd7.h"
#include "key_app.h"

#include "math.h" 
#include "mode_program.h"
#include "cursor.h"
#include "pcs_unit.h"
#include "menu_code_text1.h"
#include "pgm_app.h"

 
void	pcsunit_create(void)		// command code  0x1291
{	
	// <CODE> JHC, 040414, -change code
	// [
	// $1: JHC, 040414, Modify, -0x1291, 0x129A to 0x1281, 0x128A
	INT16U cap_code[2]={15,0x3282};
	menu1_create(0x1281, cap_code, FLASH_PCS_UNIT_AREA, PCS_UNIT_STRUCT_SIZE, OFF);
	// ]
}

void    pcsunit_delete(void)		// command code  0x1292
{
	// <CODE> JHC, 040414, -change code
	// [
	// $1: JHC, 040414, Modify, -0x1292, 0x129A to 0x1282, 0x128A
	menu1_delete(0x1283, 15, FLASH_PCS_UNIT_AREA, PCS_UNIT_STRUCT_SIZE);		
	// ]
}

#ifndef DISABLE_LIST_MENU
void 	pcsunit_list(void)			// command code  0x1293
{	
	INT16U input_cap[3]={0x1E00,0x1E01,0x1E02};
	display_pgm_list(0x1282, 15, input_cap, 0x71FF, FLASH_PCS_UNIT_AREA, PCS_UNIT_STRUCT_SIZE);		
}
#endif

#ifdef USE_GSMART_PLU
#ifdef USE_LOTTEMART_DEFAULT
INT8U pcsSymbolDefault[18][6] = {	"장"	,	// 1
					"마리"	,	// 2
					"필"	,	// 3
					"자루"	,	// 4
					"개"	,	// 5
					"다발"	,	// 6
					"쪽"	,	// 7
					"접시"	,	// 8
					"그릇"	,	// 9
					"잔"	, 	// 10
					"손"	,	// 11, 롯데마트는 kg->손(생선단위)으로 변경 요청.
					"g"	,	// 12
					"토막"	,	// 13
					"봉"	,	// 14
					"두름"	,	// 15
					"세트"	,	// 16
					"축"	,	// 17
					"짝"		// 18
				};

#else
INT8U pcsSymbolDefault[18][6] = {	"장"	,	// 1
					"마리"	,	// 2
					"필"	,	// 3
					"자루"	,	// 4
					"개"	,	// 5
					"다발"	,	// 6
					"쪽"	,	// 7
					"접시"	,	// 8
#ifdef USE_EMART_DEFAULT
					" "	,	// 9
#else
					"그릇"	,	// 9
#endif
					"잔"	, 	// 10
					"kg"	,	// 11
					"g"	,	// 12
					"토막"	,	// 13
					"봉"	,	// 14
					"두름"	,	// 15
					"세트"	,	// 16
					"축"	,	// 17
					"짝"		// 18
				};

#endif

void pcsunit_default_KR(void)
{
	INT16U i;
	INT32U addr;

	for (i = 0; i < MAX_PCS_NO; i++)
	{
		addr  = PCS_UNIT_STRUCT_SIZE * i;
		addr += DFLASH_BASE + FLASH_PCS_UNIT_AREA;
		Flash_wwrite(addr, i + 1);
		Flash_swrite(addr+2, (HUGEDATA INT8U *)pcsSymbolDefault[i], PCS_UNIT_STRUCT_SIZE - 2);
	}
}
#endif
