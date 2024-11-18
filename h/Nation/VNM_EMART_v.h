 /* 
 * File:   VNM_EMART_v.h
 * Author: JiMin JEONG
 * @brief  globals.h���� �и��� ��Ʈ�� �̸�Ʈ ���� ����
 * Created 2024/06/05
 */

#ifndef VNM_EMART_v_H
#define	VNM_EMART_v_H

//----------   ���� Default  ----------//
 #define DEALER_VER_STRING	"(v)"
 #define COUNTRY_STR "  "
 #define USE_VIETNAM_EMART_DEFAULT          // ��Ʈ�� �⺻ ���� ���
//----------   ���� ���� ���  ----------//
 #define USE_TRACEABILITY			        // Use European Traceability
 #define USE_NUTRITION_FACT			        // Use Nutrition Fact
 #define USE_TAX_RATE				        // Use Tax Rate
 #define USE_DISCOUNTED_TAX_FOR_DISCOUNTED_TOTAL
 #define USE_SELLBYDATETIME_ZERO	        // Sell by Date, Sell by Time�� 0�� �� �μ� ���ϴ� ���
 #define USE_REPORT_TAX         	        // TAX Report ���
 #define USE_REPORT_PAY         	        // PAY/CHANGE Report ���
 #define USE_CHECK_OUTOFPAPER_ON_LABEL	    // Label��忡�� Paper End üũ�ϴ� ���. ��� ����
 #define USE_INWEIGHTKEY_WITHOUT_INTERVAL	// IN_Weight Key ���� Weight Interval�� ������� 1g������ ��� ����
 #define USE_FIXED_PERCENT_KEY		        // fixed Percent discount key
 #define USE_PRINT_TEMPERATURE              // �޴� 1120���� Ext.barcode �ʵ忡 �µ� �Է��ϸ� ����Ʈ�� name4�ʵ忡�� �μ�.
 #define USE_VIETNAM_EMART_INGREDIENT	    // ��Ʈ��-�̸�Ʈ ���� PLU 8000ea, Ingredient 2500ea
//----------   ���� ���� �� ����  ----------//
 #define USE_ENG_LABELFORMAT		        // ���� ������ ���
 #define USE_KOR_LABELFORMAT		        // ���� ������ ���(No.32~45 ���)
//----------   ���� ���� ���÷��� ����  ----------//
 #define USE_VIETNAM_FONT			        // ��Ʈ�� ��Ʈ ���
 #define USE_PRICECHECK_8DIGIT		        // Price Checkdigit(8digit)
 #define USE_PRICE_TOTAL_7DIGIT		        // Total 7�ڸ� ���
 #define USE_PRICE_UNIT_7DIGIT	            // Unit Price 7�ڸ� ���
 #define FONT_CODEPAGE CODEPAGE_1258_VNM
//----------   ���� ���� ���ڵ� ����  ----------//

//----------   ���� ���� Ư������ ����  ----------//

#endif  /* VNM_EMART_v_H */