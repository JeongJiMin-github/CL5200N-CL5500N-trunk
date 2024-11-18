 /* 
 * File:   CHN_l.h
 * Author: JiMin JEONG
 * @brief  globals.h���� �и��� �߱� �˸��ٹ� ���� ����
 * Created 2024/06/05
 */

#ifndef CHN_l_H
#define	CHN_l_H

//----------   ���� Default  ----------//
 #define DEALER_VER_STRING  "(l)"
 #define USE_CHINA_DEFAULT				            // PARAMETER SETTING
 #define COUNTRY_STR "  "
//----------   ���� ���� ���  ----------//
 #define USE_TRACEABILITY						    // Use European Traceability
 #define USE_NUTRITION_FACT						    // Use Nutrition Fact
 #define USE_SELLBYDATETIME_ZERO					// Sell by Date, Sell by Time�� 0�� �� �μ� ���ϴ� ���
 #define USE_TRANSFER_MENU_SETTING				    // MENU Setting Up/down ��� �߰�, *���� ������ ����.
 #define USE_INWEIGHTKEY_WITHOUT_INTERVAL		    // IN_Weight Key ���� Weight Interval�� ������� 1g������ ��� ����
 #define USE_TEXT_EMPHASIS						    // ~1 : Italic, ~2 : Bold, ~3 : Underline �����ϴ� ��� ���� ���� �����̽� �� ���Ͱ� ������ ���� ����ǥ�� �����Ѵ�
 #define USE_DISCOUNTED_TAX_FOR_DISCOUNTED_TOTAL
 #define USE_TRANSFER_MENU_SETTING
 #define USE_TAX_RATE						        // Use Tax Rate
 #define USE_REPORT_TAX						        // TAX Report ���
 #define USE_REPORT_PAY						        // PAY/CHANGE Report ���
 #define USE_WEIGHT_UNIT_500G			            // �߱� �ܰ� 500g ���� �߰� (1�� = 500g)
 #define USE_UNIT_500G_PLU_500G_BASE	            // �߱� �ܰ� 500g ���� �߰� (PLU ���� �ܰ�: 500g)
 #define USE_PROTOCOL_CHINA_DAHUA		            // �߱� ���� ��ȭ ��������
 #define USE_CHINA_PINYIN				            // �߹� �Է±�
 #define USE_PLU_TIMELEN_CUT   			            // Sell by Time, Packed Time Length 1byte
#define USE_PRT_CONVERT_WEIGHT_KG_G                 // ���� �μ� ��ȯ ��� (kg <-> g)
#define USE_BARCODE_DISCOUNT_RATE                   // Barcode�� Discount Rate �׸� �߰�
#define USE_PRT_CHNIA_DISCOUNT_RATE                 // Discount �� �� �μ� ��� �߰�
#define USE_FIXED_PERCENT_KEY_SET                   // fixed Percent discount key ������ ���� ��� �߰�(1~99%)
// #define USE_FIXED_PERCENT_KEY			        // fixed Percent discount key
// #define USE_CHINA_ROUND   				        // Chinese round price
//----------   ���� ���� �� ����  ----------//
 #define USE_CHECK_OUTOFPAPER_ON_LABEL			    // Label��忡�� Paper End üũ�ϴ� ���. ��� ����
//----------   ���� ���� ���÷��� ����  ----------//
 #define USE_CHN_FONT
 #define USE_GB2312_CHN_FONT				        // simple chinese (gb2312)
 #define FONT_CODEPAGE CODEPAGE_0936_CHN
//----------   ���� ���� ���ڵ� ����  ----------//
 #define USE_BARCODE_UPRICE_OVER    		        // when Unit Price Barcode print, valid digits exceeded  number of digits Print ' U ' to zero 
 #define USE_BARCODE_LEN_50 
//----------   ���� ���� Ư������ ����  ----------//

 #endif /* CHN_l_H */