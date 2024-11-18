/**
 * @file dsp_lcd7_37d_40dot.c
 * @attention (c) 2008 CAS
 * @version 0.1
 * @author 윤여철
 * @date 2015/02/10
 * @brief LCD CL5200J Char LCD Version을 구동시키는 드라이버임.
 */

#include <stdio.h>
#include "globals.h"
#include "dsp_app.h"
#include "dsp_vfd7.h"
#include "prt_font.h"
#include "dsp_common.h"
#include "dsp_lcd37d_40dot.h"

#ifdef _USE_LCD_7_37D_40DOT_

#define MINUS_SEG             0x10
#define SPACE_SEG             0x02
#define DOT_SEG               0x01
#define COMMA_SEG             0x01
#define TRIANGLE_SEG          0x40
#define SPA_SEG               0x00
#define ALL_SEG               0x7f
#define CURSOR_SEG			  0x02
#define OTHER_SEG             0x70
#define SLASH_SEG             0x38
//#define CURSOR_CHAR			  0x02


#define DIFFUSE_ALL		0
#define DIFFUSE_7SEG	1
#define DIFFUSE_12CHAR	2

#define DOT_TYPE_COMMA	0x80

#define LCD_COMMA_START_ADDR    48
#define LCD_DIGIT_OFFSET         2
#define LCD_TARE_OFFSET          2
#define LCD_WEIGHT_OFFSET        3
#define LCD_COMMA_OFFSET         3
#define LCD_CHAR_OFFSET         (64+2) //CS2의 SEG2~SEG31, SEG33~SEG62 사용
#define LCD_TARE_END_PTR         5
#define LCD_WEIGHT_START_ADDR    8
#define LCD_SIGN_MAX            35 //S1~S35

#define LCD_CHAR_AREA1_SIZE    (DSP_MAX_CHAR / 2 * LCD_CHAR_FONT_WIDTH)
#define LCD_CHAR_AREA1_OFFSET  LCD_CHAR_OFFSET
#define LCD_CHAR_AREA2_SIZE    (DSP_MAX_CHAR / 2 * LCD_CHAR_FONT_WIDTH)
#define LCD_CHAR_AREA2_OFFSET  (LCD_CHAR_OFFSET + LCD_CHAR_AREA1_SIZE + 1) // +1은 불연속 구간


#define LCD7_37D_40DOT_CS1_START_ADDR    0
#define LCD7_37D_40DOT_CS1_END_ADDR     64
#define LCD7_37D_40DOT_CS2_START_ADDR   64
#define LCD7_37D_40DOT_CS2_END_ADDR    128

/** @brief Turn off both system oscillator and Lcd bias generator*/
#define HT162X_SYS_DIS	0x00
/** @brief Turn on system oscillator*/
#define HT162X_SYS_EN	0x01
/** @brief Turn off Lcd display*/
#define HT162X_OFF		0x02
/** @brief Turn on Lcd display*/
#define HT162X_ON		0x03
/** @brief Disable time base output*/
#define HT162X_TIMER_DIS	0x04
/** @brief Disable WDT time-out flag output*/
#define HT162X_WDT_DIS	0x05
/** @brief Enable time base output*/
#define HT162X_TIMER_EN	0x06
/** @brief Enable WDT time-out flag output*/
#define HT162X_WDT_EN	0x07
/** @brief Turn off tone outputs*/
#define HT162X_TONE_OFF	0x08
/** @brief Clear the contents of the time base generator*/
#define HT162X_CLR_TIMER	0x0d
/** @brief Clear the contents of WDT stage*/
#define HT162X_CLR_WDT	0x0f
/** @brief System clock source, on-chip RC oscillator*/
#define HT162X_RC_32K	0x18
/** @brief System clock source, external clock source*/
#define HT162X_EXT_32K	0x1c
/** @brief Tone frequency output : 4kHz*/
#define HT162X_TONE_4K	0x40
/** @brief Tone frequency output : 2kHz*/
#define HT162X_TONE_2K	0x60
/** @brief Disable IRQ output*/
#define HT162X_IRQ_DIS	0x80
/** @brief Enable IRQ output*/
#define HT162X_IRQ_EN	0x88
/** @brief Time base clock output : 1Hz.   The WDT time-out flag after : 4s*/
#define HT162X_F1		0xa0
/** @brief Time base clock output : 2Hz.   The WDT time-out flag after : 2s*/
#define HT162X_F2		0xa1
/** @brief Time base clock output : 4Hz.   The WDT time-out flag after : 1s*/
#define HT162X_F4		0xa2
/** @brief Time base clock output : 8Hz.   The WDT time-out flag after : 1/2s*/
#define HT162X_F8		0xa3
/** @brief Time base clock output : 16Hz.  The WDT time-out flag after : 1/4s*/
#define HT162X_F16		0xa4
/** @brief Time base clock output : 32Hz.  The WDT time-out flag after : 1/8s*/
#define HT162X_F32		0xa5
/** @brief Time base clock output : 64Hz.  The WDT time-out flag after : 1/16s*/
#define HT162X_F64		0xa6
/** @brief Time base clock output : 128Hz. The WDT time-out flag after : 1/32s*/
#define HT162X_F128		0xa7
/** @brief Test mode, user don't use*/
#define HT162X_TEST		0xe0
/** @brief Normal mode*/
#define HT162X_NORMAL	0xe3

