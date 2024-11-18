extern INT8U barcodefile_print_list(INT16U start_id, INT16U end_id);
extern INT8U clerk_print_list(INT16U start_id, INT16U end_id);
extern INT8U customer_print_list(INT16U start_id, INT16U end_id);
extern INT8U department_print_list(INT16U start_id, INT16U end_id);
extern INT8U discount_print_list(INT16S dept_no, long start_id, long end_id, INT16U *data_pos, INT16U num);
#ifdef USE_DIRECT_NUTRITION
extern INT8U nutri_print_list(INT16U start_id, INT16U end_id, INT8U dept_id);
#else
extern INT8U nutri_print_list(INT16U start_id, INT16U end_id);
#endif
extern INT8U origin_print_list(INT16U start_id, INT16U end_id);
extern INT8U table_print_list(INT16U title_cap, INT32U base_addr, INT16U str_size, INT16U start_id, INT16U end_id);
extern INT8U ingredient_print_list(INT16U start_id, INT16U end_id);
extern INT8U store_name_print_list(INT16U start_id, INT16U end_id);
extern INT8U tarefile_print_list(INT16U start_id, INT16U end_id);
extern INT8U tax_print_list(INT16U start_id, INT16U end_id);
extern INT8U traceability_print_list(INT16U start_id, INT16U end_id);
extern INT8U workplace_table_print_list(INT16U title_cap, INT32U base_addr, INT16U str_size, INT16U start_id, INT16U end_id);
extern INT8U print_plu_list(INT16S dept_no, long start_id, long end_id);
extern INT8U print_plu_list_forlabel(INT16S dept_no, long start_id, long end_id);
extern INT8U print_plu_contents(INT16S plu_no, INT16S dept_no, INT16S *enable_list, INT16S max_id);
extern INT8U history_print_list(void);
extern INT8U parameter_print_list(INT8U mode);
extern INT8U currency_print_list(INT16U start_id, INT16U end_id);
extern INT8U scanner_convtable_print_list(INT16U start_id, INT16U end_id);
#ifdef PRINT_AD_SETTINGS
extern INT8U adm_env_print_list(void);
#endif
extern INT8U scroll_print_list(INT16U start_id, INT16U end_id);

#ifdef USE_TRACE_STANDALONE
extern INT8U individualData_print_list(INT8U start_id, INT8U end_id);
#endif