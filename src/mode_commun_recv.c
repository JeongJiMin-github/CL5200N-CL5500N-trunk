/*****************************************************************************
|*  Copyright		:	(c) 2015 CAS
|*  Filename		:	Mode_common_recv.c
|*  Version			:	0.1
|*  Programmer(s)	:	Chung jong kwon (CJK)
|*  Created			:	2015/09/18
|*  Description		:	CLP communication recieve data routine    
****************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
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
#include "adm.h"
#include "adm_app.h"
#include "key.h"
#include "main.h"
#include "cal.h"
#include "prt_cmdtest.h"
#include "programmode.h"
//#include "file.h"
#include "rtc.h"
#include "debug.h"
#include "common.h"
#include "mode_commun.h"
#include "plu_manager.h"
#include "plu_device.h"
#include "initial.h"
#include "flash_app.h"
#include "comm_app.h"
#include "caption_app.h"
#include "menu.h"
#include "prt_font.h"
#include "dsp_app.h"
#include "commbuf.h"
#include "ethernet.h"
#include "ethernet_comm.h"
#include "mode_error.h"
#include "global_set.h"
#include "discount.h"
#include "mode_sale.h"
#include "network_command.h"
#include "origin.h"
#include "global_set.h"
#include "plu_edit.h"
#include "department.h"
#include "tare_file.h"
#include "tax.h"
#include "pcs_unit.h"
#include "barcode_file.h"
#include "workplace.h"
#include "nutrition_fact.h"
#include "traceability.h"
#include "mode_calibration.h"
#include "sale_report.h"
#include "customer_app.h"
#include "shopname.h"
#include "clerk_app.h"
#include "key_typedef.h"
#include "currency.h"
#include "key_function.h"
#include "mode_sale.h"
#include "prt_app.h"
#include "ingredient.h"
#include "initial_glb.h"
#include "prt_advertise.h"
#include "network_set.h"
#include "inventory.h"
#include "license.h"
#include "sqi.h"
#include "scale_config.h"
#ifdef USE_PROTOCOL_CL5000JR
#include "cl5kj_interpreter.h"
#endif

extern INT8U global_one2one_send;
extern INT16U global_one2one_count;
extern INT16U key_pole_map[];
extern INT16U key_hanging_map[];
extern INT16U key_self_map[];
extern INT16U key_doublebody_map[];
#if defined(USE_CL5200_KEY)
extern INT16U key_CL5200_bench_map[];
extern INT16U key_CL5200_hanging_map[];
#endif
#ifdef USE_CTS_FUNCTION
//INT8U CtsCalCompReset = 0;
#endif
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
INT8U DispDisable;
#endif
// Static proto type
static INT8U commun_recv_plustrinfo(INT16S number,INT8U *str);
static INT8U commun_recv_plu(HUGEDATA COMM_BUF *CBuf,INT32U ipluno,INT16U ideptno,INT8U *str,INT16S leng);
static INT8U commun_recv_plusub(HUGEDATA COMM_BUF *CBuf,INT32U ipluno,INT16U ideptno,INT8U *str,INT16S leng);
static INT8U commun_recv_fontbin(INT16S number,INT8U *str,INT16S leng);
static INT8U commun_recv_fontinfo(INT16S number,INT8U *str,INT16S leng);
static INT8U commun_recv_key(INT16S key,INT16S key_id,INT8U *str,INT16S leng);
static INT8U commun_recv_caption(HUGEDATA COMM_BUF *CBuf,INT16S mode,INT16S id,INT8U *str);
static INT8U commun_recv_labelbmp(INT32U first,INT16U second,INT8U *str,INT16S leng);
static INT8U commun_recv_plu_part(HUGEDATA COMM_BUF *CBuf,INT32U ipluno,INT16U ideptno,INT8U *str,INT16S leng);
static INT16U commun_recv_block_addr(INT16S param0,INT16S param1,INT8U *str,INT16S leng);
static INT16U commun_recv_block(HUGEDATA COMM_BUF *CBuf,INT16S ipluno,INT16S ideptno,INT8U *str,INT16S leng);
static INT16U commun_recv_parameter(HUGEDATA COMM_BUF *CBuf,INT32U fcode,INT32U nn,INT8U *str,INT32U leng);
static INT8U commun_recv_pluinhibit(HUGEDATA COMM_BUF *CBuf,INT32U spart,INT16U sid,INT8U *str,INT16S leng);
static INT8U commun_recv_set(HUGEDATA COMM_BUF *CBuf,INT16S nth_block,INT16S data_type,INT8U *str,INT16S leng);
static INT8U commun_recv_table1(HUGEDATA COMM_BUF *CBuf,INT32U spart,INT16U sid,INT8U *str,INT16S leng);
static INT8U commun_recv_table2(HUGEDATA COMM_BUF *CBuf,INT32U spart,INT16U sid,INT8U *str,INT16S leng, INT8U cmd);
static INT8U commun_recv_table3(HUGEDATA COMM_BUF *CBuf,INT32U spart,INT16U sid,INT8U *str,INT16S leng, INT8U cmd);
static INT8U commun_recv_discount(HUGEDATA COMM_BUF *CBuf,INT16U cmd,INT16S xdcno,INT16S xmode,INT8U *str,INT16S leng);
static INT8U commun_recv_other(HUGEDATA COMM_BUF *CBuf,INT32U spart,INT16U sid,INT8U *str,INT16S leng);
#ifdef USE_REMOTE_FW_DOWNLOAD
static INT8U commun_recv_firmware(HUGEDATA COMM_BUF *CBuf, INT32U first,INT16U second,INT8U *str,INT16S leng);
#endif
#ifdef USE_REMOTE_ROM_UPDATE
static INT8U commun_recv_settingrom(HUGEDATA COMM_BUF *CBuf, INT32U first,INT16U second,INT8U *str,INT16S leng);
#endif
#ifdef USE_INDIA_FUNCTION
static INT8U commun_recv_inventory(HUGEDATA COMM_BUF *CBuf,INT32U spart,INT32U sid,INT8U *str,INT16S leng);
#endif
#ifdef USE_DIRECT_INGREDIENT
#ifdef USE_INGREDIENT_98_PROTOCOL
static INT8U commun_recv_directingredient(HUGEDATA COMM_BUF *CBuf,INT32U ipluno,INT16U ideptno,INT8U *str,INT16S leng);
#endif
#endif
#ifdef USE_DIRECT_NUTRITION
static INT8U commun_recv_direct_nutrition(COMM_BUF *CBuf,INT32U ipluno,INT16U ideptno,INT8U *str,INT16S leng);
#endif
static void  commun_recv_monitor(char key,INT16S xtype,INT16S v);

#ifdef USE_CHN_EMART_BIRDS_TRACE
static INT8U commun_recv_qr(HUGEDATA COMM_BUF *CBuf,INT32U pluno,INT16U deptno,INT8U *str,INT16S leng);
#endif

extern void Delay_10ms_w_TcpIp(INT16U cnt);

//"W<cmd>A<number>,<nn>L<length>:<data><bcc>"
//"W<cmd>F<number>,<nn>L<length>:<data><bcc>"
void commun_write(HUGEDATA COMM_BUF *CBuf,INT8U cmd,INT32U number,INT32U nn,INT32U length,INT8U *str)
{
	INT16U ret16;
	INT8U com_type;
	INT8U chk;
	INT8U noheader = OFF;
	INT8U precmd;
	com_type=serial_get_comtype(CBuf);
	status_run.run_option|=0x0001;
	recv_counter++;
	precmd = 0;

	chk=1; 
	if (length<=0) { chk=0; ret16=0x82; goto ERR; }
//sprintf(MsgBuf, "<SRW>cmd = %02X, n=%ld, nn=%ld, len=%ld\r\n", cmd, number, nn, length);
//MsgOut(MsgBuf);	// CJK090105
	switch (cmd) {
		case 0x001: ret16=(INT16U)commun_recv_plustrinfo((INT16S)number,str);
		         break;
		case 0x002: ret16=(INT16U)commun_recv_plu(CBuf,(INT32U)number,(INT16U)nn,str,length);
			 break;
#ifdef USE_CHN_EMART_BIRDS_TRACE
		case 0x050: ret16=(INT16U)commun_recv_qr(CBuf,(INT32U)number,(INT16U)nn,str,(INT16S)length);
				 //chk=0;
			 break;
#endif
		case 0x052: ret16=(INT16U)commun_recv_plusub(CBuf,(INT32U)number,(INT16U)nn,str,length);
			 break;
		case 0x003: if (number) {
				ret16=(INT16U)commun_recv_fontbin((INT16S)number,str,(INT16S)length);
			 } else {
				ret16=(INT16U)commun_recv_fontinfo((INT16S)nn,str,(INT16S)length);
			 }
			 break;
		case 0x004: ret16=(INT16U)commun_recv_key((INT16S)number,(INT16S)nn,str,(INT16S)length);
			 break;
		case 0x005: 
#ifdef	USE_CHECK_CAL_SW_TO_UPDATE_DATAROM
			/* EXT CAP Block */
			if(CAL)
			{
				if(nn == 0x18)
				{
					ret16 = 0;
					chk = 0;
					break;
				}
			}
#endif	// USE_CHECK_CAL_SW_TO_UPDATE_DATAROM
#ifdef USE_EXTEND_CAPNTION_AREA
			// 확장 caption이 없는 펌웨어에서 error를 리턴하기 위해 number(block)에 3을 더해서 전송하도록 함
			// number가 3이상이면, commun_rev_caption() 함수에서 error 리턴함
			if (number >= 3) 
			{
				ret16=commun_recv_block(CBuf,(INT16S)number-3,(INT16S)nn,str,(INT16S)length);
				chk = 2;
			}
			else
			{
				ret16=(INT16U)commun_recv_caption(CBuf,(INT16S)number,(INT16S)nn,str);
			}
#else
			ret16=(INT16U)commun_recv_caption(CBuf,(INT16S)number,(INT16S)nn,str);
#endif
			 break;
		case 0x006: ret16=(INT16U)commun_recv_labelbmp((INT32U)number,(INT16U)nn,str,length);
			 break;
		case 0x007: ret16=(INT16U)commun_recv_plu_part(CBuf,(INT16S)number,(INT16S)nn,str,length);
			 break;
		case 0x029: ret16=(INT16U)commun_recv_block_addr((INT16S)number,(INT16S)nn,str,length);
			 break;
		case 0x008:
		case 0x082:
		case 0x088: ret16=commun_recv_block(CBuf,(INT16S)number,(INT16S)nn,str,(INT16S)length);
			    chk=2;
			 break;
		case 0x085: ret16=commun_recv_parameter(CBuf,number,nn,str,length);
				chk=2;
			 break;
		case 0x036: ret16=commun_recv_pluinhibit(CBuf,(INT16S)number,(INT16S)nn,str,(INT16S)length);
			    chk=0;
			    break;
		case 0x089: ret16=(INT16U)commun_recv_set(CBuf,(INT16S)number,(INT16S)nn,str,(INT16S)length);
			    chk=2;
			 break;
		case 0x020: ret16=(INT16U)commun_recv_table1(CBuf,(INT32U)number,(INT16U)nn,str,(INT16S)length);
			    chk=0;
			   break; 
		case 0x072:	// scale(master)->scale(slave) for data sync
		case 0x030: ret16=(INT16U)commun_recv_table2(CBuf,(INT32U)number,(INT16U)nn,str,(INT16S)length, cmd);
			    chk=0;
			   break;
		case 0x073:	// scale(master)->scale(slave) for data sync
		case 0x032: ret16=(INT16U)commun_recv_table3(CBuf,(INT32U)number,(INT16U)nn,str,(INT16S)length, cmd);
			    chk=0;
			   break; 
		case 0x019: 
		case 0x009: ret16=(INT16U)commun_recv_discount(CBuf,(INT16U)cmd,(INT16S)number,(INT16S)nn,str,(INT16S)length);
			    break;
		case 0x045: ret16=(INT16U)commun_recv_other(CBuf,(INT32U)number,(INT16U)nn,str,(INT16S)length);
#ifdef USE_CTS_FUNCTION
			    if (number == 0x13 || number == 0x14 || number == 0x15 || number == 0x16 || number == 0x17) 
					noheader = ON; // CTS일 때 header 전송안 함
#endif
			    break;
#ifdef USE_INDIA_FUNCTION
		case 0x040: ret16=(INT16U)commun_recv_inventory(CBuf, number, nn, str, length);
			 break;
#endif
#ifdef USE_DIRECT_INGREDIENT
#ifdef USE_INGREDIENT_98_PROTOCOL
		case 0x046: ret16=(INT16U)commun_recv_directingredient(CBuf, number, nn, str, length);
			 break;
#endif
#endif
#ifdef USE_REMOTE_FW_DOWNLOAD
        case 0x099: ret16=(INT16U)commun_recv_firmware(CBuf, (INT32U)number,(INT16U)nn,str,length);
             break;
#endif
#ifdef USE_REMOTE_ROM_UPDATE
        case 0x098: ret16=(INT16U)commun_recv_settingrom(CBuf, (INT32U)number,(INT16U)nn,str,length);
             break;             
#endif             
#ifdef USE_DIRECT_NUTRITION
		case 0x047: ret16=(INT16U)commun_recv_direct_nutrition(CBuf, number, nn, str, length);
			break;
#endif
		default:    ret16=FALSE;
	}
ERR:
	if (cmd==0x89) 
	{
		precmd=0x89; 
		cmd=0x88;
	}

	if ((com_type==0) || (ethernet_list.status)) 
	{
		if ((nn != 5) || (precmd != 0x89)) {
#if defined(USE_REMOTE_FW_DOWNLOAD) || defined(USE_REMOTE_ROM_UPDATE) 
            if ((cmd != 0x99) && (cmd != 0x98))
#endif
			{
                commun_write_err16(CBuf,(INT8U)cmd,ret16,chk,noheader);
            }
        }
	}

	status_run.run_option&=0xfffe;
	if ((cmd==8) && (ret16==3)) {
		_SOFTWARE_RESET;
	}
	if ((precmd == 0x89) && (nn == 9) && (ret16 == 1)) {
		Delay_10ms_w_TcpIp(20);//for PC정상종료
		_SOFTWARE_RESET;
	}
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
	DispDisable =0;
	if ((precmd == 0x89) && (nn == 5) && (ret16 == 1)) {
		DispDisable = 1;
	}
#endif
#ifdef USE_CTS_FUNCTION
//	if ((cmd == 0x045) && (CtsCalCompReset == ENCRYPT_CALCOMP)) {
//		Delay_100ms_w_net(10, OFF, ON);
//		_SOFTWARE_RESET;
//	}
#endif
}

INT8U commun_recv_plustrinfo(INT16S number,INT8U *str)
{
	INT16S r,status;
	INT8U code,v8,ret;
	INT32U pnt,p,v32;
	INT16U v16;
	INT8U plustr[PLU_STRUCT_SIZE];
	INT8U plustr_name[PLUSTR_NAME_SIZE];

	r     =0;
	code  =0;
	status=0;
	if (number<=0) { ret=0x80; goto RET; }
	if (number==0xfff) {
		while (1) {
			switch (status) {
				case 0:if (str[r]=='N') status=1;
				       break;
				case 1:status=0;
				       v16=0;
				       if (str[r]=='=') status=2;
				       break;
				case 2:if (str[r]=='.') {
					   plu_struct_set_nfield(v16);
					   status=0;
					   Flash_flush_nvbuffer();
					   _SOFTWARE_RESET;
				       }
				       v16<<=4;
				       v16|=ctoh(str[r]);
				       break;
			}
			r++;
			if (r>80) break;
		}
		ret=0x82;
		goto RET;
	}
	pnt  = DFLASH_BASE;
	pnt += FLASH_PLU_STRUCT_AREA;
	pnt += PLU_STRUCT_POINT;
	pnt += PLU_STRUCT_SIZE*(number-1);
	plustr_name[0]=0;
	Flash_sread(pnt,plustr,PLU_STRUCT_SIZE);
	while (1) {
		switch (status) {
			case 0:	code=str[r];
				if (code==0) goto END;
				v32=0; v16=0; v8=0;
				status=1;
				break;
			case 1: if (str[r]=='=') {
				   switch (code) {
				   	case 'C': status=10; break; // p=PLUSTR_CAPTION_POINT;  break;
					case 'P': status=20; p=PLUSTR_PTYPE_POINT;    break;
					case 'D': status=30; p=PLUSTR_VDEFAULT_POINT; break;
					case 'M': status=40; p=PLUSTR_MAXVALUE_POINT; break;
					case 'S': status=50; p=PLUSTR_STYPE_POINT;    break;
					case 'L': status=60; p=PLUSTR_LENGTH_POINT;   break;
					case 'N': status=70; p=PLUSTR_LINENO_POINT;   break;
					case 'O': status=80; p=PLUSTR_DPOINT_POINT;   break;
					case 'Z': status=90; p=PLUSTR_USIZE_POINT;    break;
					default : goto END;
				   }
				}
				break;
			case 10:v16 =ctoh(str[r]); status++; break;
			case 11:v16<<=4;
				v16|=ctoh(str[r]); status++; break;
			case 12:
				memset(plustr_name,0,PLUSTR_NAME_SIZE);
				memcpy(plustr_name,&str[r],v16);
				r+=v16;
				status=0;
				goto SKIP;
			case 20:
			case 90:if (str[r]=='.') {
					memcpy(&plustr[p],(INT8U *)&v16,2);
					status=0;
					break;
				} else {
					v16<<=4; v16|=ctoh(str[r]);
				}
				break;
			case 30:
			case 40:if (str[r]=='.') {
					memcpy(&plustr[p],(INT8U *)&v32,4);
					status=0;
					break;
				}
				v32<<=4; v32|=ctoh(str[r]); break;
			case 50:
			case 60:
			case 70:
			case 80:if (str[r]=='.') {
					plustr[p]=v8;
					status=0;
					break;
				}
				v8<<=4; v8|=ctoh(str[r]);
				break;
		}
		r++;
SKIP:;
	}
END:
	ret=1;
	Flash_swrite(pnt,plustr,PLU_STRUCT_SIZE);
	if (plustr_name[0]) {
		pnt  = DFLASH_BASE;
		pnt += FLASH_PLU_STRUCT_CAPTION_AREA;
		pnt += PLUSTR_NAME_SIZE*(number-1);
		Flash_swrite(pnt,plustr_name,PLUSTR_NAME_SIZE);
	}
RET:
	return ret;
}

