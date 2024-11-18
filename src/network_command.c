#include <stdio.h>
#include <string.h>
#include "globals.h"
#include "commbuf.h"
#include "ethernet.h"
#include "prtdrv.h"
#include "main.h"
#include "flash.h"
#include "flash_app.h"
#include "initial.h"
#include "common.h"
#include "mode_sale.h"
#include "mode_commun.h"
#include "ethernet_comm.h"
#include "main.h"
#include "mode_main.h"
#include "prt_interpreter.h"
#include "network_command.h"
#include "plu_device.h"
#include "plu_manager.h"
#include "caption_app.h"
#include "debug.h"
#include "dsp_app.h"
#include "traceability.h"

//DEL070705INT8U DisplayNetworkError = OFF;

ETHERNET_SLAVE_SCALE NetSlaveInfo[MAX_SLAVE_SCALE];

extern HUGEDATA INT8U serial_send_point[MAX_SERIAL_BUFFER_LEN];
//extern void debug_send_value(INT8U x,INT8U v);
//extern INT16S gen_transaction_data(char *commun_buffer,TRANSACTION_RECORD_TYPE trans);
extern INT16S gen_transaction_pay(char *commun_buffer,TRANSACTION_PAY trans);
extern INT16S PutSmallString(INT16S y, INT16S x,char *data);
//extern INT8U  commun_send_block_data(HUGEDATA COMM_BUF *CBuf,INT32U cmd,INT8U data,INT32U no,char *ip,INT16U port);
extern INT8U  commun_send_block_data(HUGEDATA COMM_BUF *CBuf,INT32U cmd,INT8U data,INT32U no);
extern char   gmsg[];
extern INT8U  GlbFlagPCnMasterConnection;
extern void display_message_page(char *str);
#pragma pack(2)
typedef struct {
	INT8U operator;
	INT8U ip[4];
	INT8U reserved;
} NETWORK_CLERK_SALE_INFO;
#pragma pack()
NETWORK_CLERK_SALE_AMOUNT ClerkSaleAmount;
//INT8U ClerkLockState[(MAX_CLERK_NO/8)+1];
INT8U ClerkLockState[MAX_CLERK_NO];

void net_set_clerksale(INT8U operator,INT8U *ip)
{
	INT32U addr;
	NETWORK_CLERK_SALE_INFO clerk_sale_info;

	addr=(INT32U)operator;
	addr*=6;
	addr+=NVRAM_CLERK_SALEINFO;
	clerk_sale_info.operator=operator;
	memcpy(clerk_sale_info.ip,ip,4);
	set_nvram_sparam(addr,(INT8U *)&clerk_sale_info,6);
}

void net_get_clerksale(INT8U operator,INT8U *ip)
{
	INT32U 			addr;
	NETWORK_CLERK_SALE_INFO clerk_sale_info;
	memset(ip,0,4);
	if (operator>=MAX_CLERK_NO) return;
	addr=(INT32U)operator;
	addr*=6;
	addr+=NVRAM_CLERK_SALEINFO;
	get_nvram_sparam(addr,(INT8U *)&clerk_sale_info,6);
	if (clerk_sale_info.operator==operator) {
		memcpy((char *)ip,clerk_sale_info.ip,4);
	}
/////////////////////////////////////////////////////////////////////
//	for (i=0; i<MAX_CLERK_NO; i++) {
//		get_nvram_sparam(addr,(INT8U *)&clerk_sale_info,6);
//		if (clerk_sale_info.operator==0) return;
//		if (clerk_sale_info.operator==operator) {
//			memcpy(ip,clerk_sale_info.ip,4);
//			break;
//		}
//		addr+=6;
//	}
/////////////////////////////////////////////////////////////////////
}

void netClerkLockInit(void)
{
	memset(ClerkLockState,0,sizeof(ClerkLockState)); 
}

void netSetClerkLock(INT8U operator, INT8U scaleid, INT8U lock)
{
	INT8U lock_flag;
	lock_flag=get_global_bparam(GLOBAL_SALE_SETUP_FLAG8);
	lock_flag=(lock_flag>>4)&0x03;

	if (!lock_flag) scaleid = 0;
	
	if (lock)
	{
		ClerkLockState[operator] = scaleid;
	}
	else
	{
		ClerkLockState[operator] = 0;
	}
}

INT16S netGetClerkLock(INT8U operator, INT8U scaleid)  //master
{

	INT8U lock;

	INT8U lock_flag;
	lock_flag=get_global_bparam(GLOBAL_SALE_SETUP_FLAG8);
	lock_flag=(lock_flag>>4)&0x03;

	lock = OFF;

	if(lock_flag)	//Modified by JJANG 20080221 LOCK 기능 살림
	{
		if (ClerkLockState[operator]) lock = ON;
		if (ClerkLockState[operator] == scaleid) 	// CJK080315 lock이 되어 있을때 scale id가 같으면 lock 해제
		{
			ClerkLockState[operator] = 0;
			lock = OFF;
		}
	}

	return lock; 
}

/*

void netSetClerkLock(INT8U operator, INT8U lock)
{
	INT8U temp, qt, rm;
	INT8U lock_flag;
	lock_flag=get_global_bparam(GLOBAL_SALE_SETUP_FLAG8);
	lock_flag=(lock_flag>>4)&0x03;

	qt = operator / 8;
	rm = operator % 8;
	temp = ClerkLockState[qt];

	if(!lock_flag) lock = OFF;	//Modified by JJANG 20080221

	if(lock) temp |= (0x01 << rm);
	else temp &= ~(0x01 << rm);

	ClerkLockState[qt] = temp;
}

INT16S netGetClerkLock(INT8U operator)  //master
{

	INT8U lock;
	INT8U temp,qt, rm;

	INT8U lock_flag;
	lock_flag=get_global_bparam(GLOBAL_SALE_SETUP_FLAG8);
	lock_flag=(lock_flag>>4)&0x03;

	lock = OFF;

	if(lock_flag)	//Modified by JJANG 20080221 LOCK 기능 살림
	{
		qt = operator / 8;
		rm = operator % 8;
		temp = ClerkLockState[qt];
		lock = 0x01 & (temp>>rm);
	}

	return lock; 
}
*/

void ClearClerkTareList(void)
{
	nvram_set(NVRAM_CLERK_TARE, 0, MAX_CLERK_NO * 4);
}

void ClearClerkTare(INT8U clerk_id)
{
	set_nvram_lparam(NVRAM_CLERK_TARE + 4 * clerk_id, 0);
}

void SetClerkTare(INT8U clerk_id, long tare_value)
{
	set_nvram_lparam(NVRAM_CLERK_TARE + 4 * clerk_id, tare_value);
	if (ethernet_list.status == 2)
	{
		network_SetClerkData(clerk_id, tare_value, 0x50);
	}
}

long GetClerkTare(INT8U clerk_id)
{
	long clTare;

	if (ethernet_list.status == 1)
	{
		clTare = get_nvram_lparam(NVRAM_CLERK_TARE + 4 * clerk_id);
	}
	else
	{
		// 네트웍 login 시 tare정보가 전송되어 ClerkTareList를 업데이트 되었음
		clTare = get_nvram_lparam(NVRAM_CLERK_TARE + 4 * clerk_id);
	}
	return clTare;
}

void SetClerkCustomerNo(INT8U clerk_id, INT16S value)
{
	set_nvram_wparam(NVRAM_CLERK_CUSTOMER_NO+clerk_id*2 ,value);
	if (ethernet_list.status == 2)
	{
		network_SetClerkData(clerk_id, value, 0x60);
	}
}

