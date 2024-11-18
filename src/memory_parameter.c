#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "globals.h"
#include "serial.h"
#include "flash.h"
#include "ram.h"
#include "flash.h"
#include "flash_app.h"
#include "initial.h"
#include "sale_report.h"
#include "common.h"
#include "main.h"
//////////////////////////////////////////////////
void set_global_bparam(INT32U a,INT8U v)
{
	INT32U pnt;
	pnt =DFLASH_BASE;
	pnt+=FLASH_GLOBAL_AREA;
	pnt+=a;
	Flash_bwrite(pnt,v);
}

INT8U get_global_bparam(INT32U a)
{
	INT32U pnt;
	INT8U  value;
	pnt =DFLASH_BASE;
	pnt+=FLASH_GLOBAL_AREA;
	pnt+=a;
	value=Flash_bread(pnt);
	return value;
}

void set_global_wparam(INT32U a,INT16U v)
{
	INT32U pnt;
	pnt =DFLASH_BASE;
	pnt+=FLASH_GLOBAL_AREA;
	pnt+=a;
	Flash_wwrite(pnt,v);
}

INT16U get_global_wparam(INT32U a)
{
	INT32U pnt;
	INT16U  value;
	pnt =DFLASH_BASE;
	pnt+=FLASH_GLOBAL_AREA;
	pnt+=a;
	value=Flash_wread(pnt);
	return value;
}

void set_global_lparam(INT32U a,INT32U v)
{
	INT32U pnt;
	pnt =DFLASH_BASE;
	pnt+=FLASH_GLOBAL_AREA;
	pnt+=a;
	Flash_lwrite(pnt,v);
}

INT32U get_global_lparam(INT32U a)
{
	INT32U pnt;
	INT32U  value;
	pnt =DFLASH_BASE;
	pnt+=FLASH_GLOBAL_AREA;
	pnt+=a;
	value=Flash_lread(pnt);
	return value;
}

void get_global_sparam(INT32U a,INT8U *s,INT16S leng)
{
	INT32U pnt;
	pnt =DFLASH_BASE;
	pnt+=FLASH_GLOBAL_AREA;
	pnt+=a;
	Flash_sread(pnt,(HUGEDATA INT8U*)s,leng);
}

void set_global_sparam(INT32U a,INT8U *s,INT16S leng)
{
	INT32U pnt;
	pnt =DFLASH_BASE;
	pnt+=FLASH_GLOBAL_AREA;
	pnt+=a;
	Flash_swrite(pnt,(HUGEDATA INT8U*)s,leng);
}

// ================================
//
void set_prt_bparam(INT32U a,INT8U v)
{
	INT32U pnt;
	pnt =DFLASH_BASE;
	pnt+=FLASH_PRINTER_AREA;
	pnt+=a;
	Flash_bwrite(pnt,v);
}

INT8U get_prt_bparam(INT32U a)
{
	INT32U pnt;
	INT8U  value;
	pnt =DFLASH_BASE;
	pnt+=FLASH_PRINTER_AREA;
	pnt+=a;
	value=Flash_bread(pnt);
	return value;
}

void set_net_bparam(INT32U a,INT8U v)
{
	INT32U pnt;
	pnt =DFLASH_BASE;
	pnt+=FLASH_NET_AREA;
	pnt+=a;
	Flash_bwrite(pnt,v);
}

void set_net_sparam(INT32U a,INT8U *s,INT16S leng)
{
	INT32U pnt;
	pnt =DFLASH_BASE;
	pnt+=FLASH_NET_AREA;
	pnt+=a;
	Flash_swrite(pnt,(HUGEDATA INT8U*)s,leng);
}

INT8U get_net_bparam(INT32U a)
{
	INT32U pnt;
	INT8U  value;
	pnt =DFLASH_BASE;
	pnt+=FLASH_NET_AREA;
	pnt+=a;
	value=Flash_bread(pnt);
	return value;
}

void set_net_wparam(INT32U a,INT16U v)
{
	INT32U pnt;
	pnt =DFLASH_BASE;
	pnt+=FLASH_NET_AREA;
	pnt+=a;
	Flash_wwrite(pnt,v);
}

INT16U get_net_wparam(INT32U a)
{
	INT32U pnt;
	INT16U  value;
	pnt =DFLASH_BASE;
	pnt+=FLASH_NET_AREA;
	pnt+=a;
	value=Flash_wread(pnt);
	return value;
}

