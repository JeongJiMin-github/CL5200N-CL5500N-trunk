#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
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
#include "key_typedef.h"
#include "key_function.h"
#include "cal.h"
#include "prt_cmdtest.h"
#include "programmode.h"
//#include "file.h"
#include "rtc.h"
#include "debug.h"
#include "ethernet.h"
#include "main.h"
#include "initial.h"
#include "flash_app.h"
#include "common.h"
#include "caption_app.h"

#include "mode_program.h"
#include "mode_calibration.h"
#include "mode_main.h"
#include "mode_sale.h"
#include "mode_commun.h"

#include "cal.h"
#include "cursor.h"
#include "menu.h"
#include "prt_app.h"
#include "dsp_app.h"
#include "network_set.h"
#include "initial_glb.h"
#include "plu_device.h"
#include "pgm_app.h"
#include "license.h"
#include "encrypt.h"

extern INT8U dealer_factory_mode;
extern INT8U cal_mode_plu_init;
INT16U history_index;
INT8U  history_unset;
INT8U  cal_plu_init_flag;

extern INT16S mode_page(INT16S odeep,INT16U code,INT16S *page_cur);
//extern void  display_pgmcode(INT16U code,INT8U set);

INT8U cal_menu_function(INT16U fcode)
{
	INT8U moveto;
	INT16U xcode;

	CurMenuCode = fcode;
	moveto = 0;
	//history_start();	// cal mode 진입위치로 이동 : cal 진입시에만 count 증가
	xcode=fcode;
	xcode&=0xff00;

#ifdef USE_HIDE_DATETIME_MENU
	if(fcode == 0x1840)
		fcode = 0x8700;
#endif

	if ((xcode==0x8200) || (xcode==0x8600)) {
		if (cal_plu_init_flag==0) plu_init(1);
		cal_plu_init_flag=1;
	}
	switch (fcode) {
		case 0x8110 : CalRun_SpanCal(CAL_SPAN_NORMAL);	break;
#ifndef CPU_PIC32MZ
		case 0x8120 : CalRun_FineAdjust();		break;
#endif
		case 0x8130 : CalRun_CapaUnit();		break;
		case 0x8140 : CalRun_Gravity();			break;
		case 0x8150 : CalRun_SpanCal(CAL_SPAN_PERCENT);	break;
		case 0x8160 : CalRun_Linearize();		break;
		case 0x8170 : CalRun_ZeroTare();		break;
		case 0x8183 : CalRun_InitADM(1, ON);		break;
#ifndef CPU_PIC32MZ
		case 0x8184 : CalRun_LinearityFineAdjust();	break;
#endif
		case 0x8185 : CalRun_Hysteresis();		break;
		case 0x8186 : CalRun_SetCreep();		break;
		case 0x8187 : CalRun_AdFirmware();		break;
#ifdef USE_MORNITORING_CNT
		case 0x8188 : CalRun_Scale_Info();		break;
#endif
		case 0x8220 : CalRun_ScaleType();		break;

		case 0x8211 : CalRun_ClearMemory(1);		break;	// Clear Report		// mode 1:NVRam, 2:PLU 4:Table, 8:Flash All, 16:print/adm default
		case 0x8212 : CalRun_ClearMemory(2);		break;	// Clear All PLU
		case 0x8213 : CalRun_ClearMemory(4);		break;	// Clear All Table
		case 0x8214 : CalRun_ClearMemory(7);		break;	// Clear Report,PLU,Table
#if !defined(COUNTRY_USA) && !defined(USE_CHINA_ADD_DEFAULT)
		case 0x8215 : moveto = CalRun_ClearMemory(8);	break;	// Clear Flash
#endif
		case 0x8310 : printer_mode();		break; // Modified by CJK 20050927
		case 0x8320 : 
			if(PrtStruct.Mode)	printer_receiptset();
			else 			printer_labelset();
			break;
		case 0x8330 : printer_sensor();			break;
		case 0x8340 : printer_settings();		break;
		case 0x8350 : printer_stb_time();		break;
		case 0x8360 : adjust_feed_length();		break;
		case 0x8370 : printer_preprint_setting();	break;
		case 0x8380 : CalRun_InitPrinter(ON);		break;
		
		case 0x8410 : netset_enable_interface();	break;

		case 0x8510 : CalRun_DisplayTest();		break;
		case 0x8520 : CalRun_TestAD();			break;
		case 0x8530 : CalRun_KeyTest();			break;
		case 0x8540 : printer_test();			break;
		case 0x8550 : printer_sensor_test();		break;
#if defined(CPU_PIC32MZ) && ( defined(CL5200_PROJECT) || defined(CL5500_PROJECT))
		case 0x8560 : CalRun_MemoryInformation();	break;
#endif
		case 0x8570 : CalRun_FirmwareVersion();		break;
		case 0x8580 : CalRun_CashDrawerTest();
			      break;
		case 0x8590 : CalRun_ComTest();			break;
		case 0x8600 : dealer_factory_mode = FACTORY_MODE;
			      pgm_parameter_function(fcode);
			      dealer_factory_mode = 0;	// Added by CJK 20060412
			      break;
#ifdef USE_HIDE_DATETIME_MENU
		case 0x8700 : keyapp_date(0);	  	break; 
#endif
	}
	return moveto;
}

