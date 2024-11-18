 /* 
 * File:   IND_UN.h
 * Author: JiMin JEONG
 * @brief  globals.h에서 분리된 영공 인도 전용 파일
 * Created 2024/06/05
 */

#ifndef IND_UN_H
#define	IND_UN_H

//----------   국가 Default  ----------//
 #define COUNTRY_STR "IN"
 #define USE_INDIA_DEFAULT
//----------   국가 전용 기능  ----------//
 #define USE_INDIA_FUNCTION				        // India Function(Item Code 13, Area Scale Mode)
 #define USE_QUICK_DELETE_PLU			        // PLU Delete all 속도 개선
 #define USE_INDIA_PRINT_UNIT			        // 인도 전용 화폐 및 단가 표시
 #define USE_INGREDIENT_CODE_4DIGIT		    // 성분은 510byte 999개, 성분 index번호는 4자리 (1~9999번) 사용 가능
 #define USE_INDIA_TAX					          // 인도 세법 적용 (CGST, SGST, GST)
 #define USE_INDIA_TAX_REPORT_MENU		    // 인도 TAX 메뉴 추가
 #define USE_CHECK_DUPLICATION_TICKET	    // duplication ticket 기능 사용할 경우, print 후에 복사 여부를 사용자가 매번 선택.
 #define USE_COPYKEY						          // copy ticket 발행시 Ticket에 Copy 표시
 #define USE_PRINT_FILL_WEIGHT            // Fixed Weight 계량 후 라벨 발행 시 Gross Weight Field를 이용하여 "Fill Weight"로 인쇄
 #define USE_10000_SCANNER_CONVTABLE	    // M1461 Scanner Convert Table 10000개 사용 (1~10000, PLU 개수 만큼 사용)
 #define USE_QR_BARCODE_ERASE_GAP		      // QR Add Total 인쇄 시 QR 바코드가 인쇄되는 위치를 최대한 올려서 여백 간격을 없애서 인쇄되게 함 - 영수증 티켓 용지 아끼는 목적 (인도 요청)
 #define USE_PRT_SCALE_NUM_IN_REPORT	    // Print Scale Number in report
 #define USE_PRT_SCALE_NUM_IN_TICKET	    // 티켓 인쇄 시 Scale No 출력 (인도 요청)
//  #define USE_SRAM_PLU_INDIA				    // NVRAM Plu
//#define USE_9999_INDIRECT_INGREDIENT	  // 성분은 510byte 999개, 성분 index번호는 9999번까지 사용 가능함
//----------   국가 전용 라벨 설정  ----------//

//----------   국가 전용 디스플레이 설정  ----------//
 #define FONT_CODEPAGE CODEPAGE_1252_IND
//----------   국가 전용 바코드 설정  ----------//
 #define USE_BARCODE_LEN_50
  #ifdef USE_QR_BARCODE
    #define USE_QR_ADDTOTAL				      //QR바코드 이용한 ADD TOTAL 기능
  #endif
//----------   국가 전용 특정업장 설정  ----------//

#endif  /* IND_UN_H */
