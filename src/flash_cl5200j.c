/*****************************************************************************
|*		       
|*  Copyright		:	(c) 2002 CAS
|*  Filename		:	flash.c
|*  Version		:	0.1
|*  Programmer(s)	:	Hong Yi-Phyo (HYP)
|*  Created		:	2002/11/11
|*  Description		:	CLP Printer Flash memory routine    
|*				
****************************************************************************/

#include <stdio.h>
#include <string.h>
#include "globals.h"
#include "commbuf.h"
#include "flash.h"
#include "flash_app.h"
#include "initial.h"
#include "wdt.h"
#include "main.h"
#include "adm.h"
#include "prtdrv.h"
#include "mode_calibration.h" // for NON_REMOVABLE_AREA_BACKUP
#include "sqi.h"

extern INT16U crc_16(INT8U *aData, INT32U aSize);

#ifdef CPU_PIC32MZ

//HUGEDATA FTL_MANAGEMENT_STRUCT __attribute__((persistent,coherent,address(0x80076018))) FtlManagement;
HUGEDATA FTL_MANAGEMENT_STRUCT __attribute__((persistent,coherent,address(BASE_AD+0x1010+0x1010+0x1010))) FtlManagement;
FTL_ERROR_MANAGEMENT_STRUCT FtlErrManagement;
CACHE_ERROR_COUNT_STRUCT cacheCrcErrCount;
INT8U FtlBackupEnableFlag = OFF;
INT8U MemLifetimeState = 0;
INT32U FtlMaxLifeTime = 0;
INT32U FtlMinLifeTime = 0;

INT16S InterruptDisableCount = 0;

void External_Interrupt_Disable(void)
{
	InterruptDisableCount++;
	SYS_INT_SourceDisable(INT_SOURCE_EXTERNAL_0);
}

void External_Interrupt_Enable(void)
{
	InterruptDisableCount--;
	if (InterruptDisableCount == 0)
	{
		SYS_INT_SourceEnable(INT_SOURCE_EXTERNAL_0);
	}

}

void FtlCheckBackupTime(void)
{
	// FTL의 안전한 BACKUP을 위한 최소 충전시간 체크 (4초)
	if (FtlBackupEnableFlag == OFF)
	{
		if ((INT16U)(SysTimer100ms - SysBootTime) > 40)	// 4초
		{
			FtlBackupEnableFlag = ON;
			//MsgOut("FTL ENABLE\r\n");
		}
	}
}

void set_Used_Sector_Erase(INT32U psaErasePtr)
{
	INT32U physicalAddr;
	INT32U i;
	INT32U idx;
	
	physicalAddr = 0xFFFFFFFF;
	if (psaErasePtr >= (MAX_PSA_QTY - 1)) psaErasePtr = 0;
	for (i = 0; i < MAX_PSA_QTY; i++)
	{
		idx = (i + psaErasePtr) % MAX_PSA_QTY;
		if (FtlManagement.PsaStatus[idx] == PSA_STATUS_USED)
		{
			if (FtlManagement.PsaEraseCnt[idx] >= PSA_LIFE_TIME_INVALID)
			{
				External_Interrupt_Disable();
				FtlManagement.PsaStatus[idx] = PSA_STATUS_INVALID;
				FtlManagement.TableCrc = crc_16((INT8U*)&FtlManagement,NVRAM_BACKUP_FTL_TABLE_CRC_SIZE);
				External_Interrupt_Enable();
				continue;
			}
			physicalAddr = (idx * DFLASH_BLOCK_SIZE) + NVRAM_BASE;
			break;
		}
	}
	if(physicalAddr == 0xFFFFFFFF)
	{
		return;
	}

	External_Interrupt_Disable();
	SQI_Flash_SectorErase(FLASH1_CS, physicalAddr);
//sprintf(MsgBuf, "[ERA]PsaId=%d, cnt=%d\r\n", idx, FtlManagement.PsaEraseCnt[idx]);
//MsgOut(MsgBuf);
	FtlManagement.PsaErasePtr = idx;
	FtlManagement.PsaStatus[idx] = PSA_STATUS_FREE;
	FtlManagement.PsaEraseCnt[idx]++;
	FtlManagement.TableCrc = crc_16((INT8U*)&FtlManagement,NVRAM_BACKUP_FTL_TABLE_CRC_SIZE);
	External_Interrupt_Enable();

}

void FtlHist_AddHistory(INT16U laddr, INT16U paddr);
INT8U get_Free_Sector_addr(INT32U assignedIndex, INT32U *physicalAddr, INT32U psaWritePtr)
{
	INT32U usedPsaIndex;
	INT32U i;
	INT32U idx;
	INT16U count = 0;

	if (psaWritePtr >= (MAX_PSA_QTY - 1)) psaWritePtr = 0;
//	for (i = 0; i < MAX_PSA_QTY; i++)
//	{
//		idx = (i + psaWritePtr) % MAX_PSA_QTY;
//		if (FtlManagement.PsaStatus[idx] == PSA_STATUS_USING) count++;
//	}
//	if (count != MAX_LSA_QTY)
//	{
//		sprintf(MsgBuf, "<<<<< USING COUNT = %d >>>>>\r\n", count);
//		MsgOut(MsgBuf);
//	}
	
	for (i = 0; i < MAX_PSA_QTY; i++)
	{
		idx = (i + psaWritePtr) % MAX_PSA_QTY;
		if (FtlManagement.PsaStatus[idx] == PSA_STATUS_FREE)
		{
			*physicalAddr = (idx * DFLASH_BLOCK_SIZE) + NVRAM_BASE;
			FtlManagement.PsaWritePtr = idx;
			usedPsaIndex = (FtlManagement.AssignAddr[assignedIndex] - NVRAM_BASE) / DFLASH_BLOCK_SIZE; //PSA Index
			FtlManagement.PsaStatus[usedPsaIndex] = PSA_STATUS_USED;
			FtlManagement.PsaStatus[idx] = PSA_STATUS_USING;
			FtlManagement.AssignAddr[assignedIndex] = *physicalAddr;
			FtlManagement.TableCrc = crc_16((INT8U*)&FtlManagement,NVRAM_BACKUP_FTL_TABLE_CRC_SIZE);
			FtlHist_AddHistory(assignedIndex, idx);
			return TRUE;
		}
	}

	*physicalAddr = 0;
	return FALSE;
}

INT32U assign_Free_Sector(INT32U logicalAddr) //LSA에 PSA를 할당하는 함수.
{
	INT32U physicalAddr;
	INT8U assign_check;
	INT32U lsaIndex;

	lsaIndex = (logicalAddr >> 12)-LSA_INDEX_OFFSET;
	assign_check = get_Free_Sector_addr(lsaIndex, &physicalAddr,FtlManagement.PsaWritePtr);
	while(!assign_check)
	{
		set_Used_Sector_Erase(FtlManagement.PsaErasePtr);
		assign_check = get_Free_Sector_addr(lsaIndex, &physicalAddr,FtlManagement.PsaWritePtr);
	}
	return physicalAddr;
}


INT32U get_PSA_Sector_addr(INT32U address)
{
	INT32U hiLsa;
	INT32U lowLsa;
	INT32U physicalAddress;
	INT32U lsaIndex;
	
	hiLsa = address & FLASH_BLOCK_HADDR_MASK;
	lowLsa = address & FLASH_BLOCK_LADDR_MASK;

	lsaIndex = (hiLsa >> 12) - LSA_INDEX_OFFSET;
	physicalAddress = FtlManagement.AssignAddr[lsaIndex] | lowLsa;
	return physicalAddress;
}

void NvramAutoEraseSector(void)
{
	static INT16U autoEraseNvSectorStartTime = 0;
	static INT16U autoEraseNvSectorPeriodTime = 0;
	static INT8U flagStartAutoEraseNvSector = ON;
	
	if (!ADM_GetData.Stable || Operation.pressKeyForNvErase || Operation.receiving)
	{
		autoEraseNvSectorStartTime = PrtDrvStruct.Timer10ms;
		if (autoEraseNvSectorStartTime == 0) autoEraseNvSectorStartTime++;
		Operation.pressKeyForNvErase = OFF;
		Operation.receiving = OFF;
	}
	
	if (autoEraseNvSectorStartTime)
	{
		flagStartAutoEraseNvSector = OFF;
		if (CheckTimeOver10ms(100, autoEraseNvSectorStartTime)) // 1s
		{
			autoEraseNvSectorStartTime = 0;
			autoEraseNvSectorPeriodTime = PrtDrvStruct.Timer10ms - 1;
			flagStartAutoEraseNvSector = ON;
		}
	}
	else
	{
		if (flagStartAutoEraseNvSector)
		{
			if (CheckTimeOver10ms(3, autoEraseNvSectorPeriodTime)) // 100ms : sectore erase time이 약 50~60ms
			{
				autoEraseNvSectorPeriodTime = PrtDrvStruct.Timer10ms;
				set_Used_Sector_Erase(FtlManagement.PsaErasePtr);
			}
		}
	}
}

