 /* 
 * File:   RUS_SP_D.h
 * Author: JiMin JEONG
 * @brief  globals.h에서 분리된 direct_ingredient를 사용하는 러시아 SPAR 마트 전용 파일
 * Created 2024/06/10
 */

#ifndef RUS_SP_D_H
#define	RUS_SP_D_H

//----------   국가 Default  ----------//
 #define DEALER_VER_STRING	".SP"
 #define COUNTRY_STR "  "
 #define USE_RUSSIA_DEFAULT			                // default set for russia
//----------   국가 전용 기능  ----------//
 #define USE_NUTRITION_FACT			                // Use Nutrition Fact
 // #define USE_TAX_RATE				            // Use Tax Rate
 #define USE_SELLBYDATETIME_ZERO	                // Sell by Date, Sell by Time이 0일 때 인쇄 안하는 기능
 #define USE_SELLBYDATE_CHANGE		                // Sell by Date 자리수에 동작 변경(Ignore PARAM#532, 6자리 : YYMMDD, 4자리 : 생산일에 더함)
 #define USE_PROTOCOL_CL5000JR		                // for CL5000J Protocol
 #define USE_INWEIGHTKEY_WITHOUT_INTERVAL	        // IN_Weight Key 사용시 Weight Interval에 상관없이 1g단위로 사용 가능
 #define USE_RUSSIA_BARCODE_FUNCTION	            // 추가 심벌 추가 (decimal Point, sell by date. packing date, produce date)
 #define USE_PERCENT_WEIGHT			                //영공 percent Tare 기능을 러시아에서 percent Weight 로 사용
 #define USE_DMSG_2100_BYTE			                //Direct Ingredient 2100자
 #define USE_JULIAN_DATE
 //	#define USE_CONTINUOUS_LABEL_WITH_INGREDIENT
 #define USE_SPAR_MEMBER_PRICE
 #define USE_REMOTE_KEY
 #define USE_REMOTE_DISP
 #define USE_ADMIN_EMAIL
 #define USE_TRANSFER_MENU_SETTING
 #define USE_SALE_MIN_WEIGHT_AT_AUTO_MODE           //카자흐스탄 요청사항: 라벨 Auto 모드에서 Min.Weight 이하 값 판매 가능하게 설정하는 기능
 #define USE_RUSSIA_VER_DISPLAY		                // Russia 버전 표기 사용. 외부 표시 버전은 고정됨.
 #define USE_CL5J_REALTIME_TRNASFER
 #define USE_DIRECT_INGREDIENT				        // Use direct ingredient
 #define USE_INGREDIENT_98_PROTOCOL			        // PLU 통신 할 경우 Direct ingredient는 98을 사용(protocol 매뉴얼 참조)
 #define USE_PROHIBIT_AUTO_SERVER_CHECK		        // Spar마트 요청사항: RTT 기능 판매데이터 발생 시에만 서버와 통신 연결 및 정보 전송 (자동 연결체크 기능 해제 요청)
 #define USE_KEEPING_CONNETION						// 전송데이터가 계속 있을 때 Remote와 Connection 유지
//----------   국가 전용 라벨 설정  ----------//

//----------   국가 전용 디스플레이 설정  ----------//
 #define FONT_CODEPAGE CODEPAGE_1251
 #define USE_PRICE_TOTAL_7DIGIT		                // Total 7자리 사용
 #define USE_PRICE_UNIT_7DIGIT	                    // Unit Price 7자리 사용
 #define USE_PRICECHECK_8DIGIT	                    // Price Checkdigit(8digit)
//----------   국가 전용 바코드 설정  ----------//
/*		    GS1 바코드 설정 (두 바코드 중복 적용 안됨)           */
//  #define USE_RSS_BARCODE
//  #define USE_ONLY_RSS_EXPAND_BARCODE
/* 바코드 데이터 길이 (길이 증가를 원하지 않으면 모두 주석처리) */
//  #define USE_BARCODE_LEN_50
 #define USE_BARCODE_LEN_60
//  #define USE_BARCODE_LEN_70
//  #define USE_BARCODE_LEN_90
/////////////////////////////////////////////////////////////
//----------   국가 전용 특정업장 설정  ----------//

#endif  /* RUS_SP_D_H */
