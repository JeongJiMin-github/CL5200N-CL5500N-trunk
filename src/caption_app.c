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
#include "key_typedef.h"
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
#include "dsp_app.h"
#include "caption_app.h"
#include "key_app.h"
#include "menu.h"
#include "pgm_app.h"
#include "network_command.h"
#include "adm.h"

//extern INT32U plu_get_display_info(INT16U id,CAPTION_STRUCT *cap,INT8U x);
extern char	gmsg[];
INT32U caption_start_addr[16];
// 2000,3000,4000,7000,8000,9000,C000,F000
INT8U FlagCapAdjust_RightAlign = OFF;
INT8U caption_continuous_search = OFF;
INT32U caption_continuous_search_addr = 0;

#ifdef DISABLE_LIST_MENU
INT16U cap_code_list_menu[] =
{
	0x1132,0x1150,0x1183,
	0x1212,0x1222,0x1222,0x1232,0x1242,0x1252,0x1262,0x1272,0x1282,
	0x1312,0x1322,0x1332,0x1342,0x1352,0x1362,
	0x1412,0x1422,0x1433,0x1442,0x1452,0x1462,
	0x1853,
	//0x1940,
	//0x1962
};

INT8U caption_run_check_list_menu(INT16U code)
{
	INT8U i;
	for (i = 0; i < sizeof(cap_code_list_menu)/2; i++)
	{
		if (code == cap_code_list_menu[i]) return 0;
	}
	return 1;
}
#endif

// return value
//  0 : 해당 caption 사용 안함
//  1 : 해당 caption 사용함
// status_scale.restrict의 해당 bit가 1일 경우 해당 function 사용 가능, 0일 경우 사용 불가
INT8U   caption_run_check(INT16U code,INT16U runflag)
{
	//INT16S i;
	INT16U temp;


#ifndef USE_GSMART_PLU
	if (code == 0x1550) return 0;	// PLU Default(Menu1550) : GSMart PLU 구조일 때만 사용
#endif
#ifndef USE_TRACE_STANDALONE
	if (runflag&FUNC_TRACE_KOR) return 0;
#endif
#if defined(USE_USA_INGREDIENT) || defined(USE_NORTH_AMERICA_INGREDIENT)
	if(code == 0x1310) return 0;
#endif
#ifndef USE_NUTRITION_FACT
	if(code == 0x1320) return 0;
#endif
#ifndef USE_MANAGE_LICENSE
	/* CL7200에서만 사용했었으나 더 이상 사용하지 않음 */
	//if (code == 0x1890) return 1;
#endif
#ifdef USE_EMART_DEFAULT
	if(code == 0x13A0) return 0;
#endif
#ifdef NOT_USE_NETSET_LOCK
	if(code == 0x1930) return 0;
#endif
#ifdef USE_HIDE_DATETIME_MENU
	if(code == 0x1840) return 0;
#endif
#ifdef DISABLE_LIST_MENU
	if (!caption_run_check_list_menu(code)) return 0;
#endif
	//if (status_scale.restrict&FUNC_DEPTNO) runflag&=0x1ff;
	if ((code==0x1534) && (!status_scale.use_pcs)) return 0;
	if ((code==0x1533) && (!status_scale.use_bycount)) return 0;
	
	if (GlbOper.PluReportType)
	{
		if (code == 0x1145) return 0;
		else
		{
			temp = code&0xff0f;
			if ((temp == 0x1602) || (temp == 0x1603)) return 0;
		} 
	}

	if (runflag==0) return 1;
	/*
	mask=0x8000;
	for (i=0; i<16; i++) {
		if ((status_scale.restrict&mask) && (runflag&mask)) return 1;
		mask>>=1;
	}
	*/
	if ((status_scale.restrict&runflag) == runflag) return 1;
	return 0;
}


