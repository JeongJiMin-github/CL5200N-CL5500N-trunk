/*****************************************************************************
|*			
|*  Copyright		:	(c) 2002 CAS
|*  Filename		:	ethernet.h
|*  Version			:	0.1
|*  Programmer(s)	:	Hong Yi-Phyo (HYP)
|*  Created			:	2003/08/25
|*  Description		:	    
|*				
****************************************************************************/

#ifndef _ETHERNET_H
#define _ETHERNET_H

#define MAX_REQ_WAIT_COUNT	 	10000
#define MAX_RES_WAIT_COUNT	 	10000
#define MAX_TX_RX_WAIT_COUNT	10000
//default settings
#define ETH_DEFAULT_CONNECT_TIME_OUT 		20	 // 5sec
#define ETH_DEFAULT_CLI_MAX_NUM_4_SRV		4//5
#define ETH_DEFAULT_WINDOW_SIZE				1500
#define ETH_DEFAULT_INTER_CH_DELAY			1000 //1000ms
#define ETH_DEFAULT_PS_FLAG					0
#define ETH_DEFAULT_OP_MODE					1//Infrastructure
#define ETH_DEFAULT_CHANNEL					1
#define ETH_DEFAULT_TX_RATE					15//auto
#define ETH_DEFAULT_AP_DENSITY				0x03
#define ETH_DEFAULT_DENY_UNENCRYPTED		0//disable
#define ETH_DEFAULT_SHARE_KEY_AUTH			0//disable

#define TCP_STATE_REQ_COMMAND	0x08
#define TCP_OPEN_COMMAND		0x20	
#define TCP_CLOSE_COMMAND		0x40	
#define UDP_SEND_DATA_COMMAND	0x00	

#define ETH_CMD_MAX_NUM			0x24
//HYP 20041213
#define ETH_REQ_VER					0x50
#define ETH_REQ_ID					0x51
#define ETH_REQ_MAC					0x52
#define ETH_REQ_DHCP				0x53
#define ETH_REQ_IP					0x54
#define ETH_REQ_SOURCE_TCP_PORT		0x55
#define ETH_REQ_TCP_CLIENT_PORT		0x56
#define ETH_REQ_SOURCE_UDP_PORT		0x57
#define ETH_REQ_DEST_IP				0x58
#define ETH_REQ_DEST_TCP_PORT		0x59
#define ETH_REQ_DEST_UDP_PORT		0x5A
#define ETH_REQ_GATEWAY				0x5B
#define ETH_REQ_SUBNET_MASK			0x5C
#define ETH_REQ_CONFIG_PORT			0x5D
#define ETH_REQ_CONN_TIMEOUT		0x5E
#define ETH_REQ_PARALLEL_SERIAL		0x5F
#define ETH_REQ_SERIAL_BAUD			0x60
#define ETH_REQ_INTER_CH_DELAY		0x61
#define ETH_REQ_PACKET_WINDOW		0x62
#define ETH_REQ_TCPIP_CONN_MAX		0x63
#define ETH_REQ_PASSWORD			0x64
//wlan
#define ETH_REQ_SSID				0x65
#define ETH_REQ_OP_MODE				0x66
#define ETH_REQ_CHANNEL				0x67
#define ETH_REQ_TX_RATE				0x68
#define ETH_REQ_AP_DENSITY			0x69
#define ETH_REQ_WEP_ENABLE			0x6A
#define ETH_REQ_WEP_ID				0x6B
#define ETH_REQ_WEP_LENGTH			0x6C
#define ETH_REQ_WEP_KEY1			0x6D
#define ETH_REQ_WEP_KEY2			0x6E
#define ETH_REQ_WEP_KEY3			0x6F
#define ETH_REQ_WEP_KEY4			0x70
#define ETH_REQ_DENY_UNENCRYPTED	0x71
#define ETH_REQ_SHARE_KEY_AUTH		0x72

