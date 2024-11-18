/*****************************************************************************
|*			
|*  Copyright		:	(c) 2004 CAS
|*  Filename		:	cashdrawer.c
|*  Version			:	0.1
|*  Programmer(s)	:	Hong Yi Phyo (HYP)
|*  Created			:	2004/02/09
|*  Description		:	CLP cash drawer driver    
|*				
****************************************************************************/

#include <stdio.h>
#include "globals.h"
#include "prtdrv.h"
#include "main.h"
#include "cashdrawer.h"
#include "initial.h"
#include "mode_sale.h"
#include "mode_main.h"
#include "commbuf.h"


INT16U TimeOpenCashDrawer=0;

HUGEDATA	INT8U	EthConPortValue;
//__no_init  volatile HUGEDATA INT8U EthConPort @ (CPLD_ADDR+ 0x0c);		//WR//volatile HUGEDATA INT8U EthConPort;
volatile HUGEDATA INT8U EthConPort;


void Cash_open(void)
{
	INT16U delay_100ms;
	CASH_ON;
	delay_100ms = get_global_wparam(GLOBAL_TIME_CASHDRAWEROPEN);		// Flash

	TimeOpenCashDrawer = SysTimer100ms + delay_100ms;
	Operation.openCash =ON;
}

void Cash_close(void)
{
	CASH_OFF;
}

INT8U Cash_check(void)
{
	if(PLIB_PORTS_PinGet(PORTS_ID_0, PORT_CHANNEL_K, PORTS_BIT_POS_6)) {
		return CASH_SEN_CLOSE;
	} else {
		return CASH_SEN_OPEN;
	}
}
