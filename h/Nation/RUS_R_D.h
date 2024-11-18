 /* 
 * File:   RUS_R_D.h
 * Author: JiMin JEONG
 * @brief  globals.h에서 분리된 direct_ingredient를 사용하는 영공 러시아 전용 파일
 * Created 2024/06/10
 */

#ifndef RUS_R_D_H
#define	RUS_R_D_H

//----------   국가 Default  ----------//
 #define COUNTRY_STR "  "
//----------   국가 전용 기능  ----------//
 #define USE_RUSSIA_VER_DISPLAY		// Russia 버전 표기 사용. 외부 표시 버전은 고정됨.
// #define USE_LABEL_LENGTH_120MM		// 기본 80mm
//----------   국가 전용 라벨 설정  ----------//

//----------   국가 전용 디스플레이 설정  ----------//
 #define USE_PRICE_TOTAL_7DIGIT		        // Total 7자리 사용
 #define USE_PRICE_UNIT_7DIGIT	            // Unit Price 7자리 사용
 #define USE_PRICECHECK_8DIGIT	            // Price Checkdigit(8digit)
 #define FONT_CODEPAGE CODEPAGE_1251
//----------   국가 전용 바코드 설정  ----------//

//----------   국가 전용 특정업장 설정  ----------//

#endif  /* RUS_R_D_H */
