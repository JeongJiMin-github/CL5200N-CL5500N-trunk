 /* 
 * File:   KOR_NHMART_N.h
 * Author: JiMin JEONG
 * @brief  globals.h���� �и��� �ѱ� ������Ʈ ���� ����
 * Created 2024/06/10
 */

#ifndef KOR_NHMART_N_H
#define	KOR_NHMART_N_H

//----------   ���� Default  ----------//
 #define USE_NHMART_DEFAULT
 #define USE_TRACE_MEATGRADE_STANDARD_DEFAULT
 #define DEALER_VER_STRING	"(N)"
 #define COUNTRY_STR "  "
//----------   ���� ���� ���  ----------//
 #define USE_KOREA_VERSION_DISPLAY	            // KOREA ���� ǥ��
 #define USE_TRACE_STANDALONE   	            // ��� �̷±��
 #define USE_GSMART_PLU         	            // Use GSMart PLU Structure(Kor)
 #define PLU_COMMODITY_LINE_NO 5	            // ��ǰ��5��
 #define USE_NUTRITION_FACT		                // Use Nutrition Fact
 #define USE_SELLBYDATETIME_ZERO	            // Sell by Date, Sell by Time�� 0�� �� �μ� ���ϴ� ���
 #define USE_KOR_LABELFORMAT		            // ���� ������ ���(No.32~45 ���)
 #define USE_KOR_TRACE_999      	            // ���� ��ü������ 999��, ���� ������
 #define USE_TRACE_FLAG_0_CHECK		            // ��ü�����÷��� 0�϶� ��ü���� ������ üũ
 #define USE_TRACE_MEATGRADE		            // ���� ���ǥ��(2011.7.1����)
 #define USE_CHECK_OUTOFPAPER_ON_LABEL	        // Label��忡�� Paper End üũ�ϴ� ���. ��� ����
 #define USE_NHMART_PIG_TRACE
 #define USE_EDIT_TOTALPRICE
 #define USE_SETPRODUCT_SALE		            // ��Ʈ��ǰ �Ǹ� ���
 #define USE_PRICE_CHANGEABLE_CHECK	            // ���� ���� ���� ������ üũ
 #define USE_MARBLING_GRADE_PRINT
 #undef USE_CTS_FUNCTION 		                // ��Ʈ�� CTS ��� ����
//----------   ���� ���� �� ����  ----------//

//----------   ���� ���� ���÷��� ����  ----------//
 #define FONT_CODEPAGE CODEPAGE_0949_KOR
 #define USE_KOR_FONT
//----------   ���� ���� ���ڵ� ����  ----------//

//----------   ���� ���� Ư������ ����  ----------//

#endif  /* KOR_NHMART_N_H */
