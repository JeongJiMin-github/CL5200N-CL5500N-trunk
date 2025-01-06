 /* 
 * File:   KOR_IMPORT_MEAT_I.h
 * Author: JiMin JEONG
 * @brief  globals.h���� �и��� �ѱ� ������ ��ü�̷� (������ ���� ��ü) ���� ����
 * Created 2024/06/05
 */

#ifndef KOR_IMPORT_MEAT_I_H
#define	KOR_IMPORT_MEAT_I_H

//----------   ���� Default  ----------//
 #define DEALER_VER_STRING	"(I)"
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
 #define USE_TRACE_SCANNER_FOR_GRADE	    // ���ڵ� ��ĳ�ʸ� �̿��� ����� ȣ���ϴ� ���
 #define USE_CHECK_OUTOFPAPER_ON_LABEL	    // Label��忡�� Paper End üũ�ϴ� ���. ��� ����
 #define USE_TRACE_PIG_STANDALONE
 #define USE_TRACE_CALL_MEATGRADE_KEY	    // ��޺��� Ű ���. (PLU ��� �̻��)
 #define USE_DIGIT4_PRODUCT_SYMBOL          //������ ���ڵ��μ�� 4�ڸ��� ǥ��  ��(1)��(1)��(2) -> 2015�� 12�� 25�� : 5275 10�� �Ѿ�� �Ͽ� 50����. ��������ü�� ���ֳ����� ��û
 #define USE_KOR_INGREDIENT_EXPAND	        // ���� ������ �߿��� PLU ����(��ǰ��5��) ���� ���� 1,600�� Ȯ�� (����: 200��), PLU ���� 8,000�� ����
 #define NOT_USE_NETSET_LOCK				// ���� ��ݱ�� ������� ����
 #define USE_CTS_FUNCTION 					// Calibration Traceability System
 #define USE_IMPORT_LOTNO 					// ������ �̷� ��ȣ 24�ڸ� Ȯ�� ���. �� ��� ����ϸ� �̷����� �ʱ�ȭ�Ŀ� ����ؾ���. �޸� �ʱ�ȭ!!
 #define USE_MANUAL_INPUT_IMPORT_LOTNO		// ������ ������ȣ �Է� ���
 #define USE_MARBLING_GRADE_PRINT			// �ҵ�ü �ٳ����浵
//----------   ���� ���� �� ����  ----------//
 #define USE_ENG_LABELFORMAT		        // ���� ������ ���
 #define USE_KOR_LABELFORMAT		        // ���� ������ ���(No.32~45 ���)
//----------   ���� ���� ���÷��� ����  ----------//
 #define FONT_CODEPAGE CODEPAGE_0949_KOR
 #define USE_KOR_FONT
//----------   ���� ���� ���ڵ� ����  ----------//

//----------   ���� ���� Ư������ ����  ----------//
 
#endif  /* KOR_IMPORT_MEAT_I_H */
