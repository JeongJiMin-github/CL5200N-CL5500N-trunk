/*****************************************************************************
|*			
|*  Copyright		:	(c) 2002 CAS
|*  Filename		:	prt_interpreter.c
|*  Version		:	1.0
|*  Programmer(s)	:	Hong Yi-Phyo (HYP)
|*  Created		:	2002/12/16
|*  Description		:	CLP Printer emulator(interpreter) routine    
|*				
****************************************************************************/

#include <STDIO.H>
#include <string.h>
#include "globals.h"
#include "commbuf.h"
#include "prt_common.h"
#include "prtdrv.h"
#include "serial.h"
#include "flash.h"
#include "prt_render.h"
#include "bar_common.h"
#include "timer.h"
#include "sensor.h"
#include "prt_font.h"
#include "prt_interpreter.h"
#include "prt_line.h"
#include "prt_cmdtest.h"
#include "ethernet.h"
#include "prt_render.h"//HYP 20060427
#include "main.h"
#ifdef USE_SRAM_PRT_RENERING_BUF
#include "spi_sram.h"
#endif


//HUGEDATA	INTERPRETER_STRUCT	InterpreterStruct;	
NEARDATA INTERPRETER_STRUCT	InterpreterStruct;//HYP 20060412 _huge --> _near for small code size

HUGEDATA	INT8U	SenDCMPortValue;
//__no_init  volatile HUGEDATA INT8U SenDCMPort @ (CPLD_ADDR + 0x00); 		//WR
volatile HUGEDATA INT8U SenDCMPort;



_inline INT8U Inline_Interpreter_GetCharRxBuf(HUGEDATA COMM_BUF *CBuf) 
{
 	InterpreterStruct.UsedChars++;
	return GetCharRxBuf(CBuf);
}

INT16U Interpreter_GetWordRxBuf(HUGEDATA COMM_BUF *CBuf) 
{
	CWD	chw;

	chw.byte.left = Inline_Interpreter_GetCharRxBuf(CBuf);
	chw.byte.right = Inline_Interpreter_GetCharRxBuf(CBuf);
	return chw.word;
}

void CAS_EscLInterpreter(HUGEDATA COMM_BUF	*CBuf);
void CAS_GsInterpreter(HUGEDATA COMM_BUF	*CBuf);
void CAS_EnqInterpreter(HUGEDATA COMM_BUF	*CBuf);
void CAS_DleInterpreter(HUGEDATA COMM_BUF	*CBuf);
void CAS_StrLInterpreter(HUGEDATA COMM_BUF	*CBuf);
void CAS_BarLInterpreter(HUGEDATA COMM_BUF	*CBuf);
void CAS_LineInterpreter(HUGEDATA COMM_BUF	*CBuf);
void CAS_BmpLInterpreter(HUGEDATA COMM_BUF	*CBuf);

void CAS_BarRInterpreter(HUGEDATA COMM_BUF	*CBuf);
void CAS_StrRInterpreter(HUGEDATA COMM_BUF	*CBuf);
void CAS_EscRInterpreter(HUGEDATA COMM_BUF	*CBuf);
void CAS_BmpRInterpreter(HUGEDATA COMM_BUF	*CBuf);

#ifdef USE_CONTINUOUS_LABEL_WITH_INGREDIENT
extern INT16U find_ingredient_y_interpreter(HUGEDATA COMM_BUF *label_str);
extern short ingredient_height_offset;
extern short ingredient_y_pos;
extern INT8U ingredient_height_adjust_flag;
#endif

INT8U	vtab_pos_count;

void CASInterpreter(HUGEDATA COMM_BUF *CBuf)
{
	char sbuf;	
//	HUGEDATA	char *s_ptr;
//	char string_buf[50];
	BOOLEAN	loop;
	//HYP 20040225
	//Backup variable (Control Ch)/////
	char backup_Magx, backup_Magy, backup_ChRot, backup_Typeface;
	INT8U backup_Id1, backup_Id2;
	////////////////////////////////////

	if(PrtStruct.Mode == RECEIPT && PrtStruct.ReceiptModeState == RM_COMPLETE)	{
		PrtStruct.ReceiptModeState = RM_INIT;
		PrtDrvStruct.TphMemMaxRowNum = StrStruct.LineSp;
    #ifdef USE_SRAM_PRT_RENERING_BUF
        SramPrtMemory = SramTphMemoryA + StrStruct.LineSp*TPH_ROW_BYTE;
    #else //USE_SRAM_PRT_RENERING_BUF
		PrtStruct.PrtMemory = PrtDrvStruct.TphMemoryA + StrStruct.LineSp*TPH_ROW_BYTE;//start ptr
	#endif //USE_SRAM_PRT_RENERING_BUF
		//HYP 20041130
		//PrtStruct.PrintableHeight = MAX_FORM_LEN;
		PrtStruct.PrintableHeight = MAX_RECEIPT_LEN;
#ifdef USE_FORMFEED_WITH_PRE_PRINT //HYP 20060428
		PrtDrvTphMemoryClear(OFF); //tph memory clear
#else
		PrtDrvTphMemoryClear(); //tph memory clear
#endif
		PrtStruct.PrintableHeight = PrtStruct.PrintableHeight - StrStruct.LineSp;
		PrtStruct.PrintHeight = PrtStruct.PrintableHeight - PrtStruct.TMargin - PrtStruct.BMargin;
//		vtab_pos_count = 0;
	}
	if (CheckRxBuf(CBuf) && PrtDrvStruct.PrtState == PRT_STATE0I) {
		InterpreterStruct.CompleteFlag = CI_NOTCOMPLETE; //not complete
		InterpreterStruct.UsedChars = 0;
		sbuf = Inline_Interpreter_GetCharRxBuf(CBuf);
		switch(sbuf) {
			default:
			//HYP 20040621 
//				if(PrtStruct.Mode == RECEIPT && (INT8U)sbuf >= 0x20) {
				if(PrtStruct.Mode == RECEIPT && (((INT8U)sbuf >= 0x20) || (sbuf == FONT_CON_CH))) {
					PrtStruct.ReceiptModeState = RM_RENDERING;
					PrtStruct.StrBuffer[PrtStruct.StrBufferCount] = sbuf;
					PrtStruct.StrBufferCount++;
					loop = ON;
					while(CheckRxBuf(CBuf) && loop == ON) {
						sbuf = Inline_Interpreter_GetCharRxBuf(CBuf);
			//HYP 20040621 
//						if(sbuf == HT || (INT8U)sbuf >= 0x20) {
						if(sbuf == HT || (((INT8U)sbuf >= 0x20) || (sbuf == FONT_CON_CH))) {
							PrtStruct.StrBuffer[PrtStruct.StrBufferCount] = sbuf;
							PrtStruct.StrBufferCount++;
						} else {
							loop = OFF;
							RewindRxBuf(CBuf,1);
						}
					}
					InterpreterStruct.CompleteFlag = CI_COMPLETE;
				} else {
					InterpreterStruct.CompleteFlag = CI_INVALID;  //invalid command
					InterpreterStruct.PreInterpreter = IDLE;
				}
				break;
			case FF:    // Form Feed
				if(PrtStruct.Mode == LABEL || PrtStruct.ReceiptModeState == RM_INIT) {
					PrtDrvStart(PRT_STATE_FFS);
				}
				InterpreterStruct.CompleteFlag = CI_COMPLETE;
				break;
			case HT:
				if(PrtStruct.Mode == RECEIPT) {
					PrtStruct.ReceiptModeState = RM_RENDERING;
					PrtStruct.StrBuffer[PrtStruct.StrBufferCount] = sbuf;
					PrtStruct.StrBufferCount++;
					InterpreterStruct.CompleteFlag = CI_COMPLETE;				
				}
				break;
/*
			case VT:
				if(PrtStruct.Mode == RECEIPT) {
					PrtStruct.ReceiptModeState = RM_RENDERING;
					if (PrtDrvStruct.TphMemMaxRowNum < StrStruct.VTab[vtab_pos_count] && StrStruct.VTab[vtab_pos_count] != 0) {
						PrtDrvStruct.TphMemMaxRowNum = StrStruct.VTab[vtab_pos_count];
						PrtStruct.PrtMemory = PrtStruct.PrtMemory + StrStruct.VTab[vtab_pos_count]*TPH_ROW_BYTE;
						PrtStruct.PrintableHeight = PrtStruct.PrintableHeight - StrStruct.VTab[vtab_pos_count];
						PrtStruct.PrintHeight = PrtStruct.PrintableHeight - PrtStruct.TMargin - PrtStruct.BMargin;
					} else {
						PrtStruct.PrtMemory = PrtStruct.PrtMemory + StrStruct.VTab[vtab_pos_count]*TPH_ROW_BYTE;
					}
					vtab_pos_count++;
				}
				InterpreterStruct.CompleteFlag = CI_COMPLETE;
				break;
*/
			case LF:    // Line Feed
//				PutCStrTxBuf(CBuf, "linefeed");
				InterpreterStruct.CompleteFlag = CI_COMPLETE;
				if(PrtStruct.Mode == RECEIPT) {
					PrtStruct.ReceiptModeState = RM_RENDERING;
					StrStruct.StrHeight = PrtStruct.PrintHeight;
					StrStruct.StrWidth = PrtStruct.PrintWidth - 1;
					if (PrtStruct.StrBufferCount != 0) {
						PrtStruct.x	= 0;
						PrtStruct.y = 0;
						PrtStruct.StrBuffer[PrtStruct.StrBufferCount] = sbuf;
						PrtStruct.StrBuffer[PrtStruct.StrBufferCount + 1] = '\0';
						//HYP 20040225
						//Backup variable (Control Ch)/////
						backup_Magx = StrStruct.Magx;
						backup_Magy = StrStruct.Magy;
						backup_ChRot = StrStruct.ChRot;
						backup_Typeface = StrStruct.Typeface;
						backup_Id1 = StrStruct.Id1;
						backup_Id2 = StrStruct.Id2;
						////////////////////////////////////						
						FontStrOut(PrtStruct.StrBuffer);
						//PutStrTxBuf(&CommBuf, PrtStruct.StrBuffer);
						//sprintf(string_buf,"s=%d,lh=%d,tn=%d,",PrtStruct.ReceiptLineHeight,PrtDrvStruct.TphMemMaxRowNum,StrStruct.StrWidth);
						//PutStrTxBuf(&CommBuf,string_buf);
						//sprintf(string_buf,"sh=%d,sw=%d",StrStruct.StrHeight,StrStruct.StrWidth);
						//PutStrTxBuf(&CommBuf,string_buf);
					} else {
						PrtStruct.ReceiptLineHeight = PrtDrvStruct.LineFeedLength;
					}
					PrtDrvStruct.PrevTphMemMaxRowNum = PrtDrvStruct.TphMemMaxRowNum;
					PrtDrvStruct.TphMemMaxRowNum = PrtDrvStruct.TphMemMaxRowNum + PrtStruct.ReceiptLineHeight + StrStruct.LineSp;
					//HYP 20041130
					//if (PrtStruct.ReceiptLineHeight == 0 || PrtDrvStruct.TphMemMaxRowNum >= MAX_FORM_LEN) {
					if (PrtStruct.ReceiptLineHeight == 0 || PrtDrvStruct.TphMemMaxRowNum >= MAX_RECEIPT_LEN) {
						//HYP 20040225
						//Recover Backup variable (Control Ch)/////
						//HYP 20040426
					   	if (PrtStruct.StrBufferCount != 0) {
						   	StrStruct.Magx	  = backup_Magx;
						   	StrStruct.Magy	  = backup_Magy;
						   	StrStruct.ChRot	  = backup_ChRot;
						    	StrStruct.Typeface= backup_Typeface;
						   	StrStruct.Id1	  = backup_Id1;
						   	StrStruct.Id2	  = backup_Id2;
						}
						////////////////////////////////////						
						PrtDrvStruct.TphMemMaxRowNum = PrtDrvStruct.PrevTphMemMaxRowNum - StrStruct.LineSp;
						//sprintf(string_buf,"**l:%d=%d-%d**",PrtDrvStruct.TphMemMaxRowNum,PrtDrvStruct.PrevTphMemMaxRowNum,StrStruct.LineSp);
						//PutStrTxBuf(&CommBuf,string_buf);
						InterpreterStruct.CompleteFlag = CI_NOTCOMPLETE;
						if (PrtDrvStruct.PrtState == PRT_STATE0I) {	 //prohibit duplicating start!!
							PrtDrvStruct.LabelCopyCount = 1;
							PrtDrvStart(PRT_STATE0S);
						}
					} else {
					#ifdef USE_SRAM_PRT_RENERING_BUF
//	                        SramTphMemory = SramTphMemory + (PrtStruct.ReceiptLineHeight + StrStruct.LineSp)*TPH_ROW_BYTE;
                        SramPrtMemory = SramPrtMemory + (PrtStruct.ReceiptLineHeight + StrStruct.LineSp)*TPH_ROW_BYTE;
                    #else // USE_SRAM_PRT_RENERING_BUF
						PrtStruct.PrtMemory = PrtStruct.PrtMemory + (PrtStruct.ReceiptLineHeight + StrStruct.LineSp)*TPH_ROW_BYTE;
                    #endif // USE_SRAM_PRT_RENERING_BUF
                        PrtStruct.PrintableHeight = PrtStruct.PrintableHeight - (PrtStruct.ReceiptLineHeight + StrStruct.LineSp);
						PrtStruct.PrintHeight = PrtStruct.PrintableHeight - PrtStruct.TMargin - PrtStruct.BMargin;
						//sprintf(string_buf,"n=%d,h=%d,",PrtDrvStruct.TphMemMaxRowNum,PrtStruct.PrintHeight);
						//PutStrTxBuf(&CommBuf,string_buf);
						//HYP20040430
						PrtStruct.StrBufferCount = 0; ///// only LF is rebuild
					}
				} else {
					PrtDrvStart(PRT_STATE_LFS);	 /////
				}
				break;
			case BS:
//				PutCStrTxBuf(CBuf, "reversefeed");
#ifdef PRT_REVERSE_FEED
				PrtDrvStart(PRT_STATE_RFS);	 /////
#endif
				InterpreterStruct.CompleteFlag = CI_COMPLETE;
				break;
			case _ESC:
//			case '?':
				if (CheckRxBuf(CBuf)) {
					if (PrtStruct.Mode == LABEL) {
						InterpreterStruct.PreInterpreter = ESC_INTERPRETER;	
						CAS_EscLInterpreter(CBuf);
					} else {
						CAS_EscRInterpreter(CBuf);
					}
				}  //if
				break;
			case GS:
//			case '!':
				if (CheckRxBuf(CBuf)) {
					InterpreterStruct.PreInterpreter = GS_INTERPRETER;
					CAS_GsInterpreter(CBuf);
				}  //if
				break;
			case ENQ:
//			case '^':
				if (CheckRxBuf(CBuf)) {
					InterpreterStruct.PreInterpreter = ENQ_INTERPRETER;
					CAS_EnqInterpreter(CBuf);
				}  //if
				break;
			case DLE:
//			case '~':
				if (CheckRxBuf(CBuf)) {
					InterpreterStruct.PreInterpreter = DLE_INTERPRETER;
					CAS_DleInterpreter(CBuf);	
				}  //if
				break;
			case ',':
				if (PrtStruct.Mode == LABEL) {			
					if (CheckRxBuf(CBuf)) {
						switch(InterpreterStruct.PreInterpreter) {
							default:
								InterpreterStruct.CompleteFlag = CI_INVALID;  //invalid command
								InterpreterStruct.PreInterpreter = IDLE;
								break;
							case ESC_INTERPRETER:
								CAS_EscLInterpreter(CBuf);
								break;
							case GS_INTERPRETER:
								CAS_GsInterpreter(CBuf);
								break;
							case ENQ_INTERPRETER:
								CAS_EnqInterpreter(CBuf);
								break;
							case DLE_INTERPRETER:
								CAS_DleInterpreter(CBuf);
								break;
							case STR_INTERPRETER:
								CAS_StrLInterpreter(CBuf);
								break;
							case BAR_INTERPRETER:
								CAS_BarLInterpreter(CBuf);
								break;
							case LINE_INTERPRETER:
								CAS_LineInterpreter(CBuf);
								break;
							case BMP_INTERPRETER:
								CAS_BmpLInterpreter(CBuf);
								break;
						}
					}
				} else {
					PrtStruct.StrBuffer[PrtStruct.StrBufferCount] = sbuf;
					PrtStruct.StrBufferCount++;
					InterpreterStruct.CompleteFlag = CI_COMPLETE;
				}	
				break;
		} //switch
		if ( InterpreterStruct.CompleteFlag == CI_NOTCOMPLETE ) RewindRxBuf(CBuf,InterpreterStruct.UsedChars);
	}
}

