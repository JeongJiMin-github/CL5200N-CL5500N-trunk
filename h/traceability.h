#pragma pack(2)
#ifndef _TRACEABILITY_H_1
#define _TRACEABILITY_H_1
/*	// Deleted by CJK 20060412
typedef struct {
		INT16U	code;
		INT8U	contents[22];
		INT16U	idata[4];
}TRACEABILITY_STRUCT;
*/
typedef struct {
		INT16U	code;
		INT8U	contents[18];
		INT16U	idata[4];
}TRACEABILITY_STRUCT;

#endif


#ifndef _TRACEABILITY_H_2
#define _TRACEABILITY_H_2

extern 	void	traceability_create(void);			// command code  0x1351
extern  void    traceability_delete(void);			// command code  0x1352
extern  void 	traceability_list(void);				// command code  0x1353

#endif 


/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
//// 저울나라 축산 이력 
#ifdef USE_KOR_TRACE
#define USE_KORTRACE_PROTOCOL_V2

#ifndef KORTRACE_DATA_TYPE_DEFINE
#define KORTRACE_DATA_TYPE_DEFINE
typedef struct {
	INT16U	cmd;
	INT16U	ret_cmd;
}KORTRACE_RECEIVE_ACK_TYPE;

typedef struct {
	INT16U	cmd;
	INT16U	ret_cmd;
	INT16U	err_code;
}KORTRACE_RECEIVE_NAK_TYPE;

#ifdef USE_KORTRACE_PROTOCOL_V2

//개체정보요청 (scale -> pc)
typedef struct
{
	INT32U	commodity_no;			// plu_no
	INT32U	weight;
	INT32U	unitprice;
	INT32U	totalprice;
	INT8U	traceid_barcode[46];   //SG060331
}KORTRACE_SEND_DATA_TYPE;

//응답 (PC-> Scale), 인쇄  
#ifdef USE_HANAPOS_PROTOCOL
typedef struct {
	INT16U	cmd;
	INT16U	ret_cmd;
	INT8U	traceid_barcode[46];   //SG060331
	INT8U	traceid_printdata[5][64];
	INT8U   traceid_barcode2[30];
	INT16U  sellbydate;
}KORTRACE_RECEIVE_DATA_TYPE;
#else
typedef struct {
	INT16U	cmd;
	INT16U	ret_cmd;
	INT8U	traceid_barcode[46];   //SG060331
	INT8U	traceid_printdata[5][64];
	INT16U  sellbydate;
#ifdef USE_KORTRACE_PROTOCOL_ADD_LABEL
	INT8U	labelNo;
#endif
}KORTRACE_RECEIVE_DATA_TYPE;
#endif

//라벨 인쇄 명령 : Protocol 3
typedef struct {
	INT16U	cmd;
	INT32U	commodity_no;			// plu_no
	INT32U	weight;
	INT32U	unitprice;
	INT32U	totalprice;
	INT8U	traceid_barcode[46];   //SG060331
	INT8U	traceid_printdata[5][64];
	INT8U   traceid_barcode2[30];
	INT16U  sellbydate;
#ifdef USE_KORTRACE_PROTOCOL_ADD_LABEL
	INT8U	labelNo;
#endif
}KORTRACE_RECEIVE_CMD_TYPE;

#ifdef USE_HANAPOS_PROTOCOL
typedef struct{
	INT8U flagReceive;
	INT8U flagPrint;
	INT8U flagSendWeight;			   
	INT8U err_cnt;
	INT8U CurBarData[46]; //SG060331	프로토콜에서 수신한 데이타
	INT8U barcode_data[40]; //SG060515	바코드를 찍기 위한 데이타
	INT8U display_data[24]; //SG060331  화면에 보여질 개체정보
	INT8U flagDrawBarcode;  //SG060403  Barcode2를 그릴것인지를 결정 
	INT8U flagCommSuccess;  //SG060403 
	INT16U WaitTime;
	INT8U bufferManualMode[18];
	INT8U CurBarData2[46]; //SG060331	프로토콜에서 수신한 데이타
	INT8U barcode_data2[40]; //SG060515	바코드를 찍기 위한 데이타
	INT8U memberCode[15];   //SG070201  Member Code 	
}KORTRACE_FLAG;

