/*****************************************************************************
|*		       
|*  Copyright		:	(c) 2012 CAS
|*  Filename		:	department.c
|*  Version		:	v 0.5
|*  Programmer(s)	:	cjk
|*  Created		:	2012/03/07
|*  Description		:	traceability_chn.c
|*              				
****************************************************************************/


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "globals.h"
#include "initial.h"
#include "ram.h"
#include "flash.h"
#include "mode_sale.h"
#include "sale_report.h"
#include "main.h"
#include "menu.h"
#include "caption_app.h"
#include "prt_font.h"
#include "dsp_common.h"

#ifdef USE_CHN_CART_SCALE
#include "traceability_chn.h"



TRACE_STRUCT CurCHNTrace;
INT32S CurCHNTraceRemainWeight;
INT8U CurCHNTraceLotNo;


void meat_trace_load(TRACE_STRUCT *buf, INT16U idx)
{
	INT32U addr;

	addr = DFLASH_BASE + FLASH_MEAT_TRACECODE_AREA + TRACE_SIZE * idx;
	Flash_sread(addr, (INT8U *)buf, TRACE_SIZE);
}

void meat_trace_save(TRACE_STRUCT* buf, INT16U idx)
{
	INT32U addr;

	addr = DFLASH_BASE + FLASH_MEAT_TRACECODE_AREA + TRACE_SIZE * idx;
	Flash_swrite(addr, (INT8U *)buf, TRACE_SIZE);
}

void meat_trace_erase(INT16U idx)
{
	INT32U addr;

	addr = DFLASH_BASE + FLASH_MEAT_TRACECODE_AREA + TRACE_SIZE * idx;
	Flash_clear(addr, 0xff, TRACE_SIZE);
}

INT32S meat_trace_remainWeight_load(INT16U idx)
{
	INT32U addr;

	addr = NVRAM_MEAT_REMAIN_WEIGHT + MEAT_REMAIN_WEIGHT_SIZE * idx;
	return (INT32S)get_nvram_lparam(addr);
}

void meat_trace_remainWeight_save(INT16U idx, INT32S weight)
{
	INT32U addr;

	addr = NVRAM_MEAT_REMAIN_WEIGHT + MEAT_REMAIN_WEIGHT_SIZE * idx;
	set_nvram_lparam(addr, weight);
}



void vege_trace_load(TRACE_STRUCT *buf, INT32U pluNo, INT8U lotRoom)
{
	INT32U addr;
	
	addr = TRACE_SIZE * (VEGE_TRACE_LOT_COUNT * (pluNo - 1) + lotRoom);
	addr += DFLASH_BASE + FLASH_VEGE_TRACECODE_AREA;
	Flash_sread(addr, (INT8U *)buf, TRACE_SIZE);
}

void vege_trace_save(TRACE_STRUCT* buf, INT32U pluNo, INT8U lotRoom)
{
	INT32U addr;
	
	addr = TRACE_SIZE * (VEGE_TRACE_LOT_COUNT * (pluNo - 1) + lotRoom);
	addr += DFLASH_BASE + FLASH_VEGE_TRACECODE_AREA;
	Flash_swrite(addr, (INT8U *)buf, TRACE_SIZE);
}

void vege_trace_erase(INT32U pluNo, INT8U lotRoom)
{
	INT32U addr;
	
	addr = TRACE_SIZE * (VEGE_TRACE_LOT_COUNT * (pluNo - 1) + lotRoom);
	addr += DFLASH_BASE + FLASH_VEGE_TRACECODE_AREA;
	Flash_clear(addr, 0xff, TRACE_SIZE);
}

INT32S vege_trace_remainWeight_load(INT32U pluNo, INT8U lotRoom)
{
	INT32U addr;

	addr = VEGE_REMAIN_WEIGHT_SIZE * (VEGE_TRACE_LOT_COUNT * (pluNo - 1) + lotRoom);
	addr += NVRAM_VEGE_REMAIN_WEIGHT;
	return (INT32S)get_nvram_lparam(addr);
}

void vege_trace_remainWeight_save(INT32U pluNo, INT8U lotRoom, INT32S weight)
{
	INT32U addr;

	addr = VEGE_REMAIN_WEIGHT_SIZE * (VEGE_TRACE_LOT_COUNT * (pluNo - 1) + lotRoom);
	addr += NVRAM_VEGE_REMAIN_WEIGHT;
	set_nvram_lparam(addr, weight);
}

