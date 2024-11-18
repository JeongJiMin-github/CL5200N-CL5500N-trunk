/*****************************************************************************
 |*		       
 |*  Copyright		:	(c) 2003 CAS
 |*  Filename		:	flash.c
 |*  Version		:	0.1
 |*  Programmer(s)	:	Hong Yi-Phyo (HYP)
 |*  Created		:	2003/12/04
 |*  Description		:	CLP Display basic 6x8 font(for lcd 202x32)    
 |*				
 ****************************************************************************/

#include "globals.h"

/****************************************************/
// Don't modify this file 
// This data is reside in Boot Area
/****************************************************/

/****************************************************/
// !! 폰트 배열 크기 계산 방식 !!
// [ Char # * Font BMP Width(byte) * Font BMP Height(bits) + Font Bitmap Offset ]
// 각각의 정보는 폰트 매니저 프로르그램(FontManager.exe)에서 해당 폰트를 Open 후
// Save as 항목 실행 시 확인 할 수 있음
//
//
// 배열 크기 계산 예시
//
// 		ROMDATA INT8U Lcd_4_12x24_1252_Fixedwidth_Normal[800*3*12+64] = {
// 		#include "1256-arab/4-12x24_L1256F_Normal.hpf"
//
// 	**  해당 파일을 폰트 매니저 프로그램을 통해서 각각의 정보를 확인 할 수 있음.
//		Font Bitmap Offset = 64		/		Font BMP Widht(byte) = 3
//		Font BMP Height(bits) = 12	/		Char # = 800
/****************************************************/

#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
ROMDATA INT8U Dsp_Arial_12x8[224*16+64] = {
#include "8x12_Lcd_1252_Fixedwidth.hpf"
};
#if defined(USE_ARAB_FONT) || defined(HEBREW_FONT)    //20120914 for CL5200 Arab version by kiho
  #ifdef USE_PRICE_TOTAL_8DIGIT
ROMDATA INT8U Dsp_Arial_16x8[224*16+64] = { 
#include "8x16_Lcd_1252_Fixedwidth_IRAN.hpf"
};
  #else
ROMDATA INT8U Dsp_Arial_16x8[224*16+64] = { 
#include "8x16_Lcd_1252_Fixedwidth.hpf"
};
  #endif
#else
ROMDATA INT8U Dsp_Arial_20x8[224*24+64] = {
#include "8x20_Lcd_1252_Fixedwidth.hpf"
};
#endif
#endif

#if (FONT_CODEPAGE == CODEPAGE_1254)
ROMDATA INT8U Prt_3_8x16_1252_Fixedwidth_Normal[224*16+64] = {
#include "1254-turkey/3-8x16_Prt_1254_Fixedwidth_Normal.hpf"
};
ROMDATA INT8U Prt_4_12x24_1252_Fixedwidth_Normal[224*48+64] = {
#include "1254-turkey/4-12x24_Prt_1254_Fixedwidth_Normal.hpf"
};
ROMDATA INT8U Prt_5_6x12_1252_Fixedwidth_Normal[224*12+64] = {
#include "1254-turkey/5-6x12_Prt_1254_Fixedwidth_Normal.hpf"
};
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
ROMDATA INT8U Dsp_Arial_8x6[256*6+64] = {
#include "1254-turkey/2-6x8_Lcd_1254_Fixedwidth_Normal.hpf"
};
ROMDATA INT8U Lcd_3_8x16_1252_Fixedwidth_Normal[256*16+64] = {
#include "1254-turkey/3-8x16_Lcd_1254_Fixedwidth_Normal.hpf"
};
ROMDATA INT8U Lcd_4_12x24_1252_Fixedwidth_Normal[192*48+64] = {
#include "1254-turkey/4-12x24_Lcd_1254_Fixedwidth_Normal.hpf"
};
#elif defined(_USE_LCD37D_40DOT_)
ROMDATA INT8U Dsp_Arial_8x6[256*6+64] = {
#include "1254-turkey/2-6x8_Lcd_1254_Fixedwidth_Normal.hpf"
};
#endif

