 /* 
 * File:   USA_UN_D.h
 * Author: JiMin JEONG
 * @brief  globals.h���� �и��� direct_ingredient�� ����ϴ� �̱� ���� ����
 * Created 2024/06/05
 */

#ifndef USA_UN_D_H
#define	USA_UN_D_H

//----------   ���� Default  ----------//
 #define USE_USA_DEFAULT
 #define COUNTRY_STR "US"
//----------   ���� ���� ���  ----------//
 #define USE_USA_FIXEDWEIGHT_UNIT_OZ		  // Nonweight, Count PLU Fixed Weight Unit change to oz
 #define ETH_LED_INDICATOR_DISABLE		    // wired lan link indicator disable for NTEP USA
 //#define USE_USA_INGREDIENT				        // USA Direct Ingredient(PLU 4000EA, each ingredient data Max : 2100bytes)
 #define USE_NORTH_AMERICA_INGREDIENT	        // �̱� / ĳ���� ���� Direct Ingredient & Nutrition Fact ���� ( PLU 4,000EA, each Nutrition Fact)
 #define USE_ADDITIONAL_NUTRIENT			    // Nutrient �׸� �߰�.
 #define USE_DIRECT_NUTRITION
 #undef USE_ALLOW_CAL_MODE				        // �̱��� �ش� ��� ���� �� �ϰ� ������ ����			
 #define USE_USA_VERSION_DISPLAY			    // �̱� ���� ���� ǥ�� (CL5200N -> CL7000, CL5500N -> CL5000���� ǥ��)
 #define USE_JULIAN_DATE
 #define USE_BEARER_BAR_ONE_LINE			    // ���ڵ� ��� �ϴ� �ɼǻ����� �׵θ� �ٸ� ����� �ϳ��θ� ���(�̱� ��û)
 #define USE_CANADA_ZERO_MARK_SETTING	    // ���� ���� ����. CAL8170���� Gross Zero Mark[N] / Net Zero Mark[Y], Menu���� ���� �Ұ�
  #ifdef USE_USA_NTEP_SETTING
    #define USE_NTEP_CERT_DEFAULT			    // ������ �⺻�� ���� (1. ������ �� ID ����)
  #endif
  #define USE_STANDARD_DIRECT_INGREDIENT
//----------   ���� ���� �� ����  ----------//
 // USA/CANADA ���� �� ���
 #define USE_USA_LABELFORMAT
 #define USE_LABEL_LENGTH_200MM			      // �� ���� ��� 200mm (���� 120mm)
//----------   ���� ���� ���÷��� ����  ----------//
 #define USE_KOR_FONT
 #define USE_FONT_ID6_12BY28				      // Font ID 6 12x28 font ���
 #define USE_FONT_ID4_Arial				        // Font ID 4 Arial font ���
 #define USE_USA_UNIT_PRICE_DOT_DISP		  // �̱� ȭ���ǿ� �´� $/kg, $/lb dot ǥ��
 #define FONT_CODEPAGE CODEPAGE_0949_KOR
//----------   ���� ���� ���ڵ� ����  ----------//

//----------   ���� ���� Ư������ ����  ----------//

#endif /* USA_UN_D_H */