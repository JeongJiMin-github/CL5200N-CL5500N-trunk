/*****************************************************************************
 *		       
 *  Copyright		:	(c) 2003 CAS
 *  Filename		:	department.c
 *  Version		:	v 0.9
 *  Programmer(s)	:	getwing (Jae-Ok Kim)
 *  Created		:	2003/12/26
 *  Description		:	barcode form
 *              				
****************************************************************************/


#include <stdio.h>
#include <string.h>
#include "globals.h"

#include "commbuf.h"
#include "prtdrv.h"
#include "timer.h"
#include "serial.h"
#include "flash.h"
#include "sensor.h"
#include "ram.h"
#include "prt_render.h"
#include "bar_common.h"
#include "prt_font.h"
#include "dsp_common.h"
#include "bar_common.h"
#include "prt_interpreter.h"
#include "prt_cmd.h"

#include "rtc.h"

#include "adm.h"
#include "main.h"
#include "initial.h"
#include "key.h"
#include "key_typedef.h"
#include "key_function.h"
#include "mode_main.h"
#include "caption_app.h"
#include "common.h"
#include "prt_common.h"
#include "menu.h"
#include "flash_app.h"
#include "dsp_app.h"
#include "dsp_vfd7.h"
#include "key_app.h"

#include "math.h" 
#include "mode_program.h"
#include "cursor.h"
#include "department.h"
#include "menu_code_text1.h"
#include "barcode_file.h"
#include "initial.h"
#include "pgm_app.h"
#include "prt_list.h"
#include "mode_error.h"
#include "initial_glb.h"

//BARCODE_STRUCT 	barcode_str;

#ifdef USE_RSS_BARCODE
#define BARCODE_TYPE_NUM    11+5
char	barcodetype[BARCODE_TYPE_NUM][20]={
	"barcodetype", //00
	"EAN-13",      //48	
	"EAN13A5",     //50
	"Invalid Value",       //51			
	"Invalid Value",       //70
	"UPCA",        		//54			
	"UPC-13",      		//48
	"Invalid Value",      	//68, CODE39
	"Invalid Value",      	//65, CODE93
	"Invalid Value",     	//61, CODE128
	"Invalid Value",      	//74, CODABAR
	"DataBar",    		//	("RSS14-Standard")
	"DataBar Stack",   	//	("RSS14-Stack")
	"DataBar Stk Omni",  	//	("RSS14-Stack Omni")
	"DataBar Limited",     	//	("RSS-Limit")
	"Invalid Value"		//RSSEXP
     	    		};
INT8U	barcodetype_codevalue[BARCODE_TYPE_NUM] =	{
	0,		//0	//0	"barcodetype", 
	EAN13,	 	//1	//4	"EAN-13",      
	EAN13P5,  	//2	//5	"EAN13A5",     
#if defined (CL5200_PROJECT) || defined (CL5500_PROJECT)
    EAN8,       //3 //3 "EAN-8",       
    I2OF5,      //4 //6 "I2OF5", 
#else   // #ifdef CL5200_PROJECT
    NO_BARCODE,//EAN8, 		//3	//3	"EAN-8",       
	NO_BARCODE,//I2OF5,	 	//4	//6	"I2OF5",       
#endif  // #ifdef CL5200_PROJECT
	UPCA,		//5	//2	"UPCA",        
	EAN13,		//6	//1	"UPC-13",      
#if defined (CL5200_PROJECT) || defined (CL5500_PROJECT)
    CODE39,		//7	//7	"CODE39",      
	CODE93,	   	//8	//8	"CODE93",  
	CODE128X,  	//9	//9	"CODE128",   
	CODABAR,   	//10	//10	"CODABAR", 
#else
	NO_BARCODE,//CODE39,		//7	//7	"CODE39",      
	NO_BARCODE,//CODE93,	   	//8	//8	"CODE93",      
#ifdef USE_BARCODE_CODE128X
	NO_BARCODE,//CODE128X,  	//9	//9	"CODE128",     
#else
	NO_BARCODE,//CODE128C,  	//9	//9	"CODE128",     
#endif
	NO_BARCODE,//CODABAR,   	//10	//10	"CODABAR", 
#endif  // #ifdef CL5200_PROJECT
	RSS14, 		//14	//11	"RSS14" 
	RSS14S,   	//12	//12	"RSS14S",      
	RSS14SO,   	//11	//13	"RSS14SO",      
	RSSLIM, 	//13	//14	"RSS-Limit"
	RSSEXP,		//15	//15	"RSSEXP"   
				};
char	barcodetype_length[BARCODE_TYPE_NUM] =	{
//20060511
	BARCODE_FORMAT_LEN,    	//0	"barcodetype", 
	13,    			//1	"EAN-13",      
	18,  			//2	"EAN13A5",     
	8,     			//3	"EAN-8",       
	BARCODE_FORMAT_LEN,    	//4	"I2OF5",       
	12,    			//5	"UPCA",        
	13,    			//6	"UPC-13",      
	BARCODE_FORMAT_LEN,   	//7	"CODE39",      
	BARCODE_FORMAT_LEN,   	//8	"CODE93",      
	BARCODE_FORMAT_LEN, 	//9	"CODE128",     
	BARCODE_FORMAT_LEN, 	//10	"CODABAR",
	14,		 	//11	"RSS14" 
	14,   			//12	"RSS14S",      
	14,		   	//13	"RSS14SO",      
	14,		 	//14	"RSS-Limit",
	BARCODE_FORMAT_LEN, 	//15	"RSSEXP"
				};
#elif defined(USE_ONLY_RSS_EXPAND_BARCODE)
#define BARCODE_TYPE_NUM    11+6
char	barcodetype[BARCODE_TYPE_NUM][20]={
	"barcodetype", //00
	"EAN-13",      //48	
  #ifdef USE_EAN13A5_IN_EXPAND_BARCODE
	"EAN13A5",		//50
  #else
	"Invalid Value",		//50
  #endif
	"Invalid Value",       	//51			
	"Invalid Value",       	//70
	"Invalid Value",        //54			
	"Invalid Value",      	//48
	"Invalid Value",      	//68
	"Invalid Value",      	//65
	"CODE128",     	//61	
    	"Invalid Value",     	//74
    	"Invalid Value",    	//??
	"Invalid Value",   	//??
	"Invalid Value",     	//??
    	"Invalid Value",    	//??
	"DataBar Expand",    	//??	("RSS-Expand")
  #ifdef USE_QR_BARCODE
	"QR Code",
  #else
	"Invalid Value",    	//??
  #endif
     	    		};
INT8U	barcodetype_codevalue[BARCODE_TYPE_NUM] =	{
	0,		//0	//0	"barcodetype", 
	EAN13,	 	//1	//4	"EAN-13",      
#if defined (CL5200_PROJECT) || defined (CL5500_PROJECT)
    EAN13P5,  	//2	//5	"EAN13A5",     
	EAN8, 		//3	//3	"EAN-8",       
	I2OF5,	 	//4	//6	"I2OF5",       
	UPCA,		//5	//2	"UPCA",        
	EAN13,		//6	//1	"UPC-13",      
	CODE39,		//7	//7	"CODE39",      
	CODE93,	   	//8	//8	"CODE93",      
  #else  // CL5200_PROJECT
    NO_BARCODE,//EAN13P5,  	//2	//5	"EAN13A5",     
	NO_BARCODE,//EAN8, 		//3	//3	"EAN-8",       
	NO_BARCODE,//I2OF5,	 	//4	//6	"I2OF5",       
	NO_BARCODE,//UPCA,		//5	//2	"UPCA",        
	NO_BARCODE,//EAN13,		//6	//1	"UPC-13",      
	NO_BARCODE,//CODE39,		//7	//7	"CODE39",      
	NO_BARCODE,//CODE93,	   	//8	//8	"CODE93",      
  #endif  // CL5200_PROJECT
  #ifdef USE_BARCODE_CODE128X
	CODE128X,  	//9	//9	"CODE128",     
  #else
	CODE128C,  	//9	//9	"CODE128",     
  #endif
#if defined (CL5200_PROJECT) || defined (CL5500_PROJECT)
    CODABAR,   	//10	//10	"CODABAR", 
  #else
	NO_BARCODE,//CODABAR,   	//10	//10	"CODABAR", 
  #endif
	RSS14, 		//14	//11	"RSS14" 
	RSS14S,   	//12	//12	"RSS14S",      
	RSS14SO,   	//11	//13	"RSS14SO",      
	RSSLIM, 	//13	//14	"RSS-Limit"
	RSSEXP,		//15	//15	"RSSEXP" , 15digit 이상 부터 인쇄 가능
  #ifdef USE_QR_BARCODE
	QRCODE,
  #else
	NO_BARCODE,
  #endif
	};
char	barcodetype_length[BARCODE_TYPE_NUM] =	{
//20060511
	BARCODE_FORMAT_LEN,    	//0	"barcodetype", 
	13,    			//1	"EAN-13",      
	18,  			//2	"EAN13A5",     
	8,     			//3	"EAN-8",       
	BARCODE_FORMAT_LEN,    	//4	"I2OF5",       
	12,    			//5	"UPCA",        
	13,    			//6	"UPC-13",      
	BARCODE_FORMAT_LEN,   	//7	"CODE39",      
	BARCODE_FORMAT_LEN,   	//8	"CODE93",      
	BARCODE_FORMAT_LEN, 	//9	"CODE128",     
	BARCODE_FORMAT_LEN, 	//10	"CODABAR",
	14,		 	//11	"RSS14", 
	14,   			//12	"RSS14S",      
	14,		   	//13	"RSS14SO",      
	14,		 	//14	"RSS-Limit",
	BARCODE_FORMAT_LEN, 	//15	"RSSEXP"           
	BARCODE_FORMAT_LEN,		//16 "QR

				};
#else
#ifndef USE_QR_BARCODE //20140903
#define BARCODE_TYPE_NUM    11
#else
#ifdef USE_QRCODE_11SET_DEFAULT
#define BARCODE_TYPE_NUM    12
#else
#define BARCODE_TYPE_NUM    17
#endif
#endif
char	barcodetype[BARCODE_TYPE_NUM][20]={
	"barcodetype", //00
	"EAN-13",      //48	
	"EAN13A5",     //50
	"EAN-8",       //51			
	"I2OF5",       //70
	"UPCA",        //54			
	"UPC-13",      //48
	"CODE39",      //68			
	"CODE93",      //65			
	"CODE128",     //61			
    "CODABAR",      //74
#ifdef USE_QR_BARCODE //20140903
#ifdef USE_QRCODE_11SET_DEFAULT
	"QR Code",
#else
	"Invalid Value",
	"Invalid Value",
	"Invalid Value",
	"Invalid Value",
	"Invalid Value",
    "QR Code",
#endif
#endif    	
        	    		};
        
INT8U	barcodetype_codevalue[BARCODE_TYPE_NUM] =	{
	0,		//0	//0	"barcodetype", 
	EAN13,	 	//1	//4	"EAN-13",      
#if defined (CL5200_PROJECT) || defined (CL5500_PROJECT)
    EAN13P5,  	//2	//5	"EAN13A5",     
	EAN8, 		//3	//3	"EAN-8",       
#else  // CL5200_PROJECT
#ifdef USE_JULIAN_DATE
	EAN13P5,//EAN13P5,  	//2	//5	"EAN13A5",     
#else
	NO_BARCODE,//EAN13P5,  	//2	//5	"EAN13A5",     
#endif
	NO_BARCODE,//EAN8, 		//3	//3	"EAN-8",       
#endif  // CL5200_PROJECT
	I2OF5,	 	//4	//6	"I2OF5",       
	UPCA,		//5	//2	"UPCA",        
	EAN13,		//6	//1	"UPC-13",      
#if defined (CL5200_PROJECT) || defined (CL5500_PROJECT)
    CODE39,   	//7	//7	"CODE39",      
	CODE93,   	//8	//8	"CODE93",  
#else  // CL5200_PROJECT
	NO_BARCODE,//CODE39,   	//7	//7	"CODE39",      
	NO_BARCODE,//CODE93,   	//8	//8	"CODE93",      
#endif  // CL5200_PROJECT
#ifdef USE_BARCODE_CODE128X
	CODE128X,  	//9	//9	"CODE128",     
#else
	CODE128C, 	//9	//9	"CODE128",     
#endif
#if defined (CL5200_PROJECT) || defined (CL5500_PROJECT)
    CODABAR, 	//10	//10	"CODABAR"      
#else  // CL5200_PROJECT
	NO_BARCODE,//CODABAR, 	//10	//10	"CODABAR"
#endif  // CL5200_PROJECT
#ifdef USE_QR_BARCODE //20140903
#ifdef USE_QRCODE_11SET_DEFAULT
	QRCODE,		//11	//	"QRCODE"
#else
	RSS14, 		//14	//11	"RSS14" 
	RSS14S,   	//12	//12	"RSS14S",      
	RSS14SO,   	//11	//13	"RSS14SO",      
	RSSLIM, 	//13	//14	"RSS-Limit"
	RSSEXP,		//15	//15	"RSSEXP" 
	QRCODE,		//	//	"QRCODE"
#endif
#endif	
				};
        
