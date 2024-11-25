
///////////////////////////////////
// Flash Data Version
//  [ver]: [date]. [description] (programmer)
//  15 : Standard Version
//  16 : Russia Version
//  17 : 2008.09.04. GS Mart Version. Direct Ingredient 추가 (CJK)
//  18 : 2009.08.26. NH Mart(KOR) Version. 개체데이터 999개, 묶음 도축장 추가 (CJK)
//                   - 농협전용에만 추가됨 "/WorkBranch/V1.76.0_KOR(NHMart)"
//  21 : 2009.06.22. Chinese Font (goldman)
//  22 : 2010.11.18. Chinese Font & GS Mart PLU Struct For Chinese E-Mart. (CJK)
//  23 : 2012.03.07. Chinese Standart Market(표준화시장) Cart Scale (CJK)
//
// NVRAM Data Version
//  [ver]: [date]. [description] (programmer)
//  103 : Standard Version
//  113 : Russia Version
//  203 : Korea Meat Version. 개체번호 기억공간 사용 (CJK)
//
#if (ROM_COUNTRY == ROM_RUS)
#include "initial_russia.h"	//1.6
#elif defined(USE_CHN_CART_SCALE)
#include "initial_chn_cartscale.h"	//2.3
#elif (defined(USE_CHN_FONT) && defined(USE_GSMART_PLU))
#include "initial_chn_emart.h"	//2.2
#elif (defined(USE_GSMART_PLU) && defined(USE_KOR_TRACE_999))	// 농협
#include "initial_nhmart.h"		//1.8
#elif defined(USE_GSMART_PLU)
#include "initial_gsmart_plu.h"	//1.7
#elif defined(USE_CHN_FONT)
#include "initial_chn_font.h"	//2.1
#else
#include "initial_standard.h"	//1.5
#endif


#ifndef _FUNC_DISABLE
#define _FUNC_DISABLE
#define FUNC_CLERK		0x0001
#define FUNC_DISCOUNT	0x0002
#define FUNC_TAX_TBL	0x0004
#define FUNC_ORG_TBL	0x0008
#define FUNC_TARE_TBL	0x0010
#define FUNC_NUT_TBL	0x0020
#define FUNC_TRACE		0x0040
#define FUNC_CUSTOM		0x0080
#define FUNC_CURRENCY	0x0100
#define FUNC_DEPTNO		0x0200
#define FUNC_GROUP		0x0400
#define FUNC_TRACE_KOR		0x0800
#define FUNC_ALL_ENABLE 0x0FFF
#endif

#ifndef _CLEAR_NVRAM
#define _CLEAR_NVRAM
#define CLEAR_CAL		0x43
#define CLEAR_VERBOSE	0x40
#define CLEAR_CHECK		0x80
#define CLEAR_NVRAM    	0x01
#define CLEAR_REPORT   	0x02
#define CLEAR_ALL		0x03
#define CLEAR_BOOT		0x83
#endif

#ifndef _VERIFY_CODE
#define _VERIFY_CODE
#define VERIFY_TARE     0x01
#define VERIFY_FUNC		0x02
#define VERIFY_PCS      0x04
#define VERIFY_BYCOUNT  0x08
#define VERIFY_ALL      0xff
#endif

#ifndef _MEMORY_MAP_DIRECT_PNT
#define _MEMORY_MAP_DIRECT_PNT
extern INT32U FLASH_PLU_AREA_PNT;
#endif


// Size Information
#define MAX_SPEEDKEY_QTY			160
#define MAX_SPEEDKEY_SIZE			(MAX_SPEEDKEY_QTY*4)//640
//#define COUNTRY_STRUCT_SIZE			32
//#define TRACEABILITY_STRUCT_SIZE	32
#define COUNTRY_STRUCT_SIZE			20	// Modified by CJK 20060412
#define TRACEABILITY_STRUCT_SIZE	28	// Modified by CJK 20060412
//#define GLOBAL_DISPLAY_INDICATOR_NO	16	// CJK080220

#define GLOBAL_DISPLAY_INDICATOR_NO	26	// 26이상일 경우 FLASH MEMORY MAP 수정해야함


#define LABEL_CAP_STRUCT_SIZE		32
// #define PLU_STRUCT_SIZE	48
#define PRINT_FORMAT_SIZE			54
#define TARE_STRUCT_SIZE			24

#ifdef USE_EXPAND_ADVEMSG_STR
	#define	PRT_ADVE_STRUCT_SIZE		102		// 광고문구 100byte 확장(나머지 2byte는 id)
#else
	#define	PRT_ADVE_STRUCT_SIZE		32		// 기존 광고문구 30byte(나머지 2byte는 id)
#endif

#ifdef	USE_BARCODE_LEN_60
#define BARCODE_STRUCT_SIZE			74
#elif	defined (USE_BARCODE_LEN_70)
#define BARCODE_STRUCT_SIZE			84
#elif	defined (USE_BARCODE_LEN_50)
#define BARCODE_STRUCT_SIZE			64
#elif	defined (USE_BARCODE_LEN_90)
#define BARCODE_STRUCT_SIZE			114
#elif 	defined	(USE_BARCODE_LEN_50_DES_20)
#define BARCODE_STRUCT_SIZE			74
#else
#define BARCODE_STRUCT_SIZE			54
#endif
#ifdef USE_FIXED_PERCENT_KEY_SET
#define DISCOUNT_KEY_STRUCT_SIZE	2
#endif
#define	DEPARTMENT_STRUCT_SIZE	 	32
#define PCS_TABLE_SIZE				144	//18*8
#ifdef USE_TRACE_STANDALONE
 #define TRACE_SPECIALUSE_STRUCT_SIZE	52
 #ifdef USE_SLAUGHTERHOUSE_LENG_100
 #define TRACE_SLAUGHTERHOUSE_KR_STRUCT_SIZE	104
 #else
 #define TRACE_SLAUGHTERHOUSE_KR_STRUCT_SIZE	52
 #endif
 #define TRACE_MEATPART_STRUCT_SIZE	52
#endif


#define EXT_MEMCARD_USE_VOLUME	0x200000

#define MAX_INGREDIENT_SIZE 2048
#define MIN_INGREDIENT_SIZE 200


