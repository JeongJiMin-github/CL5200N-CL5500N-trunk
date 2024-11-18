 /* 
 * File:   KOR_IMPORT_MEAT_CART_s.h
 * Author: JiMin JEONG
 * @brief  globals.h에서 분리된 수입육 위해 개체(CART)(Pb Bat) 전용 파일
 * Created 2024/06/10
 */

#ifndef KOR_IMPORT_MEAT_CART_s_H
#define	KOR_IMPORT_MEAT_CART_s_H

//----------   국가 Default  ----------//
 #define DEALER_VER_STRING	"(s)"
 #define COUNTRY_STR "  "
 #define USE_STANDARD_TRACE_DEFAULT
 #define USE_TRACE_MEATPART_DEFAULT	        // 기본 부위명 11개 사용
 #define USE_TRACE_MEATGRADE_STANDARD_DEFAULT
//----------   국가 전용 기능  ----------//
 #define USE_KOREA_VERSION_DISPLAY	        // KOREA 버전 표시 
 #define USE_IMPORT_MEAT_PROTOCOL
 //#define USE_CHECK_SCANNING_DATA	        // SCANNER Disabel CR, Enable LF일 때만 동작하게 함
 #define USE_TRACE_STANDALONE   	        // 축산 이력기능
 #define USE_GSMART_PLU         	        // Use GSMart PLU Structure(Kor)
 #define PLU_COMMODITY_LINE_NO 5	        // 상품명5단
 //#define USE_TRACEABILITY       	        // Use European Traceability
 #define USE_NUTRITION_FACT		            // Use Nutrition Fact
 //#define USE_TAX_RATE				        // Use Tax Rate
 #define USE_SELLBYDATETIME_ZERO	        // Sell by Date, Sell by Time이 0일 때 인쇄 안하는 기능
 //#define USE_EMART_NETWORK      	        // E-Mart Network(실시간 판매 업로드, 매가변경 업로드)
 //#define USE_REPORT_TAX         	        // TAX Report 사용
 //#define USE_REPORT_PAY         	        // PAY/CHANGE Report 사용
 //#define USE_RFID_TAG_READER		        // Use RFID Reader(한맥 ENG)
 #define USE_CHECKING_HARMFUL_MEAT	        // 수입쇠고기 위해차단 기능(위해리스트 저울 저장)
 //#define USE_EXT_CHECKING_HARMFULNESS	    // 위해리스트 서버 저장
 #define USE_TRACE_MEATPART			        // 부위명 사용
 //#define USE_MAX_SCALE_NO_255		        // 저울번호 255개 사용
 #define USE_MULTI_TRACE_NO_TABLE	        // Use Multi Trace Number Table with saving
 #define USE_TRACE_MEATGRADE		        // 국내 등급표시(2011.7.1시행)
 //#define USE_TRACE_SCANNER_FOR_GRADE	    // 바코드 스캐너를 이용해 등급을 호출하는 기능
 #define USE_CHECK_OUTOFPAPER_ON_LABEL	    // Label모드에서 Paper End 체크하는 기능. 즉시 멈춤
 #define USE_CHN_IO_BOARD		            // 중국 대차스케일용 I/O Board 사용
 #define USE_PB_BATTERY			            // Pb Battery  기능
//----------   국가 전용 라벨 설정  ----------//
 //#define USE_ENG_LABELFORMAT		        // 영공 라벨포맷 사용
 #define USE_KOR_LABELFORMAT		        // 국내 라벨포맷 사용(No.32~45 사용)
//----------   국가 전용 디스플레이 설정  ----------//

//----------   국가 전용 바코드 설정  ----------//

//----------   국가 전용 특정업장 설정  ----------//

#endif  /* KOR_IMPORT_MEAT_CART_s_H */