void CAS_EscLInterpreter(HUGEDATA COMM_BUF *CBuf) {
	char sbuf;	
//	char string_buf[20];
	INT16U wbuf;//HYP 20060412

	sbuf = Inline_Interpreter_GetCharRxBuf(CBuf);
	switch (sbuf) {
		default : 
			InterpreterStruct.CompleteFlag = CI_INVALID;
			InterpreterStruct.PreInterpreter = IDLE;
			break;
		case 'X':
			if (CheckRxBufCount(CBuf,2)) {
				PrtStruct.x = Interpreter_GetWordRxBuf(CBuf);
				InterpreterStruct.CompleteFlag = CI_COMPLETE;
 			}
			break;			
		case 'Y':
			if (CheckRxBufCount(CBuf,2)) {
#ifdef 	USE_CONTINUOUS_LABEL_WITH_INGREDIENT				
				PrtStruct.y = Interpreter_GetWordRxBuf(CBuf);
				if((PrtStruct.y > ingredient_y_pos) && ingredient_height_adjust_flag)
				{
					PrtStruct.y += ingredient_height_offset;
				}
				InterpreterStruct.CompleteFlag = CI_COMPLETE;
#else
				PrtStruct.y = Interpreter_GetWordRxBuf(CBuf);
				InterpreterStruct.CompleteFlag = CI_COMPLETE;
#endif
			}
			break;
		case 'R':
			if (CheckRxBuf(CBuf)) {
				PrtStruct.Rot = Inline_Interpreter_GetCharRxBuf(CBuf);
				InterpreterStruct.CompleteFlag = CI_COMPLETE;
			}
			break;
		case 'Q':
			if (CheckRxBuf(CBuf)) {
				PrtStruct.Opaque = Inline_Interpreter_GetCharRxBuf(CBuf);
				InterpreterStruct.CompleteFlag = CI_COMPLETE;
			}
			break;
		case 'M':
			if (CheckRxBuf(CBuf)) {
				StrStruct.Magx = Inline_Interpreter_GetCharRxBuf(CBuf);
				InterpreterStruct.CompleteFlag = CI_COMPLETE;
			}
			break;
		case 'N':
			if (CheckRxBuf(CBuf)) {
				StrStruct.Magy = Inline_Interpreter_GetCharRxBuf(CBuf);
				InterpreterStruct.CompleteFlag = CI_COMPLETE;
			}
			break;
		case 'J':
			if (CheckRxBufCount(CBuf,2)) {
				//HYP 20060412
				//when PrtStruct is _near, compiling isn't possible
				//PrtStruct.x = PrtStruct.x + Interpreter_GetWordRxBuf(CBuf);
				wbuf = Interpreter_GetWordRxBuf(CBuf);
				PrtStruct.x = PrtStruct.x + wbuf;
				InterpreterStruct.CompleteFlag = CI_COMPLETE;
 			}
			break;			
		case 'K':
			if (CheckRxBufCount(CBuf,2)) {
				//HYP 20060412
				//when PrtStruct is _near, compiling isn't possible
				//PrtStruct.y = PrtStruct.y + Interpreter_GetWordRxBuf(CBuf);
				wbuf = Interpreter_GetWordRxBuf(CBuf);
				PrtStruct.y = PrtStruct.y + wbuf;
				InterpreterStruct.CompleteFlag = CI_COMPLETE;
			}
			break;
		case 'S':
			if (CheckRxBuf(CBuf)) {
				InterpreterStruct.PreInterpreter = STR_INTERPRETER;
				CAS_StrLInterpreter(CBuf);	
			}  //if
			break;
		case 'B':
			if (CheckRxBuf(CBuf)) {
				InterpreterStruct.PreInterpreter = BAR_INTERPRETER;
				CAS_BarLInterpreter(CBuf);	
			}  //if
			break;		
		case 'L':
			if (CheckRxBuf(CBuf)) {
				InterpreterStruct.PreInterpreter = LINE_INTERPRETER;
				CAS_LineInterpreter(CBuf);	
			}  //if
			break;				
		case 'G':
			if (CheckRxBuf(CBuf)) {
				InterpreterStruct.PreInterpreter = BMP_INTERPRETER;
				CAS_BmpLInterpreter(CBuf);	
			}  //if
			break;			
		case '!':
			if (CheckRxBuf(CBuf)) {
				if (PrtDrvStruct.PrtState == PRT_STATE0I) {	 //prohibit duplicating start!!
//					PutCStrTxBuf(CBuf, "start print");
					PrtDrvStruct.LabelCopyCount = Inline_Interpreter_GetCharRxBuf(CBuf);
					PrtDrvStart(PRT_STATE0S);
					InterpreterStruct.CompleteFlag = CI_COMPLETE;
				} else {
//					PutCStrTxBuf(CBuf, "start error");
					InterpreterStruct.CompleteFlag = CI_INVALID;
				}
			}  //if
			break;
		case 'T':
			PrtTestPattern();
//			PutCStrTxBuf(CBuf, "Test Pattern");
			InterpreterStruct.CompleteFlag = CI_COMPLETE;
			break;
	}//esc switch
}

