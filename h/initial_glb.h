#ifndef PGM_DEALER_SET
#define PGM_DEALER_SET

#define	INPUT_NUMERIC		0
#define	INPUT_STRING		1
#define	INPUT_BIT		2
//#define	INPUT_YESNO	3
#pragma pack(4)
typedef struct {
	INT16U	funcCode;	// function code number
	INT16U	captionNum;	// caption 주소 ParameterUserSetting에서는 초기값으로 사용
	INT32U	addrMemory;	// Memory address digit
	INT32U 	offsetMemory;   // Memory offset 
	INT8U	sizeMemory;	// bytes
	INT8U	bitFlag;	// 1:Byte, 2:Word, 3:String, 4:Bit, 5:Long
	INT8U	startBit;	// 상위비트 부터 시작 <0 ~ 15>, ex) start 3, countbit 2 이면 xxxx xxxx xxxx DDxx
	INT8U	countBit;	// bit count
	INT8S	country;        // country code number 
}PARAMETER_SETTING_TYPE;

typedef struct {
	INT16U funcCode;
	INT8U  country;
	INT8U  defaultValue;
} COUNTRY_SETTING_TYPE;
#pragma pack()
#define COUNTRY_EMPTY	-1
#define	COUNTRY_UN		0
#define	COUNTRY_KR		1
#define	COUNTRY_US		2
#define	COUNTRY_EU		3
#define	COUNTRY_EUT		4
#define	COUNTRY_RU		5
#define	COUNTRY_ETC		6
#define	COUNTRY_CN		7
	
#define	FACTORY_MODE	1
#define DEALER_MODE		2
extern INT16S CodeSearchParameter(INT16U funcCode);
extern void pgm_parameter_function(INT16U fcode);
extern void SaveCodeParameter(INT32U arrayid, INT32U defaultValue);
extern void CodeDefaultParameterByCode(INT16U funcCode);
extern void ParameterAction(INT32U paramNum, INT8U changed);
extern void DefaultDealerParamSetting(INT8U selCountry, INT8U mode);
#ifdef USE_CTS_FUNCTION
extern void history_add(void);
#endif

void init_by_funcCode(const INT16U*);
void parameter_init_by_idx(INT16U, INT16U);
INT16U search_parameter_country(INT16U);
INT16U search_parameter_setting(INT16U);

#define PARAMETER_COUNTRY_SEARCH_FAIL (-1)
#define PARAMETER_SETTING_SEARCH_FAIL (-1) 


#endif
