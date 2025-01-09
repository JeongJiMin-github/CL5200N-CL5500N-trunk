/*****************************************************************************
|*  Copyright		:	(c) 2003 CAS
|*  Filename		:	initial_glb.c
|*  Version		:	0.1
|*  Modified		:	2004/08/20
|*  Modifier		:	CJK
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
#include "department.h"
#include "menu_code_text1.h"
#include "barcode_file.h"
#include "initial.h"
#include "pgm_app.h"
#include "pgm_global_set.h"
#include "tax.h"
#include "discount.h"
#include "mode_sale.h"
#include "clerk_app.h"
#include "key_function.h"
#include "initial_glb.h"
#include "global_set.h"
#include "prt_app.h"
#include "adm_app.h"  //se-hyung 20080325
#include "ethernet.h"
#include "ethernet_comm.h"
#include "traceability.h"
#include "mode_calibration.h"
#include "dsp_lcd26d_16x96.h"

#define MAX_FUNC_CODE		55
#define MAX_FACTORY_CODE	72

INT8U dealer_factory_mode = 0;	// initialize : Modified by CJK 20060412
#ifdef USE_REALTIME_CHECK_INTERVAL
extern INT32U check_interval;
extern INT8U netcheckflag;
#endif

void keyin_setcallback(void (*function)() );
void keyin_clearcallback(void);
void ad_set_fulltare(INT8U fulltare);	//se-hyung 20080325

extern INT8U parameter_print_list(INT8U mode);


extern INT8U  get_global_bparam(INT32U a);
extern INT16U get_global_wparam(INT32U a);

extern void   set_global_bparam(INT32U a,INT8U v);
extern void   set_global_wparam(INT32U a,INT16U v);
extern void   set_global_lparam(INT32U a,INT32U v);
extern void   set_global_sparam(INT32U a,INT8U *s,INT16S l);

extern INT8U  get_net_bparam(INT32U a);
extern INT16U get_net_wparam(INT32U a);
extern INT32U get_net_lparam(INT32U a);
extern void   get_net_sparam(INT32U a,INT8U *v,INT16S l);

extern void   set_net_bparam(INT32U a,INT8U v);
extern void   set_net_wparam(INT32U a,INT16U v);
extern void   set_net_lparam(INT32U a,INT32U v);
extern void   set_net_sparam(INT32U a,INT8U *s,INT16S l);
extern void display_style(void);
extern INT8U ad_get_accumul_tare(void);
extern void  ad_set_accumul_tare(INT8U val);
extern INT8U ad_get_subtract_tare(void);
extern void  ad_set_subtract_tare(INT8U val);
extern void plu_tare_inhibit_set(void);
extern INT16U plu_ptype_reset(void);
extern void  plu_ptype_enable_code(void);
extern void  KEY_CopyPS2SelfKeyTable(void);
extern void ClearTransactionHeadTail(void);

//funcion,     default, memory addresst,			memory offset,		 memory size, bitflag, startbit, bit count, country
#ifndef USE_TRANSFER_MENU_SETTING		//아래는 기존 Menu setting 배열. Trasfer menu setting 시엔 parameter 배열과 합침.
ROMDATA PARAMETER_SETTING_TYPE  ParameterUserSetting[]=
{
	//funcion, default, memory addresst,			memory offset,		memory size, bitflag, startbit, bit count, country
	{  2,	     1,		GLOBAL_DISPLAY_INFO_FLAG,   	FLASH_GLOBAL_AREA,		1,	    4,	     0,	       1,	COUNTRY_UN},//Show Display info
	{  3,	     1,		GLOBAL_DISPLAY_INFO_FLAG,   	FLASH_GLOBAL_AREA,		1,	    4,	     3,	       1,	COUNTRY_UN},//Show Clerk Nickname
#ifdef USE_CHN_EMART_DEFAULT
	{  4,	     1,		GLOBAL_DISPLAY_ALIGN,		FLASH_GLOBAL_AREA, 		1,	    1,	     0,	       0,	COUNTRY_UN},//PLU Name Align (M1871)
#else
	{  4,	     0,		GLOBAL_DISPLAY_ALIGN,		FLASH_GLOBAL_AREA, 		1,	    1,	     0,	       0,	COUNTRY_UN},//PLU Name Align (M1871)
#endif
#ifdef USE_GSMART_TRACE_DEFAULT
	{  4,	     0,		GLOBAL_DISPLAY_ALIGN,		FLASH_GLOBAL_AREA, 		1,	    1,	     0,	       0,	COUNTRY_KR},//PLU Name Align (M1871)
#else
	{  4,	     1,		GLOBAL_DISPLAY_ALIGN,		FLASH_GLOBAL_AREA, 		1,	    1,	     0,	       0,	COUNTRY_KR},//PLU Name Align (M1871)
#endif
#ifdef USE_NHMART_DEFAULT
	{  10,	     0, 	GLOBAL_BARCODE_PRIORITY,     	FLASH_GLOBAL_AREA,		1,	    1,	     0,	       0,	COUNTRY_UN},//Use Global barcode
#else
	{  10,	     1, 	GLOBAL_BARCODE_PRIORITY,     	FLASH_GLOBAL_AREA,		1,	    1,	     0,	       0,	COUNTRY_UN},//Use Global barcode
#endif
#ifdef USE_MANAGE_INVENTORY_DEFAULT
	{  31,	     0,		GLOBAL_LABEL_PRIORITY,		FLASH_GLOBAL_AREA,   		1,	    1,	     0,	       0,	COUNTRY_UN},//Use global label
#else
	{  31,	     1,		GLOBAL_LABEL_PRIORITY,		FLASH_GLOBAL_AREA,   		1,	    1,	     0,	       0,	COUNTRY_UN},//Use global label
	{  31,       0,		GLOBAL_LABEL_PRIORITY,		FLASH_GLOBAL_AREA,			1,	    1,	     0,	       0,	COUNTRY_RU},//Use global label
#endif
#ifdef USE_CHN_EMART_DEFAULT
	{  32,	    10,		GLOBAL_LABEL_STANDARD,		FLASH_GLOBAL_AREA, 		2,	    2,	     0,	       0,	COUNTRY_UN},//Std Label ID
#elif defined(USE_HEBREW_DEFAULT)
	{  32,	    51,		GLOBAL_LABEL_STANDARD,		FLASH_GLOBAL_AREA,  	2,	    2,	     0,	       0,	COUNTRY_UN},//Std Label ID
#else
	{  32,	    10,		GLOBAL_LABEL_STANDARD,		FLASH_GLOBAL_AREA, 		2,	    2,	     0,	       0,	COUNTRY_UN},//Std Label ID
#endif
//글로벌라벨 초기값
#if defined(USE_GSMART_TRACE_DEFAULT) || defined(USE_LOTTESUPER_DEFAULT)
	{  32,	    53,		GLOBAL_LABEL_STANDARD,		FLASH_GLOBAL_AREA,  		2,	    2,	     0,	       0,	COUNTRY_KR},
#elif (defined(USE_GSMART_DEFAULT) || defined(USE_EMART_DEFAULT) || defined(USE_NHMART_DEFAULT)|| defined(USE_SHINSEGAE_DEFAULT))
	{  32,	    51,		GLOBAL_LABEL_STANDARD,		FLASH_GLOBAL_AREA,  		2,	    2,	     0,	       0,	COUNTRY_KR},
#elif defined(USE_LOTTEMART_DEFAULT)
	{  32,		81, 	GLOBAL_LABEL_STANDARD,		FLASH_GLOBAL_AREA,			2,		2,		 0, 	   0,	COUNTRY_KR},
#elif defined(USE_TOPMART_DEFAULT) //sewon 170117
	{  32,		60, 	GLOBAL_LABEL_STANDARD,		FLASH_GLOBAL_AREA,			2,		2,		 0, 	   0,	COUNTRY_KR},
#elif defined(USE_NHMART_SAFE_DEFAULT)
	{  32,	    90,		GLOBAL_LABEL_STANDARD,		FLASH_GLOBAL_AREA,  		2,	    2,	     0,	       0,	COUNTRY_KR},
#elif defined(USE_MANAGE_INVENTORY_DEFAULT)
	{  32,	    70,		GLOBAL_LABEL_STANDARD,		FLASH_GLOBAL_AREA,  		2,	    2,	     0,	       0,	COUNTRY_KR},
#else
	{  32,	    31,		GLOBAL_LABEL_STANDARD,		FLASH_GLOBAL_AREA,  		2,	    2,	     0,	       0,	COUNTRY_KR},
#endif
	{  32,	    11,		GLOBAL_LABEL_STANDARD,		FLASH_GLOBAL_AREA,  		2,	    2,	     0,	       0,	COUNTRY_RU},

#ifdef USE_CHN_EMART_DEFAULT
	{  33,	    31,		GLOBAL_LABEL_MISC,		FLASH_GLOBAL_AREA,		2,	    2,	     0,	       0,	COUNTRY_UN},//Misc. Label ID
#else
	{  33,	    10,		GLOBAL_LABEL_MISC,		FLASH_GLOBAL_AREA,		2,	    2,	     0,	       0,	COUNTRY_UN},//Misc. Label ID
#endif
	{  33,	    31,		GLOBAL_LABEL_MISC,		FLASH_GLOBAL_AREA,		2,	    2,	     0,	       0,	COUNTRY_KR},
	{  33,	    11,		GLOBAL_LABEL_MISC,		FLASH_GLOBAL_AREA,		2,	    2,	     0,	       0,	COUNTRY_RU},
    
	{  34,	    29, 	GLOBAL_LABEL_ADDUP,		FLASH_GLOBAL_AREA,		2,	    2,	     0,	       0,	COUNTRY_UN},//Add-up TTL Label ID
#ifdef USE_SETPRODUCT_SALE
	{  34,		61, 	GLOBAL_LABEL_ADDUP, 	FLASH_GLOBAL_AREA,		2,		2,		 0, 	   0,	COUNTRY_KR},
#else
	{  34,	    29, 	GLOBAL_LABEL_ADDUP,		FLASH_GLOBAL_AREA,		2,	    2,	     0,	       0,	COUNTRY_KR},
#endif
	{  34,	    29, 	GLOBAL_LABEL_ADDUP,		FLASH_GLOBAL_AREA,		2,	    2,	     0,	       0,	COUNTRY_RU},

	{  50,	     0,		GLOBAL_PRINT_OPER_FLAG2,  	FLASH_GLOBAL_AREA,		1,	    4,	     0,	       1,	COUNTRY_UN},//Print PLU Test Label

	{  58,	     0,		GLOBAL_PRINT_OPER_FLAG,		FLASH_GLOBAL_AREA,  		1,	    4,	     1,	       1,	COUNTRY_UN},//Label Prt. w.o Rewind (X Key)

	{  59,	     2,		GLOBAL_PRINT_OPER_FLAG,		FLASH_GLOBAL_AREA,		1,	    4,	     3,	       2,	COUNTRY_UN},//Confirm Prt Report on Label

	{  60,	     0,		GLOBAL_PRINT_OPER_FLAG,		FLASH_GLOBAL_AREA, 		1,	    4,	     5,	       2,	COUNTRY_UN},//Print origin for comm. line:
	{  60,	     2,		GLOBAL_PRINT_OPER_FLAG,		FLASH_GLOBAL_AREA, 		1,	    4,	     5,	       2,	COUNTRY_KR},//Print origin for comm. line:

    {  66,	     1,		GLOBAL_PRINT_CLERK_FLAG1,    	FLASH_GLOBAL_AREA,		1,	    4,	     0,	       1,	COUNTRY_UN},//Print Clerk name
	{  67,	     1,		GLOBAL_PRINT_CLERK_FLAG1,    	FLASH_GLOBAL_AREA, 		1,	    4,	     1,	       1,	COUNTRY_UN},//Print Store name
	{  68,	     1,		GLOBAL_PRINT_CLERK_FLAG1,    	FLASH_GLOBAL_AREA, 		1,	    4,	     2,	       1,	COUNTRY_UN},//Print List
	{  69,	     1,		GLOBAL_PRINT_CLERK_FLAG1,    	FLASH_GLOBAL_AREA,  		1,	    4,	     3,	       1,	COUNTRY_UN},//Print Total Barcode
	{  70,	     0,		GLOBAL_PRINT_CLERK_FLAG1,    	FLASH_GLOBAL_AREA,  		1,	    4,	     4,	       1,	COUNTRY_UN},//Print Total Barcode
	{  71,	     0,		GLOBAL_PRINT_CLERK_FLAG1,    	FLASH_GLOBAL_AREA,  		1,	    4,	     5,	       1,	COUNTRY_UN},//Print Total Barcode
	{  74,	     1,		GLOBAL_PRINT_CLERK_FLAG2,    	FLASH_GLOBAL_AREA,		1,	    4,	     1,	       1,	COUNTRY_UN},//Prt. Tax Information

	{  88,	     0,		GLOBAL_PRINT_ADDUP_FLAG2,    	FLASH_GLOBAL_AREA,		1,	    4,	     2,	       1,	COUNTRY_UN},//Rotate Total Label
	{  89,	     0,		GLOBAL_PRINT_ADDUP_FLAG2,    	FLASH_GLOBAL_AREA,		1,	    4,	     3,	       1,	COUNTRY_UN},//Rotate Item Label

	{  100,	     1,		GLOBAL_AREA_SALEMODE,		FLASH_GLOBAL_AREA,  		1,	    1,	     0,	       0,	COUNTRY_UN},//Select Clerk Sale mode
	{  100,	     1,		GLOBAL_AREA_SALEMODE,		FLASH_GLOBAL_AREA,  		1,	    1,	     0,	       0,	COUNTRY_EU},
	{  100,	     3,		GLOBAL_AREA_SALEMODE,		FLASH_GLOBAL_AREA,  		1,	    1,	     0,	       0,	COUNTRY_EUT},

	{  102,	     2,		GLOBAL_SALERUN_MODE,		FLASH_GLOBAL_AREA,  		1,	    1,	     0,	       0,	COUNTRY_UN},//Select Sale mode(MENU1810)
	{  102,	     1,		GLOBAL_SALERUN_MODE,		FLASH_GLOBAL_AREA,  		1,	    1,	     0,	       0,	COUNTRY_EUT},

	{  106,	     0,		GLOBAL_SALE_SAVE_FLAG,		FLASH_GLOBAL_AREA,  		1,	    4,	     0,	       1,	COUNTRY_UN},//Save unit price change
#if defined(USE_LOTTEMART_DEFAULT) || defined(USE_HYUNDAI_DEFAULT)
	{  106, 	 0, 	GLOBAL_SALE_SAVE_FLAG,		FLASH_GLOBAL_AREA,			1,		4,		 0, 	   1,	COUNTRY_KR},
#else
	{  106, 	 1, 	GLOBAL_SALE_SAVE_FLAG,		FLASH_GLOBAL_AREA,			1,		4,		 0, 	   1,	COUNTRY_KR},
#endif
	{  107,	     0,		GLOBAL_SALE_SAVE_FLAG,		FLASH_GLOBAL_AREA,   		1,	    4,	     1,	       1,	COUNTRY_UN},//Save Type change
#ifdef USE_HYUNDAI_DEFAULT
	{  107,	     0,		GLOBAL_SALE_SAVE_FLAG,		FLASH_GLOBAL_AREA,   		1,	    4,	     1,	       1,	COUNTRY_KR},
#else
	{  107,	     1,		GLOBAL_SALE_SAVE_FLAG,		FLASH_GLOBAL_AREA,   		1,	    4,	     1,	       1,	COUNTRY_KR},
#endif
	{  120,	     0,		GLOBAL_SALE_SETUP_FLAG1,     	FLASH_GLOBAL_AREA,   		1,	    4,	     0,	       1,	COUNTRY_UN},//Start save mode
#ifdef USE_MANAGE_INVENTORY_DEFAULT
	{  120,	     0,		GLOBAL_SALE_SETUP_FLAG1,     	FLASH_GLOBAL_AREA,   		1,	    4,	     0,	       1,	COUNTRY_KR}, // Added by CJK 20060123
#else
	{  120,	     1,		GLOBAL_SALE_SETUP_FLAG1,     	FLASH_GLOBAL_AREA,   		1,	    4,	     0,	       1,	COUNTRY_KR}, // Added by CJK 20060123
#endif

#ifdef USE_CHN_EMART_DEFAULT
	{  121, 	 1, 	GLOBAL_SALE_SETUP_FLAG1,		FLASH_GLOBAL_AREA,		1,		4,		 5, 	   1,	COUNTRY_UN},//PLU Clear Aft Sale(WT. On Tray)
#else
	{  121, 	 0, 	GLOBAL_SALE_SETUP_FLAG1,		FLASH_GLOBAL_AREA,		1,		4,		 5, 	   1,	COUNTRY_UN},//PLU Clear Aft Sale(WT. On Tray)
#endif

	{  121,	     1,		GLOBAL_SALE_SETUP_FLAG1,     	FLASH_GLOBAL_AREA,  		1,	    4,	     5,	       1,	COUNTRY_EU},
	{  121,	     1,		GLOBAL_SALE_SETUP_FLAG1,     	FLASH_GLOBAL_AREA,  		1,	    4,	     5,	       1,	COUNTRY_KR},	// Added by CJK 20050506
    
	{  122,	     0,		GLOBAL_SALE_SETUP_FLAG2,     	FLASH_GLOBAL_AREA, 		1,	    4,	     7,	       2,	COUNTRY_UN},// Swedish Auto Barcode[0~2]

	{  125,	     1,		GLOBAL_DISCOUNT_PRIORITY,    	FLASH_GLOBAL_AREA,		1,	    1,	     0,	       0,	COUNTRY_UN},//Discount Priority
	{  130,	     1,		GLOBAL_TAX_FLAG,		FLASH_GLOBAL_AREA,    		1,	    4,	     0,	       1,	COUNTRY_UN},//Use Global TAX
	{  145,	     1,		GLOBAL_LOG_FLAG,		FLASH_GLOBAL_AREA,    		1,	    4,	     0,	       1,	COUNTRY_UN},//LogOff after Transaction
	{  146,	     30,	GLOBAL_TIME_CLERKLOGOUT,     	FLASH_GLOBAL_AREA, 		2,	    2,	     0,	       0,	COUNTRY_UN},//Clerk log out Time

	//250~273 PRINT AREA
	{  252,	     10,	FLASH_OFFSET_StbTimeTrimValue, 	FLASH_PRINTER_AREA,		2,	    2,	     0,	       0,	COUNTRY_UN},//Print Intensity (0-20) --> when save (*30)//HYP 20040811

	{  266,	     128,	FLASH_OFFSET_GapThreshold,     	FLASH_PRINTER_AREA,		1,	    1,	     0,	       0,	COUNTRY_UN},//Gap Threshold
	{  267,	     128,	FLASH_OFFSET_PeelThreshold,    	FLASH_PRINTER_AREA,		1,	    1,	     0,	       0,	COUNTRY_UN},//Peel Threshold
	{  268,	     200,	FLASH_OFFSET_GapOutOfPaperThreshold, 	FLASH_PRINTER_AREA,	1,	    1,	     0,	       0,	COUNTRY_UN},//Out Of Paper Threshold
	{  270,	     1,		FLASH_OFFSET_PrePrintMode,     		FLASH_PRINTER_AREA,	1,	    1,	     0,	       0,	COUNTRY_UN},//Enable Preprint
	{  271,	     10,	FLASH_OFFSET_PrePrintLength,   		FLASH_PRINTER_AREA,	1,	    1,	     0,	       0,	COUNTRY_UN},//Preprint Length (0-10) 
#ifdef USE_GSMART_PLU
 #if defined(USE_GSMART_TRACE_DEFAULT) || defined(USE_GSMART_DEFAULT)
	{  271,	     6,	FLASH_OFFSET_PrePrintLength,   		FLASH_PRINTER_AREA,	1,	    1,	     0,	       0,	COUNTRY_KR},//Preprint Length (0-10) 
 #elif defined(USE_EMART_DEFAULT)
	{  271,	     6,	FLASH_OFFSET_PrePrintLength,   		FLASH_PRINTER_AREA,	1,	    1,	     0,	       0,	COUNTRY_KR},//Preprint Length (0-10) 
 #elif defined(USE_NHMART_SAFE_DEFAULT)
 	{  271,	     0,	FLASH_OFFSET_PrePrintLength,   		FLASH_PRINTER_AREA,	1,	    1,	     0,	       0,	COUNTRY_KR},//Preprint Length (0-10) 
 #else
	{  271,	     8,	FLASH_OFFSET_PrePrintLength,   		FLASH_PRINTER_AREA,	1,	    1,	     0,	       0,	COUNTRY_KR},//Preprint Length (0-10) 
 #endif  
#endif
#ifdef USE_CHINA_DEFAULT
	{  271,	     5,		FLASH_OFFSET_PrePrintLength,   		FLASH_PRINTER_AREA,	1,	    1,	     0,	       0,	COUNTRY_UN},//Preprint Length (0-10)
#endif
	
	{  272,	     0,		FLASH_OFFSET_GapBufInPtrTrimValue,	FLASH_PRINTER_AREA,	2,	    2,	     0,	       0,	COUNTRY_UN},//Feed Adjustment (-200-200)//HYP 20040811
	{  273,	     0,		FLASH_OFFSET_GapBufInPtrReverseTrimValue,	FLASH_PRINTER_AREA,	2,	    2,	     0,	       0,	COUNTRY_UN},//Reverse Feed Adjustment (0-80)//KKH 201105118
	
	{  300,	     1,		GLOBAL_LABEL_MARKDOWNFLAG,   	FLASH_GLOBAL_AREA,		1,	    4,	     0,	       1,	COUNTRY_UN},//Total Price Mark Down
	{  300,	     0,		GLOBAL_LABEL_MARKDOWNFLAG,   	FLASH_GLOBAL_AREA,		1,	    4,	     0,	       1,	COUNTRY_RU},//Total Price Mark Down

	{  301,	     1,		GLOBAL_LABEL_MARKDOWNFLAG,   	FLASH_GLOBAL_AREA,		1,	    4,	     1,	       1,	COUNTRY_UN},//Unit Price Mark Down
	{  301,	     0,		GLOBAL_LABEL_MARKDOWNFLAG,   	FLASH_GLOBAL_AREA,		1,	    4,	     1,	       1,	COUNTRY_RU},//Unit Price Mark Down

	{  302,	     0,		GLOBAL_LABEL_MARKDOWNFLAG,   	FLASH_GLOBAL_AREA,		1,	    4,	     2,	       1,	COUNTRY_UN},//Weight Mark Down

	{  303,	     0,		GLOBAL_LABEL_MARKDOWNFLAG,   	FLASH_GLOBAL_AREA,		1,	    4,	     3,	       1,	COUNTRY_UN},//Quantity Mark Down
#ifdef  USE_BELARUS_DEFAULT		// Use Belarus Default 
 	{  305,	     0,		GLOBAL_LABEL_FLAG,		FLASH_GLOBAL_AREA,	   	1,	    4,	     0,	       1,	COUNTRY_UN},//print pcs symbol in qty area
#else	
 	{  305,	     1,		GLOBAL_LABEL_FLAG,		FLASH_GLOBAL_AREA,	   	1,	    4,	     0,	       1,	COUNTRY_UN},//print pcs symbol in qty area
#endif
	{  306, 	 1, 	GLOBAL_LABEL_FLAG,		FLASH_GLOBAL_AREA,		1,		4,		 1, 	   1,	COUNTRY_UN},//Print Pcs in Unit Price Area

//	{  310,	     1, 	GLOBAL_SALE_SETUP_FLAG2,     	FLASH_GLOBAL_AREA,		1,    	    4,	     4,	       1,	COUNTRY_UN},  //   Use Double Speed Key
//	{  310,	     0, 	GLOBAL_SALE_SETUP_FLAG2,     	FLASH_GLOBAL_AREA,		1,    	    4,	     4,	       1,	COUNTRY_KR},  //   Use Double Speed Key

	{  311,	     0, 	GLOBAL_SALE_SETUP_FLAG5,     	FLASH_GLOBAL_AREA,		1,    	    4,	     2,	       2,	COUNTRY_UN},  //   Prepack

	{  312,	     1, 	GLOBAL_SALE_SETUP_FLAG6,     	FLASH_GLOBAL_AREA,		1,    	    4,	     0,	       1,	COUNTRY_UN},  // Multi-print on discount sale SG060419  
	{  313,	     1, 	GLOBAL_SALE_SETUP_FLAG6,     	FLASH_GLOBAL_AREA,		1,    	    4,	     1,	       1,	COUNTRY_UN},  // Date-Print ON/OFF flag SG060510  
	{  314,	     0, 	GLOBAL_SALE_SETUP_FLAG6,     	FLASH_GLOBAL_AREA,		1,    	    4,	     2,	       1,	COUNTRY_UN},  // Start Auto Mode SG060928  
	{  315,	     0, 	GLOBAL_SALE_SETUP_FLAG5,     	FLASH_GLOBAL_AREA,		1,    	    4,	     3,	       1,	COUNTRY_UN},  // Enable Quick PLU CJK070130  
	{  316,	     1, 	GLOBAL_SALE_SETUP_FLAG5,     	FLASH_GLOBAL_AREA,		1,    	    4,	     4,	       1,	COUNTRY_UN},  // Save Member Code SG070206  
#ifdef USE_HYUNDAI_DEFAULT
	{  317,	     1, 	GLOBAL_SALE_SETUP_FLAG5,     	FLASH_GLOBAL_AREA,		1,    	    4,	     5,	       1,	COUNTRY_UN},  // Print Item when "Return" SG070215  
#else
	{  317,	     0, 	GLOBAL_SALE_SETUP_FLAG5,     	FLASH_GLOBAL_AREA,		1,    	    4,	     5,	       1,	COUNTRY_UN},  // Print Item when "Return" SG070215  
#endif
	{  318,	     1,		GLOBAL_DISPLAY_INFO_FLAG,   	FLASH_GLOBAL_AREA,		1,	    4,	     1,	       1,	COUNTRY_UN},// Menu1873: T.Price=Price+Tax

	{  319,	     1,		GLOBAL_SALE_SETUP_FLAG7,	FLASH_GLOBAL_AREA,		1,	    4,	     2,	       1,	COUNTRY_UN}, //Menu1871 Call PLU Use Scroll MSG	  se-hyung 080124
	{  319,	     0,		GLOBAL_SALE_SETUP_FLAG7,	FLASH_GLOBAL_AREA,		1,	    4,	     2,	       1,	COUNTRY_US}, //Menu1871 Call PLU Use Scroll MSG	  se-hyung 080124

	{  320,	     0,		GLOBAL_SALE_SETUP_FLAG7,	FLASH_GLOBAL_AREA,		1,	    4,	     3,	       1,	COUNTRY_UN}, //Menu1873 Use double DATE/TIME key
	{  320,	     1,		GLOBAL_SALE_SETUP_FLAG7,	FLASH_GLOBAL_AREA,		1,	    4,	     3,	       1,	COUNTRY_KR}, //Menu1873 Use double DATE/TIME key
    
#if defined(USE_TRACE_STANDALONE)
 #if defined(USE_NHMART_DEFAULT)
	{  321,	     1,		GLOBAL_SALE_SETUP_FLAG10,	FLASH_GLOBAL_AREA,		1,    	    4,	     2,	       3,	COUNTRY_UN}, //Menu1873 Use saving trace. no :
 #elif defined(USE_GSMART_TRACE_DEFAULT) || defined(USE_LOTTEMART_DEFAULT) || defined(USE_LOTTESUPER_DEFAULT) || \
 	defined(USE_NHMART_SAFE_DEFAULT) || defined(USE_HYUNDAI_DEFAULT) || defined(USE_TOPMART_DEFAULT)
	{  321,	     4,		GLOBAL_SALE_SETUP_FLAG10,	FLASH_GLOBAL_AREA,		1,    	    4,	     2,	       3,	COUNTRY_UN}, //Menu1873 Use saving trace. no : //csh 20140303
 #elif defined(USE_EMART_DEFAULT)
	{  321,	     6,		GLOBAL_SALE_SETUP_FLAG10,	FLASH_GLOBAL_AREA,		1,    	    4,	     2,	       3,	COUNTRY_UN}, //Menu1873 Use saving trace. no : //csh 20140303
 #elif defined(USE_STANDARD_TRACE_DEFAULT)
	{  321,	     4,		GLOBAL_SALE_SETUP_FLAG10,	FLASH_GLOBAL_AREA,		1,    	    4,	     2,	       3,	COUNTRY_UN}, //Menu1873 Use saving trace. no :
 #endif
#endif	//USE_TRACE_STANDALONE
#ifdef USE_NHMART_SAFE_DEFAULT
	{  322,	     1,		GLOBAL_SALE_SETUP_FLAG10,	FLASH_GLOBAL_AREA,		1,    	    4,	     4,	       2,	COUNTRY_UN}, //Menu1873 Use sending trace. Info:
#else
	{  322,	     0,		GLOBAL_SALE_SETUP_FLAG10,	FLASH_GLOBAL_AREA,		1,    	    4,	     4,	       2,	COUNTRY_UN}, //Menu1873 Use sending trace. Info:
#endif
#if (defined(USE_GSMART_DEFAULT) || defined(USE_GSMART_TRACE_DEFAULT) || defined(USE_LOTTEMART_DEFAULT))
	{  323,	     2,		GLOBAL_SALE_SETUP_FLAG10,	FLASH_GLOBAL_AREA,		1,    	    4,	     6,	       2,	COUNTRY_KR}, //Menu1873 Use double/togle key 
#endif
	{  324,	     3,		GLOBAL_SALE_SETUP_FLAG11,	FLASH_GLOBAL_AREA,		1,    	    4,	     3,	       4,	COUNTRY_UN}, //Menu1873 Double key delay time(100ms):
#if defined(USE_LOTTESUPER_DEFAULT) || defined(USE_EMART_DEFAULT)
	{  330, 	 0, 	GLOBAL_PRT_HEADUP_SENSOR,	FLASH_GLOBAL_AREA,		1,		1,		 0, 	   0,	COUNTRY_KR}, //Active Headup sensor
#else
	{  330, 	 1, 	GLOBAL_PRT_HEADUP_SENSOR,	FLASH_GLOBAL_AREA,		1,		1,		 0, 	   0,	COUNTRY_UN}, //Active Headup sensor
#endif
#ifdef USE_EMART_PRODUCTION_KEY
	{  331, 	 1, 	GLOBAL_SALE_SETUP_FLAG19,	FLASH_GLOBAL_AREA,		1,		1,		 7, 	   1,	COUNTRY_UN}, //Menu1873 Use Regist Product Key
#endif
#if (defined(USE_GSMART_TRACE_DEFAULT) || defined(USE_GSMART_DEFAULT))
	{  340,	    50,		GLOBAL_SCROLL_WAIT_TIME,	FLASH_GLOBAL_AREA,  		2,	    2,	     0,	       0,	COUNTRY_KR}, //Menu1431 Wait Time(1s) :
#endif
#ifdef USE_CHECK_CHANGE_REASON
#ifdef USE_HYUNDAI_DEFAULT
	{  341,	    0,		GLOBAL_SALE_SETUP_FLAG12,	FLASH_GLOBAL_AREA,  		1,	    4,	     4,	       1,	COUNTRY_KR}, //Menu1873 Ask reason when price changed
#else
	{  341,	    1,		GLOBAL_SALE_SETUP_FLAG12,	FLASH_GLOBAL_AREA,  		1,	    4,	     4,	       1,	COUNTRY_KR}, //Menu1873 Ask reason when price changed
#endif
#endif
#ifdef USE_EMART_DEFAULT
	{  345, 	 1, 	GLOBAL_SALE_SETUP_FLAG14,		FLASH_GLOBAL_AREA,		1,			4,		 2, 	   1,	COUNTRY_KR},  // csh 20140226
#endif
#ifdef USE_HYUNDAI_MEAT_FUNCTION
	{  346, 	 0, 	GLOBAL_SALE_SETUP_FLAG2,	FLASH_GLOBAL_AREA,		  1,		4,		 4, 	   1,		COUNTRY_KR}, // Menu1873 현대 전용 축산 기능 Y/N
#endif
	{  0,	     0,		0,			     	0,				0,          0,	     0,	       0,	COUNTRY_UN} //End
};
#endif	//ifndef USE_TRANSFER_MENU_SETTING

// 나라별 default setting
// 1. ParameterCountry 의 funcCode를 search
// 2. COUNTRY_UN 을 default로 load
// 3. 원하는 나라의 default(COUNTRY_KR)가 존재하면 재 load
// 4. ParameterSetting,ParameterFactory에서 해당 function code를  search하여 메모리에 write
//new								    					    
ROMDATA COUNTRY_SETTING_TYPE	ParameterCountry[]=
{
	// funcCode, country, defaultValue

	{  501,	     COUNTRY_UN,	1   },//ride position
	{  501,	     COUNTRY_KR, 	1   },
	
	{  528,	     COUNTRY_RU,	0   },//No Print
	{  528,	     COUNTRY_UN,	0   },//No Print 
#ifdef USE_JULIAN_DATE
	{  529,	     COUNTRY_RU,	0   },
	{  529,	     COUNTRY_UN,	0   },
#endif
#if defined USE_MON_EMART || defined USE_PERSIA_DEFAULT
	{  530,	     COUNTRY_UN,	0   },//Date Format 표시값은 1이 중가됨.
#else
	{  530,	     COUNTRY_UN,	3   },//Date Format 표시값은 1이 중가됨.
#endif
  	{  530,	     COUNTRY_KR, 	0   },
	{  530,	     COUNTRY_US, 	1   },
	{  530,	     COUNTRY_CN,	0   },

	{  531,	     COUNTRY_UN,	0   },//Time Format
	{  531,	     COUNTRY_CN,	2   },//Time Format
#ifdef USE_HUNGARY_DEFAULT
	{  532,	     COUNTRY_UN,	1   },//sell by date Format
#else
	{  532,	     COUNTRY_UN,	0   },//sell by date Format
#endif
	{  532,	     COUNTRY_RU,	1   },//sell by date Format

	{  533,	     COUNTRY_UN,	0   },//Multi unit
	{  533,	     COUNTRY_US,	1   },//Multi unit

	{  534,	     COUNTRY_UN,	1   },//Default Unit Weight
	{  534,	     COUNTRY_KR, 	2   },
#ifdef USE_CHINA_CARRFOUR_DEFAULT
	{  534,	     COUNTRY_CN, 	3   },//Weight unit(1kg,100g,500g)
#else
	{  534,	     COUNTRY_CN, 	1   },//Weight unit(1kg,100g,500g)
#endif
//	{  534,	     COUNTRY_US, 	1   },

	{  535,	     COUNTRY_UN,	12  },//Display Message Time(0.1s)
#ifndef USE_CHINA_DEFAULT
	{  536,	     COUNTRY_UN,	0   },//Use Kg/Lb	// Added by CJK 20050929
//	{  536,	     COUNTRY_US,	1   },//Use Kg/Lb	// Modified by JJANG 20070704				

	{  537,	     COUNTRY_UN,	0   },//Use kg/lb auto conversion	// Added by CJK 20050930

	{  538,	     COUNTRY_UN,	0   },//Use 1/2lb,1/4lb Key
	{  538,	     COUNTRY_US,	1   },//Use 1/2lb,1/4lb Key
#else
	{  537, 	 COUNTRY_CN,	0	},//Enable Gram Weight print (0: kg, 1: g)
#endif
	{  540,	     COUNTRY_UN,	0   },//Barcode Price  Cut
#ifdef USE_SHINSEGAE_DEFAULT
	{  540,	     COUNTRY_KR,	1   },//Barcode Price  Cut
#endif
	{  541,	     COUNTRY_UN,	0   },//Barcode Weight Cut

	{  542,	     COUNTRY_UN,	0   },//Barcode Itemcode  Cut
	{  542,	     COUNTRY_KR,	1   },//Barcode Itemcode  Cut
	{  542,	     COUNTRY_CN,	1   },//Barcode Itemcode  Cut

#ifdef USE_HUNGARY_DEFAULT
	{  543,	     COUNTRY_UN,	1   },//Weight Barcode print Setting       //add by son 070123
#else
	{  543,	     COUNTRY_UN,	0   },//Weight Barcode print Setting       //add by son 070123
#endif
#ifdef USE_TOPMART_DEFAULT
	{  543, 	 COUNTRY_KR,	0	},
#else
	{  543, 	 COUNTRY_KR,	3	},//Weight Barcode print Setting	   //add by son 070123
#endif
#ifdef USE_CHINA_CARRFOUR_DEFAULT
	{  543, 	 COUNTRY_CN,	1	},//Weight Barcode print Setting 
#else
	{  543, 	 COUNTRY_CN,	0	},//Weight Barcode print Setting 
#endif
	
	{  545,	     COUNTRY_UN,	0   },//Size of Display Error Message (menu font size)
	{  545,	     COUNTRY_US,	1   },//Size of Display Error Message (Plu font size)

	{  546,	     COUNTRY_UN,	0   },//Disable Frequenct Shopper Program (FSP)
	{  546,	     COUNTRY_US,	1   },//Enable Frequenct Shopper Program (FSP)
#ifdef USE_HUNGARY_DEFAULT
	{  548,	     COUNTRY_UN,	2   },// Nutrition fact Type
#elif defined(USE_AUSTRALIA_DEFAULT)
	{  548,	     COUNTRY_UN,	4   },// Nutrition fact Type
#elif defined(USE_EU_NUTRITIONFACT)
	{  548,	     COUNTRY_UN,	8   },// Use EU Nutrition fact
#else
	{  548, 	 COUNTRY_UN,	0	},
#endif
#ifdef USE_GRB_HMART_DEFAULT
	{  548, 	 COUNTRY_EU,	8	}, //Nutrifact Print
#endif
	{  550,		 COUNTRY_UN, 	200 }, //Out Of Paper Threshold

	{  551,	     COUNTRY_UN,	0   }, //20080609  Use double print(auto)
	{  555,	     COUNTRY_UN,	0   },//Packed Time(dep.Sellbytime)
//	{  555,	     COUNTRY_KR,	1   },//Packed Time(dep.Sellbytime)	// Deleted by CJK 20061120

#if defined USE_AUSTRALIA_DEFAULT || defined USE_PERSIA_DEFAULT
	{  556,	     COUNTRY_UN,	1   },//Use PS2 port for Scanner
#else
	{  556,	     COUNTRY_UN,	0   },//Use PS2 port for Scanner
#endif
	{  556,	     COUNTRY_CN,	1   },//Use PS2 port for Scanner	
	
#ifdef USE_AUSTRALIA_DEFAULT
	{  557, 	 COUNTRY_UN,	1	},//Use PS2 port for Scanner
#else
	{  557,	     COUNTRY_UN,	0   },//Scanner Working Mode
#endif
#ifdef	USE_GERMANY_TARE_SCANNER
	{  556,	     COUNTRY_UN,	1   },//Use PS2 port for Scanner
#endif
#if defined(USE_STANDARD_TRACE_DEFAULT) || defined(USE_GSMART_TRACE_DEFAULT) || \
	defined(USE_EMART_DEFAULT) || defined(USE_LOTTEMART_DEFAULT) || defined(USE_LOTTESUPER_DEFAULT) || \
	defined(USE_TOPMART_DEFAULT) || defined(USE_NHMART_DEFAULT) || defined(USE_NHMART_SAFE_DEFAULT) || \
	defined(USE_HYUNDAI_DEFAULT)
	{  556,	     COUNTRY_KR,	1   },//Use PS2 port for Scanner
	{  557,	     COUNTRY_KR,	4   },//Scanner Working Mode
#endif

	{  558,	     COUNTRY_UN,	1   },//Scanner Mapping Format

	{  560,	     COUNTRY_UN,	0   },//use special price for d.c

	{  561,	     COUNTRY_UN,	0   },
#if defined(USE_GSMART_TRACE_DEFAULT) || defined(USE_TOPMART_DEFAULT)
	{  561,	     COUNTRY_KR,	3   },		//개체관리유형
#elif defined(USE_EMART_DEFAULT)
	{  561,	     COUNTRY_KR,	5   },
#elif defined(USE_LOTTEMART_DEFAULT)
	{  561, 	 COUNTRY_KR,	1	},
#elif defined(USE_NHMART_DEFAULT) || defined(USE_NHMART_SAFE_DEFAULT)
	{  561, 	 COUNTRY_KR,	2	},
#elif defined(USE_STANDARD_TRACE_DEFAULT) || defined(USE_LOTTESUPER_DEFAULT) || \
	  defined(USE_HYUNDAI_DEFAULT)
	{  561, 	 COUNTRY_KR,	4	},
#endif

	{  562,	     COUNTRY_UN,	1   },//GLOBAL_TIME_CASHDRAWEROPEN

	{  563,	     COUNTRY_UN,	1   },//Allow reverse feed at continuous label

	{  564,	     COUNTRY_UN,	0   },//Automatic Sellbydate
	{  564,	     COUNTRY_KR,	1   },//Automatic Sellbydate

	{  565,	     COUNTRY_UN,	1   },//Training clerk print	    //Add by se-hyung 20070627

	{  566,	     COUNTRY_UN,	0   },//Check Connection of Master's Remote ip
	{  567, 	 COUNTRY_UN,	0	},
#ifdef USE_VIETNAM_EMART_DEFAULT
	{  567, 	 COUNTRY_UN,	1	},
#endif
#if defined (USE_TOPMART_DEFAULT)
	{  567, 	 COUNTRY_KR,	1	},
#endif
#if defined(USE_LATVIA_DEFAULT) || defined(USE_AUSTRALIA_DEFAULT)
	{  568,	     COUNTRY_UN,	1   },//Display total info. (Total information)
#endif

#ifdef USE_HUNGARY_DEFAULT
	{  570,	     COUNTRY_UN,	1   },//Bycount Sale (WT.on Tray)
#else
	{  570,	     COUNTRY_UN,	0   },//Bycount Sale (WT.on Tray)
#endif
	//{  570,	     COUNTRY_KR,	1   },//Bycount Sale (WT.on Tray)  //국내는Y면 인증 위반
	{  570,	     COUNTRY_RU,	1   },//Bycount Sale (WT.on Tray)  // Add by SG 20051026
	{  570,	     COUNTRY_CN,	1   },//Bycount Sale (WT.on Tray)
#if defined(USE_VIETNAM_DEFAULT) || defined(USE_VIETNAM_LOTTEMART_DEFAULT)
	{  570,	     COUNTRY_ETC,	1   },//Bycount Sale (WT.on Tray)
#else
	{  570,	     COUNTRY_ETC,	0   },//Bycount Sale (WT.on Tray)
#endif

	{  571,	     COUNTRY_UN,	0   },//Print Weight(Bycount Sale)
	{  571,	     COUNTRY_US, 	1   },	       

#if defined(USE_AUSTRALIA_DEFAULT)
	{  573,	     COUNTRY_UN, 	1   },//Allow Unit Price Change
#else
	{  573,	     COUNTRY_UN, 	0   },//Allow Unit Price Change
#endif
#if defined(USE_LOTTEMART_DEFAULT) || defined(USE_SUNWOO_POS_DEFAULT) || defined(USE_LOTTESUPER_DEFAULT) || defined(USE_BESTCO_DEFAULT)
	{  573,	     COUNTRY_KR, 	0   },//Allow Unit Price Change
#else //USE_GSMART_TRACE_DEFAULT
	{  573,	     COUNTRY_KR, 	1   },//Allow Unit Price Change
#endif
	{  574,	     COUNTRY_UN, 	0   },// Print Zero Price
#if defined(USE_LOTTEMART_DEFAULT) || defined(USE_NHMART_DEFAULT) || \
	defined(USE_LOTTESUPER_DEFAULT) || defined(USE_TOPMART_DEFAULT) || \
	defined(USE_EMART_DEFAULT) || defined(USE_BESTCO_DEFAULT)
    {  574,	     COUNTRY_KR, 	0   },// Print Zero Price
#else
	{  574,	     COUNTRY_KR, 	1   },// Print Zero Price
#endif
	{  574,	     COUNTRY_CN, 	1   },// Print Zero Price
#if defined(USE_VIETNAM_EMART_DEFAULT) || defined(USE_SHINSEGAE_DEFAULT)
	{  575, 	 COUNTRY_UN,	1	},// Use E-Mart Barcode
#else
	{  575,	     COUNTRY_UN, 	0   },// Use E-Mart Barcode
#endif
	{  575,	     COUNTRY_KR, 	1   },// Use E-Mart Barcode
	{  575,	     COUNTRY_CN, 	1   },// Use E-Mart Barcode

	{  576,	     COUNTRY_UN, 	0   },// Hide Fixed Price
	{  576,	     COUNTRY_KR, 	1   },// Hide Fixed Price

	{  577,	     COUNTRY_UN, 	0   },// Use Common Barcode #
#if defined(USE_LOTTEMART_DEFAULT) || defined(USE_NHMART_DEFAULT) || defined(USE_LOTTESUPER_DEFAULT) || defined(USE_TOPMART_DEFAULT)
	{  577,	     COUNTRY_KR, 	0   },// Use Common Barcode #
#else
	{  577,	     COUNTRY_KR, 	1   },// Use Common Barcode #
#endif
	{  578,	     COUNTRY_UN, 	0   },// Use Common Label #
	{  578,	     COUNTRY_KR, 	1   },// Use Common Label #

	{  579,	     COUNTRY_UN, 	0   },// KS_TYPE -> Use Temporary Key
	{  579,	     COUNTRY_KR, 	1   },// KS_TYPE -> Use Temporary Key

	{  581,	     COUNTRY_UN, 	0   },//Print only WT.(u.p.zero)
#ifdef USE_STOKROTKA_MART_DEFAULT
	{  581,	     COUNTRY_EU, 	1   },//Print only WT.(u.p.zero)
#else
	{  581,	     COUNTRY_EU, 	0   },//Print only WT.(u.p.zero)
#endif

#if defined USE_LOTTEMART_DEFAULT || defined(USE_GSMART_DEFAULT) || \
	defined(USE_GSMART_TRACE_DEFAULT) || defined(USE_TRACE_MEATGRADE_MEATPRICE_DEFAULT) || \
	defined (USE_LOTTESUPER_DEFAULT)
	{  581, 	 COUNTRY_KR,	1	},//Print only WT.(u.p.zero)
#endif

#ifdef USE_AUSTRALIA_DEFAULT
	{  582,	     COUNTRY_UN,	1   },//Use 'X' key (byWT.)
#else
	{  582,	     COUNTRY_UN,	0   },//Use 'X' key (byWT.)
#endif
	{  582,	     COUNTRY_KR, 	1   },

	{  583,	     COUNTRY_UN,	0   },//Allow Continues sale (add up)

	{  584,	     COUNTRY_UN,	0   },//Use Tare ID
#ifdef USE_INDIA_TATA_DEFAULT
	{  584,	     COUNTRY_EU,	1   },//Use Tare ID
#endif

	{  585,	     COUNTRY_UN,	1   },//Use U.Price for key discount
#if defined(USE_GSMART_TRACE_DEFAULT) || defined(USE_GSMART_DEFAULT) || \
	defined(USE_TOPMART_DEFAULT) || defined(USE_LOTTEMART_DEFAULT) || \
	defined(USE_NHMART_DEFAULT) || defined(USE_NHMART_SAFE_DEFAULT) || \
	defined(USE_STANDARD_TRACE_DEFAULT) || defined(USE_HYUNDAI_DEFAULT)
	{  585,	     COUNTRY_KR,	0   },//Use U.Price for key discount
#endif
	{  585,	     COUNTRY_RU,	0   },//Use U.Price for key discount // Add by SG 20051026

	{  586,	     COUNTRY_UN, 	0   },

	{  587,	     COUNTRY_UN,	1   },	//Override only u.p.zero PLU
#ifdef USE_AUSTRALIA_DEFAULT
	{  588,	     COUNTRY_UN,	1   },	//Save Overrided u.p.zero PLU: [ ]
#else
	{  588,	     COUNTRY_UN,	0   },	//Save Overrided u.p.zero PLU: [ ]
#endif
	{  588,	     COUNTRY_KR,	1   },	//Save Overrided u.p.zero PLU: [ ]
#if defined(USE_VIETNAM_DEFAULT) || defined(USE_VIETNAM_LOTTEMART_DEFAULT)
	{  588,	     COUNTRY_ETC,	1   },	//Save Overrided u.p.zero PLU: [ ]
#else
	{  588,	     COUNTRY_ETC,	0   },	//Save Overrided u.p.zero PLU: [ ]
#endif

#ifdef USE_AUSTRALIA_DEFAULT
	{  589,	     COUNTRY_UN,	2   },	// select price round
#else
	{  589,	     COUNTRY_UN,	0   },	// select price round
#endif
	{  589,	     COUNTRY_KR,	4   },
	{  589,	     COUNTRY_CN,	1   },

	{  590,	     COUNTRY_UN,	1   },	// round off	// Modified by CJK 20050928

#ifdef USE_AUSTRALIA_DEFAULT
	{  591,	     COUNTRY_UN,	5   },
#elif defined(USE_BESTCO_DEFAULT)
	{  591,	     COUNTRY_KR,	10  }, // Price Round Unit
#else
	{  591,	     COUNTRY_UN,	0   }, // Price Round Unit
#endif

	{  592,	     COUNTRY_UN,	0   }, // Weight Round Unit
	{  592,	     COUNTRY_KR,	0   },

	{  593,	     COUNTRY_UN,	1   }, // Use By pcs PLU
	{  593,	     COUNTRY_KR,	0   },

	{  594,	     COUNTRY_UN,	0   }, // Print under Min. Weight
#if defined (USE_INDIA_TATA_DEFAULT) 
	{  594,	     COUNTRY_EU,	1   },
#endif

#if defined (USE_GSMART_DEFAULT)
	{  594,	     COUNTRY_KR,	0   },
#endif

#if defined(USE_VIETNAM_DEFAULT)|| defined(USE_VIETNAM_LOTTEMART_DEFAULT)
	{  594,	     COUNTRY_ETC,	1   }, // pirnt under min. weight
#else
	{  594,	     COUNTRY_ETC,	0   }, //Print under Min. Weight
#endif
	{  594,	     COUNTRY_CN,	1   },

	{  595,	     COUNTRY_UN,	0   },//Indvisual reset(Z Report)

	{  596,	     COUNTRY_UN,	1   },//X/Z Report
	{  596,	     COUNTRY_KR, 	1   },

	{  597,	     COUNTRY_UN,	1   },//Prepack report

	{  598,	     COUNTRY_UN,	1   },//Print when Z Report clear

#ifdef USE_HUNGARY_DEFAULT
	{  600,	     COUNTRY_UN, 	0   },//Auto Clear key tare,NTEP,OIML
#else
	{  600,	     COUNTRY_UN, 	1   },//Auto Clear key tare,NTEP,OIML
#endif
 	{  600,	     COUNTRY_KR, 	0   },	// CJK061120, 김백춘과장 요청에 의해 수정

	{  601,	     COUNTRY_UN, 	1   },//Auto Clear weight tare,NTEP,OIML
	{  601,	     COUNTRY_KR, 	0   },	// CJK061120, 김백춘과장 요청에 의해 수정

	{  602,	     COUNTRY_UN, 	0   },//Use canadian tare

	{  603,	     COUNTRY_UN, 	0   },//PLU Tare override by WT. Tare


	{  604,	     COUNTRY_UN,	0   },//Keep Key Tare(PLU sale)
	{  604,	     COUNTRY_KR, 	1   },

#ifdef USE_HUNGARY_DEFAULT
	{  605,	     COUNTRY_UN,	1   },//Accumulation Tare(Y/N)
#else
	{  605,	     COUNTRY_UN,	0   },//Accumulation Tare(Y/N)
#endif
#ifdef USE_STOKROTKA_MART_DEFAULT
	{  605,	     COUNTRY_EU,	1   },//Accumulation Tare(Y/N)
#else
	{  605,	     COUNTRY_EU,	0   },//Accumulation Tare(Y/N)
#endif
	{  605,	     COUNTRY_KR, 	1   },
	{  605,	     COUNTRY_RU, 	1   },

	{  606,	     COUNTRY_UN, 	0   },//Subtraction Tare(Y/N)
	{  606,	     COUNTRY_RU, 	1   },//Subtraction Tare(Y/N)

	{  607,	     COUNTRY_UN, 	0   },//Tare Input Flag(0,1....)

	{  608,	     COUNTRY_UN, 	0   },//After Plu call
	{  608,	     COUNTRY_US, 	1   },//After Plu call


	{  609,	     COUNTRY_UN, 	0   },//After Plu call
	{  610,	     COUNTRY_UN, 	1   },//

	{  611,	     COUNTRY_UN, 	1   },//JJANG 20090420 전국가 적용

	{  612,	     COUNTRY_UN, 	0   },//Use Neg. Weight Limit

	{  614,	     COUNTRY_UN,        0   },//Clerk lock type		//Added by JJANG 20080219
	{  615,	     COUNTRY_UN,        1   },
	{  616,	     COUNTRY_UN,        0   }, //hold time

#ifdef USE_AUSTRALIA_DEFAULT
	{  617,	     COUNTRY_UN,        1   }, //Modify by se-hyung for Clerk Subtotal List 20070727
#else
	{  617,	     COUNTRY_UN,        0   }, //Modify by se-hyung for Clerk Subtotal List 20070727
#endif

#ifdef USE_PERSIA_DEFAULT
	{  620,	     COUNTRY_UN,        9   }, // Use RealTime Transaction
#else
	{  620,	     COUNTRY_UN,        0   }, // Use RealTime Transaction
#endif

#if defined(USE_TRACE_MEATGRADE_MEATPRICE_DEFAULT)
	{  620,	     COUNTRY_KR,        8   },// Use RealTime Transaction
#elif defined(USE_EMART_DEFAULT) || defined(USE_LOTTEMART_DEFAULT) || \
	defined(USE_STANDARD_TRACE_DEFAULT) || defined(USE_LOTTESUPER_DEFAULT)
	{  620,	     COUNTRY_KR,        4   },// Use RealTime Transaction
#elif defined(USE_NHMART_DEFAULT) || defined(USE_NHMART_SAFE_DEFAULT)
	{  620,	     COUNTRY_KR,        10   },// Use RealTime Transaction
#endif

	{  624,	     COUNTRY_UN,	0   },//Net Error Type		//Added by JJANG 20080219
#ifdef USE_PERSIA_DEFAULT
	{  636,	     COUNTRY_UN,	2   },//Price Decimal Point
#else
	{  636,	     COUNTRY_UN,	1   },//Price Decimal Point
#endif
	{  636,	     COUNTRY_KR,	2   },

#ifdef USE_AUSTRALIA_DEFAULT
	{  637, 	 COUNTRY_UN,	1	},//Print output format(LABEL)
#elif defined(USE_HUNGARY_DEFAULT)
	{  637, 	 COUNTRY_UN,	2	},//Print output format(LABEL)
#else
	{  637,	     COUNTRY_UN,	0   },//Print output format(LABEL)
#endif
	{  637,	     COUNTRY_EU,	0   },//Print output format(LABEL)
	{  637,	     COUNTRY_US,	1   },
#ifdef USE_CHINA_ADD_DEFAULT
	{  637,	     COUNTRY_CN,	6   },
#endif
	{  638,	     COUNTRY_UN,	0   },

	{  639,	     COUNTRY_UN,	1   },//Print Repack Caption On Repack
	{  639,	     COUNTRY_CN,	0   },//Print Repack Caption On Repack

	{  679, 	 COUNTRY_UN,	0	},//PLU clear after remove weight
#ifdef USE_DOMINICA_DEFAULT
	{  679, 	 COUNTRY_US,	0	},//PLU clear after remove weight
#else
	{  679, 	 COUNTRY_US,	1	},//PLU clear after remove weight
#endif
	{  680, 	 COUNTRY_UN,	1	},//Transaction count reset by date 1-> reset by date, 0 -> don't reset.
	{  685, 	 COUNTRY_UN,	0	}, // %weight mode for Russia
	{  686, 	 COUNTRY_UN,	0	}, // Print Total price with external tax in barcode

	{  687,	     COUNTRY_UN,	0   },// Numeric Tare Block Disable[N] (Default 값)
#if defined(USE_INDIA_DEFAULT) || defined(USE_INDIA_DADUS_DEFAULT) || defined(USE_INDIA_TATA_DEFAULT)
	{  687,	     COUNTRY_EU,	1   },// Numeric Tare Block Enable[Y]
#else
	{  687,	     COUNTRY_EU,	0   },// Numeric Tare Block Disable[N] (Default 값)
#endif

	{  688,	     COUNTRY_UN,	0   },// Weight Tare Block Disable[N] (Default 값)
	{  690, 	 COUNTRY_UN,	0	},//Print under Min. Weight on prepack mode 

	{  691, 	 COUNTRY_UN,	0	},//Print Decimal Point on Barcode

#if defined(USE_INDIA_DEFAULT)
	{  699,		 COUNTRY_EU,  	1	},
#else
    {  699, 	 COUNTRY_EU,	0	},
	{  699,		 COUNTRY_UN,	0	},	
#endif

#ifdef USE_AUSTRALIA_DEFAULT
	{  700, 	 COUNTRY_UN,	1	},	//Total displaying time (sec), 0-> 멈춰있음
#else
	{  700,		 COUNTRY_UN,	0	},	//Total displaying time (sec), 0-> 멈춰있음
#endif
	{  700,		 COUNTRY_US,	1	},	//Total displaying time (sec), 0-> 멈춰있음
#ifdef USE_REPORT_TAX
	{  701,	     COUNTRY_UN,	1   },//Prt Tax(Report)
	{  701,	     COUNTRY_KR,	0   },
#endif

	{  702,	     COUNTRY_UN,	1   },//Prt Pay(Report)
	{  702,	     COUNTRY_KR,	0   },

	//{  703,	     COUNTRY_UN,	1   },//Prt RoundedSummary(Report)
	{  703,	     COUNTRY_UN,	0   },// CJK070322
	{  703,	     COUNTRY_KR,	0   },

	{  704,	     COUNTRY_UN,	0   },//Prt StoreName(Report)
	{  705,	     COUNTRY_KR,	0   },	//Prt Slaughter name on LOT list
	{  706,	     COUNTRY_UN,	0   },	//Using all individual list as harmful list, KOR-Meat에만 caption 존재
#ifdef USE_EMART_DEFAULT
	{  706,	     COUNTRY_KR,	99  },	//Using all individual list as harmful list, KOR-Meat에만 caption 존재
#else
	{  706,	     COUNTRY_KR,	20  },	//Using all individual list as harmful list, KOR-Meat에만 caption 존재
#endif
	{  707,	     COUNTRY_KR,	0   },  //Auto changing indiv field in label, KOR-Meat에만 caption 존재	

#ifdef USE_AUSTRALIA_DEFAULT
	{  708,	     COUNTRY_UN,	1   },	//Clerk key action in Label mode
#else
	{  708, 	 COUNTRY_UN,	0	},	//Clerk key action in Label mode
#endif
	{  708,	     COUNTRY_CN,	1   },	//Clerk key action in Label mode

#ifdef USE_CHECK_INDIV_INFO
  #ifdef USE_HYUNDAI_DEFAULT
  	{  709,	     COUNTRY_KR,	0   },	//Check Indiv info at Mtrace site.
  #else
	{  709,	     COUNTRY_KR,	1   },	//Check Indiv info at Mtrace site.
  #endif
#endif
	{  710,	     COUNTRY_UN,	0   }, // PLU name Scroll
#ifdef USE_STOKROTKA_MART_DEFAULT
	{  710,	     COUNTRY_EU,	1   }, // PLU name Scroll
#else
	{  710,	     COUNTRY_EU,	0   }, // PLU name Scroll
#endif
	{  711,	     COUNTRY_UN,	1   },//Prt Repack(Report)
	{  711,	     COUNTRY_KR,	0   },

	{  712,	     COUNTRY_UN,	1   },//Prt Prepack(Report)
	{  712,	     COUNTRY_KR,	0   },

	{  713,	     COUNTRY_UN,	1   },//Prt Negative(Report)
	{  713,	     COUNTRY_KR,	0   },

	{  714,	     COUNTRY_UN,	1   },//Prt NoSaleOpen(Report)
	{  714,	     COUNTRY_KR,	0   },

	{  715,	     COUNTRY_UN,	0   },//PLU Report Type

#if defined(USE_LOTTEMART_DEFAULT) || defined(USE_NHMART_DEFAULT) || \
	defined(USE_NHMART_SAFE_DEFAULT) || defined(USE_EMART_DEFAULT)
	{  715,	     COUNTRY_KR,	1   },
#endif

	{  716,	     COUNTRY_UN,	0   }, // 날짜 전송 타입(Send Sale date Type)

#if defined(USE_NHMART_DEFAULT) || defined(USE_NHMART_SAFE_DEFAULT)
	{  716,	     COUNTRY_KR,	1   },
#endif

#ifdef USE_TRACE_STANDALONE
  #ifdef USE_TOPMART_DEFAULT
	{  717, 	 COUNTRY_UN,		1	},// Use Checking Indiv.No
  #else
	{  717, 	 COUNTRY_UN,		0	},// Use Checking Indiv.No
  #endif
#ifdef USE_EMART_DEFAULT
	{  717,	     COUNTRY_KR,        0   },// Use Checking Indiv.No
	{  718,	     COUNTRY_KR,        10  }, //후방라벨 유효일.
#endif
#endif
#ifdef USE_DONT_SAVE_VALIDDATE_SALEDATE
	{  719,	     COUNTRY_UN,	0   }, // 유효일, 유효기간 저장하지 않음
#endif
	{  720,	     COUNTRY_UN,	1   },//Prt Zero Value(Report)

	{  721,	     COUNTRY_UN,	0   },//Report type

	{  722,	     COUNTRY_UN,	0   },//Clear clerk trans. in Z-rep.

	{  723,	     COUNTRY_UN,        0   },// Send Real Time U.P Change
#if defined(USE_EMART_DEFAULT) || defined(USE_LOTTESUPER_DEFAULT)
	{  723,	     COUNTRY_KR,        1   },// Send Real Time U.P Change
#endif

	{  725,	     COUNTRY_UN,	0   },	//Label format change key
	{  726,	     COUNTRY_UN,	0   },	// QR BARCODE ADD TOTAL

#if defined USE_GSMART_DEFAULT
	{  725,	     COUNTRY_KR,	0   },
#elif defined (USE_TOPMART_DEFAULT) || defined (USE_EMART_DEFAULT)
	{  725,	     COUNTRY_KR,	1   },
#endif

#if defined (USE_INDIA_TATA_DEFAULT)
	{  726,	     COUNTRY_EU,	1   },	// QR BARCODE ADD TOTAL
#endif
#ifdef USE_SET_QUICK_PLU_DELAY_TIME	
	{  727,	     COUNTRY_UN,	3   },	//Quick PLU Delay Time(1.5s)
  #if defined(USE_VIETNAM_DEFAULT) || defined(USE_VIETNAM_LOTTEMART_DEFAULT)
	{  727,	     COUNTRY_ETC,	0   },	//Quick PLU Delay Time(200ms)
  #endif
#endif
#ifdef USE_EFT_POS_PROTOCOL
	{  728,      COUNTRY_UN,    2   },	// EFT-POS PROTOCOL 사용(IRAN ONLY)
#endif
	{  729,	     COUNTRY_UN,	0   },//  Use Tare fielt with Member Price
	{  731,	     COUNTRY_UN,	0   },//Wireless Bridge
#if defined(USE_LOTTEMART_DEFAULT) || defined(USE_GSMART_DEFAULT) || \
	defined(USE_LOTTESUPER_DEFAULT) || defined(USE_TOPMART_DEFAULT) || \
	defined(USE_EMART_DEFAULT)
	{  731,	     COUNTRY_KR,	1   },//Wireless Bridge
#endif
#ifdef USE_STOKROTKA_MART_DEFAULT
	{  731,	     COUNTRY_EU,	0   },//Wireless Bridge
#elif defined(USE_GRB_HMART_DEFAULT)
	{  731,      COUNTRY_EU,    1   }, //Wireless Bridge
#endif
#ifdef USE_GSMART_TRACE_DEFAULT
	{  725,	     COUNTRY_KR,	1   },	//Label format change key
	{  731,	     COUNTRY_KR,	1   },//Wireless Bridge
#endif

	{  730,	     COUNTRY_UN,	1   },//Ethernet(TCP/IP)
#ifdef USE_PERSIA_DEFAULT
    {  732,	     COUNTRY_UN,	1   },//0: 상품명 폰트 1:상품명 메뉴폰트 사용 
#else
	{  732,	     COUNTRY_UN,	0   },//0: 상품명 폰트 1:상품명 메뉴폰트 사용 
#endif

#if defined(USE_NHMART_SAFE_DEFAULT) || defined(USE_LOTTEMART_DEFAULT) || \
	defined(USE_GSMART_TRACE_DEFAULT) || defined(USE_TOPMART_DEFAULT) || \
	defined(USE_EMART_DEFAULT)
	{  732,	     COUNTRY_KR,	1   },//0: 상품명 폰트 1:상품명 메뉴폰트 사용
#endif

	{  733,	     COUNTRY_UN,	2   },  // label-ticket key mode 1 // 2:label mode
	{  734,	     COUNTRY_UN,	0   },  // EFT POS displayed firstf
#ifdef USE_CHN_CART_SCALE
	{  735,	     COUNTRY_UN,	0   },  // Use check remain WT.(CHN):  //0:no
	{  736,	     COUNTRY_UN,	0   },  // Trace code type(CHN):  //0:meat, 1:vegetable
#endif

#ifdef USE_CL5200_DISP_CONTINUOUS_REFRESH
	{  737, 	 COUNTRY_UN,	1	},// USE DSIP CONTINOUS REFRESH : // 0:not use, 1:display continous refresh
#endif

	{  738,	     COUNTRY_UN,	0   },//Separate Quantity caption from Weight caption
#ifdef USE_STOKROTKA_MART_DEFAULT
	{  738,	     COUNTRY_EU,	1   },//Separate Quantity caption from Weight caption
#else
	{  738,	     COUNTRY_EU,	0   },//Separate Quantity caption from Weight caption
#endif
#ifdef USE_AUSTRALIA_DEFAULT
	{  739,	     COUNTRY_UN,	2   },//Weight,Quantity Unit symbol Print setting (0~2)
#else
	{  739,	     COUNTRY_UN,	0   },//Weight,Quantity Unit symbol Print setting (0~2)
#endif
#ifdef USE_CHINA_DEFAULT
	{  739, 	 COUNTRY_CN,	2	},//Weight,Quantity Unit symbol Print setting (0~2)
#endif
	{  740,	     COUNTRY_UN,	0   },//Unit price Unit symbol Print setting (0~5))
    {  742,	     COUNTRY_UN,	0   },//Realtime Add Data Send flag
	{  743,	     COUNTRY_UN,	0   },//Replace Fixed weight with Packing Price
	{  750,	     COUNTRY_UN, 	255 }, //Out Of Paper Threshold On Label
    {  751,	     COUNTRY_UN, 	0 }, //Send speedkey position via eth and 232
    {  765,	     COUNTRY_UN,	0    },//Realtime chk interval(sec)
#ifdef USE_EMART_DEFAULT
    {  765,	     COUNTRY_KR,	60   },//Realtime chk interval(sec)
#endif
	{  773,	     COUNTRY_UN,	0   },//Enable TAX
#ifdef USE_TAX_RATE
	{  773,	     COUNTRY_UN,	1   },//Enable TAX
	{  773,	     COUNTRY_US,	0   },
#endif
#if defined(USE_NHMART_DEFAULT) || defined(USE_NHMART_SAFE_DEFAULT)
	{  773,	     COUNTRY_UN,	0   },//Enable TAX
	{  773,	     COUNTRY_KR,	1   },
#endif
#if defined(USE_EMART_DEFAULT)
	{  773,	     COUNTRY_KR,	1   },
#endif

#if defined(COUNTRY_INDONESIA) || defined(USE_VIETNAM_DEFAULT) || defined(USE_VIETNAM_EMART_DEFAULT) || defined(USE_VIETNAM_LOTTEMART_DEFAULT) || defined(USE_INDONESIA_EMPORIUM) || defined(USE_MON_EMART)
	{  741,	     COUNTRY_UN,	1 	},//Enable Use 1000 separator Sign
#else
	{  741,	     COUNTRY_UN,	0 	},	
#endif
	
	{  746, 	 COUNTRY_UN,	0	},//Use Auto Zero Setting

#ifdef USE_EMART_DEFAULT
	{  752, 	 COUNTRY_KR,	1 },//이마트 전용 
#endif

#ifdef USE_DISPLAY_COMM_SPEED_SETTING
	{  770,	     COUNTRY_UN,	0   },//Displlay refresh speed & period setting 2 MHz 30ms
  #if defined(USE_VIETNAM_DEFAULT) || defined(USE_VIETNAM_LOTTEMART_DEFAULT)
	{  770,	     COUNTRY_ETC,	3   },//Displlay refresh speed & period setting 3 : 4 MHz 100ms
  #endif
#endif

	{  772,		 COUNTRY_UN,	0	},//Enable Discount
	{  781,		 COUNTRY_UN,	0	},//Enable Group
#ifdef USE_GSMART_TRACE_DEFAULT
	{  772,		 COUNTRY_UN,	0	},//Enable Discount
	{  772,	     COUNTRY_KR,	1	},
	{  781,		 COUNTRY_UN,	0	},//Enable Group
	{  781,	     COUNTRY_KR,	1	},
#endif
#if defined (USE_TOPMART_DEFAULT)
	{  772,		 COUNTRY_KR,	1	},//Enable Discount
#endif

#if defined (USE_TOPMART_DEFAULT)
	{  781,		 COUNTRY_KR,	1	},//Enable Group
#endif

	{  774,	     COUNTRY_UN,	1   },//Enable Origin

	{  775,	     COUNTRY_UN,	1   },//Enable Tare
	{  775,	     COUNTRY_US,	0   },

#ifdef USE_AUSTRALIA_DEFAULT
	{  776, 	 COUNTRY_UN,	1	},
#else
	{  776, 	 COUNTRY_UN,	0	},
#endif
#if defined(USE_EMART_DEFAULT)
	{  776, 	 COUNTRY_KR,	1	},
#endif

	{  778,	     COUNTRY_UN,	1   },//Enable Customer
	{  778,	     COUNTRY_US,	0   },

	{  779,	     COUNTRY_UN,	1   },//Enable Currency
	{  779,	     COUNTRY_US,	0   },
	{  782,	     COUNTRY_UN,	0   },	//Enable KOR Trace
#ifdef NOT_USE_TRACE_CL5200
	{  782,	     COUNTRY_KR,	0   },	//Enable KOR Trace
#else
	{  782,	     COUNTRY_KR,	1   },	//Enable KOR Trace
#endif
	{  783,	     COUNTRY_UN,	0 },// Print Label Under Min. Weight on Auto Mode
	{  784,	     COUNTRY_UN,	0 },// Print Label when Changing PLU in Auto Mode
#ifdef USE_CHECK_DUPLICATION_TICKET
	{  793,      COUNTRY_UN,    0   },//Use Continuous Duplication Ticket
#endif
	{  795,      COUNTRY_UN,    0   },//Use Key Tare With Weight

	{  796,	     COUNTRY_UN,	0   },//tare auto zero	se-hyung 20080325
	{  796,	     COUNTRY_CN,	1   },//tare auto zero	se-hyung 20080325

	{  797,	     COUNTRY_UN,	0   },//full tare

	{  798,	     COUNTRY_UN,	0   },//tare range
	{  798,	     COUNTRY_CN,	3   },//tare range
#if defined(USE_VIETNAM_EMART_DEFAULT) || defined(USE_VIETNAM_DEFAULT) || defined(USE_PERSIA_DEFAULT) || \
	defined(USE_MON_EMART_DEFAULT) || defined(USE_MONGOL_DEFAULT) || defined(USE_VENEZUELA_DEFAULT) || \
	defined(USE_LEBANON_DEFAULT) || defined(USE_HUNGARY_DEFAULT) || defined(USE_VIETNAM_LOTTEMART_DEFAULT)
	{  810,	     COUNTRY_UN,	0   },//Price Decimal Point
#else
	{  810,	     COUNTRY_UN,	2   },//Price Decimal Point
#endif
	{  810,	     COUNTRY_KR,	0   },
#if defined(USE_UZBEKISTAN_DEFAULT)
	{  810,	     COUNTRY_RU,	0   },
#endif

	{  831,	     COUNTRY_UN,	2   },//

	{  861,	     COUNTRY_UN, 	20  },
	{  861,	     COUNTRY_KR, 	50  },

	{  862,	     COUNTRY_UN, 	20  },
#if defined(USE_PERSIA_DEFAULT)
	{  863,	     COUNTRY_UN, 	1   },//Display dummy digit(weight)
#else
	{  863,	     COUNTRY_UN, 	0   },//Display dummy digit(weight)
#endif
	{  885,	     COUNTRY_UN,	2   },//00_Key format
	{  886,	     COUNTRY_UN,	30  },//Network Error Clear Timeout
	{  888,	     COUNTRY_UN,	30  },//Key Clear Time
#if defined(USE_VIETNAM_DEFAULT) || defined(USE_VIETNAM_LOTTEMART_DEFAULT)
    {  888,      COUNTRY_ETC,    7  },//Key Clear Time
#endif

#ifdef USE_CHECK_CAL_SW_TO_UPDATE_FW
	{  996,	     COUNTRY_UN,	0   },//Allow Fwupdate	인증용
#else
	{  996,	     COUNTRY_UN,	1   },//Allow Fwupdate	// Added by CJK 20060123
#endif
#ifdef USE_CHN_FONT
	{  980,	     COUNTRY_US,	1   },
	{  980,	     COUNTRY_CN,	1   },
#endif


//MENU SETTING DEFAULT
#ifdef USE_TRANSFER_MENU_SETTING
	{ 1810,	     COUNTRY_UN,	1   },	//Show Display info (M1871)
	{ 1811,	     COUNTRY_UN,	1   },	//Show Clerk Nickname (M1871)
#if defined (USE_CL5200_DISPLAY)
#ifdef USE_CHINA_ADD_DEFAULT
	{ 1812,	     COUNTRY_CN,	1   },	//PLU Name Align (M1871)
#else
	{ 1812,	     COUNTRY_CN,	0   },	//PLU Name Align (M1871)
#endif
#elif defined (USE_CL5500_DISPLAY) 
	{ 1812,	     COUNTRY_UN,	0   },	//PLU Name Align (M1871)
#else
	{ 1812,	     COUNTRY_UN,	2   },	//PLU Name Align (M1871)
#endif
#ifdef USE_GSMART_TRACE_DEFAULT
	{ 1812,	     COUNTRY_KR,	0   },	//PLU Name Align (M1871)
#else
	{ 1812,	     COUNTRY_KR,	1   },	//PLU Name Align (M1871)
#endif
#ifdef USE_EMART_DEFAULT
	{ 1508,	     COUNTRY_UN,	0   },	//Use Global barcode
#else
	{ 1508,	     COUNTRY_UN,	1   },	//Use Global barcode
#endif
	{ 1501,	     COUNTRY_UN,	1   },	//Use global label
#if defined(USE_VIETNAM_DEFAULT) || defined(USE_VIETNAM_LOTTEMART_DEFAULT)
	{ 1501, 	 COUNTRY_ETC,	0	},	//Use global label
#else
	{ 1501, 	 COUNTRY_ETC,	1	},	//Use global label
#endif
#ifdef USE_UKRAINE_DEFAULT
	{ 1501,      COUNTRY_RU,    0  },   //Use global label
#endif
#ifdef USE_STOKROTKA_MART_DEFAULT
	{ 1501,      COUNTRY_EU,    0  },   //Use global label
#endif

#if defined (COUNTRY_INDONESIA) || defined (USE_EMPORIUM_DEFAULT) || defined(USE_HEBREW_DEFAULT) || defined(USE_MON_EMART_DEFAULT)
	{ 1502,	     COUNTRY_UN,	51  },	//Standard Label ID
#else
	{ 1502,	     COUNTRY_UN,	10  },	//Standard Label ID
#endif
#if defined(USE_GSMART_TRACE_DEFAULT)
	{ 1502,	     COUNTRY_KR,	53  },	//Standard Label ID
#elif (defined(USE_GSMART_DEFAULT) || defined(USE_EMART_DEFAULT) || defined(USE_LOTTESUPER_DEFAULT) || defined(USE_TOPMART_DEFAULT))
	{ 1502,	     COUNTRY_KR,	51  },	//Standard Label ID
#elif defined(USE_NHMART_SAFE_DEFAULT)
	{ 1502,	     COUNTRY_KR,	90  },	//Standard Label ID
#else
	{ 1502,	     COUNTRY_KR,	31  },	//Standard Label ID
#endif
#ifdef USE_UKRAINE_DEFAULT
	{ 1502,	     COUNTRY_RU,	12  },	//Standard Label ID
#else
	{ 1502,	     COUNTRY_RU,	11  },	//Standard Label ID
#endif
#ifdef USE_CHINA_ADD_DEFAULT
	{ 1502,	     COUNTRY_CN,	 1  },	//Standard Label ID
#else
	{ 1502,	     COUNTRY_CN,	10  },	//Standard Label ID
#endif
#if defined(USE_VIETNAM_DEFAULT) || defined(USE_VIETNAM_LOTTEMART_DEFAULT)
	{ 1502, 	 COUNTRY_ETC,	51	},	//Standard Label ID
#else
	{ 1502, 	 COUNTRY_ETC,	10	},	//Standard Label ID
#endif
#ifdef USE_STOKROTKA_MART_DEFAULT
	{ 1502, 	 COUNTRY_EU,	51	},	//Standard Label ID
#elif defined (USE_INDIA_TATA_DEFAULT)
	{ 1502, 	 COUNTRY_EU,	78	},	//Standard Label ID
#else
	{ 1502, 	 COUNTRY_EU,	10	},	//Standard Label ID
#endif
#ifdef USE_DOMINICA_DEFAULT
	{ 1502,	     COUNTRY_US,	10  },	//Standard Label ID 도미니카는 영공 라벨 사양 적용(기본 라벨 10 번)
#elif defined(USE_NTEP_CERT_DEFAULT)
	{ 1502, 	 COUNTRY_US,	51	},	//Standard Label ID 인증용 라벨 적용 (인증용 펌웨어에만 적용)
#else
	{ 1502,	     COUNTRY_US,	1 	},	//Standard Label ID 미국 신규 라벨 적용 23.12.28
#endif

#ifdef USE_CHN_EMART_DEFAULT
	{ 1503,	     COUNTRY_UN,	31  },	//Misc. Label ID
#else
	{ 1503,	     COUNTRY_UN,	10  },	//Misc. Label ID
#endif
	{ 1503,	     COUNTRY_KR,	31  },	//Misc. Label ID
#ifdef USE_UKRAINE_DEFAULT
	{ 1503,	     COUNTRY_RU,	12  },	//Misc. Label ID
#else
	{ 1503,	     COUNTRY_RU,	11  },	//Misc. Label ID
#endif
#if defined(USE_VIETNAM_DEFAULT) || defined(USE_VIETNAM_LOTTEMART_DEFAULT)
	{ 1503, 	 COUNTRY_ETC,	51	},	//Misc. Label ID
#endif
#ifdef USE_CHINA_ADD_DEFAULT
	{ 1503,	     COUNTRY_CN,	 1  },	//Misc. Label ID
#else
	{ 1503,	     COUNTRY_CN,	31  },	//Misc. Label ID
#endif
#ifdef USE_STOKROTKA_MART_DEFAULT
	{ 1503, 	 COUNTRY_EU,	51	},	//Misc. Label ID
#else
	{ 1503, 	 COUNTRY_EU,	10	},	//Misc. Label ID
#endif
#ifdef USE_DOMINICA_DEFAULT
	{ 1503, 	 COUNTRY_US,	10	},	//Misc. Label ID 도미니카는 영공 라벨 사양 적용(기본 라벨 10 번)
#elif defined(USE_NTEP_CERT_DEFAULT)
	{ 1502, 	 COUNTRY_US,	51	},	//Standard Label ID 인증용 라벨 적용 (인증용 펌웨어에만 적용)
#else
	{ 1503, 	 COUNTRY_US,	1	},	///Misc. Label ID미국 신규 라벨 적용 23.12.28
#endif

	{ 1504,	     COUNTRY_UN,	29  },	//Add-up TTL Label ID
#ifdef USE_INDIA_TATA_DEFAULT
	{ 1504,	     COUNTRY_EU,	53  },	//Add-up TTL Label ID
#endif
	{ 1504,	     COUNTRY_KR,	29  },	//Add-up TTL Label ID
	{ 1504,	     COUNTRY_RU,	29  },	//Add-up TTL Label ID
#if defined(USE_VIETNAM_DEFAULT) || defined(USE_VIETNAM_LOTTEMART_DEFAULT)
	{ 1504, 	 COUNTRY_ETC,	52	},	//Add-up TTL Label ID
#endif

	{ 1506,	     COUNTRY_UN,	0  },	//Rotate Item Label
#if defined(USE_INDIA_TATA_DEFAULT)
	{ 1506,	     COUNTRY_EU,	1  },	//Reverse Item Label
#endif

	{ 1509, 	 COUNTRY_UN,	0  },	//PLU weight barcode No.
#if defined(USE_INDIA_TATA_DEFAULT)
	{ 1509, 	 COUNTRY_EU,	1  },	//PLU weight barcode No.
#endif

	{ 1510, 	 COUNTRY_UN,	0  },	//PLU count barcode No.
#if defined(USE_INDIA_TATA_DEFAULT)
	{ 1510, 	 COUNTRY_EU,	2  },	//PLU count barcode No.
#endif

	{ 1511, 	 COUNTRY_UN,	0  },	//PLU pcs barcode No.
#if defined(USE_INDIA_TATA_DEFAULT)
	{ 1511, 	 COUNTRY_EU,	2  },	//PLU pcs barcode No.
#endif

	{ 1516, 	 COUNTRY_UN,	0  },	//Add up TTL  barcode No.
#if defined(USE_INDIA_TATA_DEFAULT)
	{ 1516, 	 COUNTRY_EU,	3  },	//Add up TTL  barcode No.
#endif

	{ 1517, 	 COUNTRY_UN,	0  },	//Add up TTL  barcode2 No.

	{ 1101,	     COUNTRY_UN,	0  },	//Print PLU Test Label
	{ 1814,	     COUNTRY_UN,	0  },	//Label Prt. w.o Rewind (X Key)
	{ 1815,	     COUNTRY_UN,	2  },	//Confirm Prt Report on Label

	{ 1816,	     COUNTRY_UN,	0  },	//Print origin for comm. line:
	{ 1816,	     COUNTRY_KR,	2  },	//Print origin for comm. line:
#ifdef USE_CHINA_ADD_DEFAULT
	{ 1816,	     COUNTRY_CN,	2  },	//Print origin for comm. line:
#else
	{ 1816,	     COUNTRY_CN,	0  },	//Print origin for comm. line:
#endif
	{ 1788,	     COUNTRY_UN,	1  },	//Print Clerk name
	{ 1787,	     COUNTRY_UN,	1  },	//Print Store name
	{ 1782,	     COUNTRY_UN,	1  },	//Print List
	{ 1783,	     COUNTRY_UN,	1  },	//Print Total Barcode
	{ 1784,	     COUNTRY_UN,	0  },	//Print Total Barcode
	{ 1789,	     COUNTRY_UN,	0  },	//Print Total Barcode
	{ 1790,	     COUNTRY_UN,	1  },	//Prt. Tax Information
	
	{ 1802,	     COUNTRY_UN,	1  },	//Select Clerk Sale mode
	{ 1802,	     COUNTRY_EU,	1  },	//Select Clerk Sale mode
	{ 1802,	     COUNTRY_EUT,	3  },	//Select Clerk Sale mode

	{ 1805,	     COUNTRY_UN,	0  },	//M1840 Month Format (1=DEC,0=12) :
	{ 1805,	     COUNTRY_CN,	0  },	//M1840 Month Format (1=DEC,0=12) :
	{ 1806,	     COUNTRY_CN,	0  },	//M1840 Year  Format (0=2001,1=01):
	{ 1807,	     COUNTRY_CN,	0  },	//M1840 Time  Format (0=24,1=12, 2=24시간+초, 3=12시간+초)
#if defined(USE_VIETNAM_DEFAULT) || defined(USE_VIETNAM_LOTTEMART_DEFAULT)
	{ 1805, 	 COUNTRY_ETC,	0  },	//M1840 Month Format (1=DEC,0=12) :
	{ 1807, 	 COUNTRY_ETC,	2  },	//M1840 Time  Format (0=24,1=12, 2=24시간+초, 3=12시간+초)
#endif
#ifdef USE_STOKROTKA_MART_DEFAULT
	{ 1805,	     COUNTRY_EU,	0  },	//M1840 Month Format (1=DEC,0=12) :
#endif
	{ 1824,	     COUNTRY_UN,	0  },	//Save unit price change
	{ 1824,	     COUNTRY_KR,	1  },	//Save unit price change
#ifdef USE_CHINA_ADD_DEFAULT
	{ 1824,	     COUNTRY_CN,	1  },	//Save unit price change
#else
	{ 1824,	     COUNTRY_CN,	0  },	//Save unit price change
#endif
	{ 1824, 	 COUNTRY_ETC,	0  },	//Save unit price change
	{ 1825,	     COUNTRY_UN,	0  },	//Save Type change
	{ 1825,	     COUNTRY_KR,	1  },	//Save Type change
#ifdef USE_CHINA_ADD_DEFAULT	
	{ 1825,	     COUNTRY_CN,	1  },	//Save Type change
#else
	{ 1825,	     COUNTRY_CN,	0  },	//Save Type change
#endif
	{ 1826,	     COUNTRY_UN,	0  },	//Start save mode
	{ 1826,	     COUNTRY_KR,	1  },	//Start save mode
#ifdef USE_CHINA_ADD_DEFAULT
	{ 1826,	     COUNTRY_CN,	1  },	//Start save mode
#else
	{ 1826,	     COUNTRY_CN,	0  },	//Start save mode
#endif

	{ 1827,	     COUNTRY_UN,	0  },	//PLU Clear Aft Sale(WT. On Tray)
	{ 1827,	     COUNTRY_CN,	1  },	//PLU Clear Aft Sale(WT. On Tray)
	{ 1827,	     COUNTRY_EU,	1  },	//PLU Clear Aft Sale(WT. On Tray)
	{ 1827,	     COUNTRY_KR,	1  },	//PLU Clear Aft Sale(WT. On Tray)
#if defined(USE_VIETNAM_DEFAULT) || defined(USE_VIETNAM_LOTTEMART_DEFAULT)
	{ 1827,	     COUNTRY_ETC,	1  },	//PLU Clear Aft Sale(WT. On Tray)
#else
	{ 1827,	     COUNTRY_ETC,	0  },	//PLU Clear Aft Sale(WT. On Tray)
#endif
	{ 1820,	     COUNTRY_UN,	0  },	// Swedish Auto Barcode[0~2]
	{ 1519,	     COUNTRY_UN,	1  },	//Use Global TAX
#ifdef USE_GERMANY_DEFAULT
	{ 1838, 	 COUNTRY_UN,	0  },	//LogOff after Transaction
	{ 1839, 	 COUNTRY_UN,	0  },	//Clerk log out Time
#else
	{ 1838, 	 COUNTRY_UN,	1  },	//LogOff after Transaction
	{ 1839, 	 COUNTRY_UN,	30	},	//Clerk log out Time

#endif

	{ 1774,	     COUNTRY_UN,	10  },	//Print Intensity (0-20) --> when save (*30)//HYP 20040811
#if defined(USE_VIETNAM_DEFAULT) || defined(USE_VIETNAM_LOTTEMART_DEFAULT)
	{ 1774,	     COUNTRY_ETC,	18  },	//Print Intensity (0-20) --> when save (*30)//HYP 20040811
#else
	{ 1774,	     COUNTRY_ETC,	10  },	//Print Intensity (0-20) --> when save (*30)//HYP 20040811
#endif
#if defined (USE_STOKROTKA_MART_DEFAULT)
	{ 1774,	     COUNTRY_EU,	15  },	//Print Intensity (0-20) --> when save (*30)//HYP 20040811
#else
	{ 1774,	     COUNTRY_EU,	10  },	//Print Intensity (0-20) --> when save (*30)//HYP 20040811
#endif
#ifdef USE_CHN_CART_SCALE
	{ 1752,	     COUNTRY_UN,	1  },	//Print Mode(0-2)
#endif
	{ 1762,	     COUNTRY_UN,	128  },	//Gap Threshold
	{ 1763,	     COUNTRY_UN,	128  },	//Peel Threshold
#if defined(USE_INDIA_TATA_DEFAULT)
	{ 1766,	     COUNTRY_UN,	1  },	//Label Paper
	{ 1766,	     COUNTRY_EU,	0  },	//Label Paper
#endif
	{ 1776,	     COUNTRY_UN,	1  },	//Enable Preprint
	{ 1777,	     COUNTRY_UN,	10  },	//Preprint Length (0-10)
#if defined(USE_INDIA_TATA_DEFAULT)
	{ 1777,	     COUNTRY_EU,	5  },	//Preprint Length (0-10)
#endif
#if defined(USE_VIETNAM_DEFAULT) || defined(USE_VIETNAM_LOTTEMART_DEFAULT)
	{ 1777,	     COUNTRY_ETC,	4  },	//Preprint Length (0-10)
#else
	{ 1777,	     COUNTRY_ETC,	10  },	//Preprint Length (0-10)
#endif
#ifdef USE_GSMART_PLU
 #if defined(USE_GSMART_TRACE_DEFAULT) || defined(USE_GSMART_DEFAULT)
	{ 1777,	     COUNTRY_KR,	6  },	//Preprint Length (0-10)
 #elif defined(USE_EMART_DEFAULT)
 	{ 1777,	     COUNTRY_KR,	6  },	//Preprint Length (0-10)
 #elif defined(USE_NHMART_SAFE_DEFAULT)
 	{ 1777,	     COUNTRY_KR,	0  },	//Preprint Length (0-10)
 #else
	{ 1777,	     COUNTRY_KR,	8  },	//Preprint Length (0-10)
 #endif  
#endif
	{ 1775,	     COUNTRY_UN,	0  },	//Feed Adjustment (-200-200)
#ifdef USE_MON_EMART_DEFAULT
	{ 1798, 	 COUNTRY_UN, 	56 },	//Reverse Feed Adjustment (0-80)
#else
	{ 1798, 	 COUNTRY_UN, 	0  },	//Reverse Feed Adjustment (0-80)
#endif

#ifdef COUNTRY_DOMINICA 
	{ 1798,	     COUNTRY_US,	0  },	//Reverse Feed Adjustment (0-80)
#else
	{ 1798,	     COUNTRY_US,	200  },	//Reverse Feed Adjustment (0-80)
#endif
	{ 1817,	     COUNTRY_UN,	1  },	//print pcs symbol in qty area
	{ 1818,	     COUNTRY_UN,	1  },	//Print Pcs in Unit Price Area
	{ 1821,	     COUNTRY_UN,	0  },	//Prepack
	{ 1836,	     COUNTRY_UN,	0  },	// Start Auto Mode SG060928
	{ 1837, 	 COUNTRY_UN,	0  },	// Enable Quick PLU CJK070130
#if defined(USE_INDIA_TATA_DEFAULT)
	{ 1837,	     COUNTRY_EU,	1  },	// Enable Quick PLU CJK070130
#endif
#if defined(USE_VIETNAM_DEFAULT) || defined(USE_VIETNAM_LOTTEMART_DEFAULT)
	{ 1837,	     COUNTRY_ETC,	1  },	// Enable Quick PLU CJK070130
#else
	{ 1837,	     COUNTRY_ETC,	0  },	// Enable Quick PLU CJK070130
#endif
	{ 1822,	     COUNTRY_UN,	0  },	// Print Item when "Return" SG070215  
	{ 1830,	     COUNTRY_UN,	1  },	// Menu1873: T.Price=Price+Tax
#ifdef USE_GERMANY_DEFAULT
	{ 1828, 	 COUNTRY_UN,	1  },	//Menu1873 Print count w.o misc. key:
	{ 1829, 	 COUNTRY_UN,	1  },	//Menu1873 Print weight w.o misc. key :
	{ 1833,	     COUNTRY_UN,	10  },	//Menu1873 Double key delay time
#else
	{ 1828, 	 COUNTRY_UN,	1  },	//Menu1873 Print count w.o misc. key:
	{ 1828, 	 COUNTRY_CN,	0  },	//Menu1873 Print count w.o misc. key:
	{ 1829, 	 COUNTRY_UN,	1  },	//Menu1873 Print weight w.o misc. key :
	{ 1829, 	 COUNTRY_CN,	0  },	//Menu1873 Print weight w.o misc. key :
	{ 1833, 	 COUNTRY_UN,	3  },	//Menu1873 Double key delay time
#endif
#if defined (USE_GERMANY_DEFAULT)
	{ 1834,	     COUNTRY_UN,	1  },	//Menu1873 Use double DATE/TIME key
#else
	{ 1834,	     COUNTRY_UN,	0  },	//Menu1873 Use double DATE/TIME key
#endif
	{ 1834,	     COUNTRY_KR,	1  },	//Menu1873 Use double DATE/TIME key
#ifdef USE_CHINA_ADD_DEFAULT
	{ 1834,	     COUNTRY_CN,	1  },	//Menu1873 Use double DATE/TIME key
#else
	{ 1834,	     COUNTRY_CN,	0  },	//Menu1873 Use double DATE/TIME key
#endif
#if (defined(USE_GSMART_DEFAULT) || defined(USE_GSMART_TRACE_DEFAULT) || defined(USE_LOTTEMART_DEFAULT))
	{ 1832,	     COUNTRY_KR,	2  },	//Menu1873 Use double/togle key 
#elif defined (USE_GERMANY_DEFAULT)
	{ 1832, 	 COUNTRY_UN,	1  },	//Menu1873 Use double/togle key 
#endif
	{ 1773,	     COUNTRY_UN,	1  },	//Active Headup sensor

	{ 1840,	     COUNTRY_UN,	2  },	//Select Sale mode(MENU1810)
	{ 1840,	     COUNTRY_EUT,	1  },	//Select Sale mode(MENU1810)

	{ 1531,	     COUNTRY_UN,	1  },	//Discount Priority(MENU 1531)
    
#ifdef USE_ARAB
	{ 1071,	     COUNTRY_EU,	0  },	//Total Price Mark Down(MENU 1720)
	{ 1072,	     COUNTRY_EU,	0  },	//Unit Price Mark Down(MENU 1720)	
#else
	{ 1071,	     COUNTRY_UN,	1  },	//Total Price Mark Down(MENU 1720)
	{ 1071,	     COUNTRY_RU,	0  },	//Total Price Mark Down(MENU 1720)
	
	{ 1072,	     COUNTRY_UN,	1  },	//Unit Price Mark Down(MENU 1720)
	{ 1072,	     COUNTRY_RU,	0  },	//Unit Price Mark Down(MENU 1720)
#endif
	{ 1073,	     COUNTRY_UN,	0  },	//Weight Mark Down(MENU 1720)
	{ 1074,	     COUNTRY_UN,	0  },	//Quantity Mark Down(MENU 1720)

//	{ 1841,	     COUNTRY_UN, 	1  },  //   Use Double Speed Key(MENU 1873)
//	{ 1841,	     COUNTRY_KR, 	0  },  //   Use Double Speed Key(MENU 1873)

	{ 1835,	     COUNTRY_UN, 	1  },  // Multi-print on discount sale(MENU 1873)
	{ 1843,	     COUNTRY_UN, 	1  },  // Date-Print ON/OFF Key(#49) flag, No caption
	{ 1844,	     COUNTRY_UN, 	1  },  // Save Member Code(MENU 1873)
	{ 1845,	     COUNTRY_UN, 	1  },  //Enable scroll msg with PLU (MENU 1871)
	{ 1845,	     COUNTRY_US, 	0  },  //Enable scroll msg with PLU (MENU 1871)

#ifdef USE_TRACE_STANDALONE
 #if defined(USE_NHMART_DEFAULT)
	{ 1846,	     COUNTRY_UN, 	1  },  //Menu1873 Type of saving trace. no :, KOR-Meat에만 caption 존재
 #elif defined(USE_GSMART_TRACE_DEFAULT) || defined(USE_LOTTEMART_DEFAULT) || defined(USE_EMART_DEFAULT) || defined(USE_NHMART_SAFE_DEFAULT) || \
 	   defined(USE_HYUNDAI_DEFAULT) || defined(USE_TOPMART_DEFAULT) || defined(USE_STANDARD_TRACE_DEFAULT)
	{ 1846,	     COUNTRY_UN, 	4  },  //Menu1873 Type of saving trace. no :, KOR-Meat에만 caption 존재
 #else
	{ 1846,	     COUNTRY_UN, 	0  },  //Menu1873 Type of saving trace. no :, KOR-Meat에만 caption 존재
 #endif
#endif	//USE_TRACE_STANDALONE

	{ 1847,	     COUNTRY_UN, 	0  },  //Menu1873 Use sending trace. Info:, KOR-Meat에만 caption 존재
	{ 1848,	     COUNTRY_UN, 	0  },  //Menu1872 Print total label continously.

#ifdef USE_INDIA_FUNCTION
	{ 1849,	     COUNTRY_UN, 	0  },  //Menu1873 India Scale Mode (IND Only)
	{ 1850,	     COUNTRY_UN, 	0  },  //Menu1873 Use Ext.Barcode as ItemCode (IND Only)
	{ 1851,	     COUNTRY_UN, 	1  },  //Menu1873 Allow Negative Inventory (IND Only)
#endif
#if defined(USE_CHECK_CHANGE_REASON) || defined(USE_HYUNDAI_CHECK_REASON_CODE)
	{ 1852,	     COUNTRY_KR, 	1  },  //Menu1873 Ask reason when price changed
#endif
#ifdef USE_HYUNDAI_DEFAULT
	{ 1853,	     COUNTRY_KR,	1  },	//Menu 1872 Print Item when "Return"
#endif
#if (defined(USE_GSMART_TRACE_DEFAULT) || defined(USE_GSMART_DEFAULT))
	{ 1431,	     COUNTRY_KR, 	50  },  //Menu1431 Wait Time(1s) :
#endif

#ifdef USE_QR_SIZE_SETTING
	{ 1857,	     COUNTRY_UN, 	3  },  //Menu1873 Ticket QR Size in Add-Total, size 1~5
#endif /* USE_QR_SIZE_SETTING */
#ifdef USE_POLAND_DEFAULT
	{ 1939,	     COUNTRY_UN,	0  },	//Menu1873 Close ticket without print
