/*****************************************************************************
|*			
|*  Copyright		:	(c) 2002 CAS
|*  Filename		:	key.c  V1.0
|*  Programmer(s)	:	Chung jong kwon (CJK)
|*  Created		:	2003/03/20
|*  Modified		:	2004/08/20
|*  Description		:	CLP Key routine    
|*
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
#include "prt_advertise.h"
#include "adm.h"
#include "main.h"
#include "dsp_app.h"
#include "initial.h"
#include "key.h"
#include "key_function.h"
#include "key_typedef.h"
#include "mode_main.h"
#include "flash_app.h"
#include "mode_commun.h"
#include "ethernet_comm.h"
#include "initial_glb.h"
#include "caption_app.h"
#include "ps2.h"
#include "shopname.h"
#include "mode_sale.h"
#include "dsp_vfd7.h"
#include "mode_sale_display.h"
#include "key_app.h"
#include "main.h"
#include "peripheral/int/plib_int.h"
#include "network_command.h"

extern SCANNER_DATA_STR   Scanner;

//__no_init  volatile HUGEDATA INT8U KeyPort @ (CPLD_ADDR+ 0x0a);			//RD
volatile HUGEDATA INT8U KeyPort;


//KEY_STRKEYDRV KeyDrv;
NEARDATA KEY_STRKEYDRV KeyDrv;//HYP 20060412  _huge --> _near for small code size
KEY_BUF_STRUCT KeyBuf;
KEY_UNIKEYCODE KeyCode;
KEY_BUF_TEMP_STRUCT KeyBufTemp;  //yoo 091230
#ifdef USE_FUNCTION_KEY_BLOCK
KEY_ONOFF_STRUCT KeyOnoff[18];
#endif

/*
INT8U rawkey_cnv_table[] =
{
	0xff, 0x00, 0x10, 0x20, 0x30, 0x40, 0x50,
		  0x01, 0x11, 0x21, 0x31, 0x41, 0x51,		0xff,0xff,0xff,
	0xff, 0x02, 0x12, 0x22, 0x32, 0x42, 0x52,
		  0x03, 0x13, 0x23, 0x33, 0x43, 0x53,		0xff,0xff,0xff,
	0xff, 0x04, 0x14, 0x24, 0x34, 0x44, 0x54,
		  0x05, 0x15, 0x25, 0x35, 0x45, 0x55,		0xff,0xff,0xff,
	0xff, 0x06, 0x16, 0x26, 0x36, 0x46, 0x56,
		  0x07, 0x17, 0x27, 0x37, 0x47, 0x57,		0xff,0xff,0xff,
	0xff, 0x08, 0x18, 0x28, 0x38, 0x48, 0x58,
		  0x09, 0x19, 0x29, 0x39, 0x49, 0x59,		0xff,0xff,0xff,
	0xff, 0x0a, 0x1a, 0x2a, 0x3a, 0x4a, 0x5a,
		  0x0b, 0x1b, 0x2b, 0x3b, 0x4b, 0x5b,		0xff,0xff,0xff,
	0xff, 0x0c, 0x1c, 0x2c, 0x3c, 0x4c, 0x5c,
		  0x0d, 0x1d, 0x2d, 0x3d, 0x4d, 0x5d,		0xff,0xff,0xff,
	0xff, 0x0e, 0x1e, 0x2e, 0x3e, 0x4e, 0x5e,
		  0x0f, 0x1f, 0x2f, 0x3f, 0x4f, 0x5f,		0xff,0xff,0xff
};
*/
INT16U key_pole_map[] =
{
//	0	   	1	     	2	        3  	        4 	       	5  	    	6      		7	       	8  	   	9          	A    	 	B   	     	C	 	 							 D	  								 E    						  F
	0xffff,    	KS_PLU_09,	KS_PLU_10,  	KS_PLU_11, 	KS_PLU_12, 	KS_PLU_13,  	KS_PLU_14, 	KS_PLU_15, 	KS_PLU_07, 	KS_PLU_06, 	KS_PLU_05, 	KS_PLU_04, 	KS_PLU_03,		 KS_PLU_02,		 KS_PLU_01, 	 0xffff,      //	0
	0xffff,	  	KS_PLU_17, 	KS_PLU_18, 	KS_PLU_19,	KS_PLU_20, 	KS_PLU_21, 	KS_PLU_22, 	KS_PLU_23, 	KS_PLU_24, 	KS_PLU_16,	KS_PLU_08, 	0xffff, 	0xffff, 				 0xffff, 				 0xffff, 				 0xffff,      //	1

	0xffff,	   	KS_PLU_25,  	KS_PLU_26,	KS_PLU_27, 	KS_PLU_28, 	KS_PLU_29, 	KS_PLU_30, 	KS_PLU_31      ,KS_PLU_32, 	0xffff, 	0xffff, 	0xffff, 	0xffff, 				 0xffff, 				 0xffff, 				 0xffff,      //	2
	0xffff,	   	KS_PLU_33,  	KS_PLU_34,	KS_PLU_35, 	KS_PLU_36, 	KS_PLU_37, 	KS_PLU_38, 	KS_PLU_39      ,KS_PLU_40, 	0xffff, 	0xffff, 	0xffff, 	0xffff, 				 0xffff, 				 0xffff, 				 0xffff,      //	3
	0xffff,	   	KS_PLU_41, 	KS_PLU_42,	KS_PLU_43, 	KS_PLU_44, 	KS_PLU_45, 	KS_PLU_46, 	KS_PLU_47      ,KS_PLU_48, 	0xffff, 	0xffff, 	0xffff, 	0xffff, 				 0xffff, 				 0xffff, 				 0xffff,      //	4
	0xffff,	   	KS_PLU_49,	KS_PLU_50,	KS_PLU_51, 	KS_PLU_52,	KS_PLU_53,	KS_PLU_54,	KS_PLU_55      ,KS_PLU_56, 	0xffff, 	0xffff, 	0xffff, 	0xffff, 				 0xffff, 				 0xffff, 				 0xffff,      //	5
	0xffff,	   	KS_PLU_57,   	KS_PLU_58,	KS_PLU_59, 	KS_PLU_60, 	KS_PLU_61, 	KS_PLU_62, 	KS_PLU_63      ,KS_PLU_64, 	0xffff, 	0xffff, 	0xffff, 	0xffff, 				 0xffff, 				 0xffff, 				 0xffff,      //	6
	0xffff,	   	KS_PLU_65,	KS_PLU_66,	KS_PLU_67, 	KS_PLU_68, 	KS_PLU_69, 	KS_PLU_70, 	KS_PLU_71      ,KS_PLU_72, 	0xffff, 	0xffff, 	0xffff, 	0xffff, 				 0xffff, 				 0xffff, 				 KEY_IDENTIFY,//	7
};

//Function key 맨 왼쪽열 Define 수정 가능하도록 설정 (Default vaule : KSG_PLUS_149~KSG_PLUS_154)
INT16U key_doublebody_map[] =
{
//	0	   	1	     	2	        3  	        4 	       	5  	    	6      		7	       	8  	   	9          	A    	 	B   	     	C	 	 							 D	  								 E    						  F
	0xffff,    	KS_PLU_02,	KS_PLU_03,  	KS_PLU_04, 	KS_PLU_05, 	KS_PLU_06,  	KS_PLU_07, 	KS_PLU_08, 	KS_PLU_10, 	KS_PLU_19, 	KS_PLU_28, 	KS_PLU_37, 	KS_PLU_46,		 KS_PLU_55,		 KS_PLU_64, 	 0xffff,      //	0
	0xffff,	  	KS_PLU_11, 	KS_PLU_12, 	KS_PLU_13,	KS_PLU_14, 	KS_PLU_15, 	KS_PLU_16, 	KS_PLU_17, 	KS_PLU_18, 	KS_PLU_09,	KS_PLU_01, 	KS_PLU_145, 	KS_PLU_146, 		 KS_PLU_147, 		 KS_PLU_148, 				 0xffff,      //	1

	0xffff,	   	KS_PLU_20,  	KS_PLU_21,	KS_PLU_22, 	KS_PLU_23, 	KS_PLU_24, 	KS_PLU_25, 	KS_PLU_26      ,KS_PLU_27, 	KS_PLU_149, 	0xffff, 	0xffff, 	0xffff, 				 0xffff, 				 0xffff, 				 0xffff,      //	2
	0xffff,	   	KS_PLU_29,  	KS_PLU_30,	KS_PLU_31, 	KS_PLU_32, 	KS_PLU_33, 	KS_PLU_34, 	KS_PLU_35      ,KS_PLU_36, 	KS_PLU_150, 	0xffff, 	0xffff, 	0xffff, 				 0xffff, 				 0xffff, 				 0xffff,      //	3
	0xffff,	   	KS_PLU_38, 	KS_PLU_39,	KS_PLU_40, 	KS_PLU_41, 	KS_PLU_42, 	KS_PLU_43, 	KS_PLU_44      ,KS_PLU_45, 	KS_PLU_151, 	0xffff, 	0xffff, 	0xffff, 				 0xffff, 				 0xffff, 				 0xffff,      //	4
	0xffff,	   	KS_PLU_47,	KS_PLU_48,	KS_PLU_49, 	KS_PLU_50,	KS_PLU_51,	KS_PLU_52,	KS_PLU_53      ,KS_PLU_54, 	KS_PLU_152, 	0xffff, 	0xffff, 	0xffff, 				 0xffff, 				 0xffff, 				 0xffff,      //	5
	0xffff,	   	KS_PLU_56,   	KS_PLU_57,	KS_PLU_58, 	KS_PLU_59, 	KS_PLU_60, 	KS_PLU_61, 	KS_PLU_62      ,KS_PLU_63, 	KS_PLU_153, 	0xffff, 	0xffff, 	0xffff, 				 0xffff, 				 0xffff, 				 0xffff,      //	6
	0xffff,	   	KS_PLU_65,	KS_PLU_66,	KS_PLU_67, 	KS_PLU_68, 	KS_PLU_69, 	KS_PLU_70, 	KS_PLU_71      ,KS_PLU_72, 	KS_PLU_154, 	0xffff, 	0xffff, 	0xffff, 				 0xffff, 				 0xffff, 				 KEY_IDENTIFY,//	7
};


INT16U key_hanging_map[] =
{
//	0	   	1	     	2	        3  	        4 	       	5  	    	6      		7	       	8  	   	9          	A    	 	B   	     	C	 	 							 D	  								 E    						  F
	0xffff,    	KS_PLU_02,	KS_PLU_03,  	KS_PLU_04, 	KS_PLU_05, 	KS_PLU_06,  	KS_PLU_07, 	KS_PLU_08, 	KS_PLU_10, 	KS_PLU_19, 	KS_PLU_28, 	KS_PLU_37, 	KS_PLU_46,		 KS_PLU_55,		 KS_PLU_64, 	 0xffff,      //	0
	0xffff,	  	KS_PLU_11, 	KS_PLU_12, 	KS_PLU_13,	KS_PLU_14, 	KS_PLU_15, 	KS_PLU_16, 	KS_PLU_17, 	KS_PLU_18, 	KS_PLU_09,	KS_PLU_01, 	0xffff, 	0xffff, 				 0xffff, 				 0xffff, 				 0xffff,      //	1

	0xffff,	   	KS_PLU_20,  	KS_PLU_21,	KS_PLU_22, 	KS_PLU_23, 	KS_PLU_24, 	KS_PLU_25, 	KS_PLU_26      ,KS_PLU_27, 	0xffff, 	0xffff, 	0xffff, 	0xffff, 				 0xffff, 				 0xffff, 				 0xffff,      //	2
	0xffff,	   	KS_PLU_29,  	KS_PLU_30,	KS_PLU_31, 	KS_PLU_32, 	KS_PLU_33, 	KS_PLU_34, 	KS_PLU_35      ,KS_PLU_36, 	0xffff, 	0xffff, 	0xffff, 	0xffff, 				 0xffff, 				 0xffff, 				 0xffff,      //	3
	0xffff,	   	KS_PLU_38, 	KS_PLU_39,	KS_PLU_40, 	KS_PLU_41, 	KS_PLU_42, 	KS_PLU_43, 	KS_PLU_44      ,KS_PLU_45, 	0xffff, 	0xffff, 	0xffff, 	0xffff, 				 0xffff, 				 0xffff, 				 0xffff,      //	4
	0xffff,	   	KS_PLU_47,	KS_PLU_48,	KS_PLU_49, 	KS_PLU_50,	KS_PLU_51,	KS_PLU_52,	KS_PLU_53      ,KS_PLU_54, 	0xffff, 	0xffff, 	0xffff, 	0xffff, 				 0xffff, 				 0xffff, 				 0xffff,      //	5
	0xffff,	   	KS_PLU_56,   	KS_PLU_57,	KS_PLU_58, 	KS_PLU_59, 	KS_PLU_60, 	KS_PLU_61, 	KS_PLU_62      ,KS_PLU_63, 	0xffff, 	0xffff, 	0xffff, 	0xffff, 				 0xffff, 				 0xffff, 				 0xffff,      //	6
	0xffff,	   	KS_PLU_65,	KS_PLU_66,	KS_PLU_67, 	KS_PLU_68, 	KS_PLU_69, 	KS_PLU_70, 	KS_PLU_71      ,KS_PLU_72, 	0xffff, 	0xffff, 	0xffff, 	0xffff, 				 0xffff, 				 0xffff, 				 KEY_IDENTIFY,//	7
};

#if defined(USE_CL5200_KEY)
INT16U key_CL5200_bench_map[] =
{
//	0		1		2		3		4		5		6		7		8		9		A		B		C		D		E		F
	0xffff,		KS_PLU_55,	KS_PLU_56,	KS_PLU_57,	KS_PLU_58,	KS_PLU_59,	KS_PLU_60,	KS_PLU_61,	KS_PLU_62,	KS_PLU_01,	KS_PLU_10,	KS_PLU_19,	KS_PLU_28,	KS_PLU_37,	KS_PLU_46,	0xffff,      // 0
	0xffff,		KS_PLU_63,	KS_PLU_64,	KS_PLU_65,	KS_PLU_66,	KS_PLU_67,	KS_PLU_68,	KS_PLU_69,	KS_PLU_70,	KS_PLU_71,	KS_PLU_72,	0xffff, 	0xffff, 	0xffff, 	0xffff, 	0xffff,      // 1
	0xffff,		KS_PLU_02,	KS_PLU_03,	KS_PLU_04,	KS_PLU_05,	KS_PLU_06,	KS_PLU_07,	KS_PLU_08,	KS_PLU_09,	0xffff, 	0xffff, 	0xffff, 	0xffff, 	0xffff, 	0xffff, 	0xffff,      // 2
	0xffff,		KS_PLU_11,	KS_PLU_12,	KS_PLU_13,	KS_PLU_14,	KS_PLU_15,	KS_PLU_16,	KS_PLU_17,	KS_PLU_18,	0xffff, 	0xffff, 	0xffff, 	0xffff, 	0xffff, 	0xffff, 	0xffff,      // 3
	0xffff,		KS_PLU_20,	KS_PLU_21,	KS_PLU_22,	KS_PLU_23,	KS_PLU_24,	KS_PLU_25,	KS_PLU_26,	KS_PLU_27,	0xffff, 	0xffff, 	0xffff, 	0xffff, 	0xffff, 	0xffff, 	0xffff,      // 4
	0xffff,		KS_PLU_29,	KS_PLU_30,	KS_PLU_31,	KS_PLU_32,	KS_PLU_33,	KS_PLU_34,	KS_PLU_35,	KS_PLU_36,	0xffff, 	0xffff, 	0xffff, 	0xffff, 	0xffff, 	0xffff, 	0xffff,      // 5
	0xffff,		KS_PLU_38,	KS_PLU_39,	KS_PLU_40,	KS_PLU_41,	KS_PLU_42,	KS_PLU_43,	KS_PLU_44,	KS_PLU_45,	0xffff, 	0xffff, 	0xffff, 	0xffff, 	0xffff, 	0xffff, 	0xffff,      // 6
	0xffff,		KS_PLU_47,	KS_PLU_48,	KS_PLU_49,	KS_PLU_50,	KS_PLU_51,	KS_PLU_52,	KS_PLU_53,	KS_PLU_54,	0xffff, 	0xffff, 	0xffff, 	0xffff, 	0xffff, 	0xffff, 	KEY_IDENTIFY,// 7

};

INT16U key_CL5200_hanging_map[] = 
{
//	0	   	1	     	2	        3  	        4 	       	5  	    	6      		7	       	8  	   	9          	A    	 	B   	     	C	 	 							 D	  								 E    						  F
	0xffff,    	KS_PLU_09,	KS_PLU_10,  KS_PLU_11, 	KS_PLU_12, 	KS_PLU_13,  KS_PLU_14, 	KS_PLU_15, 	KS_PLU_07, 	KS_PLU_06, 	KS_PLU_05, 	KS_PLU_04, 	KS_PLU_03,  KS_PLU_02,  KS_PLU_01,  0xffff,      //	0
	0xffff,	  	KS_PLU_17, 	KS_PLU_18, 	KS_PLU_19,	KS_PLU_20, 	KS_PLU_21, 	KS_PLU_22, 	KS_PLU_23, 	KS_PLU_24, 	KS_PLU_16,	KS_PLU_08, 	0xffff, 	0xffff,     0xffff,     0xffff,     0xffff,      //	1
	0xffff,	   	KS_PLU_25,  KS_PLU_26,	KS_PLU_27, 	KS_PLU_28, 	KS_PLU_29, 	KS_PLU_30, 	KS_PLU_31  ,KS_PLU_32, 	0xffff,     0xffff,     0xffff,     0xffff,     0xffff,     0xffff,     0xffff,      //	2
	0xffff,	   	KS_PLU_33,  KS_PLU_34,	KS_PLU_35, 	KS_PLU_36, 	KS_PLU_37, 	KS_PLU_38, 	KS_PLU_39  ,KS_PLU_40, 	0xffff,     0xffff,     0xffff,     0xffff,     0xffff,     0xffff,     0xffff,      //	3
	0xffff,	   	KS_PLU_41, 	KS_PLU_42,	KS_PLU_43, 	KS_PLU_44, 	KS_PLU_45, 	KS_PLU_46, 	KS_PLU_47  ,KS_PLU_48, 	0xffff,     0xffff,     0xffff,     0xffff,     0xffff,     0xffff,     0xffff,      //	4
	0xffff,	   	KS_PLU_49,	KS_PLU_50,	KS_PLU_51, 	KS_PLU_52,	KS_PLU_53,	KS_PLU_54,	KS_PLU_55  ,KS_PLU_56, 	0xffff,     0xffff,     0xffff,     0xffff,     0xffff,     0xffff,     0xffff,      //	5
	0xffff,	   	KS_PLU_57,   KS_PLU_58,	KS_PLU_59, 	KS_PLU_60, 	KS_PLU_61, 	KS_PLU_62, 	KS_PLU_63  ,KS_PLU_64, 	0xffff,     0xffff,     0xffff,     0xffff,     0xffff,     0xffff,     0xffff,      //	6
	0xffff,	   	KS_PLU_65,	KS_PLU_66,	KS_PLU_67, 	KS_PLU_68, 	KS_PLU_69, 	KS_PLU_70, 	KS_PLU_71  ,KS_PLU_72, 	0xffff,     0xffff,     0xffff,     0xffff,     0xffff,     0xffff,     KEY_IDENTIFY,//	7
};
#endif

#ifdef _USE_UNDER_SELFKEY_
INT16U key_self_map[] =
{
//	0	   	1	     	2	        3  	        4 	       	5  	    	6      		7	       	8  	   	9          	A    	 	B   	     	C	 	 							 D	  								 E    						  F
	0xffff,    	0xffff,		0xffff,  	0xffff, 	0xffff, 	0xffff,  	0xffff, 	0xffff,		0xffff, 	0xffff, 	0xffff, 	0xffff, 	0xffff, 				 0xffff, 				 0xffff, 				 0xffff,      //0
	0xffff,	  	0xffff,		0xffff, 	0xffff, 	0xffff, 	0xffff, 	0xffff, 	0xffff, 	0xffff, 	0xffff, 	0xffff, 	0xffff, 	0xffff, 				 0xffff, 				 0xffff, 				 0xffff,      //1

	0xffff,	   	KS_PLU_01,  	KS_PLU_01,	KS_PLU_02, 	KS_PLU_02, 	KS_PLU_03, 	KS_PLU_03, 	KS_PLU_04      ,KS_PLU_04, 	KS_PLU_05, 	KS_PLU_05, 	KS_PLU_06, 	KS_PLU_06,			KS_PLU_07, 				KS_PLU_07,     0xffff,      //0
	0xffff,	   	KS_PLU_08,  	KS_PLU_08,	KS_PLU_09, 	KS_PLU_09, 	KS_PLU_10, 	KS_PLU_10, 	KS_PLU_11      ,KS_PLU_11, 	KS_PLU_12, 	KS_PLU_12, 	KS_PLU_13,   	KS_PLU_13	,  KS_PLU_14,					KS_PLU_14, 				0xffff,      //1
	0xffff,	   	KS_PLU_15, 	KS_PLU_15,	KS_PLU_16, 	KS_PLU_16, 	KS_PLU_17, 	KS_PLU_17, 	KS_PLU_18      ,KS_PLU_18, 	KS_PLU_19, 	KS_PLU_19, 	KS_PLU_20, 	KS_PLU_20, 	 KS_PLU_21, 				KS_PLU_21,     0xffff,      //2
	0xffff,	   	KS_PLU_22,	KS_PLU_22,	KS_PLU_23, 	KS_PLU_23,	KS_PLU_24,	KS_PLU_24,	KS_PLU_25      ,KS_PLU_25, 	KS_PLU_26,	KS_PLU_26, 	KS_PLU_27, 	KS_PLU_27, 	 KS_PLU_28, 				KS_PLU_28,     0xffff,      //3
	0xffff,	   	KS_PLU_29,   	KS_PLU_29,	KS_PLU_30, 	KS_PLU_30, 	KS_PLU_31, 	KS_PLU_31, 	KS_PLU_32      ,KS_PLU_32, 	KS_PLU_33,	KS_PLU_33, 	KS_PLU_34, 	KS_PLU_34, 	 KS_PLU_35, 			 KS_PLU_35,     0xffff,      //4
	0xffff,	   	KS_PLU_36,	KS_PLU_36,	KS_PLU_37, 	KS_PLU_37, 	KS_PLU_38, 	KS_PLU_38, 	KS_PLU_39      ,KS_PLU_39, 	KS_PLU_40,	KS_PLU_40,  	KS_PLU_41, 	KS_PLU_41, 		0xffff, 		 				0xffff,        KEY_IDENTIFY //5
};
#endif


//SG060517
ROMDATA INT16U prog_key_code_setA_htype_un[] =									
{
//	0	   	1	     	2	        3  	        4 	       	5  	    	6      		7	       	8
	KP_ASC_A,       KP_ASC_B,       KP_ASC_C,       KP_ASC_D,       KP_ASC_E,       KP_ASC_F,       KP_ASC_G,       KP_ASC_H,       0xffff,  
	KP_ASC_I,	KP_ASC_J,       KP_ASC_K,       KP_ASC_L,       KP_ASC_M,       KP_ASC_N,       KP_ASC_O,       KP_ASC_P,       0xffff,
	KP_ASC_Q,       KP_ASC_R,	KP_ASC_S,       KP_ASC_T,       KP_ASC_U,       KP_ASC_V,       KP_ASC_W,       KP_ASC_X,       0xffff,
	KP_ASC_Y,       KP_ASC_Z,	KP_ASC_24,      KP_ASC_2D,      KP_BS,          KP_ASC_20,      KP_ASC_20,      KP_ASC_LF,      0xffff,
	KP_ASC_2F,      KP_ASC_25,      KP_ASC_28,      KP_ASC_29,      KP_ASC_2E,      KP_ASC_2C,      KP_CHAR_CODE,   KP_CHANGELANG,  0xffff,
	0x03C0,         0x03C8,         0x03CC,         0x03D1,         0x03D2,         0x03D9,         0x03DC,         0xffff,         0xffff,
	0xffff,         0xffff,         0xffff,         0xffff,         0xffff,         0xffff,         0xffff,         0xffff,         0xffff,
	0xffff,         0xffff,         0xffff,         0xffff,         0xffff,         0xffff,         0xffff,         0xffff,         0xffff
};

