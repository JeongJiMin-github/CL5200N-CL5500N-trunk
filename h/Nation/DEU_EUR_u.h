 /* 
 * File:   DEU_EUR_u.h
 * Author: JiMin JEONG
 * @brief  globals.h에서 분리된 유럽 디폴트 사용 독일 전용 파일
 */

#ifndef DEU_EUR_u_H
#define	DEU_EUR_u_H

//----------   국가 Default  ----------//
 #define USE_GERMANY_DEFAULT				// PARAMETER SETTING
 #define COUNTRY_STR "DE"
//----------   국가 전용 기능  ----------//
 #define USE_PRINT_GROSS_AMOUNT_FOR_TAX	    // TAX Report에서 GROSS/NET AMOUNT 표시(for DEU)
 #define NOT_USE_SOME_KEYS_IN_SELFSERVICE
 #define USE_GERMANY_TARE_SCANNER		    // Scanner에 Tare 기능 추가
//----------   국가 전용 라벨 설정  ----------//

//----------   국가 전용 디스플레이 설정  ----------//

//----------   국가 전용 바코드 설정  ----------//
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

#endif /* DEU_EUR_u_H */