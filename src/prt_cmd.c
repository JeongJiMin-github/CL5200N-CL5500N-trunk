/*****************************************************************************
|*  Copyright		:	(c) 2003 CAS
|*  Filename		:	prt_cmd.c
|*  Version		:	0.1
|*  Modified		:	2004/08/20
****************************************************************************/
#include <stdio.h>
#include <string.h>
#include "globals.h"
#include "initial.h"
#include "commbuf.h"
#include "prt_common.h"
#include "mode_sale.h"
#include "common.h"
#include "prt_interpreter.h"
#include "barcode_file.h"
///#include "bar_common.h"
///#include "prt_line.h"
///#include "prt_font.h"
///#include "prt_cmdtest.h"
///#include "main.h"
///#include "mode_main.h"
///#include "mode_commun.h"
///#include "initial.h"
///#include "flash.h"
///#include "flash_app.h"
///#include "caption_app.h"
///#include "common_sunday.h"
///#include "plu_manager.h"
///#include "rtc.h"
///#include "prt_font.h"
///#include "plu_auxcall.h"
///
///#include "plu_manager.h"
///#include "tax.h"
///#include "rtc.h"
///#include "rtc_app.h"
///#include "common_sunday.h"
///#include "origin.h"
///#include "prt_advertise.h"
///#include "shopname.h"
///#include "clerk_app.h"
///#include "customer_app.h"
///#include "ingredient.h"
///#include "tare_file.h"
///#include "pcs_unit.h"
///#include "department.h"
///#include "pcs_unit.h"
///#include "traceability.h"
///#include "workplace.h"
///#include "label_caption.h"
///#include "adm_app.h"
///#include "nutrition_fact.h"
///#include "menu.h"
///#include "plu_device.h"

#include "prtdrv.h"
#include "prt_cmd.h"
#include "prt_app.h"
#include "mode_main.h"


#include "comm_app.h"
#include "main.h"
#include "dsp_common.h"
#include "flash_app.h"
#include "plu_device.h"


INT16U LabelCurY;
INT8U PrtFontType;
//!INT8U Rcpt_WidthMode;
INT8U prev_font_type = 0xff, prev_inchsp=0xff, prev_just=0xff;

void LabCmd_SetStrField(INT16U x, INT16U y, INT16U h, INT16U w) 
{
	INT16U	i;
	char string_buf[50];
	CWD cwd;

	i = 0;
	string_buf[i++] = _ESC;
    string_buf[i++] = 'S';
	string_buf[i++] = 'X';
	cwd.word = x;
	string_buf[i++] = cwd.byte.left;
	string_buf[i++] = cwd.byte.right;
	string_buf[i++] = ',';
	string_buf[i++] = 'Y';
	cwd.word = y;
	string_buf[i++] = cwd.byte.left;
	string_buf[i++] = cwd.byte.right;
	string_buf[i++] = ',';
	string_buf[i++] = 'H';
	cwd.word = h;
	string_buf[i++] = cwd.byte.left;
	string_buf[i++] = cwd.byte.right;
	string_buf[i++] = ',';
	string_buf[i++] = 'W';
	cwd.word = w;
	string_buf[i++] = cwd.byte.left;
	string_buf[i++] = cwd.byte.right;
	PutDataRxBuf(&CommBufPrt, string_buf, i);
}

void LabCmd_MakeBarField(INT16U x, INT16U y, INT16U fh, INT16U bh, INT16U ch, INT8U w) 
{
	INT16U	i;
	char string_buf[50];
	CWD cwd;

	i = 0;
	string_buf[i++] = _ESC;
	string_buf[i++] = 'B';
	string_buf[i++] = 'X';
	cwd.word = x;
	string_buf[i++] = cwd.byte.left;
	string_buf[i++] = cwd.byte.right;
	string_buf[i++] = ',';
	string_buf[i++] = 'Y';
	cwd.word = y;
	string_buf[i++] = cwd.byte.left;
	string_buf[i++] = cwd.byte.right;
	string_buf[i++] = ',';
	string_buf[i++] = 'H';
	cwd.word = fh;
	string_buf[i++] = cwd.byte.left;
	string_buf[i++] = cwd.byte.right;
	cwd.word = bh;
	string_buf[i++] = cwd.byte.left;
	string_buf[i++] = cwd.byte.right;
	cwd.word = ch;
	string_buf[i++] = cwd.byte.left;
	string_buf[i++] = cwd.byte.right;
	string_buf[i++] = ',';
	string_buf[i++] = 'W';
	string_buf[i++] = w;
	PutDataRxBuf(&CommBufPrt, string_buf, i);
}

void LabCmd_CharSize(INT8U m, INT8U n)
// m=width,n=height
{
	char string_buf[10];
	INT8U i;
	i = 0;
	//sprintf(string_buf, "%cM%c%cN%c", ESC, m,ESC, n);
	string_buf[i++] = _ESC;
	string_buf[i++] = 'S';
	string_buf[i++] = 'M';
	string_buf[i++] = m;
	string_buf[i++] = ',';
	string_buf[i++] = 'N';
	string_buf[i++] = n;

	PutDataRxBuf(&CommBufPrt,string_buf,i);
}

void LabCmd_PrintStart(INT8U num)
{
	char string_buf[10];

    string_buf[0] = _ESC;
	string_buf[1] = '!';
	string_buf[2] = num;
	PutDataRxBuf(&CommBufPrt,string_buf,3);
}

void LabCmd_ReverseFeed(void)
{
	char string_buf[5];

	string_buf[0] = BS;
	PutDataRxBuf(&CommBufPrt,string_buf,1);
}

void LabCmd_SetTextField(INT8U field_num,char *str)
{
	char string_buf[MAX_INGREDIENT_SIZE + 20];
	INT16U	i,j;

	i=0;
	string_buf[i++] = _ESC;
	string_buf[i++] = 'S';
	string_buf[i++] = '{';
	string_buf[i++] = field_num;
	string_buf[i++] = '\0';
	string_buf[i++] = ',';
	string_buf[i++] = 'G';

	string_buf[i++] = PrtFontType;//1=double, 0=one byte font

	string_buf[i++] = ',';
	string_buf[i++] = '[';
	j=0;
	while(str[j] != '\0') {
		string_buf[i++] = str[j++];
	}
	string_buf[i++] = '\0';

	PutDataRxBuf(&CommBufPrt,string_buf,i);
}

void LabCmd_SetTextField_Direct(INT8U field_num, char *str, INT16U m_size)
{
	char string_buf[500];
	INT16U	i, len;

	i=0;
	string_buf[i++] = _ESC;
	string_buf[i++] = 'S';
	string_buf[i++] = '{';
	string_buf[i++] = field_num;
	string_buf[i++] = '\0';
	string_buf[i++] = ',';
	string_buf[i++] = 'G';

	string_buf[i++] = PrtFontType;//1=double, 0=one byte font

	string_buf[i++] = ',';
	string_buf[i++] = '[';
	PutDataRxBuf(&CommBufPrt,string_buf,i);
//	len = strlen(str); //SG060309
	for(len=0; len<m_size; len++){
		if(str[len]=='\0') break;
	}
	if(len > m_size) len = m_size;
	PutDataRxBuf(&CommBufPrt,(HUGEDATA char*)str,len);
	string_buf[0] = '\0';
	PutDataRxBuf(&CommBufPrt,string_buf,1);
}

#ifdef USE_DIRECT_INGREDIENT

void LabCmd_SetTextField_DirectIngredient(INT8U field_num, INT8U *ingred_text, int len)
{
	INT16U	i;
	INT8U string_buf[15];

	i=0;
	string_buf[i++] = _ESC;
	string_buf[i++] = 'S';
	string_buf[i++] = '{';
	string_buf[i++] = field_num;
	string_buf[i++] = '\0';
	string_buf[i++] = ',';
	string_buf[i++] = 'G';

	string_buf[i++] = PrtFontType;//1=double, 0=one byte font

	string_buf[i++] = ',';
	string_buf[i++] = '[';
	PutDataRxBuf(&CommBufPrt,string_buf,i);

	PutDataRxBuf(&CommBufPrt,ingred_text,len);

	string_buf[0] = '\0';
	PutDataRxBuf(&CommBufPrt,string_buf,1);
}
#endif

//void LabCmd_SetBarField(INT8U field_num,HUGEDATA char *Str) // 

