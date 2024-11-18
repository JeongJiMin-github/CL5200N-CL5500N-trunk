//typedef enum
//{
//	WS_RECV_WEIGHT,
//	WS_CALC_PRICE,
//	WS_CHECK_KEY,
//	WS_DISPLAY,
//	WS_PRINT,
//	WS_NETWORK,
//	WS_CLOCK
//} STATE_WEIGHING;


#ifndef UPDATE_DEFINE
#define UPDATE_DEFINE

#define UPDATE_WEIGHT   0x001
#define UPDATE_UNIT     0x002
#define UPDATE_PRICE    0x004
#define UPDATE_TARE     0x008

#define UPDATE_ERROR    0x010
#define UPDATE_CENTER   0x020
#define UPDATE_TITLE    0x040
#define UPDATE_MODE     0x080

#define UPDATE_DATETIME 0x100
#define UPDATE_RIDE     0x200
#define UPDATE_TITLE2   0x400
#define UPDATE_STATUS   0x800

#define UPDATE_SUBTOTAL	0x1000

#define UPDATE_CLEARALL	0x8000

#define UPDATE_ALL		0x1fff

#endif	// UPDATE_DEFINE

#ifndef MODE_SALE_H
#define MODE_SALE_H
//youngmin
#define	MISC_PLU_WEIGHT		1000000L
#define	MISC_PLU_COUNT	 	1000001L
#define	MISC_PLU_PCS		1000002L

// PLU CLASSES
#define	PLU_BY_WEIGHT		1   // by weight
#define PLU_BY_COUNT		2   // by count (pieces)
#define	PLU_BY_PCS			3   // non-food
#ifdef USE_FIXED_PLUTYPE//USE_EMART_NETWORK
#define PLU_BY_WEIGHT_FIX	4	// by weight(fixed)
#define PLU_BY_COUNT_FIX	5	// by count(fixed)
#define	PLU_BY_PCS_FIX		6   // non-food(fixed)
#endif
// misc transaction versions of above
//#define	MISC_BY_WEIGHT		0x10
//#define	MISC_BY_COUNT		0x11
//#define MISC_FIXED_PRICE	0x12

// SALE OPERATION MODES
#define OPER_NORMAL		1
#define OPER_STD_CLERK	2
#define	OPER_FLOATING	3

#define MODE_NORMAL		1
#define MODE_REPACK		2
#define	MODE_MANAGER	3
#define	MODE_SELF		4

// PRINTER MODES
#define PRINTER_MANUAL	0
#define PRINTER_AUTO	1
#define	PRINTER_OFF		2

// DISCOUNT
#define	DISCOUNT_NO		0
#define	DISCOUNT_PLU	1
#define	DISCOUNT_GLOBAL	2
#define	DISCOUNT_INDEX	3

#define	DISC_PRIORITY_NO				0
#define	DISC_PRIORITY_INDEX				1
#define	DISC_PRIORITY_GLOBAL			2
#define	DISC_PRIORITY_INDEX_GLOBAL		3

#define	DISC_TARGET_WEIGHT		0
#define	DISC_TARGET_COUNT		1
#define	DISC_TARGET_TPRICE		2
#define	DISC_TARGET_FREE		3
#define	DISC_TARGET_FIXED		4
#define	DISC_TARGET_N_PLUS_1	5

#define	DISC_PRICE_WEIGHT		0
#define	DISC_PRICE_COUNT		1
#define	DISC_PRICE_UPRICE		2
#define	DISC_PRICE_TPRICE		3
#define	DISC_PRICE_PERCENT		4
#define	DISC_PRICE_FREE			5
#define	DISC_PRICE_FIXED		6
#define	DISC_PRICE_UPERCENT		7
#define	DISC_PRICE_USPECIAL		8
#define	DISC_PRICE_TSPECIAL		9
#define	DISC_FREE_ADDITION_WEIGHT		10
#define	DISC_FREE_ADDITION_COUNT		11
#define	DISC_PRICE_UPRICE_PLUS		12
#define	DISC_N_PLUS_1			13
#ifdef USE_TOPMART_DEFAULT
#define DISC_PRICE_TOTALTOUNIT	14 //서원유통 금액할인키
#endif
#define DISC_PRICE_EDITTOTAL	15 //20170522

#define	PRICE_ORIGIN	0
#define	PRICE_SAVED		1
#define	PRICE_RESULT	2

#define	ROUND_NO			0
#define	ROUND_ALL_PLU		1	// no clerk, 기본 계산에
#define	ROUND_ONLY_TOTAL_SUM	2	// 무조건 total summey에
#define	ROUND_TOTAL_OR_PLU	3	// add가 되는 total summey에하고 add가 되지 않으면 PLU마다
#define	ROUND_PLU_ROUND_CUT	4	// 라운드 하는데 기본 계산에서 없앰

// 2004.10.29 bmlim [
#define TRANS_TYPE_PAY_ROUND_PLU		10
#define TRANS_TYPE_PAY_TOTAL_NOPAY		11
#define TRANS_TYPE_PAY_TOTAL_PAY		12
#define TRANS_TYPE_PAY_ROUND_PLU_NEG	13
// ]
#define	PAY_CASH			0
#define	PAY_PINCHIP			1
#define	PAY_CHECK			2
#define	PAY_CARD			3
#define	PAY_NOTE			4
#define	PAY_COUPON			5
#define	PAY_BONUS_POINT		6
#define	PAY_CREDIT			7
#define MAX_PAID_TYPE		8

#if (ROM_COUNTRY == ROM_RUS)

#define MAX_ADD_TRANSACTION	 399
#define MAX_LAST_TRANSACTION 399

#else

#ifdef USE_CHN_CART_SCALE
#define MAX_ADD_TRANSACTION	 349
#define MAX_LAST_TRANSACTION 349
#else
#ifdef USE_INDIA_FUNCTION
#define MAX_ADD_TRANSACTION	 99
#define MAX_LAST_TRANSACTION 99
#else
#define MAX_ADD_TRANSACTION	 399
#define MAX_LAST_TRANSACTION 399
#endif
#endif

#endif

#define MAX_CLERK_TRANSACTION	50

#define MAX_SCALE		32
#define MAX_SLAVE_SCALE	16
// #define MAX_CLERK		100
// #define MAX_DEPT		99
#define MAX_GROUP		100		// INPUT 1~99
#define MAX_CREDIT_CUSTOMER	99
// #define	MAX_PLU_SUMMARIES	2000
// #define	MAX_TAX			6
#ifdef DISABLE_X2Z2_REPORT
#define	MAX_REPORT_PERIOD	1
#else
#define	MAX_REPORT_PERIOD	2
#endif
#define	MAX_SALES_VOLUME	2000000000L

#define	TO_NUM		0
#define	TO_STR	1

#define	FONT_SMALL		1
#define	FONT_MEDIUM		2
#define	FONT_LARGE		3

#define	MAX_PLU_BATCH	10
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
#define DISPLAY_DATE_MAX	11
#else
#define DISPLAY_DATE_MAX	10 // DISPLAY_DATE_OPENCLERK 까지만 표시
#endif
#define	DISPLAY_DATE_CLEAR	0
#define	DISPLAY_DATE_MIN	1
#define	DISPLAY_DATE_SEC	2
#define	DISPLAY_DATE_DAY	3
#define	DISPLAY_DATE_SCALE	4
#define	DISPLAY_DATE_DEPT	5
#define DISPLAY_DATE_PLUNO  6
#define DISPLAY_DATE_NETSTATUS	7	// CJK080313
#define	DISPLAY_DATE_CLERK	8
#define	DISPLAY_DATE_OPENCLERK	9	// new  : Added by CJK 20060426
#define DISPLAY_DATE_ITEM   10		//8-> 9 : Modified by CJK 20060426

