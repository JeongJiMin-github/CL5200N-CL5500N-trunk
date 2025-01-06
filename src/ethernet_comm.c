#include <stdio.h>
#include <string.h>
#include "globals.h"
#include "commbuf.h"
#include "prtdrv.h"
#include "main.h"
#include "flash.h"
#include "flash_app.h"
#include "initial.h"
#include "common.h"
#include "mode_sale.h"
#include "mode_commun.h"
#include "ethernet_comm.h"
#include "ethernet.h"
#include "main.h"
#include "mode_main.h"
#include "network_command.h"
#include "prt_interpreter.h"
#include "debug.h"
#include "mode_error.h"
#include "dsp_app.h"
#include "plu_device.h"
#include "plu_manager.h"
#include "traceability.h"
#include "caption_app.h"
#include "wlan_set_lantronix.h"
#ifdef USE_CHN_CART_SCALE
#include "traceability_chn.h"
#endif
#include "inventory.h"
#include "serial.h"
#include "scale_config.h"
#include "barcode_file.h"
#ifdef USE_EFT_POS_PROTOCOL
#include "network_set.h"
#endif
#include "origin.h"
#ifdef USE_MORNITORING_CNT
#include "rtc_app.h"
#endif

INT8U useCheckingMaster = ON;	// CJK080701 Parameter 추가
//INT8U useCheckingMaster = OFF;	// CJK080701 Parameter 추가
INT8U   Debugmode = 0;
#ifdef USE_SETPRODUCT_SALE
extern INT8U SetproductSaleMode;
#endif
#ifdef USE_REALTIME_CHECK_INTERVAL
INT32U check_interval;
INT8U netcheckflag = 0;
#endif
#ifdef USE_EFT_POS_PROTOCOL
 extern INT8U ChangeRemote2IPFlag;
 INT8U PosProtocolSendFlag = OFF;
 INT8U remote2_connection = OFF;
 void send_eft_pos_protocol(INT8U use_rs232);
#endif
#if defined(USE_INDIA_FUNCTION) || defined(USE_SEND_TICKET_DATA)
INT8U reopenCheck = OFF;
#endif
#if defined(USE_REMOTE_FW_DOWNLOAD) || defined(USE_REMOTE_ROM_UPDATE)
INT8U RemoteFwUpdateFlag = REMOTE_UPDATE_NONE;
#endif

#ifdef USE_HYUNDAI_SET_FUNCTION
extern RTC_STRUCT SetSaleRTC;
#endif

extern INT16S PutSmallString(INT16S y, INT16S x, char *data);
//extern INT16S DisplayMsg(char *msg);
NETWORK_STATUS network_status;
extern INT16S netGetClerkLock(INT8U operator, INT8U scaleid);
//extern INT8U transaction_sale_send_master(HUGEDATA COMM_BUF *CBuf,INT8U direct);
//extern INT8U transaction_sale_send_serial(HUGEDATA COMM_BUF *CBuf,INT8U direct);
extern void display_message_page(char *str);
extern char  gmsg[];
extern INT8U KEY_Read(void);
//extern INT8U  network_remotecounter(void);
extern void network_transaction_check(INT8U use_rs232);
INT16S ethernet_gen_head(char *str)
{
	INT16S slen;
	slen=0;
	sprintf(str,"^=%02X.*=%02X.$=%01X.",status_scale.scaleid,status_scale.departmentid,status_scale.scale_lock);
	slen=strlen(str);
	sprintf(&str[slen],"&=%02X%02X%02X%02X.",EthCardStruct.IP[3],EthCardStruct.IP[2],EthCardStruct.IP[1],EthCardStruct.IP[0]);
	slen+=strlen(&str[slen]);
	sprintf(&str[slen],"@=%04X.",EthCardStruct.TCPSourcePort);
	slen+=strlen(&str[slen]);
	sprintf(&str[slen],"?=%1X.",ethernet_list.status);
	slen+=strlen(&str[slen]);
	return slen;
}

INT16S ethernet_gen_head_ip(char *str,unsigned char *ip)
{
	INT16S slen;
	slen=0;
	sprintf(str,"^=%02X.*=%02X.",status_scale.scaleid,status_scale.departmentid);
	slen=strlen(str);
	sprintf(&str[slen],"&=%02X%02X%02X%02X.",ip[0],ip[1],ip[2],ip[3]);
	slen+=strlen(&str[slen]);
	sprintf(&str[slen],"@=%04X.",EthCardStruct.TCPSourcePort);
	slen+=strlen(&str[slen]);
	return slen;
}

//void ethernet_command(INT8U ip0,INT8U ip1,INT8U ip2,INT8U ip3,INT16U base,INT16U cmd)
//{
//	INT8U sdata[72];// ,data[32];
//	INT16S  slen,r;// ,sl;
//	INT8U bcc;
//	slen =15;
//	r=ethernet_gen_head((char *)&sdata[slen]);
//	slen+=r;
//	sprintf((char *)sdata,"i00F%03X,%03XL%02X",base,cmd,slen-15);
//	bcc=0x3a;
//	sdata[14]=bcc;
//	bcc = gen_bcc((INT8U *)&sdata[15],slen-15);
//	sdata[slen++]=bcc;
//	ethernet_udpcast( ip0, ip1, ip2, ip3,(char *)sdata,slen); // break;
//}

void  ethernet_debug(void)
{
	INT16S i;
	char str[32];
	sprintf(str,"Global_Com=%02x.%02x\r\n",Startup.global_com,ethernet_list.status);
	MsgOut(str);
	sprintf(str,"MST%d %3d.%3d.%3d.%3d:%4x\r\n",ethernet_list.mstatus,ethernet_list.master.ip[0],ethernet_list.master.ip[1],ethernet_list.master.ip[2],ethernet_list.master.ip[3],(unsigned short)ethernet_list.master.port);
	MsgOut(str);
	for (i=0; i<ethernet_list.nlist; i++) {
		sprintf(str,"[%2d] %3d.%3d.%3d.%3d:%4x,%d\r\n",i,ethernet_list.elist[i].ip[0],ethernet_list.elist[i].ip[1],ethernet_list.elist[i].ip[2],ethernet_list.elist[i].ip[3],(unsigned short)ethernet_list.elist[i].port,ethernet_list.elist[i].request_data);
		MsgOut(str);
	}
}

extern void TcpIp_Task(void);
void network_tx_proc(void)
{
//	INT8U i;
	INT16U len;
	INT8U tmpflag;
   	INT8U usbtmpbuf[1500];

	TcpIp_Task();
	
	if(CheckTxBuf(&CommBuf)) {
        Uart0SendData();		
	}

	if (CommBufAD.Txing != 1) {
		if(CheckTxBuf(&CommBufAD)) {
			Uart1SendData();
		}
	}

#ifndef CPU_PIC32MZ
	if (CommBufKey.Txing != 1) {
		if(CheckTxBuf(&CommBufKey)) {
			Uart2SendData();
		}
	}
	
	len = CheckTxBufChars(&CommBufUsb);
	if (len) {
		GetDataTxBuf(&CommBufUsb, (char*)usbtmpbuf, len);
		tmpflag = Usb_Write(usbtmpbuf, len);
	}
#endif
#ifdef USE_CHN_IO_BOARD
	if (UseI2CComm)
	{
		i2c_txDataProc();
	}
#endif	
}

void  network_param_init(void)
{
	memset((INT8U *)&network_status,0,sizeof(NETWORK_STATUS));
}

void  network_timeout_reset(INT16S id)
{
	network_status.timeset[id]=SysTimer100ms;
}

void network_timeout_init(void)
{
	INT16S i;
	for (i=0; i<NETWORK_TIMEOUT_MAX; i++) {
		network_status.timeset[i]=SysTimer100ms;
		network_status.counter[i]=0;
		network_status.prevTime[i]=0;
	}
#ifdef USE_PB_BATTERY
	network_status.timeset[NETWORK_TIMEOUT_BAT_SIGN] = PrtDrvStruct.Timer10ms;
#endif
	network_status.time_trans1=10;
	network_status.time_trans2=10;
	network_status.time_trans3=0;		//Modified by JJANG 20080312 바로 실행시키기 위해 0으로 초기화
#ifdef USE_EFT_POS_PROTOCOL
	network_status.time_trans4=10;
	network_status.time_trans5=10;
#endif
#ifdef USE_REALTIME_CHECK_INTERVAL
	check_interval = get_global_wparam(GLOBAL_REALTIME_CHECK_INTERVAL);
	if( check_interval < REALTIME_CHECK_INTERVAL_MIN || check_interval > REALTIME_CHECK_INTERVAL_MAX )
	{
		check_interval = REALTIME_CHECK_INTERVAL_DEFAULT;
		set_global_wparam(GLOBAL_REALTIME_CHECK_INTERVAL, REALTIME_CHECK_INTERVAL_DEFAULT);
	}
	check_interval = check_interval*10;
#endif

}

/***************
	id=0 : timesync
	id=1 : slave
	id=2 : transaction
 **************************/
INT8U network_timeout_check(INT16U out,INT16S id)
{
	if ((INT16U)(SysTimer100ms - network_status.timeset[id])>out) {
		network_status.timeset[id] = SysTimer100ms;
		return TRUE;
	}
	return FALSE;
}

INT8U network_timeout_check_10ms(INT16U out,INT16S id)
{
	if ((INT16U)(PrtDrvStruct.Timer10ms - network_status.timeset[id])>out) {
		network_status.timeset[id] = PrtDrvStruct.Timer10ms;;
		return TRUE;
	}
	return FALSE;
}

void  network_counter_reset(INT16S id)
{
	network_status.counter[id]=0;
}

INT8U network_counter_check(INT16U cnt,INT16S id)
{
	INT8U ret;
	ret=FALSE;
	if (network_status.counter[id]<cnt) {
		ret=TRUE;
	}
	network_status.counter[id]++;
	return ret;
}

void network_common_trans(void)
{
	INT8U i;//HYP 20041222

	serial_proc(&CommBuf);
#ifndef CPU_PIC32MZ
	serial_proc(&CommBufUsb);
#endif
	if (Startup.global_com&0x01) {
		for(i=0; i<ETH_BUF_MAX_NUM; i++) {
			EthDataProc(&(CommBufEthData[i]));
			serial_proc(&(CommBufEthData[i])); 
		}
	}
#ifdef USE_CHN_IO_BOARD
	if (UseI2CComm)
	{
		i2c_dataProc();
	}
	//serial_proc(&CommBufRFID);	// 별도의 interpreter 사용
	//serial_proc(&CommBufGPRS);	// 응용방법 정의 안됨
	//serial_proc(&CommBufUsbMem);	// 별도의 interpreter 사용
#endif
	network_tx_proc();
	CASInterpreter(&CommBufPrt);
}

void network_common_trans_client(void)
{
	if (Startup.global_com&0x01) {
		EthDataProc(&(CommBufEthData[0]));
		serial_proc(&(CommBufEthData[0])); 
	}
	network_tx_proc();
	CASInterpreter(&CommBufPrt);
}

void network_command(HUGEDATA COMM_BUF *CBuf,INT16U base,INT16U cmd)
{
	INT8U sdata[72];
	INT16S  slen,r;
	INT8U bcc;

	slen =15;
	r=ethernet_gen_head((char *)&sdata[slen]);
	slen+=r;
	// i00F16,01L... 
	sprintf((char *)sdata,"i00F%03X,%03XL%02X",base,cmd,slen-15);
	bcc=0x3a;
	sdata[14]=bcc;
	bcc = gen_bcc((INT8U *)&sdata[15],slen-15);
	sdata[slen++]=bcc;
	commun_outleng(CBuf,(char *)sdata,slen);
}

void network_direct(INT8U set)
{
	static INT8U allow_old;
	if (set) {
		allow_old=network_status.allow_sale;
		network_status.allow_sale=1;
	} else {
		network_status.allow_sale=allow_old;
	}
}

extern INT8U CheckingWirelessLAN;
void network_timesync(void)
{
	INT8U ret;
	static INT8U tmchk=0;

	if (ethernet_list.timesync) return;
	if (CheckingWirelessLAN) return;
	if (tmchk) {
		if (network_timeout_check(100,NETWORK_TIMEOUT_TIMESYNC)) {
			if (network_counter_check(10,NETWORK_TIMEOUT_TIMESYNC)) {
//MsgOut("TimeSync retry\r\n");
				goto NEXT;
			} else {
//MsgOut("TimeSync timeout\r\n");
				ethernet_list.timesync=1;
				tmchk = 0;
				return;
			}
		} else {
			goto SKIP;
		}
	} else {
		network_timeout_reset(NETWORK_TIMEOUT_TIMESYNC);
		network_counter_reset(NETWORK_TIMEOUT_TIMESYNC);
	}
NEXT:
//sprintf(gmsg,"tm\r\n");
//MsgOut(gmsg);
	ret=Ethernet_Server(CLIENT_OPEN, 0, CLIENT_NORMAL);
	if (ret==1) {
		network_direct(1);
		network_timeout_reset(NETWORK_TIMEOUT_SLAVE);
		network_counter_reset(NETWORK_TIMEOUT_SLAVE);
		network_command(&(CommBufEthData[0]),0x16,0x01);
		while (1) {
			network_common_trans();
			if (ethernet_list.timesync) break;
			if (network_timeout_check(5,NETWORK_TIMEOUT_SLAVE)) {
				if (network_counter_check(2,NETWORK_TIMEOUT_SLAVE)) {
					network_command(&(CommBufEthData[0]),0x16,0x01);
				} else {
					break;
				}
			}
		}
		network_direct(0);
		Ethernet_Server(CLIENT_CLOSE, 0, CLIENT_NORMAL);
	}
SKIP:
	tmchk=1;
}

