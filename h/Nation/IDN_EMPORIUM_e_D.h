 /* 
 * File:   IDN_EMPORIUM_e_D.h
 * Author: JiMin JEONG
 * @brief  globals.h에서 분리된 Direct ingredient를 사용하는 인도네시아 엠포리움 마켓 전용 파일
 * Create 2024/06/04
 */

#ifndef IDN_EMPORIUM_e_D_H
#define	IDN_EMPORIUM_e_D_H

//----------   국가 Default  ----------//
 #define DEALER_VER_STRING  "(e)"
 #define COUNTRY_STR "  "
 #define USE_EMPORIUM_DEFAULT		        // Use Indonesia Emporium Default Barcode
//----------   국가 전용 기능  ----------//
 #define USE_TRACEABILITY		            // Use European Traceability
 #define USE_NUTRITION_FACT	                // Use Nutrition Fact
 #define USE_SELLBYDATETIME_ZERO            // Sell by Date, Sell by Time이 0일 때 인쇄 안하는 기능
 #define USE_TRANSFER_MENU_SETTING          // MENU Setting Up/down 기능 추가, *아직 영공만 가능.
 #define USE_CHECK_OUTOFPAPER_ON_LABEL      // Label모드에서 Paper End 체크하는 기능. 즉시 멈춤
 #define USE_INWEIGHTKEY_WITHOUT_INTERVAL	// IN_Weight Key 사용시 Weight Interval에 상관없이 1g단위로 사용 가능
 #define USE_TEXT_EMPHASIS				    // ~1 : Italic, ~2 : Bold, ~3 : Underline 강조하는 기능 이후 부터 스페이스 나 엔터가 나오기 전에 강조표현 유지한다
 #define USE_BARCODE_SHARP_CHECKDIGIT       // indonesia code128X 바코드에서 #캐릭터 사용시 체크섬 값 변경.
 #define USE_EMPORIUM_INGREDIENT 	        // use direct ingredient for emporium (Indonesia Market) => 150byte*2000ea(Max: 300byte)
 #define USE_PLU_MULTIPLE_PRICE		        // use direct multiple price (Direct discount function) for emporium
 #define USE_DIRECT_INGREDIENT	            // use direct ingredient. Russia 는 항상 사용
//----------   국가 전용 라벨 설정  ----------//
 #define USE_ENG_LABELFORMAT 	            // 영공 라벨포맷 사용(No.1~28 사용)	   // CODE 0x407F(16511bytes)
 #define USE_KOR_LABELFORMAT 	            // 국내 라벨포맷 사용(No.32~45 사용)    // CODE 0x2314(8980bytes)
//----------   국가 전용 디스플레이 설정  ----------//
 #define USE_PRICE_TOTAL_7DIGIT	            // Total 7자리 사용
 #define USE_PRICECHECK_8DIGIT	            // Price Checkdigit(8digit)
//----------   국가 전용 바코드 설정  ----------//

//----------   국가 전용 특정업장 설정  ----------//

#endif  /* IDN_EMPORIUM_e_D_H */