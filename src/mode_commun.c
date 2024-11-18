/*****************************************************************************
|*  Copyright		:	(c) 2002 CAS
|*  Filename		:	mode_commun.c
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
#ifdef USE_PROTOCOL_CL5000JR
#include "cl5kj_interpreter.h"
#endif
#ifdef USE_PROTOCOL_CHINA_DAHUA	// 중국 전용 다화 프로토콜
#include "chn_dahua_interpreter.h"
#endif
#include "webclient-strings.h"
#ifdef USE_SEND_TRACE_PRICE
#include "euckr_to_utf8.h"
#endif

//#define DEFAULT_INGREDIENT_SIZE  		302l
//#define DEFAULT_INGREDIENT_TEXT_SIZE	304l
//#define DEFAULT_DIRECT_INGREDIENT_ALL_SIZE  ((DEFAULT_DIRECT_INGREDIENT_SIZE - 2) * DIRECT_INGREDIENT_BLOCK_NUM)  // ingredient 전체길이

//#define _USE_DBG_PLU_SYNC

INT16U serial_mode[MAX_NUM_COM_TYPE];
INT8U  serial_command[MAX_NUM_COM_TYPE];
INT8U  serial_cmdtype[MAX_NUM_COM_TYPE];
INT8U  serial_errtype[MAX_NUM_COM_TYPE];
INT8U  serial_bcc[MAX_NUM_COM_TYPE];
INT8U  serial_ethtype;
INT16U serial_length[MAX_NUM_COM_TYPE];
INT32U serial_number[MAX_NUM_COM_TYPE];
INT32U serial_fnumber[MAX_NUM_COM_TYPE];
INT16U serial_timeout_n[MAX_NUM_COM_TYPE];
INT16U serial_param_len[MAX_NUM_COM_TYPE];
ETH_CONNECT_LIST serial_eth[MAX_NUM_COM_TYPE];
HUGEDATA INT8U serial_buffer_point[MAX_NUM_COM_TYPE][MAX_SERIAL_BUFFER_LEN];
HUGEDATA INT8U serial_send_point[MAX_SERIAL_BUFFER_LEN];
// Client 송신용 버퍼 분리
//  - Client 통신 중, Server interpreter(serial_proc함수 또는 network_common_trans함수)가 수행되는 경우,
//    Tx Buffer(serial_send_point)의 data가 바뀌어서 잘못된 data로 retry하는 경우 발생
//  - tx할 내용을 한 번 만들어서 retry 할 경우 serial_send_point_for_client[] 를 사용해야함
//     ex) transaction_sale_send(), transaction_pay_send()
//  - tx할 내용을 만들어서 Interpreter를 거치지 않고 바로 전송(commun_outleng함수)하는 경우
//    serial_send_point를 사용하도록 놔둠
HUGEDATA INT8U serial_send_point_for_client[MAX_CLIENT_TX_BUF_LEN];
short  serial_param_nstr[MAX_NUM_COM_TYPE];
HUGEDATA INT16U recv_counter;
extern INT16U plu_n_field;
extern PLU_GLOBAL_INFO_STRUCT plu_global[128];
extern INT8U global_one2one_send;

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
extern INT16U ClerkTransactionCnt(INT16U ClerkNum);
extern INT16S LocalPluTotalSummaries(INT8U scale,TRANSACTION_RECORD_TYPE* pTrans,TRANSACTION_RECORD_SUB_TYPE* pTrans_sub,TR_ADDITION_DATA_TYPE* pTrans_add,INT8U recvmode,INT8U onlyvoid);
extern INT16S gen_transaction_pay(char *commun_buffer,TRANSACTION_PAY trans);
extern INT16S Network_ClerkTransaction(HUGEDATA COMM_BUF *CBuf,ETH_CONNECT_LIST eth,unsigned char ClerkNum,INT16S id);
extern void send_clerk_sale_data(HUGEDATA COMM_BUF *CBuf,ETH_CONNECT_LIST eth,INT8U ClerkNum,INT16S id,TRANSACT_CHAIN *xtrans);
extern INT16S  PutSmallString(INT16S y, INT16S x,char *data);
extern void Report_WriteCurResetTime(INT32U rel_addr, INT16S period);
extern INT16S  ClearPluSummaryRecord(PLU_SALES_SUMMARY_TYPE* tp,INT16S counter);
extern INT16S  ClearSummaryRecord(SALES_SUMMARY_TYPE* tp,INT16S counter);
extern long PayChangeSummary(TRANSACTION_PAY trans_pay);
extern void Report_GetResetTime(INT32U rel_addr, RTC_STRUCT *time_str, INT8U period);
extern void transaction_pay_end(HUGEDATA COMM_BUF *CBuf,INT8U tail);
extern void transaction_pay_send_abort(HUGEDATA COMM_BUF *CBuf,INT8U tail);
extern INT16U plu_ptype_reset(void);
void  commun_recv_monitor(char key,INT16S xtype,INT16S v);
extern void plu_save_deptid_com_type(INT8U m,INT32U pnt,INT32U data,INT8U stype,INT16U usize);
extern INT32U ing_calc_addr_by_key(INT16U ingid);
extern void ingredient_delete_all(void);
extern void Flash_write_data(INT32U startAddr,INT16U memSize,HUGEDATA INT8U *value,INT8U v_leng,INT8U bitFlag,INT8U startBit,INT8U countBit,INT16U fcode);
extern PRICE_TYPE Price;
extern INT16U key_pole_map[];
extern INT16U key_hanging_map[];
extern INT16U key_self_map[];
extern INT16U key_doublebody_map[];
extern TARE_OPER_TYPE		TareOper;
extern ROMDATA PARAMETER_SETTING_TYPE	ParameterSetting[];

extern INT16U   global_one2one_count;
extern char	gmsg[];
extern INT8U    GlbFlagPCnMasterConnection;
extern INT16S      NetCheckMasterConnection(void);

extern void commun_write_response(HUGEDATA COMM_BUF *CBuf, INT8U *str, INT8U errCode); //SG071204
extern INT8U commun_write_plu_data(HUGEDATA COMM_BUF *CBuf,INT8U *str,INT16S leng); //SG071204
//extern void commun_send_plu_data(HUGEDATA COMM_BUF *CBuf,INT32U nth); //SG080107
extern INT8U commun_delete_plu_data(HUGEDATA COMM_BUF *CBuf,INT8U *str, INT16S leng); //SG080111
#ifdef USE_RUSSIA_LOGISTICS_PLU
static INT8U commun_write_plu_gtin_data(HUGEDATA COMM_BUF *CBuf,INT8U *str,INT16S leng);
extern void commun_write_gtin_response(HUGEDATA COMM_BUF *CBuf, INT8U *str, INT8U errCode);
#endif
#ifdef USE_PROTOCOL_CHINA_DAHUA	// 중국 전용 다화 프로토콜
static void commun_dahua_write(HUGEDATA COMM_BUF *CBuf,INT8U cmd,INT32U number,INT32U nn,INT32U length,INT8U *str);
#endif

HUGEDATA INT8U PPIState;
HUGEDATA INT8U PPITxWaitTime100ms;
volatile HUGEDATA INT8U IntReqRegPort;

#ifndef CPU_PIC32MZ
    __no_init  INT16U	PrevTransactionSaleTail[MAX_SCALE] @ (NVRAM_LAST_TRANS_COUNT+NVRAM_BASE);
    __no_init  INT16U	PrevTransactionPayTail[MAX_SCALE] @ (NVRAM_LAST_TRANS_PAY+NVRAM_BASE);
    #ifdef USE_MAX_SCALE_NO_255
    __no_init  INT16U	PrevTransactionSaleTail2[256-MAX_SCALE] @ (NVRAM_LAST_TRANS_SALE2+NVRAM_BASE);
    __no_init  INT16U	PrevTransactionPayTail2[256-MAX_SCALE] @ (NVRAM_LAST_TRANS_PAY2+NVRAM_BASE);
    #endif
#endif

#ifdef USE_CHN_IO_BOARD
INT8U UseI2CComm = OFF;
#endif

void commun_outend(HUGEDATA COMM_BUF *CBuf)
{
	if (CBuf->Type==COMM_TYPE_HOST) {
		if (CheckTxBuf(CBuf)) {
			Uart0SendData();
		}
	}
	else if (CBuf->Type==COMM_TYPE_ETHDATA) {
		if (CommBufEth.Txing != 1) {
			EthDataProc(CBuf);
		}
	}
#ifdef USE_CHN_IO_BOARD
	else if (CBuf->Type==COMM_TYPE_RFID) {
		if (UseI2CComm) i2c_txDataProcRFID();
	}
	else if (CBuf->Type==COMM_TYPE_GPRS) {
		if (UseI2CComm) i2c_txDataProcGPRS();
	}
	else if (CBuf->Type==COMM_TYPE_USBMEM) {
		if (UseI2CComm) i2c_txDataProcUsbMem();
	}
#endif
}

void commun_out(HUGEDATA COMM_BUF *CBuf,char *str)
{
	PutStrTxBuf(CBuf,(HUGEDATA char *)str);
	commun_outend(CBuf);
}

void commun_out_waiting(HUGEDATA COMM_BUF *CBuf, char *str)
{
	PutStrTxBuf(CBuf,(HUGEDATA char *)str);
	commun_outend(CBuf);

	while (CheckTxBuf(CBuf));
}

void commun_outleng(HUGEDATA COMM_BUF *CBuf,char *str,INT16S leng)
{
	PutDataTxBuf(CBuf,(HUGEDATA char *)str,leng);
	commun_outend(CBuf);
}

extern INT8U flagTempAllowSale;
void serial_init(void)
{
	INT16S i;
//	recv_display=0;
	for (i=0; i<MAX_NUM_COM_TYPE; i++) {
		serial_mode[     i]   =0;
		serial_timeout_n[i]   =0;
		serial_ethtype=0;
		memset(&serial_eth[i],0,sizeof(ETH_CONNECT_LIST));
	}
	flagTempAllowSale = OFF;	// Added by CJK 20051220
}

void serial_timeout_reset(INT8U com_type)
{
	if (serial_mode[com_type]) {
		serial_timeout_n[com_type]=SysTimer100ms;
	}
}

INT8U serial_timeout_check(INT8U com_type)
{
	INT16U tic1,tic2;
	INT16U gap;
	tic2=8;
//	tic1=15;	// serial_mode[]=0 으로 회귀할 시간. 1 sec
	tic1=5;

	if (serial_mode[com_type]) {
		if (serial_timeout_n[com_type]) {	// 일반상황
			gap=SysTimer100ms-serial_timeout_n[com_type];
			if (gap>tic1) {
				serial_mode[com_type]=0;
				return 1;
			}
		} else {	// Timeout 이 0 일때 보호용
			serial_timeout_n[com_type]=SysTimer100ms;
		}
	} else {
		gap=SysTimer100ms-serial_timeout_n[com_type];
		if (gap>tic2) {
			return 1;
		}
	}
	return 0;
}

INT8U* get_temp_area(INT8U m)
{
	INT8U* addr;
	switch (m) {
		default:
		case              0    :addr = NvramTempBuffer[0]; break;
		case COMM_TYPE_HOST    :addr = NvramTempBuffer[1];break;
		case COMM_TYPE_ETHDATA :addr = NvramTempBuffer[2]; break;
	}
	return addr;
}

//////////////////////
// comtype
//  0 : RS232C
//  1 : USB
//  2 : Ethernet Client (ID0)
//  3~6 : Ethernet Server 1~4 (ID1~4)
INT16S  serial_get_comtype(HUGEDATA COMM_BUF *CBuf)
{
	if (CBuf->Type == COMM_TYPE_HOST) return 0;
#ifndef CPU_PIC32MZ
    if (CBuf->Type == COMM_TYPE_USB) return 1;
#endif
	if (CBuf->Type == COMM_TYPE_ETHDATA) {
		if (CBuf->SocketID>ETH_DEFAULT_CLI_MAX_NUM_4_SRV) return (ETH_DEFAULT_CLI_MAX_NUM_4_SRV+2);
		return (CBuf->SocketID+2);
	}
	return 0;
}

extern INT16S CheckRxBufCountInt(HUGEDATA COMM_BUF *CBuf);
extern void CL5000JRCommInterpreter(HUGEDATA COMM_BUF *CBuf);
#ifdef USE_HTTP_CLIENT
INT8U commun_gethttp(COMM_BUF *CBuf);
#endif
void serial_proc(HUGEDATA COMM_BUF *CBuf) // Start
{
	INT8U r_data;
	INT16S   com_type,rept,ret,isz,q;
	INT8U *serial_buffer;
	WLAN_CARD_STRUCT ecard;
	ETH_CONNECT_LIST eth;
	INT8U first;
#ifdef USE_PROTOCOL_CL5000JR
	INT8U ret8;
	INT16U temp;
	INT16U fullLength;
#endif
#ifdef USE_PROTOCOL_CHINA_DAHUA	// 중국 전용 다화 프로토콜
	DAHUA_DHBZ_PLU temp_dahua_plu;
	DAHUA_TMA_PLU temp_dahua_tma_plu;
	INT8U temp_ans[11];
#endif
	INT8U backup_directdraw;
#ifdef USE_HTTP_CLIENT
	INT8U http_ok;
#endif

	backup_directdraw = DspStruct.DirectDraw;

	com_type=serial_get_comtype(CBuf);
	rept  =0;
	first =0;
	recv_counter=0;
	serial_buffer=(INT8U *)serial_buffer_point[com_type];
REPT:
	isz = CheckRxBufCountInt(CBuf);

	for (q=0; q<isz; ) {
		if (first==0) {
			DspStruct.DirectDraw = 1;
			display_sign_noflush(SIGN_NET,1);
			Operation.receiving = ON; // for NVRAM Auto Erasing
#ifndef USE_DISPLAY_OPT_CL5200
			VFD7_Diffuse();
#endif
			DspStruct.DirectDraw = backup_directdraw;
			first=1;
		} else {
			if (first==1) {
				if (recv_counter>6) {
					//VFD7_save_restore(VFD_BACKUP);
					display_save_restore(DSP_BACKUP);
					display_alloff_primary_indication();
					display_sign_noflush(SIGN_NET,1);
					strcpy(gmsg,"----");
					display_string(DISPLAY_TARE     ,(INT8U*)gmsg);
					display_string(DISPLAY_UNITPRICE,(INT8U*)gmsg);
					display_string(DISPLAY_PRICE    ,(INT8U*)gmsg);
					strcpy(gmsg,"COMM");
					display_string(DISPLAY_WEIGHT   ,(INT8U*)gmsg);
					VFD7_Diffuse();
					Dsp_Clear();
					first=9;
				}
			}
		}
		r_data = GetCharRxBuf(CBuf);
		q++;
		if (Startup.start_flag&0x100) {
			if (com_type==0) Startup.start_flag&=0xfeff;
			else {
				sprintf(gmsg,"%c",r_data);
				commun_outleng(&CommBuf,gmsg,1);
			}
		}
		switch(serial_mode[com_type]) {
#ifdef USE_PROTOCOL_CL5000JR
			case 110:
				if (serial_param_len[com_type] > 4)
				{
					commun_write_response(CBuf, serial_buffer, 84);	//syntax error
					serial_mode[com_type] = 0;
					goto TX_PROC;
				}
				if(r_data==',' && serial_param_len[com_type] == 4)
				{
					serial_mode[com_type]=111;
					serial_length[com_type]=0;
					serial_param_len[com_type] = 0;
				}
				else
				{
			  		serial_param_len[com_type]++;
					//address 4byte 처리
				}
				serial_bcc[com_type]+=r_data;
				break;
			case 111:
				if (serial_param_len[com_type] > 2)
				{
					commun_write_response(CBuf, serial_buffer, 84);	//length error
					serial_mode[com_type] = 0;
					goto TX_PROC;
				}
				if(r_data==':' && serial_param_len[com_type] == 2)
				{
					serial_param_nstr[com_type] =0;
					serial_mode[com_type]=112;
					temp = serial_length[com_type];
					serial_length[com_type] = ((temp >>8)&0xff) | ((temp<<8)& 0xff00);
                    fullLength = serial_length[com_type];
					if (serial_length[com_type] >= MAX_SERIAL_BUFFER_LEN)
					{
						commun_write_response(CBuf, serial_buffer, 84);	//length error
						serial_mode[com_type] = 0;
						goto TX_PROC;
					}
					if (serial_length[com_type] > (147 + 150*3 + 1)) //PLU_HOST_STORAGE_SIZE + PLU_STORAGE_SIZE * 3 + 1
					{
						serial_length[com_type] = (147 + 150*3 + 1);
					}
				}
				else
				{
					serial_length[com_type] <<=8;
					serial_length[com_type] |=r_data;
			  		serial_param_len[com_type]++;
				}
				serial_bcc[com_type]+=r_data;
				break;
			case 112:
REPT_112:
				if (serial_param_nstr[com_type]== (serial_length[com_type]+1))
				{
					if (r_data == serial_bcc[com_type])
					{
						if(serial_length[com_type]==6) //delete
						{
							ret8 = commun_delete_plu_data(CBuf, serial_buffer, serial_length[com_type]);
							serial_length[com_type]=0;
						}
						else
						{
							ret8 = commun_write_plu_data(CBuf, serial_buffer, fullLength);
						}
						commun_write_response(CBuf, serial_buffer, ret8);
					}
					else
					{
						commun_write_response(CBuf, serial_buffer, 84);	//checksum error
					}
				        serial_mode[com_type]=0;
					goto TX_PROC;
				 }
				 else
				 {
					serial_buffer[serial_param_nstr[com_type]++]=r_data;
					serial_bcc[com_type]+=r_data; //CL5000Jr Checksum
					if (serial_param_nstr[com_type] >= MAX_SERIAL_BUFFER_LEN)
					{
						commun_write_response(CBuf, NULL, 84);	//length error
						serial_mode[com_type] = 0;
						goto TX_PROC;
					}
				 }

				 if (q<isz) {
					r_data = GetCharRxBuf(CBuf); q++;
					goto REPT_112;
				 }

				break;
			case 120:
				if (serial_param_len[com_type] > 4)
				{
					commun_write_response(CBuf, serial_buffer, 84);	//length error
					serial_mode[com_type] = 0;
					goto TX_PROC;
				}
				if(r_data==',' && serial_param_len[com_type] == 4)
				{
					serial_mode[com_type]=121;
					serial_param_len[com_type] = 0;
				}
				else
				{
			  		serial_param_len[com_type]++;
					//address 4byte 처리
				}
				serial_bcc[com_type]+=r_data;
				break;
			case 121:
				if (serial_param_len[com_type] > 2)
				{
					commun_write_response(CBuf, serial_buffer, 84);	//length error
					serial_mode[com_type] = 0;
					goto TX_PROC;
				}
				if(r_data==':' && serial_param_len[com_type] == 2)
				{
					serial_number[com_type] =0;
					serial_param_nstr[com_type]=0;
					serial_mode[com_type]=122;
				}
				else
				{
			  		serial_param_len[com_type]++;
				}
				serial_bcc[com_type]+=r_data;
				break;
			case 122:
				if(r_data==':')
				{
					if(serial_param_nstr[com_type]==4)
					{
						memcpy((INT8U *)&serial_number[com_type],&serial_buffer[0],4);
						serial_mode[com_type]=123;
					}
				}
				serial_buffer[serial_param_nstr[com_type]++]=r_data;
				serial_bcc[com_type]+=r_data;
				if (serial_param_nstr[com_type] >= MAX_SERIAL_BUFFER_LEN)
				{
					commun_write_response(CBuf, NULL, 84);	//length error
					serial_mode[com_type] = 0;
					goto TX_PROC;
				}
				break;
			case 123:

				if (r_data == serial_bcc[com_type])
				{
					commun_send_plu_data(CBuf,serial_number[com_type]);
				}
				else
				{
					commun_write_response(CBuf, NULL, 84);	//checksum error
				}
			        serial_mode[com_type]=0;
				goto TX_PROC;

				break;
#ifdef USE_RUSSIA_LOGISTICS_PLU
			case 115:
				if (serial_param_len[com_type] > 4)
				{
					commun_write_gtin_response(CBuf, serial_buffer, 84);	//syntax error
					serial_mode[com_type] = 0;
					goto TX_PROC;
				}
				if(r_data==',' && serial_param_len[com_type] == 4)
				{
					serial_mode[com_type]=116;
					serial_length[com_type]=0;
					serial_param_len[com_type] = 0;
				}
				else
				{
			  		serial_param_len[com_type]++;
					//address 4byte 처리
				}
				serial_bcc[com_type]+=r_data;  
				break;
			case 116:
				if (serial_param_len[com_type] > 2)
				{
					commun_write_gtin_response(CBuf, serial_buffer, 84);	//length error
					serial_mode[com_type] = 0;
					goto TX_PROC;
				}
				if(r_data==':' && serial_param_len[com_type] == 2)
				{
					serial_param_nstr[com_type] =0;
					serial_mode[com_type]=117;
					temp = serial_length[com_type];
					serial_length[com_type] = ((temp >>8)&0xff) | ((temp<<8)& 0xff00);
	                fullLength = serial_length[com_type];
					if (serial_length[com_type] >= MAX_SERIAL_BUFFER_LEN) 
					{
						commun_write_gtin_response(CBuf, serial_buffer, 84);	//length error
						serial_mode[com_type] = 0;
						goto TX_PROC;
					}
					if (serial_length[com_type] > (147 + 150*3 + 1)) //PLU_HOST_STORAGE_SIZE + PLU_STORAGE_SIZE * 3 + 1
					{
						serial_length[com_type] = (147 + 150*3 + 1);
					}					
				}
				else
				{
					serial_length[com_type] <<=8;
					serial_length[com_type] |=r_data;
			  		serial_param_len[com_type]++;
				}
				serial_bcc[com_type]+=r_data; 
				break;
			case 117:
	REPT_117:	 
				if (serial_param_nstr[com_type]== (serial_length[com_type]+1)) 
				{
					if (r_data == serial_bcc[com_type]) 
					{
						if(serial_length[com_type]==6) //delete
						{
							ret8 = commun_delete_plu_data(CBuf, serial_buffer, serial_length[com_type]);
							serial_length[com_type]=0;
						}
						else
						{
							ret8 = commun_write_plu_gtin_data(CBuf, serial_buffer, fullLength);
						}
						commun_write_gtin_response(CBuf, serial_buffer, ret8);
					} 
					else 
					{
						commun_write_gtin_response(CBuf, serial_buffer, 84);	//checksum error				
					}
				        serial_mode[com_type]=0;
					goto TX_PROC;
				 } 
				 else 
				 {
					serial_buffer[serial_param_nstr[com_type]++]=r_data;
					serial_bcc[com_type]+=r_data; //CL5000Jr Checksum 
					if (serial_param_nstr[com_type] >= MAX_SERIAL_BUFFER_LEN) 
					{
						commun_write_gtin_response(CBuf, NULL, 84);	//length error
						serial_mode[com_type] = 0;
						goto TX_PROC;
					}
				 }

				 if (q<isz) {
					r_data = GetCharRxBuf(CBuf); q++;
					goto REPT_117;
				 }

				break;
			case 125:
				if (serial_param_len[com_type] > 4)
				{
					commun_write_gtin_response(CBuf, serial_buffer, 84);	//length error
					serial_mode[com_type] = 0;
					goto TX_PROC;
				}
				if(r_data==',' && serial_param_len[com_type] == 4)
				{
					serial_mode[com_type]=126;
					serial_param_len[com_type] = 0;
				}
				else
				{
			  		serial_param_len[com_type]++;
					//address 4byte 처리
				}
				serial_bcc[com_type]+=r_data;
				break;
			case 126:
				if (serial_param_len[com_type] > 2)
				{
					commun_write_gtin_response(CBuf, serial_buffer, 84);	//length error
					serial_mode[com_type] = 0;
					goto TX_PROC;
				}
				if(r_data==':' && serial_param_len[com_type] == 2)
				{
					serial_number[com_type] =0;
					serial_param_nstr[com_type]=0;
					serial_mode[com_type]=127;
				}
				else
				{
			  		serial_param_len[com_type]++;
				}
				serial_bcc[com_type]+=r_data;
				break;
			case 127:
				if(r_data==':')
				{
					if(serial_param_nstr[com_type]==4)
					{
						memcpy((INT8U *)&serial_number[com_type],&serial_buffer[0],4);
						serial_mode[com_type]=128;
					}
				}
				serial_buffer[serial_param_nstr[com_type]++]=r_data;
				serial_bcc[com_type]+=r_data;
				if (serial_param_nstr[com_type] >= MAX_SERIAL_BUFFER_LEN) 
				{
					commun_write_gtin_response(CBuf, NULL, 84);	//length error
					serial_mode[com_type] = 0;
					goto TX_PROC;
				}
				break;
			case 128: 
				if (r_data == serial_bcc[com_type])
				{
					ret8 = commun_send_plu_gtin_data(CBuf,serial_number[com_type]);
				}
				else
				{
					commun_write_gtin_response(CBuf, NULL, 84);	//checksum error
				}
			        serial_mode[com_type]=0;
				goto TX_PROC;

				break;
#endif	// #ifdef USE_RUSSIA_LOGISTICS_PLU#endif	// #ifdef USE_PROTOCOL_CL5000JR
#endif
			case   0:serial_command[com_type]=0;
				serial_ethtype = 0;
				serial_cmdtype[com_type]=r_data;
				switch(r_data) {
					case 'T' : serial_mode[com_type] = 1; break;
					case 'W' : serial_mode[com_type] = 30;
								break;
					case 'G' :					
					case 'N' :
					case 'R' : serial_mode[com_type] = 20; break;
					case 'c' :
					case 'C' : serial_mode[com_type] = 10; break;
					case 'I' : serial_mode[com_type] = 40; break;
					case 'j' : serial_ethtype = 1;
						   serial_mode[com_type] = 50; break;
					case 'i' : serial_mode[com_type] = 50; break;
					case 'S' : serial_mode[com_type] = 60; break;
					case '^' :
					case '*' :
					case '$' :
					case '&' :
					case '@' :
					case '?' : serial_mode[com_type] = 90; break;
					case '~' : commun_outleng(CBuf,(char *)&r_data,1);
						   break;
#ifdef USE_KOR_TRACE
					case 'P' : serial_mode[com_type] = 100; break;	// Other Protocol (ex, 저울나라프로토콜))
#endif
#ifdef USE_PROTOCOL_CHINA_DAHUA	// 중국 전용 다화 프로토콜
					case '!' : serial_mode[com_type] = 200; break;	// Dahua Protocol
#endif
#ifdef USE_HTTP_CLIENT
					case 'H' : 
						serial_mode[com_type] = 190; 
						break;	// HTTP response
#endif
					default  : serial_mode[com_type] = 0;  break;
				}
				break;
			case   1://ret=0;
				 //ret1=0;
				serial_mode[com_type]=0;
				if (r_data=='0') serial_mode[com_type]=2;
				break;
			case   2:
				status_run.run_mode=RUN_FJIG;
				sprintf(gmsg,"R0\x0a");
				commun_out(CBuf,gmsg);
				serial_mode[com_type]=0;
				break;
			case  10: //"Cx"
				 if ((r_data=='m') || (r_data=='M'))
					serial_mode[com_type] = 11;
				 else if (isdigit(r_data)) {
					serial_number[com_type]=ctoh(r_data);
					serial_mode[com_type] = 15;
				 } else
					serial_mode[com_type] = 0;
				 break;
			case  11:if ((r_data=='d') || (r_data=='D'))
					serial_mode[com_type] = 12;
				 else
					serial_mode[com_type] = 0;
				 break;
			case  12:if ((r_data == 0x0d) || (r_data==0x0a)) {
					DebugTerminal(CBuf);
					if (status_run.run_mode==RUN_RDY_CAP) display_save_restore(DSP_RESTORE);//VFD7_save_restore(VFD_RESTORE);
			         }
				 serial_mode[com_type] = 0;
				 break;
			case  15: //"Cxx"
				 if (isdigit(r_data)) {
					serial_command[com_type]<<=4;
					serial_command[com_type]|=ctoh(r_data);
				 } else {
				        serial_number[com_type]=0;
				 	switch (r_data) {
				 		case 'A': serial_mode[com_type]=16; break;
						case 'F': serial_mode[com_type]=17; break;
						case ':': serial_mode[com_type]=80; break;
						default : serial_mode[com_type]=0; break;
					}
				 }
				 break;
			case 16: //"CxxAxx<0x0a>"
				 if (r_data==0x0a) {
					commun_command(CBuf,serial_eth[com_type],serial_command[com_type],serial_number[com_type],0);
					serial_mode[com_type] =0;
					goto TX_PROC;
				 }
				 serial_number[com_type]<<=4;
				 serial_number[com_type] |=ctoh(r_data);
				 break;
			case  17: //"CxxFxx,"
				 if (r_data==',') {
					serial_fnumber[com_type]=0;
					serial_mode[com_type]=18;
					break;
				 }
				 serial_number[com_type]<<=4;
				 serial_number[com_type] |=ctoh(r_data);
				 break;
			case  18: //"CxxFxx,xx<0x0a>"
				 if (r_data==0x0a) {
				    commun_command(CBuf,serial_eth[com_type],serial_command[com_type],serial_number[com_type],serial_fnumber[com_type]);
				    serial_mode[com_type] = 0;
				    goto TX_PROC;
				 }
				 serial_fnumber[com_type]<<=4;
				 serial_fnumber[com_type] |=ctoh(r_data);
				 break;
			case  20: // "Rxx", "G", "N"
				 if (isdigit(r_data)) {
					serial_command[com_type]<<=4;
					serial_command[com_type]|=ctoh(r_data);
				 } else {
				        serial_number[com_type]=0;
				 	switch (r_data) {
#ifdef USE_PROTOCOL_CL5000JR
						case DATA_PLU:
							  serial_bcc[com_type]  =0;
							  serial_mode[com_type]=120;
							  serial_param_len[com_type] = 0;
							  break;
#ifdef USE_RUSSIA_LOGISTICS_PLU
						case DATA_LOGISTICS_PLU: // 0x31('1')
							  serial_bcc[com_type]	=0;
							  serial_mode[com_type]=125;
							  serial_param_len[com_type] = 0;
							  break;
						case DATA_LOGISTICS_INGREDIENT: // 0x32('2') for Direct Ingredient
#endif
						case DATA_BARCODE:
						case DATA_SCMSG:
						case DATA_KEY:
						case DATA_IMESSAGE:
						case DATA_STATUS:
						case DATA_GROUP:
						case DATA_SHOP:
						case DATA_DATE:
						//case DATA_VERSION:	// 추후 요청시 version 요청 추가
						case DATA_PLU2:/*small L*/
						case DATA_ACCOUNT:
                        case DATA_REMOTE_DISP:
						case DATA_RTT:
							  serial_mode[com_type]=0;
							  RewindRxBuf(CBuf, 2);
					          CL5000JRCommInterpreter(CBuf);
							  isz = q;
							  break;
#endif
						case ':': serial_mode[com_type]=80; break;
				 		case 'A': serial_mode[com_type]=21; break;
						case 'F': serial_mode[com_type]=23; break;
//						case 'M': serial_mode[com_type]=27; break;		//110624 removed by KKH (미사용으로 추정)

						default : serial_mode[com_type]=0; break;
					}
				 }
				 break;
			case  21: //"RxxAxx<0x0a>"
				 if (r_data==0x0a) {  // R<#>A[nnnn]<0x0a>
					commun_read(CBuf,eth,serial_command[com_type],serial_number[com_type],0);
					serial_mode[com_type]=0;
					goto TX_PROC;
				 }
				 serial_number[com_type]<<=4;
				 serial_number[com_type] |=ctoh(r_data);
				 break;
//			case  27:				//110624 removed by KKH (미사용으로 추정)
			case  23: //"RxxFxxxx,"
				 if (r_data==',') {
					serial_fnumber[com_type]=0;
					serial_mode[com_type]++;
					break;
				 }
				 serial_number[com_type]<<=4;
				 serial_number[com_type] |=ctoh(r_data);
				 break;
