 /* 
 * File:   CHN_UN.h
 * Author: JiMin JEONG
 * @brief  globals.h에서 분리된 영공 중국 전용 파일
 * Created 2024/06/05
 */

#ifndef CHN_UN_H
#define	CHN_UN_H

//----------   국가 Default  ----------//
 #define USE_CHINA_DEFAULT				    // PARAMETER SETTING
 #define COUNTRY_STR "CN"
//#define USE_CHINA_ADD_DEFAULT			    // PARAMETER SETTING (중국 추가 요청)
//#define USE_CHN_STANDARD_DEFAULT		    // PLU 5000 ea  -> USE_CHN_TRACE_STANDARD
//----------   국가 전용 기능  ----------//
 #define USE_WEIGHT_UNIT_500G			    // 중국 단가 500g 단위 추가 (1근 = 500g)
 #define USE_PROTOCOL_CHINA_DAHUA		    // 중국 전용 다화 프로토콜
 #define USE_CHINA_PINYIN				    // 중문 입력기
 #define USE_PLU_TIMELEN_CUT   			    // Sell by Time, Packed Time Length 1byte
 #define USE_BARCODE_DISCOUNT_RATE		    // Barcode에 Discount Rate 항목 추가
 #define USE_PRT_CHNIA_DISCOUNT_RATE		// Discount 값 라벨 인쇄 기능 추가
 #define USE_FIXED_PERCENT_KEY_SET		    // fixed Percent discount key 할인율 설정 기능 추가(1~99%)
 #define USE_PRT_CONVERT_WEIGHT_KG_G		// 무게 인쇄 변환 기능 (kg <-> g)
//  #define PLU_COMMODITY_LINE_NO 8			// 상품명8단 -> USE_CHN_TRACE_STANDARD
//  #define USE_GSMART_PLU					// Use GSMart PLU Structure(Kor) -> USE_CHN_TRACE_STANDARD
//  #define USE_FIXED_PERCENT_KEY			// fixed Percent discount key
//  #define USE_CHINA_ROUND   				// Chinese round price
//  #define USE_PROTOCOL_CL5000JR
//  #define USE_WEIGHING_PRINTER_COMM
//  #define USE_QRCODE_11SET_DEFAULT		// CL5200 QR Barcode Setting 11번 사용 (현재 16번)
//----------   국가 전용 라벨 설정  ----------//
//  #define USE_CHN_LABELFORMAT 	  		// China label Format (No.1~2)
//----------   국가 전용 디스플레이 설정  ----------//
 #define USE_CHN_FONT
 #define USE_GB2312_CHN_FONT				// simple chinese (gb2312)
//  #define USE_BIG5_CHN_FONT				// tranditional chinese (big5) -> 미국 차이나 타운, 홍콩, 대만 용
 #define FONT_CODEPAGE CODEPAGE_0936_CHN
//----------   국가 전용 바코드 설정  ----------//
 #define USE_BARCODE_UPRICE_OVER    		// when Unit Price Barcode print, valid digits exceeded  number of digits Print ' U ' to zero
 #define USE_BARCODE_LEN_50 
//----------   국가 전용 특정업장 설정  ----------//

#endif  /* CHN_UN_H */
