/*****************************************************************************
|*			
|*  Copyright		:	(c) 2003 CAS
|*  Filename		:	prt_interpreter.h
|*  Version			:	0.1
|*  Programmer(s)	:	Hong Yi-Phyo (HYP)
|*  Created			:	2003/04/22
|*  Description		:	CLP Printer interpreter header    
|*				
****************************************************************************/

#ifndef _INTERPRETER_H
#define _INTERPRETER_H

#define ESC_INTERPRETER		1
#define GS_INTERPRETER		2
#define ENQ_INTERPRETER		3
#define DLE_INTERPRETER		4
#define STR_INTERPRETER		5
#define BAR_INTERPRETER		6
#define LINE_INTERPRETER	7
#define BMP_INTERPRETER		8
#pragma pack(4)
typedef struct {
	short	UsedChars;
	char 	CompleteFlag;
	INT8U	PreInterpreter;
} INTERPRETER_STRUCT;
#pragma pack()
//extern HUGEDATA	INTERPRETER_STRUCT	InterpreterStruct;
extern NEARDATA INTERPRETER_STRUCT	InterpreterStruct;//HYP 20060412 _huge --> _near for small code size

extern void CASInterpreter(HUGEDATA COMM_BUF *CBuf);

#endif  //_INTERPRETER_H