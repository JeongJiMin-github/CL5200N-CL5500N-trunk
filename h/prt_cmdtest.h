/*****************************************************************************
|*  Copyright		:	(c) 2003 CAS
|*  Version		:	0.1
|*  Modified		:	2004/08/20
****************************************************************************/


#ifndef _PRT_CMDTEST_H
#define	_PRT_CMDTEST_H

#include "initial.h"
#include "mode_sale.h"
#include "mode_main.h"
#include "rtc.h"

#define MAX_INHIBIT_SET	30

#define	FIELD_ID_DEPT_NAME	1
#define	FIELD_ID_DEPT_NO	115
#define	FIELD_ID_GROUP_CODE	9
#define	FIELD_ID_PLU_NO	2
#define	FIELD_ID_NAME	10
#define	FIELD_ID_NAME1	29
#define	FIELD_ID_NAME2	30
#define	FIELD_ID_NAME3	31
#define	FIELD_ID_NAME4	228
#define	FIELD_ID_NAME5	229
#define	FIELD_ID_NAME6	94
#define	FIELD_ID_NAME7	95
#define	FIELD_ID_NAME8	96

////#define	FIELD_ID_NAME4	32
////#define	FIELD_ID_NAME5	33
////#define	FIELD_ID_NAME6	34
//////#define	FIELD_ID_BARSTRING	37
////#define	FIELD_ID_AUTOCODE	47//38
////#define	FIELD_ID_SN	48//39

#define	FIELD_ID_INDIV_MSG_1	47
#define	FIELD_ID_INDIV_MSG_2	32
#define	FIELD_ID_INDIV_MSG_3	33
#define	FIELD_ID_INDIV_MSG_4	34
#define	FIELD_ID_INDIV_MSG_5	48
#define FIELD_ID_INDIV_MSG_6	49  //SG070201
#define FIELD_ID_INDIV_NO		51



#define	FIELD_ID_COMMODITY_CODE	11
#define	FIELD_ID_NUM_PREFIX	3
#define	FIELD_ID_WEIGHT_UNIT	5
#define	FIELD_ID_TAX_RATE	8
#define	FIELD_ID_TAX_PRICE	111
#define	FIELD_ID_TARE_VALUE	13
#define	FIELD_ID_UNIT_PCS	14
#define	FIELD_ID_PCS_SYM_STR	15
#define	FIELD_ID_SELL_BY_DATE	16
#define	FIELD_ID_SELL_BY_TIME	17
#define	FIELD_ID_PACKED_DATE	18
#define	FIELD_ID_PACKED_TIME	19
#define	FIELD_ID_PRODUCED_DATE	20
#ifdef USE_JULIAN_DATE
#define FIELD_ID_JULIAN_SELL_BY_DATE	21
#endif
#define	FIELD_ID_COOK_BY_DATE	22
#define	FIELD_ID_BONUS_POINT	50
#define	FIELD_ID_ORIGIN_STR	55
#define	FIELD_ID_INGREDIENT_STR	25
#define	FIELD_ID_INGREDIENT_STR2	26
#define	FIELD_ID_INGREDIENT_STR3	75
#ifdef USE_JULIAN_DATE
#define	FIELD_ID_JULIAN_PACKED_DATE	28
#endif

#define	FIELD_ID_LABEL_MSG_STR	90
#define	FIELD_ID_NET_WEIGHT	100					// Arabic Weight for ARAB (USE_ARABIC_NUMBER)
#define	FIELD_ID_2ND_NET_WEIGHT	124				// Arabic Gross Weight for ARAB (USE_ARABIC_NUMBER)
#define	FIELD_ID_GROSS_WEIGHT	116
#define	FIELD_ID_PERCENT_TARE	24
#define	FIELD_ID_PERCENT_TARE_LIMIT	23
#define FIELD_ID_PERCENT_WEIGHT	216
#define	FIELD_ID_WEIGHT	101
#define	FIELD_ID_NEW_WEIGHT	132
#define	FIELD_ID_MARK_DN_WEIGHT	108
#define	FIELD_ID_QUANTITY	112
#define	FIELD_ID_NEW_QUANTITY	133				// Arabic QUANTITY for ARAB (USE_ARABIC_NUMBER)
#define	FIELD_ID_MARK_DN_QUANTITY	134
#define	FIELD_ID_UNIT_PRICE	6
#define	FIELD_ID_NEW_UNITPRICE	113
#define	FIELD_ID_MARK_DN_UNIT	109
#define	FIELD_ID_TOTALPRICE	102
#define	FIELD_ID_NEW_TOTALPRICE	114
#define	FIELD_ID_MARK_DN_TOTAL	110
#define	FIELD_ID_SAVED_PRICE	131
#define	FIELD_ID_TRANSACTION_NO	135
#define	FIELD_ID_SHOP_NAME_STR	105
#define	FIELD_ID_SHOP_TEL_STR	138
#define	FIELD_ID_SHOP_TEXT_STR	139
#define	FIELD_ID_CLERK_NO	128
#define	FIELD_ID_CLERK_NAME	106
#define	FIELD_ID_CUSTOMER_NAME	107
#define	FIELD_ID_CUSTOMER_NO	130
#define	FIELD_ID_SCALE_NO	137
#define	FIELD_ID_WEEK_DAY	117
//#define	FIELD_ID_WEEKSIGN_DAY	121