#else
	{ 1939,	     COUNTRY_UN,	1  },	//Menu1873 Close ticket without print
#endif
	// 초기값이 없는 funcCode 일부 추가(Menu 1872, 1873)
	{ 1819,	     COUNTRY_UN,	0  },	// Menu1872 Print all report within range
	{ 1823,	     COUNTRY_UN,	0  },	// Menu1872 Print Label when Label #0 
	{ 1831,	     COUNTRY_UN,	0  },	// Menu1873 Shift key instead return key
	{ 1832,	     COUNTRY_UN,	0  },	// Menu1873 Use double speed key
	{ 1854,	     COUNTRY_UN,	0  },	// Menu1873 Use Ext.Prt on Prepack
#ifdef USE_INIT_MASK
	{ 1940,      COUNTRY_UN,    0  },   // Menu 1891 Keep Init Settings - Printer Hardware
	{ 1941,      COUNTRY_UN,    0  },   // Menu 1891 Keep Init Settings - Network
	{ 1942,      COUNTRY_UN,    0  },   // Menu 1891 Keep Init Settings - Label Format 
#endif

#endif		//#ifdef USE_TRANSFER_MENU_SETTING
	{    0,	     	      0,	0   } // last condition
};

ROMDATA PARAMETER_SETTING_TYPE	ParameterSetting[]=
{	
	// <MEMO> JHC, 040520, Modify
	// [
	// countbit : if bit-setting then <bit count> else <input type>
	// 1:Byte(1byte), 2:Word(2byte), 3:String(n*1byte), 4or8:Bit(1bit), 5:Long(4byte)
	// ]
	// funcCode, captionNum, addrMemory, 			offsetMemory,  	sizeMemory, bitFlag, startBit, countBit,CountryCode   
	// Dealer mode data
	// <CODE> JHC, 040525, MOdify
	// [
	{  501,	     0x4501, 	 GLOBAL_SALE_SETUP_FLAG3,	FLASH_GLOBAL_AREA,	1,    	    4,	     1,	       1,	COUNTRY_EMPTY},  //  20
	{  502,	     0x4502, 	 FLASH_START_PASSWORD,		0,			10,    	    3,	     0,	       0,	COUNTRY_EMPTY},  //   1
	{  510,	     0x4510, 	 GLOBAL_CTS_MSG,		FLASH_GLOBAL_AREA, 22,    	    3,	     0,	       0,	COUNTRY_EMPTY},  //   1	
#ifdef USE_LOGISTICS_BARCODE	
	{  526, 	 0x4526,	 GLOBAL_SALE_SETUP_FLAG19,	FLASH_GLOBAL_AREA,	1,			4,		 1, 	   1,		COUNTRY_EMPTY},
	{  527, 	 0x4527,	 GLOBAL_SALE_SETUP_FLAG19,	FLASH_GLOBAL_AREA,	1,			4,		 3, 	   2,		COUNTRY_EMPTY}, 
#endif	
	{  528,      0x4528,     GLOBAL_PRICE_ZERO_FLAG,	FLASH_GLOBAL_AREA, 	1,          4,       7,	       1,       COUNTRY_EMPTY},
#ifdef USE_JULIAN_DATE
	{  529,      0x4529,     GLOBAL_SALE_SETUP_FLAG17,	FLASH_GLOBAL_AREA, 	1,          4,       2,	       2,       COUNTRY_EMPTY},
#endif
	{  530,	     0x4530, 	 GLOBAL_DATETIME_MODE,		FLASH_GLOBAL_AREA,	1,    	    4,	     5,	       2,	COUNTRY_EMPTY},  //   1
	{  531,	     0x3845, 	 GLOBAL_DATETIME_MODE,		FLASH_GLOBAL_AREA,	1,    	    4,	     3,	       4,	COUNTRY_EMPTY},  //   2
#ifndef USE_SELLBYDATE_CHANGE
	{  532,	     0x4532, 	 GLOBAL_SALEDATE_TYPE,		FLASH_GLOBAL_AREA,     	1,    	    1,	     0,	       0,	COUNTRY_EMPTY},  //   3
#endif
	//{  533,      0x4533, 	 GLOBAL_AREA_MULTIUNITFLAG,   	FLASH_GLOBAL_AREA,     	1,    	    1,	     0,	       1,	COUNTRY_EMPTY},  //   4
	{  533,	     0x4533, 	 GLOBAL_AREA_MULTIUNITFLAG,   	FLASH_GLOBAL_AREA,     	1,    	    4,	     0,	       1,	COUNTRY_EMPTY},  //   4
	// ]
	{  534,	     0x4534, 	 GLOBAL_DEFAULTUNIT,		FLASH_GLOBAL_AREA,     	2,	    2,	     0,	       0,	COUNTRY_EMPTY},  //   5
	{  535,	     0x4535, 	 GLOBAL_ERROR_TIMEOUT,		FLASH_GLOBAL_AREA,     	1,    	    1,	     0,	       0,	COUNTRY_EMPTY},  //   7
	{  536,	     0x4536, 	 GLOBAL_AREA_MULTIUNITFLAG,   	FLASH_GLOBAL_AREA,     	1,    	    4,	     1,	       1,	COUNTRY_EMPTY},  //   4
	{  537,	     0x4537, 	 GLOBAL_AREA_MULTIUNITFLAG,   	FLASH_GLOBAL_AREA,     	1,    	    4,	     2,	       1,	COUNTRY_EMPTY},  //   4
	{  538,	     0x4538, 	 GLOBAL_AREA_MULTIUNITFLAG,   	FLASH_GLOBAL_AREA,     	1,    	    4,	     3,	       1,	COUNTRY_EMPTY},  //   4
	{  539,	     0x4539, 	 GLOBAL_BARCODE_CUT	,     	FLASH_GLOBAL_AREA,   	1,    	    4,	     5,	       1,	COUNTRY_EMPTY},  //  10

	{  540,	     0x4540, 	 GLOBAL_BARCODE_PRICE_CUT,    	FLASH_GLOBAL_AREA,   	1,    	    1,	     0,	       0,	COUNTRY_EMPTY},  //   6
	{  541,	     0x4541, 	 GLOBAL_BARCODE_WEIGHT_CUT,   	FLASH_GLOBAL_AREA,   	1,    	    1,	     0,	       0,	COUNTRY_EMPTY},  //   7
	{  542,	     0x4542, 	 GLOBAL_BARCODE_CUT	,     	FLASH_GLOBAL_AREA,   	1,    	    4,	     0,	       1,	COUNTRY_EMPTY},  //  10
	{  543,	     0x4543, 	 GLOBAL_BARCODE_CUT	,     	FLASH_GLOBAL_AREA,   	1,    	    4,	     2,	       2,	COUNTRY_EMPTY},  //  add by son 070119

	//SG070419
	{  544,	     0x4544, 	 GLOBAL_WEIGHT_ROUND,	 	FLASH_GLOBAL_AREA,	1,    	    4,	     7,	       2,	COUNTRY_EMPTY},  // print weight cut number  
	
	{  545,	     0x4545, 	 GLOBAL_SALE_SETUP_FLAG6 ,     	FLASH_GLOBAL_AREA,   	1,    	    4,	     3,	       1,	COUNTRY_EMPTY},  //  
	{  546,	     0x4546, 	 GLOBAL_SALE_SETUP_FLAG6 ,     	FLASH_GLOBAL_AREA,   	1,    	    4,	     4,	       1,	COUNTRY_EMPTY},  //  For FSP
	{  547,	     0x4547, 	 GLOBAL_SALE_SETUP_FLAG6 ,     	FLASH_GLOBAL_AREA,   	1,    	    4,	     6,	       2,	COUNTRY_EMPTY},  //  SG061218
	{  548,	     0x4548, 	 GLOBAL_SALE_SETUP_FLAG9 ,     	FLASH_GLOBAL_AREA,   	1,    	    4,	     3,	       4,	COUNTRY_EMPTY},
	
	//SG070419
	{  549,	     0x4549, 	 GLOBAL_WEIGHT_ROUND,	 	FLASH_GLOBAL_AREA,	1,    	    4,	     5,	       2,	COUNTRY_EMPTY},  // display weight cut number   


	// Added by CJK 20040820
	{  550,	     0x9303, 	 FLASH_OFFSET_GapOutOfPaperThreshold,	FLASH_PRINTER_AREA,		1,	1,		0,		0,		COUNTRY_EMPTY},  //Out Of Paper Threshold
    	//yoo  080604	{  551,	     0x4551,     GLOBAL_PRINT_OPER_FLAG ,     	FLASH_GLOBAL_AREA, 	1,    	    4,	     0,	       1,	COUNTRY_EMPTY},  //  10
    	{  551,	     0x4551,	 GLOBAL_DUAL_PRINT_CNT, 	        FLASH_GLOBAL_AREA,	1,			1,		 0, 	       0,	COUNTRY_EMPTY},  // double print 발행 횟수를 0~9 사이로 선택  
    	{  552,	     0x4552,     GLOBAL_PRINT_OPER_FLAG2,     	FLASH_GLOBAL_AREA, 	1,    	    4,	     1,	       1,	COUNTRY_EMPTY},  //  10
	{  553,	     0x4553,	 GLOBAL_PRINT_OPER_FLAG2,     	FLASH_GLOBAL_AREA, 	1,    	    4,	     2,	       1,	COUNTRY_EMPTY},  //  10
	{  554,	     0x4554,	 GLOBAL_PRINT_OPER_FLAG2,     	FLASH_GLOBAL_AREA, 	1,    	    4,	     3,	       1,	COUNTRY_EMPTY},  //  
//DEL070126	{  554,	     0x4554,	 GLOBAL_PRINT_OPER_FLAG2,     	FLASH_GLOBAL_AREA, 	1,    	    4,	     3,	       1,	COUNTRY_EMPTY},  //  10
	{  555,      0x4555,	 GLOBAL_SALE_SETUP_FLAG5,     	FLASH_GLOBAL_AREA, 	1,    	    4,	     0,	       1,	COUNTRY_EMPTY},  //  10

	{  556,	     0x4556, 	 GLOBAL_SCANNER_FLAG1 ,     	FLASH_GLOBAL_AREA,   	1,    	    4,	     3,	       1,	COUNTRY_EMPTY},  //  SG070123
	{  557,	     0x4557, 	 GLOBAL_SCANNER_FLAG1 ,     	FLASH_GLOBAL_AREA,   	1,    	    4,	     2,	       3,	COUNTRY_EMPTY},  //  SG070123
//Added by JJANG 20071205
	{  558,	     0x4558, 	 GLOBAL_SCANNER_FLAG1 ,     	FLASH_GLOBAL_AREA,   	1,    	    4,	     4,	       1,	COUNTRY_EMPTY},  //  SG070123
	{  559,	     0x4559, 	 GLOBAL_BARCODE_CUT ,     	FLASH_GLOBAL_AREA,   	1,    	    4,	     4,	       2,	COUNTRY_EMPTY},  //  SG070123
	{  560,	     0x4560, 	 GLOBAL_SALE_SAVE_FLAG,     	FLASH_GLOBAL_AREA,	1,    	    4,	     7,	       1,	COUNTRY_EMPTY},
	{  561,	     0x4561, 	 GLOBAL_SCANNER_FLAG1 ,     	FLASH_GLOBAL_AREA,   	1,    	    4,	     7,	       3,	COUNTRY_EMPTY},
//DEL CJK070321	{  561,	     0x4561, 	 GLOBAL_CASHDRAWER_SENSOR,    	FLASH_GLOBAL_AREA, 	1,    	    1,	     0,	       1,	COUNTRY_EMPTY},  //   8
	{  562,	     0x4562, 	 GLOBAL_TIME_CASHDRAWEROPEN,  	FLASH_GLOBAL_AREA, 	2,    	    2,	     0,	       0,	COUNTRY_EMPTY},  //   9
	{  563,	     0x4563,	 GLOBAL_PRINT_OPER_FLAG2,     	FLASH_GLOBAL_AREA, 	1,    	    4,	     4,	       1,	COUNTRY_EMPTY},  //  10
//DEL CJK070321	{  564,	     0x4564,	 GLOBAL_PRINT_OPER_FLAG2,     	FLASH_GLOBAL_AREA, 	1,    	    4,	     5,	       1,	COUNTRY_EMPTY},  //  10
#ifdef USE_EMART_DEFAULT
	{  564, 	 0x4564,	 GLOBAL_SALE_SETUP_FLAG14,		FLASH_GLOBAL_AREA,	1,			4,		 2, 	   1,	COUNTRY_EMPTY},//se-hyung 20140226
#endif
#ifndef USE_ONLY_LABEL
	{  565,	     0x4565,     GLOBAL_SALE_SETUP_FLAG5,	FLASH_GLOBAL_AREA,      1,	    4,	     7,		1,	COUNTRY_EMPTY},	    //se-hyung 070627
#endif	
	{  566,	     0x4566, 	 GLOBAL_SALE_SETUP_FLAG1,     	FLASH_GLOBAL_AREA, 	1,    	    4,	     4,	       1,	COUNTRY_EMPTY},  //   3	//Check connection of master R-ip (P-NUM 566)
	{  567,	     0x4567, 	 GLOBAL_BARCODE_CUT,     	FLASH_GLOBAL_AREA, 	1,    	    4,	     7,	       2,	COUNTRY_EMPTY},  // 
	{  568,	     0x4568, 	 GLOBAL_SALE_SETUP_FLAG7,     	FLASH_GLOBAL_AREA, 	1,    	    4,	     5,	       2,	COUNTRY_EMPTY},  // 
	{  569,	     0x4569, 	 GLOBAL_PLU_TYPE_AREA,     	FLASH_GLOBAL_AREA, 	4,    	    3,	     0,	       0,	COUNTRY_EMPTY},  // 

	{  570,	     0x4570, 	 GLOBAL_SALE_SETUP_FLAG1,     	FLASH_GLOBAL_AREA, 	1,    	    4,	     2,	       1,	COUNTRY_EMPTY},  //  10 //Non weight sale(WT.on tray): (P-NUM 570)
	{  571,	     0x4571, 	 GLOBAL_SALE_SETUP_FLAG1,     	FLASH_GLOBAL_AREA, 	1,    	    4,	     3,	       1,	COUNTRY_EMPTY},  //   1 //Print Weight(by count sale): (P-NUM 571)
	{  572,	     0x4572, 	 GLOBAL_SALE_SETUP_FLAG3,     	FLASH_GLOBAL_AREA, 	1,    	    4,	     2,	       1,	COUNTRY_EMPTY},  //   1
	{  573,	     0x4573, 	 GLOBAL_SALE_SETUP_FLAG3,     	FLASH_GLOBAL_AREA, 	1,    	    4,	     3,	       1,	COUNTRY_EMPTY},  //   1	 //SG060224

	{  574,	     0x4574, 	 GLOBAL_SALE_SETUP_FLAG2,     	FLASH_GLOBAL_AREA, 	1,    	    4,	     5,	       1,	COUNTRY_EMPTY},  //   5

	{  575,	     0x4575, 	 GLOBAL_SALE_SETUP_FLAG3,     	FLASH_GLOBAL_AREA, 	1,    	    4,	     4,	       1,	COUNTRY_EMPTY},  //   4
	{  576,	     0x4576, 	 GLOBAL_SALE_SETUP_FLAG3,     	FLASH_GLOBAL_AREA, 	1,    	    4,	     5,	       1,	COUNTRY_EMPTY},  //   5
	{  577,	     0x4577, 	 GLOBAL_SALE_SETUP_FLAG3,     	FLASH_GLOBAL_AREA, 	1,    	    4,	     6,	       1,	COUNTRY_EMPTY},  //   6
	{  578,	     0x4578, 	 GLOBAL_SALE_SETUP_FLAG3,     	FLASH_GLOBAL_AREA, 	1,    	    4,	     7,	       1,	COUNTRY_EMPTY},  //   6

	{  579,	     0x4579, 	 GLOBAL_SALE_SAVE_FLAG ,      	FLASH_GLOBAL_AREA, 	1,    	    4,	     2,	       1,	COUNTRY_EMPTY},  //   6
// DEL CJK080709
//	{  580,	     0x4580, 	 GLOBAL_SALE_SETUP_FLAG1,     	FLASH_GLOBAL_AREA, 	1,    	    4,	     1,	       1,	COUNTRY_EMPTY},  //   3
	{  581,	     0x4581, 	 GLOBAL_SALE_SETUP_FLAG1,     	FLASH_GLOBAL_AREA, 	1,    	    4,	     6,	       1,	COUNTRY_EMPTY},  //   4 //Print only WT.(u.p.zero): (P-NUM 581)
	{  582,	     0x4582, 	 GLOBAL_SALE_SETUP_FLAG1,     	FLASH_GLOBAL_AREA, 	1,    	    4,	     7,	       1,	COUNTRY_EMPTY},  //   5 //Use X key (by WT.): (P-NUM 582)
	{  583,	     0x4583, 	 GLOBAL_SALE_SETUP_FLAG2,     	FLASH_GLOBAL_AREA, 	1,    	    4,	     0,	       1,	COUNTRY_EMPTY},  //   6
	{  584,	     0x4584, 	 GLOBAL_SALE_SETUP_FLAG2,     	FLASH_GLOBAL_AREA, 	1,    	    4,	     1,	       1,	COUNTRY_EMPTY},  //   6
	// 0x08
	{  585,	     0x4585, 	 GLOBAL_SALE_SETUP_FLAG2,     	FLASH_GLOBAL_AREA, 	1,    	    4,	     3,	       1,	COUNTRY_EMPTY},  //   6
	{  586,	     0x4586, 	 GLOBAL_KEY_TIMEOUT_KEYPRESS, 	FLASH_GLOBAL_AREA, 	1,    	    1,	     0,	       0,	COUNTRY_EMPTY},  //   6
	{  587,	     0x4587, 	 GLOBAL_PRICE_ZERO_FLAG,      	FLASH_GLOBAL_AREA, 	1,    	    4,	     0,	       1,	COUNTRY_EMPTY},  //   1
	{  588,	     0x4588, 	 GLOBAL_PRICE_ZERO_FLAG,      	FLASH_GLOBAL_AREA, 	1,    	    4,	     1,	       1,	COUNTRY_EMPTY},  //   1
	{  589,	     0x4589, 	 GLOBAL_PRICE_ROUND_APPLY,    	FLASH_GLOBAL_AREA, 	1,    	    1,	     0,	       0,	COUNTRY_EMPTY},  //   8
	{  590,	     0x4590, 	 GLOBAL_SALE_SETUP_FLAG4,     	FLASH_GLOBAL_AREA, 	1,    	    4,	     0,	       1,	COUNTRY_EMPTY},  //   7
	{  591,	     0x4591, 	 GLOBAL_AREA_PRICE_ROUND,     	FLASH_GLOBAL_AREA, 	2,    	    2,	     0,	       0,	COUNTRY_EMPTY},  //   8
//	{  592,	     0x4592, 	 GLOBAL_WEIGHT_ROUND,		FLASH_GLOBAL_AREA, 	4,    	    5,	     0,	       0,	COUNTRY_EMPTY},  //   9
	{  592,	     0x4592, 	 GLOBAL_WEIGHT_ROUND,	 	FLASH_GLOBAL_AREA,	1,    	    4,	     3,	       4,	COUNTRY_EMPTY},  //   9

	{  593,	     0x4593, 	 GLOBAL_SALE_SETUP_FLAG3,     	FLASH_GLOBAL_AREA, 	1,    	    4,	     0,	       1,	COUNTRY_EMPTY},  //  
//	{  594,	     0x258B, 	 GLOBAL_SALE_SETUP_FLAG3,     	FLASH_GLOBAL_AREA, 	1,    	    4,	     1,	       1,	COUNTRY_EMPTY},  //  20
	{  594,	     0x4594, 	 GLOBAL_REPORT_FLAG,		FLASH_GLOBAL_AREA, 	1,    	    4,	     5,	       1,	COUNTRY_EMPTY},  //   1

	{  595,	     0x4595, 	 GLOBAL_REPORT_FLAG,		FLASH_GLOBAL_AREA, 	1,    	    4,	     0,	       1,	COUNTRY_EMPTY},  //  20
	{  596,	     0x4596, 	 GLOBAL_REPORT_FLAG,		FLASH_GLOBAL_AREA, 	1,    	    4,	     1,	       1,	COUNTRY_EMPTY},  //   1
	{  597,	     0x4597, 	 GLOBAL_REPORT_FLAG,		FLASH_GLOBAL_AREA, 	1,    	    4,	     2,	       1,	COUNTRY_EMPTY},  //   1
	{  598,	     0x4598, 	 GLOBAL_REPORT_FLAG,		FLASH_GLOBAL_AREA, 	1,    	    4,	     3,	       1,	COUNTRY_EMPTY},  //   1
////DEL090910	{  599,	     0x4599, 	 GLOBAL_REPORT_FLAG,		FLASH_GLOBAL_AREA, 	1,    	    4,	     4,	       1,	COUNTRY_EMPTY},  //   1
	///////////////////////

	{  600,	     0x4600, 	 GLOBAL_AREA_TAREFLAG,		FLASH_GLOBAL_AREA, 	1,    	    4,	     0,	       1,	COUNTRY_EMPTY},  //   2
	{  601,	     0x4601, 	 GLOBAL_AREA_TAREFLAG,		FLASH_GLOBAL_AREA, 	1,    	    4,	     1,	       1,	COUNTRY_EMPTY},  //   3
	{  602,	     0x4602, 	 GLOBAL_AREA_TAREFLAG,		FLASH_GLOBAL_AREA, 	1,    	    4,	     2,	       1,	COUNTRY_EMPTY},  //   4
	{  603,	     0x4603, 	 GLOBAL_AREA_TAREFLAG,		FLASH_GLOBAL_AREA, 	1,    	    4,	     3,	       1,	COUNTRY_EMPTY},  //   5
	{  604,	     0x4604, 	 GLOBAL_AREA_TAREFLAG,		FLASH_GLOBAL_AREA, 	1,    	    4,	     4,	       1,	COUNTRY_EMPTY},  //   6
	{  605,	     0x9157, 	 FLASH_ADC_TARE_CONFIG,		0,			1,    	    4,	     7,	       1,	COUNTRY_EMPTY},  //   6
	{  606,	     0x9158, 	 FLASH_ADC_TARE_CONFIG,		0,		        1,    	    4,	     6,	       1,	COUNTRY_EMPTY},  //   6
	{  607,	     0x4607, 	 GLOBAL_TAREINPUT_FLAG,		FLASH_GLOBAL_AREA, 	1,    	    1,	     0,	       0,	COUNTRY_EMPTY},  //   7
	{  608,	     0x4608, 	 GLOBAL_AREA_TAREFLAG,		FLASH_GLOBAL_AREA, 	1,    	    4,	     5,	       1,	COUNTRY_EMPTY},  //   6
	{  609,	     0x4609, 	 GLOBAL_AREA_TAREFLAG,		FLASH_GLOBAL_AREA, 	1,    	    4,	     6,	       1,	COUNTRY_EMPTY},  //   6
	{  610,	     0x4610, 	 GLOBAL_REPORT_FLAG,		FLASH_GLOBAL_AREA, 	1,    	    4,	     7,	       1,	COUNTRY_EMPTY},  //   1
	{  611,	     0x4611, 	 GLOBAL_AREA_TAREFLAG,		FLASH_GLOBAL_AREA, 	1,    	    4,	     7,	       1,	COUNTRY_EMPTY},  //   6
	{  612,      0x4612,     GLOBAL_SALE_SETUP_FLAG4,     	FLASH_GLOBAL_AREA, 	1,          4,       1,	       1,       COUNTRY_EMPTY},  //
	//Added by JJANG 20080219
#ifndef USE_ONLY_LABEL		
	{  613,      0x4613,     GLOBAL_SALE_SETUP_FLAG8,     	FLASH_GLOBAL_AREA, 	1,          4,       3,	       1,       COUNTRY_EMPTY},  // Transaction msg size :
	{  614,      0x4614,     GLOBAL_SALE_SETUP_FLAG8,     	FLASH_GLOBAL_AREA, 	1,          4,       5,	       2,       COUNTRY_EMPTY},  // Clerk lock type :
#endif
	//SG070712
	{  615,      0x4615,     GLOBAL_SALE_SETUP_FLAG4,     	FLASH_GLOBAL_AREA, 	1,          4,       2,	       1,       COUNTRY_EMPTY},  //
	{  616,      0x4616,     GLOBAL_SALE_SETUP_HOLDTIME,   	FLASH_GLOBAL_AREA, 	1,          1,       0,	       0,       COUNTRY_EMPTY},  // Hold time
#ifndef CL5200J
#ifndef USE_ONLY_LABEL			
	{  617,      0x4617,     GLOBAL_SALE_SETUP_FLAG4,   	FLASH_GLOBAL_AREA, 	1,          4,       4,	       1,       COUNTRY_EMPTY},	 //Modify by se-hyung for Clerk Subtotal List 20070727
#endif	
#endif
	{  618,      0x4618,     GLOBAL_SALE_SETUP_FLAG4,   	FLASH_GLOBAL_AREA, 	1,          4,       5,	       1,       COUNTRY_EMPTY},	 //Modify by se-hyung 고정무게 입력시 캐파 무시, e값 무시 20070920
	//SG070920
	{  619,      0x4619,     GLOBAL_SALE_SETUP_FLAG4,     	FLASH_GLOBAL_AREA, 	1,          4,       7,	       1,       COUNTRY_EMPTY},  // RS232 RealTimeReport

	//{  620,      0x4620,     GLOBAL_SALE_SETUP_FLAG4,     	FLASH_GLOBAL_AREA, 	1,          4,       6,	       1,       COUNTRY_EMPTY},  // Use RealTime Transaction 
	{  620,      0x4620,     GLOBAL_SALE_SAVE_FLAG,     	FLASH_GLOBAL_AREA, 	1,          4,       6,	       4,       COUNTRY_EMPTY},  // Use RealTime Transaction 
	
	//Added by JJANG 20071112
	{  621,      0x4621,     GLOBAL_SALE_SETUP_FLAG7,     	FLASH_GLOBAL_AREA, 	1,          4,       0,	       1,       COUNTRY_EMPTY},  // Discount Type

	{  622,      0x4622,     GLOBAL_SALE_SETUP_FLAG7,     	FLASH_GLOBAL_AREA, 	1,          4,       7,	       2,       COUNTRY_EMPTY},  // 실시간 전송 시 CAPA에 따른 무게값 조정(0:x1,1:x10, 2:x100)
	{  623,      0x4623,     GLOBAL_SALE_SETUP_FLAG7,     	FLASH_GLOBAL_AREA, 	1,          4,       1,	       1,       COUNTRY_EMPTY},  // Use PS2 keyboard(117Key) for Self service Type scale

	//Added by JJANG 20080122
	//추 후 확장을 위해 2bit 잡아둠
//20080314 deleted by JJANG  임시삭제
//	{  624,      0x4624,     GLOBAL_SALE_SETUP_FLAG8,     	FLASH_GLOBAL_AREA, 	1,          4,       0,	       1,       COUNTRY_EMPTY},  // Net Error Message Type
#ifndef USE_RUSSIA_DEFAULT
	{  625,      0x4625,     GLOBAL_SALE_SETUP_FLAG8,     	FLASH_GLOBAL_AREA, 	1,          4,       2,	       2,       COUNTRY_EMPTY},  // Ext NVRAM Type
#endif
														     
	{  626,	     0x4626, 	 GLOBAL_DISPLAY_PRICE_PRIMARYSIGN,    	FLASH_GLOBAL_AREA, 	3,  3,	     0,	       0,	COUNTRY_EMPTY},  //   9
//20141218 deleted by LYH 사용하지 않음 
//	{  627,	     0x4627, 	 GLOBAL_DISPLAY_PRICE_LASTSIGN,		FLASH_GLOBAL_AREA, 	1,  3,	     0,	       0,	COUNTRY_EMPTY},  //  30

	{  628,	     0x4628, 	 GLOBAL_WEIGHT_LBSYMBOL_1,     	FLASH_GLOBAL_AREA, 	2,    	    3,	     0,	       0,	COUNTRY_EMPTY},  //   7
	{  629,	     0x4629, 	 GLOBAL_WEIGHT_LBSYMBOL_2,     	FLASH_GLOBAL_AREA, 	2,    	    3,	     0,	       0,	COUNTRY_EMPTY},  //   8
	{  630,	     0x4630, 	 GLOBAL_WEIGHT_SYMBOL_1,       	FLASH_GLOBAL_AREA, 	2,    	    3,	     0,	       0,	COUNTRY_EMPTY},  //   7
	{  631,	     0x4631, 	 GLOBAL_WEIGHT_SYMBOL_2,       	FLASH_GLOBAL_AREA, 	2,    	    3,	     0,	       0,	COUNTRY_EMPTY},  //   8
	{  632,	     0x4632, 	 GLOBAL_AREA_PRICE_PRIMARYSIGN,	FLASH_GLOBAL_AREA, 	4,    	    3,	     0,	       0,	COUNTRY_EMPTY},  //   9
	{  633,	     0x4633, 	 GLOBAL_AREA_PRICE_LASTSIGN,   	FLASH_GLOBAL_AREA,	4,    	    3,	     0,	       0,	COUNTRY_EMPTY},  //  30
	{  634,	     0x4634, 	 GLOBAL_TIME_SEPARATOR,		FLASH_GLOBAL_AREA, 	1,    	    3,	     0,	       0,	COUNTRY_EMPTY},  //   1
	{  635,	     0x4635, 	 GLOBAL_DATE_SEPARATOR,		FLASH_GLOBAL_AREA,	1,    	    3,	     0,	       0,	COUNTRY_EMPTY},  //   2
	//{  636,      0x2588,	 GLOBAL_AREA_PRICE_TYPE,       	FLASH_GLOBAL_AREA,	1,    	    1,	     0,	       0,	COUNTRY_EMPTY},  //   1
	{  636,      0x4636,	 GLOBAL_AREA_PRICE_TYPE,       	FLASH_GLOBAL_AREA,	1,    	    4,	     3,	       4,	COUNTRY_EMPTY},  //   1
	{  637,      0x4637,	 GLOBAL_AREA_PRICE_TYPE,       	FLASH_GLOBAL_AREA,	1,    	    4,	     7,	       4,	COUNTRY_EMPTY},  //   
	{  638,      0x4638,	 GLOBAL_DUAL_CURRENCY  ,       	FLASH_GLOBAL_AREA, 	1,    	    1,	     0,	       0,	COUNTRY_EMPTY},  //   1
	{  639,	     0x4639, 	 GLOBAL_REPORT_FLAG,		FLASH_GLOBAL_AREA,	1,    	    4,	     6,	       1,	COUNTRY_EMPTY},  //   1

	{  640,	     0x9241, 	 FLASH_LABEL_CAPTION_AREA+(LABEL_CAP_STRUCT_SIZE*0)+2,	0,   LABEL_CAP_STRUCT_SIZE-2,       3,	     0,	       0,	COUNTRY_EMPTY},  //   3
	{  641,	     0x9242, 	 FLASH_LABEL_CAPTION_AREA+(LABEL_CAP_STRUCT_SIZE*1)+2,	0,   LABEL_CAP_STRUCT_SIZE-2,  	    3,	     0,	       0,	COUNTRY_EMPTY},  //   4
	{  642,	     0x9243, 	 FLASH_LABEL_CAPTION_AREA+(LABEL_CAP_STRUCT_SIZE*2)+2,	0,   LABEL_CAP_STRUCT_SIZE-2,  	    3,	     0,	       0,	COUNTRY_EMPTY},  //   5
	{  643,	     0x9244, 	 FLASH_LABEL_CAPTION_AREA+(LABEL_CAP_STRUCT_SIZE*3)+2,	0,   LABEL_CAP_STRUCT_SIZE-2,       3,	     0,	       0,	COUNTRY_EMPTY},  //   6
	{  644,	     0x9245, 	 FLASH_LABEL_CAPTION_AREA+(LABEL_CAP_STRUCT_SIZE*4)+2,	0,   LABEL_CAP_STRUCT_SIZE-2,       3,	     0,	       0,	COUNTRY_EMPTY},  //   7
	{  645,	     0x9246, 	 FLASH_LABEL_CAPTION_AREA+(LABEL_CAP_STRUCT_SIZE*5)+2,	0,   LABEL_CAP_STRUCT_SIZE-2,       3,	     0,	       0,	COUNTRY_EMPTY},  //   8
	{  646,	     0x9247, 	 FLASH_LABEL_CAPTION_AREA+(LABEL_CAP_STRUCT_SIZE*6)+2,	0,   LABEL_CAP_STRUCT_SIZE-2,       3,	     0,	       0,	COUNTRY_EMPTY},  //   9
	{  647,	     0x9248, 	 FLASH_LABEL_CAPTION_AREA+(LABEL_CAP_STRUCT_SIZE*7)+2,	0,   LABEL_CAP_STRUCT_SIZE-2,       3,	     0,	       0,	COUNTRY_EMPTY},  //  40
	{  648,	     0x9249, 	 FLASH_LABEL_CAPTION_AREA+(LABEL_CAP_STRUCT_SIZE*8)+2,	0,   LABEL_CAP_STRUCT_SIZE-2,       3,	     0,	       0,	COUNTRY_EMPTY},  //   1
	{  649,	     0x924A, 	 FLASH_LABEL_CAPTION_AREA+(LABEL_CAP_STRUCT_SIZE*9)+2,	0,   LABEL_CAP_STRUCT_SIZE-2,       3,	     0,	       0,	COUNTRY_EMPTY},  //   2
	{  650,	     0x924B, 	 FLASH_LABEL_CAPTION_AREA+(LABEL_CAP_STRUCT_SIZE*10)+2,	0,   LABEL_CAP_STRUCT_SIZE-2,       3,	     0,	       0,	COUNTRY_EMPTY},  //   3
	{  651,	     0x924C, 	 FLASH_LABEL_CAPTION_AREA+(LABEL_CAP_STRUCT_SIZE*11)+2,	0,   LABEL_CAP_STRUCT_SIZE-2,       3,	     0,	       0,	COUNTRY_EMPTY},  //   4
	{  652,	     0x924D, 	 FLASH_LABEL_CAPTION_AREA+(LABEL_CAP_STRUCT_SIZE*12)+2,	0,   LABEL_CAP_STRUCT_SIZE-2,       3,	     0,	       0,	COUNTRY_EMPTY},  //   5
	{  653,	     0x924E, 	 FLASH_LABEL_CAPTION_AREA+(LABEL_CAP_STRUCT_SIZE*13)+2,	0,   LABEL_CAP_STRUCT_SIZE-2,       3,	     0,	       0,	COUNTRY_EMPTY},  //   6
	{  654,	     0x924F, 	 FLASH_LABEL_CAPTION_AREA+(LABEL_CAP_STRUCT_SIZE*14)+2,	0,   LABEL_CAP_STRUCT_SIZE-2,       3,	     0,	       0,	COUNTRY_EMPTY},  //   7
	{  655,	     0x9250, 	 FLASH_LABEL_CAPTION_AREA+(LABEL_CAP_STRUCT_SIZE*15)+2,	0,   LABEL_CAP_STRUCT_SIZE-2,       3,	     0,	       0,	COUNTRY_EMPTY},  //   8
	{  656,	     0x9251, 	 FLASH_LABEL_CAPTION_AREA+(LABEL_CAP_STRUCT_SIZE*16)+2,	0,   LABEL_CAP_STRUCT_SIZE-2,       3,	     0,	       0,	COUNTRY_EMPTY},  //   9
	{  657,	     0x9252, 	 FLASH_LABEL_CAPTION_AREA+(LABEL_CAP_STRUCT_SIZE*17)+2,	0,   LABEL_CAP_STRUCT_SIZE-2,       3,	     0,	       0,	COUNTRY_EMPTY},  //  50
	{  658,	     0x9253, 	 FLASH_LABEL_CAPTION_AREA+(LABEL_CAP_STRUCT_SIZE*18)+2,	0,   LABEL_CAP_STRUCT_SIZE-2,       3,	     0,	       0,	COUNTRY_EMPTY},  //   1
	{  659,	     0x9254, 	 FLASH_LABEL_CAPTION_AREA+(LABEL_CAP_STRUCT_SIZE*19)+2,	0,   LABEL_CAP_STRUCT_SIZE-2,       3,	     0,	       0,	COUNTRY_EMPTY},  //   2
	{  660,	     0x9255, 	 FLASH_LABEL_CAPTION_AREA+(LABEL_CAP_STRUCT_SIZE*20)+2,	0,   LABEL_CAP_STRUCT_SIZE-2,       3,	     0,	       0,	COUNTRY_EMPTY},  //   3
	{  661,	     0x9256, 	 FLASH_LABEL_CAPTION_AREA+(LABEL_CAP_STRUCT_SIZE*21)+2,	0,   LABEL_CAP_STRUCT_SIZE-2,       3,	     0,	       0,	COUNTRY_EMPTY},  //   4
	{  662,	     0x9257, 	 FLASH_LABEL_CAPTION_AREA+(LABEL_CAP_STRUCT_SIZE*22)+2,	0,   LABEL_CAP_STRUCT_SIZE-2,       3,	     0,	       0,	COUNTRY_EMPTY},   //   5
     	{  663,	     0x9258, 	 FLASH_LABEL_CAPTION_AREA+(LABEL_CAP_STRUCT_SIZE*23)+2,	0,   LABEL_CAP_STRUCT_SIZE-2,       3,	     0,	       0,	COUNTRY_EMPTY},   //   5
     	{  664,	     0x9259, 	 FLASH_LABEL_CAPTION_AREA+(LABEL_CAP_STRUCT_SIZE*24)+2,	0,   LABEL_CAP_STRUCT_SIZE-2,       3,	     0,	       0,	COUNTRY_EMPTY},   //   5
     	{  665,	     0x925A, 	 FLASH_LABEL_CAPTION_AREA+(LABEL_CAP_STRUCT_SIZE*25)+2,	0,   LABEL_CAP_STRUCT_SIZE-2,       3,	     0,	       0,	COUNTRY_EMPTY},   //   5
     	{  666,	     0x925B, 	 FLASH_LABEL_CAPTION_AREA+(LABEL_CAP_STRUCT_SIZE*26)+2,	0,   LABEL_CAP_STRUCT_SIZE-2,       3,	     0,	       0,	COUNTRY_EMPTY},   //   5
     	{  667,	     0x925C, 	 FLASH_LABEL_CAPTION_AREA+(LABEL_CAP_STRUCT_SIZE*27)+2,	0,   LABEL_CAP_STRUCT_SIZE-2,       3,	     0,	       0,	COUNTRY_EMPTY},   //   5
     	{  668,	     0x925D, 	 FLASH_LABEL_CAPTION_AREA+(LABEL_CAP_STRUCT_SIZE*28)+2,	0,   LABEL_CAP_STRUCT_SIZE-2,       3,	     0,	       0,	COUNTRY_EMPTY},   //   5
     	{  669,	     0x925E, 	 FLASH_LABEL_CAPTION_AREA+(LABEL_CAP_STRUCT_SIZE*29)+2,	0,   LABEL_CAP_STRUCT_SIZE-2,       3,	     0,	       0,	COUNTRY_EMPTY},   //   5
     	{  670,	     0x925F, 	 FLASH_LABEL_CAPTION_AREA+(LABEL_CAP_STRUCT_SIZE*30)+2,	0,   LABEL_CAP_STRUCT_SIZE-2,       3,	     0,	       0,	COUNTRY_EMPTY},   //   5
     	{  671,	     0x9260, 	 FLASH_LABEL_CAPTION_AREA+(LABEL_CAP_STRUCT_SIZE*31)+2,	0,   LABEL_CAP_STRUCT_SIZE-2,       3,	     0,	       0,	COUNTRY_EMPTY},   //   5
     	{  672,	     0x9261, 	 FLASH_LABEL_CAPTION_AREA+(LABEL_CAP_STRUCT_SIZE*32)+2,	0,   LABEL_CAP_STRUCT_SIZE-2,       3,	     0,	       0,	COUNTRY_EMPTY},   //   5
     	{  673,	     0x9262, 	 FLASH_LABEL_CAPTION_AREA+(LABEL_CAP_STRUCT_SIZE*33)+2,	0,   LABEL_CAP_STRUCT_SIZE-2,       3,	     0,	       0,	COUNTRY_EMPTY},   //   5
     	{  674,	     0x9263, 	 FLASH_LABEL_CAPTION_AREA+(LABEL_CAP_STRUCT_SIZE*34)+2,	0,   LABEL_CAP_STRUCT_SIZE-2,       3,	     0,	       0,	COUNTRY_EMPTY},   //   5
     	{  675,	     0x9264, 	 FLASH_LABEL_CAPTION_AREA+(LABEL_CAP_STRUCT_SIZE*35)+2,	0,   LABEL_CAP_STRUCT_SIZE-2,       3,	     0,	       0,	COUNTRY_EMPTY},   //   5
     	{  676,	     0x9265, 	 FLASH_LABEL_CAPTION_AREA+(LABEL_CAP_STRUCT_SIZE*36)+2,	0,   LABEL_CAP_STRUCT_SIZE-2,       3,	     0,	       0,	COUNTRY_EMPTY},   //   5
     	{  677,	     0x9266, 	 FLASH_LABEL_CAPTION_AREA+(LABEL_CAP_STRUCT_SIZE*37)+2,	0,   LABEL_CAP_STRUCT_SIZE-2,       3,	     0,	       0,	COUNTRY_EMPTY},   //   5
     	{  678,	     0x9267, 	 FLASH_LABEL_CAPTION_AREA+(LABEL_CAP_STRUCT_SIZE*38)+2,	0,   LABEL_CAP_STRUCT_SIZE-2,       3,	     0,	       0,	COUNTRY_EMPTY},   //   5

	{  679, 	 0x4679,	 GLOBAL_SALE_SETUP_FLAG17,	FLASH_GLOBAL_AREA,	1,			4,		 3, 	   1,		COUNTRY_EMPTY},  //   1
	{  680, 	 0x4678,	 GLOBAL_PRICESIGN_1000,	FLASH_GLOBAL_AREA,		1,			4,		 5, 	   1,		COUNTRY_EMPTY},  //   1
	{  685, 	 0x4685,	 GLOBAL_SALE_SETUP_FLAG19,	FLASH_GLOBAL_AREA,	1,			4,		 0, 	   1,	COUNTRY_EMPTY},
	{  686, 	 0x4686,	 GLOBAL_SALE_SETUP_FLAG20,	FLASH_GLOBAL_AREA,	1,			4,		 1, 	   1,	COUNTRY_EMPTY},
	{  687,	     0x4687, 	 GLOBAL_TARE_SETITING,		FLASH_GLOBAL_AREA, 	1,    	    4,	     4,	       1,	COUNTRY_EMPTY},		// Numeric Tare Block 설정
	{  688,	     0x4688, 	 GLOBAL_TARE_SETITING,		FLASH_GLOBAL_AREA, 	1,    	    4,	     5,	       1,	COUNTRY_EMPTY},		// Weight Tare Block 설정
	{  690, 	 0x4690,	 GLOBAL_SALE_SETUP_FLAG15, FLASH_GLOBAL_AREA,	1,			4,		 5, 	   1,		COUNTRY_EMPTY},  //   1
	{  691,  	 0x4691,	 GLOBAL_REPORT_FLAG2,    	FLASH_GLOBAL_AREA,  1,    	    4,	     2,	       3,	COUNTRY_EMPTY},  //   1
#ifdef USE_INDIA_TAX	 
	{  699,      0x38C0,	 GLOBAL_SALE_SETUP_FLAG2, 	FLASH_GLOBAL_AREA,  1,			4,		 4,		   1,	COUNTRY_EMPTY},
#endif		
// CL5500용
	{  700,      0x4640,	 GLOBAL_SALE_SETUP_FLAG12,	FLASH_GLOBAL_AREA, 	1,    	    4,	     3,	       4,	COUNTRY_EMPTY},  //   1
#ifdef USE_REPORT_TAX
	{  701,      0x4701,	 GLOBAL_REPORT_FLAG1  ,		FLASH_GLOBAL_AREA, 	1,    	    4,	     4,	       1,	COUNTRY_EMPTY},  //   1
#endif
	{  702,      0x4702,	 GLOBAL_REPORT_FLAG1  ,		FLASH_GLOBAL_AREA, 	1,    	    4,	     5,	       1,	COUNTRY_EMPTY},  //   1
	{  703,      0x4703,	 GLOBAL_REPORT_FLAG1  ,		FLASH_GLOBAL_AREA, 	1,    	    4,	     6,	       1,	COUNTRY_EMPTY},  //   1
	{  704,      0x4704,	 GLOBAL_REPORT_FLAG1  ,		FLASH_GLOBAL_AREA, 	1,    	    4,	     7,	       1,	COUNTRY_EMPTY},  //SG060106
	{  705,      0x4705,	 GLOBAL_SALE_SETUP_FLAG11,	FLASH_GLOBAL_AREA, 	1,    	    4,	     6,	       1,	COUNTRY_EMPTY},	// 	 1
//CJK070413	{  710,      0x4710,	 GLOBAL_REPORT_FLAG2  ,		FLASH_GLOBAL_AREA, 	1,    	    4,	     3,	       1,	COUNTRY_EMPTY},  //
	{  706,      0x4706,	 GLOBAL_SALE_SETUP_FLAG13,	FLASH_GLOBAL_AREA, 	1,    	    1,	     0,	       0,	COUNTRY_EMPTY},	// 	 1
	{  707,      0x4707,	 GLOBAL_SALE_SETUP_FLAG12,	FLASH_GLOBAL_AREA, 	1,    	    4,	     5,	       1,	COUNTRY_EMPTY},	// 	 1
	{  708,      0x4708,	 GLOBAL_SALE_SETUP_FLAG11,	FLASH_GLOBAL_AREA, 	1,    	    4,	     5,	       2,	COUNTRY_EMPTY},	// 	 1
#ifdef USE_CHECK_INDIV_INFO
	{  709,      0x4709,	 GLOBAL_PRINT_OPER_FLAG2,   	FLASH_GLOBAL_AREA, 	1,    	    4,	     7,	       1,	COUNTRY_EMPTY},
#endif
	{  710, 	 0x4710,	 GLOBAL_SALE_SETUP_FLAG14,		FLASH_GLOBAL_AREA,	1,			4,		 0, 	   1,	COUNTRY_EMPTY},
	{  711,      0x4711,	 GLOBAL_REPORT_FLAG2  ,		FLASH_GLOBAL_AREA, 	1,    	    4,	     4,	       1,	COUNTRY_EMPTY},  //   1
	{  712,      0x4712,	 GLOBAL_REPORT_FLAG2  ,		FLASH_GLOBAL_AREA, 	1,    	    4,	     5,	       1,	COUNTRY_EMPTY},  //   1
	{  713,      0x4713,	 GLOBAL_REPORT_FLAG2  ,		FLASH_GLOBAL_AREA, 	1,    	    4,	     6,	       1,	COUNTRY_EMPTY},  //   1
	{  714,      0x4714,	 GLOBAL_REPORT_FLAG2  ,		FLASH_GLOBAL_AREA, 	1,    	    4,	     7,	       1,	COUNTRY_EMPTY},  //   1

	{  715,      0x4715,	 GLOBAL_SALE_SETUP_FLAG9,	FLASH_GLOBAL_AREA, 	1,    	    4,	     6,	       3,	COUNTRY_EMPTY},  //   3
	{  716,	     0x4716, 	 GLOBAL_AREA_MULTIUNITFLAG,	FLASH_GLOBAL_AREA,	1,    	    4,	     7,	       1,	COUNTRY_EMPTY},  //   
#ifdef USE_TRACE_STANDALONE
	{  717,	     0x4717,	 GLOBAL_PRINT_OPER_FLAG2,     	FLASH_GLOBAL_AREA, 	1,    	    4,	     6,	       1,	COUNTRY_EMPTY},
#endif
#ifdef USE_EMART_BACK_LABEL
	{  718,	     0x147B,	 FLASH_BACK_LABEL_VALID_DATE,   0, 			1,    	    1,	     0,	       0,	COUNTRY_EMPTY},
#endif
#ifdef USE_DONT_SAVE_VALIDDATE_SALEDATE
	{  719, 	 0x4782,	 GLOBAL_SALE_SETUP_FLAG14,		FLASH_GLOBAL_AREA,	1,			4,		 1, 	   1,	COUNTRY_EMPTY},
#endif
	{  720,      0x4720,	 GLOBAL_REPORT_FLAG1  ,		FLASH_GLOBAL_AREA, 	1,    	    4,	     3,	       1,	COUNTRY_EMPTY},  //   1
	
#ifndef	USE_ONLY_LABEL
	{  721, 	 0x4721,	 GLOBAL_REPORT_FLAG1  , 	FLASH_GLOBAL_AREA,	1,			4,		 1, 	   2,	COUNTRY_EMPTY},  //CJK080512
	{  722, 	 0x4722,	 GLOBAL_REPORT_FLAG1  , 	FLASH_GLOBAL_AREA,	1,			4,		 2, 	   1,	COUNTRY_EMPTY},
#elif defined (USE_SALE_PRINT_LABLE_REPORT_PRINT_TICKET)
	{  721,      0x4721,	 GLOBAL_REPORT_FLAG1  ,		FLASH_GLOBAL_AREA, 	1,    	    4,	     1,	       2,	COUNTRY_EMPTY},  //CJK080512
	{  722,      0x4722,	 GLOBAL_REPORT_FLAG1  ,		FLASH_GLOBAL_AREA, 	1,    	    4,	     2,	       1,	COUNTRY_EMPTY},
#endif			
	{  723,      0x4723,     GLOBAL_SALE_SETUP_FLAG8,     	FLASH_GLOBAL_AREA, 	1,          4,       7,	       2,       COUNTRY_EMPTY},  // 

	{  724,      0x4724,     GLOBAL_SALE_SETUP_FLAG4,     	FLASH_GLOBAL_AREA, 	1,          4,       6,	       1,       COUNTRY_EMPTY},

	{  725,      0x4725,     GLOBAL_SALE_SETUP_FLAG9,     	FLASH_GLOBAL_AREA, 	1,          4,       7,	       1,       COUNTRY_EMPTY},
#ifdef USE_QR_ADDTOTAL
	{  726, 	 0x4726,	 GLOBAL_SALE_SETUP_FLAG14,		FLASH_GLOBAL_AREA,	1,			4,		 3, 	   1,		COUNTRY_EMPTY}, //20160216
#endif
#ifdef USE_SET_QUICK_PLU_DELAY_TIME
	{  727,	     0x4727, 	 GLOBAL_SALE_SETUP_FLAG19,		FLASH_GLOBAL_AREA,	1,    	    4,	     6,	       3,		COUNTRY_EMPTY},  //   1
#endif
#ifdef USE_EFT_POS_PROTOCOL
	{  728, 	 0x4728,	 GLOBAL_SALE_SETUP_FLAG21,	FLASH_GLOBAL_AREA,	1,			4,		 5, 	   2,	COUNTRY_EMPTY},	// 0:NOT_USE 1:ETHERNET 2:RS232 3:ETHERNET+RS232
#endif
    {  729,	     0x4729, 	 GLOBAL_SALE_SETUP_FLAG18,	FLASH_GLOBAL_AREA,	1,    	    4,	     7,	       1,	COUNTRY_EMPTY},  //   1
	{  730,	     0x9410, 	 GLOBAL_AREA_COMMUNICATION,	FLASH_GLOBAL_AREA,	1,    	    4,	     0,	       1,	COUNTRY_EMPTY},  //   1

	{  731,	     0x4731, 	 GLOBAL_AREA_COMMUNICATION,	FLASH_GLOBAL_AREA,	1,    	    4,	     1,	       1,	COUNTRY_EMPTY},  //   1

	{  732,	     0x4732,	 GLOBAL_SALE_SETUP_FLAG10,	FLASH_GLOBAL_AREA,	1,    	    4,	     7,	       1,	COUNTRY_EMPTY},
#ifndef USE_ONLY_LABEL        
	{  733,	     0x4733,	 GLOBAL_CHANGE_MODE1,	 FLASH_GLOBAL_AREA,	1,    	    1,	     0,	       0,	COUNTRY_EMPTY}, // label-ticket key, mode1
#endif	
	// reserve 734 : change mode 2
        //{  734,	     0x4734,	 GLOBAL_SALE_SETUP_FLAG10,	FLASH_GLOBAL_AREA,	1,    	    4,	     7,	       1,	COUNTRY_EMPTY},
	{  734,	     0x4734,	 GLOBAL_SALE_SETUP_FLAG20,	FLASH_GLOBAL_AREA,	1,    	    4,	     5,	       1,	COUNTRY_EMPTY},
#ifdef USE_CHECKING_HARMFUL_MEAT
	{  736, 	 0x4736,	 GLOBAL_SALE_SETUP_FLAG15,		FLASH_GLOBAL_AREA,	1,			4,		 2, 	   1,	COUNTRY_EMPTY}, // FOR Standalone Korean Trace
#endif

#ifdef USE_CHN_CART_SCALE
	{  735,	     0x4735,	 GLOBAL_PRICE_ZERO_FLAG,	 FLASH_GLOBAL_AREA,	1,    	    4,	     4,	       1,	COUNTRY_EMPTY}, // Trace code Type(meat/vegetable)
	{  736,	     0x4736,	 GLOBAL_PRICE_ZERO_FLAG,	 FLASH_GLOBAL_AREA,	1,    	    4,	     6,	       2,	COUNTRY_EMPTY}, // Use checking remain WT.
#endif
#ifdef USE_CL5200_DISP_CONTINUOUS_REFRESH
	{  737,	     0x4737, 	 GLOBAL_SALE_SETUP_FLAG15,		FLASH_GLOBAL_AREA,	1,    	    4,	     3,	       1,	COUNTRY_EMPTY},  // use dsip continous refresh
#endif
	{  738,	     0x4738, 	 GLOBAL_SALE_SETUP_FLAG15,		FLASH_GLOBAL_AREA,	1,    	    4,	     6,	       1,	COUNTRY_EMPTY},  // 0 //Separate Quantity caption from Weight caption
	{  739,	     0x4739, 	 GLOBAL_SALE_SETUP_FLAG18,		FLASH_GLOBAL_AREA,	1,    	    4,	     1,	       2,	COUNTRY_EMPTY},  // 0 // Weight,Quantity Unit symbol Print setting (0~2)
	{  740,	     0x4740, 	 GLOBAL_SALE_SETUP_FLAG18,		FLASH_GLOBAL_AREA,	1,    	    4,	     4,	       3,	COUNTRY_EMPTY},  // 0 // Unit price Unit symbol Print setting (0~5)
	{  741,	     0x4741, 	 GLOBAL_PRICESIGN_1000,		FLASH_GLOBAL_AREA,	1,    	    4,	     0,	       1,	COUNTRY_EMPTY},  //   1
    {  742,	     0x4742, 	 GLOBAL_SALE_SETUP_FLAG18,		FLASH_GLOBAL_AREA,	1,    	    4,	     6,	       1,	COUNTRY_EMPTY},  // 0  //Realtime Add Data Send flag
	{  743,	     0x4743, 	 GLOBAL_SALE_SETUP_FLAG19,		FLASH_GLOBAL_AREA,	1,    	    4,	     1,	       1,	COUNTRY_EMPTY},  // 0  //Replace Fixed weight with Packing Price
	{  746, 	 0x4746,	 GLOBAL_SALE_SETUP_FLAG17,	FLASH_GLOBAL_AREA,	1,			4,		 0, 	   1,	COUNTRY_EMPTY}, // Use Auto Zero Setting
	
	{  750,	     0x9306, 	 FLASH_OFFSET_GapOutOfPaperThresholdOnLabel,	FLASH_PRINTER_AREA,		1,	1,		0,		0,		COUNTRY_EMPTY},  //Out Of Paper Threshold On Label
	{  751,	     0x392a, 	 GLOBAL_PRICESIGN_1000,		FLASH_GLOBAL_AREA,	1,    	    4,	     4,	       1,	COUNTRY_EMPTY},  //   1
#ifdef USE_EMART_DEFAULT
	{  752, 	 0x4753,	 GLOBAL_SALE_SETUP_FLAG20,	FLASH_GLOBAL_AREA,	1,			4,		 2, 	   1,	COUNTRY_EMPTY},  //   1
#endif
	{  764, 	 0x4764,	 GLOBAL_SALE_SETUP_FLAG20,		FLASH_GLOBAL_AREA,	1,			8,		 0, 	   1,	COUNTRY_EMPTY},  //   0
	{  765, 	 0x4765,	 GLOBAL_REALTIME_CHECK_INTERVAL,		FLASH_GLOBAL_AREA,	2,			2,		 0, 	   0,	COUNTRY_EMPTY},
#ifdef USE_DISPLAY_COMM_SPEED_SETTING
	{  770,	     0x4770, 	 GLOBAL_START_FLAG,				FLASH_GLOBAL_AREA,		2,    	    4,	     13,	   2,	COUNTRY_EMPTY},  //   1
#endif
#ifndef USE_ONLY_LABEL  //KKH for CL3500
	{  771,      0x4771,	 GLOBAL_ACTION_ENABLE	  ,    	FLASH_GLOBAL_AREA,    	2,    	    4,	     0,	       1,	COUNTRY_EMPTY},  //   1	
#endif        
	{  772,      0x4772,	 GLOBAL_ACTION_ENABLE	  ,    	FLASH_GLOBAL_AREA,    	2,    	    4,	     1,	       1,	COUNTRY_EMPTY},  //   1	
#if defined (USE_ONLY_LABEL) || defined (USE_RUSSIA_DEFAULT)
#else
	{  773,      0x4773,	 GLOBAL_ACTION_ENABLE	  ,    	FLASH_GLOBAL_AREA,    	2,    	    4,	     2,	       1,	COUNTRY_EMPTY},  //   1	
#endif	
	{  774,      0x4774,	 GLOBAL_ACTION_ENABLE	  ,    	FLASH_GLOBAL_AREA,    	2,    	    4,	     3,	       1,	COUNTRY_EMPTY},  //   1	
	{  775,      0x4775,	 GLOBAL_ACTION_ENABLE	  ,    	FLASH_GLOBAL_AREA,    	2,    	    4,	     4,	       1,	COUNTRY_EMPTY},  //   1	
	{  776,      0x4776,	 GLOBAL_ACTION_ENABLE	  ,    	FLASH_GLOBAL_AREA,    	2,    	    4,	     5,	       1,	COUNTRY_EMPTY},  //   1	
#ifdef USE_TRACEABILITY
	{  777,      0x4777,	 GLOBAL_ACTION_ENABLE	  ,    	FLASH_GLOBAL_AREA,   	2,    	    4,	     6,	       1,	COUNTRY_EMPTY},  //   1	
#endif
#ifndef USE_ONLY_LABEL		
	{  778,      0x4778,	 GLOBAL_ACTION_ENABLE	  ,    	FLASH_GLOBAL_AREA,    	2,    	    4,	     7,	       1,	COUNTRY_EMPTY},  //   1	
#endif	
	{  779,      0x4779,	 GLOBAL_ACTION_ENABLE	  ,    	FLASH_GLOBAL_AREA,   	2,    	    4,	     8,	       1,	COUNTRY_EMPTY},  //   1	
	{  780,      0x4780,	 GLOBAL_ACTION_ENABLE	  ,    	FLASH_GLOBAL_AREA,    	2,    	    4,	     9,	       1,	COUNTRY_EMPTY},  //   1	
	{  781,      0x4781,	 GLOBAL_ACTION_ENABLE	  ,    	FLASH_GLOBAL_AREA,    	2,    	    4,	    10,	       1,	COUNTRY_EMPTY},  //   1	
#ifndef USE_RUSSIA_DEFAULT
	{  782,      0x4782,	 GLOBAL_ACTION_ENABLE	  ,    	FLASH_GLOBAL_AREA,    	2,    	    4,	    11,	       1,	COUNTRY_EMPTY},  //   1 국내축산 항목 사용. 
#endif
	{  783, 	 0x4783,	 GLOBAL_SALE_SETUP_FLAG20,	FLASH_GLOBAL_AREA,	1,			4,		 3, 	   1,	COUNTRY_EMPTY},  //   1
	{  784, 	 0x4784,	 GLOBAL_SALE_SETUP_FLAG20,	FLASH_GLOBAL_AREA,	1,			4,		 4, 	   1,	COUNTRY_EMPTY},  //   1    
#ifdef USE_CHECK_DUPLICATION_TICKET
	{  793,      0x4793,     GLOBAL_SALE_SETUP_FLAG21 , FLASH_GLOBAL_AREA,	1,    	    4,	     7,	       1,	COUNTRY_EMPTY},	 //   Use Continuous Duplication Ticket
#endif
	{  794,	     0x4794, 	 FLASH_FWMODE_SET    ,		0,	    		1,    	    4,	     2,	       1,	COUNTRY_EMPTY},  // FW USB update. bit2만 변경
	{  795,      0x4795,     GLOBAL_SALE_SETUP_FLAG21 , FLASH_GLOBAL_AREA,	1,    	    4,	     6,	       1,	COUNTRY_EMPTY},	 // Use Key Tare With Weight
	{  796,	     0x4796, 	 GLOBAL_TARE_SETITING	  ,	FLASH_GLOBAL_AREA,	1,    	    4,	     3,	       1,	COUNTRY_EMPTY},  //   tare 사용시 영점 자동 복귀  se-hyung 20080325
	{  797,	     0x4797, 	 FLASH_ADC_USE_FULLTARE	  ,	0,			1,    	    4,	     0,	       1,	COUNTRY_EMPTY},  //   a/d fulltare se-hyung 20080325
	{  798,	     0x4798, 	 GLOBAL_TARE_SETITING	  ,	FLASH_GLOBAL_AREA,	1,    	    4,	     2,	       3,	COUNTRY_EMPTY},  //   tare clear condition 20080325

	{  799,	     0x4799, 	 FLASH_ADC_FILTER_LEVEL	  ,	0,			1,    	    1,	     0,	       0,	COUNTRY_EMPTY},  //   a/d filter set

	// Factory mode data
//DEL CJK080130
//	{  801,	     0x4801, 	 GLOBAL_DISPLAY_WEIGHTPOS,     	FLASH_GLOBAL_AREA, 	1,    	    1,	     0,	       0,	COUNTRY_EMPTY},  //   1
//	{  802,	     0x4802, 	 GLOBAL_DISPLAY_WEIGHTLENGTH,  	FLASH_GLOBAL_AREA,	1,    	    1,	     0,	       0,	COUNTRY_EMPTY},  //   2
//	{  803,	     0x4803, 	 GLOBAL_DISPLAY_UNITPRICEPOS,  	FLASH_GLOBAL_AREA, 	1,    	    1,	     0,	       0,	COUNTRY_EMPTY},  //   1
//	{  804,	     0x4804, 	 GLOBAL_DISPLAY_UNITPRICELENGTH,FLASH_GLOBAL_AREA,	1,    	    1,	     0,	       0,	COUNTRY_EMPTY},  //   1
//	{  805,	     0x4805, 	 GLOBAL_DISPLAY_PRICEPOS,	FLASH_GLOBAL_AREA, 	1,    	    1,	     0,	       0,	COUNTRY_EMPTY},  //   1
//	{  806,	     0x4806, 	 GLOBAL_DISPLAY_PRICELENGTH,	FLASH_GLOBAL_AREA, 	1,    	    1,	     0,	       0,	COUNTRY_EMPTY},  //   1
//	{  807,	     0x4807, 	 GLOBAL_DISPLAY_TAREPOS,	FLASH_GLOBAL_AREA, 	1,    	    1,	     0,	       0,	COUNTRY_EMPTY},  //   1
//	{  808,	     0x4808, 	 GLOBAL_DISPLAY_TARELENGTH,	FLASH_GLOBAL_AREA, 	1,    	    1,	     0,	       0,	COUNTRY_EMPTY},  //   1

//	{  809,	     0x4809, 	 GLOBAL_DISPLAY_WEIGHTDECIMAL,	FLASH_GLOBAL_AREA, 	1,    	    1,	     0,	       0,	COUNTRY_EMPTY},  //   1
	{  810,	     0x4810, 	 GLOBAL_AREA_PRICE_DECIMALPOINT,FLASH_GLOBAL_AREA,	1,    	    1,	     0,	       0,	COUNTRY_EMPTY},  //   1
//Deleted by JJANG 20071205
//	{  811,	     0x4811, 	 GLOBAL_DISPLAY_INDICATORSIGN+0,FLASH_GLOBAL_AREA,	1,    	    1,	     0,	       0,	COUNTRY_EMPTY},  //   1
//	{  812,	     0x4812, 	 GLOBAL_DISPLAY_INDICATORSIGN+1,FLASH_GLOBAL_AREA,	1,    	    1,	     0,	       0,	COUNTRY_EMPTY},  //   1
//	{  813,	     0x4813, 	 GLOBAL_DISPLAY_INDICATORSIGN+2,FLASH_GLOBAL_AREA,	1,    	    1,	     0,	       0,	COUNTRY_EMPTY},  //   1
//	{  814,	     0x4814, 	 GLOBAL_DISPLAY_INDICATORSIGN+3,FLASH_GLOBAL_AREA,	1,    	    1,	     0,	       0,	COUNTRY_EMPTY},  //   1
//	{  815,	     0x4815, 	 GLOBAL_DISPLAY_INDICATORSIGN+4,FLASH_GLOBAL_AREA,	1,    	    1,	     0,	       0,	COUNTRY_EMPTY},  //   1
//	{  816,	     0x4816, 	 GLOBAL_DISPLAY_INDICATORSIGN+5,FLASH_GLOBAL_AREA,	1,    	    1,	     0,	       0,	COUNTRY_EMPTY},  //   1
//	{  817,	     0x4817, 	 GLOBAL_DISPLAY_INDICATORSIGN+6,FLASH_GLOBAL_AREA,	1,    	    1,	     0,	       0,	COUNTRY_EMPTY},  //   1
//	{  818,	     0x4818, 	 GLOBAL_DISPLAY_INDICATORSIGN+7,FLASH_GLOBAL_AREA,	1,    	    1,	     0,	       0,	COUNTRY_EMPTY},  //   1
//	{  819,	     0x4819, 	 GLOBAL_DISPLAY_INDICATORSIGN+8,FLASH_GLOBAL_AREA,	1,    	    1,	     0,	       0,	COUNTRY_EMPTY},  //   1

//DEL CJK080130
//	{  821,	     0x4821, 	 GLOBAL_INPUT_BC_QTYPOS,	FLASH_GLOBAL_AREA,	1,    	    1,	     0,	       0,	COUNTRY_EMPTY},  //   1
//	{  822,	     0x4822, 	 GLOBAL_INPUT_BC_QTYLENGTH,	FLASH_GLOBAL_AREA, 	1,    	    1,	     0,	       0,	COUNTRY_EMPTY},  //   1
//	{  823,	     0x4823, 	 GLOBAL_INPUT_BC_PCSPOS,	FLASH_GLOBAL_AREA, 	1,    	    1,	     0,	       0,	COUNTRY_EMPTY},  //   1
//	{  824,	     0x4824, 	 GLOBAL_INPUT_BC_PCSLENGTH,	FLASH_GLOBAL_AREA, 	1,    	    1,	     0,	       0,	COUNTRY_EMPTY},  //   1
//	{  825,	     0x4825, 	 GLOBAL_INPUT_BC_PRICEPOS,	FLASH_GLOBAL_AREA, 	1,    	    1,	     0,	       0,	COUNTRY_EMPTY},  //   1
//	{  826,	     0x4826, 	 GLOBAL_INPUT_BC_PRICELENGTH, 	FLASH_GLOBAL_AREA,	1,    	    1,	     0,	       0,	COUNTRY_EMPTY},  //   1
//	{  827,	     0x4827, 	 GLOBAL_INPUT_BN_QTYPOS,	FLASH_GLOBAL_AREA, 	1,    	    1,	     0,	       0,	COUNTRY_EMPTY},  //   1
//	{  828,	     0x4828, 	 GLOBAL_INPUT_BN_QTYLENGTH,	FLASH_GLOBAL_AREA, 	1,    	    1,	     0,	       0,	COUNTRY_EMPTY},  //   1
//	{  829,	     0x4829, 	 GLOBAL_INPUT_BN_PRICEPOS,	FLASH_GLOBAL_AREA, 	1,    	    1,	     0,	       0,	COUNTRY_EMPTY},  //   1
//	{  830,	     0x4830, 	 GLOBAL_INPUT_BN_PRICELENGTH, 	FLASH_GLOBAL_AREA,	1,    	    1,	     0,	       0,	COUNTRY_EMPTY},  //   1

	// <CODE> JHC, 040427, Modify
	// [
	{  831,	     0x4831, 	 GLOBAL_START_FLAG,		FLASH_GLOBAL_AREA,	2,    	    4,	     2,	       3,	COUNTRY_EMPTY},  //   1	
	{  832,	     0x4832, 	 GLOBAL_START_FLAG,		FLASH_GLOBAL_AREA,	2,    	    4,	     4,	       1,	COUNTRY_EMPTY},  //   1
	// ]
	{  833,	     0x4833, 	 GLOBAL_START_FLAG,		FLASH_GLOBAL_AREA,	2,    	    4,	     5,	       1,	COUNTRY_EMPTY},  //   1
	{  834,	     0x4834, 	 GLOBAL_START_FLAG,		FLASH_GLOBAL_AREA,	2,    	    4,	     7,	       1,	COUNTRY_EMPTY},  //   1
	// <CODE> JHC, 040421, -Not use
	// [
	// $COMMENT: 
	// {  855,	     0x9A0A, 	 GLOBAL_AREA_VENDOR_COUNTRY, FLASH_GLOBAL_AREA,	2,    	    2,	     0,	       0,	COUNTRY_EMPTY},  //   1
	// ]
//DEL CJK070321	{  856,	     0x4856, 	 GLOBAL_AREA_VENDOR_VENDOR,	FLASH_GLOBAL_AREA, 	2,    	    2,	     0,	       0,	COUNTRY_EMPTY},  //   1
//	{  860,	     0x20F1, 	 GLOBAL_SALE_AUTO_COUNT,	FLASH_GLOBAL_AREA, 	1,    	    1,	     0,	       0,	COUNTRY_EMPTY},  //   1
	{  861,	     0x4861, 	 GLOBAL_SALE_AUTO_DELTAWEIGHT,	FLASH_GLOBAL_AREA, 	4,    	    1,	     0,	       0,	COUNTRY_EMPTY},  //   1
	{  862,	     0x4862, 	 GLOBAL_SALE_MINIMUMWEIGHT,	FLASH_GLOBAL_AREA, 	4,    	    1,	     0,	       0,	COUNTRY_EMPTY},  //   1
	{  863,	     0x4863, 	 GLOBAL_SALE_SETUP_FLAG5,	FLASH_GLOBAL_AREA, 	1,    	    4,	     6,	       1,	COUNTRY_EMPTY},  //   1
// unification price and weight decimal : Deleted by CJK 20060418

	{  865,	     0x4865, 	 GLOBAL_AREA_MULTIUNITFLAG,	FLASH_GLOBAL_AREA,	1,    	    4,	     6,	       3,	COUNTRY_EMPTY},  //   
	{  866,	     0x4866, 	 GLOBAL_DECIMAL_SIGN, 		FLASH_GLOBAL_AREA,	1,    	    3,	     0,	       0,	COUNTRY_EMPTY},  //   1
//	{  867,	     0x4867, 	 GLOBAL_DISPLAY_PRICESIGN,	FLASH_GLOBAL_AREA,	1,    	    3,	     0,	       0,	COUNTRY_EMPTY},  //   1
	// <CODE> JHC, 040430, Comment, -Change 881, 882 To 587, 588 
	// [
	// {  881,	     0x2586, 	 GLOBAL_PRICE_ZERO_FLAG,FLASH_GLOBAL_AREA, 	1,    	    4,	     0,	       1,	COUNTRY_EMPTY},  //   1
	// {  882,	     0x2587, 	 GLOBAL_PRICE_ZERO_FLAG,FLASH_GLOBAL_AREA, 	1,    	    4,	     1,	       1,	COUNTRY_EMPTY},  //   1
	// ]
	{  885,	     0x4885, 	 GLOBAL_KEY_NUMBERZERO_N,	FLASH_GLOBAL_AREA, 	1,    	    1,	     0,	       0,	COUNTRY_EMPTY},  //   1
	{  886,	     0x4886, 	 GLOBAL_KEY_TIMEOUT_ERROR,	FLASH_GLOBAL_AREA, 	1,    	    1,	     0,	       0,	COUNTRY_EMPTY},  //   1
	{  887,	     0x4887, 	 GLOBAL_KEY_TIMEOUT_ACTION,	FLASH_GLOBAL_AREA, 	2,    	    2,	     0,	       0,	COUNTRY_EMPTY},  //   1
	{  888,	     0x4888, 	 GLOBAL_TIME_KEYCLEAR, 		FLASH_GLOBAL_AREA,	1,    	    1,	     0,	       0,	COUNTRY_EMPTY},  //   1
	{  900,	     0x9410, 	 GLOBAL_AREA_COMMUNICATION,	FLASH_GLOBAL_AREA,	1,    	    4,	     0,	       1,	COUNTRY_EMPTY},  //   1
//DEL070125	{  901,	     0x9411, 	 GLOBAL_AREA_COMMUNICATION,	FLASH_GLOBAL_AREA, 	1,    	    4,	     1,	       1,	COUNTRY_EMPTY},  //   1
//DEL070125	{  902,	     0x9412, 	 GLOBAL_AREA_COMMUNICATION,	FLASH_GLOBAL_AREA,  	1,    	    4,	     2,	       1,	COUNTRY_EMPTY},  //   1
//DEL070125	{  903,	     0x9413, 	 GLOBAL_AREA_COMMUNICATION,	FLASH_GLOBAL_AREA,  	1,    	    4,	     3,	       1,	COUNTRY_EMPTY},  //   1

	{  920,	     0x4920, 	 GLOBAL_DATE_AMPMARRAY+0,	FLASH_GLOBAL_AREA,	5,    	    3,	     0,	       0,	COUNTRY_EMPTY},  //   1
	{  921,	     0x4921, 	 GLOBAL_DATE_AMPMARRAY+5,	FLASH_GLOBAL_AREA,	5,    	    3,	     0,	       0,	COUNTRY_EMPTY},  //   1
	{  922,	     0x4922, 	 GLOBAL_DATE_MONTHARRAY+(0*10),	FLASH_GLOBAL_AREA,	10,    	    3,	     0,	       0,	COUNTRY_EMPTY},  //   1
	{  923,	     0x4922, 	 GLOBAL_DATE_MONTHARRAY+(1*10),	FLASH_GLOBAL_AREA,	10,    	    3,	     0,	       0,	COUNTRY_EMPTY},  //   1
	{  924,	     0x4922, 	 GLOBAL_DATE_MONTHARRAY+(2*10),	FLASH_GLOBAL_AREA,	10,    	    3,	     0,	       0,	COUNTRY_EMPTY},  //   1
	{  925,	     0x4922, 	 GLOBAL_DATE_MONTHARRAY+(3*10),	FLASH_GLOBAL_AREA,	10,    	    3,	     0,	       0,	COUNTRY_EMPTY},  //   1
	{  926,	     0x4922, 	 GLOBAL_DATE_MONTHARRAY+(4*10),	FLASH_GLOBAL_AREA,	10,    	    3,	     0,	       0,	COUNTRY_EMPTY},  //   1
	{  927,	     0x4922, 	 GLOBAL_DATE_MONTHARRAY+(5*10),	FLASH_GLOBAL_AREA,	10,    	    3,	     0,	       0,	COUNTRY_EMPTY},  //   1
	{  928,	     0x4922, 	 GLOBAL_DATE_MONTHARRAY+(6*10),	FLASH_GLOBAL_AREA,	10,    	    3,	     0,	       0,	COUNTRY_EMPTY},  //   1
	{  929,	     0x4922, 	 GLOBAL_DATE_MONTHARRAY+(7*10),	FLASH_GLOBAL_AREA,	10,    	    3,	     0,	       0,	COUNTRY_EMPTY},  //   1
	{  930,	     0x4922, 	 GLOBAL_DATE_MONTHARRAY+(8*10),	FLASH_GLOBAL_AREA,	10,    	    3,	     0,	       0,	COUNTRY_EMPTY},  //   1
	{  931,	     0x4922, 	 GLOBAL_DATE_MONTHARRAY+(9*10),	FLASH_GLOBAL_AREA,	10,    	    3,	     0,	       0,	COUNTRY_EMPTY},  //   1
	{  932,	     0x4922, 	 GLOBAL_DATE_MONTHARRAY+(10*10),FLASH_GLOBAL_AREA,	10,    	    3,	     0,	       0,	COUNTRY_EMPTY},  //   1
	{  933,	     0x4922, 	 GLOBAL_DATE_MONTHARRAY+(11*10),FLASH_GLOBAL_AREA,	10,    	    3,	     0,	       0,	COUNTRY_EMPTY},  //   1
	{  934,	     0x4934, 	 GLOBAL_DATE_WEEKARRAY+(0*10),	FLASH_GLOBAL_AREA,	10,    	    3,	     0,	       0,	COUNTRY_EMPTY},  //   1
	{  935,	     0x4934, 	 GLOBAL_DATE_WEEKARRAY+(1*10),	FLASH_GLOBAL_AREA,	10,    	    3,	     0,	       0,	COUNTRY_EMPTY},  //   1
	{  936,	     0x4934, 	 GLOBAL_DATE_WEEKARRAY+(2*10),	FLASH_GLOBAL_AREA,	10,    	    3,	     0,	       0,	COUNTRY_EMPTY},  //   1
	{  937,	     0x4934, 	 GLOBAL_DATE_WEEKARRAY+(3*10),	FLASH_GLOBAL_AREA,	10,    	    3,	     0,	       0,	COUNTRY_EMPTY},  //   1
	{  938,	     0x4934, 	 GLOBAL_DATE_WEEKARRAY+(4*10),	FLASH_GLOBAL_AREA,	10,    	    3,	     0,	       0,	COUNTRY_EMPTY},  //   1
	{  939,	     0x4934, 	 GLOBAL_DATE_WEEKARRAY+(5*10),	FLASH_GLOBAL_AREA,	10,    	    3,	     0,	       0,	COUNTRY_EMPTY},  //   1
	{  940,	     0x4934, 	 GLOBAL_DATE_WEEKARRAY+(6*10),	FLASH_GLOBAL_AREA,	10,    	    3,	     0,	       0,	COUNTRY_EMPTY},  //   1

	//SG070712
	{  980,      0x4980,     GLOBAL_SALE_SETUP_FLAG4,     	FLASH_GLOBAL_AREA, 	1,          4,       3,	       1,       COUNTRY_EMPTY},  //

	// param994 : FW Update over Ethernet
	//  - Boot Ver 1.05 이상일 때 동작
	//  - param996과 분리하여 신규 생성. 996을 사용시, 신규BOOT에 구 펌웨어일 경우 이상동작할 수 있음
	//  - 기존버전은 994가 없으므로, FW Update가 불가 함
#ifndef USE_RUSSIA_DEFAULT
	{  994,	     0x4996, 	 FLASH_FWMODE_SET    ,		0,	    		1,    	    1,	     0,	       0,	COUNTRY_EMPTY},  
#endif
#if defined(USE_USA_INGREDIENT) || defined(USE_NORTH_AMERICA_INGREDIENT)
#else
	{  995,	     0x4995, 	 FLASH_NUMBER_INGRIDIENT_SIZE,	0, 			2,    	    2,	     0,	       0,	COUNTRY_EMPTY},  //   1
#endif
	{  996,	     0x4996, 	 FLASH_FWMODE_SET    ,		0,	    		1,    	    1,	     0,	       0,	COUNTRY_EMPTY},  //   1
	{  997,	     0x4997, 	 0,				0,	    		0,    	    0,	     0,	       0,	COUNTRY_EMPTY},  //   1
#ifndef USE_CHINA_DEFAULT
	{  998,	     0x4998, 	 0,				0,	    		0,    	    0,	     0,	       0,	COUNTRY_EMPTY},  //   
#endif
#ifdef USE_CHINA_DEFAULT
	{  999,	     0x4998, 	 0,				0,	    		0,    	    0,	     0,	       0,	COUNTRY_EMPTY},  //
#else
	{  999,	     0x4999, 	 GLOBAL_AREA_VENDOR_COUNTRY,	FLASH_GLOBAL_AREA,	2,    	    2,	     0,	       0,	COUNTRY_EMPTY},  //   1
#endif

#ifdef USE_TRANSFER_MENU_SETTING
//menu setting area
	{  1071,	 0x1721,		GLOBAL_LABEL_MARKDOWNFLAG,   	FLASH_GLOBAL_AREA,		1,	    4,	     0,	       1,	COUNTRY_EMPTY}, //menu 1720
	{  1072,	 0x1722,		GLOBAL_LABEL_MARKDOWNFLAG,   	FLASH_GLOBAL_AREA,		1,	    4,	     1,	       1,	COUNTRY_EMPTY},
	{  1073,	 0x1723,		GLOBAL_LABEL_MARKDOWNFLAG,   	FLASH_GLOBAL_AREA,		1,	    4,	     2,	       1,	COUNTRY_EMPTY},
	{  1074,	 0x1724,		GLOBAL_LABEL_MARKDOWNFLAG,   	FLASH_GLOBAL_AREA,		1,	    4,	     3,	       1,	COUNTRY_EMPTY},
    
    {  1101,     0x3170,        GLOBAL_PRINT_OPER_FLAG2,	    FLASH_GLOBAL_AREA,	  1,	  4,	  0,      1,      COUNTRY_UN  }, //menu 1170
	{  1431,	 0x2701,		GLOBAL_SCROLL_WAIT_TIME,		FLASH_GLOBAL_AREA,    2,	  2,	  0,	  0,	  COUNTRY_EMPTY	 }, //menu 1431
	{  1501,      0x2011,       GLOBAL_LABEL_PRIORITY,  	    FLASH_GLOBAL_AREA,	  1,	  1,      0,	  0,      COUNTRY_UN  },
    {  1502,      0x2012,       GLOBAL_LABEL_STANDARD,	        FLASH_GLOBAL_AREA,	  2,	  2,      0,	  0,      COUNTRY_UN  },
    {  1503,      0x2013,       GLOBAL_LABEL_MISC,	            FLASH_GLOBAL_AREA,	  2,	  2,      0,	  0,      COUNTRY_UN  },
    {  1504,      0x2014,       GLOBAL_LABEL_ADDUP,    	        FLASH_GLOBAL_AREA,	  2,	  2,      0,	  0,      COUNTRY_UN  },
    {  1505,      0x2016,	    GLOBAL_PRINT_ADDUP_FLAG2,	    FLASH_GLOBAL_AREA,	  1,	  4,      2,	  1,      COUNTRY_UN  },
    {  1506,      0x2017,	    GLOBAL_PRINT_ADDUP_FLAG2,	    FLASH_GLOBAL_AREA,	  1,	  4,      3,	  1,      COUNTRY_UN  },
    {  1507,      0x2019,	    GLOBAL_LABEL_2,           	    FLASH_GLOBAL_AREA,	  1,	  1,      0,	  0,      COUNTRY_UN  },
    {  1508,      0x3521,	    GLOBAL_BARCODE_PRIORITY,	    FLASH_GLOBAL_AREA,	  1,	  1,      0,	  0,      COUNTRY_UN  },
    {  1509,      0x3522,	    GLOBAL_BARCODE_STD_WID, 	    FLASH_GLOBAL_AREA,	  1,	  1,      0,	  0,      COUNTRY_UN  },
    {  1510,      0x3523,	    GLOBAL_BARCODE_STD_WID+1,	    FLASH_GLOBAL_AREA,	  1,	  1,      0,	  0,      COUNTRY_UN  },
    {  1511,      0x3524,	    GLOBAL_BARCODE_STD_WID+2,	    FLASH_GLOBAL_AREA,	  1,	  1,      0,	  0,      COUNTRY_UN  },
    {  1512,      0x3525,	    GLOBAL_BARCODE_STD_WID+3,	    FLASH_GLOBAL_AREA,	  1,	  1,      0,	  0,      COUNTRY_UN  },
    {  1513,      0x3526,	    GLOBAL_BARCODE_STD_WID+4,	    FLASH_GLOBAL_AREA,	  1,	  1,      0,	  0,      COUNTRY_UN  },
    {  1514,      0x3527,	    GLOBAL_BARCODE_STD_WID+5,	    FLASH_GLOBAL_AREA,	  1,	  1,      0,	  0,      COUNTRY_UN  },
    {  1515,      0x3528,	    GLOBAL_BARCODE_STD_WID+6,	    FLASH_GLOBAL_AREA,	  1,	  1,      0,	  0,      COUNTRY_UN  },
    {  1516,      0x3529,	    GLOBAL_BARCODE_STD_WID+7,	    FLASH_GLOBAL_AREA,	  1,	  1,      0,	  0,      COUNTRY_UN  },
    {  1517,      0x352A,	    GLOBAL_BARCODE_STD_WID+8,	    FLASH_GLOBAL_AREA,	  1,	  1,      0,	  0,      COUNTRY_UN  },
    {  1518,      0x352C,	    GLOBAL_BARCODE_2,	            FLASH_GLOBAL_AREA,	  1,	  1,      0,	  0,      COUNTRY_UN  },
  //  {  1517,      0x3530,	    GLOBAL_BARCODE_STD_WID+8,	    FLASH_GLOBAL_AREA,	  1,	  1,      0,	  0,      COUNTRY_UN  },
  //  {  1518,      0x3531,	    GLOBAL_BARCODE_2,	            FLASH_GLOBAL_AREA,	  1,	  1,      0,	  0,      COUNTRY_UN  },
    {  1519,      0x2041,	    GLOBAL_TAX_FLAG,	            FLASH_GLOBAL_AREA,	  1,	  1,      0,	  0,      COUNTRY_UN  },
    {  1520,      0x2042,	    GLOBAL_TAX_VALUE,	            FLASH_GLOBAL_AREA,	  1,	  1,      0,	  0,      COUNTRY_UN  },
    {  1521,      0x2043,	    GLOBAL_MISC_TAX_VALUE,	            FLASH_GLOBAL_AREA,	  1,	  1,      0,	  0,      COUNTRY_UN  },
    {  1531,	  0x203A,		GLOBAL_DISCOUNT_PRIORITY,    		FLASH_GLOBAL_AREA,		1,	    1,	     0,	       0,	COUNTRY_EMPTY},   
    
	{  1701,      0x1A40,	    GLOBAL_PRT_INHIBIT,	                FLASH_GLOBAL_AREA,	  1,	  4,      1,	  1,      COUNTRY_UN  },
	{  1702,      0x1A41,	    GLOBAL_PRT_INHIBIT+14,	            FLASH_GLOBAL_AREA,    1,	  4,      3,	  1,      COUNTRY_UN  },
	{  1703,      0x1A42,	    GLOBAL_PRT_INHIBIT+1,	            FLASH_GLOBAL_AREA,    1,	  4,      1,	  1,      COUNTRY_UN  },
	{  1704,      0x1A43,	    GLOBAL_PRT_INHIBIT,	                FLASH_GLOBAL_AREA,	  1,	  4,      2,	  1,      COUNTRY_UN  },
	{  1705,      0x1A44,	    GLOBAL_PRT_INHIBIT+3,	            FLASH_GLOBAL_AREA,    1,	  4,      5,	  1,      COUNTRY_UN  },
	{  1706,      0x1A45,	    GLOBAL_PRT_INHIBIT+3,	            FLASH_GLOBAL_AREA,    1,	  4,      6,	  1,      COUNTRY_UN  },
	{  1707,      0x1A46,	    GLOBAL_PRT_INHIBIT+3,	            FLASH_GLOBAL_AREA,    1,	  4,      7,	  1,      COUNTRY_UN  },
	{  1708,      0x1A47,	    GLOBAL_PRT_INHIBIT+1,	            FLASH_GLOBAL_AREA,    1,	  4,      3,	  1,      COUNTRY_UN  },
	
	{  1709,      0x1A48,	    GLOBAL_PRT_INHIBIT,                 FLASH_GLOBAL_AREA,	  1,	  4,      5,	  1,      COUNTRY_UN  },
	{  1710,      0x1A49,	    GLOBAL_PRT_INHIBIT+1,	            FLASH_GLOBAL_AREA,    1,	  4,      0,	  1,      COUNTRY_UN  },
	{  1711,      0x1A4A,	    GLOBAL_PRT_INHIBIT+13,	            FLASH_GLOBAL_AREA,    1,	  4,      7,	  1,      COUNTRY_UN  },
	{  1712,      0x1A4B,	    GLOBAL_PRT_INHIBIT+1,	            FLASH_GLOBAL_AREA,    1,	  4,      5,	  1,      COUNTRY_UN  },
	{  1713,      0x1A4C,	    GLOBAL_PRT_INHIBIT+1,	            FLASH_GLOBAL_AREA,    1,	  4,      6,	  1,      COUNTRY_UN  },
	{  1714,      0x1A4D,	    GLOBAL_PRT_INHIBIT+1,	            FLASH_GLOBAL_AREA,    1,	  4,      7,	  1,      COUNTRY_UN  },
	{  1715,      0x1A4E,	    GLOBAL_PRT_INHIBIT+2,	            FLASH_GLOBAL_AREA,    1,	  4,      0,	  1,      COUNTRY_UN  },
	{  1716,      0x1A4F,	    GLOBAL_PRT_INHIBIT+2,	            FLASH_GLOBAL_AREA,    1,	  4,      1,	  1,      COUNTRY_UN  },

	{  1717,      0x1A50,	    GLOBAL_PRT_INHIBIT+2,               FLASH_GLOBAL_AREA,    1,	  4,      2,	  1,      COUNTRY_UN  },
	{  1718,      0x1A51,	    GLOBAL_PRT_INHIBIT+2,	            FLASH_GLOBAL_AREA,    1,	  4,      3,	  1,      COUNTRY_UN  },
	{  1719,      0x1A52,	    GLOBAL_PRT_INHIBIT+2,	            FLASH_GLOBAL_AREA,    1,	  4,      4,	  1,      COUNTRY_UN  },
	{  1720,      0x1A53,	    GLOBAL_PRT_INHIBIT+2,	            FLASH_GLOBAL_AREA,    1,	  4,      6,	  1,      COUNTRY_UN  },
	{  1721,      0x1A54,	    GLOBAL_PRT_INHIBIT+6,	            FLASH_GLOBAL_AREA,    1,	  4,      2,	  1,      COUNTRY_UN  },
	{  1722,      0x1A55,	    GLOBAL_PRT_INHIBIT+6,	            FLASH_GLOBAL_AREA,    1,	  4,      7,	  1,      COUNTRY_UN  },
	{  1723,      0x1A56,	    GLOBAL_PRT_INHIBIT+3,	            FLASH_GLOBAL_AREA,    1,	  4,      1,	  1,      COUNTRY_UN  },
	{  1724,      0x1A57,	    GLOBAL_PRT_INHIBIT+11,	            FLASH_GLOBAL_AREA,    1,	  4,      2,	  1,      COUNTRY_UN  },

	{  1725,      0x1A58,	    GLOBAL_PRT_INHIBIT+12,              FLASH_GLOBAL_AREA,    1,	  4,      4,	  1,      COUNTRY_UN  },
	{  1726,      0x1A59,	    GLOBAL_PRT_INHIBIT+14,	            FLASH_GLOBAL_AREA,    1,	  4,      4,	  1,      COUNTRY_UN  },
	{  1727,      0x1A5A,	    GLOBAL_PRT_INHIBIT+3,	            FLASH_GLOBAL_AREA,    1,	  4,      0,	  1,      COUNTRY_UN  },
	{  1728,      0x1A5B,	    GLOBAL_PRT_INHIBIT+2,	            FLASH_GLOBAL_AREA,    1,	  4,      7,	  1,      COUNTRY_UN  },
	{  1729,      0x1A5C,	    GLOBAL_PRT_INHIBIT+16,	            FLASH_GLOBAL_AREA,    1,	  4,      3,	  1,      COUNTRY_UN  },
	{  1730,      0x1A5D,	    GLOBAL_PRT_INHIBIT+16,	            FLASH_GLOBAL_AREA,    1,	  4,      7,	  1,      COUNTRY_UN  },
	{  1731,      0x1A5E,	    GLOBAL_PRT_INHIBIT+13,	            FLASH_GLOBAL_AREA,    1,	  4,      1,	  1,      COUNTRY_UN  },
	{  1732,      0x1A5F,	    GLOBAL_PRT_INHIBIT+16,	            FLASH_GLOBAL_AREA,    1,	  4,      0,	  1,      COUNTRY_UN  },

	{  1733,      0x1A60,	    GLOBAL_PRT_INHIBIT+13,              FLASH_GLOBAL_AREA,    1,	  4,      2,	  1,      COUNTRY_UN  },
	{  1734,      0x1A61,	    GLOBAL_PRT_INHIBIT+13,	            FLASH_GLOBAL_AREA,    1,	  4,      3,	  1,      COUNTRY_UN  },
	{  1735,      0x1A62,	    GLOBAL_PRT_INHIBIT+16,	            FLASH_GLOBAL_AREA,    1,	  4,      2,	  1,      COUNTRY_UN  },
	{  1736,      0x1A63,	    GLOBAL_PRT_INHIBIT+17,	            FLASH_GLOBAL_AREA,    1,	  4,      1,	  1,      COUNTRY_UN  },
	{  1737,      0x1A64,	    GLOBAL_PRT_INHIBIT+14,	            FLASH_GLOBAL_AREA,    1,	  4,      5,	  1,      COUNTRY_UN  },
	{  1738,      0x1A65,	    GLOBAL_PRT_INHIBIT+14,	            FLASH_GLOBAL_AREA,    1,	  4,      7,	  1,      COUNTRY_UN  },
	{  1739,      0x1A66,	    GLOBAL_PRT_INHIBIT+14,	            FLASH_GLOBAL_AREA,    1,	  4,      6,	  1,      COUNTRY_UN  },
	{  1740,      0x1A67,	    GLOBAL_PRT_INHIBIT+15,	            FLASH_GLOBAL_AREA,    1,	  4,      0,	  1,      COUNTRY_UN  },

	{  1741,      0x1A68,	    GLOBAL_PRT_INHIBIT+4,               FLASH_GLOBAL_AREA,    1,	  4,      3,	  1,      COUNTRY_UN  },
	{  1742,      0x1A69,	    GLOBAL_PRT_INHIBIT+17,	            FLASH_GLOBAL_AREA,    1,	  4,      4,	  1,      COUNTRY_UN  },
	{  1743,      0x1A6A,	    GLOBAL_PRT_INHIBIT+21,	            FLASH_GLOBAL_AREA,    1,	  4,      2,	  1,      COUNTRY_UN  },
	{  1744,      0x1A6B,	    GLOBAL_PRT_INHIBIT+26,	            FLASH_GLOBAL_AREA,    1,	  4,      2,	  1,      COUNTRY_UN  },
	{  1745,      0x1A6C,	    GLOBAL_PRT_INHIBIT+26,	            FLASH_GLOBAL_AREA,    1,	  4,      4,	  1,      COUNTRY_UN  },
	{  1746,      0x1A6D,	    GLOBAL_PRT_INHIBIT+30,	            FLASH_GLOBAL_AREA,    1,	  4,      0,	  1,      COUNTRY_UN  },
	{  1747,      0x1A6E,	    GLOBAL_PRT_INHIBIT+31,	            FLASH_GLOBAL_AREA,    1,	  4,      2,	  1,      COUNTRY_UN  },
	{  1748,      0x1A6F,	    GLOBAL_PRT_INHIBIT+12,	            FLASH_GLOBAL_AREA,    1,	  4,      5,	  1,      COUNTRY_UN  },

	{  1749,      0x1A70,	    GLOBAL_PRT_INHIBIT+14,              FLASH_GLOBAL_AREA,    1,	  4,      0,	  1,      COUNTRY_UN  },
	{  1750,      0x1A71,	    GLOBAL_PRT_INHIBIT,	                FLASH_GLOBAL_AREA,    1,	  4,      6,	  1,      COUNTRY_UN  },
	{  1751,      0x1A72,	    GLOBAL_PRT_INHIBIT+12,	            FLASH_GLOBAL_AREA,    1,	  4,      6,	  1,      COUNTRY_UN  },
  
    {  1752,      0x9351,	    GLOBAL_PAPER_TYPE,	                FLASH_GLOBAL_AREA,	  1,	  1,      0,	  0,      COUNTRY_UN  }, //menu 1731
    {  1753,      0x9341,	    GLOBAL_PRT_PAPER_WIDTH+0*2,	        FLASH_GLOBAL_AREA,	  2,	  2,      0,	  0,      COUNTRY_UN  }, //menu 1732
    {  1754,      0x9342,	    GLOBAL_PRT_PAPER_HEIGHT+0*2,	    FLASH_GLOBAL_AREA,	  2,	  2,      0,	  0,      COUNTRY_UN  },
    {  1755,      0x9343,	    GLOBAL_PRT_PAPER_ENDMARGIN+0*2,	    FLASH_GLOBAL_AREA,	  2,	  2,      0,	  0,      COUNTRY_UN  },
    {  1756,      0x9341,	    GLOBAL_PRT_PAPER_WIDTH+1*2,	        FLASH_GLOBAL_AREA,	  2,	  2,      0,	  0,      COUNTRY_UN  },
    {  1757,      0x9342,	    GLOBAL_PRT_PAPER_HEIGHT+1*2,	    FLASH_GLOBAL_AREA,	  2,	  2,      0,	  0,      COUNTRY_UN  },
    {  1758,      0x9344,	    GLOBAL_PRT_PAPER_ENDMARGIN+1*2,	    FLASH_GLOBAL_AREA,	  2,	  2,      0,	  0,      COUNTRY_UN  },
    {  1759,      0x9341,	    GLOBAL_PRT_PAPER_WIDTH+2*2,	        FLASH_GLOBAL_AREA,	  2,	  2,      0,	  0,      COUNTRY_UN  },
    {  1760,      0x9342,	    GLOBAL_PRT_PAPER_HEIGHT+2*2,	    FLASH_GLOBAL_AREA,	  2,	  2,      0,	  0,      COUNTRY_UN  },
    {  1761,      0x9344,	    GLOBAL_PRT_PAPER_ENDMARGIN+2*2,	    FLASH_GLOBAL_AREA,	  2,	  2,      0,	  0,      COUNTRY_UN  },
    {  1762,      0x9701,	    FLASH_OFFSET_GapThreshold,	        FLASH_PRINTER_AREA,	  1,      1,      0,      0,      COUNTRY_UN  }, //menu 1733
    {  1763,      0x9702,	    FLASH_OFFSET_PeelThreshold,	        FLASH_PRINTER_AREA,	  1,      1,      0,      0,      COUNTRY_UN  },
    {  1764,      0x9361,	    GLOBAL_PRT_SENSOR_MOTOR+0,          FLASH_GLOBAL_AREA,	  1,	  4,      0, 	  1,      COUNTRY_UN  }, //menu 1734
    {  1765,      0x9362,	    GLOBAL_PRT_SENSOR_MOTOR+0,          FLASH_GLOBAL_AREA,	  1,	  4,      1,	  1,      COUNTRY_UN  },
    {  1766,      0x9363,	    GLOBAL_PRT_SENSOR_MOTOR+0,          FLASH_GLOBAL_AREA,	  1,	  4,      2,	  1,      COUNTRY_UN  },
    {  1767,      0x9361,	    GLOBAL_PRT_SENSOR_MOTOR+1,          FLASH_GLOBAL_AREA,	  1,	  4,      0, 	  1,      COUNTRY_UN  },
    {  1768,      0x9362,	    GLOBAL_PRT_SENSOR_MOTOR+1,          FLASH_GLOBAL_AREA,	  1,	  4,      1,	  1,      COUNTRY_UN  },
    {  1769,      0x9363,	    GLOBAL_PRT_SENSOR_MOTOR+1,          FLASH_GLOBAL_AREA,	  1,	  4,      2,	  1,      COUNTRY_UN  },
    {  1770,      0x9361,	    GLOBAL_PRT_SENSOR_MOTOR+2,          FLASH_GLOBAL_AREA,	  1,	  4,      0, 	  1,      COUNTRY_UN  },
    {  1771,      0x9362,	    GLOBAL_PRT_SENSOR_MOTOR+2,          FLASH_GLOBAL_AREA,	  1,	  4,      1,	  1,      COUNTRY_UN  },
    {  1772,      0x9363,	    GLOBAL_PRT_SENSOR_MOTOR+2,          FLASH_GLOBAL_AREA,	  1,	  4,      2,	  1,      COUNTRY_UN  },
    {  1773,      0x9364,	    GLOBAL_PRT_HEADUP_SENSOR,	        FLASH_GLOBAL_AREA,	  1,	  1,      0,	  0,      COUNTRY_UN  },
    {  1774,      0x9711,	    FLASH_OFFSET_StbTimeTrimValue,	    FLASH_PRINTER_AREA,	  2,      2,      0,      0,      COUNTRY_UN  }, //menu 1735
    {  1775,      0x9731,	    FLASH_OFFSET_GapBufInPtrTrimValue,  FLASH_PRINTER_AREA,	  2,	  2,      0,      0,      COUNTRY_UN  }, //menu 1736
    {  1776,      0x9771,	    FLASH_OFFSET_PrePrintMode,	        FLASH_PRINTER_AREA,	  1,	  1,      0,      0,      COUNTRY_UN  }, //menu 1737
    {  1777,      0x9772,	    FLASH_OFFSET_PrePrintLength,	    FLASH_PRINTER_AREA,	  1,	  1,      0,      0,      COUNTRY_UN  },
    {  1778,      0x3741,	    GLOBAL_LABEL_SERIAL_FORM,	        FLASH_GLOBAL_AREA,	 32,	  3,      0,  	  0,      COUNTRY_UN  },
    {  1779,      0x3742,	    GLOBAL_TICKET_SERIAL_FORM,	        FLASH_GLOBAL_AREA,	 32,	  3,      0,	  0,      COUNTRY_UN  },
    {  1780,      0x38C3,	    GLOBAL_PRINT_ADDUP_FLAG1,	        FLASH_GLOBAL_AREA,	  1,	  4,      3,	  1,      COUNTRY_UN  }, //menu 1750
    {  1781,      0x38C4,	    GLOBAL_PRINT_ADDUP_FLAG2,	        FLASH_GLOBAL_AREA,	  1,	  4,      0,	  1,      COUNTRY_UN  },
    {  1782,      0x38C1,	    GLOBAL_PRINT_CLERK_FLAG1,         	FLASH_GLOBAL_AREA,	  1,	  4,      2,	  1,      COUNTRY_UN  }, //menu 1761
    {  1783,      0x38C2,	    GLOBAL_PRINT_CLERK_FLAG1,	        FLASH_GLOBAL_AREA,	  1,	  4,      3,	  1,      COUNTRY_UN  },
    {  1784,      0x38C3,	    GLOBAL_PRINT_CLERK_FLAG1,         	FLASH_GLOBAL_AREA,	  1,	  4,      4,	  1,      COUNTRY_UN  },
    {  1785,      0x38C5,	    GLOBAL_PRINT_CLERK_FLAG2,       	FLASH_GLOBAL_AREA,	  1,	  4,      3,	  1,      COUNTRY_UN  },
    {  1786,      0x38C6,	    GLOBAL_PRINT_CLERK_FLAG2,       	FLASH_GLOBAL_AREA,	  1,	  4,      4,	  1,      COUNTRY_UN  },
    {  1787,      0x38B1,	    GLOBAL_PRINT_CLERK_FLAG1,	        FLASH_GLOBAL_AREA,	  1,	  4,      1,	  1,      COUNTRY_UN  }, //menu 1762
    {  1788,      0x38B2,	    GLOBAL_PRINT_CLERK_FLAG1,	        FLASH_GLOBAL_AREA,	  1,	  4,      0,	  1,      COUNTRY_UN  },
    {  1789,      0x38B3,	    GLOBAL_PRINT_CLERK_FLAG1,	        FLASH_GLOBAL_AREA,	  1,      4,      5,	  1,      COUNTRY_UN  },
    {  1790,      0x38B4,	    GLOBAL_PRINT_CLERK_FLAG2,         	FLASH_GLOBAL_AREA,    1,	  4,      1,	  1,      COUNTRY_UN  },
    {  1791,      0x38B5,	    GLOBAL_PRINT_CLERK_FLAG2,       	FLASH_GLOBAL_AREA,	  1,	  4,      2,	  1,      COUNTRY_UN  },
    {  1792,      0x38B6,	    GLOBAL_TICKET_MESSAGE_HEAD,     	FLASH_GLOBAL_AREA,	  2,	  2,      0,	  0,      COUNTRY_UN  },
    {  1793,      0x38B7,	    GLOBAL_TICKET_MESSAGE_TAIL,     	FLASH_GLOBAL_AREA,	  2,	  2,      0,      0,      COUNTRY_UN  },
    {  1794,      0x38E1,	    GLOBAL_PRT_TICKET_FONT_SIZE,    	FLASH_GLOBAL_AREA,	  1,	  4,      1,	  2,      COUNTRY_UN  }, //menu 1763
    {  1795,      0x38E2,	    GLOBAL_PRT_TICKET_FONT_SIZE,    	FLASH_GLOBAL_AREA,	  1,	  4,      3,	  2,      COUNTRY_UN  },
    {  1796,      0x38E3,	    GLOBAL_PRT_TICKET_FONT_SIZE,    	FLASH_GLOBAL_AREA,	  1,	  4,      5,	  2,      COUNTRY_UN  },
    {  1797,      0x38E4,	    GLOBAL_PRT_TICKET_FONT_SIZE,    	FLASH_GLOBAL_AREA,	  1,	  4,      7,	  2,      COUNTRY_UN  },
	{  1798,	  0x9733,FLASH_OFFSET_GapBufInPtrReverseTrimValue,	FLASH_PRINTER_AREA,	  2,	  2,	  0,	  0,	  COUNTRY_UN  }, //menu 1738
//M8220
    {  1801,      0x9171,	    GLOBAL_AREA_SCALETYPE,	            FLASH_GLOBAL_AREA,	  1,	  1,      0,	  0,      COUNTRY_UN  },
//M1820
    {  1802,      0x20D1,	    GLOBAL_AREA_SALEMODE,	            FLASH_GLOBAL_AREA,	  1,	  1,      0,	  0,      COUNTRY_UN  },
    {  1803,      0x20D2,	    GLOBAL_OPERATION_MODE,	            FLASH_GLOBAL_AREA,	  1,	  1,      0,	  0,      COUNTRY_UN  },
//M1830
    {  1804,      0x141A,	    NETWORK_STORE_ID,	                FLASH_NET_AREA,	  2,      2,      0,      0,      COUNTRY_UN  },
//M1840
    {  1805,      0x3843,	    GLOBAL_DATETIME_MODE,	            FLASH_GLOBAL_AREA,	  1,	  4,      6,	  1,      COUNTRY_UN  },
    {  1806,      0x3844,	    GLOBAL_DATETIME_MODE,	            FLASH_GLOBAL_AREA,	  1,	  4,      7,	  1,      COUNTRY_UN  },
    {  1807,      0x3845,	    GLOBAL_DATETIME_MODE,	            FLASH_GLOBAL_AREA,	  1,	  4,      2,	  3,      COUNTRY_UN  },
    {  1808,      0x3846,	    GLOBAL_DATE_SEPARATOR,	            FLASH_GLOBAL_AREA,	  1,	  3,      0,	  0,      COUNTRY_UN  },
    {  1809,      0x3847,	    GLOBAL_TIME_SEPARATOR,	            FLASH_GLOBAL_AREA,	  1,	  3,      0,	  0,      COUNTRY_UN  },
//M1871
    {  1810,      0x21b1,	    GLOBAL_DISPLAY_INFO_FLAG,	        FLASH_GLOBAL_AREA,	  1,	  4,      0,	  1,      COUNTRY_UN  },
    {  1811,      0x21b2,	    GLOBAL_DISPLAY_INFO_FLAG,	        FLASH_GLOBAL_AREA,	  1,	  4,      3,	  1,      COUNTRY_UN  },
    {  1812,      0x21b3,	    GLOBAL_DISPLAY_ALIGN,	            FLASH_GLOBAL_AREA,	  1,	  1,      0,	  0,      COUNTRY_UN  },
    {  1813,      0x21b4,	    GLOBAL_SALE_SETUP_FLAG7,	        FLASH_GLOBAL_AREA,	  1,	  4,      2,	  1,      COUNTRY_UN  },
//M1872
    {  1814,      0x3871,	    GLOBAL_PRINT_OPER_FLAG,             FLASH_GLOBAL_AREA,	  1,	  4,      1,	  1,      COUNTRY_UN  },
    {  1815,      0x3872,	    GLOBAL_PRINT_OPER_FLAG,	            FLASH_GLOBAL_AREA,	  1,	  4,      3,	  2,      COUNTRY_UN  },
    {  1816,      0x3873,	    GLOBAL_PRINT_OPER_FLAG,	            FLASH_GLOBAL_AREA,	  1,	  4,      5,	  2,      COUNTRY_UN  },
    {  1817,      0x3874,	    GLOBAL_LABEL_FLAG,	                FLASH_GLOBAL_AREA,	  1,	  4,      0,	  1,      COUNTRY_UN  },
    {  1818,      0x3875,	    GLOBAL_LABEL_FLAG,	                FLASH_GLOBAL_AREA,	  1,	  4,      1,	  1,      COUNTRY_UN  },
    {  1819,      0x3876,	    GLOBAL_USE_REPORT,	                FLASH_GLOBAL_AREA,	  1,	  4,      0,	  1,      COUNTRY_UN  },
    {  1820,      0x3877,	    GLOBAL_SALE_SETUP_FLAG2,	        FLASH_GLOBAL_AREA,	  1,	  4,      7,	  2,      COUNTRY_UN  },
    {  1821,      0x3878,	    GLOBAL_SALE_SETUP_FLAG5,	        FLASH_GLOBAL_AREA,	  1,	  4,      2,	  2,      COUNTRY_UN  },
    {  1822,      0x3879,	    GLOBAL_SALE_SETUP_FLAG5,	        FLASH_GLOBAL_AREA,	  1,	  4,      5,	  1,      COUNTRY_UN  },
    {  1823,      0x387A,	    GLOBAL_SALE_SETUP_FLAG11,	        FLASH_GLOBAL_AREA,	  1,	  4,      7,	  1,      COUNTRY_UN  },
//M1873
    {  1824,      0x3891,	    GLOBAL_SALE_SAVE_FLAG,	            FLASH_GLOBAL_AREA,	  1,	  4,	  0,      1,      COUNTRY_UN  },
    {  1825,      0x3892,	    GLOBAL_SALE_SAVE_FLAG,	            FLASH_GLOBAL_AREA,	  1,	  4,      1,	  1,      COUNTRY_UN  },
    {  1826,      0x3893,	    GLOBAL_SALE_SETUP_FLAG1,	        FLASH_GLOBAL_AREA,	  1,	  4,      0,	  1,      COUNTRY_UN  },
    {  1827,      0x3894,	    GLOBAL_SALE_SETUP_FLAG1,	        FLASH_GLOBAL_AREA,	  1,	  4,      5,	  1,      COUNTRY_UN  },
    {  1828,      0x3895,	    GLOBAL_PRINT_OPER_FLAG,	            FLASH_GLOBAL_AREA,	  1,	  4,      6,  	  1,      COUNTRY_UN  },
    {  1829,      0x3896,	    GLOBAL_PRINT_OPER_FLAG,	            FLASH_GLOBAL_AREA,	  1,	  4,      7,	  1,      COUNTRY_UN  },
    {  1830,      0x3897,	    GLOBAL_DISPLAY_INFO_FLAG,	        FLASH_GLOBAL_AREA,	  1,	  4,      1,	  1,      COUNTRY_UN  },
    {  1831,      0x3898,	    GLOBAL_SALE_SETUP_FLAG2,	        FLASH_GLOBAL_AREA,	  1,	  4,      2,	  1,      COUNTRY_UN  },
    {  1832,      0x3899,	    GLOBAL_SALE_SETUP_FLAG10,	        FLASH_GLOBAL_AREA,	  1,	  4,      6,	  2,      COUNTRY_UN  },
    {  1833,      0x3890,	    GLOBAL_SALE_SETUP_FLAG11,	        FLASH_GLOBAL_AREA,	  1,	  4,      3,  	  4,      COUNTRY_UN  },
    {  1834,      0x389E,	    GLOBAL_SALE_SETUP_FLAG7,	        FLASH_GLOBAL_AREA,	  1,	  4,      3,   	  1,      COUNTRY_UN  },
    {  1835,      0x389A,	    GLOBAL_SALE_SETUP_FLAG6,	        FLASH_GLOBAL_AREA,	  1,	  4,      0,	  1,      COUNTRY_UN  },
    {  1836,      0x389B,	    GLOBAL_SALE_SETUP_FLAG6,	        FLASH_GLOBAL_AREA,	  1,	  4,      2,	  1,      COUNTRY_UN  },
    {  1837,      0x389C,	    GLOBAL_SALE_SETUP_FLAG5,	        FLASH_GLOBAL_AREA,	  1,	  4,      3,	  1,      COUNTRY_UN  },
//M1874
    {  1838,      0x38A1,	    GLOBAL_LOG_FLAG,	                FLASH_GLOBAL_AREA,	  1,	  4,      0,	  1,      COUNTRY_UN  },
    {  1839,      0x38A2,	    GLOBAL_TIME_CLERKLOGOUT,	        FLASH_GLOBAL_AREA,	  2,	  2,      0,	  0,      COUNTRY_UN  },
//M1871
	{  1840,      0x3F20,	    GLOBAL_SALERUN_MODE,				FLASH_GLOBAL_AREA,	  1,	  1,	  0,	  0,	  COUNTRY_EMPTY	 }, //menu 1810
//	{  1841,	  0x3899,	 	GLOBAL_SALE_SETUP_FLAG2,	     	FLASH_GLOBAL_AREA,	  1,      4,      4,      1,	  COUNTRY_EMPTY	 },//제거됨-> 현대 전용에서 사용
//	{  1842,	  0x389A,	 	GLOBAL_SALE_SETUP_FLAG6,	     	FLASH_GLOBAL_AREA,    1,      4,      0,      1,	  COUNTRY_EMPTY	 },//중복됨(1835)
//KEY 50(MENU아님)
	{  1843,	  0x0000,	 	GLOBAL_SALE_SETUP_FLAG6,	     	FLASH_GLOBAL_AREA,	  1,      4,      1,      1,	  COUNTRY_EMPTY	 },  // Date-Print ON/OFF key(49), No caption
#ifdef USE_KOR_TRACE
//M1873
	{  1844,	  0x389D,	 	GLOBAL_SALE_SETUP_FLAG5,	     	FLASH_GLOBAL_AREA,	  1,      4,      4,      1,	  COUNTRY_EMPTY	 },
#endif
//M1871
	{  1845,	  0x21B4,		GLOBAL_SALE_SETUP_FLAG7,			FLASH_GLOBAL_AREA,	  1,	  4,	  2,	  1,	  COUNTRY_EMPTY	 },
#ifdef USE_TRACE_STANDALONE
//M1873
	{  1846,	  0x389F,		GLOBAL_SALE_SETUP_FLAG10,			FLASH_GLOBAL_AREA,	  1,      4,	  2,	  3,	  COUNTRY_EMPTY	 }, //Menu1873 Type of saving trace. no :, KOR-Meat에만 caption 존재
	{  1847,	  0x38A0,		GLOBAL_SALE_SETUP_FLAG10,			FLASH_GLOBAL_AREA,	  1,      4,	  4,	  2,	  COUNTRY_EMPTY  }, //Menu1873 Use sending trace. Info:, KOR-Meat에만 caption 존재
#endif
//M1920
	{  1848,	  0x387B,		GLOBAL_PRINT_OPER_FLAG2,			FLASH_GLOBAL_AREA,	  1,      4,	  5,	  1,	  COUNTRY_EMPTY  },
#ifdef USE_INDIA_FUNCTION
	{  1849,	  0x1C6A,		GLOBAL_SALE_SETUP_FLAG12,			FLASH_GLOBAL_AREA,	  1,      4,	  7,	  2,	  COUNTRY_EMPTY  }, //Menu1873 India Scale Mode (IND Only)
	{  1850,	  0x1C6B,		GLOBAL_PRICESIGN_1000,				FLASH_GLOBAL_AREA,	  1,      4,	  2,	  1,	  COUNTRY_EMPTY  }, //Menu1873 Use Ext.Barcode as ItemCode (IND Only)
	{  1851,	  0x1C6C,		GLOBAL_PRICESIGN_1000,				FLASH_GLOBAL_AREA,	  1,      4,	  1,	  1,	  COUNTRY_EMPTY  }, //Menu1873 Allow Negative Inventory (IND Only)
#endif
#if defined(USE_CHECK_CHANGE_REASON) || defined(USE_HYUNDAI_CHECK_REASON_CODE)
    {  1852,	  0x1BD2, 	    GLOBAL_SALE_SETUP_FLAG12, 		    FLASH_GLOBAL_AREA, 		  1, 	  4,	  4,	  1, 	  COUNTRY_KR  }, //Menu1873 Ask reason when price changed
#endif
#ifdef USE_HYUNDAI_DEFAULT
	{  1853,	  0x3879, 	    GLOBAL_SALE_SETUP_FLAG5, 		    FLASH_GLOBAL_AREA, 		  1, 	  4,	  5,	  1, 	  COUNTRY_KR  }, // Print Item when "Return"
#endif
//M1873
	{  1854,	  0x38A3,		GLOBAL_SALE_SETUP_FLAG15,			FLASH_GLOBAL_AREA,		  1,	  4,	  4,	  1,  COUNTRY_UN  }, // Menu1873
	{  1855,	  0x38A7,		GLOBAL_SALE_SETUP_FLAG17,			FLASH_GLOBAL_AREA,		  1,	  4,	  7,	  1,  COUNTRY_UN  }, // Menu1873
#ifdef USE_HYUNDAI_MEAT_FUNCTION
	{  1856,	  0x1BD4,		GLOBAL_SALE_SETUP_FLAG2,			FLASH_GLOBAL_AREA,		  1,	  4,	  4,	  1,  COUNTRY_KR  }, // Menu1873 현대 전용 축산 기능 Y/N
#endif
#ifdef USE_QR_SIZE_SETTING
	{  1857,	  0x38A8,		GLOBAL_SALE_SETUP_FLAG21,			FLASH_GLOBAL_AREA,		  1,	  4,	  2,	  3,  COUNTRY_UN  }, // Menu1873 Ticket QR Size in Add-Total, size 1~5 
#endif
//M1855
	{  1861,	  0x1D51,		FLASH_CLERK_PERMISSION+0,			0,		  1,	  1,	  0,	  0,  COUNTRY_UN  }, // Menu1855 change item				
	{  1862,	  0x1D52,		FLASH_CLERK_PERMISSION+1,			0,		  1,	  1,	  0,	  0,  COUNTRY_UN  }, // Menu1855 change price				
	{  1863,	  0x1D53,		FLASH_CLERK_PERMISSION+2,			0,		  1,	  1,	  0,	  0,  COUNTRY_UN  }, // Menu1855 key_dept				
	{  1864,	  0x1D54,		FLASH_CLERK_PERMISSION+3,			0,		  1,	  1,	  0,	  0,  COUNTRY_UN  }, // Menu1855 key_pay				
	{  1865,	  0x1D55,		FLASH_CLERK_PERMISSION+4,			0,		  1,	  1,	  0,	  0,  COUNTRY_UN  }, // Menu1855 key_date_cahge 			
	{  1866,	  0x1D56,		FLASH_CLERK_PERMISSION+5,			0,		  1,	  1,	  0,	  0,  COUNTRY_UN  }, // Menu1855 key_type_chage 			
	{  1867,	  0x1D57,		FLASH_CLERK_PERMISSION+6,			0,		  1,	  1,	  0,	  0,  COUNTRY_UN  }, // Menu1855 key_inhibit				
	{  1868,	  0x1D58,		FLASH_CLERK_PERMISSION+7,			0,		  1,	  1,	  0,	  0,  COUNTRY_UN  }, // Menu1855 key_inweight				
	{  1869,	  0x1D59,		FLASH_CLERK_PERMISSION+8,			0,		  1,	  1,	  0,	  0,  COUNTRY_UN  }, // Menu1855 key_discount				
	{  1870,	  0x1D5A,		FLASH_CLERK_PERMISSION+9,			0,		  1,	  1,	  0,	  0,  COUNTRY_UN  }, // Menu1855 clerk_sales				
	
	{  1871,	  0x1D5B,		FLASH_CLERK_PERMISSION+10,			0,		  1,	  1,	  0,	  0,  COUNTRY_UN  }, // Menu1855 sale_void								
	{  1872,	  0x1D5C,		FLASH_CLERK_PERMISSION+11,			0,		  1,	  1,	  0,	  0,  COUNTRY_UN  }, // Menu1855 sale_addup 							
	{  1873,	  0x1D5D,		FLASH_CLERK_PERMISSION+12,			0,		  1,	  1,	  0,	  0,  COUNTRY_UN  }, // Menu1855 sale_return								
	{  1874,	  0x1D5E,		FLASH_CLERK_PERMISSION+13,			0,		  1,	  1,	  0,	  0,  COUNTRY_UN  }, // Menu1855 sale_override								
	{  1875,	  0x1D5F,		FLASH_CLERK_PERMISSION+14,			0,		  1,	  1,	  0,	  0,  COUNTRY_UN  }, // Menu1855 cash_open								
	{  1876,	  0x1D60,		FLASH_CLERK_PERMISSION+15,			0,		  1,	  1,	  0,	  0,  COUNTRY_UN  }, // Menu1855 edit_menu								
	{  1877,	  0x1D61,		FLASH_CLERK_PERMISSION+16,			0,		  1,	  1,	  0,	  0,  COUNTRY_UN  }, // Menu1855 edit_plu								
	{  1878,	  0x1D62,		FLASH_CLERK_PERMISSION+17,			0,		  1,	  1,	  0,	  0,  COUNTRY_UN  }, // Menu1855 edit_price 							
	{  1879,	  0x1D63,		FLASH_CLERK_PERMISSION+18,			0,		  1,	  1,	  0,	  0,  COUNTRY_UN  }, // Menu1855 edit_global								
	{  1880,	  0x1D64,		FLASH_CLERK_PERMISSION+19,			0,		  1,	  1,	  0,	  0,  COUNTRY_UN  }, // Menu1855 edit_X_report	
	{  1881,	  0x1D65,		FLASH_CLERK_PERMISSION+20,			0,		  1,	  1,	  0,	  0,  COUNTRY_UN  }, // Menu1855 edit_Z_report	

	{  1882,	  0x1D66,		FLASH_CLERK_PERMISSION+21,			0,		  1,	  1,	  0,	  0,  COUNTRY_UN  }, // Menu1855 edit_store_data						
	{  1884,	  0x1D68,		FLASH_CLERK_PERMISSION+23,			0,		  1,	  1,	  0,	  0,  COUNTRY_UN  }, // Menu1855 edit_tax_table 					
	{  1885,	  0x1D6A,		FLASH_CLERK_PERMISSION+25,			0,		  1,	  1,	  0,	  0,  COUNTRY_UN  }, // Menu1855 initialization 					
	{  1886,	  0x1D6C,		FLASH_CLERK_PERMISSION+27,			0,		  1,	  1,	  0,	  0,  COUNTRY_UN  }, // Menu1855 edit_scale_con 					
	{  1887,	  0x1D6E,		FLASH_CLERK_PERMISSION+30,			0,		  1,	  1,	  0,	  0,  COUNTRY_UN  }, // Menu1855 edit_data_table1						
	{  1888,	  0x1D6F,		FLASH_CLERK_PERMISSION+31,			0,		  1,	  1,	  0,	  0,  COUNTRY_UN  }, // Menu1855 edit_data_table2						
	{  1889,	  0x1D70,		FLASH_CLERK_PERMISSION+32,			0,		  1,	  1,	  0,	  0,  COUNTRY_UN  }, // Menu1855 print_set						
	{  1890,	  0x1D71,		FLASH_CLERK_PERMISSION+33,			0,		  1,	  1,	  0,	  0,  COUNTRY_UN  }, // Menu1855 oper_mode						
	{  1891,	  0x1D72,		FLASH_CLERK_PERMISSION+34,			0,		  1,	  1,	  0,	  0,  COUNTRY_UN  }, // Menu1855 user_parameter 					
	{  1892,	  0x1D73,		FLASH_CLERK_PERMISSION+35,			0,		  1,	  1,	  0,	  0,  COUNTRY_UN  }, // Menu1855 communication						
	{  1893,	  0x1D74,		FLASH_CLERK_PERMISSION+36,			0,		  1,	  1,	  0,	  0,  COUNTRY_UN  }, // Menu1855 key_save						
	{  1894,	  0x1D75,		FLASH_CLERK_PERMISSION+37,			0,		  1,	  1,	  0,	  0,  COUNTRY_UN  }, // Menu1855 reopen 				
	{  1895,	  0x1D76,		FLASH_CLERK_PERMISSION+38,			0,		  1,	  1,	  0,	  0,  COUNTRY_UN  }, // Menu1855 sale_negative
	{  1896,	  0x1D77,		FLASH_CLERK_PERMISSION+39,			0,		  1,	  1,	  0,	  0,  COUNTRY_UN  }, // Menu1855 key_tare 
//M1920
	{  1901,      0x3921,	    NETWORK_SERVICE_FLAG,	            FLASH_NET_AREA,	  	  1,	  4,      0,      1,      COUNTRY_UN  },
    {  1902,      0x3922,	    NETWORK_SERVICE_FLAG,	            FLASH_NET_AREA,	  	  1,	  4,      1,      1,      COUNTRY_UN  },
    {  1903,      0x3923,	    NETWORK_SERVICE_FLAG,	            FLASH_NET_AREA,	  	  1,	  4,      2,      1,      COUNTRY_UN  },
    {  1904,      0x3924,	    NETWORK_SERVICE_FLAG,	            FLASH_NET_AREA,	  	  1,	  4,      3,      1,      COUNTRY_UN  },
    {  1905,      0x3925,	    NETWORK_SERVICE_FLAG,	            FLASH_NET_AREA,	  	  1,	  4,      4,      1,      COUNTRY_UN  },
    {  1906,      0x3926,	    NETWORK_SERVICE_FLAG,	            FLASH_NET_AREA,	  	  1,	  4,      5,      1,      COUNTRY_UN  },
    {  1907,      0x3927,	    NETWORK_SERVICE_FLAG,	            FLASH_NET_AREA,	  	  1,	  4,      6,      1,      COUNTRY_UN  },
    {  1908,      0x3928,	    NETWORK_SERVICE_FLAG2,	            FLASH_NET_AREA,	  	  1,	  1,      0,      0,      COUNTRY_UN  },
//    {  1909,      0x3951,	    NETWORK_SERVICE_FLAG3,	            FLASH_NET_AREA,	  	  1,	  4,      0,      1,      COUNTRY_UN  },//M1950. 셋팅불필요
	{  1910,	  0x1a21,		NETWORK_LOCAL_IP,					FLASH_NET_AREA, 	  4,	  5,	  0,	  1,	  COUNTRY_UN  },//Menu 1913
	{  1911,	  0x1a22,		NETWORK_LOCAL_GATEWAY,				FLASH_NET_AREA, 	  4,	  5,	  0,	  1,	  COUNTRY_UN  },//Menu 1913
	{  1912,	  0x1a23,		NETWORK_LOCAL_SUBNETMASK,			FLASH_NET_AREA, 	  4,	  5,	  0,	  1,	  COUNTRY_UN  },//Menu 1913
	{  1913,	  0x1a24,		NETWORK_LOCAL_TCP_PORT, 			FLASH_NET_AREA, 	  2,	  2,	  0,	  1,	  COUNTRY_UN  },//Menu 1913
	{  1914,	  0x1a26,		NETWORK_MACHINE_REMOTE_IP,			FLASH_NET_AREA, 	  4,	  5,	  0,	  1,	  COUNTRY_UN  },//Menu 1914
	{  1915,	  0x1a24,		NETWORK_MACHINE_REMOTE_TCPPORT, 	FLASH_NET_AREA, 	  2,	  2,	  0,	  1,	  COUNTRY_UN  },//Menu 1914
//	{  1916,	  0x1a26,		FLASH_REMOTE_IP2,					0,					  4,	  5,	  0,	  1,	  COUNTRY_UN  },//Menu 1914
//	{  1917,	  0x1a24,		FLASH_REMOTE_IP2+4, 				0,					  2,	  2,	  0,	  1,	  COUNTRY_UN  },//Menu 1914

	{  1918,	  0x3f28,		GLOBAL_QR_CFG_OPEN_CMD,				FLASH_GLOBAL_AREA, 	  1,	  1,	  0,	  0,	  COUNTRY_UN  },//Menu 1264
	{  1919,	  0x3f29,		GLOBAL_QR_CFG_POS_ID,				FLASH_GLOBAL_AREA, 	  2,	  1,	  0,	  0,	  COUNTRY_UN  },//Menu 1264
	{  1920,	  0x3f2a,		GLOBAL_QR_CFG_EAN_PREFIX,			FLASH_GLOBAL_AREA, 	  2,	  1,	  0,	  0,	  COUNTRY_UN  },//Menu 1264
	{  1921,	  0x3f2b,		GLOBAL_QR_CFG_EAN_SUFFIX,			FLASH_GLOBAL_AREA, 	  2,	  1,	  0,	  0,	  COUNTRY_UN  },//Menu 1264
	{  1922,	  0x3f2c,		GLOBAL_QR_CFG_CLOSE_CMD,			FLASH_GLOBAL_AREA, 	  1,	  1,	  0,	  0,	  COUNTRY_UN  },//Menu 1264
	{  1924,	  0x3f2d,		GLOBAL_QR_CFG_DEL_LAST_SUFFIX,		FLASH_GLOBAL_AREA,	  1,	  1,	  0,	  0,	  COUNTRY_UN  },//Menu 1264
#ifdef USE_EMART_PRODUCTION_KEY
	{  1937,	  0x38A5,		GLOBAL_SALE_SETUP_FLAG19,			FLASH_GLOBAL_AREA,	  1,	  4,	  7,	  1,	  COUNTRY_EMPTY  }, //Menu1873 Use sending trace. Info:, KOR-Meat에만 caption 존재
#endif
	{  1939,	  0x38C7,		GLOBAL_SALE_SETUP_FLAG15,			FLASH_GLOBAL_AREA,	  1,	  4,	  7,	  1,	  COUNTRY_EMPTY  }, //Menu1873 Close ticket without print

#ifdef USE_INIT_MASK
	{  1940,      0x3970,       FLASH_KEEP_INIT_SETTINGS,           0,                    1,      4,      0,      1,      COUNTRY_UN     }, //Menu1891 Printer Hardware
	{  1941,      0x3971,       FLASH_KEEP_INIT_SETTINGS,           0,                    1,      4,      1,      1,      COUNTRY_UN     }, //Menu1891 Printer Hardware
	{  1942,      0x3972,       FLASH_KEEP_INIT_SETTINGS,           0,                    1,      4,      2,      1,      COUNTRY_UN     }, //Menu1891 Printer Hardware
#endif

#endif //USE_TRANSFER_MENU_SETTING
	{  0,	      0x0, 	        0,				                    0,    	    		  0,	  0,	  0,	  0,	COUNTRY_EMPTY}   //   5
};

