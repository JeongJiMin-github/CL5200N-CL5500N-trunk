/*****************************************************************************
 |*			
 |*  Copyright		:	(c) 2003 CAS
 |*  Filename		:	menu.c
 |*  Version			:	0.1
 |*  Programmer(s)	:	Chung jong kwon (CJK)
 |*  Created			:	2003/09/16
 |*  Description		:	CLP Menu routine    
 |*				
 ****************************************************************************/

#include <stdio.h>
#include <string.h>
#include "globals.h"
#include "prt_font.h"
#include "dsp_common.h"
#include "key.h"
#include "key_typedef.h"
#include "key_app.h"
#include "key_function.h"
#include "main.h"
#include "prt_common.h"
#include "menu.h"
#include "dsp_vfd7.h"
#include "dsp_app.h"
#include "common.h"
#include "initial.h"
#include "flash.h"
#include "flash_app.h"
#include "caption_app.h"
#include "cursor.h"
#include "initial.h"
#include "initial_glb.h"
#include "prt_cnvtable.h"
#include "pgm_app.h"
#include "barcode_file.h"
#include "plu_edit.h"

#ifdef USE_CHINA_PINYIN
#include "china_pinyin.h"
extern ROMDATA INT16U china_charcode[];

INT8U  china_code[12];
INT8U  china_save_pt;
char   china_select_char;
int    china_pos_x,china_pos_y;
INT8U  china_pinyin = 0;

const static char *china_ph_numbertable[]={"0","1","2abc","3edf","4ghi","5lkj","6omn","7psqr","8tuv","9ywzx"};
char  china_ph_save_pt;
char  china_ph_code[12];
char  china_ph_nfield=0;
char  china_ph_temp_nfield=0;
char  china_ph_select_field=0;
char  china_ph_field[200];
char  china_ph_temp_field[200];
char  china_temp[16];

extern void  china_ph_auto_reverse();
extern INT8U china_init(void);
extern void  china_complete(INT8U *buffer,INT16U *cur_pt,INT16U leng,INT8U mode);
extern INT8U china_ph_auto(INT16U code,INT8U *buffer,INT16U *cur_pt,INT16U leng);
extern void  china_ph_auto_reverse(void);
extern void  china_ph_auto_forward(char ch);
#endif


void (*keyin_callback)(long a);
void (*keyin_anycheck)(long a);
INT8U keyin_callback_on;
INT8U keyin_anycheck_on;
BOOLEAN keyin_get_speed_key;
//INT8U   keyin_string_flag;
INT8U   keyin_clerk_onetouch = OFF;
extern  void   korea_init(void);
extern INT16U key_pole_map[];
extern INT16U key_hanging_map[];
#ifdef USE_VIETNAM_FONT
//[start]gm 081020, define vietnam char list
//#if (ROM_COUNTRY == ROM_VNM) del 081103
INT16U VietnamCode_buf[1024];
INT16U VietnamCode_cnt;
INT16U VietnamCombinedChar_cnt;
INT8U VietnamSubscriptList[] = {0xCC, 0xD2, 0xDE, 0xEC, 0xF2, 0x00}; //첨자
INT8U VietnamList[] = {0x41, 0x45, 0x49, 0x4F, 0x55, 0x59, 0xC2, 0xC3, 0xCA, 0xD4, 0xD5, 0xDD, //대문자
0x61, 0x65, 0x69, 0x6F, 0x75, 0x79, 0xE2, 0xE3, 0xEA, 0xF4, 0xF5, 0xFD, //소문자
0x00};
#define VIETNAM_CHAR_NUM	12
#define BIG_LETTER		0
#define SMALL_LETTER		1
//#endif  del gm 081103
//[end]gm 081020, define vietnam char list
#endif

#ifdef USE_THAI_FONT
INT8U ThaiUpperVowelList[] = {0xD1, 0xD4, 0xD5, 0xD6, 0xD7, 0xE7, 0xEC, 0xEE, 0xEE}; 
INT8U ThaiLowerVowelList[] = {0xD8, 0xD9, 0xDA}; 
INT8U ThaiToneMarkList[] = {0xE8, 0xE9, 0xEA, 0xEB}; 
INT8U ThaiCP874CnvList[] = {0xDB, 0xDC, 0xDD, 0xDE}; //--> {0xE9, 0xEA, 0xEB, 0xEC}; //offset = 0x0E
INT32U ThaiCode_buf[1024];
INT16U ThaiCode_cnt;
INT16U ThaiCombinedChar_cnt;
#endif

#if defined(USE_ARAB_FONT) || defined(HEBREW_FONT)
#define ENGWORD	1	//영문 알파벳(A~Z,a~z)
#define NUMBER	2	//아라비아숫자(0~9)
#define NUMSIGN	3	//숫자관련기호($ # %)
#define SEPSIGN	4	//숫자구분기호(- , . : /)
#endif

#ifdef USE_ARAB_FONT
#define SINGLE		0
#define	INITIAL		1
#define MIDDLE		2
#define	FINAL		3

#define ARAB_BUF_SIZE 1024
#ifdef USE_ENG_BASED_ARAB_SUPPORT
INT16U  eng_codetemp[ARAB_BUF_SIZE];
INT16U  arab_revtemp[ARAB_BUF_SIZE];
#endif
INT16U 	arab_subtemp[ARAB_BUF_SIZE];
INT16U  arab_codetemp[ARAB_BUF_SIZE];
INT8U 	arab_engtemp[ARAB_BUF_SIZE];
#define CHECK	0
#define ARAB	1
#define ENG		2
#endif

#ifdef HEBREW_FONT
INT16U  hebrew_codetemp[1024];
INT8U 	hebrew_engtemp[1024];
INT8U 	hebrew_subtemp[1024];
#endif

#define HAN_INIT1	0
#define HAN_INIT2	2
#define HAN_NEUT1	10
#define HAN_NEUT2	20
#define HAN_NEUT2_A	21
#define HAN_NEUT2_YA	22
#define HAN_NEUT2_EO	23
#define HAN_NEUT2_YEO	24
#define HAN_NEUT2_O	25
#define HAN_NEUT2_U	26
#define HAN_NEUT2_EU	27
#define HAN_FINL1 	50
#define HAN_FINL2 	51
#define HAN_FINL2_K	52
#define HAN_FINL2_N	53
#define HAN_FINL2_R	54
#define HAN_FINL2_B	55
#define HAN_FINL2_S	56
#define HAN_FINL3 	70

extern INT16U keyapp_doubleplukey(INT16U dkey, INT8U settingKey);
extern INT16U keyapp_togleplukey(INT16U skeyplu, INT8U settingKey);
extern INT8U  keyin_escape_clear;
extern long   speedkey_plu_number;

#ifdef USE_ARAB_FONT
extern INT8U IsArab(INT8U ch);
extern INT8U BracketCheck(INT8U ch);
extern INT8U BracketOpenConv(INT8U ch);
extern INT8U BracketCloseConv(INT8U ch);
extern INT8U BracketConv(INT8U ch);
extern INT8U EngCheck(INT8U ch);
extern INT8U SpecialCheck(INT8U ch);
extern INT8U BracketCloseChk(INT8U open, INT8U close);

/*********************************************************************************************
* arab_status         : 아랍어 입력기에서의 status		    * arab_count      : 글자가 초성, 중성, 종성인지 확인
* arab_In             : 아랍어가 있는지 체크			    * spec_num        : 문자 위치 변경되어야 할 특수문자 갯수
* cursor_direct       : 커서 방향. 1: 왼쪽, 2: 오른쪽	    * slashNum        : '/', '.' 문자 갯수(별도 처리되는 특수 문자) 
* eBracketOpen        : 영문방향 괄호 열기		            * aBracketOpen    : 아랍어 방향 괄호 열기
* bracketClose        : 괄호 닫을 때 1되는 플래그 	        * bracketNum      : 괄호 개수 카운트
* exceptEnable        : 예외 문자( '#','$','%)
* exceptNum           : '#','$','%'문자 갯수(별도 처리되는 특수 문자)
* bracketNumClose     : 아랍어 방향 괄호 닫았을 때 1, 영문방향괄호 닫았을 때 0
* aBracketOpenNumber  : 아랍어 방향 괄호 열린 갯수 
* bracketArabIn       : 영문방향 괄호 열었을 때 아랍어 들어오는 경우 1, 괄호 모두닫히면 0
* engExceptNum		  : 영문방향에서 #, $, % 입력인 경우 1, 이후 다른 특수기호 입력 시 0
*********************************************************************************************/
INT8U	use_arab_num=0;
INT8U	arab_status,arab_count,eng_num, arab_In, spec_num, cursor_direct, slashNum, exceptNum, eBracketOpen, aBracketOpen, bracketClose, bracketNum, bracketNumClose;
INT8U   aBracketOpenNumber, bracketArabIn, exceptEnable, engExceptNum;
INT16S	arab_codetemp_pnt;
#ifdef USE_ENG_BASED_ARAB_SUPPORT
INT8U arab_exist, spec_exist, num_exist, num_count, spec_count, numOn;
INT16S eng_charPnt, arab_charPnt;
#endif
//INT16U  backup_arab_nengtemp=0; //gm 080820
INT8U	arab_special_char[10];
INT8U	arab_special_nch;
INT8U	arab_newchar;
INT8U	Arab_combine_cnt; //gm 080821
INT8U	arab_combine_cnt_flag; //gm 080823
//INT8U	ArabNumFlag=0;
#endif
#ifdef HEBREW_FONT
extern INT8U HebCheck(INT8U ch);
extern INT8U BracketCheck(INT8U ch);
extern INT8U BracketOpenConv(INT8U ch);
extern INT8U BracketCloseConv(INT8U ch);
extern INT8U BracketConv(INT8U ch);
extern INT8U EngCheck(INT8U ch);
extern INT8U SpecialCheck(INT8U ch);
extern INT8U BracketCloseChk(INT8U open, INT8U close);

INT8U  hebrew_status, eng_num, spec_num, cursor_direct, slashNum, exceptNum, eBracketOpen, hBracketOpen, bracketClose, bracketNum; //hebrew_count;
INT8U  bracketNumClose;
INT16S hebrew_codetemp_pnt;
INT8U  hBracketOpenNumber, bracketHebrewIn, exceptEnable;
INT8U  hebrew_In;
#endif

#ifdef USE_VIETNAM_FONT
//#if (ROM_COUNTRY == ROM_VNM) //gm 081020 del 081103
INT8U IsVietnamSubscript(INT8U ch)
{
	INT8U i;
	for (i=0; i<sizeof(VietnamSubscriptList); i++)
	{
		if (VietnamSubscriptList[i] == 0) break;
		if (VietnamSubscriptList[i] == ch) return i+1;
	}
	return 0;
}

INT8U IsVietnam(INT8U ch)
{
	INT8U i;
	for (i=0; i<sizeof(VietnamList); i++)
	{
		if (VietnamList[i] == 0) break;
		if (VietnamList[i] == ch) return i+1;		
	}
	return 0;
}

void Cvt_VietnamInit(void)
{
	VietnamCode_cnt = 0;
	VietnamCombinedChar_cnt = 0;	
}

void Cvt_VietnamInterpret(INT8U ch)
{
	INT16U cur_ch;
	INT16U Vietnam_nth, Subscript_nth;	
	INT8U CharType;
	
	cur_ch = (INT16U)ch&0x00ff;
	Subscript_nth = IsVietnamSubscript((INT8U)cur_ch);
	
	if (Subscript_nth)
	{	
		if (VietnamCode_cnt == 0) 
		{
			VietnamCode_buf[VietnamCode_cnt++] = cur_ch;
			return;
		}
		
		Vietnam_nth = IsVietnam((INT8U)VietnamCode_buf[VietnamCode_cnt-1]);
		
		if (Vietnam_nth)
		{
			if (Vietnam_nth <= VIETNAM_CHAR_NUM) CharType = BIG_LETTER;
			else 
			{
				Vietnam_nth -= VIETNAM_CHAR_NUM; //range 0~11
				CharType = SMALL_LETTER;
			}						
			VietnamCode_buf[--VietnamCode_cnt] = 256 + (Vietnam_nth-1)*16 + CharType*8 + (Subscript_nth-1);	
			VietnamCode_cnt++;
			VietnamCombinedChar_cnt++; //gm 081020
		}
		else VietnamCode_buf[VietnamCode_cnt++] = cur_ch;//return;
	}
	else 
	{		
		VietnamCode_buf[VietnamCode_cnt++] = cur_ch;	
	}
}

INT16U Cvt_VietnamPreview(INT8U *str, INT16U leng, INT16U cur_pt, INT8U mode)
{
	INT16U i;
	INT16U ret;
	
	ret = 0;	
	
	if (leng == 0) return ret;
	
	Cvt_VietnamInit();
	
	for(i=0; i<leng; i++)
	{
		if (str[i] == 0x00 && mode == 0) break; //gm 081103
		
		Cvt_VietnamInterpret(str[i]);
		if (cur_pt == i) ret = cur_pt - VietnamCombinedChar_cnt;
	}
	VietnamCode_buf[i] = 0;
	//ret = cur_pt - VietnamCombinedChar_cnt;
	
	return ret;
}
//#endif del gm 081103
#endif

#ifdef USE_THAI_FONT
INT8U IsThaiSubscript(INT8U ch)
{
        if (ch == 0xD1) return 1; 
        if (ch >= 0xD4 && ch <= 0xDE) return 1;  
	if (ch >= 0xE7 && ch <= 0xEE) return 1;
	return 0;        
}

INT8U ThaiCP874Cnv(INT8U ch)
{
	INT8U i;
	for (i=0; i<sizeof(ThaiCP874CnvList); i++)
	{
		if (ThaiCP874CnvList[i] == 0) break;
		if (ThaiCP874CnvList[i] == ch) return ch + 0x0E;
	}
	return ch;
}

INT8U IsThaiUpperVowel(INT8U ch)
{
	INT8U i;
	for (i=0; i<sizeof(ThaiUpperVowelList); i++)
	{
		if (ThaiUpperVowelList[i] == 0) break;
		if (ThaiUpperVowelList[i] == ch) return 1;
	}
	return 0;
}

INT8U IsThaiLowerVowel(INT8U ch)
{
	INT8U i;
	for (i=0; i<sizeof(ThaiLowerVowelList); i++)
	{
		if (ThaiLowerVowelList[i] == 0) break;
		if (ThaiLowerVowelList[i] == ch) return 1;
	}
	return 0;
}

INT8U IsThaiToneMark(INT8U ch)
{
	INT8U i;
	for (i=0; i<sizeof(ThaiToneMarkList); i++)
	{
		if (ThaiToneMarkList[i] == 0) break;
		if (ThaiToneMarkList[i] == ch) return 1;
	}
	return 0;
}

INT8U IsThai(INT8U ch)
{
        if (ch > 0xA0) return 1;
	return 0;
}

INT8U CheckThaiChNum(INT32U ch)
{
        if (ch >= 0x01000000) return 3;
        else if (ch >= 0x00010000) return 2;
        else if (ch >= 0x00000100) return 1;
	else return 0;
}

void Cvt_ThaiInit(void)
{
	ThaiCode_cnt = 0;
	ThaiCombinedChar_cnt = 0;	
}

void Cvt_ThaiInterpret(INT8U ch)
{
	INT8U thai_flag, Subscript_flag;	
        INT8U *thai_ch_ptr;
        static INT8U Subscript_num;
	
	Subscript_flag = IsThaiSubscript((INT8U)ch);
	
	if (Subscript_flag)
	{	
		if (ThaiCode_cnt == 0) 
		{
			ThaiCode_buf[ThaiCode_cnt++] = ch;
                        Subscript_num = 0;
			return;
		}

		thai_flag = IsThai((INT8U)ThaiCode_buf[ThaiCode_cnt - 1]);
		if (thai_flag)
		{
                        Subscript_num++;
                        if (Subscript_num > 3) 
                        {
                                //Subscript_num = 3;
                                ThaiCode_buf[ThaiCode_cnt++] = ch;
                                Subscript_num = 0;
                                return;                          
                        }
                        else
                        {
                                thai_ch_ptr = (INT8U *)&(ThaiCode_buf[ThaiCode_cnt - 1]);
                                thai_ch_ptr[Subscript_num] = ch;
                                ThaiCombinedChar_cnt++;
                        }
		}
		else 
                {
                        ThaiCode_buf[ThaiCode_cnt++] = ch;
                        Subscript_num = 0;
                }
	}
	else 
	{		
		ThaiCode_buf[ThaiCode_cnt++] = ch;
                Subscript_num = 0;
	}
}

INT16U Cvt_ThaiPreview(INT8U *str, INT16U leng, INT16U cur_pt, INT8U mode)
{
	INT16U i;
	INT16U ret;
	
	ret = 0;	

	if (leng == 0) return ret;

	Cvt_ThaiInit();
	
	for(i=0; i<leng; i++)
	{
		if (str[i] == 0x00 && mode == 0) break; 
		ThaiCode_buf[i] = 0x20;
		Cvt_ThaiInterpret(str[i]);
		if (cur_pt == i) ret = cur_pt - ThaiCombinedChar_cnt;
	}
	ThaiCode_buf[i] = 0x00;
        ThaiCode_buf[ThaiCode_cnt++] = 0x00;
                
	return ret;
}

INT8U Cvt_ThaiStrLength(INT8U *str)
{
	INT16U i, leng;
	
	leng = strlen((char *)str);
		
	return Cvt_ThaiPreview(str, leng, leng - 1, 0) + 1;
}
#endif

#ifdef USE_CHINA_PINYIN
INT16U read_china_charcode(int i,char mode)
// mode=0 alpha, 1=code;
{
	INT16U ret;
	if (mode==0) ret=china_alpha_pos[i];
	else ret=china_charcode[i];

	return ret;
}
#endif

INT8U IsHanCharMid(char *buf, INT16U pt)
{
	INT16U i;
	
	for (i=0; i<pt;) {								
		if((INT8U)buf[i] < 0x80) {
			i++;
		} else {
			i=i+2;
		}
	}
	if(i==pt) {
		if((INT8U)buf[i] < 0x80) {
			return CHAR_ENG_START;
			//pt   --> Eng
			//pt+1 --> Eng
		} else {
			//pt   --> Han
			return CHAR_HAN_START;
		}
	} else {//(i==pt+1)
		//pt-1 --> Han
		//pt   --> Han_mid
		//pt+1 --> Eng
		return CHAR_HAN_MID;										
	}
}


void insert_ch(char *str, INT8U size, INT8U index, char ch)
{
	INT8U i;
	
	if(size <= index) return;
	for(i = size+1; i > 0; i--) {		// null문자 포함(copy)
		if(i == index) {
			break;
		}
		str[i] = str[i-1];
	}
	str[index] = ch;
	return;
}

void keyin_Init(void)
{
	INT8U ch;
	KeyInSet.language_type =   0;	// 영문입력기
	ch = get_global_bparam(GLOBAL_DECIMAL_SIGN);
	if ((ch<0x20) || (ch==0xff)) {
		ch='.';
		set_global_bparam(GLOBAL_DECIMAL_SIGN,ch);
	}
	KeyInSet.char_decimal  = ch;
	KeyInSet.char_security = '*';
	KeyInSet.char_unit1000 = ',';
	KeyInSet.char_cursorvfd = CURSOR_CHAR;//'_';
	KeyInSet.char_fixed = '.';//':';
	ch = get_global_bparam(GLOBAL_QUESTION_YES);
	if ((ch<0x20) || (ch==0xff)) {
		ch='Y';
		set_global_bparam(GLOBAL_QUESTION_YES,ch);
	}
	KeyInSet.char_yes = (char)ch;
	ch = get_global_bparam(GLOBAL_QUESTION_NO);
	if ((ch<0x20) || (ch==0xff)) {
		ch='Y';
		set_global_bparam(GLOBAL_QUESTION_NO,ch);
	}
	KeyInSet.char_no  = (char)ch;
	//	KeyInSet.char_yes = '1';
	//	KeyInSet.char_no  = '0';
	
	keyin_escape_clear=0;
	keyin_callback_on=0;
	//	keyin_string_flag=0;
	keyin_get_speed_key =0;
	keyin_anycheck_on=0;

#ifdef USE_CHINA_PINYIN
	china_init();
	china_pos_x  = 0;
	china_pos_y  = 0;
	china_pinyin = 0;

	china_ph_field[0]=0;
	china_ph_temp_field[0]=0;
	china_temp[0]=0;
#else
	korea_init();
#endif
	
	CallbackEditPosition_x = 0;
	CallbackEditPosition_y = 0;
}

//void   keyin_string_set(INT8U x)	// x=0:string 끝에 Null을 무조건 추가, 1:string 끝에 Null을 추가안함
//{
//	keyin_string_flag=x;
//}

INT8U	search_barcode_char(char pre_key, INT8U uctemp, char *source)
{	
	char in_data;
	INT16S  size, i;
	
	size = strlen(source);
	
	if(pre_key == uctemp) {
		return 1;
	} else {		
		for(i=0; i<size; i++) {
			in_data = source[i];						
			if( in_data == pre_key) {
				source[i] = 0x22;
				
			}			
			if( source[i] == uctemp ) {
				return 1;
			}				
		}
		if(i==size)
			return 0;
	}
	return 0;	
}


INT8U	check_barcode_char(INT8U uctemp, char *form)
{	
	//char in_data;
	INT16S  size, i;
	
	////DEL 090409	if(uctemp>=0x60 && uctemp<0x7B) {
	////DEL 090409		uctemp -= 0x20;
	////DEL 090409	}
	if (!form) return 0xff;
	size = strlen(form);
	for(i = 0; i < size; i++) {			
		if(form[i] == uctemp) {
			return i;
		} else if(form[i] == '9') {
			if(uctemp >= '0' && uctemp <= '9') return i;
		}
	}
	if(i==size) return 0xff;
	return 0;
}

INT8U check_barcode_str(char *str, char *form_str)
{
	INT8U i, idx;
	char temp_str[BAR_INPUT_FORM_LEN+1];
	strcpy(temp_str, form_str);
	for(i = 0; i < BARCODE_FORMAT_LEN; i++) {
		if(str[i] == 0) break;
		else if(str[i] == '%') {
		 	if(str[i+1] == 0) return FALSE;
			else {
				i++;
				continue;
			}
		} else if(str[i] == str[i+1]) continue;
		
		idx = check_barcode_char(str[i], temp_str);
		if(idx == 0xff) {
			if(i < 1 || str[i-1] != '%') {
				return FALSE;
			}
		} else if(temp_str[idx] != '9' && temp_str[idx] != '%') temp_str[idx] = '"';
	}
	return TRUE;
}

#ifdef USE_CHINA_PINYIN
void any_language_toggle(INT8U xon);
#endif

INT16U keyin_code(void)
{
	INT16U code;
	INT16U  code1;
	INT8U  hex,c1,c2,o_mode;
	char   data[16];
	code = 0;
#ifdef _USE_LCD20848_//ck
	POINT p;	//se-hyung 20080529
	INT8U tarelength;
#endif
	o_mode=KeyDrv.PgmMode;
	KeyDrv.PgmMode=0;
	if(Startup.country == COUNTRY_RU) KeyDrv.PgmMode=1; //SG060411
#ifdef _USE_LCD20848_//ck
	tarelength = get_global_bparam(GLOBAL_DISPLAY_TARELENGTH);

	sprintf(data," %04X",code);
	if(tarelength)
		display_string(DISPLAY_UNITPRICE,(INT8U *)data);
	else
		display_string(DISPLAY_PRICE,(INT8U *)data);
/*
	p.x = 0;
	p.y = 0;
	DspStruct.Reverse = 1;
	Dsp_Write_String(p, data);
	DspStruct.Reverse = 0;
*/
#else
	display_save_restore(DSP_BACKUP);
	
	display_clear(DISPLAY_UNITPRICE);
	sprintf(data," %04X",code);
	display_string(DISPLAY_UNITPRICE,(INT8U *)data);
#endif
	VFD7_Diffuse();
	while (1) {
		while(!KEY_Read()){ }
		BuzOn(1);
		switch(KeyDrv.Type) {
			case KEY_TYPE_ASCII   :
				code1 = (KeyDrv.CnvCode - KP_ASC_00)&0xff;
				if (ishex((INT8U)code1)) {
					hex  = (INT8U)ctoh((INT8U)code1);
					code<<=4;
					code|= hex;
				}
				break;
			case KEY_TYPE_NUMERIC :
				code1 = KeyDrv.CnvCode - KEY_NUM_0;
				code<<=4;
				code|=code1;
				break;
			default:
			case KEY_TYPE_FUNCTION:
				switch(KeyDrv.CnvCode) {
					case KP_PAGE_UP  : break;
					case KP_PAGE_DN  : break;
					case KP_ARROW_DN : break;
					case KP_ARROW_UP : break;
					case KP_CLEAR	 : code=0; break;
					case KP_ENTER    :
					case KP_SAVE     : goto END;
					case KP_ESC      : code=0; goto END;
					case KP_CHANGELANG:
#ifdef USE_CHINA_PINYIN
						any_language_toggle(0);
#else
						if (KeyDrv.PgmMode) KeyDrv.PgmMode=0;
						else                KeyDrv.PgmMode=1;
						if (KeyDrv.PgmMode) {
							display_sign_noflush(SIGN_LANG,1);
						} else {
							display_sign_noflush(SIGN_LANG,0);
						}
#endif
						BuzOn(1);
						break;
				}
		}
		sprintf(data," %04X",code);
#ifdef _USE_LCD20848_//ck
/*
		DspStruct.Reverse = 1;
		Dsp_Write_String(p, data);  //se-hyung 20080529
		DspStruct.Reverse = 0;
*/
		if(tarelength)
			display_string(DISPLAY_UNITPRICE,(INT8U *)data);
		else
			display_string(DISPLAY_PRICE,(INT8U *)data);
#else
		display_string(DISPLAY_UNITPRICE,(INT8U *)data);
#endif
		VFD7_Diffuse();
	}
	END:
	c1    = code>>8;
	c2    = code;
	code  = c2;
	code<<= 8;
	code |= c1;
	KeyDrv.PgmMode=o_mode;
	sprintf(data,"     ");
#ifdef _USE_LCD20848_//ck    
	if(tarelength)
		display_string(DISPLAY_UNITPRICE,(INT8U *)data);
	else
#endif        
		display_string(DISPLAY_PRICE,(INT8U *)data);
	//	display_string(DISPLAY_UNITPRICE,(INT8U *)data);
	//display_save_restore(DSP_RESTORE);
	VFD7_Diffuse();
#ifdef USE_VIETNAM_FONT
	//	//[start]gm 081025, HEX CODE 입력할 때 베트남 첨자입력 방어
	//	if (IsVietnamSubscript(c1) || IsVietnamSubscript(c2)) 
	//	{
	//		BuzOn(2);
	//		return 0; 
	//	}
	//	//[end]gm 081025, HEX CODE 입력할 때 베트남 첨자입력 방어	
#endif
	return code;
}

#ifdef USE_CHINA_PINYIN
// China Pinyin Input Method
void any_press_buzzer(INT16U cur_pt,INT8U use_sec,INT16U leng)
{
	if(cur_pt >= leng-1) {
		if(use_sec) BuzOn(1);
		else        BuzOn(2);
	} else {
		BuzOn(1);
	}
}

void any_language_toggle(INT8U xon)
{
	INT8U onoff;
	if (xon==0) {
		if (KeyDrv.PgmMode) KeyDrv.PgmMode=0;
		else                KeyDrv.PgmMode=1;
		onoff=KeyDrv.PgmMode;
	} else if (xon==1) {
		if (KeyDrv.PhoneType) KeyDrv.PhoneType=0;
		else                  KeyDrv.PhoneType=1;
		onoff=KeyDrv.PhoneType;
	} else if (xon==2) {
		onoff=KeyDrv.PgmMode;
	} else if (xon==3) {
		onoff=KeyDrv.PhoneType;
	}
	if (onoff) {
		display_sign_noflush(SIGN_LANG,1);
	} else {
		display_sign_noflush(SIGN_LANG,0);
	}
	VFD7_Diffuse();
}

INT8U  any_string_insert(INT8U *buffer,INT16U cur_pt,int m,INT8U ctype,INT16U leng)
{
	int i;
	int k;

	if(ctype == CURSOR_TYPE_INSERT) {
	    if (cur_pt+m>=leng) return FALSE;
	    for (k=0; k<m; k++) {
		for(i = cur_pt; buffer[i] != 0x00; i++);
		if (i > leng - 1) {
			BuzOn(2);
			goto END;
		}
		for (; i > cur_pt; i--) {
			buffer[i] = buffer[i - 1];
		}
		buffer[cur_pt]=0x20;
	    }
	}
END:
	return TRUE;
}

INT8U any_string_insert_one(INT8U *buffer,INT16U cur_pt,INT8U ch,INT8U ctype,INT16U leng)
{
	if (any_string_insert(buffer,cur_pt,1,ctype,leng)==FALSE) return FALSE;
//	if ((Startup.country==COUNTRY_KR) || (Startup.country==COUNTRY_CHA)) {
		if (buffer[cur_pt]>0x80) {
			buffer[cur_pt]=0x20;
			if ((cur_pt+1)<leng) buffer[cur_pt+1]=0x20;
		}
//	}
	buffer[cur_pt]=ch;
	return TRUE;
}

INT8U  any_code_char(INT8U code1,INT8U *han_status,INT8U *buffer,INT16U *cur_pt,INT8U ctype,INT8U use_sec,INT16U leng)
{
	INT16U cpt;
	cpt=(*cur_pt);

	*han_status=0;
	//if (Startup.country==COUNTRY_CHA) china_complete(buffer,&cpt,leng,1);
        china_complete(buffer,&cpt,leng,1);

	if (cpt <= leng-1)	// 050604
		any_string_insert_one(buffer,cpt,code1,ctype,leng);
	if (cpt >= leng-1) {
		if(use_sec) BuzOn(1);
		else        BuzOn(2);
	} else {
		BuzOn(1);
		cpt++;
	}
	(*cur_pt)=cpt;
	return 0;
}

INT8U  china_init(void)
{
	INT8U ret;
	ret=FALSE;
	if (china_save_pt) ret=TRUE;
	china_code[0]     = 0;
	china_save_pt     = 0;
	china_select_char = 0;

	china_ph_save_pt=0;
	china_ph_code[0]=0;
	china_ph_nfield=0;
	china_ph_temp_nfield=0;
	china_ph_select_field=0;

	return ret;
}

//void  china_pinyin_pos(int x,int y,INT8U onoff)
//{
//	china_pinyin = onoff;
//	china_pos_x = x;
//	china_pos_y = y;
//}

void  china_complete(INT8U *buffer,INT16U *cur_pt,INT16U leng,INT8U mode)
{
	int start_ln,start_pt;
	int i,sleng;
	INT16U cpt,china_charcode;
	INT16U charmap;
	INT32U addr;

	cpt=*cur_pt;
	if (cpt+2>=leng) {
		china_code[0]=0;
		china_save_pt=0;
		return;
	}
	if (china_code[0]==0) return;
	start_ln=0;
	start_pt=0;
	addr    =CHINA_CHARMAP_PNT;
	for (i=0; i<500; i++) {
//		read_china_charmap(i,(HUGEDATA INT8U *)&china_charmap);
		if (china_charmap[i].ncode==0) break;
		sleng = strlen((char *)china_code);
		if (strncmp((char *)china_code,(char *)china_charmap[i].code,sleng)==0) {
			start_pt=china_charmap[i].pos;
			start_ln=china_charmap[i].ncode;
			break;
		}
	}
	if (start_ln) {
		if (china_select_char<0) china_select_char = start_ln-1;
		if (start_ln<china_select_char+1) china_select_char=0;
		start_pt += china_select_char;
		if (buffer[cpt]<0x80) {
			if (buffer[cpt+1]>0x80) buffer[cpt+2]=0x20;
		}
		china_charcode=read_china_charcode(start_pt,1);
		buffer[cpt  ]=(INT8U)(china_charcode>>8);
		buffer[cpt+1]=(INT8U)(china_charcode&0xff);
		if (mode) {
			cpt+=2;
		}
	} else {
		buffer[*cur_pt  ]=0x20;
		buffer[*cur_pt+1]=0x20;
	}
//END:
	*cur_pt = cpt;
	if (mode) {
		china_init();
	}
}

INT8U  china_code_auto(INT16U code1,INT8U *han_status,INT8U *buffer,INT16U *cur_pt,INT8U ctype,INT16U leng)
{
	INT8U oper=0;
	INT8U sel =0;
	INT16U cpt;
	cpt=(*cur_pt);

	*han_status=0;
	if (china_save_pt) {
		switch (code1) {
		   case KP_ARROW_UP: china_select_char++;
				     goto VIEWCODE;
		   case KP_ARROW_DN: china_select_char--;
				     goto VIEWCODE;
		   case KP_CLEAR   : china_save_pt=0;
				     china_code[0]=0;
				     goto VIEWCODE;
		   case KP_ARROW_LF:
		   case KP_ARROW_RI:
		   case KP_ENTER   : china_complete(buffer,cur_pt,leng,1);
				     oper=1;
				     goto END;
		   case KP_DEL	   : --china_save_pt;
				     china_code[china_save_pt]=0;
				     goto VIEWCODE;
		   default         :
			if ((code1>=KEY_NUM_0) && (code1<=KEY_NUM_9)) {
				china_select_char = code1-KEY_NUM_0;
DCODE:				china_complete(buffer,cur_pt,leng,1);
				oper=1;
				goto END;
			}
			if ((code1>=KP_ASC_A) && (code1<=KP_ASC_Z)) goto DCODE;
			if ((code1>=KP_ASC_A_L) && (code1<=KP_ASC_Z_L)) goto CODE;
			goto END;
		}
	} else {
		if ((code1>=KP_ASC_A) && (code1<=KP_ASC_Z)) goto NOOPER;
		if ((code1>=KEY_NUM_0) && (code1<=KEY_NUM_9)) goto NOOPER;
	}
CODE:	if ((code1>=KP_ASC_A_L) && (code1<=KP_ASC_Z_L)) {
		if (china_save_pt>=8) {
			china_init();
		}
		china_code[china_save_pt++]=(INT8U)(code1-KP_ASC_A_L+'a');
		china_code[china_save_pt  ]=0;
		oper=1;
	} else {
		if (china_save_pt) {
			china_complete(buffer,cur_pt,leng,1);
			any_code_char((INT8U)(code1-KP_ASC_00),han_status,buffer,cur_pt,0,0,leng);
			oper=1;
		}
		goto END;
	}
VIEWCODE:
	if (china_save_pt) {
		china_complete(buffer,cur_pt,leng,0);
	} else {
		buffer[*cur_pt  ]=0x20;
		buffer[*cur_pt+1]=0x20;
	}
	oper=1;
END:
	if (china_pinyin) {
		INT8U old_page;
		INT8U backup_directdraw, backup_reverse;
		POINT disp_p;

		if (china_code[0] == 0)
		{
			Dsp_Diffuse();
		}
		else
		{
			sprintf(china_temp,"%-8s",china_code);
			//PutSmallString(china_pos_y,china_pos_x,china_temp);
			old_page = DspStruct.Page;
			Dsp_SetPage(DSP_ERROR_PAGE);
			disp_p = point(china_pos_y,china_pos_x);
			backup_directdraw = DspStruct.DirectDraw;
			backup_reverse = DspStruct.Reverse;
			DspStruct.DirectDraw = 1;
			DspStruct.Reverse = 1;
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
			Dsp_Write_String(disp_p,(HUGEDATA char *)china_temp);
#else
			display_string(DISPLAY_PRICE, (INT8U*)china_temp);
#endif
			DspStruct.DirectDraw = backup_directdraw;
			DspStruct.Reverse = backup_reverse;
			
			Dsp_SetPage(old_page);
		}
	}
NOOPER:
	return oper;
}

INT8U china_ph_check_exist(char *field)
{
	int i,start;
	INT8U ok;
	INT32U addr;

	ok=0;
//	char temp[64];
//	INT32U addr;
	if (field[0]<'a') goto END;
	if (field[0]>'z') goto END;
//	start=(int)china_alpha_pos[(int)(field[0]-'a')];
	start=(int)read_china_charcode((int)(field[0]-'a'),0);

//	sprintf(temp,"code=%d\n",start);
//	MsgOut(temp);
	addr = (INT32U)start*10L;
	addr+= CHINA_CHARMAP_PNT;
	for (i=start; i<500; i++) {
//		read_china_charmap(i,(HUGEDATA INT8U *)&china_charmap);
		if (china_charmap[i].ncode==0) break;
		if (field[0]>china_charmap[i].code[0]) break;
		if (strncmp(field,china_charmap[i].code,strlen(field))==0) {
			ok=1;
			break;
		}

//		sprintf(temp,"map=%s,%d,%d\n",china_charmap.code,china_charmap.ncode,china_charmap.pos);
//		MsgOut(temp);
//		if (china_charmap[i].ncode==0) break;
//		if (field[0]>china_charmap[i].code[0]) break;
//		if (strncmp(field,china_charmap[i].code,strlen(field))==0) {
//			ok=1;
//			break;
//		}
	}
END:
	return ok;
}

void china_ph_auto_forward(char ch)
{
	char stemp[16],field[16];
	int  i,k,nfield;

	if ((ch<'0') || (ch>'9')) return;
	sprintf(stemp,"%s",china_ph_numbertable[(int)(ch-'0')]);
	if (china_ph_save_pt>=6) {
		china_init();
	}
	if (china_ph_save_pt==0) {
		china_ph_code[china_ph_save_pt++]=ch;
		china_ph_code[china_ph_save_pt  ]=0;
		k=strlen(stemp);
		for (i=1; i<k; i++) {
			sprintf(&china_ph_field[china_ph_nfield*10],"%c",stemp[i]);
			china_ph_nfield++;
		}
	} else {
		// move china_ph_field to china_ph_temp_field
		china_ph_temp_nfield=0;
		for (k=0; k<china_ph_nfield; k++) {
			sprintf(field,"%s",&china_ph_field[k*10]);
			sprintf(&china_ph_temp_field[china_ph_temp_nfield*10],"%s",field);
			china_ph_temp_nfield++;
		}

		nfield              = china_ph_nfield;
		china_ph_nfield     = 0;
		china_ph_code[china_ph_save_pt++]=ch;
		china_ph_code[china_ph_save_pt  ]= 0;

		for (i=1; i<strlen(stemp); i++) {
			for (k=0; k<nfield; k++) {
//				sprintf(field,"%s",&china_ph_temp_field[k*10]);
				sprintf(field,"%s%c",&china_ph_temp_field[k*10],stemp[i]);
				if (china_ph_check_exist(field)) {
					sprintf(&china_ph_field[china_ph_nfield*10],"%s",field);
					china_ph_nfield++;
				}
			}
		}
	}
	china_ph_select_field=0;
	china_select_char    =0;
}

void china_ph_auto_reverse(void)
{
	int  i,l;
	if (china_ph_save_pt) {
		china_ph_save_pt--;
		china_ph_code[china_ph_save_pt]=0;
		strcpy(china_temp,(char *)china_ph_code);
	}
	if (china_ph_save_pt) {
		l=(int)china_ph_save_pt;
		china_init();
		for (i=0; i<l; i++) {
			china_ph_auto_forward(china_temp[i]);
		}
	}
	china_ph_select_field=0;
	china_select_char=0;
}

INT8U china_ph_auto(INT16U code,INT8U *buffer,INT16U *cur_pt,INT16U leng)
{
	INT8U mode;
	mode=0;
	switch (code) {
		case KP_CLEAR:  if (china_ph_save_pt) {
					mode=8;
					china_init();
					buffer[*cur_pt  ]=0x20;
					buffer[*cur_pt+1]=0x20;
				}
			        break;
		case KP_ARROW_LF:if (china_ph_save_pt) {
					china_ph_select_field--;
					mode=1;
				}
				break;
		case KP_ARROW_RI:if (china_ph_save_pt) {
					china_ph_select_field++;
					mode=1;
				}
				break;
		case KP_ARROW_UP:
				if (china_ph_save_pt) {
					mode=2;
					china_select_char++;
				}
				break;
		case KP_ARROW_DN:
				if (china_ph_save_pt) {
					mode=2;
					china_select_char--;
				}
				break;
		case KP_BS	:
		case KP_DEL	:
				if (china_ph_save_pt) {
					china_ph_auto_reverse();
					mode=3;
				}
				break;
		case KP_ENTER	:
				if (china_ph_save_pt) {
					mode=9;
					china_complete(buffer,cur_pt,leng,1);
				}
				break;
		default:
			if ((code>=KEY_NUM_0) && (code<=KEY_NUM_9)) {
				china_ph_auto_forward((char)(code-KEY_NUM_0)+'0');
				mode=4;
			}
	}

	switch (mode) {
	    case 4:
	    case 3:
	    case 1:if (china_ph_nfield) {
			if (china_ph_select_field<0) china_ph_select_field=china_ph_nfield-1;
			if (china_ph_select_field>=china_ph_nfield) china_ph_select_field=0;
			strcpy((char *)china_code,&china_ph_field[china_ph_select_field*10]);
			china_complete(buffer,cur_pt,leng,0);
		   } else {
			china_ph_select_field=0;
		   }
		   break;
	    case 2:china_complete(buffer,cur_pt,leng,0);
		   break;
	}
	if (mode) {
		if (china_pinyin) {
			INT8U old_page;
			INT8U backup_directdraw, backup_reverse;
			POINT disp_p;
			if (china_code[0] == 0)
			{
				Dsp_Diffuse();
			}
			else
			{
				if (china_ph_nfield>1)
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
					sprintf(china_temp,"%d:%-6s",china_ph_select_field+1,china_code);
#else
					sprintf(china_temp,"%d_%-6s",china_ph_select_field+1,china_code);
#endif
				else
					sprintf(china_temp,"%-8s",china_code);
				//PutSmallString(china_pos_y,china_pos_x,china_temp);
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
				old_page = DspStruct.Page;
				Dsp_SetPage(DSP_ERROR_PAGE);

				disp_p = point(china_pos_y,china_pos_x);
				backup_directdraw = DspStruct.DirectDraw;
				backup_reverse = DspStruct.Reverse;
				DspStruct.DirectDraw = 1;
				DspStruct.Reverse = 1;
				Dsp_Write_String(disp_p,(HUGEDATA char *)china_temp);
				DspStruct.DirectDraw = backup_directdraw;
				DspStruct.Reverse = backup_reverse;

				Dsp_SetPage(old_page);
#else
				display_string(DISPLAY_PRICE, (INT8U*)china_temp);
				VFD7_Diffuse();
#endif
			}
		}
	}
	return mode;
}
#endif
INT8U  korea_code[8];
INT8U  korea_save_pt;
INT8U  korea_first_noinsert;
INT16U korea_old_code;
INT16U korea_old_finl;
INT16U korea_init_to_finl[21]={ 0,1,2,3,5,8,0,9,17,19,0,21,22,23,24,0,25,26,27,28,29, };

INT8U  korea_auto(INT16U code1,INT8U *han_status,INT8U *buffer,INT16U *cur_pt,INT8U ctype,INT16U leng);
void   korea_gen_code(INT8U *buffer,INT16U cur_pt);
INT8U  korea_check_initial(INT16U code1);
INT8U  korea_check_neut(INT16U code1);
void   korea_complete(INT8U *han_status,INT8U *buffer,INT16U *cur_pt,INT16U leng);
void   korea_code_auto(INT16U code1,INT8U *han_status,INT8U *buffer,INT16U *cur_pt,INT8U ctype,INT16U leng);
INT8U  korea_string_insert(INT8U *buffer,INT16U cur_pt,INT16S m,INT8U ctype,INT16U leng);
INT8U  korea_string_insert_two(INT8U *buffer,INT16U cur_pt,INT8U *ch,INT8U ctype,INT16U leng);
INT8U  korea_string_insert_one(INT8U *buffer,INT16U cur_pt,INT8U ch ,INT8U ctype,INT16U leng);
//INT8U  korea_code_eng(INT16U code1,INT8U *han_status,INT8U *buffer,INT16U *cur_pt,INT8U ctype,INT8U use_sec,INT16U leng);
INT8U  korea_code_char(INT8U code1,INT8U *han_status,INT8U *buffer,INT16U *cur_pt,INT8U ctype,INT8U use_sec,INT16U leng);
void   korea_init(void);
void   korea_cm2ks(char *cm);

void   korea_init(void)
{
	korea_first_noinsert=0;
	korea_code[0]=0;
	korea_code[1]=0;
	korea_code[2]=0;
}

void korea_cm2ks(char *cm)
{
	INT16U cm_code,ks_code,g,r;
	INT16S i;
	//	char temp[32];
	
	cm_code=cm[0];
	cm_code<<=8;
	cm_code|=(INT8U)cm[1];
	
	for (i=0; i<30; i++) {
		if (HanWan2JohabConvTable_Chosung[i]==cm_code) {
			ks_code = i+0xA4A1;
			goto CM;
		}
	}
	for (i=0; i<2350+50; i++) {
		if (HanWan2JohabConvTable[i]==cm_code) {
			g = i / 0x60;
			r = i % 0x60;
			ks_code = g + 0xB0;
			ks_code <<=8;
			ks_code|=(r+0xa0);
			goto CM;
		}
	}
	ks_code=0xB0A0;
	CM:
						cm[0]=(INT8U)(ks_code>>8);
	cm[1]=(INT8U)(ks_code&0xff);
}

INT8U  korea_string_insert(INT8U *buffer,INT16U cur_pt,INT16S m,INT8U ctype,INT16U leng)
{
	INT16S i;
	INT16S k;
	
	if(ctype == CURSOR_TYPE_INSERT) {
	    if (cur_pt+m>=leng) return FALSE;
	    for (k=0; k<m; k++) {
			for(i = cur_pt; buffer[i] != 0x00; i++);
			if (i > leng - 1) {
				BuzOn(2);
#ifdef USE_THAI_FONT                        
				goto DEL_PROC;
#else
				goto END;
#endif
			}
			for (; i > cur_pt; i--) {
				buffer[i] = buffer[i - 1];
			}
			buffer[cur_pt]=0x20;
	    }
	}
#ifdef USE_THAI_FONT
    else //CURSOR_TYPE_DELETE
    {
DEL_PROC:          
	    if (!IsThai(buffer[cur_pt])) return TRUE; //Check Thai
	    for (k = 0, i = 1; i < 4; i++) { //Check Sub
	            if (IsThaiSubscript(buffer[cur_pt + i])) k++;
	            else break;
	    }
	    if (k == 0) return TRUE;
	    for(i = cur_pt; buffer[i] != 0x00; i++) {
	            buffer[i] = buffer[i + k];
	    }
	    buffer[cur_pt]=0x20;
      }
#endif   
	END:
		return TRUE;
}

INT8U korea_string_insert_one(INT8U *buffer,INT16U cur_pt,INT8U ch,INT8U ctype,INT16U leng)
{
	//	char temp[32];
	//sprintf(temp,"(");
	//MsgOut((HUGEDATA char *)temp);
	//MsgOut(buffer);
	//sprintf(temp,")\r\n");
	//MsgOut((HUGEDATA char *)temp);
	if (korea_string_insert(buffer,cur_pt,1,ctype,leng)==FALSE) return FALSE;
	if (Startup.country==COUNTRY_KR) {
		if (buffer[cur_pt]>0x80) {
			buffer[cur_pt]=0x20;
			if ((cur_pt+1)<leng) buffer[cur_pt+1]=0x20;
		}
	}
	buffer[cur_pt]=ch;
	return TRUE;
}

INT8U korea_string_insert_two(INT8U *buffer,INT16U cur_pt,INT8U *ch,INT8U ctype,INT16U leng)
{
	if (korea_string_insert(buffer,cur_pt,2,ctype,leng)==FALSE) return FALSE;
	if (buffer[cur_pt]>(INT8U)0x80) {
		buffer[cur_pt]=0x20;
		if ((cur_pt+1)<leng) buffer[cur_pt+1]=0x20;
	} else {
		if (cur_pt+1<leng-1) {
		    if (buffer[cur_pt+1]>(INT8U)0x80) {
				buffer[cur_pt+1]=0x20;
				if (cur_pt+2<leng-1) buffer[cur_pt+2]=0x20;
		    }
		}
	}
	if ((cur_pt+1)<leng-1) {
		buffer[cur_pt]  =ch[0];
		buffer[cur_pt+1]=ch[1];
	} else {
		return FALSE;
	}
	return TRUE;
}

void  korea_gen_code(INT8U *buffer,INT16U cur_pt )
{
	INT16U code;
	INT8U  c1,c2,c3,kor[2];
	c1=korea_code[0];
	c2=korea_code[1];
	c3=korea_code[3];
	if (!c3) c3=korea_code[2];
	if (c1==0) c1=1;
	if (c2==0) c2=2;
	if (c3==0) c3=1;
	code = c1;
	code<<=5;
	code|= c2;
	code<<=5;
	code|= c3;
	code|=0x8000;
	kor[0]=(INT8U)(code>>8  );
	kor[1]=(INT8U)(code&0xff);
	korea_cm2ks((char *)kor);
	buffer[cur_pt  ]=kor[0];
	buffer[cur_pt+1]=kor[1];
}
// Line 175
INT8U  korea_check_initial(INT16U code1)
{
	if ((code1>=KP_I_BASE) && (code1<KP_N_BASE)) return 1;
	return 0;
}

INT8U  korea_check_neut(INT16U code1)
{
	if ((code1>=KP_N_BASE) && (code1<KP_F_BASE)) return 1;
	return 0;
}

void  korea_complete(INT8U *han_status,INT8U *buffer,INT16U *cur_pt,INT16U leng)
{
	INT16U code;//,cp;
	INT8U  c1,c2,c3,kor[2];
	(*han_status)=HAN_INIT1;
	c1=korea_code[0];
	c2=korea_code[1];
	c3=korea_code[3];
	korea_first_noinsert=0;
	if (!c3) c3=korea_code[2];
	if (c1==0) c1=1;
	if (c2==0) c2=2;
	if (c3==0) c3=1;
	if ((c1==1) && (c2==2) && (c3==1)) {
		goto RET_ESC;
	}
	if (c1==1) {
		goto RET_ESC;
	}
	if ((c2==2) && (c3==1)) {
		goto RET_ESC;
	}
	code = c1;
	code<<=5;
	code|= c2;
	code<<=5;
	code|= c3;
	code|=0x8000;
	kor[0]=(INT8U)(code>>8  );
	kor[1]=(INT8U)(code&0xff);
	korea_cm2ks((char *)kor);
	if ((kor[0]==0xB0) && (kor[1]==0xA0)) {
		goto RET_ESC;
	}
	if ((*cur_pt)+1<leng) { 	// Limit String Length
		buffer[(*cur_pt)++]=kor[0];
		buffer[(*cur_pt)++]=kor[1];
	}
	goto END;
	RET_ESC:
						korea_first_noinsert=1;
	END:	korea_code[0]=0;
	korea_code[1]=0;
	korea_code[2]=0;
}

INT8U  korea_code_char(INT8U code1,INT8U *han_status,INT8U *buffer,INT16U *cur_pt,INT8U ctype,INT8U use_sec,INT16U leng)
{
	INT16U cpt;
	INT8U	language_flag;		//parameter 980
	cpt=(*cur_pt);
	language_flag = get_global_bparam(GLOBAL_SALE_SETUP_FLAG4)&0x08;
	if (Startup.country==COUNTRY_KR || language_flag) korea_complete(han_status,buffer,&cpt,leng);
#if defined(USE_ARAB_CONVERT) || defined(HEBREW_FONT)
	if (cpt <= leng)	// 050604
#else
	if (cpt <= leng-1)	// 050604
#endif
		korea_string_insert_one(buffer,cpt,code1,ctype,leng);
#if defined(USE_ARAB_CONVERT) || defined(HEBREW_FONT)
	if (cpt >= leng) {
#else
	if (cpt >= leng-1) {
#endif
		if(use_sec) BuzOn(1);
		else        BuzOn(2);
	} else {
		BuzOn(1);
		cpt++;
	}
	(*cur_pt)=cpt;
	return 0;
}

void   korea_code_auto(INT16U code1,INT8U *han_status,INT8U *buffer,INT16U *cur_pt,INT8U ctype,INT16U leng)
{
	INT8U code_finl;
	INT8U ch;
	if (*han_status>HAN_INIT1) {
		if (KeyDrv.Type==KEY_TYPE_ASCII) {
			korea_complete(han_status,buffer,cur_pt,leng);
			ch=KeyDrv.CnvCode-KP_ASC_00;
			korea_string_insert_one(buffer,(*cur_pt),ch,ctype,leng);
			cur_pt++;
		}
	}
	if ((*han_status)==HAN_INIT1) {
		if (korea_check_neut(code1)) {
	        BuzOn(2);
			return;
		}
	}
	if ((*han_status)==HAN_INIT2) {
		if (korea_check_initial(code1)) {
			ch =(INT8U)(code1-KP_I_BASE);
			if (ch==korea_code[korea_save_pt]) {
			    switch (code1) {
			    	case KP_I_K:
			    	case KP_I_D:
			    	case KP_I_S:
			    	case KP_I_B:
			    	case KP_I_J: break;
			    	default    : (*han_status)=HAN_INIT1;
					korea_init();
					korea_first_noinsert=1;
					break;
			    }
			}
		}
	}
	if (((*han_status)>=HAN_NEUT1) && ((*han_status)<HAN_NEUT2) ) {
		if (korea_check_initial(code1)) {
			korea_init();
			(*han_status)=HAN_INIT1;
		}
	}
	if (((*han_status)>=HAN_NEUT2) && ((*han_status)<HAN_FINL1) ) {
		if (korea_check_initial(code1)) {
			code_finl=korea_init_to_finl[code1-KP_I_BASE];
			if (code_finl==0) {
				korea_complete(han_status,buffer,cur_pt,leng);
			} else {
				(*han_status)=HAN_FINL1;
			}
		}
	}
	if ((*han_status)==HAN_FINL1) {
		// init+neut+neut+<final>
		//               +<neut>
		if (korea_check_initial(code1)) {
			code_finl=korea_init_to_finl[ code1-KP_I_BASE ];
			if (code_finl==0) {
				korea_complete(han_status,buffer,cur_pt,leng);
			}
		} else {
			korea_complete(han_status,buffer,cur_pt,leng);
		}
	} else if ((*han_status)==HAN_FINL2)  {
		if (korea_check_neut(code1)) {
			korea_code[2]=0;
			korea_complete(han_status,buffer,cur_pt,leng);
			korea_auto(korea_old_finl   ,han_status,buffer,cur_pt,ctype,leng);
		} else if (korea_check_initial(code1)) {
			korea_complete(han_status,buffer,cur_pt,leng);
		}
	} else if (((*han_status)>HAN_FINL2) && ((*han_status)<HAN_FINL3) ) {
	    if (korea_check_neut(code1)) {
			korea_code[2]=0;
			korea_complete(han_status,buffer,cur_pt,leng);
			korea_auto(korea_old_finl   ,han_status,buffer,cur_pt,ctype,leng);
	    } else {
			switch (*han_status) {
				case HAN_FINL2_K:
					if (!((code1==KP_I_K) || (code1==KP_I_S))) {
						korea_complete(han_status,buffer,cur_pt,leng);
					}
					break;
				case HAN_FINL2_N:
					if (!((code1==KP_I_J) || (code1==KP_I_H))) {
						korea_complete(han_status,buffer,cur_pt,leng);
					}
					break;
				case HAN_FINL2_R:
					switch (code1) {
						case KP_I_K:
						case KP_I_M:
						case KP_I_B:
						case KP_I_S:
						case KP_I_T:
						case KP_I_P:
						case KP_I_H: break;
						default    :
							korea_complete(han_status,buffer,cur_pt,leng);
					}
					break;
				case HAN_FINL2_B:
					//			if (code1!=KP_I_B) {
					if (code1!=KP_I_S) { //HYP 20050902
						korea_complete(han_status,buffer,cur_pt,leng);
					}
					break;
				case HAN_FINL2_S:
					if (code1!=KP_I_S) {
						korea_complete(han_status,buffer,cur_pt,leng);
					}
					break;
			}
		}
	} else if ((*han_status)==HAN_FINL3)  {
		if (korea_check_neut(code1)) {
			korea_code[3]=0;
			korea_complete(han_status,buffer,cur_pt,leng);
			korea_auto(korea_old_finl   ,han_status,buffer,cur_pt,ctype,leng);
		} else if (korea_check_initial(code1)) {
			korea_complete(han_status,buffer,cur_pt,leng);
		}
	}
	korea_auto(code1,han_status,buffer,cur_pt,ctype,leng);
}
// Line 299
INT8U  korea_auto(INT16U code1,INT8U *han_status,INT8U *buffer,INT16U *cur_pt,INT8U ctype,INT16U leng)
{
	INT8U  code_tmp,s_ok;
	INT8U  code_finl;
	INT8U  temp_code[4];
	
	if (korea_check_initial(code1)) code_finl=korea_init_to_finl[code1-KP_I_BASE];
	else code_finl=0;
	switch (*han_status) {
	    case HAN_INIT1:
			INIT1: (*han_status)=HAN_INIT1;
			korea_save_pt=0;
			s_ok=0;
			if (korea_check_initial(code1)) {
				if (((korea_code[0]==0) && (korea_code[1]==0))) {
					s_ok=1;
				}
				memset((char *)korea_code,0,5);
				(*han_status)=HAN_INIT2;
				korea_code[korea_save_pt]=(INT8U)(code1-KP_I_BASE);
				if (s_ok) {
					korea_gen_code(temp_code,0);
					if (!korea_first_noinsert) {
						if (korea_string_insert_two(buffer,(*cur_pt),temp_code,ctype,leng)==FALSE) {
							(*han_status)=HAN_INIT1;
							korea_first_noinsert=0;
							return FALSE;
						}
						korea_first_noinsert=1;
					}
				}
			}
			break;
	    case HAN_INIT2: if (korea_check_initial(code1)) {
			code_tmp=(INT8U)(code1-KP_I_BASE);
			if (code_tmp==korea_code[korea_save_pt]) {
				// Complete Buffer[0] and Buffer[1];
				switch (code1) {
					case KP_I_K: code1=KP_I_KK; break;
					case KP_I_D: code1=KP_I_DD; break;
					case KP_I_B: code1=KP_I_BB; break;
					case KP_I_S: code1=KP_I_SS; break;
					case KP_I_J: code1=KP_I_JJ; break;
					default    : goto INIT1;
				}
				korea_code[korea_save_pt]=(INT8U)(code1-KP_I_BASE);
				(*han_status)=HAN_NEUT1;
			}
		} else {
			if (korea_check_neut(code1))
				goto NEUT1;
			else goto INIT1;
		}
		break;
		case HAN_NEUT1:
			NEUT1:  korea_old_code=code1;
			s_ok=1;
			switch (code1) {
				case KP_N_AE :
				case KP_N_YAE:
				case KP_N_E  :
				case KP_N_YE :
				case KP_N_YO :
				case KP_N_YU :
				case KP_N_I  : s_ok=0;
				korea_code[++korea_save_pt]=(INT8U)(code1-KP_N_BASE);
				(*han_status)=HAN_FINL1;
				break;
				case KP_N_A  : (*han_status)=HAN_NEUT2_A  ; break;
				case KP_N_YA : (*han_status)=HAN_NEUT2_YA ; break;
				case KP_N_EO : (*han_status)=HAN_NEUT2_EO ; break;
				case KP_N_YEO: (*han_status)=HAN_NEUT2_YEO; break;
				case KP_N_O  : (*han_status)=HAN_NEUT2_O  ; break;
				case KP_N_U  : (*han_status)=HAN_NEUT2_U  ; break;
				case KP_N_EU : (*han_status)=HAN_NEUT2_EU ; break;
				default      : goto INIT1;
			}
			if (s_ok) {
				korea_code[++korea_save_pt]=(INT8U)(code1-KP_N_BASE);
			}
			break;
	    case HAN_NEUT2    :
	    case HAN_NEUT2_A  :
	    case HAN_NEUT2_YA :
	    case HAN_NEUT2_EO :
	    case HAN_NEUT2_YEO:
	    case HAN_NEUT2_EU :
			if (code1==KP_N_I) {
				korea_code[korea_save_pt]=(INT8U)(korea_old_code+1-KP_N_BASE);
				(*han_status)=HAN_FINL1;
			}
			break;
	    case HAN_NEUT2_O:s_ok=1;
		switch (code1) {
			case KP_N_A : code1=KP_N_WA ; break;
			case KP_N_AE: code1=KP_N_WAE; break;
			case KP_N_I : code1=KP_N_OE ; break;
			default :s_ok=0;
			break;
		}
		if(s_ok) {
			korea_code[korea_save_pt]=(INT8U)(code1-KP_N_BASE);
			(*han_status)=HAN_FINL1;
		}
		break;
		
	    case HAN_NEUT2_U:s_ok=1;
		switch (code1) {
			case KP_N_EO : code1=KP_N_WO ; break;
			case KP_N_E  : code1=KP_N_WE ; break;
			case KP_N_I  : code1=KP_N_WI ; break;
			default      : s_ok=0;
			break;
		}
		if(s_ok) {
			korea_code[korea_save_pt]=(INT8U)(code1-KP_N_BASE);
			(*han_status)=HAN_FINL1;
		}
		break;
	    case HAN_FINL1:  if (korea_check_initial(code1)) {
			code_finl=korea_init_to_finl[code1-KP_I_BASE];
			korea_old_finl=code1;
			s_ok=0;
			switch (code1) {
				case KP_I_K: (*han_status)=HAN_FINL2_K  ; break;
				case KP_I_N: (*han_status)=HAN_FINL2_N  ; break;
				case KP_I_R: (*han_status)=HAN_FINL2_R  ; break;
				case KP_I_B: (*han_status)=HAN_FINL2_B  ; break;
				case KP_I_S: (*han_status)=HAN_FINL2_S  ; break;
				default     :
					(*han_status)=HAN_FINL2    ; break;
			}
			korea_code[++korea_save_pt]=code_finl;
		}
		break;
	    case HAN_FINL2:  break;
	    case HAN_FINL2_K:korea_old_finl=code1;
		switch (code1) {
			case KP_I_K: code_finl=3; break;
			case KP_I_S: code_finl=4; break;
		}
		korea_code[++korea_save_pt]=code_finl;
		(*han_status)=HAN_FINL3;
		break;
	    case HAN_FINL2_N:korea_old_finl=code1;
		switch (code1) {
			case KP_I_J: code_finl=6; break;
			case KP_I_H: code_finl=7; break;
		}
		korea_code[++korea_save_pt]=code_finl;
		(*han_status)=HAN_FINL3;
		break;
	    case HAN_FINL2_R:korea_old_finl=code1;
		switch (code1) {
			case KP_I_K: code_finl=10; break;
			case KP_I_M: code_finl=11; break;
			case KP_I_B: code_finl=12; break;
			case KP_I_S: code_finl=13; break;
			case KP_I_T: code_finl=14; break;
			case KP_I_P: code_finl=15; break;
			case KP_I_H: code_finl=16; break;
		}
		korea_code[++korea_save_pt]=code_finl;
		(*han_status)=HAN_FINL3;
		break;
	    case HAN_FINL2_B:korea_old_finl=code1;
		if (code1==KP_I_S) code_finl=20;
		korea_code[++korea_save_pt]=code_finl;
		(*han_status)=HAN_FINL3;
		break;
	    case HAN_FINL2_S:korea_old_finl=code1;
		if (code1==KP_I_S) code_finl=22;
		korea_code[++korea_save_pt]=code_finl;
		(*han_status)=HAN_FINL3;
		break;
	}
	korea_gen_code(temp_code,0);
	if (((*cur_pt)+1)<leng) {	// 050604 Limit String Length
		buffer[(*cur_pt)  ]=temp_code[0];
		buffer[(*cur_pt)+1]=temp_code[1];
	} else {
		korea_init();
	}
	return 0;
}

INT16U keyin_string(INT8U input_mode, INT8U dsp_mode, HUGEDATA INT8U *format, HUGEDATA INT8U *ret_str, 
		INT16U leng, INT16U win_size, INT8U y, INT16U x, INT16U properties, INT8U use_sec, INT8U pass)
{
	HUGEDATA char *buffer;
	INT16U i, j,ret;
	INT16U cur_pt,chcode;
	POINT disp_p;
	MENU_CURSOR_STR cursor;
	MENU_WINDOW_STR string_win[1];
	char dsp_buffer[1][KI_DSP_BUF_SIZE];
	char backup_format[64];//, pre_key;
	INT8U ip_start, ip_is_ch, uctemp,han_status,ch_code;
	INT16S   w;
	INT8U use_lf, position_disp, page_updn_key;
	INT16U num_char, str_len;
	char tmp_str[16];
	char tmp_str2[16];
	INT8U backup_directdraw;
	INT8U korean_flag = OFF;
	INT8U	language_flag;
	INT8U exclude_null;
	INT8U english_mode;
	INT8U backup_pgmmode;
	INT8U fixedCharNum = 0;
	INT8U cntFixedChar;
#ifdef USE_VIETNAM_FONT
	INT16U dsp_viet_buffer[1][KI_DSP_BUF_SIZE];
	INT8U cvt_cur_pt;
#endif
#ifdef USE_THAI_FONT
    INT32U dsp_thai_buffer[1][KI_DSP_BUF_SIZE];
	INT16U cvt_cur_pt;
#endif       
#if defined(USE_ARAB_FONT) || defined(HEBREW_FONT)
	INT8U	firstInputFlag;
	INT16U	convert_cur_pt;
	INT16U	arab_dsp_buffer[1][KI_DSP_BUF_SIZE];
#endif
	
#if defined(USE_SINGLE_LINE_GRAPHIC)	
	if (dsp_mode == KI_GRAPHIC) {
		x = 0; // 임시. 향후 좌표연동 고려.
		y = 0;
	}
#endif
	language_flag = get_global_bparam(GLOBAL_SALE_SETUP_FLAG4)&0x08;
	backup_directdraw = DspStruct.DirectDraw;
	if (pass == KI_NOPASS) DspStruct.DirectDraw = 1;
	use_lf = OFF;
	position_disp = OFF;
	page_updn_key = OFF;
	exclude_null = OFF;    
	english_mode = OFF;
#ifdef USE_CHINA_PINYIN
	china_pinyin = OFF;
#endif
	if(properties&MENU_P_USE_LINEFEED) use_lf = ON;
	if(properties&MENU_P_DSP_POSITION) position_disp = ON;
	if(properties&MENU_P_PAGEUPDN_KEY) page_updn_key = ON;
	if(properties&MENU_P_EXCLUDE_NULL) exclude_null = ON;	//OFF:string 끝에 Null을 무조건 추가, ON:string 끝에 Null을 추가안함    
	if(properties&MENU_P_ENGLISH_MODE) english_mode = ON;	//ON:영문 입력모드
#ifdef USE_CHINA_PINYIN
	if(properties&MENU_P_CHINA_PINY)   china_pinyin = ON;
#endif

	cursor.type = CURSOR_TYPE_DELETE;
#ifdef USE_CHINA_PINYIN
#else
	han_status  = HAN_INIT1;
#endif
	if (format) strcpy((char*)backup_format, (char*)format);
	//pre_key = 0;
	ret     = 0;
#ifdef USE_CHINA_PINYIN
	china_init();
#else
	korea_init();
#endif
	buffer = (HUGEDATA char *)ret_str;
	
	if (Startup.country==COUNTRY_KR || language_flag) korean_flag = ON;	//modified by JJANG 20090316 parameter 980 : Y
	
	if (english_mode)	// 영문 입력모드
	{
		backup_pgmmode = KeyDrv.PgmMode;
		KeyDrv.PgmMode = 0;	// => English Character
		if(Startup.country == COUNTRY_RU) KeyDrv.PgmMode = 1;	// => English Character	
	}
	
	if (KeyDrv.PgmMode) {
		display_sign_noflush(SIGN_LANG,1);
	} else {
		display_sign_noflush(SIGN_LANG,0);
	}
	VFD7_Diffuse();
	
	for(i = 0; i < leng; i++) {
		if(buffer[i] == 0x00) break;
	}
	
	if(input_mode == KI_IP_MODE) {
		for(;i < leng; i++) {
			if(format[i] == KeyInSet.char_fixed) {buffer[i] = KeyInSet.char_fixed;}
			else buffer[i] = 0x20;
		}
		buffer[leng] = 0x00;
	} else {
		for(;i < leng; i++) {
			buffer[i] = 0x00;
		}
	}
	
	switch(dsp_mode) {
		case KI_TARE:
			if(win_size > display_parameter.mode_len[DISPLAY_TARE])
				win_size = display_parameter.mode_len[DISPLAY_TARE];
			break;
		case KI_WEIGHT:
			//if(win_size > WEIGHT_DIGIT) win_size = WEIGHT_DIGIT;
			if(win_size > display_parameter.mode_len[DISPLAY_WEIGHT])
				win_size = display_parameter.mode_len[DISPLAY_WEIGHT];
			break;
		case KI_UNIT :
			//if(win_size > UNIT_PRICE_DIGIT) win_size = UNIT_PRICE_DIGIT;
			if(win_size > display_parameter.mode_len[DISPLAY_UNITPRICE])
				win_size = display_parameter.mode_len[DISPLAY_UNITPRICE];
			break;
		case KI_TOTAL :
			//if(win_size > TOTAL_PRICE_DIGIT) win_size = TOTAL_PRICE_DIGIT;
			if(win_size > display_parameter.mode_len[DISPLAY_PRICE])
				win_size = display_parameter.mode_len[DISPLAY_PRICE];
			break;
		case KI_SEGMENT :
			if(win_size > 25)
				win_size = 25;
			break;
		case KI_GRAPHIC :
		default :
			//if(win_size > 50) win_size = 50;
			break;
	}
#if defined(USE_ARAB_FONT) || defined(HEBREW_FONT)
#if defined(_USE_LCD37D_40DOT_)
	if(position_disp) {
		num_char = ilog(leng)*2 + 1;	// "XXX/YYY"
	}
	cur_pt = 0;
	string_win[0].start = 0;
	string_win[0].size = win_size;	
#else
	cur_pt = 0;
	string_win[0].start = 0;
	string_win[0].size = win_size;
	convert_cur_pt = 0; //gm 080820
	firstInputFlag = 1; //gm 080821
#endif
#else  
#if defined(USE_SINGLE_LINE_GRAPHIC)
	if(position_disp) {
		num_char = ilog(leng)*2 + 1;	// "XXX/YYY"
	}
#else
	if(position_disp) {
		num_char = ilog(leng)*2 + 1;	// "XXX/YYY"
		if(win_size > num_char) {
			win_size -= num_char;
		}
	}
#endif
#ifdef USE_VIETNAM_FONT
	cvt_cur_pt = 0; //gm 081024, cursor
#endif
#ifdef USE_THAI_FONT
	cvt_cur_pt = 0;
#endif  
	cur_pt = 0;
	string_win[0].start = 0;
	string_win[0].size = win_size;
#endif
	if(DspStruct.StrType == ONE_BYTE_FONT_STR) {
		string_win[0].dsp_size = string_win[0].size;
	} else {
		if(IsHanCharMid(buffer, string_win[0].start+string_win[0].size)==CHAR_HAN_MID) {		// 오른쪽끝이 한글 중간일 경우
			string_win[0].dsp_size = string_win[0].size - 1;
		} else {
			string_win[0].dsp_size = string_win[0].size;
		}
	}
	while(1) {
		str_len = strlen(buffer);
		if(str_len > leng) str_len = leng;
#ifdef USE_VIETNAM_FONT
		cvt_cur_pt = Cvt_VietnamPreview((INT8U *)buffer, leng, cur_pt, 1);
		
		if ((!KeyDrv.key_caps) && (KeyDrv.Shifted) && (KeyDrv.CnvCode != KEY_SHIFT)) KEY_Set_Shift(3);
		
		//[start]gm 081025, 현재 커서가 가리키는 문자의 ASCII 값을 VFD에 보여줌
		if (use_sec != 1)
		{
			sprintf(tmp_str, " %04X", buffer[cur_pt]&0x00ff);
			display_string(1, (INT8U *)tmp_str);
			VFD7_Diffuse();
		}
		//[end]gm 081025, 현재 커서가 가리키는 문자의 ASCII 값을 VFD에 보여줌
		
		if(cvt_cur_pt < string_win[0].start) {		// win 왼쪽 끝을 만났을 때
			string_win[0].start = cvt_cur_pt;
		} else if(cvt_cur_pt-string_win[0].start >= string_win[0].size - 1){		//win 오른쪽 끝을 만났을때
			string_win[0].start = cvt_cur_pt - (string_win[0].size - 1);
			if(DspStruct.StrType == ONE_BYTE_FONT_STR) {
				string_win[0].dsp_size = string_win[0].size;
			} else {
				if((INT8U)(buffer[cur_pt]) < 0x80) { //Eng
				} else { //Kor
					string_win[0].start++;
				}
				if(IsHanCharMid(buffer, string_win[0].start)==CHAR_HAN_MID) {		// 왼쪽 시작이 한글 중간일 경우
					string_win[0].start++;
				} else {
					string_win[0].dsp_size = string_win[0].size;
				}
			} 
		} // end if(cur_pt < string_win[0].start)
		if (use_sec == 1) {
			for (i=0, j=0; i<string_win[0].size; i++,j++) {
				dsp_viet_buffer[0][j] = (INT16U)KeyInSet.char_security;
				if(*(VietnamCode_buf + string_win[0].start + i) == 0x00 || i == string_win[0].dsp_size) {
					dsp_viet_buffer[0][j] = 0x0020;
				}
			}
		} else {
			for (i=0, j=0; i<string_win[0].size; i++,j++) {
				dsp_viet_buffer[0][j] = *(VietnamCode_buf + string_win[0].start+i);
				if(*(VietnamCode_buf + string_win[0].start + i) == 0x00 || i == string_win[0].dsp_size) {
					dsp_viet_buffer[0][j] = 0x0020;
				} else if(dsp_mode != KI_GRAPHIC) {
					if(*(VietnamCode_buf + string_win[0].start + i) == '.' || *(VietnamCode_buf + string_win[0].start + i) == ',') {
						dsp_viet_buffer[0][j+1] = *(VietnamCode_buf+string_win[0].start+i);
						dsp_viet_buffer[0][j] = 0x20;
						j++;
					}
				}
			}
		}
		dsp_viet_buffer[0][j] = 0x0000;
#endif	
#ifdef USE_THAI_FONT
		cvt_cur_pt = Cvt_ThaiPreview((INT8U *)buffer, leng, cur_pt, 1);

		if ((!KeyDrv.key_caps) && (KeyDrv.Shifted) && (KeyDrv.CnvCode != KEY_SHIFT)) KEY_Set_Shift(3);

		//[start]gm 081025, 현재 커서가 가리키는 문자의 ASCII 값을 VFD에 보여줌
		if (use_sec != 1)
		{
			sprintf(tmp_str, " %04X", buffer[cur_pt]&0x00ff);
			display_string(1, (INT8U *)tmp_str);
			VFD7_Diffuse();
		}
		//[end]gm 081025, 현재 커서가 가리키는 문자의 ASCII 값을 VFD에 보여줌

		if(cvt_cur_pt < string_win[0].start) {		// win 왼쪽 끝을 만났을 때
			string_win[0].start = cvt_cur_pt;
		} else if(cvt_cur_pt-string_win[0].start >= string_win[0].size - 1){		//win 오른쪽 끝을 만났을때
			string_win[0].start = cvt_cur_pt - (string_win[0].size - 1);
			if(DspStruct.StrType == ONE_BYTE_FONT_STR) {
				string_win[0].dsp_size = string_win[0].size;
			} else {
				if((INT8U)(buffer[cur_pt]) < 0x80) { //Eng
				} else { //Kor
					string_win[0].start++;
				}
				if(IsHanCharMid(buffer, string_win[0].start)==CHAR_HAN_MID) {		// 왼쪽 시작이 한글 중간일 경우
					string_win[0].start++;
				} else {
					string_win[0].dsp_size = string_win[0].size;
				}
			} 
		} // end if(cur_pt < string_win[0].start)
		if (use_sec == 1) {
			for (i=0, j=0; i<string_win[0].size; i++,j++) {
				dsp_thai_buffer[0][j] = (INT16U)KeyInSet.char_security;
				if(*(ThaiCode_buf + string_win[0].start + i) == 0x00 || i == string_win[0].dsp_size) {
					dsp_thai_buffer[0][j] = 0x0020;
				}
			}
		} else {
			for (i=0, j=0; i<string_win[0].size; i++,j++) {
				dsp_thai_buffer[0][j] = *(ThaiCode_buf + string_win[0].start+i);
				if(*(ThaiCode_buf + string_win[0].start + i) == 0x00 || i == string_win[0].dsp_size) {
					dsp_thai_buffer[0][j] = 0x0020;
				} else if(dsp_mode != KI_GRAPHIC) {
					if(*(ThaiCode_buf + string_win[0].start + i) == '.' || *(ThaiCode_buf + string_win[0].start + i) == ',') {
						dsp_thai_buffer[0][j+1] = *(ThaiCode_buf+string_win[0].start+i);
						dsp_thai_buffer[0][j] = 0x20;
						j++;
					}
				}
			}
		}
		dsp_thai_buffer[0][j] = 0x0000;
#endif
#ifdef USE_ARAB_FONT
#ifndef _USE_LCD37D_40DOT_
		//cur_pt: 현재 buffer[]에서의 위치, firstInputFlag: 초기 진입시 1회만 사용함
		if (input_mode != KI_IP_MODE) 
		{
#ifdef USE_ENG_BASED_ARAB_SUPPORT
			convert_cur_pt = Convert_ArabPreview((INT8U *)buffer, leng, cur_pt, firstInputFlag, 1); 
#else
			convert_cur_pt = Convert_ArabPreview((INT8U *)buffer, leng, cur_pt, firstInputFlag, 0);		
#endif
			if (firstInputFlag) firstInputFlag = 0;				 
			
			if(convert_cur_pt < string_win[0].start)// win 왼쪽 끝을 만났을 때 
			{
				string_win[0].start = convert_cur_pt;
			} 
			else if(convert_cur_pt - string_win[0].start >= string_win[0].size - 1) //win 오른쪽 끝을 만났을때
			{	
				string_win[0].start = convert_cur_pt - (string_win[0].size - 1);
				
				if(DspStruct.StrType == ONE_BYTE_FONT_STR) string_win[0].dsp_size = string_win[0].size;
				else {
					if((INT8U)(buffer[cur_pt]) < 0x80) { //Eng
					} else { //Kor
						string_win[0].start++;
					}
					if(IsHanCharMid(buffer, string_win[0].start)==CHAR_HAN_MID) {		// 왼쪽 시작이 한글 중간일 경우
						string_win[0].start++;
					} else {
						string_win[0].dsp_size = string_win[0].size;
					}
				} 
			}
			if (use_sec == 1) cursor.col = convert_cur_pt - string_win[0].start;
			else 
			{				
				for (i = 0; i < string_win[0].size; i++)
				{
#ifdef USE_ENG_BASED_ARAB_SUPPORT
					arab_dsp_buffer[0][i] = eng_codetemp[i + string_win[0].start];
#else
					arab_dsp_buffer[0][i] = arab_codetemp[i + string_win[0].start];
#endif
					if (arab_dsp_buffer[0][i] == 0x00) arab_dsp_buffer[0][i] = 0x20;		
				}
				//arab_dsp_buffer[0][string_win[0].size] = 0x00;
#ifdef USE_ENG_BASED_ARAB_SUPPORT
				arab_dsp_buffer[0][string_win[0].size] = ']';
#else
				arab_dsp_buffer[0][string_win[0].size] = '[';
#endif
				arab_dsp_buffer[0][string_win[0].size + 1] = 0x00;
			}
		}//IP_MODE		
#endif
#elif defined(HEBREW_FONT)
		//cur_pt: 현재 buffer[]에서의 위치, firstInputFlag: 초기 진입시 1회만 사용함
		//if (input_mode != KI_IP_MODE) 
		//{
			convert_cur_pt = Convert_HebrewPreview((INT8U *)buffer, leng, cur_pt, firstInputFlag);		
			
			if (firstInputFlag) firstInputFlag = 0; 			 

			if(convert_cur_pt < string_win[0].start)// win 왼쪽 끝을 만났을 때 
			{
				string_win[0].start = convert_cur_pt;
			} 
			else if(convert_cur_pt - string_win[0].start >= string_win[0].size - 1) //win 오른쪽 끝을 만났을때
			{	
				string_win[0].start = convert_cur_pt - (string_win[0].size - 1);
							
				if(DspStruct.StrType == ONE_BYTE_FONT_STR) string_win[0].dsp_size = string_win[0].size;
				else {
					if((INT8U)(buffer[cur_pt]) < 0x80) { //Eng
					} else { //Kor
						string_win[0].start++;
					}
					if(IsHanCharMid(buffer, string_win[0].start)==CHAR_HAN_MID) {		// 왼쪽 시작이 한글 중간일 경우
						string_win[0].start++;
					} else {
						string_win[0].dsp_size = string_win[0].size;
					}
				} 
			}
			if (use_sec == 1) cursor.col = convert_cur_pt - string_win[0].start;
			else 
			{				
				for (i = 0; i < string_win[0].size; i++)
				{
					arab_dsp_buffer[0][i] = hebrew_codetemp[i + string_win[0].start];
					if (arab_dsp_buffer[0][i] == 0x00) arab_dsp_buffer[0][i] = 0x20;		
				}
				arab_dsp_buffer[0][string_win[0].size] = '[';
				arab_dsp_buffer[0][string_win[0].size + 1] = 0x00;
			}
		//}//IP_MODE	
#endif
#if !defined(_USE_LCD20848_) && !defined(_USE_LCD32_)
		cntFixedChar = 0;
#endif
		if(use_sec == 1) {
			for (i=0, j=0; i<string_win[0].size; i++,j++) {
				dsp_buffer[0][j] = KeyInSet.char_security;
				if(*(buffer+string_win[0].start+i) == 0x00 || i == string_win[0].dsp_size) {
					dsp_buffer[0][j] = 0x20;
				}
			}
		} else {
			for (i=0, j=0; i<string_win[0].size; i++,j++) {
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
				dsp_buffer[0][j] = *(buffer+string_win[0].start+i);
#else
				if(dsp_mode == KI_GRAPHIC)	//IP
				{
					dsp_buffer[0][j] = *(buffer+string_win[0].start+i);
				}
				else
				{
					if (*(buffer+string_win[0].start+i+cntFixedChar) == KeyInSet.char_fixed) {
						dsp_buffer[0][j++] = *(buffer+string_win[0].start+i+cntFixedChar);
						cntFixedChar++;
					}
					dsp_buffer[0][j] = *(buffer+string_win[0].start+i+cntFixedChar);
				}
#endif
#if defined(USE_ARAB_FONT) || defined(HEBREW_FONT)
#ifndef _USE_LCD37D_40DOT_
				if(*(buffer+string_win[0].start+i) == 0x00) {
#else
				if(*(buffer+string_win[0].start+i) == 0x00 || i == string_win[0].dsp_size) {
#endif
#else
				if(*(buffer+string_win[0].start+i) == 0x00 || i == string_win[0].dsp_size) {
#endif
					dsp_buffer[0][j] = 0x20;
				}
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
				else if(dsp_mode != KI_GRAPHIC) {
					if(*(buffer+string_win[0].start+i) == '.' || *(buffer+string_win[0].start+i) == ',') {
						dsp_buffer[0][j+1] = *(buffer+string_win[0].start+i);
						dsp_buffer[0][j] = 0x20;
						j++;
					}
				}
#endif
			}
		}
		dsp_buffer[0][j] = 0x00;
#if !defined(USE_ARAB_FONT) && !defined(HEBREW_FONT)	//아랍어, 히브리어: Ingredient 개수 표시 X
#if defined(USE_SINGLE_LINE_GRAPHIC)
		if(position_disp) {
			sprintf(tmp_str, "%%%dd-%%%dd", (num_char-1)/2, (num_char-1)/2);
			sprintf(tmp_str2, tmp_str, cur_pt+1, leng);
			display_clear(DISPLAY_PRICE);
			display_string(DISPLAY_PRICE, (INT8U*)tmp_str2);
			VFD7_Diffuse();
		}
#else
		if(position_disp) {
			sprintf(tmp_str, "]%%%dd/%%%dd", (num_char-1)/2, (num_char-1)/2);
			sprintf(dsp_buffer[0]+string_win[0].size, tmp_str, cur_pt+1, leng);
			disp_p = point(y, x+string_win[0].size*display_font_get_width());
			Dsp_Write_String(disp_p, dsp_buffer[0]+string_win[0].size);
		}
#endif
#endif
			switch(dsp_mode) {
				case KI_TARE :
					//VFD7_str_weight(dsp_buffer[0]);
					display_clear(DISPLAY_TARE);
					display_string(DISPLAY_TARE, (INT8U*)dsp_buffer[0]);
					VFD7_Diffuse();
					break;
				case KI_WEIGHT:
					//VFD7_str_weight(dsp_buffer[0]);
					display_clear(DISPLAY_WEIGHT);
					display_string(DISPLAY_WEIGHT, (INT8U*)dsp_buffer[0]);
					VFD7_Diffuse();
					break;
				case KI_UNIT :
					//VFD7_str_unit(dsp_buffer[0]);
					display_clear(DISPLAY_UNITPRICE);
					display_string(DISPLAY_UNITPRICE, (INT8U*)dsp_buffer[0]);
					VFD7_Diffuse();
					break;
				case KI_TOTAL :
					// - VFD7_str_price(dsp_buffer[0]);
					display_clear(DISPLAY_PRICE);
					display_string(DISPLAY_PRICE, (INT8U*)dsp_buffer[0]);
					VFD7_Diffuse();
					break;
				case KI_SEGMENT :
					//display_clear(DISPLAY_PRICE);
					display_string_pos(x, (INT8U*)dsp_buffer[0]);
					VFD7_Diffuse();
					break;
				case KI_GRAPHIC:
				default :
					disp_p = point(y, x);
#ifdef USE_VIETNAM_FONT
					Dsp_Write_1string16(disp_p, dsp_viet_buffer[0]);
#elif defined(USE_THAI_FONT)
					Dsp_Write_1string32(disp_p, dsp_thai_buffer[0]);
#else
 #ifdef HEBREW_FONT		
				disp_p.x = 202 - disp_p.x;

				if (use_sec == 1 || input_mode == KI_IP_MODE)
				{

 					Convert_HebrewPreview((INT8U *)dsp_buffer[0], strlen(dsp_buffer[0]), 0, 0);
					hebrew_codetemp[strlen(dsp_buffer[0])] = 0;
					Dsp_Write_1stringLong(disp_p, hebrew_codetemp);

				}
				else 
				{
 					Dsp_Write_1stringLong(disp_p, (INT16U*)&arab_dsp_buffer[0]);
				}
 #elif defined(USE_ARAB_FONT)	//기존 아랍ROM 적용되어 있음
 #ifndef _USE_LCD37D_40DOT_
  #ifdef USE_ARAB_CONVERT
				disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;

				if (use_sec == 1 || input_mode == KI_IP_MODE) 
				{
					Convert_ArabPreview((INT8U *)dsp_buffer[0], strlen(dsp_buffer[0]), 0, 0, 2);	//IP Mode 경우 영문처럼 처리
					arab_codetemp[strlen(dsp_buffer[0])] = 0;
					Dsp_Write_1stringLong(disp_p, arab_codetemp);
				}
				else 
				{
					arab_codetemp[strlen(dsp_buffer[0])] = 0;
					Dsp_Write_1stringLong(disp_p, arab_codetemp);
				}
  #else
 				if (use_sec == 1 || input_mode == KI_IP_MODE) Dsp_Write_String(disp_p, dsp_buffer[0]);
				else 
				{
#ifdef USE_ENG_BASED_ARAB_SUPPORT
					//Dsp_Write_1stringLong(disp_p, eng_codetemp);
					Dsp_Write_1stringLong(disp_p, (INT16U*)&arab_dsp_buffer[0]);
#else
					DspStruct.DirectDraw = 1;
					disp_p.x += (strlen((char*)dsp_buffer)-1)*8;
					Dsp_Write_1stringLong(disp_p, (INT16U*)&arab_dsp_buffer[0]);
					DspStruct.DirectDraw = 0;
#endif	//USE_ENG_BASED_ARAB_SUPPORT
				}
  #endif	//USE_ARAB_CONVERT
#else
					Dsp_Write_String(disp_p, dsp_buffer[0]);
#endif	//_USE_LCD37D_40DOT
 #else
				Dsp_Write_String(disp_p, dsp_buffer[0]);
 #endif	//HEBREW_FONT
#endif	//USE_VIETNAM_FONT
					//DspStruct.DirectDraw = 0;
					break;
			}
			if(pass == KI_PASS) { ret=0; goto EX_ESC; };
			
#if defined(USE_ARAB_FONT) || defined(HEBREW_FONT)
		cursor.line = 0;			
		cursor.x_offset = x;	
		cursor.y_offset = y;
#endif
#if !defined(USE_ARAB_CONVERT) && defined(USE_ARAB_FONT) && !defined(USE_ENG_BASED_ARAB_SUPPORT)
		if (use_sec == 1 || input_mode == KI_IP_MODE) 
		{
			if(dsp_mode == KI_SEGMENT)	
			{
				cursor.col = cur_pt - string_win[0].start - fixedCharNum;	//CL3000 커서 위치 '.' 틀어지는 문제 반영
			}
			else
			{
				cursor.col = cur_pt - string_win[0].start;
			}
		}
		else
		{ 
			cursor.col = (strlen(dsp_buffer[0])-1) - (convert_cur_pt - string_win[0].start);
		}
		//[start]gm 080823 커서 위에 보여지는 문자		
		if (cur_pt == 0 || input_mode == KI_IP_MODE) { //처음 위치에 있을 때 or IP input mode
			cursor.chw.byte.left = buffer[cur_pt];
			cursor.chw.byte.right = buffer[cur_pt+1];
		} else {
			if (use_sec == 1) {
				//cursor.chw.byte.left = KeyInSet.char_security;
				//cursor.chw.byte.right = KeyInSet.char_security;
				cursor.chw.byte.left = 0x20;
				cursor.chw.byte.right = 0x20;
			} else {
				if(cursor_direct == 1) {
					cursor.chw.byte.left = buffer[cur_pt]; //다음 글자 표시 (변환 이전 폰트 표시)	
					if(cur_pt==0) cursor.chw.byte.right = 0x20;
					else cursor.chw.byte.right = buffer[cur_pt-1]; //마지막 글자표시
				} else if(cursor_direct == 2) {
					cursor.chw.byte.left = buffer[cur_pt-1]; //마지막 글자표시				
					cursor.chw.byte.right = buffer[cur_pt];	//다음 글자 표시	
				}
				//cursor.chw.byte.left = buffer[cur_pt-1];
				//cursor.chw.byte.right = buffer[cur_pt];
			}				
		}
#endif

#if defined(USE_ARAB_CONVERT) && !defined(USE_ENG_BASED_ARAB_SUPPORT)	//기존 아랍ROM 적용되어 있음
		cursor.col = convert_cur_pt - string_win[0].start;		//초기위치 0으로 변경

		if (convert_cur_pt == 0) { //처음 위치에 있을 때 or IP input mode
			if(cursor_direct == 1) {
				cursor.chw.words.left = arab_codetemp[convert_cur_pt];
				cursor.chw.words.right = arab_codetemp[convert_cur_pt+1];
			} else if(cursor_direct == 2) {
				cursor.chw.words.left = arab_codetemp[convert_cur_pt];
				cursor.chw.words.right = arab_codetemp[convert_cur_pt+1];
			}
		} else if(input_mode == KI_IP_MODE) {
			if(cursor_direct == 1) {
				cursor.chw.words.left = arab_codetemp[convert_cur_pt]; //다음 문자(커서 다음 위치)
				cursor.chw.words.right = arab_codetemp[convert_cur_pt+1];	//현재 문자 (커서 위치)
			} else if(cursor_direct == 2) {
				cursor.chw.words.left = arab_codetemp[convert_cur_pt];
				cursor.chw.words.right = arab_codetemp[convert_cur_pt+1];
			}
		} else {
			if (use_sec == 1) {
				cursor.chw.words.left = KeyInSet.char_security;
				cursor.chw.words.right = KeyInSet.char_security;
			} else {
				if(cursor_direct == 1) {
					cursor.chw.words.left = arab_codetemp[convert_cur_pt]; //다음 글자 표시 (변환 이전 폰트 표시)	 
					if(cur_pt==0) cursor.chw.words.right = 0x20;
					else cursor.chw.words.right = arab_codetemp[convert_cur_pt-1]; //마지막 글자표시
				} else if(cursor_direct == 2) {
					cursor.chw.words.left = arab_codetemp[convert_cur_pt]; //마지막 글자표시				
					cursor.chw.words.right = arab_codetemp[convert_cur_pt+1];	//다음 글자 표시			
				}
			}				
		}
#elif defined(HEBREW_FONT)
		cursor.col = convert_cur_pt - string_win[0].start;		//초기위치 0으로 변경
		//[start]gm 080823 커서 위에 보여지는 문자 		
		if (convert_cur_pt == 0) { //처음 위치에 있을 때 or IP input mode
			if(cursor_direct == 1) {
				cursor.chw.byte.left = hebrew_codetemp[convert_cur_pt];
				cursor.chw.byte.right = hebrew_codetemp[convert_cur_pt+1];
			} else if(cursor_direct == 2) {
				cursor.chw.byte.left = hebrew_codetemp[convert_cur_pt];
				cursor.chw.byte.right = hebrew_codetemp[convert_cur_pt+1];
			}
		} else if(input_mode == KI_IP_MODE) {
			if(cursor_direct == 1) {
				cursor.chw.byte.left = hebrew_codetemp[convert_cur_pt];	//다음 문자(커서 다음 위치)
				cursor.chw.byte.right = hebrew_codetemp[convert_cur_pt+1];	//현재 문자 (커서 위치)
			} else if(cursor_direct == 2) {
				cursor.chw.byte.left = hebrew_codetemp[convert_cur_pt];
				cursor.chw.byte.right = hebrew_codetemp[convert_cur_pt+1];
			}
		} else {
			if (use_sec == 1) {
				cursor.chw.byte.left = KeyInSet.char_security;
				cursor.chw.byte.right = KeyInSet.char_security;
			} else {
				if(cursor_direct == 1) {
					cursor.chw.byte.left = hebrew_codetemp[convert_cur_pt];	//다음 글자 표시	     
					if(cur_pt==0) cursor.chw.byte.right = 0x20;
					else cursor.chw.byte.right = hebrew_codetemp[convert_cur_pt-1];	//마지막 글자표시
				} else if(cursor_direct == 2) {
					cursor.chw.byte.left = hebrew_codetemp[convert_cur_pt];	//마지막 글자표시	       		
					cursor.chw.byte.right = hebrew_codetemp[convert_cur_pt+1];	//다음 글자 표시			
				}
			}				
		}
#else
			cursor.line = 0;
 #ifdef USE_VIETNAM_FONT
			cursor.col = cvt_cur_pt - string_win[0].start;
			cursor.y_offset = y;
			cursor.x_offset = x;
			if(use_sec == 1 && buffer[cur_pt] != 0x00) {
				cursor.chw.byte.left = KeyInSet.char_security;
				cursor.chw.byte.right = KeyInSet.char_security;
			} else {
				cursor.chw.byte.left = buffer[cur_pt];
				cursor.chw.byte.right = buffer[cur_pt+1];
			}
 #else
#if !defined(_USE_LCD20848_) && !defined(_USE_LCD32_)
			fixedCharNum = 0;
			if(input_mode == KI_IP_MODE) {
				for(i = string_win[0].start; i < cur_pt; i++) {
					if(format[i] == KeyInSet.char_fixed) fixedCharNum++;
				}
			}
 #endif
  #if !defined(USE_ARAB_FONT) || defined(USE_ENG_BASED_ARAB_SUPPORT)
  #if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
	#if defined(USE_THAI_FONT)  
			cursor.col = cvt_cur_pt - string_win[0].start;
	#else
			cursor.col = cur_pt - string_win[0].start;
	#endif
  #else
			cursor.col = cur_pt - string_win[0].start - fixedCharNum;
  #endif
			cursor.y_offset = y;
			cursor.x_offset = x;
			if (dsp_mode != KI_GRAPHIC && dsp_mode != KI_SEGMENT) cursor.x_offset = 0; // 7-seg 정렬용
			if(use_sec == 1 && buffer[cur_pt] != 0x00) {
				cursor.chw.byte.left = KeyInSet.char_security;
				cursor.chw.byte.right = KeyInSet.char_security;
			} else {
				cursor.chw.byte.left = buffer[cur_pt];
				cursor.chw.byte.right = buffer[cur_pt+1];
  #if defined(USE_THAI_FONT) 
  				cursor.thai_ch = ThaiCode_buf[cvt_cur_pt];
  #endif
			}
  #endif	//USE_ARAB_FONT
 #endif		//USE_VIETNAM_FONT
#endif	//USE_ARAB_CONVERT
			cursor.timer = (SysTimer100ms+1)%5;
			cursor.mode = dsp_mode;
			cursor.disp_on = 1;
			cursor.status = OFF;
			while(!KEY_Read()){
				//if(dsp_mode == KI_GRAPHIC) {
				cursor_blink(&cursor);
				//} else {
				//	cursor_blink_vfd(&cursor);
				//}
			}
			//DEL070619		if ((!KeyDrv.key_caps) && (KeyDrv.Shifted) && (KeyDrv.CnvCode != KEY_SHIFT)) KEY_Set_Shift(3);
			
			//SGTEST
			//sprintf(stemp,"%d,%d",KeyDrv.Type,KeyDrv.CnvCode);
			//MsgOut((HUGEDATA char*) stemp);
			
			switch(KeyDrv.Type) {
#ifdef USE_ARAB_FONT	
				case KEY_TYPE_COMBINED_ARAB : 
					if(KeyDrv.CnvCode >= KP_ARAB_00 && KeyDrv.CnvCode <= KP_ARAB_03) {
						if(cur_pt >= leng-1){ 
							BuzOn(2);	// 끝자리:입력은 되나 에러음 발생
							break;
						}
						if (KeyDrv.CnvCode == KP_ARAB_00){
							buffer[cur_pt++] = 0xE1; 
							buffer[cur_pt++] = 0xC2;
						} else if (KeyDrv.CnvCode == KP_ARAB_01) {
							buffer[cur_pt++] = 0xE1; 
							buffer[cur_pt++] = 0xC3;
						} else if (KeyDrv.CnvCode == KP_ARAB_02) {
							buffer[cur_pt++] = 0xE1; 
							buffer[cur_pt++] = 0xC5;
						} else if (KeyDrv.CnvCode == KP_ARAB_03) {
							buffer[cur_pt++] = 0xE1; 
							buffer[cur_pt++] = 0xC7;
						}
					} else {
						BuzOn(2);
						break;
					}
					BuzOn(1);
					break;
#endif			
				case KEY_TYPE_ASCII :
					if(pass == KI_NOEDIT || input_mode==KI_SNUMBER) {// Added by CJK 20041223 
						BuzOn(2);
						break;
					}
					if(input_mode == KI_FORM_MODE || input_mode == KI_IP_MODE) {
						if((KeyDrv.CnvCode >= KP_ASC_A && KeyDrv.CnvCode <= KP_ASC_Z)
								|| (KeyDrv.CnvCode >= KP_ASC_A_L && KeyDrv.CnvCode <= KP_ASC_Z_L)) {		// ALPHA
							if(format[cur_pt] == 'A' || format[cur_pt] == '#') {
							} else {
								BuzOn(2);							
								break;
							}
						} else {		// ASCII
							if(input_mode == KI_IP_MODE) {
								if(KeyDrv.CnvCode - KP_ASC_00 == KeyInSet.char_fixed) {
									for(i = cur_pt; buffer[i] != KeyInSet.char_fixed && i < leng; i++);
									if(i != leng) {// 중간에'.'이 있는경우
										BuzOn(1);
										for(j = cur_pt; j < i; j++);
										cur_pt = i + 1;		
									}
									else BuzOn(2);
									break;
								}
							}
							
							if(format[cur_pt] == '#') {
							} else {
								BuzOn(2);
								break;
							}
						}						
					}
					//------------------------------------------------------------------------------
					if(input_mode==KI_BARCODE_FORM) {
						uctemp = KeyDrv.CnvCode - KP_ASC_00;
						if( check_barcode_char(uctemp, (char*)&backup_format) == 0xff ) {		// Modified by CJK
							if(cur_pt < 1 || buffer[cur_pt-1] != '%') {
								BuzOn(2);
								break;
							}
						}
					}
					//--------------------------------------------------------------------------------   04.03.08    by getwing
					if(KeyDrv.CnvCode == KP_ASC_LF ||KeyDrv.CnvCode == KP_ASC_0C) {
						if(!use_lf) {
							BuzOn(2);
							break;
						}
					} else if(KeyDrv.CnvCode >= KP_ASC_00 && KeyDrv.CnvCode < KP_ASC_20) {
						BuzOn(2);
						break;
					}
					ch_code=KeyDrv.CnvCode - KP_ASC_00;
#ifdef USE_CHINA_PINYIN
				if (KeyDrv.PhoneType) {
					any_code_char(ch_code,&han_status,(INT8U *)buffer,&cur_pt,cursor.type,use_sec,leng);
				} else {
					if (KeyDrv.PgmMode==1) {
						china_code_auto(KeyDrv.CnvCode,&han_status,(INT8U*)buffer,&cur_pt,cursor.type,leng);
						any_press_buzzer(cur_pt,use_sec,leng);
					} else {
						any_code_char(ch_code,&han_status,(INT8U *)buffer,&cur_pt,cursor.type,use_sec,leng);
					}
				}
#else
					korea_code_char(KeyDrv.CnvCode,&han_status,(INT8U *)buffer,&cur_pt,cursor.type,use_sec,leng);
					//				    korea_code_eng(KeyDrv.CnvCode,&han_status,buffer,&cur_pt,cursor.type,use_sec,leng);
					//				    INT8U  korea_code_eng(INT16U code1,INT8U *han_status,INT8U *buffer,INT16U *cur_pt,INT8U ctype,INT8U use_sec,INT16U leng)
					//  		    if(cursor.type == CURSOR_TYPE_INSERT) {
					//  			for(i = cur_pt; buffer[i] != 0x00; i++);
					//  			if(i > leng - 1) {
					//  				BuzOn(2);
					//  				break;
					//  			}
					//  			for(; i > cur_pt; i--) {
					//  				buffer[i] = buffer[i - 1];
					//  			}
					//  		    }
					//				    buffer[cur_pt] = KeyDrv.CnvCode - KP_ASC_00;
					
					//				    if(cur_pt >= leng-1) {
					//					//BuzOn(2);	// 끝자리:입력은 되나 에러음 발생
					//					if(use_sec) BuzOn(1);
					//					else        BuzOn(2);
					//				    } else {
					//					BuzOn(1);
					//					if(DspStruct.StrType == ONE_BYTE_FONT_STR) {
					//					} else {
					//						if((INT8U)(buffer[cur_pt]) < 0x80) { //Eng
					//						} else { //Kor
					//							cur_pt++;
					//						}
					//					}
					//					cur_pt++;
					//				    }
					//				}
#endif
					if(input_mode == KI_IP_MODE) {
						if(format[cur_pt] == KeyInSet.char_fixed) {
							cur_pt++;
							if(dsp_mode == KI_SEGMENT)	//Segment 표시 경우 커서 위치 오류('.' 좌표 별도 처리 필요)
							{
								fixedCharNum++;
							}
						}
					}
					break;
				case KEY_TYPE_CODE:
					if(pass == KI_NOEDIT || input_mode==KI_SNUMBER) {
						BuzOn(2);
						break;
					}
					BuzOn(1);
					chcode=0;
					if (KeyDrv.CnvCode==KP_CODE_A1C9) {
						chcode=0xc9a1;
					}
#ifdef USE_CHN_FONT
					else if (KeyDrv.CnvCode==KP_CODE_A1E6) {
						chcode=0xe6a1;
					}
#endif                
					if (chcode) {
						if (input_mode==KI_FREE_MODE || input_mode==KI_BARCODE_FORM) {
							for (w=0; w<2; w++) {
								if (chcode==0) break;
								ch_code = (INT8U)( chcode&0xff );
								if (ch_code) korea_code_char(ch_code,(INT8U *)&han_status,(INT8U *)buffer,&cur_pt,cursor.type,use_sec,leng);
								chcode>>=8;
							}
						}
					}
					break;
				case KEY_TYPE_KOREA :
					if(pass == KI_NOEDIT || input_mode==KI_SNUMBER || input_mode==KI_BARCODE_FORM) {// Added by CJK 20041223 
						BuzOn(2);
						break;
					}
					korea_code_auto(KeyDrv.CnvCode,&han_status,(INT8U *)buffer,(INT16U *)&cur_pt,cursor.type,leng);
					if(cur_pt >= leng-1) {
						//BuzOn(2);	// 끝자리:입력은 되나 에러음 발생
						if(use_sec) BuzOn(1);
						else        BuzOn(2);
					} else {
						BuzOn(1);
					}
					break;
				case KEY_TYPE_NUMERIC :
#ifdef USE_CHINA_PINYIN
					if (KeyDrv.PhoneType) {
						if (china_ph_auto(KeyDrv.CnvCode,(INT8U*)buffer,&cur_pt,leng)) {
							any_press_buzzer(cur_pt,use_sec,leng);
						}
						break;
					}
#endif
					if(pass == KI_NOEDIT) {	// Added by CJK 20041223 
						BuzOn(2);
						break;
					}
					if (input_mode == KI_FORM_MODE || input_mode == KI_IP_MODE) {
						if(format[cur_pt] == 'A' || format[cur_pt] == '#') {
						} else if(format[cur_pt] >= '0' &&  format[cur_pt] <= '9') {
							if(KeyDrv.CnvCode <= format[cur_pt] - '0' + KEY_NUM_0) {
							} else {
								BuzOn(2);
								break;
							}
						} else if(format[cur_pt] == '1') {
							if(KeyDrv.CnvCode == KEY_NUM_0 || KeyDrv.CnvCode == KEY_NUM_1) {
							} else {
								BuzOn(2);
								break;
							}
						} else {
							BuzOn(2);
							break;
						}
					}
					//------------------------------------------------------------------------------
					if(input_mode==KI_BARCODE_FORM) {
						uctemp = KeyDrv.CnvCode - KEY_NUM_0 + '0';								
						//if( !search_barcode_char(pre_key, uctemp, (char*)&backup_format) ) {
						if( check_barcode_char(uctemp, (char*)&backup_format) == 0xff ) {		// Modified by CJK
							if(cur_pt < 1 || buffer[cur_pt-1] != '%') {
								BuzOn(2);
								break;
							}
						}		
					}
					j = 1;
					if(KeyDrv.CnvCode == KEY_NUM_10) 
					{
						j = get_global_bparam(GLOBAL_KEY_NUMBERZERO_N);
						KeyDrv.CnvCode = KEY_NUM_0;
					}
					ch_code= KeyDrv.CnvCode - KEY_NUM_0 + '0';
#ifdef USE_CHINA_PINYIN
					if (china_code_auto(KeyDrv.CnvCode,&han_status,(INT8U*)buffer,&cur_pt,cursor.type,leng)) {
						any_press_buzzer(cur_pt,use_sec,leng);
						break;
					}
					any_code_char(ch_code,(INT8U *)&han_status,(INT8U *)buffer,&cur_pt,cursor.type,use_sec,leng);
#else
					for (i = 0; i < j; i++)
					{
						korea_code_char(ch_code,(INT8U *)&han_status,(INT8U *)buffer,&cur_pt,cursor.type,use_sec,leng);
					}
#endif
					if(input_mode == KI_IP_MODE) {
						if(format[cur_pt] == KeyInSet.char_fixed) {
							cur_pt++;
							if(dsp_mode == KI_SEGMENT)	//Segment 표시 경우 커서 위치 오류('.' 좌표 별도 처리 필요)
							{
								fixedCharNum++;
							}
						}
					}
					break;
				case KEY_TYPE_FUNCTION :
#ifdef USE_CHINA_PINYIN
					if (KeyDrv.PhoneType) {
						if (china_ph_auto(KeyDrv.CnvCode,(INT8U*)buffer,&cur_pt,leng)) {
							any_press_buzzer(cur_pt,use_sec,leng);
							break;
						}
					} else {
						if (china_code_auto(KeyDrv.CnvCode,&han_status,(INT8U*)buffer,&cur_pt,cursor.type,leng)) {
							any_press_buzzer(cur_pt,use_sec,leng);
							break;
						}
					}
#endif
				default :
					if (input_mode!=KI_SNUMBER) {
						if (korean_flag) {
							if (!((KeyDrv.CnvCode==KEY_SHIFT) || (KeyDrv.CnvCode==KP_CHANGELANG))) {
								korea_complete(&han_status,(INT8U *)buffer,&cur_pt,leng);
								korea_first_noinsert=0;
							}
						}
					}
					switch(KeyDrv.CnvCode)
					{
						case KP_PAGE_UP :
							if(page_updn_key) {
								if(pass == KI_NOEDIT) {	// Added by CJK 20041223 
									BuzOn(2);
									break;
								}
								if(cur_pt == 0) {
									BuzOn(2);
								} else if(cur_pt < string_win[0].size) {
									BuzOn(1);
									cur_pt = 0;
								} else {
									BuzOn(1);
									if(DspStruct.StrType == ONE_BYTE_FONT_STR) {
									} else {
										if(cur_pt - string_win[0].size > 1) {
											if(IsHanCharMid(buffer, cur_pt-string_win[0].size-1)==CHAR_HAN_START) {
												cur_pt --;
											}
										}
									}
									cur_pt -= string_win[0].size;
									if(input_mode == KI_IP_MODE) {
										if(format[cur_pt] == KeyInSet.char_fixed) {
											if(cur_pt > 0) {
												cur_pt--;
												if(dsp_mode == KI_SEGMENT)	//Segment 표시 경우 커서 위치 오류('.' 좌표 별도 처리 필요)
												{
													fixedCharNum--;
												}
											}
										}
									}
								}
								break;
							}
						case KP_PAGE_DN : 
							if(page_updn_key) {
								if(pass == KI_NOEDIT) {	// Added by CJK 20041223 
									BuzOn(2);
									break;
								}
								if(cur_pt >= leng - 1) {
									BuzOn(2);
								} else if(cur_pt + string_win[0].size >= leng) {
									BuzOn(1);
									cur_pt = leng - 1;
								} else {
									//if(buffer[cur_pt+string_win[0].size] == 0x00) {
									if(cur_pt >= str_len) {
										BuzOn(2);
									} else if(cur_pt + string_win[0].size > str_len) {
										BuzOn(1);
										//cur_pt = leng - 1;
										cur_pt = str_len;
									} else {
										BuzOn(1);
										if(DspStruct.StrType == ONE_BYTE_FONT_STR) {
										} else {
											if((INT8U)(buffer[cur_pt+string_win[0].size]) < 0x80) { //Eng
											} else { //Kor
												cur_pt++;
											}
										}
										cur_pt += string_win[0].size;
									}
									if(input_mode == KI_IP_MODE) {
										if(format[cur_pt] == KeyInSet.char_fixed) {
											if(cur_pt < leng - 1) {
												cur_pt++;
												if(dsp_mode == KI_SEGMENT)	//Segment 표시 경우 커서 위치 오류('.' 좌표 별도 처리 필요)
												{
													fixedCharNum++;
												}
											}
										}
									}
								}
								break;
							}
						case KP_ARROW_DN :
						case KP_ARROW_UP :
						case KP_TEST :
						case KP_ZERO :
						case KP_TARE :
						case KP_FEED :
						case KP_ENTER :
						case KP_SAVE :
							BuzOn(1);
							cursor_clear(&cursor);
							if(input_mode == KI_IP_MODE) {
								ip_start = 0;
								ip_is_ch = 0;
								for(i = 0; buffer[i] != 0x00 && i < leng; i++) {
									if(buffer[i] == 0x20) {
										if(ip_is_ch) {
											//shift right
											for(j = i; j > ip_start; j--) {
												buffer[j] = buffer[j-1];
											}
											buffer[ip_start] = '0';
										} else {
											buffer[i] = '0';
										}
									} else if(buffer[i] == KeyInSet.char_fixed) {
										ip_start = i + 1;
										ip_is_ch = 0;
										//buffer[i] = KeyInSet.char_fixed;
									} else {
										ip_is_ch = 1;
										//buffer[i] = buffer[i];
									}
								}
								j = 0;
								for(i = 0; buffer[i] != 0x00 && i < leng; i++, j++) {
									//								if(buffer[i] == KeyInSet.char_fixed) i++;
									ret_str[j] = buffer[i];
								}
								if (j<leng) ret_str[j] = 0x00;
								//if (!keyin_string_flag) ret_str[j] = 0x00;
								if (!exclude_null) ret_str[j] = 0x00;
							} else {
								for(i = 0; buffer[i] != 0x00 && i < leng; i++) {
									ret_str[i] = buffer[i];
								}
								if (i<leng) ret_str[i] = 0x00;
								//if (!keyin_string_flag) ret_str[i] = 0x00;
								if (!exclude_null) ret_str[i] = 0x00;
								
							}
							ret=KeyDrv.CnvCode;
							goto EX_ESC;
						case KP_ESC : 
							BuzOn(1);
							cursor_clear(&cursor);
							ret=KeyDrv.CnvCode;
							goto EX_ESC;
						case KEY_SHIFT :
							BuzOn(1);
							KEY_Set_Shift(3);
							break;
						case KP_CAPS :
							BuzOn(1);
							KEY_Set_Caps();
							break;
#ifdef USE_CHINA_PINYIN
						case KP_PHONETYPE:
	//						if (KeyDrv.PhoneType) KeyDrv.PhoneType=0;
	//						else                  KeyDrv.PhoneType=1;
							any_language_toggle(1);
							BuzOn(1);
							break;
#endif
						case KP_CHANGELANG:
#ifdef USE_CHINA_PINYIN
							if (!KeyDrv.PhoneType) china_complete((INT8U *)buffer,&cur_pt,leng,1);
							any_language_toggle(0);
#else
  #ifdef USE_USA_DEFAULT
							if(language_flag)
							{
  #endif
							if (KeyDrv.PgmMode) KeyDrv.PgmMode=0;
							else                KeyDrv.PgmMode=1;
							if (KeyDrv.PgmMode) {
								display_sign_noflush(SIGN_LANG,1);
							} else {
								display_sign_noflush(SIGN_LANG,0);
							}
  #ifdef USE_USA_DEFAULT
							}
							else
							{
								BuzOn(2);
								break;
							}
  #endif							
#endif
							VFD7_Diffuse();
							BuzOn(1);
							break;
						case KP_CHAR_CODE:
							if(pass == KI_NOEDIT || input_mode==KI_SNUMBER) {// Added by CJK 20041223 
								BuzOn(2);
								break;
							}
							BuzOn(1);
							if (input_mode==KI_FREE_MODE || input_mode==KI_BARCODE_FORM) {
								chcode=keyin_code();
								// bmlim code
								for (w=0; w<2; w++) {
									if (chcode==0) break;
									ch_code = (INT8U)( chcode&0xff );
									if (ch_code) korea_code_char(ch_code,(INT8U *)&han_status,(INT8U *)buffer,&cur_pt,cursor.type,use_sec,leng);
									chcode>>=8;
								}
								// ---------------------------
								// bmlim end
								// --------------------------------------------
							}
							break;
						case KP_INSERT :
							if(pass == KI_NOEDIT) {	// Added by CJK 20041223 
								BuzOn(2);
								break;
							}
							if(input_mode == KI_FORM_MODE || input_mode == KI_IP_MODE) {
								BuzOn(2);
								break;
							}
							BuzOn(1);
							if(cursor.type == CURSOR_TYPE_INSERT) {
								cursor.type = CURSOR_TYPE_DELETE;
							} else {
								cursor.type = CURSOR_TYPE_INSERT;
							}
							break;
						case KP_DEL :
							if(pass == KI_NOEDIT) {	// Added by CJK 20041223 
								BuzOn(2);
								break;
							}
							if(input_mode == KI_FORM_MODE || input_mode == KI_IP_MODE) {
								BuzOn(2);
								break;
							}
							if(buffer[cur_pt] == 0x00) {
								BuzOn(2);
								break;
							}
							BuzOn(1);
							if(DspStruct.StrType == ONE_BYTE_FONT_STR) {
#ifdef USE_THAI_FONT                                       
								j = CheckThaiChNum(ThaiCode_buf[cvt_cur_pt]);
								for(i = cur_pt; buffer[i] != 0x00; i++) {
									buffer[i] = buffer[i + 1 + j];
								}
#else
								for(i = cur_pt; buffer[i] != 0x00; i++) {
									buffer[i] = buffer[i + 1];
								}
#endif
							} else {
								if((INT8U)(buffer[cur_pt]) < 0x80) { //Eng
									for(i = cur_pt; buffer[i] != 0x00; i++) {
										buffer[i] = buffer[i + 1];
									}
								} else { //Kor
									for(i = cur_pt; buffer[i] != 0x00; i++) {
										buffer[i] = buffer[i + 2];
									}
								}
							}
							break;
						case KP_BS :
							if(pass == KI_NOEDIT) {	// Added by CJK 20041223 
								BuzOn(2);
								break;
							}
							if(input_mode == KI_FORM_MODE || input_mode == KI_IP_MODE) {
								BuzOn(2);
								break;
							}
							if(cur_pt == 0) {
								BuzOn(2);
							} else {	// cur_pt >= 1
								BuzOn(1);
								if(DspStruct.StrType == ONE_BYTE_FONT_STR) {
									cur_pt--;
									for(i = cur_pt; buffer[i] != 0x00; i++) {
										buffer[i] = buffer[i + 1];
									}
								} else {
									i=IsHanCharMid(buffer, cur_pt-1);
									if ((i==CHAR_HAN_MID) || (i==CHAR_HAN_START)) { //Kor
										cur_pt = cur_pt - 2;
										for(i = cur_pt; buffer[i] != 0x00; i++) {
											buffer[i] = buffer[i + 2];
										}
									} else { //Eng
										cur_pt--;
										for(i = cur_pt; buffer[i] != 0x00; i++) {
											buffer[i] = buffer[i + 1];
										}
									}
								}
							}
							break;
#if defined(USE_ARAB_CONVERT) || defined(HEBREW_FONT)
						case KP_ARROW_LF :
							if(pass == KI_NOEDIT) {	// Added by CJK 20041223 
								BuzOn(2);
								break;
							}
							if(cur_pt >= leng) {
								BuzOn(2);
							} else {
								if(buffer[cur_pt] == 0x00) {
									BuzOn(2);
								} else {
									BuzOn(1);
									if(DspStruct.StrType == ONE_BYTE_FONT_STR) {
									} else {
										if((INT8U)(buffer[cur_pt]) < 0x80) { //Eng
										} else { //Kor
											cur_pt++;
										}
									}
									cur_pt++;
								}
								if(input_mode == KI_IP_MODE) {
									if(format[cur_pt-1] == KeyInSet.char_fixed) {
										if(cur_pt < leng - 1) {
											cur_pt++;
										}
									}
								}
							}
							break;
						case KP_ARROW_RI :
							if(pass == KI_NOEDIT) {	// Added by CJK 20041223 
								BuzOn(2);
								break;
							}
							if(cur_pt == 0) {
								BuzOn(2);
							} else {
								BuzOn(1);
								if(DspStruct.StrType == ONE_BYTE_FONT_STR) {
								} else {
									if(cur_pt > 1) {
										if(IsHanCharMid(buffer, cur_pt-2)==CHAR_HAN_START) {
											cur_pt--;
										}
									}
								}
								cur_pt--;
								if(input_mode == KI_IP_MODE) {
									if(format[cur_pt-1] == KeyInSet.char_fixed) {
										if(cur_pt > 0) {
											cur_pt--;
										}
									}
								}
							}
							break;			
#elif defined(USE_THAI_FONT)  
						case KP_ARROW_LF :
							if(pass == KI_NOEDIT) { // Added by CJK 20041223 
								BuzOn(2);
								break;
							}
							if(cur_pt == 0) {
								BuzOn(2);
							} else {
								BuzOn(1);
								if(DspStruct.StrType == ONE_BYTE_FONT_STR) {
																	if(cur_pt > 1) {
																			if(cvt_cur_pt == 0) cur_pt = cur_pt - CheckThaiChNum(ThaiCode_buf[cvt_cur_pt]);
																			else cur_pt = cur_pt - CheckThaiChNum(ThaiCode_buf[cvt_cur_pt - 1]);
																	}
								} else {
									if(cur_pt > 1) {
										if(IsHanCharMid(buffer, cur_pt-2)==CHAR_HAN_START) {
											cur_pt--;
										}
									}
								}
								cur_pt--;
								if(input_mode == KI_IP_MODE) {
									if(format[cur_pt] == KeyInSet.char_fixed) {
										if(cur_pt > 0) {
											cur_pt--;
											if(dsp_mode == KI_SEGMENT)	//Segment 표시 경우 커서 위치 오류('.' 좌표 별도 처리 필요)
											{
												fixedCharNum--;
											}
										}
									}
								}
							}
							break;
						case KP_ARROW_RI :
							if(pass == KI_NOEDIT) { // Added by CJK 20041223 
								BuzOn(2);
								break;
							}
							if(cur_pt >= leng-1) {
								BuzOn(2);
							} else {
								if(buffer[cur_pt] == 0x00) {
									BuzOn(2);
								} else {
									BuzOn(1);
									if(DspStruct.StrType == ONE_BYTE_FONT_STR) {
																			if(ThaiCode_buf[cvt_cur_pt] >= 0x80) cur_pt = cur_pt + CheckThaiChNum(ThaiCode_buf[cvt_cur_pt]);
																			if(cur_pt >= leng-1) {
														BuzOn(2);
																					cur_pt = cur_pt - CheckThaiChNum(ThaiCode_buf[cvt_cur_pt]) - 1;
																			}
									} else {
										if((INT8U)(buffer[cur_pt]) < 0x80) { //Eng
										} else { //Kor
											cur_pt++;
										}
									}
									cur_pt++;
								}
								if(input_mode == KI_IP_MODE) {
									if(format[cur_pt] == KeyInSet.char_fixed) {
										if(cur_pt < leng - 1) {
											cur_pt++;
										}
									}
								}
							}
							break; 
#else
						case KP_ARROW_LF :
							if(pass == KI_NOEDIT) {	// Added by CJK 20041223 
								BuzOn(2);
								break;
							}
							if(cur_pt == 0) {
								BuzOn(2);
							} else {
								BuzOn(1);
								if(DspStruct.StrType == ONE_BYTE_FONT_STR) {
								} else {
									if(cur_pt > 1) {
										if(IsHanCharMid(buffer, cur_pt-2)==CHAR_HAN_START) {
											cur_pt--;
										}
									}
								}
								cur_pt--;
								if(input_mode == KI_IP_MODE) {
									if(format[cur_pt] == KeyInSet.char_fixed) {
										if(cur_pt > 0) {
											cur_pt--;
										}
									}
								}
							}
							break;
						case KP_ARROW_RI :
							if(pass == KI_NOEDIT) {	// Added by CJK 20041223 
								BuzOn(2);
								break;
							}
							if(cur_pt >= leng-1) {
								BuzOn(2);
							} else {
								if(buffer[cur_pt] == 0x00) {
									BuzOn(2);
								} else {
									BuzOn(1);
									if(DspStruct.StrType == ONE_BYTE_FONT_STR) {
									} else {
										if((INT8U)(buffer[cur_pt]) < 0x80) { //Eng
										} else { //Kor
											cur_pt++;
										}
									}
									cur_pt++;
								}
								if(input_mode == KI_IP_MODE) {
									if(format[cur_pt] == KeyInSet.char_fixed) {
										if(cur_pt < leng - 1) {
											cur_pt++;
											if(dsp_mode == KI_SEGMENT)	//Segment 표시 경우 커서 위치 오류('.' 좌표 별도 처리 필요)
											{
												fixedCharNum++;
											}
										}
									}
								}
							}
							break;
#endif
						case KP_CLEAR :
							if(pass == KI_NOEDIT) {	// Added by CJK 20041223 
								BuzOn(2);
								break;
							}
#if defined(USE_ARAB_FONT) || defined(HEBREW_FONT)
#ifndef _USE_LCD37D_40DOT_
							cur_pt = 0;
							string_win[0].start = 0;
							cursor.col = 0; //gm 080819						
							convert_cur_pt = 0;//gm 080821
							memset(buffer, 0x20, leng);//gm 080820 ??? YOO 100929 ???
#else
							cur_pt = 0;
							string_win[0].start = 0;
#endif
#else
							cur_pt = 0;
#ifdef USE_VIETNAM_FONT
							cvt_cur_pt = 0;
#endif
#ifdef USE_THAI_FONT
							cvt_cur_pt = 0;
#endif 
							string_win[0].start = 0;
#endif    
							//					#endif
#ifdef USE_CHINA_PINYIN
						//if (Startup.country==COUNTRY_CHA) {
						if (china_init()) break;
						//}
#else
							//						if (Startup.country==COUNTRY_KR) {
							
							if (korean_flag) {
								han_status=HAN_INIT1;
								korea_init();
							}
#endif
							if(input_mode == KI_IP_MODE) {
								BuzOn(1);
								for(i = 0; format[i] != 0x00; i++) {
									if(format[i] == KeyInSet.char_fixed) buffer[i] = format[i];
									else buffer[i] = 0x20;
								}
							} else {
								BuzOn(1);
								for(i = 0; i < leng; i++) {
									buffer[i] = 0x00;
								}
							}
							if (format) strcpy((char*)backup_format, (char*)format);
							break;
						
#ifdef USE_HYUNDAI_SET_FUNCTION
					  case KS_SCANPRINT:
						  ret=KeyDrv.CnvCode;
						  goto EX_ESC;
#endif
						default : 
							BuzOn(2);
					}
					break;
			}
			if ((!KeyDrv.key_caps) && (KeyDrv.Shifted) && (KeyDrv.CnvCode != KEY_SHIFT)) KEY_Set_Shift(3);
#if defined(USE_ARAB_FONT) || defined(HEBREW_FONT)
			if (input_mode == KI_IP_MODE){
/*				fixedCharNum = 0;
				if(input_mode == KI_IP_MODE) {
					for(i = string_win[0].start; i < cur_pt; i++) {
						if(format[i] == KeyInSet.char_fixed) fixedCharNum++;
					}
				}
*/
				if(cur_pt < string_win[0].start) {		// win 왼쪽 끝을 만났을 때
					string_win[0].start = cur_pt;
				}	//else if(cur_pt-string_win[0].start - fixedCharNum >= string_win[0].size){		//win 오른쪽 끝을 만났을때
				else if(cur_pt-string_win[0].start >= string_win[0].size){		//win 오른쪽 끝을 만났을때
					//string_win[0].start = cur_pt - fixedCharNum - (string_win[0].size - 1);
					string_win[0].start = 0;
					/*if(input_mode == KI_IP_MODE) {
						if(format[string_win[0].start] == KeyInSet.char_fixed) string_win[0].start++;
					}*/
					if(DspStruct.StrType == ONE_BYTE_FONT_STR) {
						string_win[0].dsp_size = string_win[0].size;
					} else {
						if((INT8U)(buffer[cur_pt]) < 0x80) { //Eng
						} else { //Kor
							string_win[0].start++;
						}
						if(IsHanCharMid(buffer, string_win[0].start)==CHAR_HAN_MID) {		// 왼쪽 시작이 한글 중간일 경우
							string_win[0].start++;
						} else {
							string_win[0].dsp_size = string_win[0].size;
						}
					} 
				} // end if(cur_pt < string_win[0].start)
			}
#elif defined(USE_THAI_FONT)
			if(cvt_cur_pt < string_win[0].start) {		// win 왼쪽 끝을 만났을 때
				string_win[0].start = cvt_cur_pt;
			} else if(cvt_cur_pt-string_win[0].start >= string_win[0].size - 1){		//win 오른쪽 끝을 만났을때
				string_win[0].start = cvt_cur_pt - (string_win[0].size - 1);
				if(DspStruct.StrType == ONE_BYTE_FONT_STR) {
					string_win[0].dsp_size = string_win[0].size;
				} else {
					if((INT8U)(buffer[cur_pt]) < 0x80) { //Eng
					} else { //Kor
						string_win[0].start++;
					}
					if(IsHanCharMid(buffer, string_win[0].start)==CHAR_HAN_MID) {		// 왼쪽 시작이 한글 중간일 경우
						string_win[0].start++;
					} else {
						string_win[0].dsp_size = string_win[0].size;
					}
				} 
			} // end if(cur_pt < string_win[0].start)  
#else
#if !defined(_USE_LCD20848_) && !defined(_USE_LCD32_)
			fixedCharNum = 0;
			if(input_mode == KI_IP_MODE) {
				for(i = string_win[0].start; i < cur_pt; i++) {
					if(format[i] == KeyInSet.char_fixed) fixedCharNum++;
				}
			}
#endif
			if(cur_pt < string_win[0].start) {		// win 왼쪽 끝을 만났을 때
				string_win[0].start = cur_pt;
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
			} else if(cur_pt-string_win[0].start >= string_win[0].size-1){		//win 오른쪽 끝을 만났을때
				string_win[0].start = cur_pt - (string_win[0].size - 1);
#else
			} else if(cur_pt-string_win[0].start - fixedCharNum >= string_win[0].size-1){		//win 오른쪽 끝을 만났을때
				string_win[0].start = cur_pt - fixedCharNum - (string_win[0].size - 1);
				if(input_mode == KI_IP_MODE) {
					if(format[string_win[0].start] == KeyInSet.char_fixed) string_win[0].start++;
				}
#endif
				if(DspStruct.StrType == ONE_BYTE_FONT_STR) {
					string_win[0].dsp_size = string_win[0].size;
				} else {
					if((INT8U)(buffer[cur_pt]) < 0x80) { //Eng
					} else { //Kor
						string_win[0].start++;
					}
					if(IsHanCharMid(buffer, string_win[0].start)==CHAR_HAN_MID) {		// 왼쪽 시작이 한글 중간일 경우
						string_win[0].start++;
					} else {
						string_win[0].dsp_size = string_win[0].size;
					}
				} 
			} // end if(cur_pt < string_win[0].start)
#endif
#ifdef USE_ENG_BASED_ARAB_SUPPORT
			if(cur_pt < string_win[0].start) {		// win 왼쪽 끝을 만났을 때
					string_win[0].start = cur_pt;
				} else if(cur_pt-string_win[0].start >= string_win[0].size - 1){		//win 오른쪽 끝을 만났을때
					string_win[0].start = cur_pt - (string_win[0].size - 1);
					if(DspStruct.StrType == ONE_BYTE_FONT_STR) {
						string_win[0].dsp_size = string_win[0].size;
					} else {
						if((INT8U)(buffer[cur_pt]) < 0x80) { //Eng
						} else { //Kor
							string_win[0].start++;
						}
						if(IsHanCharMid(buffer, string_win[0].start)==CHAR_HAN_MID) {		// 왼쪽 시작이 한글 중간일 경우
							string_win[0].start++;
						} else {
							string_win[0].dsp_size = string_win[0].size;
						}
					} 
				} // end if(cur_pt < string_win[0].start)
#endif
			
			if(DspStruct.StrType == ONE_BYTE_FONT_STR) {
				string_win[0].dsp_size = string_win[0].size;
			} else {
				if(IsHanCharMid(buffer, string_win[0].start+string_win[0].size)==CHAR_HAN_MID) {		// 오른쪽끝이 한글 중간일 경우
					string_win[0].dsp_size = string_win[0].size - 1;
				} else if(buffer[cur_pt] == 0x00) {
					string_win[0].dsp_size = string_win[0].size - 1;
				} else {
					string_win[0].dsp_size = string_win[0].size;
				}
			} // end if(DspStruct.StrType == ONE_BYTE_FONT_STR) {
		}
		EX_ESC:
#ifdef USE_CHINA_PINYIN
		KeyDrv.PgmMode  =0;
		KeyDrv.PhoneType=0;
#endif
		if (pass != KI_PASS) KEY_Set_Shift(0);
		display_sign_noflush(SIGN_LANG,0);	// string 입력시에만 표시하고, 끝날때 표시만 없앰
		VFD7_Diffuse();
		if (english_mode)
		{
			KeyDrv.PgmMode = backup_pgmmode;
		}
		DspStruct.DirectDraw = backup_directdraw;
		if (dsp_mode == KI_SEGMENT)
		{
			display_string_pos_clear(x, DSP_MAX_DIGIT);
		}
		return ret;
	}
	
	INT32U PrevSpeedKeyPluNo = 0;
	INT16U PrevKeySetNo = 0;
	INT16U PrevKeyNo = 0;
	extern long		speedkey_set_number;
	extern long		speedkey_key_number;
	INT16U keyin_value(INT8U data_type, INT8U dsp_mode, INT8U format, INT8U decimal, INT32U ret_addr, INT8U leng, INT8U y, INT16U x, INT8U use_sec, INT8U pass)
	{
		long temp_val,sub_val;
		char buffer[20];
		char temp_buffer[20];
		INT16U l_space;
		INT8U i, win_size, overlength, buf_size;//, overlength;
		POINT disp_p;
		INT16U ret;
		MENU_CURSOR_STR cursor;
		INT8U key_cnt;
		INT8U char_cnt;
		INT8U insert_pt;
		INT8U first_input;
		INT32U digit10[10] = {1l, 10l, 100l, 1000l, 10000l, 100000l, 1000000l, 10000000l, 100000000l, 1000000000l};
		INT8U flagOverVal;
		INT16S nzero;
		INT8U backup_directdraw;
		//INT32U retval;
		INT32U *ret_val;
		INT32U ret_val4;
		INT16U ret_val2;
		INT8U  ret_val1;
		INT8U  ch;
		
		////////////////	
		ret_val = &ret_val4;
		switch (data_type)
		{
			case 1:
				memcpy(&ret_val1, (void*)ret_addr, 1);
				*ret_val = (INT32U)ret_val1;
				break;
			case 2:
				memcpy(&ret_val2, (void*)ret_addr, 2);
				*ret_val = (INT32U)ret_val2;
				break;
			case 4:
			default:
				memcpy(&ret_val4, (void*)ret_addr, 4);
				//*ret_val = (INT32U)ret_val4;
				break;
		}
		///////////////////
		
		backup_directdraw = DspStruct.DirectDraw;
		if (pass == KI_NOPASS) DspStruct.DirectDraw = 1;
		cursor.type = CURSOR_TYPE_DELETE;
		overlength  = 0;
		first_input = 1;
		ret         = 0;
		if(leng > 9)  leng = 9;	// 최대 입력 가능한 Digit 수:9
		if(leng == 0) leng = 1;
		switch(dsp_mode) {
			case KI_TARE:
				win_size = display_parameter.mode_len[DISPLAY_TARE];
				break;
			case KI_WEIGHT:
				win_size = display_parameter.mode_len[DISPLAY_WEIGHT];
				break;
			case KI_UNIT :
				win_size = display_parameter.mode_len[DISPLAY_UNITPRICE];
				break;
			case KI_TOTAL :
				win_size = display_parameter.mode_len[DISPLAY_PRICE];
				break;
			case KI_GRAPHIC :
			default :
				win_size = leng;
				break;
		}
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
		if(leng < win_size) win_size = leng;
#else
		l_space = 0;
		if(leng < win_size)
		{
			l_space = win_size - leng;
			if (dsp_mode == KI_GRAPHIC) l_space = 0; // graphic lcd에서는 왼쪽 띄어쓰기 필요없음
			win_size = leng;
		}
#endif
		if(decimal >= win_size) decimal = 0;	// 잘못된 deciaml일 경우 decimal을 0
		// Added by CJK
		flagOverVal = 0;
		if(*ret_val >= digit10[leng] || *ret_val < 0) flagOverVal = 1;
		/////////////////////////////
		if(format & 0x01) temp_val = *ret_val;	// b0 = 1:초기값 출력
		else temp_val = 0;
		
		// Modified by CJK
		if(flagOverVal) key_cnt = 0;
		else {
			for(key_cnt = 0; temp_val >= digit10[key_cnt]; key_cnt++);
		}
		///////////////////////
		//	if(!(format & 0x04)) {					
		//		decimal = 0;						//  (좌측정렬시 소수점 사용 불가)
		//	} else { 								
		//		if(!(format & 0x20)) {	 			// b5 = 0:고정점(decimal) 사용 불가 (우측정렬중 b5가 0일때 소수점 불가)
		//			decimal = 0;					//  (decimal 사용 안함)
		//		}
		//	}
		if (keyin_callback_on) (*keyin_callback)(temp_val);
		while(1) {
			
			if((format & 0x04) == 0x04) { 			// b2 = 1:우측 정렬
				if((format & 0x20) && decimal) {	// b5 = 1:고정점(decimal) 사용 (우측정렬중 b5가 0일때 소수점 불가)
					overlength = 1;
					buf_size = win_size + 1;
				} else {
					decimal = 0;
					overlength = 0;
					buf_size = win_size;
				}
				if (buf_size>19) buf_size=19;
				format_fixed((HUGEDATA INT8U *)buffer, temp_val, decimal, buf_size, KeyInSet.char_decimal);
				buffer[buf_size] = '\0';
				if((format & 0x40) == 0x40) { 		// b6 = 1:입력기호 허용(1000단위)
					if(temp_val >= digit10[3+decimal]) {
						overlength++;						// graphic용
						insert_pt = buf_size - decimal - 3; 
						if(decimal) insert_pt--;			// dot '.' char만큼(1) 감소
						insert_ch(buffer, buf_size, insert_pt, KeyInSet.char_unit1000);
						if(temp_val >= digit10[6+decimal]) {
							overlength++;
							insert_pt = insert_pt - 3;
							insert_ch(buffer, buf_size+1, insert_pt, KeyInSet.char_unit1000);
						}
					}
				}
			} else {								// b2 = 0:좌측 정렬
#ifdef HEBREW_FONT
				decimal = 0;
#endif
				i = format_value((HUGEDATA INT8U *)buffer, temp_val, decimal, KeyInSet.char_decimal);
				for(; i < win_size; i++) buffer[i] = 0x20;
				buffer[i] = 0x00;
			}
			if(use_sec == 1) {
				for(i = 0; buffer[i] != 0x00; i++) {
					if(buffer[i] >= '0' && buffer[i] <= '9') buffer[i] = KeyInSet.char_security;
#if defined(USE_ARAB_CONVERT) || defined(HEBREW_FONT)
				if(i>=win_size) buffer[i] = 0x00;
#endif
				}
			}
			if(flagOverVal == 1) {
				for(i = 0; buffer[i] != 0x00; i++) {
					buffer[i] = '-';
#if defined(USE_ARAB_CONVERT) || defined(HEBREW_FONT)
				if(i>=win_size) buffer[i] = 0x00;
#endif
				}
			}
#if !defined(_USE_LCD20848_) && !defined(_USE_LCD32_)
			if (dsp_mode != KI_GRAPHIC)	overlength = 0; // 7-Segment에서는 overlength 불필요
			temp_buffer[0] = 0;
			if (l_space+overlength) // 7-seg 용 dsp_mode에서 오른쪽 정렬이 필요할 때, 앞쪽에 띄어야하는 칸 수
			{
				for (i = 0; i < l_space+overlength; i++) temp_buffer[i] = 0x20;
				temp_buffer[i] = 0;
			}
			if (overlength)
			{
				for(i = 0; i < buf_size; i++)
				{
					ch = buffer[i+1];
					buffer[i] = ch;
				}		
				buffer[buf_size  - 1] = 0;
			}
			strcat(temp_buffer, buffer);
#endif
			switch(dsp_mode) {
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
				case KI_TARE:
					//VFD7_str_weight(buffer);
					display_string(DISPLAY_TARE, (INT8U*)buffer);
					VFD7_Diffuse();
					break;
				case KI_WEIGHT:
					//VFD7_str_weight(buffer);
					display_string(DISPLAY_WEIGHT, (INT8U*)buffer);
					VFD7_Diffuse();
					break;
				case KI_UNIT :
					//VFD7_str_unit(buffer);
					display_string(DISPLAY_UNITPRICE, (INT8U*)buffer);
					VFD7_Diffuse();
					break;
				case KI_TOTAL :
					display_string(DISPLAY_PRICE, (INT8U*)buffer);
					VFD7_Diffuse();
					break;
#else
				case KI_TARE:
					//VFD7_str_weight(buffer);
					display_string(DISPLAY_TARE, (INT8U*)temp_buffer);
					VFD7_Diffuse();
					break;
				case KI_WEIGHT:
					//VFD7_str_weight(buffer);
					display_string(DISPLAY_WEIGHT, (INT8U*)temp_buffer);
					VFD7_Diffuse();
					break;
				case KI_UNIT :
					//VFD7_str_unit(buffer);
					display_string(DISPLAY_UNITPRICE, (INT8U*)temp_buffer);
					VFD7_Diffuse();
					break;
				case KI_TOTAL :
					display_string(DISPLAY_PRICE, (INT8U*)temp_buffer);
					VFD7_Diffuse();
					break;
#endif
				case KI_GRAPHIC :
				default :
					disp_p = point(y, x);
#ifdef HEBREW_FONT
				Convert_HebrewPreview((INT8U *)buffer+overlength, strlen(buffer+overlength), 0, 0); 
				hebrew_codetemp[strlen(buffer+overlength)] = 0;
				disp_p.x = 202 - disp_p.x;
				Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
				Convert_ArabPreview((INT8U *)buffer+overlength, strlen(buffer+overlength), 0, 0, 0); 
				arab_codetemp[strlen(buffer+overlength)] = 0;
				disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
				Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
					Dsp_Write_String(disp_p, buffer+overlength);
#endif
					//DspStruct.DirectDraw = 0;
					break;
			}
			if(pass == KI_PASS) { ret=0; goto EX_ESC; }
			cursor.line = 0; 
			if(format & 0x04) {		// 우측정렬시
				cursor.col = win_size-1;
			} else {
#if defined(USE_ARAB_CONVERT) || defined(HEBREW_FONT)
			cursor.col = 0;
#else
				if(key_cnt == 0) cursor.col = 0;
				else cursor.col = key_cnt - 1;
#endif
			}
			cursor.y_offset = y;
			cursor.x_offset = x;
#if !defined(_USE_LCD20848_) && !defined(_USE_LCD32_)
			if (dsp_mode != KI_GRAPHIC) cursor.x_offset = l_space; // 7-seg 정렬용
#endif
#ifdef USE_ARAB_CONVERT
            if(use_sec == 1) {
                cursor.chw.words.left = KeyInSet.char_security;
            } else if(flagOverVal) {
                cursor.chw.words.left = '-';
            } else {
                cursor.chw.words.left = temp_val%10 + '0';
            }
            cursor.chw.words.right = 0;
#else
			if(use_sec == 1) {
				cursor.chw.byte.left = KeyInSet.char_security;
			} else if(flagOverVal) {
				cursor.chw.byte.left = '-';
			} else {
				cursor.chw.byte.left = temp_val%10 + '0';
			}
			cursor.chw.byte.right = 0;
#endif
			cursor.timer = (SysTimer100ms+1)%5;
			cursor.mode = dsp_mode;
			cursor.disp_on = 1;
			cursor.status = OFF;
			while(!KEY_Read()) {
				cursor_blink(&cursor);
				if (keyin_anycheck_on)      (*keyin_anycheck)(temp_val);
			}
			switch(KeyDrv.Type) {
				case KEY_TYPE_FUNCTION :
					switch(KeyDrv.CnvCode) {
						case KP_PAGE_UP :
						case KP_PAGE_DN :
						case KP_ARROW_DN :
						case KP_ARROW_UP :
						case KP_ENTER :
						case KP_ARROW_RI:
						case KP_ARROW_LF:
						case KP_TEST :
						case KP_ZERO :
						case KP_TARE :
						case KP_FEED :
						case KP_SAVE  :
							*ret_val = temp_val;
							goto KEYESC;
							//						BuzOn(1);
							//						cursor_clear(&cursor);
							//						ret=KeyDrv.CnvCode;
							//						goto EX_ESC;
						case KP_ESC :
							KEYESC:	BuzOn(1);
							cursor_clear(&cursor);
							ret=KeyDrv.CnvCode;
							goto EX_ESC;
						case KP_CLEAR :
							if (keyin_escape_clear) goto KEYESC;
							if(pass == KI_NOEDIT) {	// Added by CJK 20041223 
								BuzOn(2);
								break;
							}
							BuzOn(1);
							key_cnt = 0;
							temp_val = 0;
							*ret_val = temp_val;	   //  Jan 14, 2004 by getwing
							flagOverVal = 0;	// Added by CJK
							if (keyin_callback_on) (*keyin_callback)(temp_val);  //	  Jan 14, 2004 by getwing
							break;
						case KP_BS:			   //  Jan 14, 2004 by getwing
							BuzOn(1);		   //		"
							temp_val = temp_val/10;	   //		"
							*ret_val = temp_val;	   //		"
							if (keyin_callback_on)     //  Jan 14, 2004 by getwing
								(*keyin_callback)(temp_val);  //Jan 14, 2004 by getwing
							break;			   //		   "
						case KEY_SHIFT :
							BuzOn(1);
							KEY_Set_Shift(3);
							break;
						case KP_CAPS :
							BuzOn(1);
							KEY_Set_Caps();
							break;
						default :
							BuzOn(2);
					}
					break;
				case KEY_TYPE_NUMERIC :
					if(pass == KI_NOEDIT) {	// Added by CJK 20041223 
						BuzOn(2);
						break;
					}
					flagOverVal = 0;	// Added by CJK
					nzero = 1;
					if(KeyDrv.CnvCode==KEY_NUM_10) nzero = keyapp_nzero();
					if(first_input) {
						first_input = 0;
						key_cnt = 0;
						temp_val = 0;
					}
					if(key_cnt == 0 && (KeyDrv.CnvCode == KEY_NUM_0 || KeyDrv.CnvCode == KEY_NUM_10)) key_cnt = 0;	// Modified by CJK 20040722
					else {
						//key_cnt++;
						key_cnt += nzero;	// Added by CJK 20040722
					}
					
					char_cnt = key_cnt;
					if(dsp_mode == KI_GRAPHIC) {
						if((format & 0x44) == 0x44) { // 입력기호 허용(1000단위) & 우측정렬일 경우
							char_cnt += (key_cnt-decimal-1)/3;
						}
						if(decimal != 0) char_cnt++;
					}
					BuzOn(1);
					if(char_cnt > win_size) {
						//key_cnt--;
						//temp_val %= digit10[key_cnt-1];
						key_cnt -= nzero;
						if(key_cnt < nzero) temp_val = 0;
						else temp_val %= digit10[key_cnt-nzero];
					}
					// Modified by CJK 20040722
					if (nzero > 1) {
						temp_val = temp_val * KeyDrv.CnvCode;
					} else {
						temp_val = temp_val * 10 + KeyDrv.CnvCode;
					}
					if (keyin_callback_on) (*keyin_callback)(temp_val);
					break;
				case KEY_TYPE_SPEEDPLU:
					if (keyin_get_speed_key==0) break;
					flagOverVal = 0;
					if (KeyDrv.PrevType == KEY_TYPE_NUMERIC)
					{
						PrevSpeedKeyPluNo = temp_val;
					}
					first_input=1;
					if (Operation.doublePLUkey == 2)
					{
						KeyDrv.CnvCode = keyapp_togleplukey(KeyDrv.CnvCode, ON);
					}
					else	// Operation.doublePLUkey == 0 or 1
					{
						KeyDrv.CnvCode = keyapp_doubleplukey(KeyDrv.CnvCode, ON);
					}
					sub_val = KeyDrv.CnvCode - KS_PLU_01 + 1;
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
					if (keyin_callback_on) (*keyin_callback)(sub_val);
					if (keyin_get_speed_key==1) temp_val=sub_val;
					else {	// keyin_get_speed_key == 2
						temp_val=speedkey_plu_number;
					}
#else
					speedkey_key_number = sub_val;
					speedkey_plu_number = plu_get_speedkey((INT16U)speedkey_set_number, (INT16U)sub_val);
					if (keyin_get_speed_key==1)
					{
						temp_val = speedkey_key_number;
					}
					else if(keyin_get_speed_key == 2) //M1160. speedkey edit
					{
						temp_val = speedkey_plu_number;
					}
					if (keyin_callback_on) (*keyin_callback)(temp_val);
#endif
					BuzOn(1);		 
					break;
				case KEY_TYPE_CLERK:
					temp_val = KeyDrv.CnvCode - KS_CLERK_01 + 1;
					BuzOn(1);
					if (keyin_callback_on) (*keyin_callback)(temp_val);
					if (keyin_clerk_onetouch)
					{
						//					*ret_val = temp_val;
						*ret_val = call_clerk(temp_val);	//Modified by JJANG 20080111
						cursor_clear(&cursor);
						ret = KP_ENTER;
						goto EX_ESC;
					} 
					break;
				case KEY_TYPE_TARE:
					temp_val = KeyDrv.CnvCode - KS_TARE_01 + 1;
					BuzOn(1);
					if (keyin_callback_on) (*keyin_callback)(temp_val);
					break;
				case KEY_TYPE_LABEL:
					temp_val = KeyDrv.CnvCode - KS_LABEL_01 + 1;
					BuzOn(1);
					if (keyin_callback_on) (*keyin_callback)(temp_val);
					break;
#if defined(USE_TRACE_STANDALONE)
				case KEY_TYPE_INDIVIDUAL:	// Added by CJK 20060725
					temp_val = KeyDrv.CnvCode - KS_INDIVIDUAL_01 + 1;
					BuzOn(1);
					if (keyin_callback_on) (*keyin_callback)(temp_val);
					break;
				case KEY_TYPE_SPECIALUSE:
					temp_val = KeyDrv.CnvCode - KS_SPECIALUSE_01 + 1;
					BuzOn(1);
					if (keyin_callback_on) (*keyin_callback)(temp_val);
					break;
#endif
				default :
					BuzOn(2);
			}
			if ((!KeyDrv.key_caps) && (KeyDrv.Shifted) && (KeyDrv.CnvCode != KEY_SHIFT)) KEY_Set_Shift(3);
		}
		EX_ESC:
					if (pass != KI_PASS) KEY_Set_Shift(0);
		DspStruct.DirectDraw = backup_directdraw;
		///////////////////////
		switch (data_type)
		{
			case 1:
				ret_val1 = (INT8U)*ret_val;
				memcpy((void*)ret_addr, &ret_val1, 1);
				break;
			case 2:
				ret_val2 = (INT16U)*ret_val;
				memcpy((void*)ret_addr, &ret_val2, 2);
				break;
			case 4:
			default:
				//ret_val4 = (INT32U)*ret_val;
				memcpy((void*)ret_addr, &ret_val4, 4);
				break;
		}
		////////////////////////
		return ret;
	}
	
	INT16U keyin_ulong(INT8U dsp_mode, INT8U format, INT8U decimal, long *ret_val, INT8U leng, INT8U y, INT16U x, INT8U use_sec, INT8U pass)
	{
		long temp_val,sub_val;
		char buffer[20];
		char temp_buffer[20];
		INT16U l_space;
		INT8U i, win_size, overlength, buf_size;//, overlength;
		POINT disp_p;
		INT16U ret;
		MENU_CURSOR_STR cursor;
		INT8U key_cnt;
		INT8U char_cnt;
		INT8U insert_pt;
		INT8U first_input;
		INT32U digit10[10] = {1l, 10l, 100l, 1000l, 10000l, 100000l, 1000000l, 10000000l, 100000000l, 1000000000l};
		INT8U flagOverVal;
		INT16S nzero;
		INT8U backup_directdraw;
		INT8U ch;
		
		backup_directdraw = DspStruct.DirectDraw;
		if (pass == KI_NOPASS) DspStruct.DirectDraw = 1;
		cursor.type = CURSOR_TYPE_DELETE;
		overlength  = 0;
		first_input = 1;
		ret         = 0;
		if(leng > 9)  leng = 9;	// 최대 입력 가능한 Digit 수:9
		if(leng == 0) leng = 1;
		switch(dsp_mode) {
			case KI_TARE:
				win_size = display_parameter.mode_len[DISPLAY_TARE];
				break;
			case KI_WEIGHT:
				win_size = display_parameter.mode_len[DISPLAY_WEIGHT];
				break;
			case KI_UNIT :
				win_size = display_parameter.mode_len[DISPLAY_UNITPRICE];
				break;
			case KI_TOTAL :
				win_size = display_parameter.mode_len[DISPLAY_PRICE];
				break;
			case KI_GRAPHIC :
			default :
				win_size = leng;
				break;
		}
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
		if(leng < win_size) win_size = leng;
#else
		l_space = 0;
		if(leng < win_size)
		{
			l_space = win_size - leng;
			if (dsp_mode == KI_GRAPHIC) l_space = 0; // graphic lcd에서는 왼쪽 띄어쓰기 필요없음
			win_size = leng;
		}
#endif
		if(decimal >= win_size) decimal = 0;	// 잘못된 deciaml일 경우 decimal을 0
		// Added by CJK
		flagOverVal = 0;
		if(*ret_val >= digit10[leng] || *ret_val < 0) flagOverVal = 1;
		/////////////////////////////
		if(format & 0x01) temp_val = *ret_val;	// b0 = 1:초기값 출력
		else temp_val = 0;
		
		// Modified by CJK
		if(flagOverVal) key_cnt = 0;
		else {
			for(key_cnt = 0; temp_val >= digit10[key_cnt]; key_cnt++);
		}
		///////////////////////
		//	if(!(format & 0x04)) {					
		//		decimal = 0;						//  (좌측정렬시 소수점 사용 불가)
		//	} else { 								
		//		if(!(format & 0x20)) {	 			// b5 = 0:고정점(decimal) 사용 불가 (우측정렬중 b5가 0일때 소수점 불가)
		//			decimal = 0;					//  (decimal 사용 안함)
		//		}
		//	}
		if (keyin_callback_on) (*keyin_callback)(temp_val);
		while(1) {
			
			if((format & 0x04) == 0x04) { 			// b2 = 1:우측 정렬
				if((format & 0x20) && decimal) {	// b5 = 1:고정점(decimal) 사용 (우측정렬중 b5가 0일때 소수점 불가)
					overlength = 1;
					buf_size = win_size + 1;
				} else {
					decimal = 0;
					overlength = 0;
					buf_size = win_size;
				}
				if (buf_size>19) buf_size=19;
				format_fixed((HUGEDATA INT8U *)buffer, temp_val, decimal, buf_size, KeyInSet.char_decimal);
				buffer[buf_size] = '\0';
				if((format & 0x40) == 0x40) { 		// b6 = 1:입력기호 허용(1000단위)
					if(temp_val >= digit10[3+decimal]) {
						overlength++;						// graphic용
						insert_pt = buf_size - decimal - 3; 
						if(decimal) insert_pt--;			// dot '.' char만큼(1) 감소
						insert_ch(buffer, buf_size, insert_pt, KeyInSet.char_unit1000);
						if(temp_val >= digit10[6+decimal]) {
							overlength++;
							insert_pt = insert_pt - 3;
							insert_ch(buffer, buf_size+1, insert_pt, KeyInSet.char_unit1000);
						}
					}
				}
			} else {								// b2 = 0:좌측 정렬
				decimal = 0;
				i = format_value((HUGEDATA INT8U *)buffer, temp_val, decimal, KeyInSet.char_decimal);
				for(; i < win_size; i++) buffer[i] = 0x20;
				buffer[i] = 0x00;
			}
			if(use_sec == 1) {
				for(i = 0; buffer[i] != 0x00; i++) {
					if(buffer[i] >= '0' && buffer[i] <= '9') buffer[i] = KeyInSet.char_security;
				}
			}
			if(flagOverVal == 1) {
				for(i = 0; buffer[i] != 0x00; i++) {
					buffer[i] = '-';
				}
			}
#if !defined(_USE_LCD20848_) && !defined(_USE_LCD32_)
			if (dsp_mode != KI_GRAPHIC)	overlength = 0; // 7-Segment에서는 overlength 불필요
			temp_buffer[0] = 0;
			if (l_space+overlength) // 7-seg 용 dsp_mode에서 오른쪽 정렬이 필요할 때, 앞쪽에 띄어야하는 칸 수
			{
				for (i = 0; i < l_space+overlength; i++) temp_buffer[i] = 0x20;
				temp_buffer[i] = 0;
			}
			if (overlength)
			{
				for(i = 0; i < buf_size; i++)
				{
					ch = buffer[i+1];
					buffer[i] = ch;
				}		
				buffer[buf_size  - 1] = 0;
			}
			strcat(temp_buffer, buffer);
#endif
			switch(dsp_mode) {
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
				case KI_TARE:
					//VFD7_str_weight(buffer);
					display_string(DISPLAY_TARE, (INT8U*)buffer);
					VFD7_Diffuse();
					break;
				case KI_WEIGHT:
					//VFD7_str_weight(buffer);
					display_string(DISPLAY_WEIGHT, (INT8U*)buffer);
					VFD7_Diffuse();
					break;
				case KI_UNIT :
					//VFD7_str_unit(buffer);
					display_string(DISPLAY_UNITPRICE, (INT8U*)buffer);
					VFD7_Diffuse();
					break;
				case KI_TOTAL :
					display_string(DISPLAY_PRICE, (INT8U*)buffer);
					VFD7_Diffuse();
					break;
#else
				case KI_TARE:
					//VFD7_str_weight(buffer);
					display_string(DISPLAY_TARE, (INT8U*)temp_buffer);
					VFD7_Diffuse();
					break;
				case KI_WEIGHT:
					//VFD7_str_weight(buffer);
					display_string(DISPLAY_WEIGHT, (INT8U*)temp_buffer);
					VFD7_Diffuse();
					break;
				case KI_UNIT :
					//VFD7_str_unit(buffer);
					display_string(DISPLAY_UNITPRICE, (INT8U*)temp_buffer);
					VFD7_Diffuse();
					break;
				case KI_TOTAL :
					display_string(DISPLAY_PRICE, (INT8U*)temp_buffer);
					VFD7_Diffuse();
					break;
#endif
				case KI_GRAPHIC :
				default :
					disp_p = point(y, x);
#ifdef HEBREW_FONT
					Convert_HebrewPreview(buffer+overlength, strlen(buffer+overlength), 0, 0); 
					hebrew_codetemp[strlen(buffer+overlength)] = 0;
					disp_p.x = 202 - disp_p.x;
					Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
					Convert_ArabPreview((INT8U *)buffer+overlength, strlen(buffer+overlength), 0, 0, 0); 
					arab_codetemp[strlen(buffer+overlength)] = 0;
					disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
					Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
					Dsp_Write_String(disp_p, buffer+overlength);
#endif
					//DspStruct.DirectDraw = 0;
					break;
			}
			if(pass == KI_PASS) { ret=0; goto EX_ESC; }
			cursor.line = 0; 
			if(format & 0x04) {		// 우측정렬시
				cursor.col = win_size-1;
			} else {
#if defined(USE_ARAB_CONVERT) || defined(HEBREW_FONT)
				cursor.col = 0;		//좌측정렬 시 커서 항상 마지막에 위치
#else
				if(key_cnt == 0) cursor.col = 0;
				else cursor.col = key_cnt - 1;
#endif
			}
			cursor.y_offset = y;
			cursor.x_offset = x;
#if !defined(_USE_LCD20848_) && !defined(_USE_LCD32_)
			if (dsp_mode != KI_GRAPHIC) cursor.x_offset = l_space; // 7-seg 정렬용
#endif
#ifdef USE_ARAB_CONVERT
            if(use_sec == 1) {
                cursor.chw.words.left = KeyInSet.char_security;
            } else if(flagOverVal) {
                cursor.chw.words.left = '-';
            } else {
                cursor.chw.words.left = temp_val%10 + '0';
            }
            cursor.chw.words.right = 0;
#else
			if(use_sec == 1) {
				cursor.chw.byte.left = KeyInSet.char_security;
			} else if(flagOverVal) {
				cursor.chw.byte.left = '-';
			} else {
				cursor.chw.byte.left = temp_val%10 + '0';
			}
			cursor.chw.byte.right = 0;
#endif
			cursor.timer = (SysTimer100ms+1)%5;
			cursor.mode = dsp_mode;
			cursor.disp_on = 1;
			cursor.status = OFF;
			while(!KEY_Read()) {
				cursor_blink(&cursor);
				if (keyin_anycheck_on)      (*keyin_anycheck)(temp_val);
			}
			switch(KeyDrv.Type) {
				case KEY_TYPE_FUNCTION :
					switch(KeyDrv.CnvCode) {
						case KP_PAGE_UP :
						case KP_PAGE_DN :
						case KP_ARROW_DN :
						case KP_ARROW_UP :
						case KP_ENTER :
						case KP_ARROW_RI:
						case KP_ARROW_LF:
						case KP_TEST :
						case KP_ZERO :
						case KP_TARE :
						case KP_FEED :
						case KP_SAVE  :
						case KP_CLOSE_TICKET :
							*ret_val = temp_val;
							goto KEYESC;
							//						BuzOn(1);
							//						cursor_clear(&cursor);
							//						ret=KeyDrv.CnvCode;
							//						goto EX_ESC;
						case KP_ESC :
							KEYESC:	BuzOn(1);
							cursor_clear(&cursor);
							ret=KeyDrv.CnvCode;
							goto EX_ESC;
						case KP_CLEAR :
							if (keyin_escape_clear) goto KEYESC;
							if(pass == KI_NOEDIT) {	// Added by CJK 20041223 
								BuzOn(2);
								break;
							}
							BuzOn(1);
							key_cnt = 0;
							temp_val = 0;
							*ret_val = temp_val;	   //  Jan 14, 2004 by getwing
							flagOverVal = 0;	// Added by CJK
							if (keyin_callback_on) (*keyin_callback)(temp_val);  //	  Jan 14, 2004 by getwing
							break;
						case KP_BS:			   //  Jan 14, 2004 by getwing
							BuzOn(1);		   //		"
							temp_val = temp_val/10;	   //		"
							*ret_val = temp_val;	   //		"
							if (keyin_callback_on)     //  Jan 14, 2004 by getwing
								(*keyin_callback)(temp_val);  //Jan 14, 2004 by getwing
							break;			   //		   "
						case KEY_SHIFT :
							BuzOn(1);
							KEY_Set_Shift(3);
							break;
						case KP_CAPS :
							BuzOn(1);
							KEY_Set_Caps();
							break;
						default :
							BuzOn(2);
					}
					break;
				case KEY_TYPE_NUMERIC :
					if(pass == KI_NOEDIT) {	// Added by CJK 20041223 
						BuzOn(2);
						break;
					}
					flagOverVal = 0;	// Added by CJK
					nzero = 1;
					if(KeyDrv.CnvCode==KEY_NUM_10) nzero = keyapp_nzero();
					if(first_input) {
						first_input = 0;
						key_cnt = 0;
						temp_val = 0;
					}
					if(key_cnt == 0 && (KeyDrv.CnvCode == KEY_NUM_0 || KeyDrv.CnvCode == KEY_NUM_10)) key_cnt = 0;	// Modified by CJK 20040722
					else {
						//key_cnt++;
						key_cnt += nzero;	// Added by CJK 20040722
					}
					
					char_cnt = key_cnt;
					if(dsp_mode == KI_GRAPHIC) {
						if((format & 0x44) == 0x44) { // 입력기호 허용(1000단위) & 우측정렬일 경우
							char_cnt += (key_cnt-decimal-1)/3;
						}
						if(decimal != 0) char_cnt++;
					}
					BuzOn(1);
					if(char_cnt > win_size) {
						//key_cnt--;
						//temp_val %= digit10[key_cnt-1];
						key_cnt -= nzero;
						if(key_cnt < nzero) temp_val = 0;
						else temp_val %= digit10[key_cnt-nzero];
					}
					// Modified by CJK 20040722
					if (nzero > 1) {
						temp_val = temp_val * KeyDrv.CnvCode;
					} else {
						temp_val = temp_val * 10 + KeyDrv.CnvCode;
					}
					if (keyin_callback_on) (*keyin_callback)(temp_val);
					break;
				case KEY_TYPE_SPEEDPLU:
					if (keyin_get_speed_key==0) break;
					flagOverVal = 0;
					if (KeyDrv.PrevType == KEY_TYPE_NUMERIC)
					{
						PrevSpeedKeyPluNo = temp_val;
					}
					first_input=1;
					if (Operation.doublePLUkey == 2)
					{
						KeyDrv.CnvCode = keyapp_togleplukey(KeyDrv.CnvCode, ON);
					}
					else	// Operation.doublePLUkey == 0 or 1
					{
						KeyDrv.CnvCode = keyapp_doubleplukey(KeyDrv.CnvCode, ON);
					}
					sub_val = KeyDrv.CnvCode - KS_PLU_01 + 1;
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
					if (keyin_callback_on) (*keyin_callback)(sub_val);
					if (keyin_get_speed_key==1) temp_val=sub_val;
					else {	// keyin_get_speed_key == 2
						temp_val=speedkey_plu_number;
					}
#else
					speedkey_key_number = sub_val;
					speedkey_plu_number = plu_get_speedkey((INT16U)speedkey_set_number, (INT16U)sub_val);
					if (keyin_get_speed_key==1)
					{
						temp_val = speedkey_key_number;
					}
					else if(keyin_get_speed_key == 2) //M1160. speedkey edit
					{
						temp_val = speedkey_plu_number;
					}
					if (keyin_callback_on) (*keyin_callback)(temp_val);
#endif
					BuzOn(1);		 
					break;
				case KEY_TYPE_CLERK:
					temp_val = KeyDrv.CnvCode - KS_CLERK_01 + 1;
					BuzOn(1);
					if (keyin_callback_on) (*keyin_callback)(temp_val);
					if (keyin_clerk_onetouch)
					{
						//					*ret_val = temp_val;
						*ret_val = call_clerk(temp_val);	//Modified by JJANG 20080111
						cursor_clear(&cursor);
						ret = KP_ENTER;
						goto EX_ESC;
					} 
					break;
				case KEY_TYPE_TARE:
					temp_val = KeyDrv.CnvCode - KS_TARE_01 + 1;
					BuzOn(1);
					if (keyin_callback_on) (*keyin_callback)(temp_val);
					break;
				case KEY_TYPE_LABEL:
					temp_val = KeyDrv.CnvCode - KS_LABEL_01 + 1;
					BuzOn(1);
					if (keyin_callback_on) (*keyin_callback)(temp_val);
					break;
#if defined(USE_TRACE_STANDALONE)
				case KEY_TYPE_INDIVIDUAL:	// Added by CJK 20060725
					temp_val = KeyDrv.CnvCode - KS_INDIVIDUAL_01 + 1;
					BuzOn(1);
					if (keyin_callback_on) (*keyin_callback)(temp_val);
					break;
				case KEY_TYPE_SPECIALUSE:
					temp_val = KeyDrv.CnvCode - KS_SPECIALUSE_01 + 1;
					BuzOn(1);
					if (keyin_callback_on) (*keyin_callback)(temp_val);
					break;
#endif
				default :
					BuzOn(2);
			}
			if ((!KeyDrv.key_caps) && (KeyDrv.Shifted) && (KeyDrv.CnvCode != KEY_SHIFT)) KEY_Set_Shift(3);
#ifdef USE_SETPRODUCT_SALE
			if(KeyDrv.CnvCode == KS_FEED)	keyapp_feed(ON);
#endif
	}
		EX_ESC:
					if (pass != KI_PASS) KEY_Set_Shift(0);
		DspStruct.DirectDraw = backup_directdraw;
		return ret;
	}
	
	//INT16U keyin_ulong_hex(INT8U dsp_mode, INT8U format, INT8U decimal, long *ret_val, INT8U leng, INT8U y, INT16U x, INT8U use_sec, INT8U pass)
	INT16U keyin_ulong_hex(INT8U dsp_mode, INT8U format, INT8U decimal, INT32U *ret_val, INT8U leng, INT8U y, INT16U x, INT8U use_sec, INT8U pass)
	{
		//long temp_val;
		INT32U temp_val;
		char buffer[20];
		INT8U i, win_size, overlength, buf_size;
		POINT disp_p;
		INT16U ret;
		
		MENU_CURSOR_STR cursor;
		INT8U key_cnt;
		INT8U char_cnt;
		INT8U insert_pt;
		INT8U first_input;
		INT32U digit16[8] = {1, 0x10, 0x100, 0x1000, 0x10000, 0x100000, 0x1000000, 0x10000000};
		INT8U flagOverVal;
		INT8U backup_directdraw;
		
		backup_directdraw = DspStruct.DirectDraw;
		if (pass == KI_NOPASS) DspStruct.DirectDraw = 1;
		cursor.type = CURSOR_TYPE_DELETE;
		overlength = 0;
		ret        = 0;
		first_input = 1;
		switch(dsp_mode) {
			case KI_TARE:
				win_size = display_parameter.mode_len[DISPLAY_TARE];
				break;
			case KI_WEIGHT:
				win_size = display_parameter.mode_len[DISPLAY_WEIGHT];
				break;
			case KI_UNIT :
				win_size = display_parameter.mode_len[DISPLAY_UNITPRICE];
				break;
			case KI_TOTAL :
				win_size = display_parameter.mode_len[DISPLAY_PRICE];
				break;
			case KI_GRAPHIC :
			default :
				win_size = leng;
				break;
		}
		if(leng < win_size) win_size = leng;
		if(decimal >= win_size) decimal = 0;	// 잘못된 deciaml일 경우 decimal을 0
		// Added by CJK
		flagOverVal = 0;
		if(*ret_val >= digit16[leng] || *ret_val < 0) flagOverVal = 1;
		/////////////////////////////
		
		if(format & 0x01) temp_val = *ret_val;	// b0 = 1:초기값 출력
		else temp_val = 0;
		
		// Modified by CJK
		if(flagOverVal) key_cnt = 0;
		else {
			for(key_cnt = 0; temp_val >= digit16[key_cnt]; key_cnt++);
		}
		
		if (keyin_callback_on) (*keyin_callback)(temp_val);
		
		while(1) {
			
			if((format & 0x04) == 0x04) { 			// b2 = 1:우측 정렬
				if((format & 0x20) && decimal) {	// b5 = 1:고정점(decimal) 사용 (우측정렬중 b5가 0일때 소수점 불가)
					overlength = 1;
					buf_size = win_size + 1;
				} else {
					decimal = 0;
					overlength = 0;
					buf_size = win_size;
				}
				//format_fixed((HUGEDATA INT8U *)buffer, temp_val, decimal, buf_size, KeyInSet.char_decimal);
				format_fixed_hex((HUGEDATA INT8U *)buffer, temp_val, buf_size);
				buffer[buf_size] = '\0';
				if((format & 0x40) == 0x40) { 		// b6 = 1:입력기호 허용(1000단위)
					if(temp_val >= digit16[3+decimal]) {
						overlength++;						// graphic용
						insert_pt = buf_size - decimal - 3; 
						if(decimal) insert_pt--;			// dot '.' char만큼(1) 감소
						insert_ch(buffer, buf_size, insert_pt, KeyInSet.char_unit1000);
						if(temp_val >= digit16[6+decimal]) {
							overlength++;
							insert_pt = insert_pt - 3;
							insert_ch(buffer, buf_size+1, insert_pt, KeyInSet.char_unit1000);
						}
					}
				}
			} else {								// b2 = 0:좌측 정렬
				decimal = 0;
				i = format_value((HUGEDATA INT8U *)buffer, temp_val, decimal, KeyInSet.char_decimal);
				for(; i < win_size; i++) buffer[i] = 0x20;
				buffer[i] = 0x00;
			}
			if(use_sec == 1) {
				for(i = 0; buffer[i] != 0x00; i++) {
					if(buffer[i] >= '0' && buffer[i] <= '9') buffer[i] = KeyInSet.char_security;
				}
			}
			if(flagOverVal == 1) {
				for(i = 0; buffer[i] != 0x00; i++) {
					buffer[i] = '-';
				}
			}
			
			switch(dsp_mode) {
				case KI_TARE:
					//VFD7_str_weight(buffer);
					display_string(DISPLAY_TARE, (INT8U*)buffer);
					VFD7_Diffuse();
					break;
				case KI_WEIGHT:
					//VFD7_str_weight(buffer);
					display_string(DISPLAY_WEIGHT, (INT8U*)buffer);
					VFD7_Diffuse();
					break;
				case KI_UNIT :
					//VFD7_str_unit(buffer);
					display_string(DISPLAY_UNITPRICE, (INT8U*)buffer);
					VFD7_Diffuse();
					break;
				case KI_TOTAL :
					//VFD7_str_price(buffer);
					display_string(DISPLAY_PRICE, (INT8U*)buffer);
					VFD7_Diffuse();
					break;
				case KI_GRAPHIC :
				default :
					disp_p = point(y, x);
#ifdef HEBREW_FONT
					Convert_HebrewPreview((INT8U *)buffer+overlength, strlen(buffer+overlength), 0, 0); 
					hebrew_codetemp[strlen(buffer+overlength)] = 0;
					disp_p.x = 202 - disp_p.x;
					Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
					Convert_ArabPreview((INT8U *)buffer+overlength, strlen(buffer+overlength), 0, 0, 0); 
					arab_codetemp[strlen(buffer+overlength)] = 0;
					disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
					Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
					Dsp_Write_String(disp_p, buffer+overlength);
#endif
					//DspStruct.DirectDraw = 0;
					break;
			}
			if(pass == KI_PASS) {ret=0; goto EX_ESC; }
			cursor.line = 0; 
			if(format & 0x04) {		// 우측정렬시
				cursor.col = win_size-1;
			} else {
				if(key_cnt == 0) cursor.col = 0;
				else cursor.col = key_cnt - 1;
			}
			cursor.y_offset = y;
			cursor.x_offset = x;
#ifdef USE_ARAB_CONVERT
            if(use_sec == 1) {
                cursor.chw.words.left = KeyInSet.char_security;
            } else if(flagOverVal) {
                cursor.chw.words.left = '-';
            } else {
                cursor.chw.words.left = temp_val%16 + '0';
                if(cursor.chw.words.left > '9') cursor.chw.words.left += ('A'-'0'-10);
            }
            cursor.chw.words.right = 0;
#else
			if(use_sec == 1) {
				cursor.chw.byte.left = KeyInSet.char_security;
			} else if(flagOverVal) {
				cursor.chw.byte.left = '-';
			} else {
				cursor.chw.byte.left = temp_val%16 + '0';
				if(cursor.chw.byte.left > '9') cursor.chw.byte.left += ('A'-'0'-10);
			}
			cursor.chw.byte.right = 0;
#endif
			cursor.timer = (SysTimer100ms+1)%5;
			cursor.mode = dsp_mode;
			cursor.disp_on = 1;
			cursor.status = OFF;
			while(!KEY_Read()) {
				cursor_blink(&cursor);
			}
			//DEL070619		if ((!KeyDrv.key_caps) && (KeyDrv.Shifted) && (KeyDrv.CnvCode != KEY_SHIFT)) KEY_Set_Shift(3);
			switch(KeyDrv.Type) {
				case KEY_TYPE_FUNCTION :
					switch(KeyDrv.CnvCode) {
						case KP_PAGE_UP :
						case KP_PAGE_DN :
						case KP_ARROW_DN :
						case KP_ARROW_UP :
						case KP_ENTER :
						case KP_TEST :
						case KP_ZERO :
						case KP_TARE :
						case KP_FEED :
						case KP_SAVE  :
							BuzOn(1);
							cursor_clear(&cursor);
							*ret_val = temp_val;
							ret=KeyDrv.CnvCode;
							goto EX_ESC;
						case KP_ESC :
							BuzOn(1);
							cursor_clear(&cursor);
							ret=KeyDrv.CnvCode;
							goto EX_ESC;
						case KP_CLEAR :
							if(pass == KI_NOEDIT) {	// Added by CJK 20041223 
								BuzOn(2);
								break;
							}
							BuzOn(1);
							key_cnt = 0;
							temp_val = 0;
							flagOverVal = 0;	// Added by CJK
							break;
						case KEY_SHIFT :
							BuzOn(1);
							KEY_Set_Shift(3);	// Added by CJK 20051027
							break;
						case KP_CAPS :
							BuzOn(1);
							KEY_Set_Caps();	// Added by CJK 20051027
							break;
						default :
							BuzOn(2);
					}
					break;
				case KEY_TYPE_NUMERIC :
					if(pass == KI_NOEDIT) {	// Added by CJK 20041223 
						BuzOn(2);
						break;
					}
					flagOverVal = 0;	// Added by CJK
					if(KeyDrv.CnvCode == KEY_NUM_10) KeyDrv.CnvCode = 0;
					if(first_input) {
						first_input = 0;
						key_cnt = 0;
						temp_val = 0;
					}
					if(key_cnt == 0 && KeyDrv.CnvCode == KEY_NUM_0) key_cnt = 0;
					else key_cnt++;
					
					char_cnt = key_cnt;
					if(dsp_mode == KI_GRAPHIC) {
						if((format & 0x44) == 0x44) { // 입력기호 허용(1000단위) & 우측정렬일 경우
							char_cnt += (key_cnt-decimal-1)/3;
						}
						if(decimal != 0) char_cnt++;
					}
					BuzOn(1);
					temp_val = temp_val * 16;
					temp_val = temp_val + KeyDrv.CnvCode - KEY_NUM_0;
					if(char_cnt > win_size) {
						key_cnt--;
						if(win_size < 8) {
							temp_val %= digit16[key_cnt];
						}
					}
					if (keyin_callback_on) (*keyin_callback)(temp_val);
					break;
				case KEY_TYPE_ASCII :
					if(pass == KI_NOEDIT) {	// Added by CJK 20041223 
						BuzOn(2);
						break;
					}
					//if(KeyDrv.CnvCode >= KP_ASC_A && KeyDrv.CnvCode <= KP_ASC_F)
					if(KeyDrv.CnvCode > KP_ASC_F) {
						BuzOn(2);
						break;
					}
					flagOverVal = 0;	// Added by CJK
					if(first_input) {
						first_input = 0;
						key_cnt = 0;
						temp_val = 0;
					}
					if(key_cnt == 0 && KeyDrv.CnvCode == KEY_NUM_0) key_cnt = 0;
					else key_cnt++;
					
					char_cnt = key_cnt;
					if(dsp_mode == KI_GRAPHIC) {
						if((format & 0x44) == 0x44) { // 입력기호 허용(1000단위) & 우측정렬일 경우
							char_cnt += (key_cnt-decimal-1)/3;
						}
						if(decimal != 0) char_cnt++;
					}
					BuzOn(1);
					temp_val = temp_val * 16;
					temp_val = temp_val + KeyDrv.CnvCode - KP_ASC_A + 10;
					if(char_cnt > win_size) {
						key_cnt--;
						if(win_size < 8) {
							temp_val %= digit16[key_cnt];
						}
					}
					if (keyin_callback_on) (*keyin_callback)(temp_val);
					break;
				default :
					BuzOn(2);
			}
			if ((!KeyDrv.key_caps) && (KeyDrv.Shifted) && (KeyDrv.CnvCode != KEY_SHIFT)) KEY_Set_Shift(3);
		}
		EX_ESC:
					if (pass != KI_PASS) KEY_Set_Shift(0);
		DspStruct.DirectDraw = backup_directdraw;
		return ret;
	}
	
	INT16U keyin_yesno(INT8U dsp_mode, long *ret_val, INT8U y, INT16U x, INT8U pass)
	{
		long temp_val;
		char buffer[2];
		char temp_buffer[20];
		INT16U l_space;
		INT8U win_size;
		INT8U pgmmode,keymode;    
		INT16U ret;
		POINT disp_p;
		MENU_CURSOR_STR cursor;
		INT8U backup_directdraw;
		INT8U i;
		INT8U ch;
		INT16U temp_y, temp_n;
		
		backup_directdraw = DspStruct.DirectDraw;
		if (pass == KI_NOPASS) DspStruct.DirectDraw = 1;
		cursor.type = CURSOR_TYPE_DELETE;
		//win_size = 1;
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
		if(*ret_val) {
			temp_val = 1;
		}
		else {
			temp_val = 0;
		}
#else
		switch(dsp_mode) {
			case KI_TARE:
				win_size = display_parameter.mode_len[DISPLAY_TARE];
				break;
			case KI_WEIGHT:
				win_size = display_parameter.mode_len[DISPLAY_WEIGHT];
				break;
			case KI_UNIT :
				win_size = display_parameter.mode_len[DISPLAY_UNITPRICE];
				break;
			case KI_TOTAL :
				win_size = display_parameter.mode_len[DISPLAY_PRICE];
				break;
			case KI_GRAPHIC :
			default :
				win_size = 1;
				break;
		}
		
		l_space = win_size - 1;
		if (dsp_mode == KI_GRAPHIC) l_space = 0; // graphic lcd에서는 왼쪽 띄어쓰기 필요없음
		
		if(*ret_val) {
			temp_val = 1;
		}
		else {
			temp_val = 0;
		}
#endif
		keymode=KEY_GetMode();
		KEY_SetMode(1);
		pgmmode=KeyDrv.PgmMode;
		if (Startup.country==COUNTRY_KR) KeyDrv.PgmMode=0;
#if !defined(_USE_LCD20848_) && !defined(_USE_LCD32_)
		if (keyin_callback_on) (*keyin_callback)(temp_val);
#endif
		while(1) {
			if(temp_val) buffer[0] = KeyInSet.char_yes;
			else buffer[0] = KeyInSet.char_no;
			
			buffer[1] = 0x00;
#if !defined(_USE_LCD20848_) && !defined(_USE_LCD32_)
			temp_buffer[0] = 0;
			if (l_space) // 7-seg 용 dsp_mode에서 오른쪽 정렬이 필요할 때, 앞쪽에 띄어야하는 칸 수
			{
				for (i = 0; i < l_space; i++) temp_buffer[i] = 0x20;
				temp_buffer[i] = 0;
			}
			strcat(temp_buffer, buffer);
#endif
			switch(dsp_mode) {
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
				case KI_TARE:
					//VFD7_str_weight(buffer);
					display_string(DISPLAY_TARE, (INT8U*)buffer);
					VFD7_Diffuse();
					break;
				case KI_WEIGHT:
					//VFD7_str_weight(buffer);
					display_string(DISPLAY_WEIGHT, (INT8U*)buffer);
					VFD7_Diffuse();
					break;
				case KI_UNIT :
					//VFD7_str_unit(buffer);
					display_string(DISPLAY_UNITPRICE, (INT8U*)buffer);
					VFD7_Diffuse();
					break;
				case KI_TOTAL :
					//VFD7_str_price(buffer);
					display_string(DISPLAY_PRICE, (INT8U*)buffer);
					VFD7_Diffuse();
					break;
#else
				case KI_TARE:
					//VFD7_str_weight(buffer);
					display_string(DISPLAY_TARE, (INT8U*)temp_buffer);
					VFD7_Diffuse();
					break;
				case KI_WEIGHT:
					//VFD7_str_weight(buffer);
					display_string(DISPLAY_WEIGHT, (INT8U*)temp_buffer);
					VFD7_Diffuse();
					break;
				case KI_UNIT :
					//VFD7_str_unit(buffer);
					display_string(DISPLAY_UNITPRICE, (INT8U*)temp_buffer);
					VFD7_Diffuse();
					break;
				case KI_TOTAL :
					//VFD7_str_price(buffer);
					display_string(DISPLAY_PRICE, (INT8U*)temp_buffer);
					VFD7_Diffuse();
					break;
#endif
				case KI_GRAPHIC :
				default :
					disp_p = point(y, x);
#ifdef HEBREW_FONT
					Convert_HebrewPreview((INT8U *)buffer, strlen(buffer), 0, 0); 
					hebrew_codetemp[strlen(buffer)] = 0;
					disp_p.x = 202 - disp_p.x;
					Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
				Convert_ArabPreview((INT8U *)buffer, strlen(buffer), 0, 0, 0); 
				arab_codetemp[strlen(buffer)] = 0;
				disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
				Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
					Dsp_Write_String(disp_p, buffer);
#endif
					//DspStruct.DirectDraw = 0;
					break;
			}
			if(pass == KI_PASS) {ret= 0; goto EX_ESC; }
			cursor.line = 0; 
			cursor.col  = 0;
			cursor.y_offset = y;
			cursor.x_offset = x;
			if (dsp_mode != KI_GRAPHIC) cursor.x_offset = l_space; // 7-seg 정렬용
#ifdef USE_ARAB_CONVERT
            cursor.chw.words.left = (buffer[0] & 0x00FF);
            cursor.chw.words.right = 0;
#else
			cursor.chw.byte.left = buffer[0];
			cursor.chw.byte.right = 0;
#endif
			cursor.timer = (SysTimer100ms+1)%5;
			cursor.mode = dsp_mode;
			cursor.disp_on = 1;
			cursor.status = OFF;
			while(!KEY_Read()) {
				cursor_blink(&cursor);
			}
			//			sprintf(str,"[%04x]",KeyDrv.CnvCode);
			//			MsgOut(str);
			
			//DEL070619		if ((!KeyDrv.key_caps) && (KeyDrv.Shifted) && (KeyDrv.CnvCode != KEY_SHIFT)) KEY_Set_Shift(3);
			switch(KeyDrv.Type) {
				case KEY_TYPE_FUNCTION :
					switch(KeyDrv.CnvCode) {
						case KP_ARROW_DN :
						case KP_ARROW_UP :
						case KP_ENTER :
						case KP_PAGE_DN:
						case KP_PAGE_UP:
						case KP_TEST :
						case KP_ZERO :
						case KP_TARE :
						case KP_FEED :
						case KP_SAVE :
							BuzOn(1);
							cursor_clear(&cursor);
							*ret_val = temp_val;
							ret=KeyDrv.CnvCode;
							goto EX_ESC;
						case KP_ESC :
							BuzOn(1);
							cursor_clear(&cursor);
							ret= KeyDrv.CnvCode;
							goto EX_ESC;
						case KP_CLEAR :
							if(pass == KI_NOEDIT) {	// Added by CJK 20041223 
								BuzOn(2);
								break;
							}
							BuzOn(1);
							temp_val = 0;
							break;
						case KEY_SHIFT :
							BuzOn(1);
							KEY_Set_Shift(3);
							break;
						case KP_CAPS :
							BuzOn(1);
							KEY_Set_Caps();	// Added by CJK 20051027
							break;
						default :
							BuzOn(2);
							break;
					}
					break;
				case KEY_TYPE_NUMERIC :
					if(pass == KI_NOEDIT) {	// Added by CJK 20041223 
						BuzOn(2);
						break;
					}
					switch(KeyDrv.CnvCode) {
						case KEY_NUM_0 :
							BuzOn(1);
							temp_val = 0;
							break;
						case KEY_NUM_1 :
							BuzOn(1);
							temp_val = 1;
							break;
						default :
							BuzOn(2);
							break;
					}
					break;		 
				case KEY_TYPE_ASCII :
					if(pass == KI_NOEDIT) {	// Added by CJK 20041223 
						BuzOn(2);
						break;
					}
					temp_n = (INT16U)KeyInSet.char_no & 0xFF;
					temp_y = (INT16U)KeyInSet.char_yes & 0xFF;

					if(KeyDrv.CnvCode - KP_ASC_00 == temp_n 
							|| KeyDrv.CnvCode - KP_ASC_00 == temp_n + 0x20) {
						BuzOn(1);
						temp_val = 0;
					} else if(KeyDrv.CnvCode - KP_ASC_00 == temp_y 
							|| KeyDrv.CnvCode - KP_ASC_00 == temp_y + 0x20) {
						BuzOn(1);
						temp_val = 1;
					} else {
						BuzOn(2);
					}
					break;
				default :
					BuzOn(2);
					break;
			}
			if ((!KeyDrv.key_caps) && (KeyDrv.Shifted) && (KeyDrv.CnvCode != KEY_SHIFT)) KEY_Set_Shift(3);
		}
		EX_ESC:	
					if (pass != KI_PASS) KEY_Set_Shift(0);
		KEY_SetMode(keymode);
		KeyDrv.PgmMode = pgmmode;
		DspStruct.DirectDraw = backup_directdraw;
		return ret;
	}
	
	
	void keyin_setcallback(void (*function)() )
	{
		keyin_callback_on=1;
		keyin_callback=function;
	}
	
	extern INT16S ParamScroll_Xpt;
	extern INT16U ParamScroll_PrevTime;
	void keyin_setanycheck(void (*function)() )
	{
		keyin_anycheck_on=1;
		keyin_anycheck=function;
		// for Parameter caption Scroll
		ParamScroll_PrevTime = SysTimer10ms;
		ParamScroll_Xpt = 0;
		////
	}
	
	void keyin_clearcallback(void)
	{
		keyin_anycheck_on=0;
		keyin_callback_on=0;
	}
	
	//DEL060315INT16U keyin_getrawcode(INT16U code)
	//DEL060315{
	//DEL060315	INT32U pnt;
	//DEL060315	INT16U rawcode,zcode;
	//DEL060315	INT16S    i;
	//DEL060315
	//DEL060315	pnt = KEY_NORMAL_TABLE_ADDR;
	//DEL060315
	//DEL060315	rawcode=0;
	//DEL060315	for (i=0; i<KEY_MAX_TABLE_SIZE/2; i++) {
	//DEL060315		zcode=Flash_wread(pnt);
	//DEL060315		if (zcode==code) {
	//DEL060315			rawcode=i;
	//DEL060315			break;
	//DEL060315		}
	//DEL060315		pnt+=2;
	//DEL060315	}
	//DEL060315	return rawcode;
	//DEL060315}
	
	
	INT16U keyin_getrawcode_bytype(INT8U type, INT16U code)
	{
		INT32U pnt;
		INT16U rawcode,zcode;
		INT16S    i;
		
		switch(type) {
			default :
			case 0: pnt = FLASH_KEY_AREA;
			break;
			case 1:	pnt = FLASH_KEY_PGM_AREA;
			break;
			case 2: pnt = FLASH_KEY_PGM2_AREA;
			break;
		}
		if(KeyDrv.Shifted) {
			pnt += KEY_MAX_TABLE_SIZE;
		}
		pnt+= DFLASH_BASE;
		
		rawcode=0;
		for (i=0; i<KEY_MAX_TABLE_SIZE/2; i++) {
			zcode=Flash_wread(pnt);
			if (zcode==code) {
				rawcode=i;
				break;
			}
			pnt+=2;
		}
		return rawcode;
	}
	
#define MAX_FUNCTION_KEY	200
#ifdef USE_TRACE_STANDALONE
#define KS_SPECIALUSE_OFFSET	400	// 용도키 401~500
#endif
	INT16U  keycode[]={								  // Function Key Code
		KS_MINUS		,KS_SALE_PER		,KS_SALE_SPC    	, // 0..2	//(1)..(3)
		KS_SALE_NEG		,KS_FIXED		,KS_TYPE        	, // 3..	//(4)..
		KS_CASHOPEN		,KS_GTOTAL		,KS_REFERENCE	 	, // 6..	//(7)..
#ifndef USE_ONLY_LABEL
		KS_INWEIGHT		,KS_LABELTICKET		,KS_RESET       	, // 9..	//(10)..
#else
		KS_INWEIGHT		,KS_UNUSE		,KS_RESET       	, // 9..	//(10)..
#endif
		KS_MISC_WEIGHT  	,KS_MISC_COUNT   	,KS_MISC_PCS    	, // 12..	//(13)..
		KS_EDT_SELL		,KS_EDT_ORIGIN		,KS_EDT_ITEM    	, // 15..	//(16)..
		KS_EDIT_DATETIME	,KS_EDT_DISCOUNT 	,KS_EDT_SPEEDKEY	, // 18..	//(19)..
#ifndef USE_ONLY_LABEL		
		KS_EDT_PRICE		,KS_CLERK_CALL	 	,KS_LASTVOID	 	, // 21..	//(22)..
#else
		KS_EDT_PRICE		,KS_UNUSE	 	,KS_LASTVOID	 	, // 21..	//(22)..
#endif
		KS_CURRENCY_01  	,KS_CURRENCY_02  	,KS_CURRENCY_03 	, // 24..	//(25)..
		KS_CURRENCY_04  	,KS_BATCH        	,		       	  // 27..	//(28)..
#ifdef USE_KOR_TRACE
		KS_KTR_MEMBER			,						  // 29..	//(30)..
#else
		KS_UNUSE			,						  // 29..	//(30)..
#endif
		KS_TARE_01		,KS_TARE_02		,KS_TARE_03     	, // 30..	//(31)..
		KS_TARE_04		,KS_TARE_05		,KS_TARE_06     	, // 33..	//(34)..
		KS_TARE_07		,KS_TARE_08		,KS_TARE_09     	, // 36..	//(37)..
		KS_UNIT1		,KS_UNIT2		,KS_UNIT3       	, // 39..	//(40)..
		KS_KGLB			,KS_PACKDATE	 	,			  // 42..       //(43)..
#ifdef USE_KOR_TRACE
		KS_KTR_REQ_NEXTID	,						  // 44..		//(45)..
		KS_KTR_REQ_NONEID	,KS_KTR_REQ_COMPID	,KS_KTR_REQ_FIXEDID , // 45..	//(46)..
		KS_KTR_MANUAL_OPER	,						  // 48..	//(49)..
#else
  #ifndef USE_ONLY_LABEL		
		KS_TICKET_NO		,						  // 44..       //(45)..
  #else
		KS_UNUSE		,						  // 44..       //(45)..		
  #endif
		KS_UNUSE		,KS_UNUSE	 	,KS_UNUSE		, // 45..	//(46)..
		KS_UNUSE		,						  // 48..	//(49)..
#endif
#ifndef USE_ONLY_LABEL
		KS_PRINTDATE		,KS_CLERK_01					, // 49..	//(50)..
		KS_CLERK_02		,KS_CLERK_03		,KS_CLERK_04		, // 51..	//(52)..
		KS_CLERK_05		,KS_CLERK_06		,KS_CLERK_07		, // 54..	//(55)..
		KS_CLERK_08		,KS_CLERK_09		,KS_CLERK_10		, // 57..	//(58)..
		KS_CLERK_11		,KS_CLERK_12		,KS_CLERK_13		, // 60..	//(61)..
		KS_CLERK_14		,KS_CLERK_15		,KS_CLERK_16		, // 63..	//(64)..
		KS_CLERK_17		,KS_CLERK_18		,KS_CLERK_19		, // 66..	//(67)..
		KS_CLERK_20		,KS_CLERK_21		,KS_CLERK_22		, // 69..	//(70)..
		KS_CLERK_23		,KS_CLERK_24		,KS_CLERK_25		, // 72..	//(73)..
		KS_CLERK_26		,KS_CLERK_27		,KS_CLERK_28		, // 75..	//(76)..
		KS_CLERK_29		,KS_CLERK_30		,KS_CLERK_31		, // 78..	//(79)..
		KS_CLERK_32		,KS_LABEL_01		,KS_LABEL_02		, // 81..	//(82)..
		KS_LABEL_03		,KS_LABEL_04		,KS_LABEL_05		, // 84..	//(85)..
		KS_LABEL_06		,KS_LABEL_07		,KS_LABEL_08		, // 87..	//(88)..
		KS_EDT_TAX		,KS_EDT_TRACE		,			  // 90..	//(91)..	
#else
		KS_PRINTDATE		,KS_UNUSE					, // 49..	//(50)..
		KS_UNUSE		,KS_UNUSE		,KS_UNUSE		, // 51..	//(52)..
		KS_UNUSE		,KS_UNUSE		,KS_UNUSE		, // 54..	//(55)..
		KS_UNUSE		,KS_UNUSE		,KS_UNUSE		, // 57..	//(58)..
		KS_UNUSE		,KS_UNUSE		,KS_UNUSE		, // 60..	//(61)..
		KS_UNUSE		,KS_UNUSE		,KS_UNUSE		, // 63..	//(64)..
		KS_UNUSE		,KS_UNUSE		,KS_UNUSE		, // 66..	//(67)..
		KS_UNUSE		,KS_UNUSE		,KS_UNUSE		, // 69..	//(70)..
		KS_UNUSE		,KS_UNUSE		,KS_UNUSE		, // 72..	//(73)..
		KS_UNUSE		,KS_UNUSE		,KS_UNUSE		, // 75..	//(76)..
		KS_UNUSE		,KS_UNUSE		,KS_UNUSE		, // 78..	//(79)..
		KS_UNUSE		,KS_LABEL_01		,KS_LABEL_02		, // 81..	//(82)..
		KS_LABEL_03		,KS_LABEL_04		,KS_LABEL_05		, // 84..	//(85)..
		KS_LABEL_06		,KS_LABEL_07		,KS_LABEL_08		, // 87..	//(88)..
		KS_UNUSE		,KS_EDT_TRACE		,			  // 90..	//(91)..		
#endif
#ifdef USE_TRACE_STANDALONE
		KS_INDIVIDUAL_01	,						  // 92..	//(93)..
		KS_INDIVIDUAL_02		,KS_INDIVIDUAL_03	,KS_INDIVIDUAL_04	, // 93..	//(94)..
		KS_INDIVIDUAL_05		,KS_INDIVIDUAL_06	,KS_INDIVIDUAL_07	, // 96..	//(97)..
		KS_INDIVIDUAL_08		,KS_INDIVIDUAL_09	,KS_INDIVIDUAL_10	, // 99..	//(100)..
		KS_INDIVIDUAL_11		,KS_INDIVIDUAL_12	,KS_INDIVIDUAL_13	, // 102..	//(103)..
		KS_INDIVIDUAL_14		,KS_INDIVIDUAL_15	,KS_INDIVIDUAL_16	, // 105..	//(106)..
		KS_EDIT_INDIVIDUAL	,KS_CALL_INDIVIDUAL	,KS_INPUT_INDIVIDUAL	, // 108..	//(109)..
		KS_INPUT_LOT		,KS_TRACE_SAVETYPE	,KS_LIST_INDIV	, // 111..	//(112)..
		KS_CALL_SLAUGHTHOUSE	,KS_MULTI_INDIVIDUAL	,KS_CALL_MEATUSE	, // 114..	//(115)..
		KS_LOT_LIST		,KS_CALL_MEATPART	,KS_CALL_MEATGRADE	, // 117..	//(118)..
		KS_OFFLINE		,KS_CALL_INDIV		, // 120..	//(121)..
  #ifdef USE_SETPRODUCT_SALE
		KS_SETPRODUCT_CLEAR,						//122..	  //(123)
		KS_MODE_SETPRODUCT,						  // 123..	//(124)..
  #else
		KS_UNUSE		,						//122..	  //(123)
		KS_UNUSE		,						  // 123..	//(124)..
  #endif
#else
#ifndef USE_ONLY_LABEL
		KS_CLERK_33		,						  // 92..	//(93)..
		KS_CLERK_34		,KS_CLERK_35		,KS_CLERK_36		, // 93..	//(94)..
		KS_CLERK_37		,KS_CLERK_38		,KS_CLERK_39		, // 96..	//(97)..
		KS_CLERK_40		,KS_CLERK_41		,KS_CLERK_42		, // 99..	//(100)..
		KS_CLERK_43		,KS_CLERK_44		,KS_CLERK_45		, // 102..	//(103)..
		KS_CLERK_46		,KS_CLERK_47		,KS_CLERK_48		, // 105..	//(106)..
		KS_CLERK_49		,KS_CLERK_50		,KS_CLERK_51		, // 108..	//(109)..
		KS_CLERK_52		,KS_CLERK_53		,KS_CLERK_54		, // 111..	//(112)..
		KS_CLERK_55		,KS_CLERK_56		,KS_CLERK_57		, // 114..	//(115)..
		KS_CLERK_58		,KS_CLERK_59		,KS_CLERK_60		, // 117..	//(118)..
		KS_CLERK_61		,KS_CLERK_62		,KS_CLERK_63		, // 120..	//(121)..
		KS_CLERK_64		,						                   // 123..	//(124)..
#else
		KS_UNUSE		,						  // 92..	//(93)..
		KS_UNUSE		,KS_UNUSE		,KS_UNUSE		, // 93..	//(94)..
		KS_UNUSE		,KS_UNUSE		,KS_UNUSE		, // 96..	//(97)..
		KS_UNUSE		,KS_UNUSE		,KS_UNUSE		, // 99..	//(100)..
		KS_UNUSE		,KS_UNUSE		,KS_UNUSE		, // 102..	//(103)..
		KS_UNUSE		,KS_UNUSE		,KS_UNUSE		, // 105..	//(106)..
		KS_UNUSE		,KS_UNUSE		,KS_UNUSE		, // 108..	//(109)..
		KS_UNUSE		,KS_UNUSE		,KS_UNUSE		, // 111..	//(112)..
		KS_UNUSE		,KS_UNUSE		,KS_UNUSE		, // 114..	//(115)..
		KS_UNUSE		,KS_UNUSE		,KS_UNUSE		, // 117..	//(118)..
		KS_UNUSE		,KS_UNUSE		,KS_UNUSE		, // 120..	//(121)..
		KS_UNUSE		,						                   // 123..	//(124)..
#endif
#endif
#ifndef USE_ONLY_LABEL
		KS_SCAN_CLERK		,KS_BARCODE		, 			            // 124..	//(125)..
 		KS_EDT_INGREDIENT	,KS_LMSG_CALL		,KS_SALE_SPECIAL, // 126..	//(127)..
 		KS_PACKED_DATE		,KS_DATE_MODE		,KS_DEPT_CALL	, // 129..	//(130)..
		KS_EXT_PRINT		,KS_RFID_TAG		,KS_CHECKIN		, // 132..	//(133)..	
		KS_CHECK_PROD		,KS_DELETE_PROD		,KS_CUSTOMER_NO	, // 135..	//(136)..
		KS_CUSTOMER_CLOSE	,KS_REOPEN		,KS_FIXED_10PER		, // 138..	//(139)..
		KS_FIXED_20PER		,KS_FIXED_30PER		,KS_FIXED_40PER		, // 141..	//(142)..
		KS_FIXED_50PER		,KS_EXT_ADD_PRINT  	,					// 144..	//(145)..
  #ifdef USE_LOT_NUMBER_FUNCTION
		KS_EDIT_LOT			, // 146..	//(147)..
  #else
  		KS_EDIT_STORE			, // 146..	//(147)..	
  #endif
		
		KS_PAY_1		,KS_PAY_2		,KS_PAY_3		, // 147..	//(148)..
		KS_PAY_4		,KS_PAY_5		,KS_PAY_6		, // 150..	//(151)..
		KS_PAY_7		,KS_RETURN          	,KS_UNUSE       , // 153..	//(154)..
		KS_LABEL_09		,KS_LABEL_10        	,KS_LABEL_11    , // 156..	//(157)..	//LABEL09 ~ LABEL20 메모리 할당X, 현재 사용X
		KS_LABEL_12		,KS_LABEL_13        	,KS_LABEL_14    , // 159..	//(160)..
		KS_LABEL_15		,KS_LABEL_16        	,KS_LABEL_17    , // 162..	//(153)..
		KS_LABEL_18		,KS_LABEL_19        	,KS_LABEL_20    , // 165..	//(166)..
		KS_AUTO		,KS_DATETIME       	,KS_GLOBAL_TAX  , // 168..	//(169)..
		KS_GLOBAL_LABEL_BAR	,KS_MULTI				, // 171..	//(172)..
#else
		KS_UNUSE		,KS_BARCODE		, 			            // 124..	//(125)..
 		KS_EDT_INGREDIENT	,KS_LMSG_CALL		,KS_SALE_SPECIAL, // 126..	//(127)..
 		KS_PACKED_DATE		,KS_DATE_MODE		,KS_DEPT_CALL	, // 129..	//(130)..
		KS_EXT_PRINT		,KS_RFID_TAG		,KS_CHECKIN		, // 132..	//(133)..	
		KS_CHECK_PROD		,KS_DELETE_PROD		,KS_UNUSE	, // 135..	//(136)..
		KS_UNUSE		,KS_UNUSE		,KS_FIXED_10PER		, // 138..	//(139)..
		KS_FIXED_20PER		,KS_FIXED_30PER		,KS_FIXED_40PER		, // 141..	//(142)..
		KS_FIXED_50PER		,KS_EXT_ADD_PRINT  	,					// 144..	//(145)..
  #ifdef USE_LOT_NUMBER_FUNCTION
		KS_EDIT_LOT			, // 146..	//(147)..
  #else
  		KS_EDIT_STORE			, // 146..	//(147)..	
  #endif
		KS_PAY_1		,KS_PAY_2		,KS_PAY_3		, // 147..	//(148)..
		KS_PAY_4		,KS_PAY_5		,KS_PAY_6		, // 150..	//(151)..
		KS_PAY_7		,KS_UNUSE          	,KS_UNUSE       , // 153..	//(154)..
		KS_LABEL_09		,KS_LABEL_10        	,KS_LABEL_11    , // 156..	//(157)..	//LABEL09 ~ LABEL20 메모리 할당X, 현재 사용X
		KS_LABEL_12		,KS_LABEL_13        	,KS_LABEL_14    , // 159..	//(160)..
		KS_LABEL_15		,KS_LABEL_16        	,KS_LABEL_17    , // 162..	//(153)..
		KS_LABEL_18		,KS_LABEL_19        	,KS_LABEL_20    , // 165..	//(166)..
		KS_AUTO		,KS_DATETIME       	,KS_UNUSE  , // 168..	//(169)..
		KS_GLOBAL_LABEL_BAR	,KS_MULTI				, // 171..	//(172)..
#endif
#ifdef USE_CHN_IO_BOARD
		KS_USBSAVE_LOG							, // 173..	//(174)..
#else
		KS_UNUSE							, // 173..	//(174)..
#endif
#if defined(USE_CHN_CART_SCALE) || defined(USE_CHN_EMART_TRACE)
		KS_CHN_CALL_TRACE	,KS_CHN_TRACE1		,KS_CHN_TRACE2	, // 174..	//(175)..
		KS_CHN_TRACE3		,KS_CHN_TRACE4		,KS_CHN_EDIT_REMAIN, // 177..	//(178)..
#else
		KS_UNUSE		,KS_UNUSE		,KS_UNUSE	, // 174..	//(175)..
		KS_UNUSE		,KS_UNUSE		,KS_UNUSE	, // 177..	//(178)..
#endif
		KS_FEED			,KS_PREPACK		,KS_NETWEIGHT		, //180.. //(181)..
		KS_ZERO			,KS_TARE			,KS_EDT_KEY	, //183.. //(184)..
		KEY_SHIFT		,KS_CLEAR			,KS_PRINT		, //186.. //(187)..
		KS_PLU_LIST		,KS_LOCK_SCREEN		,KS_ADD		,	 //189.. //(190)..
#ifdef CL7200_SELF_PROJECT
		KS_PLU_PAGE_UP	,KS_PLU_PAGE_DN   ,KS_CATEGORY_01,// 191  //192
		KS_CATEGORY_02    ,KS_CATEGORY_03   ,KS_CATEGORY_04,
#else
  #ifdef USE_TOPMART_DEFAULT
  		KS_INPUT_ITEMCODE13		,KS_UNUSE		,KS_INPUTQTY	, // 192..	//(193)..
		KS_SALE_PRICE_CHG_UNITPRICE		,KS_PACKDATE_FLAG		,KS_ASK_INDIVIDUAL	, // 195..	//(196)..
  #else
		KS_INPUT_ITEMCODE13		,KS_UNUSE		,KS_UNUSE	, // 192..	//(193)..
		KS_UNUSE		,KS_UNUSE		,KS_ONLINE	, // 195..	//(196)..
  #endif
#endif
#ifdef USE_TRACE_STANDALONE
		KS_VIEW_LOT		,KS_MAKE_B_LOT		,KS_MAKE_P_LOT		, // 198..	//(199)..
		KS_INDIV_SAVE_MODE	,KS_LIST_SLAUGHT	,KS_LIST_INGREDIENT	, // 201..	//(202)..
		KS_LIST_MEATUSE		,KS_LIST_MEATPART				, // 204..	//(205)..
#else
		KS_UNUSE		,KS_UNUSE		,KS_UNUSE	, // 198..	//(199)..
		KS_UNUSE		,KS_UNUSE		,KS_UNUSE	, // 201..	//(202)..
		KS_UNUSE		,KS_UNUSE				, // 204..	//(205)..
#endif
		KS_PLU			,KS_NUM_0		,KS_NUM_1	, // 206..	//(207)..
		KS_NUM_2		,KS_NUM_3		,KS_NUM_4	, // 209..	//(210)..
		KS_NUM_5		,KS_NUM_6		,KS_NUM_7	, // 212..	//(213)..
		KS_NUM_8		,KS_NUM_9		,KS_NUM_00	, // 215..	//(216)..
		KS_OVERRIDE		,KS_FOR			,KS_VOID	, // 218..	//(219)..
		KS_PAY			,KS_TTL			,KS_PREV_PAGE		, // 221..	//(222)..
		KS_NEXT_PAGE		,KS_SELECT_CAT		,KS_COPY_TICKET			,// 224..	//(225)..
		KS_CHANGE_PREFIX	,KS_UNUSE		,KS_UNUSE			,// 227..	//(228)..
#ifdef USE_SETPRODUCT_SALE
		KS_NONWEIGHING_LABLE	,// 230..	//(231)..
#else
		KS_UNUSE	,	// 230..	//(231)..
#endif 
		KS_UNUSE		,KS_FIXED_5PER			,// 231..	//(232)..
		KS_CHANGE_KEYSET	,KS_UNUSE		,KS_UNUSE			,// 233..	//(234)..
		KS_UNUSE,		KS_UNUSE,		KS_UNUSE				,// 236..	//(237)..
		KS_UNUSE,		KS_UNUSE,		KS_UNUSE				,// 239..	//(240)..
		KS_UNUSE,		KS_UNUSE,		KS_UNUSE				,// 242..	//(243)..
		KS_UNUSE,		KS_UNUSE,		KS_UNUSE				,// 245..	//(246)..
		KS_UNUSE,		KS_UNUSE,		KS_UNUSE				,// 248..	//(249)..
		KS_UNUSE,		KS_UNUSE,		KS_UNUSE				,// 251..	//(252)..
		KS_UNUSE,		KS_UNUSE,		KS_UNUSE				,// 254..	//(255)..
		KS_UNUSE,		KS_UNUSE,		KS_UNUSE				,// 257..	//(258)..
		KS_UNUSE,		KS_UNUSE,		KS_UNUSE				,// 260..	//(261)..
		KS_UNUSE,		KS_UNUSE,		KS_UNUSE				,// 263..	//(264)..
		KS_UNUSE,		KS_UNUSE,		KS_UNUSE				,// 266..	//(267)..
#ifdef USE_SUNWOO_POS_DEFAULT
		KS_WAREHOUSING,	KS_LOSS,		KS_WHOLESALE			,// 269..	//(270)..
		KS_RETAIL												,// 272..	//(273)..
#else
		KS_UNUSE,		KS_UNUSE,		KS_UNUSE				,// 269..	//(270)..
		KS_UNUSE												,// 272..	//(273)..
#endif
#ifdef USE_HYUNDAI_SET_FUNCTION
		KS_SET_START,	KS_SET_END, 	KS_SET_CANCEL			,// 273..//(274)											,// 276..//(277)
#else
		KS_UNUSE,	KS_UNUSE,	KS_UNUSE						,// 273..//(274)
#endif 
#ifdef USE_REGIST_DISPOSAL		
		KS_DISPOSAL 											,// 276..//(277)
#else
		KS_UNUSE												,// 276..//(277)
#endif
		KS_UNUSE,		KS_UNUSE								,// 277..	//(278)..
		0 // Max Function Key(MAX_FUNCTION_KEY) : 300
	};
	
	INT16U UserFunctionQty = 0;
	void key_GetUserFunctionQty(void)
	{
		int i;
		
		UserFunctionQty = 0;
		for (i = 0; i < sizeof(keycode)/sizeof(keycode[0]); i++) {
			if (keycode[i] == 0) break;
		}
		UserFunctionQty = i;
	}
	
#define M_ID_FNCKEY_KEY_NO 1
#define M_ID_FNCKEY_CODE_NO 2
	INT8U callback_check_key_no(INT32U num)
	{
		INT8U result;
		INT16U key_max;
		
		if (Startup.scale_type==1) key_max=48;
#if defined(USE_CL5200_KEY)
		else if (Startup.scale_type==6) key_max=54;
#endif
		else                       key_max=72;
		if (Operation.doublePLUkey == 2) key_max *= 2;
		if (Startup.scale_type == 5) // D-type
		{
			key_max =154;  //add function key F1~F4, add plu function key 149~154
		}
		if (((INT16S)num > key_max) || (num == 0)) {
			BuzOn(2);
			return OFF;
		}
		
		return ON;
	}
	
	void keycall_key_number_function(long num)
	{
		CAPTION_STRUCT 	cap;
		POINT 		disp_p;
		char 		temp[36];
		char		form[36];
		INT32U plukey_id;
		INT16U funcKey_id;
		INT16U rawid;
		INT8U level;
		INT32U funcCode;
		INT16U w;
		INT8U id_code_no;
		INT32U addr;
		
		id_code_no = Menu_GetMenuId(M_ID_FNCKEY_CODE_NO);
		
		plukey_id = num;
		plukey_id-=1;
		plukey_id+=KS_PLU_01;
		rawid    = KEY_getrawcode_from_plukey((INT16U)plukey_id, &level);
		funcKey_id = KEY_getnormalkey(rawid-1, level);
		
		funcCode=0;
		for (w=0; w<UserFunctionQty; w++) {
			if ((INT16U)funcKey_id==keycode[w]) {
				funcCode=w+1;
				break;
			}
		}
#ifdef USE_TRACE_STANDALONE
		if ((funcKey_id >= KS_SPECIALUSE_01) && (funcKey_id <= KS_SPECIALUSE_100)) {
			funcCode = funcKey_id;
			funcCode-= KS_SPECIALUSE_01;
			funcCode+= (KS_SPECIALUSE_OFFSET + 1);	// 401~500
			//data[1]=funcKey_id;
		}
#endif
		
		*Menu[id_code_no].pval = funcCode;
		
		MenuRefresh = ON;
	}
	
	void key_set_function(void)
	{
		CAPTION_STRUCT 	   cap;
		POINT 		   disp_p;
		char 	input_line;
		INT16S 	i, result,w,key_max;
		long  	ret_long;
		INT32U  addr_key;
		INT32U  key_num;
		INT32U  funcCode;
		INT16U  data[2],rawid;
		INT8U   level;
		INT32U  plukey_id;
		INT32U  funcKey_id;
		INT32U  cnvCode;
		
		key_num = 0;
		
		Menu_Init();
		caption_split_by_code(0x1881, &cap,0);
		Menu_HeadCreate((HUGEDATA char *)cap.form);
		
		caption_split_by_code(0x3881,&cap,0);
		caption_adjust(&cap,NULL);
		Menu_InputCreate(M_ID_FNCKEY_KEY_NO, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
				cap.input_max, cap.input_min, 0, &key_num, NULL, NULL, NULL);
		Menu_AddEndFunction(M_ID_FNCKEY_KEY_NO, keycall_key_number_function);
		Menu_AddCheckFunction(M_ID_FNCKEY_KEY_NO, callback_check_key_no);
		Menu_SetKeyMode(M_ID_FNCKEY_KEY_NO, 9);
		
		caption_split_by_code(0x3882, &cap,0);
		caption_adjust(&cap,NULL);
		Menu_InputCreate(M_ID_FNCKEY_CODE_NO, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
				cap.input_max, cap.input_min, 0, &funcCode, NULL, NULL, NULL);
		
		while(1)
		{
			result = Menu_InputDisplay();
			if(result == MENU_RET_SAVE)
			{
				if (funcCode == 0) cnvCode = 0;
				else {
					cnvCode = 0;
					if (funcCode > 0 && funcCode <= UserFunctionQty) {
						cnvCode = keycode[funcCode - 1];
#ifdef USE_TRACE_STANDALONE
					} else if ((funcCode >= KS_SPECIALUSE_OFFSET+1) && (funcCode <= KS_SPECIALUSE_OFFSET+100)) {
						cnvCode=(INT16U)funcCode;
						cnvCode-=(KS_SPECIALUSE_OFFSET + 1);
						cnvCode+=KS_SPECIALUSE_01;
#endif
					}
					if (cnvCode == 0 || cnvCode == KS_UNUSE) {
						funcCode = 0;
						BuzOn(2);
						error_display_caption(0x3880);
						continue;
					}
				}
				
				plukey_id = key_num - 1;
				plukey_id += KS_PLU_01;
				//input_value[0]=plukey_id;
				rawid=KEY_getrawcode_from_plukey(plukey_id, &level);
				if (rawid) {
					addr_key = rawid-1;
					addr_key *= 2;
					addr_key += DFLASH_BASE;
					addr_key += FLASH_KEY_AREA;
					if (cnvCode == 0) {
						if (level == 0)	{	// first level
							Flash_wwrite(addr_key,plukey_id);
							if (Operation.doublePLUkey != 2)	// toggle이 아닐 경우  level 2도 복구
							{
								addr_key += KEY_MAX_TABLE_SIZE;
								plukey_id -= KS_PLU_01;
								if (Startup.scale_type == 1) {	// B-type
									plukey_id+=KS_PLU_49;
#if defined(USE_CL5200_KEY)
								} else if (Startup.scale_type==6) {	//CL5200 B-type
									plukey_id+=KS_PLU_55;
#endif
								} else {			// P,H-Type
									plukey_id+=KS_PLU_73;
								}
								Flash_wwrite(addr_key,plukey_id);
							}
						}
						if (level == 1) {		// second level
							addr_key += KEY_MAX_TABLE_SIZE;
							Flash_wwrite(addr_key,plukey_id);
						}
					} else {
						if (level == 0)	{	// first level
							Flash_wwrite(addr_key, cnvCode);
							if (Operation.doublePLUkey != 2)	// toggle이 아닌 경우 level 2에도 동일코드 copy
							{
								addr_key += KEY_MAX_TABLE_SIZE;
								Flash_wwrite(addr_key, cnvCode);
							}
						}
						if (level == 1) {		// second level
							addr_key += KEY_MAX_TABLE_SIZE;
							Flash_wwrite(addr_key, cnvCode);
						}
					}
				} else {
					BuzOn(2);
					error_display_caption(0x3880);
					continue;
				}
			}
			else if(result == MENU_RET_ESC)
			{
				return;
			}
		}
		
	}
	
#ifdef USE_FUNCTION_KEY_BLOCK
void key_set_onoff_function(void)//key_set_onoff_function
{
	CAPTION_STRUCT 	   cap;
	POINT 		   disp_p;
	INT16S 	i, result;
	INT32U  temp;
	INT8U	data;
	long	input_value;
	INT8U keylocation[][4] = {
		"1x1","1x4","1x5",
		"2x1","2x2","2x3","2x4","2x5","2x6",
		"3x1","3x5","3x6","4x1","4x5","4x6",
		"5x1","5x6","6x1","0x0"
		};
	char string_buf[50];
	INT8U esc_count, enter_count;

	esc_count = 0;
	enter_count = 0;
	i = 18;
	input_value = 0;

	KEY_SetMode(9);
	Dsp_Fill_Buffer(0);
	DspLoadFont1(DSP_MENU_FONT_ID);
	menu_display_menucode(CurMenuCode, 1);
	menu_displaytitle(0x1882,0);
	menu_writepage(1, 1);
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
PAGE_VIEW:

	caption_split_by_code(0x9456, &cap,0);
	caption_adjust(&cap,"%s");
	disp_p = point((Startup.menu_type)*Startup.menu_ygap, 0);
	sprintf(string_buf, (char *)cap.form, keylocation[i]);
#ifdef USE_ARAB_CONVERT
    Convert_ArabPreview((INT8U *)string_buf, strlen(string_buf), 0, 0, 0); 
    arab_codetemp[strlen(string_buf)] = 0;
    disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
    Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
	Dsp_Write_String(disp_p, string_buf);
#endif
	Dsp_Diffuse();	  

	caption_split_by_code(0x9457,&cap,0);
	caption_adjust(&cap,NULL);
	disp_p = point((1+Startup.menu_type)*Startup.menu_ygap,0);
#ifdef USE_ARAB_CONVERT
    Convert_ArabPreview((INT8U *)cap.form, strlen(cap.form), 0, 0, 0); 
    arab_codetemp[strlen(cap.form)] = 0;
    disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
    Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
	Dsp_Write_String(disp_p, (HUGEDATA char *)cap.form);
#endif
	Dsp_Diffuse();
	
PAGE_KI_IN:
	if(enter_count == 0)
	{
		while(!KEY_Read()) {
		}
		if(KeyDrv.CnvCode != KP_ENTER)
		{
			for (i=0; i<18; i++)
			{
				if (KeyOnoff[i].keycode == KeyDrv.RawCode)
					break;
			}
		}
	}
	else if(enter_count == 1)
	{
		input_value = (get_global_lparam(GLOBAL_FUNCTION_KEY_BLOCK) >> i) & 0x00000001;
		if(input_value == 0)
		{
			input_value = 1;
		}
		else
		{
			input_value = 0;
		}
		result = keyin_yesno(KI_GRAPHIC, &input_value, 
			(1+Startup.menu_type)*Startup.menu_ygap, cap.input_x*Startup.menu_xgap, KI_NOPASS);
		if(i == 18)
			goto PAGE_KI_IN;
	}
	switch(KeyDrv.CnvCode) {
		case KP_ESC :
		case KS_MULTI :
			BuzOn(1);
			if(esc_count == 0) 
			{	
				if(enter_count == 1)
				{
					enter_count = 0;
					goto PAGE_VIEW;
				}
				esc_count = 1;
			} 
			else if(esc_count == 1) 
			{
				goto RET_ESC;//return;
			} 
			else esc_count = 0;
			goto PAGE_VIEW;
		case KP_ENTER:
			if(i == 18)
			{
				BuzOn(2);
				goto PAGE_KI_IN;
			}
			BuzOn(1);
			if(enter_count == 0) 
			{
				enter_count = 1;
			} 
			else if(enter_count == 1) 
			{
				goto RET_END;//return;
			} 
			else enter_count = 0;
			goto PAGE_VIEW;				
		default :
			if(i == 18)
				BuzOn(2);
			else
				BuzOn(1);
			esc_count = 0;
			goto PAGE_VIEW;
	}
#else

PAGE_VIEW_01:

	caption_split_by_code(0x9456, &cap,0);
	caption_adjust(&cap,NULL);
	display_clear(DISPLAY_PRICE);
	display_clear(DISPLAY_UNITPRICE);
	display_string(DISPLAY_UNITPRICE,(INT8U *)cap.form);

	disp_p = point((Startup.menu_type)*Startup.menu_ygap, 0);
	sprintf(string_buf, keylocation[i]);
	Dsp_Write_String(disp_p, string_buf);
	Dsp_Diffuse();
	goto PAGE_KI_IN;

PAGE_VIEW_02:

	caption_split_by_code(0x9457,&cap,0);
	caption_adjust(&cap,NULL);
	display_clear(DISPLAY_UNITPRICE);
	display_string(DISPLAY_UNITPRICE,(INT8U *)cap.form);
	Dsp_Diffuse();

PAGE_KI_IN:
	if(enter_count == 0)
	{
		while(!KEY_Read()) {
		}
		if(KeyDrv.CnvCode != KP_ENTER)
		{
			for (i=0; i<18; i++)
			{
				if (KeyOnoff[i].keycode == KeyDrv.RawCode)
					break;
			}
		}
	}
	else if(enter_count == 1)
	{
		input_value = (INT8U)((get_global_lparam(GLOBAL_FUNCTION_KEY_BLOCK) >> i) & 0x01);
		if(input_value == 0)
		{
			input_value = 1;
		}
		else
		{
			input_value = 0;
		}
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
		result = keyin_yesno(KI_GRAPHIC, (long*)&input_value, 
			(1+Startup.menu_type)*Startup.menu_ygap, cap.input_x*Startup.menu_xgap, KI_NOPASS);
#else
		result = keyin_yesno(KI_DEFAULT, (long*)&input_value, 
			(1+Startup.menu_type)*Startup.menu_ygap, cap.input_x*Startup.menu_xgap, KI_NOPASS);
#endif
		if(i == 18)
			goto PAGE_KI_IN;
	}
	switch(KeyDrv.CnvCode) {
		case KP_ESC :
		case KS_MULTI :
			BuzOn(1);
			if(esc_count == 0) 
			{	
				if(enter_count == 1)
				{
					enter_count = 0;
					goto PAGE_VIEW_01;
				}
				esc_count = 1;
			} 
			else if(esc_count == 1) 
			{
				goto RET_ESC;//return;
			} 
			else esc_count = 0;
			goto PAGE_VIEW_01;
		case KP_ENTER:
			if(i == 18)
			{
				BuzOn(2);
				goto PAGE_KI_IN;
			}
			BuzOn(1);
			if(enter_count == 0) 
			{
				enter_count = 1;
			} 
			else if(enter_count == 1) 
			{
				goto RET_END;//return;
			} 
			else enter_count = 0;
			goto PAGE_VIEW_02;
		default :
			if(i == 18)
				BuzOn(2);
			else
				BuzOn(1);
			esc_count = 0;
			goto PAGE_VIEW_01;
	}
#endif
RET_END:
	if(input_value == 0)
	{
		input_value = 1;
	}
	else
	{
		input_value = 0;
	}
	KeyOnoff[i].flag = input_value;

	if ((i/8) == 0)
	{
		data = get_global_bparam(GLOBAL_FUNCTION_KEY_BLOCK);
		temp = (0x01 << i);
		if (input_value)
			data |= temp;
		else
			data &= (~temp);
		set_global_bparam(GLOBAL_FUNCTION_KEY_BLOCK,data);
	}
	else if ((i/8) == 1)
	{
		data = get_global_bparam(GLOBAL_FUNCTION_KEY_BLOCK+1);
		temp = (0x01 << (i-8));
		if (input_value)
			data |= temp;
		else
			data &= (~temp);
		set_global_bparam(GLOBAL_FUNCTION_KEY_BLOCK+1,data);
	}
	else if ((i/8) == 2)
	{
		data = get_global_bparam(GLOBAL_FUNCTION_KEY_BLOCK+2);
		temp = (0x01 << (i-16));
		if (input_value)
			data |= temp;
		else
			data &= (~temp);
		set_global_bparam(GLOBAL_FUNCTION_KEY_BLOCK+2,data);
	}
	
	enter_count = 0;
	i = 18;
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
	goto PAGE_VIEW;
#else
	goto PAGE_VIEW_01;
#endif
RET_ESC:
	Dsp_Fill_Buffer(0); 		// LCD Buffer clear
	Dsp_Diffuse();
	KEY_SetMode(1);
	return;
}
#endif

#ifdef HEBREW_FONT

INT16U hebrew_nengtemp=0;	//hebrew_engtemp English Number
INT16U hebrew_nspectemp=0;	//Special Character Number
INT16U hebrew_nnumtemp=0;	//hebrew_engtemp Number Number
INT16U hebrew_nengspectemp=0;	//hebrew_engtemp Special Character Number
INT16U heb_nengtottemp=0;	//hebrew_engtemp Total Number
INT8U engChgEnable=0;	//영문 + 기호 : Enable
INT8U numChgEnable=0;	//숫자 + 기호 : Enable
INT8U specChgNum=0;	// Special Change Number-영문 다음 오는 특수기호 개수 (방향 전환)
INT8U specSepNum=0;	// Special Seperate Number-숫자 다음 오는 특수기호 개수 (구분 표시)

void Convert_HebrewInit(void)
{
	hebrew_status = 0;
	eng_num = 0;
	spec_num = 0;
	specChgNum = 0;
	specSepNum = 0;
	hebrew_nspectemp = 0;
	hebrew_nnumtemp = 0;
	heb_nengtottemp = 0;
	hebrew_nengspectemp = 0;
	hebrew_codetemp_pnt = -1;
	slashNum = 0;
	exceptNum = 0;
	bracketNum = 0;
	engChgEnable = 0;
	numChgEnable = 0;
	eBracketOpen = 0;
	bracketClose = 0;
	hBracketOpen = 0;
	bracketNumClose = 0;
	hBracketOpenNumber = 0;
	bracketHebrewIn = 0;
	hebrew_In = 0;
	exceptEnable = 0;
	//hebrew_count = 0;
}

void Convert_Heb_FontAddress(int hebrew_codetemp_pnt)
{
	INT8U 	hi, lo, nth;
	INT8U	cflag;		//cflag = combined flag
	INT16U 	i;

	for (i=0; i<hebrew_codetemp_pnt; i++) {
		if (hebrew_codetemp[i]==0) break;
		hi=hebrew_codetemp[i]>>8;
		cflag = hi & 0x08;
		hi&=0x07;
		lo=hebrew_codetemp[i]&0xff;
		hebrew_codetemp[i] &= 0x00FF;//wcm
	}
}

INT8U HebCheck(INT8U ch)
{
	if (ch == LF || ch == _CR || ch == HT || ch == FF) ch = 0xA0;//0xA0 don't have image in arab font map.
	if (ch < 0x80) return 0;
	if ((ch >= 0xe0 && ch <= 0xfa) || 0xa4) return ch;
	return FALSE;
}

INT8U BracketCheck(INT8U ch)
{
	if(ch == 0x28 || ch == 0x29 || ch == 0x5b || ch == 0x5d || ch == 0x7b || ch == 0x7d) return ch;
	else return FALSE;
}

INT8U BracketCloseChk(INT8U open, INT8U close)
{
	INT8U temp;
	
	temp = open^close;
	if(temp==0x06 || temp==0x01) return 1;	//괄호 열기, 괄호 닫기 매칭 확인
	else return 0;
}

INT8U CloseBracketCheck(INT8U ch)
{
	if(ch==0x29 || ch==0x5d || ch==0x7d || ch==0x3e) return TRUE;
	else return FALSE;
}

INT8U OpenBracketCheck(INT8U ch)
{
	if(ch==0x28 || ch==0x5b || ch==0x7b || ch==0x3c) return TRUE;
	else return FALSE;
}

INT8U BracketOpenConv(INT8U ch)
{
	if(ch == 0x28) ch = 0x29;	// ( -> )
	else if(ch == 0x3c) ch = 0x3e;	// < -> >
	else if(ch == 0x5b) ch = 0x5d;	// [ -> ]
	else if(ch == 0x7b) ch = 0x7d;	// { -> }
	return ch;
}

INT8U BracketCloseConv(INT8U ch)
{
	if(ch == 0x29) ch = 0x28;	// ) -> (
	else if(ch == 0x3e) ch = 0x3c;	// > -> <
	else if(ch == 0x5d) ch = 0x5b;	// ] -> [
	else if(ch == 0x7d) ch = 0x7b;	// } -> {
	return ch;
}

INT8U BracketConv(INT8U ch)
{
	if(ch == 0x28) ch = 0x29;	// ( -> )
	else if(ch == 0x29) ch = 0x28;	// ) -> (
	else if(ch == 0x3c) ch = 0x3e;	// < -> >
	else if(ch == 0x3e) ch = 0x3c;	// > -> <
	else if(ch == 0x5b) ch = 0x5d;	// [ -> ]
	else if(ch == 0x5d) ch = 0x5b;	// ] -> [
	else if(ch == 0x7b) ch = 0x7d;	// { -> }
	else if(ch == 0x7d) ch = 0x7b;	// } -> {
	return ch;
}

INT8U EngCheck(INT8U ch)
{
	if((ch >= 0x41 && ch <= 0x5a) || (ch >= 0x61 && ch <= 0x7a)) return ENGWORD;	//영문알파벳(A~Z,a~z)
	else if(ch >= 0x30 && ch <= 0x39) return NUMBER;	//숫자 (0~9)
	else if(ch>=0x23 && ch<=0x25) return NUMSIGN;	//기호(#,$,%)
	else if(ch==0x2f || ch==0x2e || ch==0x3a || ch==0x2d || ch==0x2c) return SEPSIGN;	//기호('/' '.' ':' '-' ',')
	else return 0;
}

INT8U SpecialCheck(INT8U ch)
{
	if((ch >= 0x20 && ch <= 0x2f) || (ch >= 0x3a && ch <= 0x40) || (ch >= 0x5b && ch <= 0x60) || (ch >= 0x7b && ch <= 0x7e)) return ch;
	else return 0;
}

INT8U Convert_HebrewInterpret(INT8U ch)
{
	int    i,j,k,k_temp,e_temp, bracketStart, bracketEnd, bracketOpen;
	INT16U v_16,cur_ch,specTemp, enSpectemp;
	INT8U  ret_ch,save_newchar, engFlag, engSpecFlag;

	ret_ch = 0;
	cur_ch = (INT16U)ch&0x00ff;
	
	switch (hebrew_status)
	{
		case 0:
HEB:		if(HebCheck((INT8U)cur_ch))
			{
				hebrew_codetemp_pnt++;
				hebrew_codetemp[hebrew_codetemp_pnt]=cur_ch;
				eng_num = 0;
				spec_num = 0;
				slashNum = 0;
				engChgEnable = 0;
				numChgEnable=0;
				cursor_direct = 1; // 1: Left, 2: Right
			} 
			else if(EngCheck(cur_ch))
			{ //영문 알파벳 구분
				hebrew_status = 1;
				goto ENG;				
			}
			else
			{  //히브리어 뒤 공백, 특수문자는 히브리어로 처리
				if(!cur_ch) break;
				if(EngCheck(cur_ch)==SEPSIGN) slashNum++;	//별도 특수 문자(/,.) 처리 
				else slashNum = 0;
				if(EngCheck(cur_ch)==NUMSIGN) exceptNum++;	//별도 특수 문자(#,$,%) 처리 
				if(BracketCheck(cur_ch))
				{
					hBracketOpen = cur_ch;
					cur_ch = BracketConv(cur_ch);
					bracketNum++;
					//if(!bracketClose) hBracketOpen = cur_ch;
				}
				hebrew_codetemp_pnt++;
				hebrew_codetemp[hebrew_codetemp_pnt]=(INT16U)cur_ch;
				hebrew_nspectemp++;
				cursor_direct = 1; // 1: Left, 2: Right
			}
			break;
		case 1:
ENG:		k_temp=hebrew_codetemp_pnt;
			heb_nengtottemp=hebrew_nengtemp+hebrew_nnumtemp+hebrew_nengspectemp;
			if (HebCheck((INT8U)cur_ch || (!specChgNum && hBracketOpen && BracketCheck(cur_ch) && BracketCloseChk(hBracketOpen,cur_ch)))  //영문 + 히브리어 경우 -> 영문 뒤집어 저장 (영문 뒤 특수문자, 공백은 영문으로 처리)
			|| (!EngCheck(cur_ch) && SpecialCheck(cur_ch) && hebrew_nnumtemp && !hebrew_nengtemp)   //영문 + 히브리어 경우 -> 영문 뒤집어 저장 (영문 뒤 특수문자, 공백은 영문으로 처리)
			|| (bracketClose && !bracketNumClose && BracketCheck(cur_ch) && !hBracketOpenNumber && !bracketHebrewIn) // 영문방향에서 ( -> ( -> ) -> ), 영문-> 괄호 두번 열고 -> 숫자 -> 괄호 닫고 -> 아랍어 입력시 loof 방지
			|| (/*bracketClose &&*/ !bracketNumClose && BracketCheck(cur_ch) && eBracketOpen && !BracketCloseChk(eBracketOpen,cur_ch))) // 영문방향에서 ( -> ( -> ) -> ( 
			{
				if(BracketCheck(cur_ch)&&eBracketOpen&&/*bracketClose &&*/ !bracketNumClose) // 영문방향에서 괄호 두 번 닫고 다시 연 상태
					goto SPEC;

				if(eBracketOpen && HebCheck((INT8U)cur_ch)) bracketHebrewIn =1;
				if ((hebrew_nengtemp || hebrew_nnumtemp)&& !bracketHebrewIn) 
				{
					hebrew_codetemp_pnt=Convert_HebrewReverseEng(hebrew_engtemp,k_temp);
				}
				if(!bracketHebrewIn)
				{
					hebrew_nengtemp=0;
					hebrew_nnumtemp=0;
					hebrew_nengspectemp=0;
					heb_nengtottemp=0;
				}
				hebrew_nspectemp=0;						
				if(hBracketOpen) hBracketOpen=0;
				eng_num = 0;
				spec_num = 0;
				slashNum = 0;
				engChgEnable = 0;
				numChgEnable = 0;
				specChgNum = 0;
				specSepNum = 0;
				cursor_direct = 1; // 1: Left, 2: Right
				goto HEB;				
			} 
			else if((EngCheck(cur_ch)>0 && EngCheck(cur_ch)<3) || //영문알파벳 및 숫자 구분 ( #, $, $ 문자 경우 별도 처리)
					(EngCheck(cur_ch)==3 && EngCheck(hebrew_engtemp[heb_nengtottemp-1])==NUMBER)||	//현재 문자 '예외문자' 이고  바로 전 문자 '숫자'
					(exceptEnable && EngCheck(cur_ch)==NUMSIGN)|| //숫자 뒤에 '예외문자' 가 연달아 온 경우
					(eBracketOpen && (cur_ch != 0x20) && hebrew_nengtemp)) 	//'괄호열기'상태이고 현재 문자 '공백X'이고 '영문' 있는 경우
			{
				if(EngCheck(cur_ch)==NUMSIGN && EngCheck(hebrew_engtemp[heb_nengtottemp-1])==NUMBER) exceptEnable = TRUE;
					else if(EngCheck(cur_ch)!=NUMSIGN) exceptEnable = FALSE;
				
				if(specChgNum || specSepNum || eBracketOpen || bracketClose)		//숫자 or 알파벳 뒤 특수기호 포함된 경우 or 별도 특수문자(# $ %) 경우
				{
					if((specSepNum && !exceptNum && slashNum!=1 && !eBracketOpen && !engChgEnable) ||	//별도 특수 문자(# $ %) 없고 '/' 1번 입력아니고 영문괄호없고 이전문자 영문X 경우
						(specSepNum && !exceptNum && slashNum==1 && !eBracketOpen && !engChgEnable && EngCheck(cur_ch)==ENGWORD))
					{
						//if((!hebrew_nengtemp && slashNum!=1 && !exceptNum && !eBracketOpen && !hBracketOpen) || 	//숫자 입력인 경우
						//	(EngCheck(cur_ch)==1 && specSepNum && !hebrew_nengtemp  && !exceptNum)) 	//현재 문자 '영문' 이고 이전 문자 '숫자'이고 이전 '영문X'인 경우
						/*(bracketClose && BracketCheck(hebrew_engtemp[heb_nengtottemp-1]) && EngCheck(cur_ch)==1)) {*/		//현재 '괄호 닫기'이고 이전 괄호 안 '영문'인 경우
						if(hebrew_nnumtemp && (EngCheck(cur_ch)==ENGWORD || EngCheck(cur_ch)==NUMBER))
						{
						//if (hebrew_nnumtemp) {	//앞문자 숫자인 경우
							hebrew_codetemp_pnt=Convert_HebrewReverseEng(hebrew_engtemp,k_temp);
							//}
							hebrew_nengtemp=0;
							hebrew_nspectemp=0;
							hebrew_nnumtemp=0;
							hebrew_nengspectemp=0;
							heb_nengtottemp=0;
							eng_num = 0;
							spec_num = 0;
							engChgEnable = 0;
							numChgEnable = 0;
							specChgNum = 0;
							specSepNum = 0;
							cursor_direct = 2; // 1: Left, 2: Right
						}
					}
					else
					{	//특수기호 뒤 숫자나 영문 올 경우
						if(specSepNum && ((exceptNum && EngCheck(cur_ch)==NUMBER) ||	//이전 문자 '%,$,#' 이고 숫자 입력인 경우
							(slashNum==1 && EngCheck(cur_ch)==NUMBER && EngCheck(hebrew_codetemp[k_temp])==SEPSIGN)))	//바로 이전 문자 '/ . : -' 이고 숫자 입력인 경우
						{ 
							enSpectemp = specSepNum;
							if(slashNum==1) slashNum = 0;
						} 
						else if	(specChgNum && (hebrew_nengtemp && /*!eBracketOpen && !bracketClose && */ engChgEnable))	//이전 문자 '영문' 인 경우
						{
							// A -> B -> ( -> ( -> 1 -> 2 -> ) -> ( -> 1 -> 2 -> )
							if((!bracketClose && !bracketHebrewIn) || (eBracketOpen && (hBracketOpenNumber >= 1) && !bracketHebrewIn))
								enSpectemp = specChgNum;
							else
								enSpectemp = 0;
						} 
						else enSpectemp = 0;

						if(enSpectemp)
						{
							specTemp = k_temp - enSpectemp + 1;
							
							for(i=enSpectemp; i>0; i--)
							{
								if(!eBracketOpen && BracketCheck((INT8U)hebrew_codetemp[specTemp])) 
								{
									eBracketOpen = BracketConv((INT8U)hebrew_codetemp[specTemp]);
									//eBracketOpenNumber++;
								}
								//if(!eBracketOpen && BracketCheck((INT8U)hebrew_codetemp[specTemp])) eBracketOpen = (INT8U)hebrew_codetemp[specTemp];
								//if(!eBracketOpen) eBracketOpen = BracketCheck((INT8U)hebrew_codetemp[specTemp]);
								hebrew_engtemp[heb_nengtottemp]=BracketCloseConv((INT8U)hebrew_codetemp[specTemp]);
								//if(!eBracketOpen) eBracketOpen = BracketCheck(hebrew_engtemp[heb_nengtottemp]);
								if(EngCheck(hebrew_engtemp[heb_nengtottemp])==ENGWORD) hebrew_nengtemp++;
								else if(EngCheck(hebrew_engtemp[heb_nengtottemp])==NUMBER) hebrew_nnumtemp++;
								else if(SpecialCheck(hebrew_engtemp[heb_nengtottemp])) hebrew_nengspectemp++;	//별도 특수 문자((,),[,],{,}) 처리 
								heb_nengtottemp=hebrew_nengtemp+hebrew_nnumtemp+hebrew_nengspectemp;
								hebrew_codetemp[specTemp++] = (INT16U)0xff;
							}
							spec_num = specChgNum;	//문자 위치 변경되어야 할 특수문자 갯수
							hebrew_nspectemp=0;	//임시 수정 ("IS6040-00010(60*40)" 2번 변환 시 개수 안 맞음)
							specChgNum=0;
							specSepNum=0;
							//slashNum = 0;
							if(eBracketOpen) hBracketOpen=0;
							cursor_direct = 2;
							//slashNum = 0;
						}
/*
						else if(hebrew_nengtemp && bracketClose) 	//영문방향 괄호 닫기 이후 영문 입력 시 처리
						{
							
							for(i=k_temp+1; i>0; i--)
							{
								if(hebrew_codetemp[k_temp]==0xff) break;
								hebrew_engtemp[k_temp]=BracketConv((INT8U)hebrew_codetemp[k_temp]);
								if(EngCheck(hebrew_engtemp[k_temp])==ENGWORD) hebrew_nengtemp++;
								else if(EngCheck(hebrew_engtemp[k_temp])==NUMBER) hebrew_nnumtemp++;
								else if(SpecialCheck(hebrew_engtemp[k_temp])) hebrew_nengspectemp++;	//별도 특수 문자((,),[,],{,}) 처리 
								heb_nengtottemp=hebrew_nengtemp+hebrew_nnumtemp+hebrew_nengspectemp;
								hebrew_codetemp[k_temp--] = (INT16U)0xff;					
							}
							bracketClose=0;
							bracketNumClose=0;
						}
*/
					}
				}
				if((BracketCheck(cur_ch) && !BracketCloseChk(eBracketOpen,cur_ch)) ||	// 현재 괄호 열린 상태이고 괄호가 닫히지 않은 경우
					(bracketHebrewIn && BracketCheck(cur_ch) && BracketCloseChk(eBracketOpen,cur_ch) && (hBracketOpenNumber != 1)))//영문방향에서 괄호가 두 번 이상  열린 상태인데 아랍어부분만 괄호로 닫힌 경우
					goto SPEC;
				//if(BracketCheck(cur_ch) && eBracketOpen && eBracketOpen != cur_ch) goto SPEC;
				if(!bracketHebrewIn && BracketCheck(cur_ch) && BracketCloseChk(eBracketOpen,cur_ch)) // <-git 커밋 11.11 15:00
				{
					bracketClose=1;
					bracketNumClose=0;
					if(hBracketOpenNumber) 
						hBracketOpenNumber--;
					if(!hBracketOpenNumber)
						eBracketOpen=0;
				}
				if(bracketHebrewIn && BracketCheck(cur_ch) && BracketCloseChk(eBracketOpen,cur_ch)) //{ 	//히브리어 방향 괄호 닫기 처리
				//if(BracketCheck(cur_ch) && eBracketOpen && eBracketOpen == cur_ch)
				{
					bracketStart = 0;//처음 괄호 시작 위치
					bracketOpen = 0;//마지막 괄호 시작 위치 
					for(i=0; i<heb_nengtottemp; i++) 
					{
						if(hebrew_engtemp[i]==BracketCloseConv(eBracketOpen)) 
						{
							if(!bracketStart) bracketStart=i;// 처음 괄호 열린 위치 저장  
							bracketOpen=i;//마지막 괄호 열린 위치
						}
					}
					if(hebrew_nengtemp && bracketStart) 
					{
					//}
					//if(hebrew_nengtemp && EngCheck(hebrew_engtemp[heb_nengtottemp-1])==2) {
						e_temp = heb_nengtottemp-1;
						j=1;
						k=0;
						for(i=e_temp; i>0; i--) 
						{
							if(i>bracketStart) 
							{
								hebrew_codetemp[k_temp--]=hebrew_engtemp[bracketStart+j];
								if(EngCheck(hebrew_engtemp[bracketStart+j])==ENGWORD) hebrew_nengtemp--;
								else if(EngCheck(hebrew_engtemp[bracketStart+j])==NUMBER) hebrew_nnumtemp--;
								else if(SpecialCheck(hebrew_engtemp[bracketStart+j])) hebrew_nengspectemp--;	//별도 특수 문자((,),[,],{,}) 처리 
								heb_nengtottemp=hebrew_nengtemp+hebrew_nnumtemp+hebrew_nengspectemp;
								hebrew_engtemp[bracketStart+j] = 0;
								j++;
							}
							else 
							{
								if(BracketCheck(hebrew_engtemp[i])) 
								{
									hebrew_codetemp[i]=BracketOpenConv(hebrew_engtemp[i]);
									if(SpecialCheck(hebrew_engtemp[i])) hebrew_nengspectemp--;	//별도 특수 문자(( ) [ ] { } < >) 처리 
									heb_nengtottemp=hebrew_nengtemp+hebrew_nnumtemp+hebrew_nengspectemp;
									hebrew_codetemp[i] = 0;
								}
								if(SpecialCheck(hebrew_engtemp[i]) || EngCheck(hebrew_engtemp[i])) 
								{
									hebrew_codetemp[k++]=BracketOpenConv(hebrew_engtemp[i]);
									if(EngCheck(hebrew_engtemp[i])==1) hebrew_nengtemp--;
									else if(EngCheck(hebrew_engtemp[i])==NUMBER) hebrew_nnumtemp--;
									heb_nengtottemp=hebrew_nengtemp+hebrew_nnumtemp+hebrew_nengspectemp;
									hebrew_engtemp[i] = 0;
								} 
								else break;
							}
						}
						//cur_ch = BracketCloseConv(cur_ch);
						eBracketOpen=0;
						bracketClose=1;
						bracketNumClose=1;
						bracketHebrewIn=0;
						goto SPEC;
					}
					//cur_ch = BracketOpenConv(cur_ch);	//영문 괄호 닫기 변환
					eBracketOpen=0;
					bracketClose=1;
					bracketNumClose=0;
					bracketHebrewIn=0;
				}
				hebrew_engtemp[heb_nengtottemp]=(INT8U)cur_ch;
				if(EngCheck(cur_ch)==ENGWORD) hebrew_nengtemp++;	//영문 문자 처리
				else if(EngCheck(cur_ch)==NUMBER) hebrew_nnumtemp++;	//숫자 처리
				else if(SpecialCheck(cur_ch)) hebrew_nengspectemp++;	//그 외 특수문자 처리 
				heb_nengtottemp=hebrew_nengtemp+hebrew_nnumtemp+hebrew_nengspectemp;
				hebrew_codetemp_pnt++;
				hebrew_codetemp[hebrew_codetemp_pnt]=(INT16U)0xff;
				if(EngCheck(cur_ch)!=SEPSIGN) slashNum=0;
				if(heb_nengtottemp>1) eng_num = heb_nengtottemp;
				cursor_direct = 2;	// 1: Left, 2: Right
			} 
			else 
			{
				if(!cur_ch) break;
SPEC:			if(cur_ch!=0x20) cursor_direct = 1; //입력 안 된 공간은 빈 문자로 처리되므로 다른 특수기호만 방향 지정
				//hebrew_codetemp_pnt++;
				//hebrew_codetemp[hebrew_codetemp_pnt]=(INT16U)cur_ch;
				if(EngCheck(cur_ch)==SEPSIGN) slashNum++;	//별도 특수 문자(/,.) 처리 
				else slashNum = 0;
				if(EngCheck(cur_ch)==NUMSIGN) exceptNum++;  //별도 특수 문자(#,$,%) 처리 
				if(BracketCheck(cur_ch))
				{
					if(OpenBracketCheck(cur_ch)) 
					{
						hBracketOpen = cur_ch;
						hBracketOpenNumber++;
					}
					else if(CloseBracketCheck(cur_ch))
					{
						hBracketOpenNumber--;
					}
					cur_ch = BracketConv(cur_ch);//1125
					bracketNum++;
				}
				hebrew_codetemp_pnt++;
				hebrew_codetemp[hebrew_codetemp_pnt]=(INT16U)cur_ch;
				if(hebrew_nnumtemp || (!hebrew_In && exceptNum)) numChgEnable = 1;
				if(numChgEnable) specSepNum++;
				//eBracketOpen=0;
				if(hebrew_nengtemp) engChgEnable = 1;
				if(engChgEnable)	specChgNum++;
				hebrew_nspectemp++;
			}
			break;
	}
	return ret_ch;
}

int Convert_HebrewReverseEng(INT8U *str,int k)
{
	INT8U status;
	INT8U special_char[10];
	
	INT8U special_nch;
	INT8U cur_ch;
	int   rev_l;
	int i,j;	   

	status     =0;
	special_nch=0;
	j=0;
	rev_l=heb_nengtottemp;  //영문 문자열 갯수
	
	for (i=0; i<heb_nengtottemp; i++) 
	{
		cur_ch=str[i];
 
		hebrew_subtemp[--rev_l]=cur_ch;	//영문 경우 순서 반대로 저장			
	}
	for (i=0; i<=hebrew_codetemp_pnt; i++)
	{
		if(j == heb_nengtottemp) break;

		if(hebrew_codetemp[i] == 0xff) 
		{
			hebrew_codetemp[i]=(INT16U)hebrew_subtemp[j++];
		}

		if ( (hebrew_subtemp[j] >= '0') && (hebrew_subtemp[j] <= '9') ) 
		{
			hebrew_codetemp[i] |= 0x1000;
		}
	}
/* 	for (i=0; i<hebrew_nengtemp; i++) {
		hebrew_codetemp[++k]=(INT16U)hebrew_subtemp[i];

		if ( (hebrew_subtemp[i] >= '0') && (hebrew_subtemp[i] <= '9') ) 
		{
			hebrew_codetemp[k] |= 0x1000;
		}
	}
*/
	hebrew_nengtemp=0;	
	hebrew_nnumtemp=0;	
	return k;
}

INT16U Convert_HebrewPreview(INT8U *str, INT16U leng, INT16U cur_pt, INT8U firstInputFlag)
{
	int   i, j, k, k_temp, r, z;
	INT16U v_16;
	INT8U cur_ch, engNum, specNum;
	INT16U ret;//ggm 080820
	ret = 0;
	
	if (leng == 0) {
		return ret;
	}

	Convert_HebrewInit(); 
	
	r=0;	
	for (i=leng-1; i>=0; i--) { // Check Last space
		if ((str[i]>0x20) || (str[i]==HT) || (str[i]==_CR) || (str[i]==LF) || (str[i]==DC1) || (str[i]==0x20)) {
			leng=i+1;
			break;
		}
		r++;
	}
	//mode=0;
	for (i = 0; i < leng; i++) {
		//if (i==leng-1) mode=1;
		cur_ch=Convert_HebrewInterpret(str[i]);
		if (cur_ch) Convert_HebrewInterpret(cur_ch);
		//[end]gm 080823 커서의 좌표계산
 		if ((cur_pt - 1) == i && firstInputFlag == 0) { //str[]에서 현재문자가 변환된 arab_codetemp[]에서의 좌표값			
			
			cur_ch = str[cur_pt - 1];
			
			if (HebCheck(cur_ch)) 
			{
				//ret = cur_pt + 1;
				ret = cur_pt;
			}
			else if((EngCheck(cur_ch)>0 && EngCheck(cur_ch)<3) || 	//현재 문자 '영문' or '숫자' 경우
					(EngCheck(cur_ch)==3 && EngCheck(hebrew_engtemp[heb_nengtottemp-2])==2)		//현재 문자 '#,$,%' 이고 바로 전 문자 '숫자' 경우
					|| (eBracketOpen && (cur_ch != 0x20) && hebrew_nengtemp)) {	//'괄호' 안 열리고  현재 문자 '공백'  아닐 경우
				if(eng_num) 
				{		
					for (j = 0; j < leng; j++)
					{	      				
						cur_ch = str[cur_pt - 1 + j];//*(buffer + (cur_pt - 1) + i);  //마지막 위치 문자부터 증가

						if (HebCheck(cur_ch) || cur_ch == 0x00) break;  //문자 없거나 히브리 문자 경우
					}
					for (k = 0; k < cur_pt; k++)
					{
						cur_ch = str[cur_pt - 1 - k]; //마지막 위치 문자부터 감소

						if (HebCheck(cur_ch) || cur_ch == 0x00) break;  //문자 없거나 히브리 문자 경우
					}
					ret = cur_pt - eng_num;	
				}
				else
				{
					ret = cur_pt - 1;
				}
			}
			else  //특수문자 구분
			{
				//ret = cur_pt + 1;
				if(bracketClose && !bracketNumClose) ret = cur_pt - eng_num;
				else ret = cur_pt;
			}
		//[end]gm 080821, buffer[]의 데이터위치가 arab_codetemp[]에서 변환된 위치찾기		
		}
		else if((cur_pt - 1) < i && eng_num)
		{
			if(eng_num > cur_pt) ret = eng_num - cur_pt;	//영문 커서 위치 0에서 방향키 입력 시 커서 위치 변경
			//else ret = ret + 1;	//영문,숫자 입력 시 커시위치 0유지
		}
	}
	k_temp=hebrew_codetemp_pnt;
	hebrew_codetemp_pnt=Convert_HebrewReverseEng(hebrew_engtemp,k_temp);
		
	//[start]gm 080821, 초기진입시, 커서 위치 결정함
	if (firstInputFlag) //처음 들어왔을 때 한번 실행항
	{			
		cur_ch = *(str);
		
		if (HebCheck(cur_ch) || cur_ch == 0x00) ret = 0;
		else
		{
			for (i = 0; i < leng; i++)
			{						
				cur_ch = *(str + i);
				if (HebCheck(cur_ch) || cur_ch == 0x00) break;
			}
			ret = i - 1;
		}

	}
	if (cur_pt == 0) //처음 입력위치, clear key 누른 경우. 
	{
		cur_ch = str[0];//*str;

		if (HebCheck(cur_ch) || cur_ch == 0x00) ret = 0;
		else if((cur_ch >= 0x41 && cur_ch <= 0x5a) || (cur_ch >= 0x61 && cur_ch <= 0x7a))
		{
			for (j = 0; j < leng; j++)//forward
			{						
				cur_ch = str[j]; //*(str+i) 
				if (HebCheck(cur_ch) || cur_ch == 0x00) break;
			}
			ret = j - 1;
		}
		else ret = 0;
	} 
	//[end]gm 080821, 초기진입시, 커서 위치 결정함
	for (i = 0; i < r; i++) { //나머지 공백 처리
		hebrew_codetemp_pnt++;
		hebrew_codetemp[hebrew_codetemp_pnt] =0x20;
	}
	hebrew_codetemp_pnt++;
	hebrew_codetemp[hebrew_codetemp_pnt]=0;
	
	Convert_Heb_FontAddress(hebrew_codetemp_pnt);

	return ret;
} 
#endif

#ifdef USE_ARAB_FONT
	INT16U arab_nengtemp=0;	//Arab_engtemp English Number
	INT16U arab_nspectemp=0;	//Special Character Number
	INT16U arab_nnumtemp=0;	//Arab_engtemp Number Number
	INT16U arab_nengspectemp=0;	//Arab_engtemp Special Character Number
	INT16U arab_nengtottemp=0;	//Arab_engtemp Total Number
	INT8U engChgEnable=0;	//영문 + 기호 : Enable
	INT8U numChgEnable=0;	//숫자 + 기호 : Enable
	INT8U specChgNum=0;	// Special Change Number-영문 다음 오는 특수기호 개수 (방향 전환)
	INT8U specSepNum=0;	// Special Seperate Number-숫자 다음 오는 특수기호 개수 (구분 표시)
	INT16U arab_nsubtemp=0;
	INT8U arablist[] = {
		//	00	01	02	03	04	05	06	07	08	09	0A	0B	0C	0D	0E	0F	
		/*80*/		0x81,												0x8D,	0x8E,
		/*90*/	0x90,								0x98,
		/*A0*/	0xA0,
		/*B0*/							
		/*C0*/	            	0xC2,	0xC3,	0xC4,	0xC5,	0xC6,	0xC7,	0xC8,	0xC9,	0xCA,	0xCB,	0xCC,	0xCD,	0xCE,	0xCF,
		/*D0*/	0xD0,	0xD1,	0xD2,	0xD3,	0xD4,	0xD5,	0xD6,		0xD8,	0xD9,	0xDA,	0xDB,	0xDC,	0xDD,	0xDE,	0xDF,	
		/*E0*/		0xE1,		0xE3,	0xE4,	0xE5,	0xE6,						0xEC,	0xED,	
		/*F0*/	                                                                            0xFD,	0xFE,	
		0x00};
	//INT8U arablist_tbl[]={
	//	   0   1   2   3   4   5   6   7   8   9   A   B   C   D
	//	   0,  0,  0,  1,  0,  0,  0,  0,  2,  0,  3,  4,  5,  6};
	
	INT8U arablist_mask[]={
		// bit0: start, bit1: middle, bit2: end
		//	00	01	02	03	04	05	06	07	08	09	0A	0B	0C	0D	0E	0F
		/*80*/	0x07,	0x07,	0x07,	0x07,	0x07,	0x07,	0x07,	0x07,	0x07,	0x07,	0x07,	0x07,	0x07,	0x07,	0x04,	0x07,
		/*90*/	0x07,	0x07,	0x07,	0x07,	0x07,	0x07,	0x07,	0x07,	0x07,	0x07,	0x07,	0x07,	0x07,	0x07,	0x07,	0x07,
		/*A0*/	0x07,	0x07,	0x07,	0x07,	0x07,	0x07,	0x07,	0x07,	0x07,	0x07,	0x07,	0x07,	0x07,	0x07,	0x07,	0x07,
		/*B0*/	0x07,	0x07,	0x07,	0x07,	0x07,	0x07,	0x07,	0x07,	0x07,	0x07,	0x07,	0x07,	0x07,	0x07,	0x07,	0x07,
		/*C0*/	0x07,	0x07,	0x04,	0x04,	0x04,	0x04,	0x07,	0x04,	0x07,	0x04,	0x07,	0x07,	0x07,	0x07,	0x07,	0x04,
		/*D0*/	0x04,	0x04,	0x04,	0x07,	0x07,	0x07,	0x07,	0x07,	0x07,	0x07,	0x07,	0x07,	0x07,	0x07,	0x07,	0x07,
		/*E0*/	0x07,	0x07,	0x07,	0x07,	0x07,	0x07,	0x04,	0x07,	0x07,	0x07,	0x07,	0x07,	0x07,	0x07,	0x07,	0x07,
		/*F0*/	0x07,	0x07,	0x07,	0x07,	0x07,	0x07,	0x07,	0x07,	0x07,	0x07,	0x07,	0x07,	0x07,	0x07,	0x07,	0x07};
	//	0x04,0x07,0x07,0x07,0x07,0x07,0x07,0x04,0x04,0x04,
	//	0x04,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,
	//	0x07,0x07,0x07,0x07,0x07,0x07,0x04,0x07,0x00};
	// arabcode_combine_e1[]={};
	
	INT8U IsArab(INT8U ch)
	{
		INT8U i;
		if (ch == LF || ch == _CR || ch == HT || ch == FF) ch = 0xA0;//0xA0 don't have image in arab font map. 	    
		if (ch < 0x80) return 0;

		for (i = 0; i < 128; i++) {
			if (arablist[i] == 0) break;
			if (arablist[i] == ch) return ch;
		}
		return FALSE;
	}
	
	INT8U Convert_ArabStrLength(INT8U *str)
	{
		INT8U i, arabCombCnt, leng;

		arabCombCnt=0;

		leng = strlen((char *)str);

		for(i=0;i<leng;i++)
		{
			if(str[i]==0xE1) 
			{
				if(str[i+1]==0xC2 || str[i+1]==0xC3 || str[i+1]==0xC5 || str[i+1]==0xC7)
				{
					arabCombCnt++;
				}
			}
		}
		leng -= arabCombCnt;

		return leng;
	}

	void Convert_ArabInit(void)
	{
#ifdef USE_ENG_BASED_ARAB_SUPPORT
		eng_charPnt = -1;
		arab_charPnt = -1;
		arab_exist = 0;
		spec_exist = 0;
		num_exist = 0;
		num_count = 0;
		spec_count = 0;
		numOn = 0;
#endif
		arab_special_nch = 0;
		arab_codetemp_pnt= -1;
		arab_count   = 0;
		arab_status  = 0;
		arab_newchar = 0;
		Arab_combine_cnt = 0; //gm 080821
		arab_combine_cnt_flag = ENABLE;//gm 080823	
		eng_num = 0;
		spec_num = 0;
		specChgNum = 0;
		specSepNum = 0;
		arab_nspectemp = 0;
		arab_nnumtemp = 0;
		arab_nengtottemp = 0;
		arab_nengspectemp = 0;
		slashNum = 0;
		exceptNum = 0;
		bracketNum = 0;
		engChgEnable = 0;
		numChgEnable = 0;
		eBracketOpen = 0;
		bracketClose = 0;
		aBracketOpen = 0;
		bracketNumClose = 0;
        aBracketOpenNumber = 0;
        bracketArabIn = 0;
		arab_In = 0;
        exceptEnable = 0;
		engExceptNum = 0;
	}
#ifdef USE_ENG_BASED_ARAB_SUPPORT
	void Convert_FontAddress(int eng_charPnt)
	{
		INT8U 	hi, lo, nth;
		INT8U	cflag;		//cflag = combined flag
		//INT8U	aflag = 0;	//aflag = arab number flag
		INT16U 	i;

		for (i=0; i<eng_charPnt; i++) {
			if (eng_codetemp[i]==0) break;
			hi=eng_codetemp[i]>>8;
			cflag = hi & 0x08;

			//if (ArabNumFlag) aflag = hi & 0x10;

			hi&=0x07;
			lo=eng_codetemp[i]&0xff;
			if (cflag) { // get font address from font map in modified arabcode (range: C0 ~ DF)
				nth = IsArab(lo) - 0x80 - 0x40; 
				if (hi) eng_codetemp[i] = nth + (256+32) + 3*128 + (hi)*32; 
				else 	eng_codetemp[i] = nth + (256+32) + 3*128;			   
			} else { // get font address from fontmap in normal arab code
				nth=IsArab(lo)-0x80;
				if (hi) eng_codetemp[i] = nth + (256+32) + (hi-1)*128; 
				else
				{ 
					eng_codetemp[i] &= 0x07ff;
					//if (aflag) arab_codetemp[i] += (256 - 0x30);
				}
			}
		}
	}
#else	
	void Convert_FontAddress(int arab_codetemp_pnt)
	{
		INT8U 	hi, lo, nth;
		INT8U	cflag;		//cflag = combined flag
		//INT8U	aflag = 0;	//aflag = arab number flag
		INT16U 	i;
		
		for (i=0; i<arab_codetemp_pnt; i++) {
			if (arab_codetemp[i]==0) break;
			hi=arab_codetemp[i]>>8;
			cflag = hi & 0x08;
			
			//if (ArabNumFlag) aflag = hi & 0x10;
			
			hi&=0x07;
			lo=arab_codetemp[i]&0xff;
			if (cflag) { // get font address from font map in modified arabcode (range: C0 ~ DF)
				nth = IsArab(lo) - 0x80 - 0x40; 
				if (hi) arab_codetemp[i] = nth + (256+32) + 3*128 + (hi)*32; 
				else 	arab_codetemp[i] = nth + (256+32) + 3*128;			   
			} else { // get font address from fontmap in normal arab code
				nth=IsArab(lo)-0x80;
				if (hi) arab_codetemp[i] = nth + (256+32) + (hi-1)*128; 
				else
				{ 
					arab_codetemp[i] &= 0x07ff;
					//if (aflag) arab_codetemp[i] += (256 - 0x30);
				}
			}
		}
	}
#endif

	INT16U Change_ArabMode(INT16U ich,INT8U mode)
	//  return value: 
	//  b15 is 1, finish current arab code and arab code restart.
	//  b11 is 1, then change new char for E1Cx
	{
		INT16U	ret;
		INT8U  	nth, xch;

		xch = ich & 0xff;

		nth =IsArab((INT8U)xch)-0x80;
		if (ich&0x0800) arab_newchar=1;
		if (mode==2) {	//middle
			if ((arablist_mask[nth]&0x02)==0) {
				mode=0x83;
			}
		}
		if (mode==1) {	//start
			if ((arablist_mask[nth]&0x02)==0) {
				mode = 0x83;
			}
			if ((arablist_mask[nth]&0x01)==0) {
				mode = 0x80;
			}
		}
		if (arab_newchar) mode|=0x08;
		ret = (INT16U)mode;
		ret <<= 8;
		ret |= xch;
		return ret;
}

	INT8U Arab_CheckMode(INT8U ich)
	{
		//	if ((ich>='0') && (ich<='9')) return 1;	
		//	if (ich=='.') return 1;
		//	if ((ich>='a') && (ich<='z')) return 2;
		//	if ((ich>='A') && (ich<='Z')) return 2;
		if (IsArab(ich)) return 3;
		return 0;
	}

    INT8U CloseBracketCheck(INT8U ch)
    {
        if(ch==0x29 || ch==0x5d || ch==0x7d || ch==0x3e) return TRUE;
		else return FALSE;     
    }

    INT8U OpenBracketCheck(INT8U ch)
	{
		if(ch==0x28 || ch==0x5b || ch==0x7b || ch==0x3c) return TRUE;
		else return FALSE;
	}    
	
	INT8U BracketCheck(INT8U ch)
	{
		if(ch==0x28 || ch==0x29 || ch==0x5b || ch==0x5d || ch==0x7b || ch==0x7d || ch==0x3c || ch==0x3e) return ch;
		else return FALSE;
	}

	INT8U BracketCloseChk(INT8U open, INT8U close)
	{
		INT8U temp;

		temp = open^close;
		if(temp==0x06 || temp==0x01) return 1;	//괄호 열기, 괄호 닫기 매칭 확인
		else return 0;
	}

	INT8U BracketOpenConv(INT8U ch)
	{
		if(ch == 0x28) ch = 0x29;	// ( -> )
		else if(ch == 0x3c) ch = 0x3e;	// < -> >
		else if(ch == 0x5b) ch = 0x5d;	// [ -> ]
		else if(ch == 0x7b) ch = 0x7d;	// { -> }
		return ch;
	}

	INT8U BracketCloseConv(INT8U ch)
	{
		if(ch == 0x29) ch = 0x28;	// ) -> (
		else if(ch == 0x3e) ch = 0x3c;	// > -> <
		else if(ch == 0x5d) ch = 0x5b;	// ] -> [
		else if(ch == 0x7d) ch = 0x7b;	// } -> {
		return ch;
	}

	INT8U BracketConv(INT8U ch)
	{
		if(ch == 0x28) ch = 0x29;	// ( -> )
		else if(ch == 0x29) ch = 0x28;	// ) -> (
		else if(ch == 0x3c) ch = 0x3e;	// < -> >
		else if(ch == 0x3e) ch = 0x3c;	// > -> <
		else if(ch == 0x5b) ch = 0x5d;	// [ -> ]
		else if(ch == 0x5d) ch = 0x5b;	// ] -> [
		else if(ch == 0x7b) ch = 0x7d;	// { -> }
		else if(ch == 0x7d) ch = 0x7b;	// } -> {
		return ch;
	}

	INT8U EngCheck(INT8U ch)
	{
		if((ch >= 0x41 && ch <= 0x5a) || (ch >= 0x61 && ch <= 0x7a)) return ENGWORD;	//영문알파벳(A~Z,a~z)
		else if(ch >= 0x30 && ch <= 0x39) return NUMBER;	//숫자 (0~9)
		else if(ch>=0x23 && ch<=0x25) return NUMSIGN;	//기호(#,$,%)
		else if(ch==0x2f || ch==0x2e || ch==0x3a || ch==0x2d || ch==0x2c) return SEPSIGN;	//기호('/' '.' ':' '-' ',')
		else return 0;
	}

	INT8U SpecialCheck(INT8U ch)
	{      // 공백 ! " # $ % & ' ( ) * + , - . / : ; < = > ? @ [ \ ' ^ _ ` { | } ~
		if((ch >= 0x20 && ch <= 0x2f) || (ch >= 0x3a && ch <= 0x40) || (ch >= 0x5b && ch <= 0x60) || (ch >= 0x7b && ch <= 0x7e)) return ch;
		else return 0;
	}
	
	int Convert_ArabReverseEng(INT8U *str,int k)
	{
		INT8U cur_ch;
		int   rev_l;
		int i,j;	   
		
		j=0;

		rev_l=arab_nengtottemp;

		for (i=0; i<arab_nengtottemp; i++) 
		{
			cur_ch=str[i];
			arab_subtemp[--rev_l]=cur_ch;	//영문 경우 순서 반대로 저장	
		}
		for (i=0; i<=arab_codetemp_pnt; i++) 
		{
			if(j == arab_nengtottemp) break;
			if(arab_codetemp[i] == 0xff) 
			{
				arab_codetemp[i]=(INT16U)arab_subtemp[j++];
			}
			// added by goldman 080311
		}
	//	backup_arab_nengtemp = arab_nengtemp; //gm 080820
		arab_nengtemp=0;	
		arab_nnumtemp=0;
		return k;
	}
	
	INT8U Convert_ArabInterpret(INT8U ch,INT8U mode)
	{
		int    i,j,k,k_temp,e_temp,bracketStart, bracketOpen;
		INT16U v_16,cur_ch,specTemp,enSpectemp;
		INT8U  ret_ch,save_newchar;
		
		cur_ch = (INT16U)ch&0x00ff;
		ret_ch      =0;
		arab_newchar=0;
		
		switch (arab_status) {
			case 0:	// Arab Mode
N1:				if (IsArab((INT8U)cur_ch)) {
						arab_status = 0;
						eng_num = 0;
						spec_num = 0;
						slashNum = 0;
						engChgEnable = 0;
						numChgEnable=0;
						cursor_direct = 1; // 1: Left, 2: Right
						arab_In = 1;
//N1: 
					if (((INT8U)cur_ch==0xe1) && (mode==0)) {
						arab_status=30;
						break;
					}
X2:
                    if((INT8U)cur_ch == LF || (INT8U)cur_ch == _CR || (INT8U)cur_ch == HT || (INT8U)cur_ch == FF)// 아랍어로 취급
                    {
                        if (arab_count <= 1) // 영어 다음으로 오거나 시작 문자이거나, 앞에 아랍어가 1글자만 있는 경우. 
                        {
                            arab_count = 0;
                            arab_codetemp[++arab_codetemp_pnt] = (INT16U)(cur_ch&0x00ff); 
                        }
                        if (arab_count > 1) 
                        {
                            v_16 = Change_ArabMode(arab_codetemp[arab_codetemp_pnt],3); // change to Last,Available Character [Middle]   
                            arab_codetemp[arab_codetemp_pnt] =v_16;
                            arab_count = 0;
                            arab_codetemp[++arab_codetemp_pnt] = (INT16U)(cur_ch&0x00ff); 
                        }
                        
                    }
                    else
                    {

    					if (arab_count == 1) { // modify previous character (첫 문자 변경)
    						save_newchar=arab_newchar;
    						arab_newchar=0;
    						v_16=Change_ArabMode(arab_codetemp[arab_codetemp_pnt],1); // First  
    						arab_newchar=save_newchar;
    						if (v_16&0x8000) arab_count=0; 
    						arab_codetemp[arab_codetemp_pnt]=v_16;
    					}
    					else if(arab_count > 1)	// modify previous character (중간 문자 변경)
    					{
    						save_newchar=arab_newchar;
    						arab_newchar=0;		//이전 문자는 반영 안되도록 수정
    						v_16=Change_ArabMode(arab_codetemp[arab_codetemp_pnt],2); // Middle
    						arab_newchar=save_newchar;
    						if (v_16&0x8000) arab_count=0; 
    						arab_codetemp[arab_codetemp_pnt]=v_16;
    					}
    					arab_codetemp_pnt++;
    					// get current character
    					if (arab_count == 0) {
    						arab_codetemp[arab_codetemp_pnt] = Change_ArabMode((INT16U)cur_ch,0); // Single
    					} else {			
    						//v_16 = Change_ArabMode((INT16U)cur_ch,2); // Middle	 
    						v_16 = Change_ArabMode((INT16U)cur_ch,3); // Last
    						arab_codetemp[arab_codetemp_pnt]=v_16;
    						if (v_16&0x8000) { 
    							arab_count=0;
    							break;
    						}
    					}
    					arab_count++;
                    }
				}
				else if(EngCheck(cur_ch))	//알파벳 + 숫자 + 별도 처리 기호
				{
					arab_status=10;
					goto N2;
				}
				else {	// 일반 기호(별도 처리 기호 제외)
				/*
					if  ( (INT8U)cur_ch == HT ) {
						if (arab_count > 1) {
							v_16 = Change_ArabMode(arab_codetemp[arab_codetemp_pnt],3); // change to Last,Available Character [Middle]	 
							arab_codetemp[arab_codetemp_pnt] =v_16;
						}
						arab_count = 0;
						arab_codetemp[++arab_codetemp_pnt] = (INT16U)(cur_ch&0x00ff); 
						break;	
					} else if ( ((INT8U)cur_ch==_CR) || ((INT8U)cur_ch==LF) ) {
						if (arab_count > 1) {
							v_16 = Change_ArabMode(arab_codetemp[arab_codetemp_pnt],3); // change to Last,Available Character [Middle]	 
							arab_codetemp[arab_codetemp_pnt] =v_16;
						}
						arab_count = 0;
						arab_codetemp[++arab_codetemp_pnt] = (INT16U)(cur_ch&0x00ff); 
						break;
					} else if ((INT8U)cur_ch==DC1) {
						if (arab_count > 1) {
							v_16 = Change_ArabMode(arab_codetemp[arab_codetemp_pnt],3); // change to Last,Available Character [Middle]	 
							arab_codetemp[arab_codetemp_pnt] =v_16;
						}
						arab_special_char[arab_special_nch++]=(INT8U)cur_ch;
						arab_status++;
					} */
					if((INT8U)cur_ch == DC1)
    				{
    					if (arab_count > 1) {
    						v_16 = Change_ArabMode(arab_codetemp[arab_codetemp_pnt],3); // change to Last,Available Character [Middle]	 
    						arab_codetemp[arab_codetemp_pnt] =v_16;
    						}
    					arab_special_char[arab_special_nch++]=(INT8U)cur_ch;
    					arab_status++;
    				}
					else 
					{	// 제어문자 아닌 경우 //아랍어 뒤 공백, 특수문자는 아랍어로 처리
						if(!cur_ch) break;
						if (arab_count > 1) //기호입력 전 아랍문자 2자 이상 입력 시 처리
						{
							v_16 = Change_ArabMode(arab_codetemp[arab_codetemp_pnt],3); // 마지막 문자 종성 처리 
							arab_codetemp[arab_codetemp_pnt] =v_16;
						}
						arab_count = 0;	//아랍어 글자수 체크 초기화
						if(EngCheck(cur_ch)==SEPSIGN) slashNum++;	//별도 특수 문자(/,.,:,-) 처리 
						else slashNum = 0;
						if(EngCheck(cur_ch)==NUMSIGN) exceptNum++;  //별도 특수 문자(#,$,%) 처리 
						if(BracketCheck(cur_ch))
						{
                            if(OpenBracketCheck(cur_ch)) 
                            {
                                aBracketOpen = cur_ch;
                            }
                            bracketNum++;
                            if(bracketArabIn) //영문방향에서 입력하다가 괄호를 연 후 아랍어가 들어온 경우, 괄호 방향 유지 
                            {
                                arab_status=10;
                                goto N2;
                            }
                            else // 영문방향에서 입력하다가 아랍어 입력 없이 괄호를 입력한 경우 , 괄호 방향 뒤집음.
                                cur_ch = BracketConv(cur_ch);
						}
						arab_codetemp_pnt++;
						arab_codetemp[arab_codetemp_pnt]=(INT16U)cur_ch;
						arab_nspectemp++;
						cursor_direct = 1; // 1: Left, 2: Right
					}
				}
				break;
			case 21:
			case  1:
				arab_special_char[arab_special_nch++]=cur_ch;				
				if (((INT8U)cur_ch=='D')||((INT8U)cur_ch=='X')||((INT8U)cur_ch=='Y')||((INT8U)cur_ch=='A')||((INT8U)cur_ch=='B')) {
					arab_status++; // 2 or 12
				}
				//else if (((INT8U)cur_ch == HT) || ((INT8U)cur_ch == CR) ||((INT8U)cur_ch == LF )) {
				//	arab_codetemp[++arab_codetemp_pnt] = (INT16U)(cur_ch&0x00ff); 					
				//	arab_status = 0;
				//} 
				else {
					arab_status+=2;  // 3 or 13
				}
				break;
			case 22:
			case  2:
				arab_special_char[arab_special_nch++]=(INT8U)cur_ch; // <DC1>A1
				arab_status++;		// 3 or 13
				//if (((INT8U)cur_ch == HT) || ((INT8U)cur_ch == CR) ||((INT8U)cur_ch == LF )) {
				//	arab_codetemp[++arab_codetemp_pnt] = (INT16U)(cur_ch&0x00ff); 					
				//	arab_status = 0;
				//} 
				//else {
				//	arab_special_char[arab_special_nch++]=cur_ch;
				//	arab_status++;  // 3 or 13
				//}
				break;
			case 23:
			case  3:
				if (IsArab((INT8U)cur_ch)) {
					if ( ((INT8U)cur_ch==0xe1) && (mode==0) ) {
						arab_status=30;
						break;
					}
					if (arab_status<10) { // before arabmode, now no change mode
						for (j=0; j<arab_special_nch; j++) arab_codetemp[++arab_codetemp_pnt]=(INT16U)arab_special_char[j];
					} else {		 // before other mode, now change arabmode
						k_temp=arab_codetemp_pnt;
						if (arab_nengtemp) arab_codetemp_pnt=Convert_ArabReverseEng(arab_engtemp,k_temp);  
						else {
							for (j=0; j<arab_special_nch; j++) arab_codetemp[++arab_codetemp_pnt]=(INT16U)arab_special_char[j]; //<DC1>X1 + <arab>
						}
					}
					arab_count = 0;
					arab_status = 0;
					goto N1;
				} else {
					if (arab_status<10) { // before arabmode, now,change other mode
						if  (((INT8U)cur_ch==HT) || ((INT8U)cur_ch==_CR) || ((INT8U)cur_ch==LF))  {		
							if (arab_count > 1) {
								v_16 = Change_ArabMode(arab_codetemp[arab_codetemp_pnt],3); // change to Last,Available Character [Middle]	 
								arab_codetemp[arab_codetemp_pnt] =v_16;
							}
							arab_count=0;
							for (j=0; j<arab_special_nch; j++) arab_codetemp[++arab_codetemp_pnt]=(INT16U)arab_special_char[j];
							arab_status=0;
							//break;
						} else if ((INT8U)cur_ch==DC1) {
							if (arab_count > 1) {
								v_16 = Change_ArabMode(arab_codetemp[arab_codetemp_pnt],3); // change to Last,Available Character [Middle]	 
								arab_codetemp[arab_codetemp_pnt] =v_16;
							}
							arab_count=0;
							for (j=0; j<arab_special_nch; j++) arab_codetemp[++arab_codetemp_pnt]=(INT16U)arab_special_char[j];
							arab_special_nch=0;
							arab_special_char[arab_special_nch++]=(INT8U)cur_ch;
							arab_status-=2;	// next step, arab_status=1,21;
						} else {	// 영문 or 특수문자 (합성)
							arab_nengtemp=0;
							for (j=0; j<arab_special_nch; j++) arab_engtemp[arab_nengtemp++]=arab_special_char[j];
							arab_engtemp[arab_nengtemp++]=(INT8U)cur_ch;
							if(EngCheck(cur_ch))	//<DC1>+D1,X1,Y1,A1,B1(Print Command) 이후 문자 입력(알파벳, 숫자, 별도 처리 기호)
							{
								arab_nengtottemp=arab_nengtemp+arab_nnumtemp+arab_nengspectemp;
								arab_codetemp_pnt++;
								arab_codetemp[arab_codetemp_pnt]=(INT16U)0xff;  //아랍어X 문자 -> FF로 저장
								if(arab_nengtottemp>1) eng_num = arab_nengtottemp;
								cursor_direct = 2;
								arab_status=20;
							}
							else if(SpecialCheck(cur_ch))	//<DC1>+D1,X1,Y1,A1,B1(Print Command) 이후 별도처리기호 외 일반기호)
							{
								goto SPEC;
							}
							else arab_status=10;
						}
					} else {		// before other mode, now no change mode		//영문 or 특수문자 (합성X)
						for (j=0; j<arab_special_nch; j++) arab_engtemp[arab_nengtemp++]=arab_special_char[j];
						arab_engtemp[arab_nengtemp++]=(INT8U)cur_ch;
						arab_status=20;
					}
				}
				break;
			case 10: // Start Other Mode 
				N2:			if (arab_count > 1) { //이전 문자 2자 이상 아랍어 경우
					v_16 = Change_ArabMode(arab_codetemp[arab_codetemp_pnt],3); // change to Last,Available Character [Middle]	 
					arab_codetemp[arab_codetemp_pnt] =v_16;
				}
				arab_count  = 0;
				arab_status   =20;
				arab_nsubtemp = 0;
			case 20: // Other Mode
				k_temp=arab_codetemp_pnt;
				arab_nengtottemp=arab_nengtemp+arab_nnumtemp+arab_nengspectemp;
				if (IsArab((INT8U)cur_ch) || (!specChgNum && aBracketOpen && BracketCheck(cur_ch) && BracketCloseChk(aBracketOpen,cur_ch))
					|| (!EngCheck(cur_ch) && SpecialCheck(cur_ch) && arab_nnumtemp && !arab_nengtemp)//)  //영문 + 히브리어 경우 -> 영문 뒤집어 저장 (영문 뒤 특수문자, 공백은 영문으로 처리)
                    || (bracketClose && !bracketNumClose && BracketCheck(cur_ch) && !aBracketOpenNumber && !bracketArabIn) // 영문방향에서 ( -> ( -> ) -> ), 영문-> 괄호 두번 열고 -> 숫자 -> 괄호 닫고 -> 아랍어 입력시 loof 방지
                    || (/*bracketClose &&*/ !bracketNumClose && BracketCheck(cur_ch) && eBracketOpen && !BracketCloseChk(eBracketOpen,cur_ch))) // 영문방향에서 ( -> ( -> ) -> ( 
                {
                   if(BracketCheck(cur_ch)&&eBracketOpen&&/*bracketClose &&*/ !bracketNumClose) // 영문방향에서 괄호 두 번 닫고 다시 연 상태
                        goto SPEC;

					if(eBracketOpen && IsArab((INT8U)cur_ch)) bracketArabIn =1;
					if ((arab_nengtemp || arab_nnumtemp)&&!bracketArabIn) {  //영문 입력 후 아랍어 입력 시 (기존 영문 변환)
						arab_codetemp_pnt=Convert_ArabReverseEng(arab_engtemp,k_temp);
					}
					//arab_status=0;
					arab_count = 0;
					arab_nspectemp=0;
                    //if(!bracketArabIn && !eBracketOpen){
                    if(!bracketArabIn)
                    {
                        arab_nengtemp=0;
    					arab_nnumtemp=0;
    					arab_nengspectemp=0;
    					arab_nengtottemp=0;
                    }
					if(aBracketOpen) aBracketOpen=0;
					eng_num = 0;
					spec_num = 0;
					slashNum = 0;
					numChgEnable = 0;
                    //if(!eBracketOpen){
                        specChgNum = 0;
					    specSepNum = 0;
                        engChgEnable = 0;
                    //}
					arab_In = 1;
					cursor_direct = 1; // 1: Left, 2: Right
					goto N1;				
				}
				else if((EngCheck(cur_ch)>0 && EngCheck(cur_ch)<3) || //영문알파벳 및 숫자 구분 ( #, $, % 문자 경우 별도 처리)
						(!arab_In && EngCheck(cur_ch)==NUMSIGN && EngCheck(arab_engtemp[arab_nengtottemp-1])==NUMBER)||	//현재 문자 '예외문자' 이고  바로 전 문자 '숫자'(아랍어 입력X)
						(!arab_In && exceptEnable && EngCheck(cur_ch)==NUMSIGN)|| //숫자 뒤에 '#, $, %' 가 연달아 온 경우(아랍어 입력 없는 경우만)
						(eBracketOpen && (cur_ch != 0x20) && arab_nengtemp))  	//'괄호열기'상태이고 현재 문자 '공백X'이고 '영문' 있는 경우
                {
                    if(EngCheck(cur_ch)==NUMSIGN && EngCheck(arab_engtemp[arab_nengtottemp-1])==NUMBER) exceptEnable = TRUE;
                    else if(EngCheck(cur_ch)!=NUMSIGN) exceptEnable = FALSE;
                    
					if(specChgNum || specSepNum || eBracketOpen || bracketClose)		//숫자 or 알파벳 뒤 특수기호 포함된 경우 or 별도 특수문자(# $ %) 경우
					{                        
						if((specSepNum && !exceptNum && slashNum!=1 && !eBracketOpen && !engChgEnable) ||	//별도 특수 문자(# $ %) 없고 '/' 1번 입력아니고 영문괄호없고 이전문자 영문X 경우
							(specSepNum && !exceptNum && slashNum==1 && !eBracketOpen && !engChgEnable && (EngCheck(cur_ch)==ENGWORD || EngCheck(cur_ch)==NUMSIGN)))
						{
							if(arab_nnumtemp && (EngCheck(cur_ch)>0 && EngCheck(cur_ch)<4))
							{
								arab_codetemp_pnt=Convert_ArabReverseEng(arab_engtemp,k_temp);
								arab_nengtemp=0;
								arab_nspectemp=0;
								arab_nnumtemp=0;
								arab_nengspectemp=0;
								arab_nengtottemp=0;
								eng_num = 0;
								spec_num = 0;
								engChgEnable = 0;
								numChgEnable = 0;
								specChgNum = 0;
								specSepNum = 0;
								cursor_direct = 2; // 1: Left, 2: Right
							}
						}
						else
						{	//특수기호 뒤 숫자나 영문 올 경우
							if(specSepNum && ((exceptNum && EngCheck(cur_ch)==NUMBER) ||	//이전 문자 '% $ #' 이고 숫자 입력인 경우
								(slashNum==1 && EngCheck(cur_ch)==NUMBER && EngCheck(arab_codetemp[k_temp])==SEPSIGN)))	//바로 이전 문자 '/ . : -' 이고 숫자 입력인 경우
							{ 
								enSpectemp = specSepNum;
								if(slashNum==1) slashNum = 0;
							}
                            else if	(specChgNum && (arab_nengtemp && /*!eBracketOpen&& */ engChgEnable))	//이전 문자 '영문' 인 경우
                            {
                                // A -> B -> ( -> ( -> 1 -> 2 -> ) -> ( -> 1 -> 2 -> )
                                if((!bracketClose && !bracketArabIn) || (eBracketOpen && (aBracketOpenNumber >= 1) && !bracketArabIn))
								    enSpectemp = specChgNum;
                                else
                                    enSpectemp = 0;
                            }
							else enSpectemp = 0;

							if(enSpectemp)
							{
								specTemp = k_temp - enSpectemp + 1;

								for(i=enSpectemp; i>0; i--)
								{
                                    if(!eBracketOpen && BracketCheck((INT8U)arab_codetemp[specTemp])) 
                                    {
                                        eBracketOpen = BracketConv((INT8U)arab_codetemp[specTemp]);
                                        //eBracketOpenNumber++;
                                    }
									arab_engtemp[arab_nengtottemp] = BracketConv((INT8U)arab_codetemp[specTemp]);
									if(EngCheck(arab_engtemp[arab_nengtottemp])==ENGWORD) arab_nengtemp++;
									else if(EngCheck(arab_engtemp[arab_nengtottemp])==NUMBER) arab_nnumtemp++;
									else if(SpecialCheck(arab_engtemp[arab_nengtottemp])) arab_nengspectemp++;	//별도 특수 문자(( ) [ ] { } < >) 처리 
									arab_nengtottemp=arab_nengtemp+arab_nnumtemp+arab_nengspectemp;
									arab_codetemp[specTemp++] = (INT16U)0xff;
								}
								spec_num = specChgNum;	//문자 위치 변경되어야 할 특수문자 갯수
								arab_nspectemp=0;	//임시 수정 ("IS6040-00010(60*40)" 2번 변환 시 개수 안 맞음)
								specChgNum=0;
								specSepNum=0;
								if(eBracketOpen) aBracketOpen=0;
								cursor_direct = 2;
								//slashNum = 0;
							}
						}
					}
                    if((BracketCheck(cur_ch) && !BracketCloseChk(eBracketOpen,cur_ch)) || // 현재 괄호 열린 상태이고 괄호가 닫히지 않은 경우
                       (bracketArabIn && BracketCheck(cur_ch) && BracketCloseChk(eBracketOpen,cur_ch) && (aBracketOpenNumber != 1)))//영문방향에서 괄호가 두 번 이상  열린 상태인데 아랍어부분만 괄호로 닫힌 경우
                        goto SPEC;
                    if(!bracketArabIn && BracketCheck(cur_ch) && BracketCloseChk(eBracketOpen,cur_ch)) 	//알파벳 방향 괄호 닫기 처리 ABC(123 + '(' => ABC(123)
                    {
                        //cur_ch = BracketConv(cur_ch);1125
                        
						bracketClose=1;
						bracketNumClose=0;
                        if(aBracketOpenNumber) 
                            aBracketOpenNumber--;
                        if(!aBracketOpenNumber)                         
                            eBracketOpen=0;
                    }
                    if(bracketArabIn && BracketCheck(cur_ch) && BracketCloseChk(eBracketOpen,cur_ch)) // <-git 커밋 11.11 15:00
                    {
                        bracketStart = 0;//처음 괄호 시작 위치 
                        bracketOpen = 0;//마지막 괄호 시작 위치 
						for(i=0; i<arab_nengtottemp; i++) 
						{
							if(bracketOpen) 
							{
								//if(EngCheck(arab_engtemp[i])==ENGWORD ) { bracketStart=0; break; } //이미 다른데서 변환처리 해서 변환 필요 없음.
							}
							//if(arab_engtemp[i]==BracketConv(eBracketOpen))//1125
							if(arab_engtemp[i]==eBracketOpen)
                            {
                                if(!bracketStart) bracketStart=i;// 처음 괄호 열린 위치 저장                      
                                bracketOpen=i;//마지막 괄호 열린 위치
							}
						}
						if(arab_nengtemp && bracketStart) 
						{
							e_temp = arab_nengtottemp-1;
							j=1;
                            k=0;
							for(i=e_temp; i>=0; i--) 
							{
                                if(i>bracketStart) 
								{
									arab_codetemp[i]=arab_engtemp[bracketStart+j];
									if(EngCheck(arab_engtemp[bracketStart+j])==ENGWORD) arab_nengtemp--;
									else if(EngCheck(arab_engtemp[bracketStart+j])==NUMBER) arab_nnumtemp--;
									else if(SpecialCheck(arab_engtemp[bracketStart+j])) arab_nengspectemp--;	//별도 특수 문자(( ) [ ] { } < >) 처리 
									arab_nengtottemp=arab_nengtemp+arab_nnumtemp+arab_nengspectemp;
									arab_engtemp[bracketStart+j] = 0;
									j++;
								}
								else 
								{
									if(BracketCheck(arab_engtemp[i])) 
									{
										arab_codetemp[i]=BracketOpenConv(arab_engtemp[i]);
										if(SpecialCheck(arab_engtemp[i])) arab_nengspectemp--;	//별도 특수 문자(( ) [ ] { } < >) 처리 
										arab_nengtottemp=arab_nengtemp+arab_nnumtemp+arab_nengspectemp;
										arab_engtemp[i] = 0;
									}
                                    else if(SpecialCheck(arab_engtemp[i]) || EngCheck(arab_engtemp[i]))
                                    {
                                        arab_codetemp[k++]=arab_engtemp[i];
										if(EngCheck(arab_engtemp[i])==ENGWORD) arab_nengtemp--;
										else if(EngCheck(arab_engtemp[i])==NUMBER) arab_nnumtemp--;
										arab_nengtottemp=arab_nengtemp+arab_nnumtemp+arab_nengspectemp;
										arab_engtemp[i] = 0;
                                    }
                                    else break;
								}
							}
							//cur_ch = BracketCloseConv(cur_ch);
							eBracketOpen=0;
							bracketClose=1;
							bracketNumClose=1;
                            bracketArabIn=0;
							goto SPEC;
						}
						//cur_ch = BracketOpenConv(cur_ch);	//영문 괄호 닫기 변환
						eBracketOpen=0;
						bracketClose=1;
						bracketNumClose=0;
                        bracketArabIn=0;
                    }
                    arab_engtemp[arab_nengtottemp]=(INT8U)cur_ch;
					if(EngCheck(cur_ch)==ENGWORD) arab_nengtemp++;	//영문 문자 처리
					else if(EngCheck(cur_ch)==NUMBER) arab_nnumtemp++;	//숫자 처리
					else if(SpecialCheck(cur_ch)) arab_nengspectemp++;	//그 외 특수문자 처리 
					arab_nengtottemp=arab_nengtemp+arab_nnumtemp+arab_nengspectemp;
					arab_codetemp_pnt++;
					arab_codetemp[arab_codetemp_pnt]=(INT16U)0xff;  //아랍어X 문자 -> FF로 저장
					if(EngCheck(cur_ch)!=SEPSIGN) slashNum=0;
					if(EngCheck(cur_ch)==NUMSIGN) engExceptNum=1;	// %, $, # 입력 경우
					else engExceptNum=0;
					if(arab_nengtottemp>1) eng_num = arab_nengtottemp;
					cursor_direct = 2;	// 1: Left, 2: Right
				}
				else
				{
					if(!cur_ch) break;
	SPEC:			if(cur_ch!=0x20) cursor_direct = 1; //입력 안 된 공간은 빈 문자로 처리되므로 다른 특수기호만 방향 지정
					if(!engExceptNum && EngCheck(cur_ch)==SEPSIGN) slashNum++; //이전문자 숫자+(%,$,#) 조합 아니고 현재 문자 /,.,:,- 인 경우
					else slashNum = 0;
					if(!arab_nengtemp && !slashNum && EngCheck(cur_ch)==NUMSIGN) exceptNum++;  //이전문자 영문+(/ . : -) 조합 아니고 현재 문자 # $ % 인 경우
					else exceptNum = 0;
					if(BracketCheck(cur_ch))
					{
                        if(OpenBracketCheck(cur_ch)) 
                        {
                            aBracketOpen = cur_ch;
                            //cur_ch = BracketConv(cur_ch);//1125
                            aBracketOpenNumber++;
                        }
                        else if(CloseBracketCheck(cur_ch))
                        {
                            aBracketOpenNumber--;
                        }
                        cur_ch = BracketConv(cur_ch);//1125
						bracketNum++;
					}
					arab_codetemp_pnt++;
					arab_codetemp[arab_codetemp_pnt]=(INT16U)cur_ch;
					if(arab_nnumtemp || (!arab_In && exceptNum)) numChgEnable = 1;	//이전 문자 숫자 & 이전 #, $, % 입력 없음 or 이전 아랍문자X, 현재 #, $, % 인 경우
					if(numChgEnable) specSepNum++;	// #, $, % 개수
					//eBracketOpen=0;	//괄호 사이에 기호 경우 괄호 열기 초기화되는 문제
					if(arab_nengtemp) engChgEnable = 1;		// 영문 입력 상태
					if(engChgEnable)	specChgNum++;		// 영문 뒤 특수기호
					arab_nspectemp++;
					engExceptNum = 0;
				}
				break;
			case 30:
				if (((INT8U)cur_ch==0xc2) || ((INT8U)cur_ch==0xc3) || ((INT8U)cur_ch==0xc5) || ((INT8U)cur_ch==0xc7)) {	//중간문자 X
					arab_newchar=1;
					arab_status =0;
					if (arab_combine_cnt_flag == ENABLE) Arab_combine_cnt++;//gm 080821
					goto N1;
				} else {				
					arab_status = 0;
					ret_ch=(INT8U)cur_ch;
					cur_ch=(INT16U)0xe1;				
					goto X2;
				}
		} // end switch
		return ret_ch;
	}

	INT8U Convert_ArabInterpretExcept(INT8U ch,INT8U mode)	//IP Mode에서만 사용(알파벳 입력 시 숫자처럼 처리)
	{
		int    i,j,k_temp,e_temp,bracketStart;
		INT16U v_16,cur_ch,specTemp,enSpectemp;
		INT8U  ret_ch,save_newchar;

		cur_ch = (INT16U)ch&0x00ff;
		ret_ch      =0;
		arab_newchar=0;

		switch (arab_status) {
			case 0:	// Arab Mode
	N1:			
	X2:
				if(EngCheck(cur_ch))
				{
					arab_status=10;
					goto N2;
				}
				else 
				{	// 아랍어 아닌 경우 (영문 or 특수문자)
					if(!cur_ch) break;
					if(EngCheck(cur_ch)==SEPSIGN) slashNum++;	//별도 특수 문자(/,.,:,-) 처리 
					else slashNum = 0;
					if(EngCheck(cur_ch)==NUMSIGN) exceptNum++;  //별도 특수 문자(#,$,%) 처리 
					if(BracketCheck(cur_ch))
					{
						aBracketOpen = cur_ch;
						cur_ch = BracketConv(cur_ch);
						bracketNum++;
					}
					arab_codetemp_pnt++;
					arab_codetemp[arab_codetemp_pnt]=(INT16U)cur_ch;
					arab_nspectemp++;
					cursor_direct = 1; // 1: Left, 2: Right
				}
				break;
			case 10: // Start Other Mode 
	N2:			if (arab_count > 1) { //이전 문자 2자 이상 아랍어 경우
					v_16 = Change_ArabMode(arab_codetemp[arab_codetemp_pnt],3); // change to Last,Available Character [Middle]	 
					arab_codetemp[arab_codetemp_pnt] =v_16;
				}
				arab_count  = 0;
				arab_status   =20;
				arab_nsubtemp = 0;
			case 20: // Other Mode
				k_temp=arab_codetemp_pnt;
				arab_nengtottemp=arab_nengtemp+arab_nnumtemp+arab_nengspectemp;

				if((EngCheck(cur_ch)>0 && EngCheck(cur_ch)<3) || //영문알파벳 및 숫자 구분 ( #, $, % 문자 경우 별도 처리)
						(EngCheck(cur_ch)==NUMSIGN && EngCheck(arab_engtemp[arab_nengtottemp-1])==NUMBER)||	//현재 문자 '예외문자' 이고  바로 전 문자 '숫자'
						(eBracketOpen && (cur_ch != 0x20) && arab_nengtemp)) 	//'괄호열기'상태이고 현재 문자 '공백X'이고 '영문' 있는 경우
				{
					if(specChgNum || specSepNum || eBracketOpen || bracketClose)		//숫자 or 알파벳 뒤 특수기호 포함된 경우 or 별도 특수문자(# $ %) 경우
					{
						if(specSepNum && ((exceptNum && EngCheck(cur_ch)==NUMBER) ||	//이전 문자 '% $ #' 이고 숫자 입력인 경우
							(slashNum==1 && (EngCheck(cur_ch)==NUMBER || EngCheck(cur_ch)==ENGWORD) && EngCheck(arab_codetemp[k_temp])==SEPSIGN)))	//바로 이전 문자 '/ . : -' 이고 숫자 입력인 경우
						{ 
							enSpectemp = specSepNum;
							if(slashNum==1) slashNum = 0;
						} 
						else enSpectemp = 0;

						if(enSpectemp)
						{
							specTemp = k_temp - enSpectemp + 1;

							for(i=enSpectemp; i>0; i--)
							{
								if(!eBracketOpen && BracketCheck((INT8U)arab_codetemp[specTemp])) eBracketOpen = BracketConv((INT8U)arab_codetemp[specTemp]);
								arab_engtemp[arab_nengtottemp] = BracketConv((INT8U)arab_codetemp[specTemp]);
								if(EngCheck(arab_engtemp[arab_nengtottemp])==ENGWORD) arab_nengtemp++;
								else if(EngCheck(arab_engtemp[arab_nengtottemp])==NUMBER) arab_nnumtemp++;
								else if(SpecialCheck(arab_engtemp[arab_nengtottemp])) arab_nengspectemp++;	//별도 특수 문자(( ) [ ] { } < >) 처리 
								arab_nengtottemp=arab_nengtemp+arab_nnumtemp+arab_nengspectemp;
								arab_codetemp[specTemp++] = (INT16U)0xff;
							}
							spec_num = specChgNum;	//문자 위치 변경되어야 할 특수문자 갯수
							arab_nspectemp=0;	//임시 수정 ("IS6040-00010(60*40)" 2번 변환 시 개수 안 맞음)
							specChgNum=0;
							specSepNum=0;
							if(eBracketOpen) aBracketOpen=0;
							cursor_direct = 2;
							//slashNum = 0;
						}
						else if(arab_nengtemp && bracketClose) 	//영문방향 괄호 닫기 이후 영문 입력 시 처리
						{

							for(i=k_temp+1; i>0; i--)
							{
								if(arab_codetemp[k_temp]==0xff) break;
								arab_engtemp[k_temp]=BracketConv((INT8U)arab_codetemp[k_temp]);
								if(EngCheck(arab_engtemp[k_temp])==ENGWORD) arab_nengtemp++;
								else if(EngCheck(arab_engtemp[k_temp])==NUMBER) arab_nnumtemp++;
								else if(SpecialCheck(arab_engtemp[k_temp])) arab_nengspectemp++;	//별도 특수 문자(( ) [ ] { } < >) 처리 
								arab_nengtottemp=arab_nengtemp+arab_nnumtemp+arab_nengspectemp;
								arab_codetemp[k_temp--] = (INT16U)0xff;					
							}
							bracketClose=0;
							bracketNumClose=0;
						}
					}
					if(BracketCheck(cur_ch) && !BracketCloseChk(eBracketOpen,cur_ch)) goto SPEC;	
					if(BracketCheck(cur_ch) && BracketCloseChk(eBracketOpen,cur_ch)) 	//히브리어 방향 괄호 닫기 처리
					{
						bracketStart = 0;
						for(i=0; i<arab_nengtottemp; i++) 
						{
							if(bracketStart) 
							{
								if(EngCheck(arab_engtemp[i])==ENGWORD) { bracketStart=0; break; }		//조건 추가 필요 (괄호 안 히브리어 + 숫자 + 영문 경우 ) 
							}
							if(arab_engtemp[i]==eBracketOpen) bracketStart=i;
						}
						if(arab_nengtemp && bracketStart) 
						{
							e_temp = arab_nengtottemp-1;
							j=1;
							for(i=e_temp; i>0; i--) 
							{
								if(i>bracketStart) 
								{
									arab_codetemp[k_temp--]=arab_engtemp[bracketStart+j];
									if(EngCheck(arab_engtemp[bracketStart+j])==ENGWORD) arab_nengtemp--;
									else if(EngCheck(arab_engtemp[bracketStart+j])==NUMBER) arab_nnumtemp--;
									else if(SpecialCheck(arab_engtemp[bracketStart+j])) arab_nengspectemp--;	//별도 특수 문자(( ) [ ] { } < >) 처리 
									arab_nengtottemp=arab_nengtemp+arab_nnumtemp+arab_nengspectemp;
									arab_engtemp[bracketStart+j] = 0;
									j++;
								}
								else 
								{
									if(SpecialCheck(arab_engtemp[i])) 
									{
										arab_codetemp[k_temp--]=BracketOpenConv(arab_engtemp[i]);
										if(EngCheck(arab_engtemp[i])==ENGWORD) arab_nengtemp--;
										else if(EngCheck(arab_engtemp[i])==NUMBER) arab_nnumtemp--;
										else if(SpecialCheck(arab_engtemp[i])) arab_nengspectemp--;	//별도 특수 문자(( ) [ ] { } < >) 처리 
										arab_nengtottemp=arab_nengtemp+arab_nnumtemp+arab_nengspectemp;
										arab_engtemp[i] = 0;
									} else break;
								}
							}
							cur_ch = BracketCloseConv(cur_ch);
							eBracketOpen=0;
							bracketClose=1;
							bracketNumClose=1;
							goto SPEC;
						}
						cur_ch = BracketOpenConv(cur_ch);	//영문 괄호 닫기 변환
						eBracketOpen=0;
						bracketClose=1;
						bracketNumClose=0;
					}
					arab_engtemp[arab_nengtottemp]=(INT8U)cur_ch;
					if(EngCheck(cur_ch)==ENGWORD) arab_nengtemp++;	//영문 문자 처리
					else if(EngCheck(cur_ch)==NUMBER) arab_nnumtemp++;	//숫자 처리
					else if(SpecialCheck(cur_ch)) arab_nengspectemp++;	//그 외 특수문자 처리 
					arab_nengtottemp=arab_nengtemp+arab_nnumtemp+arab_nengspectemp;
					arab_codetemp_pnt++;
					arab_codetemp[arab_codetemp_pnt]=(INT16U)0xff;  //아랍어X 문자 -> FF로 저장

					if(arab_nengtottemp>1) eng_num = arab_nengtottemp;
					cursor_direct = 2;	// 1: Left, 2: Right
				}
				else
				{
					if(!cur_ch) break;
	SPEC:			if(cur_ch!=0x20) cursor_direct = 1; //입력 안 된 공간은 빈 문자로 처리되므로 다른 특수기호만 방향 지정
					if(EngCheck(cur_ch)==SEPSIGN) slashNum++; //별도 특수 문자(/ . : -) 처리 
					else slashNum = 0;
					if(EngCheck(cur_ch)==NUMSIGN) exceptNum++;  //별도 특수 문자(# $ %) 처리 
					if(BracketCheck(cur_ch))
					{
						if(!bracketNumClose)
						{
							aBracketOpen = cur_ch;
							cur_ch = BracketConv(cur_ch);
						}
						else bracketNumClose=0;
						bracketNum++;
					}
					arab_codetemp_pnt++;
					arab_codetemp[arab_codetemp_pnt]=(INT16U)cur_ch;
					if(arab_nengtemp || arab_nnumtemp || (!arab_In && exceptNum)) numChgEnable = 1;	//이전 문자 숫자 or 이전 아랍문자 아니고 현재 별도 특수기호인 경우
					if(numChgEnable) specSepNum++;
					//eBracketOpen=0;	//괄호 사이에 기호 경우 괄호 열기 초기화되는 문제
					//if(arab_nengtemp) engChgEnable = 1;
					//if(engChgEnable)	specChgNum++;
					arab_nspectemp++;
				}
				break;
		} // end switch
		return ret_ch;
	}
#ifdef USE_ENG_BASED_ARAB_SUPPORT
	int Convert_EngReverseArab(INT16U *str,int k)
	{
		INT16U cur_ch;
		int   rev_l;
		int i,j;	   
	
		j=0;
	
		rev_l=arab_charPnt+1;
		
		for (i=0; i<arab_charPnt+1; i++) 
		{
			cur_ch=str[i];
			arab_subtemp[--rev_l]=cur_ch;	//영문 경우 순서 반대로 저장	
		}
		for (i=0; i<=k; i++) 
		{
			if(j == arab_charPnt+1) break;
			if(eng_codetemp[i] == 0xff) 
			{
				eng_codetemp[i]=arab_subtemp[j++];
			}
		}
		arab_charPnt=-1;	
		return k;
	}
	
	INT8U Convert_EngArabInterpret(INT8U ch,INT8U mode) //IP Mode에서만 사용(알파벳 입력 시 숫자처럼 처리)
	{
		int    i,j,cntTemp,a_temp,n_temp,k_temp,s_temp,bracketStart,cnt,specCount,insertCount, specE;
		INT16U arabConv,cur_ch,specTemp,enSpectemp;
		INT8U  ret_ch,save_newchar,specOn, numEn, specEn;
		
		cur_ch = (INT16U)ch&0x00ff;
		ret_ch		=0;
		arab_newchar=0;
	
		while(1)
		{
			switch (arab_status) 
			{
				case 0: // 아랍어 체크 모드 	
					if(IsArab((INT8U)cur_ch))	// 아랍어인 경우
					{
						num_exist=0;
						if(arab_exist && spec_exist) //'아랍어 존재' 하고 'LEFT 방향' 문자입력 경우
						{
							arab_status=1;	//'아랍어' 이동 모드
							break;
						}
						if (((INT8U)cur_ch==0xe1) && (mode==0))
						{
							ret_ch=(INT8U)cur_ch;
							arab_status=3;	//아랍어 변환 모드
							return ret_ch;
						}
						else if(((INT8U)cur_ch==0xc2) || ((INT8U)cur_ch==0xc3) || ((INT8U)cur_ch==0xc5) || ((INT8U)cur_ch==0xc7))
						{
							if(arab_revtemp[arab_charPnt]==0xe1)	//특수합성문자(0xE1 + '0xC2' or '0xC3' or '0xC5' or '0xC7')
							{
								arab_newchar=1;
								if (arab_combine_cnt_flag == 1) Arab_combine_cnt++;//gm 080821
								arab_status=3;	//아랍어 변환 모드
								break;
							}
						}
						arab_status=3;	//아랍어 변환 모드
						break;
					}
					else		// 아랍어 아닌 경우 (영어/숫자/기호)
					{	
						if(!cur_ch) return 0;	//문자 입력 아닌 경우
						if(arab_exist)		//아랍어 입력 존재하는 경우
						{
							if(EngCheck((INT8U)cur_ch)==ENGWORD)	//현재 문자 '영어' 경우
							{
								arab_exist=0;		//영문 입력 시 초기화
								spec_exist=0;
								num_exist=0;
								num_count=0;
								spec_count=0;
								arab_status=4;	//아랍어 변환 모드
								numOn=0;
								break;
							}
							else if(EngCheck((INT8U)cur_ch)==NUMBER)	//현재 문자 '숫자' 경우
							{
								if(EngCheck((INT8U)eng_codetemp[eng_charPnt]==NUMBER)) //|| IsArab((INT8U)arab_revtemp[arab_charPnt]))	//이전 문자 '숫자' or '아랍어' 경우
								{
									arab_status=2;	//'숫자' 및 '기호' 이동 모드
									break;
								}
								else	//이전 문자 '기호' 경우
								{
									arab_status=1;	//'아랍어','숫자' 좌측 이동 모드
									break;
								}
							}
							else	//현재 문자 '기호' 경우
							{
								arab_status=5;	//문자 저장 모드
								break;
							}
						}
						else
						{
							arab_status=5;	//아랍어 입력 존재X 경우 -> 문자 저장 모드
							break;
						}
						arab_count=0;	//아랍어 합성 초기화
					}
				case 1: //'아랍어','숫자' 좌측 이동 모드
					specCount=0;
					a_temp=0;
					insertCount=0;
					specOn=0;
					numEn=0;
					specEn=0;
					n_temp=0;
					s_temp=0;
					if(num_count) n_temp=num_count-1;
					if(spec_count) s_temp=spec_count;
					for (i=eng_charPnt;i>=0;i--)
					{
						if(num_count && num_exist && EngCheck((INT8U)eng_codetemp[i])==NUMBER) numEn=1;
						if(spec_count && spec_exist && SpecialCheck((INT8U)eng_codetemp[i])) specEn=1;
						if(numEn && EngCheck((INT8U)eng_codetemp[i])!=NUMBER)
						{
							n_temp--;
							numEn=0;
						}
						else if(specEn && !IsArab((INT8U)eng_codetemp[i]) && !SpecialCheck((INT8U)eng_codetemp[i]))
						{
							s_temp--;
							specEn=0;
						}
						if(EngCheck((INT8U)eng_codetemp[i])==ENGWORD || (!n_temp && num_exist && EngCheck((INT8U)eng_codetemp[i])==NUMBER) || 
							(!spec_exist && SpecialCheck((INT8U)eng_codetemp[i])) || 
							(!s_temp && (spec_count || EngCheck((INT8U)cur_ch)==NUMBER) && spec_exist && SpecialCheck((INT8U)eng_codetemp[i])))
						{
							arab_status=2;
							break;
						}
						else
						{
							if(!specOn)
							{
								if(IsArab((INT8U)eng_codetemp[i]) || eng_codetemp[i]==0xFF)
								{
									specCount = a_temp;
									specOn = 1;
								}
								else if(SpecialCheck((INT8U)eng_codetemp[i])) 
								{
									if(!specCount) a_temp++;	//이전 문자 '기호' 경우
								}
							}
						}
						insertCount++;
					}	
					arab_status=2;
				case 2: //'숫자' 및 '기호' 이동 모드
					if(((INT8U)cur_ch==0xc2) || ((INT8U)cur_ch==0xc3) || ((INT8U)cur_ch==0xc5) || ((INT8U)cur_ch==0xc7))
					{
						if(arab_revtemp[arab_charPnt]==0xe1)	//특수합성문자(0xE1 + '0xC2' or '0xC3' or '0xC5' or '0xC7')
						{
							arab_newchar=1;
							if (arab_combine_cnt_flag == 1) Arab_combine_cnt++;//gm 080821
							arab_status=3;	//아랍어 변환 모드
							break;
						}
					}
					if(SpecialCheck((INT8U)eng_codetemp[eng_charPnt]))
					{
						s_temp = specCount;
						k_temp = s_temp + arab_charPnt;
						for (i = eng_charPnt; i >= 0; i--)
						{
							if(SpecialCheck((INT8U)eng_codetemp[i]))	//이전 문자 '기호' 경우
							{
								if(s_temp) 
								{
									if(BracketCheck((INT8U)eng_codetemp[i])) arab_revtemp[k_temp--] = (INT16U)BracketConv((INT8U)eng_codetemp[i]);
									else arab_revtemp[k_temp--] = eng_codetemp[i];
									s_temp--;
									arab_charPnt++;
								}
								else
									break;
								eng_codetemp[i] = 0xFF;
							}
						}
					}
					k_temp = insertCount;
					s_temp = specCount;
					for (i = eng_charPnt; i >= 0; i--)
					{
						if(EngCheck((INT8U)cur_ch)==NUMBER)
						{	
							if(s_temp)
							{
								if(eng_codetemp[i]==0xFF) eng_codetemp[i] = 0x20;
								s_temp--;
							}
							else
							{
								eng_codetemp[i+specCount+1] = eng_codetemp[i];
							}
							k_temp--;
							if(!k_temp) 
							{
								for(j=specCount;j>=1;j--)
								{
									eng_codetemp[i+j] = 0xFF;
								}
								eng_codetemp[i] = cur_ch;
								break;
							}
						}
					}
					if(IsArab((INT8U)cur_ch))
					{
						arab_status=3;	//아랍어 변환 모드
						break;
					}
					if(EngCheck((INT8U)cur_ch)==NUMBER) 	//현재 문자 '숫자' 경우
					{
						if(!num_exist) num_count++;
						num_exist=1;
						numOn=1;
						eng_charPnt++;
						if(arab_exist)
						{
							k_temp=eng_charPnt;
							eng_charPnt=Convert_EngReverseArab(arab_revtemp,k_temp);
							arab_charPnt=-1;
						}
						arab_count=0;
						if(bracketNum) bracketNum=0;
						arab_status=10;
					}
					else if (((INT8U)cur_ch==0xe1) && (mode==0))
					{
						ret_ch=(INT8U)cur_ch;
						arab_status=3;	//아랍어 변환 모드
						return ret_ch;
					}
					break;
				case 3: //아랍어 변환 모드	
					if (arab_count == 1)  // modify previous character (초성 변환)
					{
						save_newchar=arab_newchar;
						arab_newchar=0;
						arabConv=Change_ArabMode(arab_revtemp[arab_charPnt],INITIAL); // First	
						arab_newchar=save_newchar;
						if (arabConv&0x8000) arab_count=0; 
						arab_revtemp[arab_charPnt]=arabConv;
					}
					else if(arab_count > 1) // modify previous character (중성 변환)
					{
						save_newchar=arab_newchar;
						arab_newchar=0; 	//이전 문자는 반영 안되도록 수정
						arabConv=Change_ArabMode(arab_revtemp[arab_charPnt],MIDDLE); // Middle
						arab_newchar=save_newchar;
						if (arabConv&0x8000) arab_count=0; 
						arab_revtemp[arab_charPnt]=arabConv;
					}
					arab_charPnt++;
					if (arab_count == 0)	// modify current character (단독 변환)
					{
						arab_revtemp[arab_charPnt] = Change_ArabMode((INT16U)cur_ch,SINGLE); // Single
					} 
					else					// modify current character (종성 변환)
					{
						arabConv = Change_ArabMode((INT16U)cur_ch,FINAL); // Last
						arab_revtemp[arab_charPnt]=arabConv;
						if (arabConv&0x8000)
						{
							arab_count=0;
							break;
						}
					}
					eng_charPnt++;
					eng_codetemp[eng_charPnt] = 0xFF;
					arab_count++;
					if((arab_exist && num_exist) || (numOn && arab_exist && spec_exist))
					{
						k_temp = insertCount+1;
						s_temp = specCount+1;
						for(i=eng_charPnt;i>=0;i--)
						{
							//if(eng_codetemp[i]!=0xFF) eng_codetemp[i+specCount+1] = eng_codetemp[i];
							if(s_temp)
							{
								if(eng_codetemp[i]==0xFF) eng_codetemp[i] = 0x20;
								s_temp--;
							}
							eng_codetemp[i+specCount+1] = eng_codetemp[i];
							k_temp--;
							if(!k_temp) 
							{
								for(j=specCount;j>=0;j--)
								{
									eng_codetemp[i+j] = 0xFF;
								}
								break;
							}
						}
						//k_temp=eng_charPnt;
						//eng_charPnt=Convert_EngReverseArab(arab_revtemp,k_temp);
						//arab_charPnt=-1;
						if(bracketNum) bracketNum=0;
					}
					arab_exist=1;	//아랍어 입력 존재
					if(!numOn) spec_count=0;
					arab_status=10; //종료 모드
					break;	
				case 4: 	//아랍어 역변환 모드
					if(EngCheck((INT8U)cur_ch) == ENGWORD)
					{
						Convert_EngReverseArab(arab_revtemp,eng_charPnt);
					}
					arab_charPnt=-1;
					arab_status=5;
					//break;
				case 5: 	//문자 저장 모드
					if(arab_exist && SpecialCheck((INT8U)cur_ch)) 
					{
						if(!spec_exist) spec_count++;
						spec_exist=1;
						num_exist=0;
					}
					if(!arab_exist && !spec_exist && SpecialCheck((INT8U)cur_ch)) spec_count++;
					if(BracketCheck((INT8U)cur_ch)) bracketNum=1;
					eng_charPnt++;
					eng_codetemp[eng_charPnt]=(INT16U)cur_ch;
					arab_count=0;
					arab_status=10;
					//break;
				case 10:
					arab_status=0;
					return ret_ch;
			} // end switch
		//return ret_ch;
		}
	}
#endif	//USE_ARAB_ENG_BASED
INT16U Convert_ArabPreview(INT8U *str, INT16U leng, INT16U cur_pt, INT8U firstInputFlag, INT8U inputMode)
{
		int   i, j, k, k_temp, r;
		INT16U v_16;
		INT8U cur_ch, mode;
		INT16U ret;//gm 080820
		ret = 0;
		
		if (leng == 0) {
			return ret;
		}
		
		Convert_ArabInit();	
		
		r=0;	
		for (i=leng-1; i>=0; i--) { // Check Last space
			//		if ((str[i]>=0x20) || (str[i]==HT) || (str[i]==_CR) || (str[i]==LF) || (str[i]==DC1)) {
			if ((str[i]>=0x20) || (str[i]==HT) || (str[i]==_CR) || (str[i]==LF) || (str[i]==DC1)) {	//Space 문자로 처리
				leng=i+1;
				break;
			}
			r++;
		}
		mode=0;
		for (i = 0; i < leng; i++) {
			if (i==leng-1) mode=1;
/*
			cur_ch=Convert_ArabInterpret(str[i],mode);
			if (cur_ch) Convert_ArabInterpret(cur_ch,mode);
*/
			//[start]gm 080823 커서의 좌표계산
#ifdef USE_ENG_BASED_ARAB_SUPPORT

			if(inputMode == KI_ENG_ARAB_MODE)
			{
				cur_ch=Convert_EngArabInterpret(str[i],mode);
				if (cur_ch) Convert_EngArabInterpret(cur_ch,mode);
			}
			else if(inputMode == KI_IP_INPUT_MODE)
			{
				cur_ch=Convert_ArabInterpretExcept(str[i],mode);
				if (cur_ch) Convert_ArabInterpretExcept(cur_ch,mode);
			}
			else
			{
				cur_ch=Convert_ArabInterpret(str[i],mode);
				if (cur_ch) Convert_ArabInterpret(cur_ch,mode);
			}
#else
			if(inputMode != KI_IP_MODE)
			{
				cur_ch=Convert_ArabInterpret(str[i],mode);
				if (cur_ch) Convert_ArabInterpret(cur_ch,mode);
			}
			else
			{
				cur_ch=Convert_ArabInterpretExcept(str[i],mode);
				if (cur_ch) Convert_ArabInterpretExcept(cur_ch,mode);
			}

#endif

			if ((cur_pt - 1) == i && firstInputFlag == 0) { //str[]에서 현재문자가 변환된 arab_codetemp[]에서의 좌표값			
				
				arab_combine_cnt_flag = DISABLE;
				cur_ch = str[cur_pt - 1];
#ifdef USE_ENG_BASED_ARAB_SUPPORT
				ret = cur_pt - (INT16U)Arab_combine_cnt;
#else
				if (IsArab(cur_ch)) ret = cur_pt - (INT16U)Arab_combine_cnt;
				else if((EngCheck(cur_ch)>0 && EngCheck(cur_ch)<3) || //영문알파벳 및 숫자 구분 ( #, $, % 문자 경우 별도 처리)
						(!arab_In && EngCheck(cur_ch)==NUMSIGN && EngCheck(arab_engtemp[arab_nengtottemp-1])==NUMBER)||	//현재 문자 '예외문자' 이고  바로 전 문자 '숫자'
						(!arab_In && exceptEnable && EngCheck(cur_ch)==NUMSIGN)|| //숫자 뒤에 '예외문자' 가 연달아 온 경우
						(eBracketOpen && (cur_ch != 0x20) && arab_nengtemp)) { 	//'괄호열기'상태이고 현재 문자 '공백X'이고 '영문' 있는 경우
					if(eng_num) 
					{
                        if(eBracketOpen && BracketCheck(cur_ch) && !BracketCloseChk(eBracketOpen, cur_ch)&& arab_nengtemp)
                        {
                            ret = cur_pt - (INT16U)Arab_combine_cnt;
                        }
                        else 
						{
							for (j = 0; j < leng; j++)
							{	      				
								cur_ch = str[cur_pt - 1 + j];//*(buffer + (cur_pt - 1) + i);  //마지막 위치 문자부터 증가

								if (IsArab(cur_ch) || cur_ch == 0x00) break;  //문자 없거나 히브리 문자 경우
							}
							for (k = 0; k < cur_pt; k++)
							{
								cur_ch = str[cur_pt - 1 - k]; //마지막 위치 문자부터 감소

								if (IsArab(cur_ch) || cur_ch == 0x00) break;  //문자 없거나 히브리 문자 경우
							}
							ret = cur_pt - eng_num - (INT16U)Arab_combine_cnt;
                        }
					}
					else
					{
						ret = cur_pt - 1 - (INT16U)Arab_combine_cnt;
					}
				}
				else 
				{
					//for (j = 0; j < (leng - cur_pt); j++)
				if(bracketClose && !bracketNumClose) ret = cur_pt - eng_num - (INT16U)Arab_combine_cnt;
				else ret = cur_pt - (INT16U)Arab_combine_cnt;
				}
#endif
			}
			//[end]gm 080823 커서의 좌표계산
		}
		//sprintf(gmsg, "<1.leng=%d,flag=%d,cvt=%d,cur=%d,com=%d>\r\n", leng, firstInputFlag, ret, cur_pt, Arab_combine_cnt);//???
		//MsgOut(gmsg);

#ifdef USE_ENG_BASED_ARAB_SUPPORT
		k_temp=eng_charPnt;
		eng_charPnt=Convert_EngReverseArab(arab_revtemp,k_temp);
#else		
		k_temp=arab_codetemp_pnt;
		arab_codetemp_pnt=Convert_ArabReverseEng(arab_engtemp,k_temp);
/*		
		if (arab_count>1) {
			v_16 = Change_ArabMode(arab_codetemp[arab_codetemp_pnt],3); // change to Last,Available Character [Middle]	 
			arab_codetemp[arab_codetemp_pnt] =v_16;
		}
*/
#endif
		//[start]gm 080821, 초기진입시, 커서 위치 결정함
		if (firstInputFlag) //처음 들어왔을 때 한번 실행항
		{			
			cur_ch = *(str);
			
			if (IsArab(cur_ch) || cur_ch == 0x00) ret = 0;
			else
			{
				for (i = 0; i < leng; i++)
				{	      				
					cur_ch = *(str + i);
					if (IsArab(cur_ch) || cur_ch == 0x00) break;
				}
				ret = i - 1;
			}
			
		}
		if (cur_pt == 0) //처음 입력위치, clear key 누른 경우. 
		{
			cur_ch = str[0];//*str;
			
			if (IsArab(cur_ch) || cur_ch == 0x00) ret = 0;
#ifdef USE_ENG_BASED_ARAB_SUPPORT
			else if((cur_ch >= 0x41 && cur_ch <= 0x5a) || (cur_ch >= 0x61 && cur_ch <= 0x7a))
			{
				for (j = 0; j < leng; j++)//forward
				{
					cur_ch = str[j]; //*(str+i) 
					if (IsArab(cur_ch) || cur_ch == 0x00) break;
				}
				ret = j - 1;
			}
			else ret = 0;
#else
/*
			else
			{
				for (j = 0; j < leng; j++)//forward
				{	      				
					cur_ch = str[j]; //*(str+i) 
					if (IsArab(cur_ch) || cur_ch == 0x00) break;
				}
				ret = j - 1;
			}
*/
			else if((cur_ch >= 0x41 && cur_ch <= 0x5a) || (cur_ch >= 0x61 && cur_ch <= 0x7a))
			{
				for (j = 0; j < leng; j++)//forward
				{
					cur_ch = str[j]; //*(str+i) 
					if (IsArab(cur_ch) || cur_ch == 0x00) break;
				}
				ret = j - 1;
			}
			else ret = 0;

#endif
		//sprintf(gmsg, "<A>j=%d\r\n", j);//???
		//MsgOut(gmsg);
		}
		//sprintf(gmsg, "<2.flag=%d,cvt=%d,cur=%d,com=%d>\r\n", firstInputFlag, ret, cur_pt, Arab_combine_cnt);//???
		//MsgOut(gmsg);
		//sprintf(gmsg, "===============================\r\n");//???
		//MsgOut(gmsg);
		
#ifdef USE_ENG_BASED_ARAB_SUPPORT
		for (i = 0; i < r + Arab_combine_cnt;  i++) 
		{
			eng_charPnt++;
			eng_codetemp[eng_charPnt] = 0x20;
		}
		eng_charPnt++;
		eng_codetemp[eng_charPnt] = 0;

	  	Convert_FontAddress(eng_charPnt);
#else		
		//[end]gm 080821, 초기진입시, 커서 위치 결정함
		for (i = 0; i < r + Arab_combine_cnt;  i++) {
			arab_codetemp_pnt++;
			arab_codetemp[arab_codetemp_pnt] =0x20;
		}
		arab_codetemp_pnt++;
		arab_codetemp[arab_codetemp_pnt]=0;
		
		Convert_FontAddress(arab_codetemp_pnt);
#endif		
		return ret;
} 

#ifdef USE_ARABIC_NUMBER
#define ARABIAN_NUMBER_0	0x0030		// Arab Cas font의 아라비아 숫자 0값
#define ARABIAN_NUMBER_9	0x0039		// Arab Cas font의 아라비아 숫자 9값
#define ARABIC_NUMBER_0		0x0100		// Arab Cas font의 아랍어 숫자 0값
#define ARABIC_NUMBER_9		0x0109		// Arab Cas font의 아랍어 숫자 9값
#define ARABIC_NUMBER_CONVERT_OFFSET	0x00D0		// 아라비아 숫자 - 아랍숫자 코드 오프셋 (0x0100 - 0x0030)

// 아라비아숫자 -> 아랍숫자로 변경
void ConvertArabianNumeralToArabic(INT16U *source)
{
	INT8U index = 0;
	while(1)
	{
		if(source[index] == 0x00)
			break;
		else
		{
			if((ARABIAN_NUMBER_0 <= source[index]) && (source[index] <= ARABIAN_NUMBER_9))
			{
				source[index] += ARABIC_NUMBER_CONVERT_OFFSET;
			}
		}
		index++;
	}
}
#endif //USE_ARABIC_NUMBER	

#ifdef USE_PRINT_LTR_FOR_ARAB
// 아랍어 문자 포함여부 체크
INT8U IsArabBuffer(INT16U *ch_buf)
{
	int tempCnt;
	INT8U byte1st, byte2nd;

	for(tempCnt = 0; tempCnt < ARAB_BUF_SIZE; tempCnt++)
	{
		if(ch_buf[tempCnt] == 0x0000)
			return FALSE;
		else
		{
			byte1st = (ch_buf[tempCnt] >> 8) & 0xFF;
			byte2nd = ch_buf[tempCnt] & 0xFF;
			if(byte1st != 0x00)
				return TRUE;

			else if(byte2nd > 0x7F)
				return TRUE;		
		}
	}
	return FALSE;
}

// arab_codetemp에 일반방향 문자열을 저장
INT8U ChangeToOnlyEnglishBuffer(INT8U *str)
{
	int dataIndex;
	INT8U IsArabFlag = FALSE;
	IsArabFlag = IsArabBuffer(arab_codetemp);

	if(!IsArabFlag)
	{
		for(dataIndex = 0; dataIndex < ARAB_BUF_SIZE; dataIndex++)
		{
			if(str[dataIndex] == 0x00)
				break;
			else
				arab_codetemp[dataIndex] = str[dataIndex];
		}		
	}

	return IsArabFlag;
}
#endif//USE_PRINT_LTR_FOR_ARAB
#endif//USE_ARAB_FONT