void LabCmd_DrawBarInField(INT8U field_num,INT8U bar_type,HUGEDATA char *str) // 
{
#if defined(USE_QR_ADDTOTAL) || defined(USE_CARREFOUR_TRACE_QR_BARCODE) || defined(USE_PRINT_URL_VIA_QR_BARCODE)
	char string_buf[1000];
#else
	char string_buf[100];
#endif
	INT16U	i,j;

#ifdef USE_QR_BARCODE
	memset (string_buf, 0, sizeof(string_buf)); //20140917
#endif
	i=0;
	string_buf[i++] = _ESC;
	string_buf[i++] = 'B';
	string_buf[i++] = '{';
	string_buf[i++] = field_num;
	string_buf[i++] = 0;
	string_buf[i++] = ',';
	string_buf[i++] = 'T';
	string_buf[i++] = bar_type;
	switch (bar_type) {
		case CODE128C:	// Deleted by CJK 20041213//Recovered by HYP 20050528
		case CODE11  :
		case I2OF5   :
		case S2OF5   :
		case M2OF5   :
		case MSI     :
			string_buf[i++] = ',';
			string_buf[i++] = 'B';
			string_buf[i++] = 1;
			break;
#ifdef USE_BARCODE_CODE128X
		case CODE128X:
			break;
#endif
#ifdef USE_QR_BARCODE // 20140916
		case QRCODE  :
			break;
#endif
	}
	string_buf[i++] = ',';
	string_buf[i++] = '[';
	j=0;
	while(str[j] != '\0') {
		string_buf[i++] = str[j++];
	}

#ifdef USE_QR_ADDTOTAL
	if ((bar_type == QRCODE) && (TotalQRFlag == 1)) {
		if (QrCfg_DelLastSuffix) // Not Use Last Suffix
		{
			string_buf[i-2] = '\0';
			string_buf[i-1] = '\0';
		}
	}
#endif
	string_buf[i++] = '\0';
	PutDataRxBuf(&CommBufPrt,string_buf,i);
}

void LabCmd_SetBarField(INT8U field_num) 
{
	char string_buf[10];

    string_buf[0] = _ESC;
	string_buf[1] = 'B';
	string_buf[2] = 'F';
	string_buf[3] = field_num;
	PutDataRxBuf(&CommBufPrt,string_buf,4);
}

void LabCmd_StrStart(void)
{
	char string_buf[10];
	INT16U	i;

	i = 0;
	string_buf[i++] = _ESC;
	string_buf[i++] = 'S';
	string_buf[i++] = '[';

	PutDataRxBuf(&CommBufPrt,string_buf,i);
}

void LabCmd_StrEnd(void)
{
	char string_buf[10];

	string_buf[0] = 0;
	PutDataRxBuf(&CommBufPrt,string_buf,1);
}

void LabCmd_ClearLabelForm(void)
{
	char string_buf[10];
	INT16U	i;

	i = 0;
	string_buf[i++] = GS;
	string_buf[i++] = 'C';

	PutDataRxBuf(&CommBufPrt,string_buf,i);
}

void LabCmd_ClearTphImage(void)
{
	char string_buf[10];
	INT16U	i;

	i = 0;
	string_buf[i++] = GS;
	string_buf[i++] = 'Z';

	PutDataRxBuf(&CommBufPrt,string_buf,i);
}

void LabCmd_InvertPrt(INT8U mode)
{
	char string_buf[10];
	INT16U	i;

	i = 0;
	string_buf[i++] = GS;
	string_buf[i++] = 'I';
	string_buf[i++] = mode;

	PutDataRxBuf(&CommBufPrt,string_buf,i);
}

void LabCmd_FormFeed(void)		// L, R
{
	char string_buf[10];

	string_buf[0] = FF;
	PutDataRxBuf(&CommBufPrt,string_buf,1);
}

void LabCmd_RewindOnOff(INT8U on)
{
	char string_buf[10];

	sprintf(string_buf, "%cM", GS);
	PutDataRxBuf(&CommBufPrt,string_buf,2);
	string_buf[0] = on;
	PutDataRxBuf(&CommBufPrt,string_buf,1);
}

void LabCmd_AlignmentFeed(void)
{
	INT8U backup_rewind;

	backup_rewind = PrtDrvStruct.RewindEnable;

	LabCmd_RewindOnOff(OFF);
	LabCmd_FormFeed();
	LabCmd_FormFeed();
	LabCmd_RewindOnOff(backup_rewind);

	PrintListWaitEnd();
}
/*//HYP 20060407 Erase non-use function
void LabCmd_RotateStr(INT8U rot)
{
	char string_buf[10];
	INT16U	i;

	i = 0;
	string_buf[i++] = ESC;
	string_buf[i++] = 'S';
	string_buf[i++] = 'R';
	string_buf[i++] = rot;

	PutDataRxBuf(&CommBufPrt,string_buf,i);
}

void LabCmd_RotateBar(INT8U rot)
{
	char string_buf[10];
	INT16U	i;

	i = 0;
	string_buf[i++] = ESC;
	string_buf[i++] = 'B';
	string_buf[i++] = 'R';
	string_buf[i++] = rot;

	PutDataRxBuf(&CommBufPrt,string_buf,i);
}

void LabCmd_PrtStrLine(INT8U x, INT8U mw, INT8U mh, char *str, INT8U linefeed)
{
	INT16U calc_y;
	INT8U fontHeight;
	INT16U margin;

	margin = 32;
	fontHeight = FontStruct.FHbits1 * mh;
	calc_y = LabelCurY + fontHeight;

////sprintf(MsgBuf, "FH:%d, MY:%d, TH:%d, LH:%d CY:%d\r\n", 
////	FontStruct.FHbits1, mh, fontHeight, PrtStruct.LabelHeight, calc_y);
////MsgOut(MsgBuf);
	if(calc_y > PrtStruct.LabelHeight-margin) {
		LabCmd_PrintStart(1);
		PrintListWaitEnd();
		LabelCurY = 0;
		calc_y = LabelCurY + fontHeight;
	}
	LabCmd_CharSize(mw, mh);
	LabCmd_SetStrField(margin+x, margin+LabelCurY, fontHeight, PrtStruct.LabelWidth-margin*2-x);

	LabCmd_StrStart();
	PutDataRxBuf(&CommBufPrt,(HUGEDATA char *)str,strlen(str));
	LabCmd_StrEnd();

	if(linefeed) {
		LabelCurY = calc_y + StrStruct.LineSp;
	}
}

void RcptCmd_PrtStrLine(INT8U x, INT8U mw, INT8U mh, char *str, INT8U linefeed)
{
	INT16U calc_x, calc_xmargin;
	INT8U len, i;
	INT16U margin;
	char string_buf[50];

	margin = 0;

	calc_x = x / (14*mw);
	calc_xmargin = margin / (14*mw);

////	LabCmd_CharSize(mw, mh);
////	LabCmd_SetStrField(margin+x, margin+LabelCurY, fontHeight, PrtStruct.LabelWidth-margin*2-x);
////
////	LabCmd_StrStart();
////	PutDataRxBuf(&CommBufPrt,(HUGEDATA char *)str,strlen(str));
////	LabCmd_StrEnd();

	RcptCmd_CharSize(mw, mh);

	for(i = 0; i < calc_x+calc_xmargin; i++) {
		string_buf[i] = 0x20;
	}
	sprintf(string_buf+i, "%s", str);
	len = strlen(string_buf);
//sprintf(MsgBuf, "len:%d, calc_x:%d, calc_xmar:%d, i:%d\r\n", len, calc_x, calc_xmargin, i);
//MsgOut(MsgBuf);
	if(linefeed) {
		string_buf[len++] = LF;
		string_buf[len++] = 0;
		PutDataRxBuf(&CommBufPrt, string_buf, len);
	} else {
		string_buf[len++] = CR;
		string_buf[len++] = 0;
		PutDataRxBuf(&CommBufPrt, string_buf, len);
	}
		
}

void LabCmd_SkipLine(INT8U line)
{
	INT16U calc_y;
	INT8U fontHeight;
	INT16U margin;

	margin = 32;
	fontHeight = FontStruct.FHbits1 * line;
	calc_y = LabelCurY + fontHeight;

////sprintf(MsgBuf, "FH:%d, MY:%d, TH:%d, LH:%d CY:%d\r\n", 
////	FontStruct.FHbits1, mh, fontHeight, PrtStruct.LabelHeight, calc_y);
////MsgOut(MsgBuf);
	if(calc_y > PrtStruct.LabelHeight-margin) {
		LabCmd_PrintStart(1);
		PrintListWaitEnd();
		LabelCurY = 0;
		calc_y = LabelCurY + fontHeight;
	}
	LabelCurY = calc_y + StrStruct.LineSp;
}

void LabCmd_PrtBarcode(INT16U fh, INT16U bh, INT16U ch, INT8U w) 
{
	
	INT16U calc_y;
//	INT8U fontHeight;
	INT16U margin;

	margin = 32;
	calc_y = LabelCurY + bh + ch;

	if(calc_y > PrtStruct.LabelHeight-margin) {
		LabCmd_PrintStart(1);
		PrintListWaitEnd();
		LabelCurY = 0;
		calc_y = LabelCurY + bh + ch;
	}
	LabCmd_MakeBarField(margin, margin+LabelCurY, fh, bh, ch, w);
	LabelCurY = calc_y + StrStruct.LineSp;
}
*/

/***************************************************************
//void print_set_substitutebar(INT8U btype,HUGEDATA char *Str)
{
	char string_buf[100];
	INT16U	i;

	string_buf[0] = 0x1b;
	string_buf[1] = 'B';
	string_buf[2] = 'T';
	string_buf[3] = btype;
	PutDataRxBuf(&CommBufPrt,string_buf,4);

	string_buf[0] = 0x1b;
	string_buf[1] = 'B';
	string_buf[2] = '[';
	i = 0;
	while(Str[i] != '\0') {
		string_buf[3+i] = Str[i];
		i++;
	}
	string_buf[3+i] = '\0';
	PutDataRxBuf(&CommBufPrt,string_buf,4+i);
}
*/