//HYP 20041213
#define ETH_SET_VER					0x80
#define ETH_SET_ID					0x81
#define ETH_SET_MAC					0x82
#define ETH_SET_DHCP				0x83
#define ETH_SET_IP					0x84
#define ETH_SET_SOURCE_TCP_PORT		0x85
#define ETH_SET_TCP_CLIENT_PORT		0x86
#define ETH_SET_SOURCE_UDP_PORT		0x87
#define ETH_SET_DEST_IP				0x88
#define ETH_SET_DEST_TCP_PORT		0x89
#define ETH_SET_DEST_UDP_PORT		0x8A
#define ETH_SET_GATEWAY				0x8B
#define ETH_SET_SUBNET_MASK			0x8C
#define ETH_SET_CONFIG_PORT			0x8D
#define ETH_SET_CONN_TIMEOUT		0x8E
#define ETH_SET_PARALLEL_SERIAL		0x8F
#define ETH_SET_SERIAL_BAUD			0x90
#define ETH_SET_INTER_CH_DELAY		0x91
#define ETH_SET_PACKET_WINDOW		0x92
#define ETH_SET_TCPIP_CONN_MAX		0x93
#define ETH_SET_PASSWORD			0x94
//wlan
#define ETH_SET_SSID				0x95
#define ETH_SET_OP_MODE				0x96
#define ETH_SET_CHANNEL				0x97
#define ETH_SET_TX_RATE				0x98
#define ETH_SET_AP_DENSITY			0x99
#define ETH_SET_WEP_ENABLE			0x9A
#define ETH_SET_WEP_ID				0x9B
#define ETH_SET_WEP_LENGTH			0x9C
#define ETH_SET_WEP_KEY1			0x9D
#define ETH_SET_WEP_KEY2			0x9E
#define ETH_SET_WEP_KEY3			0x9F
#define ETH_SET_WEP_KEY4			0xA0
#define ETH_SET_DENY_UNENCRYPTED	0xA1
#define ETH_SET_SHARE_KEY_AUTH		0xA2
#define ETH_SET_SOFTRESET			0xA3
//HYP 20050105
#define ETH_SET_SSID_SCAN_CMD		0xB0
#define ETH_REQ_SSID_SCAN_QTY		0xB1
#define ETH_REQ_SSID_SCAN_DATA		0xB2

//HYP 20041213
#define ETH_CMD_TYPE_REQUSET	'R'
#define ETH_CMD_TYPE_PUSH		'P'
#define ETH_CMD_FLAG_DATA		'D'
#define ETH_CMD_FLAG_STATUS		'S'
#define ETH_CMD_FLAG_CONFIG		'C'
#define ETH_CMD_FLAG_COMMAND	'M'
#define ETH_CMD_PROTOCOL_TCP	'T'
#define ETH_CMD_PROTOCOL_UDP	'U'
#define ETH_CMD_OPEN			'O'
#define ETH_CMD_CLOSE			'C'
//HYP 20050105
#define ETH_CMD_SSID_SCAN		'S'
#define ETH_CMD_SSID_QTY		'Q'
#define ETH_CMD_SSID_DATA		'N'

//HYP 20041213
#define ETH_BYTES_HEADER			4

//TCP state define
#define CLOSE				0x00
#define LISTEN				0x01
#define SYNSENT				0x02
#define SYNRCVED			0x03
#define ESTABED				0x04
#define FINWAIT1			0x05
#define FINWAIT2			0x06
#define CLOSEWAIT			0x07
#define CLOSING				0x08
#define LASTACK				0x09
#define TIMEWAIT			0x0A
#define SENDARP				0x0E
#define UNKNOWN				0x0F
#define NOSOCKET			0xFF

#define MAX_TCP_START_WAIT_COUNT	10	//100ms		//CLOSE. LISTEN state wait
#define MAX_TCP_SC_CHANGE_WAIT_COUNT	10	//100ms		//Server,Client Change
#define MAX_TCP_MOPEN_WAIT_COUNT	100	//1000ms	//TCP Manual Open
#define MAX_TCP_PTX_WAIT_COUNT		100	//1000ms	//Parallel TX
#define MAX_TCP_DTX_WAIT_COUNT		100	//1000ms	//TCP TX
#define MAX_TCP_CLOSE_WAIT_COUNT	100	//1000ms	//TCP Close
//////////
#pragma pack(2)
typedef struct {
	BOOLEAN CmdFlag;
	INT8U ReqCmd;	
	INT8U ReqBytes;
	INT8U ResCmd;
	INT8U ResBytes;
	INT8U OpenState;
	//HYP 20041213
	INT8U ResCmd_Type;
	INT8U ResCmd_Flag;
	INT8U ResCmd_Ch;
	INT8U ReqCmd_Type;
	INT8U ReqCmd_Flag;
	INT8U ReqCmd_Ch;
} ETH_CMD_STRUCT;

typedef struct {
	INT8U UDPDestIP[4];			    
	INT16U UDPAppPort;
	HUGEDATA INT8U *data;
	INT16U dataCount;
} ETH_UDP_STRUCT;