char	barcodetype_length[BARCODE_TYPE_NUM] =	{
//20060511
	BARCODE_FORMAT_LEN,    	//0	"barcodetype", 
	13,    			//1	"EAN-13",      
	18,  			//2	"EAN13A5",     
	8,     			//3	"EAN-8",       
	BARCODE_FORMAT_LEN,    	//4	"I2OF5",       
	12,    			//5	"UPCA",        
	13,    			//6	"UPC-13",      
	BARCODE_FORMAT_LEN,   	//7	"CODE39",      
	BARCODE_FORMAT_LEN,   	//8	"CODE93",      
	BARCODE_FORMAT_LEN, 	//9	"CODE128",     
	BARCODE_FORMAT_LEN, 	//10	"CODABAR"      
#ifdef USE_QR_BARCODE //20140903
#ifdef USE_QRCODE_11SET_DEFAULT
	BARCODE_FORMAT_LEN,		//11	"QR CODE"
#else
	14,		 	//11	"RSS14" 
	14,   			//12	"RSS14S",      
	14,		   	//13	"RSS14SO",      
	14,		 	//14	"RSS-Limit",
	BARCODE_FORMAT_LEN, 	//15	"RSSEXP"
	BARCODE_FORMAT_LEN,		//16	"QR CODE"
#endif
#endif	
				};
#endif

char	UPC13_form[][20]={
//	"1234567890123",
	"DDIIIIIVPPPPC",
	"DDIIIIVPPPPPC",
	"DDIIIIIPPPPPC",
	"DDIIIIIWWWWWC"			};

#ifdef USE_UPCA				
char	UPC12_form[][20]={
//	"123456789012",
	"DIIIIIVPPPPC",
	"DIIIIVPPPPPC",	
	"DIIIIIPPPPPC",
	"DIIIIIWWWWWC"			};
#endif

#ifdef USE_EAN8
char	EAN8_form[][20]={
//	"12345678",
	"DDIIIIIC",
	"DDPPPPPC",
	"DIIIIIIC",
	"DPPPPPPC" 			};
#endif
	
char	EAN13_form[][20]={
//	"1234567890123",
	"DDIIIIIPPPPPC",
	"DDIIIIPPPPPPC",
	"DIIIIIPPPPPPC",
	"DIIIIIIPPPPPC"			};

#ifdef USE_EAN13SUP5
char	EAN13A5_form[][20]={
//	"123456789012345678",
	"DDIIIIIPPPPPCUUUUU",
	"DDIIIIPPPPPPCUUUUU",
	"DDIIIIIPPPPPCWWWWW",
	"DDIIIIPPPPPPCWWWWW"	    	};
#endif

#ifdef USE_I2OF5
char	I2OF5_form[][20]={
//	"123456789012345678",
	"DDIIIIWWWWWPPPPPPC",
	"DDIIIIIWWWWPPPPPPC",
	"DDIIIIIIWWWWPPPPPC",	
	"DIIIIIWWWWWPPPPPPC"		};
#endif

#ifdef USE_CODE39
char   	CODE39_form[][20]={
//	"1234567890123",
	"TTTTTTTTTTTTC",
	"TTTTTTTPPPPPC",
	"TTTTTTPPPPPPC",
	"TTTTPPPPPPPPC"			};
#endif

#ifdef USE_CODE128C
 #if defined(COUNTRY_INDONESIA) || defined(USE_INDONESIA_EMPORIUM)
char	CODE128_form[][25]={
//	"12345678901234567890",
	"2IIIIIVPPPPPPPWWWWWC",
	"2IIIIIVPPPPPPPQQ000C",
	"%#IIIIIVPPPPPPPWWWWWC",	
	"%#IIIIIVPPPPPPPQQ000C"		};
 #else
char	CODE128_form[][20]={
//	"123456789012345678",
	"DDIIIIPPPPPPUUUUUU",
	"DDIIIIIWWWWPPPPPPP",
	"DDIIIIIIWWWWPPPPPP",	
	"DIIIIIWWWWWPPPPPPP"		}; 
 #endif
#endif

#ifdef USE_CODE93    
char   	CODE93_form[][20]={
//	"1234567890123",
	"TTTTTTTTTTTTC",
	"TTTTTTTPPPPPC",
	"TTTTTTPPPPPPC",
	"TTTTPPPPPPPPC"			};
#endif

#ifdef USE_CODABAR
char   	CODABAR_form[][20]={
//	"1234567890123",
	"DDIIIIIIPPPPP",
	"DDIIIIIPPPPPP",
	"DIIIIIIPPPPPP",
	"DIIIIIPPPPPPP"			};
#endif

#if defined(USE_RSS_BARCODE) || defined(USE_ONLY_RSS_EXPAND_BARCODE)
char   	RSS14_default_form[][20]={
//	"12345678901234",
	"DDIIIIIPPPPPPC",
	"DDIIIIPPPPPPPC",
	"DIIIIIPPPPPPPC",
	"DIIIIPPPPPPPPC" 		};
#endif//#if defined(USE_RSS_BARCODE) || defined(USE_ONLY_RSS_EXPAND_BARCODE)

char	QRCODE_form[][20]={
	"DDIIIIIPPPPPC",
	"DDIIIIPPPPPPC",
	"DIIIIIPPPPPPC",
	"DIIIIIIPPPPPC"			};

void barcode_selecttype(long num);
void barcode_selectformat(long num);
void barcodefile_default_KR_trace(void);
void scanner_default_KR_trace(void);

void barcodefile_default_KR_trace(void)
{
	INT8U i;
	BARCODE_STRUCT bar_default_Form[] = {
		{1,	"EAN-13",	BARCODE_TYPE_EAN13,		0,	"DDIIIIPPPPPPC"}, 				//13digit
		{2,	"보관라벨",	BARCODE_TYPE_CODE128,		0,	"NNNNNNTTTTTTTTTTTTTTuuss"}, //24digit 
		{3, "보관+등급",	BARCODE_TYPE_CODE128,		0,	"NNNNNNTTTTTTTTTTTTTTuussgg"}, //26igit 등급추가
		{4, "이력바코드",	BARCODE_TYPE_CODE128,		0,	"TTTTTTTTTTTTTTT"}, //15digit 이력바코드
#ifdef USE_SHINSEGAE_DEFAULT
		{5, "신세계바코드",	BARCODE_TYPE_CODE128,		0,	"DDIIIIIPPPPPPWWWWWEC"}, //20digit
#elif defined (USE_BESTCO_DEFAULT)
		{5, "베스트코바코드",	BARCODE_TYPE_CODE128,		0,	"NNNNNNWWWWWPPPPPPXXC"}, //20digit
#endif
	};

	for (i = 0; i < sizeof(bar_default_Form)/sizeof(BARCODE_STRUCT); i++)
	{
		set_base_sparam(FLASH_BARCODE_FORMAT_AREA + (BARCODE_STRUCT_SIZE * (INT32U)i),(INT8U *)&bar_default_Form[i],sizeof(BARCODE_STRUCT));
	} 
}
#define KR_TRACE_SCANNER_LENGTH 27
void scanner_default_KR_trace(void)
{
	INT32U	addr;
	INT16U	i;
	INT8U	scannerDefaultBarForm[][KR_TRACE_SCANNER_LENGTH] = {
			"TTTTTTTTTTTT", //이력번호 12자리 
			"TTTTTTTTTTTTTTT", //LOT 번호 15자리 
			"NNNNNNTTTTTTTTTTTTTTuuss", //24 보관라벨
			"NNNNNNTTTTTTTTTTTTuuss",//22 구형 보관라벨
			"NNNNNNTTTTTTTTTTTTTTuussgg", //26보관라벨
	};
												
	addr = DFLASH_BASE + FLASH_SCANNER_FORMAT;

	for (i = 0; i < sizeof(scannerDefaultBarForm)/KR_TRACE_SCANNER_LENGTH; i++)
	{
		Flash_swrite(addr+(i*40), scannerDefaultBarForm[i], KR_TRACE_SCANNER_LENGTH);
	}
}

void    barcodefile_default_us(void)
{
	BARCODE_STRUCT 	barcode_str;

	memset(&barcode_str,0,sizeof(BARCODE_STRUCT));
	strcpy((char *)barcode_str.barcode_description,"UPC-13");
	barcode_str.barcode_num       =1;
	barcode_str.barcode_format_num=0;
	barcode_str.barcode_type=BARCODE_TYPE_UPC13;
	strcpy((char *)barcode_str.barcode_format,"02IIIIIVPPPPC");
	set_base_sparam(FLASH_BARCODE_FORMAT_AREA,(INT8U *)&barcode_str,sizeof(BARCODE_STRUCT));
}

#if defined(USE_EMART_DEFAULT)

void    barcodefile_default_for_Emart(void)
{
	INT8U i;
//	BARCODE_STRUCT EmartForm[] = {
//		{1,	"EAN-13",	BARCODE_TYPE_EAN13,	0,	"DDIIIIIPPPPPC"		},
//		{2,	"EAN-8",	BARCODE_TYPE_EAN8,	0,	"DDIIIIIC"		},
//		{3,	"EAN-13",	BARCODE_TYPE_EAN13,	0,	"DDIIIIIPPPPPC"		},
//		{4,	"EAN-8",	BARCODE_TYPE_EAN8,	0,	"DDIIIIIC"		},
//		{5,	"I2OF5",	BARCODE_TYPE_I2OF5,	0,	"DDWWWWWWPPPPPPPC"	},
//		{6,	"I2OF5",	BARCODE_TYPE_I2OF5,	0,	"DDIIIIWWWWWPPPPPPC"	},
//		{7,	"CODE128",	BARCODE_TYPE_CODE128,	0,	"DWWWWWPPPPPUUUUU"	},
//		{8,	"CODE128",	BARCODE_TYPE_CODE128,	0,	"DDIIIIPPPPPPUUUUUU"	}
//	};
	BARCODE_STRUCT EmartForm[] = {
		{1,	"CODE128",	BARCODE_TYPE_CODE128,	0,	"IIIIIIPPPPPPEUUUUU"	},	
		{2,    	"보관", 	BARCODE_TYPE_CODE128, 	0,	"NNNNNNTTTTTTTTTTTTTTuussssgg" },	//28 digit
		{3,	"차세대",	BARCODE_TYPE_CODE128,	0,	"IIIIIIPPPPPPEWWWWW"	},	
	};
	for (i=0;i<3;i++)
	{
		set_base_sparam(FLASH_BARCODE_FORMAT_AREA + (BARCODE_STRUCT_SIZE * (INT32U)i),(INT8U *)&EmartForm[i],sizeof(BARCODE_STRUCT));
	}
} 

#define EMART_SCANNER_LENGTH	33
void	scanner_default_for_Emart(void)
{
	INT32U	addr;
	INT16U	i;
	INT8U	scannerDefaultBarForm[][EMART_SCANNER_LENGTH] = {
			"TTTTTTTTTTTT",				//12 개체번호/묶음번호
			"TTTTTTTTTTTTTTT", 			//15 개체번호/묶음번호  //USE_EMART_PIG_TRACE
			"NNNNNNTTTTTTTTTTTTuu",			//20 호출코드 개체/묶음
			"NNNNNNTTTTTTTTTTTTTTTTTTTTTTTTuu",	//32 수입육 LOT번호 
			"NNNNNNTTTTTTTTTTTTuussssgg",		//26 호출코드 개체/묶음 + 용도 + 도축장 + 등급
			"NNNNNNTTTTTTTTTTTTTTuussssgg",		//28 호출코드 개체/묶음 + 용도 + 도축장 + 등급 
			"NNNNNNTTTTTTTTTTTTTTTuussssgg", 	//29 호출코드 개체/묶음 + 용도 + 도축장 + 등급
		};
												
	addr = DFLASH_BASE + FLASH_SCANNER_FORMAT;
	for (i = 0; i < 7; i++)
	{
		Flash_swrite(addr+(i*40), scannerDefaultBarForm[i], EMART_SCANNER_LENGTH);
	}
}
void	labelfile_default_for_Emart(void)
{
	INT32U  addr;
	INT8U	labelfmtNo;
	INT8U	barfmtNo;

	//보관 라벨
	addr = DFLASH_BASE + FLASH_LABEL_KEY_TABLE_AREA;
	labelfmtNo = 75;
	barfmtNo = 2;
	Flash_wwrite(addr, (INT16U)labelfmtNo);
	Flash_wwrite(addr+2, (INT16U)barfmtNo);
}

#elif defined(USE_GSMART_DEFAULT)

void    barcodefile_default_for_GSmart(void)
{
	INT8U i;
	BARCODE_STRUCT GSmartForm[] = {
		{1,	"GS판매바코드",	BARCODE_TYPE_CODE128,	0,	"NNNNNNWWWWWPPPPPPXHHHHHC"	}, //24 digit
	};

	for (i=0;i<1;i++)
	{
		set_base_sparam(FLASH_BARCODE_FORMAT_AREA + (BARCODE_STRUCT_SIZE * (INT32U)i),(INT8U *)&GSmartForm[i],sizeof(BARCODE_STRUCT));
	} 
} 

#elif defined(USE_GSMART_TRACE_DEFAULT)

void    barcodefile_default_for_GSmart_trace(void)
{
	INT8U i;
	BARCODE_STRUCT GSmartForm[] = {
		{1,	"GS판매바코드",	BARCODE_TYPE_CODE128,	0,	"NNNNNNWWWWWPPPPPPXHHHHHC"	}, //24 digit
		//{2,	"SCANNER",	BARCODE_TYPE_CODE128,	0,	"ss00NNNNNN0000TTTTTTTTTTTT0000"}, //30 digit   //gspig 20141212
		{2,	"SCANNER",	BARCODE_TYPE_CODE128,	0,	"ss00NNNNNN0000TTTTTTTTTTTTTT"}, //28 digit
		{3,	"개체바코드 12자리",BARCODE_TYPE_CODE128,	0,	"TTTTTTTTTTTT"}, //12 digit
		{4,	"개체바코드 14자리",BARCODE_TYPE_CODE128,	0,	"TTTTTTTTTTTTTT"}, //14 digit
	};

	for (i=0;i<sizeof(GSmartForm)/sizeof(BARCODE_STRUCT);i++)
	{
		set_base_sparam(FLASH_BARCODE_FORMAT_AREA + (BARCODE_STRUCT_SIZE * (INT32U)i),(INT8U *)&GSmartForm[i],sizeof(BARCODE_STRUCT));
	} 
} 