void LabCmd_ClearLabel(void)
{
	LabCmd_ClearLabelForm();
	LabCmd_ClearTphImage();
	status_scale.cur_labelid = 0;
}

/////////////////////////////////////////////////////////////////////
void RcptCmd_SetFontType(INT8U type)
{
	char string_buf[10];
	INT8U i;

	i = 0;
	string_buf[i++] = _ESC;
	string_buf[i++] = 'S';
	string_buf[i++] = 'G';
	string_buf[i++] = type;
	PutDataRxBuf(&CommBufPrt,string_buf,i);
}

void RcptCmd_CharSize(INT8U m, INT8U n)
{
	char string_buf[10];
	INT8U i;
	i = 0;
#ifdef USE_DOUBLE_PRINT
	if(get_global_bparam(GLOBAL_SALERUN_MODE) == 8)
	{
		m -= 1;
		string_buf[i++] = _ESC;
		string_buf[i++] = 'E';
		string_buf[i++] = m;
		PutDataTxBuf(&CommBuf, (char *)string_buf, i);
	}
	else
	{
		//sprintf(string_buf, "%cM%c%cN%c", ESC, m,ESC, n);
		string_buf[i++] = _ESC;
		string_buf[i++] = 'M';
		string_buf[i++] = m;
		string_buf[i++] = _ESC;
		string_buf[i++] = 'N';
		string_buf[i++] = n;

		PutDataRxBuf(&CommBufPrt,string_buf,i);
	}
#else
	//sprintf(string_buf, "%cM%c%cN%c", ESC, m,ESC, n);
	string_buf[i++] = _ESC;
	string_buf[i++] = 'M';
	string_buf[i++] = m;
	string_buf[i++] = _ESC;
	string_buf[i++] = 'N';
	string_buf[i++] = n;

	PutDataRxBuf(&CommBufPrt,string_buf,i);
#endif//USE_DOUBLE_PRINT
}

void RcptCmd_IntercharSP(INT8U inchsp)
{
//	char string_buf[10];
//	sprintf(string_buf, "%cSP", ESC);
//	PutDataRxBuf(&CommBufPrt,string_buf,3);
//	string_buf[0] = m;
//	PutDataRxBuf(&CommBufPrt,string_buf,1);
	char string_buf[10];
	INT8U i;

	if(prev_inchsp != inchsp || StrStruct.InterChSp != inchsp) {
		i = 0;
		string_buf[i++] = _ESC;
		string_buf[i++] = 'S';
		string_buf[i++] = 'P';
		string_buf[i++] = inchsp;
		PutDataRxBuf(&CommBufPrt,string_buf,i);
	}
	prev_inchsp = inchsp;
}

void RcptCmd_LineSP(INT8U m)
{
//	char string_buf[10];
//	sprintf(string_buf, "%cSU", ESC);
//	PutDataRxBuf(&CommBufPrt,string_buf,3);
//	string_buf[0] = m;
//	PutDataRxBuf(&CommBufPrt,string_buf,1);
	char string_buf[10];
	INT8U i;

	i = 0;
	string_buf[i++] = _ESC;
	string_buf[i++] = 'S';
	string_buf[i++] = 'U';
	string_buf[i++] = m;
	PutDataRxBuf(&CommBufPrt,string_buf,i);
}

void RcptCmd_ChRot(INT8U m)
{
	char string_buf[4];
	INT8U i;
	i = 0;
	string_buf[i++] = _ESC;
	string_buf[i++] = 'S';
	string_buf[i++] = 'O';
	string_buf[i++] = m;
	PutDataRxBuf(&CommBufPrt,string_buf,i);
}
void RcptCmd_StrMirx(INT8U m)
{
	char string_buf[4];
	INT8U i;
	i = 0;
	string_buf[i++] = _ESC;
	string_buf[i++] = 'S';
	string_buf[i++] = 'E';
	string_buf[i++] = m;
	PutDataRxBuf(&CommBufPrt,string_buf,i);
}
void RcptCmd_SetFontId(INT8U m)
{
	char string_buf[10];
	INT8U i;
//#	static INT8U prev_fontid = 0xff;
//#
//#	if(m == prev_fontid) return;

	i = 0;
	string_buf[i++] = _ESC;
	string_buf[i++] = 'S';
	string_buf[i++] = 'A';
	string_buf[i++] = m;
	string_buf[i++] = _ESC;
	string_buf[i++] = 'S';
	string_buf[i++] = 'B';
	string_buf[i++] = m;
	PutDataRxBuf(&CommBufPrt,string_buf,i);
//#	prev_fontid = m;
}


void RcptCmd_Start(void)
{
	char string_buf[2];

#ifdef USE_DOUBLE_PRINT	
	if(get_global_bparam(GLOBAL_SALERUN_MODE) == 8)
	{
		string_buf[0] = _CR;
		string_buf[1] = LF;
		PutDataTxBuf(&CommBuf,string_buf,2);
	}
	else
	{
		string_buf[0] = _ESC;
		string_buf[1] = '!';
		PutDataRxBuf(&CommBufPrt,string_buf,2);
	}
#else
    string_buf[0] = _ESC;
	string_buf[1] = '!';
	PutDataRxBuf(&CommBufPrt,string_buf,2);
#endif//USE_DOUBLE_PRINT
}

void RcptCmd_StrJust(INT8U just)
{
//	char string_buf[5];
//	sprintf(string_buf, "%cSJ%c", ESC, just);
//	PutDataRxBuf(&CommBufPrt,string_buf,4);
	char string_buf[10];
	INT8U i;

#ifdef USE_DOUBLE_PRINT
	if(get_global_bparam(GLOBAL_SALERUN_MODE) == 8)
	{
		if(just == RIGHT) {
			just = 2;
		}
		else if(just == CENTER) {
			//just = 1;	//center
			just = 0;	//left		
		}
		else{
		}
		
		if(prev_just != just) {
			
			i = 0;
			string_buf[i++] = _ESC;
			string_buf[i++] = 'a';	//0x61;
			string_buf[i++] = just;
			PutDataTxBuf(&CommBuf, (char *)string_buf, i);
		}
		prev_just = just;
	}
	else
	{
		if(prev_just != just) {
			i = 0;
			string_buf[i++] = _ESC;
			string_buf[i++] = 'S';
			string_buf[i++] = 'J';
			string_buf[i++] = just;
			PutDataRxBuf(&CommBufPrt,string_buf,i);
		}
		prev_just = just;
	}
#else
	if(prev_just != just) {
		i = 0;
		string_buf[i++] = _ESC;
		string_buf[i++] = 'S';
		string_buf[i++] = 'J';
		string_buf[i++] = just;
		PutDataRxBuf(&CommBufPrt,string_buf,i);
	}
	prev_just = just;
#endif//USE_DOUBLE_PRINT 
}

void RcptCmd_TypeBold(void)
{
//	char string_buf[5];
//	sprintf(string_buf, "%cS*", ESC);
//	PutDataRxBuf(&CommBufPrt,string_buf,3);
	char string_buf[10];
	INT8U i;

	i = 0;
	string_buf[i++] = _ESC;
	string_buf[i++] = 'S';
	string_buf[i++] = '*';
	PutDataRxBuf(&CommBufPrt,string_buf,i);
}
/* HYP 20060407 Erase non-use function
void RcptCmd_TypeItalic(void)
{
//	char string_buf[5];
//	sprintf(string_buf, "%cS/", ESC);
//	PutDataRxBuf(&CommBufPrt,string_buf,3);
	char string_buf[10];
	INT8U i;

	i = 0;
	string_buf[i++] = ESC;
	string_buf[i++] = 'S';
	string_buf[i++] = '/';
	PutDataRxBuf(&CommBufPrt,string_buf,i);
}

void RcptCmd_TypeDLine(void)
{
//	char string_buf[5];
//	sprintf(string_buf, "%cS/", ESC);
//	PutDataRxBuf(&CommBufPrt,string_buf,3);
	char string_buf[10];
	INT8U i;

	i = 0;
	string_buf[i++] = ESC;
	string_buf[i++] = 'S';
	string_buf[i++] = '=';
	PutDataRxBuf(&CommBufPrt,string_buf,i);
}
*/
void RcptCmd_TypeUnderline(void)
{
//	char string_buf[5];
//	sprintf(string_buf, "%cS/", ESC);
//	PutDataRxBuf(&CommBufPrt,string_buf,3);
	char string_buf[10];
	INT8U i;

	i = 0;
	string_buf[i++] = _ESC;
	string_buf[i++] = 'S';
	string_buf[i++] = '_';
	PutDataRxBuf(&CommBufPrt,string_buf,i);
}
#ifdef USE_DOUBLE_PRINT
void RcptCmd_TypeThroughLine(INT8U no)
#else
void RcptCmd_TypeThroughLine(void)
#endif//USE_DOUBLE_PRINT
{
//	char string_buf[5];
//	sprintf(string_buf, "%cS/", ESC);
//	PutDataRxBuf(&CommBufPrt,string_buf,3);
	char string_buf[10];
	INT8U i, k;

	i = 0;
#ifdef USE_DOUBLE_PRINT
	if(get_global_bparam(GLOBAL_SALERUN_MODE) == 8)
	{	
		string_buf[i++] = _ESC;
		string_buf[i++] = 0x2A;
		string_buf[i++] = 0x00;
		string_buf[i++] = 0x06;
		string_buf[i++] = 0x00;
		string_buf[i++] = 0x08;
		string_buf[i++] = 0x08;
		string_buf[i++] = 0x08;
		string_buf[i++] = 0x08;
		string_buf[i++] = 0x08;
		string_buf[i++] = 0x08;
		
		for(k=0; k<no; k++)
		{
			PutDataTxBuf(&CommBuf,string_buf,i);
 		}
	}
	else
	{
		string_buf[i++] = _ESC;
		string_buf[i++] = 'S';
		string_buf[i++] = '-';
		PutDataRxBuf(&CommBufPrt,string_buf,i);
	}
#else
	string_buf[i++] = _ESC;
	string_buf[i++] = 'S';
	string_buf[i++] = '-';
	PutDataRxBuf(&CommBufPrt,string_buf,i);
#endif//USE_DOUBLE_PRINT
}

