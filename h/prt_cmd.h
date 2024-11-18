/*****************************************************************************
|*  Copyright		:	(c) 2003 CAS
|*  Version		:	0.1
|*  Modified		:	2004/08/20
****************************************************************************/

#define TO_RECEIPT		0
#define TO_RESTORE		1
#define TO_GAPLABEL		2
#define TO_NOGAPLABEL	3
#define TO_BACKUP		4
#define TO_RESTORE_FORCE	5



//#define	RPT_NORMAL_FONT_MX	1
//#define RPT_NORMAL_FONT_MY	2
//#define RPT_TITLE_FONT_MX	2
//#define RPT_TITLE_FONT_MY	2

#define RPT_FONT_SMALL	0
#define RPT_FONT_NORMAL	1
#define RPT_FONT_SUBTITLE	2
#define RPT_FONT_TITLE	3

#define TKT_LAY_0	0
#define TKT_LAY_1	1
#define TKT_LAY_2	2
#define TKT_LAY_3	3
#define TKT_LAY_4	4
#define TKT_LAY_5   5  
#define TKT_LAY_6   6  
#define TKT_LAY_N	0xff

#define PRT_CMD_ModeIn		1
#define PRT_CMD_ModeOut		2
#define PRT_CMD_CodePage	5
#define PRT_CMD_ValueSend	6

extern INT16U LabelCurY;
//!extern INT8U Rcpt_WidthMode;
extern INT8U Rcpt_MaxWidthChar;

extern void LabCmd_PrintStart(INT8U num);
extern void LabCmd_ReverseFeed(void);
extern void LabCmd_DrawBarInField(INT8U field_num,INT8U bar_type,HUGEDATA char *Str);
extern void LabCmd_SetBarField(INT8U field_num);
extern void LabCmd_PrtBarcode(INT16U fh, INT16U bh, INT16U ch, INT8U w);
extern void LabCmd_SetTextField(INT8U field_num,char *str);
extern void LabCmd_SetTextField_Direct(INT8U field_num, char *str, INT16U m_size);
extern void LabCmd_SetTextField_DirectIngredient(INT8U field_num, INT8U *ingred_text, int len);
extern void RcptCmd_SetTextField_DirectIngredient(INT8U field_num, INT16U *ingred_no);
extern void LabCmd_SetStrField(INT16U x, INT16U y, INT16U h, INT16U w);
extern void LabCmd_MakeBarField(INT16U x, INT16U y, INT16U fh, INT16U bh, INT16U ch, INT8U w); 
extern void LabCmd_CharSize(INT8U m, INT8U n);
extern void LabCmd_StrStart(void);
extern void LabCmd_StrEnd(void);
extern void LabCmd_ClearLabelForm(void);
extern void LabCmd_ClearTphImage(void);
extern void LabCmd_InvertPrt(INT8U mode);
extern void LabCmd_FormFeed(void);
extern void LabCmd_AlignmentFeed(void);
extern void LabCmd_RotateStr(INT8U rot);
extern void LabCmd_RotateBar(INT8U rot);
extern void LabCmd_PrtStrLine(INT8U x, INT8U mw, INT8U mh, char *str, INT8U linefeed);
extern void LabCmd_SkipLine(INT8U line);
extern void LabCmd_ClearLabel(void);

extern void RcptCmd_SetFontType(INT8U type);
extern void RcptCmd_ChangeMode(INT8U s_mode, INT8U is_backup);		// TO_RECEIPT, TO_RESTORE
extern void RcptCmd_CharSize(INT8U m, INT8U n);
extern void RcptCmd_Start(void);
extern void RcptCmd_CharTab(INT8U no);
extern void RcptCmd_CharLF(INT8U no);
extern void RcptCmd_ReceiptFeed(void);
extern void RcptCmd_DefaultSet(void);
extern void RcptCmd_PrintBarcode(INT8U bar_type, HUGEDATA char* bar_str);
extern void RcptCmd_PrtStrLine(INT8U x, INT8U mw, INT8U mh, char *str, INT8U linefeed);

// Common
extern void RcptCmd_SetFontId(INT8U m);
extern void RcptCmd_IntercharSP(INT8U m);
extern void RcptCmd_LineSP(INT8U m);
extern void RcptCmd_StrJust(INT8U m);
extern void RcptCmd_TypeBold(void);
extern void RcptCmd_TypeItalic(void);
extern void RcptCmd_TypeDLine(void);
extern void RcptCmd_TypeUnderline(void);
#ifdef USE_DOUBLE_PRINT
 extern void RcptCmd_TypeThroughLine(INT8U no);
 extern void RcptCmd_StrCenterJust(char *src);	
#else
 extern void RcptCmd_TypeThroughLine(void);
#endif//USE_DOUBLE_PRINT
extern void RcptCmd_TypeNone(void);

extern void RcptCmd_SetFontStyle(INT8U fontid, INT8U font_type, INT8U layer_type, INT8U inchsp, INT8U just);
extern void RcptCmd_BarHeight(INT16U fh, INT16U bh, INT16U ch);
extern void RcptCmd_BarWidth(INT8U w);
extern void RcptCmd_BarGuard(INT8U guard);

extern void RcptCmd_PutStringInit(char *dst);
extern void RcptCmd_PutString(INT8U position, char *dst, char *src, INT8U align);

// extern INT8U RcptCmd_PrintMultiLine(char *src, INT16U m_size, INT8U num_tab);
extern INT8U RcptCmd_PrintMultiLine(char *src, INT16U m_size, INT8U num_tab);
extern void RcptCmd_PrintLine(INT8U line_no, INT8U width);
extern void RcptCmd_PrintCharLine(char ch, INT8U length);
extern void RcptCmd_PrintChar(char ch);
extern void RcptCmd_PrintArabAllign(char *str, INT8U maxCapSize);
