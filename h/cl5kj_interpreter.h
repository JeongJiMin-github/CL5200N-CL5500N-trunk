/*****************************************************************************
|*			
|*  Copyright		:	(c) 2011 CAS
|*  Filename		:	cl5kj_interpreter.h
|*  Version			:	0.1
|*  Programmer(s)	:	Kang kiho (KKH)
|*  Created			:	2011/04/13
|*  Description		:	cl5000j protocol interpreter    
|*				
****************************************************************************/
#ifndef CL5KJ_INTERPRETER_H
#define CL5KJ_INTERPRETER_H
//
//header start
#define COMM_VERSION_REQUEST	0x01
#define COMM_DATA_WRITING	0x02
#define COMM_DATA_READING	0x04
#define COMM_VERSION_ARRIVED	0x10
#define COMM_DATA_WRITE_OK	0x20
#define COMM_DATA_READ_OK	0x40
#define COMM_ERROR		0x80

#pragma pack(1)
typedef struct {
	INT16U No;
	INT16U Offset;
} DATA_HEADER, *lpDATA_HEADER;

typedef struct
{
	INT8U  opCode[2];
	INT32U address;
	INT8U  delilimeter;
	INT16U  length;
	INT8U  colon;
} PACKET_HEADER;

typedef struct
{
	INT8U  colon;
	INT8U  checksum;
	INT8U  cr;
} PACKET_TAIL;

/*
#define PLU_NAME1_SIZE	40//CT100 = 31, CL5000J = 40
#define PLU_NAME2_SIZE	40
#define PLU_NAME3_SIZE	5

typedef struct{
	//INT8U	being;			// Exist PLU				1	0
	INT16U  deptNo;			// Department Number		2	2
	INT32U 	no;				// PLU Number				4	6
	INT8U	type;			// PLU Type					1	7
	INT8U	name1[PLU_NAME1_SIZE];		// PLU Name 1				40	47
	INT8U	name2[PLU_NAME2_SIZE];		// PLU Name 2				40	87
	INT8U 	name3[PLU_NAME3_SIZE];		// PLU Name 3				5	92
	INT16U	groupNo;		// Group Number				2	94
	INT16U	labelNo;		// Label Number				2	96
	INT16U  auxLabelNo;		//N Aux Label Number			2	98
	INT16U	orgNo;			// Origin Number			2	100
	INT8U	unitW_no;		//N Unit Weight Number		1	101
	INT32U  fixedWeight;	// Fixed Weight				4	105
	INT32U	icode;			// Item Code				4	109
	INT16U	pcs;			// Pieces					2	111
	INT8U	pcs_symNo;		// Q'ty Unit Number			1	112
	INT8U	fixedPrice;		// Use Fixed Price Type		1	113
	INT32U	unitPrice;		// Unit Price				4	117
	//INT32U	SpecialPrice;	//N Special Price			4	121
    INT8U   taxNo;          // tax no   1
    INT8U   reserved[3];    // reserved 3
	INT32U	tare;			// Tare						4	125
	INT8U   tareNo;			//N Tare Number				1	126
	INT16U	barNo1;			// Barcode Number			2	128
	INT16U	barNo2;			// Barcode Number			2	130

	INT16U	pdDate;			// Produced Date			2	132  //현재에서 뺀다. 0-프린트 하지 않음, 1-오늘, 2-어제
	INT16U	pDate;			// Packed Date				2	134  //0-오늘, 1-내일
	INT8U	pTime;			// Packed Time				1	135  //0-현재, 1-1시간 뒤
	INT32U	sDate;			// Sell By Date				4	139  //0-프린트 하지 않음, 1-오늘, 2-내일
	INT8U	sTime;			// Sell By Time				1	140  //0-현재, 1-1시간 뒤

	INT16U	msgNo;			// Message Number			2	142
	INT16U	traceNo;		//N traceability Number		2	144
	INT16U	nutriNo;		// nutrifact Number			2	146
	INT8U	smsgNo;			// Label Sales Message Number		1	147

	INT16U	dmsgNo;			// direct Message Number		2	149
//  for future use (LP-17)
// 	INT16U	cDate;			// Cook by Date				2	0
//	INT16U	pc_tare;			// Percent tare				2	0  
// 	INT8U	chkSellby;		// Sell By Date Check		1	0  
// 	INT8U	chkCookby;		// Cook By Date Check		1	0  
// 	INT8U	prefix[14];		// Reserve					14	0
} PLU_INF;//check PLU_STORAGE_SIZE 

typedef struct {
	INT32U DeptPluNo;
	INT16S Offset;
} PLU_INDEX, *lpPLU_INDEX;

//Clerk structure
typedef struct{
    INT8U   name[24];
    INT8U   nickname[8];
    INT8U   password[8];
    INT8U   permissionLevel;
    INT8U   permissionID;
    INT8U   Dept;
    INT8U   Headid;
    INT8U   Shopid;
    INT8U   Training;
    INT8U   Reserved[80];
} CLERK_INF;//check CLERK_STORAGE_SIZE 

//tax structure
typedef struct{
    INT16U  type;
    INT32U  rate;
} TAX_INF;//check TAX_STORAGE_SIZE 
*/