#define PLU_BASIC_AREA_SIZE	(PLU_BASIC_AREA_QTY * MAX_PLU_RECORDSIZE_DEFAULT)
#define PLU_EXTER_AREA_SIZE	(PLU_EXTER_AREA_QTY * MAX_PLU_RECORDSIZE_DEFAULT)
#define INGREDIENT_BASIC_AREA_SIZE	(INGREDIENT_BASIC_AREA_QTY * DEFAULT_INGREDIENT_SIZE)
#define INGREDIENT_EXTER_AREA_SIZE	(INGREDIENT_EXTER_AREA_QTY * DEFAULT_INGREDIENT_SIZE)
#define INGREDIENT_TEXT_BASIC_AREA_SIZE	(INGREDIENT_BASIC_AREA_QTY * DEFAULT_INGREDIENT_TEXT_SIZE)
#define INGREDIENT_TEXT_EXTER_AREA_SIZE	(INGREDIENT_EXTER_AREA_QTY * DEFAULT_INGREDIENT_TEXT_SIZE)


// ========================================================
// FLASH_ADC_AREA
// ========================================================

#define	GLOBAL_AD_CAL_ZERO_AD	0
#define	GLOBAL_AD_CAL_SPAN_AD	4
#define	GLOBAL_AD_CAL_ZERO_MIN	8
#define	GLOBAL_AD_CAL_SPAN_MIN	12
#define	GLOBAL_AD_CAL_ZERO_MAX	16
#define	GLOBAL_AD_CAL_SPAN_MAX	20
#define	GLOBAL_AD_GRAVITY_FACTORY	24
#define	GLOBAL_AD_GRAVITY_LOCAL	26
#define	GLOBAL_AD_TARE_LIMIT	28
#define	GLOBAL_AD_OVER_RANGE	32
#define	GLOBAL_AD_INIT_ZERO_RANGE	33
#define	GLOBAL_AD_REZERO_RANGE	34
#define	GLOBAL_AD_TARE_ZERO	35
#define	GLOBAL_AD_CURVE_AD_ZERO	36
#define	GLOBAL_AD_CURVE_AD_MID	40
#define	GLOBAL_AD_CURVE_AD_SPAN	44
#define	GLOBAL_AD_CURVE_RATIO	48
#define	GLOBAL_AD_CHARGE_TIME	50
#define	GLOBAL_AD_RECHARGE_TIME	51
#define	GLOBAL_AD_MOTION_BAND	52
#define	GLOBAL_AD_FILT_ON_BAND	53
#define	GLOBAL_AD_FILT_OFF_BAND	54
#define	GLOBAL_AD_FILT_BUF_SIZE	55
#define	GLOBAL_AD_TIMER_PERIOD	56
#define	GLOBAL_AD_UPDATE_1	58
#define	GLOBAL_AD_UPDATE_2	59
#define	GLOBAL_AD_UPDATE_3	60
#define	GLOBAL_AD_MIX_1	61
#define	GLOBAL_AD_MIX_2	62

#define GLOBAL_AD_RESERVE1	63
#define GLOBAL_AD_HYST_AD_ZERO	64
#define GLOBAL_AD_HYST_AD_MID_UP	68
#define GLOBAL_AD_HYST_AD_MID_DN	72
#define GLOBAL_AD_HYST_AD_SPAN	76
#define GLOBAL_AD_CREEP_ENABLE	80
#define GLOBAL_AD_TARE_LIMIT_LB	82	// Added by CJK 20060223
#define GLOBAL_AD_STRUCT_VER	86
#define GLOBAL_AD_CS1	88





