 /* 
 * File:   HUN_UN.h
 * Author: JiMin JEONG
 * @brief  globals.h에서 분리된 헝가리 전용 파일
 * Created 2024/06/05
 */

#ifndef HUN_UN_H
#define	HUN_UN_H

//----------   국가 Default  ----------//
 #define USE_HUNGARY_DEFAULT
 #define COUNTRY_STR "HU"
//----------   국가 전용 기능  ----------//
 #define USE_LOT_NUMBER_FUNCTION	//LOT NUMBER 사용 //LOT NUMBER  입력, 바코드 출력 가능
//----------   국가 전용 라벨 설정  ----------//

//----------   국가 전용 디스플레이 설정  ----------//
 #define FONT_CODEPAGE CODEPAGE_1250_HUN
//----------   국가 전용 바코드 설정  ----------//
 #define USE_EAN13A5_IN_EXPAND_BARCODE
 #define USE_LOGISTICS_BARCODE
/*		 GS1 바코드 설정 (두 바코드 중복 적용 안됨)           */
//  #define USE_RSS_BARCODE
 #define USE_ONLY_RSS_EXPAND_BARCODE
/* 바코드 데이터 길이 (길이 증가를 원하지 않으면 모두 주석처리) */
//  #define USE_BARCODE_LEN_50
//  #define USE_BARCODE_LEN_60
//  #define USE_BARCODE_LEN_70
 #define USE_BARCODE_LEN_90
/////////////////////////////////////////////////////////////
//----------   국가 전용 특정업장 설정  ----------//		

#endif  /* HUN_UN_H */