#else
typedef struct
{
	INT8U flagReceive;
	INT8U flagPrint;
	INT8U flagSendWeight;			   
	INT8U err_cnt;
	INT8U CurBarData[46]; //SG060331	프로토콜에서 수신한 데이타
	INT8U barcode_data[40]; //SG060515	바코드를 찍기 위한 데이타
	INT8U display_data[24]; //SG060331  화면에 보여질 개체정보
	INT8U flagDrawBarcode;  //SG060403  Barcode2를 그릴것인지를 결정 
	INT8U flagCommSuccess;  //SG060403 
	INT16U WaitTime;
	INT8U bufferManualMode[18];
	INT8U memberCode[15];   //SG070201  Member Code 	
#ifdef USE_KORTRACE_PROTOCOL_ADD_LABEL
	INT8U labelNo;
#endif
}KORTRACE_FLAG;
#endif

#else

//개체정보요청 (scale -> pc)
typedef struct
{
	INT32U	commodity_no;			// plu_no
	INT32U	weight;
	INT32U	unitprice;
	INT32U	totalprice;
	INT8U	traceid_barcode[30];   //SG060331
}KORTRACE_SEND_DATA_TYPE;

//응답 (PC-> Scale), 인쇄  
typedef struct {
	INT16U	cmd;
	INT16U	ret_cmd;
	INT8U	traceid_barcode[30];   //SG060331
	INT8U	traceid_printdata[5][64];
	INT16U  sellbydate;
}KORTRACE_RECEIVE_DATA_TYPE;

//라벨 인쇄 명령 : Protocol 3
typedef struct {
	INT16U	cmd;
	INT32U	commodity_no;			// plu_no
	INT32U	weight;
	INT32U	unitprice;
	INT32U	totalprice;
	INT8U	traceid_barcode[30];   //SG060331
	INT8U	traceid_printdata[5][64];
	INT8U   traceid_barcode2[30];
	INT16U  sellbydate;
}KORTRACE_RECEIVE_CMD_TYPE;

typedef struct
{
	INT8U flagReceive;
	INT8U flagPrint;
	INT8U flagSendWeight;			   
//	INT8U flagBarInfo;    //개체 바코드 정보가 있는지 여부
	INT8U err_cnt;
	INT8U CurBarData[30]; //SG060331	프로토콜에서 수신한 데이타
//	INT8U barcode_data[20]; //SG060331	바코드를 찍기 위한 데이타
	INT8U barcode_data[30]; //SG060515	바코드를 찍기 위한 데이타
	INT8U display_data[24]; //SG060331  화면에 보여질 개체정보
	INT8U flagDrawBarcode;  //SG060403  Barcode2를 그릴것인지를 결정 
	INT8U flagCommSuccess;  //SG060403 
	INT16U WaitTime;
	INT8U bufferManualMode[18];
	INT8U memberCode[15];   //SG070201  Member Code 	
}KORTRACE_FLAG;

#endif

#endif

#ifdef USE_KORTRACE_PROTOCOL_V2
  #define KORTR_REQ_PRINT_ID	0x0401	// 라벨인쇄요청(SCALE->PC)
  #define KORTR_RCV_PRINT_ID	0x0402	// 라벨인쇄명령(PC->SCALE)
  #define KORTR_SEND_WEIGHT		0x0403	// 중량전송(SCALE->PC)
  #define KORTR_REQ_FIRST_ID	0x0404	// 현재개체요청(SCALE->PC)
  #define KORTR_REQ_NEXT_ID		0x0405	// 다음개체요청(SCALE->PC)
  #define KORTR_REQ_NONE_ID		0x0406
  #define KORTR_REQ_FIXED_ID	0x0407	// 개체지정요청(SCALE->PC)
  #define KORTR_REQ_COMPLETE_ID	0x0408	// 개체완료요청(SCALE->PC)
  #define KORTR_RCV_CLEAR_PLU	0x0409	// PLU 취소(PC->SCALE)
  #define KORTR_SEND_UNITPRICE	0x040A	// Unit price 전송 명령(PC-->Scale)
  #define KORTR_REQ_RETURN_ID	0x040B	// 반품(SCALE->PC)
  #define KORTR_REQ_FPPRINT_ID	0x040C	// 정액발행(SCALE->PC)
  #define KORTR_RCV_PRINT2_ID	0x040D	// 라벨인쇄명령2(개체바코드포함)(PC->SCALE)
  #define KORTR_RCV_ACK	0x0301
  #define KORTR_RCV_NOPRT_ACK	0x0302	// 외부 PRT 사용시
  #define KORTR_RCV_NAK	0x0300