void CAS_GsInterpreter(HUGEDATA COMM_BUF	*CBuf) {
	char sbuf;	

	sbuf = Inline_Interpreter_GetCharRxBuf(CBuf);
	switch (sbuf) {
		default : 
			InterpreterStruct.CompleteFlag = CI_INVALID;
			InterpreterStruct.PreInterpreter = IDLE;
			break;
		case 'O':
			if (CheckRxBuf(CBuf)) {
				if (Inline_Interpreter_GetCharRxBuf(CBuf) == RECEIPT) {
					PrtLoadModeSetting(RECEIPT);
				} else {
					PrtLoadModeSetting(LABEL);
				}
				InterpreterStruct.CompleteFlag = CI_COMPLETE;
			}
			break;
		case 'G': //Sensor Cal
			PEEL_ON;
			GAP_ON;
//			PutCStrTxBuf(CBuf, "sensor check");
			PrtDrvStart(PRT_STATE_SCS);	 /////
			InterpreterStruct.CompleteFlag = CI_COMPLETE;
			break;
		case 'A': //Label Cal
			if(PrtStruct.Mode == LABEL) {
				PEEL_ON;
				GAP_ON;
//				PutCStrTxBuf(CBuf, "label check");
				PrtDrvStart(PRT_STATE_LCS);	 /////
			}
			InterpreterStruct.CompleteFlag = CI_COMPLETE;
			break;
		case 'V':
			PEEL_ON;
			GAP_ON;
			PrtDrvStart(PRT_STATE_CSS);
			InterpreterStruct.CompleteFlag = CI_COMPLETE;
			break;
		case 'P':
			if (CheckRxBuf(CBuf)) {
				PrtDrvStruct.PeelEnable = Inline_Interpreter_GetCharRxBuf(CBuf);
				InterpreterStruct.CompleteFlag = CI_COMPLETE;
			}
			break;
		case 'J':
			if (CheckRxBuf(CBuf)) {
				//HYP 20040330
				//PrtDrvStruct.LabelGapMode = Inline_Interpreter_GetCharRxBuf(CBuf);
				PrtDrvSetLabelGapMode(Inline_Interpreter_GetCharRxBuf(CBuf));
				InterpreterStruct.CompleteFlag = CI_COMPLETE;
			}
			break;
		case 'M':
			if (CheckRxBuf(CBuf)) {
				PrtDrvStruct.RewindEnable = Inline_Interpreter_GetCharRxBuf(CBuf);
				InterpreterStruct.CompleteFlag = CI_COMPLETE;
			}
			break;
		case 'Q':
			if (CheckRxBuf(CBuf)) {
				sbuf=Inline_Interpreter_GetCharRxBuf(CBuf);
				if(sbuf < 0x04) {
					PrtDrvSetSpeed(sbuf);
				} else if (sbuf >= '0' && sbuf <= '4') {
					PrtDrvSetSpeed(sbuf - '0');
				}
				InterpreterStruct.CompleteFlag = CI_COMPLETE;
			}
			break;
		case 'I':
			if (CheckRxBuf(CBuf)) {
				sbuf=Inline_Interpreter_GetCharRxBuf(CBuf);
				if(PrtStruct.Mode == LABEL) {
					PrtDrvSetDirection(sbuf);
				}
				InterpreterStruct.CompleteFlag = CI_COMPLETE;
			}
			break;
		case 'W':
			if (CheckRxBufCount(CBuf,2)) {
				if(PrtStruct.Mode == LABEL) {
					PrtSetLabelSize(Interpreter_GetWordRxBuf(CBuf),PrtStruct.LabelHeight);
				} else {
					//HYP 20040330
					//PrtSetReceiptWidth(Interpreter_GetWordRxBuf(CBuf));
					PrtStruct.LabelWidth = Interpreter_GetWordRxBuf(CBuf);
				}
				InterpreterStruct.CompleteFlag = CI_COMPLETE;
 			}
			break;			
		case 'H':
			if (CheckRxBufCount(CBuf,2)) {
				if(PrtStruct.Mode == LABEL) {
					PrtSetLabelSize(PrtStruct.LabelWidth,Interpreter_GetWordRxBuf(CBuf));
				} else {
					//HYP 20040330
					//Interpreter_GetWordRxBuf(CBuf);
					PrtStruct.LabelHeight = Interpreter_GetWordRxBuf(CBuf);
				}
				InterpreterStruct.CompleteFlag = CI_COMPLETE;
			}
			break;
		//HYP 20040330
		//GapLength			
		case 'U':
			if (CheckRxBufCount(CBuf,2)) {
				PrtDrvStruct.GapLength = Interpreter_GetWordRxBuf(CBuf);
				InterpreterStruct.CompleteFlag = CI_COMPLETE;
			}
			break;
		//HYP 20040330
		case 'X':
			if (CheckRxBufCount(CBuf,2)) {
				if(PrtStruct.Mode == LABEL) {
					PrtStruct.ReceiptWidth = Interpreter_GetWordRxBuf(CBuf);
				} else {
					PrtSetReceiptWidth(Interpreter_GetWordRxBuf(CBuf));
				}
				InterpreterStruct.CompleteFlag = CI_COMPLETE;
 			}
			break;
		//HYP 20040330			
		case 'Y':
			if (CheckRxBufCount(CBuf,2)) {
				PrtSetReceiptHeight(Interpreter_GetWordRxBuf(CBuf));
				InterpreterStruct.CompleteFlag = CI_COMPLETE;
			}
			break;
		//HYP 20040330
		//Rct Feed			
		case 'K':
			if (CheckRxBufCount(CBuf,2)) {
				PrtSetReceiptFeedHeight(Interpreter_GetWordRxBuf(CBuf));
				InterpreterStruct.CompleteFlag = CI_COMPLETE;
			}
			break;
		case 'T':
			if (CheckRxBufCount(CBuf,2)) {
				PrtStruct.TMargin = Interpreter_GetWordRxBuf(CBuf);
				PrtSetMargin(PrtStruct.TMargin , PrtStruct.BMargin , PrtStruct.LMargin, PrtStruct.RMargin);
				InterpreterStruct.CompleteFlag = CI_COMPLETE;
 			}
			break;			
		case 'B':
			if (CheckRxBufCount(CBuf,2)) {
				PrtStruct.BMargin = Interpreter_GetWordRxBuf(CBuf);
				PrtSetMargin(PrtStruct.TMargin , PrtStruct.BMargin , PrtStruct.LMargin, PrtStruct.RMargin);
				InterpreterStruct.CompleteFlag = CI_COMPLETE;
			}
			break;
		case 'L':
			if (CheckRxBufCount(CBuf,2)) {
				PrtStruct.LMargin = Interpreter_GetWordRxBuf(CBuf);
				PrtSetMargin(PrtStruct.TMargin , PrtStruct.BMargin , PrtStruct.LMargin, PrtStruct.RMargin);
				InterpreterStruct.CompleteFlag = CI_COMPLETE;
 			}
			break;			
		case 'R':
			if (CheckRxBufCount(CBuf,2)) {
				PrtStruct.RMargin = Interpreter_GetWordRxBuf(CBuf);
				PrtSetMargin(PrtStruct.TMargin , PrtStruct.BMargin , PrtStruct.LMargin, PrtStruct.RMargin);
				InterpreterStruct.CompleteFlag = CI_COMPLETE;
			}
			break;
		case 'S':
			if (CheckRxBuf(CBuf)) {
				Inline_Interpreter_GetCharRxBuf(CBuf);
            #ifdef USE_SRAM_PRT_RENERING_BUF
                SramPrtMemory = SramTphMemoryB;
            #else // USE_SRAM_PRT_RENERING_BUF
				PrtStruct.PrtMemory = PrtDrvStruct.TphMemoryB;
            #endif // USE_SRAM_PRT_RENERING_BUF
				PrtMemoryClear();
				StrFieldInit();
				BarFieldInit();
				InterpreterStruct.CompleteFlag = CI_COMPLETE;
			}
			break;
		case 'E':
        #ifdef USE_SRAM_PRT_RENERING_BUF
            SramPrtMemory = SramTphMemoryA;
        #else // USE_SRAM_PRT_RENERING_BUF
			PrtStruct.PrtMemory = PrtDrvStruct.TphMemoryA;
        #endif // USE_SRAM_PRT_RENERING_BUF
#ifdef USE_ONLY_1EA_PRT_RENDERING_BUF
            //none
#else            
#ifdef USE_FORMFEED_WITH_PRE_PRINT //HYP 20060428
			PrtDrvTphMemoryCopy((HUGEDATA INT16U *)PrtDrvStruct.TphMemoryA,(HUGEDATA INT16U *)PrtDrvStruct.TphMemoryB,OFF);
#else
			PrtDrvTphMemoryCopy((HUGEDATA INT16U *)PrtDrvStruct.TphMemoryA,(HUGEDATA INT16U *)PrtDrvStruct.TphMemoryB);
#endif
#endif
			PrtDrvStruct.ExistBackGround = ON;
			InterpreterStruct.CompleteFlag = CI_COMPLETE;
			break;
		case 'C':
        #ifdef USE_SRAM_PRT_RENERING_BUF
            SramPrtMemory = SramTphMemoryB;
        #else // USE_SRAM_PRT_RENERING_BUF
			PrtStruct.PrtMemory = PrtDrvStruct.TphMemoryB;
        #endif // USE_SRAM_PRT_RENERING_BUF
			PrtMemoryClear();
        #ifdef USE_SRAM_PRT_RENERING_BUF
            SramPrtMemory = SramTphMemoryA;
        #else // USE_SRAM_PRT_RENERING_BUF
			PrtStruct.PrtMemory = PrtDrvStruct.TphMemoryA;
        #endif // USE_SRAM_PRT_RENERING_BUF 
			PrtDrvStruct.ExistBackGround = OFF;
			InterpreterStruct.CompleteFlag = CI_COMPLETE;
			break;
		case 'Z':
#ifdef USE_FORMFEED_WITH_PRE_PRINT //HYP 20060428
			PrtDrvTphMemoryClear(OFF);
#else
			PrtDrvTphMemoryClear();
#endif
			InterpreterStruct.CompleteFlag = CI_COMPLETE;
			break;
		//HYP 20040430
		case '%':
			if (PrtDrvStruct.PrtState == PRT_STATE0I) {
				PrtDrvStart(PRT_STATE_RW);
				InterpreterStruct.CompleteFlag = CI_COMPLETE;
			} else {
				InterpreterStruct.CompleteFlag = CI_INVALID;
			}				
			break;
	}
}

void CAS_EnqInterpreter(HUGEDATA COMM_BUF	*CBuf) {
	char sbuf;	
//	char string_buf[100];

	sbuf = Inline_Interpreter_GetCharRxBuf(CBuf);
	switch(sbuf) {
		default:
			InterpreterStruct.CompleteFlag = CI_INVALID;  //invalid command
			InterpreterStruct.PreInterpreter = IDLE;
			break;
	}
}

void CAS_DleInterpreter(HUGEDATA COMM_BUF	*CBuf) {
	char sbuf;	
//	char string_buf[20];
//	HUGEDATA char *s_ptr;

	sbuf = Inline_Interpreter_GetCharRxBuf(CBuf);
	switch (sbuf) {
		default : 
			InterpreterStruct.CompleteFlag = CI_INVALID;
			InterpreterStruct.PreInterpreter = IDLE;
			break;
	}
}

