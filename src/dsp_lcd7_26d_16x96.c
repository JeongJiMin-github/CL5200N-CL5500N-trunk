/**
 * @file dsp_lcd7_26d_16x96.c
 * @attention (c) 2008 CAS
 * @version 0.1
 * @author 이동원
 * @date 2017/02/01
 * @brief LCD CL3000 Char LCD Version을 구동시키는 드라이버
 */

#include <stdio.h>
#include "globals.h"
#include "dsp_app.h"
#include "dsp_vfd7.h"
#include "prt_font.h"
#include "dsp_common.h"
#include "initial.h"

#ifdef _USE_LCD_7_26d_16x96_

#define USE_DISP_8PIN_CON // LCD Connector 6pin(Old Version)

#define MINUS_SEG             0x40
#define SPACE_SEG             0x08
#define DOT_SEG               0x01
#define COMMA_SEG             0x01
#define SPA_SEG               0x00
#define ALL_SEG               0xee
#define CURSOR_SEG			  0x08
#define OTHER_SEG             0x62
#define SLASH_SEG             0xC2

#define DIFFUSE_ALL		0
#define DIFFUSE_7SEG	1
#define DIFFUSE_12CHAR	2

#define LCD_IC1		1
#define LCD_IC2		2

#define LCD_SIGN_MAX            36 //S1~S26
#define LCD_PAGE_MAX		2 // page 0,1
#define LCD_COLUMN_MAX		96
#define LCD_SEG_AREA_SIZE		24
#define LCD_CHAR_AREA_SIZE		96

// ST7522 INSTRUCTIONS //
/** @brief Display on*/
#define ST7522_DISPLAY_ON	0xAF
/** @brief Display off*/
#define ST7522_DISPLAY_OFF	0xAE
/** @brief Set display page address*/
#define ST7522_PAGE_ADDR_SET	0xB0
/** @brief Set MSB 4 bits of column address*/
#define ST7522_ADDR_SET_HIGH	0x10
/** @brief Set LSB 4 bits of column address*/
#define ST7522_ADDR_SET_LOW		0x00
///** @brief Write display data*/
//#define ST7522_DATA_WRITE
/** @brief Determines the RAM display line for COM 0*/
#define ST7522_START_LINE_SET	0x40
/** @brief Display RAM and Segment output correspondence*/
#define ST7522_ADC_SELECT		0xA0
/** @brief Set LCD display reverse*/
#define ST7522_DISP_NORMAL		0xA6
/** @brief Set LCD display reverse*/
#define ST7522_DISP_REVERSE		0xA7
/** @brief Set display all point on*/
#define ST7522_DISP_ALL_ON		0xA5
/** @brief Set display all point off*/
#define ST7522_DISP_ALL_OFF		0xA4
/** @brief Select LCD duty*/
#define ST7522_DUTY_SELECT		0xA8
/** @brief Select LCD bias voltage*/
#define ST7522_BIAS_SET		0xA2
/** @brief Internal reset*/
#define ST7522_RESET		0xE2
/** @brief Power control*/
#define ST7522_POWER_CONTROL	0x28
/** @brief Set contrast by 64 level (V5 fine adjust)*/
#define ST7522_CONTRAST_SET		0x81
/** @brief Internal OSC frequency adjust*/
#define ST7522_OSC_FREQUENCY_SET	0xF1
/** @brief V5 follower input voltage select(V5 coarse adjust)*/
#define ST7522_FOLLOWER_VOLT_SET	0xF8
/** @brief V5 follower amplified ratio*/
#define ST7522_AMPLIFIED_RATIO		0x20
/** @brief Booster input voltage select*/
#define ST7522_BOOSTER_VOLT_SET		0xF0

#define LCD_WR_H   PLIB_PORTS_PinSet( PORTS_ID_0, PORT_CHANNEL_D, PORTS_BIT_POS_1)		// SCLK
#define LCD_WR_L    PLIB_PORTS_PinClear( PORTS_ID_0, PORT_CHANNEL_D, PORTS_BIT_POS_1)
#define LCD_DATA_H   PLIB_PORTS_PinSet( PORTS_ID_0, PORT_CHANNEL_D, PORTS_BIT_POS_3)	// Data
#define LCD_DATA_L    PLIB_PORTS_PinClear( PORTS_ID_0, PORT_CHANNEL_D, PORTS_BIT_POS_3)
#ifdef USE_DISP_8PIN_CON
#define LCD_A0_H   PLIB_PORTS_PinSet( PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_1)		// Data Write
#define LCD_A0_L    PLIB_PORTS_PinClear( PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_1)	// Command
#define LCD_CS1_H   PLIB_PORTS_PinSet( PORTS_ID_0, PORT_CHANNEL_D, PORTS_BIT_POS_2)	// IC1 (7 Seg)
#define LCD_CS1_L    PLIB_PORTS_PinClear( PORTS_ID_0, PORT_CHANNEL_D, PORTS_BIT_POS_2)
#define LCD_CS2_H   PLIB_PORTS_PinSet( PORTS_ID_0, PORT_CHANNEL_D, PORTS_BIT_POS_12)	// IC2 on(Graphic LCD)
#define LCD_CS2_L    PLIB_PORTS_PinClear( PORTS_ID_0, PORT_CHANNEL_D, PORTS_BIT_POS_12)
#else
#define LCD_A0_H   PLIB_PORTS_PinSet( PORTS_ID_0, PORT_CHANNEL_D, PORTS_BIT_POS_2)		// Data Write
#define LCD_A0_L    PLIB_PORTS_PinClear( PORTS_ID_0, PORT_CHANNEL_D, PORTS_BIT_POS_2)	// Command
#define LCD_CS1_H   PLIB_PORTS_PinSet( PORTS_ID_0, PORT_CHANNEL_D, PORTS_BIT_POS_12)	// IC2 (7 Seg)
#define LCD_CS1_L    PLIB_PORTS_PinClear( PORTS_ID_0, PORT_CHANNEL_D, PORTS_BIT_POS_12) // IC1 (Graphic LCD)
#define LCD_CS2_H   
#define LCD_CS2_L    
#endif