void    caption_adjust(CAPTION_STRUCT *cap,char *format)
{
	INT16S k,sz,j;
	INT16U form_len, dsp_len;

	dsp_len = Dsp_GetChars();
	if (dsp_len > CAPTION_STR_LENGTH - 1) dsp_len = CAPTION_STR_LENGTH - 1;	// "-1" : null위치 확보 
#ifndef USE_SINGLE_LINE_GRAPHIC
	form_len = strlen((char *)cap->form);
#endif

	if (cap->input_itype == 'N')
	{
		if (cap->input_dlength < cap->input_length) cap->input_dlength = cap->input_length;
	}
	if (cap->input_dlength > cap->input_length) cap->input_dlength = cap->input_length;
	if (cap->input_dlength <= 0) cap->input_dlength = 2;	// 최소 기본 2byte

#ifdef USE_SINGLE_LINE_GRAPHIC
	if (dsp_len >= cap->input_dlength) {	// 입력창 크기가 충분할 경우 
		if (dsp_len < cap->input_length) cap->input_dlength = dsp_len;
		else cap->input_dlength = cap->input_length;
	} else {	// 입력창 크기가 부족할 경우 
		if (cap->input_itype == 'S') {	// string일 경우
			if (dsp_len < 1) {	// 입력창 크기가 1byte도 안될 경우, 1byte입력 허용
				cap->input_dlength = 1;
			} else {					// 입력창 크기가 1byte이상일 경우, 남은 byte만 입력 허용
				cap->input_dlength = dsp_len;
			}
		} else {	// number일 경우
			if (dsp_len < cap->input_dlength) {
				cap->input_dlength = dsp_len;
			}
		}
	}
#else
	if (dsp_len - 2 - 1 < form_len) {	// 최소 1byte 공간이 안되는 경우
		cap->form[dsp_len - 2 - 1] = 0;	// '[',']'와 1byte의 공간만 남겨둠
		form_len = strlen((char *)cap->form);
	}
	if (dsp_len - form_len - 2 >= cap->input_dlength) {	// 입력창 크기가 충분할 경우 
		if (FlagCapAdjust_RightAlign) {
			str_fill((char *)cap->form, dsp_len - cap->input_length - 2);
			form_len = dsp_len - cap->input_length - 2;
		}
	} else {	// 입력창 크기가 부족할 경우 
		if (cap->input_itype == 'S') {	// string일 경우
			if ((INT16S)(dsp_len - form_len - 2) < 1) {	// 입력창 크기가 1byte도 안될 경우, 1byte입력 허용
				cap->input_dlength = 1;
				form_len = (INT8U)(dsp_len-cap->input_dlength-2);
				cap->form[form_len] = 0;
			} else {					// 입력창 크기가 1byte이상일 경우, 남은 byte만 입력 허용
				cap->input_dlength = dsp_len - form_len - 2;
			}
		} else {	// number일 경우
			//cap->input_dlength = cap->input_length;
			if (dsp_len - 2 < cap->input_dlength) {
				cap->input_dlength = dsp_len - 2;
			}
			form_len = (INT16U)(dsp_len - cap->input_dlength - 2);
			cap->form[form_len] = 0;
		}
		//strcat((char *)cap->form, "[");
	}

#ifdef HEBREW_FONT
	strcat((char *)cap->form,"]");
#else
	strcat((char *)cap->form,"[");
#endif
	cap->input_x = form_len + 1; 
#ifdef USE_ARAB_CONVERT
	form_len = Convert_ArabStrLength((INT8U *)cap->form);
#elif defined(USE_THAI_FONT)
    form_len = Cvt_ThaiStrLength((INT8U*)cap->form);
#endif
	if (cap->input_itype=='I') {
		cap->input_dlength = 15;
#ifdef HEBREW_FONT
		strcat((char *)cap->form,"   .   .   .   [");
#else
		strcat((char *)cap->form,"   .   .   .   ]");
#endif
		return;
	}
		   
	sz=sizeof(cap->form)-2;
	if (format==NULL) {
	    for (j=0; j<cap->input_dlength; j++) {
		k=cap->input_x+j;
		if (k>=sz) break;
		cap->form[k]=0x20;
	    }
	    k=cap->input_x+cap->input_dlength;
	    if (k>=sz) {
		k=sz;
		cap->input_dlength=k-cap->input_x;
	    }
	} else {
	    strcat((char *)cap->form,(char *)format);
	    k=strlen((char *)cap->form);
	}
#ifdef HEBREW_FONT
	cap->form[k++]='[';
#else
	cap->form[k++]=']';
#endif
#ifdef USE_ARAB_CONVERT
	cap->input_x = form_len;
	cap->form[strlen((char *)cap->form)]=0;
#elif defined(USE_THAI_FONT)
	cap->input_x = form_len;
	cap->form[strlen((char *)cap->form)]=0;
#else
	cap->form[k  ]=0;
#endif
#endif
}

