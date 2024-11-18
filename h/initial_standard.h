/*****************************************************************************
|*			
|*  Copyright		:	(c) 2010 CAS
|*  Filename		:	initial_standard.h
|*  Version			:	1.5 (FLASH_STRUCT_VERSION)
|*  Programmer(s)	:	Chung Jongkwon (CJK)
|*  Created			:	2010/11/24
|*  Description		:	CL5000 Memory Definition (Standard)
|*				
*****************************************************************************/
#ifndef __INITIAL_STANDARD_H__
#define __INITIAL_STANDARD_H__
#ifndef FLASH_STRUCT_VERSION_DEF
	#define FLASH_STRUCT_VERSION_DEF
	//Direct Ingredient
	#ifdef USE_DIRECT_INGREDIENT
		#ifdef USE_USA_INGREDIENT
			#define FLASH_STRUCT_VERSION "20"	// US Direct Ingredient
		#elif defined (USE_EMPORIUM_INGREDIENT)
			#define FLASH_STRUCT_VERSION "24"	// Indonesia Emporium Market Ingredient
		#elif defined (USE_AUSTRALIA_INGREDIENT)
			#define FLASH_STRUCT_VERSION "25"	// Australia Ingredient
		#elif defined (USE_SLOVENIA_INGREDIENT)
			#define FLASH_STRUCT_VERSION "28"	// Slovenia Ingredient
		#elif defined (USE_SAUDI_INGREDIENT)
			#define FLASH_STRUCT_VERSION "32"	// Saudi Ingredient
		#elif defined (USE_RSA_SPAR_D_INGREDIENT)
			#define FLASH_STRUCT_VERSION "34"	// RSA Spar Direct Ingredient
		#elif defined (USE_NORTH_AMERICA_INGREDIENT)
			#define FLASH_STRUCT_VERSION "35"	// North America Ingredient ( Direct Nutrition Fact 사용, PLU 2,000개, CL3000 미국/캐나다 사양 적용 )
		#else	
			#define FLASH_STRUCT_VERSION "19"	// Standard Direct Ingredient
		#endif
	//Standard ingredient
	#else
		#ifdef USE_9999_INDIRECT_INGREDIENT
			#define FLASH_STRUCT_VERSION "25"	// 성분은 510byte 999개, 성분 index번호는 9999번까지 사용 가능함
		#elif defined (USE_VIETNAM_EMART_INGREDIENT)
			#define FLASH_STRUCT_VERSION "31"	//VIETNAM-EMART Ingredient
		#else
			#if defined (USE_EUR_LOGISTICS_PLU)
				#define FLASH_STRUCT_VERSION "36"	// 영공기반 Logistics(물류전용) PLU(8000개) 사용
			#else
				#define FLASH_STRUCT_VERSION "15"	// Standard
			#endif
		#endif
	#endif
	#define NVRAM_STRUCT_VERSION "103"
#endif

