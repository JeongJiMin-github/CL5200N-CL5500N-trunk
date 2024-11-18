/*****************************************************************************
|*			
|*  Copyright		:	(c) 2002 CAS
|*  Filename		:	flash.h
|*  Version			:	0.1
|*  Programmer(s)	:	Hong Yi-Phyo (HYP)
|*  Created			:	2002/11/11
|*  Description		:	CLP Printer Flash memory routine header    
|*				
****************************************************************************/

#ifndef _FLASH_H
#define _FLASH_H

#define FLASH0_CS SQI_CS_0
#define FLASH1_CS SQI_CS_1

#define CACHE1 0 // Flash의 Cache1(기본 Cache)
#define CACHE2 1 // Flash의 Cache2(Ingredient용 Cache)

//#define MAX_PSA_QTY  1020  //4의 배수로 적용해야 함.
#define MAX_PSA_QTY          1000
#define MAX_LSA_QTY          256
#define LSA_INDEX_OFFSET     0x18 //(NVRAM_BASE >> 12)

#define FTL_MEM_LIFETIME_1_MONTH    0x01
#define FTL_MEM_LIFETIME_3_MONTH    0x02

#define FTL_REF_LIFETIME_3_MONTH    95000
#define FTL_REF_LIFETIME_1_MONTH    98000

// State(1byte), action(1byte),fixed 2byte(0419)
// State와 Action은 bit 1 -> 0으로 설정되도록 셋팅해야함.(Flash에 Erase 없이 Write하려는 목적임.)

//Backup에 들어갈 때 상태
// 1. Booting 시 설정 가능한 Flag
#define FTL_INIT_FLAG_COMPLETE                   0xFFFFE419

// 2. 종료/리셋시 설정 가능한 Flag
// (1) 빨리 꺼서 Bakcup 안 할 경우
#define FTL_BACKUP_FLAG_PASS_BACKUP              0xFFFFC419
// (2) 종료/리셋시 정상 Backup 함
#define FTL_BACKUP_FLAG_CRC_FLASH_CACHE          0xFFFF8419
#define FTL_BACKUP_FLAG_ERASE_TABLE              0xFFFF0419
#define FTL_BACKUP_FLAG_ERASE_DONE               0xFFFFFFFF
#define FTL_BACKUP_FLAG_ERASE_START_RESERVED     0xFFFFFFF9
#define FTL_BACKUP_FLAG_COMPLETE                 0xFFFFF419
// (3) 종료시 FTL TABLE이 깨져서 Backup 안 함
#define FTL_BACKUP_FLAG_FTL_TABLE_CRC_ERROR      0xFFFC0419

//#define FTL_BACKUP_FLAG_RESERVED_1          0xFFF80419
//#define FTL_BACKUP_FLAG_RESERVED_2          0xFFF00419
//#define FTL_BACKUP_FLAG_RESERVED_3          0xFFE00419
//#define FTL_BACKUP_FLAG_RESERVED_4          0xFFC00419
//#define FTL_BACKUP_FLAG_RESERVED_5          0xFFC00419
//#define FTL_BACKUP_FLAG_RESERVED_6          0xFF800419
//#define FTL_BACKUP_FLAG_RESERVED_7          0xFF000419
//#define FTL_BACKUP_FLAG_RESERVED_8          0xFE000419
//#define FTL_BACKUP_FLAG_RESERVED_9          0xFC000419
//#define FTL_BACKUP_FLAG_RESERVED_10         0xF8000419
//#define FTL_BACKUP_FLAG_RESERVED_11         0xF0000419


#pragma pack(4)
typedef struct
{
    INT32U AssignAddr[MAX_LSA_QTY];	// 1024
    INT32U PsaEraseCnt[MAX_PSA_QTY]; // 4000
    INT8U PsaStatus[MAX_PSA_QTY]; // 1000
    INT32U PsaWritePtr; // 4
    INT32U PsaErasePtr; // 4
    INT16U TableCrc; // 2
    INT8U Reserved[2]; // 2
	INT8U Dummy[12]; // 12  // Cache 충돌을 막기 위해 16byte align을 하기 위한 Dummy
} FTL_MANAGEMENT_STRUCT;

