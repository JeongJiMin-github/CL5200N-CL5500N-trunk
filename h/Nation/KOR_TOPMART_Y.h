 /* 
 * File:   KOR_TOPMART_Y.h
 * Author: JiMin JEONG
 * @brief  globals.h���� �и��� ��������  TOP mart ���� ����
 * Create 2024/06/04
 */

#ifndef KOR_TOPMART_Y_H
#define	KOR_TOPMART_Y_H

//----------   ���� Default  ----------//
 #define DEALER_VER_STRING	"(Y)"
 #define COUNTRY_STR "  "
 #define USE_TOPMART_DEFAULT
 #define USE_TRACE_MEATGRADE_STANDARD_DEFAULT
//----------   ���� ���� ���  ----------//
 #define USE_TOPMART_STRUCTURE
 #define USE_TOPMART_IMPORTLOTNO
 #define USE_TRACE_1000QTY
 #define USE_GSMART_TRACE_FUNCTION	        // GS(���) ���� ���
 #define USE_TRACE_STANDALONE   	        // ��� �̷±��
 #define USE_GSMART_PLU         	        // Use GSMart PLU Structure(Kor)
 #define PLU_COMMODITY_LINE_NO 8	        // ��ǰ��8�� (GS Mart PLU)
 #define USE_NUTRITION_FACT		            // Use Nutrition Fact
 #define USE_MULTI_TRACE_NO_TABLE	        // Use Multi Trace Number Table with saving
 #define USE_TRACE_MEATGRADE		        // ���� ���ǥ��(2011.7.1����)
 #define USE_CHECK_OUTOFPAPER_ON_LABEL	    // Label��忡�� Paper End üũ�ϴ� ���. ��� ����
 #define USE_TOPMART_ASK_INDIV_INFO         //�������� ��û ���
 #define USE_MARBLING_GRADE_PRINT
 #define USE_TOPMART_BARCODE_FUNCTION 
 #undef USE_CTS_FUNCTION			        // ��Ʈ�� CTS ��� ����
//----------   ���� ���� �� ����  ----------//
 #define USE_KOR_LABELFORMAT		        // ���� ������ ���(No.32~45 ���)
//----------   ���� ���� ���÷��� ����  ----------//
 #define FONT_CODEPAGE CODEPAGE_0949_KOR
 #define USE_KOR_FONT
//----------   ���� ���� ���ڵ� ����  ----------//
 #define USE_GSMART_BARCODE			        // GS ���� ���ڵ�
/*		 GS1 ���ڵ� ���� (�� ���ڵ� �ߺ� ���� �ȵ�)           */
//  #define USE_RSS_BARCODE
 #define USE_ONLY_RSS_EXPAND_BARCODE
/* ���ڵ� ������ ���� (���� ������ ������ ������ ��� �ּ�ó��) */
//  #define USE_BARCODE_LEN_50
//  #define USE_BARCODE_LEN_60
//  #define USE_BARCODE_LEN_70
//  #define USE_BARCODE_LEN_90
/////////////////////////////////////////////////////////////
 #define USE_BARCODE_LEN_50_DES_20	        // Barcode length 60, descript length 20
//----------   ���� ���� Ư������ ����  ----------//

#endif  /* KOR_TOPMART_Y_H */
