 /* 
 * File:   GRC_UN.h
 * Author: JiMin JEONG
 * @brief  globals.h���� �и��� �׸��� ���� ����
 * Created 2024/06/05
 */

#ifndef GRC_UN_H
#define	GRC_UN_H

//----------   ���� Default  ----------//
 #define COUNTRY_STR "GR"
//----------   ���� ���� ���  ----------//
 #define USE_SALE_PRINT_LABLE_REPORT_PRINT_TICKET
//----------   ���� ���� �� ����  ----------//

//----------   ���� ���� ���÷��� ����  ----------//
 #define FONT_CODEPAGE CODEPAGE_1253
//----------   ���� ���� ���ڵ� ����  ----------//
  #ifdef USE_QR_BARCODE
    #define USE_QR_ADDTOTAL
  #endif
//----------   ���� ���� Ư������ ����  ----------//	
			
#endif  /* GRC_UN */
