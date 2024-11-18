 /* 
 * File:   MEX_UN.h
 * Author: JiMin JEONG
 * @brief  globals.h���� �и��� �߽��� ���� ����
 * Created 2024/06/05
 */

#ifndef MEX_UN_H
#define	MEX_UN_H

//----------   ���� Default  ----------//
 #define COUNTRY_STR "MX"
//----------   ���� ���� ���  ----------//
 #define USE_BAKERY_FUNCTION
  #ifdef USE_BAKERY_FUNCTION
    #define USE_SEND_TICKET_DATA
  #endif
 #define USE_LOT_NUMBER_FUNCTION     // LOT Number ��� ����
//----------   ���� ���� �� ����  ----------//

//----------   ���� ���� ���÷��� ����  ----------//

//----------   ���� ���� ���ڵ� ����  ----------//
/*		 GS1 ���ڵ� ���� (�� ���ڵ� �ߺ� ���� �ȵ�)           */
//  #define USE_RSS_BARCODE
// #define USE_ONLY_RSS_EXPAND_BARCODE
/* ���ڵ� ������ ���� (���� ������ ������ ������ ��� �ּ�ó��) */
//  #define USE_BARCODE_LEN_50
//  #define USE_BARCODE_LEN_60
 #define USE_BARCODE_LEN_70
//  #define USE_BARCODE_LEN_90
/////////////////////////////////////////////////////////////
//----------   ���� ���� Ư������ ����  ----------//
	
#endif  /* MEX_UN_H */