INT8U commun_recv_plu(HUGEDATA COMM_BUF *CBuf,INT32U ipluno,INT16U ideptno,INT8U *str,INT16S leng)
{
	INT16S    ret;
	INT32U pluno,v32,key;
	INT8U *xpos_base;
	INT8U *xpos;
	INT32U pos;
	INT8U  stype,deptno,code;
	INT16U ptype,dleng,usize,idx;
	ETH_CONNECT_LIST eth;
	INT16S status,r,i;
	char  *commun_buffer;
	INT8U *pdata,ox;
	INT8U traceChanged = OFF;	//축산데이터 입력 flag
	INT8U controlPriceChg = OFF;	//가격변경제한데이터 입력 flag
#ifdef USE_DIRECT_INGREDIENT//DEL080826#if (ROM_COUNTRY == ROM_RUS)
	INT16U 	chkIngredTextMemFull;
	INT16U  temp16u;
	INT32U  keyindex;
	HUGEDATA INT8U *dmsg;
	HUGEDATA INT8U *nv_point;
	INT16U ingred_no[DIRECT_INGREDIENT_BLOCK_NUM];
	INT16U ingred_tempno[DIRECT_INGREDIENT_BLOCK_NUM];
	INT16U ingredient_totallength;
	INT8U ingredient_99_98_select=0;
	INT8U empty_string[1];
#endif
#ifdef USE_PLU_WITH_TRACE_PROTOCOL
	PLU_INDIVIDUAL_NODE_DATA rcvTrace;
	INT16U dlengTemp;
	INT16U ind_no;
#endif
#ifdef USE_CONTROL_CHANGING_PRICE
	INT16U field70 = 0;
	INT8U ctrlPriceChange = 0;
	INT8U upperLimit = 0;
	INT8U lowerLimit = 0;
#endif
#ifdef USE_DIRECT_INGREDIENT
	INT8U  useIngredientTextField=0;
#endif
#ifdef USE_CHN_EMART_TRACE
	INT8U trace_code[CHN_EMART_TRACE_CODE_SIZE + 1];
	INT8U trace_not_available;
#endif
#ifdef USE_EMART_DEFAULT
	INT8U tmpStrBuf[EMART_PLU_NAME4_MAX_SIZE];
	INT8U changed_flag;
#endif

#ifdef USE_CHN_EMART_TRACE
	memset(trace_code, 0, sizeof(trace_code));
#endif
#ifdef USE_EMART_DEFAULT
	memset(tmpStrBuf, 0, sizeof(tmpStrBuf));
#endif
	memset((INT8U *)&eth,0,sizeof(ETH_CONNECT_LIST));
	deptno = (INT8U)ideptno;
	pluno  = (INT32U)ipluno;
	commun_buffer=(char *)serial_send_point;

	if (Startup.disable_dept) deptno = 1;
	if (pluno <=0) return 0x82;
	if (leng <0  ) return 0x84;
	if (deptno<=0) pluno=MAX_PLU_NUMBER+1;	 
	ret = 0x82; // plu no mismatch
	if (pluno>MAX_PLU_NUMBER) goto END3;

	ret    = 1;
	if (!plu_table_search(deptno,pluno,(INT16U *)&idx,0)) {
		key = plu_address_get_new();
		if (key==0) return 0x99;
		plu_temp_clear(CBuf->Type);
	} else {
		key = (INT32U)idx;
		plu_move_temp(CBuf->Type,key-1);
	}
	status = 0;
	r      = 0;
	xpos_base=get_temp_area(CBuf->Type);
#ifdef USE_PLU_WITH_TRACE_PROTOCOL
	memset ((INT8U*)&rcvTrace, 0, sizeof(rcvTrace));
	TNT_GetTraceNoTable(deptno, pluno, &ind_no, rcvTrace.indivNo[0], rcvTrace.indivNo[1], rcvTrace.indivNo[2],&rcvTrace.slaughter_place[0],&rcvTrace.slaughter_place[1],&rcvTrace.slaughter_place[2],&rcvTrace.meat_use, &rcvTrace.meat_part, &rcvTrace.isLot, &rcvTrace.meat_grade);
#endif	
	while (1) {
		switch (status) {
			case  0:dleng=0;
				ptype=0;
				stype=0;
				v32  =0;
				if (r>=leng) goto END2;
				code=str[r];
				if (code==0  ) goto END2;
				else if (code=='F') status=1;
				else status=20;
				break;
			case  1:ptype=0;
				if (str[r]=='=') status=2;
				else status=0;
				break;
			case  2:if (str[r]=='.') {
					status=3;
					break;
				}
				ptype<<=4;
				ptype|=ctoh(str[r]);
				break;
			case  3:if (str[r]==',') {
					status=4;
					break;
				}
				stype<<=4;
				stype|=ctoh(str[r]);
				break;
			case  4:if (str[r]==':') {
					status=10;
					break;
				}
				dleng<<=4;
				dleng|=ctoh(str[r]);
				break;
			case 10:
				xpos = xpos_base;
				xpos +=(INT32U)plu_get_ptype_point3((INT16U)ptype,(INT8U *)&stype,&usize);
#ifdef USE_DIRECT_INGREDIENT//DEL080826#if (ROM_COUNTRY == ROM_RUS)
				//useIngredientTextField =0;  //for test
				if(ptype==PTYPE_INGREDIENT_SIZE)
				{
					ingredient_totallength = dleng;
					ingredient_99_98_select = PTYPE_INGREDIENT_SIZE;
				}
				if(ptype==PTYPE_INGREDIENT_TEXT) //SG070731.Direct ingredient
				{
					xpos = xpos_base;
					xpos += plu_record_size;
					xpos += 4;
					usize = DEFAULT_INGREDIENT_TEXT_ALL_SIZE;
					//useIngredientTextField =1;
					ingredient_99_98_select = PTYPE_INGREDIENT_TEXT;
				}
#endif
				if (usize) 
				{
					if(ptype < 128)
					{
						pdata=(INT8U*)xpos;
#ifdef USE_PLU_TIMELEN_CUT
						if((ptype== 17 || ptype== 19) && dleng==2)
						{
							for (i=0; i<1; i++) *pdata++=str[r++];  // Length=2 경우 1로 변경(중국 프로그램용)
							r++;
						}
						else
						{
							for (i=0; i<dleng; i++) *pdata++=str[r++];
						}
#else
						for (i=0; i<dleng; i++) *pdata++=str[r++];
#endif
						for (i=dleng; i<usize; i++) *pdata++=0;
#ifdef USE_CHN_EMART_TRACE
						if (ptype == PTYPE_PLU_NAME8) //  상품명8단
						{
							memcpy(trace_code, (INT8U*)xpos, sizeof(trace_code));
						}
#endif
#ifdef USE_CONTROL_CHANGING_PRICE
						if(ptype == PTYPE_NUTRITION_NO)		//Nutritino No.
						{
							status=0;
                            				goto SKIP;
						}
#endif
					}
#if defined(USE_PLU_WITH_TRACE_PROTOCOL) || defined(USE_CONTROL_CHANGING_PRICE)
					else
					{
						if((ptype >= 128) && (ptype <= 137))
						{
							traceChanged = ON;
						}
						if((ptype >= 140) && (ptype <= 142))
						{
							controlPriceChg = ON;
						}
						switch(ptype)
						{
#ifdef USE_PLU_WITH_TRACE_PROTOCOL
							case 128:
								memcpy(&rcvTrace.isLot, &str[r], dleng);
								break;
							case 129:
								dlengTemp = dleng;
								if(dlengTemp > INDIVIDUAL_NO_LEN)
								{
									dlengTemp = INDIVIDUAL_NO_LEN;
								}
								memcpy(rcvTrace.indivNo[0], &str[r], dlengTemp);
								if(dlengTemp < INDIVIDUAL_NO_LEN)
								{
									rcvTrace.indivNo[0][dlengTemp] = 0;
								}
								break;
							case 130:
								dlengTemp = dleng;
								if(dlengTemp > INDIVIDUAL_NO_LEN)
								{
									dlengTemp = INDIVIDUAL_NO_LEN;
								}
								memcpy(rcvTrace.indivNo[1], &str[r], dlengTemp);
								if(dlengTemp < INDIVIDUAL_NO_LEN)
								{
									rcvTrace.indivNo[1][dlengTemp] = 0;
								}
								break;
							case 131:
								dlengTemp = dleng;
								if(dlengTemp > INDIVIDUAL_NO_LEN)
								{
									dlengTemp = INDIVIDUAL_NO_LEN;
								}
								memcpy(rcvTrace.indivNo[2], &str[r], dlengTemp);
								if(dlengTemp < INDIVIDUAL_NO_LEN)
								{
									rcvTrace.indivNo[2][dlengTemp] = 0;
								}
								break;			
							case 132:
								memcpy(&rcvTrace.slaughter_place[0], &str[r], dleng);
								break;
							case 133:
								memcpy(&rcvTrace.slaughter_place[1], &str[r], dleng);
								break;
							case 134:
								memcpy(&rcvTrace.slaughter_place[2], &str[r], dleng);
								break;
							case 135:
								memcpy(&rcvTrace.meat_use, &str[r], dleng);
								break;
							case 136:
								memcpy(&rcvTrace.meat_part, &str[r], dleng);
								break;
							case 137:
								memcpy(&rcvTrace.meat_grade, &str[r], dleng);
								break;
#endif
#ifdef USE_CONTROL_CHANGING_PRICE
							case 140:
								memcpy(&ctrlPriceChange, &str[r], dleng);
								break;
							case 141:
								memcpy(&upperLimit, &str[r], dleng);
								break;
							case 142:
								memcpy(&lowerLimit, &str[r], dleng);
								break;
#endif
						}
						r += dleng;
					}
#endif	//#if defined(USE_PLU_WITH_TRACE_PROTOCOL) || defined(USE_CONTROL_CHANGING_PRICE)
				}
				else 
				{
					r+=dleng;
				}
				status=0;
				goto SKIP;
			case 20:status=0;
				if (str[r]=='=') status=21;
				break;
			case 21:if (str[r]=='.') {
					if (code=='N') {
						if (v32==0) return 0x84;
#ifdef USE_INDIA_FUNCTION
					} else if (code == 'b') {
						WritePluBagWt(deptno, pluno, v32);
#endif
					} else {
						commun_common_ethernet(code,&eth,v32);
					}
					status=0;
				} else {
					v32<<=4;
					v32|=ctoh(str[r]);
				}
				break;
		}
		r++;
		SKIP:;
	}
END2:	if ((deptno<1) || (deptno>=100)) goto END_R;
	pos=plu_get_ptype_point(PTYPE_DEPARTMENT_NO,&stype,&usize);
	plu_save_deptid_com_type(CBuf->Type,pos,(INT32U)deptno,stype,usize);
	pos=plu_get_ptype_point(PTYPE_PLU_NO,&stype,&usize);
	plu_save_deptid_com_type(CBuf->Type,pos,(INT32U)pluno,stype,usize);


#ifdef USE_DIRECT_INGREDIENT//DEL080826#if (ROM_COUNTRY == ROM_RUS) //SG070830
	if(ingredient_99_98_select == PTYPE_INGREDIENT_TEXT)
	{
		dmsg = (HUGEDATA INT8U *)(get_temp_area(CBuf->Type) + (INT32U)plu_record_size + 4);
		nv_point = (HUGEDATA INT8U *)(get_temp_area(CBuf->Type) + plu_get_ptype_point(PTYPE_INGREDIENT_TEXT,NULL,(INT16U *)&usize));
		memcpy((INT8U *)&ingred_no[0], nv_point, 2*DIRECT_INGREDIENT_BLOCK_NUM);
		chkIngredTextMemFull = ingredient_text_write(&ingred_no[0], dmsg, 0, DEFAULT_INGREDIENT_TEXT_ALL_SIZE, 1);
		memcpy(nv_point, (INT8U *)&ingred_no[0], 2*DIRECT_INGREDIENT_BLOCK_NUM);
		if (!chkIngredTextMemFull) return 0x88;	// direct ingredient momory full
		ingredient_99_98_select = 0;
	}
	if(ingredient_99_98_select == PTYPE_INGREDIENT_SIZE)
	{
		if(dleng == 0) // length가 0일 경우 Ingredient 삭제
		{
			empty_string[0] = 0;
			nv_point = (HUGEDATA INT8U *)(get_temp_area(CBuf->Type) + plu_get_ptype_point(PTYPE_INGREDIENT_TEXT,NULL,(INT16U *)&usize));
			memcpy((INT8U *)&ingred_no[0], nv_point, 2*DIRECT_INGREDIENT_BLOCK_NUM);
			chkIngredTextMemFull = ingredient_text_write(&ingred_no[0], empty_string, 0, DEFAULT_INGREDIENT_TEXT_ALL_SIZE, 0);
			memcpy(nv_point, (INT8U *)&ingred_no[0], 2*DIRECT_INGREDIENT_BLOCK_NUM);
 		}
		ingredient_99_98_select = 0;
	}
/*
	xpos = plu_get_ptype_point(PTYPE_INGREDIENT_TEXT,(INT8U *)&stype,&usize);
	xpos += xpos_base;
	if(usize)
	{
		memcpy((INT8U *)&temp16u,(HUGEDATA INT8U *)xpos,2);
		if(useIngredientTextField)
		{
			//D.I 있을때 --> 기존 값이 있으면 그위치에 덮어쓰고, 없으면 New index에 Write.
			keyindex = (INT32U)deptno;
			keyindex = (keyindex<<24) | (pluno & 0x00ffffff);
			chkIngredTextMemFull = ingred_text_save(CBuf->Type, keyindex, temp16u);  //SG070731
			if(chkIngredTextMemFull == 0xffff) return 0x88; //direct ingredient memory full
		}
		else
		{
			//D.I 없을때 --> 기존 값 있는지 체크해서 있으면 삭제, 없으면 그냥 통과
			if(temp16u > 0 && temp16u <= MAX_INGREDIENT_TEXT_NUMBER)
				ingred_text_delete_temp(CBuf->Type, keyindex, temp16u);  //PLU structure 에 값 0 저장하고, D.I 위치 Clear
		}
	}
*/
#endif
	plu_restore_temp(CBuf->Type,(long)key-1);

#ifdef USE_CHN_EMART_TRACE
	//plu_get_field(idx-1, PTYPE_PLU_NAME4, (INT8U *)&trace_code, &usize, CHN_EMART_TRACE_CODE_SIZE);
	if (isdigit(trace_code[0]))
	{
		// check trace code availability
		trace_not_available = OFF;
		for (i = 0; i < CHN_EMART_TRACE_CODE_SIZE; i++)
		{
			if (trace_code[i] == 0) break;
			if (!isdigit(trace_code[i])) trace_not_available = ON;
		}
		// backup trace code
		if (trace_not_available) // 수신된 trace code가 숫자가 아닐때(빈칸 포함) => 최근 trace code로 복구
		{
			chn_emart_trace_read(key-1, trace_code, CHNEmart_TraceSelectionOrder);// 최근 trace code
			plu_set_field(key-1, PTYPE_PLU_NAME8, (INT8U *)&trace_code);
		}
		else
		{
			chn_emart_trace_save(key-1, trace_code);
		}
	}

#endif

#ifdef USE_EMART_DEFAULT
	plu_get_field(key-1, PTYPE_PLU_NAME4, (INT8U *)&tmpStrBuf, &usize, EMART_PLU_NAME4_MAX_SIZE);
	changed_flag = FALSE;
	for(i=0; i<EMART_PLU_NAME4_MAX_SIZE; i++)
	{
		if(tmpStrBuf[i] == '^')
		{
			tmpStrBuf[i] = LF;
			changed_flag = TRUE;
		}
	}
	if( changed_flag )
	{
		plu_set_field(key-1, PTYPE_PLU_NAME4, (INT8U *)&tmpStrBuf);
	}
#endif

#ifdef USE_CONTROL_CHANGING_PRICE
	if(controlPriceChg)
	{
		field70 = (ctrlPriceChange * 10000) + (upperLimit * 100) + lowerLimit;
		plu_set_field(key-1, PTYPE_NUTRITION_NO, (INT8U *)&field70);	//field 70 에 매가 변경 통제 제어값 넣음. 
	}
#endif
	if (network_status.service_flag&0x20)	// send plu
	{
		if (ethernet_list.status == 1)
		{
			commun_net_mask(0, (INT16U *)&key, 0xff, 1);
		}
	}
	plu_table_save(deptno,pluno);
#ifdef USE_PLU_WITH_TRACE_PROTOCOL
	if(traceChanged)
	{
		TNT_SetTraceNoTable(deptno, pluno, rcvTrace.indivNo[0],rcvTrace.indivNo[1], rcvTrace.indivNo[2], rcvTrace.slaughter_place[0], rcvTrace.slaughter_place[1], rcvTrace.slaughter_place[2], rcvTrace.meat_use, rcvTrace.meat_part, rcvTrace.isLot, rcvTrace.meat_grade);
	}
#endif
	if ((status_scale.cur_pluid==pluno) && ((INT8U)status_scale.departmentid==deptno)) {
		ox = network_status.remote_plu;
		network_status.remote_plu = 0;
		keyapp_pluno((INT8U)deptno,pluno,OFF);
		network_status.remote_plu=ox;
	}
END3:
#ifdef USE_MAX_SCALE_NO_255
	if ((eth.scaleid >= 1) && (eth.scaleid <= 255)) //from scale 
#else
	if ((eth.scaleid >= 1) && (eth.scaleid <= MAX_SCALE - 1)) //from scale 
#endif
	{
		r=ethernet_gen_head(commun_buffer);
		if (status_run.run_mode==RUN_BACKUP) 
		{
			//SG061227. PLU 일대일 Backup 시 ACK
			sprintf(&commun_buffer[r],"W29:O0001");
			r+=9;
		} 
		else
		{
			sprintf(&commun_buffer[r],"C02F14,%02X%06lX",deptno ,pluno);
			r+=15;
		}
		commun_buffer[r++]=0x0a;
		commun_outleng(CBuf,(char *)commun_buffer,r);
		ret=0;
	} 
	else { } 				      //from pc
	
END_R:
	network_status.load_plu=1;
	return ret;
}

INT8U commun_recv_plusub(HUGEDATA COMM_BUF *CBuf,INT32U ipluno,INT16U ideptno,INT8U *str,INT16S leng)
{
	INT16S    ret;
	INT32U pluno,v32,key;
	INT8U *xpos_base;
	INT8U *xpos;
	INT32U pos;
	INT8U  stype,deptno;
	INT16U usize,idx,v16;
	INT16S i;
	INT16S  z,new_leng;
	INT8U *pdata,ox,v8,ctype;
	char temp[41],newtable;
	const INT16S table_pnt[]={ 0, 2, 8,14,15,17,23,29,69,0};
	const INT16S table_len[]={ 2, 6, 6, 1, 2, 6, 6,40,40,0};
	const char table_typ[]={1, 2,11, 4,14, 6,13,10,30,0};

	deptno = (INT8U )ideptno;
	pluno  = (INT32U)ipluno ;

	if (Startup.disable_dept) deptno = 1;
	if (pluno <=0) return 0x82;
	ret = 0x82; // plu no mismatch
	if ((deptno<=0) || (pluno>MAX_PLU_NUMBER)) goto END3;
	ret    = 1;
	newtable=0;
	ctype=CBuf->Type;
	if (!plu_table_search(deptno,pluno,(INT16U *)&idx,0)) {
		newtable=1;
		key = plu_address_get_new();
		if (key==0) return 0x99;
		plu_temp_clear(ctype);
	} else {
		key = (INT32U)idx;
		plu_move_temp(ctype,key-1);
	}
	xpos_base=get_temp_area(ctype);
	for (i=0; i<9; i++) {
		z=table_pnt[i]+table_len[i];
		new_leng=table_len[i];
		ox = table_typ[i];
		if (z>leng) {
			new_leng=0;
			if (table_pnt[i]>leng) new_leng=leng-table_pnt[i];
		}
		if (new_leng) {
			memcpy(temp,&str[table_pnt[i]],new_leng);
		} else {
			memset((char *)temp,0,table_len[i]);
		}
		temp[table_len[i]]=0;
		xpos = xpos_base;
		xpos +=(INT32U)plu_get_ptype_point((INT16U)table_typ[i],(INT8U *)&stype,&usize);
		if (ox==16) {
		    v32 =(INT32U)atol((char *)temp);
		    memcpy(temp,&v32,4);
		} else {
		    switch (usize) {
		        case 1: v8 =(INT8U)atoi(temp);
				temp[0]=v8;
				break;
			case 2: v16 =(INT16U)atoi(temp);
				memcpy(temp,&v16,2);
				break;
			case 4: v32 =(INT32U)atol(temp);
				memcpy(temp,&v32,4);
				break;
			case 0: break;
		   }
		}
		if (ox==1) deptno=(INT8U)v16;
		if (ox==2) pluno =v32;
		if (usize) {
			pdata=(INT8U*)xpos;
			memcpy(pdata,temp,usize);
		}
	}
	if ((deptno<1) || (deptno>=100)) goto END_R;
	if (newtable) {
		pos=plu_get_ptype_point(PTYPE_DEPARTMENT_NO,&stype,&usize);
		plu_save_deptid_com_type(ctype,pos,(INT32U)deptno,stype,usize);
		pos=plu_get_ptype_point(PTYPE_PLU_NO,&stype,&usize);
		plu_save_deptid_com_type(ctype,pos,(INT32U)pluno ,stype,usize);
	}
	plu_restore_temp(ctype,(long)key-1);
	if (network_status.service_flag&0x20)	// send plu
	{
		if (ethernet_list.status == 1)
		{
			commun_net_mask(0, (INT16U *)&key, 0xff, 1);
		}
	}
	if (newtable) plu_table_save(deptno,pluno);

	if ((status_scale.cur_pluid==pluno) && ((INT8U)status_scale.departmentid==deptno)) {
		ox = network_status.remote_plu;
		network_status.remote_plu = 0;
		keyapp_pluno((INT8U)deptno,pluno,OFF);
		network_status.remote_plu=ox;
	}
END3:
END_R:
	network_status.load_plu=1;
	return ret;
}

INT8U commun_recv_fontbin(INT16S number,INT8U *str,INT16S leng)
{
	INT32U src_addr,v32,addr,font_pnt,font_max;
	INT8U  ret,cc,recv_mode;
	INT16S    status,r,save_size;

	src_addr  = DFLASH_BASE;
	src_addr += FLASH_FONT_AREA;
	ret=1;
	if (number==0) { ret=0x83; goto END; }
	if (leng==0)   { ret=0x84; goto END; }
	status  =0;
	r       =0;
	recv_mode=0;
	// S=0.D####:<data>
	// S=0: continues.... S=1: End
	while (1) {
	    switch (status) {
	    	case 0: cc=str[r];
			v32=0;
	    		status = 1;
	    		if (cc==0) goto END;
			if (cc=='D') status=10;
	    		break;
	    	case 1: status=0;
	    		if (str[r]=='=') { v32=0; status=2; }
	    		break;
	    	case 2: if (str[r]=='.') {
	    		    if (cc=='S') recv_mode=(INT8U)v32;
	    		    status = 0;
	    		} else {
	    		    v32<<=4;
	    		    v32|=ctoh(str[r]);
	    		}
	    		break;
		case 10:if (str[r]==':') {
				status=11;
			} else {
				v32<<=4;
				v32|=ctoh(str[r]);
			}
			break;
		case 11:save_size=(INT16S)v32;
	    		font_pnt = (number-1);
	    		font_pnt*= 512;
	    		addr  = src_addr;
	    		addr += font_pnt;
			addr += FONT_IMAGE;
			font_max  = font_pnt;
			font_max += save_size;
			if (font_max>=FONT_MAX_SIZE) { ret=0x85; goto END; }
//			status_run.run_option|=0x0001;
			Flash_swrite(addr,(HUGEDATA INT8U *)&str[r],save_size);
//			status_run.run_option&=0xfffe;

	     		ret=2;
			if (recv_mode) {
				font_pnt+=save_size;
				addr = src_addr;
				addr += FONT_IMAGE_SIZE;
				Flash_lwrite(addr,font_pnt);
				ret=3;
			}
			r+=save_size;
			status=0;
	    		goto NEXT_R;
	    }
	    r++;
NEXT_R:;
	}
END:
	return ret;
}

INT8U commun_recv_fontinfo(INT16S number,INT8U *str,INT16S leng)
{
	INT16S    r,ret,status,ith;
	INT32U pnt,addr,v32;
	INT8U  cc,v8;

	if (number>FONT_MAX_NO) return 0x82;
	if (leng==0 ) return 0x82;
	r     =0;
	status=0;
	addr  = DFLASH_BASE;
	addr += FLASH_FONT_AREA;
	ith   = 0;
	ret   = 0;
	while (1) {
		switch (status) {
		     case 0: cc=str[r];
			     if (cc==0) goto END;
			     status = 1;
			     break;
		     case 1: status=0;
			     if (str[r]=='=') {
				status=2; v32=0;
			     }
			     break;
		     case 2: if (str[r]=='.') {
				switch (cc) {
					case 'N': pnt   = addr + FONT_NUMBER;
						  v8 = (INT8U)v32;
						  Flash_bwrite(pnt,v8);
						  v8 = 0xff;
						  Flash_bwrite(pnt+1,v8);
						  ret=1;
						  break;
					case 'i': ith =(INT16S)v32;
						  if ((ith<=0) || (ith>FONT_MAX_NO)) { ret=0x84; goto END; }
						  break;
					case 'D': if (ith==0) { ret=0x85; goto END;}
						  pnt = addr+FONT_INFO_POINT+FONT_INFO_SIZE*(ith-1);
						  pnt+= 0;
						  v8 = (INT8U)v32;
						  Flash_bwrite(pnt,v8);
						  break;
					case 'T': if (ith==0) { ret=0x85; goto END;}
						  pnt = addr+FONT_INFO_POINT+FONT_INFO_SIZE*(ith-1);
						  pnt+= 1;
						  v8 = (INT8U)v32;
						  Flash_bwrite(pnt,v8);
						  break;
					case 'A': if (ith==0) { ret=0x85; goto END;}
						  pnt = addr+FONT_INFO_POINT+FONT_INFO_SIZE*(ith-1);
						  pnt+= 2;
						  Flash_lwrite(pnt,v32);
						  break;

			     	}
				status=0;
			     } else {
				v32<<=4;
				v32|=ctoh(str[r]);
			     }
			     break;
		}
		r++;
	}
END:
	return ret;
}

