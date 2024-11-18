/*****************************************************************************
|*			
|*  Copyright		:	(c) 2002 CAS
|*  Filename		:	ethernet.c
|*  Version			:	0.1
|*  Programmer(s)	:	Hong Yi-Phyo (HYP)
|*  Created			:	2003/08/25
|*  Description		:	    
|*				
****************************************************************************/

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
#include "mode_main.h"
#include "ethernet_comm.h"
#include "network_set.h"
#include "app.h"


HUGEDATA ETH_CMD_STRUCT EthCmdStruct;
HUGEDATA WLAN_CARD_STRUCT EthCardStruct;
HUGEDATA ETH_UDP_STRUCT EthUdpStruct;
HUGEDATA ETH_STRUCT EthStruct;
//status check variable
INT8U EthCheckStatusID;
INT8U EthCheckStatusIP[4];
//HYP 20050105 SSID Scan variable
INT8U EthSSIDScanQty;
INT8U EthSSIDScanNum;
HUGEDATA ETH_SSID_SCAN_DATA_STRUCT EthSSIDScanDataStruct[MAX_SSID_SCAN_NUM];

extern   INT8U  network_lock_command,network_unlock_command;
ETHERNET_STATE ethernet_list;
extern void commun_out(HUGEDATA COMM_BUF *CBuf,char *str);
extern INT8U Ethernet_Command(INT8U s, INT8U cmd_type);

void EthDataProc(HUGEDATA COMM_BUF *CBuf) {
	if (Startup.global_com&0x01) {
		//w5300_Proc(CBuf);
		TcpIp_Task(); 
		
	}
}

//extern char dhcp_state;
INT8U EthLocalCmdSend(INT8U cmd_type,INT8U debug)
{
	INT8U str[6];

	TcpIp_Task();
	
	if(cmd_type >= ETH_REQ_VER && cmd_type <= ETH_REQ_SHARE_KEY_AUTH) {
		switch(cmd_type) {
			case ETH_REQ_VER:
				//check w5300
				//if(getMR() == 0xB900 || (getMR() == 0xB800)) { 
					sprintf((char *)&(EthCardStruct.Ver[0]),"LAN5.0"); 
				//} else return 0xff;
				//sprintf((char *)&(EthCardStruct.Ver[0]),"LAN5.0"); 
				break;
			case ETH_REQ_ID: 
				EthCardStruct.ID[0] = 0;
				break;
			case ETH_REQ_MAC:
				break;
			case ETH_REQ_DHCP: 
#ifdef USE_DNS_RESOLVER
				EthCardStruct.DHCPEnable = (get_net_bparam(NETWORK_LOCAL_ETHERNET_FLAG) & 0x01);
#else
				EthCardStruct.DHCPEnable = get_net_bparam(NETWORK_LOCAL_ETHERNET_FLAG); 
#endif
				break;
			case ETH_REQ_IP:
			        if (EthCardStruct.DHCPEnable == FALSE) {
					get_net_sparam(NETWORK_LOCAL_IP,str,4);
				      	EthCardStruct.IP[3]=str[0];
				      	EthCardStruct.IP[2]=str[1];
				      	EthCardStruct.IP[1]=str[2];
				      	EthCardStruct.IP[0]=str[3];
				} else {
					/*
					if (dhcp_state != STATE_DHCP_LEASED) {
					      	EthCardStruct.IP[3]=0;
					      	EthCardStruct.IP[2]=0;
					      	EthCardStruct.IP[1]=0;
					      	EthCardStruct.IP[0]=0;
					}
					*/
				}
				break;
			case ETH_REQ_SOURCE_TCP_PORT:
				EthCardStruct.TCPSourcePort = get_net_wparam(NETWORK_LOCAL_TCP_PORT);
				break;
			case ETH_REQ_TCP_CLIENT_PORT:
				EthCardStruct.TCPClientPort = get_net_wparam(NETWORK_LOCAL_TCP_PORT)+1;
				break;
			case ETH_REQ_SOURCE_UDP_PORT:
				EthCardStruct.UDPSourcePort = get_net_wparam(NETWORK_LOCAL_UDP_PORT);
				break;
			case ETH_REQ_DEST_IP:
				get_net_sparam(NETWORK_MACHINE_REMOTE_IP, str, 4);
				EthCardStruct.TCPDestIP[3]=str[0];
				EthCardStruct.TCPDestIP[2]=str[1];
				EthCardStruct.TCPDestIP[1]=str[2];
				EthCardStruct.TCPDestIP[0]=str[3];
				break;
			case ETH_REQ_DEST_TCP_PORT:
				EthCardStruct.TCPDestPort = get_net_wparam(NETWORK_MACHINE_REMOTE_TCPPORT);
				break;
			case ETH_REQ_DEST_UDP_PORT:
				EthCardStruct.UDPDestPort = get_net_wparam(NETWORK_MACHINE_REMOTE_UDPPORT);
				break;
			case ETH_REQ_GATEWAY:
			        if (EthCardStruct.DHCPEnable == FALSE) {
					get_net_sparam(NETWORK_LOCAL_GATEWAY,str,4);
					EthCardStruct.Gateway[3]=str[0];
					EthCardStruct.Gateway[2]=str[1];
					EthCardStruct.Gateway[1]=str[2];
					EthCardStruct.Gateway[0]=str[3];
				}
				break;
			case ETH_REQ_SUBNET_MASK:
			        if (EthCardStruct.DHCPEnable == FALSE) {
					get_net_sparam(NETWORK_LOCAL_SUBNETMASK,str,4);
					EthCardStruct.SubnetMask[3]=str[0];
					EthCardStruct.SubnetMask[2]=str[1];
					EthCardStruct.SubnetMask[1]=str[2];
					EthCardStruct.SubnetMask[0]=str[3];
				}
				break;
			case ETH_REQ_CONFIG_PORT:
				EthCardStruct.UDPConfigPort = 65535;
				break;
			case ETH_REQ_CONN_TIMEOUT:
				EthCardStruct.ConnTimeOut = ETH_DEFAULT_CONNECT_TIME_OUT;
				break;
			case ETH_REQ_PARALLEL_SERIAL:
				EthCardStruct.ParallelSerialFlag = ETH_DEFAULT_PS_FLAG;
				break;
			case ETH_REQ_SERIAL_BAUD:
				EthCardStruct.SerialBaud = 0;
				break;
			case ETH_REQ_INTER_CH_DELAY:
				EthCardStruct.InterChDelay = ETH_DEFAULT_INTER_CH_DELAY;
				break;
			case ETH_REQ_PACKET_WINDOW:
				EthCardStruct.WindowSize   = ETH_DEFAULT_WINDOW_SIZE;
				break;
			case ETH_REQ_TCPIP_CONN_MAX:
				EthCardStruct.TCPIPConnNum = ETH_DEFAULT_CLI_MAX_NUM_4_SRV;
				break;
			case ETH_REQ_PASSWORD:
				EthCardStruct.Password[0] = 0;
				break;
			//wlan
			case ETH_REQ_SSID:
				get_net_sparam(NETWORK_WLAN_SSID,EthCardStruct.SSID,32);
				break;
			case ETH_REQ_OP_MODE:
				EthCardStruct.OPMode  = ETH_DEFAULT_OP_MODE;
				break;
			case ETH_REQ_CHANNEL:
				EthCardStruct.Channel = ETH_DEFAULT_CHANNEL;
				break;
			case ETH_REQ_TX_RATE:
				EthCardStruct.TxRate  = ETH_DEFAULT_TX_RATE;
				break;
			case ETH_REQ_AP_DENSITY:
				EthCardStruct.APDensity = ETH_DEFAULT_AP_DENSITY;	
				break;
			case ETH_REQ_WEP_ID:
			case ETH_REQ_WEP_ENABLE:
			case ETH_REQ_WEP_LENGTH:
			case ETH_REQ_WEP_KEY1:
			case ETH_REQ_WEP_KEY2:
			case ETH_REQ_WEP_KEY3:
			case ETH_REQ_WEP_KEY4:
				str[0] = get_net_bparam(NETWORK_WLAN_USEWEP);
				EthCardStruct.WEPEnable=0;
				switch (str[0]) {
					case 1: EthCardStruct.WEPEnable=1;
						EthCardStruct.WEPLength=0; 
						break;
					case 2: EthCardStruct.WEPLength=1;
						EthCardStruct.WEPEnable=1;  
						break;
				}
				get_net_sparam(NETWORK_WLAN_SSID,EthCardStruct.SSID,32);
				EthCardStruct.WEPID = get_net_bparam(NETWORK_WLAN_WEPID);
				switch (EthCardStruct.WEPID) {
					case 1: get_net_sparam(NETWORK_WLAN_WEP,EthCardStruct.WEPKey1,13);break;
					case 2: get_net_sparam(NETWORK_WLAN_WEP,EthCardStruct.WEPKey2,13);break;
					case 3: get_net_sparam(NETWORK_WLAN_WEP,EthCardStruct.WEPKey3,13);break;
					case 4: get_net_sparam(NETWORK_WLAN_WEP,EthCardStruct.WEPKey4,13);break;
				}
				break;
			case ETH_REQ_DENY_UNENCRYPTED:
				EthCardStruct.DenyUnencrypted = ETH_DEFAULT_DENY_UNENCRYPTED;
				break;
			case ETH_REQ_SHARE_KEY_AUTH:
				EthCardStruct.ShareKeyAuth    = ETH_DEFAULT_SHARE_KEY_AUTH;
				break;
		}
	} else if(cmd_type >= ETH_SET_VER && cmd_type <= ETH_SET_SOFTRESET) {
	} 
	else 
	{
		switch(cmd_type) {
			case TCP_STATE_REQ_COMMAND:
				EthStruct.TcpState =Ethernet_Command(EthCheckStatusID,TCP_STATE_REQ_COMMAND);
				break;
			case ETH_SET_SSID_SCAN_CMD:
			case ETH_REQ_SSID_SCAN_QTY:
				EthSSIDScanQty = 0;
				break;
			case TCP_OPEN_COMMAND:
				//only client
				Ethernet_Command(0,TCP_OPEN_COMMAND);
				break;
			case TCP_CLOSE_COMMAND:
				//only client
				Ethernet_Command(0,TCP_CLOSE_COMMAND);
				break;
			default:
				return 0xff;
		}
	}
//	EthCmdStruct.CmdFlag = OFF;
	
///////////////////receive module end/////////////////////////////////////

	TcpIp_Task(); 

	return 0;
}


