 /* 
 * File:   GEO_AGROHUB_MART_GA.h
 * Author: JiMin JEONG
 * @brief  영공 조지아(그루지아)에서 분리된 그루지아 Agrohub 마트 전용 파일
 */

#ifndef GEO_AGROHUB_MART_GA_H
#define	GEO_AGROHUB_MART_GA_H

//----------   국가 Default  ----------//
 #define USE_GEORGIA_AGROHUB_DEFAULT				    // 그루지아 Agrohub 마트 전용기능 추가
 #define DEALER_VER_STRING	".GA"
 #define COUNTRY_STR "  "
//----------   국가 전용 기능  ----------//
 #define USE_TRACEABILITY						        // Use European Traceability
 #define USE_NUTRITION_FACT						        // Use Nutrition Fact
 #define USE_GEORGIA_MART_SELLBYDATE_FUNCTION           // 0: Pakced_Date, 1: Pakced_Date + 1, 2: Pakced_Date + 2, ... , 900~: No_Pirnt
 #define USE_SELLBYDATETIME_ZERO					    // Sell by Date, Sell by Time이 0일 때 인쇄 안하는 기능
 #define USE_TRANSFER_MENU_SETTING				        // MENU Setting Up/down 기능 추가, *아직 영공만 가능.
 #define USE_CHECK_OUTOFPAPER_ON_LABEL			        // Label모드에서 Paper End 체크하는 기능. 즉시 멈춤
 #define USE_INWEIGHTKEY_WITHOUT_INTERVAL		        // IN_Weight Key 사용시 Weight Interval에 상관없이 1g단위로 사용 가능
 #define USE_TEXT_EMPHASIS						        // ~1 : Italic, ~2 : Bold, ~3 : Underline 강조하는 기능 이후 부터 스페이스 나 엔터가 나오기 전에 강조표현 유지한다
 #define USE_DISCOUNTED_TAX_FOR_DISCOUNTED_TOTAL
 #define USE_TAX_RATE						            // Use Tax Rate
 #define USE_REPORT_TAX						            // TAX Report 사용
 #define USE_REPORT_PAY						            // PAY/CHANGE Report 사용

 #ifdef USE_EXTEND_8M_DFLASH
  #define USE_LABEL_FORMAT_EXT 			            // 라벨 개수 확장(4~8MB 영역, 20개 추가 확장)
 #endif
//----------   국가 전용 라벨 설정  ----------//
 #define USE_ENG_LABELFORMAT				            // 영공 라벨포맷 사용(No.1~28 사용)		// CODE 0x407F(16511bytes)
 #define USE_KOR_LABELFORMAT				            // 국내 라벨포맷 사용(No.32~45 사용)	// CODE 0x2314(8980bytes)
//----------   국가 전용 디스플레이 설정  ----------//
 #define FONT_CODEPAGE CODEPAGE_1251_GEO                // 그루지아 전용 폰트
//----------   국가 전용 바코드 설정  ----------//

//----------   국가 전용 특정업장 설정  ----------//

#endif  /* GEO_AGROHUB_MART_GA_H */
