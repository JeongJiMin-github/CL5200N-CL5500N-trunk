 /* 
 * File:   BLR_r_D.h
 * Author: JiMin JEONG
 * @brief  globals.h���� �и��� direct_ingredient�� ����ϴ� ����罺 ���� ����
 * Created 2024/06/05
 */

#ifndef BLR_r_D_H
#define	BLR_r_D_H

//----------   ���� Default  ----------//
 #define DEALER_VER_STRING	"(r)"
 #define USE_BELARUS_DEFAULT		            // Use Belarus Default
 #define COUNTRY_STR "  "
//----------   ���� ���� ���  ----------//
 #define USE_TRACEABILITY       	            // Use European Traceability
 #define USE_NUTRITION_FACT		                // Use Nutrition Fact
 #define USE_TAX_RATE						    // Use Tax Rate
 #define USE_DISCOUNTED_TAX_FOR_DISCOUNTED_TOTAL
 #define USE_SELLBYDATETIME_ZERO	            // Sell by Date, Sell by Time�� 0�� �� �μ� ���ϴ� ���
 #define USE_DIRECT_INGREDIENT		            // use direct ingredient. Russia �� �׻� ���
 #define USE_PROTOCOL_CL5000JR   	            // for CL5000J Protocol
 #define USE_DMSG_2100_BYTE			            // Direct Ingredient 2100��
 #define USE_INWEIGHTKEY_WITHOUT_INTERVAL	    // IN_Weight Key ���� Weight Interval�� ������� 1g������ ��� ����
 #define USE_DONT_SAVE_VALIDDATE_SALEDATE       // ��ȿ��, ��ȿ�Ⱓ �������� ����
 #define USE_CONTINUOUS_LABEL_WITH_INGREDIENT   // Continuous label ���� Ingredient ����Ʈ�� ���������� ������
 #define USE_BELARUS_VER_DISPLAY		        // ����罺 ���ý� ǥ�� ���� ���� ���.
//----------   ���� ���� �� ����  ----------//
 #define USE_BELARUS_LABEL			            // ����罺 ���� �� born in/bred in�� �ٸ� �뵵�� ���
//----------   ���� ���� ���÷��� ����  ----------//
 #define USE_PRICE_UNIT_7DIGIT	                // Unit Price 7�ڸ� ���
 #define USE_PRICE_TOTAL_7DIGIT	                // Total 7�ڸ� ���
//----------   ���� ���� ���ڵ� ����  ----------//

//----------   ���� ���� Ư������ ����  ----------//

#endif /* BLR_r_D_H */
