/*****************************************************************************
|*			
|*  Copyright		:	(c) 2002 CAS
|*  Filename		:	dsp_lcd32.c
|*  Version		:	0.1
|*  Programmer(s)	:	Hong Yi-Phyo (HYP)
|*  Created		:	2003/04/14
|*  Description		:	    
|*				
****************************************************************************/

#include <string.h>
#include <stdio.h>
#include "globals.h"

#ifdef _USE_LCD20848_

#include "commbuf.h"
#include "prt_common.h"
#include "prtdrv.h"
#include "bar_common.h"
#include "prt_font.h"
#include "dsp_common.h"
#include "timer.h"
#include "flash.h"
#include "ram.h"
//#include "cpld.h"
#include "prt_cnvtable.h"
#include "dsp_lcd20848.h"
#include "main.h"
#include "initial.h"
#include "initial_glb.h"
#define MAX_INPUT_STRING  50
#include "mode_main.h"

#ifdef USE_CHN_FONT
HUGEDATA INT8U LCD_rot[72];
extern void Bit_RotateMCW(INT8U *src,INT16S src_x,INT16S src_y,INT8U *dst,INT16S *dest_x,INT16S *dest_y);
#endif

#define DSP_BUF_START_ADDR SRAM_BASE + 0x045900   
//#define DSP_BUF_START_ADDR 0x03A400	// memory 빈공간을 사용하기 위해 조정 : CJK080529
__no_init  HUGEDATA INT8U DspBuf[DSP_MAX_PAGE][DSP_BUF_SIZE_REAL];//  @ (DSP_BUF_START_ADDR);
__no_init  HUGEDATA INT8U DspScratchPad[DSP_SCRATCH_PAD_SIZE];// @ (DSP_BUF_START_ADDR + DSP_MAX_PAGE * DSP_BUF_SIZE); 
//__no_init  HUGEDATA INT8U DspBuf[DSP_MAX_PAGE][DSP_BUF_SIZE]  @ (DSP_BUF_START_ADDR);//HUGEDATA INT8U DspBuf[DSP_MAX_PAGE][DSP_BUF_SIZE];
//__no_init  volatile HUGEDATA INT8U DspDataPort @ (CPLD_ADDR+ 0x08);		//WR
//__no_init  volatile HUGEDATA INT8U DspConPort @ (CPLD_ADDR + 0x04);	 	//WR//volatile HUGEDATA INT8U DspConPort;
HUGEDATA INT8U DspConPortValue;
//HUGEDATA INT8U DspScratchPad[DSP_SCRATCH_PAD_SIZE];
volatile HUGEDATA INT8U DspDataPort;
volatile HUGEDATA INT8U DspConPort;

void LCD_Write_data(unsigned short addr, char cbyte); 
void LCD_BufferWrite(POINT p, SIZE m, char type, HUGEDATA unsigned char* ptr);

//extern ushort tm1ms_cnt;

/*****************************************************************************************************/
/* Delay Function										     */												
/*****************************************************************************************************/
void SPILCDSendByte(INT16U data)
{
    PLIB_SPI_BufferWrite(SPI_ID_1, data);

	while(!PLIB_SPI_TransmitBufferIsEmpty(SPI_ID_1));
}
/*
void SPILCDSend2Byte(INT16U data)
{
    PLIB_SPI_BufferWrite16bit(SPI_ID_1, data);

	while(!PLIB_SPI_TransmitBufferIsEmpty(SPI_ID_1));
}
*/
INT8U SPILCDReceiveByte(void)
{
    INT8U rxData = 0;
    
    while(!PLIB_SPI_ReceiverBufferIsFull(SPI_ID_1));
    rxData = PLIB_SPI_BufferRead(SPI_ID_1);
    
    PLIB_SPI_BufferClear(SPI_ID_1);
    PLIB_SPI_ReceiverOverflowClear (SPI_ID_1);
    
    return rxData;
}

void SPILCDWrite_1Byte(INT8U data)
{	
	//PLIB_PORTS_PinClear( PORTS_ID_0, PORT_CHANNEL_D, PORTS_BIT_POS_2);

	SPILCDSendByte(data);
	SPILCDReceiveByte();

	//PLIB_PORTS_PinSet( PORTS_ID_0, PORT_CHANNEL_D, PORTS_BIT_POS_2);
}

void SPILCDWrite_2Byte(INT16U data)
{
	INT8U temp1, temp2;

	temp1 = (INT8U)(data & 0xff);
	temp2 = (INT8U)((data & 0xff00) >> 8);
	
	PLIB_PORTS_PinClear( PORTS_ID_0, PORT_CHANNEL_D, PORTS_BIT_POS_2);

	SPILCDSendByte(temp2);
	SPILCDReceiveByte();

	SPILCDSendByte(temp1);
	SPILCDReceiveByte();

	PLIB_PORTS_PinSet( PORTS_ID_0, PORT_CHANNEL_D, PORTS_BIT_POS_2);
}

/*
void SPILCDWrite_2Byte(INT16U data)
{
	PLIB_PORTS_PinClear( PORTS_ID_0, PORT_CHANNEL_D, PORTS_BIT_POS_2);

	SPILCDSend2Byte(data);
	SPILCDReceiveByte();

	PLIB_PORTS_PinSet( PORTS_ID_0, PORT_CHANNEL_D, PORTS_BIT_POS_2);
}
*/
void Delay(void){
	INT8U i;
	for(i=0; i<1; i++);
}

void Delay_10(void){ //40 us
	INT32U i;
	for(i=0; i<0x30; i++);
}  