void EthDspMyInform(HUGEDATA COMM_BUF *CBuf,INT8U mode) {
	char string_buf[128];
	INT16S  sl,r;
//	INT16S i,sl,r;

	if (mode) goto WLAN;
	sprintf(string_buf,"VERSION=["); r=9; sl=8;
	memcpy(&string_buf[r],EthCardStruct.Ver,sl); string_buf[r+sl]=0; r=strlen(string_buf);
	sprintf(&string_buf[r],"]\r\n");
//	for(i=0; i<8; i++) {
//		string_buf[i+9]=EthCardStruct.Ver[i];
//	}
//	sprintf(&string_buf[i+9],"]\r\n");
	commun_out(CBuf,string_buf);

	sprintf(string_buf,"CARD ID=["); r=9; sl=16;
	memcpy(&string_buf[r],EthCardStruct.ID,sl); string_buf[r+sl]=0; r=strlen(string_buf);
	sprintf(&string_buf[r],"]\r\n");
	commun_out(CBuf,string_buf);

	sprintf(string_buf,"MAC=%2x:%2x:%2x-%2x:%2x:%2x\r\n",EthCardStruct.MAC[5],EthCardStruct.MAC[4],EthCardStruct.MAC[3],EthCardStruct.MAC[2],EthCardStruct.MAC[1],EthCardStruct.MAC[0]);
	commun_out(CBuf,string_buf);
	sprintf(string_buf,"DHCPEnable=%d (0:off, 1:on)\r\n",EthCardStruct.DHCPEnable);	
	commun_out(CBuf,string_buf);
	sprintf(string_buf,"IP=%d.%d.%d.%d\r\n",EthCardStruct.IP[3],EthCardStruct.IP[2],EthCardStruct.IP[1],EthCardStruct.IP[0]);	
	commun_out(CBuf,string_buf);
	sprintf(string_buf,"Gateway=%d.%d.%d.%d\r\n",EthCardStruct.Gateway[3],EthCardStruct.Gateway[2],EthCardStruct.Gateway[1],EthCardStruct.Gateway[0]);
	commun_out(CBuf,string_buf);
	sprintf(string_buf,"Subnet=%d.%d.%d.%d\r\n",EthCardStruct.SubnetMask[3],EthCardStruct.SubnetMask[2],EthCardStruct.SubnetMask[1],EthCardStruct.SubnetMask[0]);
	commun_out(CBuf,string_buf);
	sprintf(string_buf,"TCP Source Port=%d (for Server)\r\n",EthCardStruct.TCPSourcePort);
	commun_out(CBuf,string_buf);
	sprintf(string_buf,"TCP Dest. IP=%d.%d.%d.%d (for Client)\r\n",EthCardStruct.TCPDestIP[3],EthCardStruct.TCPDestIP[2],EthCardStruct.TCPDestIP[1],EthCardStruct.TCPDestIP[0]);
	commun_out(CBuf,string_buf);
	sprintf(string_buf,"TCP Source Port=%d (for Client)\r\n",EthCardStruct.TCPClientPort);
	commun_out(CBuf,string_buf);
	sprintf(string_buf,"TCP Dest. Port=%d (for Client)\r\n",EthCardStruct.TCPDestPort);
	commun_out(CBuf,string_buf);
	sprintf(string_buf,"UDP Source Port=%d\r\n",EthCardStruct.UDPSourcePort);
	commun_out(CBuf,string_buf);
	sprintf(string_buf,"UDP Dest. Port=%d\r\n",EthCardStruct.UDPDestPort);
	commun_out(CBuf,string_buf);
	sprintf(string_buf,"UDP Config Port=%d (for Card Config)\r\n",EthCardStruct.UDPConfigPort);
	commun_out(CBuf,string_buf);
	sprintf(string_buf,"Conn. TimeOut=%d s (for TCP Server)\r\n",EthCardStruct.ConnTimeOut);	
	commun_out(CBuf,string_buf);
	sprintf(string_buf,"Inter Ch Delay=%d ms (for Comm.)\r\n",EthCardStruct.InterChDelay);	
	commun_out(CBuf,string_buf);
	sprintf(string_buf,"Window Size=%d\r\n",EthCardStruct.WindowSize);	
	commun_out(CBuf,string_buf);
	sprintf(string_buf,"TCP Max Conn Num=%d EA (for Server)\r\n",EthCardStruct.TCPIPConnNum);	
	commun_out(CBuf,string_buf);
	return;
WLAN:
	if (!(Startup.global_com&0x02))	// not use wireless bridge 
	{
		sprintf(string_buf,"Cannot Use Wireless Lan\r\n");
		commun_out(CBuf,string_buf);
	}

	if (EthCardStruct.Ver[0]!='W') {
		sprintf(string_buf,"Cannot Found Wireless Lan\r\n");
		commun_out(CBuf,string_buf);
		return;
	}

	sprintf(string_buf,"WLAN SSID=["); r=11; sl=32;
	memcpy(&string_buf[r],EthCardStruct.SSID,sl); string_buf[r+sl]=0; r=strlen(string_buf);
	sprintf(&string_buf[r],"]\r\n");
	commun_out(CBuf,string_buf);

	sprintf(string_buf,"WLAN OPMode=%d (0:Ad-HOC, 1:Infrastructure)\r\n",EthCardStruct.OPMode);	
	commun_out(CBuf,string_buf);
	sprintf(string_buf,"WLAN Channel=%d (1~14 Used only Ad-HOC Mode)\r\n",EthCardStruct.Channel);	
	commun_out(CBuf,string_buf);
	sprintf(string_buf,"WLAN TxRate=%d (1:1MHz,2:2MHz,3:1or2MHz,4:5.5MHz,11:8MHz,15:Auto)\r\n",EthCardStruct.TxRate);	
	commun_out(CBuf,string_buf);
	sprintf(string_buf,"WLAN APDensity=%d \r\n",EthCardStruct.APDensity);	
	commun_out(CBuf,string_buf);
	sprintf(string_buf,"WLAN WEPEnable=%d (0:disable, 1:enable)\r\n",EthCardStruct.WEPEnable);	
	commun_out(CBuf,string_buf);
	sprintf(string_buf,"WLAN WEPID=%d (1~4)\r\n",EthCardStruct.WEPID);	
	commun_out(CBuf,string_buf);
	sprintf(string_buf,"WLAN WEPLength=%d (0:64bit, 1:128bit)\r\n",EthCardStruct.WEPLength);	
	commun_out(CBuf,string_buf);

	sprintf(string_buf,"WLAN WEPKey1=[");	r=strlen(string_buf); sl=14;
	memcpy(&string_buf[r],EthCardStruct.WEPKey1,sl); string_buf[r+sl]=0; r=strlen(string_buf);
	sprintf(&string_buf[r],"]\r\n");
	commun_out(CBuf,string_buf);

	sprintf(string_buf,"WLAN WEPKey2=[");	r=strlen(string_buf); sl=14;
	memcpy(&string_buf[r],EthCardStruct.WEPKey2,sl); string_buf[r+sl]=0; r=strlen(string_buf);
	sprintf(&string_buf[r],"]\r\n");
	commun_out(CBuf,string_buf);

	sprintf(string_buf,"WLAN WEPKey3=[");	r=strlen(string_buf); sl=14;
	memcpy(&string_buf[r],EthCardStruct.WEPKey3,sl); string_buf[r+sl]=0; r=strlen(string_buf);
	sprintf(&string_buf[r],"]\r\n");
	commun_out(CBuf,string_buf);

	sprintf(string_buf,"WLAN WEPKey4=[");	r=strlen(string_buf); sl=14;
	memcpy(&string_buf[r],EthCardStruct.WEPKey4,sl); string_buf[r+sl]=0; r=strlen(string_buf);
	sprintf(&string_buf[r],"]\r\n");
	commun_out(CBuf,string_buf);

	sprintf(string_buf,"WLAN DenyUnencrypted=%d (0:disable, 1:enable)\r\n",EthCardStruct.DenyUnencrypted);	
	commun_out(CBuf,string_buf);
	sprintf(string_buf,"WLAN ShareKeyAuth=%d (0:disable, 1:enable)\r\n",EthCardStruct.ShareKeyAuth);	
	commun_out(CBuf,string_buf);

// --------------- Setting
	sprintf(string_buf,"FL Status=%d,%d\r\n",ethernet_list.status,ethernet_list.mstatus);
	commun_out(CBuf,string_buf);
	sprintf(string_buf,"FL local  =%d.%d.%d.%d\r\n",ethernet_list.localip[0],ethernet_list.localip[1],ethernet_list.localip[2],ethernet_list.localip[3]);
	commun_out(CBuf,string_buf);
	sprintf(string_buf,"FL remote =%d.%d.%d.%d\r\n",ethernet_list.remoteip[0],ethernet_list.remoteip[1],ethernet_list.remoteip[2],ethernet_list.remoteip[3]);
	commun_out(CBuf,string_buf);
}

