 /* 
 * File:   KOR_SCALE_POS_T.h
 * Author: JiMin JEONG
 * @brief  globals.h에서 분리된 한국 저울나라 전용 파일
 * Created 2024/06/10
 */

#ifndef KOR_SCALE_POS_T_H
#define	KOR_SCALE_POS_T_H

//----------   국가 Default  ----------//
 #define DEALER_VER_STRING	"(T)"
 #define COUNTRY_STR "  "
 #define USE_TRACE_MEATGRADE_STANDARD_DEFAULT
//----------   국가 전용 기능  ----------//
 #define USE_KOREA_VERSION_DISPLAY	        // KOREA 버전 표시
 #define USE_KOR_TRACE          	        // 저울나라 프로토콜
 #define USE_TRACE_STANDALONE   	        // 축산 이력기능
 #define USE_GSMART_PLU         	        // Use GSMart PLU Structure(Kor)
 #define PLU_COMMODITY_LINE_NO 5	        // 상품명5단
 #define USE_SELLBYDATETIME_ZERO	        // Sell by Date, Sell by Time이 0일 때 인쇄 안하는 기능
 #define USE_TRACE_MEATGRADE		        // 국내 등급표시(2011.7.1시행)
 #define USE_CHECK_OUTOFPAPER_ON_LABEL	    // Label모드에서 Paper End 체크하는 기능. 즉시 멈춤
  #ifdef CL5200_BP
    //  #define USE_SCALE_POS_TEST_VERSION	// 저울나라 외부 발송용 테스트 버전
  #endif
 #define USE_TRACE_SCANNER_FOR_GRADE	    // 바코드 스캐너를 이용해 등급을 호출하는 기능
 #define USE_MULTI_TRACE_NO_TABLE           // Use Multi Trace Number Table with saving
 #define USE_TRACE_CALL_MEATGRADE_KEY	    // 등급변경 키 사용. (PLU 등급 미사용)
 #define USE_TRACE_PIG_STANDALONE
 #define USE_MARBLING_GRADE_PRINT
 #undef USE_CTS_FUNCTION 		            // 마트는 CTS 사용 안함 
//----------   국가 전용 라벨 설정  ----------//
 #define USE_KOR_LABELFORMAT		        // 국내 라벨포맷 사용(No.32~45 사용)
//----------   국가 전용 디스플레이 설정  ----------//
 #define FONT_CODEPAGE CODEPAGE_0949_KOR
 #define USE_KOR_FONT
//----------   국가 전용 바코드 설정  ----------//

//----------   국가 전용 특정업장 설정  ----------//

#endif  /* KOR_SCALE_POS_T_H */