void Delay_10us(void)
{
	INT32U i;
	for(i=0; i<0x75; i++); //test 100119
}

void  LCD_RST_HIGH(void)
{
    //Inline_DIS_Data_Tx(0x0020);
	SPILCDWrite_2Byte(0x0020);
	Delay_10us();
}

void  LCD_RST_LOW(void)
{
    //Inline_DIS_Data_Tx(0x0000);
	SPILCDWrite_2Byte(0x0000);
	Delay_10us();
}

/********************************************************
Reset (Initialization)
a. Display Off
b. Display Start Line : First Line
c. Satatic Drive : Off
d. Column Address Counter : Address 0
e. Page Address Register : Page 0
f. Select Duty : 1/32
g. Aelect ADC : Forword (ADC Command D0 = '0',ADC Status Flag = '1')
h. Read Modify Write : Off
********************************************************/
void LCD_Reset(void) 
{
    INT16U i;
   //LCD_RST_HIGH();
   //LCD_RST_LOW();
   //LCD_RST_HIGH();	
//   DSP_LCD7_diffuse_by_type(0);
//    for (i=0; i < 12000; i++) { // Delay > 100 us
//	  Delay_10us();Delay_10us();//Delay_10us();Delay_10us(); Delay_10us();
//    }
    for (i=0; i < 5; i++) { // Delay > 100 us
	  Delay_10us();Delay_10us();//Delay_10us();Delay_10us(); Delay_10us();
    }
 
}

void LCD_Write_send(char a, char byte, char cs)
{
	INT16U SPI_DATA = 0;

	SPI_DATA = byte;
	//SPI_DATA = (SPI_DATA<<8)| (0xBE|a);
	SPI_DATA = (SPI_DATA<<8)| (a|cs);
	//Inline_Tph_Data_Tx(SPI_DATA);
	//Inline_DIS_Data_Tx(SPI_DATA);
	SPILCDWrite_2Byte(SPI_DATA);
	Delay_10();
	
//	SPI_DATA = 0;
//	cs =cs|0xB0;	
	SPI_DATA = byte;
	SPI_DATA = (SPI_DATA<<8) | (0x40|cs|a);
	// Inline_Tph_Data_Tx(SPI_DATA);
	//Inline_DIS_Data_Tx(SPI_DATA);
	SPILCDWrite_2Byte(SPI_DATA);
	Delay_10();


	//SPI_DATA = 0;
	SPI_DATA = byte;
	SPI_DATA = (SPI_DATA<<8) | (a|cs);
	//  Inline_Tph_Data_Tx(SPI_DATA);	  
	//Inline_DIS_Data_Tx(SPI_DATA);
	SPILCDWrite_2Byte(SPI_DATA);
	Delay_10();
}

void LCD_Write_data(unsigned short addr, char byte)
{      
	INT8U	x,y;

	// CJK070522
	//x=(INT8U)p.y;
	//y=(INT8U)p.x;
	x = addr % 6;	// 세로좌표
	y = addr / 6;	// 가로좌표

	if(y < 64) {
		LCD_Write_send(LCD_CMD,0xb8+x,LCD_C1);	
		LCD_Write_send(LCD_CMD,0x40+y,LCD_C1); 
		LCD_Write_send(LCD_DAT,byte,LCD_C1);
	} else if( y >= 64 && y<128) {
		LCD_Write_send(LCD_CMD,0xb8+x,LCD_C2);	
		LCD_Write_send(LCD_CMD,0x40+y-64,LCD_C2); 
		LCD_Write_send(LCD_DAT,byte,LCD_C2);
	} else if( y >= 128 && y<192) {
		LCD_Write_send(LCD_CMD,0xb8+x,LCD_C3);	
		LCD_Write_send(LCD_CMD,0x40+y-128,LCD_C3); 	        
		LCD_Write_send(LCD_DAT,byte,LCD_C3);
	} else if( y >= 192 && y<LCD_Y_MARGIN_REAL) {
		LCD_Write_send(LCD_CMD,0xb8+x,LCD_C4);	
		LCD_Write_send(LCD_CMD,0x40+y-192,LCD_C4); 	        
		LCD_Write_send(LCD_DAT,byte,LCD_C4);
	}
} 		

void Dsp_Clear(void)
{
	//LCD_refresh(); 
   short i,j;

   for(i=0;i<DSP_MAX_PAGE;i++)
   {
	   for(j=0;j<DSP_BUF_SIZE_REAL;j++)
	   {
			DspBuf[i][j] = 0x00;
	   }
   }
   
   for(i=0;i<6;i++) {
      LCD_Write_send(LCD_CMD,0xb8+i,LCD_C1);
      LCD_Write_send(LCD_CMD,0x40,LCD_C1);
     
      for(j=0;j<64;j++)
        LCD_Write_send(LCD_DAT,0x00,LCD_C1);
   }
   
   for(i=0;i<6;i++)
   {
      LCD_Write_send(LCD_CMD,0xb8+i,LCD_C2);
      LCD_Write_send(LCD_CMD,0x40,LCD_C2);
     
      for(j=64;j<128;j++)
        LCD_Write_send(LCD_DAT,0x00,LCD_C2);
   }

   for(i=0;i<6;i++)
   {
      LCD_Write_send(LCD_CMD,0xb8+i,LCD_C3);
      LCD_Write_send(LCD_CMD,0x40,LCD_C3);
     
      for(j=128;j<192;j++)
        LCD_Write_send(LCD_DAT,0x00,LCD_C3);
   }

   for(i=0;i<6;i++)
   {
      LCD_Write_send(LCD_CMD,0xb8+i,LCD_C4);
      LCD_Write_send(LCD_CMD,0x40,LCD_C4);
     
      for(j=192;j<LCD_Y_MARGIN_REAL;j++)
        LCD_Write_send(LCD_DAT,0x00,LCD_C4);
   }
}