//void	global_ad_parametermenu(INT16U tcode, INT16U icode, INT16U funcno)
//{
//	CAPTION_STRUCT 	cap;
//	POINT 		disp_p;
//	INT8U		uctemp;
//	INT16U  	result;
//	long 		ret_long;
//		
//	switch(funcno)
//	{
//	case 605: uctemp = ad_get_accumul_tare(); 
//		  break;
//	case 606: uctemp = ad_get_subtract_tare();
//		  break;
//	}
//
//	Dsp_Fill_Buffer(0);
//	menu_display_menucode(CurMenuCode, 1);
//	if (tcode != icode)
//		display_title_page_change(tcode, 1, 1);
//	else
//		parameter_title_page_change(tcode, 1, 1);
//
//	caption_split_by_code(0xF034, &cap,0);
//	//caption_adjust(&cap, NULL);
//	disp_p = point((Startup.line_max-1)*Startup.menu_ygap,0);
//	Dsp_Write_String(disp_p,(HUGEDATA char*)cap.form);
//	
//	caption_split_by_code(icode, &cap,0);
//	caption_adjust(&cap, NULL);
//	disp_p = point(Startup.menu_type*Startup.menu_ygap,0);
//	Dsp_Write_String(disp_p,(HUGEDATA char*)cap.form);
//	Dsp_Diffuse();
//
//	ret_long = uctemp;
//	if( !input_range_check(ret_long, cap.input_min, cap.input_max) ) 
//		ret_long = 0;
//
//PAGE1_KI_IN:
//	switch(_gi8u_input_type)
//	{
//	case 0:	result=keyin_ulong(KI_GRAPHIC, 
//				   KI_FORM_NORMAL,  
//				   0,
//				   (long*)&ret_long, 
//				   cap.input_dlength,
//				   Startup.menu_type*Startup.menu_ygap,  
//				   cap.input_x*Startup.menu_xgap, 
//				   0,
//				   KI_NOPASS );
//		break;
//	case 1:	result = keyin_yesno(KI_GRAPHIC, 
//				     (long*)&ret_long,
//				     Startup.menu_type*Startup.menu_ygap,
//				     cap.input_x*Startup.menu_xgap,
//				     KI_NOPASS);
//		break;
//	}
//	if (_gi8u_input_type) _gi8u_input_type = 0;
//
//	if(result==KP_ESC)
//		goto RET_ESC;
//	if( !input_range_check(ret_long, cap.input_min, cap.input_max) ) 
//	{
//		error_display_form1(0, cap.input_min, cap.input_max);
//		goto PAGE1_KI_IN;
//	} 
//	switch(result) 
//	{
//		case KP_ENTER:
//		case KP_ARROW_DN:
//		case KP_SAVE:
//			if (uctemp != (INT8U)ret_long) 
//			{
//				uctemp = (INT8U)ret_long; 
//				goto RET_END;
//			}
//			else
//			{
//				goto RET_ESC;
//			}
//	default:
//			BuzOn(2);
//			goto PAGE1_KI_IN;		
//	}
//	
//RET_END:
//	switch(funcno)
//	{
//	case 605: ad_set_accumul_tare((INT8U)uctemp);
//		  break;
//	case 606: ad_set_subtract_tare((INT8U)uctemp);
//		  break;
//	}
//
//RET_ESC:
//	Dsp_Fill_Buffer(0);						// LCD Buffer clear
//	Dsp_Diffuse();
// 	return;
//}