typedef struct
{
    INT16U Flag;
    INT16U Crc;
} FTL_HISTORY_HEAD_STRUCT;

typedef struct
{
    INT16U LogicalAddr;
    INT16U PhysicalAddr;
} FTL_HISTORY_STRUCT;
#pragma pack()

#pragma pack(4)
typedef struct
{
    INT32U Flag;
    INT32U AvgLifeTime;
    INT32U FtlTableErrCnt;
    INT32U FtlFalshCacheErrCnt;
    INT32U FtlNvramCacheErrCnt;
    INT32U IngredientCacheErrCnt;
    INT32U FtlInterruptErrCnt;
    INT32U FtlFlagErrCnt;
    INT8U Reserved[16];
//    INT32U ResetPowerOn;
//    INT32U ResetBrownOut;
//    INT32U ResetWDT;
//    INT32U ResetSoftware;
//    INT32U ResetMCLR;
}FTL_ERROR_MANAGEMENT_STRUCT;

typedef struct
{
    INT32U FtlTableErrCnt;
    INT32U FtlFlashCacheErrCnt;
    INT32U FtlNvramCacheErrCnt;
    INT32U IngredientCacheErrCnt;

}CACHE_ERROR_COUNT_STRUCT;
#pragma pack()

#define FTL_ERROR_MANAGEMENT_SIZE 48
#define FLASH_USB_DATA_ROM_UPDATE_SIZE 1

extern HUGEDATA FTL_MANAGEMENT_STRUCT FtlManagement;
extern FTL_ERROR_MANAGEMENT_STRUCT FtlErrManagement;
extern CACHE_ERROR_COUNT_STRUCT cacheCrcErrCount;
#define PSA_STATUS_FREE      0
#define PSA_STATUS_USING     1
#define PSA_STATUS_USED      2
#define PSA_STATUS_INVALID   3

#define PSA_LIFE_TIME_INVALID 0x000189A0 //10만번.


// Memory Map(Flash Translation Layer)
//  0x00000000 : FTL Table 1 & Flag
//  0x00002000 : Flash Cache & Nvram Cache & Ingredient Cache
//  0x00005000 : FTL Table 2 (backup)
//  0x00007000 : FTL History
//  0x00009000 : reserved
//  0x00017000 : Tempary Area(NON_REMOVABLE_DATA_BACKUP)
//  0x00018000 : NVRAM Area

// Block Address & Size
#define NVRAM_BU_FTL_TABLE_BL_ADDR        0
#define NVRAM_BU_FTL_TABLE_BL_SIZE        2

#define NVRAM_BU_FLASH_CACHE_BL_ADDR      2
#define NVRAM_BU_FLASH_CACHE_BL_SIZE      3

#define NVRAM_BU_FTL_TABLE_BACKUP_BL_ADDR 5
#define NVRAM_BU_FTL_TABLE_BACKUP_BL_SIZE 2

#define NVRAM_BU_FTL_HISTORY_BL_ADDR      7
#define NVRAM_BU_FTL_HISTORY_BL_SIZE      2

#define NVRAM_BU_CACHE_CRC_ERROR_BL_ADDR  9
#define NVRAM_BU_CACHE_CRC_ERROR_BL_SIZE  1

//#define NVRAM_BU_RESERVED_BL_ADDR         9
//#define NVRAM_BU_RESERVED_BL_SIZE         14

#define NVRAM_BU_RESERVED_BL_ADDR         10
#define NVRAM_BU_RESERVED_BL_SIZE         13

#define NVRAM_BU_TEMPORARY_BL_ADDR        23
#define NVRAM_BU_TEMPORARY_BL_SIZE        1

#define NVRAM_BL_ADDR                     24 // 0x18