#define LCD_WR_H   PLIB_PORTS_PinSet( PORTS_ID_0, PORT_CHANNEL_D, PORTS_BIT_POS_1)
#define LCD_WR_L    PLIB_PORTS_PinClear( PORTS_ID_0, PORT_CHANNEL_D, PORTS_BIT_POS_1)
#define LCD_DATA_H   PLIB_PORTS_PinSet( PORTS_ID_0, PORT_CHANNEL_D, PORTS_BIT_POS_3)
#define LCD_DATA_L    PLIB_PORTS_PinClear( PORTS_ID_0, PORT_CHANNEL_D, PORTS_BIT_POS_3)

#define LCD_CS1_H   PLIB_PORTS_PinSet( PORTS_ID_0, PORT_CHANNEL_D, PORTS_BIT_POS_2)
#define LCD_CS1_L    PLIB_PORTS_PinClear( PORTS_ID_0, PORT_CHANNEL_D, PORTS_BIT_POS_2)
#define LCD_CS2_H   PLIB_PORTS_PinSet( PORTS_ID_0, PORT_CHANNEL_D, PORTS_BIT_POS_12)
#define LCD_CS2_L    PLIB_PORTS_PinClear( PORTS_ID_0, PORT_CHANNEL_D, PORTS_BIT_POS_12)

//OIML Sample은 아래의 설정으로 되어있음.
//#define LCD_CS1_H   PLIB_PORTS_PinSet( PORTS_ID_0, PORT_CHANNEL_D, PORTS_BIT_POS_12)
//#define LCD_CS1_L    PLIB_PORTS_PinClear( PORTS_ID_0, PORT_CHANNEL_D, PORTS_BIT_POS_12)
//#define LCD_CS2_H   PLIB_PORTS_PinSet( PORTS_ID_0, PORT_CHANNEL_D, PORTS_BIT_POS_2)
//#define LCD_CS2_L    PLIB_PORTS_PinClear( PORTS_ID_0, PORT_CHANNEL_D, PORTS_BIT_POS_2)

void ht162XWriteCommand(INT8U datcom);

INT8U DSP_Memory[DSP_MAX_PAGE][128]; //CS1, CS2 의 전체를 사용할 경우를 대비하여 64+64 개 하여 128byte로 잡았음.실제 CS1에서의 여유공간은 Reserved 임.
INT8U DSP_Memory_backup[128];

/* (0x20 ~ 0x7F) (표현하지 못하는 문자는 0x07로 일괄표현)*/
const INT8U AsciicodeSegTable[] = {
	/* space                                                *    ,    -    .    /  */
    0x00,0x70,0x70,0x70,0x70,0x70,0x70,0x70,0x70,0x70,0x10,0x70,0x01,0x10,0x01,0x38,
	/*    0    1    2    3    4    5    6    7    8    9                               */
    0xEE,0x24,0xBA,0xB6,0x74,0xD6,0xDE,0xE4,0xFE,0xF6,0x70,0x70,0x70,0x70,0x70,0x70,
	/*    A    B    C    D    E    F    G    H    I    J    K    L    M    N    O		*/
    0x70,0xFC,0x5E,0xCA,0x3E,0xDA,0xD8,0xCE,0x7C,0x04,0x2E,0x58,0x4A,0x9C,0x1C,0x1E,
   /* P    Q    R    S    T    U    V    W    X    Y    Z                       _  */
	0xF8,0xF4,0x18,0xD6,0x5A,0x6E,0x0E,0x8E,0x92,0x76,0xBA,0x70,0x70,0x70,0x70,0x02,
	/*    a    b    c    d    e    f    g    h    i    j    k    l    m    n    o		*/
    0x70,0xFC,0x5E,0xCA,0x3E,0xDA,0xD8,0xCE,0x7C,0x04,0x2E,0x58,0x4A,0x9C,0x1C,0x1E,
	/*    p    q    r    s    t    u    v    w    x    y    z                          */
	0xF8,0xF4,0x18,0xD6,0x5A,0x6E,0x0E,0x8E,0x92,0x76,0xBA,0x70,0x70,0x70,0x70,0x70
};

