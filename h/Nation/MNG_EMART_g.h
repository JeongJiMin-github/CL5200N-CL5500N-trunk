 /* 
 * File:   MNG_EMART_g.h
 * Author: JiMin JEONG
 * @brief  globals.h���� �и��� CL5500 ���� �̸�Ʈ ���� ����
 * Create 2024/06/04
 */

#ifndef MNG_EMART_g_H
#define	MNG_EMART_g_H

//----------   ���� Default  ----------//
 #define DEALER_VER_STRING	"(g)"
 #define COUNTRY_STR "  "
 #define USE_MON_EMART_DEFAULT				    // Use Mongol E-Mart Default Setting
//----------   ���� ���� ���  ----------//
 #define USE_TRACEABILITY       				// Use European Traceability
 #define USE_NUTRITION_FACT						// Use Nutrition Fact
 #define USE_SELLBYDATETIME_ZERO				// Sell by Date, Sell by Time�� 0�� �� �μ� ���ϴ� ���
 #define USE_TRANSFER_MENU_SETTING				// MENU Setting Up/down ��� �߰�, *���� ������ ����.
 #define USE_CHECK_OUTOFPAPER_ON_LABEL			// Label��忡�� Paper End üũ�ϴ� ���. ��� ����
 #define USE_INWEIGHTKEY_WITHOUT_INTERVAL		// IN_Weight Key ���� Weight Interval�� ������� 1g������ ��� ����
 #define USE_TEXT_EMPHASIS						// ~1 : Italic, ~2 : Bold, ~3 : Underline �����ϴ� ��� ���� ���� �����̽� �� ���Ͱ� ������ ���� ����ǥ�� �����Ѵ�
 #define USE_TAX_RATE						    // Use Tax Rate
 #define USE_DISCOUNTED_TAX_FOR_DISCOUNTED_TOTAL
 #define USE_REPORT_TAX						    // TAX Report ���
 #define USE_REPORT_PAY			 			    // PAY/CHANGE Report ���
 #define USE_FIXED_PERCENT_KEY			        // fixed Percent discount key
 #define USE_PRT_DISC_PERCENT			        // Use Print Discount Percent
//----------   ���� ���� �� ����  ----------//
 #define USE_ENG_LABELFORMAT				    // ���� ������ ���(No.1~28 ���)		// CODE 0x407F(16511bytes)
 #define USE_KOR_LABELFORMAT				    // ���� ������ ���(No.32~45 ���)	// CODE 0x2314(8980bytes)
//----------   ���� ���� ���÷��� ����  ----------//
 #define FONT_CODEPAGE CODEPAGE_1251_MNG
//----------   ���� ���� ���ڵ� ����  ----------//

//----------   ���� ���� Ư������ ����  ----------//

#endif  /* MNG_EMART_g_H */