 /* 
 * File:   CHN_TRACE_STANDARD_F.h
 * Author: JiMin JEONG
 * @brief  globals.h���� �и��� chinese trace standard (gb2312) <= chn E-Mart ���� ����
 * Create 2024/06/04
 */

#ifndef CHN_TRACE_STANDARD_F_H
#define	CHN_TRACE_STANDARD_F_H

//----------   ���� Default  ----------//
  #ifdef USE_CHN_CERTIFICATION
    #define DEALER_VER_STRING	"   "
  #else
    #define DEALER_VER_STRING	"(F)"
  #endif
 #define COUNTRY_STR "  "
 #define USE_CHINA_DEFAULT                  // PARAMETER SETTING
 #define USE_CHN_TRACE_DEFAULT 				// PLU 1000 ea with trace code
//----------   ���� ���� ���  ----------//
 #define USE_GSMART_PLU         	        // Use GSMart PLU Structure(Kor)
 #define PLU_COMMODITY_LINE_NO 8	        // ��ǰ��8��
 #define USE_TAX_RATE				        // Use Tax Rate
 #define USE_DISCOUNTED_TAX_FOR_DISCOUNTED_TOTAL
 #define USE_SELLBYDATETIME_ZERO	        // Sell by Date, Sell by Time�� 0�� �� �μ� ���ϴ� ���
 #define USE_KEEPING_CONNETION  	        // ���۵����Ͱ� ��� ���� �� Remote�� Connection ����
 // #define USE_REPORT_TAX         	        // TAX Report ���
 // #define USE_REPORT_PAY         	        // PAY/CHANGE Report ���
 #define USE_BARCODE_UPRICE_OVER	        // Unit Price Barcode�� �ڸ��� �ʰ��� ��� 'U'�� 0���� �μ�
 #define USE_FIXED_PERCENT_KEY	            // fixed Percent discount key
 #define DISABLE_PLUTYPE_CHANGE	            // Disable Changing PLU Type(count <-> weight)
 #define USE_CHN_EMART_TRACE                // �߱� �̸�Ʈ �̷°��� ���
 #define USE_CHN_EMART_BIRDS_TRACE          //�߱� �̸�Ʈ ���� �̷� ����  //20140827
 #define USE_INWEIGHTKEY_WITHOUT_INTERVAL   // IN_Weight Key ���� Weight Interval�� ������� 1g������ ��� ����
 #define USE_WEIGHT_UNIT_500G               // �߱� �ܰ� 500g ���� �߰� (1�� = 500g)
 #define USE_PROTOCOL_CHINA_DAHUA           // �߱� ���� ��ȭ ��������
 #define USE_CHINA_PINYIN                   // �߹� �Է±�
 //#define USE_CHINA_ROUND                  // �߱� ���� �ݿø� ���
 #define USE_PLU_TIMELEN_CUT                // Sell by Time, Packed Time Length 1byte�� ����
 #define USE_WEIGHING_PRINTER_COMM
 #define USE_CHECK_OUTOFPAPER_ON_LABEL		// Label mode, check Paper end
 #define USE_TRANSFER_MENU_SETTING			// MENU Setting Upload/download
//----------   ���� ���� �� ����  ----------//
 // #define USE_ENG_LABELFORMAT		        // ���� ������ ���(No.1~28 ���)
 // #define USE_KOR_LABELFORMAT		        // ���� ������ ���(No.32~45 ���)
 #define USE_CHN_LABELFORMAT 	  			// China label Format (No.1~2)
//----------   ���� ���� ���÷��� ����  ----------//
 #define USE_CHN_FONT
 #define FONT_CODEPAGE CODEPAGE_0936_CHN
 #define USE_GB2312_CHN_FONT                // simple chinese (gb2312)
 //#define USE_BIG5_CHN_FONT                // tranditional chinese (big5)
//----------   ���� ���� ���ڵ� ����  ----------//
/*		    GS1 ���ڵ� ���� (�� ���ڵ� �ߺ� ���� �ȵ�)           */
//  #define USE_RSS_BARCODE
//  #define USE_ONLY_RSS_EXPAND_BARCODE
/* ���ڵ� ������ ���� (���� ������ ������ ������ ��� �ּ�ó��) */
 #define USE_BARCODE_LEN_50
//  #define USE_BARCODE_LEN_60
//  #define USE_BARCODE_LEN_70
//  #define USE_BARCODE_LEN_90
/////////////////////////////////////////////////////////////
//----------   ���� ���� Ư������ ����  ----------//

#endif /* CHN_TRACE_STANDARD_F_H */