extern void adm_set_filter(INT8U scale_type, INT8U level);

//void ad_set_filterset(void)
//{
//	INT32U level;
//	INT8U result;
//	INT8U scale_type;
//	CAPTION_STRUCT cap;
//	char string_buf[50];
//
//	Menu_Init();
//	level = get_base_bparam(FLASH_ADC_FILTER_LEVEL);
//
//	caption_split_by_code(0x4799, &cap,0);
//	cap.form[32] = 0;
//	sprintf(string_buf, "    %s", cap.form);
//	Menu_HeadCreate(string_buf);
//
//	caption_split_by_code(0x4799, &cap,0);// Question
//	caption_adjust(&cap,NULL);
//	Menu_InputCreate(1, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
//			cap.input_max, cap.input_min, 0, &level, NULL, NULL, NULL);
//
//	result = Menu_InputDisplay();
//	if(result == MENU_RET_SAVE) {
//		scale_type = get_global_bparam(GLOBAL_AREA_SCALETYPE);
//		adm_set_filter(scale_type, (INT8U)level);
//		ADM_WaitData(ADM_CMD_SAVE_SETS, 83419, ADM_PARAM_ON);
//		set_base_bparam(FLASH_ADC_FILTER_LEVEL, level);
//	}
//}

void SaveCodeParameter(INT32U arrayid, INT32U defaultValue)
{
	INT32U value;

	INT8U	datatype, startbit, bitcount, i;
	INT16U	codemax, temp;
	INT32U	memaddr;

	value=defaultValue;

	if (dealer_factory_mode == DEALER_MODE)
		codemax = 800;
	else
#ifdef USE_TRANSFER_MENU_SETTING
		codemax = 2000;
#else
		codemax = 997;
#endif	//#ifdef USE_TRANSFER_MENU_SETTING
	if (ParameterSetting[arrayid].funcCode <= codemax)
	{
		datatype = ParameterSetting[arrayid].bitFlag;
		//capno = ParameterSetting[arrayid].captionNum;
		memaddr = DFLASH_BASE + ParameterSetting[arrayid].addrMemory + ParameterSetting[arrayid].offsetMemory;
		startbit = ParameterSetting[arrayid].startBit;
		bitcount = ParameterSetting[arrayid].countBit; 

#ifdef USE_TRANSFER_MENU_SETTING
		switch(ParameterSetting[arrayid].funcCode)
		{
			case 1774: value *= STB_TIME_DIV;	//FLASH_OFFSET_StbTimeTrimValue
				break;
			case 1777: value *= 8;	//FLASH_OFFSET_PrePrintLength
				break;
			case 1775: value = 0;	//-200, FLASH_OFFSET_GapBufInPtrTrimValue
				break;
			default:
				break;
		}
#endif	//#ifdef USE_TRANSFER_MENU_SETTING
	
		switch (datatype)
		{
//		case 0: 
//			switch(ParameterSetting[arrayid].funcCode)
//			{
//				case 605: ad_set_accumul_tare((INT8U)value);
//					break;
//				case 606: ad_set_subtract_tare((INT8U)value);
//					break;
//				case 797: ad_set_fulltare((INT8U)value); //se-hyung 20080325
//					break;
//			}
//			break;
		case 1:	Flash_bwrite(memaddr,(INT8U)value);

			break;
		case 2: Flash_wwrite(memaddr,(INT16U)value);
			break;
		case 3: //set_global_sparam(memaddr,INT8U *s,INT16S l);
			break;
		case 8:
		case 4: 
			switch(ParameterSetting[arrayid].sizeMemory)
			{
			case 1: temp = Flash_bread(memaddr);
				for (i = 0; i < bitcount; i++)
				{
					if ( ((value>>i)&0x01) )
						temp |= (0x01 << ((startbit-(bitcount-1))+i));
					else
						temp &= (~(0x01 << ((startbit-(bitcount-1))+i)));
				}
				Flash_bwrite(memaddr,(INT8U)temp);
				break;
			case 2: temp = Flash_wread(memaddr);
				for (i = 0; i < bitcount; i++)
				{
					if ( ((value>>i)&0x01) )
						temp |= (0x01 << ((startbit-(bitcount-1))+i));
					else
						temp &= (~(0x01 << ((startbit-(bitcount-1))+i)));
				}
				Flash_wwrite(memaddr,(INT16U)temp);
				break;
			}
			// ]
			break;
		case 5:
			Flash_lwrite(memaddr,value);
			break;

		}
		// setting 저장 이후 실행 항목
		switch(ParameterSetting[arrayid].funcCode)
		{
			case 797: ad_set_fulltare((INT8U)value);
				break;
			default:
				break;
		}
	}
}