#define GSMART_SCANNER_LENGTH	33
void	scanner_default_for_GSmart(void)
{
	INT32U	addr;
	INT16U	i;
	INT8U	scannerGSBarForm[][GSMART_SCANNER_LENGTH] = { //gspig 20141212 31->33
			"ss00NNNNNN0000TTTTTTTTTTTT0000", //30 상품보관 //gspig 20141212
			"ss00NNNNNN0000TTTTTTTTTTTTTT0000", //32 상품정보 바코드 
			"ss00NNNNNN0000TTTTTTTTTTTTTT", //28 보관라벨 바코드 
			"TTTTTTTTTTTT",			  //12 개체번호
			"000TTTTTTTTTTTTTTTTTTTT", 	  //23 선하증권
			"TTTTTTTTTTTTTT",  //14 LOT 번호 
			"TTTTTTTTTTTTTTT",  //15 LOT 번호 
	};
												
	addr = DFLASH_BASE + FLASH_SCANNER_FORMAT;

	for (i = 0; i < sizeof(scannerGSBarForm)/GSMART_SCANNER_LENGTH; i++) //gspig 20141212
	{
		Flash_swrite(addr+(i*40), scannerGSBarForm[i], GSMART_SCANNER_LENGTH);//gspig 20141212
	}
}

void	labelfile_default_for_GSmart(void)
{
	INT32U  addr;
	INT8U	labelfmtNo;
	INT8U	barfmtNo;

	//상품보관 라벨
	addr = DFLASH_BASE + FLASH_LABEL_KEY_TABLE_AREA;
	labelfmtNo = 55;
	barfmtNo = 2;
	Flash_wwrite(addr, (INT16U)labelfmtNo);
	Flash_wwrite(addr+2, (INT16U)barfmtNo);

	//대면고지 라벨
	addr += 6; //sizeof(TRACE_LABEL_STRUCT);
	labelfmtNo = 54;
	barfmtNo = 2;
	Flash_wwrite(addr, (INT16U)labelfmtNo);
	Flash_wwrite(addr+2, (INT16U)barfmtNo);
}

#elif defined(USE_NHMART_DEFAULT) || defined(USE_NHMART_SAFE_DEFAULT)


void    barcodefile_default_for_NHmart(void)
{
	INT8U i;
  #ifdef USE_NHMART_SAFE_DEFAULT    
	BARCODE_STRUCT nhmartForm[] = {
		{1,	"NH LABEL",	BARCODE_TYPE_I2OF5,	0,	"IIIIIIPPPPPPEWWWWW0C"		}, //20 digits
		{2,	"이력번호",	BARCODE_TYPE_CODE128,	0,	"TTTTTTTTTTTTTTTTTT"	} //18 digit
	};
  #else // USE_NHMART_SAFE_DEFAULT
	BARCODE_STRUCT nhmartForm[] = {
		{1, "NH WEIHGT LABEL", BARCODE_TYPE_CODE128, 0,	"IIIIIIPPPPPPCWWWWWWC" 	}, //20 digits 무게 바코드
		{2,	"NH QTY LABEL",	BARCODE_TYPE_EAN13,	0,	"IIIIIIPPPPPPC"		}, //13 digits 개수 바코드
		{3,	"이력번호 18자리",	BARCODE_TYPE_CODE128,	0,	"TTTTTTTTTTTTTTTTTT"	}, //18 digit
		{4,	"이력번호 14자리",	BARCODE_TYPE_CODE128,	0,	"TTTTTTTTTTTTTT"	}, //14 digit  // hanaro 20150116
		{5,	"이력번호 12자리",	BARCODE_TYPE_CODE128,	0,	"TTTTTTTTTTTT"	}, //12 digit
    #ifdef USE_SETPRODUCT_SALE
		{6, "세트상품 라벨",	BARCODE_TYPE_CODE128,	0,	"289467PPPPPPOOOOOOnnnnnn"	}, //24 digit
    #endif

	};
  #endif // USE_NHMART_SAFE_DEFAULT

	for (i=0;i<sizeof(nhmartForm)/sizeof(BARCODE_STRUCT);i++)
	{
		set_base_sparam(FLASH_BARCODE_FORMAT_AREA + (BARCODE_STRUCT_SIZE * (INT32U)i),(INT8U *)&nhmartForm[i],sizeof(BARCODE_STRUCT));
	} 
}

#define NHMART_SCANNER_LENGTH	19
void	scanner_default_for_NHmart(void)
{
	INT32U	addr;
	INT16U	i;
  #ifdef USE_NHMART_SAFE_DEFAULT
	INT8U	scannerDefaultBarForm[][NHMART_SCANNER_LENGTH] = {
			"TTTTTTTTTTTT",				//12 개체번호
		};

  #else // USE_NHMART_SAFE_DEFAULT
	INT8U	scannerDefaultBarForm[][NHMART_SCANNER_LENGTH] = {
			"TTTTTTTTTTTTTTTTTT",				//18 개체번호/묶음번호
			"TTTTTTTTTTTTTTT",				//15 개체번호/묶음번호			
			"TTTTTTTTTTTTTT",				//14 개체번호/묶음번호 // hanaro 20150116
			"TTTTTTTTTTTT",				//12 개체번호/묶음번호
    #ifdef USE_SETPRODUCT_SALE
			"NNNNNNPPPPPPWWWWWW",	//18 세트상품 내품 바코드
    #endif
		};
  #endif // USE_NHMART_SAFE_DEFAULT
												
	addr = DFLASH_BASE + FLASH_SCANNER_FORMAT;

	for (i = 0; i < sizeof(scannerDefaultBarForm)/NHMART_SCANNER_LENGTH; i++)
	{
		Flash_swrite(addr+(i*40), scannerDefaultBarForm[i], NHMART_SCANNER_LENGTH);
	}
}
#elif defined(USE_CHINA_DEFAULT)
  #if defined(USE_CHINA_BAONENG) || defined(USE_CHINA_BAONENG_COUNTRYGARDEN)
void barcodefile_default_for_chn(void)
{
	INT8U i;
	BARCODE_STRUCT ChnBarForm[] = {
		{1,	"Weight", BARCODE_TYPE_CODE128,	0, "29IIIIIPPPPPWWWWWC"}, 	//18 digit
		{2, "Count", BARCODE_TYPE_CODE128,	0, "28IIIIIPPPPPQQQQQC"},	//18 digit
	};

	for (i = 0 ; i < sizeof(ChnBarForm)/sizeof(BARCODE_STRUCT) ; i++)
	{
		set_base_sparam(FLASH_BARCODE_FORMAT_AREA + (BARCODE_STRUCT_SIZE * (INT32U)i),(INT8U *)&ChnBarForm[i],sizeof(BARCODE_STRUCT));
	} 
}
  #elif defined(USE_CHINA_CARRFOUR_DEFAULT)
void	  barcodefile_default_for_chn(void)
{
  INT8U i;

  BARCODE_STRUCT ChnBarForm[] = {
	  {1, "Default",  BARCODE_TYPE_EAN13, 0,  "22IIIIIIPPPPC" },
	  {2, "Carrfour",	  BARCODE_TYPE_CODE128,   0,  "IIIIIIfWWWWWPPPPPC"}
  };

  //i = 0;
  for (i=0;i<2;i++)
  {
	  set_base_sparam(FLASH_BARCODE_FORMAT_AREA + (BARCODE_STRUCT_SIZE * (INT32U)i),(INT8U *)&ChnBarForm[i],sizeof(BARCODE_STRUCT));
  }
} 
  #else
void    barcodefile_default_for_chn(void)
{
	INT8U i;

	BARCODE_STRUCT ChnBarForm[] = {
		//{1,	"Default",	BARCODE_TYPE_EAN13,	0,	"22IIIIIIPPPPC"	}
		{1,	"Default",	BARCODE_TYPE_EAN13,	0,	"DDIIIIIPPPPPC"	}	//생산요청에 따른 기본값 변경
	};

	i = 0;
	set_base_sparam(FLASH_BARCODE_FORMAT_AREA + (BARCODE_STRUCT_SIZE * (INT32U)i),(INT8U *)&ChnBarForm[i],sizeof(BARCODE_STRUCT));
} 
  #endif
#elif defined(USE_LOTTEMART_DEFAULT)
void    barcodefile_default_for_Lottemart(void)
{
	INT8U i;
	BARCODE_STRUCT lottemartForm[] = {
/*
		{1,	"중량상품",		BARCODE_TYPE_RSSEXP,	0,	"010DDIIIIPPPPPPc7003JJJJJJjjjj3103WWWWWW"},	// 50 digits
		{2,	"개수상품",		BARCODE_TYPE_RSSEXP,	0,	"010DDIIIIPPPPPPc7003JJJJJJjjjj30QQQQQQQQ"},	// 50 digit
		{3,	"판매용", 	BARCODE_TYPE_EAN13, 	0, 	"DDIIIIPPPPPPC"},	// 13
#ifdef USE_LOTTEMART_PIG_TRACE
		{4,	"보관용", 	BARCODE_TYPE_CODE128, 	0,	"NNNNNNTTTTTTTTTTTTTTuuss"}, 	// 24 //USE_LOTTEMART_PIG_TRACE
		{5, "신형중량", 	BARCODE_TYPE_RSSEXP,	0,	"01iiiiiiiiiiiiic7003JJJJJJjjjj3103WWWWWW392000000000PPPPPPP"},	// 59 digits 20160229
		{6, "신형개수", 	BARCODE_TYPE_RSSEXP,	0,	"01iiiiiiiiiiiiic7003JJJJJJjjjj30QQQQQQQQ392000000000PPPPPPP"},	// 59 digit
#else
		{4, "보관용",	BARCODE_TYPE_CODE128,	0,	"NNNNNNTTTTTTTTTTTTuuss"} 	// 22
	};
	for (i=0;i<4;i++)
#endif
*/
		{1, "중량상품", 	BARCODE_TYPE_RSSEXP,	0,	"010iiiiiiiiiiiii7003JJJJJJjjjjq3103WWWWWW3920PPPPPPPq99x"},	// 56 digits
		{2, "개수상품", 	BARCODE_TYPE_RSSEXP,	0,	"010iiiiiiiiiiiii7003JJJJJJjjjjq30QQQQQQQQq3920PPPPPPPq99x"},	// 57 digit
		{3, "판매용",	BARCODE_TYPE_EAN13, 	0,	"DDIIIIPPPPPPC"},	// 13
		{4, "보관용",	BARCODE_TYPE_CODE128,	0,	"NNNNNNTTTTTTTTTTTTTTuuss"},	// 24 //USE_LOTTEMART_PIG_TRACE
		{5, "중량", 	BARCODE_TYPE_RSSEXP,	0,	"010iiiiiiiiiiiii7003JJJJJJjjjjq3103WWWWWW3920PPPPPPP"},	// 52 digits
		{6, "개수", 	BARCODE_TYPE_RSSEXP,	0,	"010iiiiiiiiiiiii7003JJJJJJjjjjq30QQQQQQQQq3920PPPPPPP"},	// 53 digit

#ifdef USE_LOTTEMART_SUPER_INTEGRATION_DEFAULT
		// 기존 롯데슈퍼 1~7 -> 7~13
		{7, "중량상품", BARCODE_TYPE_RSSEXP,	0,	"010iiiiiiiiiiiiiq8008kkkkkkmmmmq7003JJJJJJjjjjq3103WWWWWWq3920PPPPPPPq99x"},	// 73 digits  [FNC1]+010+PLU(13자리)+[FNC1]+8008+생산일시(10자리)+[FNC1]+7003+유효일시(10자리)+[FNC1]+3103+중량(6자리)+[FNC1]+3920+금액(7자리)+[FNC1]+99+체크디지트(1자리)
		{8, "개수상품", BARCODE_TYPE_RSSEXP,	0,	"010iiiiiiiiiiiiiq8008kkkkkkmmmmq7003JJJJJJjjjjq30QQQQQQQQq3920PPPPPPPq99x"},	// 73 digits  [FNC1]+010+PLU(13자리)+[FNC1]+8008+생산일시(10자리)+[FNC1]+7003+유효일시(10자리)+[FNC1]+30+수량(8자리)+[FNC1]+3920+금액(7자리)+[FNC1]+99+체크디지트(1자리)
		{9, "판매용",		BARCODE_TYPE_EAN13, 	0,	"DDIIIIPPPPPPC"},																// 13 digits
		{10, "보관용",		BARCODE_TYPE_CODE128,	0,	"NNNNNNTTTTTTTTTTTTTTuuss"},													// 24 digits
		{11, "중량",			BARCODE_TYPE_RSSEXP,	0,	"010iiiiiiiiiiiiiq7003JJJJJJjjjjq3103WWWWWWq3920PPPPPPP"},						// 55 digits
		{12, "개수",			BARCODE_TYPE_RSSEXP,	0,	"010iiiiiiiiiiiiiq7003JJJJJJjjjjq30QQQQQQQQq3920PPPPPPP"},						// 55 digits
		{13, "농수산용",	BARCODE_TYPE_I2OF5,		0,	"DDIIIIPPPPPPWWWWWC"}
#endif
	};
	for (i=0;i<sizeof(lottemartForm)/sizeof(lottemartForm[0]);i++)
	{
		set_base_sparam(FLASH_BARCODE_FORMAT_AREA + (BARCODE_STRUCT_SIZE * (INT32U)i),(INT8U *)&lottemartForm[i],sizeof(BARCODE_STRUCT));
	} 
}