INT8U commun_recv_key(INT16S key,INT16S key_id,INT8U *str,INT16S leng)
{
	INT32U p,kp;
	INT16S    l,sz;
	INT8U  pole,cur_pole;
	INT16U ncode,scode,i,sid;

	sz=2;
	l=KEY_MAX_TABLE_SIZE;
	pole = (INT8U)(key>>8);
	key&=0xff;
	cur_pole = get_global_bparam(GLOBAL_AREA_SCALETYPE);
	if (pole) {
		if (pole!=cur_pole) return 0x82;
	}
	switch (key) {
// CL5Keymanager.exe에서 Download시 사용
		case 1: p=FLASH_KEY_AREA;  break;
		case 2: p=FLASH_KEY_SHIFT_AREA; break;
       		case 3: p=FLASH_KEY_PGM_AREA; break;
		case 4: p=FLASH_KEY_PGM_SHIFT_AREA; break;
		case 5: p=FLASH_KEY_PGM2_AREA; break;
		case 6: p=FLASH_KEY_PGM2_SHIFT_AREA; break;
////////////////////////////////////

		case 7:	p=FLASH_KEY_CUSTOMER_AREA; break;
// CL5PS2Keymaneger.exe에서 Download시 사용
		case 41: p=FLASH_PSKEY_AREA;  break;
		case 42: p=FLASH_PSKEY_SHIFT_AREA; break;
       		case 43: p=FLASH_PSKEY_PGM_AREA; break;
		case 44: p=FLASH_PSKEY_PGM_SHIFT_AREA; break;
		case 45: p=FLASH_PSKEY_PGM2_AREA; break;
		case 46: p=FLASH_PSKEY_PGM2_SHIFT_AREA; break;
////////////////////////////////////

// CL-Works.exe에서 Download시 사용 : PLU Speed key layout
		case 10:
		case 11:p=FLASH_KEY_AREA;       l+=KEY_MAX_TABLE_SIZE; break;
////////////////////////////////////
		case 12:
		case 13:p=FLASH_KEY_PGM_AREA ;  l+=KEY_MAX_TABLE_SIZE; break;
		case 14:
		case 15:p=FLASH_KEY_PGM2_AREA;  l+=KEY_MAX_TABLE_SIZE; break;

		case 31:p=FLASH_KEY_CDTC_AREA;     l=64; break;			//CLERK		
		case 32:p=FLASH_KEY_CDTC_AREA+192; l=64; break;			//DEPT
		case 33:p=FLASH_KEY_CDTC_AREA+128; l=64; break;			//TARE
//		case 34:p=FLASH_KEY_CDTC_AREA+192; l=64; break;			//CURRENCY		delete by JJANG 20070502

// CL-Works.exe에서 Download시 사용 : 
		case 21:
		case 22:
		case 23:
		case 24:
		case 25:
			p=(key-21)*MAX_SPEEDKEY_SIZE;
			p+=FLASH_KEY_SPEED_AREA;
			sz=4;
			l =MAX_SPEEDKEY_SIZE;
			break;
////////////////////////////////////
		default:
			return 0x80;

	}
	if ((key>=40) && (key<50)) {
		l=160*2l;
	}
       	p+=DFLASH_BASE;
	if (key_id) {
		p +=(key_id-1)*sz;
		l=sz;
	}
	if ((key<30) || (key>=40)) {
		if (leng!=l) return 0x81;
	} else {
		if (leng>l) return 0x81;
		l=leng;
	}
	if ((key==10) || (key==12) || (key==14)) {
		for (i=0; i<128; i++) {
			memcpy(&ncode,&str[i*2],2);
			memcpy(&scode,&str[i*2+256],2);
			if (Startup.scale_type<=2) // banch type:1, pole type:2
				sid=key_pole_map[i];
			else if (Startup.scale_type == SCALE_TYPE_HANGING)
				sid=key_hanging_map[i];
			else if (Startup.scale_type == SCALE_TYPE_SELF)	   //SG060925
#ifdef _USE_UNDER_SELFKEY_
				sid=key_self_map[i];
#else
				sid=key_pole_map[i];
#endif
			else if (Startup.scale_type == SCALE_TYPE_DOUBLEBODY)
				sid=key_doublebody_map[i]; 
#if defined(USE_CL5200_KEY)
			else if (Startup.scale_type == SCALE_TYPE_BENCH_CL5200) // CL5200 B-type
				sid=key_CL5200_bench_map[i];
			else if (Startup.scale_type == SCALE_TYPE_HANGING_CL5200)
				sid=key_CL5200_hanging_map[i];
#endif                        
#if defined(USE_CL3000_KEY)
			else if (Startup.scale_type == SCALE_TYPE_BENCH_CL3000) // CL3000 B-type
				sid = key_pole_map[i];
#endif
			else
				sid = key_pole_map[i];
				
				
			if (sid==0xffff) {
				continue;
			}
			if (ncode==0xffff) {
				ncode=0;
				scode=0;
				if (Startup.scale_type==1){ 
      					if (sid<KS_PLU_25) continue;
					sid-=KS_PLU_25;
					if(i==0x7f) sid+=(KS_PLU_25-KS_PLU_01); // Prevent KEY_IDENTIFY corrupted
					ncode=sid + KS_PLU_01;
					scode=sid + KS_PLU_49;
				} 
				else if (Startup.scale_type==4)	 //self service
				{
					sid-=KS_PLU_01;
					ncode=sid + KS_PLU_01;
					scode=sid + KS_PLU_01;
				}			
				else if (Startup.scale_type==5)	 //D type 
				{
					sid-=KS_PLU_01;
					ncode=sid + KS_PLU_01;
					if(sid < 73)
						scode=sid + KS_PLU_73;
				}
#if defined(USE_CL5200_KEY)
                else if (Startup.scale_type==6)
                { 
					sid-=KS_PLU_01;
					ncode=sid + KS_PLU_01;
					scode=sid + KS_PLU_55;
				} 
#endif                                 
				else 
				{
					sid-=KS_PLU_01;
					ncode=sid + KS_PLU_01;
					scode=sid + KS_PLU_73;
				}
			}
			if (ncode) {
				kp=i*2;
				kp+=p;
				Flash_wwrite(kp,ncode);
			}
			if ((scode==0) || (scode==0xffff)) scode=ncode;
			if (scode) {
				kp=i*2+256;
				kp+=p;
				Flash_wwrite(kp,scode);
			}
		}
	} else {
     		Flash_swrite(p,(HUGEDATA INT8U*)str,l);
		if (key >= 1 && key <=6)
		{
			KEY_CnvTable(0, cur_pole, (0x01<<(key-1)));	// P-type => cur_pole
		}
	}
	commun_recv_monitor('K',(INT16S)key_id,(INT16S)key);
	return 1;
}

INT8U commun_recv_caption(HUGEDATA COMM_BUF *CBuf,INT16S mode,INT16S id,INT8U *str)
{
	INT16S r,status;
	INT8U code,v8;
	INT32U v32,address;
	INT16U v16;
	CAPTION_STRUCT	cap;

	memset((INT8U *)&cap,0,sizeof(CAPTION_STRUCT));
	if (mode==0) return 0x80;
	if (mode>2 ) return 0x80;
	if (id==0  ) return 0x80;
	if (mode==2) { // By ID
		address=caption_search_by_code(id);
	} else {
#ifdef USE_EXTEND_CAPNTION_AREA
		if (id-1 >= MID_CAPTION_NO) {
			address =(INT32U)(id-MID_CAPTION_NO-1);
			address*=CAP_MSG_LENGTH;
			address+=DFLASH_BASE;
			address+=FLASH_CAPTION_EXT_AREA;
		}
		else
		{
			address =(INT32U)(id-1);
			address*=CAP_MSG_LENGTH;
			address+=DFLASH_BASE;
			address+=FLASH_CAPTION_AREA;
		}
#else	
		address =(INT32U)(id-1);
		address*=CAP_MSG_LENGTH;
		address+=DFLASH_BASE;
		address+=FLASH_CAPTION_AREA;
#endif
	}
	Flash_sread(address,(HUGEDATA INT8U *)&cap,sizeof(CAPTION_STRUCT) );
	status=0;
	r=0;
	while (1) {
		switch (status) {
			case 0:	code=str[r];
				if (code==0) goto END;
				v32=0; v16=0; v8=0;
				status=1;
				break;
			case 1: if (str[r]==':') {
				   switch (code) {
				   	case 'C': status= 10; break;
					case 'n': status= 20; break;
					case 'X': status= 30; break;
					case 'S': status= 40; break;
					case 'N': status= 60; break;
					case 'I': status= 70; break;
					case 'm': status= 80; break;
					case 'L': status= 90; break;
					case 'D': status=100; break;

					case 'A': status=110; break;
					case 'a': status=120; break;
					case 'b': status=130; break;
					default : goto END;
				   }
				}
				break;
			case 10:v16 =ctoh(str[r]); status=11; break;
			case 11:v16<<=4;
				v16|=ctoh(str[r]); status=12; break;
			case 12:
				memset((INT8U *)cap.form,0,sizeof(cap.form));
				memcpy((INT8U *)cap.form,&str[r],v16);
				r+=v16;
				status=0;
				goto SKIP;
			case 110:
			case 120:
			case 130:if (str[r]=='.') {
					switch (status) {
						case 110: v16=v32>>16;
							  cap.reserved=v16;
							  v16=v32&0xffff;
							  cap.runflag=v16;
							  break;
						case 120: cap.input_min=v32; break;
						case 130: cap.input_max=v32; break;
					}
					status=0;
					break;
				}
				v32<<=4;
				v32|=ctoh(str[r]);
				break;
			case 20:
			case 60:
			case 70:
			case 80:
			case 90:
			case 100:if (str[r]=='.') {
					switch (status) {
						case  20: cap.submenu_n=v16; break;
						case  60: cap.code     =v16; break;
						case  70: cap.inputtype=v16; break;
						case  80: cap.submenu_id=v16; break;
						case  90: cap.input_length=v16; break;
						case 100: cap.input_dlength=v16; break;
					}					
					status=0;
					break;
				}
				v16<<=4;
				v16|=ctoh(str[r]);
				break;
			case 30:
			case 40:if (str[r]=='.') {
					switch (status) {
						case 30: cap.input_x=v8;     break;
						case 40: cap.input_itype=v8; break;
					}
					status=0;
					break;
				}
				v8<<=4;
				v8|=ctoh(str[r]);
				break;
		}
		r++;
SKIP:;
	}
END:
	Flash_swrite(address,(HUGEDATA INT8U *)&cap,sizeof(CAPTION_STRUCT) );
	return 1;
}

INT8U commun_recv_labelbmp(INT32U first,INT16U second,INT8U *str,INT16S leng)
{
	INT32U addr,src_addr,subaddr;
	INT16U v16,save_leng,recv_w,recv_h,recv_sz,recv_id;
	INT8U  cc,ret,recv_form,recv_set,labelname[20];
	INT16S    status,recv_nth,r;
	INT16U nth_addr;

	status      = 0;
	recv_id = (INT16U)first;
	recv_form = second>>8;
	recv_set  = second&0xff;

	r = 0;
	ret=1;
	recv_nth = 0;
	recv_w   =recv_h = 0;
	src_addr = 0;
	switch (recv_form) {
		case 1: break;
		case 2: src_addr=FLASH_BITMAP_AREA; break;
		default: return 0x99;
	}
	src_addr += DFLASH_BASE;
 	if ((recv_set<1) || (recv_set>2)) return 0x99;
	if (recv_id<=0) return 0x99;

	nth_addr=999;
	switch (recv_form) {
		case 1: nth_addr= get_new_labeladdress( recv_id,0); break;
		case 2: nth_addr= get_new_bitmapaddress(recv_id,0); break;
	}
	if (nth_addr==999) return 0x98;

	memset(labelname,0,16);

	while (r<leng) {
		switch (status) {
			case 0: cc=str[r];
				if (cc==0) goto END;
				status=1;
				v16=0;
				if (cc=='D') status=10;
				break;
			case 1: status=0;
				if (str[r]=='=') {
					status=2;
					if (cc=='Z') status=21;
				}
				break;
			case 2: if (str[r]=='.') {
					switch (cc) {
						case 'N': recv_nth =(INT16S)v16;    break;
						case 'S': recv_sz  =(INT16U)v16; break;
						case 'W': recv_w   =(INT16U)v16; break;
						case 'H': recv_h   =(INT16U)v16; break;
						default : break;
					}
					status=0;
				} else {
					v16<<=4;
					v16|=ctoh(str[r]);
				}
				break;
			case 21:if (str[r]==0x0d) {
					labelname[15]=0;
					status=0;
				} else {
					if (v16>=15) v16=15;
					labelname[v16++]=str[r];
				}
				break;
			case 10:if (str[r]==':') {
					save_leng=(INT16U)v16;
					status=11;
				} else {
					v16<<=4;
					v16|=ctoh(str[r]);
				}
				break;
			case 11:
				if (recv_nth<1) return 0x97;
				if (save_leng>512) return 0x96;
				subaddr =(recv_nth-1);
				subaddr*=512l;
				switch (recv_form) {
				    case 1:
						/*
						 * 라벨 이미지 시작 주소에서 인덱스만큼 떨어진 곳에 라벨 이미지 저장하기 위한 주소 계산
						 */
						addr = get_addr_from_label_idx(nth_addr, LABEL_AREA_IMAGE);
					    break;
				    case 2:
						addr  = (INT32U)nth_addr;
						addr *= BITMAP_MAX_SIZE;
						addr += src_addr + BITMAP_IMAGE;
					    break;
				    default: return 0x97;
				}
				addr+= subaddr;
				Flash_swrite(addr,(HUGEDATA INT8U *)&str[r],save_leng);
				ret = (INT8U)(recv_nth)+0x10;
				status=0;
				r+=save_leng;
				goto R_SKIP;
		}
		r++;
	R_SKIP:;
	}
	if (recv_set==1) {
		addr  = src_addr;
		struct LABEL_HEADER_INFO info;
		memset(&info, 0, sizeof(struct LABEL_HEADER_INFO));
		info.label_id = recv_id;
		info.img_w = recv_w;
		info.img_h = recv_h;
		strncpy(info.img_name, labelname, LABEL_HEADER_IMG_NAME_SIZE);
		info.img_size = recv_sz;

		switch (recv_form) {
		case 1:
		 	/*
			 * 라벨 헤더 저장하기 위해
			 */
			addr = get_addr_from_label_idx(nth_addr, LABEL_AREA_HEADER);
			save_label_header(addr, &info, L_H_ALL_BIT);
			ret=0x43;
			break;
		case 2:
			addr += nth_addr*BITMAP_INFO_SIZE;
			Flash_wwrite(addr,recv_id);
			addr+=2;
			Flash_wwrite(addr,recv_w);
			addr+=2;
			Flash_wwrite(addr,recv_h);
			ret=0x44;
			break;
	   }
	}
END:
	return ret;
}

INT8U commun_recv_plu_part(HUGEDATA COMM_BUF *CBuf,INT32U ipluno,INT16U ideptno,INT8U *str,INT16S leng)
{
	INT16S    ret,r,status;
//	INT16S xmode;
	INT32U pluno,v32;
	INT8U  deptno,code;
	INT16U ptype,dleng,key;
	INT8U  stype;

	ETH_CONNECT_LIST eth;
	memset((INT8U *)&eth,0,sizeof(ETH_CONNECT_LIST));
	deptno = (INT8U)ideptno;
	pluno  = (INT32U)ipluno;
	if (pluno <=0) return 0x82;
	if (deptno<=0) return 0x83;
	if (leng <0  ) return 0x84;
	plu_table_search(deptno,pluno,&key,0);
	if (key==0) return 0x85;
	ret    = 1;
	status = 0;
	r      = 0;
//	xmode  = 0;
	while (1) {
		switch (status) {
			case  0:dleng=0;
				ptype=0;
				stype=0;
				if (r>=leng) goto END2;
				code=str[r];
				if (code=='F') status=1;
				else if (code) {
					status=20;
				} else goto END2;
				break;
			case  1:ptype=0;
				if (str[r]=='=') status=2;
				else status=0;
				break;
			case  2:if (str[r]=='.') {
					dleng  =0;
					status=3;
					break;
				}
				ptype<<=4;
				ptype|=ctoh(str[r]);
				break;
			case  3:if (str[r]==',') {
					dleng  =0;
					status=4;
					break;
				}
				stype<<=4;
				stype|=ctoh(str[r]);
				break;
			case  4:if (str[r]==':') {
					status=10;
					break;
				}
				dleng<<=4;
				dleng|=ctoh(str[r]);
				break;
			case 10:plu_set_field(key-1, ptype,(INT8U *)&str[r] );
				r+=dleng;
				status=0;
				goto SKIP;
			case 20:status=0;
				v32   =0;
				if (str[r]=='=') status=21;
				break;
			case 21:if (str[r]=='.') {
//					if (code=='#') {
//						xmode=(INT16S)v32;
//					} else {
						commun_common_ethernet(code,&eth,v32);
//					}
					status=0;
				} else {
					v32<<=4;
					v32|=ctoh(str[r]);
				}
				break;
		}
		r++;
		SKIP:;
	}
END2:
	if (network_status.service_flag&0x20)	// send plu
	{
		if (ethernet_list.status == 1)
		{
			commun_net_mask(0, (INT16U *)&key, 0xff, 1);
		}
	}
	return ret;
}

// param0 is flag
// param1 is size
INT16U commun_recv_block_addr(INT16S param0,INT16S param1,INT8U *str,INT16S leng)
{

	ETH_CONNECT_LIST eth;
	INT32U addr;
	INT8U  status;
	INT8U  code;
	INT16U size;
	INT16S    r;
	char   temp[32];
	r      = 0;
	status = 0;
	addr   =0l;
	size   = param1;
	recv_counter=0;
	memset((INT8U *)&eth,0,sizeof(ETH_CONNECT_LIST));

	while (1) {
		switch (status) {
			case  0: status = 0;
				 addr=0l;
				 if (r>=leng) break;
				 code = str[r];
				 if(code==0) goto END;
				 status =1;
				 //if (code=='A') { addr=0l; status=1; }
				 break;
			case  1: status = 0;
				 if (str[r]=='=') status=2;
				 break;
			case  2: 
				if (str[r]=='.') 
				{
					switch(code)
					{
						case 'A': 
							  status =3;
							  break;
						default : 
							  commun_common_ethernet(code,&eth,addr);
		  					  status=0;
							  break;

					}
				}
				else
				{
					addr<<=4;
					addr|=ctoh(str[r]);
				}
				break;
			case  3:if (addr!=0l) {
					if (size) {
						Flash_swrite(addr,(HUGEDATA INT8U *)&str[r],size);
						goto END;
					}
					break;
				} else {
					goto END;
				}
		}
		r++;
	}
END:
	global_one2one_send=param0;
	if (status_run.run_mode!=RUN_BACKUP) {
		status_run.run_mode=RUN_BACKUP;
		global_one2one_count=0;
	} else {
		switch (param0) {
		    case  9: sprintf(temp,"RESET"); break;
		    case  3: sprintf(temp,"P_SET"); break;
		    default: sprintf(temp,"%05d",global_one2one_count);
		}
		display_string(DISPLAY_UNITPRICE,(INT8U *)temp);
		VFD7_Diffuse();
		global_one2one_count++;
	}
	if (param0==3) plu_ptype_reset();
	if (param0==9) {
		_SOFTWARE_RESET;
	}
	return param0;
}

