#ifndef RUN_STATUS_DEF
#define RUN_STATUS_DEF
#pragma pack(4)
typedef struct {
	INT16U  run_level;
	INT16U	run_mode;
	INT16U  run_option;
} STATUS_RUN;
#pragma pack()
#endif

#ifndef SCALE_STATUS_DEF
#define SCALE_STATUS_DEF
#pragma pack(4)
typedef struct {
	INT16U address;
	INT16U deptid;
	INT32U PluCode;
	INT32U ItemCode;
	INT32U unitprice;
	//INT8U *ItemName;
	INT8U ItemName[60];
	INT32U tare;
	INT16U tare_limit;// % tare
	INT16U tare_percent;// % tare
	INT16U pcs;
	INT32U qty;
	INT32U weightunit;
	INT8U  weightunit_id;	// Added by CJK 20050930
	INT8U  unit_kglb;		// Added by CJK 20050930
	INT16U dc_id;
	INT16U group;
	INT8U  tax_id;
	INT8U  ptype;
#ifdef USE_TOPMART_STRUCTURE
	INT32U tr_sellby_date;
#else
	INT32U special_price;
#endif
	INT32U member_price;
	INT32U packing_price;
	INT32U fixed_weight;
	INT8U  fixed_price;
	INT8U  sales_msg;
	INT16U	origin;
	INT32U	sell_by_date;
	INT16U  prefix;
#ifdef USE_TOPMART_STRUCTURE
	INT16U tr_use_no;
	INT8U kind_no; //sewon 161130
	INT8U max_qty; //sewon 161130
	INT16U packedondate;
	INT16U packedontime;
#endif
#ifdef USE_PLU_MULTIPLE_PRICE
	INT32U first_target;	//refer_plu
	INT32U first_price;		//coupled_plu
	INT32U second_target;	//link1_plu
	INT32U second_price;	//link2_plu
#else	//Refefence PLU 기능 사용 안 함
	INT8U  coupled_dept;
	INT32U coupled_plu;
	INT8U  refer_dept;
	INT32U refer_plu;
#endif
#ifdef USE_CONTROL_CHANGING_PRICE
	INT8U ctrlPriceChange;
	INT8U upperLimit;
	INT8U lowerLimit;
#endif
#ifdef USE_TRACE_STANDALONE
  #ifdef USE_TOPMART_STRUCTURE
	INT8U	tr_comp_flag;
  #else
	INT8U	trace_flag;
  #endif
#endif
#ifdef USE_FIXED_PLUTYPE//USE_EMART_NETWORK
	INT8U  disable_typechange;	// plu type 변경 불가 flag
#endif
//	INT32U dc_firstprice;
//	INT32U dc_secondprice;
} PLU_BASE;