//Added by JJANG 20080307
extern INT8U  scroll_effect;
void network_scalecheck_send(void)
{
	static INT16U failCnt = 0;
	static INT16U networkDp = 0;
	INT8U remote_connection = 0;

#ifdef USE_REALTIME_CHECK_INTERVAL
	if (!check_interval && netcheckflag)
		return;
#endif

	if (!useCheckingMaster)	// "Use Checking Connection"
	{
		network_status.connection = 1;
		Display_flag = 0;	// "NO MASTER" display off
		return;
	}

	if (!(Startup.global_com&0x01)) return;

	if (!ethernet_list.remoteip[0]) return;	// remote ip 설정이 없을 경우 통과

	if (ethernet_list.status == 1 || ethernet_list.status == 3)
	{
		if (network_timeout_check(network_status.time_trans3,NETWORK_TIMEOUT_SEND_CHECK))	       	
		{
#ifdef USE_PROHIBIT_AUTO_SERVER_CHECK
			/* 물리적으로 네트워크가 끊기고 다시 연결되면 전송되지 못한 데이터 전송 */
			if (ETH_LED_LINK)
				remote_connection = ON;
			else
				remote_connection = OFF;
#else
			remote_connection = NetCheckMasterConnection();
#endif
			network_status.time_trans3 = 100;
			network_timeout_reset(NETWORK_TIMEOUT_SEND_CHECK);
			if (remote_connection)
			{
				network_status.connection = 1;
#ifdef USE_LOTTEMART_DEFAULT
				network_status.time_trans3 = 6000;
#else
#ifdef USE_REALTIME_CHECK_INTERVAL
				network_status.time_trans3 = check_interval;
				netcheckflag = 1;
#else
				network_status.time_trans3 = 100;
#endif
#endif
				failCnt = 0;
			}
			else
			{
				network_status.time_trans3 = 50;
				failCnt++;
				if (failCnt > 1)
				{
					failCnt = 1;
					network_status.connection = 0;
					if(ETH_LED_LINK) { Ethernet_Server(CLIENT_CLOSE, NET_CHK_MASTER_WAITTIME, CLIENT_CHECK_MASTER); }
				}
			}
		}
	}
	else if (ethernet_list.status == 2 || ethernet_list.status == 4)
	{
		if (network_timeout_check(network_status.time_trans3,NETWORK_TIMEOUT_SEND_CHECK))	       	
		{
//debug_tic(0);	//DBG080314
			network_masterscale_send();
			network_status.master_check = 0;
			network_status.time_check = 1;		
			network_status.time_trans3 = 100;	//10초
			network_timeout_reset(NETWORK_TIMEOUT_SEND_CHECK);
			network_timeout_reset(NETWORK_TIMEOUT_RECV_CHECK);
//debug_tic('q');	//DBG080314
		}
		if(network_status.time_check)
		{
			if(network_status.master_check)
			{
//debug_tic('A');	//DBG080314
#ifdef _USE_DBG_CHECK_MASTER
//sprintf(MsgBuf, "[MC] master ok\r\n");
//MsgOut(MsgBuf);
#endif
				if(ETH_LED_LINK) { Ethernet_Server(CLIENT_CLOSE, NET_CHK_MASTER_WAITTIME, CLIENT_CHECK_MASTER); }
				network_status.time_check = 0;		
				network_status.time_trans3 = 100;	//10초		
				failCnt = 0;
				network_status.connection = 1;
				if (Operation.operationClerk == OPER_FLOATING && ethernet_list.status == 2)
				{
					Display_flag = 0;
					if(!scroll_effect) sale_display_update(UPDATE_CENTER);
				}
			}															  
			else
			{ 
				if (network_timeout_check(2,NETWORK_TIMEOUT_RECV_CHECK))	//0.2초
				{
//debug_tic('B');	//DBG080314
#ifdef _USE_DBG_CHECK_MASTER
//sprintf(MsgBuf, "[MC] check again\r\n");
//MsgOut(MsgBuf);
#endif
					if(ETH_LED_LINK) { Ethernet_Server(CLIENT_CLOSE, NET_CHK_MASTER_WAITTIME, CLIENT_CHECK_MASTER); }
					network_status.time_check = 0;
					if (ethernet_list.status == 2)	// slave일 때만 시간 간격 조정
					{
						network_status.time_trans3 = 25;	// 연결이 끊어지면 2.5초마다 재시도
					}
					else if (ethernet_list.status == 4)
					{
						network_status.time_trans3 = 30;	// 연결이 끊어지면 3초마다 재시도
						failCnt = 3;				// 한번에 체크
					}
					failCnt++;
					if (failCnt > 3) 
					{
//debug_tic('C');	//DBG080314
#ifdef _USE_DBG_CHECK_MASTER
sprintf(MsgBuf, "[MC] check fail\r\n");
MsgOut(MsgBuf);
#endif
						failCnt = 3;	// count 유지
						network_status.connection = 0;
						if (Operation.operationClerk == OPER_FLOATING && ethernet_list.status == 2)
						{
							Display_flag = 1;
							sale_display_update(UPDATE_CENTER);
						}
					}
				}
			}
		}
	}
	if (networkDp != network_status.connection)
	{
		networkDp = network_status.connection;
		sale_display_update(UPDATE_MODE);
	}
}


//void serial_realtime_check(void)
//{
//	INT8U flag;
//
//	flag=get_global_bparam(GLOBAL_SALE_SETUP_FLAG4);
//	if( !(flag&0x80) ) return;
//
//	network_common_trans();
//	if (network_timeout_check(network_status.time_trans1,NETWORK_TIMEOUT_TRANSACTION)) 
//	{
//		transaction_sale_send(&CommBuf, ON); 
//	}
//}
extern INT16U SysBootTime;
INT8U CheckingWirelessLAN = OFF;
INT8U network_check_wireless_bridge(void)
{
	INT8U version[17];
	static INT8U wireless_type = WLTYPE_LANTRONIX;

	// Check Wireless Bridge
	if (!CheckingWirelessLAN) return ON;

	if (network_timeout_check_10ms(500, NETWORK_TIMEOUT_CHECK_WLAN))	// per 5 second
	{
		if ((INT16U)(SysTimer100ms - SysBootTime) < 600)	// 부팅 후 1분까지
		{
			if (wlan_CheckVer_Bridge(wireless_type, version, 1))
			{
				if (version[0] == 'T')	// TOBNET
				{
					Startup.wireless_type = WLTYPE_TOBNET;
					Startup.wireless_model = WLMODEL_T_VERSION;
					if (version[1] == 'A') // Hi-Flying
					{
						Startup.wireless_model = WLMODEL_HIFLYING;
					}
				}
				else
				{
					Startup.wireless_type = WLTYPE_LANTRONIX;
					Startup.wireless_model = WLMODEL_LANTRONIX;
				}
				CheckingWirelessLAN = OFF;
//sprintf(MsgBuf, "WLAN OK(%d)\r\n", (INT16U)(SysTimer100ms - SysBootTime));
//MsgOut(MsgBuf);
				return ON;
			}
			if (Startup.wireless_type == WLTYPE_NONE)	// type을 모를경우 번갈아가며 버전확인
			{
				if (wireless_type == WLTYPE_LANTRONIX) 
				{
					wireless_type = WLTYPE_TOBNET;
				}
				else 
				{
					wireless_type = WLTYPE_LANTRONIX;
				}
			}
			else 
			{
				wireless_type = Startup.wireless_type;
			}
		}
		else
		{
			CheckingWirelessLAN = OFF;
//sprintf(MsgBuf, "WLAN Timeout(%d)\r\n", (INT16U)(SysTimer100ms - SysBootTime));
//MsgOut(MsgBuf);
		}
	}
	return OFF;
}

#ifdef USE_PB_BATTERY
// For Debug
//extern INT16U BatVoltVal;
//#include "sensor.h"
#endif
void usb_update_check(void)
{
	INT16U wait_time;

#ifdef USE_USB_DATA_ROM_UPDATE
	if (CheckUsbDataRomUpdate)
	{
#ifdef USE_CTS_FUNCTION
		if (CtsStruct.CtsExpired)
			wait_time = 220; // CTS Alarm 10 sec
		else if (EthCardStruct.DHCPEnable)
			wait_time = 960; // DHCP 실패 시 최대 1분 33초 소요 + 3초 : 96초
		else
			wait_time = 120; // booting시간 (약9초) + 3초 : 12초
#else
		if(EthCardStruct.DHCPEnable)
			wait_time = 960; // DHCP 실패 시 최대 1분 33초 소요 + 3초 : 96초
		else			
			wait_time = 120; // booting시간 (약9초) + 3초 : 12초
#endif
		if((SysTimer100ms - SysBootTime) > wait_time) // booting시간(약9초) + 3초 : 12초
		{
			CheckUsbDataRomUpdate = OFF;
		}
		else
		{
			if(!data_rom_usb_update())
				CheckUsbDataRomUpdate = OFF;
		}
		// USB Disconnect 상태일 때 UsbDataRomUpdate가 바뀌지 않는 증상, 데이터롬 업데이트 하지 않는 경우 UsbDataRomUpdate를 초기화
		if (CheckUsbDataRomUpdate == OFF)
		{
			UsbDataRomUpdate = DATAROMUPDATE_READY;
			dataRomStatusSave();
			Flash_flush_nvbuffer();
		}
	}
#endif
#ifdef USE_REMOTE_ROM_UPDATE       
    if (RemoteFwUpdateFlag == REMOTE_UPDATE_ROM) 
    {
        if(!data_rom_eth_update())
            SaveRemoteUpdateFlag(REMOTE_UPDATE_NONE);
    }
#endif     
}

void network_check(void)
{
	INT8U flag;
	INT8U use_rs232;
#ifdef USE_EFT_POS_PROTOCOL
	INT8U use_eft_pos;
	use_eft_pos = (get_global_bparam(GLOBAL_SALE_SETUP_FLAG21) & 0x30) >> 4;	// EFT-POS PROTOCOL 전송방식 선택
#endif
	if ((Startup.global_com&0x03) == 0x03) 	// Ethernet 사용 & Wireless Bridge 사용
	{
		// Check Wireless Bridge
		network_check_wireless_bridge();
	}
#ifdef USE_MORNITORING_CNT
	if ((SysOperatingTime == 1) && (Enable_operate_time_cnt == ON))
	{
        rtc_cnt_scale_operated_time();
		SysOperatingTime = 0;
	}
#endif	
	// FTL의 안전한 BACKUP을 위한 최소 충전시간 체크 (4초)
	FtlCheckBackupTime();

	network_scalecheck_send();	// remote server 상태체크
//데이터 전송 시 저울 동작 느려지는 현상 방어, 국내만 적용 
#ifdef USE_GSMART_PLU
 #ifdef USE_EMART_NETWORK
	if (network_timeout_check_10ms(2, NETWORK_TIMEOUT_RCV_INTERVAL))	//20ms
 #else
  #ifndef USE_WEIGHING_PRINTER_COMM
	if (network_timeout_check_10ms(15, NETWORK_TIMEOUT_RCV_INTERVAL))	//150ms
  #endif
 #endif
#endif
	{
//debug_tic('A');
		network_common_trans();
//debug_tic('B');
//debug_tic(0);
	}
#ifdef USE_PB_BATTERY
	if (network_timeout_check_10ms(20, NETWORK_TIMEOUT_BAT_SIGN))
	{
		CheckBattery();
	}
//if (network_timeout_check_10ms(50, NETWORK_TIMEOUT_RCV_INTERVAL))
//{
//	sprintf(MsgBuf, "BAT=%4d, AC=%d Volt=%4.2f\r\n", BatVoltVal, AC_CHECK, (FP32)BatVoltVal*(FP32)11/(FP32)1024*3.3);
//	MsgOut(MsgBuf);
//}
#endif

	NvramAutoEraseSector();

	flag=get_global_bparam(GLOBAL_SALE_SETUP_FLAG4);

	use_rs232 = OFF;
	if( flag&0x80 ) use_rs232 = ON;

#ifdef USE_EFT_POS_PROTOCOL
	if (!use_rs232 || !(use_eft_pos == USE_RS232))
#else
	if (!use_rs232)
#endif
	{
		if (ethernet_list.status==0) return;
		if (!(Startup.global_com&0x01)) return;
		if (Startup.ethernet_notready) return;
	}
	// ===============================================================
	// global_load_parameter 에서 Network Service Flag 로 미리 결정됨.
	// ===============================================================


//	if (network_timeout_check(network_status.time_trans1,NETWORK_TIMEOUT_TRANSACTION)) 
//	{
//		transaction_sale_send(&(CommBufEthData[0]), OFF);	
//	}

	network_transaction_check(use_rs232);	//send, recieve 분리
	
#ifdef USE_EFT_POS_PROTOCOL
	send_eft_pos_protocol(use_eft_pos);		// (Iran Only)
#endif

	if (!use_rs232)
	{
#ifndef USE_INDIA_FUNCTION
		if (network_timeout_check(network_status.time_trans2,NETWORK_TIMEOUT_COMMANDREADY)) 
		{
			transaction_pay_send(&(CommBufEthData[0]),1);
		}
#endif
		network_timesync();
		plu_modify_send();
	}
}

#define REALTIME_COM_DATA_IDLE  1
#define REALTIME_COM_DATA_SEND  2
#define REALTIME_COM_DATA_WAIT  3
#define REALTIME_COM_DATA_COMPLETE 4 
#define REALTIME_COM_DATA_WAIT_PAY    5
#define REALTIME_COM_DATA_COMPLETE_PAY 6  
#define REALTIME_COM_DATA_TIMEOUT 7 