void RcptCmd_TypeNone(void)
{
//	char string_buf[5];
//	sprintf(string_buf, "%cS@", ESC);
//	PutDataRxBuf(&CommBufPrt,string_buf,3);
	char string_buf[10];
	INT8U i;

	i = 0;
	string_buf[i++] = _ESC;
	string_buf[i++] = 'S';
	string_buf[i++] = '@';
	PutDataRxBuf(&CommBufPrt,string_buf,i);
}

INT8U Rcpt_CurNumOfTab;

void RcptCmd_CharTab(INT8U no)
{
	char ch;

	Rcpt_CurNumOfTab += no;
	while(no--) {
		ch = '\t';
#ifdef USE_DOUBLE_PRINT
		if(get_global_bparam(GLOBAL_SALERUN_MODE) == 8)
			PutDataTxBuf(&CommBuf,&ch,1);
		else
			PutDataRxBuf(&CommBufPrt,&ch,1);
#else
		PutDataRxBuf(&CommBufPrt,&ch,1);
#endif//USE_DOUBLE_PRINT
	}
}

void RcptCmd_CharLF(INT8U no)
{
	char ch;

	Rcpt_CurNumOfTab = 0;
	while(no--) {
		ch = '\n';
#ifdef USE_DOUBLE_PRINT
		if(get_global_bparam(GLOBAL_SALERUN_MODE) == 8)
			PutDataTxBuf(&CommBuf,&ch,1);
		else
			PutDataRxBuf(&CommBufPrt,&ch,1);
#else
		PutDataRxBuf(&CommBufPrt,&ch,1);
#endif//USE_DOUBLE_PRINT
	}
}

#ifdef USE_DOUBLE_PRINT
void RcptCmd_PrintPosition(INT8U set)
{
	char string_buf[10];
	INT8U i;

	set *= 12;
	i = 0;
	string_buf[i++] = _ESC;
	string_buf[i++] = 0x24;
	string_buf[i++] = set;	//string_buf[i++] = 0x00;
	string_buf[i++] = 0x00;
	
	PutDataTxBuf(&CommBuf, (char *)string_buf, i);
}

void RcptCmd_StrCenterJust(char *src)	
{
	INT8U *addr;
	INT8U Max_width, width, len;

	addr = (INT8U*)src;
	len = strlen((char *)addr);
	
	Max_width = 33;
	width = ((Max_width - len + 1) / 2);
	RcptCmd_PrintPosition(0);
	RcptCmd_PrintPosition(width);
}

void RcptCmd_CutPaper(void)
{
	char string_buf[10];
	INT8U i;
	
	i = 0;
	string_buf[i++] = GS;
	string_buf[i++] = 'V';	//0x56;
	string_buf[i++] = 0x42;
	string_buf[i++] = 0x01;

	PutDataTxBuf(&CommBuf, (char *)string_buf, i);
}

void RcptCmd_PrintWidth(void)
{
	char string_buf[10];
	INT8U i;

	i = 0;
	string_buf[i++] = GS;
	string_buf[i++] = 'W';
	/*58mm paper width*/
	string_buf[i++] = 0x96;	//nL
	string_buf[i++] = 0x01;	//nH

	PutDataTxBuf(&CommBuf, (char *)string_buf, i);
}

void RcptCmd_SetModeIn(INT8U fn, char* src, INT8U len)
{
	char *temp_src;

	temp_src = src;
	temp_src[len++] = 0x03;
	temp_src[len++] = 0x00;
	temp_src[len++] = fn;
	temp_src[len++] = 0x49;
	temp_src[len++] = 0x4E;

	PutDataTxBuf(&CommBuf, (char *)temp_src, len);
}

void RcptCmd_SetModeOut(INT8U fn, char* src, INT8U len)
{
	char *temp_src;

	temp_src = src;
	temp_src[len++] = 0x04;
	temp_src[len++] = 0x00;
	temp_src[len++] = fn;
	temp_src[len++] = 0x4F;
	temp_src[len++] = 0x55;
	temp_src[len++] = 0x54;

	PutDataTxBuf(&CommBuf, (char *)temp_src, len);
}

void RcptCmd_SetCodePage(INT8U fn, char* src,  INT8U len)
{
	char *temp_src;

	temp_src = src;
	temp_src[len++] = 0x04;
	temp_src[len++] = 0x00;
	temp_src[len++] = fn;
	temp_src[len++] = 0x08;
	temp_src[len++] = 0x10;	//WPC1252 Code Page
	temp_src[len++] = 0x00;

	PutDataTxBuf(&CommBuf, (char *)temp_src, len);
}

void RcptCmd_ValueSend(INT8U fn, char* src,  INT8U len)
{
	char *temp_src;
	
	temp_src = src;
	temp_src[len++] = 0x02;
	temp_src[len++] = 0x00;
	temp_src[len++] = fn;
	temp_src[len++] = 0x08;

	PutDataTxBuf(&CommBuf, (char *)temp_src, len);
}

void RcptCmd_UserSetMdoe(INT8U fn)
{
	char string_buf[100];
	INT16U i;

	i = 0;
	string_buf[i++] = GS;
	string_buf[i++] = '(';	//0x28
	string_buf[i++] = 'E';

	switch(fn) {
		case PRT_CMD_ModeIn	:
		 		RcptCmd_SetModeIn(fn, string_buf, i);
		 	break;
		case PRT_CMD_ModeOut	:
				RcptCmd_SetModeOut(fn, string_buf, i);
			break;
		case PRT_CMD_CodePage	:
				RcptCmd_SetCodePage(fn, string_buf, i);
			break;
		case PRT_CMD_ValueSend	:
				RcptCmd_ValueSend(fn, string_buf, i);
			break;	
	}
}
#endif

void RcptCmd_ReceiptFeed(void)
{
	char string_buf[10];
	INT8U i;

	i = 0;
	string_buf[i++] = _ESC;
	string_buf[i++] = 'F';
	PutDataRxBuf(&CommBufPrt,string_buf,i);
}
/////////////////////////////////////////////////////////////////////

#ifdef USE_DOUBLE_PRINT
void RcptCmd_PrintQrcode(INT8U fn, char *src, INT16U len)
{
	char* temp_src;
	
	temp_src = src;
	temp_src[3] = 0x00;
	temp_src[len++] = fn;
	temp_src[len++] = 0x30;

	PutDataTxBuf(&CommBuf, (char *)temp_src, len);
}

void RcptCmd_QrStorData(INT8U fn, char *src, INT16U len, char *bar_str)
{
	char *temp_src;
	INT16U i, data_len;
	INT8U pl, ph;
	
	i = 0;
	data_len = strlen(bar_str) + 3;
	
	temp_src= PrtStruct.BarBuffer;
	memcpy((INT8U *)temp_src, (INT8U *)src, len);
	
	ph = data_len / 256;
	pl = data_len % 256;

	temp_src[3] = pl;
	temp_src[4] = ph;
	
	temp_src[len++] = fn;
	temp_src[len++] = 0x30;
	
	while(bar_str[i] != '\0') {
		temp_src[len++] = bar_str[i++];
	}

	PutDataTxBuf(&CommBuf, (char *)temp_src, len);
}

void RcptCmd_QrEccLevel(INT8U fn, char *src, INT16U len, INT8U level)
{
	char* temp_src;

	temp_src = src;
	temp_src[len++] = fn;
	temp_src[len++] = level;
	
	PutDataTxBuf(&CommBuf, (char *)temp_src, len);
}

void RcptCmd_QrSize(INT8U fn, char* src, INT16U len, INT8U size)
{
	char* temp_src;

	if(size <= 2 && size >= 8)	//Qrcode Size range 2 <= size <= 8
		size = 3;
	
	temp_src = src;
	temp_src[len++] = fn;
	temp_src[len++] = size;
	PutDataTxBuf(&CommBuf, (char *)temp_src, len);
}