// Address 
#define NVRAM_BACKUP_FTL_TABLE_ADDR           NVRAM_BU_FTL_TABLE_BL_ADDR
#define NVRAM_BACKUP_FLAG_ADDR                (NVRAM_BACKUP_FTL_TABLE_ADDR + sizeof(FTL_MANAGEMENT_STRUCT)) //Phsical address 0x1794  6036 byte

#define NVRAM_BACKUP_FTL_TABLE_BACKUP_ADDR    (0x1000*NVRAM_BU_FTL_TABLE_BACKUP_BL_ADDR)

#define NVRAM_BACKUP_FTL_HISTORY_ADDR         (0x1000*NVRAM_BU_FTL_HISTORY_BL_ADDR)

#define NVRAM_BACKUP_FLASH_CACHE_ADDR         (0x1000*NVRAM_BU_FLASH_CACHE_BL_ADDR) // 8192 //Flash Memory의 3번째 Block
#define NVRAM_BACKUP_NVRAM_CACHE_ADDR         (NVRAM_BACKUP_FLASH_CACHE_ADDR + sizeof(FLASH_NVRAM_STRUCT))
#define NVRAM_BACKUP_INGREDIENT_CACHE_ADDR    (NVRAM_BACKUP_NVRAM_CACHE_ADDR + sizeof(FLASH_NVRAM_STRUCT))

#define NVRAM_BACKUP_TEMPORARY_ADDR           (0x1000*NVRAM_BU_TEMPORARY_BL_ADDR) // NON_REMOVABLE_DATA Backup 용

#define FTL_MANAGEMENT_STRUCT_SIZE  (sizeof(FTL_MANAGEMENT_STRUCT)-12)  // Flash에 Backup하는 Size - Dummy(12byte)를 뺀 size
#define FLASH_NVRAM_STRUCT_SIZE     (sizeof(FLASH_NVRAM_STRUCT)-4)  // Flash에 Backup하는 Size - Dummy(4byte)를 뺀 size

#define NVRAM_BACKUP_FTL_TABLE_DSIZE          (FTL_MANAGEMENT_STRUCT_SIZE/4)//1509
#define NVRAM_BACKUP_FLASH_CACHE_DSIZE        (FLASH_NVRAM_STRUCT_SIZE/4) //1027
#define NVRAM_BACKUP_NVRAM_CACHE_DSIZE        (FLASH_NVRAM_STRUCT_SIZE/4) //1027
#define NVRAM_BACKUP_INGREDIENT_CACHE_DSIZE   (FLASH_NVRAM_STRUCT_SIZE/4) //1027
#define NVRAM_BACKUP_CACHE_CRC_ERROR_DSIZE    (sizeof(CACHE_ERROR_COUNT_STRUCT)/4)

#define NVRAM_BACKUP_FTL_TABLE_CRC_SIZE          (FTL_MANAGEMENT_STRUCT_SIZE - 4) // -4 는 FTL_MANAGEMENT_STRUCT에 포함된 CRC 변수 size임.
#define NVRAM_BACKUP_CACHE_CRC_SIZE              (FLASH_NVRAM_STRUCT_SIZE - 4) // -4 는 FLASH_NVRAM_STRUCT에 포함된 CRC 변수 size임.


#define PFLASH_DATA_BASE	0x0;

#define DFLASH_BASE			0x0		   	/* base address of data flash memory */
//#define DFLASH_BASE_SECTOR		0x3C0000		   		/* base address of data flash memory */

#define FLASH_NVRAM_SIZE               0x400000
#ifdef USE_EXTEND_FLASH
//CS0 32Mbyte 확장가능, NVRAM 영역보호
#define DFLASH_LOGICAL_FLASH2_SIZE     (0x800000 - FLASH_NVRAM_SIZE) //(8M - FLASH_NVRAM_SIZE)
#define DFLASH_LOGICAL_FLASH2_BASE     0x2000000 //32
#endif

#define DFLASH_BLOCK_SIZE			0x1000	//4KB
#define DFLASH_BLOCK_WSIZE			0x800	//
#define DFLASH_BLOCK_DSIZE		    0x0400
#define FLASH_BLOCK_HADDR_MASK		0xfffff000 //0xffffe000
#define FLASH_BLOCK_LADDR_MASK		0x00000fff //0x00001fff

