/*****************************************************************************
|*		       
|*  Copyright		:	(c) 2013 CAS
|*  Filename		:	license.c
|*  Version		:	1.0
|*  Programmer(s)		:	cjk
|*  Created		:	2013/02/18
|*  Description		:	
|*              				
****************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "globals.h"
#include "initial.h"
#include "license.h"
#include "encrypt.h"
#include "caption_app.h"
#include "menu.h"
#include "mode_main.h"
#include "rtc.h"
#include "main.h"
#include "prt_font.h"
#include "dsp_common.h"
#include "flash.h"

#ifdef USE_MANAGE_LICENSE

LICENSE_STRUCT Lic;


// scaleHWID 생성 함수
void lic_generate_scaleHWID(void)
{
	INT8U scaleHWID[LIC_SCALE_HW_ID_SIZE + 1];
	INT8U scaleHWID_E[LIC_SCALE_HW_ID_SIZE + 1];
	INT8U temp_key[LIC_KEY_SIZE];

	srand(SysTimer100ms);
	
#ifdef USE_PERSIAN_CALENDAR	
	RTC_Convert_Persian_Read();
#else
	RTC_CLK_Burst_Read();
#endif
	scaleHWID[0] = 0; // model code upper byte (0001)
	scaleHWID[1] = 1; // model code lower byte (0001)
	scaleHWID[2] = bcd2hex(RTCStruct.year);
	scaleHWID[3] = bcd2hex(RTCStruct.month);
	scaleHWID[4] = bcd2hex(RTCStruct.date);
	scaleHWID[5] = rand()%0x100;
	scaleHWID[6] = rand()%0x100;
	scaleHWID[7] = rand()%0x100;

	hexstr2hex(KEY_SCALE, temp_key, LIC_KEY_SIZE);
	des_encrypt(scaleHWID, scaleHWID_E, temp_key, LIC_SCALE_HW_ID_SIZE);

	memcpy(Lic.ScaleHWID_E, scaleHWID_E, LIC_SCALE_HW_ID_SIZE);

	Flash_swrite(DFLASH_BASE + FLASH_LICENSE, scaleHWID_E, LIC_SCALE_HW_ID_SIZE);

}


// 현재 저장된 scaleHWID 의 값이 유효한지 확인하는 함수
INT8U lic_check_validation_scaleHWID(void)
{
	INT8U scaleHWID[LIC_SCALE_HW_ID_SIZE + 1];
	INT8U scaleHWID_E[LIC_SCALE_HW_ID_SIZE + 1];
	INT8U temp_key[LIC_KEY_SIZE];
	
	Flash_sread(DFLASH_BASE + FLASH_LICENSE, scaleHWID_E, LIC_SCALE_HW_ID_SIZE);

	memcpy(Lic.ScaleHWID_E, scaleHWID_E, LIC_SCALE_HW_ID_SIZE);

	hexstr2hex(KEY_SCALE, temp_key, LIC_KEY_SIZE);
	des_decrypt(scaleHWID_E, scaleHWID, temp_key, LIC_SCALE_HW_ID_SIZE);

	if (scaleHWID[0] != 0) return FALSE; // model code high (model code 0001)
	else if (scaleHWID[1] != 1) return FALSE; // model code low (model code 0001)
	else if (scaleHWID[2] > 99) return FALSE; // 연
	else if (scaleHWID[3] > 12 || scaleHWID[3] == 0) return FALSE; // 월
	else if (scaleHWID[4] > 31 || scaleHWID[4] == 0) return FALSE; // 일

	return TRUE; // Scale HW ID 설정 완료
}


// function별 license가 유효한지 확인하는 함수
//  fnId : function Id
//  funcCode_E : 암호화된 Function Code(keygen에서 생선된 License code)
INT8U lic_check_function_license(INT8U fnId, INT8U *funcCode_E)
{
	INT32U addr;
	INT8U funcCode[LIC_FUNCTION_CODE_SIZE + 1];
	INT8U *funcCodeKey;
	INT8U temp_key[LIC_KEY_SIZE];
		
	switch (fnId)
	{
		case 0:
			funcCodeKey = (INT8U*)CODE_FUNC1;
			break;
		case 1:
			funcCodeKey = (INT8U*)CODE_FUNC2;
			break;
		case 2:
			funcCodeKey = (INT8U*)CODE_FUNC3;
			break;
		case 3:
			funcCodeKey = (INT8U*)CODE_FUNC4;
			break;
		case 4:
			funcCodeKey = (INT8U*)CODE_FUNC5;
			break;
		case 5:
			funcCodeKey = (INT8U*)CODE_FUNC6;
			break;
		case 6:
			funcCodeKey = (INT8U*)CODE_FUNC7;
			break;
		case 7:
			funcCodeKey = (INT8U*)CODE_FUNC8;
			break;
		default:
			return OFF;
			break;
	}
	hexstr2hex(funcCodeKey, temp_key, LIC_KEY_SIZE);
	des_decrypt(funcCode_E, funcCode, temp_key, LIC_FUNCTION_CODE_SIZE);

	if (memcmp(funcCode, Lic.ScaleHWID_E, LIC_SCALE_HW_ID_SIZE) == 0) return ON; // 암호화를 푼 Function Code와 ScaleHWID가 일치하는 지 체크
	else return OFF;
	
}


#define M_ID_MANAGE_SCALE_ID	1
#define M_ID_MANAGE_LIC_FUNC1	2

void callback_end_licenseCheck(INT8U *str)
{
	INT8U temp_value;
	INT8U tare_val_type;
	INT8U result;
	char string_buf[30];
	POINT disp_p;
	INT8U id_tare_value;
	INT8U backup_directdraw;
	INT8U ret;
	INT8U funcCode_E[LIC_FUNCTION_CODE_SIZE + 1];

	backup_directdraw = DspStruct.DirectDraw;
	DspStruct.DirectDraw = 1;

	hexstr2hex(str, funcCode_E, LIC_FUNCTION_CODE_SIZE);
	ret = lic_check_function_license(CallbackMenuId - M_ID_MANAGE_LIC_FUNC1, funcCode_E);
	if (ret)
	{
		sprintf(string_buf, "OK  ");
	}
	else
	{
		sprintf(string_buf, "FAIL");
	}
	disp_p = point(CallbackEditPosition_y*Startup.menu_ygap, CallbackEditPosition_x*Startup.menu_xgap); // 2digits : kg/lb symbol
	Dsp_Write_String(disp_p,(HUGEDATA char*)string_buf);

	DspStruct.DirectDraw = backup_directdraw;
	return;
}



void lic_manage(void)
{
	INT8U result;
	CAPTION_STRUCT cap;
	INT8U temp_lic[9];
	INT8U temp_scale[LIC_SCALE_HW_ID_SIZE * 2 + 1];
	INT8U lic_key[LIC_FUNCTION_NUMBER][LIC_FUNCTION_CODE_SIZE * 2 + 1];
	INT16U i;
	INT32U addr;
	INT16U len;

	// License 체크를 위한 Scale H/W ID 설정 여부 판단
	if (!lic_check_validation_scaleHWID()) // Scale H/W ID가 유효하지 않은 경우 ID 생성
	{
		lic_generate_scaleHWID();
	}

	// 암호화된 Scale H/W ID 읽음
	addr = DFLASH_BASE + FLASH_LICENSE;
	Flash_sread(addr, temp_lic, LIC_SCALE_HW_ID_SIZE);
	hex2hexstr(temp_lic, temp_scale, LIC_SCALE_HW_ID_SIZE);
	temp_scale[LIC_SCALE_HW_ID_SIZE * 2] = 0;

	Menu_Init();

	caption_split_by_code(0x1890, &cap,0);
	Menu_HeadCreate((HUGEDATA char *)cap.form);

	caption_split_by_code(0x38f1, &cap,0);	// Scale H/W ID
	caption_adjust(&cap,NULL);
	Menu_InputCreate(M_ID_MANAGE_SCALE_ID, (HUGEDATA char *)cap.form, MENU_TYPE_STR, cap.input_x, cap.input_dlength, cap.input_length,
						cap.input_max, cap.input_min, 0, NULL, temp_scale, NULL, NULL);
	Menu_SetDisable(M_ID_MANAGE_SCALE_ID, ON);

	// Function 별 License
	for (i = 0; i < LIC_FUNCTION_NUMBER; i++)
	{
		addr = DFLASH_BASE + FLASH_LICENSE + LIC_SCALE_HW_ID_SIZE + i * 8;
		Flash_sread(addr, temp_lic, LIC_FUNCTION_CODE_SIZE);
		hex2hexstr(temp_lic, lic_key[i], LIC_FUNCTION_CODE_SIZE);
		lic_key[i][LIC_FUNCTION_CODE_SIZE * 2] = 0;
		
		caption_split_by_code(0x38f2, &cap, 0);// "Func"
		len = strlen(cap.form);
		cap.form[len] = '1' + i;
		cap.form[len+1] = ':';
		cap.form[len+2] = 0;
		caption_adjust(&cap,NULL);
		Menu_InputCreate(M_ID_MANAGE_LIC_FUNC1 + i, (HUGEDATA char *)cap.form, MENU_TYPE_STR, cap.input_x, cap.input_dlength, cap.input_length,
							cap.input_max, cap.input_min, 0, NULL, lic_key[i], NULL, NULL);
		Menu_AddEndFunction(M_ID_MANAGE_LIC_FUNC1 + i, callback_end_licenseCheck);
	}

	result = Menu_InputDisplay();
	if(result == MENU_RET_SAVE) 
	{
		// Function 별 License 저장
		for (i = 0; i < LIC_FUNCTION_NUMBER; i++)
		{
			hexstr2hex(lic_key[i], temp_lic, LIC_FUNCTION_CODE_SIZE);
			addr = DFLASH_BASE + FLASH_LICENSE + LIC_SCALE_HW_ID_SIZE + i * LIC_FUNCTION_CODE_SIZE;
			Flash_swrite(addr, temp_lic, LIC_FUNCTION_CODE_SIZE);
		}
	}

}


#endif