#ifdef USE_LOTTEMART_MEATSCANNER
#define LOTTEMART_SCANNER_LENGTH	50
#else
#define LOTTEMART_SCANNER_LENGTH	50
#endif
void	scanner_default_for_Lottemart(void)
{
	INT32U	addr;
	INT16U	i;
	INT8U	scannerLotteBarForm[][LOTTEMART_SCANNER_LENGTH] = {
			"TTTTTTTTTTTT",					// 12 개체번호/ 기존 묶음번호
			"iiiiiiiiiiiii", 				// 13자리 아이템 코드
			"TTTTTTTTTTTTTTT",				// 15 묶음번호  //USE_LOTTEMART_PIG_TRACE
			"NNNNNNggssstttvvvTTTTTTTTTTTTTTTTTTTTTTTT", //41자리 스캔바코드
			"NNNNNNTTTTTTTTTTTTTTuuss",
#ifdef USE_LOTTEMART_SUPER_INTEGRATION_DEFAULT
			//"NNNNNNTTTTTTTTTTTTTTuuss", 	  //24 보관라벨
			"NNNNNNTTTTTTTTTTTTuuss",
#endif
	};
												
	addr = DFLASH_BASE + FLASH_SCANNER_FORMAT;

	for (i = 0; i < sizeof(scannerLotteBarForm)/LOTTEMART_SCANNER_LENGTH; i++)
	{
		Flash_swrite(addr+(i*BAR_FORMAT_SIZE), scannerLotteBarForm[i], LOTTEMART_SCANNER_LENGTH);
	}
}

void labelfile_default_for_Lottemart(void)		//라벨 변경키 default
{
	INT32U  	addr;
	INT8U	labelfmtNo;
	INT8U	barfmtNo;

//일반라벨
	addr = DFLASH_BASE + FLASH_LABEL_KEY_TABLE_AREA;
	labelfmtNo = 81;
	barfmtNo = 1;
	Flash_wwrite(addr, (INT16U)labelfmtNo);
	Flash_wwrite(addr+2, (INT16U)barfmtNo);
//보관라벨
	addr += 6; //sizeof(TRACE_LABEL_STRUCT);
	labelfmtNo = 82;
	barfmtNo = 4;
	Flash_wwrite(addr, (INT16U)labelfmtNo);
	Flash_wwrite(addr+2, (INT16U)barfmtNo);
//세트라벨
	addr += 6; //sizeof(TRACE_LABEL_STRUCT);
	labelfmtNo = 83;
	barfmtNo = 4;
	Flash_wwrite(addr, (INT16U)labelfmtNo);
	Flash_wwrite(addr+2, (INT16U)barfmtNo);

#ifdef USE_LOTTEMART_SUPER_INTEGRATION_DEFAULT
	/* 롯데마트 3개 + 롯데슈퍼 4개 = 총 7개(MAX : 8개) */
	// 이하 롯데슈퍼 설정
// 일반라벨
	addr += 6;
	labelfmtNo = 95;
	//barfmtNo = 1;
	barfmtNo = 7;
	Flash_wwrite(addr, (INT16U)labelfmtNo);
	Flash_wwrite(addr+2, (INT16U)barfmtNo);
// 보관라벨
	addr += 6;
	//labelfmtNo = 96;
	labelfmtNo = 98;
	//barfmtNo = 4;
	barfmtNo = 10;
	Flash_wwrite(addr, (INT16U)labelfmtNo);
	Flash_wwrite(addr+2, (INT16U)barfmtNo);
// 일반라벨 (무게 상품)
	addr += 6;
	labelfmtNo = 95;
	//barfmtNo = 1;
	barfmtNo = 7;
	Flash_wwrite(addr, (INT16U)labelfmtNo);
	Flash_wwrite(addr+2, (INT16U)barfmtNo);
// 일반라벨 (개수 상품)
	addr += 6;
	labelfmtNo = 95;
	//barfmtNo = 2;
	barfmtNo = 8;
	Flash_wwrite(addr, (INT16U)labelfmtNo);
	Flash_wwrite(addr+2, (INT16U)barfmtNo);
#endif
}

#elif defined(USE_LOTTESUPER_DEFAULT)

void barcodefile_default_for_Lottesuper(void)
{
	INT8U i;
	BARCODE_STRUCT lottesuperForm[] = {
		{1, "중량상품", BARCODE_TYPE_RSSEXP,	0,	"010iiiiiiiiiiiiiq8008kkkkkkmmmmq7003JJJJJJjjjjq3103WWWWWWq3920PPPPPPPq99x"},	// 73 digits  [FNC1]+010+PLU(13자리)+[FNC1]+8008+생산일시(10자리)+[FNC1]+7003+유효일시(10자리)+[FNC1]+3103+중량(6자리)+[FNC1]+3920+금액(7자리)+[FNC1]+99+체크디지트(1자리)
		{2, "개수상품", BARCODE_TYPE_RSSEXP,	0,	"010iiiiiiiiiiiiiq8008kkkkkkmmmmq7003JJJJJJjjjjq30QQQQQQQQq3920PPPPPPPq99x"},	// 73 digits  [FNC1]+010+PLU(13자리)+[FNC1]+8008+생산일시(10자리)+[FNC1]+7003+유효일시(10자리)+[FNC1]+30+수량(8자리)+[FNC1]+3920+금액(7자리)+[FNC1]+99+체크디지트(1자리)
		{3, "판매용",		BARCODE_TYPE_EAN13, 	0,	"DDIIIIPPPPPPC"},																// 13 digits
		{4, "보관용",		BARCODE_TYPE_CODE128,	0,	"NNNNNNTTTTTTTTTTTTTTuuss"},													// 24 digits
		{5, "중량",			BARCODE_TYPE_RSSEXP,	0,	"010iiiiiiiiiiiiiq7003JJJJJJjjjjq3103WWWWWWq3920PPPPPPP"},						// 55 digits
		{6, "개수",			BARCODE_TYPE_RSSEXP,	0,	"010iiiiiiiiiiiiiq7003JJJJJJjjjjq30QQQQQQQQq3920PPPPPPP"},						// 55 digits
		{7,	"농수산용",	BARCODE_TYPE_I2OF5,		0,	"DDIIIIPPPPPPWWWWWC"}															// 18 digits
	};
	
	for (i=0;i<7;i++)
	{
		set_base_sparam(FLASH_BARCODE_FORMAT_AREA + (BARCODE_STRUCT_SIZE * (INT32U)i),(INT8U *)&lottesuperForm[i],sizeof(BARCODE_STRUCT));
	}
}

#define LOTTESUPER_SCANNER_LENGTH	25
void	scanner_default_for_Lottesuper(void)
{
	INT32U	addr;
	INT16U	i;
	INT8U	scannerLottesuperBarForm[][LOTTESUPER_SCANNER_LENGTH] = {
			"NNNNNNTTTTTTTTTTTTTTuuss", 	  //24 보관라벨
			"NNNNNNTTTTTTTTTTTTuuss",
			"TTTTTTTTTTTT", //이력번호 12자리 
			"TTTTTTTTTTTTTTT", //LOT 번호 15자리 
	};
												
	addr = DFLASH_BASE + FLASH_SCANNER_FORMAT;

	for (i = 0; i < 4; i++)
	{
		Flash_swrite(addr+(i*40), scannerLottesuperBarForm[i], LOTTESUPER_SCANNER_LENGTH);
	}
}

void labelfile_default_for_Lottesuper(void)		//라벨 변경키 default
{
	INT32U  	addr;
	INT8U	labelfmtNo;
	INT8U	barfmtNo;
	INT8U	structSize;

	structSize = sizeof(LABEL_KEY_STRUCT); // 라벨변경 정보 저장용 구조체 크기 : 6
// 일반라벨
	addr = DFLASH_BASE + FLASH_LABEL_KEY_TABLE_AREA;
	labelfmtNo = 95;
	barfmtNo = 1;
	Flash_wwrite(addr, (INT16U)labelfmtNo);
	Flash_wwrite(addr+2, (INT16U)barfmtNo);
// 보관라벨
	addr += structSize;
	labelfmtNo = 96;
	barfmtNo = 4;
	Flash_wwrite(addr, (INT16U)labelfmtNo);
	Flash_wwrite(addr+2, (INT16U)barfmtNo);
// 일반라벨 (무게 상품)
	addr += structSize;
	labelfmtNo = 95;
	barfmtNo = 1;
	Flash_wwrite(addr, (INT16U)labelfmtNo);
	Flash_wwrite(addr+2, (INT16U)barfmtNo);
// 일반라벨 (개수 상품)
	addr += structSize;
	labelfmtNo = 95;
	barfmtNo = 2;
	Flash_wwrite(addr, (INT16U)labelfmtNo);
	Flash_wwrite(addr+2, (INT16U)barfmtNo);
}
#elif defined(USE_HYUNDAI_DEFAULT)
void barcodefile_default_for_Hyundai(void)
{
	INT8U i;
	BARCODE_STRUCT bar_default_Hyundai_Form[] = {
		{1,	"I2OF5",	BARCODE_TYPE_I2OF5,		0,	"DDIIIIIPPPPPPPWWWWWC"}, //20digit
		{2,	"CODE128",	BARCODE_TYPE_CODE128,	0,	"NNNNNNTTTTTTTTTTTTTTuussKKKKgg"}, //30 digit
		{3, "EAN-13",	BARCODE_TYPE_EAN13, 	0,	"DDIIIIPPPPPPC"}, //13digit
		{4,	"I2OF5",	BARCODE_TYPE_I2OF5,		0,	"DDIIIIIPPPPPPPhWWWWC"}, //20digit
	};

	for (i = 0 ; i < 4 ; i++)
	{
		set_base_sparam(FLASH_BARCODE_FORMAT_AREA + (BARCODE_STRUCT_SIZE * (INT32U)i),(INT8U *)&bar_default_Hyundai_Form[i],sizeof(BARCODE_STRUCT));
	} 
}

#define HYUNDAI_SCANNER_LENGTH	31
void	scanner_default_for_Hyundai(void)
{
	INT32U	addr;
	INT16U	i;
	INT8U	scannerHyundaiBarForm[][HYUNDAI_SCANNER_LENGTH] = {
			"NNNNNNTTTTTTTTTTTTuussKKKKgg", 	  //28 보관라벨
			"NNNNNNTTTTTTTTTTTTTTuussKKKKgg", 	  //30 보관라벨
			"TTTTTTTTTTTT",
			"TTTTTTTTTTTTTTT",
			"880eeeeeeeeee",		  //13 친환경 인증 바코드.
			"rrrrrrrrrrrrrrrrrrrrrr", // 22 영수증 번호
			"NNNNNPPPPPPPWWWW" 	// 16 세트 내품 호출 바코드
	};
												
	addr = DFLASH_BASE + FLASH_SCANNER_FORMAT;

	for (i = 0; i < 7; i++)
	{
		Flash_swrite(addr+(i*40), scannerHyundaiBarForm[i], HYUNDAI_SCANNER_LENGTH);
	}
}
void labelfile_default_for_Hyundai(void)		//라벨 변경키 default
{
	INT32U  	addr;
	INT8U	labelfmtNo;
	INT8U	barfmtNo;

//일반라벨
	addr = DFLASH_BASE + FLASH_LABEL_KEY_TABLE_AREA;
	labelfmtNo = 92;
	barfmtNo = 1;
	Flash_wwrite(addr, (INT16U)labelfmtNo);
	Flash_wwrite(addr+2, (INT16U)barfmtNo);
//보관라벨
	addr += 6; //sizeof(TRACE_LABEL_STRUCT);
	labelfmtNo = 94;
	barfmtNo = 2;
	Flash_wwrite(addr, (INT16U)labelfmtNo);
	Flash_wwrite(addr+2, (INT16U)barfmtNo);
}

void labelfile_change_for_Hyundai(INT16U labelfmtNo, INT16U barfmtNo, INT32U labelkeyNo)
{
	INT32U  	addr;
	
	addr = DFLASH_BASE + FLASH_LABEL_KEY_TABLE_AREA;
	addr = addr + (labelkeyNo - 1) * 6; // key No. Start 1, Memory Start 0, sizeof(TRACE_LABEL_STRUCT) = 6;
	Flash_wwrite(addr, (INT16U)labelfmtNo);
	Flash_wwrite(addr+2, (INT16U)barfmtNo);
}

