#ifndef CURRENCY_STR_DEF
#define CURRENCY_STR_DEF
#pragma pack(2)
typedef struct {
	INT32U rate;
	INT8U  ratepoint;
	INT8U  primary[4];
	INT8U  last[4];
	INT8U  decimalpoint;
	INT8U  prtform;
} CURRENCY_STRUCT;
#pragma pack()
#endif

extern void currency_create(void);
extern void currency_delete(void);
extern void currency_list(void);
