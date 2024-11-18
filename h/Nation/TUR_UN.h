 /* 
 * File:   TUR_UN.h
 * Author: JiMin JEONG
 * @brief  globals.h���� �и��� Ƣ��Ű�� ���� ����
 * Created 2024/06/05
 */

#ifndef TUR_UN_H
#define	TUR_UN_H

//----------   ���� Default  ----------//
 #define USE_ESD_ADVANCE_DEFAULT
 #define COUNTRY_STR "TR"
//----------   ���� ���� ���  ----------//
 #define USE_LONGPRESS_KEY_OVERRIDE         // long key �� PLU �ܰ� ���� ���
//----------   ���� ���� �� ����  ----------//

//----------   ���� ���� ���÷��� ����  ----------//
 #define USE_PRICE_TOTAL_7DIGIT		          // Total 7�ڸ� ���
 #define USE_PRICE_UNIT_7DIGIT		          // Unit Price 7�ڸ� ���
 #define FONT_CODEPAGE CODEPAGE_1254
//----------   ���� ���� ���ڵ� ����  ----------//
  #ifdef USE_QR_BARCODE
    #define USE_QR_ADDTOTAL				          //QR���ڵ� �̿��� ADD TOTAL ���
    #ifdef USE_QR_ADDTOTAL
        #define USE_QR_SIZE_SETTING		      //QR������ ���� ���(1 ~ 5) : ADD-TOTAL TICKET������ ���
    #endif
  #endif
//----------   ���� ���� Ư������ ����  ----------//

#endif /* TUR_UN_H */