extern INT16U mode_get_code(INT32U addr,INT16U key);

void mode_cal_init(void)
{
	Operation.rewindReady = OFF;
}

void mode_subcal(INT16U code)
{
	INT16U list_code,new_code;
	INT16U x,inputtype;
	INT32U addr,addr_main;
	INT16S    start_id,end_id,cur_id,i;
	INT16S    menu_cnt,page_max;
	char   data[64];//,string_buf[64];
	POINT  disp_p;
	INT8U  moveto, depth;
	INT16S    deep,page_cur[4];
	INT8U  menu_no;
	INT8U  prev_menu_no;
#ifdef USE_HIDE_DATETIME_MENU
	INT32U addr_temp;
#endif
#if defined(USE_SINGLE_LINE_GRAPHIC)
	MENU_CURSOR_STR cursor;
	INT8U cursor_id;
#endif

	for(i = 0; i < 4; i++) page_cur[i] = 0;
	depth = 0;
	moveto = 0;
	deep=0;
	cur_id  =  0;
	cal_plu_init_flag=0;
START:
	if(code < 0x8000) {
		BuzOn(2);
		code = 0x8000;
	}
	//display_pgmcode(code,1);
	addr_main =caption_search_by_code(code);
	addr      =DFLASH_BASE+ FLASH_CAPTION_AREA;
	if (addr_main==addr) goto CAP_ERROR;
	list_code=Flash_wread(addr_main+CAPTION_MENUSTART_POINT);
#ifdef USE_HIDE_DATETIME_MENU
	if(code == 0x8000 && cur_id == 7)
	{
		cur_id = 0;
		addr_main = caption_search_by_code(0x1840);
	}
	list_code=Flash_wread(addr_main+CAPTION_MENUSTART_POINT);
#endif
	if(list_code == 0) {	// Link Code가 없을 경우
		inputtype = Flash_wread(addr_main+CAPTION_INPUTTYPE_POINT);		// 실행코드
		if(inputtype == 0) {	// 실행 Code가 없을 경우
		        BuzOn(2);
			caption_error(0);
		} else {
			moveto = cal_menu_function(inputtype);		// 실행
			if(moveto == MENU_MOVE_TO_ROOT) return;
		}
		depth--;
		switch(depth) {
			case 0 : code&=0xf000;	break;
			case 1 : code&=0xff00;	break;
			case 2 : code&=0xfff0;	break;
			default : code = 0x8000; depth = 0; break;
		}
		goto START;
	}
	addr=caption_search_by_code(list_code);
	menu_cnt = 0;
	prev_menu_no = '0';
	while (1) {
		x=Flash_wread(addr+CAP_MSG_LENGTH*menu_cnt);
		if (x==0) break;	// "(end)" 일때
		//////////////////////////////////////////////////
		// "(end)" caption을 삭제하기 위해 다음 비교 추가	// CJK080611
		menu_no=Flash_bread(addr+CAP_MSG_LENGTH*menu_cnt+CAPTION_NAME_POINT);	// 메뉴번호
		if (!isdigit(menu_no) || menu_no <= prev_menu_no) break;	
		//////////////////////////////////////////////////
		if (menu_cnt>10)  goto CAP_ERROR;
		menu_cnt++;
		prev_menu_no = menu_no;
	}
#ifdef USE_HIDE_DATETIME_MENU
	if(code == 0x8000)
		menu_cnt++;
#endif
	page_max=menu_cnt/Startup.line_max;
	if (menu_cnt%Startup.line_max) page_max++;
	deep = mode_page(deep,code,page_cur);
NEWPAGE:
	if (page_cur[deep]>=page_max) page_cur[deep]=page_max-1;
	if (page_cur[deep]<0) page_cur[deep]=0;
	DspStruct.DirectDraw = 0;
	Dsp_Fill_Buffer(0);
	DspLoadFont1(DSP_MENU_FONT_ID);
	menu_display_menucode(code,1);
	menu_displaytitle(code,0);
	menu_writepage(page_max, (page_cur[deep]+1));
	start_id=page_cur[deep] * Startup.line_max;
	end_id  =start_id + Startup.line_max - 1;
	if (end_id>=menu_cnt) end_id=menu_cnt-1;
	for (i=start_id; i<=end_id; i++) {
		Flash_sread(addr+i*CAP_MSG_LENGTH+2,(HUGEDATA INT8U *)data,32);
#ifdef USE_HIDE_DATETIME_MENU
		if(code == 0x8000 && i == 6)
		{
			addr_temp = caption_search_by_code(0x1840);
			Flash_sread(addr_temp+2,(HUGEDATA INT8U *)data,32);
			data[0] = '7';
		}
#endif
#if defined(USE_SINGLE_LINE_GRAPHIC)
		display_clear(DISPLAY_UNITPRICE);
		display_string(DISPLAY_UNITPRICE,(INT8U *)data+3);
#else
		disp_p = point(Startup.menu_ygap*(i-start_id+Startup.menu_type), 0);
#ifdef HEBREW_FONT
		Convert_HebrewPreview((INT8U *)data, strlen(data), 0, 0); 
		hebrew_codetemp[strlen(data)] = 0;
		disp_p.x = 202 - disp_p.x;
		Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
		Convert_ArabPreview((INT8U *)data, strlen(data), 0, 0, 0); 
		arab_codetemp[strlen(data)] = 0;
		disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
		Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
		Dsp_Write_String(disp_p,data);
#endif
#endif
	}
#if defined(USE_SINGLE_LINE_GRAPHIC)
	display_clear(DISPLAY_PRICE);
#endif
	Dsp_Diffuse();
	DspStruct.DirectDraw = 1;
	while (1) {
#if defined(USE_SINGLE_LINE_GRAPHIC)
		cursor_init(&cursor, KI_TARE, 0, 0);
		cursor_id = Flash_bread(addr+CAP_MSG_LENGTH*start_id+CAPTION_NAME_POINT);
		cursor_ready(&cursor, CURSOR_TYPE_DELETE, 0, deep + 1, cursor_id, 0);
#endif
		while (!KEY_Read()) {
			usb_update_check();
			CASInterpreter(&CommBufPrt);
#if defined(USE_SINGLE_LINE_GRAPHIC)
			cursor_blink(&cursor);
#endif
		}
		BuzOn(1);
		switch(KeyDrv.Type) {
			case KEY_TYPE_NUMERIC :
					cur_id = KeyDrv.CnvCode;
#ifdef USE_HIDE_DATETIME_MENU
					if(code == 0x8000 && cur_id == 7)
						goto START;
#endif
					new_code=mode_get_code(addr,cur_id);
					if (new_code) {
						code      = new_code;
						depth++;
					} else {
						BuzOn(2);
						break;
					}
					goto START;
			case KEY_TYPE_FUNCTION :
				switch(KeyDrv.CnvCode) {
					case KS_CLEAR	 : break;
					case KP_PAGE_DN  :
					case KP_ARROW_DN : page_cur[deep]++; goto NEWPAGE;
					case KP_PAGE_UP  :
					case KP_ARROW_UP : page_cur[deep]--; goto NEWPAGE;
					case KP_ESC :
						if(code <= 0x8000) {
							if(Cal_flagAdmBackup==1) {
								ADM_BackupEeprom();	//
								Cal_flagAdmBackup = 0;
							}
							return;
						} else {
							depth--;
							switch(depth) {
								case 0 : code&=0xf000;	break;
								case 1 : code&=0xff00;	break;
								case 2 : code&=0xfff0;	break;
							}
#ifdef USE_HIDE_DATETIME_MENU
							cur_id = 0;
#endif
							goto START;
						}
					case KP_FEED :
						keyapp_feed(OFF);
						break;
#ifdef CPU_PIC32MZ
					case KP_ENTER :
						cur_id = Flash_bread(addr+CAP_MSG_LENGTH*start_id+CAPTION_NAME_POINT);
						cur_id -= '0';
						new_code=mode_get_code(addr,cur_id);
						if (new_code) {
							code      = new_code;
							depth++;
						} else {
							BuzOn(2);
							break;
						}
						goto START;
#endif
					default :
						BuzOn(2);
						break;
					////////////////////////
				}
				break;
			default :
				BuzOn(2);
				break;
		}
#if !defined(USE_SINGLE_LINE_GRAPHIC)
		menu_display_menucode(code,1);
#endif
	}
CAP_ERROR:
	caption_error(0);
}