void vege_trace_clear_stock(INT32U pluNo, INT8U *dt, INT32U percent)
{
	TRACE_STRUCT vege_trace;
	INT8U i;
	INT32S remainWeight;
	INT32U saled_weight;
	INT32U srcDateVal, destDateVal;
	INT32U srcTimeVal, destTimeVal;
	INT32U stock_percent;

	for (i = 0; i < VEGE_TRACE_LOT_COUNT; i++)
	{
		vege_trace_load(&vege_trace, pluNo, i);
		remainWeight = vege_trace_remainWeight_load(pluNo, i);
		saled_weight = vege_trace.inputWeight - remainWeight;

		//삭제조건 
		//1) 판매백분률이 입력받은 백분률보다 크거나 같은 경우 
		stock_percent = saled_weight * 100L / vege_trace.inputWeight;
        	if (stock_percent >= percent)
		{
			srcDateVal = vege_trace.dt[0]*10000 + vege_trace.dt[1]*100 + vege_trace.dt[2]*1;
			srcTimeVal = vege_trace.dt[3]*10000 + vege_trace.dt[4]*100 + vege_trace.dt[5]*1;

			destDateVal = dt[0]*10000 + dt[1]*100 + dt[2]*1;
			destTimeVal = dt[3]*10000 + dt[4]*100 + dt[5]*1;

			//2) 시간이 입력받은 날짜보다 이전인 경우
			if ((srcDateVal < destDateVal) || (srcDateVal == destDateVal) && (srcTimeVal <= destTimeVal))
			{
				vege_trace_remainWeight_save(pluNo, i, 0);
			}
		}
	}

}

void vege_trace_test(void)
{
	INT16U i;
	INT16U j;
	TRACE_STRUCT vege_trace;
	INT32U start_addr;
	INT32U offset;
	INT8U head;
	INT32S r_weight;
	
	for (i = 0; i < 1000; i++)
	{
		head = get_nvram_bparam(NVRAM_VEGE_TRACE_HEAD + i);
		//if (head)
		{
			//sprintf(MsgBuf, "%ld (h=%d)\r\n", i+1, head);
			//MsgOut(MsgBuf);
			for (j = 0; j < VEGE_TRACE_LOT_COUNT; j++)
			{
				offset = TRACE_SIZE * (VEGE_TRACE_LOT_COUNT * i + j);
				start_addr = DFLASH_BASE + FLASH_VEGE_TRACECODE_AREA + offset;
				Flash_sread(start_addr, (INT8U *)&vege_trace, TRACE_SIZE);
				offset = VEGE_REMAIN_WEIGHT_SIZE * (VEGE_TRACE_LOT_COUNT * i + j);
				start_addr = NVRAM_VEGE_REMAIN_WEIGHT + offset;
				r_weight = get_nvram_lparam(start_addr);
				if (vege_trace.code[0] == 0xff) break;
				if (j == 0)
				{
					sprintf(MsgBuf, "%ld (h=%d)\r\n", i+1, head);
					MsgOut(MsgBuf);
				}
				sprintf(MsgBuf, "  [%02d] = %02d/%02d/%02d %s,%ld,%ld\r\n", j, vege_trace.dt[0], vege_trace.dt[1], vege_trace.dt[2], vege_trace.code, vege_trace.inputWeight, r_weight);
				MsgOut(MsgBuf);
				Delay_10ms(1);
			}
		}

	}

}




void trace_init(void)
{
	// vegetable
	//memset((INT8U*)(NVRAM_BASE + NVRAM_VEGE_TRACE_HEAD), 0, VEGE_TRACE_PLU_COUNT);	// head
	nvram_set(NVRAM_VEGE_TRACE_HEAD, 0, VEGE_TRACE_PLU_COUNT);	// head
	//memset((INT8U*)(NVRAM_BASE + NVRAM_VEGE_REMAIN_WEIGHT), 0, 4*VEGE_TRACE_LOT_COUNT*VEGE_TRACE_PLU_COUNT);	// remain weight
	nvram_set(NVRAM_VEGE_REMAIN_WEIGHT, 0, 4*VEGE_TRACE_LOT_COUNT*VEGE_TRACE_PLU_COUNT);	// remain weight
	Flash_clear(DFLASH_BASE + FLASH_VEGE_TRACECODE_AREA, 0xff, TRACE_SIZE * VEGE_TRACE_LOT_COUNT * VEGE_TRACE_PLU_COUNT);

	// meat
	//memset((INT8U*)(NVRAM_BASE + NVRAM_MEAT_REMAIN_WEIGHT), 0, 4*MEAT_TRACE_LOT_COUNT);	// remain weight
	nvram_set(NVRAM_MEAT_REMAIN_WEIGHT, 0, 4*MEAT_TRACE_LOT_COUNT);	// remain weight
	Flash_clear(DFLASH_BASE + FLASH_MEAT_TRACECODE_AREA, 0xff, TRACE_SIZE * MEAT_TRACE_LOT_COUNT);
}

