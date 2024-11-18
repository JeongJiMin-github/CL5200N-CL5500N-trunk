/*****************************************************************************
|*  Copyright		:	(c) 2004 CAS
|*  Filename		:	scale_config.h
|*  Version			:	0.9
|*  Programmer(s)	:	getwing
|*  Created			:	2004/2/13
|*  Description		:	Scale configure
****************************************************************************/

#ifndef SCALE_CONFIG
#define SCALE_CONFIG

// JHC 040408 <ADD>
// NOTE : Menu->8.ScaleConfig->6.ScaleParameter
// [
extern void	scale_set_operationmode(void);
extern void scale_parameter_display(void);	  
extern void scale_parameter_scalesetup(void);
extern void scale_parameter_logoff(void);	  
extern void scale_set_salemode(void);
extern void salemode_reset(void);
// ]

extern void	scale_set_name(void);
extern void	scale_set_scale_id(void); //set_network_id
extern void	scale_set_department(void);	// set_network_department
#ifdef USE_USB_DATA_ROM_UPDATE
extern void dataRomStatusSave(void);
extern void dataRomStatusLoad(void);
#if defined(USE_REMOTE_FW_DOWNLOAD) || defined(USE_REMOTE_ROM_UPDATE)
extern void SaveRemoteUpdateFlag(INT8U flag);
extern void LoadRemoteUpdateFlag(void);
#endif
extern INT8U data_rom_usb_update(void);
#if defined(USE_REMOTE_ROM_UPDATE)
extern INT8U data_rom_eth_update(void);
#endif
extern INT8U restore_data(void);
extern INT8U zig_read_data(void);
extern INT8U backup_data(void);
extern INT8U zig_backup_data(void);
#ifdef USE_USB_ZIG_TEST
extern INT8U zig_usb_write(void);
extern INT8U zig_usb_read(void);
#endif
extern void partial_keep_init_set(void);
extern void partial_init_sel_plu(void);
extern void partial_init_cus_label(void); 
extern void partial_init_key_set(void); 
extern void partial_init_scale_param(void);
extern void partial_init_clerk_perm(void); 
extern void partial_init_scanner(void); 
extern void partial_init_prt_set(void);
extern void partial_init_net_set(void); 
#define SIZE_REAL_BASICSYMBOL	268L

#define INI_CHECK_STATUS          0
#define INI_SECTION               1
#define INI_GET_SECTION_NAME      2
#define INI_SAVE_SECTION_NAME     3
#define INI_CHECK_CRLF            4
#define INI_KEY                   5
#define INI_INDEX_VALUE           6
#define INI_DOWNLOAD_VALUE        7
#define INI_SCALE_TYPE_VALUE      8
#define NEXT_SECTION              9

#define USB_DATAROM_UPDATE_STATUS_NONE   0x00
#define USB_DATAROM_UPDATE_STATUS_CLEAR  0x01
#define USB_DATAROM_UPDATE_STATUS_END    0x02
#define USB_UPDATE_BLOCK_SIZE			 512


#define DATAROMUPDATE_MEMCLR   0x01 // memory clear
#define DATAROMUPDATE_OTHERS   0x10 // 기타
#define DATAROMUPDATE_FWUPDATE 0x02 // firmware update
#define DATAROMUPDATE_CONFIRM  0x80 // update에 대한 확인 여부

#define DATAROMUPDATE_COMPLETE 0x00
#define DATAROMUPDATE_READY    (DATAROMUPDATE_MEMCLR | DATAROMUPDATE_OTHERS | DATAROMUPDATE_FWUPDATE | DATAROMUPDATE_CONFIRM)

#define USB_READ_WRITE_ERROR		-1

#pragma pack(1)	//datarom header start
typedef struct {
	char ver[32];
	INT16U 	param_id;
	INT32U  param_pnt;
	INT32U  param_size;

	INT32U  symbol_pnt;
	INT32U  symbol_size;

	INT32U  caption_pnt;
	INT32U  caption_size;

	INT32U  origin_pnt;
	INT32U  origin_size;

	INT16U 	keypad_id;
	INT32U  keypad_pnt;
	INT32U  keypad_size;

	INT32U  font_pnt;
	INT32U  font_size;

	INT16U 	label_id;
	INT32U  label_pnt;
	INT32U  label_size;
} MASTER_MEMORY;

