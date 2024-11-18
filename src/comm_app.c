#include <stdio.h>
#include <string.h>
#include "globals.h"
#include "serial.h"
#include "flash.h"

#include "commbuf.h"
#include "comm_app.h"

void MsgOut(HUGEDATA char *str)
{
	PutStrTxBuf(&CommBuf,str);
	if(CheckTxBuf(&CommBuf)) 
	{
	        Uart0SendData();           
	}
}

#ifdef USE_DEBUG_MESSAGE
void MsgOut_debug(const char *format, ...)
{
	char tempBuf[100];
	va_list args;

	va_start( args, format );

	//sprintf(MsgBuf, format, args);
	vsnprintf(tempBuf, 100, format, args);

	va_end( args );
	
	MsgOut(tempBuf);
}
#endif
//void MsgOutData(char *str,INT16S leng)
//{
//	PutDataTxBuf(&CommBuf,str,leng);
//	if (CommBuf.Txing != 1) {
//		if(CheckTxBuf(&CommBuf)) {
//			TI_0 = 1;
//		}
//	}
//}
