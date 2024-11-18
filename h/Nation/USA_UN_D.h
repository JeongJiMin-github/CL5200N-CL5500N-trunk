 /* 
 * File:   USA_UN_D.h
 * Author: JiMin JEONG
 * @brief  globals.h에서 분리된 direct_ingredient를 사용하는 미국 전용 파일
 * Created 2024/06/05
 */

#ifndef USA_UN_D_H
#define	USA_UN_D_H

//----------   국가 Default  ----------//
 #define USE_USA_DEFAULT
 #define COUNTRY_STR "US"
//----------   국가 전용 기능  ----------//
 #define USE_USA_FIXEDWEIGHT_UNIT_OZ		  // Nonweight, Count PLU Fixed Weight Unit change to oz
 #define ETH_LED_INDICATOR_DISABLE		    // wired lan link indicator disable for NTEP USA
 //#define USE_USA_INGREDIENT				        // USA Direct Ingredient(PLU 4000EA, each ingredient data Max : 2100bytes)
 #define USE_NORTH_AMERICA_INGREDIENT	        // 미국 / 캐나다 전용 Direct Ingredient & Nutrition Fact 구조 ( PLU 4,000EA, each Nutrition Fact)
 #define USE_ADDITIONAL_NUTRIENT			    // Nutrient 항목 추가.
 #define USE_DIRECT_NUTRITION
 #undef USE_ALLOW_CAL_MODE				        // 미국은 해당 기능 동작 안 하게 디파인 제거			
 #define USE_USA_VERSION_DISPLAY			    // 미국 인증 버전 표시 (CL5200N -> CL7000, CL5500N -> CL5000으로 표시)
 #define USE_JULIAN_DATE
 #define USE_BEARER_BAR_ONE_LINE			    // 바코드 상단 하단 옵션사항인 테두리 바를 상단의 하나로만 출력(미국 요청)
 #define USE_CANADA_ZERO_MARK_SETTING	    // 인증 법규 적용. CAL8170에서 Gross Zero Mark[N] / Net Zero Mark[Y], Menu에서 변경 불가
  #ifdef USE_USA_NTEP_SETTING
    #define USE_NTEP_CERT_DEFAULT			    // 인증용 기본값 적용 (1. 인증용 라벨 ID 적용)
  #endif
  #define USE_STANDARD_DIRECT_INGREDIENT
//----------   국가 전용 라벨 설정  ----------//
 // USA/CANADA 전용 라벨 사용
 #define USE_USA_LABELFORMAT
 #define USE_LABEL_LENGTH_200MM			      // 라벨 길이 사용 200mm (기존 120mm)
//----------   국가 전용 디스플레이 설정  ----------//
 #define USE_KOR_FONT
 #define USE_FONT_ID6_12BY28				      // Font ID 6 12x28 font 사용
 #define USE_FONT_ID4_Arial				        // Font ID 4 Arial font 사용
 #define USE_USA_UNIT_PRICE_DOT_DISP		  // 미국 화장판에 맞는 $/kg, $/lb dot 표시
 #define FONT_CODEPAGE CODEPAGE_0949_KOR
//----------   국가 전용 바코드 설정  ----------//

//----------   국가 전용 특정업장 설정  ----------//

#endif /* USA_UN_D_H */