INT16U commun_recv_block(HUGEDATA COMM_BUF *CBuf,INT16S ipluno,INT16S ideptno,INT8U *str,INT16S leng)
{
	INT16S    r,status,com_type;
	INT32U data_size;
	INT16S    datablock;
	INT32U addr,addr_org,max_size,v32,xaddr;
	INT8U  r_status,datatype,code;
	INT16U ret;
	ETH_CONNECT_LIST eth;
	char   temp[32];
	INT8U  fwversion[3],version[2];
	INT16U country, keyCompareValue;
	INT8U  scaleType;

	memset((INT8U *)&eth,0,sizeof(ETH_CONNECT_LIST));
	datatype   = (INT8U)ideptno;
	datablock  = ipluno;
	memset(fwversion,0,3);
	memset(version  ,0,2);
	country=0xffff;
	if (datablock<=0) return 0x82;
	com_type=serial_get_comtype(CBuf);
	status   = 0;
	r        = 0;
	r_status = 0;
	datablock--;

// 스리랑카 전용 펌웨어 데이터롬 업데이트시 CAL 스위치 체크하도록
#ifdef	USE_CHECK_CAL_SW_TO_UPDATE_DATAROM
	/* CAL SW / Symbol(0x14) / Caption(0x15) / Origin(0x08) / Keypad(0x04) / Font(0x0C) */
	if(CAL)
	{
		if((datatype == 0x14) || (datatype == 0x15)|| (datatype == 0x08) || (datatype == 0x04) || (datatype == 0x0C))
		{
			ret = 0;
			DisplayMsg("Check CAL Mode");
			BuzOn(2);
			return ret;
		}
	}
#endif	// 	USE_CHECK_CAL_SW_TO_UPDATE_DATAROM
	while (1) {
		switch (status) {
			case  0:data_size=0;
				r_status =0;
				code=str[r];
				v32  =0;
				if (r>=leng) goto END2;
				if (code=='D') status=1;
				else status=20;
				break;
			case  1:if (str[r]=='=') status=2;
				else status=0;
				break;
			case  2:if (str[r]=='.') {
					status=3;
					break;
				}
				r_status<<=4;
				r_status|=ctoh(str[r]);
				break;
			case  3:if (str[r]==':') {
					status=10;
					break;
				}
				data_size<<=4;
				data_size|=ctoh(str[r]);
				break;
			case 10:	
				addr_org =(INT32U)commun_get_block_type_address(datatype,&max_size);
				addr =(INT32U)datablock;
				addr*=(INT32U)COM_DATABLOCK_SIZE;
				xaddr = addr + data_size;
				if (xaddr<=max_size ) {
					addr+=addr_org;
					sprintf(temp,"%02d:%05lX",datatype,addr&0x0fffffl);
					PutString(DSP_MSG_R_Y, DSP_MSG_R_X, temp, DSP_MSG_R_FONT_ID, DSP_MSG_R_MAGY, DSP_MSG_R_MAGX, MAX_MSG_R_CHAR);
					Flash_swrite(addr,(HUGEDATA INT8U *)&str[r],data_size);
#ifdef USE_EXTEND_CAPNTION_AREA
					if (datatype == 21 && datablock == 0) // Caption & 첫번째 block일 경우 Extended Caption 영역 삭제
					{
						addr_org =(INT32U)commun_get_block_type_address(24, &max_size);
						Flash_clear(addr_org, 0x00, max_size);
					}
#endif
				}
				status=0;
				goto END2;
			case 20:status=0;
				v32   =0;
				if (str[r]=='=') status=21;
				break;
			case 21:if (str[r]=='.') {
					switch (code) {
					   case 'V': // FW Version
						    fwversion[0]=(INT8U)(v32>>16);
						    fwversion[1]=(INT8U)(v32>>8);
						    fwversion[2]=(INT8U)(v32);
						    break;
					   case 'W': version[0]=(INT8U)(v32>>8);
						    version[1]=(INT8U)(v32);
						    break;
					   case 'C': country=(INT16U)v32;
						    break;
					   default : commun_common_ethernet(code,&eth,v32);
						    break;
					}
					status=0;
				} else {
					v32<<=4;
					v32|=ctoh(str[r]);
				}
				break;
		}
		r++;
	}
END2:
	if (r_status==1) 
	{		
	    sprintf(temp,"End [%02d]",datatype);
	    PutString(DSP_MSG_R_Y, DSP_MSG_R_X, temp, DSP_MSG_R_FONT_ID, DSP_MSG_R_MAGY, DSP_MSG_R_MAGX, MAX_MSG_R_CHAR);
	    if (com_type) {
	    } 
	    else 
	    {
		if ((datatype==1) || (datatype==6)|| (datatype==3) || (datatype==5)) 
		{
		    // 1=All Global Parameter,3=Dealer Paramerer,5=Caption,6=Basic Parameter
		    if ((datatype==1) || (datatype==6) || (datatype==3)) parameter_backup();
		    if (country!=0xffff) 
		    {
			ret=get_global_wparam(GLOBAL_AREA_VENDOR_COUNTRY);
			set_global_wparam(GLOBAL_AREA_VENDOR_COUNTRY,country);
			if (ret!=country) 
			{
				KEY_CopyKeyTable(0xffff);
			}
			Startup.country = country;
		    }
		}
		else if (datatype==4)  // Keypad
		{
			keyCompareValue = Flash_wread(addr_org);
			keyCompareValue = (keyCompareValue&0x0f00)>>8;
			scaleType = get_global_bparam(GLOBAL_AREA_SCALETYPE);
			if(scaleType != keyCompareValue)
			{
			#ifdef _USE_UNDER_SELFKEY
				if(scaleType != 4) KEY_CnvTable(0, scaleType, 0x3f);
			#else
				KEY_CnvTable(0, scaleType, 0x3f);
			#endif
			}
		}
		else if (datatype==12) // Font
		{		
		    Dsp_default_fontload();
		}
#ifdef USE_GSMART_PLU
		else if (datatype == 21)// total caption
		{
			pcsunit_default_KR();	// Setting ROM의 symbol size(18 & 6)가 다르므로 별도 수행
		}
#endif
	    }
	} 
	else 
	{
	    r_status=0;
	    if (com_type) {
		datablock++;
	    }
	}
	ret=(INT16U)datablock+1;
	if (r_status) {
		ret  =0;
	}
	return ret;
}

INT16U commun_recv_parameter(HUGEDATA COMM_BUF *CBuf,INT32U fcode,INT32U nn,INT8U *str,INT32U leng)
{
	INT16S    r,status;
	INT32U r_size,data_size;
	INT32U startAddr,v32;
	//INT8U  r_status;
	INT8U  code;
	//INT8U  bitFlag,startBit,countBit;
	INT16U ret;
	ETH_CONNECT_LIST eth;
	INT8U  tempData[64];
	PARAMETER_SETTING_TYPE pset;

	memset((INT8U *)&eth,0,sizeof(ETH_CONNECT_LIST));
	status   = 0;
	r        = 0;
//	r_status = 0;
	r_size   = 0;
	data_size =0;
	ret =0;
	while (1) {
		switch (status) {
			case  0:
				r_size = 0;
//				r_status =0;
				code=str[r];
				v32  =0;
				if (r>=leng) goto END2;
				if (code=='D') status=1;
				else goto END2; //status=20;
				break;
			case  1:if (str[r]=='=') status=2;
				else status=0;
				break;
			case  2:if (str[r]=='.') {
					status=3;
					break;
				}
				r_size<<=4;
				r_size|=ctoh(str[r]);
				break;
			case  3:
				if(r_size>0) {
					memcpy(tempData,&str[r],r_size);
					tempData[r_size]=0;
				}
				data_size = r_size;
				status=0;
				r+=r_size;
				continue;
			case 20:status=0;
				v32   =0;
				if (str[r]=='=') status=21;
				break;
			case 21:if (str[r]=='.') {
					switch (code) {
//					   case 'F': // bit flag
//						    bitFlag = (INT8U)(v32);
//						    break;
//					   case 'S': // start bit 
//					   	    startBit =(INT8U)(v32);
//						    break;
//					   case 'L': // count bit 
//					   	    countBit =(INT8U)(v32);
//						    break;
					   default : commun_common_ethernet(code,&eth,v32);
						    break;
					}
					status=0;
				} else {
					v32<<=4;
					v32|=ctoh(str[r]);
				}
				break;
		}
		r++;
	}
END2:
	if( SearchParamSet(fcode, &pset) ){
#ifdef USE_POLAND_DEFAULT
		if (fcode > 800 && fcode < 1000 && fcode != 995)//801~999 check cal key
#else
		if (fcode > 800 &&  fcode < 1000)//801~999 check cal key
#endif
		{
			if(!CAL)return ret = 2;
		}
		if (pset.addrMemory)
		{
			startAddr = DFLASH_BASE + pset.addrMemory + pset.offsetMemory; 
			Flash_write_data(startAddr,pset.sizeMemory,(HUGEDATA INT8U *)tempData,(INT8U)data_size,pset.bitFlag,pset.startBit,pset.countBit,fcode);
			if (nn == 0x99) // use action
			{
				if (fcode == 994)
				{
					INT16U prev_timer;
					BuzOn(2);
					commun_write_err16(CBuf, 0x85, 1, 2, OFF);
					prev_timer = SysTimer100ms;
					while(CheckTxBuf(CBuf))
					{
						if ((INT16U)(SysTimer100ms - prev_timer) > 30) break;
						network_tx_proc();
					}
					BuzOnAdd(1);
				}
				ParameterAction(pset.funcCode, 0x11);
			}
		}		
        ret=1;
	}
	return ret;
}

INT8U commun_recv_pluinhibit(HUGEDATA COMM_BUF *CBuf,INT32U spart,INT16U sid,INT8U *str,INT16S leng)
{
	INT16S    ret,r,status;
	INT16S    part;//,erase;
	INT16U id,ptype;
	ETH_CONNECT_LIST eth;
	INT8U  code,pset;
	INT32U dsize,v32;

	memset((INT8U *)&eth,0,sizeof(ETH_CONNECT_LIST));

	part = (INT16S)spart;
	id   = (INT16S)sid;
	if (part<=0) return 0x99;
	if (id<=0) return 0x99;
	if (leng<3) return 0x97;
	ret    = 1;
	status = 0;
	r      = 0;
	ptype  = 0;
	pset   = 0;
	if (id!=1) return 0x97;
	while (1) {
		if (r>=leng) break;
		switch (status) {
			case  0:code  =str[r];
				if (!code) goto END;
				if (!(((code>='a') && (code<='z')) || ((code>='A') && (code<='Z')))) status=20;
				else status=1;
				break;
			case  1:
				dsize = 0;
				if (str[r]=='=') status=2;
				else status=0;
				break;
			case  2:if (str[r]=='.') {
					switch (code) {
						case 'P': ptype=(INT16U)dsize; break;
						case 'S': pset =(INT8U)dsize; break;
					}
					status=0;
				} else {
					dsize<<=4;
					dsize|=ctoh(str[r]);
				}
				break;
			case 20:status=0;
				v32   =0;
				if (str[r]=='=') status=21;
				break;
			case 21:if (str[r]=='.') {
					commun_common_ethernet(code,&eth,v32);
					status=0;
				} else {
					v32<<=4;
					v32|=ctoh(str[r]);
				}
				break;
		}
		r++;
	}
	if (id==1) {
		if ((ptype<=0) || (ptype>=128)) return 0x97;
		if (pset>=2) return 0x97;
	}
	ret=1;
	switch (id) {
	   case 1: plu_inhibit_bycode(ptype,pset);
		   if (ptype==0x80) {
			if (pset==1) set_global_bparam(GLOBAL_LABEL_PRIORITY,1);
		   }
		   if (ptype==0x85) {
			if (pset==1) set_global_bparam(GLOBAL_BARCODE_PRIORITY,1);
		   }
		   if (ptype==0x08) {
			if (pset==1) set_global_bparam(GLOBAL_TAX_FLAG,1);
		   }
	   	   break;
	   default: ret=0x97;
	}
END:
	return ret;
}

extern void SetScaleType(INT8U scaleType, INT8U display, INT8U alwaysChange);
// W89A<data_typeid>,<data_type>L<data size>
INT8U commun_recv_set(HUGEDATA COMM_BUF *CBuf,INT16S nth_block,INT16S data_type,INT8U *str,INT16S leng)
{
	INT16S    r,status;
	INT32U v32;
	INT8U  code,ret;
	ETH_CONNECT_LIST eth;
	char   temp[32];
	INT8U  fwversion[2],version[2];
//	INT16U country;
	INT8U buffer[HISTORY_BUF_SIZE];
	INT8U scaleType;
	INT8U cmd;
#ifdef USE_CHN_CERTIFICATION
	INT16U fw_checksum;
	INT32U addr;
#endif
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
	POINT  disp_p;
#endif
#ifdef USE_REMOTE_FUNCTION
    INT8U ip[4];
    INT8U gateway[4];
    INT8U subnetmask[4];
    INT16U port[2];
    INT16U temp16u;    
    INT8U ssid[34];
    INT8U wepVal[65];
    INT8U temp8uArr[2]; //[0]:wepId [1]:securType  
    INT8U global_com;
#endif    

	memset((INT8U *)&eth,0,sizeof(ETH_CONNECT_LIST));
	memset(fwversion,0,2);
	memset(version  ,0,2);
//	country=0xffff;
	status   = 0;
	r        = 0;
	if(data_type==5)  goto END2;
	while (1) {
		switch (status) {
			case  0:
				code=str[r];
				v32  =0;
				if (r>=leng) goto END2;
				status=20;
				break;
			case 20:status=0;
				v32   =0;
				if (str[r]=='=') status=21;
				break;
			case 21:if (str[r]=='.') {
					switch (code) {
					   case 'V': // FW Version
						    fwversion[0]=(INT8U)(v32>>8);
						    fwversion[1]=(INT8U)(v32);
						    break;
					   case 'W':version[0]=(INT8U)(v32>>8);
						    version[1]=(INT8U)(v32);
						    break;
//					   case 'C': country=(INT16U)v32;
//						    break;
					   case 'T': scaleType=(INT8U)v32;
#if defined(USE_CL5200_KEY)
							if(scaleType == SCALE_TYPE_BENCH)	//Firmware update 프로그램에서 bench로 보냈을 경우 6으로 저장. 
							{
								scaleType = SCALE_TYPE_BENCH_CL5200;
							}
							else if(scaleType == SCALE_TYPE_HANGING)
							{
								scaleType = SCALE_TYPE_HANGING_CL5200;
							}
#elif defined(USE_CL3000_KEY)
							if(scaleType == SCALE_TYPE_BENCH)	//Firmware update 프로그램에서 bench로 보냈을 경우 7로 저장.
							{
								scaleType = SCALE_TYPE_BENCH_CL3000;
							}
#endif
						    break;
					   default : commun_common_ethernet(code,&eth,v32);
						    break;
					}
					status=0;
				} else {
					v32<<=4;
					v32|=ctoh(str[r]);
				}
				break;
		}
		r++;
	}
END2:
	ret=0;
	nth_block--;
	switch (data_type) {
		case 5: //memory clear command	// "W89A0064,05L0005:C=03.S"
			if(nth_block==99){
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
	#ifdef USE_CHN_FONT
				VertScrollMsg_Init(DSP_SYSTEM_FONT_ID);
	#else
				VertScrollMsg_Init(DSP_MENU_FONT_ID);
	#endif
				VertScrollMsg_Add("MEMORY CLEAR...");
#else
				VertScrollMsg_Init(DSP_PLU_FONT_ID);
				VertScrollMsg_Add("MEM CLEAR...");
#endif
				cmd = 0x88;
				commun_write_err16(CBuf,(INT8U)cmd,1,2,OFF);
				history_flash2ram(buffer);
				
#ifdef USE_INIT_MASK
				init_mask(INIT_MASK_BACKUP);				
#endif

#ifdef USE_CHN_CERTIFICATION
				addr = DFLASH_BASE;
				addr += FLASH_FW_CHECKSUM;
				fw_checksum = Flash_wread(addr);
#endif
				Flash_init_nvbuffer();
#ifdef USE_REMOTE_FUNCTION                 
                if (network_status.remote_control == ON)
                {
                    get_network_setting_sparam(ip, gateway, subnetmask, &port[0]);
					temp16u = get_net_wparam(NETWORK_SERVICE_FLAG);
                    global_com = get_global_bparam(GLOBAL_AREA_COMMUNICATION);//wlan flag
                    if ((global_com&0x03) == 0x03)
                    {
                        get_wifi_setting_sparam(ssid, wepVal, temp8uArr);
                    }
                }
#endif                
				if(FlashDChipErase() == FLASH_SUCCESS) {
					ret=1;
					VertScrollMsg_Add("Done");
					BuzOn(1);
				}
				else {
					ret=0;
					VertScrollMsg_Add("Fail");
					BuzOn(2);
				}
				history_ram2flash(buffer);
#ifdef USE_CHN_CERTIFICATION
				Flash_wwrite(addr,fw_checksum);
#endif
#ifdef USE_REMOTE_FUNCTION 
                if (network_status.remote_control == ON)
                {
                    set_network_setting_sparam(ip, gateway, subnetmask, port[0]);
                    if ((global_com&0x03) == 0x03)
                    {
                        set_wifi_setting_sparam(ssid, wepVal, temp8uArr[0], temp8uArr[1]);
                        set_global_bparam(GLOBAL_AREA_COMMUNICATION ,global_com); 
                    }
                    //enable use remote control
                    set_net_wparam(NETWORK_SERVICE_FLAG, temp16u);                
                }
#endif                
#ifdef USE_INIT_MASK 
				init_mask(INIT_MASK_RESTORE);				
#endif
				Flash_flush_nvbuffer();
				VertScrollMsg_Add("Reboot please");
				Delay_100ms(100); 
			}
			break;
		case 6:	// "W89A000x,06L0005:....." 
		case 1: 
#ifdef	USE_CHECK_CAL_SW_TO_UPDATE_DATAROM
			/* Parameter Block */
			if(CAL)
			{
				ret = 0;
				break;
			}
#endif	// USE_CHECK_CAL_SW_TO_UPDATE_DATAROM
#ifdef USE_INIT_MASK
			init_mask(INIT_MASK_BACKUP);				
#endif
			set_global_wparam(GLOBAL_AREA_VENDOR_COUNTRY, (INT16U)nth_block);
			global_set_country((INT16U)nth_block, OFF);
#ifdef USE_INIT_MASK 
			init_mask(INIT_MASK_RESTORE);				
#endif
			break; // Parameter
		case 4: KEY_CopyKeyTable((INT16U)nth_block); break;   // Keypad
		case 9:	// "W89A0001,09L0005:....." 
#ifdef	USE_CHECK_CAL_SW_TO_UPDATE_DATAROM
			/* Restart Block */
			if(CAL)
			{
				ret = 0;
				break;
			}
#endif	// USE_CHECK_CAL_SW_TO_UPDATE_DATAROM
				ret = 1;
				break;
			//_SOFTWARE_RESET;
		case 2:	// "W89A0001,02L0005:T=02.<bcc>" 
#ifdef	USE_CHECK_CAL_SW_TO_UPDATE_DATAROM
			/* Scale Type Block */
			if(CAL)
			{
				ret = 0;
				break;
			}
#endif	// USE_CHECK_CAL_SW_TO_UPDATE_DATAROM
			/* m1731은 1810 등 엮여 있는 부분들이 있어 백업 안하는 것으로 결정 */
			SetScaleType(scaleType, OFF, ON);
			ret = 1;
			break;
	}
#ifdef	USE_CHECK_CAL_SW_TO_UPDATE_DATAROM
	/* W89****시 CAL 스위치를 체크 */
	if(CAL)
	{
		DisplayMsg("Check CAL Mode");
		BuzOn(1);
		return ret;
	}
#endif	// USE_CHECK_CAL_SW_TO_UPDATE_DATAROM
	if(data_type != 5){
		sprintf(temp,"Set [%2d]",(INT16U)nth_block);
		PutString(DSP_MSG_R_Y, DSP_MSG_R_X, temp, DSP_MSG_R_FONT_ID, DSP_MSG_R_MAGY, DSP_MSG_R_MAGX, MAX_MSG_R_CHAR);
	}

	return ret;
}