#elif defined(USE_MANAGE_INVENTORY_DEFAULT)
void    barcodefile_default_for_ManageInventory(void)
{
	INT8U i;
	BARCODE_STRUCT inventoryForm[] = {
		{1,	"CODE128X",	BARCODE_TYPE_CODE128,	0,	"TTTTTTTTTTTT"	}
	};
	i = 0;
	set_base_sparam(FLASH_BARCODE_FORMAT_AREA + (BARCODE_STRUCT_SIZE * (INT32U)i),(INT8U *)&inventoryForm[i],sizeof(BARCODE_STRUCT));
}
void labelfile_default_for_ManageInventory(void)		//라벨 변경키 default
{
	INT32U  addr;
	INT8U	labelfmtNo;
	INT8U	barfmtNo;
//기본라벨
	addr = DFLASH_BASE + FLASH_LABEL_KEY_TABLE_AREA;
	labelfmtNo = 70;
	barfmtNo = 1;
	Flash_wwrite(addr, (INT16U)labelfmtNo);
	Flash_wwrite(addr+2, (INT16U)barfmtNo);
//수량 직접입력 라벨
	addr += 6; //sizeof(TRACE_LABEL_STRUCT);
	labelfmtNo = 71;
	barfmtNo = 1;
	Flash_wwrite(addr, (INT16U)labelfmtNo);
	Flash_wwrite(addr+2, (INT16U)barfmtNo);
}
#elif defined(USE_EMPORIUM_DEFAULT)
void barcodefile_default_for_Emporium(void)
{
	INT8U i;
	BARCODE_STRUCT EmporiumForm[] = {
		{1,	"Emporium Weight",	BARCODE_TYPE_CODE128,	0,	"%#IIIIIVPPPPPPPWWWWWC"	}, //20 digit
		{2,	"Emporium Count",	BARCODE_TYPE_CODE128,	0,	"%#IIIIIVPPPPPPPQQ000C"	}, //20 digit
	};

	for (i=0;i<2;i++)
	{
		set_base_sparam(FLASH_BARCODE_FORMAT_AREA + (BARCODE_STRUCT_SIZE * (INT32U)i),(INT8U *)&EmporiumForm[i],sizeof(BARCODE_STRUCT));
	} 
}
#elif defined(USE_VIETNAM_EMART_DEFAULT)
void    barcodefile_default_for_Vietman_Emart(void)
{
	INT8U i;
	BARCODE_STRUCT VietnamEmartForm[] = {
		{1,	"CODE128",	BARCODE_TYPE_CODE128,	0,	"NNNNNNPPPPPPPEWWWWWC"	},
		{2,	"CODE128",	BARCODE_TYPE_CODE128,	0,	"NNNNNNPPPPPPPEQQQQQC"	},
	};
	for (i=0;i<2;i++)
	{
		set_base_sparam(FLASH_BARCODE_FORMAT_AREA + (BARCODE_STRUCT_SIZE * (INT32U)i),(INT8U *)&VietnamEmartForm[i],sizeof(BARCODE_STRUCT));
	}
}
#elif defined(USE_MON_EMART_DEFAULT)
void    barcodefile_default_for_mon_emart(void)
{
	INT8U i;
	BARCODE_STRUCT VietnamEmartForm[] = {
		{1,	"CODE128",	BARCODE_TYPE_CODE128,	0,	"20IIIIIIWWWWWPPPPPPC"	},
		{2,	"CODE128",	BARCODE_TYPE_CODE128,	0,	"20IIIIIIQQQQQPPPPPPC"	},
	};
	for (i=0;i<2;i++)
	{
		set_base_sparam(FLASH_BARCODE_FORMAT_AREA + (BARCODE_STRUCT_SIZE * (INT32U)i),(INT8U *)&VietnamEmartForm[i],sizeof(BARCODE_STRUCT));
	}
} 
#elif defined(USE_TOPMART_DEFAULT) //sewon 161122
void    barcodefile_default_for_Topmart(void)
{
	INT8U i;
	BARCODE_STRUCT TopmartForm[] = {
		{1,	"무게판매",	BARCODE_TYPE_CODE128,	0,	"25IIIIIEWWWWWPPPPPPC"	}, //240digit
		{2,	"개수판매",	BARCODE_TYPE_CODE128,	0,	"25IIIIIEQQQQQPPPPPPC"}, //28 digit
		{3,	"개체바코드 12자리",BARCODE_TYPE_CODE128,	0,	"TTTTTTTTTTTT"}, //12 digit
		{4,	"개체바코드 16자리",BARCODE_TYPE_CODE128,	0,	"TTTTTTTTTTTTTTTT"}, //16 digit
		{5,	"개체바코드 24자리",BARCODE_TYPE_CODE128,	0,	"TTTTTTTTTTTTTTTTTTTTTTTT"}, //24 digit
		{6,	"무게판매 GS1",	BARCODE_TYPE_RSSEXP,	0,	"25IIIIIEWWWWWPPPPPPCkkkkkkmmmmmm"},//32 digit
		{7,	"개수판매 GS1",BARCODE_TYPE_RSSEXP,	0,	"25IIIIIEQQQQQPPPPPPCkkkkkkmmmmmm"},	//32 digit
	};

	for (i=0;i<7;i++)
	{
		set_base_sparam(FLASH_BARCODE_FORMAT_AREA + (BARCODE_STRUCT_SIZE * (INT32U)i),(INT8U *)&TopmartForm[i],sizeof(BARCODE_STRUCT));
	} 
} 

#define TOPMART_SCANNER_LENGTH	33
void	scanner_default_for_Topmart(void)
{
	INT32U	addr;
	INT16U	i;
	INT8U	scannerTopBarForm[][TOPMART_SCANNER_LENGTH] = { //gspig 20141212 31->33
			"ss00NNNNNN0000TTTTTTTTTTTT0000", //30 상품보관 //gspig 20141212
			"ss00NNNNNN0000TTTTTTTTTTTTTT0000", //32 상품정보 바코드 
			"ss00NNNNNN0000TTTTTTTTTTTTTT", //28 보관라벨 바코드 
			"TTTTTTTTTTTT",			  //12 개체번호
			"000TTTTTTTTTTTTTTTTTTTT", 	  //23 선하증권
			"TTTTTTTTTTTTTTTT",  //16 LOT 번호 
			"TTTTTTTTTTTTTTT",  //15 LOT 번호
			"TTTTTTTTTTTTTTTTTTTTTTTT", 	  //24 수입육 LOT
	};
												
	addr = DFLASH_BASE + FLASH_SCANNER_FORMAT;

	for (i = 0; i < 8; i++)
	{
		Flash_swrite(addr+(i*40), scannerTopBarForm[i], TOPMART_SCANNER_LENGTH);//gspig 20141212
	}
}

void	labelfile_default_for_Topmart(void)
{
	INT32U  addr;
	INT8U	labelfmtNo;
	INT8U	barfmtNo;

	//한우 이력 라벨
	addr = DFLASH_BASE + FLASH_LABEL_KEY_TABLE_AREA;
	labelfmtNo = 51;
	barfmtNo = 1;
	Flash_wwrite(addr, (INT16U)labelfmtNo);
	Flash_wwrite(addr+2, (INT16U)barfmtNo);

	//한우 대면 라벨
	addr += 6; //sizeof(TRACE_LABEL_STRUCT);
	labelfmtNo = 52;
	barfmtNo = 4;
	Flash_wwrite(addr, (INT16U)labelfmtNo);
	Flash_wwrite(addr+2, (INT16U)barfmtNo);

	//부속 한우 라벨
	addr += 6; //sizeof(TRACE_LABEL_STRUCT);
	labelfmtNo = 53;
	barfmtNo = 1;
	Flash_wwrite(addr, (INT16U)labelfmtNo);
	Flash_wwrite(addr+2, (INT16U)barfmtNo);

	//돼지 이력 라벨
	addr += 6; //sizeof(TRACE_LABEL_STRUCT);
	labelfmtNo = 54;
	barfmtNo = 1;
	Flash_wwrite(addr, (INT16U)labelfmtNo);
	Flash_wwrite(addr+2, (INT16U)barfmtNo);

	//돼지 대면 라벨
	addr += 6; //sizeof(TRACE_LABEL_STRUCT);
	labelfmtNo = 55;
	barfmtNo = 4;
	Flash_wwrite(addr, (INT16U)labelfmtNo);
	Flash_wwrite(addr+2, (INT16U)barfmtNo);

	//돼지 부속  라벨
	addr += 6; //sizeof(TRACE_LABEL_STRUCT);
	labelfmtNo = 56;
	barfmtNo = 1;
	Flash_wwrite(addr, (INT16U)labelfmtNo);
	Flash_wwrite(addr+2, (INT16U)barfmtNo);

	//수산  라벨
	addr += 6; //sizeof(TRACE_LABEL_STRUCT);
	labelfmtNo = 60;
	barfmtNo = 1;
	Flash_wwrite(addr, (INT16U)labelfmtNo);
	Flash_wwrite(addr+2, (INT16U)barfmtNo);
}
#elif defined(USE_VIETNAM_DEFAULT) || defined(USE_VIETNAM_LOTTEMART_DEFAULT)
void barcodefile_default_for_vietnam(void)
{
	INT8U i;
	BARCODE_STRUCT vietnamForm[] = {
		{1,	"Weight",	BARCODE_TYPE_EAN13,		0,	"2IIIIIIWWWWWC"	},
		{2,	"Count",	BARCODE_TYPE_EAN13,		0,	"1IIIIIIQQQQQC"	},
		{3,	"New",		BARCODE_TYPE_CODE128,	0,	"F11111VPPPPPPPWWWWC"	},
	};
	for (i=0;i<3;i++)
	{
		set_base_sparam(FLASH_BARCODE_FORMAT_AREA + (BARCODE_STRUCT_SIZE * (INT32U)i),(INT8U *)&vietnamForm[i],sizeof(BARCODE_STRUCT));
	}
}
#elif defined(USE_STOKROTKA_MART_DEFAULT)
void barcodefile_default_for_poland_stokrotka(void)
{
	INT8U i;
	BARCODE_STRUCT polandStokrotkaForm[] = {
		{1,	"EAN13-29",	BARCODE_TYPE_EAN13,	0,	"29IIIIIWWWWWC"	},
	};
	for (i=0;i<1;i++)
	{
		set_base_sparam(FLASH_BARCODE_FORMAT_AREA + (BARCODE_STRUCT_SIZE * (INT32U)i),(INT8U *)&polandStokrotkaForm[i],sizeof(BARCODE_STRUCT));
	}	
}
#elif defined(USE_INDIA_TATA_DEFAULT)
void barcodefile_default_for_indiaTATA(void)
{
	INT8U i;
	BARCODE_STRUCT IndiaTATAForm[] = {
		{1, "WT",		BARCODE_TYPE_CODE128,	0,	"21NNNNUUUUUUWWWWW123"		},
		{2, "QT",		BARCODE_TYPE_CODE128,	0,	"21NNNNUUUUUUQQQQQ123"		},
		{3, "TTL",		16					,	0,	""							},
	};
	for (i=0;i<3;i++)
	{
		set_base_sparam(FLASH_BARCODE_FORMAT_AREA + (BARCODE_STRUCT_SIZE * (INT32U)i),(INT8U *)&IndiaTATAForm[i],sizeof(BARCODE_STRUCT));
	}
}
#endif

#define M_ID_BAR_NO       1
#define M_ID_BAR_DESC     2
#define M_ID_BAR_TYPE     3
#define M_ID_BAR_FORM_NO  4
#define M_ID_BAR_FORM_STR 5

void callback_end_barcode_no(INT32U num)
{
	BARCODE_STRUCT 	barcode_str;
	INT16U  strSize;
	INT32U	addr;
	INT8U  id_bar_no;
	INT8U  id_bar_desc;
	INT8U  id_bar_type;
	INT8U  id_bar_form_no;
	INT8U  id_bar_form_str;

	id_bar_no = Menu_GetMenuId(M_ID_BAR_NO);
	id_bar_desc = Menu_GetMenuId(M_ID_BAR_DESC);
	id_bar_type = Menu_GetMenuId(M_ID_BAR_TYPE);
	id_bar_form_no = Menu_GetMenuId(M_ID_BAR_FORM_NO);
	id_bar_form_str = Menu_GetMenuId(M_ID_BAR_FORM_STR);
	
	strSize = BARCODE_STRUCT_SIZE;		      
	addr = DFLASH_BASE;
	addr += FLASH_BARCODE_FORMAT_AREA;
	addr += ((num-1) * (INT32U)strSize);

	Flash_sread(addr,(HUGEDATA INT8U *)&barcode_str, strSize);
	if (barcode_str.barcode_num != num) {
		memset((INT8U *)&barcode_str, 0, strSize);
		barcode_str.barcode_num = num;
	}

	if (id_bar_no != 0xff) *Menu[id_bar_no].pval = num;
	if (id_bar_desc != 0xff) memcpy(Menu[id_bar_desc].pstr, barcode_str.barcode_description, Menu[id_bar_desc].length);
	if (id_bar_form_no != 0xff) *Menu[id_bar_form_no].pval = 0;//barcode_str.barcode_format_num;
	if (id_bar_form_str != 0xff)
	{
		memcpy(Menu[id_bar_form_str].pstr, barcode_str.barcode_format, Menu[id_bar_form_str].length);
		Menu[id_bar_form_str].pstr[Menu[id_bar_form_str].length] = 0;
	}

	if (id_bar_type != 0xff)
	{
		*Menu[id_bar_type].pval = barcode_str.barcode_type;
#if defined(USE_RSS_BARCODE)
		if(*Menu[id_bar_type].pval == 0 || *Menu[id_bar_type].pval > 14)
#elif defined(USE_ONLY_RSS_EXPAND_BARCODE)
  #ifdef USE_QR_BARCODE //20140903
  		if(*Menu[id_bar_type].pval == 0 || *Menu[id_bar_type].pval > 16)
  #else
		if(*Menu[id_bar_type].pval == 0 || *Menu[id_bar_type].pval > 15)
  #endif
#else
#ifdef USE_QR_BARCODE //20140903
#ifdef USE_QRCODE_11SET_DEFAULT
		if(*Menu[id_bar_type].pval == 0 || *Menu[id_bar_type].pval > 11)
#else
		if(*Menu[id_bar_type].pval == 0 || *Menu[id_bar_type].pval > 16)
#endif
#else
		if(*Menu[id_bar_type].pval == 0 || *Menu[id_bar_type].pval > 10)
#endif
#endif
		{
			if(Startup.country == COUNTRY_US) *Menu[id_bar_type].pval = 6;
			else *Menu[id_bar_type].pval = 1;
		}
	}

	MenuRefresh = ON;
}

