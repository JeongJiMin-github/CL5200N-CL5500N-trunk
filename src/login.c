/*****************************************************************************
|*		       
|*  Copyright		:	(c) 2004 CAS
|*  Filename		:	login.c
|*  Created		:	2004/03/
|*  Description		:	Logoin,Logout
|*              				
****************************************************************************/
#include <stdio.h>
#include <string.h>
#include "globals.h"

#include "commbuf.h"
#include "prtdrv.h"
#include "serial.h"
#include "flash.h"
#include "sensor.h"
#include "ram.h"
#include "prt_render.h"
#include "bar_common.h"
#include "prt_font.h"
#include "dsp_common.h"
#include "prt_interpreter.h"
#include "prt_cmd.h"
#include "main.h"
#include "initial.h"
#include "key.h"
#include "key_typedef.h"
#include "key_function.h"
#include "mode_main.h"
#include "caption_app.h"
#include "common.h"
#include "prt_common.h"
#include "menu.h"
#include "flash_app.h"
#include "dsp_app.h"
#include "dsp_vfd7.h"
#include "key_app.h"

#include "pgm_app.h"
#include "adm_app.h"
#include "department.h"
#include "tax.h"
#include "tare_file.h"
#include "pcs_unit.h"
#include "menu_code_text1.h"
#include "discount.h"
#include "origin.h"
#include "traceability.h"
#include "nutrition_fact.h"
#include "barcode_file.h"
#include "plu_device.h"
#include "plu_manager.h"
#include "plu_edit.h"
#include "rtc.h"
#include "common_sunday.h"
#include "clerk_app.h"
#include "mode_sale.h"
#include "login.h"

extern 	char	gmsg[];
extern 	INT16U	ClerkLoginTime;
//extern INT16S DisplayMsg(char *msg);
extern 	INT8U network_login(INT16U clerkid, INT8U scaleid, INT8U mode);

//INT8U system_password(void)
//{
//	CLERK_STRUCT clerk;
//	INT8U        result;
//	status_scale.level=0;
//REPT:
//	result = password_input_clerk(&clerk,1);// define status.scale.level 
//	if (result) {
//		status_scale.clerkid=clerk.code;
//		status_scale.level  =clerk.permissionID;
//		goto END;
//	}
//	goto REPT;
//END:
//	return 0;
//}

INT8U	DisplayModePrev = 0;
INT8U login_clerk(INT16U clerkid)
{
	if ((clerkid<0) || (clerkid>=100)) return FALSE;
	if(Operation.operationClerk != OPER_NORMAL)
	{
		status_scale.flag_display|=1;
		if(!DisplayModePrev) {	// Added by CJK 20060426 (login_clerk()이 두번 실행될때 이전값이 사라지는 문제 방어)
			DisplayModePrev = get_nvram_bparam(NVRAM_DISPLAY_DATETIME_MODE);
		}
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
		display_date_change(DISPLAY_DATE_CLERK,ON);
#endif
	}
	//ClerkLoginTime = SysTimer100ms;// OPTION:   // Move : Deleted by CJK 20051221
	status_scale.clerkid = clerkid;
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
	set_nvram_bparam(NVRAM_LASTCLERKNUMBER, (char)clerkid);
#endif
	//DisplayClerkTotalInfo(clerkid);

	return 0;
}

//INT8U	LastClerk(void)
//{
//	if (Operation.operationClerk == OPER_NORMAL) {
//		status_scale.clerkid = 0;
//	}
//	else
//	{
//		if (!status_scale.clerkid)
//		{
//			status_scale.clerkid = (INT16S)get_nvram_bparam(NVRAM_LASTCLERKNUMBER);
//			display_date_change(DISPLAY_DATE_CLERK,ON);
//			ClerkLoginTime = SysTimer100ms;// OPTION:   
//		}
//	}
//	return status_scale.clerkid;
//}
extern INT8U NetClerkLockRelease(INT8U operator);
INT8U logout(void)
{

	if (TareOper.addupWeightZero && Operation.operationClerk == OPER_FLOATING && TareOper.tareClerk)
	{
		keyapp_tare(TARE_TYPE_PRESET, 0, OFF);
	}
	
//Start Insert by se-hyung for Clerk Subtotal List 20070727
	if (Clerk_Subtotal_List == 1)
	{
		Clerk_Subtotal_List = 0; 
	 	sale_display_update(UPDATE_SUBTOTAL);
		NetClerkLockRelease(status_scale.clerkid);
	 	display_lcd_clear();	 
	}
//End Insert by se-hyung for Clerk Subtotal List 20070727
	ClerkLoginTime=0;
	status_scale.clerkid=0;
	status_scale.level=0;

	if(DisplayModePrev)
	{
		set_nvram_bparam(NVRAM_DISPLAY_DATETIME_MODE,DisplayModePrev);
		DisplayModePrev = 0;
	}
	return 0;
}

void	InitPermissionUS(CLERK_PERMISSION_STRUCT* pp)
{
	memset((char *)pp,0,sizeof(CLERK_PERMISSION_STRUCT));
	pp->initialization 	=2;
}
#ifdef USE_CHINA_DEFAULT
void	InitPermissionCHN(CLERK_PERMISSION_STRUCT* pp)
{
	memset((char *)pp,0,sizeof(CLERK_PERMISSION_STRUCT));
	pp->edit_menu		=1;
	pp->initialization 	=2;
}
#endif