#elif (FONT_CODEPAGE == CODEPAGE_1256)
ROMDATA INT8U Prt_3_8x16_1252_Fixedwidth_Normal[12352] = { //배열크기: hpf파일 Open시 데이터(1byte)의 갯수
#include "1256-arab/3-8x16_P1256F_Normal.hpf"
};
ROMDATA INT8U Prt_4_12x24_1252_Fixedwidth_Normal[36928] = { //배열크기: hpf파일 Open시 데이터(1byte)의 갯수
#include "1256-arab/4-12x24_P1256F_Normal.hpf"
};
ROMDATA INT8U Prt_5_6x12_1252_Fixedwidth_Normal[224*12+64] = {
#include "1252-eng/5-6x12_Prt_1252_Fixedwidth_Normal.hpf"
};
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
ROMDATA INT8U Dsp_Arial_8x6[800*8+64] = {
#include "1256-arab/2-8x8_L1256F_Normal.hpf"
};
ROMDATA INT8U Lcd_3_8x16_1252_Fixedwidth_Normal[800*16+64] = {
#include "1256-arab/3-8x16_L1256F_Normal.hpf"
};
  #ifdef _USE_LCD20848_					// [ CL5200(N) DISP ]
ROMDATA INT8U Lcd_4_12x24_1252_Fixedwidth_Normal[600*48+64] = {
#include "1256-arab/4-12x20_L1256F_Normal.hpf"
};
  #elif defined(_USE_LCD32_)			// [ CL5500(N) DISP ]
ROMDATA INT8U Lcd_4_12x24_1252_Fixedwidth_Normal[800*3*12+64] = {
#include "1256-arab/4-12x24_L1256F_Normal.hpf"
};
  #endif
ROMDATA INT8U Dsp_SYSTEM_ENG_8x6[256*6+64] = {
#include "6x8_Lcd_1252_Fixedwidth_Normal.hpf"	
};
#elif defined(_USE_LCD37D_40DOT_)
ROMDATA INT8U Dsp_Arial_8x6[256*6+64] = {
#include "6x8_Lcd_1252_Fixedwidth_Normal.hpf"
};
#endif

#elif (FONT_CODEPAGE == CODEPAGE_1256_IRN)
// Print Font
ROMDATA INT8U Prt_3_8x16_1252_Fixedwidth_Normal[12352] = { //배열크기: hpf파일 Open시 데이터(1byte)의 갯수
#include "1256-IRN/3-8x16_P1256F_Farsi_Font.hpf"
};
ROMDATA INT8U Prt_4_12x24_1252_Fixedwidth_Normal[36928] = { //배열크기: hpf파일 Open시 데이터(1byte)의 갯수
#include "1256-IRN/4-12x24_P1256F_Farsi_Font.hpf"
};
ROMDATA INT8U Prt_5_6x12_1252_Fixedwidth_Normal[224*12+64] = {
#include "1252-eng/5-6x12_Prt_1252_Fixedwidth_Normal.hpf"
};
ROMDATA INT8U Prt_6_12x16_1252_Fixedwidth_Normal[24640] = {
#include "1256-IRN/6-12x16_P1256F_Farsi_Font.hpf"
};
// Dsp font
#ifdef _USE_LCD20848_ // Disp CL5200N
ROMDATA INT8U Dsp_Arial_8x6[800*8+64] = {
#include "1256-IRN/2-8x8_L1256F_Farsi_Font.hpf"
};
ROMDATA INT8U Lcd_3_8x16_1252_Fixedwidth_Normal[800*16+64] = {
#include "1256-IRN/3-8x16_L1256F_Farsi_Font.hpf"
};
ROMDATA INT8U Lcd_4_12x24_1252_Fixedwidth_Normal[28864] = {
#include "1256-IRN/4-12x20_L1256F_Farsi_Font.hpf"
};
ROMDATA INT8U Dsp_SYSTEM_ENG_8x6[256*6+64] = {
#include "6x8_Lcd_1252_Fixedwidth_Normal.hpf"	
};
#elif defined(_USE_LCD37D_40DOT_) // Disp for CL5200J
ROMDATA INT8U Dsp_Arial_8x6[256*6+64] = {
#include "6x8_Lcd_1252_Fixedwidth_Normal.hpf"
};
#endif

#elif (FONT_CODEPAGE == CODEPAGE_1251)