//			case  28:				//110624 removed by KKH (미사용으로 추정)
			case  24: //"RxxFxxxx,xx<0x0a>"
				 if (r_data==0x0a) {
					if (serial_mode[com_type]==24) {
						commun_read(CBuf,serial_eth[com_type],serial_command[com_type],serial_number[com_type],serial_fnumber[com_type]);
					}
					serial_mode[com_type] = 0;
					goto TX_PROC;
				 }
				 serial_fnumber[com_type]<<=4;
				 serial_fnumber[com_type] |=ctoh(r_data);
				 break;
			case 50: //"ixx" or "jxx"
			case 30: //"Wxx"
				if (isdigit(r_data)) {
					serial_command[com_type]<<=4;
					serial_command[com_type]|=ctoh(r_data);
				}
				else
				{
			 		switch(r_data)
		 			{
#ifdef USE_PROTOCOL_CL5000JR
		 				case DATA_PLU:
							serial_bcc[com_type]  =0;
							serial_mode[com_type]=110;
							serial_param_len[com_type] = 0;
							break;
#ifdef USE_RUSSIA_LOGISTICS_PLU
						case DATA_LOGISTICS_PLU:
							serial_bcc[com_type]  =0;
							serial_mode[com_type]=115;
							serial_param_len[com_type] = 0;
							break;
						case DATA_LOGISTICS_INGREDIENT:
#endif	// #ifdef USE_RUSSIA_LOGISTICS_PLU
						case DATA_BARCODE:
						case DATA_SCMSG:
						case DATA_KEY:
						case DATA_IMESSAGE:
						case DATA_STATUS:
						case DATA_GROUP:
						case DATA_SHOP:
						case DATA_DATE:
						//case DATA_VERSION:	// 추후 요청시 version 요청 추가
						case DATA_PLU2:/*small L*/
						case DATA_BOOTH:
						case DATA_MEAT_TRACE:
						case DATA_VEGE_TRACE:
						case DATA_ACCOUNT:
                        case DATA_REMOTE_KEY:
						case DATA_LABEL_CL55:
						case DATA_BITMAP:
							serial_mode[com_type]=0;
							RewindRxBuf(CBuf, 2);
							CL5000JRCommInterpreter(CBuf);
							isz = q;
							break;
#endif	// #ifdef USE_PROTOCOL_CL5000JR
						case ':':
							serial_number[com_type]=0;
							serial_mode[com_type]=80;
							break;
						case 'A':
							serial_number[com_type]=0;
							serial_mode[com_type]+=2;   // 32, 52
							break;
						case 'F':
							serial_number[com_type]=0;
							serial_mode[com_type]+=6;   // 36,56
							break;
						default:
							serial_mode[com_type]=0;
							break;
		 			}
			 	}
				break;
			case  32: //"WxxAxx,"
			case  56: //"ixxFxx," or "jxxFxx"
			case  36: //"WxxFxx,"
				 if (r_data==',') {
					serial_fnumber[com_type]=0;
					serial_mode[com_type]++;
					break;
				 }
				 serial_number[com_type]<<=4;
				 serial_number[com_type] |=ctoh(r_data);
				 break;
			case 53:
			case 57: //"ixxFxx,xxL" or "ixxFxx,xxN" or "jxxFxx,xxL" or "jxxFxx,xxN"
			case 33: //"WxxAxx,xxL"
			case 37: //"WxxFxx,xxL"
				 if (r_data=='L') {
					serial_mode[com_type]++;
					serial_length[com_type]=0;
					break;
				 } else if (r_data=='N' && serial_mode[com_type] == 57) {
					serial_mode[com_type] = 158;
					serial_length[com_type]=0;
					break;
				 }
				 serial_fnumber[com_type]<<=4;
				 serial_fnumber[com_type] |=ctoh(r_data);
				 break;
			case 158: //"ixxFxx,xxNxx:" or "jxxFxx,xxNxx:" or "ixxF20,03N<0x00>"
				if (serial_number[com_type] == 0x20  && serial_fnumber[com_type] == 0x03)
				{
					if (r_data == 0x00) network_status.send_trace_info = 1;
					else                network_status.send_trace_info = 2;
					serial_mode[com_type] = 0;
				}
#ifdef USE_IMPORT_MEAT_PROTOCOL
				else
				{
					if (r_data==':') {
						serial_param_nstr[com_type]=0;
						serial_bcc[com_type]  =0;
						serial_mode[com_type]++;
						if (serial_length[com_type] >= MAX_SERIAL_BUFFER_LEN)
						{
//							if (serial_mode[com_type]/10==3) {
//								commun_write_err16(CBuf,serial_command[com_type],0x97,1);	//length error
//							} else {
//								commun_write_err_info(CBuf,serial_command[com_type],serial_number[com_type],0x97);
//							}
							command_import_meat_send(CBuf, 0x02, 2);	//length error
							serial_mode[com_type] = 0;
							goto TX_PROC;
						}
						break;
					 }
					 serial_length[com_type] *= 10;
					 serial_length[com_type] +=ctod(r_data);
				}
#endif
				break;
			case  54:
			case  58: //"ixxFxx,xxLxx:" or "jxxFxx,xxLxx:"
			case  34: //"WxxAxx,xxLxx:"
			case  38: //"WxxFxx,xxLxx:"
				 if (r_data==':') {
					serial_param_nstr[com_type]=0;
					serial_bcc[com_type]  =0;
					serial_mode[com_type]++;
					if (serial_length[com_type] >= MAX_SERIAL_BUFFER_LEN)
					{
						if (serial_mode[com_type]/10==3) {
							commun_write_err16(CBuf,serial_command[com_type],0x97,1,OFF);	//length error
						} else {
							commun_write_err_info(CBuf,serial_command[com_type],serial_number[com_type],0x97);
						}
						serial_mode[com_type] = 0;
						goto TX_PROC;
					}
					break;
				 }
				 serial_length[com_type] <<=4;
				 serial_length[com_type] |=ctoh(r_data);
				 break;
			case  55:
			case  59: //"ixxFxx,xxLxx:<data><bcc>" or "jxxFxx,xxLxx:<data><bcc>"
			case  35: //"WxxAxx,xxLxx:<data><bcc>"
			case  39: //"WxxFxx,xxLxx:<data><bcc>"
			case  159: //"ixxFxx,xxNxx:<data><bcc>" or "jxxFxx,xxNxx:<data><bcc>"
			REPT_1:	 if (serial_param_nstr[com_type]==serial_length[com_type]) {
				 	serial_mode[com_type]=serial_mode[com_type]/10;
					serial_buffer[serial_param_nstr[com_type]]=0;
				        if (serial_mode[com_type]==3) {
					    if (r_data!=serial_bcc[com_type]) {
						commun_write_err16(CBuf,serial_command[com_type],0xfe,1,OFF);
					    } else {
						commun_write(CBuf,serial_command[com_type],
							serial_number[com_type],serial_fnumber[com_type],
							(INT32U)serial_param_nstr[com_type],(INT8U *)serial_buffer);
					    }
				        } else {
					    if (r_data!=serial_bcc[com_type]) {
#ifdef USE_IMPORT_MEAT_PROTOCOL
						if (serial_mode[com_type]==15)
							command_import_meat_send(CBuf, 0x02, 1);	//Checksum error
						else
#endif
							commun_write_err_info(CBuf,serial_command[com_type],serial_number[com_type],0xfe);
					    } else {
						commun_setinfo(CBuf,serial_command[com_type],(INT16U)serial_number[com_type],
							serial_fnumber[com_type],(INT32U)serial_param_nstr[com_type],(INT8U *)serial_buffer);
					    }
				        }
				        serial_mode[com_type]=0;
					goto TX_PROC;
				 } else {
					serial_buffer[serial_param_nstr[com_type]++]=r_data;
					serial_bcc[com_type]^=r_data;
					if (serial_param_nstr[com_type] >= MAX_SERIAL_BUFFER_LEN)
					{
						commun_write_response(CBuf, NULL, 84);	//length error
						serial_mode[com_type] = 0;
						goto TX_PROC;
					}
				 }
				 if (q<isz) {
					r_data = GetCharRxBuf(CBuf); q++;
					goto REPT_1;
				 }
				 break;
			case  40: //"IxxA" or "IxxF"
				 if (isdigit(r_data)) {
					serial_command[com_type]<<=4;
					serial_command[com_type]|=ctoh(r_data);
				 } else {
					serial_number[com_type]=0;
				 	switch (r_data) { // I00A00
				 		case 'A': serial_mode[com_type]=41; break;
						case 'F': serial_mode[com_type]=43; break;
						default : serial_mode[com_type]=0; break;
					}
				 }
				 break;
			case  41: //"IxxAxx<0x0a>"
				 if (r_data==0x0a) { // I00A00<0x0a>
					commun_getinfo(CBuf,serial_command[com_type],serial_number[com_type],0);
					serial_mode[com_type]=0;
					goto TX_PROC;
				 }
				 serial_number[com_type]<<=4;
				 serial_number[com_type] |=ctoh(r_data);
				 break;
			case  43: //"IxxFxx,"
				 if (r_data==',') {  // I00F00,00<0x0a>
					serial_fnumber[com_type]=0;
					serial_mode[com_type]++;
					break;
				 }
				 serial_number[com_type]<<=4;
				 serial_number[com_type] |=ctoh(r_data);
				 break;
			case  44: //"IxxFxx,xx<0x0a>"
				 if (r_data==0x0a) { // I00F00,00
					commun_getinfo(CBuf,serial_command[com_type],serial_number[com_type],serial_fnumber[com_type]);
					serial_mode[com_type] = 0;
					goto TX_PROC;
				 }
				 serial_fnumber[com_type]<<=4;
				 serial_fnumber[com_type] |=ctoh(r_data);
				 break;
			case  60:// 'S<0x04>';
				 serial_mode[com_type]=0;
				 switch (r_data) {
				 	case 0x04: serial_bcc[com_type]=r_data;
						   serial_param_nstr[com_type]=0;
				 	           serial_mode[com_type]=61; break; // Setting Data
					case 0x08: serial_mode[com_type]=71; break; // Setting Data
				 }
				 break;
			case  61:if (serial_param_nstr[com_type]==35) {
					if (r_data==serial_bcc[com_type]) {
						//HYP 20041213 needs replace struct wlan card
						memcpy((INT8U *)&ecard,(INT8U *)serial_buffer,35);

					}
					serial_mode[com_type]=0;
				 } else {
					serial_bcc[com_type]^=r_data;
					serial_buffer[serial_param_nstr[com_type]++]=r_data;
					if (serial_param_nstr[com_type] >= MAX_SERIAL_BUFFER_LEN)
					{
						commun_write_response(CBuf, NULL, 84);	//length error
						serial_mode[com_type] = 0;
						goto TX_PROC;
					}
				 }
				 break;
			case  71:serial_mode[com_type]=0;
				 break;
			case  80: //"Cxxx:E" or "Cxxx:O" or "Wxx:E" or "Wxx:O"
				 serial_errtype[com_type]=r_data;
				 serial_mode[com_type]+=1;
				 break;
			case  81: //"Cxxx:<E|O>xx<0x0a>" or "Wxx:<E|O>xx<0x0a>"
				 if (r_data==0x0a) {
					commun_return(serial_command[com_type],(INT16U)serial_number[com_type]);
					serial_mode[com_type] = 0;
				 } else {
					serial_number[com_type]<<=4;
					serial_number[com_type] |=ctoh(r_data);
				 }
				 break;
			case 90:serial_number[com_type]=0;
				serial_mode[com_type]=0;
				if (r_data=='=') serial_mode[com_type] = 91;
				break;
			case 91:if ((r_data=='.') || (r_data==0x0a)) {
					commun_common_ethernet(serial_cmdtype[com_type],&serial_eth[com_type],serial_number[com_type]);
					serial_mode[com_type]=0;
				} else {
					serial_number[com_type]<<=4;
					serial_number[com_type] |=ctoh(r_data);
				}
				break;
#ifdef USE_KOR_TRACE
			case 100:
				if (isdigit(r_data)) {
					serial_command[com_type]<<=4;
					serial_command[com_type]|=ctoh(r_data);
				} else {
					serial_length[com_type]=0;
					if (r_data == 'L') { 
						serial_mode[com_type]=101;
					} else {
						serial_mode[com_type]=0;
					}
				}
				break;
			case 101:
				if (r_data == ':') {
					serial_param_nstr[com_type]=0;
					serial_bcc[com_type]  =0;
					serial_mode[com_type]=102;
					if (serial_length[com_type] >= MAX_SERIAL_BUFFER_LEN) 
					{
						serial_mode[com_type] = 0;
						goto TX_PROC;
					}
				} else {
					serial_length[com_type]<<=4;
					serial_length[com_type]|=ctoh(r_data);
				}
				break;
			case 102:
			REPT_2:  if (serial_param_nstr[com_type]==serial_length[com_type]) {
					if (r_data == serial_bcc[com_type]) {
						switch (serial_command[com_type]) {
							case 1 :
								kortrace_com_receive(CBuf, serial_buffer);
								break;
							case 0x50:		//POS 연동 프로토콜
								break;
							default :
								break;
						}
					} else {
						// Error
						//kortrace_com_send_ack(CBuf, 0x0000, KORTR_RCV_NAK, 0x0002);	// 체크섬오류
					}
						serial_mode[com_type]=0;
					goto TX_PROC;
				 } else {
					serial_buffer[serial_param_nstr[com_type]++]=r_data;
					serial_bcc[com_type]^=r_data;
					if (serial_param_nstr[com_type] >= MAX_SERIAL_BUFFER_LEN) 
					{
						commun_write_response(CBuf, NULL, 84);	//length error
						serial_mode[com_type] = 0;
						goto TX_PROC;
					}
				 }
				 if (q<isz) {
					r_data = GetCharRxBuf(CBuf); q++;
					goto REPT_2;
				 }

				 break;
#endif
#ifdef USE_HTTP_CLIENT
			case 190: //"H"
				if(r_data=='T')serial_mode[com_type]++;
				else serial_mode[com_type]=0;
				break;
			case 191: //"HT"
				if(r_data=='T')serial_mode[com_type]++;
				else serial_mode[com_type]=0;
				break;
			case 192: //"HTTP"
				if(r_data=='P')
				{
					http_ok = commun_gethttp(CBuf);
					if(http_ok)
					{
						network_status.send_trans_sale = 1;
						network_status.rcv_tail = network_status.send_tail;
					}
				}
				serial_mode[com_type]=0;
				q = isz;				
				break;
#endif
#ifdef USE_PROTOCOL_CHINA_DAHUA	// 중국 전용 다화 프로토콜
			case 200: //"!xx"
				if(r_data=='0')
				{
					serial_mode[com_type]=201;
					serial_command[com_type] = r_data;
				}
				break;
			case 201: //"!0"
				if(r_data=='W') // Write PLU
				{
					serial_mode[com_type]=202;
					memset((INT8U*)&temp_dahua_plu, 0, sizeof(temp_dahua_plu));
				break;
				}
				else if(r_data=='V') // Write PLU TMA07
				{
					serial_mode[com_type]=222;
					memset((INT8U*)&temp_dahua_plu, 0, sizeof(temp_dahua_plu));
						break;
				}
						break;
			case 202: //"!0WxxxxA" PLU num
				if(r_data=='A')
				{
					serial_mode[com_type]=203;
					break;
				}
				else
				{
					temp_dahua_plu.PLUnum = temp_dahua_plu.PLUnum * 10;
					temp_dahua_plu.PLUnum += ctod(r_data);
				}
				break;
			case 222: //"!0VxxxxA" PLU num
				if(r_data=='A')
				{
					serial_mode[com_type]=223;
					serial_length[com_type] = 0;
					break;
				}
				else
				{
					temp_dahua_plu.PLUnum = temp_dahua_plu.PLUnum * 10;
					temp_dahua_plu.PLUnum += ctod(r_data);
				}
				serial_length[com_type]++;
				break;

			case 223: //"!0V itemcode
				if(serial_length[com_type]==6)
				{
					serial_mode[com_type]=224;
					serial_length[com_type] = 0;
					temp_dahua_plu.itemcode = temp_dahua_plu.itemcode * 10;
					temp_dahua_plu.itemcode += ctod(r_data);
					break;
				}
				else
				{
					temp_dahua_plu.itemcode = temp_dahua_plu.itemcode * 10;
					temp_dahua_plu.itemcode += ctod(r_data);
				}
				serial_length[com_type]++;
				break;

			case 224: //"!0VxxxxA" Unit Price
				if(serial_length[com_type]==5)
				{
					serial_mode[com_type]=225;
					serial_length[com_type] = 0;
					temp_dahua_plu.unitprice = temp_dahua_plu.unitprice * 10;
					temp_dahua_plu.unitprice += ctod(r_data);
					break;
				}
				else
				{
					temp_dahua_plu.unitprice = temp_dahua_plu.unitprice * 10;
					temp_dahua_plu.unitprice += ctod(r_data);
				}
				serial_length[com_type]++;
				break;

			case 225: //"!0VxxxxA" PLU type
				serial_mode[com_type]=226;
				serial_length[com_type] = 0;
				temp_dahua_plu.unit += ctod(r_data);
				break;

			case 226: //"!0VxxxxA" emoty
				if(serial_length[com_type]==5)
				{
					serial_mode[com_type]=227;
					serial_length[com_type] = 0;
					break;
				}
				serial_length[com_type]++;
				break;

			case 227: //"!0VxxxxA" validdate
				if(serial_length[com_type]==2)
				{
					serial_mode[com_type]=228;
					serial_length[com_type] = 0;
					temp_dahua_plu.validdate = temp_dahua_plu.validdate * 10;
					temp_dahua_plu.validdate += ctod(r_data);
					break;
				}
				else
				{
					temp_dahua_plu.validdate = temp_dahua_plu.validdate * 10;
					temp_dahua_plu.validdate += ctod(r_data);
				}
				serial_length[com_type]++;
				break;

			case 228: //"!0VxxxxA" emoty
				if(serial_length[com_type]==16)
				{
					serial_mode[com_type]=229;
					serial_length[com_type] = 0;
					break;
				}
				serial_length[com_type]++;
				break;


			case 229: //"!0VxxxxA" tare
				if(serial_length[com_type]==4)
				{
					serial_mode[com_type]=230;
					serial_length[com_type] = 0;
					temp_dahua_plu.tarevalue= temp_dahua_plu.tarevalue * 10;
					temp_dahua_plu.tarevalue += ctod(r_data);
					break;
				}
				else
				{
					temp_dahua_plu.tarevalue= temp_dahua_plu.tarevalue * 10;
					temp_dahua_plu.tarevalue += ctod(r_data);
				}
				serial_length[com_type]++;
				break;

			case 230: //"!0WXXXXAXXXXXXB" PLU itemcode
				if(r_data=='B')
				{
					serial_mode[com_type]=231;
					serial_length[com_type] = 0;
					break;
				}
				break;

			case 231: //"!0WXXXXAXXXXXXB" PLU itemcode
				if(r_data=='C')
				{
					serial_mode[com_type]=232;
					serial_length[com_type] = 0;
					break;
				}
				if(serial_length[com_type]%2 == 0)
				{
					temp_dahua_plu.PLUname[serial_length[com_type]/2] = ctod(r_data);
				}
				else
				{
					temp_dahua_plu.PLUname[serial_length[com_type]/2] = temp_dahua_plu.PLUname[serial_length[com_type]/2] * 10;
					temp_dahua_plu.PLUname[serial_length[com_type]/2] += ctod(r_data);
				}
				serial_length[com_type]++;
				break;

			case 232: //"!0WXXXXAXXXXXXB" PLU itemcode
				if(r_data=='D')
				{
					serial_mode[com_type]=233;
					serial_length[com_type] = 0;
					break;
				}
				if(serial_length[com_type]%2 == 0)
				{
					temp_dahua_plu.PLUname2[serial_length[com_type]/2] = ctod(r_data);
				}
				else
				{
					temp_dahua_plu.PLUname2[serial_length[com_type]/2] = temp_dahua_plu.PLUname2[serial_length[com_type]/2] * 10;
					temp_dahua_plu.PLUname2[serial_length[com_type]/2] += ctod(r_data);
				}
				serial_length[com_type]++;
				break;

			case 233: //"!0WXXXXAXXXXXXB" PLU itemcode
				if(r_data=='E')
				{
					serial_mode[com_type]=234;
					serial_length[com_type] = 0;
					break;
				}
				if(serial_length[com_type]%2 == 0)
				{
					temp_dahua_plu.PLUname3[serial_length[com_type]/2] = ctod(r_data);
				}
				else
				{
					temp_dahua_plu.PLUname3[serial_length[com_type]/2] = temp_dahua_plu.PLUname3[serial_length[com_type]/2] * 10;
					temp_dahua_plu.PLUname3[serial_length[com_type]/2] += ctod(r_data);
				}

				break;
			case 234: //"!0WXXXXAXXXXXXB" PLU itemcode
                if(r_data==0x0d)
				{
					serial_mode[com_type]=235;
					serial_length[com_type] = 0;
				}
				break;

			case 235: //"!0WXXXXAXXXXXXB" PLU itemcode
                if(r_data==0x0a)
				{
					CHN_dhbz_plu_save(&temp_dahua_plu);
						serial_mode[com_type]=0;
					temp_ans[0]='0';
					temp_ans[1]='V';
					temp_ans[2]=(temp_dahua_plu.PLUnum/1000)+'0';
					temp_ans[3]=((temp_dahua_plu.PLUnum%1000)/100)+'0';
					temp_ans[4]=((temp_dahua_plu.PLUnum%100)/10)+'0';
					temp_ans[5]=((temp_dahua_plu.PLUnum%10))+'0';
                    temp_ans[6]='a';
                    temp_ans[7]=((temp_dahua_plu.itemcode%10000000)/1000000)+'0';
                    temp_ans[8]=0x0d;
                    temp_ans[9]=0x0a;
                    temp_ans[10]=0x03;
					commun_outleng(CBuf,(char *)temp_ans, 11);
						break;
				}
				break;
			case 203: //"!0WXXXXAXXXXXXB" PLU itemcode
				if(r_data=='B')
				{
					serial_mode[com_type]=204;
					break;
				}
				else
				{
					temp_dahua_plu.itemcode = temp_dahua_plu.itemcode * 10;
					temp_dahua_plu.itemcode += ctod(r_data);
				}
				break;
			case 204: //"!0WXXXXAXXXXXXBXXXXXXC" PLU unitprice
				if(r_data=='C')
				{
					serial_mode[com_type]=205;
					break;
				}
				else
				{
					temp_dahua_plu.unitprice = temp_dahua_plu.unitprice * 10;
					temp_dahua_plu.unitprice += ctod(r_data);
				}
				break;
			case 205: //"!0WXXXXAXXXXXXBXXXXXXCXD" PLU unit(500g/1kg)
				if(r_data=='D')
				{
					serial_mode[com_type]=206;
					break;
				}
				else
				{
					temp_dahua_plu.unit = temp_dahua_plu.unit * 10;
					temp_dahua_plu.unit += ctod(r_data);
				}
				break;

			case 206: //"!0WXXXXAXXXXXXBXXXXXXCXDXXXE" PLU validdate(3digit)
				if(r_data=='E')
				{
					serial_mode[com_type]=207;
					break;
				}
				else
				{
					temp_dahua_plu.validdate = temp_dahua_plu.validdate * 10;
					temp_dahua_plu.validdate += ctod(r_data);
				}
				break;

			case 207: //"!0WXXXXAXXXXXXBXXXXXXCXDXXXEXXF" PLU barcode index number
				if(r_data=='F')
				{
					serial_mode[com_type]=208;
					break;
				}
				else
				{
					temp_dahua_plu.barcodenum= temp_dahua_plu.barcodenum * 10;
					temp_dahua_plu.barcodenum += ctod(r_data);
				}
				break;

			case 208: //"!0WXXXXAXXXXXXBXXXXXXCXDXXXEXXFXXXXXG" PLU tare
				if(r_data=='G')
				{
					serial_mode[com_type]=209;
					break;
				}
				else
				{
					temp_dahua_plu.tarevalue= temp_dahua_plu.tarevalue * 10;
					temp_dahua_plu.tarevalue += ctod(r_data);
				}
				break;

			case 209: //"!0WXXXXAXXXXXXBXXXXXXCXDXXXEXXFXXXXXGXH" PLU special
				if(r_data=='H')
				{
					serial_mode[com_type]=210;
					serial_length[com_type] = 0;
					break;
				}
				else
				{
					temp_dahua_plu.specialnum += ctod(r_data);
				}
				break;

			case 210: // PLU name
				if(r_data=='I')
				{

					serial_mode[com_type]=211;
					serial_length[com_type] = 0;
					break;
				}
				else
				{
					if(serial_length[com_type]%2 == 0)
					{
						temp_dahua_plu.PLUname[serial_length[com_type]/2] = ctod(r_data);
					}
					else
					{
						temp_dahua_plu.PLUname[serial_length[com_type]/2] = temp_dahua_plu.PLUname[serial_length[com_type]/2] * 10;
						temp_dahua_plu.PLUname[serial_length[com_type]/2] += ctod(r_data);
					}
				}
				serial_length[com_type]++;
				break;

			case 211: // PLU ingredient
				if(r_data=='J')
				{
					// Save PLU
					CHN_dhbz_plu_save(&temp_dahua_plu);
					serial_mode[com_type]=0;
					temp_ans[0]=0x0d;
					temp_ans[1]=0x0a;
					temp_ans[2]=0x03;
					commun_outleng(CBuf,(char *)temp_ans,3);
					break;
				}
				else
				{
					if(serial_length[com_type]%2 == 0)
					{
						temp_dahua_plu.ingredient[serial_length[com_type]/2] = ctod(r_data);
					}
					else
					{
						temp_dahua_plu.ingredient[serial_length[com_type]/2] = temp_dahua_plu.ingredient[serial_length[com_type]/2] * 10;
						temp_dahua_plu.ingredient[serial_length[com_type]/2] += ctod(r_data);
					}
				}
				serial_length[com_type]++;
				break;
#endif

			default :serial_mode[com_type] = 0;
				 break;
		}
		serial_timeout_reset(com_type);
		rept=1;
		continue;
	TX_PROC:network_tx_proc();
	}
	ret=serial_timeout_check(com_type);
	if (network_status.allow_sale || flagTempAllowSale) {
		if (serial_mode[com_type]==0) goto END;
	}
	if (ret) goto END;
	if (rept) {
		if (com_type) {
			EthDataProc(CBuf);
		}
		network_tx_proc();
		goto REPT;
	}
END:
	if (first) {
		if (first==9) {
			Dsp_Diffuse();
			if (status_run.run_mode!=RUN_SALE) {
				//VFD7_save_restore(VFD_RESTORE);
				display_save_restore(DSP_RESTORE);
			}
			sale_display_update(0x0fff);
		}
		DspStruct.DirectDraw = 1;
		display_sign_noflush(SIGN_NET,0);
#ifndef USE_DISPLAY_OPT_CL5200
		VFD7_Diffuse();
#endif
	}
	DspStruct.DirectDraw = backup_directdraw;
}

#ifdef USE_HTTP_CLIENT
INT8U commun_gethttp(COMM_BUF *CBuf)
{
	INT8U flag;
	char version[10];
	char response[10];	
	INT16S   isz;
	INT8U r_data;
	INT16S   q,k;
	INT8U status;
	INT8U ret;

	isz = CheckRxBufCountInt(CBuf);	
	status = 0;
	ret = 0;
	for (q=0; q<isz; q++) 
	{
		r_data = GetCharRxBuf(CBuf);
		// Get version
		switch(status)
		{
			case 0: // check version
				version[q] =  r_data;
				if(q == 3)
				{
					flag = memcmp(version, &http_10[4],4);
					if(flag == 0) status++;
					flag = memcmp(version, &http_11[4],4);
					if(flag == 0) status++;
					if(status ==0) ret = 0;
				}
				break;
			case 1: // seperator
				if(r_data == 0x20);
				else	
				{
					k=0;
					response[k++] =  r_data;
					status++;
				}
				break;
			case 2: // check response number
				response[k++] =  r_data;
				if(k == 4)
				{
					response[k] = 0;
					flag = memcmp(response, http_200, 4);
					status++;					
					if(flag == 0) ret = 1;
					k=0;
#ifdef USE_DEBUG_SEND_TRACE_PRICE
					MsgOut(response);
#endif
				}
				break;
			case 4: // parsing end
				break;				
		}
	}
	return ret;
}

#endif
void  commun_recv_eth_header(ETH_CONNECT_LIST *eth,char *str,INT16S length)
{
	INT16S status;
	INT16S r;
	INT32U v32;
	INT8U code;

	status=0;
	r=0;
	memset((INT8U *)eth,0,sizeof(ETH_CONNECT_LIST));
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
					commun_common_ethernet(code,eth,v32);
					status=0;
				} else {
					v32<<=4;
					v32|=ctoh(str[r]);
				}
		}
		r++;
	}
END:;
}

TRANSACTION_PAY RecievedTransPay;

extern NETWORK_CLERK_SALE_AMOUNT ClerkSaleAmount;
extern INT8U ClerkTransactionSaleStore(INT8U clerkNum);

INT8U ClerkTransactionSaleStore(INT8U clerkNum)
{
	TRANSACT_CHAIN  *cp;
	TRANSACT_CHAIN  tc;
	INT16U h_index, next;
	TRANSACTION_RECORD_TYPE *pTrans;
	INT16S i=0;

	if (!(Operation.operationClerk == OPER_FLOATING && (ethernet_list.status == 1 || ethernet_list.status == 4))) return 0; // Master & Local only
	if((!ethernet_list.remoteip[0]) && (!ethernet_list.remoteip[1])) return 0;
  	if(MAX_CLERK_NO-1 < clerkNum)	return 0;
	//cp = (TRANSACT_CHAIN *)get_nvram_lparam(NVRAM_TRANS_HEAD+clerkNum*4);
	h_index = get_nvram_lparam(NVRAM_TRANS_HEAD+clerkNum*4);
	if (h_index)
	{
		next = h_index;
		get_nvram_trans_addr(NVRAM_TRANS_DATA, h_index - 1, &tc);
		cp = &tc;
	}
	else
	{
		cp = 0;
	}
	if (!cp) goto ENDDATA;
  	for(;cp;)
  	{
		if (i>=MAX_ADD_TRANSACTION) break;
		if (!cp) break;
		pTrans = &(cp->content);
		if (!GlbFlagPCnMasterConnection) pTrans->trtStatus2 |= FLAG2_TRANS_DISCON;
		transaction_sale_store(pTrans, NULL, NULL, NULL, 1, 0);
		next = cp->next;
		//cp     = cpNext;
		i++;
		if (!cp->next) break;
		get_nvram_trans_addr(NVRAM_TRANS_DATA, next - 1, &tc);
		cp = &tc;
	}
ENDDATA:
	return 1;
}

INT16U transaction_sale_make_data(char *buffer, INT8U code, INT16U head, INT16U tail, INT16U *max_cnt)
{
	INT16U cnt;
	INT32U addr;
	INT8U temp1;
	long  mul;
	TRANSACTION_RECORD_TYPE trans;
#ifdef USE_TRACE_STANDALONE
	TRANSACTION_RECORD_SUB_TYPE trans_sub;
#endif
#ifdef USE_CHN_EMART_TRACE
	TRANSACTION_RECORD_SUB_TYPE trans_sub;
#endif
	INT16S   slen,i;
	INT8U bcc;
	//INT8U tflag;
	//INT8U	chg_flag;
#ifdef USE_TRACE_STANDALONE
	INT8U lotFlag;
	INT16U plu_key;
	INT16U len;
	//INT8U len;
	//INT8U form[20];
#endif


	//chg_flag = get_global_bparam(GLOBAL_SALE_SETUP_FLAG8) & 0x40;	//parameter 723
	slen = 15;

	//Scale Capa에 따른 무게값을 "g" 단위로 전송 하고자 할때
	temp1=get_global_bparam(GLOBAL_SALE_SETUP_FLAG7);
	temp1=(temp1>>6)&0x03;
	mul = 1L;
	if(temp1 > 0) mul = power10(temp1);

	if(head >= tail) cnt = head-tail;
	else             cnt = head+Startup.max_send_buf-tail;
	if(cnt > *max_cnt) cnt = *max_cnt; //max 10개 전송
	if(cnt > 0 && UseEmartNetwork) *max_cnt = 1;

	addr=tail;
	addr%=Startup.max_send_buf;
	addr*=MAX_TRANSACTION_TYPE1_SALE_SIZE;//sizeof(TRANSACTION_RECORD_TYPE);
	addr+=Startup.addr_send_buf;

	get_nvram_sparam(addr+2,(INT8U *)&trans,sizeof(TRANSACTION_RECORD_TYPE));
	slen+= ethernet_gen_head(&buffer[slen]);
	sprintf(&buffer[slen],"C=%02X.",trans.trtOperator); slen+=5;
	sprintf(&buffer[slen],"T=%04X.",tail+cnt);  	    slen+=7; // 현재 tail에 전송할 tr수를 더한 값. buffer 개수보다 클 수 있다.
	if (*max_cnt != 1 && UseEmartNetwork == 0)
	{
		sprintf(&buffer[slen],"N=%02X.",cnt);               slen+=5; //transaction 개수
	}

	for (i = 0; i < cnt; i++)
	{
		addr=tail+i;
		addr%=Startup.max_send_buf;
		addr*=MAX_TRANSACTION_TYPE1_SALE_SIZE;//sizeof(TRANSACTION_RECORD_TYPE);
		addr+=Startup.addr_send_buf;

		get_nvram_sparam(addr+2,(INT8U *)&trans,sizeof(TRANSACTION_RECORD_TYPE));
		trans.trtWeight *= mul;
#if defined(USE_CHN_CART_SCALE) || defined(USE_CHONGQING_FUNCTION)
		// 중국 대차스케일은 Trace Code(20bytes)를 저장하나 통신으로 전송하지는 않음(기존 프로토콜 유지)
		memcpy(&buffer[slen],&trans, sizeof(TRANSACTION_RECORD_TYPE)-TRACE_CODE_SIZE);
		slen+=(sizeof(TRANSACTION_RECORD_TYPE)-TRACE_CODE_SIZE);
#else
		memcpy(&buffer[slen],&trans,sizeof(TRANSACTION_RECORD_TYPE));
		slen+=sizeof(TRANSACTION_RECORD_TYPE);
#endif

		if (code == 0x53)	// type 5
		{
#ifdef USE_TRACE_STANDALONE
			plu_table_search(trans.trtDept, trans.trtPLU, &plu_key, 0);
			memset(&buffer[slen], 0, 55);
			plu_get_field(plu_key-1, PTYPE_PLU_NAME1, (INT8U *)&buffer[slen], (INT16S *)&len, 55); // name1
			buffer[slen+len] = 0;
			slen += 55;
#else
			code = 0x03;
#endif
		}

		if (code == 0x23 || code == 0x53)
		{
#ifdef USE_TRACE_STANDALONE
			get_nvram_sparam(addr+2+MAX_TRANSACTION_STRUCT_SIZE, (INT8U *)&trans_sub, sizeof(TRANSACTION_RECORD_SUB_TYPE));
			//lotFlag = trans_sub.indivLen&0x80;
			lotFlag = trans.trtIndividualIndex&0x80;
			if (lotFlag) code++;	// lot 판매 command : 0x24, 0x54
			memcpy(&buffer[slen], trans_sub.indivNo, INDIVIDUAL_NO_LEN_TRANS);
			slen+=INDIVIDUAL_NO_LEN_TRANS;
#elif defined (USE_CHN_EMART_TRACE)
			get_nvram_sparam(addr+2+MAX_TRANSACTION_STRUCT_SIZE, (INT8U *)&trans_sub, sizeof(TRANSACTION_RECORD_SUB_TYPE));
			memcpy(&buffer[slen], trans_sub.indivNo, sizeof(trans_sub.indivNo));
			slen+=sizeof(trans_sub.indivNo);
#else
			code = 0x03;
#endif
		}
	}

	sprintf(buffer,"i00F070,%02XL%03X", code, slen-15);

	buffer[14]=':';
	bcc=gen_bcc((INT8U *)&buffer[15],slen-15);
	buffer[slen++]=bcc;
	*max_cnt = cnt;

	return slen;
}

