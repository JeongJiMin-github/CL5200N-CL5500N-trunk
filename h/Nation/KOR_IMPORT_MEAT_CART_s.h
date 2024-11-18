 /* 
 * File:   KOR_IMPORT_MEAT_CART_s.h
 * Author: JiMin JEONG
 * @brief  globals.h���� �и��� ������ ���� ��ü(CART)(Pb Bat) ���� ����
 * Created 2024/06/10
 */

#ifndef KOR_IMPORT_MEAT_CART_s_H
#define	KOR_IMPORT_MEAT_CART_s_H

//----------   ���� Default  ----------//
 #define DEALER_VER_STRING	"(s)"
 #define COUNTRY_STR "  "
 #define USE_STANDARD_TRACE_DEFAULT
 #define USE_TRACE_MEATPART_DEFAULT	        // �⺻ ������ 11�� ���
 #define USE_TRACE_MEATGRADE_STANDARD_DEFAULT
//----------   ���� ���� ���  ----------//
 #define USE_KOREA_VERSION_DISPLAY	        // KOREA ���� ǥ�� 
 #define USE_IMPORT_MEAT_PROTOCOL
 //#define USE_CHECK_SCANNING_DATA	        // SCANNER Disabel CR, Enable LF�� ���� �����ϰ� ��
 #define USE_TRACE_STANDALONE   	        // ��� �̷±��
 #define USE_GSMART_PLU         	        // Use GSMart PLU Structure(Kor)
 #define PLU_COMMODITY_LINE_NO 5	        // ��ǰ��5��
 //#define USE_TRACEABILITY       	        // Use European Traceability
 #define USE_NUTRITION_FACT		            // Use Nutrition Fact
 //#define USE_TAX_RATE				        // Use Tax Rate
 #define USE_SELLBYDATETIME_ZERO	        // Sell by Date, Sell by Time�� 0�� �� �μ� ���ϴ� ���
 //#define USE_EMART_NETWORK      	        // E-Mart Network(�ǽð� �Ǹ� ���ε�, �Ű����� ���ε�)
 //#define USE_REPORT_TAX         	        // TAX Report ���
 //#define USE_REPORT_PAY         	        // PAY/CHANGE Report ���
 //#define USE_RFID_TAG_READER		        // Use RFID Reader(�Ѹ� ENG)
 #define USE_CHECKING_HARMFUL_MEAT	        // ���Լ��� �������� ���(���ظ���Ʈ ���� ����)
 //#define USE_EXT_CHECKING_HARMFULNESS	    // ���ظ���Ʈ ���� ����
 #define USE_TRACE_MEATPART			        // ������ ���
 //#define USE_MAX_SCALE_NO_255		        // �����ȣ 255�� ���
 #define USE_MULTI_TRACE_NO_TABLE	        // Use Multi Trace Number Table with saving
 #define USE_TRACE_MEATGRADE		        // ���� ���ǥ��(2011.7.1����)
 //#define USE_TRACE_SCANNER_FOR_GRADE	    // ���ڵ� ��ĳ�ʸ� �̿��� ����� ȣ���ϴ� ���
 #define USE_CHECK_OUTOFPAPER_ON_LABEL	    // Label��忡�� Paper End üũ�ϴ� ���. ��� ����
 #define USE_CHN_IO_BOARD		            // �߱� ���������Ͽ� I/O Board ���
 #define USE_PB_BATTERY			            // Pb Battery  ���
//----------   ���� ���� �� ����  ----------//
 //#define USE_ENG_LABELFORMAT		        // ���� ������ ���
 #define USE_KOR_LABELFORMAT		        // ���� ������ ���(No.32~45 ���)
//----------   ���� ���� ���÷��� ����  ----------//

//----------   ���� ���� ���ڵ� ����  ----------//

//----------   ���� ���� Ư������ ����  ----------//

#endif  /* KOR_IMPORT_MEAT_CART_s_H */