/* (0x80 ~ 0xFF) */
#if (FONT_CODEPAGE == CODEPAGE_1254)
/* (0x80 ~ 0xFF) */
const INT8U fontCodePage[] = {
    /* (0x80 ~ 0x8F) */
    0x70,0x70,0x01,0x70,0x70,0x70,0x70,0x70,0x70,0x70,0xD6,0x70,0x70,0x70,0x70,0x70,
    /* (0x90 ~ 0x9F) */
    0x70,0x70,0x70,0x70,0x70,0x70,0x10,0x10,0x70,0x70,0xD6,0x70,0x70,0x70,0x70,0x76,
    /* (0xA0 ~ 0xAF) */
    0x70,0x70,0x70,0x70,0x70,0x70,0x70,0x70,0x70,0x70,0x70,0x70,0x14,0x10,0x70,0x80,
    /* (0xB0 ~ 0xBF) */
    0x70,0x70,0x70,0x70,0x70,0x70,0x70,0x70,0x01,0x70,0x70,0x70,0x70,0x70,0x70,0x70,
    /* (0xC0 ~ 0xCF) */
    0xFC,0xFC,0xFC,0xFC,0xFC,0xFC,0x70,0xCA,0xDA,0xDA,0xDA,0xDA,0x04,0x04,0x04,0x04,
    /* (0xD0 ~ 0xDF) */
    0xCE,0x1C,0x1E,0x1E,0x1E,0x1E,0x1E,0x92,0x1E,0x6E,0x6E,0x6E,0x6E,0x04,0xD6,0x70,
    /* (0xE0 ~ 0xEF) */
    0xFC,0xFC,0xFC,0xFC,0xFC,0xFC,0x70,0xCA,0xDA,0xDA,0xDA,0xDA,0x04,0x04,0x04,0x04,
    /* (0xF0 ~ 0xFF) */
    0xCE,0x1C,0x1E,0x1E,0x1E,0x1E,0x1E,0x70,0x1E,0x6E,0x6E,0x6E,0x6E,0x04,0xD6,0x76
};
/* (0x80 ~ 0xFF) */
#elif (FONT_CODEPAGE == CODEPAGE_1256) //Display Font가 영문이기 때문에 CDOEPAGE1252와 동일
const INT8U fontCodePage[] = {
    /* (0x80 ~ 0x8F) */
    0x70,0x70,0x01,0x70,0x70,0x70,0x70,0x70,0x70,0x70,0xD6,0x70,0x70,0x70,0xBA,0x70,
    /* (0x90 ~ 0x9F) */
    0x70,0x70,0x70,0x70,0x70,0x70,0x10,0x10,0x70,0x70,0xD6,0x70,0x70,0x70,0xBA,0x76,
    /* (0xA0 ~ 0xAF) */
    0x70,0x70,0x70,0x70,0x70,0x70,0x70,0x70,0x70,0x70,0x70,0x70,0x14,0x10,0x70,0x80,
    /* (0xB0 ~ 0xBF) */
    0x70,0x70,0x70,0x70,0x70,0x70,0x70,0x70,0x01,0x70,0x70,0x70,0x70,0x70,0x70,0x70,
    /* (0xC0 ~ 0xCF) */
    0xFC,0xFC,0xFC,0xFC,0xFC,0xFC,0x70,0xCA,0xDA,0xDA,0xDA,0xDA,0x04,0x04,0x04,0x04,
    /* (0xD0 ~ 0xDF) */
    0x3E,0x1C,0x1E,0x1E,0x1E,0x1E,0x1E,0x92,0x1E,0x6E,0x6E,0x6E,0x6E,0x76,0x70,0x70,
    /* (0xE0 ~ 0xEF) */
    0xFC,0xFC,0xFC,0xFC,0xFC,0xFC,0x70,0xCA,0xDA,0xDA,0xDA,0xDA,0x04,0x04,0x04,0x04,
    /* (0xF0 ~ 0xFF) */
    0x1E,0x1C,0x1E,0x1E,0x1E,0x1E,0x1E,0x70,0x1E,0x6E,0x6E,0x6E,0x6E,0x76,0xF8,0x76
};
#elif ((FONT_CODEPAGE == CODEPAGE_1251) || (FONT_CODEPAGE == CODEPAGE_1251_MNG))
const INT8U fontCodePage[] = {
    /* (0x80 ~ 0x8F) */
    0x70,0x70,0x01,0x70,0x70,0x70,0x70,0x70,0x70,0x70,0x70,0x70,0x70,0x70,0x70,0x70,
    /* (0x90 ~ 0x9F) */
    0x70,0x70,0x70,0x70,0x70,0x70,0x10,0x10,0x70,0x70,0x70,0x70,0x70,0x70,0x70,0x70,
    /* (0xA0 ~ 0xAF) */
    0x70,0x70,0x70,0x2E,0x70,0x70,0x70,0x70,0xDA,0x70,0x70,0x70,0x14,0x10,0x70,0x70,
    /* (0xB0 ~ 0xBF) */
    0x70,0x70,0x70,0x24,0x18,0x70,0x70,0x70,0x70,0x70,0x70,0x70,0x70,0xD6,0xD6,0x70,
    /* (0xC0 ~ 0xCF) */
    0xFC,0x70,0x70,0x70,0xFC,0xDA,0x70,0x70,0x70,0x70,0x70,0x70,0x70,0x7C,0x1E,0x70,
    /* (0xD0 ~ 0xDF) */
    0xF8,0xCA,0x70,0x76,0x70,0x70,0x70,0x70,0x70,0x70,0x70,0x70,0x5E,0x70,0x70,0x70,
    /* (0xE0 ~ 0xEF) */
    0x70,0x70,0x70,0x70,0x70,0x70,0x70,0x70,0x70,0x70,0x70,0x70,0x70,0x7C,0x1E,0x70,
    /* (0xF0 ~ 0xFF) */
    0xF8,0xCA,0x70,0x76,0x70,0x70,0x70,0x70,0x70,0x70,0x70,0x70,0x5E,0x70,0x70,0x70
};
#elif (FONT_CODEPAGE == CODEPAGE_1252_ARM)
const INT8U fontCodePage[] = {
    /* (0x80 ~ 0x8F) */
    0x70,0x70,0x01,0x70,0x70,0x70,0x70,0x70,0x70,0x70,0x70,0x70,0x70,0x70,0x70,0x70,
    /* (0x90 ~ 0x9F) */
    0x70,0x70,0x70,0x70,0x70,0x70,0x10,0x10,0x70,0x70,0x70,0x70,0x70,0x70,0x70,0x70,
    /* (0xA0 ~ 0xAF) */
    0x70,0x70,0x70,0x70,0x70,0x70,0x70,0x70,0x4E,0x01,0x70,0x01,0x80,0x10,0x70,0x70,
    /* (0xB0 ~ 0xBF) */
    0x70,0x70,0x6E,0x70,0x70,0x70,0x70,0x70,0x70,0x70,0x70,0x70,0x70,0x70,0x70,0x70,
    /* (0xC0 ~ 0xCF) */
    0x70,0x70,0x70,0x70,0x70,0x70,0x70,0x70,0x4A,0x70,0x70,0x70,0x70,0x7C,0x70,0x70,
    /* (0xD0 ~ 0xDF) */
    0x70,0x70,0x70,0x70,0x70,0x70,0x70,0x70,0x6E,0x70,0x70,0x70,0x70,0x70,0x70,0x70,
    /* (0xE0 ~ 0xEF) */
    0x70,0x1C,0x70,0x70,0x70,0x70,0x70,0x70,0x70,0x70,0x6E,0x70,0x70,0x7C,0xD6,0x70,
    /* (0xF0 ~ 0xFF) */
    0x70,0x70,0x70,0x70,0x70,0x70,0x70,0x70,0x70,0x70,0xEE,0x1E,0x70,0x70,0x70,0x70
};
/* 새로운 CodePage를 추가하려면 #elif (FONT_CODEPAGE == CODEPAGE_xxxx)와 같이 추가할 것 */

