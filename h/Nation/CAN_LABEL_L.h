 /* 
 * File:   CAN_LABEL_L.h
 * Author: JiMin JEONG
 * @brief  globals.h���� �и���ĳ���� �� ���� ���(200mm) ���� ����
 * Created 2024/06/10
 */

#ifndef CAN_LABEL_L_H
#define	CAN_LABEL_L_H

//----------   ���� Default  ----------//
 #define DEALER_VER_STRING	"(L)"
 #define COUNTRY_STR "  "
//----------   ���� ���� ���  ----------//
 //#define USE_TRACE_STANDALONE   	// ��� �̷±��
 //#define USE_GSMART_PLU         	// Use GSMart PLU Structure(Kor)
 //#define PLU_COMMODITY_LINE_NO 5	// ��ǰ��5��
 //#define USE_TRACE_STANDALONE   	// ��� �̷±��
 //#define USE_GSMART_PLU         	// Use GSMart PLU Structure(Kor)
 //#define PLU_COMMODITY_LINE_NO 5	// ��ǰ��5��
 #define USE_TRACEABILITY       	// Use European Traceability
 #define USE_NUTRITION_FACT		    // Use Nutrition Fact
 #define USE_TAX_RATE				// Use Tax Rate
 #define USE_DISCOUNTED_TAX_FOR_DISCOUNTED_TOTAL
 #define USE_SELLBYDATETIME_ZERO	// Sell by Date, Sell by Time�� 0�� �� �μ� ���ϴ� ���
 //#define USE_EMART_NETWORK      	// E-Mart Network(�ǽð� �Ǹ� ���ε�, �Ű����� ���ε�)
 #define USE_REPORT_TAX         	// TAX Report ���
 #define USE_REPORT_PAY         	// PAY/CHANGE Report ���
//----------   ���� ���� �� ����  ----------//
 #define USE_ENG_LABELFORMAT		// ���� ������ ���
 //#define USE_KOR_LABELFORMAT		// ���� ������ ���(No.32~45 ���)
 #define USE_CAN_LABEL_LENGTH		// ĳ���� �� ���� ��� 200mm (���� 120mm)
//----------   ���� ���� ���÷��� ����  ----------//

//----------   ���� ���� ���ڵ� ����  ----------//

//----------   ���� ���� Ư������ ����  ----------//

#endif  /* CAN_LABEL_L_H */