//void SFlashBlockRead(INT8U cs, INT32U address, INT8U *data)
//{
//	INT32U logicalAddr;
//	INT32U physicalAddr;
//	logicalAddr = address;
//	physicalAddr = logicalAddr;
//	if (cs == FLASH1_CS)
//	physicalAddr = get_PSA_Sector_addr(logicalAddr);
//    SQI_Flash_ReadSector(cs, physicalAddr, (INT8U*)&data);
//}

void SFlashByteDataRead(INT8U cs, INT32U address, INT8U *data, INT32U size)
{
	INT32U logicalAddr;
	INT32U physicalAddr;
    
    INT32U start_addr;
    INT32U end_addr;
    INT16U start_offset;
    INT16U end_offset;
    INT16U startBlock,endBlock;
    INT16U blockNum;
    INT16U i;
    
	logicalAddr = address;
	physicalAddr = logicalAddr;
	if (cs == FLASH1_CS)
    {
        end_addr      = logicalAddr + size - 1;
        start_offset  = (INT16U)(logicalAddr & FLASH_BLOCK_LADDR_MASK);
        end_offset    = (INT16U)(end_addr & FLASH_BLOCK_LADDR_MASK);
        startBlock = (logicalAddr & FLASH_BLOCK_HADDR_MASK) >> 12;
        endBlock = (end_addr & FLASH_BLOCK_HADDR_MASK) >> 12;
        blockNum = (endBlock - startBlock) + 1;
         
        if(blockNum > 1)
        {
            for (i = 0; i < blockNum; i++)
            {
                if (i == 0)
                {
                    physicalAddr = get_PSA_Sector_addr(logicalAddr);
                    SQI_Flash_Read(cs, physicalAddr, data, (DFLASH_BLOCK_SIZE - start_offset));
                    data+=(DFLASH_BLOCK_SIZE - start_offset);
                    logicalAddr += (DFLASH_BLOCK_SIZE - start_offset);
                    //start_addr++;
                }
                else if(i == blockNum-1)
                {
                    physicalAddr = get_PSA_Sector_addr(logicalAddr);
                    SQI_Flash_Read(cs, physicalAddr, data, end_offset+1);
                }
                else
                {
                    physicalAddr = get_PSA_Sector_addr(logicalAddr);
                    SQI_Flash_Read(cs, physicalAddr, data, DFLASH_BLOCK_SIZE);
                    data += DFLASH_BLOCK_SIZE;
                    logicalAddr += DFLASH_BLOCK_SIZE;
                }
            }
        }
        else
        {
            physicalAddr = get_PSA_Sector_addr(logicalAddr);
            SQI_Flash_Read(cs, physicalAddr, data, size);
        }
    }
    else
    {
#ifdef USE_EXTEND_FLASH
        if(cs == FLASH0_CS && physicalAddr >= DFLASH_LOGICAL_FLASH2_BASE) 
        {
              cs = FLASH1_CS;
              physicalAddr = physicalAddr - DFLASH_LOGICAL_FLASH2_BASE + FLASH_NVRAM_SIZE;
//sprintf(MsgBuf, "[RBYTE]cs=%d, before=%d, after=%d\r\n", cs, logicalAddr,  physicalAddr);
//MsgOut(MsgBuf);  
        }
#endif        
        SQI_Flash_Read(cs, physicalAddr, data, size);
    }

}

extern char MsgBuf[100];
/*SFlashDWordDataRead 이 함수는 블럭내에서만 사용이 가능함.*/
void SFlashDWordDataRead(INT8U cs, INT32U address, INT32U *data, INT32U dsize)
{
	INT32U logicalAddr;
	INT32U physicalAddr;
	logicalAddr = address;
	physicalAddr = logicalAddr;
	if (cs == FLASH1_CS) {
        if (logicalAddr > NVRAM_BASE + NVRAM_VOLUME && logicalAddr < FLASH_NVRAM_SIZE) return;//nvram 영역을 벗어난 경우 return
        
		if ((logicalAddr & FLASH_BLOCK_HADDR_MASK) >= NVRAM_BASE && (logicalAddr & FLASH_BLOCK_HADDR_MASK) < (NVRAM_BASE + NVRAM_VOLUME))
        {        
            physicalAddr = get_PSA_Sector_addr(logicalAddr);
        }
    }

#ifdef USE_EXTEND_FLASH
    if(cs == FLASH0_CS && physicalAddr > DFLASH_LOGICAL_FLASH2_BASE) 
    {
          cs = FLASH1_CS;
          physicalAddr = physicalAddr - DFLASH_LOGICAL_FLASH2_BASE + FLASH_NVRAM_SIZE;
//sprintf(MsgBuf, "[RWord]cs=%d, before=%d, after=%d\r\n", cs, logicalAddr,  physicalAddr);
//MsgOut(MsgBuf);    
    }
#endif

   
	SQI_Flash_Read4byte(cs, physicalAddr, data, dsize);
}

//void SFlashPageRead(INT8U cs, INT32U address, INT8U *data)
//{
//	INT32U logicalAddr;
//	INT32U physicalAddr;
//	logicalAddr = address;
//	physicalAddr = logicalAddr;
//	if (cs == FLASH1_CS)
//	physicalAddr = get_PSA_Sector_addr(logicalAddr);
//    SQI_Flash_ReadPage(cs, physicalAddr, (INT8U *)&data);
//}





void SFlashBlockErase(INT8U cs, HUGEDATA INT16U *address) // flash & nvram(s.flash) 공용
{
	INT32U physicalAddr;
	INT32U logicalAddr;
	logicalAddr = (INT32U) address;
	physicalAddr = logicalAddr;

	if (cs != FLASH1_CS)
	{
#ifdef USE_EXTEND_FLASH
	    if(cs == FLASH0_CS && physicalAddr >= DFLASH_LOGICAL_FLASH2_BASE) 
	    {
	          cs = FLASH1_CS;
	          physicalAddr = physicalAddr - DFLASH_LOGICAL_FLASH2_BASE + FLASH_NVRAM_SIZE;
//sprintf(MsgBuf, "[Erase]cs=%d, before=%d, after=%d\r\n", cs, logicalAddr,  physicalAddr);
//MsgOut(MsgBuf); 
	    }
#endif         
	    SQI_Flash_SectorErase(cs, physicalAddr); // EN25QH32에서는 4kbyte SECTOR
	}
	else
	{
        if (logicalAddr > NVRAM_BASE + NVRAM_VOLUME && logicalAddr < FLASH_NVRAM_SIZE) return;//nvram 영역을 벗어난 경우 return
        
		if ((logicalAddr & FLASH_BLOCK_HADDR_MASK) >= NVRAM_BASE && (logicalAddr & FLASH_BLOCK_HADDR_MASK) < (NVRAM_BASE + NVRAM_VOLUME))
		{
			physicalAddr = assign_Free_Sector(logicalAddr);
		}
		else
		{
			SQI_Flash_SectorErase(cs, physicalAddr);
		}

	}
}

void SFlashDWordProgram(INT8U cs, HUGEDATA INT32U *address, INT32U data)
{
	SQI_Flash_Write4byte(cs, (INT32U)address, (INT32U*)&data, 1);
}

void SFlashSectorProgram(INT8U cs, HUGEDATA INT32U *address, INT8U *data)
{
	SQI_Flash_WriteSector(cs, (INT32U)address, data);
}

