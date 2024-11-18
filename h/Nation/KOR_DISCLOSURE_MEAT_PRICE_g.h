 /* 
 * File:   KOR_DISCLOSURE_MEAT_PRICE_g.h
 * Author: JiMin JEONG
 * @brief  globals.h���� �и��� �ѱ� �������� ���� ����
 * Created 2024/06/10
 */

#ifndef KOR_DISCLOSURE_MEAT_PRICE_g_H
#define	KOR_DISCLOSURE_MEAT_PRICE_g_H

//----------   ���� Default  ----------//
 #define DEALER_VER_STRING	"(g)"
 #define COUNTRY_STR "  "
 #define USE_STANDARD_TRACE_DEFAULT
 #define USE_TRACE_MEATPART_DEFAULT	            // �⺻ ������ 11�� ���
 #define USE_TRACE_MEATGRADE_MEATPRICE_DEFAULT  // USE ������ ���� ��� ���  
//----------   ���� ���� ���  ----------//
 #define USE_KOREA_VERSION_DISPLAY	            // KOREA ���� ǥ��
 #define USE_IMPORT_MEAT_PROTOCOL
 #define USE_TRACE_STANDALONE   	            // ��� �̷±��
 #define USE_GSMART_PLU         	            // Use GSMart PLU Structure(Kor)
 #define PLU_COMMODITY_LINE_NO 5	            // ��ǰ��5��
 #define USE_NUTRITION_FACT		                // Use Nutrition Fact
 #define USE_SELLBYDATETIME_ZERO	            // Sell by Date, Sell by Time�� 0�� �� �μ� ���ϴ� ���
 #define USE_CHECKING_HARMFUL_MEAT	            // ���Լ��� �������� ���(���ظ���Ʈ ���� ����)
 #define USE_TRACE_MEATPART			            // ������ ���
 #define USE_MULTI_TRACE_NO_TABLE	            // Use Multi Trace Number Table with saving
 #define USE_TRACE_MEATGRADE		            // ���� ���ǥ��(2011.7.1����)
 #define USE_TRACE_SCANNER_FOR_GRADE	        // ���ڵ� ��ĳ�ʸ� �̿��� ����� ȣ���ϴ� ���
 #define USE_CHECK_OUTOFPAPER_ON_LABEL	        // Label��忡�� Paper End üũ�ϴ� ���. ��� ����
 #define USE_TRACE_PIG_STANDALONE
 #define USE_TRACE_CALL_MEATGRADE_KEY	        // ��޺��� Ű ���. (PLU ��� �̻��)
 #define USE_DIGIT4_PRODUCT_SYMBOL              //������ ���ڵ��μ�� 4�ڸ��� ǥ��  ��(1)��(1)��(2) -> 2015�� 12�� 25�� : 5275 10�� �Ѿ�� �Ͽ� 50����. ��������ü�� ���ֳ����� ��û
 #define NOT_USE_NETSET_LOCK
 #define USE_CTS_FUNCTION                       // Calibration Traceability System	    
 #define USE_MARBLING_GRADE_PRINT
 #define USE_DNS_RESOLVER		                // USE DNS IP QUERY
 #define USE_EUCKR_UTF8_CONVERT	                // USE EUCKR to UTF8 converter
 #define USE_HTTP_CLIENT		                // USE HTTP Client
 #define USE_SEND_TRACE_PRICE	                // USE ��� ���� Structure , realtime# 8
 #define USE_SCALENAME_TRACE_SHOP_CODE 	        // USE  Scale name to Trace Shop code
 #define USE_PCS_FONT5_TO_TRACE_CODE	        // USE PCS, PLUFONT5 field for Trace CODE 
 //#define USE_DEBUG_SEND_TRACE_PRICE           // USE ��� ���� HTTP packet ����� USE rs232
 //#define USE_DISCLOSURE_MEATGRADE	            // ��ü �� PLU ��� ���� ���, ��ü�� �켱������ ����.
 #undef USE_CTS_FUNCTION			            // ��Ʈ�� CTS ��� ����
//----------   ���� ���� �� ����  ----------//
 #define USE_KOR_LABELFORMAT		            // ���� ������ ���(No.32~45 ���)
//----------   ���� ���� ���÷��� ����  ----------//
 #define FONT_CODEPAGE CODEPAGE_0949_KOR
 #define USE_KOR_FONT
//----------   ���� ���� ���ڵ� ����  ----------//

//----------   ���� ���� Ư������ ����  ----------//

#endif  /* KOR_DISCLOSURE_MEAT_PRICE_g_H */