void RcptCmd_MakeQRcode(INT8U size, INT8U level, char* bar_str)
{
	char string_buf[100];
	INT16U i;
	
	i = 0;
	string_buf[i++] = GS;
	string_buf[i++] = '(';	//0x28
	string_buf[i++] = 'k';	//0x6B
	string_buf[i++] = 0x03;	//pL
	string_buf[i++] = 0;	//0x00;	//pH
	string_buf[i++] = 0x31;	//0x31	//cn: QR code
        
	RcptCmd_CharTab(2);	
	RcptCmd_QrSize(SET_MODUL_SIZE, string_buf, i, size);
	RcptCmd_QrEccLevel(ERROR_CORRECTION_LEVEL, string_buf, i, level);
	RcptCmd_QrStorData(STOR_SYMBOL_DATA, string_buf, i, bar_str);
	RcptCmd_PrintQrcode(PRINT_SYMBOL_DATA, string_buf, i); 
}

void RcptCmd_BarcodeCommand(INT8U bar_type, INT8U type_no, char* bar_str, INT8U start_code, INT8U end_code, INT16U cmd_len )
{
	char string_buf[100];
	INT16U i, j, data_len;
	
	i = 0;
	j = 0;

	/*
	* Barcode Command: GS k type_no cmd_len (start_code) data (end_code) null
	*/
	
	string_buf[i++] = GS;
	string_buf[i++] = 'k';	//0x6B
	string_buf[i++] = type_no;
	//string_buf[i++] = cmd_len;

	if(bar_type != CODE39)		
	{
		if(bar_type == CODE128A || bar_type == CODE128B || bar_type == CODE128C)
		{
			cmd_len += 1;	//add 0x7B data len
			string_buf[i++] = cmd_len;
			string_buf[i++] = 0x7B;		//'{'
		}
		else
			string_buf[i++] = cmd_len;
	}
	
	if(start_code)
		string_buf[i++] = start_code;

	while(bar_str[j] != '\0') {
		string_buf[i++] = bar_str[j++];
	}

	if(end_code)
		string_buf[i++] = end_code;
	
	string_buf[i++] = 0x00;	//NULL
	
	if(bar_type == EAN13 || bar_type == EAN8 || bar_type == UPCA)
		RcptCmd_StrCenterJust((char*)string_buf);
	else
		RcptCmd_PrintPosition(0);
	
	PutDataTxBuf(&CommBuf, (char *)string_buf, i);
}
void RcptCmd_MakeBarcode(INT8U bar_type, char* bar_str)
{
	INT8U type_no, start_code, end_code;
	INT16U cmd_len;
	
	type_no = 0;
	start_code = 0;
	end_code = 0;
	cmd_len = strlen(bar_str);

	if(bar_type == CODE128C)
	{
		bar_type = CODE128A;	//CODE128C HEX로 HRI 문자 출력으로 인해 CODE128A로 변경 출력 
	}
	
	switch (bar_type) {
		case UPCA	:
			type_no = 65;	//0x41;
			break;
		case UPCE0	:
			type_no = 66;	//0x42;
			start_code =  48;	//0x30;
			break;
		case EAN13	:
			type_no = 67;	//0x43;
			break;
		case EAN8	:
			type_no = 68;	//0x44;
			break;
		case CODE39	:
			type_no = 4;	//0x04;	//formatA
			//type_no = 69;	//0x45;	//formatB
			break;
		case I2OF5	:
			type_no = 70;	//0x46;
			break;
		case CODABAR	:
			type_no = 71;	//0x47;
			start_code = 65;	//start characters 'A'로 임시 지정
			end_code = 65;		//end characters 'A'로 임시 지정
			break;
		case CODE93	:
			type_no = 72;	//0x48;
			break;
		case CODE128A:
			type_no = 73;	//0x49;
			start_code = 65;	//'A';	 
			break;
		case CODE128B:
			type_no = 73;	//0x49;
			start_code = 66;	//'B';
			break;
		case CODE128C:
			type_no = 73;	//0x49;
			start_code = 67;	//'C';
			break;
	} 

	if(start_code)
		cmd_len += 1;	//add set data len
		
	if(end_code)
		cmd_len += 1;

	RcptCmd_BarcodeCommand(bar_type, type_no, bar_str, start_code, end_code, cmd_len);
}
#endif//USE_DOUBLE_PRINT

void RcptCmd_PrintBarcode(INT8U bar_type, HUGEDATA char* bar_str)
{
#ifdef USE_DOUBLE_PRINT
	if(get_global_bparam(GLOBAL_SALERUN_MODE) == 8)
	{
 #ifdef USE_QR_ADDTOTAL
	if(bar_type == QRCODE){
		RcptCmd_MakeQRcode(3, LEVEL_L_, bar_str);
		}
	else
		RcptCmd_MakeBarcode(bar_type, bar_str);
 #else
		RcptCmd_MakeBarcode(bar_type, bar_str);
 #endif
	}
	else
	{
 #ifdef USE_QR_ADDTOTAL
		char string_buf[1000];  // 출력할 QR Code를 저장하는 버퍼(사이즈 조정)
 #else
		char string_buf[100];
 #endif
		INT16U	i,j;

 #ifdef USE_QR_BARCODE
		memset (string_buf, 0, sizeof(string_buf)); //20140917
 #endif

		i=0;
		string_buf[i++] = _ESC;
		string_buf[i++] = 'B';
	//	string_buf[i++] = '{';
	//	string_buf[i++] = field_num;
	//	string_buf[i++] = 0;
	//	string_buf[i++] = ',';
		string_buf[i++] = 'T';
		string_buf[i++] = bar_type;
		switch (bar_type) {
			case CODE128C:	// Deleted by CJK 20041213//Recovered by HYP 20050528
			case CODE11  :
			case I2OF5   :
			case S2OF5   :
			case M2OF5   :
			case MSI     :
		        string_buf[i++] = _ESC;
				string_buf[i++] = 'B';
				string_buf[i++] = 'B';
				string_buf[i++] = 1;
				break;
 #ifdef USE_BARCODE_CODE128X
			case CODE128X:
				break;
 #endif
 #ifdef USE_QR_BARCODE // 20140916
			case QRCODE  :
				break;
 #endif
		}
		string_buf[i++] = _ESC;
		string_buf[i++] = 'B';
		string_buf[i++] = '[';
		j=0;
		while(bar_str[j] != '\0') {
			string_buf[i++] = bar_str[j++];
		}
 #ifdef USE_QR_ADDTOTAL
		if ((bar_type == QRCODE) && (TotalQRFlag == 1)) {
			if (QrCfg_DelLastSuffix) // Not Use Last Suffix
			{
				string_buf[i-2] = '\0';
				string_buf[i-1] = '\0';
			}
		}
 #endif
		string_buf[i++] = '\0';
		PutDataRxBuf(&CommBufPrt,string_buf,i);
	}
#else//USE_DOUBLE_PRINT
 #ifdef USE_QR_ADDTOTAL
	char string_buf[1000];  // 출력할 QR Code를 저장하는 버퍼(사이즈 조정)
 #else
	char string_buf[100];
 #endif
	INT16U	i,j;

 #ifdef USE_QR_BARCODE
	memset (string_buf, 0, sizeof(string_buf)); //20140917
 #endif

	i=0;
	string_buf[i++] = _ESC;
	string_buf[i++] = 'B';
//	string_buf[i++] = '{';
//	string_buf[i++] = field_num;
//	string_buf[i++] = 0;
//	string_buf[i++] = ',';
	string_buf[i++] = 'T';
	string_buf[i++] = bar_type;
	switch (bar_type) {
		case CODE128C:	// Deleted by CJK 20041213//Recovered by HYP 20050528
		case CODE11  :
		case I2OF5   :
		case S2OF5   :
		case M2OF5   :
		case MSI     :
	        string_buf[i++] = _ESC;
			string_buf[i++] = 'B';
			string_buf[i++] = 'B';
			string_buf[i++] = 1;
			break;
 #ifdef USE_BARCODE_CODE128X
		case CODE128X:
			break;
 #endif
 #ifdef USE_QR_BARCODE // 20140916
		case QRCODE  :
			break;
 #endif
	}
	string_buf[i++] = _ESC;
	string_buf[i++] = 'B';
	string_buf[i++] = '[';
	j=0;
	while(bar_str[j] != '\0') {
		string_buf[i++] = bar_str[j++];
	}
 #ifdef USE_QR_ADDTOTAL
	if ((bar_type == QRCODE) && (TotalQRFlag == 1)) {
		if (QrCfg_DelLastSuffix) // Not Use Last Suffix
		{
			string_buf[i-2] = '\0';
			string_buf[i-1] = '\0';
		}
	}
 #endif
	string_buf[i++] = '\0';
	PutDataRxBuf(&CommBufPrt,string_buf,i);


//	char string_buf[30];
//
//	sprintf(string_buf, "%cBT", ESC);
//	PutDataRxBuf(&CommBufPrt,string_buf,3);
//	string_buf[0] = bar_type;
//	PutDataRxBuf(&CommBufPrt,string_buf,1);
//
//	sprintf(string_buf, "%cB[%s", ESC, bar_str);
//	PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf)+1);
#endif//USE_DOUBLE_PRINT
}

