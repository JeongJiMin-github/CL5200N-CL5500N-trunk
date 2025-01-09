/*****************************************************************************
|*  Copyright		:	(c) 2003 CAS
|*  Filename		:	prt_cmdtest.c
|*  Version		:	0.1
|*  Modified		:	2004/08/20
|*  Modifier		:	CJK
****************************************************************************/
#include <stdio.h>
#include <string.h>
#include "globals.h"
#include "initial.h"
#include "commbuf.h"
#include "prt_common.h"
#include "bar_common.h"
#include "prtdrv.h"
#include "prt_line.h"
#include "prt_font.h"
#include "main.h"
#include "mode_main.h"
#include "mode_commun.h"
#include "mode_sale.h"
#include "flash.h"
#include "flash_app.h"
#include "common.h"
#include "caption_app.h"
#include "common_sunday.h"
#include "plu_manager.h"
#include "rtc.h"
#include "prt_font.h"
#include "plu_auxcall.h"

#include "plu_manager.h"
#include "tax.h"
#include "rtc.h"
#include "rtc_app.h"
#include "common_sunday.h"
#include "origin.h"
#include "prt_advertise.h"
#include "shopname.h"
#include "clerk_app.h"
#include "customer_app.h"
#include "ingredient.h"
#include "tare_file.h"
#include "pcs_unit.h"
#include "department.h"
#include "pcs_unit.h"
#include "traceability.h"
#include "workplace.h"
#include "label_caption.h"
#include "adm_app.h"
#include "adm.h"
#include "nutrition_fact.h"
#ifdef USE_EU_NUTRITIONFACT
#include "nutrition_fact_caption.h"
#endif
#include "menu.h"
#include "plu_device.h"
#include "prt_app.h"
#include "prt_cmd.h"
#include "initial_glb.h"

#include "dsp_common.h"
#include "key_typedef.h"
#include "pgm_app.h"
#include "key_app.h"
#include "ethernet_comm.h"
#include "key.h"
#include "key_function.h"

#include "prt_cmdtest.h"
#include "prt_list.h"
#include "dsp_app.h"
#include "barcode_file.h"
#include "global_set.h"

#ifdef USE_SRAM_SPI_COMM
#include "spi_sram.h"
#endif
#include "scale_config.h"

// Modified by CJK 20050726
#define WAIT_2ND_TICKET_100MS	20

const PRTSEL_MENU_TYPE cap2field_table[] = {
	{0x1a40,	FIELD_ID_DEPT_NAME	},//	0	Department Name     (Y/N) :	부문이름  (Y/N):	
	{0x1a41,	FIELD_ID_DEPT_NO	},//	1	Department Number   (Y/N) :	부문번호  (Y/N):	
	{0x1a42,	FIELD_ID_GROUP_CODE	},//	2	Group Number        (Y/N) :	그룹번호  (Y/N):	
	{0x1a43,	FIELD_ID_PLU_NO		},//	3	PLU Number          (Y/N) :	PLU 번호  (Y/N):	
	{0x1a44,	FIELD_ID_NAME1		},//	4	PLU Name1           (Y/N) :	상품명1   (Y/N):	
	{0x1a45,	FIELD_ID_NAME2		},//	5	PLU Name2           (Y/N) :	상품명2   (Y/N):	
	{0x1a46,	FIELD_ID_NAME3		},//	6	PLU Name3           (Y/N) :	상품명3   (Y/N):	
	{0x1a47,	FIELD_ID_COMMODITY_CODE	},//	7	Item Code           (Y/N) :	상품코드  (Y/N):	
	{0x1a48,	FIELD_ID_WEIGHT_UNIT	},//	8	Unit Weight         (Y/N) :	단위무게  (Y/N):	
	{0x1a49,	FIELD_ID_TAX_RATE	},//	9	Tax Rate            (Y/N) :	세율      (Y/N):	
	{0x1a4a,	FIELD_ID_TAX_PRICE	},//	10	Tax Price           (Y/N) :	세금      (Y/N):	
	{0x1a4b,	FIELD_ID_TARE_VALUE	},//	11	Tare Weight         (Y/N) :	용기무게  (Y/N):	
	{0x1a4c,	FIELD_ID_UNIT_PCS	},//	12	Pieces              (Y/N) :	개수      (Y/N):	
	{0x1a4d,	FIELD_ID_PCS_SYM_STR	},//	13	Qty Symbol          (Y/N) :	개수단위  (Y/N):	
	{0x1a4e,	FIELD_ID_SELL_BY_DATE	},//	14	Sell By Date        (Y/N) :	유효일    (Y/N):	
	{0x1a4f,	FIELD_ID_SELL_BY_TIME	},//	15	Sell By Time        (Y/N) :	유효시간  (Y/N):	
	{0x1a50,	FIELD_ID_PACKED_DATE	},//	16	Packed On Date      (Y/N) :	발행일    (Y/N):	
	{0x1a51,	FIELD_ID_PACKED_TIME	},//	17	Packed On Time      (Y/N) :	발행시간  (Y/N):	
	{0x1a52,	FIELD_ID_PRODUCED_DATE	},//	18	Produced Date       (Y/N) :	제조일    (Y/N):	
	{0x1a53,	FIELD_ID_COOK_BY_DATE	},//	19	Cook By Date        (Y/N) :	요리일    (Y/N):	
	{0x1a54,	FIELD_ID_BONUS_POINT	},//	20	Bonus Point         (Y/N) :	보너스점수(Y/N):	
	{0x1a55,	FIELD_ID_ORIGIN_STR	},//	21	Origin              (Y/N) :	원산지    (Y/N):	
	{0x1a56,	FIELD_ID_INGREDIENT_STR	},//	22	Ingredient          (Y/N) :	성분및원료(Y/N):	
	{0x1a57,	FIELD_ID_LABEL_MSG_STR	},//	23	Sales Message       (Y/N)	판매광고  (Y/N):	
	{0x1a58,	FIELD_ID_NET_WEIGHT	},//	24	Refer. Net Weight   (Y/N) :	참고순중량(Y/N):	
	{0x1a59,	FIELD_ID_GROSS_WEIGHT	},//	25	Refer. Gross Weight (Y/N) :	참고총중량(Y/N):	
	{0x1a5a,	FIELD_ID_PERCENT_TARE	},//	26	Percent Tare        (Y/N) :	퍼센트용기(Y/N):	
	{0x1a5b,	FIELD_ID_PERCENT_TARE_LIMIT}	,//	27	Percent Tare Limit  (Y/N) :	퍼센트제한(Y/N):	
	{0x1a5c,	FIELD_ID_SAVED_PRICE	},//	28	Saved Price         (Y/N) :	할인된금액(Y/N):	
	{0x1a5d,	FIELD_ID_TRANSACTION_NO	},//	29	Label Serial        (Y/N) :	판매번호  (Y/N):	
	{0x1a5e,	FIELD_ID_SHOP_NAME_STR	},//	30	Store Name          (Y/N) :	상점명    (Y/N):	//105,138,139(Store Name)
	{0x1a5f,	FIELD_ID_CLERK_NO	},//	31	Clerk Number        (Y/N) :	사용자번호(Y/N):	
	{0x1a60,	FIELD_ID_CLERK_NAME	},//	32	Clerk Name          (Y/N) :	사용자이름(Y/N):	
	{0x1a61,	FIELD_ID_CUSTOMER_NAME	},//	33	Customer Name       (Y/N) :	고객명    (Y/N):	
	{0x1a62,	FIELD_ID_CUSTOMER_NO	},//	34	Customer Number     (Y/N) :	고객번호  (Y/N):	
	{0x1a63,	FIELD_ID_SCALE_NO	},//	35	Scale Number        (Y/N) :	저울번호  (Y/N):	
	{0x1a64,	FIELD_ID_WEEK_DAY	},//	36	Day of Week         (Y/N) :	발행요일  (Y/N):	
	{0x1a65,	FIELD_ID_CURRENCY_RATE	},//	37	Currency Rate       (Y/N) :	환율      (Y/N):	
	{0x1a66,	FIELD_ID_2ND_TOTALPRICE	},//	38	Second Total Price  (Y/N) :	변환가격  (Y/N):	
	{0x1a67,	FIELD_ID_2ND_UNIT_PRICE	},//	39	Second Unit Price   (Y/N) :	변환단가  (Y/N):	
#ifdef USE_TRACEABILITY
	{0x1a68,	FIELD_ID_TRACEABILITY_CODE}	,//	40	Traceability        (Y/N) :	유통정보  (Y/N):	
#endif
	{0x1a69,	FIELD_ID_N_SERVING_SIZE	},//	41	Nutrition Fact      (Y/N) :	영양성분  (Y/N):	
	{0x1a6a,	FIELD_ID_CAP_TOTAL	},//	42	Label Caption       (Y/N) :	아이템제목(Y/N):	
	{0x1a6b,	FIELD_ID_TTL_TAX_AMOUNT	},//	43	Total Tax Price     (Y/N) :	총세금액  (Y/N):	
	{0x1a6c,	FIELD_ID_TTL_2ND_TOTAL_PRICE}	,//	44	Second Add Total    (Y/N) :	변환총액  (Y/N):	
	{0x1a6d,	FIELD_ID_BARCODE	},//	45	Barcodes            (Y/N) :	바코드    (Y/N):	
	{0x1a6e,	FIELD_ID_IMAGE	},//	46	Image               (Y/N) :	그림      (Y/N):	
	{0x1a6f,	FIELD_ID_WEIGHT	},//	47	Weight              (Y/N) :	무게      (Y/N):	//101,108,132(Weight)		       //se-hyung 20071002
	{0x1a70,	FIELD_ID_QUANTITY	},//	48	Quantity            (Y/N) :	수량      (Y/N):	//112,133,134(Quantity)
	{0x1a71,	FIELD_ID_UNIT_PRICE	},//	49	Unit Price          (Y/N) :	단가      (Y/N):	//  6,109,113(Unit Price)
	{0x1a72,	FIELD_ID_TOTALPRICE	},//	50	Total Price         (Y/N) :	총액      (Y/N):	//102,110,114(Total Price)
	{0,	0	},//	end	// 64를 초과하지않도록			
};
LABEL_DATA_BACKUP_TYPE LabelData;
STRUCT_PRT_ITEM PrtItemStr;
STRUCT_PRT_TOTAL PrtTotalStr;
STRUCT_PRT_COMMON PrtComStr;
STRUCT_LABEL_SET label_set;
INT8U PrtWeightUnit, PrtWeightUnitId;
INT8U MarkDownLineNum = FONT_CON_TYPE_THROUGH_LINE;
INT8U OneItemPrepack=OFF;
INT8U LotStr[4] = {"LOT"};
INT8U LotStrL[4] = {"lot"};
INT8U GlbBarcodeData_Field1[BARCODE_FORMAT_LEN+1];	// > BARCODE_FORMAT_LEN
#ifdef USE_GSMART_PLU
INT8U PluNameFontSizeId[8] = 	{39, 40, 41, 42, 43, 44, 45, 46};
INT8U PluNameId[8] = 		{10, 30, 31, 92, 93, 94, 95, 96};
#endif
INT8U PrtFirstLabel;
BOOLEAN ExtPrtTotalFlag;
INT8U ExtPrtTotalTypeFlag;
BOOLEAN FlagExternalPrtAddUp = OFF;
#ifdef USE_QR_ADDTOTAL
INT8U TotalQRFlag = 0;
#endif
#ifdef USE_CONTINUOUS_LABEL_WITH_INGREDIENT
short ingredient_height_offset;
short ingredient_y_pos;
INT8U ingredient_height_adjust_flag;
#endif
INT8U OtherWeightUnit;
TRANSACT_CHAIN  TrChain; //SG070213
TRANSACTION_RECORD_SUB_TYPE Trans_sub;
TR_ADDITION_DATA_TYPE TrReturn_add;

#ifdef USE_DISCOUNT_MEMBERSHIP
long Prt_Calc_Discount_Price(long price,long percent);
#endif

extern HUGEDATA STR_FIELD_STRUCT StrFieldStruct[MAX_STR_FIELD_NUM+MAX_BAR_FIELD_NUM];
extern HUGEDATA BAR_FIELD_STRUCT BarFieldStruct[MAX_BAR_FIELD_NUM];
extern CURRENCY_PRICE_TYPE	CurrencyPrice;
extern PRICE_TYPE  		PriceFSP;
extern PRICE_TYPE Price;
extern PRICE_TYPE MemberPrice;
extern long	TicketTransactionNumber[];
extern INT32U NVRAM_CLEARK_LASTSALE_PNT;
extern PRINT_OPER_TYPE		PrtOper;	// Added by CJK 20051226
extern PREPACK_STATUS_TYPE PrepackStatus;
#ifdef USE_DONT_SAVE_VALIDDATE_SALEDATE
extern TEMP_SELLDATE_STRUCT temp_selldate;
#endif
extern char PeelHeadMsgFlag;
extern INT8U UseDisplayMsg2Area;
extern INT8U StartPrintFlag;
extern CAPTION_STRUCT cap;
extern long	CounterTicket;
extern SALE_G_OPER_TYPE	GlbOper;
extern long	CounterTicket;
extern long	CounterTransaction;
extern INT8U DividedPluName[80];
extern char gmsg[]; //Test only
extern DISC_SALE_TYPE 	DiscSale;
extern char barcodetype_codevalue[];
#ifdef USE_GSMART_PLU
extern INT8U PrtFontType;
#endif
extern INT8U	ClerkLastIndex;
extern INT8U	ClerkLastNumber;
extern INT32U	ClerkLastHead;
extern SALE_G_FLOAT_TYPE	GlbFloat;
extern SALE_G_ADD_TYPE		GlbAddup;
extern INT8U LinkedPrintFlag;
extern TRANSACTION_PAY transPay;
extern DISC_SALE_TYPE		DiscSale;
extern long InventoryUnitWeight;
extern DISC_SALE_TYPE	DiscSale;
extern INT32U InventoryQuantity;
extern INT8U GlbFlagPCnMasterConnection;
extern INT8U printdateOFF;
extern char gmsg[];
#ifdef USE_COPYKEY
extern INT8U flagcopykey;
#endif
extern INT8U   plu_base_change;
extern INT32U	ClerkLastHead;
#ifdef USE_TRACE_STANDALONE
extern INT8U PrtFontType;
extern INT8U UseLotNum;
#endif
extern INT8U Prt_labelCap_flag;
extern INT8U Prt_uprice_Symbol_flag;
extern INT8U Prt_weight_Symbol_flag;
#ifdef USE_HYUNDAI_SET_FUNCTION
extern RTC_STRUCT SetSaleRTC;
#endif
extern void CheckCashClose(void); //se-hyung 070813
extern char BarUpc_Checksum(HUGEDATA char *data,short leng);
#ifdef USE_LOTTEMART_BARCODE_FUNCTION
extern char Lottemart_GS1_Checksum(HUGEDATA char *data,short leng);
#endif
extern INT8U Flash_check_nvbuffer_4_rd(INT32U start_addr, INT32U byte_count);    
extern INT16S	PutSmallString(INT16S y, INT16S x, char *data);
extern INT32U ing_calc_addr_by_key(INT16U ingid);
extern INT8U OnOff(INT8U tmp);
extern void prtfield_set_variable(INT8U label_mode);
extern void prtfield_set_serial_no(INT8U label_mode, INT8U linked);
extern void keyapp_cash_open(INT16S beepFlag);
extern INT8U Prt_TotalList(INT8U mode, INT16U ClerkNum, RTC_STRUCT *time_str, INT8U copy_ticket);// Modified by CJK 20050726
extern INT8U Prt_TotalList_Credit(INT8U mode, INT16U ClerkNum, RTC_STRUCT *time_str);
extern INT8U Prt_TotalBarcode(void);
#ifdef USE_TAX_RATE
extern INT16S GetTaxData(PRICE_TYPE *pPrice,INT8U taxNumber,INT8U misc_flag);
#endif
extern INT16S Prt_CalcTotalPrice(INT8U addr_mode, INT8U ClerkNum, INT8U ret_chk);
extern INT32U label_rom_addr(INT16U id);
extern void PrintListWaitEndWithoutComm(void);
extern INT16U find_ingredient_y_interpreter(HUGEDATA COMM_BUF *label_str);
#ifdef USE_COPYKEY
extern void Prt_copykey(void);
#endif
extern void TotalRcpt_PrintBarcode(void);
extern void PrintTotalHead(INT16U clerkNum, RTC_STRUCT *cur_time);
extern void TotalRcpt_PrintListTitle(INT8U w_unit);
extern INT8U PrintOneTransaction(TRANSACT_CHAIN  *cp, INT8U w_unit);
extern void PrintTotalTail(INT8U copy_ticket);// Modified by CJK 20050726
extern void TotalRcpt_PrintTicketSerial(void);
extern void TotalRcpt_CustomerName(INT8U customerNo);
extern void TotalRcpt_PrintCreditTotal(long totalPrice);
extern void PrintCreditSign(void);
#ifdef USE_TRACE_STANDALONE
extern void StrInit4UserLabel(void);
#endif
extern void start_external_print(INT16U count);
    
    
#ifdef USE_TRACEABILITY
INT8U get_traceability_name(INT16U id, HUGEDATA char *name)
{
	INT32U addr;
	
	addr = DFLASH_BASE + FLASH_TRACEABILITY_AREA;
	addr = addr + TRACEABILITY_STRUCT_SIZE * (INT32U)(id-1);
	if(id != Flash_wread(addr)) return FALSE;
	addr += 2;
	Flash_sread(addr, (HUGEDATA INT8U *)name, TRACEABILITY_STRUCT_SIZE-10);
	name[TRACEABILITY_STRUCT_SIZE-10] = 0;
	return TRUE;
}

INT8U get_country_name(INT16U id, HUGEDATA char *name)
{
	INT32U addr;
	
	addr = DFLASH_BASE + FLASH_COUNTRY_AREA;
	addr = addr + COUNTRY_STRUCT_SIZE * (INT32U)(id-1);
	if(id != Flash_wread(addr)) return FALSE;
	addr += 2;
	Flash_sread(addr, (HUGEDATA INT8U *)name, COUNTRY_STRUCT_SIZE-2);
	name[COUNTRY_STRUCT_SIZE-2] = 0;
	return TRUE;
}

INT8U get_slaughterhouse_name(INT16U id, HUGEDATA char *name)
{
	INT32U addr;
	
	addr = DFLASH_BASE + FLASH_SLAUGHTERHOUSE_AREA;
	addr = addr + SLAUGHTERHOUSE_STRUCT_SIZE * (INT32U)(id - 1);
	if(id != Flash_wread(addr)) return FALSE;
	addr += 2;
	Flash_sread(addr, (HUGEDATA INT8U *)name, SLAUGHTERHOUSE_STRUCT_SIZE-4);
	name[SLAUGHTERHOUSE_STRUCT_SIZE-4] = 0;
	return TRUE;
}

INT8U get_slaughtercountry_name(INT16U slaught_no, HUGEDATA char *name, INT16U *country_id)
{
	INT32U addr;
	
	addr = DFLASH_BASE + FLASH_SLAUGHTERHOUSE_AREA;
	addr = addr + SLAUGHTERHOUSE_STRUCT_SIZE * (INT32U)(slaught_no - 1);
	if(slaught_no != Flash_wread(addr)) return FALSE;
	addr += SLAUGHTERHOUSE_COUNTRY_OFFSET;
	*country_id = Flash_wread(addr);
	if(!(*country_id)) return FALSE;
	if(!get_country_name(*country_id, name)) return FALSE;
	return TRUE;
}

INT8U get_cuttinghall_name(INT16U id, HUGEDATA char *name)
{
	INT32U addr;
	
	addr = DFLASH_BASE + FLASH_CUTTINGHALL_AREA;
	addr = addr + CUTTINGHALL_STRUCT_SIZE * (INT32U)(id - 1);
	if(id != Flash_wread(addr)) return FALSE;
	addr += 2;
	Flash_sread(addr, (HUGEDATA INT8U *)name, CUTTINGHALL_STRUCT_SIZE-4);
	name[CUTTINGHALL_STRUCT_SIZE-4] = 0;
	return TRUE;
}

INT8U get_cuttingcountry_name(INT16U cutting_no, HUGEDATA char *name, INT16U *country_id)
{
	INT32U addr;
	
	addr = DFLASH_BASE + FLASH_CUTTINGHALL_AREA;
	addr = addr + CUTTINGHALL_STRUCT_SIZE * (INT32U)(cutting_no - 1);
	if(cutting_no != Flash_wread(addr)) return FALSE;
	addr += CUTTINGHALL_COUNTRY_OFFSET;
	*country_id = Flash_wread(addr);
	if(!(*country_id)) return FALSE;
	if(!get_country_name(*country_id, name)) return FALSE;
	return TRUE;
}
#endif

INT8U Prt_CheckPeelAndHead(INT8U check_peeloff)
{
	INT8U result;

	result = ON;

	if (CheckPeelHead(check_peeloff, OFF))
	{
		return ON;
	}
	else
	{
		BuzOn(2);
	}

	if (check_peeloff) UseDisplayMsg2Area = ON;
	while(1) {
		if(KEY_Read()) {
			if(KeyDrv.CnvCode == KP_CLEAR || KeyDrv.CnvCode == KP_ESC
				|| KeyDrv.CnvCode == KS_CLEAR || KeyDrv.CnvCode == KS_MULTI) {
				BuzOn(1);
				CheckPeelHead(check_peeloff, ON);
				result = OFF;
				break;
			} else if(KeyDrv.CnvCode == KP_FEED && PeelHeadMsgFlag != 1) {
				BuzOn(1);
				PutCharRxBuf(&CommBufPrt, 0x0c);
				result = OFF;
				break;
			} else {
				BuzOn(2);
			}
		}
			if (CheckPeelHead(check_peeloff, OFF))
			{
				result = ON;
				break;
			}
	}
	UseDisplayMsg2Area = OFF;
	return result;
}


INT8U Prt_CheckOutOfPaper(void)
{
	CAPTION_STRUCT cap;
	POINT disp_p;
	INT8U backup_directdraw, result;
	char string_buf[36];
	char old_font;
	INT8U flagOut;

	old_font=DspStruct.Id1;
	backup_directdraw = DspStruct.DirectDraw;
	DspStruct.DirectDraw = 1;
	result = ON;
	flagOut = get_nvram_bparam(NVRAM_PRT_OUTOFPAPER);
	if (flagOut) {
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
		sale_display_update_error(0xE602,0,0);
		Key_DelaySec(10,0);
#else
		caption_split_by_code(0xE602,&cap,0);
#endif
		result = OFF;
	} else {
		if (flagOut != PrtDrvStruct.OutOfPaperFlag) // 변화 있을 때만 저장하도록 수정
		{
			set_nvram_bparam(NVRAM_PRT_OUTOFPAPER, PrtDrvStruct.OutOfPaperFlag);
		}
		if (PrtDrvStruct.OutOfPaperFlag) {
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
			sale_display_update_error(0xE601,0,0);
			Key_DelaySec(10,0);
#else
			caption_split_by_code(0xE601,&cap,0);
#endif
			result = OFF;
		}
	}
#if !defined(_USE_LCD20848_) && !defined(_USE_LCD32_)
	if(result == OFF) {
		Dsp_SetPage(DSP_ERROR_PAGE);
		DspLoadFont1(DSP_SMALL_FONT_ID);
		disp_p = point(0,0);
		sprintf(string_buf, "%-34s", cap.form);
#ifdef HEBREW_FONT
		Convert_HebrewPreview((INT8U *)string_buf, strlen(string_buf), 0, 0); 
		hebrew_codetemp[strlen(string_buf)] = 0;
		disp_p.x = 202 - disp_p.x;
		Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
		Convert_ArabPreview((INT8U *)string_buf, strlen(string_buf), 0, 0, 0); 
		arab_codetemp[strlen(string_buf)] = 0;
		disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
		Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
		Dsp_Write_String(disp_p, string_buf);
#endif
		Delay_100ms(15);

		DspStruct.DirectDraw = backup_directdraw;

		Dsp_SetPage(DSP_DEFAULT_PAGE);
		DspLoadFont1(old_font);
		Dsp_Diffuse();
	}
#endif
	return result;
}

INT8U Prt_CheckGapErr(void)
{
	CAPTION_STRUCT cap;
	POINT disp_p;
	INT8U result, backup_directdraw;
	char string_buf[36];
	char old_font;

	old_font=DspStruct.Id1;

	if (!PrtDrvStruct.GapSenErrFlag) return ON;
	
	result = OFF;

	backup_directdraw = DspStruct.DirectDraw;
	DspStruct.DirectDraw = 0;

	Dsp_SetPage(DSP_ERROR_PAGE);
	DspLoadFont1(DSP_SMALL_FONT_ID);

	DspStruct.DirectDraw = 1;

	disp_p = point(0,0);
	memset(&cap,0,sizeof(cap));
	if (PrtDrvStruct.GapSenErrFlag) {
		PrtDrvStruct.GapSenErrFlag=OFF;
		caption_split_by_code(0xE605,&cap,0);
	}
	sprintf(string_buf, "%-34s", cap.form);
#ifdef HEBREW_FONT
	Convert_HebrewPreview((INT8U *)string_buf, strlen(string_buf), 0, 0); 
	hebrew_codetemp[strlen(string_buf)] = 0;
	disp_p.x = 202 - disp_p.x;
	Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
	Convert_ArabPreview((INT8U *)string_buf, strlen(string_buf), 0, 0, 0); 
	arab_codetemp[strlen(string_buf)] = 0;
	disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
	Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
	Dsp_Write_String(disp_p, string_buf);
#endif
	Key_DelaySec(15,0);

	DspStruct.DirectDraw = backup_directdraw;

	Dsp_SetPage(DSP_DEFAULT_PAGE);
	DspLoadFont1(old_font);
	Dsp_Diffuse();

	return result;
}

INT8U PrintGrandTotal(INT16U clerkNum, INT16S count, long price, long weight, INT16S qty, long nweight, long per_tweight, long gweight)
{
	INT8U result;
	INT8U prt_total_conti = 0;
	INT8U	escape_prt = 0;
	prt_total_conti = get_global_bparam(GLOBAL_PRINT_OPER_FLAG2) & 0x20;     //1872 Prt total label conti
	memset((INT8U *)&TotalData, 0, sizeof(STRUCT_TOTAL_PRICE));
	TotalData.TotalCount = count;
	TotalData.TotalPrice = price;
	TotalData.TotalWeight = weight;
	TotalData.TotalQuantity = qty;
	TotalData.SummaryTotalPrice = price;

	if (!clerkNum)
	{
		TotalData.TotalNetWeight = nweight;
		TotalData.TotalPerTareWeight = per_tweight;
		TotalData.TotalGrossWeight = gweight;
	}
	result = Prt_PrintStart(PRT_TOTAL_GRAND_MODE, PrtStruct.Mode, clerkNum, ON, 0, 0);
	if (result != 1) result = OFF; 	//paper end err
	if(result == 1)
	{
		if(prt_total_conti)
		{
			if(Startup.country != COUNTRY_KR)
			{
				caption_split_by_code(0x97A3, &cap, 0);     //"[ESC]=escape"
				PutSmallString(3,0,cap.form);
				Dsp_Diffuse();
			}
			while(!escape_prt) 
			{
				while(!KEY_Read());
				switch(KeyDrv.CnvCode) 
				{
					case KS_PRINT :
					case KS_EXT_PRINT:
						result = Prt_PrintStart(PRT_TOTAL_GRAND_MODE, PrtStruct.Mode, clerkNum, ON, 0, 0);
						if(result != 1) 	escape_prt = ON;
						break;
					case KS_CLEAR : 
					case KS_MULTI : 
						escape_prt = ON;
						break;
					default :
						BuzOn(2);
						break;
				}
			}
		}
	}
	return result;
}

INT16U Get1stLabelId(INT16U dept_no, INT32U plu_no)
{
	INT32U pnt;
	INT8U  gs;
	INT8U  flag;
	INT16U labelid;
	INT16S    sz;
	INT32U plu_address;
#ifdef USE_SPAR_MEMBER_PRICE
    INT32U member_price;
#endif

	labelid = 0;

	gs = get_global_bparam(GLOBAL_LABEL_PRIORITY);

	switch(plu_no) {
		case MISC_PLU_WEIGHT :
		case MISC_PLU_PCS    :
		case MISC_PLU_COUNT :	
			pnt = GLOBAL_LABEL_MISC;
			break;
		case 0:
			pnt = GLOBAL_LABEL_STANDARD;	
			break;
		default :
			pnt = GLOBAL_LABEL_STANDARD;
			if(GlbOper.UseFSP && DiscSale.saleDiscFSP) //SG061215. FSP Label
			{
				pnt = GLOBAL_LABEL_FSP;
			}
#ifdef USE_SPAR_MEMBER_PRICE
            plu_table_search(dept_no,plu_no,(INT16U *)&plu_address,0);
            plu_get_field(plu_address-1,PTYPE_MEMBER_PRICE,(INT8U *)&member_price,&sz,sizeof(member_price));
            if(member_price)
            {
                pnt = GLOBAL_LABEL_MEMBER_PRICE;
            }
#endif
			if ((gs&0x01) == 0) {// Global Label 적용하지 않을 경우
				plu_table_search(dept_no,plu_no,(INT16U *)&plu_address,0);
				plu_get_field(plu_address-1,PTYPE_LABEL_NO,(INT8U *)&labelid,&sz,sizeof(labelid));
			}
			break;
	}
	
	if(PrtStruct.Mode == LABEL && Operation.operationClerk == OPER_FLOATING) 
	{
		// Label & Floating Mode 일때만 동작하도록 한정 함		
		// Setting 0 = 라벨 인쇄 안함, setting 1 = Default Label Format으로 인쇄 함	
		flag = get_global_bparam(GLOBAL_SALE_SETUP_FLAG11);
		if(OnOff(flag & 0x80)) 
		{
			if (labelid == 0) labelid = get_global_wparam(pnt);	// global setting이거나, PLU Label이 0일때
		}
	}
	else
	{
		if (labelid == 0) labelid = get_global_wparam(pnt);	// global setting이거나, PLU Label이 0일때
	}
	return labelid;
}

INT16U Get2ndLabelId(INT16U dept_no, INT32U plu_no)
{
	INT32U pnt;
	INT16U labelid;
	INT16S    sz;
	INT32U plu_address;
	INT8U  flag;
	
	labelid = 0;

	pnt = GLOBAL_LABEL_2;    
	switch(plu_no) {
		case MISC_PLU_WEIGHT :
		case MISC_PLU_PCS    :
		case MISC_PLU_COUNT  :	
		case 0:
			labelid = 0;
			break;
		default :
			plu_table_search(dept_no,plu_no,(INT16U *)&plu_address,0);
#ifndef USE_TOPMART_DEFAULT
			plu_get_field(plu_address-1,PTYPE_AUX_LABEL_NO,(INT8U *)&labelid,&sz,sizeof(labelid));
#endif
			break;
	}
	
	if(PrtStruct.Mode == LABEL && Operation.operationClerk == OPER_FLOATING) 
	{
		// Label & Floating Mode 일때만 동작하도록 한정 함		
		// Setting 0 = 라벨 인쇄 안함, setting 1 = Default Label Format으로 인쇄 함	
		flag = get_global_bparam(GLOBAL_SALE_SETUP_FLAG11);
		if(OnOff(flag & 0x80)) 
		{
	     if (labelid == 0) labelid = get_global_bparam(pnt);	// global setting이거나, PLU Label이 0일때
		}
	}
	else
	{
	if (labelid == 0) labelid = get_global_bparam(pnt);	// global setting이거나, PLU Label이 0일때
	}
	
	if(labelid > 99) labelid = 0;
	return labelid;
}

#ifdef USE_AUSTRALIA_COOL_FUNCTION
void prt_get_plu_coolinfo(INT32U plu_addr, STRUCT_COOL_IMAGE *coolinfo)// Plu에 종속된 항
{
	INT16U sz;
	INT32U addr;
	ORIGIN_STRUCT		origin_str;	
	INT16S  	str_size;	
	INT16U ingredNo[DIRECT_INGREDIENT_BLOCK_NUM];	

	memset(coolinfo, 0, sizeof(STRUCT_COOL_IMAGE));
	// Get Cool Type
	plu_get_field((INT16S)plu_addr-1, PTYPE_REF_DEPT_NO, (INT8U *)&(coolinfo->Cooltype), (INT16S *)&sz,sizeof(coolinfo->Cooltype));
	if(coolinfo->Cooltype == 0)return; // Don't have any cool type
	if(coolinfo->Cooltype > 9)return;  // Invalied value

	// Get Cool Contains
	plu_get_field((INT16S)plu_addr-1, PTYPE_REF_PLU_NO, (INT8U *)&(coolinfo->CoolContainPercent), (INT16S *)&sz,sizeof(coolinfo->CoolContainPercent));

	// Get Cool Origin
	plu_get_field((INT16S)plu_addr-1, PTYPE_ORIGIN_NO, (INT8U *)&(coolinfo->CoolOrigin), (INT16S *)&sz,sizeof(coolinfo->CoolOrigin));
	// Get Origin Text
	if(coolinfo->CoolOrigin != 0)
	{
		str_size = ORIGIN_STRUCT_SIZE;	
		addr  = str_size * ((coolinfo->CoolOrigin)-1);
		addr += DFLASH_BASE + FLASH_ORIGIN_AREA;
		Flash_sread(addr, (HUGEDATA INT8U *)&origin_str, str_size);
		if(origin_str.origin_code == coolinfo->CoolOrigin)strcpy(coolinfo->CoolOriginText,(char*)origin_str.origin_text);
		else coolinfo->CoolOriginText[0] = 0;
	}
	else coolinfo->CoolOriginText[0] = 0;
			
	// Get Cool Function Custom Phrase
	plu_get_field((INT16S)plu_addr-1, PTYPE_INGREDIENT_TEXT, (INT8U *)&ingredNo[0], (INT16S *)&sz,sizeof(ingredNo));	//Direct Ingredient
	if(ingredNo[0]) //Exist Custom Pharase
	{
		ingredient_text_read(ingredNo, (HUGEDATA INT8U*)coolinfo->CoolCustom, 0, DEFAULT_INGREDIENT_TEXT_ALL_SIZE);
	}
	else // Adapt Defualt phrase
	{
		coolinfo->CoolCustom[0] = 0;
	}
}
#endif

INT16U prt_get_plu_barid(INT32U plu_no, INT32U plu_addr, INT8U plu_type, INT8U no)	// Modified by CJK 20050503
{
	INT16U barid;
	INT32U pnt;
	INT8U  gs;
	INT16S    sz;

	barid = 0;
	pnt = DFLASH_BASE;
	pnt+= FLASH_GLOBAL_AREA;
	pnt+= GLOBAL_BARCODE_PRIORITY;
	gs=Flash_bread(pnt);

	switch(plu_no) {
		case MISC_PLU_WEIGHT :	pnt = GLOBAL_BARCODE_NON_WID;	break;
		case MISC_PLU_COUNT  :	pnt = GLOBAL_BARCODE_NON_CID;	break;
		case MISC_PLU_PCS    : 	pnt = GLOBAL_BARCODE_NON_NID;	break;
		case 0:			pnt = GLOBAL_BARCODE_STD_WID;	break;
		default :
			if(no == 2) {	// 2nd Barcode	// Modified by CJK 20050503
#ifndef USE_TOPMART_DEFAULT
				plu_get_field(plu_addr-1,PTYPE_AUX_BARCODE_NO,(INT8U *)&barid,&sz,sizeof(barid));
#endif
				if(barid != 0) return barid; //barid = 0xff;	// 2nd barcode가 0일때 출력안함
			} else {	// 1st Barcode
				if ((gs&0x01) == 0) {	// PLU우선일 경우 (Global Barcode 적용하지 않을 경우)
					plu_get_field(plu_addr-1,PTYPE_BARCODE_NO,(INT8U *)&barid,&sz,sizeof(barid));
					if(barid != 0) return barid;
				}
			}
			if (no == 2) 
			{
				pnt=GLOBAL_BARCODE_2; 
				break; 
			}
			if(Operation.fixedPrice) {
				pnt=GLOBAL_BARCODE_STD_FIX; 
			} else {
				switch(plu_type) {
					case PLU_BY_WEIGHT: pnt=GLOBAL_BARCODE_STD_WID; break;
					case PLU_BY_COUNT : pnt=GLOBAL_BARCODE_STD_CID; break;
					case PLU_BY_PCS   : pnt=GLOBAL_BARCODE_STD_NID; break;
					default: pnt=GLOBAL_BARCODE_STD_WID; break;
				}
			}
			break;
	}
	barid=get_global_bparam(pnt);	// global setting이거나, PLU Barcode가 0일때
	if (no == 2)
	{
		if (barid == 0) return 0xff;	// 인쇄안함
	}
	if (barid > 99) barid = 0;

	return barid;
}

INT8U prt_load_labelform(INT8U mode, INT8U linked)
{
	INT16U labelid;

	if(mode == ITEM_LABEL || mode == TEST_LABEL) 
	{
		if(linked) 
		{
			labelid = Get2ndLabelId(PrtItemStr.l_DeptNo, PrtItemStr.l_PluNo);
			if(!labelid) return FALSE;	// 인쇄 안함
		} 
		else 
		{
			labelid = Get1stLabelId(PrtItemStr.l_DeptNo, PrtItemStr.l_PluNo);
#ifdef USE_DISCOUNT_MEMBERSHIP
			// 보관/세트라벨일 경우 보관/세트 라벨 발행
			if (PrtItemStr.disc_type && (labelid != MEAT_BACKLABEL_ID && labelid != IMPORT_MEAT_BACKLABEL_ID && labelid != MEAT_SETLABEL_ID))
			{
				labelid = get_global_bparam(GLOBAL_DISCOUNT_LABEL);
			}
#endif
#ifdef USE_LOTTEMART_DEFAULT
			if(OnlineFlag)//할인 라벨 보다 온라인 라벨 우선
			{
				labelid = get_global_bparam(GLOBAL_SALE_ONLINE_LABEL);
			}
#endif
#ifdef USE_SETPRODUCT_SALE
			if(setproduct_PrtFlag)
				labelid = get_global_wparam(GLOBAL_LABEL_ADDUP);
#endif
#ifdef USE_HYUNDAI_SET_FUNCTION
			if (Operation.saleStatus == PRINT_SET_COMPONENTS_CUSTOMER)
				labelid = get_global_wparam(GLOBAL_SALE_SET_LABEL);
#endif
			if(!labelid) return FALSE;	// 인쇄 안함
			if (LabelNumber)
			{
				labelid = LabelNumber;
#ifndef USE_EMART_BACK_LABEL	//prepack 사용시 해제 되면 안됨. 
				LabelNumber = 0;
#endif
			}
		}
	} 
	else 
	{	// TOTAL_LABEL
		labelid = get_global_wparam(GLOBAL_LABEL_ADDUP);
	}

#ifdef USE_ONLY_1EA_PRT_RENDERING_BUF
    label_load(labelid);
#else    
	if (status_scale.cur_labelid!=labelid) {
		label_load(labelid);
	}
#endif

	return TRUE;
}

void prt_get_plu_parameter(INT32U plu_addr, STRUCT_STRFORM_PARAM *param)// Plu에 종속된 항
{ 
	INT16U sz;
	INT32U addr;

	switch(param->PluNumber) {
		case MISC_PLU_WEIGHT :
		case MISC_PLU_PCS    :
		case MISC_PLU_COUNT  :
		case 0:
			param->Text[0] = 0;
			param->Prefix[0] = 0;
			break;
		default :
			// GroupNo
			plu_get_field((INT16S)plu_addr-1, PTYPE_GROUP_NO, (INT8U *)&(param->GroupNo), (INT16S *)&sz,sizeof(param->GroupNo));
			// ItemCode
			plu_get_field((INT16S)plu_addr-1, PTYPE_ITEM_CODE, (INT8U *)&(param->ItemCode), (INT16S *)&sz, sizeof(param->ItemCode)); 
#ifdef USE_INDIA_FUNCTION
			if (UseItemCode13Digit)
			{
				plu_get_field((INT16S)plu_addr-1, PTYPE_EXT_BARCODE, (INT8U *)(param->ItemCode13), (INT16S *)&sz, sizeof(param->ItemCode13)); 
				param->ItemCode = 0;
			}
#endif

			// Text
			if (plu_base_change)
			{
				addr = plu_get_field_point_nvram(plu_addr-1, BAR_PARAM_TEXT_FIELD, (INT16S*)&sz);
				memcpy(param->Text, &NvramTempBuffer[0][addr], BAR_PARAM_TEXT_LEN);
			}
			else
			{
				addr = plu_get_field_point(plu_addr-1, BAR_PARAM_TEXT_FIELD, (INT16S*)&sz);
				if(addr > DFLASH_BASE && Flash_check_nvbuffer_4_rd(addr, BAR_PARAM_TEXT_LEN) == 1) {
					addr = (INT32U)(FlashNVRAMStruct.buffer + (addr&FLASH_BLOCK_LADDR_MASK));
					memcpy(param->Text, (INT8U *)addr, BAR_PARAM_TEXT_LEN);	// Size 제한 29bytes
				}
				else
				{
					Flash_sread(addr, param->Text, BAR_PARAM_TEXT_LEN);
				}
			}

#ifndef USE_INVENTORY_LABEL
#ifdef USE_TRACE_STANDALONE
			CurTraceStatus.flagBarcodePrint = ON;
			memset((char *)param->Text,0x00,BAR_PARAM_TEXT_LEN);
			memcpy(param->Text, CurTraceStatus.indivStr.individualNO, INDIVIDUAL_NO_LEN); 
			// NH-mart일 때만, 바코드 인쇄안함
			if(ModeIndiv == 2 && !CurTraceStatus.indivStr.individualNO[0]) CurTraceStatus.flagBarcodePrint = OFF;
			sz = strlen((char*)param->Text);
#endif
#endif
#ifdef USE_KOR_TRACE
			KorTrace_Flag.flagDrawBarcode = ON;
//			if(network_status.service_bflag2 && KorTrace_Flag.flagPrint){
			if(KorTrace_Flag.flagPrint){	//SG060517
				//memset((char *)param->Text,0x00,27);
				memset((char *)param->Text,0x00,BAR_PARAM_TEXT_LEN);	//SG060515	  목
				memcpy(param->Text, (INT8U *)KorTrace_Flag.barcode_data,sizeof(KorTrace_Flag.barcode_data));  //SG060331
				if(!KorTrace_Flag.barcode_data[0]) KorTrace_Flag.flagDrawBarcode = OFF;
			} else { //SG060518  when printing manually
				if( !param->Text[0] )	KorTrace_Flag.flagDrawBarcode = OFF;
			}
			sz = strlen((char*)param->Text);
#endif
			if(sz >= BAR_PARAM_TEXT_LEN) sz = BAR_PARAM_TEXT_LEN - 1;   //SG060515
			param->Text[sz] = 0;
			// Prefix
			if (Startup.country==COUNTRY_KR) {
				plu_get_field(plu_addr-1, PTYPE_PLU_PREFIX, (INT8U *)&(param->iPrefix), (INT16S *)&sz, sizeof(param->iPrefix));
				if (param->iPrefix==0) param->iPrefix=get_net_wparam(NETWORK_DIVISION_ID);
			} else {
				plu_get_field(plu_addr-1, PTYPE_PLU_PREFIX, (INT8U *)param->Prefix, (INT16S *)&sz, sizeof(param->Prefix));
				if(sz > 9) sz = 9;
				param->Prefix[sz] = 0;
			}
#if defined (USE_RUSSIA_LOGISTICS_PLU) || defined (USE_EUR_LOGISTICS_PLU)
			// GTIN code
			plu_get_field((INT16S)plu_addr-1, PTYPE_GTIN_DATA, (INT8U *)&(param->Gtin_code), (INT16S *)&sz, sizeof(param->Gtin_code));
			// Containd_unit
			plu_get_field((INT16S)plu_addr-1, PTYPE_CONTAINED_UNIT, (INT8U *)&(param->Contained_unit), (INT16S *)&sz, sizeof(param->Contained_unit));
			// LOT number
			plu_get_field((INT16S)plu_addr-1, PTYPE_LOT_NUMBER, (INT8U *)&(param->Lot_number), (INT16S *)&sz, sizeof(param->Lot_number));
			// Containd_unit
			plu_get_field((INT16S)plu_addr-1, PTYPE_SERIAL_NUMBER, (INT8U *)&(param->Serial_number), (INT16S *)&sz, sizeof(param->Serial_number));
#endif //  USE_RUSSIA_LOGISTICS_PLU
			break;
	}
}

void prt_set_strform_parameter_item(STRUCT_STRFORM_PARAM *param, TRANSACT_CHAIN  *cp)
{
	INT32U plu_addr;
	long tprice;

	memset((INT8U *)param, 0, sizeof(STRUCT_STRFORM_PARAM));

	if(cp) {	// Ticket
		param->DeptId     = cp->content.trtDept;
		param->PluNumber  = cp->content.trtPLU;
		param->TotalPrice = cp->content.trtVolume;
		param->ScaleId = status_scale.scaleid;
		if(cp->delete) {
			param->TotalPrice = 0;
		}
		param->Quantity   = cp->content.trtQty;
		param->UnitPrice  = cp->content.trtUnitPrice;
		//if(status_scale.Plu.ptype != PLU_BY_WEIGHT && shared)
		//add Weight Barcode Setting print by son 070119	-> para 543
		//파라미터 543 설정시
		// 0 :  무게바코드 -> 무게만큼 인쇄
		// 1 :  PLU타입이 COUNT 일때  무게바코드 ->갯수 인쇄
		// 2 :  정가 판매시나 용기판매일때 무게바크드 -> "1"인쇄
		// 3 :  1번과 2번을 같이 사용함.
		//  KOR 일때 3 ,  default = 0
		if(cp->content.trtPLUtype != PLU_BY_WEIGHT)	// MODE 1 :  IF PLU(count type) then BARCODE "W"->"Q"
		{ 
			if((get_global_bparam(GLOBAL_BARCODE_CUT)>>1)&0x01)
				param->Weight = cp->content.trtQty;
			else
				param->Weight = cp->content.trtWeight;
		}
		else						// MODE 2 :  IF Fixedprice then BARCODE "W"->"1"	
		{
			if(((get_global_bparam(GLOBAL_BARCODE_CUT)>>2)&0x01) && Operation.fixedPrice)
				param->Weight = 1;
			else
				param->Weight = cp->content.trtWeight;
		}
  		param->LabelTranNo= 0;
		param->PluTrCnt   = 0;
		param->ClerkNo    = status_scale.clerkid;
//		plu_addr = plu_table_search(param->DeptId, param->PluNumber);
		param->CountryNo  = 0;
		plu_table_search(param->DeptId, param->PluNumber,(INT16U *)&plu_addr,0);
	} else {	// Label
		param->DeptId     = PrtItemStr.l_DeptNo;
		param->PluNumber  = PrtItemStr.l_PluNo;
		
		//param->TotalPrice = PrtItemStr.TotalPrice;//Price.TotalPrice[PRICE_RESULT];
		//SGTEST.SG071113 ----------------------
		//tprice = (long)PrtItemStr.TotalPrice;
#ifdef USE_TAX_RATE
		if(GlbOper.priceAddExTax && PrtItemStr.l_Price.ExTax && PrtBarcodePriceAddExTaxFlag)
			tprice = (long)PrtItemStr.l_Price.TotalPrice[PRICE_RESULT]+PrtItemStr.l_Price.TaxPrice;
		else
#endif
		tprice = (long)PrtItemStr.l_Price.TotalPrice[PRICE_RESULT];	//Modified by JJANG 20071228
		if ((get_global_bparam(GLOBAL_BARCODE_CUT)>>5)&0x01)	//Added by JJANG 20080620	
		{
			if (PrtItemStr.l_Price.TotalPrice[PRICE_SAVED])
			{
				tprice = (long)PrtItemStr.l_Price.TotalPrice[PRICE_ORIGIN];
			}
		}
		if(tprice<0) tprice = -tprice;
		param->TotalPrice = (INT32U)tprice;
		//--------------------------------------
		param->Quantity   = (INT32U)PrtItemStr.l_Price.Qty[PRICE_RESULT];
		if(PrtItemStr.l_Price.Qty[PRICE_RESULT]<0) param->Quantity = (INT32U)(-PrtItemStr.l_Price.Qty[PRICE_RESULT]);

		param->UnitPrice  = PrtItemStr.l_Price.UnitPrice[PRICE_RESULT];
		if(Operation.fixedPrice)
		{
			param->UnitPrice = 0;
		}
		//add Weight Barcode Setting print by son 070119	-> para 543
		//위의 티켓이랑 동일하게 적용됨
		if(PrtItemStr.l_PluType != PLU_BY_WEIGHT)	// MODE 1 :  IF PLU(count type) then BARCODE "W"->"Q"
		{
			if((get_global_bparam(GLOBAL_BARCODE_CUT)>>1)&0x01)
				param->Weight = PrtItemStr.l_Price.Qty[PRICE_RESULT];
			else
				param->Weight = PrtItemStr.l_Price.Weight[PRICE_RESULT];
		}
		else						// MODE 2 :  IF Fixedprice then BARCODE "W"->"1"	
		{
			if(((get_global_bparam(GLOBAL_BARCODE_CUT)>>2)&0x01) && Operation.fixedPrice)
				param->Weight = 1;
			else
				param->Weight = PrtItemStr.l_Price.Weight[PRICE_RESULT];
		}	
		param->LabelTranNo= PrtItemStr.tran_no;// + 1;
		param->PluTrCnt   = PrtItemStr.plu_tran_no + 1;
		param->ClerkNo    = PrtComStr.clerk_no;
		param->CountryNo  = PrtItemStr.origin;
		param->trace_no	  	  = PrtItemStr.trace_no;
		param->slaught_no	  = PrtItemStr.slaught_no;
		param->cutting_no	  = PrtItemStr.cutting_no;
		param->born_country_no	  = PrtItemStr.born_country_no;
		param->bred_country_no	  = PrtItemStr.bred_country_no;
		param->TareWeight = PrtItemStr.l_TareWeight;		//Added by JJANG 20071212
		plu_addr = PrtItemStr.plu_addr;
		param->ScaleId = status_scale.scaleid;
#ifdef USE_PERCENT_WEIGHT
		param->PercentWeight = PrtItemStr.l_Price.PercentTareWeight;
#endif

#ifdef USE_GSMART_BARCODE
		param->DiscountFlag = status_scale.discountflag;
  #ifdef USE_BESTCO_DISCOUNT_DIVISION
		if (status_scale.percent_discount_flag == 1) param->DiscountVolume = (INT32U)status_scale.discount_rate;
  #else /* USE_BESTCO_DISCOUNT_DIVISION */
		if (status_scale.discountflag == 2) param->DiscountVolume = (INT32U)status_scale.discount_rate;
  #endif /* USE_BESTCO_DISCOUNT_DIVISION*/
		else param->DiscountVolume	= PrtItemStr.l_Price.TotalPrice[PRICE_SAVED]; 
#endif

#ifdef USE_TRACE_STANDALONE
		param->MeatUseNo = CurTraceStatus.meatUse;
	#ifdef USE_TRACE_MEATPART
		param->MeatPartNo = CurTraceStatus.meatPart;
	#endif        
	#ifdef USE_TRACE_MEATGRADE
	  #ifdef USE_TRACE_SCANNER_FOR_GRADE
		param->MeatGradeNo = CurTraceStatus.gradeNo;
	  #endif
	#endif
	#ifdef USE_KOR_SLAUGHT_CODE_4DIGIT
		param->SlaughtHouseNo = trace_slaughthouse_index2no(CurTraceStatus.slPlace);
	#else
		param->SlaughtHouseNo = CurTraceStatus.slPlace;
	#endif
#endif
#ifdef USE_HYUNDAI_CHECK_REASON_CODE
		param->ReasonCode = status_scale.Plu.sales_msg;
#endif
		if(CurrencyPrice.CurrencyUseDefault)
		{
			param->SecondCurrency = PrtItemStr.CPrice.CurrencyPriceH8 * power10(8) + PrtItemStr.CPrice.CurrencyPriceL8;
		}
		else
		{
			param->SecondCurrency = 0;
		}
 	}
 
	prt_get_plu_parameter(plu_addr, param);
}

void prt_set_strform_parameter_total(STRUCT_STRFORM_PARAM *param)
{
	INT32U plu_addr;
	RTC_STRUCT time_str;

	memset((INT8U *)param, 0, sizeof(STRUCT_STRFORM_PARAM));

	if(OneItemPrepack) 
	{
		param->DeptId     = PrtItemStr.l_DeptNo;
		param->PluNumber  = PrtItemStr.l_PluNo;
		param->CountryNo  = PrtItemStr.origin;
		param->trace_no	  	  = PrtItemStr.trace_no;
		param->slaught_no	  = PrtItemStr.slaught_no;
		param->cutting_no	  = PrtItemStr.cutting_no;
		param->born_country_no	  = PrtItemStr.born_country_no;
		param->bred_country_no	  = PrtItemStr.bred_country_no;
		plu_addr = PrtItemStr.plu_addr;

		prt_get_plu_parameter(plu_addr, param);
	}
	else
	{
		param->DeptId     = status_scale.Plu.deptid;
	}
	param->Quantity   = PrtTotalStr.TotalQuantity;
	param->Weight     = PrtTotalStr.TotalWeight;
	param->TicketNo   = PrtTotalStr.TicketTransactNumber;
	param->TotalPrice = PrtTotalStr.SummaryTotalPrice;
	param->LabelTranNo= PrtTotalStr.TotalCount;
	param->ClerkNo    = PrtComStr.clerk_no;
	param->ScaleId = status_scale.scaleid;

	Report_GetCurTime(&time_str,1);
	PrtComStr.time.year  = time_str.year;
	PrtComStr.time.month = time_str.month;
	PrtComStr.time.date  = time_str.date;
}

INT8U prt_make_strform_sub(char *bar_str, char *bar_form, STRUCT_STRFORM_PARAM *param, INT8U scan_flag, INT8U bar_type)
{
	INT32U temp32;
	INT8U i, j, k, m;
	INT8U cnt, q_cnt, temp_cnt, str_pnt, p_v, w_v, q_v, v_index, v_index_w, v_index_q, q_flag;
	char  format[10];
	INT8U trace_bar_alone, x;
#ifdef USE_LOTTEMART_BARCODE_FUNCTION
	INT8U checksum;
#endif
	INT8U ret,emart;
#ifdef USE_TRACEABILITY
	INT32U addr;
#endif
	INT32U power;
#ifdef USE_PRICECHECK_8DIGIT
	char tmpPriceChecksum[10];
#endif
	char string_buf[50];
	char plucode_buf[10];
	INT16U v16_1;
	INT16U v16_2, v16_3, v16_4, v16_5;
#ifdef USE_DIGIT4_PRODUCT_SYMBOL
	INT16U tmpv16_3, tmpv16_4, tmpdate; //20150423
#endif
	INT16U t16_3, t16_4, t16_5;
	INT8U v8_1, v8_2, v8_3;
	INT8U date_form;		//  Yxxx xxxx  :  Y = Year format
					//  xMxx xxxx  :  M = month format
					//  xxDD xxxx	: DD = Date format
					//  xxxx xTTTT :  T = Time format

	INT8U date_type;
	INT8U sellbydate_check;
#ifdef USE_JULIAN_DATE
	INT8U sellbydate_type;
	INT16U days;
#endif
	INT8U time_print_type;
	INT8U org_plucode;
	INT16S uz;
#if defined (USE_BARCODE_CODE128X) || defined (USE_LOGISTICS_BARCODE)
	INT8U weight_decimal_point,valuePram;
	INT32U weightValue,temp;
#endif
#ifdef USE_PERSIAN_CALENDAR
	INT16U perCentury;
#endif
#ifdef USE_LOGISTICS_BARCODE
	INT8U use_idbracket;
	INT8U aiflag;
	INT16U ainumber;
	INT8U netweight_decimalpoint;
#endif
#ifdef USE_EMART_PIG_TRACE
	INT8S z;
	INT8U length;
	INT8U count;
	INT8U countno;

	length = 0;
	countno = 0;
#elif defined(USE_GSMART_PIG_TRACE) || defined(USE_TRACE_PIG_STANDALONE) || defined(USE_LOTTEMART_PIG_TRACE)\
   || defined(USE_NHMART_PIG_TRACE) || defined(USE_NHSAFE_PIG_TRACE) || defined(USE_TRACE_STANDALONE)
	INT8S z;
#endif
 
//--[bmlim060330
	cnt    =0;
	q_cnt = 0;
	q_flag = 0; // quantity barcode flag
	str_pnt=0;
	v_index = 0xff;		// price check digit position
	v_index_w = 0xff;	// weight check digit position
	v_index_q = 0xff;	// quantity check digit position
	p_v = 0;		// price digit number
	w_v = 0;		// weight digit number
	q_v = 0;		// quantity digit number
	ret = ON;

#ifdef USE_LOGISTICS_BARCODE
	use_idbracket = get_global_bparam(GLOBAL_SALE_SETUP_FLAG19);
	use_idbracket = (use_idbracket>>1)& 0x01;	//param 526, date format
#endif
    org_plucode = get_global_bparam(GLOBAL_BARCODE_CUT);
    org_plucode = (org_plucode>>6)&0x01;
	date_form = get_global_bparam(GLOBAL_DATETIME_MODE);	//Added by JJANG 20071211
	date_type = ((date_form>>4)& 0x03);		//param 530, date format
	time_print_type  = get_global_bparam(GLOBAL_SALE_SETUP_FLAG6); 	//param 547, printing format for time
	time_print_type>>=5;
	time_print_type&=0x03;
	sellbydate_check= get_global_bparam(GLOBAL_SALE_SETUP_FLAG5)&0x01;		//param 555, 가공시간, 유효시간의존
#ifdef USE_JULIAN_DATE
	sellbydate_type = (get_global_bparam(GLOBAL_SALE_SETUP_FLAG17)>>1)&0x03;	//pram 529 sellby date 인쇄시 date type 설정(일반, Juliandate format 1,2,3)
#endif
	if (!scan_flag)
	{
#ifdef USE_INDIA_FUNCTION
		if (UseItemCode13Digit) goto NEXT;
#endif
#if defined(USE_PRINT_TEMPERATURE) || defined(USE_LOTTEMART_ITEMCODE13)
				goto NEXT;
#endif
		if (!plu_check_inhibit_ptype(PTYPE_EXT_BARCODE)) { //EXT. BARCODE
			if (PrtItemStr.plu_addr) {
				plu_get_field(PrtItemStr.plu_addr-1,PTYPE_EXT_BARCODE,(INT8U *)string_buf,&uz,20);
				string_buf[20]=0;
				str_trimleft(string_buf);
				strcpy(bar_str,string_buf);
				str_pnt=strlen(bar_str);
				ret=2;
				if (str_pnt==0) goto NEXT;
				if (str_pnt==13){
					i=bar_str[12];
					bar_str[12]=0;
					k=BarUpc_Checksum((HUGEDATA char *)bar_str,12);
					if (k!=i) {
						ret=OFF;
					}
					goto SKIP;
				} else if (str_pnt==12) {
					bar_str[12]=0;
					k=BarUpc_Checksum((HUGEDATA char *)bar_str,12);
					bar_str[12]=k;
					goto SKIP;
				} else {
					ret=OFF;
					goto SKIP;
				}

			}
		}
	}
//--[end
NEXT:
	//memset(param,0,sizeof(STRUCT_STRFORM_PARAM));
	ret=ON;
	emart=get_global_bparam(GLOBAL_SALE_SETUP_FLAG3);
	emart&=0x10;

#ifdef USE_EMART_PIG_TRACE
	for(i=0; i<BARCODE_FORMAT_LEN+2; i++) {		// Emart 수입육 바코드 체계의 경우 32글자여서 마지막 2글자 (용도심볼) 이 잘리는 현상 개선
#else
	for(i=0; i<BARCODE_FORMAT_LEN; i++) {
#endif
		cnt++;
		if(bar_form[i]==0) break;
#ifdef USE_LOGISTICS_BARCODE
		else if((bar_form[i]>='0') && (bar_form[i]<='9')) 
		{
			if(aiflag && use_idbracket)
			{
				ainumber *= 10;
				ainumber += bar_form[i] - '0';
			}
			bar_str[str_pnt]=bar_form[i];
			str_pnt++;
			cnt = 0;
		}
		else if(bar_form[i]==BARCODE_SYMBOL_AI1) {
			bar_str[str_pnt]=bar_form[i];
			str_pnt++;
			cnt = 0;
			aiflag = 1;
			ainumber = 0;
		}
		else if(bar_form[i]==BARCODE_SYMBOL_AI2) {
			if(use_idbracket){
				if(ainumber == AI_NET_WEIGHT_1 || ainumber == AI_NET_WEIGHT_2 || ainumber == AI_NET_WEIGHT_3) // weight
				{
  #if defined (USE_RUSSIA_LOGISTICS_PLU) || defined (USE_EUR_LOGISTICS_PLU)
  					plu_get_field((INT16S)PrtItemStr.plu_addr-1, PTYPE_NETWEIGHT_DECIMAL,(INT8U *)&netweight_decimalpoint,&uz,1);
  #else
					netweight_decimalpoint = get_global_bparam(GLOBAL_DISPLAY_WEIGHTDECIMAL);
  #endif
					//plu_get_field(PrtItemStr.plu_addr-1, PTYPE_NETWEIGHT_DECIMAL,&netweight_decimalpoint,&uz,1);
					switch(netweight_decimalpoint)
					{
						case 1 :
							bar_str[(str_pnt-1)] = '1';
							break;

						case 2 :
							bar_str[(str_pnt-1)] = '2';
							break;
						default :
						case 3 :
							bar_str[(str_pnt-1)] = '3';
							break;
					}
				}
			}
			bar_str[str_pnt]=bar_form[i];
			str_pnt++;
			cnt = 0;
			aiflag = 0;
		}
#else		
		else if((bar_form[i]>='0') && (bar_form[i]<='9')) {
			if ((bar_form[i] == '0') && (q_flag == 1))
				q_cnt++;
				bar_str[str_pnt]=bar_form[i];
				str_pnt++;
		    	cnt = 0;
		}
#endif
		else if(bar_form[i] != bar_form[i+1] || bar_form[i] == '%') {
			sprintf(format,"%c0%dld",'%',cnt); // %06d
			temp_cnt = cnt;
// ABCDEFGHIJKLMNOPQRSTUVWXYZ
//             OO          
			if(cnt > 9) temp_cnt = 9;
			power=power10(temp_cnt);
			if(scan_flag) {
				switch(bar_form[i]) {
					case SCAN_SYMBOL_ITEMCODE:
#ifdef USE_INDIA_FUNCTION
						if (UseItemCode13Digit)
						{
							for(j = 0; j < cnt && j < sizeof(param->ItemCode13); j++)
							{
								param->ItemCode13[j] = bar_str[str_pnt+j]; 
							}
							param->ItemCode13[j] = 0;
						}
						else
						{
							sscanf(&bar_str[str_pnt],format,&(param->ItemCode));
						}
#else
						sscanf(&bar_str[str_pnt],format,&(param->ItemCode));
#endif
						param->Scan_flag = param->Scan_flag | BAR_SCANFLAG_ITEMCODE;	//0x01;
						break;
					case SCAN_SYMBOL_WEIGHT:
						sscanf(&bar_str[str_pnt],format,&(param->Weight));
						param->Scan_flag = param->Scan_flag | BAR_SCANFLAG_WEIGHT;	//0x02;
						break;
					case SCAN_SYMBOL_TARE_WEIGHT:
						sscanf(&bar_str[str_pnt],format,&(param->TareWeight));
						param->Scan_flag = param->Scan_flag | BAR_SCANFLAG_TARE_WEIGHT;	//0x2000;
						break;
					case SCAN_SYMBOL_DEPTNO:
						sscanf(&bar_str[str_pnt],format,&temp32);	
						param->DeptId = (INT16U)temp32;
						param->Scan_flag = param->Scan_flag | BAR_SCANFLAG_DEPTNO;	//0x04;
						break;
					case SCAN_SYMBOL_PLUNO:
						sscanf(&bar_str[str_pnt],format,&(param->PluNumber));
						param->Scan_flag = param->Scan_flag | BAR_SCANFLAG_PLUNO;	//0x08;
						break;
					case SCAN_SYMBOL_FIXED_PRICE:
						sscanf(&bar_str[str_pnt],format,&(param->TotalPrice));
						param->Scan_flag = param->Scan_flag | BAR_SCANFLAG_FIXED_PRICE;	//0x800
						break;
#ifdef USE_SCANNER_FOR_ECO_FRIENDLY
					case SCAN_SYMBOL_CODE_880:
						for(j = 0; j < cnt && j < sizeof(param->code880); j++)
						{
							param->code880[j] = bar_str[str_pnt+j]; 
						}
						param->Scan_flag = param->Scan_flag | BAR_SCANFLAG_CODE_880;	//0x1000;
						break;
#endif

#ifdef USE_TRACE_STANDALONE
#ifdef USE_LOTTEMART_MEATSCANNER
					case SCAN_SYMBOL_MEATUSE:
						sscanf(&bar_str[str_pnt],format,&temp32);
						param->Meat_use_no = (INT8U)temp32;
						param->Scan_flag = param->Scan_flag | BAR_SCANFLAG_MEATUSE;	//0x10;
						break;
					case SCAN_SYMBOL_SL_NO:
						sscanf(&bar_str[str_pnt],format,&temp32);
						param->SlaughtHouseNo1 = (INT16U)temp32%100;
						param->Scan_flag = param->Scan_flag | BAR_SCANFLAG_SL_NO;	//0x20;
						break;
					case SCAN_SYMBOL_SL_NO2:
						sscanf(&bar_str[str_pnt],format,&temp32);
						param->SlaughtHouseNo2 = (INT16U)temp32%100;
						param->Scan_flag = param->Scan_flag | BAR_SCANFLAG_SL_NO2;	//0x1000;
						break;
					case SCAN_SYMBOL_SL_NO3:
						sscanf(&bar_str[str_pnt],format,&temp32);
						param->SlaughtHouseNo3 = (INT16U)temp32%100;
						param->Scan_flag = param->Scan_flag | BAR_SCANFLAG_SL_NO3;	//0x8000;
						break;
#else
					case SCAN_SYMBOL_MEATUSE:
						sscanf(&bar_str[str_pnt],format,&temp32);
						param->MeatUseNo = (INT8U)temp32;
						param->Scan_flag = param->Scan_flag | BAR_SCANFLAG_MEATUSE;	//0x10;
						break;
					case SCAN_SYMBOL_SL_NO:
						sscanf(&bar_str[str_pnt],format,&temp32);
						param->SlaughtHouseNo = (INT16U)temp32;
						param->Scan_flag = param->Scan_flag | BAR_SCANFLAG_SL_NO;	//0x20;
						break;
#endif
					case SCAN_SYMBOL_ORIGIN:
						sscanf(&bar_str[str_pnt],format,&temp32);	
						param->CountryNo = (INT16U)temp32;
						param->Scan_flag = param->Scan_flag | BAR_SCANFLAG_ORIGIN;	//0x400;
						break;
#ifdef USE_TRACE_MEATPART
					case SCAN_SYMBOL_MEATPART:
						sscanf(&bar_str[str_pnt],format,&temp32);
						param->MeatPartNo = (INT8U)temp32;
						param->Scan_flag = param->Scan_flag | BAR_SCANFLAG_MEATPART;	//0x100;
						break;
#endif
#ifdef USE_LOTTEMART_ITEMCODE13
					case SCAN_SYMBOL_ITEMCODE13:
						for(j = 0; j < cnt && j < sizeof(param->ItemCode13); j++)
						{
							param->ItemCode13[j] = bar_str[str_pnt+j];
						}
						param->ItemCode13[j] = 0;
						param->Scan_flag = param->Scan_flag | BAR_SCANFLAG_ITEMCODE13;	//0x4000;
					break;
#endif
#ifdef USE_LOTTEMART_MEATSCANNER
					case SCAN_SYMBOL_MEATGRADE:
						sscanf(&bar_str[str_pnt],format,&temp32);
						param->Meat_grade_no= (INT8U)temp32;
						param->Scan_flag = param->Scan_flag | BAR_SCANFLAG_MEATGRADE;	//0x200;
						break;
#else
  #ifdef USE_TRACE_MEATGRADE
    #ifdef USE_TRACE_SCANNER_FOR_GRADE
					case SCAN_SYMBOL_MEATGRADE:
						sscanf(&bar_str[str_pnt],format,&temp32);
						param->MeatGradeNo = (INT8U)temp32;
						param->Scan_flag = param->Scan_flag | BAR_SCANFLAG_MEATGRADE;	//0x200;
						break;
    #endif	// USE_TRACE_SCANNER_FOR_GRADE
  #endif //USE_TRACE_MEATGRADE
#endif //USE_LOTTEMART_MEATSCANNER
#endif //#ifdef USE_TRACE_STANDALONE
					case SCAN_SYMBOL_REMNANT:
						for(j = 0; j < cnt && j < sizeof(param->RemnantData); j++) {
							param->RemnantData[j] = bar_str[str_pnt+j]; 
						}
						param->RemnantData[j] = 0;
						param->Scan_flag = param->Scan_flag | BAR_SCANFLAG_REMNANT;	//0x80;
						break;
#ifdef USE_LOTTEMART_MEATSCANNER
					case SCAN_SYMBOL_TEXT:
						for(j = 0; j < cnt && j < sizeof(param->Text); j++)
						{
							param->Indiv_no[j] = bar_str[str_pnt+j];
						}
						if(strlen((char*)param->Indiv_no) == 24)
						{
							param->No14_indiv_flag = 0;
							if ((param->Indiv_no[0]>='0') && (param->Indiv_no[0]<='9')) param->Indiv_no[12] = 0;
							else if (param->Indiv_no[0]>='L') param->Indiv_no[15] = 0;
							else if (param->Indiv_no[0]!='A') param->Indiv_no[0] = 0;
							else param->Indiv_no[j] = 0;
						}
						else if (strlen((char*)param->Indiv_no) == 14)
						{
							param->No14_indiv_flag = 1;
							if(strncmp((char *)&param->Indiv_no[0], "99", 2) == 0)
							{
								memcpy(param->Indiv_no, LotStr, 3);

								for(j = 0; j < cnt - 2; j++)
								{
									param->Indiv_no[j + 3] = param->Indiv_no[j + 2];
								}
								param->Indiv_no[j + 3] = 0;
							}
							else if(strncmp((char *)&param->Indiv_no[0], "0099", 4) == 0)  //묶음번호 12자리 판매 라벨 LOT 표시
							{
								memcpy(param->Indiv_no, LotStr, 3);
								for(j = 0; j < cnt-2; j++)
								{
									if(!param->Indiv_no[j]) break;
									param->Indiv_no[j+3]= param->Indiv_no[j+4];
								}
								param->Indiv_no[cnt-1] = '\0';

							}
							else if((strncmp((char *)&param->Indiv_no[0], "00", 2) == 0))
							{
								for(j = 0; j < cnt-2; j++)
								{
									param->Indiv_no[j]= param->Indiv_no[j+2];
								}
								param->Indiv_no[cnt-2] = '\0';
								param->Indiv_no[cnt-1] = '\0';
							}
							else // 신규 보관라벨 표시
							{
								for(z = cnt+1 ; z>=0 ; z--)
								{
									param->Indiv_no[z]= param->Indiv_no[z-1];
								}
								param->Indiv_no[0] = 'L';
							}

						}
						else
						{
							param->No14_indiv_flag = 0;
							param->Indiv_no[j] = 0;
						}
						param->Scan_flag = param->Scan_flag | BAR_SCANFLAG_TEXT;	//0x40;
						break;
#else
					case SCAN_SYMBOL_TEXT:
						for(j = 0; j < cnt && j < sizeof(param->Text); j++)
						{
							param->Text[j] = bar_str[str_pnt+j]; 
						}
						param->Text[j] = 0;
  #ifdef USE_TRACE_STANDALONE
						switch (ModeIndiv){
							default:
							case 0://not used							
								break;
							case 1://신세계
							case 4://standard (신세계 + 개체인덱스)
								if(strncmp((char *)&bar_str[str_pnt], "999", 3) == 0)
								{
									memcpy(param->Text, LotStr, 3);
								}
								else if((strncmp((char *)&bar_str[str_pnt], "00", 2) == 0)  && (cnt == 14))	//이력번호	12자리 판매 라벨  표시
								{
									for(j = 0; j < cnt-2; j++) 
									{
										param->Text[j]= param->Text[j+2];
									}
									param->Text[cnt-2] = '\0';
									param->Text[cnt-1] = '\0';
									if(strncmp((char *)&bar_str[str_pnt+2], "999", 3) == 0)  //묶음번호 12자리 판매 라벨 LOT 표시
									{
										memcpy(param->Text, LotStr, 3);										
									}
								}
								else if(cnt == 14) // 신규 보관라벨 표시 
								{
									for(z = cnt+1 ; z>=0 ; z--) 
									{
										param->Text[z]= param->Text[z-1];
									}
									param->Text[0] = 'L';
								}
								break;
							case 3://GS마트
#ifdef USE_TOPMART_DEFAULT
								if((strncmp((char *)&bar_str[str_pnt], "00", 2) == 0) && (cnt == 16))
								{
									//memcpy(param->Text, LotStr, 3);
									param->Text[0] = 'L';

									for(j = 0; j < cnt - 1; j++) 
									{
										param->Text[j + 1] = bar_str[str_pnt + j + 2]; 
									}
									param->Text[j + 1] = 0;
								}
#else
								if((strncmp((char *)&bar_str[str_pnt], "99", 2) == 0) && cnt != 12)
								{
									memcpy(param->Text, LotStr, 3);

									for(j = 0; j < cnt - 2; j++) 
									{
										param->Text[j + 3] = bar_str[str_pnt + j + 2]; 
									}
									param->Text[j + 3] = 0;
								} 
#endif
//#ifdef USE_GSMART_PIG_TRACE //gspig 20141212
								else if(strncmp((char *)&bar_str[str_pnt], "0099", 4) == 0)  //묶음번호 12자리 판매 라벨 LOT 표시
								{
									memcpy(param->Text, LotStr, 3);
									for(j = 0; j < cnt-2; j++) 
									{
										if(!param->Text[j]) break;
										param->Text[j+3]= param->Text[j+4]; 
									}
									//param->Text[cnt-2] = '\0';
									param->Text[cnt-1] = '\0';

								}
								//else if(((strncmp((char *)&bar_str[str_pnt], "00", 2) == 0) || (((char *)&bar_str[str_pnt], "10", 2) == 0)) && (cnt == 14))  //이력번호  12자리 판매 라벨  표시
								//else if(((strncmp((char *)&bar_str[str_pnt], "00", 2) == 0) || (strncmp((char *)&bar_str[str_pnt], "10", 2) == 0)) && (cnt == 14))  //이력번호  12자리 판매 라벨  표시
								else if(((strncmp((char *)&bar_str[str_pnt], "00", 2) == 0)) && (cnt == 14)) 
								{
									for(j = 0; j < cnt-2; j++) 
									{
										param->Text[j]= param->Text[j+2];
									}
									param->Text[cnt-2] = '\0';
									param->Text[cnt-1] = '\0';
								}
								else if(cnt == 14) // 신규 보관라벨 표시 
								{
									for(z = cnt+1 ; z>=0 ; z--) 
									{
										param->Text[z]= param->Text[z-1];
									}
									param->Text[0] = 'L';
								}
//#endif
								break;

							case 5://이마트
    #ifdef USE_EMART_PIG_TRACE							
								length = strlen(bar_form);
								countno = cnt;
								if(length == 29)
								{
									for(j = 0; j < countno; j++) 
									{
										param->Text[j]= param->Text[j+1]; 
									}
									param->Text[countno-1] = '\0';
									countno = cnt-1;
								}
    #endif //USE_EMART_PIG_TRACE
								if((strncmp((char *)&bar_str[str_pnt], "99", 2) == 0) && cnt != 12)
								{
									memcpy(param->Text, LotStr, 3);
								} 
    #ifdef USE_EMART_PIG_TRACE
								else if(strncmp((char *)&bar_str[str_pnt], "0099", 4) == 0)  //묶음번호 12자리 판매 라벨 LOT 표시
								{
									memcpy(param->Text, LotStr, 3);
									for(j = 0; j < countno-2; j++) 
									{
										if(!param->Text[j]) break;
										//if((param->Text[j] == 'L') && (param->Text[j+1]>='0') && (param->Text[j+1]<='9')) z=1;
										param->Text[j+3]= param->Text[j+5]; 
									}
									param->Text[countno-2] = '\0';
									param->Text[countno-1] = '\0';

								}
								else if((strncmp((char *)&bar_str[str_pnt], "00", 2) == 0) && (length == 28))  //이력번호  12자리 판매 라벨  표시
								{
									for(j = 0; j < countno-2; j++) 
									{
										param->Text[j]= param->Text[j+2];
									}
									param->Text[countno-2] = '\0';
									param->Text[countno-1] = '\0';
								}
								else if(countno == 14) // 신규 보관라벨 표시 
								{
									for(z = countno+1 ; z>=0 ; z--) 
									{
										param->Text[z]= param->Text[z-1];
									}
									param->Text[0] = 'L';
								}
								else if(length == 32)
								{
									param->Text[0] = 'A';
								}
    #endif // USE_EMART_PIG_TRACE
								break;
							case 2://농협
    #ifdef USE_NHMART_SAFE_MEAT
								if(strncmp((char *)&bar_str[str_pnt], "99", 2) == 0)
								{
									memcpy(param->Text, LotStr, 3);

									for(j = 0; j < cnt - 2; j++) 
									{
										param->Text[j + 3] = bar_str[str_pnt + j + 2]; 
									}
									param->Text[j + 3] = 0;
								} 								
    #else //USE_NHMART_SAFE_MEAT                            
								if(cnt == 18) //hanaro 20150116
								{
								    if(strncmp((char *)&bar_str[str_pnt], "999", 3) == 0)
									{
										memcpy(param->Text, LotStr, 3);
									} 								
									else if (strncmp((char *)&bar_str[str_pnt], "888", 3) == 0)
									{
										for(j = 0; j < cnt - 6; j++) 
										{
											param->Text[j] = bar_str[str_pnt + j + 6]; 
										}
										param->Text[j] = 0;
									}
								}
								else //hanaro 20150116
								{
									if(strncmp((char *)&bar_str[str_pnt], "999", 3) == 0)
									{
										memcpy(param->Text, LotStr, 3);
									}
									else if((strncmp((char *)&bar_str[str_pnt], "00", 2) == 0)  && (cnt == 14))	//이력번호	12자리 판매 라벨  표시
									{
										for(j = 0; j < cnt-2; j++) 
										{
											param->Text[j]= param->Text[j+2];
										}
										param->Text[cnt-2] = '\0';
										param->Text[cnt-1] = '\0';
										if(strncmp((char *)&bar_str[str_pnt+2], "999", 3) == 0)  //묶음번호 12자리 판매 라벨 LOT 표시
										{
											memcpy(param->Text, LotStr, 3);										
										}
									}
									else if(cnt == 14) // 신규 보관라벨 표시 
									{
										for(z = cnt+1 ; z>=0 ; z--) 
										{
											param->Text[z]= param->Text[z-1];
										}
										param->Text[0] = 'L';
									}									
								}
    #endif //USE_NHMART_SAFE_MEAT                                 
								break;
						}
  #endif //USE_TRACE_STANDALONE
						param->Scan_flag = param->Scan_flag | BAR_SCANFLAG_TEXT;	//0x40;
						break;
#endif //USE_LOTTEMART_MEATSCANNER
#ifdef USE_HYUNDAI_SET_FUNCTION
					case SCAN_SYMBOL_RECEIPT_NO:
						for(j = 0; j < cnt && j < sizeof(param->ReceiptNo); j++)
						{
							param->ReceiptNo[j] = bar_str[str_pnt+j];
						}
						param->Scan_flag = param->Scan_flag | BAR_SCANFLAG_RECEIPT_NO;	//0x1000;
						break;
#endif
				}
			} else {
				if (bar_form[i] == BARCODE_SYMBOL_QUANTITY)
					q_flag = 1; // 바코드'Q' 뒤의 '0'을 카운트하는 상태
				else if ((q_flag == 1) && (q_cnt > 0))
					q_flag = 2; // 바코드'Q' 뒤에 '0'이 있고, 'e' 전에 다른 바코드가 있는 상태 ('Q'와'e' 사이에 '0'이 있어도 카운트 하지 않는다)
				else
					q_flag = 0; // q_cnt 초기화
				if ((!q_flag) && (bar_form[i] != BARCODE_SYMBOL_QUANTITYCHECKSUM))
					q_cnt = 0;
// Barcode Symbol
			switch(bar_form[i]) {
#if defined (USE_RUSSIA_LOGISTICS_PLU) || defined(USE_EUR_LOGISTICS_PLU)
				case BARCODE_SYMBOL_GTIN:
					// Replace symbol for GTIN
					if(use_idbracket)
					{
						if(ainumber == AI_GTIN_TYPE_01 || ainumber == AI_GTIN_TYPE_02) //	GTIN_01, GTIN_02
						{
							v16_1 = strlen((char*)param->Gtin_code);
							j = 0;
							k = 0;
							if(v16_1 < cnt) {
								for(j = 0; j < cnt - v16_1; j++) bar_str[str_pnt+j] = '0';	// 우측정렬
							}
							for( ; j < cnt; j++) {
								bar_str[str_pnt+j] = param->Gtin_code[k];
								k++;
							}
						}
						else // AI number error
						{
							for(j = 0 ; j < cnt; j++) bar_str[str_pnt+j] = '0';
						}
					}
					else
					{
						v16_1 = strlen((char*)param->Gtin_code);
						j = 0;
						k = 0;
						if(v16_1 < cnt) {
							for(j = 0; j < cnt - v16_1; j++) bar_str[str_pnt+j] = '0';	// 우측정렬
						}
						for( ; j < cnt; j++) {
							bar_str[str_pnt+j] = param->Gtin_code[k];
							k++;
						}
					}
					ainumber = 0;
					break;
				case BARCODE_SYMBOL_CONTAINED_UNIT:
					// Replace symbol for Contained Unit
					if(use_idbracket)
					{
						if(ainumber == AI_CONTAINED_UNIT) //	Contained Unit
						{
							v16_1 = strlen((char*)param->Contained_unit);
							j = 0;
							k = 0;
							if(v16_1 < cnt) {
								for(j = 0; j < cnt - v16_1; j++) bar_str[str_pnt+j] = '0';	// 우측정렬
							}
							for( ; j < cnt; j++) {
								bar_str[str_pnt+j] = param->Contained_unit[k];
								k++;
							}
						}
						else // AI number error
						{
							for(j = 0 ; j < cnt; j++) bar_str[str_pnt+j] = '0';
						}
					}
					else
					{
						v16_1 = strlen((char*)param->Contained_unit);
						j = 0;
						k = 0;
						if(v16_1 < cnt) {
							for(j = 0; j < cnt - v16_1; j++) bar_str[str_pnt+j] = '0';	// 우측정렬
						}
						for( ; j < cnt; j++) {
							bar_str[str_pnt+j] = param->Contained_unit[k];
							k++;
						}
					}
					ainumber = 0;
					break;
				case BARCODE_SYMBOL_LOT_NUMBER:
					// Replace symbol for Lot Number
					if(use_idbracket)
					{
						if(ainumber == AI_LOT_NUMBER) //	Lot Nunber
						{
							v16_1 = strlen((char*)param->Lot_number);
							j = 0;
							k = 0;
							if(v16_1 < cnt) {
								for(j = 0; j < cnt - v16_1; j++) bar_str[str_pnt+j] = '0';	// 우측정렬
							}
							for( ; j < cnt; j++) {
								bar_str[str_pnt+j] = param->Lot_number[k];
								k++;
							}
						}
						else // AI number error
						{
							for(j = 0 ; j < cnt; j++) bar_str[str_pnt+j] = '0';
						}
					}
					else
					{
						v16_1 = strlen((char*)param->Lot_number);
						j = 0;
						k = 0;
						if(v16_1 < cnt) {
							for(j = 0; j < cnt - v16_1; j++) bar_str[str_pnt+j] = '0';	// 우측정렬
						}
						for( ; j < cnt; j++) {
							bar_str[str_pnt+j] = param->Lot_number[k];
							k++;
						}
					}
					ainumber = 0;
					break;
				case BARCODE_SYMBOL_SERIAL_NUMBER:
					// Replace symbol for Serial Number
					if(use_idbracket)
					{
						if(ainumber == AI_SERIAL_NUMBER) // Lot Nunber
						{
							v16_1 = strlen((char*)param->Serial_number);
							j = 0;
							k = 0;
							if(v16_1 < cnt) {
								for(j = 0; j < cnt - v16_1; j++) bar_str[str_pnt+j] = '0';	// 우측정렬
							}
							for( ; j < cnt; j++) {
								bar_str[str_pnt+j] = param->Serial_number[k];
								k++;
							}
						}
						else // AI number error
						{
							for(j = 0 ; j < cnt; j++) bar_str[str_pnt+j] = '0';
						}
					}
					else
					{
						v16_1 = strlen((char*)param->Serial_number);
						j = 0;
						k = 0;
						if(v16_1 < cnt) {
							for(j = 0; j < cnt - v16_1; j++) bar_str[str_pnt+j] = '0';	// 우측정렬
						}
						for( ; j < cnt; j++) {
							bar_str[str_pnt+j] = param->Serial_number[k];
							k++;
						}
					}
					ainumber = 0;
					break;
#endif
				case SCAN_SYMBOL_ITEMCODE:
#ifdef USE_INDIA_FUNCTION
					if (UseItemCode13Digit)
					{
						for(j = 0; j < cnt; j++) {
							if(!param->ItemCode13[j]) break;
							bar_str[str_pnt+j] = param->ItemCode13[j]; 
						}
						for( ; j < cnt; j++) bar_str[str_pnt+j] = '0';
					}
					else
					{
						if(param->ItemCode >= power) {
							if (get_global_bparam(GLOBAL_BARCODE_CUT)&0x01) {
								param->ItemCode %= power;
							} else {
								param->ItemCode = 0L;
								ret = OFF;
							}
						}
						sprintf(&bar_str[str_pnt],format,param->ItemCode);
					}
#else
					if(param->ItemCode >= power) {
						if (get_global_bparam(GLOBAL_BARCODE_CUT)&0x01) {
							param->ItemCode %= power;
						} else {
							param->ItemCode = 0L;
							ret = OFF;
						}
					}
					sprintf(&bar_str[str_pnt],format,param->ItemCode);
#endif
					bar_str[str_pnt+cnt]=0;
					break;
#ifdef USE_LOTTEMART_ITEMCODE13
				case BARCODE_SYMBOL_ITEMCODE13: // "i" 
					plu_get_field(PrtItemStr.plu_addr-1, PTYPE_EXT_BARCODE, (INT8U *)&param->ItemCode13, (INT16S *)&v16_2,sizeof(param->ItemCode13)); //20151013
					param->ItemCode13[v16_2] = 0;
					
					for(j = 0; j < cnt; j++) 
					{
						if(!param->ItemCode13[j]) break;
						bar_str[str_pnt+j] = param->ItemCode13[j]; 
					}
					for( ; j < cnt; j++) bar_str[str_pnt+j] = '0';
	
					break;	
#endif
#ifdef USE_LOTTEMART_BARCODE_FUNCTION
				case BARCODE_SYMBOL_LOTTECHECKSUM: //"x"
					checksum = Lottemart_GS1_Checksum(bar_str, strlen(bar_str));
				
					sprintf(&bar_str[str_pnt],"%c",checksum);
					break;
#endif

				case BARCODE_SYMBOL_PLUNO:
					if(param->PluNumber >= power) {
						param->PluNumber = 0L; 
						ret = OFF;
					}
			        	sprintf(&bar_str[str_pnt],format,param->PluNumber);	
			        	break;
 				case BARCODE_SYMBOL_PRICE:
					p_v=cnt;
					if(param->TotalPrice >= power) {
						param->TotalPrice = 0L;
						ret = OFF;
					}
#if defined(USE_TAX_RATE) && defined(USE_BARCODE_TAX)
					if(GlbOper.priceAddExTax && PrtItemStr.l_Price.ExTax)
						sprintf(&bar_str[str_pnt],format,PrtItemStr.l_Price.TotalPrice[PRICE_RESULT]+PrtItemStr.l_Price.TaxPrice);
					else
#endif
					sprintf(&bar_str[str_pnt],format,param->TotalPrice);  
					if(v_index != 0xff) {	// 'P'가 'V' 뒤에 있는 경우
						if (p_v == 4) bar_str[v_index] = checksum_price_weight1(param->TotalPrice)+'0';
						else if (p_v == 5) bar_str[v_index] = checksum_price_weight2(param->TotalPrice)+'0';
#ifdef USE_PRICECHECK_8DIGIT
						//[start]gm 081010, 6 digit Price CheckSum for Vietnam
						if((get_global_bparam(GLOBAL_BARCODE_CUT)>>3)&0x01) //have to add when vietnam setting enable !!!
						{
						       sprintf(tmpPriceChecksum, format, param->TotalPrice);	
						       bar_str[v_index] = BarUpc_Checksum(tmpPriceChecksum, p_v); 
						}
#endif
					}
					break;

				case BARCODE_SYMBOL_SECONDCURRENCY:
					p_v=cnt;
					if(param->SecondCurrency >= power) {
						param->SecondCurrency = 0L;
						ret = OFF;
					}
					sprintf(&bar_str[str_pnt],format,param->SecondCurrency);  
					if(v_index != 0xff) {	// 'P'가 'V' 뒤에 있는 경우
						if (p_v == 4) bar_str[v_index] = checksum_price_weight1(param->SecondCurrency)+'0';
						else if (p_v == 5) bar_str[v_index] = checksum_price_weight2(param->SecondCurrency)+'0';
					}
					break;
					
				case BARCODE_SYMBOL_PRICECHECKSUM:
					if(p_v == 0) v_index = str_pnt;	// 'P'가 'V' 뒤에 있는 경우
					else if(p_v == 4) bar_str[str_pnt]=checksum_price_weight1(param->TotalPrice)+'0';
					else if(p_v == 5) bar_str[str_pnt]=checksum_price_weight2(param->TotalPrice)+'0';
#ifdef USE_PRICECHECK_8DIGIT
					//[start]gm 081010, 6 digit Price CheckSum for Vietnam
					if((get_global_bparam(GLOBAL_BARCODE_CUT)>>3)&0x01) //have to add when vietnam setting enable !!!
					{
						sprintf(tmpPriceChecksum, "%0dld", p_v, param->TotalPrice);
						bar_str[str_pnt] = BarUpc_Checksum(tmpPriceChecksum, p_v);
					}
#endif
					break;
				case BARCODE_SYMBOL_WEIGHTCHECKSUM:	// weight check digit
					if(w_v == 0) v_index_w = str_pnt;	// 'W'가 'v' 뒤에 있는 경우
					else if(w_v == 4) bar_str[str_pnt]=checksum_price_weight1(param->Weight)+'0';
					else if(w_v == 5) bar_str[str_pnt]=checksum_price_weight2(param->Weight)+'0';
					break;
				case BARCODE_SYMBOL_WEIGHT:
#ifdef USE_EMART_NETWORK
					if(status_scale.divisor)		//이마트 쪼개기인 경우 쪼개기 값 인쇄
					{
						param->Weight = status_scale.divisor;
					}
#endif
#ifdef USE_TOPMART_DEFAULT //sewon 170113
					if (status_scale.Plu.ptype == PLU_BY_COUNT)
					{
						if(InputQtyFlag == ON)
						{
							if(InputQty >= power) {
								InputQty = 0L; 
								ret = OFF;
							}
							sprintf(&bar_str[str_pnt],format,InputQty);

							break;
						}
						else
						{
							if(param->Quantity >= power) {
								param->Quantity = 0L; 
								ret = OFF;
							}
							sprintf(&bar_str[str_pnt],format,param->Quantity); 
							break;
						}
					}
					else
					{
#endif
					w_v=cnt;
//					if(cnt > 9) temp_cnt = 9;
					if(org_plucode)		//Added by JJANG 20080404
					{
						if (PrtItemStr.fixed_price) param->Weight = 0;
					}
#if defined (USE_BARCODE_CODE128X) || defined (USE_LOGISTICS_BARCODE)
					valuePram = get_global_bparam(GLOBAL_REPORT_FLAG2) & 0x01;
					if(valuePram != 0)// use barcode decimal point on weight. 
					{
						if(param->Weight >= power+1) 
						{
							param->Weight = 0L; 
							ret = OFF;
						}
					}
					else//valuePram == 0) don't use barcode decimal point on weight. 
					{
						if(param->Weight >= power) 
						{
							param->Weight = 0L; 
							ret = OFF;
						}
  #if defined (USE_RUSSIA_LOGISTICS_PLU) || defined (USE_EUR_LOGISTICS_PLU)
						weight_decimal_point = get_decimal_pt(0,0);
						if( weight_decimal_point != netweight_decimalpoint)
						{
							if(weight_decimal_point > netweight_decimalpoint)
							{
								switch( weight_decimal_point - netweight_decimalpoint)
								{
									case 1:
										param->Weight /= 10;
										break;
									case 2:
										param->Weight /= 100;
										break;
								}
							}
							else // (weight_decimal_point < netweight_decimalpoint)
							{
								switch( netweight_decimalpoint - weight_decimal_point)
								{
									case 1:
										param->Weight *= 10;
										break;
									case 2:
										param->Weight *= 100;
										break;
								}
							}
						}
  #endif
					}
			        if(valuePram == 1)// use barcode decimal point on weight. 
					{
						weight_decimal_point = get_decimal_pt(0,0);
						temp = 1;
						for (j=0; j<weight_decimal_point; j++)
						{
							temp *= 10;
						}
						weightValue = (param->Weight / temp) * (temp*10);
						weightValue += (param->Weight % temp);
						
  #ifdef USE_BARCODE_FLEXIBLE
						j=0;
						temp = weightValue;
						while(temp)
						{
							temp /= 10;
							j++;
						}
						if(j<4)
							j+=2;
						sprintf(format,"%c0%dld",'%',j); //
  #endif
						sprintf(&bar_str[str_pnt],format,weightValue);
						bar_str[str_pnt + w_v - (weight_decimal_point + 1)] = '.';
					}
					
  #ifdef USE_HYUNDAI_SET_FUNCTION
				    if(MeatSetSaleMode && (Operation.saleStatus == PRINT_SET_LABEL))
				    {
			  		    v16_1 = (INT16U)SetSaleRTC.hour;
					    v16_2 = (INT16U)SetSaleRTC.min;				  
					    sprintf(&bar_str[str_pnt],"%02d%02d",v16_1,v16_2);
				    }
  #endif
					else
						sprintf(&bar_str[str_pnt],format,param->Weight);
#else
					if(param->Weight >= power) 
					{
						param->Weight = 0L; 
						ret = OFF;
					}
					sprintf(&bar_str[str_pnt],format,param->Weight);
#endif
					if(v_index_w != 0xff) {	// 'W'가 'v' 뒤에 있는 경우
						if (w_v == 4) bar_str[v_index_w] = checksum_price_weight1(param->Weight)+'0';
						else if (w_v == 5) bar_str[v_index_w] = checksum_price_weight2(param->Weight)+'0';
					}
			        	break;
#ifdef USE_TOPMART_DEFAULT
					}
#endif
				case BARCODE_SYMBOL_GROSS_WEIGHT:
					if(status_scale.Weight >= power) 
					{
						ret = OFF;
					}
					//파라미터 543 설정시
					// 0 :  무게바코드 -> 무게만큼 인쇄
					// 1 :  PLU타입이 COUNT 일때  무게바코드 ->갯수 인쇄
					// 2 :  정가 판매시나 용기판매일때 무게바크드 -> "1"인쇄
					// 3 :  1번과 2번을 같이 사용함.
					//  KOR 일때 3 ,  default = 0
					if(PrtItemStr.l_PluType != PLU_BY_WEIGHT)	// MODE 1 :  IF PLU(count type) then BARCODE "W"->"Q"
					{
						if((get_global_bparam(GLOBAL_BARCODE_CUT)>>1)&0x01)
							param->TareWeight = 0L;
					}
					else						// MODE 2 :  IF Fixedprice then BARCODE "W"->"1"	
					{
						if(((get_global_bparam(GLOBAL_BARCODE_CUT)>>2)&0x01) && Operation.fixedPrice)
							param->TareWeight = 0L;
					}						
					sprintf(&bar_str[str_pnt],format,param->Weight+param->TareWeight);
					break;
				case BARCODE_SYMBOL_QUANTITYCHECKSUM:	// Quantity check digit
					q_v += q_cnt;  // quntity checksum 사용 시 바코드 Q 뒤의 '0'을 Q의 갯수에 포함
					if(q_v == 0) v_index_q = str_pnt;	// 'Q'가 'e' 뒤에 있는 경우
					else if(q_v == 4) {
						q_cnt = 0;
						bar_str[str_pnt]=checksum_price_weight1(param->Quantity)+'0';
					}
					else if(q_v == 5) {
						q_cnt = 0;
						bar_str[str_pnt]=checksum_price_weight2(param->Quantity)+'0';
					}
					break;
				case BARCODE_SYMBOL_QUANTITY:
#ifdef USE_TOPMART_DEFAULT //sewon 170113
					if (status_scale.Plu.ptype == PLU_BY_WEIGHT)
					{
						if(org_plucode)
						{
							if (PrtItemStr.fixed_price) param->Weight = 0;
						}
						if(param->Weight >= power) 
						{
							param->Weight = 0L; 
							ret = OFF;
						}
							sprintf(&bar_str[str_pnt],format,param->Weight);  
						if(v_index_w != 0xff) { // 'W'가 'v' 뒤에 있는 경우
							if (w_v == 4) bar_str[v_index_w] = checksum_price_weight1(param->Weight)+'0';
							else if (w_v == 5) bar_str[v_index_w] = checksum_price_weight2(param->Weight)+'0';
						} 
							break;
					}
					else
					{
#endif
					q_v=cnt;
					if(param->Quantity >= power) {
						param->Quantity = 0L; 
						ret = OFF;
					}
#ifdef USE_BARCODE_FLEXIBLE
					j=0;
					temp = param->Quantity;
					while(temp/10)
					{
						temp /= 10;
						j++;
					}
					sprintf(format,"%c0%dld",'%',j); //
#endif
					sprintf(&bar_str[str_pnt],format,param->Quantity);
					if(v_index_q != 0xff) {	// 'Q'가 'e' 뒤에 있는 경우
						m = i;
						while (bar_form[m+1] == '0')
						{
							q_cnt++;
							m++;
						}
						q_v += q_cnt;
						q_cnt = 0;
						if (q_v == 4) bar_str[v_index_q] = checksum_price_weight1(param->Quantity)+'0';
						else if (q_v == 5) bar_str[v_index_q] = checksum_price_weight2(param->Quantity)+'0';
					}
#ifdef USE_TOPMART_DEFAULT
					}
#endif
					break;
				case BARCODE_SYMBOL_UNITPRICE:
					if(param->UnitPrice >= power) {
						param->UnitPrice = 0L;
					#ifndef USE_BARCODE_UPRICE_OVER
						ret = OFF;
					#endif
					}
					sprintf(&bar_str[str_pnt],format,param->UnitPrice);
					break;
				case BARCODE_SYMBOL_DEPTNO:
					if (emart) goto EMART;
					if(param->DeptId >= power) {
						param->DeptId = 0; 
						ret = OFF;
					}
					sprintf(&bar_str[str_pnt],format,(INT32U)param->DeptId);	
					break;
				case BARCODE_SYMBOL_GROUP:
					if(param->GroupNo >= power) {
						param->GroupNo = 0L; 
						ret = OFF;
					}
					sprintf(&bar_str[str_pnt],format,(INT32U)param->GroupNo);	
					break;
				case BARCODE_SYMBOL_COUNTRY:
					if(param->CountryNo >= power) {
						param->CountryNo = 0L; 
						ret = OFF;
					}
					sprintf(&bar_str[str_pnt],format,(INT32U)param->CountryNo);	
					break;
				case BARCODE_SYMBOL_TICKET_NO:
					if(param->TicketNo >= power) {
						param->TicketNo = 0L; 
						ret = OFF;
					}
					sprintf(&bar_str[str_pnt],format,param->TicketNo);	
					break;
				case BARCODE_SYMBOL_LABELTRAN_NO:
					if(param->LabelTranNo >= power) {
						param->LabelTranNo = 0L; 
						ret = OFF;
					}
					sprintf(&bar_str[str_pnt],format,param->LabelTranNo);	
					break;
				case BARCODE_SYMBOL_CLERK_NO:
					if(param->ClerkNo >= power) {
						param->ClerkNo = 0; 
						ret = OFF;
					}
					sprintf(&bar_str[str_pnt],format,(INT32U)param->ClerkNo);	
					break;
#ifdef USE_CHINA_CARRFOUR_DEFAULT
				case BARCODE_SYMBOL_PRICE_TYPE: //Price Type 중국 Carrfour 기능 (기존 bonus 항목을 변경)
					plu_get_field(PrtItemStr.plu_addr-1,50, (INT8U *)&PrtItemStr.bonus_point,&uz,sizeof(PrtItemStr.bonus_point)); //carrfour에서는 Price Type으로 사용함.
					sprintf(&bar_str[str_pnt],format,PrtItemStr.bonus_point);
					break;
#endif
				case BARCODE_SYMBOL_PREFIX:	// PLU Prefix Character
EMART:	
					if (Startup.country==COUNTRY_KR) {
				        	sprintf(&bar_str[str_pnt],format,(INT32U)param->iPrefix);
					} else {
						for(j = 0; j < cnt; j++) {
							if(!param->Prefix[j]) break;
							bar_str[str_pnt+j] = param->Prefix[j]; 
						}
						for( ; j < cnt; j++) bar_str[str_pnt+j] = '0';
					}
					break;
				case BARCODE_SYMBOL_SPECIALCHAR:	// Print Next Fixed Character
					i++;
					bar_str[str_pnt] = bar_form[i];
					break;
				case BARCODE_SYMBOL_TEXT:	// Text
#ifdef USE_KOR_TRACE          	// 저울나라 프로토콜
					if(!KorTrace_Flag.flagDrawBarcode){
						ret = OFF;
					}
#endif
#ifndef USE_INVENTORY_LABEL
#ifdef USE_TRACE_STANDALONE
					if(!CurTraceStatus.flagBarcodePrint){
						ret = OFF;
					}
#endif			         
#endif
#ifdef USE_TOPMART_DEFAULT
					if(!strncmp(param->Text,"777777777777",12))
					{	
						ret = ON;
						break;	
					}
#endif
#ifdef USE_NHMART_SAFE_MEAT
					for(j = 0; j < cnt; j++) {
						if(!param->Text[j])
						{
							cnt = j;
							break;
						}
						bar_str[str_pnt+j] = param->Text[j]; 
					}
#elif defined (USE_EMART_PIG_TRACE)
					z = 0;
					if(param->Text[0] == 'A')
					{
						count = cnt+1;
					}
					else
					{
						count = cnt;
					}

					for(j = 0; j < cnt; j++) 
					{
						if(!param->Text[j]) break;
						if(((param->Text[j] == 'L') || (param->Text[j] == 'A')) && (param->Text[j+1]>='0') && (param->Text[j+1]<='9')) z=1;
						bar_str[str_pnt+j] = param->Text[j+z]; 
					}
					//for( ; j < cnt; j++) bar_str[str_pnt+j] = '0';
					for( ; j < count; j++) 
					{
						for(z=j-1; z>=0 ;z--)
						{
							bar_str[str_pnt+z+1]=bar_str[str_pnt+z];
						}
						bar_str[str_pnt] = '0';
					}
#elif defined(USE_GSMART_PIG_TRACE) ||  defined(USE_TRACE_PIG_STANDALONE) || defined(USE_LOTTEMART_PIG_TRACE)
					if(cnt != 14)	// 'T'가 14개가 아닐 경우 '0'으로 프린트 함
					{
						if(cnt == 12) // 바코드 포맷이 T 12 이력바코드용.
						{
							if(trace_bar_alone == 1) //이력번호만 단독으로 사용 될 경우
							{
								if(!param->Text[0]) break; // data가 없을 경우
							}
							for(j = 0; j < cnt; j++)
							{
								if(!param->Text[j]) break;
								bar_str[str_pnt+j] = param->Text[j];
							}
							for( ; j < cnt; j++) bar_str[str_pnt+j] = '0';
						}
						else if(cnt == 15) //바코드 포맷이 T 15 이력 바코드용 20150423
						{
							for(j = 0; j < cnt; j++)
							{
								if(!param->Text[j])
								{
									cnt = j;
									break;
								}
								bar_str[str_pnt+j] = param->Text[j];
							}
						}
  #if defined (USE_IMPORT_LOTNO) && defined (USE_MANUAL_INPUT_IMPORT_LOTNO)
						else if(cnt == 24 && param->Text[0] == 'A')
						{
							for(j=0 ; j < cnt+1; j++) bar_str[str_pnt+j] = param->Text[j];
						}
  #endif
						else
						{
							for(j=0 ; j < cnt; j++) bar_str[str_pnt+j] = '0';
						}
					}
					else
					{
						z = 0;
  #ifdef USE_LOTTEMART_MEATSCANNER
						if(CurTraceStatus.no14indivflag)
						{
							if(CurTraceStatus.indivno[0] == 'L' && CurTraceStatus.indivno[1] != 'O') // 신규 LOT 번호
							{
								for(j = 0; j < cnt; j++) // 'L'을 제외하고 복사
								{
									if(!CurTraceStatus.indivno[j+1]) break;
									else
									{
										bar_str[str_pnt+j] = CurTraceStatus.indivno[j+1];
									}
								}
								for( ; j < cnt; j++) bar_str[str_pnt+j] = '0';	//이력번호자리수 14 까지 '0'으로 채움
							}
							else	// 일반 이력번호 , 구 LOT
							{
								for(j = 0; j < cnt; j++)
								{
									if(!CurTraceStatus.indivno[j]) break;
									else
									{
										bar_str[str_pnt+j] = CurTraceStatus.indivno[j];
									}
								}
								for( ; j < (cnt-2); j++) bar_str[str_pnt+j] = '0';	//이력번호자리수 14
								for( ; j < cnt; j++) //이력번호에 앞부분에 00을 넣는 부분
								{
									for(z=j-1; z>=0 ;z--)
									{
										bar_str[str_pnt+z+1]=bar_str[str_pnt+z];
									}
									bar_str[str_pnt] = '0';
								}
							}
						}
						else
						{
  #endif
						if(param->Text[0] == 'L' && param->Text[1] != 'O') // 신규 LOT 번호
						{
							for(j = 0; j < cnt; j++) // 'L'을 제외하고 복사
							{
								if(!param->Text[j+1]) break;
								else
								{
									bar_str[str_pnt+j] = param->Text[j+1];
								}
							}
							for( ; j < cnt; j++) bar_str[str_pnt+j] = '0';	//이력번호자리수 14 까지 '0'으로 채움
						}
						else	// 일반 이력번호 , 구 LOT
						{
							if(trace_bar_alone == 1) //이력번호만 단독으로 사용 될 경우
							{
								if(!param->Text[0]) break; // data가 없을 경우
							}
							for(j = 0; j < cnt; j++)
							{
								if(!param->Text[j]) break;
								else
								{
									bar_str[str_pnt+j] = param->Text[j];
								}
							}
							for( ; j < (cnt-2); j++) bar_str[str_pnt+j] = '0';	//이력번호자리수 14
							for( ; j < cnt; j++) //이력번호에 앞부분에 00을 넣는 부분
							{
								for(z=j-1; z>=0 ;z--)
								{
									bar_str[str_pnt+z+1]=bar_str[str_pnt+z];
								}
								bar_str[str_pnt] = '0';
							}
						}
  #ifdef USE_LOTTEMART_MEATSCANNER
						}
  #endif
					}
#elif defined(USE_TOPMART_DEFAULT)					
					if(param->Text[0] == 'L')
					{
						for(j = 0; j < cnt; j++)
						{
							if(!param->Text[j]) break;
							else
							{
								if(j==0)
								{
									bar_str[str_pnt+j] = '0';
									bar_str[str_pnt+j+1] = '0';
								}
								else
								{
									bar_str[str_pnt+j+1] = param->Text[j];
								}
							}
						}						
					}
					else
					{
						for(j = 0; j < cnt; j++) 
						{
							if(!param->Text[j]) break;
							bar_str[str_pnt+j] = param->Text[j]; 
						}
						//for( ; j < cnt; j++) bar_str[str_pnt+j] = '0';

					}
#elif defined (USE_NHMART_PIG_TRACE) //hanaro 20150116
					if (cnt == 14)
					{
						z = 0;
						if(param->Text[0] == 'L' && param->Text[1] != 'O') // 신규 LOT 번호 
						{
// 15자리 묶음번호 프린트 기능 시 사용 
//							for(j = 0; j < cnt; j++) 
//							{
//								if(!param->Text[j]) break;
//								bar_str[str_pnt+j] = param->Text[j]; 
//							}
//							for( ; j < cnt; j++) bar_str[str_pnt+j] = '0';
// 15자리 묶음번호 프린트 기능 END
							for(j = 0; j < cnt; j++) // 'L'을 제외하고 복사
							{
								if(!param->Text[j+1]) break;
								else
								{
									bar_str[str_pnt+j] = param->Text[j+1];
								}
							}
							for( ; j < cnt; j++) bar_str[str_pnt+j] = '0';  //이력번호자리수 14 까지 '0'으로 채움
						}
						else	// 일반 이력번호 , 구 LOT 
						{
							for(j = 0; j < cnt; j++)
							{
								if(!param->Text[j]) break;
								else
								{
									bar_str[str_pnt+j] = param->Text[j];
								}
							}
							for( ; j < (cnt-2); j++) bar_str[str_pnt+j] = '0';  //이력번호자리수 14
							for( ; j < cnt; j++) //이력번호에 앞부분에 00을 넣는 부분 
							{
								for(z=j-1; z>=0 ;z--)
								{
									bar_str[str_pnt+z+1]=bar_str[str_pnt+z];
								}
								bar_str[str_pnt] = '0';
							}
						}

					}
					else
					{
						for(j = 0; j < cnt; j++) 
						{
							if(!param->Text[j]) break;
							bar_str[str_pnt+j] = param->Text[j]; 
						}
						for( ; j < cnt; j++) bar_str[str_pnt+j] = '0';
					}
#else	// 기타
					for(j = 0; j < cnt; j++) 
					{
						if(!param->Text[j]) break;
						bar_str[str_pnt+j] = param->Text[j]; 
					}
					for( ; j < cnt; j++) bar_str[str_pnt+j] = '0';
#endif	// USE_NHMART_SAFE_MEAT
					
#ifndef USE_INVENTORY_LABEL
#ifdef USE_TRACE_STANDALONE			
					switch (ModeIndiv){
						default:
						case 0://not used								
							break;
						case 3://GS마트
							if(strncmp((char *)param->Text, (char *)LotStr, 3) == 0)
							{
								for(j = 0; j < cnt - 2; j++) 
								{
									if(!param->Text[j]) break;
									bar_str[str_pnt + j + 4] = param->Text[j + 3]; 
								}
								bar_str[str_pnt+0]='0';
								bar_str[str_pnt+1]='0'; 
								bar_str[str_pnt+2]='9'; //gspig 20141212
								bar_str[str_pnt+3]='9';
							}
							break;
						case 1://신세계
						case 4://standard (신세계 + 개체인덱스), 독립형 
							if(strncmp((char *)param->Text, (char *)LotStr, 3) == 0)
							{
								bar_str[str_pnt+2]='9';
								bar_str[str_pnt+3]='9';
								bar_str[str_pnt+4]='9';
							}
							break;
						case 5://이마트
							if(strncmp((char *)param->Text, (char *)LotStr, 3) == 0)
							{
#ifdef USE_EMART_PIG_TRACE
								bar_str[str_pnt+2]='9';
								bar_str[str_pnt+3]='9';  
								bar_str[str_pnt+4]='2';
#else	// USE_EMART_PIG_TRACE 
								bar_str[str_pnt+0]='9';
								bar_str[str_pnt+1]='9';
								bar_str[str_pnt+2]='2';
#endif	// USE_EMART_PIG_TRACE 
							}
							break;                            
						case 2://농협
#ifdef USE_NHMART_SAFE_MEAT                        
							if(strncmp((char *)param->Text, (char *)LotStr, 3) == 0)
							{
								bar_str[str_pnt+0]='9';
								bar_str[str_pnt+1]='9';
                                
								for(j = 0; j < cnt - 2; j++) 
								{
									if(!param->Text[j]) break;
									bar_str[str_pnt + j + 2] = param->Text[j + 3]; 
								}
							} 								
#else	//USE_NHMART_SAFE_MEAT
							if(cnt == 18) //hanaro 20150116
							{
								if(strncmp((char *)param->Text, (char *)LotStr, 3) == 0)
								{
									bar_str[str_pnt+0]='9';
									bar_str[str_pnt+1]='9';                                
									bar_str[str_pnt+2]='9';
								} 								
								else 
								{
									bar_str[str_pnt+0]='8';
									bar_str[str_pnt+1]='8';
									bar_str[str_pnt+2]='8';
									bar_str[str_pnt+3]='4';
									bar_str[str_pnt+4]='1';
									bar_str[str_pnt+5]='0';

									for(j = 0; j < cnt; j++) 
									{
										if(!param->Text[j]) break;
										bar_str[str_pnt + j + 6] = param->Text[j]; 
									}
								}
							}
#endif	//USE_NHMART_SAFE_MEAT
							break;
					}
#endif	//#ifdef USE_TRACE_STANDALONE
#endif	//#ifndef USE_INVENTORY_LABEL
			        	break;
#ifdef USE_TRACE_STANDALONE
				case BARCODE_SYMBOL_MEATUSE:	// 용도
  #ifdef USE_LOTTEMART_MEATSCANNER
					if(CurTraceStatus.no14indivflag)
					{
						if(CurTraceStatus.meatUseNo >= power)
						{
							CurTraceStatus.meatUseNo = 0;
							ret = OFF;
						}
						sprintf(&bar_str[str_pnt],format,(INT32U)CurTraceStatus.meatUseNo);
					}
					else
					{
  #endif
					if(param->MeatUseNo >= power) {
						param->MeatUseNo = 0; 
						ret = OFF;
					}
					sprintf(&bar_str[str_pnt],format,(INT32U)param->MeatUseNo);	
  #ifdef USE_LOTTEMART_MEATSCANNER
					}
  #endif
					break;
				case BARCODE_SYMBOL_SL_NO:	// 도축장명
#ifdef USE_LOTTEMART_MEATSCANNER
					if(CurTraceStatus.no14indivflag)
					{
						if(CurTraceStatus.slPlace1>= power) {
							CurTraceStatus.slPlace1 = 0;
							ret = OFF;
						}
						sprintf(&bar_str[str_pnt],format,(INT32U)CurTraceStatus.slPlace1);
					}
					else
					{
#endif
					if(param->SlaughtHouseNo >= power) {
						param->SlaughtHouseNo = 0; 
						ret = OFF;
					}
					sprintf(&bar_str[str_pnt],format,(INT32U)param->SlaughtHouseNo);	
  #ifdef USE_LOTTEMART_MEATSCANNER
					}
  #endif
					break;
  #ifdef USE_TRACE_MEATPART
				case BARCODE_SYMBOL_MEATPART:	// 부위
					if(param->MeatPartNo >= power) {
						param->MeatPartNo = 0; 
						ret = OFF;
					}
					sprintf(&bar_str[str_pnt],format,(INT32U)param->MeatPartNo);	
					break;
  #endif                        
  #ifdef USE_TRACE_MEATGRADE
    #ifdef USE_TRACE_SCANNER_FOR_GRADE
				case BARCODE_SYMBOL_MEATGRADE:	// 축산등급
					if(param->MeatGradeNo >= power) {
						param->MeatGradeNo = 0; 
						ret = OFF;
					}
					sprintf(&bar_str[str_pnt],format,(INT32U)param->MeatGradeNo);	
					break;
    #endif //USE_TRACE_SCANNER_FOR_GRADE
  #endif // USE_TRACE_MEATGRADE
#endif //USE_LOTTEMART_MEATSCANNER
				case BARCODE_SYMBOL_PLU_TRANS_COUNTER:	// PLU Transaction Count
//					if(cnt > 9) temp_cnt = 9;
					if(GlbOper.PluReportType) param->PluTrCnt = 0;
					else
					{ 
						if(param->PluTrCnt >= power) {
							param->PluTrCnt = 0; 
							ret = OFF;
						}
					}
					sprintf(&bar_str[str_pnt],format,(INT32U)param->PluTrCnt);	
					break;
				case BARCODE_SYMBOL_CHECKSUM:
#ifdef USE_DOUBLE_PRINT
					if (bar_type == CODE128C || bar_type == CODE128X || bar_type == QRCODE)
#else
					if (bar_type == CODE128C || bar_type == CODE128X || bar_type == RSSEXP)
#endif//USE_DOUBLE_PRINT
					{
						bar_str[str_pnt] = BarUpc_Checksum((HUGEDATA char *)&bar_str[0], str_pnt);
					}
					else
					{
						cnt = 0;
					}
					break;
				case BARCODE_SYMBOL_GTIN_CHECKSUM:	// GTIN-14 Check Digit
					if (str_pnt > 13)
					{
						bar_str[str_pnt] = BarUpc_Checksum((HUGEDATA char *)&bar_str[str_pnt-13], 13);	// 13digit check digit
					}

					else
					{
						ret = OFF;
					}
					break;
/////////////////// Traceability ////////////////////
#ifdef USE_GSMART_BARCODE
				case BARCODE_SYMBOL_DISCOUNT:
					if (param->DiscountFlag >= power)
					{
						param->DiscountFlag = 0L;
						ret = OFF;
					}
					sprintf(&bar_str[str_pnt],format,(INT32U)param->DiscountFlag);
#else
				case BARCODE_SYMBOL_TRACEABILITY:	// Traceability Refer.no. 	
#endif
#ifdef USE_TRACEABILITY //gm 090407, memory 충돌 방지
					string_buf[0] = 0;
					if(param->trace_no) {
						addr = DFLASH_BASE + FLASH_TRACEABILITY_AREA;
						addr = addr + TRACEABILITY_STRUCT_SIZE * (INT32U)(param->trace_no - 1) + 2;
						Flash_sread(addr, (HUGEDATA INT8U *)string_buf, TRACEABILITY_STRUCT_SIZE-10);
					}
					v16_1 = strlen(string_buf);
					j = 0;
					k = 0;
					if(v16_1 < cnt) {
						for(j = 0; j < cnt - v16_1; j++) bar_str[str_pnt+j] = '0';	// 우측정렬
					}
					for( ; j < cnt; j++) {
						if(!string_buf[k]) break;
						bar_str[str_pnt+j] = string_buf[k]; 
						k++;
					}
					for( ; j < cnt; j++) bar_str[str_pnt+j] = '0';
#endif
					break;
				case BARCODE_SYMBOL_SLAUGHTER_NO:	// Slaughter House ID
#ifdef USE_TRACEABILITY //gm 090407, memory 충돌 방지
					string_buf[0] = 0;
					if(param->slaught_no) {
						addr = DFLASH_BASE + FLASH_SLAUGHTERHOUSE_AREA;
						addr = addr + SLAUGHTERHOUSE_STRUCT_SIZE * (INT32U)(param->slaught_no - 1);// + 30;
						v16_1 = Flash_wread(addr);
						if(v16_1 == param->slaught_no) {
							addr += 2;
							Flash_sread(addr, (HUGEDATA INT8U *)string_buf, SLAUGHTERHOUSE_STRUCT_SIZE-4);
						}
					}
					v16_1 = strlen(string_buf);
					j = 0;
					k = 0;
					if(v16_1 < cnt) {
						for(j = 0; j < cnt - v16_1; j++) bar_str[str_pnt+j] = '0';	// 우측정렬
					}
					for( ; j < cnt; j++) {
						if(!string_buf[k]) break;
						bar_str[str_pnt+j] = string_buf[k]; 
						k++; 
					}
					for( ; j < cnt; j++) bar_str[str_pnt+j] = '0';
#endif
					break;
				case BARCODE_SYMBOL_CUTTINGHALL:	// Cutting Hall ID
#ifdef USE_TRACEABILITY //gm 090407, memory 충돌 방지					
					string_buf[0] = 0;
					if(param->cutting_no) {
						addr = DFLASH_BASE + FLASH_CUTTINGHALL_AREA;
						addr = addr + CUTTINGHALL_STRUCT_SIZE * (INT32U)(param->cutting_no - 1);// + 30;
						v16_1 = Flash_wread(addr);
						if(v16_1 == param->cutting_no) {
							addr += 2;
							Flash_sread(addr, (HUGEDATA INT8U *)string_buf, CUTTINGHALL_STRUCT_SIZE-4);
						}
					}
					v16_1 = strlen(string_buf);
					j = 0;
					k = 0;
					if(v16_1 < cnt) {
						for(j = 0; j < cnt - v16_1; j++) bar_str[str_pnt+j] = '0';	// 우측정렬
					}
					for( ; j < cnt; j++) {
						if(!string_buf[k]) break;
						bar_str[str_pnt+j] = string_buf[k]; 
						k++; 
					}
					for( ; j < cnt; j++) bar_str[str_pnt+j] = '0';
#endif
					break;	
				case BARCODE_SYMBOL_BORN_IN:	// Born in Country no.
					//Modified by JJANG 20080229 Parameter 569에서 설정한 값을 적용, default : 2110
					get_global_sparam(GLOBAL_PLU_TYPE_AREA,(INT8U *)plucode_buf,4);	
					v16_1 = 0;
					if (emart) // E-mart Code
					{							
						if(org_plucode)		//Added by JJANG 20080403 ,parameter 567 : Y
						{
							if (status_scale.Plu.ptype == PLU_BY_WEIGHT) v16_1 = plucode_buf[0] - 0x30;
							if (status_scale.Plu.ptype == PLU_BY_COUNT ) v16_1 = plucode_buf[1] - 0x30;
							if (status_scale.Plu.ptype == PLU_BY_PCS )   v16_1 = plucode_buf[2] - 0x30;
						#ifdef USE_EMART_NETWORK
							if (Operation.checkin)
							{
								if (status_scale.Plu.ptype == PLU_BY_WEIGHT) v16_1 = 4;
								if (status_scale.Plu.ptype == PLU_BY_COUNT)  v16_1 = 3;
								if (status_scale.Plu.ptype == PLU_BY_PCS)    v16_1 = 3;
							}
							#ifdef USE_DIVIDING_FUNCTION
							if(status_scale.divisor)
							{
								if(Operation.checkin)
								{
									if (status_scale.Plu.ptype == PLU_BY_COUNT)  v16_1 = 6;
									if (status_scale.Plu.ptype == PLU_BY_PCS)    v16_1 = 6;
								}
								else
								{
									if (status_scale.Plu.ptype == PLU_BY_COUNT)  v16_1 = 5;
									if (status_scale.Plu.ptype == PLU_BY_PCS)    v16_1 = 5;
								}
							}
							#endif

						#endif
						}
						else
						{
							if (PrtItemStr.fixed_price) v16_1 = plucode_buf[3]-0x30;
							else
							{
								if (PrtItemStr.l_PluType == PLU_BY_WEIGHT) v16_1 = plucode_buf[0] - 0x30;
								if (PrtItemStr.l_PluType == PLU_BY_COUNT ) v16_1 = plucode_buf[1] - 0x30;
								if (PrtItemStr.l_PluType == PLU_BY_PCS )   v16_1 = plucode_buf[2] - 0x30;
						#ifdef USE_EMART_NETWORK
								if (Operation.checkin)
								{
									if (PrtItemStr.l_PluType == PLU_BY_WEIGHT) v16_1 = 4;
									if (PrtItemStr.l_PluType == PLU_BY_COUNT)  v16_1 = 3;
									if (PrtItemStr.l_PluType == PLU_BY_PCS)    v16_1 = 3;
								}
							#ifdef USE_DIVIDING_FUNCTION
								if(status_scale.divisor)
								{
									if(Operation.checkin)
									{
										if (status_scale.Plu.ptype == PLU_BY_COUNT)  v16_1 = 6;
										if (status_scale.Plu.ptype == PLU_BY_PCS)    v16_1 = 6;
									}
									else
									{
										if (status_scale.Plu.ptype == PLU_BY_COUNT)  v16_1 = 5;
										if (status_scale.Plu.ptype == PLU_BY_PCS)    v16_1 = 5;
									}
								}
							#endif

						#endif
							}
						}
					}
					else 
					{
#ifdef USE_TRACEABILITY //gm 090407, memory 충돌 방지
						if(param->born_country_no >= power) 
						{
							param->born_country_no = 0; 
							ret = OFF;
						}
						v16_1 = param->born_country_no;
#endif
					}
		        		sprintf(&bar_str[str_pnt],format,(INT32U)v16_1);
			        	break;
#ifdef USE_RUSSIA_BARCODE_FUNCTION
				case BARCODE_SYMBOL_WEIGHT_POINT:	//weight decimal point value
					v16_3 = get_global_bparam(GLOBAL_DISPLAY_WEIGHTDECIMAL);
					sprintf(&bar_str[str_pnt],"%01d",v16_3); 				
					break;
				case BARCODE_SYMBOL_PRODUCED_DATE:	//Produced date
					// Produced Date = Current Date - Days + 1
					// 0:no print, 1:today, 2:yesterday...	// 0:no print, 1:yesterday...
 					if(PrtItemStr.produceddate)
					{
						if(PrtItemStr.produceddate > 999) break;
	//					PrtItemStr.produceddate--;	// Added by CJK 20060123
						v16_1 = PrtItemStr.produceddate - 1;
						v16_3 = (INT16U)PrtComStr.time.year;
						v16_4 = (INT16U)PrtComStr.time.month;
						v16_5 = (INT16U)PrtComStr.time.date;
						date_getbeforeday((short*)&v16_3,(short*)&v16_4,(short*)&v16_5,(short)v16_1);
#ifdef USE_LOGISTICS_BARCODE
						sprintf(&bar_str[str_pnt],"%02d%02d%02d",v16_3,v16_4,v16_5);	//year, month, date
						ainumber = 0;
#else //#ifdef USE_LOGISTICS_BARCODE
						rtc_makedate_str(&bar_str[str_pnt], (INT8U)v16_3, (INT8U)v16_4, (INT8U)v16_5);
#endif
 					}
					else
					{
						sprintf(&bar_str[str_pnt],"%02d%02d%02d",0,0,0); 
					}
 					break;
					
#endif
				case BARCODE_SYMBOL_PACKED_DATE:	//Packed on Date
					#ifdef USE_GSMART_PLU
					v8_1 = PrtComStr.time.hour + PrtItemStr.packedontime/100;	// current hour + packed time
					if(PrtComStr.time.min + PrtItemStr.packedontime%100 >= 60) v8_1++;
					#else
					v8_1 = PrtComStr.time.hour + PrtItemStr.packedontime;	// current hour + packed time
					#endif
					v8_1 /= 24;		// calculate day from hour( 0~23 = 0, 24~47 = 1 ...)
					v16_2 = (INT16U)v8_1 + PrtItemStr.packedondate;
					v16_3 = (INT16U)PrtComStr.time.year;
					v16_4 = (INT16U)PrtComStr.time.month;
					v16_5 = (INT16U)PrtComStr.time.date;
					date_getnextday((short*)&v16_3,(short*)&v16_4,(short*)&v16_5,(short)v16_2);
#ifdef USE_PERSIAN_CALENDAR
					if(v16_3 < 0x5a) perCentury = 1400; //90년 이전: 1400년대, 90년 이후: 1300년대
					else perCentury = 1300;
#endif
					if(date_type==0) sprintf(&bar_str[str_pnt],"%02d%02d%02d",v16_3,v16_4,v16_5);	//year, month, date
					else if(date_type==1) sprintf(&bar_str[str_pnt],"%02d%02d%02d",v16_4,v16_5,v16_3);	//month, date, year
#ifdef USE_PERSIAN_CALENDAR
					else if(date_type==2) sprintf(&bar_str[str_pnt],"%02d%04d",v16_4,v16_3+perCentury);	//month, year(4digit)
#else
					else if(date_type==2) sprintf(&bar_str[str_pnt],"%02d%04d",v16_4,v16_3+2000);	//month, year(4digit)
#endif
					else sprintf(&bar_str[str_pnt],"%02d%02d%02d",v16_5,v16_4,v16_3);		//date, month, year
					break;
#ifdef USE_DIGIT4_PRODUCT_SYMBOL
				case BARCODE_SYMBOL_PACKED_TIME: //4자리 pack on date
#ifdef USE_GSMART_PLU
					v8_1 = PrtComStr.time.hour + PrtItemStr.packedontime/100;	// current hour + packed time
					if(PrtComStr.time.min + PrtItemStr.packedontime%100 >= 60) v8_1++;
#else
					v8_1 = PrtComStr.time.hour + PrtItemStr.packedontime;	// current hour + packed time
#endif
					v8_1 /= 24; 	// calculate day from hour( 0~23 = 0, 24~47 = 1 ...)
					v16_2 = (INT16U)v8_1 + PrtItemStr.packedondate;
					v16_3 = (INT16U)PrtComStr.time.year;
					v16_4 = (INT16U)PrtComStr.time.month;
					v16_5 = (INT16U)PrtComStr.time.date;
					date_getnextday((short*)&v16_3,(short*)&v16_4,(short*)&v16_5,(short)v16_2);
#ifdef USE_PERSIAN_CALENDAR
					if(v16_3 < 0x5a)	perCentury = 1400; //90년 이전: 1400년대, 90년 이후: 1300년대
					else perCentury = 1300;
#endif
					tmpv16_3 = v16_3 % 10; //20150423
					tmpdate = v16_4 / 10;
					tmpv16_4 = v16_4 % 10;
					if(tmpdate == 1) v16_5 = v16_5 + 50;
					if(date_type==1) sprintf(&bar_str[str_pnt],"%01d%01d%02d",tmpv16_3,tmpv16_4,v16_5);	//year, month, date
					else if(date_type==2) sprintf(&bar_str[str_pnt],"%01d%02d%01d",tmpv16_4,v16_5,tmpv16_3);	//month, date, year
#ifdef USE_PERSIAN_CALENDAR
					else if(date_type==3) sprintf(&bar_str[str_pnt],"%02d%04d",v16_4,v16_3+perCentury); //month, year(4digit)
#else
					else if(date_type==3) sprintf(&bar_str[str_pnt],"%01d%01d",v16_4,v16_3+2000);	//month, year(4digit)
#endif
					else sprintf(&bar_str[str_pnt],"%02d%01d%01d",v16_5,tmpv16_4,tmpv16_3);		//date, month, year
					break;
#endif
				case BARCODE_SYMBOL_SELL_DATE:	//Sell by Date
#ifdef USE_LOGISTICS_BARCODE
					if(bar_type == RSSEXP) {
					if (PrtItemStr.sellbydate || PrtItemStr.sellbytime) 		// Sell by Date or Sell by Time존재시
					{
						if(PrtItemStr.sellbydate > 9999) {
							v16_3 = (INT16U)(PrtItemStr.sellbydate / 10000L % 100L);
							v16_4 = (INT16U)(PrtItemStr.sellbydate / 100L % 100L);
							v16_5 = (INT16U)(PrtItemStr.sellbydate %100L);
							v16_1 = 0;
						} else {
							v16_3 = (INT16U)PrtComStr.time.year;
							v16_4 = (INT16U)PrtComStr.time.month;
							v16_5 = (INT16U)PrtComStr.time.date;
							v16_1 = (INT16U)PrtItemStr.sellbydate;
							if(v16_1 > 0) v16_1--;	// sell by date  0:no print, 1:today, 2:tomorrow...
							v16_1 = v16_1 + PrtItemStr.packedondate;	// Packed Date + Sell by Date
						}
						// Time 계산
						v8_1 = PrtComStr.time.hour + PrtItemStr.packedontime + PrtItemStr.sellbytime;		// current time + Packed Time + Sell by Time
						v8_1 /= 24;
						v16_1 += (INT16U)v8_1;
						t16_3=v16_3; t16_4=v16_4; t16_5=v16_5;
						date_getnextday((short*)&v16_3,(short*)&v16_4,(short*)&v16_5,(short)v16_1);
						sprintf(&bar_str[str_pnt],"%02d%02d%02d",v16_3,v16_4,v16_5);	//year, month, date
					}
					else
					{
						sprintf(&bar_str[str_pnt],"000000");
					}
					}
					else
					{
#endif // #ifdef USE_LOGISTICS_BARCODE	
#ifdef USE_GEORGIA_MART_SELLBYDATE_FUNCTION
					/* Sell_by_Date Value 0일때 Today 출력하고 900 이상일때 No Print 기능 (그루지아 Agrohub 마트 요청) */
					if (PrtItemStr.sellbydate < 900)
#else			
  #ifdef USE_SELLBYDATETIME_ZERO
					if (PrtItemStr.sellbydate || PrtItemStr.sellbytime) 		// Sell by Date or Sell by Time존재시
  #else
					if (PrtItemStr.sellbydate || PrtItemStr.sellbytime || PrtItemStr.flagsellbydate == 2)		// Sell by Date or Sell by Time존재시
  #endif
#endif
						// Date 계산
					{
						if(PrtItemStr.sellbydate > 9999) {
							v16_3 = (INT16U)(PrtItemStr.sellbydate / 10000L % 100L);
							v16_4 = (INT16U)(PrtItemStr.sellbydate / 100L % 100L);
							v16_5 = (INT16U)(PrtItemStr.sellbydate %100L);
							v16_1 = 0;
						} else {
							v16_3 = (INT16U)PrtComStr.time.year;
							v16_4 = (INT16U)PrtComStr.time.month;
							v16_5 = (INT16U)PrtComStr.time.date;
							v16_1 = (INT16U)PrtItemStr.sellbydate;
#ifdef USE_GEORGIA_MART_SELLBYDATE_FUNCTION
							// 그루지아 Agrohub 마트 요청을 Input_value 1일때 바로 Tomorrow 출력되도록 함
#else
							if(v16_1 > 0) v16_1--;	// sell by date  0:no print, 1:today, 2:tomorrow...
#endif							
							v16_1 = v16_1 + PrtItemStr.packedondate;	// Packed Date + Sell by Date
						}
						// Time 계산
						#ifdef USE_GSMART_PLU
						if (PrtItemStr.flagsellbytime == 3)	//  3:지정
						{
							v8_1 = PrtItemStr.sellbytime / 100;
						}
						else
						{
							if (PrtItemStr.flagpackedontime == 3)	//  packed on time 3:지정
							{
								v8_1 = PrtItemStr.packedontime / 100;
								v8_2 = PrtItemStr.packedontime % 100;
							}
							else
							{
								v8_1 = PrtComStr.time.hour + PrtItemStr.packedontime / 100;		// current time + Packed Time + Sell by Time
								v8_1 += (PrtComStr.time.min + PrtItemStr.packedontime % 100) / 60;
								v8_2 = (PrtComStr.time.min + PrtItemStr.packedontime % 100) % 60;
							}
							v8_1 += PrtItemStr.sellbytime / 100;
							v8_1 += (v8_2 + PrtItemStr.sellbytime % 100) / 60;
						}
						#else
						v8_1 = PrtComStr.time.hour + PrtItemStr.packedontime + PrtItemStr.sellbytime;		// current time + Packed Time + Sell by Time
						#endif
						v8_1 /= 24;
#ifndef USE_LOTTEMART_BARCODE_FUNCTION
						v16_1 += (INT16U)v8_1;	//lottemart는 유효시간 계산이 유효일에 더해지지 않음.
#endif
						t16_3=v16_3; t16_4=v16_4; t16_5=v16_5;
						#ifdef USE_GSMART_PLU
						if (packed_date)
						{
							date_getbeforeday((short*)&v16_3,(short*)&v16_4,(short*)&v16_5,(short)packed_date);
						}
						#endif
						date_getnextday((short*)&v16_3,(short*)&v16_4,(short*)&v16_5,(short)v16_1);
						//-[BM20060309
						v8_1=1;
						if (sellbydate_check && PrtItemStr.flagsellbydate == 0) {
							v8_1=0;
							if ((!((t16_3==v16_3) && (t16_4==v16_4) && (t16_5==v16_5))) || PrtItemStr.sellbydate) v8_1=1;
						}
						//-[END
#ifdef USE_JULIAN_DATE
						days = date_getdays((short)v16_3+2000, (short)v16_4, (short)v16_5);
#endif
						if(v8_1)
						{
#ifdef USE_JULIAN_DATE
							if(sellbydate_type == 0)
							{
								if(date_type==0) sprintf(&bar_str[str_pnt],"%02d%02d%02d",v16_3,v16_4,v16_5);	//year, month, date
								else if(date_type==1) sprintf(&bar_str[str_pnt],"%02d%02d%02d",v16_4,v16_5,v16_3);	//month, date, year
								else if(date_type==2) sprintf(&bar_str[str_pnt],"%02d%04d",v16_4,v16_3+2000);	//month, year(4digit)
								else sprintf(&bar_str[str_pnt],"%02d%02d%02d",v16_5,v16_4,v16_3);	//date, month, year
							}
							else if(sellbydate_type == 1) sprintf(&bar_str[str_pnt],"%03d%02d",days,v16_3);	//date, year,
							else if(sellbydate_type == 2) sprintf(&bar_str[str_pnt],"%03d",days);	//date,(time)
							else if (sellbydate_type == 3) sprintf(&bar_str[str_pnt],"%01d%02d%02d",v16_3%10,v16_4,v16_5);	//year, month, date
#else
							if(date_type==0) sprintf(&bar_str[str_pnt],"%02d%02d%02d",v16_3,v16_4,v16_5);	//year, month, date
							else if(date_type==1) sprintf(&bar_str[str_pnt],"%02d%02d%02d",v16_4,v16_5,v16_3);	//month, date, year
#ifdef USE_PERSIAN_CALENDAR
							else if(date_type==2) sprintf(&bar_str[str_pnt],"%02d%04d",v16_4,v16_3+perCentury);	//month, year(4digit)
#else
							else if(date_type==2) sprintf(&bar_str[str_pnt],"%02d%04d",v16_4,v16_3+2000);	//month, year(4digit)
#endif
							else sprintf(&bar_str[str_pnt],"%02d%02d%02d",v16_5,v16_4,v16_3);	//date, month, year
#endif
						}
					}
					else
					{
						sprintf(&bar_str[str_pnt],"000000");
					}
#ifdef USE_LOGISTICS_BARCODE
					}
#endif					
					break;
				case BARCODE_SYMBOL_SELL_TIME:	//Sell by Time
					#ifdef USE_SELLBYDATETIME_ZERO
					if(PrtItemStr.sellbydate || PrtItemStr.sellbytime) 
					#else
					if(PrtItemStr.sellbydate || PrtItemStr.sellbytime || PrtItemStr.flagsellbytime == 2 || PrtItemStr.flagsellbytime == 3) 
					#endif
					{
						#ifdef USE_GSMART_PLU
						if (PrtItemStr.flagsellbytime == 3)	//  3:지정
						{
							v8_1 = PrtItemStr.sellbytime / 100;
							v8_2 = PrtItemStr.sellbytime % 100;
						}
						else
						{
							if (PrtItemStr.flagpackedontime == 3)	//  packed on time 3:지정
							{
								v8_1 = PrtItemStr.packedontime / 100;
								v8_2 = PrtItemStr.packedontime % 100;
							}
							else
							{
								v8_1 = PrtComStr.time.hour + PrtItemStr.packedontime / 100;		// current time + Packed Time + Sell by Time
								v8_1 += (PrtComStr.time.min + PrtItemStr.packedontime % 100) / 60;
								v8_2 = (PrtComStr.time.min + PrtItemStr.packedontime % 100) % 60;
							}
							v8_1 += PrtItemStr.sellbytime / 100;
							v8_1 += (v8_2 + PrtItemStr.sellbytime % 100) / 60;
							v8_2 = (v8_2 + PrtItemStr.sellbytime % 100) % 60;
						}
						#else
						v8_1 = PrtComStr.time.hour + PrtItemStr.packedontime + PrtItemStr.sellbytime;		// current time + Packed Time + Sell by Time
						v8_2 = PrtComStr.time.min;
						#endif
#ifdef USE_LOTTEMART_BARCODE_FUNCTION
						if(v8_1 >= 24)	//hhmm, lottemart는 유효시간이 다음날로 넘어가지 않음. 유효일이 2일 이상일 경우 무조건 23:59로 찍음
						{	
							v8_1 = 23;
							v8_2 = 59;	
						}
						else if( (PrtItemStr.sellbydate == 1 || PrtItemStr.sellbydate == 0) && PrtItemStr.sellbytime != 0); // 이 경우 에만 시간을 적용함 
  #ifdef USE_DISCOUNT_MEMBERSHIP
						else if(PrtItemStr.flagsellbytime == 3); // 유효시간 지정인 경우 지정된 유효시간을 인쇄함
  #endif
						else
						{	
							v8_1 = 23;
							v8_2 = 59;
						}
#else
						v8_1 %= 24;		//다음날 시간. 
#endif
						sprintf(&bar_str[str_pnt],"%02d%02d",v8_1,v8_2);		//hhmm
					} 
					else 
					{
						sprintf(&bar_str[str_pnt],"0000");
					}
					break;
                case BARCODE_SYMBOL_CURRENTDATE:
					v16_3 = (INT16U)PrtComStr.time.year;
					v16_4 = (INT16U)PrtComStr.time.month;
					v16_5 = (INT16U)PrtComStr.time.date;                   
#ifdef USE_PERSIAN_CALENDAR
                    if(v16_3 < 0x5a) perCentury = 1400; //90년 이전: 1400년대, 90년 이후: 1300년대
                    else perCentury = 1300;
#endif
#ifdef USE_CURRENT_DATE_JULIAN
					days = date_getdays((short)v16_3+2000, (short)v16_4, (short)v16_5);
					if(days)
						sprintf(&bar_str[str_pnt],"%03d",days); //date. 1~365, leap year : 1~366
					else
						sprintf(&bar_str[str_pnt],"000");
#else
					if(date_type==0) sprintf(&bar_str[str_pnt],"%02d%02d%02d",v16_3,v16_4,v16_5);	//year, month, date
					else if(date_type==1) sprintf(&bar_str[str_pnt],"%02d%02d%02d",v16_4,v16_5,v16_3);	//month, date, year
	#ifdef USE_PERSIAN_CALENDAR
					else if(date_type==2) sprintf(&bar_str[str_pnt],"%02d%04d",v16_4,v16_3+perCentury); //month, year(4digit)
	#else
					else if(date_type==2) sprintf(&bar_str[str_pnt],"%02d%04d",v16_4,v16_3+2000);	//month, year(4digit)
	#endif
					else sprintf(&bar_str[str_pnt],"%02d%02d%02d",v16_5,v16_4,v16_3);	//date, month, year
#endif
					break; 
#if defined (USE_RUSSIA_LOGISTICS_PLU) || defined (USE_EUR_LOGISTICS_PLU)
// Logistics PLU를 사용할때 BARCODE_SYMBOL_CONTAINED_UNIT와 Symbol이 겹치기 때문에 #else 처리
#else
				case BARCODE_SYMBOL_CURRENTTIME:
					v16_3 = (INT16U)PrtComStr.time.hour;
					v16_4 = (INT16U)PrtComStr.time.min;
					v16_5 = (INT16U)PrtComStr.time.sec;

					sprintf(&bar_str[str_pnt],"%02d%02d%02d",v16_3,v16_4,v16_5);	//hour, min, sec
					break;
#endif
				case BARCODE_SYMBOL_BRED_IN:	// Bred in Country no.
#ifdef USE_GSMART_BARCODE
					if (param->DiscountVolume >= power) 
					{
						param->DiscountVolume = 0L; 
						ret = OFF;
					}
					sprintf(&bar_str[str_pnt],format,param->DiscountVolume);	
#else
					if(param->bred_country_no >= power) {
						param->bred_country_no = 0; 
						ret = OFF;
					}
					sprintf(&bar_str[str_pnt],format,(INT32U)param->bred_country_no);	
#endif
			        	break;
				case BARCODE_SYMBOL_TARE_WEIGHT:	//Added by JJANG 20071212
					if(param->TareWeight >= power) 
					{
						param->TareWeight = 0L; 
						ret = OFF;
					}   
					sprintf(&bar_str[str_pnt],format,param->TareWeight); 
					break;
				case BARCODE_SYMBOL_SCALE_ID:
					if(param->ScaleId >= power)
					{
						param->ScaleId = 0L; 
						ret = OFF;
					}
					sprintf(&bar_str[str_pnt],format,param->ScaleId); 
					break;
				case BARCODE_SYMBOL_FNC1:
					sprintf(&bar_str[str_pnt],"%c",0x23);
					break;
#ifdef USE_BARCODE_DISCOUNT_RATE
				case BARCODE_SYMBOL_DISCOUNT_RATE:
					if(status_scale.discountflag == 2)
					{
#ifdef USE_PRT_CHNIA_DISCOUNT_RATE
						v8_1 = status_scale.discount_rate;
#else
						v8_1 = 100 - status_scale.discount_rate;
#endif
						sprintf(&bar_str[str_pnt],"%02d",v8_1);
					}
					else
					{
						v8_1 = 0;
						sprintf(&bar_str[str_pnt],"%02d",v8_1);
					}
					break;
#endif
#ifdef USE_PERCENT_WEIGHT
				case BARCODE_SYMBOL_WEIGHT_PERCENT:
					w_v=cnt;
					if(param->PercentWeight>= power) {
						param->PercentWeight = 0L; 
						ret = OFF;
					}
					sprintf(&bar_str[str_pnt],format,param->PercentWeight);  
					if(v_index_w != 0xff) {	// 'W'가 'v' 뒤에 있는 경우
						if (w_v == 4) bar_str[v_index_w] = checksum_price_weight1(param->PercentWeight)+'0';
						else if (w_v == 5) bar_str[v_index_w] = checksum_price_weight2(param->PercentWeight)+'0';
					}
#endif
					break;
#ifdef USE_SETPRODUCT_SALE
				case BARCODE_SYMBOL_SETNUMBER:
					if(param->ScaleId >= power)
					{
						param->ScaleId = 0L;
						ret = OFF;
					}
					v16_1 = get_setproduct_number();
					sprintf(&bar_str[str_pnt],"%02d%04d",param->ScaleId, v16_1);
					break;
#endif
#ifdef USE_LOT_NUMBER_FUNCTION
				case BARCODE_SYMBOL_LOT_NUMBER:
					string_buf[0] = 0;
					addr = DFLASH_BASE + FLASH_LOT_NUMBER;

					Flash_sread(addr, (HUGEDATA INT8U *)string_buf, 20);

					v16_1 = strlen(string_buf);
					j = 0;
					k = 0;
					if(v16_1 < cnt) {
						for(j = 0; j < cnt - v16_1; j++) bar_str[str_pnt+j] = '0';	// 우측정렬
					}
					for( ; j < cnt; j++) {
						if(!string_buf[k]) break;
						bar_str[str_pnt+j] = string_buf[k]; 
						k++; 
					}
					
					break;
#endif
#ifdef USE_HYUNDAI_CHECK_REASON_CODE
				case BARCODE_SYMBOL_REASONCODE:
					if (param->ReasonCode >= power) {
						param->ReasonCode = 0L;
						ret = OFF;
					}
					sprintf(&bar_str[str_pnt],"%01d",(INT32U)param->ReasonCode);
					break;
#endif
				}
			}
			str_pnt+=cnt;
		    	cnt = 0;
			temp_cnt = 0;
		}
	}
SKIP:	
	bar_str[str_pnt]=0;
	return ret;
}

INT8U prt_make_strform(char *bar_str, char *bar_form, STRUCT_STRFORM_PARAM *param, INT8U bar_type)
{
	return prt_make_strform_sub(bar_str, bar_form, param, OFF, bar_type);	
}

INT8U prt_scan_strform(char *bar_str, char *bar_form, STRUCT_STRFORM_PARAM *param)
{
	INT8U modeScanner;

	modeScanner = get_global_bparam(GLOBAL_SCANNER_FLAG1);
	modeScanner = modeScanner&0x07;
	memset((INT8U *)param, 0, sizeof(STRUCT_STRFORM_PARAM));
	if ((modeScanner == 4) && (strlen(bar_str) != strlen(bar_form))) return OFF;
	return prt_make_strform_sub(bar_str, bar_form, param, ON, NO_BARCODE);	
}

void prt_insert_ch(char *str, INT8U pt, char ch)	// 0:No act ,1:첫번째, 2:두번째,....
{
	INT8U i, len;

	if(pt == 0) return;
	len = strlen(str);
	for(i = len; i >= pt; i--) {
		str[i] = str[i - 1];
	}
	str[pt-1]  = ch;
	str[len+1] = 0;
}

INT8U prt_make_barform(char *bar_str, char *bar_form, STRUCT_STRFORM_PARAM *param, INT8U bar_type)
{
	INT8U cut_digit;
	INT32U price_cut, weight_cut;
	INT8U ret, pt;

	cut_digit=get_global_bparam(GLOBAL_BARCODE_PRICE_CUT);
	price_cut = power10((INT16S)cut_digit);
	cut_digit=get_global_bparam(GLOBAL_BARCODE_WEIGHT_CUT);
	weight_cut = power10((INT16S)cut_digit);

	param->TotalPrice /= price_cut;
	param->UnitPrice /= price_cut;
	if(PrtItemStr.l_PluType <= PLU_BY_WEIGHT && !PrtItemStr.fixed_price)	// MODE 1 :  IF PLU(count type) then BARCODE "W"->"Q" par 543
	{
		param->Weight /= weight_cut;
	}
	cut_digit = get_global_bparam(GLOBAL_SALE_SETUP_FLAG2); 
	cut_digit = (cut_digit>>6) & 0x03;
	if(cut_digit==1)
	{
		if(param->TotalPrice>=100000l ){
			param->TotalPrice = param->TotalPrice/100;
			param->DeptId = 22;
		}
		else if(param->TotalPrice>=10000l){
			param->TotalPrice = param->TotalPrice/10;
			param->DeptId = 21;	
		} else{							
			param->DeptId = 20;
		}		
	} 
	else if(cut_digit==2)
	{
		if(param->Weight>=100000l ){
			param->Weight = param->Weight/100;
			param->DeptId = 25;
		}
		else if(param->Weight>=10000l){
			param->Weight = param->Weight/10;
			param->DeptId = 24;	
		} else{							
			param->DeptId = 23;
		}		
	}

	ret = prt_make_strform(bar_str, bar_form, param, bar_type);	// barcode format
	pt = 0;
	switch(bar_type) {
		case EAN13P2 :
		case EAN13P5 :
			//prt_insert_ch(bar_str, 13, ' ');
			pt = 13;
			break;
		case EAN8P2 :
		case EAN8P5 :
			//prt_insert_ch(bar_str, 8, ' ');
			pt = 8;
			break;
		case UPCAP2 :
		case UPCAP5 :
			//prt_insert_ch(bar_str, 12, ' ');
			pt = 12;
			break;
		case UPCE0P2 :
		case UPCE0P5 :
			//prt_insert_ch(bar_str, 7, ' ');
			pt = 7;
			break;
	}
	prt_insert_ch(bar_str, pt, ' ');
	return ret;
}


// N: Item Number
// P: Price
// D: Department
// W: Weight
#ifdef USE_LOTTESUPER_BARCODE_FUNCTION
#define PACKING_LABEL_BARCODE_DATA_LEN	24
#define IDX_OF_PRODNO_BARCODE_HRI_DATA	3
#define IDX_OF_WT_CO_BARCODE_HRI_DATA	52
#define	IDX_OF_PRICE_BARCODE_HRI_DATA	63
#define LEN_OF_SALECODE_BARCODE_DATA	16
#define	LEN_OF_PRODNO_BARCODE_HRI_DATA	6
#define LEN_OF_WT_CO_BARCODE_HRI_DATA	5
#define	LEN_OF_PRICE_BARCODE_HRI_DATA	6
#endif
INT8U prt_make_barcode(INT8U prt_mode, INT16U barid, STRUCT_STRFORM_PARAM *pbar_param, INT8U field_num)
{
	char bar_form[BARCODE_FORMAT_LEN+1];
#ifdef USE_LOTTEMART_BARCODE_FUNCTION
	char bar_str[BARCODE_FORMAT_LEN+1] = {0, };
	char bar_str2[46];		//3103WWWWWW을 담기 위함.
	char bar_str3[35];		//30QQQQQQQQ을 담기 위함.
	char bar_str4[8];
	INT8U i;
	INT8U j = 0;
    bar_str[BARCODE_FORMAT_LEN]='\0';
#elif defined(USE_TOPMART_BARCODE_FUNCTION)
	char bar_str[BARCODE_FORMAT_LEN+1] = {0, };;
	bar_str[BARCODE_FORMAT_LEN]='\0';
	INT8U i;
#else
	char bar_str[BARCODE_FORMAT_LEN+1] = {0, };;
#endif
	INT8U bar_type;
	INT8U barcode_type;
	INT8U ret;
#ifdef USE_QR_ADDTOTAL
	char QRtemp[1000]={0};
//	char backup_bar_str[BARCODE_FORMAT_LEN+1]={0};
//	char csdigit;
//	INT8U cslength;
//	int i;
#endif
#if defined( USE_CARREFOUR_TRACE_QR_BARCODE ) || defined( USE_PRINT_URL_VIA_QR_BARCODE )
	INT8U traceUrlStr[150];
	INT16U strSize = 0;
	INT8U tempUrlSize = 0;
	INT16U key = 0;
#endif
#ifdef USE_PRINT_URL_VIA_QR_BARCODE
	INT16U  indirect_ingredient_no;
	INT16U  usize;
	char	name3_string[40];	// PLU Name3 Field Size: 40
	char	saletime_data[15];	// tYYMMDDHHmmss
#endif
#ifdef USE_DIRECT_INGREDIENT
	INT16U ingredNo[DIRECT_INGREDIENT_BLOCK_NUM];
	char string_buf2[2560];
#endif


#ifdef USE_CHN_EMART_BIRDS_TRACE
	INT32U addr;
	INT8U tmpQrTrace[CHN_EMART_TRACE_QR_SIZE + 1];
	INT16U	key;

#endif
#ifdef USE_REGIST_DISPOSAL
	INT8U len;
#endif

	if (barid<1 || barid>99) barid=0;
	if(!get_barcodeformat(barid,(char *)bar_form, &barcode_type)) return OFF;

	bar_type=barcodetype_codevalue[barcode_type];
	bar_str[0] = 0;
	ret = 0;
	if (bar_type != NO_BARCODE) ret = prt_make_barform(bar_str, bar_form, pbar_param, bar_type);
#ifdef USE_CHN_EMART_BIRDS_TRACE
	if(bar_type == QRCODE)
	{
		plu_table_search(status_scale.departmentid, status_scale.cur_pluid,&key,0);
		addr = QR_CODE_TRACE_ADDR + ((key-1) * CHN_EMART_TRACE_QR_AREA_SIZE);
		Flash_sread(addr + (CHN_EMART_TRACE_QR_SIZE * CHNEmart_TraceSelectionOrder), tmpQrTrace,CHN_EMART_TRACE_QR_SIZE);
		tmpQrTrace[CHN_EMART_TRACE_QR_SIZE] = 0;
        if(tmpQrTrace[0] == 0x00) ret = 0;
	    else ret = 1; //QR code -> barcode form이 없으므로 1261 입력시 입력 값(IIIIIIWWWWWPPPPPC) 상관없이 출력 
	}
#endif
#ifdef USE_CARREFOUR_TRACE_QR_BARCODE
	if( field_num == 3 )
	{
		bar_type = QRCODE; //Barcode Feild Type이 '3' 일 때, 1261 입력 값(IIIIIIWWWWWPPPPPC) 상관없이 호출 된 PLU에서 URL을 QR 바코드로 출력 
  #ifdef USE_CARREFOUR_ADD_PLU_URL_FEILD
  		plu_table_search(status_scale.departmentid, status_scale.cur_pluid,&key,0);
		if( key == 0 )
			return 0;	// Error , Cannot found PLU
		plu_get_field(key-1, PTYPE_TRACE_URL, traceUrlStr, (INT16S *)&strSize, sizeof(traceUrlStr)); // CHN Carrefour Traceablility URL
		if ( traceUrlStr[0] == 0 )
			return 0;
		tempUrlSize = strlen(traceUrlStr);
		plu_get_field(key-1, PTYPE_TRACE_CODE, traceUrlStr+tempUrlSize, (INT16S *)&strSize, sizeof(traceUrlStr)); // CHN Carrefour Traceablility Code
		traceUrlStr[tempUrlSize+strSize] = 0;
  #endif
        if(traceUrlStr[0] == 0x00) ret = 0;
	    else ret = 1;
	}
#endif


#ifdef USE_PRINT_URL_VIA_QR_BARCODE
	if( field_num == 4 )
	{
		// 터키요청 상품 조회 용 URL 구성
		// 예시) 	 https://izlenebilirlik.metro tr.com/fvizlenebilirlik.cfm?id=t210108104925a10051s28499v0520517255
		// 고정 URL 문구(60Bytes) + 판매시간(13Bytes) + 상품번호(6Bytes) + 판매자정보(17Bytes)
		//  ** 고정 URL 문구 - Indirect Ingredient Field 사용
		//  ** 판매시간 - 판매한 저울 내부 시간 사용
		//  ** 상품번호, 판매자정보 - Name3 Field 에 저장한 String
		
		bar_type = QRCODE; //Barcode Feild Type이 '4' 일 때, 1261 입력 값 상관없이 호출 된 PLU의 Direct Ingredient 를 QR 바코드로 출력 
		
		plu_table_search(status_scale.departmentid, status_scale.cur_pluid,&key,0);
		if( key == 0 )
			return 0;	// Error , Cannot found PLU

		plu_get_field(key-1, PTYPE_INGREDIENT_NO, (INT8U *)&indirect_ingredient_no, (INT16S *)&usize, sizeof(indirect_ingredient_no));  // Indirect Ingredient (고정 URL 문구)
		ingredient_get_data(indirect_ingredient_no, string_buf2, 150);
		if( string_buf2[0] == 0 )
			return 0;
		tempUrlSize = strlen(string_buf2);

		sprintf(saletime_data, "t%02d%02d%02d%02d%02d%02d", PrtComStr.time.year, PrtComStr.time.month, PrtComStr.time.date, PrtComStr.time.hour, PrtComStr.time.min, PrtComStr.time.sec);
		saletime_data[14] = 0;	// 판매시간 (저울 내부시간 사용)

		sprintf(string_buf2+tempUrlSize, "%s", saletime_data);
		tempUrlSize = strlen(string_buf2);
		
		plu_get_field(PrtItemStr.plu_addr-1, PTYPE_PLU_NAME3, string_buf2+tempUrlSize, (INT16S *)&usize, sizeof(name3_string)); // name3 (상품번호, 판매자정보)
		if( string_buf2[tempUrlSize] == 0 )
			return 0;
		string_buf2[tempUrlSize+usize] = 0;

		ret = 1;
	}
#endif
#ifdef USE_REGIST_DISPOSAL
	if (status_scale.Plu.sales_msg == TRANS_SAMPLE) {
		ret = 0;// 바코드는 만들어서 전송 하되, 인쇄는 안 함.

		len = strlen(bar_str);
		if(len > sizeof(BarcodeData)) len = sizeof(BarcodeData);
		memcpy(BarcodeData, bar_str, len);
		BarcodeData[len]=0;
	}
#endif

#ifdef USE_QR_ADDTOTAL
	if(QrAddTotalFlag) //QR ADD 토탈이 활성화 되어 있을 경우
	{
		get_nvram_sparam(NVRAM_QR_ADD_DATA,QRtemp,sizeof(QRtemp)); // 이전에 NVRAM에 저장 된 QR 바코드 DATA 로딩
		if(TotalQRFlag == 1)
		{
			if(QRtemp[0] == 0x00) // QR 코드를 인쇄 했는지를 체크
			{
				ret = 0;
				TotalQRFlag = 0; //20160203
			}
			else ret = 1; 		
		}

//		if (AddSaleFlag == ON) // QR바코드에 판매내역 저장부분
//		//if (Operation.transAddup == ON) // QR바코드에 판매내역 저장부분
//		{
//			strcpy(backup_bar_str, bar_str);
//			if(barcode_type == 0x01) //checksum 추가 부분 
//			{
//				csdigit = BarUpc_Checksum(bar_str, strlen(bar_str));
//				cslength = strlen(backup_bar_str);
//				backup_bar_str[cslength] = csdigit;
//				backup_bar_str[cslength+1] = '\0';
//			}
//			strcat(QRtemp,backup_bar_str);  // QR temp 의 포인터가 증가하며 이전에 추가한 다음에 DATA를 추가
//			strcat(QRtemp,"\r\n");
//			set_nvram_sparam(NVRAM_QR_ADD_DATA,QRtemp,sizeof(QRtemp)); //20160221
//		}
			}
#endif

	if(ret)
	{
		if (!plu_check_inhibit_ptype(PTYPE_EXT_BARCODE)) {
			if (ret==2) {
				bar_type=barcodetype_codevalue[1];
			}
		}
		if(prt_mode == LABEL) 
		{	
#ifdef USE_QR_ADDTOTAL
			if(TotalQRFlag == 1)
			{
				//QRtemp = get_nvram_bparam(NVRAM_QR_ADD_DATA);
  #ifdef USE_PRINT_URL_VIA_QR_BARCODE
  				if( field_num != 4 )
  				{
  #endif
				get_nvram_sparam(NVRAM_QR_ADD_DATA,QRtemp,sizeof(QRtemp));  // 저장한 QR 바코드의 데이터를 불러와서
				LabCmd_DrawBarInField(field_num, bar_type, (char *)QRtemp); // QR바코드를 그림
  #ifdef USE_PRINT_URL_VIA_QR_BARCODE
  				}
  #endif
  				
//				for (i=0;i<1000;i++) {
//					QRtemp[i] ='\0';										// Total QR바코드를 그렸다는 표시
//				}
//				set_nvram_sparam(NVRAM_QR_ADD_DATA,QRtemp,sizeof(QRtemp));  // 표시를 저장
//				set_nvram_bparam(NVRAM_QR_ADD_COUNT, 0);
				}
			else
			{
  #ifdef USE_PRINT_URL_VIA_QR_BARCODE
  				if( field_num != 4 )
  				{
  #endif
				LabCmd_DrawBarInField(field_num, bar_type, bar_str);
  #ifdef USE_PRINT_URL_VIA_QR_BARCODE
  				}
  #endif
			}
#elif defined(USE_CHN_EMART_BIRDS_TRACE)
			if(bar_type == QRCODE)
			{
				LabCmd_DrawBarInField(field_num, bar_type, tmpQrTrace);
			}
			else
			{
				LabCmd_DrawBarInField(field_num, bar_type, bar_str);
			}
#else
			LabCmd_DrawBarInField(field_num, bar_type, bar_str);
#endif


#ifdef USE_CARREFOUR_TRACE_QR_BARCODE
			if( field_num == 3 )
			{
				LabCmd_DrawBarInField(field_num, bar_type, traceUrlStr);
			}
#elif defined( USE_PRINT_URL_VIA_QR_BARCODE )
			if( field_num == 4 )
			{
				LabCmd_DrawBarInField(field_num, bar_type, string_buf2);
			}
#endif
			if (field_num == 1)
			{
				if (StrFieldStruct[FIELD_ID_BARCODE].ExistField) 
				{
#ifdef USE_LOTTEMART_BARCODE_FUNCTION
  #ifdef USE_LOTTESUPER_BARCODE_FUNCTION													//HRI 영역 표기를 위해
					if(strlen(bar_str) > PACKING_LABEL_BARCODE_DATA_LEN)
					{
						strncpy(bar_str2, &bar_str[IDX_OF_WT_CO_BARCODE_HRI_DATA], LEN_OF_WT_CO_BARCODE_HRI_DATA);			  // 중량 또는 수량(5자리) 시작 위치
						strncpy(bar_str3, &bar_str[IDX_OF_PRICE_BARCODE_HRI_DATA], LEN_OF_PRICE_BARCODE_HRI_DATA);			  // 금액 (6자리)시작 위치
						strncpy(bar_str4, &bar_str[IDX_OF_PRODNO_BARCODE_HRI_DATA], LEN_OF_PRODNO_BARCODE_HRI_DATA);			  // 품전 ('2'+5자리)시작 위치
						
						for(i = 0 ; i < (BARCODE_FORMAT_LEN + 1) ; i++)
						{
							bar_str[i] = NULL;											  	 // bar string 초기화
						}
						
						strncat(bar_str, bar_str4, LEN_OF_PRODNO_BARCODE_HRI_DATA);			  // '2'포함 품번 (5자리)
						strncat(bar_str, bar_str3, LEN_OF_PRICE_BARCODE_HRI_DATA);			  // 품번 다음 자리 부터 가격데이터 (6자리)
						strncat(bar_str, bar_str2, LEN_OF_WT_CO_BARCODE_HRI_DATA);			  // 판매코드 다음 자리부터 중량 또는 수량 데이터 (5자리)
					}
					if(strlen(bar_str) != PACKING_LABEL_BARCODE_DATA_LEN)				  // 보관라벨 제외 위해
					{
						bar_str[strlen(bar_str)] = BarUpc_Checksum(bar_str, strlen(bar_str));	//checksum추가
						bar_str2[0] ='\0';
						strcpy(bar_str2, bar_str);
						if(strlen(bar_str) == LEN_OF_PRODNO_BARCODE_HRI_DATA+LEN_OF_PRICE_BARCODE_HRI_DATA+LEN_OF_WT_CO_BARCODE_HRI_DATA+1) //HRI Data 총길이 + checksum 1digit
						{
							for(i=0;i<strlen(bar_str);i++)																					// HRI 영역 표현 시 띄어 쓰기를 위해
							{
								if( (i == 1) 																								// 품번 앞 '2' 뒤 뜨어쓰기
									|| (i == LEN_OF_PRODNO_BARCODE_HRI_DATA) 																// 품번 5자리 뒤 뜨어쓰기
									|| (i == LEN_OF_PRODNO_BARCODE_HRI_DATA+LEN_OF_PRICE_BARCODE_HRI_DATA) 									// 가격 6자리 뒤 뜨어쓰기
									|| (i == LEN_OF_PRODNO_BARCODE_HRI_DATA+LEN_OF_PRICE_BARCODE_HRI_DATA+LEN_OF_WT_CO_BARCODE_HRI_DATA) )	// 무게/수량 정보 5자리 뒤 뜨어쓰기
								{	
									bar_str[i+j] = ' ';
									j++;
									bar_str[i+j] = bar_str2[i];
								}
								else
								{
									bar_str[i+j] =	bar_str2[i];
								}

							}
						}
					}
  #else //USE_LOTTESUPER_BARCODE_FUNCTION
					if(strlen(bar_str) > 24)
					{
						for(i=0;i<8;i++)
						{
							bar_str4[i] = '0';
						}
						strcpy(bar_str2, &bar_str[31]);
						strcpy(bar_str3, &bar_str[42]);

						for(i = 16 ; i < (BARCODE_FORMAT_LEN + 1) ; i++)
						{
							bar_str[i] = NULL;
						}

						if((strlen(bar_str2)== 22) || (strlen(bar_str2)== 26))
						{
							strncat(bar_str, bar_str2, 10);
							strncat(bar_str, bar_str3, 4);
							strncat(bar_str, bar_str4, 8);

							for(i=38;i<46;i++)
							{
								if(i == 45) bar_str[i] = 0x00;
								else bar_str[i] = bar_str3[i-34];
							}
						}
						else
						{
							strncat(bar_str, bar_str2, 14);
							strncat(bar_str, bar_str4, 8);
							for(i=38;i<46;i++)
							{
								if(i == 45) bar_str[i] = 0x00;
								else bar_str[i] = bar_str2[i-24];
							}
						}
					}
					if(strlen(bar_str) != 24) //보관라벨 제외 위해
					{
						bar_str[strlen(bar_str)] = BarUpc_Checksum(bar_str, strlen(bar_str));	//checksum추가
						bar_str2[0] ='\0';
						strcpy(bar_str2, bar_str);
						if(strlen(bar_str) == 46)
						{
							for(i=0;i<46;i++)
							{
								if(i%5 == 0)
								{
									bar_str[i+j] = 0x20;
									j++;
									bar_str[i+j] = bar_str2[i];
								}
								else
								{
									bar_str[i+j] =  bar_str2[i];
								}

							}
						}

					}
  #endif //USE_LOTTESUPER_BARCODE_FUNCTION
#elif defined(USE_TOPMART_BARCODE_FUNCTION)
				  if (bar_type == RSSEXP)
				  {
					  for(i = 20 ; i < (BARCODE_FORMAT_LEN + 1) ; i++)	  //2단 바코드 20자리까지 출력
					  {
						  bar_str[i] = NULL;
					  }
				  }
#endif
					LabCmd_SetTextField(FIELD_ID_BARCODE, bar_str);
				}
			}            
		} 
		else // RECEIPT MODE
		{
#ifdef USE_QR_ADDTOTAL
			if(TotalQRFlag == 1)
			{
				get_nvram_sparam(NVRAM_QR_ADD_DATA,QRtemp,sizeof(QRtemp));  // 저장한 QR 바코드의 데이터를 불러와서
				RcptCmd_PrintBarcode(bar_type, (char *)QRtemp);
//				QRtemp[0] ='\0';											// Total QR바코드를 그렸다는 표시
//				set_nvram_sparam(NVRAM_QR_ADD_DATA,QRtemp,sizeof(QRtemp));  // 표시를 저장
//				set_nvram_bparam(NVRAM_QR_ADD_COUNT, 0);
//				TotalQRFlag = 0;
//				AddSaleFlag = 1;
			}
			else
			{
					RcptCmd_PrintBarcode(bar_type, bar_str);
				}
#else
			RcptCmd_PrintBarcode(bar_type, bar_str);
#endif
		}
	}
#ifdef USE_QR_ADDTOTAL
	else	// 티켓 출력 모드에서 각 항목별 바코드 인쇄 시 첫번째 아이템 바코드가 출력 되지 않아서 추가한 부분
	{
		if ((prt_mode == RECEIPT) && (GlbFloat.printBarcodeForList))
		{
			RcptCmd_PrintBarcode(bar_type, bar_str);
		}
	}
#endif

	return ON;
}


#ifdef USE_QR_ADDTOTAL
INT8U GenerateQrTotalStringbyTransaction(INT8U mode, unsigned char ClerkNum)
{
	INT16U plu_key;
	INT8U bar_type;
	INT8U barcode_type;
	char bar_form[BARCODE_FORMAT_LEN+1];
	char bar_str[BARCODE_FORMAT_LEN+1];
	STRUCT_STRFORM_PARAM bar_param;
	INT16U barid;
	INT16U length;
	char QRtemp[1000]={0};
	INT8U ret;
	
	TRANSACT_CHAIN  *cp;
	TRANSACT_CHAIN  tc;
	INT16U h_index, next;
 
	if ((mode == 0) || (mode==2)) 
	{
        h_index = get_nvram_lparam(NVRAM_TRANS_HEAD+ClerkNum*4);
		if (h_index)
		{
			next = h_index;
			get_nvram_trans_addr(NVRAM_TRANS_DATA, h_index - 1, &tc);
			cp = &tc;
		}
		else
		{
			cp = 0;
		}
	} 
#ifndef USE_TRACE_STANDALONE	// 개체기억기능 영역공유 
	else if (mode==1) 
	{
		if (ClerkLastHead)
		{
            next = ClerkLastHead;
            get_nvram_trans_addr(NVRAM_CLEARK_LASTSALE, ClerkLastHead - 1, &tc);
            cp = &tc;
        }
        else
        {
            cp = 0;
        }
	}
#endif
	else if (mode == 3) //SG070214
	{
		cp = (TRANSACT_CHAIN *)&TrChain;
	}

	if (QrAddTotalFlag) //QR ADD 토탈이 활성화 되어 있을 경우
	{
		QRtemp[0] = 0;
		strcpy(QRtemp, QrCfg_OpeningCommand);
		strcat(QRtemp, QrCfg_PosId);
	}
  	for(;cp;)
	{
		if(!cp->delete)
		{
			plu_table_search(cp->content.trtDept, cp->content.trtPLU, &plu_key, 0);
			if(plu_key || cp->content.trtPLU >= MISC_PLU_WEIGHT) 
			{
				barid = 0;
				if (plu_key) {
					barid = prt_get_plu_barid(cp->content.trtPLU, plu_key, cp->content.trtPLUtype, 1);	// Modified by CJK 20050503
				}
				prt_set_strform_parameter_item(&bar_param, cp);

				if (barid < 1 || barid > 99) barid = 0;
				if (!get_barcodeformat(barid, (char *)bar_form, &barcode_type)) return OFF;

				bar_type=barcodetype_codevalue[barcode_type];
				bar_str[0] = 0;
				ret = 0;
				if (bar_type != NO_BARCODE) ret = prt_make_barform(bar_str, bar_form, &bar_param, bar_type);
				if (!ret) return OFF;
				if (QrAddTotalFlag) //QR ADD 토탈이 활성화 되어 있을 경우
				{
					//get_nvram_sparam(NVRAM_QR_ADD_DATA, QRtemp, sizeof(QRtemp)); // 이전에 NVRAM에 저장 된 QR 바코드 DATA 로딩
					strcat(QRtemp, QrCfg_EanPrefix);
					if (barcode_type == 0x01) //checksum 추가 부분 
					{
						length = strlen(bar_str);
						bar_str[length] = BarUpc_Checksum(bar_str, length);
						bar_str[length+1] = '\0';
					}
					strcat(QRtemp, bar_str);  // QR temp 의 포인터가 증가하며 이전에 추가한 다음에 DATA를 추가
//				strcat(QRtemp, "\r\n");
					strcat(QRtemp, QrCfg_EanSuffix);
//				set_nvram_sparam(NVRAM_QR_ADD_DATA, QRtemp, sizeof(QRtemp)); //20160221
				}
			}
			else 
				return OFF;
		}
		if(cp->next) {
			if(mode == 0) {
				next = cp->next;
				get_nvram_trans_addr(NVRAM_TRANS_DATA, next - 1, &tc);
				cp = &tc;
			} else {
				next = cp->next;
				get_nvram_trans_addr(NVRAM_CLEARK_LASTSALE, next - 1, &tc);
				cp = &tc;
			}
		} else {
			break;
		}
	}
	if (QrAddTotalFlag) //QR ADD 토탈이 활성화 되어 있을 경우
	{
		strcat(QRtemp, QrCfg_ClosingCommand);
		set_nvram_sparam(NVRAM_QR_ADD_DATA, QRtemp, sizeof(QRtemp));
	}
}
#endif

INT8U ExistShopName(INT16U id, INT32U *addr)
{
	//INT32U addr;

	*addr =  DFLASH_BASE + FLASH_SHOPNAME_AREA;
	*addr =  *addr + SHOPNAME_STRUCT_SIZE * (INT32U)(id-1);
	if(PrtComStr.shop_name_no != Flash_wread(*addr)) return OFF;
	return ON;
}

void GetOriginName(INT16U id, char *str)
{
	INT32U addr;

	addr =  DFLASH_BASE + FLASH_ORIGIN_AREA;
	addr =  addr + ORIGIN_STRUCT_SIZE * (INT32U)(id - 1);
	addr += 2;
	Flash_sread(addr, (HUGEDATA INT8U *)str, ORIGIN_STRUCT_SIZE-2);
}

#ifdef USE_GSMART_PLU
INT8U PrintPluName_GSMart(INT8U field_num, INT32U plu_addr)
{
	char string_buf[800];
	INT16U	i, y, len, h, tot_h, start_y;
	INT16S  k;
	INT8U fontSizeNo, fh, fw;
	CWD cwd;
	INT8U lineNo;
	INT16U nameHeight, lineSpace;
	INT8U fontSize[8];
	INT8U fontJust[8];
	INT8U lastLine;
	INT16U space;
	INT8U space_idx;
	//INT8U	magX_idx;
#ifdef USE_TRACE_STANDALONE
	INT16U id;
	INT32U addr;
	INT8U  len_use;
	INT8U	meatUseFlag;
	meatUseFlag = OFF;
#endif
//	x = StrFieldStruct[field_num].sx;
	y = StrFieldStruct[field_num].sy;
	tot_h = StrFieldStruct[field_num].StrHeight;
	fh = Font1Struct[StrFieldStruct[field_num].Id1].Hbits1;
	
	lineNo = 0;
	nameHeight = 0;
	lastLine = 0;

#ifdef USE_TRACE_STANDALONE	
	if (ModeIndiv == 5 && CurTraceStatus.meatUse) meatUseFlag = ON;
#endif
	 
	for (k = 7; k >= 0; k--)
	{
		plu_get_field(plu_addr-1, PluNameFontSizeId[k], (INT8U *)&fontSizeNo, (INT16S *)&len, sizeof(fontSizeNo));
		if (fontSizeNo == 0) 
		{
			fontSizeNo = get_base_bparam(FLASH_PLU_FONT_SIZE + k);
		}
		switch(fontSizeNo)
		{
			default:
			case 1: 
				fontSize[k] = 1;	// 1배
				fontJust[k] = 0;	// left align
				if (k == 0) fontJust[k] = 2;	// 1 line은 항상 center align
				break;
			case 2: 
				fontSize[k] = 2;	// 2배
				fontJust[k] = 2;	// center align
				break;
			case 3: 
				fontSize[k] = 4;	// 4배
				fontJust[k] = 2;	// center align
				break;
		}

#ifdef USE_GSMART_TRACE_FUNCTION
		if (k >= 3 && k <= 7) fontJust[k] = 0;	// left align for GS Mart. 상품명 4~8단
#endif

		plu_get_field(plu_addr-1, PluNameId[k], (INT8U *)&string_buf, (INT16S *)&len, sizeof(string_buf));
		if (!lastLine && string_buf[0] == 0)
		{
			;
		}
		else 
		{
			if (!lastLine)
			{
				lastLine = k + 1;
			}
			lineNo++;
			nameHeight += (fh * fontSize[k]);
		}
	}
#ifdef USE_TRACE_STANDALONE
	if (meatUseFlag == ON) 
	{
		lineNo++;
		nameHeight += (fh * fontSize[lastLine]);
		lastLine++;
	}
#endif
	if (tot_h >= nameHeight) 
		lineSpace = tot_h - nameHeight;
	else
		lineSpace = 0;
	lineSpace /= (lineNo + 1);

	i = 0;
	start_y = y;
	y += lineSpace;	// 세로 정렬	

	for (k = 0; k < 8; k++)
	{
		h = fh * fontSize[k];// + StrFieldStruct[field_num].LineSp;
		space = StrFieldStruct[field_num].InterChSp * fontSize[k];

		string_buf[i++] = _ESC;
		string_buf[i++] = 'S';
		string_buf[i++] = '{';
		string_buf[i++] = field_num;
		string_buf[i++] = '\0';
		string_buf[i++] = ',';
		string_buf[i++] = 'G';
		string_buf[i++] = PrtFontType;//1=double, 0=one byte font

		// position-y
		string_buf[i++] = ',';
		string_buf[i++] = 'Y';
		cwd.word = y;
		string_buf[i++] = cwd.byte.left;
		string_buf[i++] = cwd.byte.right;
		// height
		string_buf[i++] = ',';
		string_buf[i++] = 'H';
		cwd.word = h;//StrFieldStruct[field_num].StrHeight;
		string_buf[i++] = cwd.byte.left;
		string_buf[i++] = cwd.byte.right;
		// magnitude-x
		string_buf[i++] = ',';
		string_buf[i++] = 'M';
		string_buf[i++] = fontSize[k];
		// magnitude-y
		string_buf[i++] = ',';
		string_buf[i++] = 'N';
		string_buf[i++] = fontSize[k];
		// justification
		string_buf[i++] = ',';
		string_buf[i++] = 'J';
		string_buf[i++] = fontJust[k];
		// intercharacter spacing
		string_buf[i++] = ',';
		string_buf[i++] = 'P';
		string_buf[i++] = space;
		space_idx = i - 1;

		string_buf[i++] = ',';
		string_buf[i++] = '[';

#ifdef USE_TRACE_STANDALONE
	       if (meatUseFlag && k == (lastLine - 1))//용도
		{
			addr =  DFLASH_BASE + FLASH_TRACE_SPECIALUSE_AREA;
			addr =  addr + sizeof(TRACE_SPECIALUSE_STRUCT) * (INT32U)(CurTraceStatus.meatUse-1);
			id = Flash_wread(addr);
			if(CurTraceStatus.meatUse == id)
			{
				addr += 2;
				len_use = sizeof(TRACE_SPECIALUSE_STRUCT)-2;
				Flash_sread(addr, (HUGEDATA INT8U *)string_buf+i, len_use);
				string_buf[i + len_use] = 0;
				len_use = strlen(&string_buf[i]);
				i += len_use;
				len += len_use;
			}
		}
		else 
		{
#endif
			plu_get_field(plu_addr-1, PluNameId[k], (INT8U *)&string_buf+i, (INT16S *)&len, sizeof(string_buf)-i);
			string_buf[i + len] = 0;
			len = strlen(&string_buf[i]);
			i += len;
#ifdef USE_TRACE_STANDALONE
		}		

		if (ModeIndiv != 5 && k == 0)	// 상품명 1단
		{
			if(CurTraceStatus.meatUse)
			{
				addr =  DFLASH_BASE + FLASH_TRACE_SPECIALUSE_AREA;
				addr =  addr + sizeof(TRACE_SPECIALUSE_STRUCT) * (INT32U)(CurTraceStatus.meatUse-1);	// code-1 (?)
				id = Flash_wread(addr);
				if(CurTraceStatus.meatUse == id)
				{
					string_buf[i++] = ' ';
					len++;
					addr += 2;
					len_use = sizeof(TRACE_SPECIALUSE_STRUCT)-2;
					Flash_sread(addr, (HUGEDATA INT8U *)string_buf+i, len_use);
					string_buf[i + len_use] = 0;
					len_use = strlen(&string_buf[i]);
					i += len_use;
					len += len_use;
				}
			}
		}	       
#endif
		if (fontJust[k] == 2)
		{
			fw = Font1Struct[StrFieldStruct[field_num].Id1].Wbits1;
			fw *= fontSize[k];
			if (StrFieldStruct[field_num].StrWidth > len * fw)
			{
				space = StrFieldStruct[field_num].StrWidth - len * fw;
			}
			else
			{
				space = 0;
			}
			space /= (len + 1);
			if (space > 127) space = 127;
			string_buf[space_idx] = (INT8U)space;
		}		
		string_buf[i++] = '\0';
		if (k + 1 == lastLine) break;		

		y += h;
		y += lineSpace;

		if (y - start_y > tot_h) break;
	}
	PutDataRxBuf(&CommBufPrt, string_buf, i);

	return ON;
}

#ifdef USE_TRACE_STANDALONE
INT8U PrintMultiName_GSMart(INT8U field_num, INT8U mode) //gm 090422
{
	//mode 0: 개체번호, 1: 도축장, 2: 개체번호+도축장
	INT8U 	string_buf[800];
	INT16U	i, y, len, h, tot_h;
	INT16S  k;
	INT8U 	fh;
	CWD cwd;
	INT16U nameHeight, lineSpace;
	INT8U fontSize;
	INT32U addr;
	INT8U	fontMagY;
	INT8U	cnt;
	INT16U 	v16_2;
	INT16U	tempSize;

	y = StrFieldStruct[field_num].sy;
	tot_h = StrFieldStruct[field_num].StrHeight;
	fh = Font1Struct[StrFieldStruct[field_num].Id1].Hbits1;
	fontMagY = StrFieldStruct[field_num].Magy;
	
	nameHeight = 0;
	lineSpace = 0;
	cnt = 0;

	nameHeight = fh*fontMagY*MultiIndex;

	while (1) 
	{
		if (tot_h >= nameHeight) 
		{
			lineSpace = tot_h - nameHeight;
			break;
		}
		else 
		{
			cnt++;
			fontMagY--;			
			nameHeight = fh*fontMagY*MultiIndex;
		}
	}
	lineSpace /= 2;//(MultiIndex + 1);	
	fontSize = fontMagY;
	i = 0;
	y += lineSpace;	// 세로 정렬
	for (k = 0; k < MultiIndex; k++)    //max = 3
	{
		h = fh * fontSize;// + StrFieldStruct[field_num].LineSp;

		string_buf[i++] = _ESC;
		string_buf[i++] = 'S';
		string_buf[i++] = '{';
		string_buf[i++] = field_num;
		string_buf[i++] = '\0';
		string_buf[i++] = ',';
		string_buf[i++] = 'G';
		string_buf[i++] = PrtFontType;//1=double, 0=one byte font

		// position-y
		string_buf[i++] = ',';
		string_buf[i++] = 'Y';
		cwd.word = y;
		string_buf[i++] = cwd.byte.left;
		string_buf[i++] = cwd.byte.right;
		// height
		string_buf[i++] = ',';
		string_buf[i++] = 'H';
		cwd.word = h;//StrFieldStruct[field_num].StrHeight;
		string_buf[i++] = cwd.byte.left;
		string_buf[i++] = cwd.byte.right;
		// magnitude-y
		string_buf[i++] = ',';
		string_buf[i++] = 'N';
		string_buf[i++] = fontSize;

		string_buf[i++] = ',';
		string_buf[i++] = '[';

		switch(mode)
		{
			case 0:
				sprintf((HUGEDATA char *)&string_buf[i], (HUGEDATA char *)IndivdualMultiData[k].indivNoStr);
				len = strlen((HUGEDATA char *)&string_buf[i]);
				i += len;  
				break;
			case 1:
				addr =  DFLASH_BASE + FLASH_TRACE_SLAUGHTERHOUSE_AREA;
				addr =  addr + TRACE_SLAUGHTERHOUSE_KR_STRUCT_SIZE * (INT32U)(IndivdualMultiData[k].slaughterHouse-1);	// code-1 (?)
				v16_2 = Flash_wread(addr);
			#ifdef USE_KOR_SLAUGHT_CODE_4DIGIT
				if(trace_slaughthouse_index2no(IndivdualMultiData[k].slaughterHouse) != v16_2)
			#else
				if(IndivdualMultiData[k].slaughterHouse != v16_2)
			#endif
				{
					break;
				}
				addr += 2;
			#ifdef USE_KOR_SLAUGHT_CODE_4DIGIT
				sprintf((char*)&string_buf[i], "도축장명:");
				i += strlen((char*)&string_buf[i]);
			#endif
				tempSize = TRACE_SLAUGHTHOUSE_NAME_SIZE;
				Flash_sread(addr, (HUGEDATA INT8U *)&string_buf[i], tempSize);
				string_buf[i+tempSize-1] = 0;
				len = strlen((HUGEDATA char *)&string_buf[i]);
				i += len;  
				break;
			case 2:
				sprintf((HUGEDATA char *)&string_buf[i], (HUGEDATA char *)IndivdualMultiData[k].indivNoStr);
				len = strlen((HUGEDATA char *)&string_buf[i]);				
				i += len;

				if (len) string_buf[i++] = ' ';

				addr =  DFLASH_BASE + FLASH_TRACE_SLAUGHTERHOUSE_AREA;
				addr =  addr + TRACE_SLAUGHTERHOUSE_KR_STRUCT_SIZE * (INT32U)(IndivdualMultiData[k].slaughterHouse-1);	// code-1 (?)
				v16_2 = Flash_wread(addr);
			#ifdef USE_KOR_SLAUGHT_CODE_4DIGIT
				if(trace_slaughthouse_index2no(IndivdualMultiData[k].slaughterHouse) != v16_2)
			#else
				if(IndivdualMultiData[k].slaughterHouse != v16_2)
			#endif
				{
					break;
				}
				addr += 2;
				tempSize = TRACE_SLAUGHTHOUSE_NAME_SIZE;
				Flash_sread(addr, (HUGEDATA INT8U *)&string_buf[i], tempSize);
				string_buf[i+tempSize-1] = 0;
				len = strlen((HUGEDATA char *)&string_buf[i]);
				i += len; 
				break;
		}
		string_buf[i++] = '\0';

		y += h;
	}
	PutDataRxBuf(&CommBufPrt, (HUGEDATA char *)string_buf, i);

	return ON;
}
#endif
#endif

void prtfield_generate_common_item(void)
{
	INT16S fieldid;
	INT32U addr;
	INT8U select,v8_1,v8_2,v8_3;
	INT16U v16_1, v16_2, v16_3, v16_4, v16_5;
	INT16U t16_3, t16_4, t16_5;
	INT16U ingredSize,days;
	INT32U v32_1;
	INT8U sellbydate_check,prepack_type,time_print_type;
	char string_buf[128];
#ifdef USE_PRT_DISC_PERCENT
	char disc_buf[40];
	long discTemp;
	INT8U i, name2Size;
	INT8U discTensPlace, discUnitsPlace;
#endif
	INT8U wt_prt_type, DirectSetField;
	char *addrp;
	INT8U dec_ch;
	char w_sign1[5], w_sign2[5];
	INT8U changeIndivString;
	INT8U ingredientBuf[MAX_INGREDIENT_SIZE];
#if defined(USE_EMART_MEATGRADE_PRINT) || defined(USE_MARBLING_GRADE_PRINT)
	INT8U len;
#endif

#ifdef USE_TRACE_STANDALONE
	INT16U tempSize;

#endif
#ifdef USE_DIRECT_INGREDIENT
	INT16U ingredNo[DIRECT_INGREDIENT_BLOCK_NUM];
	INT8U flagDirectIngred=0;
	char string_buf2[DEFAULT_INGREDIENT_TEXT_ALL_SIZE];
	#define INGREDIENT_FIELD_NUM 3		// Direct Ingredient 정보를 라벨폼 필드상 몇개의 필드로 나눌 지 숫자를 정의 
	int len[INGREDIENT_FIELD_NUM + 1], start, end, i, j;
	memset(len, 0, sizeof(len));
#endif
#ifdef USE_USA_FIXEDWEIGHT_UNIT_OZ
	INT8U rate=16;
	char string_buf1[128];
#endif
#ifdef USE_PERCENT_WEIGHT
	INT8U precentweight_mode;
#endif
#if defined(USE_9999_INDIRECT_INGREDIENT) || defined(USE_INGREDIENT_CODE_4DIGIT)
	INT16U 	ing_id;
#endif

	dec_ch = get_global_bparam(GLOBAL_DECIMAL_SIGN);
	if(PrtWeightUnit == WEIGHT_UNIT_KG || PrtWeightUnit == WEIGHT_UNIT_G) 
	{
		get_global_sparam(GLOBAL_WEIGHT_SYMBOL_1, (INT8U *)w_sign1, 4);
		get_global_sparam(GLOBAL_WEIGHT_SYMBOL_2, (INT8U *)w_sign2, 4);
	} 
	else 
	{
		get_global_sparam(GLOBAL_WEIGHT_LBSYMBOL_1, (INT8U *)w_sign1, 4);
		get_global_sparam(GLOBAL_WEIGHT_LBSYMBOL_2, (INT8U *)w_sign2, 4);
	}
	w_sign1[4] = 0;
	w_sign2[4] = 0;

	sellbydate_check= get_global_bparam(GLOBAL_SALE_SETUP_FLAG5)&0x01;
	prepack_type  = get_global_bparam(GLOBAL_SALE_SETUP_FLAG5);
	prepack_type>>=1;
	prepack_type&=0x03;
	//SG061218. PackOnTime, SellbyTime에 대한 인쇄 포맷 설정
	//0=12:45(time), 1=A,B,..(alphabet), 2=1,2,..(numeric) format
	time_print_type  = get_global_bparam(GLOBAL_SALE_SETUP_FLAG6); 
	time_print_type>>=5;
	time_print_type&=0x03;
	wt_prt_type = 0;
	changeIndivString = get_global_bparam(GLOBAL_SALE_SETUP_FLAG12);
	changeIndivString &= 0x20;
//	for (fieldid=0; fieldid<FIELD_ID_N_SERVING_SIZE; fieldid++) {
	for (fieldid=0; fieldid<MAX_STR_FIELD_NUM; fieldid++) {
		string_buf[0] = 0;
		DirectSetField = OFF;
		select = prtsel_get_table(fieldid);
		if (StrFieldStruct[fieldid].ExistField && select) {
#ifdef USE_SETPRODUCT_SALE
			if(SetproductSaleMode && setproduct_PrtFlag)
				if(fieldid != FIELD_ID_NAME1) continue;
#endif
			switch (fieldid) {
				case FIELD_ID_DEPT_NAME:	// department
					switch(PrtItemStr.l_PluNo) {
						case MISC_PLU_WEIGHT :
						case MISC_PLU_PCS    :
						case MISC_PLU_COUNT  :
						case 0:
							v16_1 = 0;
							break;
						default :
							plu_get_field(PrtItemStr.plu_addr-1, PTYPE_DEPARTMENT_NO, (INT8U *)&v16_1, (INT16S *)&v16_2,sizeof(v16_1));
							break;
					}
					if (v16_1) {
						addr =  DFLASH_BASE + FLASH_DEPARTMENT_AREA;
						addr =  addr + DEPARTMENT_STRUCT_SIZE * (INT32U)(v16_1-1);	// code-1 (?)
						v16_2 = Flash_wread(addr);
						if(v16_1 != v16_2) continue;	// Data 가 존재하지않는 경우
						addr += 4;
						Flash_sread(addr, (HUGEDATA INT8U *)string_buf, sizeof(DEPARTMENT_STRUCT)-4);
					} else {
						continue;
					}
					break;
				case FIELD_ID_DEPT_NO:	// department
					switch(PrtItemStr.l_PluNo) {
						case MISC_PLU_WEIGHT :
						case MISC_PLU_PCS    :
						case MISC_PLU_COUNT  :
						case 0:
							v16_1 = 0;
							break;
						default :
							plu_get_field((INT16S)PrtItemStr.plu_addr-1, PTYPE_DEPARTMENT_NO, (INT8U *)&v16_1, (INT16S *)&v16_2,sizeof(v16_1));
							break;
					}
					sprintf(string_buf,"%d",v16_1);
					break;
				case FIELD_ID_PLU_NO:	// number
					sprintf(string_buf,"%ld",PrtItemStr.l_PluNo);
#ifdef USE_EMART_NETWORK
					if (Operation.checkin)
					{
						strcat(string_buf, " P");
					}
#endif
					break;
				case FIELD_ID_NAME:	// Name1~Name3, Name4~6 is not inclueded.
					switch(PrtItemStr.l_PluNo) {
						case MISC_PLU_WEIGHT :
							receipt_caption_read(RCPTCAP_ID_COM_MISC_WEIGHT, string_buf);
							break;
						case MISC_PLU_COUNT  :
							receipt_caption_read(RCPTCAP_ID_COM_MISC_COUNT, string_buf);
							break;
						case MISC_PLU_PCS    :
							receipt_caption_read(RCPTCAP_ID_COM_MISC_PCS, string_buf);
							break;
						case 0:
							continue;
						default :
							#if (defined(USE_GSMART_PLU) && !defined(USE_TOPMART_DEFAULT))
							PrintPluName_GSMart(FIELD_ID_NAME, PrtItemStr.plu_addr);
							continue;
							#else
							if(!prtsel_get_table(FIELD_ID_NAME1)) {
								string_buf[0] = 0;
							} else {
								if(label_set.origin_prttype == 1 && PrtItemStr.origin) {
									GetOriginName(PrtItemStr.origin, string_buf);
								} else {
									plu_get_field(PrtItemStr.plu_addr-1, PTYPE_PLU_NAME1, (INT8U *)&string_buf, (INT16S *)&v16_2,sizeof(string_buf));
									string_buf[v16_2] = 0;
								}
							}
							v16_1 = strlen(string_buf);
							string_buf[v16_1++] = LF;
							if(!prtsel_get_table(FIELD_ID_NAME2)) {
								string_buf[v16_1] = 0;
								v16_2 = 0;
							} else {
								if(label_set.origin_prttype == 2 && PrtItemStr.origin) {
									GetOriginName(PrtItemStr.origin, string_buf + v16_1);
									v16_2 = strlen(string_buf+v16_1);
								} else {
									plu_get_field(PrtItemStr.plu_addr-1, PTYPE_PLU_NAME2, (INT8U *)&string_buf+v16_1, (INT16S *)&v16_2,sizeof(string_buf)-v16_1);
								}
							}
							v16_1 = strlen(string_buf);
							string_buf[v16_1++] = LF;
							if(!prtsel_get_table(FIELD_ID_NAME3)) {
								string_buf[v16_1] = 0;
								v16_2 = 0;
							} else {
								if(label_set.origin_prttype == 3 && PrtItemStr.origin) {
									GetOriginName(PrtItemStr.origin, string_buf + v16_1);
									v16_2 = strlen(string_buf+v16_1);
								} else {
									plu_get_field(PrtItemStr.plu_addr-1, PTYPE_PLU_NAME3, (INT8U *)&string_buf+v16_1, (INT16S *)&v16_2,sizeof(string_buf)-v16_1);
								}
							}
							string_buf[v16_1+v16_2] = 0;
							break;
							#endif
					}
					break;
				case FIELD_ID_NAME1:
					switch(PrtItemStr.l_PluNo) {
						case MISC_PLU_WEIGHT :
							receipt_caption_read(RCPTCAP_ID_COM_MISC_WEIGHT, string_buf);
							break;
						case MISC_PLU_COUNT  :
							receipt_caption_read(RCPTCAP_ID_COM_MISC_COUNT, string_buf);
							break;
						case MISC_PLU_PCS    :
							receipt_caption_read(RCPTCAP_ID_COM_MISC_PCS, string_buf);
							break;
						case 0:
#ifdef USE_SETPRODUCT_SALE
							if(SetproductSaleMode && setproduct_PrtFlag)
							{
								sprintf(string_buf,"한우선물세트");
								break;
							}
							else
								continue;
#else
							continue;
#endif
						default :
							if(label_set.origin_prttype == 1 && PrtItemStr.origin) 
							{
								GetOriginName(PrtItemStr.origin, string_buf);
							} 
							else 
							{
#ifdef USE_DIVIDING_FUNCTION
								if(status_scale.divisor)
								{
									memcpy(string_buf, DividedPluName, sizeof(DividedPluName));
									v16_2 = sizeof(DividedPluName);
								}
								else
								{
									plu_get_field(PrtItemStr.plu_addr-1, PTYPE_PLU_NAME1, (INT8U *)&string_buf, (INT16S *)&v16_2,sizeof(string_buf));
								}
#else
								plu_get_field(PrtItemStr.plu_addr-1, PTYPE_PLU_NAME1, (INT8U *)&string_buf, (INT16S *)&v16_2,sizeof(string_buf));
#endif
								string_buf[v16_2] = 0;
							}
							break;
					}
					break;
				case FIELD_ID_NAME2:
					switch(PrtItemStr.l_PluNo) {
						case MISC_PLU_WEIGHT :
						case MISC_PLU_PCS   :
						case MISC_PLU_COUNT  :
						case 0:
							continue;
						default :
							if(label_set.origin_prttype == 2 && PrtItemStr.origin) {
								GetOriginName(PrtItemStr.origin, string_buf);
							} else {
								plu_get_field(PrtItemStr.plu_addr-1, PTYPE_PLU_NAME2, (INT8U *)&string_buf, (INT16S *)&v16_2,sizeof(string_buf));
#ifdef USE_PRT_DISC_PERCENT
								name2Size = 40-strlen(string_buf);
								if(name2Size>40) name2Size = 0;
								if(name2Size>=5)
								{
									for(i=0;i<name2Size;i++)
									{	
										if(DiscSale.saleDiscountType==DISCOUNT_NO)	//Discount 사용X
										{
											disc_buf[i] = ' ';
										}
										else	//Discount 사용 시
										{
											discTemp = DiscSale.discIndexData.discPrice[0]/100L;	// 할인률을 저장 
											discTensPlace = discTemp / 10;							// 할인률 중 10의 자리
											discUnitsPlace = discTemp % 10;							// 할인률 중 1의 자리
											
											if(name2Size==40) disc_buf[0] = 0xA0;	//NBSP -> Name2 입력 없을 경우 (문자입력 처리)
											
											if(i==name2Size-5) disc_buf[i] = '-';
											else if(i==name2Size-4)
											{
												if (discTensPlace)	disc_buf[i] = discTensPlace + '0';	// 5% 할인의 경우 "- 5%" 로 표기  //"-05%"(X) 
												else				disc_buf[i] = ' ';
											}
											else if(i==name2Size-3) disc_buf[i] = discUnitsPlace + '0';
											else if(i==name2Size-2) disc_buf[i] = '%';
											else if(i==name2Size-1) disc_buf[i] = '\0';
											else disc_buf[i] = ' ';
										}
									}
									strcat(string_buf,disc_buf);
								}
#endif
								string_buf[v16_2] = 0;
							}
							break;
					}
					break;
				case FIELD_ID_NAME3:
					switch(PrtItemStr.l_PluNo) {
						case MISC_PLU_WEIGHT :
						case MISC_PLU_PCS    :
						case MISC_PLU_COUNT  :
						case 0:
							continue;
						default :
#ifdef USE_EMART_MINCED_MEAT
							if((CurTraceStatus.meatUse == 8)|| (CurTraceStatus.meatUse == 15)||(CurTraceStatus.meatUse == 25)||(CurTraceStatus.meatUse == 26))
							{
								sprintf(string_buf, "보관방법: -2도에서 5도 이하 냉장보관");
							} 
							else 
							{
								plu_get_field(PrtItemStr.plu_addr-1, PTYPE_PLU_NAME3, (INT8U *)&string_buf, (INT16S *)&v16_2,sizeof(string_buf));
								string_buf[v16_2] = 0;
							}
#else
							if(label_set.origin_prttype == 3 && PrtItemStr.origin) {
								GetOriginName(PrtItemStr.origin, string_buf);
							} else {
								plu_get_field(PrtItemStr.plu_addr-1, PTYPE_PLU_NAME3, (INT8U *)&string_buf, (INT16S *)&v16_2,sizeof(string_buf));
								string_buf[v16_2] = 0;
							}
#endif
							break;
					}
					break;
#ifdef USE_PRINT_TEMPERATURE
				case FIELD_ID_NAME4:
							plu_get_field(PrtItemStr.plu_addr-1, PTYPE_EXT_BARCODE, (INT8U *)&string_buf, (INT16S *)&v16_2,sizeof(string_buf)); //20151013
							string_buf[v16_2] = 0;
							break;
#endif
#if defined(USE_GSMART_PLU) || defined(USE_CHINA_BAONENG) || defined(USE_CHINA_BAONENG_COUNTRYGARDEN)
				case FIELD_ID_NAME4:
					switch(PrtItemStr.l_PluNo) {
						case MISC_PLU_WEIGHT :
						case MISC_PLU_PCS    :
						case MISC_PLU_COUNT  :
						case 0:
							continue;
						default :
					#ifdef USE_KOR_SLAUGHT_CODE_4DIGIT
						#ifdef USE_EMART_NETWORK
							if (FlagIndividualMultiStart || FlagIndividualMultiRemain)//복수개체 인쇄
							{
								PrintMultiName_GSMart(FIELD_ID_SLAUGHTER_HOUSE, 1);
								FlagIndividualMultiStart = OFF;
								FlagIndividualMultiRemain = ON;
								if (!PrtName4_With_slaughthouse) // Param 752 : N 상품명 4 단 인쇄 안 함
								continue;
							}
							else
							{
								if (!PrtName4_With_slaughthouse) // Param 752 : N 상품명 4 단 인쇄 안 함
									if (CurTraceStatus.slPlace) continue;	// 이마트 도축장 있을 경우 
							}
						#endif
					#endif                          
								plu_get_field(PrtItemStr.plu_addr-1, PTYPE_PLU_NAME4, (INT8U *)&string_buf, (INT16S *)&v16_2,sizeof(string_buf));
							string_buf[v16_2] = 0;
							break;
					}
					break;
				case FIELD_ID_NAME5:
					switch(PrtItemStr.l_PluNo) {
						case MISC_PLU_WEIGHT :
						case MISC_PLU_PCS    :
						case MISC_PLU_COUNT  :
						case 0:
							continue;
						default :
							plu_get_field(PrtItemStr.plu_addr-1, PTYPE_PLU_NAME5, (INT8U *)&string_buf, (INT16S *)&v16_2,sizeof(string_buf));
							string_buf[v16_2] = 0;
							break;
					}
					break;
#if (PLU_COMMODITY_LINE_NO == 8)
				case FIELD_ID_NAME6:
					switch(PrtItemStr.l_PluNo) {
						case MISC_PLU_WEIGHT :
						case MISC_PLU_PCS    :
						case MISC_PLU_COUNT  :
						case 0:
							continue;
						default :
							plu_get_field(PrtItemStr.plu_addr-1, PTYPE_PLU_NAME6, (INT8U *)&string_buf, (INT16S *)&v16_2,sizeof(string_buf));
							string_buf[v16_2] = 0;
							break;
					}
					break;
				case FIELD_ID_NAME7:
					switch(PrtItemStr.l_PluNo) {
						case MISC_PLU_WEIGHT :
						case MISC_PLU_PCS    :
						case MISC_PLU_COUNT  :
						case 0:
							continue;
						default :
							plu_get_field(PrtItemStr.plu_addr-1, PTYPE_PLU_NAME7, (INT8U *)&string_buf, (INT16S *)&v16_2,sizeof(string_buf));
							string_buf[v16_2] = 0;
							break;
					}
					break;
				case FIELD_ID_NAME8:
					switch(PrtItemStr.l_PluNo) {
						case MISC_PLU_WEIGHT :
						case MISC_PLU_PCS    :
						case MISC_PLU_COUNT  :
						case 0:
							continue;
						default :
#ifdef USE_CHN_EMART_TRACE
							chn_emart_trace_read(PrtItemStr.plu_addr-1, string_buf, CHNEmart_TraceSelectionOrder);
							v16_2 = CHN_EMART_TRACE_CODE_SIZE;							
							string_buf[v16_2] = 0;
#else
							plu_get_field(PrtItemStr.plu_addr-1, 96, (INT8U *)&string_buf, (INT16S *)&v16_2,sizeof(string_buf));
							string_buf[v16_2] = 0;
#endif
							break;
					}
					break;
#endif
#endif
#ifndef USE_ARABIC_NUMBER
				case FIELD_ID_NET_WEIGHT:	// by-count의 무게 표시용 or by-weight의 Net weight표시용
					if (PrtItemStr.l_PluType == PLU_BY_COUNT || PrtItemStr.l_PluType == PLU_BY_PCS) 
					{
						if (GlbOper.byCountApplyWeight) 
						{
							if (PrtItemStr.l_Price.NetWeight<0) PrtItemStr.l_Price.NetWeight = 0;
							v32_1 = PrtItemStr.l_Price.NetWeight;
						} 
						else continue;
					} 
					else if (PrtItemStr.l_PluType == PLU_BY_WEIGHT) 
					{
						if (Startup.country == COUNTRY_US && !PrtItemStr.fixed_price) continue;
#ifdef USE_PERCENT_WEIGHT
						precentweight_mode = get_global_bparam(GLOBAL_SALE_SETUP_FLAG19);
						precentweight_mode = precentweight_mode&0x01;
						if(precentweight_mode)
						{
							v32_1 = PrtItemStr.l_Price.GrossWeight - PrtItemStr.l_TareWeight -PrtItemStr.l_Price.PercentTareWeight;
						}
						else
						{
							v32_1 = PrtItemStr.l_Price.NetWeight;
						}
#else
						v32_1 =  PrtItemStr.l_Price.NetWeight;
#endif
					} 
					else continue;
					if (!v32_1) continue;
					v8_1=get_decimal_pt(4, PrtItemStr.plu_addr);
					v8_2=dec_ch;
#ifdef USE_USA_FIXEDWEIGHT_UNIT_OZ
					if (PrtWeightUnit == WEIGHT_UNIT_LB)
					{
						if (PrtItemStr.l_PluType != PLU_BY_WEIGHT || PrtItemStr.fixed_price){
							if(!ADM_GetData.Zero) //Actual Weight Not Zero
							{
								v32_1 = LbtoOz(PrtItemStr.l_Price.NetWeight, rate, v8_1);
							}
							PrtGenWeightStr(string_buf1, v32_1, 8, w_sign1, w_sign2, v8_1, v8_2);	// " (6 lb 4 oz) 2835g"
							PrtGenWeightStr(string_buf, v32_1, 9, w_sign1, w_sign2, v8_1, v8_2);	// "100 oz"
							strcat(string_buf, string_buf1);	 				// "100 oz( 6 lb 4 oz),2845g"
						}
					} 
					else PrtGenWeightStr(string_buf, v32_1, 4, w_sign1, w_sign2, v8_1, v8_2); 	// "100 g"
#else
					PrtGenWeightStr(string_buf, v32_1, wt_prt_type, w_sign1, w_sign2, v8_1, v8_2);
#endif					
					break;
#endif //USE_ARABIC_NUMBER
#ifndef USE_ARABIC_NUMBER				
				case FIELD_ID_2ND_NET_WEIGHT :		//net weight + %tare weight
					if(PrtItemStr.l_PluType != PLU_BY_WEIGHT) continue;
#ifndef USE_PERCENT_WEIGHT
					v32_1 = PrtItemStr.l_Price.PercentTareWeight + PrtItemStr.l_Price.NetWeight;
#else
					precentweight_mode = get_global_bparam(GLOBAL_SALE_SETUP_FLAG19);
					precentweight_mode = precentweight_mode&0x01;
					if(precentweight_mode)
					{
						v32_1 = PrtItemStr.l_Price.PercentTareWeight;
					}
					else
					{
						v32_1 = PrtItemStr.l_Price.NetWeight;
					}
#endif
					if(!v32_1) continue;
					v8_1=get_decimal_pt(4, PrtItemStr.plu_addr);
					v8_2=dec_ch;
					PrtGenWeightStr(string_buf, v32_1, wt_prt_type, w_sign1, w_sign2, v8_1, v8_2);
					break;
#endif //USE_ARABIC_NUMBER					
				case FIELD_ID_GROSS_WEIGHT :	 	// Percent Tare의 Gross Weight 표시용
#ifdef USE_ARABIC_NUMBER
				case FIELD_ID_2ND_NET_WEIGHT :
#endif //USE_ARABIC_NUMBER				
					if(PrtItemStr.l_PluType != PLU_BY_WEIGHT) continue;
					v32_1 = PrtItemStr.l_Price.GrossWeight;
					if(!v32_1) continue;
					v8_1=get_decimal_pt(4, PrtItemStr.plu_addr);
					v8_2=dec_ch;
					PrtGenWeightStr(string_buf, v32_1, wt_prt_type, w_sign1, w_sign2, v8_1, v8_2);	// 20060315
					break;
#ifdef USE_TRACE_STANDALONE  
			#ifdef USE_KOR_TRACE_999
				case FIELD_ID_TRACEABILITY_CODE:	// 묶음번호 도축장(농협)
					switch(PrtItemStr.l_PluNo) {
						case MISC_PLU_WEIGHT :
						case MISC_PLU_PCS    :
						case MISC_PLU_COUNT  :
						case 0:
							continue;
						default :
							if (!CurTraceStatus.indivStr.lotFlag) continue;
							if (CurTraceStatus.slPlace)
							{
								addr =  DFLASH_BASE + FLASH_TRACE_SLAUGHTERHOUSE_AREA;
								addr =  addr + sizeof(TRACE_SLAUGHTHOUSE_STRUCT) * (INT32U)(CurTraceStatus.slPlace-1);	// code-1 (?)
								v16_2 = Flash_wread(addr);
								if(CurTraceStatus.slPlace != v16_2) continue;
								addr += 2;
								tempSize = sizeof(TRACE_SLAUGHTHOUSE_STRUCT)-2;
								Flash_sread(addr, (HUGEDATA INT8U *)string_buf, tempSize);

							} 
							else 
							{
#ifndef USE_NHMART_SAFE_MEAT
								strcpy(string_buf, "도축장:");
								strcat(string_buf, (char *)CurTraceStatus.indivStr.slaughtHouse);
								tempSize = 7+101;	// 묶음 도축장
#else
								strcpy(string_buf, (char *)CurTraceStatus.indivStr.slaughtHouse);
								tempSize = 101;	// 묶음 도축장
#endif
							}
							string_buf[tempSize] = 0;
							break;
					}
					break;
				case FIELD_ID_BORN_COUNTRY:		// 묶음번호 부위(농협)
					switch(PrtItemStr.l_PluNo) {
						case MISC_PLU_WEIGHT :
						case MISC_PLU_PCS    :
						case MISC_PLU_COUNT  :
						case 0:
							continue;
						default :
							if (!CurTraceStatus.indivStr.lotFlag) continue;
							strcpy(string_buf, "묶음부위:");
							strcat(string_buf, (char *)CurTraceStatus.indivStr.farm);
							tempSize = sizeof(CurTraceStatus.indivStr.farm);
							string_buf[tempSize] = 0;
							break;
					}
					break;
			#endif // USE_KOR_TRACE_999
#ifndef USE_KOR_TRACE
				case FIELD_ID_INDIV_MSG_1:	// 개체번호
					switch(PrtItemStr.l_PluNo) {
						case MISC_PLU_WEIGHT :
						case MISC_PLU_PCS    :
						case MISC_PLU_COUNT  :
						case 0:
							continue;
						default :
  #ifdef USE_TOPMART_DEFAULT
							if(!strncmp((char *)CurTraceStatus.indivStr.individualNO, "777777777777",12))
								continue;
  #endif
		      				if (FlagIndividualMultiStart || FlagIndividualMultiRemain)//복수개체 인쇄
							{
								PrintMultiName_GSMart(FIELD_ID_INDIV_MSG_1, 0);
								FlagIndividualMultiStart = OFF;
								FlagIndividualMultiRemain = ON;
								continue;
							}
							else
							{
								if(changeIndivString)
								{
									if((CurTraceStatus.indivStr.individualNO[0] == '0') || (CurTraceStatus.indivStr.individualNO[0] == '1') || 
									  (CurTraceStatus.indivStr.individualNO[0] == '8') || (CurTraceStatus.indivStr.individualNO[0] == '9')) //소, 돼지 ,수입육 //소, 돼지 ,수입육 
									{
										//개체식별번호
										sprintf(string_buf, "%cX1%s %cX2%s", FONT_CON_CH, "이력번호 :", FONT_CON_CH,(char*)CurTraceStatus.indivStr.individualNO);
									}
#ifdef USE_EMART_PIG_TRACE
									else if(CurTraceStatus.indivStr.individualNO[0] == 'A')
									{
										//수입개체이력라벨 32자리 
										sprintf(string_buf, "%cX1%s %cX2%s", FONT_CON_CH, "이력번호 :", FONT_CON_CH, ImportLotNumber);
									}
#endif
									else if(CurTraceStatus.indivStr.individualNO[0] == 0)
									{
										//공란이면 인쇄안함.
										continue;	//다음 field 로 넘어감. 
									}
									else
									{
										//묶음번호로 인쇄
										sprintf(string_buf, "%cX1%s %cX2%s", FONT_CON_CH, "묶음번호 :", FONT_CON_CH, (char*)CurTraceStatus.indivStr.individualNO);
									}
									tempSize = strlen(string_buf);
								}
								else
								{
#ifdef USE_EMART_PIG_TRACE
									if(ImportLotNumberflag == 1) 
									{	
										strcpy(string_buf, ImportLotNumber);
										tempSize = 24;
									}
									else
									{
										strcpy(string_buf, (char *)CurTraceStatus.indivStr.individualNO);
										tempSize = sizeof(CurTraceStatus.indivStr.individualNO);
									}
#else
  #ifdef USE_LOTTEMART_MEATSCANNER
									if(CurTraceStatus.meatscannerflag)
									{
										if(CurTraceStatus.indivno)
										{
											memset((INT8U*)string_buf,0,sizeof(string_buf));
											strcpy(string_buf, (char *)CurTraceStatus.indivno);
											tempSize = strlen((char *)CurTraceStatus.indivno);
										}
									}
									else
									{
  #endif
									strcpy(string_buf, (char *)CurTraceStatus.indivStr.individualNO);
									tempSize = sizeof(CurTraceStatus.indivStr.individualNO);

  #ifdef USE_LOTTEMART_MEATSCANNER
									}
  #endif
#endif // USE_EMART_PIG_TRACE

								}
							}
							string_buf[tempSize] = 0;
							break;
					}
					break; 
				case FIELD_ID_INDIV_MSG_2:	// 등급
					switch(PrtItemStr.l_PluNo) {
						case MISC_PLU_WEIGHT :
						case MISC_PLU_PCS    :
						case MISC_PLU_COUNT  :
						case 0:
							continue;
						default :	
#ifdef USE_TOPMART_STRUCTURE
							if(!strncmp((char *)CurTraceStatus.indivStr.individualNO,"777777777777", 12)) continue;	
#endif
							strcpy(string_buf, (char *)CurTraceStatus.curGradeText );								
							tempSize = sizeof(CurTraceStatus.curGradeText);
							string_buf[tempSize] = 0;
							break;
					}
					break;
				case FIELD_ID_INDIV_MSG_3:	// 품종
					switch(PrtItemStr.l_PluNo) {
						case MISC_PLU_WEIGHT :
						case MISC_PLU_PCS    :
						case MISC_PLU_COUNT  :
						case 0:
							continue;
						default :
#ifdef USE_TOPMART_STRUCTURE
							if(!strncmp((char *)CurTraceStatus.indivStr.individualNO,"777777777777", 12)) continue;							
							trace_lookup_breed_from_table(CurTraceStatus.indivStr.meatKind, string_buf, sizeof(string_buf));
#else
							strcpy(string_buf, (char *)CurTraceStatus.indivStr.breed);
							tempSize = sizeof(CurTraceStatus.indivStr.breed);
							string_buf[tempSize] = 0;
#endif
							break;
					}
					break;
				case FIELD_ID_INDIV_MSG_4:	// 사육지
					switch(PrtItemStr.l_PluNo) {
						case MISC_PLU_WEIGHT :
						case MISC_PLU_PCS    :
						case MISC_PLU_COUNT  :
						case 0:
							continue;
						default :
						#ifdef USE_KOR_TRACE_999
							if (CurTraceStatus.indivStr.lotFlag) continue;
						#endif                        
							strcpy(string_buf, (char *)CurTraceStatus.indivStr.farm);
							tempSize = sizeof(CurTraceStatus.indivStr.farm);
							string_buf[tempSize] = 0;
							break;
					}
					break;
				case FIELD_ID_INDIV_MSG_5:    // 도축일짜
					switch(PrtItemStr.l_PluNo) {
						case MISC_PLU_WEIGHT :
						case MISC_PLU_PCS    :
						case MISC_PLU_COUNT  :
						case 0:
							continue;
						default :
						#ifdef USE_KOR_TRACE_999
							if (CurTraceStatus.indivStr.lotFlag) continue;
						#endif   
#ifdef USE_TOPMART_STRUCTURE
							if(!strncmp((char *)CurTraceStatus.indivStr.individualNO,"777777777777", 12)) continue;	
#endif                        
							if(CurTraceStatus.indivStr.slaughtDate[0] && CurTraceStatus.indivStr.slaughtDate[1] && CurTraceStatus.indivStr.slaughtDate[2])
							{
								sprintf(string_buf, "%02d.%02d.%02d", CurTraceStatus.indivStr.slaughtDate[0],CurTraceStatus.indivStr.slaughtDate[1],CurTraceStatus.indivStr.slaughtDate[2]);
								string_buf[8] = 0;
							}
							else 
							{	
								string_buf[0]=0;
							}
							break;
					}
					break;
#endif // USE_KOR_TRACE
				case FIELD_ID_INDIV_NO:
					switch(PrtItemStr.l_PluNo) 
					{
						case MISC_PLU_WEIGHT :
						case MISC_PLU_PCS	 :
						case MISC_PLU_COUNT  :
						case 0:
							continue;
						default :
							sprintf(string_buf, "%d", CurTraceStatus.indivStr.index );
							break;
					}
					break;
				case FIELD_ID_MEAT_USE:	// 용도
					switch(PrtItemStr.l_PluNo) {
						case MISC_PLU_WEIGHT :
						case MISC_PLU_PCS    :
						case MISC_PLU_COUNT  :
						case 0:
							continue;
						default :
#ifdef USE_LOTTEMART_MEATSCANNER
							if(CurTraceStatus.meatscannerflag)
							{

								if(CurTraceStatus.meatUseNo)
								{
									addr =	DFLASH_BASE + FLASH_TRACE_SPECIALUSE_AREA;
									addr =	addr + sizeof(TRACE_SPECIALUSE_STRUCT) * (INT32U)(CurTraceStatus.meatUseNo-1);	// code-1 (?)
									v16_2 = Flash_wread(addr);
									if(CurTraceStatus.meatUseNo != v16_2) continue;
									addr += 2;
									Flash_sread(addr, ( INT8U *)string_buf, sizeof(TRACE_SPECIALUSE_STRUCT)-2);
								} else {
									continue;
								}
							}
							else
							{
#endif

							if(CurTraceStatus.meatUse)
							{
								addr =  DFLASH_BASE + FLASH_TRACE_SPECIALUSE_AREA;
								addr =  addr + sizeof(TRACE_SPECIALUSE_STRUCT) * (INT32U)(CurTraceStatus.meatUse-1);	// code-1 (?)
								v16_2 = Flash_wread(addr);
								if(CurTraceStatus.meatUse != v16_2) continue;
								addr += 2;
								Flash_sread(addr, (HUGEDATA INT8U *)string_buf, sizeof(TRACE_SPECIALUSE_STRUCT)-2);
							} else {
								continue;
							}
#ifdef USE_LOTTEMART_MEATSCANNER
							}
#endif
							break;
					}
					break;
#ifdef USE_TRACE_1000QTY
				case FIELD_ID_PIG_FARM_CODE: //농장 번호 
					switch(PrtItemStr.l_PluNo) {
						case MISC_PLU_WEIGHT :
						case MISC_PLU_PCS	 :
						case MISC_PLU_COUNT  :
						case 0:
							continue;
						default :
							strcpy(string_buf, (char *)CurTraceStatus.indivStr.nongjangNo);
							tempSize = sizeof(CurTraceStatus.indivStr.nongjangNo);
							string_buf[tempSize] = 0;
							break;
					}
					break;
#endif
				case FIELD_ID_SLAUGHTER_HOUSE:	// 도축장
					switch(PrtItemStr.l_PluNo) {
						case MISC_PLU_WEIGHT :
						case MISC_PLU_PCS    :
						case MISC_PLU_COUNT  :
						case 0:
							continue;
						default :
						#ifdef USE_KOR_TRACE_999
							if (CurTraceStatus.indivStr.lotFlag) continue;
						#endif
		      					if (FlagIndividualMultiStart || FlagIndividualMultiRemain)//복수개체 인쇄
							{
								PrintMultiName_GSMart(FIELD_ID_SLAUGHTER_HOUSE, 1);
								FlagIndividualMultiStart = OFF;
								FlagIndividualMultiRemain = ON;
								continue;
							}
							else
							{

#ifdef USE_LOTTEMART_MEATSCANNER
								if(CurTraceStatus.meatscannerflag)
								{
									if(CurTraceStatus.slPlace1)
									{
										addr =	DFLASH_BASE + FLASH_TRACE_SLAUGHTERHOUSE_AREA;
										addr =	addr + TRACE_SLAUGHTERHOUSE_KR_STRUCT_SIZE * (INT32U)(CurTraceStatus.slPlace1-1);	// code-1 (?)
										v16_2 = Flash_wread(addr);

										if(CurTraceStatus.slPlace1 != v16_2) continue;

										addr += 2;
										tempSize = TRACE_SLAUGHTHOUSE_NAME_SIZE;
										v8_1 = strlen(string_buf);
										if(v8_1)
										{
											string_buf[v8_1] = ',';
											v8_1++;
										}
										Flash_sread(addr, ( INT8U *)&string_buf[v8_1], tempSize);
									}

									if(CurTraceStatus.slPlace2)
									{
										addr =	DFLASH_BASE + FLASH_TRACE_SLAUGHTERHOUSE_AREA;
										addr =	addr + TRACE_SLAUGHTERHOUSE_KR_STRUCT_SIZE * (INT32U)(CurTraceStatus.slPlace2-1);	// code-1 (?)
										v16_2 = Flash_wread(addr);

										if(CurTraceStatus.slPlace2 != v16_2) continue;

										addr += 2;
										tempSize = TRACE_SLAUGHTHOUSE_NAME_SIZE;
										v8_1 = strlen(string_buf);
										if(v8_1)
										{
											string_buf[v8_1] = ',';
											v8_1++;
										}
										Flash_sread(addr, ( INT8U *)&string_buf[v8_1], tempSize);
									}
									if(CurTraceStatus.slPlace3)
									{
										addr =	DFLASH_BASE + FLASH_TRACE_SLAUGHTERHOUSE_AREA;
										addr =	addr + TRACE_SLAUGHTERHOUSE_KR_STRUCT_SIZE * (INT32U)(CurTraceStatus.slPlace3-1);	// code-1 (?)
										v16_2 = Flash_wread(addr);

										if(CurTraceStatus.slPlace3 != v16_2) continue;

										addr += 2;
										tempSize = TRACE_SLAUGHTHOUSE_NAME_SIZE;
										v8_1 = strlen(string_buf);
										if(v8_1)
										{
											string_buf[v8_1] = ',';
											v8_1++;
										}
										Flash_sread(addr, ( INT8U *)&string_buf[v8_1], tempSize);
									}
								}
								else
								{
#endif
								if(CurTraceStatus.slPlace)
								{
									addr =  DFLASH_BASE + FLASH_TRACE_SLAUGHTERHOUSE_AREA;
									addr =  addr + TRACE_SLAUGHTERHOUSE_KR_STRUCT_SIZE * (INT32U)(CurTraceStatus.slPlace-1);	// code-1 (?)
									v16_2 = Flash_wread(addr);
								#ifdef USE_KOR_SLAUGHT_CODE_4DIGIT
									if(trace_slaughthouse_index2no(CurTraceStatus.slPlace) != v16_2) continue;
								#else
									if(CurTraceStatus.slPlace != v16_2) continue;
								#endif
									addr += 2;
								    tempSize = TRACE_SLAUGHTHOUSE_NAME_SIZE;
									v8_1 = 0;
								#ifdef USE_KOR_SLAUGHT_CODE_4DIGIT
									sprintf(string_buf, "도축장명:");
									v8_1 = strlen(string_buf);
								#endif
									Flash_sread(addr, ( INT8U *)&string_buf[v8_1], tempSize);
								}
								else
								{
#ifdef USE_TOPMART_STRUCTURE
									if(!strncmp((char *)CurTraceStatus.indivStr.individualNO,"777777777777", 12)) continue;	
									trace_lookup_slaughter_from_table(CurTraceStatus.indivStr.slaughtNo, string_buf, sizeof(string_buf));
									tempSize = sizeof(CurTraceStatus.indivStr.slaughtHouse);
#else
									strcpy(string_buf, (char *)CurTraceStatus.indivStr.slaughtHouse);
									tempSize = sizeof(CurTraceStatus.indivStr.slaughtHouse);
								#ifdef USE_KOR_TRACE_999
									if (CurTraceStatus.indivStr.lotFlag) tempSize = 101;	// 묶음 도축장
								#endif                                    
#endif
								}
							}
#ifdef USE_LOTTEMART_BARCODE_FUNCTION
#ifdef USE_LOTTEMART_MEATSCANNER
							}
  #endif

							tempSize = strlen(string_buf);
							if (tempSize > 127) tempSize = 127;
#endif
							string_buf[tempSize] = 0;
							break;
					}
					break;
				//Added by JJANG 20090423
				case FIELD_ID_INDIN_NO_AND_S_HOUSE:	// 개체번호 + 도축장
					switch(PrtItemStr.l_PluNo) {
						case MISC_PLU_WEIGHT :
						case MISC_PLU_PCS    :
						case MISC_PLU_COUNT  :
						case 0:
							continue;
						default :
		      					if (FlagIndividualMultiStart || FlagIndividualMultiRemain)//복수개체 인쇄
							{
								PrintMultiName_GSMart(FIELD_ID_INDIN_NO_AND_S_HOUSE, 2);
								FlagIndividualMultiStart = OFF;
								FlagIndividualMultiRemain = ON;
								continue;
							}
							else
							{
								v16_2 = strlen((char*)CurTraceStatus.indivStr.individualNO);
								if (v16_2)	//개체번호 유
								{
									if(CurTraceStatus.slPlace)
									{
										strcpy(string_buf, (char *)CurTraceStatus.indivStr.individualNO);
										addr =  DFLASH_BASE + FLASH_TRACE_SLAUGHTERHOUSE_AREA;
										addr =  addr + TRACE_SLAUGHTERHOUSE_KR_STRUCT_SIZE * (INT32U)(CurTraceStatus.slPlace-1);	// code-1 (?)
										v16_2 = Flash_wread(addr);
									#ifdef USE_KOR_SLAUGHT_CODE_4DIGIT
										if(trace_slaughthouse_index2no(CurTraceStatus.slPlace) != v16_2) continue;
									#else
										if(CurTraceStatus.slPlace != v16_2) continue;
									#endif
										addr += 2;
										tempSize = TRACE_SLAUGHTHOUSE_NAME_SIZE;
										v16_2 = strlen((char*)CurTraceStatus.indivStr.individualNO);
										string_buf[v16_2] = ' ';
										Flash_sread(addr, (HUGEDATA INT8U *)string_buf+v16_2+1, tempSize);
										tempSize += sizeof(CurTraceStatus.indivStr.individualNO);
										tempSize++;

									}
									else //도축장 없을경우에는 개체번호만을 찍는다.
									{
					      					v16_2 = strlen((char*)CurTraceStatus.indivStr.individualNO);
										strcpy(string_buf, (char *)CurTraceStatus.indivStr.individualNO);
										tempSize = sizeof(CurTraceStatus.indivStr.individualNO);
									}
								}
								else	//개체번호 무
								{
									if(CurTraceStatus.slPlace)
									{
										addr =  DFLASH_BASE + FLASH_TRACE_SLAUGHTERHOUSE_AREA;
										addr =  addr + TRACE_SLAUGHTERHOUSE_KR_STRUCT_SIZE * (INT32U)(CurTraceStatus.slPlace-1);	// code-1 (?)
										v16_2 = Flash_wread(addr);
									#ifdef USE_KOR_SLAUGHT_CODE_4DIGIT
										if(trace_slaughthouse_index2no(CurTraceStatus.slPlace) != v16_2) continue;
									#else
										if(CurTraceStatus.slPlace != v16_2) continue;
									#endif
										addr += 2;
										tempSize = TRACE_SLAUGHTHOUSE_NAME_SIZE;
										Flash_sread(addr, (HUGEDATA INT8U *)string_buf, tempSize);
									} 
									else
									{
										strcpy(string_buf, (char *)CurTraceStatus.indivStr.slaughtHouse);
										tempSize = sizeof(CurTraceStatus.indivStr.slaughtHouse);
									#ifdef USE_KOR_TRACE_999
										if (CurTraceStatus.indivStr.lotFlag) tempSize = 101;	// 묶음 도축장
									#endif                                        
									}
								}
							}
							string_buf[tempSize] = 0;
							break;
					}
					break;
#ifdef USE_TRACE_MEATPART
				case FIELD_ID_MEAT_PART:	// 부위
					switch(PrtItemStr.l_PluNo) {
						case MISC_PLU_WEIGHT :
						case MISC_PLU_PCS    :
						case MISC_PLU_COUNT  :
						case 0:
							continue;
						default :
							if(CurTraceStatus.meatPart)
							{
								addr =  DFLASH_BASE + FLASH_TRACE_MEATPART_AREA;
								addr =  addr + sizeof(TRACE_SPECIALUSE_STRUCT) * (INT32U)(CurTraceStatus.meatPart-1);	// code-1 (?)
								v16_2 = Flash_wread(addr);
								if(CurTraceStatus.meatPart != v16_2) continue;
								addr += 2;
								Flash_sread(addr, (HUGEDATA INT8U *)string_buf, sizeof(TRACE_MEATPART_STRUCT)-2);
							} else {
								continue;
							}
							break;
					}
					break;
#endif                    
#ifdef USE_TRACE_MEATGRADE
				case FIELD_ID_ALL_MEAT_GRADE:	// 전체등급
					switch(PrtItemStr.l_PluNo) {
						case MISC_PLU_WEIGHT :
						case MISC_PLU_PCS    :
						case MISC_PLU_COUNT  :
						case 0:
							continue;
						default :
  #ifdef USE_TOPMART_STRUCTURE
							if(!strncmp((char *)CurTraceStatus.indivStr.individualNO,"777777777777", 12)) continue; 
  #endif
  #ifdef USE_LOTTEMART_MEATSCANNER
						if (CurTraceStatus.meatscannerflag && !CurTraceStatus.no14indivflag)
						{
							if (CurTraceStatus.meatgradeNo)
							{
								if (TraceMeatGradeListNum[CurTraceStatus.meatgradeNo - 1] == 0) continue;	// "없음" 또는 순서가 정의되지 않은 등급
								if (CurTraceStatus.meatgradeNo <= 10)
								{
									v8_1 = 0;	// Beef
								}
								else
								{
									v8_1 = 1;	// Pork
								}
    #ifdef USE_MARBLING_GRADE_PRINT
								if(v8_1) //돼지 등급
								{
									sprintf(string_buf, (char*)&TraceMeatGradeListText[v8_1][0]);
								}
      #ifdef USE_EXPAND_MARBLING_LIST
								else //소 등급
								{
									len = 0;
									trace_meatgrade_getGradeText(CurTraceStatus.meatgradeNo, CurTraceStatus.curGradeText);
									if((CurTraceStatus.meatgradeNo >= 7) && (CurTraceStatus.meatgradeNo <= 9)) // 등급번호 8,9,10 -> 1++(7), 1++(8),1++(9) 중 번호에 맞는 한개만 인쇄.
									{
										sprintf(string_buf, "등급(마블링): ");
										len = strlen(string_buf);
										sprintf(string_buf+len, "%s  ",CurTraceStatus.curGradeText);
										len = strlen(string_buf);
									}
									else if (CurTraceStatus.meatgradeNo == 1)
									{
										sprintf(string_buf, "등급(마블링): ");
										len = strlen(string_buf);
										sprintf(string_buf+len, "1++( )   "); //등급번호 1,2,3,4,5 -> 1++( ) 가 1+ 앞에 인쇄
										len = strlen(string_buf);
									}
									else
									{
										sprintf(string_buf, "등급(마블링): ");
										len = strlen(string_buf);
										sprintf(string_buf+len, "1++( )   "); //등급번호 1,2,3,4,5 -> 1++( ) 가 1+ 앞에 인쇄
										len = strlen(string_buf);

									}
									sprintf(string_buf+len, (char*)&TraceMeatGradeListText[v8_1][39]); //31이 1+스트링 시작 위치
								}
      #else
								else //소 등급
								{
									len = 0;
									trace_meatgrade_getGradeText(CurTraceStatus.meatgradeNo, CurTraceStatus.curGradeText);
									if((CurTraceStatus.meatgradeNo >= 7) && (CurTraceStatus.meatgradeNo <= 9)) // 등급번호 8,9,10 -> 1++(7), 1++(8),1++(9) 중 번호에 맞는 한개만 인쇄.
									{
										sprintf(string_buf, "등급(마블링):");
										len = strlen(string_buf);
										sprintf(string_buf+len, "%s,",CurTraceStatus.curGradeText);
										len = strlen(string_buf);
									}
									else if (CurTraceStatus.meatgradeNo == 1)
									{
										sprintf(string_buf, "등급:");
										len = strlen(string_buf);
										sprintf(string_buf+len, "%s,",CurTraceStatus.curGradeText);
										len = strlen(string_buf);
									}
									else
									{
										sprintf(string_buf, "등급(마블링):");
										len = strlen(string_buf);
										sprintf(string_buf+len, "1++( ),"); //등급번호 1,2,3,4,5 -> 1++( ) 가 1+ 앞에 인쇄
										len = strlen(string_buf);
									}
									sprintf(string_buf+len, (char*)&TraceMeatGradeListText[v8_1][31]); //31이 1+스트링 시작 위치
								}
      #endif
    #else
									sprintf(string_buf, (char*)&TraceMeatGradeListText[v8_1][0]);
    #endif
								}
								else
								{
									continue;
								}
								break;
							}
							else
							{
  #endif //#ifdef USE_LOTTEMART_MEATSCANNER

							if (CurTraceStatus.gradeNo)
							{
								if (TraceMeatGradeListNum[CurTraceStatus.gradeNo - 1] == 0) continue;	// "없음" 또는 순서가 정의되지 않은 등급
								if (CurTraceStatus.gradeNo <= 10)
								{
									v8_1 = 0;	// Beef
								}
								else
								{
									v8_1 = 1;	// Pork
								}
  #ifdef USE_EMART_MEATGRADE_PRINT  //근내지방도 등급 리스트 인쇄 기능
								if(v8_1) //돼지 등급
								{
									sprintf(string_buf, (char*)&TraceMeatGradeListText[v8_1][0]);
								}
								else //소 등급 
								{
									len = 0;
									trace_meatgrade_getGradeText(CurTraceStatus.gradeNo, CurTraceStatus.curGradeText);
									if((CurTraceStatus.gradeNo >= 8) && (CurTraceStatus.gradeNo <= 10)) // 등급번호 8,9,10 -> 1++(7), 1++(8),1++(9) 중 번호에 맞는 한개만 인쇄.
									{
										sprintf(string_buf, "등급(마블링):");
										len = strlen(string_buf);
										sprintf(string_buf+len, "%s,",CurTraceStatus.curGradeText);
										len = strlen(string_buf);
									}
									else if (CurTraceStatus.gradeNo == 7)
									{
										sprintf(string_buf, "등급:");
										len = strlen(string_buf);
										sprintf(string_buf+len, "%s,",CurTraceStatus.curGradeText);
										len = strlen(string_buf);									
									}
									else
									{
										sprintf(string_buf, "등급(마블링):");
										len = strlen(string_buf);
										sprintf(string_buf+len, "1++( ),"); //등급번호 1,2,3,4,5 -> 1++( ) 가 1+ 앞에 인쇄
										len = strlen(string_buf);								
									}
									sprintf(string_buf+len, (char*)&TraceMeatGradeListText[v8_1][31]); //31이 1+스트링 시작 위치
								}
    #elif defined(USE_MARBLING_GRADE_PRINT)
								if(v8_1) //돼지 등급
								{
									sprintf(string_buf, (char*)&TraceMeatGradeListText[v8_1][0]);
								}
      #ifdef USE_EXPAND_MARBLING_LIST
								else //소 등급
								{
									len = 0;
									trace_meatgrade_getGradeText(CurTraceStatus.gradeNo, CurTraceStatus.curGradeText);
									if((CurTraceStatus.gradeNo >= 7) && (CurTraceStatus.gradeNo <= 9)) // 등급번호 8,9,10 -> 1++(7), 1++(8),1++(9) 중 번호에 맞는 한개만 인쇄.
									{
										sprintf(string_buf, "등급(마블링): ");
										len = strlen(string_buf);
										sprintf(string_buf+len, "%s  ",CurTraceStatus.curGradeText);
										len = strlen(string_buf);
									}
									else if (CurTraceStatus.gradeNo == 1)
									{
										sprintf(string_buf, "등급(마블링): ");
										len = strlen(string_buf);
										sprintf(string_buf+len, "1++( )   "); //등급번호 1,2,3,4,5 -> 1++( ) 가 1+ 앞에 인쇄
										len = strlen(string_buf);
									}
									else
									{
										sprintf(string_buf, "등급(마블링): ");
										len = strlen(string_buf);
										sprintf(string_buf+len, "1++( )   "); //등급번호 1,2,3,4,5 -> 1++( ) 가 1+ 앞에 인쇄
										len = strlen(string_buf);

									}
									sprintf(string_buf+len, (char*)&TraceMeatGradeListText[v8_1][39]); //31이 1+스트링 시작 위치
								}
      #else
								else //소 등급
								{
									len = 0;
									trace_meatgrade_getGradeText(CurTraceStatus.gradeNo, CurTraceStatus.curGradeText);
									if((CurTraceStatus.gradeNo >= 7) && (CurTraceStatus.gradeNo <= 9)) // 등급번호 8,9,10 -> 1++(7), 1++(8),1++(9) 중 번호에 맞는 한개만 인쇄.
									{
										sprintf(string_buf, "등급(마블링):");
										len = strlen(string_buf);
										sprintf(string_buf+len, "%s,",CurTraceStatus.curGradeText);
										len = strlen(string_buf);
									}
        #ifndef USE_NHMART_DEFAULT
  									else if (CurTraceStatus.gradeNo == 1)
									{
										sprintf(string_buf, "등급:");
										len = strlen(string_buf);
										sprintf(string_buf+len, "%s,",CurTraceStatus.curGradeText);
										len = strlen(string_buf);
									}
        #endif
									else
									{
										sprintf(string_buf, "등급(마블링):");
										len = strlen(string_buf);
										sprintf(string_buf+len, "1++( ),"); //등급번호 1,2,3,4,5 -> 1++( ) 가 1+ 앞에 인쇄
										len = strlen(string_buf);
									}
									sprintf(string_buf+len, (char*)&TraceMeatGradeListText[v8_1][31]); //31이 1+스트링 시작 위치
								}
      #endif
  #else						
								sprintf(string_buf, (char*)&TraceMeatGradeListText[v8_1][0]);
  #endif
							} 
							else 
							{
								continue;
							}
							break;
  #ifdef USE_LOTTEMART_MEATSCANNER
							}
  #endif
					}
					break;
				case FIELD_ID_MEAT_GRADE_CHECK:	// 등급표시
					switch(PrtItemStr.l_PluNo) {
						case MISC_PLU_WEIGHT :
						case MISC_PLU_PCS    :
						case MISC_PLU_COUNT  :
						case 0:
							continue;
						default :
  #ifdef USE_LOTTEMART_MEATSCANNER
							if (CurTraceStatus.meatscannerflag && !CurTraceStatus.no14indivflag)
							{
								if (CurTraceStatus.meatgradeNo)
								{
    #ifdef USE_MARBLING_GRADE_PRINT
									if (TraceMeatGradeListNum[CurTraceStatus.meatgradeNo - 1] == 0) continue;	// "없음" 또는 순서가 정의되지 않은 등급
									v16_1 = (CurTraceStatus.meatgradeNo - 1) / 10;	// 0:Beef, 1:Pork
									v8_1 = 0;

									memset(string_buf,0,128);

									string_buf[v8_1++] = '(';
									if((CurTraceStatus.meatgradeNo >= 7) && (CurTraceStatus.meatgradeNo <= 9))
									{
										v8_3 = 3; //기존 1++자리에 1++(7),1++(8),1++(9)로 변경되어 인쇄되므로 1++ 순서 3으로 세팅
									}
      #ifdef USE_EXPAND_MARBLING_LIST
									else if (CurTraceStatus.meatgradeNo == 1)
									{
										v8_3 = 3; //기존 1++자리에 1++(7),1++(8),1++(9)로 변경되어 인쇄되므로 1++ 순서 3으로 세팅
									}
      #endif
									else
									{
										v8_3 = TraceMeatGradeListNum[CurTraceStatus.meatgradeNo - 1];	// 등급 순서
									}
									if (TraceMeatGradeList_Len[v16_1][v8_3 - 1] == 1)
									{
										string_buf[v8_1++] = '-';
									}
									if (TraceMeatGradeList_Len[v16_1][v8_3 - 1] >= 2)
									{
										string_buf[v8_1++] = '<';
										for (v8_2 = 0; v8_2 < TraceMeatGradeList_Len[v16_1][v8_3 - 1] - 2; v8_2++)
										{
											string_buf[v8_1++] = '=';
										}
										string_buf[v8_1++] = '>';
									}
									string_buf[v8_1++] = ')';

									v16_2 = strlen(TraceMeatGradeListText[v16_1]) - TraceMeatGradeList_Position[v16_1][v8_3 - 1] - TraceMeatGradeList_Len[v16_1][v8_3 - 1] - 2;

      #ifdef USE_EXPAND_MARBLING_LIST
									if((CurTraceStatus.meatgradeNo >= 7) && (CurTraceStatus.meatgradeNo <= 9)) v16_2 = v16_2 - 6;	//등급 7,8,9  인쇄 시 -4는 등급표시 동그라미 표시 시작 위치 조정값. 원래 1++ 인쇄 시작점으로 맞춰져 표시되기에 수정함.
									if(CurTraceStatus.meatgradeNo == 1) v16_2 = v16_2 - 6;
									if(CurTraceStatus.meatgradeNo >= 11) v16_2 = v16_2 - 16;
      #else
									if((CurTraceStatus.meatgradeNo >= 7) && (CurTraceStatus.meatgradeNo <= 9)) v16_2 = v16_2 - 4;	//등급 7,8,9  인쇄 시 -4는 등급표시 동그라미 표시 시작 위치 조정값. 원래 1++ 인쇄 시작점으로 맞춰져 표시되기에 수정함.
									if(CurTraceStatus.meatgradeNo == 1) v16_2 = v16_2 + 11; //등급1++ 인쇄 시 +11는 등급표시 동그라미 표시 시작 위치 조정값. 원래 1++ 인쇄 시작점으로 맞춰져 표시되기에 수정함.
									if(CurTraceStatus.meatgradeNo >= 11) v16_2 = v16_2 - 11;

      #endif
									for (v8_2 = 0; v8_2 < v16_2; v8_2++)
									{
										string_buf[v8_1++] = ' ';
									}
    #else // USE_MARBLING_GRADE_PRINT
									if (TraceMeatGradeListNum[CurTraceStatus.meatgradeNo - 1] == 0) continue;	// "없음" 또는 순서가 정의되지 않은 등급
									v8_3 = TraceMeatGradeMaxQty - TraceMeatGradeListNum[CurTraceStatus.meatgradeNo - 1];
									v8_1 = 0;
									string_buf[v8_1++] = 0xA1;	// 특수기호 원 (0xA1DB)
									string_buf[v8_1++] = 0xDB;
									for (v8_2 = 0; v8_2 < v8_3; v8_2++)
									{
										string_buf[v8_1++] = ' ';
										string_buf[v8_1++] = ' ';
									}
									string_buf[v8_1] = 0;
    #endif //#ifdef USE_MARBLING_GRADE_PRINT
									break;
								}
								else
								{
									continue;
								}
								break;
							}
							else
							{
  #endif //#ifdef USE_LOTTEMART_MEATSCANNER
							if (CurTraceStatus.gradeNo)
							{
  #ifdef USE_TOPMART_STRUCTURE
								if(!strncmp((char *)CurTraceStatus.indivStr.individualNO,"777777777777", 12)) continue; 
  #endif
  #ifdef USE_EMART_MEATGRADE_PRINT	//근내지방도 등급 표시 기능 추가
								if (TraceMeatGradeListNum[CurTraceStatus.gradeNo - 1] == 0) continue;	// "없음" 또는 순서가 정의되지 않은 등급
								v16_1 = (CurTraceStatus.gradeNo - 1) / 10;	// 0:Beef, 1:Pork
								v8_1 = 0;

								memset(string_buf,0,128);
								
								string_buf[v8_1++] = '(';
								if((CurTraceStatus.gradeNo >= 8) && (CurTraceStatus.gradeNo <= 10))
								{
									v8_3 = 3; //기존 1++자리에 1++(7),1++(8),1++(9)로 변경되어 인쇄되므로 1++ 순서 3으로 세팅
								}
								else
								{
							 		v8_3 = TraceMeatGradeListNum[CurTraceStatus.gradeNo - 1];	// 등급 순서
								}
								if (TraceMeatGradeList_Len[v16_1][v8_3 - 1] == 1)
								{
									string_buf[v8_1++] = '-';
								}
								if (TraceMeatGradeList_Len[v16_1][v8_3 - 1] >= 2)
								{
									string_buf[v8_1++] = '<';
									for (v8_2 = 0; v8_2 < TraceMeatGradeList_Len[v16_1][v8_3 - 1] - 2; v8_2++)
									{
										string_buf[v8_1++] = '=';
									}
									string_buf[v8_1++] = '>';
								}
								string_buf[v8_1++] = ')';

								v16_2 = strlen(TraceMeatGradeListText[v16_1]) - TraceMeatGradeList_Position[v16_1][v8_3 - 1] - TraceMeatGradeList_Len[v16_1][v8_3 - 1] - 2;

								if((CurTraceStatus.gradeNo >= 8) && (CurTraceStatus.gradeNo <= 10)) v16_2 = v16_2 - 4;  //등급 8,9,10 인쇄 시 -4는 등급표시 동그라미 표시 시작 위치 조정값. 원래 1++ 인쇄 시작점으로 맞춰져 표시되기에 수정함.
								if(CurTraceStatus.gradeNo == 7) v16_2 = v16_2 + 11;	//등급7 인쇄 시 +11는 등급표시 동그라미 표시 시작 위치 조정값. 원래 1++ 인쇄 시작점으로 맞춰져 표시되기에 수정함.
								if(CurTraceStatus.gradeNo >= 11) v16_2 = v16_2 - 11;

								for (v8_2 = 0; v8_2 < v16_2; v8_2++)
								{
									string_buf[v8_1++] = ' ';
								}
								string_buf[v8_1] = 0;
  #elif defined(USE_MARBLING_GRADE_PRINT)
								if (TraceMeatGradeListNum[CurTraceStatus.gradeNo - 1] == 0) continue;	// "없음" 또는 순서가 정의되지 않은 등급
								v16_1 = (CurTraceStatus.gradeNo - 1) / 10;	// 0:Beef, 1:Pork
								v8_1 = 0;

								memset(string_buf,0,128);

								string_buf[v8_1++] = '(';
								if((CurTraceStatus.gradeNo >= 7) && (CurTraceStatus.gradeNo <= 9))
								{
									v8_3 = 3; //기존 1++자리에 1++(7),1++(8),1++(9)로 변경되어 인쇄되므로 1++ 순서 3으로 세팅
								}
    #if defined(USE_EXPAND_MARBLING_LIST) || defined(USE_NHMART_DEFAULT)
								else if (CurTraceStatus.gradeNo == 1)
								{
									v8_3 = 3; //기존 1++자리에 1++(7),1++(8),1++(9)로 변경되어 인쇄되므로 1++ 순서 3으로 세팅
								}
    #endif
								else
								{
									v8_3 = TraceMeatGradeListNum[CurTraceStatus.gradeNo - 1];	// 등급 순서
								}
								if (TraceMeatGradeList_Len[v16_1][v8_3 - 1] == 1)
								{
									string_buf[v8_1++] = '-';
								}
								if (TraceMeatGradeList_Len[v16_1][v8_3 - 1] >= 2)
								{
									string_buf[v8_1++] = '<';
									for (v8_2 = 0; v8_2 < TraceMeatGradeList_Len[v16_1][v8_3 - 1] - 2; v8_2++)
									{
										string_buf[v8_1++] = '=';
									}
									string_buf[v8_1++] = '>';
								}
								string_buf[v8_1++] = ')';

								v16_2 = strlen(TraceMeatGradeListText[v16_1]) - TraceMeatGradeList_Position[v16_1][v8_3 - 1] - TraceMeatGradeList_Len[v16_1][v8_3 - 1] - 2;

    #ifdef USE_EXPAND_MARBLING_LIST
								if((CurTraceStatus.gradeNo >= 7) && (CurTraceStatus.gradeNo <= 9)) v16_2 = v16_2 - 6;	//등급 7,8,9  인쇄 시 -4는 등급표시 동그라미 표시 시작 위치 조정값. 원래 1++ 인쇄 시작점으로 맞춰져 표시되기에 수정함.
								if(CurTraceStatus.gradeNo == 1) v16_2 = v16_2 - 6;
								if(CurTraceStatus.gradeNo >= 11) v16_2 = v16_2 - 16;
    #else
								if((CurTraceStatus.gradeNo >= 7) && (CurTraceStatus.gradeNo <= 9)) v16_2 = v16_2 - 4;	//등급 7,8,9  인쇄 시 -4는 등급표시 동그라미 표시 시작 위치 조정값. 원래 1++ 인쇄 시작점으로 맞춰져 표시되기에 수정함.
      #ifdef USE_NHMART_DEFAULT
								if(CurTraceStatus.gradeNo == 1) v16_2 = v16_2 - 4;
      #else
								if(CurTraceStatus.gradeNo == 1) v16_2 = v16_2 + 11; //등급1++ 인쇄 시 +11는 등급표시 동그라미 표시 시작 위치 조정값. 원래 1++ 인쇄 시작점으로 맞춰져 표시되기에 수정함.
      #endif
								if(CurTraceStatus.gradeNo >= 11) v16_2 = v16_2 - 11;

    #endif
								for (v8_2 = 0; v8_2 < v16_2; v8_2++)
								{
									string_buf[v8_1++] = ' ';
								}
  #else //USE_EMART_MEATGRADE_PRINT

								if (TraceMeatGradeListNum[CurTraceStatus.gradeNo - 1] == 0) continue;	// "없음" 또는 순서가 정의되지 않은 등급
							 	v8_3 = TraceMeatGradeMaxQty - TraceMeatGradeListNum[CurTraceStatus.gradeNo - 1];
								v8_1 = 0;
								string_buf[v8_1++] = 0xA1;	// 특수기호 원 (0xA1DB)
								string_buf[v8_1++] = 0xDB;
								for (v8_2 = 0; v8_2 < v8_3; v8_2++)
								{
									string_buf[v8_1++] = ' ';
									string_buf[v8_1++] = ' ';
								}
								string_buf[v8_1] = 0;
  #endif //USE_EMART_MEATGRADE_PRINT
							} 
							else 
							{
								continue;
							}
							break;
#ifdef USE_LOTTEMART_MEATSCANNER
						}
#endif

					}
					break;
#endif //#ifdef USE_TRACE_MEATGRADE
#endif	 //-------end of USE_TRACE_STANDALONE 

				case FIELD_ID_COMMODITY_CODE:
					switch(PrtItemStr.l_PluNo) {
						case MISC_PLU_WEIGHT :
						case MISC_PLU_PCS    :
						case MISC_PLU_COUNT  :
						case 0:
							continue;
						default :
							plu_get_field((INT16S)PrtItemStr.plu_addr-1, PTYPE_ITEM_CODE, (INT8U *)&v32_1, (INT16S *)&v16_2,sizeof(v32_1)); 
							break;
					}
					sprintf(string_buf,"%ld",v32_1);
					break;
				case FIELD_ID_SELL_BY_DATE:	// Sell by Date = Current Date(d) + Packed Date(d) + Days(d) + Current Time(h) + Sell by Time(h) + Packed Time(h)
#ifdef USE_PRT_DISC_PERCENT
					if(DiscSale.saleDiscountType!=DISCOUNT_NO) continue;
#endif
					if (PrtItemStr.flagsellbydate == 1) continue;
#ifdef USE_EMART_BACK_LABEL
					if(Operation.BackLabeledFlag)
					{
						PrtItemStr.sellbydate = get_base_bparam(FLASH_BACK_LABEL_VALID_DATE);
					}
#endif
				#ifdef USE_GEORGIA_MART_SELLBYDATE_FUNCTION
					/* Sell_by_Date Value 0일때 Today 출력하고 900 이상일때 No Print 기능 (그루지아 Agrohub 마트 요청) */
					if (PrtItemStr.sellbydate < 900) 
				#else
					#ifdef USE_SELLBYDATETIME_ZERO
					#ifdef USE_DONT_SAVE_VALIDDATE_SALEDATE
					if (PrtItemStr.sellbydate || PrtItemStr.sellbytime || temp_selldate.onoff)		// Sell by Date , Sell by Time, temp_selldate.onoff존재시					
					#else
					if (PrtItemStr.sellbydate || PrtItemStr.sellbytime)		// Sell by Date or Sell by Time존재시					
					#endif
					#else
					if (PrtItemStr.sellbydate || PrtItemStr.sellbytime || PrtItemStr.flagsellbydate == 2)		// Sell by Date or Sell by Time존재시
					#endif
				#endif
					{
					#ifdef USE_DONT_SAVE_VALIDDATE_SALEDATE
					if(temp_selldate.onoff==ON){
						if(temp_selldate.deptno == PrtItemStr.l_DeptNo && temp_selldate.pluno == PrtItemStr.l_PluNo)
						{
							PrtItemStr.sellbydate= temp_selldate.date;
							PrtItemStr.sellbytime= temp_selldate.time;
							if(temp_selldate.date == 0)continue;
						}
					}
					#endif
						// Date 계산
						if(PrtItemStr.sellbydate > 9999) {
							v16_3 = (INT16U)(PrtItemStr.sellbydate / 10000L % 100L);
							v16_4 = (INT16U)(PrtItemStr.sellbydate / 100L % 100L);
							v16_5 = (INT16U)(PrtItemStr.sellbydate %100L);
							v16_1 = 0;
						} else {
							v16_3 = (INT16U)PrtComStr.time.year;
							v16_4 = (INT16U)PrtComStr.time.month;
							v16_5 = (INT16U)PrtComStr.time.date;
							v16_1 = (INT16U)PrtItemStr.sellbydate;
#ifdef USE_GEORGIA_MART_SELLBYDATE_FUNCTION
							// 그루지아 Agrohub 마트 요청을 Input_value 1일때 바로 Tomorrow 출력되도록 함
#else
							if(v16_1 > 0) v16_1--;	// sell by date  0:no print, 1:today, 2:tomorrow...
#endif
							v16_1 = v16_1 + PrtItemStr.packedondate;	// Packed Date + Sell by Date
						}
						// Time 계산
						#ifdef USE_GSMART_PLU
						if (PrtItemStr.flagsellbytime == 3)	//  3:지정
						{
							v8_1 = PrtItemStr.sellbytime / 100;
						}
						else
						{
							if (PrtItemStr.flagpackedontime == 3)	//  packed on time 3:지정
							{
								v8_1 = PrtItemStr.packedontime / 100;
								v8_2 = PrtItemStr.packedontime % 100;
							}
							else
							{
								v8_1 = PrtComStr.time.hour + PrtItemStr.packedontime / 100;		// current time + Packed Time + Sell by Time
								v8_1 += (PrtComStr.time.min + PrtItemStr.packedontime % 100) / 60;
								v8_2 = (PrtComStr.time.min + PrtItemStr.packedontime % 100) % 60;
							}
							v8_1 += PrtItemStr.sellbytime / 100;
							v8_1 += (v8_2 + PrtItemStr.sellbytime % 100) / 60;
						}
						#else
						v8_1 = PrtComStr.time.hour + PrtItemStr.packedontime + PrtItemStr.sellbytime;		// current time + Packed Time + Sell by Time
						#endif
						v8_1 /= 24;
#ifndef USE_LOTTEMART_BARCODE_FUNCTION
						v16_1 += (INT16U)v8_1;	//lottemart는 유효시간 계산이 유효일에 더해지지 않음.
#endif
						t16_3=v16_3; t16_4=v16_4; t16_5=v16_5;
						#ifdef USE_GSMART_PLU
						if (packed_date)
						{
							date_getbeforeday((short*)&v16_3,(short*)&v16_4,(short*)&v16_5,(short)packed_date);
						}
						#endif
						date_getnextday((short*)&v16_3,(short*)&v16_4,(short*)&v16_5,(short)v16_1);
						//-[BM20060309
						v8_1=1;
						if (sellbydate_check && PrtItemStr.flagsellbydate == 0) {
							v8_1=0;
							if ((!((t16_3==v16_3) && (t16_4==v16_4) && (t16_5==v16_5))) || PrtItemStr.sellbydate) v8_1=1;
						}
						//-[END
						if (v8_1) rtc_makedate_str(string_buf, (INT8U)v16_3, (INT8U)v16_4, (INT8U)v16_5);

					} else {
						continue;
					}
					break;
				case FIELD_ID_SELL_BY_TIME:	// Sell by Time = Packed Time + hour
#ifdef USE_PRT_DISC_PERCENT
					if(DiscSale.saleDiscountType!=DISCOUNT_NO) continue;
#endif
					if (PrtItemStr.flagsellbytime == 1) continue;
					#ifdef USE_SELLBYDATETIME_ZERO
					#ifdef USE_DONT_SAVE_VALIDDATE_SALEDATE
					if (PrtItemStr.sellbydate || PrtItemStr.sellbytime || temp_selldate.onoff) 		// Sell by Date , Sell by Time, temp_selldate.date_flag존재시					
					#elif defined (USE_LOTTEMART_BARCODE_FUNCTION)
					// Sell by Date 이 0 이나 1이고 Sell by Time존재시만 프린트 함 또는 유효시간표시가 3인 경우에 인쇄 
					if ((PrtItemStr.sellbydate < 2 && PrtItemStr.sellbytime) || (PrtItemStr.flagsellbytime == 3)) 
					#else
					if (PrtItemStr.sellbydate || PrtItemStr.sellbytime) 		// Sell by Date or Sell by Time존재시					
					#endif
					#else
					if(PrtItemStr.sellbydate || PrtItemStr.sellbytime || PrtItemStr.flagsellbytime == 2 || PrtItemStr.flagsellbytime == 3) 
					#endif
					{
						#ifdef USE_DONT_SAVE_VALIDDATE_SALEDATE
						if(temp_selldate.onoff==ON){
							if(temp_selldate.deptno == PrtItemStr.l_DeptNo && temp_selldate.pluno == PrtItemStr.l_PluNo)
							{
								PrtItemStr.sellbytime= temp_selldate.time;
								if((temp_selldate.time == 0) && (temp_selldate.date == 0) )continue;
							}
						}
						if(!(get_global_bparam(GLOBAL_SALE_SETUP_FLAG14) & 0x02))// Parma : 719: 0: time == 0 이면  time 프린트 하지 않음
						{
							if(PrtItemStr.sellbytime == 0)continue;
						}
						#endif
						if (sellbydate_check && PrtItemStr.flagsellbytime == 0) 
						{
							if (!PrtItemStr.sellbytime) continue;//v8_2=0;
						}
						#ifdef USE_GSMART_PLU
						if (PrtItemStr.flagsellbytime == 3)	//  3:지정
						{
							v8_1 = PrtItemStr.sellbytime / 100;
							v8_2 = PrtItemStr.sellbytime % 100;
							v8_3 = 0;
						}
						else
						{
							if (PrtItemStr.flagpackedontime == 3)	//  packed on time 3:지정
							{
								v8_1 = PrtItemStr.packedontime / 100;
								v8_2 = PrtItemStr.packedontime % 100;
								v8_3 = 0;
							}
							else
							{
								v8_1 = PrtComStr.time.hour + PrtItemStr.packedontime / 100;		// current time + Packed Time + Sell by Time
								v8_1 += (PrtComStr.time.min + PrtItemStr.packedontime % 100) / 60;
								v8_2 = (PrtComStr.time.min + PrtItemStr.packedontime % 100) % 60;
								v8_3 = PrtComStr.time.sec;
							}
							v8_1 += PrtItemStr.sellbytime / 100;
							v8_1 += (v8_2 + PrtItemStr.sellbytime % 100) / 60;
							v8_2 = (v8_2 + PrtItemStr.sellbytime % 100) % 60;
							//v8_3 = v8_3;
						}
						#else
						v8_1 = PrtComStr.time.hour + PrtItemStr.packedontime + PrtItemStr.sellbytime;		// current time + Packed Time + Sell by Time
						v8_2 = PrtComStr.time.min;
						v8_3 = PrtComStr.time.sec;
						#endif
#ifdef USE_LOTTEMART_BARCODE_FUNCTION	//lottemart는 유효시간 계산이 유효일에 더해지지 않음. 하루를 넘어가면 23:59 로 고정.
						if(v8_1 >= 24) continue;
#else
						v8_1 %= 24;		//다음날 시간. 
#endif
						switch (time_print_type) {
							default:
							case 0: rtc_maketime_str(string_buf,(INT8U)v8_1,(INT8U)v8_2,(INT8U)v8_3);
								break;
#ifdef USE_LOTTEMART_BARCODE_FUNCTION
							case 1: sprintf(string_buf,"%c",'A'+ 24);
								break;
							case 2: sprintf(string_buf,"%d",24);
								break;
#else
							case 1: sprintf(string_buf,"%c",'A'+v8_1);
								break;
							case 2: sprintf(string_buf,"%d",v8_1);
								break;
#endif
						}

					} else {
						continue;
					}
					break;
				case FIELD_ID_PACKED_DATE:	// Packed Date = Current Date(d) + Days(d) + Current Time(h) + Packed Time(h)
					// 0:today, 1:tomorrow...
#ifdef USE_PRT_DISC_PERCENT
					if(DiscSale.saleDiscountType!=DISCOUNT_NO) continue;
#endif
					if (PrtItemStr.flagpackedondate == 1) continue;
					#ifdef USE_GSMART_PLU
					if(PrtItemStr.packedondate > 999) continue;
					if (PrtItemStr.flagpackedontime != 3)
					{
						v8_1 = PrtComStr.time.hour + PrtItemStr.packedontime / 100;	// current hour + packed time
						v8_1 += (PrtComStr.time.min + PrtItemStr.packedontime % 100) / 60;
					}
					#else
					if(PrtItemStr.packedondate > 999) continue;
					v8_1 = PrtComStr.time.hour + PrtItemStr.packedontime;	// current hour + packed time
					#endif
					v8_1 /= 24;		// calculate day from hour( 0~23 = 0, 24~47 = 1 ...)
					v16_1 = (INT16U)v8_1 + PrtItemStr.packedondate;
					v16_3 = (INT16U)PrtComStr.time.year;
					v16_4 = (INT16U)PrtComStr.time.month;
					v16_5 = (INT16U)PrtComStr.time.date;

					#ifdef USE_GSMART_PLU
					if (packed_date)
					{
						date_getbeforeday((short*)&v16_3,(short*)&v16_4,(short*)&v16_5,(short)packed_date);
					}
					#endif

					date_getnextday((short*)&v16_3,(short*)&v16_4,(short*)&v16_5,(short)v16_1);
					v8_1 = date_getweek(v16_3, v16_4, v16_5);
					if(PrtItemStr.flagpackedondate == 3)
					{
						prepack_type = 1;
					}
					switch (prepack_type) {
					    default:
					    case 0:rtc_makedate_str(string_buf, (INT8U)v16_3, (INT8U)v16_4, (INT8U)v16_5);
						   break;
					    case 1:sprintf(string_buf,"%c",'A'+(v8_1+7-1)%7);
						   break;
					    case 2:get_global_sparam(GLOBAL_DATE_WEEKARRAY+(v8_1*10), (INT8U *)string_buf, 10);
						   string_buf[10] = 0; 
						   break;
					    case 3:rtc_makedate_str(string_buf, (INT8U)v16_3, (INT8U)0, (INT8U)v16_5);	    //add by son remove 070126
					    	   break;
					}
					break;
				case FIELD_ID_PACKED_TIME:	// Packed Time = Current Time + hour
#ifdef USE_PRT_DISC_PERCENT
					if(DiscSale.saleDiscountType!=DISCOUNT_NO) continue;
#endif
					if (PrtItemStr.flagpackedontime == 1) continue;
					if (sellbydate_check && PrtItemStr.flagpackedontime == 0) {
						if (PrtItemStr.sellbytime==0) continue;//break;
					}
					#ifdef USE_GSMART_PLU
					if (PrtItemStr.flagpackedontime == 3)	//  3:지정
					{
						v8_1 = PrtItemStr.packedontime / 100;
						v8_2 = PrtItemStr.packedontime % 100;
						v8_3 = 0;
					}
					else
					{
						v8_1 = PrtComStr.time.hour + PrtItemStr.packedontime / 100;	// current hour + packed time
						v8_1 += (PrtComStr.time.min + PrtItemStr.packedontime % 100) / 60;
						v8_2 = (PrtComStr.time.min + PrtItemStr.packedontime % 100) % 60;
						v8_3 = PrtComStr.time.sec;
					}
					#else
					v8_1 = PrtComStr.time.hour + PrtItemStr.packedontime;	// current hour + packed time
					v8_2 = PrtComStr.time.min;
					v8_3 = PrtComStr.time.sec;
					#endif
					v8_1 %= 24;
					switch (time_print_type) {
						default:
						case 0: rtc_maketime_str(string_buf,(INT8U)v8_1,(INT8U)v8_2,(INT8U)v8_3);
							break;
						case 1: 
						   	//SG070312
						   	if(Startup.country==COUNTRY_KR)
						   	{
						   		if(v8_1==0) 	v8_1=6;
						   		else 		v8_1--;
						   	}
							sprintf(string_buf,"%c",'A'+v8_1);
							break;
						case 2: sprintf(string_buf,"%d",v8_1);
							break;
					}
					break;
				case FIELD_ID_WEEK_DAY :	// Packed on Date의 요일
					#ifdef USE_GSMART_PLU
					if(PrtItemStr.packedondate > 2359) continue;
					if (PrtItemStr.flagpackedontime == 3)	//  3:지정
					{
						v8_1 = PrtItemStr.packedontime / 100;
					}
					else
					{
						v8_1 = PrtComStr.time.hour + PrtItemStr.packedontime / 100;	// current hour + packed time
						v8_1 += (PrtComStr.time.min + PrtItemStr.packedontime % 100) / 60;
					}
					#else
					if(PrtItemStr.packedondate > 999) continue;
					v8_1 = PrtComStr.time.hour + PrtItemStr.packedontime;	// current hour + packed time
					#endif
					v8_1 /= 24;		// calculate day from hour( 0~23 = 0, 24~47 = 1 ...)
					v16_1 = (INT16U)v8_1 + PrtItemStr.packedondate;
					v16_3 = (INT16U)PrtComStr.time.year;
					v16_4 = (INT16U)PrtComStr.time.month;
					v16_5 = (INT16U)PrtComStr.time.date;
					date_getnextday((short*)&v16_3,(short*)&v16_4,(short*)&v16_5,(short)v16_1);
					v8_1 = date_getweek(v16_3, v16_4, v16_5);
					get_global_sparam(GLOBAL_DATE_WEEKARRAY+(v8_1*10), (INT8U *)string_buf, 10);
					string_buf[10] = 0;
					break;
				case FIELD_ID_PRODUCED_DATE:	// Produced Date = Current Date - Days + 1
					// 0:no print, 1:yesterday..., 2:Before yesterday, 3:...
					if(PrtItemStr.produceddate) {
						if(PrtItemStr.produceddate > 999) continue;
						//PrtItemStr.produceddate--;	// Added by CJK 20060123
						v16_1 = PrtItemStr.produceddate;
						v16_3 = (INT16U)PrtComStr.time.year;
						v16_4 = (INT16U)PrtComStr.time.month;
						v16_5 = (INT16U)PrtComStr.time.date;
						//v16_1--;       
						date_getbeforeday((short*)&v16_3,(short*)&v16_4,(short*)&v16_5,(short)v16_1);
						rtc_makedate_str(string_buf, (INT8U)v16_3, (INT8U)v16_4, (INT8U)v16_5);
					} else {
						continue;
					}
					break;				
#ifdef USE_JULIAN_DATE
				case FIELD_ID_JULIAN_SELL_BY_DATE: // Sell by Date = Current Date(d) + Packed Date(d) + Days(d) + Current Time(h) + Sell by Time(h) + Packed Time(h)
					if (PrtItemStr.flagsellbydate == 1) continue;
#ifdef USE_SELLBYDATETIME_ZERO
					if (PrtItemStr.sellbydate || PrtItemStr.sellbytime) {		// Sell by Date or Sell by Time존재시
#else
					if (PrtItemStr.sellbydate || PrtItemStr.sellbytime || PrtItemStr.flagsellbydate == 2) { 	// Sell by Date or Sell by Time존재시
#endif
						// Date 계산
						if(PrtItemStr.sellbydate > 9999) {
							v16_3 = (INT16U)(PrtItemStr.sellbydate / 10000L % 100L);
							v16_4 = (INT16U)(PrtItemStr.sellbydate / 100L % 100L);
							v16_5 = (INT16U)(PrtItemStr.sellbydate %100L);
							v16_1 = 0;
						} else {
							v16_3 = (INT16U)PrtComStr.time.year;
							v16_4 = (INT16U)PrtComStr.time.month;
							v16_5 = (INT16U)PrtComStr.time.date;
							v16_1 = (INT16U)PrtItemStr.sellbydate;
							if(v16_1 > 0) v16_1--;	// sell by date  0:no print, 1:today, 2:tomorrow...
							v16_1 = v16_1 + PrtItemStr.packedondate;	// Packed Date + Sell by Date
						}
						// Time 계산
						v8_1 = PrtComStr.time.hour + PrtItemStr.packedontime + PrtItemStr.sellbytime;		// current time + Packed Time + Sell by Time
						v8_1 /= 24;
						v16_1 += (INT16U)v8_1;
						t16_3=v16_3; t16_4=v16_4; t16_5=v16_5;
						
						date_getnextday((short*)&v16_3,(short*)&v16_4,(short*)&v16_5,(short)v16_1);
						v8_1=1;
						if (sellbydate_check && PrtItemStr.flagsellbydate == 0) {
							v8_1=0;
							if ((!((t16_3==v16_3) && (t16_4==v16_4) && (t16_5==v16_5))) || PrtItemStr.sellbydate) v8_1=1;
						}
						//if (v8_1) rtc_makedate_str(string_buf, (INT8U)v16_3, (INT8U)v16_4, (INT8U)v16_5);
						if (v8_1) {
							days = date_getdays((short)v16_3+2000, (short)v16_4, (short)v16_5);
							v16_3 %= 10;
							sprintf(string_buf,"%1d%03d",v16_3, days);
						} else {
						continue;
						}
					}
					break;
#endif
				case FIELD_ID_COOK_BY_DATE:	// Cook by Date = Packed Date + Days
					if (PrtItemStr.cookbydate) {
						if(PrtItemStr.cookbydate > 999) continue;
						v16_3 = (INT16U)PrtComStr.time.year;
						v16_4 = (INT16U)PrtComStr.time.month;
						v16_5 = (INT16U)PrtComStr.time.date;
						// 0:no print, 1:Packed Date, 2:Packed Date + 1, 3:...
						date_getnextday((short*)&v16_3,(short*)&v16_4,(short*)&v16_5,(short)PrtItemStr.cookbydate - 1);
						#ifdef USE_GSMART_PLU
						if (PrtItemStr.flagpackedontime == 3)	//  3:지정
						{
							v8_1 = PrtItemStr.packedontime / 100;
						}
						else
						{
							v8_1 = PrtComStr.time.hour + PrtItemStr.packedontime / 100;	// current hour + packed time
							v8_1 += (PrtComStr.time.min + PrtItemStr.packedontime % 100) / 60;
						}
						#else
						v8_1 = PrtComStr.time.hour + PrtItemStr.packedontime;	// current hour + packed time
						#endif
						v8_1 /= 24;
						v16_1 = (INT16U)v8_1 + PrtItemStr.packedondate;
						date_getnextday((short*)&v16_3,(short*)&v16_4,(short*)&v16_5,(short)v16_1);
						rtc_makedate_str(string_buf, (INT8U)v16_3, (INT8U)v16_4, (INT8U)v16_5);
					} else {
						continue;
					}
					break;				
#ifdef USE_JULIAN_DATE
				case FIELD_ID_JULIAN_PACKED_DATE: // Packed Date = Current Date(d) + Days(d) + Current Time(h) + Packed Time(h)
					// 0:today, 1:tomorrow...
					if (PrtItemStr.flagpackedondate == 1) continue;
					if(PrtItemStr.packedondate > 999) continue;
					v8_1 = PrtComStr.time.hour + PrtItemStr.packedontime;	// current hour + packed time
					v8_1 /= 24; 	// calculate day from hour( 0~23 = 0, 24~47 = 1 ...)
					v16_1 = (INT16U)v8_1 + PrtItemStr.packedondate;
					v16_3 = (INT16U)PrtComStr.time.year;
					v16_4 = (INT16U)PrtComStr.time.month;
					v16_5 = (INT16U)PrtComStr.time.date;
					date_getnextday((short*)&v16_3,(short*)&v16_4,(short*)&v16_5,(short)v16_1);
					v8_1 = date_getweek(v16_3, v16_4, v16_5);
					
					days = date_getdays((short)v16_3+2000, (short)v16_4, (short)v16_5);
					v16_3 %= 10;
					sprintf(string_buf,"%1d%03d",v16_3, days); 
				break;
#endif
				case FIELD_ID_INGREDIENT_STR3: 	// ingredient PAGE3
				case FIELD_ID_INGREDIENT_STR2: 	// ingredient PAGE2
				case FIELD_ID_INGREDIENT_STR: 	// ingredient PAGE1
					DirectSetField = ON;		// Added by CJK 20060524
#ifdef USE_DIRECT_INGREDIENT
					flagDirectIngred = OFF;
#endif
#ifdef USE_DISCOUNT_MEMBERSHIP
					v8_2 = 0; // indirect ingredient
					// 행사 유형 확인(0: 할인행사없음 1: 멤버십할인 2: 카드할인 3: 멤버십+카드할인)
					if (PrtItemStr.disc_type)
					{
						v16_1 = DISC_ID_CAP_BASE + PrtItemStr.disc_type; //할인 유형에 해당하는 문구의 ID 할당
					}
					else
					{
						v16_1 = 0; // 행사 없는 경우 
					}
#else
					switch(PrtItemStr.l_PluNo) {
						case MISC_PLU_WEIGHT :
						case MISC_PLU_PCS    :
						case MISC_PLU_COUNT  :
						case 0:
							v16_1 = 0;
							break;
						default :
							v8_2=0;
#ifdef USE_DIRECT_INGREDIENT//DEL080826#if (ROM_COUNTRY == ROM_RUS)
	#ifndef USE_AUSTRALIA_INGREDIENT//호주에선 direct Ingredient는 호주 전용 CoOL 기능에서 사용.
							plu_get_field(PrtItemStr.plu_addr-1, PTYPE_INGREDIENT_TEXT, (INT8U *)&ingredNo[0], (INT16S *)&v16_2,sizeof(ingredNo));  //Direct Ingredient
							if(ingredNo[0])
							{
								v8_2=1;
								v16_1 = 1;
								flagDirectIngred = ON;
								break;
							}
	#endif
#endif
							plu_get_field(PrtItemStr.plu_addr-1, PTYPE_INGREDIENT_NO, (INT8U *)&v16_1, (INT16S *)&v16_2,sizeof(v16_1));	 //Indirect Ingredient
#ifdef USE_EMART_ETC_COMMON_TEXT
						  	if (fieldid==FIELD_ID_INGREDIENT_STR2)
						  	{
							  	v16_1 = ETC_TEXT_INGREDIENT_TABLE_NUM;
						  	}
#endif
							break;
					}
#endif
					if (v16_1) 
					{
						if(v8_2) //direct ingredient
						{
#ifdef USE_DIRECT_INGREDIENT
							j = 0;
							ingredient_text_read(ingredNo, string_buf2, 0, DEFAULT_INGREDIENT_TEXT_ALL_SIZE);
							for(i = 0; string_buf2[i] != 0 && i < DEFAULT_INGREDIENT_TEXT_ALL_SIZE; i++) {	// Direct Ingredint 의 String을 조사
								if(string_buf2[i] == 0x0c) { 												// 제어문자(FormFeed / 0x0C) 를 만나면 
									if(j < INGREDIENT_FIELD_NUM - 1)	
										len[++j] = i;														// 구분할 수 있는 라벨폼 필드 (Ingrdient 1~3) 이후의 0x0C 제어 문자를 만나면 공백으로 대체 함
									else
										string_buf2[i] = ' ';												// 구분할 수 있는 라벨폼 필드 (Ingrdient 1~3) 이후의 0x0C 제어 문자를 만나면 공백으로 대체 함
								}
							}
							len[++j] = i;																	// Direct Ingredient 의 전체 길이를 배열에 저장 추후 마지막 문자열의 index로 사용 

							v8_3 = 0;
							if(fieldid == FIELD_ID_INGREDIENT_STR)			v8_3 = 0;							// 현재 추력해야 하는 필드 아이디를 구분하여 해당 문자열의 인덱스를 호출할 수 있게 
							else if(fieldid == FIELD_ID_INGREDIENT_STR2)	v8_3 = 1;							// 길이를 저장해둔 배열의 Colmn 값을 변환
							else if(fieldid == FIELD_ID_INGREDIENT_STR3)	v8_3 = 2;

							start = (len[v8_3]) ? len[v8_3] + 1 : 0;											// 상단의 for문에서 저장한 문자길이를 토대로 각 필드별 문자열 인쇄 시작점과 끝점을 추출 
							end = len[v8_3 + 1] - start;
							if(end > 0)
								LabCmd_SetTextField_DirectIngredient(fieldid, string_buf2 + start, end);
							continue;
#endif
						}
						else	//indirect ingredient
						{
#ifdef USE_9999_INDIRECT_INGREDIENT
							ing_id = ingredient_fine_table_set_value(v16_1,0,0);
							addr =  ing_calc_addr_by_key(ing_id-1);
#elif defined(USE_INGREDIENT_CODE_4DIGIT)
							ing_id = ingredient_no2index(v16_1);
							addr =  ing_calc_addr_by_key(ing_id-1); 
#else
							addr =  ing_calc_addr_by_key(v16_1-1);
#endif
							v16_2 = Flash_wread(addr);
							if(v16_1 != v16_2) continue;
							addr += 2;
							ingredSize = INGREDIENT_STRUCT_SIZE-2;
						}

//						if(Flash_check_nvbuffer_4_rd(addr, ingredSize) == 1) {
//							addr = (INT32U)(FlashNVRAMStruct.buffer + (addr&FLASH_BLOCK_LADDR_MASK));
//						}
						v8_1=0;
						Flash_sread(addr, ingredientBuf, MAX_INGREDIENT_SIZE);
						ingredientBuf[MAX_INGREDIENT_SIZE - 1] = 0;
						//addrp = (char *)addr;
						addrp = (char *)ingredientBuf;
						for (v16_2=0; v16_2<ingredSize; v16_2++) {
							if (addrp[v16_2]==0x0c) {
								v8_1 =1;
								v16_1=v16_2;
								break;
							}
							if (addrp[v16_2]==0x0) break;
						}
  #ifndef USE_EMART_ETC_COMMON_TEXT
						if (fieldid==FIELD_ID_INGREDIENT_STR2) {
							if (v8_1==0) continue;		//JJNAG 20070612
							v16_1++;
							addrp+=(INT32U)v16_1;
							v16_2 = ingredSize - v16_1;
						}
  #endif
					} 
					else 
					{
						continue;
					}
					break;
#ifdef USE_PERCENT_WEIGHT
				case FIELD_ID_PERCENT_WEIGHT :
//					if(label_set.en_mark_down_weight == 0) continue;
					if(PrtItemStr.fixed_price == 1 && !PrtItemStr.l_FixedWeight) continue;
					v32_1=PrtItemStr.l_Price.PercentTareWeight;
					if(PrtItemStr.l_PluType <= PLU_BY_WEIGHT) {			// by-weight mode
						v8_1 = get_global_bparam(GLOBAL_PRINT_OPER_FLAG2);
						sprintf(string_buf, "");
						if(v8_1&0x04) {
							sprintf(string_buf, "N ");
						}
						v16_1 = strlen(string_buf);
						v8_1=get_decimal_pt(4, PrtItemStr.plu_addr);
						v8_2=dec_ch;
						PrtGenWeightStr(string_buf+v16_1, v32_1, wt_prt_type, w_sign1, w_sign2, v8_1, v8_2);
					} else {
						continue;
					}
					break;
#endif
#ifdef USE_LOT_NUMBER_FUNCTION
				case FIELD_ID_LOT_NUMBER :
					addr =	DFLASH_BASE + FLASH_LOT_NUMBER;
					Flash_sread(addr, (HUGEDATA INT8U *)string_buf, 20);
					string_buf[20]=0;
					break;
#endif
				default :
					continue;
			}

			if(DirectSetField) {
				LabCmd_SetTextField_Direct(fieldid,(char *)addrp,v16_2);
			} else {
	 			LabCmd_SetTextField(fieldid,string_buf);
			}
			//string_buf[0] = 0;
		}
	}
}

void prtfield_generate_item(void)
{
	INT16S fieldid;
	INT8U select,v8_1,v8_2;
	INT16U v16_1, v16_2;
	INT32U addr;
	INT32U l_v;
#ifdef USE_DISCOUNT_MEMBERSHIP
	INT32U price_dsc1, price_dsc2, price_dsc_total;
	INT16U ingredSize;
	INT8U check_disc_print;
	INT8U ingredientBuf[MAX_INGREDIENT_SIZE];
#endif
	INT8U price_dec, dec_ch;
	char str[10];
	char string_buf[128];
#ifdef USE_EXTEND_PRICE_PRIMARYSIGN
	char currency_sign1[6];
#else
	char currency_sign1[5];
#endif
	INT8U wt_prt_type, DirectSetField;
	char w_sign1[5], w_sign2[5];
	INT8U useFSP;
	INT8U use_memberprice_flag;	

	useFSP = get_global_bparam(GLOBAL_SALE_SETUP_FLAG6);
	useFSP = (useFSP>>4)&0x01; // FSP Parameter
	if(DiscSale.saleDiscFSP && useFSP) useFSP=1;
	else useFSP=0;   			  	
    
	use_memberprice_flag = get_global_bparam(GLOBAL_SALE_SETUP_FLAG18) & 0x80;	//parameter 729
#ifdef USE_DISCOUNT_MEMBERSHIP
	check_disc_print = OFF;
	// 현재 라벨이 보관라벨/축산세트라벨인 경우 현재 라벨 인쇄
	// 현재 라벨이 보관라벨/축산 세트라벨이 아닌 경우 할인 라벨 인쇄
	if (status_scale.cur_labelid != MEAT_BACKLABEL_ID && status_scale.cur_labelid != IMPORT_MEAT_BACKLABEL_ID && status_scale.cur_labelid != MEAT_SETLABEL_ID)
	{
		check_disc_print = ON;
	}
#endif

	price_dec     = get_global_bparam(GLOBAL_AREA_PRICE_DECIMALPOINT);
	dec_ch        = get_global_bparam(GLOBAL_DECIMAL_SIGN);
	wt_prt_type = 0;
	if(PrtWeightUnit == WEIGHT_UNIT_KG || PrtWeightUnit == WEIGHT_UNIT_G) {
		get_global_sparam(GLOBAL_WEIGHT_SYMBOL_1, (INT8U *)w_sign1, 4);
		get_global_sparam(GLOBAL_WEIGHT_SYMBOL_2, (INT8U *)w_sign2, 4);
	} else {
		get_global_sparam(GLOBAL_WEIGHT_LBSYMBOL_1, (INT8U *)w_sign1, 4);
		get_global_sparam(GLOBAL_WEIGHT_LBSYMBOL_2, (INT8U *)w_sign2, 4);
	}
	w_sign1[4] = 0;
	w_sign2[4] = 0;
	for (fieldid=0; fieldid<FIELD_ID_N_SERVING_SIZE; fieldid++) {
		string_buf[0] = 0;
		DirectSetField = OFF;
		select = prtsel_get_table(fieldid);
		if (StrFieldStruct[fieldid].ExistField && select) {
#ifdef USE_SETPRODUCT_SALE
			if(SetproductSaleMode && setproduct_PrtFlag)
				if(fieldid != FIELD_ID_TOTALPRICE)
					continue;
#endif
			switch (fieldid) {
				case FIELD_ID_TRACEABILITY_CODE:	// Pass Next field
					fieldid = FIELD_ID_CUTTING_CODE;
					continue;
				case FIELD_ID_TRANSACTION_NO :
#ifdef USE_DISCOUNT_MEMBERSHIP // 할인율로 사용
					v16_1 = 0;
					switch (PrtItemStr.disc_type) {
						case MEMBERSHIP_DISC : // 멤버십
							plu_get_field(PrtItemStr.plu_addr-1, PTYPE_DISCOUNT_RATE1, (INT8U *)&v16_1, (INT16S *)&v16_1, sizeof(v16_1));
							break;
						case CARD_DISC :	// 카드할인
							plu_get_field(PrtItemStr.plu_addr-1, PTYPE_DISCOUNT_RATE2, (INT8U *)&v16_1, (INT16S *)&v16_1, sizeof(v16_1));
							break;
						case  MEMBERSHIP_CARD_DISC : // 멤버십 + 카드할인
							// 멤버십 할인율
							plu_get_field(PrtItemStr.plu_addr-1, PTYPE_DISCOUNT_RATE1, (INT8U *)&v16_1, (INT16S *)&v16_1, sizeof(v16_1));
							// 카드 할인율 
							plu_get_field(PrtItemStr.plu_addr-1, PTYPE_DISCOUNT_RATE2, (INT8U *)&v16_2, (INT16S *)&v16_2, sizeof(v16_2));

							l_v = PrtItemStr.l_Price.TotalPrice[PRICE_RESULT];

							price_dsc1 = Prt_Calc_Discount_Price(l_v, v16_1); // 멤버십 할인 가격
							price_dsc2 = Prt_Calc_Discount_Price(l_v - price_dsc1, v16_2); // 카드 할인 가격
							price_dsc_total = price_dsc1 + price_dsc2; // 최종할인가격

							v16_1 = (INT16U)((price_dsc_total * 100L) / l_v); // 할인율
							break;
						default :
							break;
					}
					if (PrtItemStr.disc_type) sprintf(string_buf,"%d%%", v16_1);
					break;
#else	// Pass Label Serial Number
					continue;
#endif
/////////////////////////////////////////////////////
				case FIELD_ID_NUM_PREFIX:
					switch(PrtItemStr.l_PluNo) {
						case MISC_PLU_WEIGHT :
						case MISC_PLU_PCS    :
						case MISC_PLU_COUNT  :
						case 0:
							continue;
						default :
							if (Startup.country==COUNTRY_KR) {
								plu_get_field((INT16S)PrtItemStr.plu_addr-1, PTYPE_PLU_PREFIX, (INT8U *)&v16_1, (INT16S *)&v16_2,sizeof(str)); 
								sprintf(string_buf,"%d",v16_1);
							} else {
								plu_get_field((INT16S)PrtItemStr.plu_addr-1, PTYPE_PLU_PREFIX, (INT8U *)&str, (INT16S *)&v16_2,sizeof(str)); 
								sprintf(string_buf,"%c%c",str[0],str[1]);
							}
							break;
					}
					break;
				case FIELD_ID_WEIGHT_UNIT:
					if(PrtWeightUnit == WEIGHT_UNIT_LB) {
						switch(PrtWeightUnitId)  {
							default:
							case 1:	label_caption_read(LABCAP_ID_TXT_1LB, string_buf);	break;
							case 2:	label_caption_read(LABCAP_ID_TXT_1_2LB, string_buf);	break;
							case 3:	label_caption_read(LABCAP_ID_TXT_1_4LB, string_buf);	break;
						}
					} else  {
						switch(PrtWeightUnitId)  {
							default:
							case 1:	label_caption_read(LABCAP_ID_TXT_1KG, string_buf);	break;
							case 2:	label_caption_read(LABCAP_ID_TXT_100G, string_buf);	break;
#ifdef USE_WEIGHT_UNIT_500G
							case 3: label_caption_read(LABCAP_ID_TXT_500G, string_buf); break;
#endif
						}
					}
					break;
				case FIELD_ID_UNIT_PRICE: 	// unit price
#ifdef USE_ARABIC_NUMBER
				case FIELD_ID_2ND_UNIT_PRICE:
#endif //USE_ARABIC_NUMBER				
					if((label_set.en_mark_down_unit==1) && (!useFSP)) continue;
#ifndef USE_CHINA_DEFAULT
					if(PrtItemStr.fixed_price == 1) continue;
#endif
					if(label_set.flagZeroUnitPrice) continue;
#ifdef USE_SETPRODUCT_SALE
					if(SetproductSaleMode && !setproduct_PrtFlag) continue;
					if(nonweighing_mode) continue;
#endif
					if(Startup.country == COUNTRY_US)	//NTEP
					{
						v16_1 = 0;
						if(prtsel_get_table(FIELD_ID_UNIT_PCS)) {
							if(label_set.flagPcsInUnit && PrtItemStr.l_PluType == PLU_BY_PCS) {			// by-count mode
								sprintf(string_buf,"%d/", PrtItemStr.l_Price.UnitCount[PRICE_RESULT]);
								v16_1 = strlen(string_buf);
							}
						}
					}
					l_v=PrtItemStr.l_Price.UnitPrice[PRICE_RESULT];
					v8_1=price_dec;
					v8_2=dec_ch;
					if(Startup.country == COUNTRY_US) {
						if (Prt_uprice_Symbol_flag) {
							get_global_sparam(GLOBAL_AREA_PRICE_PRIMARYSIGN,(INT8U *)currency_sign1,4);
							currency_sign1[4] = 0;
							PrtGenCurPriceStr((char*)string_buf+v16_1, l_v, 2, currency_sign1, currency_sign1, v8_1, v8_2);
						}
						else {
							format_value((HUGEDATA INT8U *)string_buf+v16_1,l_v,v8_1,v8_2);
						}
					}
					else
					{
#ifndef USE_INVENTORY_LABEL
#ifdef USE_INDIA_PRINT_UNIT
						get_global_sparam(GLOBAL_AREA_PRICE_PRIMARYSIGN,(INT8U *)string_buf,1);
						format_value((HUGEDATA INT8U *)string_buf+1,l_v,v8_1,v8_2);
						if(!prtsel_get_table(FIELD_ID_UNIT_PCS)) break;
						if(label_set.flagPcsInUnit && PrtItemStr.l_PluType == PLU_BY_PCS) {			// by-count mode
							v8_1 = strlen(string_buf);
//							sprintf(string_buf+v8_1,"/%d", PrtItemStr.l_Price.UnitCount[PRICE_RESULT]);
							sprintf(string_buf+v8_1,"/");
                            v8_1++;
                            get_global_sparam(GLOBAL_PCS_SYMBOL,(INT8U *)string_buf+v8_1,16);
						}
						else if(PrtItemStr.l_PluType <= PLU_BY_WEIGHT)
                        {           // by-weight mode
							v8_1 = strlen(string_buf);
							if(CurPluWeightUnit == WEIGHT_UNIT_KG)
								sprintf(string_buf+v8_1,"/%s", w_sign1);
							else if(CurPluWeightUnit == WEIGHT_UNIT_G)
								sprintf(string_buf+v8_1,"/%s", w_sign2);
						}
                        else if(PrtItemStr.l_PluType == PLU_BY_COUNT)
                        {
                            v8_1 = strlen(string_buf);
                            sprintf(string_buf+v8_1,"/");
                            v8_1++;
                            get_global_sparam(GLOBAL_PCS_SYMBOL,(INT8U *)string_buf+v8_1,16);
                        }
#else
						PrtGenCurUnitPriceStr((char*)string_buf, l_v, Prt_uprice_Symbol_flag, v8_1, v8_2, label_set.flagPcsInUnit
								, PrtItemStr.l_PluType, PrtItemStr.cur_pcssym_no, PrtItemStr.l_Price.UnitCount[PRICE_RESULT]);
#endif
#else
						sprintf(string_buf, "%d", l_v);	//수량 직접 입력 라벨 발행의 경우.
#endif
					}
					break;
				case FIELD_ID_NEW_UNITPRICE: 	// unit price
#ifdef USE_SPAR_MEMBER_PRICE
                    plu_get_field((INT16U)PrtItemStr.plu_addr-1, PTYPE_MEMBER_PRICE, (INT8U *)&l_v, (INT16S *)&v16_1,sizeof(l_v));
                    if(l_v)
                    {
                        if(label_set.en_mark_down_unit == 1) continue;
						l_v=MemberPrice.UnitPrice[PRICE_RESULT];
						v8_1=price_dec;
						v8_2=dec_ch;
						format_value(( INT8U *)string_buf,l_v,v8_1,v8_2);
						if(label_set.flagPcsInUnit && PrtItemStr.l_PluType == PLU_BY_PCS) { 		// by-count mode
							v8_1 = strlen(string_buf);
							sprintf(string_buf+v8_1,"/%d", PrtItemStr.l_Price.UnitCount[PRICE_RESULT]);
						}
                    }                    
#else
					if(use_memberprice_flag)
					{
						if(label_set.en_mark_down_unit == 1) continue;
						l_v=MemberPrice.UnitPrice[PRICE_RESULT];
						v8_1=price_dec;
						v8_2=dec_ch;
						format_value(( INT8U *)string_buf,l_v,v8_1,v8_2);
						if(label_set.flagPcsInUnit && PrtItemStr.l_PluType == PLU_BY_PCS) { 		// by-count mode
							v8_1 = strlen(string_buf);
							sprintf(string_buf+v8_1,"/%d", PrtItemStr.l_Price.UnitCount[PRICE_RESULT]);
						}
					}
#endif
					else
					{
					if(label_set.en_mark_down_unit == 0) continue;
					if(PrtItemStr.fixed_price == 1) continue;
					if(label_set.flagZeroUnitPrice) continue;
					if(Startup.country == COUNTRY_US)
					{
						v16_1 = 0;
						if(prtsel_get_table(FIELD_ID_UNIT_PCS)) {
							if(label_set.flagPcsInUnit && PrtItemStr.l_PluType == PLU_BY_PCS) {			// by-count mode
								sprintf(string_buf,"%d/", PrtItemStr.l_Price.UnitCount[PRICE_RESULT]);
								v16_1 = strlen(string_buf);
							}
						}
					}
					if(GlbOper.UseFSP && DiscSale.saleDiscFSP) l_v = PriceFSP.UnitPrice[PRICE_RESULT]; //SG061214
					else	l_v=PrtItemStr.l_Price.UnitPrice[PRICE_RESULT];
					v8_1=price_dec;
					v8_2=dec_ch;
					if(Startup.country == COUNTRY_US) {
						if (Prt_uprice_Symbol_flag) {
							get_global_sparam(GLOBAL_AREA_PRICE_PRIMARYSIGN,(INT8U *)currency_sign1,4);
							currency_sign1[4] = 0;
							PrtGenCurPriceStr((char*)string_buf+v16_1, l_v, 2, currency_sign1, currency_sign1, v8_1, v8_2);
						}
						else {
							format_value((HUGEDATA INT8U *)string_buf+v16_1,l_v,v8_1,v8_2);
						}
					}
					else
					{
						PrtGenCurUnitPriceStr((char*)string_buf, l_v, Prt_uprice_Symbol_flag, v8_1, v8_2, label_set.flagPcsInUnit
								, PrtItemStr.l_PluType, PrtItemStr.cur_pcssym_no, PrtItemStr.l_Price.UnitCount[PRICE_RESULT]);
					}
					}
					break;
				case FIELD_ID_MARK_DN_UNIT:
					if((label_set.en_mark_down_unit==0) || useFSP ) continue;
					if(PrtItemStr.fixed_price == 1) continue;
					l_v=PrtItemStr.l_Price.UnitPrice[PRICE_ORIGIN];
					v8_1=price_dec;
					v8_2=dec_ch;
					v16_1 = 0;
					// 20081022 modified by JJANG, Parameter 560 : 1일때 마크다운을 출력하지 않는다. 
#ifndef USE_TOPMART_STRUCTURE
					if (!(Operation.specialPriceDisc && status_scale.Plu.special_price))
					{
						string_buf[v16_1++] = FONT_CON_CH;
						string_buf[v16_1++] = MarkDownLineNum;
					}
#endif
					if(Startup.country == COUNTRY_US) //NTEP
					{
						if(prtsel_get_table(FIELD_ID_UNIT_PCS)) {
							if(label_set.flagPcsInUnit && PrtItemStr.l_PluType == PLU_BY_PCS) {			// by-count mode
								sprintf(string_buf+v16_1,"%d/", PrtItemStr.l_Price.UnitCount[PRICE_ORIGIN]);
								v16_1 += strlen(string_buf+v16_1);
							}
						}
						if (Prt_uprice_Symbol_flag) {
							get_global_sparam(GLOBAL_AREA_PRICE_PRIMARYSIGN,(INT8U *)currency_sign1,4);
							currency_sign1[4] = 0;
							PrtGenCurPriceStr((char*)string_buf+v16_1, l_v, 2, currency_sign1, currency_sign1, v8_1, v8_2);
						}
						else {
							format_value((HUGEDATA INT8U *)string_buf+v16_1,l_v,v8_1,v8_2);
						}
					}
					else
					{
						PrtGenCurUnitPriceStr((char*)string_buf+v16_1, l_v, Prt_uprice_Symbol_flag, v8_1, v8_2, label_set.flagPcsInUnit
								, PrtItemStr.l_PluType, PrtItemStr.cur_pcssym_no, PrtItemStr.l_Price.UnitCount[PRICE_RESULT]);
					}
					break;
				//Not use all Tax를 선택해도 MISC_TAX가 출력 가능
#ifdef USE_TAX_RATE                    
				case FIELD_ID_TAX_RATE:	// Tax Rate	
					switch(PrtItemStr.l_PluNo)		//Modified by JJANG 20070614
					{
						case MISC_PLU_WEIGHT :
						case MISC_PLU_PCS    : 
						case MISC_PLU_COUNT  :
							if(!get_global_bparam(GLOBAL_MISC_TAX_VALUE))continue;	 
							format_value((HUGEDATA INT8U *)string_buf,PrtItemStr.l_Price.TaxRate,2,'.');
							v8_1 = strlen(string_buf);
							sprintf(string_buf+v8_1, "%%");
							break;
						default	: 
							if(!get_global_bparam(GLOBAL_TAX_FLAG)) continue;	//0:No Tax, 1:PLU Only, 2:Global Only
							format_value((HUGEDATA INT8U *)string_buf,PrtItemStr.l_Price.TaxRate,2,'.');
							v8_1 = strlen(string_buf);
							sprintf(string_buf+v8_1, "%%");
							break;
					}break;
#endif
#ifdef USE_TAX_RATE                    
				case FIELD_ID_TAX_PRICE:		
					switch(PrtItemStr.l_PluNo)		//Modified by JJANG 20070614
					{
						case MISC_PLU_WEIGHT :
						case MISC_PLU_PCS    : 
						case MISC_PLU_COUNT  :
							if(!get_global_bparam(GLOBAL_MISC_TAX_VALUE)) continue;
							l_v = PrtItemStr.l_Price.TaxPrice;
							v8_1=price_dec;
							v8_2=dec_ch;
							format_value((HUGEDATA INT8U *)string_buf,l_v,v8_1,v8_2);
							break;
						default	: 
							if(!get_global_bparam(GLOBAL_TAX_FLAG)) continue;	//0:No Tax, 1:PLU Only, 2:Global Only
							l_v = PrtItemStr.l_Price.TaxPrice;
							v8_1=price_dec;
							v8_2=dec_ch;
							format_value((HUGEDATA INT8U *)string_buf,l_v,v8_1,v8_2);
							break;
					}break;
#endif                    
				case FIELD_ID_GROUP_CODE:
					switch(PrtItemStr.l_PluNo) {
						case MISC_PLU_WEIGHT :
						case MISC_PLU_PCS    :
						case MISC_PLU_COUNT  :
						case 0:
							v16_1 = 0;
							break;
						default :
							plu_get_field(PrtItemStr.plu_addr-1, PTYPE_GROUP_NO, (INT8U *)&v16_1, (INT16S *)&v16_2,sizeof(v16_1));
							break;
					}
					sprintf(string_buf, "%d", v16_1);
					break;
#ifdef USE_KOR_TRACE
// START // Korean Traceability
				case FIELD_ID_INDIV_MSG_1:	// 개체정보 1
					if(KorTrace_Flag.flagPrint == 0) continue;
					switch(PrtItemStr.l_PluNo) {
						case MISC_PLU_WEIGHT :
						case MISC_PLU_PCS	 :
						case MISC_PLU_COUNT  :
						case 0:
							continue;
						default :
							if(KorTrace_Flag.flagPrint ==1){
								v16_2 = sizeof(KorTrace_ReceiveData.traceid_printdata[0]);
								memcpy(string_buf, &KorTrace_ReceiveData.traceid_printdata[0][0], 64); //SG060331  strlen((char*)KorTrace_ReceiveData.traceid_printdata[0]));
							} else if(KorTrace_Flag.flagPrint ==2){
								v16_2 = sizeof(KorTrace_ReceiveCmd.traceid_printdata[0]);
								memcpy(string_buf, &KorTrace_ReceiveCmd.traceid_printdata[0][0], 64); //strlen((char*)KorTrace_ReceiveCmd.traceid_printdata[0]));
								}
							string_buf[v16_2] = 0;
							break;
					}
					break;
				case FIELD_ID_INDIV_MSG_2:	// 개체정보 2
					if(KorTrace_Flag.flagPrint == 0) continue;
					switch(PrtItemStr.l_PluNo) {
						case MISC_PLU_WEIGHT :
						case MISC_PLU_PCS	 :
						case MISC_PLU_COUNT  :
						case 0:
							continue;
						default :
							if(KorTrace_Flag.flagPrint ==1){
								v16_2 = sizeof(KorTrace_ReceiveData.traceid_printdata[1]);
								memcpy(string_buf, &KorTrace_ReceiveData.traceid_printdata[1][0], 64); //SG060331  strlen((char*)&KorTrace_ReceiveData.traceid_printdata[1][0]));
							} else if(KorTrace_Flag.flagPrint ==2){
								v16_2 = sizeof(KorTrace_ReceiveCmd.traceid_printdata[1]);
								memcpy(string_buf, &KorTrace_ReceiveCmd.traceid_printdata[1][0], 64); //strlen((char*)&KorTrace_ReceiveCmd.traceid_printdata[1][0]));
							}
							string_buf[v16_2] = 0;
							break;
					}
					break;
				case FIELD_ID_INDIV_MSG_3:	// 개체정보 3
					if(KorTrace_Flag.flagPrint == 0) continue;
					switch(PrtItemStr.l_PluNo) {
						case MISC_PLU_WEIGHT :
						case MISC_PLU_PCS	 :
						case MISC_PLU_COUNT  :
						case 0:
							continue;
						default :
							if(KorTrace_Flag.flagPrint ==1){
								v16_2 = sizeof(KorTrace_ReceiveData.traceid_printdata[2]);
								memcpy(string_buf, &KorTrace_ReceiveData.traceid_printdata[2][0], 64); //SG060331  strlen((char*)&KorTrace_ReceiveData.traceid_printdata[2][0]));
							} else if(KorTrace_Flag.flagPrint ==2){
								v16_2 = sizeof(KorTrace_ReceiveCmd.traceid_printdata[2]);
								memcpy(string_buf, &KorTrace_ReceiveCmd.traceid_printdata[2][0], 64); //strlen((char*)&KorTrace_ReceiveCmd.traceid_printdata[2][0]));
							}
							string_buf[v16_2] = 0;
							break;
					}
					break;
				case FIELD_ID_INDIV_MSG_4:	// 개체정보 4
					if(KorTrace_Flag.flagPrint == 0) continue;
					switch(PrtItemStr.l_PluNo) {
						case MISC_PLU_WEIGHT :
						case MISC_PLU_PCS	 :
						case MISC_PLU_COUNT  :
						case 0:
							continue;
						default :
							if(KorTrace_Flag.flagPrint ==1){
								v16_2 = sizeof(KorTrace_ReceiveData.traceid_printdata[3]);
								memcpy(string_buf, &KorTrace_ReceiveData.traceid_printdata[3][0], 64); //SG060331  strlen((char*)&KorTrace_ReceiveData.traceid_printdata[3][0]));
							} else if(KorTrace_Flag.flagPrint ==2){
								v16_2 = sizeof(KorTrace_ReceiveCmd.traceid_printdata[3]);
								memcpy(string_buf, &KorTrace_ReceiveCmd.traceid_printdata[3][0], 64); //strlen((char*)&KorTrace_ReceiveCmd.traceid_printdata[3][0]));
							}
							string_buf[v16_2] = 0;
							break;
					}
					break;
				case FIELD_ID_INDIV_MSG_5:	  //개체 정보 5
					if(KorTrace_Flag.flagPrint == 0) continue;
					switch(PrtItemStr.l_PluNo) {
						case MISC_PLU_WEIGHT :
						case MISC_PLU_PCS	 :
						case MISC_PLU_COUNT  :
						case 0:
							continue;
						default :
							if(KorTrace_Flag.flagPrint ==1){
								v16_2 = sizeof(KorTrace_ReceiveData.traceid_printdata[4]);
								memcpy(string_buf, &KorTrace_ReceiveData.traceid_printdata[4][0], 64); //SG060331  strlen((char*)&KorTrace_ReceiveData.traceid_printdata[4][0]));
							} else if(KorTrace_Flag.flagPrint ==2){
								v16_2 = sizeof(KorTrace_ReceiveCmd.traceid_printdata[4]);
								memcpy(string_buf, &KorTrace_ReceiveCmd.traceid_printdata[4][0], 64); //strlen((char*)&KorTrace_ReceiveCmd.traceid_printdata[4][0]));
							}
							string_buf[v16_2] = 0;
							break;
					}

					break;
				case FIELD_ID_INDIV_MSG_6:	  //개체 정보 6 : Member Code로 사용
					//if(KorTrace_Flag.flagPrint == 0) continue;
					switch(PrtItemStr.l_PluNo) {
						case MISC_PLU_WEIGHT :
						case MISC_PLU_PCS	 :
						case MISC_PLU_COUNT  :
						case 0:
							continue;
						default :
							v16_2 = sizeof(KorTrace_Flag.memberCode);
							memcpy(string_buf, KorTrace_Flag.memberCode, v16_2);
							string_buf[v16_2] = 0;
							break;
					}

					break;
// END // Korean Traceability
#endif
				case FIELD_ID_TARE_VALUE:
					if(PrtItemStr.fixed_price == 1) continue;
					if(PrtItemStr.l_PluType == PLU_BY_WEIGHT) {			// by-weight mode
						v8_1 = get_global_bparam(GLOBAL_PRINT_OPER_FLAG2);
#ifndef USE_PERCENT_WEIGHT
						if((v8_1&0x02) && (PrtItemStr.l_PluTareWeight || PrtItemStr.l_KeyTareWeight|| PrtItemStr.l_Price.PercentTareWeight)) {
#else
						if((v8_1&0x02) && (PrtItemStr.l_PluTareWeight || PrtItemStr.l_KeyTareWeight)) {
#endif
							sprintf(string_buf, "PT ");
						} else {
							sprintf(string_buf, "");
						}
						v16_1 = strlen(string_buf);
						v8_1=get_decimal_pt(4, PrtItemStr.plu_addr);
						v8_2 = dec_ch;
						if (Prt_weight_Symbol_flag == 0)
							wt_prt_type = 0;
						else if (Prt_weight_Symbol_flag == 1)
							wt_prt_type = 2;
						else if (Prt_weight_Symbol_flag == 2)
							wt_prt_type = 5;
						PrtGenWeightStr(string_buf+v16_1, PrtItemStr.l_TareWeight, wt_prt_type, w_sign1, w_sign2, v8_1, v8_2);	// 20060315
#ifdef USE_INDIA_PRINT_UNIT
						v8_1 = strlen(string_buf);
						if(CurPluWeightUnit == WEIGHT_UNIT_KG)
							get_global_sparam(GLOBAL_WEIGHT_SYMBOL_1,(INT8U *)string_buf+v8_1,16);
						else if(CurPluWeightUnit == WEIGHT_UNIT_G)
							get_global_sparam(GLOBAL_WEIGHT_SYMBOL_2,(INT8U *)string_buf+v8_1,16);
						string_buf[v8_1+16] = 0;
#endif
					} else {
						continue;
					}
					break;
				case FIELD_ID_UNIT_PCS:
					if(PrtItemStr.l_PluType == PLU_BY_PCS) {	// by-count
						v16_1 = PrtItemStr.l_Price.UnitCount[PRICE_RESULT];
						sprintf(string_buf,"%d", v16_1);
					} else {
						continue;
					}
					break;
				case FIELD_ID_PCS_SYM_STR:
					if(PrtItemStr.l_PluType <= PLU_BY_WEIGHT ) {
						continue;
					} else {
						if(PrtItemStr.cur_pcssym_no == 0) {
							get_global_sparam(GLOBAL_PCS_SYMBOL,(INT8U *)string_buf,16);	// Added by CJK 20050503
							string_buf[16] = 0;
						} else {
							addr =  DFLASH_BASE + FLASH_PCS_UNIT_AREA;
							addr =  addr + PCS_UNIT_STRUCT_SIZE * (INT32U)(PrtItemStr.cur_pcssym_no-1);	// code-1 (?)
							v16_1 = Flash_wread(addr);
							if(PrtItemStr.cur_pcssym_no != (INT8U)v16_1) continue;
							addr += 2;
							Flash_sread(addr, (HUGEDATA INT8U *)string_buf, PCS_UNIT_STRUCT_SIZE-2);
						}
					}
					break;
				case FIELD_ID_BONUS_POINT:
#ifndef USE_AUSTRALIA_COOL_FUNCTION
					sprintf(string_buf,"%d", PrtItemStr.bonus_point);
#endif
					break;
				case FIELD_ID_ORIGIN_STR:	// Origin(55)
					if (PrtItemStr.origin) {
						GetOriginName(PrtItemStr.origin, string_buf);
					} else {
						continue;
					}
					break;
				case FIELD_ID_LABEL_MSG_STR:	// Label Msg : Advertisement Msg
#ifdef USE_DISCOUNT_MEMBERSHIP
					// 롯데마트는 판매 문구(셀링포인트)로 사용
					// 성분 1~150번 해당 기능으로 사용
					// 성분 160 번 이후는 행사 관련 고정 문구로 사용
					switch(PrtItemStr.l_PluNo)
					{
						case MISC_PLU_WEIGHT :
						case MISC_PLU_PCS	 :
						case MISC_PLU_COUNT  :
						case 0:
							v16_1 = 0;
							break;
						default :
							plu_get_field(PrtItemStr.plu_addr-1, PTYPE_INGREDIENT_NO, (INT8U *)&v16_1, (INT16S *)&v16_2,sizeof(v16_1));  //Indirect Ingredient
							break;
					}
					if (v16_1 && (v16_1 <= MAX_SALE_MSG_NO))// 성분 범위 체크
					{
						DirectSetField = ON;
						
						addr =	ing_calc_addr_by_key(v16_1 - 1);
						v16_2 = Flash_wread(addr);
						if(v16_1 != v16_2) continue;
						addr += 2;
						ingredSize = INGREDIENT_STRUCT_SIZE - 2;

						//if(Flash_check_nvbuffer_4_rd(addr, ingredSize) == 1)
						//{
						//	addr = (INT32U)(FlashNVRAMStruct.buffer + (addr&FLASH_BLOCK_LADDR_MASK));
						//}

						v8_1=0;
						Flash_sread(addr, ingredientBuf, MAX_INGREDIENT_SIZE);
						ingredientBuf[MAX_INGREDIENT_SIZE - 1] = 0;
						for (v16_2 = 0; v16_2 < ingredSize; v16_2++)
						{
							if (ingredientBuf[v16_2] == 0x00) break;
						}
					}
					else
					{
						continue;
					}
#else
					switch(PrtItemStr.l_PluNo) {
						case MISC_PLU_WEIGHT :
						case MISC_PLU_PCS    :
						case MISC_PLU_COUNT  :
						case 0:
							v8_1 = 0;
							break;
						default :
							v8_1 = status_scale.Plu.sales_msg;	//JJANG 20090320
							break;
					}
					if (v8_1) {
						addr =  DFLASH_BASE + FLASH_PRT_ADVE_AREA;
						addr =  addr + PRT_ADVE_STRUCT_SIZE * (INT32U)(v8_1-1);	// code-1 (?)
						v16_2 = Flash_wread(addr);
						if(v8_1 != (INT8U)v16_2) continue;
						addr += 2;
						Flash_sread(addr, (HUGEDATA INT8U *)string_buf, PRT_ADVE_STRUCT_SIZE-2);
					} else {
						continue;
					}
#endif
					break;
				case FIELD_ID_PERCENT_TARE :
					switch(PrtItemStr.l_PluNo) {
						case MISC_PLU_WEIGHT :
						case MISC_PLU_PCS    :
						case MISC_PLU_COUNT  :
						case 0:
							continue;
						default :
							plu_get_field(PrtItemStr.plu_addr-1, PTYPE_PERCENT_TARE_LIMIT, (INT8U *)&v16_1, (INT16S *)&v16_2,sizeof(v16_1));
							break;
					}
#ifdef USE_PERCENT_WEIGHT
					if(PrtItemStr.l_Price.PercentTareRate)
						l_v = 10000 - PrtItemStr.l_Price.PercentTareRate;
					else
						l_v = 0;
#else
					l_v = PrtItemStr.l_Price.PercentTareRate;
#endif
					if(!v16_1 && !l_v) continue;		// Tare%가 0, Limit%가 0 일때 인쇄X
					v8_2=dec_ch;
					format_value((HUGEDATA INT8U *)string_buf,l_v,2,v8_2);
					break;
				case FIELD_ID_PERCENT_TARE_LIMIT :
					switch(PrtItemStr.l_PluNo) {
						case MISC_PLU_WEIGHT :
						case MISC_PLU_PCS    :
						case MISC_PLU_COUNT  :
						case 0:
							continue;
						default :
							plu_get_field(PrtItemStr.plu_addr-1, PTYPE_PERCENT_TARE_LIMIT, (INT8U *)&v16_1, (INT16S *)&v16_2,sizeof(v16_1));
							break;
					}
					if(!v16_1) continue;			// Limit%가 0일때 인쇄X
					l_v = (INT32U)v16_1;
					v8_2=dec_ch;
					format_value((HUGEDATA INT8U *)string_buf,l_v,2,v8_2);
					break;
				case FIELD_ID_WEIGHT :	// weight
#ifdef USE_ARABIC_NUMBER
				case FIELD_ID_NET_WEIGHT:
#endif //USE_ARABIC_NUMBER				
					if((label_set.en_mark_down_weight==1) && (!useFSP)) continue;
					if(PrtItemStr.fixed_price == 1 && !PrtItemStr.l_FixedWeight) continue;
#ifdef USE_SETPRODUCT_SALE
					if(SetproductSaleMode && setproduct_PrtFlag) continue;
					if(nonweighing_mode && (status_scale.Weight <= 0)) continue;
#endif
					l_v=PrtItemStr.l_Price.Weight[PRICE_RESULT];
					if (Prt_weight_Symbol_flag == 0)
						wt_prt_type = 0;
					else if (Prt_weight_Symbol_flag == 1)
						wt_prt_type = 2;
					else if (Prt_weight_Symbol_flag == 2)
						wt_prt_type = 5;
					if(PrtItemStr.l_PluType <= PLU_BY_WEIGHT) {			// by-weight mode
						v8_1 = get_global_bparam(GLOBAL_PRINT_OPER_FLAG2);
						sprintf(string_buf, "");
						if(v8_1&0x04) {
							sprintf(string_buf, "N ");
						}
						v16_1 = strlen(string_buf);
#ifdef USE_PRT_CONVERT_WEIGHT_KG_G
						if(!GlbOper.EnableGramWeight)
						{
							v8_1=get_decimal_pt(4, PrtItemStr.plu_addr);
						}
						else
						{
							v8_1=0;
						}
#else
						v8_1=get_decimal_pt(4, PrtItemStr.plu_addr);
#endif
						v8_2=dec_ch;
						if(l_v < 0) continue;
#ifdef USE_PRT_CONVERT_WEIGHT_KG_G
						if(!GlbOper.EnableGramWeight)
						{
							PrtGenWeightStr(string_buf+v16_1, l_v, wt_prt_type, w_sign1, w_sign2, v8_1, v8_2);
						}
						else
						{
							PrtGenWeightStr(string_buf+v16_1, l_v, wt_prt_type, w_sign2, w_sign2, v8_1, v8_2);
						}
#else
						PrtGenWeightStr(string_buf+v16_1, l_v, wt_prt_type, w_sign1, w_sign2, v8_1, v8_2);
#endif
#ifdef USE_INDIA_PRINT_UNIT						
						v8_1 = strlen(string_buf);
						if(CurPluWeightUnit == WEIGHT_UNIT_KG)
							get_global_sparam(GLOBAL_WEIGHT_SYMBOL_1,(INT8U *)string_buf+v8_1,16);
						else if(CurPluWeightUnit == WEIGHT_UNIT_G)
							get_global_sparam(GLOBAL_WEIGHT_SYMBOL_2,(INT8U *)string_buf+v8_1,16);
						string_buf[v8_1+16] = 0;
#endif
					} else {
						continue;
					}
					break;
				case FIELD_ID_NEW_WEIGHT :
					if(label_set.en_mark_down_weight == 0) continue;
					if(PrtItemStr.fixed_price == 1 && !PrtItemStr.l_FixedWeight) continue;
					l_v=PrtItemStr.l_Price.Weight[PRICE_RESULT];
					if (Prt_weight_Symbol_flag == 0)
						wt_prt_type = 0;
					else if (Prt_weight_Symbol_flag == 1)
						wt_prt_type = 2;
					else if (Prt_weight_Symbol_flag == 2)
						wt_prt_type = 5;
					if(PrtItemStr.l_PluType <= PLU_BY_WEIGHT) {			// by-weight mode
						v8_1 = get_global_bparam(GLOBAL_PRINT_OPER_FLAG2);
						sprintf(string_buf, "");
						if(v8_1&0x04) {
							sprintf(string_buf, "N ");
						}
						v16_1 = strlen(string_buf);
						v8_1=get_decimal_pt(4, PrtItemStr.plu_addr);
						v8_2=dec_ch;
						if(l_v < 0) continue;
						PrtGenWeightStr(string_buf+v16_1, l_v, wt_prt_type, w_sign1, w_sign2, v8_1, v8_2);
					} else {
						continue;//sprintf(string_buf, "%ld", PrtItemStr.l_Price.Qty[PRICE_RESULT]);
					}
					break;
				case FIELD_ID_MARK_DN_WEIGHT :
					if((label_set.en_mark_down_weight==0) || useFSP ) continue;
					if(PrtItemStr.fixed_price == 1 && !PrtItemStr.l_FixedWeight) continue;
					l_v=PrtItemStr.l_Price.Weight[PRICE_ORIGIN];
					if (Prt_weight_Symbol_flag == 0)
						wt_prt_type = 0;
					else if (Prt_weight_Symbol_flag == 1)
						wt_prt_type = 2;
					else if (Prt_weight_Symbol_flag == 2)
						wt_prt_type = 5;
					if(PrtItemStr.l_PluType <= PLU_BY_WEIGHT) {			// by-weight mode
#ifndef USE_TOPMART_STRUCTURE
						if (!(Operation.specialPriceDisc && status_scale.Plu.special_price))
						{
							string_buf[0] = FONT_CON_CH;
							string_buf[1] = MarkDownLineNum;
						}
#endif
						v8_1 = get_global_bparam(GLOBAL_PRINT_OPER_FLAG2);
						sprintf(string_buf+2, "");
						if(v8_1&0x04) {
							sprintf(string_buf+2, "N ");
						}
						v16_1 = strlen(string_buf);
						v8_1=get_decimal_pt(4, PrtItemStr.plu_addr);
						v8_2=dec_ch;
						PrtGenWeightStr(string_buf+v16_1, l_v, wt_prt_type, w_sign1, w_sign2, v8_1, v8_2);	// 20060315
					} else {
						continue;
					}
					break;

				case FIELD_ID_QUANTITY : // Quantity
#ifdef USE_ARABIC_NUMBER
				case FIELD_ID_NEW_QUANTITY:
#endif //USE_ARABIC_NUMBER				
#ifdef USE_INVENTORY_LABEL
					if(InventoryUnitWeight)
					{
						sprintf(string_buf, "%ld", InventoryQuantity);
						InventoryUnitWeight = 0;
					}
					else
					{
						sprintf(string_buf, "%s", "- ");
					}
					if(!prtsel_get_table(FIELD_ID_PCS_SYM_STR)) break;
					if(label_set.flagSymInQty) 
					{
						if(PrtItemStr.cur_pcssym_no) 
						{
							addr =  DFLASH_BASE + FLASH_PCS_UNIT_AREA;
							addr =  addr + PCS_UNIT_STRUCT_SIZE * (INT32U)(PrtItemStr.cur_pcssym_no-1);	// code-1 (?)
							v16_1 = Flash_wread(addr);
							if(PrtItemStr.cur_pcssym_no == (INT8U)v16_1) {
							addr += 2;
							v8_1 = strlen(string_buf);
							Flash_sread(addr, (HUGEDATA INT8U *)string_buf+v8_1, PCS_UNIT_STRUCT_SIZE-2);
							}
						} 
						else 
						{
							v8_1 = strlen(string_buf);
							get_global_sparam(GLOBAL_PCS_SYMBOL,(INT8U *)string_buf+v8_1,16);
							string_buf[v8_1+16] = 0;
						}
					}
					break;
#else
					if((label_set.en_mark_down_qty==1) && (!useFSP)) continue;
					if(PrtItemStr.fixed_price == 1) continue;
					l_v=PrtItemStr.l_Price.Qty[PRICE_RESULT];
					if(PrtItemStr.l_PluType <= PLU_BY_WEIGHT) {			// by-weight mode
						continue;
					} else {				// by-count mode
						sprintf(string_buf, "%ld", l_v);
						if(!prtsel_get_table(FIELD_ID_PCS_SYM_STR)) break;
						if(Prt_weight_Symbol_flag) {
							if(PrtItemStr.cur_pcssym_no) {
								addr =  DFLASH_BASE + FLASH_PCS_UNIT_AREA;
								addr =  addr + PCS_UNIT_STRUCT_SIZE * (INT32U)(PrtItemStr.cur_pcssym_no-1);	// code-1 (?)
								v16_1 = Flash_wread(addr);
								if(PrtItemStr.cur_pcssym_no == (INT8U)v16_1) {
									if (Prt_weight_Symbol_flag == 1)
									{
										v8_1 = strlen(string_buf);
										sprintf(string_buf+v8_1, " ");
									}
									addr += 2;
									v8_1 = strlen(string_buf);
									Flash_sread(addr, (HUGEDATA INT8U *)string_buf+v8_1, PCS_UNIT_STRUCT_SIZE-2);
								}
							} else {
								if (Prt_weight_Symbol_flag == 1)
								{
									v8_1 = strlen(string_buf);
									sprintf(string_buf+v8_1, " ");
								}
								v8_1 = strlen(string_buf);
								get_global_sparam(GLOBAL_PCS_SYMBOL,(INT8U *)string_buf+v8_1,16);
								string_buf[v8_1+16] = 0;
							}
						}
						else {
							if(label_set.flagSymInQty) {
								if(PrtItemStr.cur_pcssym_no) {
									addr =  DFLASH_BASE + FLASH_PCS_UNIT_AREA;
									addr =  addr + PCS_UNIT_STRUCT_SIZE * (INT32U)(PrtItemStr.cur_pcssym_no-1);	// code-1 (?)
									v16_1 = Flash_wread(addr);
									if(PrtItemStr.cur_pcssym_no == (INT8U)v16_1) {
										addr += 2;
										v8_1 = strlen(string_buf);
										Flash_sread(addr, (HUGEDATA INT8U *)string_buf+v8_1, PCS_UNIT_STRUCT_SIZE-2);
									}
								} else {
									v8_1 = strlen(string_buf);
									get_global_sparam(GLOBAL_PCS_SYMBOL,(INT8U *)string_buf+v8_1,16);
									string_buf[v8_1+16] = 0;
								}
							}
						}
					}
					break;
#endif
#ifndef USE_ARABIC_NUMBER
				case FIELD_ID_NEW_QUANTITY : // Quantity
					if( label_set.en_mark_down_qty == 0) continue;
					if(PrtItemStr.fixed_price == 1) continue;
					l_v=PrtItemStr.l_Price.Qty[PRICE_RESULT];
					if(PrtItemStr.l_PluType <= PLU_BY_WEIGHT) {			// by-weight mode
						continue;
					} else {				// by-count mode
						sprintf(string_buf, "%ld", l_v);
						if(!prtsel_get_table(FIELD_ID_PCS_SYM_STR)) break;
						if(Prt_weight_Symbol_flag) {
							if(PrtItemStr.cur_pcssym_no) {
								addr =  DFLASH_BASE + FLASH_PCS_UNIT_AREA;
								addr =  addr + PCS_UNIT_STRUCT_SIZE * (INT32U)(PrtItemStr.cur_pcssym_no-1);	// code-1 (?)
								v16_1 = Flash_wread(addr);
								if(PrtItemStr.cur_pcssym_no == (INT8U)v16_1) {
									if (Prt_weight_Symbol_flag == 1)
									{
										v8_1 = strlen(string_buf);
										sprintf(string_buf+v8_1, " ");
									}
									addr += 2;
									v8_1 = strlen(string_buf);
									Flash_sread(addr, (HUGEDATA INT8U *)string_buf+v8_1, PCS_UNIT_STRUCT_SIZE-2);
								}
							} else {
								if (Prt_weight_Symbol_flag == 1)
								{
									v8_1 = strlen(string_buf);
									sprintf(string_buf+v8_1, " ");
								}
								v8_1 = strlen(string_buf);
								get_global_sparam(GLOBAL_PCS_SYMBOL,(INT8U *)string_buf+v8_1,16);
								string_buf[v8_1+16] = 0;
							}
						}
						else {
							if(label_set.flagSymInQty) {
								if(PrtItemStr.cur_pcssym_no) {
									addr =  DFLASH_BASE + FLASH_PCS_UNIT_AREA;
									addr =  addr + PCS_UNIT_STRUCT_SIZE * (INT32U)(PrtItemStr.cur_pcssym_no-1);	// code-1 (?)
									v16_1 = Flash_wread(addr);
									if(PrtItemStr.cur_pcssym_no == (INT8U)v16_1) {
										addr += 2;
										v8_1 = strlen(string_buf);
										Flash_sread(addr, (HUGEDATA INT8U *)string_buf+v8_1, PCS_UNIT_STRUCT_SIZE-2);
									}
								} else {
									v8_1 = strlen(string_buf);
									get_global_sparam(GLOBAL_PCS_SYMBOL,(INT8U *)string_buf+v8_1,16);
									string_buf[v8_1+16] = 0;
								}
							}
						}
					}
					break;
#endif //USE_ARABIC_NUMBER					
				case FIELD_ID_MARK_DN_QUANTITY : // Quantity
					if((label_set.en_mark_down_qty==0) || useFSP) continue;
					if(PrtItemStr.fixed_price == 1) continue;
					l_v=PrtItemStr.l_Price.Qty[PRICE_ORIGIN];
					if(PrtItemStr.l_PluType <= PLU_BY_WEIGHT) {			// by-weight mode
						continue;
					} else {				// by-count mode
						v16_1 = 0;
#ifndef USE_TOPMART_STRUCTURE
						if (!(Operation.specialPriceDisc && status_scale.Plu.special_price))
						{
							string_buf[v16_1++] = FONT_CON_CH;
							string_buf[v16_1++] = MarkDownLineNum;
						}
#endif
						sprintf(string_buf+v16_1, "%ld", l_v);
						if(!prtsel_get_table(FIELD_ID_PCS_SYM_STR)) break;
						if(Prt_weight_Symbol_flag) {
							if(PrtItemStr.cur_pcssym_no) {
								addr =  DFLASH_BASE + FLASH_PCS_UNIT_AREA;
								addr =  addr + PCS_UNIT_STRUCT_SIZE * (INT32U)(PrtItemStr.cur_pcssym_no-1);	// code-1 (?)
								v16_1 = Flash_wread(addr);
								if(PrtItemStr.cur_pcssym_no == (INT8U)v16_1) {
									if (Prt_weight_Symbol_flag == 1)
									{
										v8_1 = strlen(string_buf);
										sprintf(string_buf+v8_1, " ");
									}
									addr += 2;
									v8_1 = strlen(string_buf);
									Flash_sread(addr, (HUGEDATA INT8U *)string_buf+v8_1, PCS_UNIT_STRUCT_SIZE-2);
								}
							} else {
								if (Prt_weight_Symbol_flag == 1)
								{
									v8_1 = strlen(string_buf);
									sprintf(string_buf+v8_1, " ");
								}
								v8_1 = strlen(string_buf);
								get_global_sparam(GLOBAL_PCS_SYMBOL,(INT8U *)string_buf+v8_1,16);
								string_buf[v8_1+16] = 0;
							}
						}
						else {
							if(label_set.flagSymInQty) {
								if(PrtItemStr.cur_pcssym_no) {
									addr =  DFLASH_BASE + FLASH_PCS_UNIT_AREA;
									addr =  addr + PCS_UNIT_STRUCT_SIZE * (INT32U)(PrtItemStr.cur_pcssym_no-1);	// code-1 (?)
									v16_1 = Flash_wread(addr);
									if(PrtItemStr.cur_pcssym_no == (INT8U)v16_1) {
										addr += 2;
										v8_1 = strlen(string_buf);
										Flash_sread(addr, (HUGEDATA INT8U *)string_buf+v8_1, PCS_UNIT_STRUCT_SIZE-2);
									}
								} else {
									v8_1 = strlen(string_buf);
									get_global_sparam(GLOBAL_PCS_SYMBOL,(INT8U *)string_buf+v8_1,16);
									string_buf[v8_1+16] = 0;
								}
							}
						}
					}
					break;

				case FIELD_ID_TOTALPRICE : // total price
#ifdef USE_ARABIC_NUMBER
				case FIELD_ID_2ND_TOTALPRICE:
#endif //USE_ARABIC_NUMBER
					if((label_set.en_mark_down_total==1) && (!useFSP)) continue;
#ifdef USE_SETPRODUCT_SALE
					if(label_set.flagZeroUnitPrice && !setproduct_PrtFlag) continue;
					if(SetproductSaleMode && !setproduct_PrtFlag) continue;
					if(nonweighing_mode) continue;
#else
					if(label_set.flagZeroUnitPrice) continue;
#endif
					l_v = PrtItemStr.l_Price.TotalPrice[PRICE_RESULT];	//Modified by JJANG 20071228
					v8_1=price_dec;
					v8_2=dec_ch;
					v16_1 = 0;
#ifdef USE_DISCOUNT_MEMBERSHIP
					// 할인 라벨 인쇄할 경우에만 총액에 마크다운 표시 , 온라인 라벨은 표시 X
					if (PrtItemStr.disc_type && check_disc_print && !OnlineFlag ) 
					{
						string_buf[v16_1++] = FONT_CON_CH;
						string_buf[v16_1++] = MarkDownLineNum;
					}
#endif
					PrtGenSystemCurPriceStr(OUTMODE_LABEL, (HUGEDATA INT8U *)string_buf + v16_1, l_v, v8_1, v8_2);
					break;					
				case FIELD_ID_NEW_TOTALPRICE :	// markdown new total price
#ifdef USE_SPAR_MEMBER_PRICE
                    plu_get_field((INT16U)PrtItemStr.plu_addr-1, PTYPE_MEMBER_PRICE, (INT8U *)&l_v, (INT16S *)&v16_1,sizeof(l_v));
                    if(l_v)
                    {
                        if(label_set.en_mark_down_unit == 1) continue;
						l_v=MemberPrice.TotalPrice[PRICE_RESULT];
						v8_1=price_dec;
						v8_2=dec_ch;
						PrtGenSystemCurPriceStr(OUTMODE_LABEL, ( INT8U *)string_buf, l_v, v8_1, v8_2);
                    }                    
#else
					if(use_memberprice_flag)
					{				
						if(label_set.en_mark_down_unit == 1) continue;
						l_v=MemberPrice.TotalPrice[PRICE_RESULT];
						v8_1=price_dec;
						v8_2=dec_ch;
						PrtGenSystemCurPriceStr(OUTMODE_LABEL, ( INT8U *)string_buf, l_v, v8_1, v8_2);
					}
#endif
					else
					{
					if(label_set.en_mark_down_total == 0) continue;
					if(label_set.flagZeroUnitPrice) continue;
					if(GlbOper.UseFSP && DiscSale.saleDiscFSP) l_v = PriceFSP.TotalPrice[PRICE_RESULT]; //SG061214
					else l_v = PrtItemStr.l_Price.TotalPrice[PRICE_RESULT];	//Modified by JJANG 20071228 
					v8_1=price_dec;
					v8_2=dec_ch;
					PrtGenSystemCurPriceStr(OUTMODE_LABEL, (HUGEDATA INT8U *)string_buf, l_v, v8_1, v8_2);
					}
					break;
				case FIELD_ID_MARK_DN_TOTAL:
					if((label_set.en_mark_down_total==0) || useFSP) continue;
					l_v=PrtItemStr.l_Price.TotalPrice[PRICE_ORIGIN];
					v8_1=price_dec;
					v8_2=dec_ch;
					v16_1 = 0;
#ifndef USE_TOPMART_STRUCTURE
					if (!(Operation.specialPriceDisc && status_scale.Plu.special_price))
					{
						string_buf[v16_1++] = FONT_CON_CH;
						string_buf[v16_1++] = MarkDownLineNum;
					}
#endif
					PrtGenSystemCurPriceStr(OUTMODE_LABEL, (HUGEDATA INT8U *)string_buf+v16_1, l_v, v8_1, v8_2);
					break;
				case FIELD_ID_TOTAL_PRICE_IN_TAX : // Total Price with Tax (Include Tax)
					if((label_set.en_mark_down_total==1) && (!useFSP)) continue;
					if(label_set.flagZeroUnitPrice) continue;
					l_v = PrtItemStr.l_Price.TotalPrice[PRICE_RESULT];

					// * ExTax    : 0 - Internal Tax Type, 1 - External Tax Type
					// * TaxPrice : Internal/External 구분 없는 Tax Price
					// * Total Price with Tax :
					//   - Internal Tax Type = TotalPrice[PRICE_RESULT]
					//   - External Tax Type = TotalPrice[PRICE_RESULT] + TaxPrice

					if(PrtItemStr.l_Price.ExTax) // External Tax Type
						l_v = l_v + PrtItemStr.l_Price.TaxPrice;
					v8_1=price_dec;
					v8_2=dec_ch;
					PrtGenSystemCurPriceStr(OUTMODE_LABEL, (HUGEDATA INT8U *)string_buf, l_v, v8_1, v8_2);
					break;
				case FIELD_ID_TOTAL_PRICE_EX_TAX : // Total Price without Tax (Exclude Tax)
					if(label_set.en_mark_down_total==1) continue;
					l_v = PrtItemStr.l_Price.TotalPrice[PRICE_RESULT];

					// * ExTax    : 0 - Internal Tax Type, 1 - External Tax Type
					// * TaxPrice : Internal/External 구분 없는 Tax Price
					// * Total Price without Tax : 
					//   - Internal Tax Type = TotalPrice[PRICE_RESULT] - TaxPrice
					//   - External Tax Type = TotalPrice[PRICE_RESULT]
					
					if(!PrtItemStr.l_Price.ExTax) // Internal Tax Type
						l_v = l_v - PrtItemStr.l_Price.TaxPrice;
					v8_1=price_dec;
					v8_2=dec_ch;
					PrtGenSystemCurPriceStr(OUTMODE_LABEL, (HUGEDATA INT8U *)string_buf, l_v, v8_1, v8_2);
					break;
				case FIELD_ID_SAVED_PRICE :
					if(PrtItemStr.l_Price.TotalPrice[PRICE_SAVED] == 0) continue;
					l_v=PrtItemStr.l_Price.TotalPrice[PRICE_SAVED];
					v8_1=price_dec;
					v8_2=dec_ch;
					format_value((HUGEDATA INT8U *)string_buf,l_v,v8_1,v8_2);
					break;
#ifndef USE_ARABIC_NUMBER
				case FIELD_ID_2ND_TOTALPRICE :
					if(label_set.flagZeroUnitPrice) continue;
					if(!PrtItemStr.CPrice.CurrencyRate) continue;
					v8_2=dec_ch;
					PrtGenCurPriceStr_2long(string_buf, PrtItemStr.CPrice.CurrencyPriceH8, PrtItemStr.CPrice.CurrencyPriceL8, PrtItemStr.CPrice.CurrencyPrtForm, 
									PrtItemStr.CPrice.CurrencyString1, PrtItemStr.CPrice.CurrencyString2, 
									PrtItemStr.CPrice.CurrencyPriceDiv, v8_2);
					break;
#endif //USE_ARABIC_NUMBER
#ifndef USE_ARABIC_NUMBER
				case FIELD_ID_2ND_UNIT_PRICE :
					if(PrtItemStr.fixed_price == 1) continue;
					if(label_set.flagZeroUnitPrice) continue;
					if(!PrtItemStr.CPrice.CurrencyRate) continue;
					v8_2 = dec_ch;
					PrtGenCurPriceStr_2long(string_buf, PrtItemStr.CPrice.CurrencyUnitPriceH8, PrtItemStr.CPrice.CurrencyUnitPriceL8, PrtItemStr.CPrice.CurrencyPrtForm, 
									PrtItemStr.CPrice.CurrencyString1, PrtItemStr.CPrice.CurrencyString2, 
									PrtItemStr.CPrice.CurrencyPriceDiv, v8_2);

					if(!prtsel_get_table(FIELD_ID_UNIT_PCS)) break;
					if(label_set.flagPcsInUnit && PrtItemStr.l_PluType == PLU_BY_PCS) {			// by-count mode
						v8_1 = strlen(string_buf);
						sprintf(string_buf+v8_1,"/%d", PrtItemStr.l_Price.UnitCount[PRICE_RESULT]);
					}
					break;
#endif //USE_ARABIC_NUMBER					
				case FIELD_ID_CURRENCY_RATE :
					if(!PrtItemStr.CPrice.CurrencyRate) continue;
#ifdef USE_EXTEND_PRICE_PRIMARYSIGN
					get_global_sparam(GLOBAL_AREA_PRICE_PRIMARYSIGN,(INT8U *)currency_sign1,5);
					currency_sign1[5] = 0;
#else
					get_global_sparam(GLOBAL_AREA_PRICE_PRIMARYSIGN,(INT8U *)currency_sign1,4);
					currency_sign1[4] = 0;
#endif
					v8_2  = dec_ch;
					v8_1 = PrtGenCurPriceStr(string_buf, 1, 2, currency_sign1, currency_sign1, 0, v8_2);
					string_buf[v8_1++] = ' ';
					string_buf[v8_1++] = '=';
					string_buf[v8_1++] = ' ';
					PrtGenCurPriceStr(string_buf+v8_1, PrtItemStr.CPrice.CurrencyRate, 2, 
									PrtItemStr.CPrice.CurrencyString1, PrtItemStr.CPrice.CurrencyString2, 
									PrtItemStr.CPrice.CurrencyRateDiv, v8_2);
					break;
#ifdef USE_PRT_CHNIA_DISCOUNT_RATE
				case FIELD_ID_DISCOUNT_RATE :
					if(status_scale.discountflag != 2) continue;
					v8_1 = status_scale.discount_rate;
					v8_2 = v8_1%10;
					v8_1 = v8_1/10;
					if(v8_2 == 0)
					{
						sprintf(string_buf, "%d", v8_1);
						string_buf[2] = 0;
					}
					else
					{
						sprintf(string_buf, "%d%s%d", v8_1, ".", v8_2);
						string_buf[4] = 0;
					}
					break;
#endif
				default :
					continue;
			}// switch(fieldid)
#ifdef USE_DISCOUNT_MEMBERSHIP
			if(DirectSetField) {
				LabCmd_SetTextField_Direct(fieldid,(char *)ingredientBuf,v16_2);
			}
			else
#endif
	 			LabCmd_SetTextField(fieldid,string_buf);
		}//if (StrFieldStruct[fieldid].ExistField && select) {
	}//for (fieldid=0; fieldid<FIELD_ID_N_SERVING_SIZE; fieldid++) 
}

void prtfield_generate_common(void)
{
	INT16S fieldid;
	INT8U select;
#ifdef USE_DISCOUNT_MEMBERSHIP
	INT8U disc_type, start_month, start_date, end_month, end_date, price_dec, dec_ch;
	INT16U startDate, endDate, cap_id, v16_2;
	long price_dsc_total, price_dsc1, price_dsc2, price;
    INT8U ingredientBuf[MAX_INGREDIENT_SIZE];
	char *tempStr;
#endif
	INT16U v16_1;
	INT32U addr;
	char string_buf[200];

	for (fieldid=0; fieldid<FIELD_ID_N_SERVING_SIZE; fieldid++) {
		select = prtsel_get_table(fieldid);
		if (StrFieldStruct[fieldid].ExistField && select) {
			switch (fieldid) {
				case FIELD_ID_SHOP_NAME_STR :	// Shop name
#ifdef USE_DISCOUNT_MEMBERSHIP // 할인전 가격 캡션으로 사용 
					cap_id = 0;
					switch(PrtItemStr.disc_type)
					{
						case MEMBERSHIP_DISC :	// 엘포인트
							cap_id = CAP_BEFORE_MEMBERS_DISC;
							break;
						case CARD_DISC :
							cap_id = CAP_BEFORE_MEBERS_AND_CARD_DISC; // 판매가
							break;
						case MEMBERSHIP_CARD_DISC :
							cap_id = CAP_BEFORE_CARD_DISC; // 비회원가
							break;
						case NO_DSIC :
						default :
							break;
					}
					if (cap_id)
					{	
						// read caption
						addr =	ing_calc_addr_by_key(cap_id - 1);
						v16_2 = Flash_wread(addr);
						
						if(cap_id != v16_2) continue;
						addr += 2;

						Flash_sread(addr, ingredientBuf, MAX_INGREDIENT_SIZE);
						ingredientBuf[MAX_INGREDIENT_SIZE - 1] = 0;
						
						for (v16_2 = 0; v16_2 < INGREDIENT_STRUCT_SIZE - 2; v16_2++)
						{
							if (ingredientBuf[v16_2] == 0x0c || ingredientBuf[v16_2] == 0x00) break;
						}
						
						Flash_sread(addr, ( INT8U *)string_buf, v16_2);
						string_buf[v16_2] = 0;
					}
					else
					{
						continue;
					}
#else
					if(PrtComStr.shop_name_no) {
						if(!ExistShopName(PrtComStr.shop_name_no, &addr)) continue;
						addr += 2;
						Flash_sread(addr, (HUGEDATA INT8U *)string_buf, 26);
						string_buf[26]=0;
					} else {
						continue;
					}
#endif
					break;
				case FIELD_ID_SHOP_TEL_STR :	// Shop TEL
#ifdef USE_DISCOUNT_MEMBERSHIP // 할인후 가격 캡션으로 사용 
					cap_id = 0;
					switch(PrtItemStr.disc_type)
					{
						case MEMBERSHIP_DISC : // 멤버십 할인 
							cap_id = CAP_AFTER_MEMBERS_DISC; // 회원가
							break;
						case CARD_DISC : // 카드 할인 
							cap_id = CAP_AFTER_CARD_DISC; // 카드가
							break;
						case MEMBERSHIP_CARD_DISC :// 멤버십 + 카드 할인
							cap_id = CAP_AFTER_MEBERS_AND_CARD_DISC; // 최종 혜택가
							break;
						case NO_DSIC :
						default :
							break;
					}
					if (cap_id)
					{
						// read caption
						addr =	ing_calc_addr_by_key(cap_id - 1);
						v16_2 = Flash_wread(addr);
						
						if(cap_id != v16_2) continue;
						addr += 2;
						
						Flash_sread(addr, ingredientBuf, MAX_INGREDIENT_SIZE);
						ingredientBuf[MAX_INGREDIENT_SIZE - 1] = 0;
						
						for (v16_2 = 0; v16_2 < INGREDIENT_STRUCT_SIZE - 2; v16_2++)
						{
							if (ingredientBuf[v16_2] == 0x0c || ingredientBuf[v16_2] == 0x00) break;
						}						
						Flash_sread(addr, ( INT8U *)string_buf, v16_2);
						string_buf[v16_2] = 0;
					}
					else
					{
						continue;
					}
#else
					if(PrtComStr.shop_name_no) {
						if(!ExistShopName(PrtComStr.shop_name_no, &addr)) continue;
						addr += 28;
						Flash_sread(addr, (HUGEDATA INT8U *)string_buf, 20);
						string_buf[20]=0;
					} else {
						continue;
					}
#endif
					break;
				case FIELD_ID_SHOP_TEXT_STR :	// Shop Text
#ifdef USE_DISCOUNT_MEMBERSHIP // 행사 기간으로 사용		
					if (PrtItemStr.disc_type) // 0 : 행사 없음
					{
						plu_get_field(PrtItemStr.plu_addr-1, PTYPE_DISCOUNT_START_DATE, (INT8U *)&startDate, (INT16S *)&startDate, sizeof(startDate));
						plu_get_field(PrtItemStr.plu_addr-1, PTYPE_DISCOUNT_END_DATE, (INT8U *)&endDate, (INT16S *)&endDate, sizeof(endDate));

						if(startDate && endDate)
						{
							start_month = startDate / 100;
							start_date = startDate % 100;
							end_month = endDate / 100;
							end_date = endDate % 100;

							sprintf(string_buf,"(%02d/%02d-%02d/%02d)", start_month, start_date, end_month, end_date);
						}
					}
					else
					{
						continue;
					}
#else
					if(PrtComStr.shop_name_no) {
						if(!ExistShopName(PrtComStr.shop_name_no, &addr)) continue;
						addr += 48;
#ifdef USE_RUSSIA_DEFAULT
						Flash_sread(addr, (HUGEDATA INT8U *)string_buf, 150); //20151001
						string_buf[150]=0;
#else
						Flash_sread(addr, (HUGEDATA INT8U *)string_buf, 80);
						string_buf[80]=0;
#endif
					} else {
						continue;
					}
#endif
					break;
				case FIELD_ID_CLERK_NAME:
					if(PrtComStr.clerk_no) {
						addr =  DFLASH_BASE + FLASH_CLERK_AREA;
						addr =  addr + CLERK_STRUCT_SIZE * (INT32U)PrtComStr.clerk_no;
						v16_1 = Flash_wread(addr);
						if(PrtComStr.clerk_no != v16_1) continue;
						addr += 2;
						Flash_sread(addr, (HUGEDATA INT8U *)string_buf, 32);
						string_buf[32]=0;
					} else {
						continue;
					}
					break;
				case FIELD_ID_CUSTOMER_NAME:
					if(PrtComStr.customer_no) {
						addr =  DFLASH_BASE + FLASH_CUSTOMER_AREA;
						addr =  addr + CUSTOMER_STRUCT_SIZE * (INT32U)PrtComStr.customer_no;
						v16_1 = Flash_wread(addr);
						if(PrtComStr.customer_no != v16_1) continue;
						addr += 12;
						Flash_sread(addr, (HUGEDATA INT8U *)string_buf, 32);
						string_buf[32]=0;
					} else {
						continue;
					}
					break;
				case FIELD_ID_CUSTOMER_NO:
					if(PrtComStr.customer_no) {
						addr =  DFLASH_BASE + FLASH_CUSTOMER_AREA;
						addr =  addr + CUSTOMER_STRUCT_SIZE * (INT32U)PrtComStr.customer_no;
						v16_1 = Flash_wread(addr);
						if(PrtComStr.customer_no != v16_1) continue;
						addr += 2;
						Flash_sread(addr, (HUGEDATA INT8U *)string_buf, 10);
						string_buf[10]=0;
					} else {
						continue;
					}
					break;
				case FIELD_ID_CLERK_NO :	// Clerk
					sprintf(string_buf,"%d", PrtComStr.clerk_no);
					break;

				case FIELD_ID_SCALE_NO :
#ifdef USE_DISCOUNT_MEMBERSHIP
					if (PrtItemStr.disc_type)
					{
						v16_1 = 0;
						v16_2 = 0;
						price = PrtItemStr.l_Price.TotalPrice[PRICE_RESULT];
						switch (PrtItemStr.disc_type)
						{
							case MEMBERSHIP_DISC : // 멤버십
								plu_get_field(PrtItemStr.plu_addr-1, PTYPE_DISCOUNT_RATE1, (INT8U *)&v16_1, (INT16S *)&v16_1, sizeof(v16_1));
								price_dsc1 = Prt_Calc_Discount_Price(price, v16_1);
								price -= price_dsc1; //회원가
								break;
							case CARD_DISC :	// 카드할인
								plu_get_field(PrtItemStr.plu_addr-1, PTYPE_DISCOUNT_RATE2, (INT8U *)&v16_1, (INT16S *)&v16_1, sizeof(v16_1));
								price_dsc1 = Prt_Calc_Discount_Price(price, v16_1);
								price -= price_dsc1; // 카드가
								break;
							case  MEMBERSHIP_CARD_DISC : // 멤버십 + 카드할인
								// 멤버십 할인율
								plu_get_field(PrtItemStr.plu_addr-1, PTYPE_DISCOUNT_RATE1, (INT8U *)&v16_1, (INT16S *)&v16_1, sizeof(v16_1));
								// 카드 할인율 
								plu_get_field(PrtItemStr.plu_addr-1, PTYPE_DISCOUNT_RATE2, (INT8U *)&v16_2, (INT16S *)&v16_2, sizeof(v16_2));

								price_dsc1 = Prt_Calc_Discount_Price(price, v16_1); // 멤버십 할인 가격
								price_dsc2 = Prt_Calc_Discount_Price(price - price_dsc1, v16_2); // 카드 할인 가격
								price_dsc_total = price_dsc1 + price_dsc2; // 최종할인가격
								price -= price_dsc_total; // 최종 혜댁가
								break;
							default :
								break;
						}

						price_dec = get_global_bparam(GLOBAL_AREA_PRICE_DECIMALPOINT);
						dec_ch = get_global_bparam(GLOBAL_DECIMAL_SIGN);
						PrtGenSystemCurPriceStr(OUTMODE_LABEL, ( INT8U *)string_buf, price, price_dec, dec_ch);		
					}
#else
					v16_1 = get_net_wparam(NETWORK_SCALE_ID);
					sprintf(string_buf,"%d", v16_1);
#endif
					break;
				default :
					continue;
			}
			LabCmd_SetTextField(fieldid,string_buf);
			string_buf[0] = 0;
		}
	}
}

#ifdef USE_TRACEABILITY
void prtfield_generate_traceability(void)
{
	INT16S fieldid;
	INT16U v16_1;
	INT32U addr;
	char string_buf[128];

	if(!PrtItemStr.trace_no) return;
	if(!prtsel_get_table(FIELD_ID_TRACEABILITY_CODE)) return;

	for (fieldid=FIELD_ID_TRACEABILITY_CODE; fieldid<=FIELD_ID_CUTTING_CODE; fieldid++) {
		if (StrFieldStruct[fieldid].ExistField) {
			switch (fieldid) {
				case FIELD_ID_TRACEABILITY_CODE:
					switch(PrtItemStr.l_PluNo) {
						case MISC_PLU_WEIGHT :
						case MISC_PLU_PCS    :
						case MISC_PLU_COUNT  :
						case 0:
							v16_1 = 0;
							break;
						default :
							v16_1 = PrtItemStr.trace_no;
							break;
					}
					sprintf(string_buf,"%d",v16_1);
					break;
				case FIELD_ID_BORN_COUNTRY :
					if(PrtItemStr.born_country_no) {
						if(!get_country_name(PrtItemStr.born_country_no, string_buf)) continue;
					} else {
						continue;
					}
					break;
				case FIELD_ID_BRED_COUNTRY :
					if(PrtItemStr.bred_country_no) {
						if(!get_country_name(PrtItemStr.bred_country_no, string_buf)) continue;
					} else {
						continue;
					}
					break;
				case FIELD_ID_SLAUGHTER :
					if(PrtItemStr.slaught_no) {
						addr = DFLASH_BASE + FLASH_SLAUGHTERHOUSE_AREA;
						addr = addr + SLAUGHTERHOUSE_STRUCT_SIZE * (INT32U)(PrtItemStr.slaught_no - 1);
						v16_1 = Flash_wread(addr);
						if(v16_1 != PrtItemStr.slaught_no) continue;
						addr += 2;
						Flash_sread(addr, (HUGEDATA INT8U *)string_buf, SLAUGHTERHOUSE_STRUCT_SIZE-4);
					} else {
						continue;
					}
					break;
				case FIELD_ID_SLAUGHTER_CONTRY :
					if(PrtItemStr.slaught_no) {
						addr = DFLASH_BASE + FLASH_SLAUGHTERHOUSE_AREA;
						addr = addr + SLAUGHTERHOUSE_STRUCT_SIZE * (INT32U)(PrtItemStr.slaught_no - 1);// + 30;
						v16_1 = Flash_wread(addr);
						if(v16_1 != PrtItemStr.slaught_no) continue;
						addr += SLAUGHTERHOUSE_STRUCT_SIZE-2;
						v16_1 = Flash_wread(addr);
						if(!v16_1) continue;
						if(!get_country_name(v16_1, string_buf)) continue;
					} else {
						continue;
					}
					break;
				case FIELD_ID_CUTTING :
					if(PrtItemStr.cutting_no) {
						addr = DFLASH_BASE + FLASH_CUTTINGHALL_AREA;
						addr = addr + CUTTINGHALL_STRUCT_SIZE * (INT32U)(PrtItemStr.cutting_no - 1);// + 2;
						v16_1 = Flash_wread(addr);
						if(v16_1 != PrtItemStr.cutting_no) continue;
						addr += 2;
						Flash_sread(addr, (HUGEDATA INT8U *)string_buf, CUTTINGHALL_STRUCT_SIZE-4);
					} else {
						continue;
					}
					break;
				case FIELD_ID_CUTTING_COUNTRY :
					if(PrtItemStr.cutting_no) {
						addr = DFLASH_BASE + FLASH_CUTTINGHALL_AREA;
						addr = addr + CUTTINGHALL_STRUCT_SIZE * (INT32U)(PrtItemStr.cutting_no - 1);// + 30;
						v16_1 = Flash_wread(addr);
						if(v16_1 != PrtItemStr.cutting_no) continue;
						addr += SLAUGHTERHOUSE_STRUCT_SIZE-2;
						v16_1 = Flash_wread(addr);
						if(!v16_1) continue;
						if(!get_country_name(v16_1, string_buf)) continue;
					} else {
						continue;
					}
					break;
				case FIELD_ID_TRACE_REFERENCE :
					if(PrtItemStr.trace_no) {
						addr = DFLASH_BASE + FLASH_TRACEABILITY_AREA;
						addr = addr + TRACEABILITY_STRUCT_SIZE * (INT32U)(PrtItemStr.trace_no - 1) + 2;
						Flash_sread(addr, (HUGEDATA INT8U *)string_buf, TRACEABILITY_STRUCT_SIZE-10);
					} else {
						continue;
					}
					break;
				case FIELD_ID_BORN_CODE :
					sprintf(string_buf,"%d", PrtItemStr.born_country_no);
					break;
				case FIELD_ID_BRED_CODE :
					sprintf(string_buf,"%d", PrtItemStr.bred_country_no);
					break;
				case FIELD_ID_SLAUGHTER_CODE :
					sprintf(string_buf,"%d", PrtItemStr.slaught_no);
					break;
				case FIELD_ID_CUTTING_CODE :
					sprintf(string_buf,"%d", PrtItemStr.cutting_no);
					break;
				default :
					continue;
			}
			LabCmd_SetTextField(fieldid,string_buf);
		}
	}
}
#endif

#ifdef USE_NUTRITION_FACT
void getNutriStringData(char *strBuf, NUTRIFACT_STRUCT2 *nutStr, INT8U index, INT8U nutri_type)
{
	INT16U baseSize, servingSize;
	INT32U value, temp;

	INT8U actual_weight_flag;
	INT8U nutri_buf1[20];
	INT8U nutri_buf2[20];

	INT8U weight_decimal_sym;
	INT8U decimal_point;
	INT8U unit_sym[4][4] = {"", "g", "mg", "%"};
	INT8U nutri_unit[MAX_NUTRIFACT_ITEM] = {
				0	,	//		0	Serving Size(dummy)
				0	,	//		1	Serving Per(dummy)
				0	,	//		2	dummy
				0	,	//		3	Calories 
				0	,	//		4	Calories From Fat 
				1	,	//	g	5	Total Fat
				1	,	//	g	6	Saturated Fat
				1	,	//	g	7	Trans Fat
				2	,	//	mg	8	Cholesterol
				2	,	//	mg	9	Sodium
				1	,	//	g	10	Total Carbohydrates 
				1	,	//	g	11	Dietary Fibers
				1	,	//	g	12	Sugars
				1	,	//	g	13	Protein
				3	,	//	%	14	Vitamin A
				3	,	//	%	15	Calcium
				3	,	//	%	16	Vitamin C
				3	,	//	%	17	Iron
				1	,	//	g	18	salt
				0	,	//	KJ	19	Energy
	};
	INT8U unitSym[4];
	INT8U energySym[4] = {"kJ"};

	decimal_point = ReturnDecimalPointNutriTable(nutri_type, index);

	weight_decimal_sym   = get_global_bparam(GLOBAL_DECIMAL_SIGN);

	memcpy(unitSym, unit_sym[nutri_unit[index]], sizeof(unitSym));

	if((nutri_type == NUTRI_TYPE_2COL_6DIG_AUS || nutri_type == NUTRI_TYPE_2COL_NZL\
	|| nutri_type == NUTRI_TYPE_2COL_6DIG_AUS_ACTUAL_WEIGHT || nutri_type == NUTRI_TYPE_2COL_NZL_ACTUAL_WEIGHT)\
	&& index == NUTRI_FACT_CALORIES) memcpy(unitSym, energySym, sizeof(unitSym));

	// Print Actual Weight for serving(NUTRI_TYPE_2COL_6DIG_AUS_ACTUAL_WEIGHT,NUTRI_TYPE_2COL_NZL_ACTUAL_WEIGHT)
	actual_weight_flag = 0;
	if(nutri_type == NUTRI_TYPE_2COL_6DIG_AUS_ACTUAL_WEIGHT || nutri_type == NUTRI_TYPE_2COL_NZL_ACTUAL_WEIGHT)
	{
		actual_weight_flag = 1;
	}

	// !1Column : serving size에 나눗값 표시 없음
	if (nutri_type != NUTRI_TYPE_2COL_CHL && nutri_type != NUTRI_TYPE_2COL_6DIG_AUS && nutri_type != NUTRI_TYPE_2COL_NZL\
	 && nutri_type != NUTRI_TYPE_2COL_6DIG_AUS_ACTUAL_WEIGHT && nutri_type != NUTRI_TYPE_2COL_NZL_ACTUAL_WEIGHT)
	{
		// 1 column 인 경우 아래 함수 실행 후 return
		format_value((HUGEDATA INT8U *)nutri_buf1,nutStr->nutri_data[index],decimal_point,weight_decimal_sym);
		if(nutri_type == NUTRI_TYPE_TOTALFAT_CAN)
		{
			sprintf(strBuf, "%s %s", nutri_buf1, unitSym);	//canada는 %, 단위 앞에 공백 하나 추가 요청
			return;
		}
		// default : 0
		sprintf(strBuf, "%s%s", nutri_buf1, unitSym);
		return;
	} 
	// 2 column 인 경우 아래 함수들 실행 후 함수 종료
	
	baseSize = nutStr->nutri_data[NUTRI_FACT_BASE_SIZE];
	if(actual_weight_flag && (PrtItemStr.l_PluType == PLU_BY_WEIGHT) && (nutStr->nutri_data[NUTRI_FACT_BASE_UNIT]==0))
	{
		servingSize = (INT16U)PrtItemStr.l_Price.Weight[PRICE_RESULT];
	}
	else
	{
		servingSize = nutStr->nutri_data[NUTRI_FACT_SERVING_SIZE]; 
	}
	if(baseSize==0) return;

	value = nutStr->nutri_data[index]*servingSize;
	temp = value / baseSize;

	format_value((HUGEDATA INT8U *)nutri_buf1,nutStr->nutri_data[index],decimal_point,weight_decimal_sym);	
	format_value((HUGEDATA INT8U *)nutri_buf2,temp,decimal_point,weight_decimal_sym);

	strcat((char *)nutri_buf1, (char *)unitSym);
	strcat((char *)nutri_buf2, (char *)unitSym);
	sprintf(strBuf, "%10s %10s",nutri_buf1, nutri_buf2);
}
#endif

#ifdef USE_NUTRITION_FACT
#ifdef USE_EU_NUTRITIONFACT
void getEuNutriStringData(char *strBuf, NUTRIFACT_STRUCT3 *nutStr, INT8U nutri_type)
{
	INT8U i;
	INT32U base_size, serving_size;
	INT8U buf1[10];
	INT8U buf2[10];
	INT8U buf3[10];
	INT32U value1, value2;
	INT8U fat_detail_flag = 0;
	INT8U vitamin_flag = 0;
	INT8U line_number = 0;

	INT8U weight_decimal_sym;
	INT8U decimal_point;
	INT8U unit_sym[5][4] = {"", "g", "mg", "ug", "ml"};
	INT8U nutri_unit_eu[MAX_NUTRIFACT_ITEM_EU] = {
				0	,	//		0	dumm(BASE_UNIT)
				0	,	//		1	dummy(SERVING_SIZE)
				0	,	//		2	dummy(BASE_SIZE)
				0   ,   //      3   ENERGY
				1   ,   //      4   FAT
				1   ,   //      5   SATURATES
				1   ,   //      6   MONOUNSATURATES
				1   ,   //      7   POLYUNSATURATES
				1   ,   //      8   CARBOHYDRATE
				1   ,   //      9   SUGARS
				1   ,   //      10  POLYOLS
				1   ,   //      11  STARCH
				1   ,   //      12  FIBRE
				1   ,   //      13  PROTEIN
				1   ,   //      14  SALT
				3   ,   //      15  VITAMIN_A
				3   ,   //      16  VITAMIN_D
				2   ,   //      17  VITAMIN_E
				3   ,   //      18  VITAMIN_K
				2   ,   //      19  VITAMIN_C
				2   ,   //      20  THIAMIN
				2   ,   //      21  RIBOFLAVIN
				2   ,   //      22  NIACIN
				2   ,   //      23  VITAMIN_B6
				3   ,   //      24  FOLIC_ACID
				3   ,   //      25  VITAMIN_B12
				3   ,   //      26  BIOTIN
				2   ,   //      27  PANTHOTHENIC_ACID
				2   ,   //      28  POTASSIUM
				2   ,   //      29  CHLORIDE
				2   ,   //      30  CALCIUM
				2   ,   //      31  PHOSPHORUS
				2   ,   //      32  MAGNESIUM
				2   ,   //      33  IRON
				2   ,   //      34  ZINC
				2   ,   //      35  COPPER
				2   ,   //      36  MANGANESE
				2   ,   //      37  FLUORIDE
				3   ,   //      38  SELENIUM
				3   ,   //      39  CHROMIUM
				3   ,   //      40  MOLYBDENUM
				3   ,   //      41  IODINE
	};
	INT8U unit_sym_use[4];
	INT8U energySym[2][5] = {"kJ", "kcal"};
	
	weight_decimal_sym   = get_global_bparam(GLOBAL_DECIMAL_SIGN);
	serving_size = nutStr->nutri_data[1];
	base_size = nutStr->nutri_data[2];
	if (base_size == 0) base_size = 1;
	
	if (nutStr->nutri_data[0] == 0) memcpy(unit_sym_use, unit_sym[1], sizeof(unit_sym_use));
	else memcpy(unit_sym_use, unit_sym[4], sizeof(unit_sym_use));
	
	sprintf(strBuf, "%s\n",  eu_nutrition_print_nutrition_facts);
	line_number++;
	sprintf(&strBuf[strlen(strBuf)], "%s  %s\n", 
		eu_nutrition_print_serving_size, nutStr->serving_size);
	line_number++;
	sprintf(&strBuf[strlen(strBuf)], "%s  %s\n\n", 
		eu_nutrition_print_serving_per_container, nutStr->serving_per);
	line_number++;
	line_number++;
	
	sprintf(&strBuf[strlen(strBuf)], ".                    %-4s%4d%-2s %-12s  %-4s\n",
		eu_nutrition_print_per, base_size, unit_sym_use, eu_nutrition_print_per_portion, eu_nutrition_print_percent_ri);
	line_number++;
	sprintf(&strBuf[strlen(strBuf)], ".                                 %4d%-2s\n", 
		serving_size, unit_sym_use);
	line_number++;
	sprintf(&strBuf[strlen(strBuf)], "\x97\x97\x97\x97\x97\x97\x97\x97\x97\x97\x97\x97\x97\x97\x97\x97\x97\x97\x97\x97\x97\x97\x97\x97\x97\x97\x97\x97\x97\x97\x97\x97\x97\x97\x97\x97\x97\x97\x97\x97\x97\x97\x97\x97\x97\x97\x97\x97\x97\x97\n");
	line_number++;
	
	value1 = MUL_DEV(nutStr->nutri_data[3], serving_size, base_size);
	value2 = MUL_DEV(value1, 1000, Nutri_DV_refer_table_EU[0]);
	format_value((HUGEDATA INT8U *)buf3,value2, 1, weight_decimal_sym);
	sprintf(&strBuf[strlen(strBuf)], "%-20s  %4d%-2s     %4d%-2s     %5s%%\n", 
		eu_nutrition_print_caption[0], nutStr->nutri_data[3], energySym[0], value1, energySym[0], buf3);
	line_number++;
	sprintf(&strBuf[strlen(strBuf)], ".                     /%4d%-4s  /%4d%-4s\n", 
		MUL_DEV(nutStr->nutri_data[3], 24, 100), energySym[1], MUL_DEV(value1, 24, 100), energySym[1]);
	line_number++;
		
	for (i = 4 ; i < MAX_NUTRIFACT_ITEM_EU ; i++ )
	{
		if (nutStr->nutri_data[i] || i == 4 || i == 14)
		{
			decimal_point = ReturnDecimalPointNutriTable(nutri_type, i);
			//memcpy(unit_sym_use, unit_sym[nutri_unit[index]], sizeof(unitSym[1]))
			format_value((HUGEDATA INT8U *)buf1, nutStr->nutri_data[i], decimal_point, weight_decimal_sym);
			value1 = MUL_DEV(nutStr->nutri_data[i], serving_size, base_size);
			format_value((HUGEDATA INT8U *)buf2, value1, decimal_point, weight_decimal_sym);
			if (i == 4 && nutStr->nutri_data[i] < 10)
			{
				sprintf(buf1, "< ");
				format_value((HUGEDATA INT8U *)buf1 + 2, 10, decimal_point, weight_decimal_sym);
			}
			if (i == 4 && value1 < 10)
			{
				sprintf(buf2, "< ");
				format_value((HUGEDATA INT8U *)buf2 + 2, 10, decimal_point, weight_decimal_sym);
			}
			if (i == 14 && nutStr->nutri_data[i] < 1)
			{
				sprintf(buf1, "< ");
				format_value((HUGEDATA INT8U *)buf1 + 2, 1, decimal_point, weight_decimal_sym);
			}
			if (i == 14 && value1 < 1)
			{
				sprintf(buf2, "< ");
				format_value((HUGEDATA INT8U *)buf2 + 2, 1, decimal_point, weight_decimal_sym);
			}
			if (Nutri_DV_refer_table_EU[i-3])
			{
				value2 = MUL_DEV(value1, 1000, Nutri_DV_refer_table_EU[i-3]);
				format_value((HUGEDATA INT8U *)buf3,value2, 1, weight_decimal_sym);
			}
				
			if (i >= NUTRI_FACT_EU_SATURATES && i <=NUTRI_FACT_EU_POLYUNSATURATES)
			{
				if(fat_detail_flag == 0)
				{
					sprintf(&strBuf[strlen(strBuf)], "%s\n", eu_nutrition_print_of_which);
					line_number++;
					fat_detail_flag = 1;
				}
				sprintf(&strBuf[strlen(strBuf)], ". %s\n", eu_nutrition_print_fat_prefix);
				line_number++;
				if (Nutri_DV_refer_table_EU[i-3])
				{
					sprintf(&strBuf[strlen(strBuf)], ". %-18s  %6s%-2s   %6s%-2s   %5s%%\n", 
						eu_nutrition_print_caption[i-3], buf1, unit_sym[nutri_unit_eu[i]], buf2, unit_sym[nutri_unit_eu[i]], buf3);
					line_number++;
				}
				else
				{
					sprintf(&strBuf[strlen(strBuf)], ". %-18s  %6s%-2s   %6s%-2s\n", 
						eu_nutrition_print_caption[i-3], buf1, unit_sym[nutri_unit_eu[i]], buf2, unit_sym[nutri_unit_eu[i]]);
					line_number++;
				}
			}
			else if (i >= NUTRI_FACT_EU_SUGARS && i <=NUTRI_FACT_EU_STARCH)
			{
				if (Nutri_DV_refer_table_EU[i-3])
				{
					sprintf(&strBuf[strlen(strBuf)], ". %-18s  %6s%-2s   %6s%-2s   %5s%%\n", 
						eu_nutrition_print_caption[i-3], buf1, unit_sym[nutri_unit_eu[i]], buf2, unit_sym[nutri_unit_eu[i]], buf3);
					line_number++;
				}
				else
				{
					sprintf(&strBuf[strlen(strBuf)], ". %-18s  %6s%-2s   %6s%-2s\n", 
						eu_nutrition_print_caption[i-3], buf1, unit_sym[nutri_unit_eu[i]], buf2, unit_sym[nutri_unit_eu[i]]);
					line_number++;
				}
			}
			else
			{
				if(i >= NUTRI_FACT_EU_VITAMIN_A && vitamin_flag == 0)
				{
					sprintf(&strBuf[strlen(strBuf)], "\x97\x97\x97\x97\x97\x97\x97\x97\x97\x97\x97\x97\x97\x97\x97\x97\x97\x97\x97\x97\x97\x97\x97\x97\x97\x97\x97\x97\x97\x97\x97\x97\x97\x97\x97\x97\x97\x97\x97\x97\x97\x97\x97\x97\x97\x97\x97\x97\x97\x97\n");
					line_number++;
					sprintf(&strBuf[strlen(strBuf)], "%s\n", 
						eu_nutrition_print_vitamins_and_minerals);
					line_number++;
					vitamin_flag = 1;
				}
				if (Nutri_DV_refer_table_EU[i-3])
				{
					sprintf(&strBuf[strlen(strBuf)], "%-20s  %6s%-2s   %6s%-2s   %5s%%\n", 
						eu_nutrition_print_caption[i-3], buf1, unit_sym[nutri_unit_eu[i]], buf2, unit_sym[nutri_unit_eu[i]], buf3);
					line_number++;
				}
				else
				{
					sprintf(&strBuf[strlen(strBuf)], "%-20s  %6s%-2s   %6s%-2s\n",
						eu_nutrition_print_caption[i-3], buf1, unit_sym[nutri_unit_eu[i]], buf2, unit_sym[nutri_unit_eu[i]]);
					line_number++;
				}
			}
		}
		if (line_number >= 60) break;
	}
}
#endif // USE_EU_NUTRITIONFACT
#ifdef USE_ADDITIONAL_NUTRIENT
void getAddNutriStringData(char *strBuf, NUTRIFACT_STRUCT4 *nutStr, INT8U index, INT8U nutri_type)
{
	INT16U baseSize, servingSize;
	INT32U value, temp;

	INT8U actual_weight_flag;
	INT8U nutri_buf[30];
	
	INT8U weight_decimal_sym;
	INT8U decimal_point;
	INT8U unit_sym[5][4] = {"", "g", "mg", "mcg", "%"};
	INT8U nutri_unit[MAX_NUTRIFACT_ITEM] = {
				0	,	//		0	Serving Size(dummy)
				0	,	//		1	Serving Per(dummy)
				0	,	//		2	dummy
				0	,	//		3	Calories 
				0	,	//		4	Calories From Fat 
				1	,	//	g	5	Total Fat
				1	,	//	g	6	Saturated Fat
				1	,	//	g	7	Trans Fat
				2	,	//	mg	8	Cholesterol
				2	,	//	mg	9	Sodium
				1	,	//	g	10	Total Carbohydrates 
				1	,	//	g	11	Dietary Fibers
				1	,	//	g	12	Sugars
				1	,	//	g	13	Protein
				3	,	//	mcg	14	Vitamin A
				2	,	//	mg	15	Calcium
				3	,	//	mcg	16	Vitamin C
				2	,	//	mg	17	Iron
				1	,	//	g	18	salt
				0	,	//	kJ	19	Energy
				1	,	//	g	20	Added Sugars
				3	,	//	mcg	21	Vitamin D
				2	,	//	mg	22	Potassium			
	};
	INT8U unitSym[4];
	INT8U energySym[4] = {"kJ"};

	decimal_point = ReturnDecimalPointNutriTable(nutri_type, index);

	weight_decimal_sym   = get_global_bparam(GLOBAL_DECIMAL_SIGN);

	memcpy(unitSym, unit_sym[nutri_unit[index]], sizeof(unitSym));

	// Print Actual Weight for serving(NUTRI_TYPE_2COL_6DIG_AUS_ACTUAL_WEIGHT,NUTRI_TYPE_2COL_NZL_ACTUAL_WEIGHT)
	actual_weight_flag = 0;
	
	// !1Column : serving size에 나눗값 표시 없음
	// 1 column 인 경우 아래 함수 실행 후 return
	format_value((INT8U *)nutri_buf,nutStr->nutri_data[index],decimal_point,weight_decimal_sym);
	// default : 0
	sprintf(strBuf, "%s%s", nutri_buf, unitSym);
	return; 
}
#endif // USE_ADDITIONAL_NUTRIENT
#endif // USE_NUTRITION_FACT

#ifdef USE_NUTRITION_FACT
#ifdef USE_ADDITIONAL_NUTRIENT
void prtfield_generate_nutri_Added(void)
{
	INT32U pnt;
	INT8U  ucTemp, nutri_type, index;
	INT8U  k;
	INT8U nutriItemDecimalPoint;

	INT8U nutri_2column_flag;
	INT8U actual_weight_flag;
	INT16U baseSize;
	int fieldid;
	INT32U v32_1;
	char string_buf[128];

	NUTRIFACT_STRUCT4  nutrifact_str4;
	INT8U baseUnit[2][4] = {"g","ml"};
	char space[2] = "";
	INT32U plu_no;
	INT16U dept_no;

  #ifdef USE_DIRECT_NUTRITION
	dept_no = PrtItemStr.l_DeptNo;
	plu_no = PrtItemStr.l_PluNo;
  #else
	if(PrtItemStr.nutri_no == 0) return;
  #endif
	if(!prtsel_get_table(FIELD_ID_N_SERVING_SIZE)) return;

	ucTemp = get_global_bparam(GLOBAL_SALE_SETUP_FLAG9);
	// nutri_type (0:normal, 1:CHL, 2:CAN, 3:AUS, 4:NZL, 5: AUS for actual weight, 6: NZL for actual weight)
	nutri_type = ucTemp&0x0f;
#ifdef COUNTRY_USA
	nutri_type = NUTRI_TYPE_2COL_USA;
#endif
  #ifdef USE_DIRECT_NUTRITION
	get_direct_nutrition(dept_no, plu_no, &nutrifact_str4);
  #else
	pnt = DFLASH_BASE + FLASH_NUTRIFACT_AREA;
	if (nutri_type == NUTRI_TYPE_2COL_USA || nutri_type == NUTRI_TYPE_TOTALFAT_CAN)
	{
		pnt += sizeof(NUTRIFACT_STRUCT4)*(INT32U)(PrtItemStr.nutri_no-1);
		Flash_sread(pnt, (INT8U *)&nutrifact_str4, sizeof(NUTRIFACT_STRUCT4));
		if(PrtItemStr.nutri_no != nutrifact_str4.code) return;
	}
  #endif
	// Print Actual Weight for serving(NUTRI_TYPE_2COL_6DIG_AUS_ACTUAL_WEIGHT,NUTRI_TYPE_2COL_NZL_ACTUAL_WEIGHT)
	actual_weight_flag = 0;
	nutri_2column_flag = 0;
	for (fieldid=FIELD_ID_N_SERVING_SIZE; fieldid < 226; fieldid++) {
		if (StrFieldStruct[fieldid].ExistField) 
		{
			if(nutri_type == NUTRI_TYPE_TOTALFAT_CAN)
			{
				sprintf(space, " ");
			}
			switch (fieldid) {
				case FIELD_ID_N_SERVING_SIZE:
					if(actual_weight_flag && (PrtItemStr.l_PluType == PLU_BY_WEIGHT) && (nutrifact_str4.nutri_data[NUTRI_FACT_BASE_UNIT]==0))
					{
						sprintf(string_buf, "%d%s", PrtItemStr.l_Price.Weight[PRICE_RESULT], baseUnit[index]);
					}
					else
					{
						sprintf(string_buf, (char *)nutrifact_str4.serving_size);
					}
					break;
				case FIELD_ID_N_SERVINGS_PER:
					if(actual_weight_flag && (PrtItemStr.l_PluType == PLU_BY_WEIGHT) && (nutrifact_str4.nutri_data[NUTRI_FACT_BASE_UNIT]==0))
					{
						sprintf(string_buf, "%d", 1);
					}
					else
					{
						sprintf(string_buf, (char *)nutrifact_str4.serving_per);
					}
					break;
				case FIELD_ID_N_BASE_SIZE:  //SG060109. Base Size field 추가 
					if(nutri_2column_flag)
					{
						index = 0; //0: "g"     1: "ml"
						if(nutrifact_str4.nutri_data[NUTRI_FACT_BASE_UNIT]==1)	index =1 ;
						sprintf(string_buf, "%d%s", nutrifact_str4.nutri_data[NUTRI_FACT_BASE_SIZE],baseUnit[index]);
					}
					else
						continue;
					break;
				case FIELD_ID_N_CALORIES:
					getAddNutriStringData(string_buf, &nutrifact_str4, NUTRI_FACT_CALORIES, nutri_type); 
					break;
				case FIELD_ID_N_CALORIES_FAT:
					getAddNutriStringData(string_buf, &nutrifact_str4, NUTRI_FACT_CALORIES_FAT, nutri_type); 
					break;
				case FIELD_ID_N_TOTAL_FAT:
					getAddNutriStringData(string_buf, &nutrifact_str4, NUTRI_FACT_TOTAL_FAT, nutri_type); 
					break;
				case FIELD_ID_N_SATURATED_FAT:
					getAddNutriStringData(string_buf, &nutrifact_str4, NUTRI_FACT_SATURATED_FAT, nutri_type); 
					break;
				case FIELD_ID_N_TRANS_FAT:
					getAddNutriStringData(string_buf, &nutrifact_str4, NUTRI_FACT_TRANS_FAT, nutri_type); 
					break;
				case FIELD_ID_N_CHOLESTEROL:
					getAddNutriStringData(string_buf, &nutrifact_str4, NUTRI_FACT_CHOLESTEROL, nutri_type); 
					break;			   
				case FIELD_ID_N_SODIUM:
					getAddNutriStringData(string_buf, &nutrifact_str4, NUTRI_FACT_SODIUM, nutri_type); 
					break;
				case FIELD_ID_N_TOTAL_CARBO:
					getAddNutriStringData(string_buf, &nutrifact_str4, NUTRI_FACT_TOTAL_CARBO, nutri_type); 
					break;
				case FIELD_ID_N_DIETARY_FIBERS:
					getAddNutriStringData(string_buf, &nutrifact_str4, NUTRI_FACT_DIETARY_FIBERS, nutri_type); 
					break;
				case FIELD_ID_N_SUGARS:
					getAddNutriStringData(string_buf, &nutrifact_str4, NUTRI_FACT_SUGARS, nutri_type); 
					break;
				case FIELD_ID_N_PROTEIN:
					getAddNutriStringData(string_buf, &nutrifact_str4, NUTRI_FACT_PROTEIN, nutri_type); 
					break;
				case FIELD_ID_N_VITAMIN_A:
					getAddNutriStringData(string_buf, &nutrifact_str4, NUTRI_FACT_VITAMIN_A, nutri_type); 
					break;
				case FIELD_ID_N_CALCIUM:
					getAddNutriStringData(string_buf, &nutrifact_str4, NUTRI_FACT_CALCIUM, nutri_type); 
					break;
				case FIELD_ID_N_VITAMIN_C:
					getAddNutriStringData(string_buf, &nutrifact_str4, NUTRI_FACT_VITAMIN_C, nutri_type); 
					break;
				case FIELD_ID_N_IRON:
					getAddNutriStringData(string_buf, &nutrifact_str4, NUTRI_FACT_IRON, nutri_type); 
					break;
				case FIELD_ID_N_SALT:
					getAddNutriStringData(string_buf, &nutrifact_str4, NUTRI_FACT_SALT, nutri_type);
					break;
				case FIELD_ID_N_ENERGY:
					getAddNutriStringData(string_buf, &nutrifact_str4, NUTRI_FACT_ENERGY, nutri_type);
					break;
				case FIELD_ID_N_ADDED_SUGARS:
					getAddNutriStringData(string_buf, &nutrifact_str4, NUTRI_FACT_ADDED_SUGARS, nutri_type);
					break;
				case FIELD_ID_N_VITAMIN_D:
					getAddNutriStringData(string_buf, &nutrifact_str4, NUTRI_FACT_VITAMIN_D, nutri_type);
					break;
				case FIELD_ID_N_POTASSIUM:
					getAddNutriStringData(string_buf, &nutrifact_str4, NUTRI_FACT_POTASSIUM, nutri_type);
					break;
//=============  Daily Value  ====================
//Daily value는 Nutri_DV_refer_table(일일권장량)과 비교하여 소수점 없이 round off 하여 percentage로 표기함.

				case FIELD_ID_N_TOTAL_FAT_DV:
					nutriItemDecimalPoint = ReturnDecimalPointNutriTable(nutri_type, NUTRI_FACT_TOTAL_FAT);
					v32_1 = ((INT32U)nutrifact_str4.nutri_data[NUTRI_FACT_TOTAL_FAT]*1000L/power10(nutriItemDecimalPoint)/Nutri_DV_refer_table_USA[0]+5)/10;
					sprintf(string_buf, "%ld%s%%", v32_1, space);
					break;
				case FIELD_ID_N_SATURATED_FAT_DV:
					nutriItemDecimalPoint = ReturnDecimalPointNutriTable(nutri_type, NUTRI_FACT_SATURATED_FAT);
					v32_1 = ((INT32U)nutrifact_str4.nutri_data[NUTRI_FACT_SATURATED_FAT]*1000L/power10(nutriItemDecimalPoint)/Nutri_DV_refer_table_USA[1]+5)/10;
					sprintf(string_buf, "%ld%s%%", v32_1, space);
					break;
				case FIELD_ID_N_CHOLESTEROL_DV:
					nutriItemDecimalPoint = ReturnDecimalPointNutriTable(nutri_type, NUTRI_FACT_CHOLESTEROL);
					v32_1 = ((INT32U)nutrifact_str4.nutri_data[NUTRI_FACT_CHOLESTEROL]*1000L/power10(nutriItemDecimalPoint)/Nutri_DV_refer_table_USA[2]+5)/10;
					sprintf(string_buf, "%ld%s%%", v32_1, space);
					break;
				case FIELD_ID_N_SODIUM_DV:
					nutriItemDecimalPoint = ReturnDecimalPointNutriTable(nutri_type, NUTRI_FACT_SODIUM);
					v32_1 = ((INT32U)nutrifact_str4.nutri_data[NUTRI_FACT_SODIUM]*1000L/power10(nutriItemDecimalPoint)/Nutri_DV_refer_table_USA[3]+5)/10;
					sprintf(string_buf, "%ld%s%%", v32_1, space);
					break;
				case FIELD_ID_N_TOTAL_CARBO_DV:
					nutriItemDecimalPoint = ReturnDecimalPointNutriTable(nutri_type, NUTRI_FACT_TOTAL_CARBO);					
					v32_1 = ((INT32U)nutrifact_str4.nutri_data[NUTRI_FACT_TOTAL_CARBO]*1000L/power10(nutriItemDecimalPoint)/Nutri_DV_refer_table_USA[4]+5)/10;
					sprintf(string_buf, "%ld%s%%", v32_1, space);
					break;
				case FIELD_ID_N_DIETARY_FIBERS_DV:
					nutriItemDecimalPoint = ReturnDecimalPointNutriTable(nutri_type, NUTRI_FACT_DIETARY_FIBERS);					
					v32_1 = ((INT32U)nutrifact_str4.nutri_data[NUTRI_FACT_DIETARY_FIBERS]*1000L/power10(nutriItemDecimalPoint)/Nutri_DV_refer_table_USA[5]+5)/10;
					sprintf(string_buf, "%ld%s%%", v32_1, space);
					break;
				case FIELD_ID_N_VITAMIN_D_DV:
					nutriItemDecimalPoint = ReturnDecimalPointNutriTable(nutri_type, NUTRI_FACT_VITAMIN_D);					
					v32_1 = ((INT32U)nutrifact_str4.nutri_data[NUTRI_FACT_VITAMIN_D]*1000L/power10(nutriItemDecimalPoint)/Nutri_DV_refer_table_USA[6]+5)/10;
					sprintf(string_buf, "%ld%s%%", v32_1, space);
					break;
				case FIELD_ID_N_CALCIUM_DV:
					nutriItemDecimalPoint = ReturnDecimalPointNutriTable(nutri_type, NUTRI_FACT_CALCIUM);					
					v32_1 = ((INT32U)nutrifact_str4.nutri_data[NUTRI_FACT_CALCIUM]*1000L/power10(nutriItemDecimalPoint)/Nutri_DV_refer_table_USA[7]+5)/10;
					sprintf(string_buf, "%ld%s%%", v32_1, space);
					break;
				case FIELD_ID_N_IRON_DV:
					nutriItemDecimalPoint = ReturnDecimalPointNutriTable(nutri_type, NUTRI_FACT_IRON);					
					v32_1 = ((INT32U)nutrifact_str4.nutri_data[NUTRI_FACT_IRON]*1000L/power10(nutriItemDecimalPoint)/Nutri_DV_refer_table_USA[8]+5)/10;
					sprintf(string_buf, "%ld%s%%", v32_1, space);
					break;
				case FIELD_ID_N_POTASSIUM_DV:
					nutriItemDecimalPoint = ReturnDecimalPointNutriTable(nutri_type, NUTRI_FACT_POTASSIUM);					
					v32_1 = ((INT32U)nutrifact_str4.nutri_data[NUTRI_FACT_POTASSIUM]*1000L/power10(nutriItemDecimalPoint)/Nutri_DV_refer_table_USA[9]+5)/10;
					sprintf(string_buf, "%ld%s%%", v32_1, space);
					break;
				case FIELD_ID_N_ADDED_SUGARS_DV:
					nutriItemDecimalPoint = ReturnDecimalPointNutriTable(nutri_type, NUTRI_FACT_ADDED_SUGARS);					
					v32_1 = ((INT32U)nutrifact_str4.nutri_data[NUTRI_FACT_ADDED_SUGARS]*1000L/power10(nutriItemDecimalPoint)/Nutri_DV_refer_table_USA[10]+5)/10;
					sprintf(string_buf, "%ld%s%%", v32_1, space);
					break;
				case FIELD_ID_N_SUGARS_DV:
					nutriItemDecimalPoint = ReturnDecimalPointNutriTable(nutri_type, NUTRI_FACT_SUGARS);					
					v32_1 = ((INT32U)nutrifact_str4.nutri_data[NUTRI_FACT_SUGARS]*1000L/power10(nutriItemDecimalPoint)/Nutri_DV_refer_table_USA[11]+5)/10;
					sprintf(string_buf, "%ld%s%%", v32_1, space);
					break;
				default :
					continue;
			}
			LabCmd_SetTextField(fieldid,string_buf);
		}
	}
}
#else
void prtfield_generate_nutri(void)
{
	INT32U pnt;
	INT8U  ucTemp, nutri_type, index;
	INT8U  k;
	INT8U nutriItemDecimalPoint;

	INT8U nutri_2column_flag;
	INT8U actual_weight_flag;
	INT16U baseSize;
	int fieldid;
	INT32U v32_1;
	char string_buf[128];

	NUTRIFACT_STRUCT  nutrifact_str;
	NUTRIFACT_STRUCT2  nutrifact_str2;
#ifdef USE_EU_NUTRITIONFACT
	NUTRIFACT_STRUCT3  nutrifact_str3;
	char string_buf_big[3000];
#endif
	INT8U baseUnit[2][4] = {"g","ml"};
	char space[2] = "";

	if(PrtItemStr.nutri_no == 0) return;
	if(!prtsel_get_table(FIELD_ID_N_SERVING_SIZE)) return;

	ucTemp = get_global_bparam(GLOBAL_SALE_SETUP_FLAG9);
	// nutri_type (0:normal, 1:CHL, 2:CAN, 3:AUS, 4:NZL, 5: AUS for actual weight, 6: NZL for actual weight)
	nutri_type = ucTemp&0x0f;

	pnt = DFLASH_BASE + FLASH_NUTRIFACT_AREA;
	if (nutri_type == NUTRI_TYPE_2COL_6DIG_AUS || nutri_type == NUTRI_TYPE_2COL_6DIG_AUS_ACTUAL_WEIGHT
		|| nutri_type == NUTRI_TYPE_6DIG_CZE)
	{
		pnt += sizeof(NUTRIFACT_STRUCT2)*(INT32U)(PrtItemStr.nutri_no-1);
		Flash_sread(pnt, (HUGEDATA INT8U *)&nutrifact_str2, sizeof(NUTRIFACT_STRUCT2));
		if(PrtItemStr.nutri_no != nutrifact_str2.code) return;
	}
#ifdef USE_EU_NUTRITIONFACT
	else if (nutri_type == NUTRI_TYPE_42ITEM_5DIG_EU_ACTUAL_WEIGHT)
	{
		pnt += sizeof(NUTRIFACT_STRUCT3)*(INT32U)(PrtItemStr.nutri_no-1);
		Flash_sread(pnt, (HUGEDATA INT8U *)&nutrifact_str3, sizeof(NUTRIFACT_STRUCT3));
		if(PrtItemStr.nutri_no != nutrifact_str3.code) return;
	}
#endif
	else
	{
		pnt += sizeof(NUTRIFACT_STRUCT)*(INT32U)(PrtItemStr.nutri_no-1);
		Flash_sread(pnt, (HUGEDATA INT8U *)&nutrifact_str, sizeof(NUTRIFACT_STRUCT));
		if(PrtItemStr.nutri_no != nutrifact_str.code) return;

		nutrifact_str2.code = nutrifact_str.code;
		strncpy((HUGEDATA char*)nutrifact_str2.description, (HUGEDATA char*)nutrifact_str.description, sizeof(nutrifact_str2.description));
		strncpy((HUGEDATA char*)nutrifact_str2.serving_size, (HUGEDATA char*)nutrifact_str.serving_size, sizeof(nutrifact_str2.serving_size));   //serving size
		strncpy((HUGEDATA char*)nutrifact_str2.serving_per, (HUGEDATA char*)nutrifact_str.serving_per, sizeof(nutrifact_str2.serving_per));    //serving per container
		for (k = 0; k < MAX_NUTRIFACT_ITEM; k++)
		{
			nutrifact_str2.nutri_data[k] = (INT32U)nutrifact_str.nutri_data[k];
		}
	}
	// Print Actual Weight for serving(NUTRI_TYPE_2COL_6DIG_AUS_ACTUAL_WEIGHT,NUTRI_TYPE_2COL_NZL_ACTUAL_WEIGHT)
	actual_weight_flag = 0;
	if(nutri_type == NUTRI_TYPE_2COL_6DIG_AUS_ACTUAL_WEIGHT || nutri_type == NUTRI_TYPE_2COL_NZL_ACTUAL_WEIGHT)
	{
		actual_weight_flag = 1;
	}
	// Print 2Column(CHL, AUS, NZL)
	nutri_2column_flag = 0;
	if(nutri_type == NUTRI_TYPE_2COL_CHL || nutri_type == NUTRI_TYPE_2COL_6DIG_AUS || nutri_type == NUTRI_TYPE_2COL_NZL\
	|| nutri_type == NUTRI_TYPE_2COL_6DIG_AUS_ACTUAL_WEIGHT || nutri_type == NUTRI_TYPE_2COL_NZL_ACTUAL_WEIGHT)
	{
		nutri_2column_flag = 1;	// 2 column
	}
	if(nutri_2column_flag)
	{
		baseSize = nutrifact_str2.nutri_data[NUTRI_FACT_BASE_SIZE];
		if(baseSize==0) return;
	}

	for (fieldid=FIELD_ID_N_SERVING_SIZE; fieldid<FIELD_ID_CAP_TTL_VOID_COUNT; fieldid++) {

		if (StrFieldStruct[fieldid].ExistField) 
		{
			if(nutri_type == NUTRI_TYPE_TOTALFAT_CAN)
			{
				sprintf(space, " ");
			}
			switch (fieldid) {
				case FIELD_ID_N_SERVING_SIZE:
					if(actual_weight_flag && (PrtItemStr.l_PluType == PLU_BY_WEIGHT) && (nutrifact_str2.nutri_data[NUTRI_FACT_BASE_UNIT]==0))
					{
						sprintf(string_buf, "%d%s", PrtItemStr.l_Price.Weight[PRICE_RESULT], baseUnit[index]);
					}
					else
					{
						sprintf(string_buf, (char *)nutrifact_str2.serving_size);
					}
					break;
				case FIELD_ID_N_SERVINGS_PER:
					if(actual_weight_flag && (PrtItemStr.l_PluType == PLU_BY_WEIGHT) && (nutrifact_str2.nutri_data[NUTRI_FACT_BASE_UNIT]==0))
					{
						sprintf(string_buf, "%d", 1);
					}
					else
					{
						sprintf(string_buf, (char *)nutrifact_str2.serving_per);
					}
					break;
				case FIELD_ID_N_BASE_SIZE:  //SG060109. Base Size field 추가 
					if(nutri_2column_flag)
					{
						index = 0; //0: "g"     1: "ml"
						if(nutrifact_str2.nutri_data[NUTRI_FACT_BASE_UNIT]==1)	index =1 ;
						sprintf(string_buf, "%d%s", nutrifact_str2.nutri_data[NUTRI_FACT_BASE_SIZE],baseUnit[index]);
					}
					else
						continue;
					break;
				case FIELD_ID_N_CALORIES:
					getNutriStringData(string_buf, &nutrifact_str2, NUTRI_FACT_CALORIES, nutri_type); 
					break;
				case FIELD_ID_N_CALORIES_FAT:
					getNutriStringData(string_buf, &nutrifact_str2, NUTRI_FACT_CALORIES_FAT, nutri_type); 
					break;
				case FIELD_ID_N_TOTAL_FAT:
					getNutriStringData(string_buf, &nutrifact_str2, NUTRI_FACT_TOTAL_FAT, nutri_type); 
					break;
				case FIELD_ID_N_SATURATED_FAT:
					getNutriStringData(string_buf, &nutrifact_str2, NUTRI_FACT_SATURATED_FAT, nutri_type); 
					break;
				case FIELD_ID_N_TRANS_FAT:
					getNutriStringData(string_buf, &nutrifact_str2, NUTRI_FACT_TRANS_FAT, nutri_type); 
					break;
				case FIELD_ID_N_CHOLESTEROL:
					getNutriStringData(string_buf, &nutrifact_str2, NUTRI_FACT_CHOLESTEROL, nutri_type); 
					break;			   
				case FIELD_ID_N_SODIUM:
					getNutriStringData(string_buf, &nutrifact_str2, NUTRI_FACT_SODIUM, nutri_type); 
					break;
				case FIELD_ID_N_TOTAL_CARBO:
					getNutriStringData(string_buf, &nutrifact_str2, NUTRI_FACT_TOTAL_CARBO, nutri_type); 
					break;
				case FIELD_ID_N_DIETARY_FIBERS:
					getNutriStringData(string_buf, &nutrifact_str2, NUTRI_FACT_DIETARY_FIBERS, nutri_type); 
					break;
				case FIELD_ID_N_SUGARS:
					getNutriStringData(string_buf, &nutrifact_str2, NUTRI_FACT_SUGARS, nutri_type); 
					break;
				case FIELD_ID_N_PROTEIN:
					getNutriStringData(string_buf, &nutrifact_str2, NUTRI_FACT_PROTEIN, nutri_type); 
					break;
				case FIELD_ID_N_VITAMIN_A:
					getNutriStringData(string_buf, &nutrifact_str2, NUTRI_FACT_VITAMIN_A, nutri_type); 
					break;
				case FIELD_ID_N_CALCIUM:
					getNutriStringData(string_buf, &nutrifact_str2, NUTRI_FACT_CALCIUM, nutri_type); 
					break;
				case FIELD_ID_N_VITAMIN_C:
					getNutriStringData(string_buf, &nutrifact_str2, NUTRI_FACT_VITAMIN_C, nutri_type); 
					break;
				case FIELD_ID_N_IRON:
					getNutriStringData(string_buf, &nutrifact_str2, NUTRI_FACT_IRON, nutri_type); 
					break;
				case FIELD_ID_N_SALT:
					getNutriStringData(string_buf, &nutrifact_str2, NUTRI_FACT_SALT, nutri_type);
					break;
#ifdef USE_EU_NUTRITIONFACT
				// USE_EU_NUTRITIONFACT가 켜져있으면 FIELD_ID_N_ENERGY가 정의가 안되기 때문에 Block처리
#else
				case FIELD_ID_N_ENERGY:
					getNutriStringData(string_buf, &nutrifact_str2, NUTRI_FACT_ENERGY, nutri_type);
					break;
#endif	

//=============  Daily Value  ====================
//Daily value는 Nutri_DV_refer_table(일일권장량)과 비교하여 소수점 없이 round off 하여 percentage로 표기함.
				case FIELD_ID_N_TOTAL_FAT_DV:
					nutriItemDecimalPoint = ReturnDecimalPointNutriTable(nutri_type, NUTRI_FACT_TOTAL_FAT);
					v32_1 = ((INT32U)nutrifact_str2.nutri_data[NUTRI_FACT_TOTAL_FAT]*1000L/power10(nutriItemDecimalPoint)/Nutri_DV_refer_table[0]+5)/10;
					sprintf(string_buf, "%ld%s%%", v32_1, space);
					break;
				case FIELD_ID_N_SATURATED_FAT_DV:
					nutriItemDecimalPoint = ReturnDecimalPointNutriTable(nutri_type, NUTRI_FACT_SATURATED_FAT);
					if(nutri_type == NUTRI_TYPE_TOTALFAT_CAN)	//canada, parameter 548 : 2, saturated+trans
					{
						v32_1 = (((INT32U)nutrifact_str2.nutri_data[NUTRI_FACT_SATURATED_FAT] + (INT32U)nutrifact_str2.nutri_data[NUTRI_FACT_TRANS_FAT])*1000L/power10(nutriItemDecimalPoint)/Nutri_DV_refer_table[1]+5)/10;
						sprintf(string_buf, "%ld%s%%", v32_1, space);
					}
					else
					{
						v32_1 = ((INT32U)nutrifact_str2.nutri_data[NUTRI_FACT_SATURATED_FAT]*1000L/power10(nutriItemDecimalPoint)/Nutri_DV_refer_table[1]+5)/10;
						sprintf(string_buf, "%ld%s%%", v32_1, space);
					}
					break;
				case FIELD_ID_N_CHOLESTEROL_DV:
					nutriItemDecimalPoint = ReturnDecimalPointNutriTable(nutri_type, NUTRI_FACT_CHOLESTEROL);
					v32_1 = ((INT32U)nutrifact_str2.nutri_data[NUTRI_FACT_CHOLESTEROL]*1000L/power10(nutriItemDecimalPoint)/Nutri_DV_refer_table[2]+5)/10;
					sprintf(string_buf, "%ld%s%%", v32_1, space);
					break;
				case FIELD_ID_N_SODIUM_DV:
					nutriItemDecimalPoint = ReturnDecimalPointNutriTable(nutri_type, NUTRI_FACT_SODIUM);
					v32_1 = ((INT32U)nutrifact_str2.nutri_data[NUTRI_FACT_SODIUM]*1000L/power10(nutriItemDecimalPoint)/Nutri_DV_refer_table[3]+5)/10;
					sprintf(string_buf, "%ld%s%%", v32_1, space);
					break;
				case FIELD_ID_N_TOTAL_CARBO_DV:
					nutriItemDecimalPoint = ReturnDecimalPointNutriTable(nutri_type, NUTRI_FACT_TOTAL_CARBO);					
					v32_1 = ((INT32U)nutrifact_str2.nutri_data[NUTRI_FACT_TOTAL_CARBO]*1000L/power10(nutriItemDecimalPoint)/Nutri_DV_refer_table[4]+5)/10;
					sprintf(string_buf, "%ld%s%%", v32_1, space);
					break;
				case FIELD_ID_N_DIETARY_FIBERS_DV:
					nutriItemDecimalPoint = ReturnDecimalPointNutriTable(nutri_type, NUTRI_FACT_DIETARY_FIBERS);					
					v32_1 = ((INT32U)nutrifact_str2.nutri_data[NUTRI_FACT_DIETARY_FIBERS]*1000L/power10(nutriItemDecimalPoint)/Nutri_DV_refer_table[5]+5)/10;
					sprintf(string_buf, "%ld%s%%", v32_1, space);
					break;
#ifdef USE_EU_NUTRITIONFACT
				case FIELD_ID_N_INTEGRATE_FORM:
                    if (nutri_type == NUTRI_TYPE_42ITEM_5DIG_EU_ACTUAL_WEIGHT)
                    {
                        getEuNutriStringData(string_buf_big, &nutrifact_str3, nutri_type);
                    }
					break;
#endif
				default :
					continue;
			}
#ifdef USE_EU_NUTRITIONFACT
            if (fieldid == FIELD_ID_N_INTEGRATE_FORM &&  nutri_type == NUTRI_TYPE_42ITEM_5DIG_EU_ACTUAL_WEIGHT)
                LabCmd_SetTextField_Direct(fieldid, string_buf_big, sizeof(string_buf_big));
            else
			LabCmd_SetTextField(fieldid,string_buf);
#else
			LabCmd_SetTextField(fieldid,string_buf);
#endif
		}
	}
}
#endif//USE_ADDITIONAL_NUTRIENT
#endif

void prtfield_generate_item_caption(void)
{
	INT16S fieldid;
	char string_buf[128];
	long v32_1;
	INT16U v16_1, v16_2;


	fieldid = FIELD_ID_CAP_TOTAL;
	if(StrFieldStruct[fieldid].ExistField && !label_set.flagZeroUnitPrice && prtsel_get_table(FIELD_ID_TOTALPRICE)) {
		if(Operation.operationMode == MODE_REPACK && (PrtOper.PrtRepackSymbol)) {
			if(label_caption_get_string(LABCAP_ID_TOTAL_REPACK, string_buf, 0)) {
				LabCmd_SetTextField(fieldid,string_buf);
			}
		} else if(prtsel_get_table(FIELD_ID_CAP_TOTAL)) {
			if(label_caption_get_string(LABCAP_ID_TOTAL, string_buf, 0)) {
				LabCmd_SetTextField(fieldid,string_buf);
			}
		}
	}

	if(!prtsel_get_table(FIELD_ID_CAP_TOTAL)) return;

	for (fieldid=FIELD_ID_CAP_TTL_VOID_COUNT; fieldid<=240; fieldid++) {  
		if (StrFieldStruct[fieldid].ExistField) {
			switch (fieldid) {
				case FIELD_ID_CAP_2ND_TOTAL_PRICE :
					if(label_set.flagZeroUnitPrice) continue;
					if(!PrtItemStr.CPrice.CurrencyRate) continue;
					if(!prtsel_get_table(FIELD_ID_TOTALPRICE)) continue;
					if(!label_caption_get_string(LABCAP_ID_TOTAL, string_buf, 1)) continue;
					break;
				case FIELD_ID_CAP_UNIT_PRICE :
#ifndef USE_CHINA_DEFAULT
					if(label_set.flagZeroUnitPrice) continue;
					if(!prtsel_get_table(FIELD_ID_UNIT_PRICE)) continue;
					if(PrtItemStr.fixed_price == 1){
						v16_1 = LABCAP_ID_UNIT_PRICE_CNT;
					}
					else if(PrtItemStr.l_PluType == PLU_BY_PCS) {		// by-count
						v16_1 = LABCAP_ID_UNIT_PRICE_PCS;
					} else if(PrtItemStr.l_PluType == PLU_BY_COUNT) {	// non-food
						v16_1 = LABCAP_ID_UNIT_PRICE_CNT;
					} else {					// by-weight
						v16_1 = LABCAP_ID_UNIT_PRICE_WEIGHT;
					}
					if(!label_caption_get_string(v16_1, string_buf, 0)) continue;
					break;
#else
					if(PrtItemStr.fixed_price == 1) continue;
					if(label_set.flagZeroUnitPrice) continue;
					if(!prtsel_get_table(FIELD_ID_UNIT_PRICE)) continue;
					if(PrtItemStr.l_PluType == PLU_BY_PCS) {		// by-count
						v16_1 = LABCAP_ID_UNIT_PRICE_PCS;
					} else if(PrtItemStr.l_PluType == PLU_BY_COUNT) {	// non-food
						v16_1 = LABCAP_ID_UNIT_PRICE_CNT;
					} else {					// by-weight
						v16_1 = LABCAP_ID_UNIT_PRICE_WEIGHT;
					}
					if(!label_caption_get_string(v16_1, string_buf, 0)) continue;
					break;
#endif
	 			case FIELD_ID_CAP_2ND_UNIT_PRICE :
					if(PrtItemStr.fixed_price == 1) continue;
					if(label_set.flagZeroUnitPrice) continue;
					if(!PrtItemStr.CPrice.CurrencyRate) continue;
					if(!prtsel_get_table(FIELD_ID_UNIT_PRICE)) continue;
					if(PrtItemStr.l_PluType == PLU_BY_PCS) {		// by-count
						v16_1 = LABCAP_ID_UNIT_PRICE_PCS;
					} else if(PrtItemStr.l_PluType == PLU_BY_COUNT) {	// non-food
						v16_1 = LABCAP_ID_UNIT_PRICE_CNT;
					} else {					// by-weight
						v16_1 = LABCAP_ID_UNIT_PRICE_WEIGHT;
					}
					if(!label_caption_get_string(v16_1, string_buf, 1)) continue;
					break;
				case FIELD_ID_CAP_WEIGHT :
					if(PrtItemStr.fixed_price == 1) continue;
					if(PrtItemStr.l_PluType <= PLU_BY_WEIGHT) {		// by-weight
						if(!prtsel_get_table(FIELD_ID_WEIGHT)) continue;
						if(PrtItemStr.l_FixedWeight) {
#ifdef USE_PRINT_FILL_WEIGHT
                            continue;
#else
							if(!label_caption_get_string(LABCAP_ID_MAN_WEIGHT, string_buf, 0)) continue;
#endif
						} else {
							if(!label_caption_get_string(LABCAP_ID_WEIGHT, string_buf, 0)) continue;
						}
					} else {		// by-count, non-food
						if (Prt_labelCap_flag) continue;
						else {
							if(!prtsel_get_table(FIELD_ID_QUANTITY)) continue;
							if(!label_caption_get_string(LABCAP_ID_QUANTITY, string_buf, 0)) continue;
						}
					}
					break;
				case FIELD_ID_CAP_TARE :
					if(!prtsel_get_table(FIELD_ID_TARE_VALUE)) continue;
					if(PrtItemStr.fixed_price == 1) continue;
					if(PrtItemStr.l_PluType == PLU_BY_WEIGHT) {			// by weight
						if(!label_caption_get_string(LABCAP_ID_TARE, string_buf, 0)) continue;
					} else {
						continue;
					}
					break;
				case FIELD_ID_CAP_DISC_PRICE :
					if(!prtsel_get_table(FIELD_ID_SAVED_PRICE)) continue;
					if(PrtItemStr.l_Price.TotalPrice[PRICE_SAVED] == 0) continue; //SG061215
					if(!label_caption_get_string(LABCAP_ID_DISC_PRICE, string_buf, 0)) continue;
					break;
				case FIELD_ID_CAP_REG_PRICE :
					if(!label_caption_get_string(LABCAP_ID_REG_PRICE, string_buf, 0)) continue;
					break;
#ifdef USE_TAX_RATE                    
				case FIELD_ID_CAP_TAX_PRICE :
					if(!prtsel_get_table(FIELD_ID_TAX_PRICE)) continue;
					if(!label_caption_get_string(LABCAP_ID_TAX_PRICE, string_buf, 0)) continue;
					break;
#endif                    
				case FIELD_ID_CAP_PACKEDDATE :
					if(!prtsel_get_table(FIELD_ID_PACKED_DATE)) continue;
					if(PrtItemStr.flagpackedondate == 1) continue;
					if(!label_caption_get_string(LABCAP_ID_PACKEDDATE, string_buf, 0)) continue;
					break;
				case FIELD_ID_CAP_SELLBYDATE :
					if(!prtsel_get_table(FIELD_ID_SELL_BY_DATE)) continue;
					if(PrtItemStr.flagsellbydate == 1) continue;					
#ifdef USE_DONT_SAVE_VALIDDATE_SALEDATE
					if(!PrtItemStr.sellbydate && !PrtItemStr.sellbytime && PrtItemStr.flagsellbydate != 2) // Sell by Date 과 Sell by Time 이 둘다 0일경우
					{
						if(temp_selldate.onoff==OFF)continue;
						if((temp_selldate.time == 0) && (temp_selldate.date == 0) )continue;
					}
#else
					if(!PrtItemStr.sellbydate && !PrtItemStr.sellbytime && PrtItemStr.flagsellbydate != 2) continue; // Sell by Date 과 Sell by Time 이 둘다 0일경우
#endif
					if(!label_caption_get_string(LABCAP_ID_SELLBYDATE, string_buf, 0)) continue;
					break;
				case FIELD_ID_CAP_COOKBYDATE :
					if(!prtsel_get_table(FIELD_ID_COOK_BY_DATE)) continue;
					if(!PrtItemStr.cookbydate) continue;
					if(!label_caption_get_string(LABCAP_ID_COOKBYDATE, string_buf, 0)) continue;
					break;
				case FIELD_ID_CAP_PRODUCEDDATE :
					if(!prtsel_get_table(FIELD_ID_PRODUCED_DATE)) continue;
					if(!PrtItemStr.produceddate) continue;
					if(!label_caption_get_string(LABCAP_ID_PRODUCEDDATE, string_buf, 0)) continue;
					break;
				case FIELD_ID_CAP_PACKEDTIME :
					if(!prtsel_get_table(FIELD_ID_PACKED_TIME)) continue;
					if(PrtItemStr.flagpackedontime == 1) continue;
					if(!label_caption_get_string(LABCAP_ID_PACKEDTIME, string_buf, 0)) continue;
					break;
				case FIELD_ID_CAP_SELLBYTIME :
					if(!prtsel_get_table(FIELD_ID_SELL_BY_TIME)) continue;
					if(PrtItemStr.flagsellbytime == 1) continue;
#ifdef USE_DONT_SAVE_VALIDDATE_SALEDATE
					if(!PrtItemStr.sellbydate && !PrtItemStr.sellbytime && PrtItemStr.flagsellbydate != 2) // Sell by Date 과 Sell by Time 이 둘다 0일경우
					{
						if(temp_selldate.onoff==OFF)continue;
						if(temp_selldate.time == 0)
						{
							if(temp_selldate.date == 0)continue;
							else 
							{
								if(!(get_global_bparam(GLOBAL_SALE_SETUP_FLAG14) & 0x02))continue;
							}
						}
					}
#else
					if(!PrtItemStr.sellbydate && !PrtItemStr.sellbytime && PrtItemStr.flagsellbytime != 2) continue; // Sell by Date 과 Sell by Time 이 둘다 0일경우
#endif
					if(!label_caption_get_string(LABCAP_ID_SELLBYTIME, string_buf, 0)) continue;
					break;
				case FIELD_ID_CAP_QUANTITY :
					if(PrtItemStr.l_PluType <= PLU_BY_WEIGHT) {
						if (Prt_labelCap_flag) continue;
						else {
							if(!prtsel_get_table(FIELD_ID_QUANTITY)) continue;
							if(!label_caption_get_string(LABCAP_ID_QUANTITY, string_buf, 0)) continue;
						}
					}
					else {
						if(!prtsel_get_table(FIELD_ID_QUANTITY)) continue;
						if(!label_caption_get_string(LABCAP_ID_QUANTITY, string_buf, 0)) continue;
					}
					break;
				case FIELD_ID_CAP_REFERENCE :
					if(!prtsel_get_table(FIELD_ID_TRACEABILITY_CODE)) continue;
					if(!label_caption_get_string(LABCAP_ID_REFERENCE, string_buf, 0)) continue;
					break;
#ifdef  USE_BELARUS_LABEL			// 벨라루스 전용 라벨 born in/bred in을 다른 용도로 사용	
				case FIELD_ID_CAP_BORN_IN :
				case FIELD_ID_CAP_BRED_IN :					
					if(PrtItemStr.fixed_price == 1) continue;
					if(PrtItemStr.l_PluType <= PLU_BY_WEIGHT) { 	// by-weight
						if(!label_caption_get_string(LABCAP_ID_BORN_IN, string_buf, 0)) continue;
					} else {		// by-count
						if(!label_caption_get_string(LABCAP_ID_BRED_IN, string_buf, 0)) continue;
					}
					break;
#else
				case FIELD_ID_CAP_BORN_IN :
					if(!prtsel_get_table(FIELD_ID_TRACEABILITY_CODE)) continue;
					if(!label_caption_get_string(LABCAP_ID_BORN_IN, string_buf, 0)) continue;
					break;
				case FIELD_ID_CAP_BRED_IN :
					if(!prtsel_get_table(FIELD_ID_TRACEABILITY_CODE)) continue;
					if(!label_caption_get_string(LABCAP_ID_BRED_IN, string_buf, 0)) continue;
					break;
#endif					
				case FIELD_ID_CAP_SLAUGHT_IN :
					if(!prtsel_get_table(FIELD_ID_TRACEABILITY_CODE)) continue;
					if(!label_caption_get_string(LABCAP_ID_SLAUGHT_IN, string_buf, 0)) continue;
					break;
				case FIELD_ID_CAP_CUT_IN :
					if(!prtsel_get_table(FIELD_ID_TRACEABILITY_CODE)) continue;
					if(!label_caption_get_string(LABCAP_ID_CUT_IN, string_buf, 0)) continue;
					break;
				case FIELD_ID_CAP_NET_WEIGHT :
					if(!prtsel_get_table(FIELD_ID_NET_WEIGHT)) continue;
					if(PrtItemStr.l_PluType == PLU_BY_WEIGHT);	// Modified by CJK 20060509
					else if((PrtItemStr.l_PluType == PLU_BY_COUNT || PrtItemStr.l_PluType == PLU_BY_PCS) 
							&& GlbOper.byCountApplyWeight 
							&& PrtItemStr.l_Price.Weight[PRICE_RESULT]);
					else continue;
					if(PrtItemStr.l_Price.NetWeight<=0) continue;	//JJANG 20090313 임시수정, 무게가 0보다 적을 때 이상한 값이 찍히는 현상 방지
					if(!label_caption_get_string(LABCAP_ID_NET_WEIGHT, string_buf, 0)) continue;
					break;
				case FIELD_ID_CAP_GROSS_WEIGHT :
#ifndef USE_PRINT_FILL_WEIGHT
					if(!prtsel_get_table(FIELD_ID_GROSS_WEIGHT)) continue;
					if(PrtItemStr.l_PluType == PLU_BY_WEIGHT);	// Modified by CJK 20060509
					else continue;
					if(!PrtItemStr.l_Price.GrossWeight) continue;
#else
					if(!PrtItemStr.l_FixedWeight) continue;
#endif
					if(!label_caption_get_string(LABCAP_ID_GROSS_WEIGHT, string_buf, 0)) continue;
					break;
				case FIELD_ID_CAP_PERCENT_TARE :
					switch(PrtItemStr.l_PluNo) {
						case MISC_PLU_WEIGHT :
						case MISC_PLU_PCS    :
						case MISC_PLU_COUNT  :
						case 0:
							continue;
					}
					if(!prtsel_get_table(FIELD_ID_PERCENT_TARE)) continue;
					v32_1 = PrtItemStr.l_Price.PercentTareRate;
					plu_get_field(PrtItemStr.plu_addr-1, PTYPE_PERCENT_TARE_LIMIT, (INT8U *)&v16_1, (INT16S *)&v16_2,sizeof(v16_1));
					if(!v16_1 && !v32_1) continue;		// Tare%가 0, Limit%가 0 일때 인쇄X
					if(!label_caption_get_string(LABCAP_ID_PERCENT_TARE, string_buf, 0)) continue;
					break;
				case FIELD_ID_CAP_PERCENT_TARE_LIMIT :
					switch(PrtItemStr.l_PluNo) {
						case MISC_PLU_WEIGHT :
						case MISC_PLU_PCS    :
						case MISC_PLU_COUNT  :
						case 0:
							continue;
					}
					if(!prtsel_get_table(FIELD_ID_PERCENT_TARE_LIMIT)) continue;
					plu_get_field(PrtItemStr.plu_addr-1, PTYPE_PERCENT_TARE_LIMIT, (INT8U *)&v16_1, (INT16S *)&v16_2,sizeof(v16_1));
					if(!v16_1) continue;			// Limit%가 0일때 인쇄X
					if(!label_caption_get_string(LABCAP_ID_PERCENT_TARE_LIMIT, string_buf, 0)) continue;
					break;
				case FIELD_ID_CAP_INGREDIENT :
					if(!prtsel_get_table(FIELD_ID_INGREDIENT_STR) && !prtsel_get_table(FIELD_ID_INGREDIENT_STR2) ) continue;
					if(!label_caption_get_string(LABCAP_ID_INGREDIENT, string_buf, 0)) continue;
					break;
				case FIELD_ID_CAP_PLU_NO :
					if(!prtsel_get_table(FIELD_ID_PLU_NO)) continue;
					if(!label_caption_get_string(LABCAP_ID_PLU_NO, string_buf, 0)) continue;
					break;
				case FIELD_ID_CAP_ORIGIN_NO :
					if(!prtsel_get_table(FIELD_ID_ORIGIN_STR)) continue;
					if(!PrtItemStr.origin) continue;
					if(!label_caption_get_string(LABCAP_ID_ORIGIN, string_buf, 0)) continue;
					break;

				default :
					continue;
//					break;
			}
			LabCmd_SetTextField(fieldid,string_buf);
		}
	}
}

void prtfield_generate_total_caption(void)
{
	INT16S fieldid;
	char string_buf[128];

	fieldid = FIELD_ID_CAP_TTL_TOTAL;
	if(StrFieldStruct[fieldid].ExistField) {
		if(Operation.operationMode == MODE_REPACK && (PrtOper.PrtRepackSymbol)) {
			if(label_caption_get_string(LABCAP_ID_TOTAL_REPACK, string_buf, 0)) {
				LabCmd_SetTextField(fieldid,string_buf);
			}
		} else if(prtsel_get_table(FIELD_ID_CAP_TOTAL)) {
			if(label_caption_get_string(LABCAP_ID_TTL_TOTAL_PRICE, string_buf, 0)) {
				LabCmd_SetTextField(fieldid,string_buf);
			}
		}
	}

	if(!prtsel_get_table(FIELD_ID_CAP_TOTAL)) return;

	for (fieldid=FIELD_ID_CAP_TTL_VOID_PRICE; fieldid<FIELD_ID_BARCODE; fieldid++) {
		if (StrFieldStruct[fieldid].ExistField) {
			switch (fieldid) {
				case FIELD_ID_CAP_TTL_2ND_TOTAL_PRICE :
					if(!PrtTotalStr.CPrice.CurrencyRate) continue;
					if(!prtsel_get_table(FIELD_ID_TTL_2ND_TOTAL_PRICE)) continue;
					if(!label_caption_get_string(LABCAP_ID_TTL_TOTAL_PRICE, string_buf, 1)) continue;
					break;
				case FIELD_ID_CAP_TRAN_COUNT :
					if(!label_caption_get_string(LABCAP_ID_TTL_TOTAL_COUNT, string_buf, 0)) continue;
					break;
				case FIELD_ID_CAP_TTL_VOID_COUNT :
					if(!PrtTotalStr.VoidCount) continue;
					if(!label_caption_get_string(LABCAP_ID_TTL_VOID_COUNT, string_buf, 0)) continue;
					break;
				case FIELD_ID_CAP_TTL_VOID_PRICE :
					if(!PrtTotalStr.VoidCount) continue;
					if(!label_caption_get_string(LABCAP_ID_TTL_VOID_PRICE, string_buf, 0)) continue;
					break;
				case FIELD_ID_CAP_PAYMENT_PRICE :
					if(!label_set.en_print_payment) continue;
					if(!label_caption_get_string(LABCAP_ID_PAYMENT, string_buf, 0)) continue;
					break;
				case FIELD_ID_CAP_CHANGE_PRICE :
					if(!label_set.en_print_payment) continue;
					if(!PrtTotalStr.ChangePrice) continue;
					if(!label_caption_get_string(LABCAP_ID_CHANGE, string_buf, 0)) continue;
					break;
#ifdef USE_TAX_RATE                    
				case FIELD_ID_CAP_TTL_TAX_PRICE :
					if(!(PrtTotalStr.ExTaxPrice+PrtTotalStr.InTaxPrice)) continue;
					if(!prtsel_get_table(FIELD_ID_TTL_TAX_AMOUNT)) continue;
					if(!label_caption_get_string(LABCAP_ID_TTL_TOTAL_TAX, string_buf, 0)) continue;
					break;
#endif                    
				case FIELD_ID_CAP_TTL_TOTAL_WEIGHT :
					if(!label_caption_get_string(LABCAP_ID_TTL_TOTAL_WEIGHT, string_buf, 0)) continue;
					break;
				case FIELD_ID_CAP_TTL_TOTAL_QUANTITY :
					if(!label_caption_get_string(LABCAP_ID_TTL_TOTAL_QUANTITY, string_buf, 0)) continue;
					break;
				default :
					continue;
//					break;
			}
			LabCmd_SetTextField(fieldid,string_buf);
		}
	}
}


void prtfield_generate_total(INT8U label_mode)
{
#ifdef USE_TAX_RATE        
	long totalWithoutTax, totalWithTax, totalTaxPrice;
#endif
	INT8U decimal, dec_ch, len;
	INT16S fieldid;
	INT8U select;
	INT8U v8_1, v8_2;
	INT32U v32_1;
	char string_buf[128];
#ifdef USE_EXTEND_PRICE_PRIMARYSIGN
	char currency_sign1[6];
#else
	char currency_sign1[5];
#endif
	INT8U wt_prt_type;
	char w_sign1[5], w_sign2[5];
#ifdef USE_USA_FIXEDWEIGHT_UNIT_OZ
	INT8U rate=16;
	char string_buf1[128];
#endif

	decimal = get_global_bparam(GLOBAL_AREA_PRICE_DECIMALPOINT);
	dec_ch  = get_global_bparam(GLOBAL_DECIMAL_SIGN);

	wt_prt_type = 0;
	if(PrtWeightUnit == WEIGHT_UNIT_KG || PrtWeightUnit == WEIGHT_UNIT_G) {
		get_global_sparam(GLOBAL_WEIGHT_SYMBOL_1, (INT8U *)w_sign1, 4);
		get_global_sparam(GLOBAL_WEIGHT_SYMBOL_2, (INT8U *)w_sign2, 4);
	} else {
		get_global_sparam(GLOBAL_WEIGHT_LBSYMBOL_1, (INT8U *)w_sign1, 4);
		get_global_sparam(GLOBAL_WEIGHT_LBSYMBOL_2, (INT8U *)w_sign2, 4);
	}
    
#ifdef USE_TAX_RATE
	totalTaxPrice = PrtTotalStr.InTaxPrice + PrtTotalStr.ExTaxPrice;
	if(GlbOper.priceAddExTax) {
		totalWithTax    = PrtTotalStr.SummaryTotalPrice;
		totalWithoutTax = PrtTotalStr.SummaryTotalPrice - totalTaxPrice;
	} else {
		totalWithTax    = PrtTotalStr.SummaryTotalPrice + totalTaxPrice;
		totalWithoutTax = PrtTotalStr.SummaryTotalPrice;
	}
#endif
    
	for (fieldid=0; fieldid<MAX_STR_FIELD_NUM; fieldid++) {
		select = prtsel_get_table(fieldid);
		if (StrFieldStruct[fieldid].ExistField && select) {
			switch (fieldid) {
				case FIELD_ID_TTL_TOTAL_TITLE:
					if(label_mode == GTOTAL_LABEL) {
						label_caption_read(LABCAP_ID_TXT_TITLE_GTOTAL, string_buf);
					} else if(label_mode == TOTAL_LABEL) {
						label_caption_read(LABCAP_ID_TXT_TITLE_TOTAL, string_buf);
					} else {
						continue;
					}
					break;
				case FIELD_ID_PACKED_DATE:
					if(OneItemPrepack) continue;
				case FIELD_ID_CUR_DATE :	// Current Date
					rtc_makedate_str(string_buf, PrtComStr.time.year, PrtComStr.time.month, PrtComStr.time.date);
					break;
				case FIELD_ID_PACKED_TIME:
					if(OneItemPrepack) continue;
				case FIELD_ID_CUR_TIME :	// Current Time
					rtc_maketime_str(string_buf, PrtComStr.time.hour, PrtComStr.time.min, PrtComStr.time.sec);
					break;
					
				case FIELD_ID_TTL_TOTAL_PRICE:	// with Tax or without Tax 
#ifdef USE_INDIA_PRINT_UNIT
					PrtGenSystemCurPriceStr(OUTMODE_TICKET, (HUGEDATA INT8U *)string_buf, PrtTotalStr.SummaryTotalPrice, decimal, dec_ch);
#else
					format_value((HUGEDATA INT8U *)string_buf,PrtTotalStr.SummaryTotalPrice,decimal,dec_ch);
#endif
					break;
				case FIELD_ID_TTL_TOTAL_COUNT:
					sprintf(string_buf, "%d", PrtTotalStr.TotalCount);
					break;
				case FIELD_ID_TTL_VOID_PRICE:
					if(!PrtTotalStr.VoidCount) continue;
					format_value((HUGEDATA INT8U *)string_buf,PrtTotalStr.VoidPrice,decimal,dec_ch);
					break;
				case FIELD_ID_TTL_VOID_COUNT:
					if(!PrtTotalStr.VoidCount) continue;
					sprintf(string_buf, "%d", PrtTotalStr.VoidCount);
					break;
#ifdef USE_TAX_RATE                    
				case FIELD_ID_TTL_TAX_AMOUNT :
					if(!totalTaxPrice) continue;
					format_value((HUGEDATA INT8U *)string_buf,totalTaxPrice,decimal,dec_ch);
					break;
				case FIELD_ID_TTL_INTAX_AMOUNT:
					format_value((HUGEDATA INT8U *)string_buf,PrtTotalStr.InTaxPrice,decimal,dec_ch);
					break;
				case FIELD_ID_TTL_EXTAX_AMOUNT:
					format_value((HUGEDATA INT8U *)string_buf,PrtTotalStr.ExTaxPrice,decimal,dec_ch);
					break;
				case FIELD_ID_TTL_TOTAL_PRICE_EX_TAX:	// without Tax
					format_value((HUGEDATA INT8U *)string_buf,totalWithoutTax,decimal,dec_ch);
					break;
				case FIELD_ID_TTL_TOTAL_PRICE_IN_TAX:	// with Tax
					format_value((HUGEDATA INT8U *)string_buf,totalWithTax,decimal,dec_ch);
					break;
#endif                    
				case FIELD_ID_TTL_PAYMENT :
					if(!label_set.en_print_payment) continue;
					format_value((HUGEDATA INT8U *)string_buf,PrtTotalStr.PayPrice,decimal,dec_ch);
					break;
				case FIELD_ID_TTL_CHANGE :
					if(!label_set.en_print_payment) continue;
					if(!PrtTotalStr.ChangePrice) continue;	// Added by CJK 20040824
					format_value((HUGEDATA INT8U *)string_buf,PrtTotalStr.ChangePrice,decimal,dec_ch);
					break;
				case FIELD_ID_TTL_2ND_TOTAL_PRICE :
					if(!PrtTotalStr.CPrice.CurrencyRate) continue;
					PrtGenCurPriceStr_2long(string_buf, PrtTotalStr.CPrice.CurrencyPriceH8, PrtTotalStr.CPrice.CurrencyPriceL8, PrtTotalStr.CPrice.CurrencyPrtForm, 
									PrtTotalStr.CPrice.CurrencyString1, PrtTotalStr.CPrice.CurrencyString2, 
									PrtTotalStr.CPrice.CurrencyPriceDiv, dec_ch);
					break;
				case FIELD_ID_CURRENCY_RATE :
					if(!PrtTotalStr.CPrice.CurrencyRate) continue;
#ifdef USE_EXTEND_PRICE_PRIMARYSIGN
					get_global_sparam(GLOBAL_AREA_PRICE_PRIMARYSIGN,(INT8U *)currency_sign1,5);
					currency_sign1[5] = 0;
#else
					get_global_sparam(GLOBAL_AREA_PRICE_PRIMARYSIGN,(INT8U *)currency_sign1,4);
					currency_sign1[4] = 0;
#endif
					len = PrtGenCurPriceStr(string_buf, 1, 2, currency_sign1, currency_sign1, 0, dec_ch);
					string_buf[len++] = ' ';
					string_buf[len++] = '=';
					string_buf[len++] = ' ';
					//PrtGenCurPriceStrForLabel(string_buf+len, PrtTotalStr.CPrice.CurrencyRate, 1, 
					PrtGenCurPriceStr(string_buf+len, PrtTotalStr.CPrice.CurrencyRate, 2, 
									PrtTotalStr.CPrice.CurrencyString1, PrtTotalStr.CPrice.CurrencyString2, 
									PrtTotalStr.CPrice.CurrencyRateDiv, dec_ch);
					break;
				case FIELD_ID_TTL_TOTAL_WEIGHT :
					if(PrtTotalStr.TotalWeight < 0) continue;
					v8_1=get_decimal_pt(3, PrtWeightUnit);
					v8_2=dec_ch;
					PrtGenWeightStr(string_buf, PrtTotalStr.TotalWeight, wt_prt_type, w_sign1, w_sign2, v8_1, v8_2);
#ifdef USE_INDIA_PRINT_UNIT
					v8_1 = strlen(string_buf);
					if(CurPluWeightUnit == WEIGHT_UNIT_KG)
							get_global_sparam(GLOBAL_WEIGHT_SYMBOL_1,(INT8U *)string_buf+v8_1,16);
					else if(CurPluWeightUnit == WEIGHT_UNIT_G)
						get_global_sparam(GLOBAL_WEIGHT_SYMBOL_2,(INT8U *)string_buf+v8_1,16);
					string_buf[v8_1+16] = 0;
#endif
					break;
				case FIELD_ID_TTL_TOTAL_QUANTITY :
					sprintf(string_buf, "%d", PrtTotalStr.TotalQuantity);
					break;
#ifndef USE_ARABIC_NUMBER
				case FIELD_ID_NET_WEIGHT:
					if(!PrtTotalStr.TotalNetWeight) continue;
					v8_1=get_decimal_pt(4, PrtItemStr.plu_addr);
					v8_2=dec_ch;
#ifdef USE_USA_FIXEDWEIGHT_UNIT_OZ
					if (PrtWeightUnit == WEIGHT_UNIT_LB)
					{
						if (PrtItemStr.l_PluType != PLU_BY_WEIGHT || PrtItemStr.fixed_price){
							v32_1 = LbtoOz(PrtTotalStr.TotalNetWeight, rate, v8_1);  
							PrtGenWeightStr(string_buf, v32_1, 8, w_sign1, w_sign2, v8_1, v8_2);	// "6 lb 4 oz"
							sprintf(string_buf1, " (%s)", string_buf);				// " (6 lb 4 oz)"
							PrtGenWeightStr(string_buf, v32_1, 9, w_sign1, w_sign2, v8_1, v8_2);	// "100 oz"
							strcat(string_buf, string_buf1);	 				// "100 oz( 6 lb 4 oz)"
						}
					} 
					else PrtGenWeightStr(string_buf, PrtTotalStr.TotalNetWeight, 4, w_sign1, w_sign2, v8_1, v8_2); 	// "100 g"
#else
					PrtGenWeightStr(string_buf, PrtTotalStr.TotalNetWeight, wt_prt_type, w_sign1, w_sign2, v8_1, v8_2);
#endif					
					break;
#endif //USE_ARABIC_NUMBER
#ifndef USE_ARABIC_NUMBER
				case FIELD_ID_2ND_NET_WEIGHT :
					v32_1 = PrtTotalStr.TotalPerTareWeight + PrtTotalStr.TotalNetWeight;
					if(!v32_1) continue;
					v8_1=get_decimal_pt(4, PrtItemStr.plu_addr);
					v8_2=dec_ch;
					PrtGenWeightStr(string_buf, v32_1, wt_prt_type, w_sign1, w_sign2, v8_1, v8_2);
					break;
#endif //USE_ARABIC_NUMBER
				case FIELD_ID_GROSS_WEIGHT :
#ifdef USE_ARABIC_NUMBER
				case FIELD_ID_2ND_NET_WEIGHT :
#endif //USE_ARABIC_NUMBER				
					if(!PrtTotalStr.TotalGrossWeight) continue;
					v8_1=get_decimal_pt(4, PrtItemStr.plu_addr);
					v8_2=dec_ch;
					PrtGenWeightStr(string_buf, PrtTotalStr.TotalGrossWeight, wt_prt_type, w_sign1, w_sign2, v8_1, v8_2);
					break;
				default :
					continue;
			}
			LabCmd_SetTextField(fieldid,string_buf);
		}
	}
}

void prtfield_generate_barcode_item(void)
{
	INT16U barid;
	STRUCT_STRFORM_PARAM bar_param;
	INT16S fieldid;

	if(!prtsel_get_table(FIELD_ID_BARCODE)) return;
#ifdef USE_SETPRODUCT_SALE
	if(nonweighing_mode && (status_scale.Weight <= 0)) return;
#endif

	for (fieldid=0; fieldid<MAX_BAR_FIELD_NUM; fieldid++) {
		if (BarFieldStruct[fieldid].ExistField) {
			prt_set_strform_parameter_item(&bar_param, NULL);	// Modified by CJK 20050503
			barid = prt_get_plu_barid(PrtItemStr.l_PluNo, PrtItemStr.plu_addr, PrtItemStr.l_PluType, fieldid);
			if (BarcodeNumber)
			{
				barid = BarcodeNumber;
			}
#ifdef USE_SETPRODUCT_SALE
			if(setproduct_PrtFlag)
				barid = 6;
#endif
			if(barid == 0xff) continue;
			prt_make_barcode(LABEL, barid, &bar_param, fieldid);
		}

	}
}

#ifdef USE_AUSTRALIA_COOL_FUNCTION
void prtfield_generate_coolinfo(void)
{
	STRUCT_COOL_IMAGE cool_info;
	INT8U select;
	char string_buf[200];
	char print_logo;
	char print_bar;	

	select = prtsel_get_table(FIELD_ID_BONUS_POINT);
	if (StrFieldStruct[FIELD_ID_BONUS_POINT].ExistField && select) {
		prt_get_plu_coolinfo(PrtItemStr.plu_addr, &cool_info);
		switch(cool_info.Cooltype)
		{
			case 0: // Don't use Australia Origin
				return;
				print_logo = '0';
				print_bar = '0';
				if(cool_info.CoolCustom[0] == 0)sprintf(cool_info.CoolCustom, "Don't use Australian Origin");
				break;

			case 1: // Grown in Australia
				print_logo = '1';
				print_bar = '1';
				cool_info.CoolContainPercent = 100;
				if(cool_info.CoolCustom[0] == 0)sprintf(cool_info.CoolCustom, "Grown in Australia");
				break;
				
			case 2: // Produced 
				print_logo = '1';
				print_bar = '1';
				cool_info.CoolContainPercent = 100;
				if(cool_info.CoolCustom[0] == 0)sprintf(cool_info.CoolCustom, "Produced in Australia");
				break;			

			case 3: // Made in Australia
				print_logo = '1';
				print_bar = '1';
				if(cool_info.CoolCustom[0] == 0)
				{
					if(cool_info.CoolContainPercent == 0)
					{
						sprintf(cool_info.CoolCustom, "Made in Australia from 0%% Australian ingredients");
					}
					else if(cool_info.CoolContainPercent == 100)
					{
						sprintf(cool_info.CoolCustom, "Made in Australia from 100%% Australian ingredients");
					}
					else if(cool_info.CoolContainPercent < 10)
					{
						cool_info.CoolContainPercent = 10;
						sprintf(cool_info.CoolCustom, "Made in Australia from less than 10%% Australian ingredients");
					}
					else if(cool_info.CoolContainPercent < 100)
					{
						sprintf(cool_info.CoolCustom, "Made in Australia from at least %d%% Australian ingredients", cool_info.CoolContainPercent);
					}
				}
				break;
				
			case 4: // Packed in Australia
				print_logo = '0';
				print_bar = '1';
				if(cool_info.CoolCustom[0] == 0)
				{
					if(cool_info.CoolContainPercent == 0)
					{
						sprintf(cool_info.CoolCustom, "Packed in Australia from 0%% Australian ingredients");
					}
					else if(cool_info.CoolContainPercent == 100)
					{
						sprintf(cool_info.CoolCustom, "Packed in Australia from 100%% Australian ingredients");
					}
					else if(cool_info.CoolContainPercent < 10)
					{
						cool_info.CoolContainPercent = 10;						
						sprintf(cool_info.CoolCustom, "Packed in Australia from less than 10%% Australian ingredients");
					}
					else
					{
						sprintf(cool_info.CoolCustom, "Packed in Australia from at least %02d%% Australian ingredients", cool_info.CoolContainPercent);
					}

				}
				break;		
				
			case 5: // Imported 
				print_logo = '0';
				print_bar = '1';
				if(cool_info.CoolContainPercent == 0)print_bar = '0';
				
				if(cool_info.CoolCustom[0] == 0)
				{
					if(cool_info.CoolContainPercent == 0)
					{
						sprintf(cool_info.CoolCustom, "Produced in %s", cool_info.CoolOriginText);
					}
					else if(cool_info.CoolContainPercent == 100)
					{
						sprintf(cool_info.CoolCustom, "Produced in %s from 100%% Australian ingredients", cool_info.CoolOriginText);
					}
					else if(cool_info.CoolContainPercent < 10)
					{
						cool_info.CoolContainPercent = 10;					
						sprintf(cool_info.CoolCustom, "Produced in %s from less than 10%% Australian ingredients", cool_info.CoolOriginText);
					}
					else
					{
						sprintf(cool_info.CoolCustom, "Produced in %s from at least %02d%% Australian ingredients", cool_info.CoolOriginText, cool_info.CoolContainPercent);
					}	
				}
				break;		
			default: // invliad value
				sprintf(cool_info.CoolCustom,global_message[MSG_INVALID_VALUE]);
				break;

		}
		sprintf(string_buf, "%c%c%04d%s",print_logo, print_bar, cool_info.CoolContainPercent, cool_info.CoolCustom);		
		LabCmd_SetTextField(FIELD_ID_BONUS_POINT, string_buf);
	}
}
#endif

void prtfield_generate_barcode_total(void)
{
	INT16U barid;
	STRUCT_STRFORM_PARAM bar_param;
	INT16S fieldid;
#ifdef USE_QR_ADDTOTAL
	INT8U flag_tmp;
#endif

	if(!prtsel_get_table(FIELD_ID_BARCODE)) return;

	for (fieldid=0; fieldid<MAX_BAR_FIELD_NUM; fieldid++) {
		if (BarFieldStruct[fieldid].ExistField) {
			prt_set_strform_parameter_total(&bar_param);
			if (fieldid == 2)
				barid = get_global_bparam(GLOBAL_BARCODE_TOTAL_ID2);
			else
				barid = get_global_bparam(GLOBAL_BARCODE_TOTAL_ID);
			if(GlbFlagPCnMasterConnection==OFF) barid=99;
#ifdef USE_QR_ADDTOTAL
			flag_tmp = get_global_bparam(GLOBAL_SALE_SETUP_FLAG14);
			flag_tmp = OnOff(flag_tmp & 0x08);	//param 726
			if ((flag_tmp) && (fieldid == 2)); // param 726 Flag가 On이고 Add up Total Barcode 2(fieldid==2) 출력안함
			else
#endif
			prt_make_barcode(LABEL, barid, &bar_param, fieldid);
		}

	}
}


void prtfield_generate_labelserial(void)
{
	char string_buf[32], no_form[32];
	INT8U select;
	STRUCT_STRFORM_PARAM str_param;

	select = prtsel_get_table(FIELD_ID_TRANSACTION_NO);
	if (StrFieldStruct[FIELD_ID_TRANSACTION_NO].ExistField && select) {
		get_global_sparam(GLOBAL_LABEL_SERIAL_FORM,(INT8U *)no_form,32);
		no_form[31] = 0;
		prt_set_strform_parameter_item(&str_param, NULL);
		prt_make_strform(string_buf, no_form, &str_param, NO_BARCODE);	// serial format
		LabCmd_SetTextField(FIELD_ID_TRANSACTION_NO, string_buf);
	}
}

INT16U prtsel_get_menu_capcode(INT8U field_id)
{
	INT16U ret;
	INT8U id;

	ret = 0;
	id = 0;
	while (id < 64) {
		if(cap2field_table[id].FieldId == 0) break;
		if(cap2field_table[id].FieldId == field_id) {
			ret = cap2field_table[id].CapCode;
			break;
		}
		id++;
	}
	return ret;
}


void prtsel_set_table_buf(HUGEDATA INT8U *buf, INT8U field_no, INT8U val)
{
	INT8U temp;

	val &= 0x01;
	temp = 0x01 <<(field_no%8);
	if(val) buf[field_no/8] |= temp;
	else    buf[field_no/8] &= (~temp);
}

INT8U prtsel_get_table_buf(HUGEDATA INT8U *buf, INT8U field_no)
{
	INT8U val;

	val = buf[field_no/8];
	val >>= (field_no%8);
	val &= 0x01;

	return val;
}

INT8U prtsel_get_table(INT8U field_no)
{
	INT8U select;
	INT32U pnt;
	INT8U uctemp, uctemp2;
	
	uctemp = get_global_bparam(GLOBAL_SALE_SETUP_FLAG6);
	uctemp2 = (uctemp>>1)& 0x01;
	if((!uctemp2) || printdateOFF)	//modified by JJANG 20090421, 0 누르고 재포장키 누르면 날짜 안찍음
	{
		if((field_no==FIELD_ID_PACKED_DATE) ||(field_no==FIELD_ID_SELL_BY_DATE)
			|| (field_no==FIELD_ID_COOK_BY_DATE) || (field_no==FIELD_ID_PRODUCED_DATE)
			|| (field_no==FIELD_ID_PACKED_TIME) || (field_no==FIELD_ID_SELL_BY_TIME)
			|| (field_no==FIELD_ID_WEEK_DAY)  ) 
		
			return OFF;
	}
	// 인쇄설정에 상관없이 항상 PLU에 의해 출력하려면 아래 부분을 살릴 것(CJK080902)
	//if (fiele_no == FIELD_ID_PACKED_DATE && PrtItemStr.flagpackedondate == 1) return OFF;
	//else if (fiele_no == FIELD_ID_SELL_BY_DATE && PrtItemStr.flagsellbydate == 1) return OFF;
	//else if (fiele_no == FIELD_ID_PACKED_TIME && PrtItemStr.flagpackedontime == 1) return OFF;
	//else if (fiele_no == FIELD_ID_SELL_BY_TIME && PrtItemStr.flagsellbytime == 1) return OFF;
	/////////////////

	pnt = field_no/8;
	pnt += DFLASH_BASE;
	pnt += FLASH_GLOBAL_AREA;
	pnt += GLOBAL_PRT_INHIBIT;
	select = Flash_bread(pnt);
	select >>= (field_no%8);
	select &= 0x01;	
	return select;
}

void prtsel_set_table(INT8U field_no, INT8U val)
{
	INT8U select, temp;
	INT32U pnt;

	pnt = field_no/8;
	pnt += DFLASH_BASE;
	pnt += FLASH_GLOBAL_AREA;
	pnt += GLOBAL_PRT_INHIBIT;
	select = Flash_bread(pnt);

	val &= 0x01;
	temp = 0x01 << (field_no%8);
	if(val) select |= temp;
	else  	select &= (~temp);

	Flash_bwrite(pnt, select);
}


void prtsel_set_table_buf_pack(HUGEDATA INT8U *buf, INT8U field, INT8U val)
{
	switch(field) {
		case FIELD_ID_WEIGHT :	//WEIGHT		101,108,132 
			prtsel_set_table_buf(buf, FIELD_ID_WEIGHT, val);
			prtsel_set_table_buf(buf, FIELD_ID_MARK_DN_WEIGHT, val);
			prtsel_set_table_buf(buf, FIELD_ID_NEW_WEIGHT, val); 
			break;
		case FIELD_ID_QUANTITY :	//QUANTITY		112,133,134 
			prtsel_set_table_buf(buf, FIELD_ID_QUANTITY, val);
			prtsel_set_table_buf(buf, FIELD_ID_NEW_QUANTITY, val);
			prtsel_set_table_buf(buf, FIELD_ID_MARK_DN_QUANTITY, val); 
			break;
		case FIELD_ID_UNIT_PRICE :	//UNIT PRICE		  6,109,113 
			prtsel_set_table_buf(buf, FIELD_ID_UNIT_PRICE  , val);
			//prtsel_set_table_buf(buf, 91 , val);
			prtsel_set_table_buf(buf, FIELD_ID_MARK_DN_UNIT, val);
			prtsel_set_table_buf(buf, FIELD_ID_NEW_UNITPRICE, val); 
			break;
		case FIELD_ID_TOTALPRICE :	//TOTAL PRICE	102,110,114,136
			prtsel_set_table_buf(buf, FIELD_ID_TOTALPRICE, val);
			prtsel_set_table_buf(buf, FIELD_ID_MARK_DN_TOTAL, val);
			prtsel_set_table_buf(buf, FIELD_ID_NEW_TOTALPRICE, val);
			break;
		case FIELD_ID_SHOP_NAME_STR :	//STORE NAME(TEL, TEXT)	105,138,139
			prtsel_set_table_buf(buf, FIELD_ID_SHOP_NAME_STR, val);
			prtsel_set_table_buf(buf, FIELD_ID_SHOP_TEL_STR, val);
			prtsel_set_table_buf(buf, FIELD_ID_SHOP_TEXT_STR, val); 
			break;
		default :
			prtsel_set_table_buf(buf, field, val);
			break;
	}
}


void print_inhibit_init(void)
{
	Flash_clear(DFLASH_BASE+FLASH_GLOBAL_AREA+GLOBAL_PRT_INHIBIT,0xff,32);
}

void print_inhibit_set(void)
{
	CAPTION_STRUCT cap;
	INT8U   select[32];
	POINT	disp_p;
	long	ret_ptype;
	INT8U	page_num, max_page, str_cnt;
	INT16U	maxid,nlist,i, result;
	INT32U	addr;
	INT8U   menulist[64];
	INT8U   fieldid;
	INT16U  capcode;
	INT8U	temp;
	INT8U   backup_directdraw;

	addr = DFLASH_BASE;
	addr+= FLASH_GLOBAL_AREA;
	addr+= GLOBAL_PRT_INHIBIT;
	Flash_sread(addr, (HUGEDATA INT8U *)select, 32);
	
	maxid = 0;
	nlist = 0;
	caption_continuous_search = ON;
	caption_continuous_search_addr = 0;
	backup_directdraw = DspStruct.DirectDraw;
	while (maxid < 64) {
		if(cap2field_table[maxid].FieldId == 0) break;
		temp = caption_split_by_code(cap2field_table[maxid].CapCode, &cap, 0);	// 느리지만 table에 따란 Caption이 바뀔 수 있음을 감안
		if (temp == TRUE)				 //se-hyung 20071002  Caption에서 U1 설정에 맞게 변하도록 수정
		{
			if (caption_run_check(cap.code,cap.runflag)) {
				menulist[nlist++]=maxid;
			}
		}
		maxid++;
		
	}
	caption_continuous_search = OFF;
	page_num = 0;
	max_page = nlist/Startup.line_max;
	if (nlist%Startup.line_max) max_page++;
VIEW_PAGE:
	DspStruct.DirectDraw = 0;
	Dsp_Fill_Buffer(0);
	display_title_page_change(0x1710, page_num+1, max_page);
	for(i=0; i<Startup.line_max; i++)  {
		str_cnt = Startup.line_max*page_num +i;
		if(str_cnt >= nlist)	break;
		fieldid = cap2field_table[menulist[str_cnt]].FieldId;
		capcode = cap2field_table[menulist[str_cnt]].CapCode;
		ret_ptype = prtsel_get_table_buf(select, fieldid);
		caption_split_by_code(capcode, &cap,0);
		caption_adjust(&cap,NULL);
#if defined(USE_SINGLE_LINE_GRAPHIC)
		display_clear(DISPLAY_UNITPRICE);
		display_string(DISPLAY_UNITPRICE,(INT8U *)cap.form);
#else
		disp_p = point((i+Startup.menu_type)*Startup.menu_ygap, 0);
#ifdef HEBREW_FONT
		Convert_HebrewPreview((INT8U *)cap.form, strlen(cap.form), 0, 0); 
		hebrew_codetemp[strlen(cap.form)] = 0;
		disp_p.x = 202 - disp_p.x;
		Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
		Convert_ArabPreview((INT8U *)cap.form, strlen(cap.form), 0, 0, 0); 
		arab_codetemp[strlen(cap.form)] = 0;
		disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
		Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
		Dsp_Write_String(disp_p, (HUGEDATA char*)cap.form);	
#endif
#endif
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
		result = keyin_yesno(KI_GRAPHIC, (long *)&ret_ptype, (i+Startup.menu_type)*Startup.menu_ygap, cap.input_x*Startup.menu_xgap , KI_PASS);		
#else
		result = keyin_yesno(KI_DEFAULT, (long *)&ret_ptype, (i+Startup.menu_type)*Startup.menu_ygap, cap.input_x*Startup.menu_xgap , KI_PASS);
#endif
	}
	Dsp_Diffuse();
	for(i=0; i<Startup.line_max ; i++)  {
INPUT_PAGE:	str_cnt = Startup.line_max*page_num + i;
		if(str_cnt >= nlist)	break;
		fieldid = cap2field_table[menulist[str_cnt]].FieldId;
		capcode = cap2field_table[menulist[str_cnt]].CapCode;
		ret_ptype = prtsel_get_table_buf(select, fieldid);
		caption_split_by_code(capcode, &cap,0);
		caption_adjust(&cap,NULL);
		DspStruct.DirectDraw = 1;
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
		result = keyin_yesno(KI_GRAPHIC, (long *)&ret_ptype, (i+Startup.menu_type)*Startup.menu_ygap, cap.input_x*Startup.menu_xgap , KI_NOPASS);
#else
		result = keyin_yesno(KI_DEFAULT, (long *)&ret_ptype, (i+Startup.menu_type)*Startup.menu_ygap, cap.input_x*Startup.menu_xgap , KI_NOPASS);
#endif
		DspStruct.DirectDraw = backup_directdraw;
		switch(result)  {
			case KP_ESC:	goto RET_ESC;
			case KP_ARROW_UP:
				prtsel_set_table_buf_pack(select, fieldid, ret_ptype);
				if(i==0) 	goto PAGE_UP;
				i--;		
				goto INPUT_PAGE;
			case KP_PAGE_UP:
				prtsel_set_table_buf_pack(select, fieldid, ret_ptype);
PAGE_UP:			if(page_num==0)	{//goto RET_ESC;
					BuzOn(2);
					page_num = 0;
				} else {
					page_num--;
				}
				goto VIEW_PAGE;

			case KP_ARROW_DN:
			case KP_ENTER:
				prtsel_set_table_buf_pack(select, fieldid, ret_ptype);
				if(str_cnt >= nlist-1) {
					BuzOn(2);
					goto INPUT_PAGE;
				}
				if(i==(Startup.line_max-1)) goto PAGE_DN;
				break;

			case KP_PAGE_DN:
				prtsel_set_table_buf_pack(select, fieldid, ret_ptype);
PAGE_DN:			if(page_num >= max_page-1) {
					BuzOn(2);
					page_num = max_page-1;
				} else {
					page_num++;
				}
				goto VIEW_PAGE;

			case KP_SAVE:
				prtsel_set_table_buf_pack(select, fieldid, ret_ptype);

				goto	RET_SAVE;
			default:
				BuzOn(2);
				goto	INPUT_PAGE;
		}		
	}
RET_SAVE:
	Flash_swrite(addr, (HUGEDATA INT8U *)select, 32);
RET_ESC:
	return;		
}

void input_serial_no(void)
{
	CAPTION_STRUCT cap;
	INT16U result;
	char form_string[32], sr_form[2][32];


	get_global_sparam(GLOBAL_LABEL_SERIAL_FORM,(INT8U *)sr_form[0], 32);
	sr_form[0][31] = 0;
	get_global_sparam(GLOBAL_TICKET_SERIAL_FORM,(INT8U *)sr_form[1], 32);
	sr_form[1][31] = 0;
	strcpy((char*)form_string, "INPUWQDGKTABLFR9%M\0");		// Modified by CJK 20060309

	Menu_Init();
	caption_split_by_code(0x1740, &cap,0);
	Menu_HeadCreate((HUGEDATA char *)cap.form);

	caption_split_by_code(0x3741, &cap,0);	// Scale ID
	caption_adjust(&cap,NULL);
	cap.input_length=31;
	Menu_InputCreate(1, (HUGEDATA char *)cap.form, MENU_TYPE_BAR, cap.input_x, cap.input_dlength, cap.input_length, 
					 cap.input_max, cap.input_min, 0, NULL, (HUGEDATA INT8U *)sr_form[0], (HUGEDATA INT8U *)form_string, NULL);
	Menu_SetProperties(1, MENU_P_ENGLISH_MODE);

#ifndef USE_ONLY_LABEL  //KKH for CL3500
	caption_split_by_code(0x3742, &cap,0);	// Scale ID
	caption_adjust(&cap,NULL);
	cap.input_length=31;
	Menu_InputCreate(2, (HUGEDATA char *)cap.form, MENU_TYPE_BAR, cap.input_x, cap.input_dlength, cap.input_length, 
					 cap.input_max, cap.input_min, 0, NULL, (HUGEDATA INT8U *)sr_form[1], (HUGEDATA INT8U *)form_string, NULL);
	Menu_SetProperties(2, MENU_P_ENGLISH_MODE);
#endif
        
	result = Menu_InputDisplay();
	if(result == MENU_RET_SAVE) {
		set_global_sparam(GLOBAL_LABEL_SERIAL_FORM,(INT8U *)sr_form[0], 32);
		set_global_sparam(GLOBAL_TICKET_SERIAL_FORM,(INT8U *)sr_form[1],32);
	}
}

#ifdef USE_CHECK_DUPLICATION_TICKET
INT32U input_duplication_yesno(void)
{
	INT8U old_font, key_mode;
	INT32U input_value;
	CAPTION_STRUCT	cap;

	key_mode = KEY_GetMode();
	KEY_SetMode(1);
	old_font = DspStruct.Id1;
	DspLoadFont1(DSP_MENU_FONT_ID);

	Menu_Init();
	caption_split_by_code(0x38C5, &cap,0);
	caption_adjust(&cap,NULL);
	Menu_InputCreate(1, (HUGEDATA char *)cap.form, MENU_TYPE_YN, cap.input_x, cap.input_dlength, cap.input_length,
	cap.input_max, cap.input_min, 0, &input_value, NULL, NULL, NULL);
	Menu_InputDisplay();

	DspLoadFont1(old_font);
	KEY_SetMode(key_mode);
	
	return input_value;
}
#endif

INT8U get_label_name(INT16U labelid, char *str)
{
	INT32U f_addr, addr;
	INT16U id, label_idx;
	INT8U i;
	struct LABEL_HEADER_INFO info;
	memset(&info, 0, sizeof(struct LABEL_HEADER_INFO));

	str[0] = 0;

	if(labelid<=MAX_LABEL_ROM_NO) {
		addr = label_rom_addr(labelid);
		if (addr == 0) return FALSE;
		addr += PFLASH_DATA_BASE;
		addr += 4;	// width(2), height(2)
		memcpy(str, (HUGEDATA char *)addr, 16);
		str[15] = 0;
		return TRUE;
	}
	
	label_idx = get_new_labeladdress(labelid, 1);

	if(label_idx == 999)
	{
		return FALSE;
	}
	addr = get_addr_from_label_idx(label_idx, LABEL_AREA_HEADER);
	read_label_header(addr, &info, L_H_I_N_BIT);
	strncpy(str, info.img_name, LABEL_HEADER_IMG_NAME_SIZE);	

	str[15] = 0;
	return TRUE;
}

INT8U get_label_size(INT16U labelid, INT16U *w, INT16U *h)
{
	INT32U f_addr, addr;
	INT16U id, label_idx;
	INT8U i;
	struct LABEL_HEADER_INFO info;

	memset((void*)&info, 0, sizeof(info));

	*w = 0;
	*h = 0;

	if(labelid<=MAX_LABEL_ROM_NO) {
		addr = label_rom_addr(labelid);
		if (addr == 0) return FALSE;
		addr += PFLASH_DATA_BASE;
		memcpy(w, (HUGEDATA char *)addr, 2);
		addr += 2;
		memcpy(h, (HUGEDATA char *)addr, 2);
		addr += 2;
		return TRUE;
	}
	label_idx = get_new_labeladdress(labelid, 1);

	if(label_idx == 999)
	{
		return FALSE;
	}
	addr = get_addr_from_label_idx(label_idx, LABEL_AREA_HEADER);

	read_label_header(addr, &info, L_H_I_W_BIT | L_H_I_H_BIT);
	*w = info.img_w;
	*h = info.img_h;

	return TRUE;
}

void print_rewind(void)
{
	char string_buf[10];
	INT8U i;
	i = 0;
	string_buf[i++] = GS;
	string_buf[i++] = '%';
	PutDataRxBuf(&CommBufPrt,string_buf,i);
}

void prtfield_set_variable_testlabel(INT32U plu_addr)
{
	INT8U temp, wunit,ret;
	INT16S sz;
	INT8U tare_id;
#ifdef USE_USA_FIXEDWEIGHT_UNIT_OZ
	INT8U decimal;
#endif

	plu_get_field(plu_addr-1, PTYPE_PRICE, (INT8U *)&PrtItemStr.l_Price.UnitPrice[PRICE_RESULT], (INT16S *)&sz,sizeof(PrtItemStr.l_Price.UnitPrice[PRICE_RESULT]));
	plu_get_field(plu_addr-1, PTYPE_PIECE, (INT8U *)&PrtItemStr.l_Price.UnitCount[PRICE_RESULT], (INT16S *)&sz,sizeof(PrtItemStr.l_Price.UnitCount[PRICE_RESULT]));
	plu_get_field(plu_addr-1, PTYPE_PERCENT_TARE, (INT8U *)&PrtItemStr.l_Price.PercentTareRate, (INT16S *)&sz,sizeof(PrtItemStr.l_Price.PercentTareRate));
	plu_get_field(plu_addr-1, PTYPE_PLU_TYPE, (INT8U *)&PrtItemStr.l_PluType, (INT16S *)&sz,sizeof(PrtItemStr.l_PluType));
#ifdef USE_FIXED_PLUTYPE//USE_EMART_NETWORK
	if (PrtItemStr.l_PluType >= PLU_BY_WEIGHT_FIX && PrtItemStr.l_PluType <= PLU_BY_PCS_FIX)
	{
		PrtItemStr.l_PluType -= 3;
	}
#endif
#ifdef USE_USA_FIXEDWEIGHT_UNIT_OZ
	if(PrtItemStr.l_PluType == PLU_BY_WEIGHT){ //count(non-weight) type
		decimal=get_decimal_pt(4, PrtItemStr.plu_addr);
		plu_get_field(plu_addr-1,PTYPE_FIXED_WEIGHT,(INT8U *)&PrtItemStr.l_FixedWeight,&sz,sizeof(PrtItemStr.l_FixedWeight));
		PrtItemStr.l_FixedWeight = OztoLb(PrtItemStr.l_FixedWeight, 16, decimal);
	}
	else plu_get_field(plu_addr-1,PTYPE_FIXED_WEIGHT,(INT8U *)&PrtItemStr.l_FixedWeight,&sz,sizeof(PrtItemStr.l_FixedWeight));
#else
	plu_get_field(plu_addr-1, PTYPE_FIXED_WEIGHT, (INT8U *)&PrtItemStr.l_FixedWeight, (INT16S *)&sz,sizeof(PrtItemStr.l_FixedWeight));
#endif
	plu_get_field(plu_addr-1, PTYPE_TAX_NO, (INT8U *)&temp, (INT16S *)&sz,sizeof(temp));
	PrtItemStr.l_Price.TaxNumber = temp;
#ifdef USE_TAX_RATE    
	GetTaxData(&PrtItemStr.l_Price, PrtItemStr.l_Price.TaxNumber,0);
#endif    
	temp = get_global_bparam(GLOBAL_SALE_SETUP_FLAG2);
	if(temp&0x02) {
		plu_get_field(plu_addr-1, PTYPE_TARE_NO, (INT8U *)&tare_id, (INT16S *)&sz,sizeof(tare_id));
		ret = get_tare_fromid((INT16U)tare_id, &temp, &PrtItemStr.l_TareWeight, &wunit);
		if(!ret || wunit != CurPluWeightUnit) PrtItemStr.l_TareWeight = 0L;
	} else {
		plu_get_field(plu_addr-1, PTYPE_TARE_VALUE, (INT8U *)&PrtItemStr.l_TareWeight, (INT16S *)&sz,sizeof(PrtItemStr.l_TareWeight));
	}
	PrtItemStr.l_PluTareWeight = PrtItemStr.l_TareWeight;	
}

void Prt_SetPrintData(INT8U label_mode, RTC_STRUCT *time_str, INT16U dept_no, INT32U plu_no)	// LabelData -> PrtXxxxStr
{
	INT16U v16_1;
	INT32U addr;
#ifdef USE_TRACEABILITY
	TRACEABILITY_STRUCT traceability_str;
#endif
	INT8U  v8_1;//, i;
	static long prev_tran_no, prev_plu_tran_no;
	PREPACK_STATUS_TYPE PrepackStatus;

	if(PrtFirstLabel) {
		memset((INT8U*)&PrtItemStr, 0, sizeof(STRUCT_PRT_ITEM));
		memset((INT8U*)&PrtComStr, 0, sizeof(STRUCT_PRT_COMMON));
		memset((INT8U*)&label_set, 0, sizeof(STRUCT_LABEL_SET));

		// common //////////////
		PrtComStr.time.year  = time_str->year;
		PrtComStr.time.month = time_str->month;
		PrtComStr.time.date  = time_str->date;
		PrtComStr.time.hour  = time_str->hour;
		PrtComStr.time.min   = time_str->min;
		PrtComStr.time.sec   = time_str->sec;
		PrtComStr.time.day   = time_str->day;

		PrtComStr.shop_name_no = get_net_wparam(NETWORK_STORE_ID);	// Shop name number
		if(PrtComStr.shop_name_no > get_base_wparam(FLASH_NUMBER_SHOP)) PrtComStr.shop_name_no = 0;

		if(label_mode >= TOTAL_LABEL) {
			PrtComStr.clerk_no    = PrtTotalStr.ClerkNum;
			PrtComStr.customer_no = PrtTotalStr.CustomerNo;
		} else {
			PrtComStr.clerk_no    = status_scale.clerkid; 	// 현재 Clerk Number
			PrtComStr.customer_no = 0;			// 현재 Customer Number
		}
		///////////////////////

		// item //////////////
		if (label_mode == ITEM_LABEL) {
			PrtItemStr.l_DeptNo = LabelData.DeptNo;
			PrtItemStr.l_PluNo  = LabelData.PluNo;
			plu_table_search((INT8U)PrtItemStr.l_DeptNo, PrtItemStr.l_PluNo,(INT16U*)&PrtItemStr.plu_addr,0);

			PrtItemStr.l_Price  = LabelData.Price;
			PrtItemStr.l_PluType = LabelData.PluType;
			PrtItemStr.l_PluTareWeight = LabelData.PluTareWeight;
			PrtItemStr.l_KeyTareWeight = LabelData.KeyTareWeight;
			PrtItemStr.l_WeightTareWeight = LabelData.WeightTareWeight;
			PrtItemStr.l_TareWeight	= LabelData.TareWeight;
			PrtItemStr.l_FixedWeight = LabelData.FixedWeight;
			PrtItemStr.CPrice = CurrencyPrice;
		} else if(label_mode == TEST_LABEL) {
			PrtItemStr.l_DeptNo = dept_no;
			PrtItemStr.l_PluNo  = plu_no;
			plu_table_search((INT8U)PrtItemStr.l_DeptNo, PrtItemStr.l_PluNo,(INT16U *)&PrtItemStr.plu_addr,0);
			prtfield_set_variable_testlabel(PrtItemStr.plu_addr);
		} else if(OneItemPrepack) {
			get_PrepackStatus(&PrepackStatus);
			PrtItemStr.l_DeptNo = PrepackStatus.CurDeptNo;
			PrtItemStr.l_PluNo  = PrepackStatus.CurPluNo;
			plu_table_search((INT8U)PrtItemStr.l_DeptNo, PrtItemStr.l_PluNo,(INT16U*)&PrtItemStr.plu_addr,0);
		}

		if(label_mode == ITEM_LABEL || label_mode == TEST_LABEL || OneItemPrepack) {
			PrtItemStr.bonus_point = 0; 

			switch(PrtItemStr.l_PluNo) {
				case MISC_PLU_WEIGHT :
				case MISC_PLU_PCS    :
				case MISC_PLU_COUNT  :
				case 0:
					break;
				default :
					plu_get_field(PrtItemStr.plu_addr-1, PTYPE_TRACE_NO, (INT8U *)&PrtItemStr.trace_no, (INT16S *)&v16_1,sizeof(PrtItemStr.trace_no));
					plu_get_field(PrtItemStr.plu_addr-1, PTYPE_NUTRITION_NO, (INT8U *)&PrtItemStr.nutri_no, (INT16S *)&v16_1,sizeof(PrtItemStr.nutri_no));
					plu_get_field(PrtItemStr.plu_addr-1, PTYPE_QTYUNIT_NO, (INT8U *)&PrtItemStr.cur_pcssym_no, (INT16S *)&v16_1,sizeof(PrtItemStr.cur_pcssym_no));
					plu_get_field(PrtItemStr.plu_addr-1, PTYPE_ORIGIN_NO, (INT8U *)&PrtItemStr.origin, (INT16S *)&v16_1, sizeof(PrtItemStr.origin));

					plu_get_field(PrtItemStr.plu_addr-1, PTYPE_SELLBY_DATE, (INT8U *)&PrtItemStr.sellbydate, (INT16S *)&v16_1,sizeof(PrtItemStr.sellbydate));	// sell by date
					plu_get_field(PrtItemStr.plu_addr-1, PTYPE_SELLBY_TIME, (INT8U *)&PrtItemStr.sellbytime, (INT16S *)&v16_1,sizeof(PrtItemStr.sellbytime));	// sell by time
					plu_get_field(PrtItemStr.plu_addr-1, PTYPE_PACKCED_DATE, (INT8U *)&PrtItemStr.packedondate, (INT16S *)&v16_1,sizeof(PrtItemStr.packedondate));	// packed on date
					plu_get_field(PrtItemStr.plu_addr-1, PTYPE_PACKCED_TIME, (INT8U *)&PrtItemStr.packedontime, (INT16S *)&v16_1,sizeof(PrtItemStr.packedontime));	// packed on time
					plu_get_field(PrtItemStr.plu_addr-1, PTYPE_SELLBY_DATE_FLAG, (INT8U *)&PrtItemStr.flagsellbydate, (INT16S *)&v16_1,sizeof(PrtItemStr.flagsellbydate));	// sell by date flag
					plu_get_field(PrtItemStr.plu_addr-1, PTYPE_SELLBY_TIME_FLAG, (INT8U *)&PrtItemStr.flagsellbytime, (INT16S *)&v16_1,sizeof(PrtItemStr.flagsellbytime));	// sell by time flag
					plu_get_field(PrtItemStr.plu_addr-1, PTYPE_PACKCED_DATE_FLAG, (INT8U *)&PrtItemStr.flagpackedondate, (INT16S *)&v16_1,sizeof(PrtItemStr.flagpackedondate));	// packed on date flag
					plu_get_field(PrtItemStr.plu_addr-1, PTYPE_PACKCED_TIME_FLAG, (INT8U *)&PrtItemStr.flagpackedontime, (INT16S *)&v16_1,sizeof(PrtItemStr.flagpackedontime));	// packed on time flag

					plu_get_field(PrtItemStr.plu_addr-1, PTYPE_PRODUCED_DATE, (INT8U *)&PrtItemStr.produceddate, (INT16S *)&v16_1,sizeof(PrtItemStr.produceddate));	// produced date
					plu_get_field(PrtItemStr.plu_addr-1, PTYPE_COOKBY_DATE, (INT8U *)&PrtItemStr.cookbydate, (INT16S *)&v16_1,sizeof(PrtItemStr.cookbydate));	// cook by date

					plu_get_field(PrtItemStr.plu_addr-1, PTYPE_UNIT_WEIGHT, (INT8U *)&PrtItemStr.unitweight_id, (INT16S *)&v16_1,sizeof(PrtItemStr.unitweight_id));	// unit weight

#ifdef USE_DISCOUNT_MEMBERSHIP
					plu_get_field(PrtItemStr.plu_addr-1, PTYPE_DISCOUNT_TYPE, (INT8U *)&PrtItemStr.disc_type, (INT16S *)&v16_1, sizeof(PrtItemStr.disc_type));	// 할인 유형
#endif
					break;
			}
			PrtItemStr.fixed_price = Operation.fixedPrice;
#ifdef USE_GSMART_PLU
			if (PrtItemStr.flagsellbydate == 0) 
			{
				PrtItemStr.flagsellbydate = get_base_bparam(FLASH_SELL_BY_DATE_FLAG);
			}
			if (PrtItemStr.flagsellbytime == 0) 
			{
				PrtItemStr.flagsellbytime = get_base_bparam(FLASH_SELL_BY_TIME_FLAG);
			}
			if (PrtItemStr.flagpackedondate == 0)		//1120 setting
			{
				PrtItemStr.flagpackedondate = get_base_bparam(FLASH_PACKED_DATE_FLAG);	//1550 setting
			}
			if (PrtItemStr.flagpackedontime == 0) 
			{
				PrtItemStr.flagpackedontime = get_base_bparam(FLASH_PACKED_TIME_FLAG);
			}
#endif
#ifdef USE_TOPMART_DEFAULT //sewon 20170126
			if (PackdatePrintFlag== 1)		//1120 setting
			{
				PrtItemStr.flagpackedondate = 2;	//강제로 인쇄 
			}
#endif
#ifdef USE_KOR_TRACE
			if(KorTrace_Flag.flagPrint){  //SG060605
				if(LabelData.SellbyDate>=0 && LabelData.SellbyDate<=9999){
					PrtItemStr.sellbydate = LabelData.SellbyDate;
					LabelData.SellbyDate = 0;  //1회성 SellbyDate 임
				}
			}
#endif
			if(PrtItemStr.origin) {
				addr =  DFLASH_BASE + FLASH_ORIGIN_AREA;
				addr =  addr + ORIGIN_STRUCT_SIZE * (INT32U)(PrtItemStr.origin-1);	// code-1 (?)
				v16_1 = Flash_wread(addr);
				if(PrtItemStr.origin != v16_1) PrtItemStr.origin = 0;
			}

#ifdef USE_TRACEABILITY //gm 090407, memory 충돌 방지
			// Traceability
			if(PrtItemStr.trace_no) {
				addr = DFLASH_BASE + FLASH_TRACEABILITY_AREA;
				addr = addr + TRACEABILITY_STRUCT_SIZE * (INT32U)(PrtItemStr.trace_no - 1);
				Flash_sread(addr, (HUGEDATA INT8U *)&traceability_str, TRACEABILITY_STRUCT_SIZE);
				if(traceability_str.code != PrtItemStr.trace_no) PrtItemStr.trace_no = 0;
				else {
					PrtItemStr.born_country_no = traceability_str.idata[0];
					PrtItemStr.bred_country_no = traceability_str.idata[1];
					PrtItemStr.slaught_no	   = traceability_str.idata[2];
					PrtItemStr.cutting_no	   = traceability_str.idata[3];
				}
			}
#endif
			/////////////////////
			// setting
			v8_1 = get_global_bparam(GLOBAL_LABEL_MARKDOWNFLAG);

			if(PrtItemStr.l_Price.TotalPrice[PRICE_SAVED] && (v8_1&0x01)){
				label_set.en_mark_down_total = 1;
			}
			if((PrtItemStr.l_Price.UnitPrice[PRICE_SAVED] || PrtItemStr.l_Price.UnitCount[PRICE_SAVED]) && (v8_1&0x02)) {
				label_set.en_mark_down_unit = 1;
			} 
			if(PrtItemStr.l_Price.Weight[PRICE_SAVED] && (v8_1&0x04)) {
				label_set.en_mark_down_weight = 1;
			} 
			if(PrtItemStr.l_Price.Qty[PRICE_SAVED] && (v8_1&0x08)) {
				label_set.en_mark_down_qty = 1;
			} 
//PLU type에 관계없이 parameter 581:Y 이면, unit price & total price 출력 안함
			if(GlbOper.weightZeroPricePrint && label_mode != TEST_LABEL) 
			{
				if(PrtItemStr.l_Price.UnitPrice[PRICE_RESULT] == 0) label_set.flagZeroUnitPrice = 1;
			}
			v8_1 = get_global_bparam(GLOBAL_LABEL_FLAG);
			if(v8_1&0x01) label_set.flagSymInQty = 1; 
			if(v8_1&0x02) label_set.flagPcsInUnit = 1;

			if ((status_scale.restrict&FUNC_ORG_TBL)){
				v8_1  = get_global_bparam(GLOBAL_PRINT_OPER_FLAG);
				label_set.origin_prttype = (v8_1>>4) & 0x03;
			} else {
				label_set.origin_prttype = 0;
			}
		}

		if(label_mode == TOTAL_LABEL || label_mode == GTOTAL_LABEL) {
			if(PrtTotalStr.PayPrice) label_set.en_print_payment = 1;
		}
	}

	
	if(label_mode == ITEM_LABEL) {
		if(LinkedPrintFlag) {
			PrtItemStr.tran_no = prev_tran_no;		// Tran. No. 증가 안시킴
			PrtItemStr.plu_tran_no = prev_plu_tran_no;	// PLU Tran. No. 증가 안시킴
		} else {
			PrtItemStr.tran_no = GetTransactionCounter();//CounterTransaction;
			PrtItemStr.plu_tran_no = ReadPluTransactionCount(PrtItemStr.l_DeptNo, PrtItemStr.l_PluNo);
		}
		prev_tran_no = PrtItemStr.tran_no;
		prev_plu_tran_no = PrtItemStr.plu_tran_no;
	}
	
}

#ifdef USE_CONTINUOUS_LABEL_WITH_INGREDIENT
INT8U label_adjust_form(INT16U id , INT8U offset_flag)
{
	INT32U saddr,addr,label_point;
	INT16U label_address,w,label_size;
	HUGEDATA char *baddr;
	INT16S	  i,cnt;
	INT16U height, width;

// Interpreting Label form & find Ingredient field
	if (PrtStruct.Mode == RECEIPT) return FALSE;
	if (id==0) id=1;
	if (id<=MAX_LABEL_ROM_NO) {
		addr=label_rom_addr(id);
		if (addr == 0) return FALSE;
		addr+= PFLASH_DATA_BASE;
		addr += 2;	// width(2)
		memcpy(&height, (HUGEDATA char *)addr, 2);
		addr += 2;	// height(2)
		addr += 16; // name(16)
		baddr= (HUGEDATA char *)addr;
		memcpy(&label_size,baddr,2);
		addr+=2;
		label_point=addr;
		goto FORM_COMMON;
	}
	if ((id==0) || (id==0xffff)) return FALSE;
// Download Label Format
	saddr	= DFLASH_BASE;
	saddr += FLASH_LABEL_AREA;
	addr	= saddr;
	label_address = 99;
	for (i=0; i<20; i++) {
		w=Flash_wread(addr);
		if (w==id) {
			label_address=i;
			label_size = Flash_wread(addr+22);
			break;
		}
		addr += LABEL_INFO_SIZE;
	}
	if (label_address==99) return FALSE;
	if (label_size>LABEL_MAX_SIZE) return FALSE;

	label_point = label_address;
	label_point*= LABEL_MAX_SIZE;
	label_point+= LABEL_IMAGE;
	label_point+= saddr;
	Flash_flush_nvbuffer();
	addr += 2;
//	width = Flash_wread(addr);
	addr += 2;
	height = Flash_wread(addr);
FORM_COMMON:
	width  = PrtStruct.LabelWidth;
	height *= 8;
	ingredient_height_adjust_flag=offset_flag;
	PrtSetLabelSize(width, height+ingredient_height_offset);

	PutDataRxBuf(&CommBufPrt,(HUGEDATA char *)label_point,label_size);
	cnt=0;
	
	while(CheckRxBuf(&CommBufPrt)) {
		CASInterpreter(&CommBufPrt);
		cnt++;
		if (cnt>LABEL_MAX_SIZE){
			ingredient_height_adjust_flag=0;
			return FALSE;
		}
	}
	ingredient_height_adjust_flag=0;
	return TRUE;
}

void  ingredient_text_height_size(HUGEDATA char *ch_ptr) 
{
	short cx,cy,crx,cry,bits_sum,i;
	short by[100],str_left_margin[100],str_right_margin[100],whiteSpace,whitespace_V;
	char rot,wbits1,hbits1,wbits2,hbits2,wbits,hbits,nwbits,nhbits,justification,sbits;
	char line_idx,line_end,loop,spbytes,rx,ry;
	CWD		chw;	
	HUGEDATA char *p,*p_max;/////htab
	char wordwrap, p_sum, /*baseline_height,*/ctrl_ch,max_line;//HYP 20050224 erase baseline_height
	short MbaselineH,MbaselineL,baselineL,baselineH,baseline1,baseline2,line_height;
	short sx,sy;
	HUGEDATA INT8U *fontw_ptr;
	char variable_init_flag, magx_backup;
	char ch_rot_backup;/////////rot
	INT8U	sphbits;
	short	ncx;////////htab
	INT8U	htab_pos_count;	////////htab
	short	max_cx;////////htab
	BOOLEAN updata_mag;
	INT8U id1_backup, id2_backup;
	short backup_x,backup_y;
	backup_x = PrtStruct.x;
	backup_y = PrtStruct.y;

	variable_init_flag = 1;
	loop = TRUE;	
	magx_backup = StrFieldStruct[FIELD_ID_INGREDIENT_STR].Magx;	 //for auto shrink
	ch_rot_backup = StrFieldStruct[FIELD_ID_INGREDIENT_STR].ChRot;  /////////rot
	id1_backup = StrFieldStruct[FIELD_ID_INGREDIENT_STR].Id1;
	id2_backup = StrFieldStruct[FIELD_ID_INGREDIENT_STR].Id2;
	FontLoadFont1(StrFieldStruct[FIELD_ID_INGREDIENT_STR].Id1);
	FontLoadFont2(StrFieldStruct[FIELD_ID_INGREDIENT_STR].Id2);

	if(*ch_ptr == '\0') return;
	while (loop) {
		if(variable_init_flag == 1) {
			variable_init_flag = 0;

			sx = PrtStruct.x;
			sy = PrtStruct.y;
			spbytes = StrFieldStruct[FIELD_ID_INGREDIENT_STR].InterChSp>>3;
			if(StrFieldStruct[FIELD_ID_INGREDIENT_STR].InterChSp&0x07) spbytes++;
			p = ch_ptr;
			wordwrap = StrFieldStruct[FIELD_ID_INGREDIENT_STR].WordWrap;
			cx = 0;								// rel. character pos x
			cy = 0;								// rel. character pos y
			justification = StrFieldStruct[FIELD_ID_INGREDIENT_STR].Justification;
			if(StrFieldStruct[FIELD_ID_INGREDIENT_STR].StrMirx) { //mirror
				if(justification == RIGHT) {
					justification = LEFT;
				} else if(justification == LEFT) {
					justification = RIGHT;
				}
			}
//////////////////////////////////load font1,2 start
			rx = StrFieldStruct[FIELD_ID_INGREDIENT_STR].Magx;
			ry = StrFieldStruct[FIELD_ID_INGREDIENT_STR].Magy;	
			wbits2 = FontStruct.Wbits2*rx;	  	// mag. width bits of 2 bytes font
			hbits2 = FontStruct.Hbits2*ry;		// mag. height bits of 2 bytes font
			wbits1 = FontStruct.Wbits1*rx;		// mag. width bits of 1 bytes font
			hbits1 = FontStruct.Hbits1*ry;		// mag. height bits of 1 bytes font
			wbits = wbits1;		// mag. width inc. bits of 1 str line
			hbits = hbits1;		// mag. height inc. bits of 1 str line
			nwbits = wbits1;		// mag. width inc. bits of 1 str line
			nhbits = hbits1;		// mag. height inc. bits of 1 str line
			baseline2 = FontStruct.Baseline2*ry;
			baseline1 = FontStruct.Baseline1*ry;
			if(FontStruct.FixedWidthFlag1 == 1 || StrFieldStruct[FIELD_ID_INGREDIENT_STR].UseFixedWidth == 1) {
				sbits = FontStruct.FixedWbits1 * rx;
			} else {
				fontw_ptr = FontStruct.PtrWidth1 + ((INT8U)0x20-FontStruct.FirstCode1);
				sbits = *(fontw_ptr) * rx;
			}
////////////////////////////
			str_left_margin[0] = 0;
			str_right_margin[0]=0;
			line_idx = 0;
			htab_pos_count = 0;	 ////////htab
			max_cx = 0;	 ////////htab
			ncx = 0;  ///////htab
			line_end = FALSE;
			by[0] = 0;
			baselineL = 0;
			baselineH = 0;

			p_sum = 0;
			bits_sum = 0;
			MbaselineH = 0;
			MbaselineL = 0;
			line_height = 0;
		}	/*  Init end  */

 //justification and baseline check
		ctrl_ch = OFF;
		chw.byte.left = *p;
		chw.byte.right = *(p+1);//HYP 20040128
		if(chw.byte.left == _CR) {
			if(*(p+1) == LF) {
				p++;
			}
			chw.byte.left = LF;
		}
		//  two byte font
		if (StrFieldStruct[FIELD_ID_INGREDIENT_STR].StrType == TWO_BYTE_FONT_STR && chw.byte.left > 0x80) 
		{
			p = p + 2;
			wbits = wbits2;		
			hbits = hbits2;
			baselineL = baseline2;
			baselineH = hbits2 - baseline2;
			p_sum = p_sum + 2;
		} 
		else  //  one byte font
		{
			p++;
			if (chw.byte.left == FONT_CON_CH) {
				ctrl_ch = ON;
				updata_mag = OFF;
				switch (*p) {
					///////////rot
					case FONT_CON_DEG:
						p++;
						switch(*p) {
							case '0':
								StrFieldStruct[FIELD_ID_INGREDIENT_STR].ChRot = DEG_0;
								break;
							case '1':
								StrFieldStruct[FIELD_ID_INGREDIENT_STR].ChRot = DEG_90;
								break;
							case '2':
								StrFieldStruct[FIELD_ID_INGREDIENT_STR].ChRot = DEG_180;
								break;
							case '3':
								StrFieldStruct[FIELD_ID_INGREDIENT_STR].ChRot = DEG_270;
								break;
						}
						break;
					case FONT_CON_MAG_X:
						p++;
						switch(*p) {
							case '1':
								rx = 1;
								break;
							case '2':
								rx = 2;
								break;
							case '3':
								rx = 3;
								break;
							case '4':
								rx = 4;
								break;
							case '5':
								rx = 5;
								break;
						}
						updata_mag = ON;
						break;
					case FONT_CON_MAG_Y:
						p++;
						switch(*p) {
							case '1':
								ry = 1;
								break;
							case '2':
								ry = 2;
								break;
							case '3':
								ry = 3;
								break;
							case '4':
								ry = 4;
								break;
							case '5':
								ry = 5;
								break;
						}
						updata_mag = ON;
						break;
					case FONT_CON_INC_MAG:
						if(rx < 5) {
							rx++;
						}
						if(ry < 5) {
							ry++;
						}
						updata_mag = ON;
						break;
					case FONT_CON_DEC_MAG:
						if(rx > 1) {
							rx--;
						}
						if(ry > 1) {
							ry--;
						}
						updata_mag = ON;
						break;
					case FONT_CON_FONT2:
						p++;
						FontLoadFont2(*p - '0');
						updata_mag = ON;
						break;
					case FONT_CON_FONT1:
						p++;
						FontLoadFont1(*p - '0');
						updata_mag = ON;
						break;
				}
				if(updata_mag == ON) {
					baseline2 = FontStruct.Baseline2*ry;
					baseline1 = FontStruct.Baseline1*ry;
					wbits2 = FontStruct.Wbits2*rx;	  	// mag. width bits of 2 bytes font
					hbits2 = FontStruct.Hbits2*ry;		// mag. height bits of 2 bytes font
					wbits1 = FontStruct.Wbits1*rx;		// mag. width bits of 1 bytes font
					hbits1 = FontStruct.Hbits1*ry;		// mag. height bits of 1 bytes font
					fontw_ptr = FontStruct.PtrWidth1 + ((INT8U)0x20-FontStruct.FirstCode1);
				}
				p++;
			}
			if((INT8U)chw.byte.left < 0x20) {
				//HYP 20050224
				//wbits1 = 0;
				if (StrFieldStruct[FIELD_ID_INGREDIENT_STR].ChRot&MASK_DEG_90_270) {
					hbits1 = 0;
				} else {
					wbits1 = 0;
				}
				ctrl_ch = ON;  
			} else {
				if(FontStruct.FixedWidthFlag1 == 1 || StrFieldStruct[FIELD_ID_INGREDIENT_STR].UseFixedWidth == 1) {
					wbits1 = FontStruct.FixedWbits1 * rx;
				} else {
					fontw_ptr = FontStruct.PtrWidth1 + ((INT8U)chw.byte.left-FontStruct.FirstCode1);
					wbits1 = *(fontw_ptr) * rx;
				}
				//HYP 20050224
				hbits1= FontStruct.Hbits1*ry;
////////////////////////////
			}
			wbits = wbits1;		
			hbits = hbits1;
			if (StrFieldStruct[FIELD_ID_INGREDIENT_STR].ChRot&MASK_DEG_90_270) {
				baselineH = wbits1;
				baselineL = 0;
			} else {
				baselineH = hbits1 - baseline1;
				baselineL = baseline1;
			}
			if(chw.byte.left != 0x20) {
				if ((ctrl_ch == ON) || (chw.byte.left == FONT_CON_CH)) {
					p_sum = 0; //FONT_CON_CH < 0x20
				} else {
					p_sum = p_sum + 1;
				}
			} else {
				if(FontStruct.FixedWidthFlag1 == 1 || StrFieldStruct[FIELD_ID_INGREDIENT_STR].UseFixedWidth == 1) {
					sbits = FontStruct.FixedWbits1 * rx;
				} else {
					sbits = *(fontw_ptr) * rx;/////////////////////////
				}
////////////////////////////
				p_sum = 0;
			}
		}
		if (StrFieldStruct[FIELD_ID_INGREDIENT_STR].StrType == TWO_BYTE_FONT_STR && (INT8U)(*p) > 0x80) {	//you must cast!!!
			nwbits = wbits2;		
			nhbits = hbits2;
		} else {
			if((INT8U)*p < 0x20) {//FONT_CON_CH < 0x20
				nwbits = 0;
				nhbits = 0; ///////rot
			} else {
				if(FontStruct.FixedWidthFlag1 == 1 || StrFieldStruct[FIELD_ID_INGREDIENT_STR].UseFixedWidth == 1) {
					nwbits = FontStruct.FixedWbits1 * rx;
				} else {
					fontw_ptr = FontStruct.PtrWidth1 + ((INT8U)*p-FontStruct.FirstCode1);
					nwbits = *(fontw_ptr) * rx;
				}
////////////////////////////
				nhbits = FontStruct.Hbits1*ry;
			}
		}
		if((StrFieldStruct[FIELD_ID_INGREDIENT_STR].ChRot&MASK_DEG_90_270) && (chw.byte.left != 0x20)) {
			if (MbaselineH < baselineH) {
				MbaselineH = baselineH;
			}
			if (MbaselineL < baselineL) {
				MbaselineL = baselineL;
			}			
		   	if ((chw.byte.left != LF) && (chw.byte.left != '\0')) {
			   	if (((cx != 0) || (chw.byte.left != 0x20)) && (ctrl_ch == OFF)) {
			   		cx = cx + hbits; 
					cx = cx + StrFieldStruct[FIELD_ID_INGREDIENT_STR].InterChSp;
				}
				if (chw.byte.left == 0x20 || ctrl_ch == ON) {
					bits_sum = 0;
				} else {
					bits_sum = bits_sum + hbits + StrFieldStruct[FIELD_ID_INGREDIENT_STR].InterChSp;
				}
			}
			if((cx+nhbits > StrFieldStruct[FIELD_ID_INGREDIENT_STR].StrWidth) || (ncx > StrFieldStruct[FIELD_ID_INGREDIENT_STR].StrWidth) || (chw.byte.left == LF) || (chw.byte.left == '\0')) {/////htab
				if(PrtStruct.Mode != RECEIPT) {/////////htab
					if (wordwrap == ON) {
						if((chw.byte.left != 0x20) && (*p != 0x20) && (*p != HT) && (chw.byte.left != LF) && (chw.byte.left != '\0') && (cx != bits_sum)) {////htab
							cx = cx - bits_sum ;
							p = p - p_sum;
							if (cx != 0) {
								if (*(p-1) == 0x20){   ///htab
									cx = cx - sbits - 2*StrFieldStruct[FIELD_ID_INGREDIENT_STR].InterChSp;
									p--;
								} else {
									cx = cx - StrFieldStruct[FIELD_ID_INGREDIENT_STR].InterChSp;
								}
							}
					 	} else {
							if (cx != 0) {
								if (chw.byte.left == 0x20){
									cx = cx - sbits - 2*StrFieldStruct[FIELD_ID_INGREDIENT_STR].InterChSp;
									p--;
								} else { 
									cx = cx - StrFieldStruct[FIELD_ID_INGREDIENT_STR].InterChSp;
								}
							}
						}
					} else {
						if (cx != 0) {
							if (chw.byte.left == 0x20) {
								cx = cx - sbits - 2*StrFieldStruct[FIELD_ID_INGREDIENT_STR].InterChSp;
								p--;
							} else {
								cx = cx - StrFieldStruct[FIELD_ID_INGREDIENT_STR].InterChSp;
							}
						}
					}
					p_sum = 0;
					bits_sum = 0;
					by[line_idx] = cy + MbaselineH;
					line_height = MbaselineH + MbaselineL;
					cy = cy + line_height;
					max_line = line_idx;
					StrFieldStruct[FIELD_ID_INGREDIENT_STR].StrHeight= cy;
					if(cy > StrFieldStruct[FIELD_ID_INGREDIENT_STR].StrHeight) {
						PrtStruct.StrAreaOverFlag = TRUE;//HYP 20040903	not applied to PC program (PrtRender)
						whiteSpace = 0;
						loop = FALSE;
					} else {
						whiteSpace = StrFieldStruct[FIELD_ID_INGREDIENT_STR].StrWidth - cx;
						if((*p == '\0') || (chw.byte.left == '\0')) {
							loop=FALSE;
							max_line++;
						}
					}
					cy = cy + StrFieldStruct[FIELD_ID_INGREDIENT_STR].LineSp;
					MbaselineH = 0;
					MbaselineL = 0;
					cx = 0;
				}
				line_end=TRUE;
			}
		} else {
			if (MbaselineH < baselineH) {
				MbaselineH = baselineH;
			}
			if (MbaselineL < baselineL) {
				MbaselineL = baselineL;
			}
			if ((chw.byte.left != LF) && (chw.byte.left != '\0')) {
				if (((cx != 0) || (chw.byte.left != 0x20)) && (ctrl_ch == OFF)) {
					cx = cx + wbits; 
					cx = cx + StrFieldStruct[FIELD_ID_INGREDIENT_STR].InterChSp;
				}
				if (chw.byte.left == 0x20 || ctrl_ch == ON) {
					bits_sum = 0;
				} else {
					bits_sum = bits_sum + wbits + StrFieldStruct[FIELD_ID_INGREDIENT_STR].InterChSp;
				}
			}
			if((cx+nwbits > StrFieldStruct[FIELD_ID_INGREDIENT_STR].StrWidth) || (ncx > StrFieldStruct[FIELD_ID_INGREDIENT_STR].StrWidth) || (chw.byte.left == LF) || (chw.byte.left == '\0')) {/////htab
				if (wordwrap == ON) {
					if((chw.byte.left != 0x20) && (*p != 0x20) && (*p != HT) && (chw.byte.left != LF) && (chw.byte.left != '\0') && (cx != bits_sum)) {///htab
						cx = cx - bits_sum ;
						p = p - p_sum;				   
						if (cx != 0) {
							if (*(p-1) == 0x20){
								cx = cx - sbits - 2*StrFieldStruct[FIELD_ID_INGREDIENT_STR].InterChSp;
								p--;
							} else {
								cx = cx - StrFieldStruct[FIELD_ID_INGREDIENT_STR].InterChSp;
							}
						}
				 	} else {
						if (cx != 0) {
							if (chw.byte.left == 0x20){
								cx = cx - sbits - 2*StrFieldStruct[FIELD_ID_INGREDIENT_STR].InterChSp;
								p--;
							} else { 
								cx = cx - StrFieldStruct[FIELD_ID_INGREDIENT_STR].InterChSp;
							}
						}
					}
				} else {
					if (cx != 0) {
						if (chw.byte.left == 0x20) {
							cx = cx - sbits - 2*StrFieldStruct[FIELD_ID_INGREDIENT_STR].InterChSp;
							p--;
						} else { 
							cx = cx - StrFieldStruct[FIELD_ID_INGREDIENT_STR].InterChSp;
						}
					}
				}
				p_sum = 0;
				bits_sum = 0;
				by[line_idx] = cy + MbaselineH;
				line_height = MbaselineH + MbaselineL;
				cy = cy + line_height;
				max_line = line_idx;
				StrFieldStruct[FIELD_ID_INGREDIENT_STR].StrHeight = cy;
				if(cy > StrFieldStruct[FIELD_ID_INGREDIENT_STR].StrHeight) {
					if(StrFieldStruct[FIELD_ID_INGREDIENT_STR].AutoShrinkx == ON) {	//auto shrink mode
						if(StrFieldStruct[FIELD_ID_INGREDIENT_STR].Magx != 1) {
							variable_init_flag = 1;		//for auto shrink
							loop = TRUE;
							StrFieldStruct[FIELD_ID_INGREDIENT_STR].Magx--;
						} else {
							PrtStruct.StrAreaOverFlag = TRUE;//HYP 20040903	not applied to PC program (PrtRender)
							whiteSpace = 0;
							loop = FALSE;
							cy = cy - StrFieldStruct[FIELD_ID_INGREDIENT_STR].LineSp - StrFieldStruct[FIELD_ID_INGREDIENT_STR].LineSp - line_height;//for vertical centering
					    }
					} else {
						PrtStruct.StrAreaOverFlag = TRUE;//HYP 20040903	not applied to PC program (PrtRender)
						whiteSpace = 0;
						loop = FALSE;
						cy = cy - StrFieldStruct[FIELD_ID_INGREDIENT_STR].LineSp - StrFieldStruct[FIELD_ID_INGREDIENT_STR].LineSp - line_height;//for vertical centering
					}
				} else {
					whiteSpace = StrFieldStruct[FIELD_ID_INGREDIENT_STR].StrWidth - cx;	  
					if((*p == '\0') || (chw.byte.left == '\0')){
						loop=FALSE;
						max_line++;
					}
				}
				cy = cy + StrFieldStruct[FIELD_ID_INGREDIENT_STR].LineSp;
				MbaselineH = 0;
				MbaselineL = 0;
				cx = 0;
				line_end=TRUE;
			}
		}
		if((StrFieldStruct[FIELD_ID_INGREDIENT_STR].VCentering == ON) && (loop==FALSE)) {	  //Vertical Centering ,change StrFieldStruct[FIELD_ID_INGREDIENT_STR].StrHeight
		whitespace_V = StrFieldStruct[FIELD_ID_INGREDIENT_STR].StrHeight - cy;
		if (whitespace_V > 0) {
			whitespace_V = 	whitespace_V>>1;
			switch(PrtStruct.Rot) {
				case DEG_0: 
					sy = sy + whitespace_V;
					StrFieldStruct[FIELD_ID_INGREDIENT_STR].StrHeight = cy;
					break;
				case DEG_90:
					sx = sx + whitespace_V;
					StrFieldStruct[FIELD_ID_INGREDIENT_STR].StrHeight = cy;
					break;
				case DEG_180: 
					sy = sy + whitespace_V;
					StrFieldStruct[FIELD_ID_INGREDIENT_STR].StrHeight = cy;
					break;
				case DEG_270:
					sx = sx + whitespace_V;
					StrFieldStruct[FIELD_ID_INGREDIENT_STR].StrHeight = cy;
					break;
				}
			}
		}
		if(line_end==TRUE) {
			line_end = FALSE;
			switch (justification) {		// apply justification control
				case CENTER:
					str_left_margin[line_idx] = whiteSpace / 2;
					str_right_margin[line_idx] = whiteSpace / 2;
					break;
				case RIGHT:
					//HYP 20040226///////////////
					//////////////////////////////
					str_left_margin[line_idx] = whiteSpace;
					str_right_margin[line_idx] = 0;
					break;
				case LEFT:
				default:	// to Left
					str_left_margin[line_idx] = 0;
					str_right_margin[line_idx] = whiteSpace;
					break;
			}
			line_idx++;
		}
	}
	ingredient_height_offset =cy;
	ingredient_y_pos = StrFieldStruct[FIELD_ID_INGREDIENT_STR].sy;
}

void prtfield_fine_ingredient_item(INT16U *ingredNo)
{
	INT16S fieldid;
	INT32U addr;
	INT8U select,v8_1,v8_2,v8_3;
	INT16U v16_1, v16_2, v16_3, v16_4, v16_5;
	INT16U t16_3, t16_4, t16_5;
	INT16U ingredSize;
	INT32U v32_1;
	INT8U sellbydate_check,prepack_type,time_print_type;
	char string_buf[128], string_buf1[128];
	INT8U wt_prt_type, DirectSetField;
	//INT8U weightunit, weightunit_new_id;
	char *addrp;
	//INT8U	chg_flag;
	INT8U dec_ch;
	char w_sign1[5], w_sign2[5];
	INT8U changeIndivString;
	INT8U flagDirectIngred;

	dec_ch = get_global_bparam(GLOBAL_DECIMAL_SIGN);
	if(PrtWeightUnit == WEIGHT_UNIT_KG || PrtWeightUnit == WEIGHT_UNIT_G) 
	{
		get_global_sparam(GLOBAL_WEIGHT_SYMBOL_1, (INT8U *)w_sign1, 4);
		get_global_sparam(GLOBAL_WEIGHT_SYMBOL_2, (INT8U *)w_sign2, 4);
	} 
	else 
	{
		get_global_sparam(GLOBAL_WEIGHT_LBSYMBOL_1, (INT8U *)w_sign1, 4);
		get_global_sparam(GLOBAL_WEIGHT_LBSYMBOL_2, (INT8U *)w_sign2, 4);
	}
	w_sign1[4] = 0;
	w_sign2[4] = 0;

	//price_dec     = get_global_bparam(GLOBAL_AREA_PRICE_DECIMALPOINT);
	//dec_ch        = get_global_bparam(GLOBAL_DECIMAL_SIGN);
	sellbydate_check= get_global_bparam(GLOBAL_SALE_SETUP_FLAG5)&0x01;
	prepack_type  = get_global_bparam(GLOBAL_SALE_SETUP_FLAG5);
	prepack_type>>=1;
	prepack_type&=0x03;
	//SG061218. PackOnTime, SellbyTime에 대한 인쇄 포맷 설정
	//0=12:45(time), 1=A,B,..(alphabet), 2=1,2,..(numeric) format
	time_print_type  = get_global_bparam(GLOBAL_SALE_SETUP_FLAG6); 
	time_print_type>>=5;
	time_print_type&=0x03;
	//chg_flag = get_global_bparam(GLOBAL_SALE_SETUP_FLAG8) & 0x40;	//parameter 723 
	wt_prt_type = 0;
	changeIndivString = get_global_bparam(GLOBAL_SALE_SETUP_FLAG12);
	changeIndivString &= 0x20;
//	for (fieldid=0; fieldid<FIELD_ID_N_SERVING_SIZE; fieldid++) {
	for (fieldid=0; fieldid<230; fieldid++) {
		string_buf[0] = 0;
		DirectSetField = OFF;
		select = prtsel_get_table(fieldid);
		if (StrFieldStruct[fieldid].ExistField && select) {
			switch (fieldid) {
				case FIELD_ID_INGREDIENT_STR2: 	// ingredient PAGE2
				case FIELD_ID_INGREDIENT_STR: 	// ingredient PAGE1
					DirectSetField = ON;		// Added by CJK 20060524
					flagDirectIngred = OFF;
					switch(PrtItemStr.l_PluNo) {
						case MISC_PLU_WEIGHT :
						case MISC_PLU_PCS    :
						case MISC_PLU_COUNT  :
						case 0:
							v16_1 = 0;
							break;
						default :
							v8_2=0;
							plu_get_field(PrtItemStr.plu_addr-1, PTYPE_INGREDIENT_TEXT, (INT8U *)&ingredNo[0], (INT16S *)&v16_2,sizeof(INT16U)*DIRECT_INGREDIENT_BLOCK_NUM);  //Direct Ingredient
							if(ingredNo[0])
							{
								v8_2=1;
								v16_1 = 1;
								flagDirectIngred = ON;
								break;
							}
							plu_get_field(PrtItemStr.plu_addr-1, PTYPE_INGREDIENT_NO, (INT8U *)&v16_1, (INT16S *)&v16_2,sizeof(v16_1));	 //Indirect Ingredient
							break;
					}
					if (v16_1) 
					{
						if(v8_2) //direct ingredient
						{
							break;
						}
						else	//indirect ingredient
						{
							addr =  ing_calc_addr_by_key(v16_1-1);
							v16_2 = Flash_wread(addr);
							if(v16_1 != v16_2) continue;
							addr += 2;
							ingredSize = INGREDIENT_STRUCT_SIZE-2;
						}

						if(Flash_check_nvbuffer_4_rd(addr, ingredSize) == 1) {
							addr = (INT32U)(FlashNVRAMStruct.buffer + (addr&FLASH_BLOCK_LADDR_MASK));
						}
						v8_1=0;
						addrp = (char *)addr;
						for (v16_2=0; v16_2<ingredSize; v16_2++) {
							if (addrp[v16_2]==0x0c) {
								v8_1 =1;
								v16_1=v16_2;
								break;
							}
							if (addrp[v16_2]==0x0) break;
						}
						if (fieldid==FIELD_ID_INGREDIENT_STR2) {
							if (v8_1==0) continue;		//JJNAG 20070612
							v16_1++;
							addrp+=(INT32U)v16_1;
							v16_2 = ingredSize - v16_1;
						}
					} 
					else 
					{
						continue;
					}
					break;
				default :
					continue;
			}
		}
	}
}

INT8U label_load_form(INT16U id)
{
	HUGEDATA char CommTxBuf_ingredient_Memory[LABEL_MAX_SIZE];
	HUGEDATA char CommRxBuf_ingredient_Memory[LABEL_MAX_SIZE];	
	HUGEDATA COMM_BUF label_str;

	INT32U saddr,addr,label_point;
	INT16U label_address,w,label_size;
	HUGEDATA char *baddr;
	INT16S	  i,cnt;
	INT16U height, width;

	(label_str).Txing = 0;
	(label_str).Rxing = 0;
	(label_str).TxBufOutPtr = 0;
	(label_str).TxBufInPtr = (label_str).TxBufOutPtr;
	(label_str).RxBufOutPtr = 0;
	(label_str).RxBufInPtr = (label_str).RxBufOutPtr;
	(label_str).Type = 1;
	(label_str).TxBuf = CommTxBuf_ingredient_Memory;
	(label_str).RxBuf = CommRxBuf_ingredient_Memory;

	// Interpreting Label form & find Ingredient field

	if (PrtStruct.Mode == RECEIPT) return FALSE;
//	char str[32];
	if (id==0) id=1;
	if (id<=MAX_LABEL_ROM_NO) {
		addr=label_rom_addr(id);
		if (addr == 0) return FALSE;
		addr+= PFLASH_DATA_BASE;
		addr += 2;	// width(2)
		memcpy(&height, (HUGEDATA char *)addr, 2);
		addr += 2;	// height(2)
		addr += 16; // name(16)
		baddr= (HUGEDATA char *)addr;
		memcpy(&label_size,baddr,2);
		addr+=2;
		label_point=addr;
		goto FORM_COMMON;
	}
	if ((id==0) || (id==0xffff)) return FALSE;
// Download Label Format
	saddr	= DFLASH_BASE;
	saddr += FLASH_LABEL_AREA;
	addr	= saddr;
	label_address = 99;
	for (i=0; i<20; i++) {
		w=Flash_wread(addr);
		if (w==id) {
			label_address=i;
			label_size = Flash_wread(addr+22);
			break;
		}
		addr += LABEL_INFO_SIZE;
	}
	if (label_address==99) return FALSE;
	if (label_size>LABEL_MAX_SIZE) return FALSE;

	label_point = label_address;
	label_point*= LABEL_MAX_SIZE;
	label_point+= LABEL_IMAGE;
	label_point+= saddr;
	Flash_flush_nvbuffer();
	addr += 2;
//	width = Flash_wread(addr);
	addr += 2;
	height = Flash_wread(addr);
//	goto COMMON;
FORM_COMMON:
	width  = PrtStruct.LabelWidth;
//COMMON:
//	if (width>56) width=56;
	height *= 8;
	if(height != PrtStruct.LabelHeight) {
		PrtSetLabelSize(width, height);
	}
//	Find Ingredient Field & Get Y
	PutDataRxBuf(&label_str,(HUGEDATA char *)label_point,label_size-2); // 마지막 2byte는  TPH Memory 전달 부분
	cnt=0;	
	ingredient_height_adjust_flag=0;
	while(CheckRxBuf(&label_str)) {
		find_ingredient_y_interpreter(&label_str);
		cnt++;
		if (cnt>LABEL_MAX_SIZE) return FALSE;
	}
	if(StrFieldStruct[FIELD_ID_INGREDIENT_STR].ExistField==0){ // Don't exist Ingredient field
		return FALSE;
	}
	return TRUE;
}
#endif

INT8U Prt_PrintLabel(INT8U label_mode, INT8U linked)
{
	INT8U invert_mode, flag_tmp;
#ifdef USE_CONTINUOUS_LABEL_WITH_INGREDIENT	
	INT16U	directingredNo[DIRECT_INGREDIENT_BLOCK_NUM];	
	INT8U	ret_ingredient;
	INT8U	ingredient_string_buf[2101];
	INT16U ingredient_str_len;
#endif
#ifdef USE_TEST_PRT_RENDERING_TIME
    static INT16U tempCallCount = 0;
    INT16U tic = 0;
    INT16U toc = 0;
    
    sprintf(MsgBuf, "\r\n\r\n-------------\r\n#%d Print_Label_S\r\n", tempCallCount);
    MsgOut(MsgBuf);

    tic = SysTimer1_25ms;
#endif

	invert_mode = INVERT;
	if(label_mode == ITEM_LABEL || label_mode == TEST_LABEL) {
		if(OnOff(get_global_bparam(GLOBAL_PRINT_ADDUP_FLAG2)&0x08)) invert_mode = NON_INVERT;
		LabCmd_InvertPrt(invert_mode);

		PrtWeightUnit = CurPluWeightUnit;
		PrtWeightUnitId	= get_unitweight_id(status_scale.cur_weightunit, CurPluWeightUnit);
		if (label_mode == TEST_LABEL)
		{
			get_weightunit_and_id_fromid(PrtItemStr.unitweight_id, &PrtWeightUnitId, &PrtWeightUnit);
		}

		if(PrtFirstLabel) {
			if(!prt_load_labelform(label_mode, linked)) return OFF;
#ifdef USE_CONTINUOUS_LABEL_WITH_INGREDIENT
			// Load Label Form
			// Interpreting Label form & find Ingredient field			
			ret_ingredient = label_load_form(status_scale.cur_labelid);
			// Determing Ingredient size ( Y , Text size)
			prtfield_fine_ingredient_item(directingredNo);
			//Determine ingredient size
			ingredient_text_read(&directingredNo[0], ingredient_string_buf, 0, DEFAULT_INGREDIENT_TEXT_ALL_SIZE);
			ingredient_str_len= strlen(ingredient_string_buf);
			// Calculate Height size			
			ingredient_text_height_size(ingredient_string_buf);			
			if(ingredient_str_len==0)
				{
				ret_ingredient=0;
				ingredient_height_offset=0;
				}
			// label xy adjust
			label_adjust_form(status_scale.cur_labelid,ret_ingredient);
#endif
#ifdef USE_SETPRODUCT_SALE
			if(SetproductSaleMode && setproduct_PrtFlag)
			{	prtfield_generate_item();
				prtfield_generate_common_item();
				prtfield_generate_item();
			}
			else
			{
#endif
			prtfield_generate_common();
			prtfield_generate_common_item();
			prtfield_generate_item_caption();
			prtfield_generate_item();
#ifdef USE_SETPRODUCT_SALE
			}
#endif
#ifdef USE_TRACEABILITY //gm 090407, memory 충돌 방지
			prtfield_generate_traceability();
#endif
#ifdef USE_NUTRITION_FACT
#ifdef USE_ADDITIONAL_NUTRIENT
			prtfield_generate_nutri_Added();
#else
 			prtfield_generate_nutri();
#endif // USE_ADDITIONAL_NUTRIENT
#endif // USE_NUTRITION_FACT
#ifdef USE_AUSTRALIA_COOL_FUNCTION
			prtfield_generate_coolinfo();
#endif
 		}
#ifdef USE_DISCOUNT_MEMBERSHIP // 라벨 시리얼 할인률 인쇄 항목으로 사용 
#else
		prtfield_generate_labelserial();
#endif
		prtfield_generate_barcode_item();
#ifdef USE_CONTINUOUS_PRINT_DELAY_2000ms
		if(!PrtFirstLabel) Delay_100ms(20);
#endif
 	} else if(label_mode == TOTAL_LABEL || label_mode == GTOTAL_LABEL) {
		if(OnOff(get_global_bparam(GLOBAL_PRINT_ADDUP_FLAG2)&0x04)) invert_mode = NON_INVERT;
		LabCmd_InvertPrt(invert_mode);

		PrtWeightUnit = ADM_Status.UseUnit;	// CJK080219
		PrtWeightUnitId = 1;	// 기본 UnitWeight로 변환
		if(PrtWeightUnit == WEIGHT_UNIT_KG) {
			PrtWeightUnitId = (INT8U)get_global_wparam(GLOBAL_DEFAULTUNIT);
		}
		if(!prt_load_labelform(label_mode, linked)) return OFF;
		prtfield_generate_common();
		if(OneItemPrepack) {
			prtfield_generate_common_item();
		}
		prtfield_generate_total_caption();
		prtfield_generate_total(label_mode);
		prtfield_generate_barcode_total();
	}
	flag_tmp=get_global_bparam(GLOBAL_PRINT_OPER_FLAG2);
	
#if defined(COUNTRY_USA) || defined(COUNTRY_CANADA)
		if(OnOff(flag_tmp&0x10) && PrtDrvStruct.ReverseFeedLength && PrtPaperType == 2) {// all type KKH 110425 for test
#else
		if(OnOff(flag_tmp&0x10) && PrtDrvStruct.ReverseFeedLength) {// all type KKH 110425 for test
#endif
			LabCmd_ReverseFeed();
		}

//#ifdef USE_QR_ADDTOTAL
//		if(AddSaleFlag) 
//		{
//			AddSaleFlag = 0;
//			return OFF;
//		}
//#endif
#ifdef USE_TEST_PRT_RENDERING_TIME
    toc = SysTimer1_25ms;
    sprintf(MsgBuf, "#%d Print_Label_E\r\nTIC: %d, TOC: %d\r\nTotalTime(ms): %d\r\n-------------\r\n", tempCallCount++, tic, toc, toc-tic);
    MsgOut(MsgBuf);
#endif

	LabCmd_PrintStart(1);
	PrintListWaitEndWithoutComm();
#ifdef USE_CONTINUOUS_LABEL_WITH_INGREDIENT
	RcptCmd_CharLF(1);
	PrintListWaitEndWithoutComm(); 
#endif
	return ON;
}

INT8U Prt_PrintTicket(INT8U ticket_mode, INT16U clerkid, RTC_STRUCT *time_str, INT8U copy_ticket)
{
	INT8U result;
	INT16U cur_time;
	CLERK_STRUCT clerk_st;				   //insert by se-hyung for training key 20070627
	INT32U clerk_addr;				   //insert by se-hyung for training key 20070627
	char temp_string[35];				   //insert by se-hyung for training key 20070627
	INT8U training_prt;				   //insert by se-hyung for training key 20070627
#ifdef USE_TEST_PRT_RENDERING_TIME
    static INT16U tempCallCount = 0;
    INT16U tic = 0;
    INT16U toc = 0;
    INT16U totalToc = 0;
    tic = SysTimer1_25ms;
    sprintf(MsgBuf, "\r\n\r\n-------------\r\n#%d Print_Ticket_S\r\n", tempCallCount);
    MsgOut(MsgBuf);

    tic = SysTimer1_25ms;

#endif


	if(copy_ticket) {	// Added by CJK 20050725
		cur_time = SysTimer100ms;
		while((INT16U)(SysTimer100ms - cur_time) < WAIT_2ND_TICKET_100MS){
			if(KEY_Read()) {
				if(KeyDrv.CnvCode == KS_PRINT) {
					BuzOn(1);
					break;
				} else {
					BuzOn(2);
				}
			}
		}
	}

	result = ON;
	RcptCmd_ChangeMode(TO_BACKUP, ON);
	switch(ticket_mode) {
		case PRT_TOTAL_NORMAL_MODE :
#ifdef USE_COPYKEY			
			if(flagcopykey) // Notify Copy
			{
				Prt_copykey();
			}
#endif			
			if(GlbFloat.printList) {
				result = Prt_TotalList(0, clerkid, time_str, copy_ticket);
				if(result == OFF) break;
				if(PrtTotalStr.PaidTypePrice[PAY_CREDIT]) {
					RcptCmd_ChangeMode(TO_RECEIPT, OFF);
					RcptCmd_ReceiptFeed();
					PrintListWaitEnd();
					result = Prt_TotalList_Credit(0, clerkid, time_str);
					if(result == OFF) break;
				}
			}
			if(GlbFloat.printBarcode) {
				RcptCmd_ChangeMode(TO_RECEIPT, OFF);
				Prt_TotalBarcode();
			}

			training_prt = get_global_bparam(GLOBAL_SALE_SETUP_FLAG5)&0x80;
			if (training_prt)
			{
				clerk_addr = clerkid;				//insert by se-hyung for training key 20070627
				clerk_addr *= CLERK_STRUCT_SIZE;		//insert by se-hyung for training key 20070627
				clerk_addr += FLASH_CLERK_AREA;			//insert by se-hyung for training key 20070627
		   		get_base_sparam( clerk_addr, (INT8U *)&clerk_st, CLERK_STRUCT_SIZE);   //insert by se-hyung for training key 20070627	

				if ( clerk_st.training == 1)
				{
					RcptCmd_ChangeMode(TO_RECEIPT, OFF);				
					receipt_caption_read(RCPTCAP_ID_TKT_TRAINING, temp_string);
					RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_TITLE, TKT_LAY_N, 0, CENTER);
					RcptCmd_PrintMultiLine((char*)temp_string, sizeof(temp_string), 0);
					RcptCmd_Start();
					PrintListWaitEnd();					
				}
			}

			if(GlbFloat.printTotalLabel) {
				RcptCmd_ChangeMode(TO_NOGAPLABEL, OFF);
				Prt_SetPrintData(TOTAL_LABEL, time_str, 0, 0);
				Prt_PrintLabel(TOTAL_LABEL, 0);
			}
			RcptCmd_ChangeMode(TO_RECEIPT, OFF);
#ifdef USE_DOUBLE_PRINT
			if(get_global_bparam(GLOBAL_SALERUN_MODE) != 8)
				RcptCmd_ReceiptFeed();
#else		
        #ifdef USE_TEST_PRT_RENDERING_TIME
            toc = SysTimer1_25ms;
        #endif
			RcptCmd_ReceiptFeed();
#endif//USE_DOUBLE_PRINT
			PrintListWaitEnd();
			break;
		case PRT_ITEM_RETURN_MODE :
			if(GlbFloat.printList) {
				result = Prt_TotalList(2, clerkid, time_str, copy_ticket);
				if(result == OFF) break;
			}
			RcptCmd_ChangeMode(TO_RECEIPT, OFF);
			RcptCmd_ReceiptFeed();
			PrintListWaitEnd();
			break;
		default :
			result = OFF;
			break;
	}
    #ifdef USE_TEST_PRT_RENDERING_TIME
            totalToc = SysTimer1_25ms;
            sprintf(MsgBuf, "#%d Print_Ticket_E\r\nTIC: %d, TOC: %d\r\nTotalTime(ms): %d\r\n-------------\r\n", tempCallCount++, tic, toc, toc-tic);
            MsgOut(MsgBuf);
        #endif
	RcptCmd_ChangeMode(TO_RESTORE, ON);
#ifdef USE_DOUBLE_PRINT
	if(get_global_bparam(GLOBAL_SALERUN_MODE) == 8)
		RcptCmd_CutPaper();	// Double Print 용지 절단 command
#endif
	return result;
}

INT8U Prt_PrintStart(INT8U mode, INT8U prt_mode, INT16U clerkid, INT8U check_sensor, INT16U dept_no, INT32U plu_no)	//return 0:Fail, 1:OK, 2:paper end err
{
	RTC_STRUCT time_str;
	INT8U result, no_check_peel, checkExtPrint;
	STRUCT_TOTAL_PRICE  totalPriceData;
	PREPACK_STATUS_TYPE PrepackStatus;
   	CAPTION_STRUCT	cap;
#ifdef USE_CHECK_DUPLICATION_TICKET
	INT32U checkDuplicate = 0;
    INT8U string_buf[35], tPriceStr[15],priceBuf[20];
	INT8U flag_tmp;
	INT8U chkUseContinuDupliTicket;
#endif
#ifdef USE_TEST_PRT_RENDERING_TIME
    tempCount_rot0 = 0;
    tempCount_rot0m = 0;
    tempCount_rot90 = 0;
    tempCount_rot180 = 0;
    tempCount_rot180m = 0;
    tempCount_rot270 = 0;
#endif
#ifdef USE_TEST_TMP_PRTBUF_MAX_SIZE
    INT32U testResult = 0;
    INT32U testResult2 = 0;
    INT32U testResult3 = 0;
    INT32U testResult4 = 0;
        
    Flash_lwrite( DFLASH_BASE + FLASH_TMP_TEST_RESULT_AREA,  0 );
    Flash_lwrite( DFLASH_BASE + FLASH_TMP_TEST_RESULT_AREA + 4,  0 );
    Flash_lwrite( DFLASH_BASE + FLASH_TMP_TEST_RESULT_AREA + 8,  0 );
    Flash_lwrite( DFLASH_BASE + FLASH_TMP_TEST_RESULT_AREA + 12,  0 );
#endif

#ifdef USE_DOUBLE_PRINT
	if(get_global_bparam(GLOBAL_SALERUN_MODE) == 8 && mode == PRT_TOTAL_NORMAL_MODE)
			prt_mode = RECEIPT;	//Double Print 모드 설정 후 합산 발행 시 티켓 모드로 설정
#endif//USE_DOUBLE_PRINT

	if(prt_mode == RECEIPT)							//se-hyung 20070913
	{
		memset((INT8U*)&PrtItemStr, 0, sizeof(STRUCT_PRT_ITEM));
		memset((INT8U*)&PrtComStr, 0, sizeof(STRUCT_PRT_COMMON));
		memset((INT8U*)&label_set, 0, sizeof(STRUCT_LABEL_SET));
		memset((INT8U*)&PrtTotalStr, 0, sizeof(STRUCT_PRT_TOTAL));
	}

	if(ExtPrtTotalFlag != ON)
	{
		if(check_sensor) {
			if(prt_mode == LABEL) {
				no_check_peel = 1;
			} else {
				no_check_peel = 0;
			}
//#ifdef USE_QR_ADDTOTAL
//			if(!QrAddTotalFlag) 
//			{
//				if(!Prt_CheckPeelAndHead(no_check_peel)) return OFF;
//			}
//#else
				if(!Prt_CheckPeelAndHead(no_check_peel)) return OFF;
//#endif
			if(!Prt_CheckOutOfPaper()) return OFF;
		}
	}

	PrtSensorFlagInit();
	PrtFirstLabel = ON;
	OneItemPrepack = OFF;
	Report_GetCurTime(&time_str,1);
	switch(mode) {
		case PRT_ITEM_NORMAL_MODE :
 			if(prt_mode == LABEL) {
				PrtDrvStruct.PrtCompleteFlag=OFF;
				PrtDrvStruct.PrtLoadFormFlag=ON;
				if (Operation.multiply || Operation.autoDoublePrint) {
					PrtDrvStruct.PrtLoadFormFlag=OFF;
				}
				if(!StartPrintFlag) PrtFirstLabel = OFF;
				Prt_SetPrintData(ITEM_LABEL, &time_str, dept_no, plu_no);
				checkExtPrint = (get_global_bparam(GLOBAL_SALE_SETUP_FLAG15)>>4)&0x01;//menu1873, Use External Printer on Prepack mode 
				if(FlagExternalPrtAddUp)
				{
					start_external_print(1);
					FlagExternalPrtAddUp = OFF;
					PrtDrvStruct.PrtCompleteFlag = ON;//use external Print
					result = ON;
				}
				else if(checkExtPrint && (Operation.wPrepack || Operation.wAuto))
				{
					start_external_print(1);
					PrtDrvStruct.PrtCompleteFlag = ON;//use external Print
					result = ON;
				}
				else
					result = Prt_PrintLabel(ITEM_LABEL, LinkedPrintFlag);
				if(result == OFF) return OFF;
			} else {
			}
			break;
		case PRT_ITEM_TEST_MODE :
			if(prt_mode == LABEL) {
				PrtDrvStruct.PrtCompleteFlag=OFF;
				PrtDrvStruct.PrtLoadFormFlag=ON;
				Prt_SetPrintData(TEST_LABEL, &time_str, dept_no, plu_no);
				result = Prt_PrintLabel(TEST_LABEL, 0);
				if(result == OFF) return OFF;
			} else {
			}
			break;
		case PRT_TOTAL_NORMAL_MODE :
			if(prt_mode == LABEL) {
#ifdef USE_QR_ADDTOTAL
				if(QrAddTotalFlag)
				{
					TotalQRFlag = 1;
					set_nvram_bparam(NVRAM_QR_ADD_COUNT, 0);
// 실제 사용하는 바코드 폼의 길이에 따라 Add Total Count를 계산 
// NVRAM 영역, Floating Clerk Data Sync 등 추후 다시 적용 방안 검토 필요 
//					QR_addedMemUsage[status_scale.clerkid] = 0;
					GenerateQrTotalStringbyTransaction(0, clerkid);
				}
#endif
				if(GlbAddup.printTotalLabel) {
					if (Operation.operationClerk == OPER_NORMAL)
					{
						get_PrepackStatus(&PrepackStatus);
						if(PrepackStatus.OneItem_Sub) {
							OneItemPrepack = ON;
						}
					}
					PrtDrvStruct.PrtCompleteFlag=OFF;
					PrtDrvStruct.PrtLoadFormFlag=ON;
					Prt_CalcTotalPrice(0, clerkid, 0);
					Prt_SetPrintData(TOTAL_LABEL, &time_str, dept_no, plu_no);
					if(ExtPrtTotalFlag == ON) 
					{
						ExtPrtTotalTypeFlag = TOTAL_LABEL;
						start_external_print(1); 
						PrtDrvStruct.PrtCompleteFlag=ON;//use external Print
						result = 1;
					}
					else
					{
						result = Prt_PrintLabel(TOTAL_LABEL, 0);
					}
					if(result == OFF) return OFF;
				}
			} else {	//if(prt_mode == RECEIPT)
#ifdef USE_QR_ADDTOTAL
				if(QrAddTotalFlag)  // QR ADD 카운트 초기화
				{
					TotalQRFlag = 1;
					set_nvram_bparam(NVRAM_QR_ADD_COUNT, 0);
// 실제 사용하는 바코드 폼의 길이에 따라 Add Total Count를 계산 
// NVRAM 영역, Floating Clerk Data Sync 등 추후 다시 적용 방안 검토 필요 
//					QR_addedMemUsage[status_scale.clerkid] = 0;
					GenerateQrTotalStringbyTransaction(0, clerkid);
				}
#endif
				Prt_CalcTotalPrice(0, clerkid, 0);
				result = Prt_PrintTicket(mode, clerkid, &time_str, OFF);
				if(result == OFF) return OFF;
				if(GlbFloat.printCopyTicket) {
#ifdef USE_CHECK_DUPLICATION_TICKET
					/* # Use Continu Dupli-Ticket(P793)
					   Duplication Ticket 사용 시(M1761) P793이 [Y]일 경우
					   : 티켓 출력 후 재발행 여부를 확인한다([Y] 선택 시 계속 발행 가능)
					   Duplication Ticket 사용 시(M1761) P793이 [Y]일 경우
					   : 티켓 출력 후 한번만 재 발행한다 */
					flag_tmp=get_global_bparam(GLOBAL_SALE_SETUP_FLAG21);
					chkUseContinuDupliTicket = OnOff(flag_tmp&0x80);
					if(chkUseContinuDupliTicket == ON)
					{
						do
						{
							checkDuplicate = input_duplication_yesno();
							if(checkDuplicate == 1)
							{
	#ifdef USE_COPYKEY
								flagcopykey = ON;
	#endif
						result = Prt_PrintTicket(mode, clerkid, &time_str, ON);
						if(result == OFF) return OFF;
								if(check_sensor) {
									if(!Prt_CheckOutOfPaper()) return 2;	//Modified by JJANG 20090518
									if(!Prt_CheckGapErr()) return OFF;
							}
							}
						}
						while(checkDuplicate);
	#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
						FloatToString(TO_STR,priceBuf,display_parameter.mode_len[DISPLAY_PRICE],DISPLAY_PRICE,&PrtTotalStr.SummaryTotalPrice,0,1);
	#else
						FloatToString(TO_STR,priceBuf,display_parameter.mode_len[DISPLAY_PRICE],DISPLAY_PRICE,&PrtTotalStr.SummaryTotalPrice,0,0);
	#endif
						GetPriceString(tPriceStr,priceBuf,ON,PRICE_PRINTER);
						caption_split_by_code(0xc022,&cap,0);//"Total %s"
						strcat(cap.form," %s");
						sprintf(gmsg,cap.form,tPriceStr);
						PutNMediumString(1, 0,gmsg, 30);
					}
					else
					{
						result = Prt_PrintTicket(mode, clerkid, &time_str, ON);
						if(result == OFF) return OFF;
					}
#else
					result = Prt_PrintTicket(mode, clerkid, &time_str, ON);
					if(result == OFF) return OFF;
#endif
				}
			}
			break;
		case PRT_TOTAL_GRAND_MODE :
			if(prt_mode == LABEL) {
				if (Operation.operationClerk == OPER_NORMAL)
				{
					get_PrepackStatus(&PrepackStatus);
					if(PrepackStatus.OneItem_Grand) {
						OneItemPrepack = ON;
					}
				}
				PrtDrvStruct.PrtCompleteFlag=OFF;
				PrtDrvStruct.PrtLoadFormFlag=ON;
				Prt_CalcTotalPrice(0, clerkid, 0);
				Prt_SetPrintData(GTOTAL_LABEL, &time_str, dept_no, plu_no);
				if(ExtPrtTotalFlag == ON) 
				{
					ExtPrtTotalTypeFlag = GTOTAL_LABEL;
					start_external_print(1);
					PrtDrvStruct.PrtCompleteFlag=ON;//use external Print
					result = 1;
				}
				else
				{
					result = Prt_PrintLabel(GTOTAL_LABEL, 0);
				}
				if(result == OFF) return OFF;
			} else {
			}
			break;
		case PRT_ITEM_RETURN_MODE :
			if(prt_mode == LABEL) 
			{
				PrtDrvStruct.PrtCompleteFlag=OFF;
				PrtDrvStruct.PrtLoadFormFlag=ON;
				if (Operation.multiply || Operation.autoDoublePrint) {
					PrtDrvStruct.PrtLoadFormFlag=OFF;
				}
				if(!StartPrintFlag) PrtFirstLabel = OFF;
				Prt_SetPrintData(ITEM_LABEL, &time_str, dept_no, plu_no);
				result = Prt_PrintLabel(ITEM_LABEL, LinkedPrintFlag);
				if(result == OFF) return OFF;
			} 
			else 	//if(prt_mode == RECEIPT)
			{	
				ClerkTotalPrice(3, 0, &totalPriceData);
				totalPriceData.SummaryTotalPrice = totalPriceData.TotalPrice;
				TotalData = totalPriceData;
				Prt_CalcTotalPrice(2, clerkid, 1);
				result = Prt_PrintTicket(mode, clerkid, &time_str, OFF);
				if(result == OFF) return OFF;
			}
			break;
	}
	if(check_sensor) {
		if(!Prt_CheckOutOfPaper()) return 2;	//Modified by JJANG 20090518
		if(!Prt_CheckGapErr()) return OFF;
	}
#ifdef USE_TEST_PRT_RENDERING_TIME
    sprintf(MsgBuf, "rot0 total #%d Called\r\n\
rot0M total #%d Called\r\n\
rot180 total #%d Called\r\n\
rot180M total #%d Called\r\n\
rot90 total #%d Called\r\n\
rot270 total #%d Called\r\n\
-------------\r\n",\
            tempCount_rot0,
            tempCount_rot0m, tempCount_rot180, tempCount_rot180m, tempCount_rot90, tempCount_rot270);
    MsgOut(MsgBuf);
#endif
#ifdef USE_TEST_TMP_PRTBUF_MAX_SIZE
        testResult = Flash_lread( DFLASH_BASE + FLASH_TMP_TEST_RESULT_AREA );
        testResult2 = Flash_lread( DFLASH_BASE + FLASH_TMP_TEST_RESULT_AREA + 4 );
        testResult3 = Flash_lread( DFLASH_BASE + FLASH_TMP_TEST_RESULT_AREA + 8 );
        testResult4 = Flash_lread( DFLASH_BASE + FLASH_TMP_TEST_RESULT_AREA + 12 );
        sprintf( MsgBuf, "Result\r\n#1:%d   (Rot0_Mag1)\r\n#2:%d   (Rot180_Mag1)\r\n#3:%d   (Area_Typeface)\r\n#4:%d   (PrtHLine)\r\n\r\n", testResult, testResult2, testResult3, testResult4 );
        MsgOut( MsgBuf );
#endif
 	return ON;
}

INT8U Prt_TotalBarcode(void)
{
#ifdef USE_TEST_PRT_RENDERING_TIME
    static INT16U tempCallCount = 0;
    INT16U tic = 0;
    INT16U toc = 0;
    tic = SysTimer1_25ms;

    sprintf(MsgBuf, "-- #%d Prt_TotBar_S\r\n", tempCallCount);
    MsgOut(MsgBuf);
#endif
//#ifdef USE_QR_ADDTOTAL
//	if(QrAddTotalFlag)  // 티켓모드에서 토탈 바코드를 인쇄 할때 플래그 set
//	{
//		TotalQRFlag = 1;
//	}
//	if(AddSaleFlag)		// 더이상 Add할 Data는 없는 상태를 표시
//	{
//		AddSaleFlag = 0;
//	}
//#endif	RcptCmd_DefaultSet();
	TotalRcpt_PrintBarcode();
#ifdef USE_TEST_PRT_RENDERING_TIME
    toc = SysTimer1_25ms;
    sprintf(MsgBuf, "-- #%d Prt_TotBar_E(Time:%d)\r\n", tempCallCount++, toc-tic);
    MsgOut(MsgBuf);
#endif
	RcptCmd_Start();
	PrintListWaitEnd();
	return ON;
}

INT8U Prt_TotalList(INT8U mode, INT16U ClerkNum, RTC_STRUCT *time_str, INT8U copy_ticket)
{ 
	TRANSACT_CHAIN  *cp, *cp_head;
	TRANSACT_CHAIN  tc;
	TRANSACT_CHAIN  tc_head;
	INT16U h_index, next;
	INT8U result, w_unit, use_unit;
	INT32U  flash_addr;
    INT8U   strTemp1[64];
    INT8U   strTemp2[64];
	INT8U flag_tmp;
#ifdef USE_TEST_PRT_RENDERING_TIME
    static INT16U tempCallCount = 0;
    INT16U tic = 0;
    INT16U toc = 0;
    tic = SysTimer1_25ms;

    sprintf(MsgBuf, "-- #%d Prt_TotList_S\r\n", tempCallCount);
    MsgOut(MsgBuf);
#endif

  	if(MAX_CLERK_NO-1 < ClerkNum) return OFF;

	if(mode == 0)  
	{
		//cp = (TRANSACT_CHAIN *)get_nvram_lparam(NVRAM_TRANS_HEAD+ClerkNum*4);
        h_index = get_nvram_lparam(NVRAM_TRANS_HEAD+ClerkNum*4);
        if (h_index)
        {
            next = h_index;
            get_nvram_trans_addr(NVRAM_TRANS_DATA, h_index - 1, &tc_head);
            cp = &tc_head;
        }
        else
        {
            cp = 0;
        }
	} 
#ifndef USE_TRACE_STANDALONE	// 개체기억기능 영역공유
	else if(mode==1)
	{
		//cp = (TRANSACT_CHAIN *)(ClerkLastHead + mode*(NVRAM_CLEARK_LASTSALE_PNT));
		if (ClerkLastHead)
		{
            next = ClerkLastHead;
            get_nvram_trans_addr(NVRAM_CLEARK_LASTSALE, ClerkLastHead - 1, &tc_head);
            cp = &tc_head;
        }
        else
        {
            cp = 0;
        }
	}
#endif
	else if(mode==2)  
	{
		cp = (TRANSACT_CHAIN *) &TrChain;	
	}

	if(!cp) return OFF;	//empty transaction
	cp_head = cp;

    flag_tmp = get_global_bparam(GLOBAL_PRINT_OPER_FLAG2);
	
	if(OnOff(flag_tmp&0x10) && PrtDrvStruct.ReverseFeedLength) {// all type KKH 110425 for test
		LabCmd_ReverseFeed();
	}

	if(GlbFloat.advertiseLabel[0]) {
		RcptCmd_ChangeMode(TO_NOGAPLABEL, OFF);
		LabCmd_InvertPrt(NON_INVERT);
		label_load(GlbFloat.advertiseLabel[0]);
		LabCmd_PrintStart(1);
		PrintListWaitEnd();
	}

	RcptCmd_ChangeMode(TO_RECEIPT, OFF);	// Change to Receipt Mode

	RcptCmd_DefaultSet();
////////////////////////////////////////////////////////////////KKH 110427 Ticket mode head message add
	flash_addr = DFLASH_BASE + FLASH_TICKET_HEAD_MESSAGE_AREA;
	Flash_sread(flash_addr, (HUGEDATA INT8U *)strTemp1, 64);
	
	if(strlen((char*)strTemp1))
	{
	RcptCmd_TypeNone();
	
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_SUBTITLE, TKT_LAY_N, 0, CENTER);
	RcptCmd_PrintMultiLine((char*)strTemp1, 64, 0);
	RcptCmd_CharLF(1);
    }

////////////////////////////////////////////////////////////////
	PrintTotalHead(ClerkNum, time_str);

	use_unit = ADM_Status.UseUnit;
	if(use_unit == WEIGHT_UNIT_KG) {
		if(get_base_bparam(FLASH_ADC_WUNIT) == WEIGHT_UNIT_G) use_unit = WEIGHT_UNIT_G;
	}

	if(!copy_ticket || (copy_ticket && GlbFloat.printCopyList)) {	// Added by CJK 20050726
		OtherWeightUnit = 0xff;	// initial
		// check weight unit same as use_unit
		while(1) {
			OtherWeightUnit = cp->content.trtMultiple;	// weightunit은 최대 2종류라고 가정
			if(use_unit == cp->content.trtMultiple) {
				OtherWeightUnit = cp->content.trtMultiple;
				break;	// at least 1 item
			}
			if(cp->next) {
				if(mode == 0) {
					//cp = cp->next;
                    next = cp->next;
                    get_nvram_trans_addr(NVRAM_TRANS_DATA, next - 1, &tc);
                    cp = &tc;
				} else {
                    //cp = (TRANSACT_CHAIN *)((INT32U)cp->next + (NVRAM_CLEARK_LASTSALE_PNT));	// Backup
                    next = cp->next;
                    get_nvram_trans_addr(NVRAM_CLEARK_LASTSALE, next - 1, &tc);
                    cp = &tc;
				}
			} else {
				break;
			}
		}
		//////////////////////////
		if(OtherWeightUnit == use_unit) {	// kg/lb 사용시 기본무게단위와 같은무게단위 사용시
			cp = cp_head;
			TotalRcpt_PrintListTitle(use_unit);
// 티켓모드에서 개별아이템 바코드 인쇄 시 각각의 항목의 바코드를 만드는 반복문!!! [Start]
			while(1) {
				CheckCashClose();  //se-hyung 070813
				result = PrintOneTransaction(cp, use_unit);
				if(!result) {
					FlushRxBuf(&CommBufPrt);
#ifdef USE_DOUBLE_PRINT
					if(get_global_bparam(GLOBAL_SALERUN_MODE) == 8)
						FlushTxBuf(&CommBuf);
#endif//USE_DOUBLE_PRINT
					RcptCmd_ChangeMode(TO_RESTORE_FORCE, ON);
					return OFF;
				}
				if(cp->next) {
					if(mode == 0) {
                        //cp = cp->next;
                        next = cp->next;
                        get_nvram_trans_addr(NVRAM_TRANS_DATA, next - 1, &tc);
                        cp = &tc;
					} else {
						//cp = (TRANSACT_CHAIN *)((INT32U)cp->next + (NVRAM_CLEARK_LASTSALE_PNT));	// Backup
                        next = cp->next;
                        get_nvram_trans_addr(NVRAM_CLEARK_LASTSALE, next - 1, &tc);
                        cp = &tc;
					}
				} else {
					break;
				}
			}
// 티켓모드에서 개별아이템 바코드 인쇄 시 각각의 항목의 바코드를 만드는 반복문!!! [End]
		}
		if(OtherWeightUnit != use_unit) {	// kg/lb 사용시 기본무게단위와 다른무게단위 사용시
			cp = cp_head;
			w_unit = OtherWeightUnit;
			TotalRcpt_PrintListTitle(w_unit);
			while(1) {
				result = PrintOneTransaction(cp, w_unit);
				if(!result) {
					FlushRxBuf(&CommBufPrt);
					RcptCmd_ChangeMode(TO_RESTORE_FORCE, ON);
					return OFF;
				}
				if(cp->next) {
					if(mode == 0) {
                        //cp = cp->next;
                        next = cp->next;
                        get_nvram_trans_addr(NVRAM_TRANS_DATA, next - 1, &tc);
                        cp = &tc;
					} else {
                        //cp = (TRANSACT_CHAIN *)((INT32U)cp->next + (NVRAM_CLEARK_LASTSALE_PNT));	// Backup
                        next = cp->next;
                        get_nvram_trans_addr(NVRAM_CLEARK_LASTSALE, next - 1, &tc);
                        cp = &tc;
					}
				} else {
					break;
				}
			}
		}
	}
	PrintTotalTail(copy_ticket);
	TotalRcpt_PrintTicketSerial();
	
	////////////////////////////////////////////////////////////////KKH 110427 Ticket mode Tail message add
#ifdef USE_QR_BARCODE_ERASE_GAP
	/* Erase LF */
#else
	RcptCmd_CharLF(1);
#endif
	flash_addr = DFLASH_BASE + FLASH_TICKET_TAIL_MESSAGE_AREA;
	Flash_sread(flash_addr, (HUGEDATA INT8U *)strTemp2, 64);
    if(strlen((char*)strTemp2))
	{
	RcptCmd_TypeNone();
	
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_SUBTITLE, TKT_LAY_N, 0, CENTER);
	RcptCmd_PrintMultiLine((char*)strTemp2, 64, 0);
	RcptCmd_CharLF(1);
    }
////////////////////////////////////////////////////////////////
#ifdef USE_TEST_PRT_RENDERING_TIME
    toc = SysTimer1_25ms;
    sprintf(MsgBuf, "-- #%d Prt_TotList_E(Time:%d)\r\n", tempCallCount++, toc-tic);
    MsgOut(MsgBuf);
#endif

	RcptCmd_Start();
	PrintListWaitEnd();
    
	if(GlbFloat.advertiseLabel[1]) {	// Print Tail
		RcptCmd_ChangeMode(TO_NOGAPLABEL, OFF);
		LabCmd_InvertPrt(NON_INVERT);
		label_load(GlbFloat.advertiseLabel[1]);
		LabCmd_PrintStart(1);
		PrintListWaitEnd();
	}

  	return ON;
}


INT8U Prt_TotalList_Credit(INT8U mode, INT16U ClerkNum, RTC_STRUCT *time_str)
{ 
	TRANSACT_CHAIN  *cp;
	TRANSACT_CHAIN  tc;
	INT16U h_index, next;
	INT16U cur_time;

  	if(MAX_CLERK_NO-1 < ClerkNum) return OFF;

	if(mode == 0) {
		//cp = (TRANSACT_CHAIN *)get_nvram_lparam(NVRAM_TRANS_HEAD+ClerkNum*4);
        h_index = get_nvram_lparam(NVRAM_TRANS_HEAD+ClerkNum*4);
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
	} else {
		//cp = (TRANSACT_CHAIN *)(ClerkLastHead + mode*(NVRAM_CLEARK_LASTSALE_PNT));
		if (ClerkLastHead)
		{
            next = ClerkLastHead;
            get_nvram_trans_addr(NVRAM_CLEARK_LASTSALE, next - 1, &tc);
            cp = &tc;
        }
        else
        {
            cp = 0;
        }
	}
	if(!cp) return OFF;	//empty transaction

	cur_time = SysTimer100ms;
	while((INT16U)(SysTimer100ms - cur_time) < WAIT_2ND_TICKET_100MS){
		if(KEY_Read()) {
			if(KeyDrv.CnvCode == KS_PRINT) {
				BuzOn(1);
				break;
			} else {
				BuzOn(2);
			}
		}
	}

	if(GlbFloat.advertiseLabel[0]) {
		RcptCmd_ChangeMode(TO_NOGAPLABEL, OFF);
		LabCmd_InvertPrt(NON_INVERT);
		label_load(GlbFloat.advertiseLabel[0]);
		LabCmd_PrintStart(1);
		PrintListWaitEnd();
	}
	RcptCmd_ChangeMode(TO_RECEIPT, OFF);	// Change to Receipt Mode
	RcptCmd_DefaultSet();
	PrintTotalHead(ClerkNum, time_str);
	RcptCmd_CharLF(1);
	TotalRcpt_CustomerName(PrtTotalStr.CustomerNo);
	RcptCmd_CharLF(1);
	TotalRcpt_PrintCreditTotal(PrtTotalStr.PaidTypePrice[PAY_CREDIT]);
	RcptCmd_CharLF(1);
	PrintCreditSign();
	TotalRcpt_PrintTicketSerial();

	RcptCmd_Start();

	PrintListWaitEnd();

	if(GlbFloat.advertiseLabel[1]) {	// Print Tail
		RcptCmd_ChangeMode(TO_NOGAPLABEL, OFF);
		LabCmd_InvertPrt(NON_INVERT);
		label_load(GlbFloat.advertiseLabel[1]);
		LabCmd_PrintStart(1);
		PrintListWaitEnd();
	}

	RcptCmd_ChangeMode(TO_RECEIPT, OFF);
	RcptCmd_ReceiptFeed();
	PrintListWaitEnd();

  	return ON;
}


INT16S Prt_CalcTotalPrice(INT8U addr_mode, INT8U ClerkNum, INT8U ret_chk)
{ 
//	TRANSACT_CHAIN  *cp;
	INT8U i;
	memset((INT8U*)&PrtTotalStr, 0, sizeof(STRUCT_PRT_TOTAL));

	PrtTotalStr.ClerkNum   = ClerkNum;
	PrtTotalStr.SummaryTotalPrice = TotalData.SummaryTotalPrice;
	PrtTotalStr.SummarySavedPrice = TotalData.SummarySavedPrice;
	PrtTotalStr.TotalSavedPrice = TotalData.TotalSavedPrice;	//Added by JJANG 20071228
	PrtTotalStr.TotalPrice = TotalData.TotalPrice;
	PrtTotalStr.TotalCount = TotalData.TotalCount;
	PrtTotalStr.VoidPrice  = TotalData.VoidPrice;
	PrtTotalStr.VoidCount  = TotalData.VoidCount;
	PrtTotalStr.CPrice = CurrencyPrice;
	PrtTotalStr.TotalWeight = TotalData.TotalWeight;
	PrtTotalStr.TotalQuantity = TotalData.TotalQuantity;
	PrtTotalStr.SummaryRound = TotalData.SummaryRound;//transPay.summaryround;	// Added by CJK 20070119
	if (!ClerkNum)
	{
		PrtTotalStr.TotalNetWeight = TotalData.TotalNetWeight;
		PrtTotalStr.TotalPerTareWeight = TotalData.TotalPerTareWeight;
		PrtTotalStr.TotalGrossWeight = TotalData.TotalGrossWeight;
	}

	//Modified by JJANG 20080619 return시 ticket number는 '0'으로 찍음, 추 후 변경예정
	if (ret_chk) PrtTotalStr.TicketTransactNumber = 0;	
	else PrtTotalStr.TicketTransactNumber = get_TicketTransactionNumber(ClerkNum);

	if(addr_mode == 0) 
	{
//		cp = (TRANSACT_CHAIN *)get_nvram_lparam(NVRAM_TRANS_HEAD+ClerkNum*4);
	}
#ifndef USE_TRACE_STANDALONE	// 개체기억기능 영역공유
	else if(addr_mode == 1)
	{
//		cp = (TRANSACT_CHAIN *)(ClerkLastHead + (NVRAM_CLEARK_LASTSALE_PNT));
	}
#endif
	else if(addr_mode ==2) //SG070213
	{
//		cp = (TRANSACT_CHAIN *)&TrChain;
	}

#ifdef USE_TAX_RATE
	for(i = 0; i < MAX_TAX_NO; i++)
	{
		PrtTotalStr.TaxTypePrice[i] = TotalData.taxPrice[i];
		PrtTotalStr.TaxTypeTotal[i] = TotalData.taxGross[i];
	}
	PrtTotalStr.InTaxPrice = TotalData.InTaxPrice;
	PrtTotalStr.ExTaxPrice = TotalData.ExTaxPrice;
#endif    

	if(TotalData.PayPrice) {
		PrtTotalStr.PayPrice = TotalData.PayPrice;
		PrtTotalStr.ChangePrice = transPay.change;
		for(i = 0; i < MAX_PAID_TYPE; i++) {
			PrtTotalStr.PaidTypePrice[i] = transPay.paidprice[i];
		}
		PrtTotalStr.CustomerNo = DiscSale.saleCreditCustomer;
	} else {
		PrtTotalStr.ChangePrice = 0L;
		for(i = 0; i < MAX_PAID_TYPE; i++) {
			PrtTotalStr.PaidTypePrice[i] = 0L;
		}
		PrtTotalStr.CustomerNo = 0;
	}

  	return ON;
} //

#ifdef USE_DISCOUNT_MEMBERSHIP
long Prt_Calc_Discount_Price(long price,long percent)
{
	long val1, val2, result;

    val1 = price  * percent;
    result = val1 / 100L;
    
    val2 = val1 % 100L;
    if (val2) // 소수점 존재
        result = result + 1; // 올림처리

	return result;
}
#endif

#ifdef USE_COPYKEY
void Prt_copykey(void)
{
	char	string_buf[6];
	RcptCmd_ChangeMode(TO_RECEIPT, OFF);
#ifdef USE_GERMANY_DEFAULT
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_TITLE, TKT_LAY_N, 0, CENTER);
	sprintf(string_buf, "KOPIE");	
#else
    RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_TITLE, TKT_LAY_N, 0, RIGHT);
    sprintf(string_buf, "COPY");
#endif
	RcptCmd_PrintMultiLine(string_buf, sizeof(string_buf), 0);
	RcptCmd_Start();
	PrintListWaitEnd(); 
	flagcopykey=OFF;
}
#endif

#ifdef USE_TRACE_STANDALONE
void Prt_LotList(INT8U external_prt)
{
	char	string_buf[100];
	char 	pluname_buf[64];
	char 	sl_name_buf[TRACE_SLAUGHTHOUSE_NAME_SIZE];
	INT16U	v16;
	INT16U	pluAddr;
	INT8U	i;
	INT32U  addr;
	INT8U	prt_sl_name;	//묶음 장부에 도축장명을 인쇄한다.(param 705)
	HUGEDATA COMM_BUF *commBuf;

	prt_sl_name = get_global_bparam(GLOBAL_SALE_SETUP_FLAG11);
	prt_sl_name = (prt_sl_name & 0x40) >> 6;

	if (external_prt)
	{
		commBuf = &CommBuf;
		sprintf(string_buf,"\r\n");
		PutDataTxBuf(commBuf, string_buf, strlen(string_buf));
		sprintf(string_buf,"FR%cCL5LOT%c\r\n",'"','"');
		PutDataTxBuf(commBuf, string_buf, strlen(string_buf));
		sprintf(string_buf,"?\r\n");
		PutDataTxBuf(commBuf, string_buf, strlen(string_buf));
	}
	else
	{
		commBuf = &CommBufPrt;
	    	LabCmd_ClearLabel();
		StrInit4UserLabel();	// Added by CJK 20060426
		RcptCmd_SetFontType(PrtFontType);//0=one byte font, 1=two byte font //SG060313
		RcptCmd_SetFontId(3);
		RcptCmd_IntercharSP(0);
		LabCmd_SetStrField(8, 80, 560, 440);
		LabCmd_CharSize(2, 3);
		LabCmd_StrStart();
	}
	if(UseLotNum)
	{
		sprintf(string_buf, "묶음번호 (%s)\r\n", LotMultiDataNo);
	}
	else
	{
		sprintf(string_buf, "묶음장부 (LOT LIST)\r\n");
	}
	if (external_prt)
	{
		PutDataTxBuf(commBuf, string_buf, strlen(string_buf));
	}
	else
	{
		PutDataRxBuf(commBuf, string_buf, strlen(string_buf));
		LabCmd_StrEnd();
	}

	if (!external_prt)
	{
		LabCmd_SetStrField(8, 128, 560, 440);
		LabCmd_CharSize(1, 2);
		LabCmd_StrStart();
	}
	for(i = 0; i < LotListMultiIndex; i++)
	{
		plu_table_search((INT8U)LotMultiData[i].deptNo, LotMultiData[i].pluNo, (INT16U*)&pluAddr, 0);
		pluname_buf[0] = 0;
		if (pluAddr)
		{
			plu_get_field(pluAddr-1, PTYPE_PLU_NAME1, (INT8U *)&pluname_buf, (INT16S *)&v16, sizeof(pluname_buf));
			if(prt_sl_name)
			{
				if (v16 > 16) v16 = 16;		//PLU NAME이 16보다 길면 16으로 만든다. 
			}

			if (IsHanCharMid(pluname_buf, v16) == CHAR_HAN_MID) 
			{
				pluname_buf[v16-1] = ' ';
			}
			pluname_buf[v16] = 0;
		}
		
		addr = DFLASH_BASE + FLASH_TRACE_SLAUGHTERHOUSE_AREA;
		addr = addr + TRACE_SLAUGHTERHOUSE_KR_STRUCT_SIZE * (INT32U)(LotMultiData[i].slaughterNo-1);
		v16 = Flash_wread(addr);
		sl_name_buf[0] = 0;
		if (LotMultiData[i].slaughterNo == v16)
		{
			addr += 2;
			v16 = TRACE_SLAUGHTHOUSE_NAME_SIZE;
			Flash_sread(addr, (HUGEDATA INT8U *)sl_name_buf, v16);
			if (v16 > 18) v16 = 18;
			if (IsHanCharMid(sl_name_buf, v16) == CHAR_HAN_MID) 
			{
				sl_name_buf[v16-1] = ' ';
			}
			sl_name_buf[v16] = 0;
		}
#ifdef USE_GSMART_TRACE
		sprintf(string_buf, "(%d) %s: %s\r\n", i+1, LotMultiData[i].indivNoStr, pluname_buf);
#else
		if(prt_sl_name)
		{
			if (external_prt) sprintf(string_buf, "(%d)%s:%-16s,%-18s\r\n", i+1, LotMultiData[i].indivNoStr, pluname_buf, sl_name_buf);
			else sprintf(string_buf, "(%d) %s: %-16s, %-18s\r\n", i+1, LotMultiData[i].indivNoStr, pluname_buf, sl_name_buf);
		}
		else
		{
			if (external_prt) sprintf(string_buf, "(%d)%s:%s\r\n", i+1, LotMultiData[i].indivNoStr, pluname_buf);
			else sprintf(string_buf, "(%d) %s: %s\r\n", i+1, LotMultiData[i].indivNoStr, pluname_buf);
		}
#endif
		if (external_prt)
		{
			PutDataTxBuf(commBuf, string_buf, strlen(string_buf));
		}
		else
		{
			PutDataRxBuf(commBuf, string_buf, strlen(string_buf));
		}
	}
	if (external_prt)
	{
		sprintf(string_buf, "\r\n");
		for(; i < 20; i++)
		{
			PutDataTxBuf(commBuf, string_buf, strlen(string_buf));
		}
		sprintf(string_buf,"ZB\r\n");	// printer direction : bottom
		PutDataTxBuf(commBuf, string_buf, strlen(string_buf));
		sprintf(string_buf, "P1\r\n");
		PutDataTxBuf(commBuf, string_buf, strlen(string_buf));
		network_tx_proc();
	}
	else
	{	
		LabCmd_StrEnd();
		RcptCmd_TypeNone();
		LabCmd_PrintStart(1);
		PrintListWaitEnd();
	}
}
#endif

#ifdef USE_HYUNDAI_MEAT_FUNCTION
extern INT8U BarcodeData[];
extern char BarI25_Checksum( char *code,short leng,char type);
INT8U generate_barcode_string(INT8U deptno, long pluno, INT8U plutype)
{
	INT16U plu_key, barid, length;
	char bar_form[BARCODE_FORMAT_LEN+1];
	char bar_str[BARCODE_FORMAT_LEN+1];
	STRUCT_STRFORM_PARAM bar_param;
	INT8U bar_type, barcode_type;
	INT8U k, ret;
	RTC_STRUCT time_str;

	plu_table_search(deptno, pluno, &plu_key, 0);
	if (plu_key == 0) return OFF;
	barid = 0;
	if (plu_key) {
		barid = prt_get_plu_barid(deptno, plu_key, plutype, 1);	// Barcode No.1 id read
	}
	if (barid < 1 || barid > 99) barid = 0;
	if(BarcodeNumber) barid = BarcodeNumber;
	if (!get_barcodeformat(barid, (char *)bar_form, &barcode_type)) return OFF;
  	if (status_scale.Plu.sales_msg == TRANS_DISPOSAL_RETURN || status_scale.Plu.sales_msg == TRANS_DISPOSAL_FRESHNESS_LOSS \
		|| (MeatSetSaleMode && MeatSetSaleUseScanner)) {
		PrtFirstLabel = ON;
		Prt_SetPrintData(ITEM_LABEL, &time_str, deptno, pluno); //
		PrtFirstLabel = ON;
  	}
	prt_set_strform_parameter_item(&bar_param, NULL);

	bar_type=barcodetype_codevalue[barcode_type];
	if (bar_type != NO_BARCODE) ret = prt_make_barform(bar_str, bar_form, &bar_param, bar_type);
	if (!ret) return OFF;

	length = strlen(bar_str);
	if(bar_type == I2OF5) { //CODE128C & CODE128X 일 경우에만 Checksum 을 prt_make_strform에서 처리. 그 외의 경우에는 BarIngerpreter 에서 처리
		bar_str[length] = BarI25_Checksum(&bar_str[0],length,BarStruct.CheckType) + '0';
		length += 1;
		bar_str[length]=0;
	}
	if( length > sizeof(BarcodeData) ) length = sizeof(BarcodeData);
	k =0;
	memcpy(&BarcodeData[k], bar_str, length);	//SG071012. TEST ONLY
	k += length;
	BarcodeData[k]=0;

	return ON;
}
#endif

