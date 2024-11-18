#ifndef _NUTRITION_H_1
#define _NUTRITION_H_1

//SG070105. Nutri_data[20] Index
#define	NUTRI_FACT_BASE_UNIT		0
#define	NUTRI_FACT_SERVING_SIZE		1
#define	NUTRI_FACT_BASE_SIZE		2

#define	NUTRI_FACT_CALORIES			3
#define	NUTRI_FACT_CALORIES_FAT		4
#define	NUTRI_FACT_TOTAL_FAT		5
#define	NUTRI_FACT_SATURATED_FAT	6
#define	NUTRI_FACT_TRANS_FAT		7
#define	NUTRI_FACT_CHOLESTEROL		8
#define	NUTRI_FACT_SODIUM			9
#define	NUTRI_FACT_TOTAL_CARBO		10
#define	NUTRI_FACT_DIETARY_FIBERS	11
#define	NUTRI_FACT_SUGARS			12
#define	NUTRI_FACT_PROTEIN			13
#define	NUTRI_FACT_VITAMIN_A		14
#define	NUTRI_FACT_CALCIUM			15
#define	NUTRI_FACT_VITAMIN_C		16
#define	NUTRI_FACT_IRON				17
#define	NUTRI_FACT_SALT				18
#define NUTRI_FACT_ENERGY			19
#ifdef USE_EU_NUTRITIONFACT
#define NUTRI_FACT_EU_ENERGY			3
#define NUTRI_FACT_EU_FAT				4
#define NUTRI_FACT_EU_SATURATES			5
#define	NUTRI_FACT_EU_MONOUNSATURATES	6
#define	NUTRI_FACT_EU_POLYUNSATURATES	7
#define NUTRI_FACT_EU_CARBOHYDRATE		8
#define NUTRI_FACT_EU_SUGARS			9
#define	NUTRI_FACT_EU_POLYOLS			10
#define	NUTRI_FACT_EU_STARCH			11
#define NUTRI_FACT_EU_FIBRE				12
#define NUTRI_FACT_EU_PROTEIN			13
#define NUTRI_FACT_EU_SALT				14
#define NUTRI_FACT_EU_VITAMIN_A			15
#define	NUTRI_FACT_EU_VITAMIN_D			16
#define	NUTRI_FACT_EU_VITAMIN_E			17
#define	NUTRI_FACT_EU_VITAMIN_K			18
#define NUTRI_FACT_EU_VITAMIN_c			19
#define	NUTRI_FACT_EU_THIAMIN			20
#define	NUTRI_FACT_EU_RIBOFLAVIN		21
#define	NUTRI_FACT_EU_NIACIN			22
#define	NUTRI_FACT_EU_VITAMIN_B6		23
#define	NUTRI_FACT_EU_FOLIC_ACID		24
#define	NUTRI_FACT_EU_VITAMIN_B12		25
#define	NUTRI_FACT_EU_BIOTIN			26
#define	NUTRI_FACT_EU_PANTHOTHENIC_ACID	27
#define	NUTRI_FACT_EU_POTASSIUM			28
#define	NUTRI_FACT_EU_CHLORIDE			29
#define NUTRI_FACT_EU_CALCIUM			30
#define	NUTRI_FACT_EU_PHOSPHORUS		31
#define	NUTRI_FACT_EU_MAGNESIUM			32
#define NUTRI_FACT_EU_IRON				33
#define	NUTRI_FACT_EU_ZINC				34
#define	NUTRI_FACT_EU_COPPER			35
#define	NUTRI_FACT_EU_MANGANESE			36
#define	NUTRI_FACT_EU_FLUORIDE			37
#define	NUTRI_FACT_EU_SELENIUM			38
#define	NUTRI_FACT_EU_CHROMIUM			39
#define	NUTRI_FACT_EU_MOLYBDENUM		40
#define	NUTRI_FACT_EU_IODINE			41
#endif // USE_EU_NUTRITIONFACT
#ifdef USE_ADDITIONAL_NUTRIENT
#define	NUTRI_FACT_ENERGY				19
#define	NUTRI_FACT_ADDED_SUGARS			20
#define	NUTRI_FACT_VITAMIN_D			21
#define	NUTRI_FACT_POTASSIUM			22
#define MAX_NUTRIFACT_ITEM				23		//NUTRIFACT 최대 ITEM 수
#else // USE_ADDITIONAL_NUTRIENT
#define MAX_NUTRIFACT_ITEM				20		//NUTRIFACT 최대 ITEM 수
#ifdef USE_EU_NUTRITIONFACT
#define MAX_NUTRIFACT_ITEM_EU			42
#endif // USE_ADDITIONAL_NUTRIENT
#endif // _NUTRITION_H_1