extern void PrintListWaitEnd(void);
void RcptCmd_ChangeMode(INT8U s_mode, INT8U is_backup)		// TO_RECEIPT, TO_RESTORE
{
	char string_buf[10];
	static INT8U backup_prt_mode, backup_gap_mode;

	if(s_mode == TO_RESTORE || s_mode == TO_RESTORE_FORCE) {	//TO_RESTORE
		if(PrtStruct.Mode != backup_prt_mode || s_mode == TO_RESTORE_FORCE) {
			sprintf(string_buf, "%cO", GS);
			PutDataRxBuf(&CommBufPrt,string_buf,2);
			string_buf[0] = backup_prt_mode;
			PutDataRxBuf(&CommBufPrt,string_buf,1);

			status_scale.cur_labelid = 0;	// Reload Label : Added by CJK 20050224
		}
		if(PrtDrvStruct.LabelGapMode != backup_gap_mode || s_mode == TO_RESTORE_FORCE) {
			sprintf(string_buf, "%cJ", GS);
			PutDataRxBuf(&CommBufPrt,string_buf,2);
			string_buf[0] = backup_gap_mode;
			PutDataRxBuf(&CommBufPrt,string_buf,1);
		}
		PrintListWaitEnd();
		return;
	}

	if(is_backup) {
		backup_prt_mode = PrtStruct.Mode;
		backup_gap_mode = PrtDrvStruct.LabelGapMode;
	}

	if(s_mode == TO_BACKUP) {
		return;
	} else if(s_mode == TO_RECEIPT) {
		if(PrtStruct.Mode != RECEIPT) {
			if(PrtFlashStruct.RewindEnable) {
				Operation.rewindReady = ON;
			}

			sprintf(string_buf, "%cO", GS);
			PutDataRxBuf(&CommBufPrt,string_buf,2);
			string_buf[0] = RECEIPT;
			PutDataRxBuf(&CommBufPrt,string_buf,1);
			PrintListWaitEnd();
		}
	} else if(s_mode == TO_GAPLABEL) {
		if(PrtStruct.Mode != LABEL) {
			sprintf(string_buf, "%cO", GS);
			PutDataRxBuf(&CommBufPrt,string_buf,2);
			string_buf[0] = LABEL;
			PutDataRxBuf(&CommBufPrt,string_buf,1);
		}
		sprintf(string_buf, "%cJ", GS);
		PutDataRxBuf(&CommBufPrt,string_buf,2);
		string_buf[0] = GAP;
		PutDataRxBuf(&CommBufPrt,string_buf,1);
		PrintListWaitEnd();
	} else if(s_mode == TO_NOGAPLABEL) {
		if(PrtStruct.Mode != LABEL) {
			sprintf(string_buf, "%cO", GS);
			PutDataRxBuf(&CommBufPrt,string_buf,2);
			string_buf[0] = LABEL;
			PutDataRxBuf(&CommBufPrt,string_buf,1);
		}
		sprintf(string_buf, "%cJ", GS);
		PutDataRxBuf(&CommBufPrt,string_buf,2);
		string_buf[0] = NO_GAP;
		PutDataRxBuf(&CommBufPrt,string_buf,1);
		PrintListWaitEnd();
	}
}

//extern HUGEDATA FONT1_STRUCT Font1Struct[MAX_FONT1_NUM];


void RcptCmd_BarHeight(INT16U fh, INT16U bh, INT16U ch) 
{
	INT16U	i;
	char string_buf[50];
	CWD cwd;

	i = 0;
#ifdef USE_DOUBLE_PRINT
	if(get_global_bparam(GLOBAL_SALERUN_MODE) == 8)
	{
		string_buf[i++] = GS;
		string_buf[i++] = 'h';	//0x68;
		string_buf[i++] = bh;
		
		PutDataTxBuf(&CommBuf,string_buf, i);
	}
	else
	{
		string_buf[i++] = _ESC;
		string_buf[i++] = 'B';
		string_buf[i++] = 'H';
		cwd.word = fh;
		string_buf[i++] = cwd.byte.left;
		string_buf[i++] = cwd.byte.right;
		cwd.word = bh;
		string_buf[i++] = cwd.byte.left;
		string_buf[i++] = cwd.byte.right;
		cwd.word = ch;
		string_buf[i++] = cwd.byte.left;
		string_buf[i++] = cwd.byte.right;
		
		PutDataRxBuf(&CommBufPrt, string_buf, i);
	}
#else
	string_buf[i++] = _ESC;
	string_buf[i++] = 'B';
	string_buf[i++] = 'H';
	cwd.word = fh;
	string_buf[i++] = cwd.byte.left;
	string_buf[i++] = cwd.byte.right;
	cwd.word = bh;
	string_buf[i++] = cwd.byte.left;
	string_buf[i++] = cwd.byte.right;
	cwd.word = ch;
	string_buf[i++] = cwd.byte.left;
	string_buf[i++] = cwd.byte.right;

	PutDataRxBuf(&CommBufPrt, string_buf, i);
#endif//USE_DOUBLE_PRINT
}


void RcptCmd_BarWidth(INT8U w)
{
	INT16U	i;
	char string_buf[50];

	i = 0;
#ifdef USE_DOUBLE_PRINT
	if(get_global_bparam(GLOBAL_SALERUN_MODE) == 8)
	{
		string_buf[i++] = GS;
		string_buf[i++] = 'w';	//0x77
		string_buf[i++] = w;
		
		PutDataTxBuf(&CommBuf,string_buf, i);
	}
	else
	{
		string_buf[i++] = _ESC;
		string_buf[i++] = 'B';
		string_buf[i++] = 'W';
		string_buf[i++] = w;

		PutDataRxBuf(&CommBufPrt, string_buf, i);
	}
#else
	string_buf[i++] = _ESC;
	string_buf[i++] = 'B';
	string_buf[i++] = 'W';
	string_buf[i++] = w;

	PutDataRxBuf(&CommBufPrt, string_buf, i);
#endif//USE_DOUBLE_PRINT
}



void RcptCmd_BarGuard(INT8U guard) 
{
	INT16U	i;
	char string_buf[50];
//	CWD cwd;

	i = 0;
	string_buf[i++] = _ESC;
	string_buf[i++] = 'B';
	string_buf[i++] = 'G';
	string_buf[i++] = guard;

	PutDataRxBuf(&CommBufPrt, string_buf, i);
}

#ifdef USE_DOUBLE_PRINT
void RcptCmd_BarHRI(INT8U position)
{
	INT8U pt;
	INT16U i;
	char string_buf[10];

	position = pt;
	if(pt == HRI_POS_DOWN)
		pt = 2;
	
	i = 0;
	string_buf[i++] = GS;
	string_buf[i++] = 'H';	//0x48
	string_buf[i++] = pt;

	PutDataTxBuf(&CommBuf,string_buf, i);
}

void RcptCmd_BarFont(INT8U font_type)
{
	INT16U i;
	char string_buf[10];

	i = 0;
	string_buf[i++] = GS;
	string_buf[i++] = 'f';	//0x66
	string_buf[i++] = font_type;

	PutDataTxBuf(&CommBuf, string_buf, i);
}
#endif//USE_DOUBLE_PRINT

INT8U rcpt_tab_pt[4];
INT8U rcpt_tab_sz[4];
#ifdef USE_DOUBLE_PRINT
INT8U tab_cp_1[5];
INT8U tab_cp_2[5];
#endif//USE_DOUBLE_PRINT
INT8U Rcpt_MaxWidthChar = 0;

//INT8U rcpt_tab_gap_big, rcpt_tab_gap;
			//  0	1   2	3  num
//INT8U tab_sz_1[5][5] = { {  7,  9, 10,  2,  3},		//TKT_LAY_0	// Max:30  40mm(30/36 char) / 60mm(50/56 char)
//		         {  8,  8, 12,  2,  3},		//TKT_LAY_1
//		         { 14, 14,  0,  2,  2},		//TKT_LAY_2
//		         { 28,  0,  0,  0,  1},		//TKT_LAY_3
//		         { 10,  9,  9,  2,  3} };	//TKT_LAY_4

//INT8U tab_sz_2[3][5] = { {  7,  8,  0,  0,  2},	//TKT_LAY_0	// Max:15  40mm(15/18 char) / 60mm(25/28 char)
//		         {  6,  9,  0,  0,  1},		//TKT_LAY_1
//		         { 14,  0,  0,  0,  1},		//TKT_LAY_2

//SG070503
			//  0	1   2	3  num
INT8U tab_sz_1[7][5] = { {  7,  9, 10,  2,  3},		//TKT_LAY_0	// Max:30  40mm(30/36 char) / 60mm(50/56 char)
		         {  8,  8, 12,  2,  3},		//TKT_LAY_1
		         { 14, 14,  0,  2,  2},		//TKT_LAY_2
		         { 28,  0,  0,  0,  1},		//TKT_LAY_3
		         { 10,  9,  9,  2,  3}, 	//TKT_LAY_4
			 { 12,  18, 0,  0,  1}, 	//TKT_LAY_5
			 {  7,  9, 10,  2,  3} };	//TKT_LAY_6
//SG070503 
INT8U tab_sz_2[7][5] = { {  6,  8,  8,  2,  3},		//TKT_LAY_0	// Max:15  40mm(15/18 char) / 60mm(25/28 char)
		         {  0,  0,  0,  0,  0},		//TKT_LAY_1
		         { 14,  0,  0,  0,  1},		//TKT_LAY_2
		         {  6,  7,  8,  2,  3}, 	//TKT_LAY_3
			 {  0,  0,  0,  0,  0}, 	//TKT_LAY_4
			 {  6,  9,  0,  0,  1}, 	//TKT_LAY_5
			 {  3,  3,  9,  2,  3} };	//TKT_LAY_6