void network_error(INT8U code)
{
	INT8U tmp;
	if (network_status.service_flag&0x0008) 
	{
		BuzOn(4);
		tmp = get_global_bparam(GLOBAL_SALE_SETUP_FLAG6);
		tmp = (tmp>>3)&0x01;
		if(tmp)  //big size error message
		{
			display_message_page(global_message[code]);
		}
		else
		{
			DisplayMsg(global_message[code]);
		}
	}
}


/**********************************************************
 Begin. Network Locking
 **********************************************************/
//void command_network_lock(INT8U xx)
//{
//	INT16S i;
//	if (xx) {
//		network_status.lock_command=1;
//		network_status.unlock_command=0;
//	} else {
//		network_status.unlock_command=1;
//		network_status.lock_command=0;
//	}
//	for (i=0; i<ethernet_list.nlist; i++) {
//		ethernet_list.elist[i].enable=xx;
//	}
//}


//void command_network_locking(INT8U xid)
//{
//	INT16S i,sid;
//	sid=command_check_list();
//	if (sid==0) return;
//	if (ethernet_list.status!=1) return;
//	i=sid-1;
//	ethernet_command(ethernet_list.elist[i].ip[0],ethernet_list.elist[i].ip[1],
//		ethernet_list.elist[i].ip[2],ethernet_list.elist[i].ip[3],0x15,xid);
//}

//void command_network_unlocking(void)
//{
//	INT16S i;
//	if (network_status.unlock_command==2) return;
//	for (i=0; i<ethernet_list.nlist; i++) {
//		if (!ethernet_list.elist[i].enable) {
//			ethernet_command(ethernet_list.elist[i].ip[0],ethernet_list.elist[i].ip[1],
//				ethernet_list.elist[i].ip[2],ethernet_list.elist[i].ip[3],0x15,0x02);
//			return;
//		}
//	}
//	network_status.unlock_command=2;
//}

/**********************************************************
 End. Network Locking
 **********************************************************/

//void send_data_request_ready(INT16U code,INT8U step)
//{
//	INT16S slen;//,id;
//	char sdata[72];
//	INT8U bcc;
//	slen  =15;
//	slen += ethernet_gen_head(&sdata[slen]);
//	sprintf(sdata,"i00F%03X,%02XL%03X",code,step,slen-15);
//	sdata[14]=0x3a;
//	bcc=gen_bcc((INT8U *)&sdata[15],slen-15);
//	sdata[slen++]=bcc;
//	ethernet_udpcast(ethernet_list.master.ip[0],ethernet_list.master.ip[1],
//		ethernet_list.master.ip[2],ethernet_list.master.ip[3],sdata,slen);
//}

/**********************************************************
 Begin. Network Login
 **********************************************************/
//void command_login_loaddata(HUGEDATA COMM_BUF *CBuf,INT8U id,INT8U clerk,INT8U *ip) // ID=1 Load,ID=0x10 Delete
void command_login_loaddata(HUGEDATA COMM_BUF *CBuf,INT8U id,INT8U clerk,NETWORK_CLERK_SALE_AMOUNT *amount) // ID=1 Load,ID=0x10 Delete
{
	char *commun_buffer;
	INT16S  slen,start;
	INT8U bcc;

//sprintf(MsgBuf, "[NL]SEND(%02X)c=%d\r\n", id, clerk);
//MsgOut(MsgBuf);
	commun_buffer=(char *)serial_send_point;
	start = 15;
//	if (ip) {
//	       slen = ethernet_gen_head_ip(&commun_buffer[start],ip);
//	} else {
	       slen = ethernet_gen_head(   &commun_buffer[start]   );
//	}
	slen += start;
	if (clerk) {
		sprintf(&commun_buffer[slen],"C=%02X.",clerk);
	} else {
		sprintf(&commun_buffer[slen],"C=%02X.",status_scale.clerkid);
	}
	slen+=strlen(&commun_buffer[slen]);
	sprintf(&commun_buffer[slen],"N=%03X.",network_status.request_clerksaleid);
	slen+=strlen(&commun_buffer[slen]);

	if(amount){
		sprintf(&commun_buffer[slen],"Q=%04X.",amount->totalCount);
		slen+=strlen(&commun_buffer[slen]);
		sprintf(&commun_buffer[slen],"R=%08lX.",amount->totalPrice);
		slen+=strlen(&commun_buffer[slen]);
		sprintf(&commun_buffer[slen],"S=%04X.",amount->voidCount);
		slen+=strlen(&commun_buffer[slen]);
////		sprintf(&commun_buffer[slen],"T=%08lX.",amount.voidPrice);
////		slen+=strlen(&commun_buffer[slen]);
		sprintf(&commun_buffer[slen],"U=%04lX.",amount->tareValue);	// CJK08322
		slen+=strlen(&commun_buffer[slen]);
		sprintf(&commun_buffer[slen],"V=%04X.",amount->customerNo);
		slen+=strlen(&commun_buffer[slen]);
//		sprintf(&commun_buffer[slen],"M=");
//		slen+=2;
//		memcpy(&commun_buffer[slen],amount,sizeof(NETWORK_CLERK_SALE_AMOUNT));
//		slen+=sizeof(NETWORK_CLERK_SALE_AMOUNT);
//		commun_buffer[slen++]='.';
	}

	sprintf(commun_buffer,"i00F017,%02XL%03X",id,slen-start);
	commun_buffer[start-1]=0x3a;
	bcc=gen_bcc((INT8U *)&commun_buffer[start]  ,slen-start);
	commun_buffer[slen++]=bcc;
	commun_buffer[slen  ]=  0;
	commun_outleng(CBuf,commun_buffer,slen);
}

//Added by JJANG 20080307  ioof010,..L
void command_connection_data(HUGEDATA COMM_BUF *CBuf,INT8U id)
{
	char *commun_buffer;
	INT16S  slen,start;
	INT8U bcc;

	commun_buffer=(char *)serial_send_point;
	start = 15;
        slen = ethernet_gen_head(&commun_buffer[start]);
	slen += start;

	sprintf(commun_buffer,"i00F010,%02XL%03X",id,slen-start);
	commun_buffer[start-1]=0x3a;
	bcc=gen_bcc((INT8U *)&commun_buffer[start]  ,slen-start);
	commun_buffer[slen++]=bcc;
	commun_buffer[slen  ]=  0;
	commun_outleng(CBuf,commun_buffer,slen);
}

#ifdef USE_IMPORT_MEAT_PROTOCOL
#ifdef USE_EXT_CHECKING_HARMFULNESS
INT8U command_import_meat_data(HUGEDATA COMM_BUF *CBuf, INT8U use_rs232)
{
	INT8U ret = 0;
	INT8U flag;
	INT8U s_ret = 0;

	network_status.rcv_mt = 0;

	if (use_rs232) command_import_meat_send(CBuf, 0x03, 0);
	else
	{
		s_ret = Ethernet_Server(CLIENT_OPEN, NET_CHK_HARMFUL_SERVER, CLIENT_NORMAL);

		if (s_ret) command_import_meat_send(CBuf, 0x03, 0);
		else goto RET_ESC;
	}

	network_timeout_reset(NETWORK_TIMEOUT_RECV_MEAT);
	network_counter_reset(NETWORK_TIMEOUT_RECV_MEAT);

	while (1) 
	{
		if (network_status.rcv_mt == 1) break;
		else if (network_status.rcv_mt == 2) 
		{
			ret = 1;
			break;
		}

		network_common_trans();
		if (network_timeout_check(2,NETWORK_TIMEOUT_RECV_MEAT)) 	// 0.2초
		{
    		        if (network_counter_check(2, NETWORK_TIMEOUT_RECV_MEAT)) 
    		        {
				command_import_meat_send(CBuf, 0x03, 0);
	    	        } 
	    	        else
	    	        	break; 
		}
	}
RET_ESC:
	if (!use_rs232)
	{
		Ethernet_Server(CLIENT_CLOSE, NET_CHK_HARMFUL_SERVER, CLIENT_NORMAL); 
	}
	return ret;
}
#endif

