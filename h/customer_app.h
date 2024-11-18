/*****************************************************************************
|*			
|*  Copyright		:	(c) 2002 CAS
|*  Filename		:	customer_app.h
|*  Version			:	0.1
|*  Programmer(s)	:	Chung jong kwon (CJK)
|*  Created			:	
|*  Description		:	
|*				
****************************************************************************/


#ifndef _CUSTOMER_APP_H
#define _CUSTOMER_APP_H

#define CUSTOMER_STRUCT_SIZE	220
#pragma pack(2)
typedef struct {
	INT16U code;
//	INT8U  id[10];
	INT8U  name[32];
	INT8U  address[3][50];
	INT8U  tel[24];
	INT32U creditlimit;
	INT8U createyear, createmonth, createdate;
	INT8U createhour, createmin, createsec;
	INT8U	reserved[2];
} CUSTOMER_STRUCT;
#pragma pack()
extern void customer_edit(void);
extern void customer_list(void);
extern INT16U customer_search_empty_code(void);
extern INT8U customer_print_list(INT16U start_id, INT16U end_id);
extern void customer_delete_proc(void);
#endif





