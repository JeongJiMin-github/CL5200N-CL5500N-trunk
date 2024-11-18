#include <stdio.h>
#include "globals.h"

#ifndef _COMMBUF_DEFINE_ALL
#include "commbuf.h"
#endif
#ifndef ETH_CONNECT_LIST_DEFINE
#include "ethernet.h"
#endif

#ifndef TRANSACTION_RECORD_TYPE_DEFINE
#include "mode_sale.h"
#endif

#ifndef COMMBUF_STRUCT_DEFINE
#include "commbuf.h"
#endif
#ifndef COM_DATABLOCK_SIZE_DEFINE
#define COM_DATABLOCK_SIZE_DEFINE
#define COM_DATABLOCK_SIZE 512
#endif

#ifdef USE_INDIA_FUNCTION
#define SEND_TRANSACTION 0
#define SEND_PAY         1
#endif

#define MAX_NUM_COM_TYPE 7

#define TABLE2_INGREDIENT		1
#define TABLE2_NUTRITION_FACT	2
#define TABLE2_TRACEABILITY		3
#define TABLE2_COUNTRY			4
#define TABLE2_SLAUGHTERHOUSE	5
#define TABLE2_CUTTINGHALL		6
#define TABLE2_INDIVIDUAL		7
#define TABLE2_SPECIALUSE		8
#define TABLE2_SLAUGHTERHOUSE_KR	9
#define TABLE2_MEATPART			10

#if defined (USE_BELARUS) || defined (USE_DIRECT_INGREDIENT)
#define MAX_SERIAL_BUFFER_LEN	3000
#else
#define MAX_SERIAL_BUFFER_LEN	1500
#endif
#define MAX_CLIENT_TX_BUF_LEN	1000	// Ethernet Client Tx Buffer 길이(server와 분리)

#ifdef USE_DIRECT_INGREDIENT
#define PLU_INGREDIENT_SIZE 300
#define CURRNET_MAX_INGREDIENT_SIZE 600
#endif

#define INGREDIENT_COM_BLOCK_SIZE 512

#define PLUSYNC_SEND_STATE_CHECK	  0
//#define PLUSYNC_SEND_STATE_WAIT_CONN	  1
#define PLUSYNC_SEND_STATE_CHECK_PLU	  2
#define PLUSYNC_SEND_STATE_OPEN_CONN_PLU  3
#define PLUSYNC_SEND_STATE_WAIT_CONN_PLU  4
#define PLUSYNC_SEND_STATE_SEND_PLU 	  5
#define PLUSYNC_SEND_STATE_WAIT_ACK_PLU	  6

#define PLUSYNC_SEND_STATE_CHECK_DC	  7
#define PLUSYNC_SEND_STATE_OPEN_CONN_DC	  8
#define PLUSYNC_SEND_STATE_WAIT_CONN_DC   9
#define PLUSYNC_SEND_STATE_SEND_DC 	  10
#define PLUSYNC_SEND_STATE_WAIT_ACK_DC	  11

#define PLUSYNC_SEND_STATE_CHECK_INGR	  12
#define PLUSYNC_SEND_STATE_OPEN_CONN_INGR	13
#define PLUSYNC_SEND_STATE_WAIT_CONN_INGR	14
#define PLUSYNC_SEND_STATE_SEND_INGR 	  15
#define PLUSYNC_SEND_STATE_WAIT_ACK_INGR	16

#define PLUSYNC_SEND_STATE_CHECK_SCROLL	  17
#define PLUSYNC_SEND_STATE_OPEN_CONN_SCROLL	18
#define PLUSYNC_SEND_STATE_WAIT_CONN_SCROLL	19
#define PLUSYNC_SEND_STATE_SEND_SCROLL 	  20
#define PLUSYNC_SEND_STATE_WAIT_ACK_SCROLL	21

#define PLUSYNC_SEND_STATE_NEXT	       	  30
#define PLUSYNC_SEND_STATE_NEXT_WITHOUT_CLOSE	31


#define CAL_KEY     0xca082012
#define CTSON_KEY   0xCB419324
#define CTSOFF_KEY  0xCC398743
#define CALCOMP_KEY 0xCD137423
#define PERIOD_KEY  0X34190000

#define K_TIME 0x00083419
#define K_ZERO 0x41908300
#define K_SPAN 0x19830419