INT8U DSP_Memory[DSP_MAX_PAGE][240]; // (96*2) + (24*2) = (graphic) + (7-seg)
POINT DSP_Memory_SegPos[DSP_MAX_DIGIT];
POINT DSP_Memory_DotPos[DSP_MAX_DIGIT];
POINT DSP_Memory_ComPos[DSP_MAX_DIGIT];
INT8U DSP_Memory_backup[240];
INT8U ContrastLevel;

POINT DSP_Memory_SegPos[DSP_MAX_DIGIT] = 
{
	// Tare
	{0,0},
	{2,0},
	{4,0},
	{7,0},
	{9,0},
	// Weight
	{11,0},
	{13,0},
	{15,0},
	{17,0},
	{19,0},
	{21,0},
	// Unit price
	{9,5},
	{11,5},
	{13,5},
	{15,5},
	{17,5},
	{19,5},
	{21,5},
	// Total price
	{31,2},
	{33,2},
	{35,2},
	{37,2},
	{39,2},
	{41,2},
	{43,2},
	{45,2},
};

POINT DSP_Memory_DotPos[DSP_MAX_DIGIT] =
{
	// Tare
	{2, 4},
	{4, 4},
	{7, 4},
	{9, 4},
	{0, 0},
	// Weight
	{13, 4},
	{15, 4},
	{17, 4},
	{19, 4},
	{21, 4},
	{0, 0},
	// Unit price
	{35, 1},
	{37, 1},
	{39, 1},
	{41, 1},
	{43, 1},
	{45, 1},
	{0, 0},
	// Total price
	{33, 6},
	{35, 6},
	{37, 6},
	{39, 6},
	{41, 6},
	{43, 6},
	{45, 6},
	{0, 0},
};

POINT DSP_Memory_ComPos[DSP_MAX_DIGIT] = 
{
	//Tare
	{3, 4},
	{5, 4},
	{8, 4},
	{10, 4},
	{0, 0},
	// Weight
	{14, 4},
	{16, 4},
	{18, 4},
	{20, 4},
	{22, 4},
	{0, 0},
	// Unit price
	{36, 1},
	{38, 1},
	{40, 1},
	{42, 1},
	{44, 1},
	{46, 1},
	{0, 0},
	// Total price
	{34, 6},
	{36, 6},
	{38, 6},
	{40, 6},
	{42, 6},
	{44, 6},
	{46, 6},
	{0, 0},
};

/* (0x20 ~ 0x7F) (표현하지 못하는 문자는 0x62로 일괄표현)*/
const INT8U AsciicodeSegTable[] = {
	/* space                                                *    ,    -    .    /  */
    0x00,0x62,0x62,0x62,0x62,0x62,0x62,0x62,0x62,0x62,0x40,0x62,0x01,0x40,0x01,0xC2,
/*   0    1    2    3    4    5    6    7    8    9                               */
    0xAF,0x06,0xCB,0x4F,0x66,0x6D,0xED,0x27,0xEF,0x6F,0x62,0x62,0x62,0x62,0x62,0x62,
	/*    A    B    C    D    E    F    G    H    I    J    K    L    M    N    O		*/
    0x62,0xE7,0xEC,0xA9,0xCE,0xE9,0xE1,0xAD,0xE6,0x04,0x8E,0xE0,0xA8,0xC5,0xC4,0xCC,
   /* P    Q    R    S    T    U    V    W    X    Y    Z                       _  */
	0xE3,0x67,0xC0,0x6D,0xE8,0xAE,0x8C,0x8D,0x49,0x6E,0xCB,0x62,0x62,0x62,0x62,0x08,
	/*    a    b    c    d    e    f    g    h    i    j    k    l    m    n    o		*/
    0x70,0xE7,0xEC,0xA9,0xCE,0xE9,0xE1,0xAD,0xE6,0x04,0x8E,0xE0,0xA8,0xC5,0xC4,0xCC,
	/*    p    q    r    s    t    u    v    w    x    y    z                          */
	0xE3,0x67,0xC0,0x6D,0xE8,0xAE,0x8C,0x8D,0x49,0x6E,0xCB,0x62,0x62,0x62,0x62,0x62
};

