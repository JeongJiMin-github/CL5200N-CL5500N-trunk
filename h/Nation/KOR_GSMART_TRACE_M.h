 /* 
 * File:   KOR_GSMART_TRACE_M.h
 * Author: JiMin JEONG
 * @brief  globals.h에서 분리된 한국 GS마트 TRACE 전용 파일
 * Created 2024/06/10
 */

#ifndef KOR_GSMART_TRACE_M_H
#define	KOR_GSMART_TRACE_M_H

//----------   국가 Default  ----------//
 #define DEALER_VER_STRING	"(M)"
 #define COUNTRY_STR "  "
 #define USE_GSMART_TRACE_DEFAULT
 #define USE_TRACE_MEATGRADE_STANDARD_DEFAULT
//----------   국가 전용 기능  ----------//
 #define USE_KOREA_VERSION_DISPLAY	        // KOREA 버전 표시
 #define USE_GSMART_TRACE_FUNCTION	        // GS(축산) 전용 기능 
 #define USE_TRACE_STANDALONE   	        // 축산 이력기능
 #define USE_GSMART_PLU         	        // Use GSMart PLU Structure(Kor)
 #define PLU_COMMODITY_LINE_NO 8	        // 상품명8단 (GS Mart PLU)
 //#define USE_TRACEABILITY       	        // Use European Traceability
 #define USE_NUTRITION_FACT		            // Use Nutrition Fact
 //#define USE_TAX_RATE				        // Use Tax Rate
 //#define USE_SELLBYDATETIME_ZERO	        // Sell by Date, Sell by Time이 0일 때 인쇄 안하는 기능
 //#define USE_EMART_NETWORK      	        // E-Mart Network(실시간 판매 업로드, 매가변경 업로드)
 //#define USE_REPORT_TAX         	        // TAX Report 사용
 //#define USE_REPORT_PAY         	        // PAY/CHANGE Report 사용
 #define USE_MULTI_TRACE_NO_TABLE	        // Use Multi Trace Number Table with  saving
 #define USE_TRACE_MEATGRADE		        // 국내 등급표시(2011.7.1시행)
 #define USE_CHECK_OUTOFPAPER_ON_LABEL	    // Label모드에서 Paper End 체크하는 기능. 즉시 멈춤
 #define USE_GSMART_PIG_TRACE
 #define USE_PRICE_CHANGE_PROHIBITED
 #define USE_KOREAN_ERROR_MSG
 #undef USE_CTS_FUNCTION 		            // 마트는 CTS 사용 안함
//----------   국가 전용 라벨 설정  ----------//
 //#define USE_ENG_LABELFORMAT		        // 영공 라벨포맷 사용
 #define USE_KOR_LABELFORMAT		        // 국내 라벨포맷 사용(No.32~45 사용)
//----------   국가 전용 디스플레이 설정  ----------//
 #define FONT_CODEPAGE CODEPAGE_0949_KOR
 #define USE_KOR_FONT
//----------   국가 전용 바코드 설정  ----------//
 #define USE_GSMART_BARCODE			        // GS 전용 바코드
//----------   국가 전용 특정업장 설정  ----------//

#endif  /* KOR_GSMART_TRACE_M_H */