#else // (FONT_CODEPAGE == CODEPAGE_1252, CODEPAGE_1251_GEO, CODEPAGE_1250_POL, CODEPAGE_1250, CODEPAGE_1251_KAZ)
const INT8U fontCodePage[] = {
    /* (0x80 ~ 0x8F) */
    0x70,0x70,0x01,0x70,0x70,0x70,0x70,0x70,0x70,0x70,0xD6,0x70,0x70,0x70,0xBA,0x70,
    /* (0x90 ~ 0x9F) */
    0x70,0x70,0x70,0x70,0x70,0x70,0x10,0x10,0x70,0x70,0xD6,0x70,0x70,0x70,0xBA,0x76,
    /* (0xA0 ~ 0xAF) */
    0x70,0x70,0x70,0x70,0x70,0x70,0x70,0x70,0x70,0x70,0x70,0x70,0x14,0x10,0x70,0x80,
    /* (0xB0 ~ 0xBF) */
    0x70,0x70,0x70,0x70,0x70,0x70,0x70,0x70,0x01,0x70,0x70,0x70,0x70,0x70,0x70,0x70,
    /* (0xC0 ~ 0xCF) */
    0xFC,0xFC,0xFC,0xFC,0xFC,0xFC,0x70,0xCA,0xDA,0xDA,0xDA,0xDA,0x04,0x04,0x04,0x04,
    /* (0xD0 ~ 0xDF) */
    0x3E,0x1C,0x1E,0x1E,0x1E,0x1E,0x1E,0x92,0x1E,0x6E,0x6E,0x6E,0x6E,0x76,0x70,0x70,
    /* (0xE0 ~ 0xEF) */
    0xFC,0xFC,0xFC,0xFC,0xFC,0xFC,0x70,0xCA,0xDA,0xDA,0xDA,0xDA,0x04,0x04,0x04,0x04,
    /* (0xF0 ~ 0xFF) */
    0x1E,0x1C,0x1E,0x1E,0x1E,0x1E,0x1E,0x70,0x1E,0x6E,0x6E,0x6E,0x6E,0x76,0xF8,0x76
};
#endif