void set_net_lparam(INT32U a,INT32U v)
{
	INT32U pnt;
	pnt =DFLASH_BASE;
	pnt+=FLASH_NET_AREA;
	pnt+=a;
	Flash_lwrite(pnt,v);
}

INT32U get_net_lparam(INT32U a)
{
	INT32U pnt;
	INT32U  value;
	pnt =DFLASH_BASE;
	pnt+=FLASH_NET_AREA;
	pnt+=a;
	value=Flash_lread(pnt);
	return value;
}

void get_net_sparam(INT32U a,INT8U *v,INT16S l)
{
	INT32U pnt;
	pnt =DFLASH_BASE;
	pnt+=FLASH_NET_AREA;
	pnt+=a;
	Flash_sread(pnt,(HUGEDATA INT8U *)v,l);
}

//////////////////////////////////////////////////////////////////
INT8U get_base_bparam(INT32U a)
{
	INT32U pnt;
	INT8U  value;
	pnt =DFLASH_BASE;
	pnt+=a;
	value=Flash_bread(pnt);
	return value;
}

INT16U get_base_wparam(INT32U a)
{
	INT32U pnt;
	INT16U  value;
	pnt  = DFLASH_BASE;
	pnt += a;
	value=Flash_wread(pnt);
	return value;
}

INT32U get_base_lparam(INT32U a)
{
	INT32U pnt;
	INT32U  value;
	pnt =DFLASH_BASE;
	pnt+=a;
	value=Flash_lread(pnt);
	return value;
}

void set_base_lparam(INT32U a,INT32U v)
{
	INT32U pnt;
	pnt =DFLASH_BASE;
	pnt+=a;
	Flash_lwrite(pnt,v);
}

void set_base_wparam(INT32U a,INT16U v)
{
	INT32U pnt;
	pnt =DFLASH_BASE;
	pnt+=a;
	Flash_wwrite(pnt,v);
}

void set_base_bparam(INT32U a,INT8U v)
{
	INT32U pnt;
	pnt =DFLASH_BASE;
	pnt+=a;
	Flash_bwrite(pnt,v);
}

void get_base_sparam(INT32U a,INT8U *v,INT16S l)
{
	INT32U pnt;
	pnt =DFLASH_BASE;
	pnt+=a;
	Flash_sread(pnt,(HUGEDATA INT8U *)v,l);
}

#ifdef USE_TRACE_1000QTY //sewon 161206
void get_ext_base_sparam(INT32U a,INT8U *v,INT16S l)
{
	INT32U pnt;
	pnt =EXT_MEMCARD_ADDR;
	pnt+=a;
	Flash_sread(pnt,(HUGEDATA INT8U *)v,l);
}
#endif

void set_base_sparam(INT32U a,INT8U *v,INT16S l)
{
	INT32U pnt;
	pnt =DFLASH_BASE;
	pnt+=a;
	Flash_swrite(pnt,(HUGEDATA INT8U *)v,l);
}

void get_nvram_summary_addr(INT32U base, INT16U a, SALES_SUMMARY_TYPE *sst)
{
	INT32U addr;
	addr = a*NVRAM_SALE_SUMMARY_SIZE;
	addr+= base;

	get_nvram_sparam(addr, (INT8U*)sst, sizeof(SALES_SUMMARY_TYPE));
}

void set_nvram_summary_addr(INT32U base, INT16U a, SALES_SUMMARY_TYPE *sst)
{
	INT32U addr;
	addr = a*NVRAM_SALE_SUMMARY_SIZE;
	addr+= base;

	set_nvram_sparam(addr, (INT8U*)sst, sizeof(SALES_SUMMARY_TYPE));
}

void get_nvram_trans_addr(INT32U base, INT16U a, TRANSACT_CHAIN *tc)
{
	INT32U addr;
	addr = a*MAX_TRANSACTION_SALE_SIZE;
	addr+= base;
	
	get_nvram_sparam(addr, (INT8U*)tc, sizeof(TRANSACT_CHAIN));
}

