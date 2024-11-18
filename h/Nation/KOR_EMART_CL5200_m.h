 /* 
 * File:   KOR_EMART_CL5200_m.h
 * Author: JiMin JEONG
 * @brief  globals.h에서 분리된 CL5200 이마트 (V3.03.7부터 사용 안함) - 이마트의 경우 V3.03.7부터 (E)버전과  (m)버전을 통합하여 (E)으로 만 관리 전용 파일
 * Create 2024/06/04
 */

#ifndef KOR_EMART_CL5200_m_H
#define	KOR_EMART_CL5200_m_H

//----------   국가 Default  ----------//
 #define DEALER_VER_STRING	"(m)"
 #define COUNTRY_STR "  "
 #define USE_EMART_DEFAULT
 #define USE_TRACE_MEATGRADE_EMART_DEFAULT
//----------   국가 전용 기능  ----------//
 #define USE_KOREA_VERSION_DISPLAY	        // KOREA 버전 표시 
 #define USE_TRACE_STANDALONE   	        // 축산 이력기능
 #define USE_GSMART_PLU         	        // Use GSMart PLU Structure(Kor)
 #define PLU_COMMODITY_LINE_NO 5	        // 상품명5단
 //#define USE_NUTRITION_FACT		        // Use Nutrition Fact
 // #define USE_SELLBYDATETIME_ZERO	        // Sell by Date, Sell by Time이 0일 때 인쇄 안하는 기능
 //#define USE_CHECK_OUTOFPAPER_ON_LABEL	// Label모드에서 Paper End 체크하는 기능. 즉시 멈춤
 #define USE_KOREA_CL3000_STANDARD_KEY
 //#define USE_SELLBYDATETIME_ZERO	        // Sell by Date, Sell by Time이 0일 때 인쇄 안하는 기능
 #define USE_EMART_NETWORK      	        // E-Mart Network(실시간 판매 업로드, 매가변경 업로드)
 #define USE_DBG_EMART_NETWORK	            // Use Debug Network(without print)
 #define USE_KEEPING_CONNETION  	        // 전송데이터가 계속 있을 때 Remote와 Connection 유지
 #define USE_MULTI_TRACE_NO_TABLE	        // Use Multi Trace Number Table with saving
 #define USE_BARCODE_UPRICE_OVER	        // Unit Price Barcode가 자리수 초과할 경우 'U'를 0으로 인쇄
 #define USE_KOR_SLAUGHT_CODE_4DIGIT	    // 도축장 코드 4자리 사용(이마트 전용)
 #define USE_CHECKING_HARMFUL_MEAT	        // 수입쇠고기 위해차단 기능(위해리스트 저울 저장)
 #define USE_FIXED_PLUTYPE			        // Use Fixed PLU Type(중량혼용, 수량혼용, 중량, 수량)
 #define USE_TRACE_MEATGRADE		        // 국내 등급표시(2011.7.1시행)
 #define USE_TRACE_SCANNER_FOR_GRADE	    // 바코드 스캐너를 이용해 등급을 호출하는 기능
 #define USE_EMART_MEATGRADE_PRINT		    // 이마트 등급표시 인쇄 사용
 #define USE_TRACE_CALL_MEATGRADE_KEY	    // 등급변경 키 사용. (PLU 등급 미사용)
 #define USE_CHECK_OUTOFPAPER_ON_LABEL	    // Label모드에서 Paper End 체크하는 기능. 즉시 멈춤
 #define USE_CONTROL_CHANGING_PRICE	        // 이마트(2012) 매가 변경 통제 기능, USE_S_PRICE_AS_BASE_PRICE와 같이 사용해야 함.
 #define USE_DIVIDING_FUNCTION		        // 쪼개기 기능 사용
 #define USE_EMART_PIG_TRACE
 //#define USE_REALTIME_CHECK_INTERVAL      // parameter 765 로 네트워크 체크 간격 설정. 테스트 후 전체 적용 예정, 전체 적용후 해당 디파인 삭제
 #define USE_EMART_MINCED_MEAT              // 다짐육 보관온도 변경
 #define USE_EMART_PRODUCTION_KEY           // 생산등록키  사용여부 설정
 #undef USE_CTS_FUNCTION			        // 마트는 CTS 사용 안함
//----------   국가 전용 라벨 설정  ----------//
 // #define USE_KOR_LABELFORMAT		        // 국내 라벨포맷 사용(No.32~45 사용)
  #define USE_EMART_BACK_LABEL		        // 이마트 후방라벨 기능 사용. Protocol 90->100 bytes
//----------   국가 전용 디스플레이 설정  ----------//
 #define FONT_CODEPAGE CODEPAGE_0949_KOR
 #define USE_KOR_FONT
//----------   국가 전용 바코드 설정  ----------//

//----------   국가 전용 특정업장 설정  ----------//

#endif  /* KOR_EMART_CL5200_m_H*/