 /* 
 * File:   TUR_QR_BARCODE_t_D.h
 * Author: JiMin JEONG
 * @brief  globals.h���� �и��� Direct ingredient�� ����ϴ� ��Ű ��û���� ���� (�Է��� URL�� QR ���ڵ�� ����ϴ� ���) ���� ����
 * Create 2024/06/04
 */

#ifndef TUR_QR_BARCODE_t_D_H
#define	TUR_QR_BARCODE_t_D_H

//----------   ���� Default  ----------//
 #define DEALER_VER_STRING  "(t)"
 #define COUNTRY_STR "  "
 #define USE_TURKEY_QR_BARCODE_DEAULT
 #define USE_ESD_ADVANCE_DEFAULT
//----------   ���� ���� ���  ----------//
 #define USE_TRACEABILITY						// Use European Traceability
 #define USE_NUTRITION_FACT						// Use Nutrition Fact
 #define USE_SELLBYDATETIME_ZERO				// Sell by Date, Sell by Time�� 0�� �� �μ� ���ϴ� ���
 #define USE_TRANSFER_MENU_SETTING				// MENU Setting Up/down ��� �߰�, *���� ������ ����.
 #define USE_CHECK_OUTOFPAPER_ON_LABEL			// Label��忡�� Paper End üũ�ϴ� ���. ��� ����
 #define USE_INWEIGHTKEY_WITHOUT_INTERVAL		// IN_Weight Key ���� Weight Interval�� ������� 1g������ ��� ����
 #define USE_TEXT_EMPHASIS						// ~1 : Italic, ~2 : Bold, ~3 : Underline �����ϴ� ��� ���� ���� �����̽� �� ���Ͱ� ������ ���� ����ǥ�� �����Ѵ�
 #define USE_DISCOUNTED_TAX_FOR_DISCOUNTED_TOTAL
 #define USE_DIRECT_INGREDIENT_EXTEND_8M_DFLASH	// 8MB D_Flash Ȯ�念�� Ȱ��  -  Direct Ingredient (152Bytes * 2,000 Blocks) �� (152Bytes * 24,000 Blocks)
 #define USE_PRINT_URL_VIA_QR_BARCODE			// ���￡ �Է¹��� URL�� QR���ڵ带 ���ؼ� ���(URL�� PLU���� �Ҵ� �� Direct Ingredient�� ����,  USE_DIRECT_INGREDIENT���� ������ �ʿ�)
 #define USE_TAX_RATE							// Use Tax Rate
 #define USE_REPORT_TAX							// TAX Report ���
 #define USE_REPORT_PAY							// PAY/CHANGE Report ���
 #define USE_DIRECT_INGREDIENT
 #define USE_INGREDIENT_98_PROTOCOL				// PLU ��� �� ��� Direct ingredient�� 98�� ���(protocol �Ŵ��� ����)
//----------   ���� ���� �� ����  ----------//
 #define USE_ENG_LABELFORMAT						// ���� ������ ���(No.1~28 ���)		// CODE 0x407F(16511bytes)
//----------   ���� ���� ���÷��� ����  ----------//
 #define FONT_CODEPAGE	CODEPAGE_1254
//----------   ���� ���� ���ڵ� ����  ----------//
  #ifdef USE_QR_BARCODE
    #define USE_QR_ADDTOTAL						//QR���ڵ� �̿��� ADD TOTAL ���
  #endif
//----------   ���� ���� Ư������ ����  ----------//

#endif  /* TUR_QR_BARCODE_t_D_H */