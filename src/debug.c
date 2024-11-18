/*****************************************************************************
|*
|*  Copyright		:	(c) 2002 CAS
|*  Filename		:	debug.c
|*  Version			:	0.1
|*  Programmer(s)	:	Chung jong kwon (CJK)
|*  Created			:	2003/08/20
|*  Description		:	CLP main routine
|*
****************************************************************************/
#include <stdio.h>
#include "globals.h"
#include "commbuf.h"
#include "prt_common.h"
#include "prtdrv.h"
#include "timer.h"
#include "serial.h"
#include "flash.h"
#include "sensor.h"
#include "ram.h"
#include "prt_render.h"
#include "bar_common.h"
#include "prt_font.h"
#include "dsp_common.h"
#include "prt_interpreter.h"
#include "dsp_vfd7.h"
#include "dsp_app.h"
#include "adm.h"
#include "key.h"
#include "key_typedef.h"
#include "main.h"
#include "cal.h"
#include "prt_cmdtest.h"
#include "programmode.h"
// #include "file.h"
#include "rtc.h"
#include "debug.h"
#include "common.h"
#include "flash_app.h"
#include "mode_main.h"
#include "caption_app.h"
#include "mode_sale.h"
#include "ethernet.h"
#include "ethernet_comm.h"
#include "network_command.h"
#include "initial.h"
#include "commbuf.h"
#include "cashdrawer.h"
#include "ps2.h"
#include "prt_cmd.h"
#include "prt_app.h"
#include "mode_calibration.h"
#include "mode_commun.h"
#include "wlan_set_lantronix.h"
#include "traceability.h"
#include "license.h"
#include "sqi.h"
#include "spi_sram.h"
#include "plu_device.h"

INT16S   debug_w_mode;
long  debug_param[4];
short debug_nparam;
short debug_nchar;
short debug_cur_icmd;
short debug_cmd_count;
char  debug_first;
short debug_cmd_sid[30];	// 해석되는 command 의 [i]th 값을 저장,
				// 첫문자가 동일한 명령이 30개를 넘지 말아야 한다.
INT16S   debug_cmd_nid;		// debug_cmd_sid에 저장된 총량
				// debug_cmd_sid[i]=0;
short debug_stemp1;
short debug_cmd_max_count;

unsigned char debug_param_str[DBG_STR_BUF_SIZE];
short         debug_param_nstr;

void ModuleToHost(INT8U type);
void HostToModule(INT8U type);
extern void CalRun_InitADM(INT8U flag, INT8U backup);// Modified by CJK 20050816
extern void plu_struct_set_base(void);
extern void commun_out(HUGEDATA COMM_BUF *CBuf,char *str);
extern void commun_outleng(HUGEDATA COMM_BUF *CBuf,char *str,INT16S leng);
extern void EthDspMyInform(HUGEDATA COMM_BUF *CBuf,INT8U mode);
extern void menu_display_menucode(INT16U code,INT8U set);
extern void Flash_program_data(INT32U start_addr, HUGEDATA INT8U *data, INT32U byte_count);
extern void Flash_program_data_c2(INT32U start_addr, HUGEDATA INT8U *data, INT32U byte_count);
extern void plu_all_delete(HUGEDATA COMM_BUF *CBuf,INT8U sendmode);
extern INT8U Ethernet_Command(INT8U s, INT8U cmd_type);
extern INT8U global_one2one_send;
extern INT16U global_one2one_count;

void debug_init(void)
{
	debug_w_mode=DBG_CMD_START;
	debug_param[0]=0;
	debug_param[1]=0;
	debug_param[2]=0;
	debug_param[3]=0;
	memset(debug_param_str,0,DBG_STR_BUF_SIZE);
}
extern INT16U SysBootTime;
void check_delay_time(INT16S ch)
{
	static INT16U oldSys=0;
	static INT16U startTime=0;
	INT16U  cur_wait;

	if (ch==0) startTime=SysTimer100ms;
	if (SysTimer100ms!=oldSys) {
		if (startTime==0) startTime=SysTimer100ms;
	}
	cur_wait=SysTimer100ms-startTime;
	if (ch) {
		if (ch>cur_wait) Delay_100ms(ch-cur_wait);
	}
	startTime=SysTimer100ms;
	oldSys=SysTimer100ms;

	// FTL의 안전한 BACKUP을 위한 최소 충전시간 체크 (4초)
	FtlCheckBackupTime();
}
INT8U debug_tic_on = OFF;

//SGTEST
INT8U debug_tic(char ch)
{
	char str[16];
	static INT16U oldSys=0;
	static INT16U startTime=0;

	if (!debug_tic_on && ch) return 0;
	if (ch==0) {
		startTime =PrtDrvStruct.SysTimer;
		sprintf(str,"\r\n");
		debug_tic_on = ON;
	} else if (PrtDrvStruct.SysTimer!=oldSys) {
		if (startTime==0) startTime=PrtDrvStruct.SysTimer;
		sprintf(str," %c=%d.",ch,PrtDrvStruct.SysTimer-startTime);
	} else {
		sprintf(str,"%c.",ch);
	}
	MsgOut(str);
	oldSys=PrtDrvStruct.SysTimer;
	return 0;
}


/*
INT8U debug_tic(char ch)
{
	char str[16];
	static INT16U oldSys=0;
	static INT16U startTime=0;
	if (ch==0) {
		startTime=PrtDrvStruct.Timer10ms;
		sprintf(str,"\r\n");
	} else if (PrtDrvStruct.Timer10ms!=oldSys) {
		if (startTime==0) startTime=PrtDrvStruct.Timer10ms;
		sprintf(str," %c=%d.",ch,PrtDrvStruct.Timer10ms-startTime);
	} else {
		sprintf(str,"%c.",ch);
	}
	MsgOut(str);
	oldSys=PrtDrvStruct.Timer10ms;
	return 0;
}
*/

void send_crlf(HUGEDATA COMM_BUF *CBuf)
{
	char ss[4];
	sprintf(ss, "\r\n");
	commun_out(CBuf,ss);
}

extern void  serial_timeout_reset(INT8U com_type);
extern INT8U serial_timeout_check(INT8U com_type);
extern INT16S   serial_get_comtype(HUGEDATA COMM_BUF *CBuf);
extern void  printer_sensor_keytest(INT16U *t1,INT16U *t2,INT8U fontid);
extern void  printer_test(void);
extern char  gmsg[];
extern INT8U commun_setrtc(INT8U *str,INT16S leng);

#ifdef USE_EXT_SETTING_MEM
INT8U Data_Save_Restore(INT8U restore)
{
	INT16U icc;
	INT32U chk,chkaddr,src_addr,dst_addr,dat_size,chkvalue,block,nblock,rblock,i;
	HUGEDATA INT8U *data;
	char  temp[32];
	icc=FlashCheckMem();
	//if (icc==0xf0) {
	if (icc==0xf0 || restore == 8) {	// Add other condition(for data setting jig memory) : Added by CJK 20051229
		dat_size = 0x200000l - 4l;
		chkvalue = 0xaa55aa00l;
		chkaddr = EXT_MEMCARD_ADDR_X+dat_size;
		switch (restore) {
		    case 1: sprintf(temp,"Restore !!\r\n");
			    MsgOut(temp);
			    chk=Flash_lread(chkaddr);
			    if (chk!=chkvalue) {
				sprintf(temp,"-- No Back ROM\r\n");
				MsgOut(temp);
				return 0;
			    }
			    dat_size += 4l;
			    src_addr =EXT_MEMCARD_ADDR_X;
			    data = (HUGEDATA INT8U *)EXT_MEMCARD_ADDR_X;
			    dst_addr = DFLASH_BASE;
			    sprintf(temp,"Restore");
			    break;
		    case 0: data = (HUGEDATA INT8U *)DFLASH_BASE;
			    src_addr = DFLASH_BASE;
			    dst_addr = EXT_MEMCARD_ADDR_X;
			    Flash_flush_nvbuffer();
			    sprintf(temp," Backup");
			    break;
		    default:
		    case 3: // only check Normal Flash
			    chkaddr = DFLASH_BASE + dat_size;
			    chk=Flash_lread(chkaddr);
			    chk=Flash_lread(chkaddr);
			    if (chk==chkvalue) return 1;
			    return 0;
		    case 2: // only check Ext    Flash
			    chk=Flash_lread(chkaddr);
			    if (chk==chkvalue) return 1;
			    return 0;
		    case 9: chk=Flash_lread(chkaddr);
		    	    if (chk==chkvalue) {
		    	    	Flash_lwrite(chkaddr,0l);
			    	sprintf(temp,"Backup FLASH to Normal Flash\r\n");
			    } else {
			    	sprintf(temp,"Already Normal Flash\r\n");
			    }
			    MsgOut(temp);
			    return 0;
		    case 8: chkaddr = DFLASH_BASE + dat_size;
			    chk=Flash_lread(chkaddr);
		    	    if (chk==chkvalue) {
			    	sprintf(temp,"Already Normal Flash\r\n");
			    } else {
		    	    	Flash_lwrite(chkaddr,chkvalue);
			    	sprintf(temp,"Basic FLASH to Normal Flash\r\n");
			    }
			    MsgOut(temp);
			    return 0;
		}
		MsgOut(temp);
		data = (HUGEDATA INT8U*)src_addr;
		block = 0x2000;
		nblock= dat_size / block;
		rblock= dat_size % block;
		display_string(DISPLAY_WEIGHT   ,(INT8U *)temp);

		Flash_sread(src_addr+FLASH_STR_VERSION,Startup.version,2);
		sprintf(temp,"%2d.%03d",Startup.version[0]-'0',Startup.version[1]-'0');
		display_string(DISPLAY_PRICE,(INT8U *)temp);
		VFD7_Diffuse();
		for (i=0; i<nblock; i++) {
			Flash_program_data(dst_addr,data,block);
			sprintf(temp,"%06lx",i);
			display_string(DISPLAY_UNITPRICE,(INT8U *)temp);
			VFD7_Diffuse();
			dst_addr+=block;
			data    +=block;
		}
		if (rblock) {
			Flash_program_data(dst_addr,data,rblock);
		}
		sprintf(temp,"%06lx",i);
		display_string(DISPLAY_UNITPRICE ,(INT8U *)temp);
		VFD7_Diffuse();
		if (!restore) {
			Flash_lwrite(chkaddr,chkvalue);
		}
		Flash_flush_nvbuffer();
		if (restore==1) {
			sprintf(temp,"CLEAR");
			display_string(DISPLAY_WEIGHT   ,(INT8U *)temp);
			sprintf(temp,"-NVRAM");
			display_string(DISPLAY_UNITPRICE,(INT8U *)temp);
			VFD7_Diffuse();
			nvram_clear();
		}
		sprintf(temp,"--End--");
		MsgOut(temp);
		display_string(DISPLAY_UNITPRICE,(INT8U *)temp);
		VFD7_Diffuse();
	} else {
//		sprintf(temp,"Cannot Found ext memory\r\n");
//		MsgOut(temp);
	}
	return 0;
}
#endif

void ad_firmware_download(void)	// Added by CJK 20050614
{
	INT8U check_admSetBaudRate;

	check_admSetBaudRate = ADM_ScanBaudrate(BAUD_RATE_57600, BAUD_RATE_38400);
	if(check_admSetBaudRate == FALSE) {
		BuzOn(4);
		return;
	}
	//ADM_ChangeBaudrate(BAUD_RATE_38400);	// set baudrate
	Uart1Init(BAUD_RATE_38400);		// restore baudrate to
	Uart0Init(BAUD_RATE_38400);
	BuzOn(2);
	while(1)
	{
		ModuleToHost(2);
		HostToModule(2);
		network_tx_proc();
		if(KEY_IsKey()) {
			BuzOn(1);
			KEY_InKey();
			break;
		} else if(PS2_IsKey()) {
			BuzOn(1);
			PS2_InKey();
			break;
		}
	}
	ADM_ChangeBaudrate(BAUD_RATE_57600);	// set baudrate
	Uart1Init(BAUD_RATE_57600);		// restore baudrate to
	Uart0Init(get_global_bparam(GLOBAL_AREA_RS232C_BAUD));
}