void SFlashDWordDataProgram(INT8U cs, HUGEDATA INT32U *address, HUGEDATA INT32U *data, INT32U dword_count)
{
	INT32U physicalAddr;
	INT32U logicalAddr;
	logicalAddr = (INT32U)address;
	physicalAddr = logicalAddr;

	if (cs == FLASH1_CS)
	{
        if (logicalAddr > NVRAM_BASE + NVRAM_VOLUME && logicalAddr < FLASH_NVRAM_SIZE) return;//nvram 영역을 벗어난 경우 return
        
		if ((logicalAddr & FLASH_BLOCK_HADDR_MASK) >= NVRAM_BASE && (logicalAddr & FLASH_BLOCK_HADDR_MASK) < (NVRAM_BASE + NVRAM_VOLUME))
		{
			physicalAddr = get_PSA_Sector_addr(logicalAddr);
		}
	}
#ifdef USE_EXTEND_FLASH
    if(cs == FLASH0_CS && physicalAddr >= DFLASH_LOGICAL_FLASH2_BASE) 
    {
          cs = FLASH1_CS;
          physicalAddr = physicalAddr - DFLASH_LOGICAL_FLASH2_BASE + FLASH_NVRAM_SIZE;
//sprintf(MsgBuf, "[PWord]cs=%d, before=%d, after=%d\r\n", cs, logicalAddr,  physicalAddr);
//MsgOut(MsgBuf);
    }
#endif     
	while(dword_count > 0) {
		if(*data != 0xffffffff) {
			SFlashDWordProgram(cs, (INT32U *)physicalAddr, *data);
		}
		dword_count--;
		data++;
		physicalAddr+=4;
	}

}

void SFlashBlockDataProgram(INT8U cs, HUGEDATA INT32U *address, HUGEDATA INT8U *data)
{
	INT32U physicalAddr;
	INT32U logicalAddr;
	logicalAddr = (INT32U)address;
	physicalAddr = logicalAddr;

	if (cs == FLASH1_CS)
	{
        if (logicalAddr > NVRAM_BASE + NVRAM_VOLUME && logicalAddr < FLASH_NVRAM_SIZE) return;//nvram 영역을 벗어난 경우 return
        
		if ((logicalAddr & FLASH_BLOCK_HADDR_MASK) >= NVRAM_BASE && (logicalAddr & FLASH_BLOCK_HADDR_MASK) < (NVRAM_BASE + NVRAM_VOLUME))
		{
			physicalAddr = get_PSA_Sector_addr(logicalAddr);
		}
	}
#ifdef USE_EXTEND_FLASH
    if(cs == FLASH0_CS && physicalAddr >= DFLASH_LOGICAL_FLASH2_BASE) 
    {
          cs = FLASH1_CS;
          physicalAddr = physicalAddr - DFLASH_LOGICAL_FLASH2_BASE + FLASH_NVRAM_SIZE;
//sprintf(MsgBuf, "[PBlock]cs=%d, before=%d, after=%d\r\n", cs, logicalAddr,  physicalAddr);
//MsgOut(MsgBuf);          
    }
#endif    
	SFlashSectorProgram(cs, (INT32U *)physicalAddr, (INT8U *)data);
}

INT8U SFlashDataBlockReprogram(INT8U cs, HUGEDATA INT8U *address, HUGEDATA INT8U *data)
{
	HUGEDATA INT32U *p;
	INT32U start_addr;

	start_addr = (INT32U)address;
	p  = (HUGEDATA INT32U *)(start_addr & FLASH_BLOCK_HADDR_MASK);

	SFlashBlockErase(cs, (HUGEDATA INT16U *)p);
	SFlashBlockDataProgram(cs, (HUGEDATA INT32U *)p, data);

	return FLASH_SUCCESS;
}

INT8U SFlash_program_Reprogram(INT8U cs, HUGEDATA INT8U *address, HUGEDATA INT8U *data, INT32U byte_count)
{
	HUGEDATA INT8U *p, *buffer;
	INT32U start_addr, end_addr;
	INT16U end_offset, start_offset,data_blk_num, flash_blk_num, data_r, start_r;
	INT8U  state;
	INT16U i,j;
	INT8U flashBuffer[DFLASH_BLOCK_SIZE];

	start_addr    = (INT32U)address;
	end_addr      = start_addr + byte_count;
	p             = (HUGEDATA INT8U *)(start_addr & FLASH_BLOCK_HADDR_MASK);
	start_offset  = (INT16U)(start_addr & FLASH_BLOCK_LADDR_MASK);
	end_offset    = (INT16U)(end_addr & FLASH_BLOCK_LADDR_MASK);
	data_r        = (INT16U)(byte_count % DFLASH_BLOCK_SIZE);
	data_blk_num  = (INT16U)(byte_count / DFLASH_BLOCK_SIZE);
	if (data_r) data_blk_num++;
	start_r       = DFLASH_BLOCK_SIZE - start_offset;
	flash_blk_num = data_blk_num;
	if(start_r < data_r) {
		flash_blk_num++;
	} else if(data_r == 0 && start_r != DFLASH_BLOCK_SIZE) {
		flash_blk_num++;
	}
	if (end_offset == 0) end_offset = DFLASH_BLOCK_SIZE;

	for(state = 0; state < flash_blk_num; state++) {
		if (state == 0) {
			// flash -> ram(nvram)
			SFlashDWordDataRead(cs, (INT32U)p, (HUGEDATA INT32U *)flashBuffer, DFLASH_BLOCK_DSIZE);
			buffer = (HUGEDATA INT8U *)flashBuffer + start_offset;
			if (flash_blk_num == 1) {
				// ram -> ram(nv)
				for (j = 0; j < byte_count; j++){
					*buffer++=*data++;
				}
			} else {
				// ram -> ram(nv)
				for (j = 0; j < start_r; j++){
					*buffer++=*data++;
				}
			}
			SFlashBlockErase(cs, (HUGEDATA INT16U *)p);
			SFlashBlockDataProgram(cs, (HUGEDATA INT32U *)p,(HUGEDATA INT8U *)flashBuffer);
			p = p + DFLASH_BLOCK_SIZE;
		} else if (state == flash_blk_num - 1){
			// flash -> ram(nvram)
			SFlashDWordDataRead(cs, (INT32U)p, (HUGEDATA INT32U *)flashBuffer, DFLASH_BLOCK_DSIZE);
			// ram -> ram(nvram)
			buffer = (HUGEDATA INT8U *)flashBuffer;
			for (j = 0; j < end_offset; j++){
				*buffer++=*data++;
			}
			SFlashBlockErase(cs, (HUGEDATA INT16U *)p);
			SFlashBlockDataProgram(cs, (HUGEDATA INT32U *)p,(HUGEDATA INT8U *)flashBuffer);
		} else {
			SFlashBlockErase(cs, (HUGEDATA INT16U *)p);
			SFlashBlockDataProgram(cs, (HUGEDATA INT32U *)p, data);
			data = data + DFLASH_BLOCK_SIZE;
			p = p + DFLASH_BLOCK_SIZE;
		}
	}

	return FLASH_SUCCESS;
}