// ==================================================
// Global Area Define
#define GLOBAL_AREA_SCALETYPE	0
#define GLOBAL_AREA_VENDOR_COUNTRY	1
#define GLOBAL_AREA_VENDOR_VENDOR	3
#define GLOBAL_AREA_VENDOR_TZ	5
#define GLOBAL_AREA_VENDOR_CONTINENT	7
#define GLOBAL_DISPLAY_WEIGHTPOS	8
#define GLOBAL_DISPLAY_WEIGHTLENGTH	9
#define GLOBAL_DISPLAY_WEIGHTDECIMAL	10
#define GLOBAL_DISPLAY_UNITPRICEPOS	11
#define GLOBAL_DISPLAY_UNITPRICELENGTH	12
#define GLOBAL_DISPLAY_PRICEPOS	13
#define GLOBAL_DISPLAY_PRICELENGTH	14
#define GLOBAL_DISPLAY_TAREPOS	15
#define GLOBAL_DISPLAY_TAREY	16
#define GLOBAL_DISPLAY_TARELENGTH	17
#define GLOBAL_DISPLAY_DATETIME_X	18
#define GLOBAL_DISPLAY_DATETIME_Y	19
#define GLOBAL_DISPLAY_DATETIME_MODE	20
#define GLOBAL_DISPLAY_INDICATORSIGN	21
#define GLOBAL_INPUT_BC_QTYLENGTH	47
#define GLOBAL_INPUT_RESERVED2	55
#define GLOBAL_INPUT_BC_PCSLENGTH	63
//#define GLOBAL_INPUT_RESERVED3	71
#define GLOBAL_QR_CFG_OPEN_CMD	71
#define GLOBAL_QR_CFG_POS_ID	72
#define GLOBAL_QR_CFG_EAN_PREFIX	74
#define GLOBAL_QR_CFG_EAN_SUFFIX	76
#define GLOBAL_QR_CFG_CLOSE_CMD	78
#define GLOBAL_QR_CFG_DEL_LAST_SUFFIX	79
#define GLOBAL_INPUT_RESERVED4	80
#define GLOBAL_INPUT_RESERVED5	87
#define GLOBAL_INPUT_BN_QTYLENGTH	95
#define GLOBAL_INPUT_RESERVED6	103
#define GLOBAL_DISCOUNT_LABEL	109
#define GLOBAL_SALE_SET_LABEL	110
//#define GLOBAL_INPUT_RESERVED7	111
#define GLOBAL_LABEL_MEMBER_PRICE   111
#define GLOBAL_SALE_SETUP_FLAG21 116
#define GLOBAL_SALE_SETUP_FLAG20 117
#define GLOBAL_SALE_SETUP_FLAG18 118
#define GLOBAL_MENU_FONTID	119
#define GLOBAL_PLU_FONTID	120
#define GLOBAL_LABEL_FONTID	121
#define GLOBAL_SALE_SETUP_FLAG8	122
#define GLOBAL_TARE_SETITING	123
#define GLOBAL_SALE_SETUP_FLAG9	124
#define GLOBAL_SALE_SETUP_FLAG10	125
#define GLOBAL_SALE_SETUP_FLAG11	126
#define GLOBAL_DUAL_PRINT_CNT	127
#define GLOBAL_PLU_TYPE_AREA	128
#define GLOBAL_DISPLAY_PRICESIGN	132
#define GLOBAL_DISPLAY_WEIGHTSIGN	133
#define GLOBAL_DATE_MONTHARRAY	134
#define GLOBAL_DATE_SEPARATOR	254
#define GLOBAL_DATE_WEEKARRAY	258
#define GLOBAL_TIME_SEPARATOR	328
#define GLOBAL_DATE_AMPMARRAY	332
#define GLOBAL_AREA_PRICE_PRIMARYSIGN	342
#define GLOBAL_AREA_PRICE_LASTSIGN	346
#define GLOBAL_WEIGHT_SYMBOL_1	350
#define GLOBAL_WEIGHT_SYMBOL_2	354
#define GLOBAL_WEIGHT_LBSYMBOL_1	358
#define GLOBAL_WEIGHT_LBSYMBOL_2	362
#define GLOBAL_PCS_SYMBOL	366
#define GLOBAL_DECIMAL_SIGN	383
#define GLOBAL_QUESTION_YES	384
#define GLOBAL_QUESTION_NO	388
#define GLOBAL_DISPLAY_PRICE_PRIMARYSIGN	392
#define GLOBAL_DISPLAY_PRICE_LASTSIGN	396
#define GLOBAL_FUNCTION_KEY_BLOCK	400
#define GLOBAL_SALE_SETUP_FLAG17 403
#define GLOBAL_SALE_SETUP_FLAG15 405	// For saving Korean Traceability option : 전체개체 상품 개체 정보 저장 공간 (2bit)
#define GLOBAL_SALE_SETUP_FLAG14 406	// For Belarus PLU Scroll , Print Sell by time on/off LYH 20140213
#define GLOBAL_SALE_SETUP_FLAG13 407
#define GLOBAL_SALE_SETUP_FLAG12 408
#define GLOBAL_PRICESIGN_1000	409
#define GLOBAL_BARCODE_2	410
#define GLOBAL_LABEL_2	411
#define GLOBAL_OPERATION_MODE	412
#define GLOBAL_SALERUN_MODE	413
#define GLOBAL_AREA_SALEMODE	414
#define GLOBAL_DISPLAY_BRIGHT	415
#define GLOBAL_PAPER_TYPE	416
#define GLOBAL_SALE_SETUP_FLAG6 	417 	//SG060419
#define GLOBAL_SALE_SETUP_HOLDTIME 	418
#define GLOBAL_SALE_SETUP_FLAG7		419		//JJANG 20071112
#define GLOBAL_AREA_RS232C_BAUD	420
#define GLOBAL_AREA_COMMUNICATION	428
#define GLOBAL_SALE_ONLINE_LABEL	430
#define GLOBAL_REALTIME_CHECK_INTERVAL	431
#define GLOBAL_SALE_SETUP_FLAG19	433
#define GLOBAL_CTS_MSG	434
#define GLOBAL_AREA_USB	456
#define GLOBAL_CASHDRAWER_SENSOR	460
#define GLOBAL_TIME_CASHDRAWEROPEN	461
#define GLOBAL_DUAL_CURRENCY	463
#define GLOBAL_TAREINPUT_FLAG	464
#define GLOBAL_KEY_TIMEOUT_ERROR	465
#define GLOBAL_KEY_TIMEOUT_KEYPRESS	466
#define GLOBAL_KEY_TIMEOUT_ACTION	467
#define GLOBAL_KEY_NUMBERZERO_N	469
#define GLOBAL_SALE_SETUP_FLAG3	470
#define GLOBAL_AREA_TAREFLAG	471
#define GLOBAL_AREA_MULTIUNITFLAG	472
#define GLOBAL_AREA_PRICE_TYPE	473
#define GLOBAL_ACTION_ENABLE	474
#define GLOBAL_PRICE_ZERO_FLAG	476
#define GLOBAL_SALEDATE_TYPE	477
#define GLOBAL_SALE_SETUP_FLAG4	478
#define GLOBAL_TIME_KEYCLEAR	479
#define GLOBAL_SALE_SETUP_FLAG5	480
#define GLOBAL_BARCODE_CUT		481
#define GLOBAL_CHANGE_MODE1	482

#define GLOBAL_REPORT_FLAG	483
#define GLOBAL_REPORT_FLAG1	484
#define GLOBAL_REPORT_FLAG2	485
#define GLOBAL_DEFAULTUNIT	486
#define GLOBAL_BARCODE_PRICE_CUT	488
#define GLOBAL_SALE_SETUP_FLAG1	489
#define GLOBAL_SALE_SETUP_FLAG2	490
#define GLOBAL_PRINT_OPER_FLAG	491
#define GLOBAL_PRINT_OPER_FLAG2	492
#define GLOBAL_BARCODE_WEIGHT_CUT	493
#define GLOBAL_WEIGHT_ROUND	494
#define GLOBAL_START_FLAG	498
#define GLOBAL_DATETIME_MODE	500
#define GLOBAL_AREA_PRICE_ROUND	501
#define GLOBAL_PRICE_ROUND_APPLY	503
#define GLOBAL_AREA_PRICE_DECIMALPOINT	504
#define GLOBAL_ERROR_TIMEOUT	505
#define GLOBAL_SALE_MINIMUMWEIGHT	506
#define GLOBAL_SALE_AUTO_DELTAWEIGHT	510
#define GLOBAL_LABEL_MARKDOWNFLAG	514
#define GLOBAL_TAX_FLAG	515
#define GLOBAL_DISCOUNT_PRIORITY	516
#define GLOBAL_LABEL_PRIORITY	517
#define GLOBAL_LABEL_STANDARD	518
#define GLOBAL_LABEL_MISC	520
#define GLOBAL_LABEL_ADDUP	522
#define GLOBAL_BARCODE_PRIORITY	524
#define GLOBAL_TIME_CLERKLOGOUT	525
#define GLOBAL_DISPLAY_ALIGN	527
#define GLOBAL_LOG_FLAG	528
#define GLOBAL_LABEL_FLAG	529
#define GLOBAL_SALE_SAVE_FLAG	530
#define GLOBAL_DISPLAY_INFO_FLAG	531
#define GLOBAL_PRINT_CLERK_FLAG1	532
#define GLOBAL_PRINT_CLERK_FLAG2	533
#define GLOBAL_PRINT_ADDUP_FLAG2	534
#define GLOBAL_AREA_BONUSFLAG	535
#define GLOBAL_AREA_CUSTOMERFLAG	536
#define GLOBAL_TAX_VALUE	537
#define GLOBAL_MISC_TAX_VALUE	538				//Added by JJANG 20070613
#define GLOBAL_BARCODE_STD_WID	539
#define GLOBAL_BARCODE_STD_CID	540
#define GLOBAL_BARCODE_STD_NID	541
#define GLOBAL_BARCODE_STD_FIX	542
#define GLOBAL_BARCODE_NON_WID	543
#define GLOBAL_BARCODE_NON_CID	544
#define GLOBAL_BARCODE_NON_NID	545
#define GLOBAL_BARCODE_TOTAL_ID	546
#define GLOBAL_BARCODE_TOTAL_ID2	547
#define GLOBAL_BARCODE_TR_ID	548
#define GLOBAL_LABEL_MESSAGEID	549
#define GLOBAL_USE_REPORT	550
#define GLOBAL_PRINT_ADDUP_FLAG1	551
#define GLOBAL_TICKET_MESSAGE_HEAD	552
#define GLOBAL_TICKET_MESSAGE_TAIL	554
#define GLOBAL_ADDUP_MESSAGE_HEAD	556
#define GLOBAL_ADDUP_MESSAGE_TAIL	558
#define GLOBAL_SALE_AUTO_STATE	560
#define GLOBAL_BONUS_PRICE_1STTARGET	561
#define GLOBAL_BONUS_PRICE_2NDTARGET	565
#define GLOBAL_BONUS_WEIGHT_1STTARGET	569
#define GLOBAL_BONUS_WEIGHT_2NDTARGET	573
#define GLOBAL_BONUS_SPECIAL_1STTARGET	577
#define GLOBAL_BONUS_SPECIAL_2NDTARGET	581
#define GLOBAL_LABEL_SERIAL_FORM	585
#define GLOBAL_TICKET_SERIAL_FORM	617
#define GLOBAL_PRT_INHIBIT	649
#define GLOBAL_SCANNER_FLAG1	681
#define GLOBAL_SCANNER_FORMAT	682
#define GLOBAL_SCROLL_WAIT_TIME	714
#define GLOBAL_SCROLL_SPEED	716
#define GLOBAL_LABEL_FSP 717
// CJK070205
#define GLOBAL_PRT_PAPER_WIDTH	719
#define GLOBAL_PRT_PAPER_HEIGHT	725
#define GLOBAL_PRT_PAPER_ENDMARGIN	731
#define GLOBAL_PRT_SENSOR_MOTOR	737
#define GLOBAL_PRT_TICKET_FONT_SIZE 740
#define GLOBAL_PRT_HEADUP_SENSOR	741