void  EthGetMode(void)
{
	INT8U global_com;
	ethernet_list.status = get_net_bparam(NETWORK_SERVICE_TYPE);
	//if ((ethernet_list.status>3) || (ethernet_list.status<1)) ethernet_list.status=3;
	if (ethernet_list.status>NETWORK_SERVICE_MAX) ethernet_list.status=NETWORK_SERVICE_NETWORK;
	global_com =get_global_bparam(GLOBAL_AREA_COMMUNICATION);
 	if (global_com&0x01) {
	} else {
		ethernet_list.status=0;
	}
	ethernet_list.mstatus = ethernet_list.status;
}

INT8U EthReqAllData(INT8U debug) {
	INT8U i;

	if(EthLocalCmdSend(ETH_REQ_VER,debug) == 0xff) return 0xff;

	for(i=ETH_REQ_ID; i<=ETH_REQ_SHARE_KEY_AUTH ; i++) {
		if(EthLocalCmdSend(i,debug) == 0xff) return 0xff;
	} 

	if(EthCardStruct.Ver[0] == 'W') {   //wlan
		for(i=ETH_REQ_ID; i<=ETH_REQ_SHARE_KEY_AUTH ; i++) {
			if(EthLocalCmdSend(i,debug) == 0xff) return 0xff;
		}
	} else {
		for(i=ETH_REQ_ID; i<=ETH_REQ_PASSWORD ; i++) {
			if(EthLocalCmdSend(i,debug) == 0xff) return 0xff;
		}
	}

	return 0;
}