INT8U trace_compare_data(INT8U *srcBuf, INT8U *destBuf, INT16U length)
{
	INT8U i;

	for (i = 0; i < length; i++) 
	{
		if (srcBuf[i] != destBuf[i]) return 0;
		if (srcBuf[i] == 0) return 1;	// null까지 일치하면 PASS
	}
	return 1;
}

void trace_call_back(long num)
{
	CAPTION_STRUCT 		cap;
	POINT 			disp_p;
	TRACE_STRUCT 	str;
	INT8U 	buffer[49];
	INT16U	w;
	INT8U lotRoom;
	INT8U vege_trace_head;
		
	disp_p = point(CallbackEditPosition_y*Startup.menu_ygap, CallbackEditPosition_x*Startup.menu_xgap);
	memset((INT8U*)buffer, 0x20, 48);

	w = Dsp_GetChars();
	buffer[w-1] = 0;
	Dsp_Write_String(disp_p,(HUGEDATA char *)buffer);

	
	if(num == 0) 
	{
		caption_split_by_code(0xE00C,&cap,0);
		Dsp_Write_String(disp_p,(HUGEDATA char *)cap.form);
		return;
	}

	if (Operation.traceCodeType == CHN_TRACE_CODE_TYPE_VEGETABLE)
	{
		vege_trace_head = get_nvram_bparam(NVRAM_VEGE_TRACE_HEAD + (status_scale.cur_pluid-1));
		lotRoom = (VEGE_TRACE_LOT_COUNT + vege_trace_head - 1 - (num - 1)) % VEGE_TRACE_LOT_COUNT;
		vege_trace_load(&str, status_scale.cur_pluid, lotRoom);
	}
	else
	{
		lotRoom = num - 1;
		meat_trace_load(&str, lotRoom);
	}

	if(str.code[0] < 0x20 || str.code[0] > 0x7f) {	// if not ascii
		caption_split_by_code(0xE00C,&cap,0);
		Dsp_Write_String(disp_p,(HUGEDATA char *)cap.form);
		return;	
	} else {
		memcpy((INT8U*)buffer, str.code, TRACE_CODE_SIZE);
		buffer[TRACE_CODE_SIZE]=0;
		Dsp_Write_String(disp_p,(HUGEDATA char *)buffer);
	}

}

void trace_call_traceNo(void)
{
	CAPTION_STRUCT 	cap;
	INT8U	result, old_font, key_mode;
	INT32U	index = 1;
	INT8U lotRoom;
	INT8U vege_trace_head;

	if (Operation.traceCodeType == CHN_TRACE_CODE_TYPE_VEGETABLE)
	{
		if (status_scale.cur_pluid == 0)
		{
			BuzOn(2);
			return;
		}
	}

	key_mode = KEY_GetMode();
	KEY_SetMode(1);
	old_font = DspStruct.Id1;
	DspLoadFont1(DSP_MENU_FONT_ID);
	
	while(1)
	{      
		Menu_Init();

		//caption_split_by_code(0x1381, &cap,0);
		sprintf(cap.form, "  TRACE");
		Menu_HeadCreate((HUGEDATA char *)cap.form);

		//caption_split_by_code(0x138A, &cap,0);
		sprintf(cap.form, "Trace Lot:");
		cap.input_length = 2;
		if (Operation.traceCodeType == CHN_TRACE_CODE_TYPE_VEGETABLE)
		{
			cap.input_max = 32;
		}
		else
		{
			cap.input_max = 4;
		}
		cap.input_min = 0;
		cap.input_itype = 'N';
		caption_adjust(&cap,NULL);

		Menu_InputCreate(1, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
							cap.input_max, cap.input_min, 0, (HUGEDATA INT32U *)&index, NULL, NULL, trace_call_back);
		Menu_SetCallbackMode(1, 1);

		result = Menu_InputDisplay();
		if(result == MENU_RET_SAVE) 
		{
			CurCHNTraceLotNo = index;
			if (Operation.traceCodeType == CHN_TRACE_CODE_TYPE_VEGETABLE)
			{
				vege_trace_head = get_nvram_bparam(NVRAM_VEGE_TRACE_HEAD + (status_scale.cur_pluid - 1));
				lotRoom = (VEGE_TRACE_LOT_COUNT + vege_trace_head - 1 - (CurCHNTraceLotNo - 1)) % VEGE_TRACE_LOT_COUNT;
				vege_trace_load(&CurCHNTrace, status_scale.cur_pluid, lotRoom);
				CurCHNTraceRemainWeight = vege_trace_remainWeight_load(status_scale.cur_pluid, lotRoom);
			}
			else
			{
				lotRoom = CurCHNTraceLotNo - 1;
				meat_trace_load(&CurCHNTrace, lotRoom);
				CurCHNTraceRemainWeight = meat_trace_remainWeight_load(lotRoom);
			}
			break;
		} else if(result == MENU_RET_ESC) break;
	}

	Dsp_Fill_Buffer(0);						// LCD Buffer clear
	DspLoadFont1(old_font);
	Dsp_Diffuse();
	KEY_SetMode(key_mode);
	sale_display_update(0x0fff); 	

}