typedef struct{
    INT8U dispPos;
    INT8U bitPos;
}SIGN;

ROMDATA  SIGN signTable[LCD_SIGN_MAX] ={
	//   {Disp_buffer_position, bit MASK}
    // 0 ~ 4
	{ 7, 0x80}, //S1
	{ 7, 0x40}, //S2
	{ 64, 0x01}, //S3
	{ 64, 0x02}, //S4
	{ 64, 0x04}, //S5 <USB>
	// 5 ~ 9
	{ 64, 0x08}, //S6
	{ 64, 0x10}, //S7
	{ 64, 0x20}, //S8  <AUTO>
	{ 64, 0x40}, //S9 <CONNECT>
	{ 64, 0x80}, //S10 <SAVE>
	//10 ~ 14
	{ 96, 0x80}, //S11
	{127, 0x80}, //S12 <PREPACK>
	{127, 0x40}, //S13 Check in
	{127, 0x20}, //S14 <LN>
	{127, 0x10}, //S15 <D/C>
	//15 ~ 19
	{127, 0x08}, //S16 LANG
	{127, 0x04}, //S17 <SHIFT>
	{127, 0x02}, //S18
	{127, 0x01}, //S19 <TR>
	{ 51, 0x01}, //S20 ZERO symbol
	//20 ~ 24
	{ 50, 0x01}, //S21 TARE Tare symbol
	{ 49, 0x01}, //S22 Stable symbol
	{ 96, 0x02}, //S23 batt border
	{ 65, 0x02}, //S24 batt hi
	{ 65, 0x02}, //S25 batt low
	//25 ~ 29
	{ 65, 0x04}, //S26 antenna symbol
	{ 65, 0x08}, //S27 antenna low
	{ 65, 0x10}, //S28 antenna mid
	{ 65, 0x20}, //S29 antenna hi
	{ 65, 0x40}, //S30 MASTER
	//30 ~ 34
	{ 65, 0x80}, //S31 SLAVE
	{ 96, 0x02}, //S32
	{ 96, 0x04}, //S33
	{ 27, 0x01}, //S34
	{ 28, 0x80}, //S35
	//35
};


/**
 ********************************************************************************
 * @brief    HT162X IC에 command 전송 함수
 * @param    datcom : command\n
 * @return   none
 * @remark
 ********************************************************************************
 */
void delay(void);
void Ht1625Wr_Data(INT8U Dat,INT8U num);
void Ht1625Wr_Cmd(INT8U Dat,INT8U num);
void DSP_LCD7_char_diffuse(void);

void __attribute__((optimize("-O0"))) ht162XWriteCommand(INT8U datcom)
{
 	LCD_CS1_L;

	delay();
	Ht1625Wr_Cmd(0x80, 3);
	Ht1625Wr_Cmd(datcom, 9);

	LCD_CS1_H;
	delay();

	LCD_CS2_L;

	delay();
	Ht1625Wr_Cmd(0x80, 3);
	Ht1625Wr_Cmd(datcom, 9);

	LCD_CS2_H;
	delay();
}

void __attribute__((optimize("-O0"))) delay(void)
{
	INT8U i = 15; //Write mode minimum delay => 5V = 1.67 usec. 3V = 3.34 usec
	while(i--);
}

void __attribute__((optimize("-O0"))) Ht1625Wr_Cmd(INT8U Dat,INT8U num)
{
	INT8U i;

	for (i=0;i<num;i++)
	{
		LCD_WR_L;
		delay();

		if(Dat&0x80) LCD_DATA_H;
		else LCD_DATA_L;
		delay();

		LCD_WR_H;
		delay();

		Dat=Dat << 1;
	}
}
void __attribute__((optimize("-O0"))) Ht1625Wr_Data(INT8U Dat, INT8U num)
{
	INT8U i;

	for (i = 0; i < num; i++)
	{
		LCD_WR_L;
		delay();

		if(Dat&0x01) LCD_DATA_H;
		else LCD_DATA_L;
		delay();

		LCD_WR_H;
		delay();

		Dat >>= 1;
	}
}

