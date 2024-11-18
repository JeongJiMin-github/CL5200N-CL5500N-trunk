#ifndef _PRT_ADVE_H_1
#define _PRT_ADVE_H_1

#endif

  
#ifndef _PRT_ADVE_H_2
#define _PRT_ADVE_H_2

#if defined(_USE_LCD37D_40DOT_) || defined(_USE_LCD26d_16x96_)
#define	MAX_SIDE_CHAR		12
#else
#define	MAX_SIDE_CHAR		17
#endif


//PLU_SCROLL.onoff
#define PLU_SCROLL_OFF		0
#define PLU_SCROLL_ON		1

//PLU_SCROLL.state
#define PLU_SCROLL_TURNOFF					0
#define PLU_SCROLL_TURNON					1
#define PLU_SCROLL_START					2
#define PLU_SCROLL_SCROLL					3

typedef struct {
	INT16S	data_leng;		// PLU Name Length
	INT16U	speed_timer;	// Scroll Speed
	INT16S	pt;				// Display Point
	INT8U	next;			// Scroll complete and reset
	INT8U	onoff;			// Scroll PLU on/off by key
	INT8U	state;			// 0 : no need PLU scroll, 1: Need PLU scroll, 2: PLU Scroll
//	INT8U	startflag;
//	INT8U	secondflag;
	INT8U	delay;	
	INT8U	msg[40+MAX_SIDE_CHAR*2+1+12]; // Scroll중 마지막 문자 남지 않도록 PLU Name 뒤에 0x20 12개 추가
} PLU_SCROLL_STRUCT;
#ifdef USE_PLU_SCROLLING
extern void scroll_PLU_init(void);
#endif


extern 	void label_message_create(void);		// command code  0x1251
extern  void label_message_delete(void);		// command code  0x1253
extern  void label_message_list(void);		// command code  0x1252
extern  void scroll_message_config(void);
extern  void scroll_message_input(void);
extern  void scroll_message(void);
extern  void scroll_message_start(INT8U v);
extern  void scroll_message_list(void);
extern  INT8U scroll_message_check_number(INT16U scr_no);
#endif 