void	InitPermissionKor(CLERK_PERMISSION_STRUCT* pp)
{
	memset((char *)pp,0,sizeof(CLERK_PERMISSION_STRUCT));
#if defined(USE_EMART_DEFAULT)
	pp->change_item		=1;	// 0->1
	pp->change_price	=1;	// 0->1
	pp->key_dept		=0;
	pp->key_pay		=0;
	pp->key_date_chage	=0;
	pp->key_type_chage	=0;
	pp->key_inhibit		=0;
	pp->key_inweight	=0;
	pp->key_discount	=1;	// 0->1
	pp->clerk_sales		=0;
	pp->sale_void		=0;
	pp->sale_addup		=0;
	pp->sale_return		=0;
	pp->sale_override	=0;
	pp->cash_open		=0;
	pp->edit_menu		=1;	// 0->1
	pp->edit_plu		=0;
	pp->edit_price		=0;
	pp->edit_global		=2;
	pp->edit_x_report	=2;
	pp->edit_z_report	=3;
	pp->edit_store_data	=2;
	pp->edit_tax_table	=2;
//	pp->edit_plu_manage	=0;
	pp->any			=0;
	pp->edit_data_table1	=0;
	pp->edit_data_table2	=0;
	pp->print_set		=2;
	pp->edit_scale_con	=2;
	pp->oper_mode		=3;
	pp->user_parameter	=3;
	pp->communication	=2;
	pp->key_save		=0;
	pp->reopen		=0;
	pp->sale_negative	=0;
	pp->key_tare    	=0;
	pp->initialization 	=2;
#elif (defined(USE_GSMART_TRACE_DEFAULT) || defined(USE_GSMART_DEFAULT))
	pp->change_item		=0;
	pp->change_price	=0;
	pp->key_dept		=0;
	pp->key_pay			=0;
	pp->key_date_chage	=0;
	pp->key_type_chage	=0;
	pp->key_inhibit		=0;
	pp->key_inweight	=0;
	pp->key_discount	=0;
	pp->clerk_sales		=0;
	pp->sale_void		=0;
	pp->sale_addup		=0;
	pp->sale_return		=0;
	pp->sale_override	=0;
	pp->cash_open		=0;
	pp->edit_menu		=0;
	pp->edit_plu		=0;
	pp->edit_price		=0;
	pp->edit_global		=2;
	pp->edit_x_report	=2;
	pp->edit_z_report	=3;
	pp->edit_store_data	=2;
	pp->edit_tax_table	=2;
//	pp->edit_plu_manage	=0;
	pp->any			=0;
	pp->edit_data_table1	=0;
	pp->edit_data_table2	=0;
	pp->print_set		=2;
	pp->edit_scale_con	=2;
	pp->oper_mode		=3;
	pp->user_parameter	=3;
	pp->communication	=2;
	pp->key_save		=0;
	pp->reopen		=0;
	pp->sale_negative	=0;
	pp->key_tare    	=0;
	pp->initialization 	=2;
#elif defined(USE_TOPMART_DEFAULT) //sewon 161122
	pp->change_item 	=0;
	pp->change_price	=0;
	pp->key_dept		=0;
	pp->key_pay 		=0;
	pp->key_date_chage	=0;
	pp->key_type_chage	=0;
	pp->key_inhibit 	=0;
	pp->key_inweight	=0;
	pp->key_discount	=0;
	pp->clerk_sales 	=0;
	pp->sale_void		=0;
	pp->sale_addup		=0;
	pp->sale_return 	=0;
	pp->sale_override	=0;
	pp->cash_open		=0;
	pp->edit_menu		=1; //sewon 170224
	pp->edit_plu		=0;
	pp->edit_price		=0;
	pp->edit_global 	=0;
	pp->edit_x_report	=2;
	pp->edit_z_report	=3;
	pp->edit_store_data =0;
	pp->edit_tax_table	=2;
	pp->any 		=0;
	pp->edit_data_table1	=0;
	pp->edit_data_table2	=0;
	pp->print_set		=0;
	pp->edit_scale_con	=0;
	pp->oper_mode		=3;
	pp->user_parameter	=0;
	pp->communication	=0;
	pp->key_save		=0;
	pp->reopen		=0;
	pp->sale_negative	=0;
	pp->key_tare    	=0;
	pp->initialization 	=2;
#elif defined(USE_NHMART_SAFE_DEFAULT)
	pp->change_item		=3;	//
	pp->change_price	=0;
	pp->key_dept		=0;
	pp->key_pay		=0;
	pp->key_date_chage	=0;
	pp->key_type_chage	=0;
	pp->key_inhibit		=0;
	pp->key_inweight	=0;
	pp->key_discount	=0;
	pp->clerk_sales		=0;
	pp->sale_void		=0;
	pp->sale_addup		=0;
	pp->sale_return		=0;
	pp->sale_override	=0;
	pp->cash_open		=0;
	pp->edit_menu		=0;
	pp->edit_plu		=3;	//
	pp->edit_price		=0;
	pp->edit_global		=2;
	pp->edit_x_report	=2;
	pp->edit_z_report	=3;
	pp->edit_store_data	=2;
	pp->edit_tax_table	=2;
//	pp->edit_plu_manage	=0;
	pp->any			=0;
	pp->edit_data_table1	=0;
	pp->edit_data_table2	=3;	//
	pp->print_set		=2;
	pp->edit_scale_con	=2;
	pp->oper_mode		=3;
	pp->user_parameter	=3;
	pp->communication	=2;
	pp->key_save		=0;
	pp->reopen		=0;
	pp->sale_negative	=0;
	pp->key_tare    	=0;
	pp->initialization 	=2;
#elif defined(USE_LOTTEMART_DEFAULT)
	pp->change_item 	=1;
	pp->change_price	=1;
	pp->key_dept		=0;
	pp->key_pay 	=0;
	pp->key_date_chage	=0;
	pp->key_type_chage	=0;
	pp->key_inhibit 	=0;
	pp->key_inweight	=1;
	pp->key_discount	=1;
	pp->clerk_sales 	=0;
	pp->sale_void		=0;
	pp->sale_addup		=0;
	pp->sale_return 	=0;
	pp->sale_override	=0;
	pp->cash_open		=0;
	pp->edit_menu		=0;
	pp->edit_plu		=1;
	pp->edit_price		=1;
	pp->edit_global 	=0;
	pp->edit_x_report	=0;
	pp->edit_z_report	=0;
	pp->edit_store_data =0;
	pp->edit_tax_table	=2;
//	pp->edit_plu_manage =0;
	pp->any 		=0;
	pp->edit_data_table1	=0;
	pp->edit_data_table2	=0;
	pp->print_set		=0;
	pp->edit_scale_con	=0;
	pp->oper_mode		=3;
	pp->user_parameter	=3;
	pp->communication	=0;
	pp->key_save		=0;
	pp->reopen		=0;
	pp->sale_negative	=0;
	pp->key_tare    	=0;
	pp->initialization 	=2;
#elif defined(USE_LOTTESUPER_DEFAULT)
	pp->change_item		=0;
	pp->change_price	=0;
	pp->key_dept		=0;
	pp->key_pay		=0;
	pp->key_date_chage	=0;
	pp->key_type_chage	=1;
	pp->key_inhibit		=0;
	pp->key_inweight	=0;
	pp->key_discount	=0;
	pp->clerk_sales		=0;
	pp->sale_void		=0;
	pp->sale_addup		=0;
	pp->sale_return		=0;
	pp->sale_override	=1;
	pp->cash_open		=0;
	pp->edit_menu		=0;
	pp->edit_plu		=1;
	pp->edit_price		=1;
	pp->edit_global		=2;
	pp->edit_x_report	=2;
	pp->edit_z_report	=3;
	pp->edit_store_data	=2;
	pp->edit_tax_table	=2;
	pp->any			=0;
	pp->edit_data_table1	=0;
	pp->edit_data_table2	=0;
	pp->print_set		=2;
	pp->edit_scale_con	=2;
	pp->oper_mode		=3;
	pp->user_parameter	=3;
	pp->communication	=2;
	pp->key_save		=0;
	pp->reopen		=0;
	pp->sale_negative	=0;
	pp->key_tare    	=0;
	pp->initialization 	=2;
#else
	pp->change_item		=0;
	pp->change_price	=0;
	pp->key_dept		=0;
	pp->key_pay		=0;
	pp->key_date_chage	=0;
	pp->key_type_chage	=0;
	pp->key_inhibit		=0;
	pp->key_inweight	=0;
	pp->key_discount	=0;
	pp->clerk_sales		=0;
	pp->sale_void		=0;
	pp->sale_addup		=0;
	pp->sale_return		=0;
	pp->sale_override	=0;
	pp->cash_open		=0;
#if defined (USE_NHMART_DEFAULT) || defined(USE_BESTCO_DEFAULT)
	pp->edit_menu		=1;
#else
	pp->edit_menu		=0;
#endif
	pp->edit_plu		=0;
	pp->edit_price		=0;
	pp->edit_global		=2;
	pp->edit_x_report	=2;
	pp->edit_z_report	=3;
	pp->edit_store_data	=2;
	pp->edit_tax_table	=2;
//	pp->edit_plu_manage	=0;
	pp->any			=0;
	pp->edit_data_table1	=0;
	pp->edit_data_table2	=0;
	pp->print_set		=2;
	pp->edit_scale_con	=2;
	pp->oper_mode		=3;
	pp->user_parameter	=3;
	pp->communication	=2;
	pp->key_save		=0;
	pp->reopen		=0;
	pp->sale_negative	=0;
	pp->key_tare    	=0;
	pp->initialization 	=2;
#endif
}

