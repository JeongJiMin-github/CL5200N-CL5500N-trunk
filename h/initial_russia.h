/*****************************************************************************
|*			
|*  Copyright		:	(c) 2010 CAS
|*  Filename		:	initial_russia.h
|*  Version			:	1.6 (FLASH_STRUCT_VERSION)
|*  Programmer(s)	:	Chung Jongkwon (CJK)
|*  Created			:	2010/11/24
|*  Description		:	CL5000 Memory Definition (Russia Version)
|*				
****************************************************************************/
#if (ROM_COUNTRY == ROM_RUS)

#ifndef FLASH_STRUCT_VERSION_DEF
#define FLASH_STRUCT_VERSION_DEF
#ifdef USE_RUSSIA_LOGISTICS_PLU
#define FLASH_STRUCT_VERSION "26"	// Russia Logistics PLU
#else
#define FLASH_STRUCT_VERSION "16"	// Russia PLU Structure
#endif
#define NVRAM_STRUCT_VERSION "113"
//#define USE_DIRECT_INGREDIENT	// use direct ingredient. Russia �� �׻� ���
#endif

#ifndef	_MEMORY_MAP_DEFINE
#define	_MEMORY_MAP_DEFINE
#define FLASH_STR_VERSION	0
#define FLASH_CREATE_DATE	2
#define FLASH_ROM_STATUS	8
#define FLASH_MEMORY_ADDRESS	16
////////////////////////////////////////////
// �߿� : ��ġ����Ұ�
//  CTS Structure (Enable, Period, Serial No)
#define FLASH_CTS_STRUCT	48
// �߿� : FW UPDATE MODE ������ġ�� BOOT���� ����ϹǷ� ����Ǹ� �ȵ�
// 80 = FLASH_FWMODE_SET
#define FLASH_FWMODE_SET	80
////////////////////////////////////////////
#define FLASH_FW_UPDATE_VER	81
#define FLASH_START_PASSWORD	82
#define FLASH_START_COUNT_HIGH	90
#define FLASH_START_COUNT	92
#define FLASH_START_LOG	94
////////////////////////////////////////////
// �߿� : ��ġ����Ұ�
#define FLASH_CALLOG_COUNT	124
#define FLASH_CALLOG_DATA	126
// �߿� : ��ġ����Ұ�
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
// �߿� : ��ġ����Ұ�
//  BOOT���� ����� MAC, IP, Version ������ Printer ������ ���� 48byte�� ������
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
      (FLASH_LABEL_AREA + LABEL_CUSTOM_2MB_NUM * LABEL_HEADER_SIZE) // �� �̹��� �ּ� : 373166
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
#define FLASH_CUSTOMER_RESERVED_AREA	614424	// Added by CJK 20060608
#define FLASH_LICENSE		614796
#define FLASH_REMOTE_IP2	614876
#define FLASH_SCANNER_FORMAT	614882
#define FLASH_SLAVE_INFO	615304	// CJK080517
#define FLASH_EXT_PRT_LABEL_NAME	615464
#define FLASH_DISCOUNT_GLOBAL_WEIGHT	615604
#define FLASH_DISCOUNT_GLOBAL_COUNT	615652
#define FLASH_DISCOUNT_GLOBAL_NONFOOD	615700
#define FLASH_DISCOUNT_AREA	615748
#define FLASH_PLU_AREA	620548
#ifdef USE_ONLY_INDIRECT_INGREDIENT
#define FLASH_INGREDIENT_AREA	1532548
#define FLASH_INGREDIENT_RESERVED_AREA	 1879848   //SG070725.ADD
#else
#if defined USE_DMSG_1050_BYTE
#define FLASH_INGREDIENT_AREA	1604548
#define FLASH_INGREDIENT_RESERVED_AREA	1872748
#elif defined USE_DMSG_2100_BYTE	//memory �ʰ��� ���� ����
#define FLASH_INGREDIENT_AREA	1688548
#define FLASH_INGREDIENT_RESERVED_AREA	1880748
#endif
#endif
#define FLASH_NUTRIFACT_AREA	1882948

#ifdef USE_TRACEABILITY
#define FLASH_COUNTRY_AREA	1929948
#define FLASH_SLAUGHTERHOUSE_AREA	1949948
#define FLASH_CUTTINGHALL_AREA	1971948
#define FLASH_TRACEABILITY_AREA	1993948
#endif

