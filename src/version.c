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

#if defined(USE_KOR_TRACE)
  #ifdef CL5200_PROJECT
	ROMDATA  char VERSION_STRING[] = {"C42108KT1P08307"};  //CL5200-P
  #else
	ROMDATA  char VERSION_STRING[] = {"CL2108KT1D08307"};  
  #endif
#elif defined(USE_TRACE_STANDALONE)
  #ifdef CL5200_PROJECT
	ROMDATA  char VERSION_STRING[] = {"C42108KS1P08307"};  //CL5200-P
  #else
	ROMDATA  char VERSION_STRING[] = {"CL2108KS1D08307"};  
  #endif
#else
  #ifdef CL5200N_BP
	ROMDATA  char VERSION_STRING[] = {"C42108EU0P08307"}; //CL5200N-P
  #elif defined(CL5500N_BPRH) || defined(CL5500N_D)
	ROMDATA  char VERSION_STRING[] = {"CL2108EU0P08307"}; //CL5500N, CL5500N-D 
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