void    caption_adjust_ref(CAPTION_STRUCT *cap,char *format,INT16U ref_code)
{
	INT16S k,sz,j;
	CAPTION_STRUCT 	ref_cap;
#ifdef USE_THAI_FONT
	INT8U dspstrlen;
#endif

	caption_split_by_code(ref_code, &ref_cap,0);     			// Decide inpput boundary

	cap->input_dlength =ref_cap.input_dlength;
	cap->input_length  =ref_cap.input_length;
	cap->input_min     =ref_cap.input_min;
	cap->input_max     =ref_cap.input_max;

#ifdef HEBREW_FONT
	strcat((char *)cap->form,"]");
#else
	strcat((char *)cap->form,"[");
#endif
	cap->input_x=strlen((char *)cap->form);
#ifdef USE_THAI_FONT
	dspstrlen = Cvt_ThaiStrLength((INT8U*)cap->form);
#endif  
	if (cap->input_itype=='I') {
		cap->input_dlength = 15;
#ifdef HEBREW_FONT
		strcat((char *)cap->form,"   .   .   .   [");
#else
		strcat((char *)cap->form,"   .   .   .   ]");
#endif
#ifdef USE_THAI_FONT
		cap->input_x=dspstrlen;
#endif
		return;
	}
	cap->input_dlength = Dsp_GetChars()-cap->input_x-1;
	if (cap->input_dlength>cap->input_length) cap->input_dlength=cap->input_length;
	if (cap->input_dlength<=0) cap->input_dlength=2;

	sz=sizeof(cap->form)-2;
	if (format==NULL) {
	    for (j=0; j<cap->input_dlength; j++) {
		k=cap->input_x+j;
		if (k>=sz) break;
		cap->form[k]=0x20;
	    }
	    k=cap->input_x+cap->input_dlength;
	    if (k>=sz) {
		k=sz;
		cap->input_dlength=k-cap->input_x;
	    }
	} else {
	    strcat((char *)cap->form,(char *)format);
	    k=strlen((char *)cap->form);
	}
#ifdef HEBREW_FONT
	cap->form[k++]='[';
#else
	cap->form[k++]=']';
#endif
	cap->form[k  ]=0;
#ifdef USE_THAI_FONT
    cap->input_x=dspstrlen;
#endif   
}

