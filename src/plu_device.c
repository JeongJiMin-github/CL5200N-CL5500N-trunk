#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "globals.h"
#include "mode_main.h"
#include "commbuf.h"
#include "initial.h"
#include "flash.h"
#include "flash_app.h"
#include "plu_device.h"
#include "plu_manager.h"
#include "main.h"
#include "ram.h"
#include "common.h"
#include "dsp_vfd7.h"
#include "dsp_app.h"
#include "discount.h"
#include "pgm_app.h"
#include "caption_app.h"
#include "menu.h"
#include "plu_edit.h"
#include "prt_font.h"
#include "dsp_common.h"
#include "mode_commun.h"
#include "initial_glb.h"
#include "ethernet_comm.h"
#include "traceability.h"
#ifdef USE_INDIA_FUNCTION
#include "inventory.h"
#endif

//////////////////////////////
// PLU Structure Version
//  [ver]: [date]. [description] (programmer)
//  01 : Standard Structure
//  02 : Russia Structure,
//       2007.03.14. Standard Structure. PLU no를 6자리로 수정 (SG)
//  03 : 2008.09.04. GS Mart Structure. Direct Ingredient, Date/Time Print flag 추가 (CJK)
//       2009.04.03. 개체플래그 추가 (CJK)
//  04 : 2009.04.03. 농협 PLU Structure. 상품명 5단 PLU 구조. 개체플래그 추가 (CJK)
//  05 : 2014.12.03. 인도네시아 엠포리움 PLU Structure. Direct Ingredient 추가 (KKH)
//  06 : 2015.10.12. Standard Direct Ingredient 추가(LYH) 
//  07 : 2016.07.28. tax no. Percent Tare, qty unit no. 추가(YHM) 
//  08 : 2019.0x.xx. 호주 CoOL 기능 추가(LYH) 
// 	09 : 2019.06.05. 중국 보능, CHN + PLU 8단 + Direct ingredient
// 	10 : 2020.04.16. 중국 까르푸, Standard Structure  + PLU 신규필드 Trace_URL(128Bytes) + Trace_Code(20Bytes)  -> PLU Structure 400 Bytes
//  11 : 2022.11.07. RUS SPAR Member Price 추가 (YHM)
#if (ROM_COUNTRY == ROM_RUS)
    #ifdef USE_PERCENT_WEIGHT
		#ifdef USE_RUSSIA_LOGISTICS_PLU
		#define PLU_NFIELD	42
		#define PLU_STR_VERSION 07  // Russia PLU Structre
		#elif defined(USE_SPAR_MEMBER_PRICE)
		#define PLU_NFIELD	33
		#define PLU_STR_VERSION 11  // Russia SPAR PLU Structre        
		#else
        #define PLU_NFIELD	34
		#define PLU_STR_VERSION 07	// Russia PLU Structre
		#endif
	#else
		#define PLU_NFIELD	32
		#define PLU_STR_VERSION 02	// Russia PLU Structre
	#endif
#elif defined(USE_GSMART_PLU)
	#if (PLU_COMMODITY_LINE_NO == 8)
		#define PLU_NFIELD	51
		#define PLU_STR_VERSION 03	// GS Mart PLU Structure
	#else
		#define PLU_NFIELD	45
		#define PLU_STR_VERSION 04	// 농협 PLU Structure
	#endif
#elif defined(USE_DIRECT_INGREDIENT)
	#if defined(USE_USA_INGREDIENT) || defined(USE_NORTH_AMERICA_INGREDIENT)
		#define PLU_NFIELD	40
		#define PLU_STR_VERSION 06
	#elif defined(USE_CHINA_BAONENG) || defined(USE_CHINA_BAONENG_COUNTRYGARDEN)
		#define PLU_NFIELD	45
		#define PLU_STR_VERSION 9
	#elif defined (USE_AUSTRALIA_INGREDIENT)
		#define PLU_NFIELD	49
		#define PLU_STR_VERSION 0x08
	#else // Standard Direct Ingredient Structure
    	#define PLU_NFIELD	49
		#define PLU_STR_VERSION 05
	#endif
#elif defined(USE_CHINA_CARRFOUR) || defined(USE_CHINA_HUARENWANGJI)
	#define PLU_NFIELD	50
	#define PLU_STR_VERSION 10		// CHN Carrefour PLU Structure
#else	// Standart Sturcture
	#if defined (USE_EUR_LOGISTICS_PLU)
		#define PLU_NFIELD	54
		#define PLU_STR_VERSION 12	// 영공기반 Logistics(물류전용) PLU(8000개) 사용
	#else
		#define PLU_NFIELD	48
		#define PLU_STR_VERSION 02	// Standard PLU Structure (향후 04로 변경해야함.)
	#endif
#endif

INT16U	plu_n_field;
INT16S	plu_record_size;
INT16U	plu_remainder_first= 0;
INT16U	plu_remainder_end  = 0;
HUGEDATA INT16U   plu_remainder_mem[MAX_PLU_ADDRESS];
HUGEDATA PLU_ADDR plu_table_addr[MAX_PLU_ADDRESS];
HUGEDATA PLU_ADDR plu_table_temp[MAX_PLU_ADDRESS];

extern INT32U FLASH_ING_EXT_AREA_PNT;
extern INT32U FLASH_ING_NOR_AREA_PNT;
extern INT16U EXT_INGREDIENT_NO;

INT32U MAX_PLU_NO,MAX_PLU_small,MAX_PLU_large;
#ifdef USE_EXTEND_8M_DFLASH
INT32U MAX_PLU_extend8M;    // Serial Flash Memory 8M확장에 따른 PLU 확장 (PLU 12,000 개)
#endif
INT32U MAX_PLU_REPORT_NO;
INT8U   plu_base_change;
extern  char gmsg[];

PLU_GLOBAL_INFO_STRUCT plu_global[128];

extern void commun_out(HUGEDATA COMM_BUF *CBuf,char *str);
extern void global_set_pluinhibit(INT8U x, INT8U y); //se-hyung 070802
extern void plu_tare_inhibit_set(void);
extern INT8U plu_inhibit_bycode(INT16U code,INT8U onoff);
extern void plu_table_sort(INT16S cnt);
extern void plu_address_save(INT16U key);
extern void plu_address_add(INT16U key);
extern INT8U plu_struct_get_version(void);
extern void ingredientTextDeleteAll(void);
extern HUGEDATA INT8U serial_send_point[MAX_SERIAL_BUFFER_LEN];
#ifdef USE_9999_INDIRECT_INGREDIENT
extern HUGEDATA INT8U* ingredient_reminder_number;
#endif
INT32U PluRemainderNumber = 0;  // NVRAM에서 일반 변수로 변경
void  plu_rem_number_set(INT32U v)
{
	PluRemainderNumber = v;
}

INT32U plu_rem_number_get(void)
{
	return PluRemainderNumber;
}

void plu_quick_delete_all(HUGEDATA COMM_BUF *CBuf, INT8U sendmode)
{
	INT32U total_block, send_cts, r;
	INT32U block_size;
	INT32U key;
	INT32U plu_addr;
	unsigned char str[20];
#ifdef USE_QUICK_DELETE_PLU
	INT8U chk;
#endif

	block_size = 3000;

	total_block = MAX_PLU_small / block_size;
	total_block += (MAX_PLU_NO - MAX_PLU_small) / block_size;
	r = MAX_PLU_small % block_size;
	if (r) total_block++;
	if (sendmode == 2) 
	{
		sprintf((char *)str,"C016:O02,S%04lX%c", total_block, 0x0a);
		commun_out(CBuf,(HUGEDATA char *)str);
	}

//////////////////////////////
// Start Basic Mem

	plu_addr = FLASH_PLU_AREA_PNT;
	send_cts = 0;
	for (key = 0; key < MAX_PLU_small; key++)
	{
#ifdef USE_QUICK_DELETE_PLU
		chk = Flash_bread(plu_addr+4);
		if (chk != 0x80)
		{
			Flash_bwrite(plu_addr+4,0x80);	// dept no의 최상의 bit를 1로 set => delete flag
			if (key % block_size == block_size - 1) // block의 마지막 key
			{
				if (sendmode == 10) 
				{
					sprintf((char *)str,"T %04X ",total_block - send_cts);
					display_string(2,(INT8U *)str);
					VFD7_Diffuse();
				}
				if (sendmode == 2) 
				{
					sprintf((char *)str,"C016:O02,N%04lX%c", send_cts, 0x0a);
					commun_out(CBuf,(HUGEDATA char *)str);
				}
				send_cts++;
			}
		}
#else
		Flash_bwrite(plu_addr+4,0x80);	// dept no의 최상의 bit를 1로 set => delete flag
		if (key % block_size == block_size - 1) // block의 마지막 key
		{
			if (sendmode == 10) 
			{
				sprintf((char *)str,"T %04X ",total_block - send_cts);
				display_string(2,(INT8U *)str);
				VFD7_Diffuse();
			}
			if (sendmode == 2) 
			{
				sprintf((char *)str,"C016:O02,N%04lX%c", send_cts, 0x0a);
				commun_out(CBuf,(HUGEDATA char *)str);
			}
			send_cts++;
		}
#endif
		plu_addr += plu_record_size;
		TcpIp_Task();
	}
	if (sendmode == 2) 
	{
		if (r)
		{
			sprintf((char *)str,"C016:O02,N%04lX%c", send_cts, 0x0a);
			commun_out(CBuf,(HUGEDATA char *)str);
			send_cts++;
			TcpIp_Task();
		}
	}



	if (MAX_PLU_NO <= MAX_PLU_small) return;

//////////////////////////////
// Start Ext Mem
	plu_addr = EXT_MEMCARD_ADDR_X;
	for (key = MAX_PLU_small; key < MAX_PLU_NO; key++)
	{
#ifdef USE_QUICK_DELETE_PLU
		chk = Flash_bread(plu_addr+4);
		if (chk != 0x80)
		{
			Flash_bwrite(plu_addr+4,0x80);	// dept no의 최상의 bit를 1로 set => delete flag
			if ((key - MAX_PLU_small) % block_size == block_size - 1)
			{
				if (sendmode == 10) 
				{
					sprintf((char *)str,"T %04X ",total_block - send_cts);
					display_string(2,(INT8U *)str);
					VFD7_Diffuse();
				}
				if (sendmode == 2) 
				{
					sprintf((char *)str,"C016:O02,N%04lX%c", send_cts, 0x0a);
					commun_out(CBuf,(HUGEDATA char *)str);
				}
				send_cts++;
			}
		}
#else
		Flash_bwrite(plu_addr+4,0x80);	// dept no의 최상의 bit를 1로 set => delete flag
		if ((key - MAX_PLU_small) % block_size == block_size - 1)
		{
			if (sendmode == 10) 
			{
				sprintf((char *)str,"T %04X ",total_block - send_cts);
				display_string(2,(INT8U *)str);
				VFD7_Diffuse();
			}
			if (sendmode == 2) 
			{
				sprintf((char *)str,"C016:O02,N%04lX%c", send_cts, 0x0a);
				commun_out(CBuf,(HUGEDATA char *)str);
			}
			send_cts++;
		}
#endif
		plu_addr += plu_record_size;
		TcpIp_Task();
	}
/////////////////////
}

#define FLASH_DELETE_SIZE	0x4000//8192l

void plu_all_delete(HUGEDATA COMM_BUF *CBuf,INT8U sendmode)
{
	INT32U i,rem,plu_addr,data_size,nsz,rsz;
	INT32U nsz_ex,rsz_ex;
	INT32U total_block, send_cts;
	unsigned char str[20];
	POINT disp_p;
	CAPTION_STRUCT cap;
	char old_font;
	INT8U backup_directdraw;

	set_nvram_lparam(NVRAM_PLU_LASTNUMBER,0);
	if (sendmode==8) goto EXIT;
	if (sendmode==1 || sendmode == 2) { // 순수한 통신 모드에서의 행동. Menu 화면을 Delete 화면으로 변경해야 한다.
		Dsp_SetPage(DSP_ERROR_PAGE);
		old_font=DspStruct.Id1;
		DspLoadFont1(DSP_MENU_FONT_ID);
		disp_p = point( (Startup.menu_type)*Startup.menu_ygap, 0);
		caption_split_by_code(0x1149, &cap,0);
		backup_directdraw = DspStruct.DirectDraw;
		DspStruct.DirectDraw = 0;
		Dsp_Fill_Buffer(0);    	
		Dsp_Write_String(disp_p,(HUGEDATA char*)&cap.form[3]);
		Dsp_Diffuse();
	}
	disp_p = point( (1+Startup.menu_type)*Startup.menu_ygap, 0);
	if ((MAX_PLU_small<=0) || (MAX_PLU_small>9000)) {	// 6000 => 9000 : 체크
		MAX_PLU_small=PLU_BASIC_AREA_QTY;
		plu_record_size=MAX_PLU_RECORDSIZE_DEFAULT;
	}
	if ((plu_record_size<=0) || (plu_record_size>1000)) {
		plu_record_size=MAX_PLU_RECORDSIZE_DEFAULT;
	}
#ifdef USE_INDIA_FUNCTION
	DeletePluInventoryAll();
#endif	
	if (sendmode == 2 || sendmode == 10)	// for Data sync by CJK090117
	{
		plu_quick_delete_all(CBuf, sendmode);
	}
	else
	{	 
		rem = (INT32U)MAX_PLU_small;
		plu_addr = FLASH_PLU_AREA_PNT; // default address
		data_size = rem;
		data_size*= plu_record_size;
		nsz = data_size / FLASH_DELETE_SIZE;
		rsz = data_size % FLASH_DELETE_SIZE;

		nsz_ex = 0;
		rsz_ex = 0;
		if (MAX_PLU_NO>MAX_PLU_small) {
			data_size = PLU_EXTER_AREA_QTY;
			data_size*= plu_record_size;
			nsz_ex = data_size / FLASH_DELETE_SIZE;
			rsz_ex = data_size % FLASH_DELETE_SIZE;
		}
		data_size = FLASH_DELETE_SIZE;
		total_block = nsz + nsz_ex;
		if (rsz) total_block++;
		if (rsz_ex) total_block++;
		if (sendmode==1) {
			sprintf((char *)str,"C016:O02,S%04lX%c", total_block, 0x0a);
			commun_out(CBuf,(HUGEDATA char *)str);
		}
		send_cts = 0;

		for (i=0; i<nsz; i++) {
			Flash_clear(plu_addr,0xff,data_size);
			if (sendmode==9) {
				sprintf((char *)str,"T %04X ",nsz-i);
				display_string(2,(INT8U *)str);
				VFD7_Diffuse();
			}
			if (sendmode==1) {
				sprintf((char *)str,"C016:O02,N%04lX%c", send_cts, 0x0a);
				commun_out(CBuf,(HUGEDATA char *)str);
				send_cts++;
			}
			plu_addr+=data_size;
		}
		if (rsz) {
			Flash_clear(plu_addr,0xff,rsz);
			if (sendmode==1) {
				sprintf((char *)str,"C016:O02,N%04lX%c", send_cts, 0x0a);
				commun_out(CBuf,(HUGEDATA char *)str);
				send_cts++;
			}
		}

	EXIT:	
		if (MAX_PLU_NO>MAX_PLU_small) {
			plu_addr  = EXT_MEMCARD_ADDR_X;
			for (i=0; i<nsz_ex; i++) {
				Flash_clear(plu_addr,0xff,data_size);
				if (sendmode==9) {
					sprintf((char *)str,"E %04X ",nsz_ex-i);
					display_string(2,(INT8U *)str);
					VFD7_Diffuse();
				}
				if (sendmode==1) {
					sprintf((char *)str,"C016:O02,N%04lX%c", send_cts, 0x0a);
					commun_out(CBuf,(HUGEDATA char *)str);
					send_cts++;
				}
				plu_addr+=data_size;
			}
			if (rsz_ex) {
				Flash_clear(plu_addr,0xff,rsz_ex);
				if (sendmode==1) {
					sprintf((char *)str,"C016:O02,N%04lX%c", send_cts, 0x0a);
					commun_out(CBuf,(HUGEDATA char *)str);
					send_cts++;
				}
			}
		}
	}
	if (sendmode>=8) {
		sprintf((char *)str,"int TB");
		display_string(2,(INT8U *)str);
		VFD7_Diffuse();
	}
	plu_table_init();
#ifdef USE_TRACE_STANDALONE
	TNT_InitTraceNoTable();
#endif
#ifdef USE_CHN_EMART_TRACE
	chn_emart_trace_delete_all();
#endif
#ifdef USE_CHN_EMART_BIRDS_TRACE
	chn_emart_qr_trace_delete_all(); //20140911
#endif

	
#ifdef USE_DIRECT_INGREDIENT//DEL080826#if (ROM_COUNTRY == ROM_RUS)
	ingredientTextDeleteAll();
#endif	
	if (sendmode>=8) {
		sprintf((char *)str,"int DC");
		display_string(2,(INT8U *)str);
		VFD7_Diffuse();
	}
	if (sendmode==8) goto NEXT;
	discount_delete_all();
#ifdef USE_DIRECT_NUTRITION
	delete_all_plu_direct_nutrition();
#endif	
	commun_net_mask(0, NULL, 0xff, 99);
	BuzOn(2);

NEXT:
	if (sendmode==1 || sendmode == 2) {
		DspStruct.DirectDraw = backup_directdraw;
		DspLoadFont1(old_font);
		Dsp_SetPage(DSP_DEFAULT_PAGE);
		Dsp_Diffuse();
	} else if (sendmode==9 || sendmode == 10) {
		sprintf((char *)str,"--End");
		display_string(2,(INT8U *)str);
		VFD7_Diffuse();
	}
	if (sendmode>=8) {
		display_clear(DISPLAY_PRICE);
		VFD7_Diffuse();
	}
#ifdef USE_DIRECT_NUTRITION
	delete_all_plu_direct_nutrition();
#endif
}