//#define DFLASH_EXT_BASE_SECTOR	0x9000		   		/* base address of data flash memory */
#ifdef USE_EXTEND_8M_DFLASH
#define DFLASH_VOLUME			0x800000		   //29F040
  #ifdef USE_8M_EXPAND_BACKUP_RESTORE
#define DFLASH_RESTORE_VOLUME	0x800000
  #else
#define DFLASH_RESTORE_VOLUME	0x400000
  #endif
#else
#define DFLASH_VOLUME			0x400000		   //29F040
#define DFLASH_RESTORE_VOLUME	0x400000
#endif
#define DFLASH_SECTOR_SIZE		0x100		//128*2
#define DFLASH_SECTOR_WSIZE		0x80
#define	DFLASH_SECTOR_NUM		0x1000	// sector numbers per 1 chip(1MB)

#define EXT_MEMCARD_ADDR	0x200000
#define EXT_MEMCARD_ADDR_X	0x200000

#ifdef USE_EXTEND_8M_DFLASH
#define EXT_MEMCARD2_ADDR	0x400000
#define EXT_MEMCARD2_ADDR_X	0x400000
#endif

#ifdef USE_8M_EXPAND_BACKUP_RESTORE
#define FLASH_BACKUP_VOLUME		0x400000
#define MEMCARD_BACKUP_VOLUME	0x400000
#else 
#define FLASH_BACKUP_VOLUME		0x200000
#define MEMCARD_BACKUP_VOLUME	0x200000
#endif

#define RESTORE_VOLUME_8M		0x800000
#define RESTORE_VOLUME_4M		0x400000

#define DFLASH_CHIP_MAX_NUM		1//8		//max 8MB


// FTL Table history management dwefines
#define FTL_HISTORY_INIT    0x9876 // 1001 1000 0111 0110
#define FTL_HISTORY_WRITTEN 0x1032 // 0001 0000 0011 0010
#define FTL_MAX_HISTORY_NUM ((DFLASH_BLOCK_SIZE*2-4)/4) // except header 4bytes


//#define EXT_MEMCARD_SIZE	0
#define EXT_MEMCARD_SIZE	2
#define DFLASH_SECTOR_ERASE_TIME_LIMIT	 	(100*6250)   // Max sector erase cycle time 25 ms  //48
#define DFLASH_CHIP_ERASE_TIME_LIMIT		(100*25000)	 // Max chip erase cycle time 100 ms
#define DFLASH_PROGRAM_TIME_LIMIT			(100*1250) 	 // Max cycle time 20 us????
#define PFLASH_SECTOR_ERASE_TIME_LIMIT		(100*2000000) // Max sector erase cycle time 8 s	
#define PFLASH_CHIP_ERASE_TIME_LIMIT		(100*2750000)	 // Typ. chip erase cycle time 11 s
#define PFLASH_PROGRAM_TIME_LIMIT		  	(100*125000) 	 // Max cycle time 500 us

#define FLASH_SUCCESS		0
#define FLASH_ERASE_ERR		0xff
#define FLASH_SEC_ERASE_ERR	0xff
#define FLASH_PROGRAM_ERR	0xff

#define DFLASH_SEC_ADDR(sec_num)	((INT32U)sec_num*DFLASH_SECTOR_SIZE)
#define DFLASH_BASE_ADDR(chip_num)	(DFLASH_BASE + chip_num*DFLASH_VOLUME)

#ifdef USE_CHN_EMART_BIRDS_TRACE
#define QR_CODE_TRACE_ADDR EXT_MEMCARD_ADDR_X
#endif

#ifdef USE_DIRECT_NUTRITION
  #ifdef USE_NORTH_AMERICA_INGREDIENT
#define FLASH_DIRECT_NUTRIFACT_AREA EXT_MEMCARD2_ADDR_X + FLASH_NUTRIFACT_AREA
  #else