#ifndef	_MEMORY_MAP_DEFINE
#define	_MEMORY_MAP_DEFINE
#define FLASH_STR_VERSION	0
#define FLASH_CREATE_DATE	2
#define FLASH_ROM_STATUS	8
#define FLASH_MEMORY_ADDRESS	16
////////////////////////////////////////////
// 중요 : 위치변경불가
//  CTS Structure (Enable, Period, Serial No)
#define FLASH_CTS_STRUCT	48
// 중요 : FW UPDATE MODE 저장위치는 BOOT에서 사용하므로 변경되면 안됨
// 80 = FLASH_FWMODE_SET
#define FLASH_FWMODE_SET	80
////////////////////////////////////////////
#define FLASH_FW_UPDATE_VER	81
#define FLASH_START_PASSWORD	82
#define FLASH_START_COUNT_HIGH	90
#define FLASH_START_COUNT	92
#define FLASH_START_LOG	94
////////////////////////////////////////////
// 중요 : 위치변경불가
#define FLASH_CALLOG_COUNT	124
#define FLASH_CALLOG_DATA	126
// 중요 : 위치변경불가
//  CTS Structure2 (Product No)
#define FLASH_CTS_STRUCT2	246
////////////////////////////////////////////
#define FLASH_ADC_AREA	276
#define FLASH_ADC_WUNIT	404
#define FLASH_ADC_FILTER_LEVEL	405
#define FLASH_ADC_USE_FULLTARE	406
#define FLASH_ADC_TARE_CONFIG	407
#ifdef USE_ADM_AUTO_RESTORE
#define FLASH_ADM_INIT_ERROR_COUNT	408
#define FLASH_ADM_EEP_ERROR_COUNT	409
#define FLASH_ADM_INIT_COMPLETE     410
#define FLASH_ADC_RESERVED2	411
#else
#define FLASH_ADC_RESERVED2	408
#endif
#define FLASH_ETHERNET_MAC	412
////////////////////////////////////////////
// 중요 : 위치변경불가
//  BOOT에서 사용할 MAC, IP, Version 정보가 Printer 영역의 뒤쪽 48byte에 존재함
#define FLASH_PRINTER_AREA	418
///////////////////////////////////////////
#define FLASH_GLOBAL_AREA	674
#define FLASH_BASIC_SYMBOL_AREA	806
#define FLASH_DEALER_AREA	1016
#define FLASH_BASIC_SYMBOL_AREA_END	1074
#define FLASH_BASIC_PARAMETER	1086
#define FLASH_NET_AREA	1416
#define FLASH_FONT_AREA	1598
#define FLASH_CAPTION_AREA	263796
#define FLASH_LABEL_CAPTION_AREA	335800
#define FLASH_PCS_UNIT_AREA	342200
#define FLASH_PLU_STRUCT_CAPTION_AREA	342344
#define FLASH_ORIGIN_AREA	346440
#define FLASH_PLU_STRUCT_AREA	363440
#define FLASH_KEY_AREA	365518
#define FLASH_KEY_SHIFT_AREA	365774
#define FLASH_KEY_PGM_AREA	366030
#define FLASH_KEY_PGM_SHIFT_AREA	366286
#define FLASH_KEY_PGM2_AREA	366542
#define FLASH_KEY_PGM2_SHIFT_AREA	366798
#define FLASH_PSKEY_AREA	367054
#define FLASH_PSKEY_SHIFT_AREA	367374
#define FLASH_PSKEY_PGM_AREA	367694
#define FLASH_PSKEY_PGM_SHIFT_AREA	368014
#define FLASH_PSKEY_PGM2_AREA	368334
#define FLASH_PSKEY_PGM2_SHIFT_AREA	368654
#define FLASH_KEY_CDTC_AREA	368974
#define FLASH_KEY_CUSTOMER_AREA	369230
#define FLASH_KEY_SPEED_AREA	369486
#define FLASH_LABEL_AREA	372686
#define FLASH_LABEL_IMAGE_AREA_ADDR	\
      (FLASH_LABEL_AREA + LABEL_CUSTOM_2MB_NUM * LABEL_HEADER_SIZE) // 라벨 이미지 주소 : 373166