#ifdef USE_AUSTRALIA_COOL_FUNCTION
INT8U reverse_percent;
extern ROMDATA INT8U Image_Form_logo[];
extern ROMDATA INT8U Image_Form_bar[];
#endif
void CAS_StrLInterpreter(HUGEDATA COMM_BUF	*CBuf) {
	char sbuf;	
	HUGEDATA char *s_ptr;
	INT8U field_num;

	sbuf = Inline_Interpreter_GetCharRxBuf(CBuf);
	switch (sbuf) {
		default : 
			InterpreterStruct.CompleteFlag = CI_INVALID;
			InterpreterStruct.PreInterpreter = IDLE;
			break;
		case 'X':
			if (CheckRxBufCount(CBuf,2)) {
				PrtStruct.x = Interpreter_GetWordRxBuf(CBuf);
				InterpreterStruct.CompleteFlag = CI_COMPLETE;
 			}
			break;
		case 'Y':
			if (CheckRxBufCount(CBuf,2)) {
#ifdef 	USE_CONTINUOUS_LABEL_WITH_INGREDIENT				
				PrtStruct.y = Interpreter_GetWordRxBuf(CBuf);
				if((PrtStruct.y > ingredient_y_pos) && ingredient_height_adjust_flag)
				{
					PrtStruct.y += ingredient_height_offset;
				}
				InterpreterStruct.CompleteFlag = CI_COMPLETE;
#else
				PrtStruct.y = Interpreter_GetWordRxBuf(CBuf);
				InterpreterStruct.CompleteFlag = CI_COMPLETE;
#endif
			}
			break;
		case 'R':
			if (CheckRxBuf(CBuf)) {
				PrtStruct.Rot = Inline_Interpreter_GetCharRxBuf(CBuf);
				InterpreterStruct.CompleteFlag = CI_COMPLETE;
			}
			break;
		case 'Q':
			if (CheckRxBuf(CBuf)) {
				PrtStruct.Opaque = Inline_Interpreter_GetCharRxBuf(CBuf);
				InterpreterStruct.CompleteFlag = CI_COMPLETE;
			}
			break;
		case 'M':
			if (CheckRxBuf(CBuf)) {
				StrStruct.Magx = Inline_Interpreter_GetCharRxBuf(CBuf);
				InterpreterStruct.CompleteFlag = CI_COMPLETE;
			}
			break;
		case 'N':
			if (CheckRxBuf(CBuf)) {
				StrStruct.Magy = Inline_Interpreter_GetCharRxBuf(CBuf);
				InterpreterStruct.CompleteFlag = CI_COMPLETE;
			}
			break;
		case 'W':
			if (CheckRxBufCount(CBuf,2)) {
				StrStruct.StrWidth = Interpreter_GetWordRxBuf(CBuf);
				InterpreterStruct.CompleteFlag = CI_COMPLETE;
 			}
			break;			
		case 'H':
			if (CheckRxBufCount(CBuf,2)) {
				StrStruct.StrHeight = Interpreter_GetWordRxBuf(CBuf);
				InterpreterStruct.CompleteFlag = CI_COMPLETE;
			}
			break;
		case 'J':
			if (CheckRxBuf(CBuf)) {
				StrStruct.Justification = Inline_Interpreter_GetCharRxBuf(CBuf);
				InterpreterStruct.CompleteFlag = CI_COMPLETE;
			}
			break;
		case 'L':
			if (CheckRxBuf(CBuf)) {
				StrStruct.VCentering = Inline_Interpreter_GetCharRxBuf(CBuf);
				InterpreterStruct.CompleteFlag = CI_COMPLETE;
			}
			break;
		case 'S':
			if (CheckRxBuf(CBuf)) {
				StrStruct.WordWrap = Inline_Interpreter_GetCharRxBuf(CBuf);
				InterpreterStruct.CompleteFlag = CI_COMPLETE;
			}
			break;
		case 'P':
			if (CheckRxBuf(CBuf)) {
				StrStruct.InterChSp = Inline_Interpreter_GetCharRxBuf(CBuf);
				InterpreterStruct.CompleteFlag = CI_COMPLETE;
			}
			break;
		case 'U':
			if (CheckRxBuf(CBuf)) {
				StrStruct.LineSp = Inline_Interpreter_GetCharRxBuf(CBuf);
				InterpreterStruct.CompleteFlag = CI_COMPLETE;
			}
			break;
		case 'A':
			if (CheckRxBuf(CBuf)) {
				FontLoadFont1(Inline_Interpreter_GetCharRxBuf(CBuf));
				InterpreterStruct.CompleteFlag = CI_COMPLETE;
			}
			break;
		case 'B':
			if (CheckRxBuf(CBuf)) {
				FontLoadFont2(Inline_Interpreter_GetCharRxBuf(CBuf));
				InterpreterStruct.CompleteFlag = CI_COMPLETE;
			}
			break;
		case 'G':
			if (CheckRxBuf(CBuf)) {
				StrStruct.StrType = Inline_Interpreter_GetCharRxBuf(CBuf);
				InterpreterStruct.CompleteFlag = CI_COMPLETE;
			}
			break;
		case 'O':
			if (CheckRxBuf(CBuf)) {
				StrStruct.ChRot = Inline_Interpreter_GetCharRxBuf(CBuf);
				InterpreterStruct.CompleteFlag = CI_COMPLETE;
			}
			break;
		case 'E':
			if (CheckRxBuf(CBuf)) {
				StrStruct.StrMirx = Inline_Interpreter_GetCharRxBuf(CBuf);
				InterpreterStruct.CompleteFlag = CI_COMPLETE;
			}
			break;
		case 'I':
			if (CheckRxBuf(CBuf)) {
				StrStruct.AutoShrinkx = Inline_Interpreter_GetCharRxBuf(CBuf);
				InterpreterStruct.CompleteFlag = CI_COMPLETE;
			}
			break;
		case 'V':
			if (CheckRxBuf(CBuf)) {
				StrStruct.UseFixedWidth = Inline_Interpreter_GetCharRxBuf(CBuf);
				InterpreterStruct.CompleteFlag = CI_COMPLETE;
			}
			break;
		case '/':
			StrStruct.Typeface = StrStruct.Typeface | ITALIC;
			InterpreterStruct.CompleteFlag = CI_COMPLETE;
			break;
		case '!':
			StrStruct.Typeface = StrStruct.Typeface | OUTLINE;
			InterpreterStruct.CompleteFlag = CI_COMPLETE;
			break;
		case '_':
			StrStruct.Typeface = StrStruct.Typeface | UNDERLINE;
			InterpreterStruct.CompleteFlag = CI_COMPLETE;
			break;
		case '-':
			StrStruct.Typeface = StrStruct.Typeface | THROUGH_LINE;
			InterpreterStruct.CompleteFlag = CI_COMPLETE;
			break;
		case '+':
			StrStruct.Typeface = StrStruct.Typeface | SHADOW;
			InterpreterStruct.CompleteFlag = CI_COMPLETE;
			break;
		case '*':
			StrStruct.Typeface = StrStruct.Typeface | BOLD;
			InterpreterStruct.CompleteFlag = CI_COMPLETE;
			break;
		case '~':
			StrStruct.Typeface = StrStruct.Typeface | REVERSE;
			InterpreterStruct.CompleteFlag = CI_COMPLETE;
			break;
		case '=':
			StrStruct.Typeface = StrStruct.Typeface | DTHROUGH_LINE;
			InterpreterStruct.CompleteFlag = CI_COMPLETE;
			break;
		case '@':
			StrStruct.Typeface = NO_TYPEFACE;
			InterpreterStruct.CompleteFlag = CI_COMPLETE;
			break;
		case '[':
			PrtStruct.StrAreaOverFlag = FALSE;//HYP 20040903 not applied to PC program (PrtRender)
			s_ptr = PrtStruct.StrBuffer;
			while (CheckRxBuf(CBuf)) {
				sbuf = Inline_Interpreter_GetCharRxBuf(CBuf);
				if(sbuf == '\0') {
					*s_ptr = '\0';
//					FontStrOut(PrtStruct.StrBuffer);//HYP 20060427 change to below code
//HYP 20060427 str field is ignored in pre-print area(except shop data)
					field_num = PrtStruct.CurrentStrFieldNum;
					if (PrtDrvStruct.PrePrintMode == ENABLE) { 
						if ((field_num == 0)  //for fixed str during label load
						|| (field_num == FIELD_ID_SHOP_NAME_STR)  
						|| (field_num == FIELD_ID_SHOP_TEL_STR)	  
						|| (field_num == FIELD_ID_SHOP_TEXT_STR)) {
							CheckPrePrtAreaBoundary = OFF;
						} else {
							CheckPrePrtAreaBoundary = ON;
						}
					} else {
						CheckPrePrtAreaBoundary = OFF;
					}
#ifdef USE_AUSTRALIA_COOL_FUNCTION
					if(field_num == FIELD_ID_BONUS_POINT) 
					{
						cool_field_id_bonus_point_func();	
					}
					else
					{
						FontStrOut(PrtStruct.StrBuffer);
					}
#else
					FontStrOut(PrtStruct.StrBuffer);
#endif
					CheckPrePrtAreaBoundary = OFF;
					InterpreterStruct.CompleteFlag = CI_COMPLETE;
					break;
				}
				*s_ptr = sbuf;
				s_ptr++;
			}
			//HYP 20040903 not applied to PC program (PrtRender)
			switch(PrtStruct.CurrentStrFieldNum) {
				case FIELD_ID_TOTALPRICE:
				case FIELD_ID_NEW_TOTALPRICE:
				case FIELD_ID_MARK_DN_TOTAL:
					if(PrtStruct.StrAreaOverFlag == TRUE) {
						FontEraseFieldArea(PrtStruct.CurrentStrFieldNum);
						//Insert Main Field Area over flag
					}
					break;
				default:
					break;
			}

			break;
//save field
		case 'F':
			if (CheckRxBuf(CBuf)) {
				FontSaveField(Inline_Interpreter_GetCharRxBuf(CBuf));
				InterpreterStruct.CompleteFlag = CI_COMPLETE;
 			}
			break;
//load field 
		case '{':
			if (CheckRxBuf(CBuf)) {
				PrtStruct.StrAreaOverFlag = FALSE;//HYP 20040903 not applied to PC program (PrtRender)
				field_num = Inline_Interpreter_GetCharRxBuf(CBuf);
				s_ptr = PrtStruct.StrBuffer;
				while (CheckRxBuf(CBuf)) {
					sbuf = Inline_Interpreter_GetCharRxBuf(CBuf);
					if(sbuf == '\0') {
						*s_ptr = '\0';
						if(FontLoadField(field_num) != 0xff) {
						//	FontStrOut(PrtStruct.StrBuffer); //HYP 20060427 change to below code
//HYP 20060427 str field is ignored in pre-print area(except shop data)
//fixed str, fixed barcode, bitmap, line is accepted
							if (PrtDrvStruct.PrePrintMode == ENABLE) { 
								if((field_num == FIELD_ID_SHOP_NAME_STR)  
								|| (field_num == FIELD_ID_SHOP_TEL_STR)	 
								|| (field_num == FIELD_ID_SHOP_TEXT_STR)) {
									CheckPrePrtAreaBoundary = OFF;
								} else {
									CheckPrePrtAreaBoundary = ON;
								}
							} else {
								CheckPrePrtAreaBoundary = OFF;
							}
							FontStrOut(PrtStruct.StrBuffer);
							CheckPrePrtAreaBoundary = OFF;
						}
						InterpreterStruct.CompleteFlag = CI_COMPLETE;
						break;
					}
					*s_ptr = sbuf;
					s_ptr++;
				}
				//HYP 20040903 not applied to PC program (PrtRender)
				switch(PrtStruct.CurrentStrFieldNum) {
					case FIELD_ID_TOTALPRICE:
					case FIELD_ID_NEW_TOTALPRICE:
					case FIELD_ID_MARK_DN_TOTAL:
						if(PrtStruct.StrAreaOverFlag == TRUE) {
							FontEraseFieldArea(field_num);
							//Insert Main Field Area over flag
						}
						break;
					default:
						break;
				}
			}
			break;
	}//esc switch
}

