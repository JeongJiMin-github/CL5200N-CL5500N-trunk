/*****************************************************************************
|*			
|*  Copyright		:	(c) 2004 CAS
|*  Filename		:	ps2.c
|*  Version			:	0.1
|*  Programmer(s)	:	Hong Yi-Phyo (HYP)
|*  Created			:	2004/01/12
|*  Description		:	CLP ps2 module routine    
|*				
****************************************************************************/

#include <stdio.h>
#include "globals.h"
#include "initial_glb.h"
#include "initial.h"
#include "key_typedef.h"
#include "ps2.h"
#include "shopname.h"
#include "flash.h"
#include "flash_app.h"
#include "commbuf.h"
#include "key.h"

PS2_BUF_STRUCT PS2Buf;

ROMDATA INT8U ps2_key_table[PSW_KEY_TABLE_MAX_NUM] = {//scan code --> key num
/*	 0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F   */
	  0,120,  0,116,114,112,113,123,  0,121,119,117,115, 16,  1,  0,   //0
	  0, 60, 44,133, 58, 17,  2,  0,  0,  0, 46, 32, 31, 18,  3,  0,   //1
	  0, 48, 47, 33, 19,  5,  4,  0,  0, 61, 49, 34, 21, 20,  6,  0,   //2
	  0, 51, 50, 36, 35, 22,  7,  0,  0,  0, 52, 37, 23,  8,  9,  0,   //3
	  0, 53, 38, 24, 25, 11, 10,  0,  0, 54, 55, 39, 40, 26, 12,  0,   //4
	  0, 56, 41,  0, 27, 13,  0,  0, 30, 57, 43, 28,  0, 14,  0,  0,   //5
	  0, 45,  0,  0,132,  0, 15,131,  0, 93,  0, 92, 91,  0,  0,  0,   //6
	 99,104, 98, 97,102, 96,110, 90,122,106,103,105,100,101,125,  0,   //7
	  0,  0,  0,118,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0    //8
};

#define N_K	0xffff
//HYP 20060427 Insert PS2 key Sale mode Set
/*
-------------------------------------
Scale Key	PS2 Key   (Sale Mode)
-------------------------------------
KS_PLU_01	a		
KS_PLU_02	b
KS_PLU_03	c		
KS_PLU_04	d
KS_PLU_05	e		
KS_PLU_06	f
KS_PLU_07	g		
KS_PLU_08	h
KS_PLU_09	i		
KS_PLU_10	j
KS_PLU_11	k		
KS_PLU_12	l
KS_PLU_13	m		
KS_PLU_14	n
KS_PLU_15	o		
KS_PLU_16	p
KS_PLU_17	q		
KS_PLU_18	r
KS_PLU_19	s		
KS_PLU_20	t
KS_PLU_21	u		
KS_PLU_22	v
KS_PLU_23	w		
KS_PLU_24	x
KS_PLU_25	y		
KS_PLU_26	z
KS_PLU_27	A		
KS_PLU_28	B
KS_PLU_29	C		
KS_PLU_30	D
KS_PLU_31	E		
KS_PLU_32	F
KS_PLU_33	G		
KS_PLU_34	H
KS_PLU_35	I		
KS_PLU_36	J
KS_PLU_37	K		
KS_PLU_38	L
KS_PLU_39	M		
KS_PLU_40	N
KS_PLU_41	O		
KS_PLU_42	P
KS_PLU_43	Q		
KS_PLU_44	R
KS_PLU_45	S		
KS_PLU_46	T
KS_PLU_47	U		
KS_PLU_48	V
KS_PLU_49	W		
KS_PLU_50	X
KS_PLU_51	Y
KS_PLU_52	Z
-------------------------------------
Scale Key	PS2 Key   (Common)
-------------------------------------
KP_ESC		ESC
KS_MODE		F1
KP_CLEAR	F2
------		F3
------		F4
------		F5
------		F6
------		F7
------		F8
KP_CHAR_CODE	F9
KP_FEED/KS_FEED	F10
KP_TEST		F11
KP_SAVE		F12
KEY_NUM_0	0
KEY_NUM_1	1
KEY_NUM_2	2
KEY_NUM_3	3
KEY_NUM_4	4
KEY_NUM_5	5
KEY_NUM_6	6
KEY_NUM_7	7
KEY_NUM_8	8
KEY_NUM_9	9
KP_INSERT	INS
KP_DEL		DEL
KP_ARROW_UP	Up(Arrow)
KP_ARROW_DN	Down(Arrow)
KP_ARROW_RI	Right(Arrow)
KP_ARROW_LF	Left(Arrow)
KP_PAGE_UP	Page Up
KP_PAGE_DN	Page Down
----------Caution !!!----------------
KP_ENTER/KS_PRINT   Enter(Main Keypad)
KP_ASC_LF/KS_PRINT  Enter(Number Keypad)
		    Use Line Feed when input 'LF' char in ingredient
----------Notice !!!-----------------
NumLock,CapsLock,ScrollLock Display LED aren't on(Light)
1. 'Caps Lock' Key can be used
2. 'Num Lock', 'Scroll Lock' Keys cannot be used
3. 'Shift' Key can be used
-------------------------------------
한영 변환 (tableA <-> tableB)
1. 'Left Shift'와 'Space bar'동시에 누른다.
2. 'ALT' Key
----------for Developer--------------
20060428 Now
table setB는 한국어를 위한 table임
후에 ALT Key와 조합하여 사용하는 언어가 있으면
ALT Key Table로 사용 가능함 (코드 수정 필요)
-------------------------------------
*/