ROMDATA INT8U Prt_3_8x16_1252_Fixedwidth_Normal[224*16+64] = {
#include "1251-RUS/3-8x16_P1251F_Normal.hpf"
};
ROMDATA INT8U Prt_4_12x24_1252_Fixedwidth_Normal[224*48+64] = {
#include "1251-RUS/4-12x24_P1251F_Normal.hpf"
};
ROMDATA INT8U Prt_5_6x12_1252_Fixedwidth_Normal[224*12+64] = {
#include "1251-RUS/5-6x12_P1251F_Normal(renew).hpf"
};
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
ROMDATA INT8U Dsp_Arial_8x6[256*6+64] = {
#include "1251-RUS/2-6x8_L1251F_Normal.hpf"
};
ROMDATA INT8U Lcd_3_8x16_1252_Fixedwidth_Normal[256*16+64] = {
#include "1251-RUS/3-8x16_L1251F_Normal.hpf"
};
ROMDATA INT8U Lcd_4_12x24_1252_Fixedwidth_Normal[192*48+64] = {
#include "1251-RUS/4-12x24_L1251F_Normal.hpf"
};
#elif defined(_USE_LCD37D_40DOT_)
ROMDATA INT8U Dsp_Arial_8x6[256*6+64] = {
#include "1251-RUS/2-6x8_L1251F_Normal.hpf"
};
#endif

#elif (FONT_CODEPAGE == CODEPAGE_1252_ARM)

ROMDATA INT8U Prt_3_8x16_1252_Fixedwidth_Normal[224*16+64] = {
#include "1252-ARM/3-8x16_P1252mF_Arm.hpf"
};
ROMDATA INT8U Prt_4_12x24_1252_Fixedwidth_Normal[224*48+64] = {
#include "1252-ARM/4-12x24_P1252mF_Arm.hpf"
};
ROMDATA INT8U Prt_5_6x12_1252_Fixedwidth_Normal[224*12+64] = {
#include "1252-ARM/5-6x12_P1252F_Normal.hpf"
};
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
ROMDATA INT8U Dsp_Arial_8x6[256*6+64] = {
#include "1252-ARM/2-6x8_L1252mF_Arm.hpf"
};
ROMDATA INT8U Lcd_3_8x16_1252_Fixedwidth_Normal[256*16+64] = {
#include "1252-ARM/3-8x16_L1252mF_Arm.hpf"
};
ROMDATA INT8U Lcd_4_12x24_1252_Fixedwidth_Normal[192*48+64] = {
#include "1252-ARM/4-12x24_L1252mF_Arm.hpf"
};
#elif defined(_USE_LCD37D_40DOT_)
ROMDATA INT8U Dsp_Arial_8x6[256*6+64] = {
#include "1252-ARM/2-6x8_L1252mF_Arm.hpf"
};
#endif

#elif (FONT_CODEPAGE == CODEPAGE_1251_GEO)

ROMDATA INT8U Prt_3_8x16_1252_Fixedwidth_Normal[224*16+64] = {
#include "1251-GEO/3-8x16_P1251mF_GEO.hpf"
};
ROMDATA INT8U Prt_4_12x24_1252_Fixedwidth_Normal[224*48+64] = {
#include "1251-GEO/4-12x24_P1251F_GEO.hpf"
};
ROMDATA INT8U Prt_5_6x12_1252_Fixedwidth_Normal[224*12+64] = {
#include "1251-GEO/5-6x12_P1251F_GEO.hpf"
};
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
ROMDATA INT8U Dsp_Arial_8x6[256*6+64] = {
#include "1251-GEO/2-6x8_L1251mF_GEO.hpf"
};
ROMDATA INT8U Lcd_3_8x16_1252_Fixedwidth_Normal[256*16+64] = {
#include "1251-GEO/3-8x16_L1251mF_GEO(B).hpf"
};
ROMDATA INT8U Lcd_4_12x24_1252_Fixedwidth_Normal[192*48+64] = {
#include "1251-GEO/4-12x24_L1251F_GEO(B).hpf"
};
#elif defined(_USE_LCD37D_40DOT_)
ROMDATA INT8U Dsp_Arial_8x6[256*6+64] = {
#include "1251-GEO/2-6x8_L1251mF_GEO.hpf"
};
#endif

#elif ((FONT_CODEPAGE == CODEPAGE_1250_POL) || (FONT_CODEPAGE == CODEPAGE_1250))

