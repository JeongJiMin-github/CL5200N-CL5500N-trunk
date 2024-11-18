 /* 
 * File:   ARM_UN_D.h
 * Author: JiMin JEONG
 * @brief  globals.h에서 분리된 direct_ingredient를 사용하는 영공 아르메니아 전용 파일
 * Created 2024/06/10
 */

#ifndef ARM_UN_D_H
#define	ARM_UN_D_H

//----------   국가 Default  ----------//
 #define COUNTRY_STR "AM"
//----------   국가 전용 기능  ----------//

//----------   국가 전용 라벨 설정  ----------//

//----------   국가 전용 디스플레이 설정  ----------//
 #define USE_PRICE_TOTAL_7DIGIT		        // Total 7자리 사용
 #define USE_PRICE_UNIT_7DIGIT	            // Unit Price 7자리 사용
 #define USE_PRICECHECK_8DIGIT	            // Price Checkdigit(8digit)
 #define FONT_CODEPAGE CODEPAGE_1252_ARM
//----------   국가 전용 바코드 설정  ----------//
// #define USE_LOGISTICS_BARCODE
//----------   국가 전용 특정업장 설정  ----------//

#endif  /* ARM_UN_D_H */
