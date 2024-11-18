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
#include "key_app.h"
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

#include "mode_program.h"
#include "mode_main.h"
#include "caption_app.h"
#include "menu.h"
#include "plu_manager.h"
#include "plu_edit.h"
#include "prt_app.h"
#include "cursor.h"
#include "clerk_app.h"
#include "customer_app.h"
#include "mode_sale.h"
#include "dsp_app.h"
#include "network_set.h"

#include "discount.h"
#include "ingredient.h"
#include "nutrition_fact.h"
#include "shopname.h"
#include "tax.h"
#include "origin.h"
#include "prt_advertise.h"

#include "department.h"
#include "country.h"
#include "workplace.h"
#include "pcs_unit.h"
#include "tare_file.h"
#include "traceability.h"
#include "barcode_file.h"
#include "scale_config.h"
#include "pgm_app.h"
#include "pgm_global_set.h"
#include "sale_report.h"
#include "login.h"
#include "currency.h"
#include "initial_glb.h"
#include "inventory.h"
#include "license.h"
#include "comm_app.h"
#ifdef USE_PERSIA_CAPTION
#include "caption_IR_1256.h"
#endif

#pragma pack(2)
typedef struct {
	INT16U pms_code;
	INT8U  level;
	INT8U  pms_id;
} MENU_PMS_TABLE_STRUCT;
#pragma pack()
extern INT8U dealer_factory_mode;
extern PRINT_OPER_TYPE		PrtOper;	// Added by CJK 20051226

void mode_subprogram(INT16U code);
void menu_function(INT16U fcode);
void menu_dealer(INT16U fcode);
//DEL071127void menu_factory(INT16U fcode);
void netset_access_point(void);
INT8U ehternet_test(void);
INT8U mode_program_permission_check_code(INT16U *code);
extern void discount_del_by_department(void);
extern void discount_del_all(void);
extern void network_one2one_copy(void);


void mode_program(void)
{
	INT8U old_mode;
	INT16U code;

	DspLoadFont1(DSP_MENU_FONT_ID);

	old_mode = KEY_GetMode();
	KEY_SetMode(1);			// Set the Key in Program mode

	code = hex2bcd_word((INT16U)status_scale.cur_keyvalue);

	if(code >= 0x2000 || code < 0x1000) 
		code = 0x1000; 
#if defined(USE_USA_INGREDIENT) || defined(USE_NORTH_AMERICA_INGREDIENT)
	if(code == 0x1310 || code == 0x1311 || code == 0x1312 || code == 0x1313) 
	{
		code = 0x1300;
	}
#endif
#ifdef USE_FUNCTION_KEY_BLOCK
	if(code == 0x1880)
	{
		code = 0x1881;
	}
#endif
	display_alloff_primary_indication();
	VFD7_Diffuse();

	KEY_Set_Shift(0);
	DisplayMsgReset();

	mode_subprogram(code);

	KEY_Set_Shift(0);
	DisplayMsgReset();

	KEY_SetMode(old_mode);
	status_run.run_mode = RUN_SALE;

	status_scale.cur_keyvalue = 0;
	sale_display_update(UPDATE_CLEARALL|0x0fff);
	if(ConfirmPrintOnLabel == 2) EnablePrintOnLabel = OFF;
}

void mode_init(void)
{
//
}

#define MENUCODE_MASK_DEPTH0 0xf000
#define MENUCODE_MASK_DEPTH1 0x0f00
#define MENUCODE_MASK_DEPTH2 0x00f0
#define MENUCODE_MASK_DEPTH3 0x000f

INT16S mode_page(INT16S odeep,INT16U code,INT16S *page_cur)
{
	INT16S i;
	INT16S deep;

	if (code&MENUCODE_MASK_DEPTH3) deep = 3;
	else if (code&MENUCODE_MASK_DEPTH2) deep=2;
	else if (code&MENUCODE_MASK_DEPTH1) deep=1;
	else if (code&MENUCODE_MASK_DEPTH0) deep=0;
	for (i=deep+1; i<4; i++) page_cur[i]=0;
	if (odeep<deep) page_cur[deep]=0;
	return deep;
}

INT16U mode_get_code(INT32U addr,INT16U key)
{
	INT16U code,rcode,zcode;
	INT16U mask;
	INT16S    i;

	if (key == KEY_NUM_0) key = 10;
	else if (key >= KP_ASC_A && key <= KP_ASC_F) key = key - KP_ASC_A + 10;
	else if (key >= KP_ASC_A_L && key <= KP_ASC_F_L) key = key - KP_ASC_A_L + 10;
    
	mask=0x000f;
	code = Flash_wread(addr);
	if ((code==0x1137) || (code==0x1147)) {
		if ((key<0) || (key>3)) return 0;
		rcode=key-1;
		addr = addr + CAP_MSG_LENGTH * rcode;
		rcode=Flash_wread(addr);
		goto NEXT;
	}
	for (i=0; i<4; i++) {
		rcode = code & mask;
		if (rcode!=0) break;
		mask<<=4;
		key<<=4;
	}
	code &= ~mask;
	code |=key;
	for (i=0; i<10; i++) {
		rcode = Flash_wread(addr);
		if (rcode==0) break;
		if (rcode==code) {
			goto NEXT;
		}
		addr+=CAP_MSG_LENGTH;
	}
	rcode=0;
NEXT:	if (rcode) {
		zcode = Flash_wread(addr+CAPTION_RUNFLAG);
		if (!caption_run_check(rcode,zcode)) rcode=0;
	}
	return rcode;
}

