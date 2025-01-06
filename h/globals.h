/*****************************************************************************
|*			
|*  Copyright		:	(c) 2002 CAS
|*  Filename		:	globals.h
|*  Version			:	0.1
|*  Programmer(s)	:	Hong Yi-Phyo (HYP)
|*  Created			:	2002/11/11
|*  Description		:	CLP Printer global variable define    
|*				
****************************************************************************/
//테스트 브랜치
#ifndef _GLOBALS_H
#define _GLOBALS_H

#include "app.h"

#define	_USE_SERIAL_   //use 232 port  only CLP Printer 

//////////////////////////////////////////////////////////////////////////
// Compile Condition START (Single choice in one conditon)
//////////////////////////////////////////////////////////////////////////
/*
********************************************************************************
* 			                   Compile Condition 1
* 			                   < Model Type >
********************************************************************************
*/
#define CL5200N_BP     //CL5200N-BP
//#define CL5500N_BPRH
//#define CL5500N_D
/********************************************************************************/


/*
********************************************************************************
* 			                   Compile Condition 2-1
* 			                   < Nation Define >
* 		< Dealer Code >
* 		Used Code : A     D E F G H I J K L M N   P Q R S T U V W X Y Z "TR" "SM" "PT" "DM" "TA" "SK" "KR" "SR" "PL" "LG" "VL" "GA"
* 		              a b c d e   g h i   k l m n o p   r s t u v    y z "sp" "kr"
* 		Remained Code :   B C                        O                    
* 		                              j         o    q             x 
********************************************************************************
*/

//------- 영공 펌웨어 사용 -------//
//#define USE_STANDARD							// 영공

//------- 기타 추가적인 국가 확인 필요한 펌웨어 -------//
//#define USE_ARAB								// (A) 아랍 (폰트)
//#define USE_EUROPE_WITHOUT_TICKET				// (u) 티켓기능을 사용하지 않는 유럽 전용 펌웨어
//#define USE_RUSSIA							// (R) 러시아

//------- 국내 펌웨어 (알파벳, 오름차순 정렬) -------//
//#define USE_DISCLOSURE_MEAT_PRICE 			// (g) 축산물 공시제 (독립형 개체이력 base)
//#define USE_EMART           					// (E) E-Mart - 이마트의 경우 V3.03.7부터 (E)버전과  (m)버전을 통합하여 (E)으로 만 관리
//#define USE_GSMART							// (G) GS 마트 설정
//#define USE_GSMART_TRACE    					// (M) GS 마트 축산 설정
//#define USE_HYUNDAI       					// (H) 현대백화점 설정
#define USE_IMPORT_MEAT						// (I) 독립형 개체이력 (수입육 위해 개체)
//#define USE_IMPORT_MEAT_8LINE					// (I8) 독립형 + 상품명 8단 적용
//#define USE_KOREA_CL5200     					// (k) CL5200 내수 버젼 (이력관리 기능 삭제)
//#define USE_LOTTEMART       					// (J) Lotte Mart(국내 수입육 + GS1 DataBar Expand)
//#define USE_LOTTESUPER						// (U) Lotte Super(국내 수입육에서 파생)
//#define USE_LOTTESUPER_8LINE 					// (W) Lotte Super 상품명 8단(델리카 사용)
//#define USE_NHMART          					// (N) 농협 설정
//#define USE_SCALE_POS       					// (T) 저울나라 이력저울 => CL5200J에서 제거됨
//#define USE_SHINSEGAE_MEAT					// (S) 신세계 백화점
//#define USE_TOPMART							// (Y) 서원유통  TOP mart

//------- 해외 펌웨어 (알파벳 정렬) -------//
//#define USE_ARAB_EMIRATES						// (a) 아랍에미레이트
//#define USE_CANADA_LABEL						// (L) 캐나다 라벨 길이 사용(200mm)
//#define USE_CHINA_BAONENG 					// (X) CL5200N 중국 선전 보능 업체
//#define USE_CHINA_BAONENG_COUNTRYGARDEN 		// (w) CL5200N 중국 보능 Country Garden
//#define USE_CHINA_CARRFOUR	    			// (f) CL5200N 중국 Carrfour (수닝)
//#define USE_CHINA_HUARENWANGJI				// (d) CN5200N 중국 HUARENWANGJI (화룬완자)
//#define USE_CHINESE_FONT_BIG5					// TR tranditional chinese (big5)
//#define USE_CHINESE_FONT_GB2312				// SM simple chinese (gb2312)
//#define USE_CHN_NEW_STANDARDMARKET			// (a) 중국 표준화시장(2013입찰용,디스플레이프로토콜) "/Branch/CarScale_RemoteDisplay/"
//#define USE_CHN_STANDARDMARKET				// (c) 중국표준화시장
//#define USE_CHN_TRACE_STANDARD				// (F) chinese trace standard (gb2312) <= chn E-Mart
//#define USE_GBR_HMART							// (P) 영국 H-MART 설정(한글폰트)
//#define USE_GEO_AGROHUB_MART					// GA 그루지아 Agrohub 마트 전용 딜러롬 파생(전용 Sell by Date 기능 적용)
//#define USE_GLOBAL_KOREAN_MART    			// KR 해외에 있는 한인 마트 적용. 기본적으로 영공 기반 펌웨어인데, PLU 이름은 한글로 표시, 메뉴 등은 영어로 표시 (영공 기반)
//#define USE_HEBREW							// (h) 히브리어 (이스라엘)
//#define USE_INDIA_DADUS						// DM 인도(DADUS MITHAI VATIKA)
//#define USE_INDIA_TATA						// TA 인도 TATA Group 전용 펌웨어
//#define USE_INDONESIA_EMPORIUM				// (e) 인도네시아 엠포리움 마켓
//#define USE_MANAGE_INVENTORY					// (i) 자재 재고관리 라벨 발행 기능.
//#define USE_MON_EMART							// (g) CL5500 몽골 이마트
//#define USE_NINGBO_RETAIL_STANDARDMARKET		// (z) 중국닝보표준화시장(소매기능) "/Branch/CarScale_Ningbo_Retail"
//#define USE_NINGBO_STANDARDMARKET				// (b) 중국닝보표준화시장 "/Branch/CarScale_Ningbo"
//#define USE_PERSIA							// (p) Persian(이란)
//#define USE_POLAND_LOGISTICS					// PL 폴란드 물류 전용(LOGISTICS PLU)
//#define USE_POLAND_STOKROTKA_MART				// SK 폴란드 STOKROTKA 체인마트 전용 펌웨어 (폴란드 전용 사양 + 마트 전용 파라미터, 메뉴 기본값 적용)
//#define USE_RSA_SPAR_MART         			// SR 남아공 SPAR 체인마트 전용 사양
//#define USE_RUSSIA_INDIRECT					// (Q) 러시아(Indirect Ingredient Version)
//#define USE_RUSSIA_LOGISTICS					// (l) 러시아 물류 전용
//#define USE_RUSSIA_SPAR         				// (sp) 러시아 SPAR 마트
//#define USE_SAUDI								// PT 아랍어(사우디아라비아)
//#define USE_SRILANKA							// (K) 스리랑카
//#define USE_TURKEY_QR_BARCODE					// (t) 터키 요청사항 대응 (입력한 URL을 QR 바코드로 출력하는 기능)
//#define USE_UKRAINE							// (Z) 우크라이나
//#define USE_UN_KOR_MART						// kr 해외에 있는 한인마트 적용. 기본적으로 유럽 영공 기반 펌웨어인데, PLU 이름은 한글로 표시, 메뉴 등은 영어로 표시 (유럽 영공 기반)
//#define USE_UN_LOGISTICS						// LG 영공 standard_Logistics PLU 전용 펌웨어 (PLU 8,000개)
//#define USE_VIETNAM							// (V) 베트남
//#define USE_VIETNAM_EMART						// (v) 이마트 베트남
//#define USE_VIETNAM_LOTTEMART					// VL 롯데마트 베트남