void DSP_LCD7_diffuse(void)
{
	INT8U i;

	i = LCD7_37D_40DOT_CS1_START_ADDR;

    LCD_CS1_L;
	delay();
    Ht1625Wr_Cmd(0xa0, 3);
	Ht1625Wr_Cmd((i * 2) << 1, 7);
	for (i = LCD7_37D_40DOT_CS1_START_ADDR; i < LCD7_37D_40DOT_CS1_END_ADDR; i++)
	{
		Ht1625Wr_Data(DSP_Memory[DspStruct.Page][i] & 0x0f, 4);
		Ht1625Wr_Data((DSP_Memory[DspStruct.Page][i] >> 4) & 0x0f, 4);
	}
    LCD_CS1_H;
    delay();


//S3~S10, S24~S31
	i = LCD7_37D_40DOT_CS2_START_ADDR;
    LCD_CS2_L;
	delay();
    Ht1625Wr_Cmd(0xa0, 3);
	Ht1625Wr_Cmd((i * 2) << 1, 7);
	for (i = LCD7_37D_40DOT_CS2_START_ADDR; i < LCD7_37D_40DOT_CS2_START_ADDR + 2; i++)
	{
		Ht1625Wr_Data(DSP_Memory[DspStruct.Page][i] & 0x0f, 4);
		Ht1625Wr_Data((DSP_Memory[DspStruct.Page][i] >> 4) & 0x0f, 4);
	}
    LCD_CS2_H;
	delay();


// S23,S32,S33,S11
	i = LCD_CHAR_AREA1_OFFSET + LCD_CHAR_AREA1_SIZE;
    LCD_CS2_L;
	delay();
    Ht1625Wr_Cmd(0xa0, 3);
	Ht1625Wr_Cmd((i * 2) << 1, 7);
	for (; i < LCD_CHAR_AREA1_OFFSET + LCD_CHAR_AREA1_SIZE + 1; i++)
	{
		Ht1625Wr_Data(DSP_Memory[DspStruct.Page][i] & 0x0f, 4);
		Ht1625Wr_Data((DSP_Memory[DspStruct.Page][i] >> 4) & 0x0f, 4);
	}
    LCD_CS2_H;
	delay();


// S12~S19
	i = LCD_CHAR_AREA2_OFFSET + LCD_CHAR_AREA2_SIZE;
    LCD_CS2_L;
	delay();
    Ht1625Wr_Cmd(0xa0, 3);
	Ht1625Wr_Cmd((i * 2) << 1, 7);
	for (; i < LCD_CHAR_AREA2_OFFSET + LCD_CHAR_AREA2_SIZE + 1; i++)
	{
		Ht1625Wr_Data(DSP_Memory[DspStruct.Page][i] & 0x0f, 4);
		Ht1625Wr_Data((DSP_Memory[DspStruct.Page][i] >> 4) & 0x0f, 4);
	}
    LCD_CS2_H;
	delay();


	DSP_LCD7_char_diffuse();
}

void DSP_LCD7_char_diffuse(void)
{
	INT8U i;
	INT8U bufAddr;

	i = LCD_CHAR_AREA1_OFFSET;
	bufAddr = 0;
    LCD_CS2_L;
	delay();
    Ht1625Wr_Cmd(0xa0, 3);
	Ht1625Wr_Cmd((i * 2) << 1, 7);
	for (; i < LCD_CHAR_AREA1_OFFSET + LCD_CHAR_AREA1_SIZE; i++)
	{
		Ht1625Wr_Data(DspBuf[DspStruct.Page][bufAddr] & 0x0f, 4);
		Ht1625Wr_Data((DspBuf[DspStruct.Page][bufAddr] >> 4) & 0x0f, 4);
		bufAddr++;
	}
    LCD_CS2_H;
	delay();


	i = LCD_CHAR_AREA2_OFFSET;
	bufAddr = LCD_CHAR_AREA1_SIZE;
    LCD_CS2_L;
	delay();
    Ht1625Wr_Cmd(0xa0, 3);
	Ht1625Wr_Cmd((i * 2) << 1, 7);
	for (; i < LCD_CHAR_AREA2_OFFSET + LCD_CHAR_AREA2_SIZE; i++)
	{
		Ht1625Wr_Data(DspBuf[DspStruct.Page][bufAddr] & 0x0f, 4);
		Ht1625Wr_Data((DspBuf[DspStruct.Page][bufAddr] >> 4) & 0x0f, 4);
		bufAddr++;
	}
    LCD_CS2_H;
    delay();
}

void VFD7_display_bright(INT8U bright)//1~4 level
{

}