void mode_calibration(void)
{
	INT16U code;
	INT8U flag_tmp;
	INT8U string_buf[30];
	INT16S len, x, y;
	POINT disp_p;

	KEY_SetMode(1);
	Cal_flagAdmBackup = 0;
	
	code = 0x8000;
	cal_mode_plu_init=1;
	     
	display_alloff_primary_indication();
	VFD7_Diffuse();

//	menu_display_menucode(code,1);
	ADM_WaitData(ADM_CMD_SET_INI_ZR_FLAG, 1, ADM_PARAM_ON);	// Pass Initial Zero Check
	flag_tmp=get_global_bparam(GLOBAL_PRINT_OPER_FLAG);
	ConfirmPrintOnLabel = (flag_tmp>>2) & 0x03;
	KEY_InitKeyBuf();

	mode_cal_init();

	history_start();

	mode_subcal(code);

	status_run.run_mode = RUN_SALE;
	sale_display_update(UPDATE_ALL|UPDATE_CLEARALL);
	KEY_SetMode(0);
}

void history_start(void)
{
	history_index=get_base_wparam(FLASH_CALLOG_COUNT);
	if (history_index==0xffff) {
		history_index=0x8000;
	} else {
		history_index++;
	}
	history_index &= 0x7FFF;		// 0x8000으로 History qty update 되없음을 판단
	history_unset=1;
}

