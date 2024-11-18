 /* 
 * File:   DEU_UN.h
 * Author: JiMin JEONG
 * @brief  globals.h에서 분리된 영공 독일 전용 파일
 * Created 2024/06/05
 */

#ifndef DEU_UN_H
#define	DEU_UN_H

//----------   국가 Default  ----------//
#define USE_GERMANY_DEFAULT				            // PARAMETER SETTING
#define COUNTRY_STR "DE"
//----------   국가 전용 기능  ----------//
#define USE_PRINT_GROSS_AMOUNT_FOR_TAX	            // TAX Report에서 GROSS/NET AMOUNT 표시(for DEU)
#define USE_PLUNAME2_IN_TICKET			            // Ticket 프린트시 PLU Name2가 프린트 된다
#define USE_GERMANY_TICKET_DISCOUNT
#define NOT_USE_SOME_KEYS_IN_SELFSERVICE
#define USE_COPYKEY						            // Print "KOPIE"
#define USE_TARE_IN_TICKET				            // Ticket 프린트 시 Tare도 표시
#define USE_GERMANY_TARE_SCANNER		            // Scanner에 Tare 기능 추가
//#define USE_DISCOUNTED_TAX_FOR_DISCOUNTED_TOTAL
//#define USE_ZREPORT_COUNTER		                // Zreport Counter Only Calibration Mode can Reset
//#define USE_TICKETCLOSE_BEFORE_ZREPORT	        // Zreport 전에 모든 티켓 Close 및 프린트
//#define USE_ONLY_ZREPORT				            // For Ticket Zreport
//----------   국가 전용 라벨 설정  ----------//

//----------   국가 전용 디스플레이 설정  ----------//

//----------   국가 전용 바코드 설정  ----------//

//----------   국가 전용 특정업장 설정  ----------//	

#endif  /* DEU_UN_H */