ROMDATA INT16U prog_key_code_setA_htype_un_shifted[] =
{
//	0	   	1	     	2	        3  	        4 	       	5  	    	6      		7	       	8
	KP_ASC_A_L,     KP_ASC_B_L,     KP_ASC_C_L,     KP_ASC_D_L,     KP_ASC_E_L,     KP_ASC_F_L,     KP_ASC_G_L,     KP_ASC_H_L,     0xffff,  
	KP_ASC_I_L,	KP_ASC_J_L,     KP_ASC_K_L,     KP_ASC_L_L,     KP_ASC_M_L,     KP_ASC_N_L,     KP_ASC_O_L,     KP_ASC_P_L,     0xffff,
	KP_ASC_Q_L,     KP_ASC_R_L,	KP_ASC_S_L,     KP_ASC_T_L,     KP_ASC_U_L,     KP_ASC_V_L,     KP_ASC_W_L,     KP_ASC_X_L,     0xffff,
	KP_ASC_Y_L,     KP_ASC_Z_L,	KP_ASC_2A,      KP_ASC_5F,      KP_BS,          KP_ASC_20,      KP_ASC_20,      0xffff,         0xffff,
	KP_ASC_3F,      KP_ASC_21,      KP_ASC_40,      KP_ASC_23,      KP_ASC_3A,      KP_ASC_3B,      KP_CHAR_CODE,   0xffff,         0xffff,
	0x03E0,         0x03E8,         0x03EC,         0x03F1,         0x03F2,         0x03F9,         0x03FC,         0xffff,         0xffff,
	0xffff,         0xffff,         0xffff,         0xffff,         0xffff,         0xffff,         0xffff,         0xffff,         0xffff,
	0xffff,         0xffff,         0xffff,         0xffff,         0xffff,         0xffff,         0xffff,         0xffff,         0xffff
};

#ifdef USE_COUNTRY_KEY_IN_SOURCE
ROMDATA INT16U prog_key_code_setA_htype_kor[] =									
{
//	0	   	1	     	2	        3  	        4 	       	5  	    	6      		7	       	8
	KP_ASC_A,  	KP_ASC_B,	KP_ASC_C,	KP_ASC_D,	KP_ASC_E,	KP_ASC_F,	KP_ASC_G,	KP_BS,		0xffff, 
	KP_ASC_H,  	KP_ASC_I,	KP_ASC_J,	KP_ASC_K,	KP_ASC_L,	KP_ASC_M,	KP_ASC_N,	KP_ASC_20,	0xffff, 
	KP_ASC_O,  	KP_ASC_P,	KP_ASC_Q,	KP_ASC_R,	KP_ASC_S,	KP_ASC_T,	KP_ASC_U,	KP_ASC_20,	0xffff,	   
	KP_ASC_V, 	KP_ASC_W,	KP_ASC_X,	KP_ASC_Y,	KP_ASC_Z,	0xffff,		0xffff,		KP_ASC_LF,  	0xffff,   
	KP_ASC_2F, 	KP_ASC_25,	KP_ASC_28,	KP_ASC_29,	KP_ASC_2E,	KP_ASC_2C,	KP_CHAR_CODE,	KP_CHANGELANG,	0xffff,	   
	KP_ASC_3A,  	KP_ASC_26,	KP_ASC_7B,	KP_ASC_7D,	KP_ASC_2A,	KP_ASC_2D,	KP_ASC_2B,	KP_CODE_A1C9,	0xffff,	   
	0xffff,         0xffff,         0xffff,         0xffff,         0xffff,         0xffff,         0xffff,         0xffff,         0xffff,
	0xffff,         0xffff,         0xffff,         0xffff,         0xffff,         0xffff,         0xffff,         0xffff,         0xffff
};

ROMDATA INT16U prog_key_code_setA_htype_kor_shifted[] =
{
//	0	   	1	     	2	        3  	        4 	       	5  	    	6      		7	       	8
	KP_ASC_A_L,  	KP_ASC_B_L,	KP_ASC_C_L,	KP_ASC_D_L,	KP_ASC_E_L,	KP_ASC_F_L,	KP_ASC_G_L,	0xffff,		0xffff,  
	KP_ASC_H_L,  	KP_ASC_I_L,	KP_ASC_J_L,	KP_ASC_K_L,	KP_ASC_L_L,	KP_ASC_M_L,	KP_ASC_N_L,	0xffff,		0xffff,
	KP_ASC_O_L,  	KP_ASC_P_L,	KP_ASC_Q_L,	KP_ASC_R_L,	KP_ASC_S_L,	KP_ASC_T_L,	KP_ASC_U_L,	0xffff,		0xffff,
	KP_ASC_V_L, 	KP_ASC_W_L,	KP_ASC_X_L,	KP_ASC_Y_L,	KP_ASC_Z_L,	0xffff,		0xffff,		0xffff,		0xffff,
	KP_ASC_3F, 	KP_ASC_21,	KP_ASC_3C,	KP_ASC_3E,	KP_ASC_22,	KP_ASC_27,	KP_CHAR_CODE,	KP_CHANGELANG,	0xffff,
	KP_ASC_3B, 	KP_ASC_24,	KP_ASC_5B,	KP_ASC_5D,	KP_ASC_40,	KP_ASC_5F,	KP_ASC_3D,	KP_ASC_23,	0xffff,
	0xffff,         0xffff,         0xffff,         0xffff,         0xffff,         0xffff,         0xffff,         0xffff,         0xffff,
	0xffff,         0xffff,         0xffff,         0xffff,         0xffff,         0xffff,         0xffff,         0xffff,         0xffff
};

ROMDATA INT16U prog_key_code_setB_htype_kor[] =									
{
//	0	   	1	     	2	        3  	        4 	       	5  	    	6      		7	       	8
	KP_I_K,  	KP_I_N,		KP_I_D,		KP_I_R,		KP_I_M,		KP_I_B,		KP_I_S,		KP_BS,		0xffff,  
	KP_I_O,  	KP_I_J,		KP_I_CH,	KP_I_C,		KP_I_T,		KP_I_P,		KP_I_H,		KP_ASC_20, 	0xffff,
	KP_N_A,  	KP_N_YA,	KP_N_EO, 	KP_N_YEO,	KP_N_O,		KP_N_YO,	KP_N_U,		KP_ASC_20,	0xffff,
	KP_N_YU, 	KP_N_EU,	KP_N_I,		KP_N_AE,	KP_N_YAE,	KP_N_E,		KP_N_YE,	KP_ASC_LF,	0xffff,
	KP_ASC_2F, 	KP_ASC_25,	KP_ASC_28,	KP_ASC_29,	KP_ASC_2E,	KP_ASC_2C,	KP_CHAR_CODE,	KP_CHANGELANG,	0xffff,
	KP_ASC_3A,  	KP_ASC_26,	KP_ASC_7B,	KP_ASC_7D,	KP_ASC_2A,	KP_ASC_2D,	KP_ASC_2B,	KP_CODE_A1C9,	0xffff,
	0xffff,         0xffff,         0xffff,         0xffff,         0xffff,         0xffff,         0xffff,         0xffff,         0xffff,
	0xffff,         0xffff,         0xffff,         0xffff,         0xffff,         0xffff,         0xffff,         0xffff,         0xffff
};

ROMDATA INT16U prog_key_code_setB_htype_kor_shifted[] =
{
//	0	   	1	     	2	        3  	        4 	       	5  	    	6      		7	       	8
	KP_I_KK,  	KP_I_N,		KP_I_DD,       	KP_I_R,		KP_I_M,		KP_I_BB,	KP_I_SS,	KP_BS,		0xffff,	 
	KP_I_O,  	KP_I_JJ,	KP_I_CH,	KP_I_C,		KP_I_T,		KP_I_P,		KP_I_H,		KP_ASC_20,	0xffff,	
	KP_N_A,  	KP_N_YA,	KP_N_EO, 	KP_N_YEO,	KP_N_O,		KP_N_YO,	KP_N_U,		KP_ASC_20,	0xffff, 
	KP_N_YU, 	KP_N_EU,	KP_N_I,		KP_N_AE,	KP_N_YAE,	KP_N_E,		KP_N_YE,	KP_ASC_LF,	0xffff,	
	KP_ASC_3F, 	KP_ASC_21,	KP_ASC_3C,	KP_ASC_3E,	KP_ASC_22,	KP_ASC_27,	KP_CHAR_CODE,	KP_CHANGELANG,	0xffff,	
	KP_ASC_3B, 	KP_ASC_24,	KP_ASC_5B,	KP_ASC_5D,	KP_ASC_40,	KP_ASC_5F,	KP_ASC_3D,	KP_ASC_23,	0xffff,	
	0xffff,         0xffff,         0xffff,         0xffff,         0xffff,         0xffff,         0xffff,         0xffff,         0xffff,
	0xffff,         0xffff,         0xffff,         0xffff,         0xffff,         0xffff,         0xffff,         0xffff,         0xffff
};
#endif //#ifdef USE_COUNTRY_KEY_IN_SOURCE
////////////////////////////////////////////////////

INT16U normal_key_code_set[128];
INT16U normal_key_code_set_shifted[128];
INT16U prog_key_code_setA[128];
INT16U prog_key_code_setA_shifted[128];
INT16U prog_key_code_setB[128];
INT16U prog_key_code_setB_shifted[128];

//Self - service key
//JJANG 20070516 KS_PLU_41 --> KS_CLEAR
#ifdef _USE_UNDER_SELFKEY_
ROMDATA INT16U normal_self_key_code_set_un[] =	    
{  
//	0	   	1	     	2	        3  	        4 	       	5  	    	6      		7	       	8  	   	9          	A    	 	B   	     	C	 	 							 D	  								 E    						  F
	0xffff,    	0xffff,		0xffff,  	0xffff, 	0xffff, 	0xffff,  	0xffff, 	0xffff,		0xffff, 	0xffff, 	0xffff, 	0xffff, 	0xffff, 				 0xffff, 				 0xffff, 				 0xffff,      //0
	0xffff,	  	0xffff,		0xffff, 	0xffff, 	0xffff, 	0xffff, 	0xffff, 	0xffff, 	0xffff, 	0xffff, 	0xffff, 	0xffff, 	0xffff, 				 0xffff, 				 0xffff, 				 0xffff,      //1

	0xffff,	   	KS_PLU_01,  	KS_PLU_01,	KS_PLU_02, 	KS_PLU_02, 	KS_PLU_03, 	KS_PLU_03, 	KS_PLU_04      ,KS_PLU_04, 	KS_PLU_05, 	KS_PLU_05, 	KS_PLU_06, 	KS_PLU_06,			KS_PLU_07, 				KS_PLU_07,     0xffff,      //0
	0xffff,	   	KS_PLU_08,  	KS_PLU_08,	KS_PLU_09, 	KS_PLU_09, 	KS_PLU_10, 	KS_PLU_10, 	KS_PLU_11      ,KS_PLU_11, 	KS_PLU_12, 	KS_PLU_12, 	KS_PLU_13,   	KS_PLU_13	,  KS_PLU_14,					KS_PLU_14, 				0xffff,      //1
	0xffff,	   	KS_PLU_15, 	KS_PLU_15,	KS_PLU_16, 	KS_PLU_16, 	KS_PLU_17, 	KS_PLU_17, 	KS_PLU_18      ,KS_PLU_18, 	KS_PLU_19, 	KS_PLU_19, 	KS_PLU_20, 	KS_PLU_20, 	 KS_PLU_21, 				KS_PLU_21,     0xffff,      //2
	0xffff,	   	KS_PLU_22,	KS_PLU_22,	KS_PLU_23, 	KS_PLU_23,	KS_PLU_24,	KS_PLU_24,	KS_PLU_25      ,KS_PLU_25, 	KS_PLU_26,	KS_PLU_26, 	KS_PLU_27, 	KS_PLU_27, 	 KS_PLU_28, 				KS_PLU_28,     0xffff,      //3
	0xffff,	   	KS_PLU_29,   	KS_PLU_29,	KS_PLU_30, 	KS_PLU_30, 	KS_PLU_31, 	KS_PLU_31, 	KS_PLU_32      ,KS_PLU_32, 	KS_PLU_33,	KS_PLU_33, 	KS_PLU_34, 	KS_PLU_34, 	 KS_PLU_35, 			 KS_PLU_35,     0xffff,      //4
	0xffff,	   	KS_PLU_36,	KS_PLU_36,	KS_PLU_37, 	KS_PLU_37, 	KS_PLU_38, 	KS_PLU_38, 	KS_PLU_39      ,KS_PLU_39, 	KS_PLU_40,	KS_PLU_40,  	KS_CLEAR, 	KS_CLEAR, 		 KS_PRINT, 		 	 KS_PRINT,     KEY_IDENTIFY //5
};
#else 
ROMDATA INT16U normal_self_key_code_set_un[] =
{
//	0	   	1	     	2	        3  	        4 	       	5  	    	6      		7	       	8  	   	9          	A    	 	B   	     	C	 	 							 D	  								 E    						  F
	0xffff,    	KS_PLU_09,	KS_PLU_10,  	KS_PLU_11, 	KS_PLU_12, 	KS_PLU_13,  	KS_PLU_14, 	KS_PLU_15, 	KS_PLU_07, 	KS_PLU_06, 	KS_PLU_05, 	KS_PLU_04, 	KS_PLU_03,		 KS_PLU_02,		 KS_PLU_01, 	 0xffff,      //	0
	0xffff,	  	KS_PLU_17, 	KS_PLU_18, 	KS_PLU_19,	KS_PLU_20, 	KS_PLU_21, 	KS_PLU_22, 	KS_PLU_23, 	KS_PLU_24, 	KS_PLU_16,	KS_PLU_08, 	0xffff, 	0xffff, 				 0xffff, 				 0xffff, 				 0xffff,      //	1

	0xffff,	   	KS_PLU_25,  	KS_PLU_26,	KS_PLU_27, 	KS_PLU_28, 	KS_PLU_29, 	KS_PLU_30, 	KS_PLU_31      ,KS_PLU_32, 	KS_RETURN, 	KS_ZERO, 	KS_TARE, 	KS_OVERRIDE,	KS_DATETIME, KS_MODE,				 0xffff,      //	2
	0xffff,	   	KS_PLU_33,  	KS_PLU_34,	KS_PLU_35, 	KS_PLU_36, 	KS_PLU_37, 	KS_PLU_38, 	KS_PLU_39      ,KS_PLU_40, 	KS_SALE_PER, 	KS_FOR, 	KS_AUTO,   	KS_PREPACK,  KS_SAVE, 				KS_VOID,				 0xffff,      //	3
	0xffff,	   	KS_PLU_41, 	KS_PLU_42,	KS_PLU_43, 	KS_PLU_44, 	KS_PLU_45, 	KS_PLU_46, 	KS_PLU_47      ,KS_PLU_48, 	KS_MINUS, 	KEY_NUM_7, 	KEY_NUM_8, 	KEY_NUM_9, 	 KS_PLU, 					KS_ADD, 				 0xffff,      //	4
	0xffff,	   	KS_PLU_49,	KS_PLU_50,	KS_PLU_51, 	KS_PLU_52,	KS_PLU_53,	KS_PLU_54,	KS_PLU_55      ,KS_PLU_56, 	KS_TYPE,	KEY_NUM_4, 	KEY_NUM_5, 	KEY_NUM_6, 	 KS_MULTI, 			KS_PAY, 				 0xffff,      //	5
	0xffff,	   	KS_PLU_57,   	KS_PLU_58,	KS_PLU_59, 	KS_PLU_60, 	KS_PLU_61, 	KS_PLU_62, 	KS_PLU_63      ,KS_PLU_64, 	KS_MISC_WEIGHT,	KEY_NUM_1, 	KEY_NUM_2, 	KEY_NUM_3, 	 KS_FEED, 				KS_TTL, 				 0xffff,      //	6
	0xffff,	   	KS_PLU_65,	KS_PLU_66,	KS_PLU_67, 	KS_PLU_68, 	KS_PLU_69, 	KS_PLU_70, 	KS_PLU_71      ,KS_PLU_72, 	KS_MISC_COUNT,	KEY_NUM_10,  	KEY_NUM_0, 	KS_CLEAR, 		 KS_PRINT, 			KS_PRINT, 				 KEY_IDENTIFY,//	7
};
#endif //_USE_UNDER_SELFKEY_

//JJANG 20070801 NEW SELF NUMERIC MAP 
ROMDATA INT16U normal_self_key_numeric_code_set_un[] =
{
//	0	   	1	     	2	        3  	        4 	       	5  	    	6      		7	       	8  	   	9          	A    	 	B   	     	C	 	 							 D	  								 E    						  F
	0xffff,    	0xffff,		0xffff,  	0xffff, 	0xffff, 	0xffff,  	0xffff, 	0xffff,		0xffff, 	0xffff, 	0xffff, 	0xffff, 	0xffff, 				 0xffff, 				 0xffff, 				 0xffff,      //0
	0xffff,	  	0xffff,		0xffff, 	0xffff, 	0xffff, 	0xffff, 	0xffff, 	0xffff, 	0xffff, 	0xffff, 	0xffff, 	0xffff, 	0xffff, 				 0xffff, 				 0xffff, 				 0xffff,      //1

	0xffff,	   	KS_PLU_01,  	KS_PLU_01,	KS_PLU_02, 	KS_PLU_02, 	KS_PLU_03, 	KS_PLU_03, 	KS_PLU_04      ,KS_PLU_04, 	KS_PLU_05, 	KS_PLU_05, 	KS_PLU_06, 	KS_PLU_06,			KS_PLU_07, 				KS_PLU_07,     0xffff,      //0
	0xffff,	   	KS_PLU_08,  	KS_PLU_08,	KS_PLU_09, 	KS_PLU_09, 	KS_PLU_10, 	KS_PLU_10, 	KS_PLU_11      ,KS_PLU_11, 	KS_PLU_12, 	KS_PLU_12, 	KS_PLU_13,   	KS_PLU_13	,  KS_PLU_14,					KS_PLU_14, 				0xffff,      //1
	0xffff,	   	KS_PLU_15, 	KS_PLU_15,	KS_PLU_16, 	KS_PLU_16, 	KS_PLU_17, 	KS_PLU_17, 	KS_PLU_18      ,KS_PLU_18, 	KS_PLU_19, 	KEY_NUM_7, 	KEY_NUM_8, 	KEY_NUM_9, 	 KS_PLU,	 			  	KS_PLU_21,     0xffff,      //2
	0xffff,	   	KS_PLU_22,	KS_PLU_22,	KS_PLU_23, 	KS_PLU_23,	KS_PLU_24,	KS_PLU_24,	KS_PLU_25      ,KS_PLU_25, 	KS_PLU_26,	KEY_NUM_4, 	KEY_NUM_5, 	KEY_NUM_6, 	 KS_PLU_28, 				KS_PLU_28,     0xffff,      //3
	0xffff,	   	KS_PLU_29,   	KS_PLU_29,	KS_PLU_30, 	KS_PLU_30, 	KS_PLU_31, 	KS_PLU_31, 	KS_PLU_32      ,KS_PLU_32, 	KS_PLU_33,	KEY_NUM_1, 	KEY_NUM_2, 	KEY_NUM_3, 	 KS_PLU_35, 			 KS_PLU_35,     0xffff,      //4
	0xffff,	   	KS_PLU_36,	KS_PLU_36,	KS_PLU_37, 	KS_PLU_37, 	KS_PLU_38, 	KS_PLU_38, 	KS_PLU_39      ,KS_PLU_39, 	KS_PLU_40,	KEY_NUM_10,  	KEY_NUM_0, 	KS_CLEAR, 		 KS_PRINT, 		 	 KS_PRINT,     KEY_IDENTIFY //5
};

ROMDATA INT16U normal_key_code_set_un[] =
{
//	0		1		2		3		4		5		6		7		8		9		A		B		C		D		E		F
	0xa100,		KS_PLU_49,	KS_PLU_50,	KS_PLU_51,	KS_PLU_52,	KS_PLU_53,	KS_PLU_54,	KS_PLU_55,	KS_PLU_56,	KS_PLU_65,	KS_PLU_66,	KS_PLU_67,	KS_PLU_68,	KS_PLU_69,	KS_PLU_70,	0xffff,      // 0
	0xffff,		KS_PLU_57,	KS_PLU_58,	KS_PLU_59,	KS_PLU_60,	KS_PLU_61,	KS_PLU_62,	KS_PLU_63,	KS_PLU_64,	KS_PLU_71,	KS_PLU_72,	KS_CLERK_01,	KS_CLERK_02,	KS_CLERK_03,	KS_CLERK_04,	0xffff,      // 1

	0xffff,	   	KS_PLU_01,  	KS_PLU_02,	KS_PLU_03, 	KS_PLU_04, 	KS_PLU_05, 	KS_PLU_06, 	KS_PLU_07      ,KS_PLU_08, 	KS_RETURN, 	KS_ZERO, 	KS_TARE, 	KS_OVERRIDE,	KS_DATETIME, KS_MODE,     0xffff,      //0
	0xffff,	   	KS_PLU_09,  	KS_PLU_10,	KS_PLU_11, 	KS_PLU_12, 	KS_PLU_13, 	KS_PLU_14, 	KS_PLU_15      ,KS_PLU_16, 	KS_SALE_PER, 	KS_FOR, 	KS_AUTO,   	KS_PREPACK,  KS_SAVE, 				KS_VOID,     0xffff,      //1
	0xffff,	   	KS_PLU_17, 	KS_PLU_18,	KS_PLU_19, 	KS_PLU_20, 	KS_PLU_21, 	KS_PLU_22, 	KS_PLU_23      ,KS_PLU_24, 	KS_MINUS, 	KEY_NUM_7, 	KEY_NUM_8, 	KEY_NUM_9, 	 KS_PLU, 					KS_ADD,      0xffff,      //2
	0xffff,	   	KS_PLU_25,	KS_PLU_26,	KS_PLU_27, 	KS_PLU_28,	KS_PLU_29,	KS_PLU_30,	KS_PLU_31      ,KS_PLU_32, 	KS_TYPE,	KEY_NUM_4, 	KEY_NUM_5, 	KEY_NUM_6, 	 KS_MULTI, 			KS_PAY,      0xffff,      //3
	0xffff,	   	KS_PLU_33,   	KS_PLU_34,	KS_PLU_35, 	KS_PLU_36, 	KS_PLU_37, 	KS_PLU_38, 	KS_PLU_39      ,KS_PLU_40, 	KS_MISC_WEIGHT,	KEY_NUM_1, 	KEY_NUM_2, 	KEY_NUM_3, 	 KS_FEED, 				KS_TTL,      0xffff,      //4
	0xffff,	   	KS_PLU_41,	KS_PLU_42,	KS_PLU_43, 	KS_PLU_44, 	KS_PLU_45, 	KS_PLU_46, 	KS_PLU_47      ,KS_PLU_48, 	KS_MISC_COUNT,	KEY_NUM_10,  	KEY_NUM_0, 	KS_CLEAR, 		 KS_PRINT, 			KS_PRINT,    KEY_IDENTIFY //5
};

ROMDATA INT16U normal_key_code_set_un_shifted[] =
{
//	0		1		2		3		4		5		6		7		8		9		A		B		C		D		E		F
	0xa100,		KS_PLU_97,	KS_PLU_98,	KS_PLU_99,	KS_PLU_100,	KS_PLU_101,	KS_PLU_102,	KS_PLU_103,	KS_PLU_104,	KS_PLU_113,	KS_PLU_114,	KS_PLU_115,	KS_PLU_116,	KS_PLU_117,	KS_PLU_118,	0xffff,// 0
	0xffff,		KS_PLU_105,	KS_PLU_106,	KS_PLU_107,	KS_PLU_108,	KS_PLU_109,	KS_PLU_110,	KS_PLU_111,	KS_PLU_112,	KS_PLU_119,	KS_PLU_120,	KS_CLERK_01,	KS_CLERK_02,	KS_CLERK_03,	KS_CLERK_04,	0xffff,// 1

	0xffff,		KS_PLU_49, 	KS_PLU_50,	KS_PLU_51, 	KS_PLU_52, 	KS_PLU_53, 	KS_PLU_54, 	KS_PLU_55,	KS_PLU_56, 	0xffff,		0xffff,0xffff,0xffff,0xffff,0xffff, 0xffff,//2
	0xffff,		KS_PLU_57, 	KS_PLU_58,	KS_PLU_59, 	KS_PLU_60, 	KS_PLU_61, 	KS_PLU_62, 	KS_PLU_63,	KS_PLU_64, 	0xffff,		0xffff,0xffff,0xffff,0xffff,0xffff, 0xffff,//3
	0xffff,		KS_PLU_65, 	KS_PLU_66,	KS_PLU_67, 	KS_PLU_68, 	KS_PLU_69, 	KS_PLU_70, 	KS_PLU_71,	KS_PLU_72, 	0xffff,		0xffff,0xffff,0xffff,0xffff,0xffff, 0xffff,//4
	0xffff,		KS_PLU_73, 	KS_PLU_74,	KS_PLU_75, 	KS_PLU_76, 	KS_PLU_77, 	KS_PLU_78, 	KS_PLU_79,	KS_PLU_80, 	0xffff,		0xffff,0xffff,0xffff,0xffff,0xffff, 0xffff,//5
	0xffff,		KS_PLU_81,	KS_PLU_82,	KS_PLU_83,	KS_PLU_84, 	KS_PLU_85, 	KS_PLU_86,	KS_PLU_87,	KS_PLU_88,	0xffff,		0xffff,0xffff,0xffff,0xffff,0xffff, 0xffff,//6
	0xffff,		KS_PLU_89,	KS_PLU_90,	KS_PLU_91, 	KS_PLU_92, 	KS_PLU_93, 	KS_PLU_94, 	KS_PLU_95,	KS_PLU_96,	0xffff,		0xffff,0xffff,0xffff,0xffff,0xffff, 0xffff //7
	
};

