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
#include "adm_app.h"
#include "key.h"
#include "cal.h"
#include "programmode.h"
#include "rtc.h"
#include "debug.h"
#include "main.h"
#include "initial.h"
#include "flash_app.h"
#include "common.h"
#include "dsp_app.h"
#include "rtc_app.h"
#include "key_typedef.h"
#include "clerk_app.h"
#include "department.h"
#include "mode_sale.h"
#include "mode_main.h"
#include "ethernet.h"
#include "ethernet_comm.h"
#include "global_set.h"
#include "label_caption.h"
#include "menu.h"
#include "plu_edit.h"
#include "clerk_app.h"
#include "initial_glb.h"
#include "plu_device.h"
#include "scale_config.h"
#include "prt_app.h"
#include "caption_app.h"
#include "plu_manager.h"
#include "ingredient.h"
#include "traceability.h"
#ifdef USE_PERSIA_CAPTION
#include "caption_IR_1256.h"
#endif

extern NETWORK_STATUS network_status;
extern ETHERNET_STATE ethernet_list;
extern INT16U plu_n_field;
extern PLU_GLOBAL_INFO_STRUCT plu_global[];
extern INT32U FLASH_ING_EXT_AREA_PNT;
extern INT32U FLASH_ING_EXT2_AREA_PNT;
extern INT32U FLASH_ING_NOR_AREA_PNT;
extern INT16U EXT_INGREDIENT_NO;
extern INT32U MAX_PLU_REPORT_NO;

void global_permission_init_00(void);
void global_permission_init_01(void);
void global_permission_init_02(void);
extern void	InitPermissionKor(CLERK_PERMISSION_STRUCT* pp);
extern void	InitPermissionEng(CLERK_PERMISSION_STRUCT* pp);
extern void	InitPermissionUS(CLERK_PERMISSION_STRUCT* pp);
#ifdef USE_CHINA_DEFAULT
extern void	InitPermissionCHN(CLERK_PERMISSION_STRUCT* pp);
#endif
#ifdef USE_MON_EMART_DEFAULT
extern void	InitPermissionMON_Emart(CLERK_PERMISSION_STRUCT* pp);
extern void barcodefile_default_for_mon_emart(void);
#endif
#ifdef USE_MONGOL_DEFAULT
extern void	InitPermissionMONGOL(CLERK_PERMISSION_STRUCT* pp);
#endif
extern void CodeDefaultParameter(INT8U selCountry,INT8U	mode);
extern INT8U plu_inhibit_impossible(INT16U ptype);
extern void printer_setdensity(INT16U density);
extern void printer_preprint(INT16U a,INT16U b);
extern void barcodefile_default_KR_trace(void);
extern void scanner_default_KR_trace(void);
extern void barcodefile_default_us(void);
#if defined(USE_EMART_DEFAULT)
extern void barcodefile_default_for_Emart(void);
extern void scanner_default_for_Emart(void);
extern void labelfile_default_for_Emart(void);
#elif defined(USE_GSMART_DEFAULT)
extern void barcodefile_default_for_GSmart(void);
#elif defined(USE_GSMART_TRACE_DEFAULT)
extern void barcodefile_default_for_GSmart_trace(void);
extern void scanner_default_for_GSmart(void);
extern void labelfile_default_for_GSmart(void);
#elif defined(USE_NHMART_DEFAULT) || defined(USE_NHMART_SAFE_DEFAULT)
extern void barcodefile_default_for_NHmart(void);
extern void scanner_default_for_NHmart(void);
#elif defined(USE_CHN_EMART_DEFAULT) || defined(USE_CHINA_DEFAULT)
extern void barcodefile_default_for_chn(void);
#elif defined(USE_LOTTEMART_DEFAULT)
extern void barcodefile_default_for_Lottemart(void);
extern void scanner_default_for_Lottemart(void);
extern void labelfile_default_for_Lottemart(void);
#elif defined(USE_LOTTESUPER_DEFAULT)
extern void barcodefile_default_for_Lottesuper(void);
extern void	scanner_default_for_Lottesuper(void);
extern void labelfile_default_for_Lottesuper(void);
#elif defined(USE_HYUNDAI_DEFAULT)
extern void barcodefile_default_for_Hyundai(void);
extern void scanner_default_for_Hyundai(void);
extern void labelfile_default_for_Hyundai(void);
extern void labelfile_change_for_Hyundai(INT16U labelfmtNo, INT16U barfmtNo, INT32U labelkeyNo);
#elif defined(USE_MANAGE_INVENTORY_DEFAULT)
extern void barcodefile_default_for_ManageInventory(void);
extern void labelfile_default_for_ManageInventory(void);
#elif defined(USE_EMPORIUM_DEFAULT)
extern void barcodefile_default_for_Emporium(void);
#elif defined(USE_VIETNAM_EMART_DEFAULT)
extern void barcodefile_default_for_Vietman_Emart(void);
#elif defined(USE_TOPMART_DEFAULT) //sewon 161121
extern void barcodefile_default_for_Topmart(void);
extern void scanner_default_for_Topmart(void);
extern void labelfile_default_for_Topmart(void);
#elif defined(USE_INDIA_TATA_DEFAULT)
extern void barcodefile_default_for_indiaTATA(void);
extern void tareTable_default_for_indiaTATA(void);
#endif
// extern void plu_tare_inhibit_set(void);
extern INT8U dealer_factory_mode;
//extern INT16S NUM_MENU_PER_PAGE;
extern INT16U plu_ptype_reset(void);
#ifdef USE_EXT_SETTING_MEM
extern INT8U Data_Save_Restore(INT8U restore);
#endif
extern INT8U  OnOff(INT8U tmp);
char global_message[MSG_MAX_NO][34];
char global_datetime[6][16];
char global_country_code[7][4]={"UN", "KR", "US", "EUR", "EUT", "RUS", "ETC"};
//DEL080220INT8U GLOBAL_DISPLAY_INDICATOR_NO;
char display_message[6][34];
// extern void plu_ptype_enable_code(void);
INT8U UseScanner = OFF;
#ifdef USE_TRACE_STANDALONE
//modeIndiv range : 0 ~ 7
	//0: default. 사용안함
	//1: 신세계
	//2: 농협
	//3: GS Mart
	//4: Standard (신세계 + 개체인덱스)
	//5: 이마트
INT8U ModeIndiv = OFF;
INT16U MaxIndividualNo;
INT8U SaveTraceabilityNo;		//Default : read GLOBAL_SALE_SETUP_FLAG15 & 0x03  
#endif
//#ifdef USE_TRACE_STANDALONE
//extern void initGradeData_default(void);
//#endif

#ifdef USE_GSMART_PLU
extern void global_PLU_default_init(void);
#endif

INT8U get_weight_decimal(INT8U capaid,INT8U multi,INT8U unit)
{
	INT8U dec;

	if (capaid == ADM_CUSTOM_CAPA_ID) return get_global_bparam(GLOBAL_DISPLAY_WEIGHTDECIMAL);

	if (unit==2) {
		dec = 0;
	} else if (unit==0) { // kg mode
		dec = decimal_kg[multi][capaid];
	} else {
		dec = decimal_lb[multi][capaid];
	}
	return dec;
}

void change_weight_decimal(INT8U dec)
{
	display_set_weightunit_change(dec);
}

void save_weight_decimal(INT8U dec)
{
	set_global_bparam(GLOBAL_DISPLAY_WEIGHTDECIMAL,dec);
}


//Added by JJANG 20080704
//0: standard, 1:USA,canada, 2:mexico
//0xff : not use 
void global_display_set_4digit(INT8U pd, INT16U country)
{
	INT8U temp;
	INT8U triangleType;

	temp = get_global_bparam(GLOBAL_AREA_MULTIUNITFLAG);
	triangleType = (temp>>4) & 0x07;	// param 865

	if (pd<6) pd=6;
	if (pd>8) pd=8;

// position
#ifdef _USE_LCD_7_25_
  #if defined (USE_PRICE_8DIGIT) || (defined(USE_PRICE_TOTAL_7DIGIT) && !defined(USE_PRICE_UNIT_7DIGIT))
	set_global_bparam(GLOBAL_DISPLAY_TAREPOS	,1);
	set_global_bparam(GLOBAL_DISPLAY_WEIGHTPOS	,13);
	set_global_bparam(GLOBAL_DISPLAY_UNITPRICEPOS	,7);
	set_global_bparam(GLOBAL_DISPLAY_PRICEPOS	,19);
  #elif defined(USE_PRICE_TOTAL_7DIGIT)	&& defined(USE_PRICE_UNIT_7DIGIT)
	set_global_bparam(GLOBAL_DISPLAY_TAREPOS	  ,1);
	set_global_bparam(GLOBAL_DISPLAY_WEIGHTPOS  ,13);
	set_global_bparam(GLOBAL_DISPLAY_UNITPRICEPOS   ,6);
	set_global_bparam(GLOBAL_DISPLAY_PRICEPOS   ,19);
  #else
	set_global_bparam(GLOBAL_DISPLAY_TAREPOS	,2);
	set_global_bparam(GLOBAL_DISPLAY_WEIGHTPOS	,14);
	set_global_bparam(GLOBAL_DISPLAY_UNITPRICEPOS	,7);
	set_global_bparam(GLOBAL_DISPLAY_PRICEPOS	,26-pd);
  #endif
#elif defined(_USE_LCD_7_37D_40DOT_)
  #ifdef USE_PRICE_8DIGIT
	set_global_bparam(GLOBAL_DISPLAY_TAREPOS	,0);
	set_global_bparam(GLOBAL_DISPLAY_WEIGHTPOS	,5);
	set_global_bparam(GLOBAL_DISPLAY_UNITPRICEPOS	,10);	// by-weight 단가
	set_global_bparam(GLOBAL_DISPLAY_PRICEPOS	,25-7);	//가격
  #elif defined(USE_PRICE_TOTAL_7DIGIT) && !defined(USE_PRICE_UNIT_7DIGIT)
	set_global_bparam(GLOBAL_DISPLAY_TAREPOS	,0);
	set_global_bparam(GLOBAL_DISPLAY_WEIGHTPOS	,5);
	set_global_bparam(GLOBAL_DISPLAY_UNITPRICEPOS	,11);	// by-weight 단가
	set_global_bparam(GLOBAL_DISPLAY_PRICEPOS	,25-7);	//가격
  #elif defined(USE_PRICE_TOTAL_7DIGIT) && defined(USE_PRICE_UNIT_7DIGIT)
	set_global_bparam(GLOBAL_DISPLAY_TAREPOS	  ,0);
	set_global_bparam(GLOBAL_DISPLAY_WEIGHTPOS  ,5);
	set_global_bparam(GLOBAL_DISPLAY_UNITPRICEPOS   ,11);   // by-weight 단가
	set_global_bparam(GLOBAL_DISPLAY_PRICEPOS   ,25-7); //가격
  #else
	set_global_bparam(GLOBAL_DISPLAY_TAREPOS	,0);
	set_global_bparam(GLOBAL_DISPLAY_WEIGHTPOS	,5);
	set_global_bparam(GLOBAL_DISPLAY_UNITPRICEPOS	,11);
	set_global_bparam(GLOBAL_DISPLAY_PRICEPOS	,25-pd);
  #endif
#elif defined(_USE_LCD_7_26d_16x96_)
  #ifdef USE_PRICE_8DIGIT
	set_global_bparam(GLOBAL_DISPLAY_TAREPOS	,0);
	set_global_bparam(GLOBAL_DISPLAY_WEIGHTPOS	,5);
	set_global_bparam(GLOBAL_DISPLAY_UNITPRICEPOS	,11);	// by-weight 단가
	set_global_bparam(GLOBAL_DISPLAY_PRICEPOS	,26-pd);	//가격
  #elif defined(USE_PRICE_TOTAL_7DIGIT) && !defined(USE_PRICE_UNIT_7DIGIT)
	set_global_bparam(GLOBAL_DISPLAY_TAREPOS	,0);
	set_global_bparam(GLOBAL_DISPLAY_WEIGHTPOS	,5);
	set_global_bparam(GLOBAL_DISPLAY_UNITPRICEPOS	,12);	// by-weight 단가
	set_global_bparam(GLOBAL_DISPLAY_PRICEPOS	,26-7);	//가격
  #elif defined(USE_PRICE_TOTAL_7DIGIT) && defined(USE_PRICE_UNIT_7DIGIT)
	set_global_bparam(GLOBAL_DISPLAY_TAREPOS	  ,0);
	set_global_bparam(GLOBAL_DISPLAY_WEIGHTPOS  ,5);
	set_global_bparam(GLOBAL_DISPLAY_UNITPRICEPOS   ,11);   // by-weight 단가
	set_global_bparam(GLOBAL_DISPLAY_PRICEPOS   ,26-7); //가격
  #else
	set_global_bparam(GLOBAL_DISPLAY_TAREPOS	,0);
	set_global_bparam(GLOBAL_DISPLAY_WEIGHTPOS	,5);
	set_global_bparam(GLOBAL_DISPLAY_UNITPRICEPOS	,11);
	set_global_bparam(GLOBAL_DISPLAY_PRICEPOS	,26-pd);
  #endif
#else
  #ifdef USE_PRICE_8DIGIT
	set_global_bparam(GLOBAL_DISPLAY_TAREPOS	,0);
	set_global_bparam(GLOBAL_DISPLAY_WEIGHTPOS	,5);
	set_global_bparam(GLOBAL_DISPLAY_UNITPRICEPOS	,10);	// by-weight 단가
	set_global_bparam(GLOBAL_DISPLAY_PRICEPOS	,24-7);	//가격
  #elif defined(USE_PRICE_TOTAL_7DIGIT) && !defined(USE_PRICE_UNIT_7DIGIT)
	set_global_bparam(GLOBAL_DISPLAY_TAREPOS	,0);
	set_global_bparam(GLOBAL_DISPLAY_WEIGHTPOS	,5);
	set_global_bparam(GLOBAL_DISPLAY_UNITPRICEPOS	,11);	// by-weight 단가
	set_global_bparam(GLOBAL_DISPLAY_PRICEPOS	,24-7);	//가격
  #elif defined(USE_PRICE_TOTAL_7DIGIT) && defined(USE_PRICE_UNIT_7DIGIT)
	set_global_bparam(GLOBAL_DISPLAY_TAREPOS	  ,0);
	set_global_bparam(GLOBAL_DISPLAY_WEIGHTPOS  ,5);
	set_global_bparam(GLOBAL_DISPLAY_UNITPRICEPOS   ,10);   // by-weight 단가
	set_global_bparam(GLOBAL_DISPLAY_PRICEPOS   ,24-7); //가격
  #elif defined(USE_PRICE_TOTAL_8DIGIT) && defined(USE_PRICE_UNIT_7DIGIT)
	set_global_bparam(GLOBAL_DISPLAY_TAREPOS	  ,0);
	set_global_bparam(GLOBAL_DISPLAY_WEIGHTPOS  ,5);
	set_global_bparam(GLOBAL_DISPLAY_UNITPRICEPOS   ,10);   // by-weight 단가
	set_global_bparam(GLOBAL_DISPLAY_PRICEPOS   ,25-7); //가격
  #else
	set_global_bparam(GLOBAL_DISPLAY_TAREPOS	,0);
	set_global_bparam(GLOBAL_DISPLAY_WEIGHTPOS	,5);
	set_global_bparam(GLOBAL_DISPLAY_UNITPRICEPOS	,11);
	set_global_bparam(GLOBAL_DISPLAY_PRICEPOS	,24-pd);
  #endif
#endif

// length
#if defined(_USE_LCD_7_37D_40DOT_)
	set_global_bparam(GLOBAL_DISPLAY_TARELENGTH	,5);
	set_global_bparam(GLOBAL_DISPLAY_WEIGHTLENGTH	,5);
#elif defined(_USE_LCD_7_26d_16x96_)
	set_global_bparam(GLOBAL_DISPLAY_TARELENGTH	,5);
	set_global_bparam(GLOBAL_DISPLAY_WEIGHTLENGTH	,6);
#else
	set_global_bparam(GLOBAL_DISPLAY_TARELENGTH	,4);
	set_global_bparam(GLOBAL_DISPLAY_WEIGHTLENGTH	,5);
#endif

#ifdef USE_PRICE_8DIGIT
	set_global_bparam(GLOBAL_DISPLAY_UNITPRICELENGTH,7);	// by-weight 단가
	set_global_bparam(GLOBAL_DISPLAY_PRICELENGTH	,7);   	//가격
#elif defined(USE_PRICE_TOTAL_7DIGIT) && defined(USE_PRICE_UNIT_7DIGIT)
	set_global_bparam(GLOBAL_DISPLAY_UNITPRICELENGTH,7);	// by-weight 단가
	set_global_bparam(GLOBAL_DISPLAY_PRICELENGTH	,7);   	//가격
#elif defined(USE_PRICE_TOTAL_7DIGIT) && !defined (USE_PRICE_UNIT_7DIGIT)
	set_global_bparam(GLOBAL_DISPLAY_UNITPRICELENGTH,6);	// by-weight 단가
	set_global_bparam(GLOBAL_DISPLAY_PRICELENGTH	,7);	//가격
#elif defined(USE_PRICE_TOTAL_8DIGIT) && defined(USE_PRICE_UNIT_7DIGIT)
    set_global_bparam(GLOBAL_DISPLAY_UNITPRICELENGTH,7);    // by-weight 단가
    set_global_bparam(GLOBAL_DISPLAY_PRICELENGTH    ,8);    //가격
#else
	set_global_bparam(GLOBAL_DISPLAY_UNITPRICELENGTH,6);
	set_global_bparam(GLOBAL_DISPLAY_PRICELENGTH	,pd);
#endif
	set_global_bparam(GLOBAL_INPUT_BC_QTYLENGTH  ,2   );
#ifdef _USE_LCD_7_26d_16x96_
	set_global_bparam(GLOBAL_INPUT_BC_PCSLENGTH  ,2   );
	set_global_bparam(GLOBAL_INPUT_BN_QTYLENGTH  ,3   );
#else
	set_global_bparam(GLOBAL_INPUT_BC_PCSLENGTH  ,2   );
	set_global_bparam(GLOBAL_INPUT_BN_QTYLENGTH  ,3   );
#endif
	// triangle position 설정
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
	if (triangleType == 1) //USA, CANADA
	{
		display_set_sign(SIGN_STABLE  ,0 );
		display_set_sign(SIGN_ZERO    ,2 );
		display_set_sign(SIGN_TARE    ,4 );

		display_set_sign(SIGN_KG      ,6 );
		display_set_sign(SIGN_LB      ,8 );

		display_set_sign(SIGN_AUTO    ,10);
		display_set_sign(SIGN_SAVE    ,12);

		display_set_sign(SIGN_CONNECT ,0xff);
		display_set_sign(SIGN_USBMEM  ,13);
		display_set_sign(SIGN_PER_KG  ,14);
		display_set_sign(SIGN_PER_G   ,15);
		display_set_sign(SIGN_PER_LB  ,16);
		display_set_sign(SIGN_CHECKIN ,0xff);

		display_set_sign(SIGN_PREPACK ,18);
		display_set_sign(SIGN_LANG    ,19);

		display_set_sign(SIGN_DISCOUNT,20);
		display_set_sign(SIGN_SHIFT   ,21);
		display_set_sign(SIGN_MASTER  ,22);
		display_set_sign(SIGN_NET     ,23);
		display_set_sign(SIGN_USBMEM  ,25 );
		display_set_sign(SIGN_ETH_LED_LINK,11);
	}
	else if (triangleType == 2) //Mexico
	{
		display_set_sign(SIGN_STABLE  ,0 );
		display_set_sign(SIGN_ZERO    ,2 );
		display_set_sign(SIGN_TARE    ,4 );

		display_set_sign(SIGN_KG      ,6 );
		display_set_sign(SIGN_LB      ,8 );

		display_set_sign(SIGN_AUTO    ,10);
		display_set_sign(SIGN_SAVE    ,12);
		display_set_sign(SIGN_USBMEM  ,14);

		display_set_sign(SIGN_CONNECT ,0xff);
		display_set_sign(SIGN_PER_KG  ,0xff);
		display_set_sign(SIGN_PER_G   ,0xff);
		display_set_sign(SIGN_PER_LB  ,0xff);
		display_set_sign(SIGN_CHECKIN ,0xff);

		display_set_sign(SIGN_PREPACK ,18);
		display_set_sign(SIGN_LANG    ,19);	//

		display_set_sign(SIGN_DISCOUNT,20);
		display_set_sign(SIGN_SHIFT   ,21);
		display_set_sign(SIGN_MASTER  ,22);	//
		display_set_sign(SIGN_NET     ,23);

#ifdef  _USE_LCD_7_20848_
		display_set_sign(SIGN_ETH_LED_LINK,11);
#else
		display_set_sign(SIGN_ETH_LED_LINK,15);
#endif
	}
	else //standard, triangleType = 0;
	{
		display_set_sign(SIGN_KG      ,0xff ); //not use
		display_set_sign(SIGN_LB      ,0xff );

		display_set_sign(SIGN_STABLE  ,5 );
		display_set_sign(SIGN_ZERO    ,7 );
		display_set_sign(SIGN_TARE    ,9 );

		display_set_sign(SIGN_AUTO    ,12);
		display_set_sign(SIGN_CONNECT ,13);
		display_set_sign(SIGN_SAVE    ,14);
		display_set_sign(SIGN_PREPACK ,16);
		display_set_sign(SIGN_CHECKIN ,17);

		display_set_sign(SIGN_PER_KG  ,0xff);
		display_set_sign(SIGN_PER_G   ,0xff);
		display_set_sign(SIGN_PER_LB  ,0xff);

		display_set_sign(SIGN_DISCOUNT,19);
		display_set_sign(SIGN_LANG    ,20);
		display_set_sign(SIGN_SHIFT   ,21);
		display_set_sign(SIGN_MASTER  ,22);
		display_set_sign(SIGN_NET     ,23);

		display_set_sign(SIGN_ETH_LED_LINK,18);
		display_set_sign(SIGN_USBMEM  ,2 );
#ifdef USE_CTS_FUNCTION
		display_set_sign(SIGN_CTS_ALARM, 0);
#endif
	}
#endif	
	display_init();
}