void ad_set_fulltare(INT8U fulltare)	//se-hyung 20080325
{
	//INT8U temp;

	if(fulltare == 1)
	{
		ad_set_tarelimit_default(AD_FULL_TARE, 0, WEIGHT_UNIT_KG);
		ad_set_tarelimit_default(AD_FULL_TARE, 0, WEIGHT_UNIT_LB);
	}
	else
	{
		ad_set_tarelimit_default(AD_PROPER_TARE, 0, WEIGHT_UNIT_KG);
		ad_set_tarelimit_default(AD_PROPER_TARE, 0, WEIGHT_UNIT_LB);
	}

// 이미 저장되어 있음
//	temp = get_base_bparam(FLASH_ADC_USE_FULLTARE);
//	temp = temp&(~0x01);
//        temp |= fulltare&0x01;
//	set_base_bparam(FLASH_ADC_USE_FULLTARE,temp);
}

//void ad_fulltare(void)	//se-hyung 20080325
//{
//	INT32U settare;
//	INT32U temp;
//	INT8U result;
//	char string_buf[50];
//
//	Menu_Init();
//	temp = get_base_bparam(FLASH_ADC_USE_FULLTARE);
//	settare = temp&0x01;
//	sprintf(string_buf, "   USE FULLTARE");
//	Menu_HeadCreate(string_buf);
//
//	sprintf(string_buf, "USE FULLTARE :[ ]");
//	Menu_InputCreate(1, string_buf, MENU_TYPE_YN, 15, 1, 1, 
//					1, 0, 0, (HUGEDATA INT32U *)&settare, NULL, NULL, NULL);
//
//	result = Menu_InputDisplay();
//
//	if(result == MENU_RET_SAVE) {
//		ad_set_fulltare(settare);
//		
//		_SOFTWARE_RESET;
//
//	}
//}
		