void CAS_BarLInterpreter(HUGEDATA COMM_BUF	*CBuf) {
	char sbuf;	
	HUGEDATA char *s_ptr;
	char nb,ns,wb,ws;
	INT8U field_num;

	sbuf = Inline_Interpreter_GetCharRxBuf(CBuf);
	switch (sbuf) {
		default : 
			InterpreterStruct.CompleteFlag = CI_INVALID;
			InterpreterStruct.PreInterpreter = IDLE;
			break;
		case 'X':
			if (CheckRxBufCount(CBuf,2)) {
				PrtStruct.x = Interpreter_GetWordRxBuf(CBuf);
				InterpreterStruct.CompleteFlag = CI_COMPLETE;
 			}
			break;		
		case 'Y':
			if (CheckRxBufCount(CBuf,2)) {
#ifdef 	USE_CONTINUOUS_LABEL_WITH_INGREDIENT				
				PrtStruct.y = Interpreter_GetWordRxBuf(CBuf);
				if((PrtStruct.y > ingredient_y_pos) && ingredient_height_adjust_flag)
				{
					PrtStruct.y += ingredient_height_offset;
				}
				InterpreterStruct.CompleteFlag = CI_COMPLETE;
#else
				PrtStruct.y = Interpreter_GetWordRxBuf(CBuf);
				InterpreterStruct.CompleteFlag = CI_COMPLETE;
#endif
			}
			break;
		case 'R':
			if (CheckRxBuf(CBuf)) {
				PrtStruct.Rot = Inline_Interpreter_GetCharRxBuf(CBuf);
				InterpreterStruct.CompleteFlag = CI_COMPLETE;
			}
			break;
		case 'Q':
			if (CheckRxBuf(CBuf)) {
				PrtStruct.Opaque = Inline_Interpreter_GetCharRxBuf(CBuf);
				InterpreterStruct.CompleteFlag = CI_COMPLETE;
			}
			break;
		case 'T':
			if (CheckRxBuf(CBuf)) {
				BarSetBarType(Inline_Interpreter_GetCharRxBuf(CBuf));
				InterpreterStruct.CompleteFlag = CI_COMPLETE;
			}
			break;
		case 'H':
			if (CheckRxBufCount(CBuf,6)) {
				BarSetFullHeight((short)Interpreter_GetWordRxBuf(CBuf));
				BarSetBarHeight((short)Interpreter_GetWordRxBuf(CBuf));
				BarSetCharHeight((short)Interpreter_GetWordRxBuf(CBuf));
				InterpreterStruct.CompleteFlag = CI_COMPLETE;
 			}
			break;
		case 'W':
			if (CheckRxBuf(CBuf)) {
				BarSetWidthMag(Inline_Interpreter_GetCharRxBuf(CBuf));
				InterpreterStruct.CompleteFlag = CI_COMPLETE;
			}
			break;
		case 'I':
			if (CheckRxBufCount(CBuf,4)) {
				nb = Inline_Interpreter_GetCharRxBuf(CBuf);
				ns = Inline_Interpreter_GetCharRxBuf(CBuf);
				wb = Inline_Interpreter_GetCharRxBuf(CBuf);
				ws = Inline_Interpreter_GetCharRxBuf(CBuf);
				BarSetNSWS(nb,ns,wb,ws);
				InterpreterStruct.CompleteFlag = CI_COMPLETE;
 			}
			break;
		case 'S':
			if (CheckRxBuf(CBuf)) {
				BarStruct.PrtHri = Inline_Interpreter_GetCharRxBuf(CBuf);
				InterpreterStruct.CompleteFlag = CI_COMPLETE;
			}
			break;
		case 'P':
			if (CheckRxBuf(CBuf)) {
				BarStruct.BarHriPos = Inline_Interpreter_GetCharRxBuf(CBuf);
				InterpreterStruct.CompleteFlag = CI_COMPLETE;
			}
			break;
		case 'C':
			if (CheckRxBuf(CBuf)) {
				BarStruct.PrtCheckHri = Inline_Interpreter_GetCharRxBuf(CBuf);
				InterpreterStruct.CompleteFlag = CI_COMPLETE;
			}
			break;
		case 'O':
			if (CheckRxBuf(CBuf)) {
				BarStruct.PrtStartStopHri = Inline_Interpreter_GetCharRxBuf(CBuf);
				InterpreterStruct.CompleteFlag = CI_COMPLETE;
			}
			break;
		case 'B':
			if (CheckRxBuf(CBuf)) {
				BarStruct.UseOcrb = Inline_Interpreter_GetCharRxBuf(CBuf);
				InterpreterStruct.CompleteFlag = CI_COMPLETE;
			}
			break;
		case 'G':
			if (CheckRxBuf(CBuf)) {
				BarSetGuard(Inline_Interpreter_GetCharRxBuf(CBuf));
				InterpreterStruct.CompleteFlag = CI_COMPLETE;
			}
			break;
		case 'E':
			if (CheckRxBuf(CBuf)) {
				BarStruct.BearerBarWidth = Inline_Interpreter_GetCharRxBuf(CBuf);
				InterpreterStruct.CompleteFlag = CI_COMPLETE;
			}
			break;
		case 'U':
			if (CheckRxBuf(CBuf)) {
				BarStruct.QuietZone = Inline_Interpreter_GetCharRxBuf(CBuf);
				InterpreterStruct.CompleteFlag = CI_COMPLETE;
			}
			break;
		//20040211 HYP
		case 'K':
			if (CheckRxBuf(CBuf)) {
				BarStruct.CheckType = Inline_Interpreter_GetCharRxBuf(CBuf);
				InterpreterStruct.CompleteFlag = CI_COMPLETE;
			}
			break;
		case '[':
			s_ptr = PrtStruct.BarBuffer;
			while (CheckRxBuf(CBuf)) {
				sbuf = Inline_Interpreter_GetCharRxBuf(CBuf);
				if(sbuf == '\0') {
					*s_ptr = '\0';
//					BarcodeOut(PrtStruct.BarBuffer);//HYP 20060427 change to below code
//HYP 20060427 bar field is ignored in pre-print area
					if (PrtDrvStruct.PrePrintMode == ENABLE) { 
						if(PrtStruct.CurrentBarFieldNum == 0) {////for fixed bar during label load
							CheckPrePrtAreaBoundary = OFF;
						} else {						
							CheckPrePrtAreaBoundary = ON;
						}
					} else {
						CheckPrePrtAreaBoundary = OFF;
					}
					BarcodeOut(PrtStruct.BarBuffer);
					CheckPrePrtAreaBoundary = OFF;
					InterpreterStruct.CompleteFlag = CI_COMPLETE;
					break;
				}
				*s_ptr = sbuf;
				s_ptr++;
			}
			break;
//save field
		case 'F':
			if (CheckRxBuf(CBuf)) {
				BarSaveField(Inline_Interpreter_GetCharRxBuf(CBuf));
				InterpreterStruct.CompleteFlag = CI_COMPLETE;
 			}
			break;
//load field 
		case '{':
			if (CheckRxBuf(CBuf)) {
				field_num = Inline_Interpreter_GetCharRxBuf(CBuf);
				s_ptr = PrtStruct.BarBuffer;
				while (CheckRxBuf(CBuf)) {
					sbuf = Inline_Interpreter_GetCharRxBuf(CBuf);
					if(sbuf == '\0') {
						*s_ptr = '\0';
						if(BarLoadField(field_num) != 0xff) {
						//	BarcodeOut(PrtStruct.BarBuffer); //HYP 20060427 change to below code
//HYP 20060427 bar field is ignored in pre-print area
							if (PrtDrvStruct.PrePrintMode == ENABLE) { 
								CheckPrePrtAreaBoundary = ON;
							} else {
								CheckPrePrtAreaBoundary = OFF;
							}
							BarcodeOut(PrtStruct.BarBuffer);
							CheckPrePrtAreaBoundary = OFF;
						}
						InterpreterStruct.CompleteFlag = CI_COMPLETE;
						break;
					}
					*s_ptr = sbuf;
					s_ptr++;
				}
			}
			break;

	}//esc switch
}

void CAS_LineInterpreter(HUGEDATA COMM_BUF	*CBuf) {
	char sbuf;	
	INT8U	width,bw;
	short	line_x,line_y;
//	char	string_buf[50];

	sbuf = Inline_Interpreter_GetCharRxBuf(CBuf);
	switch (sbuf) {
		default : 
			InterpreterStruct.CompleteFlag = CI_INVALID;
			InterpreterStruct.PreInterpreter = IDLE;
			break;
		case 'X':
			if (CheckRxBufCount(CBuf,2)) {
				PrtStruct.x = Interpreter_GetWordRxBuf(CBuf);
				InterpreterStruct.CompleteFlag = CI_COMPLETE;
 			}
			break;		
		case 'Y':
			if (CheckRxBufCount(CBuf,2)) {
				PrtStruct.y = Interpreter_GetWordRxBuf(CBuf);
#ifdef 	USE_CONTINUOUS_LABEL_WITH_INGREDIENT				
				if((PrtStruct.y > ingredient_y_pos) && ingredient_height_adjust_flag)
				{
					PrtStruct.y += ingredient_height_offset;
				}
#endif
				InterpreterStruct.CompleteFlag = CI_COMPLETE;
			}
			break;
		case 'R':
			if (CheckRxBuf(CBuf)) {
				PrtStruct.Rot = Inline_Interpreter_GetCharRxBuf(CBuf);
				InterpreterStruct.CompleteFlag = CI_COMPLETE;
			}
			break;
		case 'Q':
			if (CheckRxBuf(CBuf)) {
				PrtStruct.Opaque = Inline_Interpreter_GetCharRxBuf(CBuf);
				InterpreterStruct.CompleteFlag = CI_COMPLETE;
			}
			break;
		case 'H':	// horizontal Line
			if (CheckRxBufCount(CBuf,4)) {
				width = Inline_Interpreter_GetCharRxBuf(CBuf);
				bw = Inline_Interpreter_GetCharRxBuf(CBuf);
				line_x = Interpreter_GetWordRxBuf(CBuf);
				PrtHLine(PrtStruct.x,line_x,PrtStruct.y,width,bw);
				InterpreterStruct.CompleteFlag = CI_COMPLETE;
			}
 			break;
		case 'V':	// Vertical Line
			if (CheckRxBufCount(CBuf,4)) {
				width = Inline_Interpreter_GetCharRxBuf(CBuf);
				bw = Inline_Interpreter_GetCharRxBuf(CBuf);
				line_y = Interpreter_GetWordRxBuf(CBuf);
#ifdef 	USE_CONTINUOUS_LABEL_WITH_INGREDIENT
				if((line_y > ingredient_y_pos) && ingredient_height_adjust_flag)
				{
					line_y += ingredient_height_offset;
				}
#endif				
				PrtVLine(PrtStruct.y,line_y,PrtStruct.x,width,bw);
				InterpreterStruct.CompleteFlag = CI_COMPLETE;
			}
 			break;
		case 'B':	// Draw box
			if (CheckRxBufCount(CBuf,6)) {
				width = Inline_Interpreter_GetCharRxBuf(CBuf);
				bw = Inline_Interpreter_GetCharRxBuf(CBuf);
				line_x = Interpreter_GetWordRxBuf(CBuf);
				line_y = Interpreter_GetWordRxBuf(CBuf);
#ifdef 	USE_CONTINUOUS_LABEL_WITH_INGREDIENT
				if((line_y > ingredient_y_pos) && ingredient_height_adjust_flag)
				{
					line_y += ingredient_height_offset;
				}
#endif				
				PrtRect(PrtStruct.x,PrtStruct.y,line_x,line_y,width,bw);
				InterpreterStruct.CompleteFlag = CI_COMPLETE;
			}
 			break;
		case 'F':	// Draw fill box
			if (CheckRxBufCount(CBuf,5)) {
				bw = Inline_Interpreter_GetCharRxBuf(CBuf);
				line_x = Interpreter_GetWordRxBuf(CBuf);
				line_y = Interpreter_GetWordRxBuf(CBuf);
#ifdef 	USE_CONTINUOUS_LABEL_WITH_INGREDIENT				
				if((line_y > ingredient_y_pos) && ingredient_height_adjust_flag)
				{
					line_y += ingredient_height_offset;
				}
#endif				
				PrtRectFill(PrtStruct.x,PrtStruct.y,line_x,line_y,bw);
				InterpreterStruct.CompleteFlag = CI_COMPLETE;
			}
 			break;
 		case 'E':	// Draw Ellipse
			if (CheckRxBufCount(CBuf,5)) {
				width = Inline_Interpreter_GetCharRxBuf(CBuf);
				line_x = Interpreter_GetWordRxBuf(CBuf);
				line_y = Interpreter_GetWordRxBuf(CBuf);
#ifdef 	USE_CONTINUOUS_LABEL_WITH_INGREDIENT
				if((line_y > ingredient_y_pos) && ingredient_height_adjust_flag)
				{
					line_y += ingredient_height_offset;
				}
#endif				
				PrtEllipse(PrtStruct.x,PrtStruct.y,line_x,line_y,width);
				InterpreterStruct.CompleteFlag = CI_COMPLETE;
			}
 			break;
	}//esc switch
}