#ifdef USE_CHN_IO_BOARD
#ifdef USE_I2C_DEBUG
void i2c_to_uart(void)	// Added by CJK 20050614
{
	while(1)
	{
		ModuleToHost(COMM_TYPE_RFID);
		HostToModule(COMM_TYPE_RFID);
		network_tx_proc();
		i2c_rxDataProc();
		if(KEY_IsKey()) {
			BuzOn(1);
			KEY_InKey();
			break;
		} else if(PS2_IsKey()) {
			BuzOn(1);
			PS2_InKey();
			break;
		}
	}
}
#endif
#endif

void DebugDirectly(INT8U type)
{
    if(type < 2 || type > 6) return; //add key, usb
	while(1)
	{
		ModuleToHost(type);
		HostToModule(type);
		network_tx_proc();
		if(type != 5) {//key
			if(KEY_IsKey()) {
				BuzOn(1);
				KEY_InKey();
				break;
			} else if(PS2_IsKey()) {
				BuzOn(1);
				PS2_InKey();
				break;
			}
		}
	}
}

//DELvoid DebugIndirectly(INT8U type)
//DEL{
//DEL	if(type < 2 || type > 4) return;
//DEL	while(1)
//DEL	{
//DEL		ModuleToHost(type);
//DEL		HostToModule(type);
//DEL		network_tx_proc();
//DEL//		if(KEY_Read()){
//DEL//			if(KeyDrv.CnvCode == KS_CLEAR) break;
//DEL//		}
//DEL		if(KEY_IsKey()) {
//DEL			BuzOn(1);
//DEL			KEY_InKey();
//DEL			break;
//DEL		}
//DEL	}
//DEL}

//DEL20060309extern void Test_MakeFile(INT32U, INT16U, INT8U);
//DEL20060309extern void Test_MakePlu(INT32U, INT16U, INT32U);
//DEL20060309extern void Test_DeleteFile(INT32U);
//DEL20060309extern void Test_ListFile(void);
//DEL20060309extern void Test_CheckFile(INT32U key_code, INT16U b_size, INT8U fill_data);
//#define DEBUG_BARFORM_FOR_SCAN
#ifdef DEBUG_BARFORM_FOR_SCAN
STRUCT_STRFORM_PARAM test_bar_param;
char test_scan_data[32];
char test_scan_form[32];
#endif

INT16S debug_interprete(HUGEDATA COMM_BUF *CBuf,INT8U r_data)
{
	const DEBUG_TYPE cmdtype[]={
			{"?" ,DBG_CMD_HP,0,"this command"},
			{"e1",DBG_CMD_E1,0,"Check Ethernet : e1"},
			{"e2",DBG_CMD_E2,1,"Send Command: e2 <param0>"},
			{"e3",DBG_CMD_E3,0,"Network Status: e3"},
#ifndef CPU_PIC32MZ
			{"w1",DBG_CMD_W1,0,"Wireless Lan Status: w1"},
			{"w2",DBG_CMD_W2,0,"Scan SSID: w2"},
#endif
			{"wb",DBG_CMD_WB,0,"Telnet to Bridge: wb"},
			{"lc",DBG_CMD_LC,1,"Lock: lc <param0>"},
			{"sf",DBG_CMD_SF,1,"Change Start Flag : sf <param0>"},
#ifndef CPU_PIC32MZ
			{"ef",DBG_CMD_EF,1,"Set Mac: ef <param0>"},
#endif
			{"mr",DBG_CMD_MR,2,"memory read : mr <start addr> <leng>"},
			{"ms",DBG_CMD_MS,2,"memory read(NVRAM) : ms <start addr> <leng>"},
#ifndef CPU_PIC32MZ
			{"mw",DBG_CMD_MW,9,"memory write : mw <start addr> <leng> <hexbyte array>"},
			{"mf",DBG_CMD_MF,3,"memory fill : mf <start addr> <leng> <hexbyte>"},
			{"ma",DBG_CMD_MA,2,"memory read with ASCII : ma <start addr> <leng>"},
#endif
			{"br",DBG_CMD_BR,1,"set A/D baudrate : br [0-4]"},
			{"cf",DBG_CMD_CF,1,"Clear Flash : cf"},
			{"cr",DBG_CMD_CR,0,"read RTC clock : cr"},
			{"cw",DBG_CMD_CW,2,"write RTC clock : cw <param0> <param1>"},
			{"dd",DBG_CMD_DD,1,"Commun. Debug Direct : dd <com_type>"},
			{"af",DBG_CMD_AF,0,"ad firmware download : af"},
			{"ac",DBG_CMD_AC,0,"Mornitoring count clear : ac"},
			{"fl",DBG_CMD_FL,0,"Status of FLASH2(CL5200J) : fl"},
			{"ft",DBG_CMD_FT,1,"FTL Table : ft [0|1|99] 0:summary,1:detail,99:default"},
#ifndef CPU_PIC32MZ
			{"fm",DBG_CMD_FM,0,"Check Mem Card : fm"},
#endif
			{"ia",DBG_CMD_IA,0,"Init ADM : ia"},
#ifdef USE_EXT_SETTING_MEM
			{"is",DBG_CMD_IS,0,"Backup to Ext Memory : is"},
			{"ir",DBG_CMD_IR,0,"Restore from Ext Memory : ir"},
			{"ii",DBG_CMD_II,0,"Init Ext memory"},
			{"ib",DBG_CMD_IB,0,"Flag Basic memory"},
#endif
			{"pd",DBG_CMD_PD,0,"set printer default : pd"},
			{"pi",DBG_CMD_PI,0,"PLU initialize : pi"},
#ifdef TEST_REPORT
			{"pr",DBG_CMD_PR,2,"Report PLU Summary : pr <param0> <param1>"},
#endif
			{"pw",DBG_CMD_PW,8,"Password : pw"},
			{"qt",DBG_CMD_QT,0,"quit : qt"},
			{"ks",DBG_CMD_KS,3,"set key table : ks <param0> <param1> <param2>"},
			{"ad",DBG_CMD_AD,1,"adm debug flag on/off : ad <param0>"},
			{"vf",DBG_CMD_VF,0,"view font list : vf"},
			{"ci",DBG_CMD_CI,1,"change client ip : ci <slave id>"},
#ifndef CPU_PIC32MZ
			{"nw",DBG_CMD_NW,9,"net write : nw <start addr> <leng> <hexbyte>"},
			{"nr",DBG_CMD_NR,2,"net read : nr <start addr> <leng>"},
#endif
#ifdef DEBUG_BARFORM_FOR_SCAN
			{"sm",DBG_CMD_SM,8,"scan form : sm <leng> <string>"},
			{"sd",DBG_CMD_SD,8,"scan data : sd <leng> <string>"},
#endif
			{"ct",DBG_CMD_CT,1,"close test : ct <socket no>"},
			{"gs",DBG_CMD_GS,0,"tcp state : gs"},
#ifdef USE_MULTI_TRACE_NO_DBG
			{"tn",DBG_CMD_TN,0,"trace no table : tn"},
			{"tr",DBG_CMD_TR,0,"read trace no table : tr"},
#endif
#ifdef USE_CHN_IO_BOARD
			{"ic",DBG_CMD_IC,0,"I2C test : ic"},
#endif
#ifdef USE_CHN_CART_SCALE
			{"cv",DBG_CMD_CV,0,"CHN Vegetable trace : cv"},
#endif
#ifdef USE_SRAM_SPI_COMM
            {"sr",DBG_CMD_SR,3,"Serial SRAM Test : sr <Parm0>   (1:Set MR, 2:Read/Write)"},
#endif
#ifdef USE_CL5200N_SFLASH_DEBUGGING
            {"df8m",DBG_CMD_DF8M,1,"S_Flash Test : df8m <Parm0> (1:DataFlsh , 2:NVRAM)"},
#endif
#ifdef USE_CPU_LOAD_TEST
            {"nm",DBG_CMD_NM,1,"S_Flash  Test : nm <Parm0>  ( 0:None   1:NVRAM_TEST )"},
#endif
			{"\0",0 ,0,"\0"},
	};
	INT16S  i,z,ret;

	ret=0;
	switch (debug_w_mode) {
		case DBG_CMD_START :
			if ((INT8U)r_data==(INT8U)0xff) {
				debug_cmd_max_count=0;
				for (i=0; i<255; i++) {
					if (cmdtype[i].name[0]==0) break;
					debug_cmd_max_count++;
				}
				//ret=-1;
			}
			debug_init();
			ret=-1;
			if ((r_data==0x0d) || (r_data==0x0a)) {

				break;
			}
			debug_cmd_nid=0;
			//ret=-1;
			for (i=0; i<debug_cmd_max_count; i++) {
				if (cmdtype[i].name[0]==r_data) {
					debug_cmd_sid[debug_cmd_nid++]=i+1;
					debug_w_mode=DBG_CMD_INTER;
					debug_nchar = 1;
					ret=0;
				}
			}
			break;
		case DBG_CMD_INTER:
			if(r_data==0x1b || r_data==0x08) {	// Cancel
				debug_w_mode=DBG_CMD_START;
				ret=-2;
				break;
			} else if (r_data<=0x20) {
				debug_cur_icmd=0;
				for (i=0; i<debug_cmd_nid; i++) {
					if (debug_cmd_sid[i]!=0) {
						debug_cur_icmd=debug_cmd_sid[i];
					}
				}
				if (debug_cur_icmd) {
					debug_nparam=0;
					debug_cur_icmd--;
					if (cmdtype[debug_cur_icmd].nparam) {
						debug_w_mode=DBG_CMD_PARAM;
					} else {
						if ((r_data==0x0d) || (r_data==0x0a)) {
							debug_w_mode=DBG_CMD_EXEC;
							goto DEBUG_EXEC;
						}
					}
				} else {
					debug_w_mode=DBG_CMD_START;
					ret=-3;
				}
				break;
			}
			debug_cmd_count=0;
			for (i=0; i<debug_cmd_nid; i++) {
				if (cmdtype[ debug_cmd_sid[i]-1 ].name[debug_nchar]!=r_data) {
					debug_cmd_sid[i]=0;
				} else {
					debug_cmd_count++;
				}
			}
			debug_nchar++;
			if (debug_cmd_count==0) {	// 없는 Cmd(첫번째 Char는 일치하나 두번째가 일치하지 않음)
				debug_w_mode=DBG_CMD_START;
				ret=-4;
			}
			break;
		case DBG_CMD_PARAM:
			if ((r_data==0x0d) || (r_data==0x0a)) {
				debug_w_mode=DBG_CMD_EXEC;
				goto DEBUG_EXEC;
			} else if(r_data==0x1b || r_data==0x08) {	// Cancel	//CJK
				debug_w_mode=DBG_CMD_START;
				ret=-2;
				break;
			}
			if ((debug_first==0) && ((r_data==0x09) || (r_data==0x20))) break;
			if ((debug_first) && ((r_data==0x09) || (r_data==0x20))) {
				debug_first=0;
				debug_nparam++;
				z=cmdtype[debug_cur_icmd].nparam;
				if ((z==9) && (debug_nparam==2)) { // nparam=9 : "cmd <addr> <leng> <string>"
					debug_param_nstr=0;
					debug_stemp1 = debug_param[1]+debug_param[1];
					debug_w_mode=DBG_CMD_SPARM;
					break;
				}
				if ((z==8) && (debug_nparam==1)) { // nparam=8 : "cmd <leng> <string>"
					debug_param_nstr=0;
					debug_stemp1 = debug_param[0];
					debug_w_mode=DBG_CMD_CPARM;
					break;
				}
				debug_param[debug_nparam]=0;
				break;
			}
			debug_first=1;
			if (debug_nparam>=cmdtype[debug_cur_icmd].nparam) {
				debug_w_mode=DBG_CMD_START;
				ret=-5;
				break;
			}
//			sprintf(MsgBuf,"%d,",debug_nparam);
//			MsgOut(MsgBuf);

			debug_param[debug_nparam]<<=4;
			debug_param[debug_nparam]|=ctoh(r_data);
			break;
		case DBG_CMD_SPARM:
			if ((r_data==0x0d) || (r_data==0x0a)) {
				debug_w_mode=DBG_CMD_EXEC;
				goto DEBUG_EXEC;
			}
			if ((debug_first==0) && ((r_data==0x09) || (r_data==0x20))) break;
			debug_first=1;
			z=debug_param_nstr;
			z>>=1;
			if (debug_param_nstr>=debug_stemp1) break;
			debug_param_str[z]<<=4;
			debug_param_str[z]|=ctoh(r_data);
			debug_param_nstr++;
			break;
		case DBG_CMD_CPARM:
			if ((r_data==0x0d) || (r_data==0x0a)) {
				debug_w_mode=DBG_CMD_EXEC;
				goto DEBUG_EXEC;
			}
			if ((debug_first==0) && ((r_data==0x09) || (r_data==0x20))) break;
			debug_first=1;
			if (debug_param_nstr>=debug_stemp1) break;
			debug_param_str[debug_param_nstr] = r_data;
			debug_param_nstr++;
			break;
	}
	return ret;
DEBUG_EXEC:
	debug_w_mode=DBG_CMD_START;
	if (cmdtype[debug_cur_icmd].cmdmode==DBG_CMD_HP) {
		for (i=0; i<debug_cmd_max_count; i++) {
			sprintf(MsgBuf,"\r\n%s : %s",cmdtype[i].name,cmdtype[i].help);
			commun_out(CBuf,MsgBuf);
		}
	}
	return cmdtype[debug_cur_icmd].cmdmode;
}