#ifdef USE_HTTP_CLIENT
INT16U make_http_header(char *buffer, char *datastr, INT16U datastr_length)
{
	INT16S   slen;
	INT8U   use_DNS;
	INT8U   URL[60];		
	INT8U   remote_ip[60];			
	INT16U   remote_port;
	use_DNS=get_net_bparam(NETWORK_LOCAL_ETHERNET_FLAG) & 0x02;
	// GET
	strcpy(buffer,http_get);
	slen = strlen(http_get);
	memcpy(&buffer[slen], datastr, datastr_length);
	slen += datastr_length;
	buffer[slen++] = 0x20;
	strcpy(&buffer[slen],http_10);
	slen += strlen(http_10);
	strcpy(&buffer[slen],http_crnl);
	slen += strlen(http_crnl);

	// Host
	strcpy(&buffer[slen],http_host);
	slen += strlen(http_host);
	if(use_DNS)
	{

		memset(URL,0,60);
		get_dns_sparam(NETWORK_REMOTE_URL, (INT8U *)URL, NETWORK_REMOTE_URL_MAX_SIZE);
		URL[NETWORK_REMOTE_URL_MAX_SIZE-1]=0;
		strcpy(&buffer[slen],(char*)URL);
		slen += strlen((char*)URL); 
	}
	else
	{
		get_net_sparam(NETWORK_MACHINE_REMOTE_IP, remote_ip, 4);
		remote_port = get_net_wparam(NETWORK_MACHINE_REMOTE_TCPPORT);		
		sprintf(&buffer[slen], "%d.%d.%d.%d:%ld", remote_ip[0], remote_ip[1], remote_ip[2], remote_ip[3], remote_port);
		slen +=  ilog(remote_ip[0]) + ilog(remote_ip[1]) + ilog(remote_ip[2]) + ilog(remote_ip[3]) + ilog(remote_port) + 4;
	}
	strcpy(&buffer[slen],http_crnl);
	slen += strlen(http_crnl);

	// Connection
//	strcpy(&buffer[slen],"Connection: close");
//	slen += strlen("Connection: close"); 
//	strcpy(&buffer[slen],http_crnl);
//	slen += strlen(http_crnl);

//	// Cache
//	strcpy(&buffer[slen],"Cache-Control: max-age=0");
//	slen += strlen("Cache-Control: max-age=0"); 
//	strcpy(&buffer[slen],http_crnl);
//	slen += strlen(http_crnl);

	// Request
//	strcpy(&buffer[slen],"Upgrade-Insecure-Requests: 1");
//	slen += strlen("Upgrade-Insecure-Requests: 1"); 
//	strcpy(&buffer[slen],http_crnl);
//	slen += strlen(http_crnl);

	// Accept
//	strcpy(&buffer[slen],"Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8");
//	slen += strlen("Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8"); 
//	strcpy(&buffer[slen],http_crnl);
//	slen += strlen(http_crnl);

	// language
//	strcpy(&buffer[slen],"Accept-Language: ko-KR,ko;q=0.9,en-US;q=0.8,en;q=0.7");
//	slen += strlen("Accept-Language: ko-KR,ko;q=0.9,en-US;q=0.8,en;q=0.7"); 
//	strcpy(&buffer[slen],http_crnl);
//	slen += strlen(http_crnl);

	// end Header
	strcpy(&buffer[slen],http_crnl);
	slen += strlen(http_crnl);
#ifdef USE_DEBUG_SEND_TRACE_PRICE
	MsgOut(buffer);
#endif
	return slen;
}
INT16U make_http_data(char *buffer, char *addr, INT16U addr_length, char *data, INT16U data_length)
{
	INT16S   slen;
    slen = make_http_header(buffer, addr, addr_length);

	memcpy(&buffer[slen], data, data_length);
	slen += data_length;
	return slen;
}
#endif

#ifdef USE_REALTIME_TRANS_TYPE3
INT16U transaction_sale_make_data_type3(char *buffer, INT8U code, INT16U head, INT16U tail, INT16U *max_cnt)
{
	INT16U cnt;
	INT32U addr;
	INT8U temp1;
	long  mul;
	REALTIME_TRANSACTION_TYPE  trData;
	INT16S   slen;
	INT8U bcc;

	if (GlbOper.realtimeTransactionType != 3) return;

	slen = 15;

	//Scale Capa에 따른 무게값을 "g" 단위로 전송 하고자 할때
	temp1=get_global_bparam(GLOBAL_SALE_SETUP_FLAG7);
	temp1=(temp1>>6)&0x03;
	mul = 1L;
	if(temp1 > 0) mul = power10(temp1);

	cnt = 1;

	addr=tail;
	addr*=MAX_TRANSACTION_REALTIME_SALE_SIZE;//sizeof(REALTIME_TRANSACTION_TYPE);
	addr+=Startup.addr_send_buf;

	get_nvram_sparam(addr,(INT8U *)&trData,sizeof(REALTIME_TRANSACTION_TYPE));
	trData.contents.trtWeight *= mul;

	slen+= ethernet_gen_head(&buffer[slen]);
	sprintf(&buffer[slen],"C=%02X.",trData.contents.trtOperator); slen+=5;
	sprintf(&buffer[slen],"T=%04X.",tail+1);
	slen+=7;

	memcpy(&buffer[slen],&trData,sizeof(REALTIME_TRANSACTION_TYPE));
	slen+=sizeof(REALTIME_TRANSACTION_TYPE);

	sprintf(buffer, "i00F070,%02XL%03X", code, slen-15);

	buffer[14]=':';
	bcc=gen_bcc((INT8U *)&buffer[15],slen-15);
	buffer[slen++]=bcc;
	*max_cnt = cnt;

	return slen;
}
#endif

#ifndef USE_INDIA_FUNCTION
INT16U transaction_sale_make_data_type4(char *buffer, INT8U code, INT16U head, INT16U tail, INT16U *max_cnt)
{
	INT16U cnt;
	INT32U addr;
	INT8U temp1;
	long  mul;
	TRANSACTION_SALE_DATA_TYPE trans_sdt;
	TRANSACTION_CHANGE_DATA_TYPE trans_cdt;
	INT16S   slen;
	INT8U bcc;
	INT8U tflag;
	INT16U plu_key;
	INT16U len;

	slen = 15;

	//Scale Capa에 따른 무게값을 "g" 단위로 전송 하고자 할때
	temp1=get_global_bparam(GLOBAL_SALE_SETUP_FLAG7);
	temp1=(temp1>>6)&0x03;
	mul = 1L;
	if(temp1 > 0) mul = power10(temp1);

	cnt = 1;

	addr=tail;
	addr%=Startup.max_send_buf;
	addr*=MAX_TRANSACTION_TYPE4_SALE_SIZE;//sizeof(TRANSACTION_RECORD_TYPE);
	addr+=Startup.addr_send_buf;

	slen+= ethernet_gen_head(&buffer[slen]);
	sprintf(&buffer[slen],"T=%04X.",tail+cnt);  	    slen+=7; // 현재 tail에 전송할 tr수를 더한 값. buffer 개수보다 클 수 있다.

	//tflag, 0:transaction, 0x80: change unit price
	tflag = get_nvram_bparam(addr);
	if (tflag&0x80)
	{
		get_nvram_sparam(addr,(INT8U *)&trans_cdt,sizeof(TRANSACTION_CHANGE_DATA_TYPE));
		memcpy(&buffer[slen],&trans_cdt,sizeof(TRANSACTION_CHANGE_DATA_TYPE));
		slen+=sizeof(TRANSACTION_CHANGE_DATA_TYPE);
		plu_table_search(trans_cdt.cdtDept, trans_cdt.cdtPLUNo, &plu_key,0);
		memset(&buffer[slen], 0, 55);
		plu_get_field(plu_key-1, PTYPE_PLU_NAME1, (INT8U *)&buffer[slen], (INT16S *)&len, 55); // name1
		buffer[slen+len] = 0;
		slen += 55;
		sprintf(buffer,"i00F080,%02XL%03X", code, slen-15);
	}
	else
	{
		get_nvram_sparam(addr,(INT8U *)&trans_sdt,sizeof(TRANSACTION_SALE_DATA_TYPE));
		trans_sdt.sdtWeight *= mul;
		memcpy(&buffer[slen],&trans_sdt,sizeof(TRANSACTION_SALE_DATA_TYPE));
		slen+=sizeof(TRANSACTION_SALE_DATA_TYPE);
		plu_table_search(trans_sdt.sdtDept, trans_sdt.sdtPLUNo, &plu_key,0);
		memset(&buffer[slen], 0, 55);
		plu_get_field(plu_key-1, PTYPE_PLU_NAME1, (INT8U *)&buffer[slen], (INT16S *)&len, 55); // name1
		buffer[slen+len] = 0;
		slen += 55;
		sprintf(buffer,"i00F070,%02XL%03X", code, slen-15);
	}
	buffer[14]=':';
	bcc=gen_bcc((INT8U *)&buffer[15],slen-15);
	buffer[slen++]=bcc;
	*max_cnt = cnt;

	return slen;
}
#endif

#ifdef USE_IMPORT_MEAT_PROTOCOL
INT16U transaction_sale_make_data_type6(char *buffer, INT8U code, INT16U tail, INT16U *max_cnt)
{
	INT16U cnt;
	INT32U addr;
	INT8U temp1;
	long  mul;
	TRANSACTION_SALE_DATA_TYPE trans_sdt;
	INT16S   slen;
	INT8U bcc;
	INT16U plu_key;
	INT16U len;
	INT8U plu_name[54];

	slen = 15;

	//Scale Capa에 따른 무게값을 "g" 단위로 전송 하고자 할때
	temp1=get_global_bparam(GLOBAL_SALE_SETUP_FLAG7);
	temp1=(temp1>>6)&0x03;
	mul = 1L;
	if(temp1 > 0) mul = power10(temp1);

	cnt = 1;
	addr=tail;
	addr%=Startup.max_send_buf;
	addr*=MAX_TRANSACTION_TYPE4_SALE_SIZE;
	addr+=Startup.addr_send_buf;

	get_nvram_sparam(addr,(INT8U *)&trans_sdt,sizeof(TRANSACTION_SALE_DATA_TYPE));
	trans_sdt.sdtWeight *= mul;

	plu_table_search(trans_sdt.sdtDept, trans_sdt.sdtPLUNo, &plu_key, 0);
	plu_get_field(plu_key-1, PTYPE_PLU_NAME1, (INT8U *)&plu_name, (INT16S *)&len, sizeof(plu_name)); // name1
	plu_name[sizeof(plu_name)-1] = 0;

	slen+= ethernet_gen_head(&buffer[slen]);

	if((trans_sdt.sdtStatus & FLAG_TRANS_NEGATIVE) || (trans_sdt.sdtStatus & FLAG_TRANS_RETURN))
	{
		sprintf(&buffer[slen],"D|");
		slen+=strlen(&buffer[slen]);
	}
	else
	{
		sprintf(&buffer[slen],"A|");
		slen+=strlen(&buffer[slen]);
	}
	sprintf(&buffer[slen],"%06ld|",trans_sdt.sdtPLUNo);
	slen+=strlen(&buffer[slen]);
	sprintf(&buffer[slen],"%06ld|",trans_sdt.sdtItemCode);
	slen+=strlen(&buffer[slen]);
	sprintf(&buffer[slen],"BF%03d|",trans_sdt.Reserved);	//meatUse
	slen+=strlen(&buffer[slen]);
	sprintf(&buffer[slen],"20%02d%02d%02d|",trans_sdt.sdtDate[0],trans_sdt.sdtDate[1],trans_sdt.sdtDate[2]);
	slen+=strlen(&buffer[slen]);
	sprintf(&buffer[slen],"%07ld|",trans_sdt.sdtTotalPrice);
	slen+=strlen(&buffer[slen]);
	sprintf(&buffer[slen],"%06ld|",trans_sdt.sdtTransactionCounter);
	slen+=strlen(&buffer[slen]);
	sprintf(&buffer[slen],"%06ld|",trans_sdt.sdtWeight);
	slen+=strlen(&buffer[slen]);
	sprintf(&buffer[slen],"%s|",trans_sdt.sdtTraceNo);
	slen+=strlen(&buffer[slen]);
	sprintf(&buffer[slen],"%s|",plu_name);
	slen+=strlen(&buffer[slen]);

	sprintf(buffer,"i00F030,%02XN%03d", code, slen-15);
	buffer[14]=':';
	bcc=gen_bcc((INT8U *)&buffer[15],slen-15);
	buffer[slen++]=bcc;
	*max_cnt = cnt;
	return slen;
}
#endif

#ifdef USE_CHN_CART_SCALE
INT16U transaction_sale_make_data_type7(char *buffer, INT16U tail)
{
	INT16U cnt;
	INT32U addr;
	INT8U temp1;
	long  mul;
	REPORT_LOG_STRUCT trReportLogData;
	LOG_UPLOAD_INF logUpload;
	INT16S   slen;
	INT8U bcc;
	INT16U plu_key;
	INT16U len;
	INT8U plu_name[54];

	slen = 15;

	//Scale Capa에 따른 무게값을 "g" 단위로 전송 하고자 할때
	temp1=get_global_bparam(GLOBAL_SALE_SETUP_FLAG7);
	temp1=(temp1>>6)&0x03;
	mul = 1L;
	if(temp1 > 0) mul = power10(temp1);

	cnt = 1;
	addr=tail;
	addr%=Startup.max_send_buf;
	addr*=MAX_TRANSACTION_TYPE7_SALE_SIZE;
	addr+=Startup.addr_send_buf;

	get_nvram_sparam(addr,(INT8U *)&trReportLogData, sizeof(REPORT_LOG_STRUCT));
	trReportLogData.weightOrcount *= mul;

	plu_table_search(status_scale.departmentid, trReportLogData.pluNo, &plu_key, 0);
	plu_get_field(plu_key-1, PTYPE_PLU_NAME1, (INT8U *)&plu_name, (INT16S *)&len, sizeof(plu_name)); // name1
	plu_name[sizeof(plu_name)-1] = 0;

	memset((char *)&logUpload, 0, sizeof(LOG_UPLOAD_INF));

	logUpload.dt[0] = trReportLogData.year;
	logUpload.dt[1] = trReportLogData.month;
	logUpload.dt[2] = trReportLogData.day;
	logUpload.dt[3] = trReportLogData.hours;
	logUpload.dt[4] = trReportLogData.minutes;
	logUpload.dt[5] = trReportLogData.seconds;

	memcpy(logUpload.traceCode, trReportLogData.traceCode, TRACE_CODE_SIZE);

	memcpy(logUpload.name, plu_name, MAX_ARTNAME);//for test current 32 assign

	logUpload.pluNo = trReportLogData.pluNo;
	logUpload.stallNo = get_net_wparam(NETWORK_STORE_ID);
	logUpload.unitPrice = trReportLogData.uprice;
	logUpload.weight = trReportLogData.weightOrcount;
	logUpload.totalPrice = trReportLogData.tprice;

	memcpy(buffer, (char *)&logUpload, sizeof(LOG_UPLOAD_INF));

	slen = sizeof(LOG_UPLOAD_INF);

	return slen;
}
#endif

#ifdef USE_INDIA_FUNCTION
INT8U RealtimeSendMode = SEND_TRANSACTION;

INT16U transaction_sale_make_data_type8(char *buffer, INT8U code, INT16U head, INT16U tail, INT16U *max_cnt)
{
	INT16U cnt;
	INT32U addr;
	INT8U temp1;
	long  mul;
	TRANSACTION_SALE_DATA_TYPE8 trans_sdt;
	TRANSACTION_PAY trans;
	INT16S   slen;
	INT8U bcc;
	INT16U plu_key;
	INT16U len;
	INT16U chk;

	slen = 15;

	//Scale Capa에 따른 무게값을 "g" 단위로 전송 하고자 할때
	temp1=get_global_bparam(GLOBAL_SALE_SETUP_FLAG7);
	temp1=(temp1>>6)&0x03;
	mul = 1L;
	if(temp1 > 0) mul = power10(temp1);

	cnt = 1;

	addr=tail;
	addr%=Startup.max_send_buf;
	addr*=MAX_TRANSACTION_TYPE8_SALE_SIZE;//sizeof(TRANSACTION_RECORD_TYPE);
	addr+=Startup.addr_send_buf;

	slen+= ethernet_gen_head(&buffer[slen]);
	sprintf(&buffer[slen],"T=%04X.",tail+cnt);  	    slen+=7; // 현재 tail에 전송할 tr수를 더한 값. buffer 개수보다 클 수 있다.

	chk = get_nvram_wparam(addr);
	if (chk == 0xffff) // pay
	{
		get_nvram_sparam(addr+2, (INT8U *)&trans, sizeof(TRANSACTION_PAY));
		sprintf(&buffer[slen], "C=%02X.", trans.clerkid); slen+=strlen(&buffer[slen]);
		slen+=gen_transaction_pay(&buffer[slen], trans);
		sprintf(buffer, "i00F071,01L%03X", slen-15);
		RealtimeSendMode = SEND_PAY;
	}
	else
	{
		get_nvram_sparam(addr,(INT8U *)&trans_sdt,sizeof(TRANSACTION_SALE_DATA_TYPE8));
		trans_sdt.sdtWeight *= mul;
		plu_table_search(trans_sdt.sdtDept, trans_sdt.sdtPLUNo, &plu_key,0);
		if (UseItemCode13Digit)
		{
			plu_get_field(plu_key-1, PTYPE_EXT_BARCODE, (INT8U *)&trans_sdt.sdtItemCode13, (INT16S *)&len, 13); // Item Code 13
			trans_sdt.sdtItemCode13[13] = 0;
		}
		memcpy(&buffer[slen],&trans_sdt,sizeof(TRANSACTION_SALE_DATA_TYPE8));
		slen+=sizeof(TRANSACTION_SALE_DATA_TYPE8);
		memset(&buffer[slen], 0, 55);
		plu_get_field(plu_key-1, PTYPE_PLU_NAME1, (INT8U *)&buffer[slen], (INT16S *)&len, 55); // name1
		buffer[slen+len] = 0;
		slen += 55;
		sprintf(buffer,"i00F078,%02XL%03X", code, slen-15);
		RealtimeSendMode = SEND_TRANSACTION;
	}

	buffer[14]=':';
	bcc=gen_bcc((INT8U *)&buffer[15],slen-15);
	buffer[slen++]=bcc;
	*max_cnt = cnt;

	return slen;
}
#endif

#ifdef USE_SEND_TICKET_DATA
INT16U transaction_sale_make_data_type9(char *buffer, INT8U code, INT16U head, INT16U tail, INT16U *max_cnt)
{
	INT16U cnt;
	INT32U addr;
	INT8U temp1;
	long  mul;
	TRANSACTION_SALE_DATA_TYPE trans_sdt;
	TRANSACTION_CHANGE_DATA_TYPE trans_cdt;
	INT16S   slen;
	INT8U bcc;
	INT8U tflag;
	INT16U plu_key;
	INT16U len;

	slen = 15;

	//Scale Capa에 따른 무게값을 "g" 단위로 전송 하고자 할때 
	temp1=get_global_bparam(GLOBAL_SALE_SETUP_FLAG7);
	temp1=(temp1>>6)&0x03;
	mul = 1L;
	if(temp1 > 0) mul = power10(temp1);

	cnt = 1;

	addr=tail; 
	addr%=Startup.max_send_buf;
	addr*=MAX_TRANSACTION_TYPE4_SALE_SIZE;//sizeof(TRANSACTION_RECORD_TYPE); 
	addr+=Startup.addr_send_buf;

	slen+= ethernet_gen_head(&buffer[slen]);
	sprintf(&buffer[slen],"T=%04X.",tail+cnt);  	    slen+=7; // 현재 tail에 전송할 tr수를 더한 값. buffer 개수보다 클 수 있다. 

	get_nvram_sparam(addr,(INT8U *)&trans_sdt,sizeof(TRANSACTION_SALE_DATA_TYPE));
	trans_sdt.sdtWeight *= mul;
	memcpy(&buffer[slen],&trans_sdt,sizeof(TRANSACTION_SALE_DATA_TYPE));
	slen+=sizeof(TRANSACTION_SALE_DATA_TYPE);
	plu_table_search(trans_sdt.sdtDept, trans_sdt.sdtPLUNo, &plu_key,0);
	memset(&buffer[slen], 0, 55);
	if(trans_sdt.sdtPLUNo != 0)
	{
		plu_get_field(plu_key-1, PTYPE_PLU_NAME1, (INT8U *)&buffer[slen], (INT16S *)&len, 55); // name1
	}
	buffer[slen+len] = 0;
	slen += 55;
	sprintf(buffer,"i00F070,%02XL%03X", code, slen-15);
	buffer[14]=':';
	bcc=gen_bcc((INT8U *)&buffer[15],slen-15);
	buffer[slen++]=bcc;
	*max_cnt = cnt;

	return slen;
}
#endif

#ifdef USE_SEND_TRACE_PRICE
INT16U transaction_sale_make_data_type10(char *buffer, INT8U code, INT16U tail, INT16U *max_cnt)
{
	INT32U addr;
	REALTIME_TRACEPRICE  trData;
	INT16S   slen;
	INT8U scale_name_buffer[22];
	INT8U plu_name_buffer[61];	
	INT8U trace_code_buffer[20];
	char temp_buffer[200];	
	INT16U i;	

	if (GlbOper.realtimeTransactionType != 8) return 0;
	// 업체 코드
	memset(scale_name_buffer,0x0,22); 
	get_net_sparam(NETWORK_SCALE_NAME,(INT8U *)scale_name_buffer,20);
	scale_name_buffer[8]=0;

	sprintf(temp_buffer, "/trans.do?scale=E@%s@", scale_name_buffer);
	slen = strlen("/trans.do?scale=E@") + 8 + 1;

	addr=tail; 
	addr%=Startup.max_send_buf;
	addr*=MAX_TRANSACTION_TYPE10_SALE_SIZE;
	addr+=Startup.addr_send_buf;
	get_nvram_sparam(addr,(INT8U *)&trData,sizeof(REALTIME_TRACEPRICE));

	// 품목 코드
	sprintf(&temp_buffer[slen],"%06ld@",trData.trace_meatcode); 
	slen+=7;
	// 등급 코드
	sprintf(&temp_buffer[slen],"%02d@",trData.trace_grade_no); 
	slen+=3;
	// 판매 시각
	sprintf(&temp_buffer[slen],"20%02d%02d%02d%02d%02d%02d@",trData.trace_date[0], trData.trace_date[1],\
		trData.trace_date[2], trData.trace_date[3], trData.trace_date[4], trData.trace_date[5]); 
	slen+=15;
	// 판매 단위
	sprintf(&temp_buffer[slen],"%02d@",trData.trace_unit);
	slen+=3;
	// 판매 가격
	sprintf(&temp_buffer[slen],"%ld@",trData.trace_price); 
	slen+=ilog(trData.trace_price)+1;
	// 상품명 
	memset(plu_name_buffer,0x0,61); 	
	euckr_utf8_converter(plu_name_buffer, trData.pluname, strlen((char*)trData.pluname));
	for(i=0;i<strlen((char*)plu_name_buffer);i++)
	{
		if(isURL_unreserved(plu_name_buffer[i]))
		{
			sprintf(&temp_buffer[slen],"%c",plu_name_buffer[i]); 		
			slen++;				
		}
		else
		{
			sprintf(&temp_buffer[slen],"%%%X", plu_name_buffer[i]); 		
			slen+=3;		
		}
	}
	sprintf(&temp_buffer[slen++],"@"); 			
	// 이력 번호 
	memcpy(trace_code_buffer, trData.trace_code, sizeof(trData.trace_code));
	trace_code_buffer[sizeof(trData.trace_code)]=0;
	sprintf(&temp_buffer[slen],"%s",trace_code_buffer); 
	slen+=strlen((char*)trace_code_buffer);	
	//근내지방도
	if((trData.trace_grade >= 7) && (trData.trace_grade <= 9))	// 등급번호 8,9,10 -> 1++(7), 1++(8),1++(9) 해당
	{
		sprintf(&temp_buffer[slen++],"@");
		sprintf(&temp_buffer[slen],"%1d",trData.trace_grade);
		slen+=1;
	}	
	slen = make_http_data(buffer, temp_buffer, slen, 0,0);	
//	slen = make_http_data(buffer, "/test/", 6, 0,0);
	return slen;

}
#endif

#ifdef USE_EFT_POS_PROTOCOL
// 이란 EFT-POS 전용 PROTOCOL
// 구성 : STX[1] + STATION ID[1] + LENGTH[2] + DATA[variable] + BCC[1] + ETX[1]
// DATA : head[fixed] + Total_Sales_Amount[variable] + tail[fixed]
//      - head : <cpml><command><purchase><amount>
//      - tail : </amount><print>yes</print></purchase></command></cpml>
// BCC  : STATION ID + DATA - XOR all byte
INT16U transaction_eft_pos_data(char *buffer, INT16U head, INT16U tail)
{
	TRANSACTION_SALE_DATA_TYPE trans_sdt;
	INT32U addr;

	addr=tail; 
	addr%=Startup.max_send_buf;
	addr*=MAX_TRANSACTION_TYPE4_SALE_SIZE;
	addr+=NVRAM_SEND_TRANSACTION2;
	get_nvram_sparam(addr,(INT8U *)&trans_sdt,sizeof(TRANSACTION_SALE_DATA_TYPE));

	const char* headDataString = "<cpml><command><purchase><amount>";
	const char* tailDataString = "</amount><print>yes</print></purchase></command></cpml>";	
	INT32U total_amount = trans_sdt.sdtTotalPrice;
	INT8U totalAmountString[10] = "";
	INT16U headDataSize = strlen(headDataString);
	INT16U tailDataSize = strlen(tailDataString);
	INT16U totalAmountSize = ilog(total_amount);
	INT16U dataSize = 0;
	INT16S slen = 0;

	sprintf(totalAmountString, "%d", total_amount);
	dataSize = headDataSize + tailDataSize + totalAmountSize;

	buffer[slen++] = STX;
	buffer[slen++] = status_scale.scaleid; 		// Station ID(0~255)
	buffer[slen++] = (dataSize & 0x00FF);
	buffer[slen++] = (dataSize & 0xFF00) >> 16;
	strcpy(&buffer[slen], headDataString);		slen += headDataSize;
	strcpy(&buffer[slen], totalAmountString);	slen += totalAmountSize;
	strcpy(&buffer[slen], tailDataString);		slen += tailDataSize;
	buffer[slen++] = gen_bcc(&buffer[1], slen-2);
	buffer[slen++] = ETX;

	return slen;
}
#endif

#ifdef USE_HYUNDAI_MEAT_FUNCTION
INT16U transaction_sale_make_data_type14(char *buffer, INT8U code, INT16U tail, INT16U *max_cnt)
{
	INT16U cnt;
	INT32U addr;
	TRANSACTION_SALE_DATA_TYPE14 trans_sdt;
	INT16S   slen;
	INT8U bcc;
	INT16U len;

	slen = 15;
    
    cnt = 1;
	addr=tail; 
	addr%=Startup.max_send_buf;
	addr*=MAX_TRANSACTION_TYPE14_SALE_SIZE;
	addr+=Startup.addr_send_buf;
    
    slen+= ethernet_gen_head(&buffer[slen]);
    sprintf(&buffer[slen],"C=%02X.",0); slen+=5; 
    sprintf(&buffer[slen],"T=%04X.",tail+cnt);          slen+=7; // 현재 tail에 전송할 tr수를 더한 값. buffer 개수보다 클 수 있다. 

	get_nvram_sparam(addr,(INT8U *)&trans_sdt,sizeof(TRANSACTION_SALE_DATA_TYPE14));

	// Sale Data type 14
	memcpy(&buffer[slen],&trans_sdt,sizeof(TRANSACTION_SALE_DATA_TYPE14));
	slen+=sizeof(TRANSACTION_SALE_DATA_TYPE14);

    sprintf(buffer,"i00F070,%02XL%03X", code, slen-15); 

	buffer[14]=':';
	bcc=gen_bcc((INT8U *)&buffer[15],slen-15);
	buffer[slen++]=bcc;
	*max_cnt = cnt;

	return slen;
}
#endif

INT8U transaction_sale_send(HUGEDATA COMM_BUF *CBuf)
{
	INT16U head,tail;
	INT16S   slen;
	char  *commun_buffer;
	INT8U  ret,err;
	CAPTION_STRUCT cap;
	INT8U  nretry;
	INT16U  cnt;

	head=get_nvram_wparam(NVRAM_SEND_REMAINDED_HEAD);
	tail=get_nvram_wparam(NVRAM_SEND_REMAINDED_TAIL);

	if (head==tail) return 4;

	if((!ethernet_list.remoteip[0]) && (!ethernet_list.remoteip[1])) return 4; //SG070712

	if (!network_status.connection)	return 6;

	commun_buffer=(char *)serial_send_point_for_client;
	if (ethernet_list.status==2)
	{
		if (get_nvram_bparam(NVRAM_TRANS_BACKUP_FLAG)) return 4;	// local backup 있을때 무시
		cnt = 1;
		slen = transaction_sale_make_data(commun_buffer, 0x03, head, tail, &cnt);
	}
	else
	{
		return 4;
	}
	tail += cnt;

	ret=0;
	err=0;
	nretry=0;
	while (!ret) {
//if (nretry>=1)
//{
//	sprintf(MsgBuf, "[FD][FL_ERR3_E_%d]\r\n", nretry);
//	MsgOut(MsgBuf);
//}
		ret=Ethernet_Server(CLIENT_OPEN, 0, CLIENT_CLERK_TRANS);
		nretry++;
		//if (nretry>=NET_CHK_MASTER_RETRY) break;
		if (nretry >= 4) break;
	}

	if (ret==1)
	{
	    	network_status.send_trans_sale=0;
		commun_outleng(CBuf,commun_buffer,slen);
		network_timeout_reset(NETWORK_TIMEOUT_REQUEST_LOGIN);
		network_counter_reset(NETWORK_TIMEOUT_REQUEST_LOGIN);
		while (1)
		{
			if (network_status.send_trans_sale) break;
			network_common_trans();
			if (network_timeout_check(10,NETWORK_TIMEOUT_REQUEST_LOGIN))
			{
	    		        if (network_counter_check(2, NETWORK_TIMEOUT_REQUEST_LOGIN)) {
					commun_outleng(CBuf,commun_buffer,slen);
		    	        } else {
					err=1;
    					break;
				}
			}
		}
		Ethernet_Server(CLIENT_CLOSE, 0, CLIENT_CLERK_TRANS);
//sprintf(MsgBuf, "[RCV2]flag=%d\r\n", network_status.send_trans_sale);
//MsgOut(MsgBuf);
		if (err) goto M1;
		//else network_status.time_trans1=10;
		if (network_status.send_trans_sale == 2)	// memory full
		{
			caption_split_by_code(0xC90D,&cap,0);
			DisplayMsg(cap.form);
			transaction_sale_set_tail(tail);
			return 2;
		}
		else if (network_status.send_trans_sale == 4)	// 잘못된 데이터 전송에 대한 응답
		{
			//BuzOn(4);
			network_error(MSG_21_NET_ERR);
			transaction_sale_set_tail(tail);
			return 3;
		}
		else if (network_status.send_trans_sale == 5)	// memory full
		{
			caption_split_by_code(0xCA06,&cap,0);	//MAX TRNASACTION IS 50!
			DisplayMsg(cap.form);
			transaction_sale_set_tail(tail);
			return 2;
		}

		else if (network_status.send_trans_sale == 1 || network_status.send_trans_sale == 3)	// 1:ok, 3:중복전송답변
		{
			transaction_sale_set_tail(tail);
		}
	}
	else
	{
M1:
		//network_status.time_trans1+=50;
		//if (network_status.time_trans1>=150) network_status.time_trans1=150;
		if (Operation.operationClerk == OPER_FLOATING && ethernet_list.status == 2) // floating clerk && slave	//CJK080313
		{
			transaction_sale_set_tail(tail);
		}
		network_error(MSG_21_NET_ERR);
		Delay_100ms(2);
		return 1;	// timeout
	}
	return 0;
}