INT32U plu_get_next_exist_address(INT16U nth)
{
	INT32U ntotal;

	ntotal = plu_rem_number_get();
	if ((nth>ntotal) || (nth==0)) {
		return 0xffff;
	} else {
		return (INT32U)(plu_table_addr[nth-1].addr);
	}
}

INT16U plu_ptype_reset(void)
{
	INT16U rsize,ptype,ttype;
	INT32U addr_plu;
	INT16S i;

	addr_plu  = DFLASH_BASE;
	addr_plu += FLASH_PLU_STRUCT_AREA;
	addr_plu += PLU_STRUCT_POINT;

	// ============...
	rsize = 3;
	// ============...
	for (i=0; i<128; i++) {
		plu_global[i].direct=0;
	}
	if ((plu_n_field<=0) || (plu_n_field>128)) {
		plu_n_field    =plu_struct_get_nfield();
		if ((plu_n_field<=0) || (plu_n_field>128)) {
			caption_error(1);
		}
	}
	for (i=0; i<plu_n_field; i++) {
		ptype               = Flash_wread(addr_plu+PLUSTR_PTYPE_POINT);
		plu_global[i].stype = Flash_bread(addr_plu+PLUSTR_STYPE_POINT);
		plu_global[i].usize = Flash_wread(addr_plu+PLUSTR_USIZE_POINT);
		plu_global[i].ptype_point=rsize;
		plu_global[i].ptype = ptype;
		ttype = ptype;
		ttype&=0x7fff;
		plu_global[i].inhibit=0;
		if (ttype<128) {
			plu_global[ttype].direct=i+1;
		}
		if (ptype&0x8000) plu_global[i].inhibit=1;
		rsize += plu_global[i].usize;
		addr_plu += PLU_STRUCT_SIZE;
	}
	return rsize;
}

#ifdef USE_DONT_SAVE_VALIDDATE_SALEDATE
TEMP_SELLDATE_STRUCT temp_selldate;
#endif

INT8U plu_init(INT8U xx)
{
	INT16U  i,dn,key,cnt,rsize,nrem;
	INT32U  xdn;
//	INT32U  addr_plu;
	INT16U  deptno;
	INT32U  pluno;
	unsigned char ret,notsave;
	INT8U plu_str_ver;
//	=========================================
//	char temp[32];
//	=========================================
							     
	plu_base_change=0;
	plu_n_field    =plu_struct_get_nfield();
	plu_str_ver    =plu_struct_get_version();	// Added by CJK 20060415
	if (plu_n_field==0   ) return FALSE;
	if (plu_n_field>=128 ) return FALSE;
	if (plu_n_field!=PLU_NFIELD) return FALSE;
	if (plu_str_ver!=PLU_STR_VERSION) return FALSE;	// Added by CJK 20060415

	plu_record_size=plu_get_recordsize();
	if (plu_rem_number_get()>MAX_PLU_ADDRESS) {	//PLU MAX에서 남은 개수 초기화되는 문제 수정
		plu_rem_number_set(0);
	}
#ifdef USE_9999_INDIRECT_INGREDIENT
	if (get_nvram_lparam(NVRAM_PLU_ROOT)>INGREDIENT_BASIC_AREA_QTY) {
		set_nvram_lparam(NVRAM_PLU_ROOT,0);
	}
#endif

	ret   = plu_struct_check();
	rsize = plu_ptype_reset();
	if (xx) return ret;
	cnt=0;
	plu_table_init();
	plu_remainder_first=MAX_PLU_NO;
	if (ret) {
		for (i=0; i<MAX_PLU_NO; i++) {
			plu_get_dept_pluno((INT32U)(i+1),&deptno,&pluno);
			dn=(INT16U)deptno;
			notsave=1;
			key = (INT16U)i+1;
			if ((dn>0) && (dn<MAX_DEPT_NO)) {
				if ((pluno>0l) && (pluno<=MAX_PLU_NUMBER)) {
					xdn =(INT32U)dn;
					xdn<<=24;
					xdn|=pluno;
					notsave=0;
					plu_table_addr[cnt].addr  = key;
					plu_table_addr[cnt].pluno = xdn;
					cnt++;
				} 
			}
			if (notsave) {
				plu_address_add((INT16U)key);
			}
		}
	}
// Swap
	nrem  = plu_remainder_end - plu_remainder_first;
	rsize = plu_remainder_first;
	dn    = plu_remainder_end -1;
	for (i=0; i<nrem/2; i++) {
		key=plu_remainder_mem[rsize];
		plu_remainder_mem[rsize]=plu_remainder_mem[dn];
		plu_remainder_mem[dn]=key;
		dn   --;
		rsize++;
	}
	plu_table_sort(cnt);
	plu_rem_number_set((INT32U)cnt);
#ifdef USE_DONT_SAVE_VALIDDATE_SALEDATE
 	temp_selldate.onoff=OFF;
#endif
	
	return ret;
}

void plu_temp_clear(INT8U m)
{
	HUGEDATA INT8U *nv_pnt;
	nv_pnt = (HUGEDATA INT8U*)get_temp_area(m);

#ifdef USE_DIRECT_INGREDIENT//DEL080826#if (ROM_COUNTRY == ROM_RUS)
	memset(nv_pnt,0,plu_record_size+4+DEFAULT_INGREDIENT_TEXT_ALL_SIZE);
#else
	memset(nv_pnt,0,plu_record_size);
#endif
	
}

extern INT8U CurPluWeightUnit;	

void plu_temp_init(void)//Unit Init
{
	INT16U  Size=1;		
	INT16U uitemp;
	INT32U data_point;

	uitemp = plu_get_item_position_from_ptype(PTYPE_UNIT_WEIGHT);
  	data_point   = plu_get_nth_point(uitemp,&Size);
	NvramTempBuffer[0][data_point] = CurPluWeightUnit;
}

void plu_temp_set(INT8U m,INT32U pnt,char *data,INT16U xsize)
{
	HUGEDATA INT8U *nv_pnt;
	nv_pnt = (HUGEDATA INT8U *)get_temp_area(m);
	nv_pnt += pnt;
	memcpy(nv_pnt,data,xsize);
}

void plu_temp_get(INT8U m,INT32U pnt,char *data,INT16U xsize)
{
	HUGEDATA INT8U *nv_pnt;
	nv_pnt = (HUGEDATA INT8U *)get_temp_area(m);
	nv_pnt += pnt;
	memcpy(data,nv_pnt,xsize);
}

INT32U ing_calc_addr_by_key(INT16U ingid)
{
	INT32U pnt;
	pnt =ingid;
	if (ingid>=MAX_INDIRECT_INGREDIENT_small) {
		pnt-=MAX_INDIRECT_INGREDIENT_small;
		pnt*=INGREDIENT_STRUCT_SIZE;
		pnt+=FLASH_ING_EXT_AREA_PNT;
	} else {
		pnt*=INGREDIENT_STRUCT_SIZE;
		pnt+=FLASH_ING_NOR_AREA_PNT;
	}
	return pnt;
}

#ifdef USE_DIRECT_INGREDIENT
INT32U ing_text_calc_addr_by_key(INT16U ingid)
{
	INT32U pnt;
	pnt = ingid;
	if (ingid>=MAX_DIRECT_INGREDIENT_small) {
		pnt-=MAX_DIRECT_INGREDIENT_small;
		pnt*=DEFAULT_INGREDIENT_TEXT_SIZE;
		pnt+=FLASH_INGREDIENT_TEXT_EXT_AREA;
	} else {
		pnt*=DEFAULT_INGREDIENT_TEXT_SIZE;
		pnt+=FLASH_INGREDIENT_TEXT_AREA;
	}
	return pnt;
}
#endif

#ifdef USE_9999_INDIRECT_INGREDIENT
INT32U ingredient_addr_by_ingredient_ID(INT16U ingredient_id)
{
	INT32U pnt;
	INT32U ingredient_index_table_addr;	
	INT16U i;	
	ingredient_index_table_addr = FLASH_INGREDIENT_TABLE_EXT_AREA;
	// Find exist Igredient ID
	for(i=0;i<MAX_INGREDIENT_NO;i++)
	{
		pnt = Flash_lread(ingredient_index_table_addr+i);
		if(pnt == ingredient_id)
		{
			return i;
		}
	}
	// 없을시 새로 부여
}

#endif

