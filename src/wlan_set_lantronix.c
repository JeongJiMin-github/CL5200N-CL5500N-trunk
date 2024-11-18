/*****************************************************************************
|*			
|*  Copyright		:	(c) 2008 CAS
|*  Filename		:	wlan_set_lantronix.c
|*  Version		:	0.1
|*  Programmer(s)	:	Chung Jong Kwon (CJK)
|*  Created		:	2008/10/28
|*  Description		:	Wireless Bridge Setting Function    
|*				Bridge Vendor : LANTRONIX
|*				Bridge Module : Matchport b/g
|*				
****************************************************************************/
#include <stdio.h>
#include <string.h>
#include "globals.h"
#include "commbuf.h"
#include "main.h"
//#include "ethernet.h"
//#include "mode_commun.h"
#include "wlan_set_lantronix.h"
#include "common.h"
#include "initial.h"

#ifdef CPU_PIC32MZ
#include "ethernet.h"
#include "app.h"
extern APP_DATA appData;
#endif

ap_info conn_ap_info;
//#define _LATRONIX_DEBUG_
extern INT32U   getSn_RX_RSR(SOCKET s);
extern void Ethernet_SetNetwork(INT8U *ip, INT8U *gw, INT8U *sn);

void send_UDP_GetSetupRecord(SOCKET s, INT8U recordNo, INT8U reboot, INT16U port)
{
	INT16U i;
#ifndef CPU_PIC32MZ
	INT8U ip[4];
#endif
	INT8U command[4];

	command[0] = 0x00;
	if (reboot) command[1] = 0x00;
	else        command[1] = 0x01;
	command[2] = 0x00;
	command[3] = WLTX_CMD_GET_CONFIG0 + recordNo;

#ifdef CPU_PIC32MZ
	appData.ucaddr.sin_addr.S_un.S_addr = 0xFFFFFFFF;
	appData.ucaddr.sin_port = port;
	sendto(s, (char *)command, 4, 0, (struct sockaddr *)&appData.ucaddr, sizeof(struct sockaddr_in));
#else
	// send broadcasting packet
	for (i = 0; i < 4; i++) ip[i] = 255;

	sendto(s, (HUGEDATA INT8U *)command, 4, ip, port);
#endif
}

void send_UDP_SetSetupRecord(SOCKET s, INT8U recordNo, INT8U *recordStr, INT8U reboot, INT16U port)
{
	INT16U i, j;
#ifndef CPU_PIC32MZ
	INT8U ip[4];
#endif
	INT8U command[512];
	INT8U len;

	i = 0;
	command[i++] = 0x00;
	if (reboot) command[i++] = 0x00;
	else        command[i++] = 0x01;
	command[i++] = 0x00;
	command[i++] = WLTX_CMD_SET_CONFIG0 + recordNo;
	len = 126;
	if (recordNo == 0) len = 120;
	memcpy(&command[i], recordStr, len);
	i += len;

#ifdef CPU_PIC32MZ
	appData.ucaddr.sin_addr.S_un.S_addr = 0xFFFFFFFF;
	appData.ucaddr.sin_port = port;
	sendto(s, (char *)command, i, 0, (struct sockaddr *)&appData.ucaddr, sizeof(struct sockaddr_in));
#else
	// send broadcasting packet
	for (j = 0; j < 4; j++) ip[j] = 255;
		
	sendto(s, (HUGEDATA INT8U *)command, i, ip, port);
#endif
}


void send_UDP_GetExtVersion(SOCKET s, INT16U port)
{
	INT16U i;
#ifndef CPU_PIC32MZ
	INT8U ip[4];
#endif
	INT8U command[4];

	command[0] = 0x00;
	command[1] = 0x01;
	command[2] = 0x00;
	command[3] = WLTX_CMD_EXT_VERSION;

#ifdef CPU_PIC32MZ
	appData.ucaddr.sin_addr.S_un.S_addr = 0xFFFFFFFF;
	appData.ucaddr.sin_port = port;
	sendto(s, (char *)command, 4, 0, (struct sockaddr *)&appData.ucaddr, sizeof(struct sockaddr_in));
#else
	// send broadcasting packet
	for (i = 0; i < 4; i++) ip[i] = 255;
		
	sendto(s, (HUGEDATA INT8U *)command, 4, ip, port);
#endif
}

void send_UDP_GetConInfo(SOCKET s, INT16U port)
{
	INT16U i;
#ifndef CPU_PIC32MZ
	INT8U ip[4];
#endif
	INT8U command[4];

	command[0] = 0x00;
	command[1] = 0x01;
	command[2] = 0x00;
	command[3] = WLTX_CMD_GET_CONINFO;

#ifdef CPU_PIC32MZ
	appData.ucaddr.sin_addr.S_un.S_addr = 0xFFFFFFFF;
	appData.ucaddr.sin_port = port;
	sendto(s, (char *)command, 4, 0, (struct sockaddr *)&appData.ucaddr, sizeof(struct sockaddr_in));
#else
	// send broadcasting packet
	for (i = 0; i < 4; i++) ip[i] = 255;
		
	sendto(s, (HUGEDATA INT8U *)command, 4, ip, port);
#endif
}

void send_UDP_NodeReset(SOCKET s, INT8U wtype)	// wtype:2=lantronix, 3=tobnet
{
	INT16U i;
#ifndef CPU_PIC32MZ
	INT8U ip[4];
#endif
	INT16U port;
	INT8U command[6];
	INT16U send_time;

#ifdef CPU_PIC32MZ
	s = appData.UdpClientSocket;
#endif
	command[0] = 0x00;
	command[1] = 0x01;
	command[2] = 0x00;
	command[3] = 0x03;
	if (wtype == WLTYPE_LANTRONIX)	// lantronix
	{
		command[4] = 0x57;	// software type : "W8"
		command[5] = 0x38;	// software type
		port = WLAN_CONTROL_PORT_LANTRONIX;
	}
	else if (wtype == WLTYPE_TOBNET)// tobnet...
	{
		command[4] = 0x00;
		command[5] = 0x00;
		port = WLAN_CONTROL_PORT_TOBNET;
	}
	else
	{ 
		return;
	}

	send_time = SysTimer100ms;
	while (1)  // wait a moment
	{
		TcpIp_Task();
		if ((INT16U)(SysTimer100ms - send_time) > 6) break;
	}

#ifdef CPU_PIC32MZ
	appData.ucaddr.sin_addr.S_un.S_addr = 0xFFFFFFFF;
	appData.ucaddr.sin_port = port;
	sendto(s, (char *)command, 6, 0, (struct sockaddr *)&appData.ucaddr, sizeof(struct sockaddr_in));
#else
	// send broadcasting packet
	for (i = 0; i < 4; i++) ip[i] = 255;
		
	sendto(s, (HUGEDATA INT8U *)command, 6, ip, port);
#endif
	
	send_time = SysTimer100ms;
	while (1)  // wait a moment
	{
		TcpIp_Task();
		if ((INT16U)(SysTimer100ms - send_time) > 3) break;
	}
}

/*
//void send_UDP_GetFwVersion(SOCKET s, INT16U port)
{
	INT16U i;
	INT8U ip[4];
	//INT16U port;
	INT16U k = 0;
	INT8U command[4];

	command[0] = 0x00;
	command[1] = 0x01;
	command[2] = 0x00;
	command[3] = WLTX_CMD_FW_VERSION;

	// send broadcasting packet
	for (i = 0; i < 4; i++) ip[i] = 255;
	//port = 0x77fe;

	sendto(s, (HUGEDATA INT8U *)command, 4, ip, port);
}
*/

INT8U convert_subnet_ip2bit(INT8U *subnet)
{
	INT8U subnet_bit;
	INT8U mask;
	INT8U i, j;

	subnet_bit = 0;
	for (i = 0; i < 4; i++)
	{
		mask = 0x01;
		for (j = 0; j < 8; j++)
		{
			if (!(subnet[3-i] & mask)) subnet_bit++;
			else return subnet_bit;
			mask <<= 1;
		}
	}
	return 32;
}