//기존 transaction_sale_send return value  ret=0:ok, 1:timeout, 2:memory full, 3:send data error, 4:etc(setting err)
//return value 0:OK, 1: time_ount 2:memory full, 3:send data error, 4:etc(setting err) 6:remote connection 7:Ethernet_Server close
INT8U send_realtime_data(INT8U use_rs232, INT16U *sendtail, INT16U *org_tail)
{
	INT16U head,tail;
	INT16S   slen;
	char  *commun_buffer;
	INT8U  ret;
	INT8U  nretry;
	INT16U  cnt;
	COMM_BUF *CBuf;
	INT8U cmd;

	head=get_nvram_wparam(NVRAM_SEND_REMAINDED_HEAD);
	tail=get_nvram_wparam(NVRAM_SEND_REMAINDED_TAIL);
	*org_tail = tail;

	if (head==tail) return 4;	

	if (!use_rs232)
	{
		if((!ethernet_list.remoteip[0]) && (!ethernet_list.remoteip[1])) return 4;

		if (!network_status.connection)	return 6;
		CBuf = &CommBufEthData[0];
	}
	else
	{
		CBuf = &CommBuf;
	}

	commun_buffer=(char *)serial_send_point_for_client;
	if (ethernet_list.status==2)	// type1,2 방식만 전송
	{
		if (get_nvram_bparam(NVRAM_TRANS_BACKUP_FLAG)) return 4;	// local backup 있을때 무시

		cnt = 1;
		if (GlbOper.realtimeTransactionType == 2)	// type 2:Basic+Indiv.(Kor)
		{
			cmd = 0x23;
		}
		else
		{
			cmd = 0x03;
		}
		slen = transaction_sale_make_data(commun_buffer, cmd, head, tail, &cnt);
	}
	else if (ethernet_list.status==1)
	{
		cnt = 10;
		slen = transaction_sale_make_data(commun_buffer, 0x03, head, tail, &cnt);
	}
	else if (ethernet_list.status==4)
	{
		return 4;
	}
	else
	{
		cnt = 1;
		if (GlbOper.realtimeTransactionType == 1)	// 1:Basic
		{
			slen = transaction_sale_make_data(commun_buffer, 0x03, head, tail, &cnt);
		}
		else if (GlbOper.realtimeTransactionType == 2)	// 2:Basic+Indiv.(Kor)
		{
			slen = transaction_sale_make_data(commun_buffer, 0x23, head, tail, &cnt);
		}
		//else if (GlbOper.realtimeTransactionType == 3)	// 3:Full
		//{
		//	slen = transaction_sale_make_data_type3(commun_buffer, 0x03, head, tail, &cnt);
		//}
		else if (GlbOper.realtimeTransactionType == 4)	// 4:E-Mart.(Kor)
		{
#ifdef USE_EMART_BACK_LABEL	//0x41 : 바코드 30자리로 10자리 늘어남. 총 100 bytes 전송.
			slen = transaction_sale_make_data_type4(commun_buffer, 0x41, head, tail, &cnt);
#elif defined(USE_INDIA_FUNCTION)
			slen = transaction_sale_make_data_type8(commun_buffer, 0x03, head, tail, &cnt);
#elif defined(USE_CL5J_REALTIME_TRNASFER)			
			slen = CL5_JR_transaction_sale_make_data(commun_buffer, 0x99, head, tail, &cnt);
#else	//0x40 : 기존 이마트 프로토콜
			slen = transaction_sale_make_data_type4(commun_buffer, 0x40, head, tail, &cnt);
#endif
		}
		else if (GlbOper.realtimeTransactionType == 5)	// 5:Basic+Plu Name+Indiv.(Kor)
		{
			slen = transaction_sale_make_data(commun_buffer, 0x53, head, tail, &cnt);
		}
#ifdef USE_IMPORT_MEAT_PROTOCOL
		else if (GlbOper.realtimeTransactionType == 6)	// 6:Import meat(Kor)
		{
			slen = transaction_sale_make_data_type6(commun_buffer, 0x05, tail, &cnt);
		}
#endif
#ifdef USE_SEND_TRACE_PRICE
		else if (GlbOper.realtimeTransactionType == 8)	// 8: Disclosure meat price(Kor)
		{
			slen = transaction_sale_make_data_type10(commun_buffer, 0x09, tail, &cnt);
		}
#endif
#ifdef USE_SEND_TICKET_DATA
		else if (GlbOper.realtimeTransactionType == 9)	// 9:Send Ticket Data ( Iran, Mexico Bakery Function )
		{
			slen = transaction_sale_make_data_type9(commun_buffer, 0x09, head, tail, &cnt);
		}
#endif
#ifdef USE_HYUNDAI_MEAT_FUNCTION
		else if (GlbOper.realtimeTransactionType == 14) //	14: Hyundai protocol (Kor)
		{
			slen = transaction_sale_make_data_type14(commun_buffer, 0x14, tail, &cnt);
		}
#endif
		else	// 0:none
		{
			return 4;
		}
	}

	*sendtail = tail + cnt;

	ret=0;
	nretry=0;
	if (!use_rs232)
	{
		while (!ret)
		{
			ret=Ethernet_Server(CLIENT_OPEN, NET_REALTIME_WAITTIME, CLIENT_REALTIME_DATA);
			nretry++;
			if (nretry>=NET_CHK_MASTER_RETRY) break;
		}
	}
	else
	{
		ret = 1;
	}

	// RS232
	if (ret==1)
	{
	    	network_status.send_trans_sale=0;
#ifdef USE_INDIA_FUNCTION
		if (GlbOper.realtimeTransactionType == 4)
		{
			network_status.send_trans_pay=0;
		}
#endif
		commun_outleng((HUGEDATA COMM_BUF *)CBuf,commun_buffer,slen);
	}
	else return 7;		//이더넷 서버가 열리지 않을경우 return 7
	return 0;
}

// return value
//	0 : connection fail
//	1 : ok
//	2 : timeout
//	3 : memory full
//	4 : data 오류(clerk 범위 오류 등)
//	5 : 설정 오류(service type, remote ip)
//	6 : cancel(MODE key)
INT8U transaction_sale_restore(HUGEDATA COMM_BUF *CBuf)
{
	INT16U head,tail;
	INT16S   slen;
	char  *commun_buffer;
	INT8U  ret, ret1;
	CAPTION_STRUCT cap;
	INT16U  cnt;
	char msg[50];
	INT16U total_cnt, cur_cnt;
	INT8U backup_directdraw;
	INT8U loop;
	INT8U cmd;

	head=get_nvram_wparam(NVRAM_SEND_REMAINDED_HEAD);
	tail=get_nvram_wparam(NVRAM_SEND_REMAINDED_TAIL);

	if (head==tail) return 1;

	if((!ethernet_list.remoteip[0]) && (!ethernet_list.remoteip[1])) return 5;

	if (ethernet_list.status != 2) return 5;
	// slave 모드에서는 type 1,2 방식만 전송

	commun_buffer=(char *)serial_send_point_for_client;

	if (head >= tail) total_cnt = head - tail;
	else              total_cnt = head + Startup.max_send_buf - tail;
	cur_cnt = 1;

	backup_directdraw = DspStruct.DirectDraw;
	DspStruct.DirectDraw = 1;
	ret=Ethernet_Server(CLIENT_OPEN, 0, CLIENT_CLERK_TRANS);

	ret1 = 0;
	loop = ON;
	if (ret==1)
	{

		while(loop)
		{
			head=get_nvram_wparam(NVRAM_SEND_REMAINDED_HEAD);
			tail=get_nvram_wparam(NVRAM_SEND_REMAINDED_TAIL);

			if (head==tail)
			{
				ret1 = 1;	// 완료
				break;
			}
			sprintf(msg, "Restore sale %d/%d",  cur_cnt, total_cnt);
			PutString(DSP_MSG_Y, DSP_MSG_X, msg, DSP_SYSTEM_FONT_ID, 1, 1, MAX_MSG_CHAR);

			cnt = 1;
			if (GlbOper.realtimeTransactionType == RTsend_basic_indiv_kr || GlbOper.realtimeTransactionType == RTsend_trace_cn )	// type 2:Basic+Indiv.(Kor)
			{
				cmd = 0x33;
			}
			else	// type 0,1,3 => type 1
			{
				cmd = 0x13;
			}
			slen = transaction_sale_make_data(commun_buffer, cmd, head, tail, &cnt);
			tail += cnt;

		    	network_status.send_trans_sale=0;
			commun_outleng(CBuf,commun_buffer,slen);
			network_timeout_reset(NETWORK_TIMEOUT_REQUEST_LOGIN);
			network_counter_reset(NETWORK_TIMEOUT_REQUEST_LOGIN);
			while (1)
			{
				network_common_trans();
				if (network_status.send_trans_sale)
				{
					break;
				}
				if (network_timeout_check(20,NETWORK_TIMEOUT_REQUEST_LOGIN))
				{
					if (network_counter_check(2, NETWORK_TIMEOUT_REQUEST_LOGIN))
					{
						commun_outleng(CBuf,commun_buffer,slen);
					}
					else
					{
						break;	// timeout
					}
				}
				if (KEY_IsKey())
				{
					DspStruct.DirectDraw = backup_directdraw;
					Ethernet_Server(CLIENT_CLOSE, 0, CLIENT_CLERK_TRANS);
					return 6;
				}
			}

			switch (network_status.send_trans_sale)
			{
				case 0:	// timeout
					loop = OFF;
					ret1 = 2;
					break;
				case 1:	// ok
				case 3:	// 0x13:같은 Transaction 수신
					cur_cnt++;
					transaction_sale_set_tail(tail);
					break;
				case 2:	// 0x12:Memory Full
					caption_split_by_code(0xC90D,&cap,0);
					DisplayMsg(cap.form);
					ret1 = 3;	// memory full
					loop = OFF;
					break;
				case 4:	// 0x11:clerk no초과, 0x23:해당index없음, 0x24중복됨
					BuzOn(2);
					ret1 = 4;
					loop = OFF;
					break;
			}
		}
	}
	else	// connection fail
	{
	}
	Ethernet_Server(CLIENT_CLOSE, 0, CLIENT_CLERK_TRANS);
	DspStruct.DirectDraw = backup_directdraw;
	return ret1;
}

void transaction_pay_send(HUGEDATA COMM_BUF *CBuf,INT8U direct)
{
	INT8U head,tail,bcc;
	INT16S slen;
	char  *commun_buffer;
	INT32U addr;
	TRANSACTION_PAY trans;
	INT8U  ret,err;
	INT8U  nretry;

	head=get_nvram_bparam(NVRAM_TRANS_PAY_HEAD);
	tail=get_nvram_bparam(NVRAM_TRANS_PAY_TAIL);
	if (head==tail) return;
	if (ethernet_list.status!=2) return;
	if (get_nvram_bparam(NVRAM_TRANS_BACKUP_FLAG)) return;	// local backup 있을때 무시
	if (!network_status.connection)	return;

	addr=tail; addr*=sizeof(TRANSACTION_PAY); addr+=Startup.addr_paysend_buf;	//Modified by JJANG 20080310
	get_nvram_sparam(addr,(INT8U *)&trans,sizeof(TRANSACTION_PAY));

	commun_buffer=(char *)serial_send_point_for_client;
	slen = 15;
	slen+= ethernet_gen_head(&commun_buffer[slen]);
	sprintf(&commun_buffer[slen],"C=%02X.",trans.clerkid); slen+=strlen(&commun_buffer[slen]);
	// not allow tail is zero
	tail++;
	sprintf(&commun_buffer[slen],"T=%02X.",tail);          slen+=strlen(&commun_buffer[slen]);
	slen+=gen_transaction_pay(&commun_buffer[slen],trans);
	sprintf(commun_buffer,"i00F071,01L%03X",slen-15);
	commun_buffer[14]=':';
	bcc=gen_bcc((INT8U *)&commun_buffer[15],slen-15);
	commun_buffer[slen++]=bcc;
	ret=1;
	err=0;
	if (direct) //ret=Ethernet_Server(CLIENT_OPEN, 0, CLIENT_CLERK_TRANS);
	{
		ret = 0;
		while (!ret)
		{
			ret = Ethernet_Server(CLIENT_OPEN, 0, CLIENT_CLERK_TRANS);
			nretry++;
			if (nretry >= 4) break;
		}
	}
	if (ret==1) {
		if (direct) network_direct(1);

		commun_outleng(CBuf,commun_buffer,slen);
	    	network_status.send_trans_pay=0;
		network_timeout_reset(NETWORK_TIMEOUT_SEND_PAY);
		while (1) {
			if (network_status.send_trans_pay) break;
			network_common_trans();
			if (network_timeout_check(10, NETWORK_TIMEOUT_SEND_PAY))
			{
				if (network_counter_check(2, NETWORK_TIMEOUT_SEND_PAY)) // 총 3회 재전송
				{
					commun_outleng(CBuf, commun_buffer, slen);
				}
				else
				{
					err=1;
					break;
				}
			}
		}
		if (network_status.send_trans_pay)
		{
			transaction_pay_set_tail(tail);
		}
		if (direct) {
			network_direct(0);
			Ethernet_Server(CLIENT_CLOSE, 0, CLIENT_CLERK_TRANS);
		}
		if (err) goto M1;
		else network_status.time_trans2=10;
	} else {
M1:
		network_error(MSG_21_NET_ERR);
		network_status.time_trans2+=50;
		if (network_status.time_trans2>=100) network_status.time_trans2=100;//
	}
}

// return value
//	0 : connection fail
//	1 : ok
//	2 : timeout
//	3 :
//	4 :
//	5 : 설정 오류(service type, remote ip)
//	6 : cancel(MODE key)
INT8U transaction_pay_restore(HUGEDATA COMM_BUF *CBuf)
{
	INT8U bcc;
	INT16U head,tail;
	INT16S   slen;
	char  *commun_buffer;
	INT8U  ret, ret1;
	TRANSACTION_PAY trans;
	char msg[50];
	INT16U total_cnt, cur_cnt;
	INT8U backup_directdraw;
	INT32U addr;

	head=get_nvram_bparam(NVRAM_TRANS_PAY_HEAD);
	tail=get_nvram_bparam(NVRAM_TRANS_PAY_TAIL);

	if (head==tail) return 1;

	if((!ethernet_list.remoteip[0]) && (!ethernet_list.remoteip[1])) return 5;

	if (ethernet_list.status != 2) return 5;

	commun_buffer=(char *)serial_send_point_for_client;

	if (head >= tail) total_cnt = head - tail;
	else              total_cnt = head + Startup.max_send_buf - tail;
	cur_cnt = 1;

	backup_directdraw = DspStruct.DirectDraw;
	DspStruct.DirectDraw = 1;
	ret=Ethernet_Server(CLIENT_OPEN, 0, CLIENT_CLERK_TRANS);

	ret1 = 0;
	if (ret==1)
	{

		while(1)
		{
			head=get_nvram_bparam(NVRAM_TRANS_PAY_HEAD);
			tail=get_nvram_bparam(NVRAM_TRANS_PAY_TAIL);

			if (head==tail)
			{
				ret1 = 1;	// 완료
				break;
			}
			sprintf(msg, "Restore pay %d/%d",  cur_cnt, total_cnt);
			PutString(DSP_MSG_Y, DSP_MSG_X, msg, DSP_SYSTEM_FONT_ID, 1, 1, MAX_MSG_CHAR);

			addr=tail; addr*=sizeof(TRANSACTION_PAY); addr+=NVRAM_TRANS_PAY;
			get_nvram_sparam(addr,(INT8U *)&trans,sizeof(TRANSACTION_PAY));

			slen = 15;
			slen+= ethernet_gen_head(&commun_buffer[slen]);
			sprintf(&commun_buffer[slen],"C=%02X.",trans.clerkid); slen+=strlen(&commun_buffer[slen]);
			// not allow tail is zero
			tail++;
			sprintf(&commun_buffer[slen],"T=%02X.",tail);          slen+=strlen(&commun_buffer[slen]);
			slen+=gen_transaction_pay(&commun_buffer[slen],trans);
			sprintf(commun_buffer,"i00F071,13L%03X",slen-15);
			commun_buffer[14]=':';
			bcc=gen_bcc((INT8U *)&commun_buffer[15],slen-15);
			commun_buffer[slen++]=bcc;

		    	network_status.send_trans_pay=0;
			commun_outleng(CBuf,commun_buffer,slen);
			network_timeout_reset(NETWORK_TIMEOUT_REQUEST_LOGIN);
			network_counter_reset(NETWORK_TIMEOUT_REQUEST_LOGIN);

			while (1)
			{
				network_common_trans();
				if (network_status.send_trans_pay) break;
				if (network_timeout_check(20,NETWORK_TIMEOUT_REQUEST_LOGIN))
				{
					if (network_counter_check(2, NETWORK_TIMEOUT_REQUEST_LOGIN))
					{
						commun_outleng(CBuf,commun_buffer,slen);
					}
					else
					{
						break;	// timeout
					}
				}
				if (KEY_IsKey())
				{
					//BuzOn(1);
					DspStruct.DirectDraw = backup_directdraw;
					Ethernet_Server(CLIENT_CLOSE, 0, CLIENT_CLERK_TRANS);
					return 6;
				}
			}
			if (network_status.send_trans_pay)	// ok
			{
				transaction_pay_set_tail(tail);
				cur_cnt++;
			}
			else	// timeout
			{
				ret1 = 2;
				break;
			}
		}
	}
	else
	{
	}
	DspStruct.DirectDraw = backup_directdraw;
	Ethernet_Server(CLIENT_CLOSE, 0, CLIENT_CLERK_TRANS);

	return ret1;
}

INT8U send_transaction_backup(void)
{
	INT8U mode;

	mode = 1;

	if (get_nvram_bparam(NVRAM_TRANS_BACKUP_FLAG) && Operation.operationClerk == OPER_FLOATING && ethernet_list.status == 2)
	{
		while(1)
		{
			mode = transaction_sale_restore(&(CommBufEthData[0]));
			if (mode == 1) break;// success(buffer empty)
			else if (mode == 6) break;
		}
		if (mode == 1)
		{
			while(1)
			{
				mode = transaction_pay_restore(&(CommBufEthData[0]));
				if (mode == 1) break;// success(buffer empty)
				else if (mode == 6) break;
			}
		}
		if (mode == 1)
		{
			set_nvram_bparam(NVRAM_TRANS_BACKUP_FLAG, 0);
		}
		sale_display_update(0x1fff);
	}
	return mode;
}

extern void network_common_trans_client(void);
#ifdef USE_TRACE_STANDALONE
void trace_information_send(HUGEDATA COMM_BUF *CBuf, TRANSACTION_RECORD_TYPE *pTrans)
{
	TRACE_TRANS_TYPE traceTrans;
	INT8U temp_buffer[100];
	INT8U remote_ip2[4];
	INT16U remote_port2;
	INT16S   slen;
	char  *commun_buffer;
	INT8U  ret, err, nretry;

	get_base_sparam(FLASH_REMOTE_IP2, remote_ip2, 4);
	remote_port2 = get_base_wparam(FLASH_REMOTE_IP2+4);

	if (remote_ip2[0] == 0) return;

	commun_buffer = (char *)serial_send_point_for_client;

	memset((char*)&traceTrans, 0x00, sizeof(traceTrans));
	memcpy(traceTrans.traceNo, CurTraceStatus.indivStr.individualNO, sizeof(traceTrans.traceNo));
	traceTrans.traceNo[INDIVIDUAL_NO_LEN] = 0;
	memcpy(traceTrans.pluName, status_scale.Plu.ItemName, 40);
	traceTrans.pluName[40] = 0;
	if (pTrans)
	{
		traceTrans.sendType = 1;	// 1:인쇄
		traceTrans.traceType = 0;	// 개체번호
		if (pTrans->trtIndividualIndex&0x80) traceTrans.traceType = 1;	// 묶음번호
		traceTrans.weight = pTrans->trtWeight;
		traceTrans.unitPrice = pTrans->trtUnitPrice;
		traceTrans.totalPrice = pTrans->trtVolume;
	}
	else
	{
		traceTrans.sendType = 0;	// 0:호출
		traceTrans.traceType = CurTraceStatus.indivStr.lotFlag;	//0:개체식별번호, 1:묶음번호
		traceTrans.weight = status_scale.Weight;
		traceTrans.unitPrice = status_scale.UnitPrice;
		traceTrans.totalPrice = status_scale.TotalPrice;
	}

	slen = 0;
	sprintf((char*)&temp_buffer[slen],"%d,", traceTrans.traceType); slen = strlen((char*)temp_buffer);
	sprintf((char*)&temp_buffer[slen],"%c%s%c,", '"', traceTrans.traceNo, '"'); slen = strlen((char*)temp_buffer);
	sprintf((char*)&temp_buffer[slen],"%d,", traceTrans.sendType); slen = strlen((char*)temp_buffer);
	sprintf((char*)&temp_buffer[slen],"%ld,", traceTrans.weight); slen = strlen((char*)temp_buffer);
	sprintf((char*)&temp_buffer[slen],"%ld,", traceTrans.unitPrice); slen = strlen((char*)temp_buffer);
	sprintf((char*)&temp_buffer[slen],"%ld,", traceTrans.totalPrice); slen = strlen((char*)temp_buffer);
	sprintf((char*)&temp_buffer[slen],"%c%s%c,", '"', traceTrans.pluName, '"'); slen = strlen((char*)temp_buffer);
	sprintf(commun_buffer, "i00F020,01N%c%c", (INT8U)(slen%0x100)&0xff, (INT8U)(slen/0x100)&0xff);
	memcpy(&commun_buffer[13], temp_buffer, slen);
	slen += 13;

	put_dest_ip(remote_ip2, remote_port2);

	network_status.send_trace_info = 0;
	ret=0;
	err=0;
	nretry=0;
	while (!ret) {
		ret=Ethernet_Server(CLIENT_OPEN, NET_CHK_REMOTE2_WAITTIME, CLIENT_TRACE_INFO);
		nretry++;
		if (nretry>=NET_CHK_MASTER_RETRY) break;
	}

	if (ret==1)
	{
		commun_outleng(CBuf, commun_buffer, slen);
		network_timeout_reset(NETWORK_TIMEOUT_SEND_TRACE);
		network_counter_reset(NETWORK_TIMEOUT_SEND_TRACE);
		while (1)
		{
			if (network_status.send_trace_info) break;
			//network_common_trans();
			network_common_trans_client();
			if (network_timeout_check(2,NETWORK_TIMEOUT_SEND_TRACE)) 	// 0.2초
			{
	    		        if (network_counter_check(0, NETWORK_TIMEOUT_SEND_TRACE)) {
					commun_outleng(CBuf,commun_buffer,slen);
		    	        } else {
					err = 1;
    					break;
				}
			}
		}
	}
	else
	{
		err = 1;
	}
	if (err || network_status.send_trace_info == 2)
	{
		//network_error(MSG_21_NET_ERR);
		//BuzOn(3);
	}
	Ethernet_Server(CLIENT_CLOSE, NET_CHK_REMOTE2_WAITTIME, CLIENT_TRACE_INFO);

	//restore_dest_ip();	// Ethernet_Server()에서 ip를 복구하도록 수정
}
#endif

INT8U inter_transaction_sale(INT8U code,TRANSACTION_RECORD_TYPE *trans,INT32U v32)
{
	INT8U ret;
	ret=TRUE;
	switch (code) {
	    case '1': trans->trtType=(INT8U)v32; break;
	    case '2': trans->trtScaleId=(INT8U)v32; break;
	    case '3': trans->trtPLUtype=(INT8U)v32; break;
	    case '4': trans->trtDept  =(INT8U)v32; break;
	    case '5': trans->trtPLU   =v32; break;
	    case '6': trans->trtGroup=(INT8U)v32; break;
	    case '7': trans->trtOperator=(INT8U)v32; break;
	    case '8': trans->trtCode=v32; break;
	    case '9': trans->trtWeight=v32; break;
	    case 'a': trans->trtQty=(INT16U)v32; break;
	    case 'b': trans->trtPCS=(INT16U)v32; break;
	    case 'c': trans->trtUnitPrice=v32; break;
	    case 'd': trans->trtVolume=v32; break;
	    case 'e': trans->trtDiscountVolume=v32; break;
	    case 'f': trans->trtMultiple=(INT16U)v32; break;
	    case 'g': trans->trtTransactionCounter=v32; break;
	    case 'h': trans->trtTaxType=(INT8U)v32; break;
	    case 'i': trans->trtTaxNumber=(INT8U)v32; break;
	    case 'j': trans->trtTax=v32; break;
	    case 'k': trans->trtTicketNumber=v32; break;
	    case 'l': trans->trtStatus2=(INT8U)v32; break;
	    case 'm': trans->trtCreditCustomerNo=(INT16U)v32; break;
	    case 'n': trans->trtStatus=(INT8U)v32; break;
	    case 'o': trans->trtSerialNo=(INT8U)v32; break;
	    default : ret=FALSE;
	}
	return ret;
}

INT8U inter_transaction_pay(INT8U code,TRANSACTION_PAY *trans,INT32U v32)
{
	INT8U ret;
	INT16S icode;
	ret=TRUE;
	switch (code) {
	    case 'a': trans->clerkid=(INT8U)v32; break;
	    case 'b': trans->scaleid=(INT8U)v32; break;
	    case 'c': trans->summaryround  =v32; break;
	    case 'd': trans->change =v32; 	 break;
	    case 'e': trans->trtType=v32;        break;
	    case 'f': trans->summarydiscount=v32; break;	//Added by JJANG 20080416
#ifdef USE_DISCOUNTED_TAX_FOR_DISCOUNTED_TOTAL
	    case 'g': trans->taxPrice[0] = v32; break;
	    case 'h': trans->taxPrice[1] = v32; break;
	    case 'i': trans->taxPrice[2] = v32; break;
	    case 'j': trans->taxPrice[3] = v32; break;
	    case 'k': trans->taxPrice[4] = v32; break;
	    case 'l': trans->taxPrice[5] = v32; break;
	    case 'm': trans->taxPrice[6] = v32; break;
	    case 'n': trans->taxPrice[7] = v32; break;
	    case 'o': trans->taxPrice[8] = v32; break;
	    case 'p': trans->taxPrice[9] = v32; break;

	    case 'G': trans->taxGross[0] = v32; break;
	    case 'H': trans->taxGross[1] = v32; break;
	    case 'I': trans->taxGross[2] = v32; break;
	    case 'J': trans->taxGross[3] = v32; break;
	    case 'K': trans->taxGross[4] = v32; break;
	    case 'L': trans->taxGross[5] = v32; break;
	    case 'M': trans->taxGross[6] = v32; break;
	    case 'N': trans->taxGross[7] = v32; break;
	    case 'O': trans->taxGross[8] = v32; break;
	    case 'P': trans->taxGross[9] = v32; break;
#endif
#ifdef USE_INDIA_FUNCTION
	    case 'v': trans->ticketNo = v32; break;
	    case 'x': trans->transCount = v32; break;
#endif
	    default : icode=(INT16S)(code-'1');
		      if ((icode>=0) && (icode<MAX_PAID_TYPE)) {
			trans->paidprice[icode]=v32;
		      } else {
			ret=FALSE;
		      }
	}
	return ret;
}

INT16S  gen_transaction_pay(char *commun_buffer,TRANSACTION_PAY trans)
{
	INT16S slen;
	INT16S i;
	slen=0;
	sprintf(&commun_buffer[slen],"a=%02X.",trans.clerkid          	); slen+=5;
	sprintf(&commun_buffer[slen],"b=%02X.",trans.scaleid          	); slen+=5;
	sprintf(&commun_buffer[slen],"c=%lX.",trans.summaryround    	); slen+=strlen(&commun_buffer[slen]);
	sprintf(&commun_buffer[slen],"d=%lX.",trans.change          	); slen+=strlen(&commun_buffer[slen]);
	sprintf(&commun_buffer[slen],"e=%02X.",trans.trtType          	); slen+=5;
	sprintf(&commun_buffer[slen],"f=%lX.",trans.summarydiscount	); slen+=strlen(&commun_buffer[slen]);	//Added by JJANG 20080416
#ifdef USE_INDIA_FUNCTION
	sprintf(&commun_buffer[slen],"v=%lX.",trans.ticketNo		); slen+=strlen(&commun_buffer[slen]);
	sprintf(&commun_buffer[slen],"x=%lX.",trans.transCount		); slen+=strlen(&commun_buffer[slen]);
#endif
#ifdef USE_DISCOUNTED_TAX_FOR_DISCOUNTED_TOTAL
	for(i = 0; i <MAX_TAX_NO; i++)
	{
		sprintf(&commun_buffer[slen],"%c=%lX.",'g'+i, trans.taxPrice[i]); slen+=strlen(&commun_buffer[slen]);
		sprintf(&commun_buffer[slen],"%c=%lX.",'G'+i, trans.taxGross[i]); slen+=strlen(&commun_buffer[slen]);
	}
#endif
	for (i=0; i<MAX_PAID_TYPE; i++)
	{
		if (trans.paidprice[i]==0)
		{
		} else
		{
			sprintf(&commun_buffer[slen],"%c=%08lX.",'1'+i,trans.paidprice[i]);
			slen+=strlen(&commun_buffer[slen]);
		}
	}
	return slen;
}

extern INT16U SyncErrCode;
//"Cx<cmd>:<E|O><errcode><0x0a>"
//"W<cmd>:<E|O><errcode><0x0a>"
void commun_return(INT8U cmd,INT16U errcode)
{
	switch (cmd) {
		case 0x02:
		case 0x19:
			SyncErrCode = errcode;
			break;
		case 0x29:
			global_one2one_send = errcode;
			if (!network_status.backup_plu) network_status.backup_plu=1;
			break;
	}
	if ( network_status.load_dc ) network_status.load_dc =0;
//	if (!network_status.load_plu) network_status.load_plu=1;
}

void commun_write_err_info(HUGEDATA COMM_BUF *CBuf,INT8U action,INT32U cmd,INT8U ret)
{
	INT8U  bcc;
	INT16S l;
	char *commun_buffer;

	commun_buffer=(char *)serial_send_point;
	l=9;
	sprintf((char *)&commun_buffer[l],"c=%03lX.",cmd);
	l+=strlen((char *)&commun_buffer[l]);
	sprintf((char *)&commun_buffer[l],"r=%02X.",ret);
	l+=strlen((char *)&commun_buffer[l]);
	if (ret==FALSE) {
		sprintf(commun_buffer,"i%02X:E84%c",action,0x0a);
		commun_out(CBuf,commun_buffer);
	} else {
		//l=strlen((char *)&commun_buffer[9]);
		sprintf(commun_buffer,"i%02X:L%03X",action,l-9);
		commun_buffer[8] = ',';
		bcc=gen_bcc((INT8U *)&commun_buffer[9],l-9);
		commun_buffer[l++]=bcc;
		commun_outleng(CBuf,(char *)commun_buffer,l);
	}
}

INT8U  commun_common_ethernet(INT8U code,ETH_CONNECT_LIST *eth,INT32U v32)
{
	INT8U ret;
	ret=TRUE;
	switch (code) {
		case '^': eth->scaleid     =(INT8U)v32; break;
		case '*': eth->departmentid=(INT8U)v32; break;
		case '&': eth->ip[0]=(INT8U)(v32>>24);
			  eth->ip[1]=(INT8U)((v32>>16)&0xff);
			  eth->ip[2]=(INT8U)((v32>> 8)&0xff);
			  eth->ip[3]=(INT8U)(v32&0xff);
			  break;
		case '@': eth->port=(INT16U)v32; break;
		case '$': if (v32) eth->enable=0;
			  else     eth->enable=1;
			  break;
		default : ret=FALSE;
	}
	return ret;
}

/*
INT8U recv_check_scaleid(INT8U scaleid)
{
	INT16S i;
	if (ethernet_list.status!=2) {
		if (ethernet_list.master.scaleid==scaleid) return TRUE;
	}
	for (i=0; i<ethernet_list.nlist-1; i++) {
		if (ethernet_list.elist[i].scaleid==scaleid) {
			return TRUE;
		}
	}
	return FALSE;
}
*/

INT8U commun_getrtc(INT16S cmd,INT8U *str)
{
	INT16S l;
	INT8U ret;

#ifdef USE_PERSIAN_CALENDAR
	RTC_Convert_Persian_Read();
#else
	RTC_CLK_Burst_Read();
#endif
	ret=TRUE;
	switch (cmd) {
		case 1: sprintf((char *)str,"Y=%02X.M=%02X.D=%02X.",bcd2hex(RTCStruct.year),bcd2hex(RTCStruct.month),bcd2hex(RTCStruct.date)); break;
		case 2: sprintf((char *)str,"h=%02X.m=%02X.s=%02X.",bcd2hex(RTCStruct.hour),bcd2hex(RTCStruct.min  ),bcd2hex(RTCStruct.sec )); break;
		case 3: sprintf((char *)str,"Y=%02X.M=%02X.D=%02X.",bcd2hex(RTCStruct.year),bcd2hex(RTCStruct.month),bcd2hex(RTCStruct.date));
			l=strlen((char *)str);
		        sprintf((char *)&str[l],"h=%02X.m=%02X.s=%02X.",bcd2hex(RTCStruct.hour),bcd2hex(RTCStruct.min),bcd2hex(RTCStruct.sec));
		default: ret=FALSE;
	}
	return ret;
}

INT8U commun_setrtc(INT8U *str,INT16S leng)
{
	INT16S status;
	INT16S r,s;
	INT8U v32;
	INT8U code;
	ETH_CONNECT_LIST eth;

	r=0;
	s=0;
	status=0;
#ifdef USE_PERSIAN_CALENDAR
	RTC_Convert_Persian_Read();
#else
	RTC_CLK_Burst_Read();
#endif
	while (1) {
		if (r>=leng) break;
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
						case 'Y':RTCStruct.year =hex2bcd((INT8U)v32); s=1; break;
						case 'M':RTCStruct.month=hex2bcd((INT8U)v32); s=1; break;
						case 'D':RTCStruct.date =hex2bcd((INT8U)v32); s=1; break;
						case 'h':RTCStruct.hour =hex2bcd((INT8U)v32); s=1; break;
						case 'm':RTCStruct.min  =hex2bcd((INT8U)v32); s=1; break;
						case 's':RTCStruct.sec  =hex2bcd((INT8U)v32); s=1; break;
						default :commun_common_ethernet(code,&eth,v32);    break;

					}
					status=0;
				} else {
					v32<<=4;
					v32|=ctoh(str[r]);
				}

		}
		r++;
	}
END:	if (s) {
		ethernet_list.timesync=1;
//sprintf(MsgBuf, "TimeSync OK(%d)\r\n", (INT16U)(SysTimer100ms - SysBootTime));
//MsgOut(MsgBuf);
#ifdef USE_PERSIAN_CALENDAR
		RTC_Convert_Persian_Write();
#else
		RTC_CLK_Burst_Write();
#endif
#ifdef USE_MORNITORING_CNT
		rtc_set_pre_minute();
#endif

		return 1;
	}
	return 0x83;
}