#ifdef USE_COUNTRY_KEY_IN_SOURCE
ROMDATA INT16U normal_key_code_set_kor[] =
{
//	0		1		2		3		4		5		6		7		8		9		A		B		C		D		E		F
	0xa100,		KS_PLU_49,	KS_PLU_50,	KS_PLU_51,	KS_PLU_52,	KS_PLU_53,	KS_PLU_54,	KS_PLU_55,	KS_PLU_56,	KS_PLU_65,	KS_PLU_66,	KS_PLU_67,	KS_PLU_68,	KS_PLU_69,	KS_PLU_70,	0xffff,      // 0
	0xffff,		KS_PLU_57,	KS_PLU_58,	KS_PLU_59,	KS_PLU_60,	KS_PLU_61,	KS_PLU_62,	KS_PLU_63,	KS_PLU_64,	KS_PLU_71,	KS_PLU_72,	KS_CLERK_01,	KS_CLERK_02,	KS_CLERK_03,	KS_CLERK_04,	0xffff,      // 1

	0xffff,	   	KS_PLU_01,  	KS_PLU_02,	KS_PLU_03, 	KS_PLU_04, 	KS_PLU_05, 	KS_PLU_06, 	KS_PLU_07      ,KS_PLU_08, 	KS_RETURN, 	KS_ZERO, 	KS_TARE, 	KS_SAVEPRICE,	KS_DATETIME, KS_MODE,     0xffff,      //0
	0xffff,	   	KS_PLU_09,  	KS_PLU_10,	KS_PLU_11, 	KS_PLU_12, 	KS_PLU_13, 	KS_PLU_14, 	KS_PLU_15      ,KS_PLU_16, 	KS_SALE_PER, 	KS_FIXED, 	KS_AUTO,   	KS_PREPACK,  KS_SAVE, 				KS_VOID,     0xffff,      //1
	0xffff,	   	KS_PLU_17, 	KS_PLU_18,	KS_PLU_19, 	KS_PLU_20, 	KS_PLU_21, 	KS_PLU_22, 	KS_PLU_23      ,KS_PLU_24, 	KS_MINUS, 	KEY_NUM_7, 	KEY_NUM_8, 	KEY_NUM_9, 	 KS_PLU, 					KS_ADD,      0xffff,      //2
	0xffff,	   	KS_PLU_25,	KS_PLU_26,	KS_PLU_27, 	KS_PLU_28,	KS_PLU_29,	KS_PLU_30,	KS_PLU_31      ,KS_PLU_32, 	KS_EDT_ORIGIN, 	KEY_NUM_4, 	KEY_NUM_5, 	KEY_NUM_6, 	 KS_MULTI, 			KS_TTL,      0xffff,      //3
	0xffff,	   	KS_PLU_33,   	KS_PLU_34,	KS_PLU_35, 	KS_PLU_36, 	KS_PLU_37, 	KS_PLU_38, 	KS_PLU_39      ,KS_PLU_40, 	KS_EDT_SELL, 	KEY_NUM_1, 	KEY_NUM_2, 	KEY_NUM_3, 	 KS_FEED, 				KS_PRINTDATE,0xffff,      //4
	0xffff,	   	KS_PLU_41,	KS_PLU_42,	KS_PLU_43, 	KS_PLU_44, 	KS_PLU_45, 	KS_PLU_46, 	KS_PLU_47      ,KS_PLU_48, 	KS_TYPE,	KEY_NUM_10,  	KEY_NUM_0, 	KS_CLEAR, 		 KS_PRINT, 			KS_PRINT,    KEY_IDENTIFY //5
};

//Modified by JJANG 20090220 KG/LB key, Department call key
ROMDATA INT16U normal_key_code_set_us[] =
{
//	0	 	1		2		3		4		5		6		7		8		9		A		B		C		D		E		F
	0xa100,		KS_PLU_49,	KS_PLU_50,	KS_PLU_51,	KS_PLU_52,	KS_PLU_53,	KS_PLU_54,	KS_PLU_55,	KS_PLU_56,	KS_PLU_65,	KS_PLU_66,	KS_PLU_67,	KS_PLU_68,	KS_PLU_69,	KS_PLU_70,	0xffff,      // 0
	0xffff,		KS_PLU_57,	KS_PLU_58,	KS_PLU_59,	KS_PLU_60,	KS_PLU_61,	KS_PLU_62,	KS_PLU_63,	KS_PLU_64,	KS_PLU_71,	KS_PLU_72,	KS_CLERK_01,	KS_CLERK_02,	KS_CLERK_03,	KS_CLERK_04,	0xffff,      // 1

	0xffff,	   	KS_PLU_01,  	KS_PLU_02,	KS_PLU_03, 	KS_PLU_04, 	KS_PLU_05, 	KS_PLU_06, 	KS_PLU_07      ,KS_PLU_08, 	KP_CAPS, 	KS_ZERO, 	KS_TARE, 	KS_OVERRIDE,	KS_DATETIME, KS_MODE,     0xffff,      //0
	0xffff,	   	KS_PLU_09,  	KS_PLU_10,	KS_PLU_11, 	KS_PLU_12, 	KS_PLU_13, 	KS_PLU_14, 	KS_PLU_15      ,KS_PLU_16, 	KS_KGLB, 	KS_FOR, 	KS_AUTO,   	KS_PREPACK,  KS_SAVE, 				KS_VOID,     0xffff,      //1
	0xffff,	   	KS_PLU_17, 	KS_PLU_18,	KS_PLU_19, 	KS_PLU_20, 	KS_PLU_21, 	KS_PLU_22, 	KS_PLU_23      ,KS_PLU_24, 	KS_UNIT3, 	KEY_NUM_7, 	KEY_NUM_8, 	KEY_NUM_9, 	 KS_PLU, 					KS_ADD,      0xffff,      //2
	0xffff,	   	KS_PLU_25,	KS_PLU_26,	KS_PLU_27, 	KS_PLU_28,	KS_PLU_29,	KS_PLU_30,	KS_PLU_31      ,KS_PLU_32, 	KS_UNIT2,	KEY_NUM_4, 	KEY_NUM_5, 	KEY_NUM_6, 	 KS_MULTI, 			KS_PAY,      0xffff,      //3
	0xffff,	   	KS_PLU_33,   	KS_PLU_34,	KS_PLU_35, 	KS_PLU_36, 	KS_PLU_37, 	KS_PLU_38, 	KS_PLU_39      ,KS_PLU_40, 	KS_MISC_WEIGHT,	KEY_NUM_1, 	KEY_NUM_2, 	KEY_NUM_3, 	 KS_FEED, 				KS_TTL,      0xffff,      //4
	0xffff,	   	KS_PLU_41,	KS_PLU_42,	KS_PLU_43, 	KS_PLU_44, 	KS_PLU_45, 	KS_PLU_46, 	KS_PLU_47      ,KS_PLU_48, 	KS_MISC_PCS,	KEY_NUM_10,  	KEY_NUM_0, 	KS_CLEAR, 		 KS_DEPT_CALL, 			KS_PRINT,    KEY_IDENTIFY //5
};

ROMDATA INT16U normal_key_code_set_us_shifted[] =
{	
//	0	 	1		2		3		4		5		6		7		8		9		A		B		C		D		E		F
	0xa100,		KS_PLU_97,	KS_PLU_98,	KS_PLU_99,	KS_PLU_100,	KS_PLU_101,	KS_PLU_102,	KS_PLU_103,	KS_PLU_104,	KS_PLU_113,	KS_PLU_114,	KS_PLU_115,	KS_PLU_116,	KS_PLU_117,	KS_PLU_118,	0xffff,// 0
	0xffff,		KS_PLU_105,	KS_PLU_106,	KS_PLU_107,	KS_PLU_108,	KS_PLU_109,	KS_PLU_110,	KS_PLU_111,	KS_PLU_112,	KS_PLU_119,	KS_PLU_120,	KS_CLERK_01,	KS_CLERK_02,	KS_CLERK_03,	KS_CLERK_04,	0xffff,// 1

	0xffff,		KS_PLU_49, 	KS_PLU_50,	KS_PLU_51, 	KS_PLU_52, 	KS_PLU_53, 	KS_PLU_54, 	KS_PLU_55,	KS_PLU_56, 	0xffff,		0xffff,0xffff,0xffff,0xffff,0xffff, 0xffff,//2
	0xffff,		KS_PLU_57, 	KS_PLU_58,	KS_PLU_59, 	KS_PLU_60, 	KS_PLU_61, 	KS_PLU_62, 	KS_PLU_63,	KS_PLU_64, 	0xffff,		0xffff,0xffff,0xffff,0xffff,0xffff, 0xffff,//3
	0xffff,		KS_PLU_65, 	KS_PLU_66,	KS_PLU_67, 	KS_PLU_68, 	KS_PLU_69, 	KS_PLU_70, 	KS_PLU_71,	KS_PLU_72, 	0xffff,		0xffff,0xffff,0xffff,0xffff,0xffff, 0xffff,//4
	0xffff,		KS_PLU_73, 	KS_PLU_74,	KS_PLU_75, 	KS_PLU_76, 	KS_PLU_77, 	KS_PLU_78, 	KS_PLU_79,	KS_PLU_80, 	0xffff,		0xffff,0xffff,0xffff,0xffff,0xffff, 0xffff,//5
	0xffff,		KS_PLU_81,	KS_PLU_82,	KS_PLU_83,	KS_PLU_84, 	KS_PLU_85, 	KS_PLU_86,	KS_PLU_87,	KS_PLU_88,	0xffff,		0xffff,0xffff,0xffff,0xffff,0xffff, 0xffff,//6
	0xffff,		KS_PLU_89,	KS_PLU_90,	KS_PLU_91, 	KS_PLU_92, 	KS_PLU_93, 	KS_PLU_94, 	KS_PLU_95,	KS_PLU_96,	0xffff,		0xffff,0xffff,0xffff,0xffff,0xffff, 0xffff //7
};

ROMDATA INT16U normal_key_code_set_euro[] =	// EUT용
{
//	0	 	1		2		3		4		5		6		7		8		9		A		B		C		D		E		F
	0xa100,		KS_PLU_49,	KS_PLU_50,	KS_PLU_51,	KS_PLU_52,	KS_PLU_53,	KS_PLU_54,	KS_PLU_55,	KS_PLU_56,	KS_PLU_65,	KS_PLU_66,	KS_PLU_67,	KS_PLU_68,	KS_PLU_69,	KS_PLU_70,	0xffff,      // 0
	0xffff,		KS_PLU_57,	KS_PLU_58,	KS_PLU_59,	KS_PLU_60,	KS_PLU_61,	KS_PLU_62,	KS_PLU_63,	KS_PLU_64,	KS_PLU_71,	KS_PLU_72,	KS_CLERK_01,	KS_CLERK_02,	KS_CLERK_03,	KS_CLERK_04,	0xffff,      // 1

	0xffff,	   	KS_PLU_01,  	KS_PLU_02,	KS_PLU_03, 	KS_PLU_04, 	KS_PLU_05, 	KS_PLU_06, 	KS_PLU_07      ,KS_PLU_08, 	KS_RETURN, 	KS_ZERO, 	KS_TARE, 	KS_OVERRIDE,	KS_DATETIME, KS_MODE,     0xffff,      //0
	0xffff,	   	KS_PLU_09,  	KS_PLU_10,	KS_PLU_11, 	KS_PLU_12, 	KS_PLU_13, 	KS_PLU_14, 	KS_PLU_15      ,KS_PLU_16, 	KS_CLERK_01, 	KS_FOR, 	KS_AUTO,   	KS_PREPACK,  KS_SAVE, 				KS_VOID,     0xffff,      //1
	0xffff,	   	KS_PLU_17, 	KS_PLU_18,	KS_PLU_19, 	KS_PLU_20, 	KS_PLU_21, 	KS_PLU_22, 	KS_PLU_23      ,KS_PLU_24, 	KS_CLERK_02, 	KEY_NUM_7, 	KEY_NUM_8, 	KEY_NUM_9, 	 KS_PLU, 					KS_ADD,      0xffff,      //2
	0xffff,	   	KS_PLU_25,	KS_PLU_26,	KS_PLU_27, 	KS_PLU_28,	KS_PLU_29,	KS_PLU_30,	KS_PLU_31      ,KS_PLU_32, 	KS_CLERK_03,	KEY_NUM_4, 	KEY_NUM_5, 	KEY_NUM_6, 	 KS_MULTI, 			KS_PAY,      0xffff,      //3
	0xffff,	   	KS_PLU_33,   	KS_PLU_34,	KS_PLU_35, 	KS_PLU_36, 	KS_PLU_37, 	KS_PLU_38, 	KS_PLU_39      ,KS_PLU_40, 	KS_CLERK_04, 	KEY_NUM_1, 	KEY_NUM_2, 	KEY_NUM_3, 	 KS_FEED, 				KS_TTL,						0xffff,      //4
	0xffff,	   	KS_PLU_41,	KS_PLU_42,	KS_PLU_43, 	KS_SALE_PER, 	KS_SALE_NEG, 	KS_MISC_WEIGHT,	KS_MISC_COUNT  ,KS_MISC_PCS,	KS_CLERK_CALL,	KEY_NUM_10,  	KEY_NUM_0, 	KS_CLEAR, 		 KS_CASHOPEN, KS_PRINT,    KEY_IDENTIFY //5
};
#endif //#ifdef USE_COUNTRY_KEY_IN_SOURCE

// UN  //////////////////////////////////////////////////
ROMDATA INT16U prog_key_code_setA_un[] =
{
//	0		1		2		3  		4   		5  	   	6  	      	7	      	8  	     	9      	      	A    		B   		C	 			      D	   	      E    	       F
	0xa100,		0xffff,	   	0xffff,		0xffff, 	0xffff, 	0xffff, 	0xffff, 	0xffff, 	0xffff, 	0xffff, 	0xffff, 	0xffff, 	0xffff, 				0xffff, 				0xffff, 	    0xffff, 	//0
	0xffff, 	0xffff,    	0xffff,		0xffff,  	0xffff, 	0xffff, 	0xffff,  	0xffff, 	0xffff,		0xffff, 	0xffff, 	0xffff, 	0xffff, 				0xffff, 				0xffff, 	    0xffff, 	//1
//	0		1		2		3  	    	4   		5  	  	6  	    	7	      	8  	    	9      		A    	B      C      D	     E    	 F
	0xffff,		KP_ASC_A,  	KP_ASC_B,	KP_ASC_C,	KP_ASC_D,	KP_ASC_E,	KP_ASC_F,	KP_ASC_G,	KP_ASC_H,	KEY_SHIFT, 	KP_ZERO,	KP_TARE,  	0xffff,					KP_ARROW_UP,KP_PAGE_UP,	 0xffff, 	//2
	0xffff,		KP_ASC_I,  	KP_ASC_J,	KP_ASC_K,	KP_ASC_L,	KP_ASC_M,	KP_ASC_N,	KP_ASC_O,	KP_ASC_P,	KP_INSERT,	0xffff,		0xffff,		KP_ARROW_LF,KP_SAVE,	   KP_ARROW_RI,	0xffff, 	//3
	0xffff,		KP_ASC_Q,  	KP_ASC_R,	KP_ASC_S,	KP_ASC_T,	KP_ASC_U,	KP_ASC_V,	KP_ASC_W,	KP_ASC_X,	KP_DEL,		KEY_NUM_7,	KEY_NUM_8,	KEY_NUM_9,		KP_ARROW_DN,KP_PAGE_DN,	 0xffff,	 //4
	0xffff,		KP_ASC_Y, 	KP_ASC_Z,	KP_ASC_24,	KP_ASC_2D,	KP_BS,		KP_ASC_20,	KP_ASC_20,	KP_ASC_LF,	KP_CAPS,	KEY_NUM_4,	KEY_NUM_5,	KEY_NUM_6,		KP_ESC,		   KP_TEST,	    0xffff, 	//5
	0xffff,		KP_ASC_2F, 	KP_ASC_25,	KP_ASC_28,	KP_ASC_29,	KP_ASC_2E,	KP_ASC_2C,	KP_CHAR_CODE,	KP_CHANGELANG,	0xffff,		KEY_NUM_1,	KEY_NUM_2,	KEY_NUM_3,		KP_FEED,	   0xffff,	     0xffff, 	//6
	0xffff,		KP_ASC_C0,   	KP_ASC_C8,	KP_ASC_CC,	KP_ASC_D1,	KP_ASC_D2,	KP_ASC_D9,	KP_ASC_DC,	0xffff,		0xffff,		KEY_NUM_10,	KEY_NUM_0,	KP_CLEAR,			KP_ENTER,	  KP_ENTER,	   KEY_IDENTIFY 		//7
};

ROMDATA INT16U prog_key_code_setA_un_shifted[] =
{
//	0		1		2		3  	    	4   		5  	  	6  	    	7	      	8  	    	9      		A    	B      C      D	     E    	 F
	0xa100,		0xffff,	   	0xffff,		0xffff, 	0xffff, 	0xffff, 	0xffff, 	0xffff, 	0xffff, 	0xffff, 	0xffff, 0xffff,0xffff,0xffff,0xffff,0xffff,	 	//0
	0xffff, 	0xffff,    	0xffff,		0xffff,  	0xffff, 	0xffff, 	0xffff,  	0xffff, 	0xffff,		0xffff, 	0xffff, 0xffff,0xffff,0xffff,0xffff,0xffff, 	 //1

	0xffff,		KP_ASC_A_L,  	KP_ASC_B_L,	KP_ASC_C_L,	KP_ASC_D_L,	KP_ASC_E_L,	KP_ASC_F_L,	KP_ASC_G_L,	KP_ASC_H_L,	0xffff,		0xffff,	0xffff,0xffff,0xffff,0xffff,0xffff,	 	//2
	0xffff,		KP_ASC_I_L,  	KP_ASC_J_L,	KP_ASC_K_L,	KP_ASC_L_L,	KP_ASC_M_L,	KP_ASC_N_L,	KP_ASC_O_L,	KP_ASC_P_L,	0xffff,		0xffff,	0xffff,0xffff,0xffff,0xffff,0xffff,	 	//3
	0xffff,		KP_ASC_Q_L,  	KP_ASC_R_L,	KP_ASC_S_L,	KP_ASC_T_L,	KP_ASC_U_L,	KP_ASC_V_L,	KP_ASC_W_L,	KP_ASC_X_L,	0xffff, 	0xffff,	0xffff,0xffff,0xffff,0xffff,0xffff,	 	//4
	0xffff,		KP_ASC_Y_L, 	KP_ASC_Z_L,	KP_ASC_2A,	KP_ASC_5F,	KP_BS,		KP_ASC_20,	KP_ASC_20,	0xffff,		0xffff,		0xffff,	0xffff,0xffff,0xffff,0xffff,0xffff,	 	//5
	0xffff,		KP_ASC_3F, 	KP_ASC_21,	KP_ASC_40,	KP_ASC_23,	KP_ASC_3A,	KP_ASC_3B,	KP_CHAR_CODE,	0xffff,		0xffff,		0xffff,	0xffff,0xffff,0xffff,0xffff,0xffff,	 	//6
	0xffff,		KP_ASC_E0,	KP_ASC_E8,	KP_ASC_EC,	KP_ASC_F1,	KP_ASC_F2,	KP_ASC_F9,	KP_ASC_FC,	0xffff,		0xffff,		0xffff,	0xffff,0xffff,0xffff,0xffff,0xffff 	 	//7
};

ROMDATA INT16U prog_key_code_setB_un[] =
{
//	0		1		2		3  		4   		5  	   	6  	      	7	      	8  	     	9      	      	A    		B   		C	 			      D	   	      E    	       F
	0xa100,		0xffff,	   	0xffff,		0xffff, 	0xffff, 	0xffff, 	0xffff, 	0xffff, 	0xffff, 	0xffff, 	0xffff, 	0xffff, 	0xffff, 				0xffff, 				0xffff, 	    0xffff, 	//0
	0xffff, 	0xffff,    	0xffff,		0xffff,  	0xffff, 	0xffff, 	0xffff,  	0xffff, 	0xffff,		0xffff, 	0xffff, 	0xffff, 	0xffff, 				0xffff, 				0xffff, 	    0xffff, 	//1
	0xffff,		KP_ASC_A,  	KP_ASC_B,	KP_ASC_C,	KP_ASC_D,	KP_ASC_E,	KP_ASC_F,	KP_ASC_G,	KP_ASC_H,	KEY_SHIFT, 	KP_ZERO,	KP_TARE,	0xffff,					KP_ARROW_UP,KP_PAGE_UP,	 0xffff, 	//2
	0xffff,		KP_ASC_I,  	KP_ASC_J,	KP_ASC_K,	KP_ASC_L,	KP_ASC_M,	KP_ASC_N,	KP_ASC_O,	KP_ASC_P,	KP_INSERT,	0xffff,		0xffff,		KP_ARROW_LF,KP_SAVE,	   KP_ARROW_RI,	0xffff, 	//3
	0xffff,		KP_ASC_Q,  	KP_ASC_R,	KP_ASC_S,	KP_ASC_T,	KP_ASC_U,	KP_ASC_V,	KP_ASC_W,	KP_ASC_X,	KP_DEL,		KEY_NUM_7,	KEY_NUM_8,	KEY_NUM_9,		KP_ARROW_DN,KP_PAGE_DN,	 0xffff,	 //4
	0xffff,		KP_ASC_Y, 	KP_ASC_Z,	KP_ASC_24,	KP_ASC_2D,	KP_BS,		KP_ASC_20,	KP_ASC_20,	KP_ASC_LF,	KP_CAPS,	KEY_NUM_4,	KEY_NUM_5,	KEY_NUM_6,		KP_ESC,		   KP_TEST,	    0xffff, 	//5
	0xffff,		KP_ASC_2F, 	KP_ASC_25,	KP_ASC_28,	KP_ASC_29,	KP_ASC_2E,	KP_ASC_2C,	KP_CHAR_CODE,	KP_CHANGELANG,	0xffff,		KEY_NUM_1,	KEY_NUM_2,	KEY_NUM_3,		KP_FEED,	   0xffff,	     0xffff, 	//6
	0xffff,		KP_ASC_C0,   	KP_ASC_C8,	KP_ASC_CC,	KP_ASC_D1,	KP_ASC_D2,	KP_ASC_D9,	KP_ASC_DC,	0xffff,		0xffff,		KEY_NUM_10,	KEY_NUM_0,	KP_CLEAR,			KP_ENTER,	  KP_ENTER,	   KEY_IDENTIFY 		//7
};

