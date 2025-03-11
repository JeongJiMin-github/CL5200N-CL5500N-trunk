/**
 * @file KOR_LOTTEMART_SUPER_INTEGRATION.h
 * @author SangDon Park
 * @brief 
 * @version 0.1
 * @date 2025-02-24
 * 
 * @copyright Copyright (c) 2025 CAS
 * 
 */

#ifndef __KOR_LOTTEMART_SUPER_INTEGRATION_H__
#define __KOR_LOTTEMART_SUPER_INTEGRATION_H__

#include "../Nation/KOR_LOTTEMART_J.h"

//----------   국가 Default  ----------//
#ifdef DEALER_VER_STRING
  #undef DEALER_VER_STRING
#endif
#define DEALER_VER_STRING	".MS"
#define USE_LOTTEMART_SUPER_INTEGRATION_DEFAULT

//----------   국가 전용 기능  ----------//
#undef PLU_COMMODITY_LINE_NO
#define PLU_COMMODITY_LINE_NO 8	        // 상품명8단
#define USE_LOTTESUPER_PROHIBIT_CHANGE_PRICE   // 롯데슈퍼 요청 매가통지용 기능 (세일모드에서 관련 키 기능 막음, 1110, 1120 Menu 비밀번호로 막음)

//----------   국가 전용 라벨 설정  ----------//

//----------   국가 전용 디스플레이 설정  ----------//

//----------   국가 전용 바코드 설정  ----------//
#undef USE_BARCODE_LEN_70
#define USE_BARCODE_LEN_90

/////////////////////////////////////////////////////////////
//----------   국가 전용 특정업장 설정  ----------//

#endif /* __KOR_LOTTEMART_SUPER_INTEGRATION_H__ */