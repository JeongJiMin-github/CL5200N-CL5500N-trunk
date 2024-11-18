#ifndef _NETWORK_STATUS_TIME
#define _NETWORK_STATUS_TIME
#ifdef USE_EFT_POS_PROTOCOL
  #define NETWORK_TIMEOUT_MAX					20
  #define NETWORK_TIMEOUT_REMOTE2				19
  #define NETWORK_TIMEOUT_EFT_POS_PROTOCOL		18
#else
#define NETWORK_TIMEOUT_MAX						18
#endif
#define NETWORK_TIMEOUT_CHECK_RTC				17
#define NETWORK_TIMEOUT_CHECK_INVENTORY			16
#define NETWORK_TIMEOUT_SEND_PAY				15
#define NETWORK_TIMEOUT_REALTIME_SEND			14
#define NETWORK_TIMEOUT_CHECK_INDIV    			13
#define NETWORK_TIMEOUT_BAT_SIGN    			12
#define NETWORK_TIMEOUT_RECV_MEAT    			11
#define NETWORK_TIMEOUT_CHECK_WLAN	 			10
#define NETWORK_TIMEOUT_RCV_INTERVAL 			9
#define NETWORK_TIMEOUT_SEND_TRACE				8
#define NETWORK_TIMEOUT_RECV_CHECK				7	//Added by JJANG 20080307
#define NETWORK_TIMEOUT_SEND_CHECK				6	//Added by JJANG 20080307
#define NETWORK_TIMEOUT_CLERK_DEL      		 	5	//Added by JJANG 20080131
#define NETWORK_TIMEOUT_COMMANDREADY   		 	4
#define NETWORK_TIMEOUT_REQUEST_LOGIN			3
#define NETWORK_TIMEOUT_SLAVE					2
#define NETWORK_TIMEOUT_TIMESYNC				0
#define NETWORK_TIMEOUT_TRANSACTION 			1
#endif

#ifndef _NETWORK_SERVICE_TYPE_LIST
#define _NETWORK_SERVICE_TYPE_LIST

#define NETWORK_SERVICE_MAX		4
#define NETWORK_SERVICE_NONE	0
#define NETWORK_SERVICE_MASTER	1
#define NETWORK_SERVICE_SLAVE	2
#define NETWORK_SERVICE_NETWORK	3
#define NETWORK_SERVICE_LOCAL	4

enum NETWORK_SEND_TRAN_STAT {
	EM_NET_SEND_TRAN_TIMEOUT = 0,
	EM_NET_SEND_TRAN_OK,
	EM_NET_SEND_TRAN_ADD_FULL,
	EM_NET_SEND_TRAN_SAME_DATA,
	EM_NET_SEND_TRAN_WRONG_DATA,
	EM_NET_SEND_TRAN_FULL,
	EM_NET_SEND_TRAN_SERIAL_NO_ERR,
};

#endif

#ifndef _NETWORK_STATUS_STRUCT
#define _NETWORK_STATUS_STRUCT

//#define NETWORK_FLAG_LOGIN    0x01
//#define NETWORK_FLAG_LASTSALE 0x02
//#define NETWORK_FLAG_SALEDEL  0x04
//#define NETWORK_FLAG_LOADPLU  0x08
//#define NETWORK_FLAG_LOADDC   0x10
//#define NETWORK_FLAG_TRANSPAY 0x20

