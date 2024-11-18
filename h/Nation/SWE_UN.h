 /* 
 * File:   SWE_UN.h
 * Author: JiMin JEONG
 * @brief  globals.h에서 분리된 스웨덴 전용 파일
 */

#ifndef SWE_UN_H
#define	SWE_UN_H

//----------   국가 Default  ----------//
 #define COUNTRY_STR "SE"
//----------   국가 전용 기능  ----------//

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

#endif  /* SWE_UN_H */