void convert_subnet_bit2ip(INT8U bit, INT8U *subnet_ip)
{
	INT32U subnet_val;
	INT32U mask;
	INT8U i;

	mask = 0x80000000;
	subnet_val = 0; 
	for (i = 0; i < bit; i++)
	{
		subnet_val |= mask;
		mask >>= 1;
	}
	subnet_ip[0] = subnet_val / 0x1000000;	
	subnet_ip[1] = (subnet_val / 0x10000) % 0x100; 
	subnet_ip[2] = (subnet_val / 0x100) % 0x100; 
	subnet_ip[3] = subnet_val  % 0x100;
}

void wlan_convertEncryptionKey(INT8U *dest_key, INT8U *src_key)
{
	INT8U max_keylen;
	INT8U j, k;

	max_keylen = strlen((char*)src_key);
	if (max_keylen >= 64) 
	{
		max_keylen = 64;
		k = 0;
		for (j = 0; j < max_keylen; )
		{
			dest_key[k] = ctoh(src_key[j++]);
			dest_key[k] <<= 4;
			dest_key[k] |= ctoh(src_key[j++]);
			k++;
		}
	}
	else
	{
		memcpy(dest_key, src_key, max_keylen);
	}
}

INT8U wlan_SetIP_Lantronix(INT8U *ip, INT8U *gateway, INT8U *subnet)
{
	INT16U send_time;
	INT8U buf[256];
	INT16U len;
	SOCKET s;
	INT8U destip[4];
	INT16U destport;
	WLAN_SETUP_RECORD0	record0;
#ifdef CPU_PIC32MZ
	s = appData.UdpClientSocket;
#else
	s = 5;	// UDP
#endif

	send_UDP_GetSetupRecord(s, 0, OFF, WLAN_CONTROL_PORT_LANTRONIX);
	send_time = SysTimer100ms;

	while ((len=getSn_RX_RSR(s)) < 124)                   // check the size of received data
	{
		if ((INT16U)(SysTimer100ms - send_time) > 20) break;
	}
	if (len)
	{
		if (len > 256) len = 256;
#ifdef CPU_PIC32MZ
		//len = recvfrom(s, (char*)buf, len, 0, (struct sockaddr *)&clientaddr, &ulen);
		len = CheckRxBufCountInt(&(CommBufEthData[UDP_SOCKET_BUFFER_NTH]));
		GetDataEthDataRxBuf(&(CommBufEthData[UDP_SOCKET_BUFFER_NTH]), (HUGEDATA char *)&buf, len);
#else
		len = recvfrom(s,buf,len,destip,&destport);  // receive data from a destination
#endif

		if (buf[3] == 0xd0 && len == 124)
		{
#ifdef _LATRONIX_DEBUG_
			INT8U i;
			for (i = 0; i < sizeof(WLAN_SETUP_RECORD0); i++)
			{
				sprintf(MsgBuf, "%02X ", buf[i + 4]);
				MsgOut(MsgBuf);
				if ((i%16) == 15)
				{
					sprintf(MsgBuf, "\r\n");
					MsgOut(MsgBuf);
				}
			}
			sprintf(MsgBuf, "(L0)\r\n");
			MsgOut(MsgBuf);
#endif
			memcpy(&record0, &buf[4], sizeof(WLAN_SETUP_RECORD0));
			record0.IP[0] = ip[0];
			record0.IP[1] = ip[1];
			record0.IP[2] = ip[2];
			record0.IP[3] = ip[3];
			record0.gatewayIP[0] = gateway[0];
			record0.gatewayIP[1] = gateway[1];
			record0.gatewayIP[2] = gateway[2];
			record0.gatewayIP[3] = gateway[3];
			record0.noHostBitsForSubnet = convert_subnet_ip2bit(subnet);
			record0.flagByte = 0x04;
			memset(record0.telnetConfigPswd, 0, 4);
			send_UDP_SetSetupRecord(s, 0, (INT8U *)&record0, OFF, WLAN_CONTROL_PORT_LANTRONIX);

			send_time = SysTimer100ms;
			while ((len=getSn_RX_RSR(s)) < 4)                   // check the size of received data
			{
				if ((INT16U)(SysTimer100ms - send_time) > 20) break;
			}
			if (len)
			{
				if (len > 256) len = 256;
#ifdef CPU_PIC32MZ
				//len = recvfrom(s, (char*)buf, len, 0, (struct sockaddr *)&clientaddr, &ulen);
				len = CheckRxBufCountInt(&(CommBufEthData[UDP_SOCKET_BUFFER_NTH]));
				GetDataEthDataRxBuf(&(CommBufEthData[UDP_SOCKET_BUFFER_NTH]), (HUGEDATA char *)&buf, len);
#else
				len = recvfrom(s, buf, len, destip, &destport);  // receive data from a destination
#endif
				if (buf[3] == 0xb0 && len == 4)
				{
					return ON;
				}
#ifdef _LATRONIX_DEBUG_
				else
				{
					INT8U i;
					for (i = 0; i < len; i++)
					{
						sprintf(MsgBuf, "%02X ", buf[i]);
						MsgOut(MsgBuf);
						if ((i%16) == 15)
						{
							sprintf(MsgBuf, "\r\n");
							MsgOut(MsgBuf);
						}
					}
					sprintf(MsgBuf, "(R0)\r\n");
					MsgOut(MsgBuf);
				}
#endif
			}
		}
	}
	return OFF;
}

INT8U wlan_SetIP_Tobnet(INT8U *ip, INT8U *gateway, INT8U *subnet)
{
	INT16U send_time;
	INT8U buf[256];
	INT16U len;
	SOCKET s;
	INT8U destip[4];
	INT16U destport;
	WLAN_SETUP_RECORD0_TOBNET	record0;
#ifdef CPU_PIC32MZ
	s = appData.UdpClientSocket;
#else
	s = 5;	// UDP
#endif

	send_UDP_GetSetupRecord(s, 0, OFF, WLAN_CONTROL_PORT_TOBNET);
#ifdef _LATRONIX_DEBUG_
	sprintf(MsgBuf, "send GET T0\r\n");
	MsgOut(MsgBuf);
#endif
	send_time = SysTimer100ms;

	while ((len=getSn_RX_RSR(s)) < 124)                   // check the size of received data
	{
		if ((INT16U)(SysTimer100ms - send_time) > 20)
		{
#ifdef _LATRONIX_DEBUG_
			sprintf(MsgBuf, "rcv timeout(T0)\r\n");
			MsgOut(MsgBuf);
#endif
			break;
		}
	}
	if (len)
	{
		if (len > 256) len = 256;
#ifdef CPU_PIC32MZ
		//len = recvfrom(s, (char*)buf, len, 0, (struct sockaddr *)&clientaddr, &ulen);
		len = CheckRxBufCountInt(&(CommBufEthData[UDP_SOCKET_BUFFER_NTH]));
		GetDataEthDataRxBuf(&(CommBufEthData[UDP_SOCKET_BUFFER_NTH]), (HUGEDATA char *)&buf, len);
#else
		len = recvfrom(s,buf,len,destip,&destport);  // receive data from a destination
#endif

#ifdef _LATRONIX_DEBUG_
		{
		INT8U i;
		sprintf(MsgBuf, "rcv len=%d(GET T0)\r\n", len);
		MsgOut(MsgBuf);
		for (i = 0; i < len; i++)
		{
			sprintf(MsgBuf, "%02X ", buf[i]);
			MsgOut(MsgBuf);
			if ((i%16) == 15)
			{
				sprintf(MsgBuf, "\r\n");
				MsgOut(MsgBuf);
			}
		}
		sprintf(MsgBuf, "(T0)\r\n");
		MsgOut(MsgBuf);
		}
#endif
		if (buf[3] == 0xd0 && len == 124)
		{
#ifdef _LATRONIX_DEBUG_
//			INT8U i;
//			for (i = 0; i < sizeof(WLAN_SETUP_RECORD0_TOBNET); i++)
//			{
//				sprintf(MsgBuf, "%02X ", buf[i + 4]);
//				MsgOut(MsgBuf);
//				if ((i%16) == 15)
//				{
//					sprintf(MsgBuf, "\r\n");
//					MsgOut(MsgBuf);
//				}
//			}
//			sprintf(MsgBuf, "(T0)\r\n");
//			MsgOut(MsgBuf);
#endif
			memcpy(&record0, &buf[4], sizeof(WLAN_SETUP_RECORD0_TOBNET));
			record0.IP[0] = ip[0];
			record0.IP[1] = ip[1];
			record0.IP[2] = ip[2];
			record0.IP[3] = ip[3];
			record0.gatewayIP[0] = gateway[0];
			record0.gatewayIP[1] = gateway[1];
			record0.gatewayIP[2] = gateway[2];
			record0.gatewayIP[3] = gateway[3];
			//record0.noHostBitsForSubnet = convert_subnet_ip2bit(subnet);
			record0.subnet[0] = subnet[0];
			record0.subnet[1] = subnet[1];
			record0.subnet[2] = subnet[2];
			record0.subnet[3] = subnet[3];
			//record0.flagByte = 0x04;
			//memset(record0.telnetConfigPswd, 0, 4);
			send_UDP_SetSetupRecord(s, 0, (INT8U *)&record0, OFF, WLAN_CONTROL_PORT_TOBNET);
#ifdef _LATRONIX_DEBUG_
			sprintf(MsgBuf, "send SET T0\r\n");
			MsgOut(MsgBuf);
#endif
			send_time = SysTimer100ms;
			while ((len=getSn_RX_RSR(s)) < 4)                   // check the size of received data
			{
				if ((INT16U)(SysTimer100ms - send_time) > 20) break;
			}
			if (len)
			{
				if (len > 256) len = 256;
#ifdef CPU_PIC32MZ
				//len = recvfrom(s, (char*)buf, len, 0, (struct sockaddr *)&clientaddr, &ulen);
				len = CheckRxBufCountInt(&(CommBufEthData[UDP_SOCKET_BUFFER_NTH]));
				GetDataEthDataRxBuf(&(CommBufEthData[UDP_SOCKET_BUFFER_NTH]), (HUGEDATA char *)&buf, len);
#else
				len = recvfrom(s, buf, len, destip, &destport);  // receive data from a destination
#endif
#ifdef _LATRONIX_DEBUG_
				{
				INT8U i;
				sprintf(MsgBuf, "rcv len=%d(SET T0)\r\n", len);
				MsgOut(MsgBuf);
				for (i = 0; i < len; i++)
				{
					sprintf(MsgBuf, "%02X ", buf[i]);
					MsgOut(MsgBuf);
					if ((i%16) == 15)
					{
						sprintf(MsgBuf, "\r\n");
						MsgOut(MsgBuf);
					}
				}
				sprintf(MsgBuf, "(T0)\r\n");
				MsgOut(MsgBuf);
				}
#endif
				if (buf[3] == 0xb0 && len == 4)
				{
					return ON;
				}
			}
		}
	}
	return OFF;
}