//SG070122. Barcode scanner 테스트 용
ROMDATA INT16U prog_ps2_code_setS_kor[PS2_KEY_MAX_NUM] = {
/*	 	 0             1             2             3             4             5             6             7             8             9  */
	 /*none*/N_K,    KP_ASC_60,    KEY_NUM_1,    KEY_NUM_2,    KEY_NUM_3,    KEY_NUM_4,    KEY_NUM_5,    KEY_NUM_6,    KEY_NUM_7,    KEY_NUM_8,   //0
           KEY_NUM_9,    KEY_NUM_0,    KP_ASC_2D,    KP_ASC_3D,    KP_ASC_5C,        KP_BS,   /*TAB*/N_K,/*q*/KS_PLU_17,/*w*/KS_PLU_23,/*e*/KS_PLU_05,   //10
      /*r*/KS_PLU_18,/*t*/KS_PLU_20,/*y*/KS_PLU_25,/*u*/KS_PLU_21,/*i*/KS_PLU_09,/*o*/KS_PLU_15,/*p*/KS_PLU_16,KP_ASC_5B,    KP_ASC_5D,/*none*/N_K,   //20
      /*CAPS*/N_K,/*a*/KS_PLU_01,/*s*/KS_PLU_19,/*d*/KS_PLU_04,/*f*/KS_PLU_06,/*g*/KS_PLU_07,/*h*/KS_PLU_08,/*j*/KS_PLU_10,/*k*/KS_PLU_11,/*l*/KS_PLU_12,   //30
           KP_ASC_3B,    KP_ASC_27,  /*none*/N_K,     KS_SCANPRINT, /*L_SFT*/N_K,  /*none*/N_K,/*z*/KS_PLU_26,/*x*/KS_PLU_24,/*c*/KS_PLU_03,/*v*/KS_PLU_22,   //40
      /*b*/KS_PLU_02,/*n*/KS_PLU_14,/*m*/KS_PLU_13,  KP_ASC_2C,    KP_ASC_2E,    KP_ASC_2F,  /*none*/N_K, /*R_SFT*/N_K, /*L_CTL*/N_K,  /*none*/N_K,   //50
        /*L_ALT*/N_K,    KP_ASC_20, /*R_ALT*/N_K,  /*none*/N_K, /*R_CTL*/N_K,  /*none*/N_K,  /*none*/N_K,  /*none*/N_K,  /*none*/N_K,  /*none*/N_K,   //60
         /*none*/N_K,  /*none*/N_K,  /*none*/N_K,  /*none*/N_K,  /*none*/N_K,    KP_INSERT,       KP_DEL,  /*none*/N_K,  /*none*/N_K,  KP_ARROW_LF,   //70
         /*HOME*/N_K,   /*END*/N_K,  /*none*/N_K,  KP_ARROW_UP,  KP_ARROW_DN,   KP_PAGE_UP,   KP_PAGE_DN,  /*none*/N_K,  /*none*/N_K,  KP_ARROW_RI,   //80
       /*N_LOCK*/N_K,    KEY_NUM_7,    KEY_NUM_4,    KEY_NUM_1,  /*none*/N_K,    KP_ASC_2F,    KEY_NUM_8,    KEY_NUM_5,    KEY_NUM_2,    KEY_NUM_0,   //90
	   KP_ASC_2A,    KEY_NUM_9,    KEY_NUM_6,    KEY_NUM_3,    KP_ASC_2E,    KP_ASC_2D,    KP_ASC_2B,  /*none*/N_K,     KS_SCANPRINT,  /*none*/N_K,   //100
	      KP_ESC,  /*none*/N_K,/*F1*/KS_MODE,/*F2*/KP_CLEAR,    /*F3*/N_K,   /*F4*/N_K,    /*F5*/N_K,    /*F6*/N_K,    /*F7*/N_K,    /*F8*/N_K,   //110
  /*F9*/KP_CHAR_CODE,/*F10*/KS_FEED,/*F11*/KP_TEST,/*F12*/KP_SAVE,/*PRT_SC*/N_K,/*S_LOCK*/N_K,/*PAUSE*/N_K, /*none*/N_K,  /*none*/N_K,  /*none*/N_K,   //120
         /*none*/N_K,  /*none*/N_K,  /*none*/N_K,  /*none*/N_K,  /*none*/N_K,  /*none*/N_K,  /*none*/N_K,  /*none*/N_K,  /*none*/N_K,  /*none*/N_K,   //130
};

//ROMDATA INT16U prog_ps2_code_setS_kor[PS2_KEY_MAX_NUM] = {
///*	 	 0             1             2             3             4             5             6             7             8             9  */
//	 /*none*/N_K,    KP_ASC_60,    KEY_NUM_1,    KEY_NUM_2,    KEY_NUM_3,    KEY_NUM_4,    KEY_NUM_5,    KEY_NUM_6,    KEY_NUM_7,    KEY_NUM_8,   //0
//           KEY_NUM_9,    KEY_NUM_0,    KP_ASC_2D,    KP_ASC_3D,    KP_ASC_5C,        KP_BS,   /*TAB*/N_K,/*q*/KS_PLU_17,/*w*/KS_PLU_23,/*e*/KS_PLU_05,   //10
//      /*r*/KS_PLU_18,/*t*/KS_PLU_20,/*y*/KS_PLU_25,/*u*/KS_PLU_21,/*i*/KS_PLU_09,/*o*/KS_PLU_15,/*p*/KS_PLU_16,KP_ASC_5B,    KP_ASC_5D,/*none*/N_K,   //20
//      /*CAPS*/N_K,/*a*/KS_PLU_01,/*s*/KS_PLU_19,/*d*/KS_PLU_04,/*f*/KS_PLU_06,/*g*/KS_PLU_07,/*h*/KS_PLU_08,/*j*/KS_PLU_10,/*k*/KS_PLU_11,/*l*/KS_PLU_12,   //30
//           KP_ASC_3B,    KP_ASC_27,  /*none*/N_K,     KS_PRINT, /*L_SFT*/N_K,  /*none*/N_K,/*z*/KS_PLU_26,/*x*/KS_PLU_24,/*c*/KS_PLU_03,/*v*/KS_PLU_22,   //40
//      /*b*/KS_PLU_02,/*n*/KS_PLU_14,/*m*/KS_PLU_13,  KP_ASC_2C,    KP_ASC_2E,    KP_ASC_2F,  /*none*/N_K, /*R_SFT*/N_K, /*L_CTL*/N_K,  /*none*/N_K,   //50
//        /*L_ALT*/N_K,    KP_ASC_20, /*R_ALT*/N_K,  /*none*/N_K, /*R_CTL*/N_K,  /*none*/N_K,  /*none*/N_K,  /*none*/N_K,  /*none*/N_K,  /*none*/N_K,   //60
//         /*none*/N_K,  /*none*/N_K,  /*none*/N_K,  /*none*/N_K,  /*none*/N_K,    KP_INSERT,       KP_DEL,  /*none*/N_K,  /*none*/N_K,  KP_ARROW_LF,   //70
//         /*HOME*/N_K,   /*END*/N_K,  /*none*/N_K,  KP_ARROW_UP,  KP_ARROW_DN,   KP_PAGE_UP,   KP_PAGE_DN,  /*none*/N_K,  /*none*/N_K,  KP_ARROW_RI,   //80
//       /*N_LOCK*/N_K,    KEY_NUM_7,    KEY_NUM_4,    KEY_NUM_1,  /*none*/N_K,    KP_ASC_2F,    KEY_NUM_8,    KEY_NUM_5,    KEY_NUM_2,    KEY_NUM_0,   //90
//	   KP_ASC_2A,    KEY_NUM_9,    KEY_NUM_6,    KEY_NUM_3,    KP_ASC_2E,    KP_ASC_2D,    KP_ASC_2B,  /*none*/N_K,     KS_PRINT,  /*none*/N_K,   //100
//	      KP_ESC,  /*none*/N_K,/*F1*/KS_MODE,/*F2*/KP_CLEAR,    /*F3*/N_K,   /*F4*/N_K,    /*F5*/N_K,    /*F6*/N_K,    /*F7*/N_K,    /*F8*/N_K,   //110
//  /*F9*/KP_CHAR_CODE,/*F10*/KS_FEED,/*F11*/KP_TEST,/*F12*/KP_SAVE,/*PRT_SC*/N_K,/*S_LOCK*/N_K,/*PAUSE*/N_K, /*none*/N_K,  /*none*/N_K,  /*none*/N_K,   //120
//         /*none*/N_K,  /*none*/N_K,  /*none*/N_K,  /*none*/N_K,  /*none*/N_K,  /*none*/N_K,  /*none*/N_K,  /*none*/N_K,  /*none*/N_K,  /*none*/N_K,   //130
//};

