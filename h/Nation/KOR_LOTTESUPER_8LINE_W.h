 /* 
 * File:   KOR_LOTTESUPER_8LINE_W.h
 * Author: JiMin JEONG
 * @brief  globals.h���� �и��� Lotte Super ��ǰ�� 8��(����ī ���) ���� ����
 * Created 2024/06/10
 */

#ifndef KOR_LOTTESUPER_8LINE_W_H
#define	KOR_LOTTESUPER_8LINE_W_H

//----------   ���� Default  ----------//
 #define DEALER_VER_STRING	"(W)"
 #define COUNTRY_STR "  "
 #define USE_LOTTESUPER_DEFAULT
//----------   ���� ���� ���  ----------//
 #define USE_KOREA_VERSION_DISPLAY				// KOREA ���� ǥ��
 #define USE_GSMART_PLU         				// Use GSMart PLU Structure(Kor)
 #define USE_NUTRITION_FACT						// Use Nutrition Fact
 #define USE_SELLBYDATETIME_ZERO				// Sell by Date, Sell by Time�� 0�� �� �μ� ���ϴ� ���
 #define USE_CHECK_OUTOFPAPER_ON_LABEL			// Label��忡�� Paper End üũ�ϴ� ���. ��� ����
 #define USE_KOREA_CL3000_STANDARD_KEY
 #define NOT_USE_TRACE_CL5200
 #define PLU_COMMODITY_LINE_NO 8				// ��ǰ��8��
 #define USE_LOTTEMART_BARCODE_FUNCTION
 #define USE_LOTTEMART_ITEMCODE13   			// Ext.barcode �ʵ带 13�ڸ� ITEMCODE�� ����ϴ� ���. ( �嵥������ ��� ��ǰ�������� �Ǹ��ڵ带 �޴� �뵵�� Ȱ�� )
 #define USE_LOTTESUPER_BARCODE_FUNCTION		// GS1 ���ڵ� �ϴ� HRI ������ ǥ���ϴ� ������ ������ �μ��ϴ� �ʵ� ���� ��� ��� ����
 #define USE_LOTTESUPER_PROHIBIT_CHANGE_PRICE	// �Ե����� ��û �Ű������� ��� (���ϸ�忡�� ���� Ű ��� ����, 1110, 1120 Menu ��й�ȣ�� ����)
//----------   ���� ���� �� ����  ----------//
 #define USE_KOR_LABELFORMAT					// ���� ������ ���(No.32~45 ���)
//----------   ���� ���� ���÷��� ����  ----------//
 #define FONT_CODEPAGE CODEPAGE_0949_KOR
 #define USE_KOR_FONT
//----------   ���� ���� ���ڵ� ����  ----------//
/*		 GS1 ���ڵ� ���� (�� ���ڵ� �ߺ� ���� �ȵ�)           */
//  #define USE_RSS_BARCODE
 #define USE_ONLY_RSS_EXPAND_BARCODE
/* ���ڵ� ������ ���� (���� ������ ������ ������ ��� �ּ�ó��) */
//  #define USE_BARCODE_LEN_50
//  #define USE_BARCODE_LEN_60
//  #define USE_BARCODE_LEN_70
 #define USE_BARCODE_LEN_90
/////////////////////////////////////////////////////////////
//----------   ���� ���� Ư������ ����  ----------//

#endif  /* KOR_LOTTESUPER_8LINE_W_H */