INT8U commun_recv_table1(HUGEDATA COMM_BUF *CBuf,INT32U spart,INT16U sid,INT8U *str,INT16S leng)
/**********************************************************************
 Department,Group,Label Message,Origin, PCS
 **********************************************************************/
{
	INT16S    l,r,status,i;
	INT16S    part;
	INT16U id;
	ETH_CONNECT_LIST eth;
	char   temp[128];
	char   desc1[MAX_TABLE_BUF],desc2[MAX_TABLE_BUF];
	INT8U  xtype,formno,code,error;
	INT16U skey,sz;
	INT32U rvalue,dsize,v32,p;
#ifdef USE_TAX_RATE    
	TAX_STRUCT  	tax;
#endif    
	TARE_STRUCT	tare;
	BARCODE_STRUCT  barcode;
#ifdef USE_TRACE_MEATGRADE
	INT16U order;
#endif
#ifdef USE_FIXED_PERCENT_KEY_SET 
	DISCOUNT_KEY_STRUCT discKey;
#endif

	memset((INT8U *)&eth,0,sizeof(ETH_CONNECT_LIST));
	part = (INT16S)spart;
	id   = (INT16S)sid;
	if (part<=0) return 0x99;
	if (id  <=0) return 0x99;
	if (leng<5 ) return 0x97;

	status = 0;
	r      = 0;
	dsize  = 0;
	xtype  = 0;
	formno = 0;
	code   = 0;
	skey   = 1;
	error  = 0;
	memset(desc1,0,MAX_TABLE_BUF);
	memset(desc2,0,MAX_TABLE_BUF);

	while (1) {
		if (r>=leng) break;
		switch (status) {
			case  0:code  =str[r];
				if (!code) goto END;
				if (!(((code>='a') && (code<='z')) || ((code>='A') && (code<='Z')))) status=20;
				else status=1;
				break;
			case  1:
				dsize = 0;
				if (str[r]=='=') status=2;
				else status=0;
				break;
			case  2:if (str[r]=='.') {
					if ((code=='D') || (code=='F')) {
						status=10;
					} else {
						switch (code) {
						    case 'K': skey=(INT16U)dsize;  break;
						    case 'T': xtype=(INT8U)dsize;  break;
						    case 'N': formno=(INT8U)dsize; break;
						    case 'B': error =(INT8U)dsize; break;
						    case 'R': 
						    case 'V': rvalue=(INT32U)dsize; break;
					#ifdef USE_TRACE_MEATGRADE
						    case 'G': order = (INT16U)dsize; break;
					#endif
						} 
						status=0;
					}
					break;
				} else {
					dsize<<=4;
					dsize|=ctoh(str[r]);
				}
				break;
			case 10:
				l=(INT16S)dsize;
				if (l > MAX_TABLE_BUF) l = MAX_TABLE_BUF;
				if (code=='D') {
					for (i=0; i<l; i++) desc1[i]=str[r++];
					desc1[l]=0;
				} else {
					for (i=0; i<l; i++) desc2[i]=str[r++];
					desc2[l]=0;
				}
				r += (dsize - l);
				status = 0;
				goto SKIP;
			case 20:status=0;
				v32   =0;
				if (str[r]=='=') status=21;
				break;
			case 21:if (str[r]=='.') {
					commun_common_ethernet(code,&eth,v32);
					status=0;
				} else {
					v32<<=4;
					v32|=ctoh(str[r]);
				}
				break;
		}
		r++;
		SKIP:;
	}
END:	if (error) {
		return 0x98;
	};
	p=0;
	switch (part) {
	    case 1: if (sid>MAX_DEPT_NO-1) return 0x99;
	    	    memcpy(temp,&id,2);	// Department
	            memcpy(&temp[2],&skey,2);
		    memcpy(&temp[4],desc1,28);
		    sz=DEPARTMENT_STRUCT_SIZE;
		    p = (id-1);
		    p*= sz;
		    p+=FLASH_DEPARTMENT_AREA;
		    break;
 	    case 2: if (sid>MAX_GROUP-1) return 0x99;
 	    	    memcpy(temp,&id,2);	// Group
		    memcpy(&temp[2],desc1,30);
#ifdef USE_TRACE_MEATGRADE
		    memcpy(&temp[30], &order, 2);	// grade order
#endif
		    sz = GROUP_STRUCT_SIZE;
		    p =(id-1)*sz;
		    p+= FLASH_SUB_DEPARTMENT_AREA;
		    break;
	    case 3: if (sid>MAX_LABEL_MSG_NO-1) return 0x99;
	    	    memcpy(temp,&id,2); // Label Message
		    memcpy(&temp[2],desc1,PRT_ADVE_STRUCT_SIZE - 2);
		    sz=PRT_ADVE_STRUCT_SIZE;
		    p =(id-1)*sz;
		    p+=FLASH_PRT_ADVE_AREA;
		    break;
	    case 4: if (sid>MAX_ORIGIN_NO) return 0x99;
	            memcpy(temp,&id,2); // Origin
		    memcpy(&temp[2],desc1,32);
		    sz=ORIGIN_STRUCT_SIZE;
		    p =(id-1)*sz;
		    p+=FLASH_ORIGIN_AREA;
		    break;
//Modified by JJANG 20081009
	    case 5: if (sid>MAX_PCS_NO) return 0x99;
	    	    memcpy(temp,&id,2); // PCS
		    memcpy(&temp[2],desc1,6);
		    sz=PCS_UNIT_STRUCT_SIZE;
		    p =(id-1)*sz;
		    p+=FLASH_PCS_UNIT_AREA;
		    break;
#ifdef USE_TAX_RATE            
	    case 6: if (sid>MAX_TAX_NO-1) return 0x99;
	    	    memset(&tax,0,sizeof(tax));
	    	    tax.tax_code=(INT16U)id; // TAX
		    tax.tax_type = xtype;
		    tax.tax_rate = rvalue;
		    sz=TAX_STRUCT_SIZE;
		    memcpy(temp,&tax,sizeof(tax));
		    p =(id-1)*sz;
		    p+=FLASH_TAX_AREA;
		    break;
#endif            
	    case 7: if (sid>MAX_TARE_NO) return 0x99;
	    	    memset(&tare,0,sizeof(tare));
		    tare._code = (INT16U)id;
		    memcpy(tare._description,desc1,17);
		    tare._type = xtype;
		    tare._value= rvalue;
#ifdef COUNTRY_USA
		    tare._weightunit = 1;
#endif
		    sz=TARE_STRUCT_SIZE;
		    memcpy(temp,&tare,sizeof(tare));
		    p =(id-1)*sz;
		    p+=FLASH_TARE_AREA;
		    break;
	    case 8: 
			if (sid>MAX_BARCODE_NO) return 0x99;
	        memset(&barcode,0,sizeof(barcode)); // Barcode Format
		    barcode.barcode_type=xtype;
		    barcode.barcode_num = (INT16U)id;
		    barcode.barcode_format_num = formno;
		    if (barcode.barcode_format_num) {
			barcode_make_format(barcode.barcode_format_num,barcode.barcode_type,barcode.barcode_format);
		    } else {
			memcpy(barcode.barcode_format,desc2,BARCODE_FORMAT_LEN);
		    }
		    memcpy(barcode.barcode_description,desc1,sizeof(barcode.barcode_description));
		    //barcode.barcode_format[29]=0;
		    barcode.barcode_description[sizeof(barcode.barcode_description)-1]=0;
		    sz=BARCODE_STRUCT_SIZE;
		    memcpy(temp,&barcode,sizeof(barcode));
		    p =(id-1)*sz;
		    p+=FLASH_BARCODE_FORMAT_AREA;
		    break;
#ifdef USE_FIXED_PERCENT_KEY_SET            
	    case 9: 
			if (sid>MAX_DISCOUNT_KEY_NO) return 0x99;
	    	memset(&discKey,0,sizeof(discKey));	// Discount rate (1~99%)
	    	discKey.code=(INT16U)id; // TAX
		    discKey.rate = rvalue;
		    sz=DISCOUNT_KEY_STRUCT_SIZE;
		    memcpy(temp,&discKey,sizeof(discKey));
		    p =(id-1)*sz;
		    p+=FLASH_DISCOUNT_RATE_AREA;
		    break;
#endif  
	}
	if (p) {
	    p+=DFLASH_BASE;
	    Flash_swrite(p,(HUGEDATA INT8U *)temp,(INT16U)sz);
	    commun_recv_monitor('T',(INT16S)part,(INT16S)id);
#ifdef USE_TRACE_MEATGRADE
	    if (part == 2) // group(grade)
	    {
		trace_meatgrade_getGradeListText();
	    }
#endif
	} else {
	    return 0x96;
	}
	return 1;
}

INT8U commun_recv_table2(HUGEDATA COMM_BUF *CBuf,INT32U spart,INT16U sid,INT8U *str,INT16S leng, INT8U cmd)
/**********************************************************************
 Ingredient,Tracebility,Country,
 **********************************************************************/
{
	INT16S    r,status,i;
	INT16S    part;
	INT16U id;//,erase;
	ETH_CONNECT_LIST eth;
	char   temp[256];
	INT8U *pdata;
	char   desc1[MAX_TABLE_BUF],desc2[MAX_TABLE_BUF],desc3[MAX_TABLE_BUF];

	INT8U  xtype,code;
	INT16U sz,nth,narray,desc_size;
#ifdef USE_EU_NUTRITIONFACT
	INT32U varray[45],dvalue;
#elif defined(USE_ADDITIONAL_NUTRIENT)
	INT32U varray[30],dvalue;
#else	
	INT32U varray[20],dvalue;
#endif
#ifdef USE_TRACEABILITY
	INT32U rvalue;
#endif
	INT32U dsize,p,rp,v32;
	NUTRIFACT_STRUCT nutri_fact;
	NUTRIFACT_STRUCT2 nutri_fact2;
#ifdef USE_EU_NUTRITIONFACT
	NUTRIFACT_STRUCT3 nutri_fact3;
#endif
#ifdef USE_ADDITIONAL_NUTRIENT
	NUTRIFACT_STRUCT4 nutri_fact4;
#endif
#ifdef USE_9999_INDIRECT_INGREDIENT
	INT16U ingre_id;
#endif

#ifdef USE_INGREDIENT_CODE_4DIGIT
	INT8U being;
	INT16U tableId, index, newIndex;
#endif

#ifdef USE_TRACEABILITY
	TRACEABILITY_STRUCT trace;
	WORKPLACE_STRUCT work;
#endif
#ifdef USE_TRACE_MEATGRADE
	INT8U  gradeNo=0;
#endif
	INT16S    l;
	char *commun_buffer;
	INT8U ucTemp, nutri_type=0;
#ifdef USE_TRACE_1000QTY //sewon 161222
	INT8U  meatKind=0;
	INT8U  slaughtNo=0;
	INT8U harmful = 0;
	char  desc7[MAX_TABLE_BUF];	
#endif 
#ifdef USE_TRACE_STANDALONE
	TRACE_INDIVIDUAL_IDENTITY indivStr;
	long  indivWeight;
	char  desc4[MAX_TABLE_BUF], desc5[MAX_TABLE_BUF];
	INT8U desc6[MAX_TABLE_BUF_SLAUGHTER];
	INT8U lotFlag = 0; //gm 090405
#ifdef USE_CHECKING_HARMFUL_MEAT
	INT8U delFlag = 0;
	INT8U harmful = 0;
#endif    
	memset(desc4,0,MAX_TABLE_BUF);
	memset(desc5,0,MAX_TABLE_BUF);
	memset(desc6,0,MAX_TABLE_BUF_SLAUGHTER);//gm 090404
#endif
#ifdef USE_TRACE_1000QTY //sewon 161222
	memset(desc7,0,MAX_TABLE_BUF);
#endif 

	memset((INT8U *)&eth,0,sizeof(ETH_CONNECT_LIST));
	part = (INT16S)spart;
	id   = (INT16S)sid;
	if (part<0) return 0x82;
	if (id  <0) return 0x82;
	if (id == 0 && part != 7) return 0x82;	// id가 0일 경우 error return, 단, 개체데이터(part=7)는 허용    
	if (leng<5) return 0x99;
#ifdef USE_TOPMART_ASK_INDIV_INFO
	if (id == 9999) //sewon 170214
	{
		network_status.indiv_info_result = 1;	
		return 1;
	}
#endif
	status = 0;
	r      = 0;
	dsize  = 0;
	xtype  = 0;
	code   = 0;
	nth    = 0;
	desc_size = 0;
	memset(desc1,0,MAX_TABLE_BUF);
	memset(desc2,0,MAX_TABLE_BUF);
	memset(desc3,0,MAX_TABLE_BUF);
#ifdef USE_EU_NUTRITIONFACT
	ucTemp = get_global_bparam(GLOBAL_SALE_SETUP_FLAG9);
	// nutri_type (0:normal, 1:CHL, 2:CAN, 3:AUS, 4:NZL, 5: AUS for actual weight, 6: NZL for actual weight)
	nutri_type = ucTemp&0x0f;
	for (i=0; i<45; i++) varray[i]=0;
#elif defined(USE_ADDITIONAL_NUTRIENT)
	for (i=0; i<sizeof(varray)/sizeof(varray[0]); i++) varray[i]=0;		
#else
	for (i=0; i<20; i++) varray[i]=0;
#endif

	while (1) {
		switch (status) {
			case  0:code=str[r];
				dsize  = 0;
				dvalue = 0;
				if (!code) goto END;
				if (!(((code>='a') && (code<='z')) || ((code>='A') && (code<='Z')))) status=20;
				else status=1;
				break;
			case  1:
				dsize = 0;
				if (str[r]=='=') status=2;
				else status=0;
				break;
			case  2:if ((str[r]=='.') || (str[r]==':')) {
					if (code=='Z') {
						narray = (INT16U)dsize;
						dvalue = 0;
						status = 3;
						break;
					} else {
#ifdef USE_TRACE_1000QTY
					    if ((code=='D') || (code=='S') || (code=='P') || (code=='B') || (code=='F') || (code=='W') || (code=='M') || (code=='N')) //sewon 161223
#else
					    if ((code=='D') || (code=='S') || (code=='P') || (code=='B') || (code=='F') || (code=='W') || (code=='M'))
#endif
					    { 
						status=10;
					    } else {
						switch (code) {
						    case 'T': xtype=(INT8U)dsize; break;
						    case 'X': nth  =(INT16U)dsize; break;
#ifdef USE_TRACEABILITY
						    case 'C': rvalue=(INT32U)dsize; break;
#endif
#ifdef USE_TRACE_1000QTY //sewon 161222
						    case 'K': meatKind=(INT8U)dsize; break;
						    case 'U': slaughtNo=(INT8U)dsize; break;
							case 'H': harmful = (INT8U)dsize; break;	// indiv harmfulness flag(0:normal, 1:harmful)
#endif							
#ifdef USE_TRACE_MEATGRADE
						    case 'G': gradeNo=(INT8U)dsize; break;
#endif
#ifdef USE_TRACE_STANDALONE
						    case 'L': lotFlag = (INT8U)dsize; break; //gm 090405, (0: not used, 1: used)
#ifdef USE_CHECKING_HARMFUL_MEAT
						    case 'A': delFlag = (INT8U)dsize; break;	// indiv add flag(0:add, 1:delete)
						    case 'H': harmful = (INT8U)dsize; break;	// indiv harmfulness flag(0:normal, 1:harmful)
#endif
#endif
						}
						status=0;
					    }
					}
					break;
				} else {
					dsize<<=4;
					dsize|=ctoh(str[r]);
				}
				break;
			case  3:if (str[r]=='.') {
#ifdef USE_EU_NUTRITIONFACT
				if (nutri_type == NUTRI_TYPE_42ITEM_5DIG_EU_ACTUAL_WEIGHT)
				{
					if (narray<45) {
						varray[narray]=dvalue;
					}
				}
				else
				{
					if (narray<20) {
						varray[narray]=dvalue;
					}
				}
#elif defined(USE_ADDITIONAL_NUTRIENT)
					if (narray < sizeof(varray)/sizeof(varray[0])) {
						varray[narray]=dvalue;
					}				
#else
					if (narray<20) {
						varray[narray]=dvalue;
					}
#endif
					status=0;
				} else {
					dvalue<<=4;
					dvalue|=ctoh(str[r]);
				}
				break;
			case 10:if (part==1) {
				    pdata = &str[r];
				    desc_size=dsize;
				    r+= dsize;
				} 
#ifdef USE_TRACE_STANDALONE				
				else if (part == TABLE2_INDIVIDUAL)
				{
					l = dsize;
					if (l > MAX_TABLE_BUF) l = MAX_TABLE_BUF;
					if (code=='D') 
					{
						for (i=0; i<l; i++) desc1[i]=str[r++];
						desc1[l]=0;
					} 
//					else if (code=='G')
//					{
//						for (i=0; i<l; i++) desc2[i]=str[r++];
//						desc2[l]=0;
//					} 
					else if (code=='W') 
					{
						for (i=0; i<l; i++) desc3[i]=str[r++];
						desc3[l]=0;
					} 
					else if (code=='B') 
					{
						for (i=0; i<l; i++) desc4[i]=str[r++];
						desc4[l]=0;
					}
					else  if (code=='F') 
					{
						for (i=0; i<l; i++) desc5[i]=str[r++];
						desc5[l]=0;
					}
					else if (code == 'S')//gm 090404, slaughter house
					{
						for (i=0; i<l; i++) desc6[i]=str[r++];
						desc6[l]=0;
					}
					else if (code == 'M')
					{
						for (i=0; i<l; i++) desc2[i]=str[r++];
						desc2[l]=0;
					}
  #ifdef USE_TRACE_1000QTY //sewon 161222
					else if (code == 'N')
					{
						for (i=0; i<l; i++) desc7[i]=str[r++];
						desc7[l]=0;
					}
  #endif
					r += (dsize - l);
				}
#endif // USE_TRACE_STANDALONE				
				else 
				{
					l = dsize;
					if (l > MAX_TABLE_BUF) l = MAX_TABLE_BUF;
					if (code=='D') {
						for (i=0; i<l; i++) desc1[i]=str[r++];
						desc1[l]=0;
					} else if (code=='S') {
						for (i=0; i<l; i++) desc2[i]=str[r++];
						desc2[l]=0;
					} else {
						for (i=0; i<l; i++) desc3[i]=str[r++];
						desc3[l]=0;
					}
					r += (dsize - l);
				}
				status=0;
				goto SKIP;
			case 20:status=0;
				v32   =0;
				if (str[r]=='=') status=21;
				break;
			case 21:if (str[r]=='.') {
					commun_common_ethernet(code,&eth,v32);
					status=0;
				} else {
					v32<<=4;
					v32|=ctoh(str[r]);
				}
				break;
		}
		r++;
		SKIP:;
	}
END:
	p=0;
	switch (part) {
		case TABLE2_INGREDIENT: 
#if defined(USE_9999_INDIRECT_INGREDIENT) || defined(USE_INGREDIENT_CODE_4DIGIT)
			if (id>MAX_INGREDIENT_INDEX_NUMBER) return 0x99;
#else
			if (id>MAX_INGREDIENT_NO) return 0x99;
#endif
			sz = desc_size;
			for (i = sz; i <= INGREDIENT_COM_BLOCK_SIZE; i++) pdata[i] = 0;
			r = (INGREDIENT_STRUCT_SIZE-2);
			i = r / INGREDIENT_COM_BLOCK_SIZE;
			if (r%INGREDIENT_COM_BLOCK_SIZE) i++;
			rp = nth*INGREDIENT_COM_BLOCK_SIZE;
			if ((i-1) == nth) 
			{
				sz = r - rp;
			} 
			else if ((i-1) > nth) 
			{
				sz = INGREDIENT_COM_BLOCK_SIZE;
				if (sz == desc_size) sz++;	// insert null
			} 
			else 
			{	// exceed block number
				return 0x90;
			}
#ifdef USE_9999_INDIRECT_INGREDIENT
			ingre_id = ingredient_get_id_by_table(id);
			if(ingre_id==0) // Empty ingredient
			{
				ingre_id = ingredient_create_table(id);
			}
			p  = ing_calc_addr_by_key(ingre_id-1);
#elif defined(USE_INGREDIENT_CODE_4DIGIT)	
			p = ingredient_insert(id, pdata, sz);

			if(!p) return 0x99;	// memory full
#else
			p  = ing_calc_addr_by_key(id-1);
			p += rp;
#endif
			break;
#ifdef USE_NUTRITION_FACT
		case TABLE2_NUTRITION_FACT: 
#ifdef USE_DIRECT_NUTRITION
			return 0x96;	//invalid protocol
#endif
#ifndef USE_EU_NUTRITIONFACT
			ucTemp = get_global_bparam(GLOBAL_SALE_SETUP_FLAG9);
			// nutri_type (0:normal, 1:CHL, 2:CAN, 3:AUS, 4:NZL, 5: AUS for actual weight, 6: NZL for actual weight)
			nutri_type = ucTemp&0x0f;
#endif
#ifdef COUNTRY_USA
			ucTemp = get_global_bparam(GLOBAL_SALE_SETUP_FLAG9);
			// nutri_type (0:normal, 1:CHL, 2:CAN, 3:AUS, 4:NZL, 5: AUS for actual weight, 6: NZL for actual weight)
			nutri_type = NUTRI_TYPE_2COL_USA;
#endif
			if (nutri_type == NUTRI_TYPE_2COL_6DIG_AUS || nutri_type == NUTRI_TYPE_2COL_6DIG_AUS_ACTUAL_WEIGHT
				|| nutri_type == NUTRI_TYPE_6DIG_CZE)
			{
				if (id>MAX_NUTRIFACT2_NO) return 0x99;
				sz =sizeof(nutri_fact2);
				memset(&nutri_fact2,0,sizeof(nutri_fact2));
				nutri_fact2.code=(INT16U)id;
				memcpy(nutri_fact2.description ,desc1,20);
				memcpy(nutri_fact2.serving_size,desc2,20);
				memcpy(nutri_fact2.serving_per ,desc3,10);
				for (i=0; i<20; i++) nutri_fact2.nutri_data[i]=varray[i];
				nutri_fact2.type = (INT16U)xtype;
			}
#ifdef USE_EU_NUTRITIONFACT
			else if (nutri_type == NUTRI_TYPE_42ITEM_5DIG_EU_ACTUAL_WEIGHT)
			{
				if (id>MAX_NUTRIFACT3_NO) return 0x99;
				sz =sizeof(nutri_fact3);
				memset(&nutri_fact3,0,sizeof(nutri_fact3));
				nutri_fact3.code=(INT16U)id;
				memcpy(nutri_fact3.description ,desc1,20);
				memcpy(nutri_fact3.serving_size,desc2,20);
				memcpy(nutri_fact3.serving_per ,desc3,10);
				for (i=0; i<45; i++) nutri_fact3.nutri_data[i]=varray[i];
				nutri_fact3.type = (INT16U)xtype;
			}
#endif
#ifdef USE_ADDITIONAL_NUTRIENT
			else if(nutri_type == NUTRI_TYPE_2COL_USA || nutri_type == NUTRI_TYPE_TOTALFAT_CAN)
			{
				if (id>MAX_NUTRIFACT4_NO) return 0x99;
				sz =sizeof(nutri_fact4);
				memset(&nutri_fact4,0,sizeof(nutri_fact4)); 
  #ifdef USE_DIRECT_NUTRITION
  #else
				nutri_fact4.code=(INT16U)id;
  #endif
				memcpy(nutri_fact4.description ,desc1,sizeof(nutri_fact4.description));
				memcpy(nutri_fact4.serving_size,desc2,sizeof(nutri_fact4.serving_size));
				memcpy(nutri_fact4.serving_per ,desc3,sizeof(nutri_fact4.serving_per));
				for (i=0; i<30; i++) 
				{
					nutri_fact4.nutri_data[i] = varray[i];
				}
  #ifdef USE_DIRECT_NUTRITION
  #else
				nutri_fact4.type = (INT16U)xtype;
  #endif
			}
#endif
			else
			{
				if (id>MAX_NUTRIFACT_NO) return 0x99;
				sz =sizeof(nutri_fact);
				memset(&nutri_fact,0,sizeof(nutri_fact));
				nutri_fact.code=(INT16U)id;
				memcpy(nutri_fact.description ,desc1,20);
				memcpy(nutri_fact.serving_size,desc2,20);
				memcpy(nutri_fact.serving_per ,desc3,10);
				for (i=0; i<20; i++) nutri_fact.nutri_data[i]=varray[i];
				nutri_fact.type = (INT16U)xtype;
			}
			p = (id-1);
			p*= sz;
			p+= FLASH_NUTRIFACT_AREA;
			break;
#endif
#ifdef USE_TRACEABILITY //gm 090407, memory 충돌 방지
		case TABLE2_TRACEABILITY: if (id>MAX_TRACEABILITY_NO-1) return 0x99;
		    memset(&trace,0,sizeof(trace));
		    trace.code=id;
		    memcpy(trace.contents,desc1,TRACEABILITY_STRUCT_SIZE-10);	// Modified by CJK 20060415
		    for (i=0; i<4; i++) trace.idata[i]=(INT16U)varray[i];
		    memcpy(temp,&trace,sizeof(trace));
		    sz=TRACEABILITY_STRUCT_SIZE;
		    p = (id-1);
		    p*= TRACEABILITY_STRUCT_SIZE;
		    p+=FLASH_TRACEABILITY_AREA;
		    break;
		case TABLE2_COUNTRY: if (id>MAX_COUNTRY_NO-1) return 0x99;
		    memcpy(temp,&id,2);
		    memcpy(&temp[2],desc1,COUNTRY_STRUCT_SIZE-2);	// Modified by CJK 20060415
		    sz=COUNTRY_STRUCT_SIZE;
		    p = (id-1);
		    p*= COUNTRY_STRUCT_SIZE;
		    p+=FLASH_COUNTRY_AREA;
		    break;
		case TABLE2_SLAUGHTERHOUSE: if (id>MAX_SLAUGHTERHOUSE_NO-1) return 0x99;
		    memset(&work,0,sizeof(work));
		    work.code=id;
		    memcpy(work.text,desc1,SLAUGHTERHOUSE_STRUCT_SIZE-4);	// Modified by CJK 20060415
		    work.country_number=(INT16U)rvalue;
		    memcpy(temp,&work,sizeof(work));
		    sz=SLAUGHTERHOUSE_STRUCT_SIZE;
		    p = (id-1);
		    p*= SLAUGHTERHOUSE_STRUCT_SIZE;
		    p+=FLASH_SLAUGHTERHOUSE_AREA;
		    break;
		case TABLE2_CUTTINGHALL: if (id>MAX_CUTTINGHALL_NO-1) return 0x99;
		    memset(&work,0,sizeof(work));
		    work.code=id;
		    memcpy(work.text,desc1,CUTTINGHALL_STRUCT_SIZE-4);	// Modified by CJK 20060415
		    work.country_number=(INT16U)rvalue;
		    memcpy(temp,&work,sizeof(work));
		    sz=CUTTINGHALL_STRUCT_SIZE;
		    p = (id-1);
		    p*= CUTTINGHALL_STRUCT_SIZE;
		    p+=FLASH_CUTTINGHALL_AREA;
		    break;
#endif
#ifdef USE_TRACE_STANDALONE

		case TABLE2_INDIVIDUAL: if (id>MAX_INDIVIDUAL_NO-1) return 0x99;

			memset(&indivStr,0,sizeof(indivStr));
  #ifdef USE_CHECKING_HARMFUL_MEAT
		    if (id >= MaxIndividualNo && delFlag != 1) return 0x99;        
		    if (id == 0)	// id=0:위해식별번호일 경우
		    {
			id = individualData_search((INT8U*)desc1, 2);	// 위해리스트 영역에서 검색
			if (delFlag == 1)
			{
				if (id == MAX_INDIVIDUAL_NO) return 1;	// 삭제하려는 개체번호가 없을 경우 정상 응답
				if (id < MaxIndividualNo || id > MAX_INDIVIDUAL_NO || id == 0) return 1;	// 발생할 수 없는 값(방어)
				p = (id - 1);
				p *= TRACE_INDIVIDUAL_IDENTITY_SIZE;
				p += FLASH_TRACE_STANDALONE_AREA;
    #ifdef USE_TRACE_1000QTY //sewon 161205
				p += EXT_MEMCARD_ADDR;
    #else
				p += DFLASH_BASE;
    #endif
				Flash_bwrite(p, 0);
				return 1;
			}
			if (id == MAX_INDIVIDUAL_NO)	// not found
			{
				id = individualData_searchEmptyIdx();	// 위해리스트 영역에서 빈곳 검색
				if (id == MAX_INDIVIDUAL_NO) return 0x99;	// momory full 
			}
		    }
  #endif  // USE_CHECKING_HARMFUL_MEAT           
		    indivStr.index = id;
		    memcpy(indivStr.individualNO ,desc1, sizeof(indivStr.individualNO));
  #ifdef USE_IMPORT_LOTNO
			if(indivStr.individualNO[0] != 'A')
			{
		    	indivStr.individualNO[sizeof(indivStr.individualNO)-1] = 0;
			}
  #else
		    indivStr.individualNO[sizeof(indivStr.individualNO)-1] = 0;
  #endif
		    indivWeight = atol(desc3);
		    memcpy(indivStr.farm ,desc5, sizeof(indivStr.farm));
		    indivStr.farm[sizeof(indivStr.farm)-1] = 0;	// 사육지 또는 묶음부위
		    indivStr.lotFlag = lotFlag; 
		    memcpy(indivStr.breed ,desc4, sizeof(indivStr.breed));
		    indivStr.breed[sizeof(indivStr.breed)-1] = 0;
  #ifdef USE_TRACE_MEATGRADE
		    indivStr.gradeNo = gradeNo;	// V1.88.0 이후 다시 사용
  #else
		    memcpy(indivStr.grade, desc2, sizeof(indivStr.grade));
		    indivStr.grade[sizeof(indivStr.grade)-1] = 0;            
  #endif
  #ifdef USE_KOR_TRACE_999
		    if (lotFlag)
		    {
			    memcpy(indivStr.slaughtHouse ,desc6, 101);//gm 090404
			    indivStr.slaughtHouse[101-1] = 0;
		    }
		    else
		    {
			    memcpy(indivStr.slaughtHouse ,desc6, sizeof(indivStr.slaughtHouse));//gm 090404
			    indivStr.slaughtHouse[sizeof(indivStr.slaughtHouse)-1] = 0;
			    indivStr.slaughtDate[0] = (unsigned char)varray[0];
			    indivStr.slaughtDate[1] = (unsigned char)varray[1]; 
    		    indivStr.slaughtDate[2] = (unsigned char)varray[2]; 
		    }
  #else
		    memcpy(indivStr.slaughtHouse ,desc6, sizeof(indivStr.slaughtHouse));
		    indivStr.slaughtHouse[sizeof(indivStr.slaughtHouse)-1] = 0;
		    indivStr.slaughtDate[0] = (unsigned char)varray[0];
		    indivStr.slaughtDate[1] = (unsigned char)varray[1]; 
		    indivStr.slaughtDate[2] = (unsigned char)varray[2];        
  #endif
  #ifdef USE_TRACE_1000QTY //sewon 161222
			memcpy(indivStr.nongjangNo,desc7, sizeof(indivStr.nongjangNo));
			indivStr.nongjangNo[sizeof(indivStr.nongjangNo)-1] = 0;
		    indivStr.meatKind= meatKind;
			indivStr.slaughtNo= slaughtNo;
		    indivStr.harmfulness = harmful;
  #endif
  #ifdef USE_CHECKING_HARMFUL_MEAT
		    if (id < MaxIndividualNo) harmful = 0;	// 영역에 따라 고정
		    else harmful = 1;            
		    indivStr.harmfulness = harmful;
  #endif            
		    memcpy(temp, &indivStr, sizeof(indivStr));		    
		    set_nvram_lparam(NVRAM_TRACE_STANDALONE_WEIGHT+((INT32U)indivStr.index-1)*4, indivWeight); //Added by JJANG 20080710
		    sz=TRACE_INDIVIDUAL_IDENTITY_SIZE;
		    if (id == 0) return 0x99;            
		    p = (id-1);
		    p*= TRACE_INDIVIDUAL_IDENTITY_SIZE;
		    p+=FLASH_TRACE_STANDALONE_AREA;
		    break; 
		case TABLE2_SPECIALUSE: if (id>MAX_SPECIALUSE_NO-1) return 0x99;
		    memcpy(temp,&id,2);
		    memcpy(&temp[2],desc1,TRACE_SPECIALUSE_STRUCT_SIZE-2);//reserved 는 임의의 값이 들어감
		    sz=TRACE_SPECIALUSE_STRUCT_SIZE;
		    p = (id-1);
		    p*= TRACE_SPECIALUSE_STRUCT_SIZE;
		    p+=FLASH_TRACE_SPECIALUSE_AREA;
		    break; 
		case TABLE2_SLAUGHTERHOUSE_KR: if (id>MAX_SLAUGHTERHOUSE_KR_NO-1) return 0x99;
  #ifdef USE_KOR_SLAUGHT_CODE_4DIGIT
		    if (!trace_slaughthousedata_insert(id, (INT8U *)desc1)) return 0x99;	// memory full
  #endif        
		    memcpy(temp,&id,2);
		    memcpy(&temp[2],desc1,TRACE_SLAUGHTERHOUSE_KR_STRUCT_SIZE-2);
		    sz=TRACE_SLAUGHTERHOUSE_KR_STRUCT_SIZE;
		    p = (id-1);
		    p*= TRACE_SLAUGHTERHOUSE_KR_STRUCT_SIZE;
		    p+=FLASH_TRACE_SLAUGHTERHOUSE_AREA;
		    break;
  #ifdef USE_TRACE_MEATPART
		case TABLE2_MEATPART: if (id>MAX_MEATPART_NO-1) return 0x99;
		    memcpy(temp,&id,2);
		    memcpy(&temp[2],desc1,TRACE_MEATPART_STRUCT_SIZE-2);
		    sz=TRACE_MEATPART_STRUCT_SIZE;
		    p = (id-1);
		    p*= TRACE_MEATPART_STRUCT_SIZE;
		    p+=FLASH_TRACE_MEATPART_AREA;
		    break;
  #endif            
#endif  // USE_TRACE_STANDALONE

	}
	if (p) 
	{
#ifdef USE_TRACE_1000QTY //sewon 161206
	    if (part==TABLE2_INDIVIDUAL) p+=EXT_MEMCARD_ADDR;
		else if(part!=TABLE2_INGREDIENT) p+=DFLASH_BASE;
#else
	    if (part!=TABLE2_INGREDIENT) p+=DFLASH_BASE;
#endif
		if (part==TABLE2_INGREDIENT) 
		{
#ifndef USE_INGREDIENT_CODE_4DIGIT
			if (nth==0) 
			{
				memcpy(temp,&id,2);
				Flash_swrite(p,(HUGEDATA INT8U*)temp,2);
			}
			p+=2;
			Flash_swrite(p,(HUGEDATA INT8U *)pdata,sz);
#endif
		} 
#ifdef USE_NUTRITION_FACT
		else if (part==TABLE2_NUTRITION_FACT) 
		{
			if (nutri_type == NUTRI_TYPE_2COL_6DIG_AUS || nutri_type == NUTRI_TYPE_2COL_6DIG_AUS_ACTUAL_WEIGHT
				|| nutri_type == NUTRI_TYPE_6DIG_CZE)
			{
				Flash_swrite(p,(HUGEDATA INT8U *)&nutri_fact2,sz);
			}
#ifdef USE_EU_NUTRITIONFACT
			else if (nutri_type == NUTRI_TYPE_42ITEM_5DIG_EU_ACTUAL_WEIGHT)
			{
				Flash_swrite(p,(HUGEDATA INT8U *)&nutri_fact3,sz);
			}
#endif
#ifdef USE_ADDITIONAL_NUTRIENT
			else if(nutri_type == NUTRI_TYPE_2COL_USA || nutri_type == NUTRI_TYPE_TOTALFAT_CAN)
			{
				Flash_swrite(p,(INT8U *)&nutri_fact4,sz);
			}
#endif
			else Flash_swrite(p,(HUGEDATA INT8U *)&nutri_fact,sz);
		}
#endif
#ifdef USE_KOR_SLAUGHT_CODE_4DIGIT
		else if (part==9) 
		{
		// no operation
		}
#endif        
		else 
		{
			Flash_swrite(p,(HUGEDATA INT8U *)temp,sz);
	    }
	    commun_recv_monitor('T',(INT16S)part,(INT16S)id);

 	    if (cmd == 0x72) // master->slave(table2)
 	    {
 		commun_buffer=(char *)serial_send_point;
 		r=ethernet_gen_head(commun_buffer);
 		sprintf(&commun_buffer[r],"C01F77,%02X%04X",part,id);	// part 1:ingredient
 		r+=strlen(&commun_buffer[r]);
 		commun_buffer[r++]=0x0a;
 		commun_buffer[r++]=0x0;
 		commun_outleng(CBuf,(char *)commun_buffer,r);
 		return 0;
 	    }
 	    else	// if(cmd == 0x31) // pc->scale(mater)
	    {
		if (network_status.service_flag&0x20)	// send plu
		{
			if (ethernet_list.status == 1)	// 전송된 data는 master일 때만 sync flag 설정
			{
				commun_net_mask(2, &id, 0xff, 1);
			}
		}
#ifdef USE_TOPMART_ASK_INDIV_INFO
		network_status.indiv_info_result = 0; //sewon 170214
#endif
	    	return 1;
	    }
	}
	else // if(p)
	{
		return 0x83;
	}
}