#define	FIELD_ID_CURRENCY_RATE	119
#define	FIELD_ID_2ND_TOTALPRICE	118				// Arabic Total Price for ARAB (USE_ARABIC_NUMBER)
#define	FIELD_ID_2ND_UNIT_PRICE	120				// Arabic Unit Price for ARAB (USE_ARABIC_NUMBER)
#define	FIELD_ID_MEAT_USE	121
#define	FIELD_ID_SLAUGHTER_HOUSE	122
#define	FIELD_ID_INDIN_NO_AND_S_HOUSE	123
#define	FIELD_ID_MEAT_PART	125
#define	FIELD_ID_ALL_MEAT_GRADE	126
#define	FIELD_ID_MEAT_GRADE_CHECK	127
		
#define	FIELD_ID_TRACEABILITY_CODE	35
#define	FIELD_ID_BORN_COUNTRY	36
#define	FIELD_ID_BRED_COUNTRY	37
#define	FIELD_ID_SLAUGHTER	38
#define	FIELD_ID_SLAUGHTER_CONTRY	39
#define	FIELD_ID_CUTTING	40
#define	FIELD_ID_CUTTING_COUNTRY	41
#define	FIELD_ID_TRACE_REFERENCE	42
#define	FIELD_ID_BORN_CODE	43
#define	FIELD_ID_BRED_CODE	44
#define	FIELD_ID_SLAUGHTER_CODE	45
#define	FIELD_ID_CUTTING_CODE	46
#ifdef USE_TRACE_1000QTY //sewon 161222
#define FIELD_ID_PIG_FARM_CODE		63
#endif
#define	FIELD_ID_N_SERVING_SIZE	140
#define	FIELD_ID_N_SERVINGS_PER	141
#define	FIELD_ID_N_CALORIES	142
#define	FIELD_ID_N_CALORIES_FAT	143
#define	FIELD_ID_N_TOTAL_FAT	144
#define	FIELD_ID_N_SATURATED_FAT	145
#define	FIELD_ID_N_CHOLESTEROL	146
#define	FIELD_ID_N_SODIUM	147
#define	FIELD_ID_N_TOTAL_CARBO	148
#define	FIELD_ID_N_DIETARY_FIBERS	149
#define	FIELD_ID_N_SUGARS	150
#define	FIELD_ID_N_PROTEIN	151
#define	FIELD_ID_N_VITAMIN_A	152
#define	FIELD_ID_N_CALCIUM	153
#define	FIELD_ID_N_VITAMIN_C	154
#define	FIELD_ID_N_IRON	155
#define	FIELD_ID_N_TOTAL_FAT_DV	156
#define	FIELD_ID_N_SATURATED_FAT_DV	157
#define	FIELD_ID_N_CHOLESTEROL_DV	158
#define	FIELD_ID_N_SODIUM_DV	159
#define	FIELD_ID_N_TOTAL_CARBO_DV	160
#define	FIELD_ID_N_DIETARY_FIBERS_DV	161
#define	FIELD_ID_N_PROTEIN_DV	162
#define	FIELD_ID_N_TRANS_FAT	163
#define	FIELD_ID_N_BASE_SIZE	164  //SG070109
#define	FIELD_ID_N_SALT	165 

