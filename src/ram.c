/*****************************************************************************
|*			
|*  Copyright		:	(c) 2002 CAS
|*  Filename		:	ram.c
|*  Version		:	0.1
|*  Programmer(s)	:	Hong Yi-Phyo (HYP)
|*  Created		:	2002/12/10
|*  Description		:	CLP Printer RAM control routine    
|*				
****************************************************************************/

#include "globals.h"
#include "ram.h"
#include "flash_app.h"
#include "main.h"

extern void SFlash_program_data(INT8U cs, INT32U start_addr, HUGEDATA INT8U *data, INT32U byte_count);
extern void SFlash_program_data_c2(INT8U cs, INT32U start_addr, HUGEDATA INT8U *data, INT32U byte_count);
extern void SFlash_sread(INT8U cs, INT32U start_addr,HUGEDATA INT8U *str,INT16U leng);
extern INT8U SFlash_check_nvbuffer_4_rd(INT8U cs, INT32U start_addr, INT32U byte_count);
/* HYP 20060404 Erase (non-use function)
INT8U RamTest(HUGEDATA INT16U *address, INT32U vol)
{
	HUGEDATA INT16U *p;
	INT32U RamBaseAdd;
	INT8U err = 0;	

	RamBaseAdd = (INT32U)address;

	p = (HUGEDATA INT16U *)(RamBaseAdd + 0x010000);
	for (; p < (HUGEDATA INT16U *)(RamBaseAdd + vol); p++) {
		*p = 0xaa55;
	}

	p = (HUGEDATA INT16U *)(RamBaseAdd + 0x010000);
	for (; p < (HUGEDATA INT16U *)(RamBaseAdd + vol); p++) {
		if(*p != 0xaa55) err=-1;
	}

	p = (HUGEDATA INT16U *)(RamBaseAdd + 0x010000);
	for (; p < (HUGEDATA INT16U *)(RamBaseAdd + vol); p++) {
		*p = 0x55aa;
	}
	p = (HUGEDATA INT16U *)(RamBaseAdd + 0x010000);
	for (; p < (HUGEDATA INT16U *)(RamBaseAdd + vol); p++) {
		if (*p != 0x55aa) err=-1;
	}

	return err;
}

INT8U InitRamTest(void)              //yik0101
{
	
	HUGEDATA INT16U *p;
	INT16U i = 0;
	INT8U err = 0;


	p = (HUGEDATA INT16U *)(NVRAM_BASE + 0x010000);
  	for(; p < (HUGEDATA INT16U *)(NVRAM_BASE + 0x03FFFF);p++)
		if((*p == 0x0000) || (*p == 0xffff)) i++;

	p = (HUGEDATA INT16U *)((NVRAM_BASE + NVRAM_VOLUME) + 0x010000);
	for(; p < (HUGEDATA INT16U *)((NVRAM_BASE + NVRAM_VOLUME) + 0x03FFFF);p++)
		if((*p == 0x0000) || (*p == 0xffff)) i++;

	if(i > 32) err= 1;

	return err;
}
*/


INT8U get_nvram_bparam(INT32U addr)
{
	INT8U v;
	get_nvram_sparam(addr, (INT8U *)&v, 1);
	return v;
}
INT16U get_nvram_wparam(INT32U addr)
{
	INT16U v;
	get_nvram_sparam(addr, (INT8U *)&v, 2);
	return v;
}

INT32U get_nvram_lparam(INT32U addr)
{
	INT32U v;
	get_nvram_sparam(addr, (INT8U *)&v, 4);
	return v;
}

void get_nvram_sparam(INT32U addr,INT8U *s,INT16S leng)
{
	if (addr + leng > NVRAM_VOLUME) return;
	SFlash_sread(FLASH1_CS, NVRAM_BASE+addr, s, leng);
}

void set_nvram_bparam(INT32U addr,INT8U v)
{
	set_nvram_sparam(addr, (INT8U *)&v, 1);
}

void set_nvram_wparam(INT32U addr,INT16U v)
{
	set_nvram_sparam(addr, (INT8U *)&v, 2);
}

void set_nvram_lparam(INT32U addr,INT32U v)
{
	set_nvram_sparam(addr, (INT8U *)&v, 4);
}

extern INT16U AutoFlushNvramTime2;
void set_nvram_sparam(INT32U addr,INT8U *str,INT16S leng)
{
	if (addr + leng > NVRAM_VOLUME) return;
	SFlash_program_data(FLASH1_CS, NVRAM_BASE+addr, str, leng);
	AutoFlushNvramTime2 = SysTimer100ms;
}