/*
INT8U cap_mask_check(INT16U x,INT16U code)
{
#ifndef USE_GSMART_PLU
	if (x == 0x1550) return 0;	// PLU Default(Menu1550) : GSMart PLU 구조일 때만 사용
#endif
	if (status_scale.restrict&FUNC_DEPTNO) code&=0x1ff;
	if ((!status_scale.use_pcs) && (x==0x1534)) return 0;
	if ((!status_scale.use_bycount) && (x==0x1533)) return 0;
	if (GlbOper.PluReportType)
	{
		if (x==0x1145) return 0;
		else
		{
			x&=0xff0f;
			if ((x==0x1602) || (x==0x1603)) return 0;
		} 
	}
	if ((code==0) || (status_scale.restrict&code)) return 1;
	return 0;
}
*/

void mode_subprogram(INT16U code)
{
	char   data[64];

	INT16U	x,inputtype;
	INT16U  menu_id, prev_code;
	INT16U 	 mastercode, subcode,endcode, list_code,new_code;
	//		 |----------------------------------------------|
	//    code ==>>  | modecode  | mastercode | subcode  | endcode	|
	//		 |----------------------------------------------|
	INT16S	start_id, cur_id, i,j;
	INT16S	menu_cnt, page_max, deep,page_cur[4],max_line;
#ifndef USE_SINGLE_LINE_GRAPHIC
	INT16S start_line ,y_gab;
#endif
	char    permission_flag = 0, permission_state = 0;
	INT32U	addr,addr_main;
//	INT8U	flag_r;
	POINT	disp_p;
	INT16U  cap_code;
	INT8U  result;
	INT8U  report_key;
	INT8U  backup_directdraw, backup_directdraw2;

	INT8U  menu_no;
	INT8U  prev_menu_no;
	INT8U  max_id;
	INT8U  backup_pgmmode;
#ifdef CPU_PIC32MZ
	INT8U  caption_offset;
#endif
#if defined(USE_SINGLE_LINE_GRAPHIC)
	MENU_CURSOR_STR cursor;
	INT8U cursor_id;
#endif
#ifdef USE_PERSIA_DEFAULT
    char   cap_str[64];
#endif
	
	backup_directdraw = DspStruct.DirectDraw;
	//code = origin_code;
	//start_line=1;
#ifndef USE_SINGLE_LINE_GRAPHIC
	if (Startup.menu_type==0) start_line=0;
	else start_line = Startup.menu_type;
	y_gab  = display_font_get_height();
#endif

	max_line=Startup.line_max;
	prev_code = code;
	page_cur[0] = 0;
	page_cur[1] = 0;
	page_cur[2] = 0;
	page_cur[3] = 0;
	cur_id  =0;
	deep = 0;
	status_scale.restrict = get_global_wparam(GLOBAL_ACTION_ENABLE);
START:
////////
	//menu_display_menucode(code, 1);	//Added by JJANG 20081126
#ifdef USE_SINGLE_LINE_GRAPHIC
	ClearDisp7();
    display_lcd_clear();
#endif
	cap_code = code;
	report_key = 0;
	if (code == 0x1600 || code == 0x1650 || code == 0x1660);	// 1600,1650
#ifdef USE_CHN_IO_BOARD
	else if (code == 0x1670);
#endif
#ifdef DISABLE_X2Z2_REPORT
	else if ((code & 0xfff0) == 0x1630 || (code & 0xfff0) == 0x1640);
#endif
	else if ((code & 0xff00) == 0x1600)	// 1611,1612,...,1621,...,1631,...,1637
	{
		report_key = (INT8U)((code >> 4) & 0x000f);
		cap_code = code & 0x000f;
		cap_code = cap_code | 0x1610;		// 16XY => 161Y
	}
// 이후로는 162Y, 163Y, 164Y 는 161Y로 변환되어 있음 : CJK070126
////////

	if(code >= 0x2000 || code < 0x1000) {
		BuzOn(2);
		code = prev_code;
	}
	addr_main = caption_search_by_code(cap_code);
	list_code = Flash_wread(addr_main+CAPTION_MENUSTART_POINT);	// 메뉴 시작 위치
	inputtype = Flash_wread(addr_main+CAPTION_INPUTTYPE_POINT);	// 실행코드
	if(addr_main == DFLASH_BASE + FLASH_CAPTION_AREA || (list_code == 0 && inputtype == 0))  {	// No Caption(0x0000) 또는 menu caption이 아닐경우
		code = 0x1000;
		cap_code = code;
		addr_main = caption_search_by_code(cap_code);
		list_code = Flash_wread(addr_main+CAPTION_MENUSTART_POINT);
		inputtype = Flash_wread(addr_main+CAPTION_INPUTTYPE_POINT);		// 실행코드
		if ((list_code<=0) || (list_code>=0x2000)) {
			caption_error(0);
		}
	}	
// Code 복구
	if (inputtype == 0x1600 || inputtype == 0x1650 || inputtype == 0x1660);// 1600, 1650
#ifdef USE_CHN_IO_BOARD
	else if (inputtype == 0x1670);
#endif
	else if ((inputtype & 0xff00) == 0x1600)	// 1611,1612,...,1621,...,1631,...,1637
	{
		if(report_key)	// report menu (MENU CODE 16XY)
		{
			inputtype = (inputtype & ~(0x00f0));
			inputtype = (inputtype | (report_key << 4));
		}
	}
#ifdef USE_PERSIA_DEFAULT
    // param 773(Enable TAX table) N 인경우, 1541+[Menu], 1542+[Menu] 키로 Tax Table 메뉴 진입 못하게 수정
    if(!(status_scale.restrict&FUNC_TAX_TBL) && (code == 0x1541 || code == 0x1542))
    {
        code = 0x1000;
        goto START;
    }
#endif
////////////////
	if(!mode_program_permission_check_code(&code)) goto START;
	if(list_code == 0) {
		if(inputtype == 0) {
			BuzOn(2);
			caption_error(0);	// Wait & Reset
			code = prev_code;
		} else {
			backup_directdraw2 = DspStruct.DirectDraw;
			DspStruct.DirectDraw = 1;
			menu_display_menucode(code, 1);
			DspStruct.DirectDraw = backup_directdraw2;
			menu_function(inputtype);	// 실행
			prev_code = code;
			endcode=code&ENDCODE_MASK;
			if (endcode) {
				if      ((inputtype==0x1147) || (inputtype==0x1148) || (inputtype==0x1149)) code=0x1142;
				else if ((inputtype==0x1137) || (inputtype==0x1138) || (inputtype==0x1139)) code=0x1133;
				//else if ((inputtype==0x1621 || inputtype==0x1641) && !(flag_r&0x01)) code&=0xff00;
				else if ((inputtype==0x1621 || inputtype==0x1641) && !(PrtOper.IndividualClear)) code&=0xff00;
//#ifdef USE_ONLY_ZREPORT
//				else if ((inputtype==0x1611 || inputtype==0x1631)) code&=0xff00;
//#endif
				else code&=0xFFF0;
			} else {
				subcode=code&SUBCODE_MASK;
				if (subcode) code&=0xFF00;
				else {
					mastercode=code&MASTERCODE_MASK;
					if(mastercode) code&=0xF000;
				}
			}
		}
		goto START;
	}
	addr=caption_search_by_code(list_code);
	menu_cnt = 0;
	i        = 0;
	prev_menu_no = '0';
	while (1) {	// Menu 개수 검사
		x=Flash_wread(addr+CAP_MSG_LENGTH*i);
		if (x==0) break;	// "(end)" 일때
		//////////////////////////////////////////////////
		// "(end)" caption을 삭제하기 위해 다음 비교 추가	// CJK080611
		menu_no=Flash_bread(addr+CAP_MSG_LENGTH*i+CAPTION_NAME_POINT);	// 메뉴번호
		if (Flash_bread(addr+CAP_MSG_LENGTH*i+CAPTION_NAME_POINT+1) != '.') break;
		if (!ishex(menu_no) || (menu_no <= prev_menu_no && menu_no != '0')) break;	// '0'은 예외	
		//////////////////////////////////////////////////
		new_code=Flash_wread(addr+CAP_MSG_LENGTH*i+CAPTION_RUNFLAG);
		if (caption_run_check(x,new_code)) {
			menu_cnt++;
		}
		i++;
		prev_menu_no = menu_no;	// 이전 메뉴번호
		if (menu_no == '0') break;	// "0. xxxx"는 마지막 메뉴        
	}
	max_id = i;
	//if (!(flag_r&0x01)) {
//#ifdef USE_ONLY_ZREPORT
//		if(code == 0x1610) {
//			code = 0x1611;
//			goto START;
//		}
//		if(code == 0x1630) {
//			code = 0x1631;
//			goto START;
//		}
//#endif
#if defined(USE_USA_INGREDIENT) || defined(USE_NORTH_AMERICA_INGREDIENT)
	if(code == 0x1310 || code == 0x1311 || code == 0x1312 || code == 0x1313)
	{
		code = 0x1300;
		goto START;
	}
#endif
#ifndef USE_FUNCTION_KEY_BLOCK
	if(code == 0x1880)	max_id = i-1;//key block 메뉴 숨김
#endif
	
	if (!(PrtOper.IndividualClear)) {
		if(code == 0x1620) {
			code = 0x1621;
			goto START;
		}
		if(code == 0x1640) {
			code = 0x1641;
			goto START;
		}
	}
	page_max=menu_cnt/max_line;
	if (menu_cnt%max_line) page_max++;
	deep=mode_page(deep,code,page_cur);
NEWPAGE:
	if (page_cur[deep]>=page_max) page_cur[deep] = page_max-1;
	if (page_cur[deep]<0) page_cur[deep]=0;
	DspStruct.DirectDraw = 0;
	Dsp_Fill_Buffer(0);
	menu_display_menucode(code,1);
	menu_displaytitle(code,0);
	menu_writepage(page_max, page_cur[deep]+1);

	start_id=page_cur[deep] * max_line;
	j = 0;
	for (i=0; i<max_id; i++) {
		x       =Flash_wread(addr+CAP_MSG_LENGTH*i);
		new_code=Flash_wread(addr+CAP_MSG_LENGTH*i+CAPTION_RUNFLAG);
		if (caption_run_check(x,new_code)) {
			if (j>=start_id) {
				start_id=i;
				break;
			}
			j++;
		}
	}
//	end_id  =start_id + max_line - 1;
//	if (end_id>=menu_cnt) end_id=menu_cnt-1;
//	for (i=start_id; i<=end_id; i++) {
	j=0;
	caption_offset = 0;
#if !defined(_USE_LCD20848_) && !defined(_USE_LCD32_)
	caption_offset = 3;
#endif
	for (i=start_id; i<max_id; i++) {
		x       =Flash_wread(addr+CAP_MSG_LENGTH*i);
		if (x==0) break;
		new_code=Flash_wread(addr+CAP_MSG_LENGTH*i+CAPTION_RUNFLAG);
		if (caption_run_check(x,new_code)) {
			Flash_sread(addr+i*CAP_MSG_LENGTH+2+caption_offset,(HUGEDATA INT8U *)data,34);
			data[34]=0;
#ifdef USE_SINGLE_LINE_GRAPHIC
			display_clear(DISPLAY_UNITPRICE);
			display_string(DISPLAY_UNITPRICE,(INT8U *)data);
#else
			disp_p = point(y_gab*(j+start_line), 0);
#ifdef HEBREW_FONT
			Convert_HebrewPreview((INT8U *)data, strlen(data), 0, 0); 
			hebrew_codetemp[strlen(data)] = 0;
			disp_p.x = 202 - disp_p.x;
			Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
  #ifdef USE_PERSIA_DEFAULT
            if((x > 0x1610 ) && (x < 0x1618)) // caption 0x1611, 1612, 1613 ..  (menu 161x, menu 163x 는 같은 캡션을 사용)
            {
                if(code == 0x1610) // menu code 1611, 1612, 1613, 1614  ..
                {
                    sprintf(cap_str,"%d. %s X1 %s", i+1, Caption_Report, &data[3]);
                }
                else if (code == 0x1630) // menu code 1631, 1632, 1633, 1634  ..
                {
                    sprintf(cap_str,"%d. %s X2 %s", i+1, Caption_Report, &data[3]);
                }
                Convert_ArabPreview((INT8U *)cap_str, strlen(cap_str), 0, 0, 0); 
			    arab_codetemp[strlen(cap_str)] = 0;
            }
            else
            {
			    Convert_ArabPreview((INT8U *)data, strlen(data), 0, 0, 0); 
			    arab_codetemp[strlen(data)] = 0;
            }
			disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
			Dsp_Write_1stringLong(disp_p, arab_codetemp);
  #else
            Convert_ArabPreview((INT8U *)data, strlen(data), 0, 0, 0); 
			arab_codetemp[strlen(data)] = 0;
			disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
			Dsp_Write_1stringLong(disp_p, arab_codetemp);
  #endif            
#else
			Dsp_Write_String(disp_p,data);
#endif
#endif
			j++;
			if (j>=max_line) break;
		}        
#ifdef USE_PERSIA_DEFAULT
        else
        {
            if(KeyDrv.CnvCode == KS_MODE)  // 권한 없는 메뉴를 메뉴키로 진입 시도할 경우, 1000번 메뉴로 이동하게 수정
            {
                code = 0x1000;
                goto START;
            }
        }       
#endif
	}
#ifdef USE_SINGLE_LINE_GRAPHIC
	display_clear(DISPLAY_PRICE);
#endif
	Dsp_Diffuse();
	menu_id = 0;
	new_code = 0;
	while (1) {
		backup_pgmmode = KeyDrv.PgmMode;
		KeyDrv.PgmMode = 0;	// => English Character
		if(Startup.country == COUNTRY_RU) KeyDrv.PgmMode = 1;	// => English Character
#if defined(USE_SINGLE_LINE_GRAPHIC)
		cursor_init(&cursor, KI_TARE, 0, 0);
		cursor_id = Flash_bread(addr+CAP_MSG_LENGTH*start_id+CAPTION_NAME_POINT);
		cursor_ready(&cursor, CURSOR_TYPE_DELETE, 0, deep + 1, cursor_id, 0);
#endif
		while (!KEY_Read()) {
#if defined(USE_SINGLE_LINE_GRAPHIC)
			cursor_blink(&cursor);
#endif
		}
		KeyDrv.PgmMode = backup_pgmmode;
		result = 0;
		BuzOn(1);
		switch(KeyDrv.Type) {
			case KEY_TYPE_NUMERIC :
			case KEY_TYPE_ASCII :
				cur_id = KeyDrv.CnvCode;
				new_code=mode_get_code(addr,cur_id);
				if (new_code) {
//					if(report_pt)
//					{
//						new_code = (new_code & ~(0x000f << report_pt));
//						new_code = (new_code | (cur_id << report_pt));
//					}
//					prev_code = code;
//					code      = new_code;
					result = 1;//goto START;
				} else {
					BuzOn(2);
				}
				break;
			case KEY_TYPE_FUNCTION :
				switch(KeyDrv.CnvCode) {
					case KP_DEALER	 : 
					case KP_ZERO:
						if (code == 0x1000){
							dealer_factory_mode = DEALER_MODE;
							CurMenuCode = code;
							menu_dealer(menu_id); 
							dealer_factory_mode = 0;	// Added by CJK 20060412
						}
						goto START;
					case KP_CLEAR	 : menu_id = 0; break;//goto RE1;
					case KP_PAGE_DN  :
					case KP_ARROW_DN : 
						page_cur[deep]++; 
						result = 2;//goto NEWPAGE;
						break;
					case KP_PAGE_UP  :
					case KP_ARROW_UP : 
						page_cur[deep]--; 
						result = 2;//goto NEWPAGE;
						break;
					case KP_ESC :
						if(code == 0x1000) {
							if(Cal_flagAdmBackup==1) {
								ADM_BackupEeprom();			//
								Cal_flagAdmBackup = 0;
							}
							DspStruct.DirectDraw = backup_directdraw;
							return;
						} else {
							endcode=code&ENDCODE_MASK;
							if (endcode) new_code = code & 0xfff0;//code&=0xFFF0;
							else {
								subcode=code&SUBCODE_MASK;
								if (subcode) new_code = code & 0xff00;//code&=0xFF00;
								else {
									//mastercode=code&MASTERCODE_MASK;
									//if(mastercode) new_code = code & 0xf000;//code&=0xF000;
									new_code = code & 0xf000;//code&=0xF000;
								}
							}
							result = 1;//goto START;
						}
						break;
					case KP_FEED :
						keyapp_feed(OFF);
						break;
#ifdef CPU_PIC32MZ
					case KP_ENTER :
						cur_id = Flash_bread(addr+CAP_MSG_LENGTH*start_id+CAPTION_NAME_POINT);
						cur_id -= '0';
						new_code=mode_get_code(addr,cur_id);
						if (new_code) {
							result = 1;//goto START;
						} else {
							BuzOn(2);
						}
						break;
#endif
					default :
						BuzOn(2);
						break;
				}
				break;
			default :
				BuzOn(2);
				break;
		}
		if(result == 1)
		{	
			if (new_code == 0x1600);			// 1600
			else if ((new_code & 0xff00) == 0x1600)	// 1611,1612,...,1621,...,1631,...,1637
			{
				if(report_key)	// report menu (MENU CODE 16XY)
				{
					new_code = (new_code & ~(0x00f0));
					new_code = (new_code | (report_key << 4));
				}
			}
			prev_code = code;
			code      = new_code;
			goto START;
		}
		else if(result == 2)
		{
			goto NEWPAGE;
		}
#if !defined(USE_SINGLE_LINE_GRAPHIC)
		menu_display_menucode(code,1);
#endif
	}
}