#if defined (USE_EU_NUTRITIONFACT)
#define FIELD_ID_N_INTEGRATE_FORM 166
#elif defined (USE_ADDITIONAL_NUTRIENT)
#define	FIELD_ID_N_ENERGY			167
#define	FIELD_ID_N_VITAMIN_D		217
#define	FIELD_ID_N_ADDED_SUGARS		218
#define	FIELD_ID_N_POTASSIUM		219
#define	FIELD_ID_N_VITAMIN_D_DV		220
#define	FIELD_ID_N_ADDED_SUGARS_DV	221
#define	FIELD_ID_N_POTASSIUM_DV		222
#define	FIELD_ID_N_CALCIUM_DV		223
#define	FIELD_ID_N_IRON_DV			224
#define	FIELD_ID_N_SUGARS_DV		225
#else
#define FIELD_ID_N_ENERGY			167
#endif // USE_EU_NUTRITIONFACT
#define	FIELD_ID_CAP_TOTAL	170
#define	FIELD_ID_CAP_UNIT_PRICE	171
#define	FIELD_ID_CAP_WEIGHT	172
#define	FIELD_ID_CAP_TARE	173
#define	FIELD_ID_CAP_DISC_PRICE	174
#define	FIELD_ID_CAP_REG_PRICE	175
#define	FIELD_ID_CAP_TAX_PRICE	176
#define	FIELD_ID_CAP_PACKEDDATE	177
#define	FIELD_ID_CAP_SELLBYDATE	178
#define	FIELD_ID_CAP_COOKBYDATE	179
#define	FIELD_ID_CAP_PRODUCEDDATE	180
#define	FIELD_ID_CAP_PACKEDTIME	181
#define	FIELD_ID_CAP_SELLBYTIME	182
#define	FIELD_ID_CAP_QUANTITY	183
#define	FIELD_ID_CAP_REFERENCE	184
#define	FIELD_ID_CAP_BORN_IN	185
#define	FIELD_ID_CAP_BRED_IN	186
#define	FIELD_ID_CAP_SLAUGHT_IN	187
#define	FIELD_ID_CAP_CUT_IN	188
#define	FIELD_ID_CAP_NET_WEIGHT	189
#define	FIELD_ID_CAP_GROSS_WEIGHT	193
#define	FIELD_ID_CAP_PERCENT_TARE	194
#define	FIELD_ID_CAP_PERCENT_TARE_LIMIT	195
#define	FIELD_ID_CAP_TRAN_COUNT	190
#define	FIELD_ID_CAP_PAYMENT_PRICE	191
#define	FIELD_ID_CAP_CHANGE_PRICE	192
#define	FIELD_ID_CAP_TTL_TOTAL	196
#define	FIELD_ID_CAP_TTL_TAX_PRICE	197
#define	FIELD_ID_CAP_2ND_TOTAL_PRICE	198
#define	FIELD_ID_CAP_2ND_UNIT_PRICE	199
#define	FIELD_ID_CAP_TTL_2ND_TOTAL_PRICE	200
#define	FIELD_ID_CAP_TTL_VOID_COUNT	169
#define	FIELD_ID_CAP_TTL_VOID_PRICE	168
#define	FIELD_ID_CAP_TTL_TOTAL_WEIGHT	231
#define	FIELD_ID_CAP_TTL_TOTAL_QUANTITY	232
#define FIELD_ID_CAP_INGREDIENT	233
#define FIELD_ID_CAP_PLU_NO 	234
#define	FIELD_ID_CAP_ORIGIN_NO	230
#define	FIELD_ID_TTL_TOTAL_PRICE	201
#define	FIELD_ID_TTL_TOTAL_COUNT	202
#define	FIELD_ID_TTL_VOID_PRICE	203
#define	FIELD_ID_TTL_VOID_COUNT	204
#define	FIELD_ID_TTL_INTAX_AMOUNT	205
#define	FIELD_ID_TTL_EXTAX_AMOUNT	206
#define	FIELD_ID_TTL_TOTAL_PRICE_EX_TAX	207
#define	FIELD_ID_TTL_TOTAL_PRICE_IN_TAX	211
#define	FIELD_ID_TTL_PAYMENT	208
#define	FIELD_ID_TTL_CHANGE	209
#define	FIELD_ID_CUR_DATE	103
#define	FIELD_ID_CUR_TIME	104
#define	FIELD_ID_TTL_TAX_AMOUNT	210
#define	FIELD_ID_TTL_2ND_TOTAL_PRICE	212
#define	FIELD_ID_TTL_TOTAL_WEIGHT	213
#define	FIELD_ID_TTL_TOTAL_QUANTITY	214
#define	FIELD_ID_TTL_TOTAL_TITLE	215
#define	FIELD_ID_BARCODE	240
#ifdef USE_LOT_NUMBER_FUNCTION
#define FIELD_ID_LOT_NUMBER	227
#endif
#define	FIELD_ID_IMAGE	250
#define FIELD_ID_TOTAL_PRICE_IN_TAX 129
#define FIELD_ID_TOTAL_PRICE_EX_TAX 136
#define FIELD_ID_DISCOUNT_RATE 99





//#define FIELD_ID_BAR_DEFAULT	1
//#define FIELD_ID_BAR_TOTAL		2
//#define FIELD_ID_BAR_TRACE		3
#define BARFIELD_ID_DEFAULT	1
#define BARFIELD_ID_TOTAL		2
#define BARFIELD_ID_TRACE		3

#define BARFIELD_ID_TEMP		5

#define ITEM_LABEL		0
#define TEST_LABEL  	1
#define TOTAL_LABEL		10
#define GTOTAL_LABEL	11