ROMDATA INT16U prog_key_code_setB_un_shifted[] =
{
//	0		1		2		3  	    	4   		5  	  	6  	    	7	      	8  	    	9      		A    	B      C      D	     E    	 F
	0xa100,		0xffff,	   	0xffff,		0xffff, 	0xffff, 	0xffff, 	0xffff, 	0xffff, 	0xffff, 	0xffff, 	0xffff, 0xffff,0xffff,0xffff,0xffff,0xffff,	 	//0
	0xffff, 	0xffff,    	0xffff,		0xffff,  	0xffff, 	0xffff, 	0xffff,  	0xffff, 	0xffff,		0xffff, 	0xffff, 0xffff,0xffff,0xffff,0xffff,0xffff, 	 //1
	0xffff,		KP_ASC_A_L,  	KP_ASC_B_L,	KP_ASC_C_L,	KP_ASC_D_L,	KP_ASC_E_L,	KP_ASC_F_L,	KP_ASC_G_L,	KP_ASC_H_L,	0xffff,		0xffff,	0xffff,0xffff,0xffff,0xffff,0xffff,	 	//2
	0xffff,		KP_ASC_I_L,  	KP_ASC_J_L,	KP_ASC_K_L,	KP_ASC_L_L,	KP_ASC_M_L,	KP_ASC_N_L,	KP_ASC_O_L,	KP_ASC_P_L,	0xffff,		0xffff,	0xffff,0xffff,0xffff,0xffff,0xffff,	 	//3
	0xffff,		KP_ASC_Q_L,  	KP_ASC_R_L,	KP_ASC_S_L,	KP_ASC_T_L,	KP_ASC_U_L,	KP_ASC_V_L,	KP_ASC_W_L,	KP_ASC_X_L,	0xffff, 	0xffff,	0xffff,0xffff,0xffff,0xffff,0xffff,	 	//4
	0xffff,		KP_ASC_Y_L, 	KP_ASC_Z_L,	KP_ASC_2A,	KP_ASC_5F,	KP_BS,		KP_ASC_20,	KP_ASC_20,	0xffff,		0xffff,		0xffff,	0xffff,0xffff,0xffff,0xffff,0xffff,	 	//5
	0xffff,		KP_ASC_3F, 	KP_ASC_21,	KP_ASC_40,	KP_ASC_23,	KP_ASC_3A,	KP_ASC_3B,	KP_CHAR_CODE,	0xffff,		0xffff,		0xffff,	0xffff,0xffff,0xffff,0xffff,0xffff,	 	//6
	0xffff,		KP_ASC_E0,	KP_ASC_E8,	KP_ASC_EC,	KP_ASC_F1,	KP_ASC_F2,	KP_ASC_F9,	KP_ASC_FC,	0xffff,		0xffff,		0xffff,	0xffff,0xffff,0xffff,0xffff,0xffff 	 	//7
};

/////////////////////////////////////////////////////////

#ifdef USE_COUNTRY_KEY_IN_SOURCE
// US  //////////////////////////////////////////////////
//Modified By JJANG 20090316 미국 Default key map 변경
ROMDATA INT16U prog_key_code_setA_us[] =
{
//	0		1		2		3  		4   		5  	   	6  	      	7	      	8  	     	9      	      	A    		B   		C	 			      D	   	      E    	       F
	0xa100,		0xffff,	   	0xffff,		0xffff, 	0xffff, 	0xffff, 	0xffff, 	0xffff, 	0xffff, 	0xffff, 	0xffff, 	0xffff, 	0xffff, 				0xffff, 				0xffff, 	    0xffff, 	//0
	0xffff, 	0xffff,    	0xffff,		0xffff,  	0xffff, 	0xffff, 	0xffff,  	0xffff, 	0xffff,		0xffff, 	0xffff, 	0xffff, 	0xffff, 				0xffff, 				0xffff, 	    0xffff, 	//1
//	0		1		2		3  	    	4   		5  	  	6  	    	7	      	8  	    	9      		A    	B      C      D	     E    	 F
	0xffff,		KP_ASC_A,  	KP_ASC_B,	KP_ASC_C,	KP_ASC_D,	KP_ASC_E,	KP_ASC_F,	KP_ASC_G,	KP_ASC_H,	KP_CAPS, 	KP_ZERO,	KP_TARE,	0xffff,					KP_ARROW_UP,KP_PAGE_UP,	 0xffff, 	//2
	0xffff,		KP_ASC_I,  	KP_ASC_J,	KP_ASC_K,	KP_ASC_L,	KP_ASC_M,	KP_ASC_N,	KP_ASC_O,	KP_ASC_P,	0xffff,		0xffff,		0xffff,		KP_ARROW_LF,KP_SAVE,	   KP_ARROW_RI,	0xffff, 	//3
	0xffff,		KP_ASC_Q,  	KP_ASC_R,	KP_ASC_S,	KP_ASC_T,	KP_ASC_U,	KP_ASC_V,	KP_ASC_W,	KP_ASC_X,	KP_INSERT,	KEY_NUM_7,	KEY_NUM_8,	KEY_NUM_9,		KP_ARROW_DN,KP_PAGE_DN,	 0xffff,	 //4
	0xffff,		KP_ASC_Y, 	KP_ASC_Z,	KP_ASC_24,	KP_ASC_2D,	KP_BS,		KP_ASC_20,	KP_CHAR_CODE,	KP_ASC_LF,	KP_DEL,		KEY_NUM_4,	KEY_NUM_5,	KEY_NUM_6,		KP_ESC,		   KP_TEST,	    0xffff, 	//5
	0xffff,		KP_ASC_2F, 	KP_ASC_25,	KP_ASC_28,	KP_ASC_29,	KP_ASC_2E,	KP_ASC_2C,	KP_ASC_22,	KP_CHANGELANG,	0xffff,		KEY_NUM_1,	KEY_NUM_2,	KEY_NUM_3,		KP_FEED,	   0xffff,	     0xffff, 	//6
	0xffff,		KP_ASC_C0,   	KP_ASC_C8,	KP_ASC_CC,	KP_ASC_D1,	KP_ASC_D2,	KP_ASC_D9,	KP_ASC_DC,	0xffff,		0xffff,		KEY_NUM_10,	KEY_NUM_0,	KP_CLEAR,			KP_ENTER,	  KP_ENTER,	   KEY_IDENTIFY 		//7
};

ROMDATA INT16U prog_key_code_setA_us_shifted[] =
{
//	0		1		2		3  	    	4   		5  	  	6  	    	7	      	8  	    	9      		A    	B      C      D	     E    	 F
	0xa100,		0xffff,	   	0xffff,		0xffff, 	0xffff, 	0xffff, 	0xffff, 	0xffff, 	0xffff, 	0xffff, 	0xffff, 0xffff,0xffff,0xffff,0xffff,0xffff,	 	//0
	0xffff, 	0xffff,    	0xffff,		0xffff,  	0xffff, 	0xffff, 	0xffff,  	0xffff, 	0xffff,		0xffff, 	0xffff, 0xffff,0xffff,0xffff,0xffff,0xffff, 	 //1

	0xffff,		KP_ASC_A_L,  	KP_ASC_B_L,	KP_ASC_C_L,	KP_ASC_D_L,	KP_ASC_E_L,	KP_ASC_F_L,	KP_ASC_G_L,	KP_ASC_H_L,	0xffff,		0xffff,	0xffff,0xffff,0xffff,0xffff,0xffff,	 	//2
	0xffff,		KP_ASC_I_L,  	KP_ASC_J_L,	KP_ASC_K_L,	KP_ASC_L_L,	KP_ASC_M_L,	KP_ASC_N_L,	KP_ASC_O_L,	KP_ASC_P_L,	0xffff,		0xffff,	0xffff,0xffff,0xffff,0xffff,0xffff,	 	//3
	0xffff,		KP_ASC_Q_L,  	KP_ASC_R_L,	KP_ASC_S_L,	KP_ASC_T_L,	KP_ASC_U_L,	KP_ASC_V_L,	KP_ASC_W_L,	KP_ASC_X_L,	0xffff, 	0xffff,	0xffff,0xffff,0xffff,0xffff,0xffff,	 	//4
	0xffff,		KP_ASC_Y_L, 	KP_ASC_Z_L,	KP_ASC_2A,	KP_ASC_5F,	KP_BS,		KP_ASC_20,	0xffff,		0xffff,		0xffff,		0xffff,	0xffff,0xffff,0xffff,0xffff,0xffff,	 	//5
	0xffff,		KP_ASC_3F, 	KP_ASC_21,	KP_ASC_40,	KP_ASC_23,	KP_ASC_3A,	KP_ASC_3B,	KP_ASC_27,	0xffff,		0xffff,		0xffff,	0xffff,0xffff,0xffff,0xffff,0xffff,	 	//6
	0xffff,		KP_ASC_E0,	KP_ASC_E8,	KP_ASC_EC,	KP_ASC_F1,	KP_ASC_F2,	KP_ASC_F9,	KP_ASC_FC,	0xffff,		0xffff,		0xffff,	0xffff,0xffff,0xffff,0xffff,0xffff 	 	//7
};

/////////////////////////////////////////////////////////

// RUS //////////////////////////////////////////////////

ROMDATA INT16U prog_key_code_setA_ru[] =
{
//	0		1		2		3  		4   		5  	   	6  	      	7	      	8  	     	9      	      	A    		B   		C	 			      D	   	      E    	       F
	0xa100,		0xffff   ,   	0xffff, 	0xffff, 	0xffff, 	0xffff, 	0xffff, 	0xffff, 	0xffff, 	0xffff, 	0xffff, 	0xffff, 	0xffff, 				0xffff, 				0xffff, 	    0xffff, 	//0
	0xffff,		0xffff   ,   	0xffff, 	0xffff,  	0xffff, 	0xffff, 	0xffff,  	0xffff, 	0xffff,		0xffff, 	0xffff, 	0xffff, 	0xffff, 				0xffff, 				0xffff, 	    0xffff, 	//1
	0xffff,		KP_ASC_C0,	KP_ASC_C1,	KP_ASC_C2,	KP_ASC_C3,	KP_ASC_C4,	KP_ASC_C5,	KP_ASC_A8,	KP_ASC_C6,	KP_CAPS, 	KP_ZERO,	KP_TARE,	0xffff,					KP_ARROW_UP,KP_PAGE_UP,	 0xffff, 	//2
	0xffff,		KP_ASC_C7,	KP_ASC_C8,	KP_ASC_C9,	KP_ASC_CA,	KP_ASC_CB,	KP_ASC_CC,	KP_ASC_CD,	KP_ASC_CE,	KP_INSERT,	0xffff,		0xffff,		KP_ARROW_LF,KP_SAVE,	   KP_ARROW_RI,	0xffff, 	//3
	0xffff,		KP_ASC_CF,	KP_ASC_D0,	KP_ASC_D1,	KP_ASC_D2,	KP_ASC_D3,	KP_ASC_D4,	KP_ASC_D5,	KP_ASC_D6,	KP_DEL,		KEY_NUM_7,	KEY_NUM_8,	KEY_NUM_9,		KP_ARROW_DN,KP_PAGE_DN,	 0xffff,	 //4
	0xffff,		KP_ASC_D7,	KP_ASC_D8,	KP_ASC_D9,	KP_ASC_DA,	KP_ASC_DB,   	KP_ASC_DC,	KP_ASC_DD,	KP_ASC_DE,	0xffff,		KEY_NUM_4,	KEY_NUM_5,	KEY_NUM_6,		KP_ESC,		   KP_TEST,	    0xffff, 	//5
	0xffff,		KP_ASC_DF,   	0xffff,		0xffff,		0xffff,		KP_BS,		KP_ASC_20,	KP_ASC_20,	KP_ASC_LF,	0xffff,		KEY_NUM_1,	KEY_NUM_2,	KEY_NUM_3,		KP_FEED,	   0xffff,	     0xffff, 	//6
	0xffff,		KP_ASC_2F,	KP_ASC_25,	KP_ASC_28,	KP_ASC_29,	KP_ASC_2E,	KP_ASC_2C,	KP_CHAR_CODE,	KP_CHANGELANG, 	0xffff,		KEY_NUM_10,	KEY_NUM_0,	KP_CLEAR,		KP_ENTER,	  KP_ENTER,	   KEY_IDENTIFY 		//7
};

ROMDATA INT16U prog_key_code_setA_ru_shifted[] =
{
//	0		1		2		3  	    	4   		5  	  	6  	    	7	      	8  	    	9      		A    	B      C      D	     E    	 F
	0xa100,		0xffff,	   	0xffff,		0xffff, 	0xffff, 	0xffff, 	0xffff, 	0xffff, 	0xffff		,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,	 	//0
	0xffff, 	0xffff,    	0xffff,		0xffff,  	0xffff, 	0xffff, 	0xffff,  	0xffff, 	0xffff		,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff, 	 //1
	0xffff,		KP_ASC_E0,  	KP_ASC_E1,	KP_ASC_E2,	KP_ASC_E3,	KP_ASC_E4,	KP_ASC_E5,	KP_ASC_B8,	KP_ASC_E6	,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,	 	//2
	0xffff,		KP_ASC_E7,  	KP_ASC_E8,	KP_ASC_E9,	KP_ASC_EA,	KP_ASC_EB,	KP_ASC_EC,	KP_ASC_ED,	KP_ASC_EE	,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,	 	//3
	0xffff,		KP_ASC_EF,  	KP_ASC_F0,	KP_ASC_F1,	KP_ASC_F2,	KP_ASC_F3,	KP_ASC_F4,	KP_ASC_F5,	KP_ASC_F6	,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,	 	//4
	0xffff,		KP_ASC_F7, 	KP_ASC_F8,	KP_ASC_F9,	KP_ASC_FA,	KP_ASC_FB,	KP_ASC_FC,	KP_ASC_FD,	KP_ASC_FE	,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,	 	//5
	0xffff,		KP_ASC_FF, 	0xffff,		0xffff,		0xffff,		KP_BS,		KP_ASC_20,	KP_ASC_20,	KP_ASC_LF	,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,	 	//6
	0xffff,		KP_ASC_2F,	KP_ASC_25,	KP_ASC_28,	KP_ASC_29,	KP_ASC_2E,	KP_ASC_2C,	KP_CHAR_CODE,	KP_CHANGELANG	,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff 	 	//7
};

ROMDATA INT16U prog_key_code_setB_ru[] =
{
//	0		1		2		3  		4   		5  	   	6  	      	7	      	8  	     	9      	      	A    		B   		C	 			      D	   	      E    	       F
	0xa100,		0xffff,	   	0xffff,		0xffff, 	0xffff, 	0xffff, 	0xffff, 	0xffff, 	0xffff, 	0xffff, 	0xffff, 	0xffff, 	0xffff, 				0xffff, 				0xffff, 	    0xffff, 	//0
	0xffff, 	0xffff,    	0xffff,		0xffff,  	0xffff, 	0xffff, 	0xffff,  	0xffff, 	0xffff,		0xffff, 	0xffff, 	0xffff, 	0xffff, 				0xffff, 				0xffff, 	    0xffff, 	//1
	0xffff,		KP_ASC_A,  	KP_ASC_B,	KP_ASC_C,	KP_ASC_D,	KP_ASC_E,	KP_ASC_F,	KP_ASC_G,	KP_ASC_H,	0xffff, 	0xffff, 	0xffff, 	0xffff, 		  0xffff, 				0xffff, 				 0xffff,	 //2
	0xffff,		KP_ASC_I,  	KP_ASC_J,	KP_ASC_K,	KP_ASC_L,	KP_ASC_M,	KP_ASC_N,	KP_ASC_O,	KP_ASC_P,	0xffff,		0xffff, 	0xffff, 	0xffff, 		  0xffff, 				0xffff, 				 0xffff,	 //3
	0xffff,		KP_ASC_Q,  	KP_ASC_R,	KP_ASC_S,	KP_ASC_T,	KP_ASC_U,	KP_ASC_V,	KP_ASC_W,	KP_ASC_X,	0xffff,		0xffff, 	0xffff, 	0xffff, 		  0xffff, 				0xffff, 				 0xffff,	 //4
	0xffff,		KP_ASC_Y, 	KP_ASC_Z,	KP_ASC_24,	0xffff,		0xffff,		0xffff,		0xffff,		0xffff,		0xffff,		0xffff, 	0xffff, 	0xffff, 		  0xffff, 				0xffff, 				 0xffff,	 //5
	0xffff,		0xffff,		0xffff, 	0xffff,		0xffff,		KP_BS,		KP_ASC_20,	KP_ASC_20,	KP_ASC_LF,	0xffff,		0xffff, 	0xffff,		0xffff,				 0xffff,			  0xffff,      0xffff,	 //6
	0xffff,		KP_ASC_2F,	KP_ASC_25,	KP_ASC_28,	KP_ASC_29,	KP_ASC_2E,	KP_ASC_2C,	KP_CHAR_CODE,	KP_CHANGELANG,  0xffff,	  0xffff, 0xffff, 0xffff,0xffff,0xffff,0xffff 	 	//7
};

/////////////////////////////////////////////////////////

// KOR //////////////////////////////////////////////////

ROMDATA INT16U prog_key_code_setA_kor[] =
{
//	0		1		2		3  		4   		5  	   	6  	      	7	      	8  	     	9      	      	A    		B   		C	 			      D	   	      E    	       F
	0xa100,		0xffff,	   	0xffff,		0xffff, 	0xffff, 	0xffff, 	0xffff, 	0xffff, 	0xffff, 	0xffff, 	0xffff, 	0xffff, 	0xffff, 				0xffff, 				0xffff, 	    0xffff, 	//0
	0xffff, 	0xffff,    	0xffff,		0xffff,  	0xffff, 	0xffff, 	0xffff,  	0xffff, 	0xffff,		0xffff, 	0xffff, 	0xffff, 	0xffff, 				0xffff, 				0xffff, 	    0xffff, 	//1

	0xffff,		KP_ASC_A,  	KP_ASC_B,	KP_ASC_C,	KP_ASC_D,	KP_ASC_E,	KP_ASC_F,	KP_ASC_G,	KP_BS,		KEY_SHIFT, 	KP_ZERO,	KP_TARE,	0xffff,					KP_ARROW_UP,KP_PAGE_UP,	 0xffff, 	//2
	0xffff,		KP_ASC_H,  	KP_ASC_I,	KP_ASC_J,	KP_ASC_K,	KP_ASC_L,	KP_ASC_M,	KP_ASC_N,	KP_ASC_20,	KP_INSERT,	KP_ASC_0C,	0xffff,		KP_ARROW_LF,KP_SAVE,	   KP_ARROW_RI,	0xffff, 	//3
	0xffff,		KP_ASC_O,  	KP_ASC_P,	KP_ASC_Q,	KP_ASC_R,	KP_ASC_S,	KP_ASC_T,	KP_ASC_U,	KP_ASC_20,	KP_DEL,		KEY_NUM_7,	KEY_NUM_8,	KEY_NUM_9,		KP_ARROW_DN,KP_PAGE_DN,	 0xffff,	 //4
	0xffff,		KP_ASC_V, 	KP_ASC_W,	KP_ASC_X,	KP_ASC_Y,	KP_ASC_Z,	0xffff,		0xffff,		KP_ASC_LF,  	KP_CAPS,	KEY_NUM_4,	KEY_NUM_5,	KEY_NUM_6,		KP_ESC,		   KP_TEST,	    0xffff, 	//5
	0xffff,		KP_ASC_2F, 	KP_ASC_25,	KP_ASC_28,	KP_ASC_29,	KP_ASC_2E,	KP_ASC_2C,	KP_CHAR_CODE,	KP_CHANGELANG,	0xffff,		KEY_NUM_1,	KEY_NUM_2,	KEY_NUM_3,		KP_FEED,	   0xffff,	     0xffff, 	//6
	0xffff,		KP_ASC_3A,  	KP_ASC_26,	KP_ASC_7B,	KP_ASC_7D,	KP_ASC_2A,	KP_ASC_2D,	KP_ASC_2B,	KP_CODE_A1C9,	0xffff,		KEY_NUM_10,	KEY_NUM_0,	KP_CLEAR,			KP_ENTER,	  KP_ENTER,	   KEY_IDENTIFY 		//7
};

ROMDATA INT16U prog_key_code_setA_kor_shifted[] =
{
//	0		1		2		3  	    	4   		5  	  	6  	    	7	      	8  	    	9      		A    	B      C      D	     E    	 F
	0xa100,		0xffff,	   	0xffff,		0xffff, 	0xffff, 	0xffff, 	0xffff, 	0xffff, 	0xffff, 	0xffff, 	0xffff, 0xffff,0xffff,0xffff,0xffff,0xffff,	 	//0
	0xffff, 	0xffff,    	0xffff,		0xffff,  	0xffff, 	0xffff, 	0xffff,  	0xffff, 	0xffff,		0xffff, 	0xffff, 0xffff,0xffff,0xffff,0xffff,0xffff, 	 //1
	0xffff,		KP_ASC_A_L,  	KP_ASC_B_L,	KP_ASC_C_L,	KP_ASC_D_L,	KP_ASC_E_L,	KP_ASC_F_L,	KP_ASC_G_L,	0xffff,		0xffff,		0xffff,	0xffff,0xffff,0xffff,0xffff,0xffff,	 	//2
	0xffff,		KP_ASC_H_L,  	KP_ASC_I_L,	KP_ASC_J_L,	KP_ASC_K_L,	KP_ASC_L_L,	KP_ASC_M_L,	KP_ASC_N_L,	0xffff,		0xffff,		0xffff,	0xffff,0xffff,0xffff,0xffff,0xffff,	 	//3
	0xffff,		KP_ASC_O_L,  	KP_ASC_P_L,	KP_ASC_Q_L,	KP_ASC_R_L,	KP_ASC_S_L,	KP_ASC_T_L,	KP_ASC_U_L,	0xffff,		0xffff, 	0xffff,	0xffff,0xffff,0xffff,0xffff,0xffff,	 	//4
	0xffff,		KP_ASC_V_L, 	KP_ASC_W_L,	KP_ASC_X_L,	KP_ASC_Y_L,	KP_ASC_Z_L,	0xffff,		0xffff,		0xffff,		0xffff,		0xffff,	0xffff,0xffff,0xffff,0xffff,0xffff,	 	//5
	0xffff,		KP_ASC_3F, 	KP_ASC_21,	KP_ASC_3C,	KP_ASC_3E,	KP_ASC_22,	KP_ASC_27,	KP_CHAR_CODE,	KP_CHANGELANG,	0xffff,		0xffff,	0xffff,0xffff,0xffff,0xffff,0xffff,	 	//6
	0xffff,		KP_ASC_3B, 	KP_ASC_24,	KP_ASC_5B,	KP_ASC_5D,	KP_ASC_40,	KP_ASC_5F,	KP_ASC_3D,	KP_ASC_23,	0xffff,		0xffff,	0xffff,0xffff,0xffff,0xffff,0xffff 	 	//7
};
   
ROMDATA INT16U prog_key_code_setB_kor[] =
{
//	0		1		2		3  		4   		5  	   	6  	      	7	      	8  	     	9      	      	A       B      C      D	     E    	 F
	0xa100,		0xffff,	   	0xffff,		0xffff, 	0xffff, 	0xffff, 	0xffff, 	0xffff, 	0xffff, 	0xffff, 	0xffff, 0xffff,0xffff,0xffff,0xffff,0xffff,		//0
	0xffff,		0xffff,    	0xffff,		0xffff,  	0xffff, 	0xffff, 	0xffff,  	0xffff, 	0xffff,		0xffff, 	0xffff, 0xffff,0xffff,0xffff,0xffff,0xffff,  //1

	0xffff,		KP_I_K,  	KP_I_N,		KP_I_D,		KP_I_R,		KP_I_M,		KP_I_B,		KP_I_S,		KP_BS,		0xffff,		0xffff,	0xffff,0xffff,0xffff,0xffff,0xffff, 	//2
	0xffff,		KP_I_O,  	KP_I_J,		KP_I_CH,	KP_I_C,		KP_I_T,		KP_I_P,		KP_I_H,		KP_ASC_20, 	0xffff,		0xffff, 0xffff,0xffff,0xffff,0xffff,0xffff, 	//3
	0xffff,		KP_N_A,  	KP_N_YA,	KP_N_EO, 	KP_N_YEO,	KP_N_O,		KP_N_YO,	KP_N_U,		KP_ASC_20,	0xffff, 	0xffff, 0xffff,0xffff,0xffff,0xffff,0xffff, 	//4
	0xffff,		KP_N_YU, 	KP_N_EU,	KP_N_I,		KP_N_AE,	KP_N_YAE,	KP_N_E,		KP_N_YE,	KP_ASC_LF,	0xffff,		0xffff,	0xffff,0xffff,0xffff,0xffff,0xffff, 	//5
	0xffff,		KP_ASC_2F, 	KP_ASC_25,	KP_ASC_28,	KP_ASC_29,	KP_ASC_2E,	KP_ASC_2C,	KP_CHAR_CODE,	KP_CHANGELANG,	0xffff,		0xffff,	0xffff,0xffff,0xffff,0xffff,0xffff, 	//6
	0xffff,		KP_ASC_3A,  	KP_ASC_26,	KP_ASC_7B,	KP_ASC_7D,	KP_ASC_2A,	KP_ASC_2D,	KP_ASC_2B,	KP_CODE_A1C9,	0xffff,		0xffff,	0xffff,0xffff,0xffff,0xffff,0xffff  	//7
};

