
//#define ADM_DIRECT_BACKUP


#define AD_CAPA_6KG		0
#define AD_CAPA_15KG	1
#define AD_CAPA_30KG	2

#define AD_CAPA_15LB	3
#define AD_CAPA_30LB	4
#define AD_CAPA_60LB	5


#define AD_FULL_TARE	0
#define AD_HALF_TARE	1
#define AD_PROPER_TARE	2
#define AD_CUSTOM_TARE	3

//DELextern INT8U  ad_check_tareflag(void);
extern INT8U  ad_set_zero(void);
extern INT8U  ad_set_presettare(long v);
extern INT8U  adm_set_weighttare(long *v);

extern INT8U  adm_get_unit_weigh(void);	// 0:kg(g), 1:lb
extern void adm_set_unit_weigh(INT8U unit);
extern INT8U  adm_set_unit_cur(INT8U mode);
extern INT8U adm_get_unit_cur(void);
//extern INT8U  adm_get_kglb(INT8U *mode);
extern INT8U  adm_get_unit_cal(void);
extern INT8U adm_get_capa_id(void);

extern INT8U  ad_get_interval(INT8U range_pt);
extern INT8U  ad_get_unit_cal(void);
extern INT8U  ad_get_unit_weigh(void);
//extern INT8U  ad_get_unit_cur(void);
extern INT8U  ad_get_kglb(void);
extern INT32U ad_get_capa(void);
extern INT32U ad_get_max_capa(INT8U adm_capa, INT8U unit, INT8U is_mid_pt);
extern INT8U  ad_check_tarelimit(long val);
extern INT8U  ad_check_tarevalue(long v);
extern INT8U  ad_check_weight_value(long val);
extern INT8U  ad_get_multi_interval(void);
extern INT8U  ad_get_capa_idx(void);
extern INT32S ad_cnv_main2adm(INT32S val);
extern INT32S ad_cnv_adm2main(INT32S val);
extern INT32S ad_cnv_to_internal(INT32S ext_val);
extern INT32S ad_cnv_to_external(INT32S int_val);
extern INT8U  ad_get_accumul_tare(void);
extern void   ad_set_accumul_tare(INT8U val);
extern INT8U  ad_get_subtract_tare(void);
extern void   ad_set_subtract_tare(INT8U val);
extern void   ad_set_weight_default(INT8U mode, INT8U tarelimit);
extern long   ad_cnv_weight_value(long val, INT8U round_type);
extern void   ad_set_default(INT8U country);
extern INT32U ad_select_tarelimit(INT8U tarelimit_mode, INT32U tarelimit_value, INT8U wunit);
extern void   ad_set_tarelimit_default(INT8U tarelimit_mode, INT32U tarelimit_value, INT8U wunit);
extern INT8U adm_save_tarelimit(INT32U val, INT8U wunit);
extern long ad_cnv_weight_unit(long val, INT8U unit1, INT8U unit2);
extern INT32U get_ad_lparam(INT32U addr);
extern void adm_set_filter(INT8U scale_type, INT8U level);
extern INT32S ad_cnv_weight_by_gram(INT32S weight);
extern INT16U get_ad_wparam(INT32U addr);
#ifdef USE_SRILANKA_ZERO
extern INT8U ad_get_rezero_range(void);
extern INT8U ad_check_rezero(INT32S weight);
#endif


extern INT32U ad_max_capa[];
extern INT8U  ad_interval[];
extern INT8S div_fact_kg[2][10];
extern INT8S div_fact_lb[2][10];
extern INT8S div_fact_g[2][10];
extern INT8U decimal_kg[2][10];
extern INT8U decimal_lb[2][10];
