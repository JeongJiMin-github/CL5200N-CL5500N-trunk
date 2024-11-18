 /* 
 * File:   UN_KOR_MART_kr.h
 * Author: idhwang
 * @brief  globals.h���� �и��� �ؿܿ� �ִ� ���� ��Ʈ ���� ���� ���� (���� ���� ���)
 * Created 2024/09/02
 */

#ifndef UN_KOR_MART_kr_H
#define UN_KOR_MART_kr_H

//----------   ���� Default  ----------//
 #define DEALER_VER_STRING	".kr"
 #define COUNTRY_STR "  "
//----------   ���� ���� ���  ----------//
 #define USE_TRACEABILITY						          // Use European Traceability
 #define USE_NUTRITION_FACT						        // Use Nutrition Fact
 #define USE_SELLBYDATETIME_ZERO					    // Sell by Date, Sell by Time�� 0�� �� �μ� ���ϴ� ���
 #define USE_TRANSFER_MENU_SETTING				    // MENU Setting Up/down ��� �߰�, *���� ������ ����.
 #define USE_CHECK_OUTOFPAPER_ON_LABEL			  // Label��忡�� Paper End üũ�ϴ� ���. ��� ����
 #define USE_INWEIGHTKEY_WITHOUT_INTERVAL		  // IN_Weight Key ���� Weight Interval�� ������� 1g������ ��� ����
 #define USE_TEXT_EMPHASIS						        // ~1 : Italic, ~2 : Bold, ~3 : Underline �����ϴ� ��� ���� ���� �����̽� �� ���Ͱ� ������ ���� ����ǥ�� �����Ѵ�
 #define USE_DISCOUNTED_TAX_FOR_DISCOUNTED_TOTAL
 //Remote monitoring & Remote Transfer parameter
 //	#define USE_REMOTE_KEY
 //	#define USE_REMOTE_DISP
 //	#define USE_ADMIN_EMAIL
 #define USE_TAX_RATE
 #define USE_REPORT_TAX
 #define USE_REPORT_PAY
 //----------   ���� ���� �� ����  ----------//
 #define USE_ENG_LABELFORMAT					      // ���� ������ ���(No.1~28 ���)		// CODE 0x407F(16511bytes)
 #define USE_KOR_LABELFORMAT					      // ���� ������ ���(No.32~45 ���)	// CODE 0x2314(8980bytes)
 #define USE_ONLY_LABEL
 //----------   ���� ���� ���÷��� ����  ----------//
 #define FONT_CODEPAGE CODEPAGE_0949_KOR
 #define USE_KOR_FONT
//----------   ���� ���� ���ڵ� ����  ----------//

//----------   ���� ���� Ư������ ����  ----------//

#endif /* UN_KOR_MART_kr_H */