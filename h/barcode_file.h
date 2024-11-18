#ifndef _BARCODE_FILE_H_1
#define _BARCODE_FILE_H_1

#ifdef USE_BARCODE_LEN_60
#define BARCODE_FORMAT_LEN	60
#define BAR_INPUT_FORM_LEN	60
#define BARCODE_DESCRIPT_LEN	10
#elif defined (USE_BARCODE_LEN_70)
#define BARCODE_FORMAT_LEN	70
#define BAR_INPUT_FORM_LEN	70
#define BARCODE_DESCRIPT_LEN	10
#elif defined (USE_BARCODE_LEN_50)
#define BARCODE_FORMAT_LEN	50
#define BAR_INPUT_FORM_LEN	50
#define BARCODE_DESCRIPT_LEN	10
#elif defined (USE_BARCODE_LEN_90)
#define BARCODE_FORMAT_LEN	90
#define BAR_INPUT_FORM_LEN	90
#define BARCODE_DESCRIPT_LEN	20
#elif defined (USE_BARCODE_LEN_50_DES_20)
#define BARCODE_FORMAT_LEN	50
#define BAR_INPUT_FORM_LEN	50
#define BARCODE_DESCRIPT_LEN	20
#else
#define BARCODE_FORMAT_LEN	30
#define BAR_INPUT_FORM_LEN	40
#define BARCODE_DESCRIPT_LEN	20
#endif
#pragma pack(2)
typedef struct	{
	INT16U	barcode_num;
	INT8U	barcode_description[BARCODE_DESCRIPT_LEN];
	INT8U	barcode_type;
	INT8U 	barcode_format_num;
	INT8U	barcode_format[BARCODE_FORMAT_LEN];
} BARCODE_STRUCT;
#pragma pack()

#endif


#ifndef _BARCODE_FILE_H_2
#define _BARCODE_FILE_H_2



#define BARCODE_TYPE_0		0
#define BARCODE_TYPE_EAN13	1
#define BARCODE_TYPE_EAN13A5	2
#define BARCODE_TYPE_EAN8	3
#define BARCODE_TYPE_I2OF5	4
#define BARCODE_TYPE_UPCA	5
#define BARCODE_TYPE_UPC13	6
#define BARCODE_TYPE_CODE39	7
#define BARCODE_TYPE_CODE93	8
#define BARCODE_TYPE_CODE128	9
#define BARCODE_TYPE_CODABAR	10

#if defined(USE_RSS_BARCODE) || defined(USE_ONLY_RSS_EXPAND_BARCODE)
 #define BARCODE_TYPE_RSS14			11	//GS1 Databar Omnidirectional
 #define BARCODE_TYPE_RSS14S		12	//GS1 Databar Stacked
 #define BARCODE_TYPE_RSS14SO		13	//GS1 Databar Stacked Omnidirectional
 #define BARCODE_TYPE_RSSLIM		14	//GS1 Databar Limited
 #define BARCODE_TYPE_RSSEXP		15	//GS1 Databar Expanded
 #define BARCODE_TYPE_RSSEXPS 		16	//GS1 Databar Expanded Stacked only
#endif//#if defined(USE_RSS_BARCODE) || defined(USE_ONLY_RSS_EXPAND_BARCODE)
#ifdef USE_QR_BARCODE //20140903
 #define BARCODE_TYPE_QRCODE 		17
#endif
extern  void	barcodefile_create(void);
extern  void    barcodefile_delete(void);
extern  void 	barcodefile_list(void);

// Added by CJK
extern INT8U barcode_make_format(INT16S num, INT8U type, HUGEDATA INT8U *buffer);
//////////////


#ifdef USE_QR_ADDTOTAL
extern INT8U QrCfg_OpeningCommand[3];
extern INT8U QrCfg_PosId[3];
extern INT8U QrCfg_EanPrefix[3];
extern INT8U QrCfg_EanSuffix[3];
extern INT8U QrCfg_ClosingCommand[3];
extern INT8U QrCfg_DelLastSuffix;

extern void barcodefile_QR_config_default(void);
extern void barcodefile_QR_cofig(void);
#endif

#endif 