void global_display_set_3digit(void)
{
	INT8U temp;
	INT8U triangleType;

	temp = get_global_bparam(GLOBAL_AREA_MULTIUNITFLAG);
	triangleType = (temp>>4) & 0x07;	// param 865

	set_global_bparam(GLOBAL_DISPLAY_TAREPOS	,0);
	set_global_bparam(GLOBAL_DISPLAY_TARELENGTH	,0);
	set_global_bparam(GLOBAL_DISPLAY_WEIGHTPOS	,0);
	set_global_bparam(GLOBAL_DISPLAY_WEIGHTLENGTH	,5);
	set_global_bparam(GLOBAL_DISPLAY_UNITPRICEPOS	,8);
#ifdef USE_PRICE_UNIT_7DIGIT
	set_global_bparam(GLOBAL_DISPLAY_UNITPRICELENGTH,7);
#elif defined(USE_PRICE_UNIT_8DIGIT)
	set_global_bparam(GLOBAL_DISPLAY_UNITPRICELENGTH,8);
#elif defined(USE_PRICE_UNIT_9DIGIT)
	set_global_bparam(GLOBAL_DISPLAY_UNITPRICELENGTH,9);
#else
	set_global_bparam(GLOBAL_DISPLAY_UNITPRICELENGTH,6);
#endif
	set_global_bparam(GLOBAL_DISPLAY_PRICEPOS	,16);
#ifdef USE_PRICE_TOTAL_8DIGIT
	set_global_bparam(GLOBAL_DISPLAY_PRICELENGTH	, 8);
#elif defined USE_PRICE_TOTAL_9DIGIT
	set_global_bparam(GLOBAL_DISPLAY_PRICELENGTH	, 9);
#else
	set_global_bparam(GLOBAL_DISPLAY_PRICELENGTH	, 7);
#endif

	set_global_bparam(GLOBAL_INPUT_BC_QTYLENGTH  ,2   );
	set_global_bparam(GLOBAL_INPUT_BC_PCSLENGTH  ,2   );
	set_global_bparam(GLOBAL_INPUT_BN_QTYLENGTH  ,3   );

	
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
	if (triangleType == 1) //USA, CANADA
	{
		display_set_sign(SIGN_STABLE  ,0 );
		display_set_sign(SIGN_ZERO	  ,2 );
		display_set_sign(SIGN_TARE	  ,4 );

		display_set_sign(SIGN_KG	  ,6 );
		display_set_sign(SIGN_LB	  ,8 );

		display_set_sign(SIGN_AUTO	  ,10);
		display_set_sign(SIGN_SAVE	  ,12);
		display_set_sign(SIGN_USBMEM  ,13);

		display_set_sign(SIGN_CONNECT ,0xff);
		display_set_sign(SIGN_PER_KG  ,14);
		display_set_sign(SIGN_PER_G   ,15);
		display_set_sign(SIGN_PER_LB  ,16);
		display_set_sign(SIGN_CHECKIN ,0xff);

		display_set_sign(SIGN_PREPACK ,18);
		display_set_sign(SIGN_LANG	  ,19);

		display_set_sign(SIGN_DISCOUNT,20);
		display_set_sign(SIGN_SHIFT   ,21);
		display_set_sign(SIGN_MASTER  ,22);
		display_set_sign(SIGN_NET	  ,23);
		display_set_sign(SIGN_USBMEM  ,25 );
		display_set_sign(SIGN_ETH_LED_LINK,11);
	}
	else if (triangleType == 2) //Mexico
	{
		display_set_sign(SIGN_STABLE  ,0 );
		display_set_sign(SIGN_ZERO	  ,2 );
		display_set_sign(SIGN_TARE	  ,4 );

		display_set_sign(SIGN_KG	  ,6 );
		display_set_sign(SIGN_LB	  ,8 );

		display_set_sign(SIGN_AUTO	  ,10);
		display_set_sign(SIGN_SAVE	  ,12);
		display_set_sign(SIGN_USBMEM  ,14);

		display_set_sign(SIGN_CONNECT ,0xff);
		display_set_sign(SIGN_PER_KG  ,0xff);
		display_set_sign(SIGN_PER_G   ,0xff);
		display_set_sign(SIGN_PER_LB  ,0xff);
		display_set_sign(SIGN_CHECKIN ,0xff);

		display_set_sign(SIGN_PREPACK ,18);
		display_set_sign(SIGN_LANG	  ,19); //

		display_set_sign(SIGN_DISCOUNT,20);
		display_set_sign(SIGN_SHIFT   ,21);
		display_set_sign(SIGN_MASTER  ,22); //
		display_set_sign(SIGN_NET	  ,23);

#ifdef  _USE_LCD_7_20848_
		display_set_sign(SIGN_ETH_LED_LINK,11);
#else
		display_set_sign(SIGN_ETH_LED_LINK,15);
#endif
	}
	else //standard, triangleType = 0;
	{
		display_set_sign(SIGN_KG	  ,0xff ); //not use
		display_set_sign(SIGN_LB	  ,0xff );

		display_set_sign(SIGN_STABLE  ,5 );
		display_set_sign(SIGN_ZERO	  ,7 );
		display_set_sign(SIGN_TARE	  ,9 );

		display_set_sign(SIGN_AUTO	  ,12);
		display_set_sign(SIGN_CONNECT ,13);
		display_set_sign(SIGN_SAVE	  ,14);
		display_set_sign(SIGN_PREPACK ,16);
		display_set_sign(SIGN_CHECKIN ,17);

		display_set_sign(SIGN_PER_KG  ,0xff);
		display_set_sign(SIGN_PER_G   ,0xff);
		display_set_sign(SIGN_PER_LB  ,0xff);

		display_set_sign(SIGN_DISCOUNT,19);
		display_set_sign(SIGN_LANG	  ,20);
		display_set_sign(SIGN_SHIFT   ,21);
		display_set_sign(SIGN_MASTER  ,22);
		display_set_sign(SIGN_NET	  ,23);

		display_set_sign(SIGN_ETH_LED_LINK,18);
		display_set_sign(SIGN_USBMEM  ,2 );
#ifdef USE_CTS_FUNCTION
		display_set_sign(SIGN_CTS_ALARM, 0);
#endif
	}
#endif	

	display_init();
}

void global_set_number(void)
{
	set_base_wparam(FLASH_NUMBER_BARCODE,100);
	set_base_wparam(FLASH_NUMBER_ORIGIN,500);
	set_base_wparam(FLASH_NUMBER_TARE,100);
	set_base_wparam(FLASH_NUMBER_NUTRIFACT,1000);
	set_base_wparam(FLASH_NUMBER_INGRIDIENT_SIZE,DEFAULT_INGREDIENT_SIZE); // 514
	set_base_wparam(FLASH_NUMBER_INGRIDIENT,1000);
	set_base_wparam(FLASH_NUMBER_SHOP,10);
	set_base_wparam(FLASH_NUMBER_LABELADVERTISE,100);
	set_base_wparam(FLASH_NUMBER_DEPARTMENT,100);
	set_base_wparam(FLASH_NUMBER_GROUP  ,100);
	set_base_wparam(FLASH_NUMBER_COUNTRY,100);
	set_base_wparam(FLASH_NUMBER_SLAUGHTER,100);
	set_base_wparam(FLASH_NUMBER_CUTTINGHALL,100);
	//set_base_wparam(FLASH_NUMBER_TRACEBILITY,100);
	set_base_wparam(FLASH_NUMBER_TRACEBILITY,1000);	// Modified by CJK 20060412
	set_base_wparam(FLASH_NUMBER_CLERK,100);
	set_base_wparam(FLASH_NUMBER_CUSTOMER,100);
	set_base_wparam(FLASH_NUMBER_DISCOUNT,100);
	set_base_wparam(FLASH_NUMBER_BONUS,100);
	set_base_wparam(FLASH_NUMBER_TAX,5);
	set_base_wparam(FLASH_NUMBER_SCALE,32);
	set_base_bparam(FLASH_NUMBER_PAID , 7);
	set_base_bparam(FLASH_NUMBER_REPORT_PERIOD,2);
	set_base_wparam(FLASH_NUMBER_TRANSACTION,100);
	set_net_wparam(NETWORK_MAX_CONNECTION,32);
}

/**
 * @brief		printer modeset 초기화 함수만 묶어놓은 함수
 * @param		prt_mode	
 * @return 	void
 * @remark	
 */
void set_printer_default(INT8U prt_mode)
{
	INT16U country;
	country = get_global_wparam(GLOBAL_AREA_VENDOR_COUNTRY);
	
	switch(country)
	{
		case COUNTRY_UN:
		case COUNTRY_ETC:
			break;
		case COUNTRY_KR:
			break;
		case COUNTRY_US:
			break;
		case COUNTRY_EUT:
			prt_mode=1;
			break;
		case COUNTRY_RU:
			break;
		case COUNTRY_CN:
			break;
		case COUNTRY_EU:
		default:
			break;
	}

	printer_modeset_default();	// CJK070206
#ifdef USE_BELARUS_DEFAULT		// Use Belarus Default 	
	prt_mode=2;
	set_global_bparam(GLOBAL_PAPER_TYPE,prt_mode);
	Flash_wwrite(PRT_FLASH_BASE + FLASH_OFFSET_GapBufInPtrReverseTrimValue, 200);
#endif	
	printer_modeset(prt_mode);
}

void set_clerk_init(void)
{
	CLERK_STRUCT clerk;

	memset((INT8U *)&clerk,0x00,sizeof(CLERK_STRUCT));
	clerk.code         =0; clerk.dept         =1; clerk.permissionID =0; clerk.permissionLevel=3;
	sprintf((char *)clerk.name       ,"NO CLERK");
	sprintf((char *)clerk.nickname   ,"NO CLERK");
	sprintf((char *)clerk.password   ,"%s","1234");
	clerk_save(&clerk);

	clerk.code         =1; clerk.dept         =1; clerk.permissionID =2; clerk.permissionLevel=3;
	sprintf((char *)clerk.name,"CLERK");
	sprintf((char *)clerk.nickname,"CLERK");
#ifdef USE_LOTTEMART_DEFAULT
	sprintf((char *)clerk.password	 ,"%s","2415");
#elif defined(USE_EMART_DEFAULT)
	sprintf((char *)clerk.password   ,"%s","2021");
#elif defined(USE_TOPMART_DEFAULT)
	sprintf((char *)clerk.password	 ,"%s","0203"); //sewon 170224
#else
	sprintf((char *)clerk.password   ,"%s","1");
#endif
	clerk_save(&clerk);
    
#if defined(USE_FIXED_PERCENT_KEY) || defined(USE_FIXED_PERCENT_KEY_SET)
	clerk.code         =2; clerk.dept         =1; clerk.permissionID =0; clerk.permissionLevel=1;
	sprintf((char *)clerk.name,"DISC_MANAGER");
	sprintf((char *)clerk.nickname,"DISC");
	sprintf((char *)clerk.password   ,"%s","2");
	clerk_save(&clerk);
#endif
	return;
}

void set_department_init(void)
{
	DEPARTMENT_STRUCT dept;
	INT32U pnt;

	memset((INT8U *)&dept,0x00,sizeof(DEPARTMENT_STRUCT));
	dept._code     = 1;
	dept._speedkey = 1;
#ifdef USE_PERSIA_CAPTION
	sprintf((char *)dept._description, Caption_Default);
#else
	sprintf((char *)dept._description,"Default");
#endif
	pnt  =  DFLASH_BASE + FLASH_DEPARTMENT_AREA;
	pnt +=  DEPARTMENT_STRUCT_SIZE * (dept._code-1);
	Flash_swrite(pnt, (HUGEDATA INT8U *)&dept, sizeof(dept));
}

void set_global_permission_init(INT16U country)
{
	if (country==COUNTRY_KR) 
	{
		global_permission_init_01();
	}
	else if (country==COUNTRY_US)
	{
		global_permission_init_02();
	} 
	else 
	{
#ifdef USE_GERMANY_DEFAULT
		global_permission_init_02();
#else
		global_permission_init_00();
#endif
	}
}

void global_permission_init_00(void)
{
	CLERK_PERMISSION_STRUCT	permission;
	INT32U addr;


#if defined(USE_MON_EMART_DEFAULT)
	InitPermissionMON_Emart(&permission);
#elif defined(USE_MONGOL_DEFAULT)
	InitPermissionMONGOL(&permission);
#elif defined USE_CHINA_DEFAULT
	InitPermissionCHN(&permission);
#else
	InitPermissionEng(&permission);
#endif
	addr =0;
	addr+=FLASH_CLERK_PERMISSION;
	set_base_sparam(addr,(INT8U*)&permission, PERMISSION_STRUCT_SIZE );
	addr = PERMISSION_STRUCT_SIZE;
	addr+=FLASH_CLERK_PERMISSION;
	set_base_sparam(addr,(INT8U*)&permission, PERMISSION_STRUCT_SIZE );
	
}

void global_permission_init_01(void)
{
	CLERK_PERMISSION_STRUCT	permission;
	INT32U addr;

	InitPermissionKor(&permission);
	addr =0;
	addr+=FLASH_CLERK_PERMISSION;
	set_base_sparam(addr,(INT8U*)&permission, PERMISSION_STRUCT_SIZE );
	addr = PERMISSION_STRUCT_SIZE;
	addr+=FLASH_CLERK_PERMISSION;
	set_base_sparam(addr,(INT8U*)&permission, PERMISSION_STRUCT_SIZE );

}

void global_permission_init_02(void)	//Added by JJANG 20090420, 미국 요청 menu 1855 : all '0'
{										//Added by LYH 20140822, 독일 요청 menu 1855 : all '0'
	CLERK_PERMISSION_STRUCT	permission;
	INT32U addr;

	InitPermissionUS(&permission);
	addr =0;
	addr+=FLASH_CLERK_PERMISSION;
	set_base_sparam(addr,(INT8U*)&permission, PERMISSION_STRUCT_SIZE );
	addr = PERMISSION_STRUCT_SIZE;
	addr+=FLASH_CLERK_PERMISSION;
	set_base_sparam(addr,(INT8U*)&permission, PERMISSION_STRUCT_SIZE );

}

void global_net_init(void)
{
	INT8U str[20];
	INT16U port;
	CAPTION_STRUCT	cap;

	strcpy((char *)str,"NETINI");
	display_string(2,(INT8U *)str);

	set_net_wparam(NETWORK_DEPARTMENT_ID, 1);
	set_net_wparam(NETWORK_CLERK_ID     , 1);
	set_net_wparam(NETWORK_SCALE_ID     , 1);
	set_net_wparam(NETWORK_STORE_ID     , 1);
#ifdef USE_CHN_CART_SCALE
	set_net_wparam(FLASH_MARKET_ID     , 1);
#endif

	memset((INT8U *)str,0,20);
#ifdef USE_SCALENAME_TRACE_SHOP_CODE	
	sprintf((char *)str,"00100001");
#else
	sprintf((char *)str,"STD01");
#endif
	set_net_sparam(NETWORK_SCALE_NAME   ,(HUGEDATA INT8U *)str,20);

#ifdef USE_LOTTEMART_DEFAULT
	str[0]=10;str[1]=57;str[2]=0;str[3]=0;
	set_net_sparam(NETWORK_LOCAL_IP,(HUGEDATA INT8U *)str,4);
	str[0]=10;str[1]=57;str[2]=0;str[3]=254;
	set_net_sparam(NETWORK_LOCAL_GATEWAY,(HUGEDATA INT8U *)str,4);
	str[0]=255;str[1]=255;str[2]=255;str[3]=0;
	set_net_sparam(NETWORK_LOCAL_SUBNETMASK,(HUGEDATA INT8U *)str,4);
#elif defined(USE_TOPMART_DEFAULT)
	str[0]=192;str[1]=168;str[2]=1;str[3]=1;
	set_net_sparam(NETWORK_LOCAL_IP,(HUGEDATA INT8U *)str,4);
	str[0]=192;str[1]=168;str[2]=1;str[3]=1;
	set_net_sparam(NETWORK_LOCAL_GATEWAY,(HUGEDATA INT8U *)str,4);
	str[0]=255;str[1]=255;str[2]=255;str[3]=0;
	set_net_sparam(NETWORK_LOCAL_SUBNETMASK,(HUGEDATA INT8U *)str,4);
#else
	str[0]=10;str[1]=10;str[2]=3;str[3]=33;
	set_net_sparam(NETWORK_LOCAL_IP,(HUGEDATA INT8U *)str,4);
	str[0]=10;str[1]=10;str[2]=0;str[3]=1;
	set_net_sparam(NETWORK_LOCAL_GATEWAY,(HUGEDATA INT8U *)str,4);
	str[0]=255;str[1]=255;str[2]=0;str[3]=0;
	set_net_sparam(NETWORK_LOCAL_SUBNETMASK,(HUGEDATA INT8U *)str,4);
#endif
#ifndef CPU_PIC32MZ
	SaveEthernetDataForBoot(); // Ethernet을 통한 FW Update를 위한 IP 정보 초기값
#endif


#if defined(USE_GSMART_DEFAULT) || defined(USE_GSMART_TRACE_DEFAULT)	//GSMART DEFAULT (TCP PORT 4031)
		port = 4031;		//TCP PORT 4031번 사용
#else
		port = 0x4f50;		//TCP PORT 20304 사용
#endif

	set_net_wparam(NETWORK_LOCAL_TCP_PORT,port);
	set_net_wparam(NETWORK_LOCAL_UDP_PORT,port);
	str[0]=0;
	set_net_bparam(NETWORK_LOCAL_ETHERNET_FLAG,str[0]);
	
//	set_net_wparam(NETWORK_SERVICE_FLAG	  ,  0x000e);	//
	set_net_wparam(NETWORK_SERVICE_FLAG	  ,  0x001e);	//SG060512 
#ifdef USE_GERMANY_DEFAULT   // PARAMETER SETTING 	
	set_net_bparam(NETWORK_SERVICE_TYPE	  ,  1);	// set Master as default
#else
	set_net_bparam(NETWORK_SERVICE_TYPE	  ,  3);	// set Network as default
#endif
	set_net_bparam(NETWORK_SERVICE_FLAG3	  ,  0x00);	//SG061228. one2one backup method : RS232 default

	str[0]=0;str[1]=0;str[2]=0;str[3]=0;
	set_base_sparam(FLASH_REMOTE_IP2, str, 4);	// remote ip2
	set_base_wparam(FLASH_REMOTE_IP2+4, 0);		// remote tcp port2
}

void global_set_date(void)
{
	INT32U pnt;
	char str[32];

	pnt =DFLASH_BASE;
	pnt+=FLASH_GLOBAL_AREA;

	memset((INT8U*)str,0,32); 
	strcpy(str,"AM");
	Flash_swrite(pnt+GLOBAL_DATE_AMPMARRAY	     ,(HUGEDATA INT8U *)str,5);
	strcpy(str,"PM");
	Flash_swrite(pnt+GLOBAL_DATE_AMPMARRAY+5     ,(HUGEDATA INT8U *)str,5);

	memset((INT8U*)str,0,10); strcpy(str,"JAN");
	Flash_swrite(pnt+GLOBAL_DATE_MONTHARRAY	     ,(HUGEDATA INT8U *)str,10);
	memset((INT8U*)str,0,10); strcpy(str,"FEB");
	Flash_swrite(pnt+GLOBAL_DATE_MONTHARRAY+10   ,(HUGEDATA INT8U *)str,10);
	memset((INT8U*)str,0,10); strcpy(str,"MAR");
	Flash_swrite(pnt+GLOBAL_DATE_MONTHARRAY+20   ,(HUGEDATA INT8U *)str,10);
	memset((INT8U*)str,0,10); strcpy(str,"APR");
	Flash_swrite(pnt+GLOBAL_DATE_MONTHARRAY+30   ,(HUGEDATA INT8U *)str,10);
	memset((INT8U*)str,0,10); strcpy(str,"MAY");
	Flash_swrite(pnt+GLOBAL_DATE_MONTHARRAY+40   ,(HUGEDATA INT8U *)str,10);
	memset((INT8U*)str,0,10); strcpy(str,"JUN");
	Flash_swrite(pnt+GLOBAL_DATE_MONTHARRAY+50   ,(HUGEDATA INT8U *)str,10);
	memset((INT8U*)str,0,10); strcpy(str,"JUL");
	Flash_swrite(pnt+GLOBAL_DATE_MONTHARRAY+60   ,(HUGEDATA INT8U *)str,10);
	memset((INT8U*)str,0,10); strcpy(str,"AUG");
	Flash_swrite(pnt+GLOBAL_DATE_MONTHARRAY+70   ,(HUGEDATA INT8U *)str,10);
	memset((INT8U*)str,0,10); strcpy(str,"SEP");
	Flash_swrite(pnt+GLOBAL_DATE_MONTHARRAY+80   ,(HUGEDATA INT8U *)str,10);
	memset((INT8U*)str,0,10); strcpy(str,"OCT");
	Flash_swrite(pnt+GLOBAL_DATE_MONTHARRAY+90   ,(HUGEDATA INT8U *)str,10);
	memset((INT8U*)str,0,10); strcpy(str,"NOV");
	Flash_swrite(pnt+GLOBAL_DATE_MONTHARRAY+100  ,(HUGEDATA INT8U *)str,10);
	memset((INT8U*)str,0,10); strcpy(str,"DEC");
	Flash_swrite(pnt+GLOBAL_DATE_MONTHARRAY+110  ,(HUGEDATA INT8U *)str,10);
	Flash_bwrite(pnt+GLOBAL_DATE_SEPARATOR,'-');
	Flash_bwrite(pnt+GLOBAL_TIME_SEPARATOR,':');
}

void global_set_default_code000(void);
void global_set_default_code001(void);
void global_set_default_code002(void);
void global_set_default_code003(void);
void global_set_default_code004(void);
void global_set_default_code005(void);
void global_set_default_code007(void);
extern char	gmsg[];
void global_set_country(INT16U country, INT8U flagKeepDefaultCaption)
{
	INT8U param_restore;
	INT8U prt_mode;
	INT16U func;
#ifdef USE_TRACE_STANDALONE
	INT8U SaveTraceabilitytype;
#endif
#ifdef USE_REMOTE_FUNCTION 
    INT8U remote_control = OFF;
    INT8U global_com = 0;
    INT16U temp16u = 0;
#endif 

#ifdef USE_REMOTE_FUNCTION    
    temp16u = get_net_wparam(NETWORK_SERVICE_FLAG);
    if ((temp16u&0x80) && (temp16u != 0xffff)) 
    {
        remote_control = ON;
        //check wlan enable
        global_com = get_global_bparam(GLOBAL_AREA_COMMUNICATION);    
    }
#endif      
	//DspLoadFont1(DSP_SYSTEM_FONT_ID);
	//Startup.menu_ygap = display_font_get_height();
	VertScrollMsg_Init(DSP_SYSTEM_FONT_ID);
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
	sprintf(gmsg,"1. Set Param.[%d] Setting..",country);
#else
	sprintf(gmsg,"[%d] Set",country);
#endif
	VertScrollMsg_Add(gmsg);
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
	if (country==COUNTRY_RU)
	{
		global_set_default_parameter(7, flagKeepDefaultCaption);
	}
	else
	{
		global_set_default_parameter(6, flagKeepDefaultCaption);
	}
#else
	global_set_default_parameter(7, flagKeepDefaultCaption);
#endif
	set_global_wparam(GLOBAL_START_FLAG   ,0);
	param_restore=0;
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
	VertScrollMsg_Add("2. Country Param. Setting...");
#else
	VertScrollMsg_Add("Country Set");
#endif
	// =======================================
	// Set Parameter and User Default
	//
#ifdef USE_CHINA_DEFAULT	
	dealer_factory_mode = FACTORY_MODE;
	CodeDefaultParameter(COUNTRY_CN,dealer_factory_mode);
#else
	CodeDefaultParameter(country,dealer_factory_mode);
#endif
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
	VertScrollMsg_Add("3. Country Key. Setting...");
#else
	VertScrollMsg_Add("Key Set");
#endif
	KEY_CopyKeyTable(country);
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
	sprintf(gmsg,"4. Set Country[%d] Key ...",country);
#else
	sprintf(gmsg,"[%d] Key",country);
#endif
	VertScrollMsg_Add(gmsg);
	func = FUNC_ALL_ENABLE;
#ifdef  USE_ONLY_LABEL  //KKH for CL3500
	func &= (~( FUNC_TAX_TBL |FUNC_CUSTOM |FUNC_CLERK ));
#endif
#ifndef USE_TRACEABILITY
	func &= (~FUNC_TRACE);
#endif
	set_global_wparam(GLOBAL_ACTION_ENABLE, func);

#ifdef USE_LOTTEMART_DEFAULT
	set_net_wparam(NETWORK_DIVISION_ID,20);
#else
	set_net_wparam(NETWORK_DIVISION_ID,0);	//-[2006.03.06 bmlim
#endif

	prt_mode=0;
	switch (country) {
	    case COUNTRY_UN : 
			global_set_default_code000();
			break;
	    case COUNTRY_KR : 
			global_set_default_code001();
#if defined (NOT_USE_TRACE_CL5200)
			set_global_wparam(GLOBAL_ACTION_ENABLE,FUNC_ORG_TBL);	//CL5200 은 국내 축산 항목 제한됨. 

#elif defined (USE_GSMART_TRACE_DEFAULT)
			set_global_wparam(GLOBAL_ACTION_ENABLE, FUNC_DISCOUNT|FUNC_ORG_TBL|FUNC_GROUP|FUNC_TRACE_KOR);
#else
			set_global_wparam(GLOBAL_ACTION_ENABLE,FUNC_ORG_TBL|FUNC_GROUP|FUNC_TRACE_KOR);
#endif
			break; // Korea	  ;

	    case COUNTRY_US : 
			set_global_wparam(GLOBAL_ACTION_ENABLE, FUNC_DEPTNO|FUNC_GROUP|FUNC_NUT_TBL|FUNC_DISCOUNT|FUNC_ORG_TBL|FUNC_TRACE);
			global_set_default_code002();
			break; // U.S.A	  ;

	    case COUNTRY_EU : 
			global_set_default_code003();
			break; // Euro.label;

	    case COUNTRY_EUT: 
#ifdef  USE_ONLY_LABEL  //LYH for CL5200 2013.06.11
			global_set_default_code003();
#else
			global_set_default_code004();
			prt_mode=1;
#endif			      
			break; // Euro.Ticket;

	    case COUNTRY_RU : 
			global_set_default_code005();
//			set_global_wparam(GLOBAL_ACTION_ENABLE,FUNC_DEPTNO|FUNC_DISCOUNT|FUNC_CLERK|FUNC_TARE_TBL|FUNC_NUT_TBL);
			set_global_wparam(GLOBAL_ACTION_ENABLE, FUNC_DEPTNO|FUNC_DISCOUNT|FUNC_CLERK|FUNC_TARE_TBL|FUNC_GROUP);	// 20060809
			break;

	    case COUNTRY_ETC: 
			global_set_default_code000();
#ifdef  USE_ONLY_LABEL
			set_global_wparam(GLOBAL_ACTION_ENABLE, FUNC_DEPTNO|FUNC_DISCOUNT|FUNC_TARE_TBL|FUNC_GROUP);
#elif defined USE_VIETNAM_LOTTEMART_DEFAULT
			set_global_wparam(GLOBAL_ACTION_ENABLE, FUNC_DEPTNO|FUNC_DISCOUNT|FUNC_CLERK|FUNC_TARE_TBL|FUNC_GROUP|FUNC_ORG_TBL);
#else
			set_global_wparam(GLOBAL_ACTION_ENABLE, FUNC_DEPTNO|FUNC_DISCOUNT|FUNC_CLERK|FUNC_TARE_TBL|FUNC_GROUP);	// 20060809
#endif
#ifdef USE_PERSIA_DEFAULT
            set_global_wparam(GLOBAL_ACTION_ENABLE, FUNC_DEPTNO|FUNC_DISCOUNT|FUNC_CLERK|FUNC_TARE_TBL|FUNC_GROUP|FUNC_TAX_TBL);
#endif
			break;
#ifdef USE_CHINA_DEFAULT
	    case COUNTRY_CN: 
			global_set_default_code007();
//			set_global_wparam(GLOBAL_ACTION_ENABLE, FUNC_DEPTNO);
			break;
#endif
        default         :
			global_set_default_code003();
			set_global_wparam(GLOBAL_ACTION_ENABLE, FUNC_DEPTNO);
			break;
	}
#ifdef USE_CHN_FONT//USE_2BYTE_FONT_MENU
	set_prt_bparam(FLASH_OFFSET_StrType	,TWO_BYTE_FONT_STR);
	set_prt_bparam(FLASH_OFFSET_DspStrType	,TWO_BYTE_FONT_STR);
	set_global_bparam(GLOBAL_MENU_FONTID 	, 3);
	set_global_bparam(GLOBAL_PLU_FONTID   	, 3);
	set_global_bparam(GLOBAL_LABEL_FONTID  	, 4);
#endif    
#ifdef USE_TRACE_STANDALONE 
	SaveTraceabilitytype = get_global_bparam(GLOBAL_SALE_SETUP_FLAG15);
	SaveTraceabilitytype = (SaveTraceabilitytype | 0x01);
	set_global_bparam(GLOBAL_SALE_SETUP_FLAG15, SaveTraceabilitytype); // Default " 상품 개체 "
	set_global_bparam(GLOBAL_SALE_SETUP_FLAG13, 20); // 위해개체  수량 	
#ifdef USE_IMPORT_MEAT  // 독립형 이력 저울  (default : 0)
	set_global_bparam(GLOBAL_SALE_SETUP_FLAG13, 0); // 위해개체  수량 : 0
#endif
#ifdef USE_EMART_DEFAULT  // 이마트  (default : 99)
	set_global_bparam(GLOBAL_SALE_SETUP_FLAG13, 99); // 위해개체  수량 : 99
#endif

	
#endif
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
	VertScrollMsg_Add("5. Printer Setting ...");
#else
	VertScrollMsg_Add("Printer Set");
#endif
	set_printer_default(prt_mode);

	printer_modeset_default();	// CJK070206

#if !defined(USE_KOR_TRACE) && !defined(USE_TOPMART_DEFAULT) //sewon 161121
	plu_inhibit_bycode(PTYPE_AUX_BARCODE_NO,1);
#endif
#ifdef USE_STOKROTKA_MART_DEFAULT
	plu_inhibit_bycode(PTYPE_AUX_BARCODE_NO,0);
#endif
//#ifdef 	USE_ONLY_ZREPORT
//	Report_Select_Init();
//#endif
#ifdef USE_PCS_FONT5_TO_TRACE_CODE	// USE PCS, PLUFONT5 field for Trace CODE 
	plu_inhibit_bycode(PTYPE_PIECE,0);
	plu_inhibit_bycode(PTYPE_PLU_NAME5_FONT,0);
#endif
#ifndef USE_TOPMART_DEFAULT  //sewon 161121
	plu_inhibit_bycode( PTYPE_IMAGE_NO,1);
#endif
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
	VertScrollMsg_Add("6. Permission. Setting...");
#else
	VertScrollMsg_Add("PMS. Set");
#endif
	set_global_permission_init(country);
	
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
	VertScrollMsg_Add("7. Multiple Unit. Setting...");
#else
	VertScrollMsg_Add("Multi Unit");//Multiple Unit. Setting...
#endif
	global_set_multi_unit();
	sprintf(gmsg,"Backup...");
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
	VertScrollMsg_Add("8. Backup...");
#else
	VertScrollMsg_Add("Backup...");
#endif
	if (param_restore) {
		parameter_restore();
	} else {
		parameter_backup();
	}
	Dsp_Fill_Buffer(0);
	Dsp_Diffuse();
#ifdef USE_REMOTE_FUNCTION
    if (remote_control == ON)
    {
        if ((global_com&0x03) == 0x03)
        {
            set_global_bparam(GLOBAL_AREA_COMMUNICATION ,global_com); 
        }
    }
#endif
}