//------- 장기 미파생 및 미사용 펌웨어 -------//
//#define USE_BELARUS							// (r) 벨라루스 (2024년 기준 현재 사용 X )
//#define USE_CHINA_ALIBABA						// (l) CL5200N 알리바바 납품업체 대응 (2024년 기준 현재 사용 X )
//#define USE_MANAGE_INVENTORY_CART				// (y) 자재재고관리 기능 추가된 카트 저울 샘플 (2024년 기준 현재 사용 X)
//#define USE_EMART_CL5200     					// (m) CL5200 이마트 (V3.03.7부터 사용 안함) - 이마트의 경우 V3.03.7부터 (E)버전과  (m)버전을 통합하여 (E)으로 만 관리
//#define USE_NHMART_SAFE     					// (n) 농협 안심 한우
//#define USE_IMPORT_MEAT_CART					// (s) 수입육 위해 개체(CART)(Pb Bat)
/********************************************************************************/


/*
********************************************************************************
* 			                   Compile Condition 2-2
* 			                   < Country Define >
* 							< 영공 기반(USE_STANDARD) 국가 선택 >
* 							  영공은 국가 디파인 모두 주석처리
********************************************************************************
*/
#ifdef USE_STANDARD					//영공
/********************************************************************************/

/********************************************************************************/
//#define COUNTRY_AUSTRALIA			// 호주
//#define COUNTRY_AZERBAIJAN		// 아제르바이잔
//#define COUNTRY_BULGARIA			// 불가리아
//#define COUNTRY_CANADA			// 캐나다
//#define COUNTRY_CHINA				// 중국
//#define COUNTRY_COSTARICA			// 코스타리카
//#define COUNTRY_DOMINICA			// 도미니카
//#define COUNTRY_EASTERN_EUROPE	// CodePage1250 동유럽공용일 경우 사용합니다(국가코드 "EU").
//#define COUNTRY_GEORGIA			// 그루지아
//#define COUNTRY_GERMANY			// 독일
//#define COUNTRY_GREECE			// 그리스
//#define COUNTRY_HUNGARY			// 헝가리
//#define COUNTRY_INDIA				// 인도
//#define COUNTRY_INDONESIA			// 인도네시아 Lotte-Mart
//#define COUNTRY_KENYA				// 케냐
//#define COUNTRY_LATVIA			// 라트비아
//#define COUNTRY_LITHUANIA			// 리투아니아
//#define COUNTRY_MAKEDONIA			// 마케도니아
//#define COUNTRY_MALAYSIA_CN		// 말레이시아 chinese simplyfied version(영공 캡션: GB2312 FONT)
//#define COUNTRY_MEXICO			// 멕시코
//#define COUNTRY_MONGOL			// 몽골 공용
//#define COUNTRY_NETHERLAND		// 네덜란드
//#define COUNTRY_POLAND			// 폴란드
//#define COUNTRY_SLOVENIA			// 슬로베니아
//#define COUNTRY_SOUTHAFRICA		// 남아공
//#define COUNTRY_SWEDEN			// 스웨덴
//#define COUNTRY_THAILAND			// 태국
//#define COUNTRY_TURKEY			// 터키
//#define COUNTRY_USA				// 미국
//#define COUNTRY_VENEZUELA			// 베네수엘라

#endif /* USE_STANDARD */
/********************************************************************************/