void CAS_BmpLInterpreter(HUGEDATA COMM_BUF	*CBuf) {
	char sbuf;	
//	char string_buf[20];

	sbuf = Inline_Interpreter_GetCharRxBuf(CBuf);
	switch (sbuf) {
		default : 
			InterpreterStruct.CompleteFlag = CI_INVALID;
			InterpreterStruct.PreInterpreter = IDLE;
			break;
		case 'X':
			if (CheckRxBufCount(CBuf,2)) {
				PrtStruct.x = Interpreter_GetWordRxBuf(CBuf);
//				sprintf(string_buf, "gx=%d",PrtStruct.x);
//				PutStrTxBuf(CBuf, string_buf);
				InterpreterStruct.CompleteFlag = CI_COMPLETE;
 			}
			break;		
		case 'Y':
			if (CheckRxBufCount(CBuf,2)) {
#ifdef 	USE_CONTINUOUS_LABEL_WITH_INGREDIENT				
				PrtStruct.y = Interpreter_GetWordRxBuf(CBuf);
				if((PrtStruct.y > ingredient_y_pos) && ingredient_height_adjust_flag)
				{
					PrtStruct.y += ingredient_height_offset;
				}
				InterpreterStruct.CompleteFlag = CI_COMPLETE;
#else
				PrtStruct.y = Interpreter_GetWordRxBuf(CBuf);
				InterpreterStruct.CompleteFlag = CI_COMPLETE;
#endif
			}
			break;
		case 'R':
			if (CheckRxBuf(CBuf)) {
				PrtStruct.Rot = Inline_Interpreter_GetCharRxBuf(CBuf);
				InterpreterStruct.CompleteFlag = CI_COMPLETE;
			}
			break;
		case 'Q':
			if (CheckRxBuf(CBuf)) {
				PrtStruct.Opaque = Inline_Interpreter_GetCharRxBuf(CBuf);
				InterpreterStruct.CompleteFlag = CI_COMPLETE;
			}
			break;
		case 'M':
			if (CheckRxBuf(CBuf)) {
				StrStruct.Magx = Inline_Interpreter_GetCharRxBuf(CBuf);
				InterpreterStruct.CompleteFlag = CI_COMPLETE;
			}
			break;
		case 'N':
			if (CheckRxBuf(CBuf)) {
				StrStruct.Magy = Inline_Interpreter_GetCharRxBuf(CBuf);
				InterpreterStruct.CompleteFlag = CI_COMPLETE;
			}
			break;
		case '{':
			if (CheckRxBuf(CBuf)) {
				BitmapOut((INT16U)Inline_Interpreter_GetCharRxBuf(CBuf));
				InterpreterStruct.CompleteFlag = CI_COMPLETE;
			}
			break;		
	}
}

void CAS_StrRInterpreter(HUGEDATA COMM_BUF	*CBuf) {
	char sbuf;	
//	HUGEDATA char *s_ptr;
	INT8U htab_table[MAX_HTAB_NUM];
	INT8U i;

	sbuf = Inline_Interpreter_GetCharRxBuf(CBuf);
	switch (sbuf) {
		default : 
			InterpreterStruct.CompleteFlag = CI_INVALID;
			InterpreterStruct.PreInterpreter = IDLE;
			break;
		case 'P':
			if (CheckRxBuf(CBuf)) {
				sbuf = Inline_Interpreter_GetCharRxBuf(CBuf);
				if(PrtStruct.StrBufferCount == 0) {
					StrStruct.InterChSp = sbuf;
				}
				InterpreterStruct.CompleteFlag = CI_COMPLETE;
			}
			break;
		case 'U':
			if (CheckRxBuf(CBuf)) {
				sbuf = Inline_Interpreter_GetCharRxBuf(CBuf);
				if(PrtStruct.StrBufferCount == 0) {
					StrStruct.LineSp = sbuf;
				}
				InterpreterStruct.CompleteFlag = CI_COMPLETE;
			}
			break;
		case 'J':
			if (CheckRxBuf(CBuf)) {
				sbuf = Inline_Interpreter_GetCharRxBuf(CBuf);
				if(PrtStruct.StrBufferCount == 0) {
					StrStruct.Justification = sbuf;
				}
				InterpreterStruct.CompleteFlag = CI_COMPLETE;
			}
			break;
		case 'T':
			if (CheckRxBufCount(CBuf,MAX_HTAB_NUM)) {
				if(PrtStruct.StrBufferCount == 0) {
					for (i=0; i<MAX_HTAB_NUM; i++) {
						htab_table[i] = Inline_Interpreter_GetCharRxBuf(CBuf);
					}
					PrtSetHTabPos(htab_table);
				} else {
					for (i=0; i<MAX_HTAB_NUM; i++) {
						Inline_Interpreter_GetCharRxBuf(CBuf);
					}
				}
				InterpreterStruct.CompleteFlag = CI_COMPLETE;
			}
			break;
		case 'V':
			if (CheckRxBuf(CBuf)) {
				sbuf = Inline_Interpreter_GetCharRxBuf(CBuf);
				if(PrtStruct.StrBufferCount == 0) {
					StrStruct.UseFixedWidth = sbuf;
				}
				InterpreterStruct.CompleteFlag = CI_COMPLETE;
			}
			break;
		case 'A':
			if (CheckRxBuf(CBuf)) {
				if(PrtStruct.StrBufferCount == 0) {
					FontLoadFont1(Inline_Interpreter_GetCharRxBuf(CBuf));
				} else {
					PrtStruct.StrBuffer[PrtStruct.StrBufferCount] = FONT_CON_CH;
					PrtStruct.StrBufferCount++;
					PrtStruct.StrBuffer[PrtStruct.StrBufferCount] = FONT_CON_FONT1;
					PrtStruct.StrBufferCount++;
					PrtStruct.StrBuffer[PrtStruct.StrBufferCount] = Inline_Interpreter_GetCharRxBuf(CBuf) + '0';
					PrtStruct.StrBufferCount++;
				}
				InterpreterStruct.CompleteFlag = CI_COMPLETE;
			}
			break;
		case 'B':
			if (CheckRxBuf(CBuf)) {
				if(PrtStruct.StrBufferCount == 0) {
					FontLoadFont2(Inline_Interpreter_GetCharRxBuf(CBuf));
				} else {
					PrtStruct.StrBuffer[PrtStruct.StrBufferCount] = FONT_CON_CH;
					PrtStruct.StrBufferCount++;
					PrtStruct.StrBuffer[PrtStruct.StrBufferCount] = FONT_CON_FONT2;
					PrtStruct.StrBufferCount++;
					PrtStruct.StrBuffer[PrtStruct.StrBufferCount] = Inline_Interpreter_GetCharRxBuf(CBuf) + '0';
					PrtStruct.StrBufferCount++;
				}
				InterpreterStruct.CompleteFlag = CI_COMPLETE;
			}
			break;
		case 'G':
			if (CheckRxBuf(CBuf)) {
				sbuf = Inline_Interpreter_GetCharRxBuf(CBuf);
				if(PrtStruct.StrBufferCount == 0) {
					StrStruct.StrType = sbuf;
				}
				InterpreterStruct.CompleteFlag = CI_COMPLETE;
			}
			break;
		case 'O':
			if (CheckRxBuf(CBuf)) {
				if(PrtStruct.StrBufferCount == 0) {
					StrStruct.ChRot = Inline_Interpreter_GetCharRxBuf(CBuf);
				} else {
					PrtStruct.StrBuffer[PrtStruct.StrBufferCount] = FONT_CON_CH;
					PrtStruct.StrBufferCount++;
					PrtStruct.StrBuffer[PrtStruct.StrBufferCount] = FONT_CON_DEG;
					PrtStruct.StrBufferCount++;
					PrtStruct.StrBuffer[PrtStruct.StrBufferCount] = Inline_Interpreter_GetCharRxBuf(CBuf) + '0';
					PrtStruct.StrBufferCount++;
				}
				InterpreterStruct.CompleteFlag = CI_COMPLETE;
			}
			break;
		case 'E':
			if (CheckRxBuf(CBuf)) {
				sbuf = Inline_Interpreter_GetCharRxBuf(CBuf);
				if(PrtStruct.StrBufferCount == 0) {
					StrStruct.StrMirx = sbuf;
				}
				InterpreterStruct.CompleteFlag = CI_COMPLETE;
			}
			break;
		case '/':
			if(PrtStruct.StrBufferCount == 0) {
				StrStruct.Typeface = StrStruct.Typeface | ITALIC;
			} else {
				PrtStruct.StrBuffer[PrtStruct.StrBufferCount] = FONT_CON_CH;
				PrtStruct.StrBufferCount++;						
				PrtStruct.StrBuffer[PrtStruct.StrBufferCount] = FONT_CON_TYPE_ITALIC;
				PrtStruct.StrBufferCount++;
			}
			InterpreterStruct.CompleteFlag = CI_COMPLETE;
			break;
		case '!':
			if(PrtStruct.StrBufferCount == 0) {
				StrStruct.Typeface = StrStruct.Typeface | OUTLINE;
			} else {
				PrtStruct.StrBuffer[PrtStruct.StrBufferCount] = FONT_CON_CH;
				PrtStruct.StrBufferCount++;						
				PrtStruct.StrBuffer[PrtStruct.StrBufferCount] = FONT_CON_TYPE_OUTLINE;
				PrtStruct.StrBufferCount++;
			}
			InterpreterStruct.CompleteFlag = CI_COMPLETE;
			break;
		case '_':
			if(PrtStruct.StrBufferCount == 0) {
				StrStruct.Typeface = StrStruct.Typeface | UNDERLINE;
			} else {
				PrtStruct.StrBuffer[PrtStruct.StrBufferCount] = FONT_CON_CH;
				PrtStruct.StrBufferCount++;						
				PrtStruct.StrBuffer[PrtStruct.StrBufferCount] = FONT_CON_TYPE_UNDERLINE;
				PrtStruct.StrBufferCount++;
			}
			InterpreterStruct.CompleteFlag = CI_COMPLETE;
			break;
		case '-':
			if(PrtStruct.StrBufferCount == 0) {
				StrStruct.Typeface = StrStruct.Typeface | THROUGH_LINE;
			} else {
				PrtStruct.StrBuffer[PrtStruct.StrBufferCount] = FONT_CON_CH;
				PrtStruct.StrBufferCount++;						
				PrtStruct.StrBuffer[PrtStruct.StrBufferCount] = FONT_CON_TYPE_THROUGH_LINE;
				PrtStruct.StrBufferCount++;
			}
			InterpreterStruct.CompleteFlag = CI_COMPLETE;
			break;
		case '+':
			if(PrtStruct.StrBufferCount == 0) {
				StrStruct.Typeface = StrStruct.Typeface | SHADOW;
			} else {
				PrtStruct.StrBuffer[PrtStruct.StrBufferCount] = FONT_CON_CH;
				PrtStruct.StrBufferCount++;						
				PrtStruct.StrBuffer[PrtStruct.StrBufferCount] = FONT_CON_TYPE_SHADOW;
				PrtStruct.StrBufferCount++;
			}
			InterpreterStruct.CompleteFlag = CI_COMPLETE;
			break;
		case '*':
			if(PrtStruct.StrBufferCount == 0) {
				StrStruct.Typeface = StrStruct.Typeface | BOLD;
			} else {
				PrtStruct.StrBuffer[PrtStruct.StrBufferCount] = FONT_CON_CH;
				PrtStruct.StrBufferCount++;						
				PrtStruct.StrBuffer[PrtStruct.StrBufferCount] = FONT_CON_TYPE_BOLD;
				PrtStruct.StrBufferCount++;
			}
			InterpreterStruct.CompleteFlag = CI_COMPLETE;
			break;
		case '~':
			if(PrtStruct.StrBufferCount == 0) {
				StrStruct.Typeface = StrStruct.Typeface | REVERSE;
			} else {
				PrtStruct.StrBuffer[PrtStruct.StrBufferCount] = FONT_CON_CH;
				PrtStruct.StrBufferCount++;						
				PrtStruct.StrBuffer[PrtStruct.StrBufferCount] = FONT_CON_TYPE_REVERSE;
				PrtStruct.StrBufferCount++;
			}
			InterpreterStruct.CompleteFlag = CI_COMPLETE;
			break;
		case '=':
			if(PrtStruct.StrBufferCount == 0) {
				StrStruct.Typeface = StrStruct.Typeface | DTHROUGH_LINE;
			} else {
				PrtStruct.StrBuffer[PrtStruct.StrBufferCount] = FONT_CON_CH;
				PrtStruct.StrBufferCount++;						
				PrtStruct.StrBuffer[PrtStruct.StrBufferCount] = FONT_CON_TYPE_DTHROUGH_LINE;
				PrtStruct.StrBufferCount++;
			}
			InterpreterStruct.CompleteFlag = CI_COMPLETE;
			break;
		case '@':
			if(PrtStruct.StrBufferCount == 0) {
				StrStruct.Typeface = NO_TYPEFACE;
			} else {
				PrtStruct.StrBuffer[PrtStruct.StrBufferCount] = FONT_CON_CH;
				PrtStruct.StrBufferCount++;						
				PrtStruct.StrBuffer[PrtStruct.StrBufferCount] = FONT_CON_TYPE_NO_TYPEFACE;
				PrtStruct.StrBufferCount++;
			}
			InterpreterStruct.CompleteFlag = CI_COMPLETE;
			break;
	}//esc switch
}