#ifdef USE_DOUBLE_PRINT
/*Double print tab size*/
INT8U tab_sz_3[7][5] = { {  7,  9,  9,  2,  3},		//TKT_LAY_0	
		       	{  7,  9,  9,  2,  3},				//TKT_LAY_1
		         { 14, 14,  0,  2,  2},		//TKT_LAY_2
		         { 28,  0,  0,  0,  1},		//TKT_LAY_3
		         { 10,  6,  9,  2,  3},//{ 10,  9,  9,  2,  3}, 	//TKT_LAY_4
				 { 12,  21, 0,  0,  1},		//TKT_LAY_5
				 {  6,  9, 10,  2,  3}};	//TKT_LAY_6
INT8U tab_sz_4[7][5] = { {  6,  8,  8,  2,  3},		//TKT_LAY_0	
		         {  6,  8,  8,  2,  3}, 	//TKT_LAY_1
		         { 14,  0,  0,  0,  1},		//TKT_LAY_2
		         {  6,  7,  8,  2,  3}, 	//TKT_LAY_3
				 {  0,  0,  0,  0,  0}, 	//TKT_LAY_4
				 {  11,  20,  0,  0,  1},	//TKT_LAY_5
				 {  3,  3,  9,  2,  3}};		//TKT_LAY_6	
#endif//USE_DOUBLE_PRINT
//extern INT8U num_chars;

void RcptCmd_SetFontStyle(INT8U fontid, INT8U font_type, INT8U layer_type, INT8U inchsp, INT8U just)
{
//	INT8U num_chars;
	INT16U i;		
	INT8U ftype, tab_gap, diff, last_pt;


	RcptCmd_SetFontId(fontid);
	i = Font1Struct[fontid].Wbits1;
	//SG if(prev_font_type != font_type) {
		switch(font_type) {
			case RPT_FONT_SMALL :
				RcptCmd_CharSize(1, 1);
				//i *= 1;
				break;
			case RPT_FONT_NORMAL :
				RcptCmd_CharSize(1, 2);
				//i *= 1;
				break;
			case RPT_FONT_SUBTITLE :
				RcptCmd_CharSize(2, 2);
				i *= 2;
				break;
			case RPT_FONT_TITLE  :
				RcptCmd_CharSize(2, 3);
				i *= 2;
				break;
		}

	//SG }
	//SG prev_font_type = font_type;
#ifdef USE_DOUBLE_PRINT
if(get_global_bparam(GLOBAL_SALERUN_MODE) != 8)
	RcptCmd_IntercharSP(inchsp);
#else
	RcptCmd_IntercharSP(inchsp);
#endif
	RcptCmd_StrJust(just);
	
#ifdef USE_DOUBLE_PRINT
	i += inchsp;
	Rcpt_MaxWidthChar = PrtStruct.ReceiptWidth/i - 1;

	if(get_global_bparam(GLOBAL_SALERUN_MODE) == 8)
		Rcpt_MaxWidthChar = 33;	//Double print-Max Width size

	for(i = 0; i < 5; i++)	// 모드설정에 따라 정의된 배열을 복사하여 사용할 수 있도록 수정
	{
		if(get_global_bparam(GLOBAL_SALERUN_MODE) == 8)
		{
			tab_cp_1[i] = tab_sz_3[layer_type][i];
			tab_cp_2[i] = tab_sz_4[layer_type][i];
		}
		else
		{
			tab_cp_1[i] = tab_sz_1[layer_type][i];
			tab_cp_2[i] = tab_sz_2[layer_type][i];
		}
	}

	ftype = font_type/2;
	if(ftype == 0) {
		if((Rcpt_MaxWidthChar%2)) Rcpt_MaxWidthChar--;
		//Rcpt_MaxWidthChar -= 6;	// DEL070124
		if(layer_type == 0xff) return;
		if(layer_type > 6) layer_type = 0;

		last_pt = tab_cp_1[4];
		tab_gap = 0;
		//for(i = 0; i < last_pt; i++) {
		
		if(get_global_bparam(GLOBAL_SALERUN_MODE) == 8)
		{
			tab_gap =2;
			if((Rcpt_MaxWidthChar-6) < tab_gap) tab_gap = 0;
			else
				diff = (Rcpt_MaxWidthChar-6) - tab_gap;
		}
		else
		{
			for(i = 0; i < 3; i++) {
				tab_gap += tab_cp_1[i];
			}
			if((Rcpt_MaxWidthChar-6) < tab_gap) tab_gap = 0;
			else {
				diff = (Rcpt_MaxWidthChar-6) - tab_gap; // Margin (left:4char, right:2char) 포함
				tab_gap = diff / last_pt;
			}
		}

		rcpt_tab_pt[0] = 0;
		
		for(i = 0; i < last_pt; i++) {
			rcpt_tab_sz[i] = tab_cp_1[i] + tab_gap;
			if(diff%tab_gap) {
				rcpt_tab_sz[i]++;
				diff--;
			}
			rcpt_tab_pt[i+1] = rcpt_tab_pt[i] + rcpt_tab_sz[i];
		}
		rcpt_tab_sz[i] = tab_cp_1[i];
		i++;
		for(;i < 4; i++) {
			rcpt_tab_pt[i] = rcpt_tab_pt[i-1] + rcpt_tab_sz[i-1];
			rcpt_tab_sz[i] = tab_cp_1[i];
		}
	} else {
		//if((Rcpt_MaxWidthChar%2)) Rcpt_MaxWidthChar--;
		//Rcpt_MaxWidthChar -= 3;	// DEL070124
		if(layer_type == 0xff) return;
		//SG if(layer_type > 2) layer_type = 0;
		if(layer_type > 6) layer_type = 0;

		last_pt = tab_cp_2[4];

		tab_gap = 0;
		//for(i = 0; i < last_pt; i++) {
		for(i = 0; i < 3; i++) {
			tab_gap += tab_cp_2[i];
		}
		if((Rcpt_MaxWidthChar-3) < tab_gap) tab_gap = 0;
		else {
			diff = (Rcpt_MaxWidthChar-3) - tab_gap;	// Margin (left:2char, right:1char) 포함
			tab_gap = diff / last_pt;
		}

		rcpt_tab_pt[0] = 0;
		for(i = 0; i < last_pt; i++) {
			rcpt_tab_sz[i] = tab_cp_2[i] + tab_gap;
			//SG if(diff%tab_gap) {
			//SG	rcpt_tab_sz[i]++;
			//SG	diff--;
			//SG }
			rcpt_tab_pt[i+1] = rcpt_tab_pt[i] + rcpt_tab_sz[i];
		}
		rcpt_tab_sz[i] = tab_cp_2[i];
		i++;
		for(;i < 4; i++) {
			rcpt_tab_pt[i] = rcpt_tab_pt[i-1] + rcpt_tab_sz[i-1];
			rcpt_tab_sz[i] = tab_cp_2[i];
		}
	}
#else//USE_DOUBLE_PRINT
	i += inchsp;

	Rcpt_MaxWidthChar = PrtStruct.ReceiptWidth/i - 1;

	ftype = font_type/2;
	if(ftype == 0) {
		if((Rcpt_MaxWidthChar%2)) Rcpt_MaxWidthChar--;
		//Rcpt_MaxWidthChar -= 6;	// DEL070124
		if(layer_type == 0xff) return;
		if(layer_type > 6) layer_type = 0;

		last_pt = tab_sz_1[layer_type][4];

		tab_gap = 0;
		//for(i = 0; i < last_pt; i++) {
		for(i = 0; i < 3; i++) {
			tab_gap += tab_sz_1[layer_type][i];
		}
		if((Rcpt_MaxWidthChar-6) < tab_gap) tab_gap = 0;
		else {
			diff = (Rcpt_MaxWidthChar-6) - tab_gap;	// Margin (left:4char, right:2char) 포함
			tab_gap = diff / last_pt;
		}

		rcpt_tab_pt[0] = 0;
		for(i = 0; i < last_pt; i++) {
			rcpt_tab_sz[i] = tab_sz_1[layer_type][i] + tab_gap;          
    		if(tab_gap && diff%tab_gap) {
    			rcpt_tab_sz[i]++;
    			diff--;
    		}
			rcpt_tab_pt[i+1] = rcpt_tab_pt[i] + rcpt_tab_sz[i];
		}
		rcpt_tab_sz[i] = tab_sz_1[layer_type][i];
		i++;
		for(;i < 4; i++) {
			rcpt_tab_pt[i] = rcpt_tab_pt[i-1] + rcpt_tab_sz[i-1];
			rcpt_tab_sz[i] = tab_sz_1[layer_type][i];
		}
	} else {
		//if((Rcpt_MaxWidthChar%2)) Rcpt_MaxWidthChar--;
		//Rcpt_MaxWidthChar -= 3;	// DEL070124
		if(layer_type == 0xff) return;
		//SG if(layer_type > 2) layer_type = 0;
		if(layer_type > 6) layer_type = 0;

		last_pt = tab_sz_2[layer_type][4];

		tab_gap = 0;
		//for(i = 0; i < last_pt; i++) {
		for(i = 0; i < 3; i++) {
			tab_gap += tab_sz_2[layer_type][i];
		}
		if((Rcpt_MaxWidthChar-3) < tab_gap) tab_gap = 0;
		else {
			diff = (Rcpt_MaxWidthChar-3) - tab_gap;	// Margin (left:2char, right:1char) 포함
			tab_gap = diff / last_pt;
		}

		rcpt_tab_pt[0] = 0;
		for(i = 0; i < last_pt; i++) {
			rcpt_tab_sz[i] = tab_sz_2[layer_type][i] + tab_gap;
			//SG if(diff%tab_gap) {
			//SG	rcpt_tab_sz[i]++;
			//SG	diff--;
			//SG }
			rcpt_tab_pt[i+1] = rcpt_tab_pt[i] + rcpt_tab_sz[i];
		}
		rcpt_tab_sz[i] = tab_sz_2[layer_type][i];
		i++;
		for(;i < 4; i++) {
			rcpt_tab_pt[i] = rcpt_tab_pt[i-1] + rcpt_tab_sz[i-1];
			rcpt_tab_sz[i] = tab_sz_2[layer_type][i];
		}
	}
#endif//USE_DOUBLE_PRINT
	//Rcpt_MaxWidthChar = rcpt_tab_pt[last_pt-1] + rcpt_tab_sz[last_pt-1];

//SGTEST
//sprintf(MsgBuf, "[%d][%d]nch=%3d, diff=%d, sz0=%2d, sz1=%2d, sz2=%2d, sz3=%2d, max=%2d, gap=%2d,layer=%2d\r\n"
//		, fontid, font_type, PrtStruct.ReceiptWidth, diff, rcpt_tab_sz[0], rcpt_tab_sz[1], rcpt_tab_sz[2], rcpt_tab_sz[3], Rcpt_MaxWidthChar, tab_gap,layer_type);
//MsgOut(MsgBuf);

}

