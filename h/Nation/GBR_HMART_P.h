 /* 
 * File:   GBR_HMART_P.h
 * Author: JiMin JEONG
 * @brief  globals.h���� �и��� ���� H-MART ����(�ѱ���Ʈ) ���� ����
 * Create 2024/06/04
 */

#ifndef GBR_HMART_P_H
#define	GBR_HMART_P_H

//----------   ���� Default  ----------//
 #define DEALER_VER_STRING	"(P)"
 #define COUNTRY_STR "  "
 #define USE_GRB_HMART_DEFAULT
//----------   ���� ���� ���  ----------//
 //#define USE_TRACE_STANDALONE   	            // ��� �̷±��
 //#define USE_GSMART_PLU         	            // Use GSMart PLU Structure(Kor)
 //#define PLU_COMMODITY_LINE_NO 5	            // ��ǰ��5��
 #define USE_TRACEABILITY       	            // Use European Traceability
 #define USE_NUTRITION_FACT		                // Use Nutrition Fact
 #define USE_TAX_RATE				            // Use Tax Rate
 #define USE_DISCOUNTED_TAX_FOR_DISCOUNTED_TOTAL
 #define USE_SELLBYDATETIME_ZERO	            // Sell by Date, Sell by Time�� 0�� �� �μ� ���ϴ� ���
 //#define USE_EMART_NETWORK      	            // E-Mart Network(�ǽð� �Ǹ� ���ε�, �Ű����� ���ε�)
 #define USE_REPORT_TAX         	            // TAX Report ���
 #define USE_REPORT_PAY         	            // PAY/CHANGE Report ���
 #define USE_CHECK_OUTOFPAPER_ON_LABEL			// Label��忡�� Paper End üũ�ϴ� ���. ��� ����
 #define USE_TEXT_EMPHASIS						// ~1 : Italic, ~2 : Bold, ~3 : Underline �����ϴ� ��� ���� ���� �����̽� �� ���Ͱ� ������ ���� ����ǥ�� �����Ѵ�
 #define USE_FIXED_PERCENT_KEY				    // fixed Percent discount key
 #define USE_KOREAN_IN_GBR
//----------   ���� ���� �� ����  ----------//
 #define USE_ENG_LABELFORMAT		            // ���� ������ ���(No.1~28 ���)		// CODE 0x407F(16511bytes)
 #define USE_KOR_LABELFORMAT		            // ���� ������ ���(No.32~45 ���)
//----------   ���� ���� ���÷��� ����  ----------//
 #define FONT_CODEPAGE CODEPAGE_0949_KOR
 #define USE_KOR_FONT
//----------   ���� ���� ���ڵ� ����  ----------//

//----------   ���� ���� Ư������ ����  ----------//

#endif  /* GBR_HMART_P_H */