ROMDATA INT8U Prt_3_8x16_1252_Fixedwidth_Normal[224*16+64] = {
#include "1250-POL/3-8x16_P1250F_Normal_EU.hpf"
};
ROMDATA INT8U Prt_4_12x24_1252_Fixedwidth_Normal[224*48+64] = {
#include "1250-POL/4-12x24_P1250F_Normal.hpf"
};
ROMDATA INT8U Prt_5_6x12_1252_Fixedwidth_Normal[224*12+64] = {
#include "1250-POL/5-6x12_P1250F_Normal_EU.hpf"
};
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
ROMDATA INT8U Dsp_Arial_8x6[256*6+64] = {
#include "1250-POL/2-6x8_L1250F_Normal.hpf"
};
ROMDATA INT8U Lcd_3_8x16_1252_Fixedwidth_Normal[256*16+64] = {
#include "1250-POL/3-8x16_L1250F_Normal.hpf"
};
ROMDATA INT8U Lcd_4_12x24_1252_Fixedwidth_Normal[192*48+64] = {
#include "1250-POL/4-12x24_L1250F_Normal.hpf"
};
#elif defined(_USE_LCD37D_40DOT_)
ROMDATA INT8U Dsp_Arial_8x6[256*6+64] = {
#include "1250-POL/2-6x8_L1250F_Normal.hpf"
};
#endif

#elif (FONT_CODEPAGE == CODEPAGE_0936_CHN)

ROMDATA INT8U Prt_3_8x16_1252_Fixedwidth_Normal[224*16+64] = {
#include "936-chinese/3-8x16_Prt_1252_Fixedwidth_Normal.hpf"
};
ROMDATA INT8U Prt_3_16x16_1252_Fixedwidth_Normal[8352*32+64] = {
#include "936-chinese/3-16x16_Prt_936_SC_LP15.hpf"
};
ROMDATA INT8U Prt_4_12x24_1252_Fixedwidth_Normal[224*48+64] = {
#include "936-chinese/4-12x24_Prt_1252_Fixedwidth_NumBold.hpf"
};
ROMDATA INT8U Prt_5_6x12_1252_Fixedwidth_Normal[224*12+64] = {
#include "936-chinese/5-6x12_Prt_1252_Fixedwidth_Normal.hpf"
};
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
ROMDATA INT8U Dsp_Arial_8x6[256*6+64] = {
#include "6x8_Lcd_1252_Fixedwidth_Normal.hpf"
};
ROMDATA INT8U Dsp_Arial_8x16[256*16+64] = {
#include "936-chinese/3-8x16_Prt_1252_Fixedwidth_Normal.hpf"
};
ROMDATA INT8U Lcd_3_8x16_1252_Fixedwidth_Normal[256*16+64] = {
#include "1252-eng/3-8x16_Lcd_1252_Fixedwidth_Normal.hpf"
};
ROMDATA INT8U Lcd_4_12x24_1252_Fixedwidth_Normal[192*48+64] = {
#include "1252-eng/4-12x24_Lcd_1252_Fixedwidth_Normal.hpf"
};
#endif
ROMDATA INT8U Dsp_Arial_16x16[8352*32+64] = { // 프린터 폰트로 디스플레이
#include "936-chinese/3-16x16_Prt_936_SC_LP15.hpf"
};

#elif (FONT_CODEPAGE == CODEPAGE_0950_CHN)

ROMDATA INT8U Prt_3_8x16_1252_Fixedwidth_Normal[224*16+64] = {
#include "950-chinese/3-8x16_Prt_1252_Fixedwidth_Normal.hpf"
};
ROMDATA INT8U Prt_3_16x16_1252_Fixedwidth_Normal[13920*32+64] = {
#include "950-chinese/3-16x16_Prt_uwb5-16m_short.hpf"
};
ROMDATA INT8U Prt_4_12x24_1252_Fixedwidth_Normal[224*48+64] = {
#include "950-chinese/4-12x24_Prt_1252_Fixedwidth_Normal.hpf"
};
ROMDATA INT8U Prt_5_6x12_1252_Fixedwidth_Normal[224*12+64] = {
#include "950-chinese/5-6x12_Prt_1252_Fixedwidth_Normal.hpf"
};
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
ROMDATA INT8U Dsp_Arial_8x6[256*6+64] = {
#include "6x8_Lcd_1252_Fixedwidth_Normal.hpf"
};
ROMDATA INT8U Dsp_Arial_8x16[256*16+64] = {
#include "950-chinese/3-8x16_Prt_1252_Fixedwidth_Normal.hpf"
};
ROMDATA INT8U Lcd_3_8x16_1252_Fixedwidth_Normal[256*16+64] = {
#include "1252-eng/3-8x16_Lcd_1252_Fixedwidth_Normal.hpf"
};
ROMDATA INT8U Lcd_4_12x24_1252_Fixedwidth_Normal[192*48+64] = {
#include "1252-eng/4-12x24_Lcd_1252_Fixedwidth_Normal.hpf"
};
#endif
ROMDATA INT8U Dsp_Arial_16x16[]={// 프린터 폰트로 디스플레이 사이즈 초과로 Prt_3_16x16_1252_Fixedwidth_Normal 사용
//#include "950-chinese/3-16x16_Prt_uwb5-16m_short.hpf"
};