// <FUNC> JHC, 040422, -Fresh one line setting
// [
// $Return	: void
// $Argument	:
// INT16U fc		- function code number, 
// INT16U tcode 	- title caption no, 
// INT16U icode 	- input caption no, 
// INT8U datatype	- data type, 
// INT32U addr		- address, 
// INT8U size		- data size, 
// INT8U startbit 	- start bit, 
// INT8U bitcount 	- bit count
// ]		
INT8U _flash_set_oneline_data(INT16U fc, 
			     INT16U tcode, 
			     INT16U icode, 
			     INT8U datatype, 
			     INT32U addr, 
			     INT8U size, 
			     INT8U startbit, 
			     INT8U bitcount,
			     INT32U vdefault,
			     INT8U strno,
			     INT8U ynflag)
{
	CAPTION_STRUCT	cap;
	POINT		disp_p;	
	char    	sdata[32];
	INT8U		readdata[64];
	INT8U		i8u, t8u, i;
	INT16U		i16u, t16u;
	INT16U		result;
	//INT16U		temp16;
	long		ret_long;
	INT8U		changed = 0;
	long 		prev_data;

	Dsp_Fill_Buffer(0);
	menu_display_menucode(CurMenuCode, 1);
#if defined(USE_SINGLE_LINE_GRAPHIC)
	sprintf(cap.form, "FN %03d", fc);
	display_string(DISPLAY_UNITPRICE, (INT8U*)cap.form);
#else
	if (tcode != icode)
		display_title_page_change(tcode,1,1);
	else
		parameter_title_page_change(tcode,1,1);
#endif

	if (ynflag)
	{
		caption_split_by_code(0xF034, &cap,0);
		disp_p = point((Startup.menu_type+1)*Startup.menu_ygap,0);
#ifdef HEBREW_FONT
		Convert_HebrewPreview((INT8U *)cap.form, strlen(cap.form), 0, 0); 
		hebrew_codetemp[strlen(cap.form)] = 0;
		disp_p.x = 202 - disp_p.x;
		Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
		Convert_ArabPreview((INT8U *)cap.form, strlen(cap.form), 0, 0, 0); 
		arab_codetemp[strlen(cap.form)] = 0;
		disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
		Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
		Dsp_Write_String(disp_p,(HUGEDATA char*)cap.form);
#endif
	}
	
	caption_split_by_code(icode, &cap,0);

	if (strno > 0)
	{
		sprintf(sdata, "%2d :", strno);
		strcat(cap.form, sdata);
	}
#ifdef USE_PERSIA_DEFAULT
	caption_adjust(&cap,NULL);
#else
	if (!strchr((char *)cap.form,'[')) caption_adjust(&cap,NULL);
#endif
	Flash_sread(addr,
 	 	    (HUGEDATA INT8U*) readdata,
 	 	    size);

	switch (datatype)
	{
	case 1:	// Byte
		i8u = readdata[0];
		ret_long = i8u;
		break;
	case 2: // Word
		i16u = readdata[1];
		i16u <<= 8;
		i16u |= readdata[0];
		ret_long = i16u;
		break;
	case 3: // String
		strcpy(sdata, (char*)readdata);
		break;
	case 8:
	case 4: // Bit value
		ret_long = 0;
		switch(size)
		{
		case 1: // 1byte
			i8u = readdata[0];
			t8u = i8u;
			//t8u >>= (startbit-(1-bitcount));
			t8u >>= (startbit-(bitcount-1));
			for (i = 0; i < bitcount; i++)
			{
				if ((t8u >> i) & 0x1)
					ret_long |= (0x1 << i);
				else
					ret_long &= (~(0x1 << i));
			}
			break;
		case 2: // 2byte
			i16u = readdata[1];
			i16u <<= 8;
			i16u |= readdata[0];
			t16u = i16u;
			//t16u >>= (startbit-(1-bitcount));
			t16u >>= (startbit-(bitcount-1));
			for (i = 0; i < bitcount; i++)
			{
				if ((t16u >> i) & 0x1)
					ret_long |= (0x1 << i);
				else
					ret_long &= (~(0x1 << i));
			}
			break;
		}
		break;
	case 5: // long
		break;
	}
	prev_data = ret_long;

	if( !input_range_check(ret_long, cap.input_min, cap.input_max) ) 
		ret_long = vdefault;

	disp_p = point((Startup.menu_type)*Startup.menu_ygap,0);
#ifdef HEBREW_FONT
	Convert_HebrewPreview((INT8U *)cap.form, strlen(cap.form), 0, 0); 
	hebrew_codetemp[strlen(cap.form)] = 0;
	disp_p.x = 202 - disp_p.x;
	Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
	Convert_ArabPreview((INT8U *)cap.form, strlen(cap.form), 0, 0, 0);
	arab_codetemp[strlen(cap.form)] = 0;
	disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
	Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
	Dsp_Write_String(disp_p,(HUGEDATA char*)cap.form);
#endif
	
	Dsp_Diffuse();
	
//	temp16 = fc;
KI_IN:
	if (datatype!=3) {
		switch(ynflag)
		{
			default:
#if defined(USE_ARAB_CONVERT) || defined(HEBREW_FONT)
				result = keyin_ulong(KI_GRAPHIC, 
					   KI_FORM_ILNN,	
					   0,
					   (long*)&ret_long, 
					   cap.input_dlength, 
					   (Startup.menu_type)*Startup.menu_ygap,  
					   cap.input_x*Startup.menu_xgap, 
					   0,
					   KI_NOPASS );
#else
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
				result = keyin_ulong(KI_GRAPHIC,
					   KI_FORM_NORMAL,	
					   0,
					   (long*)&ret_long, 
					   cap.input_dlength, 
					   (Startup.menu_type)*Startup.menu_ygap,  
					   cap.input_x*Startup.menu_xgap, 
					   0,
					   KI_NOPASS );
#else
				result = keyin_ulong(KI_DEFAULT,
					   KI_FORM_NORMAL,	
					   0,
					   (long*)&ret_long, 
					   cap.input_dlength, 
					   (Startup.menu_type)*Startup.menu_ygap,  
					   cap.input_x*Startup.menu_xgap, 
					   0,
					   KI_NOPASS );
#endif
#endif
			break;
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
		case 1: result = keyin_yesno(KI_GRAPHIC,
					     (long*)&ret_long,
					     (Startup.menu_type)*Startup.menu_ygap,  
					     cap.input_x*Startup.menu_xgap, 
					     KI_NOPASS);
#else
		case 1: result = keyin_yesno(KI_DEFAULT,
					     (long*)&ret_long,
					     (Startup.menu_type)*Startup.menu_ygap,  
					     cap.input_x*Startup.menu_xgap, 
					     KI_NOPASS);
#endif
			break;
		}
		// ]

		if(result == KP_ESC)
			goto RET_ESC;
		if ( !input_range_check(ret_long, cap.input_min, cap.input_max) ) 
		{
				error_display_form1(ret_long, cap.input_min, cap.input_max);
				goto KI_IN;	
		} 
	}
	else 
	{
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
		result=keyin_string(KI_FREE_MODE, 
				    KI_GRAPHIC,
				    NULL, 
				    (HUGEDATA INT8U *)sdata, 
				    cap.input_length, 
				    cap.input_dlength,
				    (Startup.menu_type)*Startup.menu_ygap,
				    cap.input_x*Startup.menu_xgap, 
				    0, 
				    0, 
				    KI_NOPASS);
#else
		result=keyin_string(KI_FREE_MODE, 
				    KI_DEFAULT,
				    NULL, 
				    (HUGEDATA INT8U *)sdata, 
				    cap.input_length, 
				    cap.input_dlength,
				    (Startup.menu_type)*Startup.menu_ygap,
				    cap.input_x*Startup.menu_xgap, 
				    0, 
				    0, 
				    KI_NOPASS);
#endif
		if(result == KP_ESC)
			goto RET_ESC;
	}
	//keyin_clearcallback();

	switch (result) {
		case KP_SAVE:
		case KP_ENTER:		
		case KP_ARROW_DN:
			changed |= 0x01;
			goto RET_END;
	     	default:
			BuzOn(2);
			goto KI_IN;		     	 	
	} //switch (result)

RET_END:
	if (datatype != 3 && prev_data != ret_long) changed |= 0x02;
	switch (datatype) {
	case 1: // byte
		i8u = (INT8U)ret_long;
		Flash_swrite(addr, 
			     (HUGEDATA INT8U *)&i8u, 
			     size);
		break;
	case 2: // word
		i16u = (INT16U) ret_long;
		Flash_swrite(addr, 
			     (HUGEDATA INT8U *)&i16u, 
			     size);
		break;
	case 3: // string
		Flash_swrite(addr, 
			     (HUGEDATA INT8U *)sdata, 
			     size);
		if (strncmp(sdata, (char *)readdata,size)) changed |= 0x02;
		break;
	case 8:
	case 4: // bit data
		t8u = 0;
		t16u = 0;
		switch(size)
		{
		case 1: // 1byte
			t8u = (INT8U) ret_long;
			for (i = 0; i < bitcount; i++)
			{
				if ( ((t8u>>i)&0x01) )
					i8u |= (0x01 << ((startbit-(bitcount-1))+i));
				else
					i8u &= (~(0x01 << ((startbit-(bitcount-1))+i)));
			}
			Flash_swrite(addr, 
			     	     (HUGEDATA INT8U *)&i8u, 
			     	     size);
			break;
		case 2: // 2byte
			t16u = (INT16U) ret_long;
			for (i = 0; i < bitcount; i++)
			{
				if ( ((t16u>>i)&0x01) )
					i16u |= (0x01 << ((startbit-(bitcount-1))+i));
				else
					i16u &= (~(0x01 << ((startbit-(bitcount-1))+i)));
			}
			Flash_swrite(addr, 
			     	     (HUGEDATA INT8U *)&i16u, 
			     	     size);
			break;
		}
		break;
	case 5: // long
		break;
	}
RET_ESC:
	Dsp_Fill_Buffer(0);						// LCD Buffer clear
	Dsp_Diffuse(); 
	return changed;
	
}
#ifndef USE_TRANSFER_MENU_SETTING
void DefaultUserParameterSetting(INT8U country)
{
	INT8U	offset8u, datatype, startbit, bitcount, i, j;
	INT16U	offset16u, fcode, temp, value;
	INT32U	memaddr;

	i = 0;
	while(ParameterUserSetting[i].funcCode != 0)
	{
		if (ParameterUserSetting[i].country == COUNTRY_UN)
			value = (INT16U)ParameterUserSetting[i].captionNum;
		if (ParameterUserSetting[i].country == country)
			value = (INT16U)ParameterUserSetting[i].captionNum;

		if (ParameterUserSetting[i].funcCode != ParameterUserSetting[i+1].funcCode)
		{
			datatype = ParameterUserSetting[i].bitFlag;
			memaddr = ParameterUserSetting[i].addrMemory;
			startbit = ParameterUserSetting[i].startBit;
			bitcount = ParameterUserSetting[i].countBit;
			fcode = ParameterUserSetting[i].funcCode;

			if (fcode >= 250 && fcode <= 273)
			{
				switch(fcode)
				{
				case 252: value *= STB_TIME_DIV;//HYP 20050216
					  break;
				case 255:
				case 256:
				case 257:
				case 261:
				case 262:
				case 265:
				case 271: value *= 8;
					  break;
				case 272: value = 0;//-200 HYP 20040811
					  break;
				}
				switch(ParameterUserSetting[i].sizeMemory)
				{
				case 1:	offset8u = (INT8U) value;
					Flash_swrite(PRT_FLASH_BASE + memaddr, 
						     (HUGEDATA INT8U *)&offset8u, 
						     ParameterUserSetting[i].sizeMemory);
					break;
				case 2:	offset16u = (INT16U) value;
					Flash_swrite(PRT_FLASH_BASE + memaddr, 
						     (HUGEDATA INT8U *)&offset16u, 
						     ParameterUserSetting[i].sizeMemory);
					break;
				}
			}
			else if ((fcode >= 140 && fcode <= 142) ||
				 (fcode >= 151 && fcode <= 153))
			{
				switch (datatype)
				{
				case 1:	set_net_bparam(memaddr,(INT8U)value);
					break;
				case 2: set_net_wparam(memaddr,(INT16U)value);
					break;
				case 3: //set_net_sparam(memaddr,INT8U *s,INT16S l);
					break;
				case 8:
				case 4: switch(ParameterUserSetting[i].sizeMemory)
					{
					case 1: temp = get_net_bparam(memaddr);
						for (j = 0; j < bitcount; j++)
						{
							if ( ((value>>j)&0x01) )
								temp |= (0x01 << ((startbit-(bitcount-1))+j));
							else
								temp &= (~(0x01 << ((startbit-(bitcount-1))+j)));
						}
						set_net_bparam(memaddr,(INT8U)temp);
						break;
					case 2: temp = get_net_wparam(memaddr);
						for (j = 0; j < bitcount; j++)
						{
							if ( ((value>>j)&0x01) )
								temp |= (0x01 << ((startbit-(bitcount-1))+j));
							else
								temp &= (~(0x01 << ((startbit-(bitcount-1))+j)));
						}
						set_net_wparam(memaddr,(INT16U)temp);
						break;
					}
					break;
				case 5: //set_net_lparam(memaddr, temp);
					break;

				}
			}
			else
			{	
				switch (datatype)
				{
				case 1:	set_global_bparam(memaddr,(INT8U)value);
					break;
				case 2: set_global_wparam(memaddr,(INT16U)value);
					break;
				case 3: //set_global_sparam(memaddr,INT8U *s,INT16S l);
					break;
				case 8:
				case 4: switch(ParameterUserSetting[i].sizeMemory)
					{
					case 1: temp = get_global_bparam(memaddr);
						for (j = 0; j < bitcount; j++)
						{
							if ( ((value>>j)&0x01) )
								temp |= (0x01 << ((startbit-(bitcount-1))+j));
							else
								temp &= (~(0x01 << ((startbit-(bitcount-1))+j)));
						}
						set_global_bparam(memaddr,(INT8U)temp);
						break;
					case 2: temp = get_global_wparam(memaddr);
						for (j = 0; j < bitcount; j++)
						{
							if ( ((value>>j)&0x01) )
								temp |= (0x01 << ((startbit-(bitcount-1))+j));
							else
								temp &= (~(0x01 << ((startbit-(bitcount-1))+j)));
						}
						set_global_wparam(memaddr,(INT16U)temp);
						break;
					}
					break;
				case 5: //set_global_lparam(memaddr, temp);
					break;

				}  

			} 
		}
		i++;
	}

	Flash_sread(PRT_FLASH_BASE, (HUGEDATA INT8U *)&PrtFlashStruct, PRT_FLASH_SET_STRUCT_SIZE);
}
#endif	//#ifndef USE_TRANSFER_MENU_SETTING