INT8U SFlashDataFill(INT8U cs, HUGEDATA INT8U *address, INT8U data, INT32U byte_count)
{
	HUGEDATA INT8U *p,*buffer;
	INT32U start_addr, end_addr;
	INT16U start_offset, end_offset, data_r, start_r;
	INT32U data_blk_num, flash_blk_num;
	INT32U state;
	INT16U j;
	INT8U flashBuffer[DFLASH_BLOCK_SIZE];

	start_addr = (INT32U)address;
	end_addr = start_addr + byte_count;

	p = (HUGEDATA INT8U *)(start_addr & FLASH_BLOCK_HADDR_MASK);

	start_offset = (INT16U)(start_addr & FLASH_BLOCK_LADDR_MASK);
	end_offset = (INT16U)(end_addr & FLASH_BLOCK_LADDR_MASK);

	data_r = (INT16U)(byte_count % DFLASH_BLOCK_SIZE);

	data_blk_num = (byte_count / DFLASH_BLOCK_SIZE);
	if(data_r) data_blk_num++;

	start_r = DFLASH_BLOCK_SIZE - start_offset;
	flash_blk_num = data_blk_num;

	if(start_r < data_r) {
		flash_blk_num++;
	} else if(data_r == 0 && start_r != DFLASH_BLOCK_SIZE) {
		flash_blk_num++;
	}
	if (end_offset == 0) end_offset = DFLASH_BLOCK_SIZE;
	
	for(state = 0; state < flash_blk_num; state++) {
		if(state == 0){
			// flash -> ram
			SFlashDWordDataRead(cs, (INT32U)p, (HUGEDATA INT32U *)flashBuffer, DFLASH_BLOCK_DSIZE);
			buffer = (HUGEDATA INT8U *)flashBuffer + start_offset;
			if(flash_blk_num == 1){
				//byte_count 만큼 buffer에 write
				//data 증가
				// ram -> ram
				for (j = 0; j < (INT16U)byte_count; j++){
					*buffer++=data;
				}
			}
			else {
				//start_r 만큼 buffer에 write
				//data 증가
				// ram -> ram
				for (j = 0; j < start_r; j++){
					*buffer++=data;
				}
			}
			// sector erase
			SFlashBlockErase(cs, (HUGEDATA INT16U *)p);
			// word write
			// sector 크기만큼 p 증가
			SFlashBlockDataProgram(cs, (HUGEDATA INT32U *)p,(HUGEDATA INT8U *)flashBuffer);
			p = p + DFLASH_BLOCK_SIZE;

		} else if (state == flash_blk_num - 1){
			// flash -> ram
			SFlashDWordDataRead(cs, (INT32U)p, (HUGEDATA INT32U *)flashBuffer, DFLASH_BLOCK_DSIZE);
			// end_offset 만큼 buffer에 write
			buffer = (HUGEDATA INT8U *)flashBuffer;
			// ram -> ram
			for (j = 0; j < end_offset; j++){
				*buffer++=data;
			}
			// sector erase
			SFlashBlockErase(cs, (HUGEDATA INT16U *)p);
			// word write
			SFlashBlockDataProgram(cs, (HUGEDATA INT32U *)p,(HUGEDATA INT8U *)flashBuffer);
		} else {	// state < flash_sec_num - 1
			// sector erase
			SFlashBlockErase(cs, (HUGEDATA INT16U *)p);
			// sector 크기만큼 쓰고 data, p 증가
			if (data != 0xff) {
				for (j = 0; j < (DFLASH_BLOCK_SIZE); j++) {
					flashBuffer[j] = data;
				}
				SFlashBlockDataProgram(cs, (HUGEDATA INT32U *)p, flashBuffer);
				p = p + DFLASH_BLOCK_SIZE;
			} else {
				p = p + DFLASH_BLOCK_SIZE;
			}
		}
	}
	return FLASH_SUCCESS;
}

///// application

//FlashBlockErase는 꼭 단독으로 사용할 것 Write하기전 Erase용도로 사용하면 안됨.
//전원이 꺼질 때 발생하는 Interrupt가 동작할 수 있기 때문에 문제 소지가 있음.
void FlashBlockErase(HUGEDATA INT16U *address) // flash 전용
{
	External_Interrupt_Disable();
	SFlashBlockErase(FLASH0_CS, address);
	External_Interrupt_Enable();
}


// only Flash_program_data() is included this program Flash_program_Reprogram

INT8U FlashDataFill(HUGEDATA INT8U *address, INT8U data, INT32U byte_count)
{
	return SFlashDataFill(FLASH0_CS, address, data, byte_count);
}

#ifdef USE_EXTEND_FLASH
extern INT8U RemoteFwUpdateFlag;
#define REMOTE_UPDATE_NONE  0
#define REMOTE_UPDATE_FW    1
#define REMOTE_UPDATE_ROM   2
INT8U FlashCS1Erase(void)
{
    INT8U i;
    INT32U start_addr;

    if (Flash1ExtFlag != 1) return; 

//// erase 4 MByte     
    start_addr = 0x400000;
#ifdef USE_REMOTE_ROM_UPDATE    
    if (RemoteFwUpdateFlag == REMOTE_UPDATE_NONE) 
#endif        
    {
        for(i = 0; i < 63; i++) {
            SQI_Flash_BlockErase(FLASH1_CS, start_addr + (0x10000 * i));
        } 

        //7F0000H - 7F7FFFH(32 KByte)
        start_addr = 0x7F0000;
        SQI_Flash_BlockErase(FLASH1_CS, start_addr);   
        //7F8000H - 7F9FFFH(8 KByte)
        start_addr = 0x7F8000;
        SQI_Flash_BlockErase(FLASH1_CS, start_addr);   
        //7FA000H -	7FBFFFH(8 KByte)
        start_addr = 0x7FA000;
        SQI_Flash_BlockErase(FLASH1_CS, start_addr);   
        //7FC000H -	7FDFFFH(8 KByte)
        start_addr = 0x7FC000;
        SQI_Flash_BlockErase(FLASH1_CS, start_addr);       
        //7FE000H -	7FFFFFH(8 KByte))
        start_addr = 0x7FE000;
        SQI_Flash_BlockErase(FLASH1_CS, start_addr); 
    }
#ifdef USE_REMOTE_ROM_UPDATE
    else
    {
//// 2 MByte Mask Remote Firmware/Setting ROM Data
        //400000H ~ 5FFFFFH 64 KByte x 32
        start_addr = 0x400000;
        for(i = 0; i < 32; i++) {
            SQI_Flash_BlockErase(FLASH1_CS, start_addr + (0x10000 * i));
        }
//// 2 MByte        
    }
#endif
}
#endif

INT8U FlashDChipErase(void)
{
    INT8U i;
    INT32U start_addr;  
    
#ifdef USE_EXTEND_FLASH
    if (Flash1ExtFlag == 1) {
        FlashCS1Erase();
    }
#endif
	SQI_Flash_ChipErase(FLASH0_CS);
	return FLASH_SUCCESS;
}

ROMDATA INT16U BasicMemTable[7] = {0x0000, 0x0010, 0x0030};
ROMDATA INT16U ExtCardMemTable[7] = {0x0000, 0x0040, 0x00c0, 0x01c0, 0x03c0, 0x07c0, 0x0fc0};

INT16U FlashCheckMem(void) 
{
	return BasicMemTable[2] | ExtCardMemTable[2]; // 4MB
}

void NvramBackupDataRead(INT32U address, HUGEDATA INT32U *data, INT32U dsize)
{
	External_Interrupt_Disable();
	SQI_Flash_Read4byte(FLASH1_CS, address,(HUGEDATA INT32U *)data, dsize);
	External_Interrupt_Enable();
}

void NvramBackupDataProgram(HUGEDATA INT32U *address, HUGEDATA INT32U *data, INT32U dword_count)
{
	while(dword_count > 0) {
		if(*data != 0xffffffff)
		{
			SFlashDWordProgram(FLASH1_CS, address, *data);
		}
		address++;
		dword_count--;
		data++;
	}
}
void FtlDataBackupTableRead(INT8U no)
{
	INT32U addr;
	
	if (no == 0)
	{
		addr = NVRAM_BACKUP_FTL_TABLE_ADDR;
	}
	else
	{
		addr = NVRAM_BACKUP_FTL_TABLE_BACKUP_ADDR;
	}
	NvramBackupDataRead(addr, (HUGEDATA INT32U *)&FtlManagement, NVRAM_BACKUP_FTL_TABLE_DSIZE);
}

void FtlDataBackupFlagRead(INT32U *flag)
{
	External_Interrupt_Disable();
    NvramBackupDataRead(NVRAM_BACKUP_FLAG_ADDR, flag, 1);
	External_Interrupt_Enable();
}

void FtlDataBackupFlashCacheRead(void)
{
	External_Interrupt_Disable();
	NvramBackupDataRead(NVRAM_BACKUP_FLASH_CACHE_ADDR, (HUGEDATA INT32U *)&FlashNVRAMStruct, NVRAM_BACKUP_FLASH_CACHE_DSIZE);
	External_Interrupt_Enable();
}

void FtlDataBackupNvramCacheRead(void)
{
	External_Interrupt_Disable();
	NvramBackupDataRead(NVRAM_BACKUP_NVRAM_CACHE_ADDR, (HUGEDATA INT32U *)&FlashNVRAMStruct2, NVRAM_BACKUP_NVRAM_CACHE_DSIZE);
	External_Interrupt_Enable();
}

void FtlDataBackupIngredientCacheRead(void)
{
#ifdef USE_DIRECT_INGREDIENT
	External_Interrupt_Disable();
	NvramBackupDataRead(NVRAM_BACKUP_INGREDIENT_CACHE_ADDR, (HUGEDATA INT32U *) &FlashNVRAMCache2, NVRAM_BACKUP_INGREDIENT_CACHE_DSIZE);
	External_Interrupt_Enable();
#endif
}