#define	DISPLAY_DATE_OVERRIDE	11	//9->10 : Modified by CJK 20060426

#define	SALE_NORMAL_MODE	1
#define	SALE_STANDARD_CLERK	2
#define	SALE_FLOATING_CLERK	3
#ifdef _USE_LCD37D_40DOT_
#define BASE_FONT_WIDTH		6
#define BASE_FONT_HEIGHT	8
#elif defined (_USE_LCD26d_16x96_)
#define BASE_FONT_WIDTH		8
#define BASE_FONT_HEIGHT	16
#elif defined (_USE_LCD20848_)
#ifdef USE_ARAB_FONT
#define BASE_FONT_WIDTH		8
#else
#define BASE_FONT_WIDTH		6
#endif
#define BASE_FONT_HEIGHT	8
#elif defined(_USE_LCD32_)
#ifdef USE_ARAB_FONT
#define BASE_FONT_WIDTH		8
#else
#define BASE_FONT_WIDTH		6
#endif
#define BASE_FONT_HEIGHT	8
#endif

#define DSP_SMALL_FONT_ID	2
#define DSP_MEDIUM_FONT_ID	3
#define DSP_LARGE_FONT_ID	4

#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
#if defined(USE_TRACE_STANDALONE)	//Added by JJANG 20080627
#define	MAX_MSG_CHAR	24
#elif defined(USE_ARAB_FONT) || defined(HEBREW_FONT)
#define	MAX_MSG_CHAR	17
#else
#define	MAX_MSG_CHAR	22
#endif
#else
#define	MAX_MSG_CHAR	12
#endif
#define MAX_MSG_CHAR_LONG	33 // not used in CL5200J
#define DSP_MSG_X		0
#define DSP_MSG_Y		0
#define DSP_MSG_MAGX	1
#define DSP_MSG_MAGY	1
#define DSP_MSG_FONT_ID	2	// 8x6
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
#define MAX_MSG_R_CHAR	8
#else
#define MAX_MSG_R_CHAR	12
#endif
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
#if defined(USE_ARAB_FONT) || defined(HEBREW_FONT)
#define DSP_MSG_R_X		136	// 17*8
#else
#define DSP_MSG_R_X		150	// 25*6
#endif
#else
#define DSP_MSG_R_X		0
#endif
#define DSP_MSG_R_Y		0
#define DSP_MSG_R_MAGX	1
#define DSP_MSG_R_MAGY	1
#define DSP_MSG_R_FONT_ID	2	//8x6

#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
#if defined(USE_ARAB_FONT) || defined(HEBREW_FONT)
#define MAX_DATE_CHAR	10
#define DSP_DATE_X		136	// 17*8
#else
#define MAX_DATE_CHAR	8
#define DSP_DATE_X		150	// 25*6
#endif
#else
#define MAX_DATE_CHAR	0
#define DSP_DATE_X		0
#endif
#define DSP_DATE_Y		0
#define DSP_DATE_MAGX	1
#define DSP_DATE_MAGY	1
#define DSP_DATE_FONT_ID	2	//8x6

#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
#define MAX_PLU_NAME_CHAR	20
#else
#define MAX_PLU_NAME_CHAR	12
#endif
#define DSP_PLU_NAME_X	0
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
#define DSP_PLU_NAME_Y	8
#else
#define DSP_PLU_NAME_Y	0
#endif
#define DSP_PLU_NAME_MAGX	1
#define DSP_PLU_NAME_MAGY	1
#define DSP_PLU_NAME_FONT_ID	4

#define MAX_MSG_RIDE_CHAR	3
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
#if defined(USE_ARAB_FONT) || defined(HEBREW_FONT)
#define DSP_MSG_RIDE_X	112	// 14*8
#else
#define DSP_MSG_RIDE_X	132	// 22*6
#endif
#else
#define DSP_MSG_RIDE_X	0
#endif
#define DSP_MSG_RIDE_Y	0
#define DSP_MSG_RIDE_MAGX	1
#define DSP_MSG_RIDE_MAGY	1
#define DSP_MSG_RIDE_FONT_ID	2	// 8x6

#define DSP_SALE_INFO_AREA_X1	0
#define DSP_SALE_INFO_AREA_X2	201
#define DSP_SALE_INFO_AREA_Y1	0
#define DSP_SALE_INFO_AREA_Y2 	31

//#define MAX_SCROLL_MSG_CHAR
#define DSP_SCROLL_MSG_X	0//DSP_SALE_INFO_AREA_X1
#ifdef _USE_LCD26d_16x96_
#define DSP_SCROLL_MSG_Y	0//DSP_SALE_INFO_AREA_Y1
#elif defined(_USE_LCD20848_)
#define DSP_SCROLL_MSG_Y	8//DSP_SALE_INFO_AREA_Y1
#elif defined(_USE_LCD32_)
#define DSP_SCROLL_MSG_Y	8//DSP_SALE_INFO_AREA_Y1
#else
#define DSP_SCROLL_MSG_Y	0//DSP_SALE_INFO_AREA_Y1
#endif
#define DSP_SCROLL_MSG_MAGX1	1
#define DSP_SCROLL_MSG_MAGY1	1
#define DSP_SCROLL_MSG_MAGX2	1
#define DSP_SCROLL_MSG_MAGY2	1
#define DSP_SCROLL_MSG_FONT_ID	4
#define MAX_SCROLL_EFFECT	3

#define MAX_MENU_CHAR_BUF	35

#if defined(_USE_LCD20848_)
#define MAX_STOTAL_LIST_PER_PAGE	2
#else
#define MAX_STOTAL_LIST_PER_PAGE	3
#endif

#ifdef USE_CHN_CART_SCALE
//// for test
#define TRACE_CODE_SIZE	    20
#define TRACE_DATETIME_SIZE 6
#define MAX_ARTNAME     30
#endif

#ifdef USE_SETPRODUCT_SALE
#define SETPRODUCT_SALE_START	1
#define	SETPRODUCT_SALE_COMPONENTS	2
#define	SETPRODUCT_PRINT_SETLABEL	3
#endif

#ifdef USE_HYUNDAI_SET_FUNCTION
extern INT8U MeatSetSaleMode;
extern INT8U MeatSetSaleUseScanner;
extern INT8U MeatSetLabelTransFlag;
extern INT32U MeatSetPluId;
extern INT8U MeatSetCompNum;
extern long MeatSetPrice;

#define TRNAS_TYPE_NORMAL	1
#define TRNAS_TYPE_SETPRODUCT	2
#define TRANS_TYPE_MEAT_STORAGE_LABEL	3

#define MEAT_STORAGE_LABEL_KEY_NUM	2

#define	SALE_NORMAL			0	// 일반 판매 (세트 판매 X)
#define	SALE_SET_START		1	// 선물 세트 판매 시작 
#define	SALE_SET_COMPONENTS	2	// 선물 세트 내품 판매 
#define	PRINT_SET_COMPONENTS_CLERK		3	// 보관용 내품 라벨 인쇄
#define	PRINT_SET_COMPONENTS_CUSTOMER	4	// 고객용 내품 라벨 인쇄
#define	INPUT_SET_PRICE		5	// 선물 세트 가격 입력
#define	PRINT_SET_LABEL		6	// 선물 세트 라벨 인쇄
#endif