/*
********************************************************************************
*                   CL5200N 제품 전용 Define (Hardewrare 종속)
********************************************************************************
*/
#ifdef CL5200N_BP
	#define MODEL_NAME "CL5200N"
    #define CPU_PIC32MZ
    #define USE_USB_DATA_ROM_UPDATE //USB memory를 사용한 DATAROM update
    #define USE_CL5200_KEY // P:72키, B:54키, H:72키 사용
    #define USE_ONLY_1EA_PRT_RENDERING_BUF //printer buffer 1EA만 사용 (메모리 확보)
    #define NOT_USE_NETSET_LOCK
    //----------   CL5200N 추가 기능  ----------//
    #define CL5200_PROJECT                          // [ CL5200N ]  CL5200N 기타 추가 기능
	#define USE_INTERNAL_ADC                        // [ CL5200N ]  EFG CPU 내부 ADC 사용
	#define USE_SRAM_SPI_COMM                       // [ CL5200N ]  Serial SRAM SPI 통신 제어 사용
	#define USE_SRAM_PRT_RENERING_BUF               // [ CL5200N ]  Printer Buffer를 Serial SRAM으로 이동 (내부 메모리 확보)
	#define USE_CL5200_DISPLAY                      // [ CL5200N ]  CL5200 Display 적용
	#define USE_KOR_LABELFORM31
	#define USE_PRINT_TEMP_SENSOR					// [ CL5200N ]  Temp Sensor Display 표시 추가
	#define USE_EXTEND_8M_DFLASH  					// [ CL5200N ]  Data Flash 8MB 확장 영역 활용을 위한 PLU 공간 확장 (!!주의!! 국가 파생 시 해당 국가 Memory Structure 변경 필요!!!)
	//	#define USE_EXTEND_NUMBER_OF_PLU_FOR_8M_DFLASH	// [ CL5200N ]	Data Flash 8MB 확장 영역 활용을 위한 PLU 12,000개 확장 ( Defalt값 : Disable ) (영공기준, Stack Memory Size 제한으로 인해 PLU수 최대 12,000개 까지 확장가능)
	#define USE_EXTEND_CAPNTION_AREA                // [ CL5200N ]  Data Flash 8MB 확장 영역 활용을 위한 Caption 200개 확장 (!!주의!! 국가 파생 시 해당 국가 Memory Structure 변경 필요!!!)
	#define CONTINEOUS_PRT_TEST						// 체스 연속 발행 테스트(QC검토용)
	#define USE_USB_ZIG_TEST						// USB TEST (MENU 1867/MENU 8570에서 USB Read/Write 동작 확인)
	#define USE_DATETIME_ZIG_TEST					// RTC TEST (MENU 1867/MENU 8570에서 RTC Read/Write, RTC Battery 동작 확인)
	#define USE_DISPLAY_COMM_SPEED_SETTING			// 계량모드에서 화면을 계속 갱신하고 있어서, PLU 전송 속도 느려지는 문제 개선 (P770 추가)
    #define USE_DSP_USB_FW_UPDATE_VERSION           // USB FW 업데이트 시 문구 및 버전 표시
    #define USE_DSP_MESSAGE_FOR_MEM_CLEAR           // Memory clear 동안 메세지 표시
	//#define USE_AUTO_FIND_VERSION					// 버전 정보 자동으로 찾아오는 기능
    //----------   CL5200N Test용 기능  ----------//
//	#define USE_CPU_LOAD_TEST                       // CPU Load Test - 주기적으로 NVRAM Read/Write, 주기적인 연속 발행 인쇄 
//	#define USE_TEST_AUTO_LABEL_PRINT				// 전원 ON 시 체스 라벨 1번 발행 테스트
//	#define USE_CL5200N_SFLASH_DEBUGGING			// Flash Memory Read/Write Test (드라이버 개발 시 사용)
//	#define USE_BUZZER_OFF_TEST                     // 저울 부저 음소거(개발 시 디버깅 모드 일때 소리나는거 방지)
//	#define USE_TEST_TMP_PRTBUF_MAX_SIZE			// SRMA을 프린터 버퍼로 사용 시 내부 램(동적할당) 최대값을 알아보기 위한 테스트
//  #define USE_TEST_PRT_RENDERING_TIME				// SRMA을 프린터 버퍼로 사용 시 기존 동작과 랜더링 시간 차이를 알아보기 위한 테스트
	//----------   적용된 메모리 사이즈 조회 기능   ----------//
	#define USE_CHECK_MEMORY_SIZE // 적용된 메모리 사이즈 조회 기능 M1866
	//----------- 4-8MByte 확장영역 사용 기능----------//
	#define USE_8M_EXPAND_BACKUP_RESTORE
	//----------- Scale Display(USE_CL5200_DISPLAY) ----------//
	#define _USE_LCD_7_20848_               // Graphic LCD : 7-seg part (208x48)
    #define _USE_LCD20848_                  // Graphic LCD : graphic part (208x48)
	#define USE_CL5200_DISPLAY_DEFAULT      // CL5200 display bug 수정 적용
	#define USE_DISPLAY_OPT_CL5200	        // CL5200 Diplay 속도 개선
#endif // CL5200N_B
/********************************************************************************/