/* (0x80 ~ 0xFF) */
#if (FONT_CODEPAGE == CODEPAGE_1254)
/* (0x80 ~ 0xFF) */
const INT8U fontCodePage[] = {
    /* (0x80 ~ 0x8F) */
    0x62,0x62,0x00,0x62,0x62,0x62,0x62,0x62,0x62,0x62,0x6D,0x62,0x62,0x62,0x62,0x62,
    /* (0x90 ~ 0x9F) */
    0x62,0x62,0x62,0x62,0x62,0x62,0x40,0x40,0x62,0x62,0x6D,0x62,0x62,0x62,0x62,0x6E,
    /* (0xA0 ~ 0xAF) */
    0x62,0x62,0x62,0x62,0x62,0x62,0x62,0x62,0x62,0x62,0x62,0x62,0x44,0x40,0x62,0x01,
    /* (0xB0 ~ 0xBF) */
    0x62,0x62,0x62,0x62,0x62,0x62,0x62,0x62,0x00,0x62,0x62,0x62,0x62,0x62,0x62,0x62,
    /* (0xC0 ~ 0xCF) */
    0xE7,0xE7,0xE7,0xE7,0xE7,0xE7,0x62,0xA9,0xE9,0xE9,0xE9,0xE9,0x04,0x04,0x04,0x04,
    /* (0xD0 ~ 0xDF) */
    0xAD,0xC4,0xCC,0xCC,0xCC,0xCC,0xCC,0x49,0xCC,0xAE,0xAE,0xAE,0xAE,0x04,0x6D,0x62,
    /* (0xE0 ~ 0xEF) */
    0xE7,0xE7,0xE7,0xE7,0xE7,0xE7,0x62,0xA9,0xE9,0xE9,0xE9,0xE9,0x04,0x04,0x04,0x04,
    /* (0xF0 ~ 0xFF) */
    0xAD,0xC4,0xCC,0xCC,0xCC,0xCC,0xCC,0x62,0xCC,0xAE,0xAE,0xAE,0xAE,0x04,0x6D,0x6E,
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
    0x62,0x62,0x00,0x62,0x62,0x62,0x62,0x62,0x62,0x62,0x62,0x62,0x62,0x62,0x62,0x62,
    /* (0x90 ~ 0x9F) */
    0x62,0x62,0x62,0x62,0x62,0x62,0x40,0x40,0x62,0x62,0x62,0x62,0x62,0x62,0x62,0x62,
    /* (0xA0 ~ 0xAF) */
    0x62,0x62,0x62,0x8E,0x62,0x62,0x62,0x62,0xE9,0x62,0x62,0x62,0x44,0x40,0x62,0x62,
    /* (0xB0 ~ 0xBF) */
    0x62,0x62,0x62,0x24,0xC0,0x62,0x62,0x62,0x62,0x62,0x62,0x62,0x62,0x6D,0x6D,0x62,
    /* (0xC0 ~ 0xCF) */
    0xE7,0x62,0x62,0x62,0xE7,0xE9,0x62,0x62,0x62,0x62,0x62,0x62,0x62,0xE6,0xCC,0x62,
    /* (0xD0 ~ 0xDF) */
    0xE3,0xA9,0x62,0x6E,0x62,0x62,0x62,0x62,0x62,0x62,0x62,0x62,0xEC,0x62,0x62,0x62,
    /* (0xE0 ~ 0xEF) */
    0x62,0x62,0x62,0x62,0x62,0x62,0x62,0x62,0x62,0x62,0x62,0x62,0x62,0xE6,0xCC,0x62,
    /* (0xF0 ~ 0xFF) */
    0xE3,0xA9,0x62,0x6E,0x62,0x62,0x62,0x62,0x62,0x62,0x62,0x62,0xEC,0x62,0x62,0x62
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

#elif (FONT_CODEPAGE == CODEPAGE_1253)
const INT8U fontCodePage[] = {
    /* (0x80 ~ 0x8F) */
    0x62,0x62,0x01,0x62,0x62,0x62,0x62,0x62,0x62,0x62,0x6D,0x62,0x62,0x62,0xCB,0x62,
    /* (0x90 ~ 0x9F) */
    0x62,0x62,0x62,0x62,0x62,0x62,0x40,0x40,0x62,0x62,0x6D,0x62,0x62,0x62,0xCB,0x6E,
    /* (0xA0 ~ 0xAF) */
    0x62,0x62,0x62,0x62,0x62,0x62,0x62,0x62,0x62,0x62,0x62,0x62,0x44,0x40,0x62,0x01,
    /* (0xB0 ~ 0xBF) */
    0x62,0x62,0x62,0x62,0x62,0x62,0x62,0x62,0x00,0x62,0x62,0x62,0x62,0x62,0x62,0x62,
    /* (0xC0 ~ 0xCF) */
    0xE7,0xE7,0xE7,0xE7,0xE7,0xE7,0x62,0xA9,0xE9,0xE9,0xE9,0xE9,0x04,0x66,0x04,0xC4,
    /* (0xD0 ~ 0xDF) */
    0xCE,0xC4,0xCC,0xCC,0xCC,0xCC,0xCC,0x49,0xCC,0xAE,0xAE,0xAE,0xAE,0x6E,0x62,0x62,
    /* (0xE0 ~ 0xEF) */
    0xE7,0xE7,0xE7,0xE7,0xE7,0xE7,0x62,0xA9,0xE9,0xE9,0xE9,0xE9,0x04,0x04,0x04,0x04,
    /* (0xF0 ~ 0xFF) */
    0xCC,0xC4,0xCC,0xCC,0xCC,0xCC,0xCC,0x62,0xCC,0xAE,0xAE,0xAE,0xAE,0x6E,0xE3,0x6E
};

#else // (FONT_CODEPAGE == CODEPAGE_1252, CODEPAGE_1251_GEO, CODEPAGE_1250_POL, CODEPAGE_1252_IND, CODEPAGE_1250, CODEPAGE_1251_KAZ)
const INT8U fontCodePage[] = {
    /* (0x80 ~ 0x8F) */
    0x62,0x62,0x01,0x62,0x62,0x62,0x62,0x62,0x62,0x62,0x6D,0x62,0x62,0x62,0xCB,0x62,
    /* (0x90 ~ 0x9F) */
    0x62,0x62,0x62,0x62,0x62,0x62,0x40,0x40,0x62,0x62,0x6D,0x62,0x62,0x62,0xCB,0x6E,
    /* (0xA0 ~ 0xAF) */
    0x62,0x62,0x62,0x62,0x62,0x62,0x62,0x62,0x62,0x62,0x62,0x62,0x44,0x40,0x62,0x01,
    /* (0xB0 ~ 0xBF) */
    0x62,0x62,0x62,0x62,0x62,0x62,0x62,0x62,0x00,0x62,0x62,0x62,0x62,0x62,0x62,0x62,
    /* (0xC0 ~ 0xCF) */
    0xE7,0xE7,0xE7,0xE7,0xE7,0xE7,0x62,0xA9,0xE9,0xE9,0xE9,0xE9,0x04,0x04,0x04,0x04,
    /* (0xD0 ~ 0xDF) */
    0xCE,0xC4,0xCC,0xCC,0xCC,0xCC,0xCC,0x49,0xCC,0xAE,0xAE,0xAE,0xAE,0x6E,0x62,0x62,
    /* (0xE0 ~ 0xEF) */
    0xE7,0xE7,0xE7,0xE7,0xE7,0xE7,0x62,0xA9,0xE9,0xE9,0xE9,0xE9,0x04,0x04,0x04,0x04,
    /* (0xF0 ~ 0xFF) */
    0xCC,0xC4,0xCC,0xCC,0xCC,0xCC,0xCC,0x62,0xCC,0xAE,0xAE,0xAE,0xAE,0x6E,0xE3,0x6E
};
#endif

typedef struct{
    INT8U dispPos;
    INT8U bitPos;
}SIGN;

ROMDATA  SIGN signTable[LCD_SIGN_MAX] ={
	//   {Disp_buffer_position, bit MASK}
    // 0 ~ 4
	{23, 0x10}, //Z23     KG
	{23, 0x20}, //Z22     LB
	{30, 0x20}, //Z1      (미사용)
	{33, 0x80}, //Z2      (미사용)
	{35, 0x80}, //Z3      USB
	// 5 ~ 9
	{99, 0x00}, //    S6  (사용불가)
	{99, 0x00}, //    S7  (사용불가)
	{ 8, 0x80}, //Z36     <AUTO>
	{37, 0x80}, //Z4      Connect
	{32, 0x01}, //Z35     <SAVE>
	//10 ~ 14
	{99, 0x00}, //    S11 (사용불가)
	{ 8, 0x40}, //Z37     <PREPACK>
	{41, 0x80}, //Z6      CHECKIN
	{43, 0x80}, //Z7      <LN>
	{ 8, 0x20}, //Z38     <D/C>
	//15 ~ 19
	{45, 0x80}, //Z8      LANG
	{31, 0x01}, //Z31     <SHIFT>
	{99, 0x00}, //    S18 (사용불가)
	{47, 0x40}, //Z9      <TR>
	{ 7, 0x40}, //Z33     ZERO symbol
	//20 ~ 24
	{ 7, 0x80}, //Z32     TARE Tare symbol
	{ 7, 0x20}, //Z34     Stable symbol
	{30, 0x10}, //Z28     batt border
	{30, 0x04}, //Z29     batt hi
	{30, 0x08}, //Z30     batt low
	//25 ~ 29
	{ 6, 0x20}, //Z24     antenna symbol
	{ 6, 0x40}, //Z25     antenna low
	{ 6, 0x80}, //Z26     antenna mid
	{30, 0x01}, //Z27     antenna hi
	{39, 0x80}, //Z5      MASTER
	//30 ~ 34
	{99, 0x00}, //    S31 (사용불가) SLAVE 미사용
	{99, 0x00}, //    S32 (사용불가)
	{47, 0x01}, //Z19     PER_LB
	{23, 0x80}, //Z20     PER_G
	{23, 0x40}, //Z21     PER_KG
	//35
	{99, 0x00}, //    S36 (사용불가)
};


/**
 ********************************************************************************
 * @brief    St7522 IC에 command 전송 함수
 * @param    datcom : command\n
 * @return   none
 * @remark   
 ********************************************************************************
 */
void delay(void);
void St7522Wr_Send(INT8U Dat);
void St7522_WriteCommand(INT8U datcom);
void St7522_WriteData(INT8U datcom);
void St7522_clear(void);
void DSP_LCD7_diffuse(void);
INT8U Rev_Data(INT8U data);


void __attribute__((optimize("-O0"))) St7522_clear(void)
{
	int i, j;
	INT8U cmd;
	
	
	for (j = 0; j < LCD_PAGE_MAX; j++)
	{
		cmd = ST7522_PAGE_ADDR_SET | j;
		St7522_WriteCommand(cmd);
		St7522_WriteCommand(ST7522_ADDR_SET_LOW); // Low column 0 set
		St7522_WriteCommand(ST7522_ADDR_SET_HIGH); // High column 0 set

		for (i = 0; i < LCD_CHAR_AREA_SIZE; i++)
		{
			St7522_WriteData(0x00); // 0으로 초기화
		}
	}
}

void __attribute__((optimize("-O0"))) St7522_WriteCommand(INT8U datcom)
{
	LCD_A0_L;
	delay();
	
	St7522Wr_Send(datcom);
}

void __attribute__((optimize("-O0"))) St7522_WriteData(INT8U datcom)
{
	LCD_A0_H;
	delay();
	
	St7522Wr_Send(datcom);
}

void __attribute__((optimize("-O0"))) delay(void)
{
	// Write mode minimum delay => 5V = 1.67 usec. 3V = 3.34 usec
	INT8U i = 15; // 25.05 usec
	while(i--);
}

void __attribute__((optimize("-O0"))) St7522Wr_Send(INT8U Dat)
{
	INT8U i;
	
	for (i = 0;i < 8 ;i++)
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
//	delay();
//	delay();
}

// LCD 전체 Diffuse
void DSP_LCD7_diffuse(void)
{
	int i ,j, addr;
	INT8U cmd, data;

	// IC2 Select
#ifdef USE_DISP_8PIN_CON
	LCD_CS2_L; 
#else
	LCD_CS1_H;
#endif
	delay();
	
	// 7-Seg 디스플레이
	for (j = 0; j < LCD_PAGE_MAX; j++) // Page 0, 1 Diffuse
	{
		cmd = ST7522_PAGE_ADDR_SET | j;
		St7522_WriteCommand(cmd); // page 0
		// addr 0x00 set
		St7522_WriteCommand(ST7522_ADDR_SET_LOW); // Low column set
		St7522_WriteCommand(ST7522_ADDR_SET_HIGH); // High column set
		addr = j * LCD_SEG_AREA_SIZE; // 현재 사용중인 세그먼트 영역 1 page size = 24
		for (i = addr; i < (addr + LCD_SEG_AREA_SIZE); i++)
		{
			St7522_WriteData(DSP_Memory[DspStruct.Page][i]);
//			delay();
		}
	}
#ifdef USE_DISP_8PIN_CON
	LCD_CS2_H;
#endif

	// IC1 Select
	LCD_CS1_L;
	delay();
	
	// Graphic LCD
	// pin map이 뒤집어져 있기 때문에 page1에 있는 데이터를 
	cmd = ST7522_PAGE_ADDR_SET | 0x01;
	St7522_WriteCommand(cmd); // page 0
	St7522_WriteCommand(ST7522_ADDR_SET_LOW); // Low column set
	St7522_WriteCommand(ST7522_ADDR_SET_HIGH); // High column set  // seg8
	addr = 0;

	for (i = 0; i < (LCD_CHAR_AREA_SIZE); i++)
	{
		data = Rev_Data(DspBuf[DspStruct.Page][addr+(i*2)]);
		St7522_WriteData(data);
	}

	cmd = ST7522_PAGE_ADDR_SET;
	St7522_WriteCommand(cmd); // page 0
	St7522_WriteCommand(ST7522_ADDR_SET_LOW); // Low column set
	St7522_WriteCommand(ST7522_ADDR_SET_HIGH); // High column set  // seg8
	addr = 0;

	for (i = 0; i < (LCD_CHAR_AREA_SIZE); i++)
	{
		data = Rev_Data(DspBuf[DspStruct.Page][addr+(i*2)+1]);
		St7522_WriteData(data);
	}
#ifdef USE_DISP_8PIN_CON
	LCD_CS1_H;
#endif
}

// pin map이 버퍼와 반대로 되어있어 데이터 반전해주는 함수
INT8U Rev_Data(INT8U data)
{
	INT8U fData, rData;
	int i;
	
	rData = 0;
	
	for (i = 0; i < 8; i++)
	{
		fData = (data >> i) & 0x01;
		rData |= fData << (7 - i);
	}
	return rData;
}

// 그래픽 LCD만 출력
// 핀 맵과 DSP_Memory[] 버퍼는
void DSP_LCD7_char_diffuse(void)
{
	int i , addr;
	INT8U cmd, data;

	LCD_CS1_L;
	
	// 상하반전으로 page1이 위에 출력되어야 한다.
	cmd = ST7522_PAGE_ADDR_SET | 0x01; // page 1
	St7522_WriteCommand(cmd);
	St7522_WriteCommand(ST7522_ADDR_SET_LOW); // Low column set
	St7522_WriteCommand(ST7522_ADDR_SET_HIGH); // High column set
	addr = 0;

	for (i = 0; i < (LCD_CHAR_AREA_SIZE); i++)
	{
		data = Rev_Data(DspBuf[DspStruct.Page][addr+(i*2)]);
		St7522_WriteData(data);
	}
	
	cmd = ST7522_PAGE_ADDR_SET; // page 0
	St7522_WriteCommand(cmd);
	St7522_WriteCommand(ST7522_ADDR_SET_LOW); // Low column set
	St7522_WriteCommand(ST7522_ADDR_SET_HIGH); // High column set
	addr = 0;

	for (i = 0; i < (LCD_CHAR_AREA_SIZE); i++)
	{
		data = Rev_Data(DspBuf[DspStruct.Page][addr+(i*2)+1]);
		St7522_WriteData(data);
	}
	
#ifdef USE_DISP_8PIN_CON
	LCD_CS1_H;
#endif
}

void VFD7_display_bright(INT8U bright)//1~9 level
{
	ContrastLevel = bright + 0x10;
	VFD7_Reset(3);
}

// 업체에서 준 코드와 contrast를 제외하고 동일
void VFD7_Init(void)
{
	int i;
	INT8U cc;
	
	cc = get_global_bparam(GLOBAL_DISPLAY_BRIGHT);
	if ((cc < 1) || (cc > 9))
	{
		cc = 5;
		set_global_bparam(GLOBAL_DISPLAY_BRIGHT, cc);
	}
	ContrastLevel = cc + 0x10;
	
	LCD_CS1_H;
	LCD_CS2_H;
	LCD_WR_H;
	delay();
	
	// IC1, IC2 Initial
	LCD_CS1_L;
	LCD_CS2_L;
	
	St7522_WriteCommand(ST7522_RESET); // Reset
//	St7522_WriteCommand(0xF1); // OSC frequency set
//	St7522_WriteCommand(0x0F); // Frame about 80.6Hz/OSC frequency about 2.6KHz
//	St7522_WriteCommand(0xF8); // Follower input voltage set
//	St7522_WriteCommand(0x01); // V5 input voltage=3/6*VSS
	St7522_WriteCommand(0xF8); // Follower input voltage set
	St7522_WriteCommand(0x02); // V5 input voltage=2/6*VSS
	St7522_WriteCommand(ST7522_ADC_SELECT | 0x00); // 0: Normal, 1: Reverse
	St7522_WriteCommand(0xA4);
	St7522_WriteCommand(ST7522_DUTY_SELECT | 0x00); // 0: 1/17 duty, 1: 1/33 duty
	St7522_WriteCommand(ST7522_BIAS_SET | 0x01); // 0: 1/6 bias, 1: 1/5 bias
	St7522_WriteCommand(ST7522_AMPLIFIED_RATIO | 0x04); // V5 amplified ratio = 4
//	St7522_WriteCommand(0xF0); // Booster input voltage set
//	St7522_WriteCommand(0x80); // VSS2=3/5 *VSS
	St7522_WriteCommand(ST7522_CONTRAST_SET); // Contrast set command
	St7522_WriteCommand(ContrastLevel); // Contrast set, Default 35H // setting deault 0x15
	St7522_WriteCommand(ST7522_POWER_CONTROL | 0x01); // power control
//	St7522_WriteCommand(0xaf);
	
	// 200ms
	for (i = 0; i < 350000; i++)
	{
		delay();
	}
//	St7522_clear();
	St7522_WriteCommand(ST7522_DISPLAY_ON); // display on
	LCD_CS1_H;
	LCD_CS2_H;
	
	
#ifndef USE_DISP_8PIN_CON
	// IC2 Initial
	
	St7522_WriteCommand(ST7522_RESET); // Reset
//	St7522_WriteCommand(0xF1); // OSC frequency set
//	St7522_WriteCommand(0x0F); // Frame about 80.6Hz/OSC frequency about 2.6KHz
//	St7522_WriteCommand(0xF8); // Follower input voltage set
//	St7522_WriteCommand(0x01); // V5 input voltage=3/6*VSS
	St7522_WriteCommand(ST7522_ADC_SELECT | 0x00); // 0: Normal, 1: Reverse
	St7522_WriteCommand(0xA4);
	St7522_WriteCommand(ST7522_DUTY_SELECT | 0x00); // 0: 1/17 duty, 1: 1/33 duty
	St7522_WriteCommand(ST7522_BIAS_SET | 0x01); // 0: 1/6 bias, 1: 1/5 bias
	St7522_WriteCommand(ST7522_AMPLIFIED_RATIO | 0x05); // V5 amplified ratio = 4.5
//	St7522_WriteCommand(0xF0); // Booster input voltage set
//	St7522_WriteCommand(0x80); // VSS2=3/5 *VSS
	St7522_WriteCommand(ST7522_CONTRAST_SET); // Contrast set command
	St7522_WriteCommand(0x10); // Contrast set, Default 35H
	St7522_WriteCommand(ST7522_POWER_CONTROL | 0x05); // power control
//	St7522_WriteCommand(0xaf);
	
	// 200ms
	for (i = 0; i < 400000; i++)
	{
		delay();
	}
	St7522_clear();
	St7522_WriteCommand(ST7522_DISPLAY_ON); // display on
#endif

//	memset(DSP_Memory, 0x00, sizeof(DSP_Memory));
	DSP_LCD7_diffuse();
}

void VFD7_Reset(INT8U flag)
{
	int i;
	
	LCD_CS1_H;
	LCD_CS2_H;
	LCD_WR_H;
	delay();
	
	// IC1, IC2 Initial
	LCD_CS1_L;
	LCD_CS2_L;
	
	St7522_WriteCommand(ST7522_RESET); // Reset
//	St7522_WriteCommand(0xF1); // OSC frequency set
//	St7522_WriteCommand(0x0F); // Frame about 80.6Hz/OSC frequency about 2.6KHz
//	St7522_WriteCommand(0xF8); // Follower input voltage set
//	St7522_WriteCommand(0x01); // V5 input voltage=3/6*VSS
	St7522_WriteCommand(0xF8); // Follower input voltage set
	St7522_WriteCommand(0x02); // V5 input voltage=2/6*VSS
	St7522_WriteCommand(ST7522_ADC_SELECT | 0x00); // 0: Normal, 1: Reverse
	St7522_WriteCommand(0xA4);
	St7522_WriteCommand(ST7522_DUTY_SELECT | 0x00); // 0: 1/17 duty, 1: 1/33 duty
	St7522_WriteCommand(ST7522_BIAS_SET | 0x01); // 0: 1/6 bias, 1: 1/5 bias
	St7522_WriteCommand(ST7522_AMPLIFIED_RATIO | 0x04); // V5 amplified ratio = 4
//	St7522_WriteCommand(0xF0); // Booster input voltage set
//	St7522_WriteCommand(0x80); // VSS2=3/5 *VSS
	St7522_WriteCommand(ST7522_CONTRAST_SET); // Contrast set command
	St7522_WriteCommand(ContrastLevel); // Contrast set, Default 0x15
	St7522_WriteCommand(ST7522_POWER_CONTROL | 0x01); // power control
//	St7522_WriteCommand(0xaf);
	
	// 200ms
	switch(flag)
	{
		case 1: // 0ms
			break;
		case 2: // 25ms
			for (i = 0; i < 43750; i++)
			{
				delay();
			}
			break;
		case 3: // 200ms
			for (i = 0; i < 350000; i++)
			{
				delay();
			}
			break;
		default:
			break;
	}
//	St7522_clear();
	St7522_WriteCommand(ST7522_DISPLAY_ON); // display on
	LCD_CS1_H;
	LCD_CS2_H;
	
	DSP_LCD7_diffuse();
}

extern HUGEDATA INT8U *DspFontAddrHpf;

// Seg 영역 버퍼에 데이터 넣어주는 함수
void VFD7_display_char(INT8U nDigitPos, INT8U cData)
{
	INT8U i;
	INT8U fData, oData, hData, lData;
	INT8U digitPos;
	INT8U mask;
	POINT dPos;
	
//	DspStruct.Page = 0;
	
	if (nDigitPos < DSP_MAX_DIGIT)
	{
		// Offset 계산부분 생략
		// nDigitPos 0부터 들어온다.
		digitPos = nDigitPos;
		
		// 데이터 분류
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
			if (digitPos != 4 && digitPos != 10 && digitPos != 17 && digitPos != 25) // dot,comma가 없는 영역
			{
				dPos = DSP_Memory_DotPos[digitPos];
				fData = 0x01 << dPos.y;
				DSP_Memory[DspStruct.Page][dPos.x] |= fData;
			}
		}
		else if (cData == ',')
		{
			if (digitPos != 4 && digitPos != 10 && digitPos != 17 && digitPos != 25) // dot,comma가 없는 영역
			{
				dPos = DSP_Memory_ComPos[digitPos];
				fData = 0x01 << dPos.y;
				DSP_Memory[DspStruct.Page][dPos.x] |= fData;
			}
		}
		else
		{
			// digitPos를 메모리상 주소로 변환
			dPos = DSP_Memory_SegPos[digitPos];
			mask = 0x0F << dPos.y;
			hData = fData >> 4; // 데이터 2byte로 나누기
			lData = fData & 0x0F;

			// Segment 문자 출력
			oData = DSP_Memory[DspStruct.Page][dPos.x] & ~(mask); // 바꿔주는 부분 이외 데이터 보호
			DSP_Memory[DspStruct.Page][dPos.x] = (hData << dPos.y) | oData;
			oData = DSP_Memory[DspStruct.Page][dPos.x + 1] & ~(mask);
			DSP_Memory[DspStruct.Page][dPos.x+1] = (lData << dPos.y) | oData;
			if (dPos.y > 4) // page address가 넘어가는 경우
			{
				mask = 0xFF << (dPos.y - 4);
				oData = DSP_Memory[DspStruct.Page][dPos.x + LCD_SEG_AREA_SIZE] & mask;
				DSP_Memory[DspStruct.Page][dPos.x + LCD_SEG_AREA_SIZE] = (hData >> 3) | oData;
				oData = DSP_Memory[DspStruct.Page][dPos.x + LCD_SEG_AREA_SIZE + 1] & mask;
//				cData = lData >> 3;
				DSP_Memory[DspStruct.Page][dPos.x + LCD_SEG_AREA_SIZE + 1] = (lData >> 3) | oData;
			}
			
			if (digitPos != 4 && digitPos != 10 && digitPos != 17 && digitPos != 25) // dot,comma가 없는 영역
			{
				// dot clear
				dPos = DSP_Memory_DotPos[digitPos];
				fData = 0x01 << dPos.y;
				DSP_Memory[DspStruct.Page][dPos.x] &= ~fData;
				// comma clear
				dPos = DSP_Memory_ComPos[digitPos];
				fData = 0x01 << dPos.y;
				DSP_Memory[DspStruct.Page][dPos.x] &= ~fData;
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
		charPos = x * 2;
		
		// 16x8 font
		font_addr = ((INT8U)cData)&0x00ff;
		font_addr *= 2;
		font_addr *= 8;
		font_addr += 64;
		for (i = 0; i < LCD_CHAR_FONT_WIDTH * 2; i++)
		{
			DspBuf[DspStruct.Page][charPos] = DspFontAddrHpf[font_addr + i];
			charPos++;
		}
	}
}

void VFD7_display_triangle(INT8U nDigitPos, INT8U onoff)
{
	INT8U pos;
	INT8U seg;
	INT8U subSignFlag = 0;
	pos = signTable[nDigitPos].dispPos;
	seg = signTable[nDigitPos].bitPos;
	
#ifdef USE_PB_BATTERY
	if(nDigitPos > SIGN_BAT3) return;
#else
	if(nDigitPos >= LCD_SIGN_MAX) return;
#endif
	
	if (onoff) DSP_Memory[DspStruct.Page][pos] |= seg;
	else DSP_Memory[DspStruct.Page][pos] &= ~seg;
	
#ifdef USE_PB_BATTERY
	else if(nDigitPos == SIGN_BAT1){		
		if(onoff) DSP_Memory[DspStruct.Page][0] |= 0x02;    // TEST ONLY: BAT1 Signal
		else  DSP_Memory[DspStruct.Page][0] &= 0xFD;   //			
	}
	else if(nDigitPos == SIGN_BAT2){		
		if(onoff) DSP_Memory[DspStruct.Page][1] |= 0x02;    // TEST ONLY: BAT2 Signal
		else  DSP_Memory[DspStruct.Page][1] &= 0xFD;   //			
	}
	else if(nDigitPos == SIGN_BAT3){		
		if(onoff) DSP_Memory[DspStruct.Page][2] |= 0x02;    // TEST ONLY: BAT3 Signal
		else  DSP_Memory[DspStruct.Page][2] &= 0xFD;   //			
	}
#endif
	
	// SubSign 표시(트라이앵글 아래 꺽쇠)
	if ((nDigitPos == 3) || (nDigitPos == 4) || (nDigitPos == 8) || (nDigitPos == 29) || (nDigitPos == 12) || (nDigitPos == 13) || (nDigitPos == 15)) // Z2~Z8
	{
		pos += 1;
		subSignFlag = 1;
	}
	else if ((nDigitPos == 2) || (nDigitPos == 18)) // Z1, Z9
	{
		seg = 0x80;
		subSignFlag = 1;
	}
	
	if	(subSignFlag = 1)
	{
		if (onoff) DSP_Memory[DspStruct.Page][pos] |= seg;
		else DSP_Memory[DspStruct.Page][pos] &= ~seg;
	}
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

void VFD7_Diffuse(void)
{
	DSP_LCD7_diffuse();	
}

void VFD7_Fill_Buffer(unsigned char x)
{
	memset(DSP_Memory[DspStruct.Page], x, LCD_SEG_AREA_SIZE * LCD_PAGE_MAX);
}

void VFD7_Fill_Buffer_CHAR(unsigned char x)
{
	memset(DspBuf[DspStruct.Page], x, LCD_CHAR_AREA_SIZE * LCD_PAGE_MAX);
}

void VFD7_save_restore(INT8U x)
{
	if (x) {	// backup
		memcpy(DSP_Memory_backup, DSP_Memory[DspStruct.Page], sizeof(DSP_Memory[DspStruct.Page]));
	} else {	// restore
		memcpy(DSP_Memory[DspStruct.Page], DSP_Memory_backup, sizeof(DSP_Memory[DspStruct.Page]));
	}
}

#endif