INT32U plu_calc_addr_by_key(long key)
{
	INT32U fl_xpnt;

	fl_xpnt=(INT32U)key;

#ifdef USE_SRAM_PLU_INDIA
	fl_xpnt*= (long)plu_record_size;
	fl_xpnt+= NVRAM_PLU_AREA;
	fl_xpnt+= NVRAM_BASE;
	return fl_xpnt;
#else

 #ifdef USE_EXTEND_8M_DFLASH
    if ( key < 0 || key >= MAX_PLU_extend8M ) {
 #else
	if (key < 0 || key>=MAX_PLU_large) {
 #endif
		return FLASH_PLU_AREA_PNT;
	}

 #ifdef USE_EXTEND_8M_DFLASH
	if( key >= MAX_PLU_large )              // PLU No. 10,000 ~ 12,000 (영공 기준)
    {
		fl_xpnt-= MAX_PLU_large;
		fl_xpnt*= (long)plu_record_size;
		fl_xpnt+= EXT_MEMCARD2_ADDR_X + FLASH_PLU_EXT2_AREA;
	}
    else if( key >= MAX_PLU_small )         // PLU No. 3000 ~ 10,000 (영공 기준)
    {
		fl_xpnt-= MAX_PLU_small;
		fl_xpnt*= (long)plu_record_size;
		fl_xpnt+= EXT_MEMCARD_ADDR_X;
	}
    else                                    // PLU No. ~3000 (영공 기준)
    {
        fl_xpnt*= (long)plu_record_size;
		fl_xpnt+= FLASH_PLU_AREA_PNT;
    }
 #else
    if (key>=MAX_PLU_small) {
		fl_xpnt-= MAX_PLU_small;
		fl_xpnt*= (long)plu_record_size;
		fl_xpnt+= EXT_MEMCARD_ADDR_X;
	} else {
		fl_xpnt*= (long)plu_record_size;
		fl_xpnt+= FLASH_PLU_AREA_PNT;
	}
 #endif
	return fl_xpnt;
#endif
}

INT16S plu_move_temp(INT8U m,long key)
{
	INT32U   fl_xpnt;
	HUGEDATA INT8U *nv_pnt;

	nv_pnt = (HUGEDATA INT8U*)get_temp_area(m);
	fl_xpnt = plu_calc_addr_by_key(key);
#ifdef USE_SRAM_PLU_INDIA
	memcpy(nv_pnt, (INT8U*)fl_xpnt, plu_record_size);
#else
	Flash_sread(fl_xpnt,nv_pnt,plu_record_size);
#endif
	nv_pnt = nv_pnt + plu_record_size;
	return (INT32U)nv_pnt;
}

void plu_restore_temp(INT8U m,long key)
{
	INT32U   fl_pnt;
	HUGEDATA INT8U *nv_pnt;
	nv_pnt = (HUGEDATA INT8U *)get_temp_area(m);
	fl_pnt = plu_calc_addr_by_key(key);
#ifdef USE_SRAM_PLU_INDIA
	memcpy((INT8U*)fl_pnt, nv_pnt, plu_record_size);
#else
	Flash_swrite(fl_pnt,nv_pnt,plu_record_size);
#endif

}

#ifdef USE_DIRECT_INGREDIENT
// nblock : 탐색할 연속된 block 수
// index : 탐색 위치
INT16U ingredient_text_search_empty(INT16U index, INT16U nblock)
{
	INT16U i;
	INT32U fl_xpnt;
	INT16U chkValue;
	static INT16U lastIndex = 0;
	INT16U blockCnt = 0; //empty block count
	INT16U indexBuf = 0;
	INT16U emptyIndex = 0;
	INT8U emptyIndexFlag = 1; //empty block 탐색 flag

	if (index) {
		lastIndex = index;
	}
	else {
		lastIndex++;
	}
	
	if (lastIndex > MAX_INGREDIENT_TEXT_NUMBER) lastIndex = 1;

	indexBuf = lastIndex;
	for (i = 0; i < MAX_INGREDIENT_TEXT_NUMBER; i++)
	{
		fl_xpnt = ing_text_calc_addr_by_key(indexBuf - 1);
		chkValue = Flash_wread_c2(fl_xpnt);
		
		if (chkValue != indexBuf)
		{
			if (emptyIndexFlag) //empty block을 미리 탐색
			{
				emptyIndex = indexBuf;
				emptyIndexFlag = 0;
			}
			blockCnt++;
			if (blockCnt == nblock)
			{
				blockCnt = 0;
				lastIndex = indexBuf-(nblock-1);
				return lastIndex;
			}
		}
		else
		{
			blockCnt = 0;
		}
		indexBuf++;
		if (indexBuf > MAX_INGREDIENT_TEXT_NUMBER)
		{
			indexBuf = 1;
		}
	}

	return emptyIndex;
}

INT16U ingredient_text_count_empty(void)
{
	INT16U i;
	INT32U fl_xpnt;
	INT16U chkValue;
	INT16U count;

	count = 0;
	fl_xpnt = FLASH_INGREDIENT_TEXT_AREA;
	for (i = 0; i < MAX_DIRECT_INGREDIENT_small; i++)
	{
		chkValue = Flash_wread_c2(fl_xpnt);
		if(chkValue != i + 1)
		{
			count++;
		}
		fl_xpnt += DEFAULT_INGREDIENT_TEXT_SIZE;
	}
	fl_xpnt = FLASH_INGREDIENT_TEXT_EXT_AREA;
	for (i = MAX_DIRECT_INGREDIENT_small; i < MAX_INGREDIENT_TEXT_NUMBER; i++)
	{
		chkValue = Flash_wread_c2(fl_xpnt);
		if(chkValue != i + 1)
		{
			count++;
		}
		fl_xpnt += DEFAULT_INGREDIENT_TEXT_SIZE;
	} 
			
	return count;
}

INT16U ingredient_text_check_empty_count(INT16U qty)
{
	INT16U i;
	INT32U fl_xpnt;
	INT16U chkValue;
	static INT16U lastIndex = 0;
	INT16U count;
	INT16U offset;

	lastIndex++;
	if (lastIndex > MAX_INGREDIENT_TEXT_NUMBER) lastIndex = 1;

	count = 0;
	for (i = 0; i < MAX_INGREDIENT_TEXT_NUMBER; i++)
	{
		fl_xpnt = ing_text_calc_addr_by_key(lastIndex - 1);
		chkValue = Flash_wread_c2(fl_xpnt);
		
		if (chkValue != lastIndex)
		{
			count++;
			if (count >= qty) return 1;
		}
		lastIndex++;
		if (lastIndex > MAX_INGREDIENT_TEXT_NUMBER) 
		{
			lastIndex = 1;
		}
	} 
			
	return 0;
}


/////////////////////////////
// paramter
//   ingred_no : PLU에 등록된 Ingredient No 배열 포인터
//   dmsg : read할 Ingredient Text Buffe 포인터
//   start_offset : read할 시작 위치
//   size : read할 길이
INT8U ingredient_text_read(INT16U *ingred_no, HUGEDATA INT8U *dmsg, INT16U start_offset, INT16U size)	// flash -> nvram
{
	INT16S i;
	INT32U fl_xpnt;
	INT16U sz;
	INT8U *ptr;
	INT16U start_block, end_block, start_point, end_point;

	start_block = start_offset / (DEFAULT_INGREDIENT_TEXT_SIZE - 2);
	start_point = start_offset % (DEFAULT_INGREDIENT_TEXT_SIZE - 2);
	end_block = (start_offset + size - 1) / (DEFAULT_INGREDIENT_TEXT_SIZE - 2);
	end_point = (start_offset + size - 1) % (DEFAULT_INGREDIENT_TEXT_SIZE - 2);
	if (end_block > DIRECT_INGREDIENT_BLOCK_NUM - 1) 
	{
		end_block = DIRECT_INGREDIENT_BLOCK_NUM - 1;
		end_point = (DEFAULT_INGREDIENT_TEXT_SIZE - 2) - 1;
	}

	ptr = dmsg;
	for (i = start_block; i <= end_block; i++)
	{
		sz = DEFAULT_INGREDIENT_TEXT_SIZE - 2;
		if (i == start_block) 
		{
			sz -= start_point;
		}
		else if (i == end_block)
		{
			sz = end_point + 1;
		}
		if ((ingred_no[i] > 0) && (ingred_no[i] <= MAX_INGREDIENT_TEXT_NUMBER))
		{
			fl_xpnt = ing_text_calc_addr_by_key(ingred_no[i] - 1);
			fl_xpnt += 2;
			if (i == start_block) 
			{
				fl_xpnt += start_point;
			}
			Flash_sread_c2(fl_xpnt, ptr, sz);
		}
		else
		{ 
			ingred_no[i] = 0;
			memset(ptr , 0, sz);
		}
		ptr += sz;
		*ptr = 0;	// block이 꽉찬 경우(150byte) null이 포함되지 않은 경우 대비
	}

	return 1;
}

/////////////////////////////
// paramter
//   ingred_no : PLU에 등록된 Ingredient No 배열 포인터
//   dmsg : write할 Ingredient Text
//   start_offset : write할 시작 위치
//   size : write할 길이(write할 길이 이후 나머지 영역은 삭제함)
extern INT16U AutoFlushCache2Time;
INT8U ingredient_text_write(INT16U *ingred_no, INT8U *dmsg, INT16U start_offset, INT16U size, INT16U nblock)	// nvram->flash
{
	INT8U i;
	INT16U index;
	INT16U cur_size;
	INT16U len;
	INT8U temp[8];
	INT8U *ptr;
	INT16U start_block;
	INT16U start_point;
	//INT16U end_block;
	//INT16U end_point;
	INT16U msg_end_block;
	INT16U msg_end_point;
	INT16U use_block;
	INT16U total_block;
	INT16U sz;
	INT32U fl_xpnt;

	start_block = start_offset / (DEFAULT_INGREDIENT_TEXT_SIZE - 2);
	start_point = start_offset % (DEFAULT_INGREDIENT_TEXT_SIZE - 2);
	len = strlen((char *)dmsg);
	if (len > size) len = size;
	if (len > 0)
	{
		msg_end_block = (start_offset + len - 1) / (DEFAULT_INGREDIENT_TEXT_SIZE - 2);
		msg_end_point = (start_offset + len - 1) % (DEFAULT_INGREDIENT_TEXT_SIZE - 2);
		use_block = 0;
		for (i = start_block; i <= DIRECT_INGREDIENT_BLOCK_NUM - 1; i++)
		{
			if (ingred_no[i]) use_block++;
		}

		// check empty size
		total_block = msg_end_block - start_block + 1;

		if (total_block > use_block)
		{
			if (!ingredient_text_check_empty_count(total_block-use_block)) return 0;	// lack of space
		}
	}
	else
	{
		msg_end_block = 0;
		msg_end_point = 0;
	}

	memset(temp, 0, sizeof(temp));
	cur_size = 0;
	ptr = dmsg;
	for (i = start_block; i <= DIRECT_INGREDIENT_BLOCK_NUM - 1; i++)	// start_offset 부터 size만큼 write하고 남은 영역은 삭제함
	{
		if (ingred_no[i] > MAX_INGREDIENT_TEXT_NUMBER) ingred_no[i] = 0;
		sz = 0;

		if (cur_size < len)
		{
			sz = DEFAULT_INGREDIENT_TEXT_SIZE - 2;
			if (i == start_block)
			{
				sz -= start_point;
			}
			if (i == msg_end_block)
			{
				sz -= (DEFAULT_INGREDIENT_TEXT_SIZE - 2) - (msg_end_point + 1);
			}
			if (!ingred_no[i])
			{
				index = 0;
				if (i != 0)
				{
					index = ingred_no[i-1];
					nblock = 1;
				}
				ingred_no[i] = ingredient_text_search_empty(index, nblock);
				if (!ingred_no[i]) return 0;	// memory full
			}
			fl_xpnt = ing_text_calc_addr_by_key(ingred_no[i] - 1);
			fl_xpnt += 2;
			if (i == start_block) 
			{
				fl_xpnt += start_point;
			}

			Flash_swrite_c2(fl_xpnt-2, (INT8U *)&ingred_no[i], 2);	// (1:사용,0:미사용)
			Flash_swrite_c2(fl_xpnt, ptr, sz);	// nth ingredient block
			if (sz < DEFAULT_INGREDIENT_TEXT_SIZE - 2) Flash_swrite_c2(fl_xpnt+sz, (HUGEDATA INT8U *)temp, 1);	// NULL
			AutoFlushCache2Time = SysTimer100ms;
		}
		else
		{
			if (ingred_no[i])
			{
				fl_xpnt = ing_text_calc_addr_by_key(ingred_no[i] - 1);
				fl_xpnt += 2;
				if (i == start_block) 
				{
					fl_xpnt += start_point;
				}

				Flash_swrite_c2(fl_xpnt-2, (HUGEDATA INT8U *)temp, 4);
				AutoFlushCache2Time = SysTimer100ms;
			}
			ingred_no[i] = 0;
		}
		cur_size += sz;
		ptr += sz;
	}

	return 1;
}
#endif

INT8U plu_delete_department(unsigned char deptno)
{
	return plu_table_hdelete(deptno);
}

INT8U plu_delete(unsigned char deptno,long pluno,INT8U mode)
{
	INT16U dc_no;
	INT8U ret;
	INT16S  ideptno;
	INT16U key;

	if (plu_n_field==0) return 0x85;
	ideptno=(INT16S)deptno;
	dc_no    = _get_dc_number((INT16U)ideptno,(INT32U)pluno);
	if (dc_no) {
	    if (!mode) {
		error_display_caption(0xF03E);
		return 0x99;
	    } else {
		discount_delete_byid(dc_no);
	    }
	}
	ret=plu_table_delete(deptno, pluno, &key);
	if (ret != 0x99)
	{
		if (network_status.service_flag&0x20)	// send plu
		{
			if (ethernet_list.status == 1)	// plu delete sync는 master에서만 가능
			{
				commun_net_mask(0, (INT16U *)&key, 0xff, 1);	// set update flag
			}
		}
	}
#ifdef USE_DIRECT_NUTRITION
	delete_plu_direct_nutrition(deptno, pluno);
#endif
	return ret;
}

void plu_table_init(void)
{
	INT16S i;
	for (i=0; i<MAX_PLU_ADDRESS; i++) {
		plu_remainder_mem[i]=i+1;
		memset(&plu_table_addr[i],0,sizeof(PLU_ADDR));
	}
	plu_remainder_first=0;
	plu_remainder_end  =MAX_PLU_NO;
	plu_rem_number_set((long)0l);
}

void plu_address_add(INT16U key)
{
	if (plu_remainder_first) {
		plu_remainder_first--;
		plu_remainder_mem[plu_remainder_first]=key;
	} else {
		if (plu_remainder_end>=MAX_PLU_ADDRESS) return;
		plu_remainder_mem[plu_remainder_end++]=key;
	}
}

INT16U plu_address_get_new(void)
{
	if (plu_remainder_first==plu_remainder_end) return 0;
	if (plu_remainder_first> plu_remainder_end) {
		plu_remainder_first=plu_remainder_end;
		return 0;
	}
	return plu_remainder_mem[plu_remainder_first];
}

void plu_address_save(INT16U key)
{
	if (plu_remainder_mem[plu_remainder_first]==key) {
		plu_remainder_first++;
	}
}

void plu_table_insert(INT16S idx,PLU_ADDR pluidx)
{
	INT16S i,sz;
	INT16U ntotal;
	PLU_ADDR plu_addr;

	sz = sizeof(PLU_ADDR);
	ntotal=(INT16U)plu_rem_number_get();
	for (i=ntotal; i>idx; i--) {
		memcpy(&plu_addr,&plu_table_addr[i-1],sz);
		memcpy(&plu_table_addr[i ],&plu_addr,sz);
	}
	memcpy(&plu_table_addr[idx],&pluidx,sizeof(PLU_ADDR));
	ntotal++;
	plu_rem_number_set((long)ntotal);
}

void plu_table_delete_idx(INT16U idx)
{
	INT16S i,sz;
	INT16U ntotal;
	PLU_ADDR plu_addr;
	sz=sizeof(PLU_ADDR);
	ntotal=(INT16U)plu_rem_number_get();
	if (ntotal==0) return;
	for (i=idx; i<ntotal-1; i++) {
		memcpy(&plu_addr,&plu_table_addr[i+1],sz);
		memcpy(&plu_table_addr[i],&plu_addr,sz);
	}
	ntotal--;
	plu_rem_number_set((long)ntotal);
}

INT8U plu_table_hdelete(unsigned char deptno)
{
	INT16U idx;
	INT32U xno,cur_xno;
	INT16U i;
	INT32U plu_addr;
	INT8U ret;
	INT16U ntotal;
#ifdef USE_INDIA_FUNCTION
	INT16U usize;
	INT32U pluno;
#endif

	xno = deptno;
	xno<<=24;

	ret=0;
	ntotal=(INT16U)plu_rem_number_get();
	plu_table_search(deptno,0,&idx,1);
	if (idx<ntotal) 
	{
		for (i=idx; i<ntotal;i++ ) 
		{  //SGDebug
			cur_xno = plu_table_addr[i].pluno&0xff000000;
			if (cur_xno==xno) 
			{
				plu_address_add(plu_table_addr[i].addr);
				plu_addr =plu_calc_addr_by_key((long)(plu_table_addr[i].addr-1));

				plu_table_delete_idx((INT16U)i);
#ifdef USE_INDIA_FUNCTION
				plu_get_field(plu_table_addr[i].addr-1, PTYPE_PLU_NO, (INT8U *)&pluno, &usize, sizeof(pluno));
				DeletePluInventory(deptno, pluno);
#endif
				i--;
				ntotal=(INT16U)plu_rem_number_get();

				//Flash_clear(plu_addr,0xff,12);
				Flash_bwrite(plu_addr+4,0x80);	// CJK080522  dept no의 최상의 bit를 1로 set => delete flag

#ifdef USE_TRACE_STANDALONE
				TNT_DelTraceNoTable(0, 0, plu_table_addr[i].addr-1);
#endif
#ifdef USE_CHN_EMART_TRACE
				chn_emart_trace_delete_key(plu_table_addr[i].addr-1);
#endif
#ifdef USE_CHN_EMART_BIRDS_TRACE
				chn_emart_qr_trace_delete_key(plu_table_addr[i].addr-1); //20140911
#endif

				ret=1;
			} 
			else break;
		}
	}
	return ret;
}

#ifdef USE_DIRECT_INGREDIENT//DEL080826#if (ROM_COUNTRY == ROM_RUS)
void ingredient_text_table_delete(INT16U key)
{
	INT16U ingredTextNo[DIRECT_INGREDIENT_BLOCK_NUM];
	INT16S usize;
	INT8U i;
	INT32U fl_xpnt;
	INT8U temp[8];

	plu_get_field(key-1, PTYPE_INGREDIENT_TEXT,(INT8U *)&ingredTextNo[0], &usize, sizeof(ingredTextNo));

	memset(temp,0,sizeof(temp));
	for (i = 0; i < DIRECT_INGREDIENT_BLOCK_NUM; i++)
	{
		if (ingredTextNo[i] > 0 && ingredTextNo[i] <= MAX_INGREDIENT_TEXT_NUMBER)
		{
			fl_xpnt = ing_text_calc_addr_by_key(ingredTextNo[i] - 1);
			Flash_swrite_c2(fl_xpnt,(HUGEDATA INT8U *)temp,4);
			AutoFlushCache2Time = SysTimer100ms;
		}
		ingredTextNo[i] = 0;
	}
	plu_set_field(key - 1, PTYPE_INGREDIENT_TEXT, (INT8U *)&ingredTextNo[0]);
	if (network_status.service_flag&0x20)	// send plu
	{
		commun_net_mask(0, &key, 0xff, 1);
	}
}
#endif

INT8U plu_table_delete(unsigned char deptno, long pluno, INT16U *key)
{
	INT16U idx;
	INT32U plu_addr;

	*key = 0;
#ifdef USE_INDIA_FUNCTION
	DeletePluInventory(deptno, pluno);
#endif
	if (plu_table_search(deptno,pluno,&idx,1)) {
		*key = plu_table_addr[idx].addr;
		plu_address_add(*key);
		plu_addr=plu_calc_addr_by_key( (long)(*key-1) );
#ifdef USE_DIRECT_INGREDIENT//DEL080826#if (ROM_COUNTRY == ROM_RUS)
		ingredient_text_table_delete(*key);  //SG070730
#endif
		Flash_bwrite(plu_addr+4,0x80);	// CJK080522  dept no의 최상의 bit를 1로 set => delete flag
		plu_table_delete_idx((INT16U)idx);
#ifdef USE_TRACE_STANDALONE
		TNT_DelTraceNoTable(deptno, pluno, *key);
#endif
#ifdef USE_CHN_EMART_TRACE
		chn_emart_trace_delete_key(*key - 1);
#endif
#ifdef USE_CHN_EMART_BIRDS_TRACE
		chn_emart_qr_trace_delete_key(*key - 1);
#endif

		
		return 0x01;
	}
	return 0x99;
}

INT8U plu_table_save(unsigned char deptno,long pluno)
{
	PLU_ADDR plutbl;
	INT16U mem_addr;
	INT16U idx;

	if (!plu_table_search(deptno,pluno,&idx,1)) {
		mem_addr=plu_address_get_new();
		plu_address_save(mem_addr);
		plutbl.pluno = (INT32U)deptno;
		plutbl.pluno<<=24;
		plutbl.pluno|=pluno;
		plutbl.addr  = mem_addr;
		plu_table_insert(idx,plutbl);
	}
	if (!idx) return 0xff;
	return 0;
}


// mode = 0 : flash 상의 index. plu_get_field() 등에 사용
//        1 : sorting 된 index. plu_table_addr[]의 index에 사용
//        2 : flash 상의 index중 delete된 PLU까지 검색. plu_get_field() 등에 사용
INT8U plu_table_search(unsigned char deptno,long pluno,INT16U *idx,INT8U mode)
{
	INT32U xno;
	INT16S    low,high,mid,end,save;
	INT8U  ret;
	INT16U ntotal;
	xno = deptno;
	xno<<=24;
	xno|= pluno;
	low = 0;
	ret = 0;
	ntotal = (INT16U)plu_rem_number_get();
	mid  = 0;
	end  = 3;
	save = 0;
	if (ntotal==0) goto END;
	high = ntotal-1;
	while (low<=high) {
		mid=(high+low)/2;
		if (xno==plu_table_addr[mid].pluno) {
			end  = 0;
			save = mid;
			ret  = 1;
			break;
		} else if ((INT32U)xno<(INT32U)plu_table_addr[mid].pluno) {
			end = 2;
			save= mid;
			high= mid-1;
		} else {
			end = 1;
			low = mid+1;
			save= low;
		}
	}
	if (end) {
		mid=save;
	}
END:
	*idx = mid;
	if (mode==0) {
		if (end) *idx=0;
		else     *idx=plu_table_addr[mid].addr;
	}
	return ret;
}

INT8U plu_table_search_small(unsigned char deptno,long pluno,INT16U *idx,INT8U mode)	// Added by CJK 20051102
// 기존 plu_table_search()의 경우 찾는 plu가 없을 경우 큰 plu의 idx(key)값을 반환하나
// plu_table_search_small()의 경우 작은 plu의 idx(key)값을 반환하여 작은쪽으로 search하는 기능 제공
{
	INT32U xno;
	INT16S    low,high,mid,end,save;
	INT8U  ret;
	INT16U ntotal;
	xno = deptno;
	xno<<=24;
	xno|= pluno;
	low = 0;
	ret = 0;
	ntotal = (INT16U)plu_rem_number_get();
	mid  = 0;
	end  = 3;
	save = 0;
	if (ntotal==0) goto END;
	high = ntotal-1;
	while (low<=high) {
		mid=(high+low)/2;
		if (xno==plu_table_addr[mid].pluno) {
			end  = 0;
			save = mid;
			ret  = 1;
			break;
		} else if ((INT32U)xno<(INT32U)plu_table_addr[mid].pluno) {
			end = 2;
			save= mid-1;	// 작은값
			high= mid-1;
		} else {
			end = 1;
			low = mid+1;
			save= mid;	// 작은값
		}
	}
	if (end) {
		mid=save;
	}
END:
	*idx = mid;
	if (mode==0) {
		if (end) *idx=0;
		else     *idx=plu_table_addr[mid].addr;
	}
	return ret;
}


INT32U plu_table_seq_search(INT16S deptno,INT32U *pluno,INT32U max_id)
/////////////////////////////////////////////////////////////
// 현Pluno 를기준으로 현재 PLU no 를 포함하여 큰쪽으로 가장 작은값.
// pluno = 5 이면 5보다 큰 pluno 의 모든 것중에서 가장 작은 pluno
// Return 찾으면 address번호  못찾으면 0L
/////////////////////////////////////////////////////////////
{
	INT8U ret,s_ok;
	INT32U req_pluno,addr,xno,end_pluno;
	INT16U key,ntotal;

	req_pluno  = (INT32U)(*pluno);
	end_pluno  = deptno;
	end_pluno<<=24;
	end_pluno |= max_id;

	ret=plu_table_search((INT8U)deptno,req_pluno,&key,1);
	ntotal = (INT16U)plu_rem_number_get();
	addr=0l;
	xno =0l;
	s_ok=0;
	if (ret) {
		addr=(INT32U)plu_table_addr[key].addr;
		xno =(INT32U)plu_table_addr[key].pluno;
		s_ok=1;
		if (xno>end_pluno) {
			addr=0;
			xno =0;
			s_ok=0;
		}
	} else {
		if (key<=ntotal-1) {
			addr=(INT32U)plu_table_addr[key].addr;
			xno =(INT32U)plu_table_addr[key].pluno;
			s_ok=1;
		}
		if (xno>end_pluno) {
			addr=0;
			xno =0;
			s_ok=0;
		}
	}
	if (s_ok) {
		xno &=0xffffff;
		*pluno = xno;
	} else {
		(*pluno)=end_pluno;
		addr = 0l;
	}
	return addr;
}

INT32U plu_table_prev_search(INT16S deptno,INT32U *pluno,INT32U min_id)
/////////////////////////////////////////////////////////////
// 현Pluno 에 대하여 작은쪽 으로 가장 큰 값
// Return 찾으면 address번호  못찾으면 0L
/////////////////////////////////////////////////////////////
{
	INT8U ret,s_ok;
	INT32U req_pluno,addr,xno,end_pluno,cur_pluno;
	INT16U key,ntotal;
												    
	req_pluno = (INT32U)(*pluno);
	if (req_pluno==0) req_pluno=1;
	end_pluno = min_id;
	ntotal = (INT16U)plu_rem_number_get();
	ret=plu_table_search_small((INT8U)deptno,req_pluno,&key,1);	// Added by CJK 20051102	//작은쪽으로 Search하는 함수로 수정
	addr=0l;
	xno =0l;
	s_ok=0;
	if (ret) {
		if (key<ntotal-1) {
			addr=(INT32U)plu_table_addr[key].addr;
			xno =(INT32U)plu_table_addr[key].pluno;
			s_ok=1;
		}
		cur_pluno=xno&0xffffffl;
		if (cur_pluno<end_pluno) {
			s_ok=0;
		}
	} else {
		if (key<ntotal-1) {
			addr=(INT32U)plu_table_addr[key].addr;
			xno =(INT32U)plu_table_addr[key].pluno;
			s_ok=1;
		}
		cur_pluno=xno&0xffffffl;
		if (cur_pluno<end_pluno) {
			s_ok=0;
		}
	}
	if (s_ok) {
		xno &=0xffffff;
		*pluno = xno;
	} else {
		*pluno = 1L;
		addr   = 0;
	}
	return addr;

}
/************************************************************************
 * sort
 ************************************************************************/
void plu_radix_sort(INT16S byte, INT16S N, PLU_ADDR *source, PLU_ADDR *dest)
{
        long count[256];
        long index[256];
        INT16S  i,sz;

        memset(count, 0, sizeof(count));
	sz=sizeof(PLU_ADDR);
        for ( i=0; i<N; i++) count[((source[i].pluno)>>(byte*8))&0xff]++;
        index[0]=0;
        for ( i=1; i<256; i++ ) index[i]=index[i-1]+count[i-1];
        for ( i=0; i<N; i++ ) {
        	memcpy(&dest[index[((source[i].pluno)>>(byte*8))&0xff]++],&source[i],sz);
	}
}

void plu_table_sort(INT16S cnt)
{
        plu_radix_sort(0, cnt, plu_table_addr, plu_table_temp);
        plu_radix_sort(1, cnt, plu_table_temp, plu_table_addr);
        plu_radix_sort(2, cnt, plu_table_addr, plu_table_temp);
        plu_radix_sort(3, cnt, plu_table_temp, plu_table_addr);
}

INT8U plu_get_dept_pluno(INT32U address, INT16U *dept, INT32U *pluno)
{
	INT16S sz;
	if (address<=0l) return OFF;
	address--;
	plu_get_field((INT16U)address,PTYPE_PLU_NO,(INT8U *)pluno,&sz,sizeof(INT32U));
	if (*pluno>1000000L) *pluno=0;
	plu_get_field((INT16U)address,PTYPE_DEPARTMENT_NO,(INT8U *)dept ,&sz,sizeof(INT16U));
	if (*dept>99) *dept=0;
	if ((*pluno==0l) || (*dept==0)) {
		*pluno=0l;
		*dept =0;
		return OFF;
	}
	return ON;
}

INT8U plu_struct_check(void)
{
	INT16S sid;
	sid=plu_struct_get_nfield();
	if ((sid<1) || (sid>9999)) {
		plu_struct_set_base();
		sid=plu_struct_get_nfield();
		return FALSE;
	}
	return TRUE;
}

INT8U plu_struct_get_version(void)
{
	INT32U pnt;
	INT8U ver;

	pnt = DFLASH_BASE;
	pnt+= FLASH_PLU_STRUCT_AREA;
	pnt+= PLU_STRUCT_VERSION;
	ver =Flash_bread(pnt);
	return ver;
}

void plu_struct_set_version(INT8U ver)
{
	INT32U pnt;

	pnt = DFLASH_BASE;
	pnt+= FLASH_PLU_STRUCT_AREA;
	pnt+= PLU_STRUCT_VERSION;

	Flash_bwrite(pnt,ver);
}

INT16S  plu_struct_get_nfield(void)
{
	INT32U pnt;
	INT16S maxid;
	pnt  = DFLASH_BASE;
	pnt += FLASH_PLU_STRUCT_AREA;
	pnt += PLU_STRUCT_FIELDNO;
	maxid =Flash_wread(pnt);
	return maxid;
}

void plu_struct_set_nfield(INT16U w)
{
	INT32U pnt;
	pnt  = DFLASH_BASE;
	pnt += FLASH_PLU_STRUCT_AREA;
	pnt += PLU_STRUCT_FIELDNO;

	Flash_wwrite(pnt,w);
}

#if (ROM_COUNTRY == ROM_RUS)
void plu_struct_set_base(void)
{
	INT16U sum;
	INT8U i;
	INT8U idx;

	for (i=0; i<2; i++) {
		idx = 0;
		sum =plu_struct_set(idx++,"Department No." 	,PTYPE_DEPARTMENT_NO	,'W', 1l,	  99l, 2, 1, 0,i);
		sum+=plu_struct_set(idx++,"PLU No."			,PTYPE_PLU_NO			,'L', 1l, 999999l, 6, 1, 0,i);
		sum+=plu_struct_set(idx++,"PLU Type"		,PTYPE_PLU_TYPE 		,'M', 1l,	   4l, 1, 1, 0,i);
		sum+=plu_struct_set(idx++,"Name "			,PTYPE_PLU_NAME1		,'S', 0l,	   0l,40, 1, 0,i);
		sum+=plu_struct_set(idx++,"Name2"			,PTYPE_PLU_NAME2		,'S', 0l,	   0l,40, 1, 0,i);
		sum+=plu_struct_set(idx++,"Name3"			,PTYPE_PLU_NAME3		,'S', 0l,	   0l, 5, 1, 0,i);
		sum+=plu_struct_set(idx++,"Group"			,PTYPE_GROUP_NO 		,'W', 0l,	  99l, 2, 1, 0,i);
//		sum+=plu_struct_set( 7,"Ext.Barcode"	,PTYPE_EXT_BARCODE		,'S', 0l,      0l,20, 1, 0,i);
		sum+=plu_struct_set(idx++,"Label No."		,PTYPE_LABEL_NO 		,'W', 0l,	  99l, 2, 1, 0,i);
		sum+=plu_struct_set(idx++,"Aux. Label No." 	,PTYPE_AUX_LABEL_NO 	,'W', 0l,	  99l, 2, 1, 0,i);
		sum+=plu_struct_set(idx++,"Origin No." 		,PTYPE_ORIGIN_NO		,'W', 0l,	 500l, 3, 1, 0,i);
		sum+=plu_struct_set(idx++,"Unit Weight"		,PTYPE_UNIT_WEIGHT		,'B', 1l,	   3l, 1, 1, 0,i);
		sum+=plu_struct_set(idx++,"FixedWeight"		,PTYPE_FIXED_WEIGHT 	,'L', 0l, 999999l, 6, 1, 0,i);
//		sum+=plu_struct_set(12,"Prefix"			,PTYPE_PREFIX			,'S', 0l,      0l, 2, 1, 0,i);
		sum+=plu_struct_set(idx++,"ItemCode"		,PTYPE_ITEM_CODE		,'L', 0l, 999999l, 6, 1, 0,i);
		sum+=plu_struct_set(idx++,"Piece"			,PTYPE_PIECE			,'W', 0l,	 999l, 3, 1, 0,i);
		sum+=plu_struct_set(idx++,"Qty Unit No."	,PTYPE_QTYUNIT_NO		,'B', 0l,	   8l, 1, 1, 0,i);
		sum+=plu_struct_set(idx++,"Fixed Price"		,PTYPE_FIXED_PRICE		,'B', 0l,	   1l, 1, 1, 0,i);
  #ifdef USE_PRICE_TOTAL_7DIGIT		// Total 7자리 사용
		sum+=plu_struct_set(idx++,"Price"			,PTYPE_PRICE			,'L', 0l,9999999l, 7, 1, 0,i);
  #else
  		sum+=plu_struct_set(idx++,"Price"			,PTYPE_PRICE			,'L', 0l, 999999l, 7, 1, 0,i);
  #endif
  #ifdef USE_PERCENT_WEIGHT
		sum+=plu_struct_set(idx++,"Tax No."			,PTYPE_TAX_NO			,'B', 0l,      9l, 1, 1, 0,i);
		sum+=plu_struct_set(idx++,"%Tare"			,PTYPE_PERCENT_TARE		,'W', 0l,  99999l, 5, 1, 2,i);
		sum+=plu_struct_set(idx++,"reserved"		,PTYPE_RESERVED			,'B', 0l,      0l, 1, 1, 0,i);	// reserve
		sum+=plu_struct_set(idx++,"Tare"			,PTYPE_TARE_VALUE		,'L', 0l,  99999l, 6, 1, 0,i);
		sum+=plu_struct_set(idx++,"Tare No."		,PTYPE_TARE_NO			,'B', 0l,     99l, 2, 1, 0,i);
		sum+=plu_struct_set(idx++,"Barcode No."		,PTYPE_BARCODE_NO		,'W', 0l,     99l, 2, 1, 0,i);
		sum+=plu_struct_set(idx++,"Aux BCode No."	,PTYPE_AUX_BARCODE_NO	,'W', 0l,     99l, 2, 1, 0,i);
		sum+=plu_struct_set(idx++,"ProducedDate"	,PTYPE_PRODUCED_DATE	,'W', 0l,    999l, 3, 1, 0,i);
		sum+=plu_struct_set(idx++,"Packed Date"		,PTYPE_PACKCED_DATE		,'W', 0l,    999l, 3, 1, 0,i);
		sum+=plu_struct_set(idx++,"Packed Time"		,PTYPE_PACKCED_TIME		,'B', 0l,     99l, 2, 1, 0,i);
		sum+=plu_struct_set(idx++,"Sell By Date"	,PTYPE_SELLBY_DATE		,'L', 0l,   9999l, 4, 1, 0,i);
		sum+=plu_struct_set(idx++,"Sell By Time"	,PTYPE_SELLBY_TIME		,'B', 0l,     99l, 2, 1, 0,i);
		sum+=plu_struct_set(idx++,"Ingredient No."	,PTYPE_INGREDIENT_NO	,'W', 0l,     99l, 2, 1, 0,i);
  #ifdef USE_SPAR_MEMBER_PRICE
        sum+=plu_struct_set(idx++,"Member Price"    ,PTYPE_MEMBER_PRICE		,'L', 0l,9999999l, 7, 1, 0,i);
  #else
		sum+=plu_struct_set(idx++,"Traceability No.",PTYPE_TRACE_NO			,'W', 0l,    999l, 3, 1, 0,i);
		sum+=plu_struct_set(idx++,"NutriFact No."	,PTYPE_NUTRITION_NO		,'W', 0l,    500l, 3, 1, 0,i);
  #endif
		sum+=plu_struct_set(idx++,"LabelMsg No."	,PTYPE_LABEL_MSG_NO		,'B', 0l,     99l, 2, 1, 0,i);
	#ifdef USE_RUSSIA_LOGISTICS_PLU
		sum+=plu_struct_set(idx++,"GTIN Type"		,PTYPE_GTIN_TYPE		,'B', 0l,		3l, 1, 1, 0,i);
		sum+=plu_struct_set(idx++,"GTIN Data"		,PTYPE_GTIN_DATA		,'S', 0l,		0l,18, 1, 0,i);
		sum+=plu_struct_set(idx++,"Netweight Dec"	,PTYPE_NETWEIGHT_DECIMAL,'B', 0l,		4l, 1, 1, 0,i);	// 현재 저울 및 CL-Works Pro에서 사용하지 않음(24년 9월 기준)
		sum+=plu_struct_set(idx++,"Contained_unit"	,PTYPE_CONTAINED_UNIT	,'S', 0l,		0l, 8, 1, 0,i);
		sum+=plu_struct_set(idx++,"Lot Number"		,PTYPE_LOT_NUMBER		,'S', 0l,		0l,20, 1, 0,i);
		sum+=plu_struct_set(idx++,"Serial Number"	,PTYPE_SERIAL_NUMBER	,'S', 0l,		0l,20, 1, 0,i);
		sum+=plu_struct_set(idx++,"reserved2"		,PTYPE_RESERVED2		,'S', 0l,		0l,59, 1, 0,i);
		sum+=plu_struct_set(idx++,"reserved3"		,PTYPE_RESERVED3		,'S', 0l,		0l,70, 1, 0,i);
    #endif
  #else	//if dont use percent tare
		sum+=plu_struct_set(idx++,"SpecialPrice"	,PTYPE_SPECIAL_PRICE	,'L', 0l, 999999l, 7, 1, 0,i);
//		sum+=plu_struct_set(19,"Tax No."		,PTYPE_TAX_NO			,'B', 0l,      9l, 1, 1, 0,i);
		sum+=plu_struct_set(idx++,"Tare"			,PTYPE_TARE_VALUE		,'L', 0l,  99999l, 6, 1, 0,i);
		sum+=plu_struct_set(idx++,"Tare No."		,PTYPE_TARE_NO			,'B', 0l,     99l, 2, 1, 0,i);
//		sum+=plu_struct_set(23,"%Tare"			,PTYPE_PERCENT_TARE		,'W', 0l,  99999l, 5, 1, 2,i);
//		sum+=plu_struct_set(24,"Tare % limit"	,PTYPE_PERCENT_TARE_LIMIT,'W', 0l,  99999l, 5, 1, 2,i);
		sum+=plu_struct_set(idx++,"Barcode No."		,PTYPE_BARCODE_NO		,'W', 0l,     99l, 2, 1, 0,i);
		sum+=plu_struct_set(idx++,"Aux BCode No."	,PTYPE_AUX_BARCODE_NO	,'W', 0l,     99l, 2, 1, 0,i);
//		sum+=plu_struct_set(27,"Image No."		,PTYPE_IMAGE_NO			,'B', 0l,     14l, 2, 1, 0,i);
		sum+=plu_struct_set(idx++,"ProducedDate"	,PTYPE_PRODUCED_DATE	,'W', 0l,    999l, 3, 1, 0,i);
		sum+=plu_struct_set(idx++,"Packed Date"		,PTYPE_PACKCED_DATE		,'W', 0l,    999l, 3, 1, 0,i);
		sum+=plu_struct_set(idx++,"Packed Time"		,PTYPE_PACKCED_TIME		,'B', 0l,     99l, 2, 1, 0,i);
		sum+=plu_struct_set(idx++,"Sell By Date"	,PTYPE_SELLBY_DATE		,'L', 0l,   9999l, 4, 1, 0,i);
		sum+=plu_struct_set(idx++,"Sell By Time"	,PTYPE_SELLBY_TIME		,'B', 0l,     99l, 2, 1, 0,i);
//		sum+=plu_struct_set(28,"CookByDate"		,PTYPE_COOKBY_DATE		,'W', 0l,   9999l, 4, 1, 0,i);
		sum+=plu_struct_set(idx++,"Ingredient No."	,PTYPE_INGREDIENT_NO	,'W', 0l,     99l, 2, 1, 0,i);
  #ifdef USE_SPAR_MEMBER_PRICE
        sum+=plu_struct_set(idx++,"Member Price"    ,PTYPE_MEMBER_PRICE     ,'L', 0l,9999999l, 7, 1, 0,i);
  #else
		sum+=plu_struct_set(idx++,"Traceability No.",PTYPE_TRACE_NO			,'W', 0l,    999l, 3, 1, 0,i);
//		sum+=plu_struct_set(36,"Bonus"			,PTYPE_BONUS			,'W', 0l,    999l, 3, 1, 0,i);
		sum+=plu_struct_set(idx++,"NutriFact No."	,PTYPE_NUTRITION_NO		,'W', 0l,    500l, 3, 1, 0,i);
  #endif
		sum+=plu_struct_set(idx++,"LabelMsg No."	,PTYPE_LABEL_MSG_NO		,'B', 0l,     99l, 2, 1, 0,i);
  #endif
//		sum+=plu_struct_set(39,"Reference Dept"	,PTYPE_REF_DEPT_NO		,'W', 0l,     99l, 2, 1, 0,i);
//		sum+=plu_struct_set(40,"Reference PLU"	,PTYPE_REF_PLU_NO		,'L', 0l, 999999l, 6, 1, 0,i);
//		sum+=plu_struct_set(41,"Coupled Dept"	,PTYPE_COUPLE_DEPT_NO	,'W', 0l,     99l, 2, 1, 0,i);
//		sum+=plu_struct_set(42,"Coupled PLU"	,PTYPE_COUPLE_PLU_NO	,'L', 0l, 999999l, 6, 1, 0,i);
//		sum+=plu_struct_set(43,"# of LinkPLU"	,PTYPE_NO_OF_LINK_PLU	,'B', 0l,      2l, 1, 1, 0,i);
//		sum+=plu_struct_set(44,"Link Dept1"		,PTYPE_LINK1_DEPT_NO	,'W', 0l,     99l, 2, 1, 0,i);
//		sum+=plu_struct_set(45,"Link PLU1"		,PTYPE_LINK1_PLU_NO		,'L', 0l, 999999l, 6, 1, 0,i);
//		sum+=plu_struct_set(46,"Link Dept2"		,PTYPE_LINK2_DEPT_NO	,'W', 0l,     99l, 2, 1, 0,i);
//		sum+=plu_struct_set(47,"Link PLU2"		,PTYPE_LINK2_PLU_NO		,'L', 0l, 999999l, 6, 1, 0,i);
  #ifdef USE_DIRECT_INGREDIENT
		sum+=plu_struct_set(idx++,"Ingredient Text",PTYPE_INGREDIENT_TEXT	,'S', 0l,    999l, DIRECT_INGREDIENT_BLOCK_NUM*2, 1, 0,i);  // size : PLU당 가질 수 있는 Direct Block수*2byte
  #else
		sum+=plu_struct_set(idx++,"Ingredient Text",PTYPE_INGREDIENT_TEXT	,'W', 0l,    999l, 3, 1, 0,i);  //SG070727
  #endif
//		sum+=plu_struct_set(31,"Ingredient Text",98
	}
	plu_struct_set_nfield(PLU_NFIELD);
	plu_struct_set_version(PLU_STR_VERSION);	// Added by CJK 20060415
	Flash_flush_nvbuffer();

}
#elif defined(USE_GSMART_PLU)
void plu_struct_set_base(void)
{
	INT16U sum;
	INT8U i;
	INT8U idx;

	for (i=0; i<2; i++) {
		idx = 0;
		sum =plu_struct_set(idx++,"Department No."	,PTYPE_DEPARTMENT_NO	,'W', 1l,     99l, 2, 1, 0,i);	// 0	// 1
		sum+=plu_struct_set(idx++,"PLU No."			,PTYPE_PLU_NO			,'L', 1l, 999999l, 6, 1, 0,i);	// 1	// 2
		sum+=plu_struct_set(idx++,"PLU Type"		,PTYPE_PLU_TYPE			,'M', 1l,      4l, 1, 1, 0,i);	// 2	// 3
		sum+=plu_struct_set(idx++,"Name1 Font Size"	,PTYPE_PLU_NAME1_FONT	,'B', 0l,      3l, 1, 1, 0,i);	// 11
		sum+=plu_struct_set(idx++,"Name1"			,PTYPE_PLU_NAME1		,'S', 0l,      0l,55, 1, 0,i);	// 3	// 4
		sum+=plu_struct_set(idx++,"Name2 Font Size"	,PTYPE_PLU_NAME2_FONT	,'B', 0l,      3l, 1, 1, 0,i);	// 12
		sum+=plu_struct_set(idx++,"Name2"			,PTYPE_PLU_NAME2		,'S', 0l,      0l,55, 1, 0,i);	// 4	// 5
		sum+=plu_struct_set(idx++,"Name3 Font Size"	,PTYPE_PLU_NAME3_FONT	,'B', 0l,      3l, 1, 1, 0,i);	// 13
		sum+=plu_struct_set(idx++,"Name3"			,PTYPE_PLU_NAME3		,'S', 0l,      0l,55, 1, 0,i);	// 5
		sum+=plu_struct_set(idx++,"Name4 Font Size"	,PTYPE_PLU_NAME4_FONT	,'B', 0l,      3l, 1, 1, 0,i);	// 14
		sum+=plu_struct_set(idx++,"Name4"			,PTYPE_PLU_NAME4		,'S', 0l,      0l,55, 1, 0,i);	// 6
		sum+=plu_struct_set(idx++,"Name5 Font Size"	,PTYPE_PLU_NAME5_FONT	,'B', 0l,      3l, 1, 1, 0,i);	// 15
		sum+=plu_struct_set(idx++,"Name5"			,PTYPE_PLU_NAME5		,'S', 0l,      0l,55, 1, 0,i);	// 7
  #if (PLU_COMMODITY_LINE_NO == 8)
		sum+=plu_struct_set(idx++,"Name6 Font Size"	,PTYPE_PLU_NAME6_FONT	,'B', 0l,      3l, 1, 1, 0,i);	// 16
		sum+=plu_struct_set(idx++,"Name6"			,PTYPE_PLU_NAME6		,'S', 0l,      0l,55, 1, 0,i);	// 8
		sum+=plu_struct_set(idx++,"Name7 Font Size"	,PTYPE_PLU_NAME7_FONT	,'B', 0l,      3l, 1, 1, 0,i);	// 17
		sum+=plu_struct_set(idx++,"Name7"			,PTYPE_PLU_NAME7		,'S', 0l,      0l,55, 1, 0,i);	// 9
		sum+=plu_struct_set(idx++,"Name8 Font Size"	,PTYPE_PLU_NAME8_FONT	,'B', 0l,      3l, 1, 1, 0,i);	// 18
		sum+=plu_struct_set(idx++,"Name8"			,PTYPE_PLU_NAME8		,'S', 0l,      0l,55, 1, 0,i);	// 10
  #endif
		sum+=plu_struct_set(idx++,"Group"			,PTYPE_GROUP_NO			,'W', 0l,     99l, 2, 1, 0,i);	// 19
		sum+=plu_struct_set(idx++,"Ext.Barcode"		,PTYPE_EXT_BARCODE		,'S', 0l,      0l,20, 1, 0,i);	// 20
		sum+=plu_struct_set(idx++,"Label No."		,PTYPE_LABEL_NO			,'W', 0l,     99l, 2, 1, 0,i);	// 21
  #ifdef USE_TOPMART_STRUCTURE //sewon 161118
		sum+=plu_struct_set(idx++,"Tr.Use No."		,PTYPE_TR_USE_NO		,'W', 0l,     3l, 1, 1, 0,i);	// 22
  #else
		sum+=plu_struct_set(idx++,"Aux. Label No."	,PTYPE_AUX_LABEL_NO		,'W', 0l,     99l, 2, 1, 0,i);	// 22
  #endif
		sum+=plu_struct_set(idx++,"Origin No."		,PTYPE_ORIGIN_NO		,'W', 0l,    500l, 3, 1, 0,i);	// 23
  #ifdef USE_DISCOUNT_MEMBERSHIP
		sum+=plu_struct_set(idx++,"Discount Rate 1" ,PTYPE_DISCOUNT_RATE1   ,'B', 0l, 	99l, 2, 1, 0,i);  // 24 // 할인율01
  #else
		sum+=plu_struct_set(idx++,"Unit Weight"	  ,PTYPE_UNIT_WEIGHT	  ,'B', 1l, 	 3l, 1, 1, 0,i);  // 24
  #endif
		sum+=plu_struct_set(idx++,"FixedWeight"		,PTYPE_FIXED_WEIGHT		,'L', 0l,  99999l, 6, 1, 0,i);	// 25
		sum+=plu_struct_set(idx++,"Prefix"			,PTYPE_PREFIX			,'S', 0l,      0l, 2, 1, 0,i);	// 26
		sum+=plu_struct_set(idx++,"ItemCode"		,PTYPE_ITEM_CODE		,'L', 0l, 999999l, 6, 1, 0,i);	// 27
  #ifdef USE_DISCOUNT_MEMBERSHIP
		sum+=plu_struct_set(idx++,"End Date"		,PTYPE_DISCOUNT_END_DATE,'W', 0l,   1231l, 4, 1, 0,i);	// 28 //행사종료일
  #else
		sum+=plu_struct_set(idx++,"Piece"			,PTYPE_PIECE			,'W', 0l,    999l, 3, 1, 0,i);	// 28
  #endif
		sum+=plu_struct_set(idx++,"Qty Unit No."	,PTYPE_QTYUNIT_NO		,'B', 0l,     18l, 2, 1, 0,i);	// 29 
//		sum+=plu_struct_set(idx++,"Qty Unit No."	,PTYPE_QTYUNIT_NO		,'B', 0l,      8l, 1, 1, 0,i);	// 29
		sum+=plu_struct_set(idx++,"Fixed Price"		,PTYPE_FIXED_PRICE		,'B', 0l,      1l, 1, 1, 0,i);	// 30
		sum+=plu_struct_set(idx++,"Price"			,PTYPE_PRICE			,'L', 0l, 999999l, 7, 1, 0,i);	// 31
  #ifdef USE_TOPMART_STRUCTURE //sewon 161118
		sum+=plu_struct_set(idx++,"Tr. sell by date",PTYPE_TR_SELLBY_DATE	,'L', 0l, 999l, 3, 1, 0,i);		// 32
		sum+=plu_struct_set(idx++,"Tr. Comp. Flag"	,PTYPE_TR_COMP_FLAG		,'B', 0l,	3l, 1, 1, 0,i);		// 33
  #else		
		sum+=plu_struct_set(idx++,"SpecialPrice"	,PTYPE_SPECIAL_PRICE	,'L', 0l, 999999l, 7, 1, 0,i);	// 32
		sum+=plu_struct_set(idx++,"Tr. Flag"		,PTYPE_TRACE_FLAG		,'B', 0l,      1l, 1, 1, 0,i);	// 33
  #endif
  #ifdef USE_TRACE_NUM_CHECK_FUNCTION
		sum+=plu_struct_set(idx++,"TraceNum Check"	,PTYPE_TRACE_NUM_CHECK	,'B', 0l,      9l, 1, 1, 0,i);	// 33	// 이력번호체크
  #else
		sum+=plu_struct_set(idx++,"Tax No."			,PTYPE_TAX_NO			,'B', 0l,      9l, 1, 1, 0,i);	// 33
  #endif
  #ifdef USE_DISCOUNT_MEMBERSHIP
		sum+=plu_struct_set(idx++,"Start Date"		,PTYPE_DISCOUNT_START_DATE,'L', 0l,   1231l, 4, 1, 0,i);// 34 //행사시작일
		sum+=plu_struct_set(idx++,"Discount Rate 2"	,PTYPE_DISCOUNT_RATE2	,'B', 0l,     99l, 2, 1, 0,i);	// 35 //할인율02
  #else
		sum+=plu_struct_set(idx++,"Tare"			,PTYPE_TARE_VALUE		,'L', 0l,  99999l, 6, 1, 0,i);	// 34
		sum+=plu_struct_set(idx++,"Tare No."		,PTYPE_TARE_NO			,'B', 0l,     99l, 2, 1, 0,i);	// 35
  #endif
//		sum+=plu_struct_set(idx++,"%Tare"			,PTYPE_PERCENT_TARE		,'W', 0l,  99999l, 5, 1, 2,i);	// 36
//		sum+=plu_struct_set(idx++,"Tare % limit"	,PTYPE_PERCENT_TARE_LIMIT,'W', 0l,  99999l, 5, 1, 2,i);	// 37
  #if defined (USE_BARCODE_LEN_60) || defined (USE_BARCODE_LEN_70)
		sum+=plu_struct_set(idx++,"Barcode No."		,PTYPE_BARCODE_NO		,'W', 0l,     59l, 2, 1, 0,i);	// 38
  #elif defined (USE_BARCODE_LEN_50)
		sum+=plu_struct_set(idx++,"Barcode No."		,PTYPE_BARCODE_NO		,'W', 0l,     79l, 2, 1, 0,i);	// 38
  #elif defined (USE_BARCODE_LEN_90)
		sum+=plu_struct_set(idx++,"Barcode No."		,PTYPE_BARCODE_NO		,'W', 0l,     39l, 2, 1, 0,i);	// 38
  #elif defined (USE_BARCODE_LEN_50_DES_20)
		sum+=plu_struct_set(idx++,"Barcode No."	  ,PTYPE_BARCODE_NO 	  ,'W', 0l, 	69l, 2, 1, 0,i);  // 38
  #else
		sum+=plu_struct_set(idx++,"Barcode No."		,PTYPE_BARCODE_NO		,'W', 0l,     99l, 2, 1, 0,i);	// 38
  #endif
  #ifdef USE_TOPMART_STRUCTURE //sewon 161118
		sum+=plu_struct_set(idx++,"Kind No."		  ,PTYPE_KIND_NO		  ,'W', 0l, 	99l, 2, 1, 0,i);  // 39
		sum+=plu_struct_set(idx++,"Max Qty."		  ,PTYPE_MAX_QTY		  ,'B', 0l, 	99l, 2, 1, 0,i);  // 40
  #elif defined USE_DISCOUNT_MEMBERSHIP
  		sum+=plu_struct_set(idx++,"Aux BCode No."   ,PTYPE_AUX_BARCODE_NO   ,'W', 0l, 	99l, 2, 1, 0,i);  	// 39
	  	sum+=plu_struct_set(idx++,"Discount Type"	,PTYPE_DISCOUNT_TYPE	,'B', 0l,     99l, 2, 1, 0,i);	// 40 //행사유형
  #else
	  	sum+=plu_struct_set(idx++,"Aux BCode No."   ,PTYPE_AUX_BARCODE_NO   ,'W', 0l, 	99l, 2, 1, 0,i);  // 39
	#ifdef USE_BESTCO_UNIT_PRICE_CATEGORY
		sum+=plu_struct_set(idx++,"Unit Price Category"   ,PTYPE_UNIT_PRICE_CATEGORY ,'B', 0l, 	50l, 2, 1, 0,i);  // 40	// 판매단가구분
	#else
	  	sum+=plu_struct_set(idx++,"Image No." 	  ,PTYPE_IMAGE_NO		  ,'B', 0l, 	14l, 2, 1, 0,i);  // 40
	#endif
  #endif
//		sum+=plu_struct_set(idx++,"ProducedDate"	,PTYPE_PRODUCED_DATE	,'W', 0l,    999l, 3, 1, 0,i);	// 41
		sum+=plu_struct_set(idx++,"Packed Date flag",PTYPE_PACKCED_DATE_FLAG,'B', 0l,      3l, 1, 1, 0,i);	// 42
		sum+=plu_struct_set(idx++,"Packed Date"		,PTYPE_PACKCED_DATE		,'W', 0l,    999l, 3, 1, 0,i);	// 43
		sum+=plu_struct_set(idx++,"Packed Time flag",PTYPE_PACKCED_TIME_FLAG,'B', 0l,      3l, 1, 1, 0,i);	// 44
		sum+=plu_struct_set(idx++,"Packed Time"		,PTYPE_PACKCED_TIME		,'W', 0l,   2359l, 4, 1, 0,i);	// 45
		sum+=plu_struct_set(idx++,"Sell By Date flag",PTYPE_SELLBY_DATE_FLAG,'B', 0l,      2l, 1, 1, 0,i);	// 46
		sum+=plu_struct_set(idx++,"Sell By Date"	,PTYPE_SELLBY_DATE		,'L', 0l,   9999l, 4, 1, 0,i);	// 47
		sum+=plu_struct_set(idx++,"Sell By Time flag",PTYPE_SELLBY_TIME_FLAG,'B', 0l,      3l, 1, 1, 0,i);	// 48
		sum+=plu_struct_set(idx++,"Sell By Time"	,PTYPE_SELLBY_TIME		,'W', 0l,   2359l, 4, 1, 0,i);	// 49
//		sum+=plu_struct_set(idx++,"CookByDate"		,PTYPE_COOKBY_DATE		,'W', 0l,   9999l, 4, 1, 0,i);	// 50
#ifdef USE_VIETNAM_LOTTEMART_DEFAULT
		sum+=plu_struct_set(idx++,"Ingredient No."	,PTYPE_INGREDIENT_NO	,'W', 0l,    800l, 4, 1, 0,i);	// 51
#else
		sum+=plu_struct_set(idx++,"Ingredient No."	,PTYPE_INGREDIENT_NO	,'W', 0l,    999l, 4, 1, 0,i);	// 51
#endif
//		sum+=plu_struct_set(idx++,"Traceability No.",PTYPE_TRACE_NO			,'W', 0l,    999l, 3, 1, 0,i);	// 52
//		sum+=plu_struct_set(idx++,"Bonus"			,PTYPE_BONUS			,'W', 0l,    999l, 3, 1, 0,i);	// 53
		sum+=plu_struct_set(idx++,"NutriFact No."	,PTYPE_NUTRITION_NO		,'W', 0l,    500l, 3, 1, 0,i);	// 54
#ifdef USE_HYUNDAI_CHECK_REASON_CODE
		sum+=plu_struct_set(idx++,"LabelMsg No."	,PTYPE_LABEL_MSG_NO 	,'B', 0l,	   9l, 1, 1, 0,i);	// 55
#elif defined(USE_VIETNAM_LOTTEMART_DEFAULT)
		sum+=plu_struct_set(idx++,"LabelMsg No."	,PTYPE_LABEL_MSG_NO 	,'B', 0l,	   30l, 2, 1, 0,i);	// 55
#else
		sum+=plu_struct_set(idx++,"LabelMsg No."	,PTYPE_LABEL_MSG_NO		,'B', 0l,     99l, 2, 1, 0,i);	// 55
#endif
//		sum+=plu_struct_set(idx++,"Reference Dept"	,PTYPE_REF_DEPT_NO		,'W', 0l,     99l, 2, 1, 0,i);	// 56
//		sum+=plu_struct_set(idx++,"Reference PLU"	,PTYPE_REF_PLU_NO		,'L', 0l, 999999l, 6, 1, 0,i);	// 57
//		sum+=plu_struct_set(idx++,"Coupled Dept"	,PTYPE_COUPLE_DEPT_NO	,'W', 0l,     99l, 2, 1, 0,i);	// 58
//		sum+=plu_struct_set(idx++,"Coupled PLU"		,PTYPE_COUPLE_PLU_NO	,'L', 0l, 999999l, 6, 1, 0,i);	// 59
//		sum+=plu_struct_set(idx++,"# of LinkPLU"	,PTYPE_NO_OF_LINK_PLU	,'B', 0l,      2l, 1, 1, 0,i);	// 60
//		sum+=plu_struct_set(idx++,"Link Dept1"		,PTYPE_LINK1_DEPT_NO	,'W', 0l,     99l, 2, 1, 0,i);	// 61
//		sum+=plu_struct_set(idx++,"Link PLU1"		,PTYPE_LINK1_PLU_NO		,'L', 0l, 999999l, 6, 1, 0,i);	// 62
//		sum+=plu_struct_set(idx++,"Link Dept2"		,PTYPE_LINK2_DEPT_NO	,'W', 0l,     99l, 2, 1, 0,i);	// 63
//		sum+=plu_struct_set(idx++,"Link PLU2"		,PTYPE_LINK2_PLU_NO		,'L', 0l, 999999l, 6, 1, 0,i);	// 64
	}
	plu_struct_set_nfield(PLU_NFIELD);
	plu_struct_set_version(PLU_STR_VERSION);	// Added by CJK 20060415
	Flash_flush_nvbuffer();

}
#elif defined(USE_CHINA_BAONENG) || defined(USE_CHINA_BAONENG_COUNTRYGARDEN)
void plu_struct_set_base(void)
{
	INT16U sum;
	INT8U i;
	INT8U idx;

	for (i=0; i<2; i++) {
		idx = 0;
		sum =plu_struct_set(idx++,"Department No."	,PTYPE_DEPARTMENT_NO		,'W', 1l,     99l, 2, 1, 0,i);	// 0	// 1
		sum+=plu_struct_set(idx++,"PLU No."			,PTYPE_PLU_NO				,'L', 1l, 999999l, 6, 1, 0,i);	// 1	// 2
		sum+=plu_struct_set(idx++,"PLU Type"		,PTYPE_PLU_TYPE				,'M', 1l,      4l, 1, 1, 0,i);	// 2	// 3
		sum+=plu_struct_set(idx++,"Name "			,PTYPE_PLU_NAME1			,'S', 0l,      0l,55, 1, 0,i);	// 3	// 4
		sum+=plu_struct_set(idx++,"Name2"			,PTYPE_PLU_NAME2			,'S', 0l,      0l,55, 1, 0,i);	// 4	// 5
		sum+=plu_struct_set(idx++,"Name3"			,PTYPE_PLU_NAME3			,'S', 0l,      0l,55, 1, 0,i);	// 5
		sum+=plu_struct_set(idx++,"Name5"			,PTYPE_PLU_NAME4			,'S', 0l,      0l,55, 1, 0,i);	// 5		
		sum+=plu_struct_set(idx++,"Name6"			,PTYPE_PLU_NAME5			,'S', 0l,      0l,55, 1, 0,i);	// 5		
		sum+=plu_struct_set(idx++,"Name7"			,PTYPE_PLU_NAME6			,'S', 0l,      0l,55, 1, 0,i);	// 5		
		sum+=plu_struct_set(idx++,"Name8"			,PTYPE_PLU_NAME7			,'S', 0l,      0l,55, 1, 0,i);	// 5		
		sum+=plu_struct_set(idx++,"Name9"			,PTYPE_PLU_NAME8			,'S', 0l,      0l,55, 1, 0,i);	// 5		
		sum+=plu_struct_set(idx++,"Group"			,PTYPE_GROUP_NO				,'W', 0l,     99l, 2, 1, 0,i);	// 6
		sum+=plu_struct_set(idx++,"Ext.Barcode"		,PTYPE_EXT_BARCODE			,'S', 0l,      0l,20, 1, 0,i);	// 7
		sum+=plu_struct_set(idx++,"Label No."		,PTYPE_LABEL_NO				,'W', 0l,     99l, 2, 1, 0,i);	// 8
		sum+=plu_struct_set(idx++,"Aux. Label No."	,PTYPE_AUX_LABEL_NO			,'W', 0l,     99l, 2, 1, 0,i);	// 9
		sum+=plu_struct_set(idx++,"Origin No."		,PTYPE_ORIGIN_NO			,'W', 0l,    500l, 3, 1, 0,i);	// 10
		sum+=plu_struct_set(idx++,"Unit Weight"		,PTYPE_UNIT_WEIGHT			,'B', 1l,      3l, 1, 1, 0,i);	// 11
		sum+=plu_struct_set(idx++,"FixedWeight"		,PTYPE_FIXED_WEIGHT			,'L', 0l,  99999l, 6, 1, 0,i);	// 12
		sum+=plu_struct_set(idx++,"Prefix"			,PTYPE_PREFIX				,'S', 0l,      0l, 2, 1, 0,i);	// 13
		sum+=plu_struct_set(idx++,"ItemCode"		,PTYPE_ITEM_CODE			,'L', 0l, 999999l, 6, 1, 0,i);	// 14
		sum+=plu_struct_set(idx++,"Piece"			,PTYPE_PIECE				,'W', 0l,    999l, 3, 1, 0,i);	// 15
		sum+=plu_struct_set(idx++,"Qty Unit No."	,PTYPE_QTYUNIT_NO			,'B', 0l,      8l, 1, 1, 0,i);	// 16
		sum+=plu_struct_set(idx++,"Fixed Price"		,PTYPE_FIXED_PRICE			,'B', 0l,      1l, 1, 1, 0,i);	// 17
		sum+=plu_struct_set(idx++,"Price"			,PTYPE_PRICE				,'L', 0l, 999999l, 7, 1, 0,i);	// 18
		sum+=plu_struct_set(idx++,"SpecialPrice"	,PTYPE_SPECIAL_PRICE		,'L', 0l, 999999l, 7, 1, 0,i);	// 19
		sum+=plu_struct_set(idx++,"Tax No."			,PTYPE_TAX_NO				,'B', 0l,      9l, 1, 1, 0,i);	// 20
		sum+=plu_struct_set(idx++,"Tare"			,PTYPE_TARE_VALUE			,'L', 0l,  99999l, 6, 1, 0,i);	// 21
		sum+=plu_struct_set(idx++,"Tare No."		,PTYPE_TARE_NO				,'B', 0l,     99l, 2, 1, 0,i);	// 22
		sum+=plu_struct_set(idx++,"%Tare"			,PTYPE_PERCENT_TARE			,'W', 0l,  99999l, 5, 1, 2,i);	// 23
		sum+=plu_struct_set(idx++,"Tare % limit"	,PTYPE_PERCENT_TARE_LIMIT	,'W', 0l,  99999l, 5, 1, 2,i);	// 24
		sum+=plu_struct_set(idx++,"Barcode No."		,PTYPE_BARCODE_NO			,'W', 0l,     99l, 2, 1, 0,i);	// 25 //
		sum+=plu_struct_set(idx++,"Aux BCode No."	,PTYPE_AUX_BARCODE_NO		,'W', 0l,     99l, 2, 1, 0,i);	// 26
		sum+=plu_struct_set(idx++,"Image No."		,PTYPE_IMAGE_NO				,'B', 0l,     14l, 2, 1, 0,i);	// 27
		sum+=plu_struct_set(idx++,"ProducedDate"	,PTYPE_PRODUCED_DATE		,'W', 0l,    999l, 3, 1, 0,i);	// 28
		sum+=plu_struct_set(idx++,"Packed Date"		,PTYPE_PACKCED_DATE			,'W', 0l,    999l, 3, 1, 0,i);	// 29
		sum+=plu_struct_set(idx++,"Packed Time"		,PTYPE_PACKCED_TIME			,'B', 0l,     99l, 2, 1, 0,i);	// 30
		sum+=plu_struct_set(idx++,"Sell By Date"	,PTYPE_SELLBY_DATE			,'L', 0l,   9999l, 4, 1, 0,i);	// 31
		sum+=plu_struct_set(idx++,"Sell By Time"	,PTYPE_SELLBY_TIME			,'B', 0l,     99l, 2, 1, 0,i);	// 32
		sum+=plu_struct_set(idx++,"CookByDate"		,PTYPE_COOKBY_DATE			,'W', 0l,   9999l, 4, 1, 0,i);	// 33
		sum+=plu_struct_set(idx++,"Ingredient No."	,PTYPE_INGREDIENT_NO		,'W', 0l,    999l, 4, 1, 0,i);	// 34
		sum+=plu_struct_set(idx++,"Traceability No.",PTYPE_TRACE_NO				,'W', 0l,    999l, 3, 1, 0,i);	// 35
		sum+=plu_struct_set(idx++,"Bonus"			,PTYPE_BONUS				,'W', 0l,    999l, 3, 1, 0,i);	// 36
		sum+=plu_struct_set(idx++,"NutriFact No."	,PTYPE_NUTRITION_NO			,'W', 0l,    500l, 3, 1, 0,i);	// 37
		sum+=plu_struct_set(idx++,"LabelMsg No."	,PTYPE_LABEL_MSG_NO			,'B', 0l,     99l, 2, 1, 0,i);	// 38
		sum+=plu_struct_set(idx++,"Ingredient Text"	,PTYPE_INGREDIENT_TEXT		,'S', 0l,    999l, DIRECT_INGREDIENT_BLOCK_NUM*2, 1, 0,i);	// 48 Dynamic direct Ingredient)
	}
	plu_struct_set_nfield(PLU_NFIELD);
	plu_struct_set_version(PLU_STR_VERSION);	// Added by CJK 20060415
	Flash_flush_nvbuffer();
	plu_struct_get_nfield();
}
#else
void plu_struct_set_base(void)
{
	INT16U sum;
	INT8U i;
	INT8U idx;

	for (i=0; i<2; i++) {
		idx = 0;
		sum =plu_struct_set(idx++,"Department No."	,PTYPE_DEPARTMENT_NO		,'W', 1l,     99l, 2, 1, 0,i);	// 0	// 1
  #ifdef USE_CHN_CART_SCALE
		sum+=plu_struct_set(idx++,"PLU No."			,PTYPE_PLU_NO				,'L', 1l,    999l, 3, 1, 0,i);	// 1	// 2
  #else
		sum+=plu_struct_set(idx++,"PLU No."			,PTYPE_PLU_NO				,'L', 1l, 999999l, 6, 1, 0,i);	// 1	// 2
  #endif
		sum+=plu_struct_set(idx++,"PLU Type"		,PTYPE_PLU_TYPE				,'M', 1l,      4l, 1, 1, 0,i);	// 2	// 3
		sum+=plu_struct_set(idx++,"Name "			,PTYPE_PLU_NAME1			,'S', 0l,      0l,40, 1, 0,i);	// 3	// 4
		sum+=plu_struct_set(idx++,"Name2"			,PTYPE_PLU_NAME2			,'S', 0l,      0l,40, 1, 0,i);	// 4	// 5
		sum+=plu_struct_set(idx++,"Name3"			,PTYPE_PLU_NAME3			,'S', 0l,      0l,40, 1, 0,i);	// 5
		sum+=plu_struct_set(idx++,"Group"			,PTYPE_GROUP_NO				,'W', 0l,     99l, 2, 1, 0,i);	// 6
		sum+=plu_struct_set(idx++,"Ext.Barcode"		,PTYPE_EXT_BARCODE			,'S', 0l,      0l,20, 1, 0,i);	// 7
		sum+=plu_struct_set(idx++,"Label No."		,PTYPE_LABEL_NO				,'W', 0l,     99l, 2, 1, 0,i);	// 8
		sum+=plu_struct_set(idx++,"Aux. Label No."	,PTYPE_AUX_LABEL_NO			,'W', 0l,     99l, 2, 1, 0,i);	// 9
		sum+=plu_struct_set(idx++,"Origin No."		,PTYPE_ORIGIN_NO			,'W', 0l,    500l, 3, 1, 0,i);	// 10
		sum+=plu_struct_set(idx++,"Unit Weight"		,PTYPE_UNIT_WEIGHT			,'B', 1l,      3l, 1, 1, 0,i);	// 11
		sum+=plu_struct_set(idx++,"FixedWeight"		,PTYPE_FIXED_WEIGHT			,'L', 0l,  99999l, 6, 1, 0,i);	// 12
		sum+=plu_struct_set(idx++,"Prefix"			,PTYPE_PREFIX				,'S', 0l,      0l, 2, 1, 0,i);	// 13
		sum+=plu_struct_set(idx++,"ItemCode"		,PTYPE_ITEM_CODE			,'L', 0l, 999999l, 6, 1, 0,i);	// 14
		sum+=plu_struct_set(idx++,"Piece"			,PTYPE_PIECE				,'W', 0l,    999l, 3, 1, 0,i);	// 15
		sum+=plu_struct_set(idx++,"Qty Unit No."	,PTYPE_QTYUNIT_NO			,'B', 0l,      8l, 1, 1, 0,i);	// 16
		sum+=plu_struct_set(idx++,"Fixed Price"		,PTYPE_FIXED_PRICE			,'B', 0l,      1l, 1, 1, 0,i);	// 17
#ifdef USE_PRICE_UNIT_7DIGIT
        sum+=plu_struct_set(idx++,"Price"           ,PTYPE_PRICE                ,'L', 0l, 9999999l, 7, 1, 0,i); // 18
#else
        sum+=plu_struct_set(idx++,"Price"			,PTYPE_PRICE				,'L', 0l, 999999l, 7, 1, 0,i);	// 18
#endif
		sum+=plu_struct_set(idx++,"SpecialPrice"	,PTYPE_SPECIAL_PRICE		,'L', 0l, 999999l, 7, 1, 0,i);	// 19
		sum+=plu_struct_set(idx++,"Tax No."			,PTYPE_TAX_NO				,'B', 0l,      9l, 1, 1, 0,i);	// 20
		sum+=plu_struct_set(idx++,"Tare"			,PTYPE_TARE_VALUE			,'L', 0l,  99999l, 6, 1, 0,i);	// 21
		sum+=plu_struct_set(idx++,"Tare No."		,PTYPE_TARE_NO				,'B', 0l,     99l, 2, 1, 0,i);	// 22
		sum+=plu_struct_set(idx++,"%Tare"			,PTYPE_PERCENT_TARE			,'W', 0l,  99999l, 5, 1, 2,i);	// 23
		sum+=plu_struct_set(idx++,"Tare % limit"	,PTYPE_PERCENT_TARE_LIMIT	,'W', 0l,  99999l, 5, 1, 2,i);	// 24
		sum+=plu_struct_set(idx++,"Barcode No."		,PTYPE_BARCODE_NO			,'W', 0l,     99l, 2, 1, 0,i);	// 25 //
		sum+=plu_struct_set(idx++,"Aux BCode No."	,PTYPE_AUX_BARCODE_NO		,'W', 0l,     99l, 2, 1, 0,i);	// 26
		sum+=plu_struct_set(idx++,"Image No."		,PTYPE_IMAGE_NO				,'B', 0l,     14l, 2, 1, 0,i);	// 27
		sum+=plu_struct_set(idx++,"ProducedDate"	,PTYPE_PRODUCED_DATE		,'W', 0l,    999l, 3, 1, 0,i);	// 28
		sum+=plu_struct_set(idx++,"Packed Date"		,PTYPE_PACKCED_DATE			,'W', 0l,    999l, 3, 1, 0,i);	// 29
		sum+=plu_struct_set(idx++,"Packed Time"		,PTYPE_PACKCED_TIME			,'B', 0l,     99l, 2, 1, 0,i);	// 30
		sum+=plu_struct_set(idx++,"Sell By Date"	,PTYPE_SELLBY_DATE			,'L', 0l,   9999l, 4, 1, 0,i);	// 31
		sum+=plu_struct_set(idx++,"Sell By Time"	,PTYPE_SELLBY_TIME			,'B', 0l,     99l, 2, 1, 0,i);	// 32
		sum+=plu_struct_set(idx++,"CookByDate"		,PTYPE_COOKBY_DATE			,'W', 0l,   9999l, 4, 1, 0,i);	// 33
		sum+=plu_struct_set(idx++,"Ingredient No."	,PTYPE_INGREDIENT_NO		,'W', 0l,    999l, 4, 1, 0,i);	// 34
		sum+=plu_struct_set(idx++,"Traceability No.",PTYPE_TRACE_NO				,'W', 0l,    999l, 3, 1, 0,i);	// 35
  #ifdef USE_CHINA_CARRFOUR_DEFAULT
		sum+=plu_struct_set(idx++,"Price Type"		   ,	50,'W', 0l,    9l, 1, 1, 0,i);	// 40
  #else
		sum+=plu_struct_set(idx++,"Bonus"			,PTYPE_BONUS				,'W', 0l,    999l, 3, 1, 0,i);	// 36
  #endif
		sum+=plu_struct_set(idx++,"NutriFact No."	,PTYPE_NUTRITION_NO			,'W', 0l,    500l, 3, 1, 0,i);	// 37
		sum+=plu_struct_set(idx++,"LabelMsg No."	,PTYPE_LABEL_MSG_NO			,'B', 0l,     99l, 2, 1, 0,i);	// 38
  #ifdef USE_EUR_LOGISTICS_PLU
		sum+=plu_struct_set(idx++,"GTIN Type"		,PTYPE_GTIN_TYPE			,'B', 0l,		3l, 1, 1, 0,i);
		sum+=plu_struct_set(idx++,"GTIN Data"		,PTYPE_GTIN_DATA			,'S', 0l,		0l,18, 1, 0,i);
		sum+=plu_struct_set(idx++,"Netweight Dec"	,PTYPE_NETWEIGHT_DECIMAL	,'B', 0l,		4l, 1, 1, 0,i);	// 현재 저울 및 CL-Works Pro에서 사용하지 않음(24년 9월 기준)
		sum+=plu_struct_set(idx++,"Contained_unit"	,PTYPE_CONTAINED_UNIT		,'S', 0l,		0l, 8, 1, 0,i);
		sum+=plu_struct_set(idx++,"Lot Number"		,PTYPE_LOT_NUMBER			,'S', 0l,		0l,20, 1, 0,i);
		sum+=plu_struct_set(idx++,"Serial Number"	,PTYPE_SERIAL_NUMBER		,'S', 0l,		0l,20, 1, 0,i);
  #endif
  #ifdef USE_DIRECT_INGREDIENT
	#if defined(USE_USA_INGREDIENT) || defined(USE_NORTH_AMERICA_INGREDIENT)
		sum+=plu_struct_set(idx++,"Ingredient Text"	,PTYPE_INGREDIENT_TEXT		,'S', 0l,    999l, DIRECT_INGREDIENT_BLOCK_NUM*2, 1, 0,i);	// 39 Dynamic direct Ingredient)	
	#else
		sum+=plu_struct_set(idx++,"Reference Dept"	,PTYPE_REF_DEPT_NO			,'W', 0l,     99l, 2, 1, 0,i);	// 39
		sum+=plu_struct_set(idx++,"Reference PLU"	,PTYPE_REF_PLU_NO			,'L', 0l, 999999l, 6, 1, 0,i);	// 40
		sum+=plu_struct_set(idx++,"Coupled Dept"	,PTYPE_COUPLE_DEPT_NO		,'W', 0l,     99l, 2, 1, 0,i);	// 41
		sum+=plu_struct_set(idx++,"Coupled PLU"		,PTYPE_COUPLE_PLU_NO		,'L', 0l, 999999l, 6, 1, 0,i);	// 42
		sum+=plu_struct_set(idx++,"# of LinkPLU"	,PTYPE_NO_OF_LINK_PLU		,'B', 0l,      2l, 1, 1, 0,i);	// 43
		sum+=plu_struct_set(idx++,"Link Dept1"		,PTYPE_LINK1_DEPT_NO		,'W', 0l,     99l, 2, 1, 0,i);	// 44
		sum+=plu_struct_set(idx++,"Link PLU1"		,PTYPE_LINK1_PLU_NO			,'L', 0l, 999999l, 6, 1, 0,i);	// 45
		sum+=plu_struct_set(idx++,"Link Dept2"		,PTYPE_LINK2_DEPT_NO		,'W', 0l,     99l, 2, 1, 0,i);	// 46
		sum+=plu_struct_set(idx++,"Link PLU2"		,PTYPE_LINK2_PLU_NO			,'L', 0l, 999999l, 6, 1, 0,i);	// 47
		sum+=plu_struct_set(idx++,"Ingredient Text"	,PTYPE_INGREDIENT_TEXT		,'S', 0l,    999l, DIRECT_INGREDIENT_BLOCK_NUM*2, 1, 0,i);	// 48 Dynamic direct Ingredient)
	#endif
  #else
		sum+=plu_struct_set(idx++,"Reference Dept"	,PTYPE_REF_DEPT_NO			,'W', 0l,     99l, 2, 1, 0,i);	// 39
		sum+=plu_struct_set(idx++,"Reference PLU"	,PTYPE_REF_PLU_NO			,'L', 0l, 999999l, 6, 1, 0,i);	// 40
		sum+=plu_struct_set(idx++,"Coupled Dept"	,PTYPE_COUPLE_DEPT_NO		,'W', 0l,     99l, 2, 1, 0,i);	// 41
		sum+=plu_struct_set(idx++,"Coupled PLU"		,PTYPE_COUPLE_PLU_NO		,'L', 0l, 999999l, 6, 1, 0,i);	// 42
		sum+=plu_struct_set(idx++,"# of LinkPLU"	,PTYPE_NO_OF_LINK_PLU		,'B', 0l,      2l, 1, 1, 0,i);	// 43
		sum+=plu_struct_set(idx++,"Link Dept1"		,PTYPE_LINK1_DEPT_NO		,'W', 0l,     99l, 2, 1, 0,i);	// 44
		sum+=plu_struct_set(idx++,"Link PLU1"		,PTYPE_LINK1_PLU_NO			,'L', 0l, 999999l, 6, 1, 0,i);	// 45
		sum+=plu_struct_set(idx++,"Link Dept2"		,PTYPE_LINK2_DEPT_NO		,'W', 0l,     99l, 2, 1, 0,i);	// 46
		sum+=plu_struct_set(idx++,"Link PLU2"		,PTYPE_LINK2_PLU_NO			,'L', 0l, 999999l, 6, 1, 0,i);	// 47
	#ifdef USE_CHINA_CARRFOUR_DEFAULT
		sum+=plu_struct_set(idx++,"Traceability URL"	,PTYPE_TRACE_URL		,'S', 0l,     0l,128, 1, 0,i);	// 48
		sum+=plu_struct_set(idx++,"Traceability Code"	,PTYPE_TRACE_CODE		,'S', 0l,     0l, 20, 1, 0,i);	// 49
	#endif
  #endif
	}
	plu_struct_set_nfield(PLU_NFIELD);
	plu_struct_set_version(PLU_STR_VERSION);	// Added by CJK 20060415
	Flash_flush_nvbuffer();
	plu_struct_get_nfield();

}
#endif

INT16U plu_struct_set(INT8U s_id,char *caption,INT16U ptype,INT8U stype,
		    INT32U vdefault,INT32U maxvalue,
		    INT8U length,INT8U lineno,INT8U dpoint,INT8U mode)
{
	INT16U usize;
	INT16S k;
	INT32U pnt,npnt;
	INT8U  plustr[PLU_STRUCT_SIZE];
	INT8U  plustr_name[PLUSTR_NAME_SIZE];

	if (mode==0) {
		usize = 1;
		switch (stype) {
			case 'B': usize=1; break;		// Byte
			case 'S': usize=length*lineno; break;	// String
			case 'W': usize=2; break;		// Word
			case 'L': usize=4; break;		// Long
			case 'D': usize=3; break;		// Date
			case 'T': usize=3; break;		// Time
			case 'M': usize=1; break;		// Menu
			case 'C': usize=length; break;		// BCD
		}
		pnt = DFLASH_BASE;
		pnt+= FLASH_PLU_STRUCT_AREA;
		pnt+= PLU_STRUCT_POINT;
		pnt+= PLU_STRUCT_SIZE*s_id;
	}
	k=0;
	if (mode==1) {
		memset(plustr_name,0,PLUSTR_NAME_SIZE);
		npnt = DFLASH_BASE;
		npnt+= FLASH_PLU_STRUCT_CAPTION_AREA;
		npnt+= PLUSTR_NAME_SIZE*s_id;
		strcpy((char *)plustr_name,caption);
		Flash_swrite(npnt,plustr_name,PLUSTR_NAME_SIZE);
	} else {
		memcpy((INT8U*)&plustr[k],(INT8U*)&ptype   ,2); k+=2;
		memcpy((INT8U*)&plustr[k],(INT8U*)&vdefault,4); k+=4;
		memcpy((INT8U*)&plustr[k],(INT8U*)&maxvalue,4); k+=4;
		plustr[k++]= stype;
		plustr[k++]=length;
		plustr[k++]=lineno;
		plustr[k++]=dpoint;

		memcpy((INT8U*)&plustr[k],(INT8U*)&usize,2); k+=2;
		Flash_swrite(pnt,plustr,PLU_STRUCT_SIZE);
	}
	return usize;
}

INT16S plu_copy_move_device(INT16S from_dept, long from_plu, INT16S to_dept, long to_plu, INT8U flag)
{
	char ch;
	INT16S i;
	long pnt;
	INT8U  stype;
	INT16U usize,idx;
	INT32U addr_source, addr_target,key;
	INT16U dc_no;
	DISCOUNT_STRUCT	 disc_str;
#ifdef USE_DIRECT_INGREDIENT
	INT16U total_block_src;
	INT16U total_block_tar;
	INT16U ingred_no_src[DIRECT_INGREDIENT_BLOCK_NUM];
	INT16U ingred_no_tar[DIRECT_INGREDIENT_BLOCK_NUM];
	INT8U dmsg[DEFAULT_INGREDIENT_TEXT_ALL_SIZE+1];
#endif

	if ((from_dept==to_dept) && (from_plu==to_plu)) return 1; // Self Copy or Move
	if (!plu_table_search((INT8U)from_dept, from_plu,&idx,0)) {
		return 0;
	}
	key = (INT32U)idx;
	addr_source=plu_calc_addr_by_key((long)(key-1));
#ifdef USE_DIRECT_INGREDIENT
	pnt = plu_get_ptype_point(PTYPE_INGREDIENT_TEXT, &stype, &usize);	// direct ingredient point
#endif
	if (!plu_table_search((INT8U)to_dept, to_plu,(INT16U *)&idx,0)) {
		idx = plu_address_get_new();
		if (idx==0) {
			return 9;
		}
		key = (INT32U)idx;
		addr_target=plu_calc_addr_by_key(key-1);
#ifdef USE_DIRECT_INGREDIENT
		memset((HUGEDATA INT8U *)&ingred_no_tar[0], 0, 2 * DIRECT_INGREDIENT_BLOCK_NUM);
#endif
	}  else {
		key = (INT32U)idx;
		addr_target = plu_calc_addr_by_key((long)(key-1));
#ifdef USE_DIRECT_INGREDIENT
		Flash_sread(addr_target + pnt, (HUGEDATA INT8U *)&ingred_no_tar[0], 2 * DIRECT_INGREDIENT_BLOCK_NUM);
#endif
	}

#ifdef USE_DIRECT_INGREDIENT
	// read direct ingredient no
	//pnt = plu_get_ptype_point(PTYPE_INGREDIENT_TEXT, &stype, &usize);	// direct ingredient point
	Flash_sread(addr_source + pnt, (HUGEDATA INT8U *)&ingred_no_src[0], 2 * DIRECT_INGREDIENT_BLOCK_NUM);
	//Flash_sread(addr_target + pnt, (HUGEDATA INT8U *)&ingred_no_tar[0], 2 * DIRECT_INGREDIENT_BLOCK_NUM);
	// check direct ingredient area
	total_block_src = 0;
	total_block_tar = 0;
	for (i = 0; i < DIRECT_INGREDIENT_BLOCK_NUM; i++)
	{
		if((ingred_no_src[i] > 0) && (ingred_no_src[i] <= MAX_INGREDIENT_TEXT_NUMBER)) total_block_src++;;	// direct ingredient no(source)
		if((ingred_no_tar[i] > 0) && (ingred_no_tar[i] <= MAX_INGREDIENT_TEXT_NUMBER)) total_block_tar++;;	// direct ingredient no(target)
	}
	// move시에서 source만큼의 공간 필요(향후 개선 필요)
	//if (ingredient_text_count_empty() + total_block_tar < total_block_src) return 9;	// memory full
	if (total_block_src > total_block_tar)
	{
		if (!ingredient_text_check_empty_count(total_block_src-total_block_tar)) return 0;	// lack of space
	}
#endif

	for (i=0; i<plu_record_size; i++) {
		ch=Flash_bread(addr_source + i );
		if (i==0) ch=0;
		Flash_bwrite(addr_target + i, ch);
	}

#ifdef USE_DIRECT_INGREDIENT
	// direct ingredient copy
	ingredient_text_read(&ingred_no_src[0], dmsg, 0, DEFAULT_INGREDIENT_TEXT_ALL_SIZE);
	ingredient_text_write(&ingred_no_tar[0], dmsg, 0, DEFAULT_INGREDIENT_TEXT_ALL_SIZE,1);
	pnt = plu_get_ptype_point(PTYPE_INGREDIENT_TEXT, &stype, &usize);	// direct ingredient point
	Flash_swrite(addr_target + pnt, (HUGEDATA INT8U *)&ingred_no_tar[0], 2 * DIRECT_INGREDIENT_BLOCK_NUM);
#endif

	pnt=plu_get_ptype_point(PTYPE_DEPARTMENT_NO,&stype,&usize);
	Flash_wwrite(addr_target+pnt,(INT16U)to_dept);
	pnt=plu_get_ptype_point(PTYPE_PLU_NO,&stype,&usize);

	Flash_lwrite(addr_target+pnt,to_plu );
	plu_table_save(to_dept, to_plu);

	if (network_status.service_flag&0x20)	// send plu
	{
		commun_net_mask(0, (INT16U *)&key, 0xff, 1);	// "key" :to_dept, to_plu의 key
	}

	// SG.060213.Copy 시 Discount 도 copy 되게 수정
//	if(flag==1) { // Move 
		dc_no    = _get_dc_number((INT16U)from_dept,(INT32U)from_plu);
		if(dc_no) {
			addr_target = get_addr_discount_area(dc_no);
			if (!addr_target) { /* 에러 처리 */
				return 0;
			}
			Flash_sread(addr_target, (HUGEDATA INT8U*)&disc_str, DISCOUNT_STRUCT_SIZE);
			//--------------------------- SG.060213			
			dc_no=_get_new_dc_number();
			addr_target = get_addr_discount_area(dc_no);
			if (!addr_target) { /* 에러 처리 */
				return 0;
			}
			//--------------------------- 			
			disc_str.dept_no = to_dept;
			disc_str.plu_no  = to_plu;
			Flash_swrite(addr_target, (HUGEDATA INT8U*)&disc_str, DISCOUNT_STRUCT_SIZE);

		}
	if(flag==1)  
	{
		//plu_delete((unsigned char)from_dept,(long)from_plu,0);
		plu_delete((unsigned char)from_dept,(long)from_plu,1);
#ifdef USE_DIRECT_INGREDIENT
		// delete source ingredient
		dmsg[0] = 0;
		ingredient_text_write(&ingred_no_src[0], dmsg, 0, DEFAULT_INGREDIENT_TEXT_ALL_SIZE, 1);
#endif
	}

//	}
	return 1;

}

INT16S plu_get_ptype_point2(INT16U cur_ptype,INT8U *stype,INT16U *xsize)
{
	INT16S    ret_size,p;
	INT16U rsize;

	if (stype) *stype=0;
	*xsize = 0;
	ret_size = -1;
	if (cur_ptype<128) {
		if (plu_global[cur_ptype].direct) {
			p = plu_global[cur_ptype].direct-1;
			if (stype) *stype = plu_global[p].stype;
			*xsize = plu_global[p].usize;
			rsize  = plu_global[p].ptype_point;
			if ((Startup.country==COUNTRY_KR) && (cur_ptype==3)) {
				*xsize=2;
				if (stype) *stype='W';
			}
			ret_size=rsize;
			if (plu_global[p].inhibit) ret_size = -1;
		}
	}
	return ret_size;
}

INT16S plu_get_ptype_point(INT16U cur_ptype,INT8U *stype,INT16U *xsize)
{
	INT16S p;
	INT16S rsize;

	if (stype) *stype=0;
	*xsize=0;
	rsize =0;
	if (cur_ptype<128) {
		if (plu_global[cur_ptype].direct) {
			p=(INT16S)plu_global[cur_ptype].direct-1;
			if (stype) *stype = plu_global[p].stype;
			*xsize = plu_global[p].usize;
			rsize  = plu_global[p].ptype_point;
			if ((Startup.country==COUNTRY_KR) && (cur_ptype==3)) {
				*xsize=2;
				if (stype) *stype='W';
			}
			if (plu_global[p].inhibit) *xsize=0;
		}
	}
	return rsize;
}

//SG071018
INT16S plu_get_ptype_point3(INT16U cur_ptype,INT8U *stype,INT16U *xsize)
{
	INT16S p;
	INT16S rsize;

	if (stype) *stype=0;
	*xsize=0;
	rsize =0;
	if (cur_ptype<128) {
		if (plu_global[cur_ptype].direct) {
			p=(INT16S)plu_global[cur_ptype].direct-1;
			if (stype) *stype = plu_global[p].stype;
			*xsize = plu_global[p].usize;
			rsize  = plu_global[p].ptype_point;
			if ((Startup.country==COUNTRY_KR) && (cur_ptype==3)) {
				*xsize=2;
				if (stype) *stype='W';
			}
			//if (plu_global[p].inhibit) *xsize=0;
		}
	}
#if defined(USE_PLU_WITH_TRACE_PROTOCOL) || defined(USE_CONTROL_CHANGING_PRICE)
	else
	{
		switch(cur_ptype)
		{
			case 128:	//is lot
			case 132:	//slaughter_place[0]
			case 133:	//slaughter_place[1]
			case 134:	//slaughter_place[2]
			case 135:	//meat_use
			case 136:	//meat_part
			case 137:	//meat_grade
			case 140:	//control changing price
			case 141:	//changed price upper
			case 142:	//changed price lower
				if (stype) *stype = 'B';
				*xsize = 1;
				break;
			case 129:	//indivNo[0]
			case 130:	//indivNo[1]
 			case 131:	//indivNo[2]
				if (stype) *stype = 'S';
				*xsize = INDIVIDUAL_NO_LEN;
				break;
		}
	}
#endif
	return rsize;
}

///////////////////////
// return : 0 = use
//          1 = inhibit
INT8U plu_check_inhibit_ptype(INT16U ptype)
{
	INT16S p;
	if (ptype<128) {
		if (plu_global[ptype].direct) {
			p=(INT16S)plu_global[ptype].direct-1;
			if (plu_global[p].inhibit) return 1;
		}
	}
	return 0;
}

INT8U plu_get_plu_type(INT16U dept_no, INT32U plu_no)
{
	INT16S sz;
	INT16U   key,ptype;
	plu_table_search((INT8U)dept_no, plu_no,&key,0);
	if (key==0) {	   // Error , Cannot found PLU
		return 0;
	}
	ptype=0;
	plu_get_field(key-1, PTYPE_PLU_TYPE,(INT8U *)&ptype,&sz,sizeof(ptype)); // Number
	if (ptype==0) ptype=1;
#ifdef USE_FIXED_PLUTYPE//USE_EMART_NETWORK
	if (ptype >= PLU_BY_WEIGHT_FIX && ptype <= PLU_BY_PCS_FIX)
	{
		ptype -= 3;
	}
#endif
	return ptype;
}
