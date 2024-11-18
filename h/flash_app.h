/*****************************************************************************
|*			
|*  Copyright		:	(c) 2002 CAS
|*  Filename		:	flash_app.h
|*  Version			:	0.1
|*  Programmer(s)	:	Lim Byoung Moo (LBM)
|*  Created			:	2002/11/11
|*  Description		:	CLP Flash Application header    
|*				
****************************************************************************/
#include "flash.h"
#include "ram.h"
#include "sale_report.h"
#include "commbuf.h"

#ifndef _FLASH_APP_H
#define _FLASH_APP_H

#define BASE_AD (0x8007B820)

#pragma pack(4)
typedef struct {
	INT8U	flag;
	INT8U	check;
    INT8U   reserved_1[2];
	INT32U	addr;
	INT8U	buffer[DFLASH_BLOCK_SIZE];
	INT16U	TableCrc;
    INT8U   reserved_2[2];
	INT8U   Dummy[4];  // Cache 충돌을 막기 위해 16byte align을 하기 위한 Dummy
} FLASH_NVRAM_STRUCT;
#pragma pack()

extern FLASH_NVRAM_STRUCT FlashNVRAMStruct;
extern FLASH_NVRAM_STRUCT FlashNVRAMStruct2;
extern FLASH_NVRAM_STRUCT FlashNVRAMCache2;
#ifdef USE_CPU_LOAD_TEST
extern INT8U FlagSFlashTest;
#endif
extern void Flash_Init_Cache(INT8U cache_no);
extern void Flash_init_nvbuffer(void);
extern void Nvram_init_nvbuffer(void);
extern void Flash_flush_nvbuffer(void);
extern void Nvram_flush_nvbuffer(void);
extern void Flash_flush_nvbuffer_c2(void) ;
extern void Flash_check_boot_changed(INT32U start_addr, INT32U byte_count);

extern void Flash_lwrite(INT32U start_addr,INT32U s);
extern void Flash_wwrite(INT32U start_addr,INT16U s);
extern void Flash_bwrite(INT32U start_addr,INT8U c);
extern void Flash_swrite(INT32U start_addr,HUGEDATA INT8U *str,INT16U leng);
extern void Flash_swrite_c2(INT32U start_addr,HUGEDATA INT8U *str,INT16U leng);
extern void Flash_dwrite(INT32U start_addr,HUGEDATA INT8U *date,INT16U leng);
extern INT32U Flash_lread(INT32U start_addr);
extern INT32U Flash_lread_c2(INT32U start_addr);
extern INT16U Flash_wread(INT32U start_addr);
extern INT16U Flash_wread_c2(INT32U start_addr);
extern INT8U Flash_bread(INT32U start_addr);
extern void Flash_sread(INT32U start_addr,HUGEDATA INT8U *str,INT16U leng);
extern void Flash_sread_c2(INT32U start_addr,HUGEDATA INT8U *str,INT16U leng);
extern void Flash_clear(INT32U start_addr, INT8U ch,INT32U leng);
extern void Flash_write_data(INT32U startAddr,INT16U memSize,HUGEDATA INT8U *value,INT8U v_leng,INT8U bitFlag,INT8U startBit,INT8U countBit,INT16U fcode);

extern INT8U SFlashDataFill(INT8U cs, HUGEDATA INT8U *address, INT8U data, INT32U byte_count);
#ifdef USE_CPU_LOAD_TEST
extern INT8U SFlashTestReadWrite (HUGEDATA COMM_BUF *CBuf);
#endif
#endif