ROMDATA INT16U prog_ps2_code_setS_kor_shifted[PS2_KEY_MAX_NUM] = {
/*	 	 0             1             2             3             4             5             6             7             8             9  */
	 /*none*/N_K,    KP_ASC_7E,    KP_ASC_21,    KP_ASC_40,    KP_ASC_23,    KP_ASC_24,    KP_ASC_25,    KP_ASC_5E,    KP_ASC_26,    KP_ASC_2A,   //0
           KP_ASC_28,    KP_ASC_29,    KP_ASC_5F,    KP_ASC_2B,    KP_ASC_7C,        KP_BS,   /*TAB*/N_K,/*Q*/KS_PLU_43,/*W*/KS_PLU_49,/*E*/KS_PLU_31,   //10							     
      /*R*/KS_PLU_44,/*T*/KS_PLU_46,/*Y*/KS_PLU_51,/*U*/KS_PLU_47,/*I*/KS_PLU_35,/*O*/KS_PLU_41,/*P*/KS_PLU_42,KP_ASC_7B,  KP_ASC_7D,  /*none*/N_K,   //20
      /*CAPS*/N_K,/*A*/KS_PLU_27,/*S*/KS_PLU_45,/*D*/KS_PLU_30,/*F*/KS_PLU_32,/*G*/KS_PLU_33,/*H*/KS_PLU_34,/*J*/KS_PLU_36,/*K*/KS_PLU_37,/*L*/KS_PLU_38,   //30
	   KP_ASC_3A,    KP_ASC_22,  /*none*/N_K,     KS_SCANPRINT, /*L_SFT*/N_K,  /*none*/N_K,/*Z*/KS_PLU_52,/*X*/KS_PLU_40,/*C*/KS_PLU_29,/*V*/KS_PLU_48,   //40
      /*B*/KS_PLU_28,/*N*/KS_PLU_40,/*M*/KS_PLU_39,  KP_ASC_3C,    KP_ASC_3E,    KP_ASC_3F,  /*none*/N_K, /*R_SFT*/N_K, /*L_CTL*/N_K,  /*none*/N_K,   //50
        /*L_ALT*/N_K,    KP_ASC_20, /*R_ALT*/N_K,  /*none*/N_K, /*R_CTL*/N_K,  /*none*/N_K,  /*none*/N_K,  /*none*/N_K,  /*none*/N_K,  /*none*/N_K,   //60
	 /*none*/N_K,  /*none*/N_K,  /*none*/N_K,  /*none*/N_K,  /*none*/N_K,    KP_INSERT,       KP_DEL,  /*none*/N_K,  /*none*/N_K,  KP_ARROW_LF,   //70
	 /*HOME*/N_K,   /*END*/N_K,  /*none*/N_K,  KP_ARROW_UP,  KP_ARROW_DN,   KP_PAGE_UP,   KP_PAGE_DN,  /*none*/N_K,  /*none*/N_K,  KP_ARROW_RI,   //80
       /*N_LOCK*/N_K,    KEY_NUM_7,    KEY_NUM_4,    KEY_NUM_1,  /*none*/N_K,    KP_ASC_2F,    KEY_NUM_8,    KEY_NUM_5,    KEY_NUM_2,    KEY_NUM_0,   //90
	   KP_ASC_2A,    KEY_NUM_9,    KEY_NUM_6,    KEY_NUM_3,    KP_ASC_2E,    KP_ASC_2D,    KP_ASC_2B,  /*none*/N_K,     KS_SCANPRINT,  /*none*/N_K,   //100
	      KP_ESC,  /*none*/N_K,/*F1*/KS_MODE,/*F2*/KP_CLEAR,   /*F3*/N_K,    /*F4*/N_K,    /*F5*/N_K,    /*F6*/N_K,    /*F7*/N_K,    /*F8*/N_K,   //110
  /*F9*/KP_CHAR_CODE,/*F10*/KS_FEED,/*F11*/KP_TEST,/*F12*/KP_SAVE,/*PRT_SC*/N_K,/*S_LOCK*/N_K,/*PAUSE*/N_K, /*none*/N_K,  /*none*/N_K,  /*none*/N_K,   //120
         /*none*/N_K,  /*none*/N_K,  /*none*/N_K,  /*none*/N_K,  /*none*/N_K,  /*none*/N_K,  /*none*/N_K,  /*none*/N_K,  /*none*/N_K,  /*none*/N_K,   //130
};
	 