//20090507 Added by JJANG 농협요청, 영업일자 프로토콜 추가
INT8U commun_set_business_time(INT8U *str,INT16S leng)
{
	INT16S status;
	INT16S r,s;
	INT8U v32;
	INT8U code;
	ETH_CONNECT_LIST eth;
	INT8U b_date,b_month,b_year;
#ifdef USE_PERSIAN_CALENDAR
	INT16U perCentury;
#endif

	r=0;
	s=0;
	status=0;
#ifdef USE_PERSIAN_CALENDAR
	RTC_Convert_Persian_Read();
#else
	RTC_CLK_Burst_Read();
#endif
	while (1) {
		if (r>=leng) break;
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
						case 'Y':RTCStruct.year =hex2bcd((INT8U)v32); s=1; break;
						case 'M':RTCStruct.month=hex2bcd((INT8U)v32); s=1; break;
						case 'D':RTCStruct.date =hex2bcd((INT8U)v32); s=1; break;
						case 'y':b_year	=hex2bcd((INT8U)v32); s=1; break;
						case 'm':b_month=hex2bcd((INT8U)v32); s=1; break;
						case 'd':b_date	=hex2bcd((INT8U)v32); s=1; break;
						default :commun_common_ethernet(code,&eth,v32);    break;
					}
					status=0;
				} else {
					v32<<=4;
					v32|=ctoh(str[r]);
				}
		}
		r++;
	}
END:	if (s) {
		ethernet_list.timesync=1;
#ifdef USE_PERSIAN_CALENDAR
		if(RTCStruct.year < 0x5a)	perCentury = 1400; //90년 이전: 1400년대, 90년 이후: 1300년대
		else perCentury = 1300;

		RTCStruct.day = date_getweek((short)RTCStruct.year+perCentury,(short)RTCStruct.month,(short)RTCStruct.date);
#else
		RTCStruct.day = date_getweek((short)RTCStruct.year+2000,(short)RTCStruct.month,(short)RTCStruct.date);
#endif
		RTC_CLK_Write(6, RTCStruct.year);
		RTC_CLK_Write(4, RTCStruct.month);
		RTC_CLK_Write(3, RTCStruct.date);
		RTC_CLK_Write(5, RTCStruct.day);

		set_nvram_bparam(NVRAM_BUSINESS_DATE+2, b_year);
		set_nvram_bparam(NVRAM_BUSINESS_DATE+1, b_month);
		set_nvram_bparam(NVRAM_BUSINESS_DATE, b_date);

		set_nvram_bparam(NVRAM_BUSINESS_DATE_FLAG,1);
		return 1;
	}
	return 0x83;
}

//"W88Axxxx,<data>....."
// Setting ROM download 순서
// (1) Parameter ("W85A...")
// (2) 20(0x14) : Symbol
// (3) 21(0x15) : Caption + label caption + PCS Unit + PLU NAME
// (4) 8(0x08) : Origin
// (5) 4(0x04) : Keypad
// (6) 12(0x0C) : Font
// (7) Label ()
INT32U commun_get_block_type_address(INT8U data,INT32U *max_size)
{
	INT32U addr;
	switch (data) {
		case 1: addr=DFLASH_BASE+FLASH_GLOBAL_AREA;		// All Global Data
			(*max_size)=FLASH_NET_AREA - FLASH_GLOBAL_AREA; break;
		case 2: addr=DFLASH_BASE+FLASH_KEY_SPEED_AREA;		// Speed Key
			(*max_size)=MAX_SPEEDKEY_SIZE*5; break;
		case 3: addr=DFLASH_BASE+FLASH_DEALER_AREA;		// All Dealer Area
			(*max_size)=FLASH_NET_AREA-FLASH_DEALER_AREA; break;

		case 4: addr=DFLASH_BASE+FLASH_KEY_AREA;		// E. Key Sale,PGM1,PGM2
			(*max_size)=256*6; break;

		case 5: addr=DFLASH_BASE+FLASH_CAPTION_AREA;		//only caption
		#ifdef USE_EXTEND_CAPNTION_AREA		// 확인필요!
            (*max_size) = 72000l+200l;
        #else
			(*max_size) = 72000l+4l;
        #endif
			break;
		case 6: addr=DFLASH_BASE+FLASH_BASIC_PARAMETER;		// A
			(*max_size)=FLASH_NET_AREA-FLASH_BASIC_PARAMETER;
			break;
		case 7: addr=DFLASH_BASE+FLASH_NET_AREA;		// Network Set
			(*max_size)=NETWORK_RESERVED; break;
		case 8: addr=DFLASH_BASE+FLASH_ORIGIN_AREA;		// D. Origin
			(*max_size)=ORIGIN_STRUCT_SIZE*MAX_ORIGIN_NO;
			break;
		case 9: addr=DFLASH_BASE+FLASH_DISCOUNT_GLOBAL_WEIGHT;   // Discount
			(*max_size)=DISCOUNT_STRUCT_SIZE*(3+MAX_DISCOUNT_NO);
			break;
		case 10:addr=DFLASH_BASE+FLASH_CLERK_AREA;
			(*max_size)=sizeof(CLERK_STRUCT)*MAX_CLERK_NO; break;
		case 11:addr=DFLASH_BASE+FLASH_CLERK_PERMISSION;
			(*max_size)=40*2;
			break;
		case 12:addr =(INT32U)DFLASH_BASE+(INT32U)FLASH_FONT_AREA;		// Font
#ifdef USE_CHN_FONT
			(*max_size) =(INT32U)FLASH_FONT_CODEMAP;			// china font codemap
#else
			(*max_size) =(INT32U)FLASH_CAPTION_AREA;
#endif
			(*max_size)-=(INT32U)FLASH_FONT_AREA; break;
		case 13:addr=DFLASH_BASE+FLASH_LABEL_AREA;				// Label
			(*max_size)=FLASH_BITMAP_AREA-FLASH_LABEL_AREA; break;
		case 14:addr=DFLASH_BASE+FLASH_BITMAP_AREA;
			(*max_size) =28l*3;
			(*max_size)+=8192l*14l;
			break;
		case 15:addr=DFLASH_BASE+FLASH_LABEL_CAPTION_AREA;
			(*max_size)=LABEL_CAP_STRUCT_SIZE*MAX_LABEL_CAP_NO; break;
		case 16:addr=DFLASH_BASE+FLASH_PCS_UNIT_AREA;	// unit symbol
			(*max_size)=8*18; break;
//		case 17:addr=DFLASH_BASE+FLASH_PRINTER_AREA;	// Font ID and
//			addr+=PRT_FONT_MENUID;
//			(*max_size)=3; break;
		case 20:addr=DFLASH_BASE+FLASH_BASIC_SYMBOL_AREA;	// B. Basic Symbol
			(*max_size)=FLASH_BASIC_SYMBOL_AREA_END-FLASH_BASIC_SYMBOL_AREA;
			break;
		case 21: addr =(INT32U)DFLASH_BASE+(INT32U)FLASH_CAPTION_AREA;	// caption + label caption + PCS Unit + PLU NAME
		#ifdef USE_EXTEND_CAPNTION_AREA
            (*max_size) = 72004l+200l+6400l+PCS_TABLE_SIZE+4096l; // C. PLU Name
        #else
			(*max_size) = 72004l+6400l+PCS_TABLE_SIZE+4096l; // C. PLU Name
        #endif
			break;
		case 22: addr=DFLASH_BASE+FLASH_KEY_AREA;		// All Key
			(*max_size) =256*8;
			(*max_size)+=512*5; break;
#ifdef USE_CHN_FONT
		case 23: addr=DFLASH_BASE+FLASH_FONT_CODEMAP;
			(*max_size)=FLASH_CAPTION_AREA-FLASH_FONT_CODEMAP;
			break;
#endif
#ifdef USE_EXTEND_CAPNTION_AREA
		case 24: addr =(INT32U)DFLASH_BASE+(INT32U)FLASH_CAPTION_EXT_AREA;	// caption_ext
			(*max_size) = (MAX_CAPTION_NO-MID_CAPTION_NO)*CAP_MSG_LENGTH; // 
			break;
#endif
		case 99:addr=DFLASH_BASE;
			(*max_size)=0; break;
	}
	return addr;
}

INT32U SearchParamSet(INT16U code, PARAMETER_SETTING_TYPE *pset)
{
	INT16U k=0;
	INT32U ret;

	ret = 0;
	memset(pset,0,sizeof(PARAMETER_SETTING_TYPE));
#ifdef USE_TRANSFER_MENU_SETTING
	if(code>500 && code<2000)
#else
	if(code>500 && code<1000)
#endif	//#ifdef USE_TRANSFER_MENU_SETTING
	{
		while (1)
		{
			if (ParameterSetting[k].funcCode==0) break;
			if (ParameterSetting[k].funcCode==code) {
				memcpy(pset,&ParameterSetting[k],sizeof(PARAMETER_SETTING_TYPE));
				ret = 1;
				break;
			}
			k++;
		}
	}
	else
	{
	}
	//return pset->addrMemory;
	return ret;
}
/*
#ifdef USE_PROTOCOL_CL5000JR
response CL5000J version protocol instead of CL5000 versio protocol for test CL5000J protocol via RS-232
in case of TCP, you can set CL5000J model each IP in setting menu
INT8U commun_send_block_data(HUGEDATA COMM_BUF *CBuf,INT32U cmd,INT8U data,INT32U no)
{
	INT32U addr,max_size,pnt;
	INT16S    com_type,r,d_size;
	INT8U  status,bcc,ret;
	//char   *commun_buffer;
	char	commun_buffer[30];
	FW_VERSION fversion;
	INT8U nvram_version[4];
	INT8U plu_version;

	commun_buffer[0]='W';
	commun_buffer[1]='V';
	commun_buffer[2]=0x01;
	commun_buffer[3]=0x00;
	commun_buffer[4]=0x00;
	commun_buffer[5]=0x00;
	commun_buffer[6]=',';
	commun_buffer[7]=0x10;
	commun_buffer[8]=0x00;
	commun_buffer[9]=':';
	commun_buffer[10]=0x00;
	commun_buffer[11]=0x92;
	commun_buffer[12]=0x13;
	commun_buffer[13]=0x02;
	commun_buffer[14]=0x00;
	commun_buffer[15]=0x00;
	commun_buffer[16]=0x20;
	commun_buffer[17]=0x00;
	commun_buffer[18]=0x01;
	commun_buffer[19]=0x00;
	commun_buffer[20]=0x01;
	commun_buffer[21]=0x00;
	commun_buffer[22]=0x00;
	commun_buffer[23]=0x00;
	commun_buffer[24]='R';
	commun_buffer[25]='U';
	commun_buffer[26]=':';
	commun_buffer[27]=0x21;
	commun_buffer[28]=0x0d;

	r=29;
	commun_outleng(CBuf,(char *)commun_buffer,r);
	ret=1;
	return ret;
}
#endif//#ifdef USE_PROTOCOL_CL5000JR
*/

void commun_send_block_addr(HUGEDATA COMM_BUF *CBuf,INT32U startaddr,INT8U flag,INT16U size)
{
	char  *commun_buffer;
	INT8U bcc;
	INT16S   start_r,r;

	commun_buffer=(char *)serial_send_point;
	start_r = 16;
	r  = start_r;
	r+= ethernet_gen_head(&commun_buffer[r]);
	sprintf(&commun_buffer[r],"A=%08lX.",startaddr);
	r+=11;
	if (size) Flash_sread(startaddr,(HUGEDATA INT8U *)&commun_buffer[r],size);
	r+=size;
	sprintf(commun_buffer,"W29F%02X,%04XL%03X",flag,size,r-start_r);
	commun_buffer[15]=':';
	bcc=gen_bcc((INT8U *)&commun_buffer[start_r],(INT16S)(r-start_r));
	commun_buffer[r++]=bcc;
	commun_outleng(CBuf,(char *)commun_buffer,r);
}

//SG071220
#ifdef USE_PROTOCOL_CL5000JR
INT8U commun_read_plu_data(HUGEDATA COMM_BUF *CBuf,INT16U address,INT32U xptype,INT8U xmode)
{
	INT8U  bcc;
	INT16U deptno;
	INT16S    r,str_start;
	char   *commun_buffer;
	INT8U  ret;
	INT32U pluno;

	deptno=0;
	ret   =1;
	commun_buffer=(char *)serial_send_point;

	Flash_flush_nvbuffer();

	str_start=18;
 	r=str_start;

	sprintf((char*)&commun_buffer[r],"N=%04X.",address);
	r+=7;
	if (address<=0l ) {
		pluno =xptype;
		deptno=0;
		goto END1;
	}
	if (xptype>MAX_PLU_NUMBER) goto END1;

	if (deptno>99) { deptno=0; pluno=0; }
	ret = 1;
END1:	bcc=gen_bcc((INT8U *)&commun_buffer[str_start],r-str_start);
	sprintf(commun_buffer,"W02A%05lX,",pluno);
	sprintf(&commun_buffer[10],"%02XL%04X",deptno,r-str_start);
	commun_buffer[str_start-1]=':';
	commun_buffer[r++]=bcc;
	commun_outleng(CBuf,(char *)commun_buffer,r);
	return ret;
}
#endif

INT8U commun_send_plu_from_address(HUGEDATA COMM_BUF *CBuf,INT16U address,INT32U xptype,INT8U xmode)
{
	INT8U *data,bcc,stype;
	INT16U sz,ptype,rsz,deptno;
	INT16S    i,j,iret,r,str_start;
	char   *commun_buffer;
	INT8U  ret;
	INT32U addr,pluno;
#ifdef USE_DIRECT_INGREDIENT//DEL080826#if (ROM_COUNTRY == ROM_RUS)
	INT16U ingred_no[DIRECT_INGREDIENT_BLOCK_NUM];
	INT16S k;
	INT8U  end;
	INT16U r_temp;
#endif
#ifdef USE_PLU_WITH_TRACE_PROTOCOL
	PLU_INDIVIDUAL_NODE_DATA savedTrace;
	INT16U indivNo;
#endif
#ifdef USE_CONTROL_CHANGING_PRICE
	INT16U field70;
	INT8U ctrlPriceChange;
	INT8U upperLimit;
	INT8U lowerLimit;
#endif
#ifdef USE_INDIA_FUNCTION
	INT32U bagWeight;
#endif
	INT8U tempBuf[256];
	INT16U tsz;

	deptno=0;
	ret   =1;
	commun_buffer=(char *)serial_send_point;
	Flash_flush_nvbuffer();
	str_start=18;
 	r=str_start;
	if (xmode)
		r+=ethernet_gen_head(&commun_buffer[r]);

	sprintf((char*)&commun_buffer[r],"N=%04X.",address);
	r+=7;
	if (address<=0l    ) {
		pluno =xptype;
		deptno=0;
		goto END1;
	}

	if (xptype>MAX_PLU_NUMBER) goto END1;
	if (Startup.disable_dept) {
		ptype=1;
		commun_buffer[r++]='F';
		commun_buffer[r++]='=';
		tohex2((INT8U)ptype,(char *)&commun_buffer[r]); r+=2;
		commun_buffer[r++]='.';
		stype = 'W';
		sz    = 2;
		tohex2((INT8U)stype,(char *)&commun_buffer[r]); r+=2;
		commun_buffer[r++]=',';
		tohex2((INT8U)sz   ,(char *)&commun_buffer[r]); r+=2;
		commun_buffer[r++]=':';
		deptno = 1;
		memcpy((char *)&commun_buffer[r],&deptno,sz);
		r     += sz;
	}
	for (i=0; i<plu_n_field; i++) {
		ptype=plu_get_ptype_from_fid(i);
		stype=plu_get_stype_from_fid(i);
		iret =plu_callbykey_from_fid(address-1,i,&addr,&sz);
		tsz = sz;
		if (sz > sizeof(tempBuf))       tsz = sizeof(tempBuf);
        else if (sz == 0)                     continue;
        
		Flash_sread(addr, tempBuf, tsz);
		//data=(INT8U *)addr;
		data = tempBuf;
		if (iret==PLUSTR_EOF) break;
		if (iret==PLUSTR_OK) {
			commun_buffer[r++]='F';
			commun_buffer[r++]='=';
#ifdef USE_DIRECT_INGREDIENT
#ifdef USE_INGREDIENT_98_PROTOCOL	// PLU 통신 할 경우 Direct ingredient는 98을 사용(protocol 매뉴얼 참조)
			if (ptype==PTYPE_INGREDIENT_TEXT) //direct ingredient
			{
				tohex2((INT8U)PTYPE_INGREDIENT_SIZE,(char *)&commun_buffer[r]); r+=2;
			}
			else
			{
				tohex2((INT8U)ptype,(char *)&commun_buffer[r]); r+=2;
			}
			commun_buffer[r++]='.';
			if (ptype==PTYPE_INGREDIENT_TEXT) //Only send direct ingredient text size
			{
				memcpy((INT8U *)&ingred_no[0], data, 2*DIRECT_INGREDIENT_BLOCK_NUM);
				stype='W';
			}
			if (stype=='S' && ptype != PTYPE_INGREDIENT_TEXT) {
				rsz=strlen((char *)data);
				if (rsz>sz) rsz=sz;
				sz=rsz;
			}
			tohex2((INT8U)stype,(char *)&commun_buffer[r]); r+=2;
			commun_buffer[r++]=',';
			if (ptype==PTYPE_INGREDIENT_TEXT) // 2 byte length
			{
				tohex2( 2   ,(char *)&commun_buffer[r]); r+=2;
			}
			else	      // 1 byte length
			{
				tohex2((INT8U)sz   ,(char *)&commun_buffer[r]); r+=2;
			}
			commun_buffer[r++]=':';
			rsz=r;
			if (ptype == PTYPE_INGREDIENT_TEXT)
			{
				memcpy((INT8U *)&ingred_no[0], data, 2*DIRECT_INGREDIENT_BLOCK_NUM);
				data = (HUGEDATA INT8U *)&commun_buffer[r];
				ingredient_text_read(&ingred_no[0], (HUGEDATA INT8U*)data, 0, DEFAULT_INGREDIENT_TEXT_ALL_SIZE);
				sz = strlen((char*)data);	// size가 클 경우 나눠서 보내는 방법 필요
				if (sz > DEFAULT_INGREDIENT_TEXT_ALL_SIZE) sz = DEFAULT_INGREDIENT_TEXT_ALL_SIZE;
				data = (INT8U *)&sz;
				commun_buffer[r++]=data[0];
				commun_buffer[r++]=data[1];
			}
			else
			{
				for (j=0; j<sz; j++) {
					commun_buffer[r++]=data[j];
				}
			}
#else	// USE_INGREDIENT_99_PROTOCOL(Max 300byte)
			tohex2((INT8U)ptype,(char *)&commun_buffer[r]); r+=2;
			commun_buffer[r++]='.';
			if (ptype==PTYPE_INGREDIENT_TEXT) //direct ingredient
			{
				memcpy((INT8U *)&ingred_no[0], data, 2*DIRECT_INGREDIENT_BLOCK_NUM);
				stype='S';
			}
			if (stype=='S' && ptype != PTYPE_INGREDIENT_TEXT) {
				rsz=strlen((char *)data);
				if (rsz>sz) rsz=sz;
				sz=rsz;
			}
			tohex2((INT8U)stype,(char *)&commun_buffer[r]); r+=2;
			commun_buffer[r++]=',';
			if (ptype==PTYPE_INGREDIENT_TEXT) // 2 byte length
			{
				r_temp = r;
				r += 4;
				//tohex4(sz   ,(char *)&commun_buffer[r]); r+=4;
			}
			else	      // 1 byte length
			{
				tohex2((INT8U)sz   ,(char *)&commun_buffer[r]); r+=2;
			}
			commun_buffer[r++]=':';
			rsz=r;
			if (ptype == PTYPE_INGREDIENT_TEXT)
			{
				memcpy((INT8U *)&ingred_no[0], data, 2*DIRECT_INGREDIENT_BLOCK_NUM);
				data = (HUGEDATA INT8U *)&commun_buffer[r];
				ingredient_text_read(&ingred_no[0], (HUGEDATA INT8U*)data, 0, DEFAULT_INGREDIENT_TEXT_ALL_SIZE);
				sz = strlen((char*)data);	// size가 클 경우 나눠서 보내는 방법 필요
				if (sz > DEFAULT_INGREDIENT_TEXT_ALL_SIZE) sz = DEFAULT_INGREDIENT_TEXT_ALL_SIZE;
				r += sz;
				tohex4(sz, (char *)&commun_buffer[r_temp]);	//size
			}
			else
			{
				for (j=0; j<sz; j++) {
					commun_buffer[r++]=data[j];
				}
			}
#endif
#else
			tohex2((INT8U)ptype,(char *)&commun_buffer[r]); r+=2;
			commun_buffer[r++]='.';
			if (stype=='S') {
				rsz=strlen((char *)data);
				if (rsz>sz) rsz=sz;
				sz=rsz;
			}
			tohex2((INT8U)stype,(char *)&commun_buffer[r]); r+=2;
			commun_buffer[r++]=',';
			if (ptype==PTYPE_INGREDIENT_TEXT) // 2 byte length
			{
				tohex4(sz   ,(char *)&commun_buffer[r]); r+=4;
			}
			else	      // 1 byte length
			{
				tohex2((INT8U)sz   ,(char *)&commun_buffer[r]); r+=2;
			}
			commun_buffer[r++]=':';
			rsz=r;
			for (j=0; j<sz; j++) {
				commun_buffer[r++]=data[j];
			}
#endif
			if (ptype==1) memcpy((INT8U *)&deptno,(INT8U *)&commun_buffer[rsz],sz);
			if (ptype==2) memcpy((INT8U *)&pluno ,(INT8U *)&commun_buffer[rsz],sz);
		}
	}
#ifdef USE_PLU_WITH_TRACE_PROTOCOL
	TNT_GetTraceNoTable(deptno, pluno, &indivNo, savedTrace.indivNo[0], savedTrace.indivNo[1], savedTrace.indivNo[2], &savedTrace.slaughter_place[0], &savedTrace.slaughter_place[1], &savedTrace.slaughter_place[2], &savedTrace.meat_use, &savedTrace.meat_part, &savedTrace.isLot, &savedTrace.meat_grade);
	for(i = 0; i < 10 ; i++)
	{
		ptype = 128 + i;		//id 128부터 개체정보 전송.
		switch(ptype)
		{
			case 128:
				data = &savedTrace.isLot;
				stype = 'B';
				sz = 1;
				break;
			case 129:
				data = savedTrace.indivNo[0];
				stype = 'S';
				sz = strlen(savedTrace.indivNo[0]) + 1;
				break;
			case 130:
				data = savedTrace.indivNo[1];
				stype = 'S';
				sz = strlen(savedTrace.indivNo[1]) + 1;
				break;
			case 131:
				data = savedTrace.indivNo[2];
				stype = 'S';
				sz = strlen(savedTrace.indivNo[2]) + 1;
				break;
			case 132:
				data = &savedTrace.slaughter_place[0];
				stype = 'B';
				sz = 1;
				break;
			case 133:
				data = &savedTrace.slaughter_place[1];
				stype = 'B';
				sz = 1;
				break;
			case 134:
				data = &savedTrace.slaughter_place[2];
				stype = 'B';
				sz = 1;
				break;
			case 135:
				data = &savedTrace.meat_use;
				stype = 'B';
				sz = 1;
				break;
			case 136:
				data = &savedTrace.meat_part;
				stype = 'B';
				sz = 1;
				break;
			case 137:
				data = &savedTrace.meat_grade;
				stype = 'B';
				sz = 1;
				break;
			default:
				break;
		}

		commun_buffer[r++]='F';
		commun_buffer[r++]='=';
		tohex2((INT8U)ptype,(char *)&commun_buffer[r]); r+=2;
		commun_buffer[r++]='.';
		tohex2((INT8U)stype,(char *)&commun_buffer[r]); r+=2;
		commun_buffer[r++]=',';
		tohex2((INT8U)sz   ,(char *)&commun_buffer[r]); r+=2;
		commun_buffer[r++]=':';
		for (j=0; j<sz; j++)
		{
			commun_buffer[r++]=data[j];
		}

	}
#endif
#ifdef USE_CONTROL_CHANGING_PRICE
	plu_get_field(address-1, PTYPE_NUTRITION_NO,(INT8U *)&field70,(INT16S *)&sz,sizeof(field70));
	ctrlPriceChange = field70 / 10000;	//x0000(10)
	upperLimit = field70 % 10000 / 100;	//0xx00(10)
	lowerLimit = field70 % 10000 % 100;	//000xx(10)
	for(i = 0; i < 3 ; i++)
	{
		ptype = 140 + i;		//id 140~142 가격변경 허용 관련 필드.
		switch(ptype)
		{
			case 140:	//가격변경통제사용
				data = &ctrlPriceChange;
				stype = 'B';
				sz = 1;
				break;
			case 141:	//가격변경 상한%
				data = &upperLimit;
				stype = 'B';
				sz = 1;
				break;
			case 142:	//가격변경 하한%
				data = &lowerLimit;
				stype = 'B';
				sz = 1;
				break;
			default:
				break;
		}
		commun_buffer[r++]='F';
		commun_buffer[r++]='=';
		tohex2((INT8U)ptype,(char *)&commun_buffer[r]); r+=2;
		commun_buffer[r++]='.';
		tohex2((INT8U)stype,(char *)&commun_buffer[r]); r+=2;
		commun_buffer[r++]=',';
		tohex2((INT8U)sz   ,(char *)&commun_buffer[r]); r+=2;
		commun_buffer[r++]=':';
		for (j=0; j<sz; j++)
		{
			commun_buffer[r++]=data[j];
		}
	}
#endif
#ifdef USE_INDIA_FUNCTION
	bagWeight = 0;
	ReadPluBagWt(deptno, pluno, &bagWeight);
	if (bagWeight)
	{
		sprintf(&commun_buffer[r], "b=%08lX.", bagWeight);
		r += strlen(&commun_buffer[r]);
	}
#endif
	if (deptno>99) { deptno=0; pluno=0; }
	ret = 1;
END1:	bcc=gen_bcc((INT8U *)&commun_buffer[str_start],r-str_start);
	sprintf(commun_buffer,"W02A%05lX,",pluno);
	sprintf(&commun_buffer[10],"%02XL%04X",deptno,r-str_start);
	commun_buffer[str_start-1]=':';
	commun_buffer[r++]=bcc;
	commun_outleng(CBuf,(char *)commun_buffer,r);
	return ret;
}

void commun_send_plu_delete(HUGEDATA COMM_BUF *CBuf, INT16U deptNo, INT32U pluNo)
{
	char  *commun_buffer;
	INT16S   r;

	commun_buffer=(char *)serial_send_point;
	r = ethernet_gen_head(commun_buffer);
	sprintf(&commun_buffer[r],"C01F15,%02X%06lX%c", (INT8U)deptNo, pluNo, 0x0a);
	commun_outleng(CBuf, commun_buffer, strlen(commun_buffer));
}

void commun_send_dc_delete(HUGEDATA COMM_BUF *CBuf, INT16U deptNo, INT32U pluNo)
{
	char  *commun_buffer;
	INT16S   r;

	commun_buffer=(char *)serial_send_point;
	r = ethernet_gen_head(commun_buffer);
	sprintf(&commun_buffer[r],"C01F19,%02X%06lX%c", (INT8U)deptNo, pluNo, 0x0a);
	commun_outleng(CBuf, commun_buffer, strlen(commun_buffer));
}

void commun_send_table2_delete(HUGEDATA COMM_BUF *CBuf, INT8U part, INT16U id)
{
	char  *commun_buffer;
	INT16S   r;

	commun_buffer=(char *)serial_send_point;
	r = ethernet_gen_head(commun_buffer);
	sprintf(&commun_buffer[r],"C01F31,%02X%04X%c", part, id, 0x0a);
	commun_outleng(CBuf, commun_buffer, strlen(commun_buffer));
}

void commun_send_table3_delete(HUGEDATA COMM_BUF *CBuf, INT8U part, INT16U id)
{
	char  *commun_buffer;
	INT16S   r;

	commun_buffer=(char *)serial_send_point;
	r = ethernet_gen_head(commun_buffer);
	sprintf(&commun_buffer[r],"C01F33,%02X%04X%c", part, id, 0x0a);
	commun_outleng(CBuf, commun_buffer, strlen(commun_buffer));
}

#ifdef USE_EFT_POS_PROTOCOL
INT8U ChangeRemote2IPFlag = OFF;
#endif
INT8U ChangeDestIPFlag = OFF;
INT8U PutDestIPFlag = OFF;
INT8U TCPDestTempIP[4] = {0, 0, 0, 0};
INT16U TCPDestTempPort = 0;
void change_dest_ip_val(INT32U ip)
{
	EthCardStruct.TCPDestIP[3]= (INT8U)(ip / 0x1000000);
	EthCardStruct.TCPDestIP[2]= (INT8U)((ip / 0x10000) & 0xff);
	EthCardStruct.TCPDestIP[1]= (INT8U)((ip / 0x100) & 0xff);
	EthCardStruct.TCPDestIP[0]= (INT8U)(ip & 0xff);
	EthLocalCmdSend(ETH_SET_DEST_IP ,0);
}

void change_dest_ip(INT8U *ip, INT16U tcp_port)
{
	ChangeDestIPFlag = ON;
	EthCardStruct.TCPDestIP[3]= ip[0];
	EthCardStruct.TCPDestIP[2]= ip[1];
	EthCardStruct.TCPDestIP[1]= ip[2];
	EthCardStruct.TCPDestIP[0]= ip[3];
	EthCardStruct.TCPDestPort = tcp_port;
	EthLocalCmdSend(ETH_SET_DEST_IP ,0);
	EthLocalCmdSend(ETH_SET_DEST_TCP_PORT ,0);
}

void put_dest_ip(INT8U *ip, INT16U tcp_port)
{
	PutDestIPFlag = ON;
	TCPDestTempIP[0]= ip[0];
	TCPDestTempIP[1]= ip[1];
	TCPDestTempIP[2]= ip[2];
	TCPDestTempIP[3]= ip[3];
	TCPDestTempPort = tcp_port;
}

//void change_dest_ip(INT8U idx)
//{
//	EthCardStruct.TCPDestIP[3]= NetSlaveInfo[idx-1].ip[0];
//	EthCardStruct.TCPDestIP[2]= NetSlaveInfo[idx-1].ip[1];
//	EthCardStruct.TCPDestIP[1]= NetSlaveInfo[idx-1].ip[2];
//	EthCardStruct.TCPDestIP[0]= NetSlaveInfo[idx-1].ip[3];
//	EthCardStruct.TCPDestPort = NetSlaveInfo[idx-1].tcp_port;
//	EthLocalCmdSend(ETH_SET_DEST_IP ,0);
//	EthLocalCmdSend(ETH_SET_DEST_TCP_PORT ,0);
//}

void restore_dest_ip(void)
{
	INT8U ip[4];
	INT16U port;

	if (!ChangeDestIPFlag) return;

#ifdef USE_EFT_POS_PROTOCOL
	if(ChangeRemote2IPFlag)
	{
		get_base_sparam(FLASH_REMOTE_IP2, ip, 4);
		port = get_base_wparam(FLASH_REMOTE_IP2 + 4);
	}
	else
	{
		get_net_sparam(NETWORK_MACHINE_REMOTE_IP, ip, 4);
		port = get_net_wparam(NETWORK_MACHINE_REMOTE_TCPPORT);
	}
#else
	get_net_sparam(NETWORK_MACHINE_REMOTE_IP, ip, 4);
	port = get_net_wparam(NETWORK_MACHINE_REMOTE_TCPPORT);
#endif
	EthCardStruct.TCPDestIP[3]=ip[0];
	EthCardStruct.TCPDestIP[2]=ip[1];
	EthCardStruct.TCPDestIP[1]=ip[2];
	EthCardStruct.TCPDestIP[0]=ip[3];
	EthCardStruct.TCPDestPort = port;
	EthLocalCmdSend(ETH_SET_DEST_IP ,0);
	EthLocalCmdSend(ETH_SET_DEST_TCP_PORT ,0);
	ChangeDestIPFlag = OFF;
}

INT16U SyncDeptNo;
INT32U SyncPluNo;
INT16U SyncDelAddr;
INT8U SyncStatus[MAX_SLAVE_SCALE];
INT16U SyncErrCode = 0;

