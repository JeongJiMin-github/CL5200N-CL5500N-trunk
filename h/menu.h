/*****************************************************************************
|*			
|*  Copyright		:	(c) 2002 CAS
|*  Filename		:	menu.h
|*  Version			:	0.1
|*  Programmer(s)	:	Chung jong kwon (CJK)
|*  Created			:	2003/09/16
|*  Description		:	CLP menu header
|*				
****************************************************************************/
//extern INT16S NUM_MENU_PER_PAGE;

#ifndef _MENU_H_1
#define _MENU_H_1

// <CODE> JHC, 040514, Modify, -30 to 35
// [
#ifdef USE_EU_NUTRITIONFACT
#define MAX_MENU_INDEX 50
#else
#define MAX_MENU_INDEX 40
#endif
#define MAX_MENU_KEY_FUNCTION 3
// ]

#define MENU_TYPE_NUM			0
#define MENU_TYPE_STR			1
#define MENU_TYPE_NUM_FILL_0	2
#define MENU_TYPE_IP			3
#define MENU_TYPE_YN			4
#define MENU_TYPE_FIX			5
#define MENU_TYPE_HEX			6
#define MENU_TYPE_BAR			7
#define MENU_TYPE_STRL			8
#define MENU_TYPE_DISABLE		9
#define MENU_TYPE_NUM_1BYTE		10
#define MENU_TYPE_NUM_2BYTE		11
#define MENU_TYPE_FORM			12
#define MENU_TYPE_DATE			13
#define MENU_TYPE_TIME			14
#define MENU_TYPE_WEIGHT		15
#define MENU_TYPE_DATENUM		16
#define MENU_TYPE_PASSWORD		17


// keyin mode
#define KI_WEIGHT	0
#define	KI_UNIT		1
#define	KI_TOTAL 	2
#define KI_GRAPHIC	3
#define KI_TARE		4
#define KI_SEGMENT	5

#define KI_DEFAULT  KI_TOTAL

#define CHAR_HAN_MID	0
#define CHAR_HAN_START	1
#define CHAR_ENG_START	2

#define KI_FORM_MODE		0
#define KI_FREE_MODE		1
#define KI_IP_MODE			2
#define KI_BARCODE_FORM		3		//  04.03.08  by getwing   
#define KI_SNUMBER			4

#define KI_ARAB_ENG_MODE	0 	//Arab Based(English Compatible)
#define KI_ENG_ARAB_MODE	1	//English Based(Arab Compatible)
#define KI_IP_INPUT_MODE	2	//IP Address Input Mode

#define KI_F_USE_INIT_VAL	0x01
#define KI_F_LEFT_ALIGN		0x00
#define KI_F_RIGHT_ALIGN	0x04
#define KI_F_USE_DECIMAL	0x20
#define KI_F_USE_UNIT1000	0x40

#define KI_FORM_IRDN	(KI_F_USE_INIT_VAL + KI_F_RIGHT_ALIGN + KI_F_USE_DECIMAL)		//0x25//
#define KI_FORM_IRNU	(KI_F_USE_INIT_VAL + KI_F_RIGHT_ALIGN + KI_F_USE_UNIT1000)		//0x45//
#define KI_FORM_IRDU	(KI_F_USE_INIT_VAL + KI_F_RIGHT_ALIGN + KI_F_USE_DECIMAL + KI_F_USE_UNIT1000)	//0x65//
#define KI_FORM_IRNN	(KI_F_USE_INIT_VAL + KI_F_RIGHT_ALIGN)							//0x01//
#define KI_FORM_ILNN	(KI_F_USE_INIT_VAL + KI_F_LEFT_ALIGN)							//0x05//

#define KI_FORM_NORMAL	 	KI_FORM_IRDN
#define KI_FORM_UNIT1000	KI_FORM_IRDU

#define KI_NOPASS	0
#define KI_PASS		1
#define KI_NOEDIT	2

#define MENU_RET_SAVE	0//KP_SAVE
#define MENU_RET_UP		1//KP_ARROW_UP
#define MENU_RET_DN		2//KP_ARROW_DN
#define MENU_RET_EXIT	3//gm 090407 KP_TEST
#if defined(USE_ARAB_FONT) || defined(HEBREW_FONT)
#define MENU_RET_TEST	4
#define MENU_RET_FEED	5
#define MENU_RET_ZERO	6
#endif
#define MENU_RET_ESC	0xff//KP_ESC

#define KI_STR_BUF_SIZE 	200
#define KI_DSP_BUF_SIZE 	50

#define MENU_MOVE_TO_UP		0
#define MENU_MOVE_TO_ROOT	1

#define DISP_TITLE_START_POSITION	2
#define DISP_TITLE_LENGTH			26

// Properties
#define	MENU_P_USE_LINEFEED	0x0001
#define MENU_P_DSP_POSITION	0x0002
#define MENU_P_PAGEUPDN_KEY	0x0004
#define MENU_P_CHECK_BLANKS	0x0008
#define MENU_P_EXIT_MENU	0x0010 //gm 090407, Menu_AddKeyFunction()에 등록된 키를 입력하면 메뉴입력 중 빠져나옴
#define MENU_P_EXCLUDE_NULL	0x0020
#define MENU_P_ENGLISH_MODE	0x0040
#define MENU_P_CHINA_PINY	0x0080