void VFD7_Init(void)
{
	ht162XWriteCommand(HT162X_ON);
	ht162XWriteCommand(HT162X_SYS_EN); /* Turn on system oscillator*/
	ht162XWriteCommand(HT162X_TONE_OFF); /* Turn off tone outputs */
	ht162XWriteCommand(HT162X_RC_32K); /* System clock source, on-chip RC oscillator*/
	ht162XWriteCommand(HT162X_TIMER_DIS);
	ht162XWriteCommand(HT162X_WDT_DIS);
	//ht162XWriteCommand(HT162X_BIAS_1_3);
	ht162XWriteCommand(HT162X_IRQ_DIS);	/* Disable IRQ output */
	ht162XWriteCommand(HT162X_NORMAL);	/* Normal mode*/

	memset(DSP_Memory, 0x00, sizeof(DSP_Memory));
	DSP_LCD7_diffuse();
}


extern HUGEDATA INT8U *DspFontAddrHpf;
void VFD7_display_char(INT8U nDigitPos, INT8U cData)
{
	INT8U i;
	INT16U fData;
	INT8U digitPos;
	//	INT8U dotPos;
	INT8U commaPos;
	INT8U charPos;
	INT16U font_addr;

	if (nDigitPos < DSP_MAX_DIGIT)
	{
		if (nDigitPos < LCD_TARE_END_PTR)
		{
			digitPos = nDigitPos + LCD_TARE_OFFSET;
		}
		else
		{
			digitPos = nDigitPos + LCD_WEIGHT_OFFSET;
		}
		//		dotPos = digitPos;

		//DSP_Memory[dotPos] = DSP_Memory[dotPos];

		if (cData >= 0x20 && cData <= 0x7F)
		{
			fData = AsciicodeSegTable[cData - 0x20];
		}
		else if (cData >= 0x80 && cData <= 0xFF)
		{
			fData = fontCodePage[cData - 0x80];
		}
		else if (cData == CURSOR_CHAR)
		{
			fData = CURSOR_SEG;
		}
		else if (cData == '\0')
		{
			fData = '\0';
		}
		else
		{
			fData = OTHER_SEG;
		}


		if (cData == '.')
		{
			if (digitPos != 6 && digitPos != 27) // dot,comma가 없는 영역
			{
				DSP_Memory[DspStruct.Page][digitPos] |= DOT_SEG;
			}
		}
		else if (cData == ',')
		{
			if (digitPos < LCD_TARE_END_PTR + LCD_TARE_OFFSET)
			{
				if (digitPos != 6 && digitPos != 27) // dot,comma가 없는 영역
				{
    				DSP_Memory[DspStruct.Page][digitPos] |= DOT_SEG;
				}
				switch (digitPos)
				{
					// Tare Comma 위치가 연속적이지 않음 각 들어오는 자리를 참조로하여 하드코딩됨. CommaPos에 대입되는 번호의 의미는
					// LCD Driver IC Map 에서 확인 할 것.
					case 2:
						commaPos = 29;
						DSP_Memory[DspStruct.Page][commaPos] = COMMA_SEG;
						break;

					case 3:
						commaPos = 1;
						DSP_Memory[DspStruct.Page][commaPos] = COMMA_SEG;
						break;

					case 4:
						commaPos = 0;
						DSP_Memory[DspStruct.Page][commaPos] = COMMA_SEG;
						break;
					case 5:
						commaPos = 6;
						DSP_Memory[DspStruct.Page][commaPos] |= COMMA_SEG;
						break;
				}
			}
			else if ((digitPos >= LCD_WEIGHT_START_ADDR) && (digitPos < DSP_MAX_DIGIT + LCD_WEIGHT_OFFSET))
			{
				if (digitPos != 6 && digitPos != 27) // dot,comma가 없는 영역
				{
					DSP_Memory[DspStruct.Page][digitPos] |= DOT_SEG;

					commaPos = LCD_COMMA_START_ADDR - (digitPos - 8);

                    DSP_Memory[DspStruct.Page][commaPos] |= COMMA_SEG;
				}
			}
		}
		else
		{
			DSP_Memory[DspStruct.Page][digitPos] = fData;
			if (digitPos < LCD_TARE_END_PTR + LCD_TARE_OFFSET)
			{
				switch (digitPos)
				{
					// Tare Comma 위치가 연속적이지 않음 각 들어오는 자리를 참조로하여 하드코딩됨. CommaPos에 대입되는 번호의 의미는
					// LCD Driver IC Map 에서 확인 할 것.
					case 2:
						commaPos = 29;
						DSP_Memory[DspStruct.Page][commaPos] = 0;
						break;
					case 3:
						commaPos = 1;
						DSP_Memory[DspStruct.Page][commaPos] = 0;
						break;
					case 4:
						commaPos = 0;
						DSP_Memory[DspStruct.Page][commaPos] = 0;
						break;
					case 5:
						commaPos = 6;
						DSP_Memory[DspStruct.Page][commaPos] &= ~COMMA_SEG;
						break;
				}
			}
			else if ((digitPos >= LCD_WEIGHT_START_ADDR) && (digitPos < DSP_MAX_DIGIT + LCD_WEIGHT_OFFSET))
			{
				if (digitPos != 6 && digitPos != 27) // dot,comma가 없는 영역
				{
					commaPos = LCD_COMMA_START_ADDR - (digitPos - 8);
					DSP_Memory[DspStruct.Page][commaPos] &= ~COMMA_SEG;
				}
			}
		}
	}

	else return;
}