void network_transaction_check(INT8U use_rs232)
{
	static INT8U status = REALTIME_COM_DATA_IDLE;
	INT16U sendtail;
	INT16U org_tail;
	INT16U cur_tail;
	INT16U sendchk;
#ifdef USE_KEEPING_CONNETION
	static INT16U timeOutCnt = 0;
	INT16U head,tail; // 전송해야 할 실시간 판매정보가 쌓여 있을 경우, 마지막 판매정보인지 판단하기 위한 변수
#endif

	switch (status) 
	{
		case REALTIME_COM_DATA_IDLE:
			if (network_timeout_check(network_status.time_trans1,NETWORK_TIMEOUT_TRANSACTION))
			{
				//connection : O --> SEND, X --> IDLE
				if (network_status.connection || use_rs232)
				{
				#ifdef USE_EMART_NETWORK
					network_status.time_trans1 = 4;	//500ms
				#else
					network_status.time_trans1 = 1;	//200ms
				#endif
					status = REALTIME_COM_DATA_SEND;
				}
				else
				{
					//서버의 응답이 없으면, 5초에 한번씩 다시 시도. 
					network_status.time_trans1=50;
					status = REALTIME_COM_DATA_IDLE;
				}
			}
			break;
	
		case REALTIME_COM_DATA_SEND:
			//transaction_sale_send(&(CommBufEthData[0]), OFF);
			if(!Debugmode){
			sendchk = send_realtime_data(use_rs232, &sendtail, &org_tail);
			}//send_eth_data();
#ifdef USE_INDIA_FUNCTION
			if (RealtimeSendMode == SEND_PAY)
			{
				status = REALTIME_COM_DATA_WAIT_PAY;
			}
			else
			{
				status = REALTIME_COM_DATA_WAIT;
			}
#else
			status = REALTIME_COM_DATA_WAIT;
#endif
			if (sendchk == 0) //OK
			{
				network_status.send_tail = sendtail;
				network_status.org_tail = org_tail;
				network_timeout_reset(NETWORK_TIMEOUT_REALTIME_SEND);
				network_counter_reset(NETWORK_TIMEOUT_REALTIME_SEND);
				network_timeout_reset(NETWORK_TIMEOUT_SEND_CHECK);	// remote check time reset(check 시간 연장)
			}
			//else if (sendchk == 7 || sendchk == 4) // 7:ethernet server close, 4:empty send buffer
			else if (sendchk == 7) // 7:ethernet server close, 4:empty send buffer	// 4는 connection 을 close할 필요없음
			{
			#ifndef USE_EMART_NETWORK
				if (!use_rs232)
				{
					Ethernet_Server(CLIENT_CLOSE, NET_REALTIME_WAITTIME, CLIENT_REALTIME_DATA); 
				}
			#endif
				status = REALTIME_COM_DATA_IDLE;
			}
			else status = REALTIME_COM_DATA_IDLE;
			break;

		case REALTIME_COM_DATA_WAIT: 
			//ACK 올 때까지 대기함. 오면 complete, 안오면 time out
			if (network_status.send_trans_sale == 1 || network_status.send_trans_sale == 3)	// 1:ok, 3:중복전송답변
			{
				status = REALTIME_COM_DATA_COMPLETE;
#ifdef USE_KEEPING_CONNETION
				timeOutCnt = 0;
#endif
			}
			else if (network_status.send_trans_sale == 2)	// memory full
			{
				//caption_split_by_code(0xC90D,&cap,0);
				//DisplayMsg(cap.form);
				status = REALTIME_COM_DATA_COMPLETE;
#ifdef USE_KEEPING_CONNETION
				timeOutCnt = 0;
#endif
			}
			else if (network_status.send_trans_sale == 4)	// 잘못된 데이터 전송에 대한 응답
			{
				//network_error(MSG_21_NET_ERR);
				status = REALTIME_COM_DATA_COMPLETE;
#ifdef USE_KEEPING_CONNETION
				timeOutCnt = 0;
#endif
			}
			else if (network_status.send_trans_sale == 5)	// memory full
			{
				//caption_split_by_code(0xCA06,&cap,0);	//MAX TRNASACTION IS 50!
				//DisplayMsg(cap.form);
				status = REALTIME_COM_DATA_COMPLETE;
#ifdef USE_KEEPING_CONNETION
				timeOutCnt = 0;
#endif
			}
			
			if (network_timeout_check(30, NETWORK_TIMEOUT_REALTIME_SEND)) 
			{
				status = REALTIME_COM_DATA_TIMEOUT;
#ifdef USE_KEEPING_CONNETION
				timeOutCnt++;
#endif
			}
			break;

		case REALTIME_COM_DATA_COMPLETE: 
			if (network_status.send_tail == network_status.rcv_tail)
			{
				cur_tail = get_nvram_wparam(NVRAM_SEND_REMAINDED_TAIL);
				// send buffer에서 빼가는 속도보다 쌓이는 속도가 더 빠를 경우,
				// data의 손실은 불가피하며, 최신 data 유지를 위해 send buffer가 다 찼을 경우
				// tail을 하나씩 증가시키게 됨(즉, data를 하나씩 손실하게 됨). 이 경우 
				// server로 전송한 data의 응답이 오기 전에 tail이 증가하는 경우가 발생할 수 있으므로
				// 전송 전 tail이 변하지 않았을 경우만 tail을 저장함
				// 응답 온 tail을 저장하지 않고, 증가된 tail을 유지하도록 함.
				if (cur_tail == network_status.org_tail)
				{
					transaction_sale_set_tail(network_status.rcv_tail);
				}
			}
			if (!use_rs232)
			{
//TEST CJK090519
#ifdef USE_KEEPING_CONNETION
				head=get_nvram_wparam(NVRAM_SEND_REMAINDED_HEAD);
				tail=get_nvram_wparam(NVRAM_SEND_REMAINDED_TAIL);

				if (head==tail) // 마지막 판매정보 전송 완료 시 Remote Server 연결을 Close
				{
					Ethernet_Server(CLIENT_CLOSE, NET_REALTIME_WAITTIME, CLIENT_REALTIME_DATA);
				}
#else
				Ethernet_Server(CLIENT_CLOSE, NET_REALTIME_WAITTIME, CLIENT_REALTIME_DATA);
#endif
			}
			status = REALTIME_COM_DATA_IDLE;
			break;
#ifdef USE_INDIA_FUNCTION
		case REALTIME_COM_DATA_WAIT_PAY: 
			//ACK 올 때까지 대기함. 오면 complete, 안오면 time out
			if (network_status.send_trans_pay == 1 || network_status.send_trans_pay == 5)	// 1:ok
			{
				status = REALTIME_COM_DATA_COMPLETE_PAY;
#ifdef USE_KEEPING_CONNETION
				timeOutCnt = 0;
#endif
			}
			if (network_timeout_check(30, NETWORK_TIMEOUT_REALTIME_SEND)) 
			{
				status = REALTIME_COM_DATA_TIMEOUT;
#ifdef USE_KEEPING_CONNETION
				timeOutCnt++;
#endif
			}
			break;
		case REALTIME_COM_DATA_COMPLETE_PAY: 
			if (network_status.send_tail == network_status.rcv_tail_pay)
			{
				transaction_sale_set_tail(network_status.rcv_tail_pay);
			}
			if (!use_rs232)
			{
//TEST CJK090519
#ifdef USE_KEEPING_CONNETION
#else
				Ethernet_Server(CLIENT_CLOSE, NET_REALTIME_WAITTIME, CLIENT_REALTIME_DATA);
#endif
			}
			status = REALTIME_COM_DATA_IDLE;
			break;
#endif
		case REALTIME_COM_DATA_TIMEOUT:
//TEST CJK090519
#ifdef USE_KEEPING_CONNETION
			if (!use_rs232 && timeOutCnt > 3)
#else
			if (!use_rs232)
#endif
			{
				Ethernet_Server(CLIENT_CLOSE, NET_REALTIME_WAITTIME, CLIENT_REALTIME_DATA);
			}
//			network_status.time_trans1+=50;
//			if (network_status.time_trans1>=150) network_status.time_trans1=150;
			if (Operation.operationClerk == OPER_FLOATING && ethernet_list.status == 2)
			{
//					transaction_sale_set_tail(tail);
			}
			//network_status.time_trans1=10;	// 1sec
			//network_error(MSG_21_NET_ERR);
			status = REALTIME_COM_DATA_IDLE;	//응답이 오지 않으면 다시 IDLE로
	}
}


//SG071009
extern INT8U GlbBarcodeData_Field1[BARCODE_FORMAT_LEN+1];
#ifdef USE_REALTIME_TRANS_TYPE3
void get_realtime_transaction_data(REALTIME_TRANSACTION_TYPE *pTrData, TRANSACTION_RECORD_TYPE *pTrans)
{
	INT8U deptno;
	INT32U pluno;
	INT16U key, v16;
#ifdef USE_TRACE_STANDALONE
	INT16U tempSize;
#endif
	INT8U  string_buf[60];

	deptno = pTrans->trtDept;
	pluno = pTrans->trtPLU;

	memcpy((INT8U *)&(pTrData->contents), (INT8U *)pTrans, sizeof(TRANSACTION_RECORD_TYPE));

	plu_table_search(deptno,(long)pluno,&key,0);

	plu_get_field(key-1, PTYPE_PLU_NAME1, (INT8U *)&string_buf, (INT16S *)&v16, sizeof(string_buf)); // name1
	string_buf[v16] = 0;
	memcpy((INT8U *)pTrData->pluname1, string_buf, sizeof(pTrData->pluname1));

	plu_get_field(key-1, PTYPE_PLU_NAME2, (INT8U *)&string_buf, (INT16S *)&v16, sizeof(string_buf)); // name2
	string_buf[v16] = 0;
	memcpy((INT8U *)pTrData->pluname2, string_buf, sizeof(pTrData->pluname2));

	plu_get_field(key-1, PTYPE_PLU_NAME3, (INT8U *)&string_buf, (INT16S *)&v16, sizeof(string_buf)); // name3
	string_buf[v16] = 0;
	memcpy((INT8U *)pTrData->pluname3, string_buf, sizeof(pTrData->pluname3));
	
	memcpy((INT8U *)pTrData->barcode_data, GlbBarcodeData_Field1, sizeof(GlbBarcodeData_Field1));
	memset((INT8U *)GlbBarcodeData_Field1, 0, sizeof(GlbBarcodeData_Field1));

	//pTrData->additional_info1;
	//pTrData->additional_info2;

#ifdef USE_TRACE_STANDALONE
//	individual_number[20];
	v16 = strlen((char*)CurTraceStatus.indivStr.individualNO);
	strcpy((char *)pTrData->individual_number, (char *)CurTraceStatus.indivStr.individualNO);
	tempSize = sizeof(CurTraceStatus.indivStr.individualNO);
	if( v16>=tempSize ) v16 = tempSize-1;
	pTrData->individual_number[v16] = 0;

//	grade[10]; 
	v16 = strlen((char*)CurTraceStatus.curGradeText );
	strcpy((char *)pTrData->grade, (char *)CurTraceStatus.curGradeText );
	tempSize = sizeof(CurTraceStatus.curGradeText);
	if( v16>=tempSize ) v16 = tempSize-1;
	pTrData->grade[v16] = 0;

//	slaughtDate[4]; 
	pTrData->slaughtDate[0] = CurTraceStatus.indivStr.slaughtDate[0];
	pTrData->slaughtDate[1] = CurTraceStatus.indivStr.slaughtDate[1];
	pTrData->slaughtDate[2] = CurTraceStatus.indivStr.slaughtDate[2];
	pTrData->slaughtDate[3] = 0;

//	breed[10]; 
	v16 = strlen((char*)CurTraceStatus.indivStr.breed);
	strcpy((char *)pTrData->breed, (char *)CurTraceStatus.indivStr.breed);
	tempSize = sizeof(CurTraceStatus.indivStr.breed);
	if( v16>=tempSize ) v16 = tempSize-1;
	pTrData->breed[v16] = 0;

//	farm[20]; 
	v16 = strlen((char*)CurTraceStatus.indivStr.farm);
	strcpy((char *)pTrData->farm, (char *)CurTraceStatus.indivStr.farm);
	tempSize = sizeof(CurTraceStatus.indivStr.farm);
	if( v16>=tempSize ) v16 = tempSize-1;
	pTrData->farm[v16] = 0;
#endif

//sprintf(MsgBuf,"%s",GlbBarcodeData_Field1);
//MsgOut(MsgBuf);

}
#endif

#ifdef USE_EFT_POS_PROTOCOL
void check_remote2_connection(void)
{
	INT16U remote2_port;
	INT8U remote2_ip[4];
	INT8U ret, err, nretry;
	INT8U connection;
	
	if (network_timeout_check(network_status.time_trans5,NETWORK_TIMEOUT_REMOTE2))
	{	
		// IP/PORT 설정
		get_base_sparam(FLASH_REMOTE_IP2, remote2_ip, 4);		// remote2 IP 가져오기
		remote2_port = get_base_wparam(FLASH_REMOTE_IP2 + 4);	// remote2 Port 가져오기
		if (remote2_ip[0] == 0 || remote2_port == 0) return;

		// Dest REMOTE IP2로 변경
		put_dest_ip(remote2_ip, remote2_port);
		ChangeRemote2IPFlag = ON;
		
		ret = err = nretry = 0;
		while (!ret) {
			ret = Ethernet_Server(CLIENT_OPEN, NET_CHK_PC_WAITTIME, CLIENT_CHECK_REMOTE2);
			nretry++;
			if (nretry>=NET_CHK_MASTER_RETRY) break;
		}
		// 연결성공
		if(ret)
		{
			Ethernet_Server(CLIENT_CLOSE, NET_CHK_PC_WAITTIME, CLIENT_CHECK_REMOTE2);
			ChangeRemote2IPFlag = OFF;
			restore_dest_ip(); // REMOTE IP1로 되돌림
			network_status.time_trans5 = 100;
			remote2_connection = ON;
		}
		// 연결실패
		else
		{
			ChangeRemote2IPFlag = OFF;
			restore_dest_ip(); // REMOTE IP1로 되돌림
            Ethernet_Server(CLIENT_CLOSE, NET_CHK_PC_WAITTIME, CLIENT_CHECK_REMOTE2);
			network_status.time_trans5 = 50;
			remote2_connection = OFF;
		}
		network_timeout_reset(NETWORK_TIMEOUT_REMOTE2);
	}
}

void send_eft_pos_data(INT8U use_eft_pos)
{
	INT8U remote2_ip[4];
	INT16U remote2_port;
	INT8U  ret, err, nretry;

	//if (use_eft_pos == NOT_USE) return;
	if((use_eft_pos == USE_ETHERNET) && (remote2_connection == OFF)) return;

	if (network_timeout_check(network_status.time_trans4,NETWORK_TIMEOUT_EFT_POS_PROTOCOL))
	{
		COMM_BUF *CBufNetwork;
		COMM_BUF *CBufRS232;
		INT8U  *commun_buffer;
		INT32U dataLength;
		INT16U head,tail;

		head=get_nvram_wparam(NVRAM_SEND_REMAINDED_HEAD2);
		tail=get_nvram_wparam(NVRAM_SEND_REMAINDED_TAIL2);
		if(head == tail) return;	// 남은 데이터 없음

		commun_buffer = (INT8U *)serial_send_point_for_client;
		dataLength = transaction_eft_pos_data(commun_buffer, head, tail);
		CBufNetwork = &CommBufEthData[0];
		CBufRS232 = &CommBuf;

		// RS232
		if(use_eft_pos == USE_RS232)
		{
			// Data Send
			commun_outleng(CBufRS232, commun_buffer, dataLength);

			// Tail Set
			tail++;
			tail%=Startup.max_send_buf;
			set_nvram_wparam(NVRAM_SEND_REMAINDED_TAIL2,tail);
		}

		//ETHERNET
		else if(use_eft_pos == USE_ETHERNET)
		{
			// IP/PORT 설정
			get_base_sparam(FLASH_REMOTE_IP2, remote2_ip, 4);		// remote2 IP 가져오기
			remote2_port = get_base_wparam(FLASH_REMOTE_IP2 + 4);	// remote2 Port 가져오기
			if (remote2_ip[0] == 0 || remote2_port == 0) return;

			// Dest REMOTE IP2로 변경
			put_dest_ip(remote2_ip, remote2_port);
			ChangeRemote2IPFlag = ON;

			// Remote 연결확인
			ret = err = nretry = 0;
			while (!ret) {
				ret=Ethernet_Server(CLIENT_OPEN, NET_CHK_REMOTE2_WAITTIME, CLIENT_SEND_EFT_POS);
				nretry++;
				if (nretry>=NET_CHK_MASTER_RETRY) break;
			}
			// 연결성공
			if(ret==1)
			{
				commun_outleng(CBufNetwork, commun_buffer, dataLength);
				
				tail++;
				tail%=Startup.max_send_buf;
				set_nvram_wparam(NVRAM_SEND_REMAINDED_TAIL2,tail);
				Ethernet_Server(CLIENT_CLOSE, NET_CHK_REMOTE2_WAITTIME, CLIENT_SEND_EFT_POS);
				ChangeRemote2IPFlag = OFF;
				restore_dest_ip(); // REMOTE IP1로 되돌림
			}
			// 연결실패
			else
			{
				ChangeRemote2IPFlag = OFF;
				restore_dest_ip(); // REMOTE IP1로 되돌림
				Ethernet_Server(CLIENT_CLOSE, NET_CHK_REMOTE2_WAITTIME, CLIENT_SEND_EFT_POS);
			}
		}
		network_timeout_reset(NETWORK_TIMEOUT_EFT_POS_PROTOCOL);
	}
}

