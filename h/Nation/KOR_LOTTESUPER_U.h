 /* 
 * File:   KOR_LOTTESUPER_U.h
 * Author: JiMin JEONG
 * @brief  globals.h에서 분리된 Lotte Super(국내 수입육에서 파생) 전용 파일
 * Created 2024/06/10
 */

#ifndef KOR_LOTTESUPER_U_H
#define	KOR_LOTTESUPER_U_H

//----------   국가 Default  ----------//
 #define DEALER_VER_STRING	"(U)"
 #define COUNTRY_STR "  "
 #define USE_LOTTESUPER_DEFAULT
 #define USE_TRACE_MEATPART_DEFAULT	            // 기본 부위명 11개 사용
 #define USE_TRACE_MEATGRADE_STANDARD_DEFAULT
//----------   국가 전용 기능  ----------//
 #define USE_KOREA_VERSION_DISPLAY	            // KOREA 버전 표시 
 #define USE_IMPORT_MEAT_PROTOCOL
 #define USE_TRACE_STANDALONE   	            // 축산 이력기능
 #define USE_GSMART_PLU         	            // Use GSMart PLU Structure(Kor)
 #define PLU_COMMODITY_LINE_NO 8	            // 상품명5단
 #define USE_SELLBYDATETIME_ZERO	            // Sell by Date, Sell by Time이 0일 때 인쇄 안하는 기능
 #define USE_CHECKING_HARMFUL_MEAT	            // 수입쇠고기 위해차단 기능(위해리스트 저울 저장)
 #define USE_TRACE_MEATPART			            // 부위명 사용
 #define USE_MULTI_TRACE_NO_TABLE	            // Use Multi Trace Number Table with saving
 #define USE_TRACE_MEATGRADE		            // 국내 등급표시(2011.7.1시행)
 #define USE_CHECK_OUTOFPAPER_ON_LABEL	        // Label모드에서 Paper End 체크하는 기능. 즉시 멈춤
 #define USE_TRACE_PIG_STANDALONE
 #define USE_MARBLING_GRADE_PRINT
  #ifdef USE_MARBLING_GRADE_PRINT
    #define USE_EXPAND_MARBLING_LIST            // 마블링 등급리스트를 폭을 넓게 인쇄.
  #endif
 #define USE_LOTTEMART_BARCODE_FUNCTION
 #define USE_LOTTEMART_ITEMCODE13   		    // Ext.barcode 필드를 13자리 ITEMCODE로 사용하는 기능. ( 룟데슈퍼의 경우 상품마스터의 판매코드를 받는 용도로 활용 )
 #define USE_LOTTESUPER_BARCODE_FUNCTION	    // GS1 바코드 하단 HRI 영역에 표현하는 데이터 영역에 인쇄하는 필드 전용 출력 기능 구현
 #define USE_LOTTESUPER_PROHIBIT_CHANGE_PRICE   // 롯데슈퍼 요청 매가통지용 기능 (세일모드에서 관련 키 기능 막음, 1110, 1120 Menu 비밀번호로 막음)
 #undef USE_CTS_FUNCTION 		  				// 마트는 CTS 사용 안함
//----------   국가 전용 라벨 설정  ----------//
 #define USE_KOR_LABELFORMAT		            // 국내 라벨포맷 사용(No.32~45 사용)
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
//  #define USE_BARCODE_LEN_70
 #define USE_BARCODE_LEN_90
/////////////////////////////////////////////////////////////
//----------   국가 전용 특정업장 설정  ----------//
 
#endif  /* KOR_LOTTESUPER_U_H */