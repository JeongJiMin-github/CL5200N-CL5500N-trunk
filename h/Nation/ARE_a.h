 /* 
 * File:   ARE_a.h
 * Author: JiMin JEONG
 * @brief  globals.h에서 분리된 아랍에미레이트 전용 파일
 * Create 2024/06/04
 */

#ifndef ARE_a_H
#define	ARE_a_H

//----------   국가 Default  ----------//
#define DEALER_VER_STRING	"(a)"
  #define COUNTRY_STR "  "
//----------   국가 전용 기능  ----------//
 #define USE_TRACEABILITY       	    // Use European Traceability
 #define USE_NUTRITION_FACT		        // Use Nutrition Fact
 #define USE_TAX_RATE				    // Use Tax Rate
 #define USE_DISCOUNTED_TAX_FOR_DISCOUNTED_TOTAL
 #define USE_SELLBYDATETIME_ZERO	    // Sell by Date, Sell by Time이 0일 때 인쇄 안하는 기능
 #define USE_REPORT_TAX         	    // TAX Report 사용
 #define USE_REPORT_PAY         	    // PAY/CHANGE Report 사용
  //#define USE_ARAB_CONVERT		    //아랍어 변환 사용
 #define USE_ENG_BASED_ARAB_SUPPORT		// 영문 기본 + 아랍어 지원(영문방식 표시)
 #define USE_BARCODE_TAX		        // 1873에 T.Price = Price+Tax 가 Y일 경우 barcode total price 필드에 tax 포함
 //#define USE_INGREDIENT_LINE_FEED	    // Ingredient 자동 줄 변환 처리
 //#define USE_BARCODE_FLEXIBLE	// dealer 전용기능
//----------   국가 전용 라벨 설정  ----------//
 #define USE_ENG_LABELFORMAT		    // 영공 라벨포맷 사용(No.1~28 사용)		// CODE 0x407F(16511bytes)
 #define USE_KOR_LABELFORMAT		    // 국내 라벨포맷 사용(No.32~45 사용)	// CODE 0x2314(8980bytes)
//----------   국가 전용 디스플레이 설정  ----------//
 #define USE_ARAB_FONT			        // 아랍폰트 사용
 #define FONT_CODEPAGE CODEPAGE_1256
  #ifdef USE_ARAB_FONT
    #define USE_PRINT_LTR_FOR_ARAB	// 아랍어 없는 데이터 LTR 방향으로 인쇄
  #endif
//----------   국가 전용 바코드 설정  ----------//

//----------   국가 전용 특정업장 설정  ----------//

#endif  /* ARE_a_H */