void history_save(INT8U mode,INT32U value)
{
	CAL_HISTORY hist;
	INT32U addr;
#ifdef USE_PERSIAN_CALENDAR
	RTC_Convert_Persian_Read();
#else
	RTC_CLK_Burst_Read();
#endif

	addr =(INT32U)history_index;
	addr%=HISTORY_QTY;
	addr*=HISTORY_STRUCT_SIZE;
	addr+=DFLASH_BASE;
	addr+=FLASH_CALLOG_DATA;

	if (history_unset) {
		set_base_wparam(FLASH_CALLOG_COUNT,history_index | 0x8000);		// 0x8000으로 History qty update 되없음을 판단
		memset(&hist,0,HISTORY_STRUCT_SIZE);
		hist.year  = RTCStruct.year;
		hist.month = RTCStruct.month;
		hist.date  = RTCStruct.date;
		hist.hour  = RTCStruct.hour;
		hist.min   = RTCStruct.min;
		hist.sec   = RTCStruct.sec;
		history_unset=0;
	} else {
		Flash_sread(addr,(HUGEDATA INT8U *)&hist,sizeof(hist));
	}
	switch (mode) {
		case HISTORY_CAPA     : hist.capa=value; break;
		case HISTORY_ADZERO   : hist.ad_zero=value; break;
		case HISTORY_ADSPAN   : hist.ad_span=value; break;
		case HISTORY_ADMIDUP  : hist.ad_midup=value; break;
		case HISTORY_ADMIDDN  : hist.ad_middn=value; break;
	}
	hist.ver = CAL_HISTORY_VER;
	Flash_swrite(addr,(HUGEDATA INT8U *)&hist,sizeof(hist));
	
}

extern void SFlashBlockErase(INT8U cs, HUGEDATA INT16U *address);
extern void NvramBackupDataProgram(HUGEDATA INT32U *address, HUGEDATA INT32U *data, INT32U dword_count);
void history_flash2ram(HUGEDATA INT8U *buf)
{
	INT32U base_addr;
	INT32U addr;
	INT8U *ram;
	INT32U flag;

	ram = buf;
	base_addr = DFLASH_BASE;
	addr = base_addr + FLASH_CALLOG_COUNT;
	Flash_sread(addr, ram, HISTORY_STRUCT_SIZE * HISTORY_QTY + 2);	// Calibration History

	ram += HISTORY_STRUCT_SIZE * HISTORY_QTY + 2;

	addr = base_addr + FLASH_ETHERNET_MAC;
	Flash_sread(addr, ram, 6);	// MAC Address
	
	ram += 6;

	// CTS Function backup	
	// CTS 사용 여부와 상관없이 해당 영역 유지
	addr = base_addr + FLASH_CTS_STRUCT;
	Flash_sread(addr, ram, CTS_STRUCT_SIZE);	// CTS Struct

	ram += CTS_STRUCT_SIZE;

	// CTS Function backup	
	// CTS 사용 여부와 상관없이 해당 영역 유지
	addr = base_addr + FLASH_CTS_STRUCT2;
	Flash_sread(addr, ram, CTS_STRUCT_SIZE2);	// CTS Struct2

	ram += CTS_STRUCT_SIZE2;

	// Reserve 2byte (CTS struct2 관련 reserve) 
	addr = base_addr + FLASH_CTS_STRUCT2 + CTS_STRUCT_SIZE2;
	Flash_sread(addr, ram, 2);	// reserve 2byte

	ram += 2;

// LICENSE Backup
	addr = base_addr + FLASH_LICENSE;
	Flash_sread(addr, ram, LIC_STRUCT_SIZE); // 8 + 4 * 8 + 40
	
//USB_DATA_ROM_UPDATE_STATUS backup
	ram += LIC_STRUCT_SIZE;
	
	addr = EXT_MEMCARD_ADDR_X + FLASH_USB_DATA_ROM_UPDATE;
	Flash_sread(addr, ram, FLASH_USB_DATA_ROM_UPDATE_SIZE);
	
	//FTL: ERROR MANAGEMNET Backup
	ram += FLASH_USB_DATA_ROM_UPDATE_SIZE;
	
	addr = EXT_MEMCARD_ADDR_X + FLASH_FTL_ERR_MANAGEMENT_AREA;
	Flash_sread(addr, ram, FTL_ERROR_MANAGEMENT_SIZE);
	
	//FLASH_START_COUNT backup
	ram += FTL_ERROR_MANAGEMENT_SIZE;
	
	addr = base_addr + FLASH_START_COUNT_HIGH;
	Flash_sread(addr, ram, 4);
	
	ram += 4;

	// FWMODE 1byte (BOOT에서 사용하는 FWMODE) 
	addr = base_addr + FLASH_FWMODE_SET;
	Flash_sread(addr, ram, 1);

	ram += 1;
#ifdef USE_ADM_AUTO_RESTORE
	// ADM Init error count backup 1byte 
	addr = base_addr + FLASH_ADM_INIT_ERROR_COUNT;
	Flash_sread(addr, ram, 1);

	ram += 1;

	// ADM eeprom error count backup 1byte 
	addr = base_addr + FLASH_ADM_EEP_ERROR_COUNT;
	Flash_sread(addr, ram, 1);

	ram += 1;
	
	// ADM init complete backup 1byte 
	addr = base_addr + FLASH_ADM_INIT_COMPLETE;
	Flash_sread(addr, ram, 1);

	ram += 1;
#endif	

	//flash erase 중 꺼지는 경우를 방어하기 위해 Backup
	External_Interrupt_Disable(); // Power off Pending
	SFlashBlockErase(FLASH1_CS,(INT16U *)(NVRAM_BACKUP_TEMPORARY_ADDR));
	NvramBackupDataProgram((INT32U*)(NVRAM_BACKUP_TEMPORARY_ADDR+4), (INT32U*)buf, HISTORY_BUF_SIZE/4);
	flag = NVRAM_BACKUP_FLAG_HISTORY_WRITTEN;
	NvramBackupDataProgram((INT32U*)(NVRAM_BACKUP_TEMPORARY_ADDR), (INT32U*)&flag, 1);
	External_Interrupt_Enable();
}