//i00F030,..N
//02:유통식별번호 수신 성공/실패, 03: 인쇄할 유통식별 번호 전송
void command_import_meat_send(HUGEDATA COMM_BUF *CBuf,INT8U id, INT8U check_bcc)
{
	char *commun_buffer;
	int  slen,start;
	INT8U bcc;
	char 	prt_string[33];

	commun_buffer=(char *)serial_send_point;

	start = 15;
	slen = start;

	if (id == 0x02)
	{
		sprintf(&commun_buffer[slen],"%d",check_bcc);
		slen+=strlen(&commun_buffer[slen]);
	}
	else if (id == 0x03)
	{
		strcpy((char *)prt_string,(char *)CurTraceStatus.indivStr.individualNO);

		slen+= ethernet_gen_head(&commun_buffer[slen]);

		sprintf(&commun_buffer[slen],"0");
		slen+=strlen(&commun_buffer[slen]);
		sprintf(&commun_buffer[slen],"|");
		slen+=strlen(&commun_buffer[slen]);
		sprintf(&commun_buffer[slen],"%s",prt_string);
		slen+=strlen(&commun_buffer[slen]);
	}
	sprintf(&commun_buffer[slen],"|");
	slen+=strlen(&commun_buffer[slen]);

	sprintf(commun_buffer,"i00F030,%02XN%03d",id,slen-start);
	commun_buffer[start-1]=0x3a;
	bcc=gen_bcc((INT8U *)&commun_buffer[start]  ,slen-start);
	commun_buffer[slen++]=bcc;
	commun_buffer[slen  ]=  0;
	commun_outleng(CBuf,commun_buffer,slen);
}
#endif

#ifdef USE_CHECK_INDIV_INFO
//return 0xff : 통신오류 / 0x0 : 정상 / 개체 도축장 등급 이상 :  bitwise OR
void command_send_indiv_info(HUGEDATA COMM_BUF *CBuf);
INT8U command_check_indiv_info(HUGEDATA COMM_BUF *CBuf)
{
	INT8U ret = 0;
	INT8U s_ret = 0;
	network_status.valid_indiv = 0xff;	//수신을 통해 0이 되어야 정상 개체.
	s_ret = Ethernet_Server(CLIENT_OPEN, 0, CLIENT_CHECK_TRACE_VALID);
	if(s_ret)
	{
		command_send_indiv_info(CBuf);
	}
	else 
	{
		ret = 0xff;
        	goto RET_ESC;
	}
	network_timeout_reset(NETWORK_TIMEOUT_CHECK_INDIV);
	network_counter_reset(NETWORK_TIMEOUT_CHECK_INDIV);
	while (1) 
	{
		network_common_trans();
		if (network_status.valid_indiv != 0xff)
		{
			ret = network_status.valid_indiv;
			break;
		}
#ifdef USE_NHMART_SAFE_MEAT
		if (network_timeout_check(30, NETWORK_TIMEOUT_CHECK_INDIV)) 	// 3초
#else
		if (network_timeout_check(60, NETWORK_TIMEOUT_CHECK_INDIV)) 	// 6초
#endif
		{
			ret = 0xff;
    	        	break; 
		}
	}
RET_ESC:
	Ethernet_Server(CLIENT_CLOSE, 0, CLIENT_CHECK_TRACE_VALID);
	return ret;
}
void command_send_indiv_info(HUGEDATA COMM_BUF *CBuf)
{
	char *commun_buffer;
	int  slen,start;
	INT8U bcc;
	char prt_string[TRACE_SLAUGHTHOUSE_NAME_SIZE];
	INT32U addr;

	commun_buffer=(char *)serial_send_point;

	start = 16;
	slen = start;
//data block
	strcpy((char *)prt_string,(char *)CurTraceStatus.indivStr.individualNO);
	sprintf(&commun_buffer[slen],"%s",prt_string);	//개체번호
	slen+=strlen(&commun_buffer[slen]);
	sprintf(&commun_buffer[slen],"|");
	slen+=1;
	
	sprintf(&commun_buffer[slen],"%02d", CurTraceStatus.slPlace);	//도축장 코드
	slen+=2;
	sprintf(&commun_buffer[slen],"|");
	slen+=1;	
	if(CurTraceStatus.slPlace)
	{
		addr =  DFLASH_BASE + FLASH_TRACE_SLAUGHTERHOUSE_AREA;
		addr =  addr + TRACE_SLAUGHTERHOUSE_KR_STRUCT_SIZE * (INT32U)(CurTraceStatus.slPlace-1);
		addr += 2;
		Flash_sread(addr, (HUGEDATA INT8U*)&prt_string[0], TRACE_SLAUGHTHOUSE_NAME_SIZE);
	} 
	else 
	{
		strcpy(prt_string, (char *)CurTraceStatus.indivStr.slaughtHouse);
	}
	sprintf(&commun_buffer[slen],"%s", prt_string);		//도축장 명
	slen+=strlen(&commun_buffer[slen]);
	sprintf(&commun_buffer[slen],"|");
	slen+=1;

	sprintf(&commun_buffer[slen],"%02d", CurTraceStatus.gradeNo);	//등급 코드
	slen+=2;
	sprintf(&commun_buffer[slen],"|");
	slen+=1;

	strcpy((char *)prt_string,(char *)CurTraceStatus.curGradeText);
	sprintf(&commun_buffer[slen],"%s", prt_string);		//등급명
	slen+=strlen(&commun_buffer[slen]);
	sprintf(&commun_buffer[slen],"|");
	slen+=1;
#ifdef USE_NHMART_SAFE_MEAT
	sprintf(&commun_buffer[slen],"%d", status_scale.Weight);	//중량
	slen+=strlen(&commun_buffer[slen]);
	sprintf(&commun_buffer[slen],"|");
	slen+=1;
	sprintf(&commun_buffer[slen],"%06d", status_scale.cur_pluid);	//PLU 품번
	slen+=6;
	sprintf(&commun_buffer[slen],"|");
	slen+=1;
#endif
	
//command 0x01
	sprintf(commun_buffer,"i00F031,%02XL%04X", 0x01, slen-start);
	commun_buffer[start-1]=':';
//bcc
	bcc=gen_bcc((INT8U *)&commun_buffer[start]  ,slen-start);
	commun_buffer[slen++]=bcc;
	commun_buffer[slen  ]=  0;
	commun_outleng(CBuf,commun_buffer,slen);
}
#ifdef USE_NHMART_SAFE_MEAT
void command_call_indiv_info(HUGEDATA COMM_BUF *CBuf);
INT8U command_request_indiv_info(HUGEDATA COMM_BUF *CBuf)
{
	INT8U ret = 0;
	INT8U s_ret = 0;
	network_status.indivInfo = 0xff;	//수신을 통해 0이 되어야 정상 개체.
	s_ret = Ethernet_Server(CLIENT_OPEN, 0, CLIENT_CHECK_TRACE_VALID);
	if(s_ret)
	{
		command_call_indiv_info(CBuf);
	}
	else 
	{
		ret = 0xff;
        	goto RET_ESC;
	}
	network_timeout_reset(NETWORK_TIMEOUT_CHECK_INDIV);
	network_counter_reset(NETWORK_TIMEOUT_CHECK_INDIV);
	while (1) 
	{
		network_common_trans();
		if (network_status.indivInfo != 0xff)
		{
			network_status.indivCallCts++;	// 응답 받으면 조회 횟수 올림. 
			ret = network_status.indivInfo;
			break;
		}
		if (network_timeout_check(30, NETWORK_TIMEOUT_CHECK_INDIV)) 	// 3초
		{
			ret = 0xff;
    	        	break; 
		}
	}
RET_ESC:
	Ethernet_Server(CLIENT_CLOSE, 0, CLIENT_CHECK_TRACE_VALID);
	return ret;
}
void command_call_indiv_info(HUGEDATA COMM_BUF *CBuf)
{
	char *commun_buffer;
	int  slen,start;
	INT8U bcc;
	char prt_string[49];
	INT32U addr;

	commun_buffer=(char *)serial_send_point;

	start = 16;
	slen = start;
//data block
	strcpy((char *)prt_string,(char *)CurTraceStatus.indivStr.individualNO);
	sprintf(&commun_buffer[slen],"%s",prt_string);	//개체번호
	slen+=strlen(&commun_buffer[slen]);
	sprintf(&commun_buffer[slen],"|");
	slen+=1;
	
	sprintf(&commun_buffer[slen],"%06d", status_scale.cur_pluid);	//PLU 품번
	slen+=6;
	sprintf(&commun_buffer[slen],"|");
	slen+=1;

	sprintf(&commun_buffer[slen],"%02d", network_status.indivCallCts);	//조회 횟수
	slen+=2;
	sprintf(&commun_buffer[slen],"|");
	slen+=1;
	
//command 0x01
	sprintf(commun_buffer,"i00F032,%02XL%04X", 0x01, slen-start);
	commun_buffer[start-1]=':';
//bcc
	bcc=gen_bcc((INT8U *)&commun_buffer[start]  ,slen-start);
	commun_buffer[slen++]=bcc;
	commun_buffer[slen  ]=  0;
	commun_outleng(CBuf,commun_buffer,slen);
}
#endif	//#ifdef USE_NHMART_SAFE_MEAT
#endif	//#ifdef USE_CHECK_INDIV_INFO