void caption_error(INT8U mode)
{
	char data[10];
//	if (!caption_get_name(0x1000,msg_buf)) {
	Uart0Init(BAUD_RATE_115200);
	CommBufInit(&CommBuf,COMM_TYPE_HOST,0);////HYP 20041222
	display_temp_setting();
	status_run.run_mode=RUN_RDY_CAP;
    
	display_alloff_primary_indication();
	sprintf(data,"RDY" );
	display_string(0,(INT8U *)data);
	sprintf(data,"NEED");
	display_string(1,(INT8U *)data);
	switch (mode) {
	    case 0: sprintf(data,"CAP   "); break;
	    case 1: sprintf(data,"PLUSTR"); break;
	}
	display_string(2,(INT8U *)data);
	//init_lcdchess();
	VFD7_Diffuse();
//DEL070619	VFD7_save_restore(VFD_BACKUP);
	KEY_InitKeyBuf();
	Startup.start_flag&=0xfeff;

	ADM_CheckModuleAndGetVersion();	// Added by CJK 20050808
	ADM_CheckCsLocation();
	while (1) {
		if (KEY_Read()) {
			switch(KeyDrv.CnvCode) {
				case KP_ENTER:
					debug_Factory();
					break;
				case KP_ESC : 
				case KS_CLEAR:
					_SOFTWARE_RESET;							
					break;
				default     : BuzOn(1); break;
			}
		}
		else
		{
			usb_update_check();
		}
//DEL CJK070316		network_common_trans();
	}
}
/*
void caption_save(INT16U id,INT16U code,char *form,INT16U inputtype,INT32U saveaddr,
	INT16U menustart,INT16U nitem,INT8U x,INT16U leng,INT16U dleng,INT8U itype,INT32U xmin,INT32U xmax)
{
	INT32U addr;
	char   str[CAP_MSG_LENGTH];
	INT16S    l;
	memset((INT8U*)str,0,CAP_MSG_LENGTH);
	memcpy((INT8U*)&str[CAPTION_CODE_POINT],(INT8U*)&code,2);
	l=strlen(form);
	if (l>32) l=32;
	memcpy((INT8U*)&str[CAPTION_NAME_POINT],(INT8U*)form,l);
	memcpy((INT8U*)&str[CAPTION_INPUTTYPE_POINT],(INT8U*)&inputtype,2);
	memcpy((INT8U*)&str[CAPTION_SAVEADDRESS_POINT],(INT8U*)&saveaddr ,4);
	memcpy((INT8U*)&str[CAPTION_MENUSTART_POINT],(INT8U*)&menustart,2);
	memcpy((INT8U*)&str[CAPTION_NITEM_POINT],(INT8U*)&nitem,2);
	memcpy((INT8U*)&str[CAPTION_LENG_POINT],(INT8U*)&leng,2);
	memcpy((INT8U*)&str[CAPTION_DLENG_POINT],(INT8U*)&dleng,2);
	str[CAPTION_ITYPE_POINT]=itype;
	str[CAPTION_X_POINT]=x;
	memcpy((INT8U*)&str[CAPTION_XMIN_POINT],(INT8U*)&xmin,4);
	memcpy((INT8U*)&str[CAPTION_XMAX_POINT],(INT8U*)&xmax,4);

	addr = DFLASH_BASE + FLASH_CAPTION_AREA;	//0x395600
	Flash_swrite(addr+(CAP_MSG_LENGTH*id), (HUGEDATA INT8U *)str, CAP_MSG_LENGTH);
}
*/
//void caption_get_id(INT16U id,char *str)
//{
//	INT32U addr;
//	addr = DFLASH_BASE + FLASH_CAPTION_AREA;
//	addr+=(CAP_MSG_LENGTH*id);
//	Flash_sread(addr, (HUGEDATA INT8U *)str, CAP_MSG_LENGTH);
//}

INT8U caption_get_name(INT16U code,char *str)
{
	INT32U addr;
	INT32U x_addr;

#ifdef USE_EXTEND_CAPNTION_AREA
    if( code >= CAPTION_EXT_CODE_START_POINT && code < CAPNTION_CODE_END_POINT )
        x_addr = DFLASH_BASE + FLASH_CAPTION_EXT_AREA;
	else
        x_addr=DFLASH_BASE+ FLASH_CAPTION_AREA;
#else
        x_addr=DFLASH_BASE+ FLASH_CAPTION_AREA;
#endif

	if (Startup.start_flag&0x8000) {
		sprintf(gmsg,"[%04x>",code);
		MsgOut(gmsg);
	}
	addr =caption_search_by_code(code);
	if (addr==x_addr) return 0;
	addr+=CAPTION_NAME_POINT;
	Flash_sread(addr,(HUGEDATA INT8U *)str,32);
	return 1;
}