#pragma pack(4)
typedef struct {
	char str[50];
	INT8U id;
	INT8U type;
	INT32U max;
	INT32U min;
	HUGEDATA INT32U *pval;
	HUGEDATA INT8U *pstr;
	INT8U form[50];
	INT16U length;
	INT16U w_length;
	INT8U start;
	INT8U decimal;
	void  (*function)();
	INT8U inhibit;
	INT8U disable;
	void  (*end_function)();
	INT16U callback_key[MAX_MENU_KEY_FUNCTION];
	void (*key_function[MAX_MENU_KEY_FUNCTION])();
	INT8U clerk_one_touch;
	INT16U properties;
	INT8U page_end;
	INT8U symbol;
	INT8U str_adjustable;
	INT8U callback_mode;
	INT8U general_flag;	// 범용 플래그, BarType, DateType...
	INT8U (*check_function)();
	INT8U key_mode;	// key_set : 0:Sale, 1:Pgm, 2:Sale+Pgm...
	INT8U char_fixed;
} MENU_STR;

typedef struct {
	INT16U size;
	INT16U start;
	INT16U dsp_size;
} MENU_WINDOW_STR;
#pragma pack()
extern INT8U MenuIndex;//, MenuInhibitCnt;
extern INT8U MenuRefresh;
extern INT8U MenuCapAdjust;
extern INT8U MenuReorder;
extern INT8U MenuDateType;
extern INT8U MenuNumLinePerPage;
extern INT8U MenuDefaultCallbackMode;
extern MENU_STR Menu[MAX_MENU_INDEX];
extern INT8U Menu_order_table[MAX_MENU_INDEX];
extern INT16U CurMenuCode;
extern INT16U CallbackEditPosition_x;
extern INT16U CallbackEditPosition_y;
extern INT8U CallbackMenuId;
extern INT8U Menu_return_last_item_arrow_dn;
extern INT8U Menu_return_first_item_arrow_up;

extern INT8U Scrl_Msg_UseLogo;

extern void Menu_Init(void);
extern void Menu_Init_WithPage(INT8U start_page);
extern void Menu_HeadCreate(HUGEDATA char *str);
//extern void Menu_TreeCreate(INT8U id, HUGEDATA char *str);
//extern INT8U Menu_TreeDisplay(HUGEDATA INT8U *page);
extern void Menu_InputCreate(INT8U id, HUGEDATA char *str, INT8U type, INT8U start, INT16U w_length, 
                               INT16U length, INT32U max, INT32U min, INT8U decimal, HUGEDATA INT32U *pval, 
                               HUGEDATA INT8U *pstr, HUGEDATA INT8U *form,void (*function)(long));
extern INT8U Menu_GetMenuId(INT8U id);
extern INT8U Menu_AddEndFunction(INT8U id, void(*function)());
extern INT8U Menu_AddCheckFunction(INT8U id, INT8U (*function)());
extern INT8U Menu_AddKeyFunction(INT8U id, INT16U key, void (*function)());
extern INT8U Menu_SetProperties(INT8U id, INT16U properties);
extern void Menu_SetNextPage(void);
extern INT8U Menu_SetDisable(INT8U id, INT8U onoff);
extern INT8U Menu_SetOneTouch(INT8U id, INT8U onoff);
extern INT8U Menu_SetInhibit(INT8U id, INT8U onoff);
extern INT8U Menu_SetSymbol(INT8U id, INT8U symbol_id);
extern INT8U Menu_SetStrAdjustable(INT8U id, INT8U onoff);
extern INT8U Menu_SetCallbackMode(INT8U id, INT8U mode);
extern INT8U Menu_SetGeneralFlag(INT8U id, INT8U mode);
extern INT8U Menu_SetKeyMode(INT8U id, INT8U mode);
extern INT8U Menu_SetFixedCharacter(INT8U id, INT8U character);
extern INT8U Menu_InputDisplay(void);

extern void Menu_ModifyRetVal(INT8U id, INT32U val);
extern void Menu_ModifyRetStr(INT8U id, HUGEDATA INT8U* str, INT8U size);

//extern INT16U get_center_position(char *str);

extern void temp_addr_init(void);
extern INT32U temp_addr_set(INT16U id, INT16U size);
extern INT32U temp_addr_get(INT16U id);
extern void menu_display_menucode(INT16U code,INT8U set);
extern void	menu_displaytitle(INT16U code,INT8U mode);
extern void menu_writetitle_str(char *form_string);
// extern void menu_writetitle(INT16U code);
extern void menu_writepage(INT8U totalpage, INT8U curpage);

extern void VertScrollMsg_Init(INT8U fontId);
//extern void VertScrollMsg_Add(HUGEDATA char *str);
extern void VertScrollMsg_Add(const char *str);
extern INT8U Menu_ConfirmDisplay(INT16U captionCode);

#endif