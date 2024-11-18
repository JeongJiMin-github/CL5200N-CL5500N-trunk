/*****************************************************************************
|*			
|*  Copyright		:	(c) 2002 CAS
|*  Filename		:	serial.c
|*  Version			:	0.1
|*  Programmer(s)	:	Hong Yi-Phyo (HYP)
|*  Created			:	2002/11/11
|*  Description		:	CLP Printer (UART)serial routine    
|*				
****************************************************************************/

#include <STDIO.H>
#include "globals.h"
#include "commbuf.h"
#include "flash.h"
#include "serial.h"
#include "initial.h"
#include "main.h"

#define OSC            30000000L               /* Xtal frequency           */
#define DIVIDER        (OSC/(64L*BAUD_RATE))

void Uart0Init(INT8U baud)	//PC interface
{
	Uart0InitialArm(baud);	
}


void Uart1Init(INT8U baud) //AD interface
{
	Uart1InitialArm(baud);	
}

void Uart0RInt(void)
{
}

void Uart0TInt(void)
{
}

void Uart1RInt(void)
{
}

void Uart1TInt(void)
{
}		 

void Uart0Baudrate(void)
{
	INT8U cc;
	cc=get_global_bparam(GLOBAL_AREA_RS232C_BAUD);
	if (cc>BAUD_RATE_115200) {
		cc=BAUD_RATE_115200;
		set_global_bparam(GLOBAL_AREA_RS232C_BAUD,cc);
	}
	Uart0Init(cc);
}

#include "peripheral/usart/plib_usart.h"

void Uart0SendData(void)
{
	CommBuf.Txing = 1;
	PLIB_INT_SourceEnable(INT_ID_0, INT_SOURCE_USART_1_TRANSMIT);
}

void Uart1SendData(void)
{
	CommBufAD.Txing = 1;
	PLIB_INT_SourceEnable(INT_ID_0, INT_SOURCE_USART_4_TRANSMIT);
}

void Uart2SendData(void)
{

}