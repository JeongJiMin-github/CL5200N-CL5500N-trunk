 /* 
 * File:   TUR_QR_BARCODE_t_D.h
 * Author: JiMin JEONG
 * @brief  globals.h에서 분리된 Direct ingredient를 사용하는 터키 요청사항 대응 (입력한 URL을 QR 바코드로 출력하는 기능) 전용 파일
 * Create 2024/06/04
 */

#ifndef TUR_QR_BARCODE_t_D_H
#define	TUR_QR_BARCODE_t_D_H

//----------   국가 Default  ----------//
 #define DEALER_VER_STRING  "(t)"
 #define COUNTRY_STR "  "
 #define USE_TURKEY_QR_BARCODE_DEAULT
 #define USE_ESD_ADVANCE_DEFAULT
//----------   국가 전용 기능  ----------//
 #define USE_TRACEABILITY						// Use European Traceability
 #define USE_NUTRITION_FACT						// Use Nutrition Fact
 #define USE_SELLBYDATETIME_ZERO				// Sell by Date, Sell by Time이 0일 때 인쇄 안하는 기능
 #define USE_TRANSFER_MENU_SETTING				// MENU Setting Up/down 기능 추가, *아직 영공만 가능.
 #define USE_CHECK_OUTOFPAPER_ON_LABEL			// Label모드에서 Paper End 체크하는 기능. 즉시 멈춤
 #define USE_INWEIGHTKEY_WITHOUT_INTERVAL		// IN_Weight Key 사용시 Weight Interval에 상관없이 1g단위로 사용 가능
 #define USE_TEXT_EMPHASIS						// ~1 : Italic, ~2 : Bold, ~3 : Underline 강조하는 기능 이후 부터 스페이스 나 엔터가 나오기 전에 강조표현 유지한다
 #define USE_DISCOUNTED_TAX_FOR_DISCOUNTED_TOTAL
 #define USE_DIRECT_INGREDIENT_EXTEND_8M_DFLASH	// 8MB D_Flash 확장영역 활용  -  Direct Ingredient (152Bytes * 2,000 Blocks) → (152Bytes * 24,000 Blocks)
 #define USE_PRINT_URL_VIA_QR_BARCODE			// 저울에 입력받은 URL을 QR바코드를 통해서 출력(URL은 PLU마다 할당 된 Direct Ingredient에 저장,  USE_DIRECT_INGREDIENT관련 디파인 필요)
 #define USE_TAX_RATE							// Use Tax Rate
 #define USE_REPORT_TAX							// TAX Report 사용
 #define USE_REPORT_PAY							// PAY/CHANGE Report 사용
 #define USE_DIRECT_INGREDIENT
 #define USE_INGREDIENT_98_PROTOCOL				// PLU 통신 할 경우 Direct ingredient는 98을 사용(protocol 매뉴얼 참조)
//----------   국가 전용 라벨 설정  ----------//
 #define USE_ENG_LABELFORMAT						// 영공 라벨포맷 사용(No.1~28 사용)		// CODE 0x407F(16511bytes)
//----------   국가 전용 디스플레이 설정  ----------//
 #define FONT_CODEPAGE	CODEPAGE_1254
//----------   국가 전용 바코드 설정  ----------//
  #ifdef USE_QR_BARCODE
    #define USE_QR_ADDTOTAL						//QR바코드 이용한 ADD TOTAL 기능
  #endif
//----------   국가 전용 특정업장 설정  ----------//

#endif  /* TUR_QR_BARCODE_t_D_H */