void DefaultDealerParamSetting(INT8U selCountry, INT8U mode)
{
	INT32U	defaultValue=0l;
	INT16S  arrayid;
	INT16U	i;
	
	VertScrollMsg_Add("2.2 Set Private Code");
	
	for(i=0;ParameterCountry[i].funcCode;i++)
	{
#ifdef USE_POLAND_DEFAULT
		if(mode == DEALER_MODE && (800<ParameterCountry[i].funcCode || ParameterCountry[i].funcCode == 995))// factory
#else
		if(mode == DEALER_MODE && 800<ParameterCountry[i].funcCode)// factory
#endif
			break;

		if(ParameterCountry[i].country == COUNTRY_UN)
			defaultValue = (INT32U)ParameterCountry[i].defaultValue;
		if(ParameterCountry[i].country == selCountry)
			defaultValue = (INT32U)ParameterCountry[i].defaultValue;

		// 다음 code가 틀리면 현재의 정보를 저장
		if(ParameterCountry[i].funcCode != ParameterCountry[i+1].funcCode)
		{
			arrayid = CodeSearchParameter(ParameterCountry[i].funcCode);
			if(arrayid >= 0)
			{
				SaveCodeParameter((INT32U)arrayid, defaultValue);
			}
		}
	}
}

void CodeDefaultParameter(INT8U selCountry,INT8U	mode)// mode -> CODE_FACTORY, CODE_DEALER
{
	// user parameter setting
	
#ifndef USE_TRANSFER_MENU_SETTING
	DefaultUserParameterSetting(selCountry);
#endif	//#ifndef USE_TRANSFER_MENU_SETTING
	

	// setting
	DefaultDealerParamSetting(selCountry, mode);
}


void CodeDefaultParameterByCode(INT16U funcCode)
{
	INT32U	defaultValue=0l;
	INT16S  arrayid;
	INT16U	i;
	INT8U	selCountry;

	selCountry = get_global_wparam(GLOBAL_AREA_VENDOR_COUNTRY);
	for (i = 0; ParameterCountry[i].funcCode; i++)
	{
		if (funcCode != ParameterCountry[i].funcCode) continue;
		
		if (ParameterCountry[i].country == COUNTRY_UN)
			defaultValue = (INT32U)ParameterCountry[i].defaultValue;
		if (ParameterCountry[i].country == selCountry)
			defaultValue = (INT32U)ParameterCountry[i].defaultValue;

		// 다음 code가 틀리면 현재의 정보를 저장
		if (ParameterCountry[i].funcCode != ParameterCountry[i+1].funcCode)
		{
			arrayid = CodeSearchParameter(ParameterCountry[i].funcCode);
			if (arrayid >= 0)
			{
				SaveCodeParameter((INT32U)arrayid, defaultValue);
			}
		}
	}
}
								    					    