void global_set_multi_unit(void)
{
	INT8U flag;
	flag = get_global_bparam(GLOBAL_AREA_MULTIUNITFLAG);
	GlbOper.EnableKgLb = OnOff(flag&0x02);
	if (GlbOper.EnableKgLb) {
		plu_inhibit_bycode(PTYPE_UNIT_WEIGHT,0);	// Enable
	} else {
		plu_inhibit_bycode(PTYPE_UNIT_WEIGHT,1);	// Disable
	}
	plu_ptype_reset();
}

//Department	1
//PLU No	2
//PLU Type	4
//Name	10
//Group	9
//1st Label ID	80
//2nd Label ID	81
//Origin	55
//Unit Weight	5
//FixedWeight	100
//Currency	7
//Prefix	3
//ItemCode	11
//PCS	14
//PCS ID	15
//Price	6
//Cost Price	21
//SpecialPrice	91
//Tax ID	8
//Tare ID	12
//Tare	13
//Barcode ID	85
//ProducedDate	20
//Packed Date	18
//Packed Time	19
//Sell By Date	16
//Sell By Time	17
//CookByDate	22
//Ingredient	25
//Traceability	35
//Bonus	50
//NutriFact ID	70
//LabelMsg	90
//Reference Dept	71
//Reference PLU	69
//Coupled Dept	64
//Coupled PLU	68
//# of LinkPLU	60
//Link Dept1	61
//Link PLU1	65
//Link Dept2	62
//Link PLU2	66


#ifdef USE_EMART_DEFAULT

void plu_inhibit_default_emart(void)	// KR
{
	plu_inhibit_bycode(PTYPE_ALL,1);	//전체 사용 불가
	plu_inhibit_bycode(PTYPE_PLU_PREFIX,0);	// prefix
	plu_inhibit_bycode(PTYPE_PRICE,0);	// price
	plu_inhibit_bycode(PTYPE_PLU_NAME1,0);	// name1
	plu_inhibit_bycode(PTYPE_ITEM_CODE,0);	// itemcode
	plu_inhibit_bycode(PTYPE_INGREDIENT_NO,0);	// ingredient no
	plu_inhibit_bycode(PTYPE_FIXED_PRICE,0);	// use fixed price type
	plu_inhibit_bycode(PTYPE_PLU_NAME2,0);	// name2
	plu_inhibit_bycode(PTYPE_ORIGIN_NO,0);	// orgin no
	plu_inhibit_bycode(PTYPE_SELLBY_DATE_FLAG,0);	// sell by date flag
	plu_inhibit_bycode(PTYPE_SELLBY_TIME_FLAG,0);	// sell by time flag
	plu_inhibit_bycode(PTYPE_PACKCED_DATE_FLAG,0);	// packed date flag
	plu_inhibit_bycode(PTYPE_PACKCED_TIME_FLAG,0);	// packed time flag
	plu_inhibit_bycode(PTYPE_SELLBY_DATE,0);	// sell by date
	plu_inhibit_bycode(PTYPE_SELLBY_TIME,0);	// sell by time
	plu_inhibit_bycode(PTYPE_PACKCED_DATE,0);	// packed date
	plu_inhibit_bycode(PTYPE_PACKCED_TIME,0);	// packed time
	plu_inhibit_bycode(PTYPE_PLU_NAME3,0);	// Name3
	plu_inhibit_bycode(PTYPE_PLU_NAME4,0);	// Name4
	plu_inhibit_bycode(PTYPE_PLU_NAME5,0);	// Name5
	plu_inhibit_bycode(PTYPE_TRACE_FLAG,0);	// Trace Flag
	plu_inhibit_bycode(PTYPE_PLU_NAME1_FONT,0);	// Name1 Font Size
	plu_inhibit_bycode(PTYPE_PLU_NAME2_FONT,0);	// Name2 Font Size
	plu_inhibit_bycode(PTYPE_PLU_NAME3_FONT,0);	// Name3 Font Size
	plu_inhibit_bycode(PTYPE_PLU_NAME4_FONT,0);	// Name4 Font Size
	plu_inhibit_bycode(PTYPE_PLU_NAME5_FONT,0);	// Name5 Font Size
	plu_inhibit_bycode(PTYPE_FIXED_WEIGHT,0);	// FixedWeight
	plu_inhibit_bycode(PTYPE_LABEL_MSG_NO,0);	// sales message
#ifdef USE_CONTROL_CHANGING_PRICE
	plu_inhibit_bycode(PTYPE_NUTRITION_NO,0);	// Nutrition No.
#endif
#ifdef USE_DIVIDING_FUNCTION
	plu_inhibit_bycode(PTYPE_TAX_NO,0);	// Tax No. 
#endif
	plu_inhibit_bycode(PTYPE_SPECIAL_PRICE,0);	// special price
}
#elif (defined(USE_TOPMART_DEFAULT))
void plu_inhibit_default_topmart(void)	//sewon 161118
{
	plu_inhibit_bycode(PTYPE_ALL,1);	//전체 사용 불가
	plu_inhibit_bycode(PTYPE_PLU_PREFIX,0);	// prefix
	plu_inhibit_bycode(PTYPE_PRICE,0);	// price
	plu_inhibit_bycode(PTYPE_PLU_NAME1,0);	// name1
	plu_inhibit_bycode(PTYPE_ITEM_CODE,0);	// itemcode
	//plu_inhibit_bycode(PTYPE_INGREDIENT_NO,0);	// ingredient no
	plu_inhibit_bycode(PTYPE_FIXED_PRICE,0);	// use fixed price type
	plu_inhibit_bycode(PTYPE_PLU_NAME2,0);	// name2
	plu_inhibit_bycode(PTYPE_ORIGIN_NO,0);	// orgin no
	plu_inhibit_bycode(PTYPE_SELLBY_DATE_FLAG,0);	// sell by date flag
	plu_inhibit_bycode(PTYPE_SELLBY_TIME_FLAG,0);	// sell by time flag
	plu_inhibit_bycode(PTYPE_PACKCED_DATE_FLAG,0);	// packed date flag
	plu_inhibit_bycode(PTYPE_PACKCED_TIME_FLAG,0);	// packed time flag
	plu_inhibit_bycode(PTYPE_SELLBY_DATE,0);	// sell by date
	plu_inhibit_bycode(PTYPE_SELLBY_TIME,0);	// sell by time
	plu_inhibit_bycode(PTYPE_PACKCED_DATE,0);	// packed date
	plu_inhibit_bycode(PTYPE_PACKCED_TIME,0);	// packed time
	plu_inhibit_bycode(PTYPE_PLU_NAME3,0);	// Name3
	plu_inhibit_bycode(PTYPE_PLU_NAME4,0);	// Name4
	plu_inhibit_bycode(PTYPE_PLU_NAME5,0);	// Name5
	plu_inhibit_bycode(PTYPE_PLU_NAME6,0);	// Name6
	plu_inhibit_bycode(PTYPE_PLU_NAME7,0);	// Name7
	plu_inhibit_bycode(PTYPE_PLU_NAME8,0);	// Name8
	plu_inhibit_bycode(PTYPE_FIXED_WEIGHT,0);	// FixedWeight
	plu_inhibit_bycode(PTYPE_LABEL_MSG_NO,0);	// sales message
	plu_inhibit_bycode(PTYPE_GROUP_NO,0);	//Group
	plu_inhibit_bycode(PTYPE_MAX_QTY,0);	//Max Qty
	plu_inhibit_bycode(PTYPE_TR_COMP_FLAG,0);	//Tr. Comp. Flag
	plu_inhibit_bycode(PTYPE_TR_USE_NO,0);	//Tr. Use No.
	plu_inhibit_bycode(PTYPE_KIND_NO,0);	//Kind No.
	plu_inhibit_bycode(PTYPE_TR_SELLBY_DATE,0);	//Tr. sell by date
	plu_inhibit_bycode(PTYPE_BARCODE_NO,0);	//Barcode ID

}
#elif (defined(USE_GSMART_TRACE_DEFAULT))

void plu_inhibit_default_gs_trace(void)	// KR
{
	plu_inhibit_bycode(PTYPE_ALL,1);	//전체 사용 불가
	plu_inhibit_bycode(PTYPE_PLU_PREFIX,0);	// prefix
	plu_inhibit_bycode(PTYPE_PRICE,0);	// price
	plu_inhibit_bycode(PTYPE_PLU_NAME1,0);	// name1
	plu_inhibit_bycode(PTYPE_ITEM_CODE,0);	// itemcode
	plu_inhibit_bycode(PTYPE_INGREDIENT_NO,0);	// ingredient no
	plu_inhibit_bycode(PTYPE_FIXED_PRICE,0);	// use fixed price type
	plu_inhibit_bycode(PTYPE_PLU_NAME2,0);	// name2
	plu_inhibit_bycode(PTYPE_ORIGIN_NO,0);	// orgin no
	plu_inhibit_bycode(PTYPE_SELLBY_DATE_FLAG,0);	// sell by date flag
	plu_inhibit_bycode(PTYPE_SELLBY_TIME_FLAG,0);	// sell by time flag
	plu_inhibit_bycode(PTYPE_PACKCED_DATE_FLAG,0);	// packed date flag
	plu_inhibit_bycode(PTYPE_PACKCED_TIME_FLAG,0);	// packed time flag
	plu_inhibit_bycode(PTYPE_SELLBY_DATE,0);	// sell by date
	plu_inhibit_bycode(PTYPE_SELLBY_TIME,0);	// sell by time
	plu_inhibit_bycode(PTYPE_PACKCED_DATE,0);	// packed date
	plu_inhibit_bycode(PTYPE_PACKCED_TIME,0);	// packed time
	plu_inhibit_bycode(PTYPE_PLU_NAME3,0);	// Name3
	plu_inhibit_bycode(PTYPE_PLU_NAME4,0);	// Name4
	plu_inhibit_bycode(PTYPE_PLU_NAME5,0);	// Name5
	plu_inhibit_bycode(PTYPE_PLU_NAME6,0);	// Name6
	plu_inhibit_bycode(PTYPE_PLU_NAME7,0);	// Name7
	plu_inhibit_bycode(PTYPE_PLU_NAME8,0);	// Name8
	plu_inhibit_bycode(PTYPE_PLU_NAME1_FONT,0);	// Name1 Font Size
	plu_inhibit_bycode(PTYPE_PLU_NAME2_FONT,0);	// Name2 Font Size
	plu_inhibit_bycode(PTYPE_PLU_NAME3_FONT,0);	// Name3 Font Size
	plu_inhibit_bycode(PTYPE_PLU_NAME4_FONT,0);	// Name4 Font Size
	plu_inhibit_bycode(PTYPE_PLU_NAME5_FONT,0);	// Name5 Font Size
	plu_inhibit_bycode(PTYPE_PLU_NAME6_FONT,0);	// Name6 Font Size
	plu_inhibit_bycode(PTYPE_PLU_NAME7_FONT,0);	// Name7 Font Size
	plu_inhibit_bycode(PTYPE_PLU_NAME8_FONT,0);	// Name8 Font Size
	plu_inhibit_bycode(PTYPE_FIXED_WEIGHT,0);	// FixedWeight
	plu_inhibit_bycode(PTYPE_LABEL_MSG_NO,0);	// sales message
	plu_inhibit_bycode(PTYPE_GROUP_NO,0);	//Group
}

#elif (defined(USE_GSMART_DEFAULT))

void plu_inhibit_default_gs(void)	// KR
{
	plu_inhibit_bycode(PTYPE_ALL,1);	//전체 사용 불가
	plu_inhibit_bycode(PTYPE_PLU_PREFIX,0);	// prefix
	plu_inhibit_bycode(PTYPE_PRICE,0);	// price
	plu_inhibit_bycode(PTYPE_PLU_NAME1,0);	// name1
	plu_inhibit_bycode(PTYPE_ITEM_CODE,0);	// itemcode
	plu_inhibit_bycode(PTYPE_INGREDIENT_NO,0);	// ingredient no
	plu_inhibit_bycode(PTYPE_FIXED_PRICE,0);	// use fixed price type
	plu_inhibit_bycode(PTYPE_PLU_NAME2,0);	// name2
	plu_inhibit_bycode(PTYPE_ORIGIN_NO,0);	// orgin no
	plu_inhibit_bycode(PTYPE_SELLBY_DATE_FLAG,0);	// sell by date flag
	plu_inhibit_bycode(PTYPE_SELLBY_TIME_FLAG,0);	// sell by time flag
	plu_inhibit_bycode(PTYPE_PACKCED_DATE_FLAG,0);	// packed date flag
	plu_inhibit_bycode(PTYPE_PACKCED_TIME_FLAG,0);	// packed time flag
	plu_inhibit_bycode(PTYPE_SELLBY_DATE,0);	// sell by date
	plu_inhibit_bycode(PTYPE_SELLBY_TIME,0);	// sell by time
	plu_inhibit_bycode(PTYPE_PACKCED_DATE,0);	// packed date
	plu_inhibit_bycode(PTYPE_PACKCED_TIME,0);	// packed time
	plu_inhibit_bycode(PTYPE_PLU_NAME3,0);	// Name3
	plu_inhibit_bycode(PTYPE_PLU_NAME4,0);	// Name4
	plu_inhibit_bycode(PTYPE_PLU_NAME5,0);	// Name5
	plu_inhibit_bycode(PTYPE_PLU_NAME6,0);	// Name6
	plu_inhibit_bycode(PTYPE_PLU_NAME7,0);	// Name7
	plu_inhibit_bycode(PTYPE_PLU_NAME8,0);	// Name8
	plu_inhibit_bycode(PTYPE_PLU_NAME1_FONT,0);	// Name1 Font Size
	plu_inhibit_bycode(PTYPE_PLU_NAME2_FONT,0);	// Name2 Font Size
	plu_inhibit_bycode(PTYPE_PLU_NAME3_FONT,0);	// Name3 Font Size
	plu_inhibit_bycode(PTYPE_PLU_NAME4_FONT,0);	// Name4 Font Size
	plu_inhibit_bycode(PTYPE_PLU_NAME5_FONT,0);	// Name5 Font Size
	plu_inhibit_bycode(PTYPE_PLU_NAME6_FONT,0);	// Name6 Font Size
	plu_inhibit_bycode(PTYPE_PLU_NAME7_FONT,0);	// Name7 Font Size
	plu_inhibit_bycode(PTYPE_PLU_NAME8_FONT,0);	// Name8 Font Size
	plu_inhibit_bycode(PTYPE_FIXED_WEIGHT,0);	// FixedWeight
	plu_inhibit_bycode(PTYPE_LABEL_MSG_NO,0);	// sales message
}

#elif defined(USE_NHMART_DEFAULT) || defined(USE_NHMART_SAFE_DEFAULT)

void plu_inhibit_default_nh(void)	// KR
{
	plu_inhibit_bycode(PTYPE_ALL,1);	//전체 사용 불가
	plu_inhibit_bycode(PTYPE_PLU_PREFIX,0);	// prefix
	plu_inhibit_bycode(PTYPE_PRICE,0);	// price
	plu_inhibit_bycode(PTYPE_PLU_NAME1,0);	// name1
	plu_inhibit_bycode(PTYPE_ITEM_CODE,0);	// itemcode
	plu_inhibit_bycode(PTYPE_INGREDIENT_NO,0);	// ingredient no
	plu_inhibit_bycode(PTYPE_FIXED_PRICE,0);	// use fixed price type
	plu_inhibit_bycode(PTYPE_PLU_NAME2,0);	// name2
	plu_inhibit_bycode(PTYPE_ORIGIN_NO,0);	// orgin no
	plu_inhibit_bycode(PTYPE_SELLBY_DATE_FLAG,0);	// sell by date flag
	plu_inhibit_bycode(PTYPE_SELLBY_TIME_FLAG,0);	// sell by time flag
	plu_inhibit_bycode(PTYPE_PACKCED_DATE_FLAG,0);	// packed date flag
	plu_inhibit_bycode(PTYPE_PACKCED_TIME_FLAG,0);	// packed time flag
	plu_inhibit_bycode(PTYPE_SELLBY_DATE,0);	// sell by date
	plu_inhibit_bycode(PTYPE_SELLBY_TIME,0);	// sell by time
	plu_inhibit_bycode(PTYPE_PACKCED_DATE,0);	// packed date
	plu_inhibit_bycode(PTYPE_PACKCED_TIME,0);	// packed time
	plu_inhibit_bycode(PTYPE_PLU_NAME3,0);	// Name3
	plu_inhibit_bycode(PTYPE_PLU_NAME4,0);	// Name4
	plu_inhibit_bycode(PTYPE_PLU_NAME5,0);	// Name5
	plu_inhibit_bycode(PTYPE_FIXED_WEIGHT,0);	// FixedWeight
	plu_inhibit_bycode(PTYPE_LABEL_MSG_NO,0);	// sales message
#ifdef USE_PRICE_CHANGEABLE_CHECK
	plu_inhibit_bycode(PTYPE_TAX_NO,0);	// price change flag
#endif
}

#elif defined(USE_HYUNDAI_DEFAULT)

void plu_inhibit_default_hyundai(void)	// KR
{
	plu_inhibit_bycode(PTYPE_ALL,1);	//전체 사용 불가
	plu_inhibit_bycode(PTYPE_PLU_PREFIX,0);	// prefix
	plu_inhibit_bycode(PTYPE_PRICE,0);	// price
	plu_inhibit_bycode(PTYPE_PLU_NAME1,0);	// name1
	plu_inhibit_bycode(PTYPE_ITEM_CODE,0);	// itemcode
	plu_inhibit_bycode(PTYPE_INGREDIENT_NO,0);	// ingredient no
	plu_inhibit_bycode(PTYPE_FIXED_PRICE,0);	// use fixed price type
	plu_inhibit_bycode(PTYPE_PLU_NAME2,0);	// name2
	plu_inhibit_bycode(PTYPE_ORIGIN_NO,0);	// orgin no
	plu_inhibit_bycode(PTYPE_SELLBY_DATE,0);	// sell by date
	plu_inhibit_bycode(PTYPE_SELLBY_TIME,0);	// sell by time
	plu_inhibit_bycode(PTYPE_PACKCED_DATE,0);	// packed date
	plu_inhibit_bycode(PTYPE_PACKCED_TIME,0);	// packed time
	plu_inhibit_bycode(PTYPE_PLU_NAME3,0);	// Name3
	plu_inhibit_bycode(PTYPE_FIXED_WEIGHT,0);	// FixedWeight
	plu_inhibit_bycode(PTYPE_LABEL_MSG_NO,0);	// sales message
	plu_inhibit_bycode(PTYPE_GROUP_NO,0);	//Group
	plu_inhibit_bycode(PTYPE_SPECIAL_PRICE,0);	//SpecialPrice
}

#elif defined(USE_STANDARD_TRACE_DEFAULT)