void send_eft_pos_protocol(INT8U use_eft_pos)
{
	if(use_eft_pos != NOT_USE)
	{
		check_remote2_connection();			// REMOTE IP2 서버 연결 확인
		send_eft_pos_data(use_eft_pos);		// 데이터 전송
	}
}
#endif

void get_transaction_data_type4_sale(TRANSACTION_SALE_DATA_TYPE *pTrSaleData, TR_ADDITION_DATA_TYPE* pTrans_add, TRANSACTION_RECORD_TYPE *pTrans)
{
	INT8U len;

	if (pTrans)
	{
		pTrSaleData->sdtType = pTrans->trtType;	//0:sale data, 0x80:change data
		pTrSaleData->sdtScaleId = pTrans->trtScaleId;
		pTrSaleData->sdtPLUtype = pTrans->trtPLUtype;
		pTrSaleData->sdtDept = pTrans->trtDept;
		pTrSaleData->sdtPLUNo = pTrans->trtPLU;
		pTrSaleData->sdtItemCode = pTrans->trtCode;
		pTrSaleData->sdtWeight = pTrans->trtWeight;
		pTrSaleData->sdtQty = pTrans->trtQty;
		pTrSaleData->sdtPCS = pTrans->trtPCS;
		pTrSaleData->sdtUnitPrice = pTrans->trtUnitPrice;
		pTrSaleData->sdtTotalPrice = pTrans->trtVolume;
		pTrSaleData->sdtTransactionCounter = pTrans->trtTransactionCounter;
		pTrSaleData->sdtDiscountPrice = pTrans->trtDiscountVolume;
		pTrSaleData->sdtStatus = pTrans->trtStatus;
#ifdef USE_CL5J_REALTIME_TRNASFER
		pTrSaleData->sdtTareWeight = pTrans->trtTareWeight;
		pTrSaleData->sdtGroup = pTrans->trtGroup;
		pTrSaleData->sdtOperator = pTrans->trtOperator;
#endif

	}

	if (pTrans_add)
	{
		pTrSaleData->sdtPrefix = pTrans_add->adtPrefix;
		memcpy(pTrSaleData->sdtDate, pTrans_add->adtDate, sizeof(pTrSaleData->sdtDate));
		memcpy(pTrSaleData->sdtSaleDate, pTrans_add->adtSaleDate, sizeof(pTrSaleData->sdtSaleDate));
#ifndef USE_EMART_NETWORK
		pTrSaleData->Reserved = pTrans_add->Reserved;	//091126
#endif
	}
	
	len = strlen((char*)GlbBarcodeData_Field1);
	if (len > sizeof(pTrSaleData->sdtBarcodeStr)) len = sizeof(pTrSaleData->sdtBarcodeStr);
	memcpy(pTrSaleData->sdtBarcodeStr, GlbBarcodeData_Field1, len);
#ifdef USE_TRACE_STANDALONE
	memcpy(pTrSaleData->sdtTraceNo, CurTraceStatus.indivStr.individualNO, sizeof(pTrSaleData->sdtTraceNo));
	pTrSaleData->sdtSlaughterHouse = CurTraceStatus.slPlace;
#endif
#ifdef USE_EMART_NETWORK
	pTrSaleData->sdtMeatUse = CurTraceStatus.meatUse;
#endif
}

void get_transaction_data_type4_change(TRANSACTION_CHANGE_DATA_TYPE *pTrChangeData, TR_ADDITION_DATA_TYPE* pTrans_add, TRANSACTION_RECORD_TYPE *pTrans, UNITPRICE_CHANGE_TYPE *pChgPrice)
{
	INT8U len;
	if (pTrans)
	{
		pTrChangeData->cdtType = pTrans->trtType;
		pTrChangeData->cdtQty = pTrans->trtQty;
		pTrChangeData->cdtWeight = pTrans->trtWeight;
		pTrChangeData->cdtTotalPrice = pTrans->trtVolume;
		len = strlen((char*)GlbBarcodeData_Field1);
		if (len > sizeof(pTrChangeData->cdtBarcodeString)) len = sizeof(pTrChangeData->cdtBarcodeString);
		memcpy(pTrChangeData->cdtBarcodeString, GlbBarcodeData_Field1, len);
	}
	pTrChangeData->cdtType |= 0x80;	//0:sale data, 0x80:change data
	if (pChgPrice)
	{
		pTrChangeData->cdtScaleId = pChgPrice->trtScaleId;	// pTrans->trtScaleId;
		pTrChangeData->cdtDept = pChgPrice->trtDept;	// pTrans->trtDept;
		pTrChangeData->cdtPLUNo = pChgPrice->trtPLUNo;	// pTrans->trtPLU;
		pTrChangeData->cdtOriginUnitPrice = pChgPrice->trtOriginUnitPrice;
		pTrChangeData->cdtResultUnitPrice = pChgPrice->trtResultUnitPrice;
		pTrChangeData->cdtReasonCode = pChgPrice->trtReasonCode;
		pTrChangeData->cdtChangeFlag = pChgPrice->trtChangeFlag;
	}
	if (pTrans_add)
	{
		pTrChangeData->cdtItemCode = pTrans_add->adtItemCode;
		pTrChangeData->cdtPrefix = pTrans_add->adtPrefix;
		memcpy(pTrChangeData->cdtDate, pTrans_add->adtDate, sizeof(pTrChangeData->cdtDate));
	}
}


#ifdef USE_CHN_CART_SCALE
void get_transaction_data_type7_sale(REPORT_LOG_STRUCT *pRepLogData, TR_ADDITION_DATA_TYPE* pTrans_add, TRANSACTION_RECORD_TYPE *pTrans)
{
	INT8U len;

	if (pTrans)
	{
		pRepLogData->saleType = pTrans->trtType;	//0:sale data, 0x80:change data
		//pRepLogData->sdtScaleId = pTrans->trtScaleId;
		pRepLogData->pluType = pTrans->trtPLUtype;
		//pRepLogData->sdtDept = pTrans->trtDept;
		pRepLogData->pluNo = pTrans->trtPLU;
		pRepLogData->itemNo = pTrans->trtCode;
		pRepLogData->weightOrcount = pTrans->trtWeight;
		//pRepLogData->sdtQty = pTrans->trtQty;
		//pRepLogData->sdtPCS = pTrans->trtPCS;
		pRepLogData->uprice = pTrans->trtUnitPrice;
		pRepLogData->tprice = pTrans->trtVolume;
		pRepLogData->ticketNo = pTrans->trtTransactionCounter;
		//pRepLogData->sdtDiscountPrice = pTrans->trtDiscountVolume;
		//pRepLogData->sdtStatus = pTrans->trtStatus;
	}

	if (pTrans_add)
	{
		//pRepLogData->sdtPrefix = pTrans_add->adtPrefix;
		//memcpy(&(pRepLogData->year), pTrans_add->adtDate, sizeof(pRepLogData->year));	//year,month,day,hours,mnutes,seconds
		//memcpy(pRepLogData->sdtSaleDate, pTrans_add->adtSaleDate, sizeof(pTrSaleData->sdtSaleDate));
		//pRepLogData->Reserved = pTrans_add->Reserved;	//091126
		pRepLogData->year = pTrans_add->adtDate[0];
		pRepLogData->month = pTrans_add->adtDate[1];
		pRepLogData->day = pTrans_add->adtDate[2];
		pRepLogData->hours = pTrans_add->adtDate[3];
		pRepLogData->minutes = pTrans_add->adtDate[4];
		pRepLogData->seconds = pTrans_add->adtDate[5];
	}
	
	//len = strlen((char*)GlbBarcodeData_Field1);
	//if (len > sizeof(pRepLogData->sdtBarcodeStr)) len = sizeof(pTrSaleData->sdtBarcodeStr);
	//memcpy(pRepLogData->sdtBarcodeStr, GlbBarcodeData_Field1, len);

	memcpy(pRepLogData->traceCode, CurCHNTrace.code, TRACE_CODE_SIZE);

}
#endif


#ifdef USE_INDIA_FUNCTION
void get_transaction_data_type8_sale(TRANSACTION_SALE_DATA_TYPE8 *pTrSaleData, TR_ADDITION_DATA_TYPE* pTrans_add, TRANSACTION_RECORD_TYPE *pTrans)
{
	INT8U len;

	if (pTrans)
	{
		pTrSaleData->sdtType = pTrans->trtType;	//0:sale data, 0x80:change data
		pTrSaleData->sdtScaleId = pTrans->trtScaleId;
		pTrSaleData->sdtPLUtype = pTrans->trtPLUtype;
		pTrSaleData->sdtDept = pTrans->trtDept;
		pTrSaleData->sdtPLUNo = pTrans->trtPLU;
		pTrSaleData->sdtItemCode = pTrans->trtCode;
		pTrSaleData->sdtWeight = pTrans->trtWeight;
		pTrSaleData->sdtQty = pTrans->trtQty;
		pTrSaleData->sdtPCS = pTrans->trtPCS;
		pTrSaleData->sdtUnitPrice = pTrans->trtUnitPrice;
		pTrSaleData->sdtTotalPrice = pTrans->trtVolume;
		pTrSaleData->sdtTransactionCounter = pTrans->trtTransactionCounter;
		pTrSaleData->sdtDiscountPrice = pTrans->trtDiscountVolume;
		pTrSaleData->sdtStatus = pTrans->trtStatus;
		pTrSaleData->sdtTicketNo = pTrans->trtTicketNumber;
	}

	if (pTrans_add)
	{
		pTrSaleData->sdtPrefix = pTrans_add->adtPrefix;
		memcpy(pTrSaleData->sdtDate, pTrans_add->adtDate, sizeof(pTrSaleData->sdtDate));
		memcpy(pTrSaleData->sdtSaleDate, pTrans_add->adtSaleDate, sizeof(pTrSaleData->sdtSaleDate));
		pTrSaleData->Reserved = pTrans_add->Reserved;	//091126
	}
	
	len = strlen((char*)GlbBarcodeData_Field1);
	if (len > sizeof(pTrSaleData->sdtBarcodeStr)) len = sizeof(pTrSaleData->sdtBarcodeStr);
	memcpy(pTrSaleData->sdtBarcodeStr, GlbBarcodeData_Field1, len);
}
#endif

#ifdef USE_SEND_TICKET_DATA
void get_transaction_data_type9_sale(TRANSACTION_SALE_DATA_TYPE *pTrSaleData, TR_ADDITION_DATA_TYPE* pTrans_add, TRANSACTION_RECORD_TYPE *pTrans)
{
	INT8U len;

	if (pTrans)
	{
		pTrSaleData->sdtType = pTrans->trtType;
		pTrSaleData->sdtScaleId = pTrans->trtScaleId;
		pTrSaleData->sdtPLUtype = pTrans->trtPLUtype;
		pTrSaleData->sdtDept = pTrans->trtDept;
		pTrSaleData->sdtPLUNo = pTrans->trtPLU;
		pTrSaleData->sdtItemCode = pTrans->trtCode;
		pTrSaleData->sdtWeight = pTrans->trtWeight;
		pTrSaleData->sdtQty = pTrans->trtQty;
		pTrSaleData->sdtPCS = pTrans->trtPCS;
		pTrSaleData->sdtUnitPrice = pTrans->trtUnitPrice;
		pTrSaleData->sdtTotalPrice = pTrans->trtVolume;
		pTrSaleData->sdtTransactionCounter = pTrans->trtTransactionCounter;
		pTrSaleData->sdtDiscountPrice = pTrans->trtDiscountVolume;
		pTrSaleData->sdtStatus = pTrans->trtStatus;
		
//			pTrans->trtTicketNumber = TicketTransactionNumber[pTrans->trtOperator];
        pTrans->trtTicketNumber = get_TicketTransactionNumber(pTrans->trtOperator); // CL5500 -> CL5200N Ticket Transaction Number 호출 방식 변경
		pTrSaleData->sdtPrefix = pTrans->trtTicketNumber;
		pTrSaleData->sdtSlaughterHouse = pTrans->trtIndividualIndex;
		pTrSaleData->Reserved = pTrans->trtOperator;
		if(pTrans->trtType == 0x40)//0x40 : Ticket Info, 0x20: PLU info
		{
			pTrSaleData->sdtDate[0] = bcd2hex(RTCStruct.year);
			pTrSaleData->sdtDate[1] = bcd2hex(RTCStruct.month);
			pTrSaleData->sdtDate[2] = bcd2hex(RTCStruct.date);
			pTrSaleData->sdtDate[3] = bcd2hex(RTCStruct.hour);
			pTrSaleData->sdtDate[4] = bcd2hex(RTCStruct.min);
			pTrSaleData->sdtDate[5] = bcd2hex(RTCStruct.sec);
		}
	}

	if (pTrans_add)
	{
		memcpy(pTrSaleData->sdtDate, pTrans_add->adtDate, sizeof(pTrSaleData->sdtDate));
		memcpy(pTrSaleData->sdtSaleDate, pTrans_add->adtSaleDate, sizeof(pTrSaleData->sdtSaleDate));
	#ifdef USE_BAKERY_FUNCTION
		pTrSaleData->Reserved = pTrans->trtOperator;
	#else
		pTrSaleData->Reserved = pTrans_add->Reserved;	//091126
	#endif
		
	}
	
	len = strlen((char*)GlbBarcodeData_Field1);
	if (len > sizeof(pTrSaleData->sdtBarcodeStr)) len = sizeof(pTrSaleData->sdtBarcodeStr);
	memcpy(pTrSaleData->sdtBarcodeStr, GlbBarcodeData_Field1, len);
}
#endif