#elif (FONT_CODEPAGE == CODEPAGE_0949_KOR)

ROMDATA INT8U Prt_3_8x16_1252_Fixedwidth_Normal[224*16+64] = {
#include "949-korea/3-8x16_Prt_1252_Fixedwidth_Normal.hpf"
};
ROMDATA INT8U Prt_3_16x16_1252_Fixedwidth_Normal[3840*32+64] = {
#include "949-korea/16x16_Prt_949_Wansung_Gothic_wSpecial.hpf"
};
  #ifdef USE_FONT_ID4_Arial
ROMDATA INT8U Prt_4_12x24_1252_Fixedwidth_Normal[224*48+64] = {
#include "949-korea/4-12x24_Prt_1252_Fixedwidth_Arial.hpf"
};  
  #else
ROMDATA INT8U Prt_4_12x24_1252_Fixedwidth_Normal[224*48+64] = {
#include "949-korea/4-12x24_Prt_1252_Fixedwidth_NumBold.hpf"
};
  #endif
ROMDATA INT8U Prt_5_6x12_1252_Fixedwidth_Normal[224*12+64] = {
#include "949-korea/5-6x12_Prt_1252_Fixedwidth_Normal.hpf"
};
  #ifdef USE_FONT_ID6_12BY28
ROMDATA INT8U Prt_6_8x24_1252_Fixedwidth_CircleOnly[224*2*28+64] = {	//FID*Height+Offset
#include "949-korea/6-12x28_Prt_1252_Fixedwidth_Arial_Bold.hpf"
};
  #else
ROMDATA INT8U Prt_6_8x24_1252_Fixedwidth_CircleOnly[32*24+64] = { 
#include "949-korea/6-8x24_Prt_1252_Fixedwidth_CircleOnly.hpf"
};
  #endif
  
ROMDATA INT8U Dsp_Arial_8x16[256*16+64] = {
#include "8x16_Lcd_1252_Fixedwidth_Normal.hpf"
};
ROMDATA INT8U Dsp_Arial_16x16[360*32+64] = {
#include "949-korea/3-16x16_Lcd_949_Johab_Iyagi.hpf"
};

#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)  // (CL5200 DISP)  || (CL5500 DISP)
ROMDATA INT8U Dsp_Arial_8x6[256*6+64] = {
#include "6x8_Lcd_1252_Fixedwidth_Normal.hpf"
};
ROMDATA INT8U Lcd_3_8x16_1252_Fixedwidth_Normal[256*16+64] = {
#include "1252-eng/3-8x16_Lcd_1252_Fixedwidth_Normal.hpf"
};
ROMDATA INT8U Lcd_4_12x24_1252_Fixedwidth_Normal[192*48+64] = {
#include "1252-eng/4-12x24_Lcd_1252_Fixedwidth_Normal.hpf"
};
  #ifdef _USE_LCD32_	// CL5500 DISP
ROMDATA INT8U Lcd_4_24x24_1252_Fixedwidth_Normal[720*48+64] = {
#include "949-korea/4-24x24_PrtLcd_949_Johab_Myungjo.hpf"
};
  #endif
#elif defined(_USE_LCD37D_40DOT_)
ROMDATA INT8U Dsp_Arial_8x6[256*6+64] = {
#include "6x8_Lcd_1252_Fixedwidth_Normal.hpf"
};
#endif

#elif (FONT_CODEPAGE == CODEPAGE_1252_IND)

