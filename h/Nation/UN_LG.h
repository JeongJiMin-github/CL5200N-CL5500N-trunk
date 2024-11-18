 /* 
 * File:   UN_LG.h
 * Author: JiMin JEONG
 * @brief  globals.h���� �и��� ������� ���� ����(LOGISTICS) ����
 * Created 2024/06/05
 */

#ifndef UN_LG_H
#define	UN_LG_H

//----------   ���� Default  ----------//
 #define USE_UN_LOGISTICS_DEFAULT
 #define DEALER_VER_STRING  ".LG"
 #define COUNTRY_STR "  "
//----------   ���� ���� ���  ----------//
 #define USE_TRACEABILITY						            // Use European Traceability
 #define USE_NUTRITION_FACT						            // Use Nutrition Fact
 #define USE_SELLBYDATETIME_ZERO					        // Sell by Date, Sell by Time�� 0�� �� �μ� ���ϴ� ���
 #define USE_TRANSFER_MENU_SETTING				            // MENU Setting Up/down ��� �߰�, *���� ������ ����.
 #define USE_CHECK_OUTOFPAPER_ON_LABEL			            // Label��忡�� Paper End üũ�ϴ� ���. ��� ����
 #define USE_INWEIGHTKEY_WITHOUT_INTERVAL		            // IN_Weight Key ���� Weight Interval�� ������� 1g������ ��� ����
 #define USE_TEXT_EMPHASIS						            // ~1 : Italic, ~2 : Bold, ~3 : Underline �����ϴ� ��� ���� ���� �����̽� �� ���Ͱ� ������ ���� ����ǥ�� �����Ѵ�
 #define USE_DISCOUNTED_TAX_FOR_DISCOUNTED_TOTAL
 //Remote monitoring & Remote Transfer parameter
 //	#define USE_REMOTE_KEY
 //	#define USE_REMOTE_DISP
 //	#define USE_ADMIN_EMAIL
 #define USE_TAX_RATE						                // Use Tax Rate
 #define USE_REPORT_TAX						                // TAX Report ���
 #define USE_REPORT_PAY						                // PAY/CHANGE Report ���
  #ifdef USE_EXTEND_8M_DFLASH
    #define USE_LABEL_FORMAT_EXT 			                // �� ���� Ȯ��(4~8MB ����, 20�� �߰� Ȯ��)
  #endif
 #define USE_EUR_LOGISTICS_PLU                              // ������� Logistics(��������) PLU(8000��) ���
 #define USE_LOGISTICS_BARCODE
//----------   ���� ���� �� ����  ----------//
 #define USE_ENG_LABELFORMAT				                // ���� ������ ���(No.1~28 ���)		// CODE 0x407F(16511bytes)
 #define USE_KOR_LABELFORMAT				                // ���� ������ ���(No.32~45 ���)	// CODE 0x2314(8980bytes)
//----------   ���� ���� ���÷��� ����  ----------//
 #define FONT_CODEPAGE CODEPAGE_1252
//----------   ���� ���� ���ڵ� ����  ----------//
/*  		    GS1 ���ڵ� ���� (�� ���ڵ� �ߺ� ���� �ȵ�)           */
//  #define USE_RSS_BARCODE
//  #define USE_ONLY_RSS_EXPAND_BARCODE
/* ���ڵ� ������ ���� (���� ������ ������ ������ ��� �ּ�ó��) */
//  #define USE_BARCODE_LEN_50
//  #define USE_BARCODE_LEN_60
//  #define USE_BARCODE_LEN_70
//  #define USE_BARCODE_LEN_90                   // ���� ������ 70�ڱ��� ��� ���� (24�� 6������)
/////////////////////////////////////////////////////////////
//----------   ���� ���� Ư������ ����  ----------//

#endif  /* UN_LG_H */