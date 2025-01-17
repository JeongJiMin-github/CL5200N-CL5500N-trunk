#include <stdio.h>
#include <string.h>
#include "globals.h"

#include "commbuf.h"
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
#include "prt_common.h"
#include "prt_interpreter.h"
#include "dsp_vfd7.h"

#include "adm.h"
#include "key.h"
#include "key_typedef.h"
#include "main.h"
#include "cal.h"
#include "debug.h"
#include "menu.h"
#include "initial.h"
#include "common.h"
#include "mode_main.h"
#include "mode_error.h"
#include "plu_device.h"
#include "plu_manager.h"
#include "flash_app.h"
#include "caption_app.h"
#include "key_app.h"
#include "dsp_app.h"
#include "ingredient.h"
#include "discount.h"
#include "plu_auxcall.h"
#include "pgm_app.h"
#include "adm_app.h"
#include "department.h"
#include "mode_sale.h"
#include "ethernet.h"
#include "ethernet_comm.h"
#include "network_command.h"

// #define PLU_PAGEVIEW_LINE_PER_DISPLAY 3
extern  PLU_GLOBAL_INFO_STRUCT plu_global[128];
extern  INT32U FLASH_PLU_STRUCT_PNT;

// extern  INT8U plu_global_inhibit_point[128];
// #define MULTIPLE_UNIT_FLAG  1  // 0 = use Fixed UnitWeight    1= Use unitweight
extern INT16S    		plu_record_size;
extern INT16U 		plu_n_field;
extern INT8U	OnOff(INT8U	tmp);
extern INT8U   plu_base_change;
extern ETHERNET_STATE ethernet_list;
extern void  commun_out(HUGEDATA COMM_BUF *CBuf,char *str);
extern void  commun_outleng(HUGEDATA COMM_BUF *CBuf,char *buf,INT16S leng);
extern void  network_common_trans(void);
extern INT32U ing_calc_addr_by_key(INT16U ingid);

INT8U client_recv_mode;
extern INT32U  MAX_PLU_NO,MAX_PLU_large,MAX_PLU_small;
#ifdef USE_EXTEND_8M_DFLASH
extern INT32U MAX_PLU_extend8M;
#endif
extern char  gmsg[];

#ifdef USE_INDIA_FUNCTION
INT8U UseItemCode13Digit = ON;
#endif

INT8U NvramTempBuffer[3][2500];

INT16S  plu_get_recordsize(void)
{
	INT16S maxid,rsize,i;
	INT32U pnt;
	INT16U usize;

	maxid = plu_struct_get_nfield();
	pnt  = FLASH_PLU_STRUCT_PNT;

	if ((maxid>512) || (maxid<0)) {
		return 0;
	}
	rsize =3;
	for (i=0; i<maxid; i++) {
	   	usize = Flash_wread(pnt+PLUSTR_USIZE_POINT);
		rsize += usize;
		pnt += PLU_STRUCT_SIZE;
	}
#if defined(USE_GSMART_PLU) || defined(USE_RUSSIA_LOGISTICS_PLU) 	// RUS나 내수 일반의 경우 현재 적용안함. (Default와 실제 사이지가 틀림 -> 저장간격이 실제사이즈 간격으로 되어있음)
	if (rsize < MAX_PLU_RECORDSIZE_DEFAULT) rsize = MAX_PLU_RECORDSIZE_DEFAULT;	// 정의된 Size로 변경
#endif
	return rsize;
}

INT32U plu_get_nth_point(INT16U nth,INT16U *xsize)
{
	INT32U pnt;

	pnt=0;
	*xsize=0;
	if (nth<128) {
		*xsize = plu_global[nth].usize;
		pnt    = (INT32U)plu_global[nth].ptype_point;
		if (plu_global[nth].inhibit) *xsize=0;
	}
	return pnt;
/*	INT16S maxid,rsize,i;
	INT32U pnt;
	INT16U usize,ptype;

	pnt   = DFLASH_BASE;
	pnt  += FLASH_PLU_STRUCT_AREA;
	pnt  += PLU_STRUCT_FIELDNO;
	maxid = Flash_wread(pnt);
	*xsize=0;
	if (nth>=maxid) return -1;
	pnt  = DFLASH_BASE;
	pnt += FLASH_PLU_STRUCT_AREA;
	pnt += PLU_STRUCT_POINT;
	rsize =1;   // First Character = PLU Exist Flag
	*xsize=0;
	for (i=0; i<=nth; i++) {
		ptype = Flash_wread(pnt+PLUSTR_PTYPE_POINT);
	   	usize = Flash_wread(pnt+PLUSTR_USIZE_POINT);
		*xsize = usize;
		if (ptype&0x8000) *xsize=0;
		if (i!=nth) {
			rsize += usize;
		}
		pnt += PLU_STRUCT_SIZE;
	}
	return rsize;
*/
}

