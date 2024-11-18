 /* 
 * File:   CHN_FONT_BIG5_TR_D.h
 * Author: JiMin JEONG
 * @brief  globals.h���� �и��� TR tranditional chinese (big5) ���� ����
 * Create 2024/06/04
 */

#ifndef CHN_FONT_BIG5_TR_D_H
#define	CHN_FONT_BIG5_TR_D_H

//----------   ���� Default  ----------//
 #define DEALER_VER_STRING	".TR"
 #define COUNTRY_STR "  "
 #define COUNTRY_USA
 #define USE_USA_DEFAULT
//----------   ���� ���� ���  ----------//
 #define USE_USA_VERSION_DISPLAY			    // �̱� ���� ���� ǥ�� (CL5200N -> CL7000, CL5500N -> CL5000���� ǥ��)
 #define USE_USA_INGREDIENT					    // USA Direct Ingredient(PLU 4000EA, each ingredient data Max : 2100bytes)
  #ifdef USE_USA_INGREDIENT
   #define USE_DIRECT_INGREDIENT				// Use direct ingredient
   #define USE_INGREDIENT_98_PROTOCOL			// PLU ��� �� ��� Direct ingredient�� 98�� ���(protocol �Ŵ��� ����)
  #endif
 #define USE_TRACEABILITY					    // Use European Traceability
 #define USE_NUTRITION_FACT					    // Use Nutrition Fact
 #define USE_SELLBYDATETIME_ZERO			    // Sell by Date, Sell by Time�� 0�� �� �μ� ���ϴ� ���
 #define USE_TRANSFER_MENU_SETTING			    // MENU Setting Up/down ��� �߰�, *���� ������ ����.
 #define USE_CHECK_OUTOFPAPER_ON_LABEL		    // Label��忡�� Paper End üũ�ϴ� ���. ��� ����
 #define USE_INWEIGHTKEY_WITHOUT_INTERVAL	    // IN_Weight Key ���� Weight Interval�� ������� 1g������ ��� ����
 #define USE_TEXT_EMPHASIS					    // ~1 : Italic, ~2 : Bold, ~3 : Underline �����ϴ� ��� ���� ���� �����̽� �� ���Ͱ� ������ ���� ����ǥ�� �����Ѵ�
 #define USE_DISCOUNTED_TAX_FOR_DISCOUNTED_TOTAL
 #define ETH_LED_INDICATOR_DISABLE 		 	    // wired lan link indicator disable for NTEP USA 		   
 #define USE_ADDITIONAL_NUTRIENT			    // Nutrient �׸� �߰�.
 #define USE_USA_FIXEDWEIGHT_UNIT_OZ		    // Nonweight, Count PLU Fixed Weight Unit change to oz
 #define USE_USA_UNIT_PRICE_DOT_DISP		    // �̱� ȭ���ǿ� �´� $/kg, $/lb dot ǥ��
 #define USE_TAX_RATE						    // Use Tax Rate
 #define USE_REPORT_TAX						    // TAX Report ���
 #define USE_REPORT_PAY						    // PAY/CHANGE Report ���
//----------   ���� ���� �� ����  ----------//
 #define USE_LABEL_LENGTH_200MM		   		    // �� ���� ��� 200mm (���� 120mm)
 #define USE_ENG_LABELFORMAT
 #define USE_KOR_LABELFORMAT
//----------   ���� ���� ���÷��� ����  ----------//
 #define USE_CHN_FONT
 #define USE_BIG5_CHN_FONT 					    // tranditional chinese (big5)
 #define FONT_CODEPAGE CODEPAGE_0950_CHN	    // tranditional chinese code page
//----------   ���� ���� ���ڵ� ����  ----------//

//----------   ���� ���� Ư������ ����  ----------//



#endif  /* CHN_FONT_BIG5_TR_D_H */