#if defined(USE_INDIA_FUNCTION) || defined(USE_SEND_TICKET_DATA) || defined(USE_SETPRODUCT_SALE) || defined(USE_HYUNDAI_SET_FUNCTION)
INT32U get_nvram_transAdd_addr(INT32U base,INT16U a, TRANS_ADD_CHAIN *tac)
{
	INT32U addr;
	addr = a*MAX_TRANSACTION_ADD_SIZE;
	addr+= base+NVRAM_BASE;

	get_nvram_sparam(addr, (INT8U*)tac, sizeof(TRANS_ADD_CHAIN));
}

void set_nvram_transAdd_addr(INT32U base, INT16U a, TRANS_ADD_CHAIN *tc)
{
	INT32U addr;
	addr = a*MAX_TRANSACTION_ADD_SIZE;
	addr+= base+NVRAM_BASE;
	
	set_nvram_sparam(addr, (INT8U*)tc, sizeof(TRANS_ADD_CHAIN));
}
#endif

void set_nvram_trans_addr(INT32U base, INT16U a, TRANSACT_CHAIN *tc)
{
	INT32U addr;
	addr = a*MAX_TRANSACTION_SALE_SIZE;
	addr+= base;
	
	set_nvram_sparam(addr, (INT8U*)tc, sizeof(TRANSACT_CHAIN));
}

void get_nvram_plu_summary(INT32U base, INT16U a, PLU_SALES_SUMMARY_TYPE *pst)
{
	INT32U addr;
	addr = (INT32U)a*NVRAM_PLU_SALE_SUMMARY_SIZE;
	addr+= base;
	
	get_nvram_sparam(addr, (INT8U*)pst, sizeof(PLU_SALES_SUMMARY_TYPE));
}

void set_nvram_plu_summary(INT32U base, INT16U a, PLU_SALES_SUMMARY_TYPE *pst)
{
	INT32U addr;
	addr = (INT32U)a*NVRAM_PLU_SALE_SUMMARY_SIZE;
	addr+= base;
	
	set_nvram_sparam(addr, (INT8U*)pst, sizeof(PLU_SALES_SUMMARY_TYPE));
}

INT32U get_TicketTransactionNumber(INT16U clekr_id)
{
	return get_nvram_lparam(NVRAM_TICKET_TR_NUMBER+clekr_id*4);
}

void set_TicketTransactionNumber(INT16U clekr_id, INT32U value)
{
	set_nvram_lparam(NVRAM_TICKET_TR_NUMBER+clekr_id*4, value);
}

void get_PrepackStatus(PREPACK_STATUS_TYPE *prepackStatus)
{
	get_nvram_sparam(NVRAM_PREPACK_STATUS, (INT8U*)prepackStatus, sizeof(PREPACK_STATUS_TYPE));
}

void set_PrepackStatus(PREPACK_STATUS_TYPE *prepackStatus)
{
	set_nvram_sparam(NVRAM_PREPACK_STATUS, (INT8U*)prepackStatus, sizeof(PREPACK_STATUS_TYPE));
}

void parameter_backup(void)
{
	INT32U dest_pos,sorc_pos;
	INT16U backup_size,i;
	INT8U cc;

	backup_size=(INT16U)(GLOBAL_PRT_HEADUP_SENSOR-GLOBAL_AREA_PRICE_PRIMARYSIGN);
	dest_pos = NVRAM_DEFAULT_SETTING;
	sorc_pos = GLOBAL_AREA_PRICE_PRIMARYSIGN;

	//dest_pos+= NVRAM_BASE;
	sorc_pos+= DFLASH_BASE+FLASH_GLOBAL_AREA;
	for (i=0; i<backup_size; i++) {
		cc=Flash_bread(sorc_pos++);
		set_nvram_bparam(dest_pos++, cc);
	}
}

void parameter_restore(void)
{
	INT32U dest_pos,sorc_pos;
	INT16U backup_size,i;
	INT8U cc;

	backup_size=(INT16U)(GLOBAL_PRT_HEADUP_SENSOR-GLOBAL_AREA_PRICE_PRIMARYSIGN);
	sorc_pos = NVRAM_DEFAULT_SETTING;
	dest_pos = GLOBAL_AREA_PRICE_PRIMARYSIGN;

	//sorc_pos+= NVRAM_BASE;
	dest_pos+= DFLASH_BASE+FLASH_GLOBAL_AREA;
	for (i=0; i<backup_size; i++) {
		cc = get_nvram_bparam(sorc_pos++);
		Flash_bwrite(dest_pos,cc);
		dest_pos++;
	}
}

