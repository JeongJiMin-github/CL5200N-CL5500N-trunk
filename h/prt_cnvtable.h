/*****************************************************************************
|*			
|*  Copyright		:	(c) 2002 CAS
|*  Filename		:	prt_cnvtable.h
|*  Version			:	0.1
|*  Programmer(s)	:	Hong Yi-Phyo (HYP)
|*  Created			:	2002/11/11
|*  Description		:	    
|*				
****************************************************************************/

#ifndef _CNVTABLE_H
#define _CNVTABLE_H
extern ROMDATA INT16U HanWan2JohabConvTable_Chosung[30];
extern ROMDATA INT16U HanWan2JohabConvTable[2350+50];
extern ROMDATA INT8U CnvTableMirror[256];
extern ROMDATA INT8U CnvTableMagX1[256];
extern ROMDATA INT8U CnvTableMagX2[256][2];
extern ROMDATA INT8U CnvTableMagX3[256][3];
extern ROMDATA INT8U CnvTableMagX4[256][4];
extern ROMDATA INT8U CnvTableMagX5[256][5];


#endif /* _CNVTABLE_H */