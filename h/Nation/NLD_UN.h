 /* 
 * File:   NLD_UN.h
 * Author: JiMin JEONG
 * @brief  globals.h에서 분리된 네덜란드 전용 파일
 * Created 2024/06/05
 */

#ifndef NLD_UN_H
#define	NDL_UN_H

//----------   국가 Default  ----------//
 #define COUNTRY_STR "NL"
//----------   국가 전용 기능  ----------//
//#define USE_DOUBLE_PRINT	                //Double Print 기능 사용여부 선택
//----------   국가 전용 라벨 설정  ----------//

//----------   국가 전용 디스플레이 설정  ----------//

//----------   국가 전용 바코드 설정  ----------//
  #ifdef USE_QR_BARCODE
    #define USE_QR_ADDTOTAL
  #endif
//----------   국가 전용 특정업장 설정  ----------//

#endif  /* NDL_UN_H */
