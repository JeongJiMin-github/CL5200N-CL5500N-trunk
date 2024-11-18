/**
********************************************************************************
* Copyright (c) 2024 CAS
* @brief   8M 플래시 공통 영역 메모리 주소 정의 
* @file    initial_8m_common.h
* @version 1.0
* @date    2024/02/29
* @author  SangDon Park
********************************************************************************
* @remark 8M 직전 공통 영역 주소 정의
*         include 전에 메모리 남은 공간 계산해볼 것
********************************************************************************
*/

#ifndef	__INITIAL_8M_COMMON_H__
#define	__INITIAL_8M_COMMON_H__

/*
********************************************************************************
*                       GLOBAL DEFINITIONS & MACROS
********************************************************************************
*/
/* Insert #define here */

#ifdef USE_EXTEND_8M_DFLASH

#ifdef USE_10000_SCANNER_CONVTABLE
/**
 * @remark 
 * 확장 메모리 영역(EXT_MEMCARD2_ADDR_X(0x400000))에 정의
 */
/** @brief Address : 4,194,304 */
#define FLASH_SCANNER_TABLE_AREA	0	 
#endif

#ifdef USE_NORTH_AMERICA_INGREDIENT
#define FLASH_NUTRIFACT_AREA 2971405
#define FLASH_NUTRIFACT_AREA_END FLASH_NUTRIFACT_AREA + 460000
#endif

#ifdef USE_LABEL_FORMAT_EXT
/** @brief Address : 7,625,709 */
#define FLASH_LABEL_FORMAT_EXT_HEADER_AREA	\
	  (EXT_MEMCARD_ADDR + EXT_MEMCARD_ADDR + FLASH_LABEL_FORMAT_EXT_OFFSET)	

/** @brief Address : 7,626,429 */
#define FLASH_LABEL_FORMAT_EXT_IMAGE_AREA	\
	(FLASH_LABEL_FORMAT_EXT_HEADER_AREA + LABEL_HEADER_SIZE * LABEL_CUSTOM_EXT_NUM) 	
#endif /* USE_LABEL_FORMAT_EXT */

#ifdef USE_DISCOUNT_QTY_EXT
/**
 * @remark FLASH_DISCOUNT_EXT_AREA_ADDR
 *         = 0x400000h + (3670205 - 48 (Discount 구조체 크기) * 2400개)
 *         = 0x400000h + 3555005
 *         = 7749309
 */
/** @brief Address : 7,749,309 */
#define FLASH_DISCOUNT_EXT_AREA_ADDR     (EXT_MEMCARD2_ADDR_X + 3555005)
#endif /* USE_DISCOUNT_QTY_EXT */

#if defined(USE_CHINA_CARRFOUR) || defined(USE_CHINA_HUARENWANGJI)
#define FLASH_PLU_EXT2_AREA	        0
#else
/** @brief Address : 7,864,509 */
#define FLASH_PLU_EXT2_AREA	        3670205 
#endif

#ifdef USE_EXTEND_CAPNTION_AREA
/** @brief Address : 8,376,509 */
#define FLASH_CAPTION_EXT_AREA      EXT_MEMCARD2_ADDR_X + 4182205
#endif /* USE_EXTEND_CAPNTION_AREA */

/** @remark base : EXT_MEMCARD2_ADDR_X(0x400000)
 *         사용하는 부분에서 "EXT_MEMCARD_ADDR_X + FLASH_USB_DATA_ROM_UPDATE" 와 같이 사용하여 기존 소스를
 *         유지하기 위해 Define은 메모리 주소를 "2MB + 2MB + 주소" 로 설정 하였음.
 */
/** @brief Address : 8,388,558 */
#define FLASH_REMOTE_DATA_ROM_UPDATE_FLAG     EXT_MEMCARD_ADDR_X + 4194254

/** @brief Address : 8,388,559 */
#define FLASH_USB_DATA_ROM_UPDATE       EXT_MEMCARD_ADDR_X + 4194255
/** @brief Address : 8,388,560 */
#define FLASH_FTL_ERR_MANAGEMENT_AREA	EXT_MEMCARD_ADDR_X + 4194256

#else
/**
 * @remark 확장메모리 영역
 *         base : EXT_MEMCARD_ADDR_X(0x200000)
 */
/** @brief Address : 4,194,255 */
#define FLASH_USB_DATA_ROM_UPDATE	2097103	 // CL5200J
/** @brief Address : 4,194,256 */
#define FLASH_FTL_ERR_MANAGEMENT_AREA	2097104	 // CL5200J

#endif	// USE_EXTEND_8M_DFLASH


/*
********************************************************************************
*                       GLOBAL DATA TYPES & STRUCTURES
********************************************************************************
*/
/* Insert global typedef & struct here */


/*
********************************************************************************
*                       GLOBAL(EXPORTED) VARIABLE EXTERNALS
********************************************************************************
*/
/* Insert global variable externals here */


/*
********************************************************************************
*                       GLOBAL(EXPORTED) FUNCTION PROTOTYPES
********************************************************************************
*/
/* Insert global function prototypes here */


#endif /* __INITIAL_8M_COMMON_H__ */