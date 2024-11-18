 /* 
 * File:   KOR_EMART_E.h
 * Author: JiMin JEONG
 * @brief  globals.h에서 분리된 한국 이마트 전용 파일
 * Created 2024/06/10
 */

#ifndef KOR_EMART_E_H
#define	KOR_EMART_E_H

//----------   국가 Default  ----------//
 #define DEALER_VER_STRING	"(E)"
 #define COUNTRY_STR "  "
 #define USE_EMART_DEFAULT
 #define USE_TRACE_MEATGRADE_EMART_DEFAULT
//----------   국가 전용 기능  ----------//
 #define USE_KOREA_VERSION_DISPLAY	        // KOREA 버전 표시 
 #define USE_TRACE_STANDALONE   	        // 축산 이력기능
 #define USE_GSMART_PLU         	        // Use GSMart PLU Structure(Kor)
 #define PLU_COMMODITY_LINE_NO 5	        // 상품명5단
 //#define USE_TRACEABILITY       	        // Use European Traceability
 //#define USE_TAX_RATE				        // Use Tax Rate
 #define USE_SELLBYDATETIME_ZERO	        // Sell by Date, Sell by Time이 0일 때 인쇄 안하는 기능
 #define USE_EMART_NETWORK      	        // E-Mart Network(실시간 판매 업로드, 매가변경 업로드)
 //#define USE_REPORT_TAX         	        // TAX Report 사용
 //#define USE_REPORT_PAY         	        // PAY/CHANGE Report 사용
 #define USE_DBG_EMART_NETWORK	            // Use Debug Network(without print)
 #define USE_KEEPING_CONNETION  	        // 전송데이터가 계속 있을 때 Remote와 Connection 유지
 #define USE_MULTI_TRACE_NO_TABLE	        // Use Multi Trace Number Table with saving
 #define USE_BARCODE_UPRICE_OVER	        // Unit Price Barcode가 자리수 초과할 경우 'U'를 0으로 인쇄
 #define USE_KOR_SLAUGHT_CODE_4DIGIT	    // 도축장 코드 4자리 사용(이마트 전용)
 //#define USE_TRACE_FLAG_0_CHECK		    // 개체관리플래그 0일때 개체선택 유무를 체크
 #define USE_CHECKING_HARMFUL_MEAT	        // 수입쇠고기 위해차단 기능(위해리스트 저울 저장)
//#define USE_EXT_CHECKING_HARMFULNESS	    // 위해리스트 서버 저장
 #define USE_FIXED_PLUTYPE			        // Use Fixed PLU Type(중량혼용, 수량혼용, 중량, 수량)
 #define USE_TRACE_MEATGRADE		        // 국내 등급표시(2011.7.1시행)
 #define USE_TRACE_SCANNER_FOR_GRADE	    // 바코드 스캐너를 이용해 등급을 호출하는 기능
 #define USE_EMART_MEATGRADE_PRINT		    // 이마트 등급표시 인쇄 사용
 #define USE_TRACE_CALL_MEATGRADE_KEY	    // 등급변경 키 사용. (PLU 등급 미사용)
 // #define USE_CHECK_CHANGE_REASON	        //단가 변경시 사유 코드 입력 기능.
 #define USE_CHECK_OUTOFPAPER_ON_LABEL	    // Label모드에서 Paper End 체크하는 기능. 즉시 멈춤
 #define USE_EMART_BACK_LABEL		        // 이마트 후방라벨 기능 사용. Protocol 90->100 bytes
 // #define USE_NUTRITION_FACT		        // Use Nutrition Fact, USE_CONTROL_CHANGING_PRICE 과 동시 사용 불가.
 #define USE_CONTROL_CHANGING_PRICE	        // 이마트(2012) 매가 변경 통제 기능, USE_S_PRICE_AS_BASE_PRICE와 같이 사용해야 함.
 #define USE_DIVIDING_FUNCTION		        // 쪼개기 기능 사용
 #define USE_EMART_PIG_TRACE
 #define USE_EMART_MINCED_MEAT              // 다짐육 보관온도 변경
 #define USE_EMART_PRODUCTION_KEY           // 생산등록키  사용여부 설정
 #define USE_EMART_ETC_COMMON_TEXT		    // 이마트 신선식품 온도별 표기강화 적용에 따른 기타 공통표시 사항 항목 인쇄 기능 (라벨폼에 "성분2" 필드가 있으면 간접성분 테이블 #99의 내용을 인쇄)
 #define USE_REALTIME_CHECK_INTERVAL	    // parameter 765 로 네트워크 체크 간격 설정
                                            // 실시간 판매전송 간격설정 기능의 경우 마트 전개 시 기존 저울의 메모리 클리어 이슈로 인해 다음 차세대 개발에 적용하기로 결정 (해당기능 주석처리)
  #ifdef USE_DISPLAY_OPT_CL5200
    #undef USE_DISPLAY_OPT_CL5200
  #endif
  //#ifdef CL5200N_BP                       // CL5200N 공통적용
    //#define USE_DISPLAY_COMM_SPEED_SETTING	// 계량모드에서 화면을 계속 갱신하고 있어서, PLU 전송 속도 느려지는 문제 개선 (P770 추가)
  //#endif
 #undef USE_CTS_FUNCTION			        // 마트는 CTS 사용 안함
//----------   국가 전용 라벨 설정  ----------//
 //#define USE_ENG_LABELFORMAT		        // 영공 라벨포맷 사용
 //#define USE_KOR_LABELFORMAT		        // 국내 라벨포맷 사용(No.32~45 사용)
//----------   국가 전용 디스플레이 설정  ----------//
 #define FONT_CODEPAGE CODEPAGE_0949_KOR
 #define USE_KOR_FONT
//----------   국가 전용 바코드 설정  ----------//

//----------   국가 전용 특정업장 설정  ----------//

#endif  /* KOR_EMART_E_H */