#pragma pack(4)
typedef struct {
	INT16U  slave_timeset;
	INT16U  timeset[NETWORK_TIMEOUT_MAX];
	INT16S     counter[NETWORK_TIMEOUT_MAX];
	INT16U  prevTime[NETWORK_TIMEOUT_MAX];	// CJK070316
	INT16U  service_flag;
	INT8U service_bflag2;

//	INT16U lock_command:3;
//	INT16U unlock_command:3;
//
//	unsigned char load_flag;	// b0: login, b1:lastsale,b2=clerk_saledelete
//								// b3: load_plu, b4=load_dc
//								// b5= send_trans_pay;
	INT8U allow_sale;
	INT8U allow_sale_trans;
	INT8U remote_plu;
	INT8U login_flag;
	INT8U load_lastsale;
	INT8U clerk_saledelete;
	INT8U  master_check;
	INT8U  time_check;
	INT8U clerk_trans_send;
	INT8U load_plu;
	INT8U load_dc;
	INT8U send_trans_pay;
	INT8U connection;
//	INT16U dummy:10;
	INT8U    check_lastscale;
	INT8U    counterticket;
    INT8U   send_trans_sale;
//DEL CJK070319	INT16U  master_oktime;
	INT16S     request_clerksaleid;
	INT16S     time_trans1;
	INT16S     time_trans2;
	INT16S     time_trans3;
#ifdef USE_EFT_POS_PROTOCOL
	INT16S     time_trans4;	// EFT-POS PROTOCOL(Iran) 전송 time
	INT16S     time_trans5; // REMOTE IP2 연결 체크 time
#endif
	INT16U	send_tail;
	INT16U	rcv_tail;
	INT16U	org_tail;
#ifdef USE_INDIA_FUNCTION
	INT16U	rcv_tail_pay;
#endif
	INT8U   send_trace_info;
	INT8U	reopen_status;
	INT8U	rcv_mt;
	INT8U	backup_plu;
	INT8U valid_indiv;
#ifdef USE_NHMART_SAFE_MEAT
	INT8U indivInfo;
	INT8U indivCallCts;
#endif
#ifdef USE_TOPMART_ASK_INDIV_INFO
	INT8U indiv_info_result; //sewon 170215
#endif
#ifdef USE_SCANNER_FOR_ECO_FRIENDLY
	INT8U ecoInfo;
#endif
#ifdef USE_INDIA_FUNCTION
	INT8U	send_inventory_check;
#endif
	INT8U remote_control;
} NETWORK_STATUS;
#pragma pack()
#endif

#ifndef ETH_CONNECT_LIST_DEFINE
#include "ethernet.h"
#endif

#if defined(USE_INDIA_FUNCTION) || defined(USE_SEND_TICKET_DATA)
extern INT8U reopenCheck;
#endif

#if defined(USE_REMOTE_FW_DOWNLOAD) || defined(USE_REMOTE_ROM_UPDATE)
extern INT8U RemoteFwUpdateFlag;

#define REMOTE_UPDATE_NONE  0
#define REMOTE_UPDATE_FW    1
#define REMOTE_UPDATE_ROM   2

#endif

#ifdef USE_REALTIME_CHECK_INTERVAL
  #ifdef USE_EMART_DEFAULT
#define REALTIME_CHECK_INTERVAL_DEFAULT	60
#define REALTIME_CHECK_INTERVAL_MAX		9999
#define REALTIME_CHECK_INTERVAL_MIN		10
  #else
#define REALTIME_CHECK_INTERVAL_DEFAULT	10
#define REALTIME_CHECK_INTERVAL_MAX		9999
#define REALTIME_CHECK_INTERVAL_MIN		1
  #endif
#endif
//#define _USE_DBG_CHECK_MASTER	// for debuging

extern NETWORK_STATUS network_status;
extern void  network_timeout_init(void);
extern void  network_timeout_reset(INT16S id);
extern INT8U network_counter_check(INT16U cnt,INT16S id);
extern void  network_counter_reset(INT16S id);
extern INT8U network_timeout_check(INT16U out,INT16S id);
extern void  transaction_sale_set_tail(INT16U tail);
extern void  transaction_pay_set_tail(INT8U tail);
extern void  network_tx_proc(void);
extern void  ethernet_recv(void);
extern void  network_direct(INT8U set);
// extern void  ethernet_command(INT8U ip0,INT8U ip1,INT8U ip2,INT8U ip3,INT16U x,INT16U cmd);
extern void  ethernet_tcp_command(INT16U base,INT16U cmd);
extern void network_param_init(void);
extern INT8U network_check_wireless_bridge(void);
extern void network_check(void);
extern void ethernet_debug(void);
extern INT16S  ethernet_gen_head(char *str);
extern INT16S  ethernet_gen_head_ip(char *str,unsigned char *ip);
extern INT8U network_login(INT16U clerkid, INT8U scaleid, INT8U mode);
extern INT8U network_ClerkDeleteTransaction(INT8U clerk);
extern INT8U network_masterscale_send(void);
extern void network_reopen_start_send(INT32U TicketNum, INT16U ClerkNum, INT8U flagCopyTicket);
//extern INT8U network_cursale_check(INT8U clerkid);
extern void transaction_pay_store(TRANSACTION_PAY *pTrans);
extern void transaction_pay_store_type8(TRANSACTION_PAY *pTrans);
#if defined(USE_INDIA_FUNCTION) || defined(USE_SEND_TICKET_DATA) || defined(USE_SETPRODUCT_SALE) || defined(USE_HYUNDAI_SET_FUNCTION) 
extern void TransactionSendWithoutVoid(INT16U lastClerk);
#endif
#ifdef USE_SEND_TICKET_DATA
extern void transactionSendTicketData(INT16U LastClerk);
#endif