#ifdef USE_DIRECT_NUTRITION
INT8U commun_recv_direct_nutrition(COMM_BUF *CBuf,INT32U plu_no,INT16U dept_no,INT8U *str,INT16S leng)
/**********************************************************************
 Ingredient,Tracebility,Country,
 **********************************************************************/
{
	INT16S    ret,r,status,com_type;
	INT16S    part;
	INT16U id;//,erase;
	char   temp[256];
	INT8U *pdata;
	char   desc1[MAX_TABLE_BUF],desc2[MAX_TABLE_BUF],desc3[MAX_TABLE_BUF];
	int i, l;
	INT8U  xtype,formno,code, ucTemp, nutri_type=0;;
	INT16U skey,sz,nth,narray,desc_size, pluAddr, idx;
	INT32U varray[30],dvalue;
	INT32U rvalue,dsize,p,rp,v32, key;
	ETH_CONNECT_LIST eth;
#ifdef USE_ADDITIONAL_NUTRIENT
	NUTRIFACT_STRUCT4 nutri_fact4;
	NUTRIFACT_STRUCT4 nutri_fact4_temp;
	//NUTRIFACT_ADDTIONAL_STRUCT nutri_add_fact;
	//INT16U sz2;
	//INT32U varray2[5];
#endif
	char *commun_buffer;
       
	if (plu_no <=0) return 0x82;
	if (leng<5) return 0x99;
	
	if (!plu_table_search(dept_no, plu_no,(INT16U *)&idx,0)) {
		return 0x98; // PLU 없음
	}
	memset((INT8U *)&eth,0,sizeof(ETH_CONNECT_LIST));
	key = (INT32U)idx;
	com_type=serial_get_comtype(CBuf);
	
	ret    = 1;
	status = 0;
	r      = 0;
	dsize  = 0;
	xtype  = 0;
	formno = 0;
	code   = 0;
	skey   = 1;
	nth    = 0;
	desc_size = 0;
	memset(desc1,0,MAX_TABLE_BUF);
	memset(desc2,0,MAX_TABLE_BUF);
	memset(desc3,0,MAX_TABLE_BUF);
	for (i=0; i<sizeof(varray)/sizeof(varray[0]); i++) varray[i]=0;

	while (1) {
		switch (status) {
			case  0:code=str[r];
				dsize  = 0;
				dvalue = 0;
				if (!code) goto END;
				if (!(((code>='a') && (code<='z')) || ((code>='A') && (code<='Z')))) status=20;
				else status=1;
				break;
			case  1:
				dsize = 0;
				if (str[r]=='=') status=2;
				else status=0;
				break;
			case  2:if ((str[r]=='.') || (str[r]==':')) {
					if (code=='Z') {
						narray = (INT16U)dsize;
						dvalue = 0;
						status = 3;
						break;
					}
#ifdef USE_ADDITIONAL_NUTRIENT
					/*else if (code=='a' || code=='b' || code=='c' || code=='d' || code=='e') {
						narray = (INT16U)dsize;
						dvalue = 0;
						status = 3;
						break;
					}*/
#endif
					else {
					    if ((code=='D') || (code=='S') || (code=='P') || (code=='B') || (code=='F') || (code=='W') || (code=='M'))
					    { 
						status=10;
					    } else {
						switch (code) {
						    case 'T': xtype=(INT8U)dsize; break;
						    case 'X': nth  =(INT16U)dsize; break;
						    case 'C': rvalue=(INT32U)dsize; break;
						}
						status=0;
					    }
					}
					break;
				} else {
					dsize<<=4;
					dsize|=ctoh(str[r]);
				}
				break;
			case  3:if (str[r]=='.') {
					if (narray<sizeof(varray)/sizeof(varray[0])) {
						varray[narray]=dvalue;
					}
					status=0;
				}
#ifdef USE_ADDITIONAL_NUTRIENT
/*				else if(str[r]==':')
				{
					if(str[r-1]=='a' || str[r-1]=='b' || str[r-1]=='c' || str[r-1]=='d' || str[r-1]=='e')
					{
						varray2[str[r-1]-'a']=dvalue;
					}
				}*/
#endif
				else {
					dvalue<<=4;
					dvalue|=ctoh(str[r]);
				}
				break;
			case 10:
				l = dsize;
				if (l > MAX_TABLE_BUF) l = MAX_TABLE_BUF;
				if (code=='D') {
					for (i=0; i<l; i++) desc1[i]=str[r++];
					desc1[l]=0;
				} else if (code=='S') {
					for (i=0; i<l; i++) desc2[i]=str[r++];
					desc2[l]=0;
				} else {
					for (i=0; i<l; i++) desc3[i]=str[r++];
					desc3[l]=0;
				}
				r += (dsize - l);
				status=0;
				goto SKIP;
			case 20:status=0;
				v32   =0;
				if (str[r]=='=') status=21;
				break;
			case 21:if (str[r]=='.') {
					commun_common_ethernet(code,&eth,v32);
					status=0;
				} else {
					v32<<=4;
					v32|=ctoh(str[r]);
				}
				break;
		}
		r++;
		SKIP:;
	}
END:
	if(dept_no > MAX_DEPT_NO || plu_no > MAX_PLU_NUMBER) 
		return 0x99;
	sz =sizeof(nutri_fact4);
	memset(&nutri_fact4,0,sizeof(nutri_fact4));
	nutri_fact4.dept_no = dept_no;
	nutri_fact4.plu_num = plu_no;

	memcpy(nutri_fact4.description ,desc1,sizeof(nutri_fact4.description));
	memcpy(nutri_fact4.serving_size,desc2,sizeof(nutri_fact4.serving_size));
	memcpy(nutri_fact4.serving_per ,desc3,sizeof(nutri_fact4.serving_per));
	for (i=0; i<30; i++) 
	{
		nutri_fact4.nutri_data[i] = varray[i];
	}
/*				for(i=0; i<5; i++)
	{
		nutri_add_fact.Nutri_code = varray2[i];
	}
	nutri_add_fact.Nutri_code = nutri_fact4.code;
*/
	if(insert_direct_nutrition(&nutri_fact4) < 0)
		return 0x97;
	return ret;
}
#endif  // USE_DIRECT_NUTRITION

