 /* 
 * File:   GRC_UN.h
 * Author: JiMin JEONG
 * @brief  globals.h에서 분리된 그리스 전용 파일
 * Created 2024/06/05
 */

#ifndef GRC_UN_H
#define	GRC_UN_H

//----------   국가 Default  ----------//
 #define COUNTRY_STR "GR"
//----------   국가 전용 기능  ----------//
 #define USE_SALE_PRINT_LABLE_REPORT_PRINT_TICKET
//----------   국가 전용 라벨 설정  ----------//

//----------   국가 전용 디스플레이 설정  ----------//
 #define FONT_CODEPAGE CODEPAGE_1253
//----------   국가 전용 바코드 설정  ----------//
  #ifdef USE_QR_BARCODE
    #define USE_QR_ADDTOTAL
  #endif
//----------   국가 전용 특정업장 설정  ----------//	
			
#endif  /* GRC_UN */
