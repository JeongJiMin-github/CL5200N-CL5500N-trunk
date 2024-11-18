#ifndef  _PGM_APP_H_
#define  _PGM_APP_H_

extern  void display_title_num_page(INT16U cap_code,INT8U xmode, INT32U _number, INT8U current_page, INT8U total_page);
extern  void display_pgm_list(INT16U title_cap, INT16U boundary_cap, INT16U *input_cap, INT16U list_cap, INT32U _menu_area, INT16U str_size);
extern  void plucall_department(long num);
extern  void plucall_plutype(long num);
extern  void plucall_unitweight(long num);
extern  void plucall_taxcode(long num);
extern  void plucall_group(long num);
extern  void plucall_tareid(long num);
extern  void plucall_pcscode(long num);
extern  void plucall_ingredient(long num);
extern  void plucall_discount(long num);
extern  void plucall_traceability(long num);
extern  void plucall_origin(long num);
extern  void plucall_nutrifact(long num);
extern  void plucall_barcodefile(long num);
extern  void plucall_label_message(long num);
extern  void plucall_label_id(long num);		// Added by CJK
extern  void plucall_country(long num);
extern  void plucall_slaughter(long num);
extern  void plucall_cutting(long num);
#ifdef USE_AUSTRALIA_COOL_FUNCTION
extern  void plucall_CoOL_type(long num);
#endif
extern  void callback_barcode(long num);

extern  void display_title_page_change(INT16U cap_code, INT8U current_page, INT8U total_page);
extern  void parameter_title_page_change(INT16U cap_code, INT8U current_page, INT8U total_page);
extern  void error_display_form1(INT8U number, INT32U min, INT32U max);
extern  void error_display_savekey(void);
extern  void error_display_caption(INT16U code);
extern  void keycall_key_number(long num);
extern  void plucall_plu_number(long num);
extern  void plucall_plu_dcnumber(long num);
extern  INT16U search_pgm_next_id(INT16U code, INT16U str_size, INT32U flash_addr);
extern BOOLEAN	input_range_check(long input_value, long min, long max);
extern  BOOLEAN input_time_check_str(char *buffer,INT8U mode);
extern  INT8U  rtc_get_date_format(void);
extern void input_convert_time(INT8U *str, INT8U *hour, INT8U *min, INT8U *sec);
extern void input_convert_date(INT8U day_type, INT8U *str, INT8U *y, INT8U *m, INT8U *d);
BOOLEAN input_day_check(INT8U year, INT8U month, INT8U day);
extern BOOLEAN input_day_check_str(INT8U day_type, char *buffer, INT8U mode);
#if defined (USE_RUSSIA_LOGISTICS_PLU) || defined (USE_EUR_LOGISTICS_PLU)
extern  void plucall_gtintype(long num);
#endif
extern INT16S	CallbackPluEditPosition_x;
extern INT16S	CallbackPluEditPosition_y;
#endif