#define FLASH_JOBBATCH_AREA 		2041548	 //SG061220:ADD
#define FLASH_SCANNER_TABLE_AREA 	2041708	 //SG070124:ADD
#define FLASH_LABEL_KEY_TABLE_AREA	2085708  //SG070222:ADD
#define FLASH_TICKET_HEAD_MESSAGE_AREA	2085756
#define FLASH_TICKET_TAIL_MESSAGE_AREA	2086076

// Menu 1891 �޴� ���� �÷��� �ּ� (2 byte)
#ifdef USE_INIT_MASK
#define FLASH_KEEP_INIT_SETTINGS 2086448	
#endif

#define FLASH_END_POINT 	2092160
#endif	// of #ifndef	_MEMORY_MAP_DEFINE
/********************************** ~2M End **********************************/


/********************************** 2M ~ 8M **********************************/
#ifdef USE_ONLY_INDIRECT_INGREDIENT

// PLU & ingredient size //
#define MAX_PLU_RECORDSIZE_DEFAULT	152 //add direct ingredient no field
#define PLU_BASIC_AREA_QTY	6000l
#define PLU_EXTER_AREA_QTY	8000l

#define DEFAULT_INGREDIENT_SIZE  		302l
#define DEFAULT_INGREDIENT_TEXT_SIZE	304l
#define DIRECT_INGREDIENT_BLOCK_NUM	1	// Direct Ingredient Block ��(�� PLU�� ���� �� �ִ� �ִ� Block ��)
#define DEFAULT_INGREDIENT_TEXT_ALL_SIZE  ((DEFAULT_INGREDIENT_TEXT_SIZE - 2) * DIRECT_INGREDIENT_BLOCK_NUM)  // ingredient ��ü����
#define INGREDIENT_BASIC_AREA_QTY	1150l
#define INGREDIENT_EXTER_AREA_QTY	2850l
#define INGREDIENT_TEXT_BASIC_AREA_QTY	0l
#define INGREDIENT_TEXT_EXTER_AREA_QTY	0l

#define MAX_PLU_ADDRESS 14000	// sorting table qty(PLU�Ѱ��� ������ �ʿ��� ��� NVRAM Address Table�����ʿ�)
								// Max 16000�� ���� ����(NVRAM_PLU_TABLE����)

//Added by JJANG 2080124
#define PLU_REPORT_QTY_1	14000	//para 625 : 0, PLU ����(6000+8000)�� PLU report ����(6000+8000) �� ���� ��
#define PLU_REPORT_QTY_2	14000	//para 625 : 1, PLU ����(6000+8000)�� PLU report ����(6000+8000) �� ���� ��

#elif defined (USE_RUSSIA_LOGISTICS_PLU)

// PLU & ingredient size //
#define MAX_PLU_RECORDSIZE_DEFAULT	372
#define PLU_BASIC_AREA_QTY	2500l
#define PLU_EXTER_AREA_QTY	3000l
#define PLU_EXTER2_AREA_QTY 0l   // Serial Flash 8MB Ȯ�� �׽�Ʈ�� ���ؼ� �ִ� PLU ���� 12,000���� Ȯ�� ���þƿ��� ��� ����

#define DEFAULT_INGREDIENT_SIZE  		402l	// indirect ingredient size
#define DEFAULT_INGREDIENT_TEXT_SIZE	152l	// direct ingredient block size
#define DIRECT_INGREDIENT_BLOCK_NUM	14	// Direct Ingredient Block ��(�� PLU�� ���� �� �ִ� �ִ� Block ��)
#define DEFAULT_INGREDIENT_TEXT_ALL_SIZE  ((DEFAULT_INGREDIENT_TEXT_SIZE - 2) * DIRECT_INGREDIENT_BLOCK_NUM)  // ingredient ��ü����

#define DIRECT_INGREDIENT_BLOCK_NUM_CL5KJ_BLOCK_SIZE1	((DEFAULT_INGREDIENT_TEXT_SIZE - 2) * 3)  //ù��° �������� ���� (450byte)
#define DIRECT_INGREDIENT_BLOCK_NUM_CL5KJ_BLOCK_SIZE2	((DEFAULT_INGREDIENT_TEXT_SIZE - 2) * 4)  //�ι�° �������� ���� (600byte)

