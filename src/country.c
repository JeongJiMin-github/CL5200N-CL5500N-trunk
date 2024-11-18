/*****************************************************************************
|*		       
|*  Copyright		:	(c) 2003 CAS
|*  Filename		:	country.c
|*  Version		:	0.5
|*  Programmer(s)	:	getwing (Jae-Ok Kim)
|*  Created		:	2003/12/15
|*  Description		:	Country Code    ISO 3166 Code  
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
#include "pgm_app.h"

#include "country.h"
#include "menu_code_text1.h"
//

#ifdef USE_TRACEABILITY
// #define	  FLASH_COUNTRY_AREA  	1193034	
void	country_create(void)			// command code  0x1351
{
	INT16U cap_code[2]={0x134A,0x134B};
	menu1_create(0x1341,cap_code,FLASH_COUNTRY_AREA, COUNTRY_STRUCT_SIZE, OFF);
}

void    country_delete(void)			// command code  0x1352
{	menu1_delete(0x1343, 0x134A, FLASH_COUNTRY_AREA, COUNTRY_STRUCT_SIZE);		}

#ifndef DISABLE_LIST_MENU
void 	country_list(void)				// command code  0x1353
{
	INT16U input_cap[3]={0x1E00,0x1E01,0x1E02};
	display_pgm_list(0x1342, 0x134A, input_cap, 0x71FF, FLASH_COUNTRY_AREA, COUNTRY_STRUCT_SIZE);
}
#endif



INT16U 	country_code_read(INT16S read_num)
{
	INT16U 	country_code;
	INT32U	flash_addr;

	if(read_num <= 0) return 0;
	
	flash_addr = COUNTRY_STRUCT_SIZE * (long)(read_num-1);	
	flash_addr += DFLASH_BASE + FLASH_COUNTRY_AREA;
//	flash_addr = flash_addr + COUNTRY_STRUCT_SIZE * (read_addr-1);
	country_code = Flash_wread(flash_addr);

	return country_code;
}

#endif