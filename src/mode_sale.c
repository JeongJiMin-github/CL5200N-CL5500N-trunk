/*****************************************************************************
 |*  Copyright		:	(c) 2003 CAS
 |*  Filename		:	mode_sale.c
 |*  Version		:	0.1
 |*  Modified		:	2004/05/27
 |*  Modifier		:	CJK
 ****************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "globals.h"
#include "serial.h"
#include "flash.h"
#include "commbuf.h"
#include "prt_common.h"
#include "prtdrv.h"
#include "timer.h"
#include "serial.h"
#include "flash.h"
#include "sensor.h"
#include "ram.h"
#include "prt_render.h"
#include "bar_common.h"
#include "prt_font.h"
#include "dsp_common.h"
#include "prt_interpreter.h"
#include "dsp_vfd7.h"
#include "adm.h"
#include "adm_app.h"
#include "key.h"
#include "key_typedef.h"
#include "key_function.h"
#include "key_app.h"

#include "cal.h"
#include "prt_cmdtest.h"
#include "programmode.h"
// #include "file.h"
#include "rtc.h"
#include "debug.h"
#include "ethernet.h"
#include "main.h"
#include "initial.h"
#include "flash_app.h"
#include "common.h"

#include "mode_sale.h"
#include "mode_main.h"
#include "mode_commun.h"
#include "mode_error.h"
#include "mode_sale_display.h"
#include "mode_calibration.h"
#include "plu_manager.h"
#include "caption_app.h"
#include "dsp_app.h"
#include "rtc_app.h"
#include "plu_auxcall.h"
#include "discount.h"
#include "ethernet_comm.h"
#include "pgm_app.h"
#include "global_set.h"
#include "clerk_app.h"
#include "login.h"
#include "plu_edit.h"
#include "cashdrawer.h"
#include "network_command.h"
#include "prt_advertise.h"
#include "initial_glb.h"
#include "traceability.h"
#include "menu.h"
#include "plu_device.h"
//#include "common.h"
#include "network_command.h"
#include "prt_app.h"
#ifdef USE_CHN_CART_SCALE
#include "traceability_chn.h"
#endif
#include "barcode_file.h"
#include "department.h"
#ifdef USE_HYUNDAI_MEAT_FUNCTION
#include "shopname.h"
#endif
extern INT8U plu_table_search(unsigned char deptno,long pluno,INT16U *idx,INT8U mode);//Modify by se-hyung for Clerk Subtotal List 20070727
extern INT16S ClerkSearchVoidTransaction(INT16U dept, INT32U plu, INT16U clerkNum, INT8U deleteFlag, long *totalPrice); //Modify by se-hyung for Clerk Subtotal List 20070727
extern void MsgNoPriceClerk(INT16U clerk);			//Modify by se-hyung for Clerk Subtotal List 20070727
extern INT16S MiscNameCopy(char *p,INT32U	pluid);	      		//Modify by se-hyung for Clerk Subtotal List 20070727
extern NETWORK_CLERK_SALE_AMOUNT ClerkSaleAmount;		//Modify by se-hyung for Clerk Subtotal List 20070727
INT8U Clerk_Subtotal_List;					//Modify by se-hyung for Clerk Subtotal List 20070727
INT8U Pageupdown;						//Modify by se-hyung for Clerk Subtotal List 20070727
INT8U Display_flag=0;
INT8U SpeedKeyPressed; //csh 20140304
INT8U Disp_refresh_flag;
INT8U Prt_labelCap_flag;
INT8U Prt_weight_Symbol_flag;
INT8U Prt_uprice_Symbol_flag;
INT8U RealtimeAddDataFlag;
INT8U PrtBarcodePriceAddExTaxFlag;
#ifdef USE_EMART_DEFAULT
INT8U PrtName4_With_slaughthouse;
#endif
#ifdef USE_TEST_AUTO_LABEL_PRINT
INT8U PowerStartFlag = 1;
#endif
INT8U Prt_under_MinWeight;
//void sale_display_proc(INT8U onlyWeighing);
void sale_key_proc(void);
void sale_calcprice(void);
//extern INT16S DisplayMsg(char *msg);
INT16S FontSizeChange(INT16S fontSize);
extern INT8U	OnOff(INT8U	tmp);
extern INT8U Selfkey_flag;
extern void Key_DelaySec(INT16S delay,INT16U preTime);
extern void PrintLabelWait(void);
extern void ProcessAll(void);
//extern void LoadWeighingInfo(void);
extern void LoginClerk(INT16U clerkid);
extern void sale_display_price(void);
extern INT16S  LoadDefaultCurrency(void);
extern void sale_display_discount(INT16S clearFlag);
//extern INT16S	SaleAcctLocalAddTransaction(INT8U dept_id,INT32U plu_id,PRICE_TYPE *pPrice,INT8U saleType);
extern void display_message_page(char *str);

DISC_SALE_TYPE		DiscSale;
PRICE_TYPE		Price;
PRICE_TYPE		LinkedPrice[MAX_LINKED_PLU];
PRICE_TYPE  		PriceFSP; //SG061214. for FSP Backup Price
PRICE_TYPE  	MemberPrice;
//SALE_OPER_TYPE		Operation;
NEARDATA SALE_OPER_TYPE		Operation;//HYP 20060412 _huge --> _near for small code size
INT8U	PriorityGlobalDiscount;
extern CAPTION_STRUCT cap;

extern void serial_realtime_check(void); //SG070917
extern void network_scalecheck_send(void);
extern char ClerkClearTransaction(void);
#if defined(USE_INDIA_FUNCTION) || defined(USE_SEND_TICKET_DATA) || defined(USE_SETPRODUCT_SALE)
extern void ClearAdditionalData(void);
#endif
extern INT16S  ClearSalesSummaries(INT16S counter);
extern void Flash_flush_nvbuffer_c2(void);
extern void Cash_close();
INT8U	PrintWaitFlag=0;
#ifdef USE_EMART_PRODUCTION_KEY
extern INT8U BackupCheckin;
#endif
extern INT8U StartPrintFlag;
extern char gmsg[];
extern INT16U TimeOpenCashDrawer,ClerkLoginTime;
//extern INT16U	MsgDisplayTime;
extern TRANSACTION_PAY transPay;
extern PLU_SCROLL_STRUCT PLU_SCROLL;
extern INT8U barcodetype_codevalue[];
SALE_G_PRICE_TYPE	GlbPrice;
SALE_G_OPER_TYPE	GlbOper;
SALE_G_FLOAT_TYPE	GlbFloat;
SALE_G_ADD_TYPE		GlbAddup;
SALE_G_PRINT_TYPE	GlbPrint;
TARE_OPER_TYPE		TareOper;
PRINT_OPER_TYPE		PrtOper;	// Added by CJK 20051226
SALE_G_HOLD_FUNC	GlbHold;
#ifdef USE_ZERO_RANGE_MSG
extern INT8U Zerokey_input_Flag;
#endif

PLU_BATCH_TYPE	Batch[MAX_PLU_BATCH];
INT8U	BatchCount,BatchIndex;
long MaxCapaWeight;
INT8U 	PercentTareOver = OFF;
INT8U CallQuickPlu = OFF;
INT8U ReadyQuickPlu = OFF;
INT16U QuickPluKeyTime = 0;
INT8U InputQuickPluState = OFF;
INT8U GlbFlagPCnMasterConnection = ON;
INT16U AutoFlushCache2Time =0;
INT16U AutoFlushNvramTime =0;
INT16U AutoFlushNvramTime2 =0;
INT8U SaleWeightChk = OFF;
INT8U UseEmartNetwork = OFF;	// parameter 723 : 매가변경 전송기능
#ifdef USE_TRACE_STANDALONE
INT8U UseCheckIndivNo = OFF;
#endif
INT8U	PrvPrtPluId = 0;
#ifdef USE_INDIA_FUNCTION
INT8U UseAreaScaleMode = 0; // 0:NormalScale(STD), 1:DualModeScale, 2:AreaScale
INT8U AllowNegativeInventory = OFF;
#endif
#if defined(USE_NHMART_SAFE_MEAT) || defined(USE_CHECK_INDIV_INFO)
INT8U checkMtrace;
#endif
#ifdef USE_DISPLAY_COMM_SPEED_SETTING
INT8U DispRefreshIntvl;
#endif

#ifdef USE_HYUNDAI_MEAT_FUNCTION
INT8U HyundaiMeatFlag = OFF;
INT8U ReceiptData[31+1];	// > scanner format len
INT8U BarcodeData[30+1];	  // > Barcode format len
  #ifdef USE_HYUNDAI_SET_FUNCTION
INT8U MeatSetSaleMode = OFF;
INT8U MeatSetSaleUseScanner = OFF;
INT8U MeatSetLabelTransFlag = OFF;
INT32U MeatSetPluId;
INT8U MeatSetCompNum = 0;
long MeatSetPrice = 0;
  #endif
#endif

#ifdef USE_TRACE_STANDALONE
extern void keyapp_direct_individual(INT16U skeyIndivID);
INT8U	FlagDisplayIndividual = OFF;
INT8U	FlagDisplaySpecialUse = OFF;
INT8U	FlagDisplaySlaughtHouse = OFF;
#ifdef USE_TRACE_MEATPART
INT8U	FlagDisplayMeatPart = OFF;
#endif
#ifdef USE_TRACE_MEATGRADE
INT8U	FlagDisplayMeatGrade = OFF;
#endif
INT16U  IndividualDisplayTime = 0;
INT8U   IndividualDisplayTimeFlag = OFF;
#ifdef USE_TEST_AUTO_LABEL_PRINT
INT8U PowerStartFlag = ON;
#endif
extern void keyapp_specialuse(INT16U skeyIndivID);
#endif
extern void	InitSystem(void);
extern void KEY_Set_Shift(INT8U onoff);
extern void KEY_Set_Caps(void);
extern void sale_send_weight(void);
extern void keyapp_cleardiscount(void);
#ifdef USE_NHMART_SAFE_MEAT
void SaveCalledIndiv(void);
#endif
#ifdef USE_CHECK_INDIV_INFO
extern INT8U CheckIndivInfo(void);
#endif
extern INT8U GlbBarcodeData_Field1[BARCODE_FORMAT_LEN+1 ];
#ifdef USE_CTS_FUNCTION
void sale_display_cts_msg(void);
#endif
#ifdef USE_REGIST_DISPOSAL
void sale_display_disposal(INT8U mode);
#endif
#ifdef USE_PERSIAN_SALE_AMOUNT_LIMIT
extern INT8U sale_check_sale_total_amount(long total_price);
#endif
void mode_sale_init(void)
{
	INT8U updateLog;
	memset((INT8U*)&status_scale,0,sizeof(status_scale));	// clear all zero

	//	InitSystem();
	global_load_parameter(1);
	status_scale.cur_weightunit=get_defaultunit(CurPluWeightUnit);
	DebugTime.Begin = 0;
	DebugTime.Mid = 0;
	DebugTime.End = 0;
	DebugTime.Diff1 = 0;
	DebugTime.Diff2 = 0;
	DebugTime.Flag = 0;
	DebugTime.Enable = 0;

	//no setting
	TareOper.tarePLU	    	= 0;
	TareOper.tareKey	    	= 0;
	TareOper.tareWeight	    	= 0;
	TareOper.tareClerk		= 0;	// CJK080322
	TareOper.flagAutoClearTare      = OFF;
	TareOper.addupWeight    = 0;
	TareOper.saveKeyTare = 0;
	LoadWeighingInfo();
    if ((status_scale.cur_speedkeyid<0) || (status_scale.cur_speedkeyid>5)) status_scale.cur_speedkeyid=1;
	status_scale.flag_input     = get_global_bparam(GLOBAL_SALE_AUTO_STATE) & 0xe0;
	//status_scale.code00         = keyin_getrawcode(KEY_NUM_0);
	status_scale.code00         = keyin_getrawcode_bytype(0, KEY_NUM_0);	// Modified by CJK 20060315
	status_scale.auto_count     = 0;
	status_scale.departmentid   = get_net_wparam(NETWORK_DEPARTMENT_ID);
	status_scale.clerkid        = get_net_wparam(NETWORK_CLERK_ID);
	status_scale.scaleid        = get_net_wparam(NETWORK_SCALE_ID);
	if ((status_scale.departmentid<=0) || (status_scale.departmentid>99)) {
        status_scale.departmentid  =99;
		status_scale.cur_speedkeyid= 1;
		keyapp_call_department(status_scale.departmentid,1);
	}
	else
		set_department_speedkey(status_scale.departmentid);
	if ((status_scale.clerkid<0) || (status_scale.clerkid>99)) {
		status_scale.clerkid=0;
	}

	status_scale.key_last_used_100ms = SysTimer100ms;
	status_scale.ad_last_used_100ms = SysTimer100ms;

	//	status_scale.clerkid=0;
	KeyDrv.FirstKey = 1;
	KeyDrv.Shifted = 0;

	Operation.systemOnlyStart = OFF;
	GlbFlagPCnMasterConnection = ON;
	//KEY_Set_Shift(0);

	memset(GlbBarcodeData_Field1, 0, sizeof(GlbBarcodeData_Field1));
#ifdef USE_HYUNDAI_MEAT_FUNCTION
	memset(ReceiptData, 0x00, sizeof(ReceiptData));
	memset(BarcodeData, 0x00, sizeof(BarcodeData));
#endif
#ifdef USE_KOR_TRACE
	korTraceMemberCodeInit();
#endif

#ifdef USE_KOR_SLAUGHT_CODE_4DIGIT
	trace_slaughthousedata_init();
#endif
#ifdef USE_INGREDIENT_CODE_4DIGIT
	ingredient_init();
#endif
#ifdef USE_TRACE_MEATGRADE
	trace_meatgrade_getGradeListText();
#endif
#if defined(USE_SETPRODUCT_SALE) || defined(USE_HYUNDAI_SET_FUNCTION)
	ClearAdditionalData();
	ClerkDeleteTransaction(status_scale.clerkid);
#endif
#ifdef USE_DISPLAY_COMM_SPEED_SETTING
	Set_SPI_ID1_LCD_Comm_Speed();
#endif

// init_flash_check()로 이동
	//	updateLog = get_global_bparam(FLASH_FW_UPDATE_VER);
	//	if((updateLog < 10) || (updateLog == 0xff))
	//	{
	//		set_global_bparam(FLASH_FW_UPDATE_VER, 10);
	//	}
}

//SaleMainState
//#define		SALE_PROC_START			0
//#define		SALE_PROC_KEY			1
//#define		SALE_PROC_ADM			2
//#define		SALE_PROC_CALC_PRICE		3
//#define		SALE_PROC_ETH_RCV		4
//#define		SALE_PROC_DISPLAY		5

//status_scale.flag_display
#define		SALE_FLAG_DISP_UPDATE		0x01
#define		SALE_FLAG_DISP_REQ_PRINT	0x02
#define		SALE_FLAG_DISP_ERR_MSG_CLR	0x04

//status_scale.print_status
//B0	출력중
//B1	Hanging Form
//B2	Out of Paper
//B3	Headup
//B4	Gab Error
//B5	출력초기 시작시 AD 안정상태에서 출력 할것
//B6	출력 완료 후 Auto Clear Mode 실행
//B7	출력완료

//status_scale.flag_input
//B0	[Not Used] 1=Multiple, 0= Normal (status_scale.sale_value, status_scale.sale_type)
//B1	0=PLU AND Default Status, 1=BY WEIGHT, 2=BY COUNT, 3=NONFOOD = same to PLU Type
//B2
//B3
//B4
//B5	Auto
//B6	Save
//B7	Prepack

//INT16S	ClearNullPlu(void)
//{
//	return 1;
//}

//INT16S	SaleModeChange(void)
//{
//	return 1;
//}

//void ClearAllScreen(void)
//{
//	display_alloff_primary_indication();
//	VFD7_Diffuse();
//	Dsp_Fill_Buffer(0);
//	Dsp_Diffuse();
//
//	//sale_display_update(0x0fff);
//	//Operation.modeScreenType = 0;
//}

//INT16S SaleChangeMode(INT8U mode)
//{
//	if(Operation.modeSaleType == mode)	return 0;
//
//	Operation.modeSalePrevType = Operation.modeSaleType;
//	Operation.modeSaleType = mode;
//	//Operation.modeScreenType = 0;	// 전체 화면 클리어 해야 함, 그리고 다시 디스플레이 한 후 세팅
//	//ClearAllScreen();
//
//	return 1;
//}

INT8U	CurrFontSize=0;
INT16S FontSizeChange(INT16S fontSize)
{
	//if(CurrFontSize == fontSize)	return 0;
	if(fontSize == FONT_SMALL)
		DspLoadFont1(DSP_SMALL_FONT_ID);
	else if(fontSize == FONT_MEDIUM)
		DspLoadFont1(DSP_MEDIUM_FONT_ID);
	else	//FONT_LARGE
#ifdef USE_CHN_FONT
		DspLoadFont1(DSP_MEDIUM_FONT_ID);
#else
		DspLoadFont1(DSP_LARGE_FONT_ID);
#endif
	CurrFontSize=fontSize;

	return 0;
}

INT16S	PutString(INT16S y, INT16S x, char *data, char font_id, char magy, char magx, INT16S n)
{
	POINT disp_p;
	char buf[100];
	char clearBuf[61];
	char old;
	INT8U backup_directdraw;

	old=DspStruct.Id1;
	if (old != font_id) DspLoadFont1(font_id);
	Dsp_FontSize(magx, magy);
	strcpy(buf, data);
	strset(clearBuf, 0x20, 60);
	strcat(buf, clearBuf);
	*(buf+n)=0;

	disp_p.x = x;// 가로
	disp_p.y = y;// 세로
	//DEL071204	disp_p.x += PutXStringOffsetX;
	//DEL071204	disp_p.y += PutXStringOffsetY;
	backup_directdraw = DspStruct.DirectDraw;
	DspStruct.DirectDraw = 1;
#ifdef HEBREW_FONT
	Convert_HebrewPreview((INT8U *)buf, strlen(buf), 0, 0);
	hebrew_codetemp[strlen(buf)] = 0;
	disp_p.x = 202 - disp_p.x;
	Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
	Convert_ArabPreview((INT8U *)buf, strlen(buf), 0, 0, 0);
	arab_codetemp[strlen(buf)] = 0;
	disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
	Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
	Dsp_Write_String(disp_p, (HUGEDATA char *)buf);
#endif
	DspStruct.DirectDraw = backup_directdraw;
	if (old != font_id) DspLoadFont1(old);
	return 0;
}

INT16S	PutStringIndirect(INT16S y, INT16S x, char *data, char font_id, char magy, char magx, INT16S n)
{
	POINT disp_p;
	char buf[100];
	char clearBuf[61];
	char old;
	INT8U backup_directdraw;

	old=DspStruct.Id1;
	if (old != font_id) DspLoadFont1(font_id);
	Dsp_FontSize(magx, magy);
	strcpy(buf, data);
	strset(clearBuf, 0x20, 60);
	strcat(buf, clearBuf);
	*(buf+n)=0;

	disp_p.x = x;// 가로
	disp_p.y = y;// 세로
	//DEL071204	disp_p.x += PutXStringOffsetX;
	//DEL071204	disp_p.y += PutXStringOffsetY;
	//backup_directdraw = DspStruct.DirectDraw;
	//DspStruct.DirectDraw = 1;
#ifdef HEBREW_FONT
	Convert_HebrewPreview((INT8U *)buf, strlen(buf), 0, 0);
	hebrew_codetemp[strlen(buf)] = 0;
	disp_p.x = 202 - disp_p.x;
	Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
	Convert_ArabPreview((INT8U *)buf, strlen(buf), 0, 0, 0);
	arab_codetemp[strlen(buf)] = 0;
	disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
	Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
	Dsp_Write_String(disp_p, (HUGEDATA char *)buf);
#endif
	//DspStruct.DirectDraw = backup_directdraw;
	if (old != font_id) DspLoadFont1(old);
	return 0;
}

INT16U PutXStringOffsetX = 0;
INT16U PutXStringOffsetY = 0;

INT16S	PutSmallString(INT16S y, INT16S x,char *data)
{
	POINT disp_p;
	INT16S old_id;
	INT8U backup_directdraw;
	//	FontSizeChange(FONT_SMALL);
	old_id = DspStruct.Id1;
	if (old_id != DSP_SMALL_FONT_ID) DspLoadFont1(DSP_SMALL_FONT_ID);

	disp_p.x = x * display_font_get_width();// 가로
	disp_p.y = y * display_font_get_height();// 세로

	disp_p.x += PutXStringOffsetX;
	disp_p.y += PutXStringOffsetY;
	backup_directdraw = DspStruct.DirectDraw;
	DspStruct.DirectDraw = 1;
#ifdef HEBREW_FONT
	Convert_HebrewPreview((INT8U *)data, strlen(data), 0, 0);
	hebrew_codetemp[strlen(data)] = 0;
	disp_p.x = 202 - disp_p.x;
	Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
	Convert_ArabPreview((INT8U *)data, strlen(data), 0, 0, 0);
	arab_codetemp[strlen(data)] = 0;
	disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
	Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
	Dsp_Write_String(disp_p,(HUGEDATA char *)data);
#endif
	DspStruct.DirectDraw = backup_directdraw;
	//Dsp_Diffuse();

	if (old_id != DSP_SMALL_FONT_ID) DspLoadFont1(old_id);

	return 0;
}

//INT16S 	ClearString(char *p,INT16U length)
//{
//	INT16U	i;
//	for(i=0;i<length;i++,p++) *p=' ';
//	*p=0;
//	return 0;
//}

INT16S	PutNSmallString(INT16S y, INT16S x,char *data, INT16S n)
{
	POINT disp_p;
	char buf[100];
	char clearBuf[31];
	char old;
	INT8U backup_directdraw;


	old=DspStruct.Id1;
	if (old != DSP_SMALL_FONT_ID) DspLoadFont1(DSP_SMALL_FONT_ID);
	strcpy(buf,data);
	strset(clearBuf,0x20,30);
	strcat(buf,clearBuf);
	*(buf+n)=0;

	disp_p.x = x * BASE_FONT_WIDTH;// 가로
	disp_p.y = y * BASE_FONT_HEIGHT;// 세로

	disp_p.x += PutXStringOffsetX;
	disp_p.y += PutXStringOffsetY;
	backup_directdraw = DspStruct.DirectDraw;
	DspStruct.DirectDraw = 1;
#ifdef HEBREW_FONT
	Convert_HebrewPreview((INT8U *)buf, strlen(buf), 0, 0);
	hebrew_codetemp[strlen(buf)] = 0;
	disp_p.x = 202 - disp_p.x;	//(Display Font: 6x8)  <= 208 - 6
	Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
	Convert_ArabPreview((INT8U *)buf, strlen(buf), 0, 0, 0);
	arab_codetemp[strlen(buf)] = 0;
	disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
	Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
	Dsp_Write_String(disp_p,(HUGEDATA char *)buf);
#endif
	DspStruct.DirectDraw = backup_directdraw;
	//Dsp_Diffuse();
	if (old != DSP_SMALL_FONT_ID) DspLoadFont1(old);
	return 0;
}

INT16S	PutNMediumString(INT16S y, INT16S x,char *data, INT16S n)
{
	POINT disp_p;
	char buf[100];
	char clearBuf[31];
	INT8U backup_directdraw;
	//INT16S i;

	FontSizeChange(FONT_MEDIUM);
	strcpy(buf,data);
	strset(clearBuf,0x20,30);
	strcat(buf,clearBuf);
	*(buf+n)=0;

	//	disp_p.x = x * display_font_get_width();// 가로
	//	disp_p.y = y * display_font_get_height();// 세로
	disp_p.x = x * BASE_FONT_WIDTH;// 가로
	disp_p.y = y * BASE_FONT_HEIGHT;// 세로
	disp_p.x += PutXStringOffsetX;
	disp_p.y += PutXStringOffsetY;
	backup_directdraw = DspStruct.DirectDraw;
	DspStruct.DirectDraw = 1;
#ifdef HEBREW_FONT
	Convert_HebrewPreview((INT8U *)buf, strlen(buf), 0, 0);
	hebrew_codetemp[strlen(buf)] = 0;
	disp_p.x = 200 - disp_p.x;	//(Display Font: 8x16)  <= 208 - 8
	Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
	Convert_ArabPreview((INT8U *)buf, strlen(buf), 0, 0, 0);
	arab_codetemp[strlen(buf)] = 0;
	disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
	Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
	Dsp_Write_String(disp_p,(HUGEDATA char *)buf);
#endif
	DspStruct.DirectDraw = backup_directdraw;
	//Dsp_Diffuse();

	return 0;
}

INT16S	PutNLargeString(INT16S y, INT16S x,char *data, INT16S n)
{
	POINT disp_p;
	char buf[100];
	char clearBuf[31];
	INT8U backup_directdraw;
	//INT16S i;

	FontSizeChange(FONT_LARGE);
	strcpy(buf,data);
	strset(clearBuf,0x20,30);
	strcat(buf,clearBuf);
	*(buf+n)=0;

	//	disp_p.x = x * display_font_get_width();// 가로
	//	disp_p.y = y * display_font_get_height();// 세로
	disp_p.x = x * BASE_FONT_WIDTH;// 가로
	disp_p.y = y * BASE_FONT_HEIGHT;// 세로
	disp_p.x += PutXStringOffsetX;
	disp_p.y += PutXStringOffsetY;
	backup_directdraw = DspStruct.DirectDraw;
	DspStruct.DirectDraw = 1;
#ifdef HEBREW_FONT
	Convert_HebrewPreview((INT8U *)buf, strlen(buf), 0, 0);
	hebrew_codetemp[strlen(buf)] = 0;
	disp_p.x = 202 - disp_p.x;
	Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
	Convert_ArabPreview((INT8U *)buf, strlen(buf), 0, 0, 0);
	arab_codetemp[strlen(buf)] = 0;
	disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
	Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
	Dsp_Write_String(disp_p,(HUGEDATA char *)buf);
#endif
	DspStruct.DirectDraw = backup_directdraw;
	//Dsp_Diffuse();

	return 0;
}

long GetMaxTotalPrice(void)
{
	INT16S n;
	long max_total;

	n=(INT16S)get_global_bparam(GLOBAL_DISPLAY_PRICELENGTH);
#ifdef USE_PRICE_8DIGIT
	n += 2; //gm 081103, 1:vietnam
#endif
	max_total=power10(n);
	return max_total;
}

long GetMaxUnitPrice(void)
{
	INT16S n;
	long max_total;
	n=(INT16S)get_global_bparam(GLOBAL_DISPLAY_UNITPRICELENGTH);
	max_total=power10(n);
	return max_total;
}

void	CheckCashClose(void)
{
	if (Operation.openCash)
	{
		//if((INT16U)TimeOpenCashDrawer < (INT16U)(SysTimer100ms-get_global_wparam(GLOBAL_TIME_CASHDRAWEROPEN)))
		if((INT16U)TimeOpenCashDrawer < (INT16U)SysTimer100ms)
		{
			//DisplayMsg("Close cash drawer");
			Cash_close();
			Operation.openCash =OFF;
			TimeOpenCashDrawer = 0;
		}
	}
}

void CheckDisplayMsg(void)
{
	//	char	tBuf[30];
	INT8U	msgClearTimes;
	INT16S	clearFlag;

	// load msg delay time setting
	msgClearTimes=get_global_bparam(GLOBAL_ERROR_TIMEOUT);
	if(MsgDisplayTime)
	{
		if (MsgShort)
			clearFlag = CheckTimeOver(msgClearTimes/2, MsgDisplayTime);
		else
			clearFlag = CheckTimeOver(msgClearTimes, MsgDisplayTime);
		if (clearFlag)
		{
			MsgDisplayTime = 0;
			sale_display_update(UPDATE_TITLE);
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
			if(MsgWeight)	//Added by JJANG 20080401, parameter 568
			{
				MsgWeight = OFF;
				sale_display_update(UPDATE_TITLE | UPDATE_DATETIME | UPDATE_RIDE);
			}
#endif
		}
	}
#if !defined(_USE_LCD20848_) && !defined(_USE_LCD32_)
	if (Msg7SegDispTime)
	{
		msgClearTimes = get_global_wparam(GLOBAL_TIME_CLERKLOGOUT);
		clearFlag = CheckTimeOver(msgClearTimes, Msg7SegDispTime);
		if (clearFlag)
		{
			Msg7SegDispTime = 0;
			sale_display_update(UPDATE_WEIGHT | UPDATE_UNIT | UPDATE_TARE | UPDATE_PRICE);
		}
	}
	if (DateTimeDispTime)
	{
		clearFlag =CheckTimeOver(40, DateTimeDispTime);
		if (clearFlag)
		{
			DateTimeDispTime = 0;
			//display_lcd_clear();
			sale_display_update(UPDATE_CENTER);
		}
	}
#else
	msgClearTimes=get_global_bparam(GLOBAL_KEY_TIMEOUT_ERROR);		//PARAMETER 886 시간을 따라감.
	if(MsgRDisplayTime)
	{
		clearFlag = CheckTimeOver(msgClearTimes, MsgRDisplayTime);
		if (clearFlag)
		{
			MsgRDisplayTime = 0;
			sale_display_update(UPDATE_TITLE2);
		}
	}
	msgClearTimes=get_global_bparam(GLOBAL_ERROR_TIMEOUT);		//PARAMETER 535 시간을 따라감.
	if(MsgPLUDisplayTime)
	{
		clearFlag = CheckTimeOver(msgClearTimes, MsgPLUDisplayTime);
		if (clearFlag)
		{
			MsgPLUDisplayTime = 0;
			sale_display_update(UPDATE_CENTER);
		}
	}
#endif
}

//extern INT16U	DoublePLUkeyTime;
void CheckTimes(void)
{
#ifdef USE_SET_QUICK_PLU_DELAY_TIME
	INT8U delayQuickPLU;
	INT8U delayTime;

	delayQuickPLU = get_global_bparam(GLOBAL_SALE_SETUP_FLAG19);
	delayQuickPLU = (delayQuickPLU>>4)& 0x07;	//param 527, barcode line limit
	
	if(delayQuickPLU) 
	{
		delayTime = 5*delayQuickPLU;	// 5 x Set Value (5 x "1" => 500ms)
	}
	else delayTime = 2;	// 0: 200ms
#endif
	if(ClerkLoginTime && CheckTimeOver(get_global_wparam(GLOBAL_TIME_CLERKLOGOUT),ClerkLoginTime))// OPTION: 10초후 log off
	{
		//new
		// && !status_scale.cur_pluid)// prepack에서는 log off 하지 않음
		if(Operation.wPrepack||Operation.wAuto){}
		else
		{
			if(Operation.salePrintWait || status_scale.print_multiple){}
			else
				logout();
		}
	}
	//	if(Operation.doublePLUkey && DoublePLUkeyTime)
	//	{
	//		//if(CheckTimeOver(20,DoublePLUkeyTime))	DoublePLUkeyTime = 0;
	//		if(CheckTimeOver(8,DoublePLUkeyTime))	DoublePLUkeyTime = 0;   //SG060222
	//
	//	}

#ifdef USE_DIRECT_INGREDIENT//DEL080826#if (ROM_COUNTRY == ROM_RUS) //SG070827
	if(FlashNVRAMCache2.flag && AutoFlushCache2Time )
	{
		if(CheckTimeOver(20, AutoFlushCache2Time))
		{
			Flash_flush_nvbuffer_c2();
			AutoFlushCache2Time = 0;
		}
	}
#endif

#ifdef USE_TRACE_STANDALONE
	if(IndividualDisplayTimeFlag)
	{
		if(CheckTimeOver(80,IndividualDisplayTime)){
			sale_display_indivFlag_set(INDIV_DISP_MODE_ALL, OFF);
			//sale_display_individual();
			sale_display_update(UPDATE_TITLE);	// CJK070705
			IndividualDisplayTime = 0;
			IndividualDisplayTimeFlag = OFF;
		}
	}
#endif
	if (InputQuickPluState && ReadyQuickPlu)
	{
#ifdef USE_SET_QUICK_PLU_DELAY_TIME
		if (CheckTimeOver(delayTime, QuickPluKeyTime))
#else
		if (CheckTimeOver(2, QuickPluKeyTime))
#endif
		{
			CallQuickPlu = ON;
		}
	}

	/*
	 if(MsgDisplayTime)
	 {
	 char	tBuf[30];
	 INT8U	msgClearTimes;

	 // load msg delay time setting
	 msgClearTimes=get_global_bparam(GLOBAL_ERROR_TIMEOUT);
	 clearFlag = OFF;
	 if(MsgShort)
	 clearFlag = CheckTimeOver(msgClearTimes/2,MsgDisplayTime);
	 else
	 clearFlag = CheckTimeOver(msgClearTimes,MsgDisplayTime);
	 if(clearFlag)
	 {
	 //TRANSACT_CHAIN  *cp;
	 // message clear
	 //cp = (TRANSACT_CHAIN *)get_nvram_lparam(NVRAM_TRANS_HEAD);
	 //if(!cp)
	 {
	 strset(tBuf,0x20,MAX_MSG_CHAR);
	 PutNSmallString(0,0,tBuf,MAX_MSG_CHAR);
	 MsgDisplayTime=0;
	 }
	 }
	 }
	 */
	CheckCashClose();
}

#ifdef USE_PB_BATTERY

#define BAT_FULL	1
#define BAT_CHARGING_LOW	2
#define BAT_CHARGING_MID	3

#define BAT_HIGH	11
#define BAT_MID		12
#define BAT_LOW		13
#define BAT_LOW_BLINK	14
#define BAT_POWER_OFF	15

#define BAT_NOT_AVAILABLE 21

//#define CHARGE_FULL_THRESHOLD	745//26.4V*(10kohm/(100kohm+10kohm))*1024/3.3V
#define CHARGE_FULL_THRESHOLD	775//27.5V*(10kohm/(100kohm+10kohm))*1024/3.3V
#define CHARGE_MID_THRESHOLD	733//26.0V/11*1024/3.3V
#define MID_BAT_THRESHOLD	691//24.5V/11*1024/3.3V
#define LOW_BAT_THRESHOLD	677//24.0V/11*1024/3.3V
#define LOW_BAT_BLINK_THRESHOLD	669//23.7V/11*1024/3.3V
#define POWER_OFF_THRESHOLD	663//23.5V/11*1024/3.3V
#define NO_BAT_THRESHOLD	564//20.0V/11*1024/3.3V

INT8U BatterySign1 = OFF;
INT8U BatterySign2 = OFF;
INT8U BatterySign3 = OFF;
extern INT16U BatVoltVal;

INT8U GetBatteryMode(void)
{
	INT8U tempModeBattery;

	if (AC_CHECK)	// power in (충전중)
	{
		//tempModeBattery = BAT_CHARGING;
		if (BatVoltVal > CHARGE_FULL_THRESHOLD)
		{
			tempModeBattery = BAT_FULL;
		}
		else if (BatVoltVal > CHARGE_MID_THRESHOLD)
		{
			tempModeBattery = BAT_CHARGING_MID;
		}
		else if (BatVoltVal <= NO_BAT_THRESHOLD)
		{
			tempModeBattery = BAT_NOT_AVAILABLE;
		}
		else
		{
			tempModeBattery = BAT_CHARGING_LOW;
		}
	}
	else
	{
		if (BatVoltVal > MID_BAT_THRESHOLD) // 정상
		{
			tempModeBattery = BAT_HIGH;
		}
		else if (BatVoltVal > LOW_BAT_THRESHOLD) // Mid Battery
		{
			tempModeBattery = BAT_MID;
		}
		else if (BatVoltVal > LOW_BAT_BLINK_THRESHOLD) // Low Battery
		{
			tempModeBattery = BAT_LOW;
		}
		else if (BatVoltVal > POWER_OFF_THRESHOLD) // Low Battery Blink
		{
			tempModeBattery = BAT_LOW_BLINK;
		}
		else if (BatVoltVal <= NO_BAT_THRESHOLD)
		{
			tempModeBattery = BAT_NOT_AVAILABLE;
		}
		else // Power OFF
		{
			tempModeBattery = BAT_POWER_OFF;
		}
	}
	return tempModeBattery;

}


// 200ms마다 체크
void CheckBattery(void)
{
	static INT8U modeBattery;
	INT8U tempModeBattery;
	static INT8U prev_modeBattery;
	static INT8U charge_state, lowbat_state, charge_mid_state;
	static INT8U cntModeChange = 0;
	POINT disp_p;
	char old_font;
	INT8U old_page;
	INT8U string_buf[33];
	INT16U len;
	INT16U x, y;
	static INT16U prevTimer100ms=0;
	static INT16U cnt = 0;


	//if (network_timeout_check_10ms(70, NETWORK_TIMEOUT_BAT_SIGN))
	{
		tempModeBattery = GetBatteryMode();
		if (tempModeBattery != prev_modeBattery)
		{
			cntModeChange = 0;
			prev_modeBattery = tempModeBattery;
		}
		if (++cntModeChange >= 10)
		{
			//if (modeBattery != tempModeBattery)
			//{
			//	BuzOn(3);BuzOn(3);BuzOn(3);
			//}
			//if (cntModeChange == 5) BuzOn(2);	// mode 바뀔 때, buzzer 울리게 함
			cntModeChange = 10;
			modeBattery = tempModeBattery;
		}
	}

	if (++cnt >= 3) // display period : 600ms
	{
		cnt = 0;
		switch (modeBattery)
		{
			case BAT_CHARGING_LOW:
				//if (network_timeout_check_10ms(70, NETWORK_TIMEOUT_BAT_SIGN))
			{
				switch (charge_state)
				{
					default:
					case 0:
						BatterySign1 = ON; // 테두리
						BatterySign2 = OFF; // high
						BatterySign3 = OFF; // low
						charge_state = 1;
						break;
					case 1:
						BatterySign1 = ON;
						BatterySign2 = OFF;
						BatterySign3 = ON;
						charge_state = 2;
						break;
					case 2:
						BatterySign1 = ON;
						BatterySign2 = ON;
						BatterySign3 = ON;
						charge_state = 0;
						break;
				}
				sale_display_update(UPDATE_MODE);
			}
			//charge_state = 0;
			lowbat_state = 0;
			break;
			case BAT_CHARGING_MID:
				switch (charge_mid_state)
				{
					default:
					case 0:
						BatterySign1 = ON; // 테두리
						BatterySign2 = OFF; // high
						BatterySign3 = ON; // low
						charge_mid_state = 1;
						break;
					case 1:
						BatterySign1 = ON;
						BatterySign2 = ON;
						BatterySign3 = ON;
						charge_mid_state = 0;
						break;
				}
				sale_display_update(UPDATE_MODE);
				//charge_state = 0;
				//lowbat_state = 0;
				break;
			case BAT_FULL:
				switch (charge_state)
				{
					default:
					case 0:
						BatterySign1 = ON;
						BatterySign2 = ON;
						BatterySign3 = ON;
						charge_state = 1;
						break;
					case 1:
						BatterySign1 = OFF;
						BatterySign2 = ON;
						BatterySign3 = ON;
						charge_state = 0;
						break;
				}
				sale_display_update(UPDATE_MODE);
				//lowbat_state = 0;
				break;
			case BAT_HIGH:
				BatterySign1 = ON;
				BatterySign2 = ON;
				BatterySign3 = ON;
				sale_display_update(UPDATE_MODE);
				//charge_state = 0;
				//lowbat_state = 0;
				break;
			case BAT_MID:
				BatterySign1 = ON;
				BatterySign2 = OFF; // high
				BatterySign3 = ON;  // low
				sale_display_update(UPDATE_MODE);
				//charge_state = 0;
				//lowbat_state = 0;
				break;
			case BAT_LOW:
				BatterySign1 = ON;
				BatterySign2 = OFF;
				BatterySign3 = OFF;
				sale_display_update(UPDATE_MODE);
				//charge_state = 0;
				//lowbat_state = 0;
				break;
			case BAT_LOW_BLINK:
				//if (network_timeout_check_10ms(70, NETWORK_TIMEOUT_BAT_SIGN))
			{
				switch (lowbat_state)
				{
					default:
					case 0:
						BatterySign1 = ON;
						BatterySign2 = OFF;
						BatterySign3 = OFF;
						lowbat_state = 1;
						break;
					case 1:
						BatterySign1 = OFF;
						BatterySign2 = OFF;
						BatterySign3 = OFF;
						lowbat_state = 0;
						break;
				}
				sale_display_update(UPDATE_MODE);
			}
			//charge_state = 0;
			//lowbat_state = 0;
			break;
			case BAT_POWER_OFF:
				old_page = DspStruct.Page;
				old_font = DspStruct.Id1;
				Dsp_SetPage(DSP_ERROR_PAGE);
				DspLoadFont1(DSP_MENU_FONT_ID);

				Dsp_Fill_Buffer(0);
				sprintf((char*)string_buf, "LOW BATTERY");
				len = strlen((char*)string_buf);
				x = (LCD_Y_MARGIN-display_font_get_width()*len)/2;
				y = (LCD_X_MARGIN-display_font_get_height())/2;
				disp_p = point(y, x);
				Dsp_Write_String(disp_p, (char*)string_buf);
				Dsp_Diffuse();

				display_alloff_primary_indication();
				VFD7_Diffuse();
				BatterySign1 = ON;
				BatterySign2 = OFF;
				BatterySign3 = OFF;
				sale_display_update_clear();
				sale_display_update(UPDATE_MODE);
				sale_display_sign();
				VFD7_Diffuse();
				BuzOn(3);
				while (1)
				{
					//if (KEY_IsKey()) break;	// 임시
					if (AC_CHECK)
					{
						//modeBattery = BAT_NORMAL;
						//cntModeChange = 7;
						_SOFTWARE_RESET;
						break;
					}
					//if (BatVoltVal > POWER_OFF_THRESHOLD) break;

					//					if ((INT16U)(SysTimer100ms - prevTimer100ms) >= 600)	// 1 min
					//					{
					//						RTC_TIME time;
					//						prevTimer100ms = SysTimer100ms;
					//						RTC_GetTime(BCD,&time);
					//						sprintf(MsgBuf, "%02X:%02X:%02X BAT=%3d ", time.hours, time.minutes, time.seconds, BatVoltVal);
					//						MsgOut(MsgBuf);
					//						sprintf(MsgBuf, "[OFF]AC=%d Volt=%4.2f mode=%d(%d)PWOFF\r\n", AC_CHECK, (FP32)BatVoltVal*(FP32)11/(FP32)1024*3.3, modeBattery, tempModeBattery);
					//						MsgOut(MsgBuf);
					//					}
					//					Delay_100ms(7);
				}

				Dsp_SetPage(old_page);
				DspLoadFont1(old_font);
				Dsp_Diffuse();

				sale_display_update(UPDATE_ALL);
				BuzOn(1);
				//charge_state = 0;
				//lowbat_state = 0;
				break;
			case BAT_NOT_AVAILABLE:
				BatterySign1 = OFF;
				BatterySign2 = OFF;
				BatterySign3 = OFF;
				sale_display_update(UPDATE_MODE);
				//charge_state = 0;
				//lowbat_state = 0;
				break;
		}
		//		if ((INT16U)(SysTimer100ms - prevTimer100ms) >= 600)	// 0.5 min
		//		{
		//			RTC_TIME time;
		//			prevTimer100ms = SysTimer100ms;
		//			RTC_GetTime(BCD,&time);
		//			sprintf(MsgBuf, "%02X:%02X:%02X BAT=%3d ", time.hours, time.minutes, time.seconds, BatVoltVal);
		//			MsgOut(MsgBuf);
		//			sprintf(MsgBuf, "AC=%d Volt=%4.2f mode=%d(%d)\r\n", AC_CHECK, (FP32)BatVoltVal*(FP32)11/(FP32)1024*3.3, modeBattery, tempModeBattery);
		//			MsgOut(MsgBuf);
		//		}
	}

}
#endif

INT8U   HoldTimeCheck(void)
{
	INT8U ret=OFF;
	INT8U i;
	long sumWeight;

	GlbHold.buffer[GlbHold.count%HOLD_BUFFER_NUM] = status_scale.Weight;
	GlbHold.count++;

	if( (INT16U)(SysTimer100ms - GlbHold.holdStartTime100ms) >= GlbHold.holdOperationTime )
	{
		sumWeight=0;
		if(GlbHold.count > HOLD_BUFFER_NUM) GlbHold.count = HOLD_BUFFER_NUM;
		for(i=0; i<GlbHold.count; i++)
		{
			sumWeight+=GlbHold.buffer[i];
		}
		if(GlbHold.count)
		{
			status_scale.Weight = sumWeight / GlbHold.count;
		}

		GlbHold.holdStartTime100ms=0;
		GlbHold.count=0;
		GlbHold.flagCalcOK=ON;
		ret = ON;
	}
	return ret;
}

void sale_hold_check(void)
{
	if( Operation.salePrintWait && GlbHold.holdOperationTime && GlbHold.holdStartTime100ms )	//SG070718.Hold Check
	{
		HoldTimeCheck();
	}
}

INT8U	OnOff(INT8U	tmp)
{
	if(tmp)	return ON;
	return OFF;
}

// 2,3은 Price.roundedPrice에 절삭된 가격이 입력된다 Price.roundedKind는 라벨인지 티켓인지
void	LoadGlobalSetting(void)
{
	//	INT8U flag_1,v8;
	INT8U v8;
	INT8U flag_clk,flag_add,flag_tmp,flag_font_size;
	INT16S   i16;
	INT8U scale_type;
#ifdef USE_QR_ADDTOTAL
	INT8U ttlbarcode_type;
	INT8U bartype;
#endif

	// Tare Flag
	TareOper.continuousTareUp = ad_get_accumul_tare();
	TareOper.continuousTareDn = ad_get_subtract_tare();

	flag_tmp = get_global_bparam(GLOBAL_AREA_TAREFLAG);
	TareOper.clearKeyTare      	= OnOff(flag_tmp&0x01);	// Auto Clear Key Tare
	TareOper.clearWeightTare	= OnOff(flag_tmp&0x02);	// Clear Key Tare
	TareOper.canadianTare      	= OnOff(flag_tmp&0x04);	// 반드시 TareOper.clearKeyTare가 ON 되어야 한다
	TareOper.overridePluTare	= OnOff(flag_tmp&0x08);	// Assent Tare Override
	TareOper.saveKeyTareForPLU	= OnOff(flag_tmp&0x10);
	TareOper.useTareAfterPLUCall	= OnOff(flag_tmp&0x20);
	TareOper.displayTareEU		= OnOff(flag_tmp&0x40);	// only weight
	TareOper.displayWeightTare	= OnOff(flag_tmp&0x80);	// Display Weight Tare on PT window
	//Price.PercentTareRate		= 0;
	Price.PercentTareWeight		= 0;

	// Price Zero Plu
	flag_tmp = get_global_bparam(GLOBAL_PRICE_ZERO_FLAG);

	Operation.unitpriceZeroOverride	= OnOff(flag_tmp&0x01);
	Operation.saveZeroOverride	= OnOff(flag_tmp&0x02);
#ifdef USE_CHN_CART_SCALE
	Operation.useCheckRemain	= OnOff(flag_tmp&0x10);
	Operation.traceCodeType		= (flag_tmp>>5)&0x03;
#endif
	// ====================================
	// Round Method
	// ====================================
	GlbPrice.roundPrice   = (INT16S)get_global_wparam(GLOBAL_AREA_PRICE_ROUND);// 0,5,10,100,1000

	//SG070419
	flag_tmp = get_global_bparam(GLOBAL_WEIGHT_ROUND);
	flag_tmp &= 0x07;
	GlbPrice.roundWeight = power10(flag_tmp);

	flag_tmp = get_global_bparam(GLOBAL_WEIGHT_ROUND);
	flag_tmp = (flag_tmp >>3)&0x01 ;
	GlbPrice.roundWeightType = flag_tmp;  //0 =round off/ 1=round down

	// BMLIM 2004.09.14
	GlbPrice.roundApply = get_global_bparam(GLOBAL_PRICE_ROUND_APPLY);
	//
	if (GlbPrice.roundApply==ROUND_NO) {
		GlbPrice.roundUp   =0;
		GlbPrice.roundPrice=0;
	}
	v8=get_global_bparam(GLOBAL_SALE_SETUP_FLAG4);
	GlbPrice.roundUp      = OnOff(v8&0x01);	//round up
	GlbOper.UseNegWeightLimit = OnOff(v8&0x02); // NegativeWeightLimit(남아공)
	v8=get_global_bparam(GLOBAL_BARCODE_PRICE_CUT);
	i16=power10((INT16S)v8);
	if (i16==0) i16=1;
	GlbPrice.roundBarcode = i16;// 0,10,100,1000
	// ====================================
	GlbOper.keyClearTime		= get_global_bparam(GLOBAL_TIME_KEYCLEAR);// 3초
	//DEL070130	GlbOper.keyAutoCallPlu		= get_global_bparam(GLOBAL_KEY_TIMEOUT_KEYPRESS);// time 1초, 0이면 사용하지 않음

	//tare
	flag_tmp=get_global_bparam(GLOBAL_SALE_SETUP_FLAG1);
	scale_type = get_global_bparam(GLOBAL_AREA_SCALETYPE);
	if(scale_type == SCALE_TYPE_SELF)		//SELF SERVICE 경우 SAVE로 시작 안함.
	{
		GlbOper.startSaveMode = OFF;
	}
	else
	{
		GlbOper.startSaveMode		= OnOff(flag_tmp&0x01);
	}
	GlbOper.printOneTimeAfterCall		= OnOff(flag_tmp&0x02);
	GlbOper.byCountWeight			= OnOff(flag_tmp&0x04);
	GlbOper.byCountApplyWeight		= OnOff(flag_tmp&0x08);	// display 공간 확보 해야함

	//===============================================================
	// GlbOper.nonFoodWeight		= OnOff(flag_tmp&0x10);
	//===============================================================

	GlbOper.weightDirectClear		= OnOff(flag_tmp&0x20);
	GlbOper.weightZeroPricePrint		= OnOff(flag_tmp&0x40);
	GlbOper.byWeightMulti			= OnOff(flag_tmp&0x80);

	flag_tmp = get_global_bparam(GLOBAL_REPORT_FLAG);	// Report Individual Clear
	GlbOper.XZreport		= OnOff(flag_tmp&0x02);
	if (MemLifetimeState == FTL_MEM_LIFETIME_1_MONTH) GlbOper.XZreport = OFF; // NVRAM 수명이 한 달 안 남았을 경우 Report 사용을 제한(다른 곳에서 NVRAM은 계속 사용)
	GlbOper.PrepackReport		= OnOff(flag_tmp&0x04);
	GlbOper.PrintUnderMinWeight	= OnOff(flag_tmp&0x20);	// Added by CJK 20050429
	GlbOper.PrintFixedItemAlways	= OnOff(flag_tmp&0x80); // Added by CJK 20050429
	PrtOper.IndividualClear 	= OnOff(flag_tmp&0x01);
	PrtOper.EnPrtZReport		= OnOff(flag_tmp&0x08);
	PrtOper.PrtLastTicket  		= OnOff(flag_tmp&0x10);
	PrtOper.PrtRepackSymbol		= OnOff(flag_tmp&0x40);

	flag_tmp=get_global_bparam(GLOBAL_SALE_SETUP_FLAG15);
	GlbOper.PrintUnderMinWeightOnPrepack = OnOff(flag_tmp&0x20);
	Prt_labelCap_flag = OnOff(flag_tmp&0x40);
#ifdef USE_CL5200_DISP_CONTINUOUS_REFRESH
	Disp_refresh_flag = flag_tmp&0x08; // param 737
	Disp_refresh_flag = Disp_refresh_flag>>3;
#endif

	flag_tmp=get_global_bparam(GLOBAL_SALE_SETUP_FLAG2);
	TareOper.addupWeightZero 	= OnOff(flag_tmp&0x01);	// 34
    GlbOper.PrintZeroTotalPrice = OnOff(flag_tmp&0x20);	// param 574 Allow Print Zero T.Price
#ifdef USE_HYUNDAI_MEAT_FUNCTION
	HyundaiMeatFlag = OnOff(flag_tmp&0x10);
#endif
	flag_tmp=get_global_bparam(GLOBAL_DISPLAY_INFO_FLAG);
	GlbOper.viewDiscount 	=OnOff(flag_tmp&0x01);
	GlbOper.priceAddExTax	=OnOff(flag_tmp&0x02);

	//SG070718
	memset(&GlbHold,0,sizeof(GlbHold));
	GlbHold.holdOperationTime = get_global_bparam(GLOBAL_SALE_SETUP_HOLDTIME);

	GlbOper.autoDoublePrint = get_global_bparam(GLOBAL_DUAL_PRINT_CNT);                      //yoo 080605   double print 상황에서 몇 장을 출력할지의 정보를 전달함
    if(GlbOper.autoDoublePrint > 9 )GlbOper.autoDoublePrint = 0;

	flag_tmp = get_global_bparam(GLOBAL_PRINT_OPER_FLAG);
	//yoo DEL 080605	GlbOper.autoDoublePrint 		= OnOff(flag_tmp&0x01);	//B0
	//DEL070705	GlbPrint.printCheckPeelOff		= ON;
	GlbPrint.printNoRewind			= OnOff(flag_tmp&0x02);	//B1
	GlbPrint.directNonfood			= OnOff(flag_tmp&0x40);	//B6
	GlbPrint.directWeight			= OnOff(flag_tmp&0x80);	//B7

	ConfirmPrintOnLabel  			= (flag_tmp>>2) & 0x03;
	flag_tmp=get_global_bparam(GLOBAL_REPORT_FLAG1);
	GlbPrint.reportType			= flag_tmp & 0x03;
	GlbOper.ClearOpenedClerkTransaction	= OnOff(flag_tmp&0x04);
	GlbPrint.reportZeroValue	= OnOff(flag_tmp&0x08);
	GlbPrint.reportTax			=OnOff(flag_tmp&0x10);
	GlbPrint.reportPay			=OnOff(flag_tmp&0x20);
	GlbPrint.reportRoundedSummary		=OnOff(flag_tmp&0x40);
	GlbPrint.reportStoreName		=OnOff(flag_tmp&0x80); //SG060106,ADD


	flag_tmp=get_global_bparam(GLOBAL_REPORT_FLAG2);
	//	GlbPrint.reportChange			=OnOff(flag_tmp&0x08);
	GlbPrint.reportChange			=OFF;
	GlbPrint.reportRepack			=OnOff(flag_tmp&0x10);
	GlbPrint.reportPrepack			=OnOff(flag_tmp&0x20);
	GlbPrint.reportNegative			=OnOff(flag_tmp&0x40);
	GlbPrint.reportOpenCash			=OnOff(flag_tmp&0x80);

	// Logging Parameter
	GlbFloat.logOffClerkTime		= (INT8U)get_global_wparam(GLOBAL_TIME_CLERKLOGOUT); 	// 15초

	flag_tmp=get_global_bparam(GLOBAL_LOG_FLAG);
	GlbFloat.logOffTransaction	= OnOff(flag_tmp&0x01);

	//////// Modified by CJK 20050726
	// Clerk Printing
	flag_clk=get_global_bparam(GLOBAL_PRINT_CLERK_FLAG1);
	GlbFloat.printClerkName	 		= OnOff(flag_clk&0x01);//	0->no, 1-> top, 2->bottom
	GlbFloat.printStoreName			= OnOff(flag_clk&0x02);//	0->no, 1-> top, 2->bottom
	GlbFloat.printList			= OnOff(flag_clk&0x04);
#ifdef USE_CHINA_DEFAULT
	GlbFloat.printBarcode			= 0;
#else
	GlbFloat.printBarcode			= OnOff(flag_clk&0x08);
#endif
	GlbFloat.printTotalLabel		= OnOff(flag_clk&0x10);
	GlbFloat.printBarcodeForList		= OnOff(flag_clk&0x20);
	//GlbFloat.advertiseLabel[0]		= 0;
	//GlbFloat.advertiseLabel[1]		= 0;

	GlbFloat.advertiseLabel[0]=get_global_wparam(GLOBAL_TICKET_MESSAGE_HEAD);
	GlbFloat.advertiseLabel[1]=get_global_wparam(GLOBAL_TICKET_MESSAGE_TAIL);

	flag_font_size = get_global_bparam(GLOBAL_PRT_TICKET_FONT_SIZE);
	GlbFloat.printFontSize[0] = flag_font_size & 0x03;      	// plu name
	GlbFloat.printFontSize[1] = (flag_font_size>>2) & 0x03;		// transaction data
	GlbFloat.printFontSize[2] = (flag_font_size>>4) & 0x03;		// total value
	GlbFloat.printFontSize[3] = (flag_font_size>>6) & 0x03;		// ticket serial number

	flag_clk=get_global_bparam(GLOBAL_PRINT_CLERK_FLAG2);
	GlbFloat.printLabel			= OnOff(flag_clk&0x01);
	GlbFloat.printTaxInfo			= OnOff(flag_clk&0x02);
	GlbFloat.printCopyTicket		= OnOff(flag_clk&0x08);
	GlbFloat.printCopyList			= OnOff(flag_clk&0x10);

	// Addup Printing
	flag_add=get_global_bparam(GLOBAL_PRINT_ADDUP_FLAG1);
	//GlbAddup.printStoreName		   	= OnOff(flag_add&0x01);		// UNUSE 12-MAY-05
	//GlbAddup.printList		       	= OnOff(flag_add&0x02);		// UNUSE 12-MAY-05
	//GlbAddup.printBarcode			= OnOff(flag_add&0x04);		// UNUSE 12-MAY-05
	GlbAddup.printTotalLabel		= OnOff(flag_add&0x08);
	//GlbAddup.printBarcodeForList		= OnOff(flag_add&0x10);			// UNUSE 12-MAY-05
	//GlbAddup.advertiseAddupLabel[0]	= 0;					// UNUSE 12-MAY-05
	//GlbAddup.advertiseAddupLabel[1]	= 0;					// UNUSE 12-MAY-05
	//if(  flag_add&0x20   )	GlbAddup.advertiseAddupLabel[0]=get_global_wparam(GLOBAL_ADDUP_MESSAGE_HEAD);
	//if(  flag_add&0x40   )	GlbAddup.advertiseAddupLabel[1]=get_global_wparam(GLOBAL_ADDUP_MESSAGE_TAIL);
	/////////////////////////

	flag_add=get_global_bparam(GLOBAL_PRINT_ADDUP_FLAG2);
	GlbAddup.printLabel    			= OnOff(flag_add&0x01);

	flag_add=get_global_bparam(GLOBAL_SALE_SAVE_FLAG);
	GlbOper.saveChangePrice = OnOff(flag_add&0x01);
	GlbOper.saveChangeType  = OnOff(flag_add&0x02);
	GlbOper.realtimeTransactionType = (flag_add >> 3)&0x0F;
	// 0:none, 1:basic, 2:basic+indiv.(Korea), 3:full(Korea)
	GlbOper.useSpecialPriceForDisc = OnOff(flag_add&0x80);

#ifdef USE_CHN_TRACE_DEFAULT
	flag_add=get_global_bparam(GLOBAL_SALE_SETUP_FLAG15);
	GlbOper.realtimeTransactionType |= (flag_add >> 3)&0x18;
	// 0:none, 1:basic, 2:basic+indiv.(Korea), 3:full(Korea) ~ 15 까지 사용
#endif
	
	flag_add=get_global_bparam(GLOBAL_SALE_SETUP_FLAG3);
	//DEL051229	GlbOper.enableByCount 		= OnOff(flag_add&0x01);
	GlbOper.applyUnitpriceForDisc 	= OnOff(flag_add&0x04);

	//flag_add = get_global_bparam(GLOBAL_SALE_SETUP_FLAG2);
	//Operation.doublePLUkey 		= OnOff(flag_add&0x10);

	flag_add = get_global_bparam(GLOBAL_SALE_SETUP_FLAG7);
	Operation.doubleDatekey		= OnOff(flag_add&0x08);	// CJK080320

	flag_add = get_global_bparam(GLOBAL_AREA_MULTIUNITFLAG);
	GlbOper.EnableMultiKgKey         = OnOff(flag_add&0x01);
	GlbOper.EnableKgLb 		 = OnOff(flag_add&0x02);
#ifdef USE_PRT_CONVERT_WEIGHT_KG_G
	GlbOper.EnableGramWeight = OnOff(flag_add&0x04);
#else
	GlbOper.EnableAutoConversionKgLb = OnOff(flag_add&0x04);
#endif
	GlbOper.EnableMultiLbKey 	 = OnOff(flag_add&0x08);

	//SG061214 FSP
	flag_add = get_global_bparam(GLOBAL_SALE_SETUP_FLAG6);
	GlbOper.UseFSP = (flag_add>>4)&0x01;

	flag_add = get_global_bparam(GLOBAL_SALE_SETUP_FLAG5);
	GlbOper.EnableQuickPlu = OnOff(flag_add&0x08);

	flag_add = get_global_bparam(GLOBAL_SALE_SETUP_FLAG9);
	GlbOper.PluReportType = OnOff(flag_add&0x10);

	flag_add = get_global_bparam(GLOBAL_SALE_SETUP_FLAG10);
	GlbOper.TraceabilityCallType = flag_add&0x07;		// menu 1873
	GlbOper.UseSendTraceInformation = (flag_add>>3)&0x03;		// menu 1873
	Operation.doublePLUkey 		= (flag_add>>5)&0x03;		// menu 1873

	flag_add = get_global_bparam(GLOBAL_SALE_SETUP_FLAG11);
	Operation.doubleKeyTime100ms = flag_add&0x0F;		// menu 1873, 하위 4bit 사용

	flag_tmp=get_global_bparam(GLOBAL_SALE_SETUP_FLAG8);
	UseEmartNetwork = OnOff(flag_tmp&0x40);	// parameter 723

#ifdef USE_TRACE_STANDALONE
	flag_tmp = get_global_bparam(GLOBAL_PRINT_OPER_FLAG2);
	UseCheckIndivNo = OnOff(flag_tmp&0x40);	// parameter 717
#endif

#ifdef USE_INDIA_FUNCTION
	flag_add = get_global_bparam(GLOBAL_SALE_SETUP_FLAG12);
	UseAreaScaleMode = (flag_add>>6)&0x03;	// 0:NormalScale(STD), 1:DualModeScale, 2:AreaScale

	if (UseAreaScaleMode == 2) GlbOper.realtimeTransactionType = 4; // 1:DualModeScale, 2:AreaScale

	flag_add = get_global_bparam(GLOBAL_PRICESIGN_1000);
	AllowNegativeInventory = OnOff(flag_add&0x02);
	UseItemCode13Digit = OnOff(flag_add&0x04);
#endif
#if defined(USE_NHMART_SAFE_MEAT) || defined(USE_CHECK_INDIV_INFO)
	flag_add = get_global_bparam(GLOBAL_PRINT_OPER_FLAG2);
	checkMtrace = OnOff(flag_add & 0x80);	//param 709
#endif
#ifdef USE_QR_ADDTOTAL
	flag_tmp = get_global_bparam(GLOBAL_SALE_SETUP_FLAG14);
	QrAddTotalFlag = OnOff(flag_tmp & 0x08);	//param 726
	if (QrAddTotalFlag)
	{
		ttlbarcode_type = get_barcodetype(GLOBAL_BARCODE_TOTAL_ID);
		bartype = barcodetype_codevalue[ttlbarcode_type];
		if (bartype == QRCODE) // Add up Total barcode type이 QR이면 QrAddTotalFlag On
			QrAddTotalFlag = 1;
		else
			QrAddTotalFlag = 0;
	}
#endif
	flag_tmp = get_global_bparam(GLOBAL_PRICESIGN_1000);
	GlbOper.send_speedkey_position = OnOff(flag_tmp & 0x10);	//param 751

	flag_tmp = get_global_bparam(GLOBAL_SALE_SETUP_FLAG17);
	GlbOper.UseAutoZero = OnOff(flag_tmp&0x01); // param 746

	flag_tmp = get_global_bparam(GLOBAL_SALE_SETUP_FLAG18);
	Prt_weight_Symbol_flag = flag_tmp&0x03; // param 739
	Prt_uprice_Symbol_flag = flag_tmp&0x1C; // param 740
	Prt_uprice_Symbol_flag = Prt_uprice_Symbol_flag>>2;
	RealtimeAddDataFlag = flag_tmp&0x40;
	RealtimeAddDataFlag = RealtimeAddDataFlag>>6;

	flag_tmp = get_global_bparam(GLOBAL_SALE_SETUP_FLAG20);
	PrtBarcodePriceAddExTaxFlag = OnOff(flag_tmp & 0x02);
	GlbOper.PrintUnderMinWeightOnAUTO = OnOff(flag_tmp&0x08);
	GlbOper.PrintWhenchangingPLUInAUTO = OnOff(flag_tmp&0x10);    
#ifdef USE_EMART_DEFAULT
	PrtName4_With_slaughthouse = OnOff(flag_tmp & 0x04);// param 752
#endif

	flag_tmp = get_global_bparam(GLOBAL_SALE_SETUP_FLAG19);
	Operation.packingPrice = OnOff(flag_tmp&0x02);

	flag_add = get_global_bparam(GLOBAL_AREA_MULTIUNITFLAG);
	GlbOper.TriangleType = (flag_add>>4) & 0x07;

	flag_add = get_global_bparam(GLOBAL_SALE_SETUP_FLAG14);
	PLU_SCROLL.onoff=(flag_add&0x01);

#ifdef USE_QR_ADDTOTAL
	memset(QrCfg_OpeningCommand, 0, sizeof(QrCfg_OpeningCommand));
	memset(QrCfg_PosId, 0, sizeof(QrCfg_PosId));
	memset(QrCfg_EanPrefix, 0, sizeof(QrCfg_EanPrefix));
	memset(QrCfg_EanSuffix, 0, sizeof(QrCfg_EanSuffix));
	memset(QrCfg_ClosingCommand, 0, sizeof(QrCfg_ClosingCommand));
	QrCfg_DelLastSuffix = 0;
	get_global_sparam(GLOBAL_QR_CFG_OPEN_CMD, QrCfg_OpeningCommand, 1);
	get_global_sparam(GLOBAL_QR_CFG_POS_ID, QrCfg_PosId, 2);
	get_global_sparam(GLOBAL_QR_CFG_EAN_PREFIX, QrCfg_EanPrefix, 2);
	get_global_sparam(GLOBAL_QR_CFG_EAN_SUFFIX, QrCfg_EanSuffix, 2);
	get_global_sparam(GLOBAL_QR_CFG_CLOSE_CMD, QrCfg_ClosingCommand, 1);
	QrCfg_DelLastSuffix = get_global_bparam(GLOBAL_QR_CFG_DEL_LAST_SUFFIX);
#endif
}

extern INT8U flagTempAllowSale;
extern CURRENCY_PRICE_TYPE	CurrencyPrice;
extern INT16U scroll_timer;
extern INT16S    scroll_startid;
INT16S LabelAndTransaction(void);
INT16S LinkedLabelPrint(void);
//DEL070123extern	void	TransactionCheckDate(void);
//extern INT16S GetMsg(CAPTION_STRUCT* cap,INT16U messageNum);
extern void	CheckScroll(void);
extern INT8U NetClerkLockRelease(INT8U operator);
#ifdef USE_RSS_DEBUG
long rssBarStrDataHigh[4] = {1L, 10L, 100L, 1000L};
long rssBarStrDataLow[9] = {	1L,
10L,
100L,
1000L,
10000L,
100000L,
1000000L,
10000000L,
100000000L
};

long rssBarStrDataHighBase[10] =
{
	0,
	1111,
	2222,
	3333,
	4444,
	5555,
	6666,
	7777,
	8888,
	9999
};
long rssBarStrDataLowBase[10] =
{
	0,
	111111111L,
	222222222L,
	333333333L,
	444444444L,
	555555555L,
	666666666L,
	777777777L,
	888888888L,
	999999999L
};

RSS_BARCODE_TEST_STRUCT rssBarStr;
#endif//#ifdef USE_RSS_DEBUG
extern INT8U UsbdeviceIsConnected;
void mode_sale(void)
{
	INT8U xmode,okFlag,oper_mode;
	//	INT8U sale_mode;
	INT8U startAuto;
	INT16U key_action;
	INT16U linkedLabel;
	INT8U clearFlag,checkClrParam;

	INT8U Slave_flag=0;
	INT8U transaction_clerk;
	INT8U mode;
#ifdef USE_RSS_DEBUG
	INT8U i, j, k;
#endif//#ifdef USE_RSS_DEBUG

	sale_display_update(UPDATE_CLEARALL|0x0fff);

	// Set Sales Key
	KEY_SetMode(0);
	// default setting
	LoadGlobalSetting();
	Operation.fixedAction=0;
	Operation.fixedPrice = 0;
	Operation.useMultipleUnits = 0;
	Operation.salePluClearWait=OFF;
	Operation.salePrintWait = OFF;
	Operation.OverTotalPrice = OFF;
	Operation.UnderWeight = OFF;
#ifndef USE_EMART_PRODUCTION_KEY
	Operation.checkin = OFF;
#endif
#ifdef USE_EMART_BACK_LABEL
	Operation.BackLabeledFlag = OFF;
#endif
#ifdef USE_REGIST_DISPOSAL
	Operation.RegistDisposalFlag = OFF;
#endif
	//	Operation.woldMode=0;
	//Operation.modeScreenType = 0;
	if(!Operation.systemOnlyStart)	// 초기 한번만 세팅
	{
		status_scale.weight_print = 0;
		Operation.systemOnlyStart = ON;
#ifdef USE_TEST_AUTO_LABEL_PRINT
		PowerStartFlag = ON;
#endif
		oper_mode = get_global_bparam(GLOBAL_OPERATION_MODE);
		if (oper_mode==MODE_SELF) {
			Operation.operationMode    = oper_mode;
		} else {
			if (Operation.operationMode!=MODE_NORMAL) {
				set_global_bparam(GLOBAL_OPERATION_MODE,MODE_NORMAL);
			}
			Operation.operationMode    = MODE_NORMAL;
		}
		Operation.addStandatdClerk = OFF;

		Operation.wAuto = OFF;
		startAuto = get_global_bparam(GLOBAL_SALE_SETUP_FLAG6);
		Operation.wAuto = OnOff(startAuto&0x04); //SG060928  : auto mode
		if(GlbOper.startSaveMode)
			Operation.wSave = ON;	//OPTION: start save mode
		else
			Operation.wSave = OFF;	//OPTION: start save mode
		//		Operation.woldMode = Operation.wSave;
		Operation.wPrepack = 0;
		Operation.rewindReady = OFF;

		Cash_close();
		Operation.openCash =OFF;
		TimeOpenCashDrawer = 0;

		Dsp_ChangeMode(DSP_SALE_MODE);
		mode = get_nvram_bparam(NVRAM_DISPLAY_DATETIME_MODE);
		if(mode)	display_date_change(mode,ON);

		memset(&ReopenedTransPay, 0, sizeof(TRANSACTION_PAY));
	}
	Operation.override = 0;
	Operation.multiply = 0;
	Operation.multiplyEdit = OFF;
	Operation.transAddup = 0;
	Operation.operator = 0;
	Operation.level = 0;
	Operation.openCash =OFF;
	Operation.negative = OFF;
	Operation.specialPrice = OFF;
	Operation.specialPriceDisc = OFF;
	Operation.directClearWeight = OFF;
	//DEL051229	Operation.keyInWeight = OFF;
	//Operation.doublePLUkey = OFF;
	Operation.transactionOK = OFF;	//plu가 호출되면 off, transaction이 되면 on
	Operation.autoDoublePrint = OFF;
	//key tare clear 또는 weight tare을 제거

	//DEL051229	Operation.keyDiscount=OFF;
	Operation.logOperator = OFF;
	Operation.logLevel = OFF;
	Operation.pressKey = OFF;
	Operation.pressKeyForNvErase = OFF;
#ifdef USE_CHN_CART_SCALE
	Operation.oneTouchTicketPrint = OFF;
#endif

	DiscSale.saleDiscountType = DISCOUNT_NO;	// CJK070619 : 초기화가 안될 경우 무게 디스플레이 이상발생
	DiscSale.saleGlobalOk     = OFF;

	// batch test
	BatchCount=0;
	memset(Batch, 0, sizeof(PLU_BATCH_TYPE)*MAX_PLU_BATCH); //SG070322
	operation_load();
	Operation.MaxUnitPrice   = GetMaxUnitPrice();
	Operation.MaxTotalPrice  = GetMaxTotalPrice();
	////정산 삭제
	//ClerkClearTransaction();
	//ClearSalesSummaries(99);
	////
	CurrencyPrice.CurrencyUseDefault = get_global_bparam(GLOBAL_DUAL_CURRENCY);
	LoadDefaultCurrency();
#ifdef USE_HYUNDAI_MEAT_FUNCTION
	if(get_labelid_from_labelkey_num(MEAT_STORAGE_LABEL_KEY_NUM) == get_global_wparam(GLOBAL_LABEL_STANDARD)) // 현재 라벨이 보관 라벨로 설정 된 경우 
		Operation.meatStorageLabelFlag = ON;
	else // 현재 라벨이 보관라벨이 아닌 경우
		Operation.meatStorageLabelFlag = OFF;
#endif
	
#ifdef USE_KOR_TRACE
	kortrace_init();
#endif
	MaxCapaWeight = ad_get_max_capa(ADM_Status.Capa, CurPluWeightUnit, 0);
	MaxCapaWeight = ad_cnv_adm2main(MaxCapaWeight);

	checkClrParam = (get_global_bparam(GLOBAL_SALE_SETUP_FLAG17)>>3)&0x01;
	clearFlag = OFF;
	sale_pluclear(ON);
	sale_display_update(0x0fff);//061128

	Operation.modeProcess = SALE_PROC_ADM;

	if(!status_scale.clerkid)
		status_scale.level=0;

	if(Operation.operationMode == MODE_SELF)
	{
		Operation.wSave = OFF;
		Operation.wPrepack = OFF;

	}
	scroll_timer=SysTimer100ms;
	scroll_startid = 0;
	logout();
	while (status_run.run_mode == RUN_SALE)
	{
		if (network_status.connection)	// remote와 연결 되어 있을 때
		{
			mode = send_transaction_backup();// 백업된 판매 데이터를 master로 전송
			if (mode == 6)	// key 눌렸을 때
			{
				Operation.modeProcess = SALE_PROC_KEY;
			}
		}
		xmode=(status_scale.flag_input>>1)&0x07;
		if(!KEY_IsKey()) network_check();
		usb_update_check();
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
		Slave_flag = get_nvram_bparam(NVRAM_SLAVECHECK_FLAG);
		transaction_clerk = get_nvram_bparam(NVRAM_TRANSACTION_CLERK);
		if(Slave_flag)
		{
			if(network_timeout_check(50,NETWORK_TIMEOUT_CLERK_DEL))		//5초에 한번씩 시도하도록~! 메뉴키 들어갈 수 있게.
			{
				if(network_ClerkDeleteTransaction(transaction_clerk))
				{
					Slave_flag = 0;
					set_nvram_bparam(NVRAM_SLAVECHECK_FLAG, Slave_flag);
					ClerkDeleteTransaction(transaction_clerk);
					NetClerkLockRelease(transaction_clerk);
				}
			}
		}
#else
		if(network_timeout_check(50,NETWORK_TIMEOUT_CLERK_DEL))		//5초에 한번씩 시도하도록~! 메뉴키 들어갈 수 있게.
		{
			Slave_flag = get_nvram_bparam(NVRAM_SLAVECHECK_FLAG);
			if(Slave_flag)
			{
				transaction_clerk = get_nvram_bparam(NVRAM_TRANSACTION_CLERK);
				if(network_ClerkDeleteTransaction(transaction_clerk))
				{
					Slave_flag = 0;
					set_nvram_bparam(NVRAM_SLAVECHECK_FLAG, Slave_flag);
					ClerkDeleteTransaction(transaction_clerk);
					NetClerkLockRelease(transaction_clerk);
				}
			}
		}
#endif
		CheckTimes();
#ifdef USE_PLU_SCROLLING
		scroll_PLU();
#endif
		scroll_message();

#ifdef USE_CTS_FUNCTION
		CtsAlarmProc();
#endif
#ifdef USE_SETPRODUCT_SALE
		sale_setproduct_Proc();
#endif
#ifdef USE_HYUNDAI_SET_FUNCTION
		sale_hyundai_meatset_proc();
#endif
#ifdef USE_REGIST_DISPOSAL
		if(Operation.RegistDisposalFlag)
			sale_display_disposal(REGIST_DISPOSAL_STAT_DISP);
#endif

		if (!xmode) {
		    if (sale_key_time_clear()) {
				if (get_global_bparam(GLOBAL_KEY_TIMEOUT_KEYPRESS) && (key_action=sale_key_get_action())) {
					KEY_Write((INT8U)key_action,0, 0);
				} else {
					KeyDrv.FirstKey = 1;
				}
		    }
		}
		if (InputQuickPluState && CallQuickPlu)
		{
			keyapp_pluno(status_scale.departmentid, status_scale.cur_quickplu, OFF);
			sale_display_update(UPDATE_DATETIME);
			CallQuickPlu = OFF;
			ReadyQuickPlu = OFF;
		}

		if(BatchCount && !Operation.salePrintWait && !status_scale.print_multiple)
		{
			if(BatchCount>BatchIndex)
			{
				keyapp_pluno(Batch[BatchIndex].dept,Batch[BatchIndex].plu,OFF);
				status_scale.print_multiple = Batch[BatchIndex].printCount;

				Operation.wPrepack = OFF;
				Operation.multiply = ON;
				Operation.multiplyEdit = ON;
				ProcessAll();
				keyapp_print(OFF);
				BatchIndex++;
			}
			else
			{
				BatchCount=0;    BatchIndex=0;
			}
		}
		switch(Operation.modeProcess)
		{
			case SALE_PROC_START:
				Operation.modeProcess = SALE_PROC_DISPLAY;
				break;
			case SALE_PROC_DISPLAY:
				Operation.modeProcess = SALE_PROC_KEY;
				sale_display_proc(OFF);
				break;
			case SALE_PROC_KEY:
				Operation.modeProcess = SALE_PROC_ADM;
				sale_key_proc();
				break;
			case SALE_PROC_ADM:
				Operation.modeProcess = SALE_PROC_CALC_PRICE;
				sale_adm_proc();
				sale_hold_check();
				CurPluWeightUnit = ADM_GetData.CurUnit;
#ifdef NOT_USE_NETSET_LOCK
				status_scale.scale_lock = 0;
#endif
				if (!status_scale.scale_lock) {
					sale_auto_check();
					sale_prepack_check();
					sale_weight_check();	//Added by JJANG 20080617
				}
				break;
			case SALE_PROC_CALC_PRICE:
				Operation.modeProcess = SALE_PROC_DISPLAY;
				if (!status_scale.scale_lock) {
					sale_calcprice();
				}
				break;
			default:
				Operation.modeProcess = SALE_PROC_ADM;
				break;
		}
		okFlag = OFF;// check stable
#ifdef USE_TEST_AUTO_LABEL_PRINT
		if(PowerStartFlag)
		{
			printer_test();
			PowerStartFlag = 0;
		}
#endif
		if(Operation.salePrintWait && ADM_GetData.Stable)
		{
			if(PluType()==PLU_BY_WEIGHT)
			{
				if(status_scale.Plu.fixed_weight || Operation.fixedPrice)	okFlag=ON;
				else
				{
					okFlag=ON;
					if(!CheckPluPrint(0))
					{
						okFlag = OFF;
						Operation.salePrintWait = OFF;
						Operation.insertLabelData = OFF;
					}
				}
			}
			else
			{
				okFlag = ON;
#ifdef USE_INDIA_FUNCTION
				if(!CheckPluPrint(0))
				{
					okFlag = OFF;
					Operation.salePrintWait = OFF;
					Operation.insertLabelData = OFF;
				}
#endif
			}
		}
		else if(Operation.salePrintWait && GlbHold.holdOperationTime)	//SG070718.Hold Check
		{
			if(GlbHold.flagCalcOK)
			{
				okFlag=ON;
				if(!CheckPluPrint(0))
				{
					okFlag = OFF;
					Operation.salePrintWait = OFF;
					Operation.insertLabelData = OFF;
				}
				else
				{
					sale_calcprice();
					sale_display_update(0x0fff);
					GlbHold.flagCalcOK = OFF;
				}
			}
		}

		if(status_scale.print_multiple && okFlag)
		{
			if(Operation.operationClerk==OPER_STD_CLERK && !Operation.addStandatdClerk)
			{
				// 라벨을 출력하지않고 total add도 하지 않는데 정산만 들어감
				if(PrtStruct.Mode == RECEIPT)
				{
					DisplayMsg(global_message[MSG_NOT_ADDSUMMARY]);
					Operation.salePrintWait = OFF;
					okFlag = OFF;
				}
			}
			if(okFlag)
			{
				sale_display_proc(OFF);// new insert
				linkedLabel = Get2ndLabelId(status_scale.departmentid, status_scale.cur_pluid);//LoadLinkedLabel();
				if(LabelAndTransaction())
				{
#ifdef USE_HYUNDAI_SET_FUNCTION
					if(MeatSetSaleMode)
						hyundai_print_set_customer();
#endif
					if(linkedLabel)
					{
						StartPrintFlag=ON;
						LinkedLabelPrint();
					}
					sale_display_update(UPDATE_WEIGHT);
					
#ifdef USE_KOR_TRACE
					KorTraceCheckMemberCode();
#endif
				}
			}
		}

        if(!(Operation.wPrepack || Operation.wSave))// 우선순위 : Prepack, Save  키 > P679
        {// P679가 Y여도 Prepack, Save 키 누르면 PLU 해제 되지 않음.
    		if(checkClrParam)// P679 
    		{
    			if((status_scale.Plu.ptype == PLU_BY_WEIGHT != 0 || status_scale.cur_keyvalue > 0)  && ADM_GetData.Stable) // (Plu call 또는 Unit price 표시 값이 > 0) 이면서 stable
    			{
    				if(status_scale.Weight > 0)
    					clearFlag = ON;
				}
				if((status_scale.Weight <= 0) && ADM_GetData.Stable && clearFlag)
    			{
					clearFlag = OFF;
					Operation.salePluClearWait = ON;
					Operation.transactionOK = ON;
    			}
    		}
        }
		sale_auto_clear();
		sale_auto_zero();
#ifdef USE_KOR_TRACE
		KorTraceSendWeight();
#else
		sale_send_weight();
#endif
#ifdef USE_CPU_LOAD_TEST
        SFlashTestReadWrite( &CommBuf );
#endif
    } //while()
    CheckScroll();
}

void Check_Peel_Head_Sensor(void)
{
	char string_buf[5];
	string_buf[0] = GS;
	string_buf[1] = 'V';
	PutDataRxBuf(&CommBufPrt, string_buf, 2);
	//	while(CheckRxBuf(&CommBufPrt)) {
	//		CASInterpreter(&CommBufPrt);
	//	}
	//	while(PrtDrvStruct.PrtState != PRT_STATE0I);
	PrintListWaitEndWithoutComm();

	//flagOutOfPaper = PrtDrvStruct.OutOfPaperFlag;
}

char	PeelHeadMsgFlag = 0;
//DEL070705void	DispClearPeelHeadMsg(void)
//DEL070705{
//DEL070705//DEL070705	if(PeelHeadMsgFlag)
//DEL070705//DEL070705	{
//DEL070705		//strset(gmsg,0x20,8);
//DEL070705		//PutNSmallString(0,25,gmsg,8);
//DEL070705		PeelHeadMsgFlag = 0;
//DEL070705		sale_display_update(UPDATE_TITLE2);
//DEL070705//DEL070705	}
//DEL070705}

INT16S CheckPeelHead(INT8U check_peeloff, INT8U clear_msg)
{
	static char prev_peelhead = 0;
	INT8U enable_headup;
	INT16S result;
	//DEL070705	if(Operation.rewindReady)	return ON;

	result = ON;
	if (clear_msg)
	{
		PeelHeadMsgFlag = 0;
		//result = ON;
	}
	else
	{
		Check_Peel_Head_Sensor();
		//		if (PrtDrvStruct.HeadUpFlag)
		enable_headup = get_global_bparam(GLOBAL_PRT_HEADUP_SENSOR);
		if (enable_headup && PrtDrvStruct.HeadUpFlag)	//20090720 Modified by JJANG
		{
			//PutSmallString(0,25,global_message[MSG_OPEN_HEAD]);
			PeelHeadMsgFlag = 1;
			PrtDrvStruct.HeadUpFlag=OFF;
			result = OFF;
		}
		else if (PrtDrvStruct.HangingFormFlag && check_peeloff)//GlbPrint.printCheckPeelOff -> by feed key
		{
			//PutSmallString(0,25,global_message[MSG_PEELOFF]);
			PeelHeadMsgFlag = 2;
			PrtDrvStruct.HangingFormFlag=OFF;
			result = OFF;
		}
		else if (PeelHeadMsgFlag)
		{
			PeelHeadMsgFlag = 0;
			//result = ON;
		}

		if(StartPrintFlag);
		//		else if(!PrtDrvStruct.PrtCompleteFlag && PrtStruct.Mode == LABEL)	// Modified by CJK 20040923
		else if(!PrtDrvStruct.PrtCompleteFlag && PrtStruct.Mode == LABEL && Operation.operationClerk != OPER_FLOATING)	// Modified by CJK 20040923
		{
			result = OFF;
		}
	}
	if (prev_peelhead != PeelHeadMsgFlag || clear_msg)
	{
		sale_display_update(UPDATE_TITLE2);
	}
	prev_peelhead = PeelHeadMsgFlag;
	return result;
}


INT8U GetChangeFlag(INT8U curPluType, INT8U orgPluType, INT32U curPrice, INT32U orgPrice, INT8U curFixed, INT8U orgFixed, INT32U dcPrice)
{
	INT8U changeFlag;

	//sprintf(MsgBuf, "plutype=(%d,%d), price=(%ld,%ld),Fixed=(%d,%d),dc=%ld\r\n", curPluType, orgPluType, curPrice, orgPrice, curFixed, orgFixed, dcPrice);
	//MsgOut(MsgBuf);
#ifdef USE_EMART_NETWORK
	curPluType %= 3;
	orgPluType %= 3;
#endif
#ifdef USE_DIVIDING_FUNCTION
	if(status_scale.divisor)
	{
		orgPrice /= status_scale.divisor;
	}
#endif
#ifdef USE_LOTTEMART_NETWORK
	if (curPluType == PLU_BY_WEIGHT && !curFixed)	// 현재 중량상품
	{
		if (dcPrice)	// 할인 판매
		{
			changeFlag = FLAG3_TRANS_NONE;		// 0:변경없음  롯데마트는 할인이 없음.
		}
		else
		{
			if (!(orgPluType == PLU_BY_WEIGHT && !orgFixed))
				changeFlag = FLAG3_TRANS_NONE;	// 3:정액상품(by-ct)->중량상품 //롯데마트는 가격변경 내역만 보기 원함.

			else if (orgPrice == curPrice)
				changeFlag = FLAG3_TRANS_NONE;		// 0:변경없음
			else
				changeFlag = FLAG3_TRANS_WEIGHT;	// 1:중량상품 단가변경
		}
	}
	else	// 현재 정액상품
	{
		if (dcPrice)	// 할인 판매
		{
			changeFlag = FLAG3_TRANS_NONE;		// 0:변경없음 롯데마트는 할인이 없음.
		}
		else
		{
			if (orgPluType == PLU_BY_WEIGHT && !orgFixed)
				changeFlag = FLAG3_TRANS_NONE;	// 4:중량상품->정액상품 단가변경 //롯데마트는 가격변경 내역만 보기 원함.
			else if (orgPrice == curPrice)
				changeFlag = FLAG3_TRANS_NONE;		// 0:변경없음
			else
				changeFlag = FLAG3_TRANS_FIXED; 	// 2:정액상품 단가변경
		}
	}
#else
	if (curPluType == PLU_BY_WEIGHT && !curFixed)	// 현재 중량상품
	{
		if (dcPrice)	// 할인 판매
		{
			if (!(orgPluType == PLU_BY_WEIGHT && !orgFixed))
				changeFlag = FLAG3_TRANS_DC_CHG_WEIGHT;	// 7:정액상품(by-ct)->중량상품 할인
			else
				changeFlag = FLAG3_TRANS_DC_WEIGHT;	// 5:중량상품 할인
		}
		else
		{
			if (!(orgPluType == PLU_BY_WEIGHT && !orgFixed))
				changeFlag = FLAG3_TRANS_CHG_WEIGHT;	// 3:정액상품(by-ct)->중량상품
			else if (orgPrice == curPrice)
				changeFlag = FLAG3_TRANS_NONE;		// 0:변경없음
			else
				changeFlag = FLAG3_TRANS_WEIGHT;	// 1:중량상품 단가변경
		}
	}
	else	// 현재 정액상품
	{
		if (dcPrice)	// 할인 판매
		{
			if (orgPluType == PLU_BY_WEIGHT && !orgFixed)
				changeFlag = FLAG3_TRANS_DC_CHG_FIXED;	// 8:중량상품->정액상품 할인
			else
				changeFlag = FLAG3_TRANS_DC_FIXED;	// 6:정액상품 할인
		}
		else
		{
			if (orgPluType == PLU_BY_WEIGHT && !orgFixed)
				changeFlag = FLAG3_TRANS_CHG_FIXED;	// 4:중량상품->정액상품 단가변경
			else if (orgPrice == curPrice)
				changeFlag = FLAG3_TRANS_NONE;		// 0:변경없음
			else
				changeFlag = FLAG3_TRANS_FIXED;		// 2:정액상품 단가변경
		}
	}
#endif
	return changeFlag;
}
#ifdef USE_NHMART_SAFE_MEAT
#define END_OF_CALL_KEY 7	//개체 index 1~7 OFFLINE 모드에서 개체 호출키에 사용.
void SaveCalledIndiv(void)
{
	INT32U addr;
	INT16U start_indiv_idx;
	INT16U i;
	INT16U idx;
	TRACE_INDIVIDUAL_IDENTITY savedIndiv;
	INT8U savedIndStr[INDIVIDUAL_NO_LEN + 1];
	INT8U isSAME = OFF;

#ifdef USE_TRACE_1000QTY //sewon 161201
	addr = EXT_MEMCARD_ADDR ;
#else
	addr = DFLASH_BASE;
#endif
	addr += FLASH_TRACE_STANDALONE_AREA;
	for(i = 0 ; i < MAX_INDIVIDUAL_NO ; i++)
	{
		//개체번호 비교.
		Flash_sread(addr + (i * TRACE_INDIVIDUAL_IDENTITY_SIZE) + 3, (HUGEDATA INT8U *)&savedIndStr, INDIVIDUAL_NO_LEN);
		if(savedIndStr[11] != CurTraceStatus.indivStr.individualNO[11]) continue;	//short circuit
		if(savedIndStr[10] != CurTraceStatus.indivStr.individualNO[10]) continue;	//short circuit
		if(strcmp(savedIndStr, CurTraceStatus.indivStr.individualNO) == 0)
		{
			isSAME = ON;
			idx = i;
			break;
		}
	}
	if(isSAME == ON)		//개체정보 항목 전체를 비교하고 다르면 해당 FLASH 정보 업데이트
	{
		Flash_sread(addr + (idx * TRACE_INDIVIDUAL_IDENTITY_SIZE), (HUGEDATA INT8U *)&savedIndiv, TRACE_INDIVIDUAL_IDENTITY_SIZE);
		if(savedIndiv.lotFlag != CurTraceStatus.indivStr.lotFlag)
		{
			isSAME = OFF;
		}
		for(i = 24 ; i < TRACE_INDIVIDUAL_IDENTITY_SIZE - 1 ; i++)	//개체번호 이 후 부터 비교.
		{
			if(((INT8U *)&savedIndiv)[i] != ((INT8U *)&CurTraceStatus.indivStr)[i])
			{
				isSAME = OFF;
				break;
			}
		}
		if(isSAME == OFF)
		{
			CurTraceStatus.indivStr.index = savedIndiv.index;
			addr += (INT32U)(savedIndiv.index - 1) * TRACE_INDIVIDUAL_IDENTITY_SIZE;
			Flash_swrite(addr, (HUGEDATA INT8U *)&CurTraceStatus.indivStr, TRACE_INDIVIDUAL_IDENTITY_SIZE);
		}
	}
	else	//다르면 FLASH에 새로운 개체 추가.
	{
		start_indiv_idx = get_nvram_wparam(NVRAM_NH_SAFE_INDIV_INDEX);
		if(start_indiv_idx >= MAX_INDIVIDUAL_NO) start_indiv_idx = END_OF_CALL_KEY + 1; //저장된 값이 문제 있는 경우
		CurTraceStatus.indivStr.index = start_indiv_idx;
		addr += (INT32U)(start_indiv_idx - 1) * TRACE_INDIVIDUAL_IDENTITY_SIZE;
		Flash_swrite(addr, (HUGEDATA INT8U *)&CurTraceStatus.indivStr, TRACE_INDIVIDUAL_IDENTITY_SIZE);
		start_indiv_idx++;
		if(start_indiv_idx == MAX_INDIVIDUAL_NO)	//max 에 다다른 경우 처음으로 다시 돌아감.
		{
			start_indiv_idx = END_OF_CALL_KEY + 1;
		}
		set_nvram_wparam(NVRAM_NH_SAFE_INDIV_INDEX, start_indiv_idx);
	}
}
#endif


INT8U FlagTransactionWithoutPrint = 0;

extern void	SaleInsertLabelData(void);
extern void display_message_page_mid(char *str);
extern long PrevPrintOriginPrice;
extern INT8U Multi_PeelEnable;
extern INT8U Multi_RewindEnable;
extern INT8U MultiPrintFlag;
extern INT16U	DoublePLUPrevSkey;
extern INT8U printdateOFF;
extern long InventoryUnitWeight;
extern void transaction_sale_store(TRANSACTION_RECORD_TYPE *pTrans, TRANSACTION_RECORD_SUB_TYPE *pTrans_sub, TR_ADDITION_DATA_TYPE* pTrans_add, UNITPRICE_CHANGE_TYPE *pChgPrice, INT8U mode, INT8U sendflag);
INT16S LabelAndTransaction(void)
{
	INT16S stat;
	INT8U  checkPrint;
#ifdef USE_MULTI_TRACE_NO_TABLE
	INT8U ind_str1[INDIVIDUAL_NO_LEN];
	INT8U ind_str2[INDIVIDUAL_NO_LEN];
	INT8U ind_str3[INDIVIDUAL_NO_LEN];
	INT8U sl_no1;
	INT8U sl_no2;
	INT8U sl_no3;
#endif
	INT8U flagOut;

	if(Operation.OverTotalPrice)	return OFF;
#ifdef USE_PERSIAN_SALE_AMOUNT_LIMIT
	if(sale_check_sale_total_amount(Price.TotalPrice[PRICE_RESULT]))
	{
		Operation.salePrintWait=OFF;
		return OFF;
	}
#endif

	if(!Operation.insertLabelData || PrevPrintOriginPrice != Price.TotalPrice[PRICE_ORIGIN])
	{
		SaleInsertLabelData();
	}

	checkPrint = (get_global_bparam(GLOBAL_PRICE_ZERO_FLAG)>>7)&0x01;	// param 528, No Print
#ifdef USE_SETPRODUCT_SALE
	if(SetproductSaleMode && setproduct_scan_flag)
	{
		checkPrint = 1;
		setproduct_scan_flag = OFF;
	}
#endif
#ifdef USE_REGIST_DISPOSAL
	if (Operation.RegistDisposalFlag && (status_scale.Plu.sales_msg == TRANS_DISPOSAL_RETURN || status_scale.Plu.sales_msg == TRANS_DISPOSAL_FRESHNESS_LOSS))
		checkPrint = 1;
#endif
#ifdef USE_HYUNDAI_SET_FUNCTION
	if(MeatSetSaleMode)
	{
		if(MeatSetSaleUseScanner) // 기존 라벨 스캔해서 세트 재구성 하는 경우 라벨 발행 안 함 
			checkPrint = 1;
		if(Operation.saleStatus == SALE_SET_COMPONENTS)
			Operation.saleStatus = PRINT_SET_COMPONENTS_CLERK;
	}
#endif

	stat = 1;
	if(checkPrint == 1)
		Operation.printON = OFF;
	else
		Operation.printON = ON;
	if(Operation.transAddup)
	{
		//new
		if(GlbAddup.printLabel || Operation.operationClerk==OPER_STD_CLERK)	// no print , yes transaction
			Operation.printON = ON;
		else
			Operation.printON = OFF;
#ifdef USE_QR_ADDTOTAL
//		if (QrAddTotalFlag) // QR Add total 사용시 Addup일 경우 라벨 인쇄 안 함 
//			Operation.printON = OFF;	//<- GlbAddup.printLabel 설정값 따라가게 수정.
#endif
#ifdef USE_INDIA_FUNCTION
		if (UseAreaScaleMode == 2) Operation.printON = OFF; // 0:NormalScale(STD), 1:DualModeScale, 2:AreaScale
#endif
	}
	if(PrtStruct.Mode == RECEIPT) Operation.printON = OFF;	// Added by CJK

	if(Operation.printON && !CheckPeelHead(ON, OFF))	return OFF;

	if(Operation.printON)
	{
		flagOut = get_nvram_bparam(NVRAM_PRT_OUTOFPAPER);
		if(flagOut)
		{
			Operation.salePrintWait = OFF;
			status_scale.print_multiple = 0;
			sale_display_update_error(0xE602,0,0);
			Key_DelaySec(10,0);
			if(Operation.multiply)
			{
				Operation.multiply = OFF;
			}
			return OFF;
		}
//		if(StartPrintFlag)
//		{
			if (FlagTransactionWithoutPrint)
			{
				stat = ON;
			}
			else
			{
				stat = Prt_PrintStart(PRT_ITEM_NORMAL_MODE, PrtStruct.Mode, 0, OFF, 0, 0);
			}
//		}
		/*else
		{
			if (FlagTransactionWithoutPrint)
			{
				stat = ON;
			}
			else
			{
				stat = Prt_PrintStart(PRT_ITEM_NORMAL_MODE, PrtStruct.Mode, 0, OFF, 0, 0);
			}
		}*///if(StartPrintFlag)와 else 가 동작이 같아서 삭제
		ADM_Status.WaitingTimes = 4;	// 이후 4개의 데이터 무시
		if(PrtDrvStruct.GapSenErrFlag) //no gap
		{
			Operation.salePrintWait = OFF;
			status_scale.print_multiple = 0;
			sale_display_update_error(0xE605,0,0);
			PrtDrvStruct.GapSenErrFlag=OFF;
			Key_DelaySec(10,0);
			PrtDrvStruct.PrtLoadFormFlag=ON;
			label_load(status_scale.cur_labelid);	// 여러장 발행 중 중단시 라벨폼 초기화
			if(Operation.multiply)
			{
				PrtDrvStruct.PrtLoadFormFlag=ON;
				label_load(status_scale.cur_labelid);
				Operation.multiply = OFF;
			}
			return OFF;
		}
		if (PrtDrvStruct.OutOfPaperFlag)//long gap
		{
			set_nvram_bparam(NVRAM_PRT_OUTOFPAPER, ON);
			Operation.salePrintWait = OFF;
			status_scale.print_multiple = 0;
			sale_display_update_error(0xE601,0,0);
			Key_DelaySec(10,0);
			PrtDrvStruct.PrtLoadFormFlag=ON;
			label_load(status_scale.cur_labelid);	// 여러장 발행 중 중단시 라벨폼 초기화
			if(Operation.multiply)
			{
				PrtDrvStruct.PrtLoadFormFlag=ON;
				label_load(status_scale.cur_labelid);
				Operation.multiply = OFF;
			}
			return OFF;
		}
	}

	if(stat)
	{
		for(;Operation.printON && !PrtDrvStruct.PrtCompleteFlag;)
		{
			CASInterpreter(&CommBufPrt);
		}
	}

	if(GlbOper.autoDoublePrint && Operation.wAuto)
	{// auto에서 두장 발행은 정산은 한번으로
		if(status_scale.print_multiple == 1)
			stat = SaleAcctLocalAddTransaction(status_scale.Plu.deptid,status_scale.cur_pluid,&Price,0);
		else
			stat = ON;
	}
	else
	{
#ifdef USE_HYUNDAI_SET_FUNCTION
		if (Operation.saleStatus == PRINT_SET_COMPONENTS_CUSTOMER)
			stat= ON;	// 고객용 라벨은 정산에 반영 안 함 
		else
#endif
		stat = SaleAcctLocalAddTransaction(status_scale.Plu.deptid,status_scale.cur_pluid,&Price,0);
	}
	/////////////////////////////////

	if(stat)
	{
		status_scale.print_multiple--;//발행이 되면 출력라벨 줄임
	}
	else
	{
		Operation.salePrintWait = OFF;
		status_scale.print_multiple = 0;
		return OFF;	// Added by CJK 20050809
	}
	if (StartPrintFlag)	// 발행시 처음 1회
	{
		StartPrintFlag=OFF;
		if(status_scale.cur_pluid<MISC_PLU_WEIGHT)
		{
			if(Price.UnitPrice[PRICE_ORIGIN] != status_scale.Plu.unitprice)
			{
#ifdef	USE_WEIGHT_UNIT_500G
				//if(GlbOper.saveChangePrice && status_scale.cur_weightunit != 500 && PluType()!= PLU_BY_WEIGHT)
                if(GlbOper.saveChangePrice && status_scale.cur_weightunit != 500) // 중국 상해영업 요청에 의한 수정
#else
				if(GlbOper.saveChangePrice)
#endif
				{
					if(Operation.saveZeroOverride)// zero 일때도 save
					{
						if(Price.UnitPrice[PRICE_ORIGIN])
						{
							save_override_unit_price();
							status_scale.Plu.unitprice = Price.UnitPrice[PRICE_ORIGIN];
						}
					}
					else
					{
						if(status_scale.Plu.unitprice)
						{
							save_override_unit_price();
							status_scale.Plu.unitprice = Price.UnitPrice[PRICE_ORIGIN];
						}
					}
				}
			}
			if(GlbOper.saveChangeType) {
				if(status_scale.Plu.ptype != PluType())//OPTION: save changed type
				{
					save_plu_type_change();
				}
				if(Operation.fixedPrice != status_scale.Plu.fixed_price)//OPTION: save changed type
				{
					INT8U	byte;
					if (Operation.fixedAction!=2) {
						byte = OnOff(Operation.fixedPrice);
						plu_set_field(status_scale.Plu.address-1, PTYPE_FIXED_PRICE,&byte);// Save
						status_scale.Plu.fixed_price = Operation.fixedPrice;
						if (network_status.service_flag&0x20)	// send plu
						{
							commun_net_mask(0, &status_scale.Plu.address, 0xff, 1);
						}
					}
				}
			}
		}
#ifdef USE_TRACE_STANDALONE
#ifdef USE_MULTI_TRACE_NO_TABLE
		memcpy(ind_str1, CurTraceStatus.indivStr.individualNO, INDIVIDUAL_NO_LEN);
		sl_no1 = CurTraceStatus.slPlace;
		memset(ind_str2, 0, INDIVIDUAL_NO_LEN);
		sl_no2 = 0;
		memset(ind_str3, 0, INDIVIDUAL_NO_LEN);
		sl_no3 = 0;
		if (FlagIndividualMultiRemain)
		{
			switch (MultiIndex)
			{
				case 3:
					memcpy(ind_str3, IndivdualMultiData[2].indivNoStr, INDIVIDUAL_NO_LEN);
					sl_no3 = IndivdualMultiData[2].slaughterHouse;
				case 2:
					memcpy(ind_str2, IndivdualMultiData[1].indivNoStr, INDIVIDUAL_NO_LEN);
					sl_no2 = IndivdualMultiData[1].slaughterHouse;
					memcpy(ind_str1, IndivdualMultiData[0].indivNoStr, INDIVIDUAL_NO_LEN);
					sl_no1 = IndivdualMultiData[0].slaughterHouse;
					break;
			}
		}
		TNT_SetTraceNoTable((INT8U)status_scale.Plu.deptid, status_scale.cur_pluid, ind_str1, ind_str2, ind_str3, sl_no1, sl_no2, sl_no3, CurTraceStatus.meatUse, CurTraceStatus.meatPart, CurTraceStatus.indivStr.lotFlag, CurTraceStatus.gradeNo);
#else
		TNT_SetTraceNoTable_Single((INT8U)status_scale.Plu.deptid, status_scale.cur_pluid, CurTraceStatus.indivStr.individualNO, CurTraceStatus.slPlace, CurTraceStatus.meatUse, CurTraceStatus.meatPart, CurTraceStatus.indivStr.lotFlag, CurTraceStatus.gradeNo);
#endif
#ifdef USE_NHMART_SAFE_MEAT
  		if(CurTraceStatus.indivStr.individualNO[0]) SaveCalledIndiv();
#endif
#endif
	}
#ifdef USE_SETPRODUCT_SALE
	if(SetproductSaleMode && stat)
		setproduct_components_sale();
#endif
#ifdef USE_HYUNDAI_SET_FUNCTION
	if(MeatSetSaleMode && stat){
		hyundai_sale_set_components();
	}
#endif
	if(!status_scale.print_multiple)	//출력을 다했으면
	{
		if(Startup.country == COUNTRY_US)
		{
			if (MultiPrintFlag)	//Added by JJANG 20090421, motor, sensor 기존값 복구
			{	MultiPrintFlag = OFF;
			PrtDrvStruct.PeelEnable = Multi_PeelEnable;
			PrtDrvStruct.RewindEnable = Multi_RewindEnable;
			}
		}
		if(Operation.transAddup)
		{
			if(TareOper.addupWeightZero && PluType()==PLU_BY_WEIGHT && !status_scale.clerkid)	// only addup
			{
				long	pluTare=0;

				if(TareOper.tarePLU)
				{
					pluTare = TareOper.tarePLU;
					TareClearPLU();
				}
				if(status_scale.Weight > 0)
				{
					TareOper.addupWeight += (Price.Weight[PRICE_RESULT]+pluTare);
				}
			}
			CheckSubTotalStatus();
		}
		// 조건에 맞으면 클리어(PLU & screen & default setting)

		status_scale.weight_print = Price.Weight[PRICE_RESULT];	//Added by JJANG 20080617
		SaleWeightChk = OFF;

		Operation.salePrintWait=OFF;
		Operation.salePluClearWait=ON;
		Operation.transAddup = OFF;
		Operation.negative = OFF;
		Operation.transactionOK = ON;
		//		Operation.autoDoublePrint = OFF;
		if (Operation.wPrepack) Operation.salePluClearWait=OFF; // Prepack Mode
		if (Operation.wSave) Operation.salePluClearWait=OFF; // Save Mode
		if (printdateOFF) printdateOFF = OFF;
		if(status_scale.clerkid && GlbFloat.logOffTransaction)//OPTION: transaction log off
		{
			// && !status_scale.cur_pluid)// prepack,Auto에서는 log off 하지 않음
			if(Operation.wPrepack || Operation.wAuto){}
			else
				logout();
		}
#ifdef USE_TRACE_STANDALONE
		//[start]판매후 초기화 값
		//MultiIndex = 0;
		FlagMultiStart = 0;	//복수개체 시작키로 시작값
		DoublePLUPrevSkey = 0;	//double togle key state
#endif
#ifndef USE_EMART_BACK_LABEL	//prepack 사용시 해제 되면 안됨.
		BarcodeNumber = 0;	//임시라벨 포맷에 적용된 바코드번호
#endif
		packed_date = 0;	//1회 판매 후 재포장 일자 (GS일반도 가공일변경 1회키 사용함.)
#ifdef USE_PRICE_CHANGE_PROHIBITED
		if((Operation.wAuto == OFF) && (Operation.wPrepack == OFF))
		{
			PressedNumberKey_1000Digits = 0;
			status_scale.Plu.sales_msg = 0;
		}
#endif	
#ifdef USE_TOPMART_DEFAULT //sewon 170123
		if (!Operation.wPrepack)
		{
			InputQtyFlag = OFF;
			InputQty =1;
		}
#endif
		//[end]판매후 초기화 값
		if (Operation.multiply)	{
			PrtDrvStruct.PrtLoadFormFlag=ON;
			label_load(status_scale.cur_labelid);
			Operation.multiply=OFF;
		}
		if (Operation.autoDoublePrint) {
			PrtDrvStruct.PrtLoadFormFlag=ON;
			label_load(status_scale.cur_labelid);
			Operation.autoDoublePrint = OFF;
		}
#ifdef USE_CHN_CART_SCALE
		if (Operation.oneTouchTicketPrint)
		{
			keyapp_total(ON);
			Operation.oneTouchTicketPrint = OFF;
		}
#endif
#ifdef USE_KOR_TRACE
		KorTrace_Flag.flagPrint = 0;	// 저울 offline 발행시 인쇄완료되었을 때 초기화함
#endif
#ifdef USE_INVENTORY_LABEL
		InventoryUnitWeight = 0;
#endif
#ifdef USE_EMART_PRODUCTION_KEY
		if(BackupCheckin == 1)	//이마트 차세대2012, PLU 999999를 마스터 업데이트 정보 저장에 사용. 생산등록X
		{
			Operation.checkin = 1;
			BackupCheckin = 0;
		}
#endif
#ifdef USE_REGIST_DISPOSAL
		if(Operation.RegistDisposalFlag)
		{
			Operation.RegistDisposalFlag = OFF;
			sale_display_disposal(Operation.RegistDisposalFlag);
			sale_pluclear(ON);
			sale_display_update(UPDATE_ALL|UPDATE_CLEARALL);
		}
#endif
	}
#ifdef USE_HYUNDAI_SET_FUNCTION
	if(Operation.saleStatus == PRINT_SET_COMPONENTS_CLERK)
		Operation.saleStatus = PRINT_SET_COMPONENTS_CUSTOMER;
#endif
	sale_display_update(UPDATE_TITLE);
	return ON;
}

INT8U	LinkedPrintFlag = OFF;
INT16S LinkedLabelPrint(void)
{
	LinkedPrintFlag = ON;
	while(1)
	{
		network_check();
		if(KEY_Read())
			break;
		if(Operation.printON)
		{
			if(!CheckPeelHead(ON, OFF))	continue;
			Prt_PrintStart(PRT_ITEM_NORMAL_MODE, PrtStruct.Mode, 0, OFF, 0, 0);
		}

		if(PrtDrvStruct.GapSenErrFlag) //no gap
		{
			sale_display_update_error(0xE605,0,0);
			PrtDrvStruct.GapSenErrFlag=OFF;
			Key_DelaySec(10,0);
		}
		if (PrtDrvStruct.OutOfPaperFlag)//long gap
		{
			set_nvram_bparam(NVRAM_PRT_OUTOFPAPER, ON);
			sale_display_update_error(0xE601,0,0);
			Key_DelaySec(10,0);
		}
		break;
	}
	LinkedPrintFlag = OFF;
	return ON;
}

void sale_print_cancel(void)
{
	char prt_cmd[10];

	prt_cmd[0] = GS;
	prt_cmd[1] = 'E';
	PutDataRxBuf(&CommBufPrt,prt_cmd,2);
}

////#define	TARE_TYPE_WEIGHT	1
////#define	TARE_TYPE_KEY		2
////#define	TARE_TYPE_PLU		3
INT8U	AutoClearTare=OFF;
void sale_auto_clear(void)
{
	//	INT16S    sz;
	if(Operation.salePluClearWait)
	{		 // End Print
		if(!Operation.directClearWeight)	//PARAMETER 580
		{
			if(!GlbOper.weightDirectClear)	//MENU 1873 "PLU clr Aft sale(WT.OnTray):"
			{
			    if (PluType()==PLU_BY_WEIGHT && 0L < status_scale.Weight)	return;
			}
		}
		//new
		//if(Operation.wAuto && !Operation.wSave && PluType()==PLU_BY_WEIGHT)
		//	Key_DelaySec(8,0);

		sale_pluclear(OFF);
		//sale_display_update(UPDATE_MODE|UPDATE_STATE|UPDATE_STATUS|UPDATE_RIDE);
		//sale_display_update(0x0fff);
		sale_display_update(0x0fff);
		Operation.salePluClearWait=OFF;
		if(Operation.directClearWeight)		Operation.directClearWeight=OFF;
		AutoClearTare = ON;
	} else {
		if (!Operation.salePrintWait) {
			if (Operation.transactionOK) {
				//				if (status_scale.cur_pluid) {
				//					plu_get_field(status_scale.Plu.address-1, PTYPE_FIXED_PRICE, (INT8U *)&status_scale.Plu.fixed_price,&sz,sizeof(status_scale.Plu.fixed_price));// fixed price
				//				}
				//if((!Operation.wPrepack && !Operation.wAuto))
				if(!Operation.wPrepack){
#ifndef USE_EMART_PRODUCTION_KEY
					Operation.checkin = OFF;	// 생산 등록 해제 : 마지막장 출력 후, prepack일때는 해제 안됨
#endif
#ifdef USE_EMART_BACK_LABEL
					Operation.BackLabeledFlag = OFF;	// 보관 라벨 선택 flag 해제.
					LabelNumber = 0;
					BarcodeNumber = 0;
#endif
					sale_display_update(UPDATE_MODE);
					if (status_scale.Plu.fixed_price==0) Operation.fixedPrice=0;
					switch(status_scale.cur_pluid)
					{
						case MISC_PLU_WEIGHT :
							keyapp_misc_byweight(OFF);
							break;
						case MISC_PLU_COUNT :
							keyapp_misc_bycount();
							break;
						case MISC_PLU_PCS :
							keyapp_misc_bypcs(OFF);
							break;
						default :
							keyapp_pluno((INT8U)status_scale.departmentid,status_scale.cur_pluid,OFF);
							break;
					}
#ifdef USE_PLU_MULTIPLE_PRICE
					if(!status_scale.Plu.first_price && !status_scale.Plu.second_price)
					{
						keyapp_cleardiscount();	//PLU Save 시 Discount 유지
					}
#else
					keyapp_cleardiscount();
#endif
					//					sale_display_update(UPDATE_UNIT);
				}
			}
		}
	}
	if(Operation.transactionOK && AutoClearTare)
	{
		if(ADM_GetData.Stable)
		{
			//DisplayMsg("222");
			if(TareOper.clearWeightTare && TareOper.tareWeight)
			{
				//if(ADM_GetData.Zero || (TareOper.tareWeight+status_scale.Weight) == 0)
				if(TareOper.tareWeight+status_scale.Weight == 0)
				{
					INT16S stat;
					//AutoClearTare = TareClearWeight();
					stat = keyapp_tare(TARE_TYPE_WEIGHT,0, OFF);
					if(stat)	AutoClearTare = OFF;
				}
				//DisplayMsg("111");
			}
			else if(TareOper.clearKeyTare && TareOper.tareKey)
			{
				if(status_scale.Weight < 1L)
				{
					//DisplayMsg("Clear Tare");
					//Key_DelaySec(10,0);
					TareClearKey();
					AutoClearTare = OFF;
				}
			}
		}
	}
#ifndef USE_PLU_MULTIPLE_PRICE	//Refefence PLU 기능 사용 안 함
	if(Operation.transactionOK && status_scale.Plu.coupled_dept && status_scale.Plu.coupled_plu)
	{
		keyapp_pluno(status_scale.Plu.coupled_dept,status_scale.Plu.coupled_plu,OFF);
	}
#endif


}

////void sale_plu_clear(void) // Deleted by CJK 20050927
////{
////	if (status_scale.cur_pluid==0) return;
////	status_scale.print_multiple = 0;
////	status_scale.cur_pluid    = 0;
////	status_scale.tare_type    = 0;
////	status_scale.Plu.tare     = 0;
////	status_scale.cur_unitprice= 0;
////	status_scale.sale_type    = 0;
////	ad_set_presettare(0);
////	sale_display_update(0xfff);
////}

void sale_auto_print(void)
{
	status_scale.print_multiple=1;
	status_scale.flag_display|=0x02;
}

extern INT8U display_reason_code_input_page(void);

void sale_auto_check(void) // LP-II
{
	INT8U changeFlag;
#if defined (USE_CHECK_CHANGE_REASON) || defined(USE_HYUNDAI_CHECK_REASON_CODE)
	INT8U askReason;
#endif
	if (PluType()!=1) return;

	if (!Operation.wAuto)	return;
	if (GlbOper.autoDoublePrint)	return;     //yoo  080604 추가 ..autoDoublePrint가 on 이면, auto 기능을 막음

	if (status_scale.Weight < 1)
	{
		if (status_scale.weight_stable)
			status_scale.weight_stable=0;
	}
	if(GlbOper.PrintWhenchangingPLUInAUTO)
	{
		if ((PrvPrtPluId == status_scale.cur_pluid)&& status_scale.weight_stable) return;
	}
	else
	{
		if (status_scale.weight_stable) return;
	}
	if (GlbOper.PrintZeroTotalPrice && (status_scale.weight_min > status_scale.Weight)) return; 			// param 574 is Y (Allow Print Zero Total Price)인 경우 최소중량미만에서 AUTO 인쇄 안함.
	if (GlbOper.PrintUnderMinWeightOnAUTO && (status_scale.weight_delta > status_scale.Weight)) return;	// param 783 is Y (Print Under Min.Weight on Auto)인 경우에 10d 미만에서 인쇄 안함(status_scale.weight_delta = 10d)
	//if ((ADM_GetData.Stable) && (status_scale.Weight>0)) {
	//if ((ADM_GetData.Stable) && ((status_scale.weight_min <= status_scale.Weight) || status_scale.Plu.fixed_weight))

	if (ADM_GetData.Stable && (status_scale.weight_min <= status_scale.Weight || GlbOper.PrintUnderMinWeightOnAUTO)&& status_scale.Weight > 0)
	{
		status_scale.weight_stable=status_scale.Weight;
		if (CheckPluPrint(0))
		{
#ifdef USE_CHECK_CHANGE_REASON
			askReason = get_global_bparam(GLOBAL_SALE_SETUP_FLAG12) & 0x10;
			if(askReason)
			{
				changeFlag = GetChangeFlag(PluType(), status_scale.Plu.ptype, Price.UnitPrice[PRICE_RESULT], status_scale.Plu.unitprice, Operation.fixedPrice, status_scale.Plu.fixed_price, Price.TotalPrice[PRICE_SAVED]);
				if (UseEmartNetwork && changeFlag)
				{
					if(!display_reason_code_input_page()) return;
				}
			}
#elif defined(USE_HYUNDAI_CHECK_REASON_CODE)
			askReason = get_global_bparam(GLOBAL_SALE_SETUP_FLAG12) & 0x10;
			if(askReason && (status_scale.Plu.sales_msg != 9) && !Operation.meatStorageLabelFlag)
			{
				changeFlag = GetChangeFlag(PluType(), status_scale.Plu.ptype, Price.UnitPrice[PRICE_RESULT], status_scale.Plu.unitprice, Operation.fixedPrice, status_scale.Plu.fixed_price, Price.TotalPrice[PRICE_SAVED]);
				if (changeFlag || Operation.RegistDisposalFlag) {
					if(!display_reason_code_input_page()) return;
				}
			}
#endif
#ifdef USE_CHECK_INDIV_INFO
			if(!CheckIndivInfo()) return;
#endif
#ifdef USE_KOR_TRACE
			keyapp_ktr_print();
#else
			PrintLabelWait();
			status_scale.print_multiple=1;
			StartPrintFlag = ON;
			status_scale.cur_keyvalue=0;	// quick plu사용시 auto일때 입력한숫자가 unit price에 남는 문제 처리
#endif
			PrvPrtPluId = status_scale.cur_pluid;
		}
	}
}

#define STATE_AZ_IDLE     1
#define STATE_AZ_WAIT     2
#define STATE_AZ_SET_ZERO 3
void sale_auto_zero(void)
{
	static INT8U state = STATE_AZ_IDLE;
	static INT16U start_timer = 0;

	switch (state)
	{
		default:
		case STATE_AZ_IDLE:
			if (!GlbOper.UseAutoZero) break;
			if (ADM_GetData.Stable)
			{
				if (status_scale.Weight < 0)
				{
					start_timer = SysTimer100ms;
					state = STATE_AZ_WAIT;
				}
			}
			break;
		case STATE_AZ_WAIT:
			if (!ADM_GetData.Stable) state = STATE_AZ_IDLE;
			else if (status_scale.Tare) state = STATE_AZ_IDLE;
			else if (status_scale.Weight >= 0) state = STATE_AZ_IDLE;
			else if ((INT16U)(SysTimer100ms - start_timer) > (INT16U)50) state = STATE_AZ_SET_ZERO;
			break;
		case STATE_AZ_SET_ZERO:
			ADM_SendCommand(ADM_CMD_SET_ZERO, 0, ADM_PARAM_OFF);	// Set Zero
			state = STATE_AZ_IDLE;
			break;
	}
}

extern INT16S ClerkSearchEmpty(void);
long	WeightMaxDelta=0;
void sale_prepack_check(void)
{
	//	INT8U sale_mode;
	long delta_w;
	INT8U changeFlag;
	//	long gloss_weight;
#if defined(USE_CHECK_CHANGE_REASON) || defined(USE_HYUNDAI_CHECK_REASON_CODE)
	INT8U askReason;
#endif

	if (PluType()!=1) return;

	if (!Operation.wPrepack)	return;

	if (PeelHeadMsgFlag && status_scale.Weight <= 0)
	{
		//DEL070705		DispClearPeelHeadMsg();
		CheckPeelHead(OFF, ON);
		Operation.salePrintWait = OFF;
		status_scale.print_multiple  = 0;
	}

	//msg
	//sprintf(gmsg,"1 MIN %ld,THERES %ld",status_scale.weight_min,status_scale.weight_delta);
	//DisplayMsg(gmsg);

	delta_w=_labs(status_scale.Weight,status_scale.weight_stable);

	if(WeightMaxDelta < delta_w)	WeightMaxDelta = delta_w;

	if (GlbOper.PrintUnderMinWeightOnPrepack)
	{
		if  (!Operation.fixedPrice && status_scale.Weight <= 0) return;
	}
	else
	{
		if (!Operation.fixedPrice && status_scale.Weight < status_scale.weight_min) return;
	}

	//msg
	//sprintf(gmsg,"2 MIN %ld,THERES %ld",status_scale.weight_min,status_scale.weight_delta);
	//DisplayMsg(gmsg);
	if (ADM_GetData.Stable)
	{
		if( !Operation.salePrintWait && (status_scale.weight_delta<=WeightMaxDelta && status_scale.weight_delta <= status_scale.Weight) && CheckPluPrint(0))
		{
#ifdef USE_CHECK_CHANGE_REASON
			askReason = get_global_bparam(GLOBAL_SALE_SETUP_FLAG12) & 0x10;
			if(askReason)
			{
				changeFlag = GetChangeFlag(PluType(), status_scale.Plu.ptype, Price.UnitPrice[PRICE_RESULT], status_scale.Plu.unitprice, Operation.fixedPrice, status_scale.Plu.fixed_price, Price.TotalPrice[PRICE_SAVED]);
				if (UseEmartNetwork && changeFlag)
				{
					if(!display_reason_code_input_page()) return;
				}
			}
#elif defined(USE_HYUNDAI_CHECK_REASON_CODE)
			askReason = get_global_bparam(GLOBAL_SALE_SETUP_FLAG12) & 0x10;
			if(askReason && (status_scale.Plu.sales_msg != 9) && !Operation.meatStorageLabelFlag)
			{
				changeFlag = GetChangeFlag(PluType(), status_scale.Plu.ptype, Price.UnitPrice[PRICE_RESULT], status_scale.Plu.unitprice, Operation.fixedPrice, status_scale.Plu.fixed_price, Price.TotalPrice[PRICE_SAVED]);
				if (changeFlag || Operation.RegistDisposalFlag) {
					if(!display_reason_code_input_page()) return;
				}
			}
#endif
#ifdef USE_CHECK_INDIV_INFO
			if(!CheckIndivInfo()) return;
#endif
			status_scale.weight_stable=status_scale.Weight;
			WeightMaxDelta = 0;
			if (Operation.addStandatdClerk )
			{
				if(ClerkSearchEmpty() == -1) {
					caption_split_by_code(0xC90D,&cap,0);
					DisplayMsg(cap.form);
					Key_DelaySec(10,0);
					return;
				}
				Operation.transAddup = ON;
			}
#ifdef USE_KOR_TRACE
			keyapp_ktr_print();
#else
			PrintLabelWait();
			status_scale.print_multiple=1;
			StartPrintFlag = ON;
			status_scale.cur_keyvalue=0;	// quick plu사용시 auto일때 입력한숫자가 unit price에 남는 문제 처리
#endif	
		}
	}
}

//Added by JJANG 20080617
void sale_weight_check(void)
{
	long delta_w;

	delta_w=_labs(status_scale.Weight,status_scale.weight_print);
	if (status_scale.weight_delta <= delta_w)
	{
		SaleWeightChk = ON;
	}
}


// return value : ON  = 무게 데이터 수신시. overload 시
//                OFF = A/D 데이터 미수신시. 커맨드의 결과 수신시.
INT8U sale_adm_proc(void)
{
	static	long	PrevAddupTareWeight=0;
	INT8U unit;
#ifdef USE_KOR_TRACE
	static INT8U check_stable = 0; // send_period = 0;
#endif
	INT8U ret;
	INT8U wtDec;

	ret = OFF;
  	if (ADM_RecvProc())
  	{
		if(ADM_Status.WaitingTimes) {
			ADM_Status.WaitingTimes--;
			return OFF;
		}
		if(ADM_GetData.Overload) {
			sale_display_update_error(0xE501,0,0);
			sale_display_update(UPDATE_WEIGHT|UPDATE_PRICE|UPDATE_STATUS);
			return ON;
		}

		if(ADM_GetData.DataMode == ADM_DATAMODE_WEIGHT_KG || ADM_GetData.DataMode == ADM_DATAMODE_WEIGHT_LB) {
			if(ADM_GetData.DataMode == ADM_DATAMODE_WEIGHT_KG) {
				unit = WEIGHT_UNIT_KG;
				if(get_base_bparam(FLASH_ADC_WUNIT) == WEIGHT_UNIT_G) unit = WEIGHT_UNIT_G;
			} else {
				unit = WEIGHT_UNIT_LB;
			}
			wtDec = get_weight_decimal(ADM_Status.Capa, ADM_Status.Dual, unit);
			change_weight_decimal(wtDec);
			status_scale.Weight = ad_cnv_adm2main(ADM_GetData.RecvData) - TareOper.addupWeight;
			ret = ON;
		}
		if(ADM_GetData.ChangeData || (PrevAddupTareWeight != TareOper.addupWeight)) {
			PrevAddupTareWeight = TareOper.addupWeight;
			ADM_GetData.ChangeData = 0;
			sale_display_update(UPDATE_WEIGHT|UPDATE_PRICE|UPDATE_STATUS);
#ifdef USE_USA_FIXEDWEIGHT_UNIT_OZ
			if(GlbOper.byCountWeight) sale_display_update(UPDATE_TITLE);
#endif
		}
#ifdef USE_ZERO_RANGE_MSG
		if(Zerokey_input_Flag)
		{
			Zerokey_input_Flag = OFF;
			if(ADM_GetData.Stable && !ADM_GetData.Zero)
			{
				caption_split_by_code(0x9C86,&cap,0);
				DisplayMsg(cap.form);
			}
		}
#endif

#ifdef USE_KOR_TRACE
		if (network_status.service_bflag2==2) {

			if(check_stable && ADM_GetData.Stable == 1) {
				if(check_stable++>2) {
					check_stable = 0;
					//send_period = 0;
					KorTrace_Flag.flagSendWeight = 1;
					//KorTrace_Flag.err_cnt=0;
				}
			}
			else
			{
				if(ADM_GetData.PrevStable == 0 && ADM_GetData.Stable == 1)
				{
					check_stable = 1;
				}
				else if(ADM_GetData.PrevStable == 1 && ADM_GetData.Stable == 1)
				{
					if(ADM_GetData.PrevRecvData!=ADM_GetData.RecvData)
						check_stable = 1;
					else 	check_stable = 0;
				}
				else
				{
					check_stable = 0;
				}
			}
		}
#endif
#ifdef USE_CTS_FUNCTION
		if (!ADM_GetData.Stable) status_scale.ad_last_used_100ms = SysTimer100ms;
#endif
	}
	return ret;
}

/*
 void sale_send_weight(void)
 {
 static long weight=-9999l;
 if (weight==Price.Weight[PRICE_RESULT]) return;
 weight = Price.Weight[PRICE_RESULT];
 if (network_status.service_bflag2==1) {
 commun_getinfo(&CommBuf,0x00,0x60,0);
 }
 if (network_status.service_bflag2==2) {
 if (Ethernet_Server(CLIENT_OPEN, 0, CLIENT_NORMAL)) {
 commun_getinfo(&(CommBufEthData[0]),0x00,0x60,0);
 Ethernet_Server(CLIENT_CLOSE, 0, CLIENT_NORMAL);
 }
 }
 }
 */
//////////////////
// mode
//   1 : CAS 22bytes
void sale_send_weight(void)
{
	INT8U w_sign1[5];
	INT8U string_buf[30];
	static INT16U sendWeightTime = 0;
	INT8U len;
	INT8U lampState;
	INT8U dec;
	INT8U dec_ch;
	long  weight;
	INT8U negative;
	INT8U i;

	if (network_status.service_bflag2==3)
	{
		if(CheckTimeOver(2, sendWeightTime))
		{
			sendWeightTime = SysTimer100ms;
			lampState = 0xa0;	// default  1010 0000
			lampState |= 0x04;	// Net weight
			if (ADM_GetData.Stable)
			{
				memcpy(string_buf, "ST", 2);
				lampState |= 0x40;	// STABLE
			}
			else
			{
				memcpy(string_buf, "US", 2);
			}
			if (ADM_GetData.Overload)
			{
				memcpy(string_buf, "OL", 2);
			}
			if (ADM_GetData.Zero)
			{
				lampState |= 0x01;	// ZERO
			}
			if (ADM_GetData.Tare)
			{
				lampState |= 0x02;	// NET
			}

			len = 2;
			string_buf[len++] = ',';
			memcpy(&string_buf[len], "NT", 2);	// net weight
			len += 2;
			string_buf[len++] = ',';
			string_buf[len++] = status_scale.scaleid;	// Device ID
			string_buf[len++] = lampState;
			string_buf[len++] = ',';
			GetWeightSymbol(ADM_GetData.CurUnit, (char *)w_sign1);
			dec = get_decimal_pt(0, 0);
			dec_ch = get_global_bparam(GLOBAL_DECIMAL_SIGN);
			weight = status_scale.Weight;
			negative = OFF;
			if (weight < 0)
			{
				weight *= -1;
				negative = ON;
			}
			if (dec)
			{
				format_fixed(&string_buf[len], weight, dec, 8, dec_ch);
			}
			else
			{
				format_fixed(&string_buf[len], weight, dec, 7, dec_ch);
				string_buf[len + 7] = ' ';
			}
			if (negative) string_buf[len] = '-';
			if (ADM_GetData.Overload)
			{
				for (i = 0; i < 8; i++) string_buf[len + i] = '-';
			}
#ifdef USE_UNDER_LIMIT_MINUS20D
			if(Operation.UnderWeight == ON)
			{
				for (i = 0; i < 8; i++) string_buf[len + i] = '-';
			}
#endif
			len += 8;

			string_buf[len++] = ' ';
			strcpy((char *)&string_buf[len], (char *)w_sign1);
			len += 2;
            string_buf[len++] = _CR;
			string_buf[len++] = LF;
			commun_outleng(&CommBuf,(char *)string_buf,len);
		}
	}
}

void sale_key_time_reset(void)
{
	status_scale.cur_time_key=SysTimer100ms;
}

INT16U sale_key_get_action(void)
{
	INT32U pnt;
	INT16U  c16;
	INT16U cc;

	pnt =DFLASH_BASE;
	pnt+=FLASH_GLOBAL_AREA;
	c16=Flash_wread(pnt+GLOBAL_KEY_TIMEOUT_ACTION);
	//cc = keyin_getrawcode(c16);
	cc = keyin_getrawcode_bytype(0, c16);	// Modified by CJK 20060315

	return cc;
}

INT8U sale_key_time_clear(void)
{
	INT16U  time_key_clear;
	INT8U   cc;

	if (!KeyDrv.FirstKey) {
		cc = get_global_bparam(GLOBAL_KEY_TIMEOUT_KEYPRESS);
		time_key_clear = (INT16U)cc;
		if (time_key_clear) {
		    if ((INT16U)(SysTimer100ms-status_scale.cur_time_key)>time_key_clear) {
				status_scale.cur_time_key=SysTimer100ms;
				return 1;
		    }
		}
	}
	return 0;
}

//DEL CJK070319void sale_err_time_reset(void)
//DEL CJK070319{
//DEL CJK070319	status_scale.cur_time_err=SysTimer100ms;
//DEL CJK070319}
//DEL CJK070319
//DEL CJK070319INT8U sale_err_time_clear(void)
//DEL CJK070319{
//DEL CJK070319	INT16U  time_err_clear;
//DEL CJK070319	INT8U   cc;
//DEL CJK070319
//DEL CJK070319	cc = get_global_bparam(GLOBAL_KEY_TIMEOUT_ERROR);
//DEL CJK070319	time_err_clear = (INT16U) cc;
//DEL CJK070319	if (time_err_clear) {
//DEL CJK070319	    if (status_scale.flag_display&0x04) {
//DEL CJK070319		if ((SysTimer100ms-status_scale.cur_time_err)>time_err_clear) {
//DEL CJK070319			status_scale.cur_time_err=SysTimer100ms;
//DEL CJK070319			return 1;
//DEL CJK070319		}
//DEL CJK070319	    }
//DEL CJK070319	}
//DEL CJK070319	return 0;
//DEL CJK070319}

INT16U KeyWait(INT16U key1,INT16U key2,INT16U key3,INT16U key4, INT16U key5)
{
	INT16S flag=ON;

	while(flag)
	{
		if (KEY_Read())
		{
			BuzOn(1);
			if(KeyDrv.Type == KEY_TYPE_FUNCTION)
			{
				switch(KeyDrv.CnvCode)
				{
					case KS_PRINT :
					case KS_SAVE :
					case KS_MULTI :
					case KS_CLEAR :
						flag = OFF;
						break;
					case KS_EXT_PRINT:
						ExtPrtTotalFlag =ON;
						flag = OFF;
						break;
					case KS_PAY_1:
					case KS_PAY_2:
					case KS_PAY_3:
					case KS_PAY_4:
					case KS_PAY_5:
					case KS_PAY_6:
					case KS_PAY_7:
						if(key1==KS_PAY || key2==KS_PAY || key3==KS_PAY || key4==KS_PAY)
							flag = OFF;
						break;
					default:
						if(KeyDrv.CnvCode == key1)
						{
							flag = OFF;
							break;
						}
						if(KeyDrv.CnvCode == key2)
						{
							flag = OFF;
							break;
						}
						if(KeyDrv.CnvCode == key3)
						{
							flag = OFF;
							break;
						}
						if(KeyDrv.CnvCode == key4)
						{
							flag = OFF;
							break;
						}
						if(KeyDrv.CnvCode == key5)
						{
							flag = OFF;
							break;
						}
						break;
				}
			}
			if(KeyDrv.Type == KEY_TYPE_CURRENCY)
			{
				//keyapp_currency(1,1,Price.DisplayedPrice,KeyDrv.CnvCode);
				CurrencyPrice.CurrencyKeyCode = KeyDrv.CnvCode;
				flag = OFF;
			}
		}
	}
	// KEY_SetMode(old_key);
	return KeyDrv.CnvCode;
}

/*
 //INT16S KeyWaitYesNo(void)
 {
 INT16S flag=ON;

 while(flag)
 {
 if (KEY_Read())
 {
 BuzOn(1);
 if(KeyDrv.Type == KEY_TYPE_NUMERIC)
 {
 switch(KeyDrv.CnvCode)
 {
 case KEY_NUM_0 :
 case KEY_NUM_1 :
 flag = OFF;
 default:
 break;
 }
 }

 }
 }
 if(KeyDrv.CnvCode==KEY_NUM_0)	return OFF;
 return ON;
 }
 */

INT8U	ShiftCnvCode=0;
#ifdef USE_TRACE_STANDALONE
INT8U	FuncKeyInputFlag = OFF;
#endif
extern INT16U keyapp_togleplukey(INT16U skeyplu, INT8U settingKey);
extern INT16U  keyapp_doubleplukey(INT16U skeyplu, INT8U settingKey);
void sale_key_proc(void)
{
	INT16S	keyFlag=ON;
	INT8U   ret_value;
	INT16U  input_number;
	INT32U	pluid=0;
#ifdef USE_TOPMART_DEFAULT
	INT8U uctemp, uctemp2;
#endif

	if (KEY_Read())
	{
#ifndef NOT_USE_NETSET_LOCK
		if (status_scale.scale_lock)
		{
			switch(KeyDrv.Type)
			{
				case KEY_TYPE_FUNCTION:
					BuzOn(1);
					if (KeyDrv.CnvCode==KS_CLEAR)
					{
						if (permission_check(PMS_SCALE_LOCK,0))
						{
							status_scale.scale_lock=0;
							set_nvram_bparam(NVRAM_SCALE_LOCK,0);
							sale_display_update(0x0fff);
						}
					}
				default : BuzOn(1); break;
			}
			return;
		}
#endif
		if(status_scale.clerkid)// 키를 누르면 log out 지연 초기화
			ClerkLoginTime = SysTimer100ms;

		sale_display_update(UPDATE_UNIT|UPDATE_PRICE);
#ifdef USE_CL5200_DISPLAY_DEFAULT //cl5200에서  plu 호출하지 않은 상태에서 키 입력시 tare 사라지는 버그 수정
		sale_display_update(UPDATE_TARE);
#endif
		// 이전키가 KEY_TYPE_NUMERIC가 아니면
		// 또는 KEY_TYPE_NUMERIC키가 시간이 지나면 현재 입력하고 있는 값을 클리어 하고 새로 시작
		sale_key_time_reset();
		// key control

		//Added by JJANG 20080131 Menu에만 진입할 수 있도록.
		if(get_nvram_bparam(NVRAM_SLAVECHECK_FLAG))
		{
			if(KeyDrv.CnvCode!=KS_MODE) return;
		}
		if(get_nvram_bparam(NVRAM_TRANS_BACKUP_FLAG))	// local backup 있을 때
		{
			if(ethernet_list.status == 2 && Operation.operationClerk == OPER_FLOATING)
			{
				if(KeyDrv.CnvCode!=KS_MODE) return;
			}
		}

		if(Operation.salePrintWait)
		{
			switch(KeyDrv.Type)
			{
				case KEY_TYPE_FUNCTION :
					switch(KeyDrv.CnvCode)
					{
						case KS_CLEAR :
							PrtDrvStruct.PrtLoadFormFlag=ON;
							label_load(status_scale.cur_labelid);
							Operation.salePrintWait = OFF;
							break;
						default:
							keyFlag=OFF;
							break;
					}
					break;
				default:
					keyFlag=OFF;
					break;

			}
		}
		if(Operation.multiply)
		{
			keyFlag=OFF;
			switch(KeyDrv.Type)
			{
				case KEY_TYPE_FUNCTION :
					switch(KeyDrv.CnvCode)
					{
						case KS_MULTI :
							keyFlag=ON;
							//if(status_scale.print_multiple > 0) {// Added by CJK 20050603
							//	keyFlag=OFF;
							//}
							break;
						case KS_CLEAR :
						case KS_PRINT :
							keyFlag=ON;
							break;
						default:
							keyFlag=OFF;
							break;
					}
					break;
				case KEY_TYPE_NUMERIC :
					keyFlag=ON;
					break;
				case KEY_TYPE_CLERK :
					if (Operation.operationClerk!=OPER_NORMAL)	keyFlag=ON;
					//sprintf(gmsg,"[F %d]-",keyFlag);
					//DisplayMsg(gmsg);
					//Key_DelaySec(5,0);
					break;
				default:
					break;

			}
		}
		//sprintf(gmsg,"KeyFlag[%d]",keyFlag);
		//MsgOut(gmsg);
#ifdef _USE_UNDER_SELFKEY_
		if(Operation.operationMode == MODE_SELF)
		{
			switch(KeyDrv.Type)
			{
				case KEY_TYPE_FUNCTION :
				case KEY_TYPE_NUMERIC :
				case KEY_TYPE_SPEEDPLU:
					ret_value = key_selfmenu();
					if(KeyDrv.CnvCode==KS_PRINT)
					{
						if(ret_value == 0) keyFlag = OFF;	//Modified by JJANG 20070801
						break;

					}
					if(Selfkey_flag == ON) break;			//Added by JJANG 20070801
					if(KeyDrv.CnvCode==KS_CLEAR) break;		//Added by JJANG 20070516
					if((KeyDrv.Type != KEY_TYPE_SPEEDPLU) || (ret_value==0))
						keyFlag = OFF;
					break;
					//case KEY_TYPE_NUMERIC :
					//	if(Selfkey_flag == ON) keyFlag = ON;
					//	else keyFlag = OFF;
					//	break;
				default:
					keyFlag = OFF;
			}
		}
#else
		if(Operation.operationMode == MODE_SELF)
		{
			if (get_nvram_bparam(NVRAM_SELF_KEY_BLOCK_ON))
			{
				keyFlag = OFF;
				switch(KeyDrv.Type)
				{
					case KEY_TYPE_FUNCTION:
						switch (KeyDrv.CnvCode)
						{
							case KS_MODE:
#ifdef NOT_USE_SOME_KEYS_IN_SELFSERVICE
								input_number = hex2bcd_word((INT16U)status_scale.cur_keyvalue);
								if(input_number == 0x88)
								{
									keyFlag = ON;
								}
								else
								{
									keyFlag = OFF;
								}
								break;
#endif
							case KS_ZERO:
							case KS_AUTO:
#ifndef NOT_USE_SOME_KEYS_IN_SELFSERVICE
							case KS_SAVE:
#endif
							case KS_PRINT:
							case KS_CLEAR:
							case KS_FEED:
							case KS_BARCODE:
                            case KS_GLOBAL_TAX:
#ifdef NOT_USE_SOME_KEYS_IN_SELFSERVICE
							case KS_MISC_WEIGHT:
							case KS_MISC_COUNT:
							case KS_MISC_PCS:
#endif
								keyFlag = ON;
								break;
						}
						break;
					case KEY_TYPE_TARE:
					case KEY_TYPE_NUMERIC:
					case KEY_TYPE_SPEEDPLU:
						keyFlag = ON;
						break;
				}
			}
		}

#endif //_USE_UNDER_SELFKEY_





		//sprintf(gmsg,"[F %d]",keyFlag);
		//DisplayMsg(gmsg);
		if(keyFlag)
		{
			DisplayMsgReset();
			switch(KeyDrv.Type)
			{
				case KEY_TYPE_CLERK :
					if (InputQuickPluState && ReadyQuickPlu)
					{
						keyapp_pluno(status_scale.departmentid, status_scale.cur_quickplu, OFF);
						sale_display_update(UPDATE_DATETIME);
						CallQuickPlu = OFF;
						ReadyQuickPlu = OFF;
					}
				case KEY_TYPE_FUNCTION :
				case KEY_TYPE_LABEL :
#ifdef USE_TRACE_STANDALONE
					FuncKeyInputFlag = ON;
 					key_function();
					FuncKeyInputFlag = OFF;
#else
 					key_function();
#endif
					break;
				case KEY_TYPE_NUMERIC :
					key_numeric();
					break;
				case KEY_TYPE_SPEEDPLU :
#ifdef USE_HYUNDAI_SET_FUNCTION
					if(MeatSetSaleMode && MeatSetSaleUseScanner) {
						BuzOn(2);
						status_scale.cur_keyvalue=0;
						KeyDrv.FirstKey = 1;
						break;
					}
#endif
#ifdef USE_TOPMART_DEFAULT //sewon 170117
					InputQtyFlag = OFF;
					InputQty =1;
					mPackDate.deltadays = 0;
					PackdatePrintFlag = 0;
					
					uctemp = get_global_bparam(GLOBAL_SALE_SETUP_FLAG6);
					uctemp2 = (uctemp>>1)& 0x01;
					if(!uctemp2)
					{
						uctemp |= 0x02;
						//DisplayMsg("DATE-PRINT ON");
						set_global_bparam(GLOBAL_SALE_SETUP_FLAG6, uctemp);
					} 
#endif
					SpeedKeyPressed = 1; //csh 20140304
#ifdef USE_LOTTEMART_MEATSCANNER //20171206
					lottemart_meatscanner_initial();
#endif
					pluid = keyapp_plukey(KeyDrv.CnvCode);
					if(GlbOper.send_speedkey_position && pluid && KeyDrv.comm_key == 0)
						command_send_speedkey_value("P", pluid);
					status_scale.cur_keyvalue=0;
					KeyDrv.FirstKey = 1;
#ifdef USE_LONGPRESS_KEY_OVERRIDE
                    if(keyLongkeyIn)
                    {
                        keyLongkeyIn = OFF;
                        keyapp_override();
                    }
#endif
					break;
				case KEY_TYPE_CUSTOMER:
					keyapp_customer(KeyDrv.CnvCode);
					break;
					//#				case KEY_TYPE_CLERK :
					//#					keyapp_clerk(KeyDrv.CnvCode);
					//					break;
				case KEY_TYPE_CURRENCY:
					keyapp_currency(1,1,Price.DisplayedPrice,Price.UnitPrice[PRICE_RESULT],KeyDrv.CnvCode);
					break;
				case KEY_TYPE_DEPT:
					keyapp_department(KeyDrv.CnvCode);
					break;
				case KEY_TYPE_TARE:
					keyapp_directtare(KeyDrv.CnvCode);
					break;
#ifdef USE_TRACE_STANDALONE	// Modified by CJK 20060725
				case KEY_TYPE_INDIVIDUAL:
					keyapp_direct_individual(KeyDrv.CnvCode);
					break;
				case KEY_TYPE_SPECIALUSE:
					if (Operation.doublePLUkey == 2)
					{
						KeyDrv.CnvCode = keyapp_togleplukey(KeyDrv.CnvCode, OFF);
					}
					else	// Operation.doublePLUkey == 0 or 1
					{
						KeyDrv.CnvCode = keyapp_doubleplukey(KeyDrv.CnvCode, OFF);
					}
					keyapp_specialuse(KeyDrv.CnvCode);
					break;
#endif
			}
		}
		else
			BuzOn(1);

	}
}

extern char time_data_r[];
INT16S Century=20;

long DateTimeValue(INT16S kind)
{
	long week,year,month,day,hour,min;

	week	=(long)bcd2hex(RTCStruct.day);
	year	=(long)bcd2hex(RTCStruct.year);
	month	=(long)bcd2hex(RTCStruct.month);
	day		=(long)bcd2hex(RTCStruct.date);
	hour	=(long)bcd2hex(RTCStruct.hour);
	min		=(long)bcd2hex(RTCStruct.min);

	if(kind==2)	// week
	{
		return week;
	}
	if(kind==1)	// date
	{
		//year+= (Century*100);
		return ((year*10000L)+(month*100L)+day);
	}
	return (hour*100+min);	// time
}

INT16S GetDiscountData(INT16S id,DISCOUNT_STRUCT* pMem)
{
	INT32U Flash_addr;
	//	long   dc_price;
	//INT16U dtype;

	if (id==0)
	{
		return 0;
	}

	//discount_str.code=id-1;
	// 메모리 타입 structure 로 read
	Flash_addr = get_addr_discount_area(id); /* id가 index 아님 dc 번호임 */
	if (!Flash_addr) { /* 에러 처리 */
		return 0;
	}
	Flash_sread(Flash_addr, (HUGEDATA INT8U *)pMem, DISCOUNT_STRUCT_SIZE);
	// 사용 가능한 discount data로 변환

	return 1;
}


INT16S	ReadDiscountIndexInfo(DISC_DATA_TYPE *pData,INT16S discountDataIndex,INT16S pluType)
{
	long	todayDate,todayTime,todayWeek;
	INT16S		i,stat;
	INT16S saleOk;
	char	tempBuffer[30];
	//disc memory type
	DISCOUNT_STRUCT tempMem;
	saleOk=0;

	if(discountDataIndex)	// PLU
	{
		stat = GetDiscountData(discountDataIndex,&tempMem);//status_scale.Plu.dc_id
	}
	else			// global
	{
		INT32U	addr;
		switch(pluType)
		{
			case PLU_BY_WEIGHT:
				addr = DFLASH_BASE  + FLASH_DISCOUNT_GLOBAL_WEIGHT;
				break;
			case PLU_BY_PCS:
				addr = DFLASH_BASE  + FLASH_DISCOUNT_GLOBAL_COUNT;
				break;
			default:
				addr = DFLASH_BASE  + FLASH_DISCOUNT_GLOBAL_NONFOOD;
				break;
		}
		Flash_sread(addr, (HUGEDATA INT8U *)&tempMem, DISCOUNT_STRUCT_SIZE);
		if(0x1c90 <=tempMem.type && tempMem.type<0x1cb2)
			stat = ON;
		else
			stat = OFF;
	}
	//sprintf(gmsg,"DC %x,%d",tempMem.type,pluType);
	//DisplayMsg(gmsg);
	//MsgOut((HUGEDATA char *)gmsg);
	//Key_DelaySec(10);
	//tempMem.type = tempMem.type & 0x000f;//(0x1c9?)
	tempMem.type = tempMem.type - 0x1c90;//(0x1c9?)
	//msg
	//if(discountDataIndex)	// PLU
	//	sprintf(gmsg,"DC PLU %d",tempMem.type);
	//else
	//	sprintf(gmsg,"DC GLB %d",tempMem.type);
	//DisplayMsg(gmsg);
	//Key_DelaySec(10);
	if(!tempMem.type)	return saleOk;
	// check range
	if(stat)
	{
		saleOk = ON;
		todayDate=DateTimeValue(1);
		todayTime=DateTimeValue(0);
		todayWeek=DateTimeValue(2);

		//pData->toDate=(Century*1000000L)+pData->toDate;
		sprintf(tempBuffer,"%02d%02d%02d",tempMem.start_year,tempMem.start_month,tempMem.start_day);
		sscanf(tempBuffer,"%6ld",&pData->fromDate);

		sprintf(tempBuffer,"%02d%02d%02d",tempMem.start_year,tempMem.start_month,tempMem.start_day);
		sscanf(tempBuffer,"%6ld",&pData->fromDate);
		sprintf(tempBuffer,"%02d%02d",tempMem.start_hour,tempMem.start_minute);
		sscanf(tempBuffer,"%4ld",&pData->fromTime);

		sprintf(tempBuffer,"%02d%02d%02d",tempMem.end_year,tempMem.end_month,tempMem.end_day);
		sscanf(tempBuffer,"%6ld",&pData->toDate);
		if(!pData->toDate) pData->toDate=991231L;

		sprintf(tempBuffer,"%02d%02d",tempMem.end_hour,tempMem.end_minute);
		sscanf(tempBuffer,"%4ld",&pData->toTime);
		if(!pData->toTime) pData->toTime=2400L;

		for(i=0;i<2;i++)
		{
			pData->discTarget[i] = tempMem.target[i];
			pData->discPrice[i] = tempMem.price[i];
		}

		pData->discOriginType = tempMem.type;
		switch(tempMem.type)
		{
			case 1:
				pData->discTargetType = DISC_TARGET_TPRICE;
				pData->discPriceType  = DISC_PRICE_TPRICE;
				break;
			case 2:
				pData->discTargetType = DISC_TARGET_TPRICE;
				pData->discPriceType  = DISC_PRICE_UPRICE;
				break;
			case 3:
				pData->discTargetType = DISC_TARGET_TPRICE;
				pData->discPriceType  = DISC_PRICE_WEIGHT;
				break;
			case 4:
				pData->discTargetType = DISC_TARGET_TPRICE;
				pData->discPriceType  = DISC_PRICE_COUNT;
				break;
			case 5:
				pData->discTargetType = DISC_TARGET_TPRICE;
				pData->discPriceType  = DISC_PRICE_PERCENT;
				break;
			case 6:
				pData->discTargetType = DISC_TARGET_WEIGHT;
				pData->discPriceType  = DISC_PRICE_TPRICE;
				break;
			case 7:
				pData->discTargetType = DISC_TARGET_WEIGHT;
				pData->discPriceType  = DISC_PRICE_UPRICE;
				break;
			case 8: //Free Addition Weight
				pData->discTargetType = DISC_TARGET_WEIGHT;
				pData->discPriceType  = DISC_FREE_ADDITION_WEIGHT;
				break;
			case 9:
				pData->discTargetType = DISC_TARGET_WEIGHT;
				pData->discPriceType  = DISC_PRICE_PERCENT;
				break;
			case 10:
				pData->discTargetType = DISC_TARGET_COUNT;
				pData->discPriceType  = DISC_PRICE_TPRICE;
				break;
			case 11:
				pData->discTargetType = DISC_TARGET_COUNT;
				pData->discPriceType  = DISC_PRICE_UPRICE;
				break;
			case 12: //Free Addition Count
				pData->discTargetType = DISC_TARGET_COUNT;
				pData->discPriceType  = DISC_FREE_ADDITION_COUNT;
				break;
			case 13:
				pData->discTargetType = DISC_TARGET_COUNT;
				pData->discPriceType  = DISC_PRICE_PERCENT;
				break;
			case 14:
				pData->discTargetType = DISC_TARGET_FIXED;
				pData->discPriceType  = DISC_PRICE_FIXED;
				break;
			case 15:
				pData->discTargetType = DISC_TARGET_FREE;
				pData->discPriceType  = DISC_PRICE_FREE;
				break;
			case 16://0x1cA0
				pData->discTargetType = DISC_TARGET_WEIGHT;
				pData->discPriceType  = DISC_PRICE_UPERCENT;
				break;
			case 17://0x1cA1
				pData->discTargetType = DISC_TARGET_COUNT;
				pData->discPriceType  = DISC_PRICE_UPERCENT;
				break;
			case 18://0x1cA2
				pData->discTargetType = DISC_TARGET_WEIGHT;
				pData->discPriceType  = DISC_PRICE_UPRICE_PLUS;
				break;
			case 19://0x1cA3
				pData->discTargetType = DISC_TARGET_COUNT;
				pData->discPriceType  = DISC_PRICE_UPRICE_PLUS;
				break;
			case 20://0x1cA4
				pData->discTargetType = DISC_TARGET_N_PLUS_1;
				pData->discPriceType  = DISC_N_PLUS_1;
				break;
			case 21://0x1cA5
				pData->discTargetType = DISC_TARGET_N_PLUS_1;
				pData->discPriceType  = DISC_N_PLUS_1;
				break;

			default:
				saleOk=OFF;
				break;

		}
		pData->week=todayWeek;
		if(saleOk)// date
		{
			saleOk = OFF;
			//sprintf(gmsg,"DATE %6ld %6ld %6ld",pData->fromDate,todayDate,pData->toDate);
			//PutSmallString(2,0,gmsg);
			//Key_DelaySec(20);
		 	if(pData->fromDate<=todayDate && todayDate<=pData->toDate)
				saleOk = ON;
		}

		if(saleOk)
		{
			//sprintf(gmsg,"TIME %6ld %6ld %6ld",pData->fromTime,todayTime,pData->toTime);
			//PutSmallString(2,0,gmsg);
			//Key_DelaySec(20);
			saleOk = OFF;
			//			if(!pData->fromTime && pData->toTime)
			//				saleOk = ON;
			if(pData->fromTime<=todayTime && todayTime<=pData->toTime)
				saleOk = ON;
		}

		if(saleOk)
		{
			saleOk = OFF;
			switch(todayWeek)	//Modified by JJANG 20080305
			{
				case 0://SUN
					if(tempMem.day_of_week & 0x80)	saleOk=ON;
					break;
				case 1://MON
					if(tempMem.day_of_week & 0x40)	saleOk=ON;
					break;
				case 2:
					if(tempMem.day_of_week & 0x20)	saleOk=ON;
					break;
				case 3:
					if(tempMem.day_of_week & 0x10)	saleOk=ON;
					break;
				case 4:
					if(tempMem.day_of_week & 0x08)	saleOk=ON;
					break;
				case 5:
					if(tempMem.day_of_week & 0x04)	saleOk=ON;
					break;
				case 6:
					if(tempMem.day_of_week & 0x02)	saleOk=ON;
					break;
				default:
					saleOk = OFF;
					//DisplayMsg("Check date(week)");
					break;
			}
			//if(saleOk)
			//{
			//DisplayMsg("WEEK OK !");
			//Key_DelaySec(10);
			//}
			////else
			//{
			//sprintf(gmsg,"week %d, %x",todayWeek,tempMem.day_of_week);
			//DisplayMsg(gmsg);
			//Key_DelaySec(10);
			//}
		}
	}
	//msg
	//if(saleOk)
	//{
	//DisplayMsg("Sale OK(1) !");
	//Key_DelaySec(10);
	//}
	if(saleOk)
	{
		saleOk=OFF;
		for(i=0;i<2;i++)
		{
			if(pData->discTarget[i])
				saleOk=ON;
		}
		if(pData->discPriceType==DISC_PRICE_FIXED)	saleOk=ON;
		if(saleOk)
		{
			saleOk=OFF;
			for(i=0;i<2;i++)
			{
				if(pData->discPrice[i])
					saleOk=ON;
			}
		}
		if(pData->discPriceType==DISC_PRICE_FREE)	saleOk=ON;
	}

	if(saleOk)  //SG061212
	{
		DiscSale.saleDiscFSP = tempMem.useForFSP;
	}


	//msg
	//if(saleOk)
	//{
	//DisplayMsg("Disc OK");
	//Key_DelaySec(10);
	//}
	return saleOk;
}
//
//// PLU 호출때
INT16S	GetDiscountInfo(INT16S pluType)
{
	DISC_DATA_TYPE	discountPluData,discountGlobalData;

	Operation.discountInfo = OFF;
	PriorityGlobalDiscount = get_global_bparam(GLOBAL_DISCOUNT_PRIORITY);
	DiscSale.saleDiscountType=DISCOUNT_NO;
	DiscSale.saleDiscFSP =0;
	DiscSale.saleIndexOk=OFF;
	DiscSale.saleGlobalOk=OFF;
	if (!(status_scale.restrict&FUNC_DISCOUNT)) return 0;

	switch(PriorityGlobalDiscount)
	{
		case DISC_PRIORITY_INDEX:
			DiscSale.saleIndexOk=ON;
			DiscSale.saleGlobalOk=OFF;
			break;
		case DISC_PRIORITY_INDEX_GLOBAL:
			DiscSale.saleIndexOk=ON;
			DiscSale.saleGlobalOk=ON;
			break;
		case DISC_PRIORITY_GLOBAL:
			DiscSale.saleIndexOk=OFF;
			DiscSale.saleGlobalOk=ON;
			break;
		case DISC_PRIORITY_NO:
		default:
			return 0;
	}
	if(DiscSale.saleGlobalOk) {
		DiscSale.saleGlobalOk=OFF;
		//if(globalDiscIndex)
		{
			DiscSale.saleGlobalOk=(INT8U)ReadDiscountIndexInfo(&discountGlobalData,0,pluType);
		}
	}
	///////////////////////	INDEX
	if(DiscSale.saleIndexOk)
	{
		DiscSale.saleIndexOk=OFF;
		if(status_scale.Plu.dc_id)//PLU data
		{
			DiscSale.saleIndexOk=(INT8U)ReadDiscountIndexInfo(&discountPluData,status_scale.Plu.dc_id,pluType);
			if(DiscSale.saleIndexOk)
				Operation.discountInfo = ON;
		}
	}

	switch(PriorityGlobalDiscount)
	{
		case DISC_PRIORITY_INDEX:
			if(DiscSale.saleIndexOk)
				DiscSale.saleDiscountType=DISCOUNT_INDEX;
			break;
		case DISC_PRIORITY_GLOBAL:
			if(DiscSale.saleGlobalOk)
				DiscSale.saleDiscountType=DISCOUNT_GLOBAL;
			break;
		case DISC_PRIORITY_INDEX_GLOBAL:
			if(DiscSale.saleGlobalOk)
				DiscSale.saleDiscountType=DISCOUNT_GLOBAL;
			if(DiscSale.saleIndexOk)
				DiscSale.saleDiscountType=DISCOUNT_INDEX;
			break;
	}
	switch(DiscSale.saleDiscountType)
	{
		case DISCOUNT_NO:
			break;
		case DISCOUNT_GLOBAL:
			DiscSale.discIndexData=discountGlobalData;// copy data value
			break;
		case DISCOUNT_INDEX:
			DiscSale.discIndexData=discountPluData;// copy data value
			break;
	}
	return DiscSale.saleDiscountType;
}

long  longmuldiv(long val,long mul,long div)
{
#ifdef USE_PRICE_UNIT_9DIGIT
	long long int val1,val2,val3,val4;
#else
	long val1,val2,val3,val4;
#endif


	if (div==0L)
		return 0L;

#ifdef USE_PRICE_UNIT_9DIGIT
	val1= ((long long int)(val) / 100LL) * (long long int)(mul);
	val2= ((long long int)(val) % 100LL) * (long long int)(mul);
	val3= (val1 / (long long int)(div)) * 100LL;
	val4= (val1 % (long long int)(div)) * 100LL;
#else
	val1= (val / 100L) * mul;
	val2= (val % 100L) * mul;
	val3= (val1 / div) * 100L;
	val4= (val1 % div) * 100L;
#endif
	// Modified by CJK 20060602
	//val= val3 + (val4 / div) + (val2 / div);
	//if (1<div && (val4 % div) >= (div / 2L))
	//	++val;
	//if (1<div && (val2 % div) >= (div / 2L))
	//	++val;
	////////////////////////////
	val= val3 + (val4 + val2) / div;
#ifndef USE_CHINA_ROUND	
	if (1<div && ((val4 + val2) % div) >= (div / 2L))
		++val;
#endif
	return val;
}

long  longmuldivNoRound(long val,long mul,long div)
{
	long val1,val2,val3,val4;


	if (div==0L)
		return 0L;

	val1= (val / 100L) * mul;
	val2= (val % 100L) * mul;
	val3= (val1 / div) * 100L;
	val4= (val1 % div) * 100L;
	val= val3 + (val4 + val2) / div;
	return val;
}

long	CalcPercentPrice(long pPrice,INT16S percent)
{
	long	temp;
	temp = longmuldiv(pPrice,percent,10000);
	return temp;
}

// 판단의 시점이 애매함
//extern long SummaryRoundPrice;
long	PriceRounds(long tprice, INT16S roundType, INT16S assignFlag, long *roundDifference)
{
	long	value1=tprice,value2=0;//,roundValue;
	//INT8U	settingType;
	if(!roundType)
	{
		return tprice;
	}
	if(GlbPrice.roundApply != roundType)
		return tprice;

	if(GlbPrice.roundPrice)	// 0,5,10,100,1000
	{
		value1=(tprice/GlbPrice.roundPrice)*GlbPrice.roundPrice;
		value2=tprice-value1;
	}

	if(GlbPrice.roundUp) // 1 = 반올림(round-off), 0 = 버림(round-down)
	{
		if(value1 >= 0) // value1이 0인경우 반올림 처리 해야함. ex) roundPrice : 5, tprice 0.4 -> 0.5 로 표시. 
		{
			//if(GlbPrice.roundPrice/2 < value2)
			if(GlbPrice.roundPrice <= value2*2)
				value1+=GlbPrice.roundPrice;
		}
		else
		{
			if(GlbPrice.roundPrice <= -(value2*2))
				value1-=GlbPrice.roundPrice;
		}
	}
	if(assignFlag && roundDifference)
	{
		//transPay.summaryround = tprice - value1;
		*roundDifference = tprice - value1;
	}
	//test
	//sprintf(gmsg,"PR %7ld,+R %4ld",value1,transPay.summaryround);
	//PutSmallString(1,0,gmsg);
	return value1;
}

long CalcWeightBasedPrice(long maxPrice,long  wt,long unitprice)
{
	long lprice;
	INT8U dec;
	INT16S div, mul_kg_g, div_dec, div_kg_100g;

	if (wt<=0l)
		return 0;
	if (unitprice <=0l)
		return 0;

	if (ADM_Status.Capa == ADM_CUSTOM_CAPA_ID) {
		mul_kg_g = 3;
		dec = get_global_bparam(GLOBAL_DISPLAY_WEIGHTDECIMAL);
	} else {
		mul_kg_g = 3;		// mul_kg_g = kg,lb:3, g:0
		if(CurPluWeightUnit == WEIGHT_UNIT_LB) {
			dec = decimal_lb[ADM_Status.Dual][ADM_Status.Capa];
		} else {
			dec = decimal_kg[ADM_Status.Dual][ADM_Status.Capa];
			if(CurPluWeightUnit == WEIGHT_UNIT_G) {// Added by CJK 20060530
				dec = 0;
				mul_kg_g = 0;
			}
		}
	}
	div_dec = dec;		// div_dec = kg,lb:x, g:0
	div_kg_100g = 3;	// div_kg_100g = kg,lb:3, g:2

#ifdef USE_WEIGHT_UNIT_500G
	if (status_scale.cur_weightunit == 100 || (status_scale.cur_weightunit == 500 && PluType()==PLU_BY_WEIGHT)) {	// CJK080214
#else
	if (status_scale.cur_weightunit == 100) {	// CJK080214
#endif
		div_kg_100g = 2;
	}

	div = div_dec + div_kg_100g - mul_kg_g;
	if(div < 0) {
		wt *= power10(-div);
		div = 0;
	}

#ifdef USE_WEIGHT_UNIT_500G
	if(status_scale.cur_weightunit == 500 && PluType()==PLU_BY_WEIGHT) 
	{
		lprice = longmuldiv(wt,unitprice,power10(div)*5);
	}
	else
	{
		lprice = longmuldiv(wt,unitprice,power10(div));
	}
#else
	lprice = longmuldiv(wt,unitprice,power10(div));
#endif

	if(maxPrice<=lprice)
	{
		Operation.OverTotalPrice=ON;
		return 0;
	}

	return lprice;
}


long InternalTaxPrice(long  taxRate,long totalPrice)
{
	long lprice;

	if (taxRate<=0l)
		return 0;
	if (totalPrice <=0l)
		return 0;

	//	lprice = (taxRate*totalPrice)/(10000+taxRate);
	//	lprice = longmuldiv(taxRate,totalPrice,10000+taxRate);
	//lprice = longmuldivdown(taxRate,totalPrice,10000+taxRate,0);
	lprice = longmuldiv(taxRate,totalPrice,10000+taxRate);	// Modified by CJK 20050928

	return lprice;
}

// Overflow에 의한 계산 방지
INT16S	CheckPriceOverflow(long	qty , long unitprice)
{
	long	sum=0L,i;

	for(i=0;i<qty;i++)
	{
		sum+=unitprice;
		if(sum<0L)
		{
			//strcpy(gmsg,"overprice");
			//PutSmallString(3,0,gmsg);
			Operation.OverUnitPrice=ON;
			return ON;
		}
	}
	//sprintf(gmsg,"%9ld",sum);
	//PutSmallString(3,0,gmsg);
	return OFF;
}

long CalcByCountPrice(long  qty,long unitcount,long unitprice)
{
    //INT16S i;
	long lprice;//,fraction,price1;
	//INT16S Decimals,factor;
	long lTemp;
	char	upFlag=OFF;

	if (qty<=0l)
		return 0;
	if (unitprice <=0l)
		return 0;
	if (unitcount <=0l)
		return 0;

	if(!CheckPriceOverflow(qty , unitprice))
	{
		if(!CheckPriceOverflow(qty*10 , unitprice))
		{
			lprice= qty * unitprice *10;
			upFlag=ON;
		}
		else
			lprice= qty * unitprice;

		lprice/= unitcount;
	}
	else
		lprice=0L;

	if(upFlag)
	{
		lTemp = lprice % 10L;
		lprice /= 10L;
	}
	if(upFlag && 5 <= lTemp)	lprice++;
	return lprice;
}

//void	DisplayDiscMsg(void)
//{
//	if(GlbOper.viewDiscount)	return;
//	DisplayMsg("Check discount data");
//}

INT16S	SaleGetDiscPrice(INT16S linkedPLU,PRICE_TYPE *pPrice, INT16S pluType)
{
	INT16S	index;
	long	discWeight;
	long    roundDifference;
	long	freeaddition_value[2];
	long	freeaddition_rest;
	//	INT16S	wDecimal;// =(INT16S)get_global_bparam(GLOBAL_DISPLAY_WEIGHTDECIMAL);
	//	INT16S	pDecimal = (INT32U)get_global_bparam(GLOBAL_AREA_PRICE_DECIMALPOINT);
	//	wDecimal = 3;
	//	if(get_defaultunit()<=100)
	//		wDecimal=2;
#ifdef USE_TOPMART_DEFAULT //sewon 170118
	long	tempprice[2]; //sewon 170118
	long	tempround;
	INT32U	tempqty;
#endif

	if(!status_scale.cur_pluid)	{
		DiscSale.saleDiscountType = DISCOUNT_NO;//
		DiscSale.saleDiscFSP =0;
	}
	//
	//// 단가로 인한 discount없는 계산
	if(MISC_PLU_WEIGHT <= status_scale.cur_pluid){}
	else
	{//option:override discount
		if(Operation.override){
			DiscSale.saleDiscountType=DISCOUNT_NO;
			DiscSale.saleDiscFSP=0;
		}
	}
	if(Operation.specialPrice){
		DiscSale.saleDiscountType=DISCOUNT_NO;
		DiscSale.saleDiscFSP=0;
	}
	// [2006.03.06 bmlim
	//	if(Operation.fixedPrice)	DiscSale.saleDiscountType=DISCOUNT_NO;
	//	if (Operation.fixedPrice)	DiscSale.saleDiscountType=DISC_TARGET_TPRICE;
	// [-- end
	if(status_scale.Plu.fixed_weight){
		DiscSale.saleDiscountType=DISCOUNT_NO;
		DiscSale.saleDiscFSP=0;
	}

	if(Operation.fixedPrice) {
		if(DiscSale.discIndexData.discPriceType == DISC_PRICE_UPRICE)
			DiscSale.discIndexData.discPriceType = DISC_PRICE_TPRICE;
		if(DiscSale.discIndexData.discPriceType == DISC_PRICE_UPERCENT)
			DiscSale.discIndexData.discPriceType = DISC_PRICE_PERCENT;
	}


	//index = PRICE_SAVED;
	pPrice->Weight[PRICE_SAVED] = 0L;
	pPrice->Qty[PRICE_SAVED]= 0;
	pPrice->UnitCount[PRICE_SAVED] = 0;
	pPrice->UnitPrice[PRICE_SAVED] = 0L;
	pPrice->TotalPrice[PRICE_SAVED] = 0L;

	//OPTION:temp date
	//DiscSale.saleDiscountType=DISCOUNT_NO;

	//index = PRICE_ORIGIN;
	if(PluType()==PLU_BY_COUNT)
	{
		pPrice->UnitCount[PRICE_ORIGIN] = 1;
	}

	pPrice->Weight[PRICE_RESULT] = pPrice->Weight[PRICE_ORIGIN];
	pPrice->Qty[PRICE_RESULT]    = pPrice->Qty[PRICE_ORIGIN];
	pPrice->UnitCount[PRICE_RESULT] = pPrice->UnitCount[PRICE_ORIGIN];
	pPrice->UnitPrice[PRICE_RESULT] = pPrice->UnitPrice[PRICE_ORIGIN];

	if(GlbPrice.roundApply == ROUND_PLU_ROUND_CUT)
		pPrice->TotalPrice[PRICE_ORIGIN] = PriceRounds(pPrice->TotalPrice[PRICE_ORIGIN], ROUND_PLU_ROUND_CUT,ON, &roundDifference);
	else
		pPrice->TotalPrice[PRICE_ORIGIN] = PriceRounds(pPrice->TotalPrice[PRICE_ORIGIN], ROUND_NO,ON, &roundDifference);
	//transPay.summaryround = roundDifference;
	//	pPrice->RoundPrice = roundDifference;

	pPrice->TotalPrice[PRICE_RESULT] = pPrice->TotalPrice[PRICE_ORIGIN];
	if(Operation.MaxTotalPrice <= pPrice->TotalPrice[PRICE_ORIGIN] || 	Operation.OverUnitPrice)
	{
		Operation.OverTotalPrice=ON;
		return 0;
	}

	if(DiscSale.saleDiscountType==DISCOUNT_NO)
	{
		if(GlbPrice.roundApply == ROUND_ALL_PLU || (GlbPrice.roundApply == ROUND_TOTAL_OR_PLU && Operation.operationClerk==OPER_STD_CLERK && !Operation.addStandatdClerk))
			pPrice->TotalPrice[PRICE_RESULT] = PriceRounds(pPrice->TotalPrice[PRICE_RESULT], GlbPrice.roundApply,OFF,&roundDifference);
		return 0;
	}
#ifndef USE_PLU_MULTIPLE_PRICE	//Refefence PLU 기능 사용 안 함
	if(linkedPLU)	return 0;
#endif
	discWeight=0L;
	DiscSale.saleDiscOk=ON;

	switch(DiscSale.discIndexData.discTargetType)
	{
		case DISC_TARGET_WEIGHT:
#ifdef USE_PLU_MULTIPLE_PRICE
			if(DiscSale.discIndexData.discTarget[0]>0 && ((DiscSale.discIndexData.discTarget[1]==0L && DiscSale.discIndexData.discTarget[0]<=pPrice->Weight[PRICE_ORIGIN]) ||
				(DiscSale.discIndexData.discTarget[1]>pPrice->Weight[PRICE_ORIGIN] && DiscSale.discIndexData.discTarget[0]<=pPrice->Weight[PRICE_ORIGIN])))
				index=0;
			else if(DiscSale.discIndexData.discTarget[0]<DiscSale.discIndexData.discTarget[1] && DiscSale.discIndexData.discTarget[1]<=pPrice->Weight[PRICE_ORIGIN])
				index=1;
			else
				DiscSale.saleDiscOk=OFF;
#else
			if(DiscSale.discIndexData.discTarget[1] && DiscSale.discIndexData.discTarget[1]<=pPrice->Weight[PRICE_ORIGIN])
				index=1;
			else if(DiscSale.discIndexData.discTarget[0]<=pPrice->Weight[PRICE_ORIGIN])
				index=0;
			else
				DiscSale.saleDiscOk=OFF;
#endif
			if(pluType!=PLU_BY_WEIGHT)
				DiscSale.saleDiscOk=OFF;
			break;
		case DISC_TARGET_COUNT:
#ifdef USE_PLU_MULTIPLE_PRICE
			if(DiscSale.discIndexData.discTarget[0]>0 && ((DiscSale.discIndexData.discTarget[1]==0L && DiscSale.discIndexData.discTarget[0]<=pPrice->Qty[PRICE_ORIGIN]) ||
				(DiscSale.discIndexData.discTarget[1]>pPrice->Qty[PRICE_ORIGIN] && DiscSale.discIndexData.discTarget[0]<=pPrice->Qty[PRICE_ORIGIN])))
				index=0;
			else if(DiscSale.discIndexData.discTarget[0]<DiscSale.discIndexData.discTarget[1] && DiscSale.discIndexData.discTarget[1]<=pPrice->Qty[PRICE_ORIGIN])
				index=1;
			else
				DiscSale.saleDiscOk=OFF;
#else
			if(DiscSale.discIndexData.discTarget[1] && DiscSale.discIndexData.discTarget[1]<=pPrice->Qty[PRICE_ORIGIN])
				index=1;
			else if(DiscSale.discIndexData.discTarget[0]<=pPrice->Qty[PRICE_ORIGIN])
				index=0;
			else
				DiscSale.saleDiscOk=OFF;
#endif
			if(pluType==PLU_BY_WEIGHT)
				DiscSale.saleDiscOk=OFF;
			break;
		case DISC_TARGET_TPRICE:
			if(DiscSale.discIndexData.discTarget[1] && DiscSale.discIndexData.discTarget[1]<=pPrice->TotalPrice[PRICE_ORIGIN])
				index=1;
			else if(DiscSale.discIndexData.discTarget[0]<=pPrice->TotalPrice[PRICE_ORIGIN])
				index=0;
			else
				DiscSale.saleDiscOk=OFF;
			break;
		case DISC_TARGET_FREE:
			DiscSale.saleDiscOk=ON;
			break;
		case DISC_TARGET_FIXED:
			index=0;
			if(!Price.FixedDiscountPrice)
				DiscSale.saleDiscOk=OFF;
			break;
		case DISC_TARGET_N_PLUS_1:
			if(pluType==PLU_BY_WEIGHT)
			{
				if((DiscSale.discIndexData.discTarget[0]+1)<=status_scale.cur_qty)
					index=0;
				else
					DiscSale.saleDiscOk=OFF;
			}
			else
			{
				if((DiscSale.discIndexData.discTarget[0]+1)<=pPrice->Qty[PRICE_ORIGIN])
					index=0;
				else
					DiscSale.saleDiscOk=OFF;
			}
			break;

	}

	if(DiscSale.saleDiscOk)
		switch(DiscSale.discIndexData.discPriceType)
		{
			case DISC_PRICE_WEIGHT:
				if(DiscSale.discIndexData.discPrice[index]<pPrice->Weight[PRICE_ORIGIN])
					discWeight	=DiscSale.discIndexData.discPrice[index];
				else
				{
					//				DisplayDiscMsg();
					discWeight	=0L;
				}
				//			if(!DiscSale.saleDiscOk)
				//				discWeight	=0L;
				pPrice->Weight[PRICE_RESULT] = pPrice->Weight[PRICE_ORIGIN] - discWeight;
				// BMLIM			pPrice->TotalPrice[PRICE_RESULT]	=CalcWeightBasedPrice(wDecimal,Operation.MaxTotalPrice,pPrice->Weight[PRICE_RESULT],pPrice->UnitPrice[PRICE_RESULT]);
				pPrice->TotalPrice[PRICE_RESULT]	=CalcWeightBasedPrice(Operation.MaxTotalPrice,pPrice->Weight[PRICE_RESULT],pPrice->UnitPrice[PRICE_RESULT]);
				break;
			case DISC_PRICE_COUNT:
				if(DiscSale.discIndexData.discTargetType == DISC_TARGET_COUNT)
				{
					if(0 <= pPrice->Qty[PRICE_ORIGIN]-DiscSale.discIndexData.discTarget[index])
					{
						if(DiscSale.discIndexData.discPrice[index]<pPrice->Qty[PRICE_ORIGIN])
							pPrice->Qty[PRICE_RESULT]			=pPrice->Qty[PRICE_ORIGIN]-DiscSale.discIndexData.discPrice[index];
					}
					else
					{
						//					DisplayDiscMsg();
						pPrice->Qty[PRICE_RESULT]		=pPrice->Qty[PRICE_ORIGIN];
					}
					pPrice->TotalPrice[PRICE_RESULT]	=CalcByCountPrice(pPrice->Qty[PRICE_RESULT],pPrice->UnitCount[PRICE_RESULT],pPrice->UnitPrice[PRICE_RESULT]);
				}
				else	// count for total price
				{
					if(0 <= pPrice->TotalPrice[PRICE_ORIGIN]-DiscSale.discIndexData.discTarget[index])
					{
						if(DiscSale.discIndexData.discPrice[index]<pPrice->Qty[PRICE_ORIGIN])
							pPrice->Qty[PRICE_RESULT]			=pPrice->Qty[PRICE_ORIGIN]-DiscSale.discIndexData.discPrice[index];
					}
					else
					{
						//					DisplayDiscMsg();
						pPrice->Qty[PRICE_RESULT]		=pPrice->Qty[PRICE_ORIGIN];
					}
					pPrice->TotalPrice[PRICE_RESULT]	=CalcByCountPrice(pPrice->Qty[PRICE_RESULT],pPrice->UnitCount[PRICE_RESULT],pPrice->UnitPrice[PRICE_RESULT]);
				}
				break;
			case DISC_PRICE_UPRICE:
				//OPTION:	apply unit price
				if(GlbOper.applyUnitpriceForDisc || Operation.specialPriceDisc)
				{
					if(DiscSale.discIndexData.discPrice[index])
						pPrice->UnitPrice[PRICE_RESULT]	=DiscSale.discIndexData.discPrice[index];
				}
				else
				{
					if(0L < pPrice->UnitPrice[PRICE_ORIGIN]-DiscSale.discIndexData.discPrice[index])
					{
						//sprintf(gmsg,"discPrice[%d]=%6ld",index,DiscSale.discIndexData.discPrice[index]);
						//PutSmallString(3,0,gmsg);
						pPrice->UnitPrice[PRICE_RESULT]	=pPrice->UnitPrice[PRICE_ORIGIN]-DiscSale.discIndexData.discPrice[index];
					}
					else
					{
						//DisplayDiscMsg();
						pPrice->UnitPrice[PRICE_RESULT]	=pPrice->UnitPrice[PRICE_ORIGIN];
					}
				}
				if(PluType()==PLU_BY_WEIGHT)
					// BMLIM				pPrice->TotalPrice[PRICE_RESULT]	=CalcWeightBasedPrice(wDecimal,Operation.MaxTotalPrice,pPrice->Weight[PRICE_ORIGIN],pPrice->UnitPrice[PRICE_RESULT]);
					pPrice->TotalPrice[PRICE_RESULT]	=CalcWeightBasedPrice(Operation.MaxTotalPrice,pPrice->Weight[PRICE_ORIGIN],pPrice->UnitPrice[PRICE_RESULT]);
				else	// count
					pPrice->TotalPrice[PRICE_RESULT]	=CalcByCountPrice(pPrice->Qty[PRICE_RESULT],pPrice->UnitCount[PRICE_RESULT],pPrice->UnitPrice[PRICE_RESULT]);
				break;
			case DISC_PRICE_TPRICE:
				if(0L < pPrice->TotalPrice[PRICE_ORIGIN]-DiscSale.discIndexData.discPrice[index])
					pPrice->TotalPrice[PRICE_RESULT]	=pPrice->TotalPrice[PRICE_ORIGIN]-DiscSale.discIndexData.discPrice[index];
				else
				{
					//				DisplayDiscMsg();
					pPrice->TotalPrice[PRICE_RESULT]	=pPrice->TotalPrice[PRICE_ORIGIN];
				}
				break;
			case DISC_PRICE_PERCENT:
				pPrice->TotalPrice[PRICE_RESULT]	=pPrice->TotalPrice[PRICE_ORIGIN]-CalcPercentPrice(pPrice->TotalPrice[PRICE_ORIGIN],DiscSale.discIndexData.discPrice[index]);
				break;
			case DISC_PRICE_UPERCENT:
				pPrice->UnitPrice[PRICE_RESULT]		=pPrice->UnitPrice[PRICE_ORIGIN]-CalcPercentPrice(pPrice->UnitPrice[PRICE_ORIGIN],DiscSale.discIndexData.discPrice[index]);
				//// BMLIM			pPrice->TotalPrice[PRICE_RESULT]	=CalcWeightBasedPrice(wDecimal,Operation.MaxTotalPrice,pPrice->Weight[PRICE_ORIGIN],pPrice->UnitPrice[PRICE_RESULT]);
				//			pPrice->TotalPrice[PRICE_RESULT]	=CalcWeightBasedPrice(Operation.MaxTotalPrice,pPrice->Weight[PRICE_ORIGIN],pPrice->UnitPrice[PRICE_RESULT]);
				if(PluType()==PLU_BY_WEIGHT)
					// BMLIM				pPrice->TotalPrice[PRICE_RESULT]	=CalcWeightBasedPrice(wDecimal,Operation.MaxTotalPrice,pPrice->Weight[PRICE_ORIGIN],pPrice->UnitPrice[PRICE_RESULT]);
					pPrice->TotalPrice[PRICE_RESULT]	=CalcWeightBasedPrice(Operation.MaxTotalPrice,pPrice->Weight[PRICE_ORIGIN],pPrice->UnitPrice[PRICE_RESULT]);
				else	// count
					pPrice->TotalPrice[PRICE_RESULT]	=CalcByCountPrice(pPrice->Qty[PRICE_RESULT],pPrice->UnitCount[PRICE_RESULT],pPrice->UnitPrice[PRICE_RESULT]);
				break;
			case DISC_PRICE_FREE:
				pPrice->TotalPrice[PRICE_RESULT]	=0;
				break;
			case DISC_PRICE_FIXED:
				//strcpy(gmsg,"3333");
				//PutSmallString(3,0,gmsg);
				if(PluType()==PLU_BY_WEIGHT)
					// BMLIM				pPrice->TotalPrice[PRICE_RESULT]	=CalcWeightBasedPrice(wDecimal,Operation.MaxTotalPrice,pPrice->Weight[PRICE_ORIGIN],pPrice->UnitPrice[PRICE_RESULT]);
					pPrice->TotalPrice[PRICE_RESULT]	=CalcWeightBasedPrice(Operation.MaxTotalPrice,pPrice->Weight[PRICE_ORIGIN],pPrice->UnitPrice[PRICE_RESULT]);
				else	// count
					pPrice->TotalPrice[PRICE_RESULT]	=CalcByCountPrice(pPrice->Qty[PRICE_RESULT],pPrice->UnitCount[PRICE_RESULT],pPrice->UnitPrice[PRICE_RESULT]);
				//금액이 크면 정가할인
				Operation.fixedDiscFlag = OFF;
				if(pPrice->FixedDiscountPrice<pPrice->TotalPrice[PRICE_RESULT])
				{
					Operation.fixedDiscFlag = ON;
					pPrice->TotalPrice[PRICE_RESULT]	= pPrice->FixedDiscountPrice;
				}
				break;
			case DISC_PRICE_TSPECIAL:
				if (pPrice->TotalPrice[PRICE_ORIGIN] > DiscSale.discIndexData.discPrice[index])
					pPrice->TotalPrice[PRICE_RESULT]	= DiscSale.discIndexData.discPrice[index];
				else
					pPrice->TotalPrice[PRICE_RESULT]	= pPrice->TotalPrice[PRICE_ORIGIN];
				break;
			case DISC_PRICE_USPECIAL:
				if (pPrice->TotalPrice[PRICE_ORIGIN] > DiscSale.discIndexData.discPrice[index])
					pPrice->UnitPrice[PRICE_RESULT]	= DiscSale.discIndexData.discPrice[index];
				else
					pPrice->UnitPrice[PRICE_RESULT]	= pPrice->UnitPrice[PRICE_ORIGIN];
				if(PluType()==PLU_BY_WEIGHT)
					pPrice->TotalPrice[PRICE_RESULT]	=CalcWeightBasedPrice(Operation.MaxTotalPrice,pPrice->Weight[PRICE_ORIGIN],pPrice->UnitPrice[PRICE_RESULT]);
				else	// count
					pPrice->TotalPrice[PRICE_RESULT]	=CalcByCountPrice(pPrice->Qty[PRICE_RESULT],pPrice->UnitCount[PRICE_RESULT],pPrice->UnitPrice[PRICE_RESULT]);
				break;
#ifdef USE_TOPMART_DEFAULT //sewon 170117
			case DISC_PRICE_TOTALTOUNIT :
				//무게 or 개수
				if(PluType()==PLU_BY_WEIGHT) //무게 
				{
					//if (pPrice->TotalPrice[PRICE_ORIGIN] > DiscSale.discIndexData.discPrice[index]) //원래 가격을 넘지 않으면
					//{
						pPrice->TotalPrice[PRICE_RESULT]	= DiscSale.discIndexData.discPrice[index];
						tempprice[index] = DiscSale.discIndexData.discPrice[index];
						if(tempprice[index] % status_scale.Weight)//가격에서 개수를 나누기 나머지 생기면 무조건 올림
						{
							tempround = tempprice[index];
							tempround = (tempround*100)/status_scale.Weight;
							tempprice[index] = tempround+1;
						}
						else //나머지 없음.
						{
							tempround = tempprice[index];
							tempround = (tempround*100)/status_scale.Weight;
							tempprice[index] = tempround;
						}
																
						//if (pPrice->TotalPrice[PRICE_ORIGIN] > tempprice[index]) //단가가 기존보다 작으면
						//{
							pPrice->UnitPrice[PRICE_RESULT] = tempprice[index];
						//}
						//else //간가가 기존보다 크면 
						//{
						//	pPrice->UnitPrice[PRICE_RESULT] = pPrice->UnitPrice[PRICE_ORIGIN];
						//}
					//}
					//else //원래 가격 넘으면
					//{
					//	pPrice->TotalPrice[PRICE_RESULT]	= pPrice->TotalPrice[PRICE_ORIGIN];
					//	break;
					//}
					pPrice->TotalPrice[PRICE_RESULT]	=CalcWeightBasedPrice(Operation.MaxTotalPrice,pPrice->Weight[PRICE_ORIGIN],pPrice->UnitPrice[PRICE_RESULT]);
					pPrice->TotalPrice[PRICE_RESULT] = DiscSale.discIndexData.discPrice[index]; //입력된 가격 그대로 결정	
				}
				else // 개수 
				{
					//if(InputQtyFlag) //수량 입력 키 사용 
					//{
					//	tempqty = InputQty;
					//}
					//else //일반 수량 입력
					//{
						tempqty = status_scale.cur_qty;
					//} 
					//if (pPrice->TotalPrice[PRICE_ORIGIN] > DiscSale.discIndexData.discPrice[index]) //원래 가격을 넘지 않으면
					//{
						pPrice->TotalPrice[PRICE_RESULT]	= DiscSale.discIndexData.discPrice[index];
						tempprice[index] = DiscSale.discIndexData.discPrice[index];
						if(tempprice[index] % tempqty)//가격에서 개수를 나누기 나머지 생기면 무조건 올림
						{
							tempprice[index]/= tempqty;
							tempround = tempprice[index];
							tempprice[index] = tempround+1;
						}
						else //나머지 없음.
						{
							tempprice[index]/= tempqty;
						}
																
						//if (pPrice->TotalPrice[PRICE_ORIGIN] > tempprice[index]) //단가가 기존보다 작으면
						//{
							pPrice->UnitPrice[PRICE_RESULT] = tempprice[index];
						//}
						//else //간가가 기존보다 크면 
						//{
						//	pPrice->UnitPrice[PRICE_RESULT] = pPrice->UnitPrice[PRICE_ORIGIN];
						//}
					//}
					//else //원래 가격 넘으면
					//{
					//	pPrice->TotalPrice[PRICE_RESULT]	= pPrice->TotalPrice[PRICE_ORIGIN];
					//	break;
					//}
					pPrice->TotalPrice[PRICE_RESULT]	=CalcByCountPrice(pPrice->Qty[PRICE_RESULT],pPrice->UnitCount[PRICE_RESULT],pPrice->UnitPrice[PRICE_RESULT]);
					pPrice->TotalPrice[PRICE_RESULT] = DiscSale.discIndexData.discPrice[index]; //입력된 가격 그대로 결정
				}
				break;
#endif
#ifdef USE_EDIT_TOTALPRICE
			case DISC_PRICE_EDITTOTAL :
					if (!(pPrice->TotalPrice[PRICE_ORIGIN] > DiscSale.discIndexData.discPrice[index])) //원래 가격을 넘지 않으면
					{
						pPrice->TotalPrice[PRICE_RESULT]	= pPrice->TotalPrice[PRICE_ORIGIN];
						break;
					}
					pPrice->TotalPrice[PRICE_RESULT] = DiscSale.discIndexData.discPrice[index]; //입력된 가격 그대로 결정
				break;
#endif
			case DISC_FREE_ADDITION_WEIGHT :
				if (index == 0)
				{
					//first target
					freeaddition_value[index] = pPrice->Weight[PRICE_ORIGIN] / (DiscSale.discIndexData.discTarget[index] + DiscSale.discIndexData.discPrice[index]);
					pPrice->Weight[PRICE_RESULT] -= DiscSale.discIndexData.discPrice[index] * freeaddition_value[index];
					freeaddition_rest = pPrice->Weight[PRICE_ORIGIN] % (DiscSale.discIndexData.discTarget[index] + DiscSale.discIndexData.discPrice[index]);
					if(freeaddition_rest > DiscSale.discIndexData.discTarget[index]) pPrice->Weight[PRICE_RESULT] -= (freeaddition_rest - DiscSale.discIndexData.discTarget[index]);
				}
				if (index == 1)
				{	//Second Target
					freeaddition_value[index] = pPrice->Weight[PRICE_ORIGIN] / (DiscSale.discIndexData.discTarget[index] + DiscSale.discIndexData.discPrice[index]);
					pPrice->Weight[PRICE_RESULT] -= DiscSale.discIndexData.discPrice[index] * freeaddition_value[index];
					freeaddition_rest = pPrice->Weight[PRICE_ORIGIN] % (DiscSale.discIndexData.discTarget[index] + DiscSale.discIndexData.discPrice[index]);
					index--;
					//compare first target & second target
					freeaddition_value[index] = freeaddition_rest / (DiscSale.discIndexData.discTarget[index] + DiscSale.discIndexData.discPrice[index]);
					if((freeaddition_rest - DiscSale.discIndexData.discTarget[index+1]>DiscSale.discIndexData.discPrice[index] * freeaddition_value[index])){ // second target
						pPrice->Weight[PRICE_RESULT] -= freeaddition_rest - DiscSale.discIndexData.discTarget[index+1];
					}
					else{ //first target
						pPrice->Weight[PRICE_RESULT] -= DiscSale.discIndexData.discPrice[index] * freeaddition_value[index];
						freeaddition_rest = freeaddition_rest % (DiscSale.discIndexData.discTarget[index] + DiscSale.discIndexData.discPrice[index]);
						if(freeaddition_rest > DiscSale.discIndexData.discTarget[index]) pPrice->Weight[PRICE_RESULT] -= (freeaddition_rest - DiscSale.discIndexData.discTarget[index]);
					}
				}
				pPrice->TotalPrice[PRICE_RESULT]	=CalcWeightBasedPrice(Operation.MaxTotalPrice,pPrice->Weight[PRICE_RESULT],pPrice->UnitPrice[PRICE_RESULT]);
				break;
			case DISC_FREE_ADDITION_COUNT:
				if (index == 0)
				{
					//first target
					freeaddition_value[index] = pPrice->Qty[PRICE_ORIGIN] / (DiscSale.discIndexData.discTarget[index] + DiscSale.discIndexData.discPrice[index]);
					pPrice->Qty[PRICE_RESULT] -= DiscSale.discIndexData.discPrice[index] * freeaddition_value[index];
					freeaddition_rest = pPrice->Qty[PRICE_ORIGIN] % (DiscSale.discIndexData.discTarget[index] + DiscSale.discIndexData.discPrice[index]);
					if(freeaddition_rest > DiscSale.discIndexData.discTarget[index]) pPrice->Qty[PRICE_RESULT] -= (freeaddition_rest - DiscSale.discIndexData.discTarget[index]);
				}
				if (index == 1)
				{	//Second Target
					freeaddition_value[index] = pPrice->Qty[PRICE_ORIGIN] / (DiscSale.discIndexData.discTarget[index] + DiscSale.discIndexData.discPrice[index]);
					pPrice->Qty[PRICE_RESULT] -= DiscSale.discIndexData.discPrice[index] * freeaddition_value[index];
					freeaddition_rest = pPrice->Qty[PRICE_ORIGIN] % (DiscSale.discIndexData.discTarget[index] + DiscSale.discIndexData.discPrice[index]);
					index--;
					//compare first target & second target
					freeaddition_value[index] = freeaddition_rest / (DiscSale.discIndexData.discTarget[index] + DiscSale.discIndexData.discPrice[index]);
					if((freeaddition_rest - DiscSale.discIndexData.discTarget[index+1]>DiscSale.discIndexData.discPrice[index] * freeaddition_value[index])){ // second target
						pPrice->Qty[PRICE_RESULT] -=freeaddition_rest - DiscSale.discIndexData.discTarget[index+1];
					}
					else { //first target
						pPrice->Qty[PRICE_RESULT] -=DiscSale.discIndexData.discPrice[index] * freeaddition_value[index];
						freeaddition_rest = freeaddition_rest % (DiscSale.discIndexData.discTarget[index] + DiscSale.discIndexData.discPrice[index]);
						if(freeaddition_rest > DiscSale.discIndexData.discTarget[index]) pPrice->Qty[PRICE_RESULT] -= (freeaddition_rest - DiscSale.discIndexData.discTarget[index]);
					}
				}
				pPrice->TotalPrice[PRICE_RESULT]	=CalcByCountPrice(pPrice->Qty[PRICE_RESULT],pPrice->UnitCount[PRICE_RESULT],pPrice->UnitPrice[PRICE_RESULT]);
				break;
			case DISC_PRICE_UPRICE_PLUS:
				if(GlbOper.applyUnitpriceForDisc || Operation.specialPriceDisc)
				{
					if(DiscSale.discIndexData.discPrice[index])
						pPrice->UnitPrice[PRICE_RESULT] =DiscSale.discIndexData.discPrice[index];
				}
				else
				{
					if(0L < pPrice->UnitPrice[PRICE_ORIGIN]-DiscSale.discIndexData.discPrice[index])
					{
						pPrice->UnitPrice[PRICE_RESULT] =DiscSale.discIndexData.discPrice[index];
					}
					else
					{
						pPrice->UnitPrice[PRICE_RESULT] =pPrice->UnitPrice[PRICE_ORIGIN];
					}
				}
				if(PluType()==PLU_BY_WEIGHT)
					pPrice->TotalPrice[PRICE_RESULT]	=CalcWeightBasedPrice(Operation.MaxTotalPrice,pPrice->Weight[PRICE_ORIGIN],pPrice->UnitPrice[PRICE_RESULT]);
				else	// count
					pPrice->TotalPrice[PRICE_RESULT]	=CalcByCountPrice(pPrice->Qty[PRICE_RESULT],pPrice->UnitCount[PRICE_RESULT],pPrice->UnitPrice[PRICE_RESULT]);
				break;
			case DISC_N_PLUS_1:
				//first target
				if(PluType()==PLU_BY_WEIGHT)
				{
					freeaddition_value[index] = status_scale.cur_qty/(DiscSale.discIndexData.discTarget[0]+1);
					pPrice->Weight[PRICE_RESULT] = pPrice->Weight[PRICE_ORIGIN]*(status_scale.cur_qty-freeaddition_value[index])/status_scale.cur_qty;
					pPrice->TotalPrice[PRICE_RESULT]	=CalcWeightBasedPrice(Operation.MaxTotalPrice,pPrice->Weight[PRICE_ORIGIN]*(status_scale.cur_qty-freeaddition_value[index]),pPrice->UnitPrice[PRICE_ORIGIN])/status_scale.cur_qty;
				}
				else
				{
					freeaddition_value[index] = pPrice->Qty[PRICE_ORIGIN]/(DiscSale.discIndexData.discTarget[0]+1);
					pPrice->Qty[PRICE_RESULT] = pPrice->Qty[PRICE_ORIGIN]-freeaddition_value[index];
					pPrice->TotalPrice[PRICE_RESULT]	=CalcByCountPrice(pPrice->Qty[PRICE_RESULT],pPrice->UnitCount[PRICE_ORIGIN],pPrice->UnitPrice[PRICE_ORIGIN]);
				}
				break;
		}
		//new
		if(GlbPrice.roundApply == ROUND_ALL_PLU || GlbPrice.roundApply == ROUND_PLU_ROUND_CUT ||
				(GlbPrice.roundApply == ROUND_TOTAL_OR_PLU && Operation.operationClerk==OPER_STD_CLERK && !Operation.addStandatdClerk))//Price.DisplayPrice, Price.RoundedPrice
			pPrice->TotalPrice[PRICE_RESULT]	=PriceRounds(pPrice->TotalPrice[PRICE_RESULT], GlbPrice.roundApply,OFF,&roundDifference);
		//transPay.summaryround = roundDifference;

		pPrice->Weight[PRICE_SAVED]=pPrice->Weight[PRICE_ORIGIN]-pPrice->Weight[PRICE_RESULT];
		pPrice->Qty[PRICE_SAVED]=pPrice->Qty[PRICE_ORIGIN]-pPrice->Qty[PRICE_RESULT];
		pPrice->UnitCount[PRICE_SAVED]=pPrice->UnitCount[PRICE_ORIGIN]-pPrice->UnitCount[PRICE_RESULT];
#ifdef USE_TOPMART_DEFAULT
		if(DiscSale.discIndexData.discPriceType != DISC_PRICE_TOTALTOUNIT) //sewon 170119
		{
#endif
		pPrice->UnitPrice[PRICE_SAVED]=pPrice->UnitPrice[PRICE_ORIGIN]-pPrice->UnitPrice[PRICE_RESULT];
		pPrice->TotalPrice[PRICE_SAVED]=pPrice->TotalPrice[PRICE_ORIGIN]-pPrice->TotalPrice[PRICE_RESULT];
#ifdef USE_TOPMART_DEFAULT
		} //sewon 170119
#endif
		// 최종가격 결정및 할인된 금액 산출

		//SG061214
		if(GlbOper.UseFSP && DiscSale.saleDiscFSP)
		{
			//PRICE_TYPE  PriceFSP;

			PriceFSP.Weight[PRICE_RESULT] = pPrice->Weight[PRICE_RESULT];
			PriceFSP.Qty[PRICE_RESULT] = pPrice->Qty[PRICE_RESULT];
			PriceFSP.UnitCount[PRICE_RESULT] = pPrice->UnitCount[PRICE_RESULT];
			PriceFSP.UnitPrice[PRICE_RESULT] = pPrice->UnitPrice[PRICE_RESULT];
			PriceFSP.TotalPrice[PRICE_RESULT] = pPrice->TotalPrice[PRICE_RESULT];

			pPrice->Weight[PRICE_RESULT] = pPrice->Weight[PRICE_ORIGIN];
			pPrice->Qty[PRICE_RESULT] = pPrice->Qty[PRICE_ORIGIN];
			pPrice->UnitCount[PRICE_RESULT] = pPrice->UnitCount[PRICE_ORIGIN];
			pPrice->UnitPrice[PRICE_RESULT] = pPrice->UnitPrice[PRICE_ORIGIN];
			pPrice->TotalPrice[PRICE_RESULT] = pPrice->TotalPrice[PRICE_ORIGIN];
		}

		//sale_display_update(UPDATE_UNIT);
		return	DiscSale.saleDiscountType;
}

//DEL20060404int PercentTare(void)
//DEL20060404{
//DEL20060404	if(Price.PercentTareWeight)	return ON;
//DEL20060404	return OFF;
//DEL20060404}

void sale_get_price(INT16S linkedPLU, INT16S pluType, long pluid, PRICE_TYPE *pPrice,long tWeight,INT16S tQty,INT16S tPcs,long tUnitPrice,long tTare)
{
	//	INT16S    index;
	//	INT16S	wDecimal;// =(INT16S)get_global_bparam(GLOBAL_DISPLAY_WEIGHTDECIMAL);
	INT16S	pDecimal = (INT32U)get_global_bparam(GLOBAL_AREA_PRICE_DECIMALPOINT);
	INT16S	minusTare;
	//long	PercentTareLimit = 0;
	long 	tempWeight;
#ifdef USE_PERCENT_WEIGHT
	INT8U	precentweight_mode;
#endif
	//	wDecimal = 3;

	//sprintf(gmsg,"%d",get_defaultunit());
	//PutSmallString(3,0,gmsg);
	//	if(get_defaultunit()<=100)
	//		wDecimal=2;
	//wDecimal=2;
	//long ad_cnv_weight_value(lkeyapp_plunoong val)	// 무게값을 Interval 에 맞게 표현하도록 변환

	//index = PRICE_SAVED;
	pPrice->Weight[PRICE_SAVED] = 0L;
	pPrice->Qty[PRICE_SAVED]= 0L;
	pPrice->UnitCount[PRICE_SAVED] = 0L;
	pPrice->UnitPrice[PRICE_SAVED] = 0L;
	pPrice->TotalPrice[PRICE_SAVED] = 0L;

	//index = PRICE_ORIGIN;
	//sprintf(gmsg,"[%6ld]",status_scale.Plu.fixed_weight);
	//PutNSmallString(3,0,gmsg,20);
	pPrice->PercentTareWeight = 0;
	pPrice->WeightReference = 0;
	pPrice->GrossWeightFlag = OFF;
	pPrice->PercentTareRate = 0;
	pPrice->GrossWeight = 0;
	pPrice->NetWeight = 0;

#ifdef USE_PERCENT_WEIGHT
	precentweight_mode = get_global_bparam(GLOBAL_SALE_SETUP_FLAG19);
	precentweight_mode = precentweight_mode&0x01;
#endif

	if(!status_scale.cur_pluid)
	{
		tUnitPrice = status_scale.cur_keyvalue;
	}

	if(status_scale.Plu.fixed_weight && pluid) {
		pPrice->Weight[PRICE_ORIGIN]		= status_scale.Plu.fixed_weight;
	}
	else
	{
		if(status_scale.cur_pluid && status_scale.Plu.tare_percent && 0<tWeight)
		{
			long	tempWeight;

#ifdef USE_PERCENT_WEIGHT
			tempWeight = longmuldivNoRound(tWeight,status_scale.Plu.tare_percent,10000);
#else
			tempWeight = longmuldiv(tWeight,status_scale.Plu.tare_percent,10000);
#endif
			tempWeight = ad_cnv_weight_value(tempWeight,1);
			pPrice->PercentTareWeight = tempWeight;
			pPrice->PercentTareRate = (long)status_scale.Plu.tare_percent;
		}
		//		PercentTareOver = OFF;
		//		if(TareOper.tareKey+TareOper.tarePLU+TareOper.tareWeight+pPrice->PercentTareWeight > ad_select_tarelimit(AD_PROPER_TARE, 0, CurPluWeightUnit)) {
		//			PercentTareOver = ON;
		//			sale_display_update_error(0xE001,0,0);
		//			sale_display_update(UPDATE_WEIGHT|UPDATE_PRICE);
		//		}
		minusTare = OFF;
		if(status_scale.Plu.tare_limit)
		{
			if(status_scale.Plu.tare_limit < status_scale.Plu.tare_percent)
			{
				minusTare = ON;
			}
			else
				minusTare = OFF;
		}
		else
		{
			if(status_scale.Plu.tare_percent)
				minusTare = ON;
		}

		pPrice->GrossWeight		= ad_cnv_weight_value(tWeight + tTare, 1);
#ifndef USE_PERCENT_WEIGHT
		pPrice->NetWeight		= ad_cnv_weight_value(tWeight - pPrice->PercentTareWeight, 1);
#else //ifdef USE_RUSSIA
		pPrice->NetWeight		= ad_cnv_weight_value(tWeight, 1);
#endif
		if(minusTare)	// Percent Tare 적용 (일반상황)
		{
			pPrice->Weight[PRICE_ORIGIN]		= pPrice->NetWeight;
			//pPrice->WeightReference	      	= tWeight;
		}
		else		// Percent Tare 미적용
		{
			pPrice->GrossWeightFlag = ON;
			pPrice->Weight[PRICE_ORIGIN]		= tWeight;	// pPrice->NetWeight + pPrice->PercentTareWeight
		}

	}
	if (pluType != PLU_BY_WEIGHT)	// for USA
	{
		pPrice->NetWeight = ad_cnv_weight_value(tWeight - pPrice->PercentTareWeight, 1);
		if (pPrice->NetWeight <= 0 && status_scale.Plu.fixed_weight)
		{
			pPrice->NetWeight = status_scale.Plu.fixed_weight;
		}
	}

	//SG070419--------------------
	tempWeight = pPrice->Weight[PRICE_ORIGIN];

	if(GlbPrice.roundWeightType) //round down
	{
		tempWeight = tempWeight *10;
		tempWeight /= GlbPrice.roundWeight;
		if(tempWeight<0) tempWeight-=9;
		tempWeight /= 10;
		tempWeight *= GlbPrice.roundWeight;
	}
	else  //round off
	{
		tempWeight = tempWeight *10 / GlbPrice.roundWeight;
		tempWeight = (tempWeight + 5);
		if(tempWeight<0) tempWeight-=10;
		tempWeight /= 10;
		tempWeight *= GlbPrice.roundWeight;
	}

	pPrice->Weight[PRICE_ORIGIN] = tempWeight;

	//----------------------------

	pPrice->Qty[PRICE_ORIGIN]			= tQty;
	pPrice->UnitCount[PRICE_ORIGIN]	= tPcs;
#ifdef USE_TOPMART_STRUCTURE //sewon 161128
	pPrice->UnitPrice[PRICE_ORIGIN]	= tUnitPrice;
#else
	if (Operation.specialPrice && !linkedPLU)
		pPrice->UnitPrice[PRICE_ORIGIN] = status_scale.Plu.special_price;
	else if(Operation.memberPrice && !linkedPLU)
		pPrice->UnitPrice[PRICE_ORIGIN] = status_scale.Plu.member_price;
	else
		pPrice->UnitPrice[PRICE_ORIGIN]	= tUnitPrice;
#endif
	
	if (Operation.fixedPrice)
	{
		pPrice->TotalPrice[PRICE_ORIGIN]	= pPrice->UnitPrice[PRICE_ORIGIN];
	}
	else
	{
		switch(pluType)
		{
			default:
			case PLU_BY_WEIGHT:
				// BMLIM				pPrice->TotalPrice[PRICE_ORIGIN]	=CalcWeightBasedPrice(wDecimal,Operation.MaxTotalPrice,pPrice->Weight[PRICE_ORIGIN],pPrice->UnitPrice[PRICE_ORIGIN]);
#ifdef USE_PERCENT_WEIGHT
				if(precentweight_mode)
			{
				pPrice->TotalPrice[PRICE_ORIGIN]	=CalcWeightBasedPrice(Operation.MaxTotalPrice,pPrice->Weight[PRICE_ORIGIN],pPrice->UnitPrice[PRICE_ORIGIN]);			
			}
			else
			{
				if(pPrice->PercentTareWeight)
					pPrice->TotalPrice[PRICE_ORIGIN]=CalcWeightBasedPrice(Operation.MaxTotalPrice,pPrice->PercentTareWeight,pPrice->UnitPrice[PRICE_ORIGIN]);
				else
					pPrice->TotalPrice[PRICE_ORIGIN]=CalcWeightBasedPrice(Operation.MaxTotalPrice,pPrice->Weight[PRICE_ORIGIN],pPrice->UnitPrice[PRICE_ORIGIN]);
			}
#else
				pPrice->TotalPrice[PRICE_ORIGIN]	=CalcWeightBasedPrice(Operation.MaxTotalPrice,pPrice->Weight[PRICE_ORIGIN],pPrice->UnitPrice[PRICE_ORIGIN]);
#endif
				break;
			case PLU_BY_PCS:
				pPrice->TotalPrice[PRICE_ORIGIN]	=CalcByCountPrice(pPrice->Qty[PRICE_ORIGIN],pPrice->UnitCount[PRICE_ORIGIN],pPrice->UnitPrice[PRICE_ORIGIN]);
				break;
			case PLU_BY_COUNT:
				if(CheckPriceOverflow(pPrice->Qty[PRICE_ORIGIN] ,pPrice->UnitPrice[PRICE_ORIGIN]))
					pPrice->TotalPrice[PRICE_ORIGIN]	= 0;
				else
					pPrice->TotalPrice[PRICE_ORIGIN]	= pPrice->UnitPrice[PRICE_ORIGIN] * pPrice->Qty[PRICE_ORIGIN];
				break;
		}

		//CJK080214		if((status_scale.cur_weightunit == 100) && (Startup.country ==COUNTRY_US))	//Added by JJANG 20070828    100G 일때,
		//CJK080214		{
		//CJK080214			pPrice->TotalPrice[PRICE_ORIGIN] *= 10;
		//CJK080214		}

	}
	SaleGetDiscPrice(linkedPLU,pPrice,pluType);
	// plus linked price

	//Packing price Add
	if(Operation.packingPrice)
		pPrice->TotalPrice[PRICE_RESULT] += status_scale.Plu.packing_price;
	
	// tax
	if(pPrice->ExTax)
		//// BMLIM pPrice->TaxPrice = CalcWeightBasedPrice(pDecimal+2,Operation.MaxTotalPrice,pPrice->TaxRate,pPrice->TotalPrice[PRICE_RESULT]);
		// pPrice->TaxPrice = CalcWeightBasedPrice(Operation.MaxTotalPrice,pPrice->TaxRate,pPrice->TotalPrice[PRICE_RESULT]);
		pPrice->TaxPrice = longmuldiv(pPrice->TotalPrice[PRICE_RESULT],pPrice->TaxRate,10000);
	else
		// BMLIM	pPrice->TaxPrice = InternalTaxPrice(pDecimal+2,Operation.MaxTotalPrice,pPrice->TaxRate,pPrice->TotalPrice[PRICE_RESULT]);
		pPrice->TaxPrice = InternalTaxPrice(pPrice->TaxRate,pPrice->TotalPrice[PRICE_RESULT]);
	//msg
	//sprintf(gmsg,"Tax %ld",pPrice->TaxPrice);
	//PutSmallString(2,0,gmsg);

}

// CJK071122
void CalcDisplayedPrice(void)
{
	long totalPrice;
	INT8U Discount_flag;
	INT16U i;
	INT8U use_packingprice_flag;

	Discount_flag = get_global_bparam(GLOBAL_SALE_SETUP_FLAG7);
	use_packingprice_flag = get_global_bparam(GLOBAL_SALE_SETUP_FLAG19) & 0x02;	//parameter 743

	if(!(Discount_flag&0x01)) totalPrice = Price.TotalPrice[PRICE_RESULT];
	else totalPrice = Price.TotalPrice[PRICE_ORIGIN];

	if( Price.ExTax && GlbOper.priceAddExTax)//OPTION:exTax를 price에 포함
	{
		totalPrice += Price.TaxPrice;
	}
	if( !Price.ExTax && !GlbOper.priceAddExTax)//OPTION:inTax를 display price뺌
	{
		totalPrice -= Price.TaxPrice;
	}
#ifndef USE_PLU_MULTIPLE_PRICE	//Refefence PLU 기능 사용 안 함
	for(i=0;i<MAX_LINKED_PLU && status_scale.linked_plu[i];i++)
	{
		totalPrice += LinkedPrice[i].TotalPrice[PRICE_RESULT];
		if( LinkedPrice[i].ExTax && GlbOper.priceAddExTax)//OPTION:exTax를 display price에 포함
		{
			totalPrice += LinkedPrice[i].TaxPrice;
		}
		if( !LinkedPrice[i].ExTax && !GlbOper.priceAddExTax)//OPTION:inTax를 display price뺌
		{
			totalPrice -= LinkedPrice[i].TaxPrice;
		}
	}
#endif
	if (use_packingprice_flag)
		totalPrice -= status_scale.Plu.packing_price;
	Price.DisplayedPrice = totalPrice;
	if(GlbPrice.roundApply == ROUND_ALL_PLU ||
			(GlbPrice.roundApply == ROUND_TOTAL_OR_PLU && Operation.operationClerk==OPER_STD_CLERK && !Operation.addStandatdClerk))//Price.DisplayPrice, Price.RoundedPrice
	{
		Price.DisplayedPrice = PriceRounds(totalPrice,  ROUND_ALL_PLU,ON, NULL);
	}
}

void sale_calcprice(void)
{
	INT16S    index,pluType;
	long tareTotal;
	static long oldPrice = 0;
	//DEL060829	long tareLimit;


	Operation.OverUnitPrice=OFF;
	Operation.OverTotalPrice=OFF;
	pluType = PluType();
	sale_get_price(0,pluType,status_scale.cur_pluid,
			&Price,
			status_scale.Weight,
			status_scale.cur_qty,
			status_scale.cur_pcs,
			status_scale.cur_unitprice,
			status_scale.Tare);
	if(status_scale.Plu.member_price)
	{
		Operation.memberPrice= 1;
		sale_get_price(0,pluType,status_scale.cur_pluid,
						&MemberPrice,
						status_scale.Weight,
						status_scale.cur_qty,
						status_scale.cur_pcs,
						status_scale.cur_unitprice,
						status_scale.Tare);
		Operation.memberPrice= 0;		
	}	
	else
	{
		memset(&MemberPrice, 0, sizeof(MemberPrice));
	}
	// Added by CJK 20060405

	PercentTareOver = OFF;
#ifndef USE_PERCENT_WEIGHT
	tareTotal = ad_cnv_weight_value(TareOper.tareKey+TareOper.tarePLU+TareOper.tareWeight+Price.PercentTareWeight, 1);
#else // ifdef USE_RUSSIA
	tareTotal = ad_cnv_weight_value(TareOper.tareKey+TareOper.tarePLU+TareOper.tareWeight, 1);
#endif
	//tareLimit = ad_select_tarelimit(AD_PROPER_TARE, 0, CurPluWeightUnit);
	//sprintf(MsgBuf, "total=%ld, lmt=%ld, pertare=%ld\r\n", tareTotal, tareLimit, Price.PercentTareWeight);
	//MsgOut(MsgBuf);
	//if(tareTotal > tareLimit) {
	if(!ad_check_tarelimit(tareTotal)) {
		PercentTareOver = ON;
		sale_display_update_error(0xE001,0,0);
		sale_display_update(UPDATE_WEIGHT|UPDATE_PRICE);
	}
	////////////////////////
	if (DiscSale.saleDiscountType && oldPrice != Price.TotalPrice[PRICE_ORIGIN])
	{
		sale_display_update(UPDATE_TITLE);
		oldPrice = Price.TotalPrice[PRICE_ORIGIN];
	}
#ifdef USE_PERCENT_WEIGHT
	if (status_scale.cur_ptare)
	{
		sale_display_update(UPDATE_TITLE);
	}
#endif
	//msg
	//sprintf(gmsg,"%6ld,%6ld",Price.TotalPrice[PRICE_RESULT],Price.TaxPrice);
	//PutNSmallString(0,0,gmsg,20);
#ifndef USE_PLU_MULTIPLE_PRICE	//Refefence PLU 기능 사용 안 함
	if(DiscSale.saleDiscountType)
	{
		if(DiscSale.discIndexData.discPriceType == DISC_PRICE_FREE || DiscSale.discIndexData.discPriceType == DISC_PRICE_FIXED)
		{
			for(index=0;index<MAX_LINKED_PLU;index++)
			{
				status_scale.linked_dept[index] = 0;
				status_scale.linked_plu[index] = 0;
			}
		}
	}

	// linked price
	for(index=0;index<MAX_LINKED_PLU;index++)
	{
		// 라벨 출력이 아니면 linked성립
		//if(!Operation.printAdd && status_scale.linked_dept[index] && status_scale.linked_plu[index])
		if(status_scale.linked_dept[index] && status_scale.linked_plu[index])
		{
			INT16S t_qty;

			t_qty = Price.Qty[PRICE_ORIGIN];
			if(PluType() == PLU_BY_WEIGHT)
			{
				if(status_scale.LinkedPlu[index].ptype != PLU_BY_WEIGHT)
					t_qty = 1;

			}
			if(status_scale.LinkedPlu[index].ptype == PLU_BY_COUNT)//무조건 1개
				t_qty = 1;
			sale_get_price(1,status_scale.LinkedPlu[index].ptype,status_scale.linked_plu[index],
					&LinkedPrice[index],
					status_scale.Weight,
					t_qty,//status_scale.LinkedPlu[index].qty,
					status_scale.LinkedPlu[index].pcs,
					status_scale.LinkedPlu[index].unitprice,
					status_scale.Tare);
		}
		else
		{
			sale_get_price(1,1,0,&LinkedPrice[index],0,0,0,0,0);
		}
	}
#endif	//USE_PLU_MULTIPLE_PRICE
	CalcDisplayedPrice();
}

#ifdef USE_KOR_TRACE
extern INT8U KorTr_flagBarInfo;
void sale_display_kortrace(INT8U dispFlag) 	// Added by CJK 20060310
{
	INT16S x; //,display;
	//char string_buf[50];

	POINT p;
	DspLoadFont1(DSP_MSG_FONT_ID);



	if(dispFlag) {
	}
	else
	{
//		if(network_status.service_bflag2){
			memset((INT8U *)KorTrace_Flag.CurBarData,0x00,sizeof(KorTrace_Flag.CurBarData)); //SG060331
			memset((INT8U *)KorTrace_Flag.barcode_data,0x00,sizeof(KorTrace_Flag.barcode_data));
			strset((char *)KorTrace_Flag.display_data,0x20,23);
//			strset(string_buf,0x20,23);
//		}
	}

	x=0;
	p = point(0,x);
//	string_buf[22]=0;
	KorTrace_Flag.display_data[22]=0;
	Dsp_Write_String(p,( char *)KorTrace_Flag.display_data);
//	Dsp_Write_String(p,( char *)string_buf);
	Dsp_Diffuse();

}
#endif

#ifdef USE_TRACE_STANDALONE

#define	CHARACTER_FIXED_POS	14
extern TRACE_STATUS_STRUCT CalledTraceStatus;
void sale_display_indivFlag_set(INT8U mode, INT8U dspFlag)
{
	if(dspFlag)
	{
		if (mode&0x01)
		{
			FlagDisplayIndividual = ON;
		}
		if (mode&0x02)
		{
			FlagDisplaySpecialUse = ON;
		}
		if (mode&0x04)
		{
			FlagDisplaySlaughtHouse = ON;
		}
#ifdef USE_TRACE_MEATPART
		if (mode&0x08)
		{
			FlagDisplayMeatPart = ON;
		}
#endif
#ifdef USE_TRACE_MEATGRADE
		if (mode&0x10)
		{
			FlagDisplayMeatGrade = ON;
		}
#endif
	}
	else
	{
		if (mode&0x01)
		{
			FlagDisplayIndividual = OFF;
			memset(&CurTraceStatus.indivStr, 0, sizeof(CurTraceStatus.indivStr));
			//memset(&CurTraceStatus.curGradeText, 0, sizeof(CurTraceStatus.curGradeText));
			memset(&CalledTraceStatus.indivStr, 0, sizeof(CalledTraceStatus.indivStr));
			//CurTraceStatus.gradeNo = 0;
		}
		if (mode&0x02)
		{
			FlagDisplaySpecialUse = OFF;
			CurTraceStatus.meatUse = 0;
			CalledTraceStatus.meatUse = 0;
		}
		if (mode&0x04)
		{
			FlagDisplaySlaughtHouse = OFF;
			CurTraceStatus.slPlace = 0;
			CalledTraceStatus.slPlace = 0;
		}
#ifdef USE_TRACE_MEATPART
		if (mode&0x08)
		{
			FlagDisplayMeatPart = OFF;
			CurTraceStatus.meatPart = 0;
			CalledTraceStatus.meatPart = 0;
		}
#endif
#ifdef USE_TRACE_MEATGRADE
		if (mode&0x10)
		{
			FlagDisplayMeatGrade = OFF;
			CurTraceStatus.gradeNo = 0;
			CalledTraceStatus.gradeNo = 0;
		}
#endif
		//if (CurTraceStatus.gradeNo == 0)
		//{
		//	memset(&CurTraceStatus.curGradeText, 0, sizeof(CurTraceStatus.curGradeText));
		//}
	}
}

/*
 void sale_display_indivIndex(void)
 {
 char string_buf[66];
 POINT p;
 INT16S w,x;

 DspLoadFont1(DSP_PLU_FONT_ID);

 x=CHARACTER_FIXED_POS;
 w=display_font_get_width();
 p = point(8,176);  //fixed position

 if(FlagDisplayIndividual && CurTraceStatus.indivStr.index != 100)
 {
 sprintf(string_buf, "%02d", CurTraceStatus.indivStr.index);
 string_buf[2]=0;
 DspStruct.Reverse = 1;
 Dsp_Write_String(p,string_buf);
 DspStruct.Reverse = 0;
 }
 else
 {
 strcpy(string_buf,"  ");
 string_buf[2]=0;
 Dsp_Write_String(p,string_buf);
 }
 }
 */

void sale_display_indivIndex(void)
{
	char string_buf[66];
	POINT p;
	INT16S w,x;
	INT8U	ucTemp;
	INT8U	useSmallFont;
#ifdef USE_NHMART_SAFE_MEAT
	if(checkMtrace)
	{
		return;
	}
#endif
   	ucTemp = get_global_bparam(GLOBAL_SALE_SETUP_FLAG10);
	useSmallFont = (ucTemp>>7) & 0x01;

	if (useSmallFont) DspLoadFont1(DSP_MEDIUM_FONT_ID);
	else DspLoadFont1(DSP_PLU_FONT_ID);

	//x=CHARACTER_FIXED_POS;
	w=display_font_get_width();
	//if (useSmallFont) p = point(DSP_PLU_NAME_Y, LCD_Y_MARGIN-w*2);  //fixed position
	//else p = point(DSP_PLU_NAME_Y, LCD_Y_MARGIN-w*2);  //fixed position
#ifdef USE_KOR_TRACE_999
	p = point(DSP_PLU_NAME_Y, LCD_Y_MARGIN-w*3);  //fixed position
#else
	p = point(DSP_PLU_NAME_Y, LCD_Y_MARGIN-w*2);  //fixed position
#endif

	if(FlagDisplayIndividual && CurTraceStatus.indivStr.index < MaxIndividualNo)
	{
#ifdef USE_KOR_TRACE_999
		sprintf(string_buf, "%03d", CurTraceStatus.indivStr.index%MaxIndividualNo);
#else
		sprintf(string_buf, "%02d", CurTraceStatus.indivStr.index%MaxIndividualNo);
#endif
		string_buf[3]=0;
		DspStruct.Reverse = 1;
		Dsp_Write_String(p,string_buf);
		DspStruct.Reverse = 0;
	}
}

void sale_display_speicaluse(void)
{
	char string_buf[66];
	POINT p;
	INT16S w;
 	INT32U	addr;
	INT8U	ucTemp;
	INT8U	useSmallFont;

	ucTemp = get_global_bparam(GLOBAL_SALE_SETUP_FLAG10);
	useSmallFont = (ucTemp>>7) & 0x01;

	addr =  DFLASH_BASE + FLASH_TRACE_SPECIALUSE_AREA;
	addr += sizeof(TRACE_SPECIALUSE_STRUCT) * (INT32U)(CurTraceStatus.meatUse - 1);

	if (Flash_wread(addr) != CurTraceStatus.meatUse)
	{
		//BuzOn(2);
		return;
	}

	addr += 2; //index no

	Flash_sread(addr, (HUGEDATA INT8U *)string_buf, sizeof(TRACE_SPECIALUSE_STRUCT)-2);
	if (IsHanCharMid(string_buf, 4) == CHAR_HAN_MID)
	{
		string_buf[5] = 0;
	}
	else
	{
		string_buf[4] = 0; //한글 2글자, 영문 4글자
	}

	if (useSmallFont) DspLoadFont1(DSP_MEDIUM_FONT_ID);
	else DspLoadFont1(DSP_PLU_FONT_ID);

	//x=CHARACTER_FIXED_POS;
	w=display_font_get_width();
	if (useSmallFont) p = point(DSP_PLU_NAME_Y, LCD_Y_MARGIN-w*4);  //fixed position
	else p = point(DSP_PLU_NAME_Y, LCD_Y_MARGIN-w*4);  //fixed position

	DspStruct.Reverse = 1;
	Dsp_Write_String(p, string_buf);
	DspStruct.Reverse = 0;
}

void longToStringWithComma(long lNum, char *dest)
{
	char stemp[24];
	char i, j, k, np;
	INT8U leng=0, dleng=0;
	sprintf(stemp,"%ld",lNum);
	leng = strlen(stemp);
	np = (leng-1)/3;
	dleng = leng + np;
	*(dest+dleng)=0;
	for(i=leng-1, j=dleng-1, k=0; i>=0; i--, j--, k++)
	{
		if( (k%3)==0 && k>2 )
		{
			*(dest+j)=',';
			j--;
		}
		*(dest+j) = stemp[i];
	}
}
//#define INDIV_GRADE_LEN	8
void sale_display_individual(void)
{
#ifdef USE_EXTENDED_SCANNERFORM
	char string_buf[64];
#else
	char string_buf[32];
#endif
	char indiv_no[INDIVIDUAL_NO_LEN + 1];
	char indiv_grade[INDIV_GRADE_LEN];
	char indiv_weight[10];
	long weight;
	INT16U kgWeight;
	INT16U len;
	INT8U i,gradeSize,j;
	char temp_buf[10];


	DspLoadFont1(DSP_MSG_FONT_ID);
	memset(string_buf, 0, sizeof(string_buf));
	memset(temp_buf, 0, sizeof(temp_buf));

	if(FlagDisplayIndividual || FlagDisplaySlaughtHouse || FlagDisplaySpecialUse //Individual Display ON
#ifdef USE_TRACE_MEATPART
			|| FlagDisplayMeatPart
#endif
#ifdef USE_TRACE_MEATGRADE
			|| FlagDisplayMeatGrade
#endif
			)
	{
#ifdef USE_LOTTEMART_MEATSCANNER
		if(CurTraceStatus.meatscannerflag) 
		{
			memcpy(indiv_no,CurTraceStatus.indivno, 24);
		}
		else
		{
#endif
		memcpy(indiv_no, CurTraceStatus.indivStr.individualNO, sizeof(indiv_no));
#ifdef USE_LOTTEMART_MEATSCANNER
		}
#endif
		//		memcpy(indiv_grade, CurTraceStatus.curGradeText, sizeof(indiv_grade));
		//등급이 한글일 때, 한글은 제외하고 Display 됨
		gradeSize = strlen((char*)CurTraceStatus.curGradeText);
		j = 0;
		for (i=0;i<gradeSize;i++)
		{
			if(CurTraceStatus.curGradeText[i] > 0x80)
			{
				i++;
			}
			else
			{
				temp_buf[j] = CurTraceStatus.curGradeText[i];
				j++;
			}
		}
		memcpy(indiv_grade, temp_buf, sizeof(indiv_grade));

		weight = CurTraceStatus.indivWeight;
		if(weight <0) weight =0;
		indiv_no[INDIVIDUAL_NO_LEN] = 0;
		indiv_grade[INDIV_GRADE_LEN-1] = 0;
		kgWeight = (INT16U)(weight / 1000);
		memset(indiv_weight, 0, 10);
		if (ethernet_list.status != 2)	// if not slave
		{
			sprintf(indiv_weight, "%4d", kgWeight);
		}

		if (GlbOper.TraceabilityCallType <= 1)
		{
			sprintf(string_buf, "[%3s] %s", indiv_grade, indiv_no);
		}
		else
		{
			if (indiv_no[0] == 0)	// NULL 일경우 "      "으로 표시
			{
				memset(indiv_no, ' ', 12);
				indiv_no[12] = 0;
			}
			len = 0;
#ifdef USE_EMART_NETWORK
			if (FlagIndividualMultiStart || FlagIndividualMultiRemain)
			{
				string_buf[0] = 'M';	// 복수개체  표시
				string_buf[1] = MultiIndex + '0';	// 복수개체  순번 표시
				len = 2;
			}
#endif
			switch (GlbOper.TraceabilityCallType)
			{
				default:
					//case 0:
					//	string_buf[0] = 0;
					//case 1:
					//	sprintf(string_buf, "[%s]", indiv_no); //gm 090407
					//	break;
				case 2:
					sprintf(string_buf+len, "[%s],u%02d", indiv_no, CurTraceStatus.meatUse);
					break;
				case 3:	// GS Mart: 개체, 도축장, 용도 표시(개체,도축장만 기억)
					//sprintf(string_buf+len, "[%s],s%02d,u%02d", indiv_no, CurTraceStatus.slPlace, CurTraceStatus.meatUse);
					//break;
				case 4:
				case 5:
				case 6: //csh 20140303
#ifdef USE_KOR_SLAUGHT_CODE_4DIGIT
  #ifdef USE_TRACE_MEATPART
					sprintf(string_buf+len, "[%s],s%04d,u%02d,p%02d", indiv_no, trace_slaughthouse_index2no(CurTraceStatus.slPlace), CurTraceStatus.meatUse, CurTraceStatus.meatPart);
  #else
    #ifdef USE_EMART_PIG_TRACE
					if(ImportLotNumberflag == 1)
					{
						sprintf(string_buf+len, "%s,s%04d,u%02d", ImportLotNumber, trace_slaughthouse_index2no(CurTraceStatus.slPlace), CurTraceStatus.meatUse);
					}
					else
					{
						sprintf(string_buf+len, "[%s],s%04d,u%02d", indiv_no, trace_slaughthouse_index2no(CurTraceStatus.slPlace), CurTraceStatus.meatUse);
					}
    #else
					sprintf(string_buf+len, "[%s],s%04d,u%02d", indiv_no, trace_slaughthouse_index2no(CurTraceStatus.slPlace), CurTraceStatus.meatUse);
    #endif
  #endif
#else //USE_KOR_SLAUGHT_CODE_4DIGIT
  #ifdef USE_TRACE_MEATPART
    #ifdef USE_LOTTEMART_MEATSCANNER
				  if(CurTraceStatus.meatscannerflag)
				  {
					  sprintf(string_buf+len, "[%s]s%02du%02dp%02d", indiv_no, CurTraceStatus.slPlace1, CurTraceStatus.meatUseNo, 0);
				  }
				  else
				  {
					  sprintf(string_buf+len, "[%s]s%02du%02dp%02d", indiv_no, CurTraceStatus.slPlace, CurTraceStatus.meatUse, CurTraceStatus.meatPart);			  
				  }
    #else
			  	  sprintf(string_buf+len, "[%s]s%02du%02dp%02d", indiv_no, CurTraceStatus.slPlace, CurTraceStatus.meatUse, CurTraceStatus.meatPart);
    #endif
  #else //USE_TRACE_MEATPART
    #ifdef USE_NHMART_SAFE_MEAT
					sprintf(string_buf+len, "[%s], (%3s)", indiv_no, indiv_grade);
    #else
					sprintf(string_buf+len, "[%s],s%02d,u%02d", indiv_no, CurTraceStatus.slPlace, CurTraceStatus.meatUse);
    #endif
  #endif //USE_TRACE_MEATPART
#endif //USE_KOR_SLAUGHT_CODE_4DIGIT
					break;
			}
		}
		string_buf[24]=0;
		PutStringIndirect(DSP_MSG_Y, DSP_MSG_X, string_buf, DSP_SYSTEM_FONT_ID, 1, 1, MAX_MSG_CHAR);
	}
#ifdef USE_CTS_FUNCTION
	else if (!CTS_CheckCalibrationDate())
	{
		sale_display_cts_msg();
	}
#endif
	else 	    		//Individual Display OFF
	{
		strset(string_buf,0x20,24);
		string_buf[24]=0;
		PutStringIndirect(DSP_MSG_Y, DSP_MSG_X, string_buf, DSP_SYSTEM_FONT_ID, 1, 1, MAX_MSG_CHAR);
	}
	//string_buf[24]=0;
	//x=0;
	//p = point(0,x);
	//Dsp_Write_String(p,(HUGEDATA char *)string_buf);
	//Dsp_Diffuse();
	//PutStringIndirect(DSP_MSG_Y, DSP_MSG_X, string_buf, DSP_SYSTEM_FONT_ID, 1, 1, MAX_MSG_CHAR);

	/////////  Index display  //////////
	switch(ModeIndiv){
		default:
			//case 0://사용안함
			//case 1://신세계
			//case 3://GS Mart
			//case 5://이마트
			break;
		case 2://농협
		case 4://Standard (신세계 + 개체인덱스)
			sale_display_indivIndex(); //only NH Mart
			break;
	}
	/////////////////////////
	if (ModeIndiv != 5)	// 이마트가 아닐 때만 용도 표시
	{
		if (CurTraceStatus.meatUse != 0) sale_display_speicaluse();
	}
}

#endif


#ifdef USE_CHN_CART_SCALE
void sale_display_chn_tracecode(void)
{
	char string_buf[32];
	char temp_buf[TRACE_CODE_SIZE + 1];


	DspLoadFont1(DSP_MSG_FONT_ID);
	memset(string_buf, 0, sizeof(string_buf));

	if(CurCHNTrace.code[0] >= '0' && CurCHNTrace.code[0] <= '9')
	{
		memcpy(temp_buf, CurCHNTrace.code, TRACE_CODE_SIZE);
		temp_buf[TRACE_CODE_SIZE] = 0;
		//sprintf(string_buf, "[%s]%ld", temp_buf, CurCHNTraceRemainWeight);
		sprintf(string_buf, "[%s]", temp_buf);
	}
	else 	    		//Individual Display OFF
	{
		strset(string_buf, 0x20, 24);
	}
	string_buf[24]=0;
	PutStringIndirect(DSP_MSG_Y, DSP_MSG_X, string_buf, DSP_SYSTEM_FONT_ID, 1, 1, MAX_MSG_CHAR);

}
#endif

#ifdef USE_CHN_EMART_TRACE
void sale_display_chn_emart_tracecode(void)
{
	char string_buf[32];
	char temp_buf[CHN_EMART_TRACE_CODE_SIZE + 1];


	DspLoadFont1(DSP_MSG_FONT_ID);
	memset(string_buf, 0, sizeof(string_buf));

	if(CHNEmart_CurTraceCode[0] >= '0' && CHNEmart_CurTraceCode[0] <= '9')
	{
		memcpy(temp_buf, CHNEmart_CurTraceCode, CHN_EMART_TRACE_CODE_SIZE);
		temp_buf[CHN_EMART_TRACE_CODE_SIZE] = 0;
		//sprintf(string_buf, "[%s]%ld", temp_buf, CurCHNTraceRemainWeight);
#ifdef USE_CHN_EMART_BIRDS_TRACE
		sprintf(string_buf, "%s:%d", temp_buf, CHNEmart_TraceSelectionOrder+1); //가장 최근 이력코드 :1 가장 오래된 이력토드 :4
#else
		sprintf(string_buf, "[%s]", temp_buf);
#endif
	}
	else 	    		//Individual Display OFF
	{
		strset(string_buf, 0x20, 24);
	}
	string_buf[24]=0;
	PutStringIndirect(DSP_MSG_Y, DSP_MSG_X, string_buf, DSP_SYSTEM_FONT_ID, 1, 1, MAX_MSG_CHAR);
}
#endif

#if !defined(_USE_LCD20848_) && !defined(_USE_LCD32_)
void sale_display_error(void);
#endif
void sale_display_update_error(INT16U code,INT8U act,long val)
{
	status_scale.ErrKind  =code;
	status_scale.ErrAction=act;
	status_scale.ErrValue =val;
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
	sale_display_update(UPDATE_ERROR);
#else
	sale_display_error(); // 실시간 display
#endif
}

void sale_display_update(INT16U cc)
{
	status_scale.cur_displayon|= cc;
	status_scale.flag_display |= 1;
	//sprintf(MsgBuf, "f=%04x, r=%04x\r\n", cc, status_scale.cur_displayon);	//DBG090318
	//MsgOut(MsgBuf);
}

void sale_display_update_clear(void)
{
	status_scale.cur_displayon = 0;
	status_scale.flag_display = 0;
}

INT8U Seg7BlinkOn = 0;
INT16U Set7BlinkPrevTime;
extern void sale_display_multiply_msg(void);
void sale_display_weight(void)
{
	INT16U chk;
	INT8U smode;
	char string_buf[10], form[10];
	long temp,temp1;
	INT8U dlen1, dlen3, pluType;
	INT32U pnt;

#ifdef USE_UNDER_LIMIT_MINUS20D
	INT32S  interval;
#endif

	sale_display_update(UPDATE_WEIGHT);
#if !defined(_USE_LCD20848_) && !defined(_USE_LCD32_)
	if ((INT16U)(SysTimer100ms - Set7BlinkPrevTime) > 5)
	{
		Set7BlinkPrevTime = SysTimer100ms;
		Seg7BlinkOn ^= 1;
	}
#endif
	chk=UPDATE_WEIGHT;
	if ((status_scale.cur_displayon&chk) == 0) return;
	status_scale.cur_displayon&=(~chk);

	if (Operation.multiply) sale_display_multiply_msg();
	else
	{
		pluType = PluType();
		pnt = DFLASH_BASE ;
		pnt+= FLASH_GLOBAL_AREA;
		if (Operation.fixedPrice && !status_scale.Plu.fixed_weight)
		{
			sprintf(string_buf, "%-6s", global_message[MSG_06_FIXEDPRICE]);
			display_string(DISPLAY_WEIGHT,(INT8U *)string_buf);
		} else {
			switch (pluType) {
				case 0:
				case PLU_BY_WEIGHT:
#ifdef USE_UNDER_LIMIT_MINUS20D
					interval = (INT32S)ad_get_interval(0);
					interval = ad_cnv_adm2main(interval);
#endif
					Operation.UnderWeight = OFF;
					if(GlbOper.UseNegWeightLimit) {
						if(Price.GrossWeight <= -MaxCapaWeight*4/100) Operation.UnderWeight = ON;	// -4%
					}
#ifdef USE_UNDER_LIMIT_MINUS20D
					else
					{
						if(Price.GrossWeight < -20 * interval) Operation.UnderWeight = ON;	// -20e 미만 표시 안함(OIML2006)
					}
#endif
					if(ADM_GetData.Overload || Operation.UnderWeight || PercentTareOver)
					{
						strcpy(string_buf,"------");
						// sale_mode에서 TRUE를 사용하여 뒤에서부터 출력하고 최대 무게 초과 or 최소 무게 미만시 중량 칸에 "---" 문구를 표시하는 기능
						VFD7_display_string((HUGEDATA char *)string_buf, display_parameter.mode_pos[DISPLAY_WEIGHT], strlen(string_buf), DISPLAY_WEIGHT, TRUE);
					}
					else
					{
						temp = Price.Weight[PRICE_ORIGIN];
						temp1=temp;
						if(temp1<0) temp1=(-temp1);
						if((INT32U)temp1>(power10(5)-1))
						{
							strcpy(string_buf,"------");
							display_string(DISPLAY_WEIGHT,(INT8U *)string_buf);
						}
						else 	display_ulongsimple(DISPLAY_WEIGHT,temp);
					}
					break;
				case PLU_BY_PCS:
					dlen1= Flash_bread(pnt+GLOBAL_INPUT_BC_QTYLENGTH);
					dlen3= Flash_bread(pnt+GLOBAL_INPUT_BC_PCSLENGTH);
					if (display_parameter.mode_len[DISPLAY_WEIGHT] == 6)
						dlen1 += 1;
					if (status_scale.cur_ride == 1 && Seg7BlinkOn) //qty
					{
						string_buf[0] = 0;
						str_fill(string_buf, dlen1);
					}
					else
					{
						sprintf(form, "%%%dd", dlen1);
						sprintf(string_buf, form, Price.Qty[PRICE_RESULT]);
					}

					sprintf(&string_buf[dlen1], "-");

					if (status_scale.cur_ride == 2 && Seg7BlinkOn) //pcs
					{
						string_buf[dlen1+1] = 0;
						str_fill(&string_buf[dlen1+1], dlen3);
					}
					else
					{
						sprintf(form, "%%%dd", dlen3);
						sprintf(&string_buf[dlen1+1], form, Price.UnitCount[PRICE_RESULT]);
					}
					VFD7_display_string((HUGEDATA char *)string_buf, display_parameter.mode_pos[DISPLAY_WEIGHT], strlen(string_buf), DISPLAY_WEIGHT, TRUE);
					break;
				case PLU_BY_COUNT:
					dlen1= Flash_bread(pnt+GLOBAL_INPUT_BN_QTYLENGTH);
					if (display_parameter.mode_len[DISPLAY_WEIGHT] == 6)
						dlen1 += 1;
					if (status_scale.cur_ride == 1 && Seg7BlinkOn) //qty
					{
						string_buf[0] = 0;
						str_fill(string_buf, dlen1+2);
					}
					else
					{
						sprintf(form, "  %%%dd", dlen1);
						sprintf(string_buf, form, Price.Qty[PRICE_ORIGIN]);
					}
					VFD7_display_string((HUGEDATA char *)string_buf, display_parameter.mode_pos[DISPLAY_WEIGHT], strlen(string_buf), DISPLAY_WEIGHT, TRUE);
					break;
				default:
					break;
			}
		}
	}
}

void sale_display_tare(void)
{
	INT16U chk;
	INT8U smode;
	static long	prevPercentTare=0;
	static long	prevTare=0;
	long tempTare;
	char	str[10];

	if(TareOper.displayWeightTare || (TareOper.addupWeightZero && Operation.operationClerk == OPER_FLOATING))	// clerk tare 사용시 weight tare 강제표시
		tempTare = TareOper.tareKey+TareOper.tarePLU+TareOper.tareWeight;
	else
		tempTare = TareOper.tareKey+TareOper.tarePLU;

	if(PercentTareOver)
	{
		strcpy(str,"------");
		display_string(DISPLAY_TARE,(INT8U *)str);
		VFD7_Diffuse();
		return;
	}

	if(TareOper.displayWeightTare)
		tempTare = TareOper.tareKey+TareOper.tarePLU+TareOper.tareWeight;
	else
		tempTare = TareOper.tareKey+TareOper.tarePLU;

	smode=(status_scale.flag_input>>1)& 0x07;
	if (smode<2) {
#ifndef USE_PERCENT_WEIGHT
		if(!Price.GrossWeightFlag && Price.PercentTareWeight)//new
		{
			tempTare += Price.PercentTareWeight;
		}
#endif
	}
	else
	{
		if(!TareOper.displayTareEU)
		{
			tempTare = 0;
		}
	}
	//100831 TEST
	if(tempTare == 0 && TareOper.displayWeightTare == 0)
	{
		strcpy(str,"      ");
		display_string(DISPLAY_TARE,(INT8U *)str);
		VFD7_Diffuse();
		return;
	}

	if(prevTare != tempTare)
	{
		//status_scale.cur_displayon|=UPDATE_TARE;
		sale_display_update(UPDATE_TARE);
	}

	chk=UPDATE_TARE;
	if (status_scale.cur_displayon&chk)
	{
		status_scale.cur_displayon&=(~chk);
		display_ulongsimple(DISPLAY_TARE, tempTare);
		prevTare = tempTare;
	}
	/*
	 smode=(status_scale.flag_input>>1)& 0x07;
	 if (smode<2) {
	 if (status_scale.cur_displayon&chk || (prevPercentTare !=Price.PercentTareWeight))
	 {
	 if (status_scale.tare_type==3)
	 {
	 display_ulongsimple(DISPLAY_TARE,tempTare);
	 } else {
	 if(!Price.GrossWeightFlag && Price.PercentTareWeight)//new
	 display_ulongsimple(DISPLAY_TARE,tempTare+Price.PercentTareWeight);
	 else
	 display_ulongsimple(DISPLAY_TARE,tempTare);
	 }
	 status_scale.cur_displayon&=(~chk);
	 }
	 prevPercentTare = Price.PercentTareWeight;
	 }
	 else
	 {
	 if(!TareOper.displayTareEU)
	 display_ulongsimple(DISPLAY_TARE,0);
	 }
	 prevTare = tempTare;
	 */
}

void sale_display_tare_eu(void)
{
	if(TareOper.displayTareEU)// only weight// call plu
	{
		//if(TareOper.tareKey+TareOper.tarePLU)
		if(PluType()==PLU_BY_WEIGHT)
			display_tare(1);
		else
			display_clear(DISPLAY_TARE);
	}
	else
	{
		display_tare(1);
	}
}

#ifdef USE_INVENTORY_LABEL
extern long InventoryUnitWeight;
INT32U InventoryQuantity;
#endif
void sale_display_unit(void)
{
	INT8U chk,dlen1,dlen3,pluType;
	INT32U v32,pnt;
	char string_buf[10], form[10];
	//	static long prevValue;
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
	static long prevValue;
#else
	INT8U seg7UnitPriceBlinkOn;
#endif
	INT8U	Discount_flag;	//Added by JJANG 20071114

#if !defined(_USE_LCD20848_) && !defined(_USE_LCD32_)
	seg7UnitPriceBlinkOn = OFF;
#endif
	sale_display_update(UPDATE_UNIT);
	chk=UPDATE_UNIT;
	if (status_scale.cur_displayon&chk) {
		pluType = PluType();
		pnt = DFLASH_BASE ;
		pnt+= FLASH_GLOBAL_AREA;
		if(Operation.fixedPrice)
		{
			display_clear(DISPLAY_UNITPRICE);
		}
		else
		{
			Discount_flag = get_global_bparam(GLOBAL_SALE_SETUP_FLAG7);
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
			switch (pluType) {
				case PLU_BY_WEIGHT:
					if(!(Discount_flag&0x01)) v32=Price.UnitPrice[PRICE_RESULT];
					else v32=Price.UnitPrice[PRICE_ORIGIN];

					break;
				case 0:
					v32=status_scale.cur_keyvalue;
					break;
				case PLU_BY_PCS:
					dlen1= Flash_bread(pnt+GLOBAL_INPUT_BC_QTYLENGTH);
					dlen3= Flash_bread(pnt+GLOBAL_INPUT_BC_PCSLENGTH);
					sprintf(form, "%%%dd-%%%dd", dlen1, dlen3);
					sprintf(string_buf, form, Price.Qty[PRICE_RESULT], Price.UnitCount[PRICE_RESULT]);
					VFD7_display_string((HUGEDATA char *)string_buf, display_parameter.mode_pos[DISPLAY_WEIGHT], strlen(string_buf), DISPLAY_WEIGHT, TRUE);

					if(!(Discount_flag&0x01)) v32=Price.UnitPrice[PRICE_RESULT];
					else v32=Price.UnitPrice[PRICE_ORIGIN];
					break;
				case PLU_BY_COUNT:
					dlen1= Flash_bread(pnt+GLOBAL_INPUT_BN_QTYLENGTH);
					sprintf(form, "  %%%dd", dlen1);
					sprintf(string_buf, form, Price.Qty[PRICE_ORIGIN]);
					VFD7_display_string((HUGEDATA char *)string_buf, display_parameter.mode_pos[DISPLAY_WEIGHT], strlen(string_buf), DISPLAY_WEIGHT, TRUE);

					if(!(Discount_flag&0x01)) v32=Price.UnitPrice[PRICE_RESULT];
					else v32=Price.UnitPrice[PRICE_ORIGIN];
					break;
				default:
					break;
			}
#ifdef USE_INVENTORY_LABEL
			display_ulongsimple(DISPLAY_UNITPRICE, InventoryUnitWeight);
			sale_display_update(UPDATE_UNIT);
#else
			if(Operation.negative == ON)//added by yyc 20110520 독일요청사항으로 Negative Sale mode 동작시 Unit Price에 "-"를 표시.
                        {
                          display_ulongsimple(DISPLAY_UNITPRICE,-v32);
                        }
                        else  display_ulongsimple(DISPLAY_UNITPRICE,v32);
			prevValue = Price.UnitPrice[PRICE_RESULT];
#endif
#else
			switch (pluType) {
				case PLU_BY_WEIGHT:
					if (status_scale.cur_ride == 1 && Seg7BlinkOn) // unitprice of by-weight
					{
						seg7UnitPriceBlinkOn = ON;
					}
					if(!(Discount_flag&0x01)) v32=Price.UnitPrice[PRICE_RESULT];
					else v32=Price.UnitPrice[PRICE_ORIGIN];
					break;
				case 0:
					v32=status_scale.cur_keyvalue;
					break;
				case PLU_BY_PCS:
					if (status_scale.cur_ride == 3 && Seg7BlinkOn) // unitprice of by-pcs
					{
						seg7UnitPriceBlinkOn = ON;
					}
					if(!(Discount_flag&0x01)) v32=Price.UnitPrice[PRICE_RESULT];
					else v32=Price.UnitPrice[PRICE_ORIGIN];
					break;
				case PLU_BY_COUNT:
					if (status_scale.cur_ride == 2 && Seg7BlinkOn) // unitprice of by-count
					{
						seg7UnitPriceBlinkOn = ON;
					}
					if(!(Discount_flag&0x01)) v32=Price.UnitPrice[PRICE_RESULT];
					else v32=Price.UnitPrice[PRICE_ORIGIN];
					break;
				default:
					break;
			}
			if (seg7UnitPriceBlinkOn) //unitprice
			{
				display_clear(DISPLAY_UNITPRICE);
			}
			else
			{
#ifdef USE_INVENTORY_LABEL
				display_ulongsimple(DISPLAY_UNITPRICE, InventoryUnitWeight);
				sale_display_update(UPDATE_UNIT);
#else
				if(Operation.negative == ON)//added by yyc 20110520 독일요청사항으로 Negative Sale mode 동작시 Unit Price에 "-"를 표시.
				{
					display_ulongsimple(DISPLAY_UNITPRICE,-v32);
				}
				else  display_ulongsimple(DISPLAY_UNITPRICE,v32);
				//				prevValue = Price.UnitPrice[PRICE_RESULT];
#endif
			}
#endif
		}
		status_scale.cur_displayon&=(~chk);
	}
}

void sale_display_price(void)
{
	INT16U chk;
	
#ifdef USE_INVENTORY_LABEL
	INT8U unitWeightDecimal;
#endif

	sale_display_update(UPDATE_PRICE);

	chk=UPDATE_PRICE;
	if (status_scale.cur_displayon&chk) {
		status_scale.cur_displayon &= (~chk);
		if(ADM_GetData.Overload || Operation.OverTotalPrice || Operation.UnderWeight || PercentTareOver)
		{
			char	aaa[10];
			strcpy(aaa,"---------");
			// sale_mode에서 TRUE를 사용하여 뒤에서부터 출력하고 최대 무게 초과 or 최소 무게 미만 or 최대 총액 초과시 총액 칸에 "---" 문구를 표시하는 기능
			VFD7_display_string((HUGEDATA char *)aaa, display_parameter.mode_pos[DISPLAY_PRICE], strlen(aaa), DISPLAY_PRICE, TRUE);
		}
		else
		{
#ifdef USE_INVENTORY_LABEL
			if(InventoryUnitWeight == 0 || Price.NetWeight <= 0)
			{
				InventoryQuantity = 0;
			}
			else
			{
				unitWeightDecimal = get_global_bparam(GLOBAL_AREA_PRICE_DECIMALPOINT);
				InventoryQuantity = (float)Price.NetWeight / ((float)InventoryUnitWeight / power10(unitWeightDecimal)) + 0.5;
			}
			display_ulong(DISPLAY_PRICE, InventoryQuantity, 0);
			sale_display_update(UPDATE_PRICE);
#else
			display_ulongsimple(DISPLAY_PRICE,Price.DisplayedPrice);
			//status_scale.cur_displayon &= (~chk);
#endif
		}
	}
}

void sale_display_clear_lcd(void)
{
	INT16U chk;
	static INT16U DispCycleTime=0;

	if((INT16U)(SysTimer100ms - DispCycleTime) < 5) return;
	DispCycleTime = SysTimer100ms;

	chk = UPDATE_CLEARALL;
	if ((status_scale.cur_displayon&chk)==chk) {
		status_scale.cur_displayon &= (~chk);
		Dsp_ChangeMode(DSP_SALE_MODE);
	}
}

void sale_display_error(void)
{
	INT16U chk;
	INT16U code;
	char str[128];
	INT8U ucTemp=0;
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
	chk=UPDATE_ERROR;
	if (status_scale.cur_displayon&chk) {
#endif
	code=0;
	if(status_scale.ErrKind)
	{
		code=status_scale.ErrKind;
		caption_split_by_code(code,&cap,0);
		ucTemp = get_global_bparam(GLOBAL_SALE_SETUP_FLAG6);
		ucTemp = (ucTemp>>3)&0x01;
		if( (status_scale.ErrAction == 2) && (!ucTemp) ) //PLU Not Found !
		{
#ifdef HEBREW_FONT
			sprintf(str, "%s)#%ld(", cap.form, status_scale.ErrValue);
#else
			sprintf(str, "%s(#%ld)", cap.form, status_scale.ErrValue);
#endif
		}

		else if(status_scale.ErrAction == 3) 	       //se-hyung 20071130
		{
#ifdef HEBREW_FONT
			sprintf(str,"]#%ld[ %s",status_scale.ErrValue, cap.form);
#else
			sprintf(str,"[#%ld] %s",status_scale.ErrValue, cap.form);
#endif
		}
		else
		{
			sprintf(str, "%s", cap.form);
		}
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
		if( ucTemp && (status_scale.ErrAction==2) ) //SG061206
		{
			display_message_page(str);
		}
		else
		{
			DisplayMsg(str);
		}
#else
		DisplayMsg(str);
#endif
#ifdef USE_ADMIN_EMAIL
		if (status_scale.ErrAction==10)
		{
			command_send_realtime_alram();
		}
#endif
	}
	//#		mode_error_disp(code,status_scale.ErrAction);
	status_scale.ErrKind = 0;				   
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
		status_scale.cur_displayon&=(~chk);
	}
#endif
}

extern INT8U useCheckingMaster;
INT8U sale_display_date(INT16S displayFlag)
{
	char  str_date[24]; //,out_format[32];
	static long  l_time=0;
	static INT8U old_mode=100;
	INT32U pnt;
	INT8U  mode,ret,cc;//,save_direct;
	INT16S w;
	INT8U i,cnt;
	TRANSACT_CHAIN *cp;
	INT16U chk;
	char net_service[5] = {' ', 'M', 'S', 'N', 'L'};
	char net_connection[2] = {'X', 'O'};
	POINT disp_p;

#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
	INT8U totalinfo_disp;
	long   l_temp;
	static long prev_l=0;
	static INT8U oldClerk=0;
	INT16S y;

#ifdef USE_PERSIAN_CALENDAR
	RTC_Convert_Persian_Read();
#endif
#ifdef USE_SCALE_POS_TEST_VERSION
	sprintf(str_date, "TEST VERSION");
	PutStringIndirect(DSP_DATE_Y, DSP_DATE_X, str_date, DSP_DATE_FONT_ID, DSP_DATE_MAGY, DSP_DATE_MAGX, MAX_DATE_CHAR);
	ret = 1;
	return ret;
#endif

	totalinfo_disp = get_global_bparam(GLOBAL_SALE_SETUP_FLAG7)& 0x10;

	chk=UPDATE_DATETIME;
	ret=0;

	if(MsgWeight)	return ret;	//Added by JJANG 20080401
	if (totalinfo_disp && get_nvram_lparam(NVRAM_TRANS_HEAD)) return ret;	//Added by JJANG 20080428

	if (status_scale.cur_displayon&chk) {
		status_scale.cur_displayon&=(~chk);
		displayFlag = ON;
	}

	if(PeelHeadMsgFlag)	return ret;
	if(MsgRDisplayTime)	return ret;

	l_temp = rtc_gettime(str_date);
	if(!displayFlag) {
		if (l_temp==l_time && oldClerk == status_scale.clerkid) goto NEXT;
	}
//	DspLoadFont1(DSP_SYSTEM_FONT_ID);
//	pnt = DFLASH_BASE;
//	pnt+= FLASH_GLOBAL_AREA;
	y = 0;
	mode = get_nvram_bparam(NVRAM_DISPLAY_DATETIME_MODE);
	if ((mode==0) && (prev_l==0)) goto NEXT;
	w = display_font_get_width();
	if (mode!=old_mode)
	{
		if (prev_l) {
			memset((INT8U *)str_date,0x20,prev_l);
			str_date[prev_l]=0;
			//PutNSmallString(DSP_MSG_R_CHAR_Y, DSP_MSG_R_CHAR_X, str_date, MAX_MSG_R_CHAR);
			PutStringIndirect(DSP_DATE_Y, DSP_DATE_X, str_date, DSP_DATE_FONT_ID, DSP_DATE_MAGY, DSP_DATE_MAGX, MAX_DATE_CHAR);
		}
		old_mode=mode;
	}
////se-hyung 20071106
////se-hyung 20071106
	str_date[0]=0;
	prev_l     =0;
#else
	disp_p.x = 0;
	disp_p.y = 0;

	chk=UPDATE_DATETIME;
	ret=0;

	if (status_scale.cur_displayon&chk) {
		status_scale.cur_displayon&=(~chk);
		displayFlag = ON;
	}

	if(PeelHeadMsgFlag)	return ret;
	//	if(MsgRDisplayTime)	return ret;
	if(!DateTimeDispTime && !displayFlag) return ret;

	//	DspLoadFont1(DSP_SYSTEM_FONT_ID);
	//	pnt = DFLASH_BASE;
	//	pnt+= FLASH_GLOBAL_AREA;
	mode = get_nvram_bparam(NVRAM_DISPLAY_DATETIME_MODE);


	str_date[0]=0;
#endif
	switch (mode) {
		case DISPLAY_DATE_MIN: rtc_gettime_sub( 1,str_date);
		break;
		case DISPLAY_DATE_SEC: rtc_gettime_sub( 2,str_date);
		break;
		case DISPLAY_DATE_DAY: rtc_getdate_sub(1,0,0xff,str_date);
		break;
		case DISPLAY_DATE_SCALE:
			sprintf(str_date,global_datetime[MSG_DT_SCALE],status_scale.scaleid);
			break;
		case DISPLAY_DATE_DEPT:
			sprintf(str_date,global_datetime[MSG_DT_DEPT ],status_scale.departmentid);
			break;
		case DISPLAY_DATE_PLUNO:
			if(MISC_PLU_WEIGHT <=status_scale.cur_pluid)
			{
				sprintf(str_date,global_message[MSG_00_MISC_VALUE], (status_scale.cur_pluid%MISC_PLU_WEIGHT)+1);
			}
			else if (status_scale.cur_pluid)
				sprintf(str_date,global_datetime[MSG_DT_PLU ], status_scale.cur_pluid);
			else
				strcpy(str_date,global_datetime[MSG_DT_NOPLU]);

			break;
		case DISPLAY_DATE_NETSTATUS:
#ifdef HEBREW_FONT	//Display "N:[N   ]" 표시 순서 틀어짐 현상 개선
			cnt = 0;
			str_date[cnt++] = ']';
			str_date[cnt++] = ' ';
			str_date[cnt++] = ' ';

			if (ethernet_list.remoteip[0])
			{
				if (useCheckingMaster)	//Modified by JJANG 20080704
				{
					str_date[cnt++] = net_connection[network_status.connection]; 	// connection
				}
				else str_date[cnt++] = ' ';
			}
			else
			{
				str_date[cnt++] = ' ';
			}

			str_date[cnt++] = net_service[ethernet_list.status];		// service type

			strcat(str_date,"[:N");
			cnt+=3;
			str_date[cnt++] = 0;
#else
			sprintf(str_date,"N:[");
			cnt = 3;
			str_date[cnt++] = net_service[ethernet_list.status];		// service type
			if (ethernet_list.remoteip[0])
			{
				if (useCheckingMaster)	//Modified by JJANG 20080704
				{
					str_date[cnt++] = net_connection[network_status.connection]; 	// connection
				}
				else str_date[cnt++] = ' ';
			}
			else
			{
				str_date[cnt++] = ' ';
			}
			str_date[cnt++] = ' ';
			str_date[cnt++] = ' ';
			//cnt += 4;
			str_date[cnt++] = ']';
			str_date[cnt++] = 0;
#endif
			break;
			//////// => No Clerk Mode
		case DISPLAY_DATE_CLERK:
			cc=get_global_bparam(GLOBAL_DISPLAY_INFO_FLAG);
			if(status_scale.clerkid) {
				if (cc&0x08) {	// nick name
					CLERK_STRUCT clerk;
					pnt = status_scale.clerkid;
					pnt *= CLERK_STRUCT_SIZE;
					pnt += FLASH_CLERK_AREA;
					//get_base_sparam(pnt, (INT8U*) &clerk, CLERK_STRUCT_SIZE);
					//memcpy(str_date,clerk.nickname,8);
					get_base_sparam(pnt+26, (INT8U*)str_date, 8);
					str_date[8]=0;
				} else {	// clerk number
					sprintf(str_date,global_datetime[MSG_DT_CLERK],status_scale.clerkid);
				}
			} else {
				strcpy(str_date,global_datetime[MSG_DT_NOCLK]);
			}
			break;
			// Added by CJK 20060426
		case DISPLAY_DATE_OPENCLERK:
#ifdef HEBREW_FONT
			sprintf(str_date,"#C]");
#else
			sprintf(str_date,"C#[");
#endif
			cnt = 3;
			for(i = 0; i < 4; i++) {
				cp = (TRANSACT_CHAIN *)get_nvram_lparam(NVRAM_TRANS_HEAD+(i+1)*4);
				if(cp) {
					str_date[cnt++] = 'A' + i;
				} else {
					str_date[cnt++] = ' ';
				}
			}
			//cnt += 4;
#ifdef HEBREW_FONT
			str_date[cnt++] = '[';
#else
			str_date[cnt++] = ']';
#endif
			str_date[cnt++] = 0;
			break;
			///////////////////////////
			//////// => Clerk Mode
		case DISPLAY_DATE_ITEM:
			if (status_scale.cur_pluid) {
				sprintf(str_date,"I#%d",status_scale.Plu.ItemCode);
			}
			break;
		case DISPLAY_DATE_OVERRIDE:
			sprintf(str_date,"  - R%d -", status_scale.cur_ride);
			break;
		default:str_date[0]=0;
	}
	if (InputQuickPluState)
	{
		if (status_scale.cur_quickplu && ((INT16U)(SysTimer100ms - KeyDrv.LastKeyTime) <  GlbOper.keyClearTime))
		{
#ifdef HEBREW_FONT
			sprintf(str_date, "]%6ld[", status_scale.cur_quickplu);
#else
			sprintf(str_date, "[%6ld]", status_scale.cur_quickplu);
#endif
		}
	}
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
	prev_l = strlen((char *)str_date);
	if (prev_l) {
		for (w=prev_l; w<MAX_DATE_CHAR; w++) str_date[w]=0x20;
		str_date[MAX_DATE_CHAR]=0;
		if(mPackDate.deltadays && mode==DISPLAY_DATE_DAY) DspStruct.Reverse = 1; //SG060314
		//PutNSmallString(DSP_MSG_R_CHAR_Y, DSP_MSG_R_CHAR_X, str_date, MAX_MSG_R_CHAR);
		PutStringIndirect(DSP_DATE_Y, DSP_DATE_X, str_date, DSP_DATE_FONT_ID, DSP_DATE_MAGY, DSP_DATE_MAGX, MAX_DATE_CHAR);
		if(mPackDate.deltadays && mode==DISPLAY_DATE_DAY) DspStruct.Reverse = 0;
	}
#else
	str_fill(str_date, DSP_MAX_CHAR);

	if(mPackDate.deltadays && mode==DISPLAY_DATE_DAY) DspStruct.Reverse = 1; //SG060314
	Dsp_Write_String(disp_p, str_date);
	if(mPackDate.deltadays && mode==DISPLAY_DATE_DAY) DspStruct.Reverse = 0;
#endif
	ret=1;
NEXT:
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
	oldClerk = status_scale.clerkid;
	l_time=l_temp;
#endif
	return ret;
}

//#ifdef USE_ARAB_FONT
//extern INT8U	Arab_combine_cnt;
//#endif
extern INT8U DividedPluName[80];
void sale_display_pluname(void)
{
	char string_buf[66];
	char temp_buf[66];
	POINT p;
	INT16U chk;
	INT16S w,sl,x;
	INT8U align;
	char old_id;
	INT8U	ucTemp;
	INT8U	useSmallFont;
	INT16U  win_size, bsl = 0;
	INT16U  combinedCnt;
	INT16S dsl;

	ucTemp = get_global_bparam(GLOBAL_SALE_SETUP_FLAG10);
	useSmallFont = (ucTemp>>7) & 0x01;
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
	chk=UPDATE_CENTER;

	if (status_scale.cur_displayon&chk) {
#endif
	old_id = DspStruct.Id1;
	if (useSmallFont) DspLoadFont1(DSP_MEDIUM_FONT_ID);
	else DspLoadFont1(DSP_PLU_FONT_ID);

	Dsp_FontSize(DSP_PLU_NAME_MAGX, DSP_PLU_NAME_MAGY);
	win_size = Dsp_GetChars();
	w = win_size;

#ifdef USE_CHN_FONT
	if (w > 26) w = 26;	// font id 3 사용
#else
	if (useSmallFont) {
		if (w > 26) w = 26;//영문 26, 한글 13
	} else {
		if (w > MAX_PLU_NAME_CHAR) w = MAX_PLU_NAME_CHAR;
	}
#endif
	if (status_scale.cur_pluid) {
		if(MISC_PLU_WEIGHT <= status_scale.cur_pluid)
		{
			switch(status_scale.cur_pluid)
			{
				case MISC_PLU_WEIGHT:
					// bmlim 20050127
					strcpy(string_buf,global_message[MSG_MISC_WEIGHT]);
					break;
				case MISC_PLU_PCS:
					// bmlim 20050127
					strcpy(string_buf,global_message[MSG_MISC_PCS]);
					break;
				default:
					strcpy(string_buf,global_message[MSG_MISC_COUNT]);
					break;
			}

		} else {
			INT16S blankFlag;
#ifdef USE_DIVIDING_FUNCTION
			if(status_scale.divisor)
			{
				blankFlag = IsBlankString((char*)DividedPluName,40);
			}
			else
			{
				blankFlag = IsBlankString((char *)status_scale.Plu.ItemName,40);
			}
#else
			blankFlag = IsBlankString((char *)status_scale.Plu.ItemName,40);
#endif
			if(blankFlag)
			{
				strcpy(string_buf,global_message[MSG_NO_PLUNAME]);
			}
			else
#if defined(USE_ARAB_FONT) || defined(HEBREW_FONT)
#ifndef _USE_LCD37D_40DOT_
			{
				memcpy(string_buf,status_scale.Plu.ItemName, 40);
			}
#else
#ifdef USE_DIVIDING_FUNCTION
			{
				if(status_scale.divisor)
				{
					memcpy(string_buf,DividedPluName, 40);
				}
				else
				{
					memcpy(string_buf,status_scale.Plu.ItemName, 40);
				}
			}
#else
			{
				memcpy(string_buf,status_scale.Plu.ItemName, 40);
			}
#endif	//USE_DIVIDING_FUNCTION
#endif
#else
#ifdef USE_DIVIDING_FUNCTION
			{
				if(status_scale.divisor)
				{
					memcpy(string_buf,DividedPluName, 40);
				}
				else
				{
					memcpy(string_buf,status_scale.Plu.ItemName, 40);
				}
			}
#else
			{
				memcpy(string_buf,status_scale.Plu.ItemName, 40);
			}
#endif	//USE_DIVIDING_FUNCTION
#endif 	//USE_ARAB_FONT
		}
		string_buf[40]=0;
	}
	else
		string_buf[0]=0;
	if(Display_flag) strcpy(string_buf,global_message[MSG_NO_MASTER]);	//Added by JJANG 20080312

	combinedCnt = 0;
#ifdef USE_VIETNAM_FONT
	Cvt_VietnamPreview((INT8U *)string_buf, strlen(string_buf), 0, 0);//gm 081024
	combinedCnt = VietnamCombinedChar_cnt;
	//w = Dsp_GetChars() + VietnamCombinedChar_cnt;//gm 081024
	//if (w > 33*2) w = 33*2;//gm 081103, when veitnam 6*8 font size
#endif
	//if(DspStruct.StrType != ONE_BYTE_FONT_STR && IsHanCharMid(string_buf, w) == CHAR_HAN_MID) w--;
	sl=strlen((char *)string_buf);	// data length
	if (sl > win_size)
		bsl = sl - win_size;
#ifdef USE_ARAB_FONT
#ifndef _USE_LCD37D_40DOT_
	memset(temp_buf, 0x20, win_size+combinedCnt+bsl);
#else
	memset(temp_buf, 0x20, win_size+combinedCnt+2);
#endif
#else
	memset(temp_buf, 0x20, win_size+combinedCnt+2);
#endif
	//temp_buf[win_size+1] = 0;
	sl=strlen((char *)string_buf);	// data length
	dsl = sl - combinedCnt;	// display string length
	if (dsl > w) dsl = w;
	align=get_global_bparam(GLOBAL_DISPLAY_ALIGN);
	switch (align) {
		default:
		case 0:	// left
		   	x = 0;
		   	break;
		case 1:	// center
		   	//sl=strlen((char *)string_buf);
			x =(w-dsl)/2;
			break;
		case 2:	// right
			x = w-dsl;
			break;
	}
#ifdef USE_ARAB_FONT
	memcpy(temp_buf, string_buf, sl);	// data length만큼 copy
#else
	memcpy(temp_buf+x, string_buf, sl);	// data length만큼 copy
#endif
	//temp_buf[win_size] = ' ';
#ifdef USE_ARAB_FONT
#ifndef _USE_LCD37D_40DOT_
	temp_buf[win_size+combinedCnt+bsl] = 0;
#else
	temp_buf[win_size+combinedCnt+2] = 0;
#endif
#else
#ifdef _USE_LCD_7_26d_16x96_
	temp_buf[win_size+combinedCnt] = 0;
#else
	temp_buf[win_size+combinedCnt+2] = 0;
#endif
#endif
	p = point(DSP_PLU_NAME_Y, DSP_PLU_NAME_X);
#ifdef USE_ARAB_FONT
#ifndef _USE_LCD37D_40DOT_
	Convert_ArabPreview((INT8U *)temp_buf, strlen(temp_buf), 0, 0, 0);
	arab_codetemp[strlen(temp_buf)] = 0;
	p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - p.x;
	Dsp_Write_1stringLong(p, arab_codetemp);
//	sale_display_date(ON);
#else
	Dsp_Write_String(p, temp_buf);
#endif
#elif defined(HEBREW_FONT)
	Convert_HebrewPreview((INT8U *)temp_buf, strlen(temp_buf), 0, 0);
	hebrew_codetemp[strlen(temp_buf)] = 0;
	p.x = 196 - p.x;	//PLU Name 잘리는 현상 수정 (Display Font: 12x20)  <= 208 - 12
	Dsp_Write_1stringLong(p, hebrew_codetemp);
//	sale_display_date(ON);
#else
	Dsp_Write_String(p, temp_buf);
#endif
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
		status_scale.cur_displayon&=(~chk);
		DspLoadFont1(old_id);				   
	}
#else
	DspLoadFont1(old_id);
#endif
}

extern char*	GetCurrency(char* pCurr,INT8U prt_or_display);

#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
void sale_display_add_msg(void)
{
	char	currency[5];
	char	priceBuf[15];
	char	weightBuf[15];
	INT8U	use_unit;
	INT8U	v8_1;
	INT8U	dec_ch;

	char w_sign1[5];
	INT8U	totalinfo_disp = 0;

	STRUCT_TOTAL_PRICE totalData;

	dec_ch  = get_global_bparam(GLOBAL_DECIMAL_SIGN);
	totalinfo_disp = get_global_bparam(GLOBAL_SALE_SETUP_FLAG7)& 0x10;

	ClerkTotalPrice(0, 0, &totalData);
	FloatToString(TO_STR,priceBuf,8,DISPLAY_PRICE,&totalData.TotalPrice,0,1);
	GetCurrency(currency,PRICE_DISPLAY);

	if(totalinfo_disp)	//Added by JJANG 20080428
	{
		use_unit = ADM_Status.UseUnit;
		if(use_unit == WEIGHT_UNIT_KG)
		{
			if(get_base_bparam(FLASH_ADC_WUNIT) == WEIGHT_UNIT_G) use_unit = WEIGHT_UNIT_G;
		}
		v8_1=get_decimal_pt(3, use_unit);	// UseUnit 사용
		GetWeightSymbol(use_unit, w_sign1);
		format_value((HUGEDATA INT8U *)weightBuf, totalData.TotalWeight, v8_1, dec_ch);
		strcat(weightBuf, w_sign1);
#ifdef USE_ARAB_CONVERT
        sprintf(gmsg,"%d %s  %s  %s",totalData.TotalCount+totalData.VoidCount,global_message[MSG_14_C052],str_trimleft(priceBuf),weightBuf );
#else
		sprintf(gmsg,"%s %d  %s%s  %s",global_message[MSG_14_C052],totalData.TotalCount+totalData.VoidCount,currency,str_trimleft(priceBuf),weightBuf );
#endif
		PutStringIndirect(DSP_MSG_Y, DSP_MSG_X, gmsg, DSP_MSG_FONT_ID, DSP_MSG_MAGY, DSP_MSG_MAGX, MAX_MSG_CHAR_LONG);

	}
	else
	{
#ifdef USE_ARAB_CONVERT
        sprintf(gmsg,"%d %s %s",totalData.TotalCount+totalData.VoidCount,global_message[MSG_14_C052],str_trimleft(priceBuf));
#else	
		sprintf(gmsg,"%s %d %s %s",global_message[MSG_14_C052],totalData.TotalCount+totalData.VoidCount,currency,str_trimleft(priceBuf));
#endif
		//PutNSmallString(DSP_MSG_CHAR_Y, DSP_MSG_CHAR_X, gmsg, MAX_MSG_CHAR);
		PutStringIndirect(DSP_MSG_Y, DSP_MSG_X, gmsg, DSP_MSG_FONT_ID, DSP_MSG_MAGY, DSP_MSG_MAGX, MAX_MSG_CHAR);
	}
}
#else
void sale_display_add_msg(INT8U transaction, INT32S price)
{
	STRUCT_TOTAL_PRICE totalData;
	CAPTION_STRUCT cap;

	if (Operation.operationClerk == OPER_FLOATING && ethernet_list.status == 2)
	{
		totalData.TotalCount = ClerkSaleAmount.totalCount;
		totalData.VoidCount  = ClerkSaleAmount.voidCount;
		totalData.TotalPrice = ClerkSaleAmount.totalPrice;
		if (transaction)
		{
			totalData.TotalCount++;
			totalData.TotalPrice += price;
		}
	}
	else
	{
		ClerkTotalPrice(0, status_scale.clerkid, &totalData);
	}

	if (status_scale.clerkid)
	{
		if (display_parameter.mode_len[DISPLAY_UNITPRICE] == 7)
			sprintf(cap.form,"%.3s %3d", global_message[MSG_TRANSINFO], status_scale.clerkid);
		else
			sprintf(cap.form,"%.3s %2d", global_message[MSG_TRANSINFO], status_scale.clerkid);
		display_string(DISPLAY_UNITPRICE, (INT8U*)cap.form);
	}
	else
	{

		caption_split_by_code(0xc013,&cap,0);
		display_string(DISPLAY_UNITPRICE, (INT8U*)cap.form);
	}

	sprintf(gmsg, "n%4d", totalData.TotalCount+totalData.VoidCount);
	display_string(DISPLAY_TARE, (INT8U*)gmsg);

	if (Operation.operationClerk == OPER_FLOATING && ethernet_list.status == 2)
	{
		display_clear(DISPLAY_WEIGHT);
	}
	else
	{
		display_ulongsimple(DISPLAY_WEIGHT, totalData.TotalWeight);
	}

	display_ulongsimple(DISPLAY_PRICE, totalData.TotalPrice);
}
#endif

void sale_display_multiply_msg(void)
{
	char string_buf[10];

	status_scale.print_multiple %= 10000;  //9999 장
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
	if (display_parameter.mode_len[DISPLAY_WEIGHT] == 6)
		sprintf((char *)string_buf,"X %04d %s",status_scale.print_multiple, global_message[MSG_INPUTXKEY]);
	else
		sprintf((char *)string_buf,"X %03d %s",status_scale.print_multiple, global_message[MSG_INPUTXKEY]);
	PutStringIndirect(DSP_MSG_Y, DSP_MSG_X, string_buf, DSP_MSG_FONT_ID, DSP_MSG_MAGY, DSP_MSG_MAGX, MAX_MSG_CHAR);
#else
	if (display_parameter.mode_len[DISPLAY_WEIGHT] == 6)
		sprintf((char *)string_buf,"n%5d",status_scale.print_multiple);
	else
		sprintf((char *)string_buf,"n%4d",status_scale.print_multiple);

	display_string(DISPLAY_WEIGHT, (INT8U*)string_buf);
#endif
}
#ifndef USE_SINGLE_LINE_GRAPHIC
#ifdef USE_USA_FIXEDWEIGHT_UNIT_OZ
void sale_display_netweight_msg(void)
{
	INT8U	display[50],currency[5];
	INT32S	lb_val,oz_val;
	INT8U	decimal;
	INT8U	rate = 16;
	get_global_sparam(GLOBAL_WEIGHT_LBSYMBOL_2, (INT8U *)currency, 4);
	currency[4] = 0;
	display[0] = 0;

	//	if(status_scale.Plu.fixed_weight==Price.NetWeight) //Fixed Weight
	if(ADM_GetData.Stable == 1 && ADM_GetData.RecvData == 0) // Zero(Gross, Net zero)
	{
		sprintf((char*)display,"%ld %s", Price.NetWeight, currency);
	}
	else // Actual Weight on Platter
	{
		decimal=get_decimal_pt(6, 0);
		oz_val = LbtoOz(Price.NetWeight, rate, decimal);
		sprintf((char*)display,"%ld %s", oz_val, currency);
	}
	PutStringIndirect(DSP_MSG_Y, DSP_MSG_X, (char*)display, DSP_MSG_FONT_ID, DSP_MSG_MAGY, DSP_MSG_MAGX, MAX_MSG_CHAR);
	sale_display_update(UPDATE_TITLE);
}
#endif
#endif //#ifndef CPU_PIC32MZ

#ifndef USE_SINGLE_LINE_GRAPHIC
void sale_display_discount_msg(void)
{
	char	display[50], priceStr[20];
	char	priceBuf[15];
	char	dc_price[15]; //JJANG 20071114

	long	totalPrice;
	static long oldPrice = 0;
	INT8U	ucTemp;

	INT8U	Discount_flag;	//Modified by JJANG 20071114
	Discount_flag = get_global_bparam(GLOBAL_SALE_SETUP_FLAG7);

	//SG061213 FSP 일때 Discount Message 표시 안 함. "FSP"로 표시
	ucTemp = get_global_bparam(GLOBAL_SALE_SETUP_FLAG6);
	ucTemp = (ucTemp>>4)&0x01; //use FSP

	if(status_scale.cur_pluid && PluType()==PLU_BY_WEIGHT && status_scale.Plu.fixed_weight && !Operation.fixedPrice)
	{
		PutStringIndirect(DSP_MSG_Y, DSP_MSG_X, global_message[MSG_FIXED_WEIGHT], DSP_MSG_FONT_ID, DSP_MSG_MAGY, DSP_MSG_MAGX, MAX_MSG_CHAR);
		return;
	}

	if(!GlbOper.viewDiscount)
	{
		//DEL_DC		if(DiscSale.discIndexData.discPriceType == DISC_PRICE_FIXED)
		//DEL_DC		{
		//DEL_DC			if(Price.FixedDiscountPrice<=Price.TotalPrice[PRICE_ORIGIN])
		//DEL_DC				Operation.fixedDiscFlag = ON;
		//DEL_DC			else
		//DEL_DC				Operation.fixedDiscFlag = OFF;
		//DEL_DC		}
		return;
	}

	//	if(oldPrice != Price.TotalPrice[PRICE_RESULT])
	{
		display[0] = 0;
		if (status_scale.cur_pluid && DiscSale.saleDiscountType)
		{
			if (DiscSale.discIndexData.discTargetType == DISC_TARGET_FREE)
			{
				strcpy(display,global_message[MSG_04_FREEPLU]);
			}
			else
			{
				totalPrice = Price.TotalPrice[PRICE_SAVED];
				FloatToString(TO_STR,priceBuf,9,DISPLAY_PRICE,&totalPrice,0,1);
				totalPrice = Price.TotalPrice[PRICE_ORIGIN];
				FloatToString(TO_STR,priceStr,9,DISPLAY_PRICE,&totalPrice,0,1);
				totalPrice = Price.TotalPrice[PRICE_ORIGIN] - Price.TotalPrice[PRICE_SAVED];
				FloatToString(TO_STR,dc_price,9,DISPLAY_PRICE,&totalPrice,0,1);

				if (DiscSale.discIndexData.discPriceType == DISC_PRICE_FIXED && !Operation.fixedDiscFlag)
				{
					FloatToString(TO_STR,priceBuf,9,DISPLAY_PRICE,&Price.FixedDiscountPrice,0,1);
					sprintf(display,"%s %s",global_message[MSG_03_DC_FIXED],str_trimleft(priceBuf));
				}
				else
				{
					if (DiscSale.saleDiscFSP && ucTemp) //SG061214
					{
						sprintf(display,"FSP %s-%s",str_trimleft(priceStr),str_trimleft(priceBuf));
					}
					else
					{
						//						sprintf(display,"%s %s-%s",global_message[MSG_02_DC],str_trimleft(priceStr),str_trimleft(priceBuf));
						//Modified by JJANG 20071114
						if(!(Discount_flag&0x01))
						{
							if(DiscSale.discIndexData.discTargetType == DISC_TARGET_N_PLUS_1 && DiscSale.discIndexData.discPriceType == DISC_N_PLUS_1  && PluType()==PLU_BY_WEIGHT )
							{
								sprintf(display,"%03dPCS:%s-%s",status_scale.cur_qty,str_trimleft(priceStr),str_trimleft(priceBuf));
							}
							else
							{
#ifdef USE_ARAB_FONT
								sprintf(display,"%s-%s",str_trimleft(priceStr),str_trimleft(priceBuf));
#else
								sprintf(display,"%s %s-%s",global_message[MSG_02_DC],str_trimleft(priceStr),str_trimleft(priceBuf));
#endif
							}
						}
						else
						{			
#ifdef USE_ARAB_FONT
							sprintf(display,"%s-%s=%s",str_trimleft(priceStr),str_trimleft(priceBuf),str_trimleft(dc_price));
#else
							sprintf(display,"%s %s-%s=%s",global_message[MSG_02_DC],
						            str_trimleft(priceStr),str_trimleft(priceBuf),str_trimleft(dc_price));
#endif
						}
					}
				}
			}
			//			oldPrice = Price.TotalPrice[PRICE_RESULT];
		}
		if (!Operation.override)
		{
			PutStringIndirect(DSP_MSG_Y, DSP_MSG_X, display, DSP_MSG_FONT_ID, DSP_MSG_MAGY, DSP_MSG_MAGX, MAX_MSG_CHAR);

		}
		if (Operation.override && MISC_PLU_WEIGHT<=status_scale.cur_pluid)
		{
			PutStringIndirect(DSP_MSG_Y, DSP_MSG_X, display, DSP_MSG_FONT_ID, DSP_MSG_MAGY, DSP_MSG_MAGX, MAX_MSG_CHAR);
		}
	}
}
#endif //#ifndef USE_SINGLE_LINE_GRAPHIC

#ifdef USE_PERCENT_WEIGHT
void sale_display_without_ice_msg(void)
{
	char	display[50], priceStr[20];
	char	priceWithice[15];
	long	totalPrice;
	INT8U 	precentweight_mode;
	
	precentweight_mode = get_global_bparam(GLOBAL_SALE_SETUP_FLAG19);
	precentweight_mode = precentweight_mode&0x01;

	if(precentweight_mode)
	{
		FloatToString(TO_STR,priceWithice,9,DISPLAY_WEIGHT,&Price.NetWeight,0,1);
		totalPrice = Price.NetWeight - Price.PercentTareWeight;
		FloatToString(TO_STR,priceStr,9,DISPLAY_WEIGHT,&totalPrice,0,1);

		sprintf(display,"%s-%s",str_trimleft(priceWithice),str_trimleft(priceStr));
		PutStringIndirect(DSP_MSG_Y, DSP_MSG_X, display, DSP_MSG_FONT_ID, DSP_MSG_MAGY, DSP_MSG_MAGX, MAX_MSG_CHAR);
	}
	else
	{
		totalPrice = CalcWeightBasedPrice(Operation.MaxTotalPrice,Price.Weight[PRICE_ORIGIN],Price.UnitPrice[PRICE_ORIGIN]);
		FloatToString(TO_STR,priceWithice,9,DISPLAY_PRICE,&totalPrice,0,1);
		totalPrice = totalPrice - Price.TotalPrice[PRICE_ORIGIN];
		FloatToString(TO_STR,priceStr,9,DISPLAY_PRICE,&totalPrice,0,1);

		sprintf(display,"%s-%s",str_trimleft(priceWithice),str_trimleft(priceStr));
		PutStringIndirect(DSP_MSG_Y, DSP_MSG_X, display, DSP_MSG_FONT_ID, DSP_MSG_MAGY, DSP_MSG_MAGX, MAX_MSG_CHAR);
	}
}
#endif

#ifdef USE_CTS_FUNCTION
void sale_display_cts_msg(void)
{
	static short ctsYear = 0;
	static short ctsMonth = 0;
	static short ctsDay = 0;
	INT8U thisYear;
	INT8U thisMonth;
	INT8U thisDay;
	short daysOver;
	INT8U strbuf[20];

	if (ctsYear == 0)
	{
		ctsYear = CtsStruct.CtsCalYear;
		ctsMonth = CtsStruct.CtsCalMonth;
		ctsDay = CtsStruct.CtsCalDate;

		date_getnextday(&ctsYear, &ctsMonth, &ctsDay, CtsStruct.CtsPeriod);
	}

	thisYear = bcd2hex(RTCStruct.year);
	thisMonth = bcd2hex(RTCStruct.month);
	thisDay = bcd2hex(RTCStruct.date);

	daysOver = date_getdeltadays(thisYear, thisMonth, thisDay, ctsYear, ctsMonth, ctsDay);
    if (daysOver > 0) sprintf(strbuf, "%dday delay", daysOver);
    else sprintf(strbuf, "delay");
	DisplayMsg(strbuf);
}
#endif

void sale_display_msg2(void)
{
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
	char cleardisplay = 0x00;
	INT16U chk;
	INT8U old_page;
	static INT8U msgOn = OFF;

	chk = UPDATE_TITLE2;
	if ((status_scale.cur_displayon & chk) == 0) return;
	status_scale.cur_displayon &= (~chk);

	old_page = DspStruct.Page;

	if(MsgRDisplayTime)
	{
		msgOn = ON;
	}
	else if(PeelHeadMsgFlag == 1)
	{
		BuzOn(4);	//se-hyung 20070803
		PutStringIndirect(DSP_MSG_R_Y, DSP_MSG_R_X, global_message[MSG_OPEN_HEAD], DSP_MSG_R_FONT_ID, DSP_MSG_R_MAGY, DSP_MSG_R_MAGX, MAX_MSG_R_CHAR);
#ifdef USE_ADMIN_EMAIL
		command_send_realtime_alram();
#endif
		msgOn = ON;
	}
	else if(PeelHeadMsgFlag == 2)
	{
		BuzOn(4);	//se-hyung 20070803
		PutStringIndirect(DSP_MSG_R_Y, DSP_MSG_R_X, global_message[MSG_PEELOFF], DSP_MSG_R_FONT_ID, DSP_MSG_R_MAGY, DSP_MSG_R_MAGX, MAX_MSG_R_CHAR);
#ifdef USE_ADMIN_EMAIL
		command_send_realtime_alram();
#endif
		msgOn = ON;
	}
	else
	{
		if(msgOn)	//Modified by JJANG 20080401
		{
			msgOn=OFF;
			Dsp_SetPage(old_page);
			Dsp_ReDraw(DSP_MSG_R_X, DSP_MSG_R_Y, DSP_MSG_R_X+8*DSP_MSG_R_MAGX*MAX_MSG_R_CHAR-1, DSP_MSG_R_Y+16*DSP_MSG_R_MAGY-1);
			PutStringIndirect(DSP_MSG_R_Y, DSP_MSG_R_X, &cleardisplay, DSP_MSG_R_FONT_ID, DSP_MSG_R_MAGY, DSP_MSG_R_MAGX, MAX_MSG_R_CHAR);
		}
	}
	Dsp_SetPage(old_page);
#else
	INT8U buf[34];
	static char prevFlag = 0;

	if(PeelHeadMsgFlag == 1)
	{
		BuzOn(4);	//se-hyung 20070803
		PutStringIndirect(DSP_MSG_R_Y, DSP_MSG_R_X, global_message[MSG_OPEN_HEAD], DSP_MSG_R_FONT_ID, DSP_MSG_R_MAGY, DSP_MSG_R_MAGX, MAX_MSG_R_CHAR);
#ifdef USE_ADMIN_EMAIL
		command_send_realtime_alram();
#endif
	}
	else if(PeelHeadMsgFlag == 2)
	{
		BuzOn(4);	//se-hyung 20070803
		PutStringIndirect(DSP_MSG_R_Y, DSP_MSG_R_X, global_message[MSG_PEELOFF], DSP_MSG_R_FONT_ID, DSP_MSG_R_MAGY, DSP_MSG_R_MAGX, MAX_MSG_R_CHAR);
#ifdef USE_ADMIN_EMAIL
		command_send_realtime_alram();
#endif
	}
	else if (PeelHeadMsgFlag == 0 && prevFlag)
	{
		memset(buf, 0x20, MAX_MSG_R_CHAR);
		buf[MAX_MSG_R_CHAR] = 0;
		PutStringIndirect(DSP_MSG_R_Y, DSP_MSG_R_X, (char*)buf, DSP_MSG_R_FONT_ID, DSP_MSG_R_MAGY, DSP_MSG_R_MAGX, MAX_MSG_R_CHAR);
	}
	prevFlag = PeelHeadMsgFlag;
#endif
}

#ifdef USE_SINGLE_LINE_GRAPHIC
void sale_display_msg(void)
{
	PutStringIndirect(DSP_MSG_Y, DSP_MSG_X, (char *)MsgDisplayBuf, DSP_MSG_FONT_ID, DSP_MSG_MAGY, DSP_MSG_MAGX, MAX_MSG_CHAR);
}
#else
void sale_display_msg(void)
{
  #if (!defined(USE_KOR_TRACE) &&	!defined(USE_TRACE_STANDALONE))
	char string_buf[30];
  #endif
	INT16U chk;

	chk = UPDATE_TITLE;
	if ((status_scale.cur_displayon & chk) == 0) return;
	status_scale.cur_displayon &= (~chk);

	if(MsgDisplayTime)
	{
		if (!MsgWeight)
		{
			PutStringIndirect(DSP_MSG_Y, DSP_MSG_X, (char *)MsgDisplayBuf, DSP_MSG_FONT_ID, DSP_MSG_MAGY, DSP_MSG_MAGX, MAX_MSG_CHAR);
		}
	}

	else if(Operation.multiply)
	{
		sale_display_multiply_msg();
	}
  #if defined(USE_USA_FIXEDWEIGHT_UNIT_OZ) // Only Count, PCS PLU with Fixed Weight or actual Weight
	else if(GlbOper.byCountWeight ?
		(status_scale.cur_pluid && PluType()!=PLU_BY_WEIGHT && ((status_scale.Plu.fixed_weight)||(status_scale.Weight!=0))) :
		(status_scale.cur_pluid && PluType()!=PLU_BY_WEIGHT && status_scale.Plu.fixed_weight) )
	{
		sale_display_netweight_msg();
	}
  #endif
  #if !defined(USE_SETPRODUCT_SALE) && !defined(USE_HYUNDAI_SET_FUNCTION)
	else if(get_nvram_lparam(NVRAM_TRANS_HEAD))
	{
		sale_display_add_msg();
	}
  #endif
  #if defined(USE_KOR_TRACE)
	else if(KorTrace_Flag.flagReceive == 1)
	{
		sale_display_kortrace(ON);
	}
	else
	{
    #ifdef USE_HANAPOS_PROTOCOL
		sale_display_individual();
    #else
		sale_display_kortrace(OFF);
      #ifdef USE_CTS_FUNCTION
		if (!CTS_CheckCalibrationDate())
		{
			sale_display_cts_msg();
		}
      #endif

    #endif
	}
  #elif defined(USE_TRACE_STANDALONE)
	else
	{
		sale_display_individual();
	}
  #elif defined(USE_CHN_EMART_TRACE)
	else
	{
		sale_display_chn_emart_tracecode();
	}
  #elif defined(USE_CHN_CART_SCALE)
	else
	{
		sale_display_chn_tracecode();
	}
  #else
    #ifdef USE_PERCENT_WEIGHT
	else if(status_scale.cur_ptare && (DiscSale.saleDiscountType == DISCOUNT_NO))
	{
		sale_display_without_ice_msg();
	}
    #endif
	else if(DiscSale.saleDiscountType != DISCOUNT_NO || status_scale.Plu.fixed_weight)
	{
		sale_display_discount_msg();
	}
	else if (Clerk_Subtotal_List == 1) {}	//Clerk subtotal display 지워지는 현상 방어
    #ifdef USE_CTS_FUNCTION
	else if (!CTS_CheckCalibrationDate())
	{
		sale_display_cts_msg();
	}
    #endif
	else
	{
		memset(string_buf, 0x20, 22);
		string_buf[22] = 0;
		PutStringIndirect(DSP_MSG_Y, DSP_MSG_X, string_buf, DSP_MSG_FONT_ID, DSP_MSG_MAGY, DSP_MSG_MAGX, MAX_MSG_CHAR);
	}
  #endif
	return;
}
#endif //#if defined(USE_SINGLE_LINE_GRAPHIC)

#ifndef USE_SINGLE_LINE_GRAPHIC
void sale_display_override(void)
{
	char  str[10];
	INT16U	chk;
	INT8U	backup_reverse;
	//DEL070705	static INT8U display_over_save = 0;
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
	if(MsgWeight) return;	//Added by JJANG 20080401
#endif
	chk = UPDATE_RIDE;
	backup_reverse = DspStruct.Reverse;  //se-hyung 20071126

	if (status_scale.cur_displayon&chk)
	{
		status_scale.cur_displayon&=(~chk);
		if (Operation.override)
		{
			sprintf(str,"R%d",status_scale.cur_ride);
			PutStringIndirect(DSP_MSG_RIDE_Y, DSP_MSG_RIDE_X, str, DSP_MSG_RIDE_FONT_ID, DSP_MSG_RIDE_MAGY, DSP_MSG_RIDE_MAGX, MAX_MSG_RIDE_CHAR);
		}
		else
		{
			//DEL070705			if (display_over_save)
			//DEL070705			{
			//DEL070705				display_over_save = 0;
			sprintf(str,"  ");
			PutStringIndirect(DSP_MSG_RIDE_Y, DSP_MSG_RIDE_X, str, DSP_MSG_RIDE_FONT_ID, DSP_MSG_RIDE_MAGY, DSP_MSG_RIDE_MAGX, MAX_MSG_RIDE_CHAR);

			//DEL070705			}
		}
	}
	DspStruct.Reverse = backup_reverse;
}
#endif //#ifndef USE_SINGLE_LINE_GRAPHIC

#ifndef USE_SINGLE_LINE_GRAPHIC
void sale_display_ClerkSubtotalList (void)
{

	STRUCT_TOTAL_PRICE totalData;
	char priceBuffer1[40];
	char priceBuffer2[40];
	//TRANSACT_CHAIN	tempList[MAX_ADD_TRANSACTION];
	TRANSACT_CHAIN	tempList[50];
	TRANSACT_CHAIN  *cp;
	TRANSACT_CHAIN  tc;
	INT16U h_index, next;
	INT16S	i,index,maxIndex;
	long	gPrice,totalPrice,sz;
	long	voidNumber=0;
	INT16S	pageList, count;
	INT16S	viewList=0;
	char	priceBuf[15];
	PLU_BASE plu;
	INT16U	lastClerk;
	char    pluname[22];
	char	tempStr[50];
	INT8U	ret;
	long	temptrtvolume;	 //se-hyung 20071017
	INT16U chk;
	INT8U old_page;
	INT32U addr;

	chk = UPDATE_SUBTOTAL;
	if (status_scale.cur_displayon&chk)
	{
		status_scale.cur_displayon &= (~chk);

		temptrtvolume = 0;		 //se-hyung 20071017
	  	ret=get_global_bparam(GLOBAL_SALE_SETUP_FLAG4)&0x10;

		if (!ret) return;
		if (status_scale.cur_pluid) return;


		if(Clerk_Subtotal_List == 1)
		{
			pageList = MAX_STOTAL_LIST_PER_PAGE;

			lastClerk = status_scale.clerkid;
			viewList = Pageupdown*MAX_STOTAL_LIST_PER_PAGE;

			if (Operation.operationClerk == OPER_FLOATING && ethernet_list.status == 2)
			{
				totalData.TotalCount = ClerkSaleAmount.totalCount;
				totalData.VoidCount  = ClerkSaleAmount.voidCount;
				totalData.TotalPrice = ClerkSaleAmount.totalPrice;
			}
			else
			{
				ClerkTotalPrice(0, lastClerk, &totalData);
			}

			if (lastClerk && !status_scale.cur_pluid)
			{	  
				FloatToString(TO_STR,priceBuffer1,8,DISPLAY_PRICE,&totalData.TotalPrice,0,1);
				GetPriceString(priceBuffer2,priceBuffer1,ON,PRICE_DISPLAY);
//#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
//				cp = (TRANSACT_CHAIN *)get_nvram_lparam(NVRAM_TRANS_HEAD+lastClerk*4);
//#else
				h_index = (INT16U)get_nvram_lparam(NVRAM_TRANS_HEAD+lastClerk*4);
				if (h_index)
				{
					next = h_index;
					get_nvram_trans_addr(NVRAM_TRANS_DATA, next - 1, &tc);
					cp = &tc;
				}
				else
				{
					cp = 0;
				}
//#endif
				if(!cp)
				{
					//MsgNoPriceClerk(lastClerk);
					sale_display_update_error(0xC03A,3,lastClerk);	 //se-hyung 20071130
					Dsp_ReDraw(DSP_SALE_INFO_AREA_X1, DSP_SALE_INFO_AREA_Y1, DSP_SALE_INFO_AREA_X2, DSP_SALE_INFO_AREA_Y2);
					Clerk_Subtotal_List = 0;
					return;
				}

				old_page = DspStruct.Page;
				Dsp_SetPage(DSP_DEFAULT_PAGE);

				PutXStringOffsetX = DSP_SALE_INFO_AREA_X1;
				PutXStringOffsetY = DSP_SALE_INFO_AREA_Y1;

				//if (DspStruct.DirectDraw) display_lcd_clear();
				//else                      display_lcd_clear_buf();
				//display_lcd_clear();

				caption_split_by_code(0xc022,&cap,0);//"Total"
				sprintf(gmsg,"[%s] %d, %s", cap.form, totalData.TotalCount+totalData.VoidCount,priceBuffer2);
				PutNSmallString(0,0,gmsg,25);

				totalPrice = Price.TotalPrice[PRICE_RESULT];
				index = ClerkSearchVoidTransaction(status_scale.departmentid,status_scale.cur_pluid, lastClerk, ON, &totalPrice);
				gPrice = totalPrice;

				maxIndex=0;
	  			for(;cp;)
				{
					//tempList[maxIndex] = *cp;
					tempList[maxIndex] = tc;
					//cp=cp->next;
					maxIndex++;
					if (!cp->next) break;
					next = cp->next;
					get_nvram_trans_addr(NVRAM_TRANS_DATA, next - 1, &tc);
					cp = &tc;
				}

				count=0;
				for(i=viewList;i<maxIndex ;i++)
				{
					if(i<maxIndex)
					{
						if(tempList[i].delete)		strcpy(tempStr,global_message[MSG_29_DSP_VOID]);
						else				strset(tempStr,0x20,strlen(global_message[MSG_29_DSP_VOID]));
						temptrtvolume = tempList[i].content.trtVolume;					  	//se-hyung 20071017
						if(tempList[i].content.trtStatus& FLAG_TRANS_NEGATIVE)			    //se-hyung 20071016
						{
							temptrtvolume = -tempList[i].content.trtVolume;
						}
						FloatToString(TO_STR,priceBuf,8,DISPLAY_PRICE,&temptrtvolume,0,1);  //se-hyung 20071017
						sprintf(gmsg,"%3d,%s %s ",i+1,tempStr,priceBuf);	// Modified by CJK 20050728
						plu_table_search(tempList[i].content.trtDept,tempList[i].content.trtPLU,&plu.address,0);
						//Flash_flush_nvbuffer();
						if(Startup.menu_type)
						{
							if(tempList[i].content.trtPLU<MISC_PLU_WEIGHT)
							{
								addr = plu_get_field_point((INT16S)(plu.address-1),PTYPE_PLU_NAME1,(INT16S *)&sz); // Name
								Flash_sread(addr, plu.ItemName, sz);
								//memcpy(pluname,plu.ItemName,20);
								if(!sz || sz==0xff)
									strcpy(pluname,global_message[MSG_NO_PLUNAME]);
								else
									Flash_sread(addr,(HUGEDATA INT8U*)pluname,20);
							}
							else
							{
								MiscNameCopy(pluname,tempList[i].content.trtPLU);
							}
							pluname[20]=0;
						}
						else
						{
							sprintf(pluname,"   %s%07ld",global_message[MSG_30_PLU],tempList[i].content.trtPLU);
						}
						strcat(gmsg,pluname);

					}
					else
						strset(gmsg,0x20,35);

					PutNSmallString((i%pageList)+1,0,gmsg,35);
					count++;
					if(count == pageList) break;
				}
				for(;count<pageList ;count++)
				{
					strset(gmsg,0x20,35);
					PutNSmallString(count+1,0,gmsg,35);
				}

				if(viewList+pageList > maxIndex+2)
				{
					Pageupdown = 0;
				}
				PutXStringOffsetX = 0;
				PutXStringOffsetY = 0;
				Dsp_SetPage(old_page);

	  		}
			Delay_100ms(10);
		}
		else
		{
			Dsp_ReDraw(DSP_SALE_INFO_AREA_X1, DSP_SALE_INFO_AREA_Y1, DSP_SALE_INFO_AREA_X2, DSP_SALE_INFO_AREA_Y2);
			//			if (DspStruct.DirectDraw) display_lcd_clear();
			//			else                      display_lcd_clear_buf();
		}
	}
}
#endif //#ifndef USE_SINGLE_LINE_GRAPHIC
extern INT8U scroll_effect;
#if defined (CL5200_PROJECT) || defined (CL5500_PROJECT)
extern INT8U DividedPluName[80];
#endif
void sale_display_center(void)
{
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
	char string_buf[66];
	char temp_buf[66];
	POINT p;
	INT16U chk;
	INT16S w,sl,x;
	INT8U align;
	char old_id;
	INT8U	ucTemp;
	INT8U	useSmallFont;
	INT16U  win_size;
	INT16U  combinedCnt;
	INT16S dsl;

	chk=UPDATE_CENTER;

	if (status_scale.cur_displayon&chk) {
      	ucTemp = get_global_bparam(GLOBAL_SALE_SETUP_FLAG10);
		useSmallFont = (ucTemp>>7) & 0x01;

		old_id = DspStruct.Id1;
		if (useSmallFont) DspLoadFont1(DSP_MEDIUM_FONT_ID);
		else DspLoadFont1(DSP_PLU_FONT_ID);

		Dsp_FontSize(DSP_PLU_NAME_MAGX, DSP_PLU_NAME_MAGY);
		win_size = Dsp_GetChars();
		w = win_size;

#ifdef USE_CHN_FONT
		if (w > 26) w = 26;	// font id 3 사용
#else
		if (useSmallFont) {
			if (w > 26) w = 26;//영문 26, 한글 13
		} else {
			if (w > MAX_PLU_NAME_CHAR) w = MAX_PLU_NAME_CHAR;
		}
#endif
		if (status_scale.cur_pluid) {
			if(MISC_PLU_WEIGHT <= status_scale.cur_pluid)
			{
				switch(status_scale.cur_pluid)
				{
					case MISC_PLU_WEIGHT:
// bmlim 20050127
						strcpy(string_buf,global_message[MSG_MISC_WEIGHT]);
						break;
					case MISC_PLU_PCS:
// bmlim 20050127
						strcpy(string_buf,global_message[MSG_MISC_PCS]);
						break;
					default:
						strcpy(string_buf,global_message[MSG_MISC_COUNT]);
						break;
				}

			} else {
				INT16S blankFlag;
#ifdef USE_DIVIDING_FUNCTION
				if(status_scale.divisor)
				{
					blankFlag = IsBlankString(DividedPluName,40);
				}
				else
				{
					blankFlag = IsBlankString((char *)status_scale.Plu.ItemName,40);
				}
#else
				blankFlag = IsBlankString((char *)status_scale.Plu.ItemName,40);
#endif
				if(blankFlag)
				{
					strcpy(string_buf,global_message[MSG_NO_PLUNAME]);
				}
				else
#if defined(USE_ARAB_FONT) || defined(HEBREW_FONT)
				{
					memcpy(string_buf,status_scale.Plu.ItemName, 40);
				}
#else
#ifdef USE_DIVIDING_FUNCTION
				{
					if(status_scale.divisor)
					{
						memcpy(string_buf,DividedPluName, 40);
					}
					else
					{
						memcpy(string_buf,status_scale.Plu.ItemName, 40);
					}
				}
#else
				{
					memcpy(string_buf,status_scale.Plu.ItemName, 40);
				}
#endif	//USE_DIVIDING_FUNCTION
#endif 	//USE_ARAB_FONT
			}
			string_buf[40]=0;
		}
		else
			string_buf[0]=0;
		if(Display_flag) strcpy(string_buf,global_message[MSG_NO_MASTER]);	//Added by JJANG 20080312

		combinedCnt = 0;
#ifdef USE_VIETNAM_FONT
		Cvt_VietnamPreview((INT8U *)string_buf, strlen(string_buf), 0, 0);//gm 081024
		combinedCnt = VietnamCombinedChar_cnt;
#endif

#ifdef USE_THAI_FONT
		Cvt_ThaiPreview((INT8U *)string_buf, strlen(string_buf), 0, 0);
		combinedCnt = ThaiCombinedChar_cnt;
#endif

#if defined(USE_ARAB_FONT)
		memset(temp_buf, 0x20, win_size+combinedCnt);
#else
		memset(temp_buf, 0x20, win_size+combinedCnt+2);
#endif
		//temp_buf[win_size+1] = 0;
		sl=strlen((char *)string_buf);	// data length
		dsl = sl - combinedCnt;	// display string length
		if (dsl > w) dsl = w;
		align=get_global_bparam(GLOBAL_DISPLAY_ALIGN);
		switch (align) {
		   default:
		   case 0:	// left
		   	x = 0;
		   	break;
		   case 1:	// center
		   	//sl=strlen((char *)string_buf);
			x =(w-dsl)/2;
			break;
		   case 2:	// right
			x = w-dsl;
			break;
		}
		memcpy(temp_buf+x, string_buf, sl);	// data length만큼 copy
#if defined(USE_ARAB_FONT)
		temp_buf[win_size+combinedCnt] = 0;
#else
		temp_buf[win_size+combinedCnt+2] = 0;
#endif
		p = point(DSP_PLU_NAME_Y, DSP_PLU_NAME_X);
#ifdef USE_ARAB_FONT	//기존 아랍ROM 적용되어 있음
 #ifdef USE_ENG_BASED_ARAB_SUPPORT
		Convert_ArabPreview((INT8U *)temp_buf, strlen(temp_buf), 0, 0, 1); 
		eng_codetemp[strlen(temp_buf)] = 0;
		Dsp_Write_1stringLong(p, eng_codetemp);
 #else
		Convert_ArabPreview((INT8U *)temp_buf, strlen(temp_buf), 0, 0, 0);
		arab_codetemp[strlen(temp_buf)] = 0;
		p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1*2 - p.x;		//PLU Name 잘리는 현상 수정 (Display Font: 12x20)  <= 208 - 12
		Dsp_Write_1stringLong(p, arab_codetemp);
 #endif
		sale_display_date(ON);
#elif defined(HEBREW_FONT)
		Convert_HebrewPreview((INT8U *)temp_buf, strlen(temp_buf), 0, 0);
		hebrew_codetemp[strlen(temp_buf)] = 0;
		p.x = 196 - p.x;	//PLU Name 잘리는 현상 수정 (Display Font: 12x20)  <= 208 - 12
		Dsp_Write_1stringLong(p, hebrew_codetemp);
		sale_display_date(ON);
#else
		Dsp_Write_String(p, temp_buf);
#endif
		status_scale.cur_displayon&=(~chk);
		DspLoadFont1(old_id);
	}
#else
	INT16U chk;

	chk = UPDATE_CENTER | UPDATE_TITLE | UPDATE_TITLE2 | UPDATE_DATETIME;
	if ((status_scale.cur_displayon & chk) == 0) return;
	status_scale.cur_displayon &= (~chk);

	if (PeelHeadMsgFlag)
	{
		sale_display_msg2(); // PeelOff error, Headup error
		status_scale.cur_displayon &= (~UPDATE_TITLE2);
	}
	else if (MsgDisplayTime)
	{
		sale_display_msg();
		status_scale.cur_displayon &= (~UPDATE_TITLE);
	}
	else if (DateTimeDispTime)
	{
		sale_display_date(OFF);
		status_scale.cur_displayon &= (~UPDATE_DATETIME);
	}
	else
	{
		if (!(PLU_SCROLL.state == PLU_SCROLL_SCROLL) && !scroll_effect)
		{
			sale_display_pluname();
		}
		status_scale.cur_displayon &= (~UPDATE_CENTER);
	}
#endif
}

void sale_display_total(INT8U onlyWeighing)
{
	static INT16U DispCycleTime=0;
	INT8U dispPass;
#ifdef USE_CL5200_DISP_CONTINUOUS_REFRESH
	static INT16U DspTimer10ms = 0;
#endif
	
	if(Operation.fixedPrice && Operation.fixedAction==2) dispPass=1;	//무게 0상태 발행 시 Fixed Price 동작 -> 디스플레이 "FP" 표시 문제
	else dispPass=0;

#ifdef USE_DISPLAY_COMM_SPEED_SETTING
	if((DispRefreshIntvl < (INT16U)(SysTimer10ms - DispCycleTime)) || dispPass)
#else
	if((2 < (INT16U)(SysTimer10ms - DispCycleTime)) || dispPass)
#endif
	{
		DispCycleTime = SysTimer10ms;

		sale_display_sign();
#ifdef NOT_USE_NETSET_LOCK
		status_scale.scale_lock = 0;
#endif
		if (!status_scale.scale_lock) {
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
				sale_display_weight();
				sale_display_tare();
				sale_display_unit();
				sale_display_price();
#else
			if(!Msg7SegDispTime)
			{
				if (!onlyWeighing) {
					sale_display_weight();
					sale_display_tare();
					sale_display_unit();
				}
				sale_display_price();
			}
#endif
			if(!onlyWeighing) {
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
				sale_display_error();
				if(!MsgPLUDisplayTime) sale_display_center();		//Added by JJANG 20080220
#else
				sale_display_center();
#endif
#if !defined(USE_SINGLE_LINE_GRAPHIC)
				sale_display_override();
				sale_display_ClerkSubtotalList(); //Insert by se-hyung for Clerk Subtotal List 20070727
				sale_display_msg();	// left message
				sale_display_msg2();	// right message
#endif
			}
		}
		//display_primary_diffuse();	// VFD7_Diffuse() 대체

		//HYP 20051024 Erase for preventing Electo-discharge
		//		if (!DspStruct.DirectDraw){
		//DEL070705	if (!onlyvfd) Dsp_Diffuse();
		//	if (!onlyvfd) LCD_refresh();
	}
#ifdef USE_CERT_DISP_CONTINUOUS_REFRESH
	if(Disp_refresh_flag == TRUE) 	// continuous refresh
	{
#ifdef _USE_LCD32_
		Dsp_Diffuse();       
#else
		sale_display_update(UPDATE_ERROR|UPDATE_MODE|UPDATE_CENTER|UPDATE_STATUS|UPDATE_TITLE);
#endif        
	}
#elif defined(USE_CL5200_DISP_CONTINUOUS_REFRESH)
	if( (Disp_refresh_flag == TRUE) && (200 < (INT16U)(SysTimer10ms - DspTimer10ms)) ) 	// 2000ms(2s) refresh
	{
		DspTimer10ms = SysTimer10ms;
#ifdef _USE_LCD32_
        Dsp_Diffuse();
#else	        
		sale_display_update(UPDATE_ERROR|UPDATE_MODE|UPDATE_CENTER|UPDATE_STATUS|UPDATE_TITLE);
#endif	
	}
#endif
}

INT8U DispUpdateAll = 0;

//extern void sale_display_ethernet_sign(void);
#ifdef USE_CTS_FUNCTION
extern void sale_display_cts_alarm_sign(void);
#endif

void sale_display_proc(INT8U onlyWeighing)
{
	INT8U backup_directdraw;
	//static INT16U wait_time = 0;
	static long prevTime=0;
	static INT8U prevClerk=0;
	long l_temp;
	char  str_date[24];
	INT8U dateRefresh;

#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
	if (scroll_effect) return;
#endif
	if (KEY_IsKey()) return;
	common_display_sign();
	sale_display_ethernet_sign();
#ifdef USE_CTS_FUNCTION
	sale_display_cts_alarm_sign();
#endif
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
	sale_display_clear_lcd();
#endif
	backup_directdraw = DspStruct.DirectDraw;
	if (DispUpdateAll == 1) {
		DspStruct.DirectDraw = 0;
	}

	//if (sale_display_date(OFF))
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
	if (!DispDisable) dateRefresh = sale_display_date(OFF);
	if (dateRefresh)
	{
		status_scale.flag_display |= 1;
	}
#else
	if (network_timeout_check_10ms(20, NETWORK_TIMEOUT_CHECK_RTC))
	{
		l_temp = rtc_gettime(str_date);
	}
	else
	{
		l_temp = prevTime;
	}
	if (prevTime != l_temp || prevClerk != status_scale.clerkid) // 1초에 한 번 또는 클럭이 바뀌면 업데이트
	{
		status_scale.flag_display |= 1;
		sale_display_update(UPDATE_DATETIME);
	}
	prevTime = l_temp;
	prevClerk = status_scale.clerkid;
#endif
	if (status_scale.flag_display&0x01) {
#ifndef NOT_USE_NETSET_LOCK
		sale_display_lock(status_scale.scale_lock);
#endif
		if (!DispDisable) sale_display_total(onlyWeighing);
		status_scale.flag_display &= 0xfe;
	}
#ifdef USE_CL5200_DISPLAY_DEFAULT // plu 호출하지 않은 상태에서 키 입력시 tare 사라지는 버그 수정
	sale_display_update(UPDATE_TARE);
#endif
	display_primary_diffuse();

	if (KEY_IsKey()) {
		DspStruct.DirectDraw = backup_directdraw;
		return;	// 중간
	}
	if (!DspStruct.DirectDraw) Dsp_Diffuse();

	if (DispUpdateAll == 1) {
		DspStruct.DirectDraw = backup_directdraw;
		DispUpdateAll = 0;
	}
	else
	{
#if 0
		if ((INT16U)(SysTimer100ms - wait_time) > 5)
		{
			wait_time = SysTimer100ms;
			if (!MsgDisplayTime && !MsgRDisplayTime && !MsgPLUDisplayTime && !Clerk_Subtotal_List && !PeelHeadMsgFlag && status_run.run_mode == RUN_SALE)
			{
				LCD_refresh();
			}
		}
#endif
	}
}

#ifdef USE_CTS_FUNCTION
void CtsAlarmProc(void)
{
	static INT8U CtsAlarmRun = OFF;

	if (!CTS_CheckCalibrationDate())
	{
		if(RTCStruct.hour == 0x15 && RTCStruct.min == 0 && RTCStruct.sec < 5)
		{
			CtsAlarmRun = ON;
		}
		if(CtsAlarmRun && (INT16U)(SysTimer100ms - status_scale.key_last_used_100ms) > 100
			&& (INT16U)(SysTimer100ms - status_scale.ad_last_used_100ms)  > 100)
		{
			CtsAlarm(ON);
			CtsAlarmRun = OFF;
			RTC_CLK_Burst_Read();	// CTS alarm이 반복되서 실행되는 것을 막기 위함
		}
	}
}
#endif

#ifdef USE_SETPRODUCT_SALE
long SetproductPrice = 0;
INT8U setproduct_sale_state = 0;
void sale_setproduct_Proc(void)
{
	POINT disp_p;
	INT8U buf[20];
	INT16U result, setNumber, PrtResult;
	
	if(!SetproductSaleMode) return;

	disp_p = point(DSP_PLU_NAME_Y, DSP_PLU_NAME_X);
	Dsp_SetPage(DSP_ERROR_PAGE);
	FontSizeChange(FONT_MEDIUM);

	switch(setproduct_sale_state)
	{
		case SETPRODUCT_SALE_START:
			setproduct_sale_state = SETPRODUCT_SALE_COMPONENTS;
			break;
		case SETPRODUCT_SALE_COMPONENTS:
			if(!status_scale.cur_pluid)
			{
				sprintf((char *)buf, "세트판매모드");
				Dsp_Write_String(disp_p,(char *)buf);
			}
			break;
		case SETPRODUCT_PRINT_SETLABEL:
			sale_pluclear(ON);
			indivMultiAndLotIndexInit();
			display_lcd_clear();
			while(1)
			{
				sprintf((char *)buf, "세트가격:[%6d]", SetproductPrice);
				Dsp_Write_String(disp_p,(char *)buf);
			
				result = keyin_ulong(KI_GRAPHIC, KI_FORM_NORMAL, 0,(long*)&SetproductPrice, 6, DSP_PLU_NAME_Y,DSP_PLU_NAME_X+8*10, 0,KI_NOPASS);
				if(SetproductPrice != 0 && SetproductPrice <= 999999)
				{
					Price.TotalPrice[PRICE_RESULT] = SetproductPrice;
					setproduct_PrtFlag = ON;
					display_lcd_clear();
					StartPrintFlag = ON;
					PrtResult = LabelAndTransaction();
					if(PrtResult == ON)
					{
						setproduct_PrtFlag = OFF;
						TransactionSendWithoutVoid(status_scale.clerkid);
						ClerkDeleteTransaction(status_scale.clerkid);
						DeleteAdditionalData(status_scale.clerkid);
						break;
					}
					else
					{
						BuzOn(2);
						sale_display_msg2();
					}
				}
			}
			if(PrtResult == ON)
				setproduct_sale_state = SETPRODUCT_SALE_START;
			else
				setproduct_sale_state = SETPRODUCT_PRINT_SETLABEL;
			break;
		default:
			setproduct_sale_state = SETPRODUCT_SALE_START;
			break;
	}
}
#endif

#if defined(USE_SETPRODUCT_SALE) || defined(USE_HYUNDAI_SET_FUNCTION)
INT16U get_setproduct_number(void)
{
	INT32U pnt;
	INT16U  value;
	pnt =DFLASH_BASE;
	pnt+=FLASH_SETPRODUCT_SETNO;
	value=Flash_wread(pnt);
	return value;
}

void set_setproduct_number(INT16U v)
{
	INT32U pnt;
	pnt =DFLASH_BASE;
	pnt+=FLASH_SETPRODUCT_SETNO;
	Flash_wwrite(pnt,v);
}

void increase_setproduct_number()
{
	INT16U setNumber = get_setproduct_number();

	setNumber += 1;
	if(setNumber > 9999)
		setNumber = 1;
	set_setproduct_number(setNumber);

}
#endif

#ifdef USE_HYUNDAI_SET_FUNCTION
void sale_hyundai_meatset_proc(void)
{
	POINT disp_p;
	INT8U buf[20];
	INT8U old_page;
	char old_font;
	
	if(Operation.saleStatus == SALE_NORMAL) return;

	old_font = DspStruct.Id1;
	old_page = DspStruct.Page;
	
	Dsp_SetPage(DSP_ERROR_PAGE);
	FontSizeChange(FONT_MEDIUM);	
	disp_p = point(DSP_PLU_NAME_Y, DSP_PLU_NAME_X);
	
	
	switch(Operation.saleStatus)
	{
		case SALE_SET_START:
			Operation.saleStatus = SALE_SET_COMPONENTS;
			break;
		case SALE_SET_COMPONENTS:
			if(!status_scale.cur_pluid) {
				sprintf((char *)buf, "세트 상품 구성");
				Dsp_Write_String(disp_p,(char *)buf);
			}
			break;
		case PRINT_SET_COMPONENTS_CLERK:
		case PRINT_SET_COMPONENTS_CUSTOMER:
		case INPUT_SET_PRICE:
		case PRINT_SET_LABEL:
			break;
		default:
			Operation.saleStatus = SALE_SET_START;
			break;
	}

	Dsp_SetPage(old_page);
	DspLoadFont1(old_font);
}

void hyundai_sale_set_components(void)
{
	char string_buf[50];
	INT8U delay_time = 5;
	
	if(Operation.saleStatus == PRINT_SET_COMPONENTS_CLERK)
	{
		MeatSetCompNum++;

		sprintf(string_buf, "%2d개 판매", (INT16U)MeatSetCompNum);
		display_message_page_mid_delay(string_buf,delay_time);
	}
	else if(Operation.saleStatus == PRINT_SET_LABEL)
	{
		sprintf(string_buf, "세트상품 판매완료");
		display_message_page_mid_delay(string_buf,delay_time);
	}
}

INT16S hyundai_print_set_customer(void)
{
	if(MeatSetSaleUseScanner) Operation.printON = OFF;
	while(1)
	{
		if(KEY_Read())
			break;
		if(Operation.printON)
		{
			StartPrintFlag=ON;
			if(!CheckPeelHead(ON, OFF))	continue;
			Prt_PrintStart(PRT_ITEM_NORMAL_MODE, PrtStruct.Mode, 0, OFF, 0, 0);
		}

		if(PrtDrvStruct.GapSenErrFlag) //no gap
		{
			sale_display_update_error(0xE605,0,0);
			PrtDrvStruct.GapSenErrFlag=OFF;
			Key_DelaySec(10,0);
		}
		if (PrtDrvStruct.OutOfPaperFlag)//long gap
		{
			set_nvram_bparam(NVRAM_PRT_OUTOFPAPER, ON);
			sale_display_update_error(0xE601,0,0);
			Key_DelaySec(10,0);
		}
		break;
	}
	Operation.saleStatus = SALE_SET_COMPONENTS;
	return ON;
}

#endif

#ifdef USE_REGIST_DISPOSAL
void sale_display_disposal(INT8U mode)
{
	POINT disp_p;
	INT8U buf[20];
	INT8U old_page;
	char old_font;

	old_font = DspStruct.Id1;
	old_page = DspStruct.Page;
	
	Dsp_SetPage(DSP_ERROR_PAGE);
	FontSizeChange(FONT_MEDIUM);	
	disp_p = point(DSP_PLU_NAME_Y, DSP_PLU_NAME_X);

	switch(mode){
		case REGIST_DISPOSAL_OFF:
			sprintf((char *)buf,"폐기 등록 완료");
			display_message_page_mid((char *)buf);
			display_lcd_clear_buf();
			Dsp_Diffuse();
			break;
		case REGIST_DISPOSAL_ON:
			sprintf((char *)buf, "폐기 등록 시작");
			display_message_page_mid((char *)buf);
			display_lcd_clear_buf();
			Dsp_Diffuse();
			break;
		case REGIST_DISPOSAL_STAT_DISP:
			if(!status_scale.cur_pluid) {
				sprintf((char *)buf, "폐기 및 시료 등록");
				Dsp_Write_String(disp_p,(char *)buf);
			}
		default:
			break;		
	}

	Dsp_SetPage(old_page);
	DspLoadFont1(old_font);
}
#endif

#ifdef USE_SUBTOTAL_PRICE_9DIGIT_LIMIT
INT8U sale_check_subtotal_price(INT8U clerk_id, long total_price)
{
    STRUCT_TOTAL_PRICE totalData;
    INT8U result;

    ClerkTotalPrice(0, clerk_id, &totalData);

    if(totalData.TotalPrice + total_price > 999999999L)
        result = FALSE;
    else
        result = TRUE;    
    
    return result;
}
#endif
