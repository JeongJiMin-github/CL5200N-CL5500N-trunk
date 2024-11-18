 /* 
 * File:   ISR_HEBREW_h.h
 * Author: JiMin JEONG
 * @brief  globals.h에서 분리된 히브리어를 사용하는 이스라엘 전용 파일
 * Create 2024/06/04
 */

#ifndef ISR_HEBREW_h_H
#define	ISR_HEBREW_h_H

//----------   국가 Default  ----------//
 #define DEALER_VER_STRING  "(h)"
 #define COUNTRY_STR "  "
 #define USE_HEBREW_DEFAULT
//----------   국가 전용 기능  ----------//
 #define USE_TRACEABILITY		            // Use European Traceability
 #define USE_NUTRITION_FACT	                // Use Nutrition Fact
 #define USE_TAX_RATE			            // Use Tax Rate
 #define USE_DISCOUNTED_TAX_FOR_DISCOUNTED_TOTAL
 #define USE_SELLBYDATETIME_ZERO            // Sell by Date, Sell by Time이 0일 때 인쇄 안하는 기능
 #define USE_REPORT_TAX			            // TAX Report 사용
 #define USE_REPORT_PAY			            // PAY/CHANGE Report 사용
//----------   국가 전용 라벨 설정  ----------//
 #define USE_ENG_LABELFORMAT 	            // 영공 라벨포맷 사용(No.1~28 사용)	   // CODE 0x407F(16511bytes)
 #define USE_KOR_LABELFORMAT 	            // 국내 라벨포맷 사용(No.32~45 사용)    // CODE 0x2314(8980bytes)
//----------   국가 전용 디스플레이 설정  ----------//
 #define HEBREW_FONT				        // 히브리어 폰트 사용
//----------   국가 전용 바코드 설정  ----------//

//----------   국가 전용 특정업장 설정  ----------//

#endif  /* ISR_HEBREW_h */