void CAS_BarRInterpreter(HUGEDATA COMM_BUF	*CBuf) {
	char sbuf;	
	HUGEDATA char *s_ptr;
	char nb,ns,wb,ws;
//	char string_buf[30];

	sbuf = Inline_Interpreter_GetCharRxBuf(CBuf);
	switch (sbuf) {
		default : 
			InterpreterStruct.CompleteFlag = CI_INVALID;
			InterpreterStruct.PreInterpreter = IDLE;
			break;
		case 'T':
			if (CheckRxBuf(CBuf)) {
				BarSetBarType(Inline_Interpreter_GetCharRxBuf(CBuf));
				InterpreterStruct.CompleteFlag = CI_COMPLETE;
			}
			break;
		case 'H':
			if (CheckRxBufCount(CBuf,6)) {
				BarSetFullHeight((short)Interpreter_GetWordRxBuf(CBuf));
				BarSetBarHeight((short)Interpreter_GetWordRxBuf(CBuf));
				BarSetCharHeight((short)Interpreter_GetWordRxBuf(CBuf));
				InterpreterStruct.CompleteFlag = CI_COMPLETE;
 			}
			break;
		case 'W':
			if (CheckRxBuf(CBuf)) {
				BarSetWidthMag(Inline_Interpreter_GetCharRxBuf(CBuf));
				InterpreterStruct.CompleteFlag = CI_COMPLETE;
			}
			break;
		case 'I':
			if (CheckRxBufCount(CBuf,4)) {
				nb = Inline_Interpreter_GetCharRxBuf(CBuf);
				ns = Inline_Interpreter_GetCharRxBuf(CBuf);
				wb = Inline_Interpreter_GetCharRxBuf(CBuf);
				ws = Inline_Interpreter_GetCharRxBuf(CBuf);
				BarSetNSWS(nb,ns,wb,ws);
				InterpreterStruct.CompleteFlag = CI_COMPLETE;
 			}
			break;
		case 'S':
			if (CheckRxBuf(CBuf)) {
				BarStruct.PrtHri = Inline_Interpreter_GetCharRxBuf(CBuf);
				InterpreterStruct.CompleteFlag = CI_COMPLETE;
			}
			break;
		case 'P':
			if (CheckRxBuf(CBuf)) {
				BarStruct.BarHriPos = Inline_Interpreter_GetCharRxBuf(CBuf);
				InterpreterStruct.CompleteFlag = CI_COMPLETE;
			}
			break;
		case 'C':
			if (CheckRxBuf(CBuf)) {
				BarStruct.PrtCheckHri = Inline_Interpreter_GetCharRxBuf(CBuf);
				InterpreterStruct.CompleteFlag = CI_COMPLETE;
			}
			break;
		case 'O':
			if (CheckRxBuf(CBuf)) {
				BarStruct.PrtStartStopHri = Inline_Interpreter_GetCharRxBuf(CBuf);
				InterpreterStruct.CompleteFlag = CI_COMPLETE;
			}
			break;
		case 'B':
			if (CheckRxBuf(CBuf)) {
				BarStruct.UseOcrb = Inline_Interpreter_GetCharRxBuf(CBuf);
				InterpreterStruct.CompleteFlag = CI_COMPLETE;
			}
			break;
		case 'G':
			if (CheckRxBuf(CBuf)) {
				BarSetGuard(Inline_Interpreter_GetCharRxBuf(CBuf));
				InterpreterStruct.CompleteFlag = CI_COMPLETE;
			}
			break;
		case 'E':
			if (CheckRxBuf(CBuf)) {
				BarStruct.BearerBarWidth = Inline_Interpreter_GetCharRxBuf(CBuf);
				InterpreterStruct.CompleteFlag = CI_COMPLETE;
			}
			break;
		case 'U':
			if (CheckRxBuf(CBuf)) {
				BarStruct.QuietZone = Inline_Interpreter_GetCharRxBuf(CBuf);
				InterpreterStruct.CompleteFlag = CI_COMPLETE;
			}
			break;
		//20040211 HYP
		case 'K':
			if (CheckRxBuf(CBuf)) {
				BarStruct.CheckType = Inline_Interpreter_GetCharRxBuf(CBuf);
				InterpreterStruct.CompleteFlag = CI_COMPLETE;
			}
			break;
		case '[':
			InterpreterStruct.CompleteFlag = CI_COMPLETE;
			s_ptr = PrtStruct.BarBuffer;
			while (CheckRxBuf(CBuf)) {
				sbuf = Inline_Interpreter_GetCharRxBuf(CBuf);
				if(sbuf == '\0') {
					*s_ptr = '\0';
					PrtDrvStruct.PrevTphMemMaxRowNum = PrtDrvStruct.TphMemMaxRowNum;
					PrtDrvStruct.TphMemMaxRowNum = PrtDrvStruct.TphMemMaxRowNum + BarStruct.FullHeight + StrStruct.LineSp;
					//HYP 20041130
					//if (PrtDrvStruct.TphMemMaxRowNum >= MAX_FORM_LEN) {
					if (PrtDrvStruct.TphMemMaxRowNum >= MAX_RECEIPT_LEN) {
						PrtDrvStruct.TphMemMaxRowNum = PrtDrvStruct.PrevTphMemMaxRowNum - StrStruct.LineSp;
						InterpreterStruct.CompleteFlag = CI_NOTCOMPLETE;
						if (PrtDrvStruct.PrtState == PRT_STATE0I) {	 //prohibit duplicating start!!
							PrtDrvStruct.LabelCopyCount = 1;
							PrtDrvStart(PRT_STATE0S);
						}								
					} else {
						BarcodeOut(PrtStruct.BarBuffer);
                    #ifdef USE_SRAM_PRT_RENERING_BUF
                        SramPrtMemory = SramPrtMemory + (BarStruct.FullHeight + StrStruct.LineSp)*TPH_ROW_BYTE;
                    #else // USE_SRAM_PRT_RENERING_BUF
						PrtStruct.PrtMemory = PrtStruct.PrtMemory + (BarStruct.FullHeight + StrStruct.LineSp)*TPH_ROW_BYTE;
                    #endif
						PrtStruct.PrintableHeight = PrtStruct.PrintableHeight - (BarStruct.FullHeight + StrStruct.LineSp);
						PrtStruct.PrintHeight = PrtStruct.PrintableHeight - PrtStruct.TMargin - PrtStruct.BMargin;
//						sprintf(string_buf,"nb=%d,bh=%d,",PrtDrvStruct.TphMemMaxRowNum,PrtStruct.PrintHeight);
//						PutStrTxBuf(&CommBuf,string_buf);
					}					
					break;
				}
				*s_ptr = sbuf;
				s_ptr++;
			}
			break;
	}//esc switch
}

