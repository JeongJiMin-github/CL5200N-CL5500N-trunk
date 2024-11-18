 /* 
 * File:   IND_DADUS_DM.h
 * Author: JiMin JEONG
 * @brief  globals.h���� �и��� DM �ε�(DADUS MITHAI VATIKA) ���� ����
 * Create 2024/06/04
 */

#ifndef IND_DADUS_DM_H
#define	IND_DADUS_DM_H

//----------   ���� Default  ----------//
 #define DEALER_VER_STRING	".DM"
 #define COUNTRY_STR "  "
 #define USE_INDIA_DADUS_DEFAULT
//----------   ���� ���� ���  ----------//
 #define USE_TAX_RATE							// Use Tax Rate
 #define USE_REPORT_TAX							// TAX Report ���
 #define USE_REPORT_PAY			 				// PAY/CHANGE Report ���
 #define USE_TRACEABILITY						// Use European Traceability
 #define USE_NUTRITION_FACT						// Use Nutrition Fact
 #define USE_SELLBYDATETIME_ZERO				// Sell by Date, Sell by Time�� 0�� �� �μ� ���ϴ� ���
 #define USE_TRANSFER_MENU_SETTING				// MENU Setting Up/down ��� �߰�, *���� ������ ����.
 #define USE_CHECK_OUTOFPAPER_ON_LABEL			// Label��忡�� Paper End üũ�ϴ� ���. ��� ����
 #define USE_INWEIGHTKEY_WITHOUT_INTERVAL		// IN_Weight Key ���� Weight Interval�� ������� 1g������ ��� ����
 #define USE_TEXT_EMPHASIS						// ~1 : Italic, ~2 : Bold, ~3 : Underline �����ϴ� ��� ���� ���� �����̽� �� ���Ͱ� ������ ���� ����ǥ�� �����Ѵ�
 #define USE_DISCOUNTED_TAX_FOR_DISCOUNTED_TOTAL
 #define USE_CTS_FUNCTION						// Calibration Traceability System	
 #define USE_INDIA_FUNCTION						// India Function(Item Code 13, Area Scale Mode)
 #define USE_DISPLAY_OPT_CL5200					// CL5200 Diplay �ӵ� ����
 #define USE_QUICK_DELETE_PLU					// PLU Delete all �ӵ� ����
 #define USE_INDIA_PRINT_UNIT					// �ε� ���� ȭ�� �� �ܰ� ǥ��
 #define USE_INGREDIENT_CODE_4DIGIT				// ������ 510byte 999��, ���� index��ȣ�� 4�ڸ� (1~9999��) ��� ����
 //#define USE_INDIA_TAX						// �ε� ���� ���� (CGST, SGST, GST)
 //#define USE_INDIA_TAX_REPORT_MENU			// �ε� TAX �޴� �߰� 
 #define USE_CHECK_DUPLICATION_TICKET			// duplication ticket ��� ����� ���, print �Ŀ� ���� ���θ� ����ڰ� �Ź� ����.
 #define USE_COPYKEY							// copy ticket ����� Ticket�� Copy ǥ��
 #define USE_PRINT_FILL_WEIGHT					// Fixed Weight �跮 �� �� ���� �� Gross Weight Field�� �̿��Ͽ� "Fill Weight"�� �μ�
 #define USE_QR_BARCODE_ERASE_GAP				// QR Add Total �μ� �� QR ���ڵ尡 �μ�Ǵ� ��ġ�� �ִ��� �÷��� ���� ������ ���ּ� �μ�ǰ� �� - ������ Ƽ�� ���� �Ƴ��� ���� (�ε� ��û)
 #define USE_PRT_SCALE_NUM_IN_REPORT			// Print Scale Number in report
 #define USE_PRT_SCALE_NUM_IN_TICKET			// Ƽ�� �μ� �� Scale No ��� (�ε� ��û)
//----------   ���� ���� �� ����  ----------//
 #define USE_ENG_LABELFORMAT					// ���� ������ ���(No.1~28 ���)		// CODE 0x407F(16511bytes)
 #define USE_KOR_LABELFORMAT					// ���� ������ ���(No.32~45 ���)	// CODE 0x2314(8980bytes)
//----------   ���� ���� ���÷��� ����  ----------//
 #define FONT_CODEPAGE CODEPAGE_1252_IND
//----------   ���� ���� ���ڵ� ����  ----------//
  #ifdef USE_QR_BARCODE
    #define USE_QR_ADDTOTAL						// QR���ڵ� �̿��� ADD TOTAL ���
  #endif
/*		 GS1 ���ڵ� ���� (�� ���ڵ� �ߺ� ���� �ȵ�)           */
//  #define USE_RSS_BARCODE
//  #define USE_ONLY_RSS_EXPAND_BARCODE
/* ���ڵ� ������ ���� (���� ������ ������ ������ ��� �ּ�ó��) */
 #define USE_BARCODE_LEN_50
//  #define USE_BARCODE_LEN_60
//  #define USE_BARCODE_LEN_70
//  #define USE_BARCODE_LEN_90
/////////////////////////////////////////////////////////////
//----------   ���� ���� Ư������ ����  ----------//

#endif  /* IND_DADUS_DM */