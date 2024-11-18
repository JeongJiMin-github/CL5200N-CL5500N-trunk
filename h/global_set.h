#ifndef	_GLOBAL_SET_H
#define _GLOBAL_SET_H
//modeIndiv range : 0 ~ 7
	//0: default. 사용안함
	//1: 신세계
	//2: 농협
	//3: GS Mart
	//4: Standard (신세계 + 개체인덱스)
	//5: 이마트
enum {
    MODE_INDIV_DEFAULT,
    MODE_INDIV_SHINSEG,
    MODE_INDIV_NH,
    MODE_INDIV_GSMART,
    MODE_INDIV_STD,
    MODE_INDIV_EMART,
};
extern void global_display_set_4digit(INT8U pd, INT16U country);
extern void global_display_set_3digit(void);
extern void global_set_number(void);
extern void global_net_init(void);
extern void global_set_date(void);
// void global_factory_init(void);
extern void global_set_default_parameter(INT8U pd, INT8U flagKeepDefaultCaption);
extern void global_load_parameter(INT8U inhibit);
extern void global_set_country(INT16U country, INT8U flagKeepDefaultCaption);
extern void global_set_multi_unit(void);
extern void global_set_default_code000(void);
extern void global_set_pluinhibit(INT8U x, INT8U y); //se-hyung 070802
extern INT8U get_weight_decimal(INT8U capaid,INT8U multi,INT8U unit);
extern void change_weight_decimal(INT8U dec);
extern void save_weight_decimal(INT8U dec);
extern void set_plu_inhibit_default(void);
extern void set_clerk_init(void);
#endif
