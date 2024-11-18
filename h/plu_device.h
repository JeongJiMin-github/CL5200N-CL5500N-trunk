#ifndef PLU_GLOBAL_INFO_STRUCT_DEF
#define PLU_GLOBAL_INFO_STRUCT_DEF
#pragma pack(4)
typedef struct {
	INT16U ptype;
	INT16U ptype_point;
	INT16U usize;
	INT16U direct;
	INT8U  stype;
	INT8U  inhibit;
} PLU_GLOBAL_INFO_STRUCT;
#pragma pack()
#endif

#ifndef PLU_GLOBAL_TABLE_ADDR
#define PLU_GLOBAL_TABLE_ADDR
#pragma pack(4)
typedef struct {
	INT32U pluno;
	INT16U addr;
} PLU_ADDR;
#pragma pack()
#endif

#ifndef PLUSTR_POINT_1
#define PLUSTR_POINT_1

//#define PLUSTR_CAPTION_POINT	0
#define PLUSTR_PTYPE_POINT		0	//32
#define PLUSTR_VDEFAULT_POINT	2	//34
#define PLUSTR_MAXVALUE_POINT	6	//38
#define PLUSTR_STYPE_POINT		10	//42
#define PLUSTR_LENGTH_POINT		11	//43
#define PLUSTR_LINENO_POINT		12	//44
#define PLUSTR_DPOINT_POINT		13	//45
#define PLUSTR_USIZE_POINT		14	//46
#define PLUSTR_NAME_SIZE   		32
#define PLU_STRUCT_SIZE			16	//64
#endif

#ifndef PLUSTR_RETURN_2
#define PLUSTR_RETURN_2
#define PLUSTR_OK	    1
#define PLUSTR_UNUSE	2
#define PLUSTR_EOF	    3
#define PLUSTR_NOPLU	4
#define PLUSTR_ERROR    0
#endif

//SG071203.Test
#define PLU_NAME1_SIZE	40
#define PLU_NAME2_SIZE	40
#define PLU_NAME3_SIZE	5

