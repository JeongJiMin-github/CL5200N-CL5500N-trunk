
#include <stdio.h>
#include "globals.h"
#include "commbuf.h"
#include "main.h"
#include "ethernet.h"
#include "tcpip/src/tcpip_private.h"
#include "app.h"

INT16U unknownSockCnt = 0;	// for Debug 090515
INT8U unknownSockState = 0;
INT16U debugSockCnt = 0;	// for Debug 090515
INT8U debugSockState = 0;

extern struct BSDSocket  * BSDSocketArray;
extern APP_DATA appData;

INT8U Ethernet_Command(INT8U s, INT8U cmd_type)
{
    static struct sockaddr_in tcaddr_backup;

    if (s == 0) {//client s = 0
        s = appData.bsdClientSocket;
    } else if (s > 0 && s < 5) {//server s = 1 ~ 4
        s = appData.ClientSock[s-1];
    }

   switch (cmd_type) {
      case TCP_STATE_REQ_COMMAND:
           switch(BSDSocketArray[s].bsdState)
           {
               case SKT_CLOSED:// Socket closed state indicating a free descriptor
                        return CLOSE;
               case SKT_CREATED:// Socket created state for TCP and UDP sockets
               case SKT_BOUND:// Socket bound state for TCP and UDP sockets
                        return CLOSE;//UNKNOWN;
               case SKT_BSD_LISTEN:// Listening state for TCP BSD listener handle "socket"
                        return LISTEN;
               case SKT_LISTEN:// TCP server listen state
                        return LISTEN;
               case SKT_IN_PROGRESS:// TCP client connection in progress state
                        return SYNSENT;
               case SKT_EST:// TCP client or server established state
                        return ESTABED;
               case SKT_DISCONNECTED:// TCP client or server no longer connected to the remote host (but was historically)
                        return CLOSE;
           }
	 break;
      case TCP_OPEN_COMMAND:
      	 //only client
            appData.tcaddr.sin_addr.S_un.S_un_b.s_b1 = EthCardStruct.TCPDestIP[3];
            appData.tcaddr.sin_addr.S_un.S_un_b.s_b2 = EthCardStruct.TCPDestIP[2];
            appData.tcaddr.sin_addr.S_un.S_un_b.s_b3 = EthCardStruct.TCPDestIP[1];
            appData.tcaddr.sin_addr.S_un.S_un_b.s_b4 = EthCardStruct.TCPDestIP[0];
            appData.tcaddr.sin_port = EthCardStruct.TCPDestPort;

            if (tcaddr_backup.sin_addr.S_un.S_un_b.s_b1 == 0x00)
            {
                memcpy(&tcaddr_backup, &appData.tcaddr, sizeof(tcaddr_backup));
            }
            
            if (memcmp(&tcaddr_backup, &appData.tcaddr, sizeof(tcaddr_backup)) != 0)
            {
                memcpy(&tcaddr_backup, &appData.tcaddr, sizeof(tcaddr_backup));
                appData.TcpClientCloseFlag = true;
                TcpIp_Task();//APP_BSD_OPERATION_TCP_CLIENT
                TcpIp_Task();//APP_BSD_TCP_CLIENT_CLOSE
                TcpIp_Task();//APP_BSD_CREATE_TCP_CLIENT_SOCKET
            }
            
            //tcaddr_backup = appData.tcaddr;
            memcpy(&tcaddr_backup, &appData.tcaddr, sizeof(tcaddr_backup));
            appData.TcpClientConnectFlag = true;
            TcpIp_Task();//APP_TCPIP_WAITING_FOR_COMMAND_CONNECT
            TcpIp_Task();//APP_TCPIP_WAITING_FOR_COMMAND_CONNECT
            TcpIp_Task();//APP_BSD_OPERATION_TCP_CLIENT
            //int addrlen = sizeof(struct sockaddr);
            //connect( appData.bsdClientSocket, (struct sockaddr*)&appData.tcaddr, addrlen);
            break;

      case TCP_CLOSE_COMMAND:
       	 //only client
            appData.TcpClientCloseFlag = true;
            TcpIp_Task();//APP_BSD_OPERATION_TCP_CLIENT
            TcpIp_Task();//APP_BSD_TCP_CLIENT_CLOSE
            TcpIp_Task();//APP_BSD_CREATE_TCP_CLIENT_SOCKET
            //closesocket(appData.bsdClientSocket);
            //int tcpSktC;
            //// Create a socket for this client to connect with
            //if((tcpSktC = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == SOCKET_ERROR )
            //{
            //    return;//for creating socket order tcpclient=0, tcpserver=1/2~5, udpserver=6, udpclient=7
            //}
            //else
            //{
            //    appData.bsdClientSocket = (SOCKET)tcpSktC;
            //}
         break;
   }

   TcpIp_Task();
   return 0;
}