void history_ram2flash(HUGEDATA INT8U *buf)
{
	INT32U base_addr;
	INT32U addr;
	INT8U *ram;
	INT32U flag;

	ram = buf;
	base_addr = DFLASH_BASE;
	addr = base_addr + FLASH_CALLOG_COUNT;
	Flash_swrite(addr, ram, HISTORY_STRUCT_SIZE * HISTORY_QTY + 2);	// Calibration History

	ram += HISTORY_STRUCT_SIZE * HISTORY_QTY + 2;

	addr = base_addr + FLASH_ETHERNET_MAC;
	Flash_swrite(addr, ram, 6);	// MAC Address

	ram += 6;

	// CTS structure backup
	// CTS 사용 여부와 상관없이 해당 영역 유지
	addr = base_addr + FLASH_CTS_STRUCT;
	Flash_swrite(addr, ram, CTS_STRUCT_SIZE);	// CTS Struct

	ram+=CTS_STRUCT_SIZE;

	// CTS structure2 backup
	// CTS 사용 여부와 상관없이 해당 영역 유지
	addr = base_addr + FLASH_CTS_STRUCT2;
	Flash_swrite(addr, ram, CTS_STRUCT_SIZE2);	// CTS Struct2

	ram += CTS_STRUCT_SIZE2;

	// Reserve 2byte (CTS struct2 관련 reserve) 
	addr = base_addr + FLASH_CTS_STRUCT2 + CTS_STRUCT_SIZE2;
	Flash_swrite(addr, ram, 2);	// reserve 2byte

	ram += 2;

// LICENSE Backup
	addr = base_addr + FLASH_LICENSE;
	Flash_swrite(addr, ram, LIC_STRUCT_SIZE);
	
//USB_DATA_ROM_UPDATE_STATUS backup	
	ram += LIC_STRUCT_SIZE;
	
	addr = EXT_MEMCARD_ADDR_X + FLASH_USB_DATA_ROM_UPDATE;
	Flash_swrite(addr, ram, FLASH_USB_DATA_ROM_UPDATE_SIZE);	
	
// FTL_ERROR_MANAGEMENT backup
	ram += FLASH_USB_DATA_ROM_UPDATE_SIZE;
	
	addr = EXT_MEMCARD_ADDR_X + FLASH_FTL_ERR_MANAGEMENT_AREA;
	Flash_swrite(addr, ram, FTL_ERROR_MANAGEMENT_SIZE);
	
// FLASH_START_COUNT backup
	ram += FTL_ERROR_MANAGEMENT_SIZE;
	
	addr = base_addr + FLASH_START_COUNT_HIGH;
	Flash_swrite(addr, ram, 4);
	
	ram += 4;
	
	// FWMODE 1byte (BOOT에서 사용하는 FWMODE) 
	addr = base_addr + FLASH_FWMODE_SET;
	Flash_swrite(addr, ram, 1);
	
	ram += 1;
 #ifdef USE_ADM_AUTO_RESTORE
 	// ADM Init error count backup 1byte 
 	addr = base_addr + FLASH_ADM_INIT_ERROR_COUNT;
 	Flash_swrite(addr, ram, 1);

 	ram += 1;

 	// ADM eeprom error count backup 1byte 
 	addr = base_addr + FLASH_ADM_EEP_ERROR_COUNT;
 	Flash_swrite(addr, ram, 1);

 	ram += 1;

	// ADM complete backup 1byte 
 	addr = base_addr + FLASH_ADM_INIT_COMPLETE;
 	Flash_swrite(addr, ram, 1);

 	ram += 1;
 #endif	
 
	// Backup이 완료되었으므로 flag 초기화
	// Sector Erase 생략
	flag = NVRAM_BACKUP_FLAG_HISTORY_INIT;
	NvramBackupDataProgram((INT32U*)(NVRAM_BACKUP_TEMPORARY_ADDR), (INT32U*)&flag, 1);
}