#define PLU_QTY_MAX	(PLU_BASIC_AREA_QTY + PLU_EXTER_AREA_QTY)//6000//CL5000J
/*
#define IMSG_QTY_MAX		100
#define DMSG_QTY_MAX		1000
#define PLU_STORAGE_SIZE	149
#define IMSG_STORAGE_SIZE	401
*/
#define DMSG_STORAGE_SIZE	1051//dynamic ingredient
#define PLU_NUM_MAX_RANGE 999999

#define PLU_HOST_STORAGE_SIZE	(PLU_STORAGE_SIZE-2) //except dmsgNo
#define DMSG_HOST_STORAGE_SIZE	(DMSG_STORAGE_SIZE-1)//except being
#define IMSG_HOST_STORAGE_SIZE	(IMSG_STORAGE_SIZE-1)//except being

#define BAR_STORAGE_SIZE		33	//being(1)-bartype(1)-structure(31)
#define BAR_HOST_STORAGE_SIZE	32	//bartype(1)-structure(31)
#define BAR_QTY_MAX				20

#ifdef USE_RUSSIA_DEFAULT
#define SHOP_QTY_MAX	6
#else
#define SHOP_QTY_MAX	10
#endif

#ifdef USE_RUSSIA_DEFAULT
#define FLASH_SCALE_SHOP_HOST_SIZE	196 //without being flag
#else
#define FLASH_SCALE_SHOP_HOST_SIZE	126 //without being flag
#endif

#define FLASH_SCALE_SHOP_SIZE		127	// being(1), name(26), tel(20), address(80) // id(2)는 CL5000과 달리 사용하지 않음

#define SCMSG_STORAGE_SIZE		101
#define SCMSG_HOST_STORAGE_SIZE	(SCMSG_STORAGE_SIZE - 1)//except being
#define SCMSG_QTY_MAX		10

#define GROUP_STORAGE_SIZE	33
#define GROUP_CT100_STORAGE_SIZE 32 //CL5000J 28
#define GROUP_HOST_STORAGE_SIZE	(GROUP_STORAGE_SIZE - 1)//except being
#define GROUP_QTY_MAX		100

/*
#define CLERK_STORAGE_SIZE	    126
#define CLERK_QTY_MAX	        8//CT100 = 8 + 1(no clerk), CL5000J = 32

#define TAX_STORAGE_SIZE	    6
#define TAX_QTY_MAX	            8//CT100 = 8 + 1(no tax) 

#define DEPT_STORAGE_SIZE	33
#define DEPT_CT100_STORAGE_SIZE 32 //CL5000J 28
#define DEPT_HOST_STORAGE_SIZE	(DEPT_STORAGE_SIZE - 1)//except being
#define DEPT_QTY_MAX		99

#define TRACE_STORAGE_SIZE	4+32
#define TRACE_HOST_STORAGE_SIZE	    TRACE_STORAGE_SIZE//(TRACE_STORAGE_SIZE - 1)//except being
#define TRACE_QTY_MAX		99

#define SHOP_QTY_MAX	10
*/

//#define COM_MAX_TX_BUF_HOST_SIZE	(600+32)//(MAX_SIZE_MsgBuf+32)
//#define COM_MAX_RX_BUF_HOST_SIZE	(600+32)//(MAX_SIZE_MsgBuf+32)
//header end
  
#define DATA_LABEL		'A'
#define DATA_BARCODE	'B'
#define DATA_NUTRIFACT	'C'
#define DATA_FONT		'D'
#define DATA_EEPROM		'E'
#define DATA_FLASH		'F'
#define DATA_SMSG		'G'
#define DATA_SCMSG		'H'
#define DATA_DIRECT_KEY 'I'
#define DATA_BITMAP		'J'
#define DATA_KEY		'K'
#define DATA_PLU		'L'
#define DATA_IMESSAGE	'M'
#define DATA_STATUS		'N'
#define DATA_ORIGIN		'O'
#define DATA_PARAMETER	'P'
#define DATA_GROUP		'Q'
#define DATA_RAM		'R'
#define DATA_SHOP		'S'
#define DATA_TARE		'T'
#define DATA_ACCOUNT	'U'
#define DATA_VERSION	'V'
#define DATA_DEPT		'W'
#define DATA_CTEXT		'X'
#define DATA_PCS_SYMBOL	'Y'
#define DATA_DATE		'Z'//date
#define DATA_RECEIPT    'a'
#define DATA_BOOTH      'b'
#define DATA_CLERK		'c'
#define DATA_LABEL_CL55 'd'
#define DATA_HEADTAIL   'h'
#define DATA_PLU2		'l' //0x6c
#define DATA_RTT		'r'
#define DATA_TRACE		's'
#define DATA_CLERK		'c'
#define DATA_TAX		't'
#define DATA_MEAT_TRACE 'm'
#define DATA_VEGE_TRACE 'v'
#define DATA_REMOTE_KEY '!'
#define DATA_REMOTE_DISP '"'
#define DATA_LOGISTICS_PLU	'#'
#define DATA_LOGISTICS_INGREDIENT	'$'

