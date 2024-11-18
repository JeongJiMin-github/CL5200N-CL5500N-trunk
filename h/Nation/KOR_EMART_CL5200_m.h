 /* 
 * File:   KOR_EMART_CL5200_m.h
 * Author: JiMin JEONG
 * @brief  globals.h���� �и��� CL5200 �̸�Ʈ (V3.03.7���� ��� ����) - �̸�Ʈ�� ��� V3.03.7���� (E)������  (m)������ �����Ͽ� (E)���� �� ���� ���� ����
 * Create 2024/06/04
 */

#ifndef KOR_EMART_CL5200_m_H
#define	KOR_EMART_CL5200_m_H

//----------   ���� Default  ----------//
 #define DEALER_VER_STRING	"(m)"
 #define COUNTRY_STR "  "
 #define USE_EMART_DEFAULT
 #define USE_TRACE_MEATGRADE_EMART_DEFAULT
//----------   ���� ���� ���  ----------//
 #define USE_KOREA_VERSION_DISPLAY	        // KOREA ���� ǥ�� 
 #define USE_TRACE_STANDALONE   	        // ��� �̷±��
 #define USE_GSMART_PLU         	        // Use GSMart PLU Structure(Kor)
 #define PLU_COMMODITY_LINE_NO 5	        // ��ǰ��5��
 //#define USE_NUTRITION_FACT		        // Use Nutrition Fact
 // #define USE_SELLBYDATETIME_ZERO	        // Sell by Date, Sell by Time�� 0�� �� �μ� ���ϴ� ���
 //#define USE_CHECK_OUTOFPAPER_ON_LABEL	// Label��忡�� Paper End üũ�ϴ� ���. ��� ����
 #define USE_KOREA_CL3000_STANDARD_KEY
 //#define USE_SELLBYDATETIME_ZERO	        // Sell by Date, Sell by Time�� 0�� �� �μ� ���ϴ� ���
 #define USE_EMART_NETWORK      	        // E-Mart Network(�ǽð� �Ǹ� ���ε�, �Ű����� ���ε�)
 #define USE_DBG_EMART_NETWORK	            // Use Debug Network(without print)
 #define USE_KEEPING_CONNETION  	        // ���۵����Ͱ� ��� ���� �� Remote�� Connection ����
 #define USE_MULTI_TRACE_NO_TABLE	        // Use Multi Trace Number Table with saving
 #define USE_BARCODE_UPRICE_OVER	        // Unit Price Barcode�� �ڸ��� �ʰ��� ��� 'U'�� 0���� �μ�
 #define USE_KOR_SLAUGHT_CODE_4DIGIT	    // ������ �ڵ� 4�ڸ� ���(�̸�Ʈ ����)
 #define USE_CHECKING_HARMFUL_MEAT	        // ���Լ��� �������� ���(���ظ���Ʈ ���� ����)
 #define USE_FIXED_PLUTYPE			        // Use Fixed PLU Type(�߷�ȥ��, ����ȥ��, �߷�, ����)
 #define USE_TRACE_MEATGRADE		        // ���� ���ǥ��(2011.7.1����)
 #define USE_TRACE_SCANNER_FOR_GRADE	    // ���ڵ� ��ĳ�ʸ� �̿��� ����� ȣ���ϴ� ���
 #define USE_EMART_MEATGRADE_PRINT		    // �̸�Ʈ ���ǥ�� �μ� ���
 #define USE_TRACE_CALL_MEATGRADE_KEY	    // ��޺��� Ű ���. (PLU ��� �̻��)
 #define USE_CHECK_OUTOFPAPER_ON_LABEL	    // Label��忡�� Paper End üũ�ϴ� ���. ��� ����
 #define USE_CONTROL_CHANGING_PRICE	        // �̸�Ʈ(2012) �Ű� ���� ���� ���, USE_S_PRICE_AS_BASE_PRICE�� ���� ����ؾ� ��.
 #define USE_DIVIDING_FUNCTION		        // �ɰ��� ��� ���
 #define USE_EMART_PIG_TRACE
 //#define USE_REALTIME_CHECK_INTERVAL      // parameter 765 �� ��Ʈ��ũ üũ ���� ����. �׽�Ʈ �� ��ü ���� ����, ��ü ������ �ش� ������ ����
 #define USE_EMART_MINCED_MEAT              // ������ �����µ� ����
 #define USE_EMART_PRODUCTION_KEY           // ������Ű  ��뿩�� ����
 #undef USE_CTS_FUNCTION			        // ��Ʈ�� CTS ��� ����
//----------   ���� ���� �� ����  ----------//
 // #define USE_KOR_LABELFORMAT		        // ���� ������ ���(No.32~45 ���)
  #define USE_EMART_BACK_LABEL		        // �̸�Ʈ �Ĺ�� ��� ���. Protocol 90->100 bytes
//----------   ���� ���� ���÷��� ����  ----------//
 #define FONT_CODEPAGE CODEPAGE_0949_KOR
 #define USE_KOR_FONT
//----------   ���� ���� ���ڵ� ����  ----------//

//----------   ���� ���� Ư������ ����  ----------//

#endif  /* KOR_EMART_CL5200_m_H*/