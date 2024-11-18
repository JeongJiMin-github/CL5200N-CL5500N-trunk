 /* 
 * File:   CHN_TRACE_STANDARD_F.h
 * Author: JiMin JEONG
 * @brief  globals.h에서 분리된 chinese trace standard (gb2312) <= chn E-Mart 전용 파일
 * Create 2024/06/04
 */

#ifndef CHN_TRACE_STANDARD_F_H
#define	CHN_TRACE_STANDARD_F_H

//----------   국가 Default  ----------//
  #ifdef USE_CHN_CERTIFICATION
    #define DEALER_VER_STRING	"   "
  #else
    #define DEALER_VER_STRING	"(F)"
  #endif
 #define COUNTRY_STR "  "
 #define USE_CHINA_DEFAULT                  // PARAMETER SETTING
 #define USE_CHN_TRACE_DEFAULT 				// PLU 1000 ea with trace code
//----------   국가 전용 기능  ----------//
 #define USE_GSMART_PLU         	        // Use GSMart PLU Structure(Kor)
 #define PLU_COMMODITY_LINE_NO 8	        // 상품명8단
 #define USE_TAX_RATE				        // Use Tax Rate
 #define USE_DISCOUNTED_TAX_FOR_DISCOUNTED_TOTAL
 #define USE_SELLBYDATETIME_ZERO	        // Sell by Date, Sell by Time이 0일 때 인쇄 안하는 기능
 #define USE_KEEPING_CONNETION  	        // 전송데이터가 계속 있을 때 Remote와 Connection 유지
 // #define USE_REPORT_TAX         	        // TAX Report 사용
 // #define USE_REPORT_PAY         	        // PAY/CHANGE Report 사용
 #define USE_BARCODE_UPRICE_OVER	        // Unit Price Barcode가 자리수 초과할 경우 'U'를 0으로 인쇄
 #define USE_FIXED_PERCENT_KEY	            // fixed Percent discount key
 #define DISABLE_PLUTYPE_CHANGE	            // Disable Changing PLU Type(count <-> weight)
 #define USE_CHN_EMART_TRACE                // 중국 이마트 이력관리 기능
 #define USE_CHN_EMART_BIRDS_TRACE          //중국 이마트 조류 이력 관리  //20140827
 #define USE_INWEIGHTKEY_WITHOUT_INTERVAL   // IN_Weight Key 사용시 Weight Interval에 상관없이 1g단위로 사용 가능
 #define USE_WEIGHT_UNIT_500G               // 중국 단가 500g 단위 추가 (1근 = 500g)
 #define USE_PROTOCOL_CHINA_DAHUA           // 중국 전용 다화 프로토콜
 #define USE_CHINA_PINYIN                   // 중문 입력기
 //#define USE_CHINA_ROUND                  // 중국 전용 반올림 계산
 #define USE_PLU_TIMELEN_CUT                // Sell by Time, Packed Time Length 1byte로 조정
 #define USE_WEIGHING_PRINTER_COMM
 #define USE_CHECK_OUTOFPAPER_ON_LABEL		// Label mode, check Paper end
 #define USE_TRANSFER_MENU_SETTING			// MENU Setting Upload/download
//----------   국가 전용 라벨 설정  ----------//
 // #define USE_ENG_LABELFORMAT		        // 영공 라벨포맷 사용(No.1~28 사용)
 // #define USE_KOR_LABELFORMAT		        // 국내 라벨포맷 사용(No.32~45 사용)
 #define USE_CHN_LABELFORMAT 	  			// China label Format (No.1~2)
//----------   국가 전용 디스플레이 설정  ----------//
 #define USE_CHN_FONT
 #define FONT_CODEPAGE CODEPAGE_0936_CHN
 #define USE_GB2312_CHN_FONT                // simple chinese (gb2312)
 //#define USE_BIG5_CHN_FONT                // tranditional chinese (big5)
//----------   국가 전용 바코드 설정  ----------//
/*		    GS1 바코드 설정 (두 바코드 중복 적용 안됨)           */
//  #define USE_RSS_BARCODE
//  #define USE_ONLY_RSS_EXPAND_BARCODE
/* 바코드 데이터 길이 (길이 증가를 원하지 않으면 모두 주석처리) */
 #define USE_BARCODE_LEN_50
//  #define USE_BARCODE_LEN_60
//  #define USE_BARCODE_LEN_70
//  #define USE_BARCODE_LEN_90
/////////////////////////////////////////////////////////////
//----------   국가 전용 특정업장 설정  ----------//

#endif /* CHN_TRACE_STANDARD_F_H */