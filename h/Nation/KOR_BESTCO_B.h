 /* 
 * File:   KOR_BESTCO_B.h
 * Author: JiMin JEONG
 * @brief  globals.h에서 분리된 한국 대상유통(Bestco Mart) 전용 파일
 */

#ifndef KOR_BESTCO_B_H
#define	KOR_BESTCO_B_H

//----------   국가 Default  ----------//
 #define DEALER_VER_STRING	"(B)"
 #define COUNTRY_STR "  "
 #define USE_STANDARD_TRACE_DEFAULT
 #define USE_TRACE_MEATPART_DEFAULT	        // 기본 부위명 11개 사용
 #define USE_TRACE_MEATGRADE_STANDARD_DEFAULT
 #define USE_BESTCO_DEFAULT
//----------   국가 전용 기능  ----------//
 #define USE_KOREA_VERSION_DISPLAY	        // KOREA 버전 표시 
 #define USE_IMPORT_MEAT_PROTOCOL
 #define USE_TRACE_STANDALONE   	        // 축산 이력기능
 #define USE_TRACE_FLAG_CHECK_FUNCTION      // PLU 이력번호 오류 체크 기능
 #define USE_GSMART_PLU         	        // Use GSMart PLU Structure(Kor)
 #define USE_GSMART_BARCODE                 // 할인 바코드 심볼 추가
 #define USE_BESTCO_DISCOUNT_DIVISION       // 베스트코 전용 할인판매 여부 체크 기능 (반드시 2byte 사용)
 #define PLU_COMMODITY_LINE_NO 5	        // 상품명5단
 #define USE_NUTRITION_FACT		            // Use Nutrition Fact
 #define USE_SELLBYDATETIME_ZERO	        // Sell by Date, Sell by Time이 0일 때 인쇄 안하는 기능
 #define USE_CHECKING_HARMFUL_MEAT	        // 수입쇠고기 위해차단 기능(위해리스트 저울 저장)
 #define USE_TRACE_MEATPART			        // 부위명 사용
 #define USE_MULTI_TRACE_NO_TABLE	        // Use Multi Trace Number Table with saving
 #define USE_TRACE_MEATGRADE		        // 국내 등급표시(2011.7.1시행)
 #define USE_TRACE_SCANNER_FOR_GRADE	    // 바코드 스캐너를 이용해 등급을 호출하는 기능
 #define USE_CHECK_OUTOFPAPER_ON_LABEL	    // Label모드에서 Paper End 체크하는 기능. 즉시 멈춤
 #define USE_TRACE_PIG_STANDALONE
 #define USE_TRACE_CALL_MEATGRADE_KEY	    // 등급변경 키 사용. (PLU 등급 미사용)
 #define USE_DIGIT4_PRODUCT_SYMBOL          // 생산일 바코드인쇄시 4자리로 표현  년(1)월(1)일(2) -> 2015년 12월 25일 : 5275 10월 넘어가면 일에 50더함. 육가공업체용 제주남문점 요청
 #define USE_KOR_INGREDIENT_EXPAND	        // 국내 내수형 펌웨어 PLU 구조(상품명5단) 기준 성분 1,600개 확장 (기존: 200개), PLU 개수 8,000개 유지
 #define NOT_USE_NETSET_LOCK				// 저울 잠금기능 사용하지 않음
 #define USE_CTS_FUNCTION 					// Calibration Traceability System
 #define USE_IMPORT_LOTNO 					// 수입육 이력 번호 24자리 확장 기능. 이 기능 사용하면 이력정보 초기화후에 사용해야함. 메모리 초기화!!
 #define USE_MANUAL_INPUT_IMPORT_LOTNO		// 수입육 묶음번호 입력 기능
 #define USE_MARBLING_GRADE_PRINT			// 소도체 근내지방도
//----------   국가 전용 라벨 설정  ----------//
 #define USE_ENG_LABELFORMAT		        // 영공 라벨포맷 사용
 #define USE_KOR_LABELFORMAT		        // 국내 라벨포맷 사용(No.32~45 사용)
//----------   국가 전용 디스플레이 설정  ----------//
 #define FONT_CODEPAGE CODEPAGE_0949_KOR
 #define USE_KOR_FONT
//----------   국가 전용 바코드 설정  ----------//

//----------   국가 전용 특정업장 설정  ----------//
 
#endif  /* KOR_BESTCO_B_H */