#ifdef USE_TOPMART_ASK_INDIV_INFO //sewon 170209
void send_indiv_no(HUGEDATA COMM_BUF *CBuf,  HUGEDATA char *indiv_no);
INT8U command_ask_indiv_info(HUGEDATA COMM_BUF *CBuf, HUGEDATA char *indiv_no)
{
	INT8U ret = 0;
	INT8U s_ret = 0;
	network_status.indiv_info_result = 0xff;	//수신을 통해 0이 되어야 정상 개체.

	s_ret = Ethernet_Server(CLIENT_OPEN, 0, CLIENT_CHECK_TRACE_VALID);
	
	if(s_ret)
	{
		send_indiv_no(CBuf,indiv_no);
	}
	else 
	{
		ret = 0xff;
        	goto RET_ESC;
	}
	network_timeout_reset(NETWORK_TIMEOUT_CHECK_INDIV);
	network_counter_reset(NETWORK_TIMEOUT_CHECK_INDIV);

	while (1) 
	{
		network_common_trans();
		if (network_status.indiv_info_result != 0xff)
		{
			ret = network_status.indiv_info_result;
			break;
		}
		if (network_timeout_check(30, NETWORK_TIMEOUT_CHECK_INDIV)) 	// 3초
		{
			ret = 0xff;
    	        	break; 
		}
	}

RET_ESC:
	Ethernet_Server(CLIENT_CLOSE, 0, CLIENT_CHECK_TRACE_VALID);
	return ret;
}
void send_indiv_no(HUGEDATA COMM_BUF *CBuf,  HUGEDATA char *indiv_no)
{
	char *commun_buffer;
	int  slen,start;
	INT8U bcc;
	char prt_string[49];
	INT32U addr;
	INT8U leng;

	commun_buffer=(char *)serial_send_point;

	start = 16;
	slen = start;

//data block
	//strcpy((char *)prt_string,(char *)CurTraceStatus.indivStr.individualNO);
	strcpy((char *)prt_string,indiv_no);
	sprintf(&commun_buffer[slen],"%s",prt_string);	//개체번호
	slen+=strlen(&commun_buffer[slen]);
		
//command 0x01
	leng = slen-start;
	sprintf(commun_buffer,"i00F055,%02XL%04X", 0x01, leng);
	commun_buffer[start-1]=':';
//bcc
	bcc=gen_bcc((INT8U *)&commun_buffer[start]  ,leng);
	commun_buffer[slen++]=bcc;
	commun_buffer[slen  ]=  0;
	commun_outleng(CBuf,commun_buffer,slen);
}
#endif

INT8U command_send_speedkey_value(char* key_type, int key_position)
{
	INT8U ret = 0;
	COMM_BUF *CBuf;
	char *commun_buffer;
	int  slen,start;
	INT8U bcc;
	
	commun_buffer=(char *)serial_send_point;
	start = 16;
	slen = start;
//data block
	sprintf(&commun_buffer[slen],"%s",key_type);
	slen+=strlen(&commun_buffer[slen]);
	sprintf(&commun_buffer[slen],"|");
	slen+=1;
	sprintf(&commun_buffer[slen],"%06d",key_position);
	slen+=strlen(&commun_buffer[slen]);
	sprintf(&commun_buffer[slen],"|");
	slen+=1;
	
//command 0x01
	sprintf(commun_buffer,"i00F034,%02XL%04X", 0x01, slen-start);
	commun_buffer[start-1]=':';
//bcc
	bcc=gen_bcc((INT8U *)&commun_buffer[start]  ,slen-start);
	commun_buffer[slen++]=bcc;
	commun_buffer[slen  ]=  0;
	
	if((ethernet_list.remoteip[0]) || (ethernet_list.remoteip[1])) {
		CBuf = &CommBufEthData[0];
		Ethernet_Server(CLIENT_OPEN, 0, CLIENT_SEND_SPEEDKEY_VAL);
		commun_outleng(CBuf,commun_buffer,slen);
		network_common_trans();
		Ethernet_Server(CLIENT_CLOSE, 0, CLIENT_SEND_SPEEDKEY_VAL);
	}
	
	CBuf = &CommBuf;
	commun_outleng(CBuf,commun_buffer,slen);

	return ret;
}

