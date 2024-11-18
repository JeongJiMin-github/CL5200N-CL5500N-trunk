/*****************************************************************************
|*  Copyright		:	(c) 2002 CAS
|*  Filename		:	Mode_common.c
|*  Version			:	0.1
|*  Programmer(s)	:	Chung jong kwon (CJK)
|*  Created			:	2003/08/20
|*  Description		:	CLP main routine    
****************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "globals.h"
#include "commbuf.h"
#include "prt_common.h"
#include "prtdrv.h"
#include "timer.h"
#include "serial.h"
#include "flash.h"
#include "sensor.h"
#include "ram.h"
#include "prt_render.h"
#include "bar_common.h"
#include "prt_font.h"
#include "dsp_common.h"
#include "prt_interpreter.h"
#include "dsp_vfd7.h"
#include "adm.h"
#include "adm_app.h"
#include "key.h"
#include "main.h"
#include "cal.h"
#include "prt_cmdtest.h"
#include "programmode.h"
//#include "file.h"
#include "rtc.h"
#include "debug.h"
#include "common.h"
#include "mode_commun.h"
#include "plu_manager.h"
#include "plu_device.h"
#include "initial.h"
#include "flash_app.h"
#include "comm_app.h"
#include "caption_app.h"
#include "menu.h"
#include "prt_font.h"
#include "dsp_app.h"
#include "commbuf.h"
#include "ethernet.h"
#include "ethernet_comm.h"
#include "mode_error.h"
#include "global_set.h"
#include "discount.h"
#include "mode_sale.h"
#include "network_command.h"
#include "origin.h"
#include "global_set.h"
#include "plu_edit.h"
#include "department.h"
#include "tare_file.h"
#include "tax.h"
#include "pcs_unit.h"
#include "barcode_file.h"
#include "workplace.h"
#include "nutrition_fact.h"
#include "traceability.h"
#include "mode_calibration.h"
#include "sale_report.h"
#include "customer_app.h"
#include "shopname.h"
#include "clerk_app.h"
#include "key_typedef.h"
#include "currency.h"
#include "key_function.h"
#include "mode_sale.h"
#include "prt_app.h"
#include "ingredient.h"
#include "initial_glb.h"
#include "prt_advertise.h"
#include "network_set.h"
#include "inventory.h"
#include "license.h"
#include "scale_config.h"
#ifdef USE_PROTOCOL_CL5000JR
#include "cl5kj_interpreter.h"
#endif


//#define _USE_DBG_PLU_SYNC
extern INT16U plu_n_field;
extern PLU_GLOBAL_INFO_STRUCT plu_global[128];
extern INT8U global_one2one_send;
extern INT8U flagcopy_ticket;

extern INT16S netGetClerkLock(INT8U operator, INT8U scaleid);
extern void netSetClerkLock(INT8U operator, INT8U scaleid, INT8U lock);
extern char  date_getweek(short year,short month,short day );

extern void transaction_sale_store(TRANSACTION_RECORD_TYPE *pTrans, TRANSACTION_RECORD_SUB_TYPE *pTrans_sub, TR_ADDITION_DATA_TYPE* pTrans_add, UNITPRICE_CHANGE_TYPE *pChgPrice, INT8U mode, INT8U sendflag);
extern void transaction_sale_end(HUGEDATA COMM_BUF *CBuf,INT8U clerk,INT16U tail);
extern INT8U  commun_send_block_data(HUGEDATA COMM_BUF *CBuf,INT32U cmd,INT8U data,INT32U no);
extern INT8U  commun_send_addr_data(HUGEDATA COMM_BUF *CBuf,INT32U cmd,INT16U fcode);
extern void commun_return(INT8U cmd,INT16U errcode);
extern INT32U commun_get_block_type_address(INT8U data,INT32U *max_size);
extern INT16S ClerkAddTransaction(INT8U ClerkNum,TRANSACTION_RECORD_TYPE  *CurrentTrans, INT8U network, INT32U TicketNo);
extern INT16S LocalPluTotalSummaries(INT8U scale,TRANSACTION_RECORD_TYPE* pTrans,TRANSACTION_RECORD_SUB_TYPE* pTrans_sub,TR_ADDITION_DATA_TYPE* pTrans_add,INT8U recvmode,INT8U onlyvoid);
extern INT16S gen_transaction_pay(char *commun_buffer,TRANSACTION_PAY trans);
extern INT16S Network_ClerkTransaction(HUGEDATA COMM_BUF *CBuf,ETH_CONNECT_LIST eth,unsigned char ClerkNum,INT16S id);
extern void send_clerk_sale_data(HUGEDATA COMM_BUF *CBuf,ETH_CONNECT_LIST eth,INT8U ClerkNum,INT16S id,TRANSACT_CHAIN *xtrans);
extern INT16S  PutSmallString(INT16S y, INT16S x,char *data);
extern void Report_WriteCurResetTime(INT32U rel_addr, INT16S period);
extern INT16S  ClearPluSummaryRecord(PLU_SALES_SUMMARY_TYPE* tp,INT16S counter);
extern INT16S  ClearSummaryRecord(SALES_SUMMARY_TYPE* tp,INT16S counter);
extern long PayChangeSummary(TRANSACTION_PAY trans_pay);
extern INT16U get_new_bitmapaddress(INT16U id,INT8U chk);
extern INT16U get_new_labeladdress(INT16U id,INT8U chk);
extern void Report_GetResetTime(INT32U rel_addr, RTC_STRUCT *time_str, INT8U period);
extern void transaction_pay_end(HUGEDATA COMM_BUF *CBuf,INT8U tail);
extern void transaction_pay_send_abort(HUGEDATA COMM_BUF *CBuf,INT8U tail);
extern INT16U plu_ptype_reset(void);
void  commun_recv_monitor(char key,INT16S xtype,INT16S v);
extern void plu_save_deptid_com_type(INT8U m,INT32U pnt,INT32U data,INT8U stype,INT16U usize);
extern INT32U ing_calc_addr_by_key(INT16U ingid);
extern void Flash_write_data(INT32U startAddr,INT16U memSize,HUGEDATA INT8U *value,INT8U v_leng,INT8U bitFlag,INT8U startBit,INT8U countBit,INT16U fcode);
//extern INT16U ingred_text_save(INT8U m,INT32U plukey,INT16U offset);
//extern INT32U GetIngredientTextAddress(INT16U ingred_no);
//extern void ingred_text_delete_temp(INT8U m,INT32U plukey,INT16U offset);
extern PRICE_TYPE Price;
extern INT16U key_pole_map[];
extern INT16U key_hanging_map[];
extern INT16U key_self_map[];
extern INT16U key_doublebody_map[];
extern TARE_OPER_TYPE		TareOper;
extern ROMDATA PARAMETER_SETTING_TYPE	ParameterSetting[];
extern NETWORK_CLERK_SALE_AMOUNT ClerkSaleAmount;
extern long	TicketTransactionNumber[];
extern long	CounterTicket;
extern INT16U   global_one2one_count;
extern char	gmsg[];
extern INT8U    GlbFlagPCnMasterConnection;
extern INT16S      NetCheckMasterConnection(void);

extern void commun_write_response(HUGEDATA COMM_BUF *CBuf, INT8U *str, INT8U errCode); //SG071204
extern INT8U commun_write_plu_data(HUGEDATA COMM_BUF *CBuf,INT8U *str,INT16S leng); //SG071204
//extern INT8U commun_send_plu_data(HUGEDATA COMM_BUF *CBuf,INT32U nth); //SG080107
extern INT8U commun_delete_plu_data(HUGEDATA COMM_BUF *CBuf,INT8U *str, INT16S leng); //SG080111

//extern INT32U dmsg_free_search(void);
//extern void dmsg_save(INT32U dmsgNo, char *dmsg, INT16U dmsgSize);
//extern void dmsg_erase(INT32U dmsgNo);
#ifdef USE_NHMART_SAFE_MEAT
static INT8U recv_individual_info(int param2, INT8U *str, int length);
#endif

TRANSACTION_PAY RecievedTransPay;

static INT32S get_raw_value(void);

//static prototype
//static INT8U commun_getrtc(INT16S cmd,INT8U *str);
static void recv_scale_status(HUGEDATA COMM_BUF *CBuf, INT16S param2,INT8U *str,INT16S length);
static void recv_network_lock(INT16S param2,INT8U *str,INT16S length);
static void recv_network_timesync(HUGEDATA COMM_BUF *CBuf,INT16S param2,INT8U *str,INT16S length);
static void recv_clerk_transaction(HUGEDATA COMM_BUF *CBuf,INT16S param2,INT8U *str,INT16S length);
#ifdef USE_IMPORT_MEAT_PROTOCOL
static INT8U recv_import_meat_data(HUGEDATA COMM_BUF *CBuf, int param2, INT8U *str, int length);
#endif
#ifdef USE_CHECK_INDIV_INFO
static INT8U recv_valid_individual_info(int param2, INT8U *str);
#ifdef USE_NHMART_SAFE_MEAT
static INT8U recv_individual_info(int param2, INT8U *str, int length);
#endif
#endif
#ifdef USE_SCANNER_FOR_ECO_FRIENDLY
static INT8U recv_valid_eco_info(int param2, INT8U *str);
#endif
static INT8U recv_transaction_sale(HUGEDATA COMM_BUF *CBuf,INT16S param2,INT8U *str,INT16S length);
static INT8U recv_transaction_pay(HUGEDATA COMM_BUF *CBuf,INT16S param2,INT8U *str,INT16S length);
static INT8U recv_change_unitprice(HUGEDATA COMM_BUF *CBuf,INT16S param2,INT8U *str,INT16S length);
static INT8U commun_recv_data(INT32U addr,INT8U *str,INT16S leng);
static INT8U recv_destination_ip(HUGEDATA COMM_BUF *CBuf, INT16S param2, INT8U *str, INT16S length);
#ifdef USE_INDIA_FUNCTION
static INT8U recv_inventory(HUGEDATA COMM_BUF *CBuf, INT16S param2, INT8U *str, INT16S length);
#endif
static INT8U label_reload(void);
static INT8U label_delete(HUGEDATA COMM_BUF *CBuf,INT32U spart,INT8U mode);
static INT8U plu_delete_discount(HUGEDATA COMM_BUF *CBuf,INT32U spart,INT8U mode);
static INT8U commun_plu_delete(HUGEDATA COMM_BUF *CBuf,INT32U spart,INT8U mode);
static INT8U commun_discount_reset(HUGEDATA COMM_BUF *CBuf,ETH_CONNECT_LIST eth,INT32U spart, INT8U cmd);
static INT8U commun_plu_reset(HUGEDATA COMM_BUF *CBuf,ETH_CONNECT_LIST eth,INT32U spart,INT8U mode);
static INT8U plu_table1_delete(HUGEDATA COMM_BUF *CBuf,INT32U spart,INT8U mode);
static INT8U plu_table2_delete(HUGEDATA COMM_BUF *CBuf,INT32U spart,INT8U mode);
static INT8U plu_table3_delete(HUGEDATA COMM_BUF *CBuf,INT32U spart,INT8U mode);
static INT8U plu_inhibit_reset(HUGEDATA COMM_BUF *CBuf,INT32U spart,INT8U mode);
static INT8U commun_report_delete(HUGEDATA COMM_BUF *CBuf,INT32U param,INT8U mode);
static INT8U commun_table2_reset(HUGEDATA COMM_BUF *CBuf,ETH_CONNECT_LIST eth,INT32U spart);
static INT8U commun_table3_reset(HUGEDATA COMM_BUF *CBuf,ETH_CONNECT_LIST eth,INT32U spart);
static INT8U commun_command_remote_key(INT16U param);
#ifdef USE_INDIA_FUNCTION
static INT8U plu_inventory_delete(HUGEDATA COMM_BUF *CBuf,INT32U spart,INT8U mode);
#endif
#ifdef USE_DIRECT_NUTRITION
static INT8U commun_direct_nutrition_delete( COMM_BUF *CBuf,INT32U spart,INT8U mode);
#endif

extern char	PeelHeadMsgFlag; 

//"I<action>A<cmd><0x0a>"
//"I<action>F<cmd>,<fnumber><0x0a>"
void commun_getinfo(HUGEDATA COMM_BUF *CBuf,INT8U action,INT32U cmd,INT32U fnumber)
{
	INT16S    n,ret;
	INT16S    r,start_r;
	char  *commun_buffer;
	INT8U  bcc;
	FW_VERSION fversion;
	INT16S	head, tail;
	INT8U ip[4];
	INT16U port;
	INT8U flag;
	INT8U string_buf[64];
	INT32U tarelimit;
	INT32S rawval;
	INT32U temp32u;
	char subversion[3];
	INT8U i;
#ifdef USE_MORNITORING_CNT
	INT32U temp_date;
	INT8U year, month, date;
#endif

	ret=TRUE;
	commun_buffer=(char *)serial_send_point;
	start_r = 9;
	r = start_r;
	sprintf((char *)&commun_buffer[r],"c=%03lX.",cmd); r+= 6;
	switch (cmd) {
		case 0x001: n=plu_struct_get_nfield();
			    sprintf((char *)&commun_buffer[r],"V=%04X.",n);
			    r+=7;
			    break;
		case 0x002: n=plu_rem_number_get();	// PLU 사용갯수
			    sprintf((char *)&commun_buffer[r],"V=%04X.",n);
			    r+=7;
			    break;
		case 0x003: sprintf((char *)&commun_buffer[r],"V=%04X.",5000);
			    r+=7;
			    break;
		case 0x004: ret=commun_getrtc(fnumber,(INT8U *)&commun_buffer[r]);
			    r+=strlen((char *)&commun_buffer[r]);
			    break;
		case 0x006: n = (INT16S)(MAX_PLU_NO - plu_rem_number_get());	// 남은 PLU 갯수
			    sprintf((char *)&commun_buffer[r],"V=%04X.",n);
			    r+=7;
			    break;
#ifdef USE_DIRECT_INGREDIENT
		case 0x007: //n = MAX_INGREDIENT_TEXT_NUMBER - GetIngredTextCnt();	// 남은 Direct Ingredient 갯수
			    n = MAX_INGREDIENT_TEXT_NUMBER - ingredient_text_count_empty();	// 남은 Direct Ingredient 갯수
			    sprintf((char *)&commun_buffer[r],"V=%04X.",n);
			    r+=7;
			    break;
#endif
		case 0x008: 							// 남은 실시간정산 갯수
			    head = get_nvram_wparam(NVRAM_SEND_REMAINDED_HEAD);
			    tail = get_nvram_wparam(NVRAM_SEND_REMAINDED_TAIL);
			    if (head >= tail) n = head - tail;
			    else              n = head + Startup.max_send_buf - tail;
			    n = Startup.max_send_buf - 1 - n;
			    sprintf((char *)&commun_buffer[r],"V=%04X.",n);
			    r+=7;
			    break;
		case 0x010: version_get(&fversion);
			    sprintf((char *)&commun_buffer[r],"V=%02X%02X%02X.",fversion.version,fversion.reversion[0],fversion.reversion[1]);
			    r+=strlen((char *)&commun_buffer[r]);
			    sprintf((char *)&commun_buffer[r],"S=%02X%02X.",fversion.subversion[0],fversion.subversion[1]);
			    r+=strlen((char *)&commun_buffer[r]);
			    sprintf((char *)&commun_buffer[r],"W=%02X%02X.",Startup.version[0],Startup.version[1]);
			    r+=strlen((char *)&commun_buffer[r]);
			    break;
		case 0x060: bcc=1;
			    if (ADM_GetData.Overload) bcc=2;
			    if (ADM_GetData.Zero) bcc=0;
			    sprintf((char *)&commun_buffer[r],"O=%d.",bcc);
			    r+=strlen((char *)&commun_buffer[r]);

			    bcc=0;
			    if (ADM_GetData.Stable) bcc=1;
			    sprintf((char *)&commun_buffer[r],"S=%d.",bcc);
			    r+=strlen((char *)&commun_buffer[r]);
#ifdef USE_UNDER_LIMIT_MINUS20D
				if(Operation.UnderWeight == ON)
					sprintf((char *)&commun_buffer[r],"W=------.");
				else
					sprintf((char *)&commun_buffer[r],"W=%06ld.",Price.Weight[PRICE_RESULT]);
#else
				sprintf((char *)&commun_buffer[r],"W=%06ld.",Price.Weight[PRICE_RESULT]);
#endif
			    r+=strlen((char *)&commun_buffer[r]);
			    sprintf((char *)&commun_buffer[r],"U=%1d.",ADM_GetData.CurUnit);
			    r+=strlen((char *)&commun_buffer[r]);
			    sprintf((char *)&commun_buffer[r],"w=%1d.",display_parameter.mode_dec[DISPLAY_WEIGHT]);
			    r+=strlen((char *)&commun_buffer[r]);
			    sprintf((char *)&commun_buffer[r],"p=%1d.",display_parameter.mode_dec[DISPLAY_PRICE]);
			    r+=strlen((char *)&commun_buffer[r]);
			    sprintf((char *)&commun_buffer[r],"D=%d.",status_scale.Plu.deptid);
			    r+=strlen((char *)&commun_buffer[r]);
			    sprintf((char *)&commun_buffer[r],"N=%ld.",status_scale.cur_pluid);
			    r+=strlen((char *)&commun_buffer[r]);
			    sprintf((char *)&commun_buffer[r],"P=%ld.",status_scale.cur_unitprice);
			    r+=strlen((char *)&commun_buffer[r]);
			    sprintf((char *)&commun_buffer[r],"Z=%ld.",Price.TotalPrice[PRICE_ORIGIN]);
			    r+=strlen((char *)&commun_buffer[r]);
			    if (ADM_GetData.Tare) {
				sprintf((char *)&commun_buffer[r],"T=A%06ld.",TareOper.tareKey+TareOper.tarePLU+TareOper.tareWeight);
				r+=strlen((char *)&commun_buffer[r]);
			    }
 			    if (Price.PercentTareWeight) {
				sprintf((char *)&commun_buffer[r],"T=P%06ld.",Price.PercentTareWeight);
				r+=strlen((char *)&commun_buffer[r]);
			    }
 			    break;
		case 0x040:
			    get_net_sparam(NETWORK_MACHINE_REMOTE_IP, ip, 4);
			    port = get_net_wparam(NETWORK_MACHINE_REMOTE_TCPPORT);
			    sprintf((char *)&commun_buffer[r],"I=%02X%02X%02X%02X.", ip[0], ip[1], ip[2], ip[3]);
			    r+=strlen((char *)&commun_buffer[r]);
			    sprintf((char *)&commun_buffer[r],"P=%04X.", port);
			    r+=strlen((char *)&commun_buffer[r]);
			    break;
		case 0x020://weight info 
				sprintf((char *)&commun_buffer[r],"W=%X.",Price.Weight[PRICE_RESULT]);
				r+=strlen((char *)&commun_buffer[r]);
				sprintf((char *)&commun_buffer[r],"T=%X.",TareOper.tareKey+TareOper.tarePLU+TareOper.tareWeight);
				r+=strlen((char *)&commun_buffer[r]);
				sprintf((char *)&commun_buffer[r],"D=%X.",display_parameter.mode_dec[DISPLAY_WEIGHT]);
				r+=strlen((char *)&commun_buffer[r]);
				sprintf((char *)&commun_buffer[r],"U=%X.",status_scale.cur_unitprice);
				r+=strlen((char *)&commun_buffer[r]);
				sprintf((char *)&commun_buffer[r],"t=%X.",Price.TotalPrice[PRICE_ORIGIN]);
				r+=strlen((char *)&commun_buffer[r]);								
				sprintf((char *)&commun_buffer[r],"d=%X.",display_parameter.mode_dec[DISPLAY_PRICE]);
				r+=strlen((char *)&commun_buffer[r]);
				break;
		case 0x021://printer info. 
				sprintf((char *)&commun_buffer[r],"I=%X.",SensorStruct.MinGapLastValue);
				r+=strlen((char *)&commun_buffer[r]);
				sprintf((char *)&commun_buffer[r],"A=%X.",SensorStruct.MaxGapLastValue);
				r+=strlen((char *)&commun_buffer[r]);
				sprintf((char *)&commun_buffer[r],"T=%X.",SensorStruct.GapThreshold);
				r+=strlen((char *)&commun_buffer[r]);
				sprintf((char *)&commun_buffer[r],"i=%X.",SensorStruct.MinPeelLastValue);
				r+=strlen((char *)&commun_buffer[r]);
				sprintf((char *)&commun_buffer[r],"a=%X.",SensorStruct.MaxPeelLastValue);
				r+=strlen((char *)&commun_buffer[r]);
				sprintf((char *)&commun_buffer[r],"t=%X.",SensorStruct.PeelThreshold);
				r+=strlen((char *)&commun_buffer[r]);
				break;
		case 0x022://current error info.
				if(PeelHeadMsgFlag == 1) flag = 1;
				else flag = 0;
				sprintf((char *)&commun_buffer[r],"H=%X.",flag);
				r+=strlen((char *)&commun_buffer[r]);
				if(PeelHeadMsgFlag == 2) flag = 1;
				else flag = 0;				
				sprintf((char *)&commun_buffer[r],"P=%X.",flag);
				r+=strlen((char *)&commun_buffer[r]);
				if(PrtDrvStruct.OutOfPaperFlag) flag =1;
				else flag = 0;			
				sprintf((char *)&commun_buffer[r],"E=%X.",flag);
				r+=strlen((char *)&commun_buffer[r]);			
				break;
		case 0x023://scale info.
				version_get(&fversion);
				i = 0;
				if(fversion.subversion[0] != '0') subversion[i++] = fversion.subversion[0];
				subversion[i++] = fversion.subversion[1];
				subversion[i] = 0;
				sprintf(string_buf, "V%c.%c%c.%s", fversion.version
														  , fversion.reversion[0]
														  , fversion.reversion[1]
														  , subversion);//V3.01.0
				sprintf((char *)&commun_buffer[r],"F=%02X.%s" ,strlen(string_buf),string_buf);//f/w version 													  
				r+=strlen((char *)&commun_buffer[r]);
		
				temp32u = ad_get_max_capa(ADM_Status.Capa, ADM_Status.CalUnit, 0);
				temp32u = ad_cnv_adm2main(temp32u);
				sprintf((char *)&commun_buffer[r],"A=%X.",temp32u);//max2 15kg
				r+=strlen((char *)&commun_buffer[r]);

				temp32u = ad_get_max_capa(ADM_Status.Capa, ADM_Status.CalUnit, 1);
				temp32u = ad_cnv_adm2main(temp32u); 			
				sprintf((char *)&commun_buffer[r],"a=%X.",temp32u);//max1 6kg
				r+=strlen((char *)&commun_buffer[r]);
				
				sprintf((char *)&commun_buffer[r],"I=%X.",status_scale.weight_min);//min 40g
				r+=strlen((char *)&commun_buffer[r]);
				
				temp32u = (INT32U)ad_get_interval(1);
				temp32u = ad_cnv_adm2main(temp32u); 
				sprintf((char *)&commun_buffer[r],"E=%X.",temp32u);//e2 5g
				r+=strlen((char *)&commun_buffer[r]);
				
				temp32u = (INT32U)ad_get_interval(0);
				temp32u = ad_cnv_adm2main(temp32u); 
				sprintf((char *)&commun_buffer[r],"e=%X.",temp32u);//e1 2g
				r+=strlen((char *)&commun_buffer[r]);																
				
				temp32u = get_ad_lparam(GLOBAL_AD_TARE_LIMIT);
				temp32u = ad_cnv_to_external(temp32u);
			    sprintf((char *)&commun_buffer[r],"T=%X.",temp32u);
			    r+=strlen((char *)&commun_buffer[r]);
				
				sprintf((char *)&commun_buffer[r],"D=%X.",display_parameter.mode_dec[DISPLAY_WEIGHT]);
				r+=strlen((char *)&commun_buffer[r]);
				break;
		case 0x024:
				rawval = get_raw_value();
				sprintf((char *)&commun_buffer[r],"Z=%X.",ADM_GetData.Zero);
				r+=strlen((char *)&commun_buffer[r]);
				sprintf((char *)&commun_buffer[r],"S=%X.",ADM_GetData.Stable);
				r+=strlen((char *)&commun_buffer[r]);
				sprintf((char *)&commun_buffer[r],"O=%X.",ADM_GetData.Overload);
				r+=strlen((char *)&commun_buffer[r]);
				sprintf((char *)&commun_buffer[r],"R=%X.",rawval);
				r+=strlen((char *)&commun_buffer[r]);
				break; 
#ifdef USE_MORNITORING_CNT
		case 0x025: // TPH info
				//PrtLoadTphUseData();
				//rtc_load_operated_time();

				temp_date = TphUseData.InspectDate;
				year = temp_date / 10000L;
				temp_date %= 10000L;
				month = temp_date / 100L;
				temp_date %= 100L;
				date = temp_date;
				
				sprintf((char *)&commun_buffer[r],"T=%04X.",TphUseData.TotalCnt);
				r+=strlen((char *)&commun_buffer[r]);
				sprintf((char *)&commun_buffer[r],"R=%04X.",TphUseData.AfterCnt);
				r+=strlen((char *)&commun_buffer[r]);
				sprintf((char *)&commun_buffer[r],"D=%02X%02X%02X.",year ,month ,date); 
				r+=strlen((char *)&commun_buffer[r]);
				break;
#endif
		case 0x005: 
		default   : ret=FALSE;
	}
	if (ret==FALSE) {
		sprintf(commun_buffer,"I%02X:E84%c",(INT8U)action,0x0a);
		r = strlen((char *)commun_buffer);
	} else {
		sprintf(commun_buffer,"I%02X:L%03X",(INT8U)action,r-start_r);
		commun_buffer[start_r-1]=',';
		bcc=gen_bcc((INT8U *)&commun_buffer[start_r],r-start_r);
		commun_buffer[r++]=bcc;
	}
	commun_outleng(CBuf,(char *)commun_buffer,r);
}

//"i<action>F<cmd>,<fnumber>L<length>:<data><bcc>" or "j<action>F<cmd>,<fnumber>L<length>:<data><bcc>"
//"i<action>F<cmd>,<fnumber>N<length>:<data><bcc>" or "j<action>F<cmd>,<fnumber>N<length>:<data><bcc>"
void commun_setinfo(HUGEDATA COMM_BUF *CBuf,INT8U action,INT16U cmd,INT32U fnumber,INT32U length,INT8U *str)
{
	INT16S  ret;

	recv_counter=0;
	ret=1;
	cmd&=0x0ff;
	switch (cmd) {
		case 0x065:
		case 0x004: ret=commun_setrtc(str,(INT16S)length);
			    if (cmd==0x65) goto NOAC;
			    break;
		case 0x010: recv_scale_status(CBuf,(INT16S)fnumber,str,(INT16S)length);	//Added by JJANG 20080307
			    goto NOAC;
		case 0x015: recv_network_lock((INT16S)fnumber,str,(INT16S)length); // Fnumber=1...
			    goto NOAC;
		case 0x016: recv_network_timesync(CBuf,(INT16S)fnumber,str,(INT16S)length);
			    goto NOAC;
		case 0x017: recv_clerk_transaction(CBuf,(INT16S)fnumber,str,(INT16S)length);
			    goto NOAC;
		case 0x025:
	      		    goto NOAC;
		case 0x026:
			    goto NOAC;
#ifdef USE_IMPORT_MEAT_PROTOCOL
		case 0x030: recv_import_meat_data(CBuf,(int)fnumber,str,(int)length);
			    goto NOAC;
#endif
#ifdef USE_CHECK_INDIV_INFO
		case 0x031: recv_valid_individual_info((int)fnumber,str);
			    goto NOAC;
#else
		case 0x031:
			    goto NOAC;
#endif
#ifdef USE_NHMART_SAFE_MEAT
		case 0x032: recv_individual_info((int)fnumber,str,(int)length);	//농협 안심한우 개체 조회 시
			    goto NOAC;
#else
		case 0x032:
			    goto NOAC;
#endif
#ifdef USE_SCANNER_FOR_ECO_FRIENDLY
		case 0x033: recv_valid_eco_info((int)fnumber,str);
			    goto NOAC;
#endif
		case 0x035:
		case 0x034:
		case 0x036:
			    goto NOAC;
		case 0x070: recv_transaction_sale(CBuf,(INT16S)fnumber,str,(INT16S)length);
			    goto NOAC;
		case 0x071: recv_transaction_pay(CBuf,(INT16S)fnumber,str,(INT16S)length);
			    goto NOAC;
		case 0x080: recv_change_unitprice(CBuf,(INT16S)fnumber,str,(INT16S)length);
			    goto NOAC;
		case 0x050: ret=commun_recv_data((INT32U)fnumber,str,(INT16S)length);
			    break;
		case 0x040: ret = recv_destination_ip(CBuf, fnumber, str, (INT16S)length);
			    break;
#ifdef USE_INDIA_FUNCTION
		case 0x078: recv_transaction_sale(CBuf,(INT16S)fnumber,str,(INT16S)length);
			    goto NOAC;
		case 0x079: recv_inventory(CBuf, (INT16S)fnumber, str, (INT16S)length);
			    goto NOAC;
#endif
		default   : ret=FALSE;
	}
	fnumber=0;
	commun_write_err_info(CBuf,(INT8U)action,cmd,ret);
NOAC:;
}

//"Cx<action>F<cmd>,<param><0x0a>"
//"Cx<action>A<cmd><0x0a>"
void commun_command(HUGEDATA COMM_BUF *CBuf,ETH_CONNECT_LIST eth,INT32U action,INT32U cmd,INT32U param)
{
	INT8U ret;
	INT16S   p;
	char  *commun_buffer;
	INT16S   r;
	char str[32];
       	INT16U part, id;

	ret=0;
	recv_counter++;
//sprintf(MsgBuf, "<SRC>cmd = %02lX, param=%08lx\r\n", cmd, param);
//MsgOut(MsgBuf);	// CJK090105
	switch (cmd) {
		case 0x001: plu_struct_set_base();   break; // Set Default PLU Structure
		//case 0x002: plu_all_delete(CBuf,1);  break; // CxxA02<0x0a>
		case 0x002: plu_all_delete(CBuf,2);  break; // CxxA02<0x0a>
							    // return Cxx:C<number><0x0a>
		case 0x003: PrtFlashSetDefault();    break; // Set Printer Default
		case 0x004: KEY_CopyKeyTable(0xffff);    break; // Set Default Key Table
		case 0x005: ret=label_reload(); break; // ret=label_reload(CBuf); break;
		case 0x006:
		case 0x007: ret=label_delete(CBuf,(INT32U)param,(INT8U)cmd);
			    break;
		case 0x019:
		case 0x009: 
		case 0x010: ret=plu_delete_discount(CBuf,(INT32U)param,(INT8U)cmd); break; 
		case 0x012:
		case 0x013:
		case 0x015: ret=commun_plu_delete(CBuf,(INT32U)param,(INT8U)cmd); break;
		case 0x017:// delete reset
		case 0x018: ret=commun_discount_reset(CBuf,eth,(INT32U)param,(INT8U)cmd); break;
		case 0x016:
		case 0x014: ret=commun_plu_reset(CBuf,eth,(INT32U)param, (INT8U)cmd); break; 
		case 0x020:
		case 0x021: ret=plu_table1_delete(CBuf,(INT32U)param,(INT8U)cmd); break;
		case 0x030:
		case 0x031: ret=plu_table2_delete(CBuf,(INT32U)param,(INT8U)cmd); break;
		case 0x032:
		case 0x033: ret=plu_table3_delete(CBuf,(INT32U)param,(INT8U)cmd); break; 
		case 0x036: ret=plu_inhibit_reset(CBuf,(INT32U)param,(INT8U)cmd); break; 
		case 0x042:
		case 0x043: ret=commun_report_delete(CBuf,(INT32U)param,(INT8U)cmd); break;
		case 0x044: ret=keyapp_print(OFF); break;// remote printing
#ifdef USE_DIRECT_NUTRITION
		case 0x047:
		case 0x048: ret=commun_direct_nutrition_delete(CBuf,(INT32U)param,(INT8U)cmd); break; 
#endif
		case 0x077: ret=commun_table2_reset(CBuf,eth,(INT32U)param); break;
		case 0x078: ret=commun_table3_reset(CBuf,eth,(INT32U)param); break;
		case 0x079: ret=commun_command_remote_key((INT16U) param); break;
#ifdef USE_INDIA_FUNCTION
		case 0x040:
		case 0x041: ret=plu_inventory_delete(CBuf, (INT32U)param, (INT8U)cmd); break;
#endif
		//case 0x080:	// reserving for CL7000(Mr. Yang)        
		case 0x099: ;
	}
	if (network_status.service_flag&0x20)	// send plu
	{
		if (ethernet_list.status == 1)	// 전송된 data는 master일 때만 sync flag 설정
		{
			part = (INT16S)(param>>16);
			id  =  (INT16S)(param&0xffff);
			if (cmd == 0x31 && part == 1) // ingredient
			{
				commun_net_mask(2, &id, 0xff, 1);
			}
			else if (cmd == 0x33 && part == 3) // scroll msg.
			{
				commun_net_mask(3, &id, 0xff, 1);
			}
		}
	}
#ifdef USE_MAX_SCALE_NO_255
	if ((eth.scaleid >= 1) && (eth.scaleid <= 255)) //from scale 
#else
	if ((eth.scaleid >= 1) && (eth.scaleid <= MAX_SCALE - 1)) //from scale 
#endif
	{
		if (cmd==0x15 && ret == 0) 	// plu delete
		{ 
			commun_buffer = (char *)serial_send_point;
			r = ethernet_gen_head(commun_buffer);
			sprintf(&commun_buffer[r], "C02F16,%08lX%c", param, 0x0a);	// part:1byte, id:2byte form="%02X%04X"
			commun_outleng(CBuf, commun_buffer, strlen(commun_buffer));
			return;
		}
		else if (cmd == 0x19 && ret == 1)
		{ 
			commun_buffer = (char *)serial_send_point;
			r = ethernet_gen_head(commun_buffer);
			sprintf(&commun_buffer[r], "C02F17,%08lX%c", param, 0x0a);
			commun_outleng(CBuf, commun_buffer, strlen(commun_buffer));
			return;
		}
		else if (cmd == 0x31 && ret == 1)
		{ 
			commun_buffer = (char *)serial_send_point;
			r = ethernet_gen_head(commun_buffer);
			sprintf(&commun_buffer[r], "C02F77,%06lX%c", param, 0x0a);
			commun_outleng(CBuf, commun_buffer, strlen(commun_buffer));
			return;
		}
		else if (cmd == 0x33 && ret == 1)
		{  
			commun_buffer = (char *)serial_send_point;
			r = ethernet_gen_head(commun_buffer);
			sprintf(&commun_buffer[r], "C02F78,%06lX%c", param, 0x0a);
			commun_outleng(CBuf, commun_buffer, strlen(commun_buffer));
			return;
		}
	}
	sprintf((char *)str,"C%03lX:O%02lX",action,cmd);
	p=strlen(str);
	if (ret>=0x80) {
//		str[5]='E';
		commun_write_err16(CBuf,(INT8U)cmd,ret,1,OFF);
	} else {
		p=strlen(str);
		sprintf(&str[p],"%c%c",0x0a,0x0a);
		commun_out(CBuf,(HUGEDATA char *)str);
	}
}

void recv_scale_status(HUGEDATA COMM_BUF *CBuf, INT16S param2,INT8U *str,INT16S length)
{
	ETH_CONNECT_LIST eth;
	INT16S status,r;
	INT32U v32;
	INT8U code;

	status=0;
	r=0;
	memset((INT8U *)&eth,0,sizeof(ETH_CONNECT_LIST));
	while (1) 
	{
		if (r>=length) break;
		switch (status) 
		{
			case 0: v32=0;
				code=str[r];
				if (code==0) goto END;
				status=1;
				break;
			case 1: status=0;
				if (str[r]=='=') status=2;
				break;
			case 2: if (str[r]=='.') 
				{
					commun_common_ethernet(code,&eth,v32);
					status=0;
				} 
				else 
				{
					v32<<=4;
					v32|=ctoh(str[r]);
				}
		}
		r++;
	}
END:
	if (ethernet_list.status == 1)	//Master
	{
		switch (param2) 
		{
		   	case 0x01:
#ifdef _USE_DBG_CHECK_MASTER
sprintf(MsgBuf, "[MC] %02x:%02x:%02x RECV [%d.%d.%d.%d]\r\n", RTCStruct.hour, RTCStruct.min, RTCStruct.sec, eth.ip[0], eth.ip[1], eth.ip[2], eth.ip[3]);
MsgOut(MsgBuf);
#endif
		   	        command_connection_data(CBuf,0x02);	//master --> slave
		   		break;
		}
	}
	if (ethernet_list.status == 2 || ethernet_list.status == 4)	//Slave
	{
		switch (param2) 
		{
		   	case 0x02:
				network_status.master_check = 1;
		   		break;
		}
	}
	return;
}

void recv_network_lock(INT16S param2,INT8U *str,INT16S length) // 0x15
{
	ETH_CONNECT_LIST eth;
	INT16S status,r;
	INT32U v32;
	INT8U code;

	status=0;
	r=0;
	memset((INT8U *)&eth,0,sizeof(ETH_CONNECT_LIST));
	while (1) {
		if (r>=length) break;
		switch (status) {
			case 0: v32=0;
				code=str[r];
				if (code==0) goto END;
				status=1;
				break;
			case 1: status=0;
				if (str[r]=='=') status=2;
				break;
			case 2: if (str[r]=='.') {
					commun_common_ethernet(code,&eth,v32);
					status=0;
				} else {
					v32<<=4;
					v32|=ctoh(str[r]);
				}
		}
		r++;
	}
END:
#ifdef NOT_USE_NETSET_LOCK
	status_scale.scale_lock = 0;
	set_nvram_bparam(NVRAM_SCALE_LOCK,status_scale.scale_lock);
	return;
#endif
	switch (param2) {
	   case 0x01: status_scale.scale_lock=1; break;
	   case 0x02: status_scale.scale_lock=0; break;
	}
	if ((param2>0) && (param2<3)) {
		set_nvram_bparam(NVRAM_SCALE_LOCK,status_scale.scale_lock);
	}
	return;
}

// "i00F016..."
void  recv_network_timesync(HUGEDATA COMM_BUF *CBuf,INT16S param2,INT8U *str,INT16S length)
{
	INT16S l;
	INT8U bcc;
	ETH_CONNECT_LIST eth;
	char  *commun_buffer;

	commun_buffer=(char *)serial_send_point;
	commun_recv_eth_header(&eth,(char *)str,length);
	switch (param2) {
		case 0x01: // Request Time
			l = 13;
			l+=ethernet_gen_head(&commun_buffer[13]);
			commun_getrtc(3,(INT8U *)&commun_buffer[l]);
			l+=strlen((char *)&commun_buffer[l]);
			sprintf(commun_buffer,"i00F065,L%03X",l-13);
			commun_buffer[12]=':';
			bcc=gen_bcc((INT8U *)&commun_buffer[13],l-13);
			commun_buffer[l++] = bcc;
			commun_buffer[l]=0;
			commun_outleng(CBuf,commun_buffer,l);
			break;
	}
}

//"i00F017"
void recv_clerk_transaction(HUGEDATA COMM_BUF *CBuf,INT16S param2,INT8U *str,INT16S length)
{
	ETH_CONNECT_LIST eth;
	INT32U v32;
	INT16S status,r,nth;
	INT8U  clerk,code;
	TRANSACTION_RECORD_TYPE trans;
	char *tp;
	INT32U ticketno;
	INT8U result_reopen;
	INT16S nrec,i;
	INT16U bufdata_count = 0;
	INT8U ret_value = 0;
	CAPTION_STRUCT 	cap;
	STRUCT_TOTAL_PRICE totalData;
	NETWORK_CLERK_SALE_AMOUNT clerkAmount;

	memset(&clerkAmount,0, sizeof(NETWORK_CLERK_SALE_AMOUNT));

  	status = 0;
	r      = 0;
	ticketno = 0L;
	result_reopen = 0;
	memset((INT8U *)&eth,0,sizeof(ETH_CONNECT_LIST));
	memset((INT8U *)&trans,0,sizeof(TRANSACTION_RECORD_TYPE));
	nrec=0;
	nth =0;
	while (1) {
		if (r>=length) break;
		switch (status) {
			case 0: v32=0;
				code=str[r];
				if (code==0) goto END;
				status=1;
				break;
			case 1: status=0;
				if (str[r]=='=') status=2;
				break;
			case 2: if (str[r]=='.') 
				{
					switch (code) {
			   		    case 'C': clerk    =(INT8U)v32; break;
					    case 'N': nth      =(INT16S)v32;   break;
					    case '#': ticketno =(INT16S)v32;   break;
					    case 'P': result_reopen = (INT8U)v32; break;
					    case 'Q': clerkAmount.totalCount = (INT16S)v32;  break;
					    case 'R': clerkAmount.totalPrice = (long)v32; break;
					    case 'S': clerkAmount.voidCount  = (INT16S)v32;  break;
					    case 'T': clerkAmount.voidPrice  = (long)v32; break;					    
					    case 'U': clerkAmount.tareValue  = (long)v32; break;
					    case 'V': clerkAmount.customerNo  = (INT16S)v32; break;
					    case 'X': nrec = (INT16S)v32;
						      tp   = (char *)&str[r+1];
					    	      goto END;
					    default : if (!commun_common_ethernet(code,&eth,v32)) 
					    		   inter_transaction_sale(code,&trans,v32);
					              break;
					}
					status=0;
				} else {
					v32<<=4;
					v32|=ctoh(str[r]);
				}
		}
		r++;
	}
END:
//sprintf(MsgBuf, "[i00F017] code=%02X, clerk=%d, nth=%d, tNo=%ld\r\n", param2, clerk, nth, ticketno);
//MsgOut(MsgBuf);  
	switch (param2) {
		case 0x01: // Master
		//case 0x21:
			if ( netGetClerkLock(clerk, eth.scaleid) ) //Clerk이 Lock 일경우
			{
				command_login_loaddata(CBuf,0x07,clerk,NULL);
			}
			else
			{
				ClerkTotalPrice(0, clerk, &totalData);
				clerkAmount.totalCount = totalData.TotalCount;
				clerkAmount.totalPrice = totalData.TotalPrice;
				clerkAmount.voidCount = totalData.VoidCount;
				clerkAmount.voidPrice = totalData.VoidPrice;
				clerkAmount.tareValue = get_nvram_lparam(NVRAM_CLERK_TARE + 4 * clerk);
				clerkAmount.customerNo = get_nvram_wparam(NVRAM_CLERK_CUSTOMER_NO + clerk*2);

				code=(INT8U)param2;
				code++;
				command_login_loaddata(CBuf,code,clerk,&clerkAmount);
			}
			break;
		//case 0x22:
		case 0x02: // Slave
			network_status.login_flag    = 0;
			network_timeout_reset(NETWORK_TIMEOUT_REQUEST_LOGIN);
			network_counter_reset(NETWORK_TIMEOUT_REQUEST_LOGIN);

			if (param2==0x02) {
				ClerkSaleAmount = clerkAmount;
				set_nvram_lparam(NVRAM_CLERK_TARE + 4 * clerk, clerkAmount.tareValue);
				set_nvram_wparam(NVRAM_CLERK_CUSTOMER_NO + clerk*2, clerkAmount.customerNo);
			} else {
				network_status.load_lastsale = 2;
				network_status.check_lastscale=1;
				if (eth.ip[0]==0) network_status.check_lastscale=2;
				if (memcmp(eth.ip,ethernet_list.localip,4)==0) network_status.check_lastscale=2;
			}
			network_status.load_lastsale = 2;
			break;
		case 0x63: //master. 0x03 에 대한 Retry 
			netSetClerkLock(clerk, eth.scaleid, OFF);
		case 0x03: //master
			if ( netGetClerkLock(clerk, eth.scaleid) ) //Clerk이 Lock 일경우
			{
				command_login_loaddata(CBuf,0x07,clerk,NULL);
			}
			else 
			{
				netSetClerkLock(clerk, eth.scaleid, ON);			
				Network_ClerkTransaction(CBuf,eth,clerk,nth);
			}
			break; 
		case 0x06: //slave
			network_status.load_lastsale = 4;
			if (nth < network_status.request_clerksaleid)	// 중복 데이터 // CJK081126 Double registration 방어(LRE)
			{
				break;
			}
			network_timeout_reset(NETWORK_TIMEOUT_REQUEST_LOGIN);
			network_counter_reset(NETWORK_TIMEOUT_REQUEST_LOGIN);
			for (i=0; i<nrec; i++) {
#ifdef USE_CHN_CART_SCALE
				memcpy(&trans,tp,(sizeof(TRANSACTION_RECORD_TYPE)-TRACE_CODE_SIZE));
				tp+=(sizeof(TRANSACTION_RECORD_TYPE)-TRACE_CODE_SIZE);
#else
				memcpy(&trans,tp,sizeof(TRANSACTION_RECORD_TYPE));
				tp+=sizeof(TRANSACTION_RECORD_TYPE);
#endif
				ClerkAddTransaction(clerk,&trans,1,0);
			}
			goto N4;
		case 0x04:
			if (nth < network_status.request_clerksaleid)	// 중복 데이터
			{
				break;
			}
			ClerkAddTransaction(clerk,&trans,1,0);
			LocalPluTotalSummaries(status_scale.scaleid,&trans,NULL,NULL,1,0);
		N4:	network_status.request_clerksaleid=nth+1;
			network_timeout_reset(NETWORK_TIMEOUT_REQUEST_LOGIN);
			network_counter_reset(NETWORK_TIMEOUT_REQUEST_LOGIN);
			command_login_loaddata(CBuf,0x33,clerk,NULL);
			break;
		case 0x05: //slave
			if (ticketno) {
				set_TicketTransactionNumber(clerk, (long)ticketno);
			}
			network_status.load_lastsale = 2;
			break;
		case 0x07:  // Slave : if clerk is locked
			network_status.load_lastsale = 3; 		
			break;
		case 0x10: //master- Clerk TR Delete
			if(GlbOper.realtimeTransactionType) // param 620 : 0(not use real time transaction)
			{
				bufdata_count = GetClerkTransactionCount(clerk);
				ret_value = GetCheckAddTransactionData(0,bufdata_count,AUDIT_MODE_LINEAR);		//mode : sale
				if(ret_value) ClerkTransactionSaleStore(clerk);
			}
			ReopenedTransPay = RecievedTransPay;
			ClerkDeleteTransaction(clerk);
			memset(&RecievedTransPay, 0, sizeof(TRANSACTION_PAY));
			command_login_loaddata(CBuf,0x11,0,NULL);
			break;
		case 0x11: 
			network_status.clerk_saledelete=0;
			break;
		case 0x30: //master- Clerk TR Delete(Master-PC Disconnection)
			if(GlbOper.realtimeTransactionType) // param 620 : 0(not use real time transaction)
			{
				bufdata_count = GetClerkTransactionCount(clerk);
				ret_value = GetCheckAddTransactionData(0,bufdata_count,AUDIT_MODE_LINEAR);		//mode : sale
				if(ret_value) 
				{
					GlbFlagPCnMasterConnection = OFF;
					ClerkTransactionSaleStore(clerk);
					GlbFlagPCnMasterConnection = ON;
				}
			}
			ReopenedTransPay = RecievedTransPay;
			ClerkDeleteTransaction(clerk);
			memset(&RecievedTransPay, 0, sizeof(TRANSACTION_PAY));
			command_login_loaddata(CBuf,0x31,0,NULL);
			break;
		case 0x31: 
			network_status.clerk_saledelete=0;
			break;
		case 0x33: //master - next data
			Network_ClerkTransaction(CBuf,eth,clerk,nth);
			break;
		case 0x43: //master - unlock
			code=(INT8U)param2;
			code++;
			netSetClerkLock(clerk, eth.scaleid, OFF);	 
			command_login_loaddata(CBuf,code,clerk,NULL);
			break;
		case 0x44: //slave
			network_status.load_lastsale = 2;
			break;
		case 0x70: //master - check connection to PC
			clerk = 0xff - (INT8U)NetCheckMasterConnection();
			command_login_loaddata(CBuf,0x71,clerk,NULL);
			break;
		case 0x71: //slave - 
			GlbFlagPCnMasterConnection = 0xff - clerk;
			network_status.clerk_saledelete=0;
			break;
		case 0x50: //master - set clerk tare
			set_nvram_lparam(NVRAM_CLERK_TARE + 4 * clerk, clerkAmount.tareValue);
			command_login_loaddata(CBuf,0x51,clerk,NULL);
			break;
		case 0x51: //slave - ack "0x50:set clerk tare"
			network_status.clerk_saledelete=0;
			break;
		case 0x60: //master - set clerk customer no
			set_nvram_wparam(NVRAM_CLERK_CUSTOMER_NO+clerk*2 ,clerkAmount.customerNo);
			command_login_loaddata(CBuf,0x61,clerk,NULL);
			break;
		case 0x61: //slave - ack "0x60:set clerk customer no"
			network_status.clerk_saledelete=0;
			break;
		case 0x65: //master - return reopen result
			result_reopen = GetClerkReopenTransaction(ticketno, clerk);
			command_reopen_data(CBuf, 0x66, ticketno, clerk, result_reopen);
			flagcopy_ticket = OFF;
			break;
		case 0x66: //slave - ack "0x66: "
			network_status.reopen_status = result_reopen;
			break;
	}
}

#ifdef USE_IMPORT_MEAT_PROTOCOL
#define MT_MAX_PARAM_COUNT	2
#define MT_MAX_PARAM_LENGTH	64
extern INT8U keyapp_call_individual(INT16U indiv_index, INT8U beep);
// "i00F030..N"
INT8U recv_import_meat_data(HUGEDATA COMM_BUF *CBuf, int param2, INT8U *str, int length)
{
	int status,r;
	INT8U i,j;
	INT8U param_str[MT_MAX_PARAM_COUNT][MT_MAX_PARAM_LENGTH];
	INT16U ind_idx;
	INT8U rcv_flag = 0;
	char string_buf[50];
	INT32U rcv_count;
	INT16U tail;
	INT32U addr;
	TRANSACTION_SALE_DATA_TYPE trans_sdt;

	rcv_count = 0;
	status = 0;
	r = 0;
	i = 0;
	j = 0;
	while (1) 
	{
		if (i > MT_MAX_PARAM_COUNT) break;
		if (j > MT_MAX_PARAM_LENGTH) break;
		if (r >= length) break;
		else if (str[r] == 0) break;
		else if (str[r] == '|')
		{
			param_str[i][j++] = 0;
			i++;
			j = 0;
		}
		else
		{
			param_str[i][j++] = str[r];
		}
		r++;
	}
	rcv_flag = param_str[0][0] - '0';

	switch (param2) {
#ifdef USE_EXT_CHECKING_HARMFULNESS            
		case 0x01:
			if (rcv_flag) 	//위해개체
			{
				rcv_flag = 0;
				BuzOn(4);
				sprintf(string_buf, "담당자 확인 필요 상품");
				//sprintf(string_buf, "위해 쇠고기입니다");
				display_message_page_mid(string_buf);
				sale_display_indivFlag_set(INDIV_DISP_MODE_INDIV_NO, OFF);
			}
			else 		//정상개체, 개체 식별 번호 display
			{
				BuzOn(1);
				memcpy(CalledTraceStatus.indivStr.individualNO, &param_str[1][0], sizeof(CalledTraceStatus.indivStr.individualNO));
				individual_checkZero(CalledTraceStatus.indivStr.individualNO);
				ind_idx = individualData_search(CalledTraceStatus.indivStr.individualNO, 0);
				keyapp_call_individual(ind_idx, OFF);
			}
			// Send ACK(cmd02)
			command_import_meat_send(CBuf, 0x02, 0); //checksum ok
			break;
		case 0x04:
			if (rcv_flag) network_status.rcv_mt = 2;	//위해개체
			else network_status.rcv_mt = 1; 		//정상개체
			break;
#endif            
		case 0x06:
			if (rcv_flag)	//Checksum err
			{
			}
			else
			{
				rcv_count = atol((char *)param_str[1]);
				tail = get_nvram_wparam(NVRAM_SEND_REMAINDED_TAIL);

				addr=tail; 
				addr%=Startup.max_send_buf;
				addr*=MAX_TRANSACTION_TYPE4_SALE_SIZE;
				addr+=Startup.addr_send_buf;
				get_nvram_sparam(addr,(INT8U *)&trans_sdt,sizeof(TRANSACTION_SALE_DATA_TYPE));

				if (trans_sdt.sdtTransactionCounter == rcv_count)
				{
					tail++;
					tail%= Startup.max_send_buf;
					set_nvram_wparam(NVRAM_SEND_REMAINDED_TAIL,tail);
				}
			}
			break;
	}
	sale_display_update(UPDATE_ALL|UPDATE_CLEARALL);	
	return 0;
}
#endif

#ifdef USE_CHECK_INDIV_INFO
INT8U recv_valid_individual_info(int param2, INT8U *str)
{	
	if(param2 == 0x02)
	{
		network_status.valid_indiv = atoi(str);
	}
	return 0;
}
//CBuf : 통신으로 받은 data, parm2 : command, str : data block 부분, length : serial buffer
#ifdef USE_NHMART_SAFE_MEAT
extern TRACE_INDIVIDUAL_IDENTITY indivInfo;
INT8U recv_individual_info(int param2, INT8U *str, int length)
{
	INT8U j = 0, r = 0;
	INT8U error = 0;
	INT8U state = 0;
	INT8U size = 0;
	INT8U result = 0;
	INT8U fixed = 0;	//고정 자리수인 경우. 1
	if(param2 != 2)
	return;
	memset(&indivInfo, 0, sizeof(indivInfo));	//임시 개체 저장 공간 초기화. 
	while (error == 0) 
	{
		if (r >= length) break;	//정상 탈출
		else if (str[r] == 0) error = 1;
		else
		{
			if(str[r] == '|') 
			{
				if(fixed && (j < size))
				{
					error = 1;
					break;
				}
				state++;
				r++;
				j = 0;
				fixed = 0;
				continue;
			}
			else
			{
				j++;	//자리 인덱스
			}
			switch(state)
			{
				case 0:	//통신 조회 결과
					result += ctod(str[r]);
					if(j < 2) result *= 10;
					fixed = 1;
					size = 2;
					break;
				case 1:	//묶음구분
					indivInfo.lotFlag = ctod(str[r]);
					fixed = 1;
					size = 1;
					break;
				case 2:	//개체번호
					indivInfo.individualNO[j - 1] = str[r];
					size = 20;
					break;
				case 3:	//사육지
					indivInfo.farm[j - 1] = str[r];
					size = 48;
					break;
				case 4:	//품종
					indivInfo.breed[j - 1] = str[r];
					size = 24;
					break;
				case 5:	//등급번호
					indivInfo.gradeNo += ctod(str[r]);
					if(j < 2) indivInfo.gradeNo *= 10;
					fixed = 1;
					size = 2;
					break;
				case 6:	//도축장
					indivInfo.slaughtHouse[j - 1] = str[r];
					if(indivInfo.lotFlag)
					{
						size = 100;
					}
					else
					{
						size = 48;
					}
					break;
				case 7:	//도축일자 (년)
					if(!indivInfo.lotFlag)
					{
						indivInfo.slaughtDate[0] += ctod(str[r]);
						if(j < 2) indivInfo.slaughtDate[0] *= 10;
						fixed = 1;
						size = 2;
					}
					break;
				case 8:	//도축일자 (월)
					if(!indivInfo.lotFlag)
					{
						indivInfo.slaughtDate[1] += ctod(str[r]);
						if(j < 2) indivInfo.slaughtDate[1] *= 10;
						fixed = 1;
						size = 2;
					}
					break;
				case 9:	//도축일자 (일)
					if(!indivInfo.lotFlag)
					{
						indivInfo.slaughtDate[2] += ctod(str[r]);
						if(j < 2) indivInfo.slaughtDate[2] *= 10;
						fixed = 1;
						size = 2;
					}
					break;
			}
			if(j > size)	//변수 초과.
			{
				error = 1;
			}
		}
		r++;	//다음 자리 해석
	}
	if(error == 1)
	{
		network_status.indivInfo = 10;	//프로토콜 오류
	}
	else
	{
		network_status.indivInfo = result;
	}
	return;
}
#endif 
#endif
#ifdef USE_SCANNER_FOR_ECO_FRIENDLY
INT8U recv_valid_eco_info(int param2, INT8U *str)
{	
	if(param2 == 0x02)
	{
		network_status.ecoInfo= str[0] - '0';
	}
	return 0;
}
#endif

// "i00F070..."
// "i00F078..."
INT8U recv_transaction_sale(HUGEDATA COMM_BUF *CBuf,INT16S param2,INT8U *str,INT16S length)
// =====================================================
// Login 후 Sale Data 의 전송시만 사용.
// Transaction 시만 사용함.
// =====================================================
{
	ETH_CONNECT_LIST eth;
	TRANSACTION_RECORD_TYPE trans;
	INT16S status,r,z, x;
#ifdef USE_REALTIME_TRANS_TYPE3
	INT16S y;
#endif
	INT32U v32;
	//INT32U ticketno;
	INT8U code,clerk;
	INT16U tail;
	INT8U ret;
	INT16U prevSTail;
#ifdef USE_MAX_SCALE_NO_255
	INT16U prevSTail2;
#endif

	status=0;
	r     =0;
	tail  =0;

	//ticketno=0L;
	memset((INT8U *)&eth,0,sizeof(ETH_CONNECT_LIST));
	memset(&trans,0,sizeof(TRANSACTION_RECORD_TYPE));
#ifdef USE_CHN_CART_SCALE
	z = sizeof(TRANSACTION_RECORD_TYPE) - TRACE_CODE_SIZE;
#else
	z = sizeof(TRANSACTION_RECORD_TYPE);
#endif
#ifdef USE_REALTIME_TRANS_TYPE3
	y = sizeof(REALTIME_TRANSACTION_TYPE);
#endif
	x = 20;
	while (1) {
		if (r>=length) break;
		switch (status) {
			case 0: v32=0;
				code=str[r];
				if (code==0) goto END;
				status=1;
				break;
			case 1: status=0;
				if (str[r]=='=') status=2;
				break;
			case 2: if (str[r]=='.') {
					switch (code) {
					    case 'C': clerk=(INT8U)v32; break;
					    case 'T': tail =(INT16U)v32;
						      network_status.rcv_tail = tail;
						      if (param2 == 0x03 || param2 == 0x13) 
						      {
							      if (r + z + 1 == length) 
							      {
							      	   memcpy(&trans, &str[r+1], z);
								   goto END;
							      }
							#ifdef USE_REALTIME_TRANS_TYPE3
							      else if (r + z + y + 1 == length)
							      {
							      	   memcpy(&trans, &str[r+1], z);
							      	   //REALTIME_TRANSACTION_TYPE 생략
								   goto END;
							      }
							#endif
						      }
						      else if (param2 == 0x23 || param2 == 0x33)
						      {
							      if (r + z + x + 1 == length) 
							      {
							      	   memcpy(&trans, &str[r+1], z);
								   //individual no 생략
								   //memcpy(&indiv, &str[r+z+1], x);
								   goto END;
							      }
						      }
					    	      break;
					    //case '#': ticketno=v32; break;
					    default : if (!commun_common_ethernet(code,&eth,v32)) {
					    		   inter_transaction_sale(code,&trans,v32);
						      }
					    	      break;
					}
					status=0;
				} else {
					v32<<=4;
					v32|=ctoh(str[r]);
				}
		}
		r++;
	}
END:

	// ================================================
	// Param2 = 2 일때전송 완료.clerkid=0,
	// Param2 = 1 일때수신중 , 
	// ================================================
	switch (param2) {
		case 0x03: //master
		case 0x01: 
		case 0x13: // restore
		case 0x23: // realtime_transaction_type_2 
		case 0x33: // restore : realtime_transaction_type_2
			if (tail) {
				// 조건의 유효성 검증 필요, 중복성. : CJK070423
				prevSTail = get_nvram_wparam(NVRAM_LAST_TRANS_COUNT + 2 * eth.scaleid);
#ifdef USE_MAX_SCALE_NO_255
				prevSTail2 = get_nvram_wparam(NVRAM_LAST_TRANS_SALE2 + 2 * (eth.scaleid - MAX_SCALE));
#endif
				if (eth.scaleid < MAX_SCALE && tail == prevSTail)	// 똑같은 Transaction이 재발송될 경우
				{
					ret = 0x13;
				}
#ifdef USE_MAX_SCALE_NO_255
				else if (eth.scaleid >= MAX_SCALE && tail == prevSTail2)	// 똑같은 Transaction이 재발송될 경우
				{
					ret = 0x13;
				}
#endif                
				else
				{
					ret = 0;
					if (trans.trtStatus&FLAG_TRANS_VOID) {
						if (param2 == 0x13 || param2 == 0x33)	// restore
						{
							transaction_sale_store(&trans, NULL, NULL, NULL, 1, 0);	// master->PC
							LocalPluTotalSummaries(eth.scaleid, &trans, NULL, NULL, 1, 1);
						}
						else
						{
							ret = ClerkVoidTransaction(clerk,trans.trtSerialNo,ON,eth.scaleid, 1);
						}
					} else if (trans.trtStatus&FLAG_TRANS_NOVOID) {
						if (param2 == 0x13 || param2 == 0x33)	// restore
						{
						}
						else
						{
							ret = ClerkVoidTransaction(clerk,trans.trtSerialNo,OFF,eth.scaleid, 1);
						}
                    //} else if ((trans.trtStatus&FLAG_TRANS_LABEL)||(trans.trtStatus&FLAG_TRANS_RETURN)){  //SG060614
					//	LocalPluTotalSummaries(eth.scaleid, &trans, NULL, NULL, 1, 0);
					} else {
						if (trans.trtStatus&FLAG_TRANS_ADD)
						{
							if (param2 == 0x13 || param2 == 0x33)	// restore
							{
								transaction_sale_store(&trans, NULL, NULL, NULL, 1, 0);	// master->PC
							}
							else
							{
								ret = ClerkAddTransaction(trans.trtOperator,&trans,1,0);
							}
						}
						if (ret == 0) {
							LocalPluTotalSummaries(eth.scaleid, &trans, NULL, NULL, 1, 0);
						}
					}
				}
// return value 
//  0x00:ok, 
//  0x01:timeout, (master에서 사용X)
//  0x02:memory full, (master에서 사용X) 
//  0x02:잘못된 데이터 전송, (master에서 사용X) 
//  0x11:clerk no 초과,
//  0x13:중복 transaction(동일한 scale id)
//  0x14:중복 transaction(다른 scale id)
//  0x12:memory full 
//  0x23:해당index없음, 
//  0x24:중복설정(void)
				if (eth.scaleid < MAX_SCALE) set_nvram_wparam(NVRAM_LAST_TRANS_COUNT + 2 * eth.scaleid, tail);
#ifdef USE_MAX_SCALE_NO_255
				else set_nvram_wparam(NVRAM_LAST_TRANS_SALE2 + 2 * eth.scaleid, tail);
#endif
				if (ret == 0)	// 0:ok
				{
					transaction_sale_end(CBuf,clerk,tail);		// 0x02
				}
				else if (ret == 0x12 || ret == 0x02 || ret == 0x16)	// 0x12:Memory Full
				{
					transaction_sale_send_memory_full(CBuf, ret, clerk, tail);	// 0x04
				}

				else if (ret == 0x13)	// 0x13:중복 Transaction 수신(동일 scale)
				{
					transaction_sale_send_abort(CBuf, clerk, tail);		// 0x05
				}
				else //if (ret == 0x11 || ret == 0x23 || ret == 0x24 || ret == 0x01)	// 0x11:clerk no초과, 0x23:해당index없음, 0x24:중복됨(void), 0x14:중복 Transaction(동일 scale)
				{
					transaction_sale_send_fail(CBuf, clerk, tail);		// 0x06
				}
		       }
		       break;
		case 0x02: //slave
			network_status.send_trans_sale = 1;
			break;
		case 0x04:	// Slave // Memory Full
			network_status.send_trans_sale = 2;
			break;
		case 0x05:	// Slave // 동일한 transaction 재전송시
			network_status.send_trans_sale = 3;
			break;
		case 0x06:	// Slave // clekr no 초과 등
			network_status.send_trans_sale = 4;
			break;
		case 0x08:	// Slave // clekr no 초과 등
			network_status.send_trans_sale = 5;
			break;
	}
//sprintf(MsgBuf, "[RCV] param=%02X, flag=%d, tail=%d clerk=%d\r\n", param2, network_status.send_trans_sale, tail, clerk);
//MsgOut(MsgBuf);
	return 0;
}

// "i00F071..."
INT8U recv_transaction_pay(HUGEDATA COMM_BUF *CBuf,INT16S param2,INT8U *str,INT16S length)
{
	ETH_CONNECT_LIST eth;
	TRANSACTION_PAY trans;
	INT16S status,r;
	INT32U v32;
	INT8U code,tail;
	//INT8U clerk;
	INT16U prevPTail;
#ifdef USE_MAX_SCALE_NO_255
	INT16U prevPTail2;
#endif

	status=0;
	r     =0;
	memset((INT8U *)&eth,0,sizeof(ETH_CONNECT_LIST));
	memset((INT8U *)&trans,0,sizeof(TRANSACTION_PAY));
	while (1) {
		if (r>=length) break;
		switch (status) {
			case 0: v32=0;
				code=str[r];
				if (code==0) goto END;
				status=1;
				break;
			case 1: status=0;
				if (str[r]=='=') status=2;
				break;
			case 2: if (str[r]=='.') {
					switch (code) {
					    //case 'C': clerk=(INT8U)v32; break;
					    case 'T': tail =(INT8U)v32; 
#ifdef USE_INDIA_FUNCTION
					    	      network_status.rcv_tail_pay = tail;
#endif
					    	break;
					    default : if (!commun_common_ethernet(code,&eth,v32))
					    		   inter_transaction_pay(code,&trans,v32);
					    	      
					    	      break;
					}
					status=0;
				} else {
					v32<<=4;
					v32|=ctoh(str[r]);
				}
		}
		r++;
	}
END:
// ================================================
// Param2 = 2 일때전송 완료.clerkid=0,
// Param2 = 1 일때수신중 , 
// ================================================
	switch (param2) {
	    case 0x01:
	    case 0x13:
			prevPTail = get_nvram_wparam(NVRAM_LAST_TRANS_PAY + 2 * eth.scaleid);
#ifdef USE_MAX_SCALE_NO_255
			prevPTail2 = get_nvram_wparam(NVRAM_LAST_TRANS_PAY2 + 2 * (eth.scaleid - MAX_SCALE));
#endif
			if (eth.scaleid < MAX_SCALE && tail == prevPTail)	// 중복재전송시 무시
			{
				transaction_pay_send_abort(CBuf,tail);
				break;
			}
#ifdef USE_MAX_SCALE_NO_255
			else if (eth.scaleid >= MAX_SCALE && tail == prevPTail2)
			{
				transaction_pay_send_abort(CBuf,tail);
				break;
			}
#endif            
			if (tail) 
			{
				PayChangeSummary(trans);
				RecievedTransPay = trans;
				transaction_pay_end(CBuf,tail);
			}
			if (eth.scaleid < MAX_SCALE) set_nvram_wparam(NVRAM_LAST_TRANS_PAY + 2 * eth.scaleid, tail);
#ifdef USE_MAX_SCALE_NO_255
			else set_nvram_wparam(NVRAM_LAST_TRANS_PAY2 + 2 * (eth.scaleid - MAX_SCALE), tail);
#endif
			break;
	    case 0x02: // slave receive. OK
			network_status.send_trans_pay = 1;
			break;
	    case 0x05: // slave receive. 중복에 의한 abort
			network_status.send_trans_pay = 2;
			break;
	    case 0x03: // slave receive. Wiat
			network_status.send_trans_pay = 3;
			break;
	}
	return 0;
}

// "i00F080..."
INT8U recv_change_unitprice(HUGEDATA COMM_BUF *CBuf,INT16S param2,INT8U *str,INT16S length)
// =====================================================
// 변경 단가 전송시만 사용함.
// =====================================================
{
	ETH_CONNECT_LIST eth;
	INT16S status,r;
	INT32U v32;
	INT8U code;
	INT8U clerk;

	status=0;
	r     =0;

	memset((INT8U *)&eth,0,sizeof(ETH_CONNECT_LIST));
	while (1) {
		if (r>=length) break;
		switch (status) {
			case 0: v32=0;
				code=str[r];
				if (code==0) goto END;
				status=1;
				break;
			case 1: status=0;
				if (str[r]=='=') status=2;
				break;
			case 2: if (str[r]=='.') 
				{
					switch (code) 
					{
					    //case 'C': clerk=(INT8U)v32; break;
					    case 'T': network_status.rcv_tail = (INT16U)v32; break;
					    default : break;
					}
					status=0;
				} 
				else 
				{
					v32<<=4;
					v32|=ctoh(str[r]);
				}
		}
		r++;
	}
END:
	switch (param2) {
//		case 0x03: //Master
		case 0x02: //slave
			network_status.send_trans_sale = 1;
			break;
	}
	return 0;
}

INT8U commun_recv_data(INT32U addr,INT8U *str,INT16S leng)
{
	INT32U src_addr;
	INT16S    i;
	src_addr  = DFLASH_BASE;
	src_addr += addr;
	for (i=0; i<leng; i++) {
		Flash_bwrite(src_addr,str[i]);
	}
	return 1;
}

// "i00F040..."
INT8U recv_destination_ip(HUGEDATA COMM_BUF *CBuf, INT16S param2, INT8U *str, INT16S length)
{
	ETH_CONNECT_LIST eth;
	INT16S status,r;
	INT32U v32;
	INT8U code;
	INT8U ip[4];
	INT16U port;
	INT8U o_ip[4];
	INT16U i;

	status=0;
	r     =0;

	memset((INT8U *)&eth,0,sizeof(ETH_CONNECT_LIST));
	memset(ip, 0, 4);
	port = 0;
	while (1) {
		if (r>=length) break;
		switch (status) {
			case 0: v32=0;
				code=str[r];
				if (code==0) goto END;
				status=1;
				break;
			case 1: status=0;
				if (str[r]=='=') status=2;
				break;
			case 2: if (str[r]=='.') 
				{
					switch (code) 
					{
						case 'I':
							ip[0] = (INT8U)(v32>>24);
							ip[1] = (INT8U)((v32>>16)&0xff);
							ip[2] = (INT8U)((v32>> 8)&0xff);
							ip[3] = (INT8U)(v32&0xff);
							break;
						case 'P': 
							port = (INT16U)v32; 
							break;
						default : break;
					}
					status=0;
				} 
				else 
				{
					v32<<=4;
					v32|=ctoh(str[r]);
				}
		}
		r++;
	}
END:
	switch (param2) {
		case 0x01: // destination ip 1
			if (!netset_ip_valid_check(ip, 0)) return 0;
			get_net_sparam(NETWORK_MACHINE_REMOTE_IP, o_ip, 4);
			for (i = 0; i < 4; i++)
			{
				if (o_ip[i] != ip[i])
				{
					set_net_sparam(NETWORK_MACHINE_REMOTE_IP, ip, 4);
					break;
				}
			}
			if (port != get_net_wparam(NETWORK_MACHINE_REMOTE_TCPPORT))
			{
				set_net_wparam(NETWORK_MACHINE_REMOTE_TCPPORT, port);
			}
			EthCardStruct.TCPDestIP[3] = ip[0];
			EthCardStruct.TCPDestIP[2] = ip[1];
			EthCardStruct.TCPDestIP[1] = ip[2];
			EthCardStruct.TCPDestIP[0] = ip[3];
			EthCardStruct.TCPDestPort =  port;
			ethernet_list.remoteip[0] = ip[0];
			ethernet_list.remoteip[1] = ip[1];
			ethernet_list.remoteip[2] = ip[2];
			ethernet_list.remoteip[3] = ip[3];
			EthLocalCmdSend(ETH_SET_DEST_IP, 0);
			EthLocalCmdSend(ETH_SET_DEST_TCP_PORT, 0);
			return 1;
			break;
	}
	return 0;
}

#ifdef USE_INDIA_FUNCTION
INT16U commun_check_inventory(char *buffer, INT8U code, INT8U deptNo, INT32U pluNo, INT8U orgType, INT8U newType, INT32U weight, INT32U qty, INT8U result)
{
	INT16S slen;
	INT8U  bcc;	

	slen = 15;

	if (code == 0x01)
	{
		slen += ethernet_gen_head(&buffer[slen]);
		sprintf(&buffer[slen],"D=%02X.", deptNo);
		slen += strlen(&buffer[slen]);
		
		sprintf(&buffer[slen],"P=%08lX.", pluNo);
		slen += strlen(&buffer[slen]);
		
		sprintf(&buffer[slen],"O=%02X.", orgType);
		slen += strlen(&buffer[slen]);
		
		sprintf(&buffer[slen],"N=%02X.", newType);
		slen += strlen(&buffer[slen]);
		
		sprintf(&buffer[slen],"W=%06lX.", weight);
		slen += strlen(&buffer[slen]);
		
		sprintf(&buffer[slen],"Q=%06lX.", qty);
		slen += strlen(&buffer[slen]);
	}
	else if (code == 0x02)
	{
		slen += ethernet_gen_head(&buffer[slen]);
		sprintf(&buffer[slen],"R=%02X.", result);
		slen += strlen(&buffer[slen]);
	}
	else
	{
		return 0;
	}

	sprintf(buffer,"i00F079,%02XL%03X", code, slen-15);

	buffer[14] = ':';
	bcc = gen_bcc((INT8U *)&buffer[15], slen-15);
	buffer[slen++] = bcc;

	return slen;
}


INT8U inventory_check_send(HUGEDATA COMM_BUF *CBuf, INT8U code, INT8U deptNo, INT32U pluNo, INT8U orgType, INT8U newType, INT32U weight, INT32U qty)
{
	INT16S slen;
	char   *commun_buffer;
	INT8U  ret, err;
	INT8U  nretry;    


	commun_buffer=(char *)serial_send_point_for_client;
	
	slen = commun_check_inventory(commun_buffer, code, deptNo, pluNo, orgType, newType, weight, qty, 0);

	err = 0;
	ret = 0;            
	while (!ret) 
	{
		ret = Ethernet_Server(CLIENT_OPEN, 0, CLIENT_CLERK_TRANS);
		nretry++;
		if (nretry >= 4) break;
	}
	if (ret == 1) 
	{
		commun_outleng(CBuf, commun_buffer, slen);
	    	network_status.send_inventory_check = 0;
		network_timeout_reset(NETWORK_TIMEOUT_CHECK_INVENTORY);
		while (1) 
		{
			if (network_status.send_inventory_check) break; // result received
			network_common_trans();
			if (network_timeout_check(20, NETWORK_TIMEOUT_CHECK_INVENTORY)) 
			{
				if (network_counter_check(2, NETWORK_TIMEOUT_CHECK_INVENTORY)) // 총 3회 재전송
				{
					commun_outleng(CBuf, commun_buffer, slen);
				} 
				else 
				{
					err = 1;
					break;
				}
			}
		}
		ret = 0;
		if (network_status.send_inventory_check == 1) // OK
		{
			ret = 1;
		}
		else if (network_status.send_inventory_check == 2) // 재고부족
		{
			ret = 0;
		}
		Ethernet_Server(CLIENT_CLOSE, 0, CLIENT_CLERK_TRANS);
	}

	if (err)
	{
		network_error(MSG_21_NET_ERR);
		ret = 0;
	}

	return ret;
}


INT8U CheckInventory(INT16U deptno, INT32U pluno, INT8U orgType, INT8U newType, INT32U weight, INT32U qty)
{
	INT8U ret;
	
	if (ethernet_list.status == 2) // slave
	{
		ret = inventory_check_send(&CommBufEthData[0], 0x01, deptno, pluno, orgType, newType, weight, qty);
	}
	else
	{
		ret = DeductPluInventory(deptno, pluno, orgType, newType, weight, qty, INVENTORY_CHECK);
	}

	return ret;
}

// "i00F079..."
INT8U recv_inventory(HUGEDATA COMM_BUF *CBuf, INT16S param2, INT8U *str, INT16S length)
{
	ETH_CONNECT_LIST eth;
	TRANSACTION_PAY trans;
	INT16S status,r;
	INT32U v32;
	INT8U  code, tail, clerk;
	INT8U  deptNo;
	INT32U pluNo;
	INT8U  orgPluType;
	INT8U  newPluType;
	INT32U weight;
	INT32U qty;
	INT8U  result;
	INT8U  ret;
	INT16S slen;
	char   *commun_buffer;

	deptNo = 0;
	pluNo = 0;
	orgPluType = 0;
	newPluType = 0;
	weight = 0;
	qty = 0;
	ret = 0;
	result = 0;
	
	status=0;
	r     =0;
	memset((INT8U *)&eth,0,sizeof(ETH_CONNECT_LIST));
	while (1) {
		if (r>=length) break;
		switch (status) {
			case 0: v32=0;
				code=str[r];
				if (code==0) goto END;
				status=1;
				break;
			case 1: status=0;
				if (str[r]=='=') status=2;
				break;
			case 2: 
				if (str[r]=='.') 
				{
					switch (code) 
					{
						case 'D': deptNo = (INT8U)v32; break;
						case 'P': pluNo = v32; break;
						case 'O': orgPluType = (INT8U)v32; break;
						case 'N': newPluType = (INT8U)v32; break;
						case 'W': weight = v32; break;
						case 'Q': qty = v32; break;
						case 'R': result = (INT8U)v32; break;
						default : commun_common_ethernet(code, &eth, v32);					    	      
							break;
					}
					status=0;
				} else {
					v32<<=4;
					v32|=ctoh(str[r]);
				}
		}
		r++;
	}
END:
	switch (param2) {
		case 0x01: // master receive
			ret = DeductPluInventory(deptNo, pluNo, orgPluType, newPluType, weight, qty, INVENTORY_CHECK);
			commun_buffer=(char *)serial_send_point;
			slen = commun_check_inventory(commun_buffer, 0x02, 0, 0, 0, 0, 0, 0, ret);
			commun_outleng(CBuf, commun_buffer, slen);
		case 0x02: // slave receive
			if (result) // OK
			{
				network_status.send_inventory_check = 1;
			}
			else // 재고 부족
			{
				network_status.send_inventory_check = 2;
			}
			break;
	}
	return 0;
}
#endif

INT8U label_reload(void)
{
	label_load(status_scale.cur_labelid);
	return 0x77;
}

INT8U label_delete(HUGEDATA COMM_BUF *CBuf,INT32U spart,INT8U mode)
{
	ETH_CONNECT_LIST eth;
	INT16S form;
	INT32U addr;
	INT16U id;
	INT16U n_address,info_size;

	mode &=0x01;
	memset((INT8U *)&eth,0,sizeof(ETH_CONNECT_LIST));
	if (mode) {	// CxxF06,<value>
		form  = (INT16S)(spart>>16);
		id    =  (INT16U)(spart&0xffff);
		if ((form<1) || (form>4)) return 0x82;
		if ((id<1) || (id>99)) return 0x82;
		n_address=999;
		switch (form) {
		    case 1: n_address=get_new_labeladdress(id,1);
		    	    break;
		    case 2: n_address=get_new_bitmapaddress(id,1);
		    	    break;
		    case 3: if ((id<=0) || (id>20)) return 0x82;
		            n_address=id;
			    break;
		    case 4: if ((id<=0) || (id>14)) return 0x82;
		            n_address=id;
			    break;
		}
		if (n_address==999) return 0x82;
	} else {
		form = (INT16U)spart;
		id  = 0;
		if ((form<1) || (form>3)) return 0x82;
	}
	if (id) {
		info_size=BITMAP_INFO_SIZE;
		addr     =FLASH_BITMAP_AREA;
		if ((form==1) || (form==3)) {
			addr = get_addr_from_label_idx(n_address, LABEL_AREA_HEADER);
			Flash_clear(addr, LABEL_CLEAR_DATA, LABEL_INFO_SIZE);	
			return 1;
		}
		n_address *= info_size;
		addr += (INT32U)n_address;
		addr += DFLASH_BASE;
		Flash_clear(addr,0,info_size);
	} else {
		switch (form) {
			case 1: label_clear(); break;
			case 2: bitmap_clear(); break;
			case 3: label_clear();
				bitmap_clear(); break;
		}
	}
	return 1;
}

//"Cx<action>F<mode>,<spart><0x0a>"
INT8U plu_delete_discount(HUGEDATA COMM_BUF *CBuf,INT32U spart,INT8U mode)
{
	ETH_CONNECT_LIST eth;
	INT32U pluno;
	INT16U dc_no,deptno;

	mode &=0x01;
	memset((INT8U *)&eth,0,sizeof(ETH_CONNECT_LIST));
	if (mode) {	// CxxF09,<value>
		deptno = (INT16S)(spart>>24);
		pluno  =  (INT32U)(spart&0xffffff);
		if ((deptno<=0) || (deptno>=MAX_DEPT_NO))  return 0x82;
		if (pluno<=0) return 0x82;
		dc_no = _get_dc_number((INT16U)deptno,pluno);
	} else {
		deptno = 0;
		pluno  = 0;
	}
	//sz = 0;
	//p  = 0;
	//max_id=MAX_DISCOUNT_NO;
	//sz=DISCOUNT_STRUCT_SIZE;
	//p=FLASH_DISCOUNT_AREA;
	//p+=DFLASH_BASE;
	//memset(temp,0,32);
	if (mode) {
		//rp=0;
		if (dc_no) {
			discount_delete_byid(dc_no);
			//rp = (dc_no-1)*sz;
			//p+=rp;
			//Flash_swrite(p,temp,2);
		}
	} else {
		discount_delete_all();
	    //for (i=0; i<max_id; i++) {
	    //	Flash_swrite(p,temp,4);
	    //	p+=sz;
	    //}
	}
	return 1;
}

// plu 삭제 "C01F15,..."
INT8U commun_plu_delete(HUGEDATA COMM_BUF *CBuf,INT32U spart,INT8U mode)
{
	ETH_CONNECT_LIST eth;
	INT8U deptno,ret;
	INT32U pluno;
	INT16U addr;

	memset((INT8U *)&eth,0,sizeof(ETH_CONNECT_LIST));
	ret=0x99;
	if ((mode==0x13) || (mode==0x15))  {
		if (mode==0x15) {
		    if (!(network_status.service_flag&0x0002)) return 0;
		}
		deptno = (INT8U)(spart>>24);
		pluno  = (INT32U)(spart&0xffffff);
		if (deptno<1) return 0x82;
		if (pluno<1)  return 0x82;
		plu_table_search(deptno,pluno,&addr,0);
		if (addr) {
			plu_delete(deptno,pluno,1);
			ret=1;
		} else {
			ret=0x45;
		}
		if (mode==0x15) {
			network_status.load_plu=1;
			if (ret==1) network_status.load_plu=2;
			ret=0;
		}
	} else {
//		id=0;
		deptno = (INT8U)spart;
		ret=plu_delete_department(deptno);
		ret=1;
	}
	return ret;
}

INT8U commun_discount_reset(HUGEDATA COMM_BUF *CBuf,ETH_CONNECT_LIST eth,INT32U spart, INT8U cmd)
{
	INT8U deptno;
	INT32U pluno;//,addr;
	INT16S dc_no;
	INT8U scaleListId;

	deptno = (INT8U)(spart>>24);
	pluno  = (INT32U)(spart&0xffffff);
	scaleListId = network_get_scale_list_id(eth.ip);
	if (cmd == 0x17)
	{
		if ((INT8U)SyncDeptNo == deptno && SyncPluNo == pluno)
		{
#ifdef _USE_DBG_PLU_SYNC
sprintf(MsgBuf, "[PS][DDR]s_id=%d, dc_addr=%d\r\n", scaleListId, dc_no);
MsgOut(MsgBuf);
#endif
			commun_net_mask(1, &SyncDelAddr, scaleListId, 2);
			SyncDeptNo = 0;
			SyncPluNo = 0;
			SyncDelAddr = 0;
		}
	}
	else
	{
		dc_no = _get_dc_number(deptno, pluno);
		if ((dc_no>0) && (dc_no<MAX_DISCOUNT_NO)) {
			if (network_status.service_flag&0x20)	// send plu
			{
				commun_net_mask(1, (INT16U*)&dc_no, scaleListId, 2);
#ifdef _USE_DBG_PLU_SYNC
sprintf(MsgBuf, "[PS][DR]s_id=%d, dc_addr=%d\r\n", scaleListId, dc_no);
MsgOut(MsgBuf);
#endif
			}
		}
	}
	return 0;
}

// 저울간 PLU 전송 완료	"C02F14,..."
INT8U commun_plu_reset(HUGEDATA COMM_BUF *CBuf,ETH_CONNECT_LIST eth,INT32U spart,INT8U mode)
{
	INT8U deptno;
	INT32U pluno;
	INT16U addr;
	INT8U listId;

	deptno = (INT8U)(spart>>24);
	pluno  = (INT32U)(spart&0xffffff);
	listId = network_get_scale_list_id(eth.ip);
	if (mode == 0x16)	// del plu
	{
		if ((INT8U)SyncDeptNo == deptno && SyncPluNo == pluno)
		{
			commun_net_mask(0, &SyncDelAddr, listId, 2);
			SyncDeptNo = 0;
			SyncPluNo = 0;
			SyncDelAddr = 0;
		}
	}
	else
	{
		plu_table_search(deptno,pluno,&addr,0);
		if (addr) {
			commun_net_mask(0, &addr, listId, 2);
		}
	}
	return 0;
}

INT8U plu_table1_delete(HUGEDATA COMM_BUF *CBuf,INT32U spart,INT8U mode)
{
	ETH_CONNECT_LIST eth;
	INT16S part,id;
	INT8U ret;

	mode &=0x01;
	memset((INT8U *)&eth,0,sizeof(ETH_CONNECT_LIST));
	if (mode) {
		part = (INT16S)(spart>>16);
		id  =  (INT16S)(spart&0xffff);
		if (id<=0) return 0x82;
	} else {
		id=0;
		part = (INT16S)spart;
	}
	if (part<=0) return 0x82;
	ret=0x82;
	ret=table1_delete_any((INT8U)part,(INT8U)mode,(INT16U)id);
	return ret;
}

INT8U plu_table2_delete(HUGEDATA COMM_BUF *CBuf,INT32U spart,INT8U mode)
{
	ETH_CONNECT_LIST eth;
	INT16S part,id;
	INT8U ret;

	mode &=0x01;
	memset((INT8U *)&eth,0,sizeof(ETH_CONNECT_LIST));
	if (mode) {
		part = (INT16S)(spart>>16);
		id  =  (INT16S)(spart&0xffff);
	} else {
		id=0;
		part = (INT16S)spart;
	}
	if (part<0) return 0x82;
	ret=0x82;
	ret=table2_delete_any((INT8U)part,(INT8U)mode,(INT16U)id);
	return ret;
}

INT8U plu_table3_delete(HUGEDATA COMM_BUF *CBuf,INT32U spart,INT8U mode)
{
	ETH_CONNECT_LIST eth;
	INT16S part,id;
	INT8U ret;

	mode &=0x01;
	memset((INT8U *)&eth,0,sizeof(ETH_CONNECT_LIST));
	if (mode) {
		part = (INT16S)(spart>>16);
		id  =  (INT16S)(spart&0xffff);
	} else {
		id=0;
		part = (INT16S)spart;
	}
	if (part<0) return 0x82;
	ret=0x82;
	ret=table3_delete_any((INT8U)part,(INT8U)mode,(INT16U)id);
	return ret;
}
#ifdef USE_DIRECT_NUTRITION
INT8U commun_direct_nutrition_delete( COMM_BUF *CBuf,INT32U spart,INT8U mode)
{
	INT8U dept_no;
	INT32U plu_no;
	mode &= 0x01;
	int i, str_size;
	if(mode) {
		//delete one
		dept_no = spart & 0xff;
		plu_no = spart >> 8;
		if(delete_plu_direct_nutrition(dept_no, plu_no) < 0)
			return 0x95;
	} else {
		//delete all
		Flash_clear(FLASH_DIRECT_NUTRIFACT_AREA, 0, MAX_NUTRIFACT4_NO * str_size);
	}
	return 1;
}
#endif
INT8U plu_inhibit_reset(HUGEDATA COMM_BUF *CBuf,INT32U spart,INT8U mode)
{
	ETH_CONNECT_LIST eth;
	INT16S part;
	INT8U ret;

	mode &=0x01;
	memset((INT8U *)&eth,0,sizeof(ETH_CONNECT_LIST));

	part = (INT16S)spart;
	ret  = 0x97;
	if (part==1) {
		ret=0x36;
		global_set_pluinhibit(VERIFY_ALL,0);  //se-hyung 070802
	}
	return ret;
}

INT8U commun_report_delete(HUGEDATA COMM_BUF *CBuf,INT32U param,INT8U mode)
{
	ETH_CONNECT_LIST eth;
	INT16S i,period,xtype;
	SALES_SUMMARY_TYPE* tp;
	SALES_SUMMARY_TYPE sst;
	PLU_SALES_SUMMARY_TYPE* tp1;
	PLU_SALES_SUMMARY_TYPE pst;
	RTC_STRUCT time_str;
	char temp[16];

	tp = &sst;

	Report_GetCurTime(&time_str,3);

	mode &=0x01;
	memset((INT8U *)&eth,0,sizeof(ETH_CONNECT_LIST));
	if (mode) {
		period = (INT16S)(param>>8);
		xtype  =  (INT16S)(param&0xff);
	} else {
		period = 2;
		xtype = 9;
	}
	if (period<0) return 0x82;
	if (period>2) return 0x82;
	switch (xtype) {
		case 1:	for (i=0;i< MAX_SCALE;i++) {
				//tp = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_SCALE_SUMMARY, i);
				get_nvram_summary_addr(NVRAM_SCALE_SUMMARY, i, tp);
				ClearSummaryRecord(tp,period);
				set_nvram_summary_addr(NVRAM_SCALE_SUMMARY, i, tp);
			}
			sprintf(temp," RSC %d  ",period);
			break;
		case 2:
		case 3:	
			if(GlbOper.PluReportType)
			{
				sprintf(temp,"");
			}
			else
			{
				for (i=0;i< MAX_PLU_REPORT_NO;i++) {
					//tp1 = (PLU_SALES_SUMMARY_TYPE *)get_nvram_plu_summary(NVRAM_PLU_SUMMARY, i);
					get_nvram_plu_summary(NVRAM_PLU_SUMMARY, i, &pst);
					tp1 = &pst;
					ClearPluSummaryRecord(tp1,period);
					set_nvram_plu_summary(NVRAM_PLU_SUMMARY, i, &pst);
				}
				sprintf(temp," RPL %d  ",period);
			}
			break;
		case 4: Report_ClearGroupReport((INT16S)period,1,MAX_GROUP,&time_str);
			sprintf(temp," RGR %d  ",period);
			break;
		case 5: Report_ClearDeptReport((INT16S)period,1,MAX_DEPT_NO,&time_str);
			sprintf(temp," RDP %d  ",period);
			break;
		case 6: Report_ClearHourlyReport((INT16S)period,1,24,&time_str);
			sprintf(temp," RHR %d  ",period);
			break;
		case 7: Report_ClearClerkReport((INT16S)period,1,MAX_CLERK_NO,&time_str);
			sprintf(temp," RCK %d  ",period);
			break;
		case 8: ClearSalesSummaries((INT16S)period);
			if (GlbOper.ClearOpenedClerkTransaction)
			{
				ClerkClearTransaction();
#if defined(USE_INDIA_FUNCTION) || defined(USE_SEND_TICKET_DATA) || defined(USE_SETPRODUCT_SALE) || defined(USE_HYUNDAI_SET_FUNCTION) 
//#ifdef USE_QR_ADDTOTAL
				ClearAdditionalData();
//#endif
#endif
			}
			Report_PeriodResetTime((INT8U)period,&time_str);
			sprintf(temp," RPD %d  ",period+1);
			break;
		case 9: ClearSalesSummaries(0);
			ClearSalesSummaries(1);
			ClerkClearTransaction();
#if defined(USE_INDIA_FUNCTION) || defined(USE_SEND_TICKET_DATA) || defined(USE_SETPRODUCT_SALE) || defined(USE_HYUNDAI_SET_FUNCTION) 
//#ifdef USE_QR_ADDTOTAL
			ClearAdditionalData();
//#endif
#endif
			Report_PeriodResetTime((INT8U)0,&time_str);
			Report_PeriodResetTime((INT8U)1,&time_str);
			sprintf(temp," R-ALL  ");
			break;
	}
	if (status_run.run_mode==RUN_SALE) {
// 20050514 COMDISPLAY
//		PutSmallString(0,25,temp);
	} else {
		temp[6]=0;
		display_string(2,(INT8U *)temp);
	}
	return 1;
}

INT8U commun_table2_reset(HUGEDATA COMM_BUF *CBuf,ETH_CONNECT_LIST eth,INT32U spart)
{
	INT8U table_no;
	INT16U data_no;
	INT8U scaleListId;

	table_no = (INT8U)(spart>>16);	// XXYYYYYY : X=table_no, Y=data_no
	data_no  = (INT16U)(spart&0x0000ffff);
	if (table_no == 1)	// only ingredient no
	{
		if ((data_no>0) && (data_no<=MAX_INGREDIENT_NO)) 
		{
			if (network_status.service_flag&0x20)	// send plu
			{
				scaleListId = network_get_scale_list_id(eth.ip);
				commun_net_mask(2, &data_no, scaleListId, 2);
#ifdef _USE_DBG_PLU_SYNC
sprintf(MsgBuf, "[PS][RI]s_id=%d, dc_addr=%d\r\n", scaleListId, data_no);
MsgOut(MsgBuf);
#endif
			}
		}
	}
	return 0;
}



INT8U commun_table3_reset(HUGEDATA COMM_BUF *CBuf,ETH_CONNECT_LIST eth,INT32U spart)
{
	INT8U table_no;
	INT16U data_no;
	INT8U scaleListId;

	table_no = (INT8U)(spart>>16);	// XXYYYYYY : X=table_no, Y=data_no
	data_no  = (INT16U)(spart&0x0000ffff);
	if (table_no == 3)	// only scroll message
	{
		if ((data_no>0) && (data_no<=MAX_SCROLL_NO)) 
		{
			if (network_status.service_flag&0x20)	// send plu
			{
				scaleListId = network_get_scale_list_id(eth.ip);
				commun_net_mask(3, &data_no, scaleListId, 2);
#ifdef _USE_DBG_PLU_SYNC
sprintf(MsgBuf, "[PS][RS]s_id=%d, dc_addr=%d\r\n", scaleListId, data_no);
MsgOut(MsgBuf);
#endif
			}
		}
	}
	return 0;
}

INT8U commun_command_remote_key(INT16U param)
{
	KEY_Write(param, 0, 1);
	if(status_run.run_mode == RUN_SALE)
		sale_key_proc();
	KeyDrv.comm_key = 0;
	BuzOnAdd(1);
	return 0;
}

#ifdef USE_INDIA_FUNCTION
INT8U plu_inventory_delete(HUGEDATA COMM_BUF *CBuf,INT32U spart,INT8U mode)
{
	ETH_CONNECT_LIST eth;
	INT16S part,id;
	INT8U ret;
	INT32U pluno;
	INT16U deptno;

	mode &=0x01;

	if (mode) {
		deptno = (INT16U)(spart>>24);
		pluno  =  (spart&0xffffff);
		DeletePluInventory(deptno, pluno);
	} else {
		if (spart != 0x01) return 0x82;
		part = (INT16S)spart;
		DeletePluInventoryAll();
	}

	return 1;
}
#endif

INT32S get_raw_value(void)
{
	INT32S rawval;
	INT8U backup_cur_unit;
	INT16U cur_time;
	INT8U errflag;

	rawval = 0;
	errflag = 0;

	backup_cur_unit = (INT8U)ADM_WaitData(ADM_CMD_GET_W_UNIT_CUR, 0, ADM_PARAM_OFF);
	ADM_WaitData(ADM_CMD_SET_OUT_AD5, 0, ADM_PARAM_OFF);		// Set Normalized A/D Value(AD2)
	ADM_WaitData(ADM_CMD_START_WT, 0, ADM_PARAM_OFF);		// Start Weight

	cur_time = SysTimer100ms;
	while(!ADM_RecvProc())
	{
		if ((INT16U)(SysTimer100ms - cur_time) > 2)
		{
			errflag = 1;
			break;
		}
	}

	if (errflag) rawval = 0;
	else rawval = ADM_GetData.RecvData;

	ADM_WaitData(ADM_CMD_SET_W_UNIT_CUR, (INT32S)backup_cur_unit, ADM_PARAM_ON);
	ADM_WaitData(ADM_CMD_SET_OUT_WT, 0, ADM_PARAM_OFF);		// Set Weight Value(C0)
	return(rawval);
}