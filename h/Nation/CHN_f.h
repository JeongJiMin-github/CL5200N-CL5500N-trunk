 /* 
 * File:   CHN_f.h
 * Author: JiMin JEONG
 * @brief  globals.h에서 분리된 중국 까르푸 전용 파일
 * Created 2024/06/05
 */

#ifndef CHN_f_H
#define	CHN_f_H

 //----------   국가 Default  ----------//
 #define DEALER_VER_STRING	"(f)"
 #define USE_CHINA_CARRFOUR_DEFAULT
 #define USE_CHINA_DEFAULT						// PARAMETER SETTING
 #define COUNTRY_STR "  "
//----------   국가 전용 기능  ----------//
 #define USE_TRACEABILITY						// Use European Traceability
 #define USE_NUTRITION_FACT						// Use Nutrition Fact
 #define USE_SELLBYDATETIME_ZERO				// Sell by Date, Sell by Time이 0일 때 인쇄 안하는 기능
 #define USE_TRANSFER_MENU_SETTING				// MENU Setting Up/down 기능 추가, *아직 영공만 가능.
 #define USE_INWEIGHTKEY_WITHOUT_INTERVAL		// IN_Weight Key 사용시 Weight Interval에 상관없이 1g단위로 사용 가능
 #define USE_TEXT_EMPHASIS						// ~1 : Italic, ~2 : Bold, ~3 : Underline 강조하는 기능 이후 부터 스페이스 나 엔터가 나오기 전에 강조표현 유지한다
 #define USE_DISCOUNTED_TAX_FOR_DISCOUNTED_TOTAL
 #define USE_TAX_RATE							// Use Tax Rate
 #define USE_REPORT_TAX							// TAX Report 사용
 #define USE_REPORT_PAY							// PAY/CHANGE Report 사용
 #define USE_WEIGHT_UNIT_500G					// 중국 단가 500g 단위 추가 (1근 = 500g)
 #define USE_PROTOCOL_CHINA_DAHUA				// 중국 전용 다화 프로토콜
 #define USE_BARCODE_LEN_50 
 #define USE_CHINA_PINYIN						// 중문 입력기
 #define USE_PLU_TIMELEN_CUT   					// Sell by Time, Packed Time Length 1byte
 #define USE_BARCODE_DISCOUNT_RATE				// Barcode에 Discount Rate 항목 추가
 #define USE_PRT_CHNIA_DISCOUNT_RATE			// Discount 값 라벨 인쇄 기능 추가
 #define USE_FIXED_PERCENT_KEY_SET				// fixed Percent discount key 할인율 설정 기능 추가(1~99%)
 #define USE_PRT_CONVERT_WEIGHT_KG_G			// 무게 인쇄 변환 기능 (kg <-> g)
 #define USE_CARREFOUR_ADD_PLU_URL_FEILD		// PLU Field에 Traceablity관련 2개의  필드 T-URL(128Bytes), T-Code(20Bytes) 추가
 #define USE_EXTEND_NUMBER_OF_PLU_FOR_8M_DFLASH
 #define USE_PROHIBITE_PRICE_CHANGE_IN_PLU_MENU	// 상해영업 요청사항: 저울(MENU 1110, 1120)에서 상품 단가 변경을 못하게 막아달라고 요청함
 #define USE_HYFLYING_REBOOT_FUNCTION			// Hyflying 모듈 1916 Setting 부분 추가 (Carrfour 전원OFF시 연결 Disconnect 현상 방어)
 #define USE_CARREFOUR_EXTEND_INGREDIENT_NUMBER	// Carrefour 요청사항 성분 개수 확장(1180개 -> 2270개)
//----------   국가 전용 라벨 설정  ----------//
 #define USE_CHECK_OUTOFPAPER_ON_LABEL			// Label모드에서 Paper End 체크하는 기능. 즉시 멈춤
 #define USE_ENG_LABELFORMAT					// 영공 라벨포맷 사용(No.1~28 사용)		// CODE 0x407F(16511bytes)
 #define USE_KOR_LABELFORMAT					// 국내 라벨포맷 사용(No.32~45 사용)	// CODE 0x2314(8980bytes)
//----------   국가 전용 디스플레이 설정  ----------//
 #define USE_CHN_FONT
 #define USE_GB2312_CHN_FONT					// simple chinese (gb2312)
 #define FONT_CODEPAGE CODEPAGE_0936_CHN
//----------   국가 전용 바코드 설정  ----------//
 #define USE_BARCODE_UPRICE_OVER    			// when Unit Price Barcode print, valid digits exceeded  number of digits Print ' U ' to zero 
 #define USE_CARREFOUR_TRACE_QR_BARCODE			// Traceability code 조회 url Data를 QR Barcode로 기존 라벨 Field Type '3' 영역에 url 출력하는 기능
//----------   국가 전용 특정업장 설정  ----------//

 #endif /* CHN_f_H */
 