void plu_inhibit_default_standard_trace(void)	// KR
{
	plu_inhibit_bycode(PTYPE_ALL,1);	//전체 사용 불가
	plu_inhibit_bycode(PTYPE_PLU_PREFIX,0);	// prefix
	plu_inhibit_bycode(PTYPE_PRICE,0);	// price
	plu_inhibit_bycode(PTYPE_PLU_NAME1,0);	// name1
	plu_inhibit_bycode(PTYPE_ITEM_CODE,0);	// itemcode
	plu_inhibit_bycode(PTYPE_INGREDIENT_NO,0);	// ingredient no
	plu_inhibit_bycode(PTYPE_FIXED_PRICE,0);	// use fixed price type
	plu_inhibit_bycode(PTYPE_PLU_NAME2,0);	// name2
	plu_inhibit_bycode(PTYPE_ORIGIN_NO,0);	// orgin no
	plu_inhibit_bycode(PTYPE_SELLBY_DATE,0);	// sell by date
	plu_inhibit_bycode(PTYPE_SELLBY_TIME,0);	// sell by time
	plu_inhibit_bycode(PTYPE_PACKCED_DATE,0);	// packed date
	plu_inhibit_bycode(PTYPE_PACKCED_TIME,0);	// packed time
	plu_inhibit_bycode(PTYPE_PLU_NAME3,0);	// Name3
	plu_inhibit_bycode(PTYPE_FIXED_WEIGHT,0);	// FixedWeight
	plu_inhibit_bycode(PTYPE_LABEL_MSG_NO,0);	// sales message
#ifdef USE_PCS_FONT5_TO_TRACE_CODE	// USE PCS, PLUFONT5 field for Trace CODE 
	plu_inhibit_bycode(PTYPE_PIECE,0);	
	plu_inhibit_bycode(PTYPE_PLU_NAME5_FONT,0); 
	plu_inhibit_bycode(PTYPE_GROUP_NO,0);	//Group 
#endif
#ifndef USE_KOREA_CL5200        
	plu_inhibit_bycode(PTYPE_GROUP_NO,0);	//Group
#endif
#ifdef USE_BESTCO_DEFAULT
	plu_inhibit_bycode(PTYPE_TRACE_NUM_CHECK,0);		// Trace Number Check
	plu_inhibit_bycode(PTYPE_UNIT_PRICE_CATEGORY,0);	// 바코드 판매단가구분 기능
#endif
#if (PLU_COMMODITY_LINE_NO == 8)
	plu_inhibit_bycode(PTYPE_PLU_NAME4,0);	// Name4
	plu_inhibit_bycode(PTYPE_PLU_NAME5,0);	// Name5
	plu_inhibit_bycode(PTYPE_PLU_NAME6,0);	// Name6
	plu_inhibit_bycode(PTYPE_PLU_NAME7,0);	// Name7
	plu_inhibit_bycode(PTYPE_PLU_NAME8,0);	// Name8
#endif
}
#elif defined(USE_LOTTESUPER_DEFAULT)
void plu_inhibit_default_lottesuper(void)	// KR
{
	plu_inhibit_bycode(PTYPE_ALL,1);				//전체 사용 불가
	plu_inhibit_bycode(PTYPE_PRICE,0);				// price
	plu_inhibit_bycode(PTYPE_PLU_NAME1,0);			// name1
	plu_inhibit_bycode(PTYPE_PLU_NAME2,0);			// name2
	plu_inhibit_bycode(PTYPE_PLU_NAME3,0);			// Name3
	plu_inhibit_bycode(PTYPE_SELLBY_DATE,0);		// sell by date
	plu_inhibit_bycode(PTYPE_SELLBY_TIME,0);		// sell by time
	plu_inhibit_bycode(PTYPE_PACKCED_DATE,0);		// packed date
	plu_inhibit_bycode(PTYPE_PACKCED_TIME,0);		// packed time
	plu_inhibit_bycode(PTYPE_GROUP_NO,0);			//Group 등급
	plu_inhibit_bycode(PTYPE_EXT_BARCODE,0);		// 보조 바코드 표시
	plu_inhibit_bycode(PTYPE_FIXED_WEIGHT,0);		// FixedWeight
	plu_inhibit_bycode(PTYPE_PLU_PREFIX,0);			// prefix
	plu_inhibit_bycode(PTYPE_ITEM_CODE,0);			// itemcode
  #if (PLU_COMMODITY_LINE_NO == 8)
	plu_inhibit_bycode(PTYPE_PLU_NAME4,0);	// Name4
	plu_inhibit_bycode(PTYPE_PLU_NAME5,0);	// Name5
	plu_inhibit_bycode(PTYPE_PLU_NAME6,0);	// Name6
	plu_inhibit_bycode(PTYPE_PLU_NAME7,0);	// Name7
	plu_inhibit_bycode(PTYPE_PLU_NAME8,0);	// Name8
  #endif
}
#elif defined(USE_LOTTEMART_DEFAULT)
void plu_inhibit_default_lottemart(void)	// KR
{
	plu_inhibit_bycode(PTYPE_ALL,1);	//전체 사용 불가
	plu_inhibit_bycode(PTYPE_PRICE,0);	// price
	plu_inhibit_bycode(PTYPE_PLU_NAME1,0);	// name1
	plu_inhibit_bycode(PTYPE_PLU_NAME2,0);	// name2
	plu_inhibit_bycode(PTYPE_SELLBY_DATE,0);	// sell by date
	plu_inhibit_bycode(PTYPE_SELLBY_TIME,0);	// sell by time
	plu_inhibit_bycode(PTYPE_PLU_NAME3,0);	// Name3
	plu_inhibit_bycode(PTYPE_PLU_NAME4,0);	// Name4
	plu_inhibit_bycode(PTYPE_PLU_NAME5,0);	// Name5
	plu_inhibit_bycode(PTYPE_GROUP_NO,0);	//Group 등급
	plu_inhibit_bycode(PTYPE_PACKCED_DATE_FLAG,0);	// 가공일표시
	plu_inhibit_bycode(PTYPE_SELLBY_DATE_FLAG,0);	// 유효일표시
	plu_inhibit_bycode(PTYPE_PACKCED_TIME_FLAG,0);	// 가공시간표시
	plu_inhibit_bycode(PTYPE_SELLBY_TIME_FLAG,0);	// 유효시간표시
	plu_inhibit_bycode(PTYPE_EXT_BARCODE,0);	// 보조 바코드 표시
	plu_inhibit_bycode(PTYPE_FIXED_WEIGHT,0);	// FixedWeight
  #ifdef USE_DISCOUNT_MEMBERSHIP
	plu_inhibit_bycode(PTYPE_DISCOUNT_RATE1,0);	// 할인율 1 (엘포인트)
	plu_inhibit_bycode(PTYPE_DISCOUNT_RATE2,0);	// 행사시작
	plu_inhibit_bycode(PTYPE_DISCOUNT_START_DATE,0);	// 행사 종료일
	plu_inhibit_bycode(PTYPE_DISCOUNT_END_DATE,0);	// 할인율 2 (카드할인율)
	plu_inhibit_bycode(PTYPE_DISCOUNT_TYPE,0);	// 행사 유형
	plu_inhibit_bycode(PTYPE_INGREDIENT_NO,0);	// ingredient no
  #endif
}
#elif defined(USE_MANAGE_INVENTORY_DEFAULT)
void plu_inhibit_default_ManageInventory(void)
{
	plu_inhibit_bycode(PTYPE_ALL,1);	//전체 사용 불가
	plu_inhibit_bycode(PTYPE_PRICE,0);	// price
	plu_inhibit_bycode(PTYPE_PLU_NAME1,0);	// name1
	plu_inhibit_bycode(PTYPE_PLU_NAME2,0);	// name2
	plu_inhibit_bycode(PTYPE_PLU_NAME3,0);	// Name3
	plu_inhibit_bycode(PTYPE_PLU_NAME4,0);	// Name4
	plu_inhibit_bycode(PTYPE_PLU_NAME5,0);	// Name5
	plu_inhibit_bycode(PTYPE_TARE_VALUE,0);	//Tare
	plu_inhibit_bycode(PTYPE_FIXED_WEIGHT,0);	// FixedWeight
	plu_inhibit_bycode(PTYPE_PACKCED_DATE,0);	// packed date
	plu_inhibit_bycode(PTYPE_PACKCED_TIME,0);	// packed time
}
#else

void plu_inhibit_default_kr(void)	// KR
{
	plu_inhibit_bycode(PTYPE_ALL,1);	//전체 사용 불가
	plu_inhibit_bycode(PTYPE_PLU_PREFIX,0);	// prefix
	plu_inhibit_bycode(PTYPE_PRICE,0);	// price
	plu_inhibit_bycode(PTYPE_PLU_NAME1,0);	// name1
	plu_inhibit_bycode(PTYPE_ITEM_CODE,0);	// itemcode
	plu_inhibit_bycode(PTYPE_SELLBY_DATE,0);	// sell by date
	plu_inhibit_bycode(PTYPE_INGREDIENT_NO,0);	// ingredient no
	plu_inhibit_bycode(PTYPE_FIXED_PRICE,0);	// use fixed price type
	plu_inhibit_bycode(PTYPE_PLU_NAME2,0);	// name2
	plu_inhibit_bycode(PTYPE_ORIGIN_NO,0);	// orgin no
#ifdef USE_KOR_TRACE			//SG060410
	plu_inhibit_bycode(PTYPE_AUX_BARCODE_NO,0); 	//barcode #2
#endif
#ifdef USE_GSMART_PLU
	plu_inhibit_bycode(PTYPE_SELLBY_DATE_FLAG,0);	// sell by date flag
	plu_inhibit_bycode(PTYPE_SELLBY_TIME_FLAG,0);	// sell by time flag
	plu_inhibit_bycode(PTYPE_PACKCED_DATE_FLAG,0);	// packed date flag
	plu_inhibit_bycode(PTYPE_PACKCED_TIME_FLAG,0);	// packed time flag
	plu_inhibit_bycode(PTYPE_SELLBY_TIME,0);	// sell by time
	plu_inhibit_bycode(PTYPE_PACKCED_DATE,0);	// packed date
	plu_inhibit_bycode(PTYPE_PACKCED_TIME,0);	// packed time
	plu_inhibit_bycode(PTYPE_PLU_NAME3,0);	// Name3
	plu_inhibit_bycode(PTYPE_PLU_NAME4,0);	// Name4
	plu_inhibit_bycode(PTYPE_PLU_NAME5,0);	// Name5
	plu_inhibit_bycode(PTYPE_PLU_NAME1_FONT,0);	// Name1 Font Size
	plu_inhibit_bycode(PTYPE_PLU_NAME2_FONT,0);	// Name2 Font Size
	plu_inhibit_bycode(PTYPE_PLU_NAME3_FONT,0);	// Name3 Font Size
	plu_inhibit_bycode(PTYPE_PLU_NAME4_FONT,0);	// Name4 Font Size
	plu_inhibit_bycode(PTYPE_PLU_NAME5_FONT,0);	// Name5 Font Size
  #if (PLU_COMMODITY_LINE_NO == 8)
	plu_inhibit_bycode(PTYPE_PLU_NAME6,0);	// Name6
	plu_inhibit_bycode(PTYPE_PLU_NAME7,0);	// Name7
	plu_inhibit_bycode(PTYPE_PLU_NAME8,0);	// Name8
	plu_inhibit_bycode(PTYPE_PLU_NAME6_FONT,0);	// Name6 Font Size
	plu_inhibit_bycode(PTYPE_PLU_NAME7_FONT,0);	// Name7 Font Size
	plu_inhibit_bycode(PTYPE_PLU_NAME8_FONT,0);	// Name8 Font Size
  #endif
#endif
}
#endif

void plu_inhibit_default_us(void)
{
#ifndef USE_TRACE_STANDALONE
	//code002
	plu_inhibit_bycode(PTYPE_ALL,1);			//Disable All
	plu_inhibit_bycode(PTYPE_GROUP_NO,0);		//Group
	plu_inhibit_bycode(PTYPE_PLU_NAME2,0);		//PLU name 2
	plu_inhibit_bycode(PTYPE_ORIGIN_NO,0);		//Origin
	plu_inhibit_bycode(PTYPE_ITEM_CODE,0);		//ItemCode
	plu_inhibit_bycode(PTYPE_PIECE,0);			//PCS
	plu_inhibit_bycode(PTYPE_PRICE,0);			//Price
	plu_inhibit_bycode(PTYPE_TARE_VALUE,0);		//Tare
	plu_inhibit_bycode(PTYPE_SELLBY_DATE,0);	//Sell by date
#if defined (USE_USA_INGREDIENT) || defined(USE_NORTH_AMERICA_INGREDIENT)
	plu_inhibit_bycode(PTYPE_INGREDIENT_TEXT,0);//direct ingredient text
#else	
	plu_inhibit_bycode(PTYPE_INGREDIENT_NO,0);	//Ingredient
#endif
	plu_inhibit_bycode(PTYPE_NUTRITION_NO,0);	//NutriFact ID	
	plu_inhibit_bycode(PTYPE_TRACE_NO,0);		//Traceability no
#endif
}

void plu_inhibit_default_eut(void)
{
#ifndef USE_TRACE_STANDALONE
	//code004
	plu_inhibit_bycode(PTYPE_ALL,1);			//Disable All
	plu_inhibit_bycode(PTYPE_GROUP_NO,0);		//Group
	plu_inhibit_bycode(PTYPE_LABEL_NO,0);		//1st Label ID
	plu_inhibit_bycode(PTYPE_ORIGIN_NO,0);		//Origin
	plu_inhibit_bycode(PTYPE_ITEM_CODE,0);		//ItemCode
	plu_inhibit_bycode(PTYPE_PIECE,0);			//PCS
	plu_inhibit_bycode(PTYPE_PRICE,0);			//Price
	plu_inhibit_bycode(PTYPE_SPECIAL_PRICE,0);	//SpecialPrice
	plu_inhibit_bycode(PTYPE_TAX_NO,0);			//Tax ID
	plu_inhibit_bycode(PTYPE_TARE_VALUE,0);		//Tare
	plu_inhibit_bycode(PTYPE_BARCODE_NO,0);		//Barcode ID
	plu_inhibit_bycode(PTYPE_PACKCED_DATE,0);	//Packed Date
	plu_inhibit_bycode(PTYPE_PACKCED_TIME,0);	//Packed Time
	plu_inhibit_bycode(PTYPE_SELLBY_DATE,0);	//Sell by date
	plu_inhibit_bycode(PTYPE_SELLBY_TIME,0);	//Sell by time
	plu_inhibit_bycode(PTYPE_INGREDIENT_NO,0);	//Ingredient
	plu_inhibit_bycode(PTYPE_LABEL_MSG_NO,0);	//LabelMsg
	plu_inhibit_bycode(PTYPE_REF_DEPT_NO,0);	//Reference Dept
	plu_inhibit_bycode(PTYPE_REF_PLU_NO,0);		//Reference PLU
	plu_inhibit_bycode(PTYPE_COUPLE_DEPT_NO,0);	//Coupled Dept
	plu_inhibit_bycode(PTYPE_COUPLE_PLU_NO,0);	//Coupled PLU
	plu_inhibit_bycode(PTYPE_NO_OF_LINK_PLU,0);	//# of LinkPLU
	plu_inhibit_bycode(PTYPE_LINK1_DEPT_NO,0);	//Link Dept1
	plu_inhibit_bycode(PTYPE_LINK1_PLU_NO,0);	//Link PLU1
	plu_inhibit_bycode(PTYPE_LINK2_DEPT_NO,0);	//Link Dept2
	plu_inhibit_bycode(PTYPE_LINK2_PLU_NO,0);	//Link PLU2
#endif
}

void plu_inhibit_default_ru(void)
{
#ifndef USE_TRACE_STANDALONE
	//code005
	plu_inhibit_bycode(PTYPE_ALL,1);			//Disable All
	plu_inhibit_bycode(PTYPE_DEPARTMENT_NO,0);	//Department No
	plu_inhibit_bycode(PTYPE_PLU_NO,0);			//PLU No
	plu_inhibit_bycode(PTYPE_PLU_TYPE,0);		//PLU Type
	plu_inhibit_bycode(PTYPE_PLU_NAME1,0);		//name
	plu_inhibit_bycode(PTYPE_PLU_NAME2,0);		//PLU name 2
	plu_inhibit_bycode(PTYPE_PLU_NAME3,0);		//PLU name 3
	plu_inhibit_bycode(PTYPE_GROUP_NO,0);		//Group
	plu_inhibit_bycode(PTYPE_LABEL_NO,0);		//1st Label ID
	plu_inhibit_bycode(PTYPE_UNIT_WEIGHT,0);	//unit weight
	plu_inhibit_bycode(PTYPE_ITEM_CODE,0);		//ItemCode
	plu_inhibit_bycode(PTYPE_PIECE,0);			//Pieces
	plu_inhibit_bycode(PTYPE_QTYUNIT_NO,0);		//qty unit no
	plu_inhibit_bycode(PTYPE_FIXED_PRICE,0);	//use fixed price
	plu_inhibit_bycode(PTYPE_PRICE,0);			//Price
	plu_inhibit_bycode(PTYPE_TARE_VALUE,0);		//Tare
	plu_inhibit_bycode(PTYPE_BARCODE_NO,0);		//Barcode ID
	plu_inhibit_bycode(PTYPE_PACKCED_DATE,0);	//Packed Date
	plu_inhibit_bycode(PTYPE_PACKCED_TIME,0);	//Packed Time
	plu_inhibit_bycode(PTYPE_SELLBY_DATE,0);	//Sell by date
	plu_inhibit_bycode(PTYPE_SELLBY_TIME,0);	//Sell by time
#ifdef USE_DIRECT_INGREDIENT
	plu_inhibit_bycode(PTYPE_INGREDIENT_TEXT,0);//ingredient text
#endif
#ifdef USE_RUSSIA_LOGISTICS_PLU
	plu_inhibit_bycode(PTYPE_GTIN_TYPE,0);
	plu_inhibit_bycode(PTYPE_GTIN_DATA,0);
//	plu_inhibit_bycode(PTYPE_NETWEIGHT_DECIMAL,0);	// 현재 저울 및 CL-Works Pro에서 사용하지 않음(24년 9월 기준)
	plu_inhibit_bycode(PTYPE_CONTAINED_UNIT,0);
	plu_inhibit_bycode(PTYPE_LOT_NUMBER,0);
	plu_inhibit_bycode(PTYPE_SERIAL_NUMBER,0);
#endif
#ifdef USE_SPAR_MEMBER_PRICE
    plu_inhibit_bycode(PTYPE_MEMBER_PRICE,0);	//Sell by time
#endif
#endif
}

void plu_inhibit_default_eu(void)
{
	plu_inhibit_bycode(PTYPE_ALL,1);			//Disable All
	plu_inhibit_bycode(PTYPE_GROUP_NO,0);		//Group
	plu_inhibit_bycode(PTYPE_LABEL_NO,0);		//1st Label ID
	plu_inhibit_bycode(PTYPE_ORIGIN_NO,0);		//Origin
#ifndef USE_SRILANKA_CERTI_DEFAULT
	plu_inhibit_bycode(PTYPE_PLU_PREFIX,0);		//Prefix
	plu_inhibit_bycode(PTYPE_ITEM_CODE,0);		//ItemCode
#endif	// USE_SRILANKA_CERTI_DEFAULT
	plu_inhibit_bycode(PTYPE_PIECE,0);			//PCS
	plu_inhibit_bycode(PTYPE_PRICE,0);			//Price
	plu_inhibit_bycode(PTYPE_TAX_NO,0);			//Tax ID
#ifndef USE_SRILANKA_CERTI_DEFAULT
	plu_inhibit_bycode(PTYPE_TARE_VALUE,0);		//Tare
#endif	// USE_SRILANKA_CERTI_DEFAULT
	plu_inhibit_bycode(PTYPE_BARCODE_NO,0);		//Barcode ID
	plu_inhibit_bycode(PTYPE_PRODUCED_DATE,0);	//ProducedDate
	plu_inhibit_bycode(PTYPE_PACKCED_DATE,0);	//Packed Date
	plu_inhibit_bycode(PTYPE_PACKCED_TIME,0);	//Packed Time
	plu_inhibit_bycode(PTYPE_SELLBY_DATE,0);	//Sell by date
	plu_inhibit_bycode(PTYPE_SELLBY_TIME,0);	//Sell by time
	plu_inhibit_bycode(PTYPE_INGREDIENT_NO,0);	//Ingredient
	plu_inhibit_bycode(PTYPE_LABEL_MSG_NO,0);	//LabelMsg
	plu_inhibit_bycode(PTYPE_REF_DEPT_NO,0);	//Reference Dept
	plu_inhibit_bycode(PTYPE_REF_PLU_NO,0);		//Reference PLU
#ifdef USE_VIETNAM_EMART_DEFAULT
	plu_inhibit_bycode(PTYPE_EXT_BARCODE,0);    // 보조 바코드 표시
	plu_inhibit_bycode(PTYPE_PLU_NAME2,0);	   //PLU name 2
	plu_inhibit_bycode(PTYPE_PLU_NAME3,0);	   //PLU name 2
#endif
#if defined(USE_STANDARD_DIRECT_INGREDIENT) || defined(USE_DIRECT_INGREDIENT)
	plu_inhibit_bycode(PTYPE_INGREDIENT_TEXT,0);	//ingredient text
#endif
#ifdef USE_TURKEY_QR_BARCODE_DEAULT
	plu_inhibit_bycode(PTYPE_PLU_NAME3,0);	   //PLU name 3
#endif
#ifdef USE_UN_LOGISTICS_DEFAULT
	plu_inhibit_bycode(PTYPE_GTIN_TYPE,0);		//GTIN TYPE
	plu_inhibit_bycode(PTYPE_GTIN_DATA,0);		//GTIN String
//	plu_inhibit_bycode(PTYPE_NETWEIGHT_DECIMAL,0);	// 현재 저울 및 CL-Works Pro에서 사용하지 않음(24년 9월 기준)
	plu_inhibit_bycode(PTYPE_CONTAINED_UNIT,0);	//Contained Unit
	plu_inhibit_bycode(PTYPE_LOT_NUMBER,0);		// LOT No
	plu_inhibit_bycode(PTYPE_SERIAL_NUMBER,0);	// Serial No
#endif
}

#ifdef USE_CHINA_DEFAULT

void plu_inhibit_default_chn_standard(void)	// CHN Standard
{
	plu_inhibit_bycode(PTYPE_ALL,1);	//전체 사용 불가
	plu_inhibit_bycode(PTYPE_PLU_PREFIX,0);	// prefix
	plu_inhibit_bycode(PTYPE_PRICE,0);	// price
	plu_inhibit_bycode(PTYPE_PLU_NAME1,0);	// name1
	plu_inhibit_bycode(PTYPE_ITEM_CODE,0);	// itemcode
	plu_inhibit_bycode(PTYPE_INGREDIENT_NO,0);	// ingredient no
#ifdef USE_CHINA_ADD_DEFAULT
	plu_inhibit_bycode(PTYPE_FIXED_PRICE,0);	// use fixed price type
#endif
	plu_inhibit_bycode(PTYPE_PLU_NAME2,0);	// name2
#ifdef USE_CHINA_ADD_DEFAULT
	plu_inhibit_bycode(PTYPE_ORIGIN_NO,0);	// orgin no
#else
	plu_inhibit_bycode(PTYPE_GROUP_NO,0);		//Group
	plu_inhibit_bycode(PTYPE_TARE_VALUE,0);		//Tare
	plu_inhibit_bycode(PTYPE_LABEL_NO,0);		//1st Label ID
	plu_inhibit_bycode(PTYPE_BARCODE_NO,0);		//Barcode ID
	plu_inhibit_bycode(PTYPE_LABEL_MSG_NO,0);	//LabelMsg
#endif
//	plu_inhibit_bycode(PTYPE_SELLBY_DATE_FLAG,0);	// sell by date flag
//	plu_inhibit_bycode(PTYPE_SELLBY_TIME_FLAG,0);	// sell by time flag
//	plu_inhibit_bycode(PTYPE_PACKCED_DATE_FLAG,0);	// packed date flag
//	plu_inhibit_bycode(PTYPE_PACKCED_TIME_FLAG,0);	// packed time flag
	plu_inhibit_bycode(PTYPE_SELLBY_DATE,0);	// sell by date
	plu_inhibit_bycode(PTYPE_PLU_NAME3,0);	// Name3
#if defined(USE_CHINA_BAONENG) || defined(USE_CHINA_BAONENG_COUNTRYGARDEN)
	plu_inhibit_bycode(PTYPE_PLU_NAME4,0);	// Name4
	plu_inhibit_bycode(PTYPE_PLU_NAME5,0);	// Name5
	plu_inhibit_bycode(PTYPE_PLU_NAME6,0);	// Name6
	plu_inhibit_bycode(PTYPE_PLU_NAME7,0);	// Name7
	plu_inhibit_bycode(PTYPE_PLU_NAME8,0);	// Name8
	plu_inhibit_bycode(PTYPE_INGREDIENT_TEXT,0);	//ingredient text
#endif
#ifdef USE_CHINA_CARRFOUR_DEFAULT 
	plu_inhibit_bycode( 50,0);//Bonus -> Price Type
#endif
#ifdef USE_CHINA_ADD_DEFAULT
	plu_inhibit_bycode(PTYPE_SELLBY_TIME,0);	// sell by time
	plu_inhibit_bycode(PTYPE_PACKCED_DATE,0);	// packed date
	plu_inhibit_bycode(PTYPE_PACKCED_TIME,0);	// packed time
	plu_inhibit_bycode(PTYPE_PLU_NAME4,0);	// Name4
	plu_inhibit_bycode(PTYPE_PLU_NAME5,0);	// Name5
//	plu_inhibit_bycode(PTYPE_PLU_NAME1_FONT,0);	// Name1 Font Size
//	plu_inhibit_bycode(PTYPE_PLU_NAME2_FONT,0);	// Name2 Font Size
//	plu_inhibit_bycode(PTYPE_PLU_NAME3_FONT,0);	// Name3 Font Size
//	plu_inhibit_bycode(PTYPE_PLU_NAME4_FONT,0);	// Name4 Font Size
//	plu_inhibit_bycode(PTYPE_PLU_NAME5_FONT,0);	// Name5 Font Size
	plu_inhibit_bycode(PTYPE_FIXED_WEIGHT,0);	// FixedWeight
	plu_inhibit_bycode(PTYPE_LABEL_MSG_NO,0);	// sales message
#endif
#ifdef USE_CHINA_CARRFOUR_DEFAULT
  plu_inhibit_bycode(PTYPE_TRACE_URL,0);	  //CHN Carrefour Traceablility URL
  plu_inhibit_bycode(PTYPE_TRACE_CODE,0);	  //CHN Carrefour Traceablility Code
#endif
#if (PLU_COMMODITY_LINE_NO == 8)
	plu_inhibit_bycode(PTYPE_PLU_NAME6,0);	// Name6
	plu_inhibit_bycode(PTYPE_PLU_NAME7,0);	// Name7
	plu_inhibit_bycode(PTYPE_PLU_NAME8,0);	// Name8
//	plu_inhibit_bycode(PTYPE_PLU_NAME6_FONT,0);	// Name6 Font Size
//	plu_inhibit_bycode(PTYPE_PLU_NAME7_FONT,0);	// Name7 Font Size
//	plu_inhibit_bycode(PTYPE_PLU_NAME8_FONT,0);	// Name8 Font Size
#endif
}
#endif

