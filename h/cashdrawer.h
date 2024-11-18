/*****************************************************************************
|*			
|*  Copyright		:	(c) 2004 CAS
|*  Filename		:	cashdrawer.h
|*  Version			:	0.1
|*  Programmer(s)	:	Hong Yi-Phyo (HYP)
|*  Created			:	2004/02/09
|*  Description		:	    
|*				
****************************************************************************/

#ifndef _CASHDRAWER_H
#define _CASHDRAWER_H

#define MAX_CASHDRAWER_10MS_DELAY  50

extern void  Cash_open(void);
extern INT8U Cash_check(void);
extern void Cash_close(void);

#endif /* _CASHDRAWER_H */

