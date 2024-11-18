 /* 
 * File:   KOR_NHMART_SAFE_n.h
 * Author: JiMin JEONG
 * @brief  globals.h에서 분리된 한국 농협마트 안심한우 전용 파일
 * Created 2024/06/10
 */

#ifndef KOR_NHMART_SAFE_n_H
#define	KOR_NHMART_SAFE_n_H

//----------   국가 Default  ----------//
 #define DEALER_VER_STRING	"(n)"
 #define COUNTRY_STR "  "
 #define USE_NHMART_SAFE_DEFAULT
 #define USE_TRACE_MEATGRADE_STANDARD_DEFAULT
//----------   국가 전용 기능  ----------//
 #define USE_KOREA_VERSION_DISPLAY	            // KOREA 버전 표시 
 #define USE_NHMART_SAFE_MEAT		            // 농협 안심 한우 전용 기능
 #define USE_TRACE_STANDALONE   	            // 축산 이력기능
 #define USE_GSMART_PLU         	            // Use GSMart PLU Structure(Kor)
 #define PLU_COMMODITY_LINE_NO 5	            // 상품명5단
 //#define USE_TRACEABILITY       	            // Use European Traceability
 #define USE_NUTRITION_FACT		                // Use Nutrition Fact
 //#define USE_TAX_RATE				            // Use Tax Rate
 #define USE_SELLBYDATETIME_ZERO	            // Sell by Date, Sell by Time이 0일 때 인쇄 안하는 기능
 //#define USE_EMART_NETWORK      	            // E-Mart Network(실시간 판매 업로드, 매가변경 업로드)
 //#define USE_REPORT_TAX         	            // TAX Report 사용
 //#define USE_REPORT_PAY         	            // PAY/CHANGE Report 사용
 #define USE_KOR_TRACE_999      	            // 농협 개체데이터 999개, 묶음 도축장
 //#define USE_TRACE_FLAG_0_CHECK		        // 개체관리플래그 0일때 개체선택 유무를 체크
 #define USE_TRACE_MEATGRADE		            // 국내 등급표시(2011.7.1시행)
 #define USE_CHECK_OUTOFPAPER_ON_LABEL	        // Label모드에서 Paper End 체크하는 기능. 즉시 멈춤
 #define USE_CHECK_INDIV_INFO		            //개체정보 유효성 확인 프로토콜 추가.
 #define USE_NHSAFE_PIG_TRACE
//----------   국가 전용 라벨 설정  ----------//
 //#define USE_ENG_LABELFORMAT		            // 영공 라벨포맷 사용
 #define USE_KOR_LABELFORMAT		            // 국내 라벨포맷 사용(No.32~45 사용)
//----------   국가 전용 디스플레이 설정  ----------//

//----------   국가 전용 바코드 설정  ----------//

//----------   국가 전용 특정업장 설정  ----------//

#endif  /* KOR_NHMART_SAFE_n_H */
