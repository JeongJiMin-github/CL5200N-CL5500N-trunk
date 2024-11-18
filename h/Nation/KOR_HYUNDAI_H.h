 /* 
 * File:   KOR_HYUNDAI_H.h
 * Author: JiMin JEONG
 * @brief  globals.h���� �и��� �ѱ� �����ȭ�� ���� ����
 * Created 2024/06/10
 */

#ifndef KOR_HYUNDAI_H_H
#define	KOR_HYUNDAI_H_H

//----------   ���� Default  ----------//
 #define DEALER_VER_STRING	"(H)"
 #define COUNTRY_STR "  "
 #define USE_HYUNDAI_DEFAULT
 #define USE_TRACE_MEATPART_DEFAULT	            // �⺻ ������ 11�� ���
 #define USE_TRACE_MEATGRADE_STANDARD_DEFAULT
//----------   ���� ���� ���  ----------//
 #define USE_KOREA_VERSION_DISPLAY	            // KOREA ���� ǥ�� 
 #define USE_IMPORT_MEAT_PROTOCOL
 #define USE_TRACE_STANDALONE   	            // ��� �̷±��
 #define USE_GSMART_PLU         	            // Use GSMart PLU Structure(Kor)
 #define PLU_COMMODITY_LINE_NO 5	            // ��ǰ��5��
 #define USE_NUTRITION_FACT		                // Use Nutrition Fact
 #define USE_SELLBYDATETIME_ZERO	            // Sell by Date, Sell by Time�� 0�� �� �μ� ���ϴ� ���
 #define USE_CHECKING_HARMFUL_MEAT	            // ���Լ��� �������� ���(���ظ���Ʈ ���� ����)
 #define USE_MULTI_TRACE_NO_TABLE	            // Use Multi Trace Number Table with saving
 #define USE_TRACE_MEATPART			            // ������ ���
 #define USE_TRACE_MEATGRADE		            // ���� ���ǥ��(2011.7.1����)
 #define USE_PLU_WITH_TRACE_PROTOCOL	        // PLU ���۽� ���� �̷� ���� ���� ���.
 // #define USE_S_PRICE_AS_BASE_PRICE	        // Special price�� ���� �������� ���, (Ticket 1288)
 // #define USE_CHECK_CHANGE_REASON	            // �ܰ� ����� ���� �ڵ� �Է� ���.
 #define USE_CHECK_OUTOFPAPER_ON_LABEL	        // Label��忡�� Paper End üũ�ϴ� ���. ��� ����
 #define USE_CHECK_INDIV_INFO		            // ��ü���� ��ȿ�� Ȯ�� �������� �߰�.
 #define USE_TRACE_SCANNER_FOR_GRADE	        // ���ڵ� ��ĳ�ʸ� �̿��� ����� ȣ���ϴ� ���
 #define USE_SCANNER_FOR_ECO_FRIENDLY	        // ģȯ�� ���� ��ǰ ��ĵ, ��ȸ ���.
 #define USE_TRACE_PIG_STANDALONE
 #define USE_MARBLING_GRADE_PRINT
 #define USE_SLAUGHTERHOUSE_LENG_100            // ������ 100 + 1 (null) byte ���
 #define USE_HYUNDAI_MEAT_FUNCTION	            // ���� ��ȭ�� ��� ���
  #ifdef USE_HYUNDAI_MEAT_FUNCTION
    #define USE_REGIST_DISPOSAL		            // ���� ���� ��� ��ϱ��
    #define USE_HYUNDAI_SET_FUNCTION	        // ���� ���� ��Ʈ ��ǰ ��� ��� 
    #define USE_HYUNDAI_CHECK_REASON_CODE	    // ���� ���� �����ڵ� ���
  #endif
//----------   ���� ���� �� ����  ----------//
 // #define USE_KOR_LABELFORMAT		            // ���� ������ ���(No.32~45 ���)
//----------   ���� ���� ���÷��� ����  ----------//
 #define FONT_CODEPAGE CODEPAGE_0949_KOR
 #define USE_KOR_FONT
 #define USE_PRICE_TOTAL_7DIGIT		            // Total 7�ڸ� ���
 #define USE_PRICE_UNIT_7DIGIT	                // Unit Price 7�ڸ� ���
//----------   ���� ���� ���ڵ� ����  ----------//

//----------   ���� ���� Ư������ ����  ----------//

#endif  /* KOR_HYUNDAI_H_H */