#ifdef USE_HYUNDAI_CHECK_REASON_CODE
#define TRANS_PRICE_DC_NONE					0 // 정상
#define TRANS_PRICE_DC_EVENT_COMMON			1 // 할인 - 행사 (공통)
#define TRANS_PRICE_DC_EVENT_MEMBER			2 // 할인 - 육학다식(현대 백화점 멤버)
#define TRANS_PRICE_DC_EVENT_STORE  		3 // 할인 - 점 자체 행사 
#define TRANS_PRICE_DC_EVENT_FRESHNESS_LOSS	4 // 할인 - 선도저하/마감
#define TRANS_PRICE_DC_EVENT_CLERK	  		5 // 직원 행사
#define TRANS_DISPOSAL_RETURN 				6 // 폐기 - 반품
#define TRANS_DISPOSAL_FRESHNESS_LOSS		7 // 폐기 - 선도저하
#define TRANS_SAMPLE						8 // 폐기 - 시료/샘플
#define TRANS_MEAT_SET						9 // 세트 상품
#endif

#ifdef USE_REGIST_DISPOSAL
#define REGIST_DISPOSAL_OFF			0
#define REGIST_DISPOSAL_ON			1
#define REGIST_DISPOSAL_STAT_DISP	2
#endif

#ifdef USE_DISCOUNT_MEMBERSHIP
#define NO_DSIC					0
#define MEMBERSHIP_DISC			1
#define CARD_DISC				2
#define MEMBERSHIP_CARD_DISC	3
  #ifdef USE_LOTTEMART_DEFAULT
#define MAX_SALE_MSG_NO			170
#define DISC_ID_CAP_BASE		180

#define CAP_BEFORE_MEMBERS_DISC		191	// 비회원가
#define CAP_AFTER_MEMBERS_DISC		192	// 회원가
#define CAP_BEFORE_CARD_DISC		193	// 판매가
#define CAP_AFTER_CARD_DISC			194	// 카드가
#define CAP_BEFORE_MEBERS_AND_CARD_DISC	195 // 비회원가
#define CAP_AFTER_MEBERS_AND_CARD_DISC	196 // 최종 혜택가
  #endif
#endif

#ifdef USE_LOTTEMART_DEFAULT
#define MEAT_BACKLABEL_ID	82	// 한우 보관 라벨
#define MEAT_SETLABEL_ID	83	// 축산세트 라벨 
#define IMPORT_MEAT_BACKLABEL_ID	86	// 수입육 보관 라벨
#endif

#endif	// MODE_SALE_H
#pragma pack(4)
#ifndef SALE_OPER_TYPE_DEFINE
#define SALE_OPER_TYPE_DEFINE
typedef struct
{
	INT8U	operationClerk;	// normal, standard clerk, floating clerk
	INT8U	operationMode;	// normal, repack, manager
	INT16U	operator;
	INT8U	level;
	INT16U	logOperator;
	INT8U	logLevel;
	INT8U	modeSalePrevType;	// 0 초기, 1 by weight, 2 by count, 3 non food
	INT8U	modeSaleType;		// 0 초기, 1 by weight, 2 by count, 3 non food
	//INT8U	modeSaleSubType;	// modeSaleType에 대한 필요한 상세 모드
	//INT8U	modeScreenType;		// 전체 클리어를 하면 0으로 세팅  ,	modeSaleType(default),
	//INT8U	modePrevProcess;
	INT8U	modeProcess;		// key -> dispaly 등에 대한 흐름
	INT8U	transAddup;			// normal, add, floating
    INT8U   fixedAction;
	long	MaxUnitPrice;
	long	MaxTotalPrice;

	INT8U systemOnlyStart;
	INT8U OverTotalPrice;
	INT8U OverUnitPrice;

	INT8U	doublePLUkey;
	INT8U	doubleDatekey;	// CJK080320
	INT8U   doubleKeyTime100ms;
	//	INT8U   woldMode;
	INT8U	wAuto;		// auto,save,prepack
	INT8U	wSave;		// auto,save,prepack
	INT8U	wPrepack;		// auto,save,prepack
	INT8U	override;
	INT8U	multiply;
	INT8U   multiplyEdit;
	//DEL051229	INT8U	addMode;
	//DEL051229	INT8U	printAdd;// add or floating
	INT8U	printON;// print label
	INT8U	keyClear;
	INT8U	openCash;
	INT8U	negative;
	//DEL051229	INT8U	keyDiscount;
	INT8U	addStandatdClerk;
	INT8U	specialPrice;
	INT8U	specialPriceDisc;
	INT8U	memberPrice;
	INT8U	packingPrice;
	INT8U	directClearWeight;
	//DEL051229	INT8U	keyInWeight;
	INT8U	transactionOK;
	INT8U	unitpriceZeroOverride;
	INT8U	saveZeroOverride;
	INT8U	rewindReady;
	INT8U	fixedPrice;
	INT8U	useMultipleUnits;
	INT8U	autoDoublePrint;
	INT8U	fixedDiscFlag;
	INT8U	pressKey;
	INT8U	pressKeyForNvErase; // NVRAM Sector Erase Time 체크용
	INT8U	receiving; // NVRAM Sector Erase Time 체크용
	INT8U	insertLabelData;
	INT8U	discountInfo;
	//DEL051229	INT8U	addup;
	INT8U	salePrintWait;		//? 출력을 원하면 세팅
	INT8U	salePluClearWait;
	INT8U	UnderWeight;
	INT8U	checkin;
#ifdef USE_CHN_CART_SCALE
	INT8U	oneTouchTicketPrint;
	INT8U	useCheckRemain;
	INT8U	traceCodeType;
#endif
#ifdef USE_EMART_BACK_LABEL
	INT8U 	BackLabeledFlag;
#endif
#ifdef USE_NHMART_SAFE_MEAT
	INT8U	OfflineMode;
#endif
#ifdef USE_HYUNDAI_SET_FUNCTION
	INT8U	saleStatus;
	INT8U	meatStorageLabelFlag;
#endif
#ifdef USE_REGIST_DISPOSAL
	INT8U	RegistDisposalFlag;
#endif
} SALE_OPER_TYPE;
#pragma pack()
#endif

#ifndef DISC_DATA_TYPE_DEFINE
#define DISC_DATA_TYPE_DEFINE
//#pragma pack(2)
#pragma pack(4)
typedef struct
{
	INT16S		code;

	long	fromDate;
	long	toDate;
	long	fromTime;
	long	toTime;
	INT8U	week;

	INT8U	discOriginType;
	INT8U	discTargetType;
	INT8U	discPriceType;

	long	discTarget[2];
	long	discPrice[2];
}DISC_DATA_TYPE;
#pragma pack()
#endif

#ifndef PRICE_TYPE_DEFINE
#define PRICE_TYPE_DEFINE
#pragma pack(4)
typedef struct
{
	long	Weight[3];
	INT16S  	Qty[3];
	INT16S  	UnitCount[3];
	long	UnitPrice[3];
	long	TotalPrice[3];

	INT8U	ExTax;
	INT8U	TaxNumber;
	//DEL051229	INT16U	RoundedKind	:1;	//label/ticket roundedPrice
	/////////
	long	TaxRate;
	long	TaxPrice;
	long	DisplayedPrice;
	//long	RoundedPrice;
	long	FixedDiscountPrice;
	long	PercentTareRate;
	long	PercentTareWeight;
	long	GrossWeight;
	long	NetWeight;
	long	WeightReference;
	INT8U	GrossWeightFlag;
}PRICE_TYPE;