void	InitPermissionEng(CLERK_PERMISSION_STRUCT* pp)
{
	memset((char *)pp,0,sizeof(CLERK_PERMISSION_STRUCT));
	pp->change_item		=0;
	pp->change_price	=0;
	pp->key_dept		=0;
	pp->key_pay		=0;
	pp->key_date_chage	=0;
#ifdef USE_INDIA_FUNCTION
	pp->key_type_chage	=0;
#else
	pp->key_type_chage	=1;
#endif
	pp->key_inhibit		=3;		// 인도 요청 기본값 변경 (Reliance 납품 건)
	pp->key_inweight	=3;
	pp->key_discount	=3;
	pp->clerk_sales		=3;
	pp->sale_void		=3;
	pp->sale_addup		=3;
	pp->sale_return		=3;
	pp->sale_override	=3;
	pp->key_inhibit		=2;
	pp->key_inweight	=1;
	pp->key_discount	=0;
	pp->clerk_sales		=0;
	pp->sale_void		=1;
	pp->sale_addup		=0;
	pp->sale_return		=1;
	pp->sale_override	=1;
	pp->cash_open		=1;

#if defined(USE_VIETNAM_EMART_DEFAULT) || defined(USE_STOKROTKA_MART_DEFAULT)
	pp->edit_menu		=1;
#else
	pp->edit_menu		=0;
#endif
	pp->edit_plu		=0;
#if defined (USE_INDIA_TATA_DEFAULT)
	pp->edit_price		=3;  	// 인도 요청 기본값 변경 (Reliance 납품 건)
#else
	pp->edit_price		=0;
#endif
	pp->edit_global		=0;

	pp->edit_x_report	=2;
	pp->edit_z_report	=3;
	pp->edit_store_data	=0;
	pp->edit_tax_table	=2;
//	pp->edit_plu_manage	=0;
	pp->any			=0;
	pp->edit_data_table1	=0;
	pp->edit_data_table2	=0;
	pp->print_set		=2;
	pp->edit_scale_con	=0;
	pp->oper_mode		=0;
	pp->user_parameter	=3;
	pp->communication	=2;
	pp->key_save		=0;	//Added by JJANG 20080131
	pp->reopen		=0;
	pp->sale_negative	=0;
	pp->key_tare    	=0;
	pp->initialization 	=2;
}

