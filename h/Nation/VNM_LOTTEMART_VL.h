 /* 
 * File:   VNM_LOTTEMART_VL.h
 * Author: JiMin JEONG
 * @brief  globals.h���� �и��� ��Ʈ�� �Ե���Ʈ ���� ����
 */

#ifndef VNM_LOTTEMART_VL_H
#define	VNM_LOTTEMART_VL_H

//----------   ���� Default  ----------//
 #define USE_VIETNAM_LOTTEMART_DEFAULT
 #define COUNTRY_STR "  "
 #define DEALER_VER_STRING  ".VL"
//----------   ���� ���� ���  ----------//
 #define USE_TRANSFER_MENU_SETTING			    // Parameter �� �޴� �⺻�� ������ ���� �߰�
 #define USE_TRACEABILITY					          // Use European Traceability
 #define USE_NUTRITION_FACT					        // Use Nutrition Fact
 #define USE_TAX_RATE						            // Use Tax Rate
 #define USE_DISCOUNTED_TAX_FOR_DISCOUNTED_TOTAL
 #define USE_SELLBYDATETIME_ZERO			      // Sell by Date, Sell by Time�� 0�� �� �μ� ���ϴ� ���
 #define USE_REPORT_TAX         			      // TAX Report ���
 #define USE_REPORT_PAY         			      // PAY/CHANGE Report ���
 #define USE_INWEIGHTKEY_WITHOUT_INTERVAL	  // IN_Weight Key ���� Weight Interval�� ������� 1g������ ��� ����
 #define USE_FIXED_PERCENT_KEY				      // fixed Percent discount key
 #define USE_CTS_FUNCTION		   			        // Calibration Traceability System
 #define USE_GSMART_PLU
 #define PLU_COMMODITY_LINE_NO 5	        // ��ǰ��5��
 #define USE_KOR_INGREDIENT_EXPAND
 #define USE_EXPAND_ADVEMSG_STR            // ������ �ʵ� ���� 100byte�� Ȯ��(���� 30byte) / ���� 30���� ���(���� 99��)
//----------   ���� ���� �� ����  ----------//
 #define USE_ENG_LABELFORMAT				        // ���� ������ ���
 #define USE_KOR_LABELFORMAT				        // ���� ������ ���(No.32~45 ���)
 #define USE_CHECK_OUTOFPAPER_ON_LABEL		  // Label��忡�� Paper End üũ�ϴ� ���. ��� ����
//----------   ���� ���� ���÷��� ����  ----------//
 #define USE_PRICECHECK_8DIGIT				      // Price Checkdigit(8digit)
 #define USE_PRICE_TOTAL_7DIGIT				      // Total 7�ڸ� ���
 #define USE_PRICE_UNIT_7DIGIT				      // Unit Price 7�ڸ� ���
 #define USE_VIETNAM_FONT					          // ��Ʈ�� ��Ʈ ���
 #define FONT_CODEPAGE CODEPAGE_1258_VNM
//----------   ���� ���� ���ڵ� ����  ----------//

//----------   ���� ���� Ư������ ����  ----------//

#endif	/* VNM_LOTTEMART_VL_H */