INT8U KeyFlagForDispRst = TRUE;//HYP 20060703

INT16U DspAutoTimer100ms=0;
void Dsp_AUTOP_Diffuse(void)
{
//    unsigned short i,j;
	
    if(5 < (INT16U)(SysTimer100ms - DspAutoTimer100ms)) {//500 ms Reflash
	    DspAutoTimer100ms = SysTimer100ms;
        LCD_Reset();  //test 100119
	    LCD_Write_send(LCD_CMD,0xc0,LCD_C123); //display start line 0
	    LCD_Write_send(LCD_CMD,0xb8,LCD_C123); //SET PAGE ADDRESS 0
	    LCD_Write_send(LCD_CMD,0x40,LCD_C123); //SET COLUMN ADDRESS 0
	    LCD_Write_send(LCD_CMD,0x3f,LCD_C123); //display on
		Dsp_Diffuse();
	}
}

void Dsp_Diffuse(void)
{
    unsigned short i,j;

/////////////////////////////////HYP 20050407 for electro-static-discharge & Burst
    static INT16U DspTimer100ms=0;//HYP 20050411

    if(KeyFlagForDispRst == TRUE) {//HYP 20060703 Reset when key is pressed
	DspTimer100ms = SysTimer100ms;
        KeyFlagForDispRst = FALSE;
        LCD_Reset();
        //LCD_Write_send(LCD_CMD,0xb8,LCD_C123); //SET PAGE ADDRESS 0 reset default
        LCD_Write_send(LCD_CMD,0x3f,LCD_C123); //display on
    } else {
//    if(5 < (SysTimer100ms - DspTimer100ms)) {//500 ms Reflash
        if(10 < (INT16U)(SysTimer100ms - DspTimer100ms)) {//1000 ms Reflash
	    DspTimer100ms = SysTimer100ms;
 	    //HYP 20060703
        LCD_Reset();  //test 100119
		LCD_Write_send(LCD_CMD,0xc0,LCD_C123); //display start line 0
	    LCD_Write_send(LCD_CMD,0xb8,LCD_C123); //SET PAGE ADDRESS 0
	    LCD_Write_send(LCD_CMD,0x40,LCD_C123); //SET COLUMN ADDRESS 0
//	    LCD_Write_send(LCD_CMD,0xa0,LCD_C123); //ADC Forword
//	    LCD_Write_send(LCD_CMD,0xa4,LCD_C123); //STATIC DRIVE OFF
//	    LCD_Write_send(LCD_CMD,0xa9,LCD_C123); // 32 DUTY
	    LCD_Write_send(LCD_CMD,0x3f,LCD_C123); //display on
	}
    }
////////////////////////////////////////////////////////////

   for(i=0;i<6;i++)
   {
      LCD_Write_send(LCD_CMD,0xb8+i,LCD_C1);
      LCD_Write_send(LCD_CMD,0x40,LCD_C1);
     
      for(j=0;j<64;j++) {
        LCD_Write_send(LCD_DAT,DspBuf[DspStruct.Page][i+6*j],LCD_C1);
      }
   }
   for(i=0;i<6;i++)
   {
      LCD_Write_send(LCD_CMD,0xb8+i,LCD_C2);
      LCD_Write_send(LCD_CMD,0x40,LCD_C2);

      for(j=0;j<64;j++) {
        LCD_Write_send(LCD_DAT,DspBuf[DspStruct.Page][i+6*(j+64)],LCD_C2);
      }
   }
   for(i=0;i<6;i++)
   {
      LCD_Write_send(LCD_CMD,0xb8+i,LCD_C3);
      LCD_Write_send(LCD_CMD,0x40,LCD_C3);
     
      for(j=0;j<64;j++) {
        LCD_Write_send(LCD_DAT,DspBuf[DspStruct.Page][i+6*(j+128)],LCD_C3);
      }
   } 
   for(i=0;i<6;i++)
   {
      LCD_Write_send(LCD_CMD,0xb8+i,LCD_C4);
      LCD_Write_send(LCD_CMD,0x40,LCD_C4);
     
      for(j=0;j<(LCD_Y_MARGIN_REAL-192);j++) {
        LCD_Write_send(LCD_DAT,DspBuf[DspStruct.Page][i+6*(j+192)],LCD_C4);
      }
   } 
}

INT8U Chess_DEFAULT[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00}; 
INT8U Chess_0x00[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00}; 
INT8U Chess_0xFF[] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff}; 
void LCD_Check(unsigned char x, unsigned char y)
{
    short i;
    short j;   
	POINT disp_p;
	SIZE m;
//	char c;
	INT8U start, height;
	INT8U *chessPtr;

	if (x == y)
	{
		chessPtr = Chess_DEFAULT;
	}
	else if (x == 0x00)
	{
		chessPtr = Chess_0x00;
	}
	else
	{
		chessPtr = Chess_0xFF;
	}

	if((status_run.run_mode == RUN_SALE)&&(UseSaleModeEditFlag==0)) {
#if defined(USE_ARAB_FONT) || defined(HEBREW_FONT)    //20120914 for CL5200 Arab version by kiho      
		start = 16;
		height = 28;
#else        
		start = 20;
		height = 24;
#endif        
	} else {
		start = 12;
		height = 32;
	}

	m.width  = 6;
	m.height = 8;
		
	for (i = 0; i < 208; i = i + 6)
	{
		for (j = start; j < start + height; j = j + 8)
		{
			disp_p = point(j,i);
			LCD_BufferWrite(disp_p, m, 1, chessPtr);
			if (chessPtr == Chess_0x00) chessPtr = Chess_0xFF;
			else if (chessPtr == Chess_0xFF) chessPtr = Chess_0x00;
		}
		
		if (height == 32)
		{
			if (chessPtr == Chess_0x00) chessPtr = Chess_0xFF;
			else if (chessPtr == Chess_0xFF) chessPtr = Chess_0x00;	
		}
	}
	Dsp_Diffuse();
}

