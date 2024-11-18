#include "globals.h"
#include <stdio.h>
#include "commbuf.h"

#pragma pack(2)
typedef struct {
	char cmd[10];
} PRINTER_GLOBAL;
#pragma pack()
PRINTER_GLOBAL printer_global;

void label_form_start(INT8U formid)
{
	sprintf(printer_global.cmd,"%cS%c",0x1d,formid);
	PutDataRxBuf(&CommBufPrt,printer_global.cmd,3);
}

void label_form_end(void)
{
	sprintf(printer_global.cmd,"%cE",0x1d);
	PutDataRxBuf(&CommBufPrt,printer_global.cmd,2);
}

void label_form_print(INT16S pageno)
{
	sprintf(printer_global.cmd,"%c!%c",0x1b,pageno);
	PutDataRxBuf(&CommBufPrt,printer_global.cmd,3);
}

void __label_make_string()
{

}