/*
********************************************************************************
*                   CL5500N 제품 전용 Define (Hardewrare 종속)
********************************************************************************
*/
#if defined (CL5500N_BPRH) || defined (CL5500N_D)
	#define MODEL_NAME "CL5500N"
    #define CPU_PIC32MZ
    #define USE_USB_DATA_ROM_UPDATE //USB memory를 사용한 DATAROM update
    //#define USE_CL5200_KEY // P:72키, B:54키, H:72키 사용
    #define USE_ONLY_1EA_PRT_RENDERING_BUF //printer buffer 1EA만 사용 (메모리 확보)
    #define NOT_USE_NETSET_LOCK
    //----------   CL5200N 추가 기능  ----------//
    #define CL5500_PROJECT                          // [ CL5200N ]  CL5200N 기타 추가 기능
	#define USE_INTERNAL_ADC                        // [ CL5200N ]  EFG CPU 내부 ADC 사용
	#define USE_SRAM_SPI_COMM                       // [ CL5200N ]  Serial SRAM SPI 통신 제어 사용
	#define USE_SRAM_PRT_RENERING_BUF               // [ CL5200N ]  Printer Buffer를 Serial SRAM으로 이동 (내부 메모리 확보)
	#define USE_CL5500_DISPLAY                      // [ CL5500N ]  CL5500 Display
	#define USE_KOR_LABELFORM31
	#define USE_PRINT_TEMP_SENSOR					// [ CL5200N ]  Temp Sensor Display 표시 추가
	#define USE_EXTEND_8M_DFLASH  		// [ CL5200N ]  Data Flash 8MB 확장 영역 활용을 위한 PLU 공간 확장 (!!주의!! 국가 파생 시 해당 국가 Memory Structure 변경 필요!!!)
	//	#define USE_EXTEND_NUMBER_OF_PLU_FOR_8M_DFLASH	// [ CL5200N ]	Data Flash 8MB 확장 영역 활용을 위한 PLU 12,000개 확장 ( Defalt값 : Disable ) (영공기준, Stack Memory Size 제한으로 인해 PLU수 최대 12,000개 까지 확장가능)
	#define USE_EXTEND_CAPNTION_AREA                // [ CL5200N ]  Data Flash 8MB 확장 영역 활용을 위한 Caption 200개 확장 (!!주의!! 국가 파생 시 해당 국가 Memory Structure 변경 필요!!!)
	#define CONTINEOUS_PRT_TEST						// 체스 연속 발행 테스트(QC검토용)
	#define USE_USB_ZIG_TEST						// USB TEST (MENU 1867/MENU 8570에서 USB Read/Write 동작 확인)
	#define USE_DATETIME_ZIG_TEST					// RTC TEST (MENU 1867/MENU 8570에서 RTC Read/Write, RTC Battery 동작 확인)
	#define USE_DSP_USB_FW_UPDATE_VERSION           // USB FW 업데이트 시 문구 및 버전 표시
    #define USE_DSP_MESSAGE_FOR_MEM_CLEAR           // Memory clear 동안 메세지 표시
	//#define USE_AUTO_FIND_VERSION					// 버전 정보 자동으로 찾아오는 기능
    //----------   CL5200N Test용 기능  ----------//
//	#define USE_CPU_LOAD_TEST                       // CPU Load Test - 주기적으로 NVRAM Read/Write, 주기적인 연속 발행 인쇄 
//	#define USE_TEST_AUTO_LABEL_PRINT				// 전원 ON 시 체스 라벨 1번 발행 테스트
//	#define USE_CL5200N_SFLASH_DEBUGGING			// Flash Memory Read/Write Test (드라이버 개발 시 사용)
//	#define USE_BUZZER_OFF_TEST                     // 저울 부저 음소거(개발 시 디버깅 모드 일때 소리나는거 방지)
//	#define USE_TEST_TMP_PRTBUF_MAX_SIZE			// SRMA을 프린터 버퍼로 사용 시 내부 램(동적할당) 최대값을 알아보기 위한 테스트
//  #define USE_TEST_PRT_RENDERING_TIME				// SRMA을 프린터 버퍼로 사용 시 기존 동작과 랜더링 시간 차이를 알아보기 위한 테스트
	//----------   CL5500N-BPRH 추가기능  ----------//
	#define DISABLE_ADM_SND_WT_WHILE_USB_COMM	// USB 데이터 읽는 동안 ADM WT 전송 중단(USB 인식 오류 방어)
    //----------   CL5500N 프린터 메카 ----------//
	#ifdef CL5500N_BPRH
	#define _CL5500_PRINTER_MECH	//CL5500 Printer
	#elif defined (CL5500N_D)
	#define _CL5500_D_PRINTER_MECH	//CL5500-D Printer
	#endif
    //----------   원격기능            ----------//
//	    #define USE_REMOTE_FUNCTION
    #ifdef USE_REMOTE_FUNCTION
		#define USE_EXTEND_FLASH // NVRAM 8MB 확장
	    #define USE_REMOTE_FW_DOWNLOAD // 원격 펌웨어 다운로드 기능
	    #define USE_REMOTE_ROM_UPDATE // 원격 셋팅롬 업데이트 기능 'USE_CHK_LABEL_CLR_WHEN_USB_UPDATE'적용시 검토 필요    
	#endif
	//----------   적용된 메모리 사이즈 조회 기능   ----------//
	#define USE_CHECK_MEMORY_SIZE // 적용된 메모리 사이즈 조회 기능 M1866
	//----------- 4-8MByte 확장영역 사용 기능----------//
	#define USE_8M_EXPAND_BACKUP_RESTORE
	//----------- Scale Display(USE_CL5500_DISPLAY) ----------//
	#ifdef CL5500N_BPRH
        #define _USE_LCD_7_24_    //CL5500 B,P,R,S,H : 7-seg LCD(24digits), CL5000JC
        #define _USE_LCD32_       //Graphic LCD(202x32)
    #elif defined (CL5500N_D)
        #define _USE_LCD_7_25_    //CL5500-D : 7-seg LCD(25digits)
        #define _USE_LCD32_       //Graphic LCD(202x32)
	#endif
#endif // CL5500N_BPRHD
/********************************************************************************/


/*
********************************************************************************
*                   ETC Compiler Option define
********************************************************************************
*/
#ifndef CPU_PIC32MZ
	#define _PACKED_DEF	__packed
	#define __no_init   __no_init
#else
	#define _PACKED_DEF	//__attribute__ ((packed))
	#define __no_init
#endif //#ifndef CPU_PIC32MZ
#define PACK_2			#pragma pack(2)
#define PACK_4			#pragma pack(4)
#define PACK_CLOSE		#pragma pack()
//#define DISCOUNT_MAX_NUM_200	<= 현재 정상 처리 안됨. 꼭 수정해야함

#ifndef USE_INTERNAL_ADC
	#define USE_EXTERNAL_ADC    // [CL5200 Renewal] 외부 ADC 사용