#ifdef USE_MON_EMART_DEFAULT
void	InitPermissionMON_Emart(CLERK_PERMISSION_STRUCT* pp)
{
	memset((char *)pp,0,sizeof(CLERK_PERMISSION_STRUCT));
	pp->change_item		=1;	// 0->1
	pp->change_price	=1;	// 0->1
	pp->edit_menu		=1;	// 0->1
	pp->edit_global		=2;
	pp->edit_x_report	=2;
	pp->edit_z_report	=3;
	pp->edit_store_data	=2;
	pp->edit_tax_table	=2;
	pp->print_set		=2;
	pp->edit_scale_con	=2;
	pp->oper_mode		=3;
	pp->user_parameter	=3;
	pp->communication	=2;
	pp->key_discount	=1;
	pp->key_tare    	=1;
	pp->initialization 	=2;
}
#endif
#ifdef USE_MONGOL_DEFAULT
void InitPermissionMONGOL(CLERK_PERMISSION_STRUCT* pp)
{
	memset((char *)pp,0,sizeof(CLERK_PERMISSION_STRUCT));
	pp->change_item		=1;	// 0->1
	pp->change_price	=1;	// 0->1
	pp->edit_menu		=1;	// 0->1
	pp->edit_global		=2;
	pp->edit_x_report	=2;
	pp->edit_z_report	=3;
	pp->edit_store_data	=2;
	pp->edit_tax_table	=2;
	pp->print_set		=2;
	pp->edit_scale_con	=2;
	pp->oper_mode		=3;
	pp->user_parameter	=3;
	pp->communication	=2;
	pp->key_discount	=1;
	pp->key_tare    	=0;
	pp->initialization 	=2;
}
#endif