// Scale Parameter Number
//#define	PARAM_PCS_SYMBOL		1 --> DATA_PCS_SYMBOL
#define	PARAM_MONTH_SYMBOL		2
#define PARAM_WEEK_SYMBOL		3
#define PARAM_AMPM_SYMBOL		4
#define	PARAM_QUESTION_SYMBOL	5
#define	PARAM_WEIGHT_SYMBOL		6
#define PARAM_CURRENCY_SYMBOL	7

//<<< Error number >>>
#define ERROR_DATA_END		99 // Data end (데이터가 더 이상 없음 upload 시)
#define ERROR_DATA_NON		98 // 해당 고유번호에 해당 하는 데이터가 없음 (예, PLU 20000번 upload시)
#define ERROR_DATA			97 // 데이터 오류 (데이터가 형식 또는 크기가 맞지 않음)
#define ERROR_INTERPRETER	95 // 데이터를 해석 불능, unknown data type char
#define ERROR_DATA_FULL		89 // 저장 공간 없음 (full)
#define ERROR_DMSG_FULL		88 // direct message 저장 공간 없음 (full)
#define ERROR_RECEIVE		84 // 데이터 Receive 오류 (header, tail, checksum)
#define ERROR_NUMBER_OVER	82 // 고유 번호 범위 오류 (예, PLU 번호 1~999999)

#define LONG_EXE_ERASE_ALL_PLU	0xf1

typedef struct{
	INT8U btLoadFlag;		//0:Zero, 1:Non Zero, 2:Overload
	INT8U btStableFlag;		//0:Unstable, 1: Stable
	INT8U btTareFlag;		//0:No tare, 1:tare, 2:Percent tare
	INT8U btDualFlag;		//2: dual range
	INT8U btWeightUnit;		//0:kg, 1:lb, 2:g
	INT8U btWeightDecimalPoint;
	INT8U btPriceDecimalPoint;
	INT8U btReserved;
	INT32U dwCurTare;
	INT32S iCurWeight;
	INT32U dwUnitPrice;
	INT32U dwTotalPrice;
	INT32U dwPLUno;
	INT16U wdDeptNo;
} SCALE_STATUS_STRUCT;

typedef struct{
	INT8U	StoreNo;
	INT16U	DeptNo;
	INT16U	GroupNo;
	INT8U	ScaleId; //scale No.
	INT8U	saleType; //  NO_SALE : 0, SALE : 1, VOID : 2, RETURN : 3
	INT8U	Plutype;
	INT8U	ClerkNo;
	INT32U 	PluNo;
	INT32U	Weight;
	INT32U 	Quantity;  //value error
	INT16S	PCS; //add
	INT32U	TareWeight;//add
	INT32U	UnitPrice;
	INT32U	TotalPrice;
	INT32U	ItemCode; //PLU code
	INT8U	Date[6];
	INT8U	BarcodeStr[20];
	INT32U	LabelTranNo;//serial no
	INT8U	reserved[9];
	INT8U   PLU_name[32];
}RTT_STRUCT;
#pragma pack()

#define ACC_NO				0
#define ACC_ADD				1       //key print, add sale
#define ACC_VOID			2
#define ACC_RET				3       //key return

//CL5000JR 버전 구조
#pragma pack(1)
typedef struct {
	INT8U Class;
	INT16U ModelNum;//ex) CT100 = 100, CL5000J = 5010
	INT8U Type;//1 = Bench, 2 = Pole, 3 = Hanging, 4 = Self, 9 = Printer(?)
	INT16U ScaleId;
	INT16U MainVersion;//1
	INT16U SubVersion;//0
	INT16U DataVersion;//0 //high byte = FLASH STRUCT VERSION, low byte = EEPROM STRUCT VERSION
	INT16U CountryNum;//not used
	INT8U CountryCode[2];
} VERSION_STRUCT;
#pragma pack()



#define ACC_GRAND_TOTAL		100000000L
#define ACC_ALLPLU_TOTAL	100000001L
#define ACC_NONPLU_TOTAL	100000002L

#define ACC_START_DATETIME	100000010L

#define ACC_LOG_TOTAL   	110000001L //(110000001 ~ 110001000)
#define ACC_DEPT_TOTAL		200000000L //(200000000 ~ 200000099)
#define ACC_GROUP_TOTAL		300000000L //(300000000 ~ 300000099)
#define ACC_HOURLY_TOTAL	400000000L //(600000000 ~ 600000023)
#define ACC_CLERK_TOTAL	    500000000L //(400000000 ~ 400000032)
#define ACC_TAX_TOTAL	    600000000L //(500000000 ~ 500000009)
#define ACC_WEEKLY_TOTAL    700000000L //(700000000 ~ 700000006)
#define ACC_MONTHLY_TOTAL   800000000L //(800000000 ~ 800000011)
#define ACC_TRACE_TOTAL     900000000L //(900000000 ~ 900000099)

INT16U CL5_JR_transaction_sale_make_data(char *buffer, INT8U code, INT16U head, INT16U tail, INT16U *max_cnt);
#endif // #define CL5KJ_INTERPRETER_H