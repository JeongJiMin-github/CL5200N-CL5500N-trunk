 /* 
 * File:   ARE_a.h
 * Author: JiMin JEONG
 * @brief  globals.h���� �и��� �ƶ����̷���Ʈ ���� ����
 * Create 2024/06/04
 */

#ifndef ARE_a_H
#define	ARE_a_H

//----------   ���� Default  ----------//
#define DEALER_VER_STRING	"(a)"
  #define COUNTRY_STR "  "
//----------   ���� ���� ���  ----------//
 #define USE_TRACEABILITY       	    // Use European Traceability
 #define USE_NUTRITION_FACT		        // Use Nutrition Fact
 #define USE_TAX_RATE				    // Use Tax Rate
 #define USE_DISCOUNTED_TAX_FOR_DISCOUNTED_TOTAL
 #define USE_SELLBYDATETIME_ZERO	    // Sell by Date, Sell by Time�� 0�� �� �μ� ���ϴ� ���
 #define USE_REPORT_TAX         	    // TAX Report ���
 #define USE_REPORT_PAY         	    // PAY/CHANGE Report ���
  //#define USE_ARAB_CONVERT		    //�ƶ��� ��ȯ ���
 #define USE_ENG_BASED_ARAB_SUPPORT		// ���� �⺻ + �ƶ��� ����(������� ǥ��)
 #define USE_BARCODE_TAX		        // 1873�� T.Price = Price+Tax �� Y�� ��� barcode total price �ʵ忡 tax ����
 //#define USE_INGREDIENT_LINE_FEED	    // Ingredient �ڵ� �� ��ȯ ó��
 //#define USE_BARCODE_FLEXIBLE	// dealer ������
//----------   ���� ���� �� ����  ----------//
 #define USE_ENG_LABELFORMAT		    // ���� ������ ���(No.1~28 ���)		// CODE 0x407F(16511bytes)
 #define USE_KOR_LABELFORMAT		    // ���� ������ ���(No.32~45 ���)	// CODE 0x2314(8980bytes)
//----------   ���� ���� ���÷��� ����  ----------//
 #define USE_ARAB_FONT			        // �ƶ���Ʈ ���
 #define FONT_CODEPAGE CODEPAGE_1256
  #ifdef USE_ARAB_FONT
    #define USE_PRINT_LTR_FOR_ARAB	// �ƶ��� ���� ������ LTR �������� �μ�
  #endif
//----------   ���� ���� ���ڵ� ����  ----------//

//----------   ���� ���� Ư������ ����  ----------//

#endif  /* ARE_a_H */