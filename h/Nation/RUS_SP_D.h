 /* 
 * File:   RUS_SP_D.h
 * Author: JiMin JEONG
 * @brief  globals.h���� �и��� direct_ingredient�� ����ϴ� ���þ� SPAR ��Ʈ ���� ����
 * Created 2024/06/10
 */

#ifndef RUS_SP_D_H
#define	RUS_SP_D_H

//----------   ���� Default  ----------//
 #define DEALER_VER_STRING	".SP"
 #define COUNTRY_STR "  "
 #define USE_RUSSIA_DEFAULT			                // default set for russia
//----------   ���� ���� ���  ----------//
 #define USE_NUTRITION_FACT			                // Use Nutrition Fact
 // #define USE_TAX_RATE				            // Use Tax Rate
 #define USE_SELLBYDATETIME_ZERO	                // Sell by Date, Sell by Time�� 0�� �� �μ� ���ϴ� ���
 #define USE_SELLBYDATE_CHANGE		                // Sell by Date �ڸ����� ���� ����(Ignore PARAM#532, 6�ڸ� : YYMMDD, 4�ڸ� : �����Ͽ� ����)
 #define USE_PROTOCOL_CL5000JR		                // for CL5000J Protocol
 #define USE_INWEIGHTKEY_WITHOUT_INTERVAL	        // IN_Weight Key ���� Weight Interval�� ������� 1g������ ��� ����
 #define USE_RUSSIA_BARCODE_FUNCTION	            // �߰� �ɹ� �߰� (decimal Point, sell by date. packing date, produce date)
 #define USE_PERCENT_WEIGHT			                //���� percent Tare ����� ���þƿ��� percent Weight �� ���
 #define USE_DMSG_2100_BYTE			                //Direct Ingredient 2100��
 #define USE_JULIAN_DATE
 //	#define USE_CONTINUOUS_LABEL_WITH_INGREDIENT
 #define USE_SPAR_MEMBER_PRICE
 #define USE_REMOTE_KEY
 #define USE_REMOTE_DISP
 #define USE_ADMIN_EMAIL
 #define USE_TRANSFER_MENU_SETTING
 #define USE_SALE_MIN_WEIGHT_AT_AUTO_MODE           //ī���彺ź ��û����: �� Auto ��忡�� Min.Weight ���� �� �Ǹ� �����ϰ� �����ϴ� ���
 #define USE_RUSSIA_VER_DISPLAY		                // Russia ���� ǥ�� ���. �ܺ� ǥ�� ������ ������.
 #define USE_CL5J_REALTIME_TRNASFER
 #define USE_DIRECT_INGREDIENT				        // Use direct ingredient
 #define USE_INGREDIENT_98_PROTOCOL			        // PLU ��� �� ��� Direct ingredient�� 98�� ���(protocol �Ŵ��� ����)
 #define USE_PROHIBIT_AUTO_SERVER_CHECK		        // Spar��Ʈ ��û����: RTT ��� �Ǹŵ����� �߻� �ÿ��� ������ ��� ���� �� ���� ���� (�ڵ� ����üũ ��� ���� ��û)
 #define USE_KEEPING_CONNETION						// ���۵����Ͱ� ��� ���� �� Remote�� Connection ����
//----------   ���� ���� �� ����  ----------//

//----------   ���� ���� ���÷��� ����  ----------//
 #define FONT_CODEPAGE CODEPAGE_1251
 #define USE_PRICE_TOTAL_7DIGIT		                // Total 7�ڸ� ���
 #define USE_PRICE_UNIT_7DIGIT	                    // Unit Price 7�ڸ� ���
 #define USE_PRICECHECK_8DIGIT	                    // Price Checkdigit(8digit)
//----------   ���� ���� ���ڵ� ����  ----------//
/*		    GS1 ���ڵ� ���� (�� ���ڵ� �ߺ� ���� �ȵ�)           */
//  #define USE_RSS_BARCODE
//  #define USE_ONLY_RSS_EXPAND_BARCODE
/* ���ڵ� ������ ���� (���� ������ ������ ������ ��� �ּ�ó��) */
//  #define USE_BARCODE_LEN_50
 #define USE_BARCODE_LEN_60
//  #define USE_BARCODE_LEN_70
//  #define USE_BARCODE_LEN_90
/////////////////////////////////////////////////////////////
//----------   ���� ���� Ư������ ����  ----------//

#endif  /* RUS_SP_D_H */
