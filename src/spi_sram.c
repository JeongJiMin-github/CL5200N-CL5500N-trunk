/**
********************************************************************************
* Copyright (c) 2018 CAS
* @brief        Control SRAM(23LC1024) via SPI Communication   _  Source File\n
* @file          spi_sram.c
* @version    1.0
* @date        2018/01/05
* @author      Application Develop CL5200 Renewal Team
********************************************************************************
* @remark     [Version History]\n
*          v1.0 2017/02/05 created by ckKim \n
********************************************************************************
*/

/*
********************************************************************************
*                       INCLUDES
********************************************************************************
*/ 
#include <stdio.h>
#include "globals.h"
#include "peripheral/peripheral.h"
#include "spi_sram.h"

#ifdef USE_SRAM_SPI_COMM

/*
********************************************************************************
*                       GLOBAL(EXPORTED) VARIABLES & TABLES
********************************************************************************
*/
/* Insert global variables & tables here */

/*
********************************************************************************
*                       LOCAL DEFINITIONS & MACROS
********************************************************************************
*/
/* Insert #define here */

/*
********************************************************************************
*                       LOCAL DATA TYPES & STRUCTURES
********************************************************************************
*/
/* Insert local typedef & structure here */


/*
********************************************************************************
*                       GLOBAL(FILE SCOPE) VARIABLES & TABLES
********************************************************************************
*/
/* Insert file scope variable & tables here */
INT32U SramTphMemoryA;  // 기존 PrtDrvStruct.TphMemoryA
INT32U SramTphMemoryB;  // 기존 PrtDrvStruct.TphMemoryB
INT32U SramTphMemory;   // 기존 PrtDrvStruct.TphMemory
INT32U SramPrtMemory;   // 기존 PrtStruct.PrtMemory

INT32U tempCount_rot0;
INT32U tempMaxSizeCount_rot0;
INT16U tempMaxH_rot0;
INT16U tempMaxMy_rot0;

INT32U tempCount_rot0m;
INT16U tempMaxSizeCount_rot0m;
INT16U tempMaxH_rot0m;
INT16U tempMaxMy_rot0m;

INT32U tempCount_rot90;
INT16U tempMaxSizeCount_rot90;
INT16U tempMaxH_rot90;
INT16U tempMaxMy_rot90;

INT32U tempCount_rot180;
INT16U tempMaxSizeCount_rot180;
INT16U tempMaxH_rot180;
INT16U tempMaxMy_rot180;

INT32U tempCount_rot180m;
INT16U tempMaxSizeCount_rot180m;
INT16U tempMaxH_rot180m;
INT16U tempMaxMy_rot180m;

INT32U tempCount_rot270;
INT16U tempMaxSizeCount_rot270;
INT16U tempMaxH_rot270;
INT16U tempMaxMy_rot270;









/*
********************************************************************************
*                       LOCAL FUNCTION PROTOTYPES
********************************************************************************
*/ 
static void sramSendByte ( INT8U data );
static INT8U sramReceiveByte ( void );



/*
********************************************************************************
*                       GLOBAL(EXPORTED) FUNCTIONS
********************************************************************************
*/ 
/**
********************************************************************************
* @brief       SRAM(23LC1024)의 Mode Register를 쓰는 함수
* @param     mode: 0x40(SequentialMode_default)  0x00(ByteMode)  0x80(PageMode)
* @return     none
* @remark    .
********************************************************************************
*/
void SramSetMode (INT8U mode)
{
    PLIB_PORTS_PinClear( PORTS_ID_0, PORT_CHANNEL_F, PORTS_BIT_POS_12 );
    
    sramSendByte( SRAM_CMD_WDMR );
    sramReceiveByte();
    
    sramSendByte( mode );
    sramReceiveByte();

    PLIB_PORTS_PinSet( PORTS_ID_0, PORT_CHANNEL_F, PORTS_BIT_POS_12 );
}

/**
********************************************************************************
* @brief       SRAM(23LC1024)의 Mode Register를 읽는 함수
* @param     none
* @return     mode: 0x40(SequentialMode_default)  0x00(ByteMode)  0x80(PageMode)
* @remark    .
********************************************************************************
*/
INT8U SramGetMode ( void )
{
    INT8U mode = 0;
    INT8U dummyData = 0xDD;

    PLIB_PORTS_PinClear( PORTS_ID_0, PORT_CHANNEL_F, PORTS_BIT_POS_12 );

    sramSendByte( SRAM_CMD_RDMR );
    sramReceiveByte();
    
    sramSendByte( dummyData );
    mode = sramReceiveByte();

    PLIB_PORTS_PinSet( PORTS_ID_0, PORT_CHANNEL_F, PORTS_BIT_POS_12 );
    
    return mode;
}