#define FLASH_BITMAP_AREA	455086
#define FLASH_CURRENCY_AREA	569858
#define FLASH_SCROLL_AREA	569918
#define FLASH_PRT_ADVE_AREA	570928
#define FLASH_NUMBER_PLU	574128
#define FLASH_NUMBER_BARCODE	574132
#define FLASH_NUMBER_ORIGIN	574134
#define FLASH_NUMBER_TARE	574136
#define FLASH_NUMBER_NUTRIFACT	574138
#define FLASH_NUMBER_INGRIDIENT_SIZE	574140
#define FLASH_NUMBER_INGRIDIENT	574142
#define FLASH_NUMBER_SHOP	574144
#define FLASH_NUMBER_LABELADVERTISE	574146
#define FLASH_NUMBER_DEPARTMENT	574148
#define FLASH_NUMBER_GROUP	574150
#define FLASH_NUMBER_COUNTRY	574152
#define FLASH_NUMBER_SLAUGHTER	574154
#define FLASH_NUMBER_CUTTINGHALL	574156
#define FLASH_NUMBER_TRACEBILITY	574158
#define FLASH_NUMBER_CLERK	574160
#define FLASH_NUMBER_CUSTOMER	574162
#define FLASH_NUMBER_DISCOUNT	574164
#define FLASH_NUMBER_BONUS	574166
#define FLASH_NUMBER_TAX	574168
#define FLASH_NUMBER_SCALE	574170
#define FLASH_NUMBER_PAID	574172
#define FLASH_NUMBER_REPORT_PERIOD	574173
#define FLASH_NUMBER_TRANSACTION	574174
#define FLASH_NUMBER_RESERVED1	574176
#define FLASH_VERIFY_DATE	574180	// Reserved for Korea Function(Check Verification Timer)	// CJK080219
#define FLASH_TAX_AREA	574184
#define FLASH_BARCODE_FORMAT_AREA	574264
#define FLASH_SHOPNAME_AREA	579664
#define FLASH_DEPARTMENT_AREA	580944
#define FLASH_SUB_DEPARTMENT_AREA	584144
#define FLASH_TARE_AREA	587344
#define FLASH_CLERK_AREA	589744
#define FLASH_CLERK_PERMISSION	594544
#define FLASH_CUSTOMER_AREA	594624	//594604
#ifdef USE_TEST_TMP_PRTBUF_MAX_SIZE
#define FLASH_TMP_TEST_RESULT_AREA      614424	// Added by ckKIM 20180916
#define FLASH_CUSTOMER_RESERVED_AREA	614430	// Added by CJK 20060608
#else
#define FLASH_CUSTOMER_RESERVED_AREA	614424	// Added by CJK 20060608
#endif
#define FLASH_LICENSE		614796
#define FLASH_REMOTE_IP2	614876
#define FLASH_SCANNER_FORMAT	614882
#define FLASH_SLAVE_INFO	615304	// CJK080517
#define FLASH_EXT_PRT_LABEL_NAME	615464 //external printer(BT-DT-4), Label form name
#define FLASH_DISCOUNT_GLOBAL_WEIGHT	615604
#define FLASH_DISCOUNT_GLOBAL_COUNT	615652
#define FLASH_DISCOUNT_GLOBAL_NONFOOD	615700
#define FLASH_DISCOUNT_AREA	615748
#if defined(USE_USA_INGREDIENT) || defined(USE_NORTH_AMERICA_INGREDIENT)
#define FLASH_INGREDIENT_AREA	620548
//	#define FLASH_INGREDIENT_TEXT_AREA	 620548
#define FLASH_INGREDIENT_RESERVED_AREA	 1122148
#define FLASH_PLU_AREA	1134548
#else //USE_USA_INGREDIENT
#define FLASH_INGREDIENT_AREA	620548
//	#define FLASH_INGREDIENT_TEXT_AREA	 1132548	   //SG070725.ADD
#define FLASH_INGREDIENT_RESERVED_AREA	 1132548   //SG070725.ADD
#endif //USE_USA_INGREDIENT

#define FLASH_PLU_AREA              1134548
#ifdef USE_EUR_LOGISTICS_PLU
#define FLASH_PLU_RESERVED_AREA	 1869248
#endif

#ifdef USE_NORTH_AMERICA_INGREDIENT
//#define FLASH_NUTRIFACT_AREA	1549548 // 8M 영역에 정의
#else
#define FLASH_NUTRIFACT_AREA	1902548
#endif  // USE_NORTH_AMERICA_INGREDIENT
#define FLASH_COUNTRY_AREA	1949548