ROMDATA INT16U prog_ps2_code_setA_kor[PS2_KEY_MAX_NUM] = {
/*	 	 0             1             2             3             4             5             6             7             8             9  */
	 /*none*/N_K,    KP_ASC_60,    KEY_NUM_1,    KEY_NUM_2,    KEY_NUM_3,    KEY_NUM_4,    KEY_NUM_5,    KEY_NUM_6,    KEY_NUM_7,    KEY_NUM_8,   //0
           KEY_NUM_9,    KEY_NUM_0,    KP_ASC_2D,    KP_ASC_3D,    KP_ASC_5C,        KP_BS,   /*TAB*/N_K,   KP_ASC_Q_L,   KP_ASC_W_L,   KP_ASC_E_L,   //10
          KP_ASC_R_L,   KP_ASC_T_L,   KP_ASC_Y_L,   KP_ASC_U_L,   KP_ASC_I_L,   KP_ASC_O_L,   KP_ASC_P_L,    KP_ASC_5B,    KP_ASC_5D,  /*none*/N_K,   //20
         /*CAPS*/N_K,   KP_ASC_A_L,   KP_ASC_S_L,   KP_ASC_D_L,   KP_ASC_F_L,   KP_ASC_G_L,   KP_ASC_H_L,   KP_ASC_J_L,   KP_ASC_K_L,   KP_ASC_L_L,   //30
           KP_ASC_3B,    KP_ASC_27,  /*none*/N_K,     KP_ENTER, /*L_SFT*/N_K,  /*none*/N_K,   KP_ASC_Z_L,   KP_ASC_X_L,   KP_ASC_C_L,   KP_ASC_V_L,   //40
          KP_ASC_B_L,   KP_ASC_N_L,   KP_ASC_M_L,    KP_ASC_2C,    KP_ASC_2E,    KP_ASC_2F,  /*none*/N_K, /*R_SFT*/N_K, /*L_CTL*/N_K,  /*none*/N_K,   //50
        /*L_ALT*/N_K,    KP_ASC_20, /*R_ALT*/N_K,  /*none*/N_K, /*R_CTL*/N_K,  /*none*/N_K,  /*none*/N_K,  /*none*/N_K,  /*none*/N_K,  /*none*/N_K,   //60
         /*none*/N_K,  /*none*/N_K,  /*none*/N_K,  /*none*/N_K,  /*none*/N_K,    KP_INSERT,       KP_DEL,  /*none*/N_K,  /*none*/N_K,  KP_ARROW_LF,   //70
         /*HOME*/N_K,   /*END*/N_K,  /*none*/N_K,  KP_ARROW_UP,  KP_ARROW_DN,   KP_PAGE_UP,   KP_PAGE_DN,  /*none*/N_K,  /*none*/N_K,  KP_ARROW_RI,   //80
       /*N_LOCK*/N_K,    KEY_NUM_7,    KEY_NUM_4,    KEY_NUM_1,  /*none*/N_K,    KP_ASC_2F,    KEY_NUM_8,    KEY_NUM_5,    KEY_NUM_2,    KEY_NUM_0,   //90
	   KP_ASC_2A,    KEY_NUM_9,    KEY_NUM_6,    KEY_NUM_3,    KP_ASC_2E,    KP_ASC_2D,    KP_ASC_2B,  /*none*/N_K,    KP_ASC_LF,  /*none*/N_K,   //100
	      KP_ESC,  /*none*/N_K,/*F1*/KS_MODE,    /*F2*/KP_CLEAR,    /*F3*/N_K,    /*F4*/N_K,    /*F5*/N_K,    /*F6*/N_K,    /*F7*/N_K,    /*F8*/N_K,   //110
  /*F9*/KP_CHAR_CODE,/*F10*/KP_FEED,   /*F11*/KP_TEST,/*F12*/KP_SAVE,/*PRT_SC*/N_K,/*S_LOCK*/N_K, /*PAUSE*/N_K,  /*none*/N_K,  /*none*/N_K,  /*none*/N_K,   //120
         /*none*/N_K,  /*none*/N_K,  /*none*/N_K,  /*none*/N_K,  /*none*/N_K,  /*none*/N_K,  /*none*/N_K,  /*none*/N_K,  /*none*/N_K,  /*none*/N_K,   //130
};