void trace_direct_call_traceNo(INT8U lotNo) // lotNo는 가장 최근 lot가 1
{
	INT8U lotRoom;
	INT8U vege_trace_head;
	
	CurCHNTraceLotNo = lotNo;
	if (Operation.traceCodeType == CHN_TRACE_CODE_TYPE_VEGETABLE)
	{
		vege_trace_head = get_nvram_bparam(NVRAM_VEGE_TRACE_HEAD + (status_scale.cur_pluid - 1));
		lotRoom = (VEGE_TRACE_LOT_COUNT + vege_trace_head - 1 - (CurCHNTraceLotNo - 1)) % VEGE_TRACE_LOT_COUNT;
		vege_trace_load(&CurCHNTrace, status_scale.cur_pluid, lotRoom);
		CurCHNTraceRemainWeight = vege_trace_remainWeight_load(status_scale.cur_pluid, lotRoom);
	}
	else
	{
		lotRoom = CurCHNTraceLotNo - 1;
		meat_trace_load(&CurCHNTrace, lotRoom);
		CurCHNTraceRemainWeight = meat_trace_remainWeight_load(lotRoom);
	}
}

void trace_remainWeightSummary(INT32U pluNo, INT8U lotNo, INT32S weight)
{
	INT8U lotRoom;
	INT8U vege_trace_head;
	INT32S oldWeight;

	if (Operation.traceCodeType == CHN_TRACE_CODE_TYPE_VEGETABLE)
	{
		vege_trace_head = get_nvram_bparam(NVRAM_VEGE_TRACE_HEAD + (pluNo - 1));
		lotRoom = (VEGE_TRACE_LOT_COUNT + vege_trace_head - 1 - (lotNo - 1)) % VEGE_TRACE_LOT_COUNT;
		oldWeight = vege_trace_remainWeight_load(pluNo, lotRoom);
		oldWeight -= ad_cnv_weight_by_gram(weight);	// gram 단위로 변경하여 감산
		if (oldWeight < 0) oldWeight = 0;
		vege_trace_remainWeight_save(pluNo, lotRoom, oldWeight);
	}
	else
	{
		lotRoom = lotNo - 1;
		oldWeight = meat_trace_remainWeight_load(lotRoom);
		oldWeight -= ad_cnv_weight_by_gram(weight);	// gram 단위로 변경하여 감산
		if (oldWeight < 0) oldWeight = 0;
		meat_trace_remainWeight_save(lotRoom, oldWeight);
	}
}