// Barcode Scanned Flag
#define BAR_SCANFLAG_ITEMCODE		0x0001
#define BAR_SCANFLAG_WEIGHT			0x0002
#define BAR_SCANFLAG_DEPTNO			0x0004
#define BAR_SCANFLAG_PLUNO			0x0008
#define BAR_SCANFLAG_MEATUSE		0x0010
#define BAR_SCANFLAG_SL_NO			0x0020
#define BAR_SCANFLAG_TEXT			0x0040
#define BAR_SCANFLAG_REMNANT		0x0080
#define BAR_SCANFLAG_MEATPART		0x0100
#define BAR_SCANFLAG_MEATGRADE		0x0200
#define BAR_SCANFLAG_ORIGIN			0x0400
#define BAR_SCANFLAG_FIXED_PRICE 	0x0800
#ifdef USE_LOTTEMART_MEATSCANNER
#define BAR_SCANFLAG_SL_NO2			0x1000
#define BAR_SCANFLAG_SL_NO3			0x8000
#else
#define BAR_SCANFLAG_CODE_880		0x1000
#endif
#define BAR_SCANFLAG_TARE_WEIGHT	0x2000
#ifdef USE_LOTTEMART_ITEMCODE13
#define BAR_SCANFLAG_ITEMCODE13		0x4000
#endif
#ifdef USE_HYUNDAI_CHECK_REASON_CODE
#define BAR_SCANFLAG_REASON_CODE	0x4000
#endif
#ifdef USE_HYUNDAI_SET_FUNCTION
#define BAR_SCANFLAG_RECEIPT_NO		0x8000
#endif

// Scanner Symbol
//    Remained Symbol : 
//	A B C E F G H J L M O Q R U V Y Z
//	a b c d f h i j k l m n o q r t v w x y z
#define SCAN_SYMBOL_ITEMCODE	'I'
#define SCAN_SYMBOL_WEIGHT		'W'
#define SCAN_SYMBOL_DEPTNO		'D'
#define SCAN_SYMBOL_PLUNO		'N'
#define SCAN_SYMBOL_TEXT		'T'
#define SCAN_SYMBOL_REMNANT		'X'
#define SCAN_SYMBOL_FIXED_PRICE	'P'
#define SCAN_SYMBOL_CODE_880	'e'
#define SCAN_SYMBOL_TARE_WEIGHT	'S'
// Korea Traceability Only
#define SCAN_SYMBOL_MEATUSE		'u'
#define SCAN_SYMBOL_SL_NO		's'
#ifdef USE_LOTTEMART_MEATSCANNER
#define SCAN_SYMBOL_SL_NO2		't'
#define SCAN_SYMBOL_SL_NO3		'v'
#endif
#define SCAN_SYMBOL_ORIGIN		'K'
#define SCAN_SYMBOL_MEATPART	'p'
#define SCAN_SYMBOL_MEATGRADE	'g'
#ifdef USE_HYUNDAI_SET_FUNCTION
#define SCAN_SYMBOL_RECEIPT_NO	'r'
#endif
#ifdef USE_HYUNDAI_CHECK_REASON_CODE
#define SCAN_SYMBOL_REASON_CODE	'h'
#endif
// String Form
#define SCAN_STRING					"IWDNTXPeS9"
#define SCAN_STRING_KOR				"suK"
#define SCAN_STRING_KOR_MEATPART	"p"
#define SCAN_STRING_KOR_MEATGRADE	"g"
#ifdef USE_LOTTEMART_ITEMCODE13
#define SCAN_SYMBOL_ITEMCODE13		'i'
#define SCAN_STRING_ITEMCODE13		"i"
#endif
#ifdef USE_LOTTEMART_MEATSCANNER
#define SCAN_STRING_SL_NO2		"t"
#define SCAN_STRING_SL_NO3		"v"
#endif
#ifdef USE_HYUNDAI_SET_FUNCTION
#define SCAN_STRING_RECEIPT_NO	"r"
#endif
#ifdef USE_HYUNDAI_CHECK_REASON_CODE
#define SCAN_STRING_REASON_CODE	"h"
#endif
// Barcode Symbol
//    Remained Symbol :
//	
//	a b f h n x z
#define BARCODE_SYMBOL_ITEMCODE			'I'
#define BARCODE_SYMBOL_PLUNO			'N'	
#define BARCODE_SYMBOL_PRICE			'P'
#define BARCODE_SYMBOL_SECONDCURRENCY	'y'
#define BARCODE_SYMBOL_PRICECHECKSUM	'V'
#define BARCODE_SYMBOL_WEIGHTCHECKSUM	'v'
#define BARCODE_SYMBOL_WEIGHT			'W'
#define BARCODE_SYMBOL_QUANTITYCHECKSUM	'e'
#define BARCODE_SYMBOL_GROSS_WEIGHT		'r'
#define BARCODE_SYMBOL_QUANTITY			'Q'
#define BARCODE_SYMBOL_UNITPRICE		'U'
#define BARCODE_SYMBOL_DEPTNO			'D'
#define BARCODE_SYMBOL_GROUP			'G'
#define BARCODE_SYMBOL_COUNTRY			'K'
#define BARCODE_SYMBOL_TICKET_NO		'A'
#define BARCODE_SYMBOL_LABELTRAN_NO		'B'
#define BARCODE_SYMBOL_CLERK_NO			'L'
#define BARCODE_SYMBOL_PREFIX			'F'
#define BARCODE_SYMBOL_SPECIALCHAR		'%'
#define BARCODE_SYMBOL_TEXT				'T'
#define BARCODE_SYMBOL_PLU_TRANS_COUNTER 'R'
#define BARCODE_SYMBOL_CHECKSUM			'C'
#define BARCODE_SYMBOL_GTIN_CHECKSUM	'c'
#ifdef USE_GSMART_BARCODE
#define BARCODE_SYMBOL_DISCOUNT			'X'
#else
#define BARCODE_SYMBOL_TRACEABILITY		'X'
#endif
#define BARCODE_SYMBOL_SLAUGHTER_NO		'Y'	
#define BARCODE_SYMBOL_CUTTINGHALL		'Z'	
#define BARCODE_SYMBOL_BORN_IN			'E'
#define BARCODE_SYMBOL_BRED_IN			'H'
#define BARCODE_SYMBOL_PACKED_DATE		'O'
#define BARCODE_SYMBOL_PACKED_TIME		'o'
#define BARCODE_SYMBOL_SELL_DATE		'J'
#define BARCODE_SYMBOL_SELL_TIME		'j'
#define BARCODE_SYMBOL_TARE_WEIGHT		'S'
#define BARCODE_SYMBOL_SCALE_ID			'M'	
#define BARCODE_SYMBOL_CURRENTDATE		'k'
#if defined (USE_RUSSIA_LOGISTICS_PLU) || defined (USE_EUR_LOGISTICS_PLU)
// Logistics PLU를 사용할때 BARCODE_SYMBOL_CONTAINED_UNIT와 Symbol이 겹치기 때문에 #else 처리
#else
#define BARCODE_SYMBOL_CURRENTTIME		'm'
#endif
// 중국 전용
#define BARCODE_SYMBOL_DISCOUNT_RATE	'd'
// 중국 까르푸 전용
#define BARCODE_SYMBOL_PRICE_TYPE	'f'
// 러시아 전용
#define BARCODE_SYMBOL_WEIGHT_PERCENT	'w'
#define BARCODE_SYMBOL_WEIGHT_POINT		't' // 'w'->'t'로 수정 (CL5000J에서 Percent Weight를 'w'로 사용하여 동일하게 적용'
#define BARCODE_SYMBOL_PRODUCED_DATE	'o'
// 국내 전용	
#define BARCODE_SYMBOL_MEATUSE			'u'
#define BARCODE_SYMBOL_SL_NO			's'
#define BARCODE_SYMBOL_MEATPART			'p'
#define BARCODE_SYMBOL_MEATGRADE		'g'
//롯데마트 전용
#ifdef USE_LOTTEMART_ITEMCODE13
#define BARCODE_SYMBOL_ITEMCODE13		'i'
#endif
#ifdef USE_LOTTEMART_BARCODE_FUNCTION
#define BARCODE_SYMBOL_LOTTECHECKSUM	'x'
#endif
// 농협 전용
#ifdef USE_SETPRODUCT_SALE
#define BARCODE_SYMBOL_SETNUMBER		'n'
#endif
// 현대 백화점 전용
#ifdef USE_HYUNDAI_CHECK_REASON_CODE
#define BARCODE_SYMBOL_REASONCODE		'h'
#endif
#define BARCODE_SYMBOL_FNC1				'q'

