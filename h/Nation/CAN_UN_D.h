 /* 
 * File:   CAN_UN_D.h
 * Author: JiMin JEONG
 * @brief  globals.h에서 분리된 direct_ingredient를 사용하는 캐나다 전용 파일
 * Created 2024/06/05
 */

#ifndef CAN_UN_D_H
#define	CAN_UN_D_H

//----------   국가 Default  ----------//
 #define COUNTRY_STR "CA"
//----------   국가 전용 기능  ----------//
 #define USE_CANADA_CALUNIT				    // Cal unit lb, using kg
 #define USE_CANADA_100GToggle			  // 100g/kg key change to toggle
 #define USE_USA_INGREDIENT				    // USA Direct Ingredient(PLU 4000EA, each ingredient data Max : 2100bytes)
 #define ETH_LED_INDICATOR_DISABLE		// wired lan link indicator disable for NTEP USA
//----------   국가 전용 라벨 설정  ----------//
 #define USE_LABEL_LENGTH_200MM			  // 라벨 길이 사용 200mm (기존 120mm)
 #define USE_USA_LABELFORMAT
 // USA/CANADA 전용 라벨 사용
  #ifdef USE_USA_LABELFORMAT
    #undef USE_ENG_LABELFORMAT
    #undef USE_KOR_LABELFORMAT
  #endif
  #define USE_STANDARD_DIRECT_INGREDIENT
//----------   국가 전용 디스플레이 설정  ----------//
 #define USE_FONT_ID6_12BY28			    // Font ID 6 12x28 font 사용
 #define USE_FONT_ID4_Arial				    // Font ID 4 Arial font 사용
//----------   국가 전용 바코드 설정  ----------//

//----------   국가 전용 특정업장 설정  ----------//

#endif /* CAN_UN_D_H */