//#endif

#ifndef _MEMORY_MAP_DEFINE_PLU_STRUCT
#define _MEMORY_MAP_DEFINE_PLU_STRUCT
#define PLU_STRUCT_ID			0
#define PLU_STRUCT_NAME			2
#define PLU_STRUCT_REPORT_TYPE	22
#define PLU_STRUCT_VERSION	25	// Added by CJK 20060415
#define PLU_STRUCT_FIELDNO		26
#define PLU_STRUCT_RECORD_SIZE	28
#define PLU_STRUCT_POINT	30
#endif

#ifndef _HISTORY_STRUCT_SIZE_DEF
#define HISTORY_STRUCT_SIZE 30
#define HISTORY_QTY	        4
#endif

#ifndef _MEMORY_MAP_DEFINE_FONT_STRUCT
#define _MEMORY_MAP_DEFINE_FONT_STRUCT
//#define FONT_NUMBER	0
//#define FONT_IMAGE_SIZE	2
//#define FONT_INFO_POINT	6
//#define FONT_INFO_SIZE	6
//#define FONT_IMAGE	54
//#define FONT_MAX_SIZE	458752
#define FONT_NUMBER	0
#define FONT_IMAGE_SIZE	2
#define FONT_INFO_POINT	6
#define FONT_INFO_SIZE	6
#define FONT_IMAGE	78
#define FONT_MAX_SIZE	204800
#define FONT_MAX_NO     12
#endif

#ifndef _MEMORY_MAP_DEFINE_LABEL_STRUCT
#define _MEMORY_MAP_DEFINE_LABEL_STRUCT
#define LABEL_INFO_POINT	0
#define LABEL_INFO_SIZE		24   // 라벨 헤더 크기
#define LABEL_HEADER_SIZE	LABEL_INFO_SIZE 	// 라벨 헤더 크기
#define LABEL_IMAGE			480  // 라벨 헤더 전체 크기 (24 byte * 20 개)
#define LABEL_MAX_SIZE		4096 // 라벨 이미지 크기
#define LABEL_IMAGE_SIZE	LABEL_MAX_SIZE	// LABEL_MAX_SIZE 의미가 불분명하여 정의하였음

#define LABEL_CUSTOM_2MB_NUM			20 		// 기존 2MB 영역에서 사용자 정의 라벨 총 개수

// 라벨 확장 영역(4~8MB) 주소 계산을 위한 define
#ifdef USE_LABEL_FORMAT_EXT
  #define LABEL_CUSTOM_EXT_NUM		20  // ~ 8MB 확장 영역에서 사용자 정의 라벨 총 개수
  // 라벨 확장 영역(4~8MB) 주소 정의
  #define FLASH_LABEL_FORMAT_EXT_OFFSET			3431405 // 라벨 확장 영역 오프셋
#else
  #define LABEL_CUSTOM_EXT_NUM	 	0  // ~ 8MB 확장 영역에서 사용자 정의 라벨 총 개수
#endif /* USE_LABEL_FORMAT_EXT */

#define LABEL_CUSTOM_TOTAL_NUM \
	(LABEL_CUSTOM_2MB_NUM + LABEL_CUSTOM_EXT_NUM)	// 라벨 총 개수

#endif

//#ifndef _MEMORY_MAP_DEFINE_PRINT_STRUCT
//#define _MEMORY_MAP_DEFINE_PRINT_STRUCT
//#define PRT_FONT_MENUID	251
//#define PRT_FONT_PLUID	252
//#define PRT_FONT_LABELID	253
// #define PRT_LABEL_DEFAULTID	254
//#endif

#ifndef _MEMORY_MAP_DEFINE_BITMAP_STRUCT
#define _MEMORY_MAP_DEFINE_BITMAP_STRUCT
#define BITMAP_INFO_POINT	0
#define BITMAP_INFO_SIZE	6
#define BITMAP_IMAGE	84
#define BITMAP_MAX_SIZE	8192
#endif