#ifdef USE_SCANNER_FOR_ECO_FRIENDLY
INT8U command_ask_eco_barcode(char* barString)
{
	INT8U ret = 0;
	INT8U s_ret = 0;
	int  slen,start;
	INT8U bcc;
	char prt_string[14];
	INT32U addr;
	char *commun_buffer;	
	HUGEDATA COMM_BUF *CBufEco;

	network_status.ecoInfo = 0xff;
	s_ret = Ethernet_Server(CLIENT_OPEN, 0, CLIENT_CHECK_TRACE_VALID);
	if(s_ret)
	{
		CBufEco = &CommBufEthData[0];
		commun_buffer = (char *)serial_send_point_for_client;
		start = 16;
		slen = start;
	//data block
		strcpy((char *)prt_string, "880");
		strcpy(&prt_string[3], barString);
		sprintf(&commun_buffer[slen], "%s", prt_string);	//친환경 인증 바코드
		slen += strlen(&commun_buffer[slen]);
		sprintf(&commun_buffer[slen], "|");
		slen += 1;
	//command 0x01
		sprintf(commun_buffer,"i00F033,%02XL%04X", 0x01, slen-start);
		commun_buffer[start-1] = ':';
	//bcc
		bcc = gen_bcc((INT8U *)&commun_buffer[start], slen-start);
		commun_buffer[slen++] = bcc;
		commun_buffer[slen  ] = 0;
		commun_outleng(CBufEco, commun_buffer, slen);
	}
	else 
	{
		ret = 0xff;
        	goto RET_ESC;
	}
	network_timeout_reset(NETWORK_TIMEOUT_CHECK_INDIV);
	network_counter_reset(NETWORK_TIMEOUT_CHECK_INDIV);


	while (1) 
	{
		network_common_trans();
		if (network_status.ecoInfo != 0xff)
		{
			ret = network_status.ecoInfo;	//0 일 때 정상
			break;
		}
		if (network_timeout_check(100, NETWORK_TIMEOUT_CHECK_INDIV)) 	// 10초
		{
			ret = 0xff;
    	    break; 
		}
	}

RET_ESC:
	Ethernet_Server(CLIENT_CLOSE, 0, CLIENT_CHECK_TRACE_VALID);
	return ret;
}
#endif
void command_reopen_data(HUGEDATA COMM_BUF *CBuf, INT8U id, INT32U TicketNum, INT16U ClerkNum, INT8U Result)
{
	char *commun_buffer;
	INT16S  slen,start;
	INT8U bcc;

	commun_buffer=(char *)serial_send_point;
	start = 15;
	slen = ethernet_gen_head(&commun_buffer[start]);
	slen += start;

	sprintf(&commun_buffer[slen],"C=%02X.",ClerkNum);
	slen+=strlen(&commun_buffer[slen]);
	sprintf(&commun_buffer[slen],"#=%05lX.",TicketNum);
	slen+=strlen(&commun_buffer[slen]);

	if (id == 0x66)
	{
		sprintf(&commun_buffer[slen],"P=%02X.",Result);
		slen+=strlen(&commun_buffer[slen]);
	}
	sprintf(commun_buffer,"i00F017,%02XL%03X",id,slen-start);
	commun_buffer[start-1]=0x3a;
	bcc=gen_bcc((INT8U *)&commun_buffer[start]  ,slen-start);
	commun_buffer[slen++]=bcc;
	commun_buffer[slen  ]=  0;
	commun_outleng(CBuf,commun_buffer,slen);
}



/*
void m2s_command_data_ready(INT8U mode,INT8U oper,INT8U datatype,INT8U deptno,INT32U nth,ETH_CONNECT_LIST eth)
{
	INT16S slen,id,sid;
	char sdata[96];
	INT8U bcc;
	if ((eth.ip[0]==0) || (eth.ip[0]==0xff)) return;
	sid=0;
	slen  = 15;
	slen += ethernet_gen_head(&sdata[slen]);
	id = (INT16S)command_queue.cmdid;
	sprintf(&sdata[slen],"D=%02X.",datatype); slen+=strlen(&sdata[slen]);
	sprintf(&sdata[slen],"P=%02X.",deptno  ); slen+=strlen(&sdata[slen]);
	sprintf(&sdata[slen],"N=%05X.",nth     ); slen+=strlen(&sdata[slen]);
	sprintf(sdata,"i00F%03X,%02XL%03X",mode,oper,slen-15); // i00F018...i00F019
	sdata[14]=0x3a;
	bcc=gen_bcc((INT8U *)&sdata[15],slen-15);
	sdata[slen++]=bcc;
	ethernet_udpcast(eth.ip[0],eth.ip[1],eth.ip[2],eth.ip[3],sdata,slen);
}
*/

void plu_modify_send(void)
{
	commun_send_plu_modify_scale();
}

//Modified by JJANG 20080121
void transaction_sale_set_tail(INT16U tail)
{
	//INT8U temp;
	
	//temp=get_global_bparam(GLOBAL_SALE_SETUP_FLAG4);
	//if( (ethernet_list.status==3) && (temp&0x40) )
#ifdef USE_REALTIME_TRANS_TYPE3
	if((ethernet_list.status == 3) && (GlbOper.realtimeTransactionType == 3))	// 3:Full
	{
		tail%=Startup.max_realsend_buf;
	}
	else
	{
		tail%=Startup.max_send_buf;
	}
#else
	tail%=Startup.max_send_buf;
#endif
	//network_status.send_trans_sale=1;
	set_nvram_wparam(NVRAM_SEND_REMAINDED_TAIL,tail);
}

void transaction_pay_set_tail(INT8U tail)
{
	tail%=Startup.max_paysend_buf;	//Modified by JJANG 20080310
	//network_status.send_trans_pay=1;
	set_nvram_bparam(NVRAM_TRANS_PAY_TAIL,tail);
}

//void send_clerk_sale_data(HUGEDATA COMM_BUF *CBuf,ETH_CONNECT_LIST eth,INT8U ClerkNum,INT16S id,TRANSACTION_RECORD_TYPE *trans)
void send_clerk_sale_data(HUGEDATA COMM_BUF *CBuf,ETH_CONNECT_LIST eth,INT8U ClerkNum,INT16S id,TRANSACT_CHAIN *cp)
{
	char  *commun_buffer;
	INT16S   slen,r,cr,i,cx;
	INT8U bcc;
	TRANSACTION_RECORD_TYPE *trans_rec;
	TRANSACT_CHAIN *xtrans;
	TRANSACT_CHAIN tc;
	INT16U next;
//	char temp[32];

	commun_buffer=(char *)serial_send_point;
	r     = 15;
	slen  = ethernet_gen_head(&commun_buffer[r]);
	slen += r;
	cx   = slen;
	if (cp==NULL) {
EX:
//		sprintf(temp,"Send 0x05->end\r\n");
//		MsgOut(temp);
		sprintf(&commun_buffer[slen],"C=%02X.",ClerkNum);
		slen+=strlen(&commun_buffer[slen]);
		sprintf(&commun_buffer[slen],"N=%03X.",id);
		slen+=strlen(&commun_buffer[slen]);
//		sprintf(&commun_buffer[slen],"X=0.");
//		slen+=strlen(&commun_buffer[slen]);
		if (ethernet_list.status==1) {
			sprintf(&commun_buffer[slen],"#=%lX.", get_TicketTransactionNumber(ClerkNum));
			slen+=strlen(&commun_buffer[slen]);
		}
		sprintf(commun_buffer,"i00F017,05L%03X",slen-r);
	} else {
		xtrans = cp;
		sprintf(&commun_buffer[slen],"C=%02X.",ClerkNum);
		slen+=strlen(&commun_buffer[slen]);
		cr=slen;
		// dummy ////
		sprintf(&commun_buffer[slen],"N=%03X.",id);
		slen+=strlen(&commun_buffer[slen]);
		sprintf(&commun_buffer[slen],"X=%02X.",1);
		slen+=strlen(&commun_buffer[slen]);
		////////////
//		cr=slen-2;
//		sprintf(temp,"Send 0x06-> in=[%d]",id);
		for (i=0; i<10;) {
			trans_rec=(TRANSACTION_RECORD_TYPE *)&(xtrans->content);
#ifdef USE_CHN_CART_SCALE
			memcpy(&commun_buffer[slen],trans_rec,(sizeof(TRANSACTION_RECORD_TYPE)-TRACE_CODE_SIZE));
			slen+=(sizeof(TRANSACTION_RECORD_TYPE)-TRACE_CODE_SIZE);
#else
			memcpy(&commun_buffer[slen],trans_rec,sizeof(TRANSACTION_RECORD_TYPE));
			slen+=sizeof(TRANSACTION_RECORD_TYPE);
#endif
			next = xtrans->next;
			//xtrans = cpNext;
			i++;
			if (!next) break;
			get_nvram_trans_addr(NVRAM_TRANS_DATA, next - 1, &tc);
			xtrans = &tc;
//			id++;
			if (!xtrans) break;
		}
		if (i==0) {
			slen=cx;
			goto EX;
		}
		sprintf(&commun_buffer[cr],"N=%03X.X=%02X",id+i-1,i);
		cr+=strlen(&commun_buffer[cr]);
		commun_buffer[cr]='.';
//		sprintf(temp,"Send 0x06,nRec=%d,nth=%d\r\n",i,id);
//		MsgOut(temp);
/// ] end new
		sprintf(commun_buffer,"i00F017,06L%03X",slen-r);
	}
	commun_buffer[r-1]=':';
	bcc=gen_bcc((INT8U *)&commun_buffer[15],slen-r);
	commun_buffer[slen++]=bcc;
	commun_outleng(CBuf,commun_buffer,slen);
//	sprintf(temp,"Send-E\r\n");
//	MsgOut(temp);
}