void debug_message(char *buf)
{
	char s_buf[10];
	sprintf(s_buf,"%s",buf);
	if (Startup.start_flag&0x10) MsgOut((HUGEDATA char*)s_buf);
}

void debug_message2(char *buf)
{
	char s_buf[10];
	sprintf(s_buf,"%s",buf);
	if (Startup.start_flag&0x200) MsgOut((HUGEDATA char*)s_buf);//HYp 20041222
}

//#define DEBUG_PASS_CH_NUM	5
//ROMDATA DebugPassword[DEBUG_PASS_CH_NUM] = {'8','3','4','1','9'};
#define DEBUG_QUIT_CH_NUM	2
ROMDATA INT8U DebugQuit[DEBUG_QUIT_CH_NUM] = {'q','t'};
extern void commun_write_err16(HUGEDATA COMM_BUF *CBuf,INT8U cmd,INT16U ret16,INT8U chk,INT8U noheader);
extern HUGEDATA INT8U  recv_display;

void  DebugBackup(void)
{
	INT16U old_run;
	POINT  p;
	INT8U  first;

//	INT16S   timeout;
	old_run = status_run.run_mode;
	//VFD7_save_restore(VFD_BACKUP);
	display_save_restore(DSP_BACKUP);
	display_alloff_primary_indication();
	Dsp_SetPage(DSP_ERROR_PAGE);
	Dsp_Fill_Buffer(0);
	DspLoadFont1(DSP_PLU_FONT_ID);
	p = point(8, 0);
	sprintf(MsgBuf,"[Receive...]");
	Dsp_Write_String(p,MsgBuf);
	Dsp_Diffuse();
	VFD7_Diffuse();
	global_one2one_count=0;
	network_status.load_plu=0;
	first = 0;
	while (1) {
		network_check();
		if (first==0) {
			if (network_status.load_plu) {
				display_clear(DISPLAY_UNITPRICE);
				sprintf(MsgBuf,"S-PLU");
				display_string(DISPLAY_UNITPRICE,(INT8U *)MsgBuf);
				first=1;
			}
		}
		if (global_one2one_send==9)  break;
		if (KEY_Read()) {
			BuzOn(1);
			if (KeyDrv.CnvCode==KS_CLEAR) {
				commun_write_err16(&CommBuf,0x29,9,0,OFF);
				if (old_run==RUN_BACKUP) old_run=RUN_SALE;
				goto END;
			}
		}
	}
END:;
	status_run.run_mode = old_run;
	//VFD7_save_restore(VFD_RESTORE);
	display_save_restore(DSP_RESTORE);
	display_sign_noflush(SIGN_NET,0);
	VFD7_Diffuse();
	Dsp_SetPage(DSP_DEFAULT_PAGE);
	DspLoadFont1(DSP_MENU_FONT_ID);
	menu_display_menucode(0,0);
	Dsp_Diffuse();
	sale_display_update(UPDATE_ALL|UPDATE_CLEARALL);
}

#ifdef DEBUG_VIEW_FONTLIST
#include "dsp_common.h"
#include "prt_font.h"
extern HUGEDATA DSP_FONT1_STRUCT DspFont1Struct[MAX_DSP_FONT1_NUM];
extern HUGEDATA DSP_FONT2_STRUCT DspFont2Struct[MAX_DSP_FONT2_NUM];
extern HUGEDATA FONT1_STRUCT Font1Struct[MAX_FONT1_NUM];
extern HUGEDATA FONT2_STRUCT Font2Struct[MAX_FONT2_NUM];
void ViewFontList(void)
{
	INT16U id;

	sprintf(MsgBuf, "<Dsp:1byte font>\r\n");
	MsgOut(MsgBuf);
	for (id = 0; id < MAX_DSP_FONT1_NUM; id++)
	{
		if (DspFont1Struct[id].ExistFont)
		{
			//sprintf(MsgBuf, " [%d] %s (W%dxH%d)\r\n", id, DspFont1Struct[id].Name1, DspFont1Struct[id].Wbits1, DspFont1Struct[id].Hbits1);
			sprintf(MsgBuf, " [%d] (W%dxH%d)\r\n", id, DspFont1Struct[id].Wbits1, DspFont1Struct[id].Hbits1);
			MsgOut(MsgBuf);
		}
	}
	sprintf(MsgBuf, "<Dsp:2byte font>\r\n");
	MsgOut(MsgBuf);
	for (id = 0; id < MAX_DSP_FONT2_NUM; id++)
	{
		if (DspFont2Struct[id].ExistFont)
		{
			sprintf(MsgBuf, " [%d] (W%dxH%d)\r\n", id, DspFont2Struct[id].Wbits2, DspFont2Struct[id].Hbits2);
			MsgOut(MsgBuf);
		}
	}
	sprintf(MsgBuf, "\r\n<Prt:1byte font>\r\n");
	MsgOut(MsgBuf);
	for (id = 0; id < MAX_FONT1_NUM; id++)
	{
		if (Font1Struct[id].ExistFont)
		{
			sprintf(MsgBuf, " [%d] (W%dxH%d)\r\n", id, Font1Struct[id].Wbits1, Font1Struct[id].Hbits1);
			MsgOut(MsgBuf);
		}
	}
	sprintf(MsgBuf, "<Prt:2byte font>\r\n");
	MsgOut(MsgBuf);
	for (id = 0; id < MAX_FONT2_NUM; id++)
	{
		if (Font2Struct[id].ExistFont)
		{
			sprintf(MsgBuf, " [%d] (W%dxH%d)\r\n", id, Font2Struct[id].Wbits2, Font2Struct[id].Hbits2);
			MsgOut(MsgBuf);
		}
	}
}
#endif

#ifndef CPU_PIC32MZ
#include "system_config.h"
#include "tcpip/src/tcpip_heap_alloc.h"
#if defined(DEBUG_TCPIP_STACK_DRAM_CAS)
extern TCPIP_HEAP_HANDLE       initialHeapH;
static int debugTcpipHeapInfo(HUGEDATA COMM_BUF *CBuf)
{
    int     ix, nTraces, nEntries;
    TCPIP_HEAP_TRACE_ENTRY    tEntry;
    size_t  heapSize;

    heapSize = TCPIP_HEAP_Size(initialHeapH);
    sprintf(MsgBuf, "Initial created heap size: %d Bytes\r\n", heapSize);
    commun_out(CBuf,MsgBuf);
    sprintf(MsgBuf, "Allocable block heap size: %d Bytes\r\n", TCPIP_HEAP_MaxSize(initialHeapH));
    commun_out(CBuf,MsgBuf);
    sprintf(MsgBuf, "All available heap size: %d Bytes\r\n", TCPIP_HEAP_FreeSize(initialHeapH));
    commun_out(CBuf,MsgBuf);
    sprintf(MsgBuf, "Last heap error: 0x%x\r\n", TCPIP_HEAP_LastError(initialHeapH));
    commun_out(CBuf,MsgBuf);

    nTraces = TCPIP_HEAP_TraceGetEntriesNo(initialHeapH, true);
    if(nTraces)
    {
        sprintf(MsgBuf, "Trace info: \r\n");
        commun_out(CBuf,MsgBuf);
        nEntries = TCPIP_HEAP_TraceGetEntriesNo(initialHeapH, false);
        for(ix = 0; ix < nEntries; ix++)
        {
            if(TCPIP_HEAP_TraceGetEntry(initialHeapH, ix, &tEntry))
            {
                sprintf(MsgBuf, "Module: %4d, totAllocated: %5d, currAllocated: %5d, totFailed: %5d, maxFailed: %5d \r\n", tEntry.moduleId, tEntry.totAllocated, tEntry.currAllocated, tEntry.totFailed, tEntry.maxFailed);
                commun_out(CBuf,MsgBuf);
            }

        }
    }
    else
    {
        sprintf(MsgBuf, "No Trace info exists.\r\n");
        commun_out(CBuf,MsgBuf);
    }

    nEntries = TCPIP_HEAP_DistGetEntriesNo(initialHeapH);
    if(nEntries)
    {
        int     modIx;
        TCPIP_HEAP_DIST_ENTRY distEntry;
        int currLowHitMem = 0;
        int currHiHitMem = 0;

        sprintf(MsgBuf, "TCPIP Heap distribution: \n\r");
        commun_out(CBuf,MsgBuf);

        for(ix = 0; ix < nEntries; ix++)
        {
            TCPIP_HEAP_DistGetEntry(initialHeapH, ix, &distEntry);

            int entryPrint = 0;
            struct moduleDist* pMDist = distEntry.modDist;
            for(modIx = 0; modIx < sizeof(distEntry.modDist)/sizeof(*distEntry.modDist); modIx++, pMDist++)
            {
                if(pMDist->modHits)
                {
                    if(entryPrint == 0)
                    {
                        sprintf(MsgBuf, "[%4d,    %5d]:\n\r", distEntry.lowLimit, distEntry.highLimit);
                        commun_out(CBuf,MsgBuf);
                        sprintf(MsgBuf, "\tcurr hits: %d, \n\r", distEntry.currHits);
                        commun_out(CBuf,MsgBuf);
                        currLowHitMem += distEntry.currHits * distEntry.lowLimit;
                        currHiHitMem += distEntry.currHits * distEntry.highLimit;
                        entryPrint = 1;
                    }
                    sprintf(MsgBuf, "\t mod: %d, \thits: %d, \n\r", pMDist->modId, pMDist->modHits);
                    commun_out(CBuf,MsgBuf);
                }
            }
            if(distEntry.gHits)
            {
                sprintf(MsgBuf, "\t mod: xx \thits: %d, \n\r", distEntry.gHits);
                commun_out(CBuf,MsgBuf);
            }
        }

        sprintf(MsgBuf, "curr Low Lim: %d, curr Hi Lim: %d, max Free: %d, min Free: %d\n\r", currLowHitMem, currHiHitMem, heapSize - currLowHitMem, heapSize - currHiHitMem);
        commun_out(CBuf,MsgBuf);
    }

    return true;
}
#endif//#if defined(DEBUG_TCPIP_STACK_DRAM_CAS)
#endif//#ifndef CPU_PIC32MZ


extern void change_dest_ip_val(INT32U);
#ifdef USE_MULTI_TRACE_NO_DBG
extern void TNT_TestTraceNoTable(void);
extern void TNT_ReadTraceNoTable(void);
#endif
extern INT16U unknownSockCnt;	// for Debug 090515
extern INT8U unknownScokState;
extern INT16U debugSockCnt;
extern INT8U debugSockState;
extern void wlan_RebootModule(INT8U wtype);	// for Debug
extern void FtlDataBackupSetDefault(void);
extern void upload_ftl_table(HUGEDATA COMM_BUF *CBuf, INT32U mode);
#define MAX_DEBUG_BUF 0x200
void   __attribute__((optimize("-O0"))) debug_Factory(void);