#ifdef USE_KOR_TRACE
#define FLASH_TRACEABILITY_AREA	2013548	// 저울나라 manual operation용   
#endif

#ifdef USE_TRACEABILITY
#define FLASH_COUNTRY_AREA	1949548
#define FLASH_SLAUGHTERHOUSE_AREA	1969548
#define FLASH_CUTTINGHALL_AREA	1991548
#define FLASH_TRACEABILITY_AREA	2013548
#endif

#define FLASH_JOBBATCH_AREA 		2041548	 //SG061220:ADD
#ifndef USE_10000_SCANNER_CONVTABLE
#define FLASH_SCANNER_TABLE_AREA 	2041708	 //SG070124:ADD
#endif
#define FLASH_LABEL_KEY_TABLE_AREA	2085740  //SG070222:ADD
#define FLASH_TICKET_HEAD_MESSAGE_AREA	2085788	//320 = 64*5 bytes
#define FLASH_TICKET_TAIL_MESSAGE_AREA	2086108	//320 = 64*5 bytes
#ifdef USE_LOT_NUMBER_FUNCTION
#define FLASH_LOT_NUMBER	2086428
#endif

// Menu 1891 메뉴 설정 플래시 주소 (2 byte)
#ifdef USE_INIT_MASK
#define FLASH_KEEP_INIT_SETTINGS 2086448	
#endif

#define FLASH_END_POINT 	2086450//2092140  //SG070222:MODD
#endif // of #ifndef _MEMORY_MAP_DEFINE
/********************************** ~2M End **********************************/


/********************************** 2M ~ 8M **********************************/
#if defined (USE_EUR_LOGISTICS_PLU)
	#define MAX_PLU_RECORDSIZE_DEFAULT	310
#else
	#define MAX_PLU_RECORDSIZE_DEFAULT	256
#endif

#if defined (USE_USA_INGREDIENT) || defined (USE_NORTH_AMERICA_INGREDIENT)
	#define MAX_PLU_ADDRESS 4000
	#define PLU_BASIC_AREA_QTY	3000l
	#define PLU_EXTER_AREA_QTY	1000l
	#define PLU_EXTER2_AREA_QTY	0
#elif defined (USE_AUSTRALIA_INGREDIENT)
	#define MAX_PLU_ADDRESS 7000
	#define PLU_BASIC_AREA_QTY	3000l
	#define PLU_EXTER_AREA_QTY	4000l
	#define PLU_EXTER2_AREA_QTY	0
#elif defined (USE_SLOVENIA_INGREDIENT)
	#define MAX_PLU_ADDRESS 6500
	#define PLU_BASIC_AREA_QTY	3000l
	#define PLU_EXTER_AREA_QTY	3500l
	#define PLU_EXTER2_AREA_QTY	0
#elif defined (USE_SAUDI_INGREDIENT)
	#define MAX_PLU_ADDRESS 3000
	#define PLU_BASIC_AREA_QTY	3000l
	#define PLU_EXTER_AREA_QTY	0
	#define PLU_EXTER2_AREA_QTY	0
#elif defined (USE_RSA_SPAR_D_INGREDIENT)
	#define MAX_PLU_ADDRESS 4000
	#define PLU_BASIC_AREA_QTY	3000l
	#define PLU_EXTER_AREA_QTY	1000l
	#define PLU_EXTER2_AREA_QTY	0