// Ptype Define
#define PTYPE_DEPARTMENT_NO		1
#define PTYPE_PLU_NO			2
#define PTYPE_PLU_PREFIX		3
#define PTYPE_PLU_TYPE			4
#define PTYPE_UNIT_WEIGHT		5
#define PTYPE_PRICE				6
#define PTYPE_TAX_NO			8
#define PTYPE_GROUP_NO			9
#define PTYPE_PLU_NAME1			10
#define PTYPE_ITEM_CODE			11
#define PTYPE_TARE_NO			12
#define PTYPE_TARE_VALUE		13
#define PTYPE_PIECE				14
#define PTYPE_QTYUNIT_NO		15
#define PTYPE_SELLBY_DATE		16
#define PTYPE_SELLBY_TIME		17
#define PTYPE_PACKCED_DATE		18
#define PTYPE_PACKCED_TIME		19
#define PTYPE_PRODUCED_DATE		20
#define PTYPE_COOKBY_DATE		22
#define PTYPE_PERCENT_TARE_LIMIT	23
#define PTYPE_PERCENT_TARE		24
#define PTYPE_INGREDIENT_NO		25
#define PTYPE_FIXED_PRICE		26
#define PTYPE_PLU_NAME2			30
#define PTYPE_PLU_NAME3			31
#define PTYPE_TRACE_NO			35
#ifdef USE_TOPMART_STRUCTURE	//sewon 161118
#define PTYPE_MAX_QTY			36
#else
#define PTYPE_IMAGE_NO			36
#endif
#define PTYPE_PLU_NAME1_FONT	39
#define PTYPE_PLU_NAME2_FONT	40
#define PTYPE_PLU_NAME3_FONT	41
#define PTYPE_PLU_NAME4_FONT	42
#define PTYPE_PLU_NAME5_FONT	43
#define PTYPE_PLU_NAME6_FONT	44
#define PTYPE_PLU_NAME7_FONT	45
#define PTYPE_PLU_NAME8_FONT	46
#define PTYPE_EXT_BARCODE		49
#define PTYPE_BONUS				50
#ifdef USE_TOPMART_STRUCTURE //sewon 161118
#define PTYPE_TR_COMP_FLAG		51
#else
#define PTYPE_TRACE_FLAG		51 //KOR traceablility
#endif
#define PTYPE_TRACE_URL			52 //CHN Carrefour Traceablility URL
#define PTYPE_TRACE_CODE		53 //CHN Carrefour Traceablility Code
#define PTYPE_ORIGIN_NO			55
#define PTYPE_NO_OF_LINK_PLU	60
#define PTYPE_LINK1_DEPT_NO		61
#define PTYPE_LINK2_DEPT_NO		62
#define PTYPE_COUPLE_DEPT_NO	64
#define PTYPE_LINK1_PLU_NO		65
#define PTYPE_LINK2_PLU_NO		66
#define PTYPE_COUPLE_PLU_NO		68
#define PTYPE_REF_PLU_NO		69
#define PTYPE_NUTRITION_NO		70
#define PTYPE_REF_DEPT_NO		71
#define PTYPE_SELLBY_DATE_FLAG	76
#define PTYPE_SELLBY_TIME_FLAG	77
#define PTYPE_PACKCED_DATE_FLAG	78
#define PTYPE_PACKCED_TIME_FLAG	79
#define PTYPE_LABEL_NO			80
#ifdef USE_TOPMART_STRUCTURE//sewon 161118
#define PTYPE_TR_USE_NO			81
#else
#define PTYPE_AUX_LABEL_NO		81
#endif
#define PTYPE_BARCODE_NO		85
#ifdef USE_TOPMART_STRUCTURE //sewon 161118
#define PTYPE_KIND_NO			86
#else
#define PTYPE_AUX_BARCODE_NO	86
#endif
#define PTYPE_LABEL_MSG_NO		90
#ifdef USE_TOPMART_STRUCTURE //sewon 161118
#define PTYPE_TR_SELLBY_DATE	91
#else
#define PTYPE_SPECIAL_PRICE		91
#endif
#define PTYPE_PLU_NAME4			92
#define PTYPE_PLU_NAME5			93
#define PTYPE_PLU_NAME6			94
#define PTYPE_PLU_NAME7			95
#define PTYPE_PLU_NAME8			96
#define PTYPE_INGREDIENT_SIZE	98
#define PTYPE_INGREDIENT_TEXT	99
#define PTYPE_FIXED_WEIGHT		100
#ifdef USE_PERCENT_WEIGHT
#define PTYPE_RESERVED			101
#endif
#ifdef USE_SPAR_MEMBER_PRICE
#define PTYPE_MEMBER_PRICE		103
#endif
#if defined (USE_RUSSIA_LOGISTICS_PLU) || defined (USE_EUR_LOGISTICS_PLU)
#define PTYPE_GTIN_TYPE			121
#define PTYPE_GTIN_DATA			122
#define PTYPE_NETWEIGHT_DECIMAL	123		// 현재 저울 및 CL-Works Pro에서 사용하지 않음(24년 9월 기준)
#define PTYPE_CONTAINED_UNIT	124
#define PTYPE_LOT_NUMBER		125
#define PTYPE_SERIAL_NUMBER		126
#define PTYPE_RESERVED2			127
#define PTYPE_RESERVED3			128
#endif
#ifdef USE_DISCOUNT_MEMBERSHIP
#define PTYPE_DISCOUNT_RATE1	111
#define PTYPE_DISCOUNT_RATE2	112
#define PTYPE_DISCOUNT_START_DATE	113
#define PTYPE_DISCOUNT_END_DATE	114
#define PTYPE_DISCOUNT_TYPE		115
#endif
#define PTYPE_PREFIX			0x8003
#define PTYPE_ALL				0xff
// Ptype Define End

#ifdef USE_LOGISTICS_BARCODE
#define AI_GTIN_TYPE_01		01
#define AI_GTIN_TYPE_02		02
#define AI_CONTAINED_UNIT	37
#define AI_PRODUCTION_DATE	11
#define AI_EXPIRATION_DATE	17
#define AI_LOT_NUMBER		10
#define AI_SERIAL_NUMBER	21
#define AI_NET_WEIGHT_1		3101
#define AI_NET_WEIGHT_2		3102
#define AI_NET_WEIGHT_3		3103
#endif

