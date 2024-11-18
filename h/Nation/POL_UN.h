 /* 
 * File:   POL_UN.h
 * Author: JiMin JEONG
 * @brief  globals.h���� �и��� ������ ���� ����
 * Created 2024/06/05
 */

#ifndef POL_UN_H
#define	POL_UN_H

//----------   ���� Default  ----------//
 #define USE_POLAND_DEFAULT
 #define COUNTRY_STR "PL"
//----------   ���� ���� ���  ----------//
 #define USE_EU_NUTRITIONFACT
//  #define USE_POL_NUTRITIONFACT_ENG_CAP			// dealer ���� ���
//----------   ���� ���� �� ����  ----------//

//----------   ���� ���� ���÷��� ����  ----------//
 #define FONT_CODEPAGE CODEPAGE_1250_POL
//----------   ���� ���� ���ڵ� ����  ----------//
  #ifdef USE_QR_BARCODE
    #define USE_QR_ADDTOTAL				            // QR���ڵ� �̿��� ADD TOTAL ���
  #endif
/*  		    GS1 ���ڵ� ���� (�� ���ڵ� �ߺ� ���� �ȵ�)           */
//  #define USE_RSS_BARCODE
 #define USE_ONLY_RSS_EXPAND_BARCODE
/* ���ڵ� ������ ���� (���� ������ ������ ������ ��� �ּ�ó��) */
//  #define USE_BARCODE_LEN_50
//  #define USE_BARCODE_LEN_60
//  #define USE_BARCODE_LEN_70
 #define USE_BARCODE_LEN_90                   // ���� ������ 70�ڱ��� ��� ���� (24�� 6������)
/////////////////////////////////////////////////////////////
//----------   ���� ���� Ư������ ����  ----------//

#endif  /* POL_UN_H */