#ifdef ETH_JIG_PGM3
INT8U wlan_CheckIP_Tobnet(INT8U *ip, INT8U *gateway, INT8U *subnet)
{
	INT16U send_time;
	INT8U buf[256];
	INT16U len;
	SOCKET s;
	INT8U destip[4];
	INT16U destport;
	WLAN_SETUP_RECORD0_TOBNET	record0;
#ifdef CPU_PIC32MZ
	s = appData.UdpClientSocket;
#else
	s = 5;	// UDP
#endif

	send_UDP_GetSetupRecord(s, 0, OFF, WLAN_CONTROL_PORT_TOBNET);
	send_time = SysTimer100ms;

	while ((len=getSn_RX_RSR(s)) < 124)                   // check the size of received data
	{
		if ((INT16U)(SysTimer100ms - send_time) > 20)  
		{
#ifdef _LATRONIX_DEBUG_
			sprintf(MsgBuf, "rcv timeout(T0)\r\n");
			MsgOut(MsgBuf);
#endif
			break;
		}
	}
	if (len)
	{
		if (len > 256) len = 256;
#ifdef CPU_PIC32MZ
		//len = recvfrom(s, (char*)buf, len, 0, (struct sockaddr *)&clientaddr, &ulen);
		len = CheckRxBufCountInt(&(CommBufEthData[UDP_SOCKET_BUFFER_NTH]));
		GetDataEthDataRxBuf(&(CommBufEthData[UDP_SOCKET_BUFFER_NTH]), (HUGEDATA char *)&buf, len);
#else
		len = recvfrom(s,buf,len,destip,&destport);  // receive data from a destination
#endif

#ifdef _LATRONIX_DEBUG_
		{
		INT8U i;
		sprintf(MsgBuf, "rcv len=%d(T0)\r\n", len);
		MsgOut(MsgBuf);
		for (i = 0; i < len; i++)
		{
			sprintf(MsgBuf, "%02X ", buf[i + 4]);
			MsgOut(MsgBuf);
			if ((i%16) == 15)
			{
				sprintf(MsgBuf, "\r\n");
				MsgOut(MsgBuf);
			}
		}
		sprintf(MsgBuf, "(T0)\r\n");
		MsgOut(MsgBuf);
		}
#endif
		if (buf[3] == 0xd0 && len == 124)
		{
			memcpy(&record0, &buf[4], sizeof(WLAN_SETUP_RECORD0_TOBNET));
			if (record0.IP[0] != ip[0] || record0.IP[1] != ip[1] || record0.IP[2] != ip[2] || record0.IP[3] != ip[3]
				|| record0.gatewayIP[0] != gateway[0] || record0.gatewayIP[1] != gateway[1] || record0.gatewayIP[2] != gateway[2] || record0.gatewayIP[3] != gateway[3]
				|| record0.subnet[0] != subnet[0] || record0.subnet[1] != subnet[1] || record0.subnet[2] != subnet[2] || record0.subnet[3] != subnet[3])
			{
				return OFF;
			}
			return ON;
		}
	}
	return OFF;
}
#endif

/*
INT8U wlan_SetAESKey_Lantronix(INT8U keyLength, INT8U *aesKey)
{
	INT16U send_time;
	INT8U buf[256];
	INT16U len;
	SOCKET s;
	INT8U destip[4];
	INT16U destport;
	WLAN_SETUP_RECORD1	record1;
	INT16U max_keylen;
	INT8U j, k;
#ifdef CPU_PIC32MZ
	s = appData.UdpClientSocket;
#else
	s = 5;	// UDP
#endif

	send_UDP_GetSetupRecord(s, 1, OFF, WLAN_CONTROL_PORT_LANTRONIX);
	send_time = SysTimer100ms;

	while ((len=getSn_RX_RSR(s)) == 0)                   // check the size of received data
	{
		if (SysTimer100ms - send_time > 20) break;
	}
	if (len)
	{
		if (len > 256) len = 256;
#ifdef CPU_PIC32MZ
		//len = recvfrom(s, (char*)buf, len, 0, (struct sockaddr *)&clientaddr, &ulen);
		len = CheckRxBufCountInt(&(CommBufEthData[UDP_SOCKET_BUFFER_NTH]));
		GetDataEthDataRxBuf(&(CommBufEthData[UDP_SOCKET_BUFFER_NTH]), (HUGEDATA char *)&buf, len);
#else
		len = recvfrom(s,buf,len,destip,&destport);  // receive data from a destination
#endif

		if (buf[3] == 0xd1 && len == 130)
		{
#ifdef _LATRONIX_DEBUG_
			INT8U i;
			for (i = 0; i < sizeof(WLAN_SETUP_RECORD1); i++)
			{
				sprintf(MsgBuf, "%02X ", buf[i + 4]);
				MsgOut(MsgBuf);
				if ((i%16) == 15)
				{
					sprintf(MsgBuf, "\r\n");
					MsgOut(MsgBuf);
				}
			}
			sprintf(MsgBuf, "\r\n");
			MsgOut(MsgBuf);
#endif
			memcpy(&record1, &buf[4], sizeof(WLAN_SETUP_RECORD1));
			record1.AESKeyLength = keyLength;
			memset(&record1.AESKey1, 0, sizeof(record1.AESKey1));
			memset(&record1.AESKey2, 0, sizeof(record1.AESKey2));
			max_keylen = strlen((char*)aesKey);
			if (max_keylen >= 64) 
			{
				max_keylen = 64;
				k = 0;
				for (j = 0; j < 32; )
				{
					record1.AESKey1[k] = ctoh(aesKey[j++]);
					record1.AESKey1[k] <<= 4;
					record1.AESKey1[k] |= ctoh(aesKey[j++]);
					k++;
				}
				for (j = 16; j < max_keylen; )
				{
					record1.AESKey2[k] = ctoh(aesKey[j++]);
					record1.AESKey2[k] <<= 4;
					record1.AESKey2[k] |= ctoh(aesKey[j++]);
					k++;
				}
			}
			else
			{
				memcpy(&record1.AESKey1, &aesKey[0], 16);
				memcpy(&record1.AESKey2, &aesKey[16], 16);
			}

			send_UDP_SetSetupRecord(s, 1, (INT8U *)&record1, OFF, WLAN_CONTROL_PORT_LANTRONIX);

			send_time = SysTimer100ms;
			while ((len=getSn_RX_RSR(s)) == 0)                   // check the size of received data
			{
				if (SysTimer100ms - send_time > 20) break;
			}
			if (len)
			{
				if (len > 256) len = 256;
#ifdef CPU_PIC32MZ
				//len = recvfrom(s, (char*)buf, len, 0, (struct sockaddr *)&clientaddr, &ulen);
				len = CheckRxBufCountInt(&(CommBufEthData[UDP_SOCKET_BUFFER_NTH]));
				GetDataEthDataRxBuf(&(CommBufEthData[UDP_SOCKET_BUFFER_NTH]), (HUGEDATA char *)&buf, len);
#else
				len = recvfrom(s, buf, len, destip, &destport);  // receive data from a destination
#endif
				if (buf[3] == 0xb1 && len == 4)
				{
					return ON;
				}
			}
		}
	}
	return OFF;
}
*/