ROMDATA INT16U prog_ps2_code_setA_kor_shifted[PS2_KEY_MAX_NUM] = {
/*	 	 0             1             2             3             4             5             6             7             8             9  */
	 /*none*/N_K,    KP_ASC_7E,    KP_ASC_21,    KP_ASC_40,    KP_ASC_23,    KP_ASC_24,    KP_ASC_25,    KP_ASC_5E,    KP_ASC_26,    KP_ASC_2A,   //0
           KP_ASC_28,    KP_ASC_29,    KP_ASC_5F,    KP_ASC_2B,    KP_ASC_7C,        KP_BS,   /*TAB*/N_K,     KP_ASC_Q,     KP_ASC_W,     KP_ASC_E,   //10							     
            KP_ASC_R,     KP_ASC_T,     KP_ASC_Y,     KP_ASC_U,     KP_ASC_I,     KP_ASC_O,     KP_ASC_P,    KP_ASC_7B,    KP_ASC_7D,  /*none*/N_K,   //20
	 /*CAPS*/N_K,     KP_ASC_A,     KP_ASC_S,     KP_ASC_D,     KP_ASC_F,     KP_ASC_G,     KP_ASC_H,     KP_ASC_J,     KP_ASC_K,     KP_ASC_L,   //30
	   KP_ASC_3A,    KP_ASC_22,  /*none*/N_K,     KP_ENTER, /*L_SFT*/N_K,  /*none*/N_K,     KP_ASC_Z,     KP_ASC_X,     KP_ASC_C,     KP_ASC_V,   //40
	    KP_ASC_B,     KP_ASC_N,     KP_ASC_M,    KP_ASC_3C,    KP_ASC_3E,    KP_ASC_3F,  /*none*/N_K, /*R_SFT*/N_K, /*L_CTL*/N_K,  /*none*/N_K,   //50
        /*L_ALT*/N_K,    KP_ASC_20, /*R_ALT*/N_K,  /*none*/N_K, /*R_CTL*/N_K,  /*none*/N_K,  /*none*/N_K,  /*none*/N_K,  /*none*/N_K,  /*none*/N_K,   //60
	 /*none*/N_K,  /*none*/N_K,  /*none*/N_K,  /*none*/N_K,  /*none*/N_K,    KP_INSERT,       KP_DEL,  /*none*/N_K,  /*none*/N_K,  KP_ARROW_LF,   //70
	 /*HOME*/N_K,   /*END*/N_K,  /*none*/N_K,  KP_ARROW_UP,  KP_ARROW_DN,   KP_PAGE_UP,   KP_PAGE_DN,  /*none*/N_K,  /*none*/N_K,  KP_ARROW_RI,   //80
       /*N_LOCK*/N_K,    KEY_NUM_7,    KEY_NUM_4,    KEY_NUM_1,  /*none*/N_K,    KP_ASC_2F,    KEY_NUM_8,    KEY_NUM_5,    KEY_NUM_2,    KEY_NUM_0,   //90
	   KP_ASC_2A,    KEY_NUM_9,    KEY_NUM_6,    KEY_NUM_3,    KP_ASC_2E,    KP_ASC_2D,    KP_ASC_2B,  /*none*/N_K,    KP_ASC_LF,  /*none*/N_K,   //100
	      KP_ESC,  /*none*/N_K,/*F1*/KS_MODE,    /*F2*/KP_CLEAR,    /*F3*/N_K,    /*F4*/N_K,    /*F5*/N_K,    /*F6*/N_K,    /*F7*/N_K,    /*F8*/N_K,   //110
  /*F9*/KP_CHAR_CODE,/*F10*/KP_FEED,   /*F11*/KP_TEST,/*F12*/KP_SAVE,/*PRT_SC*/N_K,/*S_LOCK*/N_K, /*PAUSE*/N_K,  /*none*/N_K,  /*none*/N_K,  /*none*/N_K,   //120
         /*none*/N_K,  /*none*/N_K,  /*none*/N_K,  /*none*/N_K,  /*none*/N_K,  /*none*/N_K,  /*none*/N_K,  /*none*/N_K,  /*none*/N_K,  /*none*/N_K,   //130
};

ROMDATA INT16U prog_ps2_code_setB_kor[PS2_KEY_MAX_NUM] = {
/*	 	 0             1             2             3             4             5             6             7             8             9  */
	 /*none*/N_K,    KP_ASC_60,    KEY_NUM_1,    KEY_NUM_2,    KEY_NUM_3,    KEY_NUM_4,    KEY_NUM_5,    KEY_NUM_6,    KEY_NUM_7,    KEY_NUM_8,   //0
           KEY_NUM_9,    KEY_NUM_0,    KP_ASC_2D,    KP_ASC_3D,    KP_ASC_5C,        KP_BS,   /*TAB*/N_K,   	KP_I_B,       KP_I_J,       KP_I_D,   //10							     
              KP_I_K,       KP_I_S,      KP_N_YO,     KP_N_YEO,      KP_N_YA,      KP_N_AE,       KP_N_E,    KP_ASC_5B,    KP_ASC_5D,  /*none*/N_K,   //20
         /*CAPS*/N_K,       KP_I_M,       KP_I_N,       KP_I_O,       KP_I_R,       KP_I_H,       KP_N_O,      KP_N_EO,       KP_N_A,       KP_N_I,   //30
           KP_ASC_3B,    KP_ASC_27,  /*none*/N_K,     KP_ENTER, /*L_SFT*/N_K,  /*none*/N_K,       KP_I_C,       KP_I_T,      KP_I_CH,       KP_I_P,   //40
             KP_N_YU,       KP_N_U,      KP_N_EU,    KP_ASC_2C,    KP_ASC_2E,    KP_ASC_2F,  /*none*/N_K, /*R_SFT*/N_K, /*L_CTL*/N_K,  /*none*/N_K,   //50
        /*L_ALT*/N_K,    KP_ASC_20, /*R_ALT*/N_K,  /*none*/N_K, /*R_CTL*/N_K,  /*none*/N_K,  /*none*/N_K,  /*none*/N_K,  /*none*/N_K,  /*none*/N_K,   //60
         /*none*/N_K,  /*none*/N_K,  /*none*/N_K,  /*none*/N_K,  /*none*/N_K,    KP_INSERT,       KP_DEL,  /*none*/N_K,  /*none*/N_K,  KP_ARROW_LF,   //70
         /*HOME*/N_K,   /*END*/N_K,  /*none*/N_K,  KP_ARROW_UP,  KP_ARROW_DN,   KP_PAGE_UP,   KP_PAGE_DN,  /*none*/N_K,  /*none*/N_K,  KP_ARROW_RI,   //80
       /*N_LOCK*/N_K,    KEY_NUM_7,    KEY_NUM_4,    KEY_NUM_1,  /*none*/N_K,    KP_ASC_2F,    KEY_NUM_8,    KEY_NUM_5,    KEY_NUM_2,    KEY_NUM_0,   //90
	   KP_ASC_2A,    KEY_NUM_9,    KEY_NUM_6,    KEY_NUM_3,    KP_ASC_2E,    KP_ASC_2D,    KP_ASC_2B,  /*none*/N_K,     KP_ENTER,  /*none*/N_K,   //100
	      KP_ESC,  /*none*/N_K,/*F1*/KS_MODE,    /*F2*/KP_CLEAR,    /*F3*/N_K,    /*F4*/N_K,    /*F5*/N_K,    /*F6*/N_K,    /*F7*/N_K,    /*F8*/N_K,   //110
  /*F9*/KP_CHAR_CODE,/*F10*/KP_FEED,   /*F11*/KP_TEST,/*F12*/KP_SAVE,/*PRT_SC*/N_K,/*S_LOCK*/N_K, /*PAUSE*/N_K,  /*none*/N_K,  /*none*/N_K,  /*none*/N_K,   //120
         /*none*/N_K,  /*none*/N_K,  /*none*/N_K,  /*none*/N_K,  /*none*/N_K,  /*none*/N_K,  /*none*/N_K,  /*none*/N_K,  /*none*/N_K,  /*none*/N_K,   //130
};

