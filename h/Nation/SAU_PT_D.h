 /* 
 * File:   SAU_PT_D.h
 * Author: JiMin JEONG
 * @brief  globals.h���� �и��� PT �ƶ���(����ƶ���) ���� ����
 * Create 2024/06/04
 */

#ifndef SAU_PT_D_H
#define	SAU_PT_D_H

//----------   ���� Default  ----------//
 #define DEALER_VER_STRING	".PT"
 #define COUNTRY_STR "  "
//----------   ���� ���� ���  ----------//
 //#define USE_TRACE_STANDALONE   	// ��� �̷±��
 //#define USE_GSMART_PLU         	// Use GSMart PLU Structure(Kor)
 //#define PLU_COMMODITY_LINE_NO 5	// ��ǰ��5��
 #define USE_TRANSFER_MENU_SETTING	// Parameter ������ ���� �߰� 20200820
 #define USE_TRACEABILITY       	// Use European Traceability
 #define USE_NUTRITION_FACT			// Use Nutrition Fact
 #define USE_DISCOUNTED_TAX_FOR_DISCOUNTED_TOTAL
 #define USE_SELLBYDATETIME_ZERO	// Sell by Date, Sell by Time�� 0�� �� �μ� ���ϴ� ���
 //#define USE_EMART_NETWORK      	// E-Mart Network(�ǽð� �Ǹ� ���ε�, �Ű����� ���ε�)
 #define USE_ARABIC_NUMBER			// �ƶ� ���� �μ� ��� ���
 //#define USE_ARAB_CONVERT			//�ƶ��� ��ȯ ���
 #define USE_SAUDI_INGREDIENT		// PLU 3000�� + Direct Ingredient ����(300��) / �ƶ���(300��) ����
 #define USE_TAX_RATE				// Use Tax Rate
 #define USE_REPORT_TAX         	// TAX Report ���
 #define USE_REPORT_PAY         	// PAY/CHANGE Report ���
 #define USE_INGREDIENT_98_PROTOCOL
 #define USE_DIRECT_INGREDIENT 
//----------   ���� ���� �� ����  ----------//
 #define USE_ENG_LABELFORMAT		// ���� ������ ���(No.1~28 ���)		// CODE 0x407F(16511bytes)
 //#define USE_KOR_LABELFORMAT		// ���� ������ ���(No.32~45 ���)
//----------   ���� ���� ���÷��� ����  ----------//
 #define USE_ARAB_FONT				// �ƶ���Ʈ ���
 #define FONT_CODEPAGE CODEPAGE_1256
  #ifdef USE_ARAB_FONT
    #define USE_PRINT_LTR_FOR_ARAB	// �ƶ��� ���� ������ LTR �������� �μ�
  #endif
//----------   ���� ���� ���ڵ� ����  ----------//
  #ifdef USE_QR_BARCODE
    #define USE_QR_ADDTOTAL			//QR���ڵ� �̿��� ADD TOTAL ���
  #endif
//----------   ���� ���� Ư������ ����  ----------//

#endif  /* SAU_PT_D_H */