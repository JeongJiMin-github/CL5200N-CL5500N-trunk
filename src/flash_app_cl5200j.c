/*****************************************************************************
|*			
|*  Copyright		:	(c) 2003 CAS
|*  Filename		:	flash_app.c
|*  Version			:	0.1
|*  Programmer(s)	:	Lim Byoung Moo (LBM)
|*  Created			:	2003/03/20
|*  Description		:	CLP Flash Application    
|*				
****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "globals.h"
#include "flash.h"
#include "flash_app.h"
#include "common.h"
#include "ram.h"
#include "main.h"
#include "Prtdrv.h"

#ifdef CPU_PIC32MZ
#include "sqi.h"


extern char MsgBuf[];
extern void MsgOut(HUGEDATA char *);
extern INT16U crc_16(INT8U *aData, INT32U aSize);

///////////////////////////////////////////////////////HYP 20040204 FLASH NVRAM
#define IN_NVRAM	1
#define IN_FLASH	0

//FLASH_NVRAM_STRUCT __attribute__((persistent,coherent,address(0x80074000))) FlashNVRAMStruct;
//FLASH_NVRAM_STRUCT __attribute__((persistent,coherent,address(0x8007500C))) FlashNVRAMStruct2;

FLASH_NVRAM_STRUCT __attribute__((persistent,coherent,address(BASE_AD))) FlashNVRAMStruct;
FLASH_NVRAM_STRUCT __attribute__((persistent,coherent,address(BASE_AD+0x1010))) FlashNVRAMStruct2;
FLASH_NVRAM_STRUCT __attribute__((persistent,coherent,address(BASE_AD+0x1010+0x1010))) FlashNVRAMCache2;
INT32U flash_start_haddr,flash_start_next,flash_start_addr;

void SFlash_init_nvbuffer(INT8U cs)
{
	FLASH_NVRAM_STRUCT *pFlashNVRAMStruct;
	INT32U start, end;

	External_Interrupt_Disable();
	if (cs == FLASH0_CS)
	{
		pFlashNVRAMStruct = &FlashNVRAMStruct;
		start = DFLASH_BASE;
		end = (DFLASH_BASE + DFLASH_CHIP_MAX_NUM * DFLASH_VOLUME);
	}
	else
	{
		pFlashNVRAMStruct = &FlashNVRAMStruct2;
		start = NVRAM_BASE;
		end = NVRAM_BASE + NVRAM_VOLUME;
	}

	if ((pFlashNVRAMStruct->flag == 1) && (pFlashNVRAMStruct->check == 0xa5)){
		if ((pFlashNVRAMStruct->addr >= start) && (pFlashNVRAMStruct->addr < end)) {
			SFlashDataBlockReprogram(cs, (HUGEDATA INT8U *)pFlashNVRAMStruct->addr,pFlashNVRAMStruct->buffer);
		}
	}
	pFlashNVRAMStruct->flag  = 0;
	pFlashNVRAMStruct->check = 0xa5;
	pFlashNVRAMStruct->addr  = 0xffffffff;
	pFlashNVRAMStruct->TableCrc = crc_16((INT8U*)pFlashNVRAMStruct,NVRAM_BACKUP_CACHE_CRC_SIZE);
	External_Interrupt_Enable();
}


void SFlash_flush_nvbuffer(INT8U cs)
{
	FLASH_NVRAM_STRUCT *pFlashNVRAMStruct;

	if (cs == FLASH0_CS)
		pFlashNVRAMStruct = &FlashNVRAMStruct;
	else
		pFlashNVRAMStruct = &FlashNVRAMStruct2;

	External_Interrupt_Disable(); // Power off Pending
	if(pFlashNVRAMStruct->flag == 1) {
		SFlashDataBlockReprogram(cs, (HUGEDATA INT8U *)pFlashNVRAMStruct->addr,pFlashNVRAMStruct->buffer);
//sprintf(MsgBuf, "[FLU]cs=%d,%d,%06X\r\n", cs, pFlashNVRAMStruct->flag, pFlashNVRAMStruct->addr);//CJK0416
//MsgOut(MsgBuf);
	}
	pFlashNVRAMStruct->flag = 0;
	pFlashNVRAMStruct->TableCrc = crc_16((INT8U*)pFlashNVRAMStruct,NVRAM_BACKUP_CACHE_CRC_SIZE);
	External_Interrupt_Enable();
}

INT8U SFlash_check_nvbuffer_4_rd(INT8U cs, INT32U start_addr, INT32U byte_count)
{
	INT8U ret;
	
	FLASH_NVRAM_STRUCT *pFlashNVRAMStruct;

	if (cs == FLASH0_CS)
		pFlashNVRAMStruct = &FlashNVRAMStruct;
	else
		pFlashNVRAMStruct = &FlashNVRAMStruct2;

	External_Interrupt_Disable();
#ifdef USE_DIRECT_INGREDIENT
	if (FlashNVRAMCache2.flag == 1 && cs == FLASH0_CS) {
	 	if (((start_addr&FLASH_BLOCK_HADDR_MASK) <= FlashNVRAMCache2.addr) && (((start_addr+byte_count)&FLASH_BLOCK_HADDR_MASK) >= FlashNVRAMCache2.addr))
		{
		  	Flash_flush_nvbuffer_c2();
		}
	}
#endif
	ret = 0;//IN_FLASH;
	if(pFlashNVRAMStruct->flag == 1){
		if((start_addr&FLASH_BLOCK_HADDR_MASK) == pFlashNVRAMStruct->addr) {
			if(((start_addr+byte_count-1)&FLASH_BLOCK_HADDR_MASK) == pFlashNVRAMStruct->addr) {//HYP 20090414
				ret = 1;//IN_NVRAM;
			} else {
				SFlash_flush_nvbuffer(cs);
			}
		} else {
			if(((start_addr&FLASH_BLOCK_HADDR_MASK) < pFlashNVRAMStruct->addr) && (((start_addr+byte_count-1)&FLASH_BLOCK_HADDR_MASK) >= pFlashNVRAMStruct->addr)) {//HYP 20090414
				SFlash_flush_nvbuffer(cs);
			}
		}
	}
	External_Interrupt_Enable();
	return ret;
}

void SFlash_program_data(INT8U cs, INT32U start_addr, HUGEDATA INT8U *data, INT32U byte_count)
{
	INT16U	i;
	HUGEDATA INT8U *pstr, *src_ptr, *dst_ptr;

	INT16U flash_rem_data;//HYP 20090414
	FLASH_NVRAM_STRUCT *pFlashNVRAMStruct;

	if (cs == FLASH0_CS)
		pFlashNVRAMStruct = &FlashNVRAMStruct;
	else
		pFlashNVRAMStruct = &FlashNVRAMStruct2;

	pstr=(HUGEDATA INT8U *)start_addr;
	flash_start_haddr=start_addr&FLASH_BLOCK_HADDR_MASK;
	flash_start_next=(start_addr+byte_count-1)&FLASH_BLOCK_HADDR_MASK; //flash_start_next->flash_end_haddr //HYP 20090414
	flash_start_addr=(start_addr&FLASH_BLOCK_LADDR_MASK);
#ifdef USE_DIRECT_INGREDIENT
	if (FlashNVRAMCache2.flag == 1 && cs == FLASH0_CS) {
	 	if (flash_start_haddr <= FlashNVRAMCache2.addr && flash_start_next >= FlashNVRAMCache2.addr)
		{
		  	Flash_flush_nvbuffer_c2();
		}
	}
#endif
	if (pFlashNVRAMStruct->flag == 1) {
		if (flash_start_haddr == pFlashNVRAMStruct->addr) {	 // 시작주소가 이전 사용범위와 동일할때
			if (flash_start_next == pFlashNVRAMStruct->addr) { // Data 가 범위내에 있는경우
				External_Interrupt_Disable();
				dst_ptr = pFlashNVRAMStruct->buffer+flash_start_addr;
				src_ptr = data;
				// ram -> ram(nvram)
				for(i=0; i<byte_count; i++){
					*dst_ptr = *src_ptr;
					src_ptr++;
					dst_ptr++;
				}
				//S0BUF = 'A';
				pFlashNVRAMStruct->TableCrc = crc_16((INT8U*)pFlashNVRAMStruct,NVRAM_BACKUP_CACHE_CRC_SIZE);
				External_Interrupt_Enable();
				return;
			} else if (flash_start_next == (pFlashNVRAMStruct->addr+DFLASH_BLOCK_SIZE)) {
				External_Interrupt_Disable();
				dst_ptr = pFlashNVRAMStruct->buffer+flash_start_addr;
				src_ptr = data;
				flash_rem_data   = DFLASH_BLOCK_SIZE-flash_start_addr;
				// ram -> ram(nvram)
				for (i=0; i<flash_rem_data; i++) {
					*dst_ptr = *src_ptr;
					src_ptr++;
					dst_ptr++;
				}
				pFlashNVRAMStruct->TableCrc = crc_16((INT8U*)pFlashNVRAMStruct,NVRAM_BACKUP_CACHE_CRC_SIZE);
				//Flash_flush_nvbuffer();
				SFlash_flush_nvbuffer(cs);

				pFlashNVRAMStruct->addr = flash_start_next; //start_addr&FLASH_BLOCK_HADDR_MASK;
				// flash -> ram(nvram)
				SFlashDWordDataRead(cs, pFlashNVRAMStruct->addr, (HUGEDATA INT32U *)pFlashNVRAMStruct->buffer, DFLASH_BLOCK_DSIZE);
				dst_ptr = pFlashNVRAMStruct->buffer;
				// ram -> ram(nvram)
				for(i=flash_rem_data; i<byte_count; i++){
					*dst_ptr = *src_ptr;
					src_ptr++;
					dst_ptr++;
				}
				pFlashNVRAMStruct->flag = 1;
				pFlashNVRAMStruct->TableCrc = crc_16((INT8U*)pFlashNVRAMStruct,NVRAM_BACKUP_CACHE_CRC_SIZE);
				External_Interrupt_Enable();
				//S0BUF = 'B';
				return;
			} else {
				External_Interrupt_Disable();
				SFlash_flush_nvbuffer(cs);
				External_Interrupt_Enable();
				//S0BUF = 'C';
			}
		} else {
			External_Interrupt_Disable();
			SFlash_flush_nvbuffer(cs);
			External_Interrupt_Enable();
			//S0BUF = 'D';
		}
	}
	if (flash_start_haddr == flash_start_next) { // 이전사용범위와 다르고 데이타 영역이 같을때.
		External_Interrupt_Disable();
		pFlashNVRAMStruct->addr = flash_start_haddr; //start_addr&FLASH_BLOCK_HADDR_MASK;
		// flash -> ram(nvram)
		SFlashDWordDataRead(cs, pFlashNVRAMStruct->addr, (HUGEDATA INT32U *)pFlashNVRAMStruct->buffer, DFLASH_BLOCK_DSIZE);
		dst_ptr = pFlashNVRAMStruct->buffer+flash_start_addr;
		src_ptr = data;
		// ram -> ram(nvram)
		for(i=0; i<byte_count; i++){
			*dst_ptr = *src_ptr;
			src_ptr++;
			dst_ptr++;
		}
		pFlashNVRAMStruct->flag = 1;
		pFlashNVRAMStruct->TableCrc = crc_16((INT8U*)pFlashNVRAMStruct,NVRAM_BACKUP_CACHE_CRC_SIZE);
		External_Interrupt_Enable();
		//S0BUF = 'E';
	} else {
		External_Interrupt_Disable();
		SFlash_program_Reprogram(cs, pstr,data,byte_count);
		External_Interrupt_Enable();
		//S0BUF = 'F';
//	[.]	FlashDataReprogram(pstr,data,byte_count);
	}

	return;
}

void SFlash_program_data_c2(INT8U cs, INT32U start_addr, HUGEDATA INT8U *data, INT32U byte_count)
{
	INT16U	i;
	HUGEDATA INT8U *pstr, *src_ptr, *dst_ptr;

	INT16U flash_rem_data;//HYP 20090414
	FLASH_NVRAM_STRUCT *pFlashNVRAMStruct;

	if (cs == FLASH0_CS)
		pFlashNVRAMStruct = &FlashNVRAMCache2;
	else
		return;

	pstr=(HUGEDATA INT8U *)start_addr;
	flash_start_haddr=start_addr&FLASH_BLOCK_HADDR_MASK;
	flash_start_next=(start_addr+byte_count-1)&FLASH_BLOCK_HADDR_MASK; //flash_start_next->flash_end_haddr //HYP 20090414
	flash_start_addr=(start_addr&FLASH_BLOCK_LADDR_MASK);

	if (FlashNVRAMStruct.flag == 1 && cs == FLASH0_CS) {
	 	if (flash_start_haddr <= FlashNVRAMStruct.addr && flash_start_next >= FlashNVRAMStruct.addr)
		{
		  	Flash_flush_nvbuffer();
		}
	}

	if (pFlashNVRAMStruct->flag == 1) {
		if (flash_start_haddr == pFlashNVRAMStruct->addr) {	 // 시작주소가 이전 사용범위와 동일할때
			if (flash_start_next == pFlashNVRAMStruct->addr) { // Data 가 범위내에 있는경우
				External_Interrupt_Disable();
				dst_ptr = pFlashNVRAMStruct->buffer+flash_start_addr;
				src_ptr = data;
				// ram -> ram(nvram)
				for(i=0; i<byte_count; i++){
					*dst_ptr = *src_ptr;
					src_ptr++;
					dst_ptr++;
				}
				//S0BUF = 'A';
				pFlashNVRAMStruct->TableCrc = crc_16((INT8U*)pFlashNVRAMStruct,NVRAM_BACKUP_CACHE_CRC_SIZE);
				External_Interrupt_Enable();
				return;
			} else if (flash_start_next == (pFlashNVRAMStruct->addr+DFLASH_BLOCK_SIZE)) {
				External_Interrupt_Disable();
				dst_ptr = pFlashNVRAMStruct->buffer+flash_start_addr;
				src_ptr = data;
				flash_rem_data   = DFLASH_BLOCK_SIZE-flash_start_addr;
				// ram -> ram(nvram)
				for (i=0; i<flash_rem_data; i++) {
					*dst_ptr = *src_ptr;
					src_ptr++;
					dst_ptr++;
				}
				pFlashNVRAMStruct->TableCrc = crc_16((INT8U*)pFlashNVRAMStruct,NVRAM_BACKUP_CACHE_CRC_SIZE);
				Flash_flush_nvbuffer_c2();

				pFlashNVRAMStruct->addr = flash_start_next; //start_addr&FLASH_BLOCK_HADDR_MASK;
				// flash -> ram(nvram)
				SFlashDWordDataRead(cs, pFlashNVRAMStruct->addr, (HUGEDATA INT32U *)pFlashNVRAMStruct->buffer, DFLASH_BLOCK_DSIZE);
				dst_ptr = pFlashNVRAMStruct->buffer;
				// ram -> ram(nvram)
				for(i=flash_rem_data; i<byte_count; i++){
					*dst_ptr = *src_ptr;
					src_ptr++;
					dst_ptr++;
				}
				pFlashNVRAMStruct->flag = 1;
				pFlashNVRAMStruct->TableCrc = crc_16((INT8U*)pFlashNVRAMStruct,NVRAM_BACKUP_CACHE_CRC_SIZE);
				External_Interrupt_Enable();
				//S0BUF = 'B';
				return;
			} else {
				Flash_flush_nvbuffer_c2();
				//S0BUF = 'C';
			}
		} else {
			Flash_flush_nvbuffer_c2();
			//S0BUF = 'D';
		}
	}
	if (flash_start_haddr == flash_start_next) { // 이전사용범위와 다르고 데이타 영역이 같을때.
		External_Interrupt_Disable();
		pFlashNVRAMStruct->addr = flash_start_haddr; //start_addr&FLASH_BLOCK_HADDR_MASK;
		// flash -> ram(nvram)
		SFlashDWordDataRead(cs, pFlashNVRAMStruct->addr, (HUGEDATA INT32U *)pFlashNVRAMStruct->buffer, DFLASH_BLOCK_DSIZE);
		dst_ptr = pFlashNVRAMStruct->buffer+flash_start_addr;
		src_ptr = data;
		// ram -> ram(nvram)
		for(i=0; i<byte_count; i++){
			*dst_ptr = *src_ptr;
			src_ptr++;
			dst_ptr++;
		}
		pFlashNVRAMStruct->flag = 1;
		pFlashNVRAMStruct->TableCrc = crc_16((INT8U*)pFlashNVRAMStruct,NVRAM_BACKUP_CACHE_CRC_SIZE);
		External_Interrupt_Enable();
		//S0BUF = 'E';
	} else {
		External_Interrupt_Disable();
		SFlash_program_Reprogram(cs, pstr,data,byte_count);
		External_Interrupt_Enable();
		//S0BUF = 'F';
//	[.]	FlashDataReprogram(pstr,data,byte_count);
	}

	return;
}

void Flash_check_boot_changed(INT32U start_addr, INT32U byte_count)
{
	INT8U *p;
	INT8U *pbuffer;
	INT32U idx;
	INT32U size;
	
	if (FlashNVRAMStruct.addr == (start_addr & FLASH_BLOCK_HADDR_MASK))
	{
		p = (INT8U *)start_addr;
		pbuffer = &(FlashNVRAMStruct.buffer[start_addr & FLASH_BLOCK_LADDR_MASK]);
		size = DFLASH_BLOCK_SIZE - (start_addr & FLASH_BLOCK_LADDR_MASK);
		if (size > byte_count) size = byte_count;
		for (idx = 0 ; idx < size ; idx++)
		{
			if(*p != *pbuffer) *pbuffer = *p;
			p++;
			pbuffer++;
		}
	}
	else if ((FlashNVRAMStruct.addr == (start_addr & FLASH_BLOCK_HADDR_MASK - DFLASH_BLOCK_SIZE))
			&& ((start_addr & FLASH_BLOCK_LADDR_MASK + byte_count) > DFLASH_BLOCK_SIZE))
	{
		p = (INT8U *)(start_addr & FLASH_BLOCK_HADDR_MASK + DFLASH_BLOCK_SIZE);
		pbuffer = FlashNVRAMStruct.buffer;
		size = (start_addr + byte_count) & FLASH_BLOCK_LADDR_MASK;
		if (size > DFLASH_BLOCK_SIZE) size = DFLASH_BLOCK_SIZE;
		for (idx = 0 ; idx < size ; idx++)
		{
			if(*p != *pbuffer) *pbuffer = *p;
			p++;
			pbuffer++;
		}
	}
}

void SFlash_sread(INT8U cs, INT32U start_addr,HUGEDATA INT8U *str,INT16U leng)
{
	INT16U i;
	HUGEDATA INT8U *p;
	//HYP 20090414 start
	INT16U remain;
    INT32U end_addr;
    INT16U start_offset;
    INT16U end_offset;
    INT16U startBlock,endBlock;
    INT16U blockNum;
    
	FLASH_NVRAM_STRUCT *pFlashNVRAMStruct;

	if (cs == FLASH0_CS)
		pFlashNVRAMStruct = &FlashNVRAMStruct;
	else
		pFlashNVRAMStruct = &FlashNVRAMStruct2;

	if(pFlashNVRAMStruct->flag == 1){
		if((start_addr&FLASH_BLOCK_HADDR_MASK) == pFlashNVRAMStruct->addr) {
			if(((start_addr+leng-1)&FLASH_BLOCK_HADDR_MASK) == (pFlashNVRAMStruct->addr + DFLASH_BLOCK_SIZE)) {
				//ram
				External_Interrupt_Disable();
				p = pFlashNVRAMStruct->buffer + (start_addr&FLASH_BLOCK_LADDR_MASK);
				remain = DFLASH_BLOCK_SIZE - (start_addr&FLASH_BLOCK_LADDR_MASK);
				// ram(nv) -> ram
				for(i=0; i<remain; i++) {
					*str++ = *p++;
				}
				// flash -> ram
				SFlashByteDataRead(cs, start_addr + remain, str, (leng - remain));
				str += (leng - remain);
				External_Interrupt_Enable();
				return;
			}
		}
	}
	//HYP 20090414 end
	External_Interrupt_Disable();
	if(SFlash_check_nvbuffer_4_rd(cs, start_addr, leng) == 1) {	//IN_NVRAM
		p = pFlashNVRAMStruct->buffer + (start_addr&FLASH_BLOCK_LADDR_MASK);
		// ram(nv) -> ram
		for(i=0; i<leng; i++) {
			*str++ = *p++;
		}
	} else {
		// flash -> ram
		SFlashByteDataRead(cs, start_addr, str, leng);
		str += leng;
	}
	External_Interrupt_Enable();
}

void Flash_Init_Cache(INT8U cache_no)
{
	External_Interrupt_Disable();
	FLASH_NVRAM_STRUCT *pFlashNVRAMStruct;
	
	switch(cache_no)
	{
		case FLASH_CACHE: // FLASH 1st cache
			pFlashNVRAMStruct = &FlashNVRAMStruct;
			break;
		case NVRAM_CACHE: // NVRAM 1st cache
			pFlashNVRAMStruct = &FlashNVRAMStruct2;
			break;
		case INGREDIENT_CACHE: // FLASH 2nd cache 	
			pFlashNVRAMStruct = &FlashNVRAMCache2;
			break;
		default:
			return;
			break;
	}
	pFlashNVRAMStruct->flag  = 0;
	pFlashNVRAMStruct->check = 0xa5;
	pFlashNVRAMStruct->addr  = 0xffffffff;
	pFlashNVRAMStruct->TableCrc = crc_16((INT8U*)pFlashNVRAMStruct,NVRAM_BACKUP_CACHE_CRC_SIZE);
	External_Interrupt_Enable();
}

// NVRAM 영역용 cache의 init
void Nvram_init_nvbuffer(void)
{
	SFlash_init_nvbuffer(FLASH1_CS);
}

// NVRAM 영역용 cache의 flush
void Nvram_flush_nvbuffer(void)
{
	SFlash_flush_nvbuffer(FLASH1_CS);
}

// FLASH 영역용 cache의 init
void Flash_init_nvbuffer(void)
{
	// 1st Flash cache
	SFlash_init_nvbuffer(FLASH0_CS);

	// 2nd Flash cache
	External_Interrupt_Disable();
	if ((FlashNVRAMCache2.flag == 1) && (FlashNVRAMCache2.check == 0xa5)){
		if ((FlashNVRAMCache2.addr >= DFLASH_BASE) &&
		    (FlashNVRAMCache2.addr < (DFLASH_BASE + DFLASH_CHIP_MAX_NUM * DFLASH_VOLUME))) {
			Flash_check_boot_changed(DFLASH_BASE + FLASH_FWMODE_SET, 1);
			SFlashDataBlockReprogram(FLASH0_CS, (HUGEDATA INT8U *)FlashNVRAMCache2.addr,FlashNVRAMCache2.buffer);
		}
	}
	FlashNVRAMCache2.flag  = 0;
	FlashNVRAMCache2.check = 0xa5;
	FlashNVRAMCache2.addr  = 0xffffffff;
	External_Interrupt_Enable();
}

// FLASH 영역용 1st cache의 flush
void Flash_flush_nvbuffer(void)
{
	SFlash_flush_nvbuffer(FLASH0_CS);
}

// FLASH 영역용 2nd cache의 flush
void Flash_flush_nvbuffer_c2(void) 
{
	External_Interrupt_Disable();
	if(FlashNVRAMCache2.flag == 1) {
		SFlashDataBlockReprogram(FLASH0_CS, (HUGEDATA INT8U *)FlashNVRAMCache2.addr,FlashNVRAMCache2.buffer);
	}
	FlashNVRAMCache2.flag = 0;
	FlashNVRAMCache2.TableCrc = crc_16((INT8U*)&FlashNVRAMCache2,NVRAM_BACKUP_CACHE_CRC_SIZE);
	External_Interrupt_Enable();
}

INT8U Flash_check_nvbuffer_4_rd(INT32U start_addr, INT32U byte_count) // FLASH 영역용 1st cache
{
	INT8U ret;
	
	ret = SFlash_check_nvbuffer_4_rd(FLASH0_CS, start_addr, byte_count);
	return ret;
}
  
#ifdef USE_DIRECT_INGREDIENT//DEL080826#if (ROM_COUNTRY == ROM_RUS)
INT8U Flash_check_nvbuffer_4_rd_c2(INT32U start_addr, INT32U byte_count) // FLASH 영역용 2nd cache
{
	INT8U ret;
	
	External_Interrupt_Disable();
	if (FlashNVRAMStruct.flag == 1) {
	 	if (((start_addr&FLASH_BLOCK_HADDR_MASK) <= FlashNVRAMStruct.addr) && (((start_addr+byte_count)&FLASH_BLOCK_HADDR_MASK) >= FlashNVRAMStruct.addr))
		{
		  	Flash_flush_nvbuffer();
		}
	}
	ret = IN_FLASH;
	if(FlashNVRAMCache2.flag == 1){
		if((start_addr&FLASH_BLOCK_HADDR_MASK) == FlashNVRAMCache2.addr) {
			if(((start_addr+byte_count-1)&FLASH_BLOCK_HADDR_MASK) == FlashNVRAMCache2.addr) {
				ret = IN_NVRAM;
			} else {
				Flash_flush_nvbuffer_c2();
			}
		} else {
			if(((start_addr&FLASH_BLOCK_HADDR_MASK) < FlashNVRAMCache2.addr) && (((start_addr+byte_count-1)&FLASH_BLOCK_HADDR_MASK) >= FlashNVRAMCache2.addr)) {
				Flash_flush_nvbuffer_c2();
			}
		}      
	}
	External_Interrupt_Enable();
	return ret;
}
#endif

void Flash_program_data(INT32U start_addr, HUGEDATA INT8U *data, INT32U byte_count)
{
	SFlash_program_data(FLASH0_CS, start_addr, data, byte_count);
}

#ifdef USE_DIRECT_INGREDIENT//DEL080826#if (ROM_COUNTRY == ROM_RUS)
//SG070824.cache2
void Flash_program_data_c2(INT32U start_addr, HUGEDATA INT8U *data, INT32U byte_count)
{
	SFlash_program_data_c2(FLASH0_CS, start_addr, data, byte_count);
}
#endif

///////////////////////////////////////////////////////HYP 20040204 FLASH NVRAM

extern INT16U AutoFlushNvramTime;
// ***********************************************
// long(DWord) Write
// ***********************************************
void Flash_lwrite(INT32U start_addr,INT32U s)
{
	Flash_program_data(start_addr,(HUGEDATA INT8U *)&s,4);
	AutoFlushNvramTime = SysTimer100ms;
}

// ***********************************************
// Word Write
// ***********************************************
void Flash_wwrite(INT32U start_addr,INT16U s)
{
	Flash_program_data(start_addr,(HUGEDATA INT8U *)&s,2);
	AutoFlushNvramTime = SysTimer100ms;
}

// ***********************************************
// Byte Write
// ***********************************************
void Flash_bwrite(INT32U start_addr,INT8U c)
{
	Flash_program_data(start_addr,(HUGEDATA INT8U *)&c,1);
	AutoFlushNvramTime = SysTimer100ms;
}

// ***********************************************
//  String Write
// ***********************************************
void Flash_swrite(INT32U start_addr,HUGEDATA INT8U *str,INT16U leng)
{
	Flash_program_data(start_addr,str,leng);
	AutoFlushNvramTime = SysTimer100ms;
}

#ifdef USE_DIRECT_INGREDIENT//DEL080826#if (ROM_COUNTRY == ROM_RUS)
//SG070824
// ***********************************************
//  String Write using cache2.
// ***********************************************
void Flash_swrite_c2(INT32U start_addr,HUGEDATA INT8U *str,INT16U leng)
{
	Flash_program_data_c2(start_addr,str,leng);
}
#endif


// ***********************************************
// Date Write if Leng=6 then yy,mm,dd,hour,min,sec
//		     =3 then yy,mm,dd
// ***********************************************
void Flash_dwrite(INT32U start_addr,HUGEDATA INT8U *date,INT16U leng)
{
	Flash_program_data(start_addr,date,leng);
	AutoFlushNvramTime = SysTimer100ms;
}

// ***********************************************
// long(DWord) Read
// ***********************************************
INT32U Flash_lread(INT32U start_addr)
{
	INT32U s;

	Flash_sread(start_addr,(HUGEDATA INT8U*)&s,4);
	return s;
}

#ifdef USE_DIRECT_INGREDIENT//DEL080826#if (ROM_COUNTRY == ROM_RUS)
INT32U Flash_lread_c2(INT32U start_addr)
{
	INT32U s;

	Flash_sread_c2(start_addr,(HUGEDATA INT8U*)&s,4);
	return s;
}

INT16U Flash_wread_c2(INT32U start_addr)
{
	INT16U s;

	Flash_sread_c2(start_addr,(HUGEDATA INT8U*)&s,2);
	return s;
}

#endif

// ***********************************************
// Word Read
// ***********************************************
INT16U Flash_wread(INT32U start_addr)
{
	INT16U s;

	Flash_sread(start_addr,(HUGEDATA INT8U*)&s,2);
	return s;
}

// ***********************************************
// Byte Read
// ***********************************************
INT8U Flash_bread(INT32U start_addr)
{
	INT8U s;

	Flash_sread(start_addr,(HUGEDATA INT8U*)&s,1);
	return s;
}


void Flash_sread(INT32U start_addr,HUGEDATA INT8U *str,INT16U leng)
{
	SFlash_sread(FLASH0_CS, start_addr, str, leng);
}

#ifdef USE_DIRECT_INGREDIENT//DEL080826#if (ROM_COUNTRY == ROM_RUS)
void Flash_sread_c2(INT32U start_addr,HUGEDATA INT8U *str,INT16U leng)
{
	INT16U i;
	HUGEDATA INT8U *p;
	INT16U remain;
	if(FlashNVRAMCache2.flag == 1){
		if((start_addr&FLASH_BLOCK_HADDR_MASK) == FlashNVRAMCache2.addr) {
			if(((start_addr+leng-1)&FLASH_BLOCK_HADDR_MASK) == (FlashNVRAMCache2.addr + DFLASH_BLOCK_SIZE)) {
				//ram
				p = FlashNVRAMCache2.buffer + (start_addr&FLASH_BLOCK_LADDR_MASK);
				remain = DFLASH_BLOCK_SIZE - (start_addr&FLASH_BLOCK_LADDR_MASK);
				// ram(nv) -> ram
				for(i=0; i<remain; i++) {
					*str++ = *p++;
				}
				// flash -> ram
				SQI_Flash_Read(FLASH0_CS, start_addr + remain, str, (leng - remain));
				str += (leng - remain);
				return;
			}
		}
	}

	if(Flash_check_nvbuffer_4_rd_c2(start_addr, leng) == IN_NVRAM) {
		p = FlashNVRAMCache2.buffer + (start_addr&FLASH_BLOCK_LADDR_MASK);
		// ram(nv) -> ram
		for(i=0; i<leng; i++) {
			*str++ = *p++;
		}
	} else {
		// flash -> ram
		SQI_Flash_Read(FLASH0_CS, start_addr, str, leng);
		str += leng;
	}
}
#endif

// ***********************************************
//  Flash Clear
// ***********************************************
void Flash_clear(INT32U start_addr, INT8U ch,INT32U leng)
{
	HUGEDATA INT8U *pstr;
	HUGEDATA INT8U *p;
	INT16U i;
	External_Interrupt_Disable();
	if(Flash_check_nvbuffer_4_rd(start_addr, leng) == IN_NVRAM) {
		p = FlashNVRAMStruct.buffer + (start_addr&FLASH_BLOCK_LADDR_MASK);
		// ram(nv) -> ram
		for(i=0; i<leng; i++) {
			*p++ = ch;
		}
	} else {
		// flash -> ram
		pstr=(HUGEDATA INT8U *)start_addr;
		FlashDataFill(pstr,ch,leng);
	}
	External_Interrupt_Enable();
}

// ***********************************************
//  Flash Write (bit, byte, string) 
// ***********************************************
void Flash_write_data(INT32U startAddr,INT16U memSize,HUGEDATA INT8U *value,INT8U v_leng,INT8U bitFlag,INT8U startBit,INT8U countBit,INT16U fcode)
{
	INT32U v,mask,emask,old_v;
	INT8U  v8;
	//INT8U  sl;
	INT16U v16;
	INT32U v32;
	//INT16S    addr;
	INT32S    lvalue;
	char   temp[32];
	//char   gtemp[64];
	INT8U  orgData[4];
	v8=0;
	v16=0;
	v32=0;

	if(bitFlag!=3) lvalue = atol((char *)value);

	switch (bitFlag) 
	{
		case 1: //Byte
			v8 = (INT8U)lvalue;
			if(fcode == 1777)
			{
				 v8 *= 8;
			}
			Flash_bwrite(startAddr,v8);
			break;
		case 2: //Word
			v16 = (INT16U)lvalue;
			if(fcode == 1431)
			{
				v16 *= 10;
			}
			if((fcode >= 1753) && (fcode <=1761))
			{
				if ((fcode == 1753) || (fcode == 1756) || (fcode == 1759))
				{
					if(v16 == 60) 
					{
						v16 = 56;
					} 
					else 
					{
						v16 = v16 - 2;
					}
				}
				v16 *= 8;
			}
			if(fcode == 1774)
			{
				v16 *= STB_TIME_DIV;
			}
			if(fcode == 1775) 
			{
				v16 = -v16;
			}
			Flash_wwrite(startAddr,v16);
			break;
		case 3: //String
			memset(temp,0,sizeof(temp));
			memcpy(temp,value,v_leng);
			Flash_swrite(startAddr,(HUGEDATA INT8U *)temp,memSize);
			break;
		case 8:
		case 4: //Bit
			v = (INT32U)lvalue;
			Flash_sread(startAddr, orgData, memSize);

			if (memSize==1) {
				memcpy(&v8,orgData,1);
				old_v=v8;
			} else if (memSize==2) {
				memcpy(&v16,orgData,2);
				old_v=v16;
			} else if (memSize==4) {
				memcpy(&v32,orgData,4);
				old_v=v32;
			}
			mask=0xffffffff;
			v   <<= (startBit+1-countBit);
			mask<<= (startBit+1-countBit);
			emask = mask<< countBit;	// 110000
			emask =~emask;		  	// 001111
			mask &= emask;		  	// 001100
			v&=mask;
			old_v &= ~mask;
			v     |= old_v;

			if (memSize==1) {
				v8=v;
				Flash_bwrite(startAddr,v8);
			} else if (memSize==2) {
				v16=v;
				Flash_wwrite(startAddr,v16);
			} else if (memSize==4) {
				v32=v;
				Flash_lwrite(startAddr,v32);
			}
			break;
		case 5: //Long
			v32 = (INT32U)lvalue;
			Flash_lwrite(startAddr,v32);
			break;
		default:
			break;
	}
}

void Flash_read_data(INT32U startAddr,INT16U memSize,HUGEDATA INT8U *value,INT8U bitFlag,INT8U startBit,INT8U countBit,INT16U fcode)
{
	INT32U mask,emask,old_v;
	INT8U  v8;
	INT16U v16;
	INT32U v32;
	INT8U   temp[50];
	INT8U  orgData[4];
	v8=0;
	v16=0;
	v32=0;

	switch (bitFlag) 
	{
		case 1: //Byte
			v8 = Flash_bread(startAddr);
			if(fcode == 1777) v8 /= 8;
			sprintf((char*)temp,"%d",v8);
			break;
		case 2: //Word
			v16 = Flash_wread(startAddr);
			if(fcode == 1431)
			{
				v16 /= 10;
			}
			if((fcode >= 1753) && (fcode <=1761))
			{
				v16 /= 8;
				if ((fcode == 1753) || (fcode == 1756) || (fcode == 1759))
				{
					if(v16 == 56) 
					{
						v16 = 60;
					} 
					else 
					{
						v16 = v16 + 2;
					}
				}
			}
			if(fcode == 1774) v16 /= STB_TIME_DIV;
			if(fcode == 1775) 
			{
				v16 = -v16;
				sprintf((char*)temp,"%d",(INT16S)v16);
			}
			else
			{
				sprintf((char*)temp,"%d",v16);
			}
			break;
		case 3: //String
			memset(temp,0,sizeof(temp));
			Flash_sread(startAddr,(HUGEDATA INT8U *)temp,memSize);
			break;
		case 8:
		case 4: //Bit
			Flash_sread(startAddr, orgData, memSize);

			if (memSize==1) {
				memcpy(&v8,orgData,1);
				old_v=v8;
			} else if (memSize==2) {
				memcpy(&v16,orgData,2);
				old_v=v16;
			} else if (memSize==4) {
				memcpy(&v32,orgData,4);
				old_v=v32;
			}
			mask=0xffffffff;
			mask<<= (startBit+1-countBit);
			emask = mask<< countBit;	// 110000
			emask =~emask;		  	// 001111
			mask &= emask;		  	// 001100
			old_v&=mask;

			old_v>>=(startBit+1-countBit);
			sprintf((char*)temp,"%ld",old_v);
			break;
		case 5: //Long
			v32 = Flash_lread(startAddr);
			sprintf((char*)temp,"%lu",v32);
			break;
		default:
			break;
	}

	sprintf((char*)value, "D=%02X.%s", strlen(temp), temp);
}


extern void FtlDataBackupFlagRead(INT32U *flag);
void upload_ftl_table(HUGEDATA COMM_BUF *CBuf, INT32U mode)	// mode = 0:summary, 1:detail, 99:set default
{
	INT32U test_i;
    INT32U ftlBackupFlag;
	INT16U count_free=0;
	INT16U count_using=0;
	INT16U count_used=0;
	INT16U count_invalid=0;
	
	if (mode == 0x99)
	{
		FtlDataBackupSetDefault();
		return;
	}
    
    FtlDataBackupFlagRead(&ftlBackupFlag);
    
	if (mode == 1)
	{
		commun_out(CBuf,"[Assign Address List]\r\n");
		for (test_i = 0; test_i < 256; test_i++)
		{
			sprintf(MsgBuf, "LSA:0x%06X  Assign:0x%06X\r\n", (test_i * 0x1000), FtlManagement.AssignAddr[test_i]);
			commun_out_waiting(CBuf,MsgBuf);
		}
		commun_out(CBuf, "-------------------------------------\r\n");
	}
	
	if (mode == 1)
	{
		commun_out(CBuf,"[PSA LIST]\r\n");
	}
	for (test_i = 0; test_i < 1000; test_i++)
	{	
		if (mode == 1)
		{
			sprintf(MsgBuf, "PSA:0x%06X  STATUS:%d  ERASE Cnt:%2ld\r\n", NVRAM_BASE + (test_i * 0x1000), FtlManagement.PsaStatus[test_i], FtlManagement.PsaEraseCnt[test_i]);
			commun_out_waiting(CBuf,MsgBuf);
		}
		switch (FtlManagement.PsaStatus[test_i])
		{
			case PSA_STATUS_FREE:	count_free++;	break;
			case PSA_STATUS_USING:	count_using++;	break;
			case PSA_STATUS_USED:	count_used++;	break;
			case PSA_STATUS_INVALID:	count_invalid++;	break;
		}
	}
	FTL_Check_Lifetime();	
	if (mode == 1)
	{
		commun_out(CBuf,"-------------------------------------\r\n");
	}
	commun_out(CBuf,"[ETC LIST]\r\n");
	sprintf(MsgBuf, "FTL Flag      : 0x%08X\r\n", ftlBackupFlag);
	commun_out(CBuf,MsgBuf);
	sprintf(MsgBuf, "PSA Write Ptr : 0x%08X\r\n", FtlManagement.PsaWritePtr);
	commun_out(CBuf,MsgBuf);
	sprintf(MsgBuf, "PSA Erase Ptr : 0x%08X\r\n", FtlManagement.PsaErasePtr);
	commun_out(CBuf,MsgBuf);
	sprintf(MsgBuf, "FTL Avg LifeTime : %ld\r\n", FtlErrManagement.AvgLifeTime);
	commun_out(CBuf,MsgBuf);
	sprintf(MsgBuf, "FTL Max LifeTime : %ld\r\n", FtlMaxLifeTime);
	commun_out(CBuf,MsgBuf);
	sprintf(MsgBuf, "FTL Min LifeTime : %ld\r\n", FtlMinLifeTime);
	commun_out(CBuf,MsgBuf);
	sprintf(MsgBuf, "sum of COUNT     : %3d\r\n", count_free+count_using+count_used+count_invalid);
	commun_out(CBuf,MsgBuf);
	sprintf(MsgBuf, " count of FREE    : %3d\r\n", count_free);
	commun_out(CBuf,MsgBuf);
	sprintf(MsgBuf, " count of USING   : %3d\r\n", count_using);
	commun_out(CBuf,MsgBuf);
	sprintf(MsgBuf, " count of USED    : %3d\r\n", count_used);
	commun_out(CBuf,MsgBuf);
	sprintf(MsgBuf, " count of INVALID : %3d\r\n", count_invalid);
	commun_out(CBuf,MsgBuf);
	return;
}

#ifdef USE_CPU_LOAD_TEST
/**
********************************************************************************
* @brief       Serial Flash Memory Read/Write Test를 위한 함수
* @param     .
* @return     none
* @remark    .
********************************************************************************
*/
INT8U FlagSFlashTest;
int mem_cmp (const void* dst, const void* src, INT32U cnt) {
	const INT8U *d = (const INT8U *)dst, *s = (const INT8U *)src;
	int r = 0;

	while (cnt-- && (r = *d++ - *s++) == 0) ;
	return r;
}
INT8U SFlashTestReadWrite (HUGEDATA COMM_BUF *CBuf)
{
    static INT16U TestTimer100ms=0;
    static INT32U TestFailCount = 0;
    static INT32U TestCount = 0;
    static INT8U flagPrevTest = 0;
    RTC_TIME tmpTime1, tmpTime2;
    INT8U testResult = 0;
        
    if( flagPrevTest != FlagSFlashTest )
    {
        TestCount = 0;
        TestFailCount = 0;
        flagPrevTest = FlagSFlashTest;
    }
    
    if( FlagSFlashTest == OFF )
    {
        
        return 0;
    }
    else
    {
        if ( SysTimer100ms < TestTimer100ms )
        {
            TestTimer100ms = 0;
        }
        else if ( SysTimer100ms - TestTimer100ms < 100 )
        {
            return 0;
        }
        TestTimer100ms = SysTimer100ms;
    }

    switch( FlagSFlashTest )
    {
        case 1:     // NVRAM 영역 TEST
            RTC_GetTime(BCD,&tmpTime1);
            set_nvram_sparam( NVRAM_RESERVE2,(INT8U *)&tmpTime1,sizeof(RTC_TIME) );
            get_nvram_sparam( NVRAM_RESERVE2,(INT8U *)&tmpTime2,sizeof(RTC_TIME) );
            send_crlf(CBuf);
            sprintf( MsgBuf, "** NVRAM TEST **" );
            commun_out(CBuf,MsgBuf);
            send_crlf(CBuf);
            sprintf( MsgBuf,"[NR_TESTING]" );
            commun_out(CBuf,MsgBuf);
            send_crlf(CBuf);
            sprintf( MsgBuf,"%02x:%02x:%02x", tmpTime2.hours,tmpTime2.minutes,tmpTime2.seconds );
            commun_out(CBuf,MsgBuf);
            send_crlf(CBuf);
            
            testResult = mem_cmp( &tmpTime1, &tmpTime2, sizeof(RTC_TIME) );
            if( testResult )    TestFailCount++;
            TestCount++;
            sprintf( MsgBuf, "- Result:" );
            commun_out(CBuf,MsgBuf);
            send_crlf(CBuf);
            sprintf( MsgBuf, "Fail - %d", TestFailCount );
            commun_out(CBuf,MsgBuf);
            send_crlf(CBuf);
            sprintf( MsgBuf, "%d tests", TestCount );
            commun_out(CBuf,MsgBuf);
            send_crlf(CBuf);

            if( TestCount % 3 == 0 )
            {
                keyapp_print(1);
                
            }
            else if( TestCount % 3 == 2 )
            {
                keyapp_pluno(0,1,ON);
                status_scale.print_multiple = 3;
                Operation.multiply = ON;
        		Operation.multiplyEdit = ON;
        		status_scale.cur_keyvalue=status_scale.print_multiple;
                status_scale.departmentid = 1;
            }
            break;

        default:
            FlagSFlashTest = 0;
            break;
    }
    return 1;
}
#endif

#endif //#ifdef CPU_PIC32MZ