#else
  #ifdef USE_EXTEND_NUMBER_OF_PLU_FOR_8M_DFLASH 
	#define MAX_PLU_ADDRESS 12000   // 최대 PLU 개수 12,000개로 확장(영공기준, Stack Memory Size 제한으로 인해 PLU수 최대 12,000개 까지 확장가능) 
	#define PLU_BASIC_AREA_QTY	3000l
	#define PLU_EXTER_AREA_QTY	7000l 
  #elif defined(USE_VIETNAM_EMART_INGREDIENT)
  	#define MAX_PLU_ADDRESS		8000l
	#define PLU_BASIC_AREA_QTY	3000l
	#define PLU_EXTER_AREA_QTY	5000l
  #else
	#if defined (USE_EUR_LOGISTICS_PLU)	// 영공기반 Logistics(물류전용) PLU(8000개) 사용
		#define MAX_PLU_ADDRESS 8000
		#define PLU_BASIC_AREA_QTY	2370l
		#define PLU_EXTER_AREA_QTY	5630l
	#else
		#define MAX_PLU_ADDRESS 10000
		#define PLU_BASIC_AREA_QTY	3000l
		#define PLU_EXTER_AREA_QTY	7000l 
	#endif
  #endif

 #ifdef USE_EXTEND_8M_DFLASH
    #ifdef USE_EXTEND_NUMBER_OF_PLU_FOR_8M_DFLASH 
	    #define PLU_EXTER2_AREA_QTY	2000l   // 최대 PLU 개수 12,000개로 확장(영공기준, Stack Memory Size 제한으로 인해 PLU수 최대 12,000개 까지 확장가능) 
    #else
        #define PLU_EXTER2_AREA_QTY	0
    #endif
 #endif
#endif