ROMDATA INT16U prog_ps2_code_setB_kor_shifted[PS2_KEY_MAX_NUM] = {
/*	 	 0             1             2             3             4             5             6             7             8             9  */
	 /*none*/N_K,    KP_ASC_7E,    KP_ASC_21,     KP_ASC_40,    KP_ASC_23,    KP_ASC_24,    KP_ASC_25,    KP_ASC_5E,    KP_ASC_26,    KP_ASC_2A,   //0
           KP_ASC_28,    KP_ASC_29,    KP_ASC_5F,     KP_ASC_2B,    KP_ASC_7C,        KP_BS,   /*TAB*/N_K,      KP_I_BB,      KP_I_JJ,      KP_I_DD,   //10							     
             KP_I_KK,      KP_I_SS,      KP_N_YO,      KP_N_YEO,      KP_N_YA,     KP_N_YAE,      KP_N_YE,    KP_ASC_7B,    KP_ASC_7D,  /*none*/N_K,   //20
	 /*CAPS*/N_K,       KP_I_M,       KP_I_N,        KP_I_O,       KP_I_R,       KP_I_H,       KP_N_O,      KP_N_EO,       KP_N_A,       KP_N_I,   //30
	   KP_ASC_3A,    KP_ASC_22,  /*none*/N_K,      KP_ENTER, /*L_SFT*/N_K,  /*none*/N_K,       KP_I_C,       KP_I_T,      KP_I_CH,       KP_I_P,   //40
	     KP_N_YU,       KP_N_U,      KP_N_EU,     KP_ASC_3C,    KP_ASC_3E,    KP_ASC_3F,  /*none*/N_K, /*R_SFT*/N_K, /*L_CTL*/N_K,  /*none*/N_K,   //50
        /*L_ALT*/N_K,    KP_ASC_20, /*R_ALT*/N_K,   /*none*/N_K, /*R_CTL*/N_K,  /*none*/N_K,  /*none*/N_K,  /*none*/N_K,  /*none*/N_K,  /*none*/N_K,   //60
	 /*none*/N_K,  /*none*/N_K,  /*none*/N_K,   /*none*/N_K,  /*none*/N_K,    KP_INSERT,       KP_DEL,  /*none*/N_K,  /*none*/N_K,  KP_ARROW_LF,   //70
	 /*HOME*/N_K,   /*END*/N_K,  /*none*/N_K,   KP_ARROW_UP,  KP_ARROW_DN,   KP_PAGE_UP,   KP_PAGE_DN,  /*none*/N_K,  /*none*/N_K,  KP_ARROW_RI,   //80
       /*N_LOCK*/N_K,    KEY_NUM_7,    KEY_NUM_4,     KEY_NUM_1,  /*none*/N_K,    KP_ASC_2F,    KEY_NUM_8,    KEY_NUM_5,    KEY_NUM_2,    KEY_NUM_0,   //90
	   KP_ASC_2A,    KEY_NUM_9,    KEY_NUM_6,     KEY_NUM_3,    KP_ASC_2E,    KP_ASC_2D,    KP_ASC_2B,  /*none*/N_K,    KP_ASC_LF,  /*none*/N_K,   //100
	      KP_ESC,  /*none*/N_K,/*F1*/KS_MODE,/*F2*/KP_CLEAR,    /*F3*/N_K,    /*F4*/N_K,    /*F5*/N_K,    /*F6*/N_K,    /*F7*/N_K,    /*F8*/N_K,   //110
  /*F9*/KP_CHAR_CODE,/*F10*/KP_FEED,   /*F11*/KP_TEST,/*F12*/KP_SAVE,/*PRT_SC*/N_K,/*S_LOCK*/N_K, /*PAUSE*/N_K,  /*none*/N_K,  /*none*/N_K,  /*none*/N_K,   //120
         /*none*/N_K,  /*none*/N_K,  /*none*/N_K,   /*none*/N_K,  /*none*/N_K,  /*none*/N_K,  /*none*/N_K,  /*none*/N_K,  /*none*/N_K,  /*none*/N_K,   //130
};

//SG071126 Test Only : CL5000-Selfservice type 사용시 PS2 키맵
ROMDATA INT16U prog_ps2_code_setS_self[PS2_KEY_MAX_NUM] = {
/*	 	 0          1          2          3         4           5          6          7          8          9          A          B          C          D          E          F */
	 N_K      , KS_PLU_01, KS_PLU_02, KS_PLU_03, KS_PLU_04, KS_PLU_05, KS_PLU_06, KS_PLU_07, KS_PLU_08, KS_PLU_09, KS_PLU_10, KS_PLU_11, KS_PLU_12, KS_PLU_13, KS_PLU_14, KS_PLU_15, 
	 KS_PLU_16, KS_PLU_17, KS_PLU_18, KS_PLU_19, KS_PLU_20, KS_PLU_21, KS_PLU_22, KS_PLU_23, KS_PLU_24, KS_PLU_25, KS_PLU_26, KS_PLU_27, KS_PLU_28, KS_PLU_29, KS_PLU_30, KS_PLU_31, 
	 KS_PLU_32, KS_PLU_33, KS_PLU_34, KS_PLU_35, KS_PLU_36, KS_PLU_37, KS_PLU_38, KS_PLU_39, KS_PLU_40, KS_PLU_41, KS_PLU_42, KS_PLU_43, KS_PLU_44, KS_PLU_45, KS_PLU_46, KS_PLU_47, 
	 KS_PLU_48, KS_PLU_49, KS_PLU_50, KS_PLU_51, KS_PLU_52, KS_PLU_53, KS_PLU_54, KS_PLU_55, KS_PLU_56, KS_PLU_57, KS_PLU_58, KS_PLU_59, KS_PLU_60, KS_PLU_61, KS_PLU_62, KS_PLU_63, 
	 KS_PLU_64, KS_PLU_65, KS_PLU_66, KS_PLU_67, KS_PLU_68, KS_PLU_69, KS_PLU_70, KS_PLU_71, KS_PLU_72, KS_PLU_73, KS_PLU_74, KS_PLU_75, KS_PLU_76, KS_PLU_77, KS_PLU_78, KS_PLU_79,
	 KS_PLU_80, KS_PLU_81, KS_PLU_82, KS_PLU_83, KS_PLU_84, KS_PLU_85, KS_PLU_86, KS_PLU_87, KS_PLU_88, KS_PLU_89, KS_PLU_90, KS_PLU_91, KS_PLU_92, KS_PLU_93, KS_PLU_94, KS_PLU_95,
	 KS_PLU_96, KS_PLU_97, KS_PLU_98, KS_PLU_99, KS_PLU_100,KS_PLU_101,KS_PLU_102,KS_PLU_103,KS_PLU_104,KS_PLU_105,KS_PLU_106,KS_PLU_107,KS_PLU_108,KS_PLU_109,KS_PLU_110,KS_PLU_111,
	 KS_PLU_112,KS_PLU_113,KS_PLU_114,KS_PLU_115,KS_PLU_116,KS_PLU_117,KS_PLU_118,KS_PLU_119,KS_PLU_120,KS_PLU_121,KS_PLU_122,KS_PLU_123,KS_PLU_124,KS_PLU_125,KS_PLU_126,KS_PLU_127,
	 KS_PLU_128,KS_PLU_129,KS_PLU_130,KS_PLU_131,KS_PLU_132,KS_PLU_133,KS_PLU_134,KS_PLU_135,KS_PLU_136,KS_PLU_137,KS_PLU_138,KS_PLU_139 
}; //total = 140 key