#ifdef ETH_JIG_PGM3
// speed
//  0 : auto negotiation(default)
//  2 : 10Mbps half-duplex
//  3 : 10Mbps full-duplex
//  4 : 100Mbps half-duplex
//  5 : 100Mbps full-duplex
INT8U wlan_SetEthernetSpeed_Lantronix(INT8U speed)	//only for ethernet jig
{
	INT16U send_time;
	INT8U buf[256];
	INT16U len;
	SOCKET s;
	INT8U destip[4];
	INT16U destport;
	WLAN_SETUP_RECORD3	record3;
#ifdef CPU_PIC32MZ
	s = appData.UdpClientSocket;
#else
	s = 5;	// UDP
#endif

	send_UDP_GetSetupRecord(s, 3, OFF, WLAN_CONTROL_PORT_LANTRONIX);
	send_time = SysTimer100ms;

	while ((len=getSn_RX_RSR(s)) < 130)                   // check the size of received data
	{
		if ((INT16U)(SysTimer100ms - send_time) > 20) break;
	}
	if (len)
	{
		if (len > 256) len = 256;
#ifdef CPU_PIC32MZ
		//len = recvfrom(s, (char*)buf, len, 0, (struct sockaddr *)&clientaddr, &ulen);
		len = CheckRxBufCountInt(&(CommBufEthData[UDP_SOCKET_BUFFER_NTH]));
		GetDataEthDataRxBuf(&(CommBufEthData[UDP_SOCKET_BUFFER_NTH]), (HUGEDATA char *)&buf, len);
#else
		len = recvfrom(s,buf,len,destip,&destport);  // receive data from a destination
#endif

		if (buf[3] == 0xd3 && len == 130)
		{
#ifdef _LATRONIX_DEBUG_
			INT8U i;
			for (i = 0; i < sizeof(WLAN_SETUP_RECORD3); i++)
			{
				sprintf(MsgBuf, "%02X ", buf[i + 4]);
				MsgOut(MsgBuf);
				if ((i%16) == 15)
				{
					sprintf(MsgBuf, "\r\n");
					MsgOut(MsgBuf);
				}
			}
			sprintf(MsgBuf, "(L3)\r\n");
			MsgOut(MsgBuf);
#endif
			memcpy(&record3, &buf[4], sizeof(WLAN_SETUP_RECORD3));
			record3.speedNDuplex = speed;
			send_UDP_SetSetupRecord(s, 3, (INT8U *)&record3, OFF, WLAN_CONTROL_PORT_LANTRONIX);

			send_time = SysTimer100ms;
			while ((len=getSn_RX_RSR(s)) < 4)                   // check the size of received data
			{
				if ((INT16U)(SysTimer100ms - send_time) > 20) break;
			}
			if (len)
			{
				if (len > 256) len = 256;
#ifdef CPU_PIC32MZ
				//len = recvfrom(s, (char*)buf, len, 0, (struct sockaddr *)&clientaddr, &ulen);
				len = CheckRxBufCountInt(&(CommBufEthData[UDP_SOCKET_BUFFER_NTH]));
				GetDataEthDataRxBuf(&(CommBufEthData[UDP_SOCKET_BUFFER_NTH]), (HUGEDATA char *)&buf, len);
#else
				len = recvfrom(s, buf, len, destip, &destport);  // receive data from a destination
#endif
				if (buf[3] == 0xb3 && len == 4)
				{
					return ON;
				}
#ifdef _LATRONIX_DEBUG_
				else
				{
					INT8U i;
					for (i = 0; i < len; i++)
					{
						sprintf(MsgBuf, "%02X ", buf[i]);
						MsgOut(MsgBuf);
						if ((i%16) == 15)
						{
							sprintf(MsgBuf, "\r\n");
							MsgOut(MsgBuf);
						}
					}
					sprintf(MsgBuf, "(R3)\r\n");
					MsgOut(MsgBuf);
				}
#endif
			}
		}
	}
	return OFF;
}
#endif

/*
CH1  2.412GHz  (2.400GHz~2.424GHz)
CH2  2.417GHz
CH3  2.422GHz
CH4  2.427GHz
CH5  2.432GHz
CH6  2.437GHz
CH7  2.442GHz
CH8  2.447GHz
CH9  2.452GHz
CH10 2,457GHz
CH11 2.462GHz (2.450 ~ 2.474)
CH12 2.467GHz (2.455 ~ 2.479)
CH13 2.472GHz (2.460 ~ 2.484)
CH14 2.484GHz (2.472 ~ 2.496)
---------------------------------------------------
US, CANADA : 2.412 ~ 2.462 (11 cannel)
EU, KOREA : 2.412 ~ 2.472 (13 cannel)
SPAIN : 2.457 ~ 2.462 (2 cannel)
FRANCE : 2.457 ~ 2.472 (4 cannel)
JAPAN : 2.412 ~ 2.484 (14 cannel)
*/

