 /* 
 * File:   POL_SK.h
 * Author: JiMin JEONG
 * @brief  globals.h���� �и��� ������ �Ҹ� ü�� ���� ����
 * Created 2024/06/04��
 */

#ifndef POL_SK_H
#define	POL_SK_H

//----------   ���� Default  ----------//
 #define DEALER_VER_STRING  ".SK"
 #define USE_STOKROTKA_MART_DEFAULT				  // ������ STOKROTKA ��Ʈ ���� �⺻�� ����
 ////// ������ Define ///////
 #define USE_POLAND_DEFAULT
 //////////////////////////
 #define COUNTRY_STR "  "
//----------   ���� ���� ���  ----------//
 #define USE_TRACEABILITY						        // Use European Traceability
 #define USE_NUTRITION_FACT						      // Use Nutrition Fact
 #define USE_SELLBYDATETIME_ZERO				    // Sell by Date, Sell by Time�� 0�� �� �μ� ���ϴ� ���
 #define USE_TRANSFER_MENU_SETTING				  // MENU Setting Up/down ��� �߰�, *���� ������ ����.
 #define USE_CHECK_OUTOFPAPER_ON_LABEL			// Label��忡�� Paper End üũ�ϴ� ���. ��� ����
 #define USE_INWEIGHTKEY_WITHOUT_INTERVAL		// IN_Weight Key ���� Weight Interval�� ������� 1g������ ��� ����
 #define USE_TEXT_EMPHASIS						      // ~1 : Italic, ~2 : Bold, ~3 : Underline �����ϴ� ��� ���� ���� �����̽� �� ���Ͱ� ������ ���� ����ǥ�� �����Ѵ�
 #define USE_DISCOUNTED_TAX_FOR_DISCOUNTED_TOTAL
 #define USE_TAX_RATE							          // Use Tax Rate
 #define USE_REPORT_TAX							        // TAX Report ���
 #define USE_REPORT_PAY							        // PAY/CHANGE Report ���
//----------   ���� ���� �� ����  ----------//
 #define USE_ENG_LABELFORMAT					      // ���� ������ ���(No.1~28 ���)		// CODE 0x407F(16511bytes)
 #define USE_KOR_LABELFORMAT					      // ���� ������ ���(No.32~45 ���)	// CODE 0x2314(8980bytes)
 #define USE_EU_NUTRITIONFACT
//----------   ���� ���� ���÷��� ����  ----------//
 #define FONT_CODEPAGE CODEPAGE_1250_POL
//----------   ���� ���� ���ڵ� ����  ----------//
  #ifdef USE_QR_BARCODE
    #define USE_QR_ADDTOTAL						      //QR���ڵ� �̿��� ADD TOTAL ���
  #endif
//----------   ���� ���� Ư������ ����  ----------//

#endif	/* POL_SK_H */