#if defined(USE_MON_EMART_DEFAULT)
void plu_inhibit_default_mon_emart(void)	// Mongol Emart
{
	plu_inhibit_bycode(PTYPE_ALL,1);	//전체 사용 불가
	plu_inhibit_bycode(PTYPE_GROUP_NO,0);	//Group
	plu_inhibit_bycode(PTYPE_PRICE,0);	// price
	plu_inhibit_bycode(PTYPE_PLU_NAME1,0);	// name1
	plu_inhibit_bycode(PTYPE_ITEM_CODE,0);	// itemcode
	plu_inhibit_bycode(PTYPE_INGREDIENT_NO,0);	// ingredient no
	plu_inhibit_bycode(PTYPE_PLU_NAME2,0);	// name2
	plu_inhibit_bycode(PTYPE_ORIGIN_NO,0);	// orgin no
	plu_inhibit_bycode(PTYPE_SELLBY_DATE,0);	// sell by date
	plu_inhibit_bycode(PTYPE_SELLBY_TIME,0);	// sell by time
	plu_inhibit_bycode(PTYPE_PACKCED_DATE,0);	// packed date
	plu_inhibit_bycode(PTYPE_PACKCED_TIME,0);	// packed time
	plu_inhibit_bycode(PTYPE_PLU_NAME3,0);	// Name3
}
#elif defined(USE_MONGOL_DEFAULT)
void plu_inhibit_default_mongol(void)	// Mongol Emart
{
	plu_inhibit_bycode(PTYPE_ALL,1);	//전체 사용 불가
	plu_inhibit_bycode(PTYPE_GROUP_NO,0);	//Group
	plu_inhibit_bycode(PTYPE_PRICE,0);	// price
	plu_inhibit_bycode(PTYPE_PLU_NAME1,0);	// name1
	plu_inhibit_bycode(PTYPE_ITEM_CODE,0);	// itemcode
	plu_inhibit_bycode(PTYPE_INGREDIENT_NO,0);	// ingredient no
	plu_inhibit_bycode(PTYPE_PLU_NAME2,0);	// name2
	plu_inhibit_bycode(PTYPE_ORIGIN_NO,0);	// orgin no
	plu_inhibit_bycode(PTYPE_SELLBY_DATE,0);	// sell by date
	plu_inhibit_bycode(PTYPE_SELLBY_TIME,0);	// sell by time
	plu_inhibit_bycode(PTYPE_PACKCED_DATE,0);	// packed date
	plu_inhibit_bycode(PTYPE_PACKCED_TIME,0);	// packed time
	plu_inhibit_bycode(PTYPE_PLU_NAME3,0);	// Name3
}
#elif defined(USE_EMPORIUM_DEFAULT)
void plu_inhibit_default_emporium(void)	// EMPORIUM
{
	plu_inhibit_bycode(PTYPE_ALL,1);			//Disable All
	plu_inhibit_bycode(PTYPE_GROUP_NO,0);		//Group
	plu_inhibit_bycode(PTYPE_LABEL_NO,0);		//1st Label ID
	plu_inhibit_bycode(PTYPE_ORIGIN_NO,0);		//Origin
	plu_inhibit_bycode(PTYPE_FIXED_WEIGHT,0);	//FixedWeight
	plu_inhibit_bycode(PTYPE_ITEM_CODE,0);		//ItemCode
	plu_inhibit_bycode(PTYPE_PIECE,0);			//PCS
	plu_inhibit_bycode(PTYPE_PRICE,0);			//Price
	plu_inhibit_bycode(PTYPE_TAX_NO,0);			//Tax ID
	plu_inhibit_bycode(PTYPE_TARE_VALUE,0);		//Tare
	plu_inhibit_bycode(PTYPE_BARCODE_NO,0);		//Barcode ID
	plu_inhibit_bycode(PTYPE_SELLBY_DATE,0);	//sell by date
	plu_inhibit_bycode(PTYPE_SELLBY_TIME,0);	//sell by time
	plu_inhibit_bycode(PTYPE_INGREDIENT_NO,0);	//Ingredient
	plu_inhibit_bycode(PTYPE_LABEL_MSG_NO,0);	//LabelMsg
	plu_inhibit_bycode(PTYPE_PLU_NAME2,0);		//PLU name 2
	plu_inhibit_bycode(PTYPE_INGREDIENT_TEXT,0);//ingredient text
  #ifdef	USE_PLU_MULTIPLE_PRICE
 	plu_inhibit_bycode(PTYPE_REF_PLU_NO,0);		//Reference PLU
	plu_inhibit_bycode(PTYPE_COUPLE_PLU_NO,0);	//Coupled PLU
	plu_inhibit_bycode(PTYPE_LINK1_PLU_NO,0);	//Link PLU1
	plu_inhibit_bycode(PTYPE_LINK2_PLU_NO,0);	//Link PLU2
  #endif
}
#elif defined (USE_GERMANY_DEFAULT)
void plu_inhibit_default_germany(void)	// GERMANY
{
	plu_inhibit_bycode(PTYPE_ALL,1);			//Disable All
	plu_inhibit_bycode(PTYPE_GROUP_NO,0);		//Group
	plu_inhibit_bycode(PTYPE_PLU_NAME2,0);		//PLU name 2
	plu_inhibit_bycode(PTYPE_LABEL_NO,0);		//1st Label ID
	plu_inhibit_bycode(PTYPE_FIXED_WEIGHT,0);	//FixedWeight
	plu_inhibit_bycode(PTYPE_PIECE,0);			//PCS
	plu_inhibit_bycode(PTYPE_PRICE,0);			//Price
	plu_inhibit_bycode(PTYPE_TAX_NO,0);			//Tax ID
	plu_inhibit_bycode(PTYPE_TARE_VALUE,0);		//Tare
	plu_inhibit_bycode(PTYPE_BARCODE_NO,0);		//Barcode ID
	plu_inhibit_bycode(PTYPE_SELLBY_DATE,0);	//Sell by date
	plu_inhibit_bycode(PTYPE_SELLBY_TIME,0);	//Sell by time
	plu_inhibit_bycode(PTYPE_INGREDIENT_NO,0);	//Ingredient
	plu_inhibit_bycode(PTYPE_BONUS,1);			//Bonus
	plu_inhibit_bycode(PTYPE_LABEL_MSG_NO,0);	//LabelMsg
}
#elif defined(USE_VIETNAM_DEFAULT)
void plu_inhibit_default_vietnam(void)	// Vietnam Standard Version (V)
{
	plu_inhibit_bycode(PTYPE_ALL,1);	//전체 사용 불가
	plu_inhibit_bycode(PTYPE_GROUP_NO,0);	//Group
	plu_inhibit_bycode(PTYPE_PRICE,0);	// price
	plu_inhibit_bycode(PTYPE_PLU_NAME1,0);	// name1
	plu_inhibit_bycode(PTYPE_ITEM_CODE,0);	// itemcode
	plu_inhibit_bycode(PTYPE_INGREDIENT_NO,0);	// ingredient no
	plu_inhibit_bycode(PTYPE_PLU_NAME2,0);	// name2
	plu_inhibit_bycode(PTYPE_PLU_NAME3,0);	// Name3
	plu_inhibit_bycode(PTYPE_SELLBY_DATE,0);	// sell by date
	plu_inhibit_bycode(PTYPE_SELLBY_TIME,0);	// sell by time
	plu_inhibit_bycode(PTYPE_PACKCED_DATE,0);	// packed date
	plu_inhibit_bycode(PTYPE_PACKCED_TIME,0);	// packed time
	plu_inhibit_bycode(PTYPE_FIXED_WEIGHT,0);	// fixed weight
	plu_inhibit_bycode(PTYPE_TARE_VALUE,0);	// tare value
	plu_inhibit_bycode(PTYPE_PRODUCED_DATE,0);	// produced date
	plu_inhibit_bycode(PTYPE_COOKBY_DATE,0);	// cook by date
	plu_inhibit_bycode(PTYPE_LABEL_MSG_NO,0);	// Sales Msg No.
	plu_inhibit_bycode(PTYPE_EXT_BARCODE,0);	// External Barcode (for Vietnam Temperature)
}
#elif defined(USE_VIETNAM_LOTTEMART_DEFAULT)
void plu_inhibit_default_vietnam_lottemart(void)
{
	plu_inhibit_bycode(PTYPE_ALL,1);			//Disable All
	plu_inhibit_bycode(PTYPE_GROUP_NO,0);		//Group
	plu_inhibit_bycode(PTYPE_LABEL_NO,0);		//1st Label ID
	plu_inhibit_bycode(PTYPE_ORIGIN_NO,0);		//Origin
	plu_inhibit_bycode(PTYPE_PLU_PREFIX,0);		//Prefix
	plu_inhibit_bycode(PTYPE_ITEM_CODE,0);		//ItemCode
	plu_inhibit_bycode(PTYPE_PIECE,0);			//PCS
	plu_inhibit_bycode(PTYPE_PRICE,0);			//Price
	plu_inhibit_bycode(PTYPE_TAX_NO,0);			//Tax ID
	plu_inhibit_bycode(PTYPE_TARE_VALUE,0);		//Tare
	plu_inhibit_bycode(PTYPE_BARCODE_NO,0);		//Barcode ID
	plu_inhibit_bycode(PTYPE_PRODUCED_DATE,0);	//ProducedDate
	plu_inhibit_bycode(PTYPE_PACKCED_DATE,0);	//Packed Date
	plu_inhibit_bycode(PTYPE_PACKCED_TIME,0);	//Packed Time
	plu_inhibit_bycode(PTYPE_SELLBY_DATE,0);	//Sell by date
	plu_inhibit_bycode(PTYPE_SELLBY_TIME,0);	//Sell by time
	plu_inhibit_bycode(PTYPE_INGREDIENT_NO,0);	//Ingredient
	plu_inhibit_bycode(PTYPE_LABEL_MSG_NO,0);	//LabelMsg
	plu_inhibit_bycode(PTYPE_REF_DEPT_NO,0);	//Reference Dept
	plu_inhibit_bycode(PTYPE_REF_PLU_NO,0);		//Reference PLU
	plu_inhibit_bycode(PTYPE_EXT_BARCODE,0);    // 보조 바코드 표시
	plu_inhibit_bycode(PTYPE_PLU_NAME2,0);	   //PLU name 2
	plu_inhibit_bycode(PTYPE_PLU_NAME3,0);	   //PLU name 3
	plu_inhibit_bycode(PTYPE_PLU_NAME4,0);	   //PLU name 4
	plu_inhibit_bycode(PTYPE_PLU_NAME5,0);	   //PLU name 5
}
#else
void plu_inhibit_default_un_etc(void)
{
	plu_inhibit_bycode(PTYPE_ALL,1);			//Disable All
	plu_inhibit_bycode(PTYPE_GROUP_NO,0);		//Group
	plu_inhibit_bycode(PTYPE_LABEL_NO,0);		//1st Label ID	plu_inhibit_bycode( 80,0);//1st Label ID
	plu_inhibit_bycode(PTYPE_ORIGIN_NO,0);		//Origin
	plu_inhibit_bycode(PTYPE_FIXED_WEIGHT,0);	//FixedWeight
	plu_inhibit_bycode(PTYPE_ITEM_CODE,0);		//ItemCode
	plu_inhibit_bycode(PTYPE_PIECE,0);			//PCS
	plu_inhibit_bycode(PTYPE_PRICE,0);			//Price
	plu_inhibit_bycode(PTYPE_TAX_NO,0);			//Tax ID
	plu_inhibit_bycode(PTYPE_TARE_VALUE,0);		//Tare
	plu_inhibit_bycode(PTYPE_BARCODE_NO,0);		//Barcode ID
	plu_inhibit_bycode(PTYPE_SELLBY_DATE,0);	//Sell by date
	plu_inhibit_bycode(PTYPE_SELLBY_TIME,0);	//Sell by time
	plu_inhibit_bycode(PTYPE_INGREDIENT_NO,0);	//Ingredient
	plu_inhibit_bycode(PTYPE_LABEL_MSG_NO,0);	//LabelMsg
	plu_inhibit_bycode(PTYPE_REF_DEPT_NO,0);	//Reference Dept
	plu_inhibit_bycode(PTYPE_REF_PLU_NO,0);		//Reference PLU
  #if defined(USE_STANDARD_DIRECT_INGREDIENT) || defined(USE_DIRECT_INGREDIENT)
	plu_inhibit_bycode(PTYPE_INGREDIENT_TEXT,0);	//ingredient text
  #endif
}
#endif

#ifdef USE_STOKROTKA_MART_DEFAULT
void plu_inhibit_default_poland_stokrotka(void)	// Poland stokrotka - SK
{
	plu_inhibit_bycode(PTYPE_ALL,0);					//전체 사용
}
#endif

#ifdef USE_POLAND_LOGISTICS_DEFAULT
void plu_inhibit_default_poland_logistics(void)
{
	plu_inhibit_bycode(PTYPE_ALL,1);			//Disable All
	plu_inhibit_bycode(PTYPE_GROUP_NO,0);		//Group
	plu_inhibit_bycode(PTYPE_LABEL_NO,0);		//1st Label ID	plu_inhibit_bycode( 80,0);//1st Label ID
	plu_inhibit_bycode(PTYPE_ORIGIN_NO,0);		//Origin
	plu_inhibit_bycode(PTYPE_FIXED_WEIGHT,0);	//FixedWeight
	plu_inhibit_bycode(PTYPE_ITEM_CODE,0);		//ItemCode
	plu_inhibit_bycode(PTYPE_PIECE,0);			//PCS
	plu_inhibit_bycode(PTYPE_PRICE,0);			//Price
	plu_inhibit_bycode(PTYPE_TAX_NO,0);			//Tax ID
	plu_inhibit_bycode(PTYPE_TARE_VALUE,0);		//Tare
	plu_inhibit_bycode(PTYPE_BARCODE_NO,0);		//Barcode ID
	plu_inhibit_bycode(PTYPE_SELLBY_DATE,0);	//Sell by date
	plu_inhibit_bycode(PTYPE_SELLBY_TIME,0);	//Sell by time
	plu_inhibit_bycode(PTYPE_INGREDIENT_NO,0);	//Ingredient
	plu_inhibit_bycode(PTYPE_LABEL_MSG_NO,0);	//LabelMsg
	plu_inhibit_bycode(PTYPE_REF_DEPT_NO,0);	//Reference Dept
	plu_inhibit_bycode(PTYPE_REF_PLU_NO,0);		//Reference PLU
	plu_inhibit_bycode(PTYPE_GTIN_TYPE,0);		//GTIN TYPE
	plu_inhibit_bycode(PTYPE_GTIN_DATA,0);		//GTIN String
//	plu_inhibit_bycode(PTYPE_NETWEIGHT_DECIMAL,0);	// 현재 저울 및 CL-Works Pro에서 사용하지 않음(24년 9월 기준)
	plu_inhibit_bycode(PTYPE_CONTAINED_UNIT,0);	//Contained Unit
	plu_inhibit_bycode(PTYPE_LOT_NUMBER,0);		// LOT No
	plu_inhibit_bycode(PTYPE_SERIAL_NUMBER,0);	// Serial No
}
#endif

/**
 * @brief		UN, ETC 국가 PLU inhibit 묶어 놓은 함수
 * @param		void
 * @return 	void
 * @remark	
 */
void set_plu_inhibit_un_etc(void)
{
	// code000
#if defined(USE_MON_EMART_DEFAULT)
	plu_inhibit_default_mon_emart();
#elif defined(USE_MONGOL_DEFAULT)
	plu_inhibit_default_mongol();
#elif defined(USE_EMPORIUM_DEFAULT)
	plu_inhibit_default_emporium();
#elif defined (USE_GERMANY_DEFAULT)
	plu_inhibit_default_germany();
#elif defined(USE_VIETNAM_DEFAULT)
	plu_inhibit_default_vietnam();
#elif defined(USE_VIETNAM_LOTTEMART_DEFAULT)
	plu_inhibit_default_vietnam_lottemart();
#else
	plu_inhibit_default_un_etc();
#endif
	return;
}

/**
 * @brief		KR 국가 PLU inhibit 묶어 놓은 함수
 * @param		void
 * @return 	void
 * @remark	
 */
void set_plu_inhibit_kr(void)
{
	//code001
#if defined(USE_EMART_DEFAULT)
	plu_inhibit_default_emart();
#elif defined(USE_GSMART_TRACE_DEFAULT)
	plu_inhibit_default_gs_trace();
#elif defined(USE_GSMART_DEFAULT)
	plu_inhibit_default_gs();
#elif defined(USE_NHMART_DEFAULT) || defined(USE_NHMART_SAFE_DEFAULT)
	plu_inhibit_default_nh();
#elif defined(USE_STANDARD_TRACE_DEFAULT)
	plu_inhibit_default_standard_trace();
#elif defined(USE_LOTTEMART_DEFAULT)
	plu_inhibit_default_lottemart();
#elif defined(USE_LOTTESUPER_DEFAULT)
	plu_inhibit_default_lottesuper();
#elif defined(USE_HYUNDAI_DEFAULT)
	plu_inhibit_default_hyundai();
#elif defined(USE_MANAGE_INVENTORY_DEFAULT)
	plu_inhibit_default_ManageInventory();
#elif defined(USE_TOPMART_DEFAULT) //sewon 161118
	plu_inhibit_default_topmart();
#else
	plu_inhibit_default_kr();
#endif
	return;
}

/**
 * @brief		US 국가 PLU inhibit 묶어 놓은 함수
 * @param		void
 * @return 	void
 * @remark	
 */
void set_plu_inhibit_us(void)
{
	plu_inhibit_default_us();

	return;
}

/**
 * @brief		EUT 국가 PLU inhibit 묶어 놓은 함수
 * @param		void
 * @return 	void
 * @remark	
 */
void set_plu_inhibit_eut(void)
{
	plu_inhibit_default_eut();

	return;
}

/**
 * @brief		RU 국가 PLU inhibit 묶어 놓은 함수
 * @param		void
 * @return 	void
 * @remark	
 */
void set_plu_inhibit_ru(void)
{
	plu_inhibit_default_ru();

	return;
}

/**
 * @brief		CN 국가 PLU inhibit 묶어 놓은 함수
 * @param		void
 * @return 	void
 * @remark	
 */
void set_plu_inhibit_cn(void)
{
#ifdef USE_CHINA_DEFAUL
	plu_inhibit_default_chn_standard();
#endif
}

/**
 * @brief		EU 국가 PLU inhibit 묶어 놓은 함수
 * @param		void
 * @return 	void
 * @remark	
 */
void set_plu_inhibit_eu(void)
{
#ifndef USE_TRACE_STANDALONE
	//code003
#ifdef USE_MON_EMART_DEFAULT
	plu_inhibit_default_mon_emart();
#elif defined(USE_MONGOL_DEFAULT)
	plu_inhibit_default_mongol();
#elif defined (USE_GERMANY_DEFAULT)
	plu_inhibit_default_germany();
#elif defined (USE_STOKROTKA_MART_DEFAULT)
  	plu_inhibit_default_poland_stokrotka();
#elif defined (USE_POLAND_LOGISTICS_DEFAULT)
  	plu_inhibit_default_poland_logistics();
#else
	plu_inhibit_default_eu();
#endif

	return;
#endif
}

/**
 * @brief		KR 국가 scanner default만 묶어 놓은 함수
 * @param		void
 * @return 	void
 * @remark	
 */
void set_scanner_default_kr(void)
{
#if defined(USE_EMART_DEFAULT)
	scanner_default_for_Emart();
#elif defined(USE_GSMART_TRACE_DEFAULT)
	scanner_default_for_GSmart();
#elif defined(USE_TOPMART_DEFAULT) //sewon 161118
	scanner_default_for_Topmart();
#elif defined(USE_NHMART_DEFAULT) || defined(USE_NHMART_SAFE_DEFAULT)
	scanner_default_for_NHmart();
#elif defined(USE_STANDARD_TRACE_DEFAULT)
	scanner_default_KR_trace();
#elif defined(USE_LOTTEMART_DEFAULT)
	scanner_default_for_Lottemart();
#elif defined(USE_LOTTESUPER_DEFAULT)
	scanner_default_for_Lottesuper();
#elif defined(USE_HYUNDAI_DEFAULT)
	scanner_default_for_Hyundai();
#elif defined(USE_MANAGE_INVENTORY_DEFAULT)
	scanner_default_KR_trace();
#else
#endif
}

/**
 * @brief		KR 국가 barcode default만 묶어 놓은 함수
 * @param		void
 * @return 	void
 * @remark	
 */
void set_barcode_default_kr(void)
{
	INT32U i;
#if defined(USE_EMART_DEFAULT)
	barcodefile_default_for_Emart();
	for (i=0; i<4; i++) set_global_bparam(GLOBAL_BARCODE_STD_WID+i,3);
	for (i=4; i<9; i++) set_global_bparam(GLOBAL_BARCODE_STD_WID+i,1);
#elif defined(USE_GSMART_TRACE_DEFAULT)
	barcodefile_default_for_GSmart_trace();
#elif defined(USE_TOPMART_DEFAULT) //sewon 161118
	barcodefile_default_for_Topmart();
	set_global_bparam(GLOBAL_BARCODE_STD_WID, 1);
	set_global_bparam(GLOBAL_BARCODE_STD_CID, 2);
	set_global_bparam(GLOBAL_BARCODE_STD_NID, 1);
	set_global_bparam(GLOBAL_BARCODE_STD_FIX, 1);
	set_global_bparam(GLOBAL_BARCODE_NON_WID, 1);
	set_global_bparam(GLOBAL_BARCODE_NON_CID, 2);
	set_global_bparam(GLOBAL_BARCODE_NON_NID, 1);	
	set_global_bparam(GLOBAL_BARCODE_TOTAL_ID, 1);	
	set_global_bparam(GLOBAL_BARCODE_TOTAL_ID2, 1);
#elif defined(USE_NHMART_DEFAULT) || defined(USE_NHMART_SAFE_DEFAULT)
	barcodefile_default_for_NHmart();
  #ifdef USE_NHMART_DEFAULT
	set_global_bparam(GLOBAL_BARCODE_STD_WID, 1);
	set_global_bparam(GLOBAL_BARCODE_STD_CID, 2);
	set_global_bparam(GLOBAL_BARCODE_STD_NID, 1);
	set_global_bparam(GLOBAL_BARCODE_STD_FIX, 2);
	set_global_bparam(GLOBAL_BARCODE_NON_WID, 1);
	set_global_bparam(GLOBAL_BARCODE_NON_CID, 2);
	set_global_bparam(GLOBAL_BARCODE_NON_NID, 1);
	set_global_bparam(GLOBAL_BARCODE_TOTAL_ID, 1);
	set_global_bparam(GLOBAL_BARCODE_TOTAL_ID2, 1);
  #endif
  #ifdef USE_SETPRODUCT_SALE
	set_global_bparam(GLOBAL_BARCODE_TOTAL_ID,6);
  #endif
#elif defined(USE_STANDARD_TRACE_DEFAULT)
	barcodefile_default_KR_trace();
  #if defined (USE_BESTCO_DEFAULT)
	for (i=0; i<9; i++) 
	{
		set_global_bparam(GLOBAL_BARCODE_STD_WID+i,5);
	}
	set_global_bparam(GLOBAL_BARCODE_2, 4);
  #endif
  #if defined (USE_SHINSEGAE_DEFAULT)
	for (i=0; i<9; i++) set_global_bparam(GLOBAL_BARCODE_STD_WID+i,5);
  #endif
#elif defined(USE_LOTTEMART_DEFAULT)
	barcodefile_default_for_Lottemart();
	set_global_bparam(GLOBAL_BARCODE_STD_WID, 1);
	set_global_bparam(GLOBAL_BARCODE_STD_CID, 2);
	set_global_bparam(GLOBAL_BARCODE_STD_NID, 1);
	set_global_bparam(GLOBAL_BARCODE_STD_FIX, 1);
	set_global_bparam(GLOBAL_BARCODE_NON_WID, 1);
	set_global_bparam(GLOBAL_BARCODE_NON_CID, 2);
	set_global_bparam(GLOBAL_BARCODE_NON_NID, 1);	
	set_global_bparam(GLOBAL_BARCODE_TOTAL_ID, 1);	
	set_global_bparam(GLOBAL_BARCODE_TOTAL_ID2, 1);
#elif defined(USE_LOTTESUPER_DEFAULT)
	barcodefile_default_for_Lottesuper();
	// 상품 종류별 바코드 형식 매핑
	set_global_bparam(GLOBAL_BARCODE_STD_WID, 1);		// 무게 바코드
	set_global_bparam(GLOBAL_BARCODE_STD_CID, 2);		// 개수 바코드
	set_global_bparam(GLOBAL_BARCODE_STD_NID, 1);		// 수량 바코드
	set_global_bparam(GLOBAL_BARCODE_STD_FIX, 1);		// 정가 바코드
	set_global_bparam(GLOBAL_BARCODE_NON_WID, 1);		// 오픈 무게 바코드 (Non-PLU)
	set_global_bparam(GLOBAL_BARCODE_NON_CID, 2);		// 오픈 개수 바코드 (Non-PLU)
	set_global_bparam(GLOBAL_BARCODE_NON_NID, 1);		// 오픈 수량 바코드 (Non-PLU)
	set_global_bparam(GLOBAL_BARCODE_TOTAL_ID, 1);		// 합산판매 바코드
	set_global_bparam(GLOBAL_BARCODE_TOTAL_ID2, 1);		// 합산판매 바코드2
#elif defined(USE_HYUNDAI_DEFAULT)
	barcodefile_default_for_Hyundai();
#elif defined(USE_MANAGE_INVENTORY_DEFAULT)
	barcodefile_default_for_ManageInventory();
#else
	barcodefile_default_KR_trace();
#endif
}

