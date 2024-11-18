 /* 
 * File:   CHN_STANDARDMARKET_c.h
 * Author: JiMin JEONG
 * @brief  globals.h에서 분리된 중국표준화시장 전용 파일
 * Create 2024/06/04
 */

#ifndef CHN_STANDARDMARKET_c_H
#define	CHN_STANDARDMARKET_c_H

//----------   국가 Default  ----------//
 #define DEALER_VER_STRING	"(c)"
 #define COUNTRY_STR "  "
//----------   국가 전용 기능  ----------//
 //#define USE_TRACE_STANDALONE   	        // 축산 이력기능
 //#define USE_GSMART_PLU         	        // Use GSMart PLU Structure(Kor)
 //#define PLU_COMMODITY_LINE_NO 5	        // 상품명5단
 //#define USE_TRACEABILITY       	        // Use European Traceability
 //#define USE_NUTRITION_FACT		        // Use Nutrition Fact
 #define USE_TAX_RATE				        // Use Tax Rate
 #define USE_DISCOUNTED_TAX_FOR_DISCOUNTED_TOTAL
 #define USE_SELLBYDATETIME_ZERO	        // Sell by Date, Sell by Time이 0일 때 인쇄 안하는 기능
//#define USE_EMART_NETWORK      	        // E-Mart Network(실시간 판매 업로드, 매가변경 업로드)
 #define USE_REPORT_TAX         	        // TAX Report 사용
 #define USE_REPORT_PAY         	        // PAY/CHANGE Report 사용
 #define USE_TRANSFER_MENU_SETTING	        // MENU Setting Up/down 기능 추가, *아직 영공만 가능.
 #define USE_PROTOCOL_CL5000JR   	        // for CL5000J Protocol
 #define USE_DBG_EMART_NETWORK	            // 디버깅용 기능 :인쇄없이 발행

 #define USE_CHN_CART_SCALE		            // 중국 대차스케일(표준화시장 이력코드기능)
 #define USE_CHN_IO_BOARD		            // 중국 대차스케일용 I/O Board 사용
 #define USE_PB_BATTERY			            // Pb Battery  기능
 #define USE_RFID_TAG_READER_RFID_522_B	    // 중국 RFID(522_B모듈)
//----------   국가 전용 라벨 설정  ----------//
 #define USE_ENG_LABELFORMAT		        // 영공 라벨포맷 사용(No.1~28 사용)		// CODE 0x407F(16511bytes)
 //#define USE_KOR_LABELFORMAT		        // 국내 라벨포맷 사용(No.32~45 사용)
//----------   국가 전용 디스플레이 설정  ----------//
 #define USE_CHN_FONT
 #define USE_GB2312_CHN_FONT                // simple chinese (gb2312)
//----------   국가 전용 바코드 설정  ----------//

//----------   국가 전용 특정업장 설정  ----------//

#endif  /* CHN_STANDARDMARKET_c_H */