typedef struct
{
	INT16U	CurrencyKeyCode;	// currency 적용 No.
	INT8U	CurrencyIndex;	// currency 적용 No.
	INT8U	CurrencyUseDefault;
	long	CurrencyRate;
	INT8U	CurrencyRateDiv;
	////	long 	CurrencyPrice;
	////	long 	CurrencyUnitPrice;
	long	CurrencyPriceH8;
	long 	CurrencyPriceL8;
	long 	CurrencyUnitPriceH8;
	long 	CurrencyUnitPriceL8;	// Added by CJK
	INT8U	CurrencyPriceDiv;
	char	CurrencyString1[5];
	char	CurrencyString2[5];
	INT8U	CurrencyPrtForm;	// Added by CJK

}CURRENCY_PRICE_TYPE;
#pragma pack()
#endif

#ifndef DISC_SALE_TYPE_DEFINE
#define DISC_SALE_TYPE_DEFINE
#pragma pack(4)
typedef struct
{
	INT16S   	saleCreditCustomer;
	INT8U	saleDiscountType;
	INT8U	saleIndexOk;
	INT8U	saleGlobalOk;
	INT8U	saleDiscOk;
	INT8U   saleDiscFSP;  //SG061212
	DISC_DATA_TYPE	discIndexData;
}DISC_SALE_TYPE;
#pragma pack()
#endif

#ifndef PREPACK_STATUS_TYPE_DEFINE
#define PREPACK_STATUS_TYPE_DEFINE
#pragma pack(2)
typedef struct
{
	INT16U CurDeptNo;
	INT32U CurPluNo;
	INT32U PrevPluNo;
	INT32U PrevSubPluNo;
	INT8U  OneItem_Grand;
	INT8U  OneItem_Sub;
	//	INT8U  OneItem;
}PREPACK_STATUS_TYPE;	// NVRAM에 위치
#pragma pack()
#endif

#ifndef TRANSACTION_STATUS_DEF
#define TRANSACTION_STATUS_DEF

//#define	TRANS_TYPE_NORMAL		0
//#define	TRANS_TYPE_NEGATIVE		1
//#define	TRANS_TYPE_RETURN		2
//#define	TRANS_TYPE_VOID			3
//#define	TRANS_TYPE_ADD			6
//#define	FLAG_TRANS_REPACK   0x01
//#define	FLAG_TRANS_PREPACK  0x02
//#define	FLAG_TRANS_LABEL    0x04
//#define	FLAG_TRANS_OVERRIDE 0x08
//#define	FLAG_TRANS_SELF     0x10

#define	TRANS_TYPE_NORMAL		0
#define	TRANS_TYPE_REPACK		1
#define	TRANS_TYPE_SELF			2
#define TRANS_TYPE_BACK_LABEL_SALE	0x40
#define TRANS_TYPE_OFFLINE		0x20
#ifdef USE_SETPRODUCT_SALE
#define TRANS_TYPE_SETPRODUCT	0x40 //세트 상품 데이터. 구성품(X)
#endif

#define FLAG_TRANS_NEGATIVE 0x01
#define FLAG_TRANS_RETURN   0x02
#define FLAG_TRANS_VOID     0x04
#define FLAG_TRANS_PREPACK  0x08
#define FLAG_TRANS_LABEL    0x10
#define FLAG_TRANS_OVERRIDE 0x20
#define FLAG_TRANS_ADD		0x40
#define FLAG_TRANS_NOVOID	0x80

#define FLAG2_TRANS_LOCAL	0x01
#define FLAG2_TRANS_DISCON	0x02
#define FLAG2_TRANS_CHECKIN	0x04
#define FLAG2_UPPER_NIBBLE_MASK	0xf0	// 국내 농협 전용

#define FLAG3_TRANS_NONE			0
#define FLAG3_TRANS_WEIGHT			1
#define FLAG3_TRANS_FIXED   		2
#define FLAG3_TRANS_CHG_WEIGHT  	3
#define FLAG3_TRANS_CHG_FIXED  		4
#define FLAG3_TRANS_DC_WEIGHT	  	5
#define FLAG3_TRANS_DC_FIXED 		6
#define FLAG3_TRANS_DC_CHG_WEIGHT 	7
#define FLAG3_TRANS_DC_CHG_FIXED	8
#endif

#ifndef TRANSACTION_RECORD_TYPE_DEFINE
#define TRANSACTION_RECORD_TYPE_DEFINE
#pragma pack(2)
typedef struct
{
	INT8U	trtType;		// normal, return, void ...
	INT8U	trtScaleId;
	INT8U   trtSerialNo;	// add new item for void number
	INT8U   trtStatus;		// b0,...
	INT8U	trtPLUtype;
	INT8U	trtDept;		// <--- INT16U에서
	INT32U  trtPLU;
	INT8U	trtGroup;
	INT8U	trtOperator;
	long	trtCode;
	long	trtWeight;
	INT16S		trtQty;
	INT16S		trtPCS;
	long	trtUnitPrice;		// unit price
	long	trtVolume;			// total price
	long	trtDiscountVolume;	// sale price -> origin price - trtVolume
	INT16S		trtMultiple;		// kg/lb(CJK 20060510)
	long	trtTransactionCounter;
	INT8U	trtTaxType;			//internal,external
	INT8U	trtTaxNumber;
	long	trtTax;				// tax percent에 대한 세금
	long	trtTicketNumber;
	//INT16S		trtStatus2;		// CJK080320 : trtBonusPoint => trtStatus2
	INT8U	trtStatus2;			// 2byte -> 1byte : CJK080725
	INT8U	trtIndividualIndex;	// only Korea => 상위1bit : lotFlag(개체 or 묶음), 하위7bit : 개체인덱스
	INT16S		trtCreditCustomerNo;
#ifdef USE_GERMANY_TICKET_DISCOUNT
	INT16S		trtDiscountSavedvalue;
#endif
#ifdef USE_CHN_CART_SCALE
	INT8U	trtTraceCode[TRACE_CODE_SIZE];
#endif
#ifdef USE_HYUNDAI_CHECK_REASON_CODE
	INT8U trtReaseonCode;
#endif
#ifdef USE_HYUNDAI_MEAT_FUNCTION
	INT8U trtBarcodeStr[30];
#endif
#ifdef USE_CL5J_REALTIME_TRNASFER
	long trtTareWeight;
#endif
} TRANSACTION_RECORD_TYPE;

typedef struct
{
#if defined(USE_IMPORT_LOTNO) || defined(USE_TRACE_1000QTY)
	INT8U	indivNo[24];	// INDIVIDUAL_NO_LEN_TRANS
#else
	INT8U	indivNo[20];	// INDIVIDUAL_NO_LEN_TRANS
#endif
	INT8U	indivLen;
	INT8U   reserved;
} TRANSACTION_RECORD_SUB_TYPE;