/**
 * @brief		KR 국가 label default만 묶어 놓은 함수
 * @param		void
 * @return 	void
 * @remark	
 */
void set_label_default_kr(void)
{
#if defined(USE_EMART_DEFAULT)
	labelfile_default_for_Emart();
#elif defined(USE_GSMART_TRACE_DEFAULT)
	labelfile_default_for_GSmart();
#elif defined(USE_TOPMART_DEFAULT) //sewon 161118
	labelfile_default_for_Topmart();
#elif defined(USE_LOTTEMART_DEFAULT)
	labelfile_default_for_Lottemart();
#elif defined(USE_LOTTESUPER_DEFAULT)
	labelfile_default_for_Lottesuper();
#elif defined(USE_HYUNDAI_DEFAULT)
	labelfile_default_for_Hyundai();
#elif defined(USE_MANAGE_INVENTORY_DEFAULT)
	labelfile_default_for_ManageInventory();
#endif
}



/**
 * @brief		plu_inhibit_bycode()만 묶어놓은 함수
 * @param		void
 * @return 	void
 * @remark	
 * 				- Menu 1144 관련 항목 
 * 				- Menu 1892에서 Menu 1144 초기화를 구현하기 위해 plu 초기화 관련 함수를 따로 구성하기 위해 분리함
 * 				- global_set_default_code000~007()에 있는 plu_inhibit_bycode()들을 이곳에 묶어 놓고 각 함수에서 호출하는 형태로 재구성
 */
void set_plu_inhibit_default(void)
{
	INT16U country;
	country = get_global_wparam(GLOBAL_AREA_VENDOR_COUNTRY);
	
	// global_set_country() 밖에서 수행되는 몇가지 항목들 고려
	//global_set_multi_unit();
	//plu_inhibit_bycode( PTYPE_IMAGE_NO,1);

	switch(country)
	{
		case COUNTRY_UN:
		case COUNTRY_ETC:
			set_plu_inhibit_un_etc();
			break;
		case COUNTRY_KR:
			set_plu_inhibit_kr();
			break;
		case COUNTRY_US:
			set_plu_inhibit_us();
			break;
		case COUNTRY_EUT:
			set_plu_inhibit_eut();
			break;
		case COUNTRY_RU:
			set_plu_inhibit_ru();
			break;
		case COUNTRY_CN:
			set_plu_inhibit_cn();
			break;
		case COUNTRY_EU:
		default:
			set_plu_inhibit_eu();
			break;
	}
	return;
}

/**
 * @brief		scanner default 초기화 함수만 묶어놓은 함수
 * @param		void
 * @return 	void
 * @remark	
 */
void set_scanner_default(void)
{
	INT16U country;
	country = get_global_wparam(GLOBAL_AREA_VENDOR_COUNTRY);

	switch(country)
	{
		case COUNTRY_UN:
		case COUNTRY_ETC:
			break;
		case COUNTRY_KR:
			set_scanner_default_kr();
			break;
		case COUNTRY_US:
			break;
		case COUNTRY_EUT:
			break;
		case COUNTRY_RU:
			break;
		case COUNTRY_CN:
			break;
		case COUNTRY_EU:
		default:
			break;
	}
}

/**
 * @brief		barcode default 초기화 함수만 묶어놓은 함수
 * @param		void
 * @return 	void
 * @remark	
 */
void set_barcode_default(void)
{
	INT16U country;
	country = get_global_wparam(GLOBAL_AREA_VENDOR_COUNTRY);

	switch(country)
	{
		case COUNTRY_UN:
		case COUNTRY_ETC:
			break;
		case COUNTRY_KR:
			set_barcode_default_kr();
			break;
		case COUNTRY_US:
			break;
		case COUNTRY_EUT:
			break;
		case COUNTRY_RU:
			break;
		case COUNTRY_CN:
			break;
		case COUNTRY_EU:
		default:
			break;
	}
}

/**
 * @brief		label default 초기화 함수만 묶어놓은 함수
 * @param		void
 * @return 	void
 * @remark	
 */
void set_label_default(void)
{
	INT16U country;
	country = get_global_wparam(GLOBAL_AREA_VENDOR_COUNTRY);

	switch(country)
	{
		case COUNTRY_UN:
		case COUNTRY_ETC:
			break;
		case COUNTRY_KR:
			set_label_default_kr();
			break;
		case COUNTRY_US:
			break;
		case COUNTRY_EUT:
			break;
		case COUNTRY_RU:
			break;
		case COUNTRY_CN:
			break;
		case COUNTRY_EU:
		default:
			break;
	}
}


void global_set_default_code000(void) // UN(0) or ETC(6)
{
#ifdef USE_CTS_FUNCTION
	char str[65];
#endif
	set_plu_inhibit_default();
#if defined(USE_MON_EMART_DEFAULT)
	set_global_bparam(GLOBAL_DATE_SEPARATOR, '.');
	set_global_bparam(GLOBAL_DATETIME_MODE          ,0x00); // 2016.12.31 
	set_global_bparam(GLOBAL_BARCODE_STD_WID, 1);	//Weight
	set_global_bparam(GLOBAL_BARCODE_STD_CID, 2);	//Count
	set_global_bparam(GLOBAL_BARCODE_STD_NID ,2);	//PCS
	barcodefile_default_for_mon_emart();
#elif defined(USE_MONGOL_DEFAULT)
	set_global_bparam(GLOBAL_DATE_SEPARATOR, '.');
	set_global_bparam(GLOBAL_DATETIME_MODE, 0x00); // 2016.12.31 
	set_global_bparam(GLOBAL_BARCODE_STD_WID, 0);	//Weight
	set_global_bparam(GLOBAL_BARCODE_STD_CID, 0);	//Count
	set_global_bparam(GLOBAL_BARCODE_STD_NID, 0);	//PCS
#elif defined(USE_EMPORIUM_DEFAULT)
	barcodefile_default_for_Emporium();
	set_global_bparam(GLOBAL_BARCODE_STD_WID	 ,1   );
	set_global_bparam(GLOBAL_BARCODE_STD_CID	 ,2   );
	set_global_bparam(GLOBAL_BARCODE_STD_NID	 ,2   );
#elif defined(USE_VIETNAM_DEFAULT) || defined(USE_VIETNAM_LOTTEMART_DEFAULT)
	barcodefile_default_for_vietnam();
	// 상품 종류별 바코드 형식 매핑
	set_global_bparam(GLOBAL_BARCODE_STD_WID, 1);		// 무게 바코드
	set_global_bparam(GLOBAL_BARCODE_STD_CID, 2);		// 개수 바코드
	set_global_bparam(GLOBAL_BARCODE_STD_NID, 2);		// 수량 바코드
	set_global_bparam(GLOBAL_BARCODE_STD_FIX, 1);		// 정가 바코드
	set_global_bparam(GLOBAL_BARCODE_NON_WID, 1);		// 오픈 무게 바코드 (Non-PLU)
	set_global_bparam(GLOBAL_BARCODE_NON_CID, 2);		// 오픈 개수 바코드 (Non-PLU)
	set_global_bparam(GLOBAL_BARCODE_NON_NID, 2);		// 오픈 수량 바코드 (Non-PLU)
	set_global_bparam(GLOBAL_BARCODE_TOTAL_ID, 1);		// 합산판매 바코드
	set_global_bparam(GLOBAL_BARCODE_TOTAL_ID2, 1);		// 합산판매 바코드2
#endif

#ifdef USE_CTS_FUNCTION
  #if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
	sprintf(str, "Contact Service Office");
  #else
   sprintf(str, "CALL SERVICE");
  #endif
   set_global_sparam(GLOBAL_CTS_MSG, (INT8U*)str, strlen(str));
#endif
	ad_set_default(COUNTRY_UN);
}

void global_set_default_code001(void)	// KR
{
	char str[65];
	INT16S  i;
	SCROLL_MESSAGE_STRUCT scroll;
	INT32U addr;
#ifdef USE_EMART_PRODUCTION_KEY
	INT8U backup,default_val;
#endif

	//global_display_set_4digit();
	memset(str,0,4);
	sprintf(str,"kg");
	set_global_sparam(GLOBAL_WEIGHT_SYMBOL_1,(HUGEDATA INT8U *)str,4);
	sprintf(str,"g");
	set_global_sparam(GLOBAL_WEIGHT_SYMBOL_2,(HUGEDATA INT8U *)str,4);
	sprintf(str,"lb");
	set_global_sparam(GLOBAL_WEIGHT_LBSYMBOL_1,(HUGEDATA INT8U *)str,4);
	sprintf(str,"oz");
	set_global_sparam(GLOBAL_WEIGHT_LBSYMBOL_2,(HUGEDATA INT8U *)str,4);

								// YMDDxTTT
#if (defined(USE_GSMART_TRACE_DEFAULT) || defined(USE_GSMART_DEFAULT))
	set_global_bparam(GLOBAL_DATETIME_MODE          ,0x01); // 00000000	
#else
	set_global_bparam(GLOBAL_DATETIME_MODE          ,0x00); // 00000000	
#endif
	set_global_bparam(GLOBAL_DATE_SEPARATOR, '.');
	
	sprintf(str,"원");
	set_global_sparam(GLOBAL_AREA_PRICE_PRIMARYSIGN,(INT8U *)str,4);
	sprintf(str,"WON");
	set_global_sparam(GLOBAL_DISPLAY_PRICE_PRIMARYSIGN,(INT8U *)str,4);
	memset(str,0,4);
	set_global_sparam(GLOBAL_AREA_PRICE_LASTSIGN,(INT8U *)str   ,4);
	set_global_sparam(GLOBAL_DISPLAY_PRICE_LASTSIGN   ,(INT8U *)str,4);
	sprintf(str,"개");
	set_global_sparam(GLOBAL_PCS_SYMBOL,(INT8U *)str,16);
	memset(str,0,sizeof(str));

	memset(&scroll,0,sizeof(scroll));
	scroll.stype=1;
	scroll.week=0xff;
	sprintf((char *)scroll.text,"광고...");
	set_base_sparam(FLASH_SCROLL_AREA,(INT8U *)&scroll,sizeof(scroll));

	set_prt_bparam(FLASH_OFFSET_StrType	,TWO_BYTE_FONT_STR);
	set_prt_bparam(FLASH_OFFSET_DspStrType	,TWO_BYTE_FONT_STR);
	set_global_bparam(GLOBAL_MENU_FONTID 	, 3);
	set_global_bparam(GLOBAL_PLU_FONTID   	, DSP_PLU_NAME_FONT_ID);
	set_global_bparam(GLOBAL_LABEL_FONTID  	, 4);

	for (i=0; i<9; i++) set_global_bparam(GLOBAL_BARCODE_STD_WID+i,1);

	//set_global_bparam(GLOBAL_BARCODE_2, 0);
	//set_global_bparam(GLOBAL_LABEL_2, 0);

	addr = DFLASH_BASE + FLASH_SCANNER_FORMAT;
	Flash_clear(addr, 0, 10*40);	// scanner format 초기화(10개 * 40byte)

	set_plu_inhibit_default();
	set_scanner_default();
	set_barcode_default();
	set_label_default();
#if defined(USE_SETPRODUCT_SALE) || defined(USE_HYUNDAI_SET_FUNCTION)
	set_setproduct_number(0);
#endif
#if !defined (USE_GSMART_PLU) && !defined (USE_LOTTEMART_DEFAULT) && !defined(USE_EMART_DEFAULT) && !defined(USE_TOPMART_DEFAULT)
	prtsel_set_table(FIELD_ID_PACKED_TIME, 0);
	prtsel_set_table(FIELD_ID_SELL_BY_TIME, 0);
#endif
#ifdef USE_GSMART_PLU
	global_PLU_default_init();
#endif
#ifdef USE_CTS_FUNCTION
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
	sprintf(str, "연락처 : 1577-5578");
#else
   sprintf(str, "1577.5578");
#endif
   set_global_sparam(GLOBAL_CTS_MSG, (INT8U*)str, (strlen(str)));
#endif
	ad_set_default(COUNTRY_KR);
}

void global_set_default_code002(void) // US  
{
#ifndef USE_TRACE_STANDALONE
	INT16S i;
	INT8U flag;
	char str[65];

	set_global_bparam(GLOBAL_AREA_MULTIUNITFLAG	,0x10); //Added by JJANG 20080704
	set_global_bparam(GLOBAL_DATETIME_MODE       ,0xD0); // 11010000
	//SG070710
	flag=get_global_bparam(GLOBAL_SALE_SETUP_FLAG4);
#ifdef USE_CHN_FONT
	set_prt_bparam(FLASH_OFFSET_StrType	,TWO_BYTE_FONT_STR);
	set_prt_bparam(FLASH_OFFSET_DspStrType	,TWO_BYTE_FONT_STR);
	set_global_bparam(GLOBAL_MENU_FONTID 	, 3);
	set_global_bparam(GLOBAL_PLU_FONTID   	, 3);
	set_global_bparam(GLOBAL_LABEL_FONTID  	, 4);
#else
	if(flag&0x08)
	{
		set_prt_bparam(FLASH_OFFSET_StrType	,TWO_BYTE_FONT_STR);
		set_prt_bparam(FLASH_OFFSET_DspStrType	,TWO_BYTE_FONT_STR);
		set_global_bparam(GLOBAL_MENU_FONTID 	, 3);
		set_global_bparam(GLOBAL_PLU_FONTID   	, DSP_PLU_NAME_FONT_ID);
		set_global_bparam(GLOBAL_LABEL_FONTID  	, 4);
	}
#endif
	for (i=0; i<9; i++) set_global_bparam(GLOBAL_BARCODE_STD_WID+i,1);
	barcodefile_default_us();

	//SG071030
	memset(str,0,sizeof(str));
	str[0]=192;str[1]=168;str[2]=0;str[3]=2;
	set_net_sparam(NETWORK_LOCAL_IP,(HUGEDATA INT8U *)str,4);
	str[0]=192;str[1]=168;str[2]=0;str[3]=1;
	set_net_sparam(NETWORK_LOCAL_GATEWAY,(HUGEDATA INT8U *)str,4);
	str[0]=255;str[1]=255;str[2]=255;str[3]=0;
	set_net_sparam(NETWORK_LOCAL_SUBNETMASK,(HUGEDATA INT8U *)str,4);

	set_plu_inhibit_default();
#ifdef USE_CTS_FUNCTION
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
	sprintf(str, "CONTACT SERVICE OFFICE");
#else
   sprintf(str, "CALL SERVICE");
#endif
   set_global_sparam(GLOBAL_CTS_MSG, (INT8U*)str, strlen(str));
#endif
	ad_set_default(COUNTRY_US);
#endif
}

void global_set_default_code003(void)// EU LABEL
{
#ifndef USE_TRACE_STANDALONE
	char str[65];

	set_plu_inhibit_default();
  #ifdef USE_MON_EMART_DEFAULT
	set_global_bparam(GLOBAL_DATE_SEPARATOR, '.');
	set_global_bparam(GLOBAL_DATETIME_MODE          ,0x00); // 2016.12.31 
	set_global_bparam(GLOBAL_BARCODE_STD_WID, 1);	//Weight
	set_global_bparam(GLOBAL_BARCODE_STD_CID, 2);	//Count
	set_global_bparam(GLOBAL_BARCODE_STD_NID ,2);	//PCS
	barcodefile_default_for_mon_emart();
  #elif defined(USE_MONGOL_DEFAULT)
	set_global_bparam(GLOBAL_DATE_SEPARATOR, '.');
	set_global_bparam(GLOBAL_DATETIME_MODE          ,0x00); // 2016.12.31 
	set_global_bparam(GLOBAL_BARCODE_STD_WID, 0);	//Weight
	set_global_bparam(GLOBAL_BARCODE_STD_CID, 0);	//Count
	set_global_bparam(GLOBAL_BARCODE_STD_NID ,0);	//PCS
  #elif defined (USE_STOKROTKA_MART_DEFAULT)
	barcodefile_default_for_poland_stokrotka();
	set_global_bparam(GLOBAL_BARCODE_STD_WID, 1);	//Weight Barcode
  #else // #ifdef USE_MON_EMART_DEFAULT
	memset(str,0,4);
    #ifndef USE_PERSIA_DEFAULT
	sprintf(str,"$");
	set_global_sparam(GLOBAL_AREA_PRICE_PRIMARYSIGN,(INT8U *)str,4);
	memset(str,0,4);
	sprintf(str,"%c",0xa2);
	set_global_sparam(GLOBAL_AREA_PRICE_LASTSIGN,(INT8U *)str   ,4);
    #endif
	set_global_bparam(GLOBAL_DATETIME_MODE       ,0xb0);	// 10110000
	
    #ifdef USE_VIETNAM_EMART_DEFAULT
	barcodefile_default_for_Vietman_Emart();
    #endif
	#ifdef USE_INDIA_TATA_DEFAULT
	barcodefile_default_for_indiaTATA();
	tareTable_default_for_indiaTATA();
	#endif
  #endif //#ifdef USE_MON_EMART_DEFAULT
  
  #ifdef USE_CTS_FUNCTION
    #if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
	sprintf(str, "CONTACT SERVICE OFFICE");
    #else
   sprintf(str, "CALL SERVICE");
    #endif
   set_global_sparam(GLOBAL_CTS_MSG, (INT8U*)str, strlen(str));
  #endif //#ifdef USE_CTS_FUNCTION
	ad_set_default(COUNTRY_EU);
#endif //#ifndef USE_TRACE_STANDALONE
}

void global_set_default_code004(void)// EU TICKET
{
#ifndef USE_TRACE_STANDALONE
#ifdef USE_CTS_FUNCTION
	char str[65];
#endif
	set_plu_inhibit_default();
	set_global_bparam(GLOBAL_DATETIME_MODE       ,0xb0); // 10110000
#ifdef USE_CTS_FUNCTION
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
	sprintf(str, "CONTACT SERVICE OFFICE");
#else
   sprintf(str, "CALL SERVICE");
#endif
   set_global_sparam(GLOBAL_CTS_MSG, (INT8U*)str, strlen(str));
#endif
	ad_set_default(COUNTRY_EUT);
#endif
}

void global_set_default_code005(void)	// russia
{
#ifndef USE_TRACE_STANDALONE
	//SG.060210 : Default Setting is modified for RUS
#ifdef USE_CTS_FUNCTION
	char str[65];
#endif
#ifdef USE_UKRAINE_DEFAULT
#ifndef USE_CTS_FUNCTION
	char str[4];
#endif
       memset(str,0,4);
       sprintf(str,"藎?");
       set_global_sparam(GLOBAL_AREA_PRICE_PRIMARYSIGN,(INT8U *)str,4);
       memset(str,0,4);
       sprintf(str,"儒");
       set_global_sparam(GLOBAL_WEIGHT_SYMBOL_1,(INT8U *)str,4);
#endif
	set_global_bparam(GLOBAL_DATETIME_MODE       ,0xb0); // 10110000
	set_plu_inhibit_default();
#ifdef USE_CTS_FUNCTION
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
	sprintf(str, "CONTACT SERVICE OFFICE");
#else
	sprintf(str, "CALL SERVICE");
#endif
   set_global_sparam(GLOBAL_CTS_MSG, (INT8U*)str, strlen(str));
#endif
	ad_set_default(COUNTRY_RU);
#endif
}

#ifdef USE_CHINA_DEFAULT
void global_set_default_code007(void) // CN(7)
{
	INT16U i,j;
#ifdef USE_CTS_FUNCTION
	char str[65];
#endif
#ifdef USE_FIXED_PERCENT_KEY_SET
	INT8U 	discRate[5] = {90,80,70,60,50};
	INT32U	addr;
#endif
	for (i=0; i<9; i++) set_global_bparam(GLOBAL_BARCODE_STD_WID+i,1);
#ifdef USE_CHINA_DEFAULT
	barcodefile_default_for_chn();
#endif
	set_plu_inhibit_default();
// #ifdef USE_CHINA_ADD_DEFAULT
	set_global_bparam(GLOBAL_DATETIME_MODE, 0x00);
// #endif
#ifdef USE_FIXED_PERCENT_KEY_SET
	for(i=0;i<5;i++)
	{
		str[i*DISCOUNT_KEY_STRUCT_SIZE] = i+1;	//Default ID: 1~5
		str[i*DISCOUNT_KEY_STRUCT_SIZE+1] = discRate[i];	//Default Rate: 90~50%
		addr = DFLASH_BASE + FLASH_DISCOUNT_RATE_AREA + (i*DISCOUNT_KEY_STRUCT_SIZE);
		Flash_swrite(addr,(HUGEDATA INT8U *)&str[i*DISCOUNT_KEY_STRUCT_SIZE],(INT16U)DISCOUNT_KEY_STRUCT_SIZE);
	}

#endif
#ifdef USE_GSMART_PLU
	global_PLU_default_init();
#endif
#ifdef USE_CTS_FUNCTION
	sprintf(str, "CALL SERVICE");
	set_global_sparam(GLOBAL_CTS_MSG, (INT8U*)str, strlen(str));
#endif
	ad_set_default(COUNTRY_CN);
}
#endif