void FtlDataBackupFlagProgram(INT32U flag)
{
	External_Interrupt_Disable();
	NvramBackupDataProgram((HUGEDATA INT32U *)NVRAM_BACKUP_FLAG_ADDR, (HUGEDATA INT32U *)&flag, 1);
	External_Interrupt_Enable();
}

extern void FtlHist_Init(INT16U crc);
void FtlDataBackupProgram(void)
{
	INT8U i;

	WDT_Timer_Clear();
	External_Interrupt_Disable();
	FtlDataBackupFlagProgram(FTL_BACKUP_FLAG_ERASE_TABLE);
	// table1
	for(i = NVRAM_BU_FTL_TABLE_BL_ADDR; i < NVRAM_BU_FTL_TABLE_BL_ADDR+NVRAM_BU_FTL_TABLE_BL_SIZE; i++)
	{
		SQI_Flash_SectorErase(FLASH1_CS, (i * 0x1000));
	}
	//FtlManagement.TableCrc = crc_16(&FtlManagement,NVRAM_BACKUP_FTL_TABLE_CRC_SIZE);
	NvramBackupDataProgram((HUGEDATA INT32U *)NVRAM_BACKUP_FTL_TABLE_ADDR, (HUGEDATA INT32U *) &FtlManagement, NVRAM_BACKUP_FTL_TABLE_DSIZE);
	
	// backup table2
	for(i = NVRAM_BU_FTL_TABLE_BACKUP_BL_ADDR; i < NVRAM_BU_FTL_TABLE_BACKUP_BL_ADDR+NVRAM_BU_FTL_TABLE_BACKUP_BL_SIZE; i++)
	{
		SQI_Flash_SectorErase(FLASH1_CS, (i * 0x1000));
	}
	NvramBackupDataProgram((HUGEDATA INT32U *)NVRAM_BACKUP_FTL_TABLE_BACKUP_ADDR, (HUGEDATA INT32U *) &FtlManagement, NVRAM_BACKUP_FTL_TABLE_DSIZE);

	FtlHist_Init(FtlManagement.TableCrc);
	External_Interrupt_Enable();
}

//void FtlDataBackupProgram_No(INT8U no)
//{
//	INT8U i;
//
//	WDT_Timer_Clear();
////	SYS_INT_SourceDisable(INT_SOURCE_EXTERNAL_0);
//	if(InterruptDisableCount > 0)
//	{
//		FtlManagement.TableCrc = 0x0419;
//		FtlDataBackupFlagProgram(FTL_BACKUP_FLAG_INTERRUPT_ERROR);
//	
//	}
//	else
//	{
//		FtlDataBackupFlagProgram(FTL_BACKUP_FLAG_INTERRUPT_ERROR);
//	}
//	
//	// table1
//	if(no == 0)
//	{
//		for(i = NVRAM_BU_FTL_TABLE_BL_ADDR; i < NVRAM_BU_FTL_TABLE_BL_ADDR+NVRAM_BU_FTL_TABLE_BL_SIZE; i++)
//		{
//			SFlashBlockErase(FLASH1_CS,(INT16U *)(i * 0x1000));
//		}
//		//FtlManagement.TableCrc = crc_16(&FtlManagement,NVRAM_BACKUP_FTL_TABLE_CRC_SIZE);
//		NvramBackupDataProgram((HUGEDATA INT32U *)NVRAM_BACKUP_FTL_TABLE_ADDR, (HUGEDATA INT32U *) &FtlManagement, NVRAM_BACKUP_FTL_TABLE_DSIZE);
//	}
//	else if (no == 1)
//	{
//		// backup table2
//		for(i = NVRAM_BU_FTL_TABLE_BACKUP_BL_ADDR; i < NVRAM_BU_FTL_TABLE_BACKUP_BL_ADDR+NVRAM_BU_FTL_TABLE_BACKUP_BL_SIZE; i++)
//		{
//			SFlashBlockErase(FLASH1_CS,(INT16U *)(i * 0x1000));
//		}
//		NvramBackupDataProgram((HUGEDATA INT32U *)NVRAM_BACKUP_FTL_TABLE_BACKUP_ADDR, (HUGEDATA INT32U *) &FtlManagement, NVRAM_BACKUP_FTL_TABLE_DSIZE);
//	}
////	FtlHist_Init(FtlManagement.TableCrc);
////	SYS_INT_SourceEnable(INT_SOURCE_EXTERNAL_0);
//}

//void FtlDataBackupCacheProgram(void)
//{
//	INT8U i;
//	SYS_INT_SourceDisable(INT_SOURCE_EXTERNAL_0);
//	WDT_Timer_Clear();
//	FtlDataBackupFlagProgram(FTL_BACKUP_FLAG_CRC_FLASH_CACHE);
//	for (i = NVRAM_BU_FLASH_CACHE_BL_ADDR; i < NVRAM_BU_FLASH_CACHE_BL_ADDR+NVRAM_BU_FLASH_CACHE_BL_SIZE; i++)
//	{
//	    SFlashBlockErase(FLASH1_CS,(INT16U *)(i * 0x1000));
//	}
//	FlashNVRAMStruct.TableCrc = crc_16((INT8U *)&FlashNVRAMStruct, NVRAM_BACKUP_CACHE_CRC_SIZE);
//	NvramBackupDataProgram((HUGEDATA INT32U *)NVRAM_BACKUP_FLASH_CACHE_ADDR, (HUGEDATA INT32U *) &FlashNVRAMStruct, NVRAM_BACKUP_FLASH_CACHE_DSIZE);
//	FlashNVRAMStruct2.TableCrc = crc_16((INT8U *)&FlashNVRAMStruct2, NVRAM_BACKUP_CACHE_CRC_SIZE);
//	NvramBackupDataProgram((HUGEDATA INT32U *)NVRAM_BACKUP_NVRAM_CACHE_ADDR, (HUGEDATA INT32U *) &FlashNVRAMStruct2, NVRAM_BACKUP_NVRAM_CACHE_DSIZE);
//
//#ifdef USE_DIRECT_INGREDIENT
//	NvramBackupDataProgram((HUGEDATA INT32U *)NVRAM_BACKUP_INGREDIENT_CACHE_ADDR, (HUGEDATA INT32U *) &FlashNVRAMCache2, NVRAM_BACKUP_INGREDIENT_CACHE_DSIZE);
//#endif
//	SYS_INT_SourceEnable(INT_SOURCE_EXTERNAL_0);
//}

void FtlDataBackupCacheProgram(void)
{
	INT8U i;
	
	External_Interrupt_Disable();
	WDT_Timer_Clear();
	FtlDataBackupFlagProgram(FTL_BACKUP_FLAG_CRC_FLASH_CACHE);

	Flash_flush_nvbuffer();
	Nvram_flush_nvbuffer();

#ifdef USE_DIRECT_INGREDIENT
	Flash_flush_nvbuffer_c2();
#endif
	External_Interrupt_Enable();
}

void FtlDataBackupReservedAreaProgram(void)
{
	INT8U i;
	External_Interrupt_Disable();
	FtlDataBackupFlagProgram(FTL_BACKUP_FLAG_ERASE_START_RESERVED);
	for (i = NVRAM_BU_RESERVED_BL_ADDR; i < NVRAM_BU_RESERVED_BL_ADDR+NVRAM_BU_RESERVED_BL_SIZE; i++) //Reserved area.
	{
		WDT_Timer_Clear();
		SFlashBlockErase(FLASH1_CS,(INT16U *)(i * 0x1000));
	}
	External_Interrupt_Enable();
}

void FtlDataBackupFinishFlagProgram(void)
{
	External_Interrupt_Disable();
	FtlDataBackupFlagProgram(FTL_BACKUP_FLAG_COMPLETE);
	External_Interrupt_Enable();
}

void FtlDataBackupAllDataProgram(INT8U includeReservedData)
{
	External_Interrupt_Disable();
	FtlDataBackupCacheProgram();            //FTL_BACKUP_FLAG_CRC_FLASH_CACHE          0xFFFF8419
	FtlDataBackupProgram();                 //FTL_BACKUP_FLAG_ERASE_TABLE              0xFFFF0419 => 0xFFFFFFFF (SectorErase)
	if (includeReservedData)
	{
		FtlDataBackupReservedAreaProgram(); //FTL_BACKUP_FLAG_ERASE_START_RESERVED     0xFFFFFFF9
	}
	FtlDataBackupFinishFlagProgram();       //FTL_BACKUP_FLAG_COMPLETE                 0xFFFFF419
	External_Interrupt_Enable();
	return;
}