INT16S plu_get_ptype_str_point(INT16U cur_ptype)
/***************************************************
 해당 item type 의 구조가 저장된 위치를 얻는다.
 ***************************************************/
{
	INT16S    	maxid, i;
	INT16U 	ptype,ttype;
	INT32U 	pnt;

	maxid = plu_n_field;
//	pnt   = DFLASH_BASE;
//	pnt  += FLASH_PLU_STRUCT_AREA;
//	pnt  += PLU_STRUCT_POINT;
	pnt   = FLASH_PLU_STRUCT_PNT;

	for (i=0; i<maxid; i++) {
		ptype  = Flash_wread(pnt+PLUSTR_PTYPE_POINT);
		ttype = ptype&0x7fff;
		if (ttype==cur_ptype) {
			return i+1;
		}
		pnt += PLU_STRUCT_SIZE;
	}
//EXT:
	return 0;
}

INT32U plu_struct_addr_byid(INT16U id)
{
	INT32U addr;

	addr=(long)plu_get_ptype_str_point(id);
	if (addr==0) return 0;
	addr--;
	addr*=PLU_STRUCT_SIZE;
	addr =FLASH_PLU_STRUCT_PNT;
//	addr+=FLASH_PLU_STRUCT_AREA;
//	addr+=PLU_STRUCT_POINT;
//	addr+=DFLASH_BASE;
	return addr;
}

INT32U plu_struct_addr_nameid(INT16U id,INT32U *xaddr)
{
	INT32U addr;

	addr=(long)plu_get_ptype_str_point(id);
	if (addr==0) return 0;
	addr--;
	(*xaddr)=addr;
	(*xaddr)*=PLU_STRUCT_SIZE;
//	(*xaddr)+=FLASH_PLU_STRUCT_AREA;
//	(*xaddr)+=PLU_STRUCT_POINT;
//	(*xaddr)+=DFLASH_BASE;
	(*xaddr)+=FLASH_PLU_STRUCT_PNT;

	addr*=PLUSTR_NAME_SIZE;
	addr+=FLASH_PLU_STRUCT_CAPTION_AREA;
	addr+=DFLASH_BASE;
	return addr;
}

//void plu_id_set(INT16S key)
//{
//	INT32U pnt;
//	INT8U  plu_status;
//
//	pnt = DFLASH_BASE;
//	pnt+= PLU_DATATABLE_POINT;
//	pnt+= key;
//	plu_status=Flash_bread(pnt);
//	if (plu_status==0xff) plu_status=0;
//	plu_status|=0x01;
//	Flash_bwrite(pnt,plu_status);
//}
extern INT16S plu_get_ptype_point2(INT16U cur_ptype,INT8U *stype,INT16U *xsize);

void plu_get_field(INT16U id,INT16U ptype,INT8U *buffer,INT16S *usize,INT16S sz)
{
	INT32U pnt;
	INT16S    pos;
//	pnt = (INT32U)id;
//	if (id>=EXT_PLU_NO) {
//		pnt-= EXT_PLU_NO;
//		pnt*= plu_record_size;
//		pnt+= EXT_MEMCARD_ADDR_X;
//	} else {
//	        pnt*= plu_record_size;
//		pnt+= FLASH_PLU_AREA_PNT;
//	}
	pnt = plu_calc_addr_by_key((long)id);
	pos   =0;
	*usize=0;
	if (ptype==55) {	//cjk
		if (!(status_scale.restrict&FUNC_ORG_TBL)) goto NEXT;
	}
	//pos = plu_get_ptype_point2(ptype,NULL,(INT16U *)usize);
	if(ptype==99)
		pos = plu_get_ptype_point3(ptype,NULL,(INT16U *)usize);
	else
		pos = plu_get_ptype_point(ptype,NULL,(INT16U *)usize);

NEXT:
//	if (pos < 0) {
//		if (*usize) memset(buffer,0,*usize);
//		else        memset(buffer,0,sz);
//	} else {
	if (!*usize) {
		memset(buffer, 0, sz);
	} else {
		if (*usize > sz) *usize = sz;	// CJK071213
		if (plu_base_change) { // Case PLU Edit
			memcpy(buffer, NvramTempBuffer[0]+pos, *usize);
		} else {
			pnt+=pos;
			Flash_sread(pnt,(HUGEDATA INT8U*)buffer,*usize);
		}
	}
}

