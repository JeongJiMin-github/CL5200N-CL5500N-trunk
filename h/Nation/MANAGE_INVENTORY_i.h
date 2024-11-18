 /* 
 * File:   MANAGE_INVENTORY_i.h
 * Author: JiMin JEONG
 * @brief  globals.h에서 분리된 자재 재고관리 라벨 발행 기능 전용 파일
 * Create 2024/06/04
 */

#ifndef MANAGE_INVENTORY_i_H
#define	MANAGE_INVENTORY_i_H

//----------   국가 Default  ----------//
 #define DEALER_VER_STRING	"(i)"
 #define COUNTRY_STR "  "
 #define USE_MANAGE_INVENTORY_DEFAULT
//----------   국가 전용 기능  ----------//
 #define USE_KOREA_VERSION_DISPLAY	    // KOREA 버전 표시 
 //#define USE_CHECK_SCANNING_DATA	    // SCANNER Disabel CR, Enable LF일 때만 동작하게 함
 #define USE_GSMART_PLU         	    // Use GSMart PLU Structure(Kor)
 #define PLU_COMMODITY_LINE_NO 5	    // 상품명5단
 #define USE_SELLBYDATETIME_ZERO	    // Sell by Date, Sell by Time이 0일 때 인쇄 안하는 기능
 //#define USE_RFID_TAG_READER		    // Use RFID Reader(한맥 ENG)
 //#define USE_MAX_SCALE_NO_255		    // 저울번호 255개 사용
 //#define USE_TRACE_SCANNER_FOR_GRADE	// 바코드 스캐너를 이용해 등급을 호출하는 기능
 #define USE_INVENTORY_LABEL		    //재고관리라벨 발행 기능. 무게값, 단위중량 으로 수량 계산
//----------   국가 전용 라벨 설정  ----------//
 //#define USE_ENG_LABELFORMAT		    // 영공 라벨포맷 사용
 #define USE_KOR_LABELFORMAT		    // 국내 라벨포맷 사용(No.32~45 사용)
//----------   국가 전용 디스플레이 설정  ----------//

//----------   국가 전용 바코드 설정  ----------//

//----------   국가 전용 특정업장 설정  ----------//

 #endif /* MANAGE_INVENTORY_i_H */