#ifdef USE_HANAPOS_PROTOCOL
  #define KORTR_REQ_SCALE_SALE_ID		0x0501  // 저울에서 판매.SaleData는 저울 data(SCALE-> PC)
#endif
#else
  #define KORTR_REQ_PRINT_ID	0x0201	// 라벨인쇄요청(SCALE->PC)
  #define KORTR_RCV_PRINT_ID	0x0202	// 라벨인쇄명령(PC->SCALE)
  #define KORTR_SEND_WEIGHT		0x0203	// 중량전송(SCALE->PC)
  #define KORTR_REQ_FIRST_ID	0x0204	// 현재개체요청(SCALE->PC)
  #define KORTR_REQ_NEXT_ID		0x0205	// 다음개체요청(SCALE->PC)
  #define KORTR_REQ_NONE_ID		0x0206
  #define KORTR_REQ_FIXED_ID	0x0207	// 개체지정요청(SCALE->PC)
  #define KORTR_REQ_COMPLETE_ID	0x0208	// 개체완료요청(SCALE->PC)
  #define KORTR_RCV_CLEAR_PLU	0x0209	// PLU 취소(PC->SCALE)
  #define KORTR_SEND_UNITPRICE	0x020A	// Unit price 전송 명령(PC-->Scale)
  #define KORTR_REQ_RETURN_ID  	0x020B	// 반품(SCALE->PC)
  #define KORTR_REQ_FPPRINT_ID	0x020C	// 정액발행(SCALE->PC)
  #define KORTR_RCV_PRINT2_ID  	0x020D	// 라벨인쇄명령2(개체바코드포함)(PC->SCALE)
  #define KORTR_RCV_ACK	0x0101
  #define KORTR_RCV_NAK	0x0100
#endif

#define KORTR_PROCESS_NO_LEN 4

extern INT16U kortrace_byte2toword(INT8U *str);
extern void kortrace_init(void);
extern void korTraceMemberCodeInit(void);
extern INT8U kortrace_com_receive( COMM_BUF *CBuf, INT8U *buffer);
extern INT8U kortrace_server_receive( COMM_BUF *CBuf, INT8U *buffer, INT16S length);
extern INT8U kortrace_com_parse_data1(INT8U *str);
extern INT8U kortrace_com_parse_data2(INT8U *str);
extern INT8U kortrace_com_parse_data3(INT8U *str); //Weight Ack data
extern INT8U kortrace_com_parse_common(INT8U *str,INT16U command); 
extern void kortrace_make_display_data(INT8U *disp_str, INT8U *bar_str, INT8U disp_len);
extern void kortrace_com_send_ack( COMM_BUF *CBuf, INT16U ret_cmd, INT16U ack, INT16U err_code);
#ifdef USE_HANAPOS_PROTOCOL
extern void kortrace_com_send_ack_withData( COMM_BUF *CBuf, INT16U ret_cmd, INT16U ack, INT16U err_code);
#endif
extern void kortrace_com_send_data( COMM_BUF *CBuf, INT16U cmd);
extern void kortrace_com_req_command( COMM_BUF *CBuf, INT16U command);
extern void KorTraceCheckMemberCode(void);
extern void KorTraceSendWeight(void);


extern KORTRACE_SEND_DATA_TYPE		KorTrace_SendData;
extern KORTRACE_RECEIVE_DATA_TYPE	KorTrace_ReceiveData;
extern KORTRACE_RECEIVE_CMD_TYPE	KorTrace_ReceiveCmd;
extern KORTRACE_RECEIVE_ACK_TYPE	KorTrace_ReceiveACK;
extern KORTRACE_RECEIVE_NAK_TYPE	KorTrace_ReceiveNAK;
extern KORTRACE_FLAG				KorTrace_Flag;

extern void sale_display_kortrace(INT8U dispFlag);	// Added by CJK 20060310

#endif
/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////


//SG060718
#ifdef USE_TRACE_STANDALONE

//#define USE_MULTI_TRACE_NO_DBG

#ifdef USE_KOR_TRACE_999
  #define TRACE_INDIVIDUAL_IDENTITY_SIZE	210
  #define MAX_INDIVIDUAL_NO	   		1000
#elif defined(USE_TRACE_1000QTY)
  #define TRACE_INDIVIDUAL_IDENTITY_SIZE	  200
  #define MAX_INDIVIDUAL_NO		  1000