#ifdef USE_STOKROTKA_MART_DEFAULT
#define DEFAULT_INGREDIENT_SIZE 		1022
#else
#define DEFAULT_INGREDIENT_SIZE 		512
#endif
#ifdef USE_DIRECT_INGREDIENT
 #if defined (USE_USA_INGREDIENT) || defined (USE_NORTH_AMERICA_INGREDIENT)
	#define DIRECT_INGREDIENT_BLOCK_NUM		14	// Direct Ingredient Block 수(한 PLU가 가질 수 있는 최대 Block 수)
	#define INGREDIENT_BASIC_AREA_QTY		0l
	#define INGREDIENT_EXTER_AREA_QTY		0l
	#define INGREDIENT_TEXT_BASIC_AREA_QTY	3300l
	#define INGREDIENT_TEXT_EXTER_AREA_QTY	12000l
	#define PLU_REPORT_QTY_1				4000	//para 625 : 0, PLU 수량(3000+1000)과 PLU report 수량(3000+6000) 중 작은 값
	#define PLU_REPORT_QTY_2				4000	//para 625 : 1, PLU 수량(3000+1000)과 PLU report 수량(3000+4500) 중 작은 값
 #elif defined (USE_EMPORIUM_INGREDIENT)
	#define DIRECT_INGREDIENT_BLOCK_NUM		7		// Direct Ingredient Block 수(한 PLU가 가질 수 있는 최대 Block 수)
	#define INGREDIENT_BASIC_AREA_QTY		1000l
	#define INGREDIENT_EXTER_AREA_QTY		0l
	#define INGREDIENT_TEXT_BASIC_AREA_QTY	0l
	#define INGREDIENT_TEXT_EXTER_AREA_QTY	2000l
	#define PLU_REPORT_QTY_1				9000	//para 625 : 0, PLU 수량(3000+7000)과 PLU report 수량(3000+6000) 중 작은 값
	#define PLU_REPORT_QTY_2				7500	//para 625 : 1, PLU 수량(3000+7000)과 PLU report 수량(3000+4500) 중 작은 값
 #elif defined (USE_AUSTRALIA_INGREDIENT)
	#define DIRECT_INGREDIENT_BLOCK_NUM		1		// Direct Ingredient Block 수(한 PLU가 가질 수 있는 최대 Block 수)
	#define INGREDIENT_BASIC_AREA_QTY		1000l
	#define INGREDIENT_EXTER_AREA_QTY		0l
	#define INGREDIENT_TEXT_BASIC_AREA_QTY	0l
	#define INGREDIENT_TEXT_EXTER_AREA_QTY	7000l
	#define PLU_REPORT_QTY_1				7000	//para 625 : 0, PLU 수량(3000+7000)과 PLU report 수량(3000+6000) 중 작은 값
	#define PLU_REPORT_QTY_2				7000	//para 625 : 1, PLU 수량(3000+7000)과 PLU report 수량(3000+4500) 중 작은 값
 #elif defined (USE_SLOVENIA_INGREDIENT)
	#define DIRECT_INGREDIENT_BLOCK_NUM		4		// Direct Ingredient Block 수(한 PLU가 가질 수 있는 최대 Block 수)
	#define INGREDIENT_BASIC_AREA_QTY		1000l
	#define INGREDIENT_EXTER_AREA_QTY		0l
	#define INGREDIENT_TEXT_BASIC_AREA_QTY	0l
	#define INGREDIENT_TEXT_EXTER_AREA_QTY	26000l	// PLU 당 6500 개 당 4 Block x 6500 EA = 26000 EA
	#define PLU_REPORT_QTY_1				6500	//para 625 : 0, PLU 수량(3000+3500)과 PLU report 수량(3000+6000) 중 작은 값
	#define PLU_REPORT_QTY_2				6500	//para 625 : 1, PLU 수량(3000+3500)과 PLU report 수량(3000+4500) 중 작은 값
 #elif defined (USE_SAUDI_INGREDIENT)
	#define DIRECT_INGREDIENT_BLOCK_NUM		4		// Direct Ingredient Block 수(한 PLU가 가질 수 있는 최대 Block 수)
	#define INGREDIENT_BASIC_AREA_QTY		1000l
	#define INGREDIENT_EXTER_AREA_QTY		0l
	#define INGREDIENT_TEXT_BASIC_AREA_QTY	0l
	#define INGREDIENT_TEXT_EXTER_AREA_QTY	12000l	// PLU 3000개 + Direct Ingredient 4Block / 4 Block x 3000 EA = 12000 EA
	#define PLU_REPORT_QTY_1				3000
	#define PLU_REPORT_QTY_2				3000
 #elif defined (USE_RSA_SPAR_D_INGREDIENT)
	#define DIRECT_INGREDIENT_BLOCK_NUM		6		// Direct Ingredient Block 수(한 PLU가 가질 수 있는 최대 Block 수)
	#define INGREDIENT_BASIC_AREA_QTY		1000l
	#define INGREDIENT_EXTER_AREA_QTY		0l
	#define INGREDIENT_TEXT_BASIC_AREA_QTY	0l
	#define INGREDIENT_TEXT_EXTER_AREA_QTY	24000l	//PLU 4000ea, Direct Ingredient size 900(6 blocks) = 6 * 4,000 = 24,000 blocks 
	#define PLU_REPORT_QTY_1				4000	// para 625 : 0, PLU 수량(4000)과 PLU report 수량(3000+6000) 중 작은 값
	#define PLU_REPORT_QTY_2				4000	// para 625 : 1, PLU 수량(4000)과 PLU report 수량(3000+4500) 중 작은 값
 #else // Standard Direct Ingredient
	#define DIRECT_INGREDIENT_BLOCK_NUM		7		// Direct Ingredient Block 수(한 PLU가 가질 수 있는 최대 Block 수)	
	#define INGREDIENT_BASIC_AREA_QTY		1000l
	#define INGREDIENT_EXTER_AREA_QTY		0l
	#define INGREDIENT_TEXT_BASIC_AREA_QTY	0l
	#ifdef USE_DIRECT_INGREDIENT_EXTEND_8M_DFLASH
	#define INGREDIENT_TEXT_EXTER_AREA_QTY	24000l
	#else
	#define INGREDIENT_TEXT_EXTER_AREA_QTY	2000l
	#endif
	#define PLU_REPORT_QTY_1				9000	//para 625 : 0, PLU 수량(3000+7000)과 PLU report 수량(3000+6000) 중 작은 값
	#define PLU_REPORT_QTY_2				7500    //para 625 : 1, PLU 수량(3000+7000)과 PLU report 수량(3000+4500) 중 작은 값
 #endif
	#define DEFAULT_INGREDIENT_TEXT_SIZE	152 
	#define DEFAULT_INGREDIENT_TEXT_ALL_SIZE  ((DEFAULT_INGREDIENT_TEXT_SIZE - 2) * DIRECT_INGREDIENT_BLOCK_NUM)  // ingredient 전체길이