ROMDATA INT8U Prt_3_8x16_1252_Fixedwidth_Normal[224*16+64] = {
#include "1252-IND/3-8x16_Prt_1252_Fixedwidth_Normal_India.hpf"
};
ROMDATA INT8U Prt_4_12x24_1252_Fixedwidth_Normal[224*48+64] = {
#include "1252-IND/4-12x24_Prt_1252_Fixedwidth_Normal_India.hpf"
};
ROMDATA INT8U Prt_5_6x12_1252_Fixedwidth_Normal[224*12+64] = {
#include "1252-IND/5-6x12_Prt_1252_Fixedwidth_Normal_India.hpf"
};
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
ROMDATA INT8U Dsp_Arial_8x6[256*6+64] = {
#include "1252-IND/2-6x8_Lcd_1252_Fixedwidth_Normal_India.hpf"
};
ROMDATA INT8U Lcd_3_8x16_1252_Fixedwidth_Normal[256*16+64] = {
#include "1252-IND/3-8x16_Lcd_1252_Fixedwidth_Normal_India.hpf"
};
ROMDATA INT8U Lcd_4_12x24_1252_Fixedwidth_Normal[192*48+64] = {
#include "1252-IND/4-12x24_Lcd_1252_Fixedwidth_Normal_India.hpf"
};
#elif defined(_USE_LCD37D_40DOT_)
ROMDATA INT8U Dsp_Arial_8x6[256*6+64] = {
#include "1252-IND/2-6x8_Lcd_1252_Fixedwidth_Normal_India.hpf"
};
#endif

#elif (FONT_CODEPAGE == CODEPAGE_1251_MNG)

ROMDATA INT8U Prt_3_8x16_1252_Fixedwidth_Normal[224*16+64] = {
#include "1251-mongolia/3-8x16_P1251F_Normal.hpf"
};
ROMDATA INT8U Prt_4_12x24_1252_Fixedwidth_Normal[224*48+64] = {
#include "1251-mongolia/4-12x24_P1251F_Normal.hpf"
};
ROMDATA INT8U Prt_5_6x12_1252_Fixedwidth_Normal[224*12+64] = {
#include "1251-mongolia/5-6x12_P1251F_Normal.hpf"
};
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
ROMDATA INT8U Dsp_Arial_8x6[256*6+64] = {
#include "1251-mongolia/2-6x8_L1251F_Normal.hpf"
};
ROMDATA INT8U Lcd_3_8x16_1252_Fixedwidth_Normal[256*16+64] = {
#include "1251-mongolia/3-8x16_L1251F_Normal.hpf"
};
ROMDATA INT8U Lcd_4_12x24_1252_Fixedwidth_Normal[192*48+64] = {
#include "1251-mongolia/4-12x24_L1251F_Normal.hpf"
};
#elif defined(_USE_LCD37D_40DOT_)
ROMDATA INT8U Dsp_Arial_8x6[256*6+64] = {
#include "1251-mongolia/2-6x8_L1251F_Normal.hpf"
};
#endif

#elif (FONT_CODEPAGE == CODEPAGE_1253)

ROMDATA INT8U Prt_3_8x16_1252_Fixedwidth_Normal[224*16+64] = {
#include "1253-GRC/3-8x16_Prt_1253_Fixedwidth_Normal.hpf"
};
ROMDATA INT8U Prt_4_12x24_1252_Fixedwidth_Normal[224*48+64] = {
#include "1253-GRC/4-12x24_Prt_1253_Fixedwidth_Normal.hpf"
};
ROMDATA INT8U Prt_5_6x12_1252_Fixedwidth_Normal[224*12+64] = {
#include "1253-GRC/5-6x12_Prt_1253_Fixedwidth_Normal.hpf"
};
#ifdef _USE_LCD37D_40DOT_
ROMDATA INT8U Dsp_Arial_8x6[256*6+64] = {
#include "1253-GRC/2-6x8_Lcd_1253_Fixedwidth_Normal.hpf"
};
#else
ROMDATA INT8U Dsp_Arial_8x6[256*6+64] = {
#include "1253-GRC/2-6x8_Lcd_1253_Fixedwidth_Normal.hpf"
};
ROMDATA INT8U Lcd_3_8x16_1252_Fixedwidth_Normal[256*16+64] = {
#include "1253-GRC/3-8x16_Lcd_1253_Fixedwidth_Normal.hpf"
};
ROMDATA INT8U Lcd_4_12x24_1252_Fixedwidth_Normal[192*48+64] = {
#include "1253-GRC/4-12x24_Lcd_1253_Fixedwidth_Normal.hpf"
};
#endif

#elif (FONT_CODEPAGE == CODEPAGE_1250_HUN)

