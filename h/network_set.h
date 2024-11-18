/**
********************************************************************************
* Copyright (c) 2015 CAS
* @brief   Network Setting Header File\n
* @file    Network_set.h
* @version 1.1
* @date    2015/03/30
* @author  Young Hoon Lee (LYH)
********************************************************************************
* @remark  [Version History]\n
*          v1.0 2003/12/15 created\n
*          v1.1 2015/03/30 modified by LYH \n
********************************************************************************
*/

#ifndef _NETWORK_SET_H
#define _NETWORK_SET_H

/*
********************************************************************************
*                       GLOBAL DEFINITIONS & MACROS
********************************************************************************
*/
/* Insert #define here */
#define SETMODE_DHCP_ON				0
#define SETMODE_DHCP_OFF			1

#define NETWORK_REMOTE_IP			0
#define NETWORK_REMOTE_TCP_PORT		4
#define NETWORK_REMOTE_UDP_PORT		6

#ifdef USE_EFT_POS_PROTOCOL
 #define NOT_USE					0
 #define USE_ETHERNET				1
 #define USE_RS232					2
#endif
/*
********************************************************************************
*                       GLOBAL DATA TYPES & STRUCTURES
********************************************************************************
*/
/* Insert global typedef & struct here */
#pragma pack(2)
typedef struct {
	INT8U	ip[4];
	INT16U	tcp_port;
	INT16U	udp_port;
} REMOTE_MACHINE_STR;
#pragma pack()

/*
********************************************************************************
*                       GLOBAL(EXPORTED) VARIABLE EXTERNALS
********************************************************************************
*/
/* Insert global variable externals here */

/*
********************************************************************************
*                       GLOBAL(EXPORTED) FUNCTION PROTOTYPES
********************************************************************************
*/
/* Insert global function prototypes here */
extern void netset_enable_interface(void);
extern void netset_servicetype(void);
extern void netset_local_ip_set(void);
extern void netset_remote_ip_set(void);
extern void netset_dhcp_set(void);
extern void netset_rs232c_set(void);
extern void netset_lock(void);
extern void netset_list(void);
extern BOOLEAN netset_ip_valid_check(HUGEDATA INT8U *ip, INT8U mode);
extern void netset_send_speedkey(void);
extern void netset_plu_delete(void);	// 0x1940
extern void netset_access_point(void);
extern INT8U ehternet_test(void);
extern void netset_wlan_config(void);
extern void netset_slave_create(void);
extern void netset_slave_list(void);
extern void netset_slave_delete(void);
#endif /* _NETWORK_SET_H */
