 /* 
 * File:   UN_function.h
 * Author: JiMin JEONG
 * @brief  globals.h에서 분리된 영공 공통 기능 전용 파일 (영공 기반)
 * Created 2024/06/05
 */

#ifndef UN_function_H
#define	UN_function_H

//----------   영공 공통 기능  ----------//
 #define USE_TRACEABILITY						          // Use European Traceability
 #define USE_NUTRITION_FACT						        // Use Nutrition Fact
 #define USE_SELLBYDATETIME_ZERO					    // Sell by Date, Sell by Time이 0일 때 인쇄 안하는 기능
 #define USE_TRANSFER_MENU_SETTING				    // MENU Setting Up/down 기능 추가, *아직 영공만 가능.
 #define USE_CHECK_OUTOFPAPER_ON_LABEL			  // Label모드에서 Paper End 체크하는 기능. 즉시 멈춤
 #define USE_INWEIGHTKEY_WITHOUT_INTERVAL		  // IN_Weight Key 사용시 Weight Interval에 상관없이 1g단위로 사용 가능
 #define USE_TEXT_EMPHASIS						        // ~1 : Italic, ~2 : Bold, ~3 : Underline 강조하는 기능 이후 부터 스페이스 나 엔터가 나오기 전에 강조표현 유지한다
 #define USE_DISCOUNTED_TAX_FOR_DISCOUNTED_TOTAL
 //Remote monitoring & Remote Transfer parameter
 //	#define USE_REMOTE_KEY
 //	#define USE_REMOTE_DISP
 //	#define USE_ADMIN_EMAIL

//----------   영공 전용 기능(유럽 적용 X)  ----------//
  #ifdef USE_EUROPE_WITHOUT_TICKET
    #define DEALER_VER_STRING	"(u)"
		#define USE_ENG_LABELFORMAT					      // 영공 라벨포맷 사용(No.1~28 사용)		// CODE 0x407F(16511bytes)
		#define USE_KOR_LABELFORMAT					      // 국내 라벨포맷 사용(No.32~45 사용)	// CODE 0x2314(8980bytes)
		#define USE_ONLY_LABEL
  #elif defined(USE_STANDARD_DIRECT_INGREDIENT)
    #define DEALER_VER_STRING	"(D)"
    #define USE_TAX_RATE						          // Use Tax Rate
    #define USE_REPORT_TAX						        // TAX Report 사용
    #define USE_REPORT_PAY						        // PAY/CHANGE Report 사용
    #define USE_DIRECT_INGREDIENT				      // Use direct ingredient
    #define USE_INGREDIENT_98_PROTOCOL			  // PLU 통신 할 경우 Direct ingredient는 98을 사용(protocol 매뉴얼 참조)
    //	#define USE_INGREDIENT_99_PROTOCOL		// PLU 통신 할 경우 Direct ingredient는 99을 사용(protocol 매뉴얼 참조)
    #define USE_ENG_LABELFORMAT					      // 영공 라벨포맷 사용(No.1~28 사용)		// CODE 0x407F(16511bytes)
    #define USE_KOR_LABELFORMAT					      // 국내 라벨포맷 사용(No.32~45 사용)	// CODE 0x2314(8980bytes)
    //----------   미국, 캐나다, 호주의 경우 전용 라벨 포맷 사용  ----------//
      #if defined  (COUNTRY_USA) || defined(COUNTRY_CANADA) || defined(COUNTRY_AUSTRALIA)
        #undef USE_ENG_LABELFORMAT
        #undef USE_KOR_LABELFORMAT
      #endif
  #else
    #define DEALER_VER_STRING	""
    #define USE_TAX_RATE						          // Use Tax Rate
    #define USE_REPORT_TAX						        // TAX Report 사용
    #define USE_REPORT_PAY						        // PAY/CHANGE Report 사용
    #ifdef USE_EXTEND_8M_DFLASH
      #define USE_LABEL_FORMAT_EXT 			      // 라벨 개수 확장(4~8MB 영역, 20개 추가 확장)
    #endif
    #define USE_ENG_LABELFORMAT				        // 영공 라벨포맷 사용(No.1~28 사용)		// CODE 0x407F(16511bytes)
    #define USE_KOR_LABELFORMAT				        // 국내 라벨포맷 사용(No.32~45 사용)	// CODE 0x2314(8980bytes)
  #endif /* USE_EUROPE_WITHOUT_TICKET */
  #ifndef  COUNTRY_STR
    #define COUNTRY_STR "  "
  #endif
#endif  /* UN_function_H */