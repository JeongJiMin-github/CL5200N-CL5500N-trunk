 /* 
 * File:   KOR_TOPMART_Y.h
 * Author: JiMin JEONG
 * @brief  globals.h에서 분리된 서원유통  TOP mart 전용 파일
 * Create 2024/06/04
 */

#ifndef KOR_TOPMART_Y_H
#define	KOR_TOPMART_Y_H

//----------   국가 Default  ----------//
 #define DEALER_VER_STRING	"(Y)"
 #define COUNTRY_STR "  "
 #define USE_TOPMART_DEFAULT
 #define USE_TRACE_MEATGRADE_STANDARD_DEFAULT
//----------   국가 전용 기능  ----------//
 #define USE_TOPMART_STRUCTURE
 #define USE_TOPMART_IMPORTLOTNO
 #define USE_TRACE_1000QTY
 #define USE_GSMART_TRACE_FUNCTION	        // GS(축산) 전용 기능
 #define USE_TRACE_STANDALONE   	        // 축산 이력기능
 #define USE_GSMART_PLU         	        // Use GSMart PLU Structure(Kor)
 #define PLU_COMMODITY_LINE_NO 8	        // 상품명8단 (GS Mart PLU)
 #define USE_NUTRITION_FACT		            // Use Nutrition Fact
 #define USE_MULTI_TRACE_NO_TABLE	        // Use Multi Trace Number Table with saving
 #define USE_TRACE_MEATGRADE		        // 국내 등급표시(2011.7.1시행)
 #define USE_CHECK_OUTOFPAPER_ON_LABEL	    // Label모드에서 Paper End 체크하는 기능. 즉시 멈춤
 #define USE_TOPMART_ASK_INDIV_INFO         //개제정보 요청 기능
 #define USE_MARBLING_GRADE_PRINT
 #define USE_TOPMART_BARCODE_FUNCTION 
 #undef USE_CTS_FUNCTION			        // 마트는 CTS 사용 안함
//----------   국가 전용 라벨 설정  ----------//
 #define USE_KOR_LABELFORMAT		        // 국내 라벨포맷 사용(No.32~45 사용)
//----------   국가 전용 디스플레이 설정  ----------//
 #define FONT_CODEPAGE CODEPAGE_0949_KOR
 #define USE_KOR_FONT
//----------   국가 전용 바코드 설정  ----------//
 #define USE_GSMART_BARCODE			        // GS 전용 바코드
/*		 GS1 바코드 설정 (두 바코드 중복 적용 안됨)           */
//  #define USE_RSS_BARCODE
 #define USE_ONLY_RSS_EXPAND_BARCODE
/* 바코드 데이터 길이 (길이 증가를 원하지 않으면 모두 주석처리) */
//  #define USE_BARCODE_LEN_50
//  #define USE_BARCODE_LEN_60
//  #define USE_BARCODE_LEN_70
//  #define USE_BARCODE_LEN_90
/////////////////////////////////////////////////////////////
 #define USE_BARCODE_LEN_50_DES_20	        // Barcode length 60, descript length 20
//----------   국가 전용 특정업장 설정  ----------//

#endif  /* KOR_TOPMART_Y_H */