//void caption_get_code(INT16U code,char *str)
//{
//	INT32U addr;
//	INT16U cur_code;
//	INT32U count;
//
//	if (code==0) addr = DFLASH_BASE + FLASH_CAPTION_AREA;
//	else {
//		addr = DFLASH_BASE + FLASH_CAPTION_AREA;
//		count=0;
//		while (1) {
//			cur_code=Flash_wread(addr);
//			if (cur_code==code) {
//				break;
//			}
//			if (cur_code==0xffff || count>=0xffff) { addr = DFLASH_BASE + FLASH_CAPTION_AREA; break; }
//			addr+=CAP_MSG_LENGTH;
//			count++;
//			if (count>MAX_CAPTION_NO) break;
//		}
//	}
//	Flash_sread(addr, (HUGEDATA INT8U *)str, CAP_MSG_LENGTH);
//}

INT8U   caption_init(void)
{
	INT32U addr;
	INT16U cur_code,old_code,new_code;
	INT16S    count;
	INT8U  exist_code;
//	char str[32];

	addr = DFLASH_BASE + FLASH_CAPTION_AREA;
	old_code = -1;

	exist_code=0;
	for (count=0; count<16; count++) caption_start_addr[count]=0;
	count     =0;

//	Uart0Init(BAUD_RATE_115200);
//	CommBufInit(&CommBuf,COMM_TYPE_HOST,0);////HYP 20041222
//	if (Startup.start_flag&0x8000) {
//		sprintf(str,"CAP:%08lx",addr);
//		MsgOut(str);
//	}
	while (1) {
#ifdef USE_EXTEND_CAPNTION_AREA
		if (addr == DFLASH_BASE + FLASH_CAPTION_AREA + ((MID_CAPTION_NO) * CAP_MSG_LENGTH))
			addr = DFLASH_BASE + FLASH_CAPTION_EXT_AREA;
#endif
		cur_code=Flash_wread(addr);
		if (cur_code==0x1100) exist_code=1;
		if (count>100) {
			if (exist_code==0) break;
		}
//		if (count<20) {
//			if (Startup.start_flag&0x8000) {
//				sprintf(str,"[%04x]",cur_code);
//				MsgOut(str);
//			}
//		}
		new_code=cur_code>>12;
		if (new_code) {
			if (new_code!=old_code) {
				if (caption_start_addr[new_code]==0) {
					caption_start_addr[new_code]=count;
//					if (Startup.start_flag&0x8000) {
//						sprintf(str,"%08lx,%04x\r\n",addr,new_code);
//						MsgOut(str);
//					}
				}
			}
			old_code=new_code;
		}
		if (cur_code==0xffff) { 
			break; 
		}
		addr+=CAP_MSG_LENGTH;
		count++;
		if (count>=MAX_CAPTION_NO) break;
	}
	return exist_code;
}

INT32U caption_search_by_code(INT16U code)
{
	INT32U addr;
	INT16U cur_code;
	INT32U count;

#ifdef USE_EXTEND_CAPNTION_AREA
	cur_code = code>>12;
	addr = caption_start_addr[cur_code];
	if (addr >= MID_CAPTION_NO) {
		addr = (addr - MID_CAPTION_NO) * CAP_MSG_LENGTH;
		addr += DFLASH_BASE + FLASH_CAPTION_EXT_AREA;
	}
	else {
		addr = addr * CAP_MSG_LENGTH;
		addr += DFLASH_BASE+ FLASH_CAPTION_AREA;
	}
#else
	addr=DFLASH_BASE+ FLASH_CAPTION_AREA;
	cur_code = code>>12;
	addr+=CAP_MSG_LENGTH*caption_start_addr[cur_code];
#endif
	if (Startup.start_flag&0x8000) {
#ifdef HEBREW_FONT
		sprintf(gmsg,"]%04x[",code);
#else
		sprintf(gmsg,"[%04x]",code);
#endif
		MsgOut(gmsg);
	}
	if (code==0) return addr;
	//	count=0;
	//addr로 count 역계산. 아래 while루프에서 마지막 체크를 위해 필요
#ifdef USE_EXTEND_CAPNTION_AREA
	if (addr > DFLASH_BASE + FLASH_CAPTION_EXT_AREA)
	{
		count = (addr - (DFLASH_BASE + FLASH_CAPTION_EXT_AREA)) / CAP_MSG_LENGTH + MID_CAPTION_NO;
	}
	else
	{
		count = (addr - (DFLASH_BASE + FLASH_CAPTION_AREA)) / CAP_MSG_LENGTH;
	}
#else
	count = (addr - (DFLASH_BASE + FLASH_CAPTION_AREA)) / CAP_MSG_LENGTH;
#endif
	while (1) {
#ifdef USE_EXTEND_CAPNTION_AREA
		if (addr == DFLASH_BASE + FLASH_CAPTION_AREA + ((MID_CAPTION_NO) * CAP_MSG_LENGTH))
			addr = DFLASH_BASE + FLASH_CAPTION_EXT_AREA;
#endif		
		cur_code=Flash_wread(addr);
		if (cur_code==code) break;
		if (cur_code==0xffff || count>=MAX_CAPTION_NO-1) { 
			addr=DFLASH_BASE + FLASH_CAPTION_AREA;
			break; 
		}
		addr+=CAP_MSG_LENGTH;
		count++;
		//if (count>=MAX_CAPTION_NO) goto R1;
	}
	return addr;
}