#else	// Static indexed ingredient
  #ifdef USE_9999_INDIRECT_INGREDIENT		// 성분은 510byte 999개, 성분 index번호는 1~9999번까지 사용 가능함
		#define MAX_INGREDIENT_INDEX_NUM		9999l		// Ingredient index Max 값 
		#define INGREDIENT_BASIC_AREA_QTY		1000l
		#define INGREDIENT_EXTER_AREA_QTY		0l
		#define INGREDIENT_TEXT_BASIC_AREA_QTY	0l
		#define INGREDIENT_TEXT_EXTER_AREA_QTY	0l
		#define PLU_REPORT_QTY_1				9000	//para 625 : 0, PLU 수량(3000+7000)과 PLU report 수량(3000+6000) 중 작은 값
		#define PLU_REPORT_QTY_2				7500	//para 625 : 1, PLU 수량(3000+7000)과 PLU report 수량(3000+4500) 중 작은 값
  #else
    #ifdef USE_INGREDIENT_CODE_4DIGIT
		#define MAX_INGREDIENT_INDEX_NUM		9999l		// Ingredient index Max 값 
	#endif
		#define INGREDIENT_BASIC_AREA_QTY		1000l

	#ifdef USE_VIETNAM_EMART_INGREDIENT
		#define INGREDIENT_EXTER_AREA_QTY		1500l
	#else
		#define INGREDIENT_EXTER_AREA_QTY		0l
	#endif
		#define INGREDIENT_TEXT_BASIC_AREA_QTY	0l
		#define INGREDIENT_TEXT_EXTER_AREA_QTY	0l
	#ifdef USE_INDIA_FUNCTION
		#define PLU_REPORT_QTY_1				3000	//para 625 : 0, PLU 수량(3000+7000)과 PLU report 수량(3000 + 0) 중 작은 값
		#define PLU_REPORT_QTY_2				7500    //para 625 : 1, PLU 수량(3000+7000)과 PLU report 수량(3000+4500) 중 작은 값
	#else
		#define PLU_REPORT_QTY_1				9000	//para 625 : 0, PLU 수량(3000+7000)과 PLU report 수량(3000+6000) 중 작은 값
		#define PLU_REPORT_QTY_2				7500	//para 625 : 1, PLU 수량(3000+7000)과 PLU report 수량(3000+4500) 중 작은 값
	#endif
  #endif
	#define DEFAULT_INGREDIENT_TEXT_SIZE	304
#endif //USE_DIRECT_INGREDIENT

//0x200000(2M Byte)
#define FLASH_FIRMWARE_SIZE 0x200000
#ifdef USE_EXTEND_FLASH
#define FLASH_FIRMWARE_CS1_AREA (DFLASH_LOGICAL_FLASH2_BASE + 0x200000) //32 MByte + 2 MByte(Reserved)
#define FLASH_FIRMWARE_CHKSUM_CS1_AREA (FLASH_FIRMWARE_CS1_AREA + FLASH_FIRMWARE_SIZE - 0x08) //4byte
#define FLASH_FIRMWARE_FLAG_CS1_AREA (FLASH_FIRMWARE_CS1_AREA + FLASH_FIRMWARE_SIZE - 0x04) //4byte
#endif
//0x200000(2M Byte)

/********************************** ~8M 공통 **********************************/
#include "initial_8m_common.h"

#endif /* __INITIAL_STANDARD_H__ */
