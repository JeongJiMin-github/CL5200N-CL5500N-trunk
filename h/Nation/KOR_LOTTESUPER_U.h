 /* 
 * File:   KOR_LOTTESUPER_U.h
 * Author: JiMin JEONG
 * @brief  globals.h���� �и��� Lotte Super(���� ���������� �Ļ�) ���� ����
 * Created 2024/06/10
 */

#ifndef KOR_LOTTESUPER_U_H
#define	KOR_LOTTESUPER_U_H

//----------   ���� Default  ----------//
 #define DEALER_VER_STRING	"(U)"
 #define COUNTRY_STR "  "
 #define USE_LOTTESUPER_DEFAULT
 #define USE_TRACE_MEATPART_DEFAULT	            // �⺻ ������ 11�� ���
 #define USE_TRACE_MEATGRADE_STANDARD_DEFAULT
//----------   ���� ���� ���  ----------//
 #define USE_KOREA_VERSION_DISPLAY	            // KOREA ���� ǥ�� 
 #define USE_IMPORT_MEAT_PROTOCOL
 #define USE_TRACE_STANDALONE   	            // ��� �̷±��
 #define USE_GSMART_PLU         	            // Use GSMart PLU Structure(Kor)
 #define PLU_COMMODITY_LINE_NO 8	            // ��ǰ��5��
 #define USE_SELLBYDATETIME_ZERO	            // Sell by Date, Sell by Time�� 0�� �� �μ� ���ϴ� ���
 #define USE_CHECKING_HARMFUL_MEAT	            // ���Լ��� �������� ���(���ظ���Ʈ ���� ����)
 #define USE_TRACE_MEATPART			            // ������ ���
 #define USE_MULTI_TRACE_NO_TABLE	            // Use Multi Trace Number Table with saving
 #define USE_TRACE_MEATGRADE		            // ���� ���ǥ��(2011.7.1����)
 #define USE_CHECK_OUTOFPAPER_ON_LABEL	        // Label��忡�� Paper End üũ�ϴ� ���. ��� ����
 #define USE_TRACE_PIG_STANDALONE
 #define USE_MARBLING_GRADE_PRINT
  #ifdef USE_MARBLING_GRADE_PRINT
    #define USE_EXPAND_MARBLING_LIST            // ���� ��޸���Ʈ�� ���� �а� �μ�.
  #endif
 #define USE_LOTTEMART_BARCODE_FUNCTION
 #define USE_LOTTEMART_ITEMCODE13   		    // Ext.barcode �ʵ带 13�ڸ� ITEMCODE�� ����ϴ� ���. ( �嵥������ ��� ��ǰ�������� �Ǹ��ڵ带 �޴� �뵵�� Ȱ�� )
 #define USE_LOTTESUPER_BARCODE_FUNCTION	    // GS1 ���ڵ� �ϴ� HRI ������ ǥ���ϴ� ������ ������ �μ��ϴ� �ʵ� ���� ��� ��� ����
 #define USE_LOTTESUPER_PROHIBIT_CHANGE_PRICE   // �Ե����� ��û �Ű������� ��� (���ϸ�忡�� ���� Ű ��� ����, 1110, 1120 Menu ��й�ȣ�� ����)
 #undef USE_CTS_FUNCTION 		  				// ��Ʈ�� CTS ��� ����
//----------   ���� ���� �� ����  ----------//
 #define USE_KOR_LABELFORMAT		            // ���� ������ ���(No.32~45 ���)
//----------   ���� ���� ���÷��� ����  ----------//
 #define FONT_CODEPAGE CODEPAGE_0949_KOR
 #define USE_KOR_FONT
//----------   ���� ���� ���ڵ� ����  ----------//
/*		 GS1 ���ڵ� ���� (�� ���ڵ� �ߺ� ���� �ȵ�)           */
//  #define USE_RSS_BARCODE
 #define USE_ONLY_RSS_EXPAND_BARCODE
/* ���ڵ� ������ ���� (���� ������ ������ ������ ��� �ּ�ó��) */
//  #define USE_BARCODE_LEN_50
//  #define USE_BARCODE_LEN_60
//  #define USE_BARCODE_LEN_70
 #define USE_BARCODE_LEN_90
/////////////////////////////////////////////////////////////
//----------   ���� ���� Ư������ ����  ----------//
 
#endif  /* KOR_LOTTESUPER_U_H */