typedef struct
{
	INT8U	sdtType;
	INT8U	sdtScaleId;
	INT8U	sdtPLUtype;
	INT8U	sdtDept;
	INT32U	sdtPLUNo;
	long	sdtItemCode;
	long	sdtWeight;
	INT16S	sdtQty;
	INT16S	sdtPCS;
	long	sdtUnitPrice;
	long	sdtTotalPrice;
	long	sdtDiscountPrice;
	long	sdtTransactionCounter;
	INT16U	sdtPrefix;
	INT8U	sdtStatus;
	INT8U	sdtDate[6];
	INT8U	sdtSaleDate[3];
#ifdef USE_EMART_BACK_LABEL
	INT8U	sdtBarcodeStr[30];
#else
	INT8U	sdtBarcodeStr[20];
#endif
#ifdef USE_CL5J_REALTIME_TRNASFER
	long	sdtTareWeight;
	INT8U	sdtGroup;
	INT8U	sdtOperator;
	INT8U	sdtTraceNo[14];
#else
	INT8U	sdtTraceNo[20];
#endif
	INT8U	sdtSlaughterHouse;
#ifdef USE_EMART_NETWORK
	INT8U	sdtMeatUse;
#else
	INT8U	Reserved;
#endif
} TRANSACTION_SALE_DATA_TYPE;

typedef struct
{
	INT8U	cdtType;
	INT8U	cdtScaleId;
	INT8U	cdtReasonCode;
	INT8U	cdtDept;
	INT32U	cdtPLUNo;
	long	cdtOriginUnitPrice;
	long	cdtResultUnitPrice;
	long	cdtItemCode;
	INT16S	cdtQty;
	long	cdtWeight;
	long	cdtTotalPrice;
	INT8U	cdtDate[6];
#ifdef USE_EMART_BACK_LABEL
	INT8U	cdtBarcodeString[30];
#else
	INT8U	cdtBarcodeString[20];
#endif
	INT16U	cdtPrefix;
	INT8U	cdtChangeFlag;
	INT8U	Reserved[31];
} TRANSACTION_CHANGE_DATA_TYPE;

typedef struct
{
	INT8U	adtDate[6];
	INT16U	adtPrefix;
	INT8U	adtPluName[54];
	long	adtItemCode;
	INT8U	adtSaleDate[3];
	INT8U	Reserved;
} TR_ADDITION_DATA_TYPE;

// bmlim
typedef struct {
	INT8U	trtType;
    INT8U   scaleid;
	INT8U   clerkid;
    long    paidprice[MAX_PAID_TYPE];
	long    change;
	long    summaryround;
	long	summarydiscount;	//Added by JJANG 20080416, SUBTOTAL에서 할인된 값
#ifdef USE_DISCOUNTED_TAX_FOR_DISCOUNTED_TOTAL
	long	taxPrice[MAX_TAX_NO];
	long	taxGross[MAX_TAX_NO];
#endif
#ifdef USE_INDIA_FUNCTION
	long	ticketNo;
	long	transCount;
#endif
} TRANSACTION_PAY;

typedef struct
{
	INT8U	trtScaleId;
	INT8U	trtDept;
	INT32U	trtPLUNo;
	long	trtOriginUnitPrice;
	long	trtResultUnitPrice;
	INT8U	trtReasonCode;
	INT8U	trtClerkId;
	INT8U 	trtChangeFlag;
	INT8U	Reserved;
} UNITPRICE_CHANGE_TYPE;	//Added by JJANG 20090113


typedef struct
{
	INT8U	sdtType;
	INT8U	sdtScaleId;
	INT8U	sdtPLUtype;
	INT8U	sdtDept;
	INT32U	sdtPLUNo;
	long	sdtItemCode;
	long	sdtWeight;
	INT16S	sdtQty;
	INT16S	sdtPCS;
	long	sdtUnitPrice;
	long	sdtTotalPrice;
	long	sdtDiscountPrice;
	long	sdtTransactionCounter;
	INT16U	sdtPrefix;
	INT8U	sdtStatus;
	INT8U	sdtDate[6];
	INT8U	sdtSaleDate[3];
#ifdef USE_EMART_BACK_LABEL
	INT8U	sdtBarcodeStr[30];
#else
	INT8U	sdtBarcodeStr[20];
#endif
	INT8U	sdtItemCode13[16];
	long	sdtTicketNo;
	INT8U	Reserved;
	INT8U	Reserved1;
} TRANSACTION_SALE_DATA_TYPE8;

#pragma pack()


#endif

#ifdef USE_SEND_TRACE_PRICE
  #ifndef REALTIME_TRACEPRICE_TYPE_DEFINE
#define REALTIME_TRACEPRICE_TYPE_DEFINE
#pragma pack(2)
typedef struct {
	INT32U trace_meatcode;	// 4
	INT32U trace_price;		// 8
	INT16U trace_grade_no;	// 10
	INT16U trace_unit;		// 12		
	INT8U pluname[40];		// 52
	INT8U  trace_date[6];	// 58
	INT8U  trace_code[12];	// 70
	INT8U  reserved[10];	// 80	
	INT8U  trace_grade;		// 81
} REALTIME_TRACEPRICE;
#pragma pack()
  #endif
#endif

#ifdef USE_REALTIME_TRANS_TYPE3
//SG071009
#ifndef REALTIME_TRANSACTION_TYPE_DEFINE
#define REALTIME_TRANSACTION_TYPE_DEFINE
#pragma pack(2)
typedef struct {
	TRANSACTION_RECORD_TYPE contents;
	INT8U pluname1[40];
	INT8U pluname2[40];
	INT8U pluname3[40];
	INT8U barcode_data[32];
	INT8U additional_info1[32];
	INT8U additional_info2[32];
	INT8U individual_number[20];
	INT8U grade[10];
	INT8U slaughtDate[4];
	INT8U breed[10];
	INT8U farm[20];
	INT8U reserved[8];
} REALTIME_TRANSACTION_TYPE;
#pragma pack()
#endif
#endif


#ifndef TRACE_TRANS_TYPE_DEFINE
#define TRACE_TRANS_TYPE_DEFINE
#pragma pack(2)
typedef struct {
	INT8U traceType;	// 식별번호구분, 0:개체번호, 1:묶음번호
	INT8U traceNo[20 + 1];	// 식별번호,		// INDIVIDUAL_NO_LEN_TRANS
	INT8U sendType;		// 인쇄여부, 0:호출, 1:인쇄
	INT32U weight;		// 중량
	INT32U unitPrice;	// 단가
	INT32U totalPrice;	// 판매가격
	INT8U pluName[64];	// 상품
} TRACE_TRANS_TYPE;
#pragma pack()
#endif

#ifdef USE_HYUNDAI_MEAT_FUNCTION
  #ifndef TRANSACTION_SALE_DATA_TYPE14_DEFINE
#define TRANSACTION_SALE_DATA_TYPE14_DEFINE
#pragma pack(1)
typedef struct
{
	INT8U  sdtSaleType;
	INT8U  sdtScaleId;
	INT32U sdtPLUNo;
	long sdtSETSerial;
	long sdtWeight;
	INT16S sdtQty;
	long sdtUnitPrice;
	long sdtTotalPrice;
	long sdtDiscountPrice;
	INT8U sdtSaleTime[3]; // [0]:hour, [1]:minutes, [2]:seconds
	INT8U sdtDept;
	INT8U sdtSaleDate[3]; // [0]:year, [1]:month, [2]:date
	INT8U sdtBarcodeStr[30];
	INT8U sdtReceiptNo[22];
	INT8U sdtReasonCode;
	INT8U sdtSETCompNum; 
	INT8U reserved;   
} TRANSACTION_SALE_DATA_TYPE14;
#pragma pack()
  #endif
#endif

#ifndef PLU_BATCH_TYPE_DEFINE
#define PLU_BATCH_TYPE_DEFINE
#pragma pack(2)
typedef struct
{
	INT8U	dept;
	INT32U	plu;
	INT8U	printCount;
}PLU_BATCH_TYPE;
#pragma pack()
#endif

