/**
********************************************************************************
* Copyright (c) 2018 CAS
* @brief        Control SRAM(23LC1024) via SPI Communication  _  Header File\n
* @file          spi_sram.h
* @version    1.0
* @date        2018/01/05
* @author      Application Develop CL5200 Renewal Team
********************************************************************************
* @remark     [Version History]\n
*          v1.0 2017/02/05 created by ckKim \n
********************************************************************************
*/

#ifndef _SPI_SRAM_H
#define _SPI_SRAM_H

#ifdef USE_SRAM_SPI_COMM

/*
********************************************************************************
*                       GLOBAL DEFINITIONS & MACROS
********************************************************************************
*/
/** @brief Command for SRAM Operation */
#define SRAM_CMD_READ           0x03
#define SRAM_CMD_WRITE          0x02
#define SRAM_CMD_RDMR           0x05
#define SRAM_CMD_WDMR           0x01

/** @brief Mode for SRAM */
#define SRAM_MODE_BYTE          0x00
#define SRAM_MODE_PAGE          0x80
#define SRAM_MODE_SEQUENTIAL    0x40

/** @brief ETC */
#define SRAM_DUMMY_DATA         0xDD

/** @brief SRAM Address definition */
/* SRAM */
#define SRAM_HIGHEST_ADDR       0x1FFFF
#define SRAM_LOWEST_ADDR        0x00

/* Print */
// TphBufC[MAX_FORM_LEN][TPH_ROW_BYTE] -> size: 80mm 기준   640 * 56 = 35,840  (0x08C00)
//                                                                      120mm 기준   960 * 56 = 53,760   (0x0D200)
//                                                                      200mm 기준  1600 * 56 = 89,600   (0x15E00)
#define SRAM_ADDR_PRT_BASE      SRAM_LOWEST_ADDR      //  최대 SRAM 영역(200mm 기준) : 0x01000 ~ 0x16000 (Reserved size: 0x200 , 512)

/* Display */
//	#define SRAM_ADDR_DSP_MEM       0x0100
//	#define SRAM_ADDR_DSP_BUF       0x0400
//	#define SRAM_ADDR_DSP_PAD       0x0700
//	#define SRAM_ADDR_DSP_BACKUP    0x0A00

/*
********************************************************************************
*                       GLOBAL DATA TYPES & STRUCTURES
********************************************************************************
*/
/* Insert global typedef & struct here */




/*
********************************************************************************
*                       GLOBAL(EXPORTED) VARIABLE EXTERNALS
********************************************************************************
*/
/* Insert global variable externals here */
extern INT32U SramTphMemoryA;  // 기존 PrtDrvStruct.TphMemoryA
extern INT32U SramTphMemoryB;  // 기존 PrtDrvStruct.TphMemoryB
extern INT32U SramTphMemory;   // 기존 PrtDrvStruct.TphMemory
extern INT32U SramPrtMemory;   // 기존 PrtStruct.PrtMemory

extern INT32U tempCount_rot0;
extern INT32U tempMaxSizeCount_rot0;
extern INT16U tempMaxH_rot0;
extern INT16U tempMaxMy_rot0;

extern INT32U tempCount_rot0m;
extern INT16U tempMaxSizeCount_rot0m;
extern INT16U tempMaxH_rot0m;
extern INT16U tempMaxMy_rot0m;

extern INT32U tempCount_rot90;
extern INT16U tempMaxSizeCount_rot90;
extern INT16U tempMaxH_rot90;
extern INT16U tempMaxMy_rot90;

extern INT32U tempCount_rot180;
extern INT16U tempMaxSizeCount_rot180;
extern INT16U tempMaxH_rot180;
extern INT16U tempMaxMy_rot180;

extern INT32U tempCount_rot180m;
extern INT16U tempMaxSizeCount_rot180m;
extern INT16U tempMaxH_rot180m;
extern INT16U tempMaxMy_rot180m;

extern INT32U tempCount_rot270;
extern INT16U tempMaxSizeCount_rot270;
extern INT16U tempMaxH_rot270;
extern INT16U tempMaxMy_rot270;

/*
********************************************************************************
*                       GLOBAL(EXPORTED) FUNCTION PROTOTYPES
********************************************************************************
*/
extern void SramSetMode (INT8U mode);
extern INT8U SramGetMode (void);
extern INT8U SramReadData ( INT32U addr, INT8U *rx, INT32U len  );
extern INT8U SramReadData_Byte ( INT32U addr, INT8U *rx );
extern INT8U SramWriteData ( INT32U addr, INT8U *data, INT32U len );
extern INT8U SramWriteData_Byte ( INT32U addr, INT8U data );
extern INT8U SramMemoryClear ( INT32U addr, INT32U len );
extern INT8U SramFillMemory ( INT32U addr, INT8U c, INT32U len );

#endif //USE_SRAM_SPI_COMM
#endif //_SPI_SRAM_H