void trace_edit_remainWeight(INT32U pluNo, INT8U lotNo)
{
	CAPTION_STRUCT 	cap;
	INT8U	result, old_font, key_mode;
	INT32U	index = 1;
	INT8U lotRoom;
	INT8U vege_trace_head;
	INT32S rweight;

	if (Operation.traceCodeType == CHN_TRACE_CODE_TYPE_VEGETABLE)
	{
		if (pluNo == 0)
		{
			BuzOn(2);
			return;
		}
	}
	if (lotNo == 0)
	{
		BuzOn(2);
		return;
	}

	key_mode = KEY_GetMode();
	KEY_SetMode(1);
	old_font = DspStruct.Id1;
	DspLoadFont1(DSP_MENU_FONT_ID);


	if (Operation.traceCodeType == CHN_TRACE_CODE_TYPE_VEGETABLE)
	{
		vege_trace_head = get_nvram_bparam(NVRAM_VEGE_TRACE_HEAD + (pluNo - 1));
		lotRoom = (VEGE_TRACE_LOT_COUNT + vege_trace_head - 1 - (lotNo - 1)) % VEGE_TRACE_LOT_COUNT;
		rweight = vege_trace_remainWeight_load(pluNo, lotRoom);
	}
	else
	{
		lotRoom = lotNo - 1;
		rweight = meat_trace_remainWeight_load(lotRoom);
	}

	
	Menu_Init();

	//caption_split_by_code(0x1381, &cap,0);
	sprintf(cap.form, "  TRACE");
	Menu_HeadCreate((HUGEDATA char *)cap.form);

	//caption_split_by_code(0x138A, &cap,0);
	sprintf(cap.form, "Remain Weight:");
	cap.input_length = 7;
	cap.input_max = 999999;
	cap.input_min = 0;
	cap.input_itype = 'N';
	caption_adjust(&cap,NULL);

	Menu_InputCreate(1, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
						cap.input_max, cap.input_min, 3, (HUGEDATA INT32U *)&rweight, NULL, NULL, NULL);

	result = Menu_InputDisplay();
	if(result == MENU_RET_SAVE) 
	{
		CurCHNTraceRemainWeight = rweight;
		if (Operation.traceCodeType == CHN_TRACE_CODE_TYPE_VEGETABLE)
		{
			vege_trace_remainWeight_save(pluNo, lotRoom, rweight);
		}
		else
		{
			meat_trace_remainWeight_save(lotRoom, rweight);
		}
	}

	Dsp_Fill_Buffer(0);						// LCD Buffer clear
	DspLoadFont1(old_font);
	Dsp_Diffuse();
	KEY_SetMode(key_mode);
	sale_display_update(0x0fff); 	
}

void transaction_log_delete(void)
{
	//memset((INT8U*)(NVRAM_BASE+Startup.addr_send_buf), 0, (INT32U)Startup.max_send_buf*MAX_TRANSACTION_TYPE7_SALE_SIZE);
	nvram_set(Startup.addr_send_buf, 0, (INT32U)Startup.max_send_buf*MAX_TRANSACTION_TYPE7_SALE_SIZE);

	Report_InitResetTime();
	ClerkClearTransaction();
	//Modified by JJANG 20080121
	set_nvram_wparam(NVRAM_SEND_REMAINDED_HEAD,0);
	set_nvram_wparam(NVRAM_SEND_REMAINDED_TAIL,0);
	set_nvram_wparam(NVRAM_TOTAL_SEND_SALE,0);
	set_nvram_wparam(NVRAM_TOTAL_SEND_CHEANGE,0);
	
	set_nvram_bparam(NVRAM_TRANS_PAY_HEAD,0);
	set_nvram_bparam(NVRAM_TRANS_PAY_TAIL,0);
	
	set_nvram_bparam(NVRAM_SLAVECHECK_FLAG,0);
	set_nvram_bparam(NVRAM_TRANSACTION_CLERK,0);
	set_nvram_lparam(NVRAM_BUSINESS_DATE_FLAG,0);	//NVRAM_BUSINESS_DATE_FLAG 1 byte + NVRAM_BUSINESS_DATE 3 byte
	
	set_nvram_wparam(NVRAM_REOPEN_HEAD,0);
	
	ClearSalesSummaries(99);

}




#define M_ID_M_TRACE_LOT		1
#define M_ID_M_TRACE_CODE		2
#define M_ID_M_TRACE_WEIGHT		3
#define M_ID_M_TRACE_DATE		4
#define M_ID_M_TRACE_TIME		5

