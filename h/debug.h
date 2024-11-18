/*****************************************************************************
|*
|*  Copyright		:	(c) 2002 CAS
|*  Filename		:	debug.h
|*  Version			:	0.1.1
|*  Programmer(s)	:	Chung jong kwon (CJK)
|*  Created			:	2003/08/20
|*  Modify		:	ByungMoo Lim (LBM) 2003/09/14
|*  Description		:	CLP DEBUG global variable define
|*
****************************************************************************/

#ifndef _DEBUG_H_DEF
#define _DEBUG_H_DEF

#define DBG_CMD_START 0
#define DBG_CMD_INTER 1
#define DBG_CMD_PARAM 2
#define DBG_CMD_SPARM 3
#define DBG_CMD_EXEC  4
#define DBG_CMD_CPARM 5

#define DBG_STR_BUF_SIZE 2048
#define DBG_CMD_AD	 104
#define DBG_CMD_AF	 106
#define DBG_CMD_AC	 108
#define DBG_CMD_CT   317
#define DBG_CMD_CV   319
#define DBG_CMD_GS	 716
#define DBG_CMD_SF   212
#define DBG_CMD_LC   213
#define DBG_CMD_EF   214
#define DBG_CMD_E3   215
#define DBG_CMD_E2   216
#define DBG_CMD_E1   217
#define DBG_CMD_BR	 218
#define DBG_CMD_W1   219
#define DBG_CMD_W2   220
#define DBG_CMD_WB   221

//DEL20060309#define DBG_CMD_CC	 303
#define DBG_CMD_CF	 306
#define DBG_CMD_CR	 318
#define DBG_CMD_CW	 323
#define DBG_CMD_DA	 401
#define DBG_CMD_DD	 404
#define DBG_CMD_DE	 405
#define DBG_CMD_CI   409
//DEL#define DBG_CMD_DI	 409
#define DBG_CMD_FF	 606
#define DBG_CMD_FC	 603
#define DBG_CMD_FD	 60
#define DBG_CMD_FE	 605
#define DBG_CMD_FL	 612
#define DBG_CMD_FM	 613
#define DBG_CMD_FR	 618
#define DBG_CMD_FS	 619
#define DBG_CMD_FT	 620
#define DBG_CMD_HP   700
#define DBG_CMD_IA	 901
#define DBG_CMD_IC	 903
#define DBG_CMD_IS   902
#define DBG_CMD_IR   903
#define DBG_CMD_II   904
#define DBG_CMD_IB   905
#define DBG_CMD_KS	1119

#define DBG_CMD_MA	1301
#define DBG_CMD_MF	1306
#define DBG_CMD_MR	1318
#define DBG_CMD_MS	1319
#define DBG_CMD_MT	1320
#define DBG_CMD_MW	1323
#define DBG_CMD_NW	1401
#define DBG_CMD_NR	1402
#define	DBG_CMD_PC	1603
#define	DBG_CMD_PD	1604
#define	DBG_CMD_PI	1605
#define DBG_CMD_PR	1618
#define DBG_CMD_PW	1619

#define	DBG_CMD_SM	1620
#define	DBG_CMD_SD	1621

#define DBG_CMD_QT	1720

#define DBG_CMD_TC	2003
#define DBG_CMD_TD	2004
#define DBG_CMD_TF	2006
#define DBG_CMD_TL	2012
#define DBG_CMD_TN	2014
#define DBG_CMD_TP	2016
#define DBG_CMD_TR	2018

#define DBG_CMD_VF	2206

#define DBG_CMD_SR  999
#define DBG_CMD_DF8M 777

#define DBG_CMD_NM  9797

#endif

#ifndef _DEBUG_H_TYPE_1
#define _DEBUG_H_TYPE_1
#pragma pack(4)
typedef struct {
	char name[4];
	INT16S cmdmode;
	INT16S nparam;
	char *help;
} DEBUG_TYPE;
#pragma pack()
#endif

#ifndef _DEBUG_H_TYPE_2
#define _DEBUG_H_TYPE_2

extern unsigned char debug_param_str[DBG_STR_BUF_SIZE];
extern short         debug_param_nstr;
extern INT8U debug_tic_on;

extern INT16S  debug_interprete(HUGEDATA COMM_BUF *CBuf,INT8U r_data);
extern void debug_init(void);
extern void DebugTerminal(HUGEDATA COMM_BUF *CBuf);
extern void DebugBackup(void);
extern void debug_message(char *buf);
extern void debug_message2(char *buf);
extern INT8U debug_tic(char ch);
extern void ad_firmware_download(void);
#endif