#ifdef USE_REMOTE_FUNCTION
void set_network_setting_sparam(INT8U *ip, INT8U *gateway, INT8U *subnetmask, INT16U port)
{
    set_net_sparam(NETWORK_LOCAL_IP, (HUGEDATA INT8U *)ip, 4);
    set_net_sparam(NETWORK_LOCAL_GATEWAY, (HUGEDATA INT8U *)gateway, 4);
    set_net_sparam(NETWORK_LOCAL_SUBNETMASK, (HUGEDATA INT8U *)subnetmask, 4);
    set_net_wparam(NETWORK_LOCAL_TCP_PORT, (INT16U)port);
    set_net_wparam(NETWORK_LOCAL_UDP_PORT, (INT16U)port); 
}

void get_network_setting_sparam(INT8U *ip, INT8U *gateway, INT8U *subnetmask, INT16U *port)
{
    INT8U getIp[4];
    INT8U getGate[4];
    INT8U getSubnet[4];
    INT16U getPort;
    
    get_net_sparam(NETWORK_LOCAL_IP, getIp, 4);
    memcpy(ip, getIp, 4);
    get_net_sparam(NETWORK_LOCAL_GATEWAY, getGate, 4);
    memcpy(gateway, getGate, 4);
    get_net_sparam(NETWORK_LOCAL_SUBNETMASK, getSubnet, 4);
    memcpy(subnetmask, getSubnet, 4);
    getPort = get_net_wparam(NETWORK_LOCAL_TCP_PORT);    
    port[0] = getPort;
}

void set_wifi_setting_sparam(INT8U *ssid, INT8U *wepVal, INT8U wepId, INT8U securType)
{
    set_net_sparam(NETWORK_WLAN_SSID, (INT8U *)ssid, 34);
    set_net_sparam(NETWORK_WLAN_WEP ,(INT8U *)wepVal, 65);
    set_net_bparam(NETWORK_WLAN_WEPID ,wepId);
    set_net_bparam(NETWORK_WLAN_USEWEP, securType);
}

void get_wifi_setting_sparam(INT8U *ssid, INT8U *wepVal, INT8U *wepIdsecuType)
{
    INT8U getSsid[34];
    INT8U getWepVal[65];
    INT8U temp8uArr[2];//[0]:WEPID, [1]:USEWEP
    
    get_net_sparam(NETWORK_WLAN_SSID, (INT8U *)getSsid ,sizeof(getSsid));
    memcpy(ssid, getSsid, sizeof(getSsid));
    get_net_sparam(NETWORK_WLAN_WEP ,getWepVal ,sizeof(getWepVal));
    memcpy(wepVal, getWepVal, sizeof(getWepVal));
    temp8uArr[0] = get_net_bparam(NETWORK_WLAN_WEPID);   
    wepIdsecuType[0] = temp8uArr[0];
    temp8uArr[1] = get_net_bparam(NETWORK_WLAN_USEWEP);
    wepIdsecuType[1] = temp8uArr[1];
}
#endif

/*
INT8U gen_price(HUGEDATA INT8U *dest,long data,INT8U digit, char decimal_ch)
{
	char currency_sign1[6];
	char currency_sign2[6];
	char wtemp[20];
	char currency_type;

	get_global_sparam(GLOBAL_AREA_PRICE_PRIMARYSIGN,(INT8U *)currency_sign1,4);
	get_global_sparam(GLOBAL_AREA_PRICE_LASTSIGN   ,(INT8U *)currency_sign2,4);
	currency_type=get_global_bparam(GLOBAL_AREA_PRICE_TYPE);
	currency_type &= 0x0f;
	if ((currency_type>3) || (currency_type<0)) currency_type=1;
	format_value((HUGEDATA INT8U*)wtemp,data,digit,decimal_ch);
	switch (currency_type) {
		case 0: strcpy((char *)dest,(char*)wtemp);
			break;
		case 3: strcpy((char *)dest,(char *)currency_sign1);
			strcat((char *)dest,(char*)wtemp);
			strcat((char *)dest,(char *)currency_sign2);
			break;
		case 1: strcpy((char *)dest,currency_sign1);	
			strcat((char *)dest,(char*)wtemp);
			break;
		case 2: strcat(wtemp,currency_sign1);
			strcpy((char *)dest,(char*)wtemp);
			break;
	}
	return strlen((char *)dest);
}
*/