#ifdef USE_SEND_TRACE_PRICE
INT8U get_transaction_data_type10_sale(REALTIME_TRACEPRICE *pTrData, TRANSACTION_RECORD_TYPE *pTrans)
{
	INT8U deptno;
	INT32U pluno;
	INT16U key, v16;
	INT16U tempSize;
	INT8U  string_buf[60];
	INT8U  string_buf2[60];
	INT16U plu_pcs;
	INT16S pcs_size;	
	INT16U plu_font5;
	INT16S font5_size;	
	INT16U plu_group;
	INT16S group_size;	
	INT16U plu_origin;
	INT16U origin_size;
	ORIGIN_STRUCT origin_str;
	INT32U addr;
	char *retptr;

	deptno = pTrans->trtDept;
	pluno = pTrans->trtPLU;

	plu_table_search(deptno,(long)pluno,&key,0);

	// 상품명 INT8U pluname[40];
	plu_get_field(key-1, PTYPE_PLU_NAME1, (INT8U *)&string_buf, (INT16S *)&v16, sizeof(string_buf)); // name1
	string_buf[v16] = 0;
	memcpy((INT8U *)pTrData->pluname, string_buf, sizeof(pTrData->pluname));

	// 품목코드	INT32U trace_meatcode;
//	plu_get_field(key-1, PTYPE_PIECE, (INT8U *)&string_buf, (INT16S *)&v16, sizeof(string_buf)); // name1
	plu_pcs = 0;
	plu_font5 = 0;
	plu_get_field(key-1, PTYPE_PIECE, (INT8U *)&plu_pcs, &pcs_size, sizeof(plu_pcs));	
	plu_get_field(key-1, PTYPE_PLU_NAME5_FONT, (INT8U *)&plu_font5, &font5_size, sizeof(plu_font5));	
	if(plu_pcs == 0) return 0;
	if(plu_font5 == 0) return 0;	
	pTrData->trace_meatcode = plu_pcs * 100 + plu_font5;

	// 등급 코드	INT16U trace_grade_no;
	plu_get_field(key-1, PTYPE_GROUP_NO, (INT8U *)&plu_group, (INT16S *)&group_size,sizeof(plu_group));
	pTrData->trace_grade_no = 0;

	if(plu_pcs == 4301) // 한우 
	{
		if(CurTraceStatus.gradeNo)
		{
			if((CurTraceStatus.gradeNo >= 7) && (CurTraceStatus.gradeNo <= 9) || (CurTraceStatus.gradeNo == 1))	// 등급번호 1, 8,9,10 -> 1++, 1++(7), 1++(8),1++(9) 해당 
			{
				pTrData->trace_grade_no = 1;	//1++등급 분류코드: 1
			}
			else if((CurTraceStatus.gradeNo >= 2) && (CurTraceStatus.gradeNo <= 3))	//등급번호 2, 3 -> 1+, 1 해당
			{
				pTrData->trace_grade_no = CurTraceStatus.gradeNo;	//1+등급 분류코드: 2, 1등급 분류코드: 3
			}
			else
			{
				pTrData->trace_grade_no = 0; //해당 없음 분류코드 : 0
			}
		}
		else
		{
			if(plu_group == 1 || plu_group == 2 || plu_group == 3)
			{
				pTrData->trace_grade_no = plu_group;		
			}
		}
	}
	else if(plu_pcs == 4401) // 수입 소고기 
	{
		if(plu_group == 81)	// 미국
		{
			pTrData->trace_grade_no = 81;		
		}
		if(plu_group == 82)	// 호주
		{
			pTrData->trace_grade_no = 82;		
		}
	}
	else if(plu_pcs == 4402) // 수입 돼지고기
	{
		if(plu_group == 91)	// 미국
		{
			pTrData->trace_grade_no = 91;		
		}
		if(plu_group == 92)	// 독일
		{
			pTrData->trace_grade_no = 92;		
		}
		if(plu_group == 93)	// 스페인
		{
			pTrData->trace_grade_no = 93;		
		}
	}

	// 판매 가격, 판매 단위 	INT32U trace_price;
	pTrData->trace_price = pTrans->trtUnitPrice;	// 100g/won	
	pTrData->trace_unit = 2;	// 100g/won		
	if(plu_pcs == 4301) // 한우 
	{
		switch(plu_font5)
		{
			case 72:	// 우두
			case 56:	// 내장 
			case 90:	// 가죽 
			case 61:	// 사골
			case 71:	// 우족
			case 62:	// 꼬리
			case 63:	// 도가니
			case 69:	// 잡뼈
			case 57:	// A 지방 
			case 58:	// B 지방 
				pTrData->trace_price = pTrans->trtUnitPrice * 10; // kg/won
				pTrData->trace_unit = 1; // kg/won
				break;
		}
	}
	else if(plu_pcs == 4304) // 한돈
	{
		switch(plu_font5)
		{
			case 42:	// 돈두
			case 81:	// 내장 
			case 61:	// 돈피
			case 41:	// 족
			case 82:	// 등뼈
			case 83:	// 잡뼈
			case 84:	// A 지방 
			case 85:	// B 지방 
				pTrData->trace_price = pTrans->trtUnitPrice * 10; // kg/won
				pTrData->trace_unit = 1; // kg/won				
				break;
		}
	}

	// 판매 시각 	INT8U  trace_date[6];
	pTrData->trace_date[0] = bcd2hex(RTCStruct.year);
	pTrData->trace_date[1] = bcd2hex(RTCStruct.month);
	pTrData->trace_date[2] = bcd2hex(RTCStruct.date);
	pTrData->trace_date[3] = bcd2hex(RTCStruct.hour);
	pTrData->trace_date[4] = bcd2hex(RTCStruct.min);
	pTrData->trace_date[5] = bcd2hex(RTCStruct.sec);

//	individual_number[20];
	// 이력 번호	INT8U  trace_code[12];		
	v16 = strlen((char*)CurTraceStatus.indivStr.individualNO);
	if(v16 <=12)
	{
		strcpy(pTrData->trace_code, CurTraceStatus.indivStr.individualNO);
	}

	//근내지방도
	if((CurTraceStatus.gradeNo >= 7) && (CurTraceStatus.gradeNo <= 9))	// 등급번호 8,9,10 -> 1++(7), 1++(8),1++(9) 해당
	{
		pTrData->trace_grade = CurTraceStatus.gradeNo;
	}

//	grade[10]; 
//	v16 = strlen((char*)CurTraceStatus.curGradeText );
//	strcpy((char *)pTrData->grade, (char *)CurTraceStatus.curGradeText );
//	tempSize = sizeof(CurTraceStatus.curGradeText);
//	if( v16>=tempSize ) v16 = tempSize-1;
//	pTrData->grade[v16] = 0;
	return 1;
}
#endif

#ifdef USE_EFT_POS_PROTOCOL
void get_transaction_data_EFT_POS_sale(TRANSACTION_SALE_DATA_TYPE *pTrSaleData, TR_ADDITION_DATA_TYPE* pTrans_add, TRANSACTION_RECORD_TYPE *pTrans)
{
	if (pTrans)
	{
		pTrSaleData->sdtType = pTrans->trtType;
		pTrSaleData->sdtScaleId = pTrans->trtScaleId;
		pTrSaleData->sdtPLUtype = pTrans->trtPLUtype;
		pTrSaleData->sdtDept = pTrans->trtDept;
		pTrSaleData->sdtPLUNo = pTrans->trtPLU;
		pTrSaleData->sdtItemCode = pTrans->trtCode;
		pTrSaleData->sdtWeight = pTrans->trtWeight;
		pTrSaleData->sdtQty = pTrans->trtQty;
		pTrSaleData->sdtPCS = pTrans->trtPCS;
		pTrSaleData->sdtUnitPrice = pTrans->trtUnitPrice;
		pTrSaleData->sdtTotalPrice = pTrans->trtVolume;
		pTrSaleData->sdtTransactionCounter = pTrans->trtTransactionCounter;
		pTrSaleData->sdtDiscountPrice = pTrans->trtDiscountVolume;
		pTrSaleData->sdtStatus = pTrans->trtStatus;
	}
}
#endif

#ifdef USE_HYUNDAI_MEAT_FUNCTION
INT8U get_transaction_data_type14_sale(TRANSACTION_SALE_DATA_TYPE14 *pTrSaleData, TR_ADDITION_DATA_TYPE* pTrans_add, TRANSACTION_RECORD_TYPE *pTrans)
{
	INT8U deptno, len;
	INT16U key, iPrefix;
	INT16S size;
	INT32U pluno;

	pTrSaleData->sdtSaleType = pTrans->trtType;
#ifdef USE_HYUNDAI_SET_FUNCTION
	if(MeatSetSaleMode) { 
		pTrSaleData->sdtSETSerial = (long)get_setproduct_number();
	}
#endif

	deptno = pTrans->trtDept;
	pluno = pTrans->trtPLU;	
	plu_table_search(deptno,(long)pluno,&key,0);

	if(pTrans)
	{
		pTrSaleData->sdtScaleId = pTrans->trtScaleId;
		pTrSaleData->sdtPLUNo = pTrans->trtPLU;
		pTrSaleData->sdtWeight = pTrans->trtWeight;
		pTrSaleData->sdtQty = pTrans->trtQty;
		pTrSaleData->sdtUnitPrice = pTrans->trtUnitPrice;
		pTrSaleData->sdtTotalPrice = pTrans->trtVolume;
		pTrSaleData->sdtDiscountPrice = pTrans->trtDiscountVolume;
		
		plu_get_field(key-1, PTYPE_PLU_PREFIX, (INT8U *)&(iPrefix), (INT16S *)&size, sizeof(iPrefix));
		if (iPrefix==0) pTrSaleData->sdtDept = get_net_wparam(NETWORK_DIVISION_ID);
		else pTrSaleData->sdtDept = (INT8U)iPrefix;
#ifdef  USE_HYUNDAI_CHECK_REASON_CODE
		pTrSaleData->sdtReasonCode = pTrans->trtReaseonCode;
#endif
	}
#ifdef USE_HYUNDAI_SET_FUNCTION
	if(MeatSetSaleMode)
	{
		pTrSaleData->sdtSaleDate[0] = SetSaleRTC.year;	// year 
		pTrSaleData->sdtSaleDate[1] = SetSaleRTC.month; // month
		pTrSaleData->sdtSaleDate[2] = SetSaleRTC.date;	// date
		
		pTrSaleData->sdtSaleTime[0] = SetSaleRTC.hour;	// hour 
		pTrSaleData->sdtSaleTime[1] = SetSaleRTC.min;	// minutes
		pTrSaleData->sdtSaleTime[2] = SetSaleRTC.sec;	// seconds
	}
	else
	{
		if(pTrans_add)
		{
			pTrSaleData->sdtSaleDate[0] = pTrans_add->adtDate[0]; // year 
			pTrSaleData->sdtSaleDate[1] = pTrans_add->adtDate[1]; // month
			pTrSaleData->sdtSaleDate[2] = pTrans_add->adtDate[2]; // date
			
			pTrSaleData->sdtSaleTime[0] = pTrans_add->adtDate[3]; // hour 
			pTrSaleData->sdtSaleTime[1] = pTrans_add->adtDate[4]; // minutes
			pTrSaleData->sdtSaleTime[2] = pTrans_add->adtDate[5]; // seconds
		}
	}
#else
	if(pTrans_add)
	{
		pTrSaleData->sdtSaleDate[0] = pTrans_add->adtDate[0]; // year 
		pTrSaleData->sdtSaleDate[1] = pTrans_add->adtDate[1]; // month
		pTrSaleData->sdtSaleDate[2] = pTrans_add->adtDate[2]; // date
		
		pTrSaleData->sdtSaleTime[0] = pTrans_add->adtDate[3]; // hour 
		pTrSaleData->sdtSaleTime[1] = pTrans_add->adtDate[4]; // minutes
		pTrSaleData->sdtSaleTime[2] = pTrans_add->adtDate[5]; // seconds
	}
#endif
	len = strlen((char*)pTrans->trtBarcodeStr);
	memcpy(pTrSaleData->sdtBarcodeStr, pTrans->trtBarcodeStr, len);
	
#ifdef USE_HYUNDAI_SET_FUNCTION
	if(MeatSetSaleMode) { //세트 상품인 경우 영수증 번호 전송. 아닌 경우 null 값 유지 
		len = strlen((char*)ReceiptData);
		if (len > sizeof(pTrSaleData->sdtReceiptNo)) len = sizeof(pTrSaleData->sdtReceiptNo);
		memcpy(pTrSaleData->sdtReceiptNo, ReceiptData, len);

		if(MeatSetLabelTransFlag) //세트 상품인 경우 세트 내품 수 전송. 아닌 경우 null 값 유지 
			pTrSaleData->sdtSETCompNum = MeatSetCompNum;
	}
#endif
	return 1;
}
#endif