void VFD7_display_char_graphic(INT16U x, char cData)
{
	INT8U i;
	INT8U charPos;
	INT16U font_addr;

	if (x < DSP_MAX_CHAR * LCD_CHAR_FONT_WIDTH)
	{
		if (x < ((DSP_MAX_CHAR / 2) * LCD_CHAR_FONT_WIDTH))
		{
			charPos = x + LCD_CHAR_OFFSET;
		}
		else
		{
			charPos = x + LCD_CHAR_OFFSET + 1; //+1의 의미는 LCD Driver IC CS2 MAP 참조(중간에 Sign으로 빠지는 부분이 있어 점프)
		}

		font_addr = ((INT8U)cData)&0x00ff;
		font_addr *= 6;
		font_addr += 64;
		for (i = 0; i < LCD_CHAR_FONT_WIDTH; i++)
		{
			//Insert(delete) cursor
			DspBuf[DspStruct.Page][charPos] = DspFontAddrHpf[font_addr + i];
			if (DspStruct.DeleteCur) DspBuf[DspStruct.Page][charPos] ^= 0x80;
			else if (DspStruct.InsertCur) DspBuf[DspStruct.Page][charPos] ^= 0x80;
			charPos++;
		}
	}
}

void VFD7_display_triangle(INT8U nDigitPos, INT8U onoff)
{
	INT8U pos;
	INT8U seg;

	pos = signTable[nDigitPos].dispPos;
	seg = signTable[nDigitPos].bitPos;

	if(nDigitPos >= LCD_SIGN_MAX) return;

	if (onoff) DSP_Memory[DspStruct.Page][pos] |= seg;
	else DSP_Memory[DspStruct.Page][pos] &= ~seg;
}

void VFD7_display_triangle_all(INT8U onoff)
{
	INT8U i;

	for (i = 0; i < LCD_SIGN_MAX; i++)
	{
		VFD7_display_triangle(i, onoff);
	}
}

void VFD7_display_string(HUGEDATA char *dataString, INT8U nDigitPos, char size)
{
	//DSP_str_common((HUGEDATA char *)dataString, nDigitPos, size);
	INT8U i,j;
	INT8U type_7seg;

	if (nDigitPos < (INT8U)DSP_MAX_DIGIT) type_7seg = 1;
	else type_7seg = 0;

	i=nDigitPos;
	j=0;
    if(i == 0)
    {
        i = nDigitPos;
    }
	//    if ((nDigitPos+size)>=DSP_MAX_DIGIT) return;
    while ((*(dataString) != '\0') && (j < size))
    {
        VFD7_display_char(i, *dataString);

        if (type_7seg)
        {
            if (*(dataString + 1) != '\0')
            {
                if (*(dataString + 1) == '.' || *(dataString + 1) == ',')
                {
                    VFD7_display_char(i, *(dataString + 1));
                    dataString++;
                }
            }
        }

		dataString++;
		i++;
		j++;
	}
	//clear remain area
	while(j<size)
	{
		VFD7_display_char(i, '\0');
		i++;
		j++;
	}
	//	DSP_LCD7_diffuse();//comm speed slow
}

void  VFD7_Diffuse(void)
{
	DSP_LCD7_diffuse();
}

void VFD7_Fill_Buffer(unsigned char x)
{
	memset(DSP_Memory[DspStruct.Page], x, LCD_CHAR_AREA1_OFFSET);
	memset(&DSP_Memory[DspStruct.Page][LCD_CHAR_AREA1_OFFSET+LCD_CHAR_AREA1_SIZE], x, 1);
	memset(&DSP_Memory[DspStruct.Page][LCD_CHAR_AREA2_OFFSET+LCD_CHAR_AREA2_SIZE], x, 1);
}

void VFD7_Fill_Buffer_CHAR(unsigned char x) // 5 x 12digit 용
{
	memset(&DspBuf[DspStruct.Page], x, LCD_CHAR_AREA1_SIZE + LCD_CHAR_AREA2_SIZE);
}

void VFD7_save_restore(INT8U x)
{
	if (x) {	// backup
		memcpy(DSP_Memory_backup, DSP_Memory[DspStruct.Page], sizeof(DSP_Memory[DspStruct.Page]));
	} else {	// restore
		memcpy(DSP_Memory[DspStruct.Page], DSP_Memory_backup, sizeof(DSP_Memory[DspStruct.Page]));
	}
}

#endif // _USE_LCD_7_37D_40DOT_