#ifndef PLU_SALES_SUMMARY_TYPE_DEFINE
#define PLU_SALES_SUMMARY_TYPE_DEFINE
#pragma pack(2)
#ifdef USE_INDIA_FUNCTION
typedef struct
{
	INT32U	pstPLU;
	INT8U	pstDept;
	INT16S	pstTransactions[MAX_REPORT_PERIOD];
	INT16S	pstCountNumber;
	long	pstVolume[MAX_REPORT_PERIOD];
	long	pstWeight[MAX_REPORT_PERIOD];
	long	pstQty[MAX_REPORT_PERIOD];

	INT16U	pstPrev; // index into array
	INT16U  pstNext;
	INT8U	pstPLUtype;
	long	pstPrepackVolume[MAX_REPORT_PERIOD];
	long	pstPrepackWeight[MAX_REPORT_PERIOD];
	long	pstTax[MAX_REPORT_PERIOD];

	long	pstInventory;
	long	pstInvGross;
	long	pstInvBagWt;
}	PLU_SALES_SUMMARY_TYPE; //PLU sales summary:
#else
typedef struct
{
	INT32U	pstPLU;
	INT8U	pstDept;
	INT16S		pstTransactions[MAX_REPORT_PERIOD];
	INT16S		pstCountNumber;
	long	pstVolume[MAX_REPORT_PERIOD];
	long	pstWeight[MAX_REPORT_PERIOD];
	long	pstQty[MAX_REPORT_PERIOD];

	INT16U	pstPrev; // index into array
	INT16U  pstNext;
	INT8U	pstPLUtype;
#if !(ROM_COUNTRY == ROM_RUS)
	long	pstCountOfLabelVolume[MAX_REPORT_PERIOD];
	INT16U	pstCountOfLabel[MAX_REPORT_PERIOD];
#endif
	long	pstPrepackVolume[MAX_REPORT_PERIOD];
	long	pstPrepackWeight[MAX_REPORT_PERIOD];
#if !(ROM_COUNTRY == ROM_RUS)
	long	pstTax[MAX_REPORT_PERIOD];
#endif
}	PLU_SALES_SUMMARY_TYPE; //PLU sales summary:
#endif
#pragma pack()
//typedef struct
//{
//	long	pstCountOfLabelVolume[MAX_REPORT_PERIOD];
//	INT16U	pstCountOfLabel[MAX_REPORT_PERIOD];
//	long	pstPrepackVolume[MAX_REPORT_PERIOD];
//	long	pstPrepackWeightOrPieces[MAX_REPORT_PERIOD];
//}	PLU_SALES_SUMMARY2_TYPE; //PLU sales summary: 62 byte
//
#endif

#ifndef SALES_SUMMARY_TYPE_DEFINE
#define SALES_SUMMARY_TYPE_DEFINE
#pragma pack(2)
typedef struct
{
	//	INT16S		sstTransactions[MAX_REPORT_PERIOD];
	INT16S	sstTransactions[MAX_REPORT_PERIOD];
	long 	sstVolume[MAX_REPORT_PERIOD];
	long 	sstWeight[MAX_REPORT_PERIOD];
	long	sstQty[MAX_REPORT_PERIOD];
}	SALES_SUMMARY_TYPE; // 12 byte
#endif
#pragma pack()
#ifndef SALES_BASIC_REPORT_TYPE_DEFINE
#define SALES_BASIC_REPORT_TYPE_DEFINE
#pragma pack(2)
typedef struct
{
	INT16S		brtTransactions;
	long 	brtVolume;
	long 	brtWeight;
	long	brtQty;
}	BASIC_REPORT_TYPE; // 12 byte
#pragma pack()
#endif

#ifndef TRANS_CHAIN_DEFINE
#define TRANS_CHAIN_DEFINE
#pragma pack(2)
struct TRANS_CHAIN
{
	unsigned char flag;
	unsigned char delete; // void flag
	TRANSACTION_RECORD_TYPE  content;
	//struct TRANS_CHAIN  *next;
	INT16U next;
};
#pragma pack()
#endif

#if defined(USE_INDIA_FUNCTION) || defined(USE_SEND_TICKET_DATA) || defined(USE_SETPRODUCT_SALE) || defined(USE_HYUNDAI_SET_FUNCTION)
#ifndef TRANS_ADD_CHAIN_DEFINE
#define TRANS_ADD_CHAIN_DEFINE
#pragma pack(2)
struct TRANS_ADD_CHAIN
{
  TR_ADDITION_DATA_TYPE  addtion_data;
  INT16U next;
};
#pragma pack()
#endif
#endif

#if defined(USE_INDIA_FUNCTION) || defined(USE_SEND_TICKET_DATA) || defined(USE_SETPRODUCT_SALE) || defined(USE_HYUNDAI_SET_FUNCTION)
#pragma pack()
typedef struct TRANS_ADD_CHAIN TRANS_ADD_CHAIN;
#endif

#ifndef SALE_G_PRICE_TYPE_DEFINE
#define SALE_G_PRICE_TYPE_DEFINE
#pragma pack(4)
typedef struct
{
	INT16U	roundPrice;
	INT16U	roundBarcode;
	INT16U 	roundWeight;
	INT8U	roundWeightType; // 0=display weight round off, 1=display weight round down
	INT8U	roundApply;
	INT8U	roundUp;
} SALE_G_PRICE_TYPE;
#pragma pack()
#endif

#ifndef SALE_G_HOLD_FUNC_DEFINE
#define SALE_G_HOLD_FUNC_DEFINE
#define HOLD_BUFFER_NUM  30
#pragma pack(4)
typedef struct
{
	long			buffer[HOLD_BUFFER_NUM];
	INT16U 			holdStartTime100ms;
	INT16U 			count;
	INT8U 			holdOperationTime;
	INT8U			flagCalcOK;
} SALE_G_HOLD_FUNC;
#pragma pack()
#endif

#ifndef SALE_G_OPER_TYPE_DEFINE
#define SALE_G_OPER_TYPE_DEFINE
#pragma pack(4)
typedef struct
{
	INT8U	keyClearTime;
	//DEL070130	INT8U	keyAutoCallPlu;
	INT8U	weightDirectClear;
	INT8U	weightZeroPricePrint;
	INT8U	printOneTimeAfterCall;
	INT8U	startSaveMode;
	INT8U	byWeightMulti;
	INT8U	byCountWeight;
	//INT8U	nonFoodWeight		:1;
	INT8U	byCountApplyWeight;
	INT8U	saveChangeType;
	INT8U	saveChangePrice;
	INT8U	priceAddExTax;
	INT8U	viewDiscount;
	INT8U	autoDoublePrint;
	INT8U	XZreport;
	INT8U	PrepackReport;
	//DEL051229	INT8U	enableByCount;
	INT8U	applyUnitpriceForDisc;

	INT8U	PrintUnderMinWeight;
	INT8U	PrintFixedItemAlways;
	INT8U	PrintUnderMinWeightOnPrepack;
	INT8U	PrintUnderMinWeightOnAUTO;
	INT8U	PrintWhenchangingPLUInAUTO;
	INT8U	PrintZeroTotalPrice;    

	INT8U   UseNegWeightLimit;

	INT8U	EnableMultiKgKey;
	INT8U	EnableKgLb;
#ifdef USE_PRT_CONVERT_WEIGHT_KG_G
	INT8U	EnableGramWeight;
#else
	INT8U	EnableAutoConversionKgLb;
#endif
	INT8U	EnableMultiLbKey;

	INT8U	UseFSP; //SG061214
	INT8U	EnableQuickPlu;	//CJK070130

	INT8U	realtimeTransactionType;

	INT8U 	ClearOpenedClerkTransaction;
	INT8U   useSpecialPriceForDisc;

	INT8U	TraceabilityCallType;
	INT8U   UseSendTraceInformation;

	INT8U   PluReportType;	//JJANG 20090408

	INT8U	TriangleType; // triangle 표시 타입. param 865.

	INT8U   UseAutoZero;
    INT8U   send_speedkey_position;
} SALE_G_OPER_TYPE;
#pragma pack()
#define RTsend_none					0
#define RTsend_basic_kr				1
#define RTsend_basic_indiv_kr		2
#define RTsend_full_kr				3
#define RTsend_EMART_NH_kr			4
#define RTsend_basic_name_indiv_kr	5
#define RTsend_importmeat_kr		6
#define RTsend_cart_cn				7
#define RTsend_trace_cn				8