void global_set_pluinhibit(INT8U verify, INT8U flag)
{
	INT8U  global,sflag;
	INT8U  ret,reset;
	INT16U ptype;
	INT16S   i;

	status_scale.restrict=get_global_wparam(GLOBAL_ACTION_ENABLE);
	Startup.disable_dept=1;
	if (status_scale.restrict&FUNC_DEPTNO) Startup.disable_dept=0;
	// Verify PCS
	if (verify&VERIFY_PCS) {
		ret=get_global_bparam(GLOBAL_SALE_SETUP_FLAG3);
		status_scale.use_pcs=0;
		if (ret&0x01) status_scale.use_pcs=1;
	}

	// Verify BY-COUNT 
	if (verify&VERIFY_BYCOUNT) {
		ret=get_global_bparam(GLOBAL_SALE_SETUP_FLAG4);
		status_scale.use_bycount=0;
		if (ret&0x04) status_scale.use_bycount=1;
	}

	ret  =0;
	reset=0;
	// Verify Tare Inhibit Set
	if ((verify&VERIFY_TARE) || (verify&VERIFY_FUNC)) {
		sflag=get_global_bparam(GLOBAL_SALE_SETUP_FLAG2);
		if (sflag&0x02) {
			if (status_scale.restrict&FUNC_TARE_TBL) ret+=plu_inhibit_bycode(PTYPE_TARE_NO,0);
			else                                     ret+=plu_inhibit_bycode(PTYPE_TARE_NO,1);
			if ((!plu_check_inhibit_ptype(PTYPE_TARE_VALUE)) && (!plu_check_inhibit_ptype(PTYPE_TARE_NO))) {
				ret+=plu_inhibit_bycode(PTYPE_TARE_VALUE,1);
			}
		} else {
			ret+=plu_inhibit_bycode(PTYPE_TARE_NO,1);	// Disable Tare No
			if ((!plu_check_inhibit_ptype(PTYPE_TARE_VALUE)) && (!plu_check_inhibit_ptype(PTYPE_TARE_NO))) {
				ret+=plu_inhibit_bycode(PTYPE_TARE_VALUE,0);
			}
		}
	}
	if (verify&VERIFY_FUNC) 
	{	
		if(!flag)	 //se-hyung 070802
		{
#ifndef USE_CHINA_DEFAULT
			if (status_scale.restrict&FUNC_ORG_TBL ) {
				ret+=plu_inhibit_bycode(PTYPE_ORIGIN_NO,0);
			} else {
				ret+=plu_inhibit_bycode(PTYPE_ORIGIN_NO,1);
			}
			if (status_scale.restrict&FUNC_TAX_TBL ) {
				ret+=plu_inhibit_bycode(PTYPE_TAX_NO,0);
			} else {
				ret+=plu_inhibit_bycode(PTYPE_TAX_NO,1);
			}

#ifndef USE_TRACE_STANDALONE
			if (status_scale.restrict&FUNC_GROUP ) {
				ret+=plu_inhibit_bycode(PTYPE_GROUP_NO,0);
			} else {
				ret+=plu_inhibit_bycode(PTYPE_GROUP_NO,1);
			}
#endif

//#ifdef USE_TRACEABILITY
			if (status_scale.restrict&FUNC_TRACE   ) {
				ret+=plu_inhibit_bycode(PTYPE_TRACE_NO,0);
			} else {
				ret+=plu_inhibit_bycode(PTYPE_TRACE_NO,1);
			}
//#endif
			if (status_scale.restrict&FUNC_NUT_TBL ) {
				ret+=plu_inhibit_bycode(PTYPE_NUTRITION_NO,0);
			} else {
				ret+=plu_inhibit_bycode(PTYPE_NUTRITION_NO,1);
			}
			if (status_scale.restrict&FUNC_CUSTOM  ) {
				ret+=plu_inhibit_bycode(PTYPE_BONUS,0);
			} else {
				ret+=plu_inhibit_bycode(PTYPE_BONUS,1);
			}
#endif
		}
#ifndef USE_PLU_MULTIPLE_PRICE	//Refefence PLU 기능 사용 안 함		
		if (Startup.disable_dept) {
			ret+=plu_inhibit_bycode(PTYPE_REF_DEPT_NO,1);
			ret+=plu_inhibit_bycode(PTYPE_COUPLE_DEPT_NO,1);
			ret+=plu_inhibit_bycode(PTYPE_LINK1_DEPT_NO,1);
			ret+=plu_inhibit_bycode(PTYPE_LINK2_DEPT_NO,1);
		} else {
			if (plu_check_inhibit_ptype(PTYPE_REF_PLU_NO)) ret+=plu_inhibit_bycode(PTYPE_REF_DEPT_NO,1);
			else                             ret+=plu_inhibit_bycode(PTYPE_REF_DEPT_NO,0);
			if (plu_check_inhibit_ptype(PTYPE_COUPLE_PLU_NO)) ret+=plu_inhibit_bycode(PTYPE_COUPLE_DEPT_NO,1);
			else                             ret+=plu_inhibit_bycode(PTYPE_COUPLE_DEPT_NO,0);
		}
#endif
		ret+=plu_inhibit_bycode(PTYPE_DEPARTMENT_NO,0);
	}
	for (i=0; i<plu_n_field; i++) {
		ptype=plu_global[i].ptype;
		ptype&=0x7fff;
		sflag=plu_inhibit_impossible(ptype);
		if (sflag==1) {
			if (plu_global[i].inhibit) {
				ret+=plu_inhibit_bycode(ptype,0);
			}
		}
	}
	global=get_global_bparam(GLOBAL_BARCODE_PRIORITY);
	if (global) {
		ret+=plu_inhibit_bycode(PTYPE_BARCODE_NO,1);
	} else {
		ret+=plu_inhibit_bycode(PTYPE_BARCODE_NO,0);
	}
	global=get_global_bparam(GLOBAL_LABEL_PRIORITY);
	if (global) {
		ret+=plu_inhibit_bycode(PTYPE_LABEL_NO,1);
	} else {
		ret+=plu_inhibit_bycode(PTYPE_LABEL_NO,0);
	}
	global = get_global_bparam(GLOBAL_AREA_MULTIUNITFLAG);
	GlbOper.EnableKgLb = OnOff(global&0x02);
	if (GlbOper.EnableKgLb) {
		ret+=plu_inhibit_bycode(PTYPE_UNIT_WEIGHT,0);
	} else {
		ret+=plu_inhibit_bycode(PTYPE_UNIT_WEIGHT,1);
	}
#if defined(USE_USA_INGREDIENT) || defined(USE_NORTH_AMERICA_INGREDIENT)
	ret+=plu_inhibit_bycode(PTYPE_NO_OF_LINK_PLU, 1);
	ret+=plu_inhibit_bycode(PTYPE_LINK1_DEPT_NO, 1);
	ret+=plu_inhibit_bycode(PTYPE_LINK2_DEPT_NO, 1);
	ret+=plu_inhibit_bycode(PTYPE_LINK1_PLU_NO, 1);
	ret+=plu_inhibit_bycode(PTYPE_LINK2_PLU_NO, 1);
#elif defined(USE_PLU_MULTIPLE_PRICE)	//Direct Discount의 Second Target, Price 항상 사용
	ret+=plu_inhibit_bycode(PTYPE_LINK1_PLU_NO, 0);
	ret+=plu_inhibit_bycode(PTYPE_LINK2_PLU_NO, 0);
#else
	if (plu_check_inhibit_ptype(PTYPE_NO_OF_LINK_PLU)) {
		ret+=plu_inhibit_bycode(PTYPE_LINK1_DEPT_NO, 1);
		ret+=plu_inhibit_bycode(PTYPE_LINK2_DEPT_NO, 1);
		ret+=plu_inhibit_bycode(PTYPE_LINK1_PLU_NO, 1);
		ret+=plu_inhibit_bycode(PTYPE_LINK2_PLU_NO, 1);
	} else {	
		ret+=plu_inhibit_bycode(PTYPE_LINK1_DEPT_NO, 0);
		ret+=plu_inhibit_bycode(PTYPE_LINK2_DEPT_NO, 0);
		ret+=plu_inhibit_bycode(PTYPE_LINK1_PLU_NO, 0);
		ret+=plu_inhibit_bycode(PTYPE_LINK2_PLU_NO, 0);
	}
#endif
	if (ret) {
		reset=1;
	}
	if (reset) {
		plu_ptype_reset();
	}
}

void global_set_default_parameter(INT8U pd, INT8U flagKeepDefaultCaption)
{
	INT32U addr;
//	char version[3];	
	char date[65];
	SCROLL_MESSAGE_STRUCT scroll;

//	s_addr = DFLASH_BASE;
//	strcpy(version,FLASH_STRUCT_VERSION);
//	version[0] = '1';
//	version[1] = '0';
//	Flash_swrite((INT32U)(s_addr+FLASH_STR_VERSION),(HUGEDATA INT8U*)version,2);
	date[0]=03; date[1]=8; date[2]=1; date[3]=12; date[4]=0 ; date[5]=0;
	addr=DFLASH_BASE;
	addr+=FLASH_CREATE_DATE;
	Flash_swrite(addr,(HUGEDATA INT8U*)date,6);
#ifndef USE_PERSIA_DEFAULT
    global_set_date();
#endif
	global_set_number();
#ifdef USE_DISPLAY_STYLE_3DIGIT
	global_display_set_3digit();
#else
	global_display_set_4digit(pd, COUNTRY_UN);
  #endif
#ifdef USE_PERSIA_DEFAULT
  #ifdef USE_EXTEND_PRICE_PRIMARYSIGN
    memset(date,0,5);
	sprintf(date,Caption_Price);
    set_global_sparam(GLOBAL_AREA_PRICE_PRIMARYSIGN,(INT8U *)date,5);
	set_global_sparam(GLOBAL_DISPLAY_PRICE_PRIMARYSIGN,(INT8U *)date,5);
  #else
    memset(date,0,4);
	sprintf(date,Caption_Price);
    set_global_sparam(GLOBAL_AREA_PRICE_PRIMARYSIGN,(INT8U *)date,4);
	set_global_sparam(GLOBAL_DISPLAY_PRICE_PRIMARYSIGN,(INT8U *)date,4);
  #endif
#else
	if (!flagKeepDefaultCaption)
	{
		global_set_date();

		memset(date,0,4);
		sprintf(date,"kg");
		set_global_sparam(GLOBAL_WEIGHT_SYMBOL_1,(HUGEDATA INT8U *)date,4);
		sprintf(date,"g");
		set_global_sparam(GLOBAL_WEIGHT_SYMBOL_2,(HUGEDATA INT8U *)date,4);
		sprintf(date,"lb");
		set_global_sparam(GLOBAL_WEIGHT_LBSYMBOL_1,(HUGEDATA INT8U *)date,4);
		sprintf(date,"oz");
		set_global_sparam(GLOBAL_WEIGHT_LBSYMBOL_2,(HUGEDATA INT8U *)date,4);
		memset(date,0,4);
		sprintf(date,"$");
		set_global_sparam(GLOBAL_AREA_PRICE_PRIMARYSIGN,(INT8U *)date,4);
		set_global_sparam(GLOBAL_DISPLAY_PRICE_PRIMARYSIGN,(INT8U *)date,4);
		sprintf(date,"%c",0xa2);
		set_global_sparam(GLOBAL_AREA_PRICE_LASTSIGN   ,(INT8U *)date,4);
		set_global_sparam(GLOBAL_DISPLAY_PRICE_LASTSIGN   ,(INT8U *)date,4);
		sprintf(date,"%s","PCS");
		set_global_sparam(GLOBAL_PCS_SYMBOL,(INT8U *)date,16);
		memset(date,0,sizeof(date));
		
 #if defined(USE_VIETNAM_LOTTEMART_DEFAULT) || defined(USE_VIETNAM_EMART_DEFAULT) || defined(USE_VIETNAM_DEFAULT)
		date[0]=',';
 #else
		date[0]='.';
 #endif
		set_global_sparam(GLOBAL_DECIMAL_SIGN,(INT8U *)date,1);
		date[0]='Y';
		set_global_sparam(GLOBAL_QUESTION_YES,(INT8U *)date,4);
		date[0]='N';
		set_global_sparam(GLOBAL_QUESTION_NO ,(INT8U *)date,4);
	}

	memset(&scroll,0,sizeof(scroll));
	scroll.stype=1;
	scroll.week=0xff;	// Added by CJK 20050202
	sprintf((char *)scroll.text,"Scale Mart...");
	set_base_sparam(FLASH_SCROLL_AREA,(INT8U *)&scroll,sizeof(scroll));

#endif	//USE_PERSIA_DEFAULT
	set_prt_bparam(FLASH_OFFSET_StrType	,ONE_BYTE_FONT_STR);
	set_prt_bparam(FLASH_OFFSET_DspStrType	,ONE_BYTE_FONT_STR);
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
#ifdef USE_8_16_FONT_MENU_DISPLAY
	set_global_bparam(GLOBAL_MENU_FONTID	, DSP_MEDIUM_FONT_ID);
#else
#ifdef USE_CHN_FONT
	set_global_bparam(GLOBAL_MENU_FONTID	, DSP_MEDIUM_FONT_ID);
#else
	set_global_bparam(GLOBAL_MENU_FONTID	, DSP_SMALL_FONT_ID);
#endif	//USE_CHN_FONT
#endif	//USE_8_16_FONT_MENU_DISPLAY
#endif	//_USE_LCD20848_
	set_global_bparam(GLOBAL_PLU_FONTID   	, DSP_PLU_NAME_FONT_ID);
	set_global_bparam(GLOBAL_LABEL_FONTID 	, 4);
	set_nvram_bparam(NVRAM_DISPLAY_DATETIME_MODE	,0x01);
	set_global_bparam(GLOBAL_DISPLAY_DATETIME_X	,120);
	set_global_bparam(GLOBAL_DISPLAY_DATETIME_Y	,0);
	//set_global_bparam(GLOBAL_DECIMAL_SIGN	,'.');
	//set_global_bparam(GLOBAL_DECIMAL_SIGN	,'.');

	set_global_bparam(GLOBAL_DATETIME_MODE          ,0xf0); // 10010000
	set_global_bparam(GLOBAL_SALEDATE_TYPE       ,0   );

	set_global_bparam(GLOBAL_AREA_MULTIUNITFLAG  ,0   );
	set_global_bparam(GLOBAL_BARCODE_STD_WID     ,0   );
	set_global_bparam(GLOBAL_BARCODE_STD_CID     ,0   );
	set_global_bparam(GLOBAL_BARCODE_STD_NID     ,0   );
	set_global_bparam(GLOBAL_BARCODE_STD_FIX     ,0   );
	set_global_bparam(GLOBAL_BARCODE_NON_WID     ,0   );
	set_global_bparam(GLOBAL_BARCODE_NON_CID     ,0   );
	set_global_bparam(GLOBAL_BARCODE_NON_NID     ,0   );
	set_global_bparam(GLOBAL_BARCODE_TOTAL_ID    ,0   );
	set_global_bparam(GLOBAL_BARCODE_TOTAL_ID2 ,0   );

	set_global_wparam(GLOBAL_ADDUP_MESSAGE_HEAD  ,0);
	set_global_wparam(GLOBAL_ADDUP_MESSAGE_TAIL  ,0);
	set_global_wparam(GLOBAL_TICKET_MESSAGE_HEAD ,0);
	set_global_wparam(GLOBAL_TICKET_MESSAGE_TAIL ,0);
#ifdef USE_GERMANY_DEFAULT
	set_global_bparam(GLOBAL_PRINT_OPER_FLAG  ,  0xC8);
	set_global_bparam(GLOBAL_LOG_FLAG		  ,  0x00);
	set_global_bparam(GLOBAL_TIME_CLERKLOGOUT ,  0x00); 
#else
	set_global_bparam(GLOBAL_PRINT_OPER_FLAG  ,  0x08);
#endif	//USE_GERMANY_DEFAULT
	set_global_bparam(GLOBAL_PRINT_CLERK_FLAG2,  0x00);
	set_global_bparam(GLOBAL_PRINT_ADDUP_FLAG1,  0x0e); // 1110
	set_global_bparam(GLOBAL_PRINT_ADDUP_FLAG2,  0x01);

//DEL070130	set_global_bparam(GLOBAL_KEY_TIMEOUT_KEYPRESS,10);
	set_global_wparam(GLOBAL_KEY_TIMEOUT_ACTION  ,KS_PLU);
	set_global_wparam(GLOBAL_AREA_PRICE_ROUND,    0   );
	set_global_bparam(GLOBAL_BARCODE_PRICE_CUT,   0   );

	set_global_bparam(GLOBAL_TAX_VALUE        ,  0x01 );		
	set_global_bparam(GLOBAL_MISC_TAX_VALUE   ,  0 );		//Added by JJANG 20070619 초기세팅값 0 : MISC TAX 사용안함
	set_global_lparam(GLOBAL_AREA_RS232C_BAUD,BAUD_RATE_57600);
    
#if defined(USE_NHMART_SAFE_DEFAULT)
	set_global_lparam(GLOBAL_BARCODE_2, 2);		//2번 바코드 2사용
#elif defined(USE_GSMART_TRACE_DEFAULT)
	set_global_lparam(GLOBAL_BARCODE_2, 3);		//2번 바코드 3사용
#else
	set_global_lparam(GLOBAL_BARCODE_2, 0);
#endif
	set_global_bparam(GLOBAL_LABEL_2, 0);
#ifdef USE_LOTTEMART_DEFAULT
	set_global_bparam(GLOBAL_SALE_ONLINE_LABEL,66);
#endif
#ifdef USE_HYUNDAI_SET_FUNCTION
	set_global_bparam(GLOBAL_SALE_SET_LABEL,71);
#endif

//DEL CJK070321	set_global_wparam(GLOBAL_TIME_CASHDRAWEROPEN ,10 );
//DEL CJK070321	set_global_bparam(GLOBAL_CASHDRAWER_SENSOR   ,0  );
// Data
	memset(date,0,DEALER_PASSWORD_LEN);
#ifdef USE_LOTTEMART_DEFAULT
	strcpy(date,"8341");
#else
	strcpy(date,"0419");
#endif
	set_base_sparam(FLASH_START_PASSWORD,(INT8U *)date,6); // 7 byte Write

//Added by JJANG 20080229
//내수에서는 이마트 세팅을 따라간다.  
//Weight , Count , PCS , Fixed price
	memset(date,0,4);
#ifdef USE_NHMART_SAFE_DEFAULT
	sprintf(date,"1000");
	set_nvram_wparam(NVRAM_NH_SAFE_INDIV_INDEX, 8);	//1~7은 기존 개체 호출키로 사용됨.
#else    
	#if defined(USE_VIETNAM_EMART_DEFAULT) || defined(USE_SHINSEGAE_DEFAULT)
		sprintf(date,"0111");
	#else
		sprintf(date,"2110");
	#endif
#endif	
    set_global_sparam(GLOBAL_PLU_TYPE_AREA,(INT8U *)date,4);
	set_base_bparam(FLASH_TICKET_HEAD_MESSAGE_AREA, 0);
	set_base_bparam(FLASH_TICKET_TAIL_MESSAGE_AREA, 0);
	//Modified by JJANG 20080121
	set_nvram_wparam(NVRAM_SEND_REMAINDED_HEAD,0);
	set_nvram_wparam(NVRAM_SEND_REMAINDED_TAIL,0);
	set_nvram_wparam(NVRAM_TOTAL_SEND_SALE,0);
	set_nvram_wparam(NVRAM_TOTAL_SEND_CHEANGE,0);

	set_nvram_bparam(NVRAM_SLAVECHECK_FLAG,0);		//Added by JJANG 20080130
	set_nvram_bparam(NVRAM_TRANSACTION_CLERK,0);
	set_nvram_bparam(NVRAM_TRANS_BACKUP_FLAG, 0);
	set_nvram_lparam(NVRAM_BUSINESS_DATE_FLAG,0);

	set_nvram_bparam(NVRAM_PLU_LASTNUMBER     ,0);
	strcpy(date,"BBBBBB\0");
	set_global_sparam(GLOBAL_LABEL_SERIAL_FORM,(INT8U *)date,32);
#ifdef USE_PERSIA_DEFAULT
    strcpy(date,"AAAAA\0");
#else
    strcpy(date,"%N%O%:AAAAA\0");
#endif
	set_global_sparam(GLOBAL_TICKET_SERIAL_FORM,(INT8U *)date,32);
	set_nvram_lparam(NVRAM_COUNTER_TICKET,0);
	set_nvram_lparam(NVRAM_COUNTER_TRANSACTION,0);
	set_nvram_bparam(NVRAM_SELF_KEY_BLOCK_ON, 0);
#ifdef USE_QR_ADDTOTAL
	set_nvram_bparam(NVRAM_QR_ADD_COUNT, 0);
	barcodefile_QR_config_default();
#endif
	set_nvram_wparam(NVRAM_REOPEN_HEAD,0);
	strcpy(date,"FORM1\0");
	set_base_sparam(FLASH_EXT_PRT_LABEL_NAME, (INT8U *)date, 7);
	set_nvram_lparam(NVRAM_COUNTER_GROUP_Z+4*0, 1L);
	set_nvram_lparam(NVRAM_COUNTER_GROUP_Z+4*1, 1L);
	print_inhibit_init();	// Added by CJK 20041222

#ifdef USE_FUNCTION_KEY_BLOCK
  #ifdef USE_LOTTESUPER_PROHIBIT_CHANGE_PRICE				// 롯데슈퍼 요청 매가통제용 기능 (세일모드에서 관련 키 기능 막음, 1110, 1120 Menu 비밀번호로 막음)
	set_global_bparam(GLOBAL_FUNCTION_KEY_BLOCK,26);		// 가격정정, %할인, 정가 키 막음			0001 1010 (26)
	set_global_bparam(GLOBAL_FUNCTION_KEY_BLOCK+1,11);		// 합산정정, (-)할인, 합산발행 키 막음	0000 1011 (11)
	set_global_bparam(GLOBAL_FUNCTION_KEY_BLOCK+2,3);		// 날짜인쇄, 개수 키 막음					0000 0011 (3)
  #elif defined(USE_EMART_DEFAULT)
  	set_global_bparam(GLOBAL_FUNCTION_KEY_BLOCK,2);			// 유효일 키 막음			0000 0010 (2)
  	set_global_bparam(GLOBAL_FUNCTION_KEY_BLOCK+1,1);		// 후방유효일 키 막음		0000 0001 (1)
  #else
	set_global_bparam(GLOBAL_FUNCTION_KEY_BLOCK,0);
	set_global_bparam(GLOBAL_FUNCTION_KEY_BLOCK+1,0);
	set_global_bparam(GLOBAL_FUNCTION_KEY_BLOCK+2,0);
  #endif
#endif
}

//#define MAX_INDIRECT_INGREDIENT_CHN_QTY 680
void 	get_max_pluno(void)
{
	INT16U icc,xr;
	long   sl;
//	INT16S    rc;

//	rc = plu_get_recordsize();
#if (ROM_COUNTRY == ROM_RUS)
	INGREDIENT_STRUCT_SIZE = DEFAULT_INGREDIENT_SIZE;
#else
	INGREDIENT_STRUCT_SIZE = get_base_wparam(FLASH_NUMBER_INGRIDIENT_SIZE);
	if (INGREDIENT_STRUCT_SIZE > MAX_INGREDIENT_SIZE || INGREDIENT_STRUCT_SIZE < MIN_INGREDIENT_SIZE)
	{
		INGREDIENT_STRUCT_SIZE = DEFAULT_INGREDIENT_SIZE;
		set_base_wparam(FLASH_NUMBER_INGRIDIENT_SIZE, INGREDIENT_STRUCT_SIZE);
	}
#endif

//	MAX_PLU_small = PLU_BASIC_AREA_QTY;
//	MAX_PLU_large = PLU_BASIC_AREA_QTY + PLU_EXTER_AREA_QTY;
//	MAX_INDIRECT_INGREDIENT_small = INGREDIENT_BASIC_AREA_QTY;
//	MAX_INDIRECT_INGREDIENT_large = INGREDIENT_BASIC_AREA_QTY + INGREDIENT_EXTER_AREA_QTY;

	MAX_PLU_small = PLU_BASIC_AREA_QTY;
	MAX_PLU_large = PLU_BASIC_AREA_QTY + PLU_EXTER_AREA_QTY;
#ifdef USE_EXTEND_8M_DFLASH
    MAX_PLU_extend8M = PLU_BASIC_AREA_QTY + PLU_EXTER_AREA_QTY + PLU_EXTER2_AREA_QTY; // Serial Flash Memory 8M확장에 따른 PLU 확장 (PLU 12,000 개)
#endif

	// Basic Ingredient No
	sl = INGREDIENT_BASIC_AREA_SIZE;	//INGREDIENT_BASIC_AREA_QTY*DEFAULT_INGREDIENT_SIZE (1000*512)
	sl /= INGREDIENT_STRUCT_SIZE;
	MAX_INDIRECT_INGREDIENT_small = (sl/10l)*10l;	// 10단위
#ifdef USE_DIRECT_INGREDIENT
	MAX_DIRECT_INGREDIENT_small = INGREDIENT_TEXT_BASIC_AREA_QTY;	// 고정사이즈(parameter 995 설정 적용 안됨)
#endif

	// External Ingredient No
	sl = INGREDIENT_EXTER_AREA_SIZE;
	sl /= INGREDIENT_STRUCT_SIZE;
	MAX_INDIRECT_INGREDIENT_large = (sl/10l)*10l;
	MAX_INDIRECT_INGREDIENT_large += MAX_INDIRECT_INGREDIENT_small;
#ifdef USE_DIRECT_INGREDIENT
	MAX_DIRECT_INGREDIENT_large = INGREDIENT_TEXT_EXTER_AREA_QTY;	// 고정사이즈(parameter 995 설정 적용 안됨)
	MAX_DIRECT_INGREDIENT_large += MAX_DIRECT_INGREDIENT_small;
#endif
#if defined(USE_9999_INDIRECT_INGREDIENT) || defined(USE_INGREDIENT_CODE_4DIGIT)
	MAX_INGREDIENT_INDEX_NUMBER = MAX_INGREDIENT_INDEX_NUM;	 // 10000
#endif

//	MAX_INGREDIENT_TEXT_NUMBER = MAX_DIRECT_INGREDIENT_small;	// 일단 기본메모리 영역만

	icc = FlashCheckMem();
#ifdef USE_EXT_SETTING_MEM
	if (Data_Save_Restore(2)) icc=0x30;
#endif
	// Only Basic Memory
	if (icc == 0x30)	
	{
		MAX_PLU_NO = MAX_PLU_small;
		MAX_INGREDIENT_NO = MAX_INDIRECT_INGREDIENT_small;
#ifdef USE_DIRECT_INGREDIENT
		MAX_INGREDIENT_TEXT_NUMBER = MAX_DIRECT_INGREDIENT_small;
#endif
		MAX_PLU_REPORT_NO = MAX_PLU_small;

	}
	// Include Extenal Memory
	else
	{
#ifdef USE_EXTEND_8M_DFLASH
        MAX_PLU_NO = MAX_PLU_extend8M;
#else
		MAX_PLU_NO = MAX_PLU_large;
#endif
		MAX_INGREDIENT_NO = MAX_INDIRECT_INGREDIENT_large;
#ifdef USE_DIRECT_INGREDIENT
		MAX_INGREDIENT_TEXT_NUMBER = MAX_DIRECT_INGREDIENT_large;
#endif
		MAX_PLU_REPORT_NO = PLU_REPORT_QTY_1;	// para 625 : 0
		//Modified by JJANG 20080124
		//러시아의 경우 추후 작업필요!!! PLU_REPORT 수량조절
		//파라미터 625세팅에 따라 최대 저장할수 있는 PLU REPORT 개수 제한
		if(Startup.use_ext_nvram == 1)	// para 625
		{			 
#ifndef USE_EXTEND_8M_DFLASH
            MAX_PLU_NO = MAX_PLU_large;
#endif
			MAX_INGREDIENT_NO = MAX_INDIRECT_INGREDIENT_small;
#ifdef USE_DIRECT_INGREDIENT
			MAX_INGREDIENT_TEXT_NUMBER = MAX_DIRECT_INGREDIENT_small;
#endif
			MAX_PLU_REPORT_NO = PLU_REPORT_QTY_2;	//7500	// para 625 : 1
		}
		else if(Startup.use_ext_nvram == 2)
		{	
#ifndef USE_EXTEND_8M_DFLASH
            MAX_PLU_NO = MAX_PLU_small;	 	//3000
#endif
			MAX_INGREDIENT_NO = MAX_INDIRECT_INGREDIENT_small;
#ifdef USE_DIRECT_INGREDIENT
			MAX_INGREDIENT_TEXT_NUMBER = MAX_DIRECT_INGREDIENT_small;
#endif
			MAX_PLU_REPORT_NO = MAX_PLU_small;		// para 625 : 2
		}
	}

//SGTEST ONLY
//sprintf(MsgBuf,"MAXINGNO=%d",MAX_INGREDIENT_NO);
//MsgOut(MsgBuf);

	xr = get_base_wparam(FLASH_NUMBER_INGRIDIENT);
	if (xr!=MAX_INGREDIENT_NO) set_base_wparam(FLASH_NUMBER_INGRIDIENT,MAX_INGREDIENT_NO);
    
#ifdef USE_TRACE_STANDALONE
	//read GLOBAL_SALE_SETUP_FLAG15 & 0x03	
	SaveTraceabilityNo = get_global_bparam(GLOBAL_SALE_SETUP_FLAG15);
	SaveTraceabilityNo = SaveTraceabilityNo & 0x03;
	xr = 0;
#ifdef USE_CHECKING_HARMFUL_MEAT
	xr = get_global_bparam(GLOBAL_SALE_SETUP_FLAG13); //위해 개체수량 0 일 경우 사용하지 않음 
#ifdef USE_IMPORT_MEAT	// 독립형 이력 저울  (default : 0)
	if(xr > 99) 
	{		
		xr = 0;
		set_global_bparam(GLOBAL_SALE_SETUP_FLAG13, xr);
	}
#else
	if((xr > 99) || (xr < 10))
	{
	#ifdef USE_EMART_DEFAULT
		xr = 99;
	#else
		xr = 20;
	#endif
		set_global_bparam(GLOBAL_SALE_SETUP_FLAG13, xr);
	}
#endif
/*	if(xr & 0x04) //위해 리스트 기능 on
	{
		xr = get_global_bparam(GLOBAL_SALE_SETUP_FLAG13); //위해 리스트 수량(default 20, 이마트 99)
		#ifdef USE_IMPORT_MEAT  // 독립형 이력 저울  (default : 0)
		if(xr > 99)	xr = 0;
		#else
		if((xr > 99) || (xr < 10))
		{
			xr = 20;
		#ifdef USE_EMART_DEFAULT // E-MART
			xr = 99;
		#endif
		}
		#endif
	}
	else //위해 리스트 기능 off
	{
		xr = 0;
	}
	set_global_bparam(GLOBAL_SALE_SETUP_FLAG13, xr);	*/
#endif
	MaxIndividualNo = MAX_INDIVIDUAL_NO - xr;
#endif    
}