ROMDATA INT16U prog_key_code_setB_kor_shifted[] =
{
//	0		1		2		3  	    	4   		5  	  	6  	    	7	      	8  	    	9      		A      	B      C      D	     E    	 F
	0xa100,		0xffff,	   	0xffff,		0xffff, 	0xffff, 	0xffff, 	0xffff, 	0xffff, 	0xffff, 	0xffff, 	0xffff, 0xffff,0xffff,0xffff,0xffff,0xffff,	 	//0
	0xffff,		0xffff,    	0xffff,		0xffff,  	0xffff, 	0xffff, 	0xffff,  	0xffff, 	0xffff,		0xffff, 	0xffff, 0xffff,0xffff,0xffff,0xffff,0xffff, 	 //1

	0xffff,		KP_I_KK,  	KP_I_N,		KP_I_DD,       	KP_I_R,		KP_I_M,		KP_I_BB,	KP_I_SS,	KP_BS,		0xffff,		0xffff,	0xffff,0xffff,0xffff,0xffff,0xffff,	 	//2
	0xffff,		KP_I_O,  	KP_I_JJ,	KP_I_CH,	KP_I_C,		KP_I_T,		KP_I_P,		KP_I_H,		KP_ASC_20,	0xffff,		0xffff, 0xffff,0xffff,0xffff,0xffff,0xffff,	 	//3
	0xffff,		KP_N_A,  	KP_N_YA,	KP_N_EO, 	KP_N_YEO,	KP_N_O,		KP_N_YO,	KP_N_U,		KP_ASC_20,	0xffff, 	0xffff, 0xffff,0xffff,0xffff,0xffff,0xffff,	 	//4
	0xffff,		KP_N_YU, 	KP_N_EU,	KP_N_I,		KP_N_AE,	KP_N_YAE,	KP_N_E,		KP_N_YE,	KP_ASC_LF,	0xffff,		0xffff,	0xffff,0xffff,0xffff,0xffff,0xffff,	 	//5
	0xffff,		KP_ASC_3F, 	KP_ASC_21,	KP_ASC_3C,	KP_ASC_3E,	KP_ASC_22,	KP_ASC_27,	KP_CHAR_CODE,	KP_CHANGELANG,	0xffff,		0xffff,	0xffff,0xffff,0xffff,0xffff,0xffff,	 	//6
	0xffff,		KP_ASC_3B, 	KP_ASC_24,	KP_ASC_5B,	KP_ASC_5D,	KP_ASC_40,	KP_ASC_5F,	KP_ASC_3D,	KP_ASC_23,	0xffff,		0xffff,	0xffff,0xffff,0xffff,0xffff,0xffff 	 	//7
};

#endif //#ifdef USE_COUNTRY_KEY_IN_SOURCE
/////////////////////////////////////////////////////////
// China(CN)/////////////////////////////////////////////
ROMDATA INT16U normal_key_code_set_cn[] =
{
//	0		1			2			3			4			5			6			7			8			9				A			B			C			D			E			F
	0xa100,	KS_PLU_49,	KS_PLU_50,	KS_PLU_51,	KS_PLU_52,	KS_PLU_53,	KS_PLU_54,	KS_PLU_55,	KS_PLU_56,	KS_PLU_65,		KS_PLU_66,	KS_PLU_67,	KS_PLU_68,	KS_PLU_69,	KS_PLU_70,	0xffff,      // 0
	0xffff,	KS_PLU_57,	KS_PLU_58,	KS_PLU_59,	KS_PLU_60,	KS_PLU_61,	KS_PLU_62,	KS_PLU_63,	KS_PLU_64,	KS_PLU_71,		KS_PLU_72,	KS_CLERK_01,KS_CLERK_02,KS_CLERK_03,KS_CLERK_04,0xffff,      // 1

	0xffff,	KS_PLU_01,  KS_PLU_02,	KS_PLU_03, 	KS_PLU_04, 	KS_PLU_05, 	KS_PLU_06, 	KS_PLU_07  ,KS_PLU_08, 	KS_RETURN, 		KS_ZERO, 	KS_TARE, 	KS_OVERRIDE,KS_DATETIME,KS_MODE,    0xffff,      //0
	0xffff,	KS_PLU_09,  KS_PLU_10,	KS_PLU_11, 	KS_PLU_12, 	KS_PLU_13, 	KS_PLU_14, 	KS_PLU_15  ,KS_PLU_16, 	KS_SALE_PER, 	KS_FOR, 	KS_AUTO,   	KS_PREPACK, KS_SAVE, 	KS_VOID,    0xffff,      //1
	0xffff,	KS_PLU_17, 	KS_PLU_18,	KS_PLU_19, 	KS_PLU_20, 	KS_PLU_21, 	KS_PLU_22, 	KS_PLU_23  ,KS_PLU_24, 	KS_MINUS, 		KEY_NUM_7, 	KEY_NUM_8, 	KEY_NUM_9, 	KS_PLU, 	KS_ADD,     0xffff,      //2
	0xffff,	KS_PLU_25,	KS_PLU_26,	KS_PLU_27, 	KS_PLU_28,	KS_PLU_29,	KS_PLU_30,	KS_PLU_31  ,KS_PLU_32, 	KS_TYPE,		KEY_NUM_4, 	KEY_NUM_5, 	KEY_NUM_6, 	KS_MULTI, 	KS_PAY,     0xffff,      //3
	0xffff,	KS_PLU_33,  KS_PLU_34,	KS_PLU_35, 	KS_PLU_36, 	KS_PLU_37, 	KS_PLU_38, 	KS_PLU_39  ,KS_PLU_40, 	KS_CLERK_01,	KEY_NUM_1, 	KEY_NUM_2, 	KEY_NUM_3, 	KS_FEED, 	KS_TTL,     0xffff,      //4
	0xffff,	KS_PLU_41,	KS_PLU_42,	KS_PLU_43, 	KS_PLU_44, 	KS_PLU_45, 	KS_PLU_46, 	KS_PLU_47  ,KS_PLU_48, 	KS_CLERK_02,	KEY_NUM_10, KEY_NUM_0, 	KS_CLEAR, 	KS_PRINT, 	KS_PRINT,   KEY_IDENTIFY //5
};

ROMDATA INT16U normal_key_code_set_cn_shifted[] =
{
//	0		1			2			3			4			5			6			7			8			9				A			B			C			D			E			F
	0xa100,	KS_PLU_121,	KS_PLU_122,	KS_PLU_123,	KS_PLU_124,	KS_PLU_125,	KS_PLU_126,	KS_PLU_127,	KS_PLU_128,	KS_PLU_129,		KS_PLU_130,	KS_PLU_131,	KS_PLU_132,	KS_PLU_133,	KS_PLU_134,	0xffff,// 0
	0xffff,	KS_PLU_135,	KS_PLU_136,	KS_PLU_137,	KS_PLU_138,	KS_PLU_139,	KS_PLU_140,	KS_PLU_141,	KS_PLU_142,	KS_PLU_143,		KS_PLU_144,	KS_CLERK_01,KS_CLERK_02,KS_CLERK_03,KS_CLERK_04,0xffff,// 1

	0xffff,	KS_PLU_73, 	KS_PLU_74,	KS_PLU_75, 	KS_PLU_76, 	KS_PLU_77, 	KS_PLU_78, 	KS_PLU_79,	KS_PLU_80, 	KS_RETURN, 		KS_ZERO, 	KS_TARE, 	KS_OVERRIDE,KS_DATETIME,KS_MODE,    0xffff,      //0
	0xffff,	KS_PLU_81,	KS_PLU_82,	KS_PLU_83,	KS_PLU_84, 	KS_PLU_85, 	KS_PLU_86,	KS_PLU_87,	KS_PLU_88, 	KS_SALE_PER, 	KS_FOR, 	KS_AUTO,   	KS_PREPACK, KS_SAVE, 	KS_VOID,    0xffff,      //1
	0xffff,	KS_PLU_89,	KS_PLU_90,	KS_PLU_91, 	KS_PLU_92, 	KS_PLU_93, 	KS_PLU_94, 	KS_PLU_95,	KS_PLU_96, 	KS_MINUS, 		KEY_NUM_7, 	KEY_NUM_8, 	KEY_NUM_9, 	KS_PLU, 	KS_ADD,     0xffff,      //2
	0xffff,	KS_PLU_97, 	KS_PLU_98,	KS_PLU_99, 	KS_PLU_100,	KS_PLU_101,	KS_PLU_102,	KS_PLU_103,	KS_PLU_104,	KS_TYPE,		KEY_NUM_4, 	KEY_NUM_5, 	KEY_NUM_6, 	KS_MULTI, 	KS_PAY,     0xffff,      //3
	0xffff,	KS_PLU_105,	KS_PLU_106,	KS_PLU_107,	KS_PLU_108,	KS_PLU_109,	KS_PLU_110,	KS_PLU_111,	KS_PLU_112,	KS_CLERK_01,	KEY_NUM_1, 	KEY_NUM_2, 	KEY_NUM_3, 	KS_FEED, 	KS_TTL,     0xffff,      //4
	0xffff,	KS_PLU_113,	KS_PLU_114,	KS_PLU_115,	KS_PLU_116,	KS_PLU_117, KS_PLU_118, KS_PLU_119,	KS_PLU_120,	KS_CLERK_02,	KEY_NUM_10, KEY_NUM_0, 	KS_CLEAR, 	KS_PRINT, 	KS_PRINT,   KEY_IDENTIFY //5
	
};

ROMDATA INT16U prog_key_code_setA_cn[] =
{
//	0		1			2			3			4			5			6			7				8				9			A			B			C			D			E			F
	0xa100,	0xffff,	   	0xffff,		0xffff, 	0xffff, 	0xffff, 	0xffff, 	0xffff, 		0xffff, 		0xffff, 	0xffff, 	0xffff, 	0xffff, 	0xffff, 	0xffff, 	0xffff, 	//0
	0xffff, 0xffff,    	0xffff,		0xffff,  	0xffff, 	0xffff, 	0xffff,  	0xffff, 		0xffff,			0xffff, 	0xffff, 	0xffff, 	0xffff, 	0xffff, 	0xffff, 	0xffff, 	//1

	0xffff,	KP_ASC_A,  	KP_ASC_B,	KP_ASC_C,	KP_ASC_D,	KP_ASC_E,	KP_ASC_F,	KP_ASC_G,		KP_ASC_H,		KEY_SHIFT, 	KP_ZERO,	KP_TARE,  	0xffff,		KP_ARROW_UP,KP_PAGE_UP,	0xffff, 	//2
	0xffff,	KP_ASC_I,  	KP_ASC_J,	KP_ASC_K,	KP_ASC_L,	KP_ASC_M,	KP_ASC_N,	KP_ASC_O,		KP_ASC_P,		KP_INSERT,	KP_PHONETYPE,	0xffff,		KP_ARROW_LF,KP_SAVE,	KP_ARROW_RI,0xffff, 	//3
	0xffff,	KP_ASC_Q,  	KP_ASC_R,	KP_ASC_S,	KP_ASC_T,	KP_ASC_U,	KP_ASC_V,	KP_ASC_W,		KP_ASC_X,		KP_DEL,		KEY_NUM_7,	KEY_NUM_8,	KEY_NUM_9,	KP_ARROW_DN,KP_PAGE_DN,	0xffff,	 //4
	0xffff,	KP_ASC_Y, 	KP_ASC_Z,	KP_ASC_24,	KP_ASC_2D,	KP_BS,		KP_ASC_20,	KP_ASC_20,		KP_ASC_LF,		KP_CAPS,	KEY_NUM_4,	KEY_NUM_5,	KEY_NUM_6,	KP_ESC,		KP_TEST,	0xffff, 	//5
	0xffff,	KP_ASC_2F, 	KP_ASC_25,	KP_ASC_28,	KP_ASC_29,	KP_ASC_2E,	KP_ASC_2C,	KP_CHAR_CODE,	KP_CHANGELANG,	0xffff,		KEY_NUM_1,	KEY_NUM_2,	KEY_NUM_3,	KP_FEED,	0xffff,	    0xffff, 	//6
	0xffff,	KP_ASC_C0,  KP_ASC_C8,	KP_ASC_CC,	KP_ASC_D1,	KP_ASC_D2,	KP_ASC_D9,	KP_ASC_DC,		0xffff,			0xffff,		KEY_NUM_10,	KEY_NUM_0,	KP_CLEAR,	KP_ENTER,	KP_ENTER,	KEY_IDENTIFY 		//7
};

ROMDATA INT16U prog_key_code_setA_cn_shifted[] =
{
//	0		1			2			3			4			5			6			7				8				9			A			B			C			D			E			F
	0xa100,	0xffff,	   	0xffff,		0xffff, 	0xffff, 	0xffff, 	0xffff, 	0xffff, 		0xffff, 		0xffff, 	0xffff, 	0xffff,		0xffff,		0xffff,		0xffff,		0xffff,	 	//0
	0xffff, 0xffff,    	0xffff,		0xffff,  	0xffff, 	0xffff, 	0xffff,  	0xffff, 		0xffff,			0xffff, 	0xffff, 	0xffff,		0xffff,		0xffff,		0xffff,		0xffff, 	 //1

	0xffff,	KP_ASC_A_L, KP_ASC_B_L,	KP_ASC_C_L,	KP_ASC_D_L,	KP_ASC_E_L,	KP_ASC_F_L,	KP_ASC_G_L,		KP_ASC_H_L,		KEY_SHIFT, 	KP_ZERO,	KP_TARE,  	0xffff,		KP_ARROW_UP,KP_PAGE_UP,	0xffff, 	//2
	0xffff,	KP_ASC_I_L, KP_ASC_J_L,	KP_ASC_K_L,	KP_ASC_L_L,	KP_ASC_M_L,	KP_ASC_N_L,	KP_ASC_O_L,		KP_ASC_P_L,		KP_INSERT,	KP_PHONETYPE,	0xffff,		KP_ARROW_LF,KP_SAVE,	KP_ARROW_RI,0xffff, 	//3
	0xffff,	KP_ASC_Q_L, KP_ASC_R_L,	KP_ASC_S_L,	KP_ASC_T_L,	KP_ASC_U_L,	KP_ASC_V_L,	KP_ASC_W_L,		KP_ASC_X_L,		KP_DEL,		KEY_NUM_7,	KEY_NUM_8,	KEY_NUM_9,	KP_ARROW_DN,KP_PAGE_DN,	0xffff,	 //4
	0xffff,	KP_ASC_Y_L, KP_ASC_Z_L,	KP_ASC_2A,	KP_ASC_5F,	KP_BS,		KP_ASC_20,	KP_ASC_20,		KP_ASC_LF,		KP_CAPS,	KEY_NUM_4,	KEY_NUM_5,	KEY_NUM_6,	KP_ESC,		KP_TEST,	0xffff, 	//5
	0xffff,	KP_ASC_3F, 	KP_ASC_21,	KP_ASC_40,	KP_ASC_23,	KP_ASC_3A,	KP_ASC_3B,	KP_CHAR_CODE,	KP_CHANGELANG,	0xffff,		KEY_NUM_1,	KEY_NUM_2,	KEY_NUM_3,	KP_FEED,	0xffff,	    0xffff, 	//6
	0xffff,	KP_ASC_E0,	KP_ASC_E8,	KP_ASC_EC,	KP_ASC_F1,	KP_ASC_F2,	KP_ASC_F9,	KP_ASC_FC,		0xffff,			0xffff,		KEY_NUM_10,	KEY_NUM_0,	KP_CLEAR,	KP_ENTER,	KP_ENTER,	KEY_IDENTIFY 		//7
};
/////////////////////////////////////////////////////////

INT8U KEY_CnvPtoB_Sale_CL5200[] =
{
	0,	40,	10,	49,	50,	51,	52,	53,	38,	37,	36,	35,	34,	33,	9,	15,
	16,	55,	56,	11,	65,	66,	67,	68,	69,	54,	39,	27,	28,	29,	30,	31,
	32,	70,	71,	72,	12,	81,	82,	83,	84,	41,	42,	43,	44,	45,	46,	47,
	48,	85,	86,	87,	88,	13,	97,	98,	99,	57,	58,	59,	60,	61,	62,	63,
	64,	100,	101,	102,	103,	104,	14,	113,	114,	73,	74,	75,	76,	77,	78,	79,
	80,	115,	116,	117,	118,	119,	120,	1,	2,	89,	90,	91,	92,	93,	94,	95,
	96,	3,	4,	5,	6,	7,	8,	17,	18,	105,	106,	107,	108,	109,	110,	111,
	112,	19,	20,	21,	22,	23,	24,	25,	26,	121,	122,	123,	124,	125,	126,	127
};

INT8U KEY_CnvPtoB_Prog_CL5200[] =
{
	0,	10,	49,	50,	51,	52,	53,	54,	38,	37,	36,	35,	34,	33,	9,	15,
	16,	11,	65,	66,	67,	68,	69,	70,	71,	55,	39,	27,	28,	29,	30,	31,
	32,	12,	81,	82,	83,	84,	85,	86,	87,	41,	42,	43,	44,	45,	46,	47,
	48,	13,	97,	98,	99,	100,	101,	102,	103,	57,	58,	59,	60,	61,	62,	63,
	64,	14,	113,	114,	115,	116,	117,	118,	119,	73,	74,	75,	76,	77,	78,	79,
	80,	1,	2,	3,	4,	5,	6,	7,	8,	89,	90,	91,	92,	93,	94,	95,
	96,	17,	18,	19,	20,	21,	22,	23,	26,	105,	106,	107,	108,	109,	110,	111,
	112,	25,	24,	40,	56,	72,	88,	104,	120,	121,	122,	123,	124,	125,	126,	127
};

#ifdef USE_KOREA_CL3000_STANDARD_KEY
INT8U KEY_CnvPtoB_Sale_CL3000[] =
{
	0,	1,	2,	3,	4,	5,	6,	7,	8,	9,	10,	11,	12,	13,	14,	15,
	16,	17,	18,	19,	20,	21,	22,	23,	24,	25,	26,	27,	28,	29,	30,	31,
	32,	33,	34,	35,	36,	37,	38,	39,	40,	58,	59,	60, 61,	62,42,47,
	48,	49,	50,	51,	52,	53,	54,	55,	56,	57,	109,	93,	77,	78,	41,	63,
	64,	65,	66,	67,	68,	69,	70,	71,	72,	73,	74,	75,	76,	94,	43,	79,
	80,	81,	82,	83,	84,	85,	86,	87,	88,	89,	90,	91,	92,	110,	44,	95,
	96,	97,	98,	99,	100, 101, 102, 103,	104, 105, 106, 107,	108, 125, 45, 111,
	112,113,114,115,116,117,118,119, 120, 121, 122, 123, 124, 126, 46,	127
};
#else
INT8U KEY_CnvPtoB_Sale_CL3000[] =
{
	0,	1,	2,	3,	4,	5,	6,	7,	8,	9,	10,	11,	12,	13,	14,	15,
	16,	17,	18,	19,	20,	21,	22,	23,	24,	25,	26,	27,	28,	29,	30,	31,
	32,	33,	34,	35,	36,	37,	38,	39,	40,	58,	59,	60, 61,	62,42,47,
	48,	49,	50,	51,	52,	53,	54,	55,	56,	57,	109,	93,	77,	78,	43,	63,
	64,	65,	66,	67,	68,	69,	70,	71,	72,	73,	74,	75,	76,	94,	44,	79,
	80,	81,	82,	83,	84,	85,	86,	87,	88,	89,	90,	91,	92,	110,	41,	95,
	96,	97,	98,	99,	100, 101, 102, 103,	104, 105, 106, 107,	108, 125, 45, 111,
	112,113,114,115,116,117,118,119, 120, 121, 122, 123, 124, 126, 46,	127
};
#endif
INT8U KEY_CnvPtoB_Prog_CL3000[] =
{
	0,	1,	2,	3,	4,	5,	6,	7,	8,	9,	10,	11,	12,	13,	14,	15,
	16,	17,	18,	19,	20,	21,	22,	23,	24,	25,	26,	27,	28,	29,	30,	31,
	32,	33,	34,	35,	36,	37,	38,	39,	40,	58,	59,	60, 61,	62,42,47,
	48,	49,	50,	51,	52,	53,	54,	55,	56,	57,	109,93,	77,	78,	43,	63,
	64,	65,	66,	67,	68,	69,	70,	71,	72,	73,	74,	75,	76,	94,	44,	79,
	80,	81,	82,	83,	84,	85,	86,	87,	88,	89,	90,	91,	92,	110, 45, 95,
	96,	97,	98,	99,	100, 101, 102, 103,	104, 105, 106, 107,	108, 125, 41, 111,
	112,113,114,115,116,117,118,119, 120, 121, 122, 123, 124, 126, 46,	127
};

INT8U KEY_CnvPtoB_Sale[] =
{
	0,	49,	50,	51,	52,	53,	54,	55,	39,	38,	37,	36,	35,	34,	33,	15,
	16,	65,	66,	67,	68,	69,	70,	71,	72,	56,	40,	27,	28,	29,	30,	31,
	32,	81,	82,	83,	84,	85,	86,	87,	88,	41,	42,	43,	44,	45,	46,	47,
	48,	97,	98,	99,	100,	101,	102,	103,	104,	57,	58,	59,	60,	61,	62,	63,
	64,	113,	114,	115,	116,	117,	118,	119,	120,	73,	74,	75,	76,	77,	78,	79,
	80,	1,	2,	3,	4,	5,	6,	7,	8,	89,	90,	91,	92,	93,	94,	95,
	96,	17,	18,	19,	20,	21,	22,	23,	24,	105,	106,	107,	108,	109,	110,	111,
	112,	9,	10,	11,	12,	13,	14,	25,	26,	121,	122,	123,	124,	125,	126,	127
};

INT8U KEY_CnvPtoB_Prog[] =
{
	0,	49,	50,	51,	52,	53,	54,	55,	39,	38,	37,	36,	35,	34,	33,	15,
	16,	65,	66,	67,	68,	69,	70,	71,	72,	56,	40,	27,	28,	29,	30,	31,
	32,	81,	82,	83,	84,	85,	86,	87,	88,	41,	42,	43,	44,	45,	46,	47,
	48,	97,	98,	99,	100,	101,	102,	103,	104,	57,	58,	59,	60,	61,	62,	63,
	64,	113,	114,	115,	116,	117,	118,	119,	120,	73,	74,	75,	76,	77,	78,	79,
	80,	1,	2,	3,	4,	5,	6,	7,	8,	89,	90,	91,	92,	93,	94,	95,
	96,	17,	18,	19,	20,	21,	22,	23,	24,	105,	106,	107,	108,	109,	110,	111,
	112,	9,	10,	11,	12,	13,	14,	25,	26,	121,	122,	123,	124,	125,	126,	127
};
INT8U KEY_CnvPtoH_Sale[] =
{
	0,	25,	8,	17,	18,	19,	20,	21,	6,	5,	4,	3,	2,	1,	26,	15,
	16,	23,	24,	9,	33,	34,	35,	36,	37,	22,	7,	27,	28,	29,	30,	31,
	32,	38,	39,	40,	10,	49,	50,	51,	52,	41,	42,	43,	44,	45,	46,	47,
	48,	53,	54,	55,	56,	11,	65,	66,	67,	57,	58,	59,	60,	61,	62,	63,
	64,	68,	69,	70,	71,	72,	12,	81,	82,	73,	74,	75,	76,	77,	78,	79,
	80,	83,	84,	85,	86,	87,	88,	13,	97,	89,	90,	91,	92,	93,	94,	95,
	96,	98,	99,	100,	101,	102,	103,	104,	14,	105,	106,	107,	108,	109,	110,	111,
	112,	113,	114,	115,	116,	117,	118,	119,	120,	121,	122,	123,	124,	125,	126,	127
};

INT8U KEY_CnvPtoH_Prog[] =
{
	0,	8,	17,	18,	19,	20,	21,	22,	6,	5,	4,	3,	2,	1,	26,	15,
	16,	9,	33,	34,	35,	36,	37,	38,	39,	23,	7,	27,	28,	29,	30,	31,
	32,	10,	49,	50,	51,	52,	53,	54,	55,	41,	42,	43,	44,	45,	46,	47,
	48,	11,	65,	66,	67,	68,	69,	70,	71,	57,	58,	59,	60,	61,	62,	63,
	64,	12,	81,	82,	83,	84,	85,	86,	87,	73,	74,	75,	76,	77,	78,	79,
	80,	13,	97,	98,	99,	100,	101,	102,	103,	89,	90,	91,	92,	93,	94,	95,
	96,	14,	113,	114,	115,	116,	117,	118, 	119,	105,	106,	107,	108,	109,	110,	111,
	112,	25,	24,	40,	56,	72,	88,	104,	120,	121,	122,	123,	124,	125,	126,	127
};