/**
********************************************************************************
* @brief       SRAM(23LC1024)의 특정 번지수로 부터 원하는 길이만큼 Data를 읽는 함수
* @param     addr: Data 시작번지, rx: 받아온 Data를 저장할 주소, len: 받아올 Data 길이
* @return     0: Success, 1: Fail
* @remark    .
********************************************************************************
*/
INT8U SramReadData ( INT32U addr, INT8U *rx, INT32U len  )
{
    INT8U tempAddr1, tempAddr2, tempAddr3;
    INT32U i = 0;

    if( addr + len >= SRAM_HIGHEST_ADDR ) return 1;
    
    tempAddr1 = (INT8U) (addr & 0xff);
    tempAddr2 = (INT8U) ( (addr & 0xff00) >> 8 );
    tempAddr3 = (INT8U) ( (addr & 0xff0000) >> 16 );

    PLIB_PORTS_PinClear( PORTS_ID_0, PORT_CHANNEL_F, PORTS_BIT_POS_12 );

    sramSendByte( SRAM_CMD_READ );
    sramReceiveByte();

    sramSendByte( tempAddr3 );
    sramReceiveByte();

    sramSendByte( tempAddr2 );
    sramReceiveByte();

    sramSendByte( tempAddr1 );
    sramReceiveByte();

    for( i=0; i<len; i++ )  // 받아올 Data의 양만큼 Dummy Data를 보내 SPI CLK를 유지
    {
        sramSendByte( SRAM_DUMMY_DATA );
        rx[i] = sramReceiveByte();
    }

    PLIB_PORTS_PinSet( PORTS_ID_0, PORT_CHANNEL_F, PORTS_BIT_POS_12 );

    return 0;
}

/**
********************************************************************************
* @brief       SRAM(23LC1024)의 특정 번지수로 부터 원하는 1Byte 만큼 Data를 읽는 함수
* @param     addr: Data 시작번지, rx: 받아온 Data를 저장할 주소
* @return     0: Success, 1: Fail
* @remark    .
********************************************************************************
*/
INT8U SramReadData_Byte ( INT32U addr, INT8U *rx )
{
    INT8U tempAddr1, tempAddr2, tempAddr3;

    if( addr >= SRAM_HIGHEST_ADDR ) return 1;
    
    tempAddr1 = (INT8U) (addr & 0xff);
    tempAddr2 = (INT8U) ( (addr & 0xff00) >> 8 );
    tempAddr3 = (INT8U) ( (addr & 0xff0000) >> 16 );

    PLIB_PORTS_PinClear( PORTS_ID_0, PORT_CHANNEL_F, PORTS_BIT_POS_12 );

    sramSendByte( SRAM_CMD_READ );
    sramReceiveByte();

    sramSendByte( tempAddr3 );
    sramReceiveByte();

    sramSendByte( tempAddr2 );
    sramReceiveByte();

    sramSendByte( tempAddr1 );
    sramReceiveByte();

    sramSendByte( SRAM_DUMMY_DATA );
    *rx = sramReceiveByte();

    PLIB_PORTS_PinSet( PORTS_ID_0, PORT_CHANNEL_F, PORTS_BIT_POS_12 );

    return 0;
}

/**
********************************************************************************
* @brief       SRAM(23LC1024)의 특정 번지수로 부터 원하는 길이만큼 Data를 쓰는 함수
* @param     addr: Data 시작번지, data: 저장할 Data의 시작 주소, len: 저장할 Data 길이
* @return     0: Success, 1: Fail
* @remark    .
********************************************************************************
*/
INT8U SramWriteData ( INT32U addr, INT8U *data, INT32U len )
{
    INT8U tempAddr1, tempAddr2, tempAddr3;
    INT32U i = 0;

    if( addr + len >= SRAM_HIGHEST_ADDR ) return 1;
    
    tempAddr1 = (INT8U) (addr & 0xff);
    tempAddr2 = (INT8U) ( (addr & 0xff00) >> 8 );
    tempAddr3 = (INT8U) ( (addr & 0xff0000) >> 16 );

    PLIB_PORTS_PinClear( PORTS_ID_0, PORT_CHANNEL_F, PORTS_BIT_POS_12 );

    sramSendByte( SRAM_CMD_WRITE );
    sramReceiveByte();

    sramSendByte( tempAddr3 );
    sramReceiveByte();

    sramSendByte( tempAddr2 );
    sramReceiveByte();

    sramSendByte( tempAddr1 );
    sramReceiveByte();

    for( i=0; i<len; i++)
    {
        sramSendByte( *(data + i) );
        sramReceiveByte();
    }

    PLIB_PORTS_PinSet( PORTS_ID_0, PORT_CHANNEL_F, PORTS_BIT_POS_12 );

    return 0;
}

/**
********************************************************************************
* @brief       SRAM(23LC1024)의 특정 번지수로 부터 원하는 1Byte 길이만큼 Data를 쓰는 함수
* @param     addr: Data 시작번지, data: 저장할 Data
* @return     0: Success, 1: Fail
* @remark    .
********************************************************************************
*/
INT8U SramWriteData_Byte ( INT32U addr, INT8U data )
{
    INT8U tempAddr1, tempAddr2, tempAddr3;

    if( addr >= SRAM_HIGHEST_ADDR ) return 1;
    
    tempAddr1 = (INT8U) (addr & 0xff);
    tempAddr2 = (INT8U) ( (addr & 0xff00) >> 8 );
    tempAddr3 = (INT8U) ( (addr & 0xff0000) >> 16 );

    PLIB_PORTS_PinClear( PORTS_ID_0, PORT_CHANNEL_F, PORTS_BIT_POS_12 );

    sramSendByte( SRAM_CMD_WRITE );
    sramReceiveByte();

    sramSendByte( tempAddr3 );
    sramReceiveByte();

    sramSendByte( tempAddr2 );
    sramReceiveByte();

    sramSendByte( tempAddr1 );
    sramReceiveByte();

    sramSendByte( data );
    sramReceiveByte();

    PLIB_PORTS_PinSet( PORTS_ID_0, PORT_CHANNEL_F, PORTS_BIT_POS_12 );

    return 0;
}


