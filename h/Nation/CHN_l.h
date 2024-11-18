 /* 
 * File:   CHN_l.h
 * Author: JiMin JEONG
 * @brief  globals.h에서 분리된 중국 알리바바 전용 파일
 * Created 2024/06/05
 */

#ifndef CHN_l_H
#define	CHN_l_H

//----------   국가 Default  ----------//
 #define DEALER_VER_STRING  "(l)"
 #define USE_CHINA_DEFAULT				            // PARAMETER SETTING
 #define COUNTRY_STR "  "
//----------   국가 전용 기능  ----------//
 #define USE_TRACEABILITY						    // Use European Traceability
 #define USE_NUTRITION_FACT						    // Use Nutrition Fact
 #define USE_SELLBYDATETIME_ZERO					// Sell by Date, Sell by Time이 0일 때 인쇄 안하는 기능
 #define USE_TRANSFER_MENU_SETTING				    // MENU Setting Up/down 기능 추가, *아직 영공만 가능.
 #define USE_INWEIGHTKEY_WITHOUT_INTERVAL		    // IN_Weight Key 사용시 Weight Interval에 상관없이 1g단위로 사용 가능
 #define USE_TEXT_EMPHASIS						    // ~1 : Italic, ~2 : Bold, ~3 : Underline 강조하는 기능 이후 부터 스페이스 나 엔터가 나오기 전에 강조표현 유지한다
 #define USE_DISCOUNTED_TAX_FOR_DISCOUNTED_TOTAL
 #define USE_TRANSFER_MENU_SETTING
 #define USE_TAX_RATE						        // Use Tax Rate
 #define USE_REPORT_TAX						        // TAX Report 사용
 #define USE_REPORT_PAY						        // PAY/CHANGE Report 사용
 #define USE_WEIGHT_UNIT_500G			            // 중국 단가 500g 단위 추가 (1근 = 500g)
 #define USE_UNIT_500G_PLU_500G_BASE	            // 중국 단가 500g 단위 추가 (PLU 기준 단가: 500g)
 #define USE_PROTOCOL_CHINA_DAHUA		            // 중국 전용 다화 프로토콜
 #define USE_CHINA_PINYIN				            // 중문 입력기
 #define USE_PLU_TIMELEN_CUT   			            // Sell by Time, Packed Time Length 1byte
#define USE_PRT_CONVERT_WEIGHT_KG_G                 // 무게 인쇄 변환 기능 (kg <-> g)
#define USE_BARCODE_DISCOUNT_RATE                   // Barcode에 Discount Rate 항목 추가
#define USE_PRT_CHNIA_DISCOUNT_RATE                 // Discount 값 라벨 인쇄 기능 추가
#define USE_FIXED_PERCENT_KEY_SET                   // fixed Percent discount key 할인율 설정 기능 추가(1~99%)
// #define USE_FIXED_PERCENT_KEY			        // fixed Percent discount key
// #define USE_CHINA_ROUND   				        // Chinese round price
//----------   국가 전용 라벨 설정  ----------//
 #define USE_CHECK_OUTOFPAPER_ON_LABEL			    // Label모드에서 Paper End 체크하는 기능. 즉시 멈춤
//----------   국가 전용 디스플레이 설정  ----------//
 #define USE_CHN_FONT
 #define USE_GB2312_CHN_FONT				        // simple chinese (gb2312)
 #define FONT_CODEPAGE CODEPAGE_0936_CHN
//----------   국가 전용 바코드 설정  ----------//
 #define USE_BARCODE_UPRICE_OVER    		        // when Unit Price Barcode print, valid digits exceeded  number of digits Print ' U ' to zero 
 #define USE_BARCODE_LEN_50 
//----------   국가 전용 특정업장 설정  ----------//

 #endif /* CHN_l_H */