extern void clerk_list(void);
extern void customer_list(void);

void menu_function(INT16U fcode)
{
	char data[8];

	CurMenuCode = fcode;
	switch (fcode) 
	{
#if !defined(USE_PRICE_CHANGE_PROHIBITED) && !defined(USE_PROHIBITE_PRICE_CHANGE_IN_PLU_MENU)
	case 0x1110: plu_unit_price_change();	break;
#endif
	case 0x1120: plu_new();       		break;

	case 0x1131: discount_create();		break;
#ifndef DISABLE_LIST_MENU
	case 0x1132: discount_list();		break;
#endif
	case 0x1137: discount_delete();		break;
	case 0x1138: discount_del_by_department();  break;
	case 0x1139: discount_del_all();	break;

	case 0x1141: plu_copy();		break;

	case 0x1147: plu_del_by_plu_no();	break;
	case 0x1148: plu_del_by_department();	break;
	case 0x1149: plu_del_all();		break;
	case 0x1143: plu_move();		break;

//	case 0x1144: plu_inhibit_all();		break;
	case 0x1144: plu_inhibit_step();	break;
	case 0x1145: plu_mgr_trancnt();		break;

#ifndef DISABLE_LIST_MENU
	case 0x1150: plu_list();		break;
#endif
	case 0x1160: plu_speed_key_define();	break;
	case 0x1170: plu_test_label_flag(); 	break;

#ifdef USE_INDIA_FUNCTION
	case 0x1181: inventory_input(); 	break;
	case 0x1182: inventory_edit(); 		break;
#ifndef DISABLE_LIST_MENU
	case 0x1183: inventory_list(); 		break;
#endif
#endif
	case 0x1211: department_create();	break;
#ifndef DISABLE_LIST_MENU
	case 0x1212: department_list();		break;
#endif
	case 0x1213: department_delete();	break;

#ifdef USE_TRACE_MEATGRADE
	case 0x1221: trace_meatgrade_create();	break;
#ifndef DISABLE_LIST_MENU
	case 0x1222: trace_meatgrade_list();	break;
#endif
	case 0x1223: trace_meatgrade_delete();	break;
#else
	case 0x1221: group_create();		break;
#ifndef DISABLE_LIST_MENU
	case 0x1222: group_list();		break;
#endif
	case 0x1223: group_delete();		break;
#endif

#ifdef USE_TAX_RATE    
	case 0x1231: tax_create();		break;
#ifndef DISABLE_LIST_MENU
	case 0x1232: tax_list();		break;
#endif
	case 0x1233: tax_delete();		break;
#endif
    
	case 0x1241: label_message_create();   	break;
#ifndef DISABLE_LIST_MENU
	case 0x1242: label_message_list();	break;
#endif
	case 0x1243: label_message_delete();	break;
	case 0x1251: origin_create();		break;
#ifndef DISABLE_LIST_MENU
	case 0x1252: origin_list();		break;
#endif
	case 0x1253: origin_delete();		break;
	case 0x1261: barcodefile_create();	break;
#ifndef DISABLE_LIST_MENU
	case 0x1262: barcodefile_list();	break;
#endif
	case 0x1263: barcodefile_delete();	break;
#ifdef USE_QR_ADDTOTAL
	case 0x1264: barcodefile_QR_cofig();	break;
#endif
#ifdef	USE_PROHIBIT_PRESET_TARE
	// 스리랑카 현지 인증관련 Tare기능 캡션수정 및 Block 처리
#else	// USE_PROHIBIT_PRESET_TARE
	case 0x1271: tarefile_create();		break;
  #ifndef DISABLE_LIST_MENU
	case 0x1272: tarefile_list();		break;
  #endif
	case 0x1273: tarefile_delete();		break;
	case 0x1274: tare_key_define();		break;
#endif	// USE_PROHIBIT_PRESET_TARE
	case 0x1281: pcsunit_create();		break;
#ifndef DISABLE_LIST_MENU
	case 0x1282: pcsunit_list();		break;	
#endif
	case 0x1283: pcsunit_delete();		break;
#if defined(USE_USA_INGREDIENT) || defined(USE_NORTH_AMERICA_INGREDIENT)
#else
	case 0x1311: ingredient_create();	break;
  #ifndef DISABLE_LIST_MENU
	case 0x1312: ingredient_list();		break;
  #endif
	case 0x1313: ingredient_delete();	break;
#endif // defined(USE_USA_INGREDIENT) || defined(USE_NORTH_AMERICA_INGREDIENT)

#ifdef USE_NUTRITION_FACT
	case 0x1321: nutri_create();		break;
#ifndef DISABLE_LIST_MENU
	case 0x1322: nutri_list();		break;
#endif
	case 0x1323: nutri_delete();		break;
#endif

#ifdef USE_TRACEABILITY //gm 090407, memory 충돌 방지
	case 0x1331: traceability_create();	break;
#ifndef DISABLE_LIST_MENU
	case 0x1332: traceability_list();	break;
#endif
	case 0x1333: traceability_delete();	break;

	case 0x1341: country_create();		break;
#ifndef DISABLE_LIST_MENU
	case 0x1342: country_list();		break;
#endif
	case 0x1343: country_delete();		break;

	case 0x1351: slaughter_create();	break;	
#ifndef DISABLE_LIST_MENU
	case 0x1352: slaughter_list();		break;	
#endif
	case 0x1353: slaughter_delete();	break;	

	case 0x1361: cuttinghall_create();	break;	
#ifndef DISABLE_LIST_MENU
	case 0x1362: cuttinghall_list();	break;	
#endif
	case 0x1363: cuttinghall_delete();	break;	
#endif

#ifdef USE_CHN_CART_SCALE
	case 0x1331: meat_trace_create();	break;
#endif

//SG060718
#ifdef  USE_TRACE_STANDALONE
	case 0x1371: individualData_create(1);	break;	
#ifndef DISABLE_LIST_MENU
	case 0x1372: individualData_list();	break;	
#endif
	case 0x1373: individualData_delete();	break;	
	case 0x1374: individualData_assign();	break;	

//#ifdef USE_GSMART_PLU	
	case 0x1381: trace_specialuse_create();	break;	
#ifndef DISABLE_LIST_MENU
	case 0x1382: trace_specialuse_list();	break;	
#endif
	case 0x1383: trace_specialuse_delete();	break;
	
	case 0x1391: trace_slaughthouse_create();	break;	
#ifndef DISABLE_LIST_MENU
	case 0x1392: trace_slaughthouse_list();	break;	
#endif
	case 0x1393: trace_slaughthouse_delete();	break;	
#ifdef USE_TRACE_MEATPART
	case 0x13A1: trace_meatpart_create();	break;	
#ifndef DISABLE_LIST_MENU
	case 0x13A2: trace_meatpart_list();	break;	
#endif
	case 0x13A3: trace_meatpart_delete();	break;
#endif
//#endif
#endif			

	case 0x1411: store_name_create();	break;
#ifndef DISABLE_LIST_MENU
	case 0x1412: store_name_list();		break;
#endif
	case 0x1413: store_name_delete();	break;

	case 0x1421: customer_edit(); 		break;		//Add or Edit Customer
#ifndef DISABLE_LIST_MENU
	case 0x1422: customer_list(); 		break;
#endif
	case 0x1423: customer_delete_proc();	break;
	case 0x1431: scroll_message_config(); 	break;		// in prt_advertise.c
	case 0x1432: scroll_message_input(); 	break;		// in prt_advertise.c
#ifndef DISABLE_LIST_MENU
	case 0x1433: scroll_message_list(); 	break;		// in prt_advertise.c
#endif
//	case 0x1434: scroll_message_delete(); 	break;		// in prt_advertise.c

	case 0x1441: currency_create();		break;
#ifndef DISABLE_LIST_MENU
	case 0x1442: currency_list();		break;
#endif
	case 0x1443: currency_delete();		break;

	//SG061219. Job Batch Schedule
	case 0x1451: job_batch_create();	break;
#ifndef DISABLE_LIST_MENU
	case 0x1452: job_batch_list();		break;
#endif
	case 0x1453: job_batch_delete();	break;

	//SG070124. Scanner Mapping Table
	case 0x1461: scanner_convtable_create();	break;
#ifndef DISABLE_LIST_MENU
	case 0x1462: scanner_convtable_list();		break;
#endif
	case 0x1463: scanner_convtable_delete();	break;
	case 0x1464: scanner_config();	break;

	//SG070221. Label Format Short Key
	case 0x1470: label_format_key_table();		break;

//#ifdef USE_ONLY_ZREPORT
//	case 0x1611 : Report_Select(REPORT_MODE_Z, 0);		break;
//#else	
	case 0x1611 : Report_Scale(REPORT_MODE_X, 0);		break;
	case 0x1612 : Report_PLUs(REPORT_MODE_X, 0, REPORT_PLU);	break;
	case 0x1613 : Report_PLUs(REPORT_MODE_X, 0, REPORT_MISCPLU);	break;
	case 0x1614 : Report_Menu(REPORT_MODE_X, 0, REPORT_GROUP);	break;
	case 0x1615 : Report_Menu(REPORT_MODE_X, 0, REPORT_DEPT); 	break;
	case 0x1616 : Report_Menu(REPORT_MODE_X, 0, REPORT_HOURLY);	break;
	case 0x1617 : Report_Menu(REPORT_MODE_X, 0, REPORT_CLERK);	break;
#ifdef USE_INDIA_TAX_REPORT_MENU
	case 0x1618 : Report_Tax(REPORT_MODE_X, 0);		break;
#endif
//#endif
	case 0x1621 : Report_Scale(REPORT_MODE_Z, 0);		break;
	case 0x1622 : Report_PLUs(REPORT_MODE_Z, 0, REPORT_PLU);	break;
	case 0x1623 : Report_PLUs(REPORT_MODE_Z, 0, REPORT_MISCPLU);	break;
	case 0x1624 : Report_Menu(REPORT_MODE_Z, 0, REPORT_GROUP);	break;
	case 0x1625 : Report_Menu(REPORT_MODE_Z, 0, REPORT_DEPT); 	break;
	case 0x1626 : Report_Menu(REPORT_MODE_Z, 0, REPORT_HOURLY);	break;
	case 0x1627 : Report_Menu(REPORT_MODE_Z, 0, REPORT_CLERK);	break;
#ifdef USE_INDIA_TAX_REPORT_MENU
	case 0x1628 : Report_Tax(REPORT_MODE_Z, 0);		break;
#endif

#ifndef DISABLE_X2Z2_REPORT
	case 0x1631 : Report_Scale(REPORT_MODE_X, 1);		break;
	case 0x1632 : Report_PLUs(REPORT_MODE_X, 1, REPORT_PLU);	break;
	case 0x1633 : Report_PLUs(REPORT_MODE_X, 1, REPORT_MISCPLU);	break;
	case 0x1634 : Report_Menu(REPORT_MODE_X, 1, REPORT_GROUP);	break;
	case 0x1635 : Report_Menu(REPORT_MODE_X, 1, REPORT_DEPT); 	break;
	case 0x1636 : Report_Menu(REPORT_MODE_X, 1, REPORT_HOURLY);	break;
	case 0x1637 : Report_Menu(REPORT_MODE_X, 1, REPORT_CLERK);	break;
#ifdef USE_INDIA_TAX_REPORT_MENU
	case 0x1638 : Report_Tax(REPORT_MODE_X, 1); 	break;
#endif

	case 0x1641 : Report_Scale(REPORT_MODE_Z, 1);		break;
	case 0x1642 : Report_PLUs(REPORT_MODE_Z, 1, REPORT_PLU);	break;
	case 0x1643 : Report_PLUs(REPORT_MODE_Z, 1, REPORT_MISCPLU);	break;
	case 0x1644 : Report_Menu(REPORT_MODE_Z, 1, REPORT_GROUP);	break;
	case 0x1645 : Report_Menu(REPORT_MODE_Z, 1, REPORT_DEPT); 	break;
	case 0x1646 : Report_Menu(REPORT_MODE_Z, 1, REPORT_HOURLY);	break;
	case 0x1647 : Report_Menu(REPORT_MODE_Z, 1, REPORT_CLERK);	break;
#endif
	case 0x1650 : Report_ClearAll();			break;
	case 0x1660 : Report_SendingTransaction();		break;
#ifdef USE_CHN_CART_SCALE
#ifdef USE_CHN_IO_BOARD
	case 0x1670 : keyapp_usb_save_log();			break;
#endif
#endif
	///////////////////

	// Added by CJK
	case 0x1710: print_inhibit_set();	break;
	case 0x1720: print_markdown_set();	break;
	/////////////////

	case 0x1731: printer_mode();		break; // Modified by CJK 20050927
	case 0x1732: //printer_labelset();	 
		if(PrtStruct.Mode)	printer_receiptset();
		else 			printer_labelset();
		break;
	case 0x1733: printer_sensor();		break;
	case 0x1734: printer_settings();	break;
	case 0x1735: printer_stb_time();	break;
	case 0x1736: adjust_feed_length();	break;
	case 0x1737: printer_preprint_setting();break;
	case 0x1738: adjust_reverse_feed_length();	break;			//110428 KKH for test

	case 0x1740: input_serial_no();		break;

	// Added by CJK 20040712
	case 0x1750: global_set_addup_print_condition();	break;
	case 0x1761: global_set_clerk_print_ticket_item();	break;
	case 0x1762: global_set_clerk_print_list_item();	break;
	case 0x1763: global_set_clerk_print_font_size();	break;


	case 0x1810: scale_set_salemode(); break;
	case 0x1820: scale_set_operationmode(); break;
	case 0x1830: scale_set_name();		break;
#ifndef USE_HIDE_DATETIME_MENU
	case 0x1840: keyapp_date(0);	  	break; 
#endif

	case 0x1851: clerk_new(); 		break;
	case 0x1852: clerk_password_change();	break; // 20040318
#ifndef DISABLE_LIST_MENU
	case 0x1853: clerk_list();  		break;
#endif
	case 0x1854: clerk_delete();		break;  
	case 0x1855: manager_permission_edit(); break;
	case 0x1856: clerk_key_define();	break;

	case 0x1861: CalRun_DisplayTest();	break;
	case 0x1862: CalRun_TestAD_forUser();	break;
	case 0x1863: CalRun_KeyTest();		break;
	case 0x1864: printer_test();		break;
	case 0x1865: printer_sensor_test();	break;
#if defined(CPU_PIC32MZ) && ( defined(CL5200_PROJECT) || defined(CL5500_PROJECT))
	case 0x1866: CalRun_MemoryInformation();break;
#endif
	case 0x1867: CalRun_FirmwareVersion();	break;
	case 0x1868: CalRun_ComTest();		break;

	case 0x1871: scale_parameter_display();	   break;
	case 0x1872: global_printingoperation(0);  break;
	case 0x1873: scale_parameter_scalesetup(); break;
	case 0x1874: scale_parameter_logoff();	   break;
#ifdef USE_USB_DATA_ROM_UPDATE    
	case 0x1877: backup_data();    break;
	case 0x1878: restore_data();    break;
#endif    
#ifdef USE_MORNITORING_CNT
	case 0x1879: CalRun_Scale_Info();	 break;
#endif
	case 0x1880:
	case 0x1881: key_set_function(); 	break;
#ifdef USE_FUNCTION_KEY_BLOCK
	case 0x1882: key_set_onoff_function(); 	break;
#endif
#ifdef USE_MANAGE_LICENSE
	case 0x1890: lic_manage(); 	break;
#else
	#ifdef USE_INIT_MASK
	case 0x1891: partial_keep_init_set(); break;
	#endif
	#ifdef USE_PARTIAL_MEM_INIT
	case 0x1892: partial_init_sel_plu(); break;
	case 0x1893: partial_init_cus_label(); break;
	case 0x1894: partial_init_scale_param(); break;
	case 0x1895: partial_init_clerk_perm(); break;
	case 0x1896: partial_init_scanner(); break;
	case 0x1897: partial_init_prt_set(); break;
	case 0x1898: partial_init_net_set(); break;
	#endif
#endif
	case 0x1911 : netset_servicetype();	break;
	case 0x1912 : netset_dhcp_set();	break;
	case 0x1913 : netset_local_ip_set();	break;
	case 0x1914 : netset_remote_ip_set();	break;
	case 0x1915 : netset_rs232c_set();	break;
	case 0x1916 : netset_access_point();	break;
//	case 0x1917 : netset_wlan_config();	break;
	case 0x1920 : global_application_set();	break;
	// bmlim
//	case 0x1921 : netset_plu_transfer_all();  break;
//	case 0x1922 : netset_plu_transfer_dept(); break;
//	case 0x1923 : netset_send_block(); break;
//	case 0x1924 : netset_send_speedkey(); break;
//	case 0x1930 : netset_plu_delete(); break;
#ifndef NOT_USE_NETSET_LOCK
	case 0x1930 : netset_lock(); break;
#endif
//#ifndef DISABLE_LIST_MENU
	case 0x1940 : netset_list(); break;
//#endif
	case 0x1950 : network_one2one_copy(); break;
	case 0x1961 : netset_slave_create(); break;
	case 0x1962 : netset_slave_list(); break;
	case 0x1963 : netset_slave_delete(); break;
	default:      if (global_function(fcode)==FALSE) {
				sprintf(data,"%04X",fcode);	
				display_string(1,(INT8U *) &data);
				VFD7_Diffuse();
				Delay_100ms(5);
				sprintf(data," clp ");	
				display_string(1,(INT8U *) &data);
		      }
		      break;
	}
}