void PS2_Write(INT16U code)
{
	//while(KeyBuf.WriteFlag);

	PS2Buf.Buffer[PS2Buf.Push] = code;
	PS2Buf.Push++;
	if(PS2Buf.Push >= PS2_BUFFER_LEN) 
	{	
		PS2Buf.Push=0;
	}
	if(PS2Buf.Push == PS2Buf.Pop)
	{
		PS2Buf.Pop++;
	}
	PS2Buf.Pop %= PS2_BUFFER_LEN;
}

void PS2_RawWrite(INT8U code)
{
	//while(KeyBuf.WriteFlag);

	PS2Buf.RawBuffer[PS2Buf.RawPush] = code;
	PS2Buf.RawPush++;
	if(PS2Buf.RawPush >= PS2_RAW_BUFFER_LEN) 
	{	
		PS2Buf.RawPush=0;
	}
	if(PS2Buf.RawPush == PS2Buf.RawPop)
	{
		PS2Buf.RawPop++;
	}
	PS2Buf.RawPop %= PS2_RAW_BUFFER_LEN;
}


void PS2_InitKeyBuf(void)
{
	INT16S i;
	PS2Buf.Pop = 0;
	PS2Buf.Push = 0;
	PS2Buf.RawPop = 0;
	PS2Buf.RawPush = 0;
	for(i = 0; i < PS2_BUFFER_LEN; i++)
	{
		PS2Buf.Buffer[i] = 0;
	}
	for(i = 0; i < PS2_RAW_BUFFER_LEN; i++)
	{
		PS2Buf.RawBuffer[i] = 0;
	}
}


INT16U PS2_InKey(void)
{
	INT16U key;

	key = PS2Buf.Buffer[PS2Buf.Pop];
	PS2Buf.Buffer[PS2Buf.Pop] = 0;
	PS2Buf.Pop++;
	if(PS2Buf.Pop >= PS2_BUFFER_LEN)
	{
		PS2Buf.Pop = 0;
	}
	return key;
}

INT16U PS2_InRawKey(void)
{
	INT8U raw_key;

	raw_key = PS2Buf.RawBuffer[PS2Buf.RawPop];
	PS2Buf.RawBuffer[PS2Buf.RawPop] = 0;
	PS2Buf.RawPop++;
	if(PS2Buf.RawPop >= PS2_RAW_BUFFER_LEN)
	{
		PS2Buf.RawPop = 0;
	}
	return raw_key;
}


INT8U PS2_IsRawKey(void)
{
	if(PS2Buf.RawPop != PS2Buf.RawPush)
	{
		return 1;
	}
	return 0;
}

INT8U PS2_IsKey(void)
{
	while(PS2_IsRawKey()) {
		PS2decode(PS2_InRawKey());
	}	
	if(PS2Buf.Pop != PS2Buf.Push)
	{
		return 1;
	}
	return 0;
}

INT8U PS2changelang(INT8U pgm2key)
{
//	if (Startup.country==COUNTRY_KR) {
		if(pgm2key == 0) pgm2key = 1;
		else pgm2key = 0;
//	} else {
//		pgm2key=0;
//	}
	return pgm2key;
}

//not complete yet
void PS2SendCmd(INT8U *cmd, INT16U size)
{
/*	
	char Trans_buffer[20];
	INT16U ptr;
	INT16U i;
	
	ptr = 0;
	Trans_buffer[ptr++] = 0x02;
	Trans_buffer[ptr++] = 'P';
	Trans_buffer[ptr++] = 'D';
	for (i = 0; i < size; i++)
	{
		sprintf(&Trans_buffer[ptr++], "%02X", cmd[i]);  
		ptr++;
	}
	Trans_buffer[ptr++] = 0x03;
	Trans_buffer[ptr++] = 0x00;
	
	PutStrTxBuf(&CommBufKey, &Trans_buffer[0]);  
	Key_Send();
*/	
}