INT8U commun_send_price(HUGEDATA COMM_BUF *CBuf,INT8U deptno,INT32U pluno)
{
	INT8U  bcc,stype;
	INT16U sz,ptype;//,rsz;
	INT16S    r,str_start,usize;
	char   *commun_buffer;
	INT16U addr;
	INT32U v_unitprice;
	INT16U v_pcs;
	INT8U  v_ptype;

	if (Startup.disable_dept) {
		deptno=1;
	}
	if ((deptno==0) || (pluno==0l)) return 1; // Send OK!!!
	plu_table_search((INT8U)deptno,(long)pluno,&addr,0);
	commun_buffer=(char *)serial_send_point;
	str_start=18;
	if (addr==0) {
		str_start=0;
		r=ethernet_gen_head(&commun_buffer[str_start]);
		sprintf(&commun_buffer[r],"C01F15,%02X%06lX%c",deptno,pluno,0x0a);
		commun_outleng(CBuf,commun_buffer,strlen(commun_buffer));
		return 0;
	}
 	r =str_start;
	r+=ethernet_gen_head(&commun_buffer[str_start]);
	sprintf((char*)&commun_buffer[r],"N=%04X.",addr);
	r+=7;
	if (Startup.disable_dept) {
		ptype=1;
		sprintf((char *)&commun_buffer[r],"F=%02X.",ptype);
		r    += 5; stype = 'W'; sz    = 2;
		sprintf((char *)&commun_buffer[r],"%02X,%04X:",stype,sz);
		r     += 8; deptno = 1; memcpy((char *)&commun_buffer[r],&deptno,sz);
		r     += sz;
	}
	addr--;
	plu_get_field(addr, PTYPE_PRICE,(INT8U *)&v_unitprice,&usize,4);
	plu_get_field(addr, PTYPE_PIECE,(INT8U *)&v_pcs      ,&usize,2);
	plu_get_field(addr, PTYPE_PLU_TYPE,(INT8U *)&v_ptype    ,&usize,1);
#ifdef USE_FIXED_PLUTYPE//USE_EMART_NETWORK
	if (v_ptype >= PLU_BY_WEIGHT_FIX && v_ptype <= PLU_BY_PCS_FIX)
	{
		v_ptype -= 3;
	}
#endif

	sprintf((char *)&commun_buffer[r],"F=%02X.",6);r+=5;
	sprintf((char *)&commun_buffer[r],"%02X,%03X:",'L',4); r+=7;
	memcpy(&commun_buffer[r],&v_unitprice,4); r+=4;

	sprintf((char *)&commun_buffer[r],"F=%02X.",14);r+=5;
	sprintf((char *)&commun_buffer[r],"%02X,%03X:",'W',2); r+=7;
	memcpy(&commun_buffer[r],&v_pcs,2); r+=2;

	sprintf((char *)&commun_buffer[r],"F=%02X.",4);r+=5;
	sprintf((char *)&commun_buffer[r],"%02X,%03X:",'B',1); r+=7;
	memcpy(&commun_buffer[r],&v_ptype,1); r+=1;

	bcc=gen_bcc((INT8U *)&commun_buffer[str_start],r-str_start);
	sprintf(commun_buffer,"W02A%05lX,",pluno);
	sprintf(&commun_buffer[10],"%02XL%04X",deptno,r-str_start);
	commun_buffer[str_start-1]=':';
	commun_buffer[r++]=bcc;
	commun_outleng(CBuf,(char *)commun_buffer,r);
	return 1;
}

#ifdef USE_PROTOCOL_CL5000JR
INT8U sendPluData(HUGEDATA INT8U *buffer, INT16U *sLength, INT32U nth, INT8U opCode)
{
	INT16U packetLen;
	INT32U address;
	INT32U addrTemp;
	INT8U checksum;
	INT16U i,j;
	INT32U key;
	INT16U idx;
	INT8U  useIngredientTextField=0;
	//INT16U  dmsgNo=0;
#ifdef USE_DIRECT_INGREDIENT
	INT16U  dmsgNo[DIRECT_INGREDIENT_BLOCK_NUM];
	INT8U   dmagBuf[2101];
	INT16U sz;
	PACKET_HEADER _PACKED_DEF *header;
	INT32U ingredient_address;
	INT32U start_offset;
#endif
	HUGEDATA INT8U *pstr;
	HUGEDATA INT8U *str;
    INT16U  fullDataLen;
	INT16U	dataLen = 4; //sizeof(INT32U); // include room number 4byte
	INT16U  deptno;
	INT32U  pluno;
	INT16U sendDataLen;
	INT32U startAddr;

	Flash_flush_nvbuffer();

	deptno = nth / 1000000L;
	if (deptno == 0)//room number
	{
		//key = nth;
		key = plu_get_next_exist_address((INT16U)nth);
		if (key == 0xffff) return 99;//ERROR_DATA_END
	}
	else
	{
		pluno = nth % 1000000L;
		plu_table_search((INT8U)deptno, (long)pluno, &idx, 0);
		if (idx == 0) return 99;//ERROR_DATA_END
		key = idx;
	}

	address = plu_calc_addr_by_key((long)(key-1));

	addrTemp = address + 3;	//offset 3byte
#ifdef USE_RUSSIA_LOGISTICS_PLU
	addrTemp += LOGISTIC_PLU_STORAGE_SIZE;
#else
	addrTemp += PLU_STORAGE_SIZE;
#endif

	//dmsgNo = Flash_wread(addrTemp);
	//if(dmsgNo) dataLen += 300; // dmsg size : 300 byte
#ifdef USE_DIRECT_INGREDIENT
	header = (PACKET_HEADER *)buffer;
	ingredient_address = header->address;
	memset(&buffer[10], 0, dataLen);	// buffer clear
	memcpy(&buffer[10],(INT8U *)&nth,4);

	str  = (HUGEDATA INT8U *)(buffer+14);

	Flash_sread(addrTemp, (HUGEDATA INT8U *)&dmsgNo[0], 2*DIRECT_INGREDIENT_BLOCK_NUM);

// for debugging
//{
//INT16U k;
//pstr = (HUGEDATA INT8U *)(address+3);
//sprintf(MsgBuf, "[%ld]\r\n", *((INT32U *)(pstr+2)));
//MsgOut(MsgBuf);
//for (k = 0; k < DIRECT_INGREDIENT_BLOCK_NUM; k++)
//{
//	sprintf(MsgBuf, "%d\r\n", dmsgNo[k]);
//	MsgOut(MsgBuf);
//}
//}

	ingredient_text_read(&dmsgNo[0], dmagBuf, 0, DEFAULT_INGREDIENT_TEXT_ALL_SIZE);
	sz = strlen((char*)dmagBuf);	// size가 클 경우 나눠서 보내는 방법 필요

	switch(opCode)
	{
		case DATA_PLU:
			if (sz > DIRECT_INGREDIENT_BLOCK_NUM_CL5KJ_BLOCK_SIZE1)
			{
				dataLen += PLU_STORAGE_SIZE + DIRECT_INGREDIENT_BLOCK_NUM_CL5KJ_BLOCK_SIZE1 + 1;
				fullDataLen = 4 + PLU_STORAGE_SIZE + sz + 1;
				sz = DIRECT_INGREDIENT_BLOCK_NUM_CL5KJ_BLOCK_SIZE1;
			}
			else
			{
				dataLen += PLU_STORAGE_SIZE + sz + 1;
				fullDataLen = dataLen;
			}

			pstr = (HUGEDATA INT8U *)(address+3);

		    sendDataLen = PLU_STORAGE_SIZE;
		    startAddr = address+3;

		    Flash_sread(startAddr, str, sendDataLen);
		    str += sendDataLen;

			ingredient_text_read(&dmsgNo[0], str, 0, sz);
			str += sz;
			*str++ = 0x00;	// NULL
			break;
		case DATA_PLU2: // Small letter 'L'
			if (sz <= DIRECT_INGREDIENT_BLOCK_NUM_CL5KJ_BLOCK_SIZE1) return 97;
#ifdef USE_DMSG_1050_BYTE	//	#ifdef	USE_RUSSIA	//
			sz -= DIRECT_INGREDIENT_BLOCK_NUM_CL5KJ_BLOCK_SIZE1;
			dataLen += sz + 1;
			fullDataLen = dataLen;
			fullDataLen += DIRECT_INGREDIENT_BLOCK_NUM_CL5KJ_BLOCK_SIZE1;

			ingredient_text_read(&dmsgNo[0], str, DIRECT_INGREDIENT_BLOCK_NUM_CL5KJ_BLOCK_SIZE1, sz);
			str += sz;
			*str++ = 0x00;	// NULL
#elif defined USE_DMSG_2100_BYTE
			switch(ingredient_address){
				case 0:
					if(sz>(DIRECT_INGREDIENT_BLOCK_NUM_CL5KJ_BLOCK_SIZE1+DIRECT_INGREDIENT_BLOCK_NUM_CL5KJ_BLOCK_SIZE2+1))
					{
						fullDataLen = sz+5;
						sz = DIRECT_INGREDIENT_BLOCK_NUM_CL5KJ_BLOCK_SIZE2;
						dataLen += sz + 1;
					}
					else
					{
						sz -= DIRECT_INGREDIENT_BLOCK_NUM_CL5KJ_BLOCK_SIZE1;
						dataLen += sz + 1;
						fullDataLen = dataLen;
						fullDataLen += DIRECT_INGREDIENT_BLOCK_NUM_CL5KJ_BLOCK_SIZE1;
					}
					start_offset = DIRECT_INGREDIENT_BLOCK_NUM_CL5KJ_BLOCK_SIZE1;
					break;
				case 1:
					if(sz>(DIRECT_INGREDIENT_BLOCK_NUM_CL5KJ_BLOCK_SIZE1+2*DIRECT_INGREDIENT_BLOCK_NUM_CL5KJ_BLOCK_SIZE2+1))
					{
						fullDataLen = sz+5;
						sz = DIRECT_INGREDIENT_BLOCK_NUM_CL5KJ_BLOCK_SIZE2;
						dataLen += sz + 1;
					}
					else
					{
						sz -= DIRECT_INGREDIENT_BLOCK_NUM_CL5KJ_BLOCK_SIZE1+DIRECT_INGREDIENT_BLOCK_NUM_CL5KJ_BLOCK_SIZE2;
						dataLen += sz + 1;
						fullDataLen = dataLen;
						fullDataLen += DIRECT_INGREDIENT_BLOCK_NUM_CL5KJ_BLOCK_SIZE1+DIRECT_INGREDIENT_BLOCK_NUM_CL5KJ_BLOCK_SIZE2;
					}
					start_offset = DIRECT_INGREDIENT_BLOCK_NUM_CL5KJ_BLOCK_SIZE1+DIRECT_INGREDIENT_BLOCK_NUM_CL5KJ_BLOCK_SIZE2;
					break;
				case 2:
					if(sz>(2*DIRECT_INGREDIENT_BLOCK_NUM_CL5KJ_BLOCK_SIZE1+2*DIRECT_INGREDIENT_BLOCK_NUM_CL5KJ_BLOCK_SIZE2+1))
					{
						fullDataLen = sz+5;
						sz = DIRECT_INGREDIENT_BLOCK_NUM_CL5KJ_BLOCK_SIZE1;
						dataLen += sz + 1;
					}
					else
					{
						sz -= DIRECT_INGREDIENT_BLOCK_NUM_CL5KJ_BLOCK_SIZE1+2*DIRECT_INGREDIENT_BLOCK_NUM_CL5KJ_BLOCK_SIZE2;
						dataLen += sz + 1;
						fullDataLen = dataLen;
						fullDataLen += DIRECT_INGREDIENT_BLOCK_NUM_CL5KJ_BLOCK_SIZE1+2*DIRECT_INGREDIENT_BLOCK_NUM_CL5KJ_BLOCK_SIZE2;
					}
					start_offset = DIRECT_INGREDIENT_BLOCK_NUM_CL5KJ_BLOCK_SIZE1+2*DIRECT_INGREDIENT_BLOCK_NUM_CL5KJ_BLOCK_SIZE2;
					break;
				}
			ingredient_text_read(&dmsgNo[0], str, start_offset, sz);
			str += sz;
			*str++ = 0x00;	// NULL
#endif
			break;
#ifdef USE_RUSSIA_LOGISTICS_PLU
		case DATA_LOGISTICS_PLU: // Only PLU
			dataLen += LOGISTIC_PLU_STORAGE_SIZE;
			fullDataLen = 4 + LOGISTIC_PLU_STORAGE_SIZE + sz;

//			fullDataLen = dataLen;
			pstr = (HUGEDATA INT8U *)(address+3);
			sendDataLen = LOGISTIC_PLU_STORAGE_SIZE;
			startAddr = address+3;

			Flash_sread(startAddr, str, sendDataLen);
			str += sendDataLen;
			break;
		case DATA_LOGISTICS_INGREDIENT: // Only ingredient
			if(sz>((ingredient_address+1) * DIRECT_INGREDIENT_BLOCK_NUM_CL5KJ_BLOCK_SIZE2 + 1))
			{
				fullDataLen = sz+5;
				sz = DIRECT_INGREDIENT_BLOCK_NUM_CL5KJ_BLOCK_SIZE2;
				dataLen += sz + 1;
			}
			else
			{
				sz -= (ingredient_address) * DIRECT_INGREDIENT_BLOCK_NUM_CL5KJ_BLOCK_SIZE2;
				dataLen += sz + 1;
				fullDataLen = dataLen;
				fullDataLen += (ingredient_address) * DIRECT_INGREDIENT_BLOCK_NUM_CL5KJ_BLOCK_SIZE2;
			}
			start_offset = (ingredient_address) * DIRECT_INGREDIENT_BLOCK_NUM_CL5KJ_BLOCK_SIZE2;
			ingredient_text_read(&dmsgNo[0], str, start_offset, sz);
			str += sz;
			*str++ = 0x00;	// NULL
			break;
#endif	//  USE_RUSSIA_LOGISTICS_PLU
	}
#endif

	buffer[0] = 'W';
	buffer[1] = opCode; //opCode;
	buffer[2]=status_scale.scaleid;
	buffer[3]=0;
	buffer[4]=0;
	buffer[5]=0;
	buffer[6] = ',';
	buffer[7] = (INT8U)fullDataLen;
	buffer[8] = (INT8U)(fullDataLen >> 8);
	buffer[9]=':';

	packetLen = 10 + dataLen ;
	buffer[packetLen++] = ':';

	checksum = 0;
	for(i=2; i<packetLen; i++)
		checksum += buffer[i];

	buffer[packetLen++] = checksum;
	buffer[packetLen++] = 0x0d;//CR;

	*sLength = packetLen;
	return 0;
}

//SG071218
void commun_send_plu_data(HUGEDATA COMM_BUF *CBuf,INT32U nth)
{
	INT8U  checksum;
	INT16U i;
	INT8U errNo;
	INT16U sendLength;
	HUGEDATA INT8U *commun_buffer;

	commun_buffer=serial_send_point;
	sendLength = 0;
	checksum = 0;
	errNo = sendPluData(commun_buffer, &sendLength, nth, 'L');
	if (errNo == 0)
	{
		commun_outleng(CBuf,(char *)commun_buffer,sendLength);
		return;
	}

	// send error
	commun_buffer[0]='N';
	commun_buffer[1]='L'; //plu
	commun_buffer[2]=status_scale.scaleid;
	commun_buffer[3]=0;
	commun_buffer[4]=0;
	commun_buffer[5]=0;
	commun_buffer[6]=',';
	commun_buffer[7]=5;
	commun_buffer[8]=0;
	commun_buffer[9]=':';
//	commun_buffer[10]=0;
//	commun_buffer[11]=0;
//	commun_buffer[12]=0;
//	commun_buffer[13]=0;
	commun_buffer[14]=errNo;
	commun_buffer[15]=':';

	checksum = 0;
	for(i=2; i<16; i++) checksum += commun_buffer[i];
	commun_buffer[16]=checksum;
	commun_buffer[17]=0x0d;//CR;
	commun_outleng(CBuf,(char *)commun_buffer,18);

	return;
}

#ifdef USE_RUSSIA_LOGISTICS_PLU
INT8U commun_send_plu_gtin_data(HUGEDATA COMM_BUF *CBuf,INT32U nth)
{
	INT8U  checksum;
	INT16U i;
	INT8U errNo;
	INT16U sendLength;
	INT16S    com_type;
	INT8U  ret;
	HUGEDATA INT8U *commun_buffer;

	com_type=serial_get_comtype(CBuf);
	commun_buffer=serial_send_point;
	sendLength = 0;
	checksum = 0;
	errNo = sendPluData(commun_buffer, &sendLength, nth, DATA_LOGISTICS_PLU);
	if (errNo == 0)
	{
		commun_outleng(CBuf,(char *)commun_buffer,sendLength);
		return;
	}

SEND_ERR:
	commun_buffer[0]='N';
	commun_buffer[1]=DATA_LOGISTICS_PLU;
	commun_buffer[2]=status_scale.scaleid;
	commun_buffer[3]=0;
	commun_buffer[4]=0;
	commun_buffer[5]=0;
	commun_buffer[6]=',';
	commun_buffer[7]=5;
	commun_buffer[8]=0;
	commun_buffer[9]=':';
//	commun_buffer[10]=0;
//	commun_buffer[11]=0;
//	commun_buffer[12]=0;
//	commun_buffer[13]=0;
	commun_buffer[14]=errNo;
	commun_buffer[15]=':';

	checksum = 0;
	for(i=2; i<16; i++) checksum += commun_buffer[i];
	commun_buffer[16]=checksum;
	commun_buffer[17]=0x0d;//CR;
	commun_outleng(CBuf,(char *)commun_buffer,18);

	return;
}

#endif	// USE_RUSSIA_LOGISTICS_PLU
#endif	// USE_PROTOCOL_CL5000JR

/*
// mode
//  0 :	 선택된 PLU에 대해 모든 scale flag OFF
//  1 :	 선택된 PLU에 대해 모든 scale flag ON
//  2 :	 선택된 PLU에 대해 선택된 scale만 flag OFF
//  3 :	 선택된 PLU에 대해 선택된 scale만 flag ON
//  4 :  선택된 scale에 대해 모든 PLU flag OFF
//  5 :  선택된 scale에 대해 모든 PLU flag ON
//  10 : 선택된 PLU에 대해 선택된 scale의 flag 상태 반환
//  11 : 선택된 PLU에 대해 선택된 scale의 flag 상태 반환. 상태가 OFF이면, ON일때까지 plu search(1000개 내에서)
//  12 : 선택된 scale에 대해 동기 완료 확인
INT8U commun_plu_net_mask(INT16U *address, INT8U scaleListId, INT16S mode)	// scaleListId : 0, 1~32
{
	INT32U addr,i;
	INT16U scaleMask, flag;
	INT16U *pflag;
	INT16U plu_addr;
	INT32U org_addr;
	INT32U max_no;

	org_addr = NVRAM_BASE + NVRAM_PLU_TRANS;
	max_no = MAX_PLU_NO;

	flag = 0;
	if (mode == 99) {	// all clear
		addr = org_addr;
	        flag = 0;

		for (i = 0; i < max_no; i++)
		{
			memcpy((char *)addr, (char *)&flag, 2);
			addr += 2;
		}
		return 0;
	}
	plu_addr = 1;
	if (address)
	{
		plu_addr = *address;
	}
	if (plu_addr > max_no) return 0;

	if (scaleListId == 0) return 0;
	if (mode > 1 && scaleListId > MAX_SLAVE_SCALE) return 0;
	scaleMask = 0x0001;
	scaleMask <<= (scaleListId - 1);
	addr = org_addr;
	if (plu_addr)
	{
		addr += (plu_addr - 1) * 2L;
	}
	while(1)
	{
		memcpy((char *)&flag, (char *)addr, 2);
		if (mode == 11)
		{
			if (flag&scaleMask)
			{
				*address = plu_addr;
				return 1;
			}
			else if (plu_addr >= max_no || plu_addr > *address + 1000)	// 1000개 내에서 search
			{
#ifdef _USE_DBG_PLU_SYNC
//sprintf(MsgBuf, "[PS]<END>%d\r\n", scaleListId);
//MsgOut(MsgBuf);
#endif
				*address = plu_addr;
				return 0;	// end
			}
			addr += 2;
			(plu_addr)++;
		}
		else if (mode == 10)
		{
			if (flag&scaleMask)
			{
				return 1;
			}
			else return 0;
		}
		else if (mode == 12)
		{
			if (flag&scaleMask)
			{
				return 0;	// not synchronized
			}
			else if (plu_addr >= max_no)
			{
				return 1;	// synchronized
			}
			addr += 2;
			plu_addr++;
		}
		else
		{
			break;
		}
	}

	switch (mode)
	{
		case 0: // Clear All Scale
			flag = 0;
			break;
		case 1: // Set All Scale
			flag = 0xffff;
			break;
		case 2: // Clear Scale
			flag &= ~scaleMask;
			break;
		case 3: // Set Scale
			flag |= scaleMask;
			break;
		case 4:	// Clear PLU
			pflag = (INT16U *)addr;
			for (i = 0; i < max_no; i++)
			{
				*pflag &= ~scaleMask;
				pflag++;
			}
			break;
		case 5:	// Set PLU
			pflag = (INT16U *)addr;
			for (i = 0; i < max_no; i++)
			{
				*pflag |= scaleMask;
				pflag++;
			}
			break;


//		case 10: // Get
//			if (flag&scaleMask) return 1;
//			else return 0;
//		case 11: // Get and find
//			if (flag&scaleMask) return 1;
//			else return 0;
	}
	memcpy((char *)addr, (char *)&flag, 2);
	return 0;
}


// mode
//  0 :	 선택된 D/C에 대해 모든 scale flag OFF
//  1 :	 선택된 D/C에 대해 모든 scale flag ON
//  2 :	 선택된 D/C에 대해 선택된 scale만 flag OFF
//  3 :	 선택된 D/C에 대해 선택된 scale만 flag ON
//  4 :  선택된 scale에 대해 모든 D/C flag OFF
//  5 :  선택된 scale에 대해 모든 D/C flag ON
//  10 : 선택된 D/C에 대해 선택된 scale의 flag 상태 반환
//  11 : 선택된 D/C에 대해 선택된 scale의 flag 상태 반환. 상태가 OFF이면, ON일때까지 d/c search
//  12 : 선택된 scale에 대해 동기 완료 확인
//  98 : all set
//  99 : all clear
INT16U commun_dc_net_mask(INT16U *dc_no, INT8U scaleListId, INT16S mode)
{
	INT32U addr,i;
	INT16U scaleMask, flag;
	INT16U *pflag;
	INT16U dc_addr;
	INT32U org_addr;
	INT32U max_no;

	org_addr = NVRAM_BASE + NVRAM_DC_SYNC_FLAG;
	max_no = MAX_DISCOUNT_NO;

	flag = 0;
	if (mode == 99 || mode == 98) {	// 99:all clear, 98:all set
		addr = org_addr;
	        flag = 0;
		if (mode == 98) flag = 0xffff;
		for (i = 0; i < max_no; i++)
		{
			memcpy((char *)addr, (char *)&flag, 2);
			addr += 2;
		}
		return 0;
	}
	dc_addr = 1;
	if (dc_no)
	{
		dc_addr = *dc_no;
	}
	if (dc_addr > max_no) return 0;

	if (scaleListId == 0) return 0;
	if (mode > 1 && scaleListId > MAX_SLAVE_SCALE) return 0;
	scaleMask = 0x0001;
	scaleMask <<= (scaleListId - 1);
	addr = org_addr;
	if (dc_addr)
	{
		addr += (dc_addr - 1) * 2L;
	}
	while(1)
	{
		memcpy((char *)&flag, (char *)addr, 2);
		if (mode == 11)
		{
			if (flag&scaleMask)
			{
				*dc_no = dc_addr;
				return 1;
			}
			else if (dc_addr >= max_no)
			{
#ifdef _USE_DBG_PLU_SYNC
//sprintf(MsgBuf, "[PS]<END>%d\r\n", scaleListId);
//MsgOut(MsgBuf);
#endif
				*dc_no = dc_addr;
				return 0;	// end
			}
			addr += 2;
			dc_addr++;
		}
		else if (mode == 10)
		{
			if (flag&scaleMask)
			{
				return 1;
			}
			else return 0;
		}
		else if (mode == 12)
		{
			if (flag&scaleMask)
			{
				return 0;	// not synchronized
			}
			else if (dc_addr >= max_no)
			{
				return 1;	// synchronized
			}
			addr += 2;
			dc_addr++;
		}
		else
		{
			break;
		}
	}

	switch (mode)
	{
		case 0: // Clear All Scale
			flag = 0;
			break;
		case 1: // Set All Scale
			flag = 0xffff;
			break;
		case 2: // Clear Scale
			flag &= ~scaleMask;
			break;
		case 3: // Set Scale
			flag |= scaleMask;
			break;
		case 4:	// Clear DC
			pflag = (INT16U *)addr;
			for (i = 0; i < max_no; i++)
			{
				*pflag &= ~scaleMask;
				pflag++;
			}
			break;
		case 5:	// Set DC
			pflag = (INT16U *)addr;
			for (i = 0; i < max_no; i++)
			{
				*pflag |= scaleMask;
				pflag++;
			}
			break;
	}
	memcpy((char *)addr, (char *)&flag, 2);
	return 0;
}
*/

// mode
//  0 :	 선택된 PLU(D/C)에 대해 모든 scale flag OFF
//  1 :	 선택된 PLU(D/C)에 대해 모든 scale flag ON
//  2 :	 선택된 PLU(D/C)에 대해 선택된 scale만 flag OFF
//  3 :	 선택된 PLU(D/C)에 대해 선택된 scale만 flag ON
//  4 :  선택된 scale에 대해 모든 PLU(D/C) flag OFF
//  5 :  선택된 scale에 대해 모든 PLU(D/C) flag ON
//  10 : 선택된 PLU(D/C)에 대해 선택된 scale의 flag 상태 반환
//  11 : 선택된 PLU(D/C)에 대해 선택된 scale의 flag 상태 반환. 상태가 OFF이면, ON일때까지 PLU(D/C) search
//  12 : 선택된 scale에 대해 동기 완료 확인
//  98 : all clear
//  99 : all set
INT16U commun_net_mask(INT8U data_type, INT16U *dc_no, INT8U scaleListId, INT16S mode)
{
	INT32U addr,i;
	INT16U scaleMask, flag;
	//INT16U *pflag;
	INT16U wflag;
	INT16U dc_addr;
	INT32U org_addr;
	INT32U max_no;
	INT32U async_cnt;
	INT8U temp;

#ifdef USE_DISCOUNT_QTY_EXT
	/* DC 관련 100개 이상은 NVRAM 영역에 할당해놓지 않았으므로 처리안함 */
	if (dc_no && *dc_no > DISCOUNT_QTY_2M) {
		return 0;
	}
#endif /* USE_DISCOUNT_QTY_EXT */

	switch (data_type)
	{
		case 0:	// PLU
			org_addr = NVRAM_PLU_TRANS;
			max_no = MAX_PLU_NO;
			if (max_no > MAX_PLU_ADDRESS) max_no = MAX_PLU_ADDRESS;
			break;
		case 1: // D/C
			org_addr = NVRAM_DC_SYNC_FLAG;
			max_no = MAX_DISCOUNT_NO - 1;
			break;
		case 2: // ingredient
			org_addr = NVRAM_ING_SYNC_FLAG;
			max_no = MAX_INGREDIENT_NO;
			if (max_no > 1000) max_no = 1000;
			break;
		case 3: // scroll message
			org_addr = NVRAM_SCROLL_SYNC_FLAG;
			max_no = MAX_SCROLL_NO;
			break;
		default:
			return 0;
	}

	flag = 0;
	temp = 0;
	if (mode == 99 || mode == 98) {	// 98:all clear, 99:all set
		addr = org_addr;
		if (mode == 99)
		{
			flag = 0xffff;
			temp = 0xff;
		}
//		for (i = 0; i < max_no; i++)
//		{
//			//memcpy((char *)addr, (char *)&flag, 2);
//			set_nvram_wparam(addr, flag);
//			addr += 2;
//		}
		nvram_set(addr, temp, max_no*2);

		return 0;
	}
	dc_addr = 1;
	if (dc_no)
	{
		dc_addr = *dc_no;
	}
	if (dc_addr > max_no) return 0;

	if (scaleListId == 0) return 0;
	if (mode > 1 && scaleListId > MAX_SLAVE_SCALE) return 0;
	scaleMask = 0x0001;
	scaleMask <<= (scaleListId - 1);
	addr = org_addr;
	if (dc_addr)
	{
		addr += (dc_addr - 1) * 2L;
	}
	async_cnt = 0;
	while(1)
	{
		//memcpy((char *)&flag, (char *)addr, 2);
		flag = get_nvram_wparam(addr);
		if (mode == 11)
		{
			if (dc_addr > max_no)
			{
#ifdef _USE_DBG_PLU_SYNC
//sprintf(MsgBuf, "[PS]<END>%d\r\n", scaleListId);
//MsgOut(MsgBuf);
#endif
				*dc_no = dc_addr;
				return 0;	// end
			}
			else if (flag&scaleMask)
			{
				*dc_no = dc_addr;
				return 1;
			}
			addr += 2;
			dc_addr++;
		}
		else if (mode == 10)
		{
			if (flag&scaleMask)
			{
				return 1;
			}
			else return 0;
		}
		else if (mode == 12)
		{
			if (dc_addr > max_no)
			{
				return 1;	// synchronized
			}
			else if (flag&scaleMask)
			{
				return 0;	// not synchronized
			}
			addr += 2;
			dc_addr++;
		}
		else if (mode == 14)
		{
			if (dc_addr > max_no)
			{
				return async_cnt;
			}
			else if (flag&scaleMask)	// not synchronized
			{
				async_cnt++;
			}
			addr += 2;
			dc_addr++;
		}
		else
		{
			break;
		}
	}

	switch (mode)
	{
		case 0: // Clear All Scale
			flag = 0;
			break;
		case 1: // Set All Scale
			flag = 0xffff;
			break;
		case 2: // Clear Scale
			flag &= ~scaleMask;
			break;
		case 3: // Set Scale
			flag |= scaleMask;
			break;
		case 4:	// Clear DC
			//pflag = (INT16U *)addr;
			for (i = 0; i < max_no; i++)
			{
				//*pflag &= ~scaleMask;
				//pflag++;
				wflag = get_nvram_wparam(addr+(i*2));
				set_nvram_wparam(addr+(i*2), (wflag&(~scaleMask)));
			}
			return 0;
		case 5:	// Set DC
			//pflag = (INT16U *)addr;
			for (i = 0; i < max_no; i++)
			{
				//*pflag |= scaleMask;
				//pflag++;
				wflag = get_nvram_wparam(addr+(i*2));
				set_nvram_wparam(addr+(i*2), (wflag|scaleMask));
			}
			return 0;
	}
	//memcpy((char *)addr, (char *)&flag, 2);
	set_nvram_wparam(addr, flag);
	return 0;
}

#ifdef USE_PROTOCOL_CL5000JR
INT8U commun_delete_plu_data(HUGEDATA COMM_BUF *CBuf,INT8U *str, INT16S leng) //,INT8U mode
{
	ETH_CONNECT_LIST eth;
	INT8U ret;
	INT16U deptno;
	INT32U pluno;
	INT16U addr;

	memcpy(&deptno,&str[0],2);
	memcpy(&pluno,&str[2],4);
	pluno &= 0xffffff;

	memset((INT8U *)&eth,0,sizeof(ETH_CONNECT_LIST));

	ret=0;
	if(deptno==0 && pluno==0)  //delete plu all
	{
		//plu_all_delete(CBuf,9);
		plu_all_delete(CBuf,10);	// CJK090117
		if(status_scale.cur_pluid) sale_pluclear(ON);
		sale_display_update(0x0fff);
		return ret;
	}

	if (deptno<1) return 0x82;
	if (pluno<1 || pluno>MAX_PLU_NUMBER)  return 0x82;
	plu_table_search((unsigned char)deptno,pluno,&addr,0);

	if (addr) {
		plu_delete((unsigned char)deptno,pluno,1);
		ret=0;
	} else {
		ret=98;
	}

	return ret;
}
#endif

INT8U  table3_delete_any(INT8U part,INT8U mode,INT16U id)
{
	INT16U max_id;
	INT16U i,sz;
	INT32U p,rp;
	INT8U temp[32];
	INT16U start;

	if (part<0) return 0x82;
	sz = 0;
	p  = 0;
	start=0;
	switch (part) {
	    case 1: max_id=MAX_STORE_NO;		// Store
		    sz= sizeof(STORE_NAME_STRUCT);
		    p=FLASH_SHOPNAME_AREA;
		    break;
 	    case 2: //max_id=100;		// Customer
		    max_id=MAX_CUSTOMER_NO;		// Customer	// Modified by CJK 20060608
		    sz = CUSTOMER_STRUCT_SIZE;
		    p= FLASH_CUSTOMER_AREA;
		    break;
	    case 3: //max_id=5;		// Scroll Message
		    max_id=MAX_SCROLL_NO;		// Scroll Message
		    sz=sizeof(SCROLL_MESSAGE_STRUCT);
		    p=FLASH_SCROLL_AREA;
		    start=0;
		    break;
	    case 4: max_id=MAX_CLERK_NO-1;	// User/Security
		    sz=CLERK_STRUCT_SIZE;
		    p=FLASH_CLERK_AREA;
		    start=2;
		    if (mode) {
		    	if (id==1) return 0x86;
		    }
		    break;
	    case 5: max_id=MAX_CURRENCY_NO;	// Currency
		    sz=sizeof(CURRENCY_STRUCT);
		    p=FLASH_CURRENCY_AREA;
		    start=2;
		    if (mode) {
		    	if (id==1) return 0x86;
		    }
		    break;
	    //SCANNER
	    case 6:
		    max_id=SCAN_BACODE_DATA_MAX_NUM;
		    sz=SCANNER_CONV_TABLE_STR_SIZE;
		    p=FLASH_SCANNER_TABLE_AREA;
		    start=0;
		    break;

	}
	if (id>max_id) return 0x99;
#ifdef USE_10000_SCANNER_CONVTABLE
	if(p || part == 6) {
		if(part == 6)
			p+=EXT_MEMCARD2_ADDR_X;
		else
			p+=DFLASH_BASE;
		memset(temp,0,32);
		if (mode) {
			rp=0;
			if (id)
			{
				if ((part==2) || (part==4)) // Customer, Clerk
					rp = id*sz;	// 1.. max_id
				else
					rp = (id-1)*sz;	// 0.. max_id-1
			}
			p+=rp;
			Flash_swrite(p,temp,2);
		} else {
		    for (i=0; i<start; i++) {
			p+=(INT32U)sz;
		    }
		    for (i=start; i<max_id; i++) {	// 0.. max_id-1
			Flash_swrite(p,temp,2);
			p+=(INT32U)sz;
		    }
		}
		return 1;
	}
#else
	if (p) {
		p+=DFLASH_BASE;
		memset(temp,0,32);
		if (mode) {
			rp=0;
			if (id)
			{
				if ((part==2) || (part==4)) // Customer, Clerk
					rp = id*sz;	// 1.. max_id
				else
					rp = (id-1)*sz;	// 0.. max_id-1
			}
			p+=rp;
			Flash_swrite(p,temp,2);
		} else {
		    for (i=0; i<start; i++) {
			p+=(INT32U)sz;
		    }
		    for (i=start; i<max_id; i++) {	// 0.. max_id-1
			Flash_swrite(p,temp,2);
			p+=(INT32U)sz;
		    }
		}
		return 1;
	}
#endif
	return 0x82;
}