typedef struct
{
	INT32U	addr;
	INT32U	size;
	INT16U	id;
	INT16U	width;
	INT16U	height;
} LABELIMAGEINFO;	//14

typedef struct
{
	char		format[6];
	char		version[2];
	char		company[10];
	char		model[4];
	char		usage[4];
	INT16U		id;
	INT16U		width;
	INT16U		height;
	char		name[16];
	INT32U		size_label;
	INT32U		item_addr;
	INT16U		item_count;	//58
	LABELIMAGEINFO	stImageIf[5];	//14 * 5 = 70
} LABELHEADER;

typedef struct {
	INT16U  code;
	char data[32];
} PARAMETER_FILE_STRUCT; 

typedef struct{
    INT8U section[256];
    INT32U index;
    INT8U download;
}ROM_DOWNLOAD_INI;

#ifdef USE_CHK_LABEL_CLR_WHEN_USB_UPDATE
#define DATAROM_INI_NO 12
#else
#define DATAROM_INI_NO 11
#endif
typedef struct{
    INT32U scaleType;
    INT8U download[DATAROM_INI_NO];
}ROM_DOWNLOAD_INF;
#pragma pack()	//datarom header end

struct DATAROM_INFO {
	INT32U pointer;
	INT32U size;
	INT32U type;
};

enum DATAROM_LIST {
	ROM_PARAM,
	ROM_SYMBOL,
	ROM_CAPTION,
	ROM_ORIGIN,
	ROM_KEYPAD,
	ROM_FONT,
	ROM_LABEL,
#ifdef USE_EXTEND_CAPNTION_AREA
	ROM_CAPTION_EXT,
#endif
};
#endif

// 라벨 영역
enum LABEL_AREA
{
	LABEL_AREA_HEADER,	// 라벨 헤더 영역
	LABEL_AREA_IMAGE,	// 라벨 이미지 영역
};

// 라벨 헤더 구조 정보
#define LABEL_HEADER_IMG_NAME_SIZE 16
#pragma pack(1)
struct LABEL_HEADER_INFO
{
	INT16U 	label_id;
	INT16U 	img_w;
	INT16U 	img_h;
	INT8U	img_name[LABEL_HEADER_IMG_NAME_SIZE];
	INT16U 	img_size;
};
#pragma pack()

#define LABEL_HEADER_OFFSET_ID 		(offsetof(struct LABEL_HEADER_INFO, label_id))
#define LABEL_HEADER_OFFSET_W  		(offsetof(struct LABEL_HEADER_INFO, img_w))
#define LABEL_HEADER_OFFSET_H  		(offsetof(struct LABEL_HEADER_INFO, img_h))
#define LABEL_HEADER_OFFSET_NAME 	(offsetof(struct LABEL_HEADER_INFO, img_name))
#define LABEL_HEADER_OFFSET_SIZE 	(offsetof(struct LABEL_HEADER_INFO, img_size))

// 라벨 헤더 구분하기 위한 비트 플래그
enum LABEL_HEADER_BIT_FLAG
{
	// xxx0 0000
	L_H_NONE_BIT = 0x00, // none
	L_H_ID_BIT   = (0x01 << 0), // Label Header Label id
	L_H_I_W_BIT  = (0x01 << 1), // Label Header Image width
	L_H_I_H_BIT  = (0x01 << 2), // Label Header Image height
	L_H_I_N_BIT  = (0x01 << 3), // Label Header Image name
	L_H_I_S_BIT  = (0x01 << 4), // Label Header Image size
	L_H_ALL_BIT  = L_H_ID_BIT | L_H_I_W_BIT | L_H_I_H_BIT | L_H_I_N_BIT | L_H_I_S_BIT, // all
};

#define LABEL_ADDR_ERR	0xFFFFFFFF	// 잘못된 라벨 주소 반환값
#define LABEL_CLEAR_DATA 0xFF		// 라벨 지울 때 초기값

extern INT32U get_addr_from_label_idx(const INT16U, const enum LABEL_AREA);
extern void save_label_header(INT32U, const struct LABEL_HEADER_INFO*, const enum LABEL_HEADER_BIT_FLAG);
extern void read_label_header(INT32U, struct LABEL_HEADER_INFO*, const enum LABEL_HEADER_BIT_FLAG);
extern INT32U get_label_header_bit_flag_cnt(void);