void DebugTerminal(HUGEDATA COMM_BUF *CBuf)
{
	INT8U  r_data;
	INT16S a_mode;
// ----------------------------------------
//	INT8U cnt;//, loop, status;
// ----------------------------------------
	HUGEDATA INT8U *pMem;
	INT32U i;
	POINT  p;
	char ss[10];//HYP 20050107//ss[5];
	char pass[12];
	INT16U old_run, version;
	INT8U passcnt,qtcnt,cnt,PASS_CNT;//HYP 20050107
	INT8U buffer[HISTORY_BUF_SIZE];
#ifdef CPU_PIC32MZ
	INT8U debug_buf[MAX_DEBUG_BUF];
#endif
#ifdef USE_SRAM_SPI_COMM
    INT8U data = 'T';
    INT8U j = 0;
    INT8U resultData[20];
#endif
	INT32S errFlag;

	old_run = status_run.run_mode;
	send_crlf(CBuf);
	sprintf(MsgBuf, "Start Debug Terminal\r\n");
	commun_out(CBuf,MsgBuf);

	display_alloff_primary_indication();
	VFD7_Diffuse();
	DspStruct.DirectDraw = 0;

	Dsp_SetPage(DSP_ERROR_PAGE);
	VFD7_Fill_Buffer(0);
	Dsp_Fill_Buffer(0);
	DspLoadFont1(DSP_PLU_FONT_ID);
	p = point(0, 0);
	sprintf(MsgBuf,"Console Mode");
	Dsp_Write_String(p,MsgBuf);
	Dsp_Diffuse();
	//	DspStruct.DirectDraw = 1;
	debug_w_mode=DBG_CMD_START;
	debug_interprete(CBuf,0xff);
	network_tx_proc();
	CASInterpreter(&CommBufPrt);
	status_run.run_mode = RUN_DEBUG;
	//HYP 20050107 Insert Password
PASS_START:
	passcnt = 0;
	qtcnt = 0;
	cnt = 0;
	send_crlf(CBuf);
	if (CBuf->Type==COMM_TYPE_HOST) goto TERM_START;
	sprintf(ss,"password:");
	commun_outleng(CBuf,(char *)ss,9);
	i  = DFLASH_BASE + FLASH_START_PASSWORD;
	Flash_sread(i,(HUGEDATA INT8U *)&pass[2],DEALER_PASSWORD_LEN);pass[DEALER_PASSWORD_LEN]=0;
	pass[0]='0';
	pass[1]='0';
	PASS_CNT = strlen(pass);
	while(1) {
		if (CheckRxBufCount(CBuf, 1)) {
			r_data = GetCharRxBuf(CBuf);
			ss[0]='*';
			if(r_data>=0x20) {
				commun_outleng(CBuf,(char *)ss,1);
			}
			if ((r_data == 0x0d) || (r_data == 0x0a)) {
				if(cnt == PASS_CNT && passcnt == PASS_CNT) goto TERM_START;
				if(cnt == DEBUG_QUIT_CH_NUM && qtcnt   == DEBUG_QUIT_CH_NUM ) goto TERM_END;
				goto PASS_START;
			}
			if (r_data == pass[passcnt]) passcnt++;
			if (r_data == DebugQuit[qtcnt]) qtcnt++;
			cnt++;
			if(cnt > 20) goto PASS_START;
		}
		network_tx_proc();
	}
TERM_START:
	debug_w_mode=DBG_CMD_START;
	send_crlf(CBuf);
	ss[0]='>';
	commun_outleng(CBuf,(char *)ss,1);
	while(1) {
		if (CheckRxBufCount(CBuf, 1)) {
			r_data = GetCharRxBuf(CBuf);
//			S0BUF  = r_data;
			ss[0]=r_data;
			if(r_data>=0x20) {
				commun_outleng(CBuf,(char *)ss,1);
			}
			a_mode=debug_interprete(CBuf,r_data);
			if (a_mode>0) goto EXEC;
			else if(a_mode==-1) goto TERM_ERR;	//CJK
			else if(a_mode==-2) goto TERM_CANCEL;	//CJK
		}
		network_tx_proc();
	}
EXEC:
	send_crlf(CBuf);

	switch(a_mode) {
	 	case DBG_CMD_E1 :
	 		EthDspMyInform(CBuf,0);
			break;
#ifndef CPU_PIC32MZ
	 	case DBG_CMD_W1 :
	 		EthDspMyInform(CBuf,1);
			break;
	 	case DBG_CMD_W2 :
	 		EthScanSSID(1);
			break;
#endif
	 	case DBG_CMD_WB :
	 		EthTelnetToBridge();
			break;
	 	case DBG_CMD_E2 :

		        if(EthLocalCmdSend(debug_param[0],1) == 0xff) {
		       		//yoo 091228 S0BUF = 'X';
		        }
			break;
	 	case DBG_CMD_E3 :
			ethernet_debug();
#if defined(DEBUG_TCPIP_STACK_DRAM_CAS) && defined(CPU_PIC32MZ)
            debugTcpipHeapInfo(CBuf);
#endif
			break;
		case DBG_CMD_LC :
			if (debug_param[0]) {
				status_scale.scale_lock=1;
				sprintf(MsgBuf, "Locked");
			} else {
				status_scale.scale_lock=0;
				sprintf(MsgBuf, "Unlocked");
			}
			commun_out(CBuf,MsgBuf);
			set_nvram_bparam(NVRAM_SCALE_LOCK,status_scale.scale_lock);
			break;
		case DBG_CMD_SF:
			Startup.start_flag=get_global_wparam(GLOBAL_START_FLAG);
			sprintf(MsgBuf,"Current Flag [%04X]\r\n",Startup.start_flag);
			commun_out(CBuf,MsgBuf);
			Startup.start_flag=debug_param[0];
			set_global_wparam(GLOBAL_START_FLAG,Startup.start_flag);
			sprintf(MsgBuf,"New     Flag [%04X]\r\n",Startup.start_flag);
			commun_out(CBuf,MsgBuf);
			break;
#ifndef CPU_PIC32MZ
		case DBG_CMD_EF :
			ethernet_fwset(0x000915, debug_param[0]);
			break;
#endif
		case DBG_CMD_MR :	// "mr" Read Memory
#ifdef CL5200J_CJK_DEBUG
			sprintf(MsgBuf, "flag=%d addr=%06X", FlashNVRAMStruct.flag, FlashNVRAMStruct.addr);
			commun_out(CBuf,MsgBuf);
			pMem = &FlashNVRAMStruct.buffer[debug_param[0]&0x0FFF];
			for (i = 0; i < debug_param[1]; i++) {
				if (!(i%16)) send_crlf(CBuf);
				sprintf(MsgBuf, "%02X ", *pMem++);
				commun_out(CBuf,MsgBuf);
			}
			send_crlf(CBuf);
			send_crlf(CBuf);
#endif
			Flash_flush_nvbuffer();
			sprintf(MsgBuf, "%06lX - %06lX", debug_param[0], debug_param[0] + debug_param[1]);
			commun_out(CBuf,MsgBuf);
#ifdef CPU_PIC32MZ
			if (debug_param[1] > MAX_DEBUG_BUF) debug_param[1] = MAX_DEBUG_BUF;
			//Flash_sread(debug_param[0], debug_buf, debug_param[1]);
			SFlashByteDataRead(FLASH0_CS, debug_param[0], debug_buf, debug_param[1]);
			pMem = debug_buf;
#else
			pMem = (HUGEDATA INT8U *)debug_param[0];
#endif
			for (i = 0; i < debug_param[1]; i++) {
				if (!(i%16)) send_crlf(CBuf);
				sprintf(MsgBuf, "%02X ", *pMem++);
				commun_out_waiting(CBuf,MsgBuf);
			}			   
			send_crlf(CBuf);
			break;
		case DBG_CMD_MS :	// "ms" Read Memory
			Flash_flush_nvbuffer();
			sprintf(MsgBuf, "%06lX - %06lX", debug_param[0], debug_param[0] + debug_param[1]);
				commun_out(CBuf,MsgBuf);
#ifdef CPU_PIC32MZ
			if (debug_param[1] > MAX_DEBUG_BUF) debug_param[1] = MAX_DEBUG_BUF;
			//Flash_sread(debug_param[0], debug_buf, debug_param[1]);
			SFlashByteDataRead(FLASH1_CS, debug_param[0], debug_buf, debug_param[1]);
			pMem = debug_buf;
#else
			pMem = (HUGEDATA INT8U *)debug_param[0];
#endif
			for (i = 0; i < debug_param[1]; i++) {
				if (!(i%16)) send_crlf(CBuf);
				sprintf(MsgBuf, "%02X ", *pMem++);
				commun_out_waiting(CBuf,MsgBuf);
			}
			send_crlf(CBuf);
			break;
#ifndef CPU_PIC32MZ
		case DBG_CMD_MA :	// "ma" Read Memory Ascii
			sprintf(MsgBuf, "%06lX - %06lX", debug_param[0], debug_param[0] + debug_param[1]);
			commun_out(CBuf,MsgBuf);
			pMem = (HUGEDATA INT8U *)debug_param[0];
			for (i = 0; i < debug_param[1]; i++) {
				if (!(i%32)) send_crlf(CBuf);
				if(*pMem>=0x20 && *pMem<=0x7e)
					sprintf(MsgBuf, "%c", *pMem);
				else
					sprintf(MsgBuf, "%c", 0x7f);
				commun_out(CBuf,MsgBuf);
				pMem++;
			}
			break;
		case DBG_CMD_MW :	// "mw" Write Memory
			if (debug_param[0] <= (DFLASH_BASE+4*DFLASH_VOLUME) && debug_param[0] >= DFLASH_BASE) {
				Flash_program_data((INT32U)debug_param[0],(HUGEDATA INT8U *)debug_param_str,debug_param[1]);
			}
			else {
				pMem = (HUGEDATA INT8U *)debug_param[0];
				for (i = 0; i < debug_param[1]; i++) {
					*pMem++ = debug_param_str[i];
				}
			}
			sprintf(MsgBuf, "%06lX - %06lX", debug_param[0], debug_param[0] + debug_param[1]);
			commun_out(CBuf,MsgBuf);
			pMem = (HUGEDATA INT8U *)debug_param[0];
			for (i = 0; i < debug_param[1]; i++) {
				if (!(i%16)) send_crlf(CBuf);
				sprintf(MsgBuf, "%02X", *pMem++); // Flash_bread(debug_param[0]+i));//pMem++);
				commun_out(CBuf,MsgBuf);
			}
			send_crlf(CBuf);
			break;
		case DBG_CMD_NR :	// "nr" Read w5300 Memory
			send_crlf(CBuf);
			break;
		case DBG_CMD_NW :	// "nw" Write w5300 Memory
			send_crlf(CBuf);
			break;
		case DBG_CMD_MF :	// "mf" Fill Memory with data
			if (debug_param[0] >= DFLASH_BASE) {
				Flash_clear(debug_param[0], (INT8U) debug_param[2], debug_param[1]);
			}
			else {
				pMem = (HUGEDATA INT8U *)debug_param[0];
				for (i = 0; i < debug_param[1]; i++) {
					*pMem++ = (INT8U)debug_param[2];
				}
			}
			sprintf(MsgBuf, "%06lX - %06lX", debug_param[0], debug_param[0] + debug_param[1]);
			commun_out(CBuf,MsgBuf);
			pMem = (HUGEDATA INT8U *)debug_param[0];
			for (i = 0; i < debug_param[1]; i++) {
				if (!(i%16)) send_crlf(CBuf);
				sprintf(MsgBuf, "%02X", *pMem++);
				commun_out(CBuf,MsgBuf);
			}
			send_crlf(CBuf);
			break;
#endif
		case DBG_CMD_BR :	// "br" Change Baudrate
			ADM_flagDebug = 1;
			ADM_ChangeBaudrate(debug_param[0]);
			Uart1Init((INT8U)debug_param[0]);
			ADM_WaitData(ADM_CMD_START_WT, 0, ADM_PARAM_OFF);
			ADM_flagDebug = 0;
			break;
		case DBG_CMD_CF :
			if(debug_param[0] != 0x83419)
			{
				history_flash2ram(buffer);
				Flash_init_nvbuffer();
			}
			if(FlashDChipErase() == FLASH_SUCCESS) {
				sprintf(MsgBuf,"Clear Flash : Success\r\n");
				commun_out(CBuf,MsgBuf);
			} else {
				sprintf(MsgBuf,"Clear Flash : Fail\r\n");
				commun_out(CBuf,MsgBuf);
			}
			if(debug_param[0] != 0x83419)
			{
				history_ram2flash(buffer);
				Flash_flush_nvbuffer();
			}
			break;
//DEL20060309		case DBG_CMD_CC :
//DEL20060309			caption_erase();
//DEL20060309			sprintf(MsgBuf, "Caption Erase Complete:1000pcs\r\n");
//DEL20060309			commun_out(CBuf,MsgBuf);
//DEL20060309			break;
		case DBG_CMD_CR :	// "cr" Read Clock
			RTC_CLK_Burst_Read();
			sprintf(MsgBuf,"%02x/%02x/%02x(%x)\r\n",RTCStruct.year,
				RTCStruct.month,
				RTCStruct.date,
				RTCStruct.day);
			commun_out(CBuf,MsgBuf);
			sprintf(MsgBuf,"%02x:%02x:%02x\r\n", RTCStruct.hour,RTCStruct.min,RTCStruct.sec);
			commun_out(CBuf,MsgBuf);
			break;
		case DBG_CMD_CW :	// "cw" Writed Clock
			RTC_CLK_Write((INT8U)debug_param[0], (INT8U)debug_param[1]);
			RTC_CLK_Burst_Read();
			sprintf(MsgBuf,"%02x/%02x/%02x(%x)\r\n",RTCStruct.year,
				RTCStruct.month,
				RTCStruct.date,
				RTCStruct.day);

			commun_out(CBuf,MsgBuf);
			sprintf(MsgBuf,"%02x:%02x:%02x\r\n",RTCStruct.hour,RTCStruct.min,RTCStruct.sec);
			commun_out(CBuf,MsgBuf);
			break;
		case DBG_CMD_DD :	// "dd" Debug Directlry
			DebugDirectly((INT8U)debug_param[0]);
			break;
//DEL		case DBG_CMD_DI :	// "di" Debug Indirectlry
//DEL			DebugIndirectly((INT8U)debug_param[0]);
//DEL			break;
		case DBG_CMD_AF :	// "af" AD Firmware Download	// Added by CJK 20050614
			version = (INT16U)(ADM_WaitData(ADM_CMD_GET_VERSION, 0, ADM_PARAM_OFF) % 1000);	// Version = "1023" + VER[3]
			sprintf(MsgBuf, "CUR: V%d.%02d", version/100, version%100);
			commun_out(CBuf,MsgBuf);
			ad_firmware_download();
			break;
		case DBG_CMD_AC :	// "ac" AD Count Factory Clear
			errFlag = ADM_WaitData(ADM_CMD_SET_PARAMETER, ADM_SUB_CAL_DATE, ADM_PARAM_ON);
			if (errFlag != -1) {
				sprintf(MsgBuf,"ADM_SUB_CAL_DATE CLEAR\r\n");
				commun_out(CBuf,MsgBuf);
			}
			errFlag = ADM_WaitData(ADM_CMD_SET_PARAMETER, ADM_SUB_OVERLOAD_CNT, ADM_PARAM_ON);
			if (errFlag != -1) {
				sprintf(MsgBuf,"ADM_SUB_OVERLOAD_CNT CLEAR\r\n");
				commun_out(CBuf,MsgBuf);
			}
			errFlag = ADM_WaitData(ADM_CMD_SET_PARAMETER, ADM_SUB_OVERLOAD_SUM_CNT, ADM_PARAM_ON);
			if (errFlag != -1) {
				sprintf(MsgBuf,"ADM_SUB_OVERLOAD_SUM_CNT CLEAR\r\n");
				commun_out(CBuf,MsgBuf);
			}
			errFlag = ADM_WaitData(ADM_CMD_SET_PARAMETER, ADM_SUB_SHOCK_CNT, ADM_PARAM_ON);
			if (errFlag != -1) {
				sprintf(MsgBuf,"ADM_SUB_SHOCK_CNT CLEAR\r\n");
				commun_out(CBuf,MsgBuf);
			}
			errFlag = ADM_WaitData(ADM_CMD_SET_PARAMETER, ADM_SUB_SHOCK_SUM_CNT, ADM_PARAM_ON);
			if (errFlag != -1) {
				sprintf(MsgBuf,"ADM_SUB_SHOCK_SUM_CNT CLEAR\r\n");
				commun_out(CBuf,MsgBuf);
			}
			errFlag = ADM_WaitData(ADM_CMD_SET_PARAMETER, ADM_SUB_WEIGHING_CNT, ADM_PARAM_ON);
			if (errFlag != -1) {
				sprintf(MsgBuf,"ADM_SUB_WEIGHING_CNT CLEAR\r\n");
				commun_out(CBuf,MsgBuf);
			}
			errFlag = ADM_WaitData(ADM_CMD_SET_PARAMETER, ADM_SUB_WEIGHING_SUM_CNT, ADM_PARAM_ON);
			if (errFlag != -1) {
				sprintf(MsgBuf,"ADM_SUB_WEIGHING_SUM_CNT CLEAR\r\n");
				commun_out(CBuf,MsgBuf);
			}
			set_nvram_lparam(NVRAM_MORNITORING_TPH, 0x00);
			set_nvram_lparam(NVRAM_MORNITORING_TPH + 4, 0x00);
			set_nvram_lparam(NVRAM_MORNITORING_TPH + 8, 0x00);
			sprintf(MsgBuf,"NVRAM_MORNITORING_TPH CNT CLEAR\r\n");
			commun_out(CBuf,MsgBuf);
			set_nvram_lparam(NVRAM_MORNITORING_USING_TIME, 0x00);
			set_nvram_lparam(NVRAM_MORNITORING_USING_TIME + 4, 0x00);
			set_nvram_lparam(NVRAM_MORNITORING_USING_TIME + 8, 0x00);
			sprintf(MsgBuf,"NVRAM_MORNITORING_USING_TIME CLEAR\r\n");
			commun_out(CBuf,MsgBuf);	
			break;
		case DBG_CMD_FL :	// "fl" status of FLASH2
			sprintf(MsgBuf,"FTL TABLE ERR CNT : %ld\r\n", FtlErrManagement.FtlTableErrCnt); // FTL이 깨져있는 경우(Booting시 FLASH에서 로드한 Table의 CRC 체크하여 판단)
			commun_out(CBuf,MsgBuf);
			sprintf(MsgBuf,"FTL TABLE CACHE ERR CNT : %ld\r\n", cacheCrcErrCount.FtlTableErrCnt); // Power Off시, RAM의 FTL Table CRC 오류 발생할 경우(논리적으로 생기지 않아야 함)
			commun_out(CBuf,MsgBuf);
			sprintf(MsgBuf,"FTL F.CACHE ERR CNT : %ld\r\n", cacheCrcErrCount.FtlFlashCacheErrCnt); // Power Off시, RAM의 Flash Cache CRC 오류 발생할 경우(논리적으로 생기지 않아야 함)
			commun_out(CBuf,MsgBuf);
			sprintf(MsgBuf,"FTL N.CACHE ERR CNT : %ld\r\n", cacheCrcErrCount.FtlNvramCacheErrCnt); // Power Off시, RAM의 Nvram Cache CRC 오류 발생할 경우(논리적으로 생기지 않아야 함)
			commun_out(CBuf,MsgBuf);
			sprintf(MsgBuf,"FTL I.CACHE ERR CNT : %ld\r\n", cacheCrcErrCount.IngredientCacheErrCnt); // Power Off시, RAM의 Flash Ing. Cache CRC 오류 발생할 경우(논리적으로 생기지 않아야 함)
			commun_out(CBuf,MsgBuf);
			sprintf(MsgBuf,"FTL Backup Fail CNT : %ld\r\n", FtlErrManagement.FtlInterruptErrCnt); // 정상 Backup이 되지 않고 복구할 히스토리만 남은 경우.(복구가능)
			commun_out(CBuf,MsgBuf);
			break;
		case DBG_CMD_FT :	// "ft" FTL Table
			upload_ftl_table(CBuf, debug_param[0]);
			break;
#ifndef CPU_PIC32MZ
		case DBG_CMD_FM :	// "fm" Check Mem Card
			debug_param[0] = FlashCheckMem();
			sprintf(MsgBuf,"Flash Memory Chip : %04X\r\n",(INT16U)debug_param[0]);
			commun_out(CBuf,MsgBuf);
//			CheckFlashID();
			break;
#endif
		case DBG_CMD_IA :
			CalRun_InitADM(1, ON);// Modified by CJK 20050816
			break;
#ifdef USE_EXT_SETTING_MEM
		case DBG_CMD_IS :
			Data_Save_Restore(0);
			break;
		case DBG_CMD_IR :
			Data_Save_Restore(1);
			break;
		case DBG_CMD_II :
			Data_Save_Restore(9);
			plu_all_delete(CBuf,8);
			break;
		case DBG_CMD_IB :
			Data_Save_Restore(8);
			break;
#endif
		case DBG_CMD_QT :	// "qt" Quit Debug Terminal
			goto TERM_END;
		case DBG_CMD_PD :	// "pd" Set Print Default
			PrtFlashSetDefault();
			sprintf(MsgBuf,"Set Print Default\r\n");
			commun_out(CBuf,MsgBuf);
			KEY_CopyKeyTable(0);
			sprintf(MsgBuf,"Copy Key Table\r\n");
			commun_out(CBuf,MsgBuf);
			break;
		case DBG_CMD_PI:
			sprintf(MsgBuf,"Init PLU Struct\r\n");
			commun_out(CBuf,MsgBuf);
			plu_struct_set_base();
			sprintf(MsgBuf,"End PLU Struct\r\n");
			commun_out(CBuf,MsgBuf);
			break;
		case DBG_CMD_PW:
			if (debug_param[0]==0) {
				get_base_sparam(FLASH_START_PASSWORD,(INT8U*)MsgBuf,DEALER_PASSWORD_LEN);
				commun_out(CBuf,MsgBuf);
				send_crlf(CBuf);
			} else {
				set_base_sparam(FLASH_START_PASSWORD,debug_param_str,DEALER_PASSWORD_LEN);
				sprintf(MsgBuf,"Changed Password %s\r\n",debug_param_str);
				commun_out(CBuf,MsgBuf);
			}
			break;
		case DBG_CMD_KS :
			KEY_SetKey((INT8U)debug_param[0], (INT8U)debug_param[1], (INT16U)debug_param[2]);
			break;
		case DBG_CMD_AD :
			if (debug_param[0] == 0) {
				ADM_flagDebug = 0;
				sprintf(MsgBuf, "ADM Debug Flag OFF!\r\n");
			} else {
				ADM_flagDebug = 1;
				sprintf(MsgBuf, "ADM Debug Flag ON!\r\n");
			}
			commun_out(CBuf,MsgBuf);
			break;
#ifdef TEST_REPORT
		case DBG_CMD_PR :
			TestPrintSaleReportAll((INT16U)debug_param[0],(INT16U)debug_param[1]);
			break;
#endif
#ifdef DEBUG_VIEW_FONTLIST
		case DBG_CMD_VF :
			ViewFontList();
			break;
#endif
		case DBG_CMD_CI:
			change_dest_ip_val(debug_param[0]);
			break;
#ifdef DEBUG_BARFORM_FOR_SCAN //for test scan
		case DBG_CMD_SM :
			for (i = 0; i < debug_param[0]; i++) {
				test_scan_form[i] = debug_param_str[i];
			}
			test_scan_form[i] = 0;
			sprintf(MsgBuf, "form:%s", test_scan_form);
			commun_out(CBuf,MsgBuf);
			send_crlf(CBuf);
			break;
		case DBG_CMD_SD :
			for (i = 0; i < debug_param[0]; i++) {
				test_scan_data[i] = debug_param_str[i];
			}
			test_scan_data[i] = 0;
			sprintf(MsgBuf, "data:%s", test_scan_data);
			commun_out(CBuf,MsgBuf);
			send_crlf(CBuf);

			prt_scan_strform(test_scan_data, test_scan_form, &test_bar_param);
			i = test_bar_param.DeptId;
			sprintf(MsgBuf, "D:%ld I:%ld W:%ld T:%s", i, test_bar_param.ItemCode, test_bar_param.Weight, test_bar_param.Text);
			commun_out(CBuf,MsgBuf);
			send_crlf(CBuf);
			break;
#endif
		case DBG_CMD_CT :
			switch(debug_param[0])
			{
				default:
				case 1:	// for Debug 090515
					sprintf(MsgBuf, "u=%d, d=%d, (%02x)", unknownSockCnt, debugSockCnt, debugSockState);
					break;
				case 2:
					wlan_RebootModule(Startup.wireless_type);
					MsgBuf[0] = 0;
					break;
				case 3:
					wlan_CheckVer_Bridge(Startup.wireless_type, buffer, 0);
					sprintf(MsgBuf, (char*)buffer);
					break;
			}
			commun_out(CBuf,MsgBuf);
			send_crlf(CBuf);
			break;
		case DBG_CMD_GS :
			i = Ethernet_Command(0, TCP_STATE_REQ_COMMAND);
			sprintf(MsgBuf, "tcp state : %02x", (INT8U)i);
			commun_out(CBuf,MsgBuf);
			send_crlf(CBuf);
			break;
#ifdef USE_MULTI_TRACE_NO_DBG
		case DBG_CMD_TN :
			TNT_TestTraceNoTable();
			break;
		case DBG_CMD_TR :
			TNT_ReadTraceNoTable();
			break;
#endif
#ifdef USE_CHN_IO_BOARD
#ifdef USE_I2C_DEBUG
		case DBG_CMD_IC :
			i2c_to_uart();
			break;
#endif
#endif
#ifdef USE_CHN_CART_SCALE
		case DBG_CMD_CV :
			vege_trace_test();
			break;
#endif
#ifdef USE_CL5200N_SFLASH_DEBUGGING
        case DBG_CMD_DF8M:
            switch( debug_param[0] )
            {
                case 1:
                    sflash_8M_test( 0, 34, 2 );
                    break;

                case 2:
                    nvram_8M_test( 0, 34, 2 );
                    break;

                case 3:
                    sflash_8M_test( (8*1024)-100, (8*1024)-50, 2 );
                    break;

                case 4:
                    nvram_8M_test( 66, 100, 2 );
                    break;

                default :
                    sprintf( MsgBuf, "Input the Parameter (ex: df8m 1 or df8m 2)" );
            		commun_out(CBuf,MsgBuf);
            		send_crlf(CBuf);
                break;
            }
            break;
#endif
#ifdef USE_SRAM_SPI_COMM
        case DBG_CMD_SR:
            switch(debug_param[0])
			{
                case 1:     // Mode Register Read & Write Test
                    sprintf(MsgBuf, "** - SRAM TEST (Mode Register Setting) - **");
            		commun_out(CBuf,MsgBuf);
            		send_crlf(CBuf);

                    data = 0x10;
                    for( i=0; i<4; i++ )
                    {
                        SramSetMode(data);
                        sprintf(MsgBuf, "%d.SRAM_WRMR : %02x", i+1, data);
                		commun_out(CBuf,MsgBuf);
                		send_crlf(CBuf);

                        for( j=0; j<10; j++ )
                        {
                            sprintf(MsgBuf, ">>>>SRAM_RDMR : %02x", SramGetMode());
                    		commun_out(CBuf,MsgBuf);
                    		send_crlf(CBuf);
                        }
                        if ( data == 0x10 )         data = 0x40;
                        else if ( data == 0x40 )    data = 0x80;
                        else                        data = 0x10;
                    }
                    break;

                case 2:     // Sequential Operation Read & Write Test
                    SramSetMode(0x40);
                    sprintf( MsgBuf, "** - SRAM TEST (Sequential Operation) - **" );
            		commun_out(CBuf,MsgBuf);
            		send_crlf(CBuf);

                    sprintf( MsgBuf, "SRAM_RDMR: %02x", SramGetMode() );
            		commun_out(CBuf,MsgBuf);
            		send_crlf(CBuf);

                    SramWriteData( 0x00FE, "SRAM_SPI_TEST", sizeof("SRAM_SPI_TEST") );

                    for( i=0; i<4; i++ )
                    {
                        if( !SramReadData( 0x0100, resultData, 15 ) )   sprintf( MsgBuf, "Siquential Read Test(#%d_Loop): %s", i+1, resultData );
                        else                                            sprintf( MsgBuf, "Siquential Read Test(#%d_Loop): FAIL!!", i+1 );

                        commun_out(CBuf,MsgBuf);
                        send_crlf(CBuf);
                    }
                    break;

                case 3:     // Print Mem Test
                    SramSetMode(0x40);
                    sprintf( MsgBuf, "** - SRAM TEST (Printer Memory) - **" );
            		commun_out(CBuf,MsgBuf);
            		send_crlf(CBuf);

                    for( i=0; i<5; i++ )
                    {
                        if( !SramReadData( SRAM_ADDR_PRT_BASE + (20*i), resultData, 15 ) )
                        {
                            for( j=0; j<10; j++)
                            {
                                sprintf( MsgBuf, ">SRAM_RDMR : %02x", resultData[j]);
                                commun_out(CBuf,MsgBuf);
                                send_crlf(CBuf);
                            }
                        }
                    }
                    break;

                default :
                    sprintf( MsgBuf, "Input the Parameter (ex: sr 1 or sr 2)" );
            		commun_out(CBuf,MsgBuf);
            		send_crlf(CBuf);
                    break;
            }
            break;
#endif
#ifdef USE_CPU_LOAD_TEST
    case DBG_CMD_NM:
        {
            switch(debug_param[0])
			{
                case 1:
                    FlagSFlashTest = 1;
                    sprintf( MsgBuf, "Set Test for NVRAM!" );
            		commun_out(CBuf,MsgBuf);
            		send_crlf(CBuf);
                    break;

                case 0:
                    FlagSFlashTest = 0;
                    sprintf( MsgBuf, "Not Using Test!" );
            		commun_out(CBuf,MsgBuf);
            		send_crlf(CBuf);
                    break;
                    
                default:
                    FlagSFlashTest = 0;
                    sprintf( MsgBuf, "Input the Parameter ( 0:None   1:NVRAM_TEST )" );
            		commun_out(CBuf,MsgBuf);
            		send_crlf(CBuf);
                    break;
            }
        }
        break;
#endif
	}
	goto TERM_START;
TERM_ERR :
	send_crlf(CBuf);
	goto TERM_START;
TERM_CANCEL :
	send_crlf(CBuf);
	sprintf(MsgBuf, "Cancel\r\n");
	commun_out(CBuf,MsgBuf);
	goto TERM_START;
TERM_END :
	status_run.run_mode = old_run;
	sprintf(MsgBuf, "End Debug Terminal\r\n");
	commun_out(CBuf,MsgBuf);
	Dsp_SetPage(DSP_DEFAULT_PAGE);
	DspLoadFont1(DSP_MENU_FONT_ID);
	menu_display_menucode(0,0);
	Dsp_Diffuse();
	sale_display_update(UPDATE_ALL|UPDATE_CLEARALL);
	return;
}

