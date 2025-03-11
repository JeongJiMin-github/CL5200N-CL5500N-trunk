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

//----------   ���� Default  ----------//
#ifdef DEALER_VER_STRING
  #undef DEALER_VER_STRING
#endif
#define DEALER_VER_STRING	".MS"
#define USE_LOTTEMART_SUPER_INTEGRATION_DEFAULT

//----------   ���� ���� ���  ----------//
#undef PLU_COMMODITY_LINE_NO
#define PLU_COMMODITY_LINE_NO 8	        // ��ǰ��8��
#define USE_LOTTESUPER_PROHIBIT_CHANGE_PRICE   // �Ե����� ��û �Ű������� ��� (���ϸ�忡�� ���� Ű ��� ����, 1110, 1120 Menu ��й�ȣ�� ����)

//----------   ���� ���� �� ����  ----------//

//----------   ���� ���� ���÷��� ����  ----------//

//----------   ���� ���� ���ڵ� ����  ----------//
#undef USE_BARCODE_LEN_70
#define USE_BARCODE_LEN_90

/////////////////////////////////////////////////////////////
//----------   ���� ���� Ư������ ����  ----------//

#endif /* __KOR_LOTTEMART_SUPER_INTEGRATION_H__ */