INT16S CodeSearchParameter(INT16U funcCode)								    
{													    
	INT16S	i = 0,
		searchFlag=-1;									    

	while (ParameterSetting[i].funcCode > 0)
	{
		if (dealer_factory_mode == DEALER_MODE)
		{
#ifdef USE_POLAND_DEFAULT
			if (ParameterSetting[i].funcCode < 801 || 
			    ParameterSetting[i].funcCode == 998 || ParameterSetting[i].funcCode == 995)
#elif defined USE_CHINA_DEFAULT
			if (ParameterSetting[i].funcCode < 801 || 
			    ParameterSetting[i].funcCode == 999)
#else
			if (ParameterSetting[i].funcCode < 801 ||
			    ParameterSetting[i].funcCode == 998)
#endif
			{
				if(funcCode==ParameterSetting[i].funcCode)
				{
					searchFlag=i;
					break;
				}
			}
#ifdef USE_GBR_HMART
			if ((ParameterSetting[i].funcCode < 801) ||
			    (ParameterSetting[i].funcCode == 998) ||		
			    (ParameterSetting[i].funcCode == 980))		//Exception, Use Korean in GBR_HMART
			{
				if(funcCode==ParameterSetting[i].funcCode)
				{
					searchFlag=i;
					break;
				}
			}
#endif
		}
		else
		{
			if(funcCode==ParameterSetting[i].funcCode)
			{
				searchFlag=i;
				break;
			}
		}
		i++;
	}
		
	return searchFlag;
}  

INT16S CodeSearchParameterModeMax(INT8U mode)								    
{													    
	INT16S	i = 0,
		searchFlag=-1;									    

	while (ParameterSetting[i].funcCode > 0)
	{
		if (mode == DEALER_MODE)
		{
			if(ParameterSetting[i+1].funcCode > 800)
			{
				searchFlag=i;
				break;
			}
		}
		else
		{
			if(ParameterSetting[i+1].funcCode == 0)
			{
				searchFlag=i;
				break;
			}
		}
		i++;
	}
		
	return searchFlag;
}

#define SCRL_BEGIN_TIME 6
#define SCRL_END_TIME 4
INT16S ParamScroll_Xpt;
INT16U ParamScroll_PrevTime;
void callback_parameter_mode(long num)
{
	CAPTION_STRUCT 		cap;
	POINT 			disp_p;
	INT16S 			arrayid, strno;
	char buffer[36];
	char *sp;
	INT16U scrollLen;
	INT16S x_offset;
	static long prev_num;
	char w;

#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
	disp_p = point((Startup.menu_type+1)*Startup.menu_ygap, 0);
	memset((INT8U *)buffer,0x20, 34);
	buffer[34] = 0;
	DspStruct.DirectDraw = 1;
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
	Dsp_Write_String(disp_p,(HUGEDATA char *)buffer);
#endif
#else
	if (num != prev_num)
	{
		display_lcd_clear();
		ParamScroll_PrevTime = SysTimer10ms;
		ParamScroll_Xpt = 0;
	}
	prev_num = num;

	if ((INT16U)(SysTimer10ms - ParamScroll_PrevTime) < 20) return;
	ParamScroll_PrevTime = SysTimer10ms;
#endif
#ifdef USE_ARAB_FONT   //20120914 for CL5200 Arab version by kiho
	disp_p = point((Startup.menu_type+1)*Startup.menu_ygap, 0);
#else
	disp_p = point((Startup.menu_type+1)*Startup.menu_ygap, 3*Startup.menu_xgap);
#endif

	caption_split_by_code(0x200F, &cap,0);
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
	caption_adjust(&cap,NULL);
	cap.form[32]=0;
#endif
	if ((num<cap.input_min) || (num>cap.input_max)) {
		caption_split_by_code(0xE00C, &cap,0);
		DspStruct.DirectDraw = 1;
		cap.form[32]=0;
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
#ifdef HEBREW_FONT
		Convert_HebrewPreview((INT8U *)cap.form, strlen(cap.form), 0, 0);
		hebrew_codetemp[strlen(cap.form)] = 0;
		disp_p.x = 202 - disp_p.x;
		Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
		Convert_ArabPreview((INT8U *)cap.form, strlen(cap.form), 0, 0, 0);
		arab_codetemp[strlen(cap.form)] = 0;
		disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
		Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
		Dsp_Write_String(disp_p,(HUGEDATA char *)cap.form);
#endif
#else
		sprintf(buffer, "%s", cap.form);
#endif
	} 
	else 
	{
		arrayid = CodeSearchParameter((INT16U)num);
		if (arrayid > -1)
			caption_split_by_code((INT16U)ParameterSetting[arrayid].captionNum, &cap,0);
		else
			caption_split_by_code((INT16U)0xE00B, &cap,0);
			
		DspStruct.DirectDraw = 1;
		cap.form[31]=0;
		if (cap.input_x > 2) {
			sp=strchr((char *)cap.form,':');
			if (sp) {
				strno=(INT16S)(sp-(char *)cap.form);
				cap.form[strno]=0;
			}
		}
		if (num >= 922 && num <= 940)
		{
			memset((INT8U *)buffer,0x20,32);
			buffer[32] = 0;
			strno = num-921;
			if (strno > 12)
				strno -= 12;
			sprintf(buffer, "%s  %d", cap.form, strno);
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
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
			Dsp_Write_String(disp_p,(HUGEDATA char *)buffer);
#endif
#endif
		}
//		else if (num == 797)	// se-hyung 20080325		
//		{
//			sprintf(buffer, "USE FULL TARE");
//			Dsp_Write_String(disp_p,(HUGEDATA char *)buffer);
//		}
		else
		{
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
#ifdef HEBREW_FONT
			Convert_HebrewPreview((INT8U *)cap.form, strlen(cap.form), 0, 0);
			hebrew_codetemp[strlen(cap.form)] = 0;
			disp_p.x = 202 - disp_p.x;
			Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
			Convert_ArabPreview((INT8U *)cap.form, strlen(cap.form), 0, 0, 0);
			arab_codetemp[strlen(cap.form)] = 0;
			disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
			Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
			Dsp_Write_String(disp_p,(HUGEDATA char *)cap.form);
#endif
#else
			sprintf(buffer, "%s", cap.form);
#endif
		}
	}
#if !defined(_USE_LCD20848_) && !defined(_USE_LCD32_)
	Dsp_Fill_Buffer(0x00);
	scrollLen = strlen(buffer);
	if (scrollLen < DSP_MAX_CHAR) scrollLen = 0;
	else scrollLen -= DSP_MAX_CHAR; // scroll할 길이
	
	if (ParamScroll_Xpt < SCRL_BEGIN_TIME) x_offset = 0;
	else if (ParamScroll_Xpt < SCRL_BEGIN_TIME + scrollLen) x_offset = ParamScroll_Xpt - SCRL_BEGIN_TIME;
	else if (ParamScroll_Xpt < SCRL_BEGIN_TIME + scrollLen + SCRL_END_TIME) x_offset = scrollLen;
	else x_offset = 0;
#ifdef HEBREW_FONT
	Convert_HebrewPreview((INT8U *)buffer+x_offset, strlen(buffer+x_offset), 0, 0); 
	hebrew_codetemp[strlen(buffer+x_offset)] = 0;
	disp_p.x = 202 - disp_p.x;
	Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
	Convert_ArabPreview((INT8U *)buffer+x_offset, strlen(buffer+x_offset), 0, 0, 0); 
	arab_codetemp[strlen(buffer+x_offset)] = 0;
	disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
	Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
	w = display_font_get_width();
	disp_p = point(0, DSP_SCROLL_MSG_X - x_offset * w);
	Dsp_Write_String(disp_p,(HUGEDATA char *)buffer);
#endif
	ParamScroll_Xpt++;
	if (ParamScroll_Xpt > SCRL_BEGIN_TIME + scrollLen + SCRL_END_TIME) ParamScroll_Xpt = 0;
#endif
}

void callback_default_mode(long num)
{
	CAPTION_STRUCT 		cap;
	POINT 			disp_p;
	INT16S xmin,xmax;
	char buffer[34];

	disp_p = point((Startup.menu_type+1)*Startup.menu_ygap, 0);
	memset((INT8U *)buffer,0x20, 32);
	buffer[32] = 0;

	DspStruct.DirectDraw = 1;
	Dsp_Write_String(disp_p,(HUGEDATA char *)buffer);

	disp_p = point((Startup.menu_type+1)*Startup.menu_ygap, 3*Startup.menu_xgap);
	caption_split_by_code((INT16U)0x4999, &cap,0);
	xmin = cap.input_min;
	xmax = cap.input_max;
	if (num >= xmin && num <= xmax) {
		strcpy(cap.form,global_country_code[num]);
	} else {
		strcpy(cap.form,global_message[MSG_INVALID_VALUE]);
	}								    
	DspStruct.DirectDraw = 1;
	Dsp_Write_String(disp_p,(HUGEDATA char *)cap.form);
}


INT8U TypeSymbol[] = {'B', 'P', 'H', 'S', 'D', 'B'};
void pgm_parameter_function(INT16U fcode)
{
	INT8U 	startbit,
		bitct;
	INT8U	v8,
		v8_1;
	long 	ret_long;
	INT16U  result,
		v16,
		capno,
		codemax;
	INT32U	memaddr;
	CAPTION_STRUCT 	cap;
	POINT 	disp_p;
	// 폰트의 x,y 크기
	INT8U	strno=0;
	INT8U	datatype,yn;
	INT16S 	arrayid,fc;
	char    old_font;
	INT8U	language_flag;
	INT8U   changed = 0;
//	FW_VERSION  fversion;
//	FW_VERSION2 fversion2;
//	BOOT_STRUCT boot_struct;
//	INT8U keytype;
#ifdef USE_CHECKING_HARMFUL_MEAT
	INT32U	flashAddr;
#endif

	ret_long = 501;
	old_font = DspStruct.Id1;
	DspLoadFont1(DSP_MENU_FONT_ID);

	if (dealer_factory_mode == DEALER_MODE)
		codemax = 800;
	else
		codemax = 999;
PAGE_VIEW:
	Dsp_Fill_Buffer(0);
	menu_display_menucode(CurMenuCode, 1);
	
	display_title_page_change(0x8600, 1, 1);

	caption_split_by_code(0x200F, &cap,0);
	caption_adjust(&cap,NULL);
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
	disp_p = point(Startup.menu_type*Startup.menu_ygap, 0);
#ifdef HEBREW_FONT
	Convert_HebrewPreview((INT8U *)cap.form, strlen(cap.form), 0, 0);
	hebrew_codetemp[strlen(cap.form)] = 0;
	disp_p.x = 202 - disp_p.x;
	Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
	Convert_ArabPreview((INT8U *)cap.form, strlen(cap.form), 0, 0, 0);
	arab_codetemp[strlen(cap.form)] = 0;
	disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
	Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
	Dsp_Write_String(disp_p,(HUGEDATA char*)cap.form);
#endif
#else
	display_string(DISPLAY_UNITPRICE, (INT8U*)cap.form);
#endif
#if defined(USE_ARAB_CONVERT) || defined(HEBREW_FONT)
		result=keyin_ulong(KI_GRAPHIC, 
		   KI_FORM_ILNN,  
		   0,
		   (long*)&ret_long, 
		   cap.input_dlength,
		   Startup.menu_type*Startup.menu_ygap,  
		   cap.input_x*Startup.menu_xgap, 
		   0,
		   KI_PASS );
#else
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
	result=keyin_ulong(KI_GRAPHIC,
			   KI_FORM_NORMAL,  
			   0,
			   (long*)&ret_long, 
			   cap.input_dlength,
			   Startup.menu_type*Startup.menu_ygap,  
			   cap.input_x*Startup.menu_xgap, 
			   0,
			   KI_PASS );
#else
	result=keyin_ulong(KI_DEFAULT,
			   KI_FORM_NORMAL,  
			   0,
			   (long*)&ret_long, 
			   cap.input_dlength,
			   Startup.menu_type*Startup.menu_ygap,  
			   cap.input_x*Startup.menu_xgap, 
			   0,
			   KI_PASS );
#endif
#endif
	Dsp_Diffuse();
KI_IN:
	// wait key input
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
	keyin_setcallback(callback_parameter_mode);
#else
	keyin_setanycheck(callback_parameter_mode); // scroll the title
#endif
#if defined(USE_ARAB_CONVERT) || defined(HEBREW_FONT)
	result=keyin_ulong(KI_GRAPHIC, 
			   KI_FORM_ILNN,  
			   0,
			   (long*)&ret_long, 
			   cap.input_dlength,
			   Startup.menu_type*Startup.menu_ygap,  
			   cap.input_x*Startup.menu_xgap, 
			   0,
			   KI_NOPASS );
#else
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
	result=keyin_ulong(KI_GRAPHIC,
			   KI_FORM_NORMAL,  
			   0,
			   (long*)&ret_long, 
			   cap.input_dlength,
			   Startup.menu_type*Startup.menu_ygap,  
			   cap.input_x*Startup.menu_xgap, 
			   0,
			   KI_NOPASS );
#else
	result=keyin_ulong(KI_DEFAULT,
			   KI_FORM_NORMAL,  
			   0,
			   (long*)&ret_long, 
			   cap.input_dlength,
			   Startup.menu_type*Startup.menu_ygap,  
			   cap.input_x*Startup.menu_xgap, 
			   0,
			   KI_NOPASS );
#endif
#endif
	keyin_clearcallback();
			   
	if(ret_long < cap.input_min || ret_long > cap.input_max) {
		if (result != KP_ESC){
			error_display_form1(0, cap.input_min, cap.input_max);
			goto KI_IN;
		}
	}
	else
	{
		arrayid = CodeSearchParameter((INT16U)ret_long);
		if (arrayid < 0 && 
		    result != KP_ARROW_LF && 
		    result != KP_ARROW_RI &&
		    result != KP_ESC)
		    {
			goto KI_IN;
		}
		else
		{
			while(arrayid < 0)
			{
				if (ret_long > 0)
					arrayid = CodeSearchParameter((INT16U)(--ret_long));
				else
					arrayid = 0;
			}
		}
	}
				 		
	switch(result) {
		case KP_ESC: goto RET_ESC;
		case KP_ARROW_RI: 
			if (ParameterSetting[arrayid+1].funcCode <= codemax &&
			    ParameterSetting[arrayid+1].funcCode != 0)
				arrayid++;
			else
				arrayid = 0;
			ret_long = ParameterSetting[arrayid].funcCode;
			goto PAGE_VIEW; 
		case KP_ARROW_LF: 
			if (arrayid > 0)
				arrayid--;
			else
				arrayid = CodeSearchParameterModeMax(dealer_factory_mode);
			ret_long = ParameterSetting[arrayid].funcCode;
			goto PAGE_VIEW;
		case KP_TEST:
			parameter_print_list(dealer_factory_mode);
			goto PAGE_VIEW;	
		case KP_FEED:
			keyapp_feed(0);
			goto PAGE_VIEW;
		case KP_ENTER:
		case KP_ARROW_DN:
		case KP_PAGE_DN:
		case KP_SAVE:
			changed = 0;
			if (arrayid > -1 && arrayid < 1000)
			{
				datatype = ParameterSetting[arrayid].bitFlag;
				capno = ParameterSetting[arrayid].captionNum;
				memaddr = ParameterSetting[arrayid].addrMemory; 
				startbit = ParameterSetting[arrayid].startBit;
				bitct = ParameterSetting[arrayid].countBit;
				fc = ParameterSetting[arrayid].funcCode;

				//_gi8u_input_type = ParameterSetting[arrayid].countBit;

				if( (Startup.country == COUNTRY_US)&&(ret_long==502) )
				{
					ret_long ++;
					break;
				}	      				

//				if (ret_long >= 605 && ret_long <= 606)
//				{
//					global_ad_parametermenu(capno,
//								capno,
//								(INT16U)ret_long);
//				}
//				else 
#ifdef USE_CHINA_DEFAULT
				if (ret_long == 999)
				{
					restore_factory_default(999);
				}
#else
				if (ret_long == 998) 
				{
					restore_factory_default(998);
				} 
#endif
				else if (ret_long == 997) 
				{
					display_style();
				}
				else if (ret_long == 550)
				{
					prt_set_outofpaper();
				}
#ifdef NOT_USE_TRACE_CL5200
				else if (ret_long == 782)	//CL5200 은 국내 축산 항목 사용 안함.
				{
					BuzOn(2);
					break;
				}
#endif
//				else if (ret_long == 799)
//				{
//					ad_set_filterset();
//				}
//				else if (ret_long == 797)      //se-hyung 20080325
//				{
//					ad_fulltare();
//				}
				else
				{
					strno = 0;
					if (fc >= 922 && fc <= 940)
					{
						strno = ParameterSetting[arrayid].funcCode-921;
						if (strno > 12)
							strno -= 12;
					}
					yn = 0;
					if ((datatype == 4) || (datatype == 8))
					{
						if (bitct == 1) yn = 1;	// 1bit 셋팅은 "yes/no 사용"
						if (datatype == 8) yn=0;	// 8:yes/no 사용안함
					}
					
					memaddr += DFLASH_BASE;
					memaddr += ParameterSetting[arrayid].offsetMemory;
					changed = _flash_set_oneline_data(fc, 
							        capno, 
							        capno, 
							        datatype, 
							        memaddr, 
							        ParameterSetting[arrayid].sizeMemory, 
							        startbit, 
							        bitct,
							        0,
								strno,
								yn);

				}
			}
			switch (ret_long) {
				case 999:
					  if (changed)	// changed & saved
					  {
						v16=get_global_wparam(GLOBAL_AREA_VENDOR_COUNTRY);
#ifdef USE_CHINA_DEFAULT
                        global_set_country(v16, ON);
#else
						global_set_country(v16, OFF);
#endif
						_SOFTWARE_RESET;
					  }
					  break;
				case 995:
					  if (changed)	// changed & saved
					  {
						_SOFTWARE_RESET;
					  }
					  break;
				case 797:
					  if (changed)	// changed & saved
					  {
						v8 = get_base_bparam(FLASH_ADC_USE_FULLTARE);
						v8 = v8&0x01;
						ad_set_fulltare(v8);
						_SOFTWARE_RESET;
					  }
				case 799:
					if (changed)	// changed & saved
					{
						v8 = get_base_bparam(FLASH_ADC_FILTER_LEVEL);
						v8_1 = get_global_bparam(GLOBAL_AREA_SCALETYPE);	// scale_type
						adm_set_filter(v8_1, v8);
						ADM_WaitData(ADM_CMD_SAVE_SETS, 83419, ADM_PARAM_ON);
					}
					break;
//				case 550:
//					SensorStruct.GapOutOfPaperThreshold = (INT8U)set_val;
//					  break;
				case 750:
					Flash_sread(PRT_FLASH_BASE + FLASH_OFFSET_GapOutOfPaperThresholdOnLabel, (HUGEDATA INT8U *)&SensorStruct.GapOutOfPaperThresholdOnLabel, 1);
					break;
#ifdef USE_DISPLAY_COMM_SPEED_SETTING
				case 770 :
					  if (changed)	// changed & saved
					  {
						_SOFTWARE_RESET;
					  }
					  break;
#endif
				//case 996: // ParameterAction()으로 이동
			}
			LoadGlobalSetting();
			switch (ret_long) {
				case 533:
				case 536: 
					global_set_multi_unit(); break;		//Modified by JJANG 20070712
//#ifdef USE_TRACE_STANDALONE
				case 556:
					if (changed)	// changed & saved
					{
						v8 = get_global_bparam(GLOBAL_SCANNER_FLAG1);
						UseScanner = (v8>>3)&0x01;
					}
					break;
#ifdef USE_TRACE_STANDALONE                    
				case 561:
					if (changed)	// changed & saved
					{
						v8 = get_global_bparam(GLOBAL_SCANNER_FLAG1);
						ModeIndiv = (v8>>5)&0x07;
					}
					break;
  #ifdef USE_CHECKING_HARMFUL_MEAT
				case 706:
					if(changed)
					{
    #ifdef USE_TRACE_1000QTY //sewon 161201
						flashAddr = EXT_MEMCARD_ADDR + FLASH_TRACE_STANDALONE_AREA;
    #else
						flashAddr = DFLASH_BASE + FLASH_TRACE_STANDALONE_AREA;
    #endif
						Flash_clear(flashAddr, 0x00, MAX_INDIVIDUAL_NO * TRACE_INDIVIDUAL_IDENTITY_SIZE);
						_SOFTWARE_RESET;
					}
					break;
  #endif	//USE_CHECKING_HARMFUL_MEAT
#endif	//USE_TRACE_STANDALONE
			        case 593: global_set_pluinhibit(VERIFY_PCS,0);	//se-hyung 070802
			        	  plu_ptype_reset();
			        	  break;
			        case 615: global_set_pluinhibit(VERIFY_BYCOUNT,0);  //se-hyung 070802
			        	  plu_ptype_reset();
			        	  break;
				case 623:
					  KEY_CopyPS2SelfKeyTable();
					  break;
				case 625:
				case 715:
					  //Added by JJANG 20080123
					  if (changed&0x02)	// changed only
					  {
						ClearTransactionHeadTail();
						_SOFTWARE_RESET;
					  }
					  break;
				case 731 :
					  if (changed&0x02)	// changed only
					  {
						_SOFTWARE_RESET;
					  }
					  break;
				case 980:
					if (changed&0x02)	// changed only
					{
#ifdef USE_CHN_FONT		//CHINA 폰트 사용의 경우 PARAM 980은 입력값 무시됨

						set_prt_bparam(FLASH_OFFSET_StrType	,TWO_BYTE_FONT_STR);
						set_prt_bparam(FLASH_OFFSET_DspStrType	,TWO_BYTE_FONT_STR);
						set_global_bparam(GLOBAL_MENU_FONTID  	, 3);
						set_global_bparam(GLOBAL_PLU_FONTID   	, 3);
						set_global_bparam(GLOBAL_LABEL_FONTID  	, 4);
#else
						v16=get_global_wparam(GLOBAL_AREA_VENDOR_COUNTRY);
						language_flag=get_global_bparam(GLOBAL_SALE_SETUP_FLAG4);
						if(language_flag&0x08)	//Parameter 980 : Y로 설정시 PLU 한글 출력 가능.
						{
							set_prt_bparam(FLASH_OFFSET_StrType	,TWO_BYTE_FONT_STR);
							set_prt_bparam(FLASH_OFFSET_DspStrType	,TWO_BYTE_FONT_STR);
							if(v16==COUNTRY_US)	//US의 경우 메뉴 폰트 2줄 사용으로 한글 입력 가능.
							{
								set_global_bparam(GLOBAL_MENU_FONTID 	, 3);	//16by16 사용, 
							}
							else	//US 이외의 경우 메뉴 폰트 1줄 사용, 한글 입력 불가.
							{
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
#ifdef USE_8_16_FONT_MENU_DISPLAY
								set_global_bparam(GLOBAL_MENU_FONTID	, DSP_MEDIUM_FONT_ID);	//8 by 16사용
#else
								set_global_bparam(GLOBAL_MENU_FONTID	, DSP_SMALL_FONT_ID);	//6 by 8사용
#endif
#endif
							}
							set_global_bparam(GLOBAL_PLU_FONTID   	, DSP_PLU_NAME_FONT_ID);
							set_global_bparam(GLOBAL_LABEL_FONTID  	, 4);
						}
						else	//N으로 설정시
						{
							set_prt_bparam(FLASH_OFFSET_StrType	,ONE_BYTE_FONT_STR);
							set_prt_bparam(FLASH_OFFSET_DspStrType	,ONE_BYTE_FONT_STR);
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
#ifdef USE_8_16_FONT_MENU_DISPLAY
							set_global_bparam(GLOBAL_MENU_FONTID	, DSP_MEDIUM_FONT_ID);	//8 by 16사용
#else
							set_global_bparam(GLOBAL_MENU_FONTID	, DSP_SMALL_FONT_ID);
#endif
#endif
							set_global_bparam(GLOBAL_PLU_FONTID   	, DSP_PLU_NAME_FONT_ID);
							set_global_bparam(GLOBAL_LABEL_FONTID 	, 4);
						}

#endif
						_SOFTWARE_RESET;
					}
					break;
				case 584:
					global_set_pluinhibit(VERIFY_TARE,0);
					break;
				case 865:
					v16=get_global_wparam(GLOBAL_AREA_VENDOR_COUNTRY);
#if defined(_USE_LCD_7_37D_40DOT_) || defined(_USE_LCD_7_26d_16x96_)
					global_display_set_4digit(DISPLAY_PRICE_DIGIT, v16);
#else
					global_display_set_4digit(7, v16);
#endif
				case 620:
					if (changed&0x02)	// changed only
					{
						ClearTransactionHeadTail();
					}
					break;
#ifdef USE_REALTIME_CHECK_INTERVAL
				case 765:	//실시간 판매전송 할 경우 네트워크 체크 시간 설정하는 파라미터. 테스트 후 전체 적용.
					check_interval = get_global_wparam(GLOBAL_REALTIME_CHECK_INTERVAL);
					if( check_interval < REALTIME_CHECK_INTERVAL_MIN || check_interval > REALTIME_CHECK_INTERVAL_MAX )
					{
						check_interval = REALTIME_CHECK_INTERVAL_DEFAULT;
						set_global_wparam(GLOBAL_REALTIME_CHECK_INTERVAL, REALTIME_CHECK_INTERVAL_DEFAULT);
					}
					check_interval = check_interval * 10;//입력 초단위
					netcheckflag = 0;
					break;
#endif
				default : if ((ret_long>=771) && (ret_long<=782)) {
						status_scale.restrict  = get_global_wparam(GLOBAL_ACTION_ENABLE);
//						plu_ptype_enable_code();
//						global_set_pluinhibit();
						global_set_pluinhibit(VERIFY_ALL,0);  //se-hyung 070802
//						plu_tare_inhibit_set();
						plu_ptype_reset();
					  }
 			}
			ParameterAction(ret_long, changed);
			goto PAGE_VIEW;
		default:
			BuzOn(2);
			goto KI_IN;
	}

RET_ESC:
//DEL071127	Dsp_Fill_Buffer(0);	// LCD Buffer clear
	DspLoadFont1(old_font);
//DEL071127	Dsp_Diffuse();

	Flash_sread(PRT_FLASH_BASE, (HUGEDATA INT8U *)&PrtFlashStruct, PRT_FLASH_SET_STRUCT_SIZE);

 	return;
}

#ifndef CPU_PIC32MZ
void SaveEthernetDataForBoot(void)
{
	FW_VERSION  fversion;
	FW_VERSION2 fversion2;
	BOOT_STRUCT boot_struct;
	INT8U keytype;

	memset((INT8U *)&boot_struct, 0, sizeof(BOOT_STRUCT));
	
	version_get(&fversion);
	version2_get(&fversion2);

	//boot_struct.MAC[0] = EthCardStruct.MAC[5];
	//boot_struct.MAC[1] = EthCardStruct.MAC[4];
	//boot_struct.MAC[2] = EthCardStruct.MAC[3];
	//boot_struct.MAC[3] = EthCardStruct.MAC[2];
	//boot_struct.MAC[4] = EthCardStruct.MAC[1];
	//boot_struct.MAC[5] = EthCardStruct.MAC[0];
	get_base_sparam(FLASH_ETHERNET_MAC, boot_struct.MAC, 6);
	
	//boot_struct.IP[0] = EthCardStruct.IP[3];
	//boot_struct.IP[1] = EthCardStruct.IP[2];
	//boot_struct.IP[2] = EthCardStruct.IP[1];
	//boot_struct.IP[3] = EthCardStruct.IP[0];
	get_net_sparam(NETWORK_LOCAL_IP, boot_struct.IP, 4);
	
	keytype = get_global_bparam(GLOBAL_AREA_SCALETYPE);	// change to key type number
	if (keytype > 6 || keytype == 0) keytype = 2;	//P-type
	fversion.keytype = TypeSymbol[keytype - 1];
	memcpy(boot_struct.VersionStr, (INT8U *)&fversion, 16);
	memcpy(boot_struct.DealerVer, fversion2.dealer, 4);
	memcpy(boot_struct.ConutryVer, fversion2.country, 3);

//sprintf(MsgBuf, "1.1 %02x:%02x:%02x:%02x:%02x:%02x\r\n", boot_struct.MAC[0], boot_struct.MAC[1], boot_struct.MAC[2], boot_struct.MAC[3], boot_struct.MAC[4], boot_struct.MAC[5]);
//MsgOut(MsgBuf);	
//sprintf(MsgBuf, "1.2 %d.%d.%d.%d\r\n", boot_struct.IP[0], boot_struct.IP[1], boot_struct.IP[2], boot_struct.IP[3]);
//MsgOut(MsgBuf);	
	
	Flash_swrite(DFLASH_BASE + FLASH_PRINTER_AREA + FLASH_OFFSET_BootStruct, (INT8U *)&boot_struct, sizeof(BOOT_STRUCT));

	Flash_flush_nvbuffer();

	//Key_DelaySec(20, 0);
}
#endif
		
void ParameterAction(INT32U paramNum, INT8U changed)
{
	INT8U mode;
	INT8U string_buf[33];
	POINT disp_p;

//sprintf(MsgBuf, "paramNum=%ld\r\n", paramNum);
//MsgOut(MsgBuf);	
	switch (paramNum) {
		case 794:
			Flash_flush_nvbuffer();
			mode = Flash_bread(DFLASH_BASE + FLASH_FWMODE_SET);

			if ((mode&0x04))
			{
				display_alloff_primary_indication();
				VFD7_Diffuse();
				//ClearDisp7();
				display_lcd_clear();
				DspLoadFont1(3);
				disp_p = point(0, 0);
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
				sprintf((char*)string_buf, "Firmware Update by USB");
#else
				sprintf((char*)string_buf, "FW UP by USB");
#endif
				Dsp_Write_String(disp_p,(HUGEDATA char*)string_buf);
				Dsp_Diffuse();

				_SOFTWARE_RESET;
			}
			break;
		case 994:
			// F/W Update Condition
			// 0:CalSwitch 체크, 1:Serial Update, 3:Ethernet Update(http)
			// Boot에서 사용하기 위한 정보를 copy함
			Flash_flush_nvbuffer();
			mode = Flash_bread(DFLASH_BASE + FLASH_FWMODE_SET);

#ifndef CPU_PIC32MZ
			if (mode == 3)
			{
				SaveEthernetDataForBoot(); // fw ethernet update에 사용할 ip,mac 등을 저장

//sprintf(MsgBuf, "2 addr=%lx, size=%d\r\n", DFLASH_BASE + FLASH_PRINTER_AREA + FLASH_OFFSET_BootStruct, sizeof(BOOT_STRUCT));
//MsgOut(MsgBuf);	

//sprintf(MsgBuf, "3\r\n");
//MsgOut(MsgBuf);	
				display_alloff_primary_indication();
				VFD7_Diffuse();
				//ClearDisp7();
				display_lcd_clear();
				DspLoadFont1(2);
				disp_p = point(0, 0);
				sprintf((char*)string_buf, "FW Update!");

				Dsp_Write_String(disp_p,(HUGEDATA char*)string_buf);
				Dsp_Diffuse();
				
				_SOFTWARE_RESET;
			}
#endif
			break;
	}
}

#if 0
/**
 * @brief		특정 funcCode에 해당하는 Parameter 배열의 값으로 초기화하는 함수	
 * @param		funcCode
 * 					- initial_glb.c의 ParameterCountry, ParameterSetting에서 사용하는 funcCode의 배열
 * 					- 초기화하고 싶은 funcCode들을 배열으로 구성해서 보낼 것 
 * 					- 배열 마지막 값은 0 넣어서 보낼 것(배열 끝 확인 용도)
 * @return 	void
 * @remark	
 */
void init_by_funcCode(const INT16U *funcCode)
{
	INT32S i;
	INT16U country_idx, setting_idx;

	for(i = 0; funcCode[i]; i++)
	{
		country_idx = search_parameter_country(funcCode[i]);
		if(country_idx == PARAMETER_COUNTRY_SEARCH_FAIL)
		{
			continue;
		}

		setting_idx = search_parameter_setting(funcCode[i]);
		if(setting_idx == PARAMETER_SETTING_SEARCH_FAIL)
		{
			continue;
		}

		//SaveCodeParameter();
		parameter_init_by_idx(country_idx, setting_idx);
	}

	return;
}

/**
 * @brief		Parameter 배열에서 찾은 index를 기반으로 초기화 수행하는 함수
 * 
 * @param		country_idx
 * 					- ParameterCountry 배열에서의 index		
 * @param		setting_id
 * 					- ParameterSetting 배열에서의 index		
 * @return 	void	
 * @remark	
 * 				임시 작성
 * 				SaveCodeParameter() 분석 후 parameter_init_by_idx() 삭제 후 SaveCodeParameter()로 적용 예정
 */
void parameter_init_by_idx(INT16U country_idx, INT16U setting_idx)
{
	int bitF = ParameterSetting[setting_idx].bitFlag;
	int start_bit;
	int count_bit;
	int addr = 0;
	int value;
	int read_val;
	int modified_val;

	addr += ParameterSetting[setting_idx].offsetMemory;
	addr += ParameterSetting[setting_idx].addrMemory;

	switch(bitF)
	{
		case 1: // Byte
			read_val = Flash_bread(addr);
			modified_val = ParameterCountry[country_idx].defaultValue;

			if(ParameterCountry[country_idx].funcCode == 1774)
			{
				modified_val = ParameterCountry[country_idx].defaultValue * 13;
			}
			else if(ParameterCountry[country_idx].funcCode == 1777)
			{
				modified_val = ParameterCountry[country_idx].defaultValue * 8;
			}

			Flash_bwrite(addr, modified_val);
			break;
		case 2: // Word
			read_val = Flash_wread(addr);
			modified_val = ParameterCountry[country_idx].defaultValue;

			if(ParameterCountry[country_idx].funcCode == 1774)
			{
				modified_val = ParameterCountry[country_idx].defaultValue * 13;
			}
			else if(ParameterCountry[country_idx].funcCode == 1777)
			{
				modified_val = ParameterCountry[country_idx].defaultValue * 8;
			}
			Flash_wwrite(addr, modified_val);
			break;
		case 3: // String
			//Flash_swrite(addr, ParameterCountry[country_idx].defaultValue, ParameterSetting[setting_idx].sizeMemory);
			break;
		case 4: // Bit
			start_bit = ParameterSetting[setting_idx].startBit;
			count_bit = ParameterSetting[setting_idx].countBit;
			value = 0;

			// 좀 더 분석이 필요함
			switch(ParameterSetting[setting_idx].sizeMemory)
			{
				case 1:
					Flash_bwrite(addr, ParameterCountry[country_idx].defaultValue << start_bit - 1);
					break;
				case 2:
					Flash_wwrite(addr, ParameterCountry[country_idx].defaultValue << start_bit - 1);
					break;
				default:
					break;
			}
			break;
		case 5: // Long
			read_val = Flash_bread(addr);
			Flash_lwrite(addr, ParameterCountry[country_idx].defaultValue);
			break;
		default:
			break;
	}
	
	return;
}

/**
 * @brief		ParameterCountry 배열 탐색하는 함수
 * @param		funcCode
 * 					- 검색할 funcCode 
 * @return 	- ParameterCountry 배열에서 funcCode가 있는 곳의 index	
 * 				- 검색 실패시 PARAMETER_COUNTRY_SEARCH_FAIL return 
 * @remark	
 */
INT16U search_parameter_country(INT16U funcCode)
{
	INT32S idx;

	for(idx = 0; ParameterCountry[idx].funcCode; idx++)
	{
		if(ParameterCountry[idx].funcCode == funcCode)
		{
			return (INT16U)idx;
		}
	}
	return PARAMETER_COUNTRY_SEARCH_FAIL;
}

/**
 * @brief		ParameterSetting 배열 탐색하는 함수
 * @param		funcCode
 * 					- 검색할 funcCode 
 * @return 	- ParameterSetting 배열에서 funcCode가 있는 곳의 index	
 * 				- 검색 실패시 PARAMETER_SETTING_SEARCH_FAIL return 
 * @remark	
 */
INT16U search_parameter_setting(INT16U funcCode)
{
	INT32S idx;

	for(idx = 0; ParameterSetting[idx].funcCode; idx++)
	{
		if(ParameterSetting[idx].funcCode == funcCode)
		{
			return (INT16U)idx;
		}
	}

	return PARAMETER_SETTING_SEARCH_FAIL;
}
#endif