void LCD_Fill_Buffer(INT8U x)
{
    short i;
    short j;   
	POINT disp_p;
	SIZE m;
	INT8U  backup_directdraw;
	INT8U start, height;
	
	backup_directdraw = DspStruct.DirectDraw;
	DspStruct.DirectDraw = 0;	

	if((status_run.run_mode == RUN_SALE)&&(UseSaleModeEditFlag==0)) {
#if defined(USE_ARAB_FONT) || defined(HEBREW_FONT)    //20120914 for CL5200 Arab version by kiho      
		start = 16;
		height = 28;
#else        
		start = 20;
		height = 24;
#endif
	} else {
		start = 12;
		height = 32;
	}

	m.width  = 6;
	m.height = 8;
		
	for (i = 0; i < 208; i = i + 6)
	{
		for (j = start; j < start + height; j = j + 8)
		{
			disp_p = point(j,i);
			LCD_BufferWrite(disp_p, m, 1, Chess_0x00);
		}
	}
	DspStruct.DirectDraw = backup_directdraw;
}

void LCD_Init(void)
{  		
    INT8U i;
    

    //INT8U i;		    
    LCD_Reset();
	LCD_Write_send(LCD_CMD,0xc0,LCD_C123); //display start line 0
	LCD_Write_send(LCD_CMD,0xb8,LCD_C123); //SET PAGE ADDRESS 0
	LCD_Write_send(LCD_CMD,0x40,LCD_C123); //SET COLUMN ADDRESS 0
	LCD_Write_send(LCD_CMD,0x3f,LCD_C123); //display on
	Dsp_Clear();	// 위치 변경 : display on 후 바로 clear함
	Dsp_FontInit();	
	//DspLoadFont1_SG(99, (FONT_FILE_STRUCT *)Dsp_Fixed_20x10);
	//Dsp_Clear();
	
	for (i=0; i<DSP_MAX_PAGE; i++) {
	Dsp_SetPage(i);
	Dsp_Fill_Buffer(0x00);
	}
	Dsp_SetPage(DSP_DEFAULT_PAGE);

   
}

ROMDATA	unsigned short mask[] = {0xff00,0xfe01,0xfc03,0xf807,0xf00f,0xe01f,0xc03f,0x807f};
ROMDATA	unsigned char heightmask[] = {0xff,0xfe,0xfc,0xf8,0xf0,0xe0,0xc0,0x80,0x00};

void LCD_BufferWrite(POINT p, SIZE m, char type, HUGEDATA unsigned char* ptr)
{
	unsigned short startaddr, curaddr, i, j;
//	unsigned char offset, heightB, limit, data;
	unsigned char offset, heightB, data;
	char fsize;
	word2byteunion b;
	unsigned char f_offset;

//Check boundry
	if(p.y < 0 || p.y > (LCD_X_MARGIN_REAL - m.height)) return;
//	if(p.x < 0 || p.x > (LCD_Y_MARGIN_REAL - m.width)) return;
//HYP 20050614
	if(p.x < 0) return;
	if((p.x + m.width)> LCD_Y_MARGIN_REAL) {
		//return;
		if (p.x < LCD_Y_MARGIN_REAL) {
			m.width = LCD_Y_MARGIN_REAL - p.x;
		} else {
			return;
		}
	}

	f_offset = (m.height%8);//18%8=2
    offset = (p.y%8);
    heightB = (type & 0x0f);
    startaddr = (p.x*6+p.y/8);			//320 --> buffer left margine

    for(i=0; i<m.width; i++)
    {
        fsize = m.height;
        curaddr = startaddr;
       	for(j=heightB-1; j>=0; j--)
        {
            //if(fsize>8) limit = 8;
            //else if(fsize<=0) break;
            //else limit = fsize;
			if (fsize<=0) break;
			//limit = 8;
							
            b.twobyte.l_byte = DspBuf[DspStruct.Page][curaddr];
            b.twobyte.h_byte = DspBuf[DspStruct.Page][curaddr+1];
            //b.aword &= (mask[offset]|(heightmask[limit]<<(8-offset)));
			b.aword &= mask[offset];
            
			//data = ptr[i*heightB+j];
			if(fsize > f_offset) 
			{
				if (f_offset == 0)
				{
					data = ptr[i*heightB+j];
				}
				else
				{
					data = (ptr[i*heightB+j]>>(8-f_offset)) | (ptr[i*heightB+j-1]<<f_offset);
				}
				fsize = fsize - 8;
			}
			else
			{
				data = (ptr[i*heightB+j]>>(8-f_offset)) | (DspBuf[DspStruct.Page][curaddr] & heightmask[f_offset]);
				fsize = 0;
			}
			
			if (DspStruct.Reverse) data = (~data);
//Insert(delete) curser
		if (DspStruct.InsertCur && i < 2) data ^= 0xff; 
            if (DspStruct.DeleteCur) data = (~data);
///////////////
            //b.aword |= ((data&(~heightmask[limit]))<<offset);
			b.aword |= data<<offset;
            DspBuf[DspStruct.Page][curaddr] = b.twobyte.l_byte;
            DspBuf[DspStruct.Page][curaddr+1] = b.twobyte.h_byte;
	    	if(DspStruct.DirectDraw)
		    {
				LCD_Write_data(curaddr,DspBuf[DspStruct.Page][curaddr]);
		    }
            //fsize -= 8;
            curaddr++;
            //if(fsize<8)fsize = 0;//test yoo 010107
        }
	//HYP 20040921
//	if(DspStruct.DirectDraw)
	if((DspStruct.DirectDraw) && (offset != 0))
	{
			LCD_Write_data(curaddr,DspBuf[DspStruct.Page][curaddr]);
        }
        startaddr += 6;
    }
}