INT8U  table2_delete_any(INT8U part,INT8U mode,INT16U id)
{
	INT16U max_id;
	INT16U i,sz;
	INT32U p,rp;
	INT8U temp[32];
	INT16U start;
	INT8U ucTemp, nutri_type=0;

	if (part<0) return 0x82;

	ucTemp = get_global_bparam(GLOBAL_SALE_SETUP_FLAG9);
	// nutri_type (0:normal, 1:CHL, 2:CAN, 3:AUS, 4:NZL, 5: AUS for actual weight, 6: NZL for actual weight)
	nutri_type = ucTemp&0x0f;

	sz = 0;
	p  = 0;
	start=0;
	switch (part)
	{
		case TABLE2_INGREDIENT:
#ifdef USE_INGREDIENT_CODE_4DIGIT
			max_id=MAX_INGREDIENT_INDEX_NUMBER;		// Ingredient id 1.. 10000
#else
			max_id=MAX_INGREDIENT_NO;		// Ingredient id 1.. 10000
#endif
		    sz=INGREDIENT_STRUCT_SIZE;
		    p=FLASH_INGREDIENT_AREA;
		    break;
#ifdef USE_NUTRITION_FACT
		case TABLE2_NUTRITION_FACT:
			if (nutri_type == NUTRI_TYPE_2COL_6DIG_AUS || nutri_type == NUTRI_TYPE_2COL_6DIG_AUS_ACTUAL_WEIGHT
				|| nutri_type == NUTRI_TYPE_6DIG_CZE)
			{
				max_id=MAX_NUTRIFACT2_NO;
				sz = sizeof(NUTRIFACT_STRUCT2);
			}
#ifdef USE_EU_NUTRITIONFACT
			else if (nutri_type == NUTRI_TYPE_42ITEM_5DIG_EU_ACTUAL_WEIGHT)
			{
				max_id=MAX_NUTRIFACT3_NO;
				sz = sizeof(NUTRIFACT_STRUCT3);
			}
#endif
#ifdef USE_ADDITIONAL_NUTRIENT
			if (nutri_type == NUTRI_TYPE_2COL_USA)
			{
				max_id=MAX_NUTRIFACT4_NO;
				sz = sizeof(NUTRIFACT_STRUCT4);
			}
#endif
			else
			{
				max_id=MAX_NUTRIFACT_NO;
				sz = sizeof(NUTRIFACT_STRUCT);
			}
		    p= FLASH_NUTRIFACT_AREA;
		    break;
#endif
#ifdef USE_TRACEABILITY //gm 090407, memory 충돌방지
	    case TABLE2_TRACEABILITY:
			max_id=MAX_TRACEABILITY_NO-1;
		    sz=TRACEABILITY_STRUCT_SIZE;
		    p=FLASH_TRACEABILITY_AREA;
		    break;
	    case TABLE2_COUNTRY:
			max_id=MAX_COUNTRY_NO-1;
		    sz=COUNTRY_STRUCT_SIZE;
		    p=FLASH_COUNTRY_AREA;
		    break;
	    case TABLE2_SLAUGHTERHOUSE:
			max_id=MAX_SLAUGHTERHOUSE_NO-1;
		    sz=SLAUGHTERHOUSE_STRUCT_SIZE;
		    p=FLASH_SLAUGHTERHOUSE_AREA;
		    break;
	    case TABLE2_CUTTINGHALL:
			max_id=MAX_CUTTINGHALL_NO-1;
		    sz=CUTTINGHALL_STRUCT_SIZE;
		    p=FLASH_CUTTINGHALL_AREA;
		    break;
#endif
#ifdef USE_TRACE_STANDALONE
	    case TABLE2_INDIVIDUAL:
			max_id=MaxIndividualNo-1;//MAX_INDIVIDUAL_NO;
		    sz=TRACE_INDIVIDUAL_IDENTITY_SIZE;
		    p=FLASH_TRACE_STANDALONE_AREA;
		    break;
	    case TABLE2_SPECIALUSE:
			max_id=MAX_SPECIALUSE_NO-1;
		    sz=TRACE_SPECIALUSE_STRUCT_SIZE;
		    p=FLASH_TRACE_SPECIALUSE_AREA;
		    break;
	    case TABLE2_SLAUGHTERHOUSE_KR: 
			max_id=MAX_SLAUGHTERHOUSE_KR_NO-1;
		    sz=TRACE_SLAUGHTERHOUSE_KR_STRUCT_SIZE;
		    p=FLASH_TRACE_SLAUGHTERHOUSE_AREA;
		    break;
#ifdef USE_TRACE_MEATPART
	    case TABLE2_MEATPART:
			max_id=MAX_MEATPART_NO-1;
		    sz=TRACE_MEATPART_STRUCT_SIZE;
		    p=FLASH_TRACE_MEATPART_AREA;
		    break;
#endif
#endif

	}
	if (id>max_id) return 0x99;
	if (p)
	{
#ifdef USE_TRACE_1000QTY //sewon 161205
		switch (part) 
		{
			case TABLE2_INGREDIENT:
				p+=DFLASH_BASE;
				break;
  #ifdef USE_NUTRITION_FACT				
			case TABLE2_NUTRITION_FACT: 
    #ifdef USE_NORTH_AMERICA_INGREDIENT
				p+=EXT_MEMCARD2_ADDR_X;
	#else
				p+=DFLASH_BASE;
	#endif
				break;
  #endif
  #ifdef USE_TRACEABILITY
			case TABLE2_TRACEABILITY: 
				p+=DFLASH_BASE;
				break;
			case TABLE2_COUNTRY: 
				p+=DFLASH_BASE;
				break;
			case TABLE2_SLAUGHTERHOUSE: 
				p+=DFLASH_BASE;
				break;
			case TABLE2_CUTTINGHALL: 
				p+=DFLASH_BASE;
				break;
  #endif
  #ifdef USE_TRACE_STANDALONE
			case TABLE2_INDIVIDUAL: 
				p+=EXT_MEMCARD_ADDR;
				break;
			case TABLE2_SPECIALUSE: 
				p+=DFLASH_BASE;
				break;
			case TABLE2_SLAUGHTERHOUSE_KR: 
				p+=DFLASH_BASE;
				break;
    #ifdef USE_TRACE_MEATPART				
			case TABLE2_MEATPART: 
				p+=DFLASH_BASE;
				break;
    #endif
  #endif
			default:
				p+=DFLASH_BASE;
				break;
		}
#else //#ifdef USE_TRACE_1000QTY
		p+=DFLASH_BASE;
#endif //#ifdef USE_TRACE_1000QTY
		memset(temp,0,32);
		if (mode)
		{	// delete one
			rp=0;
			if (part==1)
			{
#ifdef USE_9999_INDIRECT_INGREDIENT
				id = ingredient_fine_table_set_value(id,1,INGREDIENT_EXIST_MASK);
				// Can't find same ingredient
				if(id==0)return 1;
				p = ing_calc_addr_by_key(id-1);
#elif defined(USE_INGREDIENT_CODE_4DIGIT)
				ingredient_del_id(id);
				return 1;
#else
				p = ing_calc_addr_by_key(id-1);
#endif
			}
#ifdef USE_KOR_SLAUGHT_CODE_4DIGIT
			else if (part==9)
			{
				trace_slaughthousedata_delete(id);
				return 1;
			}
#endif
			else
			{
				if (id) rp = (INT32U)(id-1)*sz;
				p+=rp;
			}
			Flash_swrite(p,temp,2);
		}
		else
		{	// delete all
			if (part==1)
			{
			   ingredient_delete_all();
			}
#ifdef USE_KOR_SLAUGHT_CODE_4DIGIT
			else if (part==9)
			{
			   trace_slaughthousedata_delete_all();
			}
#endif
			else
			{
				for (i=0; i<start; i++)
				{
					p+=(INT32U)sz;
				}
				for (i=start; i<max_id; i++)
				{
					Flash_swrite(p,temp,2);
					p+=(INT32U)sz;
			    }
		    }
		}
		return 1;
	}
	return 0x82;
}

INT8U  table1_delete_any(INT8U part,INT8U mode,INT16U id)
{
	INT16U max_id;
	INT16U i,sz;
	INT16U start;
	INT32U p,rp;
	INT8U temp[32];

	start=0;
	switch (part) {
	    case 1: max_id=MAX_DEPT_NO-1;
		    sz=DEPARTMENT_STRUCT_SIZE;
		    p=FLASH_DEPARTMENT_AREA;
		    if (mode) {
		    	if (id==1) return 0x86;
		    }
//		    start=1;
		    break;
 	    case 2: max_id=MAX_GROUP-1;		// id = 1.. 99
		    sz = GROUP_STRUCT_SIZE;
		    p= FLASH_SUB_DEPARTMENT_AREA;
		    break;
	    case 3: max_id=MAX_LABEL_MSG_NO-1;	// id = 1.. 99
		    sz=PRT_ADVE_STRUCT_SIZE;
		    p=FLASH_PRT_ADVE_AREA;
		    break;
	    case 4: max_id=MAX_ORIGIN_NO;	// id = 1.. 500
		    sz=ORIGIN_STRUCT_SIZE;
		    p=FLASH_ORIGIN_AREA;
		    break;
	    case 5: max_id=MAX_PCS_NO;
		    sz=PCS_UNIT_STRUCT_SIZE;
		    p=FLASH_PCS_UNIT_AREA;
		    break;
#ifdef USE_TAX_RATE
	    case 6: max_id=MAX_TAX_NO-1;	// id = 1..  9
		    sz=TAX_STRUCT_SIZE;
		    p=FLASH_TAX_AREA;
		    break;
#endif
	    case 7: max_id=MAX_TARE_NO;		// id = 1.. 99
		    sz=TARE_STRUCT_SIZE;
		    p=FLASH_TARE_AREA;
		    break;
	    case 8:
            max_id = MAX_BARCODE_NO;
			sz=BARCODE_STRUCT_SIZE;
			p=FLASH_BARCODE_FORMAT_AREA;
		    break;
	    default : return 0x82;
	}
#ifdef USE_TRACE_MEATGRADE
	if (part == 2) // group(grade)
	{
		trace_meatgrade_getGradeListText();
	}
#endif
	if (id>max_id) return 0x99;
	if (p) {
	        memset(temp,0,32);
		p+=DFLASH_BASE;
		if (mode) {
			rp=0;
			if (id) {
				rp = (id-1);
				rp*= sz;
			}
			p+=rp;
			Flash_swrite(p,temp,2);
		} else {
		    for (i=0; i<start; i++) {
			p+=(INT32U)sz;
		    }
		    for (i=start; i<max_id; i++) {
			Flash_swrite(p,temp,2);
			p+=(INT32U)sz;
		    }
		}
		return 1;
	}
	return 0x82;
}

#ifdef USE_CTS_FUNCTION
INT8U CheckDecryptCalKey(INT32U rcvKey, INT16U *period)
{
	INT32U temp;
	INT32U prd;

	temp = rcvKey;

	temp ^= (CtsEncryptBufStruct.time * CtsStruct2.EncryptKey1);
	temp ^= (CtsEncryptBufStruct.zero * K_ZERO);
	temp ^= (CtsEncryptBufStruct.span * K_SPAN);

	if (!(temp^CAL_KEY)) return ENCRYPT_CALENT;
	else if (!(temp^CTSON_KEY)) return ENCRYPT_CTSON;
	else if (!(temp^CTSOFF_KEY)) return ENCRYPT_CTSOFF;
	else if (!(temp^CALCOMP_KEY)) return ENCRYPT_CALCOMP;
	else
	{
		prd = temp ^ PERIOD_KEY;
		if (prd < 0x10000)
		{
			*period = (INT16U)prd;
			return ENCRYPT_PERIOD;
		}
		else return ENCRYPT_NONE;
	}

}
#endif
#ifdef USE_PROTOCOL_CL5000JR
//SG071203. CL5000Jr Protocol
extern INT16U ingredient_text_check_empty_count(INT16U qty);
INT8U commun_write_plu_data(HUGEDATA COMM_BUF *CBuf,INT8U *str,INT16S leng)
{
	PLU_INF *pluStr;
	INT16S    ret;
	INT32U pluno;
	INT32U key;
	INT16U  deptno;
	INT16U idx;
    INT16U iSize;
#ifdef USE_DIRECT_INGREDIENT
	INT16U dmsgNo[DIRECT_INGREDIENT_BLOCK_NUM];
	INT8U tempBuf[16];
	INT16U use_block;
	INT16U total_block;
#endif
	INT32U fl_pnt;
	INT32U dmsg_addr;
	INT8U being;
	INT8U i;
	INT16U cur_size;
	INT16U  dmsgSize = 0;
	INT8U ing_ret;
	INT16S pluPart1Length;
#ifdef USE_RUSSIA_LOGISTICS_PLU
	INT8U plu_reserved[(LOGISTIC_PLU_STORAGE_SIZE-PLU_STORAGE_SIZE)];
#endif
	if(leng > PLU_STORAGE_SIZE + DIRECT_INGREDIENT_BLOCK_NUM_CL5KJ_BLOCK_SIZE1)
	{
		leng = PLU_STORAGE_SIZE + DIRECT_INGREDIENT_BLOCK_NUM_CL5KJ_BLOCK_SIZE1;
	}
	dmsgSize = 0;
	if(leng > PLU_STORAGE_SIZE) dmsgSize = leng - PLU_STORAGE_SIZE;

	//CL-Works에서 Ingredient msg 마지막에 Null 1byte 추가하여 전송함.
	//ex)dmsg 150byte 전송 시 dmsgSize에 Null이 추가되어 totalblock을 1이 아닌 2로 인식하는 경우가 발생
	if ((dmsgSize % (DEFAULT_INGREDIENT_TEXT_SIZE - 2) == 1) && (str[leng-1] == 0))
	{
		dmsgSize--;
	}
	recv_counter++;

	pluStr = (PLU_INF *)str;
	deptno = pluStr->deptNo;
	pluno  = (INT32U)pluStr->pluno;


	if (Startup.disable_dept) deptno = 1;
	if ( (pluno <=0) || (pluno>MAX_PLU_NUMBER) ) return 82;
	if ( (deptno<=0) || (deptno>=100) ) return 82;
	if ( leng < 0) return 84;

	ret = 0;

	being = plu_table_search((INT8U)deptno,pluno,(INT16U *)&idx,0);
	if (!being)
	{
		key = plu_address_get_new();
		if (key==0) return 89;
	}
	else
	{
		key = (INT32U)idx;
	}

	fl_pnt = plu_calc_addr_by_key(key-1);
	fl_pnt+=3;
#ifdef USE_RUSSIA_LOGISTICS_PLU
	dmsg_addr = fl_pnt + LOGISTIC_PLU_STORAGE_SIZE;
#else
	dmsg_addr = fl_pnt + PLU_STORAGE_SIZE;
#endif
	iSize = PLU_STORAGE_SIZE;

	//dmsgNo=0;
#ifdef USE_DIRECT_INGREDIENT
	if(being)
	{
		Flash_sread(dmsg_addr, (HUGEDATA INT8U *)&dmsgNo[0], 2*DIRECT_INGREDIENT_BLOCK_NUM);
	}
	else
	{
	  	memset((HUGEDATA INT8U *)&dmsgNo[0], 0, 2*DIRECT_INGREDIENT_BLOCK_NUM);
	}
	if (dmsgSize > 0)
	{
		total_block = (dmsgSize - 1) / (DEFAULT_INGREDIENT_TEXT_SIZE - 2);
		total_block++;
	}
	else
	{
		total_block = 0;
	}
	use_block = 0;
	for (i = 0 ; i < DIRECT_INGREDIENT_BLOCK_NUM ; i++)
	{
		if (dmsgNo[i]) use_block++;
	}
	// check empty size
	if (total_block > use_block)
	{
		if (!ingredient_text_check_empty_count(total_block-use_block)) return 0x88;	// lack of space
	}

	if (dmsgSize)
	{
#ifdef USE_RUSSIA_LOGISTICS_PLU
		//ing_ret = ingredient_text_write(&dmsgNo[0], &str[PLU_STORAGE_SIZE], 0, DIRECT_INGREDIENT_BLOCK_NUM_CL5KJ_BLOCK_SIZE1);
        ing_ret = ingredient_text_write(&dmsgNo[0], &str[PLU_STORAGE_SIZE], 0, DIRECT_INGREDIENT_BLOCK_NUM_CL5KJ_BLOCK_SIZE1, total_block);
#else
		ing_ret = ingredient_text_write(&dmsgNo[0], &str[PLU_STORAGE_SIZE], 0, DIRECT_INGREDIENT_BLOCK_NUM_CL5KJ_BLOCK_SIZE1, total_block);
#endif
		Flash_swrite(dmsg_addr, (HUGEDATA INT8U *)&dmsgNo[0], 2*DIRECT_INGREDIENT_BLOCK_NUM);
		if (!ing_ret) return 0x88;	//direct ingredient memory full
	}
	else
	{
		tempBuf[0] = 0;
		//ingredient_text_write(&dmsgNo[0], tempBuf, 0, DEFAULT_INGREDIENT_TEXT_ALL_SIZE, 1);
		ingredient_text_write(&dmsgNo[0], tempBuf, 0, DEFAULT_INGREDIENT_TEXT_ALL_SIZE, total_block);
		Flash_swrite(dmsg_addr, (HUGEDATA INT8U *)&dmsgNo[0], 2*DIRECT_INGREDIENT_BLOCK_NUM);
	}

#endif
	Flash_swrite(fl_pnt,(HUGEDATA INT8U *)str,PLU_STORAGE_SIZE);
#ifdef USE_RUSSIA_LOGISTICS_PLU
	memset(plu_reserved, 0, (LOGISTIC_PLU_STORAGE_SIZE-PLU_STORAGE_SIZE));
	Flash_swrite(fl_pnt+PLU_STORAGE_SIZE, plu_reserved, (LOGISTIC_PLU_STORAGE_SIZE-PLU_STORAGE_SIZE));
#endif

	if (network_status.service_flag&0x20)	// send plu
	{
		if (ethernet_list.status == 1)
		{
			commun_net_mask(0, (INT16U *)&key, 0xff, 1);
		}
	}

	plu_table_save((INT8U)deptno,pluno);

	return ret;
}
#ifdef USE_RUSSIA_LOGISTICS_PLU
//SG071203. CL5000Jr Protocol
INT8U commun_write_plu_gtin_data(HUGEDATA COMM_BUF *CBuf,INT8U *str,INT16S leng)
{
	PLU_INF *pluStr;
	INT16S    ret;
	INT32U pluno;
	INT32U key;
	INT16U  deptno;
	INT16U idx;
	INT16U iSize;
	char  *commun_buffer;
	INT32U fl_pnt;
	INT8U being;
	INT8U i;
	INT16U cur_size;
	INT16U  dmsgSize = 0;
	INT8U ing_ret;

	recv_counter++;

	pluStr = (PLU_INF *)str;
	deptno = pluStr->deptNo;
	pluno  = (INT32U)pluStr->pluno;

	commun_buffer=(char *)serial_send_point;

	if (Startup.disable_dept) deptno = 1;
	if ( (pluno <=0) || (pluno>MAX_PLU_NUMBER) ) return 82;
	if ( (deptno<=0) || (deptno>=100) ) return 82;

	ret = 0;

	being = plu_table_search((INT8U)deptno,pluno,(INT16U *)&idx,0);
	if (!being)
	{
		key = plu_address_get_new();
		if (key==0) return 89;
	}
	else
	{
		key = (INT32U)idx;
	}

	fl_pnt = plu_calc_addr_by_key(key-1);
	fl_pnt+=3;

	Flash_swrite(fl_pnt,(HUGEDATA INT8U *)str,LOGISTIC_PLU_STORAGE_SIZE);

	if (network_status.service_flag&0x20)	// send plu
	{
		if (ethernet_list.status == 1)
		{
			commun_net_mask(0, (INT16U *)&key, 0xff, 1);
		}
	}

	plu_table_save((INT8U)deptno,pluno);

	return ret;
}

#endif
#endif

void  commun_send_saleinfo(HUGEDATA COMM_BUF *CBuf)
{
	char temp[68];

	if (status_scale.Plu.PluCode) {
		sprintf(temp,"T01:P=%ld.",status_scale.Plu.PluCode);
		commun_out(CBuf,temp);
		sprintf(temp,"U=%ld.",status_scale.Plu.unitprice);
		commun_out(CBuf,temp);
	} else {
		sprintf(temp,"T02:P=0.");
		commun_out(CBuf,temp);
		sprintf(temp,"U=%ld.",status_scale.Plu.unitprice);
		commun_out(CBuf,temp);
	}
	sprintf(temp,"W=%ld.",status_scale.Weight);
	commun_out(CBuf,temp);
	sprintf(temp,"T=%ld.%c",status_scale.TotalPrice,0x0a);
	commun_out(CBuf,temp);
}

static unsigned short crc_table[256] = {0x0000,
	0x8005, 0x800F, 0x000A, 0x801B, 0x001E, 0x0014, 0x8011,
	0x8033, 0x0036, 0x003C, 0x8039, 0x0028, 0x802D, 0x8027,
	0x0022, 0x8063, 0x0066, 0x006C, 0x8069, 0x0078, 0x807D,
	0x8077, 0x0072, 0x0050, 0x8055, 0x805F, 0x005A, 0x804B,
	0x004E, 0x0044, 0x8041, 0x80C3, 0x00C6, 0x00CC, 0x80C9,
	0x00D8, 0x80DD, 0x80D7, 0x00D2, 0x00F0, 0x80F5, 0x80FF,
	0x00FA, 0x80EB, 0x00EE, 0x00E4, 0x80E1, 0x00A0, 0x80A5,
	0x80AF, 0x00AA, 0x80BB, 0x00BE, 0x00B4, 0x80B1, 0x8093,
	0x0096, 0x009C, 0x8099, 0x0088, 0x808D, 0x8087, 0x0082,
	0x8183, 0x0186, 0x018C, 0x8189, 0x0198, 0x819D, 0x8197,
	0x0192, 0x01B0, 0x81B5, 0x81BF, 0x01BA, 0x81AB, 0x01AE,
	0x01A4, 0x81A1, 0x01E0, 0x81E5, 0x81EF, 0x01EA, 0x81FB,
	0x01FE, 0x01F4, 0x81F1, 0x81D3, 0x01D6, 0x01DC, 0x81D9,
	0x01C8, 0x81CD, 0x81C7, 0x01C2, 0x0140, 0x8145, 0x814F,
	0x014A, 0x815B, 0x015E, 0x0154, 0x8151, 0x8173, 0x0176,
	0x017C, 0x8179, 0x0168, 0x816D, 0x8167, 0x0162, 0x8123,
	0x0126, 0x012C, 0x8129, 0x0138, 0x813D, 0x8137, 0x0132,
	0x0110, 0x8115, 0x811F, 0x011A, 0x810B, 0x010E, 0x0104,
	0x8101, 0x8303, 0x0306, 0x030C, 0x8309, 0x0318, 0x831D,
	0x8317, 0x0312, 0x0330, 0x8335, 0x833F, 0x033A, 0x832B,
	0x032E, 0x0324, 0x8321, 0x0360, 0x8365, 0x836F, 0x036A,
	0x837B, 0x037E, 0x0374, 0x8371, 0x8353, 0x0356, 0x035C,
	0x8359, 0x0348, 0x834D, 0x8347, 0x0342, 0x03C0, 0x83C5,
	0x83CF, 0x03CA, 0x83DB, 0x03DE, 0x03D4, 0x83D1, 0x83F3,
	0x03F6, 0x03FC, 0x83F9, 0x03E8, 0x83ED, 0x83E7, 0x03E2,
	0x83A3, 0x03A6, 0x03AC, 0x83A9, 0x03B8, 0x83BD, 0x83B7,
	0x03B2, 0x0390, 0x8395, 0x839F, 0x039A, 0x838B, 0x038E,
	0x0384, 0x8381, 0x0280, 0x8285, 0x828F, 0x028A, 0x829B,
	0x029E, 0x0294, 0x8291, 0x82B3, 0x02B6, 0x02BC, 0x82B9,
	0x02A8, 0x82AD, 0x82A7, 0x02A2, 0x82E3, 0x02E6, 0x02EC,
	0x82E9, 0x02F8, 0x82FD, 0x82F7, 0x02F2, 0x02D0, 0x82D5,
	0x82DF, 0x02DA, 0x82CB, 0x02CE, 0x02C4, 0x82C1, 0x8243,
	0x0246, 0x024C, 0x8249, 0x0258, 0x825D, 0x8257, 0x0252,
	0x0270, 0x8275, 0x827F, 0x027A, 0x826B, 0x026E, 0x0264,
	0x8261, 0x0220, 0x8225, 0x822F, 0x022A, 0x823B, 0x023E,
	0x0234, 0x8231, 0x8213, 0x0216, 0x021C, 0x8219, 0x0208,
	0x820D, 0x8207, 0x0202 };

INT16U crc_16(INT8U *aData, INT32U aSize)
{
	INT32U i;
	INT16U nAccum = 0;

	for ( i = 0; i < aSize; i++ )
	nAccum = ( nAccum << 8 ) ^ (INT16U)crc_table[( nAccum >> 8 ) ^ *aData++];
	return nAccum;
}
#ifdef USE_RFID_TAG_READER
INT8U  SeqNo = 1;
void commun_send_call_rfid_tag(void)
{
	char string_buf[100];
	INT8U send_buf[128];

	INT8U i;
	char crc_h, crc_l;

	i = 0;
	crc_h = 0x01; // 임시
	crc_l = 0x02; // 임시

	if (SeqNo < 255) SeqNo += 2;
	else SeqNo = 1;

	string_buf[i++] = 0x02;
	string_buf[i++] = SeqNo;
	string_buf[i++] = 0x0B;
	string_buf[i++] = 0x01;
	string_buf[i++] = 0x0E;
	string_buf[i++] = 0x01;
	string_buf[i++] = 0xB0;	//하나의 태그 읽기
	string_buf[i++] = 0x00;
	string_buf[i++] = 0x00;
	string_buf[i++] = crc_h;
	string_buf[i++] = crc_l;
	string_buf[i++] = 0x03;

	commun_outleng(&CommBuf, (char *)string_buf, 12);
}


INT8U RFIDRcvStateStart = 1;
//INT8U slen = 0;
INT8U commun_recv_rfid_tag(INT8U *reply_data, INT16U *length, INT8U *res_code)
{
	INT8U seq_no;
	INT8U send_index[2];
	INT8U recv_index[2];
	INT8U command_code;
	INT16U data_length;
	INT16U crc_data;
	INT8U received;
	INT8U rcv_buf[128];
	INT8U r_data, cnt, start_idx;
	INT16U len, j;
	static INT16S i;
	static INT8U rcv_state = 0;
	INT8U string_buf[100];

	if (RFIDRcvStateStart)
	{
		RFIDRcvStateStart = 0;
		rcv_state = 0;
	}

	if (!CheckRxBuf(&CommBuf))
	{
		return 0;
	}
	received = 0;

	r_data = GetCharRxBuf(&CommBuf);
////sprintf(string_buf,"%02X",r_data);
////PutString(8,6*slen,string_buf,2,1,1,strlen(string_buf));
////sprintf(string_buf,"%2d",rcv_state);
////PutString(16,6*slen,string_buf,2,1,1,strlen(string_buf));
////slen += strlen(string_buf);
	switch (rcv_state)
	{
		case 0:	// rcv STX
			i = 0;
			if (r_data == STX)
			{
				rcv_state = 1;
			}
			break;
		case 1:	// rcv Seq. No
			seq_no = r_data;
			rcv_buf[i++] = r_data;
			rcv_state = 2;
			break;
		case 2:	// rcv Sender Index[0]
			send_index[0] = r_data;
			rcv_buf[i++] = r_data;
			rcv_state = 3;
			break;
		case 3:	// rcv Sender Index[1]
			send_index[1] = r_data;
			rcv_buf[i++] = r_data;
			rcv_state = 4;
			break;
		case 4:	// rcv Receiver Index[0]
			recv_index[0] = r_data;
			rcv_buf[i++] = r_data;
			rcv_state = 5;
			break;
		case 5:	// rcv Receiver Index[1]
			recv_index[1] = r_data;
			rcv_buf[i++] = r_data;
			rcv_state = 6;
			break;
		case 6:	// rcv Receiver Index[1]
			command_code = r_data;
			rcv_buf[i++] = r_data;
			data_length = 0;
			cnt = 0;
			rcv_state = 7;
			break;
		case 7:	// rcv Data Length
			data_length <<= 8;
			data_length |= r_data;
			rcv_buf[i++] = r_data;
			cnt++;
			if (cnt >= 2)
			{
				cnt = 0;
				start_idx = i;
				rcv_state = 8;
			}
			break;
		case 8: // rcv Response Code & Reply Data
			rcv_buf[i++] = r_data;
			cnt++;
			if (cnt >= data_length)
			{
				crc_data = 0;
				cnt = 0;
				rcv_state = 9;
			}
			break;
		case 9: // rcv CRC
			crc_data <<= 8;
			crc_data |= r_data;
			rcv_buf[i++] = r_data;
			cnt++;
			if (cnt >= 2)
			{
				if (crc_data == crc_16(rcv_buf, start_idx + data_length))
				{
					rcv_state = 10;
				}
				else
				{
					rcv_state = 0;
				}
				rcv_state = 10;
			}
			break;
		case 10:
			if (r_data == ETX)
			{
				received = 1;
			}
			rcv_state = 0;
			break;
		default:
			rcv_state = 0;
			break;
	}

	*res_code = rcv_buf[start_idx];
	//memcpy(reply_data, rcv_buf[start_idx + 1], data_length - 1);
	len = 0;
	if (received && *res_code == 0x01)
	{
		len = 0;
		for (j = 0; j < data_length - 1; j++)
		{
			sprintf(reply_data + len, "%02X", rcv_buf[start_idx + 1 + j]);
			len = strlen(reply_data);
		}
	}
	*length = len;
	return received;
}
#endif //#ifdef USE_RFID_TAG_READER

INT16U get_new_bitmapaddress(INT16U id,INT8U chk)
{
	INT16U bmp_address,w;
	INT32U src_addr,addr;
	INT16S    i;

	src_addr  = DFLASH_BASE;
	src_addr += FLASH_BITMAP_AREA;
	addr    = src_addr;
	bmp_address = 999;
	for (i=0; i<14; i++) {
		w=Flash_wread(addr);
		if (w==id) {
			bmp_address=i;
			break;
		} else {
		   if (chk==0) {
			if ((w==0)  || (w>MAX_BITMAP_ID)) {
				bmp_address=i;
				break;
			}
		   }
		}
		addr += BITMAP_INFO_SIZE;
	}
	return bmp_address;
}

/**
 * @brief		사용자 라벨이 저장되어 있는 배열 index 가져오는 함수
 * @param		id  : 찾고자하는 사용자 라벨 id (51 ~ 99)
 * @param		chk : 사용자 라벨 사용하고 있는 영역인지 확인 여부 체크(라벨 id 영역이 깨져있는지 확인)
 * 					  0 : 확인함, 1 : 확인안함
 * @return 	사용자 라벨이 저장되어 있는 배열 index (0 ~ 19)
 * 				찾고자 하는 라벨 없으면 999 return
 * @remark	
 * 				Label header 구조
 * 				label_id	 2 byte
 * 				img_w		 2 byte
 * 				img_h		 2 byte
 * 				img_name	16 byte		
 * 				img_size	 2 byte
 * 				====================
 * 				total		24 byte		* 20 개		= 480 byte
 * 
 * 				저장한 순서대로 index 부여
 * 				ex) 배열_index	사용자_라벨_id
 * 					0			79
 * 					1			53
 */
INT16U get_new_labeladdress(INT16U id,INT8U chk)
{
	INT16U label_address,w;
	INT32U src_addr,addr;
	INT16S    i;

	src_addr  = DFLASH_BASE;
	src_addr += FLASH_LABEL_AREA;
	addr    = src_addr;
	label_address = 999; // 찾고자 하는 id 없을 때의 return 값
	if (id<=MAX_LABEL_ROM_NO) return label_address; // 기본 라벨(1~45)이면 return
	for (i=0; i<LABEL_CUSTOM_TOTAL_NUM; i++) {
#ifdef USE_LABEL_FORMAT_EXT
		if(i == LABEL_CUSTOM_2MB_NUM)
		{
			addr = FLASH_LABEL_FORMAT_EXT_HEADER_AREA;
		}
#endif
		w=Flash_wread(addr);
		if (w==id) {
			label_address=i;
			break;
		} else {
			if (chk==0) { // 사용자 라벨 사용하고 있는 영역인지 확인
				if ((w<1) || (w>999)) { // 0이거나 0xffff같은 깨진 영역이면 사용하고 있지 않은 영역
					label_address=i;
					break;
				}
			}
		}
		addr += LABEL_INFO_SIZE;
	}
	return label_address;
}

