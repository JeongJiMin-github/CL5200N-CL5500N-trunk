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
INT32U SramTphMemoryA;  // ���� PrtDrvStruct.TphMemoryA
INT32U SramTphMemoryB;  // ���� PrtDrvStruct.TphMemoryB
INT32U SramTphMemory;   // ���� PrtDrvStruct.TphMemory
INT32U SramPrtMemory;   // ���� PrtStruct.PrtMemory

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
* @brief       SRAM(23LC1024)�� Mode Register�� ���� �Լ�
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
* @brief       SRAM(23LC1024)�� Mode Register�� �д� �Լ�
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
* @brief       SRAM(23LC1024)�� Ư�� �������� ���� ���ϴ� ���̸�ŭ Data�� �д� �Լ�
* @param     addr: Data ���۹���, rx: �޾ƿ� Data�� ������ �ּ�, len: �޾ƿ� Data ����
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

    for( i=0; i<len; i++ )  // �޾ƿ� Data�� �縸ŭ Dummy Data�� ���� SPI CLK�� ����
    {
        sramSendByte( SRAM_DUMMY_DATA );
        rx[i] = sramReceiveByte();
    }

    PLIB_PORTS_PinSet( PORTS_ID_0, PORT_CHANNEL_F, PORTS_BIT_POS_12 );

    return 0;
}

/**
********************************************************************************
* @brief       SRAM(23LC1024)�� Ư�� �������� ���� ���ϴ� 1Byte ��ŭ Data�� �д� �Լ�
* @param     addr: Data ���۹���, rx: �޾ƿ� Data�� ������ �ּ�
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
* @brief       SRAM(23LC1024)�� Ư�� �������� ���� ���ϴ� ���̸�ŭ Data�� ���� �Լ�
* @param     addr: Data ���۹���, data: ������ Data�� ���� �ּ�, len: ������ Data ����
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
* @brief       SRAM(23LC1024)�� Ư�� �������� ���� ���ϴ� 1Byte ���̸�ŭ Data�� ���� �Լ�
* @param     addr: Data ���۹���, data: ������ Data
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
* @brief       SRAM(23LC1024)�� Ư�� �������� ���� ���ϴ� ���̸�ŭ Data�� 0���� �ʱ�ȭ �ϴ� �Լ�
* @param     addr: Data ���۹���, len: ������ Data ����
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
* @brief       SRAM(23LC1024)�� Ư�� �������� ���� ���ϴ� ���̸�ŭ Data�� Ư�� ���ڷ����� �ʱ�ȭ �ϴ� �Լ�
* @param     addr: Data ���۹���, len: ������ Data ����, c: ���ϴ� Ư������
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
* @brief       SRAM(23LC1024)���� Ư��1 Byte�� Transmit �ϴ� �Լ�
* @param     data: ���� �� 1 Byte�� Data
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
* @brief       SRAM(23LC1024)���κ��� 1 Byte�� Receive �ϴ� �Լ�
* @param     none
* @return      rxData: ���� ���� 1 Byte�� Data
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