#ifndef _MEMORY_MAP_DEFINE_NETWORK_STRUCT
#define _MEMORY_MAP_DEFINE_NETWORK_STRUCT
#define NETWORK_GLOBAL_TYPE	0
#define NETWORK_SERVICE_TYPE	1
#define NETWORK_SCALE_ID	2
#define NETWORK_SCALE_NAME	4
#define NETWORK_MAX_CONNECTION	24
#define NETWORK_DEPARTMENT_ID	26
#define NETWORK_CLERK_ID	28
#define NETWORK_CLERK_CHANGEOPTION	30
#define NETWORK_SERVICE_FLAG	32
#define NETWORK_STORE_ID	34
#define NETWORK_LOCAL_IP	36
#define NETWORK_LOCAL_GATEWAY	40
#define NETWORK_LOCAL_SUBNETMASK	44
#define NETWORK_LOCAL_ETHERNET_FLAG	48
#define NETWORK_LOCAL_TCP_PORT	50
#define NETWORK_LOCAL_UDP_PORT	52
#define NETWORK_MACHINE_LIST_POINT	54
#define NETWORK_MACHINE_REMOTE_IP	56
#define NETWORK_MACHINE_REMOTE_TCPPORT	60
#define NETWORK_MACHINE_REMOTE_UDPPORT	62
#define NETWORK_MACHINE_REMOTE_STATUS	64
#define NETWORK_WLAN_SSID	66
#define NETWORK_WLAN_WEP	100
#define NETWORK_WLAN_WEPID	164
#define NETWORK_WLAN_USEWEP	165
#define NETWORK_WLAN_ETC	166
#define NETWORK_SERVICE_FLAG2	175
#define NETWORK_DIVISION_ID	176		//-[2006.03.06 bmlim 분류번호
#define NETWORK_SERVICE_FLAG3	178	//SG061228. Backup to scale commun method
#define NETWORK_RESERVED	179

#endif

#ifndef _MEMORY_MAP_DEFINE_DNS_STRUCT
#define _MEMORY_MAP_DEFINE_DNS_STRUCT
#define NETWORK_REMOTE_URL		80
#define NETWORK_REMOTE_URL_MAX_SIZE		60
#define NETWORK_DNS_IP			(NETWORK_REMOTE_URL + NETWORK_REMOTE_URL_MAX_SIZE)
#define NETWORK_DNS_IP_SIZE		4
#define NETWORK_DNS_PORT		(NETWORK_DNS_IP + NETWORK_DNS_IP_SIZE)
#define NETWORK_DNS_PORT_SIZE	2
#endif

#ifndef __STARTUP_STRUCTURE
#define __STARTUP_STRUCTURE
#pragma pack(4)
typedef struct {
	INT8U   version[2];
////	INT8U   fwversion[3];
	INT16U	start_flag;
	INT8U	global_com;
	INT32U	start_addr;
	INT8U	slot_adc;
	INT32U	addr_adc;
	INT16U  number_scale;
	INT8U   menu_type;
	INT8U   scale_type;
	INT8U   line_max;
	INT16U  country;
	INT16S     menu_xgap,menu_ygap;
	INT16S     menu_length;
	INT8U   disable_dept;
	INT16U  scroll_timeout;
	INT8U   ethernet_notready;
	INT32U	addr_send_buf;
	INT32U	addr_paysend_buf;
	INT16U	max_send_buf;
	INT16U	max_paysend_buf;
	INT16U	max_realsend_buf;
	INT8U	use_ext_nvram;
	INT8U   wireless_type;
	INT8U   wireless_model;
} STARTUP_CONFIG;
#pragma pack()
#endif

#ifndef __CTS_STRUCTURE
#define __CTS_STRUCTURE
typedef struct {
	INT8U  CtsEnable;    // 0
	INT8U  CtsVersion;    // 1
	INT8U  SerialNo[16]; // 2~17
	INT8U  Initialized;  // 18 // CTS 초기화 유무 판단
	INT8U  CtsExpired;  // 19
	INT16U  CtsPeriod;  // 20
	INT8U  CtsCalYear;	// 22
	INT8U  CtsCalMonth;	// 23
	INT8U  CtsCalDate;	// 24
	INT8U  reserve[5];	// 25~29
	INT16U crc16;		// 30
} CTS_STRUCT;
typedef struct {
	INT8U  ProductCode[16];		// 0
	INT32U EncryptKey1;			// 16
	INT32U BoardId;				// 20
	INT8U  reserve[2];			// 24~25
	INT16U crc16;				// 26
} CTS_STRUCT2;	// History 영역 30바이트 중 pack(4)의 영향 고려하여 28바이트만 할당
typedef struct {
	INT32U zero;
	INT32U span;
	INT32U time;
	INT32U midup;
	INT32U middn;
} CTS_ENCRYPT_BUF_STRUCT;
// 기존에 cal history 기능에서 저장된 날짜 기준이었던 것을, CAL Comp 시 저장된 날짜를 기준으로 판단하도록 변경하여 구조체에 CAL date 추가
// CAL Password에서 사용하던 공간 통합
#define CTS_STRUCT_SIZE  sizeof(CTS_STRUCT)
#define CTS_STRUCT_SIZE2  sizeof(CTS_STRUCT2)
#define CTS_INIT_COMPLETED 0x83
//#define CTS_INIT_COMPLETED 0x8B	// Version 2 테스트롬을 베트남 지사에 테스트롬 보내기 직전 버전 2를 버전 1으로 돌렸다 다시 버전2로 업할 경우 생기는 버그 때문에 임시로 플래그를 변경. 버그 수정 후 원래대로 수정할 예정 20180402 KSY
//#define CTS_VERSION 	0x01	// 생산 적용 시 최초 적용
#define CTS_VERSION 	0x02	// CTS Struct2(ProductCode) 추가, BoardId, 암호화키 추가

#endif

#ifndef __FONT_ID_DEFINE
#define __FONT_ID_DEFINE
extern INT8U DSP_MENU_FONT_ID;
extern INT8U DSP_PLU_FONT_ID;
extern INT8U PRT_DEFAULT_FONT_ID;
#endif

//#define MAX_PLU_NO			1000	// INPUT NO Reference
extern INT32U  MAX_PLU_NO,MAX_PLU_large,MAX_PLU_small;
#ifdef USE_EXTEND_8M_DFLASH
extern INT32U MAX_PLU_extend8M;
#endif

#ifndef CLP_MAX_NUMBER_
#define CLP_MAX_NUMBER_
#define MAX_DEPT_NO			100		// INPUT 1~99
#define MAX_ORIGIN_NO		500		// INPUT 1~500

/* Discount */
#ifndef DISCOUNT_MAX_NUM_200
  #define DISCOUNT_QTY_2M		100		// 2M까지의 DISCOUNT 개수
  #ifdef USE_DISCOUNT_QTY_EXT
    #define DISCOUNT_QTY_8M		2400	// 8M까지의 DISCOUNT 개수
    #define MAX_DISCOUNT_NO		(DISCOUNT_QTY_2M + DISCOUNT_QTY_8M + 1)	// INPUT 1~2500
  #else
    #define DISCOUNT_QTY_8M		0		// 8M까지의 DISCOUNT 개수
    #define MAX_DISCOUNT_NO		(DISCOUNT_QTY_2M + DISCOUNT_QTY_8M)     // INPUT 1~99
  #endif /* USE_DISCOUNT_QTY_EXT */