void callback_end_select_barformat(INT32U num)
{
	INT8U  id_bar_type;
	INT8U  id_bar_form_str;

	id_bar_type = Menu_GetMenuId(M_ID_BAR_TYPE);
	id_bar_form_str = Menu_GetMenuId(M_ID_BAR_FORM_STR);

	Menu[id_bar_form_str].general_flag = *Menu[id_bar_type].pval;	// barcode type
#ifdef USE_QR_BARCODE
	if(*Menu[id_bar_type].pval == 16)
		*Menu[id_bar_type].pval += 1; //QR 일경우 실제 barcode type 17이지만 함수에서 16으로 선택.
#endif

	if (Menu[id_bar_form_str].pstr[0] == 0) num = 1;
	barcode_make_format(num, *Menu[id_bar_type].pval, Menu[id_bar_form_str].pstr);

#ifdef USE_QR_BARCODE
	if(*Menu[id_bar_type].pval == 17)
		*Menu[id_bar_type].pval -= 1; //QR 일경우 실제 barcode type 17이지만 함수에서 16으로 보여짐.
#endif


//	Menu[4].inhibit = OFF;

      	MenuRefresh = ON;
}

void	barcodefile_create(void) 
{
	BARCODE_STRUCT 	barcode_str;
	CAPTION_STRUCT 	cap;	
	INT16S 	i, result=0;
 	INT32U	start_addr,flash_addr; 
//	INT8U   nOldPgmMode;
	INT32U cur_id, type_no, form_no;
	INT8U  description[BARCODE_DESCRIPT_LEN+1];
	INT8U  bar_form[BARCODE_FORMAT_LEN+1];
	INT8U  form_string[BAR_INPUT_FORM_LEN];
#ifdef USE_QR_ADDTOTAL
	INT8U ttlbarcode_type;
	INT8U flag_tmp;
	INT8U bartype;
#endif


	start_addr =  DFLASH_BASE + FLASH_BARCODE_FORMAT_AREA;

	while(1)
	{	
		caption_split_by_code(85,&cap,0);
		i = 0;
		while (1)  
		{
			flash_addr = start_addr + BARCODE_STRUCT_SIZE * (INT32U)i;
			barcode_str.barcode_num = Flash_wread(flash_addr);
			if (barcode_str.barcode_num == i + 1) {
				i++;
				if(i > cap.input_max) {
					cur_id = cap.input_min;
					break;
				}
			} else {
				cur_id = i + 1;
		       		break;
			}
		}//while(1)

		flash_addr = start_addr + (BARCODE_STRUCT_SIZE * (cur_id - 1));
		Flash_sread(flash_addr, (HUGEDATA INT8U *)&barcode_str, sizeof(barcode_str));
		if(barcode_str.barcode_num != cur_id)  {
			memset((INT8U*)&barcode_str, 0x00, sizeof(barcode_str) );
		}
		barcode_str.barcode_num = cur_id;
		memcpy(description, barcode_str.barcode_description, sizeof(barcode_str.barcode_description));
		type_no = barcode_str.barcode_type;
		form_no = 0;//barcode_str.barcode_format_num;
		memcpy(bar_form, barcode_str.barcode_format, sizeof(barcode_str.barcode_format));
		bar_form[BARCODE_FORMAT_LEN] = 0;
#if defined(USE_RSS_BARCODE)
		if(type_no == 0 || type_no > 14)
#elif defined(USE_ONLY_RSS_EXPAND_BARCODE)
  #ifdef USE_QR_BARCODE
  		if(type_no == 0 || type_no > 16)
  #else
		if(type_no == 0 || type_no > 15)
  #endif
#else
  #ifdef USE_QR_BARCODE
		if(type_no == 0 || type_no > 16)
  #else
		if(type_no == 0 || type_no > 10)
  #endif
#endif
		{
			if(Startup.country == COUNTRY_US) type_no = 6;
			else type_no = 1;
		}

		Menu_Init();

		caption_split_by_code(0x1261, &cap,0);//""
		Menu_HeadCreate((HUGEDATA char *)cap.form);

		caption_split_by_code(85, &cap,0);//"Barcode No."
		caption_adjust(&cap,NULL);
		Menu_InputCreate(M_ID_BAR_NO, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
							cap.input_max, cap.input_min, 0, &cur_id, NULL, NULL, NULL);
		Menu_AddEndFunction(M_ID_BAR_NO, callback_end_barcode_no);

		caption_split_by_code(0x126B, &cap,0);//"Description"
		cap.input_length = sizeof(barcode_str.barcode_description)-1;
		caption_adjust(&cap,NULL);
		Menu_InputCreate(M_ID_BAR_DESC, (HUGEDATA char *)cap.form, MENU_TYPE_STR, cap.input_x, cap.input_dlength, cap.input_length,
							cap.input_max, cap.input_min, 0, NULL, description, NULL, NULL);
		Menu_SetProperties(M_ID_BAR_DESC, MENU_P_CHECK_BLANKS);

		caption_split_by_code(0x126C, &cap,0);//"Type"
#if defined(USE_RSS_BARCODE)
		cap.input_max = 14;
#elif defined(USE_ONLY_RSS_EXPAND_BARCODE)
  #ifdef USE_QR_BARCODE
  		cap.input_max = 16;
  #else
		cap.input_max = 15;
  #endif
#else
  #ifdef USE_QR_BARCODE
		cap.input_max = 16;
  #else
		cap.input_max = 10;
  #endif
#endif
		caption_adjust(&cap,NULL);
		Menu_InputCreate(M_ID_BAR_TYPE, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
							cap.input_max, cap.input_min, 0, &type_no, NULL, NULL, barcode_selecttype);


		caption_split_by_code(0x126D, &cap,0);//"Form"
		caption_adjust(&cap,NULL);
		Menu_InputCreate(M_ID_BAR_FORM_NO, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
							cap.input_max, cap.input_min, 0, &form_no, NULL, NULL, barcode_selectformat);
		Menu_SetCallbackMode(M_ID_BAR_FORM_NO, 1);// below
		Menu_AddEndFunction(M_ID_BAR_FORM_NO, callback_end_select_barformat);


#ifdef USE_SINGLE_LINE_GRAPHIC
		caption_split_by_code(0x126F, &cap,0);//"Form"
#else
		//caption_split_by_code(, &cap,0);
		memset(cap.form, 0x20, sizeof(cap.form));
		if (Startup.menu_type == 0) cap.input_x = 0;
		else cap.input_x = 2;
		cap.form[cap.input_x] = 0;
#endif
		cap.input_length = BARCODE_FORMAT_LEN;
		cap.input_dlength = BARCODE_FORMAT_LEN;
		cap.input_itype	= 'S';
		caption_adjust(&cap,NULL);
		strcpy((char*)form_string, "INPUVYZWQDGKTABLCFROS9%XYZEHMJjrvyekm");
#ifdef USE_ONLY_RSS_EXPAND_BARCODE
		strcat((char*)form_string, "q");
#endif
#ifdef USE_BARCODE_DISCOUNT_RATE
		strcat((char*)form_string, "d");
#endif
#ifdef USE_CHINA_CARRFOUR_DEFAULT
		strcat((char*)form_string, "f");
#endif
#ifdef USE_TRACE_STANDALONE
		strcat((char*)form_string, "suc");	// 도축장,용도,GTIN-14 check
#ifdef USE_TRACE_MEATPART
		strcat((char*)form_string, "p");	// 부위
#endif
#ifdef USE_TRACE_MEATGRADE
		strcat((char*)form_string, "g");	// 축산등급
#endif
#endif
#if defined(USE_DIGIT4_PRODUCT_SYMBOL) ||  defined(USE_RUSSIA_BARCODE_FUNCTION) 
		strcat((char*)form_string, "o"); //20150427
#endif
#ifdef USE_RUSSIA_BARCODE_FUNCTION
		strcat((char*)form_string, "w");
		strcat((char*)form_string, "t");
#endif
#ifdef USE_LOTTEMART_ITEMCODE13
strcat((char*)form_string, "i");
#endif
#ifdef USE_LOTTEMART_BARCODE_FUNCTION
strcat((char*)form_string, "x");
#endif
#ifdef USE_HYUNDAI_SET_FUNCTION
		strcat((char*)form_string, "h");
#endif
#ifdef USE_SETPRODUCT_SALE
strcat((char*)form_string, "n");
#endif
#ifdef USE_LOGISTICS_BARCODE
		strcat((char*)form_string, "()");	// AI seletor
		strcat((char*)form_string, "a");	// GTIN CODE
		strcat((char*)form_string, "m");	// Conataind UNIT
		strcat((char*)form_string, "l");	// Lot Number
		strcat((char*)form_string, "z");	// Serial Number
#endif

		Menu_InputCreate(M_ID_BAR_FORM_STR, (HUGEDATA char *)cap.form, MENU_TYPE_BAR, cap.input_x, cap.input_dlength, cap.input_length,
							0, 0, 0, NULL, bar_form, form_string, NULL);
		//Menu_SetInhibit(5, ON);
		Menu_SetGeneralFlag(M_ID_BAR_FORM_STR, barcode_str.barcode_type);
		Menu_SetProperties(M_ID_BAR_FORM_STR, MENU_P_ENGLISH_MODE);
						
		result = Menu_InputDisplay();
		if (result == MENU_RET_SAVE) 
		{
			barcode_str.barcode_num = cur_id;
			memcpy(barcode_str.barcode_description, description, sizeof(barcode_str.barcode_description));
			barcode_str.barcode_type = type_no;
			barcode_str.barcode_format_num = form_no;
			memcpy(barcode_str.barcode_format, bar_form, sizeof(barcode_str.barcode_format));

			flash_addr = start_addr + (BARCODE_STRUCT_SIZE * (cur_id - 1));
			Flash_swrite(flash_addr, (HUGEDATA INT8U *)&barcode_str, sizeof(barcode_str));
#ifdef USE_QR_ADDTOTAL
			flag_tmp = get_global_bparam(GLOBAL_SALE_SETUP_FLAG14);
			flag_tmp = OnOff(flag_tmp & 0x08);	//param 726
			if (flag_tmp)
			{
				ttlbarcode_type = get_barcodetype(GLOBAL_BARCODE_TOTAL_ID);
				bartype = barcodetype_codevalue[ttlbarcode_type];
				if (bartype == QRCODE) // Add up Total barcode type이 QR이면 QrAddTotalFlag On
					QrAddTotalFlag = 1;
				else
					QrAddTotalFlag = 0;
			}
#endif
		}
		else if (result == MENU_RET_ESC)
		{
			break;
		}
	}
}


void    barcodefile_delete(void)		// command code  0x1273
{
	// <CODE> JHC, 040414, -change code
	// [
	menu1_delete(0x1263, 85, FLASH_BARCODE_FORMAT_AREA, BARCODE_STRUCT_SIZE);
	// ]
}

