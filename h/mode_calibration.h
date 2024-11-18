#ifndef CAL_HISTORY_DEF
#define CAL_HISTORY_DEF
#pragma pack(2)
typedef struct
{
	INT8U year;
	INT8U month;
	INT8U date;
	INT8U hour;
	INT8U min;
	INT8U sec;
	INT32U capa;
	INT32U ad_zero;
	INT32U ad_span;
	//INT8U  sensor_gab;
	//INT8U  sensor_peel;
	INT32U ad_midup;
	INT32U ad_middn;
	INT8U ver; // history 저장 타입
	INT8U reserved[3]; // Total 30 bytes assigned
} CAL_HISTORY;

// history 저장타입
//  0 : 최초
//  1 : sensor_gab, sensor_peel 제거, ad_midup, ad_middn 추가
#define CAL_HISTORY_VER 1


#ifdef USE_CTS_FUNCTION
typedef struct
{
	INT8U  ModelName[12];
	INT8U  SerialNumber[16];
	INT32U  FWVersion;
	INT8U  CalDate[6];
	INT32U CalZero;
	INT32U CalMidUp;
	INT32U CalMidDn;
	INT32U CalFull;
	INT16U GravityFactory;
	INT16U GravityLocal;
	INT32U Capa;
	INT8U  e;
	INT8U  d;
	INT8U  wdp;
	INT8U  unit[2];
	INT8U  ctsEnable;
	INT16U ctsPeriod;
	INT8U  calState;
	INT8U  structVer;
	INT8U  productCode[16];
	INT32U boardId;
	INT8U  reserved[28];
} CAL_INFORMATION;
#endif



#pragma pack()
#endif

#define HISTORY_CAPA	1
#define HISTORY_ADZERO  2
#define HISTORY_ADSPAN  3
//#define HISTORY_SENSORGAB	4
//#define HISTORY_SENSORPEEL	5
#define HISTORY_ADMIDUP  4
#define HISTORY_ADMIDDN  5

//#define HISTORY_SIZE (HISTORY_STRUCT_SIZE*HISTORY_QTY + 2 + 6 + CTS_STRUCT_SIZE + CTS_STRUCT_SIZE2 + 2 + LIC_STRUCT_SIZE)

#define HISTORY_BUF_SIZE 332
  // HISTORY_INDEX = 2
  // HISTORY_STRUCT_SIZE*HISTORY_QTY = 30*4 = 120
  // MAC Address = 6
  // CTS_STRUCT_SIZE = 32
  // CTS_STRUCT_SIZE2 = 28
  // CTS_STRUCT_SIZE2_reserved = 2
  // LIC_STRUCT_SIZE = 80
  // FLASH_USB_DATA_ROM_UPDATE_SIZE = 1
  // FTL_ERROR_MANAGEMENT_SIZE = 48
  // START_COUNT = 4
  // FWMODE = 1
  // #ifdef USE_ADM_AUTO_RESTORE
  // ADM INIT ERROR COUNT = 1
  // ADM EEP ROM ERROR COUNT = 1
  // ADM INIT COMPLETE = 1
  // Reserved = 5
  // #else
  // Reserved = 8
  //  TOTAL = 332

#define NVRAM_BACKUP_FLAG_HISTORY_WRITTEN 0x52011234
#define NVRAM_BACKUP_FLAG_HISTORY_INIT    0x52010000

extern void mode_calibration(void);
extern void history_start(void);
extern void history_save(INT8U mode,INT32U value);
extern void history_flash2ram(HUGEDATA INT8U *ram);
extern void history_ram2flash(HUGEDATA INT8U *ram);
extern void history_qty_reduction(void);

#ifdef USE_CTS_FUNCTION
extern void CTS_InitCtsData(void);
extern void CTS_UpdateCtsData(void);
extern void mode_waiting_cal(void);
extern INT8U CTS_CheckCalibrationDate(void);
#endif