void HostToModule(INT8U type)
{
	switch(type)
	{
		case COMM_TYPE_HOST :
			break;
		case COMM_TYPE_AD :
			if (CheckRxBuf(&CommBuf)) {
					PutStrTxBuf_copy(&CommBufAD, &CommBuf);		// Test ADM (HOST -> ADM)
			}
			break;
		case COMM_TYPE_ETH :
			if (CheckRxBuf(&CommBuf)) {
					PutStrTxBuf_copy(&CommBufEth, &CommBuf);	// Test Ehternet (HOST -> ETHERNET)
			}
			break;
		case COMM_TYPE_PRT :
			if (CheckRxBuf(&CommBuf)) {
					PutStrRxBuf_copy(&CommBufPrt, &CommBuf);	// Test Priter (HOST -> PRINTER)
			}
			break;
#ifndef CPU_PIC32MZ
		case COMM_TYPE_KEY :
			if (CheckRxBuf(&CommBuf)) {
					PutStrTxBuf_copy(&CommBufKey, &CommBuf);	// Test Key (HOST -> KEY)
			}
			break;
		case COMM_TYPE_USB :
			if (CheckRxBuf(&CommBuf)) {
					PutStrTxBuf_copy(&CommBufUsb, &CommBuf);	// Test USB (HOST -> USB)
			}
			break;
#endif
#ifdef USE_CHN_IO_BOARD
		case COMM_TYPE_RFID:
			if (CheckRxBuf(&CommBuf)) {
					PutStrTxBuf_copy(&CommBufRFID, &CommBuf);	// Test I2C (HOST -> RFID(I2C))
			}
			break;
		case COMM_TYPE_GPRS:
			if (CheckRxBuf(&CommBuf)) {
					PutStrTxBuf_copy(&CommBufGPRS, &CommBuf);	// Test I2C (HOST -> GPRS(I2C))
			}
			break;
		case COMM_TYPE_USBMEM:
			if (CheckRxBuf(&CommBuf)) {
					PutStrTxBuf_copy(&CommBufUsbMem, &CommBuf);	// Test I2C (HOST -> USBMem(I2C))
			}
			break;
#endif
	}
}