#endif

#ifndef SALE_G_FLOAT_TYPE_DEFINE
#define SALE_G_FLOAT_TYPE_DEFINE
#pragma pack(4)
typedef struct
{
	INT8U	logOffClerkTime;
	INT8U	logOffTransaction;
	INT8U	printLabel;
	INT8U	printList;
	INT8U	printBarcode;
	INT8U	printTotalLabel;
	INT8U	printBarcodeForList;
	INT8U	printStoreName;
	INT8U	printClerkName;
	INT8U	printTaxInfo;
	INT8U	printCopyTicket;
	INT8U	printCopyList;
	INT16S		advertiseLabel[2];
	INT8U	printFontSize[4];
} SALE_G_FLOAT_TYPE;
#pragma pack()
#endif

#ifndef SALE_G_ADD_TYPE_DEFINE
#define SALE_G_ADD_TYPE_DEFINE
#pragma pack(2)
typedef struct
{
	INT8U	printLabel;
	INT8U	printTotalLabel;
} SALE_G_ADD_TYPE;//6 bytes
#pragma pack()
#endif

#ifndef SALE_G_PRINT_TYPE_DEFINE
#define SALE_G_PRINT_TYPE_DEFINE
#pragma pack(2)
typedef struct
{
	//DEL070705	INT8U	printCheckPeelOff;
	INT8U	printNoRewind;
	INT8U	directWeight;
	INT8U	directNonfood;

	INT8U	reportType;
	INT8U	reportZeroValue;
	INT8U	reportTax;
	INT8U	reportPay;
	INT8U	reportRoundedSummary;
	INT8U	reportStoreName;

	INT8U	reportRepack;
	INT8U	reportPrepack;
	INT8U	reportNegative;
	INT8U	reportOpenCash;
	INT8U 	reportChange;
} SALE_G_PRINT_TYPE;
#pragma pack()
#endif

#ifndef SALE_G_SERIAL_TYPE_DEFINE
#define SALE_G_SERIAL_TYPE_DEFINE
#pragma pack(2)
typedef struct
{
	char	serialTransaction[20];
	char	serialTicket[20];
} SALE_G_SERIAL_TYPE;//40 bytes
#pragma pack()
#endif

#ifndef TARE_OPER_TYPE_DEFINE
#define TARE_OPER_TYPE_DEFINE
#pragma pack(4)
typedef struct
{
	long	tarePLU;
	long	tareKey;
	long	tareWeight;
	long	tareClerk;	// clerktare
	long	addupWeight;
	long	saveKeyTare;	// key tare 보관 for plu
	INT8U	addupWeightZero;
	INT8U	canadianTare;
	INT8U	clearKeyTare;
	INT8U	clearWeightTare;
	INT8U	overridePluTare;
	INT8U	continuousTareUp;
	INT8U	continuousTareDn;
	INT8U	saveKeyTareForPLU;

	INT8U	displayTareEU;

	INT8U	flagAutoClearTare;
	INT8U	useTareAfterPLUCall;
	INT8U	displayWeightTare;
} TARE_OPER_TYPE;
#pragma pack()
typedef struct TRANS_CHAIN TRANSACT_CHAIN;
#endif

#ifndef PRINT_OPER_TYPE_DEFINE
#define PRINT_OPER_TYPE_DEFINE
#pragma pack(4)
typedef struct
{
	INT8U	IndividualClear;
	INT8U	EnPrtZReport;
	INT8U	PrtRepackSymbol;
	INT8U	PrtLastTicket;
} PRINT_OPER_TYPE;
#pragma pack()
#endif

#ifdef USE_CHN_CART_SCALE

#ifndef REPORT_LOG_STRUCT_DEFINE
#define REPORT_LOG_STRUCT_DEFINE
#pragma pack(1)
typedef struct
{
	/*
	 //2 (x) dept no index when send to PC, this data make from scale (dept's name. ex) name <- 123 (string)
	 //4 (o) plu No
	 //4 (o) item code
	 //1 (o) sale type
	 //1 (o) plu type (ex. weight or count plu)
	 //31(x) plu name when send to PC, make the data
	 //4 (o) ticket serial no
	 //20(o) trace no //when memory short -> high and low. long type until 18 digits (ex. 6789123110407(4digit=store no, 3 digit=shop no, 6digit=date, 1231122334 (3 digit=shop no, 7 digit=trace no)
	 //6 (o) date/time yy/mm/dd/hh/mm/ss
	 //4 (o) weight or count
	 //4 (x) origin unit price
	 //4 (o) sale unit price
	 //4 (x) origin total price
	 //4 (o) sale total price
	 //10(o) reserved
	 // total = 60 byte * 1000 = 60000 byte (buffer cnt = 1000 ea)
	 */
    INT32U pluNo;       // 4
    INT32U itemNo;      // 4
    INT8U saleType;     // 1 add, void
    INT8U pluType;      // 1
    INT32U ticketNo;    // 4
    INT8U traceCode[20];//20
    INT8U year;         // 1
    INT8U month;        // 1
    INT8U day;          // 1
    INT8U hours;        // 1
    INT8U minutes;      // 1
    INT8U seconds;      // 1
    INT32U weightOrcount;// 4
    INT32U uprice;      // 4
    INT32U tprice;      // 4
} REPORT_LOG_STRUCT;
#pragma pack()
#endif


#ifndef LOG_UPLOAD_INF_DEFINE
#define LOG_UPLOAD_INF_DEFINE
#pragma pack(1)
typedef struct{
    INT8U  dt[TRACE_DATETIME_SIZE];     //6
    INT8U  traceCode[TRACE_CODE_SIZE];  //20
    INT8U   name[MAX_ARTNAME];          //30
    INT32U  pluNo;      //4
    INT16U  stallNo;    //2
    INT32U  unitPrice;  //4
    INT32U  weight;     //4
    INT32U  totalPrice; //4
} LOG_UPLOAD_INF;
#pragma pack()
#endif

#endif

#pragma pack()
extern void  mode_sale(void);
extern void  mode_sale_init(void);
extern void  sale_auto_check(void);
extern void  sale_prepack_check(void);
extern void  sale_weight_check(void);
extern void  sale_auto_clear(void);
extern void  sale_auto_zero(void);
////extern void  sale_plu_clear(void);
extern void  sale_auto_print(void);

extern INT8U  sale_adm_proc(void);
extern void  sale_key_proc(void);
extern void  sale_display_proc(INT8U onlyWeighing);
extern void  sale_display_update(INT16U cc);
extern void  sale_display_update_clear(void);
extern INT8U sale_display_date(INT16S displayFlag);

