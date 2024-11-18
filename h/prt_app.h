/*****************************************************************************
|*  Copyright		:	(c) 2003 CAS
|*  Version		:	0.1
|*  Modified		:	2004/08/20
****************************************************************************/
// Printer Mode(Menu 1731)
#define PRINT_MODE_LABEL				0
#define PRINT_MODE_TICKET				1
#define PRINT_MODE_CONTINUOUS_LABEL		2

extern void PrintListWaitEnd(void);
extern void printer_hardware_setting(void);

extern void printer_sensor(void);
extern void printer_stb_time(void);
extern void printer_labelset(void);
extern void printer_receiptset(void);
extern void adjust_feed_length(void);
extern void adjust_reverse_feed_length(void);
extern void printer_mode(void);
extern void printer_modeset(INT8U mode);
extern void printer_modeset_default(void);
extern void printer_save_sensor(INT8U paperType, INT8U peelEnable, INT8U rewindEnable, INT8U gapMode);
extern void printer_save_size(INT8U paperType, INT16U width, INT16U height, INT16U endMargin);
extern void printer_settings(void);
extern void printer_chess(void);
extern void printer_test(void);
extern void printer_sensor_test(void);
extern void printer_preprint_setting(void);
extern void PrtChangeMode(INT8U mode);
extern INT8U PrtAutoChangeMode(void);
extern void prt_set_outofpaper(void);
extern void PrintListWaitEndWithoutComm(void);

extern INT8U PrtPaperType;
extern INT8U flagTempAllowSale;