//Added by JJANG 20080312 PAMETER 625 세팅변경 시 체크루틴
void global_load_ext_nvram(void) 
{
	INT8U nv_flag, sb_flag, sr_flag;
	INT16U icc;
	INT32U temp;
	INT16U tr_record_size;
	INT16U tr_max_no;

	nv_flag = get_global_bparam(GLOBAL_SALE_SETUP_FLAG8)&0x06;	//PARAMETER 625
	nv_flag = nv_flag>>1;

	sb_flag = get_global_bparam(GLOBAL_SALE_SETUP_FLAG9)&0x10;	//Parameter 715
	sb_flag = sb_flag>>4;

	sr_flag = get_global_bparam(GLOBAL_SALE_SAVE_FLAG);		//Parameter 620
	sr_flag = (sr_flag >> 3)&0x0F;	
	// 0:none, 1:basic, 2:basic+indiv.(Korea), 3:full(Korea), 4:Emart,NH(Korea), 6:ImportMeat(KOR), 7:CHN_CART, 8:CHN_TRACE	

	Startup.use_ext_nvram = 0;
	Startup.addr_paysend_buf = NVRAM_TRANS_PAY; 
	Startup.max_paysend_buf = MAX_TRANSACTION_PAY;

	if (sr_flag == 4 || sr_flag == 6)
	{
#ifdef USE_INDIA_FUNCTION
		tr_record_size = MAX_TRANSACTION_TYPE8_SALE_SIZE;
		tr_max_no = MAX_TRANSACTION_TYPE8_SALE;
#else
		tr_record_size = MAX_TRANSACTION_TYPE4_SALE_SIZE;
		tr_max_no = MAX_TRANSACTION_TYPE4_SALE;
#endif
	}
#ifdef USE_CHN_CART_SCALE
	else if (sr_flag == 7)
	{
		tr_record_size = MAX_TRANSACTION_TYPE7_SALE_SIZE;
		tr_max_no = MAX_TRANSACTION_TYPE7_SALE;
	}
#endif
#ifdef USE_SEND_TRACE_PRICE
	else if (sr_flag == 8)
	{
		tr_record_size = MAX_TRANSACTION_TYPE10_SALE_SIZE;
		tr_max_no = MAX_TRANSACTION_TYPE10_SALE;
	}
#endif
#ifdef USE_SEND_TICKET_DATA
    else if (sr_flag == 9)
    {
        tr_record_size = MAX_TRANSACTION_TYPE4_SALE_SIZE;
        tr_max_no = MAX_TRANSACTION_TYPE4_SALE;
    }    
#endif
#ifdef USE_MEAT_BOX_PROTOCOL
	else if (sr_flag == 11)
	{
		tr_record_size = MAX_TRANSACTION_TYPE11_SALE_SIZE;
		tr_max_no = MAX_TRANSACTION_TYPE11_SALE;
	}
#endif
#ifdef USE_HYUNDAI_MEAT_FUNCTION
	else if (sr_flag == 14)
	{
		tr_record_size = MAX_TRANSACTION_TYPE14_SALE_SIZE;
		tr_max_no = MAX_TRANSACTION_TYPE14_SALE;
	}	
#endif
	else
	{
		tr_record_size = MAX_TRANSACTION_TYPE1_SALE_SIZE;
		tr_max_no = MAX_TRANSACTION_TYPE1_SALE;
	}
	
#ifdef USE_CHN_CART_SCALE
	Startup.addr_send_buf = NVRAM_SALES_LOG;
	Startup.max_send_buf = 10000;
	Startup.max_realsend_buf = 600;
#else
	//if(GlbOper.PluReportType)
	if(sb_flag)	//Parameter 715 : Y, PLU정산영역에 사용함
	{
		Startup.addr_send_buf = NVRAM_PLUSUM_HEADINDEX;
		temp = 3000L*NVRAM_PLU_SALE_SUMMARY_SIZE/tr_record_size;
		temp = temp / 100L * 100L;	// 2600	or 3000
		Startup.max_send_buf = temp;
		Startup.max_realsend_buf = 600;  
	}
	else
	{
#if defined(USE_INDIA_FUNCTION) || defined(USE_SEND_TICKET_DATA)
//#ifdef USE_QR_ADDTOTAL
		if (RealtimeAddDataFlag)
			Startup.addr_send_buf = NVRAM_SEND_TRANSACTION;
		else
		Startup.addr_send_buf = NVRAM_SEND_ADD_TRANSACTION;
//#endif
#else
		Startup.addr_send_buf = NVRAM_SEND_TRANSACTION;
#endif
		Startup.max_send_buf = tr_max_no;
		Startup.max_realsend_buf = MAX_TRANSACTION_REALTIME_SALE;  
	}

	icc = FlashCheckMem();
	// if not Basic Memory(0x30:basic, 0xf0:external)
	if (icc != 0x30)	
	{
		if (sb_flag)	//Parameter 715 : Y, PLU정산영역에 사용함
		{
			temp = 0x80000 / tr_record_size;
			temp = temp / 100L * 100L;
			Startup.max_send_buf += temp;	// 2600 + 5800 = 8400
		}
		else		//Parameter 715 : N
		{
			// Include Extenal Memory, PARA625 : 1
			if (nv_flag == 1)	
			{
				Startup.use_ext_nvram = 1;
				Startup.addr_send_buf = NVEXT1_SEND_TRANSACTION;
				Startup.max_send_buf = MAX_EXT_TRANSACTION_SALE;
				Startup.max_realsend_buf = MAX_EXT_TRANSACTION_REALTIME_SALE;  
				Startup.addr_paysend_buf = NVEXT1_TRANS_PAY; 	
				Startup.max_paysend_buf = MAX_EXT_TRANSACTION_PAY;	
			}
			// Include Extenal Memory, PARA625 : 2	 
			else if (nv_flag == 2)
			{
				Startup.use_ext_nvram = 2;
			}
		}
	}
#endif
}


extern INT8U PrtFontType;
INT16S  global_load_caption(INT16S q, char *str, char *dest, INT16S max)
{
	INT16S i,k,l;
	l = strlen(str);
	k = 0; 
	for (i = q; i < l; i++) 
	{
		if(str[i] != ' ') break; 
	}
	for (; i < l; i++) 
	{
		if (str[i] == ',') break;
		dest[k++] = str[i];
		if (k == max) break;
	}
	dest[k] = 0;
	for (; i < l; i++)
	{
		if (str[i] == ',') 
		{
			i++; 
			break;
		}
	}
	return i;
}

void global_load_parameter(INT8U inhibit)
{
////	FW_VERSION fversion;
	INT8U cc;
	INT8U flag;
	INT16S  i,q;
	CAPTION_STRUCT cap;
#ifdef USE_EMART_PRODUCTION_KEY
	INT8U flag_add;
#endif

	Startup.version[0] =Flash_bread(Startup.start_addr+FLASH_STR_VERSION   );
	Startup.version[1] =Flash_bread(Startup.start_addr+FLASH_STR_VERSION+1 );
////	version_get(&fversion);

//	global_set_pluinhibit();

////	Startup.fwversion[0] =fversion.version;
////	Startup.fwversion[1] =fversion.reversion[0];
////	Startup.fwversion[2] =fversion.reversion[1];

	Startup.number_scale    =get_base_wparam(FLASH_NUMBER_SCALE);
#ifdef NOT_USE_NETSET_LOCK
	status_scale.scale_lock = 0;
#else
	status_scale.scale_lock =get_nvram_bparam(NVRAM_SCALE_LOCK);
#endif
	Startup.scale_type      = get_global_bparam(GLOBAL_AREA_SCALETYPE);
	Startup.country = get_global_wparam(GLOBAL_AREA_VENDOR_COUNTRY);
	Startup.scroll_timeout=get_global_wparam(GLOBAL_SCROLL_WAIT_TIME);
#ifdef USE_EMART_PRODUCTION_KEY
	flag_add = get_global_bparam(GLOBAL_SALE_SETUP_FLAG19);
	Operation.checkin = OnOff(flag_add&0x80); //생산등록 사용여부 플래그
#endif
	if (Startup.scroll_timeout>36000) Startup.scroll_timeout=0;
	//GLOBAL_DISPLAY_INDICATOR_NO=12;
//	GLOBAL_DISPLAY_INDICATOR_NO=15;	// Added by CJK 20060531
//DEL080220	GLOBAL_DISPLAY_INDICATOR_NO=16;	// Modified by JJANG 20070828
	global_load_ext_nvram();	//Added by JJANG 20080312
	switch (Startup.country) {
		case COUNTRY_US:
			  //GLOBAL_DISPLAY_INDICATOR_NO=15; // 13->15 : Modified by CJK 20060109
#ifdef USE_CHN_FONT
		  	  Startup.menu_type=0;
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
			  Startup.line_max = 2;
#else
			  Startup.line_max = 0;
#endif
			  PrtFontType = 1;
#else
			  flag=get_global_bparam(GLOBAL_SALE_SETUP_FLAG4);
			  if(flag&0x08)		//USE Korean for USA
			  {
			  	  Startup.menu_type=0;
			  	  Startup.line_max =2; 
			  	  PrtFontType = 1;	
			  }
			  else
			  {
			  	  Startup.menu_type=1;
				  Startup.line_max =3;
				  PrtFontType = 0;
			  }              
#endif
			  break;
		case COUNTRY_KR:
			  Startup.menu_type=0;
			  Startup.line_max =2; 
			  PrtFontType = 1;	
			  break;
		default : 
#ifdef USE_8_16_FONT_MENU_DISPLAY
			Startup.menu_type = 0;
			Startup.line_max = 2;
			PrtFontType = 0;
#else
			flag=get_global_bparam(GLOBAL_SALE_SETUP_FLAG4);	
			if(flag&0x08)		//USE Korean for USA 설정으로 영공의 경우에도 Korean 폰트 출력 가능
			{
				Startup.menu_type=1;
				Startup.line_max =3;
				PrtFontType = 1;
			}
			else
			{
				Startup.menu_type=1;
				Startup.line_max =3;
				PrtFontType = 0;

			}
#endif
            break;
	}
#ifdef USE_CHN_FONT//USE_2BYTE_FONT_MENU
	Startup.menu_type = 0;
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
	Startup.line_max = 2;
#else
	Startup.line_max = 0;
#endif
	PrtFontType = 1;
#endif    
//	NUM_MENU_PER_PAGE      = Startup.line_max;
#if defined(USE_SINGLE_LINE_GRAPHIC)
	Startup.menu_type = 0;
	Startup.line_max = 1;
#endif

	get_max_pluno();
	//INGREDIENT_STRUCT_SIZE = get_base_wparam(FLASH_NUMBER_INGRIDIENT_SIZE);
//	MAX_INGREDIENT_NO      = get_base_wparam(FLASH_NUMBER_INGRIDIENT);
//sprintf(gmsg,"[%d]",MAX_INGREDIENT_NO);
//MsgOut(gmsg);
	cc = get_global_bparam(GLOBAL_ERROR_TIMEOUT);
	if ((cc<=0) || (cc>99)) set_global_bparam(GLOBAL_ERROR_TIMEOUT,12);
	cc = get_global_bparam(GLOBAL_SALERUN_MODE);
	if ((cc<1) || (cc>8)) {
		set_global_bparam(GLOBAL_SALERUN_MODE,2);
		salemode_reset();
	}
// Added by CJK 20061218
	plu_n_field    =plu_struct_get_nfield();
	if ((plu_n_field<=0) || (plu_n_field>128)) {
		caption_error(1);
	}
/////////////////////////
	plu_ptype_reset(); // global_set_pluinhibit() 앞에 있어야 함.
	global_set_pluinhibit(VERIFY_ALL,1);   //se-hyung 070802
	//plu_ptype_reset(); // global_set_pluinhibit() 내부에 plu_ptype_reset() 있음
	PrtPaperType=get_global_bparam(GLOBAL_PAPER_TYPE);

	if (inhibit) return;
	caption_split_by_code(0xC701,&cap,0);
	strcpy(global_message[MSG_00_MISC_VALUE],cap.form);
	str_fill((char *)global_message[MSG_00_MISC_VALUE],6);
	strcat((char *)global_message[MSG_00_MISC_VALUE],"%ld");

	caption_split_by_code(0xC702,&cap,1);
	q=0;
	cap.form[33]=0;
#if defined(USE_ARAB_CONVERT) || defined(HEBREW_FONT)
	q=global_load_caption(q,cap.form,global_datetime[MSG_DT_NOPLU],10);	//FONT 잘림
	str_fill((char *)global_datetime[MSG_DT_NOPLU],9);
	q=global_load_caption(q,cap.form,global_datetime[MSG_DT_NOCLK],10);
	str_fill((char *)global_datetime[MSG_DT_NOCLK],9);
#else
	q=global_load_caption(q,cap.form,global_datetime[MSG_DT_NOPLU],9);
	str_fill((char *)global_datetime[MSG_DT_NOPLU],8);
	q=global_load_caption(q,cap.form,global_datetime[MSG_DT_NOCLK],9);
	str_fill((char *)global_datetime[MSG_DT_NOCLK],8);
#endif
	caption_split_by_code(0xc059,&cap,1);
	strcpy(global_message[MSG_02_DC],cap.form);
	caption_split_by_code(0xc05a,&cap,1);
	strcpy(global_message[MSG_03_DC_FIXED],cap.form);
	caption_split_by_code(0xc05b,&cap,1);
	strcpy(global_message[MSG_04_FREEPLU],cap.form);
	caption_split_by_code(0xc05d,&cap,1);
	strcpy(global_message[MSG_06_FIXEDPRICE],cap.form);

	caption_split_by_code(0xc906,&cap,1);
	strcpy(global_message[MSG_NOT_ADDSUMMARY],cap.form);
	caption_split_by_code(0xE603,&cap,1);
	strcpy(global_message[MSG_OPEN_HEAD],cap.form);
	caption_split_by_code(0xE604,&cap,1);
	strcpy(global_message[MSG_PEELOFF],cap.form);

//	caption_split_by_code(0x499A,&cap,0);
//	cap.form[33]=0;
//	q=0; for (i=0; i<5; i++) {
//		q=global_load_caption(q,cap.form,global_country_code[i],16);
//	}
//	caption_split_by_code(0x499B,&cap,0);
//	cap.form[33]=0;
//	q=0; for (i=0; i<5; i++) {
//		q=global_load_caption(q,cap.form,global_country_code[5+i],16);
//	}
	caption_split_by_code(0xC703,&cap,0);
	q=0;
	cap.form[33]=0;
	q=global_load_caption(q,cap.form,global_datetime[MSG_DT_SCALE],9);
	str_fill((char *)global_datetime[MSG_DT_SCALE],6);
	strcat((char *)global_datetime[MSG_DT_SCALE],"%02d");
	q=global_load_caption(q,cap.form,global_datetime[MSG_DT_DEPT],9);
	str_fill((char *)global_datetime[MSG_DT_DEPT],6);
	strcat((char *)global_datetime[MSG_DT_DEPT],"%02d");
	q=global_load_caption(q,cap.form,global_datetime[MSG_DT_PLU],9);
	str_fill((char *)global_datetime[MSG_DT_PLU],2);
	strcat((char *)global_datetime[MSG_DT_PLU],"%06ld");
	q=global_load_caption(q,cap.form,global_datetime[MSG_DT_CLERK],9);
	str_fill((char *)global_datetime[MSG_DT_CLERK],6);
	strcat((char *)global_datetime[MSG_DT_CLERK],"%02d");

	caption_split_by_code(0xc051,&cap,1);
	strcpy(global_message[MSG_FIXED_WEIGHT],cap.form);

	caption_split_by_code(0xc052,&cap,1);
	strcpy(global_message[MSG_14_C052],cap.form);
	caption_split_by_code(0xC90C,&cap,1);
	strcpy(global_message[MSG_NO_PLUNAME],cap.form);
	caption_split_by_code(0xC721,&cap,1);
	strcpy(global_message[MSG_MISC_WEIGHT],cap.form);
	caption_split_by_code(0xC722,&cap,1);
	strcpy(global_message[MSG_MISC_COUNT],cap.form);
	caption_split_by_code(0xC723,&cap,1);
	strcpy(global_message[MSG_MISC_PCS],cap.form);

	caption_split_by_code(0xf212,&cap,1);
	strcpy(global_message[MSG_19_NET_SFULL],cap.form);
	caption_split_by_code(0xf213,&cap,1);
	strcpy(global_message[MSG_20_NET_PFULL],cap.form);
	caption_split_by_code(0xf211,&cap,1);
	strcpy(global_message[MSG_21_NET_ERR],cap.form);

	caption_split_by_code(0xc902,&cap,1);
	strcpy(global_message[MSG_NO_TOTAL],cap.form);

	caption_split_by_code(0xc803,&cap,1);
	strcpy(global_message[MSG_23_C803],cap.form);

	caption_split_by_code(0xc04c,&cap,1);
	strcpy(global_message[MSG_24_C04C],cap.form);

	caption_split_by_code(0xE301,&cap,1);
	strcpy(global_message[MSG_MINWEIGHT],cap.form);

	caption_split_by_code(0xc804,&cap,1);
	strcpy(global_message[MSG_26_C804],cap.form);

	caption_split_by_code(0xC80F,&cap,1);
	strcpy(global_message[MSG_27_C80F],cap.form);

	caption_split_by_code(0xc907,&cap,0);
	strcpy(global_message[MSG_NOSALEDATA],cap.form);

	caption_split_by_code(0xc05e,&cap,0);
	strcpy(global_message[MSG_29_DSP_VOID],cap.form);

	caption_split_by_code(0xf110,&cap,0);
	strcpy(global_message[MSG_30_PLU],cap.form);
	
	caption_split_by_code(0xC101,&cap,0);
	str_fill((char *)cap.form,7);
#ifdef HEBREW_FONT
	strcat(cap.form,"]   [");	// Modified by CJK 20050728
#elif defined(USE_ARAB_CONVERT)
    strcat(cap.form,"[  ]");
#else
	strcat(cap.form,"[   ]");	// Modified by CJK 20050728
#endif
	str_fill((char *)cap.form,15);
#ifdef USE_ARAB_CONVERT
    strcat(cap.form,"%-2d ");
#else
	strcat(cap.form,"%-3d ");
#endif
	strcpy(global_message[MSG_VOID_LIST],cap.form);

	caption_split_by_code(0xC102,&cap,0);
	strcpy((char *)global_message[MSG_VOID_LIST_2],(char *)cap.form);

	caption_split_by_code(0xC01B,&cap,0);
	strcat(cap.form," %s");
	strcpy(global_message[MSG_RETURN],cap.form);

	caption_split_by_code(0xC020,&cap,0);
	strcat(cap.form," %d,");
	strcpy(global_message[MSG_TOTALCNT],cap.form);
	caption_split_by_code(0xC021,&cap,0);
	strcat(cap.form," %d");
	strcat(global_message[MSG_TOTALCNT],cap.form);

	caption_split_by_code(0xC807,&cap,1);
	strcpy(global_message[MSG_WAR_PLUTARE],cap.form);

	caption_split_by_code(0xC808,&cap,1);
	strcpy(global_message[MSG_WAR_KEYTARE],cap.form);

	caption_split_by_code(0xC809,&cap,1);
	strcpy(global_message[MSG_WAR_WEIGHTTARE],cap.form);

#ifdef HEBREW_FONT
	caption_split_by_code(0xC03A,&cap,1);
	sprintf(global_message[MSG_WAR_NOVOID],"]#%cd[ %s",'%',cap.form);

	caption_split_by_code(0xC03B,&cap,1);
	sprintf(global_message[MSG_WAR_PERMIT],"]#%cd[ %s",'%',cap.form);
#else
	caption_split_by_code(0xC03A,&cap,1);
	sprintf(global_message[MSG_WAR_NOVOID],"[#%cd] %s",'%',cap.form);

	caption_split_by_code(0xC03B,&cap,1);
	sprintf(global_message[MSG_WAR_PERMIT],"[#%cd] %s",'%',cap.form);
#endif
	caption_split_by_code(0xC03E,&cap,1);
	strcpy(global_message[MSG_TRANSINFO],cap.form);

	caption_split_by_code(0xC050,&cap,1);
	strcpy(global_message[MSG_CURRENCY_CHANGE],cap.form);
	strcat(global_message[MSG_CURRENCY_CHANGE]," %s %s");

	caption_split_by_code(0xE102,&cap,1);
	//cap.form[33]=0;
	strcpy(global_message[MSG_INVALID_VALUE],cap.form);

	caption_split_by_code(0xC070,&cap,1);
	//cap.form[33]=0;
	strcpy(global_message[MSG_INPUTXKEY],cap.form);

	caption_split_by_code(0xC812,&cap,1);
	//cap.form[33]=0;
	strcpy(global_message[MSG_NOT_ALLOW_KGLB],cap.form);

	caption_split_by_code(0xC813,&cap,1);
	//cap.form[33]=0;
	strcpy(global_message[MSG_INVALID_FIXEDWEIGHT],cap.form);

	caption_split_by_code(0xc816,&cap,1);
	strcpy(global_message[MSG_CHECK_WEIGHT],cap.form);

	caption_split_by_code(0xf214,&cap,1);
	strcpy(global_message[MSG_CLERK_IS_LOCKED],cap.form);

	caption_split_by_code(0xCA04,&cap,1);	//Added by JJANG 20080307		
	strcpy(global_message[MSG_NO_MASTER],cap.form);	

	caption_split_by_code(0xCA05,&cap,1);
	strcpy(global_message[MSG_MEMORY_FULL],cap.form);	

	cc = get_global_bparam(GLOBAL_SCANNER_FLAG1);
	UseScanner = (cc>>3)&0x01;
#ifdef USE_TRACE_STANDALONE
	//modeIndiv range : 0 ~ 7
	//0: default. 사용안함
	//1: 신세계
	//2: 농협
	//3: GS Mart
	//4: Standard (신세계 + 개체인덱스)
	//5: 이마트
	ModeIndiv = (cc>>5)&0x07;
#endif
	//strcpy(display_message[DSP_MSG_TARE], "TARE");
	receipt_caption_read(RCPTCAP_ID_DSP_TITLE_TARE, display_message[DSP_MSG_TARE]);

	//strcpy(display_message[DSP_MSG_WEIGHT], "WEIGHT");
	receipt_caption_read(RCPTCAP_ID_DSP_TITLE_WEIGHT, display_message[DSP_MSG_WEIGHT]);

	//strcpy(display_message[DSP_MSG_QUANTITY], "QTY");
	receipt_caption_read(RCPTCAP_ID_DSP_TITLE_QTY, display_message[DSP_MSG_QUANTITY]);

	//strcpy(display_message[DSP_MSG_PIECES], "PCS");
	receipt_caption_read(RCPTCAP_ID_DSP_TITLE_PCS, display_message[DSP_MSG_PIECES]);

	//strcpy(display_message[DSP_MSG_UNITPRICE], "UNIT PRICE");
	receipt_caption_read(RCPTCAP_ID_DSP_TITLE_UNITPRICE, display_message[DSP_MSG_UNITPRICE]);

	//strcpy(display_message[DSP_MSG_TOTALPRICE], "TOTAL PRICE");
	receipt_caption_read(RCPTCAP_ID_DSP_TITLE_TOTALPRICE, display_message[DSP_MSG_TOTALPRICE]);
}
