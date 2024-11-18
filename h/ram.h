/*****************************************************************************
|*			
|*  Copyright		:	(c) 2002 CAS
|*  Filename		:	ram.h
|*  Version			:	0.1
|*  Programmer(s)	:	Hong Yi-Phyo (HYP)
|*  Created			:	2002/12/10
|*  Description		:	CLP Printer RAM control routine header    
|*				
****************************************************************************/

#ifndef _RAM_H
#define _RAM_H

#define NVRAM_BASE		0x00018000		   /* nvram base address of RAM memory */
#define SRAM_BASE		0x00000000
#define NVRAM_VOLUME	0x100000

extern INT8U RamTest(HUGEDATA INT16U *address, INT32U vol);
extern INT8U InitRamTest(void);
#endif /* _RAM_H */