extern void  sale_display_update_error(INT16U code,INT8U nodisp,long val);
extern void sale_display_error(void);
extern INT16U sale_key_get_action(void);
extern void  sale_key_time_reset(void);
//DEL CJK070319extern void  sale_err_time_reset(void);
extern INT8U sale_key_time_clear(void);
//DEL CJK070319extern INT8U sale_err_time_clear(void);
extern void  sale_discount(void);
extern void  sale_print_cancel(void);
extern INT16S  sale_print_monitor(void);
//CHGextern INT16S  sale_print_start(INT8U v);
extern INT16S  LabelAndTransaction(void);
extern INT16S  SaleAcctLocalAddTransaction(INT8U dept_id,INT32U plu_id,PRICE_TYPE *pPrice,INT8U saleType);
extern INT16S  CheckTimeOver(INT16U delay,INT16U checkTime);
extern INT16S  CheckTimeOver10ms(INT16U delay,INT16U checkTime);
extern void	ChangePluType(INT8U	stype);
extern INT16S ClerkVoidTransaction(unsigned char clerkNum, INT16S index, INT8U deleteFlag, INT8U scaleid, INT8U recvmode);
extern INT16S	DecimalPointPrice(void);
extern INT16S	DecimalPointWeight(void);
// BMLIM extern long CalcWeightBasedPrice(INT16S Decimals,long maxPrice,long  wt,long unitprice);
extern long CalcWeightBasedPrice(long maxPrice,long  wt,long unitprice);
extern void Key_DelaySec(INT16S delay,INT16U preTime);
extern void	LoadGlobalSetting(void);
extern INT8U PluType();
extern INT16S TareClearKey(void);
extern INT16S TareClearWeight(void);
extern INT16S TareClearPLU(void);
extern INT16U	KeyWait(INT16U key1,INT16U key2,INT16U key3,INT16U key4, INT16U key5);
//extern INT8U	LastClerk(void);
extern long  longmuldiv(long val,long mul,long div);
extern long  longmuldivNoRound(long val,long mul,long div);
extern void CheckDisplayMsg(void);
extern void sale_display_msg2(void);
extern INT16S CheckPeelHead(INT8U check_peeloff, INT8U clear_msg);
extern INT16S	PutString(INT16S y, INT16S x, char *data, char font_id, char magy, char magx, INT16S n);
extern INT8U GetChangeFlag(INT8U curPluType, INT8U orgPluType, INT32U curPrice, INT32U orgPrice, INT8U curFixed, INT8U orgFixed, INT32U dcPrice);
extern INT8U  OnOff(INT8U tmp);

extern void get_nvram_summary_addr(INT32U base, INT16U a, SALES_SUMMARY_TYPE *sst);
extern void set_nvram_summary_addr(INT32U base, INT16U a, SALES_SUMMARY_TYPE *sst);
extern void get_nvram_trans_addr(INT32U base, INT16U a, TRANSACT_CHAIN *tc);
extern void set_nvram_trans_addr(INT32U base, INT16U a, TRANSACT_CHAIN *tc);
#if defined(USE_INDIA_FUNCTION) || defined(USE_SEND_TICKET_DATA) || defined(USE_HYUNDAI_SET_FUNCTION)
extern INT32U get_nvram_transAdd_addr(INT32U base,INT16U a, TRANS_ADD_CHAIN *tac);
#endif
extern void get_nvram_plu_summary(INT32U base, INT16U a, PLU_SALES_SUMMARY_TYPE *pst);
extern void set_nvram_plu_summary(INT32U base, INT16U a, PLU_SALES_SUMMARY_TYPE *pst);
extern INT32U get_TicketTransactionNumber(INT16U clekr_id);
extern void set_TicketTransactionNumber(INT16U clekr_id, INT32U value);
extern void get_PrepackStatus(PREPACK_STATUS_TYPE *prepackStatus);
extern void set_PrepackStatus(PREPACK_STATUS_TYPE *prepackStatus);
#ifdef USE_SUBTOTAL_PRICE_9DIGIT_LIMIT
extern INT8U sale_check_subtotal_price(INT8U clerk_id, long total_price);
#endif

//extern SALE_OPER_TYPE	 Operation;
extern NEARDATA SALE_OPER_TYPE	 Operation;//HYP 20060412 _huge --> _near for small code size
extern SALE_G_PRICE_TYPE	GlbPrice;
extern SALE_G_OPER_TYPE	GlbOper;
extern SALE_G_FLOAT_TYPE	GlbFloat;
extern SALE_G_ADD_TYPE		GlbAddup;
extern SALE_G_PRINT_TYPE	GlbPrint;
extern TARE_OPER_TYPE		TareOper;
extern PRINT_OPER_TYPE		PrtOper;
extern SALE_G_HOLD_FUNC     GlbHold;

extern INT8U CallQuickPlu;
extern INT8U ReadyQuickPlu;
extern INT8U InputQuickPluState;
extern INT16U QuickPluKeyTime;

extern INT8U Clerk_Subtotal_List;		//Modify by se-hyung for Clerk Subtotal List 20070727
extern INT8U Pageupdown;			//Modify by se-hyung for Clerk Subtotal List 20070727
extern INT8U Display_flag;
extern INT8U SpeedKeyPressed; //csh 20140304
extern INT16U PutXStringOffsetX;
extern INT16U PutXStringOffsetY;

#ifdef USE_DISPLAY_COMM_SPEED_SETTING
extern INT8U DispRefreshIntvl;
#endif

extern INT8U DispUpdateAll;
#ifdef USE_TRACE_STANDALONE
extern INT8U	FuncKeyInputFlag;
#endif
#ifdef USE_TEST_AUTO_LABEL_PRINT
extern INT8U PowerStartFlag;
#endif
extern INT8U UseEmartNetwork;
#ifdef USE_TRACE_STANDALONE
extern INT8U UseCheckIndivNo;
#endif
extern TRANSACTION_PAY ReopenedTransPay;

#if defined(USE_INDIA_FUNCTION) || defined(USE_SEND_TICKET_DATA)
extern INT8U UseAreaScaleMode;
extern INT8U AllowNegativeInventory;
extern INT8U RealtimeAddDataFlag;
#endif

extern INT8U PrtBarcodePriceAddExTaxFlag;

#if defined(USE_NHMART_SAFE_MEAT) || defined(USE_CHECK_INDIV_INFO)
extern INT8U checkMtrace;
#endif

#ifdef USE_HYUNDAI_MEAT_FUNCTION
extern INT8U HyundaiMeatFlag;
extern INT8U ReceiptData[31+1];	// > scanner format len
extern INT8U BarcodeData[30+1]; // > Barcode format len
#endif
#ifdef USE_REGIST_DISPOSAL
extern void sale_display_disposal(INT8U mode);
#endif
#ifdef USE_HYUNDAI_SET_FUNCTION
extern void sale_hyundai_meatset_proc(void);
extern INT16S hyundai_print_set_customer(void);
extern void hyundai_sale_set_components(void);
#endif
#ifdef USE_CTS_FUNCTION
extern void CtsAlarmProc(void);
#endif

#ifdef USE_SETPRODUCT_SALE
extern void sale_setproduct_Proc(void);
extern long SetproductPrice;
extern INT8U setproduct_sale_state;
#endif
#if defined(USE_SETPRODUCT_SALE) || defined(USE_HYUNDAI_SET_FUNCTION)
extern INT16U get_setproduct_number(void);
extern void set_setproduct_number(INT16U v);
extern void increase_setproduct_number(void);
#endif