void ModuleToHost(INT8U type)
{
	switch(type)
	{
		case COMM_TYPE_HOST :
			break;
		case COMM_TYPE_AD :
			if (CheckRxBuf(&CommBufAD)) {
					PutStrTxBuf_copy(&CommBuf, &CommBufAD);		// Test AD (ADM -> HOST)
			}
			break;
		case COMM_TYPE_ETH :
			if (CheckRxBuf(&CommBufEth)) {
					PutStrTxBuf_copy(&CommBuf, &CommBufEth);	// Test Ehternet (ETHERNET -> HOST)
			}
			break;
		case COMM_TYPE_PRT :
			if (CheckTxBuf(&CommBufPrt)) {
					PutStrTx2Buf_copy(&CommBuf, &CommBufPrt);	// Test Priter (PRINTER -> HOST)
			}
			break;
#ifndef CPU_PIC32MZ
		case COMM_TYPE_KEY :
			if (CheckRxBuf(&CommBufKey)) {
					PutStrTxBuf_copy(&CommBuf, &CommBufKey);	// Test Key (KEY -> HOST)
			}
			break;
		case COMM_TYPE_USB :
			if (CheckRxBuf(&CommBufUsb)) {
					PutStrTxBuf_copy(&CommBuf, &CommBufUsb);	// Test Priter (USB -> HOST)
			}
			break;
#endif
#ifdef USE_CHN_IO_BOARD
		case COMM_TYPE_RFID:
			if (CheckRxBuf(&CommBufRFID)) {
					PutStrTxBuf_copy(&CommBuf, &CommBuf);		// Test I2C (RFID(I2C) -> HOST)
			}
			break;
		case COMM_TYPE_GPRS:
			if (CheckRxBuf(&CommBufGPRS)) {
					PutStrTxBuf_copy(&CommBuf, &CommBuf);		// Test I2C (GPRS(I2C) -> HOST)
			}
			break;
		case COMM_TYPE_USBMEM:
			if (CheckRxBuf(&CommBufUsbMem)) {
					PutStrTxBuf_copy(&CommBuf, &CommBuf);		// Test I2C (USBMem(I2C) -> HOST)
			}
			break;
#endif
	}
}

ROMDATA char*debug_fjig_title[]={
	"Serial",
	"Display",
	"ADM",
	"RTC",
	"NVRAM",
	"Flash",
//	"Flash(Ext)",
	"Ethernet",
	"Keypad",
	"Cashdrawer",
	"PS/2&USB",
	"Prt_Sensor",
	"Prt_Chess"};


extern INT8U PrtFontType; //SG060313
extern void StrInit4UserLabel(void);