void menu_dealer(INT16U fcode)
{
	if(!permission_check(PMS_DEALER,0))	return;
	pgm_parameter_function(fcode);
	status_scale.restrict = get_global_wparam(GLOBAL_ACTION_ENABLE);
}

//DEL071127void menu_factory(INT16U fcode)
//DEL071127{
//DEL071127	// <CODE> JHC, 040329, -DebugCode
//DEL071127	// [
//DEL071127	// $1: JHC, 040329, Comment
//DEL071127	// display_ulong(2, fcode, 0);
//DEL071127	// VFD7_Diffuse();
//DEL071127	// ]
//DEL071127
//DEL071127	pgm_parameter_function(fcode);
//DEL071127}


void mode_return(INT16U code)
{
     mode_subprogram(code);	
}



INT16U mask_menu_code[4] = {0xf000, 0xff00, 0xfff0, 0xffff};
MENU_PMS_TABLE_STRUCT menu_pms_table[] = {
// pms_code는 올림차순으로 정리해야함
//		pms_code,	level,	pms_id			
	{	0x1100,		1,	PMS_EDIT_PLU		},
	{	0x1120,		1,	PMS_EDIT_PRICE		},	// Added by CJK 20050728
//	{	0x1140,		2,	PMS_MANAGEMENT		},	//사용안함.
	{	0x1144,		3,	PMS_KEY_INHIBIT		},//
	{	0x1200,		1,	PMS_DATA_TABLE1		},
	{	0x1230,		2,	PMS_TAX_RATE		},
	{	0x1300,		1,	PMS_DATA_TABLE2		},
	{	0x1400,		1,	PMS_STORE_DATA		},
	{	0x1500,		1,	PMS_GLOBAL		},
	{	0x1610,		2,	PMS_X_REPORT		},
	{	0x1620,		2,	PMS_Z_REPORT		},
	{	0x1630,		2,	PMS_X_REPORT		},
	{	0x1640,		2,	PMS_Z_REPORT		},
	{	0x1700,		1,	PMS_PRINT_SET		},
	{	0x1800,		1,	PMS_SCALE_CONFIG	},
	{	0x1820,		2,	PMS_OPER_MODE		},
//	{	0x1852,		3,	PMS_CHG_PASSWORD	},
	{	0x1870,		2,	PMS_USER_PARAMETER	},
	{	0x1880,		2,	PMS_SCALE_CONFIG	},
	{	0x1890,		2,	PMS_INITIALIZATION	},
	{	0x1900,		1,	PMS_COMMUNICATION	},
	{	0x0000,		0,	0	}	// End Code
};

INT8U mode_program_permission_check_code(INT16U *code)
{
	//INT16U temp_code;
	INT8U i;

	for(i = 0; i < 255; i++) {
		if(menu_pms_table[i].pms_code == 0x0000) break;
		if(menu_pms_table[i].level == 0 || menu_pms_table[i].level > 3) continue;
		if((*code&mask_menu_code[menu_pms_table[i].level]) == menu_pms_table[i].pms_code) {	// X1 Report
			if(!permission_check(menu_pms_table[i].pms_id,0)) {
				*code = *code&mask_menu_code[menu_pms_table[i].level-1];
				return OFF;
			}
		}
	}
	return ON;

}
