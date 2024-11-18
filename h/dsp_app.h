#ifndef DISPLAY_APPLICATION_1
#define DISPLAY_APPLICATION_1

#define DISPLAY_WEIGHT 0
#define DISPLAY_UNITPRICE   1
#define DISPLAY_PRICE  2
#define DISPLAY_TARE   3
#ifdef USE_SINGLE_LINE_GRAPHIC
#define DISPLAY_PLU   4
#endif
#define DISPLAY_UNDEFINED   9
#ifdef _USE_LCD_7_37D_40DOT_
#define DISPLAY_START_POS_TARE           0
#define DISPLAY_START_POS_WEIGHT         5
#define DISPLAY_START_POS_UNITPICE      11
#define DISPLAY_START_POS_PRICE         18
#define DISPLAY_START_POS_PLU           25
#define DISPLAY_PRICE_DIGIT				7
#elif defined(_USE_LCD_7_26d_16x96_)
#define DISPLAY_START_POS_TARE           0
#define DISPLAY_START_POS_WEIGHT         5
#define DISPLAY_START_POS_UNITPICE      11
#define DISPLAY_START_POS_PRICE         18
#define DISPLAY_START_POS_PLU           26
#define DISPLAY_PRICE_DIGIT				8
#endif

// Symbol Index (CL5200J, CL3000 공통)
// signTable[]에서의 position을 flash memory에 저장하지 않고, signTableIndex[] 배열에 고정주소로 저장.
// 변경 사유 : flash memory 저장이 불필요, 저장공간 26개로 한정됨.
#ifdef _USE_LCD_7_25_
	#define SIGN_STABLE    0
	#define SIGN_ZERO      1
	#define SIGN_TARE      2
	#define SIGN_SAVE      17  // 3
	#define SIGN_AUTO      16 // 4
	
	#define SIGN_PREPACK  23  // 5
	#define SIGN_DISCOUNT  24
	#define SIGN_SHIFT     25
	#define SIGN_MASTER    8
	#define SIGN_NET	   	22 //9  comm seg
	#define SIGN_LANG	  10
	
	#define SIGN_KG		  11
	#define SIGN_LB		  12
	#define SIGN_PER_KG	  13
	#define SIGN_PER_LB	  14
	#define SIGN_PER_G	  15
	
	//#define SIGN_ETH_LED_100   5    // icon
	//#define SIGN_ETH_LED_LINK  21  //triangle
	#define SIGN_ETH_LED_LINK  5  // icon
	
	#define SIGN_CONNECT  4  //16	// network server connection check
	#define SIGN_CHECKIN  3  //17

#ifdef USE_CTS_FUNCTION
	#define SIGN_CTS_ALARM	   19
#endif
	#define SIGN_USBMEM     21 // USB MEMORY

#ifdef USE_PB_BATTERY
	#define SIGN_BAT1	31	// 테두리
	#define SIGN_BAT2	32	// High
	#define SIGN_BAT3	33	// Low
#endif
#else	
#define SIGN_STABLE         0
#define SIGN_ZERO           1
#define SIGN_TARE           2
#define SIGN_SAVE           3
#define SIGN_AUTO           4

#define SIGN_PREPACK        5
#define SIGN_DISCOUNT       6
#define SIGN_SHIFT          7
#define SIGN_MASTER         8
#define SIGN_NET	    9
#define SIGN_LANG	   10

#define SIGN_KG		   11
#define SIGN_LB		   12
#define SIGN_PER_KG	   13
#define SIGN_PER_LB	   14
#define SIGN_PER_G	   15

#define SIGN_CONNECT       16	    // network server connection check
#define SIGN_CHECKIN       17
#define SIGN_ETH_LED_LINK  18
#if !defined(_USE_LCD20848_) && !defined(_USE_LCD32_)
#define SIGN_BAT1		20	// 테두리
#define SIGN_BAT2		21	// High
#define SIGN_BAT3		22	// Low
#define SIGN_ANTENA			23
#define SIGN_ANTENA_LOW		24
#define SIGN_ANTENA_MID		25
#define SIGN_ANTENA_HIGH	26
#ifdef USE_CTS_FUNCTION
#define SIGN_CTS_ALARM      27
#endif
#else
#ifdef USE_CTS_FUNCTION
#define SIGN_CTS_ALARM	   19
#endif
#define SIGN_USBMEM     20 // USB MEMORY
#endif
#endif // _USE_LCD_7_25_
// triangle 추가될 경우 signTableIndex[] 변경 필요

#define DSP_RESTORE	0
#define DSP_BACKUP	1
#endif