INT8U wlan_SetCountry_Lantronix(void)	//only for ethernet jig
{
	INT16U send_time;
	INT8U buf[256];
	INT16U len;
	SOCKET s;
	INT8U destip[4];
	INT16U destport;
	WLAN_SETUP_RECORD4	record4;
#ifdef CPU_PIC32MZ
	s = appData.UdpClientSocket;
#else
	s = 5;	// UDP
#endif

	send_UDP_GetSetupRecord(s, 4, OFF, WLAN_CONTROL_PORT_LANTRONIX);
	send_time = SysTimer100ms;

	while ((len=getSn_RX_RSR(s)) < 130)                   // check the size of received data
	{
		if ((INT16U)(SysTimer100ms - send_time) > 20) break;
	}
	if (len)
	{
		if (len > 256) len = 256;
#ifdef CPU_PIC32MZ
		//len = recvfrom(s, (char*)buf, len, 0, (struct sockaddr *)&clientaddr, &ulen);
		len = CheckRxBufCountInt(&(CommBufEthData[UDP_SOCKET_BUFFER_NTH]));
		GetDataEthDataRxBuf(&(CommBufEthData[UDP_SOCKET_BUFFER_NTH]), (HUGEDATA char *)&buf, len);
#else
		len = recvfrom(s,buf,len,destip,&destport);  // receive data from a destination
#endif

		if (buf[3] == 0xd4 && len == 130)
		{
#ifdef _LATRONIX_DEBUG_
			INT8U i;
			for (i = 0; i < sizeof(WLAN_SETUP_RECORD4); i++)
			{
				sprintf(MsgBuf, "%02X ", buf[i + 4]);
				MsgOut(MsgBuf);
				if ((i%16) == 15)
				{
					sprintf(MsgBuf, "\r\n");
					MsgOut(MsgBuf);
				}
			}
			sprintf(MsgBuf, "(L4)\r\n");
			MsgOut(MsgBuf);
#endif
			memcpy(&record4, &buf[4], sizeof(WLAN_SETUP_RECORD4));
			record4.country = 3;	// 0=US, 1=FR, 2=JP, 3:OT(hers), 4=SP, 5=CA
			send_UDP_SetSetupRecord(s, 4, (INT8U *)&record4, OFF, WLAN_CONTROL_PORT_LANTRONIX);

			send_time = SysTimer100ms;
			while ((len=getSn_RX_RSR(s)) < 4)                   // check the size of received data
			{
				if ((INT16U)(SysTimer100ms - send_time) > 20) break;
			}
			if (len)
			{
				if (len > 256) len = 256;
#ifdef CPU_PIC32MZ
				//len = recvfrom(s, (char*)buf, len, 0, (struct sockaddr *)&clientaddr, &ulen);
				len = CheckRxBufCountInt(&(CommBufEthData[UDP_SOCKET_BUFFER_NTH]));
				GetDataEthDataRxBuf(&(CommBufEthData[UDP_SOCKET_BUFFER_NTH]), (HUGEDATA char *)&buf, len);
#else
				len = recvfrom(s, buf, len, destip, &destport);  // receive data from a destination
#endif
				if (buf[3] == 0xb4 && len == 4)
				{
					return ON;
				}
#ifdef _LATRONIX_DEBUG_
				else
				{
					INT8U i;
					for (i = 0; i < len; i++)
					{
						sprintf(MsgBuf, "%02X ", buf[i]);
						MsgOut(MsgBuf);
						if ((i%16) == 15)
						{
							sprintf(MsgBuf, "\r\n");
							MsgOut(MsgBuf);
						}
					}
					sprintf(MsgBuf, "(R4)\r\n");
					MsgOut(MsgBuf);
				}
#endif
			}
		}
	}
	return OFF;
}

INT8U wlan_SetWiFiSetting_Lantronix(INT8U *ssid, INT8U wep, INT8U keyIdx, INT8U keyLen, INT8U opmode)	// wep - 0:none, 1:wep64, 2:wep128
{
	INT16U send_time;
	INT8U buf[256];
	INT16U len;
	SOCKET s;
	INT8U destip[4];
	INT16U destport;
	INT8U security, authenticationMethod, encryptionMethod, keyType;
	INT8U groupEncryptionMethod;
	WLAN_SETUP_RECORD8	record8;
	INT8U ssid_len;
#ifdef CPU_PIC32MZ
	s = appData.UdpClientSocket;
#else
	s = 5;	// UDP
#endif


	switch (wep)
	{
		default:
		case 0:	// none
			security = 0;
			authenticationMethod = 0;	// 0=none
			encryptionMethod = 0;
			groupEncryptionMethod = 0;	// 0=none
			keyType = 0;			// 0=hex
			break;
		case 1:	// wep64
			security = 1;
			authenticationMethod = 0;	// 0=none
			encryptionMethod = 1;		// 1=wep64
			groupEncryptionMethod = 0;	// 0=none
			keyType = 0;			// 0=hex
			break;
		case 2:	// wep128
			security = 1;
			authenticationMethod = 0;	// 0=none
			encryptionMethod = 2;		// 2=wep128
			groupEncryptionMethod = 0;	// 0=none
			keyType = 0;			// 0=hex
			break;
		case 3: // tkip(WPA)
			security = 2;			// 2=wpa
			authenticationMethod = 1;	// 1=Shared/PSK
			encryptionMethod = 3;		// 3=tkip
			groupEncryptionMethod = 3;	// 3=tkip
			keyType = 1;			// 1=passphrase
			if(keyLen >= 64) keyType = 0;	// 0=hex
			break;
		case 4: // tkip(WPA2)
			security = 3;			// 3=802.11i/wpa2
			authenticationMethod = 1;	// 1=Shared/PSK
			encryptionMethod = 3;		// 3=tkip
			groupEncryptionMethod = 3;	// 3=tkip
			keyType = 1;			// 1=passphrase
			if(keyLen >= 64) keyType = 0;	// 0=hex
			break;
		case 5: // AES(WPA2)
			security = 3;			// 3=802.11i/wpa2
			authenticationMethod = 1;	// 1=Shared/PSK
			encryptionMethod = 4;		// 4=CCMP
			groupEncryptionMethod = 4;	// 4=CCMP
			keyType = 1;			// 1=passphrase
			if(keyLen >= 64) keyType = 0;	// 0=hex
			break;
	}
	
	send_UDP_GetSetupRecord(s, 8, OFF, WLAN_CONTROL_PORT_LANTRONIX);
	send_time = SysTimer100ms;

	while ((len=getSn_RX_RSR(s)) < 130)                   // check the size of received data
	{
		if ((INT16U)(SysTimer100ms - send_time) > 20) break;
	}
	if (len)
	{
		if (len > 256) len = 256;
#ifdef CPU_PIC32MZ
		//len = recvfrom(s, (char*)buf, len, 0, (struct sockaddr *)&clientaddr, &ulen);
		len = CheckRxBufCountInt(&(CommBufEthData[UDP_SOCKET_BUFFER_NTH]));
		GetDataEthDataRxBuf(&(CommBufEthData[UDP_SOCKET_BUFFER_NTH]), (HUGEDATA char *)&buf, len);
#else
		len = recvfrom(s,buf,len,destip,&destport);  // receive data from a destination
#endif

		if (buf[3] == 0xd8 && len == 130)
		{
#ifdef _LATRONIX_DEBUG_
			INT8U i;
			for (i = 0; i < sizeof(WLAN_SETUP_RECORD8); i++)
			{
				sprintf(MsgBuf, "%02X ", buf[i + 4]);
				MsgOut(MsgBuf);
				if ((i%16) == 15)
				{
					sprintf(MsgBuf, "\r\n");
					MsgOut(MsgBuf);
				}
			}
			sprintf(MsgBuf, "(L8)\r\n");
			MsgOut(MsgBuf);
#endif
			memcpy(&record8, &buf[4], sizeof(WLAN_SETUP_RECORD8));
			memset(record8.SSID, 0, sizeof(record8.SSID));
			//ssid_len = ssid[32];
			ssid_len = strlen((char*)ssid);
			if (ssid_len > 32) ssid_len = 32;
			memcpy(record8.SSID, ssid, ssid_len);
			record8.wifiSettings = 0x7f;	// pw man. no, auto tx data rate, tx data rate 54Mbps, tx pw lev. 18dBm, Wifi enable
			record8.adHocSettings = 0xb0;	// ch:11, use infrastructure
			if(!opmode) record8.adHocSettings = 0xb1;	// ch:11, use Ad hoc
			//record8.securitySettings &= 0x1c;
			record8.securitySettings = security;	// 1:WEP
			record8.securitySettings |= (authenticationMethod<<2);
			record8.securitySettings |= (encryptionMethod<<5);
			record8.keyLength = keyLen;
			record8.groupEncryptionMethod = groupEncryptionMethod;
			record8.keyType = keyType;	// 0:hex, 1=passphrase
			record8.txKeyIndex = keyIdx - 1;
			record8.roaming = 0;
			send_UDP_SetSetupRecord(s, 8, (INT8U *)&record8, OFF, WLAN_CONTROL_PORT_LANTRONIX);

			send_time = SysTimer100ms;
			while ((len=getSn_RX_RSR(s)) < 4)                   // check the size of received data
			{
				if ((INT16U)(SysTimer100ms - send_time) > 20) break;
			}
			if (len)
			{
				if (len > 256) len = 256;
#ifdef CPU_PIC32MZ
				//len = recvfrom(s, (char*)buf, len, 0, (struct sockaddr *)&clientaddr, &ulen);
				len = CheckRxBufCountInt(&(CommBufEthData[UDP_SOCKET_BUFFER_NTH]));
				GetDataEthDataRxBuf(&(CommBufEthData[UDP_SOCKET_BUFFER_NTH]), (HUGEDATA char *)&buf, len);
#else
				len = recvfrom(s, buf, len, destip, &destport);  // receive data from a destination
#endif
				if (buf[3] == 0xb8 && len == 4)
				{
					return ON;
				}
#ifdef _LATRONIX_DEBUG_
				else
				{
					INT8U i;
					for (i = 0; i < len; i++)
					{
						sprintf(MsgBuf, "%02X ", buf[i]);
						MsgOut(MsgBuf);
						if ((i%16) == 15)
						{
							sprintf(MsgBuf, "\r\n");
							MsgOut(MsgBuf);
						}
					}
					sprintf(MsgBuf, "(R8)\r\n");
					MsgOut(MsgBuf);
				}
#endif
			}
		}
	}
	return OFF;
}

