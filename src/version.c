/*****************************************************************************
|*
|*  Copyright		:	(c) 2002 CAS
|*  Filename		:	version.c
|*  Version			:	0.2
|*  Programmer(s)	:	Hong Yi-Phyo (HYP)
|*  Created			:	2003/05/21
|*  Description		:	CLP version string
|*
****************************************************************************/

#include "globals.h"
#include "revision_info.h"

////////////////////////////////////////////////////////
//Version String Description
//
// <Model>
//  CL : CL5000/CL5500/CL5500N
//  C3 : CL3500 (not use)
//  C4 : CL5200/CL5200N
//  C5 : CL5200J
//  C7 : CL7200
//	CA : CL3000
//
// <Type>
//  Num  Symbol  Scale Type        Model Name
//   1     B     Bench(Standard)   CL5500-B
//   2     P     Pole              CL5500-P/R, CL3500-P
//   3     H     Hanging           CL5500-H
//   4     S     Self-service      CL5500-S
//   5     D     Double Body       CL5500-D
//   6     B     Bench(CL5200)     CL5200-B
//   7     B     Bench(CL3000)     CL3000-B
//
// <Country> (현재 미사용)
//  KT1 : KOR 저울나라 버전
//  KS1 : KOR 이력기능버전
//  EU0 : 일반버전
////////////////////////////////////////////////////////

#define VER1	"3"
#define VER2	"08"
#define VER3	"03"

#ifdef USE_AUTO_FIND_VERSION
#define FINDING_START 	"_veRsIon"
#define FINDING_END		"_eNd"
#endif
#if defined(USE_KOR_TRACE)
  #ifdef CL5200_PROJECT
	ROMDATA  char VERSION_STRING[] = {"C42108KT1P"VER3 VER1 VER2};  //CL5200-P
  #else
	ROMDATA  char VERSION_STRING[] = {"CL2108KT1D"VER3 VER1 VER2};  
  #endif
#elif defined(USE_TRACE_STANDALONE)
  #ifdef CL5200_PROJECT
	ROMDATA  char VERSION_STRING[] = {"C42108KS1P"VER3 VER1 VER2};  //CL5200-P
  #else
	ROMDATA  char VERSION_STRING[] = {"CL2108KS1D"VER3 VER1 VER2};  
  #endif
#else
  #ifdef CL5200N_BP
	ROMDATA  char VERSION_STRING[] = {"C42108EU0P"VER3 VER1 VER2}; //CL5200N-P
  #elif defined(CL5500N_BPRH) || defined(CL5500N_D)
	ROMDATA  char VERSION_STRING[] = {"CL2108EU0P"VER3 VER1 VER2}; //CL5500N, CL5500N-D 
  #endif
#endif
				//16 chars include NULL
				// 0~1  : Model
				// 2~3  : Year
				// 4~5  : Month
				// 6~8  : Country
				// 9    : Type(Symbol)
				// 10~11: Sub-Version//Vendor	// Modified by CJK 20061121
				// 12~14: Version

ROMDATA  char VERSION_STRING2[4] = {DEALER_VER_STRING};	// "(R)"
ROMDATA  char VERSION_COUNTRY[3] = {COUNTRY_STR};	// "AU"

#ifdef USE_AUTO_FIND_VERSION
 #ifdef CL5500N_D
	ROMDATA  char GET_VERSION_NAME_GEN[] = FINDING_START "_" "CL5500N-D" "_" COUNTRY_STR "_" "V"VER1 VER2 VER3 "_" DEALER_VER_STRING "_" LOAD_REVISION FINDING_END;
 #else
	ROMDATA  char GET_VERSION_NAME_GEN[] = FINDING_START "_" MODEL_NAME "_" COUNTRY_STR "_" "V"VER1 VER2 VER3 "_" DEALER_VER_STRING "_" LOAD_REVISION FINDING_END;
 #endif 
#endif

void version_get(FW_VERSION *fw)
{
	INT16S i;
	for (i=0; i<sizeof(FW_VERSION); i++) {
		((INT8U*)fw)[i] = ((_rom unsigned char	*)VERSION_STRING)[i];
	}
}

void version2_get(FW_VERSION2 *fw)
{
	INT16S i, j;
	for (i=0; i<sizeof(VERSION_STRING2); i++) {
		((INT8U*)fw)[i] = ((_rom unsigned char	*)VERSION_STRING2)[i];
	}
	for (j=0; j<sizeof(VERSION_COUNTRY); j++, i++) {
		((INT8U*)fw)[i] = ((_rom unsigned char	*)VERSION_COUNTRY)[j];
	}
}