void transaction_sale_store(TRANSACTION_RECORD_TYPE *pTrans, TRANSACTION_RECORD_SUB_TYPE *pTrans_sub, TR_ADDITION_DATA_TYPE* pTrans_add, UNITPRICE_CHANGE_TYPE *pChgPrice, INT8U mode, INT8U sendflag)
{
	INT16U  head,tail,xhead;
	INT32U addr;
	INT8U  flagRealTimeRS232;
	INT8U  temp;//, temp_ms;
	INT16U total_send_sale, total_send_change;

#ifdef USE_REALTIME_TRANS_TYPE3
	REALTIME_TRANSACTION_TYPE trData;
#endif
	TRANSACTION_SALE_DATA_TYPE trSaleData;
	TRANSACTION_CHANGE_DATA_TYPE trChangeData;
#ifdef USE_CHN_CART_SCALE
	REPORT_LOG_STRUCT trReportLogData;
#endif
#ifdef USE_INDIA_FUNCTION
	TRANSACTION_SALE_DATA_TYPE8 trSaleData8;
#endif
#ifdef USE_SEND_TRACE_PRICE
	REALTIME_TRACEPRICE trTraceData;
	INT8U checkdata;
#endif
#ifdef USE_HYUNDAI_MEAT_FUNCTION
	TRANSACTION_SALE_DATA_TYPE14 trSaleData14;
#endif

	temp=get_global_bparam(GLOBAL_SALE_SETUP_FLAG4);
	flagRealTimeRS232 = temp&0x80;

//Deleted by JJANG 20080314
//	temp_ms=get_global_bparam(GLOBAL_SALE_SETUP_FLAG8);
//	temp_ms=temp_ms&0x01;

	//if( !flagRealTimeRS232 ) return;
#ifdef USE_SETPRODUCT_SALE
	if(GlbOper.realtimeTransactionType == 10 && !SetproductSaleMode) return;
#endif

	if ((Startup.global_com&0x01)==0)
	{
		if(flagRealTimeRS232) goto SALE_STORE;
		else  return;
	}
	if (sendflag)	//unitprice chage
	{
		if (!pChgPrice) return;
 		total_send_change = get_nvram_wparam(NVRAM_TOTAL_SEND_CHEANGE);
		total_send_change++;
		set_nvram_wparam(NVRAM_TOTAL_SEND_CHEANGE,total_send_change);
 	}
	else
	{
		if (!pTrans) return;
		total_send_sale = get_nvram_wparam(NVRAM_TOTAL_SEND_SALE);
		total_send_sale++;
		set_nvram_wparam(NVRAM_TOTAL_SEND_SALE,total_send_sale);
	}


	if((!ethernet_list.remoteip[0]) && (!ethernet_list.remoteip[1]) && (!flagRealTimeRS232) )
	{
#if defined(USE_CHN_CART_SCALE) || defined(USE_EMART_NETWORK)
		goto SALE_STORE; // 중국은 원격IP 설정과 무관하게 판매정보 저장(향후 일반버전도 원격IP와 무관하도록 수정)
#endif
		//Modified by JJANG 20080121
		head=get_nvram_wparam(NVRAM_SEND_REMAINDED_HEAD);
		set_nvram_wparam(NVRAM_SEND_REMAINDED_TAIL,head);
	}
	else 
	{
SALE_STORE:
		if((ethernet_list.status==2 && !get_nvram_bparam(NVRAM_TRANS_BACKUP_FLAG)) || mode	// slave면 local backup 없을 때
			|| (ethernet_list.status == 3 && GlbOper.realtimeTransactionType == RTsend_basic_kr)
			|| (ethernet_list.status == 3 && GlbOper.realtimeTransactionType == RTsend_basic_indiv_kr)
			|| (ethernet_list.status == 3 && GlbOper.realtimeTransactionType == RTsend_basic_name_indiv_kr)
			|| (ethernet_list.status == 3 && GlbOper.realtimeTransactionType == RTsend_trace_cn))
		{
			head=get_nvram_wparam(NVRAM_SEND_REMAINDED_HEAD);
			tail=get_nvram_wparam(NVRAM_SEND_REMAINDED_TAIL);
			if (head>=Startup.max_send_buf) head=0;

			xhead = head+1;
			xhead%= Startup.max_send_buf;

			if (xhead==tail) 
			{
				tail++;
				tail%= Startup.max_send_buf;
				set_nvram_wparam(NVRAM_SEND_REMAINDED_TAIL,tail);  //SG070703	
			}

			addr =head; 
			addr*= MAX_TRANSACTION_TYPE1_SALE_SIZE;// sizeof(TRANSACTION_RECORD_TYPE); 
			addr+=Startup.addr_send_buf;

			set_nvram_wparam(addr,0);	//처음 2byte 초기화
			//파라미터 세팅여부와 관계없이 무조건 처음 2바이트 비움
			set_nvram_sparam(addr+2,(INT8U *)pTrans,sizeof(TRANSACTION_RECORD_TYPE));
#ifdef USE_TRACE_STANDALONE
#ifndef USE_CHN_CART_SCALE
#ifndef USE_PRINT_TICKET_DISCOUNT
			if (pTrans_sub)
			{
				set_nvram_sparam(addr+2+MAX_TRANSACTION_STRUCT_SIZE,(INT8U *)pTrans_sub,sizeof(TRANSACTION_RECORD_SUB_TYPE));
			}
#endif
#endif
#endif
			head++;
			head%= Startup.max_send_buf;
			set_nvram_wparam(NVRAM_SEND_REMAINDED_HEAD,head);	
			if (ethernet_list.status==4)
			{
				set_nvram_bparam(NVRAM_TRANS_BACKUP_FLAG, 1);
			}	
		}
		else if( ethernet_list.status==3 )
		{
			//temp=get_global_bparam(GLOBAL_SALE_SETUP_FLAG4);
			//if(temp&0x40)
			if (GlbOper.realtimeTransactionType == RTsend_full_kr)	// 3:Full
			{ 
			#ifdef USE_REALTIME_TRANS_TYPE3 
				tail=get_nvram_wparam(NVRAM_SEND_REMAINDED_TAIL);
				head=get_nvram_wparam(NVRAM_SEND_REMAINDED_HEAD);
				if (head>=Startup.max_realsend_buf) head=0;

				xhead = head+1;
				xhead%= Startup.max_realsend_buf; //MAX_TRANSACTION_SALE;

				if (xhead==tail) 
				{
					tail++;
					tail%= Startup.max_realsend_buf; //MAX_TRANSACTION_SALE;
					set_nvram_wparam(NVRAM_SEND_REMAINDED_TAIL,tail);  //SG070703	
				}

				addr =head; 
				addr*=MAX_TRANSACTION_REALTIME_SALE_SIZE;// sizeof(REALTIME_TRANSACTION_TYPE);
				addr+=Startup.addr_send_buf;

				memset((INT8U *)&trData, 0, sizeof(REALTIME_TRANSACTION_TYPE));
				get_realtime_transaction_data(&trData, pTrans);
				set_nvram_sparam(addr,(INT8U *)&trData,sizeof(REALTIME_TRANSACTION_TYPE));
				
				head++;
				head%= Startup.max_realsend_buf; //MAX_TRANSACTION_SALE;
				set_nvram_wparam(NVRAM_SEND_REMAINDED_HEAD,head);	
			#endif
			}
#ifdef USE_INDIA_FUNCTION
			else if (GlbOper.realtimeTransactionType == 4) 
			{
				if (RealtimeAddDataFlag)
				{
					head=get_nvram_wparam(NVRAM_SEND_REMAINDED_HEAD);
					tail=get_nvram_wparam(NVRAM_SEND_REMAINDED_TAIL);
					if (head>=Startup.max_send_buf) head=0;

					xhead = head+1;
					xhead%= Startup.max_send_buf;

					if (xhead==tail) 
					{
						tail++;
						tail%= Startup.max_send_buf;
						set_nvram_wparam(NVRAM_SEND_REMAINDED_TAIL,tail);
					}

					addr =head; 
					addr*= MAX_TRANSACTION_TYPE8_SALE_SIZE;
					addr+=Startup.addr_send_buf;

					memset((INT8U *)&trSaleData8, 0, sizeof(TRANSACTION_SALE_DATA_TYPE8));
					get_transaction_data_type8_sale(&trSaleData8, pTrans_add, pTrans);
					set_nvram_sparam(addr,(INT8U *)&trSaleData8,sizeof(TRANSACTION_SALE_DATA_TYPE8));

					head++;
					head%= Startup.max_send_buf;
					set_nvram_wparam(NVRAM_SEND_REMAINDED_HEAD,head);
				}
				else
				{
					if((!Operation.transAddup) && !(pTrans->trtStatus&FLAG_TRANS_VOID))
					{
						head=get_nvram_wparam(NVRAM_SEND_REMAINDED_HEAD);
						tail=get_nvram_wparam(NVRAM_SEND_REMAINDED_TAIL);
						if (head>=Startup.max_send_buf) head=0;

						xhead = head+1;
						xhead%= Startup.max_send_buf;

						if (xhead==tail) 
						{
							tail++;
							tail%= Startup.max_send_buf;
							set_nvram_wparam(NVRAM_SEND_REMAINDED_TAIL,tail);
						}

						addr =head; 
						addr*= MAX_TRANSACTION_TYPE8_SALE_SIZE;
						addr+=Startup.addr_send_buf;

						memset((INT8U *)&trSaleData8, 0, sizeof(TRANSACTION_SALE_DATA_TYPE8));
						get_transaction_data_type8_sale(&trSaleData8, pTrans_add, pTrans);
						set_nvram_sparam(addr,(INT8U *)&trSaleData8,sizeof(TRANSACTION_SALE_DATA_TYPE8));

						head++;
						head%= Startup.max_send_buf;
						set_nvram_wparam(NVRAM_SEND_REMAINDED_HEAD,head);
					}
				}
			}
#else
			else if (GlbOper.realtimeTransactionType == 4 || GlbOper.realtimeTransactionType == 10) // 4:E-Mart
			{
				head=get_nvram_wparam(NVRAM_SEND_REMAINDED_HEAD);
				tail=get_nvram_wparam(NVRAM_SEND_REMAINDED_TAIL);
				if (head>=Startup.max_send_buf) head=0;

				xhead = head+1;
				xhead%= Startup.max_send_buf;

				if (xhead==tail) 
				{
					tail++;
					tail%= Startup.max_send_buf;
					set_nvram_wparam(NVRAM_SEND_REMAINDED_TAIL,tail);  //SG070703	
				}

				addr =head; 
				addr*= MAX_TRANSACTION_TYPE4_SALE_SIZE;
				addr+=Startup.addr_send_buf;

				if (sendflag)	//Change unitprice
				{
					memset((INT8U *)&trChangeData, 0, sizeof(TRANSACTION_CHANGE_DATA_TYPE));
					get_transaction_data_type4_change(&trChangeData, pTrans_add, pTrans, pChgPrice);
					set_nvram_sparam(addr,(INT8U *)&trChangeData,sizeof(TRANSACTION_CHANGE_DATA_TYPE));
				}
				else		//transaction
				{
					memset((INT8U *)&trSaleData, 0, sizeof(TRANSACTION_SALE_DATA_TYPE));
					get_transaction_data_type4_sale(&trSaleData, pTrans_add, pTrans);
					set_nvram_sparam(addr,(INT8U *)&trSaleData,sizeof(TRANSACTION_SALE_DATA_TYPE));
				}

				head++;
				head%= Startup.max_send_buf;
				set_nvram_wparam(NVRAM_SEND_REMAINDED_HEAD,head);	
			}
#endif
#ifdef USE_IMPORT_MEAT_PROTOCOL
			else if (GlbOper.realtimeTransactionType == 6) 
			{
				head=get_nvram_wparam(NVRAM_SEND_REMAINDED_HEAD);
				tail=get_nvram_wparam(NVRAM_SEND_REMAINDED_TAIL);
				if (head>=Startup.max_send_buf) head=0;

				xhead = head+1;
				xhead%= Startup.max_send_buf;

				if (xhead==tail) 
				{
					tail++;
					tail%= Startup.max_send_buf;
					set_nvram_wparam(NVRAM_SEND_REMAINDED_TAIL,tail);
				}

				addr =head; 
				addr*= MAX_TRANSACTION_TYPE4_SALE_SIZE;
				addr+=Startup.addr_send_buf;

				memset((INT8U *)&trSaleData, 0, sizeof(TRANSACTION_SALE_DATA_TYPE));
				get_transaction_data_type4_sale(&trSaleData, pTrans_add, pTrans);
				set_nvram_sparam(addr,(INT8U *)&trSaleData,sizeof(TRANSACTION_SALE_DATA_TYPE));

				head++;
				head%= Startup.max_send_buf;
				set_nvram_wparam(NVRAM_SEND_REMAINDED_HEAD,head);	
			}
#endif
#ifdef USE_CHN_CART_SCALE
			else if (GlbOper.realtimeTransactionType == 7) 
			{
				head=get_nvram_wparam(NVRAM_SEND_REMAINDED_HEAD);
				tail=get_nvram_wparam(NVRAM_SEND_REMAINDED_TAIL);
				if (head>=Startup.max_send_buf) head=0;

				xhead = head+1;
				xhead%= Startup.max_send_buf;

				if (xhead==tail) 
				{
					tail++;
					tail%= Startup.max_send_buf;
					set_nvram_wparam(NVRAM_SEND_REMAINDED_TAIL,tail);
				}

				addr =head; 
				addr*= MAX_TRANSACTION_TYPE7_SALE_SIZE;
				addr+=Startup.addr_send_buf;

				memset((INT8U *)&trReportLogData, 0, sizeof(REPORT_LOG_STRUCT));
				get_transaction_data_type7_sale(&trReportLogData, pTrans_add, pTrans);
				set_nvram_sparam(addr,(INT8U *)&trReportLogData, sizeof(REPORT_LOG_STRUCT));

				head++;
				head%= Startup.max_send_buf;
				set_nvram_wparam(NVRAM_SEND_REMAINDED_HEAD,head);	
			}
#endif
#ifdef USE_SEND_TRACE_PRICE
			else if (GlbOper.realtimeTransactionType == 8)  // HTTP client
			{
				head=get_nvram_wparam(NVRAM_SEND_REMAINDED_HEAD);
				tail=get_nvram_wparam(NVRAM_SEND_REMAINDED_TAIL);
				if (head>=Startup.max_send_buf) head=0;

				xhead = head+1;
				xhead%= Startup.max_send_buf;

				addr =head; 
				addr*= MAX_TRANSACTION_TYPE10_SALE_SIZE;
				addr+=Startup.addr_send_buf;

				memset((INT8U *)&trTraceData, 0, sizeof(REALTIME_TRACEPRICE));
				checkdata = get_transaction_data_type10_sale(&trTraceData, pTrans);
				if(checkdata)
				{
					if (xhead==tail) 
					{
						tail++;
						tail%= Startup.max_send_buf;
						set_nvram_wparam(NVRAM_SEND_REMAINDED_TAIL,tail);  //SG070703	
					}
				
					set_nvram_sparam(addr,(INT8U *)&trTraceData, sizeof(REALTIME_TRACEPRICE));
					
					head++;
					head%= Startup.max_send_buf;
					set_nvram_wparam(NVRAM_SEND_REMAINDED_HEAD,head);	
				}
			}
#endif
#ifdef USE_SEND_TICKET_DATA
			else if (GlbOper.realtimeTransactionType == 9) 
			{
				if((!Operation.transAddup) && !(pTrans->trtStatus&FLAG_TRANS_VOID))
				{
					head=get_nvram_wparam(NVRAM_SEND_REMAINDED_HEAD);
					tail=get_nvram_wparam(NVRAM_SEND_REMAINDED_TAIL);
					if (head>=Startup.max_send_buf) head=0;

					xhead = head+1;
					xhead%= Startup.max_send_buf;

					if (xhead==tail) 
					{
						tail++;
						tail%= Startup.max_send_buf;
						set_nvram_wparam(NVRAM_SEND_REMAINDED_TAIL,tail);
					}

					addr =head; 
					addr*= MAX_TRANSACTION_TYPE4_SALE_SIZE;
					addr+=Startup.addr_send_buf;

					memset((INT8U *)&trSaleData, 0, sizeof(TRANSACTION_SALE_DATA_TYPE));
					get_transaction_data_type9_sale(&trSaleData, pTrans_add, pTrans);
					set_nvram_sparam(addr,(INT8U *)&trSaleData,sizeof(TRANSACTION_SALE_DATA_TYPE));

					head++;
					head%= Startup.max_send_buf;
					set_nvram_wparam(NVRAM_SEND_REMAINDED_HEAD,head);
				}
			}
#endif
#ifdef USE_HYUNDAI_MEAT_FUNCTION
			else if (GlbOper.realtimeTransactionType == 14) {
				head=get_nvram_wparam(NVRAM_SEND_REMAINDED_HEAD);
				tail=get_nvram_wparam(NVRAM_SEND_REMAINDED_TAIL);
				if (head>=Startup.max_send_buf) head=0;

				xhead = head+1;
				xhead%= Startup.max_send_buf;

				if (xhead==tail) 
				{
					tail++;
					tail%= Startup.max_send_buf;
					set_nvram_wparam(NVRAM_SEND_REMAINDED_TAIL,tail);  //SG070703	
				}

				addr =head; 
				addr*= MAX_TRANSACTION_TYPE14_SALE_SIZE;
				addr+=Startup.addr_send_buf;

				memset((INT8U *)&trSaleData14, 0, sizeof(TRANSACTION_SALE_DATA_TYPE14));
				get_transaction_data_type14_sale(&trSaleData14, pTrans_add, pTrans);
				set_nvram_sparam(addr,(INT8U *)&trSaleData14,sizeof(TRANSACTION_SALE_DATA_TYPE14));

				head++;
				head%= Startup.max_send_buf;
				set_nvram_wparam(NVRAM_SEND_REMAINDED_HEAD,head);
			}
#endif
		}
	}
#ifdef USE_EFT_POS_PROTOCOL
	INT8U use_eft_pos;
	use_eft_pos = (get_global_bparam(GLOBAL_SALE_SETUP_FLAG21) & 0x30) >> 4;	// EFT-POS PROTOCOL 전송방식 선택
	if(use_eft_pos != NOT_USE && PosProtocolSendFlag == 1)	// SubTotal 발행 확인
	{
		PosProtocolSendFlag = OFF;
		head=get_nvram_wparam(NVRAM_SEND_REMAINDED_HEAD2);
		tail=get_nvram_wparam(NVRAM_SEND_REMAINDED_TAIL2);
		if (head>=Startup.max_send_buf) head = 0;
		xhead = head+1;
		xhead%= Startup.max_send_buf;

		// Head Over 처리
		if (xhead==tail)
		{
			tail++;
			tail%= Startup.max_send_buf;
			set_nvram_wparam(NVRAM_SEND_REMAINDED_TAIL2,tail);
		}
		addr=head; 
		addr*= MAX_TRANSACTION_TYPE4_SALE_SIZE;
		addr+= NVRAM_SEND_TRANSACTION2;	

		memset((INT8U *)&trSaleData, 0, sizeof(TRANSACTION_SALE_DATA_TYPE));
		get_transaction_data_EFT_POS_sale(&trSaleData, pTrans_add, pTrans);
		set_nvram_sparam(addr,(INT8U *)&trSaleData,sizeof(TRANSACTION_SALE_DATA_TYPE));	
		
		head++;
		head%= Startup.max_send_buf;
		set_nvram_wparam(NVRAM_SEND_REMAINDED_HEAD2,head);
	}
#endif
}