#ifdef USE_LOGISTICS_BARCODE
#define BARCODE_SYMBOL_AI1				'('
#define BARCODE_SYMBOL_AI2				')'
#define BARCODE_SYMBOL_GTIN				'a'
#define BARCODE_SYMBOL_CONTAINED_UNIT	'm'
#define BARCODE_SYMBOL_LOT_NUMBER		'l' // small L
#define BARCODE_SYMBOL_SERIAL_NUMBER	'z'
#endif
// 헝가리, 코스타리카, 멕시코 전용
#ifdef USE_LOT_NUMBER_FUNCTION
#define BARCODE_SYMBOL_LOT_NUMBER		'l' // small L
#endif

#ifdef USE_EMART_ETC_COMMON_TEXT
#define ETC_TEXT_INGREDIENT_TABLE_NUM	99		// 이마트 신선식품 온도별 표기강화 적용에 따른 기타 공통표시 사항 항목 인쇄 기능 (라벨폼에 "성분2" 필드가 있으면 간접성분 테이블 #99의 내용을 인쇄)
#endif

#define BAR_PARAM_TEXT_LEN	40
#ifdef USE_GSMART_PLU
  #if (PLU_COMMODITY_LINE_NO == 8)
	#define BAR_PARAM_TEXT_FIELD PTYPE_PLU_NAME8 // BARDODE 'T'에 상품명 8(or 5,3)단의 내용을 인쇄함
  #else
	#define BAR_PARAM_TEXT_FIELD PTYPE_PLU_NAME5
  #endif
#else
	#define BAR_PARAM_TEXT_FIELD PTYPE_PLU_NAME3
#endif