typedef struct {
	INT8U	ScaleID;
//department
//scale group
	BOOLEAN NetStatus; //0=disconnect, 1=connect
	INT8U	TcpState;
} ETH_STRUCT;

typedef struct {
	INT8U  Ver[8];	
	INT8U  ID[16];
	INT8U  SSID[34];//SSID[32]=count,SSID[33]=0
	INT8U  MAC[6];			
	INT8U  DHCPEnable;//0:off 1:on
	INT8U  IP[4];	//my IP			    
	INT16U TCPSourcePort; //TCP Server port
	INT16U TCPClientPort; //TCP Client port
	INT16U UDPSourcePort; //UDP source port
	INT8U  TCPDestIP[4];	  //TCP Client Dest. IP
	INT16U TCPDestPort ;	  //TCP Client Dest. port
	INT16U UDPDestPort;	  //UDP Dest. port
	INT8U  Gateway[4];	  //my gateway
	INT8U  SubnetMask[4];  //my subnet mask
	INT16U UDPConfigPort; //UDP configuration port
	INT16U ConnTimeOut;	  //sec
	INT8U  ParallelSerialFlag;
	INT8U  SerialBaud;
	INT16U InterChDelay; //msec
	INT16U WindowSize;
	INT8U  TCPIPConnNum;	 //client max number for TCP server mode
	INT8U  Password[8];

	INT8U OPMode;		// 0: Ad-HOC, 1:Infrastructure
	INT8U Channel;		// 1~14(Used only Ad=HOC Mode)
	INT8U TxRate;		// 1:1MHz, 2:2MHz, 3:1or2MHz, 4:5.5MHz, 11:8MHz, 15:Auto
	INT8U APDensity;	// 0x03
	INT8U WEPEnable;	// 0:disable, 1:enable
	INT8U WEPID;		// 1~4
	INT8U WEPLength;	// 0:64bit , 1:128bit

	INT8U WEPKey1[13];
	INT8U WEPKey2[13];
	INT8U WEPKey3[13];
	INT8U WEPKey4[13];
	INT8U DenyUnencrypted; //0:disable, 1:enable
	INT8U ShareKeyAuth;	   //0:disable, 1:enable
} WLAN_CARD_STRUCT;

#define MAX_SSID_SCAN_NUM	4

typedef struct {
	INT8U  CHID[2];			//word BSS Channel ID from Probe Response
	INT8U  ANL[2];			//word Average Noise Level of the Probe Responses received during the scan, averaged on a per-channel basis.
	INT8U  SL[2];			//word Signal Level on which the Probe Response was received.
	INT8U  BSSID[6];		//3word MAC address of BSS responder from Probe Response.
	INT8U  BcnInt[2];		//word BSS Beacon Interval (in Kμs) from Probe Response.
	INT8U  Cap[2];			//word BSS Capability information, copied from the Probe Response.
	INT8U  SSID[34];		//17word the first word contains the length (the element type is 0) in the high-order octet, followed by 0 to 32 octets that contain the SSID string itself.
	INT8U  SupRates[10];	//5word Copy of the contents Supported Rates element from the Probe Response. This sub-field is terminated with a zero stamp word.
	INT8U  Rate[2];			//word Data rate of the received Probe Response.
	INT8U  ATIM[2]; 		//word The ATIM window time (in Kμs) used by the IBSS network.
} ETH_SSID_SCAN_DATA_STRUCT;
#pragma pack()
#endif

#ifndef ETH_CONNECT_LIST_DEFINE
#define ETH_CONNECT_LIST_DEFINE

#pragma pack(2)
typedef struct {
	INT8U			scaleid;
	INT8U		    departmentid;
	INT16U	enable	  :1;
	INT16U	recvtrans :1;
	INT16U    request_data:1;
	INT16U	dummy	  :13;
	INT16U          request_timeout;
	INT8U ip[4];
	INT16U port;
} ETH_CONNECT_LIST;
#pragma pack()

#endif

#ifndef ETHERNET_STATE_DEFINE
#define ETHERNET_STATE_DEFINE
#pragma pack(2)
typedef struct {
	ETH_CONNECT_LIST master;
    ETH_CONNECT_LIST elist[32];
	INT8U  last_transip[4];
	INT8U  localip[4];
	INT8U  remoteip[4];
	INT16S    nlist;
	INT8U  status;
	INT8U  mstatus;
	INT8U  timesync;
	INT16U timesync_out;
} ETHERNET_STATE;
#pragma pack()
#endif