// Menu 1891 설정
#pragma pack(1)
struct M1891_SETTINGS 
{
	union 
	{
		INT16U bits;
		struct 
		{
			unsigned int printer_hardware : 1;
			unsigned int network          : 1;
			unsigned int label_format     : 1;
		};
	};
};
#pragma pack()

// Menu 1731
// 프린트 모드 타입
enum PRINT_MODE_TYPE
{
	MODE_LABEL = 0,
	MODE_TICKET,
	MODE_CONT_LABEL,
	PRINT_MODE_TYPE_QTY, // 프린트 모드 개수
};

// 모드별 프린터 세팅 값
#pragma pack(1)
struct PRINTER_SETTING_VALUE_BY_MODE
{
	INT16U label_val;
	INT16U ticket_val;
	INT16U cont_label_val;
};

struct PRINTER_PAPER
{
	struct PRINTER_SETTING_VALUE_BY_MODE width;
	struct PRINTER_SETTING_VALUE_BY_MODE height;
	struct PRINTER_SETTING_VALUE_BY_MODE end_margin;
};

// Menu 1733
struct SENSOR_CALIBRATION
{
	INT8U gap;
	INT8U peel;
};

// Menu 1734 Printer Sensor 각각 항목
struct PRINTER_SENSOR_EACH
{
	union
	{
		INT8U bits;
		struct 
		{
			unsigned int peel_off    			: 1; // Active peel-off
			unsigned int cl_3000_printer_mech 	: 1; // ifndef _CL_3000_PRINTER_MECH
			unsigned int label_paper			: 1; // Label Paper(0:NoGap, 1:Gap)
		};
	};
};

// Menu 1734
struct PRINTER_SENSOR
{
	struct PRINTER_SENSOR_EACH prt_sensor[PRINT_MODE_TYPE_QTY]; // Label, Ticket, Cont.Label
	INT8U headup; // Active Headup sensor
};

struct PRINTER_INFO
{
	INT8U  mode;
	struct PRINTER_PAPER 	paper;
	struct PRINTER_SENSOR 	sensor[PRINT_MODE_TYPE_QTY]; // label, ticket, cont_label
	INT8U  headup_sensor;
};

// Menu 1737
struct LABEL_PRE_PRINT
{
	INT8U mode;
	INT8U length;
};

// Menu 1911
struct SERVICE_TYPE
{
	INT16U scale_no;
	INT8U  service_type;
};

// Menu 1913
#define IP_SIZE 			4
#define GATEWAY_SIZE 		4
#define SUBNET_MASK_SIZE 	4
struct IP_CONFIG
{
	INT8U  ip[IP_SIZE]; // Scale IP
	INT8U  gateway[GATEWAY_SIZE];
	INT8U  subnet_mask[SUBNET_MASK_SIZE];
	INT16U tcp_port;
};

// Menu 1914
#define REMOTE_IP_SIZE 4
struct REMOTE_IP_CONFIG
{
	INT8U  ip[REMOTE_IP_SIZE];
	INT16U tcp_port;
};

// Menu 1916
// DataFlash Structure에서 WLAN AP 영역
#define SSID_SIZE 				34
#define WEP_KEY_SIZE 			16
#define WLAN_SECURITY_KEY_QTY 	4
struct WLAN_SECURITY_KEY
{
	INT8U wep_key[WEP_KEY_SIZE];
};

struct WLAN_AP
{
	INT8U  ssid[SSID_SIZE];
	struct WLAN_SECURITY_KEY security[WLAN_SECURITY_KEY_QTY];
	INT8U  wep_keyid;
	INT8U  use_wep; // /Menu 1916 security_setting
	//INT8U encrypt_etc[9]; // 무슨 영역인지 아직 확인 안됨 
};

// GLOBAL_PRINT_ADDUP_FLAG2
struct ADDUP_PRINT_FLAG2_T
{
	union
	{
		INT8U bits;
		struct
		{
			unsigned int reserved1           : 2; // [0, 1]bit 아직 파악 못함
			unsigned int reverse_total_label : 1; // [2] Reverse Total Label
			unsigned int reverse_item_label  : 1; // [3] Reverse Item Label
		};
	};
};