INT32U plu_get_field_point(INT16S id,INT16U ptype,INT16S *usize)
{
	INT32U pnt;
	INT16S    pos;
//	char  str[128];

	if (plu_base_change) {
		return 0;
	} else {
		pnt = plu_calc_addr_by_key((long)id);
//		pnt = (INT32U)id;
//		if (id>=EXT_PLU_NO) {
//			pnt-= EXT_PLU_NO;
//			pnt*= plu_record_size;
//			pnt+= EXT_MEMCARD_ADDR_X;
//		} else {
//			pnt*= plu_record_size;
//			pnt+= FLASH_PLU_AREA_PNT;
//		}
	}
	pos = plu_get_ptype_point(ptype,NULL,(INT16U *)usize);	// usize가 0일면 return 0
	if (*usize == 0) {
		return 0L;
	}
	pnt=pnt+pos;
	return pnt;
}

INT32U plu_get_field_point_nvram(INT16S id,INT16U ptype,INT16S *usize)
{
	INT32U pnt;
	INT16S    pos;
	INT8U stype;

	if (plu_base_change) {
		pnt = 0; // return 후에 계산함
	} else {
		return 0;
	}
	pos = plu_get_ptype_point(ptype,&stype,(INT16U *)usize);	// usize가 0일면 return 0
	if (*usize == 0) {
		return 0L;
	}
	pnt=pnt+pos;
	return pnt;
}

//void plu_set_update(INT16S id)
//{
//	INT32U pnt;
//	INT16S    pos;
//
//	pnt = id;
//	if (id>=EXT_PLU_NO) {
//		pnt-= EXT_PLU_NO;
//		pnt*= plu_record_size;
//		pnt+= EXT_MEMCARD_ADDR_X;
//	} else {
//		pnt*= plu_record_size;
//		pnt+= FLASH_PLU_AREA_PNT;
//	}
//	pos = 1;
//	Flash_wwrite(pnt+pos,0xff);
//	commun_plumask(key+1,0xff,3);
//}

INT8U plu_set_field(INT16S id,INT16U ptype,INT8U *buffer)
{
	INT32U pnt;
	INT16U usize;
	INT16S    pos,sz;
	INT8U  stype;
#ifdef USE_PROTOCOL_CHINA_DAHUA
	INT32U *v_32;
	INT16U *v_16;
	INT8U  *v_8;
#endif
//	pnt=id;
//	if (id>=EXT_PLU_NO) {
//		pnt-= EXT_PLU_NO;
//		pnt*= plu_record_size;
//		pnt+= EXT_MEMCARD_ADDR_X;
//	} else {
//		pnt*= plu_record_size;
//		pnt+= FLASH_PLU_AREA_PNT;
//	}
	pnt = plu_calc_addr_by_key((long)id);
	pos = plu_get_ptype_point(ptype,&stype,&usize);	// usize가 0이면 처리 안하고 return false
	sz = usize;
	if (usize) {
#ifdef USE_PROTOCOL_CHINA_DAHUA
		switch(stype)
		{
			case 'S':
				sz=strlen((char *)buffer)+1;				
				Flash_swrite(pnt+pos,(HUGEDATA INT8U*)buffer,sz);
				break;
			case 'L':
			 	v_32=(INT32U*)buffer;
				Flash_lwrite(pnt+pos,*v_32);
				break;
			case 'W':
			 	v_16=(INT16U*)buffer;
				Flash_wwrite(pnt+pos,*v_16);
				break;

			case 'B':
				Flash_bwrite(pnt+pos,*buffer);
				break;
			default :
				Flash_swrite(pnt+pos,(HUGEDATA INT8U*)buffer,sz);
				break;
		}
#else
		if (stype=='S') {
			sz=strlen((char *)buffer)+1;
		}
		Flash_swrite(pnt+pos,(HUGEDATA INT8U*)buffer,sz);
#endif
		return TRUE;
	}
	return FALSE;
}