void transaction_sale_end(HUGEDATA COMM_BUF *CBuf,INT8U clerk,INT16U tail)
{
	char  *commun_buffer;
	INT16S   slen;
	INT8U bcc;

	commun_buffer=(char *)serial_send_point;
	slen  = ethernet_gen_head(&commun_buffer[15]);
	slen += 15;
	sprintf(&commun_buffer[slen],"C=%02X.",clerk); slen+=5;
	sprintf(&commun_buffer[slen],"T=%04X.",tail ); slen+=7;
	sprintf(commun_buffer,"i00F070,02L%03X",slen-15);
	commun_buffer[14    ]=':';
	bcc=gen_bcc((INT8U *)&commun_buffer[15],slen-15);
	commun_buffer[slen++]=bcc;
	commun_outleng(CBuf,commun_buffer,slen);
}

//Modified by JJANG 20080121
//void transaction_sale_send_memory_full(HUGEDATA COMM_BUF *CBuf,INT8U clerk,INT16U tail)
void transaction_sale_send_memory_full(HUGEDATA COMM_BUF *CBuf, INT8U cmd, INT8U clerk, INT16U tail)
{
	char  *commun_buffer;
	INT16S   slen;
	INT8U bcc;

	commun_buffer=(char *)serial_send_point;
	slen  = ethernet_gen_head(&commun_buffer[15]);
	slen += 15;
	sprintf(&commun_buffer[slen],"C=%02X.",clerk); slen+=5;
//마지막 한번 송신("i00F017,05...")
//DEL070123	if (ethernet_list.status==1) {
//DEL070123		sprintf(&commun_buffer[slen],"#=%08lX.",TicketTransactionNumber[clerk]);
//DEL070123		slen+=strlen(&commun_buffer[slen]);
//DEL070123	}
	sprintf(&commun_buffer[slen],"T=%04X.",tail ); slen+=7;
	if (cmd == 0x16) sprintf(commun_buffer,"i00F070,08L%03X",slen-15);	//090915
	else sprintf(commun_buffer,"i00F070,04L%03X",slen-15);
	commun_buffer[14    ]=':';
	bcc=gen_bcc((INT8U *)&commun_buffer[15],slen-15);
	commun_buffer[slen++]=bcc;
	commun_outleng(CBuf,commun_buffer,slen);
}


//Modified by JJANG 20080121
void transaction_sale_send_abort(HUGEDATA COMM_BUF *CBuf,INT8U clerk,INT16U tail)
{
	char  *commun_buffer;
	INT16S   slen;
	INT8U bcc;

	commun_buffer=(char *)serial_send_point;
	slen  = ethernet_gen_head(&commun_buffer[15]);
	slen += 15;
	sprintf(&commun_buffer[slen],"C=%02X.",clerk); slen+=5;
//마지막 한번 송신("i00F017,05...")
//DEL070123	if (ethernet_list.status==1) {
//DEL070123		sprintf(&commun_buffer[slen],"#=%08lX.",TicketTransactionNumber[clerk]);
//DEL070123		slen+=strlen(&commun_buffer[slen]);
//DEL070123	}
	sprintf(&commun_buffer[slen],"T=%04X.",tail ); slen+=7;
	sprintf(commun_buffer,"i00F070,05L%03X",slen-15);
	commun_buffer[14    ]=':';
	bcc=gen_bcc((INT8U *)&commun_buffer[15],slen-15);
	commun_buffer[slen++]=bcc;
	commun_outleng(CBuf,commun_buffer,slen);
}

//Modified by JJANG 080121
void transaction_sale_send_fail(HUGEDATA COMM_BUF *CBuf,INT8U clerk,INT16U tail)
{
	char  *commun_buffer;
	INT16S   slen;
	INT8U bcc;

	commun_buffer=(char *)serial_send_point;
	slen  = ethernet_gen_head(&commun_buffer[15]);
	slen += 15;
	sprintf(&commun_buffer[slen],"C=%02X.",clerk); slen+=5;
//마지막 한번 송신("i00F017,05...")
//DEL070123	if (ethernet_list.status==1) {
//DEL070123		sprintf(&commun_buffer[slen],"#=%08lX.",TicketTransactionNumber[clerk]);
//DEL070123		slen+=strlen(&commun_buffer[slen]);
//DEL070123	}
	sprintf(&commun_buffer[slen],"T=%04X.",tail ); slen+=7;
	sprintf(commun_buffer,"i00F070,06L%03X",slen-15);
	commun_buffer[14    ]=':';
	bcc=gen_bcc((INT8U *)&commun_buffer[15],slen-15);
	commun_buffer[slen++]=bcc;
	commun_outleng(CBuf,commun_buffer,slen);
}


// void transaction_pay_end(HUGEDATA COMM_BUF *CBuf,ETH_CONNECT_LIST eth,INT8U tail)
void transaction_pay_end(HUGEDATA COMM_BUF *CBuf,INT8U tail)
{
	char  *commun_buffer;
	INT16S   slen;
	INT8U bcc;

	commun_buffer=(char *)serial_send_point;
	slen  = ethernet_gen_head(&commun_buffer[15]);
	slen += 15;
	sprintf(&commun_buffer[slen],"C=00."); slen+=strlen(&commun_buffer[slen]);
	sprintf(&commun_buffer[slen],"T=%02X.",tail); slen+=strlen(&commun_buffer[slen]);
	sprintf(commun_buffer,"i00F071,02L%03X",slen-15);

	commun_buffer[14]=':';
	bcc=gen_bcc((INT8U *)&commun_buffer[15],slen-15);
	commun_buffer[slen++]=bcc;
	commun_outleng(CBuf,commun_buffer,slen);
}

void transaction_pay_send_abort(HUGEDATA COMM_BUF *CBuf,INT8U tail)
{
	char  *commun_buffer;
	INT16S   slen;
	INT8U bcc;

	commun_buffer=(char *)serial_send_point;
	slen  = ethernet_gen_head(&commun_buffer[15]);
	slen += 15;
	sprintf(&commun_buffer[slen],"C=00."); slen+=strlen(&commun_buffer[slen]);
	sprintf(&commun_buffer[slen],"T=%02X.",tail); slen+=strlen(&commun_buffer[slen]);
	sprintf(commun_buffer,"i00F071,05L%03X",slen-15);

	commun_buffer[14]=':';
	bcc=gen_bcc((INT8U *)&commun_buffer[15],slen-15);
	commun_buffer[slen++]=bcc;
	commun_outleng(CBuf,commun_buffer,slen);
}

