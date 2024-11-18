 /* 
 * File:   ISR_HEBREW_h.h
 * Author: JiMin JEONG
 * @brief  globals.h���� �и��� ���긮� ����ϴ� �̽��� ���� ����
 * Create 2024/06/04
 */

#ifndef ISR_HEBREW_h_H
#define	ISR_HEBREW_h_H

//----------   ���� Default  ----------//
 #define DEALER_VER_STRING  "(h)"
 #define COUNTRY_STR "  "
 #define USE_HEBREW_DEFAULT
//----------   ���� ���� ���  ----------//
 #define USE_TRACEABILITY		            // Use European Traceability
 #define USE_NUTRITION_FACT	                // Use Nutrition Fact
 #define USE_TAX_RATE			            // Use Tax Rate
 #define USE_DISCOUNTED_TAX_FOR_DISCOUNTED_TOTAL
 #define USE_SELLBYDATETIME_ZERO            // Sell by Date, Sell by Time�� 0�� �� �μ� ���ϴ� ���
 #define USE_REPORT_TAX			            // TAX Report ���
 #define USE_REPORT_PAY			            // PAY/CHANGE Report ���
//----------   ���� ���� �� ����  ----------//
 #define USE_ENG_LABELFORMAT 	            // ���� ������ ���(No.1~28 ���)	   // CODE 0x407F(16511bytes)
 #define USE_KOR_LABELFORMAT 	            // ���� ������ ���(No.32~45 ���)    // CODE 0x2314(8980bytes)
//----------   ���� ���� ���÷��� ����  ----------//
 #define HEBREW_FONT				        // ���긮�� ��Ʈ ���
//----------   ���� ���� ���ڵ� ����  ----------//

//----------   ���� ���� Ư������ ����  ----------//

#endif  /* ISR_HEBREW_h */
