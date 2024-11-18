 /* 
 * File:   POL_UN.h
 * Author: JiMin JEONG
 * @brief  globals.h에서 분리된 폴란드 전용 파일
 * Created 2024/06/05
 */

#ifndef POL_UN_H
#define	POL_UN_H

//----------   국가 Default  ----------//
 #define USE_POLAND_DEFAULT
 #define COUNTRY_STR "PL"
//----------   국가 전용 기능  ----------//
 #define USE_EU_NUTRITIONFACT
//  #define USE_POL_NUTRITIONFACT_ENG_CAP			// dealer 전용 기능
//----------   국가 전용 라벨 설정  ----------//

//----------   국가 전용 디스플레이 설정  ----------//
 #define FONT_CODEPAGE CODEPAGE_1250_POL
//----------   국가 전용 바코드 설정  ----------//
  #ifdef USE_QR_BARCODE
    #define USE_QR_ADDTOTAL				            // QR바코드 이용한 ADD TOTAL 기능
  #endif
/*  		    GS1 바코드 설정 (두 바코드 중복 적용 안됨)           */
//  #define USE_RSS_BARCODE
 #define USE_ONLY_RSS_EXPAND_BARCODE
/* 바코드 데이터 길이 (길이 증가를 원하지 않으면 모두 주석처리) */
//  #define USE_BARCODE_LEN_50
//  #define USE_BARCODE_LEN_60
//  #define USE_BARCODE_LEN_70
 #define USE_BARCODE_LEN_90                   // 실제 데이터 70자까지 사용 가능 (24년 6월기준)
/////////////////////////////////////////////////////////////
//----------   국가 전용 특정업장 설정  ----------//

#endif  /* POL_UN_H */