/* Command 0x019 */
/*
void recv_data_request(HUGEDATA COMM_BUF *CBuf,INT16U cmd,INT16S param2,INT8U *str,INT16S length)
{
	ETH_CONNECT_LIST eth;
	INT32U v32;
	INT32U addr;
	INT16S status,r;
	INT32U nth,v32_max;
	INT8U  deptno,code,datatype,s_ok;
	TRANSACTION_RECORD_TYPE trans;
//	char temp[32];

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
					switch (code) {
					    case 'D': datatype=(INT8U)v32; break;
					    case 'P': deptno  =(INT8U)v32; break;
					    case 'N': nth     =v32;   break;
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
//	sprintf(temp,"cmd=%04x,%d,%d",datatype,deptno,nth);
//	MsgOut((HUGEDATA char *)temp);
	switch (cmd) {
	    case 0x19:switch (datatype) {
			     case 0x10: //==================== PLU Send
				if (nth) {
					addr=plu_table_search(deptno,nth);
//					sprintf(temp,"Addr=%04x,%d,%d",addr,deptno,nth);
//					MsgOut((HUGEDATA char *)temp);
					commun_send_plu_from_address(CBuf,addr,nth,(char *)eth.ip,eth.port);
				}
	   			break;
		      }
		      break;
	    case 0x23:switch (datatype) {
			    case 0x10:
				v32_max=plu_get_last_id((INT8U)deptno);
				s_ok   =0;
			        for (v32=nth; v32<v32_max; v32++) {
					addr=plu_table_search(deptno,v32);
					if (addr) {
						s_ok=1;
						commun_send_plu_from_address(CBuf,addr,v32,(char *)eth.ip,eth.port);
						break;
					}
				}
				if (s_ok==0) {
					v32=100000l;
					commun_send_plu_from_address(CBuf,0,v32,(char *)eth.ip,eth.port);
				}
				break;
		      }
		      break;
	    case 0x25:
//	    sprintf(temp,"c25=%02x,%05lx\r\n",datatype,nth);
//	    MsgOut((HUGEDATA char *)temp);
	    	      switch (datatype) {
			    case 0x10:
			        if (nth==0) nth=1;
				addr = plu_get_next_exist_address(nth);
//	    sprintf(temp,"addr=%05x\r\n",addr);
//	    MsgOut((HUGEDATA char *)temp);
				if (addr!=0xffff) {
//					addr++;
					commun_send_plu_from_address(CBuf,addr,v32,(char *)eth.ip,eth.port);
				} else {
					v32=100000l;
					commun_send_plu_from_address(CBuf,0   ,v32,(char *)eth.ip,eth.port);
				}
				break;
		      }
		      break;
		case 0x31:
//		      ethernet_set_service(0,NULL,0);
		      commun_send_block_data(CBuf,0x08,deptno,nth);
		      break;
	}
}
*/

extern INT8U NetClerkLockRelease(INT8U operator);
//=======================================================
// NetworkGetTransactionData()
// Slave 저울에서 void, print 등의 키를 눌렀을 때 
// Master로 부터 Clerk Sale 데이터를 가져옴
//=======================================================
INT16S NetworkGetTransactionData(void)
{
	INT16S rtn;
	rtn=0;

	//slave에서는 ReopenedTransPay를 사용하지 않음
       	ClerkDeleteTransaction(status_scale.clerkid);
	memset(&ReopenedTransPay, 0, sizeof(TRANSACTION_PAY));
     	//network_status.load_lastsale = 1;	//DEL CJK080315
     	network_status.request_clerksaleid=0;
    	rtn = network_login(status_scale.clerkid, status_scale.scaleid, 0x03);
	if (rtn == 2)	// clerk lock type 2(확인 후 해제)
	{
		rtn = NetClerkLockRelease(status_scale.clerkid);
	}

	return rtn;
}

//check master scale <--> pc connection
INT16S NetCheckMasterConnection(void)
{
	INT16S ret;
	INT16S nretry=0;
	
	if(ethernet_list.status != 1 && ethernet_list.status != 3) return ON;
	if((!ethernet_list.remoteip[0]) && (!ethernet_list.remoteip[1])) return ON;
	
	ret=0;
	nretry=0;
	while (!ret) 
	{
//debug_tic(0);	//DBG080314
		ret = Ethernet_Server(CLIENT_OPEN, NET_CHK_PC_WAITTIME, CLIENT_CHECK_PC);
		nretry++;
		if (nretry >= NET_CHK_MASTER_RETRY) break;
	}
//debug_tic('!');	//DBG080314
	if (ret) 
	{
		Ethernet_Server(CLIENT_CLOSE, NET_CHK_PC_WAITTIME, CLIENT_CHECK_PC);
	}
//debug_tic('@');	//DBG080314
	
	return ret;
}

INT16S NetCheckConnection(void)
{
	INT16S ret, flag;
	INT16S nretry=0;
	INT8U  cTemp;

	ret = ON;

	cTemp = get_global_bparam(GLOBAL_SALE_SETUP_FLAG1);
	cTemp = cTemp & 0x10;
	if(!cTemp) return ON;

	if(ethernet_list.status == 1) //master
	{
		ret = NetCheckMasterConnection();
	}
	else if( ethernet_list.status == 2 ) //slave
	{
		flag = 0;
		GlbFlagPCnMasterConnection = OFF;

		if (Operation.operationClerk == OPER_FLOATING) // && ethernet_list.status == 2) 
		{
		    nretry=0;
		    while (!flag) 
		    {
			flag=Ethernet_Server(CLIENT_OPEN, 0, CLIENT_CLERK_TRANS);
			nretry++;
			if (nretry>4) break;
		    }

		    if (flag==1) 
		    {
			network_direct(1);
			network_timeout_reset(NETWORK_TIMEOUT_REQUEST_LOGIN);
			network_counter_reset(NETWORK_TIMEOUT_REQUEST_LOGIN);
			command_login_loaddata(&(CommBufEthData[0]),0x70,0,NULL);  //pc connection check command
			network_status.clerk_saledelete=1;
			while (1) 
			{
			    network_common_trans();
			    if (network_status.clerk_saledelete==0) break;
			    if (network_timeout_check(10,NETWORK_TIMEOUT_REQUEST_LOGIN)) {
		    		if (network_counter_check(2,NETWORK_TIMEOUT_REQUEST_LOGIN)) {
					command_login_loaddata(&(CommBufEthData[0]),0x70,0,NULL); // Retry
				} else {
					flag=0;
			        	break;
			    	}
			    }
			}

			network_direct(0);
			Ethernet_Server(CLIENT_CLOSE, 0, CLIENT_CLERK_TRANS);
		    }
		    else 
		    {
			//network_error(MSG_21_NET_ERR);
			flag = OFF;
		    }

		    ret = GlbFlagPCnMasterConnection;
		}
	}
	else if (ethernet_list.status == 4) //local
	{
		ret = OFF;
	}
	else
	{
		ret = ON;
	}

	return ret;
}