void plu_save_deptid(INT32U pnt,INT32U data,INT8U stype,INT16U usize)
{
	INT32U v_32;
	INT16U v_16;
	INT8U  v_8;

	if (pnt==0) return;
	if (stype=='W') {
	 	v_16=(INT16U)data;
		plu_temp_set(0,pnt,(char *)&v_16,usize);
	} else if (stype=='L') {
 	 	v_32=(INT32U)data;
		plu_temp_set(0,pnt,(char *)&v_32,usize);
	} else if (stype=='B') {
		v_8=(INT8U)data;
		plu_temp_set(0,pnt,(char *)&v_8,usize);
	}
}

// void plu_save_deptid_com(INT32U pnt,INT32U data,INT8U stype,INT16U usize)
//void plu_save_deptid_com(INT32U pnt,INT32U data,INT8U stype)
//{
//	INT32U v_32;
//	INT16U v_16;
//	INT8U  v_8;
//
//	if (stype=='W') {
//	 	v_16=(INT16U)data;
//		Flash_wwrite(pnt,v_16);
//	} else if (stype=='L') {
// 	 	v_32=(INT32U)data;
//		Flash_lwrite(pnt,v_32);
//	} else if (stype=='B') {
//		v_8=(INT8U)data;
//		Flash_bwrite(pnt,v_8);
//	}
//}

void plu_save_deptid_com_type(INT8U m,INT32U pnt,INT32U data,INT8U stype,INT16U usize)
{
	INT32U v_32;
	INT16U v_16;
	INT8U  v_8;

	if (pnt==0l) return;
	if (stype=='W') {
	 	v_16=(INT16U)data;
		plu_temp_set(m,pnt,(char *)&v_16,usize);
	} else if (stype=='L') {
 	 	v_32=(INT32U)data;
		plu_temp_set(m,pnt,(char *)&v_32,usize);
	} else if (stype=='B') {
		v_8=(INT8U)data;
		plu_temp_set(m,pnt,(char *)&v_8,usize);
	}
}

