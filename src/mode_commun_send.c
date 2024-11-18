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

extern INT16S netGetClerkLock(INT8U operator, INT8U scaleid);
extern void netSetClerkLock(INT8U operator, INT8U scaleid, INT8U lock);
extern char  date_getweek(short year,short month,short day );

extern void transaction_sale_store(TRANSACTION_RECORD_TYPE *pTrans, TRANSACTION_RECORD_SUB_TYPE *pTrans_sub, TR_ADDITION_DATA_TYPE* pTrans_add, UNITPRICE_CHANGE_TYPE *pChgPrice, INT8U mode, INT8U sendflag);
extern void transaction_sale_end(HUGEDATA COMM_BUF *CBuf,INT8U clerk,INT16U tail);
//extern INT8U  commun_send_block_data(HUGEDATA COMM_BUF *CBuf,INT32U cmd,INT8U data,INT32U no);
//extern INT8U  commun_send_addr_data(HUGEDATA COMM_BUF *CBuf,INT32U cmd,INT16U fcode);
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
extern INT8U commun_command_remote_key(INT16U param);
extern void Flash_write_data(INT32U startAddr,INT16U memSize,HUGEDATA INT8U *value,INT8U v_leng,INT8U bitFlag,INT8U startBit,INT8U countBit,INT16U fcode);
extern void Flash_read_data(INT32U startAddr,INT16U memSize,HUGEDATA INT8U *value,INT8U bitFlag,INT8U startBit,INT8U countBit,INT16U fcode);
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

extern long	TicketTransactionNumber[];
extern long	CounterTicket;
extern INT16U   global_one2one_count;
extern char	gmsg[];
extern INT8U    GlbFlagPCnMasterConnection;
extern INT16S      NetCheckMasterConnection(void);

extern void commun_write_response(HUGEDATA COMM_BUF *CBuf, INT8U *str, INT8U errCode); //SG071204
extern INT8U commun_write_plu_data(HUGEDATA COMM_BUF *CBuf,INT8U *str,INT16S leng); //SG071204
extern INT8U commun_delete_plu_data(HUGEDATA COMM_BUF *CBuf,INT8U *str, INT16S leng); //SG080111

#ifdef USE_PROTOCOL_CHINA_DAHUA	// 중국 전용 다화 프로토콜 
static void commun_dahua_write(HUGEDATA COMM_BUF *CBuf,INT8U cmd,INT32U number,INT32U nn,INT32U length,INT8U *str);
#endif
extern INT8U ChangeDestIPFlag;
extern INT16U SyncErrCode;
#ifdef USE_KOR_SLAUGHT_CODE_4DIGIT
extern INT16U SlaughtTableLastId;
#endif
#ifdef USE_INGREDIENT_CODE_4DIGIT
extern INT16U IngTableLastId;
#endif

// Static prototype
static INT8U commun_send_plustrinfo(HUGEDATA COMM_BUF *CBuf,INT16S number);
static INT8U commun_send_plu_nth(HUGEDATA COMM_BUF *CBuf,INT32U nth,INT32U pcode);
static INT8U commun_send_fontbin(HUGEDATA COMM_BUF *CBuf,INT16S number);
static INT8U commun_send_fontinfo(HUGEDATA COMM_BUF *CBuf,INT16S number);
static INT8U commun_send_key(HUGEDATA COMM_BUF *CBuf,INT16S key_mode,INT16S keyno);
static INT8U commun_send_caption(HUGEDATA COMM_BUF *CBuf,INT16U mode,INT16U id);
static INT8U commun_send_labelbmp(HUGEDATA COMM_BUF *CBuf,INT8U mode,INT16S number,INT16S fnumber);
static INT8U commun_send_discountbyplu(HUGEDATA COMM_BUF *CBuf, INT8U scaleListId, INT16U cmd, INT32U pluno, INT16U deptno);
static INT8U commun_send_discount(HUGEDATA COMM_BUF *CBuf,INT16U cmd,INT16U sdcno,INT8U mode,INT8U rept);
static INT8U commun_send_plu(HUGEDATA COMM_BUF *CBuf,ETH_CONNECT_LIST eth,INT16U cmd,INT32U nth,INT32U pcode);
static INT8U commun_send_table1(HUGEDATA COMM_BUF *CBuf,INT32U spart,INT16U sid,INT8U next);
static INT8U commun_send_table2(HUGEDATA COMM_BUF *CBuf,INT8U cmd,INT32U spart,INT32U sid,INT8U next, INT8U request);
static INT8U commun_send_table3(HUGEDATA COMM_BUF *CBuf,INT8U cmd,INT32U spart,INT32U sid,INT8U next);
static INT8U commun_send_report(HUGEDATA COMM_BUF *CBuf,INT32U spart,INT32U sid);
static INT8U commun_send_other(HUGEDATA COMM_BUF *CBuf,INT32U spart,INT32U sid);
static INT8U commun_send_pluinhibit(HUGEDATA COMM_BUF *CBuf,INT32U spart,INT8U next);
static INT8U commun_send_labelbmp_list(HUGEDATA COMM_BUF *CBuf,INT16S number);
static INT8U commun_send_block_data(HUGEDATA COMM_BUF *CBuf,INT32U cmd,INT8U data,INT32U no);
static INT8U commun_send_addr_data(HUGEDATA COMM_BUF *CBuf,INT32U cmd,INT16U fcode);
#ifdef USE_INDIA_FUNCTION
static INT8U commun_send_inventory(HUGEDATA COMM_BUF *CBuf, INT32U spart, INT32U sid);
#endif
#ifdef USE_DIRECT_INGREDIENT
#ifdef USE_INGREDIENT_98_PROTOCOL
static INT8U commun_send_directingredient(HUGEDATA COMM_BUF *CBuf,INT32U nth,INT32U block_num);
#endif
#endif
#ifdef USE_DIRECT_NUTRITION
static INT8U commun_send_direct_nutrition( COMM_BUF *CBuf,INT32U dept_no,INT32U plu_no);
#endif
static void commun_send_stringtype(INT16S *r,char *com,char *form,char *data);


//"R<cmd>F<number>,<fnumber><0x0a>"
//"R<cmd>A<number><0x0a>"
void commun_read(HUGEDATA COMM_BUF *CBuf,ETH_CONNECT_LIST eth,INT8U cmd,INT32U number,INT32U fnumber)
{
	INT8U ret;
	char   head[20];
	INT8U scaleListId;
	ret=0;

	recv_counter++;
//sprintf(MsgBuf, "<SRR>cmd = %02X, param=%ld\r\n", cmd, number);
//MsgOut(MsgBuf);	// CJK090105
	switch (cmd) {
		case 0x001:ret=commun_send_plustrinfo(CBuf,(INT16S)number);
			   break;
		case 0x002:ret=commun_send_plu_nth(CBuf,number,fnumber);
			   break;
		case 0x003:if (number) {
				ret=commun_send_fontbin(CBuf,(INT16S)number);
			   } else {
				ret=commun_send_fontinfo(CBuf,(INT16S)fnumber);
			   }
			   break;
		case 0x004: ret=commun_send_key(CBuf,(INT16S)number,(INT16S)fnumber); // ,0,NULL,0);
			   // Key No
			   break;
		case 0x005: ret=commun_send_caption(CBuf,number,fnumber);
			   break;
		case 0x006:
		case 0x007:
		case 0x056:
		case 0x057: ret=commun_send_labelbmp(CBuf,(INT8U)cmd,(INT16S)number,(INT16S)fnumber);
			   break;
		case 0x019: // request from slave:remote call
		case 0x009: // request from PC
			    scaleListId = network_get_scale_list_id(eth.ip);
			    ret=commun_send_discountbyplu(CBuf, scaleListId, (INT16U)cmd, number, fnumber);
			   break;
		case 0x010:
			   ret=commun_send_discount(CBuf,(INT16U)cmd,(INT16U)number,(INT8U)fnumber,1);
			   break;
		case 0x015:
		case 0x014:
		case 0x013:ret=commun_send_plu(CBuf,eth,(INT16U)cmd,number,fnumber);
			   break;
		case 0x020: ret=commun_send_table1(CBuf,number,fnumber,0);
			   break;
		case 0x021: ret=commun_send_table1(CBuf,number,fnumber,1);
			   break;
		case 0x030: ret=commun_send_table2(CBuf,0x30,number,fnumber,0,ON);
			   break;
		case 0x031: ret=commun_send_table2(CBuf,0x30,number,fnumber,1,ON);
			   break;
		case 0x032: ret=commun_send_table3(CBuf,0x32,number,fnumber,0);
			   break;
		case 0x033: ret=commun_send_table3(CBuf,0x32,number,fnumber,1);
			   break;
		case 0x043: ret=commun_send_report(CBuf,number,fnumber);// ,NULL,0);
			    break;
		case 0x045:
			    ret=commun_send_other(CBuf,number,fnumber);
			    break;
		case 0x036:
			    ret=commun_send_pluinhibit(CBuf,number,0);
			    break;
		case 0x037:
			    ret=commun_send_pluinhibit(CBuf,number,1);
			    break;
		case 0x038: ret=commun_send_labelbmp_list(CBuf,(INT16S)number);
			    break;
		case 0x008:
		case 0x082:
		case 0x088: ret=commun_send_block_data(CBuf,cmd,number,fnumber);
			   break;
		case 0x085:
			    ret=commun_send_addr_data(CBuf,cmd,(INT16U)number);	
			   break;
#ifdef USE_INDIA_FUNCTION
		case 0x041: ret=commun_send_inventory(CBuf,number,fnumber);// ,NULL,0);
			    break;
#endif
#ifdef USE_DIRECT_INGREDIENT
#ifdef USE_INGREDIENT_98_PROTOCOL
		case 0x046: ret=commun_send_directingredient(CBuf,number,fnumber);
			    break;
#endif
#endif
#ifdef USE_DIRECT_NUTRITION
		case 0x047: ret = commun_send_direct_nutrition(CBuf,number,fnumber);
			break;
#endif
	}
	if (ret!=1) {
	 	sprintf((char *)head,"R%02X:E",cmd);
		head[5]=tohex(ret>>4  );
		head[6]=tohex(ret&0x0f);
		head[7]=0x0a;
		head[8]=0;
		commun_out(CBuf,(HUGEDATA char *)head);
	}
}