INT8U EthSetData(INT8U debug) {
	INT8U i;

	if(EthLocalCmdSend(ETH_REQ_VER,debug) == 0xff) return 0xff;

	if(EthCardStruct.Ver[0] == 'W') {   //wlan
		for(i=ETH_SET_ID; i<=ETH_SET_SOFTRESET ; i++) {
			if(i != ETH_SET_MAC) {
				if(EthLocalCmdSend(i,debug) == 0xff) return 0xff;
			}
		}
	} else {
		for(i=ETH_SET_ID; i<=ETH_SET_PASSWORD ; i++) {
			if(i != ETH_SET_MAC) {
				if(EthLocalCmdSend(i,debug) == 0xff) return 0xff;
			}
		}
		if(EthLocalCmdSend(ETH_SET_SOFTRESET,debug) == 0xff) return 0xff;
	}
	return 0;
}

INT8U EthSetAllData(INT8U debug) {//include MAC
	INT8U i;
//	char string_buf[20];

	if(EthLocalCmdSend(ETH_REQ_VER,debug) == 0xff) return 0xff;

	if(EthCardStruct.Ver[0] == 'W') {   //wlan
		for(i=ETH_SET_ID; i<=ETH_SET_SOFTRESET ; i++) {
			if(EthLocalCmdSend(i,debug) == 0xff) return 0xff;
		}
	} else {
		for(i=ETH_SET_ID; i<=ETH_SET_PASSWORD ; i++) {
			if(EthLocalCmdSend(i,debug) == 0xff) return 0xff;
		}
		if(EthLocalCmdSend(ETH_SET_SOFTRESET,debug) == 0xff) return 0xff;
	}
	return 0;
}

void EthVariableInit(void)
{
	memset(&ethernet_list, 0, sizeof(ETHERNET_STATE));
}

INT8U EthInit(void)
{
	INT8U ret,cnt;

	cnt=0;
	ret=0;
	ethernet_list.nlist   =0;
	ethernet_list.timesync=0;
	if (!(network_status.service_flag&0x0001)) ethernet_list.timesync=1;
	ethernet_list.timesync_out = 0;
	EthGetMode();
	while (1) {
		if (EthReqAllData(0) != 0xff) { ret=1; break; } 
		cnt++;
		if (cnt>5) break;
	}
	if (ret)
	{
		ethernet_list.localip[0]=EthCardStruct.IP[3];
		ethernet_list.localip[1]=EthCardStruct.IP[2];
		ethernet_list.localip[2]=EthCardStruct.IP[1];
		ethernet_list.localip[3]=EthCardStruct.IP[0];
		ethernet_list.remoteip[0]=EthCardStruct.TCPDestIP[3];
		ethernet_list.remoteip[1]=EthCardStruct.TCPDestIP[2];
		ethernet_list.remoteip[2]=EthCardStruct.TCPDestIP[1];
		ethernet_list.remoteip[3]=EthCardStruct.TCPDestIP[0];
	}
	
	if (ret==0) return FALSE;
	return TRUE;
}



