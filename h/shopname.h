#ifndef _STORE_NAME_H_1
#define _STORE_NAME_H_1

#ifdef USE_RUSSIA_DEFAULT
#define		SHOPNAME_STRUCT_SIZE			198
#else
#define		SHOPNAME_STRUCT_SIZE			128
#endif
#pragma pack(2)
typedef struct
{
	INT16U 		code;
	INT8U		name[26];
	INT8U		tel_no[20];
#ifdef USE_RUSSIA_DEFAULT
	INT8U		text[150];
#else
	INT8U		text[80];
#endif
}	STORE_NAME_STRUCT;
#pragma pack()
#endif
  
#ifndef _STORE_NAME_H_2
#define _STORE_NAME_H_2

extern 	void	store_name_create(void);		// 0x1211 
extern  void    store_name_delete(void);		// 0x1212
extern  void    store_name_list(void);			// 0x1213
extern  void 	store_call_back(long num);
#endif 

#ifndef _JOB_BATCH_SCHEDULE
#define _JOB_BATCH_SCHEDULE

#define		JOB_BATCH_STRUCT_SIZE	16
#pragma pack(2)
typedef struct
{
	INT16U 		code;		// job batch #
    INT16U		deptNO;	 
	INT32U		pluNO;		
	INT16U		quantity;
	INT32U		unitPrice;
	INT8U		salesMsgNO;
	INT8U		shopNameNO;
}	JOB_BATCH_STRUCT;

extern 	void	job_batch_create(void);		 
extern  void    job_batch_delete(void);		
extern  void    job_batch_list(void);

//SG070118. 
#ifdef USE_EXTENDED_SCANNERFORM
#define SCANNER_BARCODE_DATA_LEN  41
#define SCANNER_BAR_MAX_SIZE 67
#else
#define SCANNER_BARCODE_DATA_LEN  32
#define SCANNER_BAR_MAX_SIZE SCANNER_BARCODE_DATA_LEN
#endif
typedef struct {
	INT8U barcodeData[SCANNER_BAR_MAX_SIZE];
	INT8U barcodeIndex;
//	INT32U weight;
//	INT32U pluno;
//	INT8U scanStartIndex;
//	INT8U scanBarcodeLength;
//	INT8U scanBarcodeNo;
	INT8U weightStartIndex;
	INT8U weightLength;
} SCANNER_DATA_STR;

#ifdef USE_10000_SCANNER_CONVTABLE
#define SCAN_BACODE_DATA_MAX_NUM    10000
#else
#define SCAN_BACODE_DATA_MAX_NUM    1999
#endif
#define SCANNER_CONV_TABLE_STR_SIZE 22
typedef struct {
	INT16U code;	     //2
	INT8U scanCode[14];	 //14
	INT16U dept_no;		 //2
	INT32U plu_no;		 //4
} SCANNER_CONV_TABLE_STR;
#pragma pack()
extern 	void	scanner_convtable_create(void);
extern 	void	scanner_convtable_list(void);
extern 	void	scanner_convtable_delete(void);
extern 	void	scanner_config(void);

extern  void    label_format_key_table(void);		
extern INT16U get_labelid_from_labelkey_num(INT32U num);

#endif