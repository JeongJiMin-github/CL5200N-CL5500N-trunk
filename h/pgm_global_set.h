#ifndef PGM_GLOBAL_SET
#define PGM_GLOBAL_SET

extern INT8U global_function(INT16U fcode);
// JHC 040325 <ADD>
// note : Parameta setting menu
// [
extern INT8U global_printingoperation(INT16U fcode);
// extern INT8U global_salesetting(INT16U fcode);
extern INT8U global_logsetting(INT16U fcode);
// ]
extern void	globla_set_function_input(void);
extern void global_set_label(void);

#ifdef USE_GSMART_PLU
extern void global_set_PLU_default(void);
extern void global_PLU_default_init(void);
#endif

extern void global_set_discount(void);
extern void global_set_tax(void);
// extern void	global_set_store_name(void);	// 0x1550
extern void global_set_label_mess(void);
extern void global_set_barcode(void);
extern void global_set_plu_data_change(void);
extern void global_set_plu_date_format(void);
// extern void global_set_change_salemode(void);
extern void global_set_round(void);
extern void global_set_disc_priority(void);
extern void global_set_disc_weight(void);
extern void global_set_disc_pcs(void);
extern void global_set_disc_count(void);
#ifdef USE_FIXED_PERCENT_KEY_SET
extern void global_set_disc_key(void);
#endif
extern void global_set_initial_condition(void);
extern void global_set_auto_print_threshold(void);
extern void global_set_auto_mode_condition(void);
extern void global_set_key_condition(void);
extern void global_set_autoplu_time(void);
//extern void global_set_clerk_log_out_time(void);
extern void global_set_weigh_mode_condition(void);
extern void global_set_count_mode_condition(void);
extern void global_set_nonfood_mode_condition(void); 
extern void global_set_start_save_mode(void);
extern void global_set_print_price_zeor(void);
extern void global_set_save_type_change(void);
//DEL070131extern void global_set_save_unit_price_change(void);
extern void global_set_display_external_tax(void);
extern void global_set_view_discount_infomation(void);
// extern void global_set_clerk_transaction_logoff(void);
//extern void global_set_clerk_floating_labelon(void);
//extern void global_set_addup_labelprint(void);
extern void global_set_check_peel_off(void);
//extern void global_set_clerk_print_condition(void);
extern void global_set_clerk_print_ticket_item(void);
extern void global_set_clerk_print_list_item(void);
extern void global_set_clerk_print_font_size(void);
extern void global_set_addup_print_condition(void);
//extern void global_set_rotate_label(void);
extern void global_set_report(void);
extern void global_set_continuance_sale(void);
extern void global_set_tare_condition(void);
// extern void global_set_system_password(void);
extern void global_set_cashdraw(void);
extern void global_set_time_sign(void);
extern void global_set_month_sign(void);
extern void global_set_week_sign(void);
extern void global_set_currency_sign(void);
//DEL CJK070321extern void global_set_callbyscanner(void);
extern void global_set_indicator_sign(void);
extern void global_set_weightsymbol1(void);
extern void global_set_weightsymbol2(void);
// extern void global_set_serviceflag(void);

extern void global_set_salesave(void);
extern void global_set_salesetup(void);
extern void global_set_salemode(void);
// extern void global_set_saleopermode(void);
extern void global_set_label_menu(void);
extern void print_markdown_set(void);
extern void plu_test_label_flag(void);
extern void global_application_set(void);

#endif
 