#else
  #define TRACE_INDIVIDUAL_IDENTITY_SIZE	200	  	//gm 090405, 64 -> 200
  #define MAX_INDIVIDUAL_NO	   		100			//gm 090406, 98->99, 100은 바코드 스캐너 전용(or 개체입력키)
#endif
 #define MAX_INDIVIDUAL_KEY_NO		16
#if defined(USE_IMPORT_LOTNO) || defined(USE_TRACE_1000QTY)//sewon 161223
#define INDIVIDUAL_NO_LEN_TRANS		24	// 실시간업로드 Buffer용 길이
#else
#define INDIVIDUAL_NO_LEN_TRANS		20	// 실시간업로드 Buffer용 길이
#endif
#if defined(USE_IMPORT_LOTNO) || defined(USE_TRACE_1000QTY)//sewon 161223
  #define INDIVIDUAL_NO_LEN			24  // 일반용 개체
#elif defined(USE_MULTI_TRACE_NO_TABLE)
  #define INDIVIDUAL_NO_LEN			21	// 일반용 개체
  //#define INDIVIDUAL_NO_LEN_IN_FLASH	21	// 개체데이터 저장용 길이(M1371)
  //#define INDIVIDUAL_NO_LEN_IN_NVRAM	21	// 개체 기억 기능용 길이
#else
  #define INDIVIDUAL_NO_LEN			20	// 일반용 개체
  //#define INDIVIDUAL_NO_LEN_IN_FLASH	20	// 개체데이터 저장용 길이(M1371)
  //#define INDIVIDUAL_NO_LEN_IN_NVRAM	20	// 개체 기억 기능용 길이
#endif

#define INDIV_GRADE_LEN	8	// CJK070409

#define INDIV_DISP_MODE_INDIV_NO		0x01
#define INDIV_DISP_MODE_SPECIALUSE		0x02
#define INDIV_DISP_MODE_SLAUGHTHOUSE	0x04
#define INDIV_DISP_MODE_MEATPART		0x08
#define INDIV_DISP_MODE_MEATGRADE		0x10
#define INDIV_DISP_MODE_ALL		(INDIV_DISP_MODE_INDIV_NO|INDIV_DISP_MODE_SPECIALUSE|INDIV_DISP_MODE_SLAUGHTHOUSE|INDIV_DISP_MODE_MEATPART|INDIV_DISP_MODE_MEATGRADE)		
/*
typedef struct 
{
    INT8U   index;		   //인덱스
    INT8U   individualNO[20];  	   //개체번호
    INT8U   grade[4];		   //등급
    INT8U   slaughtDate[3];        //도축일자 [0]=year, [1]=month, [2]=day
    long    weight;   		   //중량
    INT8U   breed[10];		   //품종
    INT8U   farm[20];		   //사육지
    INT8U   reserved[2];
} TRACE_INDIVIDUAL_IDENTITY;       
*/

#ifdef USE_KOR_TRACE_999	// 농협
typedef struct
{
	INT16U	index;			//인덱스
	INT8U   lotFlag;
	INT8U	individualNO[20 + 1];  	   //개체번호
	INT8U   farm[49];			//사육지, LOT일 때는 묶음부위
	INT8U   breed[25];			//품종
	INT8U	grade[10];			//등급(string), gradeNo추가를 위해 기존 11->10로 줄임.
	INT8U	gradeNo;			//등급 번호(등급표시 기능으로 추가됨)
	INT8U   slaughtHouse[49];	//도축장 49byte, LOT일 때는 묶음도축장 101byte, 멤버 size가 변하므로 뒤에 멤버를 넣을 때 주의하시오. 
	INT8U   slaughtDate[3];		//도축일자 [0]=year, [1]=month, [2]=day
	INT8U	reserved[49];
} TRACE_INDIVIDUAL_IDENTITY;	// 210byte

#elif defined(USE_IMPORT_LOTNO)
typedef struct 
{
	INT8U   index;		   //인덱스
	INT8U   individualNO[24+1];  	   //개체번호
	long    weight;   		   //중량
	INT8U   slaughtDate[3];        //도축일자 [0]=year, [1]=month, [2]=day
	INT8U   breed[25];		   //품종
	INT8U   farm[49];		   //사육지
	INT8U   slaughtHouse[49];
	INT8U   lotFlag;
	INT8U	grade[11];			//등급(string)
#ifdef USE_CHECKING_HARMFUL_MEAT
	INT8U	harmfulness;		// 위해 플래그
	INT8U	gradeNo;
	INT8U   reserved2[31];
#else
	INT8U	gradeNo;
	INT8U   reserved2[31];
#endif
} TRACE_INDIVIDUAL_IDENTITY; 

