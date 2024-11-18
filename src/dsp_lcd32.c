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

#ifdef _USE_LCD32_

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
#include "dsp_lcd32.h"
#include "prt_cnvtable.h"
#include "main.h"
#include "initial.h"
#include "initial_glb.h"
#define MAX_INPUT_STRING  50

#ifdef USE_CHN_FONT
 INT8U LCD_rot[72];
extern void Bit_RotateMCW(INT8U *src,INT16S src_x,INT16S src_y,INT8U *dst,INT16S *dest_x,INT16S *dest_y);
#endif

#define DSP_BUF_START_ADDR SRAM_BASE + 0x045900   
//#define DSP_BUF_START_ADDR 0x03A400	// memory 빈공간을 사용하기 위해 조정 : CJK080529
__no_init   INT8U DspBuf[DSP_MAX_PAGE][DSP_BUF_SIZE];//  @ (DSP_BUF_START_ADDR);
__no_init   INT8U DspScratchPad[DSP_SCRATCH_PAD_SIZE];// @ (DSP_BUF_START_ADDR + DSP_MAX_PAGE * DSP_BUF_SIZE); 
//__no_init   INT8U DspBuf[DSP_MAX_PAGE][DSP_BUF_SIZE]  @ (DSP_BUF_START_ADDR);// INT8U DspBuf[DSP_MAX_PAGE][DSP_BUF_SIZE];
//__no_init  volatile  INT8U DspDataPort @ (CPLD_ADDR+ 0x08);		//WR
//__no_init  volatile  INT8U DspConPort @ (CPLD_ADDR + 0x04);	 	//WR//volatile  INT8U DspConPort;
 INT8U DspConPortValue;
// INT8U DspScratchPad[DSP_SCRATCH_PAD_SIZE];
volatile  INT8U DspDataPort;
volatile  INT8U DspConPort;

void LCD_Write_data(unsigned short addr, char cbyte); 

extern unsigned short tm1ms_cnt;

/*****************************************************************************************************/
/* Delay Function										     */												
/*****************************************************************************************************/
void SPILCDSendByte(INT16U data)
{
    PLIB_SPI_BufferWrite(SPI_ID_1, data);

	while(!PLIB_SPI_TransmitBufferIsEmpty(SPI_ID_1));
}