INT8U EthComparefromFlash(void)
{
	INT8U str[34];//,temp[20];
	INT32U pnt;

	pnt =DFLASH_BASE;
	pnt+=FLASH_NET_AREA;

#ifdef USE_DNS_RESOLVER
	str[0]=get_net_bparam(NETWORK_LOCAL_ETHERNET_FLAG) & 0x01;
#else
	str[0]=get_net_bparam(NETWORK_LOCAL_ETHERNET_FLAG);
#endif
	str[0]=get_net_bparam(NETWORK_LOCAL_ETHERNET_FLAG);
	if (EthCardStruct.TCPIPConnNum!=ETH_DEFAULT_CLI_MAX_NUM_4_SRV) goto RESET;

	if (str[0]!=EthCardStruct.DHCPEnable) return FALSE;
	if (EthCardStruct.DHCPEnable) {
		goto ONLY_DHCP;
	} 
	Flash_sread(pnt+NETWORK_LOCAL_IP,str,4);
	if (!((str[0]==EthCardStruct.IP[3]) && (str[1]==EthCardStruct.IP[2]) && (str[2]==EthCardStruct.IP[1]) && (str[3]==EthCardStruct.IP[0]))) goto RESET;
	Flash_sread(pnt+NETWORK_LOCAL_GATEWAY,str,4);
	if (!((str[0]==EthCardStruct.Gateway[3]) && (str[1]==EthCardStruct.Gateway[2]) && (str[2]==EthCardStruct.Gateway[1]) && (str[3]==EthCardStruct.Gateway[0]))) goto RESET;
	Flash_sread(pnt+NETWORK_LOCAL_SUBNETMASK,str,4);
	if (!((str[0]==EthCardStruct.SubnetMask[3]) && (str[1]==EthCardStruct.SubnetMask[2]) && (str[2]==EthCardStruct.SubnetMask[1]) && (str[3]==EthCardStruct.SubnetMask[0]))) goto RESET;
	if (EthCardStruct.Ver[0]!='W') return TRUE;

	Flash_sread(pnt+NETWORK_WLAN_SSID,str,32);
	str[32]=0;
	if (strcmp((char *)EthCardStruct.SSID,(char *)str)!=0) goto RESET;
	str[0]=Flash_bread(pnt+NETWORK_WLAN_USEWEP);
	switch (str[0]) {
	   case 0: if (EthCardStruct.WEPEnable) goto RESET;
	           break;
	   case 1: if (EthCardStruct.WEPEnable==0) goto RESET;
		   if (EthCardStruct.WEPLength   ) goto RESET;
	           break;
	   case 2: if (EthCardStruct.WEPEnable==0) goto RESET;
		   if (!EthCardStruct.WEPLength  ) goto RESET;
	           break;
	}
	if (str[0]) {
		str[0]=Flash_bread(pnt+NETWORK_WLAN_WEPID);
		if (EthCardStruct.WEPID!=str[0]) goto RESET;
	}
	return TRUE;
ONLY_DHCP:
	Flash_sread(pnt+NETWORK_MACHINE_REMOTE_IP,str,4);
	if (!((str[0]==EthCardStruct.TCPDestIP[3]) && (str[1]==EthCardStruct.TCPDestIP[2]) && (str[2]==EthCardStruct.TCPDestIP[1]) && (str[3]==EthCardStruct.TCPDestIP[0]))) goto RESET;
	return TRUE;
RESET:
	return FALSE;
}

void EthReConfig(void)
{
	memset((INT8U *)&(ethernet_list.master),0,sizeof(ETH_CONNECT_LIST));
	EthCardStruct.UDPSourcePort=EthCardStruct.UDPDestPort;
	if (EthCardStruct.DHCPEnable != 1) goto N1;
N1:
	EthCardStruct.ConnTimeOut=ETH_DEFAULT_CONNECT_TIME_OUT;  //5sec
	ethernet_list.master.scaleid=get_net_wparam(NETWORK_SCALE_ID);
	ethernet_set_flash2card();
	ethernet_list.master.ip[0]=EthCardStruct.IP[3];
	ethernet_list.master.ip[1]=EthCardStruct.IP[2];
	ethernet_list.master.ip[2]=EthCardStruct.IP[1];
	ethernet_list.master.ip[3]=EthCardStruct.IP[0];
	ethernet_list.master.port =EthCardStruct.TCPSourcePort;
}

void ethernet_set_flash2card(void)
{
	INT8U str[5];//,stype;//,temp[20];
	INT16U rport,tport;

	EthCardStruct.InterChDelay = ETH_DEFAULT_INTER_CH_DELAY;
	EthCardStruct.WindowSize   = ETH_DEFAULT_WINDOW_SIZE;
	EthCardStruct.TCPIPConnNum = ETH_DEFAULT_CLI_MAX_NUM_4_SRV;
	EthCardStruct.ConnTimeOut  = ETH_DEFAULT_CONNECT_TIME_OUT;
	EthCardStruct.ParallelSerialFlag = ETH_DEFAULT_PS_FLAG;
	EthCardStruct.OPMode  = ETH_DEFAULT_OP_MODE;
	EthCardStruct.Channel = ETH_DEFAULT_CHANNEL;	
	EthCardStruct.TxRate  = ETH_DEFAULT_TX_RATE;
	EthCardStruct.APDensity = ETH_DEFAULT_AP_DENSITY;	
	EthCardStruct.DenyUnencrypted = ETH_DEFAULT_DENY_UNENCRYPTED;
	EthCardStruct.ShareKeyAuth    = ETH_DEFAULT_SHARE_KEY_AUTH;

	get_net_sparam(NETWORK_LOCAL_IP,str,4);
      	EthCardStruct.IP[3]=str[0];
      	EthCardStruct.IP[2]=str[1];
      	EthCardStruct.IP[1]=str[2];
      	EthCardStruct.IP[0]=str[3];
	memcpy(ethernet_list.localip,str,4);

	get_net_sparam(NETWORK_LOCAL_GATEWAY,str,4);
	EthCardStruct.Gateway[3]=str[0];
	EthCardStruct.Gateway[2]=str[1];
	EthCardStruct.Gateway[1]=str[2];
	EthCardStruct.Gateway[0]=str[3];

	get_net_sparam(NETWORK_LOCAL_SUBNETMASK,str,4);
	EthCardStruct.SubnetMask[3]=str[0];
	EthCardStruct.SubnetMask[2]=str[1];
	EthCardStruct.SubnetMask[1]=str[2];
	EthCardStruct.SubnetMask[0]=str[3];

	tport=get_net_wparam(NETWORK_LOCAL_TCP_PORT);

	EthCardStruct.TCPSourcePort = tport;
	EthCardStruct.TCPClientPort = tport+1;
	EthCardStruct.UDPSourcePort=tport;
	EthCardStruct.UDPDestPort  =tport;

	rport=get_net_wparam(NETWORK_MACHINE_REMOTE_TCPPORT);
	if ((rport==0xffff) || (rport==0)) {
		set_net_wparam(NETWORK_MACHINE_REMOTE_TCPPORT,tport);
		set_net_wparam(NETWORK_MACHINE_REMOTE_UDPPORT,tport);
		rport=tport;
	}
	EthCardStruct.TCPDestPort=rport;
#ifdef USE_DNS_RESOLVER
	EthCardStruct.DHCPEnable=get_net_bparam(NETWORK_LOCAL_ETHERNET_FLAG) & 0x01;
#else
	EthCardStruct.DHCPEnable=get_net_bparam(NETWORK_LOCAL_ETHERNET_FLAG);
#endif
	get_net_sparam(NETWORK_MACHINE_REMOTE_IP, str, 4);
	EthCardStruct.TCPDestIP[3]=str[0];
	EthCardStruct.TCPDestIP[2]=str[1];
	EthCardStruct.TCPDestIP[1]=str[2];
	EthCardStruct.TCPDestIP[0]=str[3];
	if (EthCardStruct.Ver[0]=='W') 
	{
		str[0] = get_net_bparam(NETWORK_WLAN_USEWEP);
		EthCardStruct.WEPEnable=0;
		switch (str[0]) {
//			case 0:
			case 1: EthCardStruct.WEPEnable=1;
				EthCardStruct.WEPLength=0;  break;
			case 2: EthCardStruct.WEPLength=1;
				EthCardStruct.WEPEnable=1;  break;
		}
		get_net_sparam(NETWORK_WLAN_SSID,EthCardStruct.SSID,32);
		EthCardStruct.WEPID = get_net_bparam(NETWORK_WLAN_WEPID);

		switch (EthCardStruct.WEPID) {
			case 1: get_net_sparam(NETWORK_WLAN_WEP,EthCardStruct.WEPKey1,13);break;
			case 2: get_net_sparam(NETWORK_WLAN_WEP,EthCardStruct.WEPKey2,13);break;
			case 3: get_net_sparam(NETWORK_WLAN_WEP,EthCardStruct.WEPKey3,13);break;
			case 4: get_net_sparam(NETWORK_WLAN_WEP,EthCardStruct.WEPKey4,13);break;
		}
	}
	
	EthSetData(0); 
}

