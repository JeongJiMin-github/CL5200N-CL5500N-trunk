 /* 
 * File:   RUS_function.h
 * Author: JiMin JEONG
 * @brief  globals.h에서 분리된 direct_ingredient를 사용하는 러시아 스탠다드 전용 파일
 * Created 2024/06/10
 */

#ifndef RUS_function_H
#define	RUS_function_H

//----------   국가 Default  ----------//
 #define DEALER_VER_STRING	"(R)"
 #define USE_RUSSIA_DEFAULT			        // default set for russia
//----------   국가 전용 기능  ----------//
 #define USE_NUTRITION_FACT			        // Use Nutrition Fact
 #define USE_SELLBYDATETIME_ZERO	        // Sell by Date, Sell by Time이 0일 때 인쇄 안하는 기능
 #define USE_SELLBYDATE_CHANGE		        // Sell by Date 자리수에 동작 변경(Ignore PARAM#532, 6자리 : YYMMDD, 4자리 : 생산일에 더함)
 #define USE_PROTOCOL_CL5000JR		        // for CL5000J Protocol
 #define USE_INWEIGHTKEY_WITHOUT_INTERVAL	// IN_Weight Key 사용시 Weight Interval에 상관없이 1g단위로 사용 가능
 #define USE_RUSSIA_BARCODE_FUNCTION	    // 추가 심벌 추가 (decimal Point, sell by date. packing date, produce date)
 #define USE_PERCENT_WEIGHT			        //영공 percent Tare 기능을 러시아에서 percent Weight 로 사용
 #define USE_DMSG_2100_BYTE			        //Direct Ingredient 2100자
 #define USE_JULIAN_DATE
 // #define USE_TRACEABILITY			    // Use European Traceability
 //	#define USE_CONTINUOUS_LABEL_WITH_INGREDIENT
 #define USE_REMOTE_KEY
 #define USE_REMOTE_DISP
 #define USE_ADMIN_EMAIL
 #define USE_TRANSFER_MENU_SETTING
 #define USE_SALE_MIN_WEIGHT_AT_AUTO_MODE   //카자흐스탄 요청사항: 라벨 Auto 모드에서 Min.Weight 이하 값 판매 가능하게 설정하는 기능
 #define USE_DIRECT_INGREDIENT		// use direct ingredient. Russia 는 항상 사용
 #define USE_INGREDIENT_98_PROTOCOL
//----------   국가 전용 라벨 설정  ----------//

//----------   국가 전용 디스플레이 설정  ----------//

//----------   국가 전용 바코드 설정  ----------//
/*		 GS1 바코드 설정 (두 바코드 중복 적용 안됨)           */
//  #define USE_RSS_BARCODE
//  #define USE_ONLY_RSS_EXPAND_BARCODE
/* 바코드 데이터 길이 (길이 증가를 원하지 않으면 모두 주석처리) */
//  #define USE_BARCODE_LEN_50
 #define USE_BARCODE_LEN_60
//  #define USE_BARCODE_LEN_70
//  #define USE_BARCODE_LEN_90
/////////////////////////////////////////////////////////////
//----------   국가 전용 특정업장 설정  ----------//

#endif  /* RUS_function_H */
