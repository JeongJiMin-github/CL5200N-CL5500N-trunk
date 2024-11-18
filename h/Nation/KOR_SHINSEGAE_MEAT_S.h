 /* 
 * File:   KOR_SHINSEGAE_MEAT_S.h
 * Author: JiMin JEONG
 * @brief  globals.h���� �и��� �ż��� ��ȭ�� ���� ����
 * Created 2024/06/10
 */

#ifndef KOR_SHINSEGAE_MEAT_S_H
#define	KOR_SHINSEGAE_MEAT_S_H

//----------   ���� Default  ----------//
 #define DEALER_VER_STRING	"(S)"
 #define COUNTRY_STR "  "
 #define USE_SHINSEGAE_DEFAULT
 #define USE_STANDARD_TRACE_DEFAULT
 #define USE_TRACE_MEATPART_DEFAULT	        // �⺻ ������ 11�� ���
 #define USE_TRACE_MEATGRADE_STANDARD_DEFAULT
//----------   ���� ���� ���  ----------//
 #define USE_KOREA_VERSION_DISPLAY	        // KOREA ���� ǥ�� 
 #define USE_IMPORT_MEAT_PROTOCOL
 #define USE_TRACE_STANDALONE   	        // ��� �̷±��
 #define USE_GSMART_PLU         	        // Use GSMart PLU Structure(Kor)
 #define PLU_COMMODITY_LINE_NO 5	        // ��ǰ��5��
 #define USE_NUTRITION_FACT		            // Use Nutrition Fact
 #define USE_SELLBYDATETIME_ZERO	        // Sell by Date, Sell by Time�� 0�� �� �μ� ���ϴ� ���
 #define USE_CHECKING_HARMFUL_MEAT	        // ���Լ��� �������� ���(���ظ���Ʈ ���� ����)
 #define USE_TRACE_MEATPART			        // ������ ���
 #define USE_MULTI_TRACE_NO_TABLE	        // Use Multi Trace Number Table with saving
 #define USE_TRACE_MEATGRADE		        // ���� ���ǥ��(2011.7.1����)
 #define USE_CHECK_OUTOFPAPER_ON_LABEL	    // Label��忡�� Paper End üũ�ϴ� ���. ��� ����
 //#define USE_MEATGRADE_NO_TEXT		    // ��� �̷� ��� ǥ�� ��� ��  ��� : ǥ�� ����
 #define USE_TRACE_PIG_STANDALONE
 #define USE_TRACE_CALL_MEATGRADE_KEY       // ��޺��� Ű ���. (PLU ��� �̻��)
 #define USE_MARBLING_GRADE_PRINT
 //#define USE_KOR_INGREDIENT_EXPAND	    // ���� ������ �߿��� PLU ����(��ǰ��5��) ���� ���� 16,000�� Ȯ�� (����: 200��), PLU ���� 8,000�� ����
 #undef USE_CTS_FUNCTION			        // ��Ʈ�� CTS ��� ����
//----------   ���� ���� �� ����  ----------//
 #define USE_KOR_LABELFORMAT		        // ���� ������ ���(No.32~45 ���)
//----------   ���� ���� ���÷��� ����  ----------//
 #define FONT_CODEPAGE CODEPAGE_0949_KOR
 #define USE_KOR_FONT
  #if defined(USE_CL5500_DISPLAY)           // [ CL5500N ]  CL5500 Display
    /* CL5500N ���� 7�ڸ� ǥ�� ���� */
    #define USE_PRICE_UNIT_7DIGIT		    // Unit Price 7�ڸ� ���
    #define USE_PRICE_TOTAL_7DIGIT		    // Total 7�ڸ� ���
  #elif defined USE_CL5200_DISPLAY          // [ CL5200N ]  CL5200 Display ����
    /* CL5200N ���� 7�ڸ� ǥ�� ���� ���� */
  #endif
//----------   ���� ���� ���ڵ� ����  ----------//

//----------   ���� ���� Ư������ ����  ----------//

#endif  /* KOR_SHINSEGAE_MEAT_S_H */