#ifndef DISABLE_LIST_MENU
void barcodefile_list(void)			// command code  0x1272
{		
	CAPTION_STRUCT 	cap;
	POINT 		disp_p;
	BARCODE_STRUCT	barcode_str;
	
//	INT8U	y_gab=8;				// 폰트의 y 크기
//	INT8U	x_gab=6;				// 폰트의 x 크기

	char	string_buf[100];
	long 	ret_long, i;
	INT16S	start_num, end_num, cur_id, lcd_size, reverse_num;
	INT16S	j = 0, result, min_num, max_num, str_size;
	INT16S	line_ct, start_id;

	INT16U	read_code;
	INT32U	addr, addr_flash, cap_addr;	

	// <CODE> JHC, 040414, -change code
	// [
	INT16U	code = 0x1262, cap_code = 0x1E00;
	str_size = BARCODE_STRUCT_SIZE;	

LIST_START:
	caption_split_by_code( 85,&cap,0);
	min_num	= cap.input_min;					//
	max_num = cap.input_max; 		
	if(min_num <= 0)  {
		disp_p = point(0,0);
		message_error(disp_p);
	}

	Dsp_Fill_Buffer(0);
	//display_title_page(code);
	display_title_page_change(code,1,1);
			
	addr = caption_search_by_code(cap_code);		
	for(i = 0; i<3; i++)  {
		cap_addr = addr + CAP_MSG_LENGTH * i;
		//memcpy((INT8U*)&cap,(INT8U *)cap_addr, sizeof(cap));	  
		Flash_sread(cap_addr, (INT8U*)&cap, sizeof(cap));
		if(i != 2) caption_adjust(&cap,NULL);
		disp_p = point((i+Startup.menu_type)*Startup.menu_ygap,0);
#ifdef HEBREW_FONT
		Convert_HebrewPreview((INT8U *)cap.form, strlen(cap.form), 0, 0); 
		hebrew_codetemp[strlen(cap.form)] = 0;
		disp_p.x = 202 - disp_p.x;
		Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
		Convert_ArabPreview((INT8U *)cap.form, strlen((char *)cap.form), 0, 0, 0); 
		arab_codetemp[strlen((char *)cap.form)] = 0;
		disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
		Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
		Dsp_Write_String(disp_p,(HUGEDATA char*)cap.form);
#endif
	}
	Dsp_Diffuse();

	start_num = min_num;
	end_num   = max_num;
	
	for(i=0; i<2; i++)  {
REN1:
		if(i == 0)	ret_long = start_num;		
		if(i == 1)	ret_long = end_num;

		cap_addr = addr + CAP_MSG_LENGTH * i;
		//memcpy((INT8U*)&cap,(INT8U *)cap_addr,sizeof(cap));	  
		Flash_sread(cap_addr, (INT8U*)&cap, sizeof(cap));
		caption_adjust(&cap,NULL);
		disp_p = point((i+Startup.menu_type)*Startup.menu_ygap,0);
#ifdef HEBREW_FONT
		Convert_HebrewPreview((INT8U *)cap.form, strlen(cap.form), 0, 0); 
		hebrew_codetemp[strlen(cap.form)] = 0;
		disp_p.x = 202 - disp_p.x;
		Dsp_Write_1stringLong(disp_p, hebrew_codetemp);

		result=keyin_ulong(KI_GRAPHIC, KI_FORM_ILNN,	0,(long*)&ret_long, 4, 
								(i+Startup.menu_type)*Startup.menu_ygap,  cap.input_x*Startup.menu_xgap, 0,KI_NOPASS );
#elif defined(USE_ARAB_CONVERT)
		Convert_ArabPreview((INT8U *)cap.form, strlen((char *)cap.form), 0, 0, 0); 
		arab_codetemp[strlen((char *)cap.form)] = 0;
		disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
		Dsp_Write_1stringLong(disp_p, arab_codetemp);

		result=keyin_ulong(KI_GRAPHIC, KI_FORM_ILNN,	0,(long*)&ret_long, 4, 
							(i+Startup.menu_type)*Startup.menu_ygap,  cap.input_x*Startup.menu_xgap, 0,KI_NOPASS );
#else
		Dsp_Write_String(disp_p,(HUGEDATA char*)cap.form);

		result=keyin_ulong(KI_GRAPHIC, KI_FORM_NORMAL,	0,(long*)&ret_long, 4, 
								(i+Startup.menu_type)*Startup.menu_ygap,  cap.input_x*Startup.menu_xgap, 0,KI_NOPASS );
#endif
		switch (result){
						 
			case KP_ARROW_UP:  // <CODE> JHC, 040531, Modify
					   // [
					   if(i > 0)	i--;
					   else		BuzOn(2);
					   // ]
					   goto REN1;    					   
			// <CODE> JHC, 040531, Comments
			// [
			// case KP_PAGE_UP:
			// ]
			case KP_ESC:
				   	goto RET_ESC;

			case KP_SAVE:	   start_num = min_num;
					   end_num = max_num;
					   i++;
					   break;
			case KP_PAGE_DN:
			case KP_ARROW_DN:
			case KP_ENTER:	   
					   if ( ret_long > max_num || ret_long < min_num ) {
						// <CODE> JHC, 040607, Modify
						// [
						error_display_form1(0, min_num, max_num);
						// ]
						goto REN1;	
				           }
					   if(i == 0)	start_num = ret_long;
					   if(i == 1)	end_num = ret_long;
					   break;
					   				  				
		     	default:	BuzOn(2);	goto REN1;		     	 	
		} //switch (result)
	}//for(i=1; i<3; i++)

	lcd_size   = Dsp_GetChars();
	line_ct    = Startup.line_max; // (Startup.menu_type?3:2);
	addr_flash = DFLASH_BASE + FLASH_BARCODE_FORMAT_AREA;
	cur_id     = start_num;
	start_id   = start_num;

	// <DEBG> JHC, 040329, New, -DebugCode
	// [
	// $1: JHC, 040529, Comment, -Not use
	// display_ulong(1, start_num, 0);
	// VFD7_Diffuse();
	// ]

READ_START:
	// <CODE> JHC, 040531, Modify
	// [
	// $MEMO: -modify display list all
	Dsp_Fill_Buffer(0);
	if (Startup.menu_type)
	{
//		caption_split_by_code(0x1269, &cap,0);
		caption_split_by_code(0x71FF, &cap,0);
//		disp_p = point(0,0);
		disp_p = point((Startup.menu_type-1) * Startup.menu_ygap, 0); //se-hyung 20071001     no. reference(descript.) 한줄 띄우기
#ifdef HEBREW_FONT
		Convert_HebrewPreview((INT8U *)cap.form, strlen(cap.form), 0, 0); 
		hebrew_codetemp[strlen(cap.form)] = 0;
		disp_p.x = 202 - disp_p.x;
		Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
		Convert_ArabPreview((INT8U *)cap.form, strlen((char *)cap.form), 0, 0, 0); 
		arab_codetemp[strlen((char *)cap.form)] = 0;
		disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
		Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
		Dsp_Write_String(disp_p,(HUGEDATA char*)cap.form);
#endif
	}

	j = 0;
	while (cur_id <= end_num)
	{
		addr =  addr_flash + (long)str_size * (cur_id-1);			
		read_code = Flash_wread(addr);
		if(read_code == cur_id /*>= min_num && read_code <= max_num*/)  
		{	
			j++;
			Flash_sread(addr, (HUGEDATA INT8U*)&barcode_str, str_size ); 
			barcode_str.barcode_format[BARCODE_FORMAT_LEN-1] = 0;
			barcode_str.barcode_description[BARCODE_DESCRIPT_LEN-1] = 0;

		 	sprintf(string_buf, 
		 		"%3d %2d %13s %s", 
			        barcode_str.barcode_num, 
			        barcode_str.barcode_type, 
			        barcode_str.barcode_format, 
			        barcode_str.barcode_description);			
#ifdef USE_CL5200_DISPLAY_DEFAULT
			if(strlen((char const *)barcode_str.barcode_format) < 13)//2byte 폰트를 사용할 경우 12자리 바코드이면 바코드 설명 부분 디스플레이 버그 현상 수정 
				string_buf[lcd_size] = 0;
			else
				string_buf[lcd_size+1] = 0;
#else
			string_buf[lcd_size] = 0;	       		
#endif
			disp_p = point(((j-1)+Startup.menu_type)*Startup.menu_ygap, 0);	       		 
#ifdef HEBREW_FONT
			Convert_HebrewPreview((INT8U *)barcode_str.barcode_description, strlen((char const *)barcode_str.barcode_description), 0, 0);
			hebrew_codetemp[strlen((char const *)barcode_str.barcode_description)] = 0;
			disp_p.x = 202 - disp_p.x;
			Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_FONT)	//기존 아랍ROM 적용되어 있음
 #ifdef USE_ENG_BASED_ARAB_SUPPORT
		   	Convert_ArabPreview((INT8U *)barcode_str.barcode_description, strlen((char const *)barcode_str.barcode_description), 0, 0, 1); 
		   	eng_codetemp[strlen((char const *)barcode_str.barcode_description)] = 0;
			Dsp_Write_1stringLong(disp_p, eng_codetemp);
 #else
			Convert_ArabPreview((INT8U *)barcode_str.barcode_description, strlen((char const *)barcode_str.barcode_description), 0, 0, 0); 
			arab_codetemp[strlen((char const *)barcode_str.barcode_description)] = 0;
			disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
			Dsp_Write_1stringLong(disp_p, arab_codetemp);
 #endif
#else
			Dsp_Write_String(disp_p, (HUGEDATA char *)string_buf);
#endif
			if (j == 1)
				start_id = cur_id;	
			
			if (j == line_ct)
				break;
		}
		
		cur_id++;	
	}
	Dsp_Diffuse();
	// <CODE> JHC, 040531, ADD
	// [
	// 
	if (cur_id >= end_num && j == 0) 
	{
		error_display_caption(0xE00C);
		goto  LIST_START;
	}
	// ]
	cur_id = start_id;
	
KEY_INPUT:
	while(!KEY_Read());
	BuzOn(1);
	
	switch(KeyDrv.CnvCode) 
	{
	case KP_ESC:
		goto RET_ESC;
	case KP_ARROW_UP:
		while(1)  
		{
			if(cur_id <= start_num) 
			{
				BuzOn(2);
				cur_id = start_num;
				goto KEY_INPUT;
			}
			cur_id--;
			addr =  addr_flash + (long)str_size * (cur_id-1);			
			read_code = Flash_wread(addr);
			if(read_code == cur_id /*>= min_num && read_code <= max_num*/ )  
				goto READ_START;
		}
	case KP_PAGE_UP :
		reverse_num = 0;
		while(1)  
		{
			if(cur_id <= start_num) 
			{
				BuzOn(2);
				cur_id = start_num;
				goto READ_START;
			}
			cur_id--;
			addr =  addr_flash + (long)str_size * (cur_id-1);			
			read_code = Flash_wread(addr);
			if(read_code == cur_id /*>= min_num && read_code <= max_num*/ )  
			{
			       	reverse_num++;
				if(reverse_num == line_ct) 
					goto READ_START;
			}
		}
	case KP_PAGE_DN :
		reverse_num = 0;
		while(1)  
		{
			if(cur_id >= end_num) 
			{
				BuzOn(2);
				error_display_caption(0xE00E);
				if (reverse_num < line_ct)
					cur_id = start_id;
				else
					cur_id = end_num;
				goto READ_START;
			}
			else
				cur_id++;
			addr =  addr_flash + (long)str_size * (cur_id-1);			
			read_code = Flash_wread(addr);
			if(read_code == cur_id /*>= min_num && read_code <= max_num*/ )  
			{
			       	reverse_num++;
				if(reverse_num == line_ct) 
					goto READ_START;
			}
		}
	case KP_ENTER:		
	case KP_ARROW_DN:
		reverse_num = cur_id;
		while(reverse_num <= end_num)  
		{
			reverse_num++;
			addr =  addr_flash + (long)str_size * (reverse_num-1);			
			read_code = Flash_wread(addr);
			if(read_code == reverse_num /*&& (read_code >= min_num && read_code <= max_num)*/ )  
			{
			       	cur_id = reverse_num;
				goto READ_START;
			}
		}
		BuzOn(2);
		error_display_caption(0xE00E);
		goto KEY_INPUT;
	case KP_TEST:
		barcodefile_print_list(start_num, end_num);
		goto KEY_INPUT;		
	case KP_FEED:
		keyapp_feed(OFF);
		goto KEY_INPUT;
	default :	
		BuzOn(2);
		goto KEY_INPUT;
	} //switch(KeyDrv.CnvCode)						
	// ]
	
	RET_ESC:    return;
}
#endif //#ifndef DISABLE_LIST_MENU

void barcode_selecttype(long num)
{
	CAPTION_STRUCT 	cap;	
 	POINT disp_p;
	char	buffer[MAX_MENU_CHAR_BUF];
	INT16U  w;

	disp_p = point(CallbackEditPosition_y*Startup.menu_ygap, CallbackEditPosition_x*Startup.menu_xgap);
	w = Dsp_GetChars();
	w -= CallbackEditPosition_x;
	memset((INT8U *)buffer,0x20, w);
	buffer[w] = 0;
#ifdef HEBREW_FONT
	Convert_HebrewPreview((INT8U *)buffer, strlen(buffer), 0, 0); 
	hebrew_codetemp[strlen(buffer)] = 0;
	disp_p.x = 202 - disp_p.x;
	Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
	Convert_ArabPreview((INT8U *)buffer, strlen(buffer), 0, 0, 0); 
	arab_codetemp[strlen(buffer)] = 0;
	disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
	Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
	Dsp_Write_String(disp_p,(HUGEDATA char *)buffer);
#endif

	caption_split_by_code(0x126C, &cap, 0);
#if defined(USE_RSS_BARCODE)
	cap.input_max = 14;
#elif defined(USE_ONLY_RSS_EXPAND_BARCODE)
  #ifdef USE_QR_BARCODE
  	cap.input_max = 16;
  #else
	cap.input_max = 15;
  #endif
#else
#ifdef USE_QR_BARCODE
	cap.input_max = 16;
#else
	cap.input_max = 10;
#endif
#endif
	caption_adjust(&cap,NULL);
	if(num >= cap.input_min  && num <= cap.input_max && barcodetype_codevalue[num] != NO_BARCODE) {
		strcpy((char *)buffer,(char *)barcodetype[num]);
		buffer[15] = '\0';
#ifdef HEBREW_FONT
		Convert_HebrewPreview((INT8U *)buffer, strlen(buffer), 0, 0); 
		hebrew_codetemp[strlen(buffer)] = 0;
		Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
		Convert_ArabPreview((INT8U *)buffer, strlen(buffer), 0, 0, 0); 
		arab_codetemp[strlen(buffer)] = 0;
		Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
		Dsp_Write_String(disp_p,(HUGEDATA char *)buffer);		
#endif
	} else {
#ifdef HEBREW_FONT
		Convert_HebrewPreview((INT8U *)&global_message[MSG_INVALID_VALUE], strlen(global_message[MSG_INVALID_VALUE]), 0, 0); 
		hebrew_codetemp[strlen(global_message[MSG_INVALID_VALUE])] = 0;
		Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
		Convert_ArabPreview((INT8U *)&global_message[MSG_INVALID_VALUE], strlen(global_message[MSG_INVALID_VALUE]), 0, 0, 0); 
		arab_codetemp[strlen(global_message[MSG_INVALID_VALUE])] = 0;
		Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
		Dsp_Write_String(disp_p,(HUGEDATA char *)global_message[MSG_INVALID_VALUE]);
#endif
	}

}