void ethernet_fwset(INT32U fh, INT32U fl)
{
	INT8U str[5];
	INT16U port;
	INT16S i;

	memset(&EthCardStruct,0,sizeof(EthCardStruct));
	//HYP 20041214 Do not modify 0 value
	EthCardStruct.InterChDelay = ETH_DEFAULT_INTER_CH_DELAY;
	EthCardStruct.WindowSize = ETH_DEFAULT_WINDOW_SIZE;
	EthCardStruct.TCPIPConnNum = ETH_DEFAULT_CLI_MAX_NUM_4_SRV;
	EthCardStruct.ConnTimeOut = ETH_DEFAULT_CONNECT_TIME_OUT;
	EthCardStruct.ParallelSerialFlag = ETH_DEFAULT_PS_FLAG;
	EthCardStruct.OPMode = ETH_DEFAULT_OP_MODE;
	EthCardStruct.Channel = ETH_DEFAULT_CHANNEL;	
	EthCardStruct.TxRate = ETH_DEFAULT_TX_RATE;
	EthCardStruct.APDensity = ETH_DEFAULT_AP_DENSITY;	
	EthCardStruct.DenyUnencrypted = ETH_DEFAULT_DENY_UNENCRYPTED;
	EthCardStruct.ShareKeyAuth = ETH_DEFAULT_SHARE_KEY_AUTH;
//	pnt =DFLASH_BASE;
//	pnt+=FLASH_NET_AREA;

	for(i=0; i<8; i++) {
		EthCardStruct.Ver[i] = '\0';
	}
	EthCardStruct.MAC[5] = (INT8U)(fh>>16);
	EthCardStruct.MAC[4] = (INT8U)(fh>>8);
	EthCardStruct.MAC[3] = (INT8U)(fh);
	EthCardStruct.MAC[2] = (INT8U)(fl>>16);
	EthCardStruct.MAC[1] = (INT8U)(fl>>8);
	EthCardStruct.MAC[0] = (INT8U)(fl);
	get_net_sparam(NETWORK_LOCAL_IP,str,4);
      	EthCardStruct.IP[3]=str[0];
      	EthCardStruct.IP[2]=str[1];
      	EthCardStruct.IP[1]=str[2];
      	EthCardStruct.IP[0]=str[3];
	get_net_sparam(NETWORK_LOCAL_GATEWAY,str,4);
	EthCardStruct.Gateway[3]=str[0];
	EthCardStruct.Gateway[2]=str[1];
	EthCardStruct.Gateway[1]=str[2];
	EthCardStruct.Gateway[0]=str[3];
	get_net_sparam(NETWORK_LOCAL_SUBNETMASK,str,4);
	EthCardStruct.SubnetMask[3]=str[0];
	EthCardStruct.SubnetMask[2]=str[1];
	EthCardStruct.SubnetMask[1]=str[2];
	EthCardStruct.SubnetMask[0]=str[3];

	port=get_net_wparam(NETWORK_LOCAL_TCP_PORT);

	EthCardStruct.TCPSourcePort = port;
	EthCardStruct.TCPClientPort = port+1;

//	port=get_net_wparam(NETWORK_LOCAL_UDP_PORT);
	EthCardStruct.UDPSourcePort=port;
	EthCardStruct.UDPDestPort  =port;

	get_net_sparam(NETWORK_MACHINE_REMOTE_IP,str,4);
	EthCardStruct.TCPDestIP[3]=str[0];
	EthCardStruct.TCPDestIP[2]=str[1];
	EthCardStruct.TCPDestIP[1]=str[2];
	EthCardStruct.TCPDestIP[0]=str[3];

	port=get_net_wparam(NETWORK_MACHINE_REMOTE_TCPPORT);
	EthCardStruct.TCPDestPort = port;

	EthSetAllData(0);
}

extern void Delay_10ms_w_TcpIp(INT16U cnt);