void history_qty_reduction(void)		// 0x8000으로 History qty update 되없음을 판단
{
	CAL_HISTORY hist[4];
	INT16U hid;
	INT16U cnt = 0;
	INT32U addr;

	hid = get_base_wparam(FLASH_CALLOG_COUNT);

	if (hid & 0x8000) return;
	
	while(cnt < 4)
	{
		addr = (INT32U)hid--;
		addr%= 5;
		addr*=HISTORY_STRUCT_SIZE;
		addr+=DFLASH_BASE;
		addr+=FLASH_CALLOG_DATA;
		
		Flash_sread(addr,(HUGEDATA INT8U *)&(hist[cnt++]),sizeof(CAL_HISTORY));
	}

	while(cnt > 0)
	{
		hid++;
		addr = (INT32U)hid;
		addr%= HISTORY_QTY;
		addr*=HISTORY_STRUCT_SIZE;
		addr+=DFLASH_BASE;
		addr+=FLASH_CALLOG_DATA;
		
		Flash_swrite(addr,(HUGEDATA INT8U *)&(hist[--cnt]),sizeof(CAL_HISTORY));
	}
	set_base_wparam(FLASH_CALLOG_COUNT, hid | 0x8000);
}

#ifdef USE_CTS_FUNCTION
void CTS_InitCtsData(void)
{
	if (CtsStruct.Initialized == CTS_INIT_COMPLETED)
	{
		memset(CtsStruct.reserve, 0xFF, sizeof(CtsStruct.reserve));
		CtsStruct.crc16 = crc_16((INT8U*)&CtsStruct, CTS_STRUCT_SIZE - 2);
		Flash_swrite(DFLASH_BASE + FLASH_CTS_STRUCT, (INT8U*)&CtsStruct, CTS_STRUCT_SIZE);
	}
	else
	{
		CtsStruct.CtsEnable = OFF;
		CtsStruct.CtsPeriod = 0;
		CtsStruct.CtsExpired = OFF;
		CtsStruct.CtsCalYear = 0;
		CtsStruct.CtsCalMonth = 0;
		CtsStruct.CtsCalDate = 0;
		CtsStruct.CtsVersion = 0;
		memset(CtsStruct.SerialNo, 0, sizeof(CtsStruct.SerialNo));
		memset(CtsStruct.reserve, 0xFF, sizeof(CtsStruct.reserve));
		CtsStruct.Initialized = CTS_INIT_COMPLETED;
		
		CtsStruct.crc16 = crc_16((INT8U*)&CtsStruct, CTS_STRUCT_SIZE - 2);
		Flash_swrite(DFLASH_BASE + FLASH_CTS_STRUCT, (INT8U*)&CtsStruct, CTS_STRUCT_SIZE);
	}
}

void CTS_UpdateCtsData(void)
{
	if(CtsStruct.CtsVersion ==0x00 || CtsStruct.CtsVersion == 0xFF)
	{
		CtsStruct.CtsVersion = 0x01;
	}
	
	if (CtsStruct.CtsVersion == 0x01)
	{
		CtsStruct2.BoardId = 0;
		memset(CtsStruct2.ProductCode, 0x20, sizeof(CtsStruct2.ProductCode));
		CtsStruct2.EncryptKey1 = K_TIME;
		memset(CtsStruct2.reserve, 0xFF, sizeof(CtsStruct2.reserve));
        CtsStruct2.crc16 = crc_16((INT8U*)&CtsStruct2, CTS_STRUCT_SIZE2 - 2);
		Flash_swrite(DFLASH_BASE + FLASH_CTS_STRUCT2, (INT8U*)&CtsStruct2, sizeof(CTS_STRUCT2));

		CtsStruct.CtsVersion = 0x02;
	}
	
	CtsStruct.crc16 = crc_16((INT8U*)&CtsStruct, CTS_STRUCT_SIZE - 2);
	Flash_swrite(DFLASH_BASE + FLASH_CTS_STRUCT, (INT8U*)&CtsStruct, CTS_STRUCT_SIZE);
}