INT8U commun_recv_table3(HUGEDATA COMM_BUF *CBuf,INT32U spart,INT16U sid,INT8U *str,INT16S leng, INT8U cmd)
/**********************************************************************
 Ingredient,Tracebility,Country,
 **********************************************************************/
{
	ETH_CONNECT_LIST eth;
	STORE_NAME_STRUCT	store;
	CUSTOMER_STRUCT		customer;
	SCROLL_MESSAGE_STRUCT	scroll;
	CLERK_STRUCT		clerk;
	CAPTION_STRUCT 	cap;
	CURRENCY_STRUCT currency;
	SCANNER_CONV_TABLE_STR  scanner;
	INT16S    r,sr,status,i;
	INT16S    part,ssize;
	INT16U id;
	INT8U  code;
	INT32U varray[4];
	INT8U  desc1[33];
	INT8U  desc2[50];
	INT8U  desc3[50];
	INT8U  telep[26];
	INT8U  descL[156];
	INT32U dsize,v32;
	INT8U *save_data;
	INT32U save_addr;
	INT16S    save_size;
	INT8U  effect,daily,ulevel,week,sc_use;
	char *commun_buffer;
	INT8U  bcount;
	INT8U  fsize;
	INT8U  imgid;
	INT8U  ontime;

	memset(desc1,0,33);
	memset(desc2,0,50);
	memset(desc3,0,50);
	memset(telep,0,26);
	memset(descL,0,156);
	memset((INT8U *)&eth,0,sizeof(ETH_CONNECT_LIST));

	part = (INT16S)spart;
	id   = (INT16S)sid;
	if (part<0) return 0x82;
	if (id  <0) return 0x82;
	if (id == 0 && part != 2 && part != 4) return 0x82;	// customer, clerk을 제외하고 id=0는 허용안함    
	if (leng<5) return 0x99;
	ulevel= 3;
	daily = 0;
	effect= 1;
	sc_use = 0;
	status = 0;
	r      = 0;
	week   = 0xff;
	for (i=0; i<4; i++) varray[i]=0;
	while (1) {
		switch (status) {
			case  0:code=str[r];
				dsize=0;
				if (!code) goto END;
				if (!(((code>='a') && (code<='z')) || ((code>='A') && (code<='Z')))) status=20;
				else status=1;
				break;
			case  1:dsize = 0;
				if (str[r]=='=') status=2;
				else status=0;
				break;
			case  2:if ((str[r]=='.') || (str[r]==':')) {
				    if ((code>='P') && (code<='T')) {
					status=10;
				    } else {
				    	switch (code) {
					    case 'u':
					    case 'L': ulevel=(INT8U)dsize; break;
					    case 'E': effect=(INT8U)dsize; break;
					    case 'A': daily =(INT8U)dsize; break;
					    case 'B': sc_use = (INT8U)dsize; break;
					    case 'w': week  =(INT8U)dsize; break;
					    case 'C':
				    	    case 'h': varray[0]=dsize; break;
				    	    case 'j': varray[2]=dsize; break;
				    	    case 'i': varray[1]=dsize; break;
				    	    case 'k': varray[3]=dsize; break;
				    	    case 'o': bcount=dsize; break;
				    	    case 'p': fsize=dsize; break;
				    	    case 'q': imgid=dsize; break;
				    	    case 'r': ontime=dsize; break;
				    	}
				    	status=0;
				    }
				} else {
					dsize<<=4;
					dsize|=ctoh(str[r]);
				}
				break;
			case 10:ssize=(INT16S)dsize;
				sr=r;
				switch (code) {
					case 'P': if (ssize>=31) ssize=31;
						  for (i=0; i<ssize; i++) desc1[i]=str[sr++];
						  desc1[ssize]=0; break;
					case 'Q': if (ssize>=50) ssize=49;
						  for (i=0; i<ssize; i++) desc2[i]=str[sr++];
						  desc2[ssize]=0; break;
					case 'R': if (ssize>=50) ssize=49;
						  for (i=0; i<ssize; i++) desc3[i]=str[sr++];
						  desc3[ssize]=0; break;
					case 'S':
#ifdef USE_RUSSIA_DEFAULT
						  if (ssize>=150) ssize=150;
#else
						  if (ssize>=80) ssize=80;
#endif
						  for (i=0; i<dsize; i++) descL[i]=str[sr++];
						  descL[ssize]=0; break;
					case 'T': if (ssize>=25) ssize=25;
						  for (i=0; i<ssize; i++) telep[i]=str[sr++];
						  telep[ssize]=0; break;
				}
				r+= dsize;
				status=0;
				goto SKIP;
			// For Ethernet Information
			case 20:status=0;
				v32   =0;
				if (str[r]=='=') status=21;
				break;
			case 21:if (str[r]=='.') {
					commun_common_ethernet(code,&eth,v32);
					status=0;
				} else {
					v32<<=4;
					v32|=ctoh(str[r]);
				}
				break;
		}
		r++;
		SKIP:;
	}
END:
	save_addr=0;
	switch (part) {
		case 1: if ((id<1) || (id>MAX_STORE_NO)) return 0x99;
			memset(&store,0,sizeof(store));
			desc1[25]=0;
			telep[19]=0;
#ifdef USE_RUSSIA_DEFAULT
			descL[149]=0;
#else
			descL[79]=0;
#endif
			store.code=id;
			strcpy((char *)store.name,(char *)desc1);
			strcpy((char *)store.text,(char *)descL);
			strcpy((char *)store.tel_no,(char *)telep);
		        save_data   = (INT8U *)&store;
			save_size   = sizeof(store);
			save_addr   = id-1;
			save_addr *= sizeof(store);
			save_addr += FLASH_SHOPNAME_AREA;
			break;
		case 2: if ((id<1) || (id>MAX_CUSTOMER_NO)) return 0x99;
		       memset(&customer,0,sizeof(customer));
		       customer.code=id;
		       desc1[31]=0;
		       desc2[49]=0;
		       desc3[49]=0;
		       descL[49]=0;
		       telep[23]=0;
		       strcpy((char *)customer.name,(char *)desc1);
		       strcpy((char *)customer.address[0],(char *)desc2);
		       strcpy((char *)customer.address[1],(char *)desc3);
		       strcpy((char *)customer.address[2],(char *)descL);
		       strcpy((char *)customer.tel       ,(char *)telep);
		       customer.creditlimit=varray[0];
		       save_data   = (INT8U *)&customer;
		       save_size   = sizeof(customer);
		       save_addr   = id;
		       save_addr *= CUSTOMER_STRUCT_SIZE;
		       save_addr += FLASH_CUSTOMER_AREA;
		       break;
		case 3: if ((id<1) || (id>MAX_SCROLL_NO)) return 0x99;
		       memset(&scroll,0,sizeof(scroll));
		       //descL[85]=0;
		       descL[79] = 0;	// CJK080215
		       strcpy((char *)scroll.text,(char *)descL);
		       scroll.week      = week;
		       scroll.start_year  = (varray[0]>>16)&0xff;
		       scroll.start_month = (varray[0]>>8)&0xff;
		       scroll.start_day   = (varray[0])&0xff;
		       scroll.end_year  = (varray[1]>>16)&0xff;
		       scroll.end_month = (varray[1]>>8)&0xff;
		       scroll.end_day   = (varray[1])&0xff;
		       scroll.start_hour = (varray[2]>>16)&0xff;
		       scroll.start_min  = (varray[2]>>8)&0xff;
		       scroll.start_sec  = (varray[2])&0xff;
		       scroll.end_hour  = (varray[3]>>16)&0xff;
		       scroll.end_min   = (varray[3]>>8)&0xff;
		       scroll.end_sec   = (varray[3])&0xff;
		       scroll.smode     = daily;	// use schedule
		       scroll.stype     = effect;
		       scroll.use       = sc_use;
		       //CJK080215
		       scroll.blink_count = bcount;
		       scroll.font_size   = fsize;
		       scroll.image_id    = imgid;
		       scroll.on_time     = ontime;	// speed
		       ////
		       save_data   = (INT8U *)&scroll;
		       save_size   = sizeof(scroll);
		       save_addr   = id-1;
		       save_addr *= sizeof(scroll);
		       save_addr += FLASH_SCROLL_AREA;
		       break;
		case 4: if ((id<1) || (id>MAX_CLERK_NO-1)) return 0x99;
		       memset(&clerk,0,sizeof(clerk));
		       clerk.code=id;
		       desc1[23]=0;
		       desc2[ 7]=0;
		       desc3[ 7]=0;
		       clerk.code=id;
		       strcpy((char *)clerk.name,(char *)desc1);
		       strcpy((char *)clerk.nickname,(char *)desc2);
		       strcpy((char *)clerk.password,(char *)desc3);
		       clerk.permissionLevel=ulevel;
		       clerk.permissionID=1;

		       save_data   = (INT8U *)&clerk;
		       save_size   = sizeof(clerk);
		       save_addr   = id;
		       save_addr *= CLERK_STRUCT_SIZE;
		       save_addr += FLASH_CLERK_AREA;
		       break;
		case 5:if ((id<1) || (id>MAX_CURRENCY_NO)) return 0x99;
		       currency.rate=varray[0];
		       currency.decimalpoint=varray[1];
		       memcpy(currency.primary,desc1,4); currency.primary[3]=0;
		       memcpy(currency.last,desc2,4); currency.last[3]=0;
		       save_size   = sizeof(currency);
		       save_addr   = id-1;
		       save_addr *= CLERK_STRUCT_SIZE;
		       save_addr += FLASH_CLERK_AREA;
		       break;

		//SCANNER 
		case 6:if ((id<1) || (id>SCAN_BACODE_DATA_MAX_NUM)) return 0x99;
		       memset(&scanner,0,sizeof(scanner));
		       desc1[13]=0; //only EAN13 format (Max 13 char)
		       strcpy((char *)scanner.scanCode,(char *)desc1);
		       scanner.code =id;
		       scanner.dept_no = (INT16U)varray[0];
		       scanner.plu_no = varray[2];
		       save_data   = (INT8U *)&scanner;
		       save_size   = SCANNER_CONV_TABLE_STR_SIZE;
		       save_addr   = id-1;
		       save_addr *= SCANNER_CONV_TABLE_STR_SIZE;
		       save_addr += FLASH_SCANNER_TABLE_AREA;
		       break;
//		case 7:	// SCANNER FORMAT
//		       if ((id<1) || (id>SCAN_BACODE_DATA_MAX_NUM)) return 0x99;
//		       memset(&scanner,0,sizeof(scanner));
//		       desc1[13]=0; //only EAN13 format (Max 13 char)
//		       strcpy((char *)scanner.scanCode,(char *)desc1);
//		       scanner.code =id;
//		       scanner.dept_no = (INT16U)varray[0];
//		       scanner.plu_no = varray[2];
//		       save_data   = (INT8U *)&scanner;
//		       save_size   = SCANNER_CONV_TABLE_STR_SIZE;
//		       save_addr   = id-1;
//		       save_addr *= SCANNER_CONV_TABLE_STR_SIZE;
//		       save_addr += FLASH_SCANNER_FORMAT;
//		       break;
		default:
		       return 0x83;
	}
#ifdef USE_10000_SCANNER_CONVTABLE
	if (part == 6) {
		save_addr+=EXT_MEMCARD2_ADDR_X;
	}
	else {
		if (save_addr==0) return 0x83;
		save_addr+=DFLASH_BASE;
	}
#else
	if (save_addr==0) return 0x83;
	save_addr+=DFLASH_BASE;
#endif
	Flash_swrite(save_addr,(HUGEDATA INT8U *)save_data,save_size);
	commun_recv_monitor('T',(INT16S)part,(INT16S)id);
	if (cmd == 0x73) // master->slave (table3)
	{
		commun_buffer=(char *)serial_send_point;
		r=ethernet_gen_head(commun_buffer);
		sprintf(&commun_buffer[r],"C01F78,%02X%04X",part,id);	// part 3:scroll msg
		r+=strlen(&commun_buffer[r]);
		commun_buffer[r++]=0x0a;
		commun_buffer[r++]=0x0;
		commun_outleng(CBuf,(char *)commun_buffer,r);
		return 0;
	}
	else	// if(cmd == 0x32) // pc->scale(mater)
	{
		if (network_status.service_flag&0x20)	// send plu
		{
			if (ethernet_list.status == 1)	// 전송된 data는 master일 때만 sync flag 설정
			{
				commun_net_mask(3, &id, 0xff, 1);
			}
		}
	}
	return 1;
}

INT8U commun_recv_discount(HUGEDATA COMM_BUF *CBuf,INT16U cmd,INT16S xdcno,INT16S xmode,INT8U *str,INT16S leng)
{
	ETH_CONNECT_LIST eth;
	INT16S r;
	INT8U status,code,mode,v8,ret;
	INT16S dc_no;
	INT32U v32;
	DISCOUNT_STRUCT	 disc_str;
	INT32U addr,plno;
	INT16U dpno,xdc;
	char *commun_buffer;
	
	status = 0;
	r     = 0;
	ret   = 1;
	dc_no    = (INT16U)xdcno;  //ipluno;
	mode     = xmode;	   //ideptno;
	memset(&disc_str,0,sizeof(disc_str));
	while (1) {
		if (r>=leng) break;
		switch (status) {
			case 0: v32=0;
				code=str[r];
				if (code==0) goto END;
				if (!(((code>='a') && (code<='z')) || ((code>='A') && (code<='Z')))) status=20;
				else status=1;
				status=1;
				break;
			case 1: status=0;
				if (str[r]=='=') status=2;
				break;
			case 2: if (str[r]=='.') {
					switch (code) {
						case 'a':disc_str.dept_no=(INT16U)v32; break;
						case 'b':disc_str.plu_no =v32; break;
						case 'c':disc_str.type   =(INT16U)v32; break;
						case 'd':disc_str.target[0]=v32; break;
						case 'e':disc_str.target[1]=v32; break;
						case 'f':disc_str.price[0]=v32; break;
						case 'g':disc_str.price[1]=v32; break;
						case 'h':v8=(INT8U)(0x0000ff&(v32>>16));
							 disc_str.start_year=v8;
							 v8=(INT8U)(0x0000ff&(v32>>8));
							 disc_str.start_month=(INT8U)v8;
							 v8=(INT8U)(0x0000ff&v32);
							 disc_str.start_day=(INT8U)v8;
							 break;
						case 'i':disc_str.start_hour=(INT8U)(v32>>16);
							 disc_str.start_minute=(INT8U)(v32>>8);
							 disc_str.start_second=(INT8U)v32;
							 break;
						case 'j':disc_str.end_year=(INT8U)(v32>>16);
							 disc_str.end_month=(INT8U)(v32>>8);
							 disc_str.end_day=(INT8U)v32;
							 break;
						case 'k':disc_str.end_hour=(INT8U)(v32>>16);
							 disc_str.end_minute=(INT8U)(v32>>8);
							 disc_str.end_second=(INT8U)v32;
							 break;
						case 'l':disc_str.day_of_week=(INT8U)v32;
							 break;
						case 'S':status=(INT8U)v32;
						         break;
						case 'm':
							disc_str.useForFSP = (INT8U)v32; 
							break;
						default :commun_common_ethernet(code,&eth,v32);    break;
					}
					status=0;
				} else {
					v32<<=4;
					v32|=ctoh(str[r]);
				}
				break;
			case 20:status=0;
				v32   =0;
				if (str[r]=='=') status=21;
				break;
			case 21:if (str[r]=='.') {
					commun_common_ethernet(code,&eth,v32);
					status=0;
				} else {
					v32<<=4;
					v32|=ctoh(str[r]);
				}
				break;
		}
		r++;
	}
END:	
	dpno=disc_str.dept_no;
	plno=disc_str.plu_no;
	switch (mode) {
		case 1:	addr = FLASH_DISCOUNT_GLOBAL_WEIGHT; break;
		case 3:	addr = FLASH_DISCOUNT_GLOBAL_COUNT; break;
		case 2:	addr = FLASH_DISCOUNT_GLOBAL_NONFOOD; break;
		default: mode=0;
	}
	if (mode) goto GDC;

	if (dpno<=0) ret=0x99;
	if (dpno>=MAX_DEPT_NO) ret=0x99;
	if (plno<=0) ret=0x99;
	if (plno>MAX_PLU_NUMBER) ret=0x99;
	if (ret==0x99) goto END_DC;
	xdc=_get_dc_number(disc_str.dept_no, disc_str.plu_no);
	if (xdc) dc_no=xdc;
	else dc_no=_get_new_dc_number();
	if (dc_no>=MAX_DISCOUNT_NO) ret=0x98;
	if (dc_no==0) ret=0x97;
	if (!(status_scale.restrict&FUNC_DISCOUNT)) ret=0x97; /* Download error */
	if (ret>0x90) goto END_DC;
GDC:
	addr = get_addr_discount_area(dc_no);
	if (!addr) { /* 에러 처리 */
		ret = 0x99; /* error value */
		goto END_DC;
	}
	Flash_swrite(addr, (HUGEDATA INT8U*)&disc_str,sizeof(disc_str));
	if (!mode)
	{
		if (network_status.service_flag&0x20)	// send plu
		{
			if (ethernet_list.status == 1)	// 전송된 d/c는 master일 때만 sync flag 설정
			{
				//set_nvram_wparam(NVRAM_DC_SYNC_FLAG+2*(dc_no-1), 0xffff);
				commun_net_mask(1, (INT16U*)&dc_no, 0xff, 1);
			}
		}
	}
END_DC:
	if (cmd==0x19 && ret <= 0x90) {
		commun_buffer=(char *)serial_send_point;
		r=ethernet_gen_head(commun_buffer);
		sprintf(&commun_buffer[r],"C01F18,%02X%06lX",disc_str.dept_no,disc_str.plu_no);
		r+=strlen(&commun_buffer[r]);
		commun_buffer[r++]=0x0a;
		commun_buffer[r++]=0x0;
		commun_outleng(CBuf,(char *)commun_buffer,r);
		network_status.load_dc=0;
		ret=0;
	}
	return ret;
}

INT8U commun_recv_other(HUGEDATA COMM_BUF *CBuf,INT32U spart,INT16U sid,INT8U *str,INT16S leng)
{
	INT16S    ret,l,r,status,i;
	INT16S    part;//,erase;
	ETH_CONNECT_LIST eth;
	char   desc1[MAX_TABLE_BUF];
	INT8U  code;
	INT32U dsize,v32;
	INT16U v_arr[10];
	INT8U  v_use,v_ref1,v_ref2;
	INT16U v_wait;
#ifdef USE_CTS_FUNCTION
	INT16U period;
	INT32U v_ref3;
	INT8U  ret_key;
	INT32U hid, addr;
	CAL_HISTORY hist;
#endif

	memset((INT8U *)&eth,0,sizeof(ETH_CONNECT_LIST));
	part = (INT16S)spart;
	if (part<=0) return 0x99;
	if (leng<3 ) return 0x97;

	ret    = 1;
	status = 0;
	memset(desc1,0,MAX_TABLE_BUF);
	if (part==1) {
		ret=commun_setrtc(str,leng);
		goto END;
	}
	else if (part == 0x11) {
		ret=commun_set_business_time(str,leng);
		goto END;
	}
	status = 0;
	r      = 0;
	v_ref1 = 0;
	v_ref2 = 0;
#ifdef USE_CTS_FUNCTION
	v_ref3 = 0;
#endif
	v_use  = 0;
	v_wait = 0;
	while (1) {
		if (r>=leng) break;
		switch (status) {
			case  0:code  =str[r];
				if (!code) goto END;
				if ((code>='A') && (code<='Z')) status=1;
				else if ((code>='a') && (code<='z')) status=1;
				else status=20;
				break;
			case  1:
				dsize = 0;
				l = 0;
				if (str[r]=='=') status=2;
				else status=0;
				break;
			case  2:
				if (str[r]=='.') {
					status=0;
					switch (code) {
					     case 'U': v_use =(INT8U)dsize;  break;
					     case 'R': v_ref1=(INT8U)dsize;  break;
					     case 'S': v_ref2=(INT8U)dsize;  break;
					     case 'P': status=10; break;
					     case 'W': v_wait=(INT16U)dsize; break;
#ifdef USE_CTS_FUNCTION
					     case 'K':
					     case 'B':
								if(l != 8) goto END;	// CTS 키, Board ID 길이 확인용
					     		v_ref3=dsize;  break;	// CTS Key, Board ID
#endif
					     default : if ((code>='a') && (code<='z')) {
								v_arr[code-'a']=(INT16U)dsize;
								break;
						       }
					}
					break;
				} else {
					l++;	// CTS 키, Board ID 길이 확인용
					dsize<<=4;
					dsize|=ctoh(str[r]);
				}
				break;
			case 10:
				l=(INT16S)dsize;
				if (l > MAX_TABLE_BUF) l = MAX_TABLE_BUF;
				for (i=0; i<l; i++) desc1[i]=str[r++];
				desc1[l]=0;
				r += (dsize - l);
				status = 0;
				goto SKIP;
			case 20:status=0;
				v32   =0;
				if (str[r]=='=') status=21;
				break;
			case 21:if (str[r]=='.') {
					commun_common_ethernet(code,&eth,v32);
					status=0;
				} else {
					v32<<=4;
					v32|=ctoh(str[r]);
				}
				break;
		}
		r++;
		SKIP:;
	}
	switch (part) {
	   case 2: if (strlen(desc1)>4) {
	   		ret=0x99;
	   	   } else {
		        set_base_sparam(FLASH_START_PASSWORD, (INT8U*)desc1, DEALER_PASSWORD_LEN);
		        ret=1;
		   }
		   break;
	   case 6: set_global_bparam(GLOBAL_LABEL_PRIORITY,v_use);
		   code =get_global_bparam(GLOBAL_PRINT_ADDUP_FLAG2);
		   code&=0xf3;
		   if (v_ref1) v_ref1=1;
		   if (v_ref2) v_ref2=1;
		   v_ref1<<=2;
		   v_ref2<<=3;
		   code|=v_ref1;
		   code|=v_ref2;
		   set_global_bparam(GLOBAL_PRINT_ADDUP_FLAG2,code);
		   for (i=0; i<3; i++) {
			set_global_wparam(GLOBAL_LABEL_STANDARD+i*2,v_arr[i]);
		   }
		   break;
	   case 7: set_global_bparam(GLOBAL_BARCODE_PRIORITY,v_use);
		   for (i=0; i<9; i++) {
			set_global_bparam(GLOBAL_BARCODE_STD_WID+i,(INT8U)v_arr[i]);
		   }
		   break;
	   case 8: set_global_bparam(GLOBAL_DISCOUNT_PRIORITY,v_use);
	           break;
#ifdef USE_TAX_RATE               
	   case 9: set_global_bparam(GLOBAL_TAX_FLAG,v_use);
		   set_global_bparam(GLOBAL_TAX_VALUE,(INT8U)v_arr[i]);
		   set_global_bparam(GLOBAL_MISC_TAX_VALUE,(INT8U)v_arr[i]);
	           break;
#endif               
	   case 0x10:
		   set_global_wparam(GLOBAL_SCROLL_WAIT_TIME, v_wait*10);
		   Startup.scroll_timeout = (INT16U)(v_wait*10);
		   break;
#ifdef USE_CTS_FUNCTION
	   case 0x13: // encrypted key
		   {
			ret_key = CheckDecryptCalKey(v_ref3, &period);
			if (ret_key != ENCRYPT_NONE && code == 'K')
			{
				switch (ret_key)
				{
					case ENCRYPT_CALENT:
						if (status_run.run_mode == RUN_WAITING_CAL)
						{
							status_run.run_mode = RUN_CALIBRATION;
						}
						break;
					case ENCRYPT_CTSON:
						CtsStruct.CtsEnable = ON;
						CtsStruct.crc16 = crc_16((INT8U*)&CtsStruct, CTS_STRUCT_SIZE - 2);
						Flash_swrite(DFLASH_BASE + FLASH_CTS_STRUCT, (INT8U*)&CtsStruct, CTS_STRUCT_SIZE);
						break;
					case ENCRYPT_CTSOFF:
						CtsStruct.CtsEnable = OFF;
						CtsStruct.crc16 = crc_16((INT8U*)&CtsStruct, CTS_STRUCT_SIZE - 2);
						Flash_swrite(DFLASH_BASE + FLASH_CTS_STRUCT, (INT8U*)&CtsStruct, CTS_STRUCT_SIZE);
						break;
					case ENCRYPT_CALCOMP:
						Flash_sread(DFLASH_BASE + FLASH_CTS_STRUCT, (INT8U*)&CtsStruct, CTS_STRUCT_SIZE);
						RTC_CLK_Burst_Read();
						CtsStruct.CtsCalYear = bcd2hex(RTCStruct.year);
						CtsStruct.CtsCalMonth = bcd2hex(RTCStruct.month);
						CtsStruct.CtsCalDate = bcd2hex(RTCStruct.date);
//						CtsStruct.CtsExpired = OFF;
						CtsStruct.crc16 = crc_16((INT8U*)&CtsStruct, CTS_STRUCT_SIZE - 2);
						Flash_swrite(DFLASH_BASE + FLASH_CTS_STRUCT, (INT8U*)&CtsStruct, CTS_STRUCT_SIZE);

						hid=get_base_wparam(FLASH_CALLOG_COUNT);
						hid&=0x7FFF;		// 0x8000으로 History qty update 되없음을 판단
						addr =(INT32U)hid;
						addr%=HISTORY_QTY;
						addr*=HISTORY_STRUCT_SIZE;
						addr+=DFLASH_BASE;
						addr+=FLASH_CALLOG_DATA;
						Flash_sread(addr,(HUGEDATA INT8U *)&hist,sizeof(hist));

						CtsEncryptBufStruct.span = hist.ad_span;
						CtsEncryptBufStruct.zero = hist.ad_zero;
						CtsEncryptBufStruct.midup = hist.ad_midup;
						CtsEncryptBufStruct.middn = hist.ad_middn;
						CtsEncryptBufStruct.time = CtsStruct.CtsCalYear * 0x1000000;
						CtsEncryptBufStruct.time += CtsStruct.CtsCalMonth * 0x10000;
						CtsEncryptBufStruct.time += CtsStruct.CtsCalDate * 0x100;
//						CtsCalCompReset = ENCRYPT_CALCOMP;
						break;
					case ENCRYPT_PERIOD:
						CtsStruct.CtsPeriod = period;
						CtsStruct.crc16 = crc_16((INT8U*)&CtsStruct, CTS_STRUCT_SIZE - 2);
						Flash_swrite(DFLASH_BASE + FLASH_CTS_STRUCT, (INT8U*)&CtsStruct, CTS_STRUCT_SIZE);
						break;
				}
			}
			else
			{
				ret = 0x96;
			}
		   }
		   break;
	   case 0x14: // write serial no 
		if (code == 'P')
		{
			memcpy(CtsStruct.SerialNo, desc1, 16);
			CtsStruct.crc16 = crc_16((INT8U*)&CtsStruct, CTS_STRUCT_SIZE - 2);
			Flash_swrite(DFLASH_BASE + FLASH_CTS_STRUCT, (INT8U*)&CtsStruct, CTS_STRUCT_SIZE);
		}
		else ret = 0x96;
		break;
	   case 0x15:	// Write Board ID
		if (code == 'B')
		{
			CtsStruct2.BoardId = v_ref3;
			CtsStruct2.crc16 = crc_16((INT8U*)&CtsStruct2, CTS_STRUCT_SIZE2 - 2);
			Flash_swrite(DFLASH_BASE + FLASH_CTS_STRUCT2, (INT8U *)&CtsStruct2, CTS_STRUCT_SIZE2);
		}
		else ret = 0x96;
		break;
	   case 0x16:	// Write Encrypt Key
		if (code == 'K')
		{
			CtsStruct2.EncryptKey1 = v_ref3;
			CtsStruct2.crc16 = crc_16((INT8U*)&CtsStruct2, CTS_STRUCT_SIZE2 - 2);
			Flash_swrite(DFLASH_BASE + FLASH_CTS_STRUCT2, (INT8U *)&CtsStruct2, CTS_STRUCT_SIZE2);
		}
		else ret = 0x96;
		break;
	   case 0x17:	// Write Product Code
		if (code == 'P')
		{
			memcpy(CtsStruct2.ProductCode, desc1, 16);
			CtsStruct2.crc16 = crc_16((INT8U*)&CtsStruct2, CTS_STRUCT_SIZE2 - 2);
			Flash_swrite(DFLASH_BASE + FLASH_CTS_STRUCT2, (INT8U *)&CtsStruct2, CTS_STRUCT_SIZE2);
		}
		else ret = 0x96;
		break;
#endif
	   default: ret=0x99;
	}
END:
	return ret;
}

