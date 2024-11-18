 /* 
 * File:   LKA_K.h
 * Author: JiMin JEONG
 * @brief  globals.h���� �и��� ������ī ���� ����
 * Create 2024/06/04
 */

#ifndef  LKA_K_H
#define	LKA_K_H

//----------   ���� Default  ----------//
 #define DEALER_VER_STRING	"(K)"	                // '(R)' -> '(K)' : CJK20100831
 #define USE_SRILANKA_ZERO			                // ������ī zero range ���� ���� define
 #define USE_SRILANKA_CERTI			               	// ������ī ���� ��û���� ����
 #define COUNTRY_STR "  "
//----------   ���� ���� ���  ----------//
 #define USE_TRACEABILITY       	                // Use European Traceability
 #define USE_NUTRITION_FACT		                  	// Use Nutrition Fact
 #define USE_SELLBYDATETIME_ZERO	                // Sell by Date, Sell by Time�� 0�� �� �μ� ���ϴ� ���
 #define USE_TAX_RATE			                    // Use Tax Rate
 #define USE_DISCOUNTED_TAX_FOR_DISCOUNTED_TOTAL
 #define USE_REPORT_TAX 		                    // TAX Report ���
 #define USE_REPORT_PAY         	                // PAY/CHANGE Report ���
 #define USE_CONTINUE_INIT_ZERO_ERROR	            // SriLanka initial zero range error ǥ��(���� ���� ����)
   #ifdef USE_SRILANKA_CERTI
      #define USE_HIDE_DATETIME_MENU		        // �޴� 1840 CAL �� �̵� �� ����.
      #define USE_PROHIBIT_AUTO_PRT			        // Auto print ��� ���
      #define USE_PROHIBIT_PREPACK					// Prepack ��� ���
      #define USE_PROHIBIT_PRESET_TARE				// PRESET_Tare ��� ���
      #define USE_SRILANKA_CERTI_DEFAULT			// ������ī ���� ��û�� ����ϴ� default_define
      #define USE_CHECK_CAL_SW_TO_UPDATE_FW		    // ������ CAL ����ġ�� ���� firmware ������Ʈ �� �� �ֵ��� ��.
      #define USE_CHECK_CAL_SW_TO_UPDATE_DATAROM	// ������ CAL ����ġ�� ���� dataROM ������Ʈ �� �� �ֵ��� ��.
   #endif /* USE_SRILANKA_CERTI */
//  #define USE_TRACE_STANDALONE   	                // ��� �̷±��
//  #define USE_GSMART_PLU         	                // Use GSMart PLU Structure(Kor)
//  #define PLU_COMMODITY_LINE_NO 5	                // ��ǰ��5��
//  #define USE_EMART_NETWORK      	                // E-Mart Network(�ǽð� �Ǹ� ���ε�, �Ű����� ���ε�)
//----------   ���� ���� �� ����  ----------//
 #define USE_ENG_LABELFORMAT		                // ���� ������ ���
 #define USE_KOR_LABELFORMAT		                // ���� ������ ���(No.32~45 ���)
//----------   ���� ���� ���÷��� ����  ----------//

//----------   ���� ���� ���ڵ� ����  ----------//

//----------   ���� ���� Ư������ ����  ----------//
   
#endif /* LKA_K_H */