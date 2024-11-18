 /* 
 * File:   KOR_DISCLOSURE_MEAT_PRICE_g.h
 * Author: JiMin JEONG
 * @brief  globals.h에서 분리된 한국 축산공시제 전용 파일
 * Created 2024/06/10
 */

#ifndef KOR_DISCLOSURE_MEAT_PRICE_g_H
#define	KOR_DISCLOSURE_MEAT_PRICE_g_H

//----------   국가 Default  ----------//
 #define DEALER_VER_STRING	"(g)"
 #define COUNTRY_STR "  "
 #define USE_STANDARD_TRACE_DEFAULT
 #define USE_TRACE_MEATPART_DEFAULT	            // 기본 부위명 11개 사용
 #define USE_TRACE_MEATGRADE_MEATPRICE_DEFAULT  // USE 축산공시 전용 등급 사용  
//----------   국가 전용 기능  ----------//
 #define USE_KOREA_VERSION_DISPLAY	            // KOREA 버전 표시
 #define USE_IMPORT_MEAT_PROTOCOL
 #define USE_TRACE_STANDALONE   	            // 축산 이력기능
 #define USE_GSMART_PLU         	            // Use GSMart PLU Structure(Kor)
 #define PLU_COMMODITY_LINE_NO 5	            // 상품명5단
 #define USE_NUTRITION_FACT		                // Use Nutrition Fact
 #define USE_SELLBYDATETIME_ZERO	            // Sell by Date, Sell by Time이 0일 때 인쇄 안하는 기능
 #define USE_CHECKING_HARMFUL_MEAT	            // 수입쇠고기 위해차단 기능(위해리스트 저울 저장)
 #define USE_TRACE_MEATPART			            // 부위명 사용
 #define USE_MULTI_TRACE_NO_TABLE	            // Use Multi Trace Number Table with saving
 #define USE_TRACE_MEATGRADE		            // 국내 등급표시(2011.7.1시행)
 #define USE_TRACE_SCANNER_FOR_GRADE	        // 바코드 스캐너를 이용해 등급을 호출하는 기능
 #define USE_CHECK_OUTOFPAPER_ON_LABEL	        // Label모드에서 Paper End 체크하는 기능. 즉시 멈춤
 #define USE_TRACE_PIG_STANDALONE
 #define USE_TRACE_CALL_MEATGRADE_KEY	        // 등급변경 키 사용. (PLU 등급 미사용)
 #define USE_DIGIT4_PRODUCT_SYMBOL              //생산일 바코드인쇄시 4자리로 표현  년(1)월(1)일(2) -> 2015년 12월 25일 : 5275 10월 넘어가면 일에 50더함. 육가공업체용 제주남문점 요청
 #define NOT_USE_NETSET_LOCK
 #define USE_CTS_FUNCTION                       // Calibration Traceability System	    
 #define USE_MARBLING_GRADE_PRINT
 #define USE_DNS_RESOLVER		                // USE DNS IP QUERY
 #define USE_EUCKR_UTF8_CONVERT	                // USE EUCKR to UTF8 converter
 #define USE_HTTP_CLIENT		                // USE HTTP Client
 #define USE_SEND_TRACE_PRICE	                // USE 축산 공시 Structure , realtime# 8
 #define USE_SCALENAME_TRACE_SHOP_CODE 	        // USE  Scale name to Trace Shop code
 #define USE_PCS_FONT5_TO_TRACE_CODE	        // USE PCS, PLUFONT5 field for Trace CODE 
 //#define USE_DEBUG_SEND_TRACE_PRICE           // USE 축산 공시 HTTP packet 디버그 USE rs232
 //#define USE_DISCLOSURE_MEATGRADE	            // 개체 및 PLU 등급 동시 사용, 개체가 우선순위를 가짐.
 #undef USE_CTS_FUNCTION			            // 마트는 CTS 사용 안함
//----------   국가 전용 라벨 설정  ----------//
 #define USE_KOR_LABELFORMAT		            // 국내 라벨포맷 사용(No.32~45 사용)
//----------   국가 전용 디스플레이 설정  ----------//
 #define FONT_CODEPAGE CODEPAGE_0949_KOR
 #define USE_KOR_FONT
//----------   국가 전용 바코드 설정  ----------//

//----------   국가 전용 특정업장 설정  ----------//

#endif  /* KOR_DISCLOSURE_MEAT_PRICE_g_H */