ROMDATA INT8U Prt_3_8x16_1252_Fixedwidth_Normal[224*16+64] = {
#include "1250-HUN/3-8x16_P1250F_Arial.hpf"
};
ROMDATA INT8U Prt_4_12x24_1252_Fixedwidth_Normal[224*48+64] = {
#include "1250-HUN/4-12x24_P1250F_Arial.hpf"
};
ROMDATA INT8U Prt_5_6x12_1252_Fixedwidth_Normal[224*32+288] = {
#include "1250-HUN/6-16x16_P_vari_Normal_edit.hpf"
};
#ifdef _USE_LCD37D_40DOT_
ROMDATA INT8U Dsp_Arial_8x6[256*6+64] = {
#include "1250-HUN/2-6x8_L1250F_Normal.hpf"
};
#else
ROMDATA INT8U Dsp_Arial_8x6[256*6+64] = {
#include "1250-HUN/2-6x8_L1250F_Normal.hpf"
};
ROMDATA INT8U Lcd_3_8x16_1252_Fixedwidth_Normal[256*16+64] = {
#include "1250-HUN/3-8x16_L1250F_Arial.hpf"
};
ROMDATA INT8U Lcd_4_12x24_1252_Fixedwidth_Normal[192*48+64] = {
#include "1250-HUN/4-12x24_L1250F_Arial.hpf"
};

#endif

#elif (FONT_CODEPAGE == CODEPAGE_1251_KAZ)

ROMDATA INT8U Prt_3_8x16_1252_Fixedwidth_Normal[224*16+64] = {
#include "1251-kazakh/3-8x16_P1251mF_Kazakh.hpf"
};
ROMDATA INT8U Prt_4_12x24_1252_Fixedwidth_Normal[224*48+64] = {
#include "1251-kazakh/4-12x24_P1251mF_Kazakh.hpf"
};
ROMDATA INT8U Prt_5_6x12_1252_Fixedwidth_Normal[224*12+64] = {
#include "1251-kazakh/5-6x12_P1251mF_Kazakh.hpf"
};
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
ROMDATA INT8U Dsp_Arial_8x6[256*6+64] = {
#include "1251-kazakh/2-6x8_L1251mF_Kazakh.hpf"
};
ROMDATA INT8U Lcd_3_8x16_1252_Fixedwidth_Normal[256*16+64] = {
#include "1251-kazakh/3-8x16_L1251mF_Kazakh.hpf"
};
ROMDATA INT8U Lcd_4_12x24_1252_Fixedwidth_Normal[192*48+64] = {
#include "1251-kazakh/4-12x24_L1251mF_Kazakh.hpf"
};
#elif defined(_USE_LCD37D_40DOT_)
ROMDATA INT8U Dsp_Arial_8x6[256*6+64] = {
#include "1251-kazakh/2-6x8_L1251mF_Kazakh.hpf"
};
#endif


#elif (FONT_CODEPAGE == CODEPAGE_1257_BAL)

ROMDATA INT8U Prt_3_8x16_1252_Fixedwidth_Normal[224*16+64] = {
#include "1257-baltic/3-8x16_P1257F_Normal.hpf"
};
ROMDATA INT8U Prt_4_12x24_1252_Fixedwidth_Normal[224*48+64] = {
#include "1257-baltic/4-12x24_P1257F_Normal.hpf"
};
ROMDATA INT8U Prt_5_6x12_1252_Fixedwidth_Normal[224*12+64] = {
#include "1257-baltic/5-6x12_P1257F_Normal.hpf"
};
#ifdef _USE_LCD37D_40DOT_
ROMDATA INT8U Dsp_Arial_8x6[256*6+64] = {
#include "1257-baltic/2-6x8_L1257F_Normal.hpf"
};
#else
ROMDATA INT8U Dsp_Arial_8x6[256*6+64] = {
#include "1257-baltic/2-6x8_L1257F_Normal.hpf"
};
ROMDATA INT8U Lcd_3_8x16_1252_Fixedwidth_Normal[256*16+64] = {
#include "1257-baltic/3-8x16_L1257F_Normal.hpf"
};
ROMDATA INT8U Lcd_4_12x24_1252_Fixedwidth_Normal[192*48+64] = {
#include "1257-baltic/4-12x24_L1257F_Normal.hpf"
};
#endif

#elif (FONT_CODEPAGE == CODEPAGE_1258_VNM)

