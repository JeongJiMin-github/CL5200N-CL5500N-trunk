 /* 
 * File:   CAN_UN_D.h
 * Author: JiMin JEONG
 * @brief  globals.h���� �и��� direct_ingredient�� ����ϴ� ĳ���� ���� ����
 * Created 2024/06/05
 */

#ifndef CAN_UN_D_H
#define	CAN_UN_D_H

//----------   ���� Default  ----------//
 #define COUNTRY_STR "CA"
//----------   ���� ���� ���  ----------//
 #define USE_CANADA_CALUNIT				    // Cal unit lb, using kg
 #define USE_CANADA_100GToggle			  // 100g/kg key change to toggle
 #define USE_USA_INGREDIENT				    // USA Direct Ingredient(PLU 4000EA, each ingredient data Max : 2100bytes)
 #define ETH_LED_INDICATOR_DISABLE		// wired lan link indicator disable for NTEP USA
//----------   ���� ���� �� ����  ----------//
 #define USE_LABEL_LENGTH_200MM			  // �� ���� ��� 200mm (���� 120mm)
 #define USE_USA_LABELFORMAT
 // USA/CANADA ���� �� ���
  #ifdef USE_USA_LABELFORMAT
    #undef USE_ENG_LABELFORMAT
    #undef USE_KOR_LABELFORMAT
  #endif
  #define USE_STANDARD_DIRECT_INGREDIENT
//----------   ���� ���� ���÷��� ����  ----------//
 #define USE_FONT_ID6_12BY28			    // Font ID 6 12x28 font ���
 #define USE_FONT_ID4_Arial				    // Font ID 4 Arial font ���
//----------   ���� ���� ���ڵ� ����  ----------//

//----------   ���� ���� Ư������ ����  ----------//

#endif /* CAN_UN_D_H */