#elif defined(USE_TRACE_1000QTY)  //sewon 161208
typedef struct 
{
	INT16U	index;		   //인덱스
	INT8U	individualNO[24]; 	   //개체번호
	long	weight; 		   //중량
	INT8U	slaughtDate[3]; 	   //도축일자 [0]=year, [1]=month, [2]=day
	INT8U	breed[25];		   //품종
	INT8U	farm[49];		   //사육지
	INT8U	slaughtHouse[49];
	INT8U	lotFlag;
	INT8U	grade[11];			//등급(string)
	INT8U	gradeNo;
	INT8U	meatKind;			//육종
	INT8U	harmfulness;
	INT8U	nongjangNo[15];
	INT8U	slaughtNo;
	INT8U	reserved[13];
} TRACE_INDIVIDUAL_IDENTITY;  

#else
typedef struct 
{
	INT8U   index;		   //인덱스
	INT8U   individualNO[21+1];  	   //개체번호
//#if (INDIVIDUAL_NO_LEN_IN_FLASH == 20)	//size 20이면 Flash 구조 유지, 21 이면 gradeNo 제거
//	INT8U   gradeNo;		   //등급(index). 현재 미사용
//#endif
	INT8U   reserved1;
	long    weight;   		   //중량
	INT8U   slaughtDate[3];        //도축일자 [0]=year, [1]=month, [2]=day
	INT8U   breed[25];		   //품종
	INT8U   farm[49];		   //사육지
	INT8U   slaughtHouse[49];
	INT8U   lotFlag;
	INT8U	grade[11];			//등급(string)
#ifdef USE_CHECKING_HARMFUL_MEAT
	INT8U	harmfulness;		// 위해 플래그
	INT8U	gradeNo;
	INT8U   reserved2[32];
#else
	INT8U	gradeNo;
	INT8U   reserved2[33];
#endif
} TRACE_INDIVIDUAL_IDENTITY;     
#endif

typedef struct 
{
	INT16U  index;		   		//인덱스
	INT8U   meatUse[49];  	//용도 or 특수부위
	INT8U	reserved;
} TRACE_SPECIALUSE_STRUCT;

#ifdef USE_SLAUGHTERHOUSE_LENG_100
#define TRACE_SLAUGHTHOUSE_NAME_SIZE 101 // 100 CHART + 1 NULL
#else
#define TRACE_SLAUGHTHOUSE_NAME_SIZE 49
#endif

typedef struct 
{
	INT16U  index;		   		//인덱스
	INT8U   slaughtHouse[TRACE_SLAUGHTHOUSE_NAME_SIZE];  	//도축장
	INT8U	reserved;
} TRACE_SLAUGHTHOUSE_STRUCT;

#ifdef USE_TRACE_MEATPART
typedef struct 
{
	INT16U  index;		   		//인덱스
	INT8U   meatPart[49];  	//부위
	INT8U	reserved;
} TRACE_MEATPART_STRUCT;        
#endif

#ifdef USE_TRACE_MEATGRADE
typedef struct 
{
	INT16U  index;		   		//인덱스
	INT8U   meatGrade[28];  	//등급
	INT8U	order;
	INT8U	reserved;
} TRACE_MEATGRADE_STRUCT;        

#define TRACE_MEATGRADE_ORDER_OFFSET	(2+28)
#endif

typedef struct
{
    TRACE_INDIVIDUAL_IDENTITY  indivStr;
    INT8U flagBarcodePrint;
    //INT8U curGradeText[4]; 
	INT8U curGradeText[INDIV_GRADE_LEN]; //CJK070409
	INT8U slPlace;	// 도축장 번호
#ifdef USE_LOTTEMART_MEATSCANNER
	INT8U indivno[INDIVIDUAL_NO_LEN+1]; //이력번호 24자리+1
	INT8U meatUseNo;	// 용도 번호
	INT8U slPlace1; // 도축장 번호1
	INT8U slPlace2; // 도축장 번호2
	INT8U slPlace3; // 도축장 번호3
	INT8U meatgradeNo;	// 등급번호
	INT8U meatscannerflag; //축산 스캐너 데이터 유무
	INT8U no14indivflag; //14자리 보관라벨용 스캔 유무
#endif
	INT8U meatUse;	// 용도 번호
//#ifdef USE_TRACE_MEATPART
	INT8U meatPart;	// 부위 번호
//#endif    
	long  indivWeight;	// 재고중량
	INT8U gradeNo;	// 등급번호
} TRACE_STATUS_STRUCT;
					   
