 /* 
 * File:   UKR_Z_D.h
 * Author: JiMin JEONG
 * @brief  globals.h���� �и��� direct_ingredient�� ����ϴ� ��ũ���̳� ���� ����
 * Created 2024/06/05
 */

#ifndef UKR_Z_D_H
#define	UKR_Z_D_H

//----------   ���� Default  ----------//
 #define DEALER_VER_STRING	"(Z)"
 #define USE_RUSSIA_DEFAULT			                // default set for russia ��ũ���̳��� ���þ� ��ɰ� ����
 #define USE_UKRAINE_DEFAULT
 #define COUNTRY_STR "  "
//----------   ���� ���� ���  ----------//
 #define USE_NUTRITION_FACT			                // Use Nutrition Fact
 #define USE_JULIAN_DATE			                // ���ڵ� �μ�� julian date ��� parameter 529���� ���� 0: ����, 1:DDDYY(default), 2:DDDTT, 3: YMMDD
 #define USE_SELLBYDATETIME_ZERO	              	// Sell by Date, Sell by Time�� 0�� �� �μ� ���ϴ� ���
 #define USE_SELLBYDATE_CHANGE		              	// Sell by Date �ڸ����� ���� ����(Ignore PARAM#532, 6�ڸ� : YYMMDD, 4�ڸ� : �����Ͽ� ����)
 #define USE_PROTOCOL_CL5000JR		              	// for CL5000J Protocol
 #define USE_INWEIGHTKEY_WITHOUT_INTERVAL	      	// IN_Weight Key ���� Weight Interval�� ������� 1g������ ��� ����
 #define USE_RUSSIA_BARCODE_FUNCTION	          	// �߰� �ɹ� �߰� (decimal Point, sell by date. packing date, produce date)
 #define USE_PERCENT_WEIGHT			                // ���� percent Tare ����� ���þƿ��� percent Weight �� ���
 #define USE_DMSG_2100_BYTE			                // Direct Ingredient 2100��
 #define USE_REMOTE_KEY
 #define USE_REMOTE_DISP
 #define USE_ADMIN_EMAIL
 #define USE_TRANSFER_MENU_SETTING
 #define USE_KEY_BLOCK_IN_SELFSERVICE_CL5000JR
 #define USE_SALE_MIN_WEIGHT_AT_AUTO_MODE       	// ī���彺ź ��û����: �� Auto ��忡�� Min.Weight ���� �� �Ǹ� �����ϰ� �����ϴ� ���
 #define USE_DIRECT_INGREDIENT				        // Use direct ingredient
//----------   ���� ���� �� ����  ----------//

//----------   ���� ���� ���÷��� ����  ----------//
 #define USE_PRICE_TOTAL_7DIGIT		              	// Total 7�ڸ� ���
 #define USE_PRICE_UNIT_7DIGIT	                	// Unit Price 7�ڸ� ���
 #define USE_PRICECHECK_8DIGIT	                	// Price Checkdigit(8digit)
 #define FONT_CODEPAGE CODEPAGE_1251
//----------   ���� ���� ���ڵ� ����  ----------//
/////////////////////////////////////////////////////////////
/*		 GS1 ���ڵ� ���� (�� ���ڵ� �ߺ� ���� �ȵ�)           */
//  #define USE_RSS_BARCODE
 #define USE_ONLY_RSS_EXPAND_BARCODE
/* ���ڵ� ������ ���� (���� ������ ������ ������ ��� �ּ�ó��) */
//  #define USE_BARCODE_LEN_50
 #define USE_BARCODE_LEN_60
//  #define USE_BARCODE_LEN_70
//  #define USE_BARCODE_LEN_90
/////////////////////////////////////////////////////////////
  #ifdef USE_QR_BARCODE
    #define USE_QR_ADDTOTAL				            // QR���ڵ� �̿��� ADD TOTAL ���
  #endif /* USE_QR_BARCODE */
//----------   ���� ���� Ư������ ����  ----------//

#endif /* UKR_Z_D_H */