#ifndef ETHERNET_SLAVE_SCALE_DEFINE
#define ETHERNET_SLAVE_SCALE_DEFINE
#pragma pack(2)
typedef struct {
	INT8U	code;
	INT8U	use;
	INT8U   ip[4];
	INT16U  tcp_port;
	INT16U	version;
} ETHERNET_SLAVE_SCALE;
#pragma pack()
#endif

#define NET_CHK_MASTER_RETRY	2	// 2times	
#define NET_CHK_MASTER_WAITTIME	10	// 100ms		

#define NET_CHK_PC_WAITTIME	20	// 200ms		//0
#define NET_REALTIME_WAITTIME	10	// 200ms
#define NET_CHK_PLU_SYNC_WAITTIME	0xffff	// no wait
#define NET_CHK_REMOTE2_WAITTIME	30	// 500ms		//0
#define NET_CHK_HARMFUL_SERVER	10	// 100ms

#define CLIENT_NORMAL		0
#define CLIENT_CHECK_MASTER	21	//slave
#define CLIENT_CHECK_PC		22	//master
#define CLIENT_PLU_SYNC		13	//master
#define CLIENT_REALTIME_DATA	14
#define CLIENT_SCALE_POS	31
#ifdef USE_EFT_POS_PROTOCOL
 #define CLIENT_SEND_EFT_POS  30
 #define CLIENT_CHECK_REMOTE2 40
#endif
#define CLIENT_CLERK_TRANS	90	//slave
#define CLIENT_BACKUP_TO_SCALE	91
#define CLIENT_TRACE_INFO	92
#define CLIENT_CHECK_TRACE_VALID 93
#define CLIENT_SEND_SPEEDKEY_VAL 94

#define CLIENT_CLOSE	0
#define CLIENT_OPEN		1
#define CLIENT_CHECK	2

#define UDP_MODE_CLIENT				0x01
#define UDP_MODE_ENABLE_DHCP		0x02
#define UDP_MODE_MANUAL_CONNETION	0x04

// Byte swapping.
#define SWAP8(A)		(A)
#define SWAP16(A)		((((A << 8 ) & 0xFF00)) | ((A >> 8)& 0x00FF))
#define SWAP32(A)		((((A<<24)&0xFF000000)) | (((A<<8)&0x00FF0000)) | (((A>>8)&0x0000FF00)) | (((A>>24)&0x000000FF)))


//UDP Socket Test용, 현재 buffer 한개만 추가해서 사용함 
#define UDP_SOCKET_BUFFER_NTH 	5  //udp buffer test

extern ETHERNET_STATE ethernet_list;
extern HUGEDATA ETH_CMD_STRUCT  EthCmdStruct;
//HYP 20041213
//extern HUGEDATA ETH_CARD_STRUCT EthCardStruct;
extern HUGEDATA WLAN_CARD_STRUCT EthCardStruct;
extern HUGEDATA ETH_UDP_STRUCT  EthUdpStruct;

extern INT8U EthSSIDScanQty;
extern HUGEDATA ETH_SSID_SCAN_DATA_STRUCT EthSSIDScanDataStruct[MAX_SSID_SCAN_NUM];

extern INT8U EthLocalCmdSend(INT8U cmd_type,INT8U debug);
extern INT8U EthRemoteCmdSend(INT8U cmd_type);
//extern INT8U EthUdpSend(void);
extern void EthVariableInit(void);
extern INT8U EthInit(void);
extern INT8U EthComparefromFlash(void);
extern void EthReConfig(void);

//HYP 20041213
extern void EthDataProc(HUGEDATA COMM_BUF *CBuf);
//HYp 20050106
extern INT8U EthScanSSID(INT8U debug);

//extern void  ethernet_udpcast(INT8U ip0,INT8U ip1,INT8U ip2,INT8U ip3,char *str,INT16S leng);
extern void  ethernet_set_flash2card(void);
// extern void  ethernet_close(void);
// extern void  ethernet_set_service(INT8U svc,char *ip,INT16U port);
extern void  ethernet_startup(void);
extern void  ethernet_fwset(INT32U fh, INT32U fl);
extern INT8U ethernet_tcpstate(void);
//extern void  ethernet_tcpsend(char *ip,INT16U port,INT8U *dp,INT16S leng);
extern void  ethernet_scanscale(void);
extern void  load_network_flag(void);
extern INT8U Ethernet_Server(INT8U open, INT16U tm, INT8U mode);
extern void EthTelnetToBridge(void);
// extern void ethernet_alarm(void);
