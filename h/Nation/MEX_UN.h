 /* 
 * File:   MEX_UN.h
 * Author: JiMin JEONG
 * @brief  globals.h에서 분리된 멕시코 전용 파일
 * Created 2024/06/05
 */

#ifndef MEX_UN_H
#define	MEX_UN_H

//----------   국가 Default  ----------//
 #define COUNTRY_STR "MX"
//----------   국가 전용 기능  ----------//
 #define USE_BAKERY_FUNCTION
  #ifdef USE_BAKERY_FUNCTION
    #define USE_SEND_TICKET_DATA
  #endif
 #define USE_LOT_NUMBER_FUNCTION     // LOT Number 기능 적용
//----------   국가 전용 라벨 설정  ----------//

//----------   국가 전용 디스플레이 설정  ----------//

//----------   국가 전용 바코드 설정  ----------//
/*		 GS1 바코드 설정 (두 바코드 중복 적용 안됨)           */
//  #define USE_RSS_BARCODE
// #define USE_ONLY_RSS_EXPAND_BARCODE
/* 바코드 데이터 길이 (길이 증가를 원하지 않으면 모두 주석처리) */
//  #define USE_BARCODE_LEN_50
//  #define USE_BARCODE_LEN_60
 #define USE_BARCODE_LEN_70
//  #define USE_BARCODE_LEN_90
/////////////////////////////////////////////////////////////
//----------   국가 전용 특정업장 설정  ----------//
	
#endif  /* MEX_UN_H */