INT8U	CheckUseFunctionLevel(INT8U func_code)
{
	INT8U	checkLevel=0;
	CLERK_PERMISSION_STRUCT permission;
	INT32U  addr,permissionid;

	if (Operation.operationClerk==OPER_NORMAL)
		permissionid=0;
	else
		permissionid=1;

	addr = permissionid;
	addr*= PERMISSION_STRUCT_SIZE;//sizeof(CLERK_PERMISSION_STRUCT);
	addr+= FLASH_CLERK_PERMISSION;
       	//memcpy((INT8U*)&permission, (INT8U*)addr, CLERK_STRUCT_SIZE);
	get_base_sparam(addr,(INT8U*)&permission, PERMISSION_STRUCT_SIZE );

	if (func_code<100)// sale mode
	{
		if(Operation.operationMode == MODE_MANAGER)
		{
		checkLevel = 0;
		return checkLevel;
		}
		switch(func_code)
		{
			case PMS_PLU_ITEM_CHANGE:
				checkLevel = permission.change_item;
				break;
			case PMS_PLU_PRICE_CHANGE:
				checkLevel = permission.change_price;
				break;
			case PMS_KEY_DEPT:			
				checkLevel = permission.key_dept;
				break;
			case PMS_KEY_PAY:
				checkLevel = permission.key_pay;
				break;
			case PMS_KEY_DATE_CHANGE:
				checkLevel = permission.key_date_chage;
				break;
			case PMS_KEY_TYPE_CHANGE:
				checkLevel = permission.key_type_chage;
				break;
			case PMS_KEY_IN_WEIGHT:		
				checkLevel = permission.key_inweight;
				break;
			case PMS_KEY_EDIT_DISC:				 // 040318  getwing
				//checkLevel = permission.key_discount; 
				checkLevel = permission.edit_dc_table;
				break;
			case PMS_CLERK_SALES:
				checkLevel = permission.clerk_sales;// 0: no check password
				//#if(!checkLevel && Operation.operationClerk == OPER_STD_CLERK)
				//#	checkLevel = 1;
				break;
			case PMS_CLERK_KEY_SALES:
				//checkLevel = 1;
				checkLevel = permission.clerk_sales;// Modified by CJK 20050823
				break;
			case PMS_VOID:				
				checkLevel = permission.sale_void;
				break;
			case PMS_ADDUP:
				checkLevel = permission.sale_addup;
				break;
			case PMS_RETURN:				
				checkLevel = permission.sale_return;
				break;
			case PMS_KEY_DISCOUNT:
				checkLevel = permission.key_discount;
				break;
			case PMS_OVERRIDE:
				checkLevel = permission.sale_override;
				break;
			case PMS_OPENCASH:
				checkLevel = permission.cash_open;
				break;
			case PMS_KEY_SAVE:
				checkLevel = permission.key_save;
				break;
			case PMS_CUSTOMER_NO:
				checkLevel = 0;
				break;
			case PMS_REOPEN_TICKET:
				checkLevel = permission.reopen;
				break;
			case PMS_NEGATIVE_SALE:
				checkLevel = permission.sale_negative;
				break;
			case PMS_KEY_TARE:
				checkLevel = permission.key_tare;
				break;
			default:
				checkLevel = permission.any;
				break;
		}
	}

	if (100<=func_code && func_code<200)// program mode
	{
	    switch(func_code)
	    {
		case PMS_MENU:
			checkLevel = permission.edit_menu;
			break;
		case PMS_EDIT_PLU:
			checkLevel = permission.edit_plu;
			break;
		case PMS_EDIT_PRICE:
			checkLevel = permission.edit_price;
			break;
		case PMS_KEY_INHIBIT:
			checkLevel = permission.key_inhibit;
			break;
//		case PMS_MANAGEMENT:
//			checkLevel = permission.change_price;	//사용시 permission 변경 필요.
//			break;
		case PMS_GLOBAL:			
			checkLevel = permission.edit_global;
//#sprintf(temp,"Check Level=%d",checkLevel);
//#PutNSmallString(0,0,temp,30);
//#Key_DelaySec(10);
			break;
		case PMS_X_REPORT:
			checkLevel = permission.edit_x_report;
			break;
		case PMS_Z_REPORT:
			checkLevel = permission.edit_z_report;
			break;
		case PMS_STORE_DATA:
			checkLevel = permission.edit_store_data;
			break;
		case PMS_SECURITY:
			checkLevel = 3;
			break;
		case PMS_SCALE_LOCK:
			checkLevel = 1;// 직원이면 됨
			break;
		case PMS_DATA_TABLE1:
			checkLevel = permission.edit_data_table1;
			break;
		case PMS_TAX_RATE:
			checkLevel = permission.edit_tax_table;
			break;
		case PMS_DATA_TABLE2:
			checkLevel = permission.edit_data_table2;
			break;
		case PMS_PRINT_SET:
			checkLevel = permission.print_set;
			break;
		case PMS_SCALE_CONFIG:
			checkLevel = permission.edit_scale_con;
			break;
		case PMS_OPER_MODE:
			checkLevel = permission.oper_mode;
			break;
		case PMS_CHG_PASSWORD:
			checkLevel = 3;
			break;
		case PMS_USER_PARAMETER:
			checkLevel = permission.user_parameter;
			break;
		case PMS_COMMUNICATION:
			checkLevel = permission.communication;
			break;
		case PMS_INITIALIZATION:
			checkLevel = permission.initialization;
			break;
		default:
			checkLevel = permission.any;
			break;
	    }
	}
	if (200<=func_code)
	{
		if(func_code==PMS_DEALER)
			checkLevel = 98;
		if(func_code==PMS_CAL   )
			checkLevel = 99;
	}
// END:
// sprintf(temp,"Check Level=%d\n",checkLevel);
// PutNSmallString(0,0,temp,30);
// Key_DelaySec(10);
// MsgOut(temp);
	return checkLevel;
}

