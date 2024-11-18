 /* 
 * File:   TUR_UN.h
 * Author: JiMin JEONG
 * @brief  globals.h에서 분리된 튀르키예 전용 파일
 * Created 2024/06/05
 */

#ifndef TUR_UN_H
#define	TUR_UN_H

//----------   국가 Default  ----------//
 #define USE_ESD_ADVANCE_DEFAULT
 #define COUNTRY_STR "TR"
//----------   국가 전용 기능  ----------//
 #define USE_LONGPRESS_KEY_OVERRIDE         // long key 로 PLU 단가 변경 기능
//----------   국가 전용 라벨 설정  ----------//

//----------   국가 전용 디스플레이 설정  ----------//
 #define USE_PRICE_TOTAL_7DIGIT		          // Total 7자리 사용
 #define USE_PRICE_UNIT_7DIGIT		          // Unit Price 7자리 사용
 #define FONT_CODEPAGE CODEPAGE_1254
//----------   국가 전용 바코드 설정  ----------//
  #ifdef USE_QR_BARCODE
    #define USE_QR_ADDTOTAL				          //QR바코드 이용한 ADD TOTAL 기능
    #ifdef USE_QR_ADDTOTAL
        #define USE_QR_SIZE_SETTING		      //QR사이즈 설정 기능(1 ~ 5) : ADD-TOTAL TICKET에서만 사용
    #endif
  #endif
//----------   국가 전용 특정업장 설정  ----------//

#endif /* TUR_UN_H */