//Hard coding 하고자 하는 캡션의 내용을 마지막 0번 코드 위에 추가
CAPTION_STRUCT caption_internal[] = {
//		code,	form[34],				inputtype,	runflag,	reserved,	submenu_id,	submenu_n,	length,	dlength,	itype,	x,		min,	max		
//		0x4761,	"CTS Period", 			0,			1,			0,			0,			0,			2,		2,			'N',	18,		0,		24
	{	0x4761,	"CTS Period", 			0,			1,			0,			0,			0,			2,		2,			'N',	18,		0,		24},
	{   0x4762,	"CAL date reset", 		0,			1,			0,			0,			0,			1,		1,			'Y',	18,		0,		1},
	{	0x4763,	"CTS Reset Done flag",	0,			1,			0,			0,			0,			3,		3,			'N',	18,		0,		255},
	{	0,		" ",					0,			0,			0,			0,			0,			0,		0,			0,		0,		0,		0},
};

void caption_form_check(CAPTION_STRUCT *caption,INT8U mode)
{
	INT8U i;
	
	if (mode && caption->input_x && caption->input_dlength) {
		for(i=0;i<34;i++)	// form size
		{
			if(caption->form[i] == 0)	caption->form[i]=' ';
		}
		caption->form[caption->input_x-2] = ':';	// 체크필요 : CJK07118
#ifdef HEBREW_FONT
		caption->form[caption->input_x-1] = ']';
		caption->form[caption->input_x + caption->input_dlength ] = '[';
		//caption->form[caption->input_x + caption->input_dlength ] = ']';
#else
		caption->form[caption->input_x-1] = '[';
		caption->form[caption->input_x + caption->input_dlength ] = ']';
#endif
		caption->form[caption->input_x + caption->input_dlength + 1] = 0;
		//caption->form[33] = 0;
	}
	caption->form[33] = 0;
}

