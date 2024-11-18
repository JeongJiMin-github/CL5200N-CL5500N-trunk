 /* 
 * File:   ZAF_SPAR_MART_SR_D.h
 * Author: JiMin JEONG
 * @brief  globals.h���� �и��� Direct ingredient�� ����ϴ� SR ���ư� SPAR ü�θ�Ʈ ���� ��� ���� ����
 * Create 2024/06/04
 */

#ifndef ZAF_SPAR_MART_SR_D_H
#define	ZAF_SPAR_MART_SR_D_H

//----------   ���� Default  ----------//
 #define DEALER_VER_STRING	".SR"
 #define COUNTRY_STR "  "
//----------   ���� ���� ���  ----------//
 #define USE_TRACEABILITY						// Use European Traceability
 #define USE_NUTRITION_FACT						// Use Nutrition Fact
 #define USE_SELLBYDATETIME_ZERO				// Sell by Date, Sell by Time�� 0�� �� �μ� ���ϴ� ���
 #define USE_TRANSFER_MENU_SETTING				// MENU Setting Up/down ��� �߰�, *���� ������ ����.
 #define USE_CHECK_OUTOFPAPER_ON_LABEL			// Label��忡�� Paper End üũ�ϴ� ���. ��� ����
 #define USE_INWEIGHTKEY_WITHOUT_INTERVAL		// IN_Weight Key ���� Weight Interval�� ������� 1g������ ��� ����
 #define USE_TEXT_EMPHASIS						// ~1 : Italic, ~2 : Bold, ~3 : Underline �����ϴ� ��� ���� ���� �����̽� �� ���Ͱ� ������ ���� ����ǥ�� �����Ѵ�
 #define USE_DISCOUNTED_TAX_FOR_DISCOUNTED_TOTAL
 //Remote monitoring & Remote Transfer parameter
 //define USE_REMOTE_KEY
 //define USE_REMOTE_DISP
 //define USE_ADMIN_EMAIL
 #define USE_RSA_SPAR_D_INGREDIENT				// ���ư� SPAR ü�θ�Ʈ ���� PLU 4,000��  /  Direct Ingredient 900��
 #define USE_TAX_RATE						// Use Tax Rate
 #define USE_REPORT_TAX						// TAX Report ���
 #define USE_REPORT_PAY						// PAY/CHANGE Report ���
 #define USE_DIRECT_INGREDIENT				// Use direct ingredient
 #define USE_INGREDIENT_98_PROTOCOL			// PLU ��� �� ��� Direct ingredient�� 98�� ���(protocol �Ŵ��� ����)
//----------   ���� ���� �� ����  ----------//
 #define USE_ENG_LABELFORMAT					// ���� ������ ���(No.1~28 ���)		// CODE 0x407F(16511bytes)
 #define USE_KOR_LABELFORMAT					// ���� ������ ���(No.32~45 ���)	// CODE 0x2314(8980bytes)
//----------   ���� ���� ���÷��� ����  ----------//

//----------   ���� ���� ���ڵ� ����  ----------//
  #ifdef USE_QR_BARCODE
    #define USE_QR_ADDTOTAL
  #endif
//----------   ���� ���� Ư������ ����  ----------//

#endif  /* ZAF_SPAR_MART_SR_D_H */