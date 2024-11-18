 /* 
 * File:   MANAGE_INVENTORY_i.h
 * Author: JiMin JEONG
 * @brief  globals.h���� �и��� ���� ������ �� ���� ��� ���� ����
 * Create 2024/06/04
 */

#ifndef MANAGE_INVENTORY_i_H
#define	MANAGE_INVENTORY_i_H

//----------   ���� Default  ----------//
 #define DEALER_VER_STRING	"(i)"
 #define COUNTRY_STR "  "
 #define USE_MANAGE_INVENTORY_DEFAULT
//----------   ���� ���� ���  ----------//
 #define USE_KOREA_VERSION_DISPLAY	    // KOREA ���� ǥ�� 
 //#define USE_CHECK_SCANNING_DATA	    // SCANNER Disabel CR, Enable LF�� ���� �����ϰ� ��
 #define USE_GSMART_PLU         	    // Use GSMart PLU Structure(Kor)
 #define PLU_COMMODITY_LINE_NO 5	    // ��ǰ��5��
 #define USE_SELLBYDATETIME_ZERO	    // Sell by Date, Sell by Time�� 0�� �� �μ� ���ϴ� ���
 //#define USE_RFID_TAG_READER		    // Use RFID Reader(�Ѹ� ENG)
 //#define USE_MAX_SCALE_NO_255		    // �����ȣ 255�� ���
 //#define USE_TRACE_SCANNER_FOR_GRADE	// ���ڵ� ��ĳ�ʸ� �̿��� ����� ȣ���ϴ� ���
 #define USE_INVENTORY_LABEL		    //�������� ���� ���. ���԰�, �����߷� ���� ���� ���
//----------   ���� ���� �� ����  ----------//
 //#define USE_ENG_LABELFORMAT		    // ���� ������ ���
 #define USE_KOR_LABELFORMAT		    // ���� ������ ���(No.32~45 ���)
//----------   ���� ���� ���÷��� ����  ----------//

//----------   ���� ���� ���ڵ� ����  ----------//

//----------   ���� ���� Ư������ ����  ----------//

 #endif /* MANAGE_INVENTORY_i_H */