void PS2decode(INT8U sc) 
{
	static INT8U is_up = 0, lshift = 0, shift = 0, alt = 0, caps = 0, ctrl = 0, e0_state = 0, pgm2key = 0;
	static INT8U led_status = 0x02;//num lock only
    INT8U	ps2keynum;
	INT16U  ps2keycode;
	INT32U lp;
	INT8U  pole=0;
	INT8U  temp=0;
	INT8U  cmd[16];
	
	//temp=get_global_bparam(GLOBAL_SALE_SETUP_FLAG7);  //parameter 623
	//temp=(temp>>1)&0x01;
	pole = get_global_bparam(GLOBAL_AREA_SCALETYPE);
	if(pole==4) temp=1;

	if(!is_up) 
	{
		if(temp) goto SELF_PS2;  //SG071121
		switch(sc) 
		{   
			case 0xfa: //ACK
				e0_state = 0;
				break;
			case 0xaa: //BAT complete
				cmd[0] = 0xed;
				cmd[1] = led_status;
				PS2SendCmd(cmd, 2);
				e0_state = 0;
				break;
			case 0xe0:
				e0_state = 1;
				break;
			case 0xf0://the up-key identifier
				is_up = 1;
				e0_state = 0;
				break;
			case 0x12://Left SHIFT
				lshift = 1;
			case 0x59://Right SHIFT
				shift = 1;
				e0_state = 0;
				break;
			case 0x11://ALT
				alt = 1;
				e0_state = 0;
#if !defined(USE_ARAB_FONT) && !defined(HEBREW_FONT)
				pgm2key=PS2changelang(pgm2key);
#endif
				break;
			case 0x14://CTRL
				ctrl = 1;
				e0_state = 0;
				break;
			case 0x58:
			  	if(caps) 
				{
					caps = 0;
					led_status = led_status & 0xFB;
			  	}
				else
				{
					caps = 1;
					led_status = led_status | 0x04;
				}
				cmd[0] = 0xed;
				cmd[1] = led_status;
				PS2SendCmd(cmd, 2);		
				e0_state = 0;
				break;				
			default:
SELF_PS2:
				if(sc < PSW_KEY_TABLE_MAX_NUM) 
				{
					if(e0_state == 0) 
					{
						if(temp) ps2keynum = sc;	//SG071126
						else ps2keynum = ps2_key_table[sc];
					} 
					else 
					{
						switch(sc) 
						{
							default:
								ps2keynum = 0;
								break;
							case 0x70:
								ps2keynum = 75;
								break;
							case 0x71:
								ps2keynum = 76;
								break;
							case 0x6B:
								ps2keynum = 79;
								break;
							case 0x6C:
								ps2keynum = 80;
								break;
							case 0x69:
								ps2keynum = 81;
								break;
							case 0x75:
								ps2keynum = 83;
								break;
							case 0x72:
								ps2keynum = 84;
								break;
							case 0x7D:
								ps2keynum = 85;
								break;
							case 0x7A:
								ps2keynum = 86;
								break;
							case 0x74:
								ps2keynum = 89;
								break;
							case 0x5A:
								ps2keynum = 108;
								break;
							case 0x4A:
								ps2keynum = 95;
								break;
							case 0x12:
								ps2keynum = 124;
								break;
						}
					}

				}
				if(ps2keynum != 0) 
				{
			  		if(KeyDrv.ps2_keytype == 0) 
					{//// 0: Normal
			  			if(shift == 1) 
						{
							lp = FLASH_PSKEY_SHIFT_AREA + DFLASH_BASE;
							ps2keycode = Flash_wread(lp + 2*ps2keynum);
						} 
						else 
						{
							lp = FLASH_PSKEY_AREA + DFLASH_BASE;
							ps2keycode = Flash_wread(lp + 2*ps2keynum);
						}
			  		} 
					else 
					{//// 1:PGM, 2: Left[Normal]+Right[PGM]
						if(shift == 1) 
						{
							if(pgm2key == 1) 
							{
								lp = FLASH_PSKEY_PGM2_SHIFT_AREA + DFLASH_BASE;
								ps2keycode = Flash_wread(lp + 2*ps2keynum);
							} 
							else 
							{
								if(caps == 1) 
								{
									lp = FLASH_PSKEY_PGM_AREA + DFLASH_BASE;
									ps2keycode = Flash_wread(lp + 2*ps2keynum);
								} 
								else 
								{
									lp = FLASH_PSKEY_PGM_SHIFT_AREA + DFLASH_BASE;
									ps2keycode = Flash_wread(lp + 2*ps2keynum);
								}
							}
							//if (Startup.country==COUNTRY_KR) {
#if !defined(USE_ARAB_FONT) && !defined(HEBREW_FONT)
							if((lshift == 1) && (ps2keycode == KP_ASC_20))
							{
								pgm2key=PS2changelang(pgm2key);
								ps2keycode = N_K;
							}
#endif
							//}
						} 
						else 
						{
							if(pgm2key == 1) 
							{
								lp = FLASH_PSKEY_PGM2_AREA + DFLASH_BASE;
								ps2keycode = Flash_wread(lp + 2*ps2keynum);
							} 
							else 
							{
								if(caps == 1) 
								{
									lp = FLASH_PSKEY_PGM_SHIFT_AREA + DFLASH_BASE;
									ps2keycode = Flash_wread(lp + 2*ps2keynum);
								} 
								else 
								{
									lp = FLASH_PSKEY_PGM_AREA + DFLASH_BASE;
									ps2keycode = Flash_wread(lp + 2*ps2keynum);
								}
							}
#if defined(USE_ARAB_FONT) || defined(HEBREW_FONT)
							if((ctrl == 1) && (ps2keycode == KP_ASC_20)) 
							{
								pgm2key=PS2changelang(pgm2key);
								ps2keycode = N_K;
							}
#endif
						}
					}
//					if(alt == 1) {
//						ps2keycode = alt_table[ps2keynum];
//					} else {
//						ps2keycode = plain_table[ps2keynum];
//					}
//					if(ctrl == 1) {
//						ps2keycode = ctrl_table[ps2keynum];
//					} else {
//						ps2keycode = plain_table[ps2keynum];
//					}
					if(ps2keycode != N_K)
					{
						PS2_Write(ps2keycode);
					}
				}
				e0_state = 0;
				break;
		}
	} 
	else 
	{
		is_up = 0; //Two 0xf0 in a row not allowed
		switch(sc) 
		{
			case 0x12://Left SHIFT
				lshift = 0;
			case 0x59://Right SHIFT
				shift = 0;
				break;
			case 0x11://ALT
				alt=0;
				break;
			case 0x14://CTRL
				ctrl=0;
				break;
			default:
				break;
		}
		e0_state = 0;
	}
}