#define FLASH_DIRECT_NUTRIFACT_AREA DFLASH_BASE + FLASH_NUTRIFACT_AREA
  #endif
#endif

#define FLASH_CACHE       0
#define NVRAM_CACHE       1
#define INGREDIENT_CACHE  2

extern ROMDATA INT32U PFLASH_SECTOR_ADDR[];
extern INT32U FtlMaxLifeTime;
extern INT32U FtlMinLifeTime;

extern INT16U FlashRdMid(HUGEDATA INT16U *address);
extern INT16U FlashRdDid(HUGEDATA INT16U *address);
extern INT8U FlashChipErase(HUGEDATA INT16U *address);
//extern INT8U FlashSectorErase(HUGEDATA INT16U *address, INT16U sec_num);//DFLASH 29sf040
extern INT8U FlashSectorProtectVerify(HUGEDATA INT16U *address);
extern INT8U FlashWordProgram(HUGEDATA INT16U *address, INT16U data);
extern INT8U FlashByteProgram(HUGEDATA INT8U *address, INT8U data);
extern INT8U FlashDataReprogram(HUGEDATA INT8U *address, HUGEDATA INT8U *data, INT32U byte_count);
extern INT8U FlashDataBlockReprogram(HUGEDATA INT8U *address, HUGEDATA INT8U *data);

extern INT8U FlashDataFill(HUGEDATA INT8U *address, INT8U data, INT32U byte_count);

extern void FlashEraseSuspend(void);
extern void FlashEraseResume(void);

extern void FtlDataBackupFlagProgram(INT32U flag);


//extern INT8U FlashTest(HUGEDATA INT16U *address, INT32U vol);
extern INT8U FlashPgCopy(void);
extern INT8U PFlashSectorEraseTest(void);
extern INT8U DFlashSectorEraseTest(void);
extern void CheckFlashID(void);
extern void CheckPflashErase(void);
extern void CheckDflashErase(void);
extern void CheckflashSector(void);
extern INT16U FlashMemSize(void);
extern INT16U FlashCheckMem(void);
extern INT8U FlashDChipErase(void);
extern INT8U FlashCheckMemCard(HUGEDATA INT16U *base, INT8U size);
extern INT32U FlashMemSizeByte(INT8U mode);

extern ROMDATA INT16U BasicMemTable[7];
extern ROMDATA INT16U ExtCardMemTable[7];


extern void SQI_Init(void);
extern void SQI_Flash_Setup (void);
extern INT32U SQI_Flash_ReadID (INT8U cs);
extern void SQI_Flash_ChipErase (INT8U cs);
extern void SQI_Flash_Read(INT8U cs, INT32U address, INT8U *data, INT32U size);
extern void SQI_Flash_Read4byte(INT8U cs, INT32U address, INT32U *data, INT32U dsize);
extern void SQI_Flash_Write (INT8U cs, INT32U address, INT8U *data, INT32U size);
extern void SQI_Flash_Write2byte(INT8U cs, INT32U address, INT16U *data, INT32U wsize);
extern void SQI_Flash_Write4byte(INT8U cs, INT32U address, INT32U *data, INT32U dwsize);
extern void SFlashByteDataRead(INT8U cs, INT32U address, INT8U *data, INT32U size);
extern void SFlashDWordDataRead(INT8U cs, INT32U address, INT32U *data, INT32U dsize);
extern INT8U FtlBackupEnableFlag;
extern INT8U MemLifetimeState;
extern INT16S InterruptDisableCount;
extern void set_Used_Sector_Erase(INT32U psaErasePtr);
extern void NvramAutoEraseSector(void);
extern INT8U SFlashDataBlockReprogram(INT8U cs, HUGEDATA INT8U *address, HUGEDATA INT8U *data);
extern INT8U SFlash_program_Reprogram(INT8U cs, HUGEDATA INT8U *address, HUGEDATA INT8U *data, INT32U byte_count);
extern void Init_Ftl(void);
extern void FtlDataBackupAllDataProgram(INT8U includeReservedData);
extern void FtlCheckBackupTime(void);

#endif /* _FLASH_H */
