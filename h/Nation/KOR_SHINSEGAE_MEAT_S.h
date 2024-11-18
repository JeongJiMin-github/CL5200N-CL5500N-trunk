 /* 
 * File:   KOR_SHINSEGAE_MEAT_S.h
 * Author: JiMin JEONG
 * @brief  globals.h에서 분리된 신세계 백화점 전용 파일
 * Created 2024/06/10
 */

#ifndef KOR_SHINSEGAE_MEAT_S_H
#define	KOR_SHINSEGAE_MEAT_S_H

//----------   국가 Default  ----------//
 #define DEALER_VER_STRING	"(S)"
 #define COUNTRY_STR "  "
 #define USE_SHINSEGAE_DEFAULT
 #define USE_STANDARD_TRACE_DEFAULT
 #define USE_TRACE_MEATPART_DEFAULT	        // 기본 부위명 11개 사용
 #define USE_TRACE_MEATGRADE_STANDARD_DEFAULT
//----------   국가 전용 기능  ----------//
 #define USE_KOREA_VERSION_DISPLAY	        // KOREA 버전 표시 
 #define USE_IMPORT_MEAT_PROTOCOL
 #define USE_TRACE_STANDALONE   	        // 축산 이력기능
 #define USE_GSMART_PLU         	        // Use GSMart PLU Structure(Kor)
 #define PLU_COMMODITY_LINE_NO 5	        // 상품명5단
 #define USE_NUTRITION_FACT		            // Use Nutrition Fact
 #define USE_SELLBYDATETIME_ZERO	        // Sell by Date, Sell by Time이 0일 때 인쇄 안하는 기능
 #define USE_CHECKING_HARMFUL_MEAT	        // 수입쇠고기 위해차단 기능(위해리스트 저울 저장)
 #define USE_TRACE_MEATPART			        // 부위명 사용
 #define USE_MULTI_TRACE_NO_TABLE	        // Use Multi Trace Number Table with saving
 #define USE_TRACE_MEATGRADE		        // 국내 등급표시(2011.7.1시행)
 #define USE_CHECK_OUTOFPAPER_ON_LABEL	    // Label모드에서 Paper End 체크하는 기능. 즉시 멈춤
 //#define USE_MEATGRADE_NO_TEXT		    // 축산 이력 등급 표시 기능 중  등급 : 표시 제거
 #define USE_TRACE_PIG_STANDALONE
 #define USE_TRACE_CALL_MEATGRADE_KEY       // 등급변경 키 사용. (PLU 등급 미사용)
 #define USE_MARBLING_GRADE_PRINT
 //#define USE_KOR_INGREDIENT_EXPAND	    // 국내 내수형 펌웨어 PLU 구조(상품명5단) 기준 성분 16,000개 확장 (기존: 200개), PLU 개수 8,000개 유지
 #undef USE_CTS_FUNCTION			        // 마트는 CTS 사용 안함
//----------   국가 전용 라벨 설정  ----------//
 #define USE_KOR_LABELFORMAT		        // 국내 라벨포맷 사용(No.32~45 사용)
//----------   국가 전용 디스플레이 설정  ----------//
 #define FONT_CODEPAGE CODEPAGE_0949_KOR
 #define USE_KOR_FONT
  #if defined(USE_CL5500_DISPLAY)           // [ CL5500N ]  CL5500 Display
    /* CL5500N 모델은 7자리 표시 적용 */
    #define USE_PRICE_UNIT_7DIGIT		    // Unit Price 7자리 사용
    #define USE_PRICE_TOTAL_7DIGIT		    // Total 7자리 사용
  #elif defined USE_CL5200_DISPLAY          // [ CL5200N ]  CL5200 Display 적용
    /* CL5200N 모델은 7자리 표시 적용 안함 */
  #endif
//----------   국가 전용 바코드 설정  ----------//

//----------   국가 전용 특정업장 설정  ----------//

#endif  /* KOR_SHINSEGAE_MEAT_S_H */