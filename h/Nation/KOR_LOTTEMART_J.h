 /* 
 * File:   KOR_LOTTEMART_J.h
 * Author: JiMin JEONG
 * @brief  globals.h에서 분리된 Lotte Mart(국내 수입육 + GS1 DataBar Expand) 전용 파일
 * Created 2024/06/10
 */

#ifndef KOR_LOTTEMART_J_H
#define	KOR_LOTTEMART_J_H

//----------   국가 Default  ----------//
 #define DEALER_VER_STRING	"(J)"
 #define COUNTRY_STR "  "
 #define USE_LOTTEMART_DEFAULT
//----------   국가 전용 기능  ----------//
 #define USE_KOREA_VERSION_DISPLAY	      // KOREA 버전 표시 
 #define USE_IMPORT_MEAT_PROTOCOL
 #define USE_LOTTEMART_BARCODE_FUNCTION
 #define USE_TRACE_STANDALONE   	        // 축산 이력기능
 #define USE_GSMART_PLU         	        // Use GSMart PLU Structure(Kor)
 #define PLU_COMMODITY_LINE_NO 5	        // 상품명5단
 #define USE_NUTRITION_FACT		            // Use Nutrition Fact
 #define USE_SELLBYDATETIME_ZERO	        // Sell by Date, Sell by Time이 0일 때 인쇄 안하는 기능
 #define USE_CHECKING_HARMFUL_MEAT	      // 수입쇠고기 위해차단 기능(위해리스트 저울 저장)
 #define USE_TRACE_MEATPART			          // 부위명 사용
 #define USE_TRACE_MEATPART_DEFAULT	      // 기본 부위명 11개 사용
 #define USE_MULTI_TRACE_NO_TABLE	        // Use Multi Trace Number Table with saving
 #define USE_TRACE_MEATGRADE		          // 국내 등급표시(2011.7.1시행)
 #define USE_TRACE_MEATGRADE_STANDARD_DEFAULT
 #define USE_HIDE_WIRELESS_PASSWORD	      // Wireless Security Password 입력시 '*'표시
 #define USE_CHECK_OUTOFPAPER_ON_LABEL	  // Label모드에서 Paper End 체크하는 기능. 즉시 멈춤
 #define USE_LOTTEMART_PROTOCOL		        // 실시간전송용 판매data는 저장되지 않고 전송되지 않음. 변경data는 전송.
 #define USE_LOTTEMART_PIG_TRACE	        // 돼지이력 및 LOT 14자리 추가
 #define USE_LOTTEMART_ITEMCODE13         // Ext.barcode 필드를 13자리 ITEMCODE로 사용하는 기능.
 #define USE_EMART_CHANGE_DATA_AT_NON_SALE
 #define USE_LOTTEMART_NETWORK
 #define USE_IMPORT_LOTNO                 //이력 번호 24자리 확장 기능. 이 기능 사용하면 이력정보 초기화후에 사용해야함. 메모리 초기화!!
 #define USE_EXTENDED_SCANNERFORM			    //스캐너 포맷 40개로 확장
 #define USE_LOTTEMART_MEATSCANNER        //롯데매트 전용 축산 스캐너 인식 기능  이력번호, 등급, 도축장3개 스캔, 저장되지 않음. 프린트하고 새로운 PLU 불러오면 클리어.
 #define USE_MARBLING_GRADE_PRINT
  #ifdef USE_MARBLING_GRADE_PRINT
    #define USE_EXPAND_MARBLING_LIST      // 마블링 등급리스트를 폭을 넓게 인쇄.
  #endif
 #define USE_DISCOUNT_MEMBERSHIP          // 회원할인 기능, 카드 할인 기능 적용 
 #undef USE_CTS_FUNCTION			            // 마트는 CTS 사용 안함
//----------   국가 전용 라벨 설정  ----------//

//----------   국가 전용 디스플레이 설정  ----------//
 #define FONT_CODEPAGE CODEPAGE_0949_KOR
 #define USE_KOR_FONT
//----------   국가 전용 바코드 설정  ----------//
/*		 GS1 바코드 설정 (두 바코드 중복 적용 안됨)           */
//  #define USE_RSS_BARCODE
 #define USE_ONLY_RSS_EXPAND_BARCODE
/* 바코드 데이터 길이 (길이 증가를 원하지 않으면 모두 주석처리) */
//  #define USE_BARCODE_LEN_50
//  #define USE_BARCODE_LEN_60
 #define USE_BARCODE_LEN_70
//  #define USE_BARCODE_LEN_90
/////////////////////////////////////////////////////////////
//----------   국가 전용 특정업장 설정  ----------//

#endif  /* KOR_LOTTEMART_J_H */