INT8U plu_remotecall(INT16U deptno,INT32U pluno,INT8U mode)
{
// mode==1, check exist plu
//     ==0, no check
// err = 0, OK
// err = 1, ERROR
// err = 9, OK, and Deleted
	char temp[64];
	INT16S  sz;
	INT8U ret,err,cmd;
	INT16U wait;

	err=0;
//debug_tic(0);
	if (!network_status.remote_plu) return 1;
	cmd=0x15;		// master의 sync flag와 상관없이 항상 PLU 전송요청
	if (mode) cmd=0x14;	// master의 sync flag여부에 따라 PLU 전송요청	
	ret=Ethernet_Server(CLIENT_OPEN, 0, CLIENT_NORMAL);
	if (ret==1) {
		network_direct(1);
		sz=ethernet_gen_head(&temp[0]);
		sprintf(&temp[sz],"R%02XF%02X%06lX,00%c",cmd,deptno,pluno,0x0a);
		commun_outleng(&(CommBufEthData[0]),temp,strlen(temp));
		network_status.load_plu=0;
		wait=SysTimer100ms;
		while (1) {
			network_common_trans();
			if (network_status.load_plu) break;
			if ((INT16U)(SysTimer100ms-wait)>15) break;
		}
		if (!network_status.load_plu) { err=1; goto OPA_ERR;}

//		sprintf(gmsg,"[%04x]",network_status.service_flag);
//		MsgOut(gmsg);

		if (network_status.service_flag&0x0004) {
		    network_status.load_dc=1;
		    sprintf(&temp[sz],"R19F%06lX,%02X%c",pluno,deptno,0x0a);
		    commun_outleng(&(CommBufEthData[0]),temp,strlen(temp));
		    wait=SysTimer100ms;
		    while (1) {
			network_common_trans();
			if (!network_status.load_dc) break;
			if ((INT16U)(SysTimer100ms-wait)>10) break;
		    }
		    if (network_status.load_dc) err=1;
		}
		if (network_status.load_plu==2) err=9;
OPA_ERR:	network_direct(0);
	 	Ethernet_Server(CLIENT_CLOSE, 0, CLIENT_NORMAL);
	} else if (ret==2) {
	} else {
		err=1;
	}
	if ((err>0)&& (err<9)) 
	{
		//BuzOn(4);
		network_error(MSG_21_NET_ERR);
	}
	return err;
}
#ifdef USE_INVENTORY_LABEL
long InventoryUnitWeight = 0;
#endif
INT8U plu_callbykey(INT16U deptno,INT32U pluno, PLU_BASE *plu,INT8U *taretype)
{
	INT32U iaddr;
	INT8U  ret,v8,ttype,sret,tunit;
	INT16U v16;
	long   fWeight;
	INT16S    sz;
	//long   baseUnit=1000L;
//DEL060829	INT32U defaultUnit;
	INT8U new_wunit_id;
	INT8U	flag_tmp;
	INT8U use_memberprice_flag;	
	INT8U use_packingprice_flag;
	INT32U addr;
#ifdef USE_CONTROL_CHANGING_PRICE
	INT16U field70;
#endif
#ifdef USE_USA_FIXEDWEIGHT_UNIT_OZ	
	INT8U decimal;
#endif
#if defined(USE_GSMART_BARCODE) || defined(USE_PRT_CHNIA_DISCOUNT_RATE)
	status_scale.discountflag = 0;
	status_scale.discount_rate = 0;
#endif		
	use_memberprice_flag = get_global_bparam(GLOBAL_SALE_SETUP_FLAG18) & 0x80;	//parameter 729
	use_packingprice_flag = get_global_bparam(GLOBAL_SALE_SETUP_FLAG19) & 0x02;	//parameter 743

	if (Startup.disable_dept) deptno=1;
	ret=0;
	if (network_status.remote_plu) { // Slave
		sret=plu_table_search((INT8U)deptno,pluno,&v16,0);
		ret=1;
		if (sret==0) ret=0;
		v8=plu_remotecall(deptno,pluno,ret);
		if (v8==9) ret=0;
	}
	if (ret==0) {
		plu_table_search((INT8U)deptno,pluno,&v16,0);
	}
	plu->address=v16;
	Flash_flush_nvbuffer();
	if (plu->address==0) {
		return FALSE;
	}
	ret=TRUE;
	plu_get_field(plu->address-1, PTYPE_PLU_NO,(INT8U *)&plu->PluCode  ,&sz,sizeof(plu->PluCode)); // Number
	addr = plu_get_field_point(plu->address-1,PTYPE_PLU_NAME1,&sz); // Name
	Flash_sread(addr, plu->ItemName, sz);

	plu_get_field(plu->address-1, PTYPE_UNIT_WEIGHT,(INT8U *)&plu->weightunit_id,&sz,sizeof(plu->weightunit_id));
	get_weightunit_and_id_fromid(plu->weightunit_id, &new_wunit_id, &plu->unit_kglb);
	plu->weightunit=get_unitweight(new_wunit_id, plu->unit_kglb);

	plu_get_field(plu->address-1, PTYPE_PRICE,(INT8U *)&plu->unitprice ,&sz,sizeof(plu->unitprice));
	plu_get_field(plu->address-1, PTYPE_DEPARTMENT_NO,(INT8U *)&plu->deptid    ,&sz,sizeof(plu->deptid));
	plu_get_field(plu->address-1, PTYPE_ITEM_CODE,(INT8U *)&plu->ItemCode,&sz,sizeof(plu->ItemCode));
#ifdef USE_TOPMART_STRUCTURE
	plu_get_field(plu->address-1, PTYPE_TR_USE_NO,(INT8U *)&plu->tr_use_no,&sz,sizeof(plu->tr_use_no));
	plu_get_field(plu->address-1, PTYPE_MAX_QTY,(INT8U *)&plu->max_qty,&sz,sizeof(plu->max_qty));
	plu_get_field(plu->address-1, PTYPE_KIND_NO,(INT8U *)&plu->kind_no,&sz,sizeof(plu->kind_no));
	plu_get_field(plu->address-1, PTYPE_PACKCED_DATE, (INT8U *)&plu->packedondate,&sz,sizeof(plu->packedondate));
	plu_get_field(plu->address-1, PTYPE_PACKCED_TIME, (INT8U *)&plu->packedontime,&sz,sizeof(plu->packedontime));
#endif
#ifdef USE_INDIA_FUNCTION
	if (UseItemCode13Digit)
	{
		plu->ItemCode = 0;
	}
#endif
	plu_get_field(plu->address-1, PTYPE_TARE_NO,(INT8U *)&v8,&sz,sizeof(v8));	// tare id
	flag_tmp=get_global_bparam(GLOBAL_SALE_SETUP_FLAG2);
	if(OnOff(flag_tmp&0x02)) {	// use tare ID
		flag_tmp=get_tare_fromid((INT16U)v8, &ttype, &(plu->tare), &tunit);
		if(!flag_tmp || tunit != plu->unit_kglb) plu->tare = 0;
	} else {			// input tare
#ifndef USE_SPAR_MEMBER_PRICE
		if(use_memberprice_flag) // Param 729
		{
			plu_get_field(plu->address-1,PTYPE_TARE_VALUE,(INT8U *)&plu->member_price,&sz,sizeof(plu->tare));			
			plu->tare = 0;
		}
		else
#endif
		{
    		plu_get_field(plu->address-1, PTYPE_TARE_VALUE,(INT8U *)&plu->tare,&sz,sizeof(plu->tare));
			plu->member_price = 0;
	    }
	}
#ifdef USE_SPAR_MEMBER_PRICE 
    plu_get_field(plu->address-1, PTYPE_MEMBER_PRICE,(INT8U *)&plu->member_price,&sz,sizeof(plu->member_price));
#endif
	plu_get_field(plu->address-1,PTYPE_PERCENT_TARE_LIMIT,(INT8U *)&plu->tare_limit,&sz,sizeof(plu->tare_limit));
	plu_get_field(plu->address-1,PTYPE_PERCENT_TARE,(INT8U *)&plu->tare_percent,&sz,sizeof(plu->tare_percent));
	*taretype=0;
#ifdef USE_FIXED_PLUTYPE//USE_EMART_NETWORK
	plu->disable_typechange = 0;	// plu type 변경 가능
#endif
	plu_get_field(plu->address-1, PTYPE_PLU_TYPE,(INT8U *)&plu->ptype     ,&sz,sizeof(plu->ptype));
	if (plu->ptype==0) plu->ptype=1;
#ifdef USE_FIXED_PLUTYPE//USE_EMART_NETWORK
	if (plu->ptype >= PLU_BY_WEIGHT_FIX && plu->ptype <= PLU_BY_PCS_FIX)
	{
		plu->ptype -= 3;
		plu->disable_typechange = 1;// plu type 변경 불가
	}
#endif
	Operation.useMultipleUnits = 0;
	if(plu->ptype == 1) {//by weight
	} else {
//SG060920
//		if (Operation.operationMode==MODE_SELF) {
//			ret=FALSE;
//			goto END;
//		}
	}
	plu_get_field(plu->address-1, PTYPE_PIECE,(INT8U *)&plu->pcs       ,&sz,sizeof(plu->pcs));
	plu_get_field(plu->address-1, PTYPE_INGREDIENT_NO,(INT8U *)&v16            ,&sz,sizeof(v16));
	if(plu->pcs<1)
		plu->pcs = 1;
#ifdef USE_LOTTEMART_DEFAULT
	plu->pcs = 1; // PLU의	갯수에 쓰레기값 막음 (롯데는 PLU갯수 사용 안함.) 
#endif
#ifdef USE_PCS_FONT5_TO_TRACE_CODE	// USE PCS, PLUFONT5 field for Trace CODE 
	plu->qty=1;	// PCS를 품목코드로 사용
#else
	plu->qty=plu->pcs;
#endif
	if (v16) {
		iaddr = ing_calc_addr_by_key(v16-1);
		iaddr+=2;
	}
	plu->dc_id =0;
	plu->tax_id=0;
	plu->group =0;
#ifdef USE_TOPMART_DEFAULT //sewon 161128
	plu->sell_by_date=0;
#else
	plu->special_price=0;
#endif
#ifndef USE_PLU_MULTIPLE_PRICE	//Refefence PLU 기능 사용 안 함
	plu->coupled_dept =0;
	plu->coupled_plu  =0;
	plu->refer_dept   =0;
	plu->refer_plu    =0;
#endif
	plu->sales_msg 	  =0;
	fWeight           =0;
	plu->dc_id=_get_dc_number(deptno,pluno);
 	plu_get_field(plu->address-1, PTYPE_FIXED_PRICE,(INT8U *)&plu->fixed_price,&sz,sizeof(plu->fixed_price));// fixed price

	plu_get_field(plu->address-1, PTYPE_TAX_NO,(INT8U *)&plu->tax_id,&sz,sizeof(plu->tax_id));
	plu_get_field(plu->address-1, PTYPE_GROUP_NO,(INT8U *)&plu->group,&sz,sizeof(plu->group));
	plu_get_field(plu->address-1, PTYPE_LABEL_MSG_NO,(INT8U *)&plu->sales_msg,&sz,sizeof(plu->sales_msg));
#ifdef USE_TOPMART_STRUCTURE
	plu_get_field(plu->address-1, PTYPE_TR_SELLBY_DATE,(INT8U *)&plu->tr_sellby_date,&sz,sizeof(plu->tr_sellby_date)); //sewon 161121
#else
	plu_get_field(plu->address-1, PTYPE_SPECIAL_PRICE,(INT8U *)&plu->special_price,&sz,sizeof(plu->special_price));
#endif
#ifdef USE_PLU_MULTIPLE_PRICE
	plu_get_field(plu->address-1, PTYPE_REF_PLU_NO,(INT8U *)&plu->first_target,&sz,sizeof(plu->first_target));
	plu_get_field(plu->address-1, PTYPE_COUPLE_PLU_NO,(INT8U *)&plu->first_price,&sz,sizeof(plu->first_price));
	plu_get_field(plu->address-1, PTYPE_LINK1_PLU_NO,(INT8U *)&plu->second_target,&sz,sizeof(plu->second_target));
	plu_get_field(plu->address-1, PTYPE_LINK2_PLU_NO,(INT8U *)&plu->second_price,&sz,sizeof(plu->second_price));
#else	//Refefence PLU 기능 사용 안 함
	plu_get_field(plu->address-1, PTYPE_COUPLE_DEPT_NO,(INT8U *)&plu->coupled_dept,&sz,sizeof(plu->coupled_dept));
	plu_get_field(plu->address-1, PTYPE_COUPLE_PLU_NO,(INT8U *)&plu->coupled_plu,&sz,sizeof(plu->coupled_plu));
	plu_get_field(plu->address-1, PTYPE_REF_DEPT_NO,(INT8U *)&plu->refer_dept,&sz,sizeof(plu->refer_dept));
	plu_get_field(plu->address-1, PTYPE_REF_PLU_NO,(INT8U *)&plu->refer_plu,&sz,sizeof(plu->refer_plu));
#endif
#ifdef USE_USA_FIXEDWEIGHT_UNIT_OZ
	if(plu->ptype == PLU_BY_WEIGHT){ //count(non-weight) type
		decimal=get_decimal_pt(6, 0);
		plu_get_field(plu->address-1,PTYPE_FIXED_WEIGHT,(INT8U *)&fWeight,&sz,sizeof(fWeight));
		fWeight = OztoLb(fWeight, 16, decimal);
	}
	else plu_get_field(plu->address-1,PTYPE_FIXED_WEIGHT,(INT8U *)&fWeight,&sz,sizeof(fWeight));
#else
	if(use_packingprice_flag) { // Param 743
		plu_get_field(plu->address-1,PTYPE_FIXED_WEIGHT,(INT8U *)&plu->packing_price,&sz,sizeof(fWeight));
		fWeight = 0;
	}
	else {
		plu_get_field(plu->address-1,PTYPE_FIXED_WEIGHT,(INT8U *)&fWeight,&sz,sizeof(fWeight));
		plu->packing_price = 0;
	}
#endif
	plu_get_field(plu->address-1, PTYPE_PLU_PREFIX,(INT8U *)&plu->prefix,&sz,sizeof(plu->prefix));
#ifdef USE_TRACE_STANDALONE
  #ifdef USE_TOPMART_STRUCTURE //sewon 161129	
	plu_get_field(plu->address-1, PTYPE_TR_COMP_FLAG,(INT8U *)&plu->tr_comp_flag,&sz,sizeof(plu->tr_comp_flag)); //sewon 161121
  #else
	plu_get_field(plu->address-1, PTYPE_TRACE_FLAG,(INT8U *)&plu->trace_flag,&sz,sizeof(plu->trace_flag));
  #endif
#endif
#ifdef USE_CONTROL_CHANGING_PRICE
	plu->ctrlPriceChange = 0;
	plu->upperLimit = 0;
	plu->lowerLimit = 0;
	
	plu_get_field(plu->address-1, PTYPE_NUTRITION_NO,(INT8U *)&field70,&sz,sizeof(field70));
	plu->ctrlPriceChange = field70 / 10000;	//x0000(10)
	plu->upperLimit = field70 % 10000 / 100;	//0xx00(10)
	plu->lowerLimit = field70 % 10000 % 100;	//000xx(10)
	
#endif
#ifdef USE_INVENTORY_LABEL
	InventoryUnitWeight = fWeight;
	plu->fixed_weight = 0;
#else
	plu->fixed_weight = fWeight;
#endif
	return ret;
}

