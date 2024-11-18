 /* 
 * File:   CHN_f.h
 * Author: JiMin JEONG
 * @brief  globals.h���� �и��� �߱� �Ǫ ���� ����
 * Created 2024/06/05
 */

#ifndef CHN_f_H
#define	CHN_f_H

 //----------   ���� Default  ----------//
 #define DEALER_VER_STRING	"(f)"
 #define USE_CHINA_CARRFOUR_DEFAULT
 #define USE_CHINA_DEFAULT						// PARAMETER SETTING
 #define COUNTRY_STR "  "
//----------   ���� ���� ���  ----------//
 #define USE_TRACEABILITY						// Use European Traceability
 #define USE_NUTRITION_FACT						// Use Nutrition Fact
 #define USE_SELLBYDATETIME_ZERO				// Sell by Date, Sell by Time�� 0�� �� �μ� ���ϴ� ���
 #define USE_TRANSFER_MENU_SETTING				// MENU Setting Up/down ��� �߰�, *���� ������ ����.
 #define USE_INWEIGHTKEY_WITHOUT_INTERVAL		// IN_Weight Key ���� Weight Interval�� ������� 1g������ ��� ����
 #define USE_TEXT_EMPHASIS						// ~1 : Italic, ~2 : Bold, ~3 : Underline �����ϴ� ��� ���� ���� �����̽� �� ���Ͱ� ������ ���� ����ǥ�� �����Ѵ�
 #define USE_DISCOUNTED_TAX_FOR_DISCOUNTED_TOTAL
 #define USE_TAX_RATE							// Use Tax Rate
 #define USE_REPORT_TAX							// TAX Report ���
 #define USE_REPORT_PAY							// PAY/CHANGE Report ���
 #define USE_WEIGHT_UNIT_500G					// �߱� �ܰ� 500g ���� �߰� (1�� = 500g)
 #define USE_PROTOCOL_CHINA_DAHUA				// �߱� ���� ��ȭ ��������
 #define USE_BARCODE_LEN_50 
 #define USE_CHINA_PINYIN						// �߹� �Է±�
 #define USE_PLU_TIMELEN_CUT   					// Sell by Time, Packed Time Length 1byte
 #define USE_BARCODE_DISCOUNT_RATE				// Barcode�� Discount Rate �׸� �߰�
 #define USE_PRT_CHNIA_DISCOUNT_RATE			// Discount �� �� �μ� ��� �߰�
 #define USE_FIXED_PERCENT_KEY_SET				// fixed Percent discount key ������ ���� ��� �߰�(1~99%)
 #define USE_PRT_CONVERT_WEIGHT_KG_G			// ���� �μ� ��ȯ ��� (kg <-> g)
 #define USE_CARREFOUR_ADD_PLU_URL_FEILD		// PLU Field�� Traceablity���� 2����  �ʵ� T-URL(128Bytes), T-Code(20Bytes) �߰�
 #define USE_EXTEND_NUMBER_OF_PLU_FOR_8M_DFLASH
 #define USE_PROHIBITE_PRICE_CHANGE_IN_PLU_MENU	// ���ؿ��� ��û����: ����(MENU 1110, 1120)���� ��ǰ �ܰ� ������ ���ϰ� ���ƴ޶�� ��û��
 #define USE_HYFLYING_REBOOT_FUNCTION			// Hyflying ��� 1916 Setting �κ� �߰� (Carrfour ����OFF�� ���� Disconnect ���� ���)
 #define USE_CARREFOUR_EXTEND_INGREDIENT_NUMBER	// Carrefour ��û���� ���� ���� Ȯ��(1180�� -> 2270��)
//----------   ���� ���� �� ����  ----------//
 #define USE_CHECK_OUTOFPAPER_ON_LABEL			// Label��忡�� Paper End üũ�ϴ� ���. ��� ����
 #define USE_ENG_LABELFORMAT					// ���� ������ ���(No.1~28 ���)		// CODE 0x407F(16511bytes)
 #define USE_KOR_LABELFORMAT					// ���� ������ ���(No.32~45 ���)	// CODE 0x2314(8980bytes)
//----------   ���� ���� ���÷��� ����  ----------//
 #define USE_CHN_FONT
 #define USE_GB2312_CHN_FONT					// simple chinese (gb2312)
 #define FONT_CODEPAGE CODEPAGE_0936_CHN
//----------   ���� ���� ���ڵ� ����  ----------//
 #define USE_BARCODE_UPRICE_OVER    			// when Unit Price Barcode print, valid digits exceeded  number of digits Print ' U ' to zero 
 #define USE_CARREFOUR_TRACE_QR_BARCODE			// Traceability code ��ȸ url Data�� QR Barcode�� ���� �� Field Type '3' ������ url ����ϴ� ���
//----------   ���� ���� Ư������ ����  ----------//

 #endif /* CHN_f_H */
 