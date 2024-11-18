 /* 
 * File:   IDN_UN.h
 * Author: JiMin JEONG
 * @brief  globals.h에서 분리된 영공 인도네시아 전용 파일
 * Created 2024/06/05
 */

#ifndef IDN_UN_H
#define	IDN_UN_H

//----------   국가 Default  ----------//
 #define COUNTRY_STR "ID"
//----------   국가 전용 기능  ----------//
 #define USE_BARCODE_SHARP_CHECKDIGIT       //indonesia code128X 바코드에서 #캐릭터 사용시 체크섬 값 변경.
 #define USE_INDONESIA_VERSION_DISPLAY      // 인도네시아 버전 표시 CL5200N > CL5200
//----------   국가 전용 라벨 설정  ----------//

//----------   국가 전용 디스플레이 설정  ----------//
 #define USE_PRICE_TOTAL_7DIGIT			    // Total 7자리 사용
 #define USE_PRICECHECK_8DIGIT			    // Price Checkdigit(8digit)

//----------   국가 전용 바코드 설정  ----------//

//----------   국가 전용 특정업장 설정  ----------//
	
#endif /* IDN_UN_H */
