 /* 
 * File:   ZAF_SPAR_MART_SR_D.h
 * Author: JiMin JEONG
 * @brief  globals.h에서 분리된 Direct ingredient를 사용하는 SR 남아공 SPAR 체인마트 전용 사양 전용 파일
 * Create 2024/06/04
 */

#ifndef ZAF_SPAR_MART_SR_D_H
#define	ZAF_SPAR_MART_SR_D_H

//----------   국가 Default  ----------//
 #define DEALER_VER_STRING	".SR"
 #define COUNTRY_STR "  "
//----------   국가 전용 기능  ----------//
 #define USE_TRACEABILITY						// Use European Traceability
 #define USE_NUTRITION_FACT						// Use Nutrition Fact
 #define USE_SELLBYDATETIME_ZERO				// Sell by Date, Sell by Time이 0일 때 인쇄 안하는 기능
 #define USE_TRANSFER_MENU_SETTING				// MENU Setting Up/down 기능 추가, *아직 영공만 가능.
 #define USE_CHECK_OUTOFPAPER_ON_LABEL			// Label모드에서 Paper End 체크하는 기능. 즉시 멈춤
 #define USE_INWEIGHTKEY_WITHOUT_INTERVAL		// IN_Weight Key 사용시 Weight Interval에 상관없이 1g단위로 사용 가능
 #define USE_TEXT_EMPHASIS						// ~1 : Italic, ~2 : Bold, ~3 : Underline 강조하는 기능 이후 부터 스페이스 나 엔터가 나오기 전에 강조표현 유지한다
 #define USE_DISCOUNTED_TAX_FOR_DISCOUNTED_TOTAL
 //Remote monitoring & Remote Transfer parameter
 //define USE_REMOTE_KEY
 //define USE_REMOTE_DISP
 //define USE_ADMIN_EMAIL
 #define USE_RSA_SPAR_D_INGREDIENT				// 남아공 SPAR 체인마트 전용 PLU 4,000개  /  Direct Ingredient 900자
 #define USE_TAX_RATE						// Use Tax Rate
 #define USE_REPORT_TAX						// TAX Report 사용
 #define USE_REPORT_PAY						// PAY/CHANGE Report 사용
 #define USE_DIRECT_INGREDIENT				// Use direct ingredient
 #define USE_INGREDIENT_98_PROTOCOL			// PLU 통신 할 경우 Direct ingredient는 98을 사용(protocol 매뉴얼 참조)
//----------   국가 전용 라벨 설정  ----------//
 #define USE_ENG_LABELFORMAT					// 영공 라벨포맷 사용(No.1~28 사용)		// CODE 0x407F(16511bytes)
 #define USE_KOR_LABELFORMAT					// 국내 라벨포맷 사용(No.32~45 사용)	// CODE 0x2314(8980bytes)
//----------   국가 전용 디스플레이 설정  ----------//

//----------   국가 전용 바코드 설정  ----------//
  #ifdef USE_QR_BARCODE
    #define USE_QR_ADDTOTAL
  #endif
//----------   국가 전용 특정업장 설정  ----------//

#endif  /* ZAF_SPAR_MART_SR_D_H */