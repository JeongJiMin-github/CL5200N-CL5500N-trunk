 /* 
 * File:   CAN_LABEL_L.h
 * Author: JiMin JEONG
 * @brief  globals.h에서 분리된캐나다 라벨 길이 사용(200mm) 전용 파일
 * Created 2024/06/10
 */

#ifndef CAN_LABEL_L_H
#define	CAN_LABEL_L_H

//----------   국가 Default  ----------//
 #define DEALER_VER_STRING	"(L)"
 #define COUNTRY_STR "  "
//----------   국가 전용 기능  ----------//
 //#define USE_TRACE_STANDALONE   	// 축산 이력기능
 //#define USE_GSMART_PLU         	// Use GSMart PLU Structure(Kor)
 //#define PLU_COMMODITY_LINE_NO 5	// 상품명5단
 //#define USE_TRACE_STANDALONE   	// 축산 이력기능
 //#define USE_GSMART_PLU         	// Use GSMart PLU Structure(Kor)
 //#define PLU_COMMODITY_LINE_NO 5	// 상품명5단
 #define USE_TRACEABILITY       	// Use European Traceability
 #define USE_NUTRITION_FACT		    // Use Nutrition Fact
 #define USE_TAX_RATE				// Use Tax Rate
 #define USE_DISCOUNTED_TAX_FOR_DISCOUNTED_TOTAL
 #define USE_SELLBYDATETIME_ZERO	// Sell by Date, Sell by Time이 0일 때 인쇄 안하는 기능
 //#define USE_EMART_NETWORK      	// E-Mart Network(실시간 판매 업로드, 매가변경 업로드)
 #define USE_REPORT_TAX         	// TAX Report 사용
 #define USE_REPORT_PAY         	// PAY/CHANGE Report 사용
//----------   국가 전용 라벨 설정  ----------//
 #define USE_ENG_LABELFORMAT		// 영공 라벨포맷 사용
 //#define USE_KOR_LABELFORMAT		// 국내 라벨포맷 사용(No.32~45 사용)
 #define USE_CAN_LABEL_LENGTH		// 캐나다 라벨 길이 사용 200mm (기존 120mm)
//----------   국가 전용 디스플레이 설정  ----------//

//----------   국가 전용 바코드 설정  ----------//

//----------   국가 전용 특정업장 설정  ----------//

#endif  /* CAN_LABEL_L_H */