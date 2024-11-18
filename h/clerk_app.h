/*****************************************************************************
|*			
|*  Copyright		:	(c) 2002 CAS
|*  Filename		:	clerk_app.h
|*  Version			:	0.1
|*  Programmer(s)	:	Chung jong kwon (CJK)
|*  Created			:	
|*  Description		:	
****************************************************************************/


#ifndef CLERK_STRUCT_DEFINE
#define CLERK_STRUCT_DEFINE
#pragma pack(2)
typedef struct {
	INT16U code;			//2
	INT8U name[24];			//34
	INT8U nickname[8];		//38
	INT8U password[8];		//46
	INT8U permissionLevel;	//47
	INT8U permissionID;
	INT8U dept;				//48
	INT8U headid;
	INT8U shopid;
	INT8U training;   			   //insert by se-hyung for training key 20070627
//	INT8U reserve[80];
} CLERK_STRUCT;
#pragma pack()
#endif

#ifndef CLERK_PERMISSION_STRUCT_DEFINE
#define CLERK_PERMISSION_STRUCT_DEFINE
#pragma pack(2)
typedef struct {
	INT8U change_item;			// 1	 //	1
	INT8U change_price;		// 1		
	INT8U key_dept;			// 1	
	INT8U key_pay;				// 1	
	INT8U key_date_chage;		// 2
	INT8U key_type_chage;		// 2	// 6
	INT8U key_inhibit;			// 2
	INT8U key_inweight;		// 2
	INT8U key_discount;		// 3
	INT8U clerk_sales;			// 3	
	INT8U sale_void;			// 3	// 11
	INT8U sale_addup;			// 3
	INT8U sale_return;			// 4
	INT8U sale_override;		// 4
	INT8U cash_open;
	INT8U edit_menu;			// 4
	INT8U edit_plu;				// 4	// 16
	INT8U edit_price;			// 5
	INT8U edit_global;			// 5
	INT8U edit_x_report;		// 5
	INT8U edit_z_report;		// 5
	INT8U edit_store_data;		// 6
	INT8U edit_dc_table;		// 6 // 7200에서 사용 중
	INT8U edit_tax_table;		// 6
	INT8U edit_tare_table;		// 6
	INT8U initialization;		// 7
	INT8U edit_custom_table;	// 7	// 26
	INT8U edit_scale_con;		// 7
	INT8U edit_plu_manage;		// 7 //xxxxxxxxxxxxxxxxxxx
	INT8U any;
	INT8U edit_data_table1;
	INT8U edit_data_table2;
	INT8U print_set;
	INT8U oper_mode;
	INT8U user_parameter;
	INT8U communication;
	INT8U key_save;
	INT8U reopen;
	INT8U sale_negative;
	INT8U key_tare;           // Tare key permission 
	INT8U reserved_byte[0];   // 여분 남아있지 않음
} CLERK_PERMISSION_STRUCT;
#pragma pack()
#endif

#ifndef CLERK_PERMISSION_DEFINE
#define CLERK_PERMISSION_DEFINE
//Permission sale mode : 0~99
#define  PMS_PLU_ITEM_CHANGE   		1
#define  PMS_PLU_PRICE_CHANGE  		2
#define  PMS_KEY_DEPT				3
#define	 PMS_KEY_PAY				4
#define  PMS_KEY_DATE_CHANGE   		5
#define  PMS_KEY_TYPE_CHANGE   		6
//#define  PMS_KEY_INHIBIT			7		// ??
#define  PMS_KEY_IN_WEIGHT			8
#define  PMS_CLERK_SALES			9
#define  PMS_VOID					10
#define  PMS_ADDUP					11
#define  PMS_RETURN					12
#define  PMS_KEY_DISCOUNT			13
#define  PMS_OVERRIDE				14
#define  PMS_OPENCASH				15
#define  PMS_CLERK_KEY_SALES		16
#define  PMS_KEY_EDIT_DISC			50		// ?? 20040318
#define  PMS_CUSTOMER_NO			51
#define  PMS_REOPEN_TICKET			52
#define  PMS_KEY_SAVE				60
#define  PMS_NEGATIVE_SALE			70
#define  PMS_KEY_TARE				80	
#define  PMS_CLERK_LOGIN			99		// ?? 20040318
//Permission program mode : 100~199
//#define	 PMS_DEPAR
#define  PMS_MENU					100
#define  PMS_EDIT_PLU				101
#define  PMS_MANAGEMENT				102
#define  PMS_GLOBAL					103
#define  PMS_X_REPORT				104
#define  PMS_Z_REPORT				105
#define  PMS_STORE_DATA				106
#define  PMS_SECURITY				107
#define  PMS_SCALE_LOCK				108
#define	 PMS_DATA_TABLE1			109
#define  PMS_TAX_RATE				110
#define  PMS_DATA_TABLE2			111
#define  PMS_PRINT_SET				112
#define  PMS_SCALE_CONFIG			113
#define  PMS_OPER_MODE				114
#define  PMS_USER_PARAMETER			115
#define  PMS_COMMUNICATION			116
#define  PMS_EDIT_PRICE				117	// Added by CJK 20050728
#define  PMS_KEY_INHIBIT			118
#define  PMS_CHG_PASSWORD			119
#define  PMS_INITIALIZATION			120


#define  PMS_DEALER					201
#define  PMS_CAL					202
#endif

#ifndef _CLERK_APP_H
#define _CLERK_APP_H
#define CLERK_STRUCT_SIZE	48
#define PERMISSION_STRUCT_SIZE 40
#define CLERK_STRUCT_QTY 100
#define PERMISSION_STRUCT_QTY 2
#endif
//extern void clerk_edit(void);
//extern void general_field_input(INT16U inputtype);
extern void clerk_new(void);
extern void clerk_password_change(void);
extern void clerk_list(void);
extern void clerk_delete(void);
extern void clerk_key_define(void);
// extern void clerk_permission_edit(void);
extern void manager_permission_edit(void);
extern INT16U permission_setting(INT8U permission_id, CLERK_PERMISSION_STRUCT *permission_str);
// extern void clerk_edit_passwd(void);
// extern void clerk_edit_print(void);
// extern void clerk_list(void);
extern void clerk_save(CLERK_STRUCT *clerk);
//extern INT8U clerk_check_override(void);
//extern INT16U clerk_search_empty_code(void);
//extern INT8U clerk_print_list(INT16U start_id, INT16U end_id);
//extern void clerk_delete_proc(void);