extern APP_DATA appData;
extern INT8U   Debugmode;
void   __attribute__((optimize("-O0"))) debug_Factory(void)
{
	INT8U r_data,cc,s_data;
	INT16U ret,ret1,gab,peel,disp_timeout,debug_timeout,key_count,gabmin,gabmax,peelmin,peelmax,temperature;
	//INT16U ret,ret1,gab,peel,disp_timeout,debug_timeout,key_count;
	INT8U  debug_factory_status,test_mode ,display_id;
	INT8U  pre_cmd,string_buf[62],date_cnt,disp_first;
	INT8U  test_ok[20],cmd_code;
	INT16S    send;
	INT8U  title[10];
	INT8U esc_count;
	ROMDATA char *cp;
	FW_VERSION fversion;
	INT8U i;
	INT8U dd;
	INT32U ret_jedecID;
	POINT disp_p;
	CAPTION_STRUCT cap;

	debug_factory_status=0;
	pre_cmd  =0;
	test_mode=0;
	dd = 0xff;
	esc_count=0;
	COMM_BUF *CBuf;
	COMM_BUF *RBuf;
	char head[20];
	INT8U  btype[15]={0x09,0x31,0x42,0x53,0x64,0x75,0x76,0x77,0x78,0x79,0x6a,0x5B,0x4C,0x3D,0x2E};
	INT8U ptype[14]={0x0E,0x02,0x13,0x24,0x35,0x46,0x57,0x68,0x79,0x6A,0x5B,0x4C,0x3D,0x2E};
	INT8U full_path[FAT_FS_MAX_LFN];
	INT8U usbtext[255];
	SYS_FS_HANDLE usb_handler;
	INT8U recive_result,recive_result1,recive_result2;
	INT8U scaletype;
	//INT8U ip[4];
	INT16U len;
        INT8U version[17];
        int eht_return;
        INT16U send_time;
        send_time = SysTimer100ms;
	scaletype = 0;
	scaletype = get_global_bparam(GLOBAL_AREA_SCALETYPE);
    	INT8U ip[4] = {10,10,4,254};
	INT8U ip_l[4] = {10,10,4,33};
	INT8U gw[4] = {10,10,0,1};
	INT8U sn[4] = {255,0,0,0};

	version_get(&fversion);

	//VFD7_Init();
	//LCD_Init();
	Dsp_InitHardware();
	display_init();
	display_alloff_primary_indication();
	display_temp_setting();
	sprintf((char *)title,"CHECK");
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
	disp_p = point(0,0);
	Dsp_Write_String(disp_p, ( char *)title);
	Dsp_Diffuse();
#else
	display_string(DISPLAY_WEIGHT,(INT8U*)title);
	VFD7_Diffuse();
#endif
	PrtDrvInit();
	PRT_DEFAULT_FONT_ID=0;

	string_buf[0] = _ESC;
	string_buf[1] = 'S';
	string_buf[2] = 'O';
	string_buf[3] = 3;
	PutDataRxBuf(&CommBufPrt,(HUGEDATA char*)string_buf, 4);
	FontLoadFont1(0);
	RTCInit();
	debug_timeout = SysTimer100ms;
	memset((char *)test_ok,0x88,20);
        Debugmode = 1;

	while (1) {
		//network_check();
		//APP_Tasks();
	    if (CheckRxBufCount(&CommBuf,1)) {
		r_data=GetCharRxBuf(&CommBuf);
		send=10;
		if (!debug_factory_status) {
		    if (test_mode==0x52) {
			if ((r_data!='T') && (r_data!='X')) {
				sprintf(gmsg,"%c",r_data);
				commun_outleng(&CommBuf,gmsg,1);
			}
		    }
		}
		// start - switch (debug_factory_status)
		switch (debug_factory_status) {
		    case 0: debug_factory_status=0;
			    pre_cmd=0;
			    cmd_code=r_data;
		    	    if ((r_data=='T') || (r_data=='X')) {
				debug_timeout = SysTimer100ms;
		    	    	debug_factory_status=1;
			    }
			    break;
		    case 1: debug_factory_status=0;
			    if (ishex(r_data)) {
				pre_cmd = ctoh(r_data);
				debug_factory_status=2;
			    }
			    break;
		    case 2: if (pre_cmd==4) {
				debug_factory_status=0;
				if (r_data=='.') {
					date_cnt=0;
					debug_factory_status=40;
				}
		    	    } else {
				if (r_data==0x0a) {
					debug_factory_status=0;
					test_mode    = pre_cmd;
					disp_first   = 0;
					disp_timeout = 0;
					key_count=0;
					if ((test_mode==0) && (cmd_code=='T')) {
						 sprintf(gmsg,"R0\x0a");
						 commun_out(&CommBuf,gmsg);
						 break;
					}
					if (cmd_code=='X') {
						test_mode >>= 4;
						r_data=pre_cmd&0x0f;
						if (test_mode==0x0a) break;
						if ((test_mode>=3) && (test_mode<=7)) break;
						test_ok[test_mode]=r_data;
					} else {
						KEY_Init();
					}
					break;
				  } else if (ishex(r_data)) {
					pre_cmd<<=4;
					pre_cmd|= ctoh(r_data);
				  }
			    }
			    break;
		    case 40:if (r_data==0x0a) {
		    		string_buf[date_cnt]=0;
				commun_setrtc(string_buf,date_cnt);
				RTC_CLK_Burst_Read();
	    		        s_data   =(INT8U)bcd2hex(RTCStruct.sec);
				test_mode=0x04;
				debug_factory_status=0;
			    } else {
				if (date_cnt>40) {
					debug_factory_status=0;
				}
		    		string_buf[date_cnt++]=r_data;
			    }
			    break;
		    default:debug_factory_status=0;
			    break;
		} // end switch (debug_factory_status)
		continue;
	    } // if (CheckRxBufCount(&CommBuf,1))
	    send=0; cc='O';
	    if (!debug_factory_status) {
	    	if (cmd_code=='X') {
	    		Dsp_Fill_Buffer(0);
	    		Dsp_Diffuse();
	    		display_alloff_primary_indication();
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
				disp_p = point(0,0);
				//sprintf(title,"SERIAL");
				Dsp_Write_String(disp_p, ( char *)title);
				Dsp_Diffuse();
#else
	    		display_string(DISPLAY_WEIGHT,(INT8U*)title);
				VFD7_Diffuse();
#endif
	    		sprintf(gmsg,"X%1X\x0a",test_mode);
	    		send    = 1;
	    		cmd_code= 0;
	    		goto SEND;
	    	}
		if (cmd_code!='T') goto SEND;
		ret =0;
		ret1=0;
	    	switch (test_mode) {
	    	    case 0x01:		//Serial Test
					sprintf(gmsg,"R%1X\x0a",test_mode);
	    		    send = 1;
	    		    test_mode=0x52;
	    		    break;
	    	    case 0x02:		//Display Test
					if (disp_first==0) {
		    			disp_first   = 1;
	    				disp_timeout = SysTimer100ms-10;
	    				display_id   = 0;
	    				send=1;
	    				sprintf(gmsg,"R2\x0a");
	    			}
	    	      		//if (SysTimer100ms-disp_timeout>6) {
					if ((INT16U)(SysTimer100ms-disp_timeout)>4) {
	    				disp_timeout=SysTimer100ms;
	    				////display_use_allon_digit('0'+display_id);
	    				////init_lcdchess();
	    				////display_id = (display_id+1)%10;

						display_alloff_primary_indication();
						if(display_id < 8) {
							for (i=0; i<DSP_MAX_DIGIT/8; i++) {
								VFD7_display_char(display_id+i*8,'8');
								VFD7_display_triangle(display_id+i*8, 1);
								VFD7_display_char(display_id+i*8,',');
							}
						} else {
							for (i=0; i<DSP_MAX_DIGIT; i++) {
								VFD7_display_char(i,(display_id-8)+'0');
								VFD7_display_triangle(i,(display_id-8)%2);
								if ((display_id-8)%2) {
									VFD7_display_char(i,',');
								}
							}
						}
						if(!(display_id%4)) {
							LCD_Check(0x00, 0x00);
							if(display_id == 16) ;
							else init_lcdchess();
							//Dsp_Fill_Buffer(dd);
							dd ^= 0xff;
						}
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
						Dsp_Diffuse();
#else
						VFD7_Diffuse();
#endif
						display_id = (display_id+1)%18;;
	    			}
	    			break;
	    	    case 0x03:		//AD Module Test
					disp_p.x = 0;
					disp_p.y = 0;
					Dsp_Fill_Buffer(0);
#ifdef USE_CHN_FONT
					DspLoadFont1(DSP_MEDIUM_FONT_ID);
#else
					DspLoadFont1(DSP_LARGE_FONT_ID);
#endif
					sprintf(string_buf,"AD TESTING....");
					Dsp_Diffuse();
					Dsp_Write_String(disp_p, (HUGEDATA char *)string_buf);
					Delay_100ms(5);
					CalRun_InitADM(2, OFF);// Modified by CJK 20050816
	    	    	ret=(INT16S)ADM_Init();
					disp_p.x = 0;
					disp_p.y = 17;
					Dsp_Fill_Buffer(0);
#ifdef USE_CHN_FONT
					DspLoadFont1(DSP_MEDIUM_FONT_ID);
#else
					DspLoadFont1(DSP_LARGE_FONT_ID);
#endif
					sprintf(string_buf,"                      ");
					Dsp_Write_String(disp_p, (HUGEDATA char *)string_buf);
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
					Dsp_Diffuse();
#endif
	    	    	test_ok[test_mode]=1;
	    			if (ret) {
	    				cc='E';
						test_ok[test_mode]=0;
					}
	    			send=1;
	    			goto OUT_END1;
	    	    case 0x04:		// RTC Test
					disp_timeout=SysTimer100ms;
					disp_p.x = 0;
					disp_p.y = 0;
					Dsp_Fill_Buffer(0);
#ifdef USE_CHN_FONT
					DspLoadFont1(DSP_MEDIUM_FONT_ID);
#else
					DspLoadFont1(DSP_LARGE_FONT_ID);
#endif
					sprintf(string_buf,"RTC TESTING....");
					Dsp_Diffuse();
					Dsp_Write_String(disp_p, (HUGEDATA char *)string_buf);
					Delay_100ms(5);
					while (1) {
						if ((INT16U)(SysTimer100ms-disp_timeout)>20) break;
						RTC_CLK_Burst_Read();
						r_data=(INT8U)bcd2hex(RTCStruct.sec);
						if (r_data!=s_data) break;
					}
					disp_p.x = 0;
					disp_p.y = 0;
					Dsp_Fill_Buffer(0);
#ifdef USE_CHN_FONT
					DspLoadFont1(DSP_MEDIUM_FONT_ID);
#else
					DspLoadFont1(DSP_LARGE_FONT_ID);
#endif
					sprintf(string_buf,"                ");
					Dsp_Diffuse();

	    			ret=1;
	    			if (r_data&0x80 ) ret=0;
	    			if (s_data-r_data==0) ret=0;
	    			if (!ret) cc='E';
					test_ok[test_mode]=(INT8U)ret;
	    			goto OUT_END1;
			    case 0x05:		//NVRAM Test
					disp_p.x = 0;
					disp_p.y = 0;
					Dsp_Fill_Buffer(0);
#ifdef USE_CHN_FONT
					DspLoadFont1(DSP_MEDIUM_FONT_ID);
#else
					DspLoadFont1(DSP_LARGE_FONT_ID);
#endif
					sprintf(string_buf,"NVRAM TESTING....");
					Dsp_Diffuse();
					Dsp_Write_String(disp_p, (HUGEDATA char *)string_buf);
					Delay_100ms(5);

					test_ok[test_mode] = SQI_Flash_Exist_Device(1);
	
					disp_p.x = 0;
					disp_p.y = 0;
					Dsp_Fill_Buffer(0);
#ifdef USE_CHN_FONT
					DspLoadFont1(DSP_MEDIUM_FONT_ID);
#else
					DspLoadFont1(DSP_LARGE_FONT_ID);
#endif
					sprintf(string_buf,"                ");
					Dsp_Diffuse();
					if (!(test_ok[test_mode]))
						cc='E';
					goto OUT_END1;
				case 0x06: 		//Flash Memory Test
					disp_p.x = 0;
					disp_p.y = 0;
					Dsp_Fill_Buffer(0);
#ifdef USE_CHN_FONT
					DspLoadFont1(DSP_MEDIUM_FONT_ID);
#else
					DspLoadFont1(DSP_LARGE_FONT_ID);
#endif
					sprintf(string_buf,"FLASH TESTING....");
					Dsp_Diffuse();
					Dsp_Write_String(disp_p, (HUGEDATA char *)string_buf);
					Delay_100ms(5);
	
					test_ok[test_mode] = SQI_Flash_Exist_Device(0);
	
					disp_p.x = 0;
					disp_p.y = 0;
					Dsp_Fill_Buffer(0);
#ifdef USE_CHN_FONT
					DspLoadFont1(DSP_MEDIUM_FONT_ID);
#else
					DspLoadFont1(DSP_LARGE_FONT_ID);
#endif
					sprintf(string_buf,"                ");
					Dsp_Diffuse();
	
					if (!(test_ok[test_mode]))
						cc='E';
		    			goto OUT_END1;
	    	    case 0x07:  	//Ethernet Module Test
					disp_p.x = 0;
					disp_p.y = 0;
					Dsp_Fill_Buffer(0);
#ifdef USE_CHN_FONT
					DspLoadFont1(DSP_MEDIUM_FONT_ID);
#else
					DspLoadFont1(DSP_LARGE_FONT_ID);
#endif
					sprintf(string_buf,"ETHERNET TESTING....");
					Dsp_Diffuse();
					Dsp_Write_String(disp_p, (HUGEDATA char *)string_buf);
					Delay_100ms(5);
					ret = 0;
                    			ret = ehternet_test();
 					disp_p.x = 0;
					disp_p.y = 0;
					Dsp_Fill_Buffer(0);
#ifdef USE_CHN_FONT
					DspLoadFont1(DSP_MEDIUM_FONT_ID);
#else
					DspLoadFont1(DSP_LARGE_FONT_ID);
#endif
					sprintf(string_buf,"                ");
					Dsp_Diffuse();
	    			if (!ret) {
	    				cc='E';
						ret=0;
					} else ret=1;
					test_ok[test_mode]=(INT8U)ret;
	    			goto OUT_END1;
	    	    case 0x08:  	//Keypad Test
/*
				 	disp_p.x = 0;
					disp_p.y = 0;
					Dsp_Fill_Buffer(0);
#ifdef USE_CHN_FONT
					DspLoadFont1(DSP_MEDIUM_FONT_ID);
#else
					DspLoadFont1(DSP_LARGE_FONT_ID);
#endif
					sprintf(string_buf,"KEYPAD TESTING....");
					Dsp_Diffuse();
					Dsp_Write_String(disp_p, (HUGEDATA char *)string_buf);
*/
/*
					if(scaletype == SCALE_TYPE_BENCH_CL5200) //BENCH type
					{
						if(key_count == 15) 
						{
							test_ok[test_mode]=1;
							key_count=0;
							goto OUT_END1;
						}
					}
					else
					{
*/
						if(key_count == 14) 
						{
							test_ok[test_mode]=1;
							key_count=0;
							goto OUT_END1;
						}
//					}
					send=0;
	    			if (!disp_first) {
	    				sprintf(gmsg,"R%1X\x0a",test_mode);
	    				disp_first=1;
	    				send=1;
	    			}

	    			if(KEY_IsKey()) {
	    				BuzOn(1);
	    				//key_count++;
	    				ret = (INT16U)KEY_InKey();
					sprintf(gmsg,"%02X",ret);
/*#ifdef _USE_LCD20848_
					disp_p = point(0,0);
					Dsp_Write_String(disp_p, ( char *)gmsg);
					Dsp_Diffuse();
#else
					display_string(DISPLAY_UNITPRICE,(INT8U *)gmsg);
					VFD7_Diffuse();
#endif*/
	    				sprintf(gmsg,"K%02X\x0a",ret);
	    				send=1;
/*
						disp_p.x = 0;
						disp_p.y = 0;
						Dsp_Fill_Buffer(0);
#ifdef USE_CHN_FONT
						DspLoadFont1(DSP_MEDIUM_FONT_ID);
#else
						DspLoadFont1(DSP_LARGE_FONT_ID);
#endif
						sprintf(string_buf,"VAL:%02x CNT:%02d",rawrey,key_count);
						Dsp_Write_String(disp_p, (HUGEDATA char *)string_buf);
						Dsp_Diffuse();
*/
/*
						if(scaletype == SCALE_TYPE_BENCH_CL5200) //BENCH type
						{
							if(key_count > 14) key_count=0;

							if(ret != btype[key_count])
							{
								test_ok[test_mode]=0;
								cc='E';
								goto OUT_END1;
							}
							else 
							{
								key_count++;
							}
						}
						else //POLE type
						{
*/
							if(key_count > 13) key_count=0;
	
							if(ret != ptype[key_count])
							{
								test_ok[test_mode]=0;
								cc='E';
								goto OUT_END1;
							}
							else 
							{
								key_count++;
							}
	
//						}
					}
//						disp_p.x = 0;
//						disp_p.y = 0;
//						Dsp_Fill_Buffer(0);
//						DspLoadFont1(DSP_LARGE_FONT_ID);
//						sprintf(string_buf,"                ");
//						Dsp_Diffuse();
	    			break;
	    	   case 0x09:		//Cashdrawer Test
			   		CASH_ON;
                                            Delay_100ms(2);
					//r_data=(INT16U)Cash_check(); // if (r_data==0) then true
					ret =0;
					r_data=Cash_check(); // if (r_data==0) then true
					if (r_data==0) ret =1;
					CASH_OFF;
					Delay_100ms(2);
					//r_data=(INT16U)Cash_check(); // if (r_data==1) then true
					//r_data=Cash_check(); // if (r_data==1) then true
					//if (r_data==1) ret1=1;
	    			        //ret+=ret1;
					test_ok[test_mode]=1;
	    			if (ret!=1) {
					test_ok[test_mode]=0;
	    				cc='E';
				}
	    			goto OUT_END1;
	    	    case 0x0A:  	//USB Test
					disp_p.x = 0;
					disp_p.y = 0;
					Dsp_Fill_Buffer(0);
#ifdef USE_CHN_FONT
					DspLoadFont1(DSP_MEDIUM_FONT_ID);
#else
					DspLoadFont1(DSP_LARGE_FONT_ID);
#endif
					sprintf(string_buf,"USB TESTING....");
					Dsp_Diffuse();
					Dsp_Write_String(disp_p, (HUGEDATA char *)string_buf);
					Delay_100ms(5);
					recive_result=0;
					recive_result1=0;
					recive_result2=0;
					//wlan_SetNetwork(ip_l, gw, sn);
					APP_Initialize ();
//					appData.tempAddressFlag = true;
//					while (1)                   // check the size of received data
//					{
//						TcpIp_Task();
//						if ((INT16U)(SysTimer100ms - send_time) > 30) break;//tcpip stack stable time 3 sec
//					}
					recive_result1 = zig_backup_data();
					Delay_100ms(5);
					if(recive_result1) 
					{
						recive_result2 = zig_read_data();
						Delay_100ms(5);
					}
					disp_p.x = 0;
					disp_p.y = 0;
					Dsp_Fill_Buffer(0);
#ifdef USE_CHN_FONT
					DspLoadFont1(DSP_MEDIUM_FONT_ID);
#else
					DspLoadFont1(DSP_LARGE_FONT_ID);
#endif
					sprintf(string_buf,"                      ");
					Dsp_Write_String(disp_p, (HUGEDATA char *)string_buf);
					Dsp_Diffuse();
					recive_result = recive_result1 + recive_result2;
					if(recive_result==2)
					{
						test_ok[test_mode]=1;
						goto OUT_END1;
					}
					else
					{
						test_ok[test_mode]=0;
						cc='E';
						goto OUT_END1;
					}
	    		    break;

	    	    case 0x0B:  	//Printer Sensor Test
					PrtDrvSetPrePrintMode(1,64);
					printer_sensor_keytest((INT16U *)&gab,(INT16U *)&peel,3);
	    			gabmin = SensorStruct.MinGapLastValue;
					gabmax = SensorStruct.MaxGapValue;
					temperature= SensorStruct.TempFilVal;
					peelmin = SensorStruct.MinPeelValue;
					peelmax = SensorStruct.MaxPeelValue;
					sprintf(gmsg,"S%04x.%04x.%04x.%04x.%04x.%04x.%04x\x0a",gab,peel,gabmin,gabmax,peelmin,peelmax,temperature); 
					goto OUT_END2;
	    	    case 0x0C: 		//Printer Chess Test
					PrtDrvSetPrePrintMode(1,64);
	    			PrtSensorFlagInit();
	    			if ((!Prt_CheckPeelAndHead(ON)) || (test_ok[12]==0xff)) {
	    				cc='E';
	    			} else {
	    				string_buf[0] = _ESC;
	    				string_buf[1] = 'T';
	    				string_buf[2] = _ESC;
	    				string_buf[3] = '!';
	    				string_buf[4] = 0x01;
	    				PutDataRxBuf(&CommBufPrt,(HUGEDATA char*)string_buf, 5);
//	    				while(CheckRxBuf(&CommBufPrt)) {
//	    					CASInterpreter(&CommBufPrt);
//	    				}
//	    				while(PrtDrvStruct.PrtState != PRT_STATE0I);
						PrintListWaitEndWithoutComm();
					cc='R';
	    	      		}
	    	    OUT_END1:	sprintf(gmsg,"%c%01X\x0a",cc,test_mode);
		    OUT_END2:	send=1;
	    			test_mode=0;
	    			break;
	    	    case 0x0F:  //goto EXIT;
				for (ret=1; ret<=12; ret++) {
					if (test_ok[ret]>2) {
						ret1=ret;
						break;
					}
				}
				if (ret1) {
					sprintf(gmsg,"Z%X\x0a",ret);
					goto F_END;
				}
	    			PrtSensorFlagInit();
				LabCmd_ClearLabel();
				StrInit4UserLabel();	// Added by CJK 20060426
				RcptCmd_SetFontType(PrtFontType);//0=one byte font, 1=two byte font //SG060313
				RcptCmd_SetFontId(5);
				RcptCmd_IntercharSP(0);
				LabCmd_CharSize(2,2);
				for (ret=0; ret<=13; ret++) {
					if (ret==0) {
						if (fversion.model[1] == 5)
							sprintf((char *)string_buf,"\x1bS[CL5200J V%c.%c%c.%c%c\n",fversion.version,fversion.reversion[0],fversion.reversion[1],fversion.subversion[0],fversion.subversion[1]);
						else
							sprintf((char *)string_buf,"\x1bS[CL3000 V%c.%c%c.%c%c\n",fversion.version,fversion.reversion[0],fversion.reversion[1],fversion.subversion[0],fversion.subversion[1]);
					} else if (ret==1) {
						sprintf((char *)string_buf,"\x1bS[=========================");
					} else {
						cp=(ROMDATA char *)debug_fjig_title[ret-2];
						for (send=0; send<13; send++) {
							if (cp[send]==0) break;
							gmsg[send]=cp[send];
						}
						for (ret1=send; ret1<13; ret1++) gmsg[ret1]='-';
						gmsg[ret1]='.';
						ret1=ret-1;
						if (test_ok[ret1])  {
							if (ret1==3)
								sprintf(&gmsg[13],"V%d.%02d",ADM_Status.Version/100,ADM_Status.Version%100);
							else
								strcpy(&gmsg[13],"O.K");
						} else          strcpy(&gmsg[13],"ERROR");

						if (ret==12) {
							sprintf((char *)string_buf,"\x1bS[%02d.%s (%d,%d)\n",ret1,gmsg,gab,peel);
						} else {
							sprintf((char *)string_buf,"\x1bS[%02d.%s\n",ret1,gmsg);
						}
					}
					ret1=strlen((char *)string_buf)+1;
					LabCmd_SetStrField(10,15+ret*16,100,428); // 8*56=448
					PutDataRxBuf(&CommBufPrt,(HUGEDATA char*)string_buf,ret1);
				}
				LabCmd_StrEnd();
				RcptCmd_TypeNone();
				LabCmd_PrintStart(1);
				PrintListWaitEnd();
				sprintf(gmsg,"R%01X\x0a",test_mode);
F_END:    			test_mode=0;
				send=1;
	    	}
SEND:
		if (send) commun_out(&CommBuf,gmsg);
	    	if ((INT16U)(SysTimer100ms-debug_timeout)>20) debug_factory_status=0;
	    } // if (!debug_factory_status)
	}
//	status_run.run_mode=RUN_FJIG_EXIT;
        Debugmode = 0;

}