#ifdef USE_DIRECT_INGREDIENT
#ifdef USE_DMSG_1050_BYTE
#define PLU_INF_SIZE    161
#define PLU_STORAGE_SIZE	(PLU_INF_SIZE-2*DIRECT_INGREDIENT_BLOCK_NUM) //except dmsgNo
#else //USE_DMSG_2100_BYTE
  #ifdef USE_RUSSIA_LOGISTICS_PLU
#define PLU_STORAGE_SIZE			(147) // For CL5000J PLU protocol compatible
#define LOGISTIC_PLU_STORAGE_SIZE	(MAX_PLU_RECORDSIZE_DEFAULT - (2 * DIRECT_INGREDIENT_BLOCK_NUM)) // For CL5000J PLU protocol compatible	
  #else
#define PLU_INF_SIZE    147+(2*DIRECT_INGREDIENT_BLOCK_NUM) // 175
#define PLU_STORAGE_SIZE	(PLU_INF_SIZE-2*DIRECT_INGREDIENT_BLOCK_NUM) //except dmsgNo
  #endif
#endif
#endif
#ifdef USE_DONT_SAVE_VALIDDATE_SALEDATE
typedef struct {
	INT32U date;
	INT32U pluno;
	INT8U deptno;
	INT8U time;
 	INT8U onoff;	
} TEMP_SELLDATE_STRUCT;
#endif
#pragma pack(2)
typedef struct _stCL5RUSPluData{
	INT16U  deptNo;			// Department Number		2	2
	INT32U 	pluno; 			// PLU Number				4	6
	INT8U	type;			// PLU Type					1	7
	INT8U	name1[PLU_NAME1_SIZE];	// PLU Name 1		40	47
	INT8U	name2[PLU_NAME2_SIZE];	// PLU Name 2		40	87
	INT8U 	name3[PLU_NAME3_SIZE];	// PLU Name 3		5	92
	INT16U	groupNo;		// Group Number				2	94
	INT16U	labelNo;		// Label Number				2	96
	INT16U  auxLabelNo;		// Aux Label Number			2	98
	INT16U	orgNo;			// Origin Number			2	100
	INT8U	unitW_no;		// Unit Weight Number		1	101
	INT32U  fixedWeight;	// Fixed Weight				4	105
	INT32U	icode;			// Item Code				4	109
	INT16U	pcs;			// Pieces					2	111
	INT8U	pcs_symNo;		// Q'ty Unit Number			1	112
	INT8U	fixedPrice;		// Use Fixed Price Type		1	113
	INT32U	unitPrice;		// Unit Price				4	117
#ifdef USE_PERCENT_WEIGHT
	INT8U	taxNo;			//N Tax Number              1	118
	INT16U	percentTare;	//percent					2	120
	INT8U	reserved[1];	//N reserved				1	121
#else
	INT32U	SpecialPrice;	// Special Price			4	121
#endif
	INT32U	tare;			// Tare						4	125
	INT8U   tareNo;			// Tare Number				1	126
	INT16U	barNo1;			// Barcode Number			2	128
	INT16U	barNo2;			// Barcode Number			2	130

	INT16U	pdDate;			// Produced Date			2	132  //현재에서 뺀다. 0-프린트 하지 않음, 1-오늘, 2-어제
	INT16U	pDate;			// Packed Date				2	134  //0-오늘, 1-내일
	INT8U	pTime;			// Packed Time				1	135  //0-현재, 1-1시간 뒤
	INT32U	sDate;			// Sell By Date				4	139  //0-프린트 하지 않음, 1-오늘, 2-내일
	INT8U	sTime;			// Sell By Time				1	140  //0-현재, 1-1시간 뒤

	INT16U	msgNo;			// Message Number			2	142
	INT16U	traceNo;		// traceability Number		2	144
	INT16U	nutriNo;		// nutrifact Number			2	146
	INT8U	smsgNo;			// Label Sales Message Number		1	147
#ifdef USE_RUSSIA_LOGISTICS_PLU
	INT8U	GTIN_type;		// GTIN type 						1	148
	INT8U	GTIN_str[18];	// GTIN string 					18	166	
	INT8U	Netweight_decimal_position; 	// Net.Weight Decimal Position	1	167		
	INT8U	Unit_contained[8];				// Qty in BOX		8	175
	INT8U	lot_number[20]; 				// Lot#			20	195
	INT8U	sereial_number[20]; 	 		// Serial#		20	215
	INT8U	reserved2[59]; 			 		// Reserved2	59	274
	INT8U	reserved3[70]; 			 		// Reserved3	70	344
#endif
#ifdef USE_DIRECT_INGREDIENT
	INT16U	dmsgNo[DIRECT_INGREDIENT_BLOCK_NUM];	// direct Message Number		14	161  //For Belarus 28 -175 // For Logistics 28	372
#else
	INT16U	dmsgNo;			// direct Message Number		2	149	// For Logistics 2	346
#endif
} PLU_INF;  
#pragma pack()