extern void commun_outleng(HUGEDATA COMM_BUF *CBuf,char *str,INT16S leng);
extern void change_dest_ip(INT8U *ip, INT16U tcp_port);
extern void restore_dest_ip(void);
#ifdef USE_EFT_POS_PROTOCOL
extern INT8U ChangeRemote2IPFlag;
#endif
extern INT8U ChangeDestIPFlag;
extern INT8U PutDestIPFlag;
extern INT8U TCPDestTempIP[];
extern INT16U TCPDestTempPort;
// open = 0 : CLIENT_CLOSE
//        1 : CLIENT_OPEN
//        2 : CLIENT_CHECK
// mode : 우선순위가 높으면 사용 가능, 우선순위가 같은 레벨(mode/10) 또는 낮으면 사용 불가
//   ex) cur_mode = 22이면, mode = 10 사용 불가
//                          mode = 23 사용 불가
//                          mode = 30 사용 가능
INT8U Ethernet_Server(INT8U open, INT16U tcpclose_time, INT8U mode)
{
	INT8U   tcp_state;
	INT16U	wait_cnt;//,nth;
	INT8U   ret;
	char    loop;
	static INT8U cur_mode = 0;
	INT8U   closeAndOpen;
	INT8U ip[4];
#ifdef USE_EFT_POS_PROTOCOL
	INT16U port;
#endif

//sprintf(MsgBuf, " %6u  (%d)ip=%d [C%d,M%d](P%d,C%d)\r\n", PrtDrvStruct.Timer10ms, open, EthCardStruct.TCPDestIP[0], cur_mode, mode, PutDestIPFlag, ChangeDestIPFlag);
//MsgOut(MsgBuf);

	closeAndOpen = OFF;
	if (cur_mode == 0);	// set cur_mode
	else if (cur_mode != mode)
	{	// return 0;	// prevent other mode
		if (cur_mode/10 >= mode/10)	// 현재 mode가 더 높거나 같은 우선순위 대 이면 거절
		{
			PutDestIPFlag = OFF;
			return 0;
		}
		closeAndOpen = ON;
//sprintf(MsgBuf, "[%6u] (%d)ip=%d [C%d,M%d]\r\n", PrtDrvStruct.Timer10ms, open, EthCardStruct.TCPDestIP[0], cur_mode, mode);
//MsgOut(MsgBuf);
	}
	cur_mode = mode;

	if (PutDestIPFlag)	// open 일 때만
	{
		if (open == CLIENT_OPEN)
		{
			change_dest_ip(TCPDestTempIP, TCPDestTempPort);
		}
		PutDestIPFlag = OFF;
	}
//sprintf(MsgBuf, "<%6u> (%d)ip=%d [C%d,M%d]\r\n", PrtDrvStruct.Timer10ms, open, EthCardStruct.TCPDestIP[0], cur_mode, mode);
//MsgOut(MsgBuf);
///////////////////////////////////
// Close
///////////////////////////////////
	if (tcpclose_time == 0) tcpclose_time = 30;//TODO: check time for client
	else if (tcpclose_time == 0xffff) tcpclose_time = 0;

//sprintf(MsgBuf, "<[CL](%d)ip=%d.%d.%d.%d:%d>[%d]", open, EthCardStruct.TCPDestIP[3],EthCardStruct.TCPDestIP[2],EthCardStruct.TCPDestIP[1],EthCardStruct.TCPDestIP[0],EthCardStruct.TCPDestPort, closeAndOpen);
//MsgOut(MsgBuf);
	if (open == CLIENT_CLOSE || closeAndOpen) {
//debug_tic('c');	//DBG080314
//		if (network_status.service_bflag2) return 1; //SG060404

		EthLocalCmdSend(TCP_CLOSE_COMMAND,0);
		loop=1;
		wait_cnt = 0;
		ret      = 0;
		while (1) {
			tcp_state=ethernet_tcpstate();
//sprintf(MsgBuf, "<[C]st=%02x>", tcp_state);
//MsgOut(MsgBuf);
			switch(tcp_state) {
				case NOSOCKET:
				case CLOSE : loop=0; ret=1; break;
	      			case LISTEN: loop=0; break;
				case SYNSENT:	break;     
				case SYNRCVED:  break;   
				case ESTABED:	break;     
				case FINWAIT1:  break;   
				case FINWAIT2:  break;   
				case CLOSEWAIT: break;   
				case CLOSING:	break;     
				case LASTACK:	break;     
				case TIMEWAIT:  break;   
				default:	break;
			}
			wait_cnt++;
			if((loop==0) || (wait_cnt>tcpclose_time)) break;
//			Delay_100ms(5);
//			Delay_10ms(1);  //SG060615
			Delay_10ms_w_TcpIp(1);
		}
		if (ChangeDestIPFlag)	// Dest. IP 변경했을 때,
		{
#ifdef USE_EFT_POS_PROTOCOL
			if(ChangeRemote2IPFlag)	// Remote IP2로 변경하는 것인지 확인
			{
				get_base_sparam(FLASH_REMOTE_IP2, ip, 4);
				port = get_base_wparam(FLASH_REMOTE_IP2 + 4);
			}
			else
			{
				get_net_sparam(NETWORK_MACHINE_REMOTE_IP, ip, 4);
				port = get_net_wparam(NETWORK_MACHINE_REMOTE_TCPPORT);
			}

			if (EthCardStruct.TCPDestIP[0] != ip[3] || EthCardStruct.TCPDestPort != port)
			{
				restore_dest_ip();
			}

			if(ChangeRemote2IPFlag == ON)
			{
				ChangeRemote2IPFlag = OFF;
			}
#else
			get_net_sparam(NETWORK_MACHINE_REMOTE_IP, ip, 4);
			if (EthCardStruct.TCPDestIP[0] != ip[3])	// 끝 IP만 비교
			{
				restore_dest_ip();
//sprintf(MsgBuf, "#%6u# (%d)ip=%d [C%d,M%d]\r\n", PrtDrvStruct.Timer10ms, open, EthCardStruct.TCPDestIP[0], cur_mode, mode);
//MsgOut(MsgBuf);
			}
#endif
		}
		if (CheckTxBuf(&(CommBufEthData[0])))	// close후 버퍼에 남아 있으면 삭제
		{
			FlushTxBuf(&(CommBufEthData[0])); //clear tx buf at first
		}
		if (!closeAndOpen)	// only close
		{
			cur_mode = 0;	// init cur_mode
			return ret;
		}
	}
///////////////////////////////////
// Open Step1. Check
///////////////////////////////////
//debug_tic('h');	//DBG080314
	if ((EthCardStruct.TCPDestIP[0]==0) && (EthCardStruct.TCPDestIP[1]==0)) {
		cur_mode = 0;	// init cur_mode
		return 2;
	}
//CJK080610	port=get_net_wparam(NETWORK_MACHINE_REMOTE_TCPPORT);
//CJK080610	EthCardStruct.TCPDestPort = port;
	ret     =0;
	wait_cnt=0;
	loop    =1;
	while (1) {
		tcp_state=ethernet_tcpstate();
//sprintf(MsgBuf, "<[CK]st=%02x>", tcp_state);
//MsgOut(MsgBuf);
		switch(tcp_state) {
			case NOSOCKET:
			case CLOSE   :  loop=0; break;
			case LISTEN  :  
					//if (network_status.service_bflag2) 
					return 1; //SG060327
					//break;
			case SYNSENT :	break;
			case SYNRCVED:  break;
			case ESTABED:	//if (network_status.service_bflag2) 
					return 1; //SG060327
					//break;
			case FINWAIT1:  break;   
			case FINWAIT2:  break;   
			case CLOSEWAIT: break;   
			case CLOSING:	break;     
			case LASTACK:	break;     
			case TIMEWAIT:  break;   
			default:	break;
		}
		wait_cnt++;
		if((loop==0) || (wait_cnt>tcpclose_time)) break;
	}
	if (open == CLIENT_CHECK)
	{
		//cur_mode = 0;	// init cur_mode
		return 0;	// check only
	}
//debug_tic('o');	//DBG080314
	wait_cnt=0;
//	nth     =0;
	EthLocalCmdSend(TCP_OPEN_COMMAND,0);
	ret =0;
	loop=1;

//	if (network_status.service_bflag2) tcpclose_time=20;  //SG060615 for KorTrace

	while (1) {
		tcp_state=ethernet_tcpstate();
//sprintf(MsgBuf, "<[O]st=%02x>", tcp_state);
//MsgOut(MsgBuf);
		switch(tcp_state) {
			case NOSOCKET:
			case CLOSE   :  break; // client
			case LISTEN  :  loop=0; ret=1; break; //
			// Opennig    
			case SYNSENT :	break; // client
			case SYNRCVED:  break; // 
			case ESTABED :	loop=0; ret=1;
					break;
			case FINWAIT1:  break;   
			case FINWAIT2:  break;   
			case CLOSEWAIT: break;   
			case CLOSING:	break;     
			case LASTACK:	break;     
			case TIMEWAIT:  break;   
			default:	break;
		}
		wait_cnt++;
		if(wait_cnt>tcpclose_time)	// 연결 실패시
		{
			if (tcpclose_time) cur_mode = 0;	// init cur_mode, waiting time이 있을 때만,(tcpclose_time == 0xffff)
			break;
		}
		if (!loop) break;
		//Delay_100ms(5);
		//Delay_10ms(1);
		Delay_10ms_w_TcpIp(1);
	}
//sprintf(MsgBuf, "<O>=>%d\r\n", ret);
//MsgOut(MsgBuf);
//sprintf(MsgBuf, "ip=%d\r\n", EthCardStruct.TCPDestIP[0]);
//MsgOut(MsgBuf);
	return ret;
}

