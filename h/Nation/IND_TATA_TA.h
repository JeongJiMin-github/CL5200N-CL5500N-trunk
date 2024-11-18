 /* 
 * File:   IND_TATA_TA.h
 * Author: JiMin JEONG
 * @brief  globals.h에서 분리된 TA 인도 TATA Group 전용 펌웨어 전용 파일
 * Create 2024/06/04
 */

#ifndef IND_TATA_TA_H
#define	IND_TATA_TA_H

//----------   국가 Default  ----------//
 #define DEALER_VER_STRING	".TA"
 #define COUNTRY_STR "  "
 #define USE_INDIA_TATA_DEFAULT
//----------   국가 전용 기능  ----------//
 #define USE_TRACEABILITY						// Use European Traceability
 #define USE_NUTRITION_FACT						// Use Nutrition Fact
 #define USE_SELLBYDATETIME_ZERO					// Sell by Date, Sell by Time이 0일 때 인쇄 안하는 기능
 #define USE_TRANSFER_MENU_SETTING				// MENU Setting Up/down 기능 추가, *아직 영공만 가능.
 #define USE_CHECK_OUTOFPAPER_ON_LABEL			// Label모드에서 Paper End 체크하는 기능. 즉시 멈춤
 #define USE_INWEIGHTKEY_WITHOUT_INTERVAL		// IN_Weight Key 사용시 Weight Interval에 상관없이 1g단위로 사용 가능
 #define USE_TEXT_EMPHASIS						// ~1 : Italic, ~2 : Bold, ~3 : Underline 강조하는 기능 이후 부터 스페이스 나 엔터가 나오기 전에 강조표현 유지한다
 #define USE_DISCOUNTED_TAX_FOR_DISCOUNTED_TOTAL
 //Remote monitoring & Remote Transfer parameter
 //	#define USE_REMOTE_KEY
 //	#define USE_REMOTE_DISP
 //	#define USE_ADMIN_EMAIL
 #define USE_TAX_RATE						// Use Tax Rate
 #define USE_REPORT_TAX						// TAX Report 사용
 #define USE_REPORT_PAY						// PAY/CHANGE Report 사용
  #ifdef USE_EXTEND_8M_DFLASH
    #define USE_LABEL_FORMAT_EXT // 라벨 개수 확장(4~8MB 영역, 20개 추가 확장)
  #endif
 #define USE_INDIA_FUNCTION				// India Function(Item Code 13, Area Scale Mode)
 //#define USE_SRAM_PLU_INDIA				// NVRAM Plu
 #define USE_QUICK_DELETE_PLU			// PLU Delete all 속도 개선
 #define USE_INDIA_PRINT_UNIT			// 인도 전용 화폐 및 단가 표시
 #define USE_INGREDIENT_CODE_4DIGIT		// 성분은 510byte 999개, 성분 index번호는 4자리 (1~9999번) 사용 가능
 //#define USE_9999_INDIRECT_INGREDIENT	// 성분은 510byte 999개, 성분 index번호는 9999번까지 사용 가능함
 #define USE_INDIA_TAX					// 인도 세법 적용 (CGST, SGST, GST)
 #define USE_INDIA_TAX_REPORT_MENU		// 인도 TAX 메뉴 추가
 #define USE_CHECK_DUPLICATION_TICKET	// duplication ticket 기능 사용할 경우, print 후에 복사 여부를 사용자가 매번 선택.
 #define USE_COPYKEY						// copy ticket 발행시 Ticket에 Copy 표시
 #define USE_PRINT_FILL_WEIGHT           // Fixed Weight 계량 후 라벨 발행 시 Gross Weight Field를 이용하여 "Fill Weight"로 인쇄
 #define USE_10000_SCANNER_CONVTABLE		// M1461 Scanner Convert Table 10000개 사용 (1~10000, PLU 개수 만큼 사용)
 #define USE_QR_BARCODE_ERASE_GAP		// QR Add Total 인쇄 시 QR 바코드가 인쇄되는 위치를 최대한 올려서 여백 간격을 없애서 인쇄되게 함 - 영수증 티켓 용지 아끼는 목적 (인도 요청)
 #define USE_PRT_SCALE_NUM_IN_REPORT		// Print Scale Number in report
 #define USE_PRT_SCALE_NUM_IN_TICKET		// 티켓 인쇄 시 Scale No 출력 (인도 요청)
//----------   국가 전용 라벨 설정  ----------//
  #if !defined(COUNTRY_AUSTRALIA)					//영공롬 사용, 호주 전용 라벨 미사용(호주는 전용 라벨 사용)
    #define USE_ENG_LABELFORMAT				// 영공 라벨포맷 사용(No.1~28 사용)		// CODE 0x407F(16511bytes)
    #define USE_KOR_LABELFORMAT				// 국내 라벨포맷 사용(No.32~45 사용)	// CODE 0x2314(8980bytes)
  #endif
//----------   국가 전용 디스플레이 설정  ----------//
 #define FONT_CODEPAGE CODEPAGE_1252_IND
//----------   국가 전용 바코드 설정  ----------//
/*		 GS1 바코드 설정 (두 바코드 중복 적용 안됨)           */
//  #define USE_RSS_BARCODE
//  #define USE_ONLY_RSS_EXPAND_BARCODE
/* 바코드 데이터 길이 (길이 증가를 원하지 않으면 모두 주석처리) */
 #define USE_BARCODE_LEN_50
//  #define USE_BARCODE_LEN_60
//  #define USE_BARCODE_LEN_70
//  #define USE_BARCODE_LEN_90
/////////////////////////////////////////////////////////////
  #ifdef USE_QR_BARCODE
	#define USE_QR_ADDTOTAL				//QR바코드 이용한 ADD TOTAL 기능
  #endif
//----------   국가 전용 특정업장 설정  ----------//

#endif  /* IND_TATA_TA_H */