#endif
/********************************************************************************/


/*
********************************************************************************
*                   < Common Definition - 전 국가 공통 >
********************************************************************************
*/
//#define USE_CERT_APPLY						// 인증용 펌웨어 디버깅 적용
//#define USE_USA_NTEP_SETTING					// 미국 NTEP 인증용 라벨 포멧 적용
#define USE_ALLOW_CAL_MODE						//8000 + 'menu'key를 통해 CAL 모드 진입 가능
												// 내수버전은 define과 상관없이 진입 불가.
//#define _USE_UNDER_SELFKEY_ 					//BODY에 SELFKEY PAD 사용시
#define USE_CHECK_WLAN_STATUS_LANTRONIX			// LANTRONIX CHECK 기능(SCAN SSID, 감도 등)
#define USE_UNDER_LIMIT_MINUS20D				// -20d 미만 무게표시제한 기능 (OIML2006)
//#define USE_ONLY_LABEL						// for CL5200 Label version
#define USE_CTS_FUNCTION 						// Calibration Tracebility System
//#define USE_MANAGE_LICENSE					// License 관리 기능
#define USE_KOR_LABELFORM31 					// 31번 라벨 포맷 사용
#define USE_FUNCTION_KEY_BLOCK 					// 키패드 오른편 기능키 onoff 사용여부 선택하는 기능
//#define USE_CHN_CERTIFICATION					//중국 생산허가를 위한 기능. 생산펌웨어 제작시 주석 처리
#define DISPLAY_REVISION_INFO   				// Revision 정보 출력여부 선택하는 디파인
#define USE_CL5200_DISP_CONTINUOUS_REFRESH		// Display Refresh Parameter(737)에 따라 화면 갱신 주기 변경(1: 2s-100ms / 0: X-1,000ms)
#define USE_ADM_AUTO_RESTORE					// ADM error(에러 03, 04) 일 때 자동으로 init 수행하도록 수정(공통적용)
//#define USE_CHK_LABEL_CLR_WHEN_USB_UPDATE		// USB로 라벨 업데이트시 라벨 유지할지 지울지 선택
#define USE_PARTIAL_MEM_INIT 					// 메모리 부분 초기화
#define USE_INIT_MASK 							// 메모리 초기화 방어
#define USE_MORNITORING_CNT 					// Menu 1879 모니터링 데이터 기능

#define USE_QR_BARCODE							// QR 바코드
#define USE_BARCODE_CODE128X					// CODE128X 사용 
#define USE_STANDARD_BARCODE					// GS1 DataBar 미포함
/********************************************************************************/


/*
********************************************************************************
*                   <FONT(Windows codepage)>
********************************************************************************
*/
#define CODEPAGE_1250	1
#define CODEPAGE_1251	2
#define CODEPAGE_1252	3
#define CODEPAGE_1253	4
#define CODEPAGE_1254	5
#define CODEPAGE_1255	6
#define CODEPAGE_1256	7
#define CODEPAGE_1257	8
#define CODEPAGE_1258	9
#define CODEPAGE_1252_ARM   10 // Armenia
#define CODEPAGE_1251_GEO   11 // Georgia
#define CODEPAGE_1250_POL   12 // POLAND
#define CODEPAGE_0936_CHN   13 // Simplified chinese
#define CODEPAGE_0949_KOR   14
#define CODEPAGE_1252_IND   15
#define CODEPAGE_1251_MNG   16 // Mongolia
#define CODEPAGE_1250_HUN   17 // Hungary
#define CODEPAGE_1251_KAZ   18 // Kazakhstan
#define CODEPAGE_1257_BAL   19 // Baltic
#define CODEPAGE_1258_VNM	20 // Vietnam
#define CODEPAGE_0874_ENG	21 // Thailand
#define CODEPAGE_1256_IRN   22 // Persian
#define CODEPAGE_0950_CHN	23 // Traditional Chinese
/********************************************************************************/


/*
********************************************************************************
*                   영공기반 국가별 기능 difine
********************************************************************************
*/
#ifdef USE_STANDARD
//----------   STANDARD_DIRECT_INGREDIENT  ----------//
	#ifdef COUNTRY_USA
		#include "./Nation/USA_UN_D.h"
	#elif defined COUNTRY_CANADA
		#include "./Nation/CAN_UN_D.h"
	#elif defined COUNTRY_AUSTRALIA
		#include "./Nation/AUS_UN_D.h"
	#elif defined COUNTRY_SLOVENIA
		#include "./Nation/SVN_UN_D.h"
	#elif defined COUNTRY_DOMINICA
		#include "./Nation/DOM_UN_D.h"