#ifdef USE_MULTI_TRACE_NO_TABLE

typedef struct
{
	INT8U  useFlag;
	INT8U  indivNo[3][INDIVIDUAL_NO_LEN];
	INT8U  isLot;			// lot(묶음)유무
	INT8U  slaughter_place[3];	// 도축장			//65(TNT_NODE_SLAUGHT_OFFSET)
	INT8U  meat_use;		// 용도
//#if (INDIVIDUAL_NO_LEN == 21)
	INT8U  meat_part;
	INT8U  meat_grade;		// 등급
	INT8U  reserved;
//#endif
} PLU_INDIVIDUAL_NODE_DATA;	// 개체번호 기억 기능 구조(in NVRAM) - 이마트

#define TNT_NODE_SLAUGHT_OFFSET	(1+3*INDIVIDUAL_NO_LEN+1)//65

#define TNT_PLU_INDIVIDUAL_DATA_SIZE	sizeof(PLU_INDIVIDUAL_NODE_DATA)

#define TNT_TRACENO_TABLE_SIZE (2*3001L)

#define TNT_TRACENO_TABLE_NO	((72000-TNT_TRACENO_TABLE_SIZE)/TNT_PLU_INDIVIDUAL_DATA_SIZE)	//(72000-(3001*2))/72byte = 916

#else	//#ifdef USE_MULTI_TRACE_NO_TABLE
					   
typedef struct
{
	INT8U  indivNo[INDIVIDUAL_NO_LEN];
	INT8U  isLot;			// lot(묶음)유무, *등급 기억을 위해 마지막bit만 isLot 사용, 상위 7bit는 grade를 저장한다. 
	INT8U  slaughter_place;	// 도축장
	INT8U  meat_use;		// 용도
	INT8U  meat_part;		// 부위
} PLU_INDIVIDUAL_DATA;	// 개체번호 기억 기능 구조(in NVRAM)

#define TNT_PLU_INDIVIDUAL_DATA_SIZE	sizeof(PLU_INDIVIDUAL_DATA)

#define TNT_TRACENO_TABLE_SIZE (2*3001L)

#define TNT_TRACENO_TABLE_NO	((72000-TNT_TRACENO_TABLE_SIZE)/TNT_PLU_INDIVIDUAL_DATA_SIZE)	//(72000-(3001*2))/24byte = 2749

#endif

/*
<개체기억타입>
   0 : 기억안함(개체 유지)
   1 : 개체번호 기억
   2 : 개체번호, 용도 기억
   3 : 개체번호, 도축장 기억
   4 : 개체번호, 도축장, 용도, 부위, 등급 기억(ALL)
   5 : 개체번호, 도축장, 용도, 부위 기억
   6 : 기억안함(E 마트용 4번 타입에서 개체 유지 안함)
*/
typedef struct 
{
	INT8U	indivNoStr[INDIVIDUAL_NO_LEN];
	INT8U	slaughterHouse;
} TRACE_INDIVIDUAL_MULTI_DATA;

typedef struct 
{
	INT8U	indivNoStr[INDIVIDUAL_NO_LEN];
	INT16U	deptNo;
	INT32U	pluNo;
	INT8U	slaughterNo;
} TRACE_LOT_MULTI_DATA;

#ifdef USE_KOR_SLAUGHT_CODE_4DIGIT
typedef struct
{
	INT16U no;
	INT16U index;
} TABLE_INDEX_STRUCT;
#endif
#ifdef USE_TOPMART_STRUCTURE
typedef struct {
	INT8U	index;
	INT8U	contents[30];
} TRACE_BREED_TYPE;
#endif
#define MAX_INDIVIAL_MULTI_DATA_CNT	3
#define MAX_LOT_MULTI_DATA_CNT		20
extern  TRACE_STATUS_STRUCT  CurTraceStatus;
extern 	TRACE_STATUS_STRUCT 	CalledTraceStatus;
extern  INT8U  FlagDisplayIndividual;
extern 	void	individualData_call(void);
extern 	INT8U individualNo_bind_flag;
extern 	TRACE_INDIVIDUAL_MULTI_DATA IndivdualMultiData[3];
extern 	INT16U IndividualMultiIndex;
extern	INT8U FlagIndividualMultiStart;