#pragma pack(4)
typedef struct {
	INT32U ItemCode;
	INT32U PluNumber;
	INT32U TotalPrice;
	INT32U UnitPrice;
	INT32U Weight;
	INT32U TareWeight;		//Added by JJANG 20071212
	INT32U PercentWeight;
	INT32U Quantity;
	INT16U DeptId;
	INT16U GroupNo;
	INT16U CountryNo;
	INT32U TicketNo;
	INT32U LabelTranNo;
	INT16U ClerkNo;
	INT16U PluTrCnt;
	INT16U iPrefix;
	INT8U Prefix[10];
//	INT8U Text[27];			// limit 26bytes			
	INT8U Text[BAR_PARAM_TEXT_LEN];			// limit 40bytes			
//	INT8U BarString[27];	// Added by CJK 20051226 
//DEL061017	INT8U BarString[30];	// Added by CJK 20051226 
	INT16U trace_no;
	INT16U born_country_no;
	INT16U bred_country_no;
	INT16U slaught_no;
	INT16U cutting_no;
	INT16U WeightCode;
	INT32U AutoCutPrice;
#ifdef USE_TRACE_STANDALONE
	INT8U  MeatUseNo;
	INT8U  MeatPartNo;
	INT16U  SlaughtHouseNo;
#ifdef USE_LOTTEMART_MEATSCANNER
	INT8U	Indiv_no[24+1]; //INDIVIDUAL_NO_LEN+1
	INT8U	Meat_use_no;
	INT16U	SlaughtHouseNo1;
	INT16U	SlaughtHouseNo2;
	INT16U	SlaughtHouseNo3;
	INT8U	Meat_grade_no;
	INT8U	No14_indiv_flag; //보관라벨용 14자리 이력번호 유무
#endif
	INT8U  MeatGradeNo;
	INT8U  Reserved;
#endif
	INT16U Scan_flag;
	INT8U  RemnantData[30];
#ifdef USE_GSMART_BARCODE
	INT32U DiscountVolume;
	INT8U  DiscountFlag;
#endif
	INT32U ScaleId;
	INT32U SecondCurrency;
	INT8U  ItemCode13[20];
#ifdef USE_SCANNER_FOR_ECO_FRIENDLY
	INT8U code880[11];
#endif
#if defined (USE_RUSSIA_LOGISTICS_PLU) || defined (USE_EUR_LOGISTICS_PLU)
	INT8U Gtin_code[18];
	INT8U Contained_unit[8];
	INT8U Lot_number[20];
	INT8U Serial_number[20];
#endif // #ifdef USE_RUSSIA_LOGISTICS_PLU
#ifdef USE_HYUNDAI_CHECK_REASON_CODE
	INT8U ReasonCode;
#endif
#ifdef USE_HYUNDAI_SET_FUNCTION
	INT8U ReceiptNo[23]; // receipt no 22 + 1
#endif
#ifdef USE_BESTCO_BARCODE_FUNCTION
	INT8U unit_price_category;
#endif
} STRUCT_STRFORM_PARAM;


typedef struct {
	INT16S  TotalCount;
	long TotalPrice;
	long TotalSavedPrice;	//Added by JJANG 20071228 
	INT16S  VoidCount;
	long VoidPrice;
	long PayPrice;
	long SummaryTotalPrice;
	long SummarySavedPrice;
	long TotalWeight;
	INT16S  TotalQuantity;
	long SummaryRound;
	long TotalNetWeight;
	long TotalPerTareWeight;
	long TotalGrossWeight;
	long taxPrice[MAX_TAX_NO];
	long taxGross[MAX_TAX_NO];
	long InTaxPrice;
	long ExTaxPrice;
//CLR	long ChangePrice;
//CLR	long PaidTypePrice[MAX_PAID_TYPE];
//CLR	INT16U CustomerNo;
//CLR	CURRENCY_PRICE_TYPE CPrice;
//CLR	INT8U ClerkNum;
} STRUCT_TOTAL_PRICE;



typedef struct {
	INT16U shop_name_no;
	INT16U clerk_no;
	INT16U customer_no;
	RTC_STRUCT time;
} STRUCT_PRT_COMMON;

typedef struct {
	INT32U plu_addr;
	INT16U trace_no;
	INT16U born_country_no;
	INT16U bred_country_no;
	INT16U slaught_no;
	INT16U cutting_no;
	INT16U nutri_no;
	INT32U TotalPrice;
	INT32U CurrencyTotalPrice;//SecondTotalPrice;
	long tran_no;
	INT16U plu_tran_no;
	INT16U origin;
	INT8U cur_pcssym_no;

	INT32U sellbydate;
	INT16U packedondate;
	INT16U produceddate;
	INT16U cookbydate;
#ifdef USE_GSMART_PLU
	INT16U sellbytime;	// GSMART일때 2byte
	INT16U packedontime;	// GSMART일때 2byte
#else
	INT8U sellbytime;	// GSMART아닐 때 1byte
	INT8U packedontime;	// GSMART아닐 때 1byte
#endif
	INT8U flagsellbydate;
	INT8U flagpackedondate;
	INT8U flagsellbytime;
	INT8U flagpackedontime;

	INT8U fixed_price;
	INT16U bonus_point;
	CURRENCY_PRICE_TYPE CPrice;

	INT8U unitweight_id;

#ifdef USE_DISCOUNT_MEMBERSHIP
	INT8U disc_type; //할인 유형
#endif

	INT32U     l_PluNo;
	INT16U     l_DeptNo;
	PRICE_TYPE l_Price;
//	PRICE_TYPE l_LinkedPrice[MAX_LINKED_PLU];
//	INT32U	   l_linked_plu[MAX_LINKED_PLU];
	INT8U 	   l_PluType;
	INT32U	   l_TareWeight;
	INT32U	   l_PluTareWeight;
	INT32U	   l_KeyTareWeight;
	INT32U	   l_WeightTareWeight;
	INT32U     l_FixedWeight;


} STRUCT_PRT_ITEM;