INT8U wlan_SetWiFiSetting_Tobnet(INT8U *ssid, INT8U wep, INT8U keyIdx, INT8U keyLen, INT8U *key, INT8U opmode)
{
	INT16U send_time;
	INT8U buf[256];
	INT16U len;
	SOCKET s;
	INT8U destip[4];
	INT16U destport;
	INT8U security, authenticationMethod, cipherType, keyType;
	WLAN_SETUP_RECORD8_TOBNET	record8;
	INT8U ssid_len;
#ifdef CPU_PIC32MZ
	s = appData.UdpClientSocket;
#else
	s = 5;	// UDP
#endif

	switch (wep)
	{
		default:
		case 0:	// none
			security = 0;			// 0x00=none, 0x01=WEP64, 0x02=WEP128, 0x03=WPA-PSK, 0x04=WPA-EAP
			authenticationMethod = 0;	// 0x00=none, 0x01=Open, 0x02=Shared, 0x03=Auto, 0x04=WPA, 0x05=WPA2, 0x06=WPA(Auto)
			cipherType = 0;			// 0x00=TKIP, 0x01=AES, 0x02=Auto
			keyType = 1;			// 0x00=hex, 0x01=ASCII
			break;
		case 1:	// wep64
			security = 1;			// 0x01=WEP64
			authenticationMethod = 3;	// 0x03=Auto
			cipherType = 2;			// 0x02=Auto
			keyType = 1;			// 0x01=ASCII
			break;
		case 2:	// wep128
			security = 2;			// 0x02=WEP128
			authenticationMethod = 3;	// 0x03=Auto
			cipherType = 2;			// 0x02=Auto
			keyType = 1;			// 0x01=ASCII
			break;
		case 3: // tkip(WPA)
			security = 3;			// 0x03=WPA-PSK
			authenticationMethod = 4;	// 0x04=WPA
			cipherType = 0;			// 0x00=TKIP
			keyType = 1;			// 0x01=ASCII
			if(keyLen >= 64) keyType = 0;	// 0x00=hex
			break;
		case 4: // tkip(WPA2)
			security = 3;			// 0x03=WPA-PSK
			authenticationMethod = 5;	// 0x05=WPA2
			cipherType = 0;			// 0x00=TKIP
			keyType = 1;			// 0x01=ASCII
			if(keyLen >= 64) keyType = 0;	// 0x00=hex
			break;
		case 5: // AES(WPA2)
			security = 3;			// 0x03=WPA-PSK
			authenticationMethod = 5;	// 0x05=WPA2
			cipherType = 1;			// 0x01=AES, 0x02=Auto
			keyType = 1;			// 0x01=ASCII
			if(keyLen >= 64) keyType = 0;	// 0x00=hex
			break;
	}
	
	send_UDP_GetSetupRecord(s, 8, OFF, WLAN_CONTROL_PORT_TOBNET);
#ifdef _LATRONIX_DEBUG_
	sprintf(MsgBuf, "send GET T8\r\n");
	MsgOut(MsgBuf);
#endif
	send_time = SysTimer100ms;

	while ((len=getSn_RX_RSR(s)) < 130)                   // check the size of received data
	{
		if ((INT16U)(SysTimer100ms - send_time) > 20) 
		{
#ifdef _LATRONIX_DEBUG_
			sprintf(MsgBuf, "rcv timeout(T8)\r\n");
			MsgOut(MsgBuf);
#endif
			break;
		}
	}
	if (len)
	{
		if (len > 256) len = 256;
#ifdef CPU_PIC32MZ
		//len = recvfrom(s, (char*)buf, len, 0, (struct sockaddr *)&clientaddr, &ulen);
		len = CheckRxBufCountInt(&(CommBufEthData[UDP_SOCKET_BUFFER_NTH]));
		GetDataEthDataRxBuf(&(CommBufEthData[UDP_SOCKET_BUFFER_NTH]), (HUGEDATA char *)&buf, len);
#else
		len = recvfrom(s,buf,len,destip,&destport);  // receive data from a destination
#endif

#ifdef _LATRONIX_DEBUG_
		{
		INT8U i;
		sprintf(MsgBuf, "rcv len=%d(GET T8)\r\n", len);
		MsgOut(MsgBuf);
		for (i = 0; i < len; i++)
		{
			sprintf(MsgBuf, "%02X ", buf[i]);
			MsgOut(MsgBuf);
			if ((i%16) == 15)
			{
				sprintf(MsgBuf, "\r\n");
				MsgOut(MsgBuf);
			}
		}
		sprintf(MsgBuf, "(T8)\r\n");
		MsgOut(MsgBuf);
		}
#endif
		if (buf[3] == 0xd8 && len == 130)
		{
			memcpy(&record8, &buf[4], sizeof(WLAN_SETUP_RECORD8_TOBNET));
			memset(record8.SSID, 0, sizeof(record8.SSID));
			memset(record8.encryptionKey, 0, sizeof(record8.encryptionKey));
			//ssid_len = ssid[32];
			ssid_len = strlen((char*)ssid);
			if (ssid_len > 32) ssid_len = 32;
			memcpy(record8.SSID, ssid, ssid_len);
			record8.securitySuite = security;
			record8.authenticationMethod = authenticationMethod;
			record8.cipherType = cipherType; 
			record8.keyType = keyType;	// 0:hex, 1=passphrase
			record8.txKeyIndex = keyIdx - 1;
			record8.operationMode = opmode; // 0:Ad-hoc, 1:infrastructure
			record8.channel = 1; // channel 1 fixed
			wlan_convertEncryptionKey(record8.encryptionKey, key);
			send_UDP_SetSetupRecord(s, 8, (INT8U *)&record8, OFF, WLAN_CONTROL_PORT_TOBNET);
#ifdef _LATRONIX_DEBUG_
			sprintf(MsgBuf, "send SET T8\r\n");
			MsgOut(MsgBuf);
#endif

			send_time = SysTimer100ms;
			while ((len=getSn_RX_RSR(s)) < 4)                   // check the size of received data
			{
				if ((INT16U)(SysTimer100ms - send_time) > 20) break;
			}
			if (len)
			{
				if (len > 256) len = 256;
#ifdef CPU_PIC32MZ
				//len = recvfrom(s, (char*)buf, len, 0, (struct sockaddr *)&clientaddr, &ulen);
				len = CheckRxBufCountInt(&(CommBufEthData[UDP_SOCKET_BUFFER_NTH]));
				GetDataEthDataRxBuf(&(CommBufEthData[UDP_SOCKET_BUFFER_NTH]), (HUGEDATA char *)&buf, len);
#else
				len = recvfrom(s, buf, len, destip, &destport);  // receive data from a destination
#endif
#ifdef _LATRONIX_DEBUG_
				{
				INT8U i;
				sprintf(MsgBuf, "rcv len=%d(SET T8)\r\n", len);
				MsgOut(MsgBuf);
				for (i = 0; i < len; i++)
				{
					sprintf(MsgBuf, "%02X ", buf[i]);
					MsgOut(MsgBuf);
					if ((i%16) == 15)
					{
						sprintf(MsgBuf, "\r\n");
						MsgOut(MsgBuf);
					}
				}
				sprintf(MsgBuf, "(T8)\r\n");
				MsgOut(MsgBuf);
				}
#endif
				if (buf[3] == 0xb8 && len == 4)
				{
					return ON;
				}
			}
		}
	}
	return OFF;
}

