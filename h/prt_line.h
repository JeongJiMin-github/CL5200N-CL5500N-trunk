/*****************************************************************************
|*			
|*  Copyright		:	(c) 2002 CAS
|*  Filename		:	prt_line.h
|*  Version			:	0.1
|*  Programmer(s)	:	Hong Yi-Phyo (HYP)
|*  Created			:	2002/12/10
|*  Description		:	    
|*				
****************************************************************************/

#ifndef _LINE_H
#define _LINE_H

#define WHITE	0
#define BLACK	1

void PrtHLine(short x1,short x2,short y,short w,char bw);
void PrtVLine(short y1,short y2,short x,char w,char bw);
void PrtRect(short x1,short y1,short x2,short y2,char w,char bw);
void PrtRectFill(short x1,short y1,short x2,short y2,char bw);
void PrtEllipse(short x1, short y1, short rad_x, short rad_y, char w);

#endif /* _LINE_H */