typedef struct {
	long SummaryTotalPrice;
	long SummarySavedPrice;
	long TotalSavedPrice;	//Added by JJANG 20071228
	INT16S  TotalCount;
	long TotalPrice;
	INT16S  VoidCount;
	long VoidPrice;
	long TaxTypePrice[MAX_TAX_NO];
	long TaxTypeTotal[MAX_TAX_NO];
	long InTaxPrice;
	long ExTaxPrice;
	long PayPrice;
	long ChangePrice;
	long PaidTypePrice[MAX_PAID_TYPE];
	INT16U CustomerNo;
	CURRENCY_PRICE_TYPE CPrice;
	INT8U ClerkNum;
	long TotalWeight;
	INT16S  TotalQuantity;
	long SummaryRound;
	long TicketTransactNumber;
	long TotalNetWeight;
	long TotalPerTareWeight;
	long TotalGrossWeight;

} STRUCT_PRT_TOTAL;


typedef struct {
	INT8U origin_prttype;
/*
	INT16U flagSymInQty	:1;
	INT16U flagPcsInUnit	:1;
	INT16U flagZeroUnitPrice	:1;
	INT16U en_print_payment	:1;
	INT16U en_mark_down_weight:1;
	INT16U en_mark_down_qty	:1;
	INT16U en_mark_down_unit	:1;
	INT16U en_mark_down_total	:1;
	INT16U dummy		:8;
*/
	INT8U flagSymInQty;
	INT8U flagPcsInUnit;
	INT8U flagZeroUnitPrice;
	INT8U en_print_payment;
	INT8U en_mark_down_weight;
	INT8U en_mark_down_qty;
	INT8U en_mark_down_unit;
	INT8U en_mark_down_total;
} STRUCT_LABEL_SET;
#pragma pack()

// Added by CJK
#ifndef LABEL_DATA_BACKUP_TYPE_DEFINE
#define LABEL_DATA_BACKUP_TYPE_DEFINE
#pragma pack(4)
typedef struct {
	INT32U     PluNo;
	INT16U     DeptNo;
	PRICE_TYPE Price;
	PRICE_TYPE LinkedPrice[MAX_LINKED_PLU];
	INT32U	   linked_plu[MAX_LINKED_PLU];
	INT8U 	   PluType;
	INT32U	   TareWeight;
	INT32U	   PluTareWeight;
	INT32U	   KeyTareWeight;		// Added by CJK 20060405
	INT32U	   WeightTareWeight;	// Added by CJK 20060405
	INT32U     FixedWeight;
	INT32U	   SellbyDate;  //SG060322 for Kor taceability
} LABEL_DATA_BACKUP_TYPE;
#pragma pack()
#endif

#ifndef PRTSEL_MENU_TYPE_DEFINE
#define PRTSEL_MENU_TYPE_DEFINE
#pragma pack(4)
typedef struct {
	INT16U CapCode;
	INT8U  FieldId;
} PRTSEL_MENU_TYPE;
#pragma pack()
#endif

#pragma pack(1)
typedef struct {
	INT16U 	id;
	INT16U 	width;
	INT16U 	height;
	char 	labelname[16];
	INT16U 	size;
} LABEL_IMAGE_INFO;

typedef struct {
	INT16U 	id;
	INT16U 	width;
	INT16U 	height;
} LABEL_BITMAP_INFO;

#pragma pack()

#ifdef USE_QR_ADDTOTAL
extern INT8U TotalQRFlag;
#endif

#ifdef USE_AUSTRALIA_COOL_FUNCTION
typedef struct {
	INT8U Cooltype;
	INT8U CoolContainPercent;	
	INT16U CoolOrigin;
	char CoolOriginText[32];
	char CoolCustom[150];
 } STRUCT_COOL_IMAGE;
#endif

#ifdef USE_PRICE_CHANGE_PROHIBITED
extern INT16U PressedNumberKey_1000Digits;
#endif
#ifdef USE_EMART_DEFAULT
extern INT8U PrtName4_With_slaughthouse;
#endif
extern STRUCT_PRT_TOTAL PrtTotalStr; 
extern BOOLEAN ExtPrtTotalFlag;
extern INT8U ExtPrtTotalTypeFlag;
extern BOOLEAN FlagExternalPrtAddUp;
extern INT8U PrtWeightUnit, PrtWeightUnitId;
#ifdef USE_PRICE_CHANGE_PROHIBITED
extern INT16U packed_date;
#else
extern INT8U packed_date;
#endif