#ifdef USE_EXTEND_FLASH
void FlashEraseNVRAMArea(void)
{
    INT8U i;
    INT32U start_addr;

    //000000H - 001FFFH(8 KByte))
    start_addr = 0x0;
    SQI_Flash_BlockErase(FLASH1_CS, start_addr);   
    //002000H - 003FFFH(8 KByte))
    start_addr = 0x2000;
    SQI_Flash_BlockErase(FLASH1_CS, start_addr);   
    //004000H - 005FFFH(8 KByte))
    start_addr = 0x4000;
    SQI_Flash_BlockErase(FLASH1_CS, start_addr);   
    //006000H - 007FFFH(8 KByte))
    start_addr = 0x6000;
    SQI_Flash_BlockErase(FLASH1_CS, start_addr);       
    //008000H - 00FFFFH(32 KByte))
    start_addr = 0x8000;
    SQI_Flash_BlockErase(FLASH1_CS, start_addr);       

    //010000H - 3FFFFFH 64 KByte x 63 
    start_addr = 0x10000;
    for(i = 0; i < 63; i++) {
        SQI_Flash_BlockErase(FLASH1_CS, start_addr + (0x10000 * i));
    }
}
#endif

void FtlDataBackupSetDefault(void)
{
	INT16U i;
	INT32U PsaStatusIndex;
	INT32U avgLifetime;

	External_Interrupt_Disable();
	memset(&FtlManagement.PsaStatus,0x00,sizeof(FtlManagement.PsaStatus));
	for(i = 0; i < MAX_PSA_QTY; i++)
	{
		FtlManagement.PsaStatus[i] = PSA_STATUS_FREE;
		if((FtlErrManagement.AvgLifeTime != 0xFFFFFFFF)&&(FtlErrManagement.AvgLifeTime != 0)&&(FtlErrManagement.AvgLifeTime < PSA_LIFE_TIME_INVALID))
		{
			FtlManagement.PsaEraseCnt[i] = FtlErrManagement.AvgLifeTime;
		}
		else
		{
			FtlManagement.PsaEraseCnt[i] = 1;
		}
	}
	for(i = 0; i < MAX_LSA_QTY; i++)
	{
		FtlManagement.AssignAddr[i] = (i * DFLASH_BLOCK_SIZE) + NVRAM_BASE;
		PsaStatusIndex = (FtlManagement.AssignAddr[i] - NVRAM_BASE) / DFLASH_BLOCK_SIZE;
		FtlManagement.PsaStatus[PsaStatusIndex] = PSA_STATUS_USING;
	}
	FtlManagement.TableCrc = crc_16((INT8U*)&FtlManagement,NVRAM_BACKUP_FTL_TABLE_CRC_SIZE);

	if ((FtlErrManagement.AvgLifeTime == 0xFFFFFFFF) || (FtlErrManagement.AvgLifeTime == 0) || (FtlErrManagement.AvgLifeTime > PSA_LIFE_TIME_INVALID))
	{
		FtlErrManagement.AvgLifeTime  = 1;
	}
	if ((FtlErrManagement.FtlTableErrCnt == 0xFFFFFFFF) || (FtlErrManagement.FtlTableErrCnt == 0))
	{
		FtlErrManagement.FtlTableErrCnt  = 0;
	}
	External_Interrupt_Enable();
//	if (FtlErrManagement.ResetBrownOut == 0xFFFFFFFF) FtlErrManagement.ResetBrownOut = 0;
//	if (FtlErrManagement.ResetMCLR == 0xFFFFFFFF) FtlErrManagement.ResetMCLR = 0;
//	if (FtlErrManagement.ResetPowerOn == 0xFFFFFFFF) FtlErrManagement.ResetPowerOn = 0;
//	if (FtlErrManagement.ResetSoftware == 0xFFFFFFFF) FtlErrManagement.ResetSoftware = 0;
//	if (FtlErrManagement.ResetWDT == 0xFFFFFFFF) FtlErrManagement.ResetWDT = 0;

//	FlashNVRAMStruct.flag = 0;
//	FlashNVRAMStruct.addr = 0;
//	FlashNVRAMStruct2.flag = 0;
//	FlashNVRAMStruct2.addr = 0;

	WDT_Timer_Disable();
	WDT_Timer_Clear();
#ifdef USE_EXTEND_FLASH
    if (Flash1ExtFlag == 1) {
        FlashEraseNVRAMArea();
    } else {
        SQI_Flash_ChipErase(FLASH1_CS);
    }
#else    
	SQI_Flash_ChipErase(FLASH1_CS);
#endif
	WDT_Timer_Enable();
	
	FtlDataBackupAllDataProgram(ON);
	FtlDataBackupFlagProgram(FTL_INIT_FLAG_COMPLETE);
}

void FTL_Init_Error_Management(void)
{
	Flash_sread(EXT_MEMCARD_ADDR_X+FLASH_FTL_ERR_MANAGEMENT_AREA,(INT8U *)&FtlErrManagement,sizeof(FtlErrManagement));
	External_Interrupt_Disable();
	if (FtlErrManagement.Flag != 0x19830419)
	{
		memset(&FtlErrManagement,0x00,sizeof(FtlErrManagement));
	}
	External_Interrupt_Enable();
}

void FTL_Save_Error_Management(void)
{
	 // Flash에 저장.
	External_Interrupt_Disable();
	Flash_swrite(EXT_MEMCARD_ADDR_X+FLASH_FTL_ERR_MANAGEMENT_AREA,(INT8U *)&FtlErrManagement,sizeof(FtlErrManagement));
	Flash_init_nvbuffer();
	External_Interrupt_Enable();
}

void FTL_Load_Cache_Error_Count(void)
{
	External_Interrupt_Disable();
	NvramBackupDataRead(NVRAM_BU_CACHE_CRC_ERROR_BL_ADDR * 0x1000, (HUGEDATA INT32U *)&cacheCrcErrCount, NVRAM_BACKUP_CACHE_CRC_ERROR_DSIZE);
	if(cacheCrcErrCount.FtlTableErrCnt == 0xFFFFFFFF) cacheCrcErrCount.FtlTableErrCnt = 0;
	if(cacheCrcErrCount.FtlFlashCacheErrCnt == 0xFFFFFFFF) cacheCrcErrCount.FtlFlashCacheErrCnt = 0;
	if(cacheCrcErrCount.FtlNvramCacheErrCnt == 0xFFFFFFFF) cacheCrcErrCount.FtlNvramCacheErrCnt = 0;
	if(cacheCrcErrCount.IngredientCacheErrCnt == 0xFFFFFFFF) cacheCrcErrCount.IngredientCacheErrCnt = 0;
	External_Interrupt_Enable();
}

void FTL_Save_Cache_Error_Count(void)
{
	External_Interrupt_Disable();
	SFlashBlockErase(FLASH1_CS,(INT16U *)(NVRAM_BU_CACHE_CRC_ERROR_BL_ADDR * 0x1000));
	NvramBackupDataProgram((HUGEDATA INT32U *)(NVRAM_BU_CACHE_CRC_ERROR_BL_ADDR * 0x1000), (HUGEDATA INT32U *) &cacheCrcErrCount, NVRAM_BACKUP_CACHE_CRC_ERROR_DSIZE);
	External_Interrupt_Enable();
}

