/*****************************************************************************
|*			
|*  Copyright		:	(c) 2002 CAS
|*  Filename		:	dsp_vfd7.h
|*  Version			:	0.1
|*  Programmer(s)	:	Kim young il (YIK0101)
|*  Created			:	2003/04/17
|*  Description		:	CLP BASIC     
|*				
****************************************************************************/

#ifndef _DSP_VFD7_H
#define _DSP_VFD7_H


#ifdef _USE_LCD_7_37D_40DOT_
#define DSP_MAX_DIGIT      25
#elif defined(_USE_LCD_7_26d_16x96_)
#define DSP_MAX_DIGIT      26
#else
#define DSP_MAX_DIGIT      26
#endif
#define DSP_MAX_CHAR       12
#define DSP_MAX_SYMBOL       28

#define VFD_SEG1              0x01
#define VFD_SEG2              0x02
#define VFD_SEG3              0x04
#define VFD_SEG4              0x08
#define VFD_SEG5              0x10
#define VFD_SEG6              0x20
#define VFD_SEG7              0x40
#define VFD_SEG8              0x80

#define VFD_RESTORE     0
#define VFD_BACKUP		1	

/*
#define STB1_ON               0xf7
#define STB1_OFF              0x08

#define STB2_ON               0xef
#define STB2_OFF              0x10

#define STBA_ON               0xe7
#define STBA_OFF              0xff
*/
//only use 3,4 bits , other bits must be 0.
#define STB1_ON               0x10
#define STB1_OFF              0x08

#define STB2_ON               0x08
#define STB2_OFF              0x10

#define STBA_ON               0x00
#define STBA_OFF              0x18

#define VFD_DRIVER1           0x08
#define VFD_DRIVER2           0x10
#define VFD_DRIVERA           0x18

#define VFD_WRITE_CMD         0x40
#define VFD_ADDR_SET_CMD      0xC0


#define CURSOR_CHAR	      0x01

#define VFD_CMD_PULSE1_16     0x80
#define VFD_CMD_PULSE2_16     0x81
#define VFD_CMD_PULSE4_16     0x82
#define VFD_CMD_PULSE10_16    0x83
#define VFD_CMD_PULSE11_16    0x84
#define VFD_CMD_PULSE12_16    0x85
#define VFD_CMD_PULSE13_16    0x86
#define VFD_CMD_PULSE14_16    0x87

#define VFD_CMD_DISPLAYON     0x08
#define VFD_CMD_DISPLAYOFF   (~VFD_CMD_DISPLAYON)

#define DISPMODE_08D20S        0x00
#define DISPMODE_09D19S        0x08
#define DISPMODE_10D18S        0x09
#define DISPMODE_11D17S        0x0A
#define DISPMODE_12D16S        0x0B
#define DISPMODE_13D15S        0x0C
#define DISPMODE_14D14S        0x0D
#define DISPMODE_15D13S        0x0E
#define DISPMODE_16D12S        0x0F

#define SEG_0     0x3F    
#define SEG_1     0x06    
#define SEG_2     0x5B   
#define SEG_3     0x4F    
#define SEG_4     0x66   
#define SEG_5     0x6D   
#define SEG_6     0x7D   
#define SEG_7     0x27   
#define SEG_8     0x7F    
#define SEG_9     0x6F

#if !defined(_USE_LCD20848_) && !defined(_USE_LCD32_)
extern INT8U VFD_7_Memory[DSP_MAX_DIGIT*2];
#endif
extern void VFD7_display_bright(INT8U num);
extern void VFD7_write_byte(char Data);
extern void VFD7_write_cmd(char Data, INT8U stb);
extern void VFD7_display_On(void);
extern void VFD7_display_Off(void);
extern void VFD7_Init(void);
extern void VFD7_Reset(INT8U flag);
extern void VFD7_display_ucomma(INT8U nDigitPos, INT8U onoff);
extern void VFD7_display_char(INT8U nDigitPos, INT8U cData);
extern void VFD7_display_string(HUGEDATA char *dataString, INT8U nDigitPos, char size, INT8U mode, BOOLEAN reverse);
extern void VFD7_Diffuse(void);
extern void VFD7_Fill_Buffer(unsigned char x);
#if defined(_USE_LCD_7_37D_40DOT_) || defined(_USE_LCD_7_26d_16x96_)
extern void VFD7_Fill_Buffer_CHAR(unsigned char x);
#endif

extern void VFD7_display_triangle(INT8U nDightPos, INT8U onoff);
#if defined(_USE_LCD_7_37D_40DOT_) || defined(_USE_LCD_7_26d_16x96_)
extern void VFD7_display_triangle_all(INT8U onoff);
#endif
extern void VFD7_save_restore(INT8U x);
#endif