extern 	TRACE_INDIVIDUAL_MULTI_DATA IndivdualMultiData[MAX_INDIVIAL_MULTI_DATA_CNT];
extern 	TRACE_LOT_MULTI_DATA LotMultiData[MAX_LOT_MULTI_DATA_CNT];
extern	INT8U LotMultiDataNo[];
extern 	INT16U 	MultiIndex;
extern 	INT8U 	LotListMultiIndex;
extern 	INT8U 	FlagMultiStart;
extern 	INT8U 	FlagIndividualMultiStart;
extern 	INT8U 	FlagIndividualMultiRemain;
extern 	INT8U 	FlagLotMultiStart;
extern  INT8U TraceMeatGradeListText[2][64];	// 0:Beef, 1:Pork
extern	INT8U ExterPrt_TraceMeatGradeListText[2][64];	//외부 PRT 등급 리스트 // 0:Beef, 1:Pork
#if defined(USE_EMART_MEATGRADE_PRINT) || defined(USE_MARBLING_GRADE_PRINT)
extern	INT8U TraceMeatGradeList_Position[2][10];
extern	INT8U TraceMeatGradeList_Len[2][10];
#endif
extern  INT16U TraceMeatGradeMaxQty;
extern  INT8U TraceMeatGradeListNum[100];

extern 	void	individualData_create(INT8U mode);
extern 	void	individualNo_input(void);//gm 090403
extern 	INT8U	lotNo_input(void);//gm 090407
extern 	void	individualData_list(void);
extern 	void    individualData_delete(void);
extern 	void 	individualData_assign(void);
extern	INT16U	individualData_search(INT8U *indStr, INT8U mode);
#ifdef USE_CHECKING_HARMFUL_MEAT
extern  INT8U individualData_searchEmptyIdx(void);
#endif
extern	void individual_checkZero(INT8U *ind_str);
extern  INT8U checkIndividualNo(INT8U *ind_no_str);
extern  void	sale_display_indivFlag_set(INT8U mode, INT8U dspFlag);
extern  void 	sale_display_individual(void);	//display routine
extern  void	RemainWeightSummary(long trWeight, INT8U index);
//extern  void    refreshIndividualDisplay(long weight);
extern  void 	keyapp_trace_noneid(void);
extern  void    TotalWeightControl(void);
//extern  void 	GetIndividualNoByIndex(INT8U index, INT8U *indivNo);

extern void TNT_InitTraceNoTable(void);
extern void TNT_DelTraceNoTable(INT8U dept, INT32U pluno, INT16U plukey);
extern void TNT_SetTraceNoTable(INT8U dept, INT32U pluno, INT8U *ind_str1, INT8U *ind_str2, INT8U *ind_str3, INT8U sl_no1, INT8U sl_no2, INT8U sl_no3, INT8U meat_use, INT8U meat_part, INT8U log_flag, INT8U meat_grade);
extern void TNT_GetTraceNoTable(INT8U dept, INT32U pluno, INT16U *ind_no, INT8U *ind_str1, INT8U *ind_str2, INT8U *ind_str3, INT8U *sl_no1, INT8U *sl_no2, INT8U *sl_no3, INT8U *meat_use, INT8U *meat_part, INT8U *lot_flag, INT8U *meat_grade);
extern void TNT_SetTraceNoTable_Single(INT8U dept, INT32U pluno, INT8U *ind_no_str, INT8U sl_no, INT8U meat_use, INT8U meat_part, INT8U lot_flag, INT8U meat_grade);
extern void TNT_GetTraceNoTable_Single(INT8U dept, INT32U pluno, INT16U *ind_no, INT8U *ind_no_str, INT8U *sl_no, INT8U *meat_use, INT8U *meat_part, INT8U *lot_flag, INT8U *meat_grade);