// NVRam Flash 수명 Spec.
// <가정> (영공기준)
//  라벨 판매 : 1일 발행량 1000회(시장에 따라 다름)
//             1회 라벨 발행당 평균 13블럭 사용
//  티켓 판매 : 1일 영수증 발행량 500회, 1회 영수증 발행당 판매수 10회 (10개 item 판매 후 티켓 출력) (시장에 따라 다름)
//             1회 영수증 발행당 평균 100블럭 사용
// <계산>
//  라벨 판매 : 1000블럭 x 100000회 / 10블럭 / 1000회 / 365일 = 약 27.4년
//  티켓 판매 : 1000블럭 x 100000회 / 100블럭 / 500회 / 365일 = 약 5.5년
/*
INT8U FTL_Check_Lifetime(void)
{
	INT32U i;
	INT32U sumLifeTime;
	INT32U dyingSectorCnt;
	INT8U ErrFlag;
	INT32U max;
	INT32U min;
	
	dyingSectorCnt = 0;
	sumLifeTime = 0;
	ErrFlag = 0;
	max = 0;
	min = 0xFFFFFFFF;
	External_Interrupt_Disable();
	for (i = 0; i < MAX_PSA_QTY; i++ )
	{
		sumLifeTime += FtlManagement.PsaEraseCnt[i];
		if (FtlManagement.PsaEraseCnt[i] > 99000) 
		{
			dyingSectorCnt++;
		}
		if (FtlManagement.PsaEraseCnt[i] > max) max = FtlManagement.PsaEraseCnt[i];
		if (FtlManagement.PsaEraseCnt[i] < min) min = FtlManagement.PsaEraseCnt[i];
	}
	
	FtlErrManagement.Flag = 0x19830419;
	FtlErrManagement.AvgLifeTime = sumLifeTime/MAX_PSA_QTY;
	FtlMaxLifeTime = max;
	FtlMinLifeTime = min;
	if (FtlErrManagement.AvgLifeTime > FTL_REF_LIFETIME_3_MONTH)
	{
		ErrFlag = FTL_MEM_LIFETIME_3_MONTH;
	}
	if (FtlErrManagement.AvgLifeTime > FTL_REF_LIFETIME_1_MONTH || dyingSectorCnt > MAX_PSA_QTY / 4)
	{
		ErrFlag = FTL_MEM_LIFETIME_1_MONTH;
	}
	External_Interrupt_Enable();
	FTL_Save_Error_Management();
	return ErrFlag;
}
*/
INT8U FTL_Check_Lifetime(void)
{
	INT32U i;
	INT32U sumLifeTime;
	INT32U remain3monthCnt;
	INT32U remain1monthCnt;
	INT8U errFlag;
	INT32U max;
	INT32U min;
	
	remain3monthCnt = 0;
	remain1monthCnt = 0;
	sumLifeTime = 0;
	errFlag = 0;
	max = 0;
	min = 0xFFFFFFFF;
	External_Interrupt_Disable();
	for (i = 0; i < MAX_PSA_QTY; i++ )
	{
		sumLifeTime += FtlManagement.PsaEraseCnt[i];
		if (FtlManagement.PsaEraseCnt[i] >= FTL_REF_LIFETIME_3_MONTH) //0x17318 == 95000 
		{
			remain3monthCnt++;
		}
		if (FtlManagement.PsaEraseCnt[i] >= FTL_REF_LIFETIME_1_MONTH) //0x17CDC == 98000
		{
			remain1monthCnt++;
		}
		if (FtlManagement.PsaEraseCnt[i] > max) max = FtlManagement.PsaEraseCnt[i];
		if (FtlManagement.PsaEraseCnt[i] < min) min = FtlManagement.PsaEraseCnt[i];
	}
	
	if ((remain3monthCnt >= 500)&&(remain1monthCnt < 500)) //Warning Flag On 3 month
	{
		errFlag = FTL_MEM_LIFETIME_3_MONTH;
	}
	else if (remain1monthCnt >= 500) //Warning Flag on 1 Month
	{
		errFlag = FTL_MEM_LIFETIME_1_MONTH;
	}
	FtlErrManagement.Flag = 0x19830419;
	FtlErrManagement.AvgLifeTime = sumLifeTime/MAX_PSA_QTY;
	FtlMaxLifeTime = max;
	FtlMinLifeTime = min;
	External_Interrupt_Enable();
	FTL_Save_Error_Management();
	return errFlag;
}

INT32U FtlHist_Index;
void FtlHist_Init(INT16U crc)
{
	FTL_HISTORY_HEAD_STRUCT histHead;
	INT32U i;
	External_Interrupt_Disable();
	// clear hisotry
	for(i = NVRAM_BU_FTL_HISTORY_BL_ADDR; i < NVRAM_BU_FTL_HISTORY_BL_ADDR+NVRAM_BU_FTL_HISTORY_BL_SIZE; i++)
	{
	    SFlashBlockErase(FLASH1_CS,(INT16U *)(i * 0x1000));
	}
	histHead.Crc = crc;
	histHead.Flag = FTL_HISTORY_INIT;
	NvramBackupDataProgram((INT32U *)NVRAM_BACKUP_FTL_HISTORY_ADDR, (INT32U *)&histHead, 1);
	FtlHist_Index = 0;
	External_Interrupt_Enable();
}

FTL_HISTORY_HEAD_STRUCT FtlHist_ReadHead(void)
{
	External_Interrupt_Disable();
	FTL_HISTORY_HEAD_STRUCT histHead;
	NvramBackupDataRead(NVRAM_BACKUP_FTL_HISTORY_ADDR, (HUGEDATA INT32U *)&histHead, 1);
	External_Interrupt_Enable();
	return histHead;
}

void FtlHist_AddHistory(INT16U laddr, INT16U paddr)
{
	FTL_HISTORY_STRUCT hist;
	FTL_HISTORY_HEAD_STRUCT histHead;
	External_Interrupt_Disable();
	hist.LogicalAddr = laddr;
	hist.PhysicalAddr = paddr;
	
	if (FtlHist_Index >= FTL_MAX_HISTORY_NUM) 
	{
//MsgOut("FTL HISTORY FULL\r\n");
		//backup
		FtlDataBackupProgram();                 //FTL_BACKUP_FLAG_ERASE_TABLE              0xFFFF0419 => 0xFFFFFFFF (SectorErase)
		FtlDataBackupFlagProgram(FTL_INIT_FLAG_COMPLETE);
	}
//sprintf(MsgBuf, "  ADD HISTORY [%d](%d,%d)\r\n", FtlHist_Index, hist.LogicalAddr, hist.PhysicalAddr);
//MsgOut(MsgBuf);
	if (FtlHist_Index == 0)
	{
		NvramBackupDataRead(NVRAM_BACKUP_FTL_HISTORY_ADDR, (HUGEDATA INT32U *)&histHead, 1);
		histHead.Flag = FTL_HISTORY_WRITTEN;
		NvramBackupDataProgram((INT32U *)NVRAM_BACKUP_FTL_HISTORY_ADDR, (INT32U *)&histHead, 1);		
	}
	NvramBackupDataProgram((INT32U *)(NVRAM_BACKUP_FTL_HISTORY_ADDR+4*(1+FtlHist_Index)), (INT32U *)&hist, 1);
	FtlHist_Index++;
	External_Interrupt_Enable();
}

// booting 초기에 실행되는 함수이므로 Backup 허용되지 않는 경우(FtlBackupEnableFlag가 OFF)에 실행됨
// FtlBackupEnableFlag이 ON인 상태에서 실행될 경우 Power Off를 위한 외부인터럽트를 disable 시킴(방어)
INT8U FtlHist_RestoreHistory(void)
{
	INT32U i;
	INT32U usedPsaIndex;
	FTL_HISTORY_STRUCT hist;

//MsgOut("FTL HISTORY RESTORE\r\n");
	if (FtlBackupEnableFlag) External_Interrupt_Disable(); // Power off Pending
	for (i = 0; i < FTL_MAX_HISTORY_NUM; i++)
	{
		NvramBackupDataRead((INT32U)NVRAM_BACKUP_FTL_HISTORY_ADDR+4*(1+i), (INT32U *)&hist, 1);
		if (hist.LogicalAddr == 0xffff) break;
		if (hist.LogicalAddr >= MAX_LSA_QTY) return false;
		if (hist.PhysicalAddr >= MAX_PSA_QTY) return false;
//sprintf(MsgBuf, "  RES HISTORY [%d](%d,%d)\r\n", i, hist.LogicalAddr, hist.PhysicalAddr);
//MsgOut(MsgBuf);
		
		usedPsaIndex = (FtlManagement.AssignAddr[hist.LogicalAddr] - NVRAM_BASE) / DFLASH_BLOCK_SIZE; //PSA Index
		if (usedPsaIndex >= MAX_PSA_QTY) return false;
		FtlManagement.PsaStatus[usedPsaIndex] = PSA_STATUS_USED;
		FtlManagement.PsaStatus[hist.PhysicalAddr] = PSA_STATUS_USING;
		FtlManagement.AssignAddr[hist.LogicalAddr] = (INT32U)hist.PhysicalAddr * DFLASH_BLOCK_SIZE + NVRAM_BASE;
		FtlManagement.TableCrc = crc_16((INT8U*)&FtlManagement,NVRAM_BACKUP_FTL_TABLE_CRC_SIZE);
		if ((i % 200) == 0) WDT_Timer_Clear();
	}
	if (FtlBackupEnableFlag) External_Interrupt_Enable();
	return true;
}