INT8U Dsp_Write_1Char(POINT p, char c)
{
    SIZE m;
    HUGEDATA unsigned char *font_ptr;
    HUGEDATA	INT8U *fontw_ptr;
    short font_bytes;
	
#ifdef USE_CHN_FONT
    INT16S w,fw;
#endif
	
//	p.x = p.x + 4;
	if((status_run.run_mode == RUN_SALE)&&(UseSaleModeEditFlag==0)) {
#if defined(USE_ARAB_FONT) || defined(HEBREW_FONT)    //20120914 for CL5200 Arab version by kiho
        p.y = p.y + 16;
#else
		p.y = p.y + 20;
#endif
	} else {
		p.y = p.y + 12;
	}
	
//	PutCharTxBuf(&CommBuf,c);//////////////////
    if ((unsigned char)c<DspStruct.FirstCode1) c=DspStruct.FirstCode1;
	
    fontw_ptr = DspStruct.PtrWidth1 + ((unsigned char)c-DspStruct.FirstCode1);
    font_bytes = DspStruct.FHbits1*DspStruct.FWBytes1;
#ifdef USE_CHN_FONT
    if (DspStruct.Id1==3) {
//	font_bytes=16;
       	font_ptr = DspStruct.PtrFont1 + ((INT8U)c - DspStruct.FirstCode1)*font_bytes;
    } else {
	if(DspStruct.PreRot1 == PREROT_ONLY_90DEG) {
        	font_ptr = DspStruct.PtrFont1 + ((INT8U)c - DspStruct.FirstCode1)*font_bytes;
	} else {//PREROT_0DEG_90DEG
		font_ptr = DspStruct.PtrFont1 + (DspStruct.TotalCharNum1 + (INT8U)c - DspStruct.FirstCode1)*font_bytes;
	}
    }
#else
    if(DspStruct.PreRot1 == PREROT_ONLY_90DEG) {
        font_ptr = DspStruct.PtrFont1 + ((INT8U)c - DspStruct.FirstCode1)*font_bytes;
    } else {//PREROT_0DEG_90DEG
        font_ptr = DspStruct.PtrFont1 + (DspStruct.TotalCharNum1 + (INT8U)c - DspStruct.FirstCode1)*font_bytes;
    }
#endif
    m.height = DspStruct.Wbits1;
    if(DspStruct.FixedWidth == 0) 
    {
        m.width = *fontw_ptr;
    }
    else
    {
        m.width = DspStruct.Hbits1;
    }
#ifdef USE_CHN_FONT
    if (DspStruct.Id1>=3) {
	w=DspStruct.Hbits1;
	fw=w>>3;
	if (w&0x07) fw++;
	m.width  = DspStruct.Wbits1;
	m.height = DspStruct.Hbits1;
	LCD_BufferWrite(p,m,fw,font_ptr);
    } else {
	LCD_BufferWrite(p,m,DspStruct.FWBytes1,font_ptr);
    }
#else
    LCD_BufferWrite(p,m,DspStruct.FWBytes1,font_ptr);
#endif
    return m.width;
}

INT8U Dsp_Write_1Char_7seg(POINT p, char c, INT8U height_bits)
{
    SIZE m;
    HUGEDATA unsigned char *font_ptr;
    short font_bytes;
	
    if ((unsigned char)c<DspStruct.FirstCode1) c=DspStruct.FirstCode1;
	
    font_bytes = DspStruct.FHbits1*DspStruct.FWBytes1;

    if(DspStruct.PreRot1 == PREROT_ONLY_90DEG) {
        font_ptr = DspStruct.PtrFont1 + ((INT8U)c - DspStruct.FirstCode1)*font_bytes;
    } else {//PREROT_0DEG_90DEG
        font_ptr = DspStruct.PtrFont1 + (DspStruct.TotalCharNum1 + (INT8U)c - DspStruct.FirstCode1)*font_bytes;
    }

    m.height = DspStruct.Wbits1;
    m.width = height_bits;//DspStruct.Hbits1;

    LCD_BufferWrite(p,m,DspStruct.FWBytes1,font_ptr);

    return m.width;
}