INT16S	LoadClerk(CLERK_STRUCT* clerkp,INT16U logOperator)
{
	INT32U	addr;

//#   	addr = DFLASH_BASE + FLASH_CLERK_AREA;
//#	addr = addr + logOperator * CLERK_STRUCT_SIZE;
//#	memcpy((INT8U*)clerkp, (INT8U*)addr, CLERK_STRUCT_SIZE);
	addr = (FLASH_CLERK_AREA + logOperator*CLERK_STRUCT_SIZE);
	get_base_sparam(addr, (INT8U*) clerkp, CLERK_STRUCT_SIZE);

	if(clerkp->code != logOperator)
		clerkp->permissionLevel = 0;
	if(!clerkp->code)
		clerkp->permissionLevel = 0;
	return clerkp->permissionLevel;
}
// 20040318 Dsp_SetPage(DSP_DEFAULT_PAGE); re_load 아닐때도 default창으로 돌려놓아야 한다.
INT8U password_input_clerk(CLERK_STRUCT* clerkp, char dsp_reload)
{
	CAPTION_STRUCT cap;
	POINT 	disp_p;

//	INT8U	y_gab=8;				// 폰트의 y 크기
//	INT8U	x_gab=6;				// 폰트의 x 크기

	INT8U	ret_string[20];
	char	password[20];
	INT8U	prt;
	INT16S   	result,i,str_size;
	INT8U   old_key;
	char    old_font;
//	INT32U	cap_addr;
	//INT32U	addr;
	//char    temp[30];
	int	cnumber;
	str_size=18;
	//Dsp_SetPage(DSP_ERROR_PAGE);
//	FontSizeChange(FONT_SMALL);
	old_font=DspStruct.Id1;
	DspLoadFont1(DSP_MENU_FONT_ID);
	Dsp_Fill_Buffer(0);
//	display_lcd_clear();
//	cap_addr = caption_search_by_code(0x1F00);
//	memcpy((char *)&cap,(char *)cap_addr,sizeof(cap));
	caption_split_by_code(0x1f00,&cap,0);
	caption_adjust(&cap,NULL);
#ifdef USE_SINGLE_LINE_GRAPHIC
	display_clear(DISPLAY_UNITPRICE);
	display_string(DISPLAY_UNITPRICE, (INT8U *)cap.form);
#else
	disp_p = point( Startup.menu_type*Startup.menu_ygap, 0);
#ifdef HEBREW_FONT
	Convert_HebrewPreview((INT8U *)cap.form, strlen(cap.form), 0, 0); 
	hebrew_codetemp[strlen(cap.form)] = 0;
	disp_p.x = 202 - disp_p.x;
	Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
	Convert_ArabPreview((INT8U *)cap.form, strlen(cap.form), 0, 0, 0); 
	arab_codetemp[strlen(cap.form)] = 0;
	disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
	Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
	Dsp_Write_String(disp_p,(HUGEDATA char *)cap.form);
#endif
	Dsp_Diffuse();
#endif
	old_key=KEY_GetMode();
	KEY_SetMode(1);
KI_IN:
	memset((INT8U*)ret_string, 0x00, 15);
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
	result = keyin_string(KI_FREE_MODE, KI_GRAPHIC,0, (HUGEDATA INT8U*)ret_string, cap.input_length, 
					cap.input_dlength, Startup.menu_type*Startup.menu_ygap, cap.input_x*Startup.menu_xgap, 1, 1, KI_NOPASS);
#else
	result = keyin_string(KI_FREE_MODE, KI_DEFAULT,0, (HUGEDATA INT8U*)ret_string, cap.input_length,
		      				cap.input_dlength, Startup.menu_type*Startup.menu_ygap, cap.input_x*Startup.menu_xgap, 1, 1, KI_NOPASS);
#endif

	for (i=cap.input_dlength+2; 2<i; i--) {
		if(*(ret_string+i-1)==0 || *(ret_string+i-1)==0x20)
			*(ret_string+i-1)=0x00;
		else
			break;
	}
	ret_string[cap.input_dlength+2]=0;

	if(result == KP_ESC) goto RET_ESC;

	sscanf((char*)ret_string,"%02d",&cnumber);
	Operation.logOperator = (INT16U)cnumber;
	if(Operation.logOperator == 0)
	{
		INT32U	cap_addr;

		cap_addr  = DFLASH_BASE;
		cap_addr += FLASH_START_PASSWORD;
		Flash_sread(cap_addr,(HUGEDATA INT8U *)password,DEALER_PASSWORD_LEN);password[DEALER_PASSWORD_LEN]=0;
		//get_base_sparam(FLASH_START_PASSWORD, (INT8U*)&password, 10);
		status_scale.level = 98;// need data
	}
	else
	{
		if(!LoadClerk(clerkp,Operation.logOperator))
			goto RET_ESC;//no
		memcpy(password,clerkp->password,str_size);
		status_scale.level = clerkp->permissionLevel;// need data
	}
	password[str_size]=0;
	for (i=8; i; i--)
	{
		if(*(password+i)==0 || *(password+i)==0x20)
			*(password+i)=0x00;
		else
			break;
	}
//	if(i==0)	no password
	prt=strncmp((char *)(ret_string+2),password,DEALER_PASSWORD_LEN);
	if (prt && Operation.logOperator == 0)// false
	{
		INT16S	i,flag=OFF;

		for(i=1;i<MAX_CLERK_NO;i++)
		{
			LoadClerk(clerkp,i);
			if(clerkp->permissionLevel==3)
			{
				flag = ON;
				break;
			}
		}
		if(!flag)	// level 3인 user가 없을 경우 "001" 사용가능
		{
			char	mngrPass[8];
			INT16S	j;

			mngrPass[0]='1';
			for(j=1;j<8;j++)
				mngrPass[j]=0;
			prt=strcmp((char *)(ret_string+2),mngrPass);
			if(!prt)
				status_scale.level = 3;
		}
	}
	if (prt)// false
	{
//DisplayMsg("FALSE");
//Key_DelaySec(10);
		goto KI_IN;
	}
	//Dsp_SetPage(DSP_DEFAULT_PAGE);

	if(dsp_reload == 1) {
		Dsp_Diffuse();
	}
	if (!status_scale.level) {
//		DisplayMsg((char *)"Clerk not used");
		DisplayMsg((char *)"Not used");//change to ERROR msg
		goto RET_ESC;
	}
KEY_SetMode(old_key);
//DisplayMsg("OK !!");
//Key_DelaySec(10);
	DspLoadFont1(old_font);

	return ON;//yes
RET_ESC:
	//KEY_SetMode(old_key);
	//Dsp_SetPage(DSP_DEFAULT_PAGE);
	if(dsp_reload == 1) {
		Dsp_Diffuse();
	}
	Operation.logOperator = 0;
	clerkp->permissionLevel=0;
	KEY_SetMode(old_key);
	DspLoadFont1(old_font);
	return OFF;//no
}