//INT8U plu_callbykey_fromid(INT16S key,INT16S id,INT32U *data,INT16U *sz)
//{
//	INT32U addr;
//	INT8U plu_status,ret;
//
//	addr = plu_calc_addr_by_key( (long)key);
////	addr = key;
////	if (key>=EXT_PLU_NO) {
////		addr-= EXT_PLU_NO;
////		addr*= plu_record_size;
////		addr+= EXT_MEMCARD_ADDR_X;
////	} else {
////		addr*= plu_record_size;
////		addr+= FLASH_PLU_AREA_PNT;
////	}
//	plu_status=Flash_bread(addr);
//	if (plu_status>0x0f) {
//		plu_status=0;
//	}
//	ret=TRUE;
//	if ((plu_status&0x01)==0) { ret=FALSE; goto END; }
//	*data=(INT32U)plu_get_field_point((INT16S)key,(INT16U)id,(INT16S *)sz);
//	if (*sz==0) ret=FALSE;
//END:
//	return ret;
//}

INT8U plu_get_stype_fromid(INT16U cur_ptype)
{
	INT16S maxid,i;
	INT32U pnt;
	INT8U  stype;
	INT16U ptype;

//	maxid = plu_struct_get_nfield();
	maxid = plu_n_field;
//	pnt  = DFLASH_BASE;
//	pnt += FLASH_PLU_STRUCT_AREA;
//	pnt += PLU_STRUCT_POINT;
	pnt  = FLASH_PLU_STRUCT_PNT;

	for (i=0; i<=maxid; i++) {
		ptype  = Flash_wread(pnt+PLUSTR_PTYPE_POINT);
	   	stype  = Flash_bread(pnt+PLUSTR_STYPE_POINT);
		ptype = ptype&0x7fff;
		if (ptype==cur_ptype) goto EXIT;
		pnt += PLU_STRUCT_SIZE;
	}
	stype=0;
EXIT:
	return stype;
}