void CAS_BmpRInterpreter(HUGEDATA COMM_BUF	*CBuf) {
	char sbuf;	
//	char string_buf[20];

	sbuf = Inline_Interpreter_GetCharRxBuf(CBuf);
	switch (sbuf) {
		default : 
			InterpreterStruct.CompleteFlag = CI_INVALID;
			break;
		case 'J':
			if (CheckRxBuf(CBuf)) {
				sbuf = Inline_Interpreter_GetCharRxBuf(CBuf);
				StrStruct.Justification = sbuf;
				InterpreterStruct.CompleteFlag = CI_COMPLETE;
			}
			break;
		case '{':
			InterpreterStruct.CompleteFlag = CI_COMPLETE;
			if (CheckRxBuf(CBuf)) {
				BitmapOut((INT16U)Inline_Interpreter_GetCharRxBuf(CBuf));
				PrtDrvStruct.PrevTphMemMaxRowNum = PrtDrvStruct.TphMemMaxRowNum;
				PrtDrvStruct.TphMemMaxRowNum = PrtDrvStruct.TphMemMaxRowNum + PrtStruct.ReceiptBmpHeight + StrStruct.LineSp;
				//HYP 20041130
				//if (PrtDrvStruct.TphMemMaxRowNum >= MAX_FORM_LEN) {
				if (PrtDrvStruct.TphMemMaxRowNum >= MAX_RECEIPT_LEN) {
					PrtDrvStruct.TphMemMaxRowNum = PrtDrvStruct.PrevTphMemMaxRowNum - StrStruct.LineSp;
					InterpreterStruct.CompleteFlag = CI_NOTCOMPLETE;
					if (PrtDrvStruct.PrtState == PRT_STATE0I) {	 //prohibit duplicating start!!
						PrtDrvStruct.LabelCopyCount = 1;
						PrtDrvStart(PRT_STATE0S);
					}
				} else {
				#ifdef USE_SRAM_PRT_RENERING_BUF
                    SramPrtMemory = SramPrtMemory + (PrtStruct.ReceiptBmpHeight + StrStruct.LineSp)*TPH_ROW_BYTE;
                #else // USE_SRAM_PRT_RENERING_BUF
					PrtStruct.PrtMemory = PrtStruct.PrtMemory + (PrtStruct.ReceiptBmpHeight + StrStruct.LineSp)*TPH_ROW_BYTE;
                #endif // USE_SRAM_PRT_RENERING_BUF
					PrtStruct.PrintableHeight = PrtStruct.PrintableHeight - (PrtStruct.ReceiptBmpHeight + StrStruct.LineSp);
					PrtStruct.PrintHeight = PrtStruct.PrintableHeight - PrtStruct.TMargin - PrtStruct.BMargin;
//					sprintf(string_buf,"gb=%d,gh=%d,",PrtDrvStruct.TphMemMaxRowNum,PrtStruct.PrintHeight);
//					PutStrTxBuf(&CommBuf,string_buf);
				}
			}
			break;
	}
}

void CAS_EscRInterpreter(HUGEDATA COMM_BUF	*CBuf) {
	char sbuf;	
//	char string_buf[20];

	sbuf = Inline_Interpreter_GetCharRxBuf(CBuf);
	switch (sbuf) {
		default : 
			InterpreterStruct.CompleteFlag = CI_INVALID;
			break;
 		case 'Q':
			if (CheckRxBuf(CBuf)) {
				sbuf = Inline_Interpreter_GetCharRxBuf(CBuf);
				if(PrtStruct.StrBufferCount == 0) {
					PrtStruct.Opaque = sbuf;
				}
				InterpreterStruct.CompleteFlag = CI_COMPLETE;
			}
			break;
		case '+':
			if(PrtStruct.StrBufferCount == 0) {
				StrStruct.Magx++;
				StrStruct.Magy++;
			} else {
				PrtStruct.StrBuffer[PrtStruct.StrBufferCount] = FONT_CON_CH;
				PrtStruct.StrBufferCount++;						
				PrtStruct.StrBuffer[PrtStruct.StrBufferCount] = FONT_CON_INC_MAG;
				PrtStruct.StrBufferCount++;
			}
			InterpreterStruct.CompleteFlag = CI_COMPLETE;
			break;
		case '-':
			if(PrtStruct.StrBufferCount == 0) {
				StrStruct.Magx--;
				StrStruct.Magy--;
			} else {
				PrtStruct.StrBuffer[PrtStruct.StrBufferCount] = FONT_CON_CH;
				PrtStruct.StrBufferCount++;						
				PrtStruct.StrBuffer[PrtStruct.StrBufferCount] = FONT_CON_DEC_MAG;
				PrtStruct.StrBufferCount++;
			}
			InterpreterStruct.CompleteFlag = CI_COMPLETE;
			break;
		case 'M':
			if (CheckRxBuf(CBuf)) {
				if(PrtStruct.StrBufferCount == 0) {
					StrStruct.Magx = Inline_Interpreter_GetCharRxBuf(CBuf);
				} else {
					PrtStruct.StrBuffer[PrtStruct.StrBufferCount] = FONT_CON_CH;
					PrtStruct.StrBufferCount++;
					PrtStruct.StrBuffer[PrtStruct.StrBufferCount] = FONT_CON_MAG_X;
					PrtStruct.StrBufferCount++;
					PrtStruct.StrBuffer[PrtStruct.StrBufferCount] = Inline_Interpreter_GetCharRxBuf(CBuf) + '0';
					PrtStruct.StrBufferCount++;
				} 
				InterpreterStruct.CompleteFlag = CI_COMPLETE;
			}
			break;
		case 'N':
			if (CheckRxBuf(CBuf)) {
				if(PrtStruct.StrBufferCount == 0) {
					StrStruct.Magy = Inline_Interpreter_GetCharRxBuf(CBuf);
				} else {
					PrtStruct.StrBuffer[PrtStruct.StrBufferCount] = FONT_CON_CH;
					PrtStruct.StrBufferCount++;
					PrtStruct.StrBuffer[PrtStruct.StrBufferCount] = FONT_CON_MAG_Y;
					PrtStruct.StrBufferCount++;
					PrtStruct.StrBuffer[PrtStruct.StrBufferCount] = Inline_Interpreter_GetCharRxBuf(CBuf) + '0';
					PrtStruct.StrBufferCount++;
				}
				InterpreterStruct.CompleteFlag = CI_COMPLETE;
			}
			break;
		case 'S':
			if (CheckRxBuf(CBuf)) {
				CAS_StrRInterpreter(CBuf);	
			}  //if
			break;
		case 'B':
			if (CheckRxBuf(CBuf)) {
				if(PrtStruct.StrBufferCount != 0) {
					PrtStruct.StrBufferCount = 0;
				} 
				CAS_BarRInterpreter(CBuf);	
			}  //if
			break;
		case 'G':
			if (CheckRxBuf(CBuf)) {
				if(PrtStruct.StrBufferCount != 0) {
					PrtStruct.StrBufferCount = 0;
				}
				CAS_BmpRInterpreter(CBuf);	
			}  //if
			break;			
		case '!':
			if (PrtDrvStruct.PrtState == PRT_STATE0I) {	 //prohibit duplicating start!!
				PrtDrvStruct.LabelCopyCount = 1; //Receipt mode (if long ticket..?)
				PrtDrvStart(PRT_STATE0S);
				InterpreterStruct.CompleteFlag = CI_COMPLETE;
			} else {
				InterpreterStruct.CompleteFlag = CI_INVALID;
			}
			break;
		//HYP 20040223
		case 'T':
			if(PrtStruct.StrBufferCount == 0) {
				PrtTestPattern();
//				PutCStrTxBuf(CBuf, "Test Pattern");
			}
			InterpreterStruct.CompleteFlag = CI_COMPLETE;
			break;
		//HYP 20040330
		case 'F':
			if (PrtDrvStruct.PrtState == PRT_STATE0I) {	 //prohibit duplicating start!!
				PrtDrvStart(PRT_STATE_RFFS);
				InterpreterStruct.CompleteFlag = CI_COMPLETE;
			} else {
				InterpreterStruct.CompleteFlag = CI_INVALID;
			}
			break;
	}//esc switch
}
#ifdef USE_CONTINUOUS_LABEL_WITH_INGREDIENT
INT16U find_ingredient_y_interpreter(HUGEDATA COMM_BUF *label_str)
{
	char sbuf;	
	BOOLEAN loop;
	////////////////////////////////////

	if (CheckRxBuf(label_str) && PrtDrvStruct.PrtState == PRT_STATE0I) {
		InterpreterStruct.CompleteFlag = CI_NOTCOMPLETE; //not complete
		InterpreterStruct.UsedChars = 0;
		sbuf = Inline_Interpreter_GetCharRxBuf(label_str);
		switch(sbuf) {
			default:
				if(PrtStruct.Mode == RECEIPT && (((INT8U)sbuf >= 0x20) || (sbuf == FONT_CON_CH))) {
					PrtStruct.ReceiptModeState = RM_RENDERING;
					PrtStruct.StrBuffer[PrtStruct.StrBufferCount] = sbuf;
					PrtStruct.StrBufferCount++;
					loop = ON;
					while(CheckRxBuf(label_str) && loop == ON) {
						sbuf = Inline_Interpreter_GetCharRxBuf(label_str);
						if(sbuf == HT || (((INT8U)sbuf >= 0x20) || (sbuf == FONT_CON_CH))) {
							PrtStruct.StrBuffer[PrtStruct.StrBufferCount] = sbuf;
							PrtStruct.StrBufferCount++;
						} else {
							loop = OFF;
							RewindRxBuf(label_str,1);
						}
					}
					InterpreterStruct.CompleteFlag = CI_COMPLETE;
				} else {
					InterpreterStruct.CompleteFlag = CI_INVALID;  //invalid command
					InterpreterStruct.PreInterpreter = IDLE;
				}
				break;
			case FF:
				if(PrtStruct.Mode == LABEL || PrtStruct.ReceiptModeState == RM_INIT) {
					PrtDrvStart(PRT_STATE_FFS);
				}
				InterpreterStruct.CompleteFlag = CI_COMPLETE;
				break;
			case HT:
				break;
			case LF:
//				PutCStrTxBuf(CBuf, "linefeed");
				InterpreterStruct.CompleteFlag = CI_COMPLETE;
				break;
			case BS:
//				PutCStrTxBuf(CBuf, "reversefeed");
				InterpreterStruct.CompleteFlag = CI_COMPLETE;
				break;
			case _ESC:
				if (CheckRxBuf(label_str)) {
					InterpreterStruct.PreInterpreter = ESC_INTERPRETER; 
					CAS_EscLInterpreter(label_str);
				}
				break;
			case GS:
				if (CheckRxBuf(label_str)) {
					InterpreterStruct.PreInterpreter = GS_INTERPRETER;
					CAS_GsInterpreter(label_str);
				} 
				break;
			case ENQ:
				if (CheckRxBuf(label_str)) {
					InterpreterStruct.PreInterpreter = ENQ_INTERPRETER;
					CAS_EnqInterpreter(label_str);	
				}  //if
				break;
			case DLE:
				if (CheckRxBuf(label_str)) {
					InterpreterStruct.PreInterpreter = DLE_INTERPRETER;
					CAS_DleInterpreter(label_str);	
				}  
				break;
			case ',':
				if (CheckRxBuf(label_str)) {
					switch(InterpreterStruct.PreInterpreter) {
						default:
							InterpreterStruct.CompleteFlag = CI_INVALID;  //invalid command
							InterpreterStruct.PreInterpreter = IDLE;
							break;
						case ESC_INTERPRETER:
							CAS_EscLInterpreter(label_str);
							break;
						case GS_INTERPRETER:
							CAS_GsInterpreter(label_str);
							break;
						case ENQ_INTERPRETER:
							CAS_EnqInterpreter(label_str);
							break;
						case DLE_INTERPRETER:
							CAS_DleInterpreter(label_str);
							break;
						case STR_INTERPRETER:
							CAS_StrLInterpreter(label_str);
							break;
						case BAR_INTERPRETER:
							CAS_BarLInterpreter(label_str);
							break;
						case LINE_INTERPRETER:
							CAS_LineInterpreter(label_str);
							break;
						case BMP_INTERPRETER:
							CAS_BmpLInterpreter(label_str);
							break;
					}
				}
				break;
		} //switch
		if ( InterpreterStruct.CompleteFlag == CI_NOTCOMPLETE ) RewindRxBuf(label_str,InterpreterStruct.UsedChars);
	}
	return TRUE;
}
#endif