//Modified by JJANG 20080310
void transaction_pay_store(TRANSACTION_PAY *pTrans)
{
	INT8U  head,tail,xhead;
	INT32U addr;
	if ((Startup.global_com&0x01)==0) return;
	if (ethernet_list.status==2 || ethernet_list.status==4)  { // Slave or local
		head=get_nvram_bparam(NVRAM_TRANS_PAY_HEAD);
		tail=get_nvram_bparam(NVRAM_TRANS_PAY_TAIL);
		if (head>=Startup.max_paysend_buf) head=0;
		xhead = head+1;
		xhead%= Startup.max_paysend_buf;
		if (xhead==tail) {
			network_error(MSG_20_NET_PFULL);
			return;
		}
		addr =head; addr*=  sizeof(TRANSACTION_PAY);
		addr+=Startup.addr_paysend_buf;

		set_nvram_sparam(addr,(INT8U *)pTrans,sizeof(TRANSACTION_PAY));
		head++;
		head%= Startup.max_paysend_buf;
		set_nvram_bparam(NVRAM_TRANS_PAY_HEAD,head);
		if (ethernet_list.status==4)
		{
			set_nvram_bparam(NVRAM_TRANS_BACKUP_FLAG, 1);
		}
	} else if (ethernet_list.status==1){ // Master
		head=get_nvram_bparam(NVRAM_TRANS_PAY_HEAD);
		set_nvram_bparam(NVRAM_TRANS_PAY_TAIL,head);
	}
}

#ifdef USE_INDIA_FUNCTION
void transaction_pay_store_type8(TRANSACTION_PAY *pTrans) // pay와 transaction 버퍼 공유
{
	INT8U  head,tail,xhead;
	INT32U addr;
	if ((Startup.global_com&0x01)==0) return;

	if (ethernet_list.status == 3 && UseAreaScaleMode == 2)  
	{
		head=get_nvram_wparam(NVRAM_SEND_REMAINDED_HEAD);
		tail=get_nvram_wparam(NVRAM_SEND_REMAINDED_TAIL);
		if (head>=Startup.max_send_buf) head=0;

		xhead = head+1;
		xhead%= Startup.max_send_buf;

		if (xhead==tail) 
		{
			tail++;
			tail%= Startup.max_send_buf;
			set_nvram_wparam(NVRAM_SEND_REMAINDED_TAIL,tail);
		}

		addr =head; 
		addr*= MAX_TRANSACTION_TYPE8_SALE_SIZE;
		addr+=Startup.addr_send_buf;

		set_nvram_wparam(addr, 0xffff); // pay type
		set_nvram_sparam(addr+2,(INT8U *)pTrans, sizeof(TRANSACTION_PAY));

		head++;
		head%= Startup.max_send_buf;
		set_nvram_wparam(NVRAM_SEND_REMAINDED_HEAD,head);
	}
}
#endif

#if defined(USE_INDIA_FUNCTION) || defined(USE_SEND_TICKET_DATA) || defined(USE_SETPRODUCT_SALE) || defined(USE_HYUNDAI_SET_FUNCTION) 
/**
********************************************************************************
* @brief	linked Transaction 종료 후 한번에 전송하는 함수 (void transaction 제외) \n
*			Send sale data 구성을 위해서는 Transaction & Addtional Transaction 필요 \n
*			Transaction : NVRAM_TRANS_DATA 에 저장 									\n
*			Addtional Transaction NVRAM_TRANS_ADD 에 저장 							\n
*			마지막Data : 합산 정보
* @param    lastClerk
* @return   none
* @remark   step 1. NVRAM 에 저장된 transaction Data 검색(공통)  	\n
*			step 2. 마지막 Transaction 검색해서 전송 (농협& 현대)	\n
*					(농협& 현대의 경우 합산 데이터를 먼저 전송)		\n
*			setp 3. 개별 Transaction Data 전송 (공통)
********************************************************************************
*/
void TransactionSendWithoutVoid(INT16U lastClerk)
{
	TRANSACT_CHAIN *cp;
	TRANSACT_CHAIN tc;
	TRANS_ADD_CHAIN *AddChain;
	TRANS_ADD_CHAIN TransAddChain;
	INT8U sendCount = 0;
	INT8U check_count = 0;
	INT8U TicketTransCount = 0;
	INT16U h_index, tac_index;
  #ifdef USE_SETPRODUCT_SALE
	long SetProductWeight = 0;
	INT16U setNumber = 0;
  #endif

// Step 1 : NVRAM 에 저장되 transaction Data 검색(공통)
	h_index = get_nvram_lparam(NVRAM_TRANS_HEAD+lastClerk*4);
	if (h_index)
	{
		get_nvram_trans_addr(NVRAM_TRANS_DATA, h_index - 1, &tc);
		cp = &tc;
	}
	else
	{
		cp = 0;
 	}	
	tac_index = get_nvram_lparam(NVRAM_TRANS_ADD_HEAD+lastClerk*4);

#if defined(USE_INDIA_FUNCTION) || defined(USE_SEND_TICKET_DATA)
	/*
	Ticket Reopen시 기존 TRANS_ADD_HEAD값(메모리 인덱스)을 지웠기 때문에 TRANS_ADD 정보를 읽을 수 없음
	하지만 TRANS_DATA와 동일한 인덱스에 저장하기 때문에 같은 인덱스로 설정하여 사용 함(임시로 이렇게 처리함)
	정상적이라면 TRANS_DATA를 Reopen 할 때 TRANS_ADD 데이터도 다시 가져와야 함
	*/  
	if(reopenCheck == ON && tac_index == 0)
		tac_index = h_index;
#endif
  #ifdef USE_HYUNDAI_SET_FUNCTION // 데이터 멈추는 문제 관련하여 세트 전송시 TransAdd Data 저장하는 부분 삭제 (현대만)
	AddChain = 0;
	tac_index = 0;
  #else
	if (tac_index)
	{
		get_nvram_transAdd_addr(NVRAM_TRANS_ADD, tac_index - 1, &TransAddChain);
		AddChain = &TransAddChain;
	}
	else
	{
		AddChain = 0;
 	}
  #endif
  #ifdef USE_SETPRODUCT_SALE
		setNumber = get_setproduct_number();
  #endif

	if(!cp)
	{
		return;
	}
// Step 2 : 마지막 Transaction 검색해서 전송 (농협& 현대)
  #if defined(USE_SETPRODUCT_SALE)||defined(USE_HYUNDAI_SET_FUNCTION)
	for(;cp;) // find last transaction
	{
	#ifdef USE_SETPRODUCT_SALE
		if(cp->content.trtStatus & 0x80)
			cp->content.trtWeight = SetProductWeight;
		else
			SetProductWeight += cp->content.trtWeight;
	#endif// 농협은 무게 합산해서 전송
		if(cp->next == NULL)// 다음 데이터가 없음-> 마지막 데이터(세트 정보). 세트 정보를 먼저 발송
			transaction_sale_store((TRANSACTION_RECORD_TYPE *)&(cp->content), NULL, (TR_ADDITION_DATA_TYPE *)&(AddChain->addtion_data), NULL, 0, 0);
		h_index = cp->next;
		if (!cp->next) break;
		get_nvram_trans_addr(NVRAM_TRANS_DATA, h_index - 1, &tc);
		cp = &tc;
		//cp=cp->next;
	#ifdef USE_HYUNDAI_SET_FUNCTION // 데이터 멈추는 문제 관련하여 세트 전송시 TransAdd Data 저장하는 부분 삭제 (현대만)
	#else
		tac_index = AddChain->next;
		get_nvram_transAdd_addr(NVRAM_TRANS_ADD, tac_index - 1, &TransAddChain);
		AddChain = &TransAddChain;
	#endif
	}
	
	#ifdef USE_HYUNDAI_SET_FUNCTION
	if (MeatSetLabelTransFlag) MeatSetLabelTransFlag = OFF;
    #endif

// Step 3 " 개별 Transaction Data 전송 (공통)
// 개별 데이터를 전송하기 위해서 index 를 다시 찾아서 저장
	h_index = get_nvram_lparam(NVRAM_TRANS_HEAD+lastClerk*4);
	get_nvram_trans_addr(NVRAM_TRANS_DATA, h_index - 1, &tc);//h_index 이면 앞에서 return 하기 때문에 값 체크 안 함
	cp = &tc;
	#ifdef USE_HYUNDAI_SET_FUNCTION // 데이터 멈추는 문제 관련하여 세트 전송시 TransAdd Data 저장하는 부분 삭제 (현대만)
	#else
	tac_index = get_nvram_lparam(NVRAM_TRANS_ADD_HEAD+lastClerk*4);
	if(tac_index) {
		get_nvram_transAdd_addr(NVRAM_TRANS_ADD, tac_index - 1, &TransAddChain);
		AddChain = &TransAddChain;
	}
	else AddChain = 0;
	#endif
  #endif

	for(;cp;)
	{
		if(cp->delete != ON)//not void
		{
  #ifdef USE_SEND_TICKET_DATA
			TicketTransCount++;
			cp->content.trtIndividualIndex = TicketTransCount;
  #endif
  #if defined(USE_SETPRODUCT_SALE)||defined(USE_HYUNDAI_SET_FUNCTION)
		if(cp->next)// 맨 마지막에 저장된 세트 정보는 이미 전송됨. 세트 정보 다시 전송하지 않음
  #endif
			transaction_sale_store((TRANSACTION_RECORD_TYPE *)&(cp->content), NULL, (TR_ADDITION_DATA_TYPE *)&(AddChain->addtion_data), NULL, 0, 0);
		}
		h_index = cp->next;
		if (!cp->next) break;
		get_nvram_trans_addr(NVRAM_TRANS_DATA, h_index - 1, &tc);
		cp = &tc;
  #ifdef USE_HYUNDAI_SET_FUNCTION // 데이터 멈추는 문제 관련하여 세트 전송시 TransAdd Data 저장하는 부분 삭제 (현대만)
  #else		
		tac_index = AddChain->next;
    #if defined(USE_INDIA_FUNCTION) || defined(USE_SEND_TICKET_DATA)
	if(reopenCheck == ON && tac_index == 0)
		tac_index = h_index;
    #endif
		get_nvram_transAdd_addr(NVRAM_TRANS_ADD, tac_index - 1, &TransAddChain);
		AddChain = &TransAddChain;
  #endif
	}

#if defined(USE_INDIA_FUNCTION) || defined(USE_SEND_TICKET_DATA)
	if(reopenCheck == ON)
		reopenCheck = OFF;
#endif
	return;
}
#endif // defined(USE_INDIA_FUNCTION) || defined(USE_SEND_TICKET_DATA) || defined(USE_SETPRODUCT_SALE) || defined(USE_HYUNDAI_SET_FUNCTION)
#ifdef USE_SEND_TICKET_DATA
void transactionSendTicketData(INT16U lastClerk)
{
	TRANSACT_CHAIN *cp;
	TRANS_ADD_CHAIN *trans_add;
	INT8U TicketTransCount = 0;
	INT8U trtType;
	TRANSACTION_RECORD_TYPE ptrans;
	TRANSACT_CHAIN tc;
	INT16U h_index;

	trtType = 0x40;

	h_index = get_nvram_lparam(NVRAM_TRANS_HEAD+lastClerk*4);
	if (h_index)
	{
		get_nvram_trans_addr(NVRAM_TRANS_DATA, h_index - 1, &tc);
		cp = &tc;
	}
	else
	{
		cp = 0;
	}
	
	if(!cp)
	{
		return;
	}
	
	// Transaction Count 계산
	for(;cp;)
	{
		TicketTransCount++;
		h_index = cp->next;
		if (!cp->next) break;
		get_nvram_trans_addr(NVRAM_TRANS_DATA, h_index - 1, &tc);
		cp = &tc;
	}

	if(Operation.operationMode == MODE_REPACK)	trtType = TRANS_TYPE_REPACK;
	if(Operation.operationMode == MODE_SELF)	trtType = TRANS_TYPE_SELF;

	memset(&ptrans, 0, sizeof(TRANSACTION_RECORD_TYPE));
	ptrans.trtType = trtType;
	ptrans.trtScaleId = status_scale.scaleid;
#ifdef USE_PERSIA
	// 이란 RTT-9번 사용 시 void 뺀 Transaction Count(Total-Count)를 전송
	if(GlbOper.realtimeTransactionType == 9)
		ptrans.trtIndividualIndex = PrtTotalStr.TotalCount;
	else
		ptrans.trtIndividualIndex = TicketTransCount;
#else
	ptrans.trtIndividualIndex = TicketTransCount;
#endif
	ptrans.trtOperator= lastClerk ;
	ptrans.trtVolume= TotalData.SummaryTotalPrice;
	ptrans.trtDiscountVolume= TotalData.SummarySavedPrice;
	ptrans.trtTransactionCounter = GetTransactionCounter();

	transaction_sale_store((TRANSACTION_RECORD_TYPE *)&ptrans, NULL, NULL, NULL, 0, 0);
	
	return;
}
#endif //USE_SEND_TICKET_DATA