void nvram_set(INT32U addr, INT8U ch, INT32U leng) // addr : logical address
{
	HUGEDATA INT8U *pstr;
	HUGEDATA INT8U *p;
	INT16U i;

	if(addr + leng > NVRAM_VOLUME) return;
	External_Interrupt_Disable(); // Power off Pending
	addr += NVRAM_BASE;
	if(SFlash_check_nvbuffer_4_rd(FLASH1_CS, addr, leng) == 1) { // IN_NVRAM
		p = FlashNVRAMStruct2.buffer + (addr&FLASH_BLOCK_LADDR_MASK);
		// ram(nv) -> ram
		for(i=0; i<leng; i++) {
			*p++ = ch;
		}
	} else {
		// flash -> ram
		pstr=(HUGEDATA INT8U *)addr;
		SFlashDataFill(FLASH1_CS, (INT8U *)pstr, ch, leng);
	}
	External_Interrupt_Enable();
}

#ifdef USE_MORNITORING_CNT
#define MAX_NVRAM_MASK_NUM 1
#define MORNITOR_INIT_COMPLETE		0x49		// Initial 완료

void nvram_clear(void)
{
	INT32U i, status;
//MsgOut("nvram_clear()\r\n");
	//need change. 1개의 Block만 지워지는 문제가 있어서 수정함.
	// nvram_set 함수안에 Sflash_Fill 함수 오류 문제 확인 해야함.
	
	status = get_nvram_lparam(NVRAM_MORNITORING_STATUS);
	
	if (status == MORNITOR_INIT_COMPLETE)
	{
		for (i = 0; i < 0x100; i++)
		{
			if (i == 0xff)
			{
				nvram_set((i*0x1000), 0, 3946);
				nvram_set((i*0x1000)+ 4078, 0, 18);
			}
			else {
				nvram_set((i*0x1000), 0, 0x1000);
			}
		}
	}
	else
	{
		for (i = 0; i < 0x100; i++)
		{
			nvram_set((i*0x1000), 0, 0x1000);
		}
		set_nvram_lparam(NVRAM_MORNITORING_STATUS, 0x49);
	}
}
#else
void nvram_clear(void)
{
	INT32U i;
//MsgOut("nvram_clear()\r\n");
	//need change. 1개의 Block만 지워지는 문제가 있어서 수정함.
	// nvram_set 함수안에 Sflash_Fill 함수 오류 문제 확인 해야함.
	for (i = 0; i < 0x100; i++)
	{
		nvram_set((i*0x1000), 0, 0x1000);
	}
}
#endif //USE_MORNITORING_CNT

void nvram_cpy(INT32U daddr, INT32U saddr, INT32U leng)
{
    INT32U i;
	INT32U cnt;
    INT8U buffer[1024];
    INT32U d_len;

	if (leng <= 0) return;

	cnt = leng / 1024 + 1; // 루프회수
    for (i = 0; i < cnt; i++)
    {
		d_len = 1024;
		if (i == cnt - 1) d_len = leng % 1024;

		get_nvram_sparam(saddr, buffer, d_len);

		set_nvram_sparam(daddr, buffer, d_len);

		saddr += d_len;
		daddr += d_len;
    }
}

#ifdef USE_DNS_RESOLVER
void set_dns_bparam(INT32U a,INT8U v)
{
	INT32U pnt;
	pnt =DFLASH_BASE;
	pnt+=FLASH_DNS_AREA;
	pnt+=a;
	Flash_bwrite(pnt,v);
}

void set_dns_sparam(INT32U a,INT8U *s,INT16S leng)
{
	INT32U pnt;
	pnt =DFLASH_BASE;
	pnt+=FLASH_DNS_AREA;
	pnt+=a;
	Flash_swrite(pnt,(INT8U*)s,leng);
}

INT8U get_dns_bparam(INT32U a)
{
	INT32U pnt;
	INT8U  value;
	pnt =DFLASH_BASE;
	pnt+=FLASH_DNS_AREA;
	pnt+=a;
	value=Flash_bread(pnt);
	return value;
}

void set_dns_wparam(INT32U a,INT16U v)
{
	INT32U pnt;
	pnt =DFLASH_BASE;
	pnt+=FLASH_DNS_AREA;
	pnt+=a;
	Flash_wwrite(pnt,v);
}

INT16U get_dns_wparam(INT32U a)
{
	INT32U pnt;
	INT16U  value;
	pnt =DFLASH_BASE;
	pnt+=FLASH_DNS_AREA;
	pnt+=a;
	value=Flash_wread(pnt);
	return value;
}

void set_dns_lparam(INT32U a,INT32U v)
{
	INT32U pnt;
	pnt =DFLASH_BASE;
	pnt+=FLASH_DNS_AREA;
	pnt+=a;
	Flash_lwrite(pnt,v);
}

INT32U get_dns_lparam(INT32U a)
{
	INT32U pnt;
	INT32U  value;
	pnt =DFLASH_BASE;
	pnt+=FLASH_DNS_AREA;
	pnt+=a;
	value=Flash_lread(pnt);
	return value;
}

void get_dns_sparam(INT32U a,INT8U *v,INT16S l)
{
	INT32U pnt;
	pnt =DFLASH_BASE;
	pnt+=FLASH_DNS_AREA;
	pnt+=a;
	Flash_sread(pnt,(INT8U *)v,l);
}
#endif // USE_DNS_RESOLVER