extern INT8U get_traceability_name(INT16U id, HUGEDATA char *name);
extern INT8U get_country_name(INT16U id, HUGEDATA char *name);
extern INT8U get_slaughterhouse_name(INT16U id, HUGEDATA char *name);
extern INT8U get_slaughtercountry_name(INT16U slaught_no, HUGEDATA char *name, INT16U *country_id);
extern INT8U get_cuttinghall_name(INT16U id, HUGEDATA char *name);
extern INT8U get_cuttingcountry_name(INT16U cutting_no, HUGEDATA char *name, INT16U *country_id);
extern INT8U Prt_CheckPeelAndHead(INT8U check_peeloff);
extern INT8U Prt_CheckOutOfPaper(void);

extern INT8U print_out(INT8U label_mode, INT8U linked);
extern INT8U print_out_totallabel(void);
extern INT16U prt_get_plu_barid(INT32U plu_no, INT32U plu_addr, INT8U plu_type, INT8U no);	// Modified by CJK 20050503

extern INT8U prt_load_labelform(INT8U mode, INT8U linked);
extern INT8U prt_make_barcode(INT8U prt_mode, INT16U barid, STRUCT_STRFORM_PARAM *bar_param, INT8U field_num);
extern void prtfield_generate_item(void);
extern void prtfield_generate_common(void);
extern void prtfield_generate_traceability(void);
#ifdef USE_ADDITIONAL_NUTRIENT
extern void prtfield_generate_nutri_Added(void);
#else
extern void prtfield_generate_nutri(void);
#endif
extern void prtfield_generate_item_caption(void);
extern void prtfield_generate_total_caption(void);
extern void prtfield_generate_total(INT8U label_mode);
extern void prtfield_generate_barcode_item(void);
extern void prtfield_generate_barcode_total(void);
extern void prtfield_generate_labelserial(void);

//DEL20060309extern void prt_clear_strform_parameter(STRUCT_STRFORM_PARAM *param);
extern void prt_set_strform_parameter_item(STRUCT_STRFORM_PARAM *param, TRANSACT_CHAIN  *cp);
extern void prt_set_strform_parameter_total(STRUCT_STRFORM_PARAM *param);
extern INT8U prt_make_barform(char *bar_str, char *bar_form, STRUCT_STRFORM_PARAM *param, INT8U bar_type);
extern INT8U prt_make_strform(char *bar_str, char *bar_form, STRUCT_STRFORM_PARAM *param, INT8U bar_type);
extern INT8U prt_scan_strform(char *bar_str, char *bar_form, STRUCT_STRFORM_PARAM *param);
extern void print_inhibit_set(void);
extern void print_inhibit_init(void);

//extern INT8U prtsel_get_menu_index(INT8U field_id);
extern INT16U prtsel_get_menu_capcode(INT8U field_id);
extern void prtsel_set_table_buf(HUGEDATA INT8U *buf, INT8U field_no, INT8U val);
extern INT8U prtsel_get_table_buf(HUGEDATA INT8U *buf, INT8U field_no);
extern INT8U prtsel_get_table(INT8U field_no);
extern void prtsel_set_table(INT8U field_no, INT8U val);

extern INT16U Get1stLabelId(INT16U dept_no, INT32U plu_no);
extern INT16U Get2ndLabelId(INT16U dept_no, INT32U plu_no);
extern void input_serial_no(void);
extern INT8U print_out_samplelabel(INT16U dept_no, INT32U plu_no, INT32U key);
extern INT8U print_out_itemlabel(INT8U label_mode, INT8U linked);
extern void print_rewind(void);
extern INT8U CheckPrtPeelAndHead(INT8U nocheck_peel);
//extern INT8U Prt_CheckOutOfPaperFlag(void);
extern INT8U Prt_CheckGapErr(void);

extern INT8U get_label_name(INT16U labelid, char *str);
extern INT8U get_label_size(INT16U labelid, INT16U *w, INT16U *h);

extern INT8U PrintGrandTotal(INT16U clerkNum, INT16S count, long price, long weight, INT16S qty, long nweight, long s_nweight, long gweight);
extern void Prt_LotList(INT8U external_prt);
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

#define	PRT_ITEM_NORMAL_MODE	1
#define	PRT_ITEM_TEST_MODE		2
#define	PRT_ITEM_LIST_MODE		3
#define PRT_TOTAL_NORMAL_MODE	4
#define PRT_TOTAL_LAST_MODE		5
#define PRT_TOTAL_GRAND_MODE	6
#define PRT_ITEM_RETURN_MODE	7

extern INT8U Prt_PrintStart(INT8U mode, INT8U prt_mode, INT16U clerkid, INT8U check_sensor, INT16U dept_no, INT32U plu_no);
extern INT8U PluType();


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

#endif