#ifdef USE_VIETNAM_FONT
INT8U Dsp_Write_1Char16(POINT p, INT16U c)
{
    SIZE m;
    HUGEDATA 	INT8U *font_ptr;
    HUGEDATA	INT8U *fontw_ptr;
    short font_bytes;

    if((status_run.run_mode == RUN_SALE)&&(UseSaleModeEditFlag==0)) {
		p.y = p.y + 20;
	} else {
		p.y = p.y + 12;
	}
//	PutCharTxBuf(&CommBuf,c);//////////////////
    fontw_ptr = DspStruct.PtrWidth1 + ((INT16U)c-DspStruct.FirstCode1);
    font_bytes = DspStruct.FHbits1*DspStruct.FWBytes1;
    if(DspStruct.PreRot1 == PREROT_ONLY_90DEG) {
        font_ptr = DspStruct.PtrFont1 + ((INT16U)c - DspStruct.FirstCode1)*font_bytes;
    } else {//PREROT_0DEG_90DEG
        font_ptr = DspStruct.PtrFont1 + (DspStruct.TotalCharNum1 + (INT16U)c - DspStruct.FirstCode1)*font_bytes;
    }

    m.height = DspStruct.Wbits1;
    if(DspStruct.FixedWidth == 0) 
    {
        m.width = *fontw_ptr;
    }
    else
    {
        m.width = DspStruct.Hbits1;			
    }
    LCD_BufferWrite(p,m,DspStruct.FWBytes1,font_ptr);
    return m.width;
}
/*
INT8U Dsp_Write_1Char16(POINT p, INT16U c)
{
    SIZE m;
    HUGEDATA unsigned char *font_ptr;
    HUGEDATA	INT8U *fontw_ptr;
    short font_bytes;

    fontw_ptr = DspStruct.PtrWidth1 + ((INT16U)c-DspStruct.FirstCode1);
    font_bytes = DspStruct.FHbits1*DspStruct.FWBytes1;
    if(DspStruct.PreRot1 == PREROT_ONLY_90DEG) {
        font_ptr = DspStruct.PtrFont1 + ((INT16U)c - DspStruct.FirstCode1)*font_bytes;
    } else {//PREROT_0DEG_90DEG
        font_ptr = DspStruct.PtrFont1 + (DspStruct.TotalCharNum1 + (INT16U)c - DspStruct.FirstCode1)*font_bytes;
    }

    m.height = DspStruct.Wbits1;
    if(DspStruct.FixedWidth == 0) 
    {
        m.width = *fontw_ptr;
    }
    else
    {
        m.width = DspStruct.Hbits1;
    }

    LCD_BufferWrite(p,m,DspStruct.FWBytes1,font_ptr);
    return m.width;
}*/
#endif

#ifdef USE_THAI_FONT
extern INT8U IsThaiUpperVowel(INT8U ch);
extern INT8U IsThaiToneMark(INT8U ch);
extern INT8U ThaiCP874Cnv(INT8U ch);

INT8U Dsp_Write_1Char32(POINT p, INT32U c)
{
    SIZE m;
    INT8U *font_ptr;
    INT8U *fontw_ptr;
    short font_bytes;
    INT16U i;
    INT8U *thai_ptr;
    INT16U tone_mark_font_offset;
    
    thai_ptr = (INT8U *)&c;
    tone_mark_font_offset = 0;
    
    if((status_run.run_mode == RUN_SALE)&&(UseSaleModeEditFlag==0)) {
        p.y = p.y + 20;
    } else {
	p.y = p.y + 12;
    }
//	PutCharTxBuf(&CommBuf,c);//////////////////
    fontw_ptr = DspStruct.PtrWidth1 + ((INT16U)thai_ptr[0] - DspStruct.FirstCode1);
    font_bytes = DspStruct.FHbits1*DspStruct.FWBytes1;
    
    if(DspStruct.PreRot1 == PREROT_ONLY_90DEG) {
        font_ptr = DspStruct.PtrFont1 + ((INT16U)thai_ptr[0] - DspStruct.FirstCode1)*font_bytes;
    } else {//PREROT_0DEG_90DEG
        font_ptr = DspStruct.PtrFont1 + (DspStruct.TotalCharNum1 + (INT16U)thai_ptr[0] - DspStruct.FirstCode1)*font_bytes;
    }
    
    for(i = 0; i < font_bytes; i++) {
        DspScratchPad[i] = font_ptr[i];
    }      
    
    thai_ptr[1] = ThaiCP874Cnv(thai_ptr[1]);
    if (thai_ptr[1] != 0x00)
    {
        if(DspStruct.PreRot1 == PREROT_ONLY_90DEG) {
            font_ptr = DspStruct.PtrFont1 + ((INT16U)thai_ptr[1] - DspStruct.FirstCode1)*font_bytes;
        } else {//PREROT_0DEG_90DEG
            font_ptr = DspStruct.PtrFont1 + (DspStruct.TotalCharNum1 + (INT16U)thai_ptr[1] - DspStruct.FirstCode1)*font_bytes;
        }
        for(i = 0; i < font_bytes; i++) {
            DspScratchPad[i] |= font_ptr[i];
        }      
    }
    
    thai_ptr[2] = ThaiCP874Cnv(thai_ptr[2]);
    if (IsThaiUpperVowel(thai_ptr[1]) && IsThaiToneMark(thai_ptr[2])) tone_mark_font_offset = 0x50;
    
    if (thai_ptr[2] != 0x00)
    {
        if(DspStruct.PreRot1 == PREROT_ONLY_90DEG) {
            font_ptr = DspStruct.PtrFont1 + ((INT16U)thai_ptr[2] - tone_mark_font_offset - DspStruct.FirstCode1)*font_bytes;
        } else {//PREROT_0DEG_90DEG
            font_ptr = DspStruct.PtrFont1 + (DspStruct.TotalCharNum1 + (INT16U)thai_ptr[2] - tone_mark_font_offset - DspStruct.FirstCode1)*font_bytes;
        }
        for(i = 0; i < font_bytes; i++) {
            DspScratchPad[i] |= font_ptr[i];
        }      
    }
    
    tone_mark_font_offset = 0x50;
    
    thai_ptr[3] = ThaiCP874Cnv(thai_ptr[3]);
    if (thai_ptr[3] != 0x00)
    {
        if(DspStruct.PreRot1 == PREROT_ONLY_90DEG) {
            font_ptr = DspStruct.PtrFont1 + ((INT16U)thai_ptr[3] - tone_mark_font_offset - DspStruct.FirstCode1)*font_bytes;//0x50 -> font offset (upper area)
        } else {//PREROT_0DEG_90DEG
            font_ptr = DspStruct.PtrFont1 + (DspStruct.TotalCharNum1 + (INT16U)thai_ptr[3] - tone_mark_font_offset - DspStruct.FirstCode1)*font_bytes;
        }
        for(i = 0; i < font_bytes; i++) {
            DspScratchPad[i] |= font_ptr[i];
        }      
    }
    
    m.height = DspStruct.Wbits1;
    if(DspStruct.FixedWidth == 0) 
    {
        m.width = *fontw_ptr;
    }
    else
    {
        m.width = DspStruct.Hbits1;			
    }
    LCD_BufferWrite(p,m,DspStruct.FWBytes1,DspScratchPad);
    return m.width;
}
#endif