extern INT16S plu_record_size;
extern INT32U MAX_PLU_REPORT_NO;
extern INT8U plu_init(INT8U x);
extern void  plu_all_delete(HUGEDATA COMM_BUF *CBuf,INT8U mode);
extern INT16S   plu_move_temp(INT8U m,long key);
extern void  plu_restore_temp(INT8U m,long key);
extern void  plu_temp_clear(INT8U m);
extern void plu_temp_init(void);
extern void  plu_temp_set(INT8U m,INT32U pnt,char *data,INT16U xsize);
extern void  plu_temp_get(INT8U m,INT32U pnt,char *data,INT16U xsize);
extern INT32U plu_get_next_exist_address(INT16U nth);
extern INT8U plu_struct_check(void);
extern void  plu_struct_set_base(void);
extern INT8U plu_struct_get_version(void);
extern void plu_struct_set_version(INT8U ver);
extern INT16S   plu_struct_get_nfield(void);
extern void  plu_struct_set_nfield(INT16U w);
extern void  plu_set_update(INT16S id);
extern INT32U plu_calc_addr_by_key(long key);
extern INT16S  plu_get_ptype_point(INT16U cur_ptype,INT8U *stype,INT16U *xsize);
extern INT16S  plu_get_ptype_point3(INT16U cur_ptype,INT8U *stype,INT16U *xsize);
extern INT16U plu_struct_set(INT8U s_id,char *caption,INT16U ptype,INT8U stype,INT32U vdefault,INT32U maxvalue,INT8U length,INT8U lineno,INT8U dpoint,INT8U mode);
extern INT32U plu_table_prev_search(INT16S deptno,INT32U *pluno,INT32U min_id);
extern INT32U plu_table_seq_search(INT16S deptno,INT32U *pluno,INT32U min_id);
extern INT8U plu_table_search(unsigned char deptno,long pluno,INT16U *idx,INT8U mode);
extern INT8U plu_table_hdelete(unsigned char deptno);
extern INT8U plu_table_delete(unsigned char deptno, long pluno, INT16U *key);
extern INT16U plu_address_get_new(void);
extern INT8U plu_table_save(unsigned char deptno,long pluno);
extern void  plu_table_init(void);
extern INT8U plu_delete_department(unsigned char deptno);
extern INT8U  plu_delete(unsigned char deptno,long pluno,INT8U mode);
extern INT16S   plu_copy_move_device(INT16S from_dept, long from_plu, INT16S to_dept, long to_plu, INT8U flag);
extern INT8U plu_get_plu_type(INT16U dept_no, INT32U plu_no);
extern INT8U plu_check_inhibit_ptype(INT16U ptype);
extern INT32U plu_rem_number_get(void);
extern void   plu_rem_number_set(INT32U v);
extern INT8U plu_get_dept_pluno(INT32U address, INT16U *dept, INT32U *pluno);
extern INT16U ingredient_text_search_empty(INT16U index, INT16U nblock);
extern INT16U ingredient_text_count_empty(void);
extern INT8U ingredient_text_write(INT16U *ingred_no, HUGEDATA INT8U *dmsg, INT16U start_offset, INT16U size, INT16U nblock);	// nvram->flash
extern INT8U ingredient_text_read(INT16U *ingred_no, HUGEDATA INT8U *dmsg, INT16U start_offset, INT16U size);	// flash -> nvram
