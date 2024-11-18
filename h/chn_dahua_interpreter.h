/*****************************************************************************
|*			
|*  Copyright		:	(c) 2015 CAS
|*  Filename		:	chn_dahua_interpreter.h
|*  Version			:	0.1
|*  Programmer(s)	:	Lee Young Hoon (LYH)
|*  Created			:	2015/05/06
|*  Description		:	Dahua protocol interpreter for china
|*				
****************************************************************************/

#ifdef USE_PROTOCOL_CHINA_DAHUA	// 중국 전용 다화 프로토콜 
// COMMON
#define RETURN_STAY			0
#define RETURN_NEXT_STEP	1
#define RETURN_END			2
typedef struct
{
	INT8U	state;
	INT8U	length;
} DAHUA_STATE;

// DHBZ PLU
#define DAHUA_DHBZ_PLUNUM_DIGIT	4
#define DAHUA_DHBZ_ITMECODE_DIGIT	6
#define DAHUA_DHBZ_UNPRICE_DIGIT	6
#define DAHUA_DHBZ_TYPE_DIGIT		1
#define DAHUA_DHBZ_VALIDDATE_DIGIT	3
#define DAHUA_DHBZ_BARCODE_DIGIT	2
#define DAHUA_DHBZ_TARE_DIGIT		5
#define DAHUA_DHBZ_SPECIAL_DIGIT	1
#define DAHUA_DHBZ_PLUNAME_SIZE 	18
#define DAHUA_DHBZ_INGREDIENT_SIZE	26
typedef struct
{
	INT32U	PLUnum;
	INT32U	itemcode;
	INT32U	unitprice;
	INT8U	unit;
	INT32U  validdate;		
	INT8U	barcodenum;
	INT32U	tarevalue;	
	INT8U	specialnum;
	INT8U	PLUname[DAHUA_DHBZ_PLUNAME_SIZE];	
	INT8U	ingredient[DAHUA_DHBZ_INGREDIENT_SIZE];
	INT8U	PLUname2[DAHUA_DHBZ_PLUNAME_SIZE];
	INT8U	PLUname3[DAHUA_DHBZ_PLUNAME_SIZE];	
} DAHUA_DHBZ_PLU;

// TMA07 PLU
#define DAHUA_TMA_PLUNUM_DIGIT	4
#define DAHUA_TMA_ITMECODE_DIGIT	7
#define DAHUA_TMA_UNPRICE_DIGIT	6
#define DAHUA_TMA_TYPE_DIGIT		1
#define DAHUA_TMA_INGREDIENT_NUMBER	3
#define DAHUA_TMA_INGREDIENT_DIGIT	2
#define DAHUA_TMA_VALIDDATE_DIGIT	3
#define DAHUA_TMA_SHOP_DIGIT		2
#define DAHUA_TMA_DEPARTMENT_DIGIT	2
#define DAHUA_TMA_BARCODE_SIZE		13
#define DAHUA_TMA_TARE_DIGIT		5
#define DAHUA_TMA_LABEL_DIGIT		2
#define DAHUA_TMA_DISCOUNT_NUMBER	4
#define DAHUA_TMA_DICOUNT_DIGIT		2
#define DAHUA_TMA_PLUNAME_NUMBER	3
#define DAHUA_TMA_PLUNAME_SIZE		24	
#define DAHUA_TMA_DISCOUNT_SIZE	4
typedef struct
{
	INT8U	start_hour;				// 할인 시작시간 (분단위 없음)
	INT8U	end_hour;				// 할인 종료시간 (분단위 없음)
	INT8U	rate;					// 할인율
} DAHUA_TMA_DISCOUNT;
typedef struct
{
	INT16U	PLUnum;
	INT32U	itemcode;
	INT32U	unitprice;
	INT8U	type;	
	INT8U	ingredientNo;	
	INT16U  validdate;
	INT8U	storeNo;
	INT8U	department;	
	INT8U	extbarcode[DAHUA_TMA_BARCODE_SIZE];
	INT32U	tarevalue;
	INT8U	labelNo;	
	DAHUA_TMA_DISCOUNT	discount[DAHUA_TMA_DISCOUNT_SIZE];
	INT8U	PLUname[DAHUA_TMA_PLUNAME_SIZE];	
	INT8U	PLUname2[DAHUA_TMA_PLUNAME_SIZE];
	INT8U	PLUname3[DAHUA_TMA_PLUNAME_SIZE];	
} DAHUA_TMA_PLU;

// extern variable
extern DAHUA_STATE dahuaprotocol;
// extern function
//extern INT8U CHN_dhbz_plu_interpreter(HUGEDATA COMM_BUF *CBuf, INT8U r_data, DAHUA_DHBZ_PLU *temp_dahua_dhbz_plu);
//extern INT8U CHN_tma_plu_interpreter(HUGEDATA COMM_BUF *CBuf, INT8U r_data, DAHUA_TMA_PLU *temp_dahua_tma_plu);
extern void CHN_dhbz_plu_save(DAHUA_DHBZ_PLU *dahua_plu);
extern void CHN_tma_plu_save(DAHUA_TMA_PLU *dahua_plu);
#endif // #ifdef USE_PROTOCOL_CHINA_DAHUA	// 중국 전용 다화 프로토콜 
