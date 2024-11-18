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
#ifdef USE_CHINA_PINYIN
// extern void   china_pinyin_pos(int x,int y,INT8U onoff);
extern INT16U read_china_charcode(int i,char mode);
#endif
extern INT8U  IsHanCharMid(char *buf, INT16U pt);
extern void   insert_ch(char *str, INT8U size, INT8U index, char ch);
//extern void   keyin_string_set(INT8U x);
extern INT16U keyin_string(INT8U input_mode, INT8U dsp_mode, HUGEDATA INT8U *format, HUGEDATA INT8U *ret_str, INT16U leng, INT16U win_size, INT8U y, INT16U x, INT16U use_lf, INT8U use_sec, INT8U pass);
extern INT16U keyin_value(INT8U data_type, INT8U dsp_mode, INT8U format, INT8U decimal, INT32U ret_addr, INT8U leng, INT8U y, INT16U x, INT8U use_sec, INT8U pass);
extern INT16U keyin_ulong(INT8U dsp_mode, INT8U format, INT8U decimal, long *ret_val, INT8U leng, INT8U y, INT16U x, INT8U use_sec, INT8U pass);
extern INT16U keyin_ulong_hex(INT8U dsp_mode, INT8U format, INT8U decimal, INT32U *ret_val, INT8U leng, INT8U y, INT16U x, INT8U use_sec, INT8U pass);
extern INT16U keyin_yesno(INT8U dsp_mode, long *ret_val, INT8U y, INT16U x, INT8U pass);
extern void   keyin_clearcallback(void);
extern void   keyin_setcallback(void (*function)() );
extern void   keyin_setanycheck(void (*function)() );
extern void   keyin_Init(void);
//DEL060315extern INT16U keyin_getrawcode(INT16U code);
extern INT16U keyin_getrawcode_bytype(INT8U type, INT16U code);
extern void   keyapp_sale_percent(void);
extern void   keyapp_sale_specialprice(void);
#ifdef USE_TOPMART_DEFAULT //sewon 170117
extern void keyapp_sale_price_change_unitprice(void);
#endif
#ifdef USE_EDIT_TOTALPRICE
extern void keyapp_sale_edit_totalprice(void);
#endif
extern void   keyapp_sale_special(void);
#if defined(USE_FIXED_PERCENT_KEY)|| defined(USE_FIXED_PERCENT_KEY_SET)
extern void keyapp_sale_fixedpercent(long tPer);
#endif
extern void   keyapp_sale_negative(void);
extern INT16U keyin_barcode_input(INT8U dsp_mode, HUGEDATA INT8U *ret_str, INT16U leng, INT16U win_size, INT8U y, INT16U x,INT8U pass);
extern void key_set_function(void);
#ifdef USE_FUNCTION_KEY_BLOCK
extern void key_set_onoff_function(void);
#endif
extern void key_GetUserFunctionQty(void);
#ifdef USE_VIETNAM_FONT
extern INT16U VietnamCombinedChar_cnt;
#endif
#ifdef USE_THAI_FONT
extern INT16U ThaiCombinedChar_cnt;
#endif
#ifdef USE_ARAB_FONT
extern INT16U Convert_ArabPreview(INT8U *str, INT16U leng, INT16U cur_pt, INT8U firstInputFlag, INT8U inputMode);
extern INT8U Convert_ArabStrLength(INT8U *str);	//아랍어 길이 변환 함수
extern INT8U Arab_combine_cnt;	//아랍어 결합문자 수
 #ifdef USE_ARABIC_NUMBER
 extern void ConvertArabianNumeralToArabic(INT16U *source);    // 아라비아숫자 -> 아랍숫자로 변경
 #endif //#ifdef USE_ARABIC_NUMBER
 #ifdef USE_PRINT_LTR_FOR_ARAB
 extern INT8U ChangeToOnlyEnglishBuffer(INT8U *str);
 #endif
#endif

#ifdef HEBREW_FONT
extern INT16U Convert_HebrewPreview(INT8U *str, INT16U leng, INT16U cur_pt, INT8U firstInputFlag);
#endif
