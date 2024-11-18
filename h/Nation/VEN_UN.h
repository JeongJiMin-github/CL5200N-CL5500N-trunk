 /* 
 * File:   VEN_UN.h
 * Author: JiMin JEONG
 * @brief  globals.h에서 분리된 베네수엘라 전용 파일
 * Created 2024/06/05
 */

#ifndef VEN_UN_H
#define	VEN_UN_H

//----------   국가 Default  ----------//
 #define USE_VENEZUELA_DEFAULT
 #define COUNTRY_STR "VE"
//----------   국가 전용 기능  ----------//
 #define USE_VENEZUELA_VER_DISPLAY		      // 베네수엘라 인증 버전 표시
//----------   국가 전용 라벨 설정  ----------//

//----------   국가 전용 디스플레이 설정  ----------//
 #define USE_PRICE_TOTAL_9DIGIT			        // total price 9digit
 #define USE_PRICE_UNIT_9DIGIT			        // unit price 9digit
//  #define USE_PRICE_TOTAL_8DIGIT			    // total price 8digit
//  #define USE_PRICE_UNIT_8DIGIT			      // unit price 8digit

//----------   국가 전용 바코드 설정  ----------//
  #ifdef USE_QR_BARCODE
    #define USE_QR_ADDTOTAL
  #endif
//----------   국가 전용 특정업장 설정  ----------//		

#endif /* VEN_UN_H */
