 /* 
 * File:   IRN_p.h
 * Author: JiMin JEONG
 * @brief  globals.h에서 분리된 Persian(이란) 전용 파일
 * Create 2024/06/04
 */

#ifndef IRN_p_H
#define	IRN_p_H

//----------   국가 Default  ----------//
 #define DEALER_VER_STRING  "(p)"
 #define COUNTRY_STR "  "
 #define USE_PERSIA_DEFAULT
//----------   국가 전용 기능  ----------//
 #define USE_PERSIA_BRACKET                 // 이란어 괄호 입력기 처리 추가
 #define USE_TRACEABILITY		                // Use European Traceability
 #define USE_NUTRITION_FACT	                // Use Nutrition Fact
 #define USE_TAX_RATE			                  // Use Tax Rate
 #define USE_SELLBYDATETIME_ZERO            // Sell by Date, Sell by Time이 0일 때 인쇄 안하는 기능
 #define USE_REPORT_TAX			                // TAX Report 사용
 #define USE_REPORT_PAY			                // PAY/CHANGE Report 사용
 #define USE_PERSIAN_CALENDAR	              //이란 달력 사용
 #define USE_ARAB_CONVERT		                //아랍어 변환 사용
 //#define USE_EXTEND_PRICE_PRIMARYSIGN	    // Use 5character price sign (Iran sign: Toman)
 #define USE_SEND_TICKET_DATA
 #define USE_ZERO_RANGE_MSG		              // 영점 범위 초과로 무게 안잡힐 경우에 경고 메시지 사용
 #define USE_ARAB_TICKET_PRT	              //현재 티켓은 일부만 해당 디파인으로 분리 되어있고, 나머지는 USE_ARAB_CONVERT 안에 선언되어있음. 추후 USE_ARAB_CONVERT 에 적용된 티켓 전용양식 분리 필요 23.07.06
 #define USE_PERSIA_CAPTION
 #define USE_PERSIAN_SALE_AMOUNT_LIMIT		  // 총액 제한 
 #define USE_SUBTOTAL_PRICE_9DIGIT_LIMIT    // 합산 금액 9자리 이하만 허용 
 #define USE_DISCOUNTED_TAX_FOR_DISCOUNTED_TOTAL
 #define USE_EFT_POS_PROTOCOL			          // EFT-POS PROTOCOL 사용(Remote IP2로 프로토콜 전송)
  #ifdef USE_EFT_POS_PROTOCOL
    #undef USE_TRACE_STANDALONE			        // 축산이력기능(국내)에서도 Remote IP2를 사용하므로 undef 처리
    #define USE_MAX_SCALE_NO_255			      // EFT-POS PROTOCOL에서 SCALE ID 0~255까지 사용
  #endif//USE_EFT_POS_PROTOCOL
 //#define USE_8_16_FONT_MENU_DISPLAY	      //프로그램 메뉴 8*16 Font Display 적용
//----------   국가 전용 라벨 설정  ----------//
 #define USE_IRN_LABELFORMAT	              // 이란 전용 라벨포맷 사용(기본 인쇄 폰트 6번타입으로 사용)
 #define USE_PERSIA_TICKET_PRT_FORMAT       //이란 티켓 양식
//----------   국가 전용 디스플레이 설정  ----------//
 #define USE_PRICECHECK_8DIGIT	            // Price Checkdigit(8digit)
 #define USE_PRICE_UNIT_7DIGIT	            // Unit Price 7자리 사용
 #define USE_PRICE_TOTAL_8DIGIT		          // Total 8자리 사용
 #define USE_ARAB_FONT			                // 아랍폰트 사용
 #define FONT_CODEPAGE CODEPAGE_1256_IRN
  #ifdef USE_ARAB_FONT
    #define USE_PRINT_LTR_FOR_ARAB	// 아랍어 없는 데이터 LTR 방향으로 인쇄
  #endif
//----------   국가 전용 바코드 설정  ----------//

//----------   국가 전용 특정업장 설정  ----------//

#endif  /* IRN_p_H */