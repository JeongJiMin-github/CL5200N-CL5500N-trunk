/*****************************************************************************
|*			
|*  Copyright		:	(c) 2002 CAS
|*  Filename		:	serial.h
|*  Version			:	0.1
|*  Programmer(s)	:	Hong Yi-Phyo (HYP)
|*  Created			:	2002/11/11
|*  Description		:	CLP Printer (UART)serial routine header    
|*				
****************************************************************************/

#ifndef _SERIAL_H
#define _SERIAL_H

#define BAUD_RATE_9600     	0
#define BAUD_RATE_19200    	1
#define BAUD_RATE_38400    	2
#define BAUD_RATE_57600    	3
#define BAUD_RATE_115200    4 

extern void Uart0RInt(void);
extern void Uart0TInt(void);
extern void Uart0Init(INT8U baud);
extern void Uart1Init(INT8U baud);

extern void Uart1RInt(void);
extern void Uart1TInt(void);
extern void Uart0Baudrate(void);

extern void Uart0SendData(void);
extern void Uart1SendData(void);
#endif /* _SERIAL_H */
 