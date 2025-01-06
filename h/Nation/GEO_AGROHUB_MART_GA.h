 /* 
 * File:   GEO_AGROHUB_MART_GA.h
 * Author: JiMin JEONG
 * @brief  ���� ������(�׷�����)���� �и��� �׷����� Agrohub ��Ʈ ���� ����
 */

#ifndef GEO_AGROHUB_MART_GA_H
#define	GEO_AGROHUB_MART_GA_H

//----------   ���� Default  ----------//
 #define USE_GEORGIA_AGROHUB_DEFAULT				    // �׷����� Agrohub ��Ʈ ������ �߰�
 #define DEALER_VER_STRING	".GA"
 #define COUNTRY_STR "  "
//----------   ���� ���� ���  ----------//
 #define USE_TRACEABILITY						        // Use European Traceability
 #define USE_NUTRITION_FACT						        // Use Nutrition Fact
 #define USE_GEORGIA_MART_SELLBYDATE_FUNCTION           // 0: Pakced_Date, 1: Pakced_Date + 1, 2: Pakced_Date + 2, ... , 900~: No_Pirnt
 #define USE_SELLBYDATETIME_ZERO					    // Sell by Date, Sell by Time�� 0�� �� �μ� ���ϴ� ���
 #define USE_TRANSFER_MENU_SETTING				        // MENU Setting Up/down ��� �߰�, *���� ������ ����.
 #define USE_CHECK_OUTOFPAPER_ON_LABEL			        // Label��忡�� Paper End üũ�ϴ� ���. ��� ����
 #define USE_INWEIGHTKEY_WITHOUT_INTERVAL		        // IN_Weight Key ���� Weight Interval�� ������� 1g������ ��� ����
 #define USE_TEXT_EMPHASIS						        // ~1 : Italic, ~2 : Bold, ~3 : Underline �����ϴ� ��� ���� ���� �����̽� �� ���Ͱ� ������ ���� ����ǥ�� �����Ѵ�
 #define USE_DISCOUNTED_TAX_FOR_DISCOUNTED_TOTAL
 #define USE_TAX_RATE						            // Use Tax Rate
 #define USE_REPORT_TAX						            // TAX Report ���
 #define USE_REPORT_PAY						            // PAY/CHANGE Report ���

 #ifdef USE_EXTEND_8M_DFLASH
  #define USE_LABEL_FORMAT_EXT 			            // �� ���� Ȯ��(4~8MB ����, 20�� �߰� Ȯ��)
 #endif
//----------   ���� ���� �� ����  ----------//
 #define USE_ENG_LABELFORMAT				            // ���� ������ ���(No.1~28 ���)		// CODE 0x407F(16511bytes)
 #define USE_KOR_LABELFORMAT				            // ���� ������ ���(No.32~45 ���)	// CODE 0x2314(8980bytes)
//----------   ���� ���� ���÷��� ����  ----------//
 #define FONT_CODEPAGE CODEPAGE_1251_GEO                // �׷����� ���� ��Ʈ
//----------   ���� ���� ���ڵ� ����  ----------//

//----------   ���� ���� Ư������ ����  ----------//

#endif  /* GEO_AGROHUB_MART_GA_H */