#define INGREDIENT_BASIC_AREA_QTY	100l   //SG070725.TEST
#define INGREDIENT_EXTER_AREA_QTY	0l
#define INGREDIENT_TEXT_BASIC_AREA_QTY	1000l
#define INGREDIENT_TEXT_EXTER_AREA_QTY	7000l

#define MAX_PLU_ADDRESS 10000	// sorting table qty(PLU�Ѱ��� ������ �ʿ��� ��� NVRAM Address Table�����ʿ�)
								// Max 16000�� ���� ����(NVRAM_PLU_TABLE����)
//Added by JJANG 2080124
#define PLU_REPORT_QTY_1	10000	//para 625 : 0, PLU ����(6000+4000)�� PLU report ����(6000+8000) �� ���� ��
#define PLU_REPORT_QTY_2	10000	//para 625 : 1, PLU ����(6000+4000)�� PLU report ����(6000+8000) �� ���� ��

#else

// PLU & ingredient size //
#define PLU_BASIC_AREA_QTY	6000l
#define PLU_EXTER_AREA_QTY	4000l
#define PLU_EXTER2_AREA_QTY 0l   // Serial Flash 8MB Ȯ�� �׽�Ʈ�� ���ؼ� �ִ� PLU ���� 12,000���� Ȯ�� ���þƿ��� ��� ����

#define DEFAULT_INGREDIENT_SIZE  		402l	// indirect ingredient size
#define DEFAULT_INGREDIENT_TEXT_SIZE	152l	// direct ingredient block size
#if defined USE_DMSG_1050_BYTE
#define MAX_PLU_RECORDSIZE_DEFAULT	164
#define DIRECT_INGREDIENT_BLOCK_NUM	7	// Direct Ingredient Block ��(�� PLU�� ���� �� �ִ� �ִ� Block ��)
#elif defined USE_DMSG_2100_BYTE
#define MAX_PLU_RECORDSIZE_DEFAULT	178
#define DIRECT_INGREDIENT_BLOCK_NUM	14	// Direct Ingredient Block ��(�� PLU�� ���� �� �ִ� �ִ� Block ��)
#endif
#define DEFAULT_INGREDIENT_TEXT_ALL_SIZE  ((DEFAULT_INGREDIENT_TEXT_SIZE - 2) * DIRECT_INGREDIENT_BLOCK_NUM)  // ingredient ��ü����

#define DIRECT_INGREDIENT_BLOCK_NUM_CL5KJ_BLOCK_SIZE1	((DEFAULT_INGREDIENT_TEXT_SIZE - 2) * 3)  //ù��° �������� ���� (450byte)
#define DIRECT_INGREDIENT_BLOCK_NUM_CL5KJ_BLOCK_SIZE2	((DEFAULT_INGREDIENT_TEXT_SIZE - 2) * 4)  //�ι�° �������� ���� (600byte)

#define INGREDIENT_BASIC_AREA_QTY	100l   //SG070725.TEST
#define INGREDIENT_EXTER_AREA_QTY	0l
#if defined USE_DMSG_1050_BYTE
#define INGREDIENT_TEXT_BASIC_AREA_QTY	1500l
#define INGREDIENT_TEXT_EXTER_AREA_QTY	7500l
#elif defined USE_DMSG_2100_BYTE	//�޸� �������� 1000 + 7000 = 8000 ���� ����
#define INGREDIENT_TEXT_BASIC_AREA_QTY	1000l
#define INGREDIENT_TEXT_EXTER_AREA_QTY	7000l
#endif

#define MAX_PLU_ADDRESS 10000	// sorting table qty(PLU�Ѱ��� ������ �ʿ��� ��� NVRAM Address Table�����ʿ�)
								// Max 16000�� ���� ����(NVRAM_PLU_TABLE����)

//Added by JJANG 2080124
#define PLU_REPORT_QTY_1	10000	//para 625 : 0, PLU ����(6000+4000)�� PLU report ����(6000+8000) �� ���� ��
#define PLU_REPORT_QTY_2	10000	//para 625 : 1, PLU ����(6000+4000)�� PLU report ����(6000+8000) �� ���� ��

#endif

/********************************** ~8M ���� **********************************/
#include "initial_8m_common.h"

#endif	// of #if (ROM_COUNTRY == ROM_RUS)

