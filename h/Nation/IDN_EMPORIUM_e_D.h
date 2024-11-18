 /* 
 * File:   IDN_EMPORIUM_e_D.h
 * Author: JiMin JEONG
 * @brief  globals.h���� �и��� Direct ingredient�� ����ϴ� �ε��׽þ� �������� ���� ���� ����
 * Create 2024/06/04
 */

#ifndef IDN_EMPORIUM_e_D_H
#define	IDN_EMPORIUM_e_D_H

//----------   ���� Default  ----------//
 #define DEALER_VER_STRING  "(e)"
 #define COUNTRY_STR "  "
 #define USE_EMPORIUM_DEFAULT		        // Use Indonesia Emporium Default Barcode
//----------   ���� ���� ���  ----------//
 #define USE_TRACEABILITY		            // Use European Traceability
 #define USE_NUTRITION_FACT	                // Use Nutrition Fact
 #define USE_SELLBYDATETIME_ZERO            // Sell by Date, Sell by Time�� 0�� �� �μ� ���ϴ� ���
 #define USE_TRANSFER_MENU_SETTING          // MENU Setting Up/down ��� �߰�, *���� ������ ����.
 #define USE_CHECK_OUTOFPAPER_ON_LABEL      // Label��忡�� Paper End üũ�ϴ� ���. ��� ����
 #define USE_INWEIGHTKEY_WITHOUT_INTERVAL	// IN_Weight Key ���� Weight Interval�� ������� 1g������ ��� ����
 #define USE_TEXT_EMPHASIS				    // ~1 : Italic, ~2 : Bold, ~3 : Underline �����ϴ� ��� ���� ���� �����̽� �� ���Ͱ� ������ ���� ����ǥ�� �����Ѵ�
 #define USE_BARCODE_SHARP_CHECKDIGIT       // indonesia code128X ���ڵ忡�� #ĳ���� ���� üũ�� �� ����.
 #define USE_EMPORIUM_INGREDIENT 	        // use direct ingredient for emporium (Indonesia Market) => 150byte*2000ea(Max: 300byte)
 #define USE_PLU_MULTIPLE_PRICE		        // use direct multiple price (Direct discount function) for emporium
 #define USE_DIRECT_INGREDIENT	            // use direct ingredient. Russia �� �׻� ���
//----------   ���� ���� �� ����  ----------//
 #define USE_ENG_LABELFORMAT 	            // ���� ������ ���(No.1~28 ���)	   // CODE 0x407F(16511bytes)
 #define USE_KOR_LABELFORMAT 	            // ���� ������ ���(No.32~45 ���)    // CODE 0x2314(8980bytes)
//----------   ���� ���� ���÷��� ����  ----------//
 #define USE_PRICE_TOTAL_7DIGIT	            // Total 7�ڸ� ���
 #define USE_PRICECHECK_8DIGIT	            // Price Checkdigit(8digit)
//----------   ���� ���� ���ڵ� ����  ----------//

//----------   ���� ���� Ư������ ����  ----------//

#endif  /* IDN_EMPORIUM_e_D_H */