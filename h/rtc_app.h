extern INT32U rtc_getdate(char *str);
extern INT32U rtc_gettime(char *str);
extern INT32U rtc_gettime_sub(INT8U mode,char *str);
extern INT32U rtc_getdate_sub(INT8U yflag,INT8U mflag,INT8U mode,char *str);


extern INT32U rtc_makedate_str(char *str, INT8U year, INT8U month, INT8U date);	// non-bcd format
extern INT32U rtc_maketime_str(char *str, INT8U hour, INT8U min, INT8U sec);	// non-bcd format

#ifdef USE_MORNITORING_CNT
extern void rtc_cnt_scale_operated_time(void);
extern void rtc_set_pre_minute(void);
extern INT8U rtc_get_pre_minute(void);
extern void rtc_save_operated_time(void);
extern void rtc_load_operated_time(void);
extern void rtc_clear_operated_time(INT32U InitDate);
#endif//#ifdef USE_MORNITORING_CNT