#else
  #define DISCOUNT_QTY_2M		200					// 2M까지의 DISCOUNT 개수
  #define MAX_DISCOUNT_NO		DISCOUNT_QTY_2M		// No Reference
#endif /* DISCOUNT_MAX_NUM_200 */


#define MAX_CLERK_NO    	96		// INPUT 1~95
#define MAX_LABEL_CAP_NO	200
#ifdef USE_EXTEND_CAPNTION_AREA
#define MAX_CAPTION_NO		1400
#define MID_CAPTION_NO		1200
#else
#define MAX_CAPTION_NO		1200
#endif
#define MAX_TAX_NO      	10		// INPUT 1~9		// id=10(room no. 9)은 Tax free

#ifdef USE_EXPAND_ADVEMSG_STR
	#define MAX_LABEL_MSG_NO    31		// INPUT 1~30
#else
	#define MAX_LABEL_MSG_NO    100		// INPUT 1~99
#endif

#ifdef USE_KOR_TRACE_999
#define MAX_NUTRIFACT_NO	100		//parameter 548 : 0,1,2,4,6
#define MAX_NUTRIFACT2_NO	50		//parameter 548 : 3,5
#ifdef USE_EU_NUTRITIONFACT
#define MAX_NUTRIFACT3_NO	40		//parameter 548 : 8
#endif
#else
#define MAX_NUTRIFACT_NO	500		//parameter 548 : 0,1,2,4,6 // INTPUT 1~500
#define MAX_NUTRIFACT2_NO	350		//parameter 548 : 3,5
#ifdef USE_EU_NUTRITIONFACT
#define MAX_NUTRIFACT3_NO	200		//parameter 548 : 8
#endif
  #ifdef USE_ADDITIONAL_NUTRIENT
    #ifdef USE_DIRECT_NUTRITION
#define MAX_NUTRIFACT4_NO	MAX_PLU_ADDRESS		// PLU 개수만큼 Nutrition Fact를 매칭 
    #else
#define MAX_NUTRIFACT4_NO	220		//parameter 548 : 2,9 // INTPUT 1~220
    #endif
  #endif // USE_ADDITIONAL_NUTRIENT
#endif
#define MAX_TRACEABILITY_NO 1000
#define MAX_COUNTRY_NO		1000		// 100 -> 1000 : Modified by CJK 20060415
#define MAX_SLAUGHTERHOUSE_NO	1000	// 100 -> 1000 : Modified by CJK 20060415
#define MAX_CUTTINGHALL_NO 1000			// 100 -> 1000 : Modified by CJK 20060415
#define MAX_SPECIALUSE_NO	100		// 1~99
#define MAX_MEATPART_NO	100			// 1~99
#ifdef USE_FIXED_PERCENT_KEY_SET
#define MAX_DISCOUNT_KEY_NO	5		// INPUT 1~5
#endif
#ifdef CPU_PIC32MZ
#if defined (CL5200_PROJECT) || defined (CL5500_PROJECT)
#if defined(USE_BARCODE_LEN_60) || defined(USE_BARCODE_LEN_70)	
#define MAX_BARCODE_NO	59
#elif defined (USE_BARCODE_LEN_50)
#define MAX_BARCODE_NO	79
#elif defined (USE_BARCODE_LEN_90)
#define MAX_BARCODE_NO	39
#else
#define MAX_BARCODE_NO	99		//INPUT 1~99
#endif
#else  //  CL5200_PROJECT
#ifdef	USE_BARCODE_LEN_60
#define MAX_BARCODE_NO	20
#elif	defined (USE_BARCODE_LEN_50)
#define MAX_BARCODE_NO	20
#else
#define MAX_BARCODE_NO	20		//INPUT 1~20
#endif
#endif  //  CL5200_PROJECT
#else
#if defined(USE_BARCODE_LEN_60) || defined(USE_BARCODE_LEN_70)
#define MAX_BARCODE_NO	59		// id = 1.. 59
#elif	defined (USE_BARCODE_LEN_50)
#define MAX_BARCODE_NO	79		// id = 1.. 79
#else
#define MAX_BARCODE_NO	99		// id = 1.. 99
#endif
#endif
#define MAX_CUSTOMER_NO	89		//INPUT 1~89
#define MAX_TARE_NO		99		//INPUT 1~99
#define MAX_STORE_NO	10		//INPUT 1~10

#ifdef USE_KOR_SLAUGHT_CODE_4DIGIT
#define MAX_SLAUGHTERHOUSE_KR_QTY	200
#define MAX_SLAUGHTERHOUSE_KR_NO	10000
#else
  #ifdef USE_TOPMART_DEFAULT
#define MAX_SLAUGHTERHOUSE_KR_QTY	200     // 1~199
#define MAX_SLAUGHTERHOUSE_KR_NO	200     // 1~199
  #else
#define MAX_SLAUGHTERHOUSE_KR_QTY	100     // 1~99
#define MAX_SLAUGHTERHOUSE_KR_NO	100     // 1~99
  #endif
#endif

#ifdef USE_GSMART_PLU
 #define MAX_PCS_NO			18
#else
 #define MAX_PCS_NO			8		// 1~8
#endif

#define MAX_CURRENCY_NO		4		// 1~4

#ifdef USE_CHN_CART_SCALE
#define MAX_TRANSACTION_SALE_SIZE	82	// TRANSACTION_RECORD_TYPE(56byte)+etc(6byte)+TraceCode(20byte)
#else
#define MAX_TRANSACTION_SALE_SIZE	72	// TRANSACTION_RECORD_TYPE(56byte)+etc(6byte)+Reserve(10byte)
#endif
#if defined(USE_INDIA_FUNCTION) || defined(USE_SEND_TICKET_DATA) || defined(USE_SETPRODUCT_SALE) || defined(USE_HYUNDAI_SET_FUNCTION) 
#define MAX_TRANSACTION_ADD_SIZE	86
#endif
#ifdef USE_TRACE_STANDALONE
 #define MAX_TRANSACTION_TYPE1_SALE 40	// 40 EA X 80 byte = 3200
 #define MAX_TRANSACTION_TYPE1_SALE_SIZE	80	// TRANSACTION_RECORD_TYPE(56) + TRANSACTION_RECORD_SUB_TYPE(22)
#else
 #define MAX_TRANSACTION_TYPE1_SALE 50	// 50 EA X 72 byte = 3600
 #define MAX_TRANSACTION_TYPE1_SALE_SIZE	72	// TRANSACTION_RECORD_TYPE(56byte)
