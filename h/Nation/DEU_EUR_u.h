 /* 
 * File:   DEU_EUR_u.h
 * Author: JiMin JEONG
 * @brief  globals.h���� �и��� ���� ����Ʈ ��� ���� ���� ����
 */

#ifndef DEU_EUR_u_H
#define	DEU_EUR_u_H

//----------   ���� Default  ----------//
 #define USE_GERMANY_DEFAULT				// PARAMETER SETTING
 #define COUNTRY_STR "DE"
//----------   ���� ���� ���  ----------//
 #define USE_PRINT_GROSS_AMOUNT_FOR_TAX	    // TAX Report���� GROSS/NET AMOUNT ǥ��(for DEU)
 #define NOT_USE_SOME_KEYS_IN_SELFSERVICE
 #define USE_GERMANY_TARE_SCANNER		    // Scanner�� Tare ��� �߰�
//----------   ���� ���� �� ����  ----------//

//----------   ���� ���� ���÷��� ����  ----------//

//----------   ���� ���� ���ڵ� ����  ----------//
/*		 GS1 ���ڵ� ���� (�� ���ڵ� �ߺ� ���� �ȵ�)           */
//  #define USE_RSS_BARCODE
 #define USE_ONLY_RSS_EXPAND_BARCODE
/* ���ڵ� ������ ���� (���� ������ ������ ������ ��� �ּ�ó��) */
//  #define USE_BARCODE_LEN_50
//  #define USE_BARCODE_LEN_60
//  #define USE_BARCODE_LEN_70
 #define USE_BARCODE_LEN_90
/////////////////////////////////////////////////////////////
//----------   ���� ���� Ư������ ����  ----------//

#endif /* DEU_EUR_u_H */