INT8U CTS_CheckCalibrationDate(void)	// CTS structure가 읽힌 상태에서만 사용해야 함
{
	short ctsYear;
	short ctsMonth;
	short ctsDay;
	INT8U thisYear;
	INT8U thisMonth;
	INT8U thisDay;
	short daysOver;

	if (CtsStruct.CtsEnable == 0) return TRUE;
	if (CtsStruct.CtsPeriod == 0) return TRUE;
	//if (CtsStruct.CtsExpired) return FALSE;
	
	ctsYear = CtsStruct.CtsCalYear;
	ctsMonth = CtsStruct.CtsCalMonth;
	ctsDay = CtsStruct.CtsCalDate;
	date_getnextday(&ctsYear, &ctsMonth, &ctsDay, CtsStruct.CtsPeriod);
	
	thisYear = bcd2hex(RTCStruct.year);
	thisMonth = bcd2hex(RTCStruct.month);
	thisDay = bcd2hex(RTCStruct.date);

	daysOver = date_getdeltadays(thisYear, thisMonth, thisDay, ctsYear, ctsMonth, ctsDay);

 	if (daysOver > 0)
	{
//		CtsStruct.CtsExpired = 1;
		status_scale.cur_displayon |= UPDATE_TITLE;
        CtsStruct.crc16 = crc_16((INT8U*)&CtsStruct, CTS_STRUCT_SIZE - 2);
		Flash_swrite(DFLASH_BASE + FLASH_CTS_STRUCT, (INT8U*)&CtsStruct, CTS_STRUCT_SIZE);
		return FALSE; // 알람 주기가 넘었을 때
	}
	else return TRUE;
	
}

/*********************************************************************************************************
 * @brief       SCALE KEY 및 PW KEY를 생성하는 함수
 * @param       year: 현재 연도
 * @param       month: 현재 월
 * @param       day: 현재 일
 * @param       plain_key: 설계된 구조를 바탕으로 생성된 Plain String의 하위 두 바이트
 * @param       cipher_key: Plain KEY에서 DES 암호화를 통해 생성된 Cipher String의 하위 두 바이트
 * @return      출력값은 포인터를 통해 전달됨
 * @remark      1) ran_key1과 ran_key2를 난수로 생성하여 SCALE KEY를 만듦
 *              2) Password Gen Version, 현재 년월일, ran_key 값을 결합하여 des_passkey를 구성 (DES 암호화)
 *              3) DES 암호화 수행 후 enc_buf[6], enc_buf[7]을 PW KEY로 설정
 *********************************************************************************************************

**********************************************Plain String************************************************
				Version(1) | Model Code(2) | Year(1) | Month(1) | Day(1) | Scale Key(Random, 2)
**********************************************************************************************************

*/
  #ifdef USE_CTS_TEMP_ENABLE
void generate_pw_key(INT16U year, INT16U month, INT16U day, INT16U *plain_key, INT16U *cipher_key)
{
	INT8U ran_key1, ran_key2;
	INT8U des_passkey[8];
	INT8U enc_buf[LIC_SCALE_HW_ID_SIZE + 1];
	INT8U temp_key[LIC_KEY_SIZE];

	memset(enc_buf, 0x00, sizeof(enc_buf));
	memset(temp_key, 0x00, sizeof(temp_key));

	srand(SysTimer100ms);
	ran_key1 = rand() % 0x100;
	ran_key2 = rand() % 0x100;
  #if defined(CL5200N_BP) 
	des_passkey[0] = 0x01; // Password Gen Version
	des_passkey[1] = 0x43; // CL5200N Model Code(ASCII) -> C 
	des_passkey[2] = 0x34; // CL5200N Model Code(ASCII) -> 4
	des_passkey[3] = year; 
	des_passkey[4] = month;
	des_passkey[5] = day;
	des_passkey[6] = ran_key1;
	des_passkey[7] = ran_key2;
  #else
 	des_passkey[0] = 0x01; // Password Gen Version
  	des_passkey[1] = 0x43; // CL5500N Model Code(ASCII) -> C 
  	des_passkey[2] = 0x38; // CL5500N Model Code(ASCII) -> 8
  	des_passkey[3] = year; 
  	des_passkey[4] = month;
  	des_passkey[5] = day;
  	des_passkey[6] = ran_key1;
  	des_passkey[7] = ran_key2;
  #endif
	hexstr2hex(KEY_SCALE, temp_key, LIC_KEY_SIZE);
	des_encrypt(des_passkey, enc_buf, temp_key, LIC_SCALE_HW_ID_SIZE);
	
	*plain_key = (ran_key1 << 8) | ran_key2;
	*cipher_key = (enc_buf[6] << 8) | enc_buf[7];
}
  
