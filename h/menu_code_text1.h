#ifndef _MENU_CODE_TEXT1_1
#define _MENU_CODE_TEXT1_1
#pragma pack(2)
typedef struct  {

		INT16U	code;
#ifdef USE_VIETNAM_LOTTEMART_DEFAULT
		INT8U	text[202];
#else
		INT8U	text[62];
#endif
} MENU_TYPE1_STRUCT;	
#pragma pack()

#endif
  
#ifndef _MENU_CODE_TEXT1_2
#define _MENU_CODE_TEXT1_2


extern long    menu_addr;
extern INT16S     menu_field_size;
extern INT16U  menu_capid1,menu_capid2;
extern INT16S     menu_call_y;
extern INT16S     menu_field_start;
extern INT8U   menu_readmode;

// extern  void	menu1_create(INT16U code, INT16U cap_code, INT32U _MENU_AREA, INT16U str_size); 
//
extern void	menu1_create(INT16U code, INT16U *cap_code, INT32U _MENU_AREA, INT16U str_size, INT8U use_LF);
extern void menu1_delete(INT16U code, INT16U cap_code, INT32U _MENU_AREA, INT16U str_size);
extern void menu_callback_init(INT16U c1,INT16U c2,INT16S y,INT32U addr,INT16S fsize, INT16S fstart);
extern void menu_callback(long v);

#endif 