void barcode_selectformat(long num)
{
	CAPTION_STRUCT 	cap;	
 	POINT 		disp_p;
	INT16U w;
	char	buffer[BARCODE_FORMAT_LEN];
	INT8U id_bar_type;

	//if (Startup.menu_type==0) return;
	id_bar_type = Menu_GetMenuId(M_ID_BAR_TYPE);
	if (id_bar_type == 0xff) return;
	
	disp_p = point(CallbackEditPosition_y*Startup.menu_ygap, CallbackEditPosition_x*Startup.menu_xgap);
	w = Dsp_GetChars();
	w -= CallbackEditPosition_x;
	memset((INT8U *)buffer,0x20, w);
	buffer[w] = 0;
#ifdef HEBREW_FONT
	Convert_HebrewPreview((INT8U *)buffer, strlen(buffer), 0, 0); 
	hebrew_codetemp[strlen(buffer)] = 0;
	disp_p.x = 202 - disp_p.x;
	Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
	Convert_ArabPreview((INT8U *)buffer, strlen(buffer), 0, 0, 0); 
	arab_codetemp[strlen(buffer)] = 0;
	disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
	Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
	Dsp_Write_String(disp_p,(HUGEDATA char *)buffer);	
#endif
	if (barcode_make_format(num, *Menu[id_bar_type].pval, (HUGEDATA INT8U*)buffer))
	{
#ifdef HEBREW_FONT
		Convert_HebrewPreview((INT8U *)buffer, strlen(buffer), 0, 0); 
		hebrew_codetemp[strlen(buffer)] = 0;
		Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
		Convert_ArabPreview((INT8U *)buffer, strlen(buffer), 0, 0, 0); 
		arab_codetemp[strlen(buffer)] = 0;
		Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
		Dsp_Write_String(disp_p,(HUGEDATA char *)buffer);
#endif

	} // if(num >= 1 && num <= 5)
	else {
		if(num == 0)  {
			caption_split_by_code(0x126E,&cap,0);
 		} else {
			strcpy(cap.form,global_message[MSG_INVALID_VALUE]);
		}
#ifdef HEBREW_FONT
		Convert_HebrewPreview((INT8U *)cap.form, strlen(cap.form), 0, 0); 
		hebrew_codetemp[strlen(cap.form)] = 0;
		Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
		Convert_ArabPreview((INT8U *)cap.form, strlen((char *)cap.form), 0, 0, 0); 
		arab_codetemp[strlen((char *)cap.form)] = 0;
		Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
		Dsp_Write_String(disp_p,(HUGEDATA char *)cap.form);
#endif
	}

}

INT8U barcode_form_len(char *str)
{
	INT8U cnt, i;

	cnt = 0;
	for (i = 0; i < 50; i++) {
		if(str[i] == 0) break;
		else if(str[i] == '%') {
			i++;
		}
		cnt++;
	}
	return cnt;
}


extern INT8U check_barcode_str(char *str, char *form_str);

INT8U barcode_make_format(INT16S num, INT8U type, HUGEDATA INT8U *buffer)
{

	if(num < 1 || num > 4) return FALSE;
	num--;

	memset(buffer, 0x20, BARCODE_FORMAT_LEN);
	buffer[BARCODE_FORMAT_LEN-1] = 0;

	switch(type) {
		case BARCODE_TYPE_UPC13://77:
			strcpy((char *)buffer,(char *)UPC13_form[num]);
			if(Startup.country == COUNTRY_US)
			{
				buffer[0]='0'; buffer[1]='2';
			}				
			break;
#ifdef USE_UPCA
		case BARCODE_TYPE_UPCA://54:
			strcpy((char *)buffer,(char *)UPC12_form[num]);
			if(Startup.country == COUNTRY_US)
			{
//				buffer[0]='0'; buffer[1]='2';
				buffer[0]='2';
			}				
			break;
#endif            
#ifdef USE_EAN8            
		case BARCODE_TYPE_EAN8://51:
			strcpy((char *)buffer,(char *)EAN8_form[num]);
			break;
#endif            
		case BARCODE_TYPE_EAN13://48:
			strcpy((char *)buffer,(char *)EAN13_form[num]);
			break;
#ifdef USE_EAN13SUP5            
		case BARCODE_TYPE_EAN13A5://50:
			strcpy((char *)buffer,(char *)EAN13A5_form[num]);
			break;
#endif
#ifdef USE_I2OF5            
		case BARCODE_TYPE_I2OF5://70:
			strcpy((char *)buffer,(char *)I2OF5_form[num]);
			break;
#endif
#ifdef USE_CODE39            
		case BARCODE_TYPE_CODE39://68:
			strcpy((char *)buffer,(char *)CODE39_form[num]);
			break;
#endif
#ifdef USE_CODE93            
		case BARCODE_TYPE_CODE93://65:
			strcpy((char *)buffer,(char *)CODE93_form[num]);
			break;
#endif
#ifdef USE_CODE128C            
		case BARCODE_TYPE_CODE128://61:
			strcpy((char *)buffer,(char *)CODE128_form[num]);
			break;
#endif
#ifdef USE_CODABAR            
		case BARCODE_TYPE_CODABAR://74:
			strcpy((char *)buffer,(char *)CODABAR_form[num]);
			break;
#endif            
#ifdef USE_RSS_BARCODE
		case BARCODE_TYPE_RSS14:
			strcpy((char *)buffer,(char *)RSS14_default_form[num]);
			break;
		case BARCODE_TYPE_RSS14S:
			strcpy((char *)buffer,(char *)RSS14_default_form[num]);
			break;
		case BARCODE_TYPE_RSS14SO:
			strcpy((char *)buffer,(char *)RSS14_default_form[num]);
			break;
		case BARCODE_TYPE_RSSLIM:
			strcpy((char *)buffer,(char *)RSS14_default_form[num]);
			break;
#endif//#ifdef USE_RSS_BARCODE
#ifdef USE_ONLY_RSS_EXPAND_BARCODE
		case BARCODE_TYPE_RSSEXP:
			strcpy((char *)buffer,(char *)RSS14_default_form[num]);
			break;
		case BARCODE_TYPE_RSSEXPS:
			strcpy((char *)buffer,(char *)RSS14_default_form[num]);
			break;
#endif//#ifdef USE_ONLY_RSS_EXPAND_BARCODE
#ifdef USE_QR_BARCODE
		case BARCODE_TYPE_QRCODE:
			strcpy((char *)buffer,(char *)QRCODE_form[num]);
			break;
#endif
	}
	return TRUE;
}

#ifdef USE_QR_ADDTOTAL
INT8U QrCfg_OpeningCommand[3];
INT8U QrCfg_PosId[3];
INT8U QrCfg_EanPrefix[3];
INT8U QrCfg_EanSuffix[3];
INT8U QrCfg_ClosingCommand[3];
INT8U QrCfg_DelLastSuffix;
void barcodefile_QR_config_default(void)
{
	INT8U temp_str[3];

	memset(temp_str, 0, sizeof(temp_str));
	set_global_sparam(GLOBAL_QR_CFG_OPEN_CMD, temp_str, 1);  // opening command
	set_global_sparam(GLOBAL_QR_CFG_POS_ID, temp_str, 2); // POS ID
	set_global_sparam(GLOBAL_QR_CFG_EAN_PREFIX, temp_str, 2); // EAN Prefix
	set_global_sparam(GLOBAL_QR_CFG_CLOSE_CMD, temp_str, 1); // closing command
#ifdef COUNTRY_TURKEY
	set_global_bparam(GLOBAL_QR_CFG_DEL_LAST_SUFFIX, 1); // Not Use Last Suffix
#else
	set_global_bparam(GLOBAL_QR_CFG_DEL_LAST_SUFFIX, 0); // Use Last Suffix
#endif
	temp_str[0] = _CR;
	temp_str[1] = LF;
	set_global_sparam(GLOBAL_QR_CFG_EAN_SUFFIX, temp_str, 2); // EAN Suffix
}


#define M_ID_QR_OPENING 1
#define M_ID_QR_POSID 2
#define M_ID_QR_EAN_PREFIX 3
#define M_ID_QR_EAN_SUFFIX 4
#define M_ID_QR_CLOSING 5
#define M_ID_QR_LAST_SUFFIX 6
void barcodefile_QR_cofig(void)
{
	CAPTION_STRUCT 	cap;
	INT8U result;
	
	INT8U opening_str[3];
	INT8U posid_str[5];
	INT8U prefix_str[5];
	INT8U suffix_str[5];
	INT8U closing_str[3];
	INT8U temp_str[3];
	INT32U last_suffix;

	memset(opening_str, 0, sizeof(opening_str));
	memset(posid_str, 0, sizeof(posid_str));
	memset(prefix_str, 0, sizeof(prefix_str));
	memset(suffix_str, 0, sizeof(suffix_str));
	memset(closing_str, 0, sizeof(closing_str));
	last_suffix = 0;
	get_global_sparam(GLOBAL_QR_CFG_OPEN_CMD, temp_str, 1);
	tohex2(temp_str[0], &opening_str[0]);
	get_global_sparam(GLOBAL_QR_CFG_POS_ID, temp_str, 2);
	tohex2(temp_str[0], &posid_str[0]);
	tohex2(temp_str[1], &posid_str[2]);
	get_global_sparam(GLOBAL_QR_CFG_EAN_PREFIX, temp_str, 2);
	tohex2(temp_str[0], &prefix_str[0]);
	tohex2(temp_str[1], &prefix_str[2]);
	get_global_sparam(GLOBAL_QR_CFG_EAN_SUFFIX, temp_str, 2);
	tohex2(temp_str[0], &suffix_str[0]);
	tohex2(temp_str[1], &suffix_str[2]);
	get_global_sparam(GLOBAL_QR_CFG_CLOSE_CMD, temp_str, 1);
	tohex2(temp_str[0], &closing_str[0]);
	last_suffix = (INT32U)get_global_bparam(GLOBAL_QR_CFG_DEL_LAST_SUFFIX);

	Menu_Init();

	caption_split_by_code(0x1264, &cap,0);//""
	Menu_HeadCreate((char *)cap.form);

	caption_split_by_code(0x3f28, &cap,0);//"Opening Command"
	caption_adjust(&cap,NULL);
	Menu_InputCreate(M_ID_QR_OPENING, (HUGEDATA char *)cap.form, MENU_TYPE_STR, cap.input_x, cap.input_dlength, cap.input_length,
						cap.input_max, cap.input_min, 0, NULL, opening_str, NULL, NULL);

	caption_split_by_code(0x3f29, &cap,0);//"POS ID"
	caption_adjust(&cap,NULL);
	Menu_InputCreate(M_ID_QR_POSID, (HUGEDATA char *)cap.form, MENU_TYPE_STR, cap.input_x, cap.input_dlength, cap.input_length,
						cap.input_max, cap.input_min, 0, NULL, posid_str, NULL, NULL);

	caption_split_by_code(0x3f2a, &cap,0);//"EAN Prefix"
	caption_adjust(&cap,NULL);
	Menu_InputCreate(M_ID_QR_EAN_PREFIX, (HUGEDATA char *)cap.form, MENU_TYPE_STR, cap.input_x, cap.input_dlength, cap.input_length,
						cap.input_max, cap.input_min, 0, NULL, prefix_str, NULL, NULL);

	caption_split_by_code(0x3f2b, &cap,0);//"EAN Suffix"
	caption_adjust(&cap,NULL);
	Menu_InputCreate(M_ID_QR_EAN_SUFFIX, (HUGEDATA char *)cap.form, MENU_TYPE_STR, cap.input_x, cap.input_dlength, cap.input_length,
						cap.input_max, cap.input_min, 0, NULL, suffix_str, NULL, NULL);

	caption_split_by_code(0x3f2c, &cap,0);//"Closing Command"
	caption_adjust(&cap,NULL);
	Menu_InputCreate(M_ID_QR_CLOSING, (HUGEDATA char *)cap.form, MENU_TYPE_STR, cap.input_x, cap.input_dlength, cap.input_length,
						cap.input_max, cap.input_min, 0, NULL, closing_str, NULL, NULL);

	caption_split_by_code(0x3f2d, &cap,0);//"Delete Last Suffix"
	caption_adjust(&cap,NULL);
	Menu_InputCreate(M_ID_QR_LAST_SUFFIX, (HUGEDATA char *)cap.form, MENU_TYPE_YN, cap.input_x, cap.input_dlength, cap.input_length,
							cap.input_max, cap.input_min, 0, &last_suffix, NULL, NULL, NULL);

	result = Menu_InputDisplay();
	if (result == MENU_RET_SAVE) 
	{
		temp_str[0] = hexstrtobyte(&opening_str[0]);
		set_global_sparam(GLOBAL_QR_CFG_OPEN_CMD, temp_str, 1);  // opening command
		memcpy(QrCfg_OpeningCommand, temp_str, sizeof(QrCfg_OpeningCommand));
		temp_str[0] = hexstrtobyte(&posid_str[0]);
		temp_str[1] = hexstrtobyte(&posid_str[2]);
		set_global_sparam(GLOBAL_QR_CFG_POS_ID, temp_str, 2); // POS ID
		memcpy(QrCfg_PosId, temp_str, sizeof(QrCfg_PosId));
		temp_str[0] = hexstrtobyte(&prefix_str[0]);
		temp_str[1] = hexstrtobyte(&prefix_str[2]);
		set_global_sparam(GLOBAL_QR_CFG_EAN_PREFIX, temp_str, 2); // EAN Prefix
		memcpy(QrCfg_EanPrefix, temp_str, sizeof(QrCfg_EanPrefix));
		temp_str[0] = hexstrtobyte(&suffix_str[0]);
		temp_str[1] = hexstrtobyte(&suffix_str[2]);
		set_global_sparam(GLOBAL_QR_CFG_EAN_SUFFIX, temp_str, 2); // EAN Suffix
		memcpy(QrCfg_EanSuffix, temp_str, sizeof(QrCfg_EanSuffix));
		temp_str[0] = hexstrtobyte(&closing_str[0]);
		set_global_sparam(GLOBAL_QR_CFG_CLOSE_CMD, temp_str, 1); // closing command
		memcpy(QrCfg_ClosingCommand, temp_str, sizeof(QrCfg_ClosingCommand));
		set_global_bparam(GLOBAL_QR_CFG_DEL_LAST_SUFFIX, (INT8U)last_suffix);
		QrCfg_DelLastSuffix = (INT8U)last_suffix;
	}
}
#endif