/**
********************************************************************************
* @brief       SRAM(23LC1024)의 특정 번지수로 부터 원하는 길이만큼 Data를 0으로 초기화 하는 함수
* @param     addr: Data 시작번지, len: 저장할 Data 길이
* @return     0: Success, 1: Fail
* @remark    .
********************************************************************************
*/
INT8U SramMemoryClear ( INT32U addr, INT32U len )
{
    INT8U tempAddr1, tempAddr2, tempAddr3;
    INT32U i = 0;

    if( addr + len >= SRAM_HIGHEST_ADDR ) return 1;
    
    tempAddr1 = (INT8U) (addr & 0xff);
    tempAddr2 = (INT8U) ( (addr & 0xff00) >> 8 );
    tempAddr3 = (INT8U) ( (addr & 0xff0000) >> 16 );

    PLIB_PORTS_PinClear( PORTS_ID_0, PORT_CHANNEL_F, PORTS_BIT_POS_12 );

    sramSendByte( SRAM_CMD_WRITE );
    sramReceiveByte();

    sramSendByte( tempAddr3 );
    sramReceiveByte();

    sramSendByte( tempAddr2 );
    sramReceiveByte();

    sramSendByte( tempAddr1 );
    sramReceiveByte();

    for( i=0; i<len; i++ )
    {
        sramSendByte( 0x00 );
        sramReceiveByte();
    }

    PLIB_PORTS_PinSet( PORTS_ID_0, PORT_CHANNEL_F, PORTS_BIT_POS_12 );

    return 0;
}




/**
********************************************************************************
* @brief       SRAM(23LC1024)의 특정 번지수로 부터 원하는 길이만큼 Data를 특정 문자로으로 초기화 하는 함수
* @param     addr: Data 시작번지, len: 저장할 Data 길이, c: 원하는 특정문자
* @return     0: Success, 1: Fail
* @remark    .
********************************************************************************
*/
INT8U SramFillMemory ( INT32U addr, INT8U c, INT32U len )
{
    INT8U tempAddr1, tempAddr2, tempAddr3;
    INT32U i = 0;

    if( addr + len >= SRAM_HIGHEST_ADDR ) return 1;
    
    tempAddr1 = (INT8U) (addr & 0xff);
    tempAddr2 = (INT8U) ( (addr & 0xff00) >> 8 );
    tempAddr3 = (INT8U) ( (addr & 0xff0000) >> 16 );

    PLIB_PORTS_PinClear( PORTS_ID_0, PORT_CHANNEL_F, PORTS_BIT_POS_12 );

    sramSendByte( SRAM_CMD_WRITE );
    sramReceiveByte();

    sramSendByte( tempAddr3 );
    sramReceiveByte();

    sramSendByte( tempAddr2 );
    sramReceiveByte();

    sramSendByte( tempAddr1 );
    sramReceiveByte();

    for( i=0; i<len; i++ )
    {
        sramSendByte( c );
        sramReceiveByte();
    }

    PLIB_PORTS_PinSet( PORTS_ID_0, PORT_CHANNEL_F, PORTS_BIT_POS_12 );

    return 0;
}



/*
********************************************************************************
*                       LOCAL FUNCTIONS
********************************************************************************
*/ 
/**
********************************************************************************
* @brief       SRAM(23LC1024)으로 특정1 Byte를 Transmit 하는 함수
* @param     data: 전송 할 1 Byte의 Data
* @return      none
* @remark    .
********************************************************************************
*/
void sramSendByte( INT8U data )
{
    PLIB_SPI_BufferWrite( SPI_ID_5, data );
    while( !PLIB_SPI_TransmitBufferIsEmpty(SPI_ID_5) );
}

/**
********************************************************************************
* @brief       SRAM(23LC1024)으로부터 1 Byte를 Receive 하는 함수
* @param     none
* @return      rxData: 전송 받은 1 Byte의 Data
* @remark    .
********************************************************************************
*/
INT8U sramReceiveByte( void )
{
    INT8U rxData = 0;
    
    while( !PLIB_SPI_ReceiverBufferIsFull(SPI_ID_5) );
    rxData = PLIB_SPI_BufferRead( SPI_ID_5 );
    
    PLIB_SPI_BufferClear( SPI_ID_5 );
    PLIB_SPI_ReceiverOverflowClear ( SPI_ID_5 );
    
    return rxData;
}


#endif // USE_SRAM_SPI_COMM