INT8U KEY_CnvPtoH_Sale_CL5200[] =
{
//		0	  1		  2		  3		  4		  5		  6		  7		  8		  9		  A		  B		  C		  D		  E		  F	
	0x00,	0x01,	0x02,	0x03,	0x04,	0x05,	0x06,	0x07,	0x08,	0x09,	0x0A,	0x0B,	0x0C,	0x0D,	0x0E,	0x0F,
	0x10,	0x11,	0x12,	0x13,	0x14,	0x15,	0x16,	0x17,	0x18,	0x19,	0x1A,	0x1B,	0x1C,	0x1D,	0x1E,	0x1F,
	0x20,	0x21,	0x22,	0x23,	0x24,	0x25,	0x26,	0x27,	0x28,	0x29,	0x2A,	0x2B,	0x2C,	0x2D,	0x2E,	0x2F,
	0x30,	0x31,	0x32,	0x33,	0x34,	0x35,	0x36,	0x37,	0x38,	0x39,	0x3A, 	0x3B,	0x3C,	0x3D,	0x3E,	0x3F,
	0x40,	0x41,	0x42,	0x43,	0x44,	0x45,	0x46,	0x47,	0x48,	0x49,	0x4A,	0x4B,	0x4C,	0x4D,	0x4E,	0x4F,
	0x50,	0x51,	0x52,	0x53,	0x54,	0x55,	0x56,	0x57,	0x58,	0x59,	0x5A,	0x5B,	0x5C,	0x5D,	0x5E,	0x5F,
	0x60,	0x61,	0x62,	0x63,	0x64,	0x65,	0x66,	0x67,	0x68,	0x69,	0x6A,	0x6B,	0x6C,	0x6D,	0x6E,	0x6F,
	0x70,	0x71,	0x72,	0x73,	0x74,	0x75,	0x76,	0x77,	0x78,	0x79,	0x7A,	0x7B,	0x7C,	0x7D,	0x7E,	0x7F
};

INT8U KEY_CnvPtoH_Prog_CL5200[] =
{
//		0	  1		  2		  3		  4		  5		  6		  7		  8		  9		  A		  B		  C		  D		  E		  F
	0x00,	0x05,	0x06,	0x07,	0x19,	0x11,	0x12,	0x13,	0x08,	0x09,	0x0A,	0x0B,	0x0C,	0x0D,	0x0E,	0x0F,
	0x10,	0x21,	0x22,	0x23,	0x24,	0x25,	0x26,	0x27,	0x28,	0x14,	0x1A,	0x1B,	0x1C,	0x1D,	0x1E,	0x1F,
	0x20,	0x35,	0x36,	0x37,	0x38,	0x41,	0x42,	0x43,	0x44,	0x29,	0x2A,	0x2B,	0x2C,	0x2D,	0x2E,	0x2F,
	0x30,	0x51,	0x52,	0x53,	0x54,	0x55,	0x56,	0x57,	0x58,	0x39,	0x3A, 	0x3B,	0x3C,	0x3D,	0x3E,	0x3F,
	0x40,	0x65,	0x66,	0x67,	0x68,	0x71,	0x72,	0x73,	0x74,	0x49,	0x4A,	0x4B,	0x4C,	0x4D,	0x4E,	0x4F,
	0x50,	0x04,	0x18,	0x34,	0x48,	0x64,	0x78,	0x03,	0x17,	0x59,	0x5A,	0x5B,	0x5C,	0x5D,	0x5E,	0x5F,
	0x60,	0x33,	0x47,	0x63,	0x77,	0x02,	0x16,	0x32,	0x46,	0x69,	0x6A,	0x6B,	0x6C,	0x6D,	0x6E,	0x6F,
	0x70,	0x62,	0x76,	0x01,	0x15,	0x31,	0x45,	0x61,	0x75,	0x79,	0x7A,	0x7B,	0x7C,	0x7D,	0x7E,	0x7F
};


////////////////////////////////////



INT8U KEY_ErrorChk(long a) // Modified by CJK 20050927
{
	INT8U rawcode, prevcode=0;
	long key_chktime;

	key_chktime=0;
	do {
		rawcode = KeyPort;
		if(rawcode == 0) return 1;
		//rawcode = KeyPort;
		if(rawcode != prevcode) key_chktime = 0;
		else key_chktime++;
		prevcode = rawcode;
	} while (key_chktime < a);
	return 0;
}

//SG091231 TEST ONLY Converting Table

INT8U ConvTable[] = {
      //0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B,0x0C,0x0D,0x0E,0x0F,
	0x0E, 0x0D, 0x0A, 0x09, 0x07, 0x08, 0x06, 0x05, 0x0B, 0x04, 0x0C, 0x03,0x02,0x01,0x0E,0x0F,  //0x
       0x10, 0x11, 0x1A, 0x19, 0x17, 0x18, 0x16, 0x15, 0x18, 0x14, 0x1A, 0x13,0x12,0x11,0x1E,0x1F,  //1x
       0x2E, 0x2D, 0x2A, 0x29, 0x27, 0x28, 0x26, 0x25, 0x2B, 0x24, 0x2C, 0x23,0x22,0x21,0x2E,0x2F,  //2x
	0x3E, 0x3D, 0x3A, 0x39, 0x37, 0x38, 0x36, 0x35, 0x3B, 0x34, 0x3C, 0x33,0x32,0x31,0x3E,0x3F,  //3x
	0x7E, 0x7D, 0x7A, 0x79, 0x77, 0x78, 0x76, 0x75, 0x7B, 0x74, 0x7C, 0x73,0x72,0x71,0x4E,0x4F,  //4x
	0x6E, 0x6D, 0x6A, 0x69, 0x67, 0x68, 0x66, 0x65, 0x6B, 0x64, 0x6C, 0x63,0x62,0x61,0x5E,0x5F,  //5x
	0x5E, 0x5D, 0x5A, 0x59, 0x57, 0x58, 0x56, 0x55, 0x5B, 0x54, 0x5C, 0x53,0x52,0x51,0x6E,0x6F,  //6x
	0x4E, 0x4D, 0x4A, 0x49, 0x47, 0x48, 0x46, 0x45, 0x4B, 0x44, 0x4C, 0x43,0x42,0x41,0x7E,0x7F   //7x
};




void KeyConvert(void)
{
	KeyPort = ConvTable[KeyPort];

}


#define MAX_PACKET_LENGTH	16
#define MAX_RCV_DAT_LENGTH	4
extern void PS2_Write(INT16U code);

#ifdef CPU_PIC32MZ
static INT8U keyDrvIndex = 0;
static INT8U keyRawData = 0;
static INT8U keyValidRawData = 0;
static INT16U keyDrvPressCount = 0;
static INT16U keyDrvValidCount = KEY_DRV_VALID_TIME / (KEY_DRV_OUT_PIN_NUM * KEY_DRV_PROC_TIME_INTERVAL);
#ifdef USE_LONGPRESS_KEY_OVERRIDE
INT8U keyLongkeyIn = 0;
static INT16U keyDrvLongVaildCount = KEY_DRV_LONG_VALID_TIME / (KEY_DRV_OUT_PIN_NUM * KEY_DRV_PROC_TIME_INTERVAL);
#endif