/********************************************************************************/
	#elif defined COUNTRY_GERMANY
		#include "./Nation/DEU_UN.h"
	#elif defined COUNTRY_INDIA
		#include "./Nation/IND_UN.h"
	#elif defined COUNTRY_CHINA
		#include "./Nation/CHN_UN.h"
	#elif defined COUNTRY_INDONESIA			/* Indonesia Lotte Mart */
		#include "./Nation/IDN_UN.h"
	#elif defined COUNTRY_GEORGIA
		#include "./Nation/GEO_UN.h"
	#elif defined COUNTRY_MALAYSIA_CN
		#include "./Nation/MYS_UN_CN.h"
	#elif defined COUNTRY_TURKEY
		#include "./Nation/TUR_UN.h"
	#elif defined COUNTRY_AZERBAIJAN
		#include "./Nation/AZE_UN.h"
	#elif defined COUNTRY_MEXICO
		#include "./Nation/MEX_UN.h"
	#elif defined COUNTRY_POLAND
		#include "./Nation/POL_UN.h"
	#elif defined COUNTRY_MAKEDONIA
		#include "./Nation/MKD_UN.h"
	#elif defined COUNTRY_GREECE			/* for CL5200 for greece 판매시 label만 리포트는 ticket으로 출력가능 */
		#include "./Nation/GRC_UN.h"
	#elif defined COUNTRY_KENYA
		#include "./Nation/KEN_UN.h"
	#elif defined COUNTRY_VENEZUELA
		#include "./Nation/VEN_UN.h"
	#elif defined COUNTRY_HUNGARY
		#include "./Nation/HUN_UN.h"
	#elif defined COUNTRY_LATVIA
		#include "./Nation/LVA_UN.h"
	#elif defined COUNTRY_LITHUANIA
		#include "./Nation/LTU_UN.h"
	#elif defined COUNTRY_SOUTHAFRICA
		#include "./Nation/ZAF_UN.h"
	#elif defined COUNTRY_NETHERLAND
		#include "./Nation/NLD_UN.h"
	#elif defined COUNTRY_EASTERN_EUROPE
		// ROMANIA
		// CROATIA
		#include "./Nation/EUR_UN_EASTERN.h"
	#elif defined COUNTRY_THAILAND
		#include "./Nation/THA_UN.h"
	#elif defined COUNTRY_BULGARIA
		#include "./Nation/BGR_UN.h"
	#elif defined COUNTRY_MONGOL
		#include "./Nation/MNG_UN.h"
	#elif defined COUNTRY_COSTARICA
		#include "./Nation/CRI_UN.h"
	#elif defined COUNTRY_SWEDEN
		#include "./Nation/SWE_UN.h"
	#else
		// else로 처리되는 국가 기록(Codepage만 다른 국가)
		// COUNTRY_GERMANY_BIZERBA
		// COUNTRY_UNITED_KINGDOM
		#define FONT_CODEPAGE CODEPAGE_1252
	#endif
//////////////////////////////////////////////////////////////////////////////////
/*				< 영공 공통사항 및 Direct Ingredient 설정 헤더파일 >			*/
	#include "./Nation/UN_function.h"
//////////////////////////////////////////////////////////////////////////////////
#endif  // #ifdef USE_STANDARD
/********************************************************************************/


/*
********************************************************************************
*                   전용 국가별 기능 difine
********************************************************************************
*/
#ifdef USE_UN_LOGISTICS
	#include "./Nation/UN_LG.h"
#endif

#ifdef USE_POLAND_LOGISTICS
	#include "./Nation/POL_PL.h"
#endif

#ifdef USE_POLAND_STOKROTKA_MART
 	#include "./Nation/POL_SK.h"
#endif

#ifdef USE_CHINA_ALIBABA
	#include "./Nation/CHN_l.h"
#endif

#ifdef USE_CHINA_HUARENWANGJI
	#include "./Nation/CHN_d.h"
#endif

#ifdef USE_CHINA_CARRFOUR
	#include "./Nation/CHN_f.h"
#endif

#ifdef USE_CHINA_BAONENG
	#include "./Nation/CHN_X_D.h"
#endif

#ifdef USE_CHINA_BAONENG_COUNTRYGARDEN
	#include "./Nation/CHN_w_D.h"
#endif

#ifdef USE_NHMART
	#include "./Nation/KOR_NHMART_N.h"
#endif

#ifdef USE_NHMART_SAFE
	#include "./Nation/KOR_NHMART_SAFE_n.h"
#endif

#ifdef USE_GSMART_TRACE
	#include "./Nation/KOR_GSMART_TRACE_M.h"
#endif

#ifdef USE_GSMART
	#include "./Nation/KOR_GSMART_G.h"
#endif

#ifdef USE_HYUNDAI
	#include "./Nation/KOR_HYUNDAI_H.h"
#endif

#ifdef USE_SCALE_POS
	#include "./Nation/KOR_SCALE_POS_T.h"
#endif

#ifdef USE_EMART
	#include "./Nation/KOR_EMART_E.h"
#endif

#ifdef USE_DISCLOSURE_MEAT_PRICE
	#include "./Nation/KOR_DISCLOSURE_MEAT_PRICE_g.h"
#endif

#ifdef USE_RUSSIA_INDIRECT
	#include "./Nation/RUS_Q.h"
#endif

#ifdef USE_RUSSIA_LOGISTICS
	#include "./Nation/RUS_l_D.h"
#endif

#ifdef USE_RUSSIA_SPAR
	#include "./Nation/RUS_SP_D.h"
#endif

#ifdef USE_UKRAINE
	#include "./Nation/UKR_Z_D.h"
#endif

#ifdef USE_UN_KOR_MART
	#include "./Nation/UN_KOR_MART_kr.h"
#endif

#ifdef USE_BELARUS
	#include "./Nation/BLR_r_D.h"
#endif

#ifdef USE_VIETNAM
	#include "./Nation/VNM_V.h"
#endif

#ifdef USE_VIETNAM_EMART
	#include "./Nation/VNM_EMART_v.h"
#endif

#ifdef USE_VIETNAM_LOTTEMART
	#include "./Nation/VNM_LOTTEMART_VL.h"
#endif

#ifdef USE_IMPORT_MEAT
	#include "./Nation/KOR_IMPORT_MEAT_I.h"
#endif

#ifdef USE_SHINSEGAE_MEAT
	#include "./Nation/KOR_SHINSEGAE_MEAT_S.h"
#endif

#ifdef USE_IMPORT_MEAT_CART
	#include "./Nation/KOR_IMPORT_MEAT_CART_s.h"
#endif

#ifdef USE_LOTTEMART
	#include "./Nation/KOR_LOTTEMART_J.h"
#endif

#ifdef USE_LOTTESUPER
	#include "./Nation/KOR_LOTTESUPER_U.h"
#endif

#ifdef USE_LOTTESUPER_8LINE
	#include "./Nation/KOR_LOTTESUPER_8LINE_W.h"
#endif

#ifdef USE_CANADA_LABEL
	#include "./Nation/CAN_LABEL_L.h"
#endif

#ifdef USE_SRILANKA
	#include "./Nation/LKA_K.h"