INT8U password_input_clerk_no(CLERK_STRUCT* clerkp, char dsp_reload)
{
	CAPTION_STRUCT cap;
	INT8U   result;
	INT8U   ret;
	INT8U   old_font;
	INT8U   old_key;
	INT32U  clerk_no;


	old_font = DspStruct.Id1;
	old_key = KEY_GetMode();
	DspLoadFont1(DSP_MENU_FONT_ID);
	KEY_SetMode(3);		       
	clerk_no = 0;

	Menu_Init();
	cap.form[0] = 0;
	Menu_HeadCreate((HUGEDATA char *)cap.form);

	caption_split_by_code(0x1f01, &cap,0);
	caption_adjust(&cap,NULL);
	Menu_InputCreate(1, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
						cap.input_max, cap.input_min, 0, &clerk_no, NULL, NULL, NULL);

	Menu_SetOneTouch(1, ON);

	while (1)
	{
		result = Menu_InputDisplay();

		if (result == MENU_RET_SAVE)
		{
			Operation.logOperator = (INT16U)clerk_no;
			if (!LoadClerk(clerkp, Operation.logOperator))
			{
				BuzOn(2);
				continue;
			}
			status_scale.level = clerkp->permissionLevel;// need data

			if (!status_scale.level) 
			{
//		             DisplayMsg((char *)"Clerk not used");
				DisplayMsg((char *)"Not used");//change to ERROR msg
				ret = OFF;
				break;
			}
			else
			{
				ret = ON;
				break;
			}
		}
		else
		{
			Operation.logOperator = 0;
			clerkp->permissionLevel=0;
			ret = OFF;
			break;
		}
	}
	if (dsp_reload == 1) 
	{
		Dsp_Diffuse();
	}
	KEY_SetMode(old_key);
	DspLoadFont1(old_font);
	return ret;
}

INT8U permission_check_sub(INT8U func_code, INT8U login_chk)
{
	CLERK_STRUCT clerk;
	INT8U result,passFlag=ON,InputPass=ON;
	INT8U oldClerk,oldLevel;
	INT8U old_page;

#ifdef USE_CL5200_DISPLAY_DEFAULT  //_USE_LCD20848_
	if(func_code != PMS_SECURITY) Dsp_Clear();
#endif

	Operation.logOperator = 0;
	old_page = DspStruct.Page;
	while(1)
	{
		oldClerk = status_scale.clerkid;
		oldLevel = status_scale.level;
		Operation.logLevel= CheckUseFunctionLevel(func_code);
		if(func_code == PMS_CLERK_LOGIN) login_chk = 1;		//Modified by JJANG 20080424
		if(status_scale.level)
		{
			if(Operation.logLevel <= status_scale.level)
				passFlag=OFF;
		}
		if (func_code == PMS_CLERK_KEY_SALES)	//Clerk key  log in
		{
			if (Operation.logLevel)
			{
				if(login_chk&0x80)
				{
					//if(status_run.run_mode == RUN_SALE) 
					//{
					//	Dsp_ChangeMode(DSP_PGM_MODE);
					//	//LCD_DrawBackgroundProgramMode();
					//}
					Dsp_SetPage(DSP_PSWD_PAGE);
					result = password_input_clerk(&clerk,1);
					Dsp_SetPage(old_page);
					Dsp_Diffuse();
					//if(status_run.run_mode == RUN_SALE) 
					//{
					//	Dsp_ChangeMode(DSP_SALE_MODE);
					//	//LCD_DrawBackgroundSaleMode();
					//	//Dsp_Diffuse();
					//}
					if (result == OFF)
					{
						result=0;
						goto LOG_RET1;
					}
				}
				else
				{
					result=ON;
					InputPass = OFF;
					goto LOG_RET1;
				}
			}
			else
			{
				result=ON;
				InputPass = OFF;
				goto LOG_RET1;
			}
		}
		else
		{
			if (Operation.logLevel == 4)	//4이면 password도 묻지않고 그냥 제한함. 
            {
                result = OFF;
				BuzOn(2);
                goto LOG_RET1;
            }
            else if ((Operation.logLevel && passFlag) || (login_chk&0x01))
			{
				//if(status_run.run_mode == RUN_SALE)
				//{
				//	Dsp_ChangeMode(DSP_PGM_MODE);
				//	//LCD_DrawBackgroundProgramMode();
				//}
				Dsp_SetPage(DSP_PSWD_PAGE);
				if((login_chk&0x01) && !Operation.logLevel)
				{
					result = password_input_clerk_no(&clerk, 1); 
				}
				else
				{
					result = password_input_clerk(&clerk,1);// define status.scale.level 
				}
				Dsp_SetPage(old_page);
				Dsp_Diffuse();
				//if(status_run.run_mode == RUN_SALE) 
				//{
				//	Dsp_ChangeMode(DSP_SALE_MODE);
				//	//LCD_DrawBackgroundSaleMode();
				//	//Dsp_Diffuse();
				//}
				if (result == OFF)
				{
					result=0;
					goto LOG_RET1;
				}
			}
			else
			{
				result=ON;
				InputPass = OFF;
				goto LOG_RET1;
			}

		}

		result = permission_check_user(status_scale.level, func_code);
		if(result) {
			goto LOG_RET1;
		}
	}
LOG_RET1:
	if(result)
	{
		if(!InputPass)
		{
			status_scale.clerkid = oldClerk;
		}
		else
		{
			status_scale.clerkid = Operation.logOperator;//clerk.code;
		}
	}
	else
	{
		status_scale.level = oldLevel;
		status_scale.clerkid = oldClerk;
	}
	return result;
}

