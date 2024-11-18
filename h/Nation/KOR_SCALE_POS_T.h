 /* 
 * File:   KOR_SCALE_POS_T.h
 * Author: JiMin JEONG
 * @brief  globals.h���� �и��� �ѱ� ���ﳪ�� ���� ����
 * Created 2024/06/10
 */

#ifndef KOR_SCALE_POS_T_H
#define	KOR_SCALE_POS_T_H

//----------   ���� Default  ----------//
 #define DEALER_VER_STRING	"(T)"
 #define COUNTRY_STR "  "
 #define USE_TRACE_MEATGRADE_STANDARD_DEFAULT
//----------   ���� ���� ���  ----------//
 #define USE_KOREA_VERSION_DISPLAY	        // KOREA ���� ǥ��
 #define USE_KOR_TRACE          	        // ���ﳪ�� ��������
 #define USE_TRACE_STANDALONE   	        // ��� �̷±��
 #define USE_GSMART_PLU         	        // Use GSMart PLU Structure(Kor)
 #define PLU_COMMODITY_LINE_NO 5	        // ��ǰ��5��
 #define USE_SELLBYDATETIME_ZERO	        // Sell by Date, Sell by Time�� 0�� �� �μ� ���ϴ� ���
 #define USE_TRACE_MEATGRADE		        // ���� ���ǥ��(2011.7.1����)
 #define USE_CHECK_OUTOFPAPER_ON_LABEL	    // Label��忡�� Paper End üũ�ϴ� ���. ��� ����
  #ifdef CL5200_BP
    //  #define USE_SCALE_POS_TEST_VERSION	// ���ﳪ�� �ܺ� �߼ۿ� �׽�Ʈ ����
  #endif
 #define USE_TRACE_SCANNER_FOR_GRADE	    // ���ڵ� ��ĳ�ʸ� �̿��� ����� ȣ���ϴ� ���
 #define USE_MULTI_TRACE_NO_TABLE           // Use Multi Trace Number Table with saving
 #define USE_TRACE_CALL_MEATGRADE_KEY	    // ��޺��� Ű ���. (PLU ��� �̻��)
 #define USE_TRACE_PIG_STANDALONE
 #define USE_MARBLING_GRADE_PRINT
 #undef USE_CTS_FUNCTION 		            // ��Ʈ�� CTS ��� ���� 
//----------   ���� ���� �� ����  ----------//
 #define USE_KOR_LABELFORMAT		        // ���� ������ ���(No.32~45 ���)
//----------   ���� ���� ���÷��� ����  ----------//
 #define FONT_CODEPAGE CODEPAGE_0949_KOR
 #define USE_KOR_FONT
//----------   ���� ���� ���ڵ� ����  ----------//

//----------   ���� ���� Ư������ ����  ----------//

#endif  /* KOR_SCALE_POS_T_H */