#endif
#define MAX_TRANSACTION_REALTIME_SALE  10  // 10 EA X 360 byte = 3600
#define MAX_TRANSACTION_REALTIME_SALE_SIZE	360
#ifdef USE_EMART_BACK_LABEL
#define MAX_TRANSACTION_TYPE4_SALE 36	// 36 EA X 100 byte = 3600
#define MAX_TRANSACTION_TYPE4_SALE_SIZE	100
#elif defined(USE_INDIA_FUNCTION)
#define MAX_TRANSACTION_TYPE8_SALE 200	// (40-> 200) EA X 90 byte = (3600 -> 18000)
#define MAX_TRANSACTION_TYPE8_SALE_SIZE	90
#elif defined(USE_SEND_TICKET_DATA)
#define MAX_TRANSACTION_TYPE4_SALE 500	// 
#define MAX_TRANSACTION_TYPE4_SALE_SIZE	90
#else
#define MAX_TRANSACTION_TYPE4_SALE 40	// 40 EA X 90 byte = 3600
#define MAX_TRANSACTION_TYPE4_SALE_SIZE	90
#endif
#define MAX_TRANSACTION_TYPE7_SALE 60	// 60 EA X 52 byte = 3120
#define MAX_TRANSACTION_TYPE7_SALE_SIZE	52
#ifdef USE_SEND_TRACE_PRICE
#define MAX_TRANSACTION_TYPE10_SALE 40	// 40 EA X 80 byte = 3200
#define MAX_TRANSACTION_TYPE10_SALE_SIZE	80	
#endif
#ifdef USE_HYUNDAI_MEAT_FUNCTION
#define MAX_TRANSACTION_TYPE14_SALE 40 // 40 EA X 90 byte = 3600
#define MAX_TRANSACTION_TYPE14_SALE_SIZE   90
#endif
#ifdef USE_DISCOUNTED_TAX_FOR_DISCOUNTED_TOTAL
#define MAX_TRANSACTION_PAY  5		// 5 EA X 144 byte
#define MAX_EXT_TRANSACTION_PAY  50
#else
#define MAX_TRANSACTION_PAY  10		// 10 EA X 72 byte
#define MAX_EXT_TRANSACTION_PAY  100
#endif
#define MAX_TRANSACTION_STRUCT_SIZE	56
#ifdef USE_CHN_CART_SCALE
#define MAX_REOPEN_HEAD 6
#else
#define MAX_REOPEN_HEAD 7
#endif
#define MAX_REOPEN_SALE	50

//Added by JJANG 20080313
#define AUDIT_MODE_LINEAR	0
#define AUDIT_MODE_CIRCULAR	1

//Added by JJANG 20080310
#if (ROM_COUNTRY == ROM_RUS)
#define MAX_EXT_TRANSACTION_SALE 500
#define MAX_EXT_TRANSACTION_REALTIME_SALE  100
#else
#define MAX_EXT_TRANSACTION_SALE 2000
#define MAX_EXT_TRANSACTION_REALTIME_SALE  400
#endif

//#define TRANSACTION_REALTIME_SALE_SIZE  360

#if !(ROM_COUNTRY == ROM_RUS)
#define MAX_LABEL_ROM_NO	 45		// Max Stored Label
#else
#define MAX_LABEL_ROM_NO	 42		// Max Stored Label
#endif

#define MAX_SCROLL_NO	9		//INPUT 1~9
//#define EXT_PLU_NO 3000
#define MAX_BITMAP_ID		99
#endif	// of #ifndef CLP_MAX_NUMBER_

//#define USE_EXT_SETTING_MEM	// CJK080313
#define DEALER_PASSWORD_LEN		8

#ifndef CLP_MESSAGE_DTNUMBER_
#define CLP_MESSAGE_DTNUMBER_
#define MSG_DT_SCALE	0
#define MSG_DT_DEPT 	1
#define MSG_DT_PLU		2
#define MSG_DT_CLERK	3
#define MSG_DT_NOPLU	4
#define MSG_DT_NOCLK	5
#endif

#ifndef CLP_MESSAGE_NUMBER_
#define CLP_MESSAGE_NUMBER_
#define MSG_00_MISC_VALUE	0
#define MSG_02_DC			2
#define MSG_03_DC_FIXED		3
#define MSG_04_FREEPLU		4
#define MSG_06_FIXEDPRICE	6
#define MSG_NOT_ADDSUMMARY	7
#define MSG_OPEN_HEAD		8
#define MSG_PEELOFF			9
#define MSG_FIXED_WEIGHT	10
#define MSG_14_C052			11
#define MSG_NO_PLUNAME		12
#define MSG_MISC_WEIGHT		13
#define MSG_MISC_PCS		14
#define MSG_MISC_COUNT		15
#define MSG_19_NET_SFULL	16
#define MSG_20_NET_PFULL	17
#define MSG_21_NET_ERR		18
#define MSG_NO_TOTAL		19
#define MSG_23_C803			20
#define MSG_24_C04C			21
#define MSG_MINWEIGHT		22
#define MSG_26_C804			23
#define MSG_27_C80F			24
#define MSG_NOSALEDATA		25
#define MSG_29_DSP_VOID		26
#define MSG_30_PLU			27
#define MSG_VOID_LIST		29
#define MSG_VOID_LIST_2		30
#define MSG_RETURN			31
#define MSG_TOTALCNT		32
#define MSG_WAR_PLUTARE		33
#define MSG_WAR_KEYTARE		34
#define MSG_WAR_WEIGHTTARE	35
#define MSG_WAR_NOVOID		36
#define MSG_WAR_PERMIT		37
#define MSG_TRANSINFO		38
#define MSG_CURRENCY_CHANGE	39
#define MSG_INVALID_VALUE	40
#define MSG_INPUTXKEY		41
#define MSG_NOT_ALLOW_KGLB	42	// added by CJK 20050930
#define MSG_INVALID_FIXEDWEIGHT	43	// added by CJK 20051027
#define MSG_DC_MINUS		44	// added by CJK 20051027
#define MSG_DC_PERCENT		45	// added by CJK 20051027
#define MSG_CHECK_WEIGHT	46
#define MSG_CLERK_IS_LOCKED     47      // SG061128
#define MSG_NO_MASTER		48	//Added by JJANG 20080307
#define MSG_MEMORY_FULL		49
#define MSG_CANT_CHANGE_TYPE	50
#define MSG_MAX_NO			51
#endif

#ifndef DSP_MESSAGE_NUMBER_
#define DSP_MESSAGE_NUMBER_
#define DSP_MSG_TARE		0
#define DSP_MSG_WEIGHT		1
#define DSP_MSG_QUANTITY	2
#define DSP_MSG_PIECES		3
#define DSP_MSG_UNITPRICE	4
#define DSP_MSG_TOTALPRICE	5
#endif