INT8U ethernet_tcpstate(void) 
{	//now only client
//static INT8U prev_state=0xff;
//	char temp[32];
	EthCheckStatusID = 0;//client
	EthCheckStatusIP[0]=EthCardStruct.TCPDestIP[0];
	EthCheckStatusIP[1]=EthCardStruct.TCPDestIP[1];
	EthCheckStatusIP[2]=EthCardStruct.TCPDestIP[2];
	EthCheckStatusIP[3]=EthCardStruct.TCPDestIP[3];
//	sprintf(temp,"[%3d.%3d.%3d.%3d]",EthCardStruct.TCPDestIP[0],
//	EthCardStruct.TCPDestIP[1],
//	EthCardStruct.TCPDestIP[2],
//	EthCardStruct.TCPDestIP[3]);
//	MsgOut(temp);
	EthLocalCmdSend(TCP_STATE_REQ_COMMAND,0);
//if (EthStruct.TcpState != prev_state)	// DBG080314
//{
//prev_state = EthStruct.TcpState;
//sprintf(MsgBuf, "[tcp=%02X]", EthStruct.TcpState);
//MsgOut(MsgBuf);
//}
	return EthStruct.TcpState;
}

extern void wlan_SetNetwork(INT8U *ip, INT8U *gw, INT8U *sn);
extern void Ethernet_RestoreNetwork(void);
void EthTelnetToBridge(void)
{
	// default bridge ip
//	INT8U ip[4] = {10,10,4,254};
	INT8U ip_l[4] = {10,10,4,253};
	INT8U gw[4] = {10,10,0,1};
	INT8U sn[4] = {255,0,0,0};

	wlan_SetNetwork(ip_l, gw, sn);	// change local IP for w/l setting
	EthCardStruct.TCPDestIP[3] = 10;
	EthCardStruct.TCPDestIP[2] = 10;
	EthCardStruct.TCPDestIP[1] = 4;
	EthCardStruct.TCPDestIP[0] = 254;
	EthCardStruct.TCPDestPort = 9999;
	Ethernet_Server(CLIENT_OPEN, 0, CLIENT_NORMAL);
	while(1)
	{
		if (CheckRxBuf(&(CommBufEthData[0]))) { 
				PutStrTxBuf_copy(&CommBuf, &(CommBufEthData[0]));	// BRIDGE -> HOST
		}
		if (CheckRxBuf(&CommBuf)) {
				PutStrTxBuf_copy(&(CommBufEthData[0]), &CommBuf);	// HOST -> BRIDGE
		}
		network_tx_proc();
		if(ESTABED != ethernet_tcpstate()) break;
	}
	Ethernet_Server(CLIENT_CLOSE, 0, CLIENT_NORMAL);

	Ethernet_RestoreNetwork();// restore local IP
}