// return ON           : 통과하여 다음 기능 수행
// Operation.logLevel : password 사용 유뮤
// INT8U check_password(INT8U func_code)
//INT8U permission_check(INT16U clerkid,INT8U func_code)
INT8U permission_check(INT8U func_code, INT8U login_chk)
{
	CLERK_STRUCT clerk;
	INT8U result,passFlag=ON,InputPass=ON;
	INT8U oldClerk;
	INT8U old_page;

#ifdef USE_CL5200_DISPLAY_DEFAULT  //_USE_LCD20848_	//메뉴 코드 사라지는 문제 수정
	if((func_code != PMS_SECURITY) && (func_code != PMS_KEY_SAVE)) Dsp_Clear();
#endif

	Operation.logOperator = 0;
	old_page = DspStruct.Page;
	while(1)
	{
		oldClerk = status_scale.clerkid;
		Operation.logLevel= CheckUseFunctionLevel(func_code);
		if(func_code == PMS_CLERK_LOGIN) login_chk = 1;		//Modified by JJANG 20080424
		if(status_scale.level)
		{
			if(Operation.logLevel <= status_scale.level)
				passFlag=OFF;
		}
		if (func_code == PMS_CLERK_KEY_SALES)	//Clerk key  log in
		{
			if (Operation.logLevel)
			{
				if(login_chk&0x80)
				{
					//if(status_run.run_mode == RUN_SALE) 
					//{
					//	Dsp_ChangeMode(DSP_PGM_MODE);
					//	//LCD_DrawBackgroundProgramMode();
					//}
					Dsp_SetPage(DSP_PSWD_PAGE);
					result = password_input_clerk(&clerk,1);
					Dsp_SetPage(old_page);
					Dsp_Diffuse();
					//if(status_run.run_mode == RUN_SALE) 
					//{
					//	Dsp_ChangeMode(DSP_SALE_MODE);
					//	//LCD_DrawBackgroundSaleMode();
					//	//Dsp_Diffuse();
					//}
					if (result == OFF)
					{
						result=0;
						goto LOG_RET1;
					}
				}
				else
				{
					result=ON;
					InputPass = OFF;
					goto LOG_RET1;
				}
			}
			else
			{
				result=ON;
				InputPass = OFF;
				goto LOG_RET1;
			}
		}
		else
		{
			if (Operation.logLevel == 4)	//4이면 password도 묻지않고 그냥 제한함. 
            {
                result = OFF;
				BuzOn(2);
                goto LOG_RET1;
            }
            else if ((Operation.logLevel && passFlag) || (login_chk&0x01))
			{
				//if(status_run.run_mode == RUN_SALE)
				//{
				//	Dsp_ChangeMode(DSP_PGM_MODE);
				//	//LCD_DrawBackgroundProgramMode();
				//}
				Dsp_SetPage(DSP_PSWD_PAGE);
				if((login_chk&0x01) && !Operation.logLevel)
				{
					result = password_input_clerk_no(&clerk, 1); 
				}
				else
				{
					result = password_input_clerk(&clerk,1);// define status.scale.level 
				}
				Dsp_SetPage(old_page);
				Dsp_Diffuse();
				//if(status_run.run_mode == RUN_SALE) 
				//{
				//	Dsp_ChangeMode(DSP_SALE_MODE);
				//	//LCD_DrawBackgroundSaleMode();
				//	//Dsp_Diffuse();
				//}
				if (result == OFF)
				{
					result=0;
					goto LOG_RET1;
				}
			}
			else
			{
				result=ON;
				InputPass = OFF;
				goto LOG_RET1;
			}

		}

		result = permission_check_user(status_scale.level, func_code);
		if(result) {
			goto LOG_RET1;
		}
	}
LOG_RET1:
	if(result)
	{
		if(!InputPass)
		{
			status_scale.clerkid = oldClerk;
		}
		else
		{
			status_scale.clerkid = Operation.logOperator;//clerk.code;
		}
	}
	else
	{
		status_scale.level = 0;
		status_scale.clerkid = 0;
	}
	return result;
}

// INT8U	CheckClerkPermission(INT8U userLevel,CLERK_STRUCT* csp, INT8U func_code)
INT8U	permission_check_user(INT8U userLevel, INT8U func_code)
{
	INT8U	pmslevel=ON;
	//char    temp[32];

	if (!userLevel)
	{
		//DisplayMsg("Permission Denied");
		return OFF;
	}
	if(func_code<100)// sale mode
	{
		if(userLevel==99)	return OFF;
		if(userLevel==98)	return OFF;
		pmslevel=CheckUseFunctionLevel(func_code);
	}
	else if(func_code<200)	//program mode
	{
		if(userLevel==99)	return ON;
		if(userLevel==98)	return ON;
		pmslevel=CheckUseFunctionLevel(func_code);

	}
	else if(200<func_code)
	{
		pmslevel=CheckUseFunctionLevel(func_code);
	}
	if(userLevel < pmslevel)
	{
		//  clerk.level < pmsLevel	// 0:system, 1: high, 2:mid, 3:low
//sprintf(gmsg,"user %d, pms %d",userLevel,pmslevel);
//		DisplayMsg(gmsg);
		Key_DelaySec(10,0);
		return OFF;
	}
	return ON;
}