// nutri_type (0:normal, 1:2column(CHL), 2:total fat(CAN), 3:2column,6digit(AUS), 4:New Zealand: 2Column, 5:AUS(serving size for actual weight, 6:NZL(serving size for actual weight))
#define NUTRI_TYPE_NORMAL						0
#define NUTRI_TYPE_2COL_CHL						1
#define NUTRI_TYPE_TOTALFAT_CAN					2
#define NUTRI_TYPE_2COL_6DIG_AUS				3
#define NUTRI_TYPE_2COL_NZL						4
#define NUTRI_TYPE_2COL_6DIG_AUS_ACTUAL_WEIGHT	5
#define NUTRI_TYPE_2COL_NZL_ACTUAL_WEIGHT		6
#define NUTRI_TYPE_6DIG_CZE						7
#ifdef USE_EU_NUTRITIONFACT
#define NUTRI_TYPE_42ITEM_5DIG_EU_ACTUAL_WEIGHT 8
#else
#define NUTRI_TYPE_GBR							8
#endif
#ifdef USE_ADDITIONAL_NUTRIENT
#define NUTRI_TYPE_2COL_USA					 	9
#endif

#ifdef USE_DIRECT_NUTRITION
extern INT8U nutri_create_dept_no;
#endif

#pragma pack(2)
typedef	struct {
		INT16U	itemcode;
		INT8U	name[20];
		INT8U	unit[4];
//		INT16U	decpoint;
//		INT16U	input_min;
//		INT16U	input_max;
} NUTRIFACT_CAP_STRUCT;
	

////extern 	NUTRIFACT_CAP_STRUCT  nutrifact_cap_str[];
extern	INT16U Nutri_DV_refer_table[6];	
#ifdef USE_EU_NUTRITIONFACT
extern	INT32U Nutri_DV_refer_table_EU[39];
#endif
#ifdef USE_ADDITIONAL_NUTRIENT
extern	INT16U Nutri_DV_refer_table_USA[12];
#endif
typedef struct {	
		INT16U	code;
		INT16U	type;
		INT8U	description[20];
		INT8U	serving_size[20]; // text
		INT8U	serving_per[10]; //
		INT16U	nutri_data[20];	 // 40
} NUTRIFACT_STRUCT;		  

typedef struct {	
		INT16U	code;
		INT16U	type;
		INT8U	description[20];
		INT8U	serving_size[20]; // text
		INT8U	serving_per[10]; //
		INT32U	nutri_data[20];	 // 80
} NUTRIFACT_STRUCT2;

#ifdef USE_EU_NUTRITIONFACT
typedef struct {
		INT16U	code;
		INT16U	type;
		INT8U	description[20];
		INT8U	serving_size[20]; // text
		INT8U	serving_per[10]; //
		INT32U	nutri_data[45];	 // 180
} NUTRIFACT_STRUCT3;
#endif

#ifdef USE_ADDITIONAL_NUTRIENT
#pragma pack(1)
typedef struct {
  #ifdef USE_DIRECT_NUTRITION
	INT8U	dept_no;
	INT32U 	plu_num;		 
  #else
	INT16U	code;
	INT16U	type;		 
  #endif
	INT8U	description[20]; 
  #ifdef USE_DIRECT_NUTRITION
	INT8U	serving_size[30];		// text  50 -> 30 size 변경
	INT8U	serving_per[30];		// 10 -> 30 size 변경
  #else
	INT8U	serving_size[20]; // text 
	INT8U	serving_per[10]; //
  #endif
	INT32U	nutri_data[30];	 // 120, 기본 (23 + Addtional 5 + reserved 2 )
} NUTRIFACT_STRUCT4;
#endif
#pragma pack()
#endif
  
#ifndef _NUTRITION_H_2
#define _NUTRITION_H_2

extern 	void	nutri_create(void);		// command code 
extern  void    nutri_delete(void);		// command code 
extern  void 	nutri_list(void);		// command code 
extern  INT8U ReturnDecimalPointNutriTable(INT8U nutriType, INT8U index);
  #ifdef USE_DIRECT_NUTRITION
extern void callback_nutrifact_description(INT32U num);
extern int delete_plu_direct_nutrition(INT16U deptno, INT32U pluno);
extern int delete_all_plu_direct_nutrition(void);
extern int get_direct_nutrition(INT16U deptno, INT32U pluno, NUTRIFACT_STRUCT4 * ret_nutriStr4);
extern int insert_direct_nutrition(NUTRIFACT_STRUCT4 *nutriStr4);
  #endif
#endif 