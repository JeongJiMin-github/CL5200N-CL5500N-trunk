 /* 
 * File:   KOR_CL5200_k.h
 * Author: JiMin JEONG
 * @brief  globals.h���� �и��� CL5200 ���� ���� (�̷°��� ��� ����) ���� ����
 * Create 2024/06/04
 */

#ifndef KOR_CL5200_k_H
#define	KOR_CL5200_k_H

//----------   ���� Default  ----------//
 #define DEALER_VER_STRING	"(k)"
 #define COUNTRY_STR "  "
 #define USE_STANDARD_TRACE_DEFAULT
//----------   ���� ���� ���  ----------//
 #define USE_KOREA_VERSION_DISPLAY	        // KOREA ���� ǥ�� 
 #define USE_GSMART_PLU         	        // Use GSMart PLU Structure(Kor)
 #define PLU_COMMODITY_LINE_NO 5	        // ��ǰ��5��
 #define USE_NUTRITION_FACT		            // Use Nutrition Fact
 #define USE_SELLBYDATETIME_ZERO	        // Sell by Date, Sell by Time�� 0�� �� �μ� ���ϴ� ���
 #define USE_CHECK_OUTOFPAPER_ON_LABEL	    // Label��忡�� Paper End üũ�ϴ� ���. ��� ����
 // #define USE_KOREA_CL5200_STANDARD_KEY
 #define USE_KOREA_CL3000_STANDARD_KEY
 #define NOT_USE_TRACE_CL5200
 #define USE_KOR_INGREDIENT_EXPAND	        // ���� ������ �߿��� PLU ����(��ǰ��5��) ���� ���� 16,000�� Ȯ�� (����: 200��), PLU ���� 8,000�� ����
//----------   ���� ���� �� ����  ----------//
 #define USE_KOR_LABELFORMAT		        // ���� ������ ���(No.32~45 ���)
 #define USE_ENG_LABELFORMAT                // �⺻ �� 1~28�� �߰�
//----------   ���� ���� ���÷��� ����  ----------//
 #define FONT_CODEPAGE CODEPAGE_0949_KOR
 #define USE_KOR_FONT
//----------   ���� ���� ���ڵ� ����  ----------//

//----------   ���� ���� Ư������ ����  ----------//

#endif  /* KOR_CL5200_k_H */