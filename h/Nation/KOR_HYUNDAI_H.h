 /* 
 * File:   KOR_HYUNDAI_H.h
 * Author: JiMin JEONG
 * @brief  globals.h에서 분리된 한국 현대백화점 전용 파일
 * Created 2024/06/10
 */

#ifndef KOR_HYUNDAI_H_H
#define	KOR_HYUNDAI_H_H

//----------   국가 Default  ----------//
 #define DEALER_VER_STRING	"(H)"
 #define COUNTRY_STR "  "
 #define USE_HYUNDAI_DEFAULT
 #define USE_TRACE_MEATPART_DEFAULT	            // 기본 부위명 11개 사용
 #define USE_TRACE_MEATGRADE_STANDARD_DEFAULT
//----------   국가 전용 기능  ----------//
 #define USE_KOREA_VERSION_DISPLAY	            // KOREA 버전 표시 
 #define USE_IMPORT_MEAT_PROTOCOL
 #define USE_TRACE_STANDALONE   	            // 축산 이력기능
 #define USE_GSMART_PLU         	            // Use GSMart PLU Structure(Kor)
 #define PLU_COMMODITY_LINE_NO 5	            // 상품명5단
 #define USE_NUTRITION_FACT		                // Use Nutrition Fact
 #define USE_SELLBYDATETIME_ZERO	            // Sell by Date, Sell by Time이 0일 때 인쇄 안하는 기능
 #define USE_CHECKING_HARMFUL_MEAT	            // 수입쇠고기 위해차단 기능(위해리스트 저울 저장)
 #define USE_MULTI_TRACE_NO_TABLE	            // Use Multi Trace Number Table with saving
 #define USE_TRACE_MEATPART			            // 부위명 사용
 #define USE_TRACE_MEATGRADE		            // 국내 등급표시(2011.7.1시행)
 #define USE_PLU_WITH_TRACE_PROTOCOL	        // PLU 전송시 기억된 이력 정보 포함 기능.
 // #define USE_S_PRICE_AS_BASE_PRICE	        // Special price를 기준 가격으로 사용, (Ticket 1288)
 // #define USE_CHECK_CHANGE_REASON	            // 단가 변경시 사유 코드 입력 기능.
 #define USE_CHECK_OUTOFPAPER_ON_LABEL	        // Label모드에서 Paper End 체크하는 기능. 즉시 멈춤
 #define USE_CHECK_INDIV_INFO		            // 개체정보 유효성 확인 프로토콜 추가.
 #define USE_TRACE_SCANNER_FOR_GRADE	        // 바코드 스캐너를 이용해 등급을 호출하는 기능
 #define USE_SCANNER_FOR_ECO_FRIENDLY	        // 친환경 인증 상품 스캔, 조회 기능.
 #define USE_TRACE_PIG_STANDALONE
 #define USE_MARBLING_GRADE_PRINT
 #define USE_SLAUGHTERHOUSE_LENG_100            // 도축장 100 + 1 (null) byte 사용
 #define USE_HYUNDAI_MEAT_FUNCTION	            // 현대 백화점 축산 기능
  #ifdef USE_HYUNDAI_MEAT_FUNCTION
    #define USE_REGIST_DISPOSAL		            // 현대 전용 폐기 등록기능
    #define USE_HYUNDAI_SET_FUNCTION	        // 현대 전용 세트 상품 등록 기능 
    #define USE_HYUNDAI_CHECK_REASON_CODE	    // 현대 전용 사유코드 기능
  #endif
//----------   국가 전용 라벨 설정  ----------//
 // #define USE_KOR_LABELFORMAT		            // 국내 라벨포맷 사용(No.32~45 사용)
//----------   국가 전용 디스플레이 설정  ----------//
 #define FONT_CODEPAGE CODEPAGE_0949_KOR
 #define USE_KOR_FONT
 #define USE_PRICE_TOTAL_7DIGIT		            // Total 7자리 사용
 #define USE_PRICE_UNIT_7DIGIT	                // Unit Price 7자리 사용
//----------   국가 전용 바코드 설정  ----------//

//----------   국가 전용 특정업장 설정  ----------//

#endif  /* KOR_HYUNDAI_H_H */
