 /* 
 * File:   KOR_GSMART_TRACE_M.h
 * Author: JiMin JEONG
 * @brief  globals.h���� �и��� �ѱ� GS��Ʈ TRACE ���� ����
 * Created 2024/06/10
 */

#ifndef KOR_GSMART_TRACE_M_H
#define	KOR_GSMART_TRACE_M_H

//----------   ���� Default  ----------//
 #define DEALER_VER_STRING	"(M)"
 #define COUNTRY_STR "  "
 #define USE_GSMART_TRACE_DEFAULT
 #define USE_TRACE_MEATGRADE_STANDARD_DEFAULT
//----------   ���� ���� ���  ----------//
 #define USE_KOREA_VERSION_DISPLAY	        // KOREA ���� ǥ��
 #define USE_GSMART_TRACE_FUNCTION	        // GS(���) ���� ��� 
 #define USE_TRACE_STANDALONE   	        // ��� �̷±��
 #define USE_GSMART_PLU         	        // Use GSMart PLU Structure(Kor)
 #define PLU_COMMODITY_LINE_NO 8	        // ��ǰ��8�� (GS Mart PLU)
 //#define USE_TRACEABILITY       	        // Use European Traceability
 #define USE_NUTRITION_FACT		            // Use Nutrition Fact
 //#define USE_TAX_RATE				        // Use Tax Rate
 //#define USE_SELLBYDATETIME_ZERO	        // Sell by Date, Sell by Time�� 0�� �� �μ� ���ϴ� ���
 //#define USE_EMART_NETWORK      	        // E-Mart Network(�ǽð� �Ǹ� ���ε�, �Ű����� ���ε�)
 //#define USE_REPORT_TAX         	        // TAX Report ���
 //#define USE_REPORT_PAY         	        // PAY/CHANGE Report ���
 #define USE_MULTI_TRACE_NO_TABLE	        // Use Multi Trace Number Table with  saving
 #define USE_TRACE_MEATGRADE		        // ���� ���ǥ��(2011.7.1����)
 #define USE_CHECK_OUTOFPAPER_ON_LABEL	    // Label��忡�� Paper End üũ�ϴ� ���. ��� ����
 #define USE_GSMART_PIG_TRACE
 #define USE_PRICE_CHANGE_PROHIBITED
 #define USE_KOREAN_ERROR_MSG
 #undef USE_CTS_FUNCTION 		            // ��Ʈ�� CTS ��� ����
//----------   ���� ���� �� ����  ----------//
 //#define USE_ENG_LABELFORMAT		        // ���� ������ ���
 #define USE_KOR_LABELFORMAT		        // ���� ������ ���(No.32~45 ���)
//----------   ���� ���� ���÷��� ����  ----------//
 #define FONT_CODEPAGE CODEPAGE_0949_KOR
 #define USE_KOR_FONT
//----------   ���� ���� ���ڵ� ����  ----------//
 #define USE_GSMART_BARCODE			        // GS ���� ���ڵ�
//----------   ���� ���� Ư������ ����  ----------//

#endif  /* KOR_GSMART_TRACE_M_H */