#if defined(USE_ARAB_FONT) || defined(HEBREW_FONT)
INT8U Dsp_Write_1CharLong(POINT p, INT16U c)
{
    SIZE m;
    HUGEDATA 	INT8U *font_ptr;
    HUGEDATA	INT8U *fontw_ptr;
    short font_bytes;

    if((status_run.run_mode == RUN_SALE)&&(UseSaleModeEditFlag==0)) {
#if defined(USE_ARAB_FONT) || defined(HEBREW_FONT)    //20120914 for CL5200 Arab version by kiho
        p.y = p.y + 16;		//Arab or Hebrew 는 12x20 Font 사용해야 함(12x24=> size over!)
#else
		p.y = p.y + 20;
#endif
	} else {
		p.y = p.y + 12;
	}
//	PutCharTxBuf(&CommBuf,c);//////////////////
    fontw_ptr = DspStruct.PtrWidth1 + ((INT16U)c-DspStruct.FirstCode1);
    font_bytes = DspStruct.FHbits1*DspStruct.FWBytes1;
    if(DspStruct.PreRot1 == PREROT_ONLY_90DEG) {
        font_ptr = DspStruct.PtrFont1 + ((INT16U)c - DspStruct.FirstCode1)*font_bytes;
    } else {//PREROT_0DEG_90DEG
        font_ptr = DspStruct.PtrFont1 + (DspStruct.TotalCharNum1 + (INT16U)c - DspStruct.FirstCode1)*font_bytes;
    }

    m.height = DspStruct.Wbits1;
    if(DspStruct.FixedWidth == 0) 
    {
        m.width = *fontw_ptr;
    }
    else
    {
        m.width = DspStruct.Hbits1;			
    }
    LCD_BufferWrite(p,m,DspStruct.FWBytes1,font_ptr);
    return m.width;
}
#endif