INT8U SPILCDReceiveByte(void)
{
    INT16U rxData = 0;
    
    while(!PLIB_SPI_ReceiverBufferIsFull(SPI_ID_1));
    rxData = PLIB_SPI_BufferRead(SPI_ID_1);
    
    PLIB_SPI_BufferClear(SPI_ID_1);
    PLIB_SPI_ReceiverOverflowClear (SPI_ID_1);
    
    return rxData;
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

void Delay(void){
  unsigned long i;
  for(i=0; i<1; i++);
}


void Delay_10(void){ //40 us
  unsigned long i;
 // for(i=0; i<0x2; i++);
  for(i=0; i<0x30; i++);
  }  

void Delay_10us(void)
{
  unsigned long i;
  for(i=0; i<0x75; i++); //test 100119
  //for(i=0; i<0x100; i++);
}  

void Delay_70000(void){
  unsigned long i;
  for(i=0; i<0x70000; i++);
}

void Delay_50_Milli_Seconds(unsigned short time){
  //tm1ms_cnt=time; 
  //while(tm1ms_cnt);

}

void  LCD_RST_HIGH(void)
{
    SPILCDSendByte(0x00BF);
	Delay_10us();
}

void  LCD_RST_LOW(void)
{
    SPILCDSendByte(0x009F);
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
    for (i=0; i < 5; i++) { // Delay > 100 us
	  Delay_10us();Delay_10us();//Delay_10us();Delay_10us(); Delay_10us();
    }
}

void LCD_Write_send(INT8U a, INT8U byte, INT8U cs)
{
    INT16U SPI_DATA = 0;

    SPI_DATA = byte;
    SPI_DATA = (SPI_DATA<<8)| (0xBE|a);
    SPILCDWrite_2Byte(SPI_DATA);

    Delay_10();
	
	SPI_DATA = 0;
	cs =cs|0xB0;
    SPI_DATA = byte;
	SPI_DATA = (SPI_DATA<<8);
    SPI_DATA |=  (cs|a);
    SPILCDWrite_2Byte(SPI_DATA);

	Delay_10();


    SPI_DATA = 0;
    SPI_DATA = byte;
	SPI_DATA = (SPI_DATA<<8) | (0xBE);
    SPILCDWrite_2Byte(SPI_DATA);

	Delay_10();

}

void LCD_Write_data(unsigned short addr, char byte)
{      
	INT8U	x,y;

	// CJK070522
	//x=(INT8U)p.y;
	//y=(INT8U)p.x;
	x = addr % 4;	// 세로좌표
	y = addr / 4;	// 가로좌표

	if(y <= 60) {
		LCD_Write_send(LCD_CMD,0xb8+x,LCD_C1);	
		LCD_Write_send(LCD_CMD,0x00+y,LCD_C1); 
		LCD_Write_send(LCD_DAT,byte,LCD_C1);
	} else if( y > 60 && y<=140) {
		LCD_Write_send(LCD_CMD,0xb8+x,LCD_C2);	
		LCD_Write_send(LCD_CMD,0x00+y-61,LCD_C2); 
		LCD_Write_send(LCD_DAT,byte,LCD_C2);
	} else if( y > 140 && y<=(LCD_Y_MARGIN-1)) {
		LCD_Write_send(LCD_CMD,0xb8+x,LCD_C3);	
		LCD_Write_send(LCD_CMD,0x00+y-141,LCD_C3); 	        
		LCD_Write_send(LCD_DAT,byte,LCD_C3);
	}
} 						

void Dsp_Clear(void)
{
	//LCD_refresh(); 
   short i,j;

   for(i=0;i<4;i++) {
      LCD_Write_send(LCD_CMD,0xb8+i,LCD_C1);
      LCD_Write_send(LCD_CMD,0x00,LCD_C1);
     
      for(j=0;j<61;j++)
        LCD_Write_send(LCD_DAT,0x00,LCD_C1);
   }
   for(i=0;i<4;i++)
   {
      LCD_Write_send(LCD_CMD,0xb8+i,LCD_C2);
      LCD_Write_send(LCD_CMD,0x00,LCD_C2);
     
      for(j=0;j<80;j++)
        LCD_Write_send(LCD_DAT,0x00,LCD_C2);
   }

   for(i=0;i<4;i++)
   {
      LCD_Write_send(LCD_CMD,0xb8+i,LCD_C3);
      LCD_Write_send(LCD_CMD,0x00,LCD_C3);
     
      for(j=0;j<(LCD_Y_MARGIN-141);j++)
        LCD_Write_send(LCD_DAT,0x00,LCD_C3);
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
	    LCD_Write_send(LCD_CMD,0x00,LCD_C123); //SET COLUMN ADDRESS 0
	    LCD_Write_send(LCD_CMD,0xa0,LCD_C123); //ADC Forword
	    LCD_Write_send(LCD_CMD,0xa4,LCD_C123); //STATIC DRIVE OFF
	    LCD_Write_send(LCD_CMD,0xa9,LCD_C123); // 32 DUTY
	    LCD_Write_send(LCD_CMD,0xaf,LCD_C123); //display on
		Dsp_Diffuse();
	}
}

void Dsp_Diffuse(void)
{
    unsigned short i,j;

/////////////////////////////////HYP 20050407 for electro-static-discharge & Burst
    static INT16U DspTimer100ms=0;//HYP 20050411

//sprintf(MsgBuf, "<%d> full Dsp\r\n", DspStruct.Page);
//MsgOut(MsgBuf);

    if(KeyFlagForDispRst == TRUE) {//HYP 20060703 Reset when key is pressed
	DspTimer100ms = SysTimer100ms;
        KeyFlagForDispRst = FALSE;
        LCD_Reset();
        //LCD_Write_send(LCD_CMD,0xb8,LCD_C123); //SET PAGE ADDRESS 0 reset default
        LCD_Write_send(LCD_CMD,0xaf,LCD_C123); //display on
    } else {
//    if(5 < (SysTimer100ms - DspTimer100ms)) {//500 ms Reflash
        if(10 < (INT16U)(SysTimer100ms - DspTimer100ms)) {//1000 ms Reflash
	    DspTimer100ms = SysTimer100ms;
 	    //HYP 20060703
        LCD_Reset();  //test 100119
	    LCD_Write_send(LCD_CMD,0xc0,LCD_C123); //display start line 0
	    LCD_Write_send(LCD_CMD,0xb8,LCD_C123); //SET PAGE ADDRESS 0
	    LCD_Write_send(LCD_CMD,0x00,LCD_C123); //SET COLUMN ADDRESS 0
	    LCD_Write_send(LCD_CMD,0xa0,LCD_C123); //ADC Forword
	    LCD_Write_send(LCD_CMD,0xa4,LCD_C123); //STATIC DRIVE OFF
	    LCD_Write_send(LCD_CMD,0xa9,LCD_C123); // 32 DUTY
	    LCD_Write_send(LCD_CMD,0xaf,LCD_C123); //display on
	}
    }
////////////////////////////////////////////////////////////

   for(i=0;i<4;i++)
   {
      LCD_Write_send(LCD_CMD,0xb8+i,LCD_C1);
      LCD_Write_send(LCD_CMD,0x00,LCD_C1);
     
      for(j=0;j<61;j++) {
        LCD_Write_send(LCD_DAT,DspBuf[DspStruct.Page][i+4*j],LCD_C1);
      }
   }
   for(i=0;i<4;i++)
   {
      LCD_Write_send(LCD_CMD,0xb8+i,LCD_C2);
      LCD_Write_send(LCD_CMD,0x00,LCD_C2);

      for(j=0;j<80;j++) {
        LCD_Write_send(LCD_DAT,DspBuf[DspStruct.Page][i+4*(j+61)],LCD_C2);
      }
   }
   for(i=0;i<4;i++)
   {
      LCD_Write_send(LCD_CMD,0xb8+i,LCD_C3);
      LCD_Write_send(LCD_CMD,0x00,LCD_C3);
     
      for(j=0;j<(LCD_Y_MARGIN-141);j++) {
        LCD_Write_send(LCD_DAT,DspBuf[DspStruct.Page][i+4*(j+61+80)],LCD_C3);
      }
   } 
}

void LCD_Check(unsigned char x, unsigned char y)
{
    unsigned short i;
    unsigned char j;   
             
    for(i=0;i<DSP_BUF_SIZE;i+=64)
    {
        for(j=0;j<32;j+=2)          
        {           
    	    DspBuf[DspStruct.Page][i+j]=x;
            DspBuf[DspStruct.Page][i+j+1]=y;
        }
        for(j=32;j<64;j+=2)          
        {           
            DspBuf[DspStruct.Page][i+j]=y;
            DspBuf[DspStruct.Page][i+j+1]=x;
        }
    }
    Dsp_Diffuse();
}

void LCD_Init(void)
{  		
    INT8U i;

    LCD_Reset();
/////////////////////////////////HYP 20050407 
    //HYP 20050411 Erase (Follow Command execute during Reset Initialization)
    LCD_Write_send(LCD_CMD,0xe2,LCD_C123); //software reset(display start line 0,SET COLUMN ADDRESS 0,SET PAGE ADDRESS 0)
    LCD_Write_send(LCD_CMD,0xa0,LCD_C123); //ADC Forword
    LCD_Write_send(LCD_CMD,0xa4,LCD_C123); //STATIC DRIVE OFF
    LCD_Write_send(LCD_CMD,0xa9,LCD_C123); // 32 DUTY
    LCD_Write_send(LCD_CMD,0xc0,LCD_C123); //display start line 0
    LCD_Write_send(LCD_CMD,0xb8,LCD_C123); //SET PAGE ADDRESS 0
    LCD_Write_send(LCD_CMD,0x00,LCD_C123); //SET COLUMN ADDRESS 0
    LCD_Write_send(LCD_CMD,0xaf,LCD_C123); //display on
////////////////////////////////////////////////////////////
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

void LCD_BufferWrite(POINT p, SIZE m, char type,  unsigned char* ptr)
{
	unsigned short startaddr, curaddr, i, j;
	unsigned char offset, heightB, limit, data;
	char fsize;
	word2byteunion b;

//Check boundry
	if(p.y < 0 || p.y > (LCD_X_MARGIN - m.height)) return;
//	if(p.x < 0 || p.x > (LCD_Y_MARGIN - m.width)) return;
//HYP 20050614
	if(p.x < 0) return;
	if((p.x + m.width)> LCD_Y_MARGIN) {
		//return;
		if (p.x < LCD_Y_MARGIN) {
			m.width = LCD_Y_MARGIN - p.x;
		} else {
			return;
		}
	}

    offset = (p.y%8);
    heightB = (type & 0x0f);
    startaddr = (p.x*4+p.y/8);			//320 --> buffer left margine

    for(i=0; i<m.width; i++)
    {
        fsize = m.height;
        curaddr = startaddr;
       	for(j=heightB-1; j>=0; j--)
        {
            if(fsize>8) limit = 8;
            else if(fsize<=0) break;
            else limit = fsize;

            b.twobyte.l_byte = DspBuf[DspStruct.Page][curaddr];
            b.twobyte.h_byte = DspBuf[DspStruct.Page][curaddr+1];
            b.aword &= (mask[offset]|(heightmask[limit]<<(8-offset)));
            data = ptr[i*heightB+j];
            if (DspStruct.Reverse) data = (~data);
//Insert(delete) curser
		if (DspStruct.InsertCur && i < 2) data ^= 0xff; 
            if (DspStruct.DeleteCur) data = (~data);
///////////////
            b.aword |= ((data&(~heightmask[limit]))<<offset);
            DspBuf[DspStruct.Page][curaddr] = b.twobyte.l_byte;
            DspBuf[DspStruct.Page][curaddr+1] = b.twobyte.h_byte;
	    	if(DspStruct.DirectDraw)
		    {
				LCD_Write_data(curaddr,DspBuf[DspStruct.Page][curaddr]);
		    }
            fsize -= 8;
            curaddr++;
            if(fsize<8)fsize = 0;//test yoo 010107
        }
	//HYP 20040921
//	if(DspStruct.DirectDraw)
        if((DspStruct.DirectDraw) && (offset != 0))
        {
            LCD_Write_data(curaddr,DspBuf[DspStruct.Page][curaddr]);
        }
        startaddr += 4;
    }
}

INT8U Dsp_Write_1Char(POINT p, char c)
{
    SIZE m;
     unsigned char *font_ptr;
    	INT8U *fontw_ptr;
    short font_bytes;

#ifdef USE_CHN_FONT
    INT16S w,fw;
#endif
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
#ifdef USE_VIETNAM_FONT
INT8U Dsp_Write_1Char16(POINT p, INT16U c)
{
    SIZE m;
     unsigned char *font_ptr;
    	INT8U *fontw_ptr;
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
}
#endif

#if defined(USE_ARAB_FONT) || defined(HEBREW_FONT)
INT8U Dsp_Write_1CharLong(POINT p, INT16U c)
{
    SIZE m;
     	INT8U *font_ptr;
    	INT8U *fontw_ptr;
    short font_bytes;

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
     INT8U *font_ptr;

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


void Dsp_Fill_Buffer(unsigned char x)
{
    unsigned short i;

    for(i=0;i<DSP_BUF_SIZE;i++)
    {
		DspBuf[DspStruct.Page][i] = x;
    }
}
#ifdef USE_PLU_SCROLLING
void Dsp_Fill_Buffer_without_TOP_Msg(unsigned char x)
{
    unsigned short i;

    for(i=0;i<DSP_BUF_SIZE;i++) //CL5500-D
    {
    	if(i%4 == 0)continue;// Top msg area
		DspBuf[DspStruct.Page][i] = x;
    }
}
#endif
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
		curaddr = i + sx * 4;
		for (j = sx; j <= ex; j++)
		{
			LCD_Write_data(curaddr, DspBuf[DspStruct.Page][curaddr]);
			curaddr += 4;
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