#ifndef DISPLAY_APPLICATION_2
#define DISPLAY_APPLICATION_2
#pragma pack(4)
typedef struct {
#ifdef USE_SINGLE_LINE_GRAPHIC
	INT8U mode_pos[5];
	INT8U mode_len[5];
	INT8U mode_dec[5];
	INT8U mode_fix[5];
#else
	INT8U mode_pos[4];
	INT8U mode_len[4];
	INT8U mode_dec[4];
	INT8U mode_fix[4];
#endif
} DISPLAY_STRUCT;
#pragma pack()
#endif
#ifndef DISPLAY_SCROLLMESSAGE_DEF
#define DISPLAY_SCROLLMESSAGE_DEF
#pragma pack(2)
typedef struct {
	INT8U  stype;			// 
	INT8U  week;			// 3
	INT8U  use;
	INT8U  smode;
//	INT8U  text[84];
/////////////////////	//Modify CJK080130
	INT8U  text[80];
	INT8U  image_id;
	INT8U  on_time;
	INT8U  blink_count;
	INT8U  font_size;
/////////////////////
	INT8U  start_year,start_month,start_day;
	INT8U  end_year,end_month,end_day;
	INT8U  start_hour,start_min,start_sec;
	INT8U  end_hour,end_min,end_sec;
} SCROLL_MESSAGE_STRUCT;
#pragma pack()
#endif

#ifndef DISPLAY_APPLICATION_3
#define DISPLAY_APPLICATION_3
extern void display_factory_init(void);
extern void init_lcdchess(void);
extern void display_init(void);
extern void display_set_weightunit_change(INT8U dec);
extern void display_net_init(void);
extern void display_use_allon_digit(INT8U ch);
extern void display_allon_primary_indication(void);
extern void display_alloff_primary_indication(void);
//DEL070618extern void display_use_allon(INT8U ch);
extern void display_ulongsimple(INT8U mode,long idata);
extern void display_ulong(INT8U mode,long idata,INT8U decimalpoint);
//DEL070619extern void display_ulong_any(long idata,INT8U spos,INT8U length,INT8U decimalpoint,INT8U sign);
extern void display_clear(INT8U mode);
extern void display_string(INT8U mode,INT8U *data);
extern void display_string_pos(INT8U pos, INT8U *data);
extern void display_string_pos_clear(INT8U pos, INT16S len);
extern void display_primary_diffuse(void);
//extern void display_string_diffuse(INT8U mode,INT8U *data);

//DEL070619extern void display_sign(INT8U sign,INT8U onoff);
extern void display_sign_noflush(INT8U sign,INT8U onoff);
extern void display_set_sign(INT8U sign,INT8U pos);
extern void display_set_3digit(void);
extern void display_set_4digit(void);
extern void display_set_number(void);
extern void display_tare(INT8U mode);
// extern void display_date(void);
extern void display_date_change(INT16S mode, INT16S saveFlag);
extern void display_lcd_clear(void);
extern void display_lcd_clear_buf(void);
extern void display_lcd_diffuse(void);
extern char display_font_get_width(void);
extern char display_font_get_height(void);
extern char display_font_get_width2(void);
extern char display_font_get_height2(void);
extern char display_font_get_width_id(INT8U fontid, INT8U mag);
extern char display_font_get_height_id(INT8U fontid, INT8U mag);
extern void display_save_restore(INT8U mode);
//DEL070618extern void display_override(INT16S xtype,INT16S rid,INT8U on);
//DEL070618extern void display_override_sub(INT16S xtype,INT16S rid,INT8U on);
extern void display_temp_setting(void);

extern INT8U get_decimal_pt(INT8U mode, INT16U temp);

extern void ClearDisp7(void);
extern INT16S DisplayMsg(char *msg);
//extern INT16S DisplayMsgRight(char *msg);
extern INT16S DisplayMsgShort(char *msg);
//extern INT16S DisplayMsgTransaction(char *msg);
extern void DisplayMsgReset(void);
//extern INT16S DisplayWeightMsg(char *msg);


extern INT16U MsgDisplayTime;
extern INT16U Msg7SegDispTime;
#if defined(_USE_LCD20848_) || defined (_USE_LCD32_)
extern INT16U MsgPLUDisplayTime;
extern INT16U MsgRDisplayTime;
extern INT8U  MsgWeight;
extern INT8S  MsgDisplayRightBuf[];
#endif
extern INT16U DateTimeDispTime;
extern INT8U  MsgShort;
extern INT8S  MsgDisplayBuf[];
extern INT8U  Disp_refresh_flag;

//extern DISPLAY_STRUCT display_parameter;
extern NEARDATA DISPLAY_STRUCT display_parameter;//HYP 20060412 _huge --> _near for small code size
#endif
