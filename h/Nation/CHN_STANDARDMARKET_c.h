 /* 
 * File:   CHN_STANDARDMARKET_c.h
 * Author: JiMin JEONG
 * @brief  globals.h���� �и��� �߱�ǥ��ȭ���� ���� ����
 * Create 2024/06/04
 */

#ifndef CHN_STANDARDMARKET_c_H
#define	CHN_STANDARDMARKET_c_H

//----------   ���� Default  ----------//
 #define DEALER_VER_STRING	"(c)"
 #define COUNTRY_STR "  "
//----------   ���� ���� ���  ----------//
 //#define USE_TRACE_STANDALONE   	        // ��� �̷±��
 //#define USE_GSMART_PLU         	        // Use GSMart PLU Structure(Kor)
 //#define PLU_COMMODITY_LINE_NO 5	        // ��ǰ��5��
 //#define USE_TRACEABILITY       	        // Use European Traceability
 //#define USE_NUTRITION_FACT		        // Use Nutrition Fact
 #define USE_TAX_RATE				        // Use Tax Rate
 #define USE_DISCOUNTED_TAX_FOR_DISCOUNTED_TOTAL
 #define USE_SELLBYDATETIME_ZERO	        // Sell by Date, Sell by Time�� 0�� �� �μ� ���ϴ� ���
//#define USE_EMART_NETWORK      	        // E-Mart Network(�ǽð� �Ǹ� ���ε�, �Ű����� ���ε�)
 #define USE_REPORT_TAX         	        // TAX Report ���
 #define USE_REPORT_PAY         	        // PAY/CHANGE Report ���
 #define USE_TRANSFER_MENU_SETTING	        // MENU Setting Up/down ��� �߰�, *���� ������ ����.
 #define USE_PROTOCOL_CL5000JR   	        // for CL5000J Protocol
 #define USE_DBG_EMART_NETWORK	            // ������ ��� :�μ���� ����

 #define USE_CHN_CART_SCALE		            // �߱� ����������(ǥ��ȭ���� �̷��ڵ���)
 #define USE_CHN_IO_BOARD		            // �߱� ���������Ͽ� I/O Board ���
 #define USE_PB_BATTERY			            // Pb Battery  ���
 #define USE_RFID_TAG_READER_RFID_522_B	    // �߱� RFID(522_B���)
//----------   ���� ���� �� ����  ----------//
 #define USE_ENG_LABELFORMAT		        // ���� ������ ���(No.1~28 ���)		// CODE 0x407F(16511bytes)
 //#define USE_KOR_LABELFORMAT		        // ���� ������ ���(No.32~45 ���)
//----------   ���� ���� ���÷��� ����  ----------//
 #define USE_CHN_FONT
 #define USE_GB2312_CHN_FONT                // simple chinese (gb2312)
//----------   ���� ���� ���ڵ� ����  ----------//

//----------   ���� ���� Ư������ ����  ----------//

#endif  /* CHN_STANDARDMARKET_c_H */