#endif

#ifdef USE_CHINESE_FONT_BIG5
	#include "./Nation/CHN_FONT_BIG5_TR_D.h"
#endif

#ifdef USE_CHINESE_FONT_GB2312
	#include "./Nation/CHN_FONT_GB2312_D.h"
#endif

#ifdef USE_CHN_TRACE_STANDARD	//USE_CHN_EMART
	#include "./Nation/CHN_TRACE_STANDARD_F.h"
#endif

#ifdef USE_GBR_HMART
	#include "./Nation/GBR_HMART_P.h"
#endif

#ifdef USE_GEO_AGROHUB_MART
	#include "./Nation/GEO_AGROHUB_MART_GA.h"
#endif

#ifdef USE_GLOBAL_KOREAN_MART
	#include "./Nation/GLOBAL_KOR_MART_KR.h"
#endif

#ifdef USE_ARAB_EMIRATES
  	#include "./Nation/ARE_a.h"
#endif

#ifdef USE_PERSIA
  	#include "./Nation/IRN_p.h"
#endif

#ifdef USE_HEBREW
  	#include "./Nation/ISR_HEBREW_h.h"
#endif

#ifdef USE_SAUDI   // 아랍공용롬 + 전용 Direct Ingredient
	#include "./Nation/SAU_PT_D.h"
#endif

#ifdef USE_CHN_STANDARDMARKET
	#include "./Nation/CHN_STANDARDMARKET_c.h"
#endif

#ifdef USE_MANAGE_INVENTORY
	#include "./Nation/MANAGE_INVENTORY_i.h"
#endif

#ifdef USE_MANAGE_INVENTORY_CART
	#include "./Nation/MANAGE_INVENTORY_CART_y.h"
#endif

#ifdef USE_KOREA_CL5200
	#include "./Nation/KOR_CL5200_k.h"
#endif

#ifdef USE_EMART_CL5200
	#include "./Nation/KOR_EMART_CL5200_m.h"
#endif

#ifdef USE_INDONESIA_EMPORIUM
	#include "./Nation/IDN_EMPORIUM_e_D.h"
#endif

#ifdef USE_MON_EMART
	#include "./Nation/MNG_EMART_g.h"
#endif

#ifdef USE_TURKEY_QR_BARCODE
	#include "./Nation/TUR_QR_BARCODE_t_D.h"
#endif

#ifdef USE_TOPMART
	#include "./Nation/KOR_TOPMART_Y.h"
#endif

#ifdef USE_INDIA_DADUS
	#include "./Nation/IND_DADUS_DM.h"
#endif

#ifdef USE_INDIA_TATA
	#include "./Nation/IND_TATA_TA.h"
#endif

#ifdef USE_RSA_SPAR_MART
	#include "./Nation/ZAF_SPAR_MART_SR_D.h"
#endif

#ifdef USE_IMPORT_MEAT_8LINE
	#include "./Nation/KOR_IMPORT_MEAT_8LINE_I8.h"
#endif
//////////////////////////////////////////////////////////////////////////////////
/*								< 러시아 펌웨어 >								*/
#ifdef USE_RUSSIA
	#include "./Nation/RUS_function.h"

/*						< 러시아 버전 특정 국가(Country) 선택 >						*/
//#define COUNTRY_RUSSIA 			// Default
//#define COUNTRY_ARMENIA
//#define COUNTRY_KAZAKHSTAN
//#define COUNTRY_LITHUANIA
//#define COUNTY_UZBEKISTAN

  #ifdef COUNTRY_RUSSIA
	#include "./Nation/RUS_R_D.h"
  #elif defined COUNTRY_ARMENIA
	#include "./Nation/ARM_R_D.h"
  #elif defined COUNTRY_KAZAKHSTAN
	#include "./Nation/KAZ_R_D.h"
  #elif defined COUNTRY_LITHUANIA
	#include "./Nation/LTU_R_D.h"
  #elif defined COUNTY_UZBEKISTAN
	#include "./Nation/UZB_R_D.h"
  #else
 	#define FONT_CODEPAGE CODEPAGE_1251
  #endif
#endif
//////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////
/*							< 아랍 폰트 사용 펌웨어 >							*/
#ifdef USE_ARAB
	#include "./Nation/ARAB_FONT_function_A.h"

/*						< 아랍 폰트 특정 국가(Country) 선택 >						*/
//#define COUNTRY_LEBANON

  #ifdef COUNTRY_LEBANON
  	#include "./Nation/LBN_A.h"
  #endif //COUNTRY_LEBANON
#endif //USE_ARAB
//////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////
/*							< 유럽(티켓 기능 X) 사용 펌웨어 >							*/
#ifdef USE_EUROPE_WITHOUT_TICKET
	#include "./Nation/UN_function.h"

/*					< 유럽(티켓 기능 X) 특정 국가(Country) 선택 >						*/
//#define COUNTRY_GERMANY

  #ifdef COUNTRY_GERMANY
	#include "./Nation/DEU_EUR_u.h"
  #endif // COUNTRY_GERMANY
#endif
//////////////////////////////////////////////////////////////////////////////////


/********************************************************************************/


/*
********************************************************************************
* 			                   디스플레이 설정
*  < 사용하고자 하는 DIGIT을 헤더파일에서 입력받아 표시하는 정의문을 자동으로 설정 >
********************************************************************************
*/
#ifdef CL5200N_BP
  #if defined(USE_PRICE_UNIT_7DIGIT) || defined(USE_PRICE_TOTAL_7DIGIT)	// 단가, 총액 7자리 사용시
	#define USE_DISPLAY_5200_7DIGIT	//5200 lcd_7_20848에서 7자리 표현, 7자리 사용할 경우 parameter 997 Display style 3(Tare 미표시 )으로 변경 불가c
  #endif

  #if defined(USE_PRICE_UNIT_9DIGIT) || defined(USE_PRICE_TOTAL_9DIGIT)	// 단가, 총액 9자리 사용시
    #define USE_DISPLAY_STYLE_3DIGIT		// Tare를 제외한 중량/단가/총액 표시
  #endif