void callback_end_meat_trace(INT32U num)
{
	TRACE_STRUCT meat_trace;
	INT32U	addr;
	INT8U	id_trace_lot, id_trace_code, id_trace_weight, id_trace_date, id_trace_time;
	INT8U	date_str[20];
	INT8U	time_str[20];
	INT8U	day_type;

	day_type = rtc_get_date_format();

	id_trace_lot = Menu_GetMenuId(M_ID_M_TRACE_LOT);
	id_trace_code = Menu_GetMenuId(M_ID_M_TRACE_CODE);
	id_trace_weight = Menu_GetMenuId(M_ID_M_TRACE_WEIGHT);
	id_trace_date = Menu_GetMenuId(M_ID_M_TRACE_DATE);
	id_trace_time = Menu_GetMenuId(M_ID_M_TRACE_TIME);

	addr = DFLASH_BASE + FLASH_MEAT_TRACECODE_AREA;
	addr += (num - 1) * TRACE_SIZE;
	Flash_sread(addr, (HUGEDATA INT8U *)&meat_trace, TRACE_SIZE);
	if (meat_trace.code[0] < 0x20 || meat_trace.code[0] > 0x7f) memset((INT8U*)&meat_trace, 0x00, TRACE_SIZE);
	input_day_format(day_type, date_str, meat_trace.dt[0], meat_trace.dt[1], meat_trace.dt[2]);
	input_time_format(time_str, meat_trace.dt[3], meat_trace.dt[4], meat_trace.dt[5]);
	
	*Menu[id_trace_lot].pval = num;
	memcpy(Menu[id_trace_code].pstr, meat_trace.code, sizeof(meat_trace.code));
	*Menu[id_trace_weight].pval = meat_trace.inputWeight;
	memcpy(Menu[id_trace_date].pstr, date_str, strlen(date_str));
	memcpy(Menu[id_trace_time].pstr, time_str, strlen(time_str));

      	MenuRefresh = ON;
}


