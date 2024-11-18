 /* 
 * File:   MANAGE_INVENTORY_CART_y.h
 * Author: JiMin JEONG
 * @brief  globals.h에서 분리된 2024년 기준 사용하지 않는 파일
 * Create 2024/06/12
 */

#ifndef MANAGE_INVENTORY_CART_y_H
#define	MANAGE_INVENTORY_CART_y_H

//----------   국가 Default  ----------//
 #define DEALER_VER_STRING	"(y)"
 #define COUNTRY_STR "  "
 #define USE_MANAGE_INVENTORY_DEFAULT
//----------   국가 전용 기능  ----------//
 #define USE_KOREA_VERSION_DISPLAY	        // KOREA 버전 표시 
 #define USE_GSMART_PLU         	        // Use GSMart PLU Structure(Kor)
 #define PLU_COMMODITY_LINE_NO 5	        // 상품명5단
 #define USE_SELLBYDATETIME_ZERO	        // Sell by Date, Sell by Time이 0일 때 인쇄 안하는 기능
 #define USE_CHECK_OUTOFPAPER_ON_LABEL	    // Label모드에서 Paper End 체크하는 기능. 즉시 멈춤
 #define USE_CHN_IO_BOARD		            // 중국 대차스케일용 I/O Board 사용
 #define USE_PB_BATTERY			            // Pb Battery  기능
//----------   국가 전용 라벨 설정  ----------//
 #define USE_KOR_LABELFORMAT		        // 국내 라벨포맷 사용(No.32~45 사용)
 #define USE_INVENTORY_LABEL		        // 재고관리라벨 발행 기능. 무게값, 단위중량 으로 수량 계산
//----------   국가 전용 디스플레이 설정  ----------//

//----------   국가 전용 바코드 설정  ----------//

//----------   국가 전용 특정업장 설정  ----------//

#endif  /* MANAGE_INVENTORY_CART_y_H */