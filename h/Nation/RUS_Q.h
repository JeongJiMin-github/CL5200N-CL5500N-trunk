 /* 
 * File:   RUS_Q.h
 * Author: JiMin JEONG
 * @brief  globals.h에서 분리된 indirect_ingredient를 사용하는 러시아 전용 파일
 * Created 2024/06/10
 */

#ifndef RUS_Q_H
#define	RUS_Q_H

//----------   국가 Default  ----------//
 #define DEALER_VER_STRING	"(Q)"
 #define COUNTRY_STR "  "
//----------   국가 전용 기능  ----------//
 #define USE_TRACEABILITY       	        // Use European Traceability
 #define USE_NUTRITION_FACT		            // Use Nutrition Fact
 #define USE_TAX_RATE				        // Use Tax Rate
 #define USE_SELLBYDATETIME_ZERO	        // Sell by Date, Sell by Time이 0일 때 인쇄 안하는 기능
 #define USE_ONLY_INDIRECT_INGREDIENT	    // Indirect Ingredient만 사용
 #define USE_RUSSIA_VER_DISPLAY		        // 러시아 부팅시 표시 버전 고정 기능.
//----------   국가 전용 라벨 설정  ----------//

//----------   국가 전용 디스플레이 설정  ----------//

//----------   국가 전용 바코드 설정  ----------//

//----------   국가 전용 특정업장 설정  ----------//

#endif  /* RUS_Q_H */