// Menu 1510
#define EXT_LABEL_NAME_SIZE 7
struct LABEL_FORMAT_M1510_T
{
	INT8U  label_priority; // Use global label1(1=Y/0=N)
	INT16U standard_label_id; // PLU Label No.
	INT16U misc_label_id; // Misc. Label No.
	INT16U addup_label_id; // Total Label No.
	INT16U fsp_label_id; // GlbOper.UseFSP 사용할 때만 이용
	struct ADDUP_PRINT_FLAG2_T addup_print_flag2; // Reverse Total Label, Reverse Item Label 
	INT8U  label2; // Label2 #
	INT8U  ext_label_name[EXT_LABEL_NAME_SIZE]; // Ext. Label Name	
};

// Parameter 730, 731
struct PARAMETER_730_731
{
	union
	{
		INT8U bits;
		struct
		{
			unsigned int ethernet        : 1; // Parameter 730, Ethernet(TCP/IP)
			unsigned int wireless_bridge : 1; // Parameter 731
		};
	};
};

#pragma pack()

enum ADDUP_PRINT_FLAG2_BIT
{
	REVERSE_TOTAL_LABEL_BIT = 2,
	REVERSE_ITEM_LABEL_BIT,
};

// Menu 1916 - Security Setting
enum SECURITY_SETTING
{
	SECURITY_SETTING_NONE = 0,
	WEP_64,
	WEP_128,
	WPA_TKIP,
	WPA2_TKIP,
	WPA2_AES
};

struct INIT_MASK_LINKED_LIST
{
	void*  (*backup_func)(void);		// 백업 함수
	void   (*restore_func)(void*);		// 복원 함수
	void*  malloc_p; 					// 백업 함수에서 동적할당했을 때의 포인터
	struct INIT_MASK_LINKED_LIST* next;
};

struct INIT_MASK_LINKED_LIST_META
{
	struct INIT_MASK_LINKED_LIST* curr;
	struct INIT_MASK_LINKED_LIST* search;
};

enum INIT_MASK_BACKUP_RESTORE
{
	INIT_MASK_BACKUP,
	INIT_MASK_RESTORE,
};

struct INIT_MASK_BACKUP_RESTORE_FUNC
{
	void* (*backup_func)(void);	
	void (*restore_func)(void*);	
};

extern void init_mask(const enum INIT_MASK_BACKUP_RESTORE);

extern void init_mask_m1891_settings_func_enroll(void);
extern void init_mask_printer_hardware_func_enroll(void);
extern void init_mask_network_func_enroll(void);
extern void init_mask_label_format_func_enroll(void);

extern INT8U init_check_m1891(struct M1891_SETTINGS *menu_settings);

// Menu 1891 설정 값 초기화 방어 백업 및 복원
void* backup_m1891(void);
void restore_m1891(void *);

// 프린터 관련 초기화 방어 백업 및 복원
void* backup_m1731(void);
void restore_m1731(void *);
void* backup_m1732(void);
void restore_m1732(void *);
void* backup_m1733(void);
void restore_m1733(void *);
void* backup_m1734(void);
void restore_m1734(void *);
void* backup_m1735(void);
void restore_m1735(void *);
void* backup_m1736(void);
void restore_m1736(void *);
void* backup_m1737(void);
void restore_m1737(void *);
void* backup_m1738(void);
void restore_m1738(void *);
void* backup_m1810(void);
void restore_m1810(void *);
void* backup_printer_area(void);
void restore_printer_area(void *);

// 네트워크 관련 초기화 방어 백업 및 복원
void* backup_parameter_731(void);
void restore_parameter_731(void *);
void* backup_m1911(void);
void restore_m1911(void *);
void* backup_m1912(void);
void restore_m1912(void *);
void* backup_m1913(void);
void restore_m1913(void *);
void* backup_m1914(void);
void restore_m1914(void *);
void* backup_m1915(void);
void restore_m1915(void *);
void* backup_m1916(void);
void restore_m1916(void *);

// 라벨 관련 초기화 방어 백업 및 복원
void* backup_m1510(void);
void restore_m1510(void *);

void init_mask_put(struct INIT_MASK_BACKUP_RESTORE_FUNC*);
void init_mask_set_search_first(void);
struct INIT_MASK_LINKED_LIST* init_mask_get_search_next(void);
void init_mask_restore(void);
void init_mask_free(void);

#define RETURN_IF_NULL(p, str, null) \
	do { \
		if (!p) { \
			Dsp_Write_String(point(0, 0),(HUGEDATA char*)str); \
			Delay_10ms(100); \
			return null; \
		} \
	} while(0);

#endif