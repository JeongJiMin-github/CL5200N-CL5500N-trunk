 /* 
 * File:   BLR_r_D.h
 * Author: JiMin JEONG
 * @brief  globals.h에서 분리된 direct_ingredient를 사용하는 벨라루스 전용 파일
 * Created 2024/06/05
 */

#ifndef BLR_r_D_H
#define	BLR_r_D_H

//----------   국가 Default  ----------//
 #define DEALER_VER_STRING	"(r)"
 #define USE_BELARUS_DEFAULT		            // Use Belarus Default
 #define COUNTRY_STR "  "
//----------   국가 전용 기능  ----------//
 #define USE_TRACEABILITY       	            // Use European Traceability
 #define USE_NUTRITION_FACT		                // Use Nutrition Fact
 #define USE_TAX_RATE						    // Use Tax Rate
 #define USE_DISCOUNTED_TAX_FOR_DISCOUNTED_TOTAL
 #define USE_SELLBYDATETIME_ZERO	            // Sell by Date, Sell by Time이 0일 때 인쇄 안하는 기능
 #define USE_DIRECT_INGREDIENT		            // use direct ingredient. Russia 는 항상 사용
 #define USE_PROTOCOL_CL5000JR   	            // for CL5000J Protocol
 #define USE_DMSG_2100_BYTE			            // Direct Ingredient 2100자
 #define USE_INWEIGHTKEY_WITHOUT_INTERVAL	    // IN_Weight Key 사용시 Weight Interval에 상관없이 1g단위로 사용 가능
 #define USE_DONT_SAVE_VALIDDATE_SALEDATE       // 유효일, 유효기간 저장하지 않음
 #define USE_CONTINUOUS_LABEL_WITH_INGREDIENT   // Continuous label 사용시 Ingredient 프린트가 가변적으로 변동됨
 #define USE_BELARUS_VER_DISPLAY		        // 벨라루스 부팅시 표시 버전 고정 기능.
//----------   국가 전용 라벨 설정  ----------//
 #define USE_BELARUS_LABEL			            // 벨라루스 전용 라벨 born in/bred in을 다른 용도로 사용
//----------   국가 전용 디스플레이 설정  ----------//
 #define USE_PRICE_UNIT_7DIGIT	                // Unit Price 7자리 사용
 #define USE_PRICE_TOTAL_7DIGIT	                // Total 7자리 사용
//----------   국가 전용 바코드 설정  ----------//

//----------   국가 전용 특정업장 설정  ----------//

#endif /* BLR_r_D_H */
