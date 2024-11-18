 /* 
 * File:   KOR_NHMART_SAFE_n.h
 * Author: JiMin JEONG
 * @brief  globals.h���� �и��� �ѱ� ������Ʈ �Ƚ��ѿ� ���� ����
 * Created 2024/06/10
 */

#ifndef KOR_NHMART_SAFE_n_H
#define	KOR_NHMART_SAFE_n_H

//----------   ���� Default  ----------//
 #define DEALER_VER_STRING	"(n)"
 #define COUNTRY_STR "  "
 #define USE_NHMART_SAFE_DEFAULT
 #define USE_TRACE_MEATGRADE_STANDARD_DEFAULT
//----------   ���� ���� ���  ----------//
 #define USE_KOREA_VERSION_DISPLAY	            // KOREA ���� ǥ�� 
 #define USE_NHMART_SAFE_MEAT		            // ���� �Ƚ� �ѿ� ���� ���
 #define USE_TRACE_STANDALONE   	            // ��� �̷±��
 #define USE_GSMART_PLU         	            // Use GSMart PLU Structure(Kor)
 #define PLU_COMMODITY_LINE_NO 5	            // ��ǰ��5��
 //#define USE_TRACEABILITY       	            // Use European Traceability
 #define USE_NUTRITION_FACT		                // Use Nutrition Fact
 //#define USE_TAX_RATE				            // Use Tax Rate
 #define USE_SELLBYDATETIME_ZERO	            // Sell by Date, Sell by Time�� 0�� �� �μ� ���ϴ� ���
 //#define USE_EMART_NETWORK      	            // E-Mart Network(�ǽð� �Ǹ� ���ε�, �Ű����� ���ε�)
 //#define USE_REPORT_TAX         	            // TAX Report ���
 //#define USE_REPORT_PAY         	            // PAY/CHANGE Report ���
 #define USE_KOR_TRACE_999      	            // ���� ��ü������ 999��, ���� ������
 //#define USE_TRACE_FLAG_0_CHECK		        // ��ü�����÷��� 0�϶� ��ü���� ������ üũ
 #define USE_TRACE_MEATGRADE		            // ���� ���ǥ��(2011.7.1����)
 #define USE_CHECK_OUTOFPAPER_ON_LABEL	        // Label��忡�� Paper End üũ�ϴ� ���. ��� ����
 #define USE_CHECK_INDIV_INFO		            //��ü���� ��ȿ�� Ȯ�� �������� �߰�.
 #define USE_NHSAFE_PIG_TRACE
//----------   ���� ���� �� ����  ----------//
 //#define USE_ENG_LABELFORMAT		            // ���� ������ ���
 #define USE_KOR_LABELFORMAT		            // ���� ������ ���(No.32~45 ���)
//----------   ���� ���� ���÷��� ����  ----------//

//----------   ���� ���� ���ڵ� ����  ----------//

//----------   ���� ���� Ư������ ����  ----------//

#endif  /* KOR_NHMART_SAFE_n_H */