void commun_write_response(HUGEDATA COMM_BUF *CBuf,INT8U *str,INT8U errCode)
{
	char *commun_buffer;
	INT16S   r,i;
	//char  cc;
	INT8U checksum;
	INT32U number;
	INT16U deptno;
	INT32U pluno;

	commun_buffer=(char *)serial_send_point;

	number=0;
	if(str)
	{
		memcpy(&deptno,&str[0],2);
		memcpy(&pluno,&str[2],4);
		number = deptno*1000000 + pluno;
	}

	r=0;

	checksum = 0;
	if(errCode) commun_buffer[0]='N';
	else        commun_buffer[0]='G';
	commun_buffer[1]='L';  //sub command 현재 "PLU"로 고정 테스트
	//commun_buffer[1]= serial_command[com_type];

	commun_buffer[2]=status_scale.scaleid;
	commun_buffer[3]=0;
	commun_buffer[4]=0;
	commun_buffer[5]=0;

	commun_buffer[6]=',';

	//data length
	if(errCode) commun_buffer[7] = 5;
	else        commun_buffer[7] = 4;

	commun_buffer[8] = 0;
	commun_buffer[9]=':';
	memcpy(&commun_buffer[10], &number, sizeof(INT32U));
	r=14;
	if(errCode)
	{
		commun_buffer[r]=errCode;
		r++;
	}
	commun_buffer[r]=':';
	r++;

	for(i=2; i<r; i++)
		checksum += commun_buffer[i];

	commun_buffer[r]=checksum;
	r++;
	commun_buffer[r]=_CR;
	r++;
	commun_outleng(CBuf,(char *)commun_buffer,r);
}
#ifdef USE_RUSSIA_LOGISTICS_PLU
void commun_write_gtin_response(HUGEDATA COMM_BUF *CBuf,INT8U *str,INT8U errCode)
{
	char *commun_buffer;
	INT16S r,i;
	INT8U checksum;
	INT32U number;
	INT16U deptno;
	INT32U pluno;
		
	commun_buffer=(char *)serial_send_point;
	
	number=0;
	if(str)
	{
		memcpy(&deptno,&str[0],2);
		memcpy(&pluno,&str[2],4);
		number = deptno*1000000 + pluno;
	}
	
	r=0;

	checksum = 0;  
	if(errCode) commun_buffer[0]='N';
	else        commun_buffer[0]='G';
	commun_buffer[1]=DATA_LOGISTICS_PLU;
	commun_buffer[2]=status_scale.scaleid;
	commun_buffer[3]=0;
	commun_buffer[4]=0;	
	commun_buffer[5]=0;
	commun_buffer[6]=',';
	
	//data length
	if(errCode) commun_buffer[7] = 5;
	else        commun_buffer[7] = 4;
	
	commun_buffer[8] = 0;
	commun_buffer[9]=':';
	memcpy(&commun_buffer[10], &number, sizeof(INT32U));
	r=14;
	if(errCode) 
	{
		commun_buffer[r]=errCode;
		r++;	
	}
	commun_buffer[r]=':';
	r++;

	for(i=2; i<r; i++)
		checksum += commun_buffer[i];

	commun_buffer[r]=checksum;
	r++;
	commun_buffer[r]=_CR;
	r++;

	//r=17;

	commun_outleng(CBuf,(char *)commun_buffer,r);
}

#endif

void commun_write_err16(HUGEDATA COMM_BUF *CBuf,INT8U cmd,INT16U ret16,INT8U chk,INT8U noheader)
{
	char *commun_buffer;
	INT16S   r;
	char  cc;
	if (ret16==0) {
	 	if (chk!=2) return;
	}
	commun_buffer=(char *)serial_send_point;
#ifdef USE_WEIGHING_PRINTER_COMM
	if(cmd >= 0x45 && cmd <= 0x47) {
		if(CBuf->Type == COMM_TYPE_HOST)
			noheader = 1;
	}
#endif
	if (noheader)
	{
		r = 0;
	}
	else
	{
		r=ethernet_gen_head(commun_buffer);
	}

	sprintf((char *)&commun_buffer[r],"W%02X:",cmd);
	r+=strlen(&commun_buffer[r]);
	if ((chk==0) || (chk==2)) {
		sprintf(&commun_buffer[r],"O%04X",ret16);
	} else {
		if (ret16<0x80) {
			cc='O';
		} else {
			cc='E';
		}
		sprintf(&commun_buffer[r],"%c%02X",cc,ret16);
	}
	r+=strlen(&commun_buffer[r]);
	sprintf(&commun_buffer[r],"%c%c",0x0a,0x0a);
	r+=strlen(&commun_buffer[r]);
	commun_outleng(CBuf,(char *)commun_buffer,r);
}

#ifdef USE_KOR_TRACE
////////////////////////////////////////////////////////
// Korean Traceability

KORTRACE_SEND_DATA_TYPE		KorTrace_SendData;
KORTRACE_RECEIVE_DATA_TYPE	KorTrace_ReceiveData;
KORTRACE_RECEIVE_CMD_TYPE	KorTrace_ReceiveCmd;
KORTRACE_RECEIVE_ACK_TYPE	KorTrace_ReceiveACK;
KORTRACE_RECEIVE_NAK_TYPE	KorTrace_ReceiveNAK;
KORTRACE_FLAG			KorTrace_Flag;

void kortrace_init(void)
{
	INT8U size;

	memset(&KorTrace_SendData, 0x00, sizeof(KorTrace_SendData));
	memset(&KorTrace_ReceiveData, 0x00, sizeof(KorTrace_ReceiveData));
	memset(&KorTrace_ReceiveCmd, 0x00, sizeof(KorTrace_ReceiveCmd));
	memset(&KorTrace_ReceiveACK, 0x00, sizeof(KorTrace_ReceiveACK));
	memset(&KorTrace_ReceiveNAK, 0x00, sizeof(KorTrace_ReceiveNAK));

	size = (INT8U)sizeof(KorTrace_Flag) - (INT8U)sizeof(KorTrace_Flag.memberCode) -1;
	memset(&KorTrace_Flag, 0x00, size);  // Member code is not initialized
	//memset(&KorTrace_Flag, 0x00, sizeof(KorTrace_Flag));

}

void korTraceMemberCodeInit(void)
{
	memset(KorTrace_Flag.memberCode, 0x00, sizeof(KorTrace_Flag.memberCode));
}

extern void	SaleInsertLabelData(void);
extern INT8U  	OnOff(INT8U tmp);
extern INT8U StartPrintFlag;

INT8U kortrace_com_receive( COMM_BUF *CBuf, INT8U *buffer)
{
	INT16U cmd;
	INT16U v16;
	INT16U v16_2=0;
	INT8U ret1;
	INT8U ret2;
	INT32U recv_unitprice;


	memcpy(&cmd, &buffer[0], 2);
	memcpy(&v16_2, &buffer[2], 2);	// return command

	if(v16_2 != KORTR_SEND_WEIGHT && cmd != KORTR_RCV_NOPRT_ACK)  		// SG061114 무게 전송후 응답시에는 아무 동작도 안하게 수정
	{	
		sale_display_kortrace(OFF);
		KorTrace_Flag.flagReceive = 0;
		sale_display_update(UPDATE_TITLE);	// CJK070705
	}

	switch (cmd) {
		case KORTR_RCV_NAK :	// NAK
			memcpy(&KorTrace_ReceiveNAK, &buffer[0], 6);

			switch(KorTrace_ReceiveNAK.ret_cmd) {
				case KORTR_REQ_PRINT_ID:
				case KORTR_REQ_FIRST_ID:
				case KORTR_REQ_NEXT_ID:
				case KORTR_REQ_FIXED_ID:
				case KORTR_REQ_COMPLETE_ID:
				case KORTR_REQ_RETURN_ID:
				case KORTR_REQ_FPPRINT_ID:
#ifdef USE_HANAPOS_PROTOCOL
				case KORTR_REQ_SCALE_SALE_ID:
#endif
				default:
					if(KorTrace_ReceiveNAK.err_code==0x0004) //PLU 없음
					{
						BuzOn(2);
						sale_pluclear(ON);
						sale_display_update(0x0fff);//061128						
						
					} 
					break;
							
			}
			KorTrace_Flag.flagReceive = 0xff;	// fail sending anything
			break;
		case KORTR_RCV_ACK :	// ACK 
			memcpy(&v16, &buffer[2], 2);	// return command

			KorTrace_Flag.flagCommSuccess = 1;
			switch (v16) {
				case KORTR_SEND_WEIGHT :	// Send Weight
					break;
				case KORTR_REQ_PRINT_ID :
				case KORTR_REQ_FPPRINT_ID :
#ifdef USE_HANAPOS_PROTOCOL
				case KORTR_REQ_SCALE_SALE_ID :
#endif
					kortrace_com_parse_common(buffer,v16); //SG060405
					KorTrace_Flag.flagReceive = 1;	// receive id
					KorTrace_Flag.flagPrint = 1;	// print label
					
					sale_display_update(UPDATE_TITLE);	// CJK070705

					SaleInsertLabelData();
					StartPrintFlag = ON;
					ret1 = Prt_PrintStart(PRT_ITEM_NORMAL_MODE, PrtStruct.Mode, 0, OFF, 0, 0);
					PrintListWaitEnd();
					ret2 = SaleAcctLocalAddTransaction(status_scale.Plu.deptid,status_scale.cur_pluid,&Price,0);
					keyapp_save_price(); //SG060403
					KorTrace_Flag.flagPrint = 0;	// print label
					StartPrintFlag = OFF;

					KorTraceCheckMemberCode();
#ifdef USE_HANAPOS_PROTOCOL
					if (ret1 == 1 && ret2) {
						kortrace_com_send_ack_withData(CBuf, v16, KORTR_RCV_ACK, 0);
					} else {
						kortrace_com_send_ack(CBuf, v16, KORTR_RCV_NAK, 0x0003);
					}
#endif
					break;
				case KORTR_REQ_FIRST_ID :
				case KORTR_REQ_NEXT_ID :
				case KORTR_REQ_FIXED_ID :
				case KORTR_REQ_COMPLETE_ID :
				case KORTR_REQ_RETURN_ID :
					//개체완료요청을 하면 PC에서 다음 개체값을 리턴 함
					kortrace_com_parse_common(buffer,v16); //SG060405
					KorTrace_Flag.flagReceive = 1;
					break;
				default :
					kortrace_com_send_ack(CBuf, cmd, KORTR_RCV_NAK, 0x0003);	// 문법 오류(없는 command)
					
					KorTrace_Flag.flagReceive = 0xff;
					KorTrace_Flag.flagPrint = 0;
					break;					
			}
			break;
		case KORTR_RCV_NOPRT_ACK :
			memcpy(&v16, &buffer[2], 2);	// return command
			switch (v16) {
				case KORTR_REQ_PRINT_ID :
				case KORTR_REQ_FPPRINT_ID :
					KorTrace_Flag.flagCommSuccess = 1;
					break;
			}
			break;
		case KORTR_RCV_PRINT_ID :	// 라벨인쇄명령	(PC-->Scale) Direct
		case KORTR_RCV_PRINT2_ID :	// 라벨인쇄명령2(개체바코드인쇄) (PC-->Scale) Direct
			ret1 = OFF;
			kortrace_com_parse_common(buffer,cmd); //SG060405
#ifdef USE_HANAPOS_PROTOCOL
			if(ADM_GetData.Stable)
			{
				if((status_scale.Weight != KorTrace_ReceiveCmd.weight) && (status_scale.Weight != 0))
				{
					kortrace_com_send_ack(CBuf, cmd, KORTR_RCV_NAK, 0x0005);
					break;
				}
				if( KorTrace_ReceiveCmd.weight >= ad_get_capa())
				{
					kortrace_com_send_ack(CBuf, cmd, KORTR_RCV_NAK, 0x0005);
					break;
				}
				if(!GlbOper.PrintUnderMinWeight && ((status_scale.Weight < status_scale.weight_min) && (status_scale.Weight != 0)\
					|| (KorTrace_ReceiveCmd.weight < status_scale.weight_min)))
				{
					kortrace_com_send_ack(CBuf, cmd, KORTR_RCV_NAK, 0x0005);
					break;
				}
			}
#endif
			KorTrace_Flag.flagPrint = 2;	// print label
			if(!keyapp_pluno(0,KorTrace_ReceiveCmd.commodity_no,OFF)){
				BuzOn(2); 
				KorTrace_Flag.flagPrint = 0;
				break;
			}
			SaleInsertLabelData();
			StartPrintFlag = ON;
//			ret1 = Prt_PrintStart(PRT_ITEM_NORMAL_MODE, PrtStruct.Mode, 0, ON, 0, 0);
			ret1 = Prt_PrintStart(PRT_ITEM_NORMAL_MODE, PrtStruct.Mode, 0, OFF, 0, 0);
			PrintListWaitEnd();
			ret2 = SaleAcctLocalAddTransaction(status_scale.Plu.deptid,status_scale.cur_pluid,&Price,0);
			if (ret1 == 1 && ret2) {
#ifdef USE_HANAPOS_PROTOCOL
				kortrace_com_send_ack_withData(CBuf, cmd, KORTR_RCV_ACK, 0);
#else
				kortrace_com_send_ack(CBuf, cmd, KORTR_RCV_ACK, 0);
#endif
			} else {
				kortrace_com_send_ack(CBuf, cmd, KORTR_RCV_NAK, 0x0003);
			}
			KorTrace_Flag.flagPrint = 0;	// print label
			StartPrintFlag = OFF;
			KorTraceCheckMemberCode();            
			sale_pluclear(ON);      //SG060405
			sale_display_update(0x0fff);//061128         
			break;
		case KORTR_RCV_CLEAR_PLU :	// PLU Clear 명령 (PC-->Scale)  //SG060413
			sale_pluclear(ON);
			sale_display_update(0x0fff);//061128
			kortrace_com_send_ack(CBuf, cmd, KORTR_RCV_ACK, 0);
			break;
		case KORTR_SEND_UNITPRICE :  // Unit price 전송 명령 (PC-->Scale) //SG061114
			memcpy(&recv_unitprice,&buffer[2],4);
			status_scale.cur_unitprice = recv_unitprice % Operation.MaxUnitPrice; 
			status_scale.cur_keyvalue=status_scale.cur_unitprice;
			kortrace_com_send_ack(CBuf, cmd, KORTR_RCV_ACK, 0);
			sale_display_update(UPDATE_TITLE|UPDATE_UNIT|UPDATE_PRICE);
			break;
		default :
			kortrace_com_send_ack(CBuf, cmd, KORTR_RCV_NAK, 0x0003);	// 문법 오류(없는 command)
			KorTrace_Flag.flagReceive = 0xff;
			KorTrace_Flag.flagPrint = 0;
			break;
	}
 	if (KorTrace_Flag.flagReceive == 1) {
	} 
	else 
	{
	} 
	return ON;
}


void kortrace_make_display_data(INT8U *disp_str, INT8U *bar_str, INT8U disp_len)
{
	INT8U cnt, leng;
	INT8U temp[8];

	//make display data
	cnt=0;
	memset((INT8U *)disp_str, 0x20, disp_len);  
	memcpy((INT8U *)&disp_str[cnt], &bar_str[0], 5);	// 재고 중량 (5자리, 소수점 포함)
	cnt+=5;
	cnt+=1; //space	 	 
	leng=strlen((char *)bar_str);
	if (leng > 32) leng = 32;
	if(leng){ 
		if(leng>12){
			memcpy(&disp_str[cnt], &bar_str[leng-12], 12); 	
			cnt+=12;	 
		}else if(leng>0){
			cnt=cnt+12-leng;	 
			memcpy(&disp_str[cnt], &bar_str[0], leng); 		
			cnt+=leng;	 
		} 
		memset(&disp_str[cnt],0x2D,1);  // '-'						
		cnt+=1; 

		memset((INT8U *)temp, 0x00, sizeof(temp));
		memcpy(temp, &bar_str[38+1], KORTR_PROCESS_NO_LEN - 1);	//4자리중 첫1자리 제외
		leng = strlen((char *)temp);
		if(leng<6 && leng>0)
			memcpy(&disp_str[cnt], temp, leng);			
		 
	} 
	disp_str[22]=0;
}

INT8U kortrace_com_parse_common(INT8U *str,INT16U command)
{
	INT8U leng;

	switch(command){
	case KORTR_REQ_PRINT_ID:
	case KORTR_REQ_FIRST_ID:
	case KORTR_REQ_NEXT_ID:
	case KORTR_REQ_FIXED_ID:
	case KORTR_REQ_COMPLETE_ID:
	case KORTR_REQ_RETURN_ID:
	case KORTR_REQ_FPPRINT_ID:
		memcpy(&KorTrace_ReceiveData, str, sizeof(KORTRACE_RECEIVE_DATA_TYPE));
		memcpy(&KorTrace_Flag.CurBarData[0],&KorTrace_ReceiveData.traceid_barcode[0],sizeof(KorTrace_Flag.CurBarData));	
#ifdef USE_KORTRACE_PROTOCOL_ADD_LABEL
		KorTrace_Flag.labelNo = bcd2hex(KorTrace_ReceiveData.labelNo);
#endif
		break;
#ifdef USE_HANAPOS_PROTOCOL
	case KORTR_REQ_SCALE_SALE_ID:
			memcpy(&KorTrace_ReceiveData, str, sizeof(KORTRACE_RECEIVE_DATA_TYPE));
			memcpy(&KorTrace_Flag.CurBarData[0],&KorTrace_ReceiveData.traceid_printdata[3],sizeof(KorTrace_Flag.CurBarData));
			memcpy(&KorTrace_Flag.CurBarData2[0],&KorTrace_ReceiveData.traceid_barcode2[0],sizeof(KorTrace_Flag.CurBarData));
#endif
		break;
	case KORTR_RCV_PRINT_ID:
	case KORTR_RCV_PRINT2_ID:
		memcpy(&KorTrace_ReceiveCmd, str, sizeof(KORTRACE_RECEIVE_CMD_TYPE));
#ifdef USE_HANAPOS_PROTOCOL
		memcpy(&KorTrace_Flag.CurBarData[0],&KorTrace_ReceiveCmd.traceid_printdata[3],sizeof(KorTrace_Flag.CurBarData));
		memcpy(&KorTrace_Flag.CurBarData2[0],&KorTrace_ReceiveCmd.traceid_barcode2[0],sizeof(KorTrace_Flag.CurBarData));
#else
		memcpy(&KorTrace_Flag.CurBarData[0],&KorTrace_ReceiveCmd.traceid_barcode[0],sizeof(KorTrace_Flag.CurBarData));
#endif
#ifdef USE_KORTRACE_PROTOCOL_ADD_LABEL
		KorTrace_Flag.labelNo = bcd2hex(KorTrace_ReceiveData.labelNo);
#endif
		break;
	case KORTR_SEND_WEIGHT:
		memcpy(&KorTrace_Flag.CurBarData[0],&str[4],sizeof(KorTrace_Flag.CurBarData));	 
		break;
	default:
		break;	
	}

	if (command == KORTR_RCV_PRINT2_ID)
	{     
		memcpy(&KorTrace_Flag.barcode_data[0],&KorTrace_Flag.CurBarData[5],sizeof(KorTrace_Flag.barcode_data)); //barcode_data[30]	 
		KorTrace_Flag.barcode_data[32] = 0;
		leng = strlen((char *)KorTrace_Flag.barcode_data);
		memcpy(&KorTrace_Flag.barcode_data[leng], &KorTrace_Flag.CurBarData[38], KORTR_PROCESS_NO_LEN);
		KorTrace_Flag.barcode_data[leng+KORTR_PROCESS_NO_LEN] = 0;
	}
	else
	{
		memset(&KorTrace_Flag.barcode_data[0], 0, sizeof(KorTrace_Flag.barcode_data));
	}
	
	//if( (command==KORTR_RCV_PRINT_ID || command==KORTR_RCV_PRINT2_ID) && KorTrace_ReceiveCmd.traceid_barcode2[0]){
#ifdef USE_HANAPOS_PROTOCOL
	if(command == KORTR_RCV_PRINT_ID || command == KORTR_REQ_SCALE_SALE_ID){
		memcpy(&KorTrace_Flag.barcode_data[0],&KorTrace_Flag.CurBarData[0],sizeof(KorTrace_ReceiveCmd.traceid_barcode2)); //barcode_data[30]	 
		KorTrace_Flag.barcode_data[29]=0;
		memcpy(&KorTrace_Flag.barcode_data2[0],&KorTrace_Flag.CurBarData2[0],sizeof(KorTrace_ReceiveCmd.traceid_barcode2)); //barcode_data[30]	 
		KorTrace_Flag.barcode_data[29]=0;
		return ON;
	}
#else
	if( ((command==KORTR_RCV_PRINT_ID) && KorTrace_ReceiveCmd.traceid_barcode2[0]) ||
		((command==KORTR_RCV_PRINT2_ID) && KorTrace_ReceiveCmd.traceid_barcode2[0])) {
		memcpy(&KorTrace_Flag.barcode_data[0],&KorTrace_ReceiveCmd.traceid_barcode2[0],sizeof(KorTrace_ReceiveCmd.traceid_barcode2)); //barcode_data[30]	 
		KorTrace_Flag.barcode_data[29]=0;
		return ON;
	}
#endif

	kortrace_make_display_data(KorTrace_Flag.display_data, KorTrace_Flag.CurBarData, sizeof(KorTrace_Flag.display_data));

	return ON;
}

void kortrace_com_send_ack( COMM_BUF *CBuf, INT16U ret_cmd, INT16U ack, INT16U err_code)
{
	INT16U cnt, header_len;
	char buf[20];

	cnt = 0;
///////////////////////////
	buf[cnt++] = 'P';
	tohex2(0x01, &buf[1]);	// Protocol 1
	cnt += 2;
	buf[cnt++] = 'L';
	cnt += 4;	// Length
	buf[cnt++] = ':';
	header_len = cnt;
///////////////////////////

	memcpy(&buf[cnt], &ack, 2);	// ACK/NAK
	cnt += 2;

	memcpy(&buf[cnt], &ret_cmd, 2);	// return command
	cnt += 2;
		
	if(ack == KORTR_RCV_NAK) {
		memcpy(&buf[cnt], &err_code, 2);	// error code
		cnt += 2;
	}

///////////////////////////
	tohex2((cnt-header_len)/0x100, &buf[4]);	// Length
	tohex2((cnt-header_len)%0x100, &buf[6]);	// Length

	buf[cnt++] = gen_bcc((INT8U*)&buf[header_len],cnt-header_len);
///////////////////////////

	commun_outleng(CBuf, buf, cnt);
}

#ifdef USE_HANAPOS_PROTOCOL
void kortrace_com_send_ack_withData( COMM_BUF *CBuf, INT16U ret_cmd, INT16U ack, INT16U err_code)
{
	INT16U cnt, header_len, head, tail; 
	INT32U addr;
	char buf[161];
	TRANSACTION_SALE_DATA_TYPE trans_sdt;

	memcpy(&buf[0],0,sizeof(buf));
	memset((INT8U *)&trans_sdt,0,sizeof(trans_sdt));


	cnt = 0;
///////////////////////////
	buf[cnt++] = 'P';
	tohex2(0x01, &buf[1]);	// Protocol 1
	cnt += 2;
	buf[cnt++] = 'L';
	cnt += 4;	// Length
	buf[cnt++] = ':';
	header_len = cnt;
///////////////////////////

	memcpy(&buf[cnt], &ack, 2);	// ACK
	cnt += 2;

	memcpy(&buf[cnt], &ret_cmd, 2);	// return command
	cnt += 2;
///////////////////////////	추가정보 4, 6

	memcpy(&buf[cnt], &KorTrace_Flag.CurBarData[0], 30);
	cnt += 30;

	memcpy(&buf[cnt], &KorTrace_Flag.CurBarData2[0], 30);
	cnt += 30;
/////////////////////////// 판매데이터

	get_transaction_data_type4_sale(&trans_sdt, &pTrans_add_Data, &pTrans_Data);
	if(ret_cmd == KORTR_RCV_PRINT_ID || ret_cmd == KORTR_RCV_PRINT2_ID)
		memcpy(&trans_sdt.sdtTraceNo[0], &KorTrace_ReceiveCmd.traceid_barcode[0], sizeof(trans_sdt.sdtTraceNo));
	else if(ret_cmd == KORTR_REQ_SCALE_SALE_ID)
		memcpy(&trans_sdt.sdtTraceNo[0], &KorTrace_ReceiveData.traceid_printdata[0], sizeof(trans_sdt.sdtTraceNo));

	memcpy(&buf[cnt], &trans_sdt.sdtType, 1);
	cnt += 1;

	memcpy(&buf[cnt], &trans_sdt.sdtScaleId, 1);
	cnt += 1;

	memcpy(&buf[cnt], &trans_sdt.sdtPLUtype, 1);
	cnt += 1;

	memcpy(&buf[cnt], &trans_sdt.sdtDept, 1);
	cnt += 1;

	memcpy(&buf[cnt], &trans_sdt.sdtPLUNo, 4);
	cnt += 4;

	memcpy(&buf[cnt], &trans_sdt.sdtItemCode, 4);
	cnt += 4;

	if(ret_cmd != KORTR_REQ_SCALE_SALE_ID)
		memcpy(&buf[cnt], &KorTrace_ReceiveCmd.weight, 4);
	else
		memcpy(&buf[cnt], &trans_sdt.sdtWeight, 4);
	cnt += 4;

	memcpy(&buf[cnt], &trans_sdt.sdtQty, 2);
	cnt += 2;

	memcpy(&buf[cnt], &trans_sdt.sdtPCS, 2);
	cnt += 2;

	if(ret_cmd != KORTR_REQ_SCALE_SALE_ID)
		memcpy(&buf[cnt], &KorTrace_ReceiveCmd.unitprice, 4);
	else
		memcpy(&buf[cnt], &trans_sdt.sdtUnitPrice, 4);
	cnt += 4;
	if(ret_cmd != KORTR_REQ_SCALE_SALE_ID)
		memcpy(&buf[cnt], &KorTrace_ReceiveCmd.totalprice, 4);
	else
		memcpy(&buf[cnt], &trans_sdt.sdtTotalPrice, 4);
	cnt += 4;

	memcpy(&buf[cnt], &trans_sdt.sdtDiscountPrice, 4);
	cnt += 4;

	memcpy(&buf[cnt], &trans_sdt.sdtTransactionCounter, 4);
	cnt += 4;

	memcpy(&buf[cnt], &trans_sdt.sdtPrefix, 2);
	cnt += 2;

	memcpy(&buf[cnt], &trans_sdt.sdtStatus, 1);
	cnt += 1;

	memcpy(&buf[cnt], &trans_sdt.sdtDate[0], 6);
	cnt += 6;

	memcpy(&buf[cnt], &trans_sdt.sdtBarcodeStr[0], 20);
	cnt += 20;

	memcpy(&buf[cnt], &trans_sdt.sdtTraceNo[0], 20);
	cnt += 20;

	memcpy(&buf[cnt], &trans_sdt.sdtSlaughterHouse, 1);
	cnt += 1;

	memcpy(&buf[cnt], &trans_sdt.Reserved, 1);
	cnt += 1;
	
///////////////////////////
	tohex2((cnt-header_len)/0x100, &buf[4]);	// Length
	tohex2((cnt-header_len)%0x100, &buf[6]);	// Length

	buf[cnt++] = gen_bcc((INT8U*)&buf[header_len],cnt-header_len);
///////////////////////////

	commun_outleng(CBuf, buf, cnt);
}
#endif

void kortrace_com_send_data( COMM_BUF *CBuf, INT16U cmd)
{
	INT16U cnt, header_len;
	char buf[100];

	cnt = 0;

	buf[cnt++] = 'P';
	tohex2(0x01, &buf[1]);	// Protocol 1
	cnt += 2;
	buf[cnt++] = 'L';
	cnt += 4;	// Length
	buf[cnt++] = ':';
	header_len = cnt;

//////////////
// Data
	memcpy(&buf[cnt], &cmd, 2);	// command
	cnt += 2;

	memcpy(&buf[cnt], &KorTrace_SendData.commodity_no, 4);
	cnt += 4;

	memcpy(&buf[cnt], &KorTrace_SendData.weight, 4);
	cnt += 4;

	memcpy(&buf[cnt], &KorTrace_SendData.unitprice, 4);
	cnt += 4;

	memcpy(&buf[cnt], &KorTrace_SendData.totalprice, 4);
	cnt += 4;

	memcpy(&buf[cnt], &KorTrace_SendData.traceid_barcode[0], sizeof(KorTrace_SendData.traceid_barcode));
	cnt += sizeof(KorTrace_SendData.traceid_barcode); //SG060401
///////////////////

	//SG070202. Add Member Code
	if(cmd == KORTR_REQ_PRINT_ID || cmd == KORTR_REQ_FPPRINT_ID)	//Modified by JJANG 071123
	{
		memcpy(&buf[cnt], KorTrace_Flag.memberCode, sizeof(KorTrace_Flag.memberCode));
		cnt += sizeof(KorTrace_Flag.memberCode); 
	}

	tohex2((cnt-header_len)/0x100, &buf[4]);	// Length
	tohex2((cnt-header_len)%0x100, &buf[6]);	// Length

	buf[cnt++] = gen_bcc((INT8U*)&buf[header_len],cnt-header_len);	// or bcc
	
	commun_outleng(CBuf, buf, cnt);

}

extern  void sale_calcprice(void);
void kortrace_com_req_command( COMM_BUF *CBuf, INT16U command)
{
	memset(&KorTrace_SendData, 0x00, sizeof(KorTrace_SendData));
	KorTrace_SendData.commodity_no = status_scale.cur_pluid; 
	KorTrace_SendData.weight = Price.Weight[PRICE_RESULT];
	KorTrace_SendData.unitprice = Price.UnitPrice[PRICE_RESULT];
	KorTrace_SendData.totalprice = Price.DisplayedPrice;

	switch(command){
		case KORTR_REQ_FIRST_ID:
			//SG060405
			sale_calcprice(); 
			KorTrace_SendData.unitprice = Price.UnitPrice[PRICE_RESULT];
			KorTrace_SendData.totalprice = Price.TotalPrice[PRICE_RESULT];
			memset(&KorTrace_SendData.traceid_barcode, 0x00, sizeof(KorTrace_SendData.traceid_barcode)); //SG060331
			break;
		case KORTR_REQ_PRINT_ID:  // 라벨 인쇄
		case KORTR_SEND_WEIGHT:  // 중량 전송
		case KORTR_REQ_NEXT_ID:  // 다음 개체
		case KORTR_REQ_FIXED_ID:  // 개체 지정 
		case KORTR_REQ_COMPLETE_ID:  // 개체 완료			
		case KORTR_REQ_RETURN_ID:  // 반품 요청 Added by JJANG 071123
		case KORTR_REQ_FPPRINT_ID:  // 라벨 인쇄(정액) 
#ifdef USE_HANAPOS_PROTOCOL
		case KORTR_REQ_SCALE_SALE_ID:
#endif
			memcpy((INT8U *)KorTrace_SendData.traceid_barcode, (INT8U *)KorTrace_Flag.CurBarData, sizeof(KorTrace_Flag.CurBarData));
			break;
		default:
			memcpy((INT8U *)KorTrace_SendData.traceid_barcode, (INT8U *)KorTrace_Flag.CurBarData, sizeof(KorTrace_Flag.CurBarData));
			break;
	}

	if(!KorTrace_SendData.commodity_no){
		memset(&KorTrace_SendData.traceid_barcode, 0x00, sizeof(KorTrace_SendData.traceid_barcode)); 
		memset(&KorTrace_Flag.CurBarData, 0x00, sizeof(KorTrace_Flag.CurBarData)); 

	}else {
		memcpy((INT8U *)KorTrace_SendData.traceid_barcode, (INT8U *)KorTrace_Flag.CurBarData, sizeof(KorTrace_Flag.CurBarData));
	}
 
	kortrace_com_send_data(CBuf, command);
}

void KorTraceCheckMemberCode(void)
{
	INT8U ucTemp;
	INT8U saveMemberCode;
	ucTemp = get_global_bparam(GLOBAL_SALE_SETUP_FLAG5);
	saveMemberCode = OnOff(ucTemp & 0x10);
	if(!saveMemberCode) 
		memset(KorTrace_Flag.memberCode, 0, sizeof(KorTrace_Flag.memberCode));

}

void KorTraceSendWeight(void)
{
	INT8U check_stable = 0, backflag,retry_cnt,flagTemp;
	INT16U wait;

	if (network_status.service_bflag2==2) 
	{
		if(KorTrace_Flag.flagSendWeight) 
		{
				
			retry_cnt = KOR_TRACE_RETRY_CNT;
			flagTemp = 0;
			KorTrace_Flag.flagCommSuccess = 0;
#ifdef USE_KOR_TRACE_RS232
			kortrace_com_req_command(&CommBuf,KORTR_SEND_WEIGHT);
			backflag = flagTempAllowSale;
			flagTempAllowSale = ON;
			wait = SysTimer100ms;
			while(1)
			{
				network_common_trans();
				if(KorTrace_Flag.flagCommSuccess)
				{
					flagTemp = 1;
					KorTrace_Flag.flagSendWeight = OFF;
					break;
				}
				if((INT16U)(SysTimer100ms-wait) > KOR_TRACE_TIMEOUT) break;
			}
			flagTempAllowSale = backflag;
#else
			while(retry_cnt>0)
			{
	
				if (Ethernet_Server(CLIENT_OPEN, 0, CLIENT_SCALE_POS)) 
				{
					
					kortrace_com_req_command(&(CommBufEthData[0]),KORTR_SEND_WEIGHT);
					
					backflag = flagTempAllowSale;
					flagTempAllowSale = ON;
					wait = SysTimer100ms;
					while(1)
					{
						network_common_trans();
						if(KorTrace_Flag.flagCommSuccess)
						{
							flagTemp = 1;
							KorTrace_Flag.flagSendWeight = OFF;
							break;
						}
						if((INT16U)(SysTimer100ms-wait) > KOR_TRACE_TIMEOUT) break;
					}
					flagTempAllowSale = backflag;
					
				} 

				Ethernet_Server(CLIENT_CLOSE, 0, CLIENT_SCALE_POS);

				retry_cnt--;
				if(flagTemp) break;

			} //while(retry_cnt>0)
#endif
			if(!KorTrace_Flag.flagCommSuccess)
			{
					BuzOn(4);
					KorTrace_CommErrorMsg();  //SG060404
			}		

			KorTrace_Flag.err_cnt=0;
			KorTrace_Flag.flagSendWeight = OFF;			

		} 
	} 
}

#endif