//extern void SetIndividualTable(INT16U key, INT8U indvidualIndex);
//extern INT8U GetIndividualTable(INT16U key);
//extern void SaveIndividualTablebyPLU(INT8U deptNo, INT32U pluNo, INT8U individualIndex);
//extern void InitIndividualTable(void);
extern void	individualData_call(void);
extern void	slaughHouseNo_call(void);
extern void	meatUseNo_call(void);
extern void	individualNo_bind(void);
extern void indivdualMultiStart(void);
extern void indivdualLotListStart(void);
extern void	trace_specialuse_create(void);
extern void 	trace_specialuse_delete(void);
extern void 	trace_specialuse_list(void);
extern void	trace_slaughthouse_create(void);
extern void 	trace_slaughthouse_delete(void);
extern void 	trace_slaughthouse_list(void);
extern void		indivMultiAndLotIndexInit(void);
#ifdef USE_TRACE_MEATPART
extern void	meatPartNo_call(void);
extern void	trace_meatpart_create(void);
extern void trace_meatpart_delete(void);
extern void trace_meatpart_list(void);
extern void trace_meatpart_default_KR(void);
#endif
#ifdef USE_TRACE_MEATGRADE
extern void	trace_meatgrade_callGradeNo(void);
extern void	trace_meatgrade_create(void);
extern void trace_meatgrade_delete(void);
extern void trace_meatgrade_list(void);
extern void trace_meatgrade_default(void);
extern void trace_meatgrade_call_back(long num);
extern void trace_meatgrade_getGradeText(INT8U gradeNo, INT8U *gradeText);
extern void trace_meatgrade_getGradeListText(void);
#endif

#ifdef USE_KOR_SLAUGHT_CODE_4DIGIT
extern INT8U trace_slaughthousedata_insert(INT16U slaughtNo, INT8U *slaughtName);
extern INT8U trace_slaughthouse_search_index(INT16U *num, INT16U *tableId, INT16U *index, INT8U nextSearch);
extern INT16U trace_slaughthouse_no2index(INT16U num);
extern INT16U trace_slaughthouse_index2no(INT16U num);
extern void trace_slaughthousedata_init(void);
extern void trace_slaughthousedata_delete(INT16U slaughtNo);
extern void trace_slaughthousedata_delete_all(void);
#endif
#ifdef USE_TOPMART_STRUCTURE
extern void trace_lookup_breed_from_table(INT8U breed_no, char* breed_str, int breed_str_size);
extern void trace_lookup_slaughter_from_table(INT16U num, char* slaughter_name, int slaughter_name_size);
#endif
#endif
#pragma pack()

#ifdef USE_CHN_EMART_TRACE
#define CHN_EMART_TRACE_HEAD_SIZE	1
#define CHN_EMART_TRACE_COUNT_SIZE	1
#define CHN_EMART_TRACE_CODE_SIZE 20
#ifdef USE_CHN_TRACE_DEFAULT
#define CHN_EMART_TRACE_BACKUP_NO	5
#elif defined(USE_CHN_EMART_BIRDS_TRACE)
#define CHN_EMART_TRACE_BACKUP_NO	4
#else
#define CHN_EMART_TRACE_BACKUP_NO	3
#endif
#define CHN_EMART_TRACE_BACKUP_AREA_SIZE (CHN_EMART_TRACE_HEAD_SIZE + CHN_EMART_TRACE_COUNT_SIZE + CHN_EMART_TRACE_CODE_SIZE*CHN_EMART_TRACE_BACKUP_NO)

#ifdef USE_CHN_EMART_BIRDS_TRACE //20140827
#define CHN_EMART_TRACE_QR_SIZE 20 //20140911
#define CHN_EMART_TRACE_QR_NO 5
#define CHN_EMART_TRACE_QR_AREA_SIZE  (CHN_EMART_TRACE_QR_SIZE*CHN_EMART_TRACE_QR_NO)
#endif

extern void chn_emart_trace_save(INT16U key, INT8U *trace);
extern INT8U chn_emart_trace_read(INT16U key, INT8U *trace, INT8U order);
extern INT8U chn_emart_trace_direct_call_traceNo(INT8U order);
extern void chn_emart_trace_delete_all(void);
extern void chn_emart_trace_delete_key(INT16U key);

#ifdef USE_CHN_EMART_BIRDS_TRACE
extern void chn_emart_qr_trace_save(INT16U key, INT8U *trace);
extern void chn_emart_qr_trace_delete_key(INT16U key); //20140911
extern void chn_emart_qr_trace_delete_all(void); //20140911
#endif



extern INT8U CHNEmart_TraceSelectionOrder;
extern INT8U CHNEmart_CurTraceCode[];


#endif