void meat_trace_create(void)
{	
	TRACE_STRUCT meat_trace;
	CAPTION_STRUCT 		cap;
	INT16U	result;
	INT32U  lot_no;
	INT32U	inputWeight;
	INT8U	trace_code[TRACE_CODE_SIZE+1];
	INT8U	date_str[20];
	INT8U	time_str[20];
	INT8U	daytype_str[20];
	INT8U	form_string[20];
	INT8U	date_form[20];
	INT8U	time_form[20];
	INT8U	date_sep;
	INT8U	time_sep;
	INT8U	day_type;
	INT32U	start_addr;
	INT32U	flash_addr;


	start_addr = DFLASH_BASE + FLASH_MEAT_TRACECODE_AREA;

	day_type = rtc_get_date_format();
#if defined(USE_SINGLE_LINE_GRAPHIC)
	date_sep = '.'; //7-seg display용
	time_sep = '.'; //7-seg display용
#else
	date_sep = get_global_bparam(GLOBAL_DATE_SEPARATOR);
	time_sep = get_global_bparam(GLOBAL_TIME_SEPARATOR);
#endif
	sprintf(date_form, "[  %c  %c  ]", date_sep, date_sep);
	sprintf(time_form, "[  %c  %c  ]", time_sep, time_sep);
	switch (day_type) 
	{
		case 0:	caption_split_by_code(0x384A, &cap,0); 	break;
		case 1:	caption_split_by_code(0x384B, &cap,0);	break;
		case 3:
		case 4: caption_split_by_code(0x384D, &cap,0);	break;
	}
	cap.form[14]=0;
	sprintf(daytype_str, "%s", cap.form);


	while (1) 
	{	
		//caption_split_by_code(35,&cap,0);
		lot_no = 1;

		flash_addr = start_addr + (TRACE_SIZE * (lot_no - 1));
		Flash_sread(flash_addr, (HUGEDATA INT8U *)&meat_trace, TRACE_SIZE);
		if (meat_trace.code[0] < 0x20 || meat_trace.code[0] > 0x7f) memset((INT8U*)&meat_trace, 0x00, TRACE_SIZE);	
		memcpy(trace_code, meat_trace.code, sizeof(meat_trace.code));
		inputWeight = meat_trace.inputWeight;
		input_day_format(day_type, date_str, meat_trace.dt[0], meat_trace.dt[1], meat_trace.dt[2]);
		input_time_format(time_str, meat_trace.dt[3], meat_trace.dt[4], meat_trace.dt[5]);
	
	
		Menu_Init();
	
		//sprintf(cap.form, "Trace Code");
		caption_split_by_code(0x1331, &cap,0);
		Menu_HeadCreate((HUGEDATA char *)cap.form);
	
		sprintf(cap.form, "Lot No.");
		cap.input_length = 1;
		cap.input_dlength = 1;
		cap.input_max = 4;
		cap.input_min = 1;
		cap.input_itype = 'N';
		caption_adjust(&cap, NULL);

		Menu_InputCreate(M_ID_M_TRACE_LOT, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
							cap.input_max, cap.input_min, 3, &lot_no, NULL, NULL, NULL);
		Menu_AddEndFunction(M_ID_M_TRACE_LOT, callback_end_meat_trace);
	
		sprintf(cap.form, "Trace Code");
		cap.input_length = TRACE_CODE_SIZE;
		cap.input_dlength = TRACE_CODE_SIZE;
		cap.input_itype = 'S';
		caption_adjust(&cap, NULL);
		Menu_InputCreate(M_ID_M_TRACE_CODE, (HUGEDATA char *)cap.form, MENU_TYPE_STR, cap.input_x, cap.input_dlength, cap.input_length,
							cap.input_max, cap.input_min, 0, NULL, trace_code, NULL, NULL);	
		Menu_SetProperties(M_ID_M_TRACE_CODE, MENU_P_CHECK_BLANKS);
	
		sprintf(cap.form, "Input Weight");
		cap.input_length = 7;
		cap.input_dlength = 7;
		cap.input_itype = 'N';
		cap.input_max = 999999;
		cap.input_min = 0;
		caption_adjust(&cap, NULL);
		Menu_InputCreate(M_ID_M_TRACE_WEIGHT, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
							cap.input_max, cap.input_min, 3, &inputWeight, NULL, NULL, NULL);


		sprintf(form_string,"99%c99%c99", date_sep, date_sep);
		sprintf(cap.form, "Date:");
		cap.input_x=strlen((char *)cap.form)+1;
		strcat((char *)cap.form, date_form);
		strcat((char *)cap.form, daytype_str);
		cap.input_length = 8;
		cap.input_dlength = 8;
		cap.input_max = 999999;
		cap.input_min = 0;
		Menu_InputCreate(M_ID_M_TRACE_DATE, (HUGEDATA char *)cap.form, MENU_TYPE_DATE, cap.input_x, cap.input_dlength, cap.input_length,
							cap.input_max, cap.input_min, 0, NULL, (HUGEDATA INT8U *)date_str, (HUGEDATA INT8U *)form_string, NULL);
		Menu_SetGeneralFlag(M_ID_M_TRACE_DATE, day_type);
		Menu_SetFixedCharacter(M_ID_M_TRACE_DATE, date_sep);


		sprintf(form_string,"99%c99%c99", time_sep, time_sep);
		sprintf(cap.form, "Time:");
		cap.input_x=strlen((char *)cap.form)+1;
		strcat((char *)cap.form,time_form);
		cap.input_length = 8;
		cap.input_dlength = 8;
		cap.input_max = 999999;
		cap.input_min = 0;
		Menu_InputCreate(M_ID_M_TRACE_TIME, (HUGEDATA char *)cap.form, MENU_TYPE_TIME, cap.input_x, cap.input_dlength, cap.input_length,
							cap.input_max, cap.input_min, 0, NULL, (HUGEDATA INT8U *)time_str, (HUGEDATA INT8U *)form_string, NULL);
		Menu_SetFixedCharacter(M_ID_M_TRACE_TIME, time_sep);


		result = Menu_InputDisplay();
		if (result == MENU_RET_SAVE) 
		{
			flash_addr = start_addr + (TRACE_SIZE * (lot_no - 1));
			memcpy(meat_trace.code, trace_code, TRACE_CODE_SIZE);
			meat_trace.inputWeight = inputWeight;
			input_convert_date(day_type, (INT8U *)date_str, &meat_trace.dt[0], &meat_trace.dt[1], &meat_trace.dt[2]);
			input_convert_time((INT8U*)time_str, &meat_trace.dt[3], &meat_trace.dt[4], &meat_trace.dt[5]);
			Flash_swrite(flash_addr, (HUGEDATA INT8U *)&meat_trace, TRACE_SIZE);
			if (Operation.traceCodeType == CHN_TRACE_CODE_TYPE_VEGETABLE)
			{
				//vege_trace_remainWeight_save(pluNo, lotRoom, inputWeight);
			}
			else
			{
				meat_trace_remainWeight_save(lot_no - 1, inputWeight);
			}

		} 
		else if (result == MENU_RET_ESC) 
		{
			break;
		}
	}

}



#endif