/*
INT8U network_remotecounter(void)
{
	INT8U noerr,ret;

	noerr=0;
	ret=Ethernet_Server(CLIENT_OPEN, 0, CLIENT_NORMAL);
	if (ret==1) {
		network_direct(1);
		network_command(&(CommBufEthData[0]),0x26,01);
		network_timeout_reset(NETWORK_TIMEOUT_REQUEST_LOGIN);
		network_counter_reset(NETWORK_TIMEOUT_REQUEST_LOGIN);
		network_status.counterticket=0;
		noerr=1;
		while (1) {
		    network_common_trans();
		    if (network_status.counterticket) break;
		    if (network_timeout_check(6,NETWORK_TIMEOUT_REQUEST_LOGIN)) {
	    		if (network_counter_check(2,NETWORK_TIMEOUT_REQUEST_LOGIN)) {
				network_command(&(CommBufEthData[0]),0x26,01);
			} else {
				noerr=0;
		        	break;
		    	}
		    }
		}
		network_direct(0);
		Ethernet_Server(CLIENT_CLOSE, 0, CLIENT_NORMAL);
		if (network_status.counterticket==2) {
			noerr=0;
		}
	}
	return noerr;
}
*/
extern INT8U GlbFlagPCnMasterConnection;
//Modified by JJANG 20080131
INT8U network_ClerkDeleteTransaction(INT8U clerk)
{
	INT8U ret,nretry;
	INT8U cmd;
//	char temp[32];
	// ===============================
	// Network Clerk Delete 는 Slave 모드인 경우만 수행한다.
	// ===============================
//	sprintf(temp,"[Clerk netdel <%d>]\r\n",clerk);
//	MsgOut(temp);

//debug_tic(0);
	if (GlbFlagPCnMasterConnection)	cmd = 0x10;
	else 				cmd = 0x30;

	set_nvram_bparam(NVRAM_TRANSACTION_CLERK, clerk);
	ret=0;
	if (Operation.operationClerk == OPER_FLOATING && ethernet_list.status == 2) 
	{
	    //ret=0;
	    nretry=0;
	    while (!ret) 
	    {
		ret=Ethernet_Server(CLIENT_OPEN, 0, CLIENT_CLERK_TRANS);
		nretry++;
//		if (nretry>6) break;
		if (nretry>4) break;
	    }

//debug_tic('A');

	    if (ret==1) 
	    {
//	sprintf(temp,"[1]\r\n");
//	MsgOut(temp);
		network_direct(1);
		network_timeout_reset(NETWORK_TIMEOUT_REQUEST_LOGIN);
		network_counter_reset(NETWORK_TIMEOUT_REQUEST_LOGIN);
		command_login_loaddata(&(CommBufEthData[0]),cmd,clerk,NULL);
		network_status.clerk_saledelete=1;
		while (1) 
		{
		    network_common_trans();
		    if (network_status.clerk_saledelete==0) break;	        

		    if(KEY_Read())	//Added by JJANG 20080131
		    {
			ret=0;
	        	break;
		    }
		    if (network_timeout_check(10,NETWORK_TIMEOUT_REQUEST_LOGIN))
		    {
	    		if (network_counter_check(2,NETWORK_TIMEOUT_REQUEST_LOGIN)) 
	    		{
				command_login_loaddata(&(CommBufEthData[0]),cmd,clerk,NULL); // Retry
			} 
			else 
			{
				ret=0;
		        	break;
		    	}
		    }
		}

//debug_tic('B');

		network_direct(0);
		Ethernet_Server(CLIENT_CLOSE, 0, CLIENT_CLERK_TRANS);
	    } 
	    else 
	    {
		network_error(MSG_21_NET_ERR);
	    }
	}
	else ret = 1;
	return ret;
}

#ifdef _USE_DBG_CHECK_MASTER
extern HUGEDATA ETH_STRUCT EthStruct;
#endif
//Added by JJANG 20080307 slave --> master
INT8U network_masterscale_send(void)
{		   
	INT8U ret;
	INT8U nretry;
	ret = 0;
//	if (Operation.operationClerk == OPER_FLOATING && ethernet_list.status == 2) 
	{
	    	nretry = 0;
	    	while (!ret) 
	    	{
			ret = Ethernet_Server(CLIENT_OPEN, NET_CHK_MASTER_WAITTIME, CLIENT_CHECK_MASTER);
			nretry++;
			if (nretry >= NET_CHK_MASTER_RETRY) break;
	    	}
	    	if (ret == 1) command_connection_data(&(CommBufEthData[0]), 0x01);
#ifdef _USE_DBG_CHECK_MASTER
sprintf(MsgBuf, "[MC] send result [%d, %02x]\r\n", ret, EthStruct.TcpState);
MsgOut(MsgBuf);
#endif
// 		ret=Ethernet_Server(1,NET_CHK_MASTER_WAITTIME);	// 20ms 대기
//	    	if (ret==1) command_connection_data(&(CommBufEthData[0]),0x01);
	}
	return ret;
}
//FlagCopyTicket : 1-> Copyticket인 경우(REOPEN 메세지 안나옴) / 0->REOPEN일 경우
void network_reopen_start_send(INT32U TicketNum, INT16U ClerkNum, INT8U flagCopyTicket)
{		   
	INT8U ret;
	INT8U nretry;
	char string_buf[50];
	CAPTION_STRUCT 	cap;

	ret = 0;
    	nretry = 0;

	while (!ret) 
	{
		ret=Ethernet_Server(CLIENT_OPEN, 0, CLIENT_CLERK_TRANS);
		nretry++;
		if (nretry>4) break;
	}

    	if (ret == 1)
    	{
		network_direct(1);
		network_timeout_reset(NETWORK_TIMEOUT_REQUEST_LOGIN);
		network_counter_reset(NETWORK_TIMEOUT_REQUEST_LOGIN);
    		
    		command_reopen_data(&(CommBufEthData[0]), 0x65, TicketNum, ClerkNum, 0);
		network_status.reopen_status = 0xff;

		while (1) 
		{
			network_common_trans();
			if (network_status.reopen_status != 0xff) break;

			if (network_timeout_check(10,NETWORK_TIMEOUT_REQUEST_LOGIN))
			{
				if (network_counter_check(2,NETWORK_TIMEOUT_REQUEST_LOGIN)) 
				{
					command_reopen_data(&(CommBufEthData[0]), 0x65, TicketNum, ClerkNum, 0);
				} 
				else 
				{
					ret=0;
					break;
				}
			}
		}
		
		if (network_status.reopen_status == 0xff) 
		{
			network_error(MSG_21_NET_ERR);
		}
		else if (network_status.reopen_status == 0)
		{
			if(!flagCopyTicket)
			{
				caption_split_by_code(0xCA09, &cap, 0);	//REOPEN OK!
				DisplayMsg(cap.form);
			}
		}
		else
		{
			caption_split_by_code(0xCA08, &cap, 0);	//CAN NOT REOPEN!
			sprintf(string_buf, "%s (%d)", cap.form, network_status.reopen_status);
			DisplayMsg(string_buf);
		}

		network_direct(0);
		Ethernet_Server(CLIENT_CLOSE, 0, CLIENT_CLERK_TRANS);
	}
	else network_error(MSG_21_NET_ERR);
}

/*
INT8U network_cursale_check(INT8U clerkid)
{
    INT8U ret;
    INT8U ip[5];

    if (ethernet_list.status==0) return 0;
    if (ethernet_list.status==1) {
	net_get_clerksale(clerkid,ip);
	ret=0;
	if (ip[0]==0) ret=1;
	if (memcmp(ethernet_list.localip,ip,4)==0) ret=1;
	return ret;
    }
//    nretry=0;
    ret   =0;
//    while (!ret) {
//	    ret=Ethernet_Server(CLIENT_OPEN, 0, CLIENT_NORMAL);
//	    nretry++;
//	    if (nretry>6) break;
//    }
    network_status.check_lastscale=1;
    ret=Ethernet_Server(CLIENT_OPEN, 0, CLIENT_NORMAL);
    if (ret==1) {
	network_direct(1);
	network_timeout_reset(NETWORK_TIMEOUT_REQUEST_LOGIN);
	network_counter_reset(NETWORK_TIMEOUT_REQUEST_LOGIN);
	command_login_loaddata(&(CommBufEthData[0]),0x21,clerkid,NULL);
	network_status.check_lastscale=0;
	while (1) {
	    network_common_trans();
     	    if (network_status.check_lastscale) break;
	    if (network_timeout_check(8,NETWORK_TIMEOUT_REQUEST_LOGIN)) {
	    	if (network_counter_check(5,NETWORK_TIMEOUT_REQUEST_LOGIN)) {
		    command_login_loaddata(&(CommBufEthData[0]),0x21,clerkid,NULL); // Retry
    	        } else {
		    ret=0;
		    break;
		}
	    }
	}
	if (ret==0) {
		network_status.check_lastscale=2;
	} 
	if (network_status.check_lastscale==2) ret=1;
	else ret=0;
//	else ret=0;
	network_direct(0);
	Ethernet_Server(CLIENT_CLOSE, 0, CLIENT_NORMAL);
    } else {
	ret=1;
    }
    return ret;
}
*/

extern char	gmsg[];
extern INT8U display_message_printerr_page(INT16U L_msg, INT16U S_msg);
INT8U network_login(INT16U clerkid, INT8U scaleid, INT8U mode)
{
	INT8U ip[5];
	INT8U neterr;
	INT8U ret,nretry;
	INT8U lock_flag;
	INT8U ret_value;
	INT16U timeout_100ms;
	INT16U retry_count;    
	ret   =0;
	neterr=0;

	lock_flag=get_global_bparam(GLOBAL_SALE_SETUP_FLAG8);	//Added by JJANG 20080221
	lock_flag=(lock_flag>>4)&0x03;

	if ((Startup.global_com&0x01)==0) 
	{
		goto END;
	}
	if (Operation.operationClerk==OPER_FLOATING) 
	{
////		network_status.load_lastsale=0;
		switch (ethernet_list.status) 
		{
			case 0: 
				goto END;
			case 1:
				if( netGetClerkLock(clerkid, scaleid)) //Clerk이 Lock 일경우 master에서도 login 불가
				{
					BuzOn(2);
					if(lock_flag==2)	//Added by JJANG 20080221
					{
						ret_value = display_message_printerr_page(0xF214,0xCA03);
						if (ret_value) neterr = 2;
						else neterr = 1;
						break; 
					}
					else 
					{
						DisplayMsg((char *)global_message[MSG_CLERK_IS_LOCKED]); 
						neterr=1;
						break;
					}
				}
			case 3:
			case 4:
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
				net_get_clerksale(clerkid,ip);
				net_set_clerksale(clerkid,ethernet_list.localip);
#endif
				break;
			case 2:
				ret=0;
				nretry=0;
				while (!ret) 
				{
					ret=Ethernet_Server(CLIENT_OPEN, 0, CLIENT_CLERK_TRANS);
					nretry++;
					if (nretry>4) break;
				}
		     		neterr=0;
				if (ret==1) 
				{
					network_direct(1);
					network_timeout_reset(NETWORK_TIMEOUT_REQUEST_LOGIN);
					network_counter_reset(NETWORK_TIMEOUT_REQUEST_LOGIN);
					command_login_loaddata(&(CommBufEthData[0]),mode,(INT8U)clerkid,NULL);
					network_status.login_flag      = 1;
					if (mode==0x03) mode = 0x63; // 0x03 retry 시 0x63 command로 전송.
					network_status.load_lastsale   = 1;
					timeout_100ms = 40;  // 4000ms
					retry_count = 2;     // 총 3회 전송
					if (mode == 0x01) //network_status.load_lastsale = 0;
					{
						timeout_100ms = 10; // 1000ms
						retry_count = 2;    // 총 3회 전송(2회 재전송)
					}
					while (1) 
					{
						network_common_trans();
						switch (network_status.load_lastsale) 
						{
////							case 0:  // 수신 대기중(0x01)  
////								if (network_timeout_check(8,NETWORK_TIMEOUT_REQUEST_LOGIN)) 
////								{
////									if (network_counter_check(2,NETWORK_TIMEOUT_REQUEST_LOGIN)) 
////									{
////										command_login_loaddata(&(CommBufEthData[0]),0x01,(INT8U)clerkid,NULL); // Retry
////									} 
////									else 
////									{
////										neterr=3;	// timeout
////										break;
////									}
////								}
////								break;
							case 1:  // 수신 대기중
								if (network_timeout_check(timeout_100ms,NETWORK_TIMEOUT_REQUEST_LOGIN)) 
								{
									if (network_counter_check(retry_count,NETWORK_TIMEOUT_REQUEST_LOGIN))	// total 3 times 
									{
										command_login_loaddata(&(CommBufEthData[0]),mode,(INT8U)clerkid,NULL);
									} 
									else 
									{
										neterr=3;	// timeout
										break;
									}
								}
								break;
							case 2:  // OK.(clerk unlock) (receive 0x02, 0x44, 0x05)
								goto ENDX;
							case 3:  // clerk lock (receive 0x07)
								neterr=1;
								break;
							case 4:  // transaction 수신중(recieve 0x06)
								if (network_timeout_check(20,NETWORK_TIMEOUT_REQUEST_LOGIN)) 
								{
									if (network_counter_check(2,NETWORK_TIMEOUT_REQUEST_LOGIN)) 
									{
										command_login_loaddata(&(CommBufEthData[0]),0x33,(INT8U)clerkid,NULL);
									} 
									else 
									{
										neterr=3;	// timeout
										break;
									}
								}
								break;

						}
						if (neterr) break;
					}
ENDX:
					network_direct(0);
					Ethernet_Server(CLIENT_CLOSE, 0, CLIENT_CLERK_TRANS);
				} 
				else 
				{
					neterr=3;
				}
				if (neterr) 
				{
				        if(network_status.load_lastsale ==3) // if clerk is locked
					{
						BuzOn(4);	//se-hyung 20070803
						if(lock_flag==2)
						{
							ret_value = display_message_printerr_page(0xF214,0xCA03);
							if (ret_value) neterr = 2;
							else neterr = 1;
							break; 
						}
						else 
						{
							DisplayMsg((char *)global_message[MSG_CLERK_IS_LOCKED]);
							neterr = 1; 
						} 
					}
					else
					{
						//BuzOn(4);	//se-hyung 20070803
						network_error(MSG_21_NET_ERR);
						Delay_100ms(2);                        
					}
				}
				break;
		} // End Switch
	}
END:	ret=1;

	if (neterr == 1) ret = 0;	// clerk lock
	else if (neterr == 2) ret = 2;	// clerk lock 해제(clerk lock type 2일때)
	else if (neterr == 3) ret = 3;	// timeout
	return ret;
}

