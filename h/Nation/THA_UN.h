 /* 
 * File:   THA_UN.h
 * Author: JiMin JEONG
 * @brief  globals.h에서 분리된 태국 전용 파일
 * Created 2024/06/05
 */

#ifndef THA_UN_H
#define	THA_UN_H

//----------   국가 Default  ----------//
 #define COUNTRY_STR "TH"
//----------   국가 전용 기능  ----------//
 #define USE_8_16_FONT_MENU_DISPLAY		    //프로그램 메뉴 8*16 Font Display 적용
 #define USE_PREVENT_CALLBACK_DISP_OVER     //태국어 캡션이 길어서 콜백함수 죽음 방어 (문제 없으면 삭제 가능)
//----------   국가 전용 라벨 설정  ----------//

//----------   국가 전용 디스플레이 설정  ----------//
 #define USE_THAI_FONT					    //태국어 폰트 적용
 #define FONT_CODEPAGE CODEPAGE_0874_ENG
 #define USE_PRICE_TOTAL_7DIGIT		          // Total 7자리 사용
 #define USE_PRICE_UNIT_7DIGIT		          // Unit Price 7자리 사용
//----------   국가 전용 바코드 설정  ----------//

//----------   국가 전용 특정업장 설정  ----------//		
			
#endif  /* THA_UN_H */