ROMDATA INT8U Prt_3_8x16_1252_Fixedwidth_Normal[416*16+64] = {
#include "1258-VNM/3-8x16_Prt_1258_Fixedwidth_Normal_new_modify.hpf"
};
ROMDATA INT8U Prt_4_12x24_1252_Fixedwidth_Normal[416*48+64] = {
#include "1258-VNM/4-12x24_Prt_1258_Fixedwidth_Normal_new_modify.hpf"
};
ROMDATA INT8U Prt_5_6x12_1252_Fixedwidth_Normal[312*16+64] = {
#include "1258-VNM/5-6x12_Prt_1258_Fixedwidth_Normal_new_modify.hpf"
};
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
ROMDATA INT8U Dsp_Arial_8x6[168*16+64] = {
#include "1258-VNM/2-6x8_Lcd_1258_Fixedwidth_Normal_new.hpf"
};
ROMDATA INT8U Lcd_3_8x16_1252_Fixedwidth_Normal[448*16+64] = {
#include "1258-VNM/3-8x16_Lcd_1258_Fixedwidth_Normal_new_modify.hpf"
};
ROMDATA INT8U Lcd_4_12x24_1252_Fixedwidth_Normal[336*48+64] = {
#include "1258-VNM/4-12x24_Lcd_1258_Fixedwidth_Normal_new_modify.hpf"
};
#elif defined(_USE_LCD37D_40DOT_)
ROMDATA INT8U Dsp_Arial_8x6[256*6+64] = {
#include "1258-VNM/2-6x8_Lcd_1258_Fixedwidth_Normal_new.hpf"
};
#endif

#elif (FONT_CODEPAGE == CODEPAGE_0874_ENG)
ROMDATA INT8U Prt_3_8x16_1252_Fixedwidth_Normal[416*16+64] = {
#include "874-THA/3-8x16_P874F_Thai_Normal.hpf"
};
ROMDATA INT8U Prt_4_12x24_1252_Fixedwidth_Normal[416*48+64] = {
#include "874-THA/4-12x24_P874F_Thai_Normal.hpf"
};
ROMDATA INT8U Prt_5_6x12_1252_Fixedwidth_Normal[312*16+64] = {
#include "874-THA/5-6x12_Prt_1252_Fixedwidth_Normal.hpf"
};
  #if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
  ROMDATA INT8U Dsp_Arial_8x6[256*6+64] = {
  #include "6x8_Lcd_1252_Fixedwidth_Normal_with_Baht.hpf"
  };
  ROMDATA INT8U Lcd_3_8x16_1252_Fixedwidth_Normal[256*16+64] = {
  #include "874-THA/3-8x16_L874F_Thai_Normal.hpf"
  };
  ROMDATA INT8U Lcd_4_12x24_1252_Fixedwidth_Normal[192*48+64] = {
  #include "874-THA/4-12x24_L874F_Thai_Normal.hpf"
  };
  #elif defined(_USE_LCD37D_40DOT_)
  ROMDATA INT8U Dsp_Arial_8x6[256*6+64] = {
  #include "6x8_Lcd_1252_Fixedwidth_Normal.hpf"
  };
  #endif
#else //#elif (FONT_CODEPAGE == CODEPAGE_1252)

#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
ROMDATA INT8U Dsp_Arial_8x6[256*6+64] = {
#include "6x8_Lcd_1252_Fixedwidth_Normal.hpf"
};
ROMDATA INT8U Lcd_3_8x16_1252_Fixedwidth_Normal[256*16+64] = {
#include "1252-eng/3-8x16_Lcd_1252_Fixedwidth_Normal.hpf"
};
ROMDATA INT8U Lcd_4_12x24_1252_Fixedwidth_Normal[192*48+64] = {
#include "1252-eng/4-12x24_Lcd_1252_Fixedwidth_Normal.hpf"
};
#elif defined(_USE_LCD37D_40DOT_)
ROMDATA INT8U Dsp_Arial_8x6[256*6+64] = {
#include "6x8_Lcd_1252_Fixedwidth_Normal.hpf"
};
#endif
ROMDATA INT8U Prt_3_8x16_1252_Fixedwidth_Normal[224*16+64] = {
#include "1252-eng/3-8x16_Prt_1252_Fixedwidth_Normal.hpf"
};
ROMDATA INT8U Prt_4_12x24_1252_Fixedwidth_Normal[224*48+64] = {
#include "1252-eng/4-12x24_Prt_1252_Fixedwidth_Normal.hpf"
};
ROMDATA INT8U Prt_5_6x12_1252_Fixedwidth_Normal[224*12+64] = {
#include "1252-eng/5-6x12_Prt_1252_Fixedwidth_Normal.hpf"
};
#endif

ROMDATA INT8U Prt_FontAddrOcrB[10*32*2*3] = { 
#include "16x16OCRB.fnt"
};

ROMDATA INT8U Prt_FontAddrOcrBW[30] = { 
#include "16x16OCRBW.fnw"
};

