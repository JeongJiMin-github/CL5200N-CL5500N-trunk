 /* 
 * File:   SAU_PT_D.h
 * Author: JiMin JEONG
 * @brief  globals.h에서 분리된 PT 아랍어(사우디아라비아) 전용 파일
 * Create 2024/06/04
 */

#ifndef SAU_PT_D_H
#define	SAU_PT_D_H

//----------   국가 Default  ----------//
 #define DEALER_VER_STRING	".PT"
 #define COUNTRY_STR "  "
//----------   국가 전용 기능  ----------//
 //#define USE_TRACE_STANDALONE   	// 축산 이력기능
 //#define USE_GSMART_PLU         	// Use GSMart PLU Structure(Kor)
 //#define PLU_COMMODITY_LINE_NO 5	// 상품명5단
 #define USE_TRANSFER_MENU_SETTING	// Parameter 설정을 위해 추가 20200820
 #define USE_TRACEABILITY       	// Use European Traceability
 #define USE_NUTRITION_FACT			// Use Nutrition Fact
 #define USE_DISCOUNTED_TAX_FOR_DISCOUNTED_TOTAL
 #define USE_SELLBYDATETIME_ZERO	// Sell by Date, Sell by Time이 0일 때 인쇄 안하는 기능
 //#define USE_EMART_NETWORK      	// E-Mart Network(실시간 판매 업로드, 매가변경 업로드)
 #define USE_ARABIC_NUMBER			// 아랍 숫자 인쇄 기능 사용
 //#define USE_ARAB_CONVERT			//아랍어 변환 사용
 #define USE_SAUDI_INGREDIENT		// PLU 3000개 + Direct Ingredient 영어(300자) / 아랍어(300자) 적용
 #define USE_TAX_RATE				// Use Tax Rate
 #define USE_REPORT_TAX         	// TAX Report 사용
 #define USE_REPORT_PAY         	// PAY/CHANGE Report 사용
 #define USE_INGREDIENT_98_PROTOCOL
 #define USE_DIRECT_INGREDIENT 
//----------   국가 전용 라벨 설정  ----------//
 #define USE_ENG_LABELFORMAT		// 영공 라벨포맷 사용(No.1~28 사용)		// CODE 0x407F(16511bytes)
 //#define USE_KOR_LABELFORMAT		// 국내 라벨포맷 사용(No.32~45 사용)
//----------   국가 전용 디스플레이 설정  ----------//
 #define USE_ARAB_FONT				// 아랍폰트 사용
 #define FONT_CODEPAGE CODEPAGE_1256
  #ifdef USE_ARAB_FONT
    #define USE_PRINT_LTR_FOR_ARAB	// 아랍어 없는 데이터 LTR 방향으로 인쇄
  #endif
//----------   국가 전용 바코드 설정  ----------//
  #ifdef USE_QR_BARCODE
    #define USE_QR_ADDTOTAL			//QR바코드 이용한 ADD TOTAL 기능
  #endif
//----------   국가 전용 특정업장 설정  ----------//

#endif  /* SAU_PT_D_H */