#define DF_FILE_BLOCK_SIZE 512
#ifdef USE_REMOTE_FW_DOWNLOAD
INT8U commun_recv_firmware(HUGEDATA COMM_BUF *CBuf, INT32U first,INT16U second,INT8U *str,INT16S leng)
{
	//INT16S r;
    char  send_buffer[10];
    char dspMsgBuf[32];
    INT32U start_addr, recv_id;
    INT16U blockNumber;
    INT16U recvBcc = 0;
    static INT16U addBcc = 0;
    INT8U ret;
    INT8U mode;
    int i;
	POINT disp_p;    
//    int j;
//INT8U temp8u[2];
    
    ret = 1;
	recv_id = (INT16U)first;
    
    if (leng > DF_FILE_BLOCK_SIZE) {
        sprintf(send_buffer, "W99:E82"); //block length error
        commun_outleng(CBuf, send_buffer, strlen(send_buffer));  
        ret = 0;
        return ret;        
    }
    
    if (BootVer == 0) {
        sprintf(send_buffer, "W99:E98"); //Unable to verify boot version.
        commun_outleng(CBuf, send_buffer, strlen(send_buffer));
        ret = 0;
        return ret;
    }
    
    if (network_status.remote_control != 1)  {
        sprintf(send_buffer, "W99:E99"); //Disable remote function
        commun_outleng(CBuf, send_buffer, strlen(send_buffer));
        ret = 0;
        return ret;
    }
    
    if (Flash1ExtFlag != 1)  {// flash 1 is not 8M chip
        sprintf(send_buffer, "W99:E84"); 
        commun_outleng(CBuf, send_buffer, strlen(send_buffer));
        ret = 0;
        return ret;
    }
    
    switch (recv_id)
    {
        case 0://recv f/w data
            blockNumber = second - 1; // offset 은 0 부터 시작
            if (blockNumber == 0) {//if first block
                addBcc = 0;
                UpdateFwPercnet = 0;
                status_run.run_mode = RUN_FW_UPDATE;
            }
    
            start_addr = FLASH_FIRMWARE_CS1_AREA + (blockNumber * DF_FILE_BLOCK_SIZE); 
            
            if (blockNumber * DF_FILE_BLOCK_SIZE + leng > FLASH_FIRMWARE_SIZE) {
                sprintf(send_buffer, "W99:E83"); //out of range 2 MByte 초과
                commun_outleng(CBuf, send_buffer, strlen(send_buffer));                 
                ret = 0;
                return ret;
            }
            Flash_swrite(start_addr, str, leng);
            for (i = 0; i < leng; i++) {
                addBcc += str[i];
            }
//sprintf(MsgBuf, "<DEBUG>bk=%ld, ln=%ld, tlen=%ld, chk=%04x\r\n", blockNumber, leng, blockNumber * DF_FILE_BLOCK_SIZE + leng, addBcc);
//MsgOut(MsgBuf);             
            //commun_buffer=(char *)serial_send_point;
            //r = ethernet_gen_head(commun_buffer);
            sprintf(send_buffer, "W99:O00");
            commun_outleng(CBuf, send_buffer, strlen(send_buffer));            
            UpdateFwTimer = SysTimer100ms;
            break;
        
        case 1://recv checksum
            start_addr = FLASH_FIRMWARE_CHKSUM_CS1_AREA;

            recvBcc = str[0];
            recvBcc <<= 8;
            recvBcc |= str[1];
            if (addBcc == recvBcc) {
                Flash_wwrite(start_addr, recvBcc);
                
				mode = Flash_bread(DFLASH_BASE + FLASH_FWMODE_SET);
				mode |= 0x08; //Turn on SFlash Update in Boot
				Flash_bwrite(DFLASH_BASE + FLASH_FWMODE_SET, mode);
                
                sprintf(send_buffer, "W99:O01");
                commun_outleng(CBuf, send_buffer, strlen(send_buffer));                
            }  else {
                mode = Flash_bread(DFLASH_BASE + FLASH_FWMODE_SET);
				mode &= (~0x08); //Turn off SFlash Update in Boot
				Flash_bwrite(DFLASH_BASE + FLASH_FWMODE_SET, mode);  
                
                sprintf(send_buffer, "W99:E81"); //checksum err
                commun_outleng(CBuf, send_buffer, strlen(send_buffer));  
                ret = 0;
                return ret;
            }
            break;
        case 2://reboot program
            if (str[0] == '.') {
                disp_p = point(0,0);
                sprintf(dspMsgBuf, "UPDATING FIRMWARE...");
                Dsp_Write_String(disp_p, (HUGEDATA char *)dspMsgBuf);
                Dsp_Diffuse();
                
                sprintf(send_buffer, "W99:O02");
                commun_outleng(CBuf, send_buffer, strlen(send_buffer)); 
                Delay_10ms_w_TcpIp(20);
                _SOFTWARE_RESET;
            }
            break;
        case 3://check update firmware complete
            if (str[0] == 0x01) {
                mode = Flash_bread(DFLASH_BASE + FLASH_FWMODE_SET);
                if (!(mode&0x08)) {
                    sprintf(send_buffer, "W99:O03");
                    commun_outleng(CBuf, send_buffer, strlen(send_buffer)); 
                }
            }
            break;            
////////////////debug/////////////////////////////////////////
//        case 4://Flash1 clear test
//MsgOut("ChksumDFLASH-1\r\n");   
//    for (i = 0x400000; i < 0x800000; i++)
//    {
//        SQI_Flash_Read(SQI_CS_1, i, &temp8u[0], 1);
//        recvBcc ^= temp8u[0];
//    }
//sprintf(MsgBuf, "<FlashDChip-1>Chksum=%04X\r\n", recvBcc);
//recvBcc = 0;
//MsgOut(MsgBuf);             
//WDT_Timer_Disable();
//WDT_Timer_Clear();            
//MsgOut("EraseNVRAM\r\n");  
//FlashEraseNVRAMArea();
//WDT_Timer_Enable();
//
//MsgOut("ChksumDFLASH-2\r\n");   
//    for (i = 0x400000; i < 0x800000; i++)
//    {
//        SQI_Flash_Read(SQI_CS_1, i, &temp8u[0], 1);
//        recvBcc ^= temp8u[0];
//    }
//sprintf(MsgBuf, "<FlashDChip-2>Chksum=%04X\r\n", recvBcc);
//MsgOut(MsgBuf); 
//            break;
////////////////debug/////////////////////////////////////////    
    }
    UpdateFwPercnet = (int)(((double)(DF_FILE_BLOCK_SIZE * blockNumber)) / (double)0x1ffff0 * 100);
    return ret;
}
#endif

#ifdef USE_REMOTE_ROM_UPDATE
INT8U commun_recv_settingrom(HUGEDATA COMM_BUF *CBuf, INT32U first,INT16U second,INT8U *str,INT16S leng)
{
	//INT16S r;
    char  send_buffer[10];
    //char dspMsgBuf[32];
    INT32U start_addr, recv_id;
    INT16U blockNumber;
    INT16U recvBcc = 0;
    static INT16U addBcc = 0;
    INT8U ret;
    INT8U code;
    INT8U temp8u;
    INT8U strVal[12];
    INT32U i;
    INT8U r;
    INT8U leng8u;
    //MASTER_MEMORY rom_header;
    
    ret = 1;
	recv_id = (INT16U)first;
    
    if (leng > DF_FILE_BLOCK_SIZE) {
        sprintf(send_buffer, "W98:E82"); //block length error
        commun_outleng(CBuf, send_buffer, strlen(send_buffer));  
        ret = 0;
        return ret;        
    }
    
    if (network_status.remote_control != 1)  {
        sprintf(send_buffer, "W98:E99"); //Disable remote function
        commun_outleng(CBuf, send_buffer, strlen(send_buffer));
        ret = 0;
        return ret;
    }
    
    if (Flash1ExtFlag != 1)  {// flash 1 is not 8M chip
        sprintf(send_buffer, "W98:E84"); 
        commun_outleng(CBuf, send_buffer, strlen(send_buffer));
        ret = 0;
        return ret;
    }
    
    switch (recv_id)
    {
        case 0://recv setting rom data
            blockNumber = second - 1; // offset 은 0 부터 시작
            if (blockNumber == 0) {//if first block
                addBcc = 0;
                UpdateFwPercnet = 50;
                status_run.run_mode = RUN_FW_UPDATE;
            }
            start_addr = FLASH_FIRMWARE_CS1_AREA + (DATAROM_INI_NO + 1) + (blockNumber * DF_FILE_BLOCK_SIZE); 
//sprintf(MsgBuf, "<DEBUG>READ FLASH VERSION:%d\r\n", start_addr);
//MsgOut(MsgBuf);                  
            if (blockNumber * DF_FILE_BLOCK_SIZE + leng > FLASH_FIRMWARE_SIZE) {
                sprintf(send_buffer, "W98:E83"); //out of range 2 MByte 초과
                commun_outleng(CBuf, send_buffer, strlen(send_buffer));                 
                ret = 0;
                return ret;
            }
            Flash_swrite(start_addr, str, leng);
            //Flash_flush_nvbuffer();
         
            for (i = 0; i < leng; i++) {
                addBcc += str[i];
            }
            sprintf(send_buffer, "W98:O00");
            commun_outleng(CBuf, send_buffer, strlen(send_buffer));            
//sprintf(MsgBuf, "<DEBUG>bk=%ld, ln=%ld, tlen=%ld \r\n", blockNumber, leng, blockNumber * DF_FILE_BLOCK_SIZE + leng);
//MsgOut(MsgBuf);              
            UpdateFwTimer = SysTimer100ms;
            break;
        
        case 1://recv checksum
            start_addr = FLASH_FIRMWARE_CHKSUM_CS1_AREA;

            recvBcc = str[0];
            recvBcc <<= 8;
            recvBcc |= str[1];
            if (addBcc == recvBcc) {
                Flash_wwrite(start_addr, recvBcc);
                
                sprintf(send_buffer, "W98:O01");
                commun_outleng(CBuf, send_buffer, strlen(send_buffer)); 
                UpdateFwPercnet = 90;
            }  else {
                sprintf(send_buffer, "W98:E81"); //checksum err
                commun_outleng(CBuf, send_buffer, strlen(send_buffer));  
                ret = 0;
                return ret;
            }
            break;
            
        case 2://data information to update
            i = 0;
            r = 0;
            leng8u = strlen(str);
            UpdateFwPercnet = 100;
            while (1) {
        		if (i >= leng8u) break;
                code = str[i]; 
                //code '0': parameter
                //code '1': symbol
                //code '2': caption
                //code '3': origin
                //code '4': keypad
                //code '5': font
                //code '6': label
                //code '7': DATAROMUPDATE_MEMCLR
                //code '8':Update Scale Type
                //code '9':DATAROMUPDATE_FWUPDATE
                //code 'A':DATAROMUPDATE_CONFIRM
                //code 'B':Scale type (Pole)
                if (str[++i] != '=') break;
                if (code == 'B') {
                    temp8u = str[++i] - '0';
                    strVal[r] = temp8u;
                } else {
                    strVal[r] = str[++i];
                }
                i++;
                r++;
            }
            start_addr = FLASH_FIRMWARE_CS1_AREA; 

            Flash_swrite(start_addr, strVal, sizeof(strVal));
    
            sprintf(send_buffer, "W98:O02");
            commun_outleng(CBuf, send_buffer, strlen(send_buffer));  
			SaveRemoteUpdateFlag(REMOTE_UPDATE_ROM);
            break;
            
        case 3://check update settingRom 
            if (str[0] == 0x01) {
                if (RemoteFwUpdateFlag == REMOTE_UPDATE_NONE) {
                    //MsgOut("chk update setting rom2\r\n"); 
                    sprintf(send_buffer, "W98:O03");
                    commun_outleng(CBuf, send_buffer, strlen(send_buffer)); 
                } else {
                    //MsgOut("chk update setting rom err\r\n"); 
                    sprintf(send_buffer, "W98:E03");
                    commun_outleng(CBuf, send_buffer, strlen(send_buffer));                     
                }
            }
            break;            
    }
    return ret;    
}
#endif

#ifdef USE_INDIA_FUNCTION
INT8U commun_recv_inventory(HUGEDATA COMM_BUF *CBuf,INT32U spart,INT32U sid,INT8U *str,INT16S leng)
{
	INT16S	  ret,l,r,status,i;
	INT16S	  part;//,erase;
	INT16U id;
	ETH_CONNECT_LIST eth;
	INT8U  code;
	INT32U dsize,v32;
	INT8U  deptno;
	INT32U pluno;
	INT32U inventory;
	INT32U bagWt;
	INT16U idx;
	INT8U  sync = OFF;

	memset((INT8U *)&eth,0,sizeof(ETH_CONNECT_LIST));
	part = (INT16S)spart;
	deptno   = (INT8U)(sid/0x1000000);
	pluno   = (INT32U)(sid%0x1000000);
	if (Startup.disable_dept) deptno = 1;
	if (pluno ==0 || pluno > MAX_PLU_NUMBER) return 0x96;
	if (deptno == 0 || deptno > MAX_DEPT_NO) return 0x96;
	if (part == 0 || part > 2) return 0x99;
	if (leng <3) return 0x97;

	if (!plu_table_search(deptno,pluno,(INT16U *)&idx,0)) {
		return 0x98; // PLU 없음
	}

	ret    = 1; //정상
	status = 0;
	r      = 0;
	inventory  = 0;
	bagWt = 0;
	if (!ReadPluInventoryBagWt(deptno, pluno, &inventory, &bagWt))
	{
		inventory  = 0;
		bagWt = 0;
	}
	while (1) {
		if (r >= leng) break;
		switch (status) {
			case  0:code = str[r];
				if (!code) goto END;
				if ((code>='A') && (code<='Z')) status=1;
				else if ((code>='a') && (code<='z')) status=1;
				else status=20;
				break;
			case  1:
				dsize = 0;
				if (str[r]=='=') status=2;
				else status=0;
				break;
			case  2:if (str[r]=='.') {
					status=0;
					switch (code) {
					     case 'A': inventory = dsize;  break;
					     case 'B': bagWt = dsize;  break;
					}
					break;
				} else {
					dsize<<=4;
					dsize|=ctoh(str[r]);
				}
				break;
			case 20:status=0;
				v32   =0;
				if (str[r]=='=') status=21;
				break;
			case 21:if (str[r]=='.') {
					commun_common_ethernet(code,&eth,v32);
					status=0;
				} else {
					v32<<=4;
					v32|=ctoh(str[r]);
				}
				break;
		}
		r++;
		SKIP:;
	}
	switch (part) {
	   case 1: 
	   	sync = WritePluInventoryBagWt(deptno, pluno, inventory, bagWt);
		break;
	   case 2: 
	   	sync = AddPluInventoryBagWt(deptno, pluno, inventory, bagWt);
	   	break;
	   default: ret=0x99;
	}
	if (network_status.service_flag&0x20)	// send plu
	{
		if (ethernet_list.status == 1 && sync == ON)
		{
			commun_net_mask(0, (INT16U *)&idx, 0xff, 1);
		}
	}

END:
	return ret;
}
#endif

#ifdef USE_DIRECT_INGREDIENT
#ifdef USE_INGREDIENT_98_PROTOCOL
INT8U commun_recv_directingredient(HUGEDATA COMM_BUF *CBuf,INT32U ipluno,INT16U ideptno,INT8U *str,INT16S leng)
{
	INT16S	  ret,l,r,status,i,com_type;
 	ETH_CONNECT_LIST eth;
	INT8U  code;
	INT32U dsize;
	INT8U  deptno;
	INT32U pluno,v32,key,xpos_base;
	register INT32U xpos;
	INT16U usize,idx;
	INT16U total_length;
	INT16U current_block_number;
	INT16U currnet_ingredient_size;
	char  *commun_buffer;
	INT16U ingred_no[DIRECT_INGREDIENT_BLOCK_NUM];
	INT16U ingred_tempno[DIRECT_INGREDIENT_BLOCK_NUM];	
	INT8U ing_ret;
	
	memset((INT8U *)&eth,0,sizeof(ETH_CONNECT_LIST));
	deptno = (INT8U)ideptno;
	pluno  = (INT32U)ipluno;
	com_type=serial_get_comtype(CBuf);
	commun_buffer=(char *)serial_send_point;

	if (Startup.disable_dept) deptno = 1;
	if (pluno <=0) return 0x82;
	if (leng <0  ) return 0x84;
	if (deptno<=0) pluno=MAX_PLU_NUMBER+1;	 
	ret = 0x82; // plu no mismatch
	if (pluno>MAX_PLU_NUMBER) goto END_R;
	
	if (!plu_table_search(deptno,pluno,(INT16U *)&idx,0)) {
		return 0x98; // PLU 없음
	}
	key = (INT32U)idx;
	ret    = 1; //정상
	status = 0;
	r      = 0;
	xpos_base= plu_calc_addr_by_key(key-1);
	while (1) {
		if (r >= leng) break;
		switch (status) {
			case  0:
				code = str[r];
				if (!code) 	return ret;
				if ((code>='A') && (code<='Z')) status=1;
				else if ((code>='a') && (code<='z')) status=1;
				else status=20;
				break;
			case  1:
				dsize = 0;
				if (str[r]=='=') status=2;
				else status=0;
				break;
			case  2:
				if (str[r]=='.') 
				{
					switch (code) {
					     case 'X': 
						 	current_block_number = dsize;  
							status=0;							
							break;
					     case 'A': 
						 	total_length = dsize;  
							status=0;
						 	break;
					     case 'D':
						 	currnet_ingredient_size = dsize;
							status=10;
							break;
					}
					break;
				}
				else
				{
					dsize<<=4;
					dsize|=ctoh(str[r]);
				}
				break;
			case 10:
				xpos = xpos_base + plu_get_ptype_point(PTYPE_INGREDIENT_TEXT,NULL,(INT16U *)&usize);
				Flash_sread(xpos, (HUGEDATA INT8U *)&ingred_no[0], 2*DIRECT_INGREDIENT_BLOCK_NUM);
				memcpy((INT8U *)&ingred_tempno[0], (INT8U *)&ingred_no[0], 2*DIRECT_INGREDIENT_BLOCK_NUM);
				ing_ret = ingredient_text_write(&ingred_no[0], (HUGEDATA INT8U*)&str[r], current_block_number*CURRNET_MAX_INGREDIENT_SIZE , currnet_ingredient_size, 1);
				ret = current_block_number+1;
				v32 = (current_block_number+1) * (CURRNET_MAX_INGREDIENT_SIZE/(DEFAULT_INGREDIENT_TEXT_SIZE-2));
				if(v32<DIRECT_INGREDIENT_BLOCK_NUM)	memcpy((INT8U *)&ingred_no[v32], (INT8U *)&ingred_tempno[v32], 2*(DIRECT_INGREDIENT_BLOCK_NUM-v32));
 				Flash_swrite(xpos, (HUGEDATA INT8U *)&ingred_no[0], 2*DIRECT_INGREDIENT_BLOCK_NUM);				
				if (!ing_ret) return 0x88;	// direct ingredient momory full
				r += currnet_ingredient_size;
				status=0;
			 	break;
			case 20:
				status=0;
				v32   =0;
 				break;
		}
		r++;
		SKIP:;
	}
END_R:
	plu_table_save((INT8U)deptno,pluno);
	if(total_length == current_block_number * CURRNET_MAX_INGREDIENT_SIZE + currnet_ingredient_size)
	{
		network_status.load_plu=1;		
	}
	return ret;
}
#endif
#endif

void  commun_recv_monitor(char key,INT16S xtype,INT16S v)
{
	char temp[16];
	if (status_run.run_mode==RUN_SALE) {
		sprintf(temp,"%c%02d|%04d",key,xtype,v);
	} else {
		sprintf(temp,"%c%05d",key,v);
	}
	if (status_run.run_mode==RUN_SALE) {
	} else {
		display_string(2,(INT8U *)temp);
	}
}

#ifdef USE_CHN_EMART_BIRDS_TRACE //20140828
//#define MAX_QR_DATA_BUF	750 
INT8U commun_recv_qr(HUGEDATA COMM_BUF *CBuf,INT32U pluno,INT16U deptno,INT8U *str,INT16S leng)
{
	INT16U key;

   	if (leng>CHN_EMART_TRACE_QR_SIZE) return 0x96;

	if (!plu_table_search(deptno,pluno,(INT16U *)&key,0)) 	return 0x99;
	chn_emart_qr_trace_save(key-1,str);

	return 1;
}
#endif