#ifndef WIRELESS_TYPE_
#define WIRELESS_TYPE_
#define WLTYPE_NONE			0
#define WLTYPE_CAS_WLAN		1
#define WLTYPE_LANTRONIX	2
#define WLTYPE_TOBNET		3

#define WLMODEL_NONE        0
#define WLMODEL_LANTRONIX   1
#define WLMODEL_T_VERSION   2
#define WLMODEL_HIFLYING    3
#endif
#ifdef USE_USB_DATA_ROM_UPDATE
extern INT8U UsbDataRomUpdate;
extern INT8U CheckUsbDataRomUpdate;
#endif

extern char global_message[MSG_MAX_NO][34];
extern char global_datetime[6][16];
extern char global_country_code[7][4];
extern char display_message[6][34];
extern INT16U MAX_INGREDIENT_NO;
extern INT16U MAX_INGREDIENT_TEXT_NUMBER;
extern INT16U MAX_INDIRECT_INGREDIENT_large;
extern INT16U MAX_INDIRECT_INGREDIENT_small;
#ifdef USE_DIRECT_INGREDIENT
extern INT16U MAX_DIRECT_INGREDIENT_large;
extern INT16U MAX_DIRECT_INGREDIENT_small;
extern INT32U FLASH_INGREDIENT_TEXT_AREA;
extern INT32U FLASH_INGREDIENT_TEXT_EXT_AREA;
#endif
#if defined(USE_9999_INDIRECT_INGREDIENT) || defined(USE_INGREDIENT_CODE_4DIGIT)
extern INT32S MAX_INGREDIENT_INDEX_NUMBER;
#elif defined(USE_9999_INDIRECT_INGREDIENT)
extern INT32U FLASH_INGREDIENT_TABLE_EXT_AREA;
#endif
extern INT16U INGREDIENT_STRUCT_SIZE;
extern INT32U NVRAM_PLU_TRANS_PNT;
extern INT16U SysBootTime;
#ifdef USE_CTS_FUNCTION
extern CTS_STRUCT CtsStruct;
extern CTS_STRUCT2 CtsStruct2;
extern CTS_ENCRYPT_BUF_STRUCT CtsEncryptBufStruct;
#endif
//#ifdef USE_QR_ADDTOTAL
// 실제 사용하는 바코드 폼의 길이에 따라 Add Total Count를 계산 
// NVRAM 영역, Floating Clerk Data Sync 등 추후 다시 적용 방안 검토 필요 
//extern INT16U QR_addedMemUsage[MAX_CLERK_NO];
//#endif

extern INT8U InitFtlChkFlag;

extern void init_default(void);
// extern void init_factory(void);
extern void init_main(void);
extern INT8U init_map(void);
extern void init_factory_nvram(INT8U v);
//extern STARTUP_CONFIG Startup;
extern NEARDATA STARTUP_CONFIG Startup;//HYP 20060412 _huge --> _near for small code size
extern void DisplayStartLogo(void);

extern INT8U  get_global_bparam(INT32U a);
extern INT16U get_global_wparam(INT32U a);
extern INT32U get_global_lparam(INT32U a);
extern void   get_global_sparam(INT32U a,INT8U *s,INT16S l);
extern void   set_global_bparam(INT32U a,INT8U v);
extern void   set_global_wparam(INT32U a,INT16U v);
extern void   set_global_lparam(INT32U a,INT32U v);
extern void   set_global_sparam(INT32U a,INT8U *s,INT16S l);

extern INT8U  get_base_bparam(INT32U a);
extern INT16U get_base_wparam(INT32U a);
extern INT32U get_base_lparam(INT32U a);
extern void   get_base_sparam(INT32U a,INT8U *v,INT16S l);
extern void   set_base_bparam(INT32U a,INT8U v);
extern void   set_base_wparam(INT32U a,INT16U v);
extern void   set_base_lparam(INT32U a,INT32U v);
extern void   set_base_sparam(INT32U a,INT8U *v,INT16S leng);

extern INT8U  get_prt_bparam(INT32U a);
extern void   set_prt_bparam(INT32U a,INT8U v);
extern INT8U  get_net_bparam(INT32U a);
extern INT16U get_net_wparam(INT32U a);
extern INT32U get_net_lparam(INT32U a);
extern void get_net_sparam(INT32U a,INT8U *v,INT16S l);

extern void   set_net_bparam(INT32U a,INT8U v);
extern void   set_net_wparam(INT32U a,INT16U v);
extern void   set_net_lparam(INT32U a,INT32U v);
extern void   set_net_sparam(INT32U a,INT8U *s,INT16S l);

extern INT8U  get_nvram_bparam(INT32U addr);
extern INT16U get_nvram_wparam(INT32U addr);
extern INT32U get_nvram_lparam(INT32U addr);
extern void   get_nvram_sparam(INT32U addr,INT8U *s,INT16S leng);
extern void   set_nvram_bparam(INT32U addr,INT8U v);
extern void   set_nvram_wparam(INT32U addr,INT16U v);
extern void   set_nvram_lparam(INT32U addr,INT32U v);
extern void   set_nvram_sparam(INT32U addr,INT8U *str,INT16S leng);
extern void nvram_set(INT32U addr, INT8U ch, INT32U leng);
extern void nvram_clear(void);
extern void nvram_cpy(INT32U daddr, INT32U saddr, INT32U leng);
extern void parameter_backup(void);
extern void parameter_restore(void);
extern void restore_factory_default(INT16U param);
#ifdef USE_REMOTE_FUNCTION
extern void set_network_setting_sparam(INT8U *ip, INT8U *gateway, INT8U *subnetmask, INT16U port);
extern void get_network_setting_sparam(INT8U *ip, INT8U *gateway, INT8U *subnetmask, INT16U *port);
extern void set_wifi_setting_sparam(INT8U *ssid, INT8U *wepVal, INT8U wepId, INT8U securType);
extern void get_wifi_setting_sparam(INT8U *ssid, INT8U *wepVal, INT8U *wepIdsecuType);
#endif

extern struct INIT_MASK_LINKED_LIST* init_mask_get_dummy_pointer(void);
extern struct INIT_MASK_LINKED_LIST_META* init_mask_get_global_meta_pointer(void);

extern void init_network(void);

#ifdef USE_CL5200N_SFLASH_DEBUGGING
extern void sflash_8M_test( INT32U startAdd, INT32U endAdd, INT8U countTest );
extern void nvram_8M_test( INT32U startAdd, INT32U endAdd, INT8U countTest );
#endif
#ifdef USE_CTS_FUNCTION
extern void CtsAlarm(INT8U key_mode);
#endif
//extern INT8U gen_price(HUGEDATA INT8U *dest,long data,INT8U digit, char decimal_ch);