#ifdef ETH_JIG_PGM3
INT8U wlan_CheckWiFiSetting_Tobnet(INT8U *ssid, INT8U wep, INT8U keyIdx, INT8U keyLen, INT8U *key)
{
	INT16U send_time;
	INT8U buf[256];
	INT16U len;
	SOCKET s;
	INT8U destip[4];
	INT16U destport;
	INT8U security, authenticationMethod, cipherType, keyType;
	WLAN_SETUP_RECORD8_TOBNET	record8;
	INT8U ssid_len;
	INT8U tempKey[65];
	INT8U i;
#ifdef CPU_PIC32MZ
	s = appData.UdpClientSocket;
#else
	s = 5;	// UDP
#endif

	switch (wep)
	{
		default:
		case 0:	// none
			security = 0;			// 0x00=none, 0x01=WEP64, 0x02=WEP128, 0x03=WPA-PSK, 0x04=WPA-EAP
			authenticationMethod = 0;	// 0x00=none, 0x01=Open, 0x02=Shared, 0x03=Auto, 0x04=WPA, 0x05=WPA2, 0x06=WPA(Auto)
			cipherType = 0;			// 0x00=TKIP, 0x01=AES, 0x02=Auto
			keyType = 1;			// 0x00=hex, 0x01=ASCII
			break;
		case 1:	// wep64
			security = 1;			// 0x01=WEP64
			authenticationMethod = 3;	// 0x03=Auto
			cipherType = 2;			// 0x02=Auto
			keyType = 1;			// 0x01=ASCII
			break;
		case 2:	// wep128
			security = 2;			// 0x02=WEP128
			authenticationMethod = 3;	// 0x03=Auto
			cipherType = 2;			// 0x02=Auto
			keyType = 1;			// 0x01=ASCII
			break;
		case 3: // tkip(WPA)
			security = 3;			// 0x03=WPA-PSK
			authenticationMethod = 4;	// 0x04=WPA
			cipherType = 0;			// 0x00=TKIP
			keyType = 1;			// 0x01=ASCII
			if(keyLen >= 64) keyType = 0;	// 0x00=hex
			break;
		case 4: // tkip(WPA2)
			security = 3;			// 0x03=WPA-PSK
			authenticationMethod = 5;	// 0x05=WPA2
			cipherType = 0;			// 0x00=TKIP
			keyType = 1;			// 0x01=ASCII
			if(keyLen >= 64) keyType = 0;	// 0x00=hex
			break;
		case 5: // AES(WPA2)
			security = 3;			// 0x03=WPA-PSK
			authenticationMethod = 5;	// 0x05=WPA2
			cipherType = 1;			// 0x01=AES, 0x02=Auto
			keyType = 1;			// 0x01=ASCII
			if(keyLen >= 64) keyType = 0;	// 0x00=hex
			break;
	}
	
	send_UDP_GetSetupRecord(s, 8, OFF, WLAN_CONTROL_PORT_TOBNET);
	send_time = SysTimer100ms;

	while ((len=getSn_RX_RSR(s)) < 130)                   // check the size of received data
	{
		if ((INT16U)(SysTimer100ms - send_time) > 20) 
		{
#ifdef _LATRONIX_DEBUG_
			sprintf(MsgBuf, "rcv timeout(T8)\r\n");
			MsgOut(MsgBuf);
#endif
			break;
		}
	}
	if (len)
	{
		if (len > 256) len = 256;
#ifdef CPU_PIC32MZ
		//len = recvfrom(s, (char*)buf, len, 0, (struct sockaddr *)&clientaddr, &ulen);
		len = CheckRxBufCountInt(&(CommBufEthData[UDP_SOCKET_BUFFER_NTH]));
		GetDataEthDataRxBuf(&(CommBufEthData[UDP_SOCKET_BUFFER_NTH]), (HUGEDATA char *)&buf, len);
#else
		len = recvfrom(s,buf,len,destip,&destport);  // receive data from a destination
#endif

#ifdef _LATRONIX_DEBUG_
		{
		INT8U i;
		sprintf(MsgBuf, "rcv len=%d(T8)\r\n", len);
		MsgOut(MsgBuf);
		for (i = 0; i < len; i++)
		{
			sprintf(MsgBuf, "%02X ", buf[i + 4]);
			MsgOut(MsgBuf);
			if ((i%16) == 15)
			{
				sprintf(MsgBuf, "\r\n");
				MsgOut(MsgBuf);
			}
		}
		sprintf(MsgBuf, "(T8)\r\n");
		MsgOut(MsgBuf);
		}
#endif
		if (buf[3] == 0xd8 && len == 130)
		{
			memcpy(&record8, &buf[4], sizeof(WLAN_SETUP_RECORD8_TOBNET));

			for (i = 0; i < sizeof(record8.SSID); i++)
			{
				if (record8.SSID[i] == NULL || ssid[i] == NULL) break;
				if (record8.SSID[i] != ssid[i]) return OFF;
			}
			wlan_convertEncryptionKey(tempKey, key);
			for (i = 0; i < sizeof(record8.encryptionKey); i++)
			{
				if (record8.encryptionKey[i] == NULL || tempKey[i] == NULL) break;
				if (record8.encryptionKey[i] != tempKey[i]) return OFF;
			}
			if (record8.securitySuite != security) return OFF;
			if (record8.authenticationMethod != authenticationMethod) return OFF;
			if (record8.cipherType != cipherType) return OFF;
			if (record8.keyType != keyType) return OFF;	// 0:hex, 1=passphrase
			if (record8.txKeyIndex != keyIdx - 1) return OFF;

			return ON;
		}
	}
	return OFF;
}
#endif

INT8U wlan_SetEncriptionKey_Lantronix(INT8U *key)	// wep - 0:none, 1:wep64, 2:wep128
{
	INT16U send_time;
	INT8U buf[256];
	INT16U len;
	SOCKET s;
	INT8U destip[4];
	INT16U destport;
	WLAN_SETUP_RECORD9	record9;
#ifdef CPU_PIC32MZ
	s = appData.UdpClientSocket;
#else
	s = 5;	// UDP
#endif

	send_UDP_GetSetupRecord(s, 9, OFF, WLAN_CONTROL_PORT_LANTRONIX);
	send_time = SysTimer100ms;

	while ((len=getSn_RX_RSR(s)) < 130)                   // check the size of received data
	{
		if ((INT16U)(SysTimer100ms - send_time) > 20) break;
	}
	if (len)
	{
		if (len > 256) len = 256;
#ifdef CPU_PIC32MZ
		//len = recvfrom(s, (char*)buf, len, 0, (struct sockaddr *)&clientaddr, &ulen);
		len = CheckRxBufCountInt(&(CommBufEthData[UDP_SOCKET_BUFFER_NTH]));
		GetDataEthDataRxBuf(&(CommBufEthData[UDP_SOCKET_BUFFER_NTH]), (HUGEDATA char *)&buf, len);
#else
		len = recvfrom(s,buf,len,destip,&destport);  // receive data from a destination
#endif

		if (buf[3] == 0xd9 && len == 130)
		{
#ifdef _LATRONIX_DEBUG_
			INT8U i;
			for (i = 0; i < sizeof(WLAN_SETUP_RECORD0); i++)
			{
				sprintf(MsgBuf, "%02X ", buf[i + 4]);
				MsgOut(MsgBuf);
				if ((i%16) == 15)
				{
					sprintf(MsgBuf, "\r\n");
					MsgOut(MsgBuf);
				}
			}
			sprintf(MsgBuf, "(L9)\r\n");
			MsgOut(MsgBuf);
#endif
			memcpy(&record9, &buf[4], sizeof(WLAN_SETUP_RECORD9));
			memset(&record9.EncryptionKey, 0, sizeof(record9.EncryptionKey));
			wlan_convertEncryptionKey(record9.EncryptionKey, key);
			/*
			max_keylen = strlen((char*)key);
			if (max_keylen >= 64) 
			{
				max_keylen = 64;
				k = 0;
				for (j = 0; j < max_keylen; )
				{
					record9.EncryptionKey[k] = ctoh(key[j++]);
					record9.EncryptionKey[k] <<= 4;
					record9.EncryptionKey[k] |= ctoh(key[j++]);
					k++;
				}
			}
			else
			{
				memcpy(&record9.EncryptionKey, key, max_keylen);
			}
			*/
			send_UDP_SetSetupRecord(s, 9, (INT8U *)&record9, OFF, WLAN_CONTROL_PORT_LANTRONIX);

			send_time = SysTimer100ms;
			while ((len=getSn_RX_RSR(s)) < 4)                   // check the size of received data
			{
				if ((INT16U)(SysTimer100ms - send_time) > 20) break;
			}
			if (len)
			{
				if (len > 256) len = 256;
#ifdef CPU_PIC32MZ
				//len = recvfrom(s, (char*)buf, len, 0, (struct sockaddr *)&clientaddr, &ulen);
				len = CheckRxBufCountInt(&(CommBufEthData[UDP_SOCKET_BUFFER_NTH]));
				GetDataEthDataRxBuf(&(CommBufEthData[UDP_SOCKET_BUFFER_NTH]), (HUGEDATA char *)&buf, len);
#else
				len = recvfrom(s, buf, len, destip, &destport);  // receive data from a destination
#endif
				if (buf[3] == 0xb9 && len == 4)
				{
					return ON;
				}
#ifdef _LATRONIX_DEBUG_
				else
				{
					INT8U i;
					for (i = 0; i < len; i++)
					{
						sprintf(MsgBuf, "%02X ", buf[i]);
						MsgOut(MsgBuf);
						if ((i%16) == 15)
						{
							sprintf(MsgBuf, "\r\n");
							MsgOut(MsgBuf);
						}
					}
					sprintf(MsgBuf, "(R9)\r\n");
					MsgOut(MsgBuf);
				}
#endif
			}
		}
	}
	return OFF;
}