void Ethernet_RestoreNetwork(void)
{
    //Prevent changing same IP 
    if(appData.tempIpAddress.v[3] == EthCardStruct.IP[0] \
        && appData.tempIpAddress.v[2] == EthCardStruct.IP[1] \
        && appData.tempIpAddress.v[1] == EthCardStruct.IP[2] \
        && appData.tempIpAddress.v[0] == EthCardStruct.IP[3]) return;
    
    closesocket(appData.bsdServerSocket);
    closesocket(appData.bsdClientSocket);
    closesocket(appData.UdpServerSocket);
    closesocket(appData.UdpClientSocket);
    APP_Initialize ();
    INT16U send_time;
    send_time = SysTimer100ms;
    while (1)                   // check the size of received data
    {
        TcpIp_Task();
        if ((INT16U)(SysTimer100ms - send_time) > 30) break;//tcpip stack stable time 3 sec
    }
}

void Ethernet_SetNetwork(INT8U *ip, INT8U *gw, INT8U *sn)
{
    appData.tempIpAddress.v[0] = ip[3];
    appData.tempIpAddress.v[1] = ip[2];
    appData.tempIpAddress.v[2] = ip[1];
    appData.tempIpAddress.v[3] = ip[0];
    appData.tempMask.v[0] = sn[3];
    appData.tempMask.v[1] = sn[2];
    appData.tempMask.v[2] = sn[1];
    appData.tempMask.v[3] = sn[0];
    appData.tempGwAddress.v[0] = gw[3];
    appData.tempGwAddress.v[1] = gw[2];
    appData.tempGwAddress.v[2] = gw[1];
    appData.tempGwAddress.v[3] = gw[0];

    //Prevent changing same IP 
    if(appData.tempIpAddress.v[3] == EthCardStruct.IP[0] \
        && appData.tempIpAddress.v[2] == EthCardStruct.IP[1] \
        && appData.tempIpAddress.v[1] == EthCardStruct.IP[2] \
        && appData.tempIpAddress.v[0] == EthCardStruct.IP[3]) return;
    
    closesocket(appData.bsdServerSocket);
    closesocket(appData.bsdClientSocket);
    closesocket(appData.UdpServerSocket);
    closesocket(appData.UdpClientSocket);
    APP_Initialize ();
    appData.tempAddressFlag = true;

    INT16U send_time;
    send_time = SysTimer100ms;
    while (1)                   // check the size of received data
    {
        TcpIp_Task();
        if ((INT16U)(SysTimer100ms - send_time) > 30) break;//tcpip stack stable time 3 sec
    }
}

extern INT16S CheckRxBufCountInt(HUGEDATA COMM_BUF *CBuf);
INT32U   getSn_RX_RSR(SOCKET s)
{
	TcpIp_Task();
//	return CheckRxBufCountInt(&(CommBufEthData[s]));
	return (INT32U)CheckRxBufCountInt(&(CommBufEthData[UDP_SOCKET_BUFFER_NTH]));
}