INT8U plu_get_stype_from_fid(INT16S id)
{
	INT32U pnt;
	INT8U stype;

	pnt  = FLASH_PLU_STRUCT_PNT;
	pnt += PLU_STRUCT_SIZE * id;
   	stype = Flash_bread(pnt+PLUSTR_STYPE_POINT);
	return stype;
}

INT16U plu_get_ptype_from_fid(INT16S id)
{
	INT32U pnt;
	INT16U ptype;

	pnt  = FLASH_PLU_STRUCT_PNT;
	pnt += PLU_STRUCT_SIZE * id;
   	ptype = Flash_wread(pnt+PLUSTR_PTYPE_POINT);
	return ptype;
}


INT16U	plu_get_item_position_from_ptype(INT16S cur_ptype)
{
	INT16U	 i, result_ptype=0xffff;

	for(i=0; i<plu_n_field; i++) {
		result_ptype = plu_get_ptype_from_fid(i);
		result_ptype = result_ptype & 0x7fff;		
		if(result_ptype == cur_ptype) {
			break;
		}
	}	
	return i;
}


INT8U plu_callbykey_from_fid(INT32U key,INT16S id,INT32U* data,INT16U *sz)
{

	INT32U addr;
//	INT8U plu_status,
	INT8U ret;
	INT32U rpnt;//,pid;

//	pid = plu_memory_get(key);
//	pid = plu_rem_number_get();
	ret=PLUSTR_OK;
//	if (!pid) { ret=PLUSTR_NOPLU; goto END; }
// ====================================================== end
//	addr = key;
//	if (key>=EXT_PLU_NO) {
//		addr-= EXT_PLU_NO;
//		addr*= (INT32U)plu_record_size;
//		addr+= EXT_MEMCARD_ADDR_X;
//	} else {
//		addr*= (INT32U)plu_record_size;
//		addr+= DFLASH_BASE+FLASH_PLU_AREA;
//	}
	addr= plu_calc_addr_by_key((long)key);
	rpnt= plu_get_nth_point((INT16U)id,(INT16U *)sz);
//	if (rpnt<0) return PLUSTR_EOF;
	addr+=rpnt;
	*data=addr;
	if (*sz==0) ret=PLUSTR_UNUSE;
END:
	return ret;
}

INT16U plu_get_number(void)
{
	INT32U addr;
	INT8U  r;
	INT16S   i;
	INT16U no;

	addr = FLASH_PLU_AREA_PNT;
	no=0;
	for (i=0; i<MAX_PLU_NO; i++) {
		if (i==MAX_PLU_small) {
			addr= EXT_MEMCARD_ADDR;
		}
		r=Flash_bread(addr);
		if (r<0x0f) {
			if (r&0x01) no++;
		}
		addr+=plu_record_size;
	}
	return no;
}

