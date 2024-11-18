#ifndef _NETWORK_COMMAND_MAX_VALUE
#define _NETWORK_COMMAND_MAX_VALUE
#define COMMAND_QUEUE_MAX	16
#define NETWORK_COMMAND_COMPLETE 2
#define NETWORK_COMMAND_BEGIN    0
#define NETWORK_COMMAND_RUN      1
#endif
#pragma pack(2)
#ifndef _NETWORK_STATUS_COMMAND_DATA
#define _NETWORK_STATUS_COMMAND_DATA
typedef struct {
	INT8U  param1;
	INT8U  param2;
	INT8U  sp1;
	INT32U sp2;
	INT8U  cmdtype;
	INT8U  status;
	INT8U  ip[4];
	INT32U id;
	INT32U recv_end;
	INT8U  timeout[32];
} COMMAND_DATA;
#endif


#ifndef _NETWORK_CLERK_SALE_AMOUNT_STRUCT
#define _NETWORK_CLERK_SALE_AMOUNT_STRUCT
typedef struct {
	INT16S totalCount;
	INT16S voidCount;
	long totalPrice;
	long voidPrice;
	long tareValue;
	INT16S customerNo;
} NETWORK_CLERK_SALE_AMOUNT;
#endif

#ifndef _NETWORK_SEND_PLU
#define _NETWORK_SEND_PLU
typedef struct {
	INT8U stype;
	INT32U pluno;
	INT8U  deptno;
} PLU_SEND_QUEUE;
#endif
#pragma pack()
#ifndef TRANSACTION_RECORD_TYPE_DEFINE
#include "mode_sale.h"
#endif
#ifndef ETH_CONNECT_LIST_DEFINE
#include "ethernet.h"
#endif

extern char PeelHeadMsgFlag;
extern void  ClearClerkIPList(void);
extern void ClearClerkTareList(void);
extern void SetClerkTare(INT8U clerk_id, long tare_value);
extern void ClearClerkTare(INT8U clerk_id);
extern long GetClerkTare(INT8U clerk_id);
extern void SetClerkCustomerNo(INT8U clerk_id, INT16S value);

extern void  command_login_loaddata(HUGEDATA COMM_BUF *CBuf,INT8U id,INT8U clerk,NETWORK_CLERK_SALE_AMOUNT *amount);
extern void command_connection_data(HUGEDATA COMM_BUF *CBuf,INT8U id);
extern void command_import_meat_send(HUGEDATA COMM_BUF *CBuf,INT8U id, INT8U check_bcc);
#ifdef USE_EXT_CHECKING_HARMFULNESS
extern INT8U command_import_meat_data(HUGEDATA COMM_BUF *CBuf, INT8U use_rs232);
#endif
extern void command_reopen_data(HUGEDATA COMM_BUF *CBuf, INT8U id, INT32U TicketNum, INT16U ClerkNum, INT8U Result);

//extern INT8U  transaction_sale_send(HUGEDATA COMM_BUF *CBuf, INT8U use_rs232);

//Modified by JJANG 20080121
extern void  transaction_sale_end( HUGEDATA COMM_BUF *CBuf,INT8U clerk,INT16U tail);
extern void transaction_sale_send_fail(HUGEDATA COMM_BUF *CBuf,INT8U clerk,INT16U tail);
extern void transaction_sale_send_memory_full(HUGEDATA COMM_BUF *CBuf, INT8U cmd, INT8U clerk, INT16U tail);
extern void transaction_sale_send_abort(HUGEDATA COMM_BUF *CBuf,INT8U clerk,INT16U tail);

extern void  transaction_sale_set_tail(INT16U tail);
extern void  transaction_pay_set_tail(INT8U tail);
extern void  m2s_command_data_ready(INT8U mode,INT8U oper,INT8U datatype,INT8U deptno,INT32U nth,ETH_CONNECT_LIST eth);
extern void send_clerk_sale_data(HUGEDATA COMM_BUF *CBuf,ETH_CONNECT_LIST eth,INT8U ClerkNum,INT16S id,TRANSACT_CHAIN *xtrans);
// extern void  send_data_request_ready(INT16U code,INT8U step);
extern void  recv_data_request(HUGEDATA COMM_BUF *CBuf,INT16U cmd,INT16S param2,INT8U *str,INT16S length);
extern void  net_set_clerksale(INT8U operator,INT8U *ip);
extern void  net_get_clerksale(INT8U operator,INT8U *ip);
extern void  network_common_trans(void);
extern void  network_error(INT8U mode);
extern void plu_modify_send(void);

extern INT16S NetCheckMasterConnection(void);
extern INT8U network_SetClerkData(INT8U clerk, long value, INT8U command);

extern void network_search_slave(void);
extern INT8U network_get_scale_list_id(INT8U *ip);
extern ETHERNET_SLAVE_SCALE NetSlaveInfo[MAX_SLAVE_SCALE];
#ifdef USE_ADMIN_EMAIL	
extern INT8U command_send_realtime_alram(void);
#endif
INT8U command_send_speedkey_value(char* key_type, int key_position);