INT8U Dsp_Write_2Char(POINT p, INT16U chw)
{
    CWD offset;
#ifdef USE_CHN_FONT
    INT16S   w,h,fw;
#endif
    SIZE m;
    short font_bytes;
    HUGEDATA INT8U *font_ptr;

   //p.x = p.x + 4;
	if((status_run.run_mode == RUN_SALE)&&(UseSaleModeEditFlag==0)) {
#if defined(USE_ARAB_FONT) || defined(HEBREW_FONT)    //20120914 for CL5200 Arab version by kiho
        p.y = p.y + 16;
#else
		p.y = p.y + 20;
#endif
	} else {
		p.y = p.y + 12;
	}
    m.height = DspStruct.Hbits2;
    m.width = DspStruct.Wbits2;

    //Use only Korean 2byte font 
    //if you want to display China font, consider font map
    offset.word = chw - DspStruct.FirstCode2;// relative
#ifdef USE_BIG5_CHN_FONT
	if (offset.byte.left >= 0x26) offset.byte.left -= 0x02; 	//skip for 0xc740 ~ 0xc8ff 
	if (offset.byte.right >= 0x60) offset.byte.right -= 0x20; 	//skip for 0x0080 ~ 0x009f
	offset.word = ((INT16U)offset.byte.left * 0xa0) + offset.byte.right;	//
#else
	offset.word = ((INT16U)offset.byte.left * 0x60) + offset.byte.right;	//
#endif

    //HYP 20040511
    //Dsp_HanJohabPattern(DspScratchPad,HanWan2JohabConvTable[offset.word],DspStruct.PreRot2);
    //m.height = DspStruct.Hbits2;
    //m.width = DspStruct.Wbits2;
    //LCD_BufferWrite(p,m,DspStruct.FWBytes2,DspScratchPad); 
    if (DspStruct.Font2Type == TWO_BYTE_FONT_WAN) {//wansung
	font_bytes = DspStruct.FHbits2 * DspStruct.FWBytes2;
	if(offset.word < DspStruct.TotalCharNum2) {
#ifdef USE_CHN_FONT
	        font_ptr = DspStruct.PtrFont2 + (INT32U)offset.word * font_bytes;
		Bit_RotateMCW((INT8U *)font_ptr,(INT16S)DspStruct.Wbits2,(INT16S)DspStruct.Hbits2,
			      (INT8U *)LCD_rot,(INT16S *)&w,(INT16S *)&h);
		fw=w>>3;
		if (w&0x07) fw++;
		m.width  = w;
		m.height = h;
		LCD_BufferWrite(p,m,fw,LCD_rot);
#else
		if(DspStruct.PreRot2 == PREROT_ONLY_90DEG) {
		        font_ptr = DspStruct.PtrFont2 + (INT32U)offset.word * font_bytes;
		} else {//PREROT_0DEG_90DEG
		        font_ptr = DspStruct.PtrFont2 + (DspStruct.TotalCharNum2 + (INT32U)offset.word) * font_bytes;
		}
		LCD_BufferWrite(p,m,DspStruct.FWBytes2,font_ptr);
#endif
	} else {
#ifndef USE_CHN_FONT
		//HYP 20050304
		//Dsp_HanJohabPattern(DspScratchPad,0xffff,0);//output 'X' pattern(only 16x16, 24x24)
		if(((chw == 0xA1C9) && (Startup.country==COUNTRY_KR))||((chw == 0xA1CC)&&(get_global_bparam(GLOBAL_SALE_SETUP_FLAG4)&0x08)))
		{
			Dsp_HanSpecialPattern(DspScratchPad,chw,DspStruct.PreRot2);
		} else {
			Dsp_HanJohabPattern(DspScratchPad,0xffff,0);//output 'X' pattern(only 16x16, 24x24)
		}
		LCD_BufferWrite(p,m,DspStruct.FWBytes2,DspScratchPad);
#endif
	}
    } else {//johab
#ifndef USE_CHN_FONT
	if((chw > 0xA4A0) && (chw <0xA4BF)) {//cho sung for automata
		Dsp_HanJohabPattern(DspScratchPad,HanWan2JohabConvTable_Chosung[(INT8U)(chw-0xA4A1)],DspStruct.PreRot2);
	} else {
		if(offset.word < (2350+50)) {//HYP 20040513 conv table max value
			Dsp_HanJohabPattern(DspScratchPad,HanWan2JohabConvTable[offset.word],DspStruct.PreRot2);
		} else {
			//HYP 20050304
			//Dsp_HanJohabPattern(DspScratchPad,0xffff,0);//output 'X' pattern(only 16x16, 24x24)
		if(((chw == 0xA1C9) && (Startup.country==COUNTRY_KR))||((chw == 0xA1CC)&&(get_global_bparam(GLOBAL_SALE_SETUP_FLAG4)&0x08))) {
				Dsp_HanSpecialPattern(DspScratchPad,chw,DspStruct.PreRot2);
			} else {
				Dsp_HanJohabPattern(DspScratchPad,0xffff,0);//output 'X' pattern(only 16x16, 24x24)
			}
		}
	}
	LCD_BufferWrite(p,m,DspStruct.FWBytes2,DspScratchPad);
#endif
    }
    return m.width;
}

HUGEDATA unsigned char triangleFont[] = {0x00, 0x20, 0x60, 0xE0, 0x60, 0x20, 0x00, 0x00};
void LCD_BufferWriteTriangle(INT8U nDigitPos, INT8U onoff)
{
	unsigned short curaddr, i;

//	nDigitPos = nDigitPos + 4;
    curaddr = (nDigitPos*6)*8+5;			//320 --> buffer left margine	
		
    for(i=0; i<8; i++)
    {
		if (onoff)
		{
			DspBuf[DspStruct.Page][curaddr] = triangleFont[i] | (DspBuf[DspStruct.Page][curaddr] & 0x0F);
		}
		else
		{
			DspBuf[DspStruct.Page][curaddr] = DspBuf[DspStruct.Page][curaddr] & 0x0F;
		}
#ifdef USE_DISPLAY_OPT_CL5200
		if(DspStruct.DirectDraw)
		{
			LCD_Write_data(curaddr,DspBuf[DspStruct.Page][curaddr]);
		}
#endif
        curaddr += 6;
    }
}


void Dsp_Fill_Buffer(unsigned char x)
{
/*
    unsigned short i;

    for(i=0;i<DSP_BUF_SIZE;i++)
    {
		DspBuf[DspStruct.Page][i] = x;
    }
*/
	LCD_Fill_Buffer(0x00);	
}

void LCD_DrawBackgroundSaleMode(void)
{
	Dsp_Fill_Buffer(0x00);
	Dsp_Diffuse();
}

void LCD_DrawBackgroundProgramMode(void)
{
	Dsp_Fill_Buffer(0x00);
	Dsp_Diffuse();
}

void LCD_DrawBackgroundScrollMsg(void)
{
	Dsp_Fill_Buffer(0x00);
	Dsp_Diffuse();
}


void Dsp_DirectDraw(short sx, short sy, short ex, short ey)
{
}


void Dsp_ReDraw(short sx, short sy, short ex, short ey)
{
	INT16U sy_addr;
	INT16U ey_addr;
	INT16U curaddr;
	INT16U i;
	INT16U j;

	sy_addr = sy / 8;
	ey_addr = ey / 8;

	for (i = sy_addr; i <= ey_addr; i++)
	{
		curaddr = i + sx * 6;
		for (j = sx; j <= ex; j++)
		{
			LCD_Write_data(curaddr, DspBuf[DspStruct.Page][curaddr]);
			curaddr += 6;
		}
	}
}

void Dsp_InitStruct(void)	// use for lcd320240 
{
}
 
void Dsp_PrtStructBackup(INT8U backup)	// use for lcd320240
{
}

#endif



