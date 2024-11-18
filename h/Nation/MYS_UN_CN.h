 /* 
 * File:   MYS_UN_CN.h
 * Author: JiMin JEONG
 * @brief  globals.h에서 분리된 영공 말레이시아(중국 폰트) 전용 파일
 * Created 2024/06/05
 */

#ifndef MYS_UN_CN_H
#define	MYS_UN_CN_H

//----------   국가 Default  ----------//
 #define COUNTRY_STR "MY"
//----------   국가 전용 기능  ----------//
 #define USE_DISCOUNT_QTY_EXT
 #undef USE_LABEL_FORMAT_EXT                // 라벨 개수 확장(4~8MB 영역, 20개 추가 확장) - 말레이시아 전용 펌웨어 작업 당시 비활성화하였음
//----------   국가 전용 라벨 설정  ----------//

//----------   국가 전용 디스플레이 설정  ----------//
 #define USE_CHN_FONT
 #define USE_GB2312_CHN_FONT				//simple chinese (gb2312)
 #define FONT_CODEPAGE CODEPAGE_0936_CHN
//----------   국가 전용 바코드 설정  ----------//

//----------   국가 전용 특정업장 설정  ----------//

#endif  /* MYS_UN_CN_H */