#endif
/********************************************************************************/


/*
********************************************************************************
* 			                   제품 인증 설정
*               < 인증시 해당 정의문을 활성화 시켜 필요한 기능 사용 >
********************************************************************************
*/
#ifdef USE_CERT_APPLY
	#define USE_CHECK_CAL_SW_TO_UPDATE_FW		//인증시 CAL스위치를 통해 firmware 업데이트 할 수 있도록 함.(#define USE_OIML_SETTING 명명 수정)
	#define USE_CONTINUOUS_PRINT_DELAY_2000ms		// EMC테스트 시 인쇄 시간 간격 조정(2000ms)
	#define USE_CERT_DISP_CONTINUOUS_REFRESH		// 인증 Sample 용 화면 연속 갱신 펌웨어 
#endif
/********************************************************************************/


/*
********************************************************************************
* 			                     폰트 설정
*   < 헤더파일에 FONT_CODEPAGE가 정의되어 있지 않다면 해당 정의문이 디폴트로 설정 >
********************************************************************************
*/
#ifndef FONT_CODEPAGE
#define FONT_CODEPAGE CODEPAGE_1252
#endif
/********************************************************************************/


/////////////////////////////Compile Condition END/////////////////////////////////

//#define USE_KEY_BUZZER	// Buzzer가 Key가 눌릴 때마다 울리도록 함

#define ROMDATA	const
#define HUGEDATA
#define _huge
#define _rom const
#define _inline
#define NEARDATA

//#define NDEBUG //debug message to output window


#define ON		1
#define OFF		0
#define IDLE	2

#define	DISABLE		0
#define	ENABLE		1
#define YES		1
#define NO		0
#if !defined(TRUE)
  #define	TRUE	(!0)
  #define	FALSE	0
#endif

// <Flash memory Type> //
#define EN25QH32A		0 // 구모델 (Eon)
#define SST26VF0XXB     1 // 대체 모델 ( Microchip - SST26VF032B[4M] or SST26VF064B[8M] )
//#define SST26VF032B		1 // 대체 모델 (Microchp)


#define NUL		0x00
#define SOH		0x01
#define	STX		0x02
#define ETX	   	0x03
#define EOT		0x04
#define ENQ		0x05
#define ACK		0x06
#define BEL		0x07
#define BS 		0x08
#define HT 	   	0x09
#define LF 		0x0A
#define VT 		0x0B
#define FF 		0x0C
#define _CR 	0x0D
#define SO		0x0E
#define SI		0x0F
#define DLE		0x10
#define DC1		0x11
#define DC2		0x12
#define DC3		0x13
#define DC4		0x14
#define NAK		0x15
#define SYN		0x16
#define ETB		0x17

#define EM		0x19
#define SUB		0x1A
#define _ESC	0x1B   		//091207 yoo test
#define FC 		0x1C
#define GS 		0x1D
#define __RS 	0x1E		//QR encode lib conflict
#define US 		0x1F
#define DEL 	0x7F

typedef unsigned char BOOLEAN; /* Logical data type (TRUE or FALSE) */
typedef unsigned char INT8U; /* Unsigned 8 bit value */
typedef signed char INT8S; /* Signed 8 bit value */
typedef unsigned short INT16U; /* Unsigned 16 bit value */
typedef signed short INT16S; /* Signed 16 bit value */
typedef unsigned long INT32U; /* Unsigned 32 bit value */
typedef signed long INT32S; /* Signed 32 bit value */
typedef unsigned long long INT64U; /* Unsigned 64 bit value (if available)*/
//typedef signed xxxxx INT64S; /* Signed 64 bit value (if available)*/
typedef float FP32; /* 32 bit, single prec. floating-point */
typedef double FP64; /* 64 bit, double prec. floating-point */

#endif /* _GLOBALS_H */
#ifndef _FW_VERSION_DEFINE
#define _FW_VERSION_DEFINE

#define FW_BOOT_VERSION_ROM_ADDR	0x21fff0
#define FW_VERSION_ROM_ADDR	 		0x07fff0

#pragma pack(2)
typedef struct {
	char model[2];
	char year[2];
	char month[2];	// 6
	char country[3];//
	char keytype;	// 10
	char subversion[2];	//vendor[2];		// Modified by CJK 20061121
	char version;
	char reversion[2];
	char testversion;	// CJK080321
} FW_VERSION;

typedef struct {
	char dealer[4];	// include NULL
	char country[3];	// include NULL
	char calpassword[6];	// Cal Password version & init default
	char reserved[3];
} FW_VERSION2;
#pragma pack()

#ifdef USE_MORNITORING_CNT
#pragma pack(2)
typedef struct {
    INT32U TotalCnt;
    INT32U AfterCnt;
    INT32U InspectDate;
    INT32U Reserved;
} REMOTE_STRUCT;
#pragma pack()
#endif

extern void version_get(FW_VERSION *fw);
extern void version2_get(FW_VERSION2 *fw);
#endif

#ifndef _ROM_COUNTRY_DEFINE
#define _ROM_COUNTRY_DEFINE

#define ROM_STD	0
#define ROM_RUS	1

#if defined(USE_RUSSIA) || defined(USE_RUSSIA_INDIRECT) || defined(USE_BELARUS) || defined(USE_UKRAINE) || defined(USE_RUSSIA_LOGISTICS) || defined(USE_RUSSIA_SPAR)
  #define ROM_COUNTRY ROM_RUS
#else
  #define ROM_COUNTRY ROM_STD
#endif
#endif

#if defined(USE_MANAGE_LICENSE)		// License 관리 기능
#define USE_DES_ENCRYPTION
#endif

//#define TEST_REPORT