void commun_send_plu_modify_scale(void)
{
	INT8U  ret;
	INT8U  nextScale;
	INT16U sz;
	static INT8U pluSendState = PLUSYNC_SEND_STATE_CHECK_PLU;
	static INT16U waitTime = 0;
	static INT16U waitTimeConnection = 0;
	static INT8U scaleListId = 1;
	static INT16U plu_addr[MAX_SLAVE_SCALE] = {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
	static INT16U dc_addr[MAX_SLAVE_SCALE] = {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
	static INT16U ingr_addr[MAX_SLAVE_SCALE] = {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
	static INT16U data_addr[MAX_SLAVE_SCALE] = {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
	static INT16U ingr_block_no;
	INT32U id;
	INT16U dummy;

	if ((network_status.service_flag&0x20) == 0) return;

	if (scaleListId > MAX_SLAVE_SCALE) scaleListId = 1;

	if (ethernet_list.status == 2) return;//scaleListId = MAX_SLAVE_SCALE;	// Id MAX_SCALE은 slave의 remote IP

	nextScale = OFF;
	switch (pluSendState)
	{
		case PLUSYNC_SEND_STATE_OPEN_CONN_PLU:
		case PLUSYNC_SEND_STATE_OPEN_CONN_DC:
		case PLUSYNC_SEND_STATE_OPEN_CONN_INGR:
		case PLUSYNC_SEND_STATE_OPEN_CONN_SCROLL:
			if (!NetSlaveInfo[scaleListId - 1].use || !NetSlaveInfo[scaleListId - 1].ip[0]) 
			{
				pluSendState = PLUSYNC_SEND_STATE_NEXT_WITHOUT_CLOSE;
				break;
			}
			put_dest_ip(NetSlaveInfo[scaleListId-1].ip, NetSlaveInfo[scaleListId-1].tcp_port);
			ret = Ethernet_Server(CLIENT_OPEN, NET_CHK_PLU_SYNC_WAITTIME, CLIENT_PLU_SYNC);
			if (pluSendState == PLUSYNC_SEND_STATE_OPEN_CONN_PLU)
				pluSendState = PLUSYNC_SEND_STATE_WAIT_CONN_PLU;
			else if (pluSendState == PLUSYNC_SEND_STATE_OPEN_CONN_DC)
				pluSendState = PLUSYNC_SEND_STATE_WAIT_CONN_DC;
			else if (pluSendState == PLUSYNC_SEND_STATE_OPEN_CONN_INGR)
				pluSendState = PLUSYNC_SEND_STATE_WAIT_CONN_INGR;
			else if (pluSendState == PLUSYNC_SEND_STATE_OPEN_CONN_SCROLL)
				pluSendState = PLUSYNC_SEND_STATE_WAIT_CONN_SCROLL;
			waitTimeConnection = SysTimer100ms;
#ifdef _USE_DBG_PLU_SYNC
sprintf(MsgBuf, "[PS][OC](%d) st=%d, s_id=%d, addr=%d, ip=%d.%d.%d.%d:%u\r\n", ret, pluSendState, scaleListId, plu_addr[scaleListId - 1], EthCardStruct.TCPDestIP[3], EthCardStruct.TCPDestIP[2], EthCardStruct.TCPDestIP[1], EthCardStruct.TCPDestIP[0], EthCardStruct.TCPDestPort);
MsgOut(MsgBuf);
#endif
			break;
		case PLUSYNC_SEND_STATE_WAIT_CONN_PLU:	// wait connection
		case PLUSYNC_SEND_STATE_WAIT_CONN_DC:
		case PLUSYNC_SEND_STATE_WAIT_CONN_INGR:
		case PLUSYNC_SEND_STATE_WAIT_CONN_SCROLL:
			//put_dest_ip(NetSlaveInfo[scaleListId-1].ip, NetSlaveInfo[scaleListId-1].tcp_port);
			if (!ChangeDestIPFlag)
			{
				pluSendState = PLUSYNC_SEND_STATE_NEXT_WITHOUT_CLOSE;
				break;
			}
			ret = Ethernet_Server(CLIENT_CHECK, NET_CHK_PLU_SYNC_WAITTIME, CLIENT_PLU_SYNC);	// Check connection
			if (ret == 1) 
			{
				if (pluSendState == PLUSYNC_SEND_STATE_WAIT_CONN_PLU)
					pluSendState = PLUSYNC_SEND_STATE_SEND_PLU;
				else if (pluSendState == PLUSYNC_SEND_STATE_WAIT_CONN_DC)
					pluSendState = PLUSYNC_SEND_STATE_SEND_DC;
				else if (pluSendState == PLUSYNC_SEND_STATE_WAIT_CONN_INGR)
					pluSendState = PLUSYNC_SEND_STATE_SEND_INGR;
				else if (pluSendState == PLUSYNC_SEND_STATE_WAIT_CONN_SCROLL)
					pluSendState = PLUSYNC_SEND_STATE_SEND_SCROLL;
				SyncErrCode = 0;
#ifdef _USE_DBG_PLU_SYNC
sprintf(MsgBuf, "[PS][CK1](%d) st=%d, s_id=%d, addr=%d, ip=%d.%d.%d.%d:%u\r\n", ret, pluSendState, scaleListId, plu_addr[scaleListId - 1], EthCardStruct.TCPDestIP[3], EthCardStruct.TCPDestIP[2], EthCardStruct.TCPDestIP[1], EthCardStruct.TCPDestIP[0], EthCardStruct.TCPDestPort);
MsgOut(MsgBuf);
#endif
			}
			else
			{
				if ((INT16U)(SysTimer100ms - waitTimeConnection) > 5)	// 0.5초 //1.5초이상 연결실패하면 다음 저울
				{
#ifdef _USE_DBG_PLU_SYNC
sprintf(MsgBuf, "[PS][CK2]timeout(%d) st=%d, s_id=%d, addr=%d, ip=%d.%d.%d.%d:%u\r\n", ret, pluSendState, scaleListId, plu_addr[scaleListId - 1], EthCardStruct.TCPDestIP[3], EthCardStruct.TCPDestIP[2], EthCardStruct.TCPDestIP[1], EthCardStruct.TCPDestIP[0], EthCardStruct.TCPDestPort);
MsgOut(MsgBuf);
#endif
					pluSendState = PLUSYNC_SEND_STATE_NEXT;
				}
			}
			break;
/////////////////////////////////////////////
// START PLU ////////////////////////////////
		case PLUSYNC_SEND_STATE_CHECK_PLU:	// check PLU
			if (commun_net_mask(0, &plu_addr[scaleListId - 1], scaleListId, 10))
			{
				pluSendState = PLUSYNC_SEND_STATE_OPEN_CONN_PLU;
#ifdef _USE_DBG_PLU_SYNC
sprintf(MsgBuf, "[PS][CP1] s_id=%d, addr=%d, ip=%d.%d.%d.%d:%u\r\n", scaleListId, plu_addr[scaleListId - 1], EthCardStruct.TCPDestIP[3], EthCardStruct.TCPDestIP[2], EthCardStruct.TCPDestIP[1], EthCardStruct.TCPDestIP[0], EthCardStruct.TCPDestPort);
MsgOut(MsgBuf);
#endif
			}
			else
			{
				plu_addr[scaleListId - 1]++;
				if (plu_addr[scaleListId - 1] > MAX_PLU_NO)
				{
					plu_addr[scaleListId - 1] = 1;
					//nextScale = ON;
					pluSendState = PLUSYNC_SEND_STATE_CHECK_DC;
				}
			}
			break;
		case PLUSYNC_SEND_STATE_SEND_PLU:
			if (!ChangeDestIPFlag)	// 전송전 다른 Client에 의해 연결이 끊길 경우
			{
				pluSendState = PLUSYNC_SEND_STATE_NEXT_WITHOUT_CLOSE;
				break;
			}
			SyncPluNo = 0;
			SyncDeptNo = 0;
			SyncDelAddr = 0;
			ret = plu_get_dept_pluno(plu_addr[scaleListId - 1], &SyncDeptNo, &SyncPluNo);
			if (!ret)	// plu가 없으면(delete 또는 원래 없는 PLU)
			{
				plu_get_field(plu_addr[scaleListId - 1] - 1, PTYPE_DEPARTMENT_NO, (INT8U *)&SyncDeptNo, (INT16S *)&sz, sizeof(SyncDeptNo));
				plu_get_field(plu_addr[scaleListId - 1] - 1, PTYPE_PLU_NO, (INT8U *)&SyncPluNo, (INT16S *)&sz, sizeof(SyncPluNo));
				SyncDeptNo &= ~0x8000;
				if (SyncDeptNo < MAX_DEPT_NO && SyncDeptNo > 0 && SyncPluNo <= MAX_PLU_NUMBER && SyncPluNo > 0L)
				{
#ifdef _USE_DBG_PLU_SYNC
sprintf(MsgBuf, "[PS][SP2]s_id=%d, addr=%d, dept=%d, plu=%ld\r\n", scaleListId, plu_addr[scaleListId - 1], SyncDeptNo, SyncPluNo);
MsgOut(MsgBuf);
#endif
					SyncDelAddr = plu_addr[scaleListId - 1];
					if(plu_table_search(SyncDeptNo, SyncPluNo, &dummy, 0))	// 삭제할 PLU가 master에 존재하면 삭제전송 안함 
					{
						commun_net_mask(0, &plu_addr[scaleListId - 1], 0xff, 0);// 해당 PLU flag OFF
						pluSendState = PLUSYNC_SEND_STATE_CHECK_PLU;
					}
					else
					{
						// delete command
						commun_send_plu_delete(&CommBufEthData[0], SyncDeptNo, SyncPluNo);
						pluSendState = PLUSYNC_SEND_STATE_WAIT_ACK_PLU;
					}
				}
				else	// 없는 데이터 => 해당 PLU flag OFF
				{
#ifdef _USE_DBG_PLU_SYNC
sprintf(MsgBuf, "[PS][SP3]s_id=%d, addr=%d, dept=%d, plu=%ld\r\n", scaleListId, plu_addr[scaleListId - 1], SyncDeptNo, SyncPluNo);
MsgOut(MsgBuf);
#endif
					commun_net_mask(0, &plu_addr[scaleListId - 1], 0xff, 0);// 해당 PLU flag OFF
					pluSendState = PLUSYNC_SEND_STATE_CHECK_PLU;
				}
			}
			else	// send plu
			{
#ifdef _USE_DBG_PLU_SYNC
sprintf(MsgBuf, "[PS][SP1]s_id=%d, addr=%d, dept=%d, plu=%ld\r\n", scaleListId, plu_addr[scaleListId - 1], SyncDeptNo, SyncPluNo);
MsgOut(MsgBuf);
#endif
				commun_send_plu_from_address(&CommBufEthData[0], plu_addr[scaleListId - 1], 0, 1);
				pluSendState = PLUSYNC_SEND_STATE_WAIT_ACK_PLU;
			}
			waitTime = SysTimer100ms;
			break;
		case PLUSYNC_SEND_STATE_WAIT_ACK_PLU:	// wait ACK
			SyncStatus[scaleListId - 1] &= ~(0x01|0x02);
			if (SyncErrCode)
			{
				if (SyncErrCode == 0x99)	// Memory Full
				{
					SyncStatus[scaleListId - 1] |= 0x01;	// PLU Memory Full
				}
				else
				{
					SyncStatus[scaleListId - 1] |= 0x02;	// PLU ETC
				}
				pluSendState = PLUSYNC_SEND_STATE_CHECK_DC;
			}
			if (!commun_net_mask(0, &plu_addr[scaleListId - 1], scaleListId, 10)) // complete sending
			{
				if (commun_net_mask(0, &plu_addr[scaleListId - 1], scaleListId, 11))	// search next PLU
				{
					pluSendState = PLUSYNC_SEND_STATE_SEND_PLU;
				}
				else
				{
					pluSendState = PLUSYNC_SEND_STATE_CHECK_DC;
				}
			}
			else if ((INT16U)(SysTimer100ms - waitTime) > 15)	// wait 1.5sec 
			{
				SyncPluNo = 0;
				SyncDeptNo = 0;
				SyncDelAddr = 0;
				pluSendState = PLUSYNC_SEND_STATE_NEXT;
#ifdef _USE_DBG_PLU_SYNC
sprintf(MsgBuf, "[PS][AP]timeout(%d)\r\n", scaleListId);
MsgOut(MsgBuf);
#endif
			}
			break;
// END PLU //////////////////////////////////
/////////////////////////////////////////////

/////////////////////////////////////////////
// START DC /////////////////////////////////
		case PLUSYNC_SEND_STATE_CHECK_DC:
			if (commun_net_mask(1, &dc_addr[scaleListId - 1], scaleListId, 10))
			{
				pluSendState = PLUSYNC_SEND_STATE_OPEN_CONN_DC;
#ifdef _USE_DBG_PLU_SYNC
sprintf(MsgBuf, "[PS][CD](%d)s_id=%d, dc_addr=%d, ip=%d.%d.%d.%d:%u\r\n", ret, scaleListId, dc_addr[scaleListId - 1], EthCardStruct.TCPDestIP[3], EthCardStruct.TCPDestIP[2], EthCardStruct.TCPDestIP[1], EthCardStruct.TCPDestIP[0], EthCardStruct.TCPDestPort);
MsgOut(MsgBuf);
#endif
			}
			else
			{
				dc_addr[scaleListId - 1]++;
				if (dc_addr[scaleListId - 1] > MAX_DISCOUNT_NO - 1)
				{
					dc_addr[scaleListId - 1] = 1;
					//nextScale = ON;
					pluSendState = PLUSYNC_SEND_STATE_CHECK_INGR;
				}
			}
			break;
		case PLUSYNC_SEND_STATE_SEND_DC: // send discount
#ifdef _USE_DBG_PLU_SYNC
sprintf(MsgBuf, "[PS][SD]s_id=%d, dc_addr=%d\r\n", scaleListId, dc_addr[scaleListId - 1]);
MsgOut(MsgBuf);
#endif
			if (!ChangeDestIPFlag)	// 전송전 다른 Client에 의해 연결이 끊길 경우
			{
				pluSendState = PLUSYNC_SEND_STATE_NEXT_WITHOUT_CLOSE;
				break;
			}
			SyncPluNo = 0;
			SyncDeptNo = 0;
			SyncDelAddr = 0;
			if (discount_check_dc_number(dc_addr[scaleListId - 1], &SyncDeptNo, &SyncPluNo))
			{
				ret = commun_send_discount(&CommBufEthData[0], 0x19, dc_addr[scaleListId - 1], 0, 0);//"W19A..."
				pluSendState = PLUSYNC_SEND_STATE_WAIT_ACK_DC;
			}
			else	// 없는 데이터 => 해당 DC flag OFF
			{
				// Delete protocol 추가 되어야 함
				if((SyncDeptNo&0x7fff) < MAX_DEPT_NO && (SyncDeptNo&0x7fff) > 0 && SyncPluNo <= MAX_PLU_NUMBER && SyncPluNo > 0) 
				{
					SyncDelAddr = dc_addr[scaleListId - 1];
					commun_send_dc_delete(&CommBufEthData[0], (SyncDeptNo&0x7fff), SyncPluNo);
					pluSendState = PLUSYNC_SEND_STATE_WAIT_ACK_DC;
				}
				else
				{
					commun_net_mask(1, &dc_addr[scaleListId - 1], 0xff, 0);// 해당 DC flag OFF
					pluSendState = PLUSYNC_SEND_STATE_CHECK_DC;
				}
			}
			waitTime = SysTimer100ms;
			break;
		case PLUSYNC_SEND_STATE_WAIT_ACK_DC:	// wait ACK for discount
			SyncStatus[scaleListId - 1] &= ~(0x04|0x08);
			if (SyncErrCode)
			{
				if (SyncErrCode == 0x98)	// Memory Full
				{
					SyncStatus[scaleListId - 1] |= 0x04;	// D/C Memory Full
				}
				else
				{
					SyncStatus[scaleListId - 1] |= 0x08;	// D/C ETC
				}
				pluSendState = PLUSYNC_SEND_STATE_CHECK_INGR;
			}
			if (!commun_net_mask(1, &dc_addr[scaleListId - 1], scaleListId, 10)) // complete sending
			{
				if (commun_net_mask(1, &dc_addr[scaleListId - 1], scaleListId, 11))	// search next PLU
				{
					pluSendState = PLUSYNC_SEND_STATE_SEND_DC;
				}
				else
				{
					pluSendState = PLUSYNC_SEND_STATE_CHECK_INGR;
				}
			}
			else if ((INT16U)(SysTimer100ms - waitTime) > 30)	// wait 1.5sec 
			{
				SyncPluNo = 0;
				SyncDeptNo = 0;
				SyncDelAddr = 0;
				pluSendState = PLUSYNC_SEND_STATE_NEXT;
#ifdef _USE_DBG_PLU_SYNC
sprintf(MsgBuf, "[PS][AD]timeout(%d)\r\n", scaleListId);
MsgOut(MsgBuf);
#endif
			}
			break;
// END DC ///////////////////////////////////
/////////////////////////////////////////////

/////////////////////////////////////////////
// START INGREDIENT MSG /////////////////////
		case PLUSYNC_SEND_STATE_CHECK_INGR:
			ingr_block_no = 0;
			if (commun_net_mask(2, &ingr_addr[scaleListId - 1], scaleListId, 10))
			{
				pluSendState = PLUSYNC_SEND_STATE_OPEN_CONN_INGR;
#ifdef _USE_DBG_PLU_SYNC
sprintf(MsgBuf, "[PS][CI](%d)s_id=%d, ingr_addr=%d, ip=%d.%d.%d.%d:%u\r\n", ret, scaleListId, ingr_addr[scaleListId - 1], EthCardStruct.TCPDestIP[3], EthCardStruct.TCPDestIP[2], EthCardStruct.TCPDestIP[1], EthCardStruct.TCPDestIP[0], EthCardStruct.TCPDestPort);
MsgOut(MsgBuf);
#endif
			}
			else
			{
				ingr_addr[scaleListId - 1]++;
				//ingr_block_no = 0;
				if (ingr_addr[scaleListId - 1] > MAX_INGREDIENT_NO)
				{
					ingr_addr[scaleListId - 1] = 1;
					//nextScale = ON;
					pluSendState = PLUSYNC_SEND_STATE_CHECK_SCROLL;
				}
			}
			break;
		case PLUSYNC_SEND_STATE_SEND_INGR: // send ingredient
			if (!ChangeDestIPFlag)	// 전송전 다른 Client에 의해 연결이 끊길 경우
			{
				pluSendState = PLUSYNC_SEND_STATE_NEXT_WITHOUT_CLOSE;
				break;
			}
			if (ingredient_check_number(ingr_addr[scaleListId - 1], ingr_block_no))
			{
#ifdef _USE_DBG_PLU_SYNC
sprintf(MsgBuf, "[PS][SI1]s_id=%d, ingr_addr=%d, ingr_bl_no=%d\r\n", scaleListId, ingr_addr[scaleListId - 1], ingr_block_no);
MsgOut(MsgBuf);
#endif
				id = (INT32U)ingr_addr[scaleListId - 1];
				id <<= 8;
				id &= 0xffffff00;
				id += ingr_block_no;
				ret = commun_send_table2(&CommBufEthData[0], 0x72, 1, id, 0, OFF);
				pluSendState = PLUSYNC_SEND_STATE_WAIT_ACK_INGR;
			}
			else	// 없는 데이터 => 해당 Ingr. flag OFF
			{
#ifdef _USE_DBG_PLU_SYNC
sprintf(MsgBuf, "[PS][SI2]s_id=%d, ingr_addr=%d, ingr_bl_no=%d\r\n", scaleListId, ingr_addr[scaleListId - 1], ingr_block_no);
MsgOut(MsgBuf);
#endif
				if (ingr_block_no == 0)
				{
					commun_send_table2_delete(&CommBufEthData[0], 1, ingr_addr[scaleListId - 1]);
					pluSendState = PLUSYNC_SEND_STATE_WAIT_ACK_INGR;
					//commun_net_mask(2, &data_addr[scaleListId - 1], 0xff, 0);// 해당 Ingr. flag OFF
					//pluSendState = PLUSYNC_SEND_STATE_CHECK_INGR;
				}
				else
				{
					commun_net_mask(2, &ingr_addr[scaleListId - 1], scaleListId, 2);
					pluSendState = PLUSYNC_SEND_STATE_CHECK_INGR;
				}
			}
			waitTime = SysTimer100ms;
			break;
		case PLUSYNC_SEND_STATE_WAIT_ACK_INGR:	// wait ACK for ingredient
			if (!commun_net_mask(2, &ingr_addr[scaleListId - 1], scaleListId, 10)) // complete sending
			{
				ingr_block_no++;
				if (ingredient_check_number(ingr_addr[scaleListId - 1], ingr_block_no))
				{
					commun_net_mask(2, &ingr_addr[scaleListId - 1], scaleListId, 3);
					pluSendState = PLUSYNC_SEND_STATE_SEND_INGR;
				}
				else
				{
					if (commun_net_mask(2, &ingr_addr[scaleListId - 1], scaleListId, 11))	// search next PLU
					{
						ingr_block_no = 0;
						pluSendState = PLUSYNC_SEND_STATE_SEND_INGR;
					}
					else
					{
						pluSendState = PLUSYNC_SEND_STATE_CHECK_SCROLL;
					}
				}
			}
			else if ((INT16U)(SysTimer100ms - waitTime) > 30)	// wait 1.5sec 
			{
				pluSendState = PLUSYNC_SEND_STATE_NEXT;
#ifdef _USE_DBG_PLU_SYNC
sprintf(MsgBuf, "[PS][AI]timeout(%d)\r\n", scaleListId);
MsgOut(MsgBuf);
#endif
			}
			break;
// END INGREDIENT MSG ///////////////////////
/////////////////////////////////////////////

/////////////////////////////////////////////
// START SCROLL MSG /////////////////////////
		case PLUSYNC_SEND_STATE_CHECK_SCROLL:
			if (commun_net_mask(3, &data_addr[scaleListId - 1], scaleListId, 10))
			{
				pluSendState = PLUSYNC_SEND_STATE_OPEN_CONN_SCROLL;
#ifdef _USE_DBG_PLU_SYNC
sprintf(MsgBuf, "[PS][CS](%d)s_id=%d, data_addr=%d, ip=%d.%d.%d.%d:%u\r\n", ret, scaleListId, data_addr[scaleListId - 1], EthCardStruct.TCPDestIP[3], EthCardStruct.TCPDestIP[2], EthCardStruct.TCPDestIP[1], EthCardStruct.TCPDestIP[0], EthCardStruct.TCPDestPort);
MsgOut(MsgBuf);
#endif
			}
			else
			{
				data_addr[scaleListId - 1]++;
				if (data_addr[scaleListId - 1] > MAX_SCROLL_NO)
				{
					data_addr[scaleListId - 1] = 1;
					//nextScale = ON;
					pluSendState = PLUSYNC_SEND_STATE_NEXT_WITHOUT_CLOSE;
				}
			}
			break;
		case PLUSYNC_SEND_STATE_SEND_SCROLL: // send scroll message
			if (!ChangeDestIPFlag)	// 전송전 다른 Client에 의해 연결이 끊길 경우
			{
				pluSendState = PLUSYNC_SEND_STATE_NEXT_WITHOUT_CLOSE;
				break;
			}
#ifdef _USE_DBG_PLU_SYNC
sprintf(MsgBuf, "[PS][SS]s_id=%d, data_addr=%d\r\n", scaleListId, data_addr[scaleListId - 1]);
MsgOut(MsgBuf);
#endif
			if (scroll_message_check_number(data_addr[scaleListId - 1]))
			{
				ret = commun_send_table3(&CommBufEthData[0], 0x73, 3, data_addr[scaleListId - 1], 0);
				pluSendState = PLUSYNC_SEND_STATE_WAIT_ACK_SCROLL;
			}
			else	// 없는 데이터 => 해당 scroll flag OFF
			{
				// Delete protocol 추가 되어야 함
				commun_send_table3_delete(&CommBufEthData[0], 3, data_addr[scaleListId - 1]);
				pluSendState = PLUSYNC_SEND_STATE_WAIT_ACK_SCROLL;
				//commun_net_mask(3, &data_addr[scaleListId - 1], 0xff, 0);// 해당 scroll flag OFF
				//pluSendState = PLUSYNC_SEND_STATE_CHECK_SCROLL;
			}
			waitTime = SysTimer100ms;
			break;
		case PLUSYNC_SEND_STATE_WAIT_ACK_SCROLL:	// wait ACK for scroll message
			if (!commun_net_mask(3, &data_addr[scaleListId - 1], scaleListId, 10)) // complete sending
			{
				if (commun_net_mask(3, &data_addr[scaleListId - 1], scaleListId, 11))	// search next PLU
				{
					pluSendState = PLUSYNC_SEND_STATE_SEND_SCROLL;
				}
				else
				{
					pluSendState = PLUSYNC_SEND_STATE_NEXT;
					//pluSendState = PLUSYNC_SEND_STATE_CHECK_PLU;
				}
			}
			else if ((INT16U)(SysTimer100ms - waitTime) > 30)	// wait 1.5sec 
			{
				pluSendState = PLUSYNC_SEND_STATE_NEXT;
#ifdef _USE_DBG_PLU_SYNC
sprintf(MsgBuf, "[PS][AS]timeout(%d)\r\n", scaleListId);
MsgOut(MsgBuf);
#endif
			}
			break;
// END SCROLL MSG ///////////////////////////
/////////////////////////////////////////////
		case PLUSYNC_SEND_STATE_NEXT:	// next step
			Ethernet_Server(CLIENT_CLOSE, NET_CHK_PLU_SYNC_WAITTIME, CLIENT_PLU_SYNC);
			//restore_dest_ip();	// Ethernet_Server()에서 ip를 복구하도록 수정
		case PLUSYNC_SEND_STATE_NEXT_WITHOUT_CLOSE:
			nextScale = ON;
			pluSendState = PLUSYNC_SEND_STATE_CHECK_PLU;
			break;
	}
	if (nextScale == ON)
	{
		while (++scaleListId <= MAX_SLAVE_SCALE)
		{
			if (NetSlaveInfo[scaleListId - 1].use && NetSlaveInfo[scaleListId - 1].ip[0]) break;
		}
	}
}

INT8U commun_send_plustrinfo(HUGEDATA COMM_BUF *CBuf,INT16S number)
{
	char temp[68],*commun_buffer;
	INT16S  maxid,l;
	INT16U value16,leng;
	INT32U value32;
	INT8U  value8;
	INT32U pnt;
	INT32U npnt;
	commun_buffer=(char *)serial_send_point;

	plu_n_field = plu_struct_get_nfield();
	maxid = plu_n_field;
	if ((INT16S)number>maxid) goto ERR;
	pnt  = DFLASH_BASE;
	pnt += FLASH_PLU_STRUCT_AREA;
	pnt += PLU_STRUCT_POINT;
	pnt += PLU_STRUCT_SIZE*(number-1);
	sprintf((char *)commun_buffer,"i=%04X.",number);
	leng=strlen((char *)commun_buffer);
	npnt = DFLASH_BASE;
	npnt+= FLASH_PLU_STRUCT_CAPTION_AREA;
	npnt+= PLUSTR_NAME_SIZE*(number-1);
	Flash_sread(npnt,(HUGEDATA INT8U*)temp,PLUSTR_NAME_SIZE);

	l=strlen(temp);
	sprintf((char *)&commun_buffer[leng],"C=%02X",l);
	leng=strlen((char *)commun_buffer);
	sprintf((char *)&commun_buffer[leng],"%s",temp);
	leng=strlen((char *)commun_buffer);

	value16=Flash_wread(pnt+PLUSTR_PTYPE_POINT);
	sprintf((char *)&commun_buffer[leng],"P=%04X.",value16);
	leng=strlen((char *)commun_buffer);
	value32=Flash_lread(pnt+PLUSTR_VDEFAULT_POINT);
	sprintf((char *)&commun_buffer[leng],"D=%08lX.",value32);
	leng=strlen((char *)commun_buffer);
	value32=Flash_lread(pnt+PLUSTR_MAXVALUE_POINT);
	sprintf((char *)&commun_buffer[leng],"M=%08lX.",value32);
	leng=strlen((char *)commun_buffer);
	Flash_sread(pnt+PLUSTR_STYPE_POINT,(HUGEDATA INT8U*)&value8,1);
	sprintf((char *)&commun_buffer[leng],"S=%02X.",value8);
	leng=strlen((char *)commun_buffer);
	Flash_sread(pnt+PLUSTR_LENGTH_POINT,(HUGEDATA INT8U*)&value8,1);
	sprintf((char *)&commun_buffer[leng],"L=%02X.",value8);
	leng=strlen((char *)commun_buffer);
	Flash_sread(pnt+PLUSTR_LINENO_POINT,(HUGEDATA INT8U*)&value8,1);
	sprintf((char *)&commun_buffer[leng],"N=%02X.",value8);
	leng=strlen((char *)commun_buffer);

	Flash_sread(pnt+PLUSTR_DPOINT_POINT,(HUGEDATA INT8U*)&value8,1);
	sprintf((char *)&commun_buffer[leng],"O=%02X.",value8);
	leng=strlen((char *)commun_buffer);

	value16=Flash_wread(pnt+PLUSTR_USIZE_POINT);
	sprintf((char *)&commun_buffer[leng],"Z=%04X.",value16);
	leng=strlen((char *)commun_buffer);

	sprintf((char *)temp,"R01:L%04X,",leng);
	commun_out(CBuf,temp);

	leng=strlen((char *)commun_buffer);
	commun_buffer[leng]=gen_bcc((INT8U *)commun_buffer,leng);
	leng++;
	commun_outleng(CBuf,(char *)commun_buffer,leng);
	return 1;
ERR:
	return 0x80;
}

INT8U commun_send_plu_nth(HUGEDATA COMM_BUF *CBuf,INT32U nth,INT32U pcode)
/****************************************
 * PLU Number (pluid) 를 전송한다.
 * nth 는 Item Number
 ****************************************/
{
	INT32U address;
	INT8U  ret;

	nth  &=0xffffff;
	address=plu_get_next_exist_address((INT16U)nth);
	ret=1;
	if (address==0xffff) { address=0l; pcode=MAX_PLU_NUMBER+1; }
	ret=commun_send_plu_from_address(CBuf,(INT16U)address,pcode,1);
	return ret;
}

INT8U commun_send_fontbin(HUGEDATA COMM_BUF *CBuf,INT16S number)
{
	INT32U addr,pnt,sz,nsz;
	INT16S    leng,maxn,readl;
	char temp[68],*commun_buffer;

	commun_buffer=(char *)serial_send_point;

	addr  = DFLASH_BASE;
	addr += FLASH_FONT_AREA;
	pnt   = addr + FONT_IMAGE;
	sz    = Flash_lread(addr + FONT_IMAGE_SIZE);
	if (number==0) goto ERR;
	nsz  = (number-1);
	nsz *= 512l;
	pnt += nsz;

	sprintf((char *)commun_buffer,"B=%06X.",number);
	leng=strlen((char *)commun_buffer);
	maxn = sz/512;
	readl= 512;
	if (maxn==number) readl=sz%512;
	Flash_sread(pnt,(HUGEDATA INT8U *)&commun_buffer[leng],readl);
	leng+=readl;
	sprintf((char *)temp,"R03:L%04X,",leng);
	commun_out(CBuf,temp);
	commun_buffer[leng]=gen_bcc((INT8U *)commun_buffer,leng);
	leng++;
	commun_outleng(CBuf,(char *)commun_buffer,leng);
	return 1;
ERR:
	return 0x80;
}

INT8U commun_send_fontinfo(HUGEDATA COMM_BUF *CBuf,INT16S number)
{
	INT32U addr,v32,pnt;
	INT16S    i1,i2,i,leng;
	INT8U  v8;
	char temp[68],*commun_buffer;

	commun_buffer=(char *)serial_send_point;

	addr  = DFLASH_BASE;
	addr += FLASH_FONT_AREA;
	pnt = addr + FONT_NUMBER;
	v8  = Flash_bread(pnt);

	if (number==0) {
	   i1=0; i2=v8;
	} else {
	   number--;
	   i1=number; i2=number+1;
	   if (number>v8) goto ERR;
	}
	sprintf((char *)commun_buffer,"N=%02X.",v8);
	leng=strlen((char *)commun_buffer);
	for (i=i1; i<i2; i++) {
	   sprintf((char *)&commun_buffer[leng],"i=%02X.",i);
	   leng=strlen((char *)commun_buffer);
	   pnt = addr+FONT_INFO_POINT+FONT_INFO_SIZE*i;
	   v8  = Flash_bread(pnt);
	   sprintf((char *)&commun_buffer[leng],"D=%02X.",v8); // Font ID
	   leng=strlen((char *)commun_buffer);
	   pnt+= 1;
	   v8  = Flash_bread(pnt);
	   sprintf((char *)&commun_buffer[leng],"T=%02X.",v8); // Font Type
	   leng=strlen((char *)commun_buffer);
	   pnt+= 1;
	   v32  = Flash_bread(pnt);
	   sprintf((char *)&commun_buffer[leng],"A=%06lX.",v32); // Font Type
	   leng=strlen((char *)commun_buffer);
	}
	leng=strlen((char *)commun_buffer);
	sprintf((char *)temp,"R03:L%04X,",leng);
	commun_out(CBuf,temp);
	commun_buffer[leng]=gen_bcc((INT8U *)commun_buffer,leng);
	leng++;
	commun_outleng(CBuf,(char *)commun_buffer,leng);
	return 1;
ERR:
	return 0x80;
}

INT8U commun_send_key(HUGEDATA COMM_BUF *CBuf,INT16S key_mode,INT16S keyno)
{
	INT32U p;
	INT8U bcc; //,*data;
	INT16U ipole;
	INT16S    i,l,r,sz;
	char   *commun_buffer;

	commun_buffer=(char *)serial_send_point;

	sz=2;
	l=KEY_MAX_TABLE_SIZE;
	switch (key_mode) {
		case 1: p=FLASH_KEY_AREA;  break;
		case 2: p=FLASH_KEY_SHIFT_AREA; break;
		case 3: p=FLASH_KEY_PGM_AREA; break;
		case 4: p=FLASH_KEY_PGM_SHIFT_AREA; break;
		case 5: p=FLASH_KEY_PGM2_AREA; break;
		case 6: p=FLASH_KEY_PGM2_SHIFT_AREA; break;
		case 7:	p=FLASH_KEY_CUSTOMER_AREA; break;
		case 41: p=FLASH_PSKEY_AREA;  break;
		case 42: p=FLASH_PSKEY_SHIFT_AREA; break;
		case 43: p=FLASH_PSKEY_PGM_AREA; break;
		case 44: p=FLASH_PSKEY_PGM_SHIFT_AREA; break;
		case 45: p=FLASH_PSKEY_PGM2_AREA; break;
		case 46: p=FLASH_PSKEY_PGM2_SHIFT_AREA; break;
		case 10:
		case 11:p=FLASH_KEY_AREA;       l+=KEY_MAX_TABLE_SIZE; break;
		case 12:
		case 13:p=FLASH_KEY_PGM_AREA ;  l+=KEY_MAX_TABLE_SIZE; break;
		case 14:
		case 15:p=FLASH_KEY_PGM2_AREA;  l+=KEY_MAX_TABLE_SIZE; break;
		case 31:p=FLASH_KEY_CDTC_AREA;     l=64; break;			//CLERK		JJANG 20070502
		case 32:p=FLASH_KEY_CDTC_AREA+192; l=64; break;			//DEPT
		case 33:p=FLASH_KEY_CDTC_AREA+128; l=64; break;			//TARE
//		case 34:p=FLASH_KEY_CDTC_AREA+192; l=64; break;			//CURRENCY	delete by JJANG 20070502
		default:if ((key_mode>=21) && (key_mode<=25)) {
				p=(key_mode-21)*MAX_SPEEDKEY_SIZE;
				p+=FLASH_KEY_SPEED_AREA;
				sz=4;
				l =MAX_SPEEDKEY_SIZE;
			} else return 0x80;

	}
	if ((key_mode>=40) && (key_mode<50)) {
		l=160*2;
	}
	p+=DFLASH_BASE;
    	if (keyno) {
		p +=(keyno-1)*sz;
		l  = sz;
	}
	r=19;
	for (i=0; i<l; i++) {
		commun_buffer[r++]=Flash_bread(p+i);
	}
	bcc=gen_bcc((INT8U *)&commun_buffer[19],r-19);
	ipole = (INT16U)get_global_bparam(GLOBAL_AREA_SCALETYPE);
	ipole<<=8;
	key_mode|=ipole;
	sprintf((char *)commun_buffer,"W04F%04X,",key_mode);
	sprintf((char *)&commun_buffer[9],"%04XL%04X",keyno,l);
	commun_buffer[18]=':';
// 01234567890123456789
// W04F0000,0000L0000:
	commun_buffer[r++]=bcc;
	commun_outleng(CBuf,(char *)commun_buffer,r);
	return 1;
}

INT8U commun_send_caption(HUGEDATA COMM_BUF *CBuf,INT16U mode,INT16U id)
{
	char   str[CAP_MSG_LENGTH];
	INT32U v32,address;
	INT16U v16;
	INT8U  v8,bcc;
	INT16S    r,l;
	char *commun_buffer;
	commun_buffer=(char *)serial_send_point;

	if (mode==0) return 0x80;
	if (mode>2 ) return 0x80;
	if (id==0  ) return 0x80;
	if (mode==2) { // By ID
		address=caption_search_by_code(id);
	} else {
#ifdef USE_EXTEND_CAPNTION_AREA
		if (id-1 >= MID_CAPTION_NO) {
			address =(INT32U)(id-MID_CAPTION_NO-1);
			address*=CAP_MSG_LENGTH;
			address+=DFLASH_BASE;
			address+=FLASH_CAPTION_EXT_AREA;
		}
		else
		{
			address =(INT32U)(id-1);
			address*=CAP_MSG_LENGTH;
			address+=DFLASH_BASE;
			address+=FLASH_CAPTION_AREA;
		}
#else
		address =(INT32U)(id-1);
		address*=CAP_MSG_LENGTH;
		address+=DFLASH_BASE;
		address+=FLASH_CAPTION_AREA;
#endif
	}
	if (address==0) return FALSE;
	sprintf((char *)commun_buffer,"i=%04X.",id);
	r=strlen((char *)commun_buffer);

	v16=Flash_wread(address+CAPTION_CODE_POINT);
	sprintf((char *)&commun_buffer[r],"N=%04X.",v16);
	r=strlen((char *)commun_buffer);
	if (v16==0xffff) goto END;
	Flash_sread(address+CAPTION_NAME_POINT,(HUGEDATA INT8U *)str,32);

	str[32]=0;  l=strlen(str);
	sprintf((char *)&commun_buffer[r],"C=%02X%s",l,str);
	r+=strlen((char *)&commun_buffer[r]);

	v16=Flash_wread(address+CAPTION_INPUTTYPE_POINT);
	sprintf((char *)&commun_buffer[r],"I=%04X.",v16);
	r+=strlen((char *)&commun_buffer[r]);

	v32=Flash_lread(address+CAPTION_SAVEADDRESS_POINT);
	sprintf((char *)&commun_buffer[r],"A=%08lX.",v32);
	r+=strlen((char *)&commun_buffer[r]);

	v16=Flash_wread(address+CAPTION_MENUSTART_POINT);
	sprintf((char *)&commun_buffer[r],"m=%04X.",v16);
	r+=strlen((char *)&commun_buffer[r]);

	v16=Flash_wread(address+CAPTION_NITEM_POINT);
	sprintf((char *)&commun_buffer[r],"n=%04X.",v16);
	r+=strlen((char *)&commun_buffer[r]);

	v8=Flash_bread(address+CAPTION_X_POINT);
	sprintf((char *)&commun_buffer[r],"X=%02X.",v8);
	r+=strlen((char *)&commun_buffer[r]);

	v16=Flash_wread(address+CAPTION_LENG_POINT);
	sprintf((char *)&commun_buffer[r],"L=%04X.",v16);
	r+=strlen((char *)&commun_buffer[r]);

	v16=Flash_wread(address+CAPTION_DLENG_POINT);
	sprintf((char *)&commun_buffer[r],"D=%04X.",v16);
	r+=strlen((char *)&commun_buffer[r]);

	v8=Flash_bread(address+CAPTION_ITYPE_POINT);
	sprintf((char *)&commun_buffer[r],"S=%02X.",v8);
	r+=strlen((char *)&commun_buffer[r]);

	v32=Flash_lread(address+CAPTION_XMIN_POINT);
	sprintf((char *)&commun_buffer[r],"a=%08lX.",v32);
	r+=strlen((char *)&commun_buffer[r]);

	v32=Flash_lread(address+CAPTION_XMAX_POINT);
	sprintf((char *)&commun_buffer[r],"b=%08lX.",v32);
	r+=strlen((char *)&commun_buffer[r]);
END:
	sprintf((char *)str,"R05:L%04X,",r);

	bcc=gen_bcc((INT8U *)commun_buffer,r);

	commun_buffer[r++]=bcc;
	commun_out(CBuf,str);
	commun_outleng(CBuf,(char *)commun_buffer,r);
	return 1;
}

INT8U commun_send_labelbmp(HUGEDATA COMM_BUF *CBuf,INT8U mode,INT16S number,INT16S fnumber)
{
	INT32U addr,src_addr;
	INT16U labelid,imagesize,sz,img_w,img_h,n_address,bz;
	INT8U  ret,last,blockid,formid;
	INT16S    rdata,r;
	char   *commun_buffer;
	char   img_name[32];
	INT8U  temp[32];
	INT8U  tempSize;
	INT8U  xmode;
	commun_buffer=(char *)serial_send_point;

	ret = 0x85;
	blockid = fnumber>>8;
	fnumber&=0xff;

	if (number ==0) goto ERR;
	if (number >99) goto ERR;
	if (fnumber==0) goto ERR;

	sz        =(fnumber-1);
	sz       *=512l;
	n_address = 999;

	switch (mode) {
	   case 0x057: n_address=get_new_labeladdress( (INT16U)number,1); break;
	   case 0x056: n_address=get_new_bitmapaddress((INT16U)number,1); break;
	   default   : n_address=(INT16U)number; break;
	}
	if (n_address==999) { ret=0x84; goto ERR; }
	xmode = mode & 0x01;
//	addr  = (INT32U)(n_address-1);
	addr  = (INT32U)(n_address);

	if (xmode) {
		bz = LABEL_INFO_SIZE;
	} else {
		bz = BITMAP_INFO_SIZE;
	}
	addr*=(INT32U)bz;
	addr+=DFLASH_BASE;
	if (xmode) {
		struct LABEL_HEADER_INFO info;
		memset((void*)&info, 0, sizeof(info));
		tempSize=24;
		
		addr = get_addr_from_label_idx((INT16U)n_address, LABEL_AREA_HEADER);

		read_label_header(addr, &info, L_H_ALL_BIT);

		labelid = info.label_id;
		img_w = info.img_w;
		img_h = info.img_h;
		strncpy(img_name, info.img_name, LABEL_HEADER_IMG_NAME_SIZE);
		imagesize = info.img_size;

		if ((labelid==0) || (labelid>99)) { ret=0x84; goto ERR; }
		if (sz>=imagesize) { ret=0x99; goto ERR; }
		bz      = LABEL_MAX_SIZE;
		formid = 1;
	} else {
		tempSize=6;
		addr += FLASH_BITMAP_AREA;
		labelid  =Flash_wread(addr);	// bitmap id
		if ((labelid>0) && (labelid<=MAX_BITMAP_ID)) {
			Flash_sread(addr,(HUGEDATA INT8U *)temp,tempSize );
			img_w = Flash_wread(addr+2);
			img_h = Flash_wread(addr+4);
			imagesize=img_w/8;
			if (img_w%8) imagesize++;
			imagesize=imagesize * img_h;
			if (imagesize>BITMAP_MAX_SIZE) goto BERR;
			if (sz>=imagesize) { ret=0x99; goto ERR; }
			bz      = BITMAP_MAX_SIZE;
			src_addr  = BITMAP_IMAGE;
			src_addr += FLASH_BITMAP_AREA;
			formid=2;
		} else {
		BERR:	ret=0x84;
			goto ERR;
		}
	}
	rdata=512;
	last =0; 
	if (sz+512>=imagesize) {
		rdata=imagesize%512;
		last=1;
	}
	
	addr = get_addr_from_label_idx((INT16U)n_address, LABEL_AREA_IMAGE);
	addr += sz; // block number

	r    = 19;
	ethernet_gen_head(&commun_buffer[r]);
	r+=strlen(&commun_buffer[r]);	  

	if (blockid==1) {
		memcpy((char *)&commun_buffer[r],(char *)temp,tempSize);
		sz+=tempSize;
	} else {
		sprintf((char *)&commun_buffer[r],"N=%04X.s=%d.D=%04X:",fnumber,last,rdata);
		sz=(INT16U)strlen((char *)&commun_buffer[r]);
	}
	r   += sz;
	if (blockid==2) {
		Flash_sread(addr,(HUGEDATA INT8U *)&commun_buffer[r],rdata);
		r   += rdata;
	}
	// 0123456789012345678
	// W00F0000,0000L0000
	sprintf(commun_buffer,"W%02XF%04X,",mode,labelid);
	sprintf(&commun_buffer[9],"%02X%02XL%04X",formid,blockid,r-19);
	commun_buffer[18]=':';
	commun_buffer[r]=gen_bcc((INT8U *)&commun_buffer[19],r-19);
	r++;
	commun_outleng(CBuf,(char *)commun_buffer,r);
	ret=1;
ERR:
	return ret;
}

// cmd - 0x09 : request PLU from PC
//       0x19 : request PLU by remote call(check sync flag)
// "R<cmd>F<pluno>,<deptno>"
INT8U commun_send_discountbyplu(HUGEDATA COMM_BUF *CBuf, INT8U scaleListId, INT16U cmd, INT32U pluno, INT16U deptno)
{
	INT16U dc_no;
	INT8U  r;
	INT16S sz;
	char   *commun_buffer;

	dc_no = _get_dc_number_net(scaleListId, (INT16U)deptno, pluno, &r);
	if (dc_no>=MAX_DISCOUNT_NO) return 0x99;
	if (cmd==0x19) {	// remote call(request plu)
		commun_buffer=(char *)serial_send_point;
		if (dc_no==0) {
			sz=ethernet_gen_head(&commun_buffer[0]);
			sprintf(&commun_buffer[sz],"C01F19,%02X%06lX%c",deptno,pluno,0x0a);
			commun_outleng(CBuf,commun_buffer,strlen(commun_buffer));
			return 0;
		} else {
			if (!r) return 0x53;
		}
	}
	r=commun_send_discount(CBuf,cmd,dc_no,0,0);
	return r;
}

INT8U commun_send_discount(HUGEDATA COMM_BUF *CBuf,INT16U cmd,INT16U sdcno,INT8U mode,INT8U rept)
{
	char   *commun_buffer;
	DISCOUNT_STRUCT	 disc_str;
	INT32U addr;
	INT16S    com_type,r;
	INT16U dc_no;
	INT8U  bcc,srr;
	dc_no = sdcno;
	com_type=serial_get_comtype(CBuf);
	commun_buffer=(char *)serial_send_point;
REPT:
	if (mode) {
		switch (mode) {
			case 1:	addr = FLASH_DISCOUNT_GLOBAL_WEIGHT; break;
			case 3:	addr = FLASH_DISCOUNT_GLOBAL_COUNT; break;
			case 2:	addr = FLASH_DISCOUNT_GLOBAL_NONFOOD; break;
			default: mode=0;
		}
		goto GDC;
	}
	if (dc_no<=0) return 0x98;
	if (dc_no>=MAX_DISCOUNT_NO) return 0x95;
	if (dc_no) {
GDC:
		addr = get_addr_discount_area(dc_no);
		if (!addr) { /* 에러 처리 */
			return 0x99;
		}
		Flash_sread(addr, (HUGEDATA INT8U*)&disc_str, sizeof(disc_str));
	}
 	r = 17;
	ethernet_gen_head(&commun_buffer[r]);
	r+=strlen(&commun_buffer[r]);
 	srr=0;

 	if (disc_str.dept_no<=0) srr=1;
	if (disc_str.dept_no>=MAX_DEPT_NO) srr=1;
	if (disc_str.plu_no<=0l) srr=1;
	if (disc_str.plu_no>MAX_PLU_NUMBER) srr=1;
	if (srr) {
	    if (mode) {
		disc_str.dept_no=0;
		disc_str.plu_no =0;
	    } else {
		dc_no ++;
		if (rept) goto REPT;
		else return 0x97;
	    }
	}
	sprintf(&commun_buffer[r],"a=%02X." ,disc_str.dept_no  ); r+=strlen(&commun_buffer[r]);
	sprintf(&commun_buffer[r],"b=%lX."  ,disc_str.plu_no   ); r+=strlen(&commun_buffer[r]);

	sprintf(&commun_buffer[r],"c=%04X." ,disc_str.type     ); r+=strlen(&commun_buffer[r]);
	sprintf(&commun_buffer[r],"d=%lX."  ,disc_str.target[0]); r+=strlen(&commun_buffer[r]);
	sprintf(&commun_buffer[r],"e=%lX."  ,disc_str.target[1]); r+=strlen(&commun_buffer[r]);
	sprintf(&commun_buffer[r],"f=%lX."  ,disc_str.price[0] ); r+=strlen(&commun_buffer[r]);
	sprintf(&commun_buffer[r],"g=%lX."  ,disc_str.price[1] ); r+=strlen(&commun_buffer[r]);
	sprintf(&commun_buffer[r],"h=%02X%02X%02X.",disc_str.start_year,disc_str.start_month,disc_str.start_day); r+=strlen(&commun_buffer[r]);
	sprintf(&commun_buffer[r],"i=%02X%02X%02X.",disc_str.start_hour,disc_str.start_minute,disc_str.start_second); r+=strlen(&commun_buffer[r]);
	sprintf(&commun_buffer[r],"j=%02X%02X%02X.",disc_str.end_year,disc_str.end_month,disc_str.end_day); r+=strlen(&commun_buffer[r]);
	sprintf(&commun_buffer[r],"k=%02X%02X%02X.",disc_str.end_hour,disc_str.end_minute,disc_str.end_second); r+=strlen(&commun_buffer[r]);
	sprintf(&commun_buffer[r],"l=%02X.",disc_str.day_of_week); r+=strlen(&commun_buffer[r]);
	sprintf(&commun_buffer[r],"m=%02X." ,disc_str.useForFSP  ); r+=strlen(&commun_buffer[r]);
//	sprintf(&commun_buffer[r],"S=%02X.",status); r+=strlen(&commun_buffer[r]);
	bcc=gen_bcc((INT8U *)&commun_buffer[17],r-17);
	sprintf(commun_buffer,"W%02xA%04X,",cmd,dc_no);
	sprintf(&commun_buffer[9],"%02XL%04X",mode,r-17);
	commun_buffer[16]=':';
	commun_buffer[r++]=bcc;
	if ((com_type==0) || (ethernet_list.status)) {
		commun_outleng(CBuf,(char *)commun_buffer,r);
	}
	return 1;
}

// cmd - 0x15 : 무조건 PLU 전송
//       0x14 : sync 여부에 따라 PLU 전송. remote plu 사용시 traffic을 줄임
// "R<cmd>F<nth>,<pcode>"
INT8U commun_send_plu(HUGEDATA COMM_BUF *CBuf,ETH_CONNECT_LIST eth,INT16U cmd,INT32U nth,INT32U pcode)
{
	INT16U address;//,addr;
	INT8U  ret;
	INT16U deptno,flag;
	INT32U pluno;
	char *commun_buffer;
	INT8U listId;

	commun_buffer=(char *)serial_send_point;

	deptno=(nth>>24); // 
	pluno = nth & 0xffffff;
	
	plu_table_search((INT8U)deptno,(long)pluno,&address,0);
	ret=1;
	if (address==0xffff) { address=0; pcode=MAX_PLU_NUMBER+1; }
	if ((cmd==0x14) || (cmd==0x015)) {
		if (address==0) {
			flag=ethernet_gen_head(&commun_buffer[0]);
			sprintf(&commun_buffer[flag],"C01F15,%02X%06lX%c",deptno,pluno,0x0a);
			commun_outleng(CBuf,commun_buffer,strlen(commun_buffer));
			return 0;
		} else {
			if (cmd==0x14) {	// sync여부에 따라 PLU 전송여부 결정
				listId = network_get_scale_list_id(eth.ip);
				ret = commun_net_mask(0, &address, listId, 10);
				if (ret) goto SEND;	// sync 안되있으면 전송
				else return 0x52;	// sync 되어 있으면 전송 안함
			}
		}
	}
SEND:
	ret=commun_send_plu_from_address(CBuf,address,pcode,1);
	return ret;
}
#define MAX_TABLE_BUF	114 	//Due to USE_BARCODE_LEN_90, 50->114
#define MAX_TABLE_BUF_SLAUGHTER	128
INT8U commun_send_table1(HUGEDATA COMM_BUF *CBuf,INT32U spart,INT16U sid,INT8U next)
// ==================================================================
// next=1 이면 현재 요청된 값을 못찾을때 다음 ID로 값을 Return 한다.
// ==================================================================
{
	INT16S    r,i;
	INT16S    part;//,erase;
	INT16U id;
	ETH_CONNECT_LIST eth;
	char   temp[80];
	char   desc1[MAX_TABLE_BUF],desc2[MAX_TABLE_BUF];
	INT8U  xtype,formno,r_start,bcc;
	INT16U skey,sz;
	INT32U rvalue,p;
#ifdef USE_TAX_RATE    
	TAX_STRUCT  	tax;
#endif    
	TARE_STRUCT	tare;
	BARCODE_STRUCT  barcode;
#ifdef USE_FIXED_PERCENT_KEY_SET
	DISCOUNT_KEY_STRUCT discKey;
#endif
	char *commun_buffer;
#ifdef USE_TRACE_MEATGRADE
	INT16U order;
#endif

	memset((INT8U *)&eth,0,sizeof(ETH_CONNECT_LIST));
	commun_buffer=(char *)serial_send_point;
	part = (INT16S)spart;
	if (part<=0) return 0x82;
NEXT_ID:;
	id   = (INT16U)sid;
	if (id  <=0) return 0x82;
	memset(desc1,0,MAX_TABLE_BUF);
	memset(desc2,0,MAX_TABLE_BUF);
	switch (part) {
	    case 1: if (sid>MAX_DEPT_NO-1) return 0x99;

		    sz=DEPARTMENT_STRUCT_SIZE;
		    p =(id-1)*sz;
		    p+=FLASH_DEPARTMENT_AREA;
		    p+=DFLASH_BASE;
		    Flash_sread(p,(HUGEDATA INT8U*)temp,sz);
		    memcpy((char *)&id  ,&temp[0],2);
		    if (id!=sid) id=0;
		    if ((id<=0) || (id>MAX_DEPT_NO-1)) id=0;
		    else {
			    memcpy((char *)&skey,&temp[2],2);
			    memcpy(desc1,&temp[4],28);
		    }
		    break;
 	    case 2: if (sid>MAX_GROUP-1) return 0x99;
		    sz = GROUP_STRUCT_SIZE;
		    p =(id-1)*sz;
		    p+= FLASH_SUB_DEPARTMENT_AREA;
		    p+= DFLASH_BASE;

		    Flash_sread(p,(HUGEDATA INT8U*)temp,sz);
		    memcpy((char *)&id  ,&temp[0],2);
		    if (id!=sid) id=0;
		    if ((id<=0) || (id>MAX_GROUP-1)) id=0;
		    else {
			    memcpy(desc1,&temp[2],30);
#ifdef USE_TRACE_MEATGRADE
			    memcpy(&order, &temp[30], 2);
#endif
		    }
		    break;
	    case 3: if (sid>MAX_LABEL_MSG_NO-1) return 0x99;
		    sz=PRT_ADVE_STRUCT_SIZE;
		    p =(id-1)*sz;
		    p+=FLASH_PRT_ADVE_AREA;
		    p+=DFLASH_BASE;
		    Flash_sread(p,(HUGEDATA INT8U*)temp,sz);
		    memcpy((char *)&id  ,&temp[0],2);
		    if (id!=sid) id=0;
		    if ((id<=0) || (id>MAX_LABEL_MSG_NO-1))  id=0;
		    else {
			    memcpy(desc1,&temp[2],PRT_ADVE_STRUCT_SIZE - 2);
		    }
		    break;
	    case 4: if (sid>MAX_ORIGIN_NO) return 0x99;
		    sz=ORIGIN_STRUCT_SIZE;
		    p =(id-1)*sz;
		    p+=FLASH_ORIGIN_AREA;
		    p+=DFLASH_BASE;
		    Flash_sread(p,(HUGEDATA INT8U*)temp,sz);
		    memcpy((char *)&id  ,&temp[0],2);
		    if (id!=sid) id=0;
		    if ((id<=0) || (id>MAX_ORIGIN_NO)) id=0;
		    else {
			    memcpy(desc1,&temp[2],32);
		    }
		    break;
//Modified by JJANG 20081009
	    case 5: if (sid>MAX_PCS_NO) return 0x99;
		    sz=PCS_UNIT_STRUCT_SIZE;
		    p =(id-1)*sz;
		    p+=FLASH_PCS_UNIT_AREA;
		    p+=DFLASH_BASE;
		    Flash_sread(p,(HUGEDATA INT8U*)temp,sz);
		    memcpy((char *)&id  ,&temp[0],2);
		    if (id!=sid) id=0;
		    if ((id<=0) || (id>MAX_PCS_NO))   id=0;
		    else 
		    {
			    memcpy(desc1,&temp[2],6);
		    }
		    break;
#ifdef USE_TAX_RATE            
	    case 6: if (sid>MAX_TAX_NO-1) return 0x99;
		    sz=TAX_STRUCT_SIZE;
		    p =(id-1)*sz;
		    p+=DFLASH_BASE;
		    p+=FLASH_TAX_AREA;
		    memset(&tax,0,sz);
		    Flash_sread(p,(HUGEDATA INT8U *)&tax,sz);
	    	    id     = tax.tax_code;
		    if (id!=sid) id=0;
		    if ((id<=0) || (id>MAX_TAX_NO-1))  id=0;
		    else {
			    xtype  = tax.tax_type;
			    rvalue = tax.tax_rate;
		    }
		    break;
#endif            
	    case 7: if (sid>MAX_TARE_NO) return 0x99;
		    sz=TARE_STRUCT_SIZE;
		    p =(id-1)*sz;
		    p+=FLASH_TARE_AREA;
		    p+=DFLASH_BASE;
		    memset(&tare,0,sz);
		    Flash_sread(p,(HUGEDATA INT8U *)&tare,sz);
		    id    =tare._code;
		    if (id!=sid) id=0;
		    if ((id<=0) || (id>MAX_TARE_NO))  id=0;
		    else {
			    memcpy(desc1,tare._description,17);
			    xtype=tare._type;
			    rvalue=tare._value;
		    }
		    break;
	    case 8: 
			if (sid>MAX_BARCODE_NO) return 0x99;
		    sz=BARCODE_STRUCT_SIZE;
		    p =(id-1)*sz;
		    p+=FLASH_BARCODE_FORMAT_AREA;
		    p+=DFLASH_BASE;
		    Flash_sread(p,(HUGEDATA INT8U*)&barcode,sz);
		    id = barcode.barcode_num;
		    if (id!=sid) id=0;
		    if ((id<=0) || (id>MAX_BARCODE_NO)) id=0;
		    else {
			    xtype=barcode.barcode_type;
			    memcpy(desc1,barcode.barcode_description,sizeof(barcode.barcode_description));
			    desc1[sizeof(barcode.barcode_description)] = 0;
			    memcpy(desc2,barcode.barcode_format     ,BARCODE_FORMAT_LEN);
			    desc2[BARCODE_FORMAT_LEN] = 0;
			    formno=barcode.barcode_format_num;
		    }
		    break;
#ifdef USE_FIXED_PERCENT_KEY_SET            
	    case 9: 
			if (sid>MAX_DISCOUNT_KEY_NO) return 0x99;
		    sz=DISCOUNT_KEY_STRUCT_SIZE;
		    p =(id-1)*sz;
		    p+=DFLASH_BASE;
		    p+=FLASH_DISCOUNT_RATE_AREA;
		    Flash_sread(p,(HUGEDATA INT8U *)&discKey,sz);
			id = discKey.code;
		    if (id!=sid) id=0;
		    if ((id<=0) || (id>MAX_DISCOUNT_KEY_NO))  id=0;
		    else {
			    rvalue=(INT32U)discKey.rate;
		    }
		    break;
#endif
	}
	if (id==0) {
		if (next) { sid++; goto NEXT_ID; }
		else return 0x82;
	}
	r_start=16;
	r=r_start;
	ethernet_gen_head(&commun_buffer[r]);
	r+=strlen(&commun_buffer[r]);
	if (id) {
#ifdef USE_FIXED_PERCENT_KEY_SET
	    if (part!=6 && part!=9) {
#else
		if (part!=6) {
#endif
		i=strlen(desc1);
		sprintf(&commun_buffer[r],"D=%02X.",i);
		r+=strlen(&commun_buffer[r]);
		sprintf(&commun_buffer[r],"%s",desc1);
		r+=strlen(&commun_buffer[r]);
	    } 
	    if (part==8) {
		i=strlen(desc2);
		sprintf(&commun_buffer[r],"F=%02X.",i);
		r+=strlen(&commun_buffer[r]);
		sprintf(&commun_buffer[r],"%s",desc2);
		r+=strlen(&commun_buffer[r]);
		sprintf(&commun_buffer[r],"N=%03X.",formno);
		r+=strlen(&commun_buffer[r]);
	    }
	    if (part==1) {
		sprintf(&commun_buffer[r],"K=%02X.",skey);
		r+=strlen(&commun_buffer[r]);
	    }
	    if ((part==6) || (part==7) || (part==8)) {
		sprintf(&commun_buffer[r],"T=%02X.",xtype);
		r+=strlen(&commun_buffer[r]);
	    }
#ifdef USE_FIXED_PERCENT_KEY_SET
	    if (part==9){
		sprintf(&commun_buffer[r],"V=%02X.",rvalue);
		r+=strlen(&commun_buffer[r]);
	    }
#endif
		if ((part==6) || (part==7)) {
		sprintf(&commun_buffer[r],"V=%06lX.",rvalue);
		r+=strlen(&commun_buffer[r]);
	    }
#ifdef USE_TRACE_MEATGRADE
	    if (part == 2)	// group
	    {
		sprintf(&commun_buffer[r],"G=%02X.", order);
		r+=strlen(&commun_buffer[r]);
	    }
#endif
	} else {
	    sprintf(&commun_buffer[r],"B=1.");
	    r+=strlen(&commun_buffer[r]);
	}
	bcc=gen_bcc((INT8U *)&commun_buffer[r_start],r-r_start);
	sprintf(commun_buffer,"W20F%02X,%03XL",part,id);
	sprintf(&commun_buffer[10],"L%04X",r-r_start);
	commun_buffer[r_start-1]=':';
	commun_buffer[r++]=bcc;

	commun_outleng(CBuf,(char *)commun_buffer,r);
	return 1;
}
#ifdef USE_KOR_SLAUGHT_CODE_4DIGIT
extern INT16U SlaughtTableLastId;
#endif
INT8U commun_send_table2(HUGEDATA COMM_BUF *CBuf,INT8U cmd,INT32U spart,INT32U sid,INT8U next, INT8U request)
{
	INT16S    r,i;
	INT16S    part;//,erase;
	INT16U id;
	ETH_CONNECT_LIST eth;
	INT16S    r_start,sarray;
	char   desc1[MAX_TABLE_BUF],desc2[MAX_TABLE_BUF],desc3[MAX_TABLE_BUF],cc;
	INT8U  xtype;
	INT8U  bcc;
	INT16U sz,nth;
#ifdef USE_EU_NUTRITIONFACT
	INT32U	varray[45];
#elif defined(USE_ADDITIONAL_NUTRIENT)
	INT32U	varray[30];	
#else
	INT32U	varray[20];
#endif
	INT32U rvalue,dsize,p,rp;
	NUTRIFACT_STRUCT nutri_fact;
	NUTRIFACT_STRUCT2 nutri_fact2;
#ifdef USE_EU_NUTRITIONFACT
	NUTRIFACT_STRUCT3 nutri_fact3;
#endif
#ifdef USE_ADDITIONAL_NUTRIENT
	NUTRIFACT_STRUCT4 nutri_fact4;
#endif
#ifdef USE_TRACEABILITY
	TRACEABILITY_STRUCT trace;
	WORKPLACE_STRUCT work;
	char   temp[72];
#endif
	char *commun_buffer;
	INT16U ing_len;
	INT16U block_count;
	INT16U block_size = INGREDIENT_COM_BLOCK_SIZE;
	char ing_str[MAX_INGREDIENT_SIZE];
	INT8U ucTemp, nutri_type=0;

#ifdef USE_9999_INDIRECT_INGREDIENT
	INT16U	 ing_id;
	INT16U	 ing_table_id;
#endif
#ifdef USE_TRACE_1000QTY //sewon 161222
	INT8U  meatKind;
	INT8U  slaughtNo;
	INT8U harmful;
	char  desc7[MAX_TABLE_BUF];	
#endif  
#ifdef USE_INGREDIENT_CODE_4DIGIT
	INT8U being;
	INT16U tableId, index, tempId;
#endif

#ifdef USE_TRACE_STANDALONE
	char   temp[256];
	TRACE_INDIVIDUAL_IDENTITY indivStr;
	long  indivWeight;
	char  desc4[MAX_TABLE_BUF];
	char  desc5[MAX_TABLE_BUF_SLAUGHTER];
	char  desc6[MAX_TABLE_BUF];
#ifdef USE_KOR_SLAUGHT_CODE_4DIGIT
	INT16U tableId;
	INT16U tempId;
#endif    
	INT8U gradeNo;

	memset(desc4,0,MAX_TABLE_BUF);
	memset(desc5,0,MAX_TABLE_BUF_SLAUGHTER);
	memset(desc6,0,MAX_TABLE_BUF);
#endif
#ifdef USE_TRACE_1000QTY //sewon 161222
	memset(desc7,0,MAX_TABLE_BUF);
#endif  
	memset((INT8U *)&eth,0,sizeof(ETH_CONNECT_LIST));
	commun_buffer=(char *)serial_send_point;
	part = (INT16S)spart;
	if (part<=0) return 0x82;
NEXT_ID:;
	id   = (INT32U)sid;
	if (part==TABLE2_INGREDIENT) {
		id = (INT16U)(sid>>8);
		nth= (INT16U)(sid&0xff);
	}
	if (id  <=0) return 0x82;
	r      = 0;
	dsize  = 0;
	memset(desc1,0,MAX_TABLE_BUF);
	memset(desc2,0,MAX_TABLE_BUF);
	memset(desc3,0,MAX_TABLE_BUF);
	switch (part) {
	    case TABLE2_INGREDIENT: 
#ifdef USE_INGREDIENT_CODE_4DIGIT
			if (id>MAX_INGREDIENT_INDEX_NUMBER) return 0x99;
#else
			if (id>MAX_INGREDIENT_NO) return 0x99;
#endif
			sz = INGREDIENT_COM_BLOCK_SIZE;
		    rp = (INT32U)nth;
		    rp *= sz;
#ifdef USE_9999_INDIRECT_INGREDIENT
			// Find Ingredient ID by Ingredient table
			ing_table_id = id;
			ing_id = ingredient_get_id_by_table(ing_table_id);
			if(ing_id == 0)// empty
			{
				id=0;
			}
			else
			{
				p = ing_calc_addr_by_key(ing_id-1);
				sid=ing_id;
				Flash_sread(p,(HUGEDATA INT8U *)desc1,2);
				memcpy(&id,desc1,2);
				if (sid!=id) id=0;
				sid <<= 8;
				p += 2;
				p += rp;
				if (id>MAX_INGREDIENT_NO) id=0;
			}
#elif defined(USE_INGREDIENT_CODE_4DIGIT)
			if(!ingredient_search_index(&id, &tableId, &index, ON))
			{
				if (tableId >= IngTableLastId) return 0x99;
		    	break;
			}
			p = ing_calc_addr_by_key(index-1);
#else
		    p  = ing_calc_addr_by_key(id-1);
#endif
		    sid=id;
		    Flash_sread(p,(HUGEDATA INT8U *)desc1,2);
		    memcpy(&id,desc1,2);
		    if (sid!=id) id=0;
		    sid <<= 8;
		    p += 2;
		    p += rp;
#ifdef USE_INGREDIENT_CODE_4DIGIT
		    if (id>MAX_INGREDIENT_INDEX_NUMBER) id=0;
#else
		    if (id>MAX_INGREDIENT_NO) id=0;
#endif
		    break;
#ifdef USE_NUTRITION_FACT
		case TABLE2_NUTRITION_FACT: 
		    ucTemp = get_global_bparam(GLOBAL_SALE_SETUP_FLAG9);
			// nutri_type (0:normal, 1:CHL, 2:CAN, 3:AUS, 4:NZL, 5: AUS for actual weight, 6: NZL for actual weight)
			nutri_type = ucTemp&0x0f;
#ifdef COUNTRY_USA
			nutri_type = NUTRI_TYPE_2COL_USA;
#endif			    		    
			if (nutri_type == NUTRI_TYPE_2COL_6DIG_AUS || nutri_type == NUTRI_TYPE_2COL_6DIG_AUS_ACTUAL_WEIGHT || nutri_type == NUTRI_TYPE_6DIG_CZE)
		    {
		    	if (id>MAX_NUTRIFACT2_NO) return 0x99;
		    	sz = sizeof(NUTRIFACT_STRUCT2);
		    }
#ifdef USE_EU_NUTRITIONFACT
			else if (nutri_type == NUTRI_TYPE_42ITEM_5DIG_EU_ACTUAL_WEIGHT)
			{
				if (id>MAX_NUTRIFACT3_NO) return 0x99;
				sz = sizeof(NUTRIFACT_STRUCT3);
			}
#endif
#ifdef USE_ADDITIONAL_NUTRIENT
			if (nutri_type == NUTRI_TYPE_2COL_USA || nutri_type == NUTRI_TYPE_TOTALFAT_CAN)
			{
				if (id>MAX_NUTRIFACT4_NO) return 0x99;
				sz = sizeof(NUTRIFACT_STRUCT4);
			}
#endif
		    else
		    {
		    	if (id>MAX_NUTRIFACT_NO) return 0x99;
		    	sz = sizeof(NUTRIFACT_STRUCT);
		    }
		    p = (id-1);
		    p*= sz;
		    p+= FLASH_NUTRIFACT_AREA;
#ifdef USE_NORTH_AMERICA_INGREDIENT
			p+= EXT_MEMCARD2_ADDR_X;
#else
		    p+= DFLASH_BASE;
#endif
			if (nutri_type == NUTRI_TYPE_2COL_6DIG_AUS || nutri_type == NUTRI_TYPE_2COL_6DIG_AUS_ACTUAL_WEIGHT || nutri_type == NUTRI_TYPE_6DIG_CZE)
		    {
		    	Flash_sread(p,(HUGEDATA INT8U *)&nutri_fact2,sizeof(NUTRIFACT_STRUCT2));
		    	for (i=0; i<20; i++) varray[i]=nutri_fact2.nutri_data[i];
		    	id=(INT16U)nutri_fact2.code;
		    	memcpy(desc1,nutri_fact2.description ,20);
		    	memcpy(desc2,nutri_fact2.serving_size,20);
		    	memcpy(desc3,nutri_fact2.serving_per ,10);
		    	xtype = (INT8U)nutri_fact2.type;
		    	if (id!=sid) id=0;
		    	if (id>MAX_NUTRIFACT2_NO) id=0;
		    }
#ifdef USE_EU_NUTRITIONFACT
			else if (nutri_type == NUTRI_TYPE_42ITEM_5DIG_EU_ACTUAL_WEIGHT)
			{
				Flash_sread(p,(HUGEDATA INT8U *)&nutri_fact3,sizeof(NUTRIFACT_STRUCT3));
				for (i=0; i<45; i++) varray[i]=nutri_fact3.nutri_data[i];
				id=(INT16U)nutri_fact3.code;
				memcpy(desc1,nutri_fact3.description ,20);
				memcpy(desc2,nutri_fact3.serving_size,20);
				memcpy(desc3,nutri_fact3.serving_per ,10);
				xtype = (INT8U)nutri_fact3.type;
				if (id!=sid) id=0;
				if (id>MAX_NUTRIFACT3_NO) id=0;
			}
#endif
#ifdef USE_ADDITIONAL_NUTRIENT
			if (nutri_type == NUTRI_TYPE_2COL_USA || nutri_type == NUTRI_TYPE_TOTALFAT_CAN)
			{
				Flash_sread(p,( INT8U *)&nutri_fact4,sizeof(NUTRIFACT_STRUCT4));
				for (i=0; i<30; i++) varray[i]=nutri_fact4.nutri_data[i]; 
  #ifdef USE_DIRECT_NUTRITION

  #else
				id=(INT16U)nutri_fact4.code; 
  #endif
				memcpy(desc1,nutri_fact4.description ,sizeof(nutri_fact4.description));
				memcpy(desc2,nutri_fact4.serving_size,sizeof(nutri_fact4.serving_size));
				memcpy(desc3,nutri_fact4.serving_per ,sizeof(nutri_fact4.serving_per));
  #ifdef USE_DIRECT_NUTRITION

  #else
				xtype = (INT8U)nutri_fact4.type;
  #endif
 				if (id!=sid) id=0;
				if (id>MAX_NUTRIFACT4_NO) id=0;
			}
#endif
		    else
		    {
		    	Flash_sread(p,(HUGEDATA INT8U *)&nutri_fact,sizeof(NUTRIFACT_STRUCT));
		    	for (i=0; i<20; i++) varray[i]=(INT32U)nutri_fact.nutri_data[i];
		    	id=(INT16U)nutri_fact.code;
		    	memcpy(desc1,nutri_fact.description ,20);
		    	memcpy(desc2,nutri_fact.serving_size,20);
		    	memcpy(desc3,nutri_fact.serving_per ,10);
		    	xtype = (INT8U)nutri_fact.type;
		    	if (id!=sid) id=0;
		    	if (id>MAX_NUTRIFACT_NO) id=0;
		    }
#ifdef USE_EU_NUTRITIONFACT
			if (nutri_type == NUTRI_TYPE_42ITEM_5DIG_EU_ACTUAL_WEIGHT)
				sarray=MAX_NUTRIFACT_ITEM_EU-1;
			else
				sarray=MAX_NUTRIFACT_ITEM-1;
#else
		    sarray=MAX_NUTRIFACT_ITEM-1;
#endif
		    break;
#endif
#ifdef USE_TRACEABILITY //gm 090407, memory 충돌 방지
		case TABLE2_TRACEABILITY: if (sid>MAX_TRACEABILITY_NO-1) return 0x99;
		    sz=TRACEABILITY_STRUCT_SIZE;
		    p = (id-1)*sz;
		    p+=FLASH_TRACEABILITY_AREA;
		    p+= DFLASH_BASE;
		    Flash_sread(p,(HUGEDATA INT8U *)&trace,sizeof(TRACEABILITY_STRUCT));
		    id=trace.code;
		    memcpy(desc1,trace.contents,TRACEABILITY_STRUCT_SIZE-10);
		    for (i=0; i<4; i++) varray[i]=(INT16U)trace.idata[i];
		    sarray=4;
		    if (id!=sid) id=0;
		    if (id>MAX_TRACEABILITY_NO-1) id=0;
		    break;
		case TABLE2_COUNTRY: if (sid>MAX_COUNTRY_NO-1) return 0x99;
		    sz=COUNTRY_STRUCT_SIZE;
		    p = (id-1)*sz;
		    p+=FLASH_COUNTRY_AREA;
		    p+= DFLASH_BASE;
		    Flash_sread(p,(HUGEDATA INT8U *)temp,sz);
		    memcpy((char *)&id,(char *)temp,2);
		    memcpy(desc1,&temp[2],COUNTRY_STRUCT_SIZE-2);
		    if (id!=sid) id=0;
		    if (id>MAX_COUNTRY_NO-1) id=0;
		    break;
		case TABLE2_SLAUGHTERHOUSE: if (sid>MAX_SLAUGHTERHOUSE_NO-1) return 0x99;
		    sz=SLAUGHTERHOUSE_STRUCT_SIZE;
		    p = (id-1)*sz;
		    p+=FLASH_SLAUGHTERHOUSE_AREA;
		    p+= DFLASH_BASE;
		    Flash_sread(p,(HUGEDATA INT8U *)&work,sizeof(WORKPLACE_STRUCT));
		    id=work.code;
		    memcpy(desc1,work.text,SLAUGHTERHOUSE_STRUCT_SIZE-4);
		    rvalue=work.country_number;
		    if (id!=sid) id=0;
		    if (id>MAX_SLAUGHTERHOUSE_NO-1) id=0;
		    break;
		case TABLE2_CUTTINGHALL: if (sid>MAX_CUTTINGHALL_NO-1) return 0x99;
		    sz=CUTTINGHALL_STRUCT_SIZE;
		    p = (id-1)*sz;
		    p+=FLASH_CUTTINGHALL_AREA;
		    p+= DFLASH_BASE;
		    Flash_sread(p,(HUGEDATA INT8U *)&work,sizeof(WORKPLACE_STRUCT));
		    id=work.code;
		    memcpy(desc1,work.text,CUTTINGHALL_STRUCT_SIZE-4);
		    rvalue=work.country_number;
		    if (id!=sid) id=0;
		    if (id>MAX_CUTTINGHALL_NO-1) id=0;
		    break;
#endif
	    default :
		    return 0x99;
#ifdef USE_TRACE_STANDALONE
		case TABLE2_INDIVIDUAL: if (sid>MAX_INDIVIDUAL_NO-1) return 0x99;
		    sz=TRACE_INDIVIDUAL_IDENTITY_SIZE;
		    p = (INT32U)(id-1)*sz;
		    p+= FLASH_TRACE_STANDALONE_AREA;
  #ifdef USE_TRACE_1000QTY //sewon 161206
		    p+= EXT_MEMCARD_ADDR;
  #else
		    p+= DFLASH_BASE;
  #endif
		    Flash_sread(p,(HUGEDATA INT8U *)&indivStr,TRACE_INDIVIDUAL_IDENTITY_SIZE);
		    indivWeight = get_nvram_lparam(NVRAM_TRACE_STANDALONE_WEIGHT+((INT32U)indivStr.index-1)*4);
		    memcpy(desc1, indivStr.individualNO, sizeof(indivStr.individualNO));
		    memcpy(desc3, indivStr.breed,  sizeof(indivStr.breed));
		    memcpy(desc4, indivStr.farm,  sizeof(indivStr.farm));
		    memcpy(desc5, indivStr.slaughtHouse,  sizeof(indivStr.slaughtHouse));
#ifdef USE_TRACE_MEATGRADE
		    gradeNo = indivStr.gradeNo;
#else
		    memcpy(desc6, indivStr.grade, sizeof(indivStr.grade));
#endif
#ifdef USE_TRACE_1000QTY //sewon 161222
			memcpy(desc7,indivStr.nongjangNo, sizeof(indivStr.nongjangNo));
#endif
#ifdef USE_KOR_TRACE_999
		    if (indivStr.lotFlag)
		    {
				memcpy(desc5, indivStr.slaughtHouse, 101);
		    }
#endif            
		    id=indivStr.index;
		    if (id!=sid) id=0;
		    if (id>MAX_INDIVIDUAL_NO-1) id=0;
		    break;
		case TABLE2_SPECIALUSE: if (sid>MAX_SPECIALUSE_NO-1) return 0x99;
		    sz=TRACE_SPECIALUSE_STRUCT_SIZE;
		    p = (INT32U)(id-1)*sz;
		    p+=FLASH_TRACE_SPECIALUSE_AREA;
		    p+= DFLASH_BASE;
		    Flash_sread(p,(HUGEDATA INT8U *)temp,sz);
		    memcpy((char *)&id,(char *)temp,2);
		    memcpy(desc1,&temp[2],TRACE_SPECIALUSE_STRUCT_SIZE-2);
		    if (id!=sid) id=0;
		    if (id>MAX_SPECIALUSE_NO-1) id=0;
		    break;
		case TABLE2_SLAUGHTERHOUSE_KR: if (sid>MAX_SLAUGHTERHOUSE_KR_NO-1) return 0x99;
#ifdef USE_KOR_SLAUGHT_CODE_4DIGIT
		    tempId = sid;
		    if (!trace_slaughthouse_search_index(&tempId, &tableId, &id, ON))
		    {
			if (tableId >= SlaughtTableLastId) return 0x99;
		    	break;
		    }
		    sid = tempId;
#endif        
		    sz=TRACE_SLAUGHTERHOUSE_KR_STRUCT_SIZE;
		    p = (INT32U)(id-1)*sz;
		    p+=FLASH_TRACE_SLAUGHTERHOUSE_AREA;
		    p+= DFLASH_BASE;
		    Flash_sread(p,(HUGEDATA INT8U *)temp,sz);
		    memcpy((char *)&id,(char *)temp,2);
		    memcpy(desc1,&temp[2],TRACE_SLAUGHTHOUSE_NAME_SIZE - 1);
		    if (id!=sid) id=0;
		    if (id>MAX_SLAUGHTERHOUSE_KR_NO-1) id=0;
		    break;
#ifdef USE_TRACE_MEATPART
		case TABLE2_MEATPART: if (sid>MAX_MEATPART_NO-1) return 0x99;
		    sz=TRACE_MEATPART_STRUCT_SIZE;
		    p = (INT32U)(id-1)*sz;
		    p+=FLASH_TRACE_MEATPART_AREA;
		    p+= DFLASH_BASE;
		    Flash_sread(p,(HUGEDATA INT8U *)temp,sz);
		    memcpy((char *)&id,(char *)temp,2);
		    memcpy(desc1,&temp[2],TRACE_MEATPART_STRUCT_SIZE-2);
		    if (id!=sid) id=0;
		    if (id>MAX_MEATPART_NO-1) id=0;
		    break;
#endif            
#endif

	}
	if (id==0) {
		if (next) {
			if (part==TABLE2_INGREDIENT) {
				id = (INT16U)(sid>>8);
				id++;
				sid = (INT32U)id;
				sid<<=8;
			} else {
				sid++; 
			}
			goto NEXT_ID;
		}
		else return 0x82;
	}

	if (part == TABLE2_INGREDIENT)
	{
		Flash_sread(p-rp, (HUGEDATA INT8U *)ing_str, INGREDIENT_STRUCT_SIZE - 2);
		ing_len = strlen((HUGEDATA char *)ing_str);
		if (ing_len > INGREDIENT_STRUCT_SIZE - 2) ing_len = INGREDIENT_STRUCT_SIZE - 2;
	        block_count = ing_len / INGREDIENT_COM_BLOCK_SIZE;
		if (ing_len % INGREDIENT_COM_BLOCK_SIZE) block_count++;
		block_size = INGREDIENT_COM_BLOCK_SIZE;
		if ((block_count-1) == nth) {
			if (request)	// 요청에 의한 전송일 때,(PC에서 마지막 Block을 구분하기 위함)
			{
				nth = 0xff;
			}
			block_size = ing_len % INGREDIENT_COM_BLOCK_SIZE;
			if (block_size == 0) block_size = INGREDIENT_COM_BLOCK_SIZE;
		} else if ((block_count-1) < nth) {
			return 0x95;
		}
	}

	r_start=16;
#ifdef USE_KOR_SLAUGHT_CODE_4DIGIT
	if (part == TABLE2_SLAUGHTERHOUSE_KR)
	{
		if (id > 0xfff) r_start++;	// id가 3자리 초과할 경우 길이 조정
	}
#endif    
#ifdef USE_INGREDIENT_CODE_4DIGIT
	if (part == TABLE2_INGREDIENT)
	{
		if (id > 0xfff) r_start++;	// id가 3자리 초과할 경우 길이 조정
	}
#endif
	r=r_start;
	ethernet_gen_head(&commun_buffer[r]);
	r+=strlen(&commun_buffer[r]);
	if (id) {
	    if (part==1) {
		sprintf(&commun_buffer[r],"X=%02X.",nth);
		r+=strlen(&commun_buffer[r]);
		dsize = 0;
		for (i=0; i<block_size; i++) {
			cc=Flash_bread(p++);
			if (cc==0) break;
			commun_buffer[r+6+dsize]=cc;
			dsize++;
		}
		sprintf(&commun_buffer[r],"D=%03lX",dsize);
		r+=strlen(&commun_buffer[r]);
		commun_buffer[r++]='.';
		r+=dsize;
	    } 

#ifdef USE_TRACE_STANDALONE	    
		else if (part ==TABLE2_INDIVIDUAL)
	    {
		// Individual number		
		sprintf(&commun_buffer[r],"D=%02X.", strlen(desc1));  
		r+=strlen(&commun_buffer[r]);
		sprintf(&commun_buffer[r],"%s", desc1);
		r+=strlen(&commun_buffer[r]);

	#ifdef USE_KOR_TRACE_999
		if (indivStr.lotFlag)
		{
		}
		else
		{
			// Slaught date							      
			sprintf(&commun_buffer[r],"Z=0:%02X.", indivStr.slaughtDate[0] ); 
			r+=strlen(&commun_buffer[r]);
			sprintf(&commun_buffer[r],"Z=1:%02X.", indivStr.slaughtDate[1] );
			r+=strlen(&commun_buffer[r]);
			sprintf(&commun_buffer[r],"Z=2:%02X.", indivStr.slaughtDate[2] );
			r+=strlen(&commun_buffer[r]);
		}
	#else
		// Slaught date							      
		sprintf(&commun_buffer[r],"Z=0:%02X.", indivStr.slaughtDate[0] ); 
		r+=strlen(&commun_buffer[r]);
		sprintf(&commun_buffer[r],"Z=1:%02X.", indivStr.slaughtDate[1] );
		r+=strlen(&commun_buffer[r]);
		sprintf(&commun_buffer[r],"Z=2:%02X.", indivStr.slaughtDate[2] );
		r+=strlen(&commun_buffer[r]);
	#endif
		
		// Weight : 7자리로 고정 
		sprintf(&commun_buffer[r],"W=%02X.", 7 ); 
		r+=strlen(&commun_buffer[r]);
		sprintf(&commun_buffer[r],"%07ld", indivWeight );
		r+=strlen(&commun_buffer[r]);

		// Breed
		sprintf(&commun_buffer[r],"B=%02X.", strlen(desc3) );
		r+=strlen(&commun_buffer[r]);
		sprintf(&commun_buffer[r],"%s", desc3 );
		r+=strlen(&commun_buffer[r]);

		// Farm & Part(묶음 부위)
		sprintf(&commun_buffer[r],"F=%02X.", strlen(desc4) );
		r+=strlen(&commun_buffer[r]);
		sprintf(&commun_buffer[r],"%s", desc4 );
		r+=strlen(&commun_buffer[r]);

		// SlaughtHouse & 묶음 도축장
		sprintf(&commun_buffer[r],"S=%02X.", strlen(desc5) );
		r+=strlen(&commun_buffer[r]);
		sprintf(&commun_buffer[r],"%s", desc5 );
		r+=strlen(&commun_buffer[r]);
		
		// LotFlag
		sprintf(&commun_buffer[r],"L=%02X.", indivStr.lotFlag ); 
		r+=strlen(&commun_buffer[r]);

#ifdef USE_TRACE_MEATGRADE
		// Grade No
		sprintf(&commun_buffer[r],"G=%02X.", indivStr.gradeNo); 
		r+=strlen(&commun_buffer[r]);
#else
		// Grade
		sprintf(&commun_buffer[r],"M=%02X.", strlen(desc6) );
		r+=strlen(&commun_buffer[r]);
		sprintf(&commun_buffer[r],"%s", desc6 );
		r+=strlen(&commun_buffer[r]);
#endif

#ifdef USE_TRACE_1000QTY //sewon 161222
		//meatkind No 육종번호  
		sprintf(&commun_buffer[r],"K=%02X.", indivStr.meatKind); 
		r+=strlen(&commun_buffer[r]);
		
		//slaughter No 도축장 인덱스 번호  
		sprintf(&commun_buffer[r],"U=%02X.", indivStr.slaughtNo); 
		r+=strlen(&commun_buffer[r]);
		
		//harmfulness 위해 개체 유무
		sprintf(&commun_buffer[r],"H=%02X.", indivStr.harmfulness ); 
		r+=strlen(&commun_buffer[r]);

		// SlaughtHouse & 묶음 도축장
		sprintf(&commun_buffer[r],"N=%02X.", strlen(desc7) );
		r+=strlen(&commun_buffer[r]);
		sprintf(&commun_buffer[r],"%s", desc7 );
		r+=strlen(&commun_buffer[r]);
#endif
		
#ifdef USE_EMART_NETWORK
		// delflag
		sprintf(&commun_buffer[r],"A=00."); 
		r+=strlen(&commun_buffer[r]);
#endif
		
#ifdef USE_CHECKING_HARMFUL_MEAT
		// harmfulness
		if (indivStr.index < MaxIndividualNo) indivStr.harmfulness = 0;	// 영역에 따라 고정
		else indivStr.harmfulness = 1;        
		sprintf(&commun_buffer[r],"H=%02X.", indivStr.harmfulness ); 
		r+=strlen(&commun_buffer[r]);
#endif        
	    }
#endif
	    else 
	    {
		i=strlen(desc1);
		sprintf(&commun_buffer[r],"D=%02X.",i);
		r+=strlen(&commun_buffer[r]);
		sprintf(&commun_buffer[r],"%s",desc1);
		r+=strlen(&commun_buffer[r]);
		if ((part==TABLE2_SLAUGHTERHOUSE) || (part==TABLE2_CUTTINGHALL)) {
			sprintf(&commun_buffer[r],"C=%03lX.",rvalue);
			r+=strlen(&commun_buffer[r]);
		}
#ifdef USE_NUTRITION_FACT
		if (part==TABLE2_NUTRITION_FACT) {
  #ifdef USE_DIRECT_NUTRITION

  #else
  			sprintf(&commun_buffer[r],"T=%02X.",xtype);
  #endif
			r+=strlen(&commun_buffer[r]);
			i=strlen(desc2);
			sprintf(&commun_buffer[r],"S=%02X.",i);
			r+=strlen(&commun_buffer[r]);
			sprintf(&commun_buffer[r],"%s",desc2);
			r+=strlen(&commun_buffer[r]);
			i=strlen(desc3);
			sprintf(&commun_buffer[r],"P=%02X.",i);
			r+=strlen(&commun_buffer[r]);
			sprintf(&commun_buffer[r],"%s",desc3);
			r+=strlen(&commun_buffer[r]);

			for (i=0; i<=sarray; i++) {
				//sprintf(&commun_buffer[r],"Z=%02X:%04X.",i,varray[i]);
				sprintf(&commun_buffer[r],"Z=%02X:%08lX.",i,varray[i]);
				r+=strlen(&commun_buffer[r]);
			}
		}
#endif
#ifdef USE_TRACEABILITY
		if (part==TABLE2_TRACEABILITY) {	// traceability
			for (i=0; i<=sarray; i++) {
				sprintf(&commun_buffer[r],"Z=%02X:%04lX.",i,varray[i]);
				r+=strlen(&commun_buffer[r]);
			}
		}
#endif
	    }

	} else {
	    sprintf(&commun_buffer[r],"B=1.");
	    r+=strlen(&commun_buffer[r]);
	}
	bcc=gen_bcc((INT8U *)&commun_buffer[r_start],r-r_start);
	sprintf(commun_buffer,"W%02XF%02X,%03XL",cmd,part,id);	//id가 3자리를 초과할 경우 길이가 1자리 늘어남
	sprintf(&commun_buffer[r_start-6],"L%04X",r-r_start);
	commun_buffer[r_start-1]=':';
	commun_buffer[r++]=bcc;
	commun_outleng(CBuf,(char *)commun_buffer,r);
	return 1;
}

INT8U commun_send_table3(HUGEDATA COMM_BUF *CBuf,INT8U cmd,INT32U spart,INT32U sid,INT8U next)
{
	ETH_CONNECT_LIST eth;
	STORE_NAME_STRUCT	store;
	CUSTOMER_STRUCT		customer;
	SCROLL_MESSAGE_STRUCT	scroll;
	CLERK_STRUCT		clerk;
	CURRENCY_STRUCT		currency;
	SCANNER_CONV_TABLE_STR  scanner;	
	
	CAPTION_STRUCT 	cap;
	INT16S    r;
	INT16S    part,r_start;
	INT16U id;
	INT8U  bcc;
	INT32U read_addr;
	char *commun_buffer;

	memset((INT8U *)&eth,0,sizeof(ETH_CONNECT_LIST));
	commun_buffer=(char *)serial_send_point;

	part = (INT16S)spart;
	if (part<=0) return 0x82;
NEXT_ID:;
	id   = (INT32U)sid;
	if (id  <=0) return 0x82;
	r      = 0;
	switch (part) {
		case 1:if (sid>MAX_STORE_NO) return 0x99;
		       read_addr   = id-1;
		       read_addr *= sizeof(store);
		       read_addr += FLASH_SHOPNAME_AREA;
		       read_addr += DFLASH_BASE;
		       Flash_sread(read_addr,(HUGEDATA INT8U *)&store,sizeof(store));
		       if (store.code!=sid) id=0;
		       if (id>MAX_STORE_NO) id=0;
		       break;
		case 2:if (sid>MAX_CUSTOMER_NO) return 0x99;
		       read_addr   = id;
		       read_addr *= CUSTOMER_STRUCT_SIZE;
		       read_addr += FLASH_CUSTOMER_AREA;
		       read_addr += DFLASH_BASE;
		       Flash_sread(read_addr,(HUGEDATA INT8U *)&customer,sizeof(customer));
		       if (customer.code!=sid) id=0;
		       if (id>MAX_CUSTOMER_NO) id=0;
		       break;
		case 3:if (sid>MAX_SCROLL_NO) return 0x99;
		       read_addr   = id-1;
		       read_addr *= sizeof(scroll);
		       read_addr += FLASH_SCROLL_AREA;
		       read_addr += DFLASH_BASE;
		       Flash_sread(read_addr,(HUGEDATA INT8U *)&scroll,sizeof(scroll));
		       //if (!((scroll.stype==1) || (scroll.stype==2))) id=0;
		       if ((scroll.stype < 1) || (scroll.stype > MAX_SCROLL_EFFECT)) id=0;
		       if (id>MAX_SCROLL_NO) id=0;
		       break;
		case 4:if (sid>MAX_CLERK_NO-1) return 0x99;
		       read_addr   = id;
		       read_addr *= sizeof(clerk);
		       read_addr += FLASH_CLERK_AREA;
		       read_addr += DFLASH_BASE;
		       Flash_sread(read_addr,(HUGEDATA INT8U *)&clerk,sizeof(clerk));
		       if (clerk.code!=sid) id=0;
		       if (id>MAX_CLERK_NO-1) id=0;
		       break;
		case 5:if (sid>MAX_CURRENCY_NO) return 0x99;
		       read_addr   = id-1;
		       read_addr *= sizeof(currency);
		       read_addr += FLASH_CURRENCY_AREA;
		       read_addr += DFLASH_BASE;
		       Flash_sread(read_addr,(HUGEDATA INT8U *)&currency,sizeof(currency));
		       if (id>MAX_CURRENCY_NO) id=0;
		       break;
		//SCANNER
		case 6:if (sid>SCAN_BACODE_DATA_MAX_NUM) return 0x99;
		       read_addr   = id-1;
		       read_addr *= SCANNER_CONV_TABLE_STR_SIZE;
		       read_addr += FLASH_SCANNER_TABLE_AREA;
#ifdef USE_10000_SCANNER_CONVTABLE
			   read_addr += EXT_MEMCARD2_ADDR_X;
#else
		       read_addr += DFLASH_BASE;
#endif
		       Flash_sread(read_addr,(HUGEDATA INT8U *)&scanner,sizeof(scanner));
		       if (scanner.code!=sid) id=0;
		       if (id>SCAN_BACODE_DATA_MAX_NUM) id=0;
		       break;
		default:
			return 0x99;
	}
	if (id==0) {
		if (next) { sid++; goto NEXT_ID; }
		else return 0x82;
	}
#ifdef USE_10000_SCANNER_CONVTABLE
	if(part == 6)
		r_start=17;
	else
#else
	r_start=16;
#endif
	r=r_start;
	ethernet_gen_head(&commun_buffer[r]);
	r+=strlen(&commun_buffer[r]);
	switch (part) {
		case 1: commun_send_stringtype(&r,commun_buffer,"P=%02X.",(char *)store.name);
			commun_send_stringtype(&r,commun_buffer,"T=%02X.",(char *)store.tel_no);
			commun_send_stringtype(&r,commun_buffer,"S=%02X.",(char *)store.text);
			break;
		case 2: sprintf(&commun_buffer[r],"C=%06lX.",customer.creditlimit); //Modified by JJANG 20071212 버그수정
			r+=strlen(&commun_buffer[r]);
			commun_send_stringtype(&r,commun_buffer,"P=%02X.",(char *)customer.name);
			commun_send_stringtype(&r,commun_buffer,"Q=%02X.",(char *)customer.address[0]);
			commun_send_stringtype(&r,commun_buffer,"R=%02X.",(char *)customer.address[1]);
			commun_send_stringtype(&r,commun_buffer,"S=%02X.",(char *)customer.address[2]);
			commun_send_stringtype(&r,commun_buffer,"T=%02X.",(char *)customer.tel);
			break;
		case 3:	commun_send_stringtype(&r,commun_buffer,"S=%02X.",(char *)scroll.text);
			sprintf(&commun_buffer[r],"E=%X.",scroll.stype);
			r+=strlen(&commun_buffer[r]);
			sprintf(&commun_buffer[r],"A=%X.",scroll.smode);
			r+=strlen(&commun_buffer[r]);
			sprintf(&commun_buffer[r],"B=%X.",scroll.use);
			r+=strlen(&commun_buffer[r]);
			sprintf(&commun_buffer[r],"w=%02X.",scroll.week);
			r+=strlen(&commun_buffer[r]);
			sprintf(&commun_buffer[r],"h=%02X%02X%02X.",scroll.start_year,scroll.start_month,scroll.start_day);
			r+=strlen(&commun_buffer[r]);
			sprintf(&commun_buffer[r],"i=%02X%02X%02X.",scroll.end_year,scroll.end_month,scroll.end_day);
			r+=strlen(&commun_buffer[r]);
			sprintf(&commun_buffer[r],"j=%02X%02X%02X.",scroll.start_hour,scroll.start_min,scroll.start_sec);
			r+=strlen(&commun_buffer[r]);
			sprintf(&commun_buffer[r],"k=%02X%02X%02X.",scroll.end_hour,scroll.end_min,scroll.end_sec);
			r+=strlen(&commun_buffer[r]);
			// CJK080215
			sprintf(&commun_buffer[r],"o=%02X.",scroll.blink_count);
			r+=strlen(&commun_buffer[r]);
			sprintf(&commun_buffer[r],"p=%02X.",scroll.font_size);
			r+=strlen(&commun_buffer[r]);
			sprintf(&commun_buffer[r],"q=%02X.",scroll.image_id);
			r+=strlen(&commun_buffer[r]);
			sprintf(&commun_buffer[r],"r=%02X.",scroll.on_time);
			r+=strlen(&commun_buffer[r]);
			///////
			break;
		case 4: sprintf(&commun_buffer[r],"L=%01X.",clerk.permissionLevel);
			r+=strlen(&commun_buffer[r]);
			commun_send_stringtype(&r,commun_buffer,"P=%02X.",(char *)clerk.name);
			commun_send_stringtype(&r,commun_buffer,"Q=%02X.",(char *)clerk.nickname);
			commun_send_stringtype(&r,commun_buffer,"R=%02X.",(char *)clerk.password);
			break;
		case 5: 
			commun_send_stringtype(&r,commun_buffer,"P=%02X.",(char *)currency.primary);
			commun_send_stringtype(&r,commun_buffer,"Q=%02X.",(char *)currency.last);
			sprintf(&commun_buffer[r],"h=%06lX.",currency.rate);
			r+=strlen(&commun_buffer[r]);
			sprintf(&commun_buffer[r],"i=%06X.",currency.decimalpoint);
			r+=strlen(&commun_buffer[r]);
			break;
		//SCANNER
		case 6: 
			commun_send_stringtype(&r,commun_buffer,"P=%02X.",(char *)scanner.scanCode);
			sprintf(&commun_buffer[r],"h=%06X.",scanner.dept_no);
			r+=strlen(&commun_buffer[r]);
			sprintf(&commun_buffer[r],"j=%06lX.",scanner.plu_no);
			r+=strlen(&commun_buffer[r]);
			break;

	}
	bcc=gen_bcc((INT8U *)&commun_buffer[r_start],r-r_start);
#ifdef USE_10000_SCANNER_CONVTABLE
	if(part == 6) {
		sprintf(commun_buffer,"W%02XF%02X,%04XL",cmd,part,id);
		sprintf(&commun_buffer[11],"L%04X",r-r_start);
	}
	else {
#endif
	sprintf(commun_buffer,"W%02XF%02X,%03XL",cmd,part,id);
	sprintf(&commun_buffer[10],"L%04X",r-r_start);
#ifdef USE_10000_SCANNER_CONVTABLE	
	}
#endif
	commun_buffer[r_start-1]=':';
	commun_buffer[r++]=bcc;	
	commun_outleng(CBuf,(char *)commun_buffer,r);
	return 1;
}

INT8U commun_send_report(HUGEDATA COMM_BUF *CBuf,INT32U spart,INT32U sid)
// ==================================================================
// next=1 이면 현재 요청된 값을 못찾을때 다음 ID로 값을 Return 한다.
// ==================================================================
{
	INT16S    r;
	INT16S    part;
	ETH_CONNECT_LIST eth;
	char   ok;
	INT8U  r_start,bcc,period;
	INT16U id,index,k,wparam;
	INT8U  checkNewData;
	INT32U addr;
	INT32U addrDate;
	INT32U headIndex;
	char *commun_buffer;
	SALES_SUMMARY_TYPE *tp;
	SALES_SUMMARY_TYPE sst;
	PLU_SALES_SUMMARY_TYPE *psr;
	PLU_SALES_SUMMARY_TYPE pst;
	INT16U next;
	RTC_STRUCT rtc;
	INT8U scaleNum;    
#ifdef USE_REPORT_PAY
	INT16U j;
	//SALES_SUMMARY_TYPE *sr;
#endif
#if (defined(USE_REPORT_TAX) && defined(USE_TAX_RATE))
	TAX_STRUCT tax_str;
	INT32U tax_value;
	INT32U flash_addr;
#endif
	
	tp = &sst;
	psr = &pst;

	scaleNum = status_scale.scaleid;
	if (scaleNum > MAX_SCALE - 1) scaleNum = MAX_SCALE - 1;	// scale no 32~255는 scale 31로 정산함
    
	memset((INT8U *)&eth,0,sizeof(ETH_CONNECT_LIST));
	commun_buffer=(char *)serial_send_point;
	period = (INT16U)spart>>8;
	part   = spart&0xff;

	id   = (INT16U)sid;
	if (part<=0) return 0x82;
	if (id  <0 ) return 0x83;
	if (id==0xffff) return 0x83;
	if (period>1) return 0x84;
	if (period<0) return 0x85;

	r      =21;
	r_start=r;
	ethernet_gen_head(&commun_buffer[r]);
	r+=strlen(&commun_buffer[r]);
	switch (part) {
	   case 1: if (id==0) id=scaleNum;
		   if (ethernet_list.status==3 || ethernet_list.status==4) id=scaleNum;
	     ID1:  //tp = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_SCALE_SUMMARY, id);
	       get_nvram_summary_addr(NVRAM_SCALE_SUMMARY, id, tp);
		   if (tp==NULL) {	// 의미 없음.검토 필요.
			if (id<MAX_SCALE - 1) {
				id++;
				goto ID1;
			}
			sprintf(&commun_buffer[r],"E=1.");
			r+=strlen(&commun_buffer[r]);
			break;
		   }
		   sprintf(&commun_buffer[r],"b=%02X.",id);
		   r+=strlen(&commun_buffer[r]);
		   sprintf(&commun_buffer[r],"N01=");
		   r+=strlen(&commun_buffer[r]);
		   sprintf(&commun_buffer[r],"%04X,%08lX.",tp->sstTransactions[period],tp->sstVolume[period]);
		   r+=strlen(&commun_buffer[r]);
		   // add for Greek	// CJK080509
		   sprintf(&commun_buffer[r],"W01=%08lX.",tp->sstWeight[period]);
		   r+=strlen(&commun_buffer[r]);
		   sprintf(&commun_buffer[r],"Q01=%08lX.",tp->sstQty[period]);
		   r+=strlen(&commun_buffer[r]);
		   //////////////

		   //tp = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_VOID_SCALE, id);
		   get_nvram_summary_addr(NVRAM_VOID_SCALE, id, tp);
		   sprintf(&commun_buffer[r],"N02=");
		   r+=strlen(&commun_buffer[r]);
		   sprintf(&commun_buffer[r],"%04X,%08lX.",tp->sstTransactions[period],tp->sstVolume[period]);
		   r+=strlen(&commun_buffer[r]);

		   //tp = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_REPACK_SCALE, id);
		   get_nvram_summary_addr(NVRAM_REPACK_SCALE, id, tp);
		   sprintf(&commun_buffer[r],"N03=");
		   r+=strlen(&commun_buffer[r]);
		   sprintf(&commun_buffer[r],"%04X,%08lX.",tp->sstTransactions[period],tp->sstVolume[period]);
		   r+=strlen(&commun_buffer[r]);

		   //tp = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_OVERRIDE_SCALE, id);
		   get_nvram_summary_addr(NVRAM_OVERRIDE_SCALE, id, tp);
		   sprintf(&commun_buffer[r],"N04=");
		   r+=strlen(&commun_buffer[r]);
		   sprintf(&commun_buffer[r],"%04X,%08lX.",tp->sstTransactions[period],tp->sstVolume[period]);
		   r+=strlen(&commun_buffer[r]);

		   //tp = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_DISCOUNT_SCALE, id);
		   get_nvram_summary_addr(NVRAM_DISCOUNT_SCALE, id, tp);
		   sprintf(&commun_buffer[r],"N05=");
		   r+=strlen(&commun_buffer[r]);
		   sprintf(&commun_buffer[r],"%04X,%08lX.",tp->sstTransactions[period],tp->sstVolume[period]);
		   r+=strlen(&commun_buffer[r]);

		   //tp = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_PREPACK_SCALE, id);
		   get_nvram_summary_addr(NVRAM_PREPACK_SCALE, id, tp);
		   sprintf(&commun_buffer[r],"N06=");
		   r+=strlen(&commun_buffer[r]);
		   sprintf(&commun_buffer[r],"%04X,%08lX.",tp->sstTransactions[period],tp->sstVolume[period]);
		   r+=strlen(&commun_buffer[r]);

		   //tp = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_RETURN_SCALE, id);
		   get_nvram_summary_addr(NVRAM_RETURN_SCALE, id, tp);
		   sprintf(&commun_buffer[r],"N07=");
		   r+=strlen(&commun_buffer[r]);
		   sprintf(&commun_buffer[r],"%04X,%08lX.",tp->sstTransactions[period],tp->sstVolume[period]);
		   r+=strlen(&commun_buffer[r]);
#ifdef USE_REPORT_PAY
		   //tp = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_CHANGE_SCALE, id);
		   get_nvram_summary_addr(NVRAM_CHANGE_SCALE, id, tp);
		   sprintf(&commun_buffer[r],"N09=");
		   r+=strlen(&commun_buffer[r]);
		   sprintf(&commun_buffer[r],"%04X,%08lX.",tp->sstTransactions[period],tp->sstVolume[period]);
		   r+=strlen(&commun_buffer[r]);

		   for(j = 0; j < MAX_PAID_TYPE; j++) {
			 //sr = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_PAY_SCALE, id*(MAX_PAID_TYPE)+j);
			 get_nvram_summary_addr(NVRAM_PAY_SCALE, id*(MAX_PAID_TYPE)+j, tp);
			 sprintf(&commun_buffer[r], "S%02X=",j+1);
			 r+=strlen(&commun_buffer[r]);
			 sprintf(&commun_buffer[r], "%04X,%08lX.",tp->sstTransactions[period],tp->sstVolume[period]);
			 r+=strlen(&commun_buffer[r]);
		   }
#endif
		   //tp = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_CUSTOM_SCALE, id);
		   get_nvram_summary_addr(NVRAM_CUSTOM_SCALE, id, tp);
		   sprintf(&commun_buffer[r],"N0A=");
		   r+=strlen(&commun_buffer[r]);
		   sprintf(&commun_buffer[r],"%04X,%08lX.",tp->sstTransactions[period],tp->sstVolume[period]);
		   r+=strlen(&commun_buffer[r]);

		   wparam = get_nvram_wparam(NVRAM_CASH_SCALE+id*4+period*2);
		   sprintf(&commun_buffer[r],"K=");
		   r+=strlen(&commun_buffer[r]);
		   sprintf(&commun_buffer[r],"%04X.",wparam);
		   r+=strlen(&commun_buffer[r]);
		   Report_GetResetTime(NVRAM_RESET_TIME_SCALE,&rtc,period);
		   break;
	   case 2: 
	   case 3: 
	   //Added by JJANG 20080519, Back up data (PLU REPORT)   
	   case 0x12:	//오늘 백업 한 데이타 
	   case 0x22: 	//하루 전 백업 한 데이타 
		   if(GlbOper.PluReportType)
		   {
			   addrDate = NVRAM_RESET_TIME_TOTAL;
			   sprintf(&commun_buffer[r],"E=1.");
			   r+=strlen(&commun_buffer[r]);

		   }
		   else
		   {
			   headIndex = NVRAM_PLUSUM_HEADINDEX;
			   addr = NVRAM_PLU_SUMMARY;
			   addrDate = NVRAM_RESET_TIME_TOTAL;
		  	   checkNewData = get_nvram_bparam(NVEXT2_REPORT_PLU_BACKUP_INDEX);
			   if (part == 0x12)
			   {
				   if (checkNewData)
				   {
					   headIndex = NVEXT2_REPORT1_PLUSUM_HEADINDEX;
					   addr = NVEXT2_REPORT1_PLU_SUMMARY;
					   addrDate = NVEXT2_REPORT1_PLU_UPDATE_DATE;
				   }
				   else 
				   {
					   headIndex = NVEXT2_REPORT2_PLUSUM_HEADINDEX;
					   addr = NVEXT2_REPORT2_PLU_SUMMARY;
					   addrDate = NVEXT2_REPORT2_PLU_UPDATE_DATE;
				   }
			   }
			   if (part == 0x22)
			   {
				   if (!checkNewData)
				   {
					   headIndex = NVEXT2_REPORT1_PLUSUM_HEADINDEX;
					   addr = NVEXT2_REPORT1_PLU_SUMMARY;
					   addrDate = NVEXT2_REPORT1_PLU_UPDATE_DATE;
				   }
				   else 
				   {
					   headIndex = NVEXT2_REPORT2_PLUSUM_HEADINDEX;
					   addr = NVEXT2_REPORT2_PLU_SUMMARY;
					   addrDate = NVEXT2_REPORT2_PLU_UPDATE_DATE;
				   }
			   }
		   	   index = get_nvram_wparam(headIndex);
			   //psr = (PLU_SALES_SUMMARY_TYPE *)get_nvram_plu_summary(addr, index);
			   get_nvram_plu_summary(addr, index, psr);
			   k   = 0;
			   ok  = 0;
			   while (1) {
				if (k==id) { ok=1; break; }
				if(psr->pstNext == 0) break;
				//psr = (PLU_SALES_SUMMARY_TYPE *)get_nvram_plu_summary(addr, psr->pstNext);
				next = psr->pstNext;
				get_nvram_plu_summary(addr, next, psr);
				k++;
			   }
			   if (psr==NULL) ok=0;
			   if ((psr->pstPLU==0) || (psr->pstDept==0)) ok=0;
			   if (ok) {
				   sprintf(&commun_buffer[r],"E=0.");
				   r+=strlen(&commun_buffer[r]);
				   sprintf(&commun_buffer[r],"n=%04X.",psr->pstTransactions[period]);
				   r+=strlen(&commun_buffer[r]);
				   sprintf(&commun_buffer[r],"P=%05lX.",psr->pstPLU);
				   r+=strlen(&commun_buffer[r]);
				   sprintf(&commun_buffer[r],"D=%02X.",psr->pstDept);
				   r+=strlen(&commun_buffer[r]);
				   sprintf(&commun_buffer[r],"T=%02X.",psr->pstPLUtype);
				   r+=strlen(&commun_buffer[r]);
				   sprintf(&commun_buffer[r],"V1=%08lX.",psr->pstVolume[period]);
				   r+=strlen(&commun_buffer[r]);
				   sprintf(&commun_buffer[r],"W1=%08lX.",psr->pstWeight[period]);
				   r+=strlen(&commun_buffer[r]);
				   sprintf(&commun_buffer[r],"Q1=%08lX.",psr->pstQty[period]);
				   r+=strlen(&commun_buffer[r]);
#if (!(ROM_COUNTRY == ROM_RUS) && !defined(USE_INDIA_FUNCTION))
				   sprintf(&commun_buffer[r],"V2=%08lX.",psr->pstCountOfLabelVolume[period]);
				   r+=strlen(&commun_buffer[r]);
				   sprintf(&commun_buffer[r],"C2=%04X.",psr->pstCountOfLabel[period]);
				   r+=strlen(&commun_buffer[r]);
#endif
				   sprintf(&commun_buffer[r],"V3=%08lX.",psr->pstPrepackVolume[period]);
				   r+=strlen(&commun_buffer[r]);
				   sprintf(&commun_buffer[r],"W3=%08lX.",psr->pstPrepackWeight[period]);
				   r+=strlen(&commun_buffer[r]);
#if !(ROM_COUNTRY == ROM_RUS)
				   sprintf(&commun_buffer[r],"X=%08lX.",psr->pstTax[period]);
				   r+=strlen(&commun_buffer[r]);
#endif
			   } else {
				   sprintf(&commun_buffer[r],"E=1.");
				   r+=strlen(&commun_buffer[r]);
			   }
		   }
		   Report_GetResetTime(addrDate,&rtc,period);
		   break;
	   case 4: //tp = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_GROUP_SUMMARY, id);
	       get_nvram_summary_addr(NVRAM_GROUP_SUMMARY, id, tp);
		   Report_GetResetTime(NVRAM_RESET_TIME_GROUP,&rtc,period);
		   break;
	   case 5: //tp = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_DEPT_SUMMARY , id);
	       get_nvram_summary_addr(NVRAM_DEPT_SUMMARY , id, tp);
		   Report_GetResetTime(NVRAM_RESET_TIME_DEPT,&rtc,period);
		   break;
	   case 6: //tp = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_HOURLY_SUMMARY, id);
	       get_nvram_summary_addr(NVRAM_HOURLY_SUMMARY, id, tp);
		   Report_GetResetTime(NVRAM_RESET_TIME_HOURLY,&rtc,period);
		   break;
	   case 7: //tp = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_CLERK_SUMMARY, id);
		   get_nvram_summary_addr(NVRAM_CLERK_SUMMARY, id, tp);
		   if (tp==NULL) {
			   sprintf(&commun_buffer[r],"E=1.");
			   r+=strlen(&commun_buffer[r]);
			   break;
		   }
		   sprintf(&commun_buffer[r],"N01=");
		   r+=strlen(&commun_buffer[r]);
		   sprintf(&commun_buffer[r],"%04X,%08lX.",tp->sstTransactions[period],tp->sstVolume[period]);
		   r+=strlen(&commun_buffer[r]);
		   // add for Greek	// CJK080509
		   sprintf(&commun_buffer[r],"W01=%08lX.",tp->sstWeight[period]);
		   r+=strlen(&commun_buffer[r]);
		   sprintf(&commun_buffer[r],"Q01=%08lX.",tp->sstQty[period]);
		   r+=strlen(&commun_buffer[r]);
		   //////////////

		   //tp = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_VOID_CLERK, id);
		   get_nvram_summary_addr(NVRAM_VOID_CLERK, id, tp);
		   sprintf(&commun_buffer[r],"N02=");
		   r+=strlen(&commun_buffer[r]);
		   sprintf(&commun_buffer[r],"%04X,%08lX.",tp->sstTransactions[period],tp->sstVolume[period]);
		   r+=strlen(&commun_buffer[r]);

		   //tp = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_REPACK_CLERK, id);
		   get_nvram_summary_addr(NVRAM_REPACK_CLERK, id, tp);
		   sprintf(&commun_buffer[r],"N03=");
		   r+=strlen(&commun_buffer[r]);
		   sprintf(&commun_buffer[r],"%04X,%08lX.",tp->sstTransactions[period],tp->sstVolume[period]);
		   r+=strlen(&commun_buffer[r]);

		   //tp = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_OVERRIDE_CLERK, id);
		   get_nvram_summary_addr(NVRAM_OVERRIDE_CLERK, id, tp);
		   sprintf(&commun_buffer[r],"N04=");
		   r+=strlen(&commun_buffer[r]);
		   sprintf(&commun_buffer[r],"%04X,%08lX.",tp->sstTransactions[period],tp->sstVolume[period]);
		   r+=strlen(&commun_buffer[r]);

		   //tp = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_DISCOUNT_CLERK, id);
		   get_nvram_summary_addr(NVRAM_DISCOUNT_CLERK, id, tp);
		   sprintf(&commun_buffer[r],"N05=");
		   r+=strlen(&commun_buffer[r]);
		   sprintf(&commun_buffer[r],"%04X,%08lX.",tp->sstTransactions[period],tp->sstVolume[period]);
		   r+=strlen(&commun_buffer[r]);

		   wparam = get_nvram_wparam(NVRAM_CASH_CLERK+id*4+period*2);
		   sprintf(&commun_buffer[r],"K=");
		   r+=strlen(&commun_buffer[r]);
		   sprintf(&commun_buffer[r],"%04X.",wparam);
		   r+=strlen(&commun_buffer[r]);

		   //tp = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_PREPACK_CLERK, id);
		   get_nvram_summary_addr(NVRAM_PREPACK_CLERK, id, tp);
		   sprintf(&commun_buffer[r],"N06=");
		   r+=strlen(&commun_buffer[r]);
		   sprintf(&commun_buffer[r],"%04X,%08lX.",tp->sstTransactions[period],tp->sstVolume[period]);
		   r+=strlen(&commun_buffer[r]);

		   //tp = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_RETURN_CLERK, id);
		   get_nvram_summary_addr(NVRAM_RETURN_CLERK, id, tp);
		   sprintf(&commun_buffer[r],"N07=");
		   r+=strlen(&commun_buffer[r]);
		   sprintf(&commun_buffer[r],"%04X,%08lX.",tp->sstTransactions[period],tp->sstVolume[period]);
		   r+=strlen(&commun_buffer[r]);

#ifdef USE_REPORT_PAY
		   for(j = 0; j < MAX_PAID_TYPE; j++) {
			 //sr = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_PAY_CLERK, id*(MAX_PAID_TYPE)+j);
			 get_nvram_summary_addr(NVRAM_PAY_CLERK, id*(MAX_PAID_TYPE)+j, tp);
			 sprintf(&commun_buffer[r], "S%02X=",j+1);
			 r+=strlen(&commun_buffer[r]);
			 sprintf(&commun_buffer[r], "%04X,%08lX.",tp->sstTransactions[period],tp->sstVolume[period]);
			 r+=strlen(&commun_buffer[r]);
		   }
		   //tp = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_CHANGE_CLERK, id);	//Modified by JJANG 20071105
		   get_nvram_summary_addr(NVRAM_CHANGE_CLERK, id, tp);
		   sprintf(&commun_buffer[r],"N09=");
		   r+=strlen(&commun_buffer[r]);
		   sprintf(&commun_buffer[r],"%04X,%08lX.",tp->sstTransactions[period],tp->sstVolume[period]);
		   r+=strlen(&commun_buffer[r]);
#endif

		   //tp = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_NEGATIVE_CLERK, id);
		   get_nvram_summary_addr(NVRAM_NEGATIVE_CLERK, id, tp);
		   sprintf(&commun_buffer[r],"N0B=");
		   r+=strlen(&commun_buffer[r]);
		   sprintf(&commun_buffer[r],"%04X,%08lX.",tp->sstTransactions[period],tp->sstVolume[period]);
		   r+=strlen(&commun_buffer[r]);
		   Report_GetResetTime(NVRAM_RESET_TIME_CLERK,&rtc,period);
		   break;
#if (defined(USE_REPORT_TAX) && defined(USE_TAX_RATE))
	    case 8:
	    	   if (id == 0) id = scaleNum;
	    	   for (j = 1; j <= MAX_TAX_NO; j++) {	// j=10(tax free) 포함
			INT32U	addr;
		   	flash_addr = DFLASH_BASE + FLASH_TAX_AREA + (TAX_STRUCT_SIZE * (INT32U)(j-1));
		   	Flash_sread(flash_addr, (HUGEDATA INT8U*)&tax_str, sizeof(TAX_STRUCT));
			if (j == MAX_TAX_NO || j != tax_str.tax_code) {	// j=MAX_TAX_NO(10)은 tax 없는 상품의 tax report
				tax_str.tax_code = 0;
				tax_str.tax_rate = 0;
		   	}
			addr = (id*MAX_TAX_NO*MAX_REPORT_PERIOD*4)+((j-1)*MAX_REPORT_PERIOD*4+period*4);
			sprintf(&commun_buffer[r],"Y%02X=",j);
			r+=strlen(&commun_buffer[r]);
			sprintf(&commun_buffer[r],"%1X%04lX,", tax_str.tax_code, tax_str.tax_rate);
			r+=strlen(&commun_buffer[r]);
			tax_value = get_nvram_lparam(addr + NVRAM_TAX);
			sprintf(&commun_buffer[r],"%08lX.",tax_value);
			r+=strlen(&commun_buffer[r]);
			
			// gross amount
			sprintf(&commun_buffer[r], "Z%02X=", j);
			r += strlen(&commun_buffer[r]);
			tax_value = get_nvram_lparam(addr + NVRAM_TAX_GROSS);	// gross amount
			sprintf(&commun_buffer[r],"%08lX.", tax_value);
			r+=strlen(&commun_buffer[r]);
		   }
		   Report_GetResetTime(NVRAM_RESET_TIME_SCALE,&rtc,period);	// CJK070226
		   break;
#endif
	}
	if ((part>=4) && (part<7)) {
	    if (tp) {
		sprintf(&commun_buffer[r],"M=");
		r+=strlen(&commun_buffer[r]);
		sprintf(&commun_buffer[r],"%04x,%08lx.",tp->sstTransactions[period],tp->sstVolume[period]);
		r+=strlen(&commun_buffer[r]);
		// add for Greek	// CJK080509
		sprintf(&commun_buffer[r],"W=%08lX.",tp->sstWeight[period]);
		r+=strlen(&commun_buffer[r]);
		sprintf(&commun_buffer[r],"Q=%08lX.",tp->sstQty[period]);
		r+=strlen(&commun_buffer[r]);
		//////////////
	    } else {
		sprintf(&commun_buffer[r],"E=1.");
		r+=strlen(&commun_buffer[r]);
	    }
	}
	sprintf(&commun_buffer[r],"y=%02X%02X%02X.",rtc.year,rtc.month,rtc.date);
	r+=strlen(&commun_buffer[r]);
	sprintf(&commun_buffer[r],"h=%02X%02X%02X.",rtc.hour,rtc.min,rtc.sec);
	r+=strlen(&commun_buffer[r]);

	bcc=gen_bcc((INT8U *)&commun_buffer[r_start],r-r_start);
//
//012345678901234567890
//W43F0000,000000L0000:
	sprintf(commun_buffer,"W43F%04lX",spart);
	sprintf(&commun_buffer[8],",%06X",id);
	sprintf(&commun_buffer[15],"L%04X",r-r_start);
	commun_buffer[r_start-1]=':';
	commun_buffer[r++]=bcc;
	commun_outleng(CBuf,(char *)commun_buffer,r);
//	}
	return 1;
}

INT8U commun_send_other(HUGEDATA COMM_BUF *CBuf,INT32U spart,INT32U sid)
{
	ETH_CONNECT_LIST eth;
	INT16S   r,r_start;
	INT16U part,id,hid,i;
	char *commun_buffer;
	char ret_str[20];
	INT8U bcc;
	INT32U addr;
	CAL_HISTORY hist;
	FW_VERSION fversion;
	FW_VERSION2 fversion2;
#ifdef USE_CTS_FUNCTION
	CAL_INFORMATION calInfo;
	INT8U desc1[64];
	INT8U backupUnit;
#endif

	version_get(&fversion);
	version2_get(&fversion2);

	memset((INT8U *)&eth,0,sizeof(ETH_CONNECT_LIST));
	commun_buffer=(char *)serial_send_point;
	r=15;
	r_start=r;
	part = (INT16U)spart;
	id   = (INT16U)sid;
#ifdef USE_CTS_FUNCTION
	if (part != 0x12 && part != 0x13 && part != 0x14 && part != 0x17)
	{
		ethernet_gen_head(&commun_buffer[r]);
		r+=strlen(&commun_buffer[r]);
	}
#endif

	switch (part) {
		case 1: // send rtc
			commun_getrtc(3,(INT8U *)&commun_buffer[r]);
			r+=strlen((char *)&commun_buffer[r]);
			goto XCOMMON;
		case 2: // send system password
			get_base_sparam(FLASH_START_PASSWORD, (INT8U*)ret_str, DEALER_PASSWORD_LEN);
			ret_str[DEALER_PASSWORD_LEN]=0;
			sprintf(&commun_buffer[r],"P=%02lX.",strlen(ret_str));
			r+=strlen((char *)&commun_buffer[r]);
			sprintf(&commun_buffer[r],"%s",ret_str);
			r+=strlen((char *)&commun_buffer[r]);
			goto XCOMMON;
		case 3: sprintf(&commun_buffer[r],"F=%02X%02X%02X.",fversion.version,fversion.reversion[0],fversion.reversion[1]);
			r+=strlen((char *)&commun_buffer[r]);
			sprintf((char *)&commun_buffer[r],"S=%02X%02X.",fversion.subversion[0],fversion.subversion[1]);
			r+=strlen((char *)&commun_buffer[r]);
			get_base_sparam(FLASH_STR_VERSION,(INT8U *)ret_str,2);
			sprintf(&commun_buffer[r],"V=%02X%02X.",ret_str[0],ret_str[1]);
			r+=strlen((char *)&commun_buffer[r]);
			bcc=get_global_bparam(GLOBAL_DISPLAY_WEIGHTDECIMAL);
			sprintf(&commun_buffer[r],"w=%X.",bcc);
			r+=strlen((char *)&commun_buffer[r]);
			bcc=get_global_bparam(GLOBAL_AREA_PRICE_DECIMALPOINT);
			sprintf(&commun_buffer[r],"p=%X.",bcc);
			r+=strlen((char *)&commun_buffer[r]);
			bcc=ad_get_kglb();
			sprintf(&commun_buffer[r],"K=%X.",bcc);
			r+=strlen((char *)&commun_buffer[r]);
			bcc=ad_get_capa_idx();
			sprintf(&commun_buffer[r],"C=%X.",bcc);
			r+=strlen((char *)&commun_buffer[r]);
			goto XCOMMON;
#ifdef USE_VIETNAM_BAEKHWASAN_MART
		case 4: 
			if(Price.NetWeight <0)
				sprintf(&commun_buffer[r],"W=------.");
			else
				sprintf(&commun_buffer[r],"W=%06lX.",status_scale.Weight);
			r+=strlen((char *)&commun_buffer[r]);
			bcc=get_global_bparam(GLOBAL_DISPLAY_WEIGHTDECIMAL);
			sprintf(&commun_buffer[r],"w=%X.",bcc);
			r+=strlen((char *)&commun_buffer[r]);
			if(ADM_GetData.Stable == OFF)
				sprintf(&commun_buffer[r],"E=2.");
			else if(Price.NetWeight <0)
				sprintf(&commun_buffer[r],"E=1.");
			else if(ADM_GetData.Overload == ON)
				sprintf(&commun_buffer[r],"E=1.");
			else
				sprintf(&commun_buffer[r],"E=0.");
			r+=strlen((char *)&commun_buffer[r]);	
			goto XCOMMON;
#else //USE_VIETNAM_BAEKHWASAN_MART
		case 4: 
#ifdef USE_UNDER_LIMIT_MINUS20D
			if(Operation.UnderWeight == ON)
				sprintf(&commun_buffer[r],"W=------");
			else
				sprintf(&commun_buffer[r],"W=%06lX.",status_scale.Weight);
#else
			sprintf(&commun_buffer[r],"W=%06lX.",status_scale.Weight);
#endif
			r+=strlen((char *)&commun_buffer[r]);
			bcc=get_global_bparam(GLOBAL_DISPLAY_WEIGHTDECIMAL);
			sprintf(&commun_buffer[r],"w=%X.",bcc);
			r+=strlen((char *)&commun_buffer[r]);
			goto XCOMMON;
#endif //USE_VIETNAM_BAEKHWASAN_MART
		case 5: hid=get_base_wparam(FLASH_CALLOG_COUNT);
			hid&=0x7FFF;		// 0x8000으로 History qty update 되없음을 판단
			id%=5;
			for (i=0; i<id; i++) {
				if (hid==0) hid=4;
				else hid--;
			}
			addr =(INT32U)hid;
			addr%=HISTORY_QTY;
			addr*=HISTORY_STRUCT_SIZE;
			addr+=DFLASH_BASE;
			addr+=FLASH_CALLOG_DATA;
			Flash_sread(addr,(HUGEDATA INT8U *)&hist,sizeof(hist));
			sprintf(&commun_buffer[r],"N=%04X.",hid);
			r+=strlen((char *)&commun_buffer[r]);

			sprintf(&commun_buffer[r],"Y=%02X.M=%02X.D=%02X.",hist.year,hist.month,hist.date);
			r+=strlen((char *)&commun_buffer[r]);

			sprintf(&commun_buffer[r],"h=%02X.m=%02X.s=%02X.",hist.hour,hist.min,hist.sec);
			r+=strlen((char *)&commun_buffer[r]);

			sprintf(&commun_buffer[r],"C=%04lX.U=%06lX.Z=%06lX.",hist.capa,hist.ad_span,hist.ad_zero);
			r+=strlen((char *)&commun_buffer[r]);

			if (hist.ver >= 1)
			{
				sprintf(&commun_buffer[r],"u=%04lX.d=%06lX.",hist.ad_midup,hist.ad_middn);
				r+=strlen((char *)&commun_buffer[r]);
			}
			
//			sprintf(&commun_buffer[r],"G=%02X.P=%02X.",hist.sensor_gab,hist.sensor_peel);
//			r+=strlen((char *)&commun_buffer[r]);
XCOMMON:
			bcc=gen_bcc((INT8U *)&commun_buffer[r_start],r-r_start);
			sprintf(commun_buffer,"W45F%02X,%03XL",part,id);
			sprintf(&commun_buffer[10],"L%03X",r-r_start);
			commun_buffer[r_start-1]=':';
			commun_buffer[r++]=bcc;
			break;
		case 6: hid=(INT16U)get_global_bparam(GLOBAL_LABEL_PRIORITY);
			sprintf(&commun_buffer[r],"U=%02X.",hid);
			r+=strlen((char *)&commun_buffer[r]);
			for (i=0; i<3; i++) {
				hid=(INT16U)get_global_wparam(GLOBAL_LABEL_STANDARD+i*2);
				sprintf(&commun_buffer[r],"%c=%03X.",'a'+i,hid);
				r+=strlen((char *)&commun_buffer[r]);
			}
			bcc=get_global_bparam(GLOBAL_PRINT_ADDUP_FLAG2);
			ret_str[0]=0;
			ret_str[1]=0;
			if (bcc&0x04) ret_str[0]=1;
			if (bcc&0x08) ret_str[1]=1;
			sprintf(&commun_buffer[r],"R=%1X.",ret_str[0]);
			r+=strlen((char *)&commun_buffer[r]);
			sprintf(&commun_buffer[r],"S=%1X.",ret_str[1]);
			r+=strlen((char *)&commun_buffer[r]);
			goto XCOMMON;
//			break;
		case 7: hid=(INT16U)get_global_bparam(GLOBAL_BARCODE_PRIORITY);
			sprintf(&commun_buffer[r],"U=%02X.",hid);
			r+=strlen((char *)&commun_buffer[r]);
			for (i=0; i<9; i++) {
				hid=(INT16U)get_global_bparam(GLOBAL_BARCODE_STD_WID+i);
				sprintf(&commun_buffer[r],"%c=%02X.",'a'+i,hid);
				r+=strlen((char *)&commun_buffer[r]);
			}
			goto XCOMMON;
		case 8: hid=(INT16U)get_global_bparam(GLOBAL_DISCOUNT_PRIORITY);
			sprintf(&commun_buffer[r],"U=%02X.",hid);
			r+=strlen((char *)&commun_buffer[r]);
			goto XCOMMON;
#ifdef USE_TAX_RATE            
		case 9: hid=(INT16U)get_global_bparam(GLOBAL_TAX_FLAG);
			sprintf(&commun_buffer[r],"U=%02X.",hid);
			r+=strlen((char *)&commun_buffer[r]);

			hid=(INT16U)get_global_bparam(GLOBAL_TAX_VALUE);
			sprintf(&commun_buffer[r],"a=%03X.",hid);
			r+=strlen((char *)&commun_buffer[r]);
			goto XCOMMON;
#endif            
		case 0x10: hid=(INT16U)get_global_wparam(GLOBAL_SCROLL_WAIT_TIME);
			sprintf(&commun_buffer[r],"W=%04X.",hid/10);
			r+=strlen((char *)&commun_buffer[r]);
			goto XCOMMON;
#ifdef USE_CTS_FUNCTION
		case 0x12: 
			memset((INT8U*)&calInfo, 0, sizeof(CAL_INFORMATION));
			
			calInfo.CalDate[0] = CtsStruct.CtsCalYear;
			calInfo.CalDate[1] = CtsStruct.CtsCalMonth;
			calInfo.CalDate[2] = CtsStruct.CtsCalDate;
			calInfo.CalDate[3] = 0;
			calInfo.CalDate[4] = 0;
			calInfo.CalDate[5] = 0;

			calInfo.CalZero = CtsEncryptBufStruct.zero;
			calInfo.CalFull = CtsEncryptBufStruct.span;
			calInfo.CalMidUp = CtsEncryptBufStruct.midup;
			calInfo.CalMidDn = CtsEncryptBufStruct.middn;

			calInfo.GravityFactory = get_ad_wparam(GLOBAL_AD_GRAVITY_FACTORY);
			calInfo.GravityLocal = get_ad_wparam(GLOBAL_AD_GRAVITY_LOCAL);

			backupUnit = CurPluWeightUnit;
			CurPluWeightUnit = ADM_Status.UseUnit;
			calInfo.Capa = ad_get_capa();
			hid = ad_get_interval(0);
			calInfo.d = ad_cnv_adm2main(hid);
			calInfo.e = calInfo.d;
			calInfo.wdp = get_decimal_pt(3, (INT16U)ADM_Status.UseUnit);
			GetWeightSymbol(ADM_Status.UseUnit, desc1);
			strncpy((char*)calInfo.unit, (char*)desc1, 2);
			CurPluWeightUnit = backupUnit;

			calInfo.ctsEnable = CtsStruct.CtsEnable;
			calInfo.ctsPeriod = CtsStruct.CtsPeriod;
			calInfo.boardId = CtsStruct2.BoardId;
			
			if (status_run.run_mode == RUN_CALIBRATION) calInfo.calState = 1;
            else if (status_run.run_mode == RUN_WAITING_CAL) calInfo.calState = 2;
			else calInfo.calState = 0;
			
			sprintf((char*)calInfo.ModelName, MODEL_NAME);
			memcpy(calInfo.SerialNumber, CtsStruct.SerialNo, 16);
			memcpy(calInfo.productCode, CtsStruct2.ProductCode, 16);

			calInfo.FWVersion = fversion.version - '0';
			calInfo.FWVersion *= 10;
			calInfo.FWVersion += fversion.reversion[0] - '0';
			calInfo.FWVersion *= 10;
			calInfo.FWVersion += fversion.reversion[1] - '0';
			calInfo.FWVersion *= 10;
			calInfo.FWVersion += fversion.subversion[0] - '0';
			calInfo.FWVersion *= 10;
			calInfo.FWVersion += fversion.subversion[1] - '0';

			memcpy(&commun_buffer[r], (INT8U *)&calInfo, sizeof(CAL_INFORMATION));
			r+=sizeof(CAL_INFORMATION);

			part = 0x11;	// CTS CAL information의 구조가 바뀌면서 return code가 12에서 11로 변경
			goto XCOMMON;
		case 0x14: // Serial No
			Flash_sread(DFLASH_BASE + FLASH_CTS_STRUCT + 2, desc1, 16);
			desc1[16] = 0;
			i=strlen((char*)desc1);
			sprintf(&commun_buffer[r],"P=%02X.",i);
			r+=strlen(&commun_buffer[r]);
			sprintf(&commun_buffer[r],"%s",(char*)desc1);
			r+=strlen(&commun_buffer[r]);
			goto XCOMMON;
		case 0x17: // Product code
			Flash_sread(DFLASH_BASE + FLASH_CTS_STRUCT2, desc1, 16);
			desc1[16] = 0;
			i=strlen((char*)desc1);
			sprintf(&commun_buffer[r],"P=%02X.",i);
			r+=strlen(&commun_buffer[r]);
			sprintf(&commun_buffer[r],"%s",(char*)desc1);
			r+=strlen(&commun_buffer[r]);
			goto XCOMMON;
#endif
		default: return 0x99;
	}
	commun_outleng(CBuf,(char *)commun_buffer,r);
	return 1;
}

INT8U commun_send_pluinhibit(HUGEDATA COMM_BUF *CBuf,INT32U spart,INT8U next)
{
	ETH_CONNECT_LIST eth;
	INT16S    r;
	INT16S    part,r_start,check;
	INT16U id,ptype,i;
	INT8U  bcc,pset;
	char *commun_buffer;

	memset((INT8U *)&eth,0,sizeof(ETH_CONNECT_LIST));
	commun_buffer=(char *)serial_send_point;

	part = (INT16S)spart;
	id   = (INT32U)spart;
	if (id  <=0) return 0x82;

	r_start=16;
	r=r_start;
	ethernet_gen_head(&commun_buffer[r]);
	r+=strlen(&commun_buffer[r]);
	if (next) {
		id --;
		if (id>=plu_n_field) return 0x99;
		ptype = plu_get_ptype_from_fid(id);
		pset = 0;
		if (ptype&0x8000) pset=1;
		ptype&= 0x7fff;
		sprintf(&commun_buffer[r],"P=%02X.",ptype);
		r+=strlen(&commun_buffer[r]);
		sprintf(&commun_buffer[r],"S=%02X.",pset);
		r+=strlen(&commun_buffer[r]);
	} else {
		check=0;
		for (i=0; i<plu_n_field; i++) {
			ptype=plu_global[id].ptype;
			ptype&=0x7fff;
			if (id==ptype) {
				sprintf(&commun_buffer[r],"P=%02X.",ptype);
				r+=strlen(&commun_buffer[r]);
				sprintf(&commun_buffer[r],"S=%02X.",plu_global[i].inhibit);
				r+=strlen(&commun_buffer[r]);
				check=1;
				break;
			}
		}
		if (check==0) return 0x97;
	}
	bcc=gen_bcc((INT8U *)&commun_buffer[r_start],r-r_start);
	sprintf(commun_buffer,"W36F%02X,%03XL",part,id);
	sprintf(&commun_buffer[10],"L%04X",r-r_start);
	commun_buffer[r_start-1]=':';
	commun_buffer[r++]=bcc;
	commun_outleng(CBuf,(char *)commun_buffer,r);
	return 1;
}

INT8U commun_send_labelbmp_list(HUGEDATA COMM_BUF *CBuf,INT16S number)
{
	INT32U addr;
	INT16U labelid;
	INT8U  ret;
	INT16S    r,i,cnt;
	char   *commun_buffer;
	INT16S    labelMaxNum = 20;
	INT16S    bmpMaxNum   = 14;
	commun_buffer=(char *)serial_send_point;

	r=15;
	ethernet_gen_head(&commun_buffer[r]);
	r+=strlen(&commun_buffer[r]);	  

	cnt=0;
	addr  = DFLASH_BASE;
	if (number==1) {   
		addr += FLASH_LABEL_AREA;
		for (i=0; i<labelMaxNum; i++) {
			labelid  =Flash_wread(addr);
			if ((labelid>0) && (labelid<=99)) {
				Flash_sread(addr, (HUGEDATA INT8U *)&commun_buffer[r], LABEL_INFO_SIZE);
				r+=LABEL_INFO_SIZE;				
				cnt++;
			}
			addr+=LABEL_INFO_SIZE;
		}
	} 
	else if (number==2) 
	{
		addr += FLASH_BITMAP_AREA;
		for (i=0; i<bmpMaxNum; i++) {
			labelid  =Flash_wread(addr);
			if ((labelid>0) && (labelid<=99)) {
				Flash_sread(addr, (HUGEDATA INT8U *)&commun_buffer[r], BITMAP_INFO_SIZE);
				r+=BITMAP_INFO_SIZE;				
				cnt++;
			}
			addr+=BITMAP_INFO_SIZE;
		}
	}
	// 01234567890123456
	// W38F01,01L0001
	sprintf(commun_buffer,"W38F%02X,%02XL%04X",number,cnt,r-15);
	commun_buffer[14]=':';
	commun_buffer[r]=gen_bcc((INT8U *)&commun_buffer[15],r-15);
	r++;
	commun_outleng(CBuf,(char *)commun_buffer,r);
	ret=1;
	return ret;
}

INT8U commun_send_block_data(HUGEDATA COMM_BUF *CBuf,INT32U cmd,INT8U data,INT32U no)
{
	INT32U addr,max_size,pnt;
	INT16S r,d_size;
	INT8U  status,bcc,ret;
	char   *commun_buffer;
	FW_VERSION fversion;
	INT8U nvram_version[4];
	INT8U plu_version;

	version_get(&fversion);
	get_nvram_sparam(NVRAM_VERSION, nvram_version, 4);
	plu_version = plu_struct_get_version();

	commun_buffer=(char *)serial_send_point;
	addr  = commun_get_block_type_address(data,&max_size);
	pnt   = (no-1)*COM_DATABLOCK_SIZE;
	status=0;
	d_size=COM_DATABLOCK_SIZE;
	if (pnt>=max_size) d_size=0;
	else if ((pnt+d_size)>=max_size) { // W08A<no>,<datatype>L0000:<ethernet>[#=<status>.]<data>,,,,<bcc>
		d_size = max_size-pnt;
		status=1;
	}
	r=17;
	ethernet_gen_head(&commun_buffer[r]);
	r+=strlen(&commun_buffer[r]);
	sprintf((char*)&commun_buffer[r],"V=%02X%02X%02X.",fversion.version,fversion.reversion[0],fversion.reversion[1]);
	r+=strlen((char*)&commun_buffer[r]);
	sprintf((char*)&commun_buffer[r],"S=%02X%02X.",fversion.subversion[0],fversion.subversion[1]);
	r+=strlen((char *)&commun_buffer[r]);
	sprintf((char*)&commun_buffer[r],"W=%02X%02X.",Startup.version[0],Startup.version[1]);
	r+=strlen((char*)&commun_buffer[r]);
	sprintf((char*)&commun_buffer[r],"C=%03X.",Startup.country);
	r+=strlen((char*)&commun_buffer[r]);
	sprintf((char*)&commun_buffer[r],"M=%c%c.",fversion.model[0],fversion.model[1]);	// model name - CL:CL5000,
	r+=strlen((char*)&commun_buffer[r]);
	sprintf((char*)&commun_buffer[r],"P=%02X%02X.",plu_version/10%10+'0',plu_version%10+'0');// PLU Version
	r+=strlen((char*)&commun_buffer[r]);
	sprintf((char*)&commun_buffer[r],"N=%02X%02X%02X.",nvram_version[0],nvram_version[1],nvram_version[2]);	// NVRAM Version
	r+=strlen((char*)&commun_buffer[r]);
	sprintf((char*)&commun_buffer[r],"D=%02X.%05X:",status,d_size);
	r+=strlen((char*)&commun_buffer[r]);
	if (d_size) Flash_sread(addr+pnt,(HUGEDATA INT8U *)&commun_buffer[r],d_size);
	r+=d_size;
	bcc=gen_bcc((INT8U *)&commun_buffer[17],r-17);
	sprintf(commun_buffer,"W%02lXA%04lX,",cmd,no);
	sprintf(&commun_buffer[9],"%02XL%04X",data,r-17);
	commun_buffer[16]=':';
	commun_buffer[r++]=bcc;
	ret=0;
	commun_outleng(CBuf,(char *)commun_buffer,r);
	ret=1;
	return ret;
}

INT8U commun_send_addr_data(HUGEDATA COMM_BUF *CBuf,INT32U cmd,INT16U fcode)
{
	INT32U flash_addr;
	INT16S r;
	INT8U  bcc,ret;
	char   *commun_buffer;
	PARAMETER_SETTING_TYPE pset;

	ret=0;
	commun_buffer=(char *)serial_send_point;
	SearchParamSet(fcode,&pset);
	if(pset.funcCode ==0) return ret;
#ifdef USE_POLAND_DEFAULT
	if (fcode > 800 && fcode < 1000 && fcode != 995)//801~999 check cal key
#else
	if (fcode > 800 &&  fcode < 1000)//801~999 check cal key
#endif
	{
		if(!CAL)return ret = 2;
	}

	flash_addr  = DFLASH_BASE + pset.addrMemory + pset.offsetMemory;
	r=17;
	ethernet_gen_head(&commun_buffer[r]);
	r+=strlen(&commun_buffer[r]);

	if (pset.sizeMemory > 0) Flash_read_data(flash_addr, pset.sizeMemory,(HUGEDATA INT8U *)&commun_buffer[r],pset.bitFlag,pset.startBit,pset.countBit,fcode);
	r+=strlen(&commun_buffer[r]);

	sprintf((char*)&commun_buffer[r],"D=%02X.",pset.sizeMemory);
	r+=strlen((char*)&commun_buffer[r]);
	if (pset.sizeMemory>0) Flash_sread(flash_addr,(HUGEDATA INT8U *)&commun_buffer[r],pset.sizeMemory);
	r+=pset.sizeMemory;
	sprintf((char*)&commun_buffer[r],"F=%02X.",pset.bitFlag);  //bitFlag
	r+=strlen((char*)&commun_buffer[r]);
	sprintf((char*)&commun_buffer[r],"S=%02X.",pset.startBit); //startBit
	r+=strlen((char*)&commun_buffer[r]);
	sprintf((char*)&commun_buffer[r],"L=%02X.",pset.countBit); //countBit
	r+=strlen((char*)&commun_buffer[r]);
	bcc=gen_bcc((INT8U *)&commun_buffer[17],r-17);
	sprintf(commun_buffer,"W%02lXA%04X,",cmd,fcode);
	sprintf(&commun_buffer[9],"%02XL%04X",pset.sizeMemory,r-17);
	commun_buffer[16]=':';
	commun_buffer[r++]=bcc;
	commun_outleng(CBuf,(char *)commun_buffer,r);
	ret=1;
	return ret;
}

#ifdef USE_DIRECT_NUTRITION
INT8U commun_send_direct_nutrition( COMM_BUF *CBuf,INT32U plu_no,INT32U dept_no)
{
	INT16U address;
	INT16S com_type, ret, r, r_start;
	INT16U flag;
	INT32U dsize, addr_flash;
	char   desc1[MAX_TABLE_BUF],desc2[MAX_TABLE_BUF],desc3[MAX_TABLE_BUF],cc;
	INT32U	varray[30];
	NUTRIFACT_STRUCT4 nutri_fact4;
	char *commun_buffer;
	int i, k;
	INT8U bcc;

	com_type=serial_get_comtype(CBuf);
	commun_buffer=(char *)serial_send_point;
	if (dept_no > MAX_DEPT_NO) return 0x99;
	if (plu_no > MAX_PLU_NUMBER) return 0x99;
	if (!plu_table_search(dept_no,plu_no,(INT16U *)&address,0))
		return 0x98; // PLU 없음
		
	ret    = 1;
	r      = 0;
	dsize  = 0;
	memset(desc1,0,MAX_TABLE_BUF);
	memset(desc2,0,MAX_TABLE_BUF);
	memset(desc3,0,MAX_TABLE_BUF);

	if(get_direct_nutrition(dept_no, plu_no, &nutri_fact4) < 0)
		return 0x95;

	memcpy(desc1,nutri_fact4.description ,sizeof(nutri_fact4.description));
	memcpy(desc2,nutri_fact4.serving_size,sizeof(nutri_fact4.serving_size));
	memcpy(desc3,nutri_fact4.serving_per ,sizeof(nutri_fact4.serving_per));
	for (k=0; k<30; k++) 
		varray[k]=nutri_fact4.nutri_data[k];

	r_start=19;
	r=r_start;
	ethernet_gen_head(&commun_buffer[r]);
	r+=strlen(&commun_buffer[r]);
	i=strlen(desc1);
	sprintf(&commun_buffer[r],"D=%02X.",i);
	r+=strlen(&commun_buffer[r]);
	sprintf(&commun_buffer[r],"%s",desc1);
	r+=strlen(&commun_buffer[r]);

	i=strlen(desc2);
	sprintf(&commun_buffer[r],"S=%02X.",i);
	r+=strlen(&commun_buffer[r]);
	sprintf(&commun_buffer[r],"%s",desc2);
	r+=strlen(&commun_buffer[r]);
	i=strlen(desc3);
	sprintf(&commun_buffer[r],"P=%02X.",i);
	r+=strlen(&commun_buffer[r]);
	sprintf(&commun_buffer[r],"%s",desc3);
	r+=strlen(&commun_buffer[r]);

	for (i=0; i<30; i++) {
		sprintf(&commun_buffer[r],"Z=%02X:%08lX.",i,varray[i]);
		r+=strlen(&commun_buffer[r]);
	}	

	bcc=gen_bcc((INT8U *)&commun_buffer[r_start],r-r_start);
	sprintf(commun_buffer,"W47F%06lX,%02XL%04X", plu_no, dept_no, r-r_start);
	commun_buffer[r_start-1]=':';
	commun_buffer[r++]=bcc;
	commun_outleng(CBuf,(char *)commun_buffer,r);
	ret   =1;
	return 1;
}
#endif  // USE_DIRECT_NUTRITION

#ifdef USE_INDIA_FUNCTION
INT8U commun_send_inventory(HUGEDATA COMM_BUF *CBuf, INT32U spart, INT32U sid)
// ==================================================================
// next=1 이면 현재 요청된 값을 못찾을때 다음 ID로 값을 Return 한다.
// ==================================================================
{
	INT16S	  r;
	INT16S    part;
	ETH_CONNECT_LIST eth;
	INT8U  r_start,bcc;
	INT16U id,index;
	char *commun_buffer;
	PLU_SALES_SUMMARY_TYPE *psr;
	INT8U pluname[55];
	INT32U unitprice;
	INT16U plu_key;
	INT16U uz;
	INT32U pluno;
	INT16U deptno;
	INT32S inventory;
	INT32U bagWt;
	INT8U  pluType;
    
	memset((INT8U *)&eth,0,sizeof(ETH_CONNECT_LIST));
	commun_buffer=(char *)serial_send_point;

	part = (INT16S)spart;
	id   = (INT16U)sid;
	if (part<= 0 || part > 1) return 0x82;
	if (id  <= 0 ) return 0x83;
	if (id==0xffff) return 0x83;

	r      =19;
	r_start=r;
	ethernet_gen_head(&commun_buffer[r]);
	r+=strlen(&commun_buffer[r]);
	switch (part) {
		case 1: 
			plu_key=plu_get_next_exist_address((INT16U)id);
			if (plu_key==0xffff) return 0x84;
			plu_get_field(plu_key-1, PTYPE_DEPARTMENT_NO, (INT8U *)&deptno, (INT16S *)&uz, sizeof(deptno));
			plu_get_field(plu_key-1, PTYPE_PLU_NO, (INT8U *)&pluno, (INT16S *)&uz, sizeof(pluno));
			plu_get_field(plu_key-1, PTYPE_PLU_TYPE, (INT8U *)&pluType, (INT16S *)&uz, sizeof(pluType));
			psr = FindPluSummary(0, pluno, deptno, 0, &index);

			if (psr) 
			{
				if ((psr->pstPLU==0) || (psr->pstDept==0)) return 0x85;
				inventory = psr->pstInventory;
				bagWt = psr->pstInvBagWt;
			}
			else // 재고 정보가 없을 때
			{
				inventory = 0;
				bagWt = 0;
				//pluType = PLU_BY_WEIGHT; // 재고 정보가 없을 때에도 PLU Type을 바꾸지 않음
			}

			sprintf(&commun_buffer[r],"P=%02X%06lX.",deptno, pluno);
			r+=strlen(&commun_buffer[r]);
			sprintf(&commun_buffer[r],"A=%08lX.", inventory);
			r+=strlen(&commun_buffer[r]);
			sprintf(&commun_buffer[r],"B=%08lX.", bagWt);
			r+=strlen(&commun_buffer[r]);
			sprintf(&commun_buffer[r],"C=%08lX.",0);
			r+=strlen(&commun_buffer[r]);
			sprintf(&commun_buffer[r],"D=%02X.", pluType);
			r+=strlen(&commun_buffer[r]);

			plu_get_field(plu_key-1, PTYPE_PRICE, (INT8U*)&unitprice, (INT16S*)&uz, sizeof(unitprice));
			sprintf(&commun_buffer[r],"E=%08lX.", unitprice);
			r+=strlen(&commun_buffer[r]);
			plu_get_field(plu_key-1, PTYPE_PLU_NAME1, (INT8U*)pluname, (INT16S*)&uz, sizeof(pluname));
			sprintf(&commun_buffer[r],"F=%02X.", strlen((char *)pluname));
			r+=strlen(&commun_buffer[r]);
			sprintf(&commun_buffer[r],"%s", pluname);
			r+=strlen(&commun_buffer[r]);

			break;
	}

	bcc=gen_bcc((INT8U *)&commun_buffer[r_start],r-r_start);
//
//012345678901234567890
//W41F01,000000L0000:
	sprintf(commun_buffer,"W41F%02X", spart);
	sprintf(&commun_buffer[6],",%06X", id);
	sprintf(&commun_buffer[13],"L%04X", r-r_start);
	commun_buffer[r_start-1]=':';
	commun_buffer[r++]=bcc;
	commun_outleng(CBuf,(char *)commun_buffer,r);
	//ret   =1;

	return 1;
}
#endif

#ifdef USE_DIRECT_INGREDIENT
#ifdef USE_INGREDIENT_98_PROTOCOL
INT8U commun_send_directingredient(HUGEDATA COMM_BUF *CBuf,INT32U nth,INT32U block_num)
{
	INT32U address;
	char *commun_buffer;
	INT8U *data,bcc,stype;
	INT16U sz,ptype,rsz,deptno;
	INT16S    i,j,iret,r,com_type,str_start;
	INT8U  ret;
	INT32U addr,pluno;
	INT16U ingred_no[DIRECT_INGREDIENT_BLOCK_NUM];
	INT16S k;
	INT16U tsz;
	INT8U  end;
	INT16U r_temp;
	INT8U xmode =0;
	INT16S current_send_data_size;
	INT8U total_data[DEFAULT_INGREDIENT_TEXT_ALL_SIZE];
	INT16U idx;
	INT32U key;
	INT8U tempBuf[256];
	commun_buffer=(char *)serial_send_point;

	deptno=(nth>>24); // 
	nth  &=0xffffff;

	if (!plu_table_search(deptno,nth,(INT16U *)&idx,0)) {
		return 0x98; // PLU 없음
	}
	key = (INT32U)idx;
	if (key==0xffff) { key=0l; }
	ret=1;

	Flash_flush_nvbuffer();
	str_start=18;
 	r=str_start;
	
	for (i=0; i<plu_n_field; i++) {
		ptype=plu_get_ptype_from_fid(i);
		if (ptype==PTYPE_INGREDIENT_TEXT) //direct ingredient
		{
			stype=plu_get_stype_from_fid(i);
			iret =plu_callbykey_from_fid(key-1,i,&addr,&sz);
			tsz = sz;
			if (sz > sizeof(tempBuf)) tsz = sizeof(tempBuf);
			Flash_sread(addr, tempBuf, tsz);
			data = tempBuf;
			if (iret==PLUSTR_EOF) break;
			if (iret==PLUSTR_OK) {
				memcpy((INT8U *)&ingred_no[0], data, 2*DIRECT_INGREDIENT_BLOCK_NUM);
				stype='S';
				data = (HUGEDATA INT8U *)&commun_buffer[r];
				ingredient_text_read(&ingred_no[0], total_data, 0, DEFAULT_INGREDIENT_TEXT_ALL_SIZE);
				sz = strlen(total_data);	// size가 클 경우 나눠서 보내는 방법 필요
				if (sz == 0) // no data
				{
					current_send_data_size = 0;
				}
				else
				{
					if (sz > DEFAULT_INGREDIENT_TEXT_ALL_SIZE) sz = DEFAULT_INGREDIENT_TEXT_ALL_SIZE;
					if (sz > (CURRNET_MAX_INGREDIENT_SIZE * (block_num+1)))
					{
						current_send_data_size = CURRNET_MAX_INGREDIENT_SIZE;
					}
					else // (sz < (CURRNET_MAX_INGREDIENT_SIZE* (block_num+1)))
					{
						current_send_data_size = sz - (CURRNET_MAX_INGREDIENT_SIZE * block_num);
					}
				}
				// Block Number
				commun_buffer[r++]='X';
				commun_buffer[r++]='=';
				tohex2((INT8U)block_num,(char *)&commun_buffer[r]); r+=2;
				commun_buffer[r++]='.';
				// Total Dmsg Length
				commun_buffer[r++]='A';
				commun_buffer[r++]='=';
				tohex4((INT16U)sz,(char *)&commun_buffer[r]); r+=4;
				commun_buffer[r++]='.';
				// Dmsg Data
				commun_buffer[r++]='D';					
				commun_buffer[r++]='=';
				tohex4((INT16U)current_send_data_size,(char *)&commun_buffer[r]); r+=4;
				commun_buffer[r++]='.';
				if(current_send_data_size!=0)
				{
					memcpy((INT8U *)&commun_buffer[r], &total_data[CURRNET_MAX_INGREDIENT_SIZE* block_num], current_send_data_size);
					r+=current_send_data_size;
				}
			}
			break;
		}
	}
	ret = 1;
END1:
	bcc=gen_bcc((INT8U *)&commun_buffer[str_start],r-str_start);
	sprintf(commun_buffer,"W46A%05lX,",nth);
	sprintf(&commun_buffer[10],"%02XL%04X",deptno,r-str_start);
	commun_buffer[str_start-1]=':';
	commun_buffer[r++]=bcc;
	commun_outleng(CBuf,(char *)commun_buffer,r);
	return ret;
}
#endif
#endif

void commun_send_stringtype(INT16S *r,char *com,char *form,char *data)
{
	INT16S ir;
	ir = *r;
	sprintf(&com[ir],form,strlen(data));
	ir+=strlen(&com[ir]);
	sprintf(&com[ir],"%s",data);
	ir+=strlen(&com[ir]);
	*r = ir;
}
