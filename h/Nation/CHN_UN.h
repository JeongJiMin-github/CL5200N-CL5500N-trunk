 /* 
 * File:   CHN_UN.h
 * Author: JiMin JEONG
 * @brief  globals.h���� �и��� ���� �߱� ���� ����
 * Created 2024/06/05
 */

#ifndef CHN_UN_H
#define	CHN_UN_H

//----------   ���� Default  ----------//
 #define USE_CHINA_DEFAULT				    // PARAMETER SETTING
 #define COUNTRY_STR "CN"
//#define USE_CHINA_ADD_DEFAULT			    // PARAMETER SETTING (�߱� �߰� ��û)
//#define USE_CHN_STANDARD_DEFAULT		    // PLU 5000 ea  -> USE_CHN_TRACE_STANDARD
//----------   ���� ���� ���  ----------//
 #define USE_WEIGHT_UNIT_500G			    // �߱� �ܰ� 500g ���� �߰� (1�� = 500g)
 #define USE_PROTOCOL_CHINA_DAHUA		    // �߱� ���� ��ȭ ��������
 #define USE_CHINA_PINYIN				    // �߹� �Է±�
 #define USE_PLU_TIMELEN_CUT   			    // Sell by Time, Packed Time Length 1byte
 #define USE_BARCODE_DISCOUNT_RATE		    // Barcode�� Discount Rate �׸� �߰�
 #define USE_PRT_CHNIA_DISCOUNT_RATE		// Discount �� �� �μ� ��� �߰�
 #define USE_FIXED_PERCENT_KEY_SET		    // fixed Percent discount key ������ ���� ��� �߰�(1~99%)
 #define USE_PRT_CONVERT_WEIGHT_KG_G		// ���� �μ� ��ȯ ��� (kg <-> g)
//  #define PLU_COMMODITY_LINE_NO 8			// ��ǰ��8�� -> USE_CHN_TRACE_STANDARD
//  #define USE_GSMART_PLU					// Use GSMart PLU Structure(Kor) -> USE_CHN_TRACE_STANDARD
//  #define USE_FIXED_PERCENT_KEY			// fixed Percent discount key
//  #define USE_CHINA_ROUND   				// Chinese round price
//  #define USE_PROTOCOL_CL5000JR
//  #define USE_WEIGHING_PRINTER_COMM
//  #define USE_QRCODE_11SET_DEFAULT		// CL5200 QR Barcode Setting 11�� ��� (���� 16��)
//----------   ���� ���� �� ����  ----------//
//  #define USE_CHN_LABELFORMAT 	  		// China label Format (No.1~2)
//----------   ���� ���� ���÷��� ����  ----------//
 #define USE_CHN_FONT
 #define USE_GB2312_CHN_FONT				// simple chinese (gb2312)
//  #define USE_BIG5_CHN_FONT				// tranditional chinese (big5) -> �̱� ���̳� Ÿ��, ȫ��, �븸 ��
 #define FONT_CODEPAGE CODEPAGE_0936_CHN
//----------   ���� ���� ���ڵ� ����  ----------//
 #define USE_BARCODE_UPRICE_OVER    		// when Unit Price Barcode print, valid digits exceeded  number of digits Print ' U ' to zero
 #define USE_BARCODE_LEN_50 
//----------   ���� ���� Ư������ ����  ----------//

#endif  /* CHN_UN_H */
