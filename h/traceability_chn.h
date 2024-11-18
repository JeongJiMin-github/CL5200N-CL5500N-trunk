
#define TRACE_SIZE		30

#define VEGE_REMAIN_WEIGHT_SIZE	4
#define VEGE_TRACE_LOT_COUNT	32
//#define VEGE_TRACE_SIZE		30
#define VEGE_TRACE_PLU_COUNT	1000

#define MEAT_REMAIN_WEIGHT_SIZE	4
#define MEAT_TRACE_LOT_COUNT	4
//#define MEAT_TRACE_SIZE		30

#define BOOTH_NO_MAX    999

// trace code type
#define CHN_TRACE_CODE_TYPE_MEAT	0
#define CHN_TRACE_CODE_TYPE_VEGETABLE	1


#ifdef USE_CHN_CART_SCALE
#pragma pack(1)
typedef struct 
{
	INT8U dt[TRACE_DATETIME_SIZE];      //trace 입고시간
	INT8U code[TRACE_CODE_SIZE];        //trace 이력코드
	INT32S inputWeight;                 //trace 입고된 상품무게
} TRACE_STRUCT; //(저장용)total size = 30 byte

/*
typedef struct 
{
	INT8U dt[TRACE_DATETIME_SIZE];      //trace 입고시간
	INT8U code[TRACE_CODE_SIZE];        //trace 이력코드
	INT32S inputWeight;                 //trace 입고된 상품무게
} MEAT_TRACE_STRUCT; //(저장용)total size = 30 byte
*/
	
typedef struct
{
	TRACE_STRUCT unit[4];
} MEAT_COMM_TRACE_STRUCT;  //(통신용)total size = 120 byte

/*
typedef struct 
{
	INT8U code[TRACE_CODE_SIZE];    //이력코드(20 byte)
	INT32S inputWeight; //입고된 상품무게
	INT8U year;
	INT8U month;
	INT8U day;
	INT8U hours;
	INT8U minutes;
	INT8U seconds;
} VEGE_TRACE_STRUCT; //(저장용)total size = 30 byte
*/

typedef struct 
{
	INT16U commodityCode;           //it is smae PLU no
	INT8U name[MAX_ARTNAME];        //plu name
	INT8U dt[TRACE_DATETIME_SIZE];  //trace 입고시간
	INT8U code[TRACE_CODE_SIZE];    //trace 이력코드
	INT32S inputWeight;             //trace 입고된 상품무게
} VEGE_COMM_TRACE_STRUCT; //(통신용)total size = 62 byte

typedef struct{
	INT16U  commodityCode;
	INT8U   dt[TRACE_DATETIME_SIZE];
	INT32U  salePercent;
} VEGE_STOCK_INF;
#pragma pack()



#endif


extern TRACE_STRUCT CurCHNTrace;
extern INT32S CurCHNTraceRemainWeight;
extern INT8U CurCHNTraceLotNo;


extern void meat_trace_load(TRACE_STRUCT *buf, INT16U idx);
extern void meat_trace_save(TRACE_STRUCT* buf, INT16U idx);
extern void meat_trace_erase(INT16U idx);
extern INT32S meat_trace_remainWeight_load(INT16U idx);
extern void meat_trace_remainWeight_save(INT16U idx, INT32S weight);

extern void vege_trace_load(TRACE_STRUCT *buf, INT32U pluNo, INT8U lotRoom);
extern void vege_trace_save(TRACE_STRUCT* buf, INT32U pluNo, INT8U lotRoom);
extern void vege_trace_erase(INT32U pluNo, INT8U lotRoom);
extern INT32S vege_trace_remainWeight_load(INT32U pluNo, INT8U lotRoom);
extern void vege_trace_remainWeight_save(INT32U pluNo, INT8U lotRoom, INT32S weight);
extern void vege_trace_clear_stock(INT32U pluNo, INT8U *dt, INT32U percent);
extern void vege_trace_test(void);

extern void trace_init(void);
extern INT8U trace_compare_data(INT8U *srcBuf, INT8U *destBuf, INT16U length);
extern void trace_call_traceNo(void);
extern void trace_direct_call_traceNo(INT8U id);
extern void trace_remainWeightSummary(INT32U pluNo, INT8U lotNo, INT32S weight);
extern void trace_edit_remainWeight(INT32U pluNo, INT8U lotNo);
extern void transaction_log_delete(void);