void Init_Ftl(void)
{
	INT16U FtlTableCRC;
	//	INT8U resetFlag; //test code
	INT8U FtlTableCrcErr;
	INT32U ftlBackupFlag = 0x00000000; //FTL Table의 상태를 나타내는 Flag;
	FTL_HISTORY_HEAD_STRUCT histHead;
	INT8U useBackup;
	INT32U flag;
	INT8U buffer[HISTORY_BUF_SIZE];


	useBackup = 0;
	FtlTableCrcErr = 0;
	
	// Check NON_REMOVABLE_AREA_BACKUP & Restore
	NvramBackupDataRead(NVRAM_BACKUP_TEMPORARY_ADDR, &flag, 1);
	if (flag == NVRAM_BACKUP_FLAG_HISTORY_WRITTEN) // Flash Chip erase 중 비정상 종료된 경우
	{
		// read backup
		NvramBackupDataRead(NVRAM_BACKUP_TEMPORARY_ADDR+4, (INT32U*)buffer, HISTORY_BUF_SIZE/4);
		// restore backup
		history_ram2flash(buffer);
		// write restore completion flag
		flag = NVRAM_BACKUP_FLAG_HISTORY_INIT;
		NvramBackupDataProgram((INT32U*)NVRAM_BACKUP_TEMPORARY_ADDR, &flag, 1);
	}
	/////////////////////////////////////////////////
	
	//READ FTL Error Management DATA
    FTL_Init_Error_Management();
	FTL_Load_Cache_Error_Count();
	//READ FTL Error Management DATA

//	resetFlag = RESET_Get_Reason();
//
//	switch(resetFlag)
//	{
//		case SCALE_RESET_REASON_NONE:
//			MsgOut("SCALE_RESET_REASON_NONE\r\n");
//			break;
//		case SCALE_RESET_REASON_POWERON:
//			MsgOut("SCALE_RESET_REASON_POWERON\r\n");
//			break;
//		case SCALE_RESET_REASON_BROWNOUT:
//			MsgOut("SCALE_RESET_REASON_BROWNOUT\r\n");
//			break;
//		case SCALE_RESET_REASON_WDT_TIMEOUT:
//			MsgOut("SCALE_RESET_REASON_WDT_TIMEOUT\r\n");
//			break;
//		case SCALE_RESET_REASON_DMT_TIMEOUT:
//			MsgOut("SCALE_RESET_REASON_DMT_TIMEOUT\r\n");
//			break;
//		case SCALE_RESET_REASON_SOFTWARE:
//			MsgOut("SCALE_RESET_REASON_SOFTWARE\r\n");
//			break;
//		case SCALE_RESET_REASON_MCLR:
//			MsgOut("SCALE_RESET_REASON_MCLR\r\n");
//			break;
//		case SCALE_RESET_REASON_CONFIG_MISMATCH:
//			MsgOut("SCALE_RESET_REASON_CONFIG_MISMATCH\r\n");
//			break;
//	}

	//RAM DATA CRC CHECK START/////////////////////////////////////////////
	FtlTableCRC = crc_16((INT8U *)&FtlManagement, NVRAM_BACKUP_FTL_TABLE_CRC_SIZE);

	if (FtlManagement.TableCrc != FtlTableCRC) FtlTableCrcErr = 1;
	//RAM DATA CRC CHECK END///////////////////////////////////////////////
	
	//RAM DATA IS VALID////////////////////////////////////////////////////
	if (!FtlTableCrcErr)
	{
		goto END;
		return;
	}
	//RAM DATA IS VALID////////////////////////////////////////////////////

  //READ FLASH DATA & VALID CHECK////////////////////////////////////////
	FtlDataBackupFlagRead(&ftlBackupFlag);
	
	FtlDataBackupTableRead(0);
	histHead = FtlHist_ReadHead();
	FtlTableCRC = crc_16((INT8U *)&FtlManagement, NVRAM_BACKUP_FTL_TABLE_CRC_SIZE);
	FtlTableCrcErr = 0;
	
	if (FtlManagement.TableCrc != FtlTableCRC) // FtlManagement CRC가 오류
	{
		FtlDataBackupTableRead(1); // 2nd backup table read
		FtlTableCRC = crc_16((INT8U *)&FtlManagement, NVRAM_BACKUP_FTL_TABLE_CRC_SIZE);
		FtlTableCrcErr = 0;
		if (FtlManagement.TableCrc != FtlTableCRC) FtlTableCrcErr = 1; // table 2개 다 깨졌을 경우 초기화 해야함(아래에서 진행)
	}

	if (!FtlTableCrcErr) // FtlManagement CRC 정상
	{
		if (histHead.Flag == FTL_HISTORY_WRITTEN) // History가 존재
		{
			if (histHead.Crc == FtlManagement.TableCrc) // FtlManagement이 old table임. history 복구 필요
			{
				FtlErrManagement.FtlInterruptErrCnt++; //정상 Backup이 되지 않고 히스토리만 남은 경우.(복구가능)
				if (!FtlHist_RestoreHistory()) // history 복구
				{
					FtlTableCrcErr = 1;
				}
				useBackup = 1; // history 적용된 table을 backup
			}
		}
	}	
	
	//FLASH DATA IS VALID////////////////////////////////////////////////////
	if (!FtlTableCrcErr)
	{
		switch (ftlBackupFlag)
		{
			case FTL_INIT_FLAG_COMPLETE: //History 적용 Table 이 있는 경우.
				if(!useBackup) break;   
			case FTL_BACKUP_FLAG_PASS_BACKUP: // Booting 후 4초 이내 Power OFF. 4초 이내의 변경 data는 무시
			case FTL_BACKUP_FLAG_COMPLETE:    // 정상 Backup
			case FTL_BACKUP_FLAG_FTL_TABLE_CRC_ERROR: //Backup 당시 FTL Table CRC가 깨져있는 상태라서 CRC 체크 안함.
				goto END;
				break;
			default:
				break;
		}
	}
	//FLASH DATA IS VALID////////////////////////////////////////////////////

	// Check to backup Flag START////////////////////////////////////////////
//	switch (ftlBackupFlag)
//	{
//		default:
//		case FTL_INIT_FLAG_COMPLETE:
//			// Backup이 되지 않은 상태.
//		case FTL_BACKUP_FLAG_ERASE_TABLE:
//			// CRC가 정상일 경우 위에서 복구 완료.
//			// CRC가 오류일 경우 아래에서 초기화.
//		case FTL_BACKUP_FLAG_CRC_FLASH_CACHE:
//			// CRC가 정상이어도 데이터는 과거 데이터로 판단. 초기화.
//		case FTL_BACKUP_FLAG_CACHE_CRC_ERROR:
//			//Cache CRC가 깨져 Cache를 Backup 하지 않은 상황			
//		case FTL_BACKUP_FLAG_ERASE_START_RESERVED:
//			// 현재 미사용이므로 복구 불필요.
//		case FTL_BACKUP_FLAG_COMPLETE:
//			// 정상 Backup. CRC 오류가 있는 경우만 초기화.
//		case FTL_BACKUP_FLAG_PASS_BACKUP: 
//			// Booting 후 4초 이내 Power OFF시 Backup 안 한 경우.
//			// CRC 오류가 있는 경우만 초기화.
//		case FTL_BACKUP_FLAG_FTL_TABLE_CRC_ERROR:
//			//Table CRC가 깨져 Table Backup을 진행하지 않고 복구한 상황
//		case FTL_BACKUP_FLAG_ONLY_FTL_BACKUP:
//			//Cache CRC에 문제가 있고 Table CRC는 정상적으로 Backup 된 상황.
//		case FTL_BACKUP_FLAG_ONLY_CACHE_BACKUP:
//			//Table CRC에 문제가 있고 Cache CRC는 정상적으로 Backup 된 상황.
//		
//			break;
//	}
    // check to backup Flag END////////////////////////////////////////////
	
	//Set default INVALID DATA/////////////////////////////////////////////
	if (FtlTableCrcErr)
	{
		FtlErrManagement.FtlTableErrCnt++;
		FtlDataBackupSetDefault();
	}
END:
	if (useBackup)
	{
		FtlDataBackupAllDataProgram(OFF);
	}
	FtlDataBackupFlagProgram(FTL_INIT_FLAG_COMPLETE);
}

#endif //#ifdef CPU_PIC32MZ
