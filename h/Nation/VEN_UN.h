 /* 
 * File:   VEN_UN.h
 * Author: JiMin JEONG
 * @brief  globals.h���� �и��� ���׼����� ���� ����
 * Created 2024/06/05
 */

#ifndef VEN_UN_H
#define	VEN_UN_H

//----------   ���� Default  ----------//
 #define USE_VENEZUELA_DEFAULT
 #define COUNTRY_STR "VE"
//----------   ���� ���� ���  ----------//
 #define USE_VENEZUELA_VER_DISPLAY		      // ���׼����� ���� ���� ǥ��
//----------   ���� ���� �� ����  ----------//

//----------   ���� ���� ���÷��� ����  ----------//
 #define USE_PRICE_TOTAL_9DIGIT			        // total price 9digit
 #define USE_PRICE_UNIT_9DIGIT			        // unit price 9digit
//  #define USE_PRICE_TOTAL_8DIGIT			    // total price 8digit
//  #define USE_PRICE_UNIT_8DIGIT			      // unit price 8digit

//----------   ���� ���� ���ڵ� ����  ----------//
  #ifdef USE_QR_BARCODE
    #define USE_QR_ADDTOTAL
  #endif
//----------   ���� ���� Ư������ ����  ----------//		

#endif /* VEN_UN_H */