//extern INT8U prev_font_type, prev_inchsp, prev_just;
void RcptCmd_DefaultSet(void)
{
	prev_font_type = 0xff;
	prev_inchsp = 0xff;
	prev_just = 0xff;

	RcptCmd_SetFontType(PrtFontType);//0=one byte font, 1=two byte font
	RcptCmd_SetFontId(3);
	RcptCmd_IntercharSP(0);
	//RcptCmd_LineSP(4);
	RcptCmd_LineSP(0);
	RcptCmd_TypeNone();
	RcptCmd_ChRot(0);		//이전 설정값 초기화
	RcptCmd_StrMirx(0);		//이전 설정값 초기화
}

void RcptCmd_PutStringInit(char *dst)
{
	INT8U i, size;

	size = Rcpt_MaxWidthChar;
	if(size > sizeof(dst)) size = sizeof(dst) - 1;
	for(i = 0; i < size; i++) {
		dst[i] = ' ';
	}
	dst[size] = 0;
}


void RcptCmd_PutString(INT8U position, char *dst, char *src, INT8U align)
{
	INT8U x, max_len, len, i, start_x;
	char align_t[3] = {'-', '+', '-'};
	char form[64];
	//char backup_ch;
	char temp_src[64];

	x       = rcpt_tab_pt[position];
	max_len = rcpt_tab_sz[position];

//	_memcpy((INT8U*)temp_src, (INT8U*)src, 64);
	memcpy((INT8U*)temp_src, (INT8U*)src, 64);
	temp_src[63] = 0;
	start_x = 0;
	len = strlen(temp_src);

//	backup_ch = 0;
	if(len > max_len) {
		//backup_ch = src[max_len];
		//if(position == 0 || len > x+max_len) {
		if(len > x+max_len) {
			temp_src[max_len] = 0;
		} else {
			x = x - (len - max_len);
			max_len = len;
		}
	} else {
		if(align == CENTER) {
			start_x = max_len - len;
			start_x++;	// 홀수 일때, 한칸 오른쪽으로 이동
			start_x /= 2;
			for(i=0;i<start_x;i++) form[i] = ' ';
		}
	}
	sprintf(form+start_x, "%%%c%ds", align_t[align], max_len-start_x);
	sprintf(dst+x, form, temp_src);
	//if(backup_ch) src[max_len] = backup_ch;
}


INT8U RcptCmd_PrintMultiLine(char *src, INT16U max_size, INT8U num_tab)
{
	INT16U len, prt_len;
	INT8U *addr;
	INT8U temp_tab, num_tab_ch;
	INT8U line_no;
	INT16U i;
	INT8U kor_flag;

	line_no = 0;
	addr = (INT8U*)src;
	if(num_tab >= Rcpt_CurNumOfTab) temp_tab = num_tab - Rcpt_CurNumOfTab;
	else temp_tab = num_tab;
	len = strlen((char *)addr);
	if(len > max_size) len = max_size;
	if(len == 0) {
		RcptCmd_CharLF(1);
		line_no++;
	}
	while(len > 0) {
		prt_len = (INT16U)Rcpt_MaxWidthChar;// + 4;
		RcptCmd_CharTab(temp_tab);
		if(Rcpt_CurNumOfTab > 0) {
			num_tab_ch = PrtFlashStruct.HTabCh[Rcpt_CurNumOfTab-1];
			prt_len -= (INT16U)num_tab_ch;
		}
		if(prt_len > len) prt_len = len;
		kor_flag = 0;
		for(i = 0; i < prt_len; i++) {
			if(DspStruct.StrType == TWO_BYTE_FONT_STR) {	// Added by CJK 20060223
				if(kor_flag == 0) {
					if(addr[i] >= 0x80) kor_flag = 1;
				} else {
					kor_flag = 0;
				}
			}
			if(addr[i] == LF) {
				prt_len = i;
				break;
			}
		}
		if(DspStruct.StrType == TWO_BYTE_FONT_STR) {	// Added by CJK 20060223
			if(kor_flag == 1) prt_len--;
		}
#ifdef USE_DOUBLE_PRINT
		if(get_global_bparam(GLOBAL_SALERUN_MODE) == 8)
			PutDataTxBuf(&CommBuf, (char *)addr, prt_len);
		else
			PutDataRxBuf(&CommBufPrt,( char *)addr,prt_len);
#else
		PutDataRxBuf(&CommBufPrt,(HUGEDATA char *)addr,prt_len);
#endif//USE_DOUBLE_PRINT
		RcptCmd_CharLF(1);
		len -= prt_len;
		addr += prt_len;
		temp_tab = num_tab;
		line_no++;
		if(*addr == LF) {
			addr++;
			len--;
		}
	}
	return line_no;
}


void RcptCmd_PrintLine(INT8U line_no, INT8U width)
{
	char string_buf[100];
	char line_ch[2] = {'-', '='};
	INT8U size;

#ifdef USE_DOUBLE_PRINT
	if(get_global_bparam(GLOBAL_SALERUN_MODE) == 8)
	{
		size = 33;	//size = PrtStruct.ReceiptWidth;
		RcptCmd_PrintPosition(0);
	
		line_no--;
		memset((INT8U *)string_buf, line_ch[line_no], size);
		string_buf[size] = 0;
		PutDataTxBuf(&CommBuf, (char *)string_buf, size);
	}
	else
	{
		if(width) size = width;
		else	  size = Rcpt_MaxWidthChar - 2;// L_Margin:2char	// + 4; // 왼쪽 TAB(4char)

		line_no--;
		line_no %= 2;
		//RcptCmd_SetFontStyle(3, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
		memset((INT8U *)string_buf, line_ch[line_no], size);
		string_buf[size] = 0;
		PutDataRxBuf(&CommBufPrt, string_buf, size);
	}
#else
	if(width) size = width;
	else      size = Rcpt_MaxWidthChar - 2;// L_Margin:2char	// + 4;	// 왼쪽 TAB(4char)

	line_no--;
	line_no %= 2;
	//RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
	memset((INT8U *)string_buf, line_ch[line_no], size);
	string_buf[size] = 0;
	PutDataRxBuf(&CommBufPrt, string_buf, size);
#endif//USE_DOUBLE_PRINT
	RcptCmd_CharLF(1);
}


void RcptCmd_PrintChar(char ch)
{
	PutDataRxBuf(&CommBufPrt, &ch, 1);
}


void RcptCmd_PrintCharLine(char ch, INT8U length)
{
	char string_buf[100];
	INT8U size;

	if(length) size = length;
	else      size = Rcpt_MaxWidthChar - 2;// L_Margin:2char	// + 4;	// 왼쪽 TAB(4char)

	memset((INT8U *)string_buf, ch, size);
	string_buf[size] = 0;
	PutDataRxBuf(&CommBufPrt, string_buf, size);
}

#ifdef USE_ARAB_CONVERT
void RcptCmd_PrintArabAllign(char *str, INT8U maxCapSize)
{
	INT8U i, len;
	
	len = strlen(str);
	
	if(len < maxCapSize)
	{
		for(i=0;i<maxCapSize-len;i++) 
		{
			RcptCmd_PrintChar(' ');
		}
	}
	RcptCmd_CharTab(1);
	for(i=0;i<15;i++) RcptCmd_PrintChar(' ');
}
#endif