#define ENCRYPT_NONE    0
#define ENCRYPT_CALENT  1
#define ENCRYPT_CTSON   2
#define ENCRYPT_CTSOFF  3
#define ENCRYPT_CALCOMP 4
#define ENCRYPT_PERIOD  5

#ifdef USE_EMART_DEFAULT
#define EMART_PLU_NAME4_MAX_SIZE	55
#endif

#define MAX_TABLE_BUF	114 	//Due to USE_BARCODE_LEN_90, 50->114 //Due to USE_BARCODE_LEN_60, 50->74(old)
#define MAX_TABLE_BUF_SLAUGHTER	128

extern HUGEDATA INT8U serial_send_point[MAX_SERIAL_BUFFER_LEN];
extern HUGEDATA INT8U serial_send_point_for_client[MAX_CLIENT_TX_BUF_LEN];
extern HUGEDATA INT16U recv_counter;

extern INT16U SyncDeptNo;
extern INT32U SyncPluNo;
extern INT16U SyncDelAddr;
extern INT8U SyncStatus[];
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
extern INT8U DispDisable;
#endif
#ifdef USE_CHN_IO_BOARD
extern INT8U UseI2CComm;
#endif

#ifdef USE_INDIA_FUNCTION
extern INT8U RealtimeSendMode;
#endif

extern INT16U get_new_labeladdress(INT16U id,INT8U chk);
extern INT16U get_new_bitmapaddress(INT16U id,INT8U chk);
extern INT32U commun_get_block_type_address(INT8U data,INT32U *max_size);

extern void  serial_init(void);
extern void  commun_outleng(HUGEDATA COMM_BUF *CBuf,char *buf,INT16S leng);
extern void  commun_out(HUGEDATA COMM_BUF *CBuf,char *buf);
extern void  commun_out_waiting(HUGEDATA COMM_BUF *CBuf, char *str);
extern void  serial_timeout_reset(INT8U com_type);
extern INT8U serial_timeout_check(INT8U com_type);
extern void  serial_proc(HUGEDATA COMM_BUF *CBuf);
extern INT16S  serial_get_comtype(HUGEDATA COMM_BUF *CBuf);
extern void  commun_recv_eth_header(ETH_CONNECT_LIST *eth,char *str,INT16S length);

extern INT8U inter_transaction_sale(INT8U code,TRANSACTION_RECORD_TYPE *trans,INT32U v32);
extern INT8U inter_transaction_pay(INT8U code,TRANSACTION_PAY *trans,INT32U v32);
extern void transaction_pay_send(HUGEDATA COMM_BUF *CBuf,INT8U direct);
extern INT8U transaction_sale_send(HUGEDATA COMM_BUF *CBuf);
extern INT8U send_realtime_data(INT8U use_rs232, INT16U *sendtail, INT16U *org_tail);
extern INT8U transaction_sale_restore(HUGEDATA COMM_BUF *CBuf);
extern INT8U transaction_pay_restore(HUGEDATA COMM_BUF *CBuf);
extern INT8U send_transaction_backup(void);
extern void trace_information_send(HUGEDATA COMM_BUF *CBuf, TRANSACTION_RECORD_TYPE *pTrans);