/************************************************************************************
 * @brief       임시로 CAL Mode를 진입할 수 있게 하는 함수
 * @return      정상적인 cipher_key 입력 시 함수 종료
 * @remark      1) RTC에서 현재 날짜를 가져와 화면에 표시
 *              2) generate_pw_key()를 호출하여 plain_key 생성
 *              3) 사용자가 입력한 cipher_key KEY가 생성된 cipher_key와 일치하는지 검증
 *                 일치하면 임시로 Cal Mode 진입 허용
 ************************************************************************************/

void allow_calibration_menu(void)
{
	INT8U result;
    INT16U year, month, day;
	INT16U plain_key, cipher_key;  
	INT32U set_val[3];
    CAPTION_STRUCT cap;
	char s_date[20];
	
	Dsp_Clear();
    
	RTC_CLK_Burst_Read();
	menu_display_menucode(0x9999, 1);

    year = bcd2hex(RTCStruct.year);
    month = bcd2hex(RTCStruct.month);
    day = bcd2hex(RTCStruct.date);
	sprintf(s_date, "%02d.%02d.%02d", year, month, day);

	generate_pw_key(year, month, day, &plain_key, &cipher_key);

	set_val[0] = (INT32U)plain_key;
	set_val[1] = 0;

	/**********************************************백도어 메뉴 구성**************************************************
	 실제 메뉴가 아닌 CTS 임시 해제 기능 전용으로 생성된 메뉴라 캡션을 새로 따지 않고 관련된 기존 캡션을 가져와 하드코딩 진행 
    ****************************************************************************************************************/
	Menu_Init();

	caption_split_by_code(0x3841, &cap, 0);
	caption_adjust(&cap,NULL);
	sprintf((char *)cap.form, "DATE:");
    strcat((char *)cap.form, "[");
    strcat((char *)cap.form, s_date);
    strcat((char *)cap.form, "]");

	Menu_InputCreate(1, (char *)cap.form, MENU_TYPE_FIX, cap.input_x, cap.input_dlength, cap.input_length,
						cap.input_max, cap.input_min, 0, NULL , s_date, NULL, NULL);

	caption_split_by_code(0x1a24, &cap, 0);
	caption_adjust(&cap,NULL);
	cap.input_x = 10;
	sprintf((char *)cap.form, "SCALE KEY:");
	Menu_InputCreate(2, (char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
						cap.input_max, cap.input_min, 0, &set_val[0], NULL, NULL, NULL);

    	
	caption_split_by_code(0x1a24, &cap, 0);
	caption_adjust(&cap,NULL);
	cap.input_x = 7;
	sprintf((char *)cap.form, "PW KEY:");
	Menu_InputCreate(3, (char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
						cap.input_max, cap.input_min, 0, &set_val[1], NULL, NULL, NULL);
	
	while(1)
	{
		result = Menu_InputDisplay();
		if(result == MENU_RET_SAVE)	
		{
			if((INT16U)set_val[1] == cipher_key)
			{
				return;
			}
			else
				BuzOnAdd(5);
		}
		else 
			BuzOnAdd(5);
	}
}
  #endif //USE_CTS_TEMP_ENABLE

void mode_waiting_cal(void)
{
//	INT8U old_page;
	INT8U old_font;
	INT16U x, y;
	INT16U len;
	POINT disp_p;
	INT8U string_buf[33];
	CAPTION_STRUCT cap;
	
//	old_page = DspStruct.Page;

	old_font = DspStruct.Id1;
	Dsp_SetPage(DSP_DEFAULT_PAGE);
	DspLoadFont1(DSP_MENU_FONT_ID);
	KEY_SetMode(1);

	Dsp_Fill_Buffer(0);
	display_alloff_primary_indication();
	caption_split_by_code(0x97B0, &cap,0);
	//sprintf((char*)string_buf, "교정 권한이 필요합니다.");
	len = strlen((char*)cap.form);
	x = (LCD_Y_MARGIN-display_font_get_width()*len)/2;
	y = (LCD_X_MARGIN-display_font_get_height())/2;
	disp_p = point(y, x);
	Dsp_Write_String(disp_p, (char*)cap.form);
	Dsp_Diffuse();
	VFD7_Diffuse();
	
//	Dsp_SetPage(old_page);
	DspLoadFont1(old_font);

	BuzOnAdd(4);
  #ifdef USE_CTS_TEMP_ENABLE
	while(status_run.run_mode != RUN_CALIBRATION)
	{
		if(KEY_Read())
		{
			switch(KeyDrv.CnvCode){
				case KP_ZERO :
					BuzOnAdd(4);
					allow_calibration_menu();
					return;
				default :
					network_check();
			}
		}
	}
	#endif
}
#endif
