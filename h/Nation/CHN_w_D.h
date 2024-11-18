 /* 
 * File:   CHN_w_D.h
 * Author: JiMin JEONG
 * @brief  globals.h���� �и��� �߱� �ٿ��� ��Ʈ�� ���� ���� ����
 * Created 2024/06/10
 */

#ifndef CHN_w_D_H
#define	CHN_w_D_H

//----------   ���� Default  ----------//
 #define USE_CHINA_DEFAULT				        // PARAMETER SETTING
 #define DEALER_VER_STRING	"(w)"
 #define COUNTRY_STR "  "
//----------   ���� ���� ���  ----------//
 #define USE_TRACEABILITY						// Use European Traceability
 #define USE_NUTRITION_FACT						// Use Nutrition Fact
 #define USE_SELLBYDATETIME_ZERO					// Sell by Date, Sell by Time�� 0�� �� �μ� ���ϴ� ���
 #define USE_TRANSFER_MENU_SETTING				// MENU Setting Up/down ��� �߰�, *���� ������ ����.
 #define USE_CHECK_OUTOFPAPER_ON_LABEL			// Label��忡�� Paper End üũ�ϴ� ���. ��� ����
 #define USE_INWEIGHTKEY_WITHOUT_INTERVAL		// IN_Weight Key ���� Weight Interval�� ������� 1g������ ��� ����
 #define USE_TEXT_EMPHASIS						// ~1 : Italic, ~2 : Bold, ~3 : Underline �����ϴ� ��� ���� ���� �����̽� �� ���Ͱ� ������ ���� ����ǥ�� �����Ѵ�
 #define USE_DISCOUNTED_TAX_FOR_DISCOUNTED_TOTAL
 #define USE_WEIGHT_UNIT_500G			        // �߱� �ܰ� 500g ���� �߰� (1�� = 500g)
 #define USE_PROTOCOL_CHINA_DAHUA		        // �߱� ���� ��ȭ ��������
 #define USE_CHINA_PINYIN				        // �߹� �Է±�
 #define USE_PLU_TIMELEN_CUT   			        // Sell by Time, Packed Time Length 1byte
 #define USE_BARCODE_DISCOUNT_RATE		        // Barcode�� Discount Rate �׸� �߰�
 #define USE_PRT_CHNIA_DISCOUNT_RATE		    // Discount �� �� �μ� ��� �߰�
 #define USE_FIXED_PERCENT_KEY_SET		        // fixed Percent discount key ������ ���� ��� �߰�(1~99%)
 #define USE_PRT_CONVERT_WEIGHT_KG_G		    // ���� �μ� ��ȯ ��� (kg <-> g)
 #define PLU_COMMODITY_LINE_NO 	8	            // ��ǰ��8��
 #define USE_TAX_RATE						    // Use Tax Rate
 #define USE_REPORT_TAX						    // TAX Report ���
 #define USE_REPORT_PAY						    // PAY/CHANGE Report ���
 #define USE_DIRECT_INGREDIENT				    // Use direct ingredient
 #define USE_INGREDIENT_98_PROTOCOL			    // PLU ��� �� ��� Direct ingredient�� 98�� ���(protocol �Ŵ��� ����)
 //	#define USE_INGREDIENT_99_PROTOCOL			// PLU ��� �� ��� Direct ingredient�� 99�� ���(protocol �Ŵ��� ����)
//----------   ���� ���� �� ����  ----------//
 #define USE_ENG_LABELFORMAT					// ���� ������ ���(No.1~28 ���)		// CODE 0x407F(16511bytes)
 #define USE_KOR_LABELFORMAT					// ���� ������ ���(No.32~45 ���)	// CODE 0x2314(8980bytes)
//----------   ���� ���� ���÷��� ����  ----------//
 #define USE_CHN_FONT
 #define USE_GB2312_CHN_FONT				    // simple chinese (gb2312)
 #define FONT_CODEPAGE CODEPAGE_0936_CHN
//----------   ���� ���� ���ڵ� ����  ----------//
 #define USE_BARCODE_UPRICE_OVER    		    // when Unit Price Barcode print, valid digits exceeded  number of digits Print ' U ' to zero 
/*		 GS1 ���ڵ� ���� (�� ���ڵ� �ߺ� ���� �ȵ�)           */
//  #define USE_RSS_BARCODE
//  #define USE_ONLY_RSS_EXPAND_BARCODE
/* ���ڵ� ������ ���� (���� ������ ������ ������ ��� �ּ�ó��) */
 #define USE_BARCODE_LEN_50
//  #define USE_BARCODE_LEN_60
//  #define USE_BARCODE_LEN_70
//  #define USE_BARCODE_LEN_90
/////////////////////////////////////////////////////////////
//----------   ���� ���� Ư������ ����  ----------//

#endif  /* CHN_w_D_H */