extern INT8U commun_common_ethernet(INT8U code,ETH_CONNECT_LIST *eth,INT32U v32);
extern void  commun_command(HUGEDATA COMM_BUF *CBuf,ETH_CONNECT_LIST eth,INT32U action,INT32U cmd,INT32U param);
extern void  commun_getinfo(HUGEDATA COMM_BUF *CBuf,INT8U cmd,INT32U number,INT32U fnumber);
extern void  commun_read(HUGEDATA COMM_BUF *CBuf,ETH_CONNECT_LIST eth,INT8U cmd,INT32U number,INT32U fnumber);
extern void  commun_write(HUGEDATA COMM_BUF *CBuf,INT8U cmd,INT32U number,INT32U nid,INT32U length,INT8U *str);
extern void  commun_setinfo(HUGEDATA COMM_BUF *CBuf,INT8U action,INT16U cmd,INT32U fnumber,INT32U length,INT8U *str);
//extern INT8U commun_send_plustrinfo(HUGEDATA COMM_BUF *CBuf,INT16S number);
//extern INT8U commun_recv_plustrinfo(INT16S number,INT8U *str);
//extern INT8U commun_recv_plu_part(HUGEDATA COMM_BUF *CBuf,INT32U ipluno,INT16U ideptno,INT8U *str,INT16S leng);
extern INT8U commun_send_price(HUGEDATA COMM_BUF *CBuf,INT8U deptno,INT32U pluno);
//extern INT8U commun_recv_key(INT16S key,INT16S key_id,INT8U *str,INT16S leng);
extern void  commun_write_err(HUGEDATA COMM_BUF *CBuf,INT8U cmd,INT32U number,INT8U ret);
extern void  commun_write_err16(HUGEDATA COMM_BUF *CBuf,INT8U cmd,INT16U ret16,INT8U chk,INT8U noheader);
extern void  commun_write_err_info(HUGEDATA COMM_BUF *CBuf,INT8U cmd,INT32U number,INT8U ret);
extern INT8U commun_send_plu_from_address(HUGEDATA COMM_BUF *CBuf,INT16U address,INT32U pluno,INT8U xmode);
extern void  commun_send_plu_delete(HUGEDATA COMM_BUF *CBuf, INT16U deptNo, INT32U pluNo);
extern void commun_send_dc_delete(HUGEDATA COMM_BUF *CBuf, INT16U deptNo, INT32U pluNo);
extern void commun_send_table2_delete(HUGEDATA COMM_BUF *CBuf, INT8U part, INT16U id);
extern void commun_send_table3_delete(HUGEDATA COMM_BUF *CBuf, INT8U part, INT16U id);
//extern INT8U commun_send_plu_nth(HUGEDATA COMM_BUF *CBuf,INT32U nth,INT32U pluid);
//extern INT8U commun_send_key(HUGEDATA COMM_BUF *CBuf,INT16S key_mode,INT16S keyno);
//extern INT8U commun_send_plu(HUGEDATA COMM_BUF *CBuf,ETH_CONNECT_LIST eth,INT16U cmd,INT32U nth,INT32U pcode);
extern void  commun_send_saleinfo(HUGEDATA COMM_BUF *CBuf);
//extern INT8U commun_send_caption(HUGEDATA COMM_BUF *CBuf,INT16U mode,INT16U id);
//extern INT8U commun_recv_caption(HUGEDATA COMM_BUF *CBuf,INT16S mode,INT16S id,INT8U *str);
extern INT8U commun_getrtc(INT16S cmd,INT8U *str);
extern INT8U commun_setrtc(INT8U *str,INT16S leng);
extern INT8U commun_set_business_time(INT8U *str,INT16S leng);
//extern INT8U commun_send_labelbmp(HUGEDATA COMM_BUF *CBuf,INT8U mode,INT16S number,INT16S fnumber);
extern INT8U commun_recv_bmp(INT8U *str,INT16S leng);
//extern INT8U commun_send_discount(HUGEDATA COMM_BUF *CBuf,INT16U cmd,INT16U dc_no,INT8U mode,INT8U rept);
extern INT8U recv_check_scaleid(INT8U scaleid);
extern INT8U * get_temp_area(INT8U m);

extern INT8U  table1_delete_any(INT8U part,INT8U mode,INT16U id);
extern INT8U  table2_delete_any(INT8U part,INT8U mode,INT16U id);
extern INT8U  table3_delete_any(INT8U part,INT8U mode,INT16U id);

extern INT16U commun_net_mask(INT8U data_type, INT16U *dc_no, INT8U scaleListId, INT16S mode);
extern void commun_send_plu_modify_scale(void);

extern void change_dest_ip_val(INT32U);
extern void change_dest_ip(INT8U *ip, INT16U tcp_port);
extern void restore_dest_ip(void);
extern void put_dest_ip(INT8U *ip, INT16U tcp_port);

extern INT8U ClerkTransactionSaleStore(INT8U clerkNum);

extern void commun_write(HUGEDATA COMM_BUF *CBuf,INT8U cmd,INT32U number,INT32U nn,INT32U length,INT8U *str);
extern void commun_read(HUGEDATA COMM_BUF *CBuf,ETH_CONNECT_LIST eth,INT8U cmd,INT32U number,INT32U fnumber);
#ifdef USE_INDIA_FUNCTION
extern INT8U CheckInventory(INT16U deptno, INT32U pluno, INT8U orgType, INT8U newType, INT32U weight, INT32U qty);
#endif

extern void commun_send_plu_data(HUGEDATA COMM_BUF *CBuf,INT32U nth); //SG080107
extern INT8U commun_send_plu_gtin_data(HUGEDATA COMM_BUF *CBuf,INT32U nth); //SG080107
extern INT16U crc_16(INT8U *aData, INT32U aSize);
