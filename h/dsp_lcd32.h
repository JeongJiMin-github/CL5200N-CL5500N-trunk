/*****************************************************************************
|*			
|*  Copyright		:	(c) 2002 CAS
|*  Filename		:	lcd.h
|*  Version			:	0.1
|*  Programmer(s)	:	Hong Yi-Phyo (HYP)
|*  Created			:	2003/04/14
|*  Description		:	CLP BASIC LCD global variable define    
|*				
****************************************************************************/

#ifdef _USE_LCD32_

#ifndef _DSP_LCD32_H
#define _DSP_LCD32_H


typedef union
{
    unsigned short aword;
    struct sprt
    {
        unsigned char l_byte;
	unsigned char h_byte;
    }twobyte;
}word2byteunion;

#define LCD_C1         0x8C
#define LCD_C2 	       0x8A
#define LCD_C3 		0x86
#define LCD_C123	0x80

#define LCD_DAT	0x01
#define LCD_CMD	0x00


#define LCD_LED 0x10
#define LCD_RST 0x80

extern void LCD_Init(void);
extern void LCD_refresh(void);
extern void LCD_Check(unsigned char x, unsigned char y);
#if defined(USE_ARAB_FONT) || defined(HEBREW_FONT)
extern INT8U Dsp_Write_1CharLong(POINT p, INT16U c);
#endif
extern INT8U Dsp_Write_1Char(POINT,char);
#ifdef USE_VIETNAM_FONT
extern INT8U Dsp_Write_1Char16(POINT,INT16U);
#endif
#ifdef USE_THAI_FONT
extern INT8U Dsp_Write_1Char32(POINT,INT32U);
#endif
extern INT8U Dsp_Write_2Char(POINT p, INT16U chw);

#endif	//_DSP_LCD32_H
#endif	//_USE_LCD32_