INT16S NetClerkLock(INT8U operator)
{
       INT16S ret;

       if (Operation.operationClerk != OPER_FLOATING) return 1;

       ret=1;
       if(ethernet_list.status==2) 
       {  // Client
		ret = NetworkGetTransactionData();	// 0:clerk lock, 1:ok, 2:clerk lock 해제, 3:timeout
//		if(ret == 0)
//		{
//			BuzOn(2);
//			//Show "Lock" or other Message 	
//			ret = 0;
//			// return 0;
//		} 
	} 
	else if (ethernet_list.status==1) 
	{ // Master
		if ( netGetClerkLock(operator, status_scale.scaleid)){
			BuzOn(2);
			//Show "Lock" Message 	
			ret = 0;
			//return 0;
		} 
		else 
		{
			netSetClerkLock(operator, status_scale.scaleid, ON);			
		} 
	}

	return ret;
}

INT8U NetClerkLockRelease(INT8U operator)
{
	INT8U ret;
	
	if (Operation.operationClerk != OPER_FLOATING) return 1;

	ret = 1;	// ok
	if(ethernet_list.status==1)
	{
		netSetClerkLock(operator, status_scale.scaleid, OFF);
	}
	else if(ethernet_list.status==2)
	{
//sprintf(gmsg,"SGDebug:release\r\n");
//MsgOut(gmsg);

		ret = network_login(operator, status_scale.scaleid, 0x43);  //release lock command
//		command_login_loaddata(&(CommBufEthData[0]),0x43,(INT8U)operator,NULL);
		// 삭제. 테스트 필요
//		if (Operation.operationClerk == OPER_FLOATING)
//		{
//			ClerkClearTransaction();
//		}
	}
	return ret;
}


INT8U network_SetClerkData(INT8U clerk, long value, INT8U command)
{
	INT8U ret,nretry;
	NETWORK_CLERK_SALE_AMOUNT clerkAmount;

	memset(&clerkAmount,0, sizeof(NETWORK_CLERK_SALE_AMOUNT));

	if (command == 0x50)	clerkAmount.tareValue = value;
	else if (command == 0x60) clerkAmount.customerNo = value;

	ret=0;
	if (Operation.operationClerk == OPER_FLOATING && ethernet_list.status == 2) 
	{
	    nretry=0;
	    while (!ret) 
	    {
		ret=Ethernet_Server(CLIENT_OPEN, 0, CLIENT_NORMAL);
		nretry++;
		if (nretry>3) break;
	    }

	    if (ret==1) 
	    {
		network_direct(1);
		network_timeout_reset(NETWORK_TIMEOUT_REQUEST_LOGIN);
		network_counter_reset(NETWORK_TIMEOUT_REQUEST_LOGIN);
		command_login_loaddata(&(CommBufEthData[0]),command,clerk,&clerkAmount);
		network_status.clerk_saledelete=1;
		while (1) 
		{
		    network_common_trans();
		    if (network_status.clerk_saledelete==0) break;	        

		    if (network_timeout_check(8,NETWORK_TIMEOUT_REQUEST_LOGIN))
		    {
	    		if (network_counter_check(2,NETWORK_TIMEOUT_REQUEST_LOGIN)) 
	    		{
				command_login_loaddata(&(CommBufEthData[0]),command,clerk,&clerkAmount); // Retry
			} 
			else 
			{
				ret=0;
		        	break;
		    	}
		    }
		}
		network_direct(0);
		Ethernet_Server(CLIENT_CLOSE, 0, CLIENT_NORMAL);
	    } 
	    else 
	    {
		network_error(MSG_21_NET_ERR);
	    }
	}
	else ret = 1;
	return ret;
}


void network_search_slave(void)
{
	ETHERNET_SLAVE_SCALE slave_str;
 	INT16U	str_size;		 
	INT32U	addr_start; 
	INT32U	addr_flash;
	INT8U 	i;
	    
	str_size = sizeof(slave_str);
	addr_start = DFLASH_BASE + FLASH_SLAVE_INFO;

	memset((INT8U *)&NetSlaveInfo[0], 0, str_size*MAX_SLAVE_SCALE);

	if (ethernet_list.status == 2)
	{
		slave_str.code = MAX_SCALE;
		slave_str.ip[0] = EthCardStruct.TCPDestIP[3];
		slave_str.ip[1] = EthCardStruct.TCPDestIP[2];
		slave_str.ip[2] = EthCardStruct.TCPDestIP[1];
		slave_str.ip[3] = EthCardStruct.TCPDestIP[0];
		slave_str.tcp_port = EthCardStruct.TCPDestPort;
		slave_str.use = 1;
		NetSlaveInfo[MAX_SLAVE_SCALE - 1] = slave_str;
		SyncStatus[MAX_SLAVE_SCALE - 1] = 0;	// initial
		return;
	}

	for (i = 0; i < MAX_SLAVE_SCALE; i++)
	{
		addr_flash = addr_start + (str_size * i);
		Flash_sread(addr_flash, (HUGEDATA INT8U *)&slave_str, str_size);
		if (slave_str.code == i + 1 && slave_str.use)  
		{
			//Flash_sread(addr_flash, (HUGEDATA INT8U *)&NetSlaveInfo[i], str_size);
			NetSlaveInfo[i] = slave_str;
		}
		SyncStatus[i] = 0;	// initial
	}
}

INT8U network_get_scale_list_id(INT8U *ip)
{
	INT8U 	i;
	INT32U  ip_val, temp_ip_val;

	memcpy((INT8U *)&ip_val, ip, 4);
	for (i = 0; i < MAX_SLAVE_SCALE; i++)
	{
		memcpy((INT8U *)&temp_ip_val, NetSlaveInfo[i].ip, 4);
		if (ip_val == temp_ip_val) return i + 1;
	}
	return 0;
}

#ifdef USE_ADMIN_EMAIL
INT8U command_send_realtime_alram(void)
{
	COMM_BUF *CBuf;
	char  *commun_buffer;
	INT16S   r,com_type,start_r;
	INT8U bcc;
	INT32U cmd;
	INT8U action;
	INT8U ip[4];	
	INT8U flag;
	INT8U ret;
	char model[2];
	FW_VERSION  fversion;

	if (!Ethernet_Server(CLIENT_OPEN, 0, CLIENT_CLERK_TRANS)) return 0;

	CBuf = &CommBufEthData[0];

	action = 1;
	cmd = 37;
	ret = TRUE;

	com_type=serial_get_comtype(CBuf);
	commun_buffer=(char *)serial_send_point;
	start_r = 9;
	r = start_r;
	sprintf((char *)&commun_buffer[r],"c=%03lX.",cmd);
	r+= 6;
	
	version_get(&fversion);
	sprintf(model,"%c%c",fversion.model[0],fversion.model[1]);// model name - CL:CL5000, C3 : CL3500 (not use), C4 : CL5200, C7 : CL7200
	sprintf(&commun_buffer[r],"M=%02X.%s",strlen(model), model);
	r+=strlen(&commun_buffer[r]);
	get_net_sparam(NETWORK_LOCAL_IP, ip, 4);
	sprintf((char *)&commun_buffer[r],"I=%02X%02X%02X%02X.", ip[0], ip[1], ip[2], ip[3]);
	r+=strlen((char *)&commun_buffer[r]);
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
	
	if (ret==FALSE) {
		sprintf(commun_buffer,"I%02X:E84%c",(INT8U)action,0x0a);
		r = strlen((char *)commun_buffer);
	} else {
		sprintf(commun_buffer,"I%02X:L%03X",(INT8U)action,r-start_r);
		commun_buffer[start_r-1]=',';
		bcc=gen_bcc((INT8U *)&commun_buffer[start_r],r-start_r);
		commun_buffer[r++]=bcc;
	}
	commun_outleng(CBuf,commun_buffer,r);

	Ethernet_Server(CLIENT_CLOSE, 0, CLIENT_CLERK_TRANS);
}


#endif

