#ifndef CAPTION_POINT_1
#define CAPTION_POINT_1
#define CAP_MSG_LENGTH	60
#define CAP_FORM_LENGTH	34

#define CAPTION_CODE_POINT 			0
#define CAPTION_NAME_POINT 			2
#define CAPTION_INPUTTYPE_POINT		36
#define CAPTION_SAVEADDRESS_POINT	38 //
#define CAPTION_RUNFLAG				38
#define CAPTION_RESERVED			40
#define CAPTION_MENUSTART_POINT 	42
#define CAPTION_NITEM_POINT 		44
#define CAPTION_LENG_POINT  		46
#define CAPTION_DLENG_POINT 		48
#define CAPTION_ITYPE_POINT 		50
#define CAPTION_X_POINT				51
#define CAPTION_XMIN_POINT  		52
#define CAPTION_XMAX_POINT  		56

#define CAPTION_STR_LENGTH		(CAPTION_INPUTTYPE_POINT - CAPTION_NAME_POINT)
#ifdef USE_EXTEND_CAPNTION_AREA
#define CAPTION_EXT_CODE_START_POINT      0xFF00
#endif
#define CAPNTION_CODE_END_POINT           0xFFFF

#endif

#ifndef CAPTION_STRUCT_1
#define CAPTION_STRUCT_1
#pragma pack(2)
typedef struct {
	INT16U code;			//  2	0
//	INT8U  form[34];		// 34	2
	char  form[CAP_FORM_LENGTH];		// 34	2
	INT16U inputtype;		//  2	36
	INT16U runflag;
	INT16U reserved;		//  4	38
	INT16U submenu_id;		//  2	42
	INT16U submenu_n;		//  2	44
	INT16U input_length;		//  2	46
	INT16U input_dlength;		//  2	48
	INT8U  input_itype;		//  1   50
	INT8U  input_x;			//  1	51
	INT32U input_min;		//  4	52
	INT32U input_max;		//  4	56
} CAPTION_STRUCT;
#pragma pack()
#endif

// extern void caption_init(void);
//extern void caption_save(INT16U id,INT16U code,char *form,INT16U inputtype,INT32U saveaddr,
//	INT16U menustart,INT16U nitem,INT8U x,INT16U leng,INT8U itype,INT32U xmin,INT32U xmax);
extern void caption_error(INT8U mode);
//void caption_save(INT16U id,INT16U code,char *form,INT16U inputtype,INT32U saveaddr,
//	INT16U menustart,INT16U nitem,INT8U x,INT16U leng,INT16U dleng,INT8U itype,INT32U xmin,INT32U xmax);
//extern void caption_get_id(INT16U id,char *str);
//extern void caption_get_code(INT16U code,char *str);
extern INT32U caption_search_by_code(INT16U code);
extern INT8U caption_split_by_code(INT16U code, CAPTION_STRUCT *caption,INT8U mode);
extern INT32U plu_get_display_info(INT16U id, CAPTION_STRUCT *cap,INT8U x);
//extern INT16U caption_id_search_by_code(INT16U code);
//DEL20060309extern void caption_erase(void);
extern INT8U caption_get_name(INT16U code,char *str);
extern void  caption_adjust(CAPTION_STRUCT *cap,char *format);
extern void  caption_adjust_ref(CAPTION_STRUCT *cap,char *format,INT16U ref_codee);
extern INT8U   caption_run_check(INT16U code,INT16U runflag);


extern INT8U FlagCapAdjust_RightAlign;
extern INT8U caption_continuous_search;
extern INT32U caption_continuous_search_addr;
