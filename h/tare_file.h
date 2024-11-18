#ifndef _TARE_FILE_H_1
#define _TARE_FILE_H_1
#pragma pack(2)
typedef struct	{
	INT16U	_code;
	//INT8U	_description[17];
	INT8U	_description[16];
	INT8U	_weightunit;	// Added by CJK 20060404
	INT8U	_type;
	INT32U	_value;	
} TARE_STRUCT;
#pragma pack(2)
#endif

  
#ifndef _TARE_FILE_H_2
#define _TARE_FILE_H_2

extern  void    tarefile_delete(void);		// command code  0x1282
extern  void 	tarefile_list(void);		// command code  0x1283
extern  void	tarefile_create(void);
extern  void    taretype_delete(INT16U code, INT16U cap_code, INT32U _MENU_AREA, INT16U str_size);
extern  void 	taretype_list(INT16U code, INT16U cap_code, INT32U _MENU_AREA, INT16U str_size);
extern  void    tare_key_define(void);
extern  INT16U weighing_tare_input(INT8U dsp_mode, INT8U decimal, long *ret_val, INT8U leng, INT8U y, INT16U x);

#endif 