INT8U caption_split_by_code(INT16U code, CAPTION_STRUCT *caption,INT8U mode)	// mode 1:"[]"포함
{
	INT32U addr;
	INT16U cur_code;
	INT32U count;
	INT16S idx;
	INT16S    i;
	//static INT32U prev_addr;

	if (code==0) return FALSE;
#ifdef USE_EXTEND_CAPNTION_AREA
	cur_code=code>>12;
	addr = caption_start_addr[cur_code];
	if (addr >= MID_CAPTION_NO) {
		addr = (addr - MID_CAPTION_NO) * CAP_MSG_LENGTH;
		addr += DFLASH_BASE + FLASH_CAPTION_EXT_AREA;
	}
	else {
		addr = addr * CAP_MSG_LENGTH;
		addr += DFLASH_BASE+ FLASH_CAPTION_AREA;
	}
#else
	addr=DFLASH_BASE+ FLASH_CAPTION_AREA;
	cur_code=code>>12;
	addr+=CAP_MSG_LENGTH*caption_start_addr[cur_code];
#endif
	if (code<0x0fff) {
		if (mode) mode = 0;
		else mode = 1;
		plu_get_display_info(code,caption,mode);	// mode 0:":[]"포함
		if (caption->code==0xffff) {
			caption->code=0;
			return FALSE;
		}
		return TRUE;
	}

	for (idx = 0 ; caption_internal[idx].code != 0 ; idx++)
	{
		if(caption_internal[idx].code == code)
		{
			memcpy((HUGEDATA INT8U *)caption, (HUGEDATA INT8U *)&caption_internal[idx], sizeof(CAPTION_STRUCT));
			caption_form_check(caption, mode);
			return TRUE;
		}
	}

	if (caption_continuous_search) {
		if (caption_continuous_search_addr) addr = caption_continuous_search_addr;
	}
	//addr로 count 역계산. 아래 while루프에서 마지막 체크를 위해 필요
#ifdef USE_EXTEND_CAPNTION_AREA
	if (addr > DFLASH_BASE + FLASH_CAPTION_EXT_AREA)
	{
		count = (addr - (DFLASH_BASE + FLASH_CAPTION_EXT_AREA)) / CAP_MSG_LENGTH + MID_CAPTION_NO;
	}
	else
	{
		count = (addr - (DFLASH_BASE + FLASH_CAPTION_AREA)) / CAP_MSG_LENGTH;
	}
#else
	count = (addr - (DFLASH_BASE + FLASH_CAPTION_AREA)) / CAP_MSG_LENGTH;
#endif
	while (1) {
#ifdef USE_EXTEND_CAPNTION_AREA
		if (addr == DFLASH_BASE + FLASH_CAPTION_AREA + ((MID_CAPTION_NO) * CAP_MSG_LENGTH))
			addr = DFLASH_BASE + FLASH_CAPTION_EXT_AREA;
#endif
		cur_code=Flash_wread(addr);
		if (cur_code==code) {
			Flash_sread(addr, (HUGEDATA INT8U *)caption, sizeof(CAPTION_STRUCT));
			caption_form_check(caption, mode);
			if (caption_continuous_search) caption_continuous_search_addr = addr + CAP_MSG_LENGTH;
			else caption_continuous_search_addr = 0;
			return TRUE;
		}
		if (cur_code==0xffff || count>=MAX_CAPTION_NO-1) { 
			addr=DFLASH_BASE + FLASH_CAPTION_AREA;	// No Caption
			Flash_sread(addr, (HUGEDATA INT8U *)caption, sizeof(CAPTION_STRUCT));
			break; 
		}
		addr+=CAP_MSG_LENGTH;
		count++;
		//if (count>MAX_CAPTION_NO) break;
	}
	return FALSE;
}

//INT16U caption_id_search_by_code(INT16U code)		// code -> id
//{
//	INT32U addr;
//	INT16U cur_code, cur_id;
//	INT32U count;
//
//	addr = DFLASH_BASE + FLASH_CAPTION_AREA;
//	cur_id = 0;
//	if (code==0) return 0;
//	count=0;
//	while (1) {
//		cur_code=Flash_wread(addr);
//		if (cur_code==code) return cur_id;
//		if (cur_code==0xffff || count>=0xffff) { cur_id=0; break; }
//		addr+=CAP_MSG_LENGTH;
//		cur_id++;
//		count++;
//		if (count>MAX_CAPTION_NO) break;
//	}
//	return cur_id;
//}

//DEL20060309void caption_erase(void)
//DEL20060309{
//DEL20060309	INT32U addr;
//DEL20060309
//DEL20060309	addr = DFLASH_BASE + FLASH_CAPTION_AREA;	//0x395600
//DEL20060309	Flash_clear(addr, 0xff, CAP_MSG_LENGTH*1000);
//DEL20060309}