INT8U wlan_CheckVer_Bridge(INT8U wtype, INT8U *ver_str, INT8U retry)
{
	INT16U send_time;
	INT8U buf[512];
	INT16U len;
	SOCKET s;
	INT8U destip[4];
	INT16U destport;
	INT16U controlPort;
#ifdef CPU_PIC32MZ
	s = appData.UdpClientSocket;
#else
	s = 5;	// UDP
#endif

	if (retry == 0) retry = 3;

	if (wtype == WLTYPE_LANTRONIX)
		controlPort = WLAN_CONTROL_PORT_LANTRONIX;
	else if (wtype == WLTYPE_TOBNET)
		controlPort = WLAN_CONTROL_PORT_TOBNET; 
	else	// WLTYPE_NONE이면 번갈아 가며 체크
	{
		controlPort = WLAN_CONTROL_PORT_TOBNET;
		retry *= 2;	// 
	}

	while(retry--)
	{
		send_UDP_GetExtVersion(s, controlPort);
#ifdef _LATRONIX_DEBUG_
		sprintf(MsgBuf, "send GET VER\r\n");
		MsgOut(MsgBuf);
#endif
		send_time = SysTimer100ms;

		while((len=getSn_RX_RSR(s)) == 0)                   // check the size of received data
		{
#ifdef CPU_PIC32MZ
			if ((INT16U)(SysTimer100ms - send_time) > 5) break;//TODO: check time
#else
			if ((INT16U)(SysTimer100ms - send_time) > 2) break;
#endif
		}

		if (len)
		{
			if (len > 256) len = 256;
#ifdef CPU_PIC32MZ
			//len = recvfrom(s, (char*)buf, len, 0, (struct sockaddr *)&clientaddr, &ulen);
			len = CheckRxBufCountInt(&(CommBufEthData[UDP_SOCKET_BUFFER_NTH]));
			GetDataEthDataRxBuf(&(CommBufEthData[UDP_SOCKET_BUFFER_NTH]), (HUGEDATA char *)&buf, len);
#else
			len = recvfrom(s, buf, len, destip, &destport);  // receive data from a destination
#endif
			if (buf[3] == 0xf5)
			{
				memcpy(ver_str, &buf[4+12], 16);
#ifdef _LATRONIX_DEBUG_
				sprintf(MsgBuf, "VER=%s, (%d)\r\n", ver_str, wtype);
				MsgOut(MsgBuf);
#endif
				return ON;
			}
		}
#ifdef _LATRONIX_DEBUG_
		sprintf(MsgBuf, "VER=[FAIL](port=0x%4x,retry=%d)\r\n", controlPort, retry);
		MsgOut(MsgBuf);
#endif
		if (wtype == WLTYPE_NONE)
		{
			if (controlPort == WLAN_CONTROL_PORT_TOBNET)
				controlPort = WLAN_CONTROL_PORT_LANTRONIX;
			else
				controlPort = WLAN_CONTROL_PORT_TOBNET;
		}
	}
	return OFF;
}

ap_info *wlan_Check_ConnInfo_Bridge(void)
{
	INT16U send_time;
	INT8U buf[512];
	INT16U len;
	SOCKET s;
	INT8U destip[4];
	INT16U destport;
	INT16U controlPort;
	INT8U retry;
	INT16U timeout;

#ifdef CPU_PIC32MZ
	s = appData.UdpClientSocket;
#else
	s = 5;	// UDP
#endif
	retry = 2;
	controlPort = WLAN_CONTROL_PORT_TOBNET; 
	
	if (Startup.wireless_model == WLMODEL_HIFLYING)
		timeout = 90;
	else
		timeout = 2;

	while(retry--)
	{
		send_UDP_GetConInfo(s, controlPort);
		send_time = SysTimer100ms;

		while((len=getSn_RX_RSR(s)) < 130)                   // check the size of received data
		{
			if ((INT16U)(SysTimer100ms - send_time) > timeout) break;
			if (KEY_IsKey()) 
			{
				return &conn_ap_info;
			}
		}

#ifdef _LATRONIX_DEBUG_
		sprintf(MsgBuf, "len=%d\r\n", len);
		MsgOut(MsgBuf);
#endif
		memset(&conn_ap_info, 0, sizeof(conn_ap_info));
		if (len)
		{
			if (len > 256) len = 256;	
#ifdef CPU_PIC32MZ
			//len = recvfrom(s, (char*)buf, len, 0, (struct sockaddr *)&clientaddr, &ulen);
			len = CheckRxBufCountInt(&(CommBufEthData[UDP_SOCKET_BUFFER_NTH]));
			GetDataEthDataRxBuf(&(CommBufEthData[UDP_SOCKET_BUFFER_NTH]), (HUGEDATA char *)&buf, len);
#else
			len = recvfrom(s, buf, len, destip, &destport);  // receive data from a destination
#endif

#ifdef _LATRONIX_DEBUG_
			{
			INT8U i;
			sprintf(MsgBuf, "rcv len=%d(GET INFO)\r\n", len);
			MsgOut(MsgBuf);
			for (i = 0; i < len; i++)
			{
				sprintf(MsgBuf, "%02X ", buf[i]);
				MsgOut(MsgBuf);
				if ((i%16) == 15)
				{
					sprintf(MsgBuf, "\r\n");
					MsgOut(MsgBuf);
				}
			}
			sprintf(MsgBuf, "(INFO)\r\n");
			MsgOut(MsgBuf);
			}
#endif
			if (buf[3] == 0xf2)
			{
#ifdef _LATRONIX_DEBUG_
				sprintf(MsgBuf, "(SUCCESS)\r\n");
				MsgOut(MsgBuf);
#endif
				memcpy(&conn_ap_info, &buf[4], sizeof(conn_ap_info));
				retry = 0;
			}
		}
	}
	return &conn_ap_info;
}

void wlan_RebootModule(INT8U wtype)
{
    send_UDP_NodeReset(5, wtype);	// UDP
}

void reverse_ip(INT8U *ip)
{
	INT8U temp[4];
	temp[0] = ip[0];
	temp[1] = ip[1];
	temp[2] = ip[2];
	temp[3] = ip[3];
	ip[0] = temp[3];
	ip[1] = temp[2];
	ip[2] = temp[1];
	ip[3] = temp[0];
}

void wlan_SetNetwork(INT8U *ip, INT8U *gw, INT8U *sn)
{
	INT8U temp_ip[4];
	INT8U temp_gw[4];
	INT8U temp_sn[4];
	memcpy(temp_ip, ip, 4);
	memcpy(temp_gw, gw, 4);
	memcpy(temp_sn, sn, 4);
	reverse_ip(temp_ip);
	reverse_ip(temp_gw);
	reverse_ip(temp_sn);
	Ethernet_SetNetwork(temp_ip, temp_gw, temp_sn);
}