//const INT8U	KeyDrvRowTable[8] = {0xFE, 0xFD, 0xFB, 0xF7, 0xEF, 0xDF, 0xBF, 0x7F};
const INT8U KeyDrvRowATable[16] = {0xFF, 0xFD, 0xFB, 0xF7, 0xEF, 0xDF, 0x7F, 0xBF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
//portD pin7 must be high(1) for CL7000 pwr on/off
const INT8U KeyDrvRowDTable[16] = {0xBF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0xFD, 0xFB, 0xE7, 0xEF, 0xDF, 0xFF, 0xFF};
//const INT8U	KeyDrvColTable[16] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F};
const INT8U	KeyDrvColTable[16] = {0x0D, 0x0A, 0x09, 0x07, 0x08, 0x06, 0x05, 0x0B, 0x04, 0x0C, 0x03, 0x02, 0x01, 0x0E, 0x00, 0x0F};

const INT16U	keyOutScanTable[KEY_DRV_OUT_PIN_NUM] = {0xfffe, 0xfffd, 0xfffb, 0xfff7, 0xffef, 0xffdf, 0xffbf, 0xff7f, 0xfeff, 0xfdff, 0xfbff, 0xf7ff, 0xefff, 0xdfff};


void KeyBufInit(void)
{
	KeyBuf.Push = 0;
	KeyBuf.Pop = 0;
}
//********************************************************************
void KeyBufPutChar(INT8U byte)
{
	KeyBuf.Buffer[KeyBuf.Push] = byte;
	KeyBuf.Push++;
	if (KeyBuf.Push >= KEY_BUF_SIZE)
	{
		KeyBuf.Push = 0;
	}
}
//********************************************************************
INT8U KeyBufGetChar(void)
{
	INT8U byte;

	byte = KeyBuf.Buffer[KeyBuf.Pop];

	KeyBuf.Pop++;
	if (KeyBuf.Pop >= KEY_BUF_SIZE)
	{
		KeyBuf.Pop = 0;
	}

	return byte;
}
//********************************************************************
INT8U KeyBufCheck(void)
{
	if (KeyBuf.Push != KeyBuf.Pop) return TRUE;
	else return FALSE;
}
//********************************************************************

void KEY_Scan(void)//1.25 ms
{
    INT8U keyInByte;

    if (KEY_IN_1 == 0) keyInByte = 0x00;
    else if (KEY_IN_2==0) keyInByte = 0x10;
    else if (KEY_IN_3==0) keyInByte = 0x20;
    else if (KEY_IN_4==0) keyInByte = 0x30;
    else if (KEY_IN_5==0) keyInByte = 0x70;
    else if (KEY_IN_6==0) keyInByte = 0x60;
    else if (KEY_IN_7==0) keyInByte = 0x50;
    else if (KEY_IN_8==0) keyInByte = 0x40;
    else keyInByte = 0xF0;

    if (keyInByte != 0xF0)
    {
        keyRawData = keyInByte + KeyDrvColTable[keyDrvIndex];
    }
    if(keyDrvIndex == (KEY_DRV_OUT_PIN_NUM - 1))
    {
        //SerialTxBufPutChar(keyRawData);
#ifdef USE_LONGPRESS_KEY_OVERRIDE
                if (keyRawData == 0xFF) // key released, 16.25ms
                {
                    // start key check
                    if((keyValidRawData != 0xFF) && (keyDrvPressCount != 0)) // 직전 상태가 key pressed 인 경우
                    {
                        if ((keyDrvPressCount >= keyDrvValidCount) && (keyDrvPressCount < keyDrvLongVaildCount))// key input 
                            KEY_Write(keyValidRawData, 1, 0);
                        else if(keyDrvPressCount >= keyDrvLongVaildCount) // long key input
                        {
                            KEY_Write(keyValidRawData, 0, 0);
                            keyLongkeyIn = ON;
                        }
                    }// end key check
                    // key clear
                    keyValidRawData = 0xFF;
                    keyDrvPressCount = 0;
                }
                else
                {
                    if (keyValidRawData == keyRawData) // key pressed
                    {
                        keyDrvPressCount++;
                        if (keyDrvPressCount == keyDrvLongVaildCount) BuzOn(1); // long key input sound
                    }
                    else // start pressed
                    {
                        keyValidRawData = keyRawData;
                        keyDrvPressCount = 0;
                    }
                }
#else
        if (keyRawData == 0xFF)
        {
            keyValidRawData = 0xFF;
            keyDrvPressCount = 0;
        }
        else
        {
            if (keyValidRawData == keyRawData)
            {
                keyDrvPressCount++;
                if (keyDrvPressCount == keyDrvValidCount)
                {
                    //KeyBufPutChar(keyRawData);
					KEY_Write(keyRawData, 1, 0);

                }
            }
            else
            {
                keyValidRawData = keyRawData;
                keyDrvPressCount = 0;
            }
        }
#endif
        keyRawData = 0xFF;
    }

    keyDrvIndex++;
    keyDrvIndex %= KEY_DRV_OUT_PIN_NUM;
    //if(keyDrvIndex == KEY_DRV_OUT_PIN_NUM) keyDrvIndex=0;

    //KEY_OUT_A = KeyDrvRowATable[keyDrvIndex];
    //KEY_OUT_D = KeyDrvRowDTable[keyDrvIndex];
    keyOutProc(keyOutScanTable[keyDrvIndex]);
}

void keyOut_Init()
{
    KEY_OUT_1_H;
    KEY_OUT_2_H;
    KEY_OUT_3_H;
    KEY_OUT_4_H;
    KEY_OUT_5_H;
    KEY_OUT_6_H;
    KEY_OUT_7_H;
    KEY_OUT_8_H;
    KEY_OUT_9_H;
    KEY_OUT_10_H;
    KEY_OUT_11_H;
    KEY_OUT_12_H;
    KEY_OUT_13_H;
    KEY_OUT_14_H;
}

void keyOutProc(INT16U H_L)
{
    keyOut_Init();

    if (H_L == 0xfffe) KEY_OUT_1_L;
    else if (H_L == 0xfffd) KEY_OUT_2_L;
    else if (H_L == 0xfffb) KEY_OUT_3_L;
    else if (H_L == 0xfff7) KEY_OUT_4_L;
    else if (H_L == 0xffef) KEY_OUT_5_L;
    else if (H_L == 0xffdf) KEY_OUT_6_L;
    else if (H_L == 0xffbf) KEY_OUT_7_L;
    else if (H_L == 0xff7f) KEY_OUT_8_L;
    else if (H_L == 0xfeff) KEY_OUT_9_L;
    else if (H_L == 0xfdff) KEY_OUT_10_L;
    else if (H_L == 0xfbff) KEY_OUT_11_L;
    else if (H_L == 0xf7ff) KEY_OUT_12_L;
    else if (H_L == 0xefff) KEY_OUT_13_L;
    else if (H_L == 0xdfff) KEY_OUT_14_L;
}
#endif //ifdef CPU_PIC32MZ


void KEY_Write(INT16U rawcode, INT8U buzzer, INT8U isconvcodeflag)
{
	//while(KeyBuf.WriteFlag);

	KeyBuf.WriteFlag = 1;
	if (isconvcodeflag) KeyBuf.ConvBuffer[KeyBuf.Push] = rawcode;
	else KeyBuf.Buffer[KeyBuf.Push] = (INT8U)rawcode;
	KeyBuf.Buzzer[KeyBuf.Push] = buzzer;
	KeyBuf.IsConvCodeFlag[KeyBuf.Push] = isconvcodeflag;
	KeyBuf.Push++;
	if(KeyBuf.Push >= KEY_BUFFER_LEN) 
	{	
		KeyBuf.Push=0;
	}
	if(KeyBuf.Push == KeyBuf.Pop)
	{
		KeyBuf.Pop++;
	}
	KeyBuf.Pop %= KEY_BUFFER_LEN;
	KeyBuf.WriteFlag = 0;
#ifdef USE_KEY_BUZZER
	BuzOnAdd(buzzer);
#endif
	//BuzOnAdd(buzzer); //HYP Erase 20150616 fix buzzer twice on
//	sprintf(MsgBuf,"r=%d,pop=%d,push=%d,b0:%04X,b1:%04X,b2:%04X,b3:%04X\r\n", 
//		 	rawcode,KeyBuf.Pop,KeyBuf.Push,KeyBuf.Buffer[0],KeyBuf.Buffer[1],KeyBuf.Buffer[2],KeyBuf.Buffer[3]);
//	MsgOut(MsgBuf);
}

void KEY_InitKeyBuf(void)
{
	INT16S i;
	KeyBuf.Pop = 0;
	KeyBuf.Push = 0;
	KeyBuf.WriteFlag = 0;
	for(i = 0; i < KEY_BUFFER_LEN; i++)
	{
		KeyBuf.Buffer[i] = 0;
		KeyBuf.Buzzer[i] = 0;
	}
/*
	KeyBuf.Buffer[0] = 0;
*/
	KeyDrv.RawCode = 0;
	KeyDrv.CnvCode = 0;
	KeyDrv.PgmMode = 0;
#ifdef USE_CHINA_PINYIN
	KeyDrv.PhoneType=0;
#endif
	if(get_global_wparam(GLOBAL_AREA_VENDOR_COUNTRY)==COUNTRY_KR)	KeyDrv.PgmMode = 1;
	KeyDrv.Buzzer = 1;//HYP 20060428 for PS2KEY		

}

//INT16U KEY_InKey(void)
INT8U KEY_InKey(void)		// Modified in 2004.1.19 by CJK
{
	INT8U key;

	key = KeyBuf.Buffer[KeyBuf.Pop];
	KeyBuf.Buffer[KeyBuf.Pop] = 0;
	KeyDrv.Buzzer = KeyBuf.Buzzer[KeyBuf.Pop];
	KeyDrv.IsConvCodeFlag = KeyBuf.IsConvCodeFlag[KeyBuf.Pop];
	if (KeyDrv.IsConvCodeFlag) KeyDrv.CnvCode = KeyBuf.ConvBuffer[KeyBuf.Pop];
	KeyBuf.Pop++;
	if(KeyBuf.Pop >= KEY_BUFFER_LEN)
	{
		KeyBuf.Pop = 0;
	}
	return key;
/*	
	key = KeyBuf.Buffer[0];
	KeyBuf.Buffer[0] = 0;
	return key;
*/
}

/*
//INT16U KEY_InKey(void)
{
	INT16U key;

	key = KeyBuf.Buffer[KeyBuf.Pop];
	KeyBuf.Buffer[KeyBuf.Pop] = 0;
	KeyDrv.Buzzer = KeyBuf.Buzzer[KeyBuf.Pop];
	KeyBuf.Pop++;
	if(KeyBuf.Pop >= KEY_BUFFER_LEN)
	{
		KeyBuf.Pop = 0;
	}
	return key;
}
*/

INT8U KEY_IsKey(void)
{
	TcpIp_Task();
#ifdef USE_CHN_IO_BOARD
	if (UseI2CComm)
	{
		i2c_dataProc();
	}
#endif
	if(KeyBuf.Pop != KeyBuf.Push)
	{
		return 1;
	}
	return 0;
}
//-[lim060317
void KEY_Set_Caps(void)
{
	if (KeyDrv.key_caps) {
		KeyDrv.key_caps = 0;
	} else {
		KeyDrv.key_caps = 1;
	}
	if (KeyDrv.key_caps) KEY_Set_Shift(1);
	else                 KEY_Set_Shift(0);
}
//-[end

void KEY_Set_Shift(INT8U onoff)
{
	if (onoff<2) {
		KeyDrv.Shifted = onoff;
	} else {
		if(KeyDrv.Shifted) {
			KeyDrv.Shifted = 0;
		} else {
			KeyDrv.Shifted = 1;
		}
	}
	if (!KeyDrv.Shifted) KeyDrv.key_caps=0;
	if(KeyDrv.Shifted) {
		display_sign_noflush(SIGN_SHIFT,1);
	} else {
		display_sign_noflush(SIGN_SHIFT,0);
	}
	VFD7_Diffuse();
}

//extern INT16U	MsgDisplayTime;
extern char	gmsg[];
extern void CheckCashClose(void);
extern INT8U KeyFlagForDispRst;//HYP 20060703
extern INT8U DisableSignInPgmMode;
INT8U KeyCheckNetwork = ON;
INT8U UseDisplayMsg2Area = OFF;
#ifdef USE_TRACE_STANDALONE
extern INT8U	FuncKeyInputFlag;
#endif
INT8U KEY_Read(void) // Modified by CJK 20050927
{
	INT8U status;
#ifdef USE_TRACE_STANDALONE
	static INT8U scan_status = OFF;
#endif
	INT32U lp;
	
	status = FALSE;
	CheckDisplayMsg();
	CheckCashClose();

	if(KEY_IsKey()) {
		status_scale.key_last_used_100ms = SysTimer100ms;
	    KeyDrv.PrevCode = KeyDrv.RawCode;	// by CJK 20050923
	    KeyDrv.RawCode = KEY_InKey();
	    if (KeyDrv.IsConvCodeFlag) goto CONV;
	    KeyDrv.DeviceType = 0;//0=Scale Key	 HYP 20051221
	    Operation.pressKey = ON;
	    Operation.pressKeyForNvErase = ON;
	    switch (KeyDrv.keytype) {
		case 2:		// Speed Key 설정
			lp=FLASH_KEY_AREA+DFLASH_BASE;
			if(KeyDrv.Shifted) {
				KeyDrv.CnvCode = Flash_wread(lp+KeyDrv.RawCode*2 + KEY_MAX_TABLE_SIZE);
			} else {
				KeyDrv.CnvCode = Flash_wread(lp+KeyDrv.RawCode*2);
			}
			if(KeyDrv.CnvCode >= KS_PLU_01 && KeyDrv.CnvCode <= KS_PLU_200) {
				////KeyDrv.Type = KEY_TYPE_SPEEDPLU;
				goto MODE_COMMON;
			} else {
				lp = FLASH_KEY_PGM_AREA;
				if (KeyDrv.PgmMode) lp=FLASH_KEY_PGM2_AREA;
			}
			break;
		case 3:		// Clerk Key, Tare Key(TLU), Label Key 설정
			lp=FLASH_KEY_AREA+DFLASH_BASE;
			if(KeyDrv.Shifted) {
				KeyDrv.CnvCode = Flash_wread(lp+KeyDrv.RawCode*2 + KEY_MAX_TABLE_SIZE);
			} else {
				KeyDrv.CnvCode = Flash_wread(lp+KeyDrv.RawCode*2);
			}
			if(KeyDrv.CnvCode >= KS_CLERK_01 && KeyDrv.CnvCode <= KS_CLERK_64) {		//KS_CLERK_32 --> KS_CLERK_64   JJANG 20070430  
				////KeyDrv.Type = KEY_TYPE_CLERK;
				goto MODE_COMMON;
			} else if(KeyDrv.CnvCode >= KS_TARE_01 && KeyDrv.CnvCode <= KS_TARE_09) {
				////KeyDrv.Type = KEY_TYPE_TARE;
				goto MODE_COMMON;
			} else if(KeyDrv.CnvCode >= KS_LABEL_01 && KeyDrv.CnvCode <= KS_LABEL_08) {
//			} else if(KeyDrv.CnvCode >= KS_LABEL_01 && KeyDrv.CnvCode <= KS_LABEL_20) { // Max Label# Function key : 20
				////KeyDrv.Type = KEY_TYPE_TARE;
				goto MODE_COMMON;
			} 
#ifdef USE_TRACE_STANDALONE
			else if(KeyDrv.CnvCode >= KS_INDIVIDUAL_01 && KeyDrv.CnvCode <= KS_INDIVIDUAL_16) {	// Added by CJK 20060725
				////KeyDrv.Type = KEY_TYPE_INDIVIDUAL;
				goto MODE_COMMON;
			}
			else if(KeyDrv.CnvCode >= KS_SPECIALUSE_01 && KeyDrv.CnvCode <= KS_SPECIALUSE_100) {
				goto MODE_COMMON;
			} 
#endif			
			else {
				lp = FLASH_KEY_PGM_AREA;
				if (KeyDrv.PgmMode) lp=FLASH_KEY_PGM2_AREA;
			}
			break;
		case 9:		// Function Key 설정(Shift key 필요없음)
			lp=FLASH_KEY_AREA+DFLASH_BASE;
			if (Startup.scale_type==3)
				KeyDrv.CnvCode = key_hanging_map[KeyDrv.RawCode];
			else if (Startup.scale_type==5)
				KeyDrv.CnvCode = key_doublebody_map[KeyDrv.RawCode];
#if defined(USE_CL5200_KEY)
			else if (Startup.scale_type==6) // CL5200 B-type
				KeyDrv.CnvCode = key_CL5200_bench_map[KeyDrv.RawCode];
			else if (Startup.scale_type==SCALE_TYPE_HANGING_CL5200)	// CL5200 H-type
				KeyDrv.CnvCode = key_CL5200_hanging_map[KeyDrv.RawCode];
#endif
			else
				KeyDrv.CnvCode = key_pole_map[KeyDrv.RawCode];
			if (Startup.scale_type==1)
				KeyDrv.CnvCode = (KeyDrv.CnvCode - KS_PLU_25) + KS_PLU_01;
			if(KeyDrv.CnvCode >= KS_PLU_01 && KeyDrv.CnvCode <= KS_PLU_200) {
				////KeyDrv.Type = KEY_TYPE_SPEEDPLU;
				if(KeyDrv.Shifted) 
				{
					if (Startup.scale_type==1) KeyDrv.CnvCode += 48;
#if defined(USE_CL5200_KEY)
					else if (Startup.scale_type==6) KeyDrv.CnvCode += 54;
#endif
					else KeyDrv.CnvCode += 72;
				}
				goto MODE_COMMON;
			} else {
				lp = FLASH_KEY_PGM_AREA;
				if (KeyDrv.PgmMode) lp=FLASH_KEY_PGM2_AREA;
			}
			break;

		case 0: lp =FLASH_KEY_AREA;
			break;
		case 1:	lp =FLASH_KEY_PGM_AREA;
			if (KeyDrv.PgmMode) lp=FLASH_KEY_PGM2_AREA;
			break;
		}
		if(KeyDrv.Shifted) {
			lp += KEY_MAX_TABLE_SIZE;
		}
		lp+=DFLASH_BASE;
		KeyDrv.CnvCode = Flash_wread(lp+KeyDrv.RawCode*2);

MODE_COMMON:
CONV:		KeyDrv.PrevType = KeyDrv.Type;
		if(KeyDrv.CnvCode >= KS_PLU_01 && KeyDrv.CnvCode <= KS_PLU_200) {
			KeyDrv.Type = KEY_TYPE_SPEEDPLU;
		} else if(KeyDrv.CnvCode >= KP_ASC_00 && KeyDrv.CnvCode <= KP_ASC_FF) {
			KeyDrv.Type = KEY_TYPE_ASCII;
		} else if(KeyDrv.CnvCode >= KP_I_BASE && KeyDrv.CnvCode <= KP_F_BASE) {
			KeyDrv.Type = KEY_TYPE_KOREA;
		} else if(KeyDrv.CnvCode >= KEY_NUM_0 && KeyDrv.CnvCode <= KEY_NUM_10) {
			KeyDrv.Type = KEY_TYPE_NUMERIC;
		} else if(KeyDrv.CnvCode >= KS_CLERK_01 && KeyDrv.CnvCode <= KS_CLERK_64) {		//KS_CLERK_32 --> KS_CLERK_64   JJANG 20070430
			KeyDrv.Type = KEY_TYPE_CLERK;
		} else if(KeyDrv.CnvCode >= KS_CUSTOMER_01 && KeyDrv.CnvCode <= KS_CUSTOMER_12) {
			KeyDrv.Type = KEY_TYPE_CUSTOMER;
		} else if(KeyDrv.CnvCode >= KS_CURRENCY_01 && KeyDrv.CnvCode <= KS_CURRENCY_04) {	//KS_CURRENCY_12 --> KS_CURRENCY_04 JJANG 20070502
			KeyDrv.Type = KEY_TYPE_CURRENCY;
		} else if(KeyDrv.CnvCode >= KS_DEPT_01 && KeyDrv.CnvCode <= KS_DEPT_12) {
			KeyDrv.Type = KEY_TYPE_DEPT;
		} else if(KeyDrv.CnvCode >= KS_TARE_01 && KeyDrv.CnvCode <= KS_TARE_09) {
			KeyDrv.Type = KEY_TYPE_TARE;
		} else if(KeyDrv.CnvCode >= KS_LABEL_01 && KeyDrv.CnvCode <= KS_LABEL_08) {
//		} else if(KeyDrv.CnvCode >= KS_LABEL_01 && KeyDrv.CnvCode <= KS_LABEL_20) {  // Max Label# function key : 20
			KeyDrv.Type = KEY_TYPE_LABEL;
		} else if(KeyDrv.CnvCode >= KS_NUM_0 && KeyDrv.CnvCode <= KS_NUM_00) {
			KeyDrv.Type = KEY_TYPE_NUMERIC;
			KeyDrv.CnvCode = KeyDrv.CnvCode - KS_NUM_0;
		}
#ifdef USE_TRACE_STANDALONE
		else if(KeyDrv.CnvCode >= KS_INDIVIDUAL_01 && KeyDrv.CnvCode <= KS_INDIVIDUAL_16) {	// Added by CJK 20060725
			KeyDrv.Type = KEY_TYPE_INDIVIDUAL;
		}
		else if(KeyDrv.CnvCode >= KS_SPECIALUSE_01 && KeyDrv.CnvCode <= KS_SPECIALUSE_100) {	// gm 090408, 용도
			KeyDrv.Type = KEY_TYPE_SPECIALUSE;
		}
#endif		
		else if(KeyDrv.CnvCode >= KP_CODE_BASE && KeyDrv.CnvCode <= KP_CODE_END) {
			KeyDrv.Type = KEY_TYPE_CODE;
		} 
#ifdef USE_ARAB_FONT
		else if(KeyDrv.CnvCode >= KP_ARAB_00 && KeyDrv.CnvCode <= KP_ARAB_03) { 		
			KeyDrv.Type = KEY_TYPE_COMBINED_ARAB;
		} 
#endif
		else {
			KeyDrv.Type = KEY_TYPE_FUNCTION;
		}
		if(GlbOper.send_speedkey_position && KeyDrv.keytype == 0 && KeyDrv.comm_key == 0) {
			if(KeyDrv.Type != KEY_TYPE_SPEEDPLU)
				command_send_speedkey_value("F", KeyDrv.CnvCode);
		}
		status = TRUE;
		KeyFlagForDispRst = TRUE;//HYP 20060703
	} 
	else 
	{
REPT:		
		KeyDrv.CnvCode = 0;
		if (PS2_IsKey()) {
			status_scale.key_last_used_100ms = SysTimer100ms;
			scroll_message_start(0);
			KeyDrv.CnvCode=PS2_InKey();
			KeyDrv.DeviceType = 1;//0=PS/2 Key HYP 20051221
			//-----------------------------------
			//SG070122. PS2를 scanner로 사용 시
#ifdef USE_TRACE_STANDALONE
			if(UseScanner && (status_run.run_mode == RUN_SALE) && (FuncKeyInputFlag == OFF))
#else
			if(UseScanner && (status_run.run_mode == RUN_SALE))
#endif
			{
				if(KeyDrv.FirstKey) 
				{
#ifdef USE_CHECK_SCANNING_DATA
					scan_status = OFF;
#endif
					KeyDrv.FirstKey =0;
					Scanner.barcodeIndex=0;
					memset(Scanner.barcodeData, 0, SCANNER_BAR_MAX_SIZE);
				}
				
				if(KeyDrv.CnvCode >= KEY_NUM_0 && KeyDrv.CnvCode <= KEY_NUM_10) 
				{
					Scanner.barcodeData[Scanner.barcodeIndex] = KeyDrv.CnvCode +'0';
					Scanner.barcodeIndex++;
					if (Scanner.barcodeIndex >= SCANNER_BAR_MAX_SIZE) Scanner.barcodeIndex = SCANNER_BAR_MAX_SIZE - 1;	// buffer overflow 방지
					goto REPT;
				}
				else if(KeyDrv.CnvCode >= KP_ASC_20 && KeyDrv.CnvCode <= KP_ASC_7F)
				{
#ifdef USE_CHECK_SCANNING_DATA
					if (scan_status)
					{
						scan_status = OFF;
						if(KeyDrv.CnvCode == KP_ASC_J || KeyDrv.CnvCode == KP_ASC_J_L)//LF
						{
							KeyDrv.CnvCode = KS_SCANPRINT;					
							goto CONV;
						}
					}
#endif
					Scanner.barcodeData[Scanner.barcodeIndex] = KeyDrv.CnvCode - KP_ASC_00;
					Scanner.barcodeIndex++;
					if (Scanner.barcodeIndex >= SCANNER_BAR_MAX_SIZE) Scanner.barcodeIndex = SCANNER_BAR_MAX_SIZE - 1;	// buffer overflow 방지
					goto REPT;
				}
				else if(KeyDrv.CnvCode == KP_ENTER)//CR
				{
#ifdef USE_CHECK_SCANNING_DATA
					scan_status = ON;
					goto REPT;
#else
					KeyDrv.CnvCode = KS_SCANPRINT;					
					goto CONV;				
#endif
				}
				else  goto CONV;
			
			}//----------------------------------
			else //normal process
			{					
				BuzOnAdd(1);
				goto CONV;
			}
		} 
		else 
		{
			if (KeyCheckNetwork)
			{
				network_check();
			}
		}
	}
	if (status_run.run_mode != RUN_SALE || UseDisplayMsg2Area) 
	{
		static INT16U StartTimer100ms = 0;
		
		if(status) DisplayMsgReset();
		sale_display_msg2();
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
		if(status_run.run_mode == RUN_PROGRAM) DisableSignInPgmMode = ON;
        else if(status_run.run_mode != RUN_CALIBRATION) DisableSignInPgmMode = OFF; 
#endif
		if (!DisableSignInPgmMode)
		{
			sale_display_ethernet_sign();
			common_display_sign();
		}
#if !defined(_USE_LCD20848_) && !defined(_USE_LCD32_)
		if ((INT16U)(SysTimer100ms - StartTimer100ms) >= 2)
		{
			StartTimer100ms = SysTimer100ms;
			VFD7_Diffuse();
		}
#endif
	}
	return status;
}

void KEY_CopyPS2SelfKeyTable(void)
{
	INT16S i;
	INT32U addr;
	INT8U temp;
//	INT8U pole;

//	pole = get_global_bparam(GLOBAL_AREA_SCALETYPE);
//	if(pole!=4) return;	

	temp=get_global_bparam(GLOBAL_SALE_SETUP_FLAG7);
	temp=(temp>>1)&0x01;

	if(temp)
	{
		addr = DFLASH_BASE+FLASH_PSKEY_AREA;
		for (i=0; i<PS2_KEY_MAX_NUM; i++) {
			Flash_wwrite(addr,prog_ps2_code_setS_self[i]);
			addr+=2;
		}
	}
	else
	{
		addr = DFLASH_BASE+FLASH_PSKEY_AREA;
		for (i=0; i<PS2_KEY_MAX_NUM; i++) {
			Flash_wwrite(addr,prog_ps2_code_setS_kor[i]);
			addr+=2;
		}
	}

}


//extern INT8U USE_SELF_PS2KEY_MAP; //SG071121

void KEY_CopyKeyTable(INT16U country)
{
//	INT16S y,x,z,w,q;
	INT16S x;    
//	INT16U id;
	INT16S i;
	INT8U pole;
//	CAPTION_STRUCT cap;
	INT32U addr;
//	INT16U *key_any_map;
	INT8U language_flag;
//	INT8U temp;

#ifndef USE_CHN_FONT
	language_flag = get_global_bparam(GLOBAL_SALE_SETUP_FLAG4)&0x08;
#endif
	if (country == 0xffff) 
	{
		country = get_global_wparam(GLOBAL_AREA_VENDOR_COUNTRY);
	}
	if (country > COUNTRY_CN) 
	{
		country = 0;//COUNTRY_UN
	}

	for (i = 0; i < KEY_MAX_TABLE_SIZE / 2; i++) 
	{
		switch (country) 
		{
#ifdef USE_COUNTRY_KEY_IN_SOURCE
			case COUNTRY_KR: 
				normal_key_code_set[i] = normal_key_code_set_kor[i];
				normal_key_code_set_shifted[i] = normal_key_code_set_un_shifted[i];
				prog_key_code_setA[i] = prog_key_code_setA_kor[i];
				prog_key_code_setA_shifted[i] = prog_key_code_setA_kor_shifted[i];
				prog_key_code_setB[i] = prog_key_code_setB_kor[i];
				prog_key_code_setB_shifted[i] = prog_key_code_setB_kor_shifted[i];
				break;
#endif //#ifdef USE_COUNTRY_KEY_IN_SOURCE
			default:
			case COUNTRY_EU:
			case COUNTRY_UN:
				normal_key_code_set[i] = normal_key_code_set_un[i];
				normal_key_code_set_shifted[i] = normal_key_code_set_un_shifted[i];
				prog_key_code_setA[i] = prog_key_code_setA_un[i];
				prog_key_code_setA_shifted[i] = prog_key_code_setA_un_shifted[i];
				prog_key_code_setB[i] = prog_key_code_setB_un[i];
				prog_key_code_setB_shifted[i] = prog_key_code_setB_un_shifted[i];
				break;
			case COUNTRY_CN:
				normal_key_code_set[i]=normal_key_code_set_cn[i];
				normal_key_code_set_shifted[i]=normal_key_code_set_cn_shifted[i];
				prog_key_code_setA[i]=prog_key_code_setA_cn[i];
				prog_key_code_setA_shifted[i]=prog_key_code_setA_cn_shifted[i];
				prog_key_code_setB[i]=prog_key_code_setA_cn[i];
				prog_key_code_setB_shifted[i]=prog_key_code_setA_cn_shifted[i];
				break;
#ifdef USE_COUNTRY_KEY_IN_SOURCE
			case COUNTRY_US:
				normal_key_code_set[i] = normal_key_code_set_us[i];
				normal_key_code_set_shifted[i] = normal_key_code_set_us_shifted[i];
				prog_key_code_setA[i] = prog_key_code_setA_us[i];
				prog_key_code_setA_shifted[i] = prog_key_code_setA_us_shifted[i];
#ifdef USE_CHN_FONT
				prog_key_code_setB[i] = prog_key_code_setA_us[i];
				prog_key_code_setB_shifted[i] = prog_key_code_setA_us_shifted[i];
#else
				if(language_flag)
				{
					prog_key_code_setB[i] = prog_key_code_setB_kor[i];
					prog_key_code_setB_shifted[i] = prog_key_code_setB_kor_shifted[i];
				}
				else
				{
					prog_key_code_setB[i] = prog_key_code_setA_us[i];
					prog_key_code_setB_shifted[i] = prog_key_code_setA_us_shifted[i];
				}
#endif
				break;
			case COUNTRY_EUT:
				normal_key_code_set[i] = normal_key_code_set_euro[i];
				normal_key_code_set_shifted[i] = normal_key_code_set_un_shifted[i];
				prog_key_code_setA[i] = prog_key_code_setA_un[i];
				prog_key_code_setA_shifted[i] = prog_key_code_setA_un_shifted[i];
				prog_key_code_setB[i] = prog_key_code_setB_un[i];
				prog_key_code_setB_shifted[i] = prog_key_code_setB_un_shifted[i];
				break;
			case COUNTRY_RU:
				normal_key_code_set[i]=normal_key_code_set_un[i];
				normal_key_code_set_shifted[i]=normal_key_code_set_un_shifted[i];
				prog_key_code_setA[i]=prog_key_code_setA_ru[i];
				prog_key_code_setA_shifted[i]=prog_key_code_setA_ru_shifted[i];
				prog_key_code_setB[i]=prog_key_code_setB_ru[i];
				if ((prog_key_code_setB_ru[i] >= KP_ASC_A) && (prog_key_code_setB_ru[i] <= KP_ASC_Z)) 
				{
					prog_key_code_setB_shifted[i]=prog_key_code_setB_ru[i]-KP_ASC_A+KP_ASC_A_L;
				}
				else
				{
					prog_key_code_setB_shifted[i]=prog_key_code_setB_ru[i];
				}
				break;
			case COUNTRY_ETC:
				normal_key_code_set[i]=normal_key_code_set_un[i];
				normal_key_code_set_shifted[i]=normal_key_code_set_un_shifted[i];
				prog_key_code_setA[i]=prog_key_code_setA_un[i];
				prog_key_code_setA_shifted[i]=prog_key_code_setA_un_shifted[i];
				prog_key_code_setB[i]=prog_key_code_setA_un[i];
				prog_key_code_setB_shifted[i]=prog_key_code_setA_un_shifted[i];
				break;
#endif //#ifdef USE_COUNTRY_KEY_IN_SOURCE
		}
		if (normal_key_code_set_shifted[i]== 0xffff) normal_key_code_set_shifted[i]=normal_key_code_set[i];
		if (prog_key_code_setB[i]         == 0xffff) prog_key_code_setB[i]         =prog_key_code_setA[i];
		if (prog_key_code_setA_shifted[i] == 0xffff) prog_key_code_setA_shifted[i] =prog_key_code_setA[i];
		if (prog_key_code_setB_shifted[i] == 0xffff) prog_key_code_setB_shifted[i] =prog_key_code_setB[i];
	}
	pole = get_global_bparam(GLOBAL_AREA_SCALETYPE);
	if ((pole < SCALE_TYPE_BENCH) || (pole > SCALE_TYPE_END))
	{
		pole = SCALE_TYPE_BENCH;
	}
//	if (caption_split_by_code(0x9171, &cap,0)) 
//    {
//		if ((pole < (INT8U)cap.input_min) || (pole > (INT8U)cap.input_max))
//       	{
//           		pole=1;
//       	}
//	}

	if (pole == SCALE_TYPE_SELF) //self key map
	{
		for (i = 0; i < KEY_MAX_TABLE_SIZE / 2; i++) 
		{
			normal_key_code_set[i] = normal_self_key_code_set_un[i];
			normal_key_code_set_shifted[i] = normal_self_key_code_set_un[i];
		}
		Flash_swrite(DFLASH_BASE+FLASH_KEY_AREA 	  , (HUGEDATA INT8U *)normal_key_code_set	 , KEY_MAX_TABLE_SIZE);
		Flash_swrite(DFLASH_BASE+FLASH_KEY_SHIFT_AREA	  , (HUGEDATA INT8U *)normal_key_code_set_shifted, KEY_MAX_TABLE_SIZE);
		Flash_swrite(DFLASH_BASE+FLASH_KEY_PGM_AREA	  , (HUGEDATA INT8U *)prog_key_code_setA	 , KEY_MAX_TABLE_SIZE);
		Flash_swrite(DFLASH_BASE+FLASH_KEY_PGM_SHIFT_AREA , (HUGEDATA INT8U *)prog_key_code_setA_shifted , KEY_MAX_TABLE_SIZE);
		Flash_swrite(DFLASH_BASE+FLASH_KEY_PGM2_AREA	  , (HUGEDATA INT8U *)prog_key_code_setB	 , KEY_MAX_TABLE_SIZE);
		Flash_swrite(DFLASH_BASE+FLASH_KEY_PGM2_SHIFT_AREA, (HUGEDATA INT8U *)prog_key_code_setB_shifted , KEY_MAX_TABLE_SIZE);
	}
	else
	{
		// Bench Type Key
		Flash_swrite(DFLASH_BASE+FLASH_KEY_AREA 	  , (HUGEDATA INT8U *)normal_key_code_set	 , KEY_MAX_TABLE_SIZE);
		Flash_swrite(DFLASH_BASE+FLASH_KEY_SHIFT_AREA	  , (HUGEDATA INT8U *)normal_key_code_set_shifted, KEY_MAX_TABLE_SIZE);
		Flash_swrite(DFLASH_BASE+FLASH_KEY_PGM_AREA	  , (HUGEDATA INT8U *)prog_key_code_setA	 , KEY_MAX_TABLE_SIZE);
		Flash_swrite(DFLASH_BASE+FLASH_KEY_PGM_SHIFT_AREA , (HUGEDATA INT8U *)prog_key_code_setA_shifted , KEY_MAX_TABLE_SIZE);
		Flash_swrite(DFLASH_BASE+FLASH_KEY_PGM2_AREA	  , (HUGEDATA INT8U *)prog_key_code_setB	 , KEY_MAX_TABLE_SIZE);
		Flash_swrite(DFLASH_BASE+FLASH_KEY_PGM2_SHIFT_AREA, (HUGEDATA INT8U *)prog_key_code_setB_shifted , KEY_MAX_TABLE_SIZE);

		// Convert B-type to any-type
		KEY_CnvTable(SCALE_TYPE_BENCH, pole, 0x3f);
	}


/*	
	if (pole == 1) 
    {
	} 
    	else if (pole == 2) 
    	{
		key_any_map = key_pole_map;
		for (y = 0; y < 8; y++) 
		{
			for (x = 0; x < 16; x++) 
			{
				i = y * 16 + x;
				id = key_any_map[i] - KS_PLU_01;
				if ((key_any_map[i] >= KS_PLU_01) && (key_any_map[i] <= KS_PLU_200)) 
				{
					normal_key_code_set[i]         = key_any_map[i];
					normal_key_code_set_shifted[i] = key_any_map[i]-KS_PLU_01+KS_PLU_73;
					z = id / 8;
					w = id % 8;
					q = (z + 2) * 16 + w + 1;
					prog_key_code_setA[i]         = prog_key_code_setA[q];
					prog_key_code_setB[i]         = prog_key_code_setB[q];
					prog_key_code_setA_shifted[i] = prog_key_code_setA_shifted[q];
					prog_key_code_setB_shifted[i] = prog_key_code_setB_shifted[q];
				}
			}
		}
		for (y = 5; y < 8; y++) 
		{
			for (x = 0; x < 16; x++) 
			{
				i = y * 16 + x;
				if ((x >= 1) && (x <= 8)) 
				{
					prog_key_code_setA[i]         = 0xffff;
					prog_key_code_setB[i]         = 0xffff;
					prog_key_code_setA_shifted[i] = 0xffff;
					prog_key_code_setB_shifted[i] = 0xffff;
				}
			}
		}
	} 
	else if (pole == 3) //hanging
	{
		for (y = 0; y < 8; y++) 
		{
			for (x = 0; x < 16; x++) 
			{
				i = y * 16 + x;
				if ((x >= 1) && (x <= 8)) 
				{
					prog_key_code_setA[i]         = 0xffff;
					prog_key_code_setB[i]         = 0xffff;
					prog_key_code_setA_shifted[i] = 0xffff;
					prog_key_code_setB_shifted[i] = 0xffff;
				}
			}
		}
		key_any_map = key_hanging_map;
		for (y = 0; y < 8; y++) 
		{
			for (x = 0; x < 16; x++) 
			{
				i = y * 16 + x;
				id = key_any_map[i] - KS_PLU_01;
				if ((key_any_map[i] >= KS_PLU_01) && (key_any_map[i] <= KS_PLU_200)) 
				{
					normal_key_code_set[i]         = key_any_map[i];
					normal_key_code_set_shifted[i] = key_any_map[i]-KS_PLU_01+KS_PLU_73;
					z = id / 8;
					w = id % 8;
					q = (z + 2) * 16 + w + 1;

					switch (country)
					{
						case COUNTRY_KR:
							prog_key_code_setA[i]         = prog_key_code_setA_htype_kor[id];//prog_key_code_setA[q];
							prog_key_code_setB[i]         = prog_key_code_setB_htype_kor[id];//prog_key_code_setB[q];
							prog_key_code_setA_shifted[i] = prog_key_code_setA_htype_kor_shifted[id];//prog_key_code_setA_shifted[q];
							prog_key_code_setB_shifted[i] = prog_key_code_setB_htype_kor_shifted[id];//prog_key_code_setB_shifted[q];
							break;
						case COUNTRY_US:
						case COUNTRY_UN:
						default:
							prog_key_code_setA[i]         = prog_key_code_setA_htype_un[id];//prog_key_code_setA[q];
							prog_key_code_setB[i]         = prog_key_code_setA_htype_un[id];//prog_key_code_setB[q];
							prog_key_code_setA_shifted[i] = prog_key_code_setA_htype_un_shifted[id];//prog_key_code_setA_shifted[q];
							prog_key_code_setB_shifted[i] = prog_key_code_setA_htype_un_shifted[id];//prog_key_code_setB_shifted[q];
							break;
					}
				}
			}
		}
	}
	else if(pole==4) //self key map
	{
		for (i = 0; i < KEY_MAX_TABLE_SIZE / 2; i++) 
		{
			normal_key_code_set[i] = normal_self_key_code_set_un[i];
			normal_key_code_set_shifted[i] = normal_self_key_code_set_un[i];
		}
	}
	else if (pole == 5) //d-type
	{
		for (y = 0; y < 8; y++) 
		{
			for (x = 0; x < 16; x++) 
			{
				i = y * 16 + x;
				if ((x >= 1) && (x <= 8)) 
				{
					prog_key_code_setA[i]         = 0xffff;
					prog_key_code_setB[i]         = 0xffff;
					prog_key_code_setA_shifted[i] = 0xffff;
					prog_key_code_setB_shifted[i] = 0xffff;
				}
			}
		}
		key_any_map = key_hanging_map;
		for (y = 0; y < 8; y++) 
		{
			for (x = 0; x < 16; x++) 
			{
				i = y * 16 + x;
				id = key_any_map[i] - KS_PLU_01;
				if ((key_any_map[i] >= KS_PLU_01) && (key_any_map[i] <= KS_PLU_200)) 
				{
					normal_key_code_set[i]         = key_any_map[i];
					normal_key_code_set_shifted[i] = key_any_map[i]-KS_PLU_01+KS_PLU_73;
					z = id / 8;
					w = id % 8;
					q = (z + 2) * 16 + w + 1;

					switch (country)
					{
						case COUNTRY_KR:
							prog_key_code_setA[i]         = prog_key_code_setA_htype_kor[id];//prog_key_code_setA[q];
							prog_key_code_setB[i]         = prog_key_code_setB_htype_kor[id];//prog_key_code_setB[q];
							prog_key_code_setA_shifted[i] = prog_key_code_setA_htype_kor_shifted[id];//prog_key_code_setA_shifted[q];
							prog_key_code_setB_shifted[i] = prog_key_code_setB_htype_kor_shifted[id];//prog_key_code_setB_shifted[q];
							break;
						case COUNTRY_US:
						case COUNTRY_UN:
						default:
							prog_key_code_setA[i]         = prog_key_code_setA_htype_un[id];//prog_key_code_setA[q];
							prog_key_code_setB[i]         = prog_key_code_setA_htype_un[id];//prog_key_code_setB[q];
							prog_key_code_setA_shifted[i] = prog_key_code_setA_htype_un_shifted[id];//prog_key_code_setA_shifted[q];
							prog_key_code_setB_shifted[i] = prog_key_code_setA_htype_un_shifted[id];//prog_key_code_setB_shifted[q];
							break;
					}
				}
			}
		}

		//F1~F4 function key initial as CLERK_KEY
		//RawCode : F1=0x1b, F2=0x1c, F3=0x1d, F4=0x1e
		if(pole == 5)
		{
			normal_key_code_set[0x1b] = KS_CLERK_01;
			normal_key_code_set[0x1c] = KS_CLERK_02;
			normal_key_code_set[0x1d] = KS_CLERK_03;
			normal_key_code_set[0x1e] = KS_CLERK_04;
		}
	}

  	Flash_swrite(DFLASH_BASE+FLASH_KEY_AREA           , (HUGEDATA INT8U *)normal_key_code_set        , KEY_MAX_TABLE_SIZE);
  	Flash_swrite(DFLASH_BASE+FLASH_KEY_SHIFT_AREA     , (HUGEDATA INT8U *)normal_key_code_set_shifted, KEY_MAX_TABLE_SIZE);
  	Flash_swrite(DFLASH_BASE+FLASH_KEY_PGM_AREA       , (HUGEDATA INT8U *)prog_key_code_setA         , KEY_MAX_TABLE_SIZE);
  	Flash_swrite(DFLASH_BASE+FLASH_KEY_PGM_SHIFT_AREA , (HUGEDATA INT8U *)prog_key_code_setA_shifted , KEY_MAX_TABLE_SIZE);
  	Flash_swrite(DFLASH_BASE+FLASH_KEY_PGM2_AREA      , (HUGEDATA INT8U *)prog_key_code_setB         , KEY_MAX_TABLE_SIZE);
  	Flash_swrite(DFLASH_BASE+FLASH_KEY_PGM2_SHIFT_AREA, (HUGEDATA INT8U *)prog_key_code_setB_shifted , KEY_MAX_TABLE_SIZE);
*/

	// Speed Key Setting
	for (x = 0; x < 5; x++)
	{
		addr=DFLASH_BASE+FLASH_KEY_SPEED_AREA+(MAX_SPEEDKEY_SIZE*x);
		for (i=0; i<160; i++) {
			Flash_lwrite(addr,i+1);
			addr+=4;
		}
	}

//	if(pole == 4)
//	{
//		addr=DFLASH_BASE+FLASH_KEY_SPEED_AREA+(MAX_SPEEDKEY_SIZE*4);
//		for (i=0; i<160; i++) {
//			Flash_lwrite(addr,i+1);
//			addr+=4;
//		}
//	}

	// Clerk Key Setting
	addr=DFLASH_BASE+FLASH_KEY_CDTC_AREA;
	for (i=0; i<64; i++) {		//32 --> 64 JJANG 20070430
		Flash_wwrite(addr,i+1);
		addr+=2;
	}
	// Tare Key Setting		//addr=DFLASH_BASE+FLASH_KEY_CDTC_AREA+128;	Dept --> Tare	JJANG 20070502
	for (i=0; i<32; i++) {
		Flash_wwrite(addr,i+1);
		addr+=2;
	}
	// Dept Key Setting		//addr=DFLASH_BASE+FLASH_KEY_CDTC_AREA+192;	Tare --> Dept 	JJANG 20070502
	for (i=0; i<32; i++) {
		Flash_wwrite(addr,i+1);
		addr+=2;
	}

#ifdef USE_TRACE_STANDALONE
	addr = FLASH_TRACE_STANDALONE_KEY_AREA + DFLASH_BASE;
	// Individual Key Setting
	for (i=0; i<16; i++) {
		Flash_bwrite(addr,i+1);
		addr+=1;
	}
#endif

//HYP 20051220 ps2key table init
	addr = DFLASH_BASE+FLASH_PSKEY_AREA;
	for (i=0; i<PS2_KEY_MAX_NUM; i++) {
		Flash_wwrite(addr,prog_ps2_code_setS_kor[i]);
		addr+=2;
	}

	//===================================================================================
	//SG071126.PS2 셀프키 사용을 위한 key map 변경 . 파라미터로 추가해야 함.(Param623)
	//temp=get_global_bparam(GLOBAL_SALE_SETUP_FLAG7);
	//temp=(temp>>1)&0x01;
	//if((pole==4) && temp)
	if(pole==4)
	{
		addr = DFLASH_BASE+FLASH_PSKEY_AREA;
		for (i=0; i<PS2_KEY_MAX_NUM; i++) {
			Flash_wwrite(addr,prog_ps2_code_setS_self[i]);
			addr+=2;
		}
	}
	//===================================================================================

	addr = DFLASH_BASE+FLASH_PSKEY_SHIFT_AREA;
	for (i=0; i<PS2_KEY_MAX_NUM; i++) {
		Flash_wwrite(addr,prog_ps2_code_setS_kor_shifted[i]);
		addr+=2;
	}
	addr = DFLASH_BASE+FLASH_PSKEY_PGM_AREA;
	for (i=0; i<PS2_KEY_MAX_NUM; i++) {
		Flash_wwrite(addr,prog_ps2_code_setA_kor[i]);
		addr+=2;
	}
	addr = DFLASH_BASE+FLASH_PSKEY_PGM_SHIFT_AREA;
	for (i=0; i<PS2_KEY_MAX_NUM; i++) {
		Flash_wwrite(addr,prog_ps2_code_setA_kor_shifted[i]);
		addr+=2;
	}
	addr = DFLASH_BASE+FLASH_PSKEY_PGM2_AREA;
	for (i=0; i<PS2_KEY_MAX_NUM; i++) {
//		if(country == COUNTRY_KR) {//default is only for korea
		if(country == COUNTRY_KR || language_flag) {	//Modified by JJANG 20090220
			Flash_wwrite(addr,prog_ps2_code_setB_kor[i]);
		} else {
			Flash_wwrite(addr,prog_ps2_code_setA_kor[i]);
		}
		addr+=2;
	}
	addr = DFLASH_BASE+FLASH_PSKEY_PGM2_SHIFT_AREA;
	for (i=0; i<PS2_KEY_MAX_NUM; i++) {
//		if(country == COUNTRY_KR) {//default is only for korea
		if(country == COUNTRY_KR || language_flag) {
			Flash_wwrite(addr,prog_ps2_code_setB_kor_shifted[i]);
		} else {
			Flash_wwrite(addr,prog_ps2_code_setA_kor_shifted[i]);
		}
		addr+=2;
	}
}


#define KEY_TABLE_SELECT_SALE	0x01
#define KEY_TABLE_SELECT_SALE_S	0x02
#define KEY_TABLE_SELECT_PRGA	0x04
#define KEY_TABLE_SELECT_PRGA_S	0x08
#define KEY_TABLE_SELECT_PRGB	0x10
#define KEY_TABLE_SELECT_PRGB_S	0x20

// Key Table Version : Table 가장 첫 2bytes
//  (1) 0xAyzz 형식 : 1회 다운로드로 scale type을 자유롭게 변경가능
//                    ex) P-type : 0xA201
//       x  : key type (1:B-type, 2:P-type,R-type, 3:H-type)
//       zz : version
//  (2) 0xffff 형식 : version관리 이전 버전. scale type에 상관없이 다운로드된 table 적용
//               scale type 변경시 ROM CODE에 있는 기본치로 변환됨
extern INT8U Flash_check_nvbuffer_4_rd(INT32U start_addr, INT32U byte_count);
INT8U KEY_CnvTable(INT8U oldType, INT8U newType, INT8U tableSelect)
{
	INT16U newTable[128];
#ifdef CPU_PIC32MZ
	INT16U oldTable[1];
#else
	INT16U *oldTable;
#endif
	INT8U *cnvTable;
	INT16U i;
	INT16U j;
	INT8U sel;
	INT32U fAddr;
	INT8U tableNo;
	INT8U oldTypeTemp;

	//Flash_flush_nvbuffer();

	for (sel = 0; sel < 6; sel++)
	{
		tableNo = (tableSelect & (0x01 << sel));
	// Pointer Assign //
		fAddr = DFLASH_BASE;
		switch (tableNo)
		{
			case KEY_TABLE_SELECT_SALE:
				fAddr += FLASH_KEY_AREA;
				break;
			case KEY_TABLE_SELECT_SALE_S:
				fAddr += FLASH_KEY_SHIFT_AREA;
				break;
			case KEY_TABLE_SELECT_PRGA:
				fAddr += FLASH_KEY_PGM_AREA;
				break;
			case KEY_TABLE_SELECT_PRGA_S:
				fAddr += FLASH_KEY_PGM_SHIFT_AREA;
				break;
			case KEY_TABLE_SELECT_PRGB:
				fAddr += FLASH_KEY_PGM2_AREA;
				break;
			case KEY_TABLE_SELECT_PRGB_S:
				fAddr += FLASH_KEY_PGM2_SHIFT_AREA;
				break;
			default:
				continue;
		}

		if(Flash_check_nvbuffer_4_rd(fAddr, KEY_MAX_TABLE_SIZE) == 1)	// IN_NVRAM
		{
			Flash_flush_nvbuffer();
		}

		oldTable[0] = Flash_wread(fAddr);

		if ((oldTable[0]&0xf000) != 0xa000)	// Key Table 내의 table version 체크(0xAXXX)
		{
			return 2;
		}
		oldTypeTemp = oldType;
		if (!oldType)	// oldType = 0 : Key Table 내의 key type 사용
		{
			oldTypeTemp = (INT8U)((oldTable[0] & 0x0f00) >> 8);
		}
		if (!newType)	// newType = 0 : MENU8220에서 설정된 scale type 사용
		{
			newType = get_global_bparam(GLOBAL_AREA_SCALETYPE);
		}

		if (oldTypeTemp < SCALE_TYPE_BENCH || oldTypeTemp > SCALE_TYPE_END)
		{
			return 1;
		}
		if (newType < SCALE_TYPE_BENCH || newType > SCALE_TYPE_END)
		{
			return 1;
		}

	// Convert [old] to Pole
	#ifdef _USE_UNDER_SELFKEY_	// self 48-key 사용시 B-type키와 동일
		if (oldTypeTemp != SCALE_TYPE_POLE)
	#else
		if (oldTypeTemp != SCALE_TYPE_POLE && oldTypeTemp != SCALE_TYPE_SELF)
	#endif
		{
			switch (oldTypeTemp)
			{
				case SCALE_TYPE_BENCH_CL3000:
					if (tableNo & (KEY_TABLE_SELECT_SALE | KEY_TABLE_SELECT_SALE_S))
					{
						cnvTable = KEY_CnvPtoB_Sale_CL3000;
					}
					else
					{
						cnvTable = KEY_CnvPtoB_Prog_CL3000;
					}
					break;
				case SCALE_TYPE_BENCH_CL5200:
					if (tableNo & (KEY_TABLE_SELECT_SALE | KEY_TABLE_SELECT_SALE_S))
					{
						cnvTable = KEY_CnvPtoB_Sale_CL5200;
					}
					else
					{
						cnvTable = KEY_CnvPtoB_Prog_CL5200;
					}
					break;
				case SCALE_TYPE_HANGING_CL5200:
					if (tableNo & (KEY_TABLE_SELECT_SALE | KEY_TABLE_SELECT_SALE_S))
					{
						cnvTable = KEY_CnvPtoH_Sale_CL5200;
					}
					else
					{
						cnvTable = KEY_CnvPtoH_Prog_CL5200;
					}
					break;
				case SCALE_TYPE_BENCH:
			#ifdef _USE_UNDER_SELFKEY_	// self 48-key 사용시 B-type키와 동일
				case SCALE_TYPE_SELF:
			#endif
					if (tableNo & (KEY_TABLE_SELECT_SALE | KEY_TABLE_SELECT_SALE_S))
					{
						cnvTable = KEY_CnvPtoB_Sale;
					}
					else
					{
						cnvTable = KEY_CnvPtoB_Prog;
					}
					break;
				case SCALE_TYPE_HANGING:
				case SCALE_TYPE_DOUBLEBODY:
					if (tableNo & (KEY_TABLE_SELECT_SALE | KEY_TABLE_SELECT_SALE_S))
					{
						cnvTable = KEY_CnvPtoH_Sale;
					}
					else
					{
						cnvTable = KEY_CnvPtoH_Prog;
					}
					break;
			}
			for (i = 0; i < 128; i++)
			{
				for (j = 0; j < 128; j++)
				{
					if (cnvTable[j] == i) 
					{
						break;
					}
				}
				if (j >= 128)
				{
					j = 0;
				}
				newTable[j] = Flash_wread(fAddr + i*2);
				if (tableNo & KEY_TABLE_SELECT_SALE_S)
				{
					if (newTable[j] >= KS_PLU_01 && newTable[j] <= KS_PLU_200)
					{
						if (oldTypeTemp == SCALE_TYPE_BENCH)
						{
							newTable[j] = newTable[j] + (KS_PLU_73 - KS_PLU_49);
						}
						else if (oldTypeTemp == SCALE_TYPE_HANGING)
						{
							//newTableSaleShift[j] = newTableSaleShift[j] + (KS_PLU_73 - KS_PLU_49);
						}
						else if (oldTypeTemp == SCALE_TYPE_BENCH_CL5200)
						{
							newTable[j] = newTable[j] + (KS_PLU_73 - KS_PLU_55); 
						}
					}
				}
			}
			//newTable[0] &= 0xf0ff;
			//newTable[0] |= (SCALE_TYPE_POLE << 8);	// Pole Type

	// Write Flash Temp Table //
		  	Flash_swrite(fAddr, (HUGEDATA INT8U *)newTable, KEY_MAX_TABLE_SIZE);
		}
		else
		{
			newTable[0] = Flash_wread(fAddr);
		}
		newTable[0] &= 0xf0ff;
		newTable[0] |= (SCALE_TYPE_POLE << 8);	// Pole Type
	// Write Flash, scale type(2bytes) //
		Flash_swrite(fAddr, (HUGEDATA INT8U *)newTable, 2);


	// Convert Pole to [new] //
	#ifdef _USE_UNDER_SELFKEY_	// self 48-key 사용시 B-type키와 동일
		if (newType != SCALE_TYPE_POLE)
	#else
		if (newType != SCALE_TYPE_POLE && newType != SCALE_TYPE_SELF)
	#endif
		{
			switch (newType)
			{
				case SCALE_TYPE_BENCH_CL3000:
					if (tableNo & (KEY_TABLE_SELECT_SALE | KEY_TABLE_SELECT_SALE_S))
					{
						cnvTable = KEY_CnvPtoB_Sale_CL3000;
					}
					else
					{
						cnvTable = KEY_CnvPtoB_Prog_CL3000;
					}
					break;
				case SCALE_TYPE_BENCH_CL5200:
					if (tableNo & (KEY_TABLE_SELECT_SALE | KEY_TABLE_SELECT_SALE_S))
					{
						cnvTable = KEY_CnvPtoB_Sale_CL5200;
					}
					else
					{
						cnvTable = KEY_CnvPtoB_Prog_CL5200;
					}
					break;
				case SCALE_TYPE_HANGING_CL5200:
					if (tableNo & (KEY_TABLE_SELECT_SALE | KEY_TABLE_SELECT_SALE_S))
					{
						cnvTable = KEY_CnvPtoH_Sale_CL5200;
					}
					else
					{
						cnvTable = KEY_CnvPtoH_Prog_CL5200;
					}
					break;					
				case SCALE_TYPE_BENCH:
			#ifdef _USE_UNDER_SELFKEY_	// self 48-key 사용시 B-type키와 동일
				case SCALE_TYPE_SELF:
			#endif
					if (tableNo & (KEY_TABLE_SELECT_SALE | KEY_TABLE_SELECT_SALE_S))
					{
						cnvTable = KEY_CnvPtoB_Sale;
					}
					else
					{
						cnvTable = KEY_CnvPtoB_Prog;
					}
					break;
				case SCALE_TYPE_HANGING:
				case SCALE_TYPE_DOUBLEBODY:
					if (tableNo & (KEY_TABLE_SELECT_SALE | KEY_TABLE_SELECT_SALE_S))
					{
						cnvTable = KEY_CnvPtoH_Sale;
					}
					else
					{
						cnvTable = KEY_CnvPtoH_Prog;
					}
					break;
			}
	//sprintf(MsgBuf, "<%lx,%lx>", &cnvSaleTable[0], &oldTableSale[0]);
	//MsgOut(MsgBuf);
			//Flash_flush_nvbuffer();
			if(Flash_check_nvbuffer_4_rd(fAddr, KEY_MAX_TABLE_SIZE) == 1)	// IN_NVRAM 
			{
				Flash_flush_nvbuffer();
			}
			for (i = 0; i < 128; i++)
			{
				newTable[cnvTable[i]] = Flash_wread(fAddr + i*2 );
				if (tableNo & KEY_TABLE_SELECT_SALE_S)
				{
					if (newTable[cnvTable[i]] >= KS_PLU_01 && newTable[cnvTable[i]] <= KS_PLU_200)
					{
						if (newType == SCALE_TYPE_BENCH)
						{
							newTable[cnvTable[i]] = newTable[cnvTable[i]] - (KS_PLU_73 - KS_PLU_49);
						}
						else if (newType == SCALE_TYPE_HANGING)
						{
							//newTableSaleShift[cnvSaleTable[i]] = newTableSaleShift[cnvSaleTable[i]] + (KS_PLU_73 - KS_PLU_49);
						}
						else if (newType == SCALE_TYPE_BENCH_CL5200)
						{
							newTable[cnvTable[i]] = newTable[cnvTable[i]] - (KS_PLU_73 - KS_PLU_55);
						}
					}
				}
			}
			//newTable[0] &= 0xf0ff;
			//newTable[0] |= (newType << 8);	// Pole Type
	// Write Flash Temp Table //
		  	Flash_swrite(fAddr, (HUGEDATA INT8U *)newTable, KEY_MAX_TABLE_SIZE);
		}
		else
		{
			newTable[0] = Flash_wread(fAddr);	// 이전 버전
		}
		newTable[0] &= 0xf0ff;
		newTable[0] |= (newType << 8);	// Pole Type
	// Write Flash, scale type(2bytes) //
		Flash_swrite(fAddr, (HUGEDATA INT8U *)newTable, 2);
	}
	return 0;
}

void KEY_InitTable(void)
{
//	HUGEDATA INT16U *p;
//	p = (HUGEDATA INT16U *)KEY_NORMAL_TABLE_ADDR + 0x7f;

	INT32U p;
	INT16U id,country;

	p= KEY_NORMAL_TABLE_ADDR + 0x7f*2;
	id=Flash_wread(p);
	if(id != KEY_IDENTIFY){	// Key table 최초 셋팅
		country=get_global_wparam(GLOBAL_AREA_VENDOR_COUNTRY);
		KEY_CopyKeyTable(country);
	}
	KeyDrv.keytype=0;
	KeyDrv.ps2_keytype = 0;
}

void KEY_SetKey(INT8U mode, INT8U position, INT16U key_id)
{
//	HUGEDATA INT16U *p;
//	p = (HUGEDATA INT16U *)(KEY_NORMAL_TABLE_ADDR + mode*KEY_MAX_TABLE_SIZE);
//	HUGEDATA INT16U *p;
	INT32U p;
	INT16U pos;
	p = (KEY_NORMAL_TABLE_ADDR + mode*KEY_MAX_TABLE_SIZE);
	pos=position;
	p+=pos*2;
	Flash_wwrite(p,key_id);
//	p[position] = key_id;
}
	
void KEY_SetMode(INT8U mode)
{
	KeyDrv.keytype=mode;  // 0: Normal , 1:PGM, 2: Left[Normal]+Right[PGM]
	if (UseScanner) KeyDrv.ps2_keytype = 1;//when using scanner, set 1
	else KeyDrv.ps2_keytype = mode; 
}

INT8U KEY_GetMode(void)
{
	return KeyDrv.keytype;
}

#ifdef USE_FUNCTION_KEY_BLOCK
void KEY_InitKeyOnoff(void)
{
	INT8U i;
	INT8U  keycode[]={
		0x29, 0x2c, 0x2d, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 
		0x49, 0x4d, 0x4e, 0x59, 0x5d, 0x5e, 0x69, 0x6e,	0x79,
		};
	INT32U result = get_global_lparam(GLOBAL_FUNCTION_KEY_BLOCK);
	
	for(i=0; i<18; i++)
	{
		KeyOnoff[i].keycode = keycode[i];
		KeyOnoff[i].flag = (INT8U)(result >> i) & 0x01;
	}	 
}
#endif

void KEY_Init(void)
{
	KEY_InitKeyBuf();
	KEY_InitTable();
	KeyBuf.Buffer[0] = 0;
	KeyDrv.Shifted   = 0;
	KeyDrv.key_caps  = 0;
	KeyDrv.PressedModeKey = 0;
	KeyDrv.FirstKey = 1;
	KeyDrv.LastKeyTime = 0;
	KeyDrv.comm_key = 0;
	PS2_InitKeyBuf();	// Added by CJK 20051014
#ifdef USE_FUNCTION_KEY_BLOCK
	KEY_InitKeyOnoff();
#endif

	key_GetUserFunctionQty();
}

INT16U KEY_getrawcode_from_plukey(INT16U key, INT8U *level)
{
	INT16S i;
	INT16U *key_any_map;
//	if ((Startup.scale_type>3) || (Startup.scale_type<1)) Startup.scale_type=1;
	if ((Startup.scale_type ==4) || (Startup.scale_type<1)) Startup.scale_type=1;
	*level = 0;
	if (Startup.scale_type == 1)	// B-type
	{
		if (key > KS_PLU_48) 
		{
			*level = 1;	// second level
			key -= 48;
		}
		key-=KS_PLU_01;
		key+=KS_PLU_25;
	}
#if defined(USE_CL5200_KEY)
	else if(Startup.scale_type == 6)	// CL5200 B-type
	{
		if (key > KS_PLU_54) 
		{
			*level = 1;	// second level
			key -= 54;
		}
	}
#endif
	else	// P,R,G,H-type
	{
		if (key > KS_PLU_72) 
		{
			*level = 1;	// second level
			key -= 72;
			if(key >KS_PLU_72) //D-type, PLU 145,146,147,148 (F1~F4)
			{
				*level = 0;
				key += 72;
			}
		}
	}
//	if (Startup.scale_type==1) {
//		key-=KS_PLU_01;
//		key+=KS_PLU_25;
//	}
	
	if (Startup.scale_type == 3) 
		key_any_map=(INT16U*)key_hanging_map;
	else if (Startup.scale_type == 5)
		key_any_map=(INT16U*)key_doublebody_map;
#if defined(USE_CL5200_KEY)
	else if (Startup.scale_type == 6) // CL5200 B-type
		key_any_map=(INT16U*)key_CL5200_bench_map;
	else if (Startup.scale_type == SCALE_TYPE_HANGING_CL5200)
		key_any_map=(INT16U*)key_CL5200_hanging_map;

#endif
	else			
		key_any_map=(INT16U*)key_pole_map;

	for (i=0; i<128; i++) {
		if (key_any_map[i]==key) {
			return (i+1);
		}
	}
//	} else {
//		for (i=0; i<128; i++) {
//			if (key_hanging_map[i]==key) {
//				return (i+1);
//			}
//		}
//	}
	return 0;
}

INT16U KEY_getnormalkey(INT16U rawkey, INT8U level)
{
	INT32U lp;
	INT16U key;
	lp =rawkey;
	lp*=2;
	lp+=DFLASH_BASE;
	lp+=FLASH_KEY_AREA;
	if (level) lp+=KEY_MAX_TABLE_SIZE;
	key = Flash_wread(lp);
	return key;
}