#define	MAX_LINKED_PLU	2
typedef struct {
	INT16U  scaleid;
	INT16U  departmentid;
	INT16U	clerkid;
	INT8U   level;
	INT32U  plu_address;
	INT8U   use_pcs;
	INT8U   use_bycount;
	INT16U  restrict;
	INT16U  scalemode;
	INT8U	flag_salemode;
	long    UnitPrice;
	long    dc_unitprice;
	long    Weight;
	long    Tare;
	long    dc_weight;
	long    TotalPrice;
	long    dc_totalprice;
	INT16U 	ErrKind;
	INT8U   ErrAction;
	long    ErrValue;

	INT16S     print_multiple;
	INT8U   print_status;
	INT8U   print_out;

//	INT8U   allow_mode;
	INT8U	flag_display;
	INT8U   flag_input;

	INT16U  cur_time_key;
//DEL CJK070319	INT16U  cur_time_err;

	INT16U  cur_speedkeyid;
	INT32U  cur_pluid;
	INT32U  cur_keyvalue;
	INT32U  cur_weightunit;
	INT32U	cur_quickplu;	// CJK070130
	INT16U  cur_displayon;
	INT8U   cur_ride;
	INT32U  cur_qty;
	INT32U  cur_unitprice;
	INT32U  cur_pcs;
	INT32U  cur_ptare;
	INT16U  cur_labelid;
	INT8U   ride_max;

	INT8U  auto_count;
	//INT8U  auto_maxcount;
	long   weight_min;
	long   weight_delta;
	long   weight_stable;
	long   weight_print;

	INT8U  display_dummydigit;

//DEL070130	INT8U  aux_input_type;
//DEL070130	INT8U  sale_type;
//DEL070130	INT32U sale_value;
	INT8U  scale_lock;
	INT32U tare;
#if defined(USE_GSMART_BARCODE) || defined(USE_PRT_CHNIA_DISCOUNT_RATE)
	INT8U	discountflag;
	INT8U	discount_rate;
#endif
#ifdef USE_BESTCO_DISCOUNT_DIVISION
	INT8U	percent_discount_flag;
#endif
    INT16U key_last_used_100ms; 	// key가 눌러질 때 업데이트
	INT16U ad_last_used_100ms;		// 무게가 변동될 때 업데이트
	INT8U divisor;	//1/2, 1/3, 1/4
	INT16U  code00;
	PLU_BASE Plu;
	INT8U  linked_count;// 0 ~ MAX_LINKED_PLU
	INT8U  linked_dept[MAX_LINKED_PLU];
	INT32U linked_plu[MAX_LINKED_PLU];
	PLU_BASE LinkedPlu[MAX_LINKED_PLU];
} STATUS_SCALE;
#pragma pack()
#endif

#ifndef MODE_SALE_PROC_1
#define MODE_SALE_PROC_1

#define		SALE_PROC_START			0
#define		SALE_PROC_KEY			1
#define		SALE_PROC_ADM			2
#define		SALE_PROC_CALC_PRICE		3
#define		SALE_PROC_ETH_RCV		4
#define		SALE_PROC_DISPLAY		5
#endif

#ifndef MODE_VARIABLE_DEF_1
#define MODE_VARIABLE_DEF_1

#define RUN_SALE		0x0001
#define RUN_CALIBRATION 0x0002
#define RUN_PROGRAM		0x0004
#define RUN_DEBUG		0x0008
#define RUN_ERROR		0x0010
#define RUN_BACKUP		0x0020
#define RUN_FJIG		0x0040
#define RUN_FJIG_EXIT	0x0080
#define RUN_RDY_CAP		0x0100
#define RUN_RESTART		0x0200
#define RUN_WAITING_CAL 	0x0400
#define RUN_FW_UPDATE	0x0800

#define SCALE_TYPE_BENCH   1
#define SCALE_TYPE_POLE    2
#define SCALE_TYPE_HANGING 3
#define SCALE_TYPE_SELF    4
#define SCALE_TYPE_DOUBLEBODY      5
#define SCALE_TYPE_BENCH_CL5200    6
#define SCALE_TYPE_BENCH_CL3000    7
#define SCALE_TYPE_HANGING_CL5200  8
#define SCALE_TYPE_END     SCALE_TYPE_HANGING_CL5200


//extern STATUS_RUN   status_run;
//extern STATUS_SCALE status_scale;
//HYP 20060412 _huge --> _near for small code size
extern NEARDATA STATUS_RUN   status_run;
extern NEARDATA STATUS_SCALE status_scale;
// extern HUGEDATA PLU_STR	Plu[55];
#if defined(USE_REMOTE_FW_DOWNLOAD) || defined(USE_REMOTE_ROM_UPDATE)
extern int UpdateFwPercnet;
extern INT16U UpdateFwTimer;
#endif
#endif

#ifndef MODE_FUNCTION_DEF
#define MODE_FUNCTION_DEF
extern void mode_main_init(void);
extern void mode_main(void);

//extern INT8U plu_write(INT32U code, PLU_STR *plu);
//extern INT8U plu_read(INT32U code, PLU_STR *plu);
extern void CalcPrice(void);

#endif
