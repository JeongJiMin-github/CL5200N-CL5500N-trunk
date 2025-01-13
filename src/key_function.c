/*****************************************************************************
 |*			
 |*  Copyright		:	(c) 2002 CAS
 |*  Filename		:	key.c
 |*  Version		:	0.1
 |*  Modified		:	2004/02/11
 |*  Modified		:	2004/09/01
 |*  Description		:	CLP Key routine    
 ****************************************************************************/

#include <stdio.h>
#include <string.h>
#include "globals.h"
#include "commbuf.h"
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
#include "bar_common.h"
#include "prt_interpreter.h"
#include "adm_app.h"
#include "rtc.h"

#include "adm.h"
#include "main.h"
#include "dsp_vfd7.h"
#include "initial.h"
#include "key.h"
#include "key_app.h"
#include "key_typedef.h"
#include "key_function.h"
#include "mode_main.h"
#include "caption_app.h"
#include "common.h"
#include "prt_common.h"
#include "menu.h"
#include "flash_app.h"
#include "dsp_app.h"
#include "plu_device.h"
#include "plu_manager.h"
#include "plu_edit.h"

#include "common_sunday.h"
#include "discount.h"
#include "mode_sale.h"
#include "adm_app.h"
#include "plu_auxcall.h"
#include "department.h"
#include "clerk_app.h"
#include "cashdrawer.h"
#include "tax.h"
#include "pgm_app.h"
#include "prt_cmdtest.h"
#include "ethernet_comm.h"
#include "login.h"
#include "prt_app.h"
#include "currency.h"
#include "ethernet_comm.h"
#include "debug.h"

#include "ps2.h"

#include "mode_commun.h"
#include "traceability.h"
#include "initial_glb.h"
#include "network_command.h"
#include "shopname.h"
#include "barcode_file.h"

#include "rfid_drv_rfid522b.h"
#include "usbmem_app.h"
#include "Pcs_unit.h"
#include "rtc_app.h"
#include "label_caption.h"
#include "global_set.h"
#ifdef USE_CHN_CART_SCALE
#include "traceability_chn.h"
#endif
#include "prt_advertise.h"  
#include "inventory.h"
#ifdef USE_EFT_POS_PROTOCOL
#include "network_set.h"
#endif

CAPTION_STRUCT cap;

#ifdef USE_TOPMART_DEFAULT //sewon 170116
INT16U InputQty;
INT8U InputQtyFlag;
INT8U PackdatePrintFlag;
#endif
INT8U  keyin_escape_clear;
INT8U  Selfkey_flag=OFF;
INT32U Scan_clerk=0;	//Added by JJANG 071022
char	gmsg[100];
INT8U ScanFlag = 0;
#ifdef USE_PRICE_CHANGE_PROHIBITED
INT16U PressedNumberKey_1000Digits = 0;	//PressedNumberKey 값 3자리까지  값 저장
INT16U PressedNumberKey = 0;		//PLU 호출 후 입력된 라벨광고문 번호 또는 임시 가공일 변경키 값 
#else
INT8U PressedNumberKey = 0;		//PLU 호출 후 입력된 라벨광고문 번호 또는 임시 가공일 변경키 값 (0~9)
#endif

#ifdef USE_LOTTEMART_DEFAULT
INT8U OnlineFlag =OFF;
#endif
#ifdef USE_SETPRODUCT_SALE
INT8U SetproductSaleMode = OFF;
#endif
#ifdef USE_ZERO_RANGE_MSG
INT8U Zerokey_input_Flag = OFF;
#endif
#ifdef USE_HYUNDAI_SET_FUNCTION
RTC_STRUCT SetSaleRTC;
#endif

extern HUGEDATA PACKDATE_STRUCT mPackDate; //SG060309
extern PRICE_TYPE Price;
extern PRICE_TYPE LinkedPrice[MAX_LINKED_PLU];
extern TARE_OPER_TYPE		TareOper;
extern SALE_G_PRINT_TYPE	GlbPrint;
extern INT16U	ClerkLoginTime;
extern void prtsel_set_table_buf_pack(HUGEDATA INT8U *buf, INT8U fieldid, INT8U val);
extern  void FloatToString(INT16S toString,char *str,INT16S totalLength,INT16S mode,long *longNum,INT16S decimalLength, INT8U decFlag);
extern  long CalcPercentPrice(long pPrice,INT16S percent);
extern  INT16S  PutNSmallString(INT16S y, INT16S x,char *data, INT16S n);
extern  void sale_calcprice(void);
extern INT8U OnOff(INT8U	tmp);
extern void  sale_display_tare_eu(void);
extern INT16S   ClerkSearchVoidTransaction(INT16U dept, INT32U plu, INT16U clerkNum, INT8U deleteFlag, long *totalPrice);
extern void  ClerkGrandTotalSum(INT8U clerkNum, STRUCT_TOTAL_PRICE *totalData);// , long trtTax);	//CJK20040909
extern long  PriceRounds(long tprice, INT16S roundType, INT16S assignFlag, long *roundDifference);
extern INT16S   PutSmallString(INT16S y, INT16S x,char *data);
extern INT16S   PutNMediumString(INT16S y, INT16S x,char *data, INT16S n);
extern void  sale_display_price(void);
extern void  keyapp_st_ttl(void);
extern INT8U keyapp_sale_status(INT8U x);
extern void  keyapp_cleardiscount(void);
extern INT16U  keyapp_doubleplukey(INT16U skeyplu, INT8U settingKey);
extern void keyapp_set_packeddate(void);
extern void keyapp_onoff_printdate(void);
extern void keyapp_weight_reset(void);
extern void keyapp_check_production(void);
extern void keyapp_delete_production(void);
extern void keyapp_scanprint(void);
extern void keyapp_labelformat_change(INT8U keyNum);
extern void  sale_display_total(INT8U onlyvfd);
extern INT8U keyin_anycheck_on;
extern TRANSACTION_PAY transPay;
//extern char MsgShort;
extern INT8U PercentTareOver;
extern void CheckZeroTotalPrice(void);
extern INT16S NetworkGetTransactionData(void);
extern INT16S netGetClerkLock(INT8U operator, INT8U scaleid);
extern void netSetClerkLock(INT8U operator, INT8U scaleid, INT8U lock);
extern INT16S NetClerkLock(INT8U operator);
extern INT8U NetClerkLockRelease(INT8U operator);
extern void DisplayClerkTotalInfo(INT8U ClerkNum);
extern NETWORK_CLERK_SALE_AMOUNT ClerkSaleAmount;
extern INT8U GlbFlagPCnMasterConnection;
extern INT16S NetCheckConnection(void);
void display_message_page(char *str);
INT8U display_message_check_page(char *S_msg1, char *S_msg2, INT8U mode);
extern INT8U display_message_printerr_page(INT16U L_msg, INT16U S_msg);
void keyapp_switch_offline(void);
extern INT16S PutNLargeString(INT16S y, INT16S x,char *data, INT16S n);
//extern void getScannerSymbolInfo(INT8U* index, INT8U* leng, char symbol);
extern void display_message_page_mid(char *str);
extern INT16S FontSizeChange(INT16S fontSize);
extern INT8U command_check_indiv_info(HUGEDATA COMM_BUF *CBuf);
#ifdef USE_PLU_MULTIPLE_PRICE
extern void multi_plu_disc(INT8U tType, long uPrice, long fTarget, long fPrice, long sTarget, long sPrice);
#endif
void keyapp_set_valid_date_back_label(void);
#ifdef USE_NHMART_SAFE_MEAT
void keyapp_call_indivInfo(void);
#endif
#ifdef USE_TOPMART_ASK_INDIV_INFO
INT8U keyapp_ask_indivInfo(HUGEDATA char *indiv_no); //sewon 170209
#endif
#ifdef USE_CHECK_INDIV_INFO
INT8U CheckIndivInfo(void);
#endif
#ifdef USE_TRACE_STANDALONE 
//extern INT8U  plu_table_individual[]; 
extern INT8U  FlagDisplayIndividual;
extern INT8U  IndividualDisplayTimeFlag;
TRACE_STATUS_STRUCT CalledTraceStatus;
#endif
#ifdef USE_EMART_PRODUCTION_KEY
INT8U BackupCheckin = 0;
#endif

#ifdef USE_EMART_PIG_TRACE
char ImportLotNumber[24];
INT8U ImportLotNumberflag;
#endif

#ifdef USE_COPYKEY
INT8U flagcopykey = OFF;
#endif
INT8U flagcopy_ticket = OFF;
//void	MsgDisplayWeight(void)
//{
//	DisplayMsg((char *)global_message[MSG_23_C803]);//"Remove weight on tray"
//}
#ifdef USE_QR_ADDTOTAL
//INT8U AddSaleFlag = 0;
INT8U QrAddCount = 0;
INT8U QrAddTotalFlag = 0;
#endif
#ifdef USE_CHN_EMART_TRACE//USE_CHN_EMART_BIRDS_TRACE
INT8U Pluchangeflag; //20140912
#endif
#ifdef USE_SETPRODUCT_SALE
void keyapp_nonweighing_mode(void);
#endif
void keyapp_change_keyset(INT16U dept_id);

static void disp_msg_ext_prt(void);

/**
 * @brief  외부 프린트 인쇄 시 화면에 문구 표시
 * @param  void 
 * @return void 
 */
static void disp_msg_ext_prt(void)
{
	/* 한글 등 폰트 크기가 다를 경우 외부 프린트 메시지를 디스플레이에 표시 시 겹침 문제로 인해 
	   영어로만 통일하여 표시하기로 함(캡션 사용 x, 하드코딩으로) */
	DisplayMsg("Ext Prt");
}

void	MsgCanadianTare(void)
{
	caption_split_by_code(0xc802,&cap,0);
	DisplayMsg(cap.form);//"Check tare"
	return;
}

INT16S CheckTimeOver(INT16U delay,INT16U checkTime)
{
	if (delay==0) return OFF;
	if ((INT16U)delay < (INT16U)(SysTimer100ms-checkTime)) return ON;
	return OFF;
}

INT16S CheckTimeOver10ms(INT16U delay,INT16U checkTime)
{
	if (delay==0) return OFF;
	if ((INT16U)delay < (INT16U)(PrtDrvStruct.Timer10ms-checkTime)) return ON;
	return OFF;
}

extern void CheckCashClose(void);

void Key_DelaySec(INT16S delay,INT16U preTimes)
{
	INT16U tic;
	if(preTimes)	
	    tic = preTimes;
	else
	    tic = SysTimer100ms;
	for(;;)
	{
		CheckCashClose();
		network_check();
		if(delay != 0)	//delay가 0인 경우 key 입력시 까지 멈춘다.
		{
			if ((INT16U)delay < (INT16U)(SysTimer100ms-tic)) break;
		}
		if (KEY_IsKey() || PS2_IsKey())
		{
			Operation.keyClear = OFF;
			break;
		}
	}
}

INT8U PluType(void)
{
	INT8U pluType;
	
	pluType=(status_scale.flag_input>>1)&0x07;
	return pluType;
}

void	ChangePluType(INT8U	stype)
{
#ifdef USE_INDIA_FUNCTION
	INT32U bagWt;
	
	if (ReadPluBagWt(status_scale.departmentid, status_scale.cur_pluid, &bagWt) && PluType() != stype)
	{
		if (stype == PLU_BY_WEIGHT) // qty->weight
		{
			status_scale.cur_unitprice = status_scale.cur_unitprice * power10(3)/ bagWt;
		}
		else
		{
			status_scale.cur_unitprice = status_scale.cur_unitprice * bagWt / power10(3);
		}
	}
#endif
#ifdef USE_WEIGHT_UNIT_500G
#ifdef USE_UNIT_500G_PLU_500G_BASE
	INT32U tempUprice;

		if (stype == PLU_BY_WEIGHT && status_scale.cur_weightunit == 1000) // qty->weight
		{
			tempUprice =  longmuldiv(status_scale.Plu.unitprice,2*status_scale.cur_weightunit,power10(3));
			status_scale.cur_unitprice = (INT32U) tempUprice;
		}
#endif
#endif
	stype<<=1;
	status_scale.flag_input&=0xf1;
	status_scale.flag_input|=stype;
}

void	SetMaxRide(INT8U stype)
{
	if (Operation.override)
	{
		if (Operation.fixedPrice) status_scale.ride_max = 1;  // By Fixed
		else if (stype == PLU_BY_WEIGHT) status_scale.ride_max = 1;  // By Weight
		else if (stype == PLU_BY_COUNT) status_scale.ride_max = 2;  // By nonFood
		else if (stype == PLU_BY_PCS) status_scale.ride_max = 3;  // By Count
	}
	else
	{
		if (Operation.fixedPrice) status_scale.ride_max = 0;  // By Fixed
		else if (stype == PLU_BY_WEIGHT) status_scale.ride_max = 0;  // By Weight
		else if (stype == PLU_BY_COUNT) status_scale.ride_max = 1;  // By nonFood
		else if (stype == PLU_BY_PCS) status_scale.ride_max = 1;  // By Count
	}
}

void SetCurRide(INT8U stype)
{
	INT8U byte;
	
	if (Operation.override)
	{
		byte = get_global_bparam(GLOBAL_SALE_SETUP_FLAG3);// override location
		if (OnOff(byte&0x02)) 
		{
			if (Operation.fixedPrice) status_scale.cur_ride = 1;  // By Fixed
			else if (stype == PLU_BY_WEIGHT) status_scale.cur_ride = 1;  // By Weight
			else if (stype == PLU_BY_COUNT) status_scale.cur_ride = 2;  // By nonFood
			else if (stype == PLU_BY_PCS) status_scale.cur_ride = 3;  // By Count
		}
		else
		{
			if (Operation.fixedPrice) status_scale.cur_ride = 1;  // By Fixed
			else if (stype == PLU_BY_WEIGHT) status_scale.cur_ride = 1;  // By Weight
			else if (stype == PLU_BY_COUNT) status_scale.cur_ride = 1;  // By nonFood
			else if (stype == PLU_BY_PCS) status_scale.cur_ride = 1;  // By Count
		}
	}
	else
	{
		if (Operation.fixedPrice) status_scale.cur_ride = 0;  // By Fixed
		else if (stype == PLU_BY_WEIGHT) status_scale.cur_ride = 0;  // By Weight
		else if (stype == PLU_BY_COUNT) status_scale.cur_ride = 1;  // By nonFood
		else if (stype == PLU_BY_PCS) status_scale.cur_ride = 1;  // By Count
	}
}

extern SALE_G_OPER_TYPE	GlbOper;

void NumericValue(INT32U maxValue, INT32U *value)
{
	INT64U temp_64bits_val;
	status_scale.cur_keyvalue = *value;
	temp_64bits_val = status_scale.cur_keyvalue;
	if (KeyDrv.FirstKey) 
	{
		temp_64bits_val = 0;
	}
	if (KeyDrv.CnvCode > 9) 
	{
		temp_64bits_val = temp_64bits_val * KeyDrv.CnvCode;
	} 
	else 
	{
		temp_64bits_val = temp_64bits_val * 10 + KeyDrv.CnvCode;
	}
	
	temp_64bits_val %= maxValue;
	status_scale.cur_keyvalue = (INT32U)temp_64bits_val;
	*value = status_scale.cur_keyvalue;
}

INT8U	HiddenClerkNumber=0;
INT16U	Netweight_flag;
extern DISC_SALE_TYPE		DiscSale;
extern INT8U Seg7BlinkOn;
extern INT16U Set7BlinkPrevTime;

void key_numeric(void)
{
	INT8U smode, ret; //,v8;
	INT8U n;
	INT32U multiple;
#ifdef USE_PRICE_CHANGEABLE_CHECK
	INT16U plu_addr;
	INT16S size;
	INT8U priceChangeFlag;
	INT8U string_buf[20];
#endif
	
	// qty, pcs, unitprice 편집시 blink. 숫자키를 누를 때 마다 값이 표시되도록 함
	Set7BlinkPrevTime = SysTimer100ms;
	Seg7BlinkOn = 0;
	
	smode=(status_scale.flag_input>>1)&0x07;
	if (KeyDrv.Buzzer) BuzOn(1);
	if (KeyDrv.CnvCode==KEY_NUM_10) keyapp_nzero();
	if (Operation.multiply && Operation.multiplyEdit)
	{
		multiple = (INT32U)status_scale.print_multiple;
#ifdef USE_X10000_PRINT
		NumericValue(10000, &multiple);
#else
		NumericValue(1000, &multiple);
#endif
		status_scale.print_multiple = (INT16S)multiple;
		//DEL070130	       	if(KeyDrv.FirstKey) {
		//DEL070130	       		KeyDrv.FirstKey = 0;
		//DEL070130	       		status_scale.cur_keyvalue   = 0;
		//DEL070130	       		status_scale.print_multiple = 0;
		//DEL070130	       	}
		//DEL070130		status_scale.cur_keyvalue =status_scale.print_multiple;
		//DEL070130		if (KeyDrv.CnvCode>10) {
		//DEL070130			status_scale.cur_keyvalue  = status_scale.cur_keyvalue * KeyDrv.CnvCode;
		//DEL070130		} else {
		//DEL070130			status_scale.cur_keyvalue  = status_scale.cur_keyvalue * 10 + KeyDrv.CnvCode;
		//DEL070130		}
		//DEL070130#ifdef TEST_99EA_LABEL	// Added by CJK 20051115
		//DEL070130		status_scale.cur_keyvalue%= 100;
		//DEL070130#else
		//DEL070130		status_scale.cur_keyvalue%= 1000;
		//DEL070130#endif
		//DEL070130		status_scale.print_multiple =status_scale.cur_keyvalue;
		KeyDrv.FirstKey = 0;
		sale_display_update(UPDATE_TITLE|UPDATE_PRICE);
		return;
	}
	if (DiscSale.discIndexData.discTargetType == DISC_TARGET_N_PLUS_1 && DiscSale.discIndexData.discPriceType == DISC_N_PLUS_1 && PluType()==PLU_BY_WEIGHT)
	{
		multiple = (INT32U)status_scale.cur_qty;
		NumericValue(1000, &multiple);
		status_scale.cur_qty = (INT16S)multiple;
		KeyDrv.FirstKey = 0;
		sale_display_update(UPDATE_TITLE|UPDATE_PRICE);
		return;
	}
	
	if(Operation.operationClerk != OPER_NORMAL && status_scale.cur_pluid && !Operation.override)
	{
		if(KeyDrv.FirstKey)  HiddenClerkNumber = 0;
		HiddenClerkNumber  = HiddenClerkNumber * 10 + KeyDrv.CnvCode;
		if(99<HiddenClerkNumber) HiddenClerkNumber=0;
	}
	if(!status_scale.ride_max && !InputQuickPluState)//(!Operation.override && status_scale.cur_pluid)
	{
		ret=get_global_bparam(GLOBAL_SALE_SETUP_FLAG3); //Param 573, 직접단가변경허용
#ifdef USE_LOTTEMART_DEFAULT
		if ( (!(ret&0x08)) && (!OnlineFlag) )
		{
				return;
		}
#elif defined(USE_PRICE_CHANGEABLE_CHECK)
		plu_table_search((INT8U)status_scale.departmentid, status_scale.cur_pluid,&plu_addr,0);
		plu_get_field(plu_addr-1,  PTYPE_TAX_NO, (INT8U *)&priceChangeFlag,(INT16S *)&size,sizeof(priceChangeFlag));
		if (!(ret&0x08))
		{
			sprintf(string_buf,"가격변경 제한");
			display_message_page_mid_delay((char *)string_buf,10);
			return;
		}
		if(priceChangeFlag)
		{
			sprintf(string_buf,"가격변경 제한");
			display_message_page_mid_delay((char *)string_buf,10);
			return;
		}
#elif defined(USE_PRICE_CHANGE_PROHIBITED) // P573에서의 세팅값과 상관없이 막도록 함.
		PressedNumberKey_1000Digits = PressedNumberKey_1000Digits * 10;
		PressedNumberKey_1000Digits = PressedNumberKey_1000Digits % 1000;
		PressedNumberKey =  PressedNumberKey_1000Digits + KeyDrv.CnvCode;
		PressedNumberKey_1000Digits = PressedNumberKey;
		return;
#else
		if ( !(ret&0x08) )
		{
  #ifdef USE_GSMART_TRACE_FUNCTION    //GS의 경우 광고문, 가공일변경키 처리를 위해 
            PressedNumberKey = KeyDrv.CnvCode;	//573 'N'시엔 UnitPrice 창에 입력한 숫자 표시안됨. 
            //광고문키 사용시 광고 1~9까지만 사용하도록 제한함.
  #endif
			return;
		}
#endif
 	}
	
	if(KeyDrv.PrevType != KEY_TYPE_NUMERIC)
	{
		KeyDrv.FirstKey = 1;
		//KeyDrv.LastKeyTime = SysTimer100ms;
	}
	if(!KeyDrv.FirstKey)
	{
		if ((INT16U)(SysTimer100ms - KeyDrv.LastKeyTime) > GlbOper.keyClearTime) KeyDrv.FirstKey = 1;
	}
	KeyDrv.LastKeyTime = SysTimer100ms;// + GlbOper.keyClearTime;	//OPTION: key time clear 3초

#ifdef USE_SET_QUICK_PLU_DELAY_TIME
	if (InputQuickPluState && !status_scale.cur_pluid)
#else
	if (InputQuickPluState)
#endif
	{
		//CallQuickPlu = ON;
		ReadyQuickPlu = ON;
		NumericValue(Operation.MaxUnitPrice, &status_scale.cur_quickplu);
		QuickPluKeyTime = SysTimer100ms;
	}
	else
	{
		if (Operation.fixedPrice)
		{
			NumericValue(Operation.MaxUnitPrice, &status_scale.cur_unitprice);
		}
		else
		{
			switch (smode) 
			{
				case  0:
					NumericValue(Operation.MaxUnitPrice, &status_scale.cur_keyvalue);
					break;
				case  PLU_BY_WEIGHT:
					if(Netweight_flag){
						NumericValue(99999, &status_scale.cur_keyvalue);											
						status_scale.Plu.fixed_weight = status_scale.cur_keyvalue;
						status_scale.cur_unitprice = status_scale.Plu.unitprice;
						sale_display_update(UPDATE_WEIGHT|UPDATE_UNIT);
					}
					else
					{
						NumericValue(Operation.MaxUnitPrice, &status_scale.cur_unitprice);					
					}
					break;
				case  PLU_BY_PCS:
					switch (status_scale.cur_ride) {     // By Count
						case 1:
							n = get_global_bparam(GLOBAL_INPUT_BC_QTYLENGTH);
							NumericValue(power10(n), &status_scale.cur_qty);
							break;
						case 2:
							n = get_global_bparam(GLOBAL_INPUT_BC_PCSLENGTH);
							NumericValue(power10(n), &status_scale.cur_pcs);
							break;
						case 3:
							NumericValue(Operation.MaxUnitPrice, &status_scale.cur_unitprice);
							break;
					}
					break;
				case  PLU_BY_COUNT:
					switch (status_scale.cur_ride) {  // nonFood
						case 1:
							n = get_global_bparam(GLOBAL_INPUT_BN_QTYLENGTH);
							NumericValue(power10(n), &status_scale.cur_qty);
							break;
						case 2:
							NumericValue(Operation.MaxUnitPrice, &status_scale.cur_unitprice);
							break;
					}
					break;
			}
		}
	}
	KeyDrv.FirstKey = 0;
	sale_display_update(UPDATE_TITLE|UPDATE_UNIT|UPDATE_PRICE);
}

extern void MsgCheckPermisson(INT16U clerk);
char selfmenu_key[12];
INT8U selfmenu_pnt;

//SG060920
INT8U key_selfmenu(void)
{
	//char password[12];
	char cc;
	INT16S i;
	//INT32U addr;
	INT8U num[10] = {0x7B,0x6A,0x6B,0x6C,0x5A,0x5B,0x5C,0x4A,0x4B,0x4C};
	INT8U ret=1;
	
	if(KeyDrv.PressedModeKey) 
	{
		ret=0;
		switch(KeyDrv.RawCode)
		{
			case 0x3B:
				keyapp_datetime();
				break;
			case 0x3D: // 'Menu' 
				keyapp_menu();
				break;
			case 0x4D:  
				keyapp_zero();		
				break;
			case 0x5D:
				keyapp_auto();
				break;
			case 0x6D: 
				keyapp_feed(ON);
				break;
			case 0x7E:		//PRINT KEY 
			case 0x7D:		//Added by JJANG 20070801(Use Numeric key)
				Selfkey_flag = (~Selfkey_flag)&0x01;
				keyapp_self(Selfkey_flag);					
				break;
			default:
				cc=0;
				for(i=0; i<10; i++)
				{
					if(num[i] ==KeyDrv.RawCode){
						cc = i+'0';
						break;
					} 
				}
				
				if (selfmenu_pnt>=10)  selfmenu_pnt=10;
				if (cc) selfmenu_key[selfmenu_pnt++]=cc;
				if (strncmp(selfmenu_key,"8000",4)==0) {
					keyapp_menu();
				}
				break;		
		}
		
	} 
	else 
	{
		INT16S i;
		selfmenu_pnt=0;
		for(i=0;i<8;i++)
			selfmenu_key[i]=0;
	}
	return ret;
}

/*
 void key_selfmenu(void)
 {
 char password[12];
 char cc;
 INT32U addr;
 
 if(KeyDrv.PressedModeKey) {
 switch (KeyDrv.Type) {
 case KEY_TYPE_ASCII  : 
 case KEY_TYPE_NUMERIC: if (KeyDrv.Type==KEY_TYPE_NUMERIC) {
 cc=KeyDrv.CnvCode - KEY_NUM_0 +'0';
 } else {
 cc=(KeyDrv.CnvCode - KP_ASC_00)&0xff + 0x20;
 }
 if (selfmenu_pnt>=10) {
 selfmenu_pnt=10;
 }
 selfmenu_key[selfmenu_pnt++]=cc;
 break;
 case KEY_TYPE_FUNCTION:
 if (KeyDrv.CnvCode==KS_PRINT) {
 if (selfmenu_key[0]) {
 char okFlag=OFF;
 INT16S i;
 
 //							sscanf(selfmenu_key,"%02d",&Operation.logOperator);
 //HYP 20060412 _huge --> _near for small code size
 //when you use Standard Lib, Must change _near --> _huge pointer
 sscanf(selfmenu_key,"%02d",(HUGEDATA INT16U *)&Operation.logOperator);
 selfmenu_key[selfmenu_pnt]=0;
 if(Operation.logOperator == 0)
 {
 addr  = DFLASH_BASE;
 addr += FLASH_START_PASSWORD;
 Flash_sread(addr,(HUGEDATA INT8U *)password,10);
 password[10]=0;
 okFlag = ON;
 }
 else
 {
 CLERK_STRUCT clerkp;
 if(LoadClerk(&clerkp,Operation.logOperator))
 {
 memcpy(password,clerkp.password,8);
 for (i=8; i; i--)
 {
 if(*(password+i)==0 || *(password+i)==0x20)
 *(password+i)=0x00;
 else
 break;
 }
 
 status_scale.level = clerkp.permissionLevel;// need data
 if(2<status_scale.level)
 okFlag = ON;
 }
 }
 if (okFlag && strncmp(password,selfmenu_key+2,4)==0) {
 keyapp_menu();
 }
 }
 }
 }
 } else {
 INT16S i;
 selfmenu_pnt=0;
 for(i=0;i<8;i++)
 selfmenu_key[i]=0;
 }
 }
 */

//0 : clerk 0
//1 : OK
//2 : stop
INT16S	ClerkLonginByKey(INT16U clerk)
{
	CLERK_STRUCT clerks;
	
	status_scale.clerkid = 0;
	status_scale.level = 0;
	if (MAX_CLERK_NO-1 < clerk) 	return 2;
	if (Operation.operationClerk == OPER_NORMAL)	return 0;
	if (clerk && !LoadClerk(&clerks,clerk))
	{
		caption_split_by_code(0xC810,&cap,0);	//"Clerk not found"
		DisplayMsg(cap.form);
		return 2;
	}
	status_scale.clerkid = clerk;
	status_scale.level = clerks.permissionLevel;
	
	return 1;
}

//Added by JJANG 20080111
INT16U call_clerk(INT16U skeyclerk)
{
	INT16U clerkid;
	INT32U key_addr;
	BuzOn(1);
	key_addr  = DFLASH_BASE + FLASH_KEY_CDTC_AREA;
	key_addr += (skeyclerk-1)*2;
	clerkid    =Flash_wread(key_addr);
	return clerkid;
}

INT16U keyapp_clerk(INT16U skeyclerk)
{
	INT16U clerkid,keyclerk;
	INT32U key_addr;
	
	BuzOn(1);
	
	keyclerk  = skeyclerk-KS_CLERK_01;
	key_addr  = DFLASH_BASE + FLASH_KEY_CDTC_AREA;
	key_addr += keyclerk*2;
	clerkid    =Flash_wread(key_addr);
	//	KEY_SetMode(1);
	//	keyapp_call_clerk(clerkid);
	//	KEY_SetMode(0);
	return clerkid;
}

#ifdef USE_KOR_TRACE
extern KORTRACE_FLAG	KorTrace_Flag;
extern void  network_common_trans(void);
#endif

#ifdef USE_TOPMART_DEFAULT //sewon 170116
void keyapp_QtyInput(void)
{
	long	result;
	char	old;
	INT8U   old_key;
	char    str[36];
	char    char_w;
	CAPTION_STRUCT	cap;
	INT16U backupInputQty;

	if(!status_scale.cur_pluid) {
		BuzOn(2);            
		return;
	}
	if(InputQty == 0) InputQty =1;
	if(InputQty >= 1000)InputQty =1;

	backupInputQty = InputQty;

	old_key = KEY_GetMode();
	KEY_SetMode(1);
	//old=DspStruct.Id1;
	//DspLoadFont1(DSP_MSG_FONT_ID);

	caption_split_by_code(0xC724,&cap,0);
	cap.input_length = 3;
	caption_adjust(&cap, NULL);


	PutString(DSP_MSG_Y, DSP_MSG_X, cap.form, DSP_MSG_FONT_ID, DSP_MSG_MAGY, DSP_MSG_MAGX, MAX_MSG_CHAR);
	char_w = display_font_get_width();

	keyin_escape_clear=1;

	//result = keyin_ulong(KI_GRAPHIC, KI_FORM_NORMAL, decpoint,(long*)&InputQty, cap.input_length, DSP_MSG_Y, DSP_MSG_X+cap.input_x*char_w, 0,KI_NOPASS );
	result = keyin_ulong(KI_GRAPHIC, KI_FORM_NORMAL, 0,(long*)&InputQty, cap.input_length, DSP_MSG_Y, DSP_MSG_X+18*char_w, 0,KI_NOPASS );

	keyin_escape_clear=0;
	keyin_clearcallback();
	//DspLoadFont1(old);

	sale_display_update(0x0fff);	

	if (result==KP_ENTER || result==KP_SAVE)
	{
    	InputQtyFlag = ON;
	}	

	if (result==KP_CLEAR || result==KP_ESC)
	{
		InputQty = backupInputQty;
	}
	KEY_SetMode(old_key);

}

void keyapp_onoff_packdate(void)
{
	INT8U uctemp, uctemp2;
	
	if(PackdatePrintFlag == 1)
	{
		DisplayMsg("PACKDATE-PRINT OFF");
		PackdatePrintFlag = 0;
	}
	else 
	{
		DisplayMsg("PACKDATE-PRINT ON");
		PackdatePrintFlag = 1;
	} 
}
#endif

#ifdef USE_LOTTEMART_MEATSCANNER
void lottemart_meatscanner_initial(void)
{
	CurTraceStatus.indivno[0] = 0;
	CurTraceStatus.slPlace1 = 0;
	CurTraceStatus.slPlace2 = 0;
	CurTraceStatus.slPlace3 = 0;
	CurTraceStatus.meatgradeNo = 0;
	CurTraceStatus.meatscannerflag = 0;
	CurTraceStatus.meatUseNo = 0;
	CurTraceStatus.no14indivflag = 0;
}
#endif

extern void PS2_Write(INT16U code);
extern void ProcessAll(void);
extern void keypp_dividing_function(INT8U keynum);
void key_function(void)
{
	INT16S   uid;
	INT8U	pmsKind,loginClerk,loginNetwork,processFlag;
	INT8U	loginChk, changeClerk;
	INT16U	clerk;
	INT8U login_ret;
	INT16S ret;
	long tare_value;
	INT16S process_ret = ON;
#ifdef USE_EMART_NETWORK
	INT8U ret_value = 0;
#endif
#ifdef USE_TOPMART_DEFAULT
	INT8U uctemp, uctemp2;
#endif
	//INT8U	chg_flag;
	INT8U clerkKeyAction, i;
#ifdef USE_FIXED_PERCENT_KEY_SET
	DISCOUNT_KEY_STRUCT discKey;
	INT32U	addr;
	INT8U tempDiscRate[5];
#endif
	processFlag = OFF;
	pmsKind = 0;
	loginClerk = OFF;
	loginNetwork = OFF;
	
	loginChk = OFF;
	changeClerk= OFF;
	
	Operation.keyClear = ON;
	clerk = 0;
	
	//chg_flag = get_global_bparam(GLOBAL_SALE_SETUP_FLAG8) & 0x40;	//parameter 723
	
	BuzOn(1);

#ifdef USE_FUNCTION_KEY_BLOCK
	for(i=0; i<18; i++)
	{
		if (KeyOnoff[i].keycode == KeyDrv.RawCode)
		{
			if(KeyOnoff[i].flag == ON)
			{
				KeyDrv.CnvCode = 0;
			}
			break;
		}
	}
#endif
	
	if(KeyDrv.Type == KEY_TYPE_CLERK)// 할당 clerk key
	{
		if(PrtStruct.Mode == LABEL)
		{
			clerkKeyAction = get_global_bparam(GLOBAL_SALE_SETUP_FLAG11);
			clerkKeyAction = (clerkKeyAction & 0x30) >> 4;	//0011 0000
			switch(clerkKeyAction)
			{
				case CLERK_KEY_DISABLED:
					caption_split_by_code(0x9352,&cap,0);	//"label mode"
					DisplayMsg(cap.form);
					BuzOn(2);
					return;
				case CLERK_KEY_ADD_FUNCTION:
				default:
					clerk = keyapp_clerk(KeyDrv.CnvCode);
					KeyDrv.CnvCode = KS_CLERK_CALL;
					break;
			}
		}
		else
		{
			clerk = keyapp_clerk(KeyDrv.CnvCode);
			KeyDrv.CnvCode = KS_CLERK_CALL;
		}
	}
	
	if (KeyDrv.CnvCode==KS_DATETIME) {				  //Doubleclick for edit datetime se-hyung 20070820
		KeyDrv.CnvCode=keyapp_doubleplukey(KeyDrv.CnvCode, OFF);	  //Doubleclick for edit datetime se-hyung 20070820	
	}
	if (KeyDrv.CnvCode==KS_EDT_SELL) {
		KeyDrv.CnvCode=keyapp_doubleplukey(KeyDrv.CnvCode, OFF);
	}
	switch(KeyDrv.CnvCode) {
		case KS_DEPT_CALL :
			pmsKind = PMS_KEY_DEPT;
			break;
		case KS_CLERK_CALL://번호입력 clerk
//#ifdef USE_QR_ADDTOTAL
//			AddSaleFlag = 1;
//#endif
			if(KeyDrv.Type == KEY_TYPE_CLERK)// 할당키
				pmsKind = PMS_CLERK_KEY_SALES;
			else
				pmsKind = PMS_CLERK_SALES;
			
			if(!clerk && status_scale.cur_pluid)
			{
				if(HiddenClerkNumber)
				{
					clerk = HiddenClerkNumber;
					HiddenClerkNumber = 0;
				}
				else
					clerk = 0;
			}
			if(!clerk && !status_scale.cur_pluid)
				clerk = (INT16U)status_scale.cur_keyvalue;
			
			/* deleted by JJANG 20080512  CLERK CALL 
			 if (!clerk || MAX_CLERK_NO-1 < clerk)
			 {
			 pmsKind = PMS_CLERK_LOGIN;
			 break;
			 }
			 */
			if(status_scale.clerkid != clerk) changeClerk = ON;	//Added by JJANG 20080328
			process_ret = ClerkLonginByKey((INT16U)clerk);
			if(Operation.operationClerk == OPER_FLOATING ) loginNetwork = ON;
			if(Operation.operationClerk == OPER_STD_CLERK) loginClerk   = ON;
			break;
		case KS_OVERRIDE  :
			if (!status_scale.cur_pluid) goto NOOPER;
			pmsKind = PMS_OVERRIDE;
			break;
		case KS_ADD	  :
		case KS_EXT_ADD_PRINT:
			pmsKind = PMS_ADDUP;
#ifdef USE_QR_ADDTOTAL
			if(QrAddTotalFlag)
			{
//				AddSaleFlag = 1;
				QrAddCount = get_nvram_bparam(NVRAM_QR_ADD_COUNT);
				if(QrAddCount > MAX_QR_ADDCOUNT) break;
				else 
				{
					QrAddCount++;
					set_nvram_bparam(NVRAM_QR_ADD_COUNT, QrAddCount);
				}
			}
#endif
			if(Operation.operationClerk == OPER_FLOATING ) loginNetwork = ON;
			if(Operation.operationClerk == OPER_STD_CLERK) loginClerk   = ON;
			break;
		case KS_VOID	  :
		case KS_LASTVOID  :
			if(Operation.operationClerk == OPER_NORMAL) {
				if (keyapp_sale_status(0)==0) {
					DisplayMsg((char *)global_message[MSG_NOSALEDATA]);
					goto NOOPER;
				}
			}
			pmsKind = PMS_VOID;
			if(Operation.operationClerk == OPER_FLOATING)
				loginNetwork = ON;
			if(Operation.operationClerk == OPER_STD_CLERK)
				loginClerk = ON;
			break;
		case KS_PRINT:
			if(Operation.operationClerk == OPER_NORMAL) break;
		case KS_GTOTAL:
			if ((Operation.operationClerk != OPER_NORMAL) && (KeyDrv.CnvCode==KS_GTOTAL)) goto NOOPER;
		case KS_TTL   :
			if(Operation.operationClerk == OPER_NORMAL) {
				if (keyapp_sale_status(2)==0) {
					DisplayMsg((char *)global_message[MSG_NO_TOTAL]);
					goto NOOPER;
				}
			}
			pmsKind = PMS_ADDUP;
			if (Operation.operationClerk == OPER_FLOATING ) loginNetwork = ON;
			if (Operation.operationClerk == OPER_STD_CLERK) loginClerk   = ON;
			break;
		case KS_RETURN	:
			uid = get_global_bparam(GLOBAL_SALE_SETUP_FLAG2);
			if (uid&0x04) {
				processFlag=ON;
				goto RUN_FUNC;
			}
			if (!status_scale.cur_pluid) goto NOOPER;
			if(Operation.operationClerk != OPER_NORMAL)	loginClerk = ON;
			pmsKind = PMS_RETURN;
			if(Operation.operationClerk == OPER_FLOATING)
				loginNetwork = ON;
			if(Operation.operationClerk == OPER_STD_CLERK)
				loginClerk = ON;
			
			break;
		case KS_PAY	:
			pmsKind = PMS_KEY_PAY;
			if(Operation.operationClerk == OPER_FLOATING)
				loginNetwork = ON;
			if(Operation.operationClerk == OPER_STD_CLERK)
				loginClerk = ON;
			break;
		case KS_TARE:
			if (TareOper.addupWeightZero && Operation.operationClerk == OPER_FLOATING)
			{
				if(Operation.operationClerk == OPER_FLOATING)
					loginNetwork = ON;
				if(Operation.operationClerk == OPER_STD_CLERK)
					loginClerk = ON;
			}
			pmsKind = PMS_KEY_TARE;
			break;
		case KS_TYPE:
			if (!status_scale.cur_pluid) goto NOOPER;
			pmsKind = PMS_KEY_TYPE_CHANGE;
			break;
		case KS_CASHOPEN:
			pmsKind = PMS_OPENCASH;
			loginClerk = ON;
			break;
		case KS_INWEIGHT:
		case KS_NETWEIGHT:			
			pmsKind = PMS_KEY_IN_WEIGHT;
			break;
		case KS_EDT_PACKDATE:
			pmsKind = PMS_KEY_DATE_CHANGE;
			break;
		case KS_EDT_SELL:
			if (!status_scale.cur_pluid) goto NOOPER;
			pmsKind = PMS_KEY_DATE_CHANGE;
			
			/*
			 if (status_scale.cur_pluid){
			 pmsKind = PMS_KEY_DATE_CHANGE;
			 }else {
			 keyapp_onoff_printdate();
			 goto NOOPER;
			 }
			 */
			break;
		case KS_EDT_ORIGIN:
			if (!(status_scale.restrict&FUNC_ORG_TBL)) goto NOOPER;
		case KS_EDT_ITEM  :
			if (!status_scale.cur_pluid) goto NOOPER;
			pmsKind = PMS_PLU_ITEM_CHANGE;
		case KS_EDT_PRICE :
			if (!status_scale.cur_pluid) goto NOOPER;
			pmsKind = PMS_PLU_PRICE_CHANGE;
		 	break;
		case  KS_EDT_DISCOUNT:
			if (!status_scale.cur_pluid) goto NOOPER;
			//pmsKind = PMS_KEY_DISCOUNT;
			pmsKind = PMS_KEY_EDIT_DISC;
		 	break;
		case KS_SALE_PER:
		case KS_MINUS:
		case KS_SALE_SPECIAL:
			pmsKind = PMS_KEY_DISCOUNT;
			break;
#ifdef USE_EDIT_TOTALPRICE
		case KS_EDIT_TOTALPRICE:
			pmsKind = PMS_EDIT_PRICE;
			break;
#endif
#ifdef USE_TOPMART_DEFAULT //sewon 170118
		case KS_SALE_PRICE_CHG_UNITPRICE:
			pmsKind = PMS_EDIT_PRICE;
			break;
#endif
		case KS_SAVE:					//Added by JJANG 20080131
			pmsKind = PMS_KEY_SAVE;
			break;
		case KS_CUSTOMER_NO:
			if (status_scale.cur_pluid) goto NOOPER;
			pmsKind = PMS_CUSTOMER_NO;
			if(Operation.operationClerk == OPER_FLOATING)
				loginNetwork = ON;
			if(Operation.operationClerk == OPER_STD_CLERK)
				loginClerk = ON;
			break;
		case KS_CUSTOMER_CLOSE:
			if (status_scale.cur_pluid) goto NOOPER;
		case KS_REOPEN:
			if (status_scale.cur_pluid) goto NOOPER;
			pmsKind = PMS_REOPEN_TICKET;
			if(Operation.operationClerk == OPER_FLOATING)
				loginNetwork = ON;
			if(Operation.operationClerk == OPER_STD_CLERK)
				loginClerk = ON;
			break;
		case KS_COPY_TICKET:
			if (status_scale.cur_pluid) goto NOOPER;
			pmsKind = PMS_REOPEN_TICKET;
			if(Operation.operationClerk == OPER_FLOATING)
				loginNetwork = ON;
			if(Operation.operationClerk == OPER_STD_CLERK)
				loginClerk = ON;
			break;
#ifdef USE_NHMART_SAFE_MEAT
		case KS_EDIT_INDIVIDUAL:
		case KS_OFFLINE:
			if(checkMtrace)
			{
				pmsKind = PMS_DATA_TABLE2;
			}
			else
			{
				pmsKind = 0;	//offline 모드 사용시에는 password 안 물어봄.
			}
			break;
#endif
#ifdef USE_FIXED_PERCENT_KEY
		case KS_FIXED_5PER :
		case KS_FIXED_10PER :
		case KS_FIXED_20PER :
		case KS_FIXED_30PER :
		case KS_FIXED_40PER :
		case KS_FIXED_50PER :
			pmsKind = PMS_KEY_DISCOUNT;
			break;
#elif defined(USE_FIXED_PERCENT_KEY_SET)
		case KS_FIXED_10PER :
			addr = DFLASH_BASE + FLASH_DISCOUNT_RATE_AREA;
			Flash_sread(addr, (HUGEDATA INT8U*)&discKey, sizeof(DISCOUNT_KEY_STRUCT));
			pmsKind = PMS_KEY_DISCOUNT;
			break;
		case KS_FIXED_20PER :
			addr = DFLASH_BASE + FLASH_DISCOUNT_RATE_AREA + 2;
			Flash_sread(addr, (HUGEDATA INT8U*)&discKey, sizeof(DISCOUNT_KEY_STRUCT));
			pmsKind = PMS_KEY_DISCOUNT;
			break;
		case KS_FIXED_30PER :
			addr = DFLASH_BASE + FLASH_DISCOUNT_RATE_AREA + 4;
			Flash_sread(addr, (HUGEDATA INT8U*)&discKey, sizeof(DISCOUNT_KEY_STRUCT));
			pmsKind = PMS_KEY_DISCOUNT;
			break;
		case KS_FIXED_40PER :
			addr = DFLASH_BASE + FLASH_DISCOUNT_RATE_AREA + 6;
			Flash_sread(addr, (HUGEDATA INT8U*)&discKey, sizeof(DISCOUNT_KEY_STRUCT));
			pmsKind = PMS_KEY_DISCOUNT;
			break;
		case KS_FIXED_50PER :
			addr = DFLASH_BASE + FLASH_DISCOUNT_RATE_AREA + 8;
			Flash_sread(addr, (HUGEDATA INT8U*)&discKey, sizeof(DISCOUNT_KEY_STRUCT));
			pmsKind = PMS_KEY_DISCOUNT;
			break;
#endif
		case KS_SALE_NEG:
			pmsKind = PMS_NEGATIVE_SALE;
			break;
		case KS_EDIT_STORE :
			pmsKind = PMS_STORE_DATA;
			break;
		default :
			break;
	}
	//Modified by JJANG 20080604
	//PLU 호출 후 Clerk key를 눌러서 상품 판매시, clerk이 없을 때 ADD되는 현상 수정   
	processFlag = ON;
	if (process_ret==2) processFlag = OFF;
	/*
	 if(!status_scale.clerkid && (loginNetwork||loginClerk))	pmsKind = PMS_CLERK_LOGIN;
	 if(pmsKind) {
	 INT16U	prevCode = KeyDrv.CnvCode;
	 if (!permission_check(pmsKind,0)) processFlag=OFF; // if not permission, then stop the process
	 KeyDrv.CnvCode = prevCode;
	 }
	 */
	//Modified by JJANG 20080328
	if(!status_scale.clerkid && (loginNetwork||loginClerk)) loginChk = ON;
	if(changeClerk) loginChk |= 0x80;
	if(pmsKind) 
	{
		INT16U	prevCode = KeyDrv.CnvCode;
		if (!permission_check(pmsKind,loginChk)) processFlag=OFF; // if not permission, then stop the process
		KeyDrv.CnvCode = prevCode;
	}
	if(pmsKind && processFlag) {
		if(status_scale.clerkid) {
			login_clerk(status_scale.clerkid);
			if(loginNetwork) {
				//if (!network_login(status_scale.clerkid,0x01)) goto NOOPER;
				login_ret = network_login(status_scale.clerkid, status_scale.scaleid, 0x01);
				if (login_ret == 0)	// clerk lock 
				{
					logout();
					goto NOOPER; 
				}
				else if (login_ret == 2)	// clerk lock 해제
				{
					NetClerkLockRelease(status_scale.clerkid);
					goto NOOPER;
				}
				else if (login_ret == 3)	// timeout
				{
					logout();	//20090902
					goto NOOPER;
				}
			}
			ClerkLoginTime = SysTimer100ms;
			
			if (KeyDrv.CnvCode == KS_CLERK_CALL)	// CJK070319
			{	
				if (TareOper.addupWeightZero && Operation.operationClerk == OPER_FLOATING)
				{
					tare_value = GetClerkTare(status_scale.clerkid);	
					keyapp_tare(TARE_TYPE_PRESET, tare_value, OFF);
					ProcessAll();
				}
				Pageupdown = 0;			//Insert by se-hyung for Clerk Subtotal List 20070727
				DisplayClerkTotalInfo(status_scale.clerkid);
			}
		}
		//if(Operation.operationClerk!=OPER_NORMAL && !status_scale.clerkid)	processFlag = OFF;
	}
	if(processFlag && Operation.operationClerk==OPER_NORMAL && status_scale.clerkid) status_scale.clerkid = 0;
	RUN_FUNC:
			if(processFlag) {
				switch(KeyDrv.CnvCode) {
					case KS_PRINT : 
#ifdef USE_EMART_NETWORK
						if (Operation.checkin)
						{
							if(status_scale.cur_pluid == 999999)	//이마트 차세대2012, PLU 999999를 마스터 업데이트 정보 저장에 사용. 생산등록X
							{
								Operation.checkin = 0;
  #ifdef USE_EMART_PRODUCTION_KEY
								BackupCheckin = 1;
  #endif
							}
							else
							{
								ret_value = GetCheckAddTransactionData(0,1,AUDIT_MODE_CIRCULAR);		//mode : sale, count : 1, Circular
								if (!ret_value)	goto NOOPER;
							}
						}
#endif

#ifdef USE_KOR_TRACE
						if(!keyapp_ktr_print()) break;
#elif defined(USE_SUNWOO_POS_DEFAULT)
						if(!Operation.productWorkStatus || Operation.productWorkStatus == PLU_LOSS) // 입고등록, 도매, 소매 기능키 눌러야 라벨 발행 가능 (LOSS 등록은 라벨 발행 하지 않음)
						{
							BuzOn(2);
							sprintf(string_buf, "기능키 선택");
							display_message_page_mid_delay((char *)string_buf,10);
							break;
						}
						else
							keyapp_print(OFF);
#else
  #ifdef USE_TOPMART_DEFAULT
						if(Operation.fixedPrice == 1)
						{
							keyapp_fixed_price(OFF,0); 
							break;
						}
  #endif
						keyapp_print(OFF);
#endif
						break;
					case KS_FEED  :	keyapp_feed(OFF);  break;		// Print Feed
					case KS_ZERO  : keyapp_zero();
#ifdef USE_ZERO_RANGE_MSG
						Zerokey_input_Flag = ON;
#endif
						break;
#ifdef USE_VIETNAM_EMART_DEFAULT
					case KS_UNUSE : BuzOn(2);break;
#endif
#ifdef USE_TOPMART_DEFAULT //sewon 170116
					case KS_INPUTQTY  : keyapp_QtyInput();break;
					case KS_PACKDATE_FLAG : keyapp_onoff_packdate(); break;
					case KS_ASK_INDIVIDUAL :keyapp_ask_indivInfo(CalledTraceStatus.indivStr.individualNO); break;
#endif
					case KS_TARE  : 
						if(!ADM_GetData.Stable)
						{
							//BuzOn(1);
							caption_split_by_code(0xE003,&cap,0);	// Unstable
							DisplayMsg(cap.form);
							break;
						}
						ret = keyapp_tare(0,status_scale.cur_keyvalue,ON);
						if (TareOper.addupWeightZero && Operation.operationClerk == OPER_FLOATING)
						{
							if (ret)
							{
								SetClerkTare(status_scale.clerkid, TareOper.tareWeight + TareOper.tareKey);
							}
						}
						break;
					case KS_CLEAR    : 
#ifdef USE_LOTTEMART_MEATSCANNER //20171206
						lottemart_meatscanner_initial();
#endif
						keyapp_clear();
						break;
					case KS_MODE     :
						//Start Modify by se-hyung for Clerk Subtotal List 20070727
						if (Clerk_Subtotal_List == 1)		  
						{
							if (Pageupdown > 0)Pageupdown--;
							else Pageupdown = 0;
							sale_display_update(UPDATE_SUBTOTAL);
						}
						//End Modify by se-hyung for Clerk Subtotal List 20070727
#ifdef USE_SETPRODUCT_SALE
					else if(SetproductSaleMode)	BuzOn(2);
#endif
#ifdef USE_HYUNDAI_SET_FUNCTION
					else if(MeatSetSaleMode) BuzOn(2);
#endif
#ifdef USE_REGIST_DISPOSAL
					else if(Operation.RegistDisposalFlag) BuzOn(2);
#endif
					else keyapp_menu();
						goto NOCLEAR;
					case KS_NUMBER_00: keyapp_nzero(); goto NOCLEAR;
					case KS_SAVE     : keyapp_save(); break;
					case KS_AUTO     : keyapp_auto(); break;
					case KS_PREPACK  : keyapp_prepack(); break;
					case KS_UNIT1    : 
					case KS_UNIT2    :
					case KS_UNIT3    :
					case KS_UNIT4    : uid=KeyDrv.CnvCode-KS_UNIT1+1;
					keyapp_unitchange(uid);
					break;
					case KS_TARE_CALL :keyapp_call_tare(status_scale.cur_keyvalue); break;
					case KS_DEPT_CALL :keyapp_call_department(status_scale.cur_keyvalue,0); break;
					case KS_CLERK_CALL:
						KEY_SetMode(1);
						keyapp_call_clerk();
						KEY_SetMode(0);
#ifdef USE_CL5200_DISPLAY_DEFAULT	// CL5200 bug fix - sale error 표시 후, plu 이름 사라지는 버그 수정.
						sale_display_update(UPDATE_CENTER);
#endif
						break;
					case KS_OVERRIDE  :keyapp_override(); break;
					case KS_SAVEPRICE :keyapp_save_price(); break;  //SG060227 _save unit price (내수)

					case KS_EXT_ADD_PRINT:
						FlagExternalPrtAddUp = ON;
					case KS_ADD	  :
#ifdef USE_QR_ADDTOTAL
						if(QrAddTotalFlag)
						{
							if(QrAddCount > MAX_QR_ADDCOUNT)
							{
								caption_split_by_code(0xC90D,&cap,0);
								DisplayMsg(cap.form);
//								if(PrtStruct.Mode == LABEL)     AddSaleFlag = 0;
								BuzOn(2);
								break;
							}
						}
#endif
						if (Clerk_Subtotal_List == 1) {
							Pageupdown++; //Modify by se-hyung for Clerk Subtotal List 20070727
							sale_display_update(UPDATE_SUBTOTAL);
						}			
						else keyapp_add(OFF);
						break;
					case KS_VOID	  :
						KEY_SetMode(1);
						keyapp_void();
						KEY_SetMode(0);
						break;
					case KS_LASTVOID:
						keyapp_lastvoid();
						break;
					case KS_TTL     :
						keyapp_st_ttl();
						break;
					case KS_GTOTAL  :
						keyapp_g_total();
						break;
					case KS_RETURN	:
						//KEY_SetMode(1);
						keyapp_return();
						//KEY_SetMode(0);
						break;
					case KS_PAY		:
						keyapp_pay(OFF);
						break;
					case KS_PAY_1	:
					case KS_PAY_2	:
					case KS_PAY_3	:
					case KS_PAY_4	:
					case KS_PAY_5	:
					case KS_PAY_6	:
					case KS_PAY_7	:
						uid = KeyDrv.CnvCode-KS_PAY_1+1;
						keyapp_pay(uid);
						break;
					case KS_SALE_NEG  : //Added by YYC 20110520
						if (Clerk_Subtotal_List == ON)
						{
							Pageupdown++; //Modify by se-hyung for Clerk Subtotal List 20070727
							sale_display_update(UPDATE_SUBTOTAL);
						}	
						else keyapp_sale_negative();
						break;
					case KS_SALE_PER  :
						//KEY_SetMode(1);
						keyapp_sale_percent();
						//KEY_SetMode(0);
						break;
					case KS_SALE_SPECIAL:
						keyapp_sale_specialprice();
						break;
					case KS_SALE_SPC  :keyapp_sale_special(); break;
#ifdef USE_TOPMART_DEFAULT //sewon 170117
					case KS_SALE_PRICE_CHG_UNITPRICE  :keyapp_sale_price_change_unitprice(); break;
#endif
#ifdef USE_EDIT_TOTALPRICE
					case KS_EDIT_TOTALPRICE :keyapp_sale_edit_totalprice(); break;
#endif
					case KS_MINUS:
						//KEY_SetMode(1);
						keyapp_minus_disc();	// minus discount
						//KEY_SetMode(0);
						break;
#ifdef USE_FIXED_PERCENT_KEY
					case KS_FIXED_5PER :
						keyapp_sale_fixedpercent(5);
						break;
					case KS_FIXED_10PER :
						keyapp_sale_fixedpercent(10);
						break;
					case KS_FIXED_20PER :
						keyapp_sale_fixedpercent(20);
						break;
					case KS_FIXED_30PER :
						keyapp_sale_fixedpercent(30);
						break;
					case KS_FIXED_40PER :
						keyapp_sale_fixedpercent(40);
						break;
					case KS_FIXED_50PER :
						keyapp_sale_fixedpercent(50);
						break;
#elif defined(USE_FIXED_PERCENT_KEY_SET)
					case KS_FIXED_10PER :
					case KS_FIXED_20PER :
					case KS_FIXED_30PER :
					case KS_FIXED_40PER :
					case KS_FIXED_50PER :
						keyapp_sale_fixedpercent(discKey.rate);
						break;
#endif            
					case KS_FIXED:
						keyapp_fixed_price(OFF,0);
						break;
					case KS_TYPE:
						keyapp_type(ON);
						break;
					case KS_CASHOPEN:
						keyapp_cash_open(ON);	// Modified by CJK 20051110
						break;
					case KS_BATCH:
						keyapp_batch(OFF);
						break;
#ifndef USE_PLU_MULTIPLE_PRICE	//Refefence PLU 기능 사용 안 함
					case KS_REFERENCE:
						//keyapp_onoff_printdate(); //SG060510 for PUSAN Nonghyup 
#ifndef USE_AUSTRALIA_COOL_FUNCTION //Refefence PLU 기능 사용 안 함	
						keyapp_reference();
#endif
						break;
#endif
					case KS_PRINTDATE:		//SG060512
#ifdef USE_EMART_DEFAULT
						if(!(get_global_bparam(GLOBAL_SALE_SETUP_FLAG14) & 0x04))  //csh 20140226
						{
							keyapp_onoff_printdate();
						}
#else
						keyapp_onoff_printdate();
#endif
						break;
					case KS_INWEIGHT:
						keyapp_in_weight();
						break;
					case KS_NETWEIGHT:
						Netweight_flag=ON;
						keyapp_override();
						break;
						
					case KS_LABELTICKET:
						//keyapp_chg_prtmode();
						keyapp_change_salemode(); //SG070223
						break;
					case KS_MULTI    : keyapp_multi(); break;
					case KS_PLU      :
						//			if (status_scale.cur_pluid) {
						//				break;
						//			}
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
							set_global_bparam(GLOBAL_SALE_SETUP_FLAG6, uctemp);
						} 
#endif
						SpeedKeyPressed = 1; //csh 20140313
#ifdef USE_LOTTEMART_MEATSCANNER //20171206
						lottemart_meatscanner_initial();
#endif
#ifdef USE_HYUNDAI_SET_FUNCTION
						if(MeatSetSaleMode && MeatSetSaleUseScanner) {
							BuzOn(2);
							break;
						}
						else
#endif
						keyapp_pluno(0,status_scale.cur_keyvalue,OFF); 
						//Added by JJANG 20070803 셀프모드에서 개수 상품 발행시 자동 발행
						if((Operation.operationMode == MODE_SELF) && (KeyDrv.CnvCode == KS_PLU)) SelfserviceAutoCheck();	
						break;
					case KS_FOR         : keyapp_for(); break;
					case KP_CAPS	    : 
					case KEY_SHIFT      : keyapp_shift(); break;
					case KS_LEDON       : keyapp_led();            break;
					case KS_DATETIME    :
						keyapp_datetime();	
#ifndef _USE_LCD32_
						DateTimeDispTime = SysTimer100ms;
#endif
						break;
					case KS_MISC_WEIGHT : keyapp_misc_byweight(OFF);  break;
					case KS_MISC_PCS    : keyapp_misc_bypcs(OFF);   break;
					case KS_MISC_COUNT  : keyapp_misc_bycount(); break;
					case KS_EDT_SELL    : keyapp_edit_sellbydate(); break;
					case KS_EDT_PACKDATE: keyapp_set_packeddate(); break;
					case KS_EDT_ORIGIN  : keyapp_edit_origin(); break;
					case KS_EDT_ITEM    : keyapp_edit_item();   break;
					case KS_EDT_PRICE   : keyapp_edit_price(); break;
					//case KS_EDT_PRICE : keyapp_save_price(); break;  //SG20060227  for Test
					case KS_PACKDATE : keyapp_edit_packdate(); break;  //SG20060309
#ifdef USE_SETPRODUCT_SALE
					case KS_MODE_SETPRODUCT: keyapp_setproduct_sale(); break;
					case KS_SETPRODUCT_CLEAR: keyapp_setproduct_clear(); break;
					case KS_NONWEIGHING_LABLE: keyapp_nonweighing_mode(); break;
#endif
					case KS_EDIT_DATETIME:
						keyapp_edit_date();
						////			Dsp_ChangeMode(DSP_PGM_MODE); 
						////			keyapp_date(0);
						////			Dsp_ChangeMode(DSP_SALE_MODE);
						////			sale_pluclear(OFF);
						////			//display_lcd_clear();
						////			sale_display_update(UPDATE_ALL|UPDATE_CLEARALL);
						break;
					case  KS_EDT_DISCOUNT: keyapp_edit_discount(); break;
					case  KS_EDT_DELPLU  : keyapp_edit_delplu(); break;
					case  KS_EDT_SPEEDKEY: keyapp_edit_speedkey(); break;
					case  KS_CHESS		 : keyapp_chess_print(); break;
					case  KS_TEST_PRINT  : keyapp_test_print(); break;
					case  KS_KGLB 	     : keyapp_toggle_weightunit(); break;
#ifdef USE_TAX_RATE        
					case  KS_EDT_TAX     : keyapp_edit_tax(); break;		//Added by JJANG 20070619
					case  KS_GLOBAL_TAX	 : keyapp_apply_global_tax(); break;		//PLU에 Global tax 임시 적용 키
#endif        
#ifdef USE_TRACEABILITY
					case  KS_EDT_TRACE   : keyapp_edit_trace(); break; 	//insert by se-hyung for function traceability key 20070706
#endif
					case  KS_TICKET_NO   : keyapp_set_ticketnumber(); break;  
					case  KS_SCAN_CLERK  : keyapp_set_scan_clerk();	break;
					case  KS_BARCODE     : kayapp_input_barcode(); break;
					case  KS_CUSTOMER_NO : kayapp_input_customer_no(); break;
					case  KS_CUSTOMER_CLOSE : kayapp_customer_close(); break;
					case  KS_REOPEN : kayapp_reopen_ticket(); break;
					//		case  KS_CLOSE_TICKET : kayapp_close_ticket(); break;
					case  KS_COPY_TICKET : 
#ifdef USE_COPYKEY			
						flagcopykey= ON;
#endif
						flagcopy_ticket = ON;
						keyapp_copy_ticket();
						break;
#ifdef USE_TRACE_STANDALONE
					case  KS_NONE_INDIV :  keyapp_trace_noneid(); break;
					//Added by JJANG 20080626
					case  KS_EDIT_INDIVIDUAL : 
						if (FlagDisplayIndividual)
						{
							individualData_create(0);
						}
						else
						{
							individualData_create(1);
						}			
						break;		
					case  KS_CALL_INDIVIDUAL : 
						individualData_call();	//개체인덱스 호출키 
						break;
					case  KS_INPUT_INDIVIDUAL :
						individualNo_input();	//개체번호 입력키			
						break;
					case  KS_INPUT_LOT :
						lotNo_input();		//묶음번호 입력키
						break;
					case  KS_TRACE_SAVETYPE :
						keyapp_check_trace();	//개체저장옵션 설정키
						break;
//					case  KS_BIND_INDIVIDUAL :
//						individualNo_bind();	//중복개체 입력키
//						break;
					case  KS_CALL_SLAUGHTHOUSE :
						slaughHouseNo_call();	//도축장 호출키
						break;
					case  KS_MULTI_INDIVIDUAL:
						indivdualMultiStart();	//복수개체 / 묶음장부 시작키
						break;
					case  KS_LOT_LIST:
						indivdualLotListStart();	// 묶음번호 리스트 시작
						break;
					case  KS_CALL_MEATUSE :
						meatUseNo_call();	//용도 호출키
						break;
#ifdef USE_NHMART_SAFE_MEAT
					case KS_OFFLINE :
						keyapp_switch_offline();
						break;
					case KS_CALL_INDIV:
						keyapp_call_indivInfo();
						break;
#endif
#ifdef USE_TRACE_MEATPART
					case  KS_CALL_MEATPART :
						meatPartNo_call();	//부위 호출키
						break;
#endif   
#ifdef USE_TRACE_MEATGRADE
#ifdef USE_TRACE_CALL_MEATGRADE_KEY
					case  KS_CALL_MEATGRADE:
						trace_meatgrade_callGradeNo();	//등급 호출키
						break;
#endif
#endif
						
#endif
#if defined(USE_RFID_TAG_READER) || defined(USE_RFID_TAG_READER_RFID_522_B)
					case KS_RFID_TAG : keyapp_send_call_tag(); break;
#endif
					case KS_EDT_INGREDIENT  : keyapp_edit_ingredient(); break;
					case KS_LMSG_CALL  : keyapp_call_lmsg(); break;
					
#ifdef USE_GSMART_PLU
					case KS_PACKED_DATE  : keyapp_call_packed_date(); break;
					case KS_DATE_MODE : keyapp_edit_date_flag(); break; 
#endif
#ifdef USE_EMART_NETWORK
					case KS_CHECKIN : keyapp_checkin(); break;
					case KS_CHECK_PROD:	// 생산등록개수 조회
						keyapp_check_production();
						break;
					case KS_DELETE_PROD:	// 생산등록개수 삭제 
						keyapp_delete_production();
						break;
#ifdef USE_EMART_BACK_LABEL
					case KS_BACK_LABEL_VALID:	//후방라벨 유효일 설정
						keyapp_set_valid_date_back_label();
						break;
#endif	//#ifdef USE_EMART_BACK_LABEL
#endif	//#ifdef USE_EMART_NETWORK
#ifdef USE_DIVIDING_FUNCTION
					case KS_DIVIDE_HALF:	// 1/2 쪼개기 기능
					case KS_DIVIDE_ONE_THIRD:	// 1/3 쪼개기 기능
					case KS_DIVIDE_QUARTER:	// 1/4 쪼개기 기능
						uid = KeyDrv.CnvCode - KS_DIVIDE_HALF + 2;
						keypp_dividing_function(uid);
						break;
#endif
#ifdef USE_LOTTEMART_ITEMCODE13
					case KS_INPUT_ITEMCODE13:
						keyapp_lottemart_itemcode13_pluCall();
						break;
#endif
#ifdef USE_LOTTEMART_DEFAULT
					case KS_ONLINE:
						keyapp_lottemart_online();
						break;
#endif
#ifdef USE_KOR_TRACE
/////////////////////////
// Korean Traceability
// Added by CJK 20060310
//						case	KS_KTR_REQ_ID : keyapp_ktr_req_id(); break;
					case  KS_KTR_REQ_NEXTID  : keyapp_ktr_req_nextid(KORTR_REQ_NEXT_ID); break;
					case  KS_KTR_REQ_FIXEDID : keyapp_ktr_req_nextid(KORTR_REQ_FIXED_ID); break;
					case  KS_KTR_REQ_NONEID  : keyapp_ktr_req_noneid(); break;
					case  KS_KTR_REQ_COMPID  : keyapp_ktr_req_nextid(KORTR_REQ_COMPLETE_ID); break;
					case  KS_KTR_MANUAL_OPER : keyapp_ktr_manual_oper(); break;
					case  KS_KTR_MEMBER  : keyapp_ktr_member(); break;
//						case  KS_KTR_REQ_PRINT : keyapp_ktr_req_print(KORTR_REQ_PRINT_ID); break;
/////////////////////////
#endif
					case  KS_RESET :
						keyapp_weight_reset();
						break;
					case  KS_SCANPRINT : //SG070122
#ifdef USE_LOTTEMART_MEATSCANNER //20171206
						lottemart_meatscanner_initial();
#endif
						keyapp_scanprint();
						ScanFlag = 0;
						break;
					case  KS_LABEL_01:
					case  KS_LABEL_02:
					case  KS_LABEL_03:
					case  KS_LABEL_04:
					case  KS_LABEL_05:
					case  KS_LABEL_06:
					case  KS_LABEL_07:
					case  KS_LABEL_08:
						//		case  KS_LABEL_09:
						//		case  KS_LABEL_10:
						//		case  KS_LABEL_11:
						//		case  KS_LABEL_12:
						//		case  KS_LABEL_13:
						//		case  KS_LABEL_14:
						//		case  KS_LABEL_15:
						//		case  KS_LABEL_16:
						//		case  KS_LABEL_17:
						//		case  KS_LABEL_18:
						//		case  KS_LABEL_19:
						//		case  KS_LABEL_2
						uid = KeyDrv.CnvCode-KS_LABEL_01 + 1;
#ifdef USE_HYUNDAI_SET_FUNCTION
						if(MeatSetSaleMode && uid == MEAT_STORAGE_LABEL_KEY_NUM) { // 대면제작세트 구성 중에는 보관라벨로 변경 못 하게 처리 
							BuzOn(2);
							break;
						}	
#endif
						keyapp_labelformat_change((INT8U)uid);
						break;
					case  KS_GLOBAL_LABEL_BAR:
						keyapp_apply_global_label_barcode();
						break;
					case KS_EXT_PRINT : keyapp_external_print(); break;
#ifdef USE_CHN_CART_SCALE
#ifdef USE_CHN_IO_BOARD
					case  KS_USBSAVE_LOG:
						keyapp_usb_save_log();
						break;
#endif
#endif
#ifdef USE_CHN_CART_SCALE
					case  KS_CHN_CALL_TRACE:
						trace_call_traceNo();
						break;
					case  KS_CHN_TRACE1:
					case  KS_CHN_TRACE2:
					case  KS_CHN_TRACE3:
					case  KS_CHN_TRACE4:
						uid = KeyDrv.CnvCode - KS_CHN_TRACE1 + 1;
						if (Operation.traceCodeType == CHN_TRACE_CODE_TYPE_VEGETABLE) uid++; // 최근lot 이전 lot부터 키할당
						trace_direct_call_traceNo(uid);
						break;
					case KS_CHN_EDIT_REMAIN:
						trace_edit_remainWeight(status_scale.cur_pluid, CurCHNTraceLotNo);
						break;
#endif
#ifdef USE_CHN_EMART_TRACE
					case  KS_CHN_TRACE1:// 어제 //20131025 개체 Toggle key로 사용
						//case  KS_CHN_TRACE3:
						//case  KS_CHN_TRACE4:
						//uid = KeyDrv.CnvCode - KS_CHN_TRACE1 + 1;
						if (!chn_emart_trace_direct_call_traceNo(1)) BuzOn(2);// 1 - next TraceNo call
						break;
#endif
					case KS_EDIT_STORE:
						keyapp_edit_store();
						break;
					case KS_CHANGE_PREFIX:
						keyapp_change_prefix();
						break;
#ifdef USE_LOT_NUMBER_FUNCTION
					case KS_EDIT_LOT:
						keyapp_edit_lot();
						break;
#endif
                    case KS_CHANGE_KEYSET:
            			keyapp_change_keyset(status_scale.departmentid);
            			break;
#ifdef USE_HYUNDAI_SET_FUNCTION	
					case KS_SET_START:
						if (HyundaiMeatFlag)
							keyapp_hyundai_meat_set_start();
						else
							BuzOn(2);
						break;
					case KS_SET_END:
						if (HyundaiMeatFlag && MeatSetSaleMode)
							keyapp_hyundai_meat_set_input_price();
						else
							BuzOn(2);
						break;
					case KS_SET_CANCEL:
						if (HyundaiMeatFlag)
							keyapp_hyundai_meat_set_cancel();
						else
							BuzOn(2);
						break;
#endif	
#ifdef USE_REGIST_DISPOSAL
					case KS_DISPOSAL:
						// if(Operation.saleStatus == SALE_NORMAL && !MeatSetSaleMode && HyundaiMeatFlag) 
						if(!MeatSetSaleMode && HyundaiMeatFlag) 
							keyapp_hyundai_disposal();
						else
							BuzOn(2);
						break;
#endif
					default : //keyapp_non_function();
						BuzOn(2);        
						break;
				}
			}
	status_scale.cur_keyvalue=0;
	KeyDrv.FirstKey = 1;
	//0min
	if(Operation.keyClear)	KEY_InitKeyBuf();
	NOCLEAR:
						NOOPER:
						return;
}

//DEL070705extern void sale_display_discount(INT16S clearFlag);

INT8U keyapp_sale_status(INT8U mode)
{
	////	INT16S	totalCount =0;
	////	INT16S	voidCount =0;
	////	long	totalPrice=0;
	////	long	voidPrice=0;
	INT8U   ret;
	INT16U   lastClerk;
	STRUCT_TOTAL_PRICE totalData;
	
	if (!(status_scale.restrict&FUNC_CLERK)) status_scale.clerkid=0;
	lastClerk = (INT16U)status_scale.clerkid;
	ret=1;
	// mode = 2 : Gtotal or Total 중 어느 하나라도 있는지 검사.
	// mode = 1 : Gtotal 만 검사.
	//	tPrice = get_nvram_lparam(NVRAM_GTOTAL_PRICE+clerkNum*4);
	//	tCount = (long)get_nvram_wparam(NVRAM_GTOTAL_COUNT+clerkNum*2);
	
	//memset(&totalData, 0, sizeof(STRUCT_TOTAL_PRICE));
	if ((mode==0) || (mode==2)) {
		ClerkTotalPrice(0, (INT8U)lastClerk, &totalData);
		if (totalData.TotalCount+totalData.VoidCount>=1) {
		} else {
			ret=0;
		}
	}
	if ((mode==1) || (mode==2)) {
		totalData.TotalCount = (long)get_nvram_wparam(NVRAM_GTOTAL_COUNT+(INT16S)lastClerk*2);
		if (mode==2) {
			if (ret==0) {
				//				ret=1;
				if (totalData.TotalCount) ret=1;
			}
		} else {
			if (totalData.TotalCount==0) ret=0;
		}
	}
	return ret;
}

void keyapp_st_ttl(void)
{
	if (keyapp_sale_status(0) || (Operation.operationClerk != OPER_NORMAL))  //total 이 있거나 floatingClerk 일때 
	{
		keyapp_total(OFF);
	}
	else 
	{
#ifdef USE_INDIA_FUNCTION
		if (UseAreaScaleMode == 2) // 0:NormalScale(STD), 1:DualModeScale, 2:AreaScale
		{
			keyapp_total(OFF);
			return;
		}
#endif
		if (keyapp_sale_status(1)) //goto TOTAL;
			keyapp_g_total();   
		else 
			keyapp_total(OFF);
	}
}

void keyapp_override(void)
{
	INT8U	pluType;

#ifdef USE_LOTTEMART_DEFAULT
	if(!OnlineFlag) return;
#endif
	InputQuickPluState = OFF;
	status_scale.cur_quickplu = 0;

	if (!status_scale.cur_pluid)	return;
	if (Operation.useMultipleUnits)
	{
		caption_split_by_code(0xC905,&cap,0);//Multiple unit PLU
		DisplayMsg(cap.form);
		return;
	}
#ifdef USE_CHINA_DEFAULT
	pluType = get_global_bparam(GLOBAL_SALE_SETUP_FLAG3); // override location(PARAM#501) china Only
	if (OnOff(pluType&0x02) == OFF) return;
#endif
	Operation.override=ON;
	Operation.specialPrice=OFF;
	
	pluType = PluType();
	SetMaxRide(pluType);
	SetCurRide(pluType);
    
	sale_display_update(UPDATE_RIDE|UPDATE_CENTER);
	return;
}

void operation_load(void)
{
	Operation.operationClerk = get_global_bparam(GLOBAL_AREA_SALEMODE);
	status_scale.clerkid = 0;
}

void keyapp_in_weight(void)
{
#ifdef USE_WEIGHT_UNIT_500G
		long tempUprice;
#endif
	//BuzOn(1);
	if(!status_scale.cur_pluid)		return;
	if(PluType()==PLU_BY_COUNT  ) {
		if (status_scale.cur_ride !=2)	return;
	}
	if(PluType()==PLU_BY_PCS) {
		if (status_scale.cur_ride !=3)	return;
	}
	//if(PluType()!=PLU_BY_WEIGHT)	return;
	if(KeyDrv.PrevType != KEY_TYPE_NUMERIC)	return;
#ifndef USE_INWEIGHTKEY_WITHOUT_INTERVAL	  // IN_Weight Key 사용시 Weight Interval에 상관없이 1g단위로 사용 가능	
	if(!ad_check_weight_value(status_scale.cur_keyvalue))
	{
		caption_split_by_code(0xE101,&cap,0);	//"Wrong weight value"
		DisplayMsg(cap.form);
		return;
	}
#endif	
	//if(!permission_check(PMS_KEY_IN_WEIGHT))	return;
	if(status_scale.cur_keyvalue)
	{
		status_scale.Plu.fixed_weight = status_scale.cur_keyvalue;
#ifdef USE_WEIGHT_UNIT_500G
 #ifdef USE_UNIT_500G_PLU_500G_BASE
 		if(status_scale.cur_weightunit == 500) {
			status_scale.cur_unitprice = status_scale.Plu.unitprice;
		} else if(status_scale.cur_weightunit == 1000 && PluType()== PLU_BY_WEIGHT) {
			tempUprice =  longmuldiv(status_scale.Plu.unitprice,2*status_scale.cur_weightunit,power10(3));
			status_scale.cur_unitprice = (INT32U) tempUprice;
		} else {
			status_scale.cur_unitprice = status_scale.Plu.unitprice;
		}
 #else
		if(status_scale.cur_weightunit == 500 && PluType()== PLU_BY_WEIGHT) {
			tempUprice =  longmuldiv(status_scale.Plu.unitprice,status_scale.cur_weightunit,power10(3));
			status_scale.cur_unitprice = (INT32U) tempUprice;
		} else {
			status_scale.cur_unitprice = status_scale.Plu.unitprice;
		}
 #endif
#else
		status_scale.cur_unitprice = status_scale.Plu.unitprice;
#endif
		sale_display_update(UPDATE_WEIGHT|UPDATE_UNIT);
		//DisplayMsg("IN WEIGHT");
		
	}
	//Price.Weight[PRICE_RESULT] = status_scale.cur_keyvalue;
	//Operation.keyInWeight = ON;
}

INT8U	StartPrintFlag=OFF;
INT16U	ClerkLoginTime=0;
//typedef struct {
//	INT16U code;			//2
//	INT8U name[32];			//34
//	INT8U permission[4];	//38
//	INT8U password[8];		//46
//	INT8U usepassword;		//47
//	INT8U dept;				//48
//	INT8U reserve[80];
//} CLERK_STRUCT;

//void	LoginClerk(INT16U clerkid)
//{
//	if(Operation.operationClerk != OPER_NORMAL)
//	{
//		INT16S	mode;
//		DisplayModePrev = get_nvram_bparam(NVRAM_DISPLAY_DATETIME_MODE);
//		status_scale.flag_display|=1;
//		display_date_change(DISPLAY_DATE_CLERK,ON);
//	}
//	ClerkLoginTime = SysTimer100ms;// OPTION:   
//	status_scale.clerkid = clerkid;
//	set_nvram_bparam(NVRAM_LASTCLERKNUMBER, (char)clerkid);
//}

//typedef struct {
//	INT32U     PluNo;
//	INT16U     DeptNo;
//	PRICE_TYPE Price;
//	//PLU_BASE   Plu;		// Update : Plu Type, Tare 
//	INT8U 	   PluType;
//	INT32U	   TareWeight;
//} LABEL_DATA_BACKUP_TYPE;
extern void	ClearPayment(INT8U trtType);
extern LABEL_DATA_BACKUP_TYPE LabelData;

void ProcessAll(void)
{
	//DEL080327		sale_adm_proc();
	//DEL080327		sale_calcprice();
	//DEL080327		sale_display_update(0x0fff);
	//sale_display_proc();
	while(!sale_adm_proc());	// CJK08327
	sale_calcprice();
	sale_display_update(0x0fff);
	sale_display_proc(OFF);
}

void SaleInsertLabelData(void)
{
	INT16S i;
	
	Operation.insertLabelData = ON;
	
	LabelData.DeptNo	= status_scale.departmentid;
	LabelData.PluNo		= status_scale.cur_pluid;
	LabelData.Price		= Price;
	LabelData.PluType	= PluType();
#ifndef USE_PERCENT_WEIGHT
	LabelData.TareWeight	= status_scale.Tare + Price.PercentTareWeight;
#else
	LabelData.TareWeight	= status_scale.Tare;
#endif
	LabelData.PluTareWeight = TareOper.tarePLU;
	LabelData.KeyTareWeight = TareOper.tareKey;
	LabelData.WeightTareWeight = TareOper.tareWeight;
#ifndef USE_PLU_MULTIPLE_PRICE	//Refefence PLU 기능 사용 안 함
#ifndef USE_AUSTRALIA_COOL_FUNCTION //Refefence PLU 기능 사용 안 함
	for(i = 0; i < MAX_LINKED_PLU; i++) {
		LabelData.linked_plu[i] = status_scale.linked_plu[i];
		LabelData.LinkedPrice[i] = LinkedPrice[i];
	}
#endif
#endif
	LabelData.FixedWeight  	= status_scale.Plu.fixed_weight;
#ifdef USE_KOR_TRACE
	if(KorTrace_Flag.flagPrint==2) {	// Added by CJK 20060316
		LabelData.PluNo = KorTrace_ReceiveCmd.commodity_no;
		LabelData.Price.Weight[PRICE_RESULT] = KorTrace_ReceiveCmd.weight;
		LabelData.Price.UnitPrice[PRICE_RESULT] = KorTrace_ReceiveCmd.unitprice;
		//LabelData.Price.TotalPrice[PRICE_RESULT] = KorTrace_ReceiveCmd.DisplayedPrice;DisplayedPrice
		LabelData.Price.TotalPrice[PRICE_RESULT] = KorTrace_ReceiveCmd.totalprice;	// CJK080829
		LabelData.Price.DisplayedPrice = KorTrace_ReceiveCmd.totalprice;
		LabelData.SellbyDate = (INT32U) KorTrace_ReceiveCmd.sellbydate;  //SG060322
		KorTrace_ReceiveCmd.sellbydate = 0;  //SG060323	 1회 Print용
	} else if(KorTrace_Flag.flagPrint==1) {	//SG060324
		//LabelData.PluNo = KorTrace_SendData.commodity_no;
		LabelData.PluNo = status_scale.cur_pluid;

		//LabelData.Price.Weight[PRICE_RESULT] = KorTrace_SendData.weight;
		LabelData.Price.Weight[PRICE_RESULT] = Price.Weight[PRICE_RESULT];

		//LabelData.Price.UnitPrice[PRICE_RESULT] = KorTrace_SendData.unitprice;
		LabelData.Price.UnitPrice[PRICE_RESULT] = Price.UnitPrice[PRICE_RESULT];

		LabelData.Price.TotalPrice[PRICE_RESULT] = Price.TotalPrice[PRICE_RESULT];

		//LabelData.Price.DisplayedPrice = KorTrace_SendData.totalprice;
		LabelData.Price.DisplayedPrice = Price.DisplayedPrice;

		LabelData.SellbyDate = (INT32U) KorTrace_ReceiveData.sellbydate;
	 	KorTrace_ReceiveData.sellbydate = 0;  //SG060323	 1회 Print용
	}

#endif

#ifdef USE_SETPRODUCT_SALE
	if(SetproductSaleMode)
	{
		if(setproduct_PrtFlag)// 세트상품 가격 출력
		{
			LabelData.Price.UnitPrice[PRICE_RESULT] = 0;
			LabelData.Price.TotalPrice[PRICE_RESULT] = 	SetproductPrice;
//			setproduct_PrtFlag = OFF;
		}
		else// 내품 가격 
		{
			LabelData.Price.UnitPrice[PRICE_RESULT] = 0;
			LabelData.Price.TotalPrice[PRICE_RESULT] = 0;
		}
	}
#endif

	ComputeCurrency(0,0,0,Price.DisplayedPrice,Price.UnitPrice[PRICE_RESULT]);
}


void PrintLabelWait(void)
{
	
	if(!status_scale.cur_pluid)	return;
	
	//////고려///////////
	//#	if((Operation.wPrepack || Operation.wAuto) && (KeyDrv.Type = KEY_TYPE_SPEEDPLU||KeyDrv.CnvCode==KS_PLU))
	//#	{
	//#		ProcessAll();
	//#	}
	/////////////////
	Operation.salePrintWait = ON;
	Operation.insertLabelData = OFF;
	
	//SG070718
	if(GlbHold.holdOperationTime)
	{
		GlbHold.holdStartTime100ms = SysTimer100ms;
		GlbHold.count =0;
		GlbHold.flagCalcOK =OFF;		
	}
	
	if(PluType()== PLU_BY_WEIGHT)
	{
		if(!ADM_GetData.Stable)	return;
	}
	SaleInsertLabelData();
}

void MsgNoPriceClerk(INT16U clerk)
{
	//	caption_split_by_code(0xc03a,&cap,1);	//"(#%d) No sale data"
	sprintf(gmsg,global_message[MSG_WAR_NOVOID],clerk);
	DisplayMsg(gmsg);
}

//DEL051115 void	MsgCheckPermisson(INT16U clerk)
//DEL051115 {
//DEL051115 //	caption_split_by_code(0xc03b,&cap,1);	//"(#%d) Check permission"
//DEL051115 	sprintf(gmsg,global_message[MSG_WAR_PERMIT],clerk);
//DEL051115 	DisplayMsg(gmsg);
//DEL051115 }

extern void sale_get_price(INT16S linkedPLU,INT16S pluType, long pluid, PRICE_TYPE *pPrice,long tWeight,INT16S tQty,INT16S tPcs,long tUnitPrice,long tTare);
extern void CalcDisplayedPrice(void);	// CJK071122
void DirectWeightMode(void)
{
	INT32S tareTotal;
	//DEL060829	INT32S tareLimit;
	INT32U	tUnitPrice;
	
	if(GlbPrint.directWeight &&!status_scale.cur_pluid && 0<status_scale.Weight && 0< status_scale.cur_keyvalue)
	{
		//new
		Operation.transactionOK = OFF;
		tUnitPrice = status_scale.cur_keyvalue;
		keyapp_misc_byweight(OFF);
		status_scale.cur_unitprice = tUnitPrice;
		sale_get_price(0,PLU_BY_WEIGHT,status_scale.cur_pluid,
				&Price,
				status_scale.Weight,
				status_scale.cur_qty,
				status_scale.cur_pcs,
				status_scale.cur_unitprice,
				status_scale.Tare);
		PercentTareOver = OFF;
#ifndef USE_PERCENT_WEIGHT
		tareTotal = ad_cnv_weight_value(TareOper.tareKey+TareOper.tarePLU+TareOper.tareWeight+Price.PercentTareWeight, 1);
#else
		tareTotal = ad_cnv_weight_value(TareOper.tareKey+TareOper.tarePLU+TareOper.tareWeight, 1);
#endif
		if(!ad_check_tarelimit(tareTotal)) {
			PercentTareOver = ON;
			sale_display_update_error(0xE001,0,0);
			sale_display_update(UPDATE_WEIGHT|UPDATE_PRICE);
		}
		CalcDisplayedPrice();	// CJK071122
		////////////////////////
		sale_display_price();
	}
	else if(Operation.operationMode == MODE_SELF) {
		ProcessAll();
	}
}

//SG070227
void DirectCountMode(void)
{
	INT32U keyvalue;
	
	if (GlbPrint.directNonfood && !status_scale.cur_pluid && 0 < status_scale.cur_keyvalue && status_scale.Weight <= 0)
	{
		keyvalue = status_scale.cur_keyvalue;
		keyapp_misc_bycount();
		status_scale.cur_ride = 2;
		status_scale.cur_qty = 1;
		status_scale.cur_unitprice = keyvalue;
	}
}

extern INT16S ClerkSearchEmpty(void);
INT16U keyapp_call_clerk(void)
{	
	INT16U clerk_cnt;

	if(ADM_GetData.Overload || PercentTareOver) return 0;//(For NMi) Added by CJK 20050713
	
	DirectWeightMode();	// no PLU -> Misc. Weight
	DirectCountMode();      //SG070227
	
	if(!CheckPluPrint(0))	return 0;
	if (!(Operation.operationClerk == OPER_FLOATING && ethernet_list.status == 2))
	// Slave 일 때 Master에서 Clerk data를 가져오기 전 시점으로 잘못된 Clerk count가 계산되므로 사용하지 않는다.
	{
		clerk_cnt = ClerkTransactionCnt(status_scale.clerkid);

#ifdef USE_QR_ADDTOTAL
		if(QrAddTotalFlag)
		{
			if(clerk_cnt >= MAX_QR_ADDCOUNT)
			{
				caption_split_by_code(0xC90D,&cap,0);
				DisplayMsg(cap.form);
				BuzOn(2);
				return 0;
			}
		}
#endif
		if(clerk_cnt >= MAX_CLERK_TRANSACTION)
		{
			caption_split_by_code(0xCA06,&cap,0);
			DisplayMsg(cap.form);
			BuzOn(2);
			return 0;
		}
		if(ClerkSearchEmpty() == -1)
		{
			caption_split_by_code(0xC90D,&cap,0);
			DisplayMsg(cap.form);
			Key_DelaySec(10,0);
			return 0;
		}
	}
	
	StartPrintFlag = ON;
	if(Operation.operationClerk==OPER_STD_CLERK && !Operation.addStandatdClerk)	// add 를 동작하지 않고 total sum에 대한 계산 flag를 설정한다
		Operation.transAddup=OFF;	// add
	else
		Operation.transAddup=ON;	// add
	if(!Operation.multiply)
		status_scale.print_multiple=1;
	PrintLabelWait();
	Operation.directClearWeight = ON;
	return 0;
}

INT16U keyapp_department(INT16U skeydept)
{
	INT32U key_addr;//,keydept;
	INT16U deptid;
	//BuzOn(1);
	//	keydept = skeydept - KS_DEPT_01;
	key_addr  = DFLASH_BASE + FLASH_KEY_CDTC_AREA + 192;		//64 --> 192 JJANG 20070430
	key_addr += skeydept*2;
	deptid    =Flash_wread(key_addr);
	keyapp_call_department(deptid,0);
	status_scale.flag_display|=1;
	display_date_change(DISPLAY_DATE_DEPT,ON);
	return deptid;
}

long longmuldiv(long val,long mul,long div);
CURRENCY_PRICE_TYPE CurrencyPrice;

void long_multi_cut(long a, long b, long cut, long *result_h, long *result_l)
{
	long a_h4, a_l4, b_h4, b_l4;
	long temp_h, temp_l;
	
	if((a >= power10(9) || b >= power10(9)) || (ilog(a)+ilog(b) >= 18)) {	// [9자리] * [8자리] 까지 가능
		temp_h = 0;
		temp_l = 0;
	} else{
		a_h4 = a / 10000L;
		a_l4 = a % 10000L;
		b_h4 = b / 10000L;
		b_l4 = b % 10000L;
		temp_h = a_h4 * b_h4;
		temp_h += (a_l4 * b_h4 / 10000L);
		temp_h += (a_h4 * b_l4 / 10000L);
		temp_l = a_l4 * b_l4;
		temp_l += (a_l4 * b_h4 % 10000L) * 10000L;
		temp_l += (a_h4 * b_l4 % 10000L) * 10000L;
		temp_h += temp_l/100000000L;
		temp_l %= 100000000L;
		////sprintf(MsgBuf, "[A0] %ld*%ld = %ld, %ld\r\n", a, b, temp_h, temp_l);
		////MsgOut(MsgBuf);
		if(cut) {
			temp_l /= power10(cut-1);
			temp_l += 5;
			temp_l /= 10;
			temp_l += temp_h%power10(cut)*power10(8-cut);
			temp_h /= power10(cut);
		}
	}
	*result_h = temp_h;
	*result_l = temp_l;  
}

////void long_multi_cut_2(long src, long dest, long cut, long *result_h, long *result_l)
////{
////	INT8U src_array[10], dest_array[10];
////	INT8U result_array[18];
////	byte s, d, r, cnt;
////	char i, j;
////
////	memset(src_array, 0, 10);
////	memset(dest_array, 0, 10);
////	memset(result_array, 0, 18);
////
////	format_fixed(src_array,(long)src,0,10,'.');
////	format_fixed(dest_array,(long)dest,0,10,'.');
////	
////	r=0;
////	for(i=0; i<10; i++){
////		d=dest_array[9-i];
////		if(d<'0') break;
////		d-='0';
////		cnt=15-i;
////		for(j=0; j<10; j++){
////			s=src_array[9-j];
////			if(s<'0'){
////				if(r) result_array[cnt]=r+'0';
////				r = 0;
////				break;
////			}
////			s-='0';
////			r=r+(byte)(s*d);
////			if(result_array[cnt]<'0'){
////				result_array[cnt]=(byte)(r%10)+'0';
////			} else {
////				r=r+(result_array[cnt]-'0');
////				result_array[cnt]=(byte)(r%10)+'0';
////			}
////			cnt--;
////			r=(byte)(r/10);
////		}
////	}
////////for(i = 0; i < 16; i++){
////////	if(result_array[i] > '0') break;
////////}
////////cnt = i;
////////sprintf(MsgBuf, "[1] %s\r\n", &result_array[cnt]);
////////MsgOut(MsgBuf);
////	for(i = 0; i < 16-cut; i++) result_array[15-i] = result_array[15-i-cut];
////	for(;i < 16; i++) result_array[15-i] = ' ';
////
////////sprintf(MsgBuf, "[2] %s\r\n", &result_array[cnt]);
////////MsgOut(MsgBuf);
////	*result_h = 0;
////	for(i = 0; i < 8; i++) {
////		*result_h *= 10;
////		if(result_array[i] < '0') continue;
////		*result_h += result_array[i] - '0';
////	}
////	*result_l = 0;
////	for(i = 8; i < 16; i++) {
////		*result_l *= 10;
////		if(result_array[i] < '0') continue;
////		*result_l += result_array[i] - '0';
////	}
////sprintf(MsgBuf, "[B1] %ld*%ld = %ld, %ld\r\n", src, dest, *result_h, *result_l);
////MsgOut(MsgBuf);
////  
////}

/*
 INT8U check_currency_overflow(long mul1, long mul2, long div1)
 {
 INT8U m1, m2, d1, i;
 
 m1 = 10;
 m2 = 10;
 d1 = 10;
 for(i = 1; i < 10; i++) {
 if(m1 == 10 && mul1 < power10(i)) m1 = i;
 if(m2 == 10 && mul2 < power10(i)) m2 = i;
 if(d1 == 10 && div1 < power10(i)) d1 = i;
 }
 if(m1 + m2 - d1 < 10) return ON;	// 임시...
 
 return OFF;
 }
 
 long calc_currency_price(long price)
 {
 INT8U sys_decimalpoint;
 INT32U div_price, ret;
 
 sys_decimalpoint = get_global_bparam(GLOBAL_AREA_PRICE_DECIMALPOINT);
 div_price = power10(CurrencyPrice.CurrencyRateDiv+sys_decimalpoint-CurrencyPrice.CurrencyPriceDiv);// 수소점이니 +1
 if(!check_currency_overflow(price, CurrencyPrice.CurrencyRate, div_price)) return 0xffffffff;
 ret = longmuldiv(price, CurrencyPrice.CurrencyRate, div_price);
 
 return ret;
 }
 */

INT16U keyapp_currency(INT16S beep,INT16S displayMsg,long currPrice,long currUnitPrice,INT16U skeycurrency)
{
	INT32U addr,keycurrency;
	INT16U currencyid;
	CURRENCY_STRUCT currency;
	char tempCurrency[5];
	//long	divPrice;
	INT16S    sz;
	//char	priceBuf[20];
	INT8U sys_decimalpoint, cut_digit;
	// Currency ID = 1 이면 주소 지역은 0 , 1~4까지 허용된다.
	// Dual Currency 의 사용. 
	//   - get_global_bparam(GLOBAL_DUAL_CURRENCY);
	if(beep)	BuzOn(1);
	keycurrency =skeycurrency-KS_CURRENCY_01;
	sz = sizeof(CURRENCY_STRUCT);
	addr = keycurrency * sz;
	addr+= DFLASH_BASE + FLASH_CURRENCY_AREA;
	Flash_sread(addr,(HUGEDATA INT8U*)&currency,sz);
	currencyid = keycurrency+1;
	if(displayMsg && !currency.rate)
	{
		caption_split_by_code(0xC90A,&cap,0);//"No currency data"
		DisplayMsg(cap.form);
		return 0;
	}
	CurrencyPrice.CurrencyIndex	= currencyid;
	CurrencyPrice.CurrencyRate	= currency.rate;
	CurrencyPrice.CurrencyRateDiv	= currency.ratepoint;
	strncpy(tempCurrency,(char *)currency.primary,4);
	tempCurrency[4]=0;
	str_trim(tempCurrency);
	strcpy(CurrencyPrice.CurrencyString1,str_trimleft(tempCurrency));
	strncpy(tempCurrency,(char *)currency.last,4);
	tempCurrency[4]=0;
	str_trim(tempCurrency);
	strcpy(CurrencyPrice.CurrencyString2,str_trimleft(tempCurrency));
	//divPrice=power10(CurrencyPrice.CurrencyRateDiv+1);// 수소점이니 +1
	////	divPrice=power10(CurrencyPrice.CurrencyRateDiv);// 수소점이니 +1
	////	if(currPrice)
	////		CurrencyPrice.CurrencyPrice = longmuldiv(currPrice,currency.rate,divPrice);
	////	else
	////		CurrencyPrice.CurrencyPrice = 0;
	CurrencyPrice.CurrencyPriceDiv	= currency.decimalpoint;
	CurrencyPrice.CurrencyPrtForm = currency.prtform;
	
	sys_decimalpoint = get_global_bparam(GLOBAL_AREA_PRICE_DECIMALPOINT);
	cut_digit = CurrencyPrice.CurrencyRateDiv+sys_decimalpoint-CurrencyPrice.CurrencyPriceDiv;
	
	////long_multi_cut_2(currPrice, CurrencyPrice.CurrencyRate, cut_digit, &CurrencyPrice.CurrencyPriceH8, &CurrencyPrice.CurrencyPriceL8);
	long_multi_cut(currPrice, CurrencyPrice.CurrencyRate, cut_digit, &CurrencyPrice.CurrencyPriceH8, &CurrencyPrice.CurrencyPriceL8);
	long_multi_cut(currUnitPrice, CurrencyPrice.CurrencyRate, cut_digit, &CurrencyPrice.CurrencyUnitPriceH8, &CurrencyPrice.CurrencyUnitPriceL8);
	////	CurrencyPrice.CurrencyPrice = calc_currency_price(currPrice);
	////	CurrencyPrice.CurrencyUnitPrice = calc_currency_price(currUnitPrice);
	return currencyid;
}

INT16U keyapp_customer(INT16U skeycustomer)
{
	INT32U key_addr,keycustomer;
	INT16U customerid;
	
	BuzOn(1);
	keycustomer =skeycustomer-KS_CUSTOMER_01;
	//	key_addr  = KEY_MAX_TABLE_SIZE;
	//	key_addr *= 5;
	key_addr = DFLASH_BASE + FLASH_KEY_CUSTOMER_AREA;
	key_addr += (keycustomer)*2;
	customerid    =Flash_wread(key_addr);
	return customerid;
}

void keyapp_directtare(INT16U skeytareid)
// Tare1,Tare2,... DirectKey Tare(Memorized Tare)
{
	INT16U keytareid;
	INT32U key_addr;
	INT16U tare_id;
	//	char temp[32];
	keytareid = skeytareid-KS_TARE_01;
	if (keytareid>MAX_TARE_KEY) {
		BuzOn(2);
		return;
	}
	BuzOn(1);
	//	sprintf(temp,"[%d]",keytareid);
	//	MsgOut((HUGEDATA char *)temp);
	//	key_addr  = KEY_MAX_TABLE_SIZE;
	//	key_addr *= 5;
	//	key_addr += DFLASH_BASE + FLASH_KEY_AREA;
	//	key_addr += keytareid*2;
	key_addr  = DFLASH_BASE + FLASH_KEY_CDTC_AREA + 128;
	key_addr += keytareid*2;
	tare_id    =Flash_wread(key_addr);
	//	sprintf(temp,"[Tareid=%d]",tare_id);
	//	MsgOut((HUGEDATA char *)temp);
	keyapp_call_tare(tare_id);
	return;
}

void keyapp_call_department(INT16U dept_id,INT8U fg)
{
	INT32U start_addr;
	INT32U ret_value = 0;
	char S_msg1[32];
	char S_msg2[32];
	INT8U result = 0;
	
	DEPARTMENT_STRUCT dept;
	if (dept_id>=100) return;
	if (dept_id==0) dept_id = (INT16U)get_net_wparam(NETWORK_DEPARTMENT_ID);
	if (status_scale.cur_pluid) 
	{
		if (!fg) sale_display_update_error(0xC061,0,0);
		return;
	}
	if (status_scale.departmentid == dept_id) return;
	
	strcpy(S_msg1,"ARE YOU SURE YOU WANT TO");
	strcpy(S_msg2,"CHANGE DEPARTMENT?(1=Y/0=N)");
	ret_value = display_message_check_page(S_msg1,S_msg2,1);
	if (!ret_value) return;

	status_scale.departmentid=dept_id;
	start_addr = (dept_id-1)*DEPARTMENT_STRUCT_SIZE;
	start_addr+=  DFLASH_BASE + FLASH_DEPARTMENT_AREA;
	Flash_sread(start_addr,(HUGEDATA INT8U *)&dept,DEPARTMENT_STRUCT_SIZE);
	if (dept._speedkey!=status_scale.cur_speedkeyid) {
		if (!fg) sale_display_update_error(0xC062,0,0);
	}
	status_scale.cur_speedkeyid=1;
	if ((dept._speedkey>=1) && (dept._speedkey<=5))
		status_scale.cur_speedkeyid=dept._speedkey;
    set_net_wparam(NETWORK_DEPARTMENT_ID,(INT16U)status_scale.departmentid); // save current dept id
	Dsp_Fill_Buffer(0); 					// LCD Buffer clear
	Dsp_Diffuse();
}

////#define	TARE_TYPE_WEIGHT	1
////#define	TARE_TYPE_KEY		2
////#define	TARE_TYPE_PLU		3
void keyapp_call_tare(INT16U tare_id)
{
	INT8U  taretype, tare_unit, ret;
	INT32U tare_value;
	
	ret = get_tare_fromid(tare_id, &taretype, &tare_value, &tare_unit);
	//if ((taretype>=0) && (taretype<3)) {
	if(ret) {
		if(tare_unit != ADM_GetData.CurUnit) {
			DisplayMsg(global_message[MSG_NOT_ALLOW_KGLB]);//"kg<->lb Not Allowed"
			return;
		}
		//keyapp_tare(TARE_TYPE_KEY,tare_value,ON);
		keyapp_tare(TARE_TYPE_PRESET, tare_value, ON);
	} else {
		//status_scale.tare_type=taretype;
		//status_scale.tare     =tare_value;
		BuzOn(2);
	}
	return;
}

void keyapp_call_lmsg(void) //JJANG 20090320 임시적용, 프리팩 모드일 때 유지
{
#ifdef USE_WEIGHT_UNIT_500G
	long tempUprice;
#endif
#ifdef USE_HYUNDAI_CHECK_REASON_CODE
	BuzOn(2); // Plu.sales_msg 를 사유 코드로 사용 하기 때문에 변경 불가
	return;
#endif
	if(!status_scale.cur_pluid)		return;
	if(KeyDrv.PrevType != KEY_TYPE_NUMERIC)	return;
	INT8U ret;
	ret=get_global_bparam(GLOBAL_SALE_SETUP_FLAG3); //Param 573, 직접단가변경허용
#ifdef USE_PRICE_CHANGE_PROHIBITED
		if (PluType() == PLU_BY_WEIGHT)
		{
			if(PressedNumberKey)
			{
				status_scale.Plu.sales_msg = PressedNumberKey % 100;
				PressedNumberKey = 0;
				PressedNumberKey_1000Digits = 0;
			}
		}
		else if (PluType() == PLU_BY_COUNT)
		{
			if(status_scale.cur_keyvalue)
			{
				status_scale.Plu.sales_msg = status_scale.cur_keyvalue % 100;
			}
		}
#else // USE_PRICE_CHANGE_PROHIBITED
	if ( !(ret&0x08) )
	{
	    if(PressedNumberKey)
	    {
	        status_scale.Plu.sales_msg = PressedNumberKey;
			PressedNumberKey = 0;
	    }
	}
	else
	{
		if(status_scale.cur_keyvalue)
		{
			status_scale.Plu.sales_msg = status_scale.cur_keyvalue;
  #ifdef USE_WEIGHT_UNIT_500G
    #ifdef USE_UNIT_500G_PLU_500G_BASE
			if(status_scale.cur_weightunit == 500) {
				status_scale.cur_unitprice = status_scale.Plu.unitprice;
			} else if(status_scale.cur_weightunit == 1000 && PluType()== PLU_BY_WEIGHT) {
				tempUprice =  longmuldiv(status_scale.Plu.unitprice,2*status_scale.cur_weightunit,power10(3));
				status_scale.cur_unitprice = (INT32U) tempUprice;
			} else {
				status_scale.cur_unitprice = status_scale.Plu.unitprice;
			}
    #else
			if(status_scale.cur_weightunit == 500 && PluType()== PLU_BY_WEIGHT) {
				tempUprice =  longmuldiv(status_scale.Plu.unitprice,status_scale.cur_weightunit,power10(3));
				status_scale.cur_unitprice = (INT32U) tempUprice;
			} else {
				status_scale.cur_unitprice = status_scale.Plu.unitprice;
			}
    #endif
  #else
			status_scale.cur_unitprice = status_scale.Plu.unitprice;
  #endif
			if (PluType() != PLU_BY_WEIGHT)
			{
				status_scale.cur_qty = status_scale.Plu.qty;
			}
			sale_display_update(UPDATE_WEIGHT|UPDATE_UNIT);
		}    
	}
#endif // USE_PRICE_CHANGE_PROHIBITED
}
#ifdef USE_PRICE_CHANGE_PROHIBITED
INT16U packed_date = 0;
#else
INT8U packed_date = 0;
#endif
INT8U printdateOFF = 0;
void keyapp_call_packed_date(void)	//Added by JJANG 20090405, for GS mart, 가공일변경, 1회만 적용 
{
	INT8U ret;
#ifdef USE_WEIGHT_UNIT_500G
	long tempUprice;
#endif	
	if(!status_scale.cur_pluid)		return;
	if(KeyDrv.PrevType != KEY_TYPE_NUMERIC)	return;
	packed_date = 0;
	ret=get_global_bparam(GLOBAL_SALE_SETUP_FLAG3); //Param 573, 직접단가변경허용
#ifdef USE_PRICE_CHANGE_PROHIBITED
	if (PluType() == PLU_BY_WEIGHT)
	{
		if(PressedNumberKey)
		{
			packed_date = PressedNumberKey;
			PressedNumberKey = 0;
			PressedNumberKey_1000Digits = 0;
		}
	}
	else if (PluType() == PLU_BY_COUNT)
	{
		if(status_scale.cur_keyvalue)
		{
			packed_date = status_scale.cur_keyvalue;
		}
	}

	if(!packed_date) printdateOFF = ON;
#else
	if ( !(ret&0x08) )
	{
		if(PressedNumberKey)
		{
			packed_date = PressedNumberKey;
			PressedNumberKey = 0;
		}
		else printdateOFF = ON;	
	}
	else
	{
		if(status_scale.cur_keyvalue) packed_date = status_scale.cur_keyvalue;
		else printdateOFF = ON;	
	}
#endif
#ifdef USE_WEIGHT_UNIT_500G
 #ifdef USE_UNIT_500G_PLU_500G_BASE
	if(status_scale.cur_weightunit == 500) {
		status_scale.cur_unitprice = status_scale.Plu.unitprice;
	} else if(status_scale.cur_weightunit == 1000 && PluType()== PLU_BY_WEIGHT) {
		tempUprice =  longmuldiv(status_scale.Plu.unitprice,2*status_scale.cur_weightunit,power10(3));
		status_scale.cur_unitprice = (INT32U) tempUprice;
	} else {
		status_scale.cur_unitprice = status_scale.Plu.unitprice;
	}
 #else
	if(status_scale.cur_weightunit == 500 && PluType()== PLU_BY_WEIGHT) {
		tempUprice =  longmuldiv(status_scale.Plu.unitprice,status_scale.cur_weightunit,power10(3));
		status_scale.cur_unitprice = (INT32U) tempUprice;
	} else {
		status_scale.cur_unitprice = status_scale.Plu.unitprice;
	}
 #endif
#else
	status_scale.cur_unitprice = status_scale.Plu.unitprice;
#endif
	sale_display_update(UPDATE_WEIGHT|UPDATE_UNIT);
}

#ifdef USE_EMART_NETWORK
void keyapp_checkin(void)
{
	INT8U string_buf[15];
	Operation.checkin = Operation.checkin^1;
	if (Operation.checkin)
	{
		sprintf((char *)string_buf, "생산등록");
		display_message_page_mid((char *)string_buf);
	}
	else
	{
		sprintf((char *)string_buf, "생산등록해제");
		display_message_page_mid((char *)string_buf);
	}
	sale_display_update(UPDATE_MODE);
}
#endif
#ifdef USE_EMART_BACK_LABEL
void keyapp_set_valid_date_back_label(void)
{
	CAPTION_STRUCT 	cap;
	INT8U	result, old_font, key_mode;
	INT32U	inputValidDate = 0;
	
	key_mode = KEY_GetMode();
	KEY_SetMode(1);
	old_font = DspStruct.Id1;
	DspLoadFont1(DSP_MENU_FONT_ID);
	inputValidDate = get_base_bparam(FLASH_BACK_LABEL_VALID_DATE);
	
	while(1)
	{      
		Menu_Init();
		
		caption_split_by_code(0x147B, &cap,0);	 //후방라벨 유효일
		//		cap.input_min = 0;	// 0입력 가능(취소 기능)
		caption_adjust(&cap,NULL);
		
		Menu_InputCreate(0, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
				cap.input_max, cap.input_min, 0, &inputValidDate, NULL, NULL, NULL);
		result = Menu_InputDisplay();
		if(result == MENU_RET_SAVE) 
		{
			set_base_bparam(FLASH_BACK_LABEL_VALID_DATE, (INT8U)inputValidDate);
			break;
		} 
		else if(result ==MENU_RET_ESC) break;
	}
	
	Dsp_Fill_Buffer(0);						// LCD Buffer clear
	DspLoadFont1(old_font);
	Dsp_Diffuse();
	KEY_SetMode(key_mode);
	sale_display_update(0x0fff);
}

#endif
#ifdef USE_DIVIDING_FUNCTION
INT8U DividedPluName[80];
//keynum : 2 : 1/2, 3 : 1/3, 4 : 1/4, else return. 
void keypp_dividing_function(INT8U keynum)
{
	INT8U pluItemName[55];
	INT16U pluAddr;
	INT8U curPcsSymNo;
	INT8U pcsSymbolStr[PCS_UNIT_STRUCT_SIZE];
	INT16S fieldSize;
	INT32U addr;
	//기능 사용 조건
	if(!status_scale.cur_pluid) return;	//PLU CALL 후에 입력되어야 함. 
	if((PluType() == PLU_BY_WEIGHT) || (!status_scale.Plu.tax_id))	//Tax ID 를 쪼개기 가능 필드로 사용.
	{
		BuzOn(2);
		sprintf((char *)MsgBuf, "쪼개기 불가 상품");
		display_message_page_mid((char *)MsgBuf);
		return;
	}
	//쪼개기 기능 시 수량은 1로 고정됨.
	status_scale.cur_qty = 1;
	//key 재입력 check. 
	if(!status_scale.divisor)
	{
	 	status_scale.cur_unitprice  = status_scale.cur_unitprice / keynum;
	}
	else
	{
		status_scale.cur_unitprice  = status_scale.Plu.unitprice / keynum;
	}
	//status_scale.divisor 에 키 값 입력, sale_pluclear와 keyapp_pluno 에서 0으로 초기화 됨. 
	status_scale.divisor = keynum;	//변경 가격 check, 인쇄, display 에서 참조.
	
	//개수 단위 가져오기.
	plu_table_search(status_scale.departmentid, status_scale.cur_pluid, &pluAddr, 0);
	plu_get_field(pluAddr-1, PTYPE_QTYUNIT_NO, (INT8U *)&curPcsSymNo, (INT16S *)&fieldSize, sizeof(curPcsSymNo));
	if(curPcsSymNo)
	{
		addr =  DFLASH_BASE + FLASH_PCS_UNIT_AREA;
		addr =  addr + PCS_UNIT_STRUCT_SIZE * (INT32U)(curPcsSymNo - 1);
		addr += 2;
		Flash_sread(addr, (HUGEDATA INT8U *)pcsSymbolStr, PCS_UNIT_STRUCT_SIZE-2);
	}
	else
	{
		get_global_sparam(GLOBAL_PCS_SYMBOL,(INT8U *)pcsSymbolStr, PCS_UNIT_STRUCT_SIZE-2);
		pcsSymbolStr[PCS_UNIT_STRUCT_SIZE-1] = 0;
	}
	//PLU NAME 가져 오기
	memcpy(pluItemName, status_scale.Plu.ItemName, sizeof(pluItemName));
	pluItemName[54] = 0;
	//DISPLAY : 쪼개기구분(1/2,1/3,1/4) + 단위표시(개,마리) + 공백(1자리) + 원상품명
	sprintf((char*)DividedPluName, "1/%d%s %s", status_scale.divisor, pcsSymbolStr, pluItemName);
	//쪼개기는 수량 1 로 고정. 숫자키 입력시 단가 변경됨. 
	status_scale.cur_ride = 2;
	sale_display_update(UPDATE_ALL);
	return;
}

#endif //USE_DIVIDING_FUNCTION

extern void start_external_print(INT16U count);
void keyapp_external_print(void)		//BPDT 인쇄 폼 설정 메뉴.
{
	CAPTION_STRUCT cap;
	INT8U	 result;
	INT8U	 old_font;
	INT8U	 key_mode;
	INT32U  label_cnt;

	label_cnt = 1;

	key_mode=KEY_GetMode();
	KEY_SetMode(1);
	old_font=DspStruct.Id1;
	DspLoadFont1(DSP_MENU_FONT_ID);

	Menu_Init();
	caption_split_by_code(0xc070, &cap,0);
	strcat(cap.form, ": ");
	cap.input_dlength = 3;
	cap.input_length  = 3;
	caption_adjust(&cap,NULL);
	cap.input_max = 999;
	Menu_InputCreate(1, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
			 cap.input_max, cap.input_min, 0, &label_cnt, NULL, NULL, NULL);

	result = Menu_InputDisplay();
	if (result == MENU_RET_SAVE) 
	{
		 if (label_cnt) start_external_print((INT16U)label_cnt);
		 else BuzOn(2);
	}
	Dsp_Fill_Buffer(0);
	Dsp_Diffuse();
	DspLoadFont1(old_font);
	KEY_SetMode(key_mode);
	sale_display_update(0x0fff);
	return;
}

extern char*	GetCurrency(char* pCurr,INT8U prt_or_display);
extern INT32U ing_calc_addr_by_key(INT16U ingid);
extern INT32U rtc_makedate_str(char *str, INT8U year, INT8U month, INT8U date);
extern void Prt_SetPrintData(INT8U label_mode, RTC_STRUCT *time_str, INT16U dept_no, INT32U plu_no);
extern STRUCT_PRT_ITEM PrtItemStr;
extern STRUCT_PRT_COMMON PrtComStr;
extern INT8U PrtFirstLabel;

extern HUGEDATA BAR_FIELD_STRUCT BarFieldStruct[MAX_BAR_FIELD_NUM];
extern INT8U barcodetype_codevalue[];

void external_print_name1(char *send_string, INT8U *formName, char *bar_str, char *name1_string)
{
	INT16U i;
	i = 0;
//	sprintf(send_string+i,"FR");
//	i=strlen((char *)send_string);
//	sprintf(send_string+i,"%c",'"');
//	i=strlen((char *)send_string);
//	sprintf(send_string+i,"%s",formName);
//	i=strlen((char *)send_string);
//	sprintf(send_string+i,"%c\n",'"');
//	i=strlen((char *)send_string);
	sprintf(send_string+i,"FR%c%s%c\n",'"',formName,'"');
	i=strlen((char *)send_string);
	sprintf(send_string+i,"?\n");
	i=strlen((char *)send_string);
	sprintf(send_string+i,"%s \n",bar_str);							//V00 : 바코드
	i=strlen((char *)send_string);
	sprintf(send_string+i,"%s \n",name1_string);							//V01 : 상품명 1단
	MsgOut(send_string);
}

void external_print_sellbydate(char *send_string, INT8U sellbydate_check)
{
	INT8U	v8_1;
	INT16U	v16_1, v16_3, v16_4, v16_5, t16_3, t16_4, t16_5;   

	if (PrtItemStr.flagsellbydate != 1)
	{
#ifdef USE_GEORGIA_MART_SELLBYDATE_FUNCTION
		/* Sell_by_Date Value 0이 아닌 값 or 900 이상일때 No Print 기능 (그루지아 Agrohub 마트 요청) */
		if (PrtItemStr.sellbydate != 0 && PrtItemStr.sellbydate < 900)				
#else
  #ifdef USE_SELLBYDATETIME_ZERO
		if (PrtItemStr.sellbydate || PrtItemStr.sellbytime)		 // Sell by Date or Sell by Time존재시
  #else
		if (PrtItemStr.sellbydate || PrtItemStr.sellbytime || PrtItemStr.flagsellbydate == 2)	 // Sell by Date or Sell by Time존재시
  #endif
#endif
		{
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
			v8_1 = PrtComStr.time.hour + PrtItemStr.packedontime + PrtItemStr.sellbytime;		 // current time + Packed Time + Sell by Time
			v8_1 /= 24;
			t16_3=v16_3; t16_4=v16_4; t16_5=v16_5;
			date_getnextday((short*)&v16_3,(short*)&v16_4,(short*)&v16_5,(short)v16_1);
			v8_1=1;
			if (sellbydate_check && PrtItemStr.flagsellbydate == 0) {
				v8_1=0;
				if ((!((t16_3==v16_3) && (t16_4==v16_4) && (t16_5==v16_5))) || PrtItemStr.sellbydate) v8_1=1;
			}
			if (v8_1) rtc_makedate_str(send_string, (INT8U)v16_3, (INT8U)v16_4, (INT8U)v16_5);
			strcat(send_string, " \n");
		}
		else if(ExtPrtTotalFlag==ON)
		{
			sprintf(send_string,"	  ");
			strcat(send_string, " \n"); 	
		}
		else
		{
			sprintf(send_string," \n"); 	
		}
	}
	else
	{
		sprintf(send_string," \n"); 	
	}
	MsgOut(send_string);
}

void external_print_sellbytime(char *send_string)
{
	INT8U	v8_1, v8_2, v8_3;
#ifdef USE_SELLBYDATETIME_ZERO
	if(PrtItemStr.sellbydate || PrtItemStr.sellbytime) 
#else
	if(PrtItemStr.sellbydate || PrtItemStr.sellbytime || PrtItemStr.flagsellbytime == 2 || PrtItemStr.flagsellbytime == 3) 
#endif
	{
		v8_1 = PrtComStr.time.hour + PrtItemStr.packedontime + PrtItemStr.sellbytime;		 // current time + Packed Time + Sell by Time
		v8_2 = PrtComStr.time.min;
		v8_3 = PrtComStr.time.sec;

		v8_1 %= 24;	 //다음날 시간. 
		rtc_maketime_str(send_string,(INT8U)v8_1,(INT8U)v8_2,(INT8U)v8_3);
		strcat(send_string, " \n");		
	}
	else if(ExtPrtTotalFlag==ON)
	{
		sprintf(send_string,"		 ");
		strcat(send_string, " \n"); 
	}
	else
	{
		sprintf(send_string," \n"); 		
	}
	MsgOut(send_string);
}
void external_print_packedbydate(char *send_string, INT8U prepack_type)
{
	INT8U	v8_1;
	INT16U	v16_1, v16_3, v16_4, v16_5;    
	
	if(PrtItemStr.packedondate <= 999)
	{
		v8_1 = PrtComStr.time.hour + PrtItemStr.packedontime;	 // current hour + packed time
		v8_1 /= 24;	 // calculate day from hour( 0~23 = 0, 24~47 = 1 ...)
		v16_1 = (INT16U)v8_1 + PrtItemStr.packedondate;
		v16_3 = (INT16U)PrtComStr.time.year;
		v16_4 = (INT16U)PrtComStr.time.month;
		v16_5 = (INT16U)PrtComStr.time.date;

		date_getnextday((short*)&v16_3,(short*)&v16_4,(short*)&v16_5,(short)v16_1);
		v8_1 = date_getweek(v16_3, v16_4, v16_5);
		switch (prepack_type) {
		 default:
			 case 0:rtc_makedate_str(send_string, (INT8U)v16_3, (INT8U)v16_4, (INT8U)v16_5);
				 break;
			 case 1:sprintf(send_string, "%c", 'A'+(v8_1+7-1)%7);
				 break;
			 case 2:get_global_sparam(GLOBAL_DATE_WEEKARRAY+(v8_1*10), (INT8U *)send_string, 10);
				 send_string[10] = 0; 
				 break;
			 case 3:rtc_makedate_str(send_string, (INT8U)v16_3, (INT8U)0, (INT8U)v16_5);	 //add by son remove 070126
				 break;
		}
	}
	strcat(send_string, " \n");
	MsgOut(send_string);
}
void external_print_packedbytime(char *send_string)
{
	INT8U	v8_1, v8_2, v8_3;
	v8_1 = PrtComStr.time.hour + PrtItemStr.packedontime;	 // current hour + packed time
	v8_2 = PrtComStr.time.min;
	v8_3 = PrtComStr.time.sec;
	v8_1 %= 24;

	rtc_maketime_str(send_string,(INT8U)v8_1,(INT8U)v8_2,(INT8U)v8_3);
	strcat(send_string, " \n");
	MsgOut(send_string);
}

#ifdef USE_TRACE_STANDALONE
void external_print_slaughtdate(char *send_string)
{
	if(FlagDisplayIndividual && CurTraceStatus.indivStr.slaughtDate)
	{
		rtc_makedate_str(send_string, CurTraceStatus.indivStr.slaughtDate[0], CurTraceStatus.indivStr.slaughtDate[1], CurTraceStatus.indivStr.slaughtDate[2]);
		strcat(send_string, " \n");		
	}
	else if(ExtPrtTotalFlag==ON)
	{
		sprintf(send_string,"		 ");
		strcat(send_string, " \n");	
	}
	else
	{
		sprintf(send_string," \n"); 		
	}
	MsgOut(send_string);

}
#endif

void total_external_print(INT8U sellbydate_check, INT8U prepack_type)
{
	char	name1_string[56];
	char	send_string[600];
	char	bar_form[BARCODE_FORMAT_LEN+1];
	char	bar_str[BARCODE_FORMAT_LEN+1];
	char	bar_type;
	char	w_sign1[5], w_sign2[5];	
	char	curDate_buf[15], curTime_buf[15], ttlTotalPrice_buf[15], ttlTotalCount_buf[15], ttlVoidPrice_buf[15];
	char	ttlVoidCount_buf[15], ttlPayment_buf[15], ttlChange_buf[15], currencyRate_buf[15], ttlTotalWeight_buf[15];
	char	ttlTotalQuantity_buf[15], netWeight_buf[15], secondNetWeight_buf[15], grossWeight_buf[15], ttl2ndTotalPrice_buf[15];
	char	currency_sign1[5], ttlTotalTitle_buf[15];
	INT8U	dec_ch;
	INT8U	v8_1, v8_2;
	INT8U	formName[7];	
	INT8U	barcode_type;
	INT8U	wt_prt_type;
	INT8U	decimal, len; 
	INT16U  usize;
	INT16U  i;
	INT16U	barid;	
	INT16S	fieldid;
	INT32U	v32_1;
	STRUCT_STRFORM_PARAM	bar_param;		

	ttlTotalPrice_buf[0] = 0;
	strcpy((char*)formName,"TOTAL\0");

	decimal = get_global_bparam(GLOBAL_AREA_PRICE_DECIMALPOINT);
	dec_ch	= get_global_bparam(GLOBAL_DECIMAL_SIGN);

	wt_prt_type = 0;
	if(PrtWeightUnit == WEIGHT_UNIT_KG || PrtWeightUnit == WEIGHT_UNIT_G) {
		get_global_sparam(GLOBAL_WEIGHT_SYMBOL_1, (INT8U *)w_sign1, 4);
		get_global_sparam(GLOBAL_WEIGHT_SYMBOL_2, (INT8U *)w_sign2, 4);
	} else {
		get_global_sparam(GLOBAL_WEIGHT_LBSYMBOL_1, (INT8U *)w_sign1, 4);
		get_global_sparam(GLOBAL_WEIGHT_LBSYMBOL_2, (INT8U *)w_sign2, 4);
	}

	// make barcode
	bar_str[0] = 0x00;
	if(prtsel_get_table(FIELD_ID_BARCODE)){

		for (fieldid=0; fieldid<MAX_BAR_FIELD_NUM; fieldid++) {
			if (BarFieldStruct[fieldid].ExistField) {
				prt_set_strform_parameter_total(&bar_param);
				if (fieldid == 2)
					barid = get_global_bparam(GLOBAL_BARCODE_TOTAL_ID2);
				else
					barid = get_global_bparam(GLOBAL_BARCODE_TOTAL_ID);
				if(GlbFlagPCnMasterConnection==OFF) barid=99;


			}
		if (barid<1 || barid>99) barid=0;
		if(!get_barcodeformat(barid,(char *)bar_form, &barcode_type)) bar_form[0]=0x00;
		
		bar_type=barcodetype_codevalue[barcode_type];
		prt_make_barform(bar_str, bar_form, &bar_param, bar_type);
		}		
	}

	plu_get_field(PrtItemStr.plu_addr-1, 10, (INT8U *)&name1_string, (INT16S *)&usize, sizeof(name1_string)); // name1
	if(name1_string[0] == 0xFF) name1_string[0] = 0x00;

	//EMART 축산 컴파일 버그로 인한 수정.
	//V00 : 바코드 //V01 : 상품명 1단
	external_print_name1(send_string, formName,bar_str,name1_string);
	//V02 : packed date 
	external_print_packedbydate(send_string, prepack_type);
	//V03 : packed time
	external_print_packedbytime(send_string);
	//V04 : sell by date 
	external_print_sellbydate(send_string, sellbydate_check);
	//V05 : sell by time
	external_print_sellbytime(send_string);	

	// V06 : Current Date //FIELD_ID_CUR_DATE	
	send_string[0] = 0;
	i = 0;
	rtc_makedate_str(curDate_buf, PrtComStr.time.year, PrtComStr.time.month, PrtComStr.time.date); 
	sprintf(send_string+i,"%s \n",curDate_buf);
	// V07 : Current Time //FIELD_ID_CUR_TIME
	i=strlen((char *)send_string);
	rtc_maketime_str(curTime_buf, PrtComStr.time.hour, PrtComStr.time.min, PrtComStr.time.sec); 
	sprintf(send_string+i,"%s \n",curTime_buf);
	// V08 : with Tax or without Tax //FIELD_ID_TTL_TOTAL_PRICE
	i=strlen((char *)send_string);
	format_value((HUGEDATA INT8U *)ttlTotalPrice_buf,PrtTotalStr.SummaryTotalPrice,decimal,dec_ch); 
	sprintf(send_string+i,"%s \n",ttlTotalPrice_buf);
	//V09 : FIELD_ID_TTL_TOTAL_COUNT
	i=strlen((char *)send_string);
	sprintf(ttlTotalCount_buf, "%d", PrtTotalStr.TotalCount); 
	sprintf(send_string+i,"%s \n",ttlTotalCount_buf);
	//V10 : FIELD_ID_TTL_VOID_PRICE
	i=strlen((char *)send_string);
	format_value((HUGEDATA INT8U *)ttlVoidPrice_buf,PrtTotalStr.VoidPrice,decimal,dec_ch); 
	sprintf(send_string+i,"%s \n",ttlVoidPrice_buf);
	//V11 : FIELD_ID_TTL_VOID_COUNT
	i=strlen((char *)send_string);
	sprintf(ttlVoidCount_buf, "%d", PrtTotalStr.VoidCount); 
	sprintf(send_string+i,"%s \n",ttlVoidCount_buf);
	//V12 : FIELD_ID_TTL_PAYMENT
	i=strlen((char *)send_string);
	format_value((HUGEDATA INT8U *)ttlPayment_buf,PrtTotalStr.PayPrice,decimal,dec_ch); 
	sprintf(send_string+i,"%s \n",ttlPayment_buf);
	//V13 : FIELD_ID_TTL_CHANGE
	i=strlen((char *)send_string);
	format_value((HUGEDATA INT8U *)ttlChange_buf,PrtTotalStr.ChangePrice,decimal,dec_ch); 
	sprintf(send_string+i,"%s \n",ttlChange_buf);
	//V14 : FIELD_ID_TTL_2ND_TOTAL_PRICE
	i=strlen((char *)send_string);
	PrtGenCurPriceStr_2long(ttl2ndTotalPrice_buf, PrtTotalStr.CPrice.CurrencyPriceH8, PrtTotalStr.CPrice.CurrencyPriceL8, PrtTotalStr.CPrice.CurrencyPrtForm, 
					PrtTotalStr.CPrice.CurrencyString1, PrtTotalStr.CPrice.CurrencyString2, 
					PrtTotalStr.CPrice.CurrencyPriceDiv, dec_ch);
	sprintf(send_string+i,"%s \n",ttl2ndTotalPrice_buf);
	//V15 : FIELD_ID_CURRENCY_RATE
	i=strlen((char *)send_string);
	get_global_sparam(GLOBAL_AREA_PRICE_PRIMARYSIGN,(INT8U *)currency_sign1,4);
	currency_sign1[4] = 0;
	len = PrtGenCurPriceStr(currencyRate_buf, 1, 2, currency_sign1, currency_sign1, 0, dec_ch);
	currencyRate_buf[len++] = ' ';
	currencyRate_buf[len++] = '=';
	currencyRate_buf[len++] = ' ';
	PrtGenCurPriceStr(currencyRate_buf+len, PrtTotalStr.CPrice.CurrencyRate, 2, 
			PrtTotalStr.CPrice.CurrencyString1, PrtTotalStr.CPrice.CurrencyString2, 
			PrtTotalStr.CPrice.CurrencyRateDiv, dec_ch);
	sprintf(send_string+i,"%s \n",currencyRate_buf);
	//V16 : FIELD_ID_TTL_TOTAL_WEIGHT
	i=strlen((char *)send_string);
	v8_1=get_decimal_pt(3, PrtWeightUnit); 
	v8_2=dec_ch;
	PrtGenWeightStr(ttlTotalWeight_buf, PrtTotalStr.TotalWeight, wt_prt_type, w_sign1, w_sign2, v8_1, v8_2);
	sprintf(send_string+i,"%s \n",ttlTotalWeight_buf);
	//V17 : FIELD_ID_TTL_TOTAL_QUANTITY
	i=strlen((char *)send_string);
	sprintf(ttlTotalQuantity_buf, "%d", PrtTotalStr.TotalQuantity); 
	sprintf(send_string+i,"%s \n",ttlTotalQuantity_buf);
	//V18 : FIELD_ID_NET_WEIGHT
	i=strlen((char *)send_string);
	v8_1=get_decimal_pt(4, PrtItemStr.plu_addr); 
	v8_2=dec_ch;
	PrtGenWeightStr(netWeight_buf, PrtTotalStr.TotalNetWeight, wt_prt_type, w_sign1, w_sign2, v8_1, v8_2);
	sprintf(send_string+i,"%s \n",netWeight_buf);
	//V19 : FIELD_ID_2ND_NET_WEIGHT
	i=strlen((char *)send_string);
	v32_1 = PrtTotalStr.TotalPerTareWeight + PrtTotalStr.TotalNetWeight; 
	v8_1=get_decimal_pt(4, PrtItemStr.plu_addr);
	v8_2=dec_ch;
	PrtGenWeightStr(secondNetWeight_buf, v32_1, wt_prt_type, w_sign1, w_sign2, v8_1, v8_2);
	sprintf(send_string+i,"%s \n",secondNetWeight_buf);
	//V20 : FIELD_ID_GROSS_WEIGHT
	i=strlen((char *)send_string);
	v8_1=get_decimal_pt(4, PrtItemStr.plu_addr); 
	v8_2=dec_ch;
	PrtGenWeightStr(grossWeight_buf, PrtTotalStr.TotalGrossWeight, wt_prt_type, w_sign1, w_sign2, v8_1, v8_2);
	sprintf(send_string+i,"%s \n",grossWeight_buf);
	MsgOut(send_string);
	//V21 : FIELD_ID_TTL_TOTAL_TITLE
	i=0;
	if(ExtPrtTotalTypeFlag == GTOTAL_LABEL)
	{
		label_caption_read(LABCAP_ID_TXT_TITLE_GTOTAL, ttlTotalTitle_buf);
	}
	else if(ExtPrtTotalTypeFlag == TOTAL_LABEL) 
	{
		label_caption_read(LABCAP_ID_TXT_TITLE_TOTAL, ttlTotalTitle_buf);
	} 
	sprintf(send_string+i,"%s \n",ttlTotalTitle_buf);
	MsgOut(send_string);

	ExtPrtTotalFlag = OFF;
	ExtPrtTotalTypeFlag = 0;
}
void PLU_external_print(INT8U sellbydate_check, INT8U prepack_type)
{
#ifdef USE_TRACE_STANDALONE
	char	indiv_str[26];
	char	slaughtHouse[TRACE_SLAUGHTHOUSE_NAME_SIZE+1];
	char	grade_list[65];
	char	grade_mark[32];
	INT16U	tempSize;
	char	breed[26];			//품종	
#endif
	char	name1_string[56];
	char	name2_string[56];
	char	name3_string[56];
	char	name4_string[56];
	char	name5_string[56];
	char	send_string[600];
	char	send_string2[10];
	char	weightBuf[15];
	char	grossBuf[15];
	char	tareBuf[15];
	char	currency[5];
	char	priceBuf[15];
	char	bar_form[BARCODE_FORMAT_LEN+1];
	char	bar_str[BARCODE_FORMAT_LEN+1];
	char	bar_type;
	INT8U	use_unit;
	INT8U	dec_pt;
	INT8U	dec_ch;
	INT8U	v8_1;
#ifdef USE_TRACE_STANDALONE	
	INT8U	v8_2;	
	INT8U	v8_3;	
	INT8U	len;
#endif
	INT8U	backup_prtfirstlabel;
	INT8U	formName[7];	
	INT8U	barcode_type;
	INT8U	ingredient_line;
	INT16U	usize;
	INT16U	ingredient_no;
	INT16U	i;
	INT16U	j, k, start;
	INT16U	ingredient_size;
	INT16U	barid;	
	INT16S	fieldid;
	INT32U	addr;
	RTC_STRUCT	time_str;	
	STRUCT_STRFORM_PARAM	bar_param;

	if (!status_scale.cur_pluid)
	{
		BuzOn(2);
		return;
	}

	if(PluType()==PLU_BY_WEIGHT) //무게상품일경우 안정될때까지 기다려야 함
	{
		while(!ADM_GetData.Stable)
		{
			ProcessAll();
			if (KEY_Read()) 
			{
				switch (KeyDrv.CnvCode) 
				{
					case KS_CLEAR:  
						BuzOn(1);
						return;
					default:
						BuzOn(2); 
						break;
				}
			}
		}
	}

	Report_GetCurTime(&time_str,1);
	backup_prtfirstlabel = PrtFirstLabel;
	PrtFirstLabel = ON;
	SaleInsertLabelData();
	Prt_SetPrintData(ITEM_LABEL, &time_str, status_scale.departmentid, status_scale.cur_pluid);
	PrtFirstLabel = backup_prtfirstlabel;

	weightBuf[0] = 0;
	grossBuf[0] = 0;
	tareBuf[0] = 0;
#ifdef USE_TRACE_STANDALONE	
	indiv_str[0] = 0;
#endif
	currency[0] = 0;

	plu_get_field(PrtItemStr.plu_addr-1, PTYPE_PLU_NAME1, (INT8U *)&name1_string, (INT16S *)&usize, sizeof(name1_string)); // name1
	plu_get_field(PrtItemStr.plu_addr-1, PTYPE_PLU_NAME2, (INT8U *)&name2_string, (INT16S *)&usize, sizeof(name2_string)); // name2
	plu_get_field(PrtItemStr.plu_addr-1, PTYPE_PLU_NAME3, (INT8U *)&name3_string, (INT16S *)&usize, sizeof(name3_string)); // name3
	plu_get_field(PrtItemStr.plu_addr-1, PTYPE_PLU_NAME4, (INT8U *)&name4_string, (INT16S *)&usize, sizeof(name4_string)); // name4
	plu_get_field(PrtItemStr.plu_addr-1, PTYPE_PLU_NAME5, (INT8U *)&name5_string, (INT16S *)&usize, sizeof(name5_string)); // name5

	plu_get_field(PrtItemStr.plu_addr-1, PTYPE_INGREDIENT_NO, (INT8U *)&ingredient_no, (INT16S *)&usize, sizeof(ingredient_no));  //Indirect Ingredient

	get_base_sparam(FLASH_EXT_PRT_LABEL_NAME, formName, 7);

	if (PluType() == PLU_BY_WEIGHT)
	{
		dec_ch = get_global_bparam(GLOBAL_DECIMAL_SIGN); 
		use_unit = ADM_Status.UseUnit;
		if(use_unit == WEIGHT_UNIT_KG) 
		{
		 if(get_base_bparam(FLASH_ADC_WUNIT) == WEIGHT_UNIT_G) use_unit = WEIGHT_UNIT_G;
		}
		dec_pt = get_decimal_pt(3, use_unit);
		//GetWeightSymbol(use_unit, w_sign1);
		format_value((HUGEDATA INT8U *)weightBuf, PrtItemStr.l_Price.Weight[PRICE_RESULT] , dec_pt, dec_ch);
		//strcat(weightBuf, w_sign1);
	}
	else if (PluType() == PLU_BY_COUNT)
	{
		if(status_scale.Weight > 0)
		{
			BuzOn(4);
			DisplayMsg(global_message[MSG_23_C803]);//"Remove weight on tray"
			return;
		}
		format_value((HUGEDATA INT8U *)weightBuf, PrtItemStr.l_Price.Qty[PRICE_RESULT], 0, 0);
 	}
	//make barcode
	fieldid = 1;
	if (BarFieldStruct[fieldid].ExistField) 
	{
		prt_set_strform_parameter_item(&bar_param, NULL);	
		barid = prt_get_plu_barid(PrtItemStr.l_PluNo, PrtItemStr.plu_addr, PrtItemStr.l_PluType, fieldid);
		if (BarcodeNumber)
		{
			barid = BarcodeNumber;
		}
		if(barid != 0xff)
		{  
			//prt_make_barcode(LABEL, barid, &bar_param, fieldid);
			if (barid<1 || barid>99) barid=0;
			if(get_barcodeformat(barid,(char *)bar_form, &barcode_type))
			{  
				bar_type=barcodetype_codevalue[barcode_type];
				prt_make_barform(bar_str, bar_form, &bar_param, bar_type);
			}
		}
	}

	//GS 축산 컴파일 버그로 인한 수정.
	//V00 : 바코드 //V01 : 상품명 1단
	external_print_name1(send_string, formName, bar_str, name1_string);
	//V02 : PLU no.
	i = 0;
	sprintf(send_string+i,"%ld\n",status_scale.cur_pluid);
	//V03 : 상품명 2단
	i=strlen((char *)send_string);
	sprintf(send_string+i,"%s \n",name2_string);
	//V04 : 상품명 3단
	i=strlen((char *)send_string);
	sprintf(send_string+i,"%s \n",name3_string);
	//V05 : weight 	
	i=strlen((char *)send_string); 				 
	sprintf(send_string+i,"%s \n",weightBuf);
	MsgOut(send_string);

	//V06 : unit price
	i = 0;
	priceBuf[0] = 0;
#if defined(_USE_LCD20848_)
	FloatToString(TO_STR,priceBuf,8,DISPLAY_PRICE,&PrtItemStr.l_Price.UnitPrice[PRICE_RESULT],0,1);
#else
	FloatToString(TO_STR,priceBuf,8,DISPLAY_PRICE,&PrtItemStr.l_Price.UnitPrice[PRICE_RESULT],0,0);
#endif
	sprintf(send_string,"%s%s\n",priceBuf,currency);	 				
	//V07 : total price
	i=strlen((char *)send_string);
	priceBuf[0] = 0;
#if defined(_USE_LCD20848_)
	FloatToString(TO_STR,priceBuf,8,DISPLAY_PRICE,&PrtItemStr.l_Price.TotalPrice[PRICE_RESULT],0,1);
#else
	FloatToString(TO_STR,priceBuf,8,DISPLAY_PRICE,&PrtItemStr.l_Price.TotalPrice[PRICE_RESULT],0,0);
#endif
	sprintf(send_string+i,"%s%s\n",priceBuf,currency);	 				
	MsgOut(send_string);

	//V08 : packed date 
	external_print_packedbydate(send_string, prepack_type);
	//V09 : packed time
	external_print_packedbytime(send_string);
	//V10 : sell by date 
	external_print_sellbydate(send_string, sellbydate_check);	
	//V11 : sell by time
	external_print_sellbytime(send_string);															 
	
#ifdef USE_TRACE_STANDALONE	//국내는 성분 9줄 사용(V12~V20)
	//V12~V20 : Indirect Ingredient(10 lines)
	ingredient_line = 9;
#else
	//V12~V21 : Indirect Ingredient(10 lines)
	ingredient_line = 10;
#endif
	send_string[0] = 0;
	i = 0;
	if (ingredient_no) 
	{
		addr =  ing_calc_addr_by_key(ingredient_no-1);
		if(ingredient_no == Flash_wread(addr))
		{
			ingredient_size = INGREDIENT_STRUCT_SIZE-2;
			if (ingredient_size > 510) ingredient_size = 510;
			Flash_sread(addr+2, (INT8U*)send_string, ingredient_size);
			k = 0;
			for (j = 0; j < ingredient_line; j++)
			{
				if (send_string[k] == 0) break;
				start = k;
				for (; k < ingredient_size; k++)
				{
					if (send_string[k] == 0x0d || send_string[k] == LF)
					{
						if (send_string[k+1] == 0x0d || send_string[k+1] == LF)	 // "CR+LF"일 때
						{
							send_string[k] = 0;
							k++;
						}
						break;
					}
				}
				send_string[k] = 0; // LF일 때 강제로 줄 바꿈
				MsgOut(&send_string[start]);
				sprintf(send_string2, " \n");
				MsgOut(send_string2);
				k++;
			}
			sprintf(send_string, " \n");
			for (; j < ingredient_line; j++) // 남은 line은 빈 데이터로 전송
			{
				MsgOut(send_string);
			}
		}
		else
		{
			for(i = 0; i < ingredient_line; i++)
			{
				sprintf(send_string," \n");
				MsgOut(send_string);
			}
		}
	}
	else
	{
		for(i = 0; i < ingredient_line; i++)
		{
			 sprintf(send_string," \n");
			 MsgOut(send_string);
		}
	}
#ifdef USE_TRACE_STANDALONE
	//V21 : 근내지방도 표시 //// 등급번호 1, 8,9,10 -> 1++, 1++(7), 1++(8),1++(9) 해당 
	i = 0;
	v8_1 = 0;
	memset(grade_mark, 0, sizeof(grade_mark));
	if(CurTraceStatus.gradeNo || (TraceMeatGradeListNum[CurTraceStatus.gradeNo - 1] != 0))
	{
		if(((CurTraceStatus.gradeNo >= 7) && (CurTraceStatus.gradeNo <= 9)) || (CurTraceStatus.gradeNo == 1))
		{
			grade_mark[v8_1++] = 0xA1;	// 특수기호 원 (0xA1DB)
			grade_mark[v8_1++] = 0xDB;

			grade_mark[v8_1] = 0;

			sprintf(send_string,"%s \n", grade_mark);
			MsgOut(send_string);
		}
		else
		{
			sprintf(send_string," \n");
			MsgOut(send_string);
		}
	}
	else
	{
		sprintf(send_string," \n");
		MsgOut(send_string);
	}
#endif
	//V22 : 상품명 4단
	i = 0;
	sprintf(send_string,"%s \n",name4_string);
	//V23 : 상품명 5단	
	i=strlen((char *)send_string);
	sprintf(send_string+i,"%s \n",name5_string);			 							
	MsgOut(send_string);

#ifdef USE_TRACE_STANDALONE
	if (FlagDisplayIndividual && CurTraceStatus.indivStr.individualNO)
	{
		strcpy((char *)indiv_str,(char *)CurTraceStatus.indivStr.individualNO);

		if(CurTraceStatus.slPlace)
		{
			addr =  DFLASH_BASE + FLASH_TRACE_SLAUGHTERHOUSE_AREA;
			addr =  addr + TRACE_SLAUGHTERHOUSE_KR_STRUCT_SIZE * (INT32U)(CurTraceStatus.slPlace-1);
			usize = Flash_wread(addr);
		#ifdef USE_KOR_SLAUGHT_CODE_4DIGIT
			if(trace_slaughthouse_index2no(CurTraceStatus.slPlace) == usize)
		#else
			if(CurTraceStatus.slPlace == usize)
		#endif
			{
				addr += 2;
				tempSize = TRACE_SLAUGHTHOUSE_NAME_SIZE;
				Flash_sread(addr, (HUGEDATA INT8U *)slaughtHouse, tempSize);
			}
		} 
		else strcpy(slaughtHouse, (char *)CurTraceStatus.indivStr.slaughtHouse);

		if(CurTraceStatus.indivStr.breed)
			strcpy(breed, CurTraceStatus.indivStr.breed);
		else
			sprintf(breed, " ");
	}
	else
	{
		sprintf(indiv_str, " ");
		sprintf(slaughtHouse, " ");
		sprintf(breed, " ");
	}
	//V24 : 개체번호.
	i = 0;
	if(indiv_str[0] != 0)
		sprintf(send_string+i,"%s\n", indiv_str);		 		
	else
		sprintf(send_string+i," \n");
	//V25 : 도축장	
	i=strlen((char *)send_string);
	sprintf(send_string+i,"%s \n", slaughtHouse);
	//V26 : 등급리스트	
	i=strlen((char *)send_string);
	if(CurTraceStatus.gradeNo || (TraceMeatGradeListNum[CurTraceStatus.gradeNo - 1] != 0))
	{
		if (CurTraceStatus.gradeNo <= 10)
		{
			v8_1 = 0;	// Beef
		}
		else
		{
			v8_1 = 1;	// Pork
		}
		if(v8_1) //돼지 등급
		{
			sprintf(grade_list, "등급: ");
			len = strlen(grade_list);
			sprintf(grade_list+len, (char*)&ExterPrt_TraceMeatGradeListText[v8_1][6]); //6이 1+스트링 시작 위치가 1+ 앞에 인쇄
		}
		else //소 등급 
		{
			len = 0;
			trace_meatgrade_getGradeText(CurTraceStatus.gradeNo, CurTraceStatus.curGradeText);
			if((CurTraceStatus.gradeNo >= 7) && (CurTraceStatus.gradeNo <= 9)) // 등급번호 8,9,10 -> 1++(7), 1++(8),1++(9) 중 번호에 맞는 한개만 인쇄.
			{
				sprintf(grade_list, "등급(마블링):");
				len = strlen(grade_list);
				sprintf(grade_list+len, "%s",CurTraceStatus.curGradeText);
				len = strlen(grade_list);
			}
			else
			{
				sprintf(grade_list, "등급(마블링):");
				len = strlen(grade_list);
				sprintf(grade_list+len, "1++( )"); //등급번호 1,2,3,4,5 -> 1++( ) 가 1+ 앞에 인쇄
				len = strlen(grade_list);
			}
			sprintf(grade_list+len, (char*)&ExterPrt_TraceMeatGradeListText[v8_1][30]); //30이 1+스트링 시작 위치
		}
	}
	else
	{
		sprintf(grade_list," ");
	}
	sprintf(send_string+i,"%s \n", grade_list);
	//V27 : 등급표시	
	i=strlen((char *)send_string);
	memset(grade_mark, 0, sizeof(grade_mark));
	if(CurTraceStatus.gradeNo || (TraceMeatGradeListNum[CurTraceStatus.gradeNo - 1] != 0))
	{
		v8_1 = 0;
		v8_3 = 0 ;

		if (CurTraceStatus.gradeNo <= 10)	// Beef
		{
			if(((CurTraceStatus.gradeNo >= 7) && (CurTraceStatus.gradeNo <= 9)) || (CurTraceStatus.gradeNo == 1))// 등급번호 8,9,10 -> 1++(7), 1++(8),1++(9) 는 V21에서 표시
			{
				sprintf(grade_mark," ");
			}
			else	
			{
				v8_3 = CurTraceStatus.gradeNo;
				v8_3 += 3;		// 소 등급 표시 시작 위치
			}
		}
		else	// Pork
		{
			v8_3 = CurTraceStatus.gradeNo - 10; //돼지 등급은 index 11 부터 시작
			grade_mark[v8_1++] = ' ';
		}

		// 등급 별 등급표시 위치 조정
		if((CurTraceStatus.gradeNo >= 3) && (CurTraceStatus.gradeNo <= 6))	// Beef 1, 2, 3, 4, 등외 등급
		{
			if(CurTraceStatus.gradeNo == 3) // Beef 1 등급
				grade_mark[v8_1++] = ' ';
			
			v8_3--;
		}
		if((CurTraceStatus.gradeNo >= 13) && (CurTraceStatus.gradeNo <= 14)) // Pork 3, 등외 등급
		{
			grade_mark[v8_1++] = ' ';
			v8_3--;
		}

		for (v8_2 = 0; v8_2 < v8_3 - 1; v8_2++)
		{
			grade_mark[v8_1++] = ' ';
			grade_mark[v8_1++] = ' ';
		}
		grade_mark[v8_1++] = 0xA1;	// 특수기호 원 (0xA1DB)
		grade_mark[v8_1++] = 0xDB;

		grade_mark[v8_1] = 0;
	}
	else
	{
		sprintf(grade_mark," ");
	}
	if(((CurTraceStatus.gradeNo >= 7) && (CurTraceStatus.gradeNo <= 9)) || (CurTraceStatus.gradeNo == 1)) // 등급번호 8,9,10 -> 1++(7), 1++(8),1++(9) 는 V21에서 표시
		sprintf(send_string+i," \n");
	else
		sprintf(send_string+i,"%s \n", grade_mark);
	//V28 : 품종
	i=strlen((char *)send_string);
	sprintf(send_string+i,"%s \n", breed);
	MsgOut(send_string);
	//V29 : 도축일자
	external_print_slaughtdate(send_string);
	//MsgOut(send_string);
#else
	// by-weight mode
	if(PluType() == PLU_BY_WEIGHT) 
	{												
		v8_1 = get_global_bparam(GLOBAL_PRINT_OPER_FLAG2);
		if((v8_1&0x02) && (PrtItemStr.l_PluTareWeight || PrtItemStr.l_KeyTareWeight|| PrtItemStr.l_Price.PercentTareWeight))
		{
			sprintf(tareBuf, "PT ");
		} 
		else 
		{
			sprintf(tareBuf, "");
		}
		format_value((HUGEDATA INT8U *)grossBuf, Price.GrossWeight, dec_pt, dec_ch);
		format_value((HUGEDATA INT8U *)tareBuf+strlen(tareBuf), PrtItemStr.l_TareWeight, dec_pt, dec_ch);
	}
	//V24 : GrossWeight
	i = 0;
	sprintf(send_string+i,"%s \n",grossBuf);		
	//V25 : tare	
	i=strlen((char *)send_string);
	sprintf(send_string+i,"%s \n",tareBuf); 		
	MsgOut(send_string);
#endif
}

void start_external_print(INT16U count)
{
	char	send_string[600];
	INT8U	sellbydate_check,prepack_type;
	INT16U  i;
	INT8U   status;
	/*
	 * 현재 외부 프린트에서 셀프모드 및 개수 상품 발행에 대한 고려는 하지 않았으므로 check = 0으로 함
	 * 만일 고려가 필요시 SelfserviceAutoCheck()를 참조하여 check 값을 고려할 것
	 */
	INT8U   check = 0; 

	if (!count && count > 999) return;

	if(ADM_GetData.Overload || PercentTareOver) return;// (For NMi) Added by CJK 20050713
	if(!CheckPluPrint(0))	 return;

	sellbydate_check= get_global_bparam(GLOBAL_SALE_SETUP_FLAG5)&0x01;
	prepack_type  = get_global_bparam(GLOBAL_SALE_SETUP_FLAG5);
	prepack_type>>=1;
	prepack_type&=0x03;

	status = check_status_weight(check);

	if (status) {
		disp_error_weight_msg(status);
	} else {
		if(ExtPrtTotalFlag == ON) // Print Total Label
		{
			total_external_print(sellbydate_check, prepack_type);
		}
		else //if(ExtPrtTotalFlag == ON)  // Print PLU Label
		{
			PLU_external_print(sellbydate_check, prepack_type);	
		}
		i = 0;
		sprintf(send_string+i,"ZB\n");  // printer direction : bottom
		i=strlen((char *)send_string);
		sprintf(send_string+i,"P%d.1\n", count);
			
		MsgOut(send_string);

		disp_msg_ext_prt();
	}
}
	
#ifdef USE_TRACE_STANDALONE
#ifdef USE_RFID_TAG_READER
	//extern INT8U slen;
extern INT8U RFIDRcvStateStart;
extern void commun_send_call_rfid_tag(void);
extern INT8U commun_recv_rfid_tag(INT8U *reply_data, INT16U *length, INT8U *res_code);
INT8U keyapp_call_individual(INT16U indiv_index, INT8U beep);
void keyapp_send_call_tag(void)
{
	INT16U len;
	INT8U ret;
	INT8U res_code;
	INT16U ind_idx;
	INT8U string_buf[50];
	
	POINT disp_p;
	char old_font;
	INT8U old_page;
	
	old_font = DspStruct.Id1;
	old_page = DspStruct.Page;
	Dsp_SetPage(DSP_ERROR_PAGE);
	FontSizeChange(FONT_MEDIUM);
	display_lcd_clear_buf();  // Clear buf PLU Name Area
	
	disp_p = point(DSP_PLU_NAME_Y, DSP_PLU_NAME_X);
	sprintf((char*)string_buf, "태그를 대주세요.");
	
	Dsp_Write_String(disp_p,(HUGEDATA char*)string_buf);
	Dsp_Diffuse();
	display_lcd_diffuse();	// Redraw PLU Name Area
	
	commun_send_call_rfid_tag();
	RFIDRcvStateStart = 1;
	//slen = 0;
	network_timeout_reset(NETWORK_TIMEOUT_TIMESYNC);
	network_counter_reset(NETWORK_TIMEOUT_TIMESYNC);
	
	Delay_100ms(2);
	while(1)
	{
		if (network_timeout_check(10,NETWORK_TIMEOUT_TIMESYNC)) 
		{
			if (network_counter_check(2,NETWORK_TIMEOUT_TIMESYNC)) 
			{
				commun_send_call_rfid_tag();
				RFIDRcvStateStart = 1;
				//slen = 0;
				//Delay_100ms(2);
			} 
			else 
			{
				BuzOn(4);
				sprintf((char*)string_buf, "실패하였습니다.");
				display_message_page_mid((char*)string_buf);
				break;
			}
		}
		res_code = 0;
		len = 0;
		ret = commun_recv_rfid_tag(CalledTraceStatus.indivStr.individualNO, &len, &res_code);
		// 빠져나가는 조건
		if (ret)
		{
			//BuzOn(1);
			if (res_code == 0x01)
			{
				BuzOn(2);
				CalledTraceStatus.indivStr.individualNO[len] = 0;
				//sprintf(MsgBuf, "%s", CalledTraceStatus.indivStr.individualNO);
				//PutString(24,0,MsgBuf,2,1,1,strlen(MsgBuf));
				individual_checkZero(CalledTraceStatus.indivStr.individualNO);
				ind_idx = individualData_search(CalledTraceStatus.indivStr.individualNO, 0);
				keyapp_call_individual(ind_idx, OFF);
				break;
			}
		}
		if(KEY_IsKey()) 
		{
			KEY_InKey();
			BuzOn(1);
			break;
		}
	}
	Dsp_SetPage(old_page);
	Dsp_Diffuse();
	DspLoadFont1(old_font);
}
#endif
#endif



#ifdef USE_RFID_TAG_READER_RFID_522_B

void keyapp_send_call_tag(void)
{
	INT16U len;
	INT8U ret;
	INT8U res_code;
	INT16U ind_idx;
	INT8U string_buf[50];
	INT8U data_buf[16];
	
	POINT disp_p;
	char old_font;
	INT8U old_page;
	COMM_BUF *CBuf;
	
	CBuf = &CommBufRFID;
	
	old_font = DspStruct.Id1;
	old_page = DspStruct.Page;
	Dsp_SetPage(DSP_ERROR_PAGE);
	FontSizeChange(FONT_MEDIUM);
	display_lcd_clear_buf();  // Clear buf PLU Name Area
	
	disp_p = point(DSP_PLU_NAME_Y, DSP_PLU_NAME_X);
	sprintf((char*)string_buf, "READ RFID TAG");
	
	Dsp_Write_String(disp_p,(HUGEDATA char*)string_buf);
	Dsp_Diffuse();
	//display_lcd_diffuse();	// Redraw PLU Name Area
	
	commun_send_cmd(CBuf, RFID_CMD_READ_DATA, NULL);
	
	network_timeout_reset(NETWORK_TIMEOUT_TIMESYNC);
	network_counter_reset(NETWORK_TIMEOUT_TIMESYNC);
	
	//Delay_100ms(2);
	//debug_tic(0);
	while(1)
	{
		if (network_timeout_check(10,NETWORK_TIMEOUT_TIMESYNC)) 
		{
			if (network_counter_check(0,NETWORK_TIMEOUT_TIMESYNC)) 
			{
				commun_send_cmd(CBuf, RFID_CMD_READ_DATA, NULL);
			} 
			else 
			{
				BuzOn(2);
				sprintf((char*)string_buf, "FAIL(TIMEOUT)");
				display_message_page_mid((char*)string_buf);
				break;
			}
		}
		ret = rfid_interpreter(CBuf);
		// 빠져나가는 조건
		if (ret == RCV_OK && RFID_RcvResult)
		{
			if (RFID_RcvCmd == RFID_CMD_READ_DATA)
			{
				//debug_tic('A');
				BuzOn(1);
				// ex)수신된 데이터의 마지막 byte를 증가시켜 writing함
				memcpy(data_buf, RFID_RcvBuffer, 16);
				data_buf[15]++;
				for (int i = 0; i < 8; i++)
				{
					sprintf((char*)&string_buf[i*2], "%02X", data_buf[i+8]);
				}
				commun_send_cmd(CBuf, RFID_CMD_WRITE_DATA, data_buf);
				display_message_page_mid((char*)string_buf);
				network_timeout_reset(NETWORK_TIMEOUT_TIMESYNC);
				network_counter_reset(NETWORK_TIMEOUT_TIMESYNC);
				//break;
			}
			else if (RFID_RcvCmd == RFID_CMD_WRITE_DATA)
			{
				//debug_tic('B');
				BuzOn(1);
				break;
			}
		}
		else if (ret)	//error
		{
			//debug_tic('C');
			BuzOn(2);
			sprintf((char*)string_buf, "FAIL(%02X)..", ret);
			display_message_page_mid((char*)string_buf);
			break;
		}
		
		if(KEY_IsKey()) 
		{
			KEY_InKey();
			BuzOn(1);
			break;
		}
	}
	Dsp_SetPage(old_page);
	Dsp_Diffuse();
	DspLoadFont1(old_font);
}
#endif //#ifdef USE_RFID_TAG_READER_RFID_522_B


//SG060720
#ifdef USE_TRACE_STANDALONE

extern INT16U IndividualDisplayTime;
extern INT8U  IndividualDisplayTimeFlag;
TRACE_STATUS_STRUCT  CurTraceStatus;
extern SCANNER_DATA_STR  Scanner;
extern INT8U SaveTraceabilityNo;		//Default 값이 "상품개체"
INT8U keyapp_call_individual(INT16U indiv_index, INT8U beep)
{
	INT16U strSize;
	INT32U addr;
	TRACE_INDIVIDUAL_IDENTITY  indivStr;
	static INT16U old_index = 0;
	long indivWeight;
	INT8U string_buf[32];   
	INT16U temp_index;    
	INT8U temp_param;   
	
	memset(&CurTraceStatus.indivStr, 0, sizeof(CurTraceStatus.indivStr));
	if (plu_check_inhibit_ptype(PTYPE_GROUP_NO))	// PLU 등급을 사용하지 않을 때만 개체를 통한 등급 호출이 가능함
	{
		memset(&CurTraceStatus.curGradeText, 0, sizeof(CurTraceStatus.curGradeText));
	}
	if (indiv_index > 0 && indiv_index <= MAX_INDIVIDUAL_NO) //1~MAX_INDIVIDUAL_NO
	{ 
		if (indiv_index == MAX_INDIVIDUAL_NO) // 개체코드 100:MAX_INDIVIDUAL_NOanner or 입력기능키
		{
			CurTraceStatus.indivStr.index = indiv_index;
			memcpy(CurTraceStatus.indivStr.individualNO, CalledTraceStatus.indivStr.individualNO, sizeof(CurTraceStatus.indivStr.individualNO));
			//#ifdef USE_EMART_PIG_TRACE
			//			if(strlen(CalledTraceStatus.indivStr.individualNO) == 24)
			//			{
			//				CurTraceStatus.indivStr.individualNO[24] = 0; //20141111
			//			}
			//			else
			//			{
			//				CurTraceStatus.indivStr.individualNO[INDIVIDUAL_NO_LEN] = 0;
			//			}
			//#else
			CurTraceStatus.indivStr.individualNO[INDIVIDUAL_NO_LEN] = 0;
			//#endif
			CurTraceStatus.indivStr.lotFlag = CalledTraceStatus.indivStr.lotFlag;
			
			if (!CurTraceStatus.indivStr.individualNO[0])
			{
				sale_display_indivFlag_set(INDIV_DISP_MODE_INDIV_NO, OFF);
			}
			else
			{
				sale_display_indivFlag_set(INDIV_DISP_MODE_INDIV_NO, ON);
				if (GlbOper.UseSendTraceInformation) trace_information_send(&(CommBufEthData[0]), NULL);
			}
		}
		else
		{
			strSize = TRACE_INDIVIDUAL_IDENTITY_SIZE;
			addr = (INT32U)(indiv_index-1) * strSize ;
			addr += FLASH_TRACE_STANDALONE_AREA;
  #ifdef USE_TRACE_1000QTY //sewon 161201
			addr += EXT_MEMCARD_ADDR ;
  #else
			addr += DFLASH_BASE;
  #endif
			Flash_sread(addr, (HUGEDATA INT8U *)&indivStr, strSize );
			indivWeight = get_nvram_lparam(NVRAM_TRACE_STANDALONE_WEIGHT+((INT32U)indiv_index-1)*4);
			//sprintf(MsgBuf, "id=%d, harm=%d, no=%s\r\n", indivStr.index, indivStr.harmfulness, indivStr.individualNO);
			//MsgOut(MsgBuf);
			if(indivStr.index==indiv_index)
			{
#ifdef USE_CHECKING_HARMFUL_MEAT
				// 위해리스트 재 검사
				temp_param = get_global_bparam(GLOBAL_SALE_SETUP_FLAG13); //위해 기능 사용 유무 
				if(temp_param != 0)// 위해 개체 사용
				{
					temp_index = individualData_search(indivStr.individualNO, 2);	// 위해리스트 영역만 검색, 존재안할 경우 MAX_INDIVIDUAL_NO return                    
					
					if (indivStr.index < MaxIndividualNo && temp_index == MAX_INDIVIDUAL_NO)	// 개체리스트 영역에 있고, 위해리스트에는 없을 경우
					{
						
						
						memcpy(&CurTraceStatus.indivStr, &indivStr, strSize);
  #ifdef USE_TRACE_MEATGRADE
    #ifdef USE_DISCLOSURE_MEATGRADE
						CurTraceStatus.gradeNo = CurTraceStatus.indivStr.gradeNo;
						trace_meatgrade_getGradeText(CurTraceStatus.gradeNo, CurTraceStatus.curGradeText);
    #else
						if (plu_check_inhibit_ptype(PTYPE_GROUP_NO)) // PLU 등급을 사용하지 않을 때만 등급 호출 가능
						{
							CurTraceStatus.gradeNo = CurTraceStatus.indivStr.gradeNo;
							trace_meatgrade_getGradeText(CurTraceStatus.gradeNo, CurTraceStatus.curGradeText);
						}
	#endif						
  #else // USE_TRACE_MEATGRADE
						strcpy((char *)CurTraceStatus.curGradeText, (char *)CurTraceStatus.indivStr.grade);
  #endif // USE_TRACE_MEATGRADE
						CurTraceStatus.indivWeight = indivWeight;
						
						if (GlbOper.TraceabilityCallType == 0)
						{
							if (!old_index) old_index = indivStr.index; 
							else
							{
								if (old_index != indivStr.index) 
								{
									old_index = indivStr.index;
									CurTraceStatus.slPlace = 0;
								}							 
							}
						}
						sale_display_indivFlag_set(INDIV_DISP_MODE_INDIV_NO, ON);
						if (GlbOper.UseSendTraceInformation) trace_information_send(&(CommBufEthData[0]), NULL);
						
						
						
					}
					else
					{
						BuzOn(2);
						sprintf((char*)string_buf, "담당자 확인 필요 상품");
						//sprintf(string_buf, "위해 쇠고기입니다");
						display_message_page_mid((char*)string_buf);
						sale_display_indivFlag_set(INDIV_DISP_MODE_INDIV_NO, OFF);
						sale_display_update(UPDATE_TITLE);	// CJK070705
						return OFF;
					}
				}
				else // 위해 개체 사용안함 
				{
					memcpy(&CurTraceStatus.indivStr, &indivStr, strSize);
  #ifdef USE_TRACE_MEATGRADE
    #ifdef USE_DISCLOSURE_MEATGRADE
					CurTraceStatus.gradeNo = CurTraceStatus.indivStr.gradeNo;
					trace_meatgrade_getGradeText(CurTraceStatus.gradeNo, CurTraceStatus.curGradeText);
    #else
					if (plu_check_inhibit_ptype(PTYPE_GROUP_NO)) // PLU 등급을 사용하지 않을 때만 등급 호출 가능
					{
						CurTraceStatus.gradeNo = CurTraceStatus.indivStr.gradeNo;
						trace_meatgrade_getGradeText(CurTraceStatus.gradeNo, CurTraceStatus.curGradeText);
					}
    #endif	
  #else // USE_TRACE_MEATGRADE
					strcpy((char *)CurTraceStatus.curGradeText, (char *)CurTraceStatus.indivStr.grade);
  #endif // USE_TRACE_MEATGRADE
					CurTraceStatus.indivWeight = indivWeight;
					
					if (GlbOper.TraceabilityCallType == 0)
					{
						if (!old_index) old_index = indivStr.index; 
						else
						{
							if (old_index != indivStr.index) 
							{
								old_index = indivStr.index;
								CurTraceStatus.slPlace = 0;
							}							 
						}
					}
					sale_display_indivFlag_set(INDIV_DISP_MODE_INDIV_NO, ON);
					if (GlbOper.UseSendTraceInformation) trace_information_send(&(CommBufEthData[0]), NULL);
				}
#else // USE_CHECKING_HARMFUL_MEAT
				memcpy(&CurTraceStatus.indivStr, &indivStr, strSize);
				//#ifdef USE_KOR_TRACE_999
				//	if (!CurTraceStatus.indivStr.lotFlag)
				//	{
				//		strcpy((char *)CurTraceStatus.curGradeText, (char *)CurTraceStatus.indivStr.grade);
				//	}
				//#else
  #ifdef USE_TRACE_MEATGRADE
    #ifdef USE_DISCLOSURE_MEATGRADE
				CurTraceStatus.gradeNo = CurTraceStatus.indivStr.gradeNo;
				trace_meatgrade_getGradeText(CurTraceStatus.gradeNo, CurTraceStatus.curGradeText);
    #else
				if (plu_check_inhibit_ptype(PTYPE_GROUP_NO))	// PLU 등급을 사용하지 않을 때만 등급 호출 가능
				{
					CurTraceStatus.gradeNo = CurTraceStatus.indivStr.gradeNo;
					trace_meatgrade_getGradeText(CurTraceStatus.gradeNo, CurTraceStatus.curGradeText);
				}
    #endif	
  #else // USE_TRACE_MEATGRADE
				strcpy((char *)CurTraceStatus.curGradeText, (char *)CurTraceStatus.indivStr.grade);
  #endif // USE_TRACE_MEATGRADE
				//#endif
				CurTraceStatus.indivWeight = indivWeight;
				
				if (GlbOper.TraceabilityCallType == 0)
				{
					if (!old_index) old_index = indivStr.index; 
					else
					{
						if (old_index != indivStr.index) 
						{
							old_index = indivStr.index;
							CurTraceStatus.slPlace = 0;
						}							 
					}
				}
				sale_display_indivFlag_set(INDIV_DISP_MODE_INDIV_NO, ON);
				if (GlbOper.UseSendTraceInformation) trace_information_send(&(CommBufEthData[0]), NULL);
#endif // USE_CHECKING_HARMFUL_MEAT
			}
			else
			{
				sale_display_indivFlag_set(INDIV_DISP_MODE_INDIV_NO, OFF);
				if(beep) BuzOn(2);
				sale_display_update(UPDATE_TITLE);	// CJK070705
				return OFF;
			}
		}		
	}
	else
	{ 
		sale_display_indivFlag_set(INDIV_DISP_MODE_INDIV_NO, OFF);
	}
	sale_display_update(UPDATE_TITLE);	// CJK070705
	return ON;
}

extern char IndivMsg_NonTrace[];
extern char IndivMsg_SameTraceNo[];
INT8U individual_call(INT16U indiv_id)
{
	INT8U string_buf[25];
	INT8U ret;
#ifdef USE_TOPMART_STRUCTURE //sewon 161129
	if (!plu_check_inhibit_ptype(PTYPE_TR_COMP_FLAG))
#else
	if (!plu_check_inhibit_ptype(PTYPE_TRACE_FLAG))		// M1144 Set 여부
#endif
	{
		if (status_scale.cur_pluid)						// PLU가 호출이 되어 있는 상태로 개체호출 기능을 사용하면
		{
#ifdef USE_TRACE_FLAG_0_CHECK
			if (!status_scale.Plu.trace_flag)			// Tr.Flag가 0 즉, 개체이력 대상이 아닐때
			{
				BuzOn(2);		
				//sprintf((char*)string_buf, "개체이력대상이 아닙니다");
				sprintf((char*)string_buf, IndivMsg_NonTrace);
				display_message_page_mid((char *)string_buf);
				return OFF;
			}
#endif
		}
		else 											// PLU가 호출이 안된 상태로 개체호출 기능을 사용하면
		{
			BuzOn(2);
			return OFF;
		}
	}
	
	if(status_scale.cur_pluid)
	{
		//		if( !checkGradeEquality( individual_index ) )
		//		{
		//			BuzOn(3);
		//			return;
		//		}
	}
	else
	{
		IndividualDisplayTime = SysTimer100ms;
		IndividualDisplayTimeFlag = ON;
	}
	
	//BuzOn(1);
	
	//Clear Individual Display area
	//sale_display_indivFlag_set(INDIV_DISP_MODE_INDIV_NO, OFF);
	//sale_display_individual();
	//sale_display_update(UPDATE_TITLE);	// CJK070705
	ret = keyapp_call_individual(indiv_id, ON);
	if(!ret) return OFF;
	
	return ON;
}
#ifdef USE_NHMART_SAFE_MEAT
TRACE_INDIVIDUAL_IDENTITY indivInfo;	//저장되기 전 임시 개체 정보.
extern PLU_ADDR plu_table_addr[];
extern INT8U command_request_indiv_info(HUGEDATA COMM_BUF *CBuf);
void keyapp_call_indivInfo(void)
{
	HUGEDATA COMM_BUF *CBufIndiv;
	INT8U checkedIndiv = 0xff;
	INT16U idx;
	INT8U string_buf[20];
	if(!status_scale.cur_pluid) {
		BuzOn(2);
		return;	//PLU CALL 후에 입력되어야 함. 
	}
	if(checkMtrace)
	{
		CBufIndiv = &CommBufEthData[0];
		checkedIndiv = command_request_indiv_info(CBufIndiv);	//개체 정보 요청. 
		if(checkedIndiv)
		{
			BuzOn(2);
			switch(checkedIndiv)
			{
				case 1:
					sprintf(string_buf, "POS동작 오류");
					break;
				case 2:
					sprintf(string_buf, "이력번호 오류");
					break;
				case 3:
					sprintf(string_buf, "상품번호 오류");
					break;
				case 4:
					sprintf(string_buf, "조회개체정보 없음");
					break;
				case 5:
					sprintf(string_buf, "개체관리상품이 아님");
					plu_table_search(status_scale.departmentid, status_scale.cur_pluid,&idx, 1);
					TNT_DelTraceNoTable(0, 0, plu_table_addr[idx].addr);
					sale_display_indivFlag_set(INDIV_DISP_MODE_ALL, OFF);
					sale_display_update(UPDATE_TITLE);
					break;
				case 6:
					sprintf(string_buf, "유통기한 오류");
					break;
				case 7:
					sprintf(string_buf, "등급 오류");
					break;
				case 8:
					sprintf(string_buf, "개체중량 오류");
					break;
				case 9:
					sprintf(string_buf, "DB연결 오류");
					break;
				case 10:
					sprintf(string_buf, "프로토콜 오류");
					break;
				default:
					sprintf(string_buf, "통신 오류");
					break;				
			}
			display_message_page_mid_delay((char *)string_buf, 20);
			return;
		}
		else	//개체정보 정상으로 받은 경우 CurTrace에 저장.		
		{
			memcpy(&CurTraceStatus.indivStr, &indivInfo, sizeof(CurTraceStatus.indivStr));
  #ifdef USE_TRACE_MEATGRADE
    #ifdef USE_DISCLOSURE_MEATGRADE
						CurTraceStatus.gradeNo = CurTraceStatus.indivStr.gradeNo;
						trace_meatgrade_getGradeText(CurTraceStatus.gradeNo, CurTraceStatus.curGradeText);
    #else
						if (plu_check_inhibit_ptype(PTYPE_GROUP_NO))	// PLU 등급을 사용하지 않을 때만 등급 호출 가능
						{
							CurTraceStatus.gradeNo = CurTraceStatus.indivStr.gradeNo;
							trace_meatgrade_getGradeText(CurTraceStatus.gradeNo, CurTraceStatus.curGradeText);
						}
    #endif			
  #else // USE_TRACE_MEATGRADE
						strcpy((char *)CurTraceStatus.curGradeText, (char *)CurTraceStatus.indivStr.grade);
  #endif // USE_TRACE_MEATGRADE
			sale_display_indivFlag_set(INDIV_DISP_MODE_INDIV_NO, ON);
			sale_display_update(UPDATE_TITLE);
		}
	}
	else // USE_NHMART_SAFE_MEAT
	{
		sprintf(string_buf, "오프라인 모드입니다");
		display_message_page_mid_delay((char *)string_buf, 20);
		BuzOn(2);
	}
}
void keyapp_switch_offline(void)
{
	INT8U string_buf[20];
	INT8U temp;
	BuzOn(1);
	if(checkMtrace)	//key 를 누를 때 토글
	{
		checkMtrace = OFF;
		sprintf((char *)string_buf, "오프라인 모드");
	}
	else
	{
		checkMtrace = ON;
		sprintf((char *)string_buf, "온라인 모드");
	}
	display_message_page_mid_delay(string_buf, 20);
	
	set_bit_on_byte(&temp, checkMtrace, 7);
	set_global_bparam(GLOBAL_PRINT_OPER_FLAG2, temp);
	sale_display_update(UPDATE_MODE);
}
#endif

#ifdef USE_TOPMART_ASK_INDIV_INFO
INT8U keyapp_ask_indivInfo(HUGEDATA char *indiv_no) //sewon 170209
{
	HUGEDATA COMM_BUF *CBufIndiv;
	INT8U checkedIndiv = 0xff;
	INT16U idx;
	INT8U string_buf[30];
	if(!status_scale.cur_pluid) 
	{
		BuzOn(2);
		return;	//PLU CALL 후에 입력되어야 함. 
	}
	
		sprintf((char *)string_buf, "이력정보 요청중입니다.");
		display_message_page_mid_delay(string_buf, 5);	
	
		CBufIndiv = &CommBufEthData[0];
		checkedIndiv = command_ask_indiv_info(CBufIndiv,indiv_no);	//개체 정보 요청. 
		if(checkedIndiv)
		{
			BuzOn(2);
			switch(checkedIndiv)
			{
				case 1:
					sprintf(string_buf, "서버에 개체정보 없음");
					break;
	
				default: //0xff
					sprintf(string_buf, "통신 오류");
					break;				
			}
			display_message_page_mid_delay((char *)string_buf, 20);
			sprintf((char *)string_buf, "이력정보 수신실패!!");
			display_message_page_mid_delay(string_buf, 5);
	
			return 0;
		}
		else
		{
			sprintf((char *)string_buf, "이력정보 수신완료");
			display_message_page_mid_delay(string_buf, 5);	
			return 1;
		}
		
	
}
#endif

void keyapp_direct_individual(INT16U skeyIndivID)
{
	INT16U keyIndivID;
	INT32U key_addr;
	INT8U  individual_index;
	INT8U  ret = 0;
	//#ifdef USE_NHMART_SAFE_MEAT
	//	if(checkMtrace)	{
	//		BuzOn(2);		
	//		return;	//온라인 모드시 사용 불가. 
	//	}
	//#endif
	keyIndivID = skeyIndivID - KS_INDIVIDUAL_01;
	
	//sprintf(temp,"[keyIndivID=%d],[skeyIndivID=%d],[KS_INDIV=%d]\r\n",keyIndivID,skeyIndivID,KS_INDIVIDUAL_01);
	//MsgOut((HUGEDATA char *)temp);
	
	//	if ((get_global_bparam(GLOBAL_SCANNER_FLAG1)&0x07) != 3)
	//	{
	if (keyIndivID > 16) {
		BuzOn(2);		
		return;
	}
	//	}
	
	key_addr  = FLASH_TRACE_STANDALONE_KEY_AREA + DFLASH_BASE;
	key_addr += keyIndivID;
	individual_index  = Flash_bread(key_addr);
	BuzOn(1);
	individual_call(individual_index);
#ifdef USE_TOPMART_STRUCTURE //sewon 170105
	if (FlagIndividualMultiStart)//복수개체 입력
	{
		if (MultiIndex < MAX_INDIVIAL_MULTI_DATA_CNT)
		{
			memcpy(IndivdualMultiData[MultiIndex].indivNoStr, CurTraceStatus.indivStr.individualNO, INDIVIDUAL_NO_LEN);
			IndivdualMultiData[MultiIndex].slaughterHouse = CurTraceStatus.indivStr.slaughtNo;

			MultiIndex++;
		}
		else
		{
			BuzOn(2);
		}
	}
	else
	{
		FlagIndividualMultiRemain = OFF;
		MultiIndex = 0;
		memset(IndivdualMultiData, 0, sizeof(TRACE_INDIVIDUAL_MULTI_DATA) * MAX_INDIVIAL_MULTI_DATA_CNT);
	}
#endif
	/*
	 if(status_scale.cur_pluid)
	 {
	 //		if( !checkGradeEquality( individual_index ) )
	 //		{
	 //			BuzOn(3);
	 //			return;
	 //		}
	 }
	 else
	 {
	 IndividualDisplayTime = SysTimer100ms;
	 IndividualDisplayTimeFlag = ON;
	 }
	 
	 BuzOn(1);
	 
	 
	 //sprintf(temp,"[keyIndivID=%d],[keyIndex=%d],[indiv_index=%d]\r\n",keyIndivID,keyIndex,individual_index);
	 //MsgOut((HUGEDATA char *)temp);
	 
	 //Clear Individual Display area
	 sale_display_indivFlag_set(OFF);
	 //sale_display_individual();
	 sale_display_update(UPDATE_TITLE);	// CJK070705
	 
	 if (skeyIndivID == 99)	//Modified by JJANG 20081212 바코드로 개체번호 스캔할 때 무조건 99번으로 할당
	 {
	 keyapp_call_individual(skeyIndivID, ON);
	 }
	 else keyapp_call_individual(individual_index, ON);
	 */
	return;
}
/*
 void keyapp_direct_individual(INT16U skeyIndivID)
 {
 //	char temp[50];
 INT16U keyIndivID;
 INT32U key_addr;
 INT16U keyIndex;
 //CJK	INT16U deptNo;
 //CJK	INT32U pluNo;
 INT8U  individual_index;
 INT8U  ret = 0;
 
 // 개체키를 누르면, 
 // PLU 와 개체 인덱스와의 일대일 mapping
 
 keyIndivID = skeyIndivID - KS_INDIVIDUAL_01;
 if (keyIndivID > 16) {
 BuzOn(2);
 return;
 }
 BuzOn(1);
 
 if(!status_scale.cur_pluid){
 IndividualDisplayTime = SysTimer100ms;
 IndividualDisplayTimeFlag = ON;
 }
 
 //CJK	deptNo = status_scale.departmentid;
 //CJK	pluNo = status_scale.cur_pluid;
 
 key_addr  = FLASH_TRACE_STANDALONE_KEY_AREA + DFLASH_BASE;
 key_addr += keyIndivID;
 individual_index  = Flash_bread(key_addr);
 
 //CJK	ret = plu_table_search(deptNo,pluNo,&keyIndex,0);
 //CJK	if( ret )
 //CJK	{
 //CJK		//plu_table_individual[keyIndex] = individual_index;
 //CJK		SetIndividualTable(keyIndex-1, individual_index);
 //CJK	}
 
 //sprintf(temp,"[keyIndivID=%d],[keyIndex=%d],[indiv_index=%d]\r\n",keyIndivID,keyIndex,individual_index);
 //MsgOut((HUGEDATA char *)temp);
 
 //Clear Individual Display area
 sale_display_indivFlag_set(OFF);
 sale_display_individual();
 
 keyapp_call_individual(individual_index, ON);
 
 return;
 }
 */

/*
 void keyapp_get_individual(void)
 {
 //INT16U keyIndivID;
 //INT32U key_addr;
 INT16U keyIndex;
 INT16U deptNo;
 INT32U pluNo;
 INT8U  individual_index;
 INT8U  ret = 0;
 
 deptNo = status_scale.departmentid;
 pluNo = status_scale.cur_pluid;
 ret = plu_table_search(deptNo,pluNo,&keyIndex,0);
 if( ret )
 {
 if( (keyIndex > 0) && (keyIndex < 10000)) //MAX_PLU_ADDRESS=10000
 {
 //individual_index = plu_table_individual[keyIndex];
 individual_index = GetIndividualTable(keyIndex-1);
 }
 }						  	
 
 keyapp_call_individual(individual_index, OFF);
 
 return;
 }
 */

void keyapp_check_trace(void)
{
	INT8U buf[10];
	INT8U tempparam;
	
	if (GlbOper.TraceabilityCallType == 1)
	{
		BuzOn(1);
		
		//개체이력 저장기능
		// 0 = 일반개체: 개체정보 저장하지 않음
		// 1  = 상품개체: 상품별로 개체정보 저장
		// 2 = 전체개체: 최종판매 개체정보 저장
		SaveTraceabilityNo++;
		SaveTraceabilityNo %= 3;	
		
		switch(SaveTraceabilityNo)
		{
			case 0: 
				sprintf((char *)buf, "일반개체");
				display_message_page_mid((char *)buf);
				break;
			case 1: 
				sprintf((char *)buf, "상품개체"); 
				display_message_page_mid((char *)buf);
				break;
			case 2:
				sprintf((char *)buf, "전체개체"); 
				display_message_page_mid((char *)buf);
				break;
			default : break;
		}
		tempparam = get_global_bparam(GLOBAL_SALE_SETUP_FLAG15);
		tempparam = (tempparam | (SaveTraceabilityNo & 0x03));
		set_global_bparam(GLOBAL_SALE_SETUP_FLAG15, tempparam);
	}
	else
	{
		BuzOn(2);
	}
}

#endif


INT8U check_unitweightchange(INT16S uid) // Added by CJK 20060403
{
	//INT32U ret;
	INT32U NewUnitWeight;
	INT32U tempUnitPrice;
	
	if(Operation.useMultipleUnits == 2) {	// 1회 conversion 가능
		return OFF;
	}
	//ret=uid;
	if (ADM_GetData.CurUnit == WEIGHT_UNIT_LB) {
		if(!GlbOper.EnableMultiLbKey) return OFF;
		if(uid > 3) return OFF;
	} else {
		if(!GlbOper.EnableMultiKgKey) return OFF;
#ifdef USE_WEIGHT_UNIT_500G
		if(uid > 3) return OFF; //500g 단가 추가
#else
		if(uid > 2) return OFF;
#endif
	}
	NewUnitWeight = get_unitweight(uid, ADM_GetData.CurUnit);
	
	if(PluType() == 1)//by weight
	{
		if(status_scale.cur_weightunit == NewUnitWeight) return OFF;
		if (ADM_GetData.CurUnit == WEIGHT_UNIT_LB)
		{
			tempUnitPrice = (status_scale.cur_unitprice * status_scale.cur_weightunit)/NewUnitWeight;
			
			if(tempUnitPrice >= Operation.MaxUnitPrice) return OFF;
			status_scale.cur_unitprice = tempUnitPrice;
			Operation.useMultipleUnits = 2;	// 1회 Conversion 가능
			status_scale.cur_weightunit = NewUnitWeight;
			return ON;
		}
		else	// CJK080214
		{
#ifdef USE_UNIT_500G_PLU_500G_BASE
			if(status_scale.cur_weightunit == 500 && NewUnitWeight == 1000)
			{
				tempUnitPrice = (status_scale.cur_unitprice * NewUnitWeight)/status_scale.cur_weightunit;
			}
			else
			{
				tempUnitPrice = status_scale.Plu.unitprice;
			}
			if(tempUnitPrice >= Operation.MaxUnitPrice) return OFF;
			status_scale.cur_unitprice = tempUnitPrice;
#endif			
			Operation.useMultipleUnits = 0;	// 계속 Conversion 가능
			status_scale.cur_weightunit = NewUnitWeight;
			return ON;
		}
	}
	return OFF;
	/*
	 if(PluType() == 1)//by weight
	 {
	 if(status_scale.cur_weightunit == NewUnitWeight) return OFF;
	 tempUnitPrice = (status_scale.cur_unitprice * status_scale.cur_weightunit)/NewUnitWeight;
	 if(tempUnitPrice >= Operation.MaxUnitPrice) {
	 return OFF;
	 }
	 Operation.useMultipleUnits = 2;	// 1회 Conversion
	 status_scale.cur_unitprice = tempUnitPrice;
	 status_scale.cur_weightunit = NewUnitWeight;
	 return ON;
	 }
	 */
}

#ifdef USE_CANADA_100GToggle		
	INT16S keyapp_toggle_checkunit(INT16S uid) // for 100g/kg toggle
	{
		if (ADM_GetData.CurUnit != WEIGHT_UNIT_LB)
		{
			if(status_scale.cur_weightunit == 1000) return 2;
			if(status_scale.cur_weightunit == 100) return 1;		
		}
		return uid;
	}
#endif
void keyapp_unitchange(INT16S uid) // Modified by CJK 20060403
{
	CAPTION_STRUCT cap;
	char str[40];
	INT8U ret;
	INT16U capcode;
	
	if(status_scale.cur_pluid) {
#ifdef USE_CANADA_100GToggle		
			uid = keyapp_toggle_checkunit(uid);
#endif
		ret = check_unitweightchange(uid);
	} else {
		ret = OFF;
	}
	if(!ret) {
		BuzOn(2);
		caption_split_by_code(0xc814,&cap,0);//"Check UnitWeight"
		strcpy(str, cap.form);
	} else {
		caption_split_by_code(0xc815,&cap,0);//"CNV"
		sprintf(str, "%s", cap.form);
		capcode = 0xc731;
		capcode += (uid-1);
		if(ADM_GetData.CurUnit == WEIGHT_UNIT_LB) capcode += 2;	// lb
		caption_split_by_code(capcode, &cap,0);
		strcat(str, cap.form);
	}
	DisplayMsg(str);
	sale_display_update(UPDATE_STATUS|UPDATE_MODE);
}

void keyapp_toggle_weightunit(void)
{
	INT8U unit;
	
	if(!GlbOper.EnableKgLb || status_scale.cur_pluid || status_scale.Tare ) {
		BuzOn(2);
		DisplayMsg(global_message[MSG_NOT_ALLOW_KGLB]);//"kg<->lb Not Allowed"
		return;
	}
	
	unit = ad_get_kglb();
	if(unit == WEIGHT_UNIT_LB) {
		unit = WEIGHT_UNIT_KG;
		if(get_base_bparam(FLASH_ADC_WUNIT) == WEIGHT_UNIT_G) unit = WEIGHT_UNIT_G;
	} else {
		unit = WEIGHT_UNIT_LB;
	}
	adm_set_unit_cur(unit);
	LoadWeighingInfo();
	sale_display_update(UPDATE_STATUS|UPDATE_MODE);
}

void keyapp_datetime(void)
{
	//BuzOn(1);
	display_date_change(OFF,OFF);
}

void	LoadWeighingInfo(void)
{
	CAPTION_STRUCT cap;
	long interval;
	interval = (long)ad_get_interval(0);
	interval = ad_cnv_adm2main(interval);
	
	//status_scale.auto_maxcount  = get_global_bparam(GLOBAL_SALE_AUTO_COUNT);
	status_scale.weight_min     = get_global_lparam(GLOBAL_SALE_MINIMUMWEIGHT);
	status_scale.weight_delta   = get_global_lparam(GLOBAL_SALE_AUTO_DELTAWEIGHT);
	status_scale.display_dummydigit  = OnOff(get_global_bparam(GLOBAL_SALE_SETUP_FLAG5)&0x40);
	
	caption_split_by_code(0x4861,&cap,0);
	if (status_scale.weight_delta<cap.input_min ) status_scale.weight_delta=cap.input_min;
	if (status_scale.weight_delta>cap.input_max ) status_scale.weight_delta=cap.input_min;
	if (GlbOper.PrintUnderMinWeightOnPrepack || GlbOper.PrintUnderMinWeightOnAUTO)     status_scale.weight_delta=10;	// Parameter 690 [Y]일 때 최소무게 10d로 사용
	
	caption_split_by_code(0x4862,&cap,0);
	if (status_scale.weight_min  <cap.input_min ) status_scale.weight_min  =cap.input_min;
	if (status_scale.weight_min  >cap.input_max ) status_scale.weight_min  =cap.input_min;
	
	status_scale.weight_min     *= interval;
	status_scale.weight_delta   *= interval;
	
	status_scale.auto_count     = 0;
	status_scale.weight_stable  = 0;
}

INT16S	CheckCanadianTareAction(void)
{
	if(TareOper.canadianTare && status_scale.Tare && status_scale.cur_pluid && PluType()==PLU_BY_WEIGHT)
		return ON;
	
	return OFF;
}

INT16S CheckPrepackAutoLogin(void)
{
	//new
	if(Operation.operationMode == MODE_SELF)	return ON;
	//	if(Operation.operationClerk != OPER_NORMAL && !status_scale.clerkid)
	if(PrtStruct.Mode == RECEIPT)//new
	{
		caption_split_by_code(0xC80C,&cap,0);//"Select label mode"
		DisplayMsg(cap.form);
		return OFF;
	}
	if(Operation.operationClerk != OPER_NORMAL && !status_scale.clerkid)
	{
		//		if(PrtStruct.Mode == RECEIPT)//new
		//		{
		//			caption_split_by_code(0xC80C,&cap,0);//"Select label mode"
		//			DisplayMsg(cap.form);
		//			return OFF;
		//		}
		caption_split_by_code(0xC80D,&cap,0);
		DisplayMsg(cap.form);//"Log in clerk"
		Key_DelaySec(5,0);
		sale_pluclear(OFF);
		sale_display_update(0x0fff);//061128
		return OFF;
	}
	return ON;
}

void keyapp_auto(void)
{
#ifdef USE_PROHIBIT_AUTO_PRT
	return;		/* 스리랑카 요청(인증) - auto 기능 block */
#endif // USE_PROHIBIT_AUTO_PRT
	if (Operation.wAuto && CheckCanadianTareAction()) {
		MsgCanadianTare();
		return;
	}
	if(!CheckPrepackAutoLogin())	return;
	Operation.wPrepack = 0;//OFF;
	if(!Operation.wAuto)
	{
		Operation.wAuto = ON;
		LoadWeighingInfo();
	}
	else
	{
		Operation.wAuto = OFF;
		if(!Operation.wSave && status_scale.cur_pluid)
			Operation.salePluClearWait = ON;
		
	}
	sale_display_update(UPDATE_STATUS|UPDATE_MODE);
}

extern ROMDATA INT16U normal_self_key_numeric_code_set_un[];
extern ROMDATA INT16U normal_self_key_code_set_un[];
void keyapp_self(INT16S Flag)	//Made by JJANG 20070801(Use Numeric key)
{
	INT16S i;
	INT16U normal_key_code_set[128];
	INT16U normal_key_code_set_shifted[128];
	
	if(Flag == ON)
	{
		for (i = 0; i < KEY_MAX_TABLE_SIZE / 2; i++) 
		{
			normal_key_code_set[i] = normal_self_key_numeric_code_set_un[i];
			normal_key_code_set_shifted[i] = normal_self_key_numeric_code_set_un[i];
		}
	}
	else 
	{
		for (i = 0; i < KEY_MAX_TABLE_SIZE / 2; i++) 
		{
			normal_key_code_set[i] = normal_self_key_code_set_un[i];
			normal_key_code_set_shifted[i] = normal_self_key_code_set_un[i];
		}
		
	}
	Flash_swrite(DFLASH_BASE+FLASH_KEY_AREA           , (HUGEDATA INT8U *)normal_key_code_set        , KEY_MAX_TABLE_SIZE);
	Flash_swrite(DFLASH_BASE+FLASH_KEY_SHIFT_AREA     , (HUGEDATA INT8U *)normal_key_code_set_shifted, KEY_MAX_TABLE_SIZE);
	
	//sale_display_update(UPDATE_STATUS|UPDATE_MODE);
}

void keyapp_save(void)
{
	//BuzOn(1);
	
	if(Operation.wSave && CheckCanadianTareAction())
	{
		MsgCanadianTare();
		return;
	}
	
	Operation.wPrepack = OFF;
	if(!Operation.wSave)
	{
		Operation.wSave=ON;
	}
	else
	{
		Operation.wSave=OFF;
		if(status_scale.cur_pluid)
			Operation.salePluClearWait = ON;
	}
	//	Operation.woldMode = Operation.wSave;
#ifdef USE_CL5200_DISPLAY_DEFAULT
	sale_display_update(UPDATE_STATUS|UPDATE_TARE|UPDATE_MODE);
#else
	sale_display_update(UPDATE_STATUS|UPDATE_MODE);
#endif
}

void prepack_onoff(INT8U mode)	// mode=0:Toggle, mode=1:ON, mode=2:Off
{
	if (mode) goto PP_START;
	if(Operation.wPrepack && CheckCanadianTareAction())
	{
		MsgCanadianTare();
		return;
	}
	PP_START:
				Operation.wAuto = OFF;
	if (mode==2) goto PP_OFF;
	if (mode==1) goto PP_ON;
	if(!Operation.wPrepack)
	{
		PP_ON:		if(!CheckPrepackAutoLogin())	return;
		Operation.wPrepack=ON;
		//Operation.wSave = ON;
		LoadWeighingInfo();
	}
	else
	{
		PP_OFF:		Operation.wPrepack=OFF;
		//		if(status_scale.cur_pluid)  //SG061129
		//			Operation.salePluClearWait = ON;
		//		if (Operation.woldMode==ON) {
		//			keyapp_save();
		//		}
		if(Operation.operationClerk!=OPER_STD_CLERK && Operation.addStandatdClerk)	// add 를 동작하지 않고 total sum에 대한 계산 flag를 설정한다
		{
			Operation.addStandatdClerk = OFF;
			//if(Operation.addStandatdClerk) {
			//	caption_split_by_code(0xc040,&cap,0);
			//} else {
			caption_split_by_code(0xc041,&cap,0);
			//}
			DisplayMsg(cap.form);
			return;
		}
	}
}

void keyapp_prepack(void)
{
	//	INT32U pnt;
	//	INT16U err;
	
#ifdef USE_PROHIBIT_PREPACK
	return;		/* 스리랑카 요청(인증) - prepack 기능 block */
#endif // USE_PROHIBIT_PREPACK
	//BuzOn(1);
	prepack_onoff(0);
	//sale_prepack_check();
	sale_display_update(UPDATE_STATUS|UPDATE_MODE);
}

void keyapp_shift(void)
{
	//BuzOn(1);
	KEY_Set_Shift(3);// 3 = auto change
}

extern DISC_SALE_TYPE		DiscSale;
void ClearPluAux(void)
{
	status_scale.Plu.dc_id = 0;
	status_scale.Plu.fixed_weight = 0;
#ifndef USE_PLU_MULTIPLE_PRICE	//Refefence PLU 기능 사용 안 함
	status_scale.Plu.refer_dept = 0;
	status_scale.Plu.refer_plu = 0;
	status_scale.linked_count = 0;
	status_scale.linked_plu[0] = 0;
	status_scale.linked_plu[1] = 0;
#endif
#ifdef USE_TOPMART_STRUCTURE //sewon 161128
	status_scale.Plu.tr_sellby_date = 0;
#else
	status_scale.Plu.special_price = 0;
#endif
	status_scale.Plu.member_price = 0;
	status_scale.Plu.packing_price = 0;
	status_scale.Plu.tax_id = 0;
	DiscSale.saleIndexOk = 0;
	DiscSale.saleDiscountType = DISCOUNT_NO;
	DiscSale.saleDiscFSP = 0;
	status_scale.Plu.tare_limit = 0;
	status_scale.Plu.tare_percent = 0;
	Operation.fixedPrice = 0;
	Operation.fixedAction=0;
}

void keyapp_for(void)
{
	INT32U  keyBackup;
	
	InputQuickPluState = OFF;	// Count PLU일 때, [PLU_NUM] + [FOR] + Qty 허용
	status_scale.cur_quickplu = 0;
	
	// qty, pcs, unitprice 편집시 blink. FOR 키를 누를 때 마다 값이 표시되도록 함
	Set7BlinkPrevTime = SysTimer100ms;
	Seg7BlinkOn = 0;
	
	if(Startup.country == COUNTRY_US)	//JJANG 20090316 임시수정, 미국일 때 For key로 타입변경 가능 By Weight --> By Count
	{
		if (PluType() == PLU_BY_WEIGHT)
		{
			ChangePluType(PLU_BY_PCS);
			keyapp_override();
			status_scale.cur_ride = 0;
		}
	}
	
	if (GlbPrint.directNonfood && !status_scale.cur_pluid && 0 < status_scale.cur_keyvalue )
	{
		keyBackup = status_scale.cur_keyvalue;
		keyapp_misc_bycount();
		status_scale.cur_ride = 1;
		status_scale.cur_qty = 1;
		status_scale.cur_unitprice = keyBackup;     
		return;
	}
	if (!status_scale.ride_max) {BuzOn(2); return;}	// Weight PLU & Not Override
	if (!Operation.override) {BuzOn(2); return;}
	status_scale.cur_ride++;
	if (status_scale.ride_max > 3) status_scale.ride_max = 1;
	status_scale.cur_ride %= (status_scale.ride_max+1);
	if (!status_scale.cur_ride) status_scale.cur_ride = 1;
	sale_display_update(UPDATE_TITLE|UPDATE_RIDE);
}

void keyapp_non_function(void)
{
	BuzOn(2);
	sale_display_update_error(0xE00A,0,0);
}

#ifdef USE_TAX_RATE
extern INT16S GetTaxData(PRICE_TYPE *pPrice,INT8U taxNumber,INT8U misc_flag);
#endif
void keyapp_misc_byweight(INT16S beep)
{
	if(beep)
	{
		BuzOn(1);
	}
	
	Operation.transactionOK=OFF;
	sale_pluclear(ON);
	sale_display_update(0x0fff);//061128
	status_scale.cur_pluid=MISC_PLU_WEIGHT;
	status_scale.flag_input&=0xf1;
	status_scale.flag_input|=0x02;
	status_scale.ride_max     = 1;
	status_scale.cur_ride = 1;
	InputQuickPluState = OFF;
	
#ifdef USE_TAX_RATE    
	GetTaxData(&Price,0,1);		//Added by JJANG 20070614 MISC tax 사용
#endif
	
	ClearPluAux();
	Operation.override = ON;
	display_tare(1);
	sale_display_update(UPDATE_WEIGHT|UPDATE_UNIT|UPDATE_PRICE|UPDATE_RIDE);
}

void keyapp_misc_bycount(void)
{
	INT16S	n;
	INT32U	maxQty,cur_keyvalue;
	
	n=(INT16S)get_global_bparam(GLOBAL_INPUT_BN_QTYLENGTH);
	maxQty=power10(n);
	
	//BuzOn(1);
	Operation.transactionOK=OFF;
	cur_keyvalue = status_scale.cur_keyvalue;
	sale_pluclear(ON);
	sale_display_update(0x0fff);//061128
	status_scale.cur_pluid=MISC_PLU_COUNT;
	status_scale.flag_input  &= 0xf1;
	status_scale.flag_input  |= 0x04;
	status_scale.ride_max     = 2;
	status_scale.cur_ride = 1;
	status_scale.cur_qty = 1;
	if(cur_keyvalue && cur_keyvalue < maxQty)
	{
		status_scale.cur_ride =2;
		status_scale.cur_qty = cur_keyvalue;
	}
	{
		INT8U	byte;
		byte=get_global_bparam(GLOBAL_SALE_SETUP_FLAG3);// override location
		if(OnOff(byte&0x02))
		{
			status_scale.cur_ride =2;
		}
	}
	
	status_scale.cur_pcs = 1;
	InputQuickPluState = OFF;
	
#ifdef USE_TAX_RATE
	GetTaxData(&Price,0,1);		//Added by JJANG 20070614 MISC tax 사용
#endif
	ClearPluAux();
	
	Operation.override = ON;
	sale_display_update(UPDATE_WEIGHT|UPDATE_UNIT|UPDATE_PRICE|UPDATE_RIDE);
	sale_display_tare_eu();
}

void keyapp_misc_bypcs(INT16S beep)
{
	INT16S	n;
	//	INT32U	maxPcs,maxQty,cur_keyvalue;
	INT32U	maxQty,cur_keyvalue;    
	
	n=(INT16S)get_global_bparam(GLOBAL_INPUT_BC_QTYLENGTH);
	maxQty=power10(n);
	n=(INT16S)get_global_bparam(GLOBAL_INPUT_BC_PCSLENGTH);
	//	maxPcs=power10(n);
	
	cur_keyvalue = status_scale.cur_keyvalue;
	if(beep)
	{
		BuzOn(1);
	}
	Operation.transactionOK=OFF;
	sale_pluclear(ON);
	sale_display_update(0x0fff);//061128
	status_scale.cur_pluid=MISC_PLU_PCS;
	status_scale.flag_input  &= 0xf1;
	status_scale.flag_input  |= 0x06;  // 0000 0110
	status_scale.ride_max     = 3;
	status_scale.cur_ride = 1;
	status_scale.cur_qty = 1;
	status_scale.cur_pcs = 1;
	InputQuickPluState = OFF;
	if(cur_keyvalue && cur_keyvalue < maxQty)
	{
		//		status_scale.cur_ride =3;
		//		status_scale.cur_pcs = cur_keyvalue % maxPcs;
		//		if(!status_scale.cur_pcs || (maxQty-1)< status_scale.cur_pcs)	status_scale.cur_pcs =1;
		//		cur_keyvalue /= maxPcs;
		//		status_scale.cur_qty = cur_keyvalue % maxQty;
		status_scale.cur_ride =3;
		if(cur_keyvalue < maxQty)
			status_scale.cur_qty = cur_keyvalue;
	}
	{
		INT8U	byte;
		byte=get_global_bparam(GLOBAL_SALE_SETUP_FLAG3);// override location
		if(OnOff(byte&0x02))
		{
			status_scale.cur_ride =3;
		}
	}
	//	if(status_scale.cur_keyvalue && status_scale.cur_keyvalue <100)
	//		status_scale.cur_pcs = status_scale.cur_keyvalue;
#ifdef USE_TAX_RATE
	GetTaxData(&Price,0,1);		//Added by JJANG 20070614 MISC tax 사용
#endif	
	ClearPluAux();
	Operation.override = ON;
	sale_display_update(UPDATE_WEIGHT|UPDATE_UNIT|UPDATE_PRICE|UPDATE_RIDE);
	sale_display_tare_eu();
}

extern INT8U FlagTransactionWithoutPrint;
void keyapp_menu(void)
{
	//BuzOn(1);
	INT16U keyv;
	INT8U allowCalEnter;
	
	allowCalEnter = ON;
	if(Startup.country == COUNTRY_KR) allowCalEnter = OFF;	// 내수 버전만 강제로 Cal 진입 막
#ifdef COUNTRY_USA	// 미국 버전 강제로 Cal 진입 막음
	allowCalEnter = OFF;
#endif	
#ifndef USE_ALLOW_CAL_MODE
	allowCalEnter = OFF;
#endif 
	
	if(TareOper.canadianTare && !Operation.transactionOK && status_scale.Tare)
	{
		MsgCanadianTare();
		return;
	}
	keyv=hex2bcd_word((INT16U)status_scale.cur_keyvalue);
#ifdef USE_DBG_EMART_NETWORK
	if (keyv==0x9999)
	{
		FlagTransactionWithoutPrint ^= 1;
		if(FlagTransactionWithoutPrint){
			DisplayMsg("PRINT LABEL OFF");
		} else {
			DisplayMsg("PRINT LABEL ON");
		}
		return;
	}
#endif
	if (allowCalEnter && (keyv==0x8000 || (KeyDrv.PressedModeKey && KeyDrv.RawCode==0x7B && Operation.operationMode==MODE_SELF))) {
		char old_font;
		CAPTION_STRUCT cap;
		POINT 	disp_p;
		INT8U   old_key;
		INT8U	ret_string[20];
		
		old_font=DspStruct.Id1;
		DspLoadFont1(DSP_MENU_FONT_ID);
		Dsp_Fill_Buffer(0);
#if !defined(_USE_LCD20848_) && !defined(_USE_LCD32_)
		VFD7_Fill_Buffer(0);
#endif
		caption_split_by_code(0x1f00,&cap,0);
		caption_adjust(&cap,NULL);
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
		disp_p = point(Startup.menu_type*Startup.menu_ygap, 0);
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
		Dsp_Write_String(disp_p,(HUGEDATA char *)cap.form);
#endif
#else
		//		Dsp_Write_String(disp_p,(HUGEDATA char *)cap.form);
		display_clear(DISPLAY_UNITPRICE);
		display_string(DISPLAY_UNITPRICE, (INT8U *)cap.form);
#endif
		Dsp_Diffuse();
		old_key=KEY_GetMode();
		KEY_SetMode(1);
		memset((INT8U*)ret_string, 0x00, 15);
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
		keyin_string(KI_FREE_MODE, KI_GRAPHIC,0, (HUGEDATA INT8U*)ret_string, cap.input_length, 
			cap.input_dlength, Startup.menu_type*Startup.menu_ygap, cap.input_x*Startup.menu_xgap, 1, 1, KI_NOPASS);
#else
		keyin_string(KI_FREE_MODE, KI_DEFAULT,0, (HUGEDATA INT8U*)ret_string, cap.input_length, 
  				cap.input_dlength, Startup.menu_type*Startup.menu_ygap, cap.input_x*Startup.menu_xgap, 1, 1, KI_NOPASS);
#endif
		ret_string[8]=0;
		str_trimleft((char *)ret_string);
		KEY_SetMode(old_key);
		DspLoadFont1(old_font);
		if (strcmp((char*)ret_string,"83419")==0) {
			if(permission_check(PMS_DEALER,0)) {
#ifdef USE_CTS_FUNCTION
				if (CtsStruct.CtsEnable)
				{
					status_run.run_mode = RUN_WAITING_CAL;
				}
				else
				{
					status_run.run_mode = RUN_CALIBRATION;
				}
#else
				status_run.run_mode = RUN_CALIBRATION;
#endif
			} else {
				//goto PGM;
				display_lcd_clear();
				return;
			}
		} else {
			//goto PGM;
			display_lcd_clear();
			return;
		}
	} else {
		if(!permission_check(PMS_MENU,0)){
#ifdef USE_CL5200_DISPLAY_DEFAULT
			sale_display_update(UPDATE_ALL|UPDATE_CLEARALL);
#endif
			//display_lcd_clear();	
			return;
		}
		if (Operation.operationMode == MODE_SELF)
		{
			if (keyv == 0x88)
			{
				if(get_nvram_bparam(NVRAM_SELF_KEY_BLOCK_ON))
				{
					set_nvram_bparam(NVRAM_SELF_KEY_BLOCK_ON, 0);
					//					DisplayMsg("Key-Block OFF");
					DisplayMsg("Key-BLK OFF"); //change to ERROR msg
				}
				else
				{
					set_nvram_bparam(NVRAM_SELF_KEY_BLOCK_ON, 1);
					DisplayMsg("Key-BLK ON"); //change to ERROR msg
				}
				status_scale.cur_keyvalue = 0;
			}
			else
			{
				status_run.run_mode = RUN_PROGRAM;
			}
		}
		else
		{
			/*PGM:*/		status_run.run_mode = RUN_PROGRAM;
		}
	}
}

extern void print_rewind(void);
extern INT16U	KeyWait(INT16U key1,INT16U key2,INT16U key3,INT16U key4,INT16U key5);

void keyapp_feed(INT16S beepFlag)
{
	if(beepFlag) BuzOn(1);
	if(PrtFlashStruct.RewindEnable && Operation.rewindReady)
	{
		char	string_buf[20];
		
		Operation.rewindReady = OFF;
		sprintf(string_buf, "%cM", GS);
		//DisplayMsg(string_buf);
		PutDataRxBuf(&CommBufPrt,string_buf,2);
		string_buf[0] = ON;
		PutDataRxBuf(&CommBufPrt,string_buf,1);
		while(1)
		{
			if (KEY_Read())
			{
				BuzOn(1);
				//if(KeyDrv.Type==KEY_TYPE_FUNCTION && KeyDrv.CnvCode==KS_CLEAR)
				if(KeyDrv.CnvCode == KS_CLEAR)	// KS_CLEAR = KP_CLEAR = 0x0b
					break;
			}
			
			// 명령
			print_rewind();
			// motor가 동작일때 까지 기다림
			//			while(CheckRxBuf(&CommBufPrt)) {
			//				CASInterpreter(&CommBufPrt);
			//			}
			//			while(PrtDrvStruct.PrtState != PRT_STATE0I);
			PrintListWaitEndWithoutComm();
			// 용지 체크
			if(CheckPeelHead(ON, OFF))	
			{
				print_rewind();
				//				while(CheckRxBuf(&CommBufPrt)) {
				//					CASInterpreter(&CommBufPrt);
				//				}
				//				while(PrtDrvStruct.PrtState != PRT_STATE0I);
				PrintListWaitEndWithoutComm();
				break;	// 없으면
			}
		}
	}
	else
	{
		if (!CheckPeelHead(OFF, OFF))
		{
			return;
		}
		PutCharRxBuf(&CommBufPrt, 0x0c);
		//		while (CheckRxBuf(&CommBufPrt)) {
		//			CASInterpreter(&CommBufPrt);
		//		}
		//		while(PrtDrvStruct.PrtState != PRT_STATE0I);
		PrintListWaitEndWithoutComm();
	}
	set_nvram_bparam(NVRAM_PRT_OUTOFPAPER, PrtDrvStruct.OutOfPaperFlag);
#ifdef	USE_CHECK_OUTOFPAPER_ON_LABEL
	Prt_CheckOutOfPaper();
#endif
}
	
long	PrevPrintOriginPrice=0;

void CheckZeroTotalPrice(void)
{
	if(status_scale.Weight <= 0) {
		if (get_global_bparam(GLOBAL_SALE_SETUP_FLAG2)&0x20) {	// parameter 574
#ifdef USE_EMART_NETWORK
			if (status_scale.cur_unitprice && PluType() == PLU_BY_WEIGHT)	// 변경불가 플래그가 있으면
			{
				keyapp_type(ON);	// 무게상품이고, 무게가 없을 때, 자동으로 개수로 변환
			}
#else
			if(status_scale.cur_unitprice && !Operation.fixedPrice && !status_scale.Plu.fixed_weight) {
				keyapp_fixed_price(OFF,2);
			}
#endif
		}
	}
}
#ifdef USE_CHECK_INDIV_INFO
extern INT8U command_check_indiv_info(HUGEDATA COMM_BUF *CBuf);
extern PLU_ADDR plu_table_addr[];
INT8U CheckIndivInfo(void)
{
	HUGEDATA COMM_BUF *CBufIndiv;
	INT8U checkedIndiv = 0xff;
	char string_buf[20];
	INT8U ret = ON;
	INT16U idx;
	CBufIndiv = &CommBufEthData[0];
#ifdef USE_NHSAFE_PIG_TRACE 
	INT8U tempcheckMtrace;
	tempcheckMtrace = checkMtrace;
	if((CurTraceStatus.indivStr.individualNO[0] == '1') || ((CurTraceStatus.indivStr.individualNO[0] == 'L') && (CurTraceStatus.indivStr.individualNO[1] == '1')))
	{
		checkMtrace = 0; //20150421
	}
#endif
	
#ifdef USE_NHMART_SAFE_MEAT
	if(checkMtrace)
	{
		Operation.OfflineMode = OFF;
	}
	else
	{
		Operation.OfflineMode = ON;
	}
	if(checkMtrace)
	{
		checkedIndiv = command_check_indiv_info(CBufIndiv);
		if(checkedIndiv != 0)	//0이면 개체정보 정상.
		{
			BuzOn(3);
			switch(checkedIndiv)
			{
				case 1:
					sprintf(string_buf, "POS동작 오류");
					break;
				case 2:
					sprintf(string_buf, "이력번호 오류");
					break;
				case 3:
					sprintf(string_buf, "상품번호 오류");
					break;
				case 4:
					sprintf(string_buf, "조회개체정보 없음");
					break;
				case 5:
					sprintf(string_buf, "개체관리상품이 아님");
					plu_table_search(status_scale.departmentid, status_scale.cur_pluid,&idx, 1);
					TNT_DelTraceNoTable(0, 0, plu_table_addr[idx].addr);
					sale_display_indivFlag_set(INDIV_DISP_MODE_ALL, OFF);
					sale_display_update(UPDATE_TITLE);
					break;
				case 6:
					sprintf(string_buf, "유통기한 오류");
					break;
				case 7:
					sprintf(string_buf, "등급 오류");
					break;
				case 8:
					sprintf(string_buf, "개체중량 오류");
					break;
				case 9:
					sprintf(string_buf, "DB연결 오류");
					break;
				case 10:
					sprintf(string_buf, "프로토콜 오류");
					break;
				default:
					sprintf(string_buf, "통신 오류");
					break;
			}
			display_message_page_mid_delay(string_buf, 30);
			ret = OFF;
		}
		else
		{
			ret = ON;
		}
	}
#ifdef USE_NHSAFE_PIG_TRACE 
	if((CurTraceStatus.indivStr.individualNO[0] == '1') || ((CurTraceStatus.indivStr.individualNO[0] == 'L') && (CurTraceStatus.indivStr.individualNO[1] == '1')))
	{
		checkMtrace = tempcheckMtrace; //20150421
	}
#endif
#else
	if(checkMtrace && FlagDisplayIndividual && ((CurTraceStatus.indivStr.individualNO[0] == '0') ||(CurTraceStatus.indivStr.individualNO[0] == '1') || (CurTraceStatus.indivStr.individualNO[0] == 'L')))	//한우(0) 돼지(1) 묶음번호(L)만 유효성 확인. 
	{
		checkedIndiv = command_check_indiv_info(CBufIndiv);
		if(checkedIndiv != 0)	//0이면 개체정보 정상.
		{
			BuzOn(3);
			if(checkedIndiv > (INVALID_INDIV_NO | INVALID_SLAUGHTERHOUSE | INVALID_GRADE))
			{
				sprintf(string_buf, "통신오류");
				display_message_page_mid_delay(string_buf, 30);
				ret = ON;
			}
			else
			{
				string_buf[0] = 0;
				if(checkedIndiv & INVALID_INDIV_NO)
				{
					strcat(string_buf, "이력번호");
				}
				if(checkedIndiv & INVALID_SLAUGHTERHOUSE)
				{
					strcat(string_buf, " 도축장");
				}
				if(checkedIndiv & INVALID_GRADE)
				{
					strcat(string_buf, " 등급");
				}
				strcat(string_buf, " 이상");
				display_message_page_mid_delay(string_buf, 30);
				sale_display_update(UPDATE_ALL|UPDATE_CLEARALL);
				ret = OFF;
			}
		}
		else
		{
			ret = ON;
		}
	}
#endif	//#ifdef USE_NHMART_SAFE_MEAT
	return ret;
}
#endif	//#ifdef USE_CHECK_INDIV_INFO
extern INT8U check_changed_price_range(INT8U divisor, long* upper, long* lower);
extern INT8U SaleWeightChk;
#ifdef USE_TOPMART_STRUCTURE
int get_packdate(RTC_STRUCT* time_str)
{
	INT8U v8_1;
	INT16U v16_1;
	INT16U v16_3;
	INT16U v16_4;
	INT16U v16_5;
	
	Report_GetCurTime(time_str,1);
	v8_1 = time_str->hour + status_scale.Plu.packedontime / 100;	// current hour + packed time
	v8_1 += (time_str->min + status_scale.Plu.packedontime % 100) / 60;
	v8_1 /= 24;		// calculate day from hour( 0~23 = 0, 24~47 = 1 ...)
	v16_1 = (INT16U)v8_1 + status_scale.Plu.packedondate;;
	v16_3 = (INT16U)time_str->year;
	v16_4 = (INT16U)time_str->month;
	v16_5 = (INT16U)time_str->date;
#ifdef USE_GSMART_PLU
	if (packed_date)
	{
		date_getbeforeday((short*)&v16_3,(short*)&v16_4,(short*)&v16_5,(short)packed_date);
	}
#endif
	date_getnextday((short*)&v16_3,(short*)&v16_4,(short*)&v16_5,(short)v16_1);
	time_str->year = v16_3;
	time_str->month = v16_4;
	time_str->date = v16_5;
	return 0;
}
int check_topmart_plu(void)
{
	INT8U string_buf[100];
	RTC_STRUCT time_str;

	if(!strncmp((char *)CurTraceStatus.indivStr.individualNO, "777777777777", 12))
	{
		if(strlen(CurTraceStatus.indivStr.individualNO) == 12)
			return 0; // plu, 개체 체크 안함.
	}
	
	if((PluType() == PLU_BY_COUNT) && (status_scale.Plu.max_qty && (status_scale.Plu.max_qty < status_scale.cur_qty))) {
		BuzOn(3);
		sprintf((char*)string_buf, "최대 허용 수량 초과");
		display_message_page_mid((char*)string_buf);
		return -1;
	}
	if(FlagDisplayIndividual) {
		switch(status_scale.Plu.tr_use_no) 
		{
			case 1: //sewon 170224
			case 2:
			case 3:
				if(strlen(CurTraceStatus.indivStr.individualNO) == 12 || strlen(CurTraceStatus.indivStr.individualNO) == 18 
					|| strlen(CurTraceStatus.indivStr.individualNO) == 19 || (strlen(CurTraceStatus.indivStr.individualNO) == 15  && CurTraceStatus.indivStr.individualNO[0] == 'L'))
					break;
				else if(strlen(CurTraceStatus.indivStr.individualNO) == 24 && !strncmp((char *)CurTraceStatus.indivStr.individualNO, "A41", 3))
					break;
				else if(strlen(CurTraceStatus.indivStr.individualNO) == 24 && !strncmp((char *)CurTraceStatus.indivStr.individualNO, "A42", 3))
					break;
				else
				{
					sprintf(string_buf, "이력번호가 아닙니다.");
					display_message_page_mid((char*)string_buf);
					return -1;
				}
/*		
			case 1:	//개체식별번호
				if(strlen(CurTraceStatus.indivStr.individualNO) != 12) {
					sprintf(string_buf, "개체번호가 아닙니다.");
					display_message_page_mid((char*)string_buf);
					return -1;
				}
				break;
			case 2:	//묶음(LOT 번호)
				if(strlen(CurTraceStatus.indivStr.individualNO) != 15 || CurTraceStatus.indivStr.individualNO[0] != 'L') {
					sprintf(string_buf, "묶음 번호가 아닙니다.");
					display_message_page_mid((char*)string_buf);
					return -1;
				}
				break;
			case 3:	//선하증권 번호
				if(strlen(CurTraceStatus.indivStr.individualNO) != 18 && strlen(CurTraceStatus.indivStr.individualNO) != 19) {
					sprintf(string_buf, "선하증권 번호가 아닙니다.");
					display_message_page_mid((char*)string_buf);
					return -1;
				}
				break;
*/
			default:
				break;
		}
		switch(status_scale.Plu.tr_comp_flag) {
			case 1:	//등급 + 품종
				if(status_scale.Plu.group == 1)
				{
					if((CurTraceStatus.indivStr.gradeNo == 1) || (CurTraceStatus.indivStr.gradeNo == 7) || (CurTraceStatus.indivStr.gradeNo == 8) || (CurTraceStatus.indivStr.gradeNo == 9) )
					{
						break;
					}
					else
					{
						sprintf(string_buf, "등급 불일치");
						display_message_page_mid((char*)string_buf);
						return -1;
					}
					
				}
				else
				{
					if(status_scale.Plu.group != CurTraceStatus.indivStr.gradeNo) {
						sprintf(string_buf, "등급 불일치");
						display_message_page_mid((char*)string_buf);
						return -1;
					}
				}
				if(status_scale.Plu.kind_no != CurTraceStatus.indivStr.meatKind) {
					sprintf(string_buf, "품종 불일치");
					display_message_page_mid((char*)string_buf);
					return -1;
				}
				break;
			case 2:	//등급
				if(status_scale.Plu.group == 1)
				{
					if((CurTraceStatus.indivStr.gradeNo == 1) || (CurTraceStatus.indivStr.gradeNo == 7) || (CurTraceStatus.indivStr.gradeNo == 8) || (CurTraceStatus.indivStr.gradeNo == 9) )
					{
						break;
					}
					else
					{
						sprintf(string_buf, "등급 불일치");
						display_message_page_mid((char*)string_buf);
						return -1;
					}
					
				}
				else
				{
					if(status_scale.Plu.group != CurTraceStatus.indivStr.gradeNo) {
						sprintf(string_buf, "등급 불일치");
						display_message_page_mid((char*)string_buf);
						return -1;
					}
				}
				break;
			case 3:	//품종
				if(status_scale.Plu.kind_no != CurTraceStatus.indivStr.meatKind) {
					sprintf(string_buf, "품종 불일치");
					display_message_page_mid((char*)string_buf);
					return -1;
				}
				break;
			default:
				break;
		}		
		
		if(CurTraceStatus.indivStr.harmfulness) {
			BuzOn(3);
			sprintf(string_buf, "담당자 확인 필요 상품");
			display_message_page_mid((char*)string_buf);
			return -1;
		}
		if(CurTraceStatus.indivStr.slaughtDate[1] != 0 && CurTraceStatus.indivStr.slaughtDate[2] != 0) {
			get_packdate(&time_str);
			if(date_getdeltadays(time_str.year + 2000, time_str.month, time_str.date,
				CurTraceStatus.indivStr.slaughtDate[0] + 2000, CurTraceStatus.indivStr.slaughtDate[1], CurTraceStatus.indivStr.slaughtDate[2]) 
				> status_scale.Plu.tr_sellby_date) {
				BuzOn(3);
				sprintf(string_buf, "개체 판매 기한이 지남");
				display_message_page_mid((char*)string_buf);
				return -1;
			}
		}
	} else {
		if(status_scale.Plu.tr_use_no) {
			BuzOn(3);
			sprintf((char*)string_buf, "이력번호 미선택");
			display_message_page_mid((char*)string_buf);
			return -1;
		}
	}
	return 0;
}
#endif

INT16S CheckPluPrint(INT8U check)
// Modified by CJK 20050429
// Modified by LBM 20060307
{
	//	INT8U flag_tmp;
	INT16S status;
	INT8U date_flag, send_date;
	INT8U b_date,b_month,b_year;
	INT8U ret_value = 0;
	char S_msg1[32];
	char S_msg2[32];
#ifdef USE_CONTROL_CHANGING_PRICE
	long upperPrice = 0;
	long lowerPrice = 0;
	INT8U checkedFlag = 0;
#endif
#ifdef USE_TOPMART_STRUCTURE
	INT8U string_buf[100];
#else
	INT8U string_buf[50];
#endif
#ifdef USE_TRACE_STANDALONE
	long indiv_weight;
#endif
#ifdef USE_SUBTOTAL_PRICE_9DIGIT_LIMIT
    INT8U check_subtotal_price;
#endif
	date_flag = get_nvram_bparam(NVRAM_BUSINESS_DATE_FLAG);
	send_date = get_global_bparam(GLOBAL_AREA_MULTIUNITFLAG);
	send_date=(send_date>>7)&0x01;
	
	if(!status_scale.cur_pluid)	return OFF; //plu 가 없으면 no
	if(Operation.OverTotalPrice || Operation.OverUnitPrice)	return OFF;    
	if(Operation.wAuto || Operation.wPrepack)
	{
		if(!CheckPrepackAutoLogin())	return OFF;
	}
#ifdef USE_SUBTOTAL_PRICE_9DIGIT_LIMIT
    check_subtotal_price = sale_check_subtotal_price(status_scale.clerkid, Price.TotalPrice[PRICE_RESULT]);
    if (check_subtotal_price == FALSE)
    {
        BuzOn(4);
		DisplayMsg(global_message[MSG_26_C804]);//"Check price"
		return OFF;
    }
#endif
	//Added by JJANG 20080617 동일 무게 판매 재판매 못하게 수정,  Parameter 554 
	if (get_global_bparam(GLOBAL_PRINT_OPER_FLAG2)&0x08)
	{
		if (!SaleWeightChk && PluType() == PLU_BY_WEIGHT)	//무게 상품일 때만 동작
		{
			DisplayMsg(global_message[MSG_27_C80F]);//"Already printed"
			return OFF;	
		}
	}
#ifdef USE_CONTROL_CHANGING_PRICE
	checkedFlag = check_changed_price_range(status_scale.divisor, &upperPrice, &lowerPrice);
	if(checkedFlag)
	{
		//매가변경 통제
		BuzOn(2);
		sprintf((char*)string_buf, "가격변경 제한 범위 초과");
		display_message_page_mid_delay((char*)string_buf, 20);
		sale_display_update(UPDATE_ALL|UPDATE_CLEARALL);
		return OFF;
	}
#endif
#ifdef USE_PRICE_CHANGEABLE_CHECK
	INT16U plu_addr;
	INT16S size;
	INT8U priceChangeFlag;
	plu_table_search((INT8U)status_scale.departmentid, status_scale.cur_pluid,&plu_addr,0);
	plu_get_field(plu_addr-1,  PTYPE_TAX_NO, (INT8U *)&priceChangeFlag,   &size,sizeof(priceChangeFlag));
	if (priceChangeFlag && !SetproductSaleMode) {
		if((Price.UnitPrice[PRICE_RESULT] != Price.UnitPrice[PRICE_ORIGIN]) || 
			(Price.TotalPrice[PRICE_RESULT] != Price.TotalPrice[PRICE_ORIGIN])) {
			INT8U string_buf[50];
			BuzOn(2);
			sprintf(string_buf, "가격변경 제한");
			display_message_page_mid_delay(string_buf, 20);
			return OFF;
		}
	}
#endif
	if (send_date)	//parameter 716, Send Sale Date Type
	{
		if(date_flag && !network_status.connection)
		{
			b_year = get_nvram_bparam(NVRAM_BUSINESS_DATE+2);
			b_month =get_nvram_bparam(NVRAM_BUSINESS_DATE+1);
			b_date = get_nvram_bparam(NVRAM_BUSINESS_DATE);
			
			if ((b_year != RTCStruct.year) || (b_month != RTCStruct.month) || (b_date != RTCStruct.date))
			{
				strcpy(S_msg1,"시스템 날짜를");
				strcpy(S_msg2,"사용하시겠습니까? (Y/N)");
				ret_value = display_message_check_page(S_msg1,S_msg2,2);
				if (ret_value)
				{
					set_nvram_lparam(NVRAM_BUSINESS_DATE_FLAG,0);
				}	
			} 
		}
	}	
#ifdef USE_TOPMART_STRUCTURE
	if(check_topmart_plu() < 0)
		return OFF;
#endif
#ifdef USE_TRACE_STANDALONE
  #ifndef USE_TOPMART_STRUCTURE
	if (!plu_check_inhibit_ptype(PTYPE_TRACE_FLAG))	// 개체 체크기능
	{
		if (status_scale.Plu.trace_flag && !FlagDisplayIndividual) 
		{
			//DisplayMsg("Check Trace No");
			BuzOn(3);
			sprintf((char*)string_buf, "이력번호 미선택");
			display_message_page_mid((char*)string_buf);
			return OFF;
		}
    #ifdef USE_TRACE_FLAG_0_CHECK
		if (!status_scale.Plu.trace_flag && FlagDisplayIndividual)
		{
			BuzOn(3);		
			//sprintf((char*)string_buf, "개체이력대상이 아닙니다");
			sprintf((char*)string_buf, IndivMsg_NonTrace);
			display_message_page_mid((char *)string_buf);
			return OFF;
		}
    #endif
	}
  #endif //USE_TOPMART_STRUCTURE
#ifndef USE_NHMART_SAFE_MEAT
	if (ModeIndiv == 2 && FlagDisplayIndividual)
	{
		if (CurTraceStatus.indivStr.index != MAX_INDIVIDUAL_NO && !CurTraceStatus.indivStr.lotFlag)	//스캔한 개체 제외, 묶음번호 제외
		{
			indiv_weight = get_nvram_lparam(NVRAM_TRACE_STANDALONE_WEIGHT+((INT32U)CurTraceStatus.indivStr.index-1)*4);
			if (indiv_weight <= 0)	//무게 없을 때
			{
				BuzOn(3);
				sprintf((char*)string_buf, "개체중량이 소진되었습니다");
				display_message_page_mid((char*)string_buf);
				return OFF;
			}
		}
	}
#endif
#elif defined(USE_CHN_CART_SCALE)
	if (Operation.useCheckRemain)	// param735
	{
		if (CurCHNTraceLotNo)
		{
			if (status_scale.Plu.fixed_weight)
			{
				if ((INT32S)ad_cnv_weight_by_gram(status_scale.Plu.fixed_weight) > CurCHNTraceRemainWeight)
				{
					BuzOn(3);
					return OFF;
				}
			}
			else
			{
				if ((INT32S)ad_cnv_weight_by_gram(status_scale.Weight) > CurCHNTraceRemainWeight)
				{
					BuzOn(3);
					return OFF;
				}
			}
		}
	}
#elif defined(USE_INDIA_FUNCTION)
	if (!AllowNegativeInventory)
	{
		if(!CheckInventory(status_scale.departmentid, status_scale.cur_pluid, status_scale.Plu.ptype, PluType(), Price.Weight[PRICE_RESULT], Price.Qty[PRICE_RESULT]))
		{
			BuzOn(3);
			sprintf((char*)string_buf, "Not Sufficient Inventory");
			display_message_page_mid((char*)string_buf);
			return OFF;
		}
	}
#endif
	// DEL CJK080709
	/*
	 //호출에 한번만 출력 가능
	 if(GlbOper.printOneTimeAfterCall && status_scale.cur_pluid && Operation.transactionOK)
	 {
	 //DEL070126		flag_tmp=get_global_bparam(GLOBAL_PRINT_OPER_FLAG2);	// prepack 시 1회만 출력
	 //if(!OnOff(flag_tmp&0x08) && (Operation.wPrepack || (Operation.wAuto && Operation.wSave)))
	 if(Operation.wPrepack || (Operation.wAuto && Operation.wSave))
	 {
	 }// 통과
	 else
	 {
	 DisplayMsg(global_message[MSG_27_C80F]);//"Already printed"
	 return OFF;	
	 }
	 }
	 */
	ReportUseDailyReportBackUp();	//Added by JJANG 20080519, PLU REPORT BACK UP
	
	
	PrevPrintOriginPrice = Price.TotalPrice[PRICE_ORIGIN];
	
	//Modified by JJANG 
	//by weight 일 때, 무게가 없으면 판매 안됨,
	//by count, by pcs 일 때, 무게와 관계없이 판매 됨 (parameter 570 동작 X)
	if(GlbOper.weightZeroPricePrint)
	{
		if (PluType() == PLU_BY_WEIGHT)
		{
			if(!Price.UnitPrice[PRICE_RESULT] && 0L < status_scale.Weight)
				return ON;	// weight가있고 unit price zero일때도 출력 가능-> 출력 조정
		}
		else return ON;
	}
#ifdef USE_EMART_BACK_LABEL
	if(Operation.BackLabeledFlag) return ON;	//이마트 후방라벨인 경우 무게나 가격 없어도 인쇄.
#endif
#ifdef USE_SETPRODUCT_SALE
	if(nonweighing_mode) return ON; // 미계량 라벨 모드 일 때에는 무게가 없을 경우에도 인쇄.
#endif

	status = check_status_weight(check);
	if (!disp_error_weight_msg(status)) {
		return OFF;
	}

	sale_calcprice();
	if(DiscSale.saleDiscountType)
	{
		if(DiscSale.discIndexData.discPriceType == DISC_PRICE_FREE && !Price.TotalPrice[PRICE_ORIGIN])
		{
			DisplayMsg(global_message[MSG_26_C804]);//"Check price"
			return OFF;
		}
		if(DiscSale.discIndexData.discPriceType == DISC_PRICE_FIXED && !Operation.fixedDiscFlag)
		{
			DisplayMsg(global_message[MSG_26_C804]);//"Check price"
			return OFF;
		}
	}
	
	else if(!Price.TotalPrice[PRICE_RESULT])
	{
		//if (get_global_bparam(GLOBAL_SALE_SETUP_FLAG2)&0x20) {	// Deleted by CJK 20060306
		//	if (Price.UnitPrice[PRICE_RESULT]) return ON;
		//	return OFF;
		//} else {
		
		if(check) return OFF;
		DisplayMsg(global_message[MSG_26_C804]);//"Check price"
		return OFF;
		//			}
	}
	return ON;
}
	
void ClearAndStart(void)
{
	//		if(status_scale.cur_pluid)
	//		{
	Operation.salePluClearWait = ON;
	Operation.directClearWeight = ON;
	//		}
	//		else
	//		{
	//			sale_pluclear();
	//			display_lcd_clear();
	//			sale_display_update(0x0fff);
	//		}
}

INT16S MiscNameCopy(char *p,INT32U	pluid)
{
	switch(pluid)
	{
		case MISC_PLU_WEIGHT:
			strcpy(p,global_message[MSG_MISC_WEIGHT]);
			break;
		case MISC_PLU_PCS:
			strcpy(p,global_message[MSG_MISC_PCS]);
			break;
		default:
			strcpy(p,global_message[MSG_MISC_COUNT]);
			break;
	}
	return 0;
}

TRANSACT_CHAIN	tempList[MAX_ADD_TRANSACTION];	// Modified by CJK 20050728
void keyapp_void(void)
{
	TRANSACT_CHAIN  *cp;
	TRANSACT_CHAIN  tc;
	INT16U result, next, h_index, blankFlag;
	INT16S	i,index,j,lnkedIndex[MAX_LINKED_PLU],maxIndex;
	long	gPrice,totalPrice,linkedPrice[MAX_LINKED_PLU],sz;
	long	voidNumber=0;
	INT16S	pageList, count;
	INT16S	viewList=0,xx;
	INT16S	viewIndex=0;
	char	priceBuf[15];
	PLU_BASE plu;
	INT16U	lastClerk;
	char    pluname[22];
	char	old;
	char	tempStr[50];
	INT8U   err;
	long	temptrtvolume;	 //se-hyung 20071017
	char    char_w, char_h;
	INT16S	ret;
	INT32U addr;

	temptrtvolume = 0;		 //se-hyung 20071017
	lastClerk = status_scale.clerkid;
	err = OFF;

	ret = NetClerkLock(lastClerk);
	if (ret == 3) goto ENDVOID;//return;
	else if (ret == 0) return;
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
	PutXStringOffsetX = DSP_SALE_INFO_AREA_X1;
	PutXStringOffsetY = DSP_SALE_INFO_AREA_Y1;

	pageList = MAX_STOTAL_LIST_PER_PAGE;
#endif
//SG060711
//===========================================================

	h_index = get_nvram_lparam(NVRAM_TRANS_HEAD+lastClerk*4);
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
	if(!cp)
	{
		sale_display_update_error(0xC03A,3,lastClerk);	     //se-hyung 20071130 subtotal의 영향으로 Y좌표가 8늘어난 것에서 영향 안받기 위한 수정 
		goto ENDVOID;
	}
	totalPrice = Price.TotalPrice[PRICE_RESULT];
	index = ClerkSearchVoidTransaction(status_scale.departmentid,status_scale.cur_pluid, lastClerk, ON, &totalPrice);
	gPrice = totalPrice;
#ifndef USE_PLU_MULTIPLE_PRICE	//Refefence PLU 기능 사용 안 함
	if(-1 < index)
	{
		for(j=0;j<MAX_LINKED_PLU;j++)
		{
			lnkedIndex[j]=-1;
			if(status_scale.linked_dept[j] && status_scale.linked_plu[j])
			{
				if(Price.TotalPrice[PRICE_RESULT])
					linkedPrice[j] = LinkedPrice[j].TotalPrice[PRICE_RESULT];
				else
					linkedPrice[j] = 0;
				lnkedIndex[j] = ClerkSearchVoidTransaction(status_scale.linked_dept[j],status_scale.linked_plu[j] , lastClerk, ON, &linkedPrice[j]);
				
				if(-1<lnkedIndex[j])	gPrice += linkedPrice[j];
			}
		}
	}
#endif
	ClearDisp7();
	display_lcd_clear();
	if(-1 < index)
	{
		// no list
		FloatToString(TO_STR,priceBuf,8,DISPLAY_PRICE,&gPrice,0,1);
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
		GetPriceString(tempStr,priceBuf,ON,PRICE_PRINTER);
		caption_split_by_code(0xc024,&cap,0);//"Voided %s"
		strcat(cap.form," %s");
		sprintf(gmsg,cap.form,tempStr);
		display_lcd_clear();	// CJK080221
		PutNMediumString(1, 0,(char *)gmsg, 30);
		Key_DelaySec(20,0);
#else
		caption_split_by_code(0xc024,&cap,0);//"Voided %s"
		display_lcd_clear();	// CJK080221
		display_string_pos(10, (INT8U*)cap.form);
		display_string(DISPLAY_PRICE, (INT8U*)priceBuf);
		VFD7_Diffuse();
		Key_DelaySec(20,0);
		display_string_pos_clear(10, 20);
#endif
		ClearAndStart();
		goto ENDVOID;
		//return;
	}
	
	maxIndex=0; 
  	for(;cp;)
	{
		tempList[maxIndex] = *cp;
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
		maxIndex++;
#endif
		if (!cp->next) break;
		next = cp->next;
		get_nvram_trans_addr(NVRAM_TRANS_DATA, next - 1, &tc);
		cp = &tc;
#if !defined(_USE_LCD20848_) && !defined(_USE_LCD32_)
		maxIndex++;
#endif
	}
#if !defined(_USE_LCD20848_) && !defined(_USE_LCD32_)
	viewIndex = maxIndex;
#endif
	for(;;)
	{
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
		count=0;
		for(i=viewList;i<maxIndex ;i++)
		{
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
			if (i <= maxIndex)
#else
			if (i < maxIndex)
#endif
			{
				if(tempList[maxIndex-i-1].delete)		strcpy(tempStr,global_message[MSG_29_DSP_VOID]);	//"VOID"
				else				strset(tempStr,0x20,strlen(global_message[MSG_29_DSP_VOID]));
				temptrtvolume = tempList[maxIndex-i-1].content.trtVolume;					  //se-hyung 20071017
				if(tempList[maxIndex-i-1].content.trtStatus& FLAG_TRANS_NEGATIVE)				  //se-hyung 20071016
				{
					temptrtvolume = -tempList[maxIndex-i-1].content.trtVolume;
				}
				FloatToString(TO_STR,priceBuf,8,DISPLAY_PRICE,&temptrtvolume,0,1);  //se-hyung 20071017
#ifdef USE_ARAB_CONVERT
                sprintf(gmsg,"%2d,%s %s ",maxIndex-i,tempStr,priceBuf);	//  
#else
				sprintf(gmsg,"%3d,%s %s ",maxIndex-i,tempStr,priceBuf);	// Modified by CJK 20050728
#endif
				plu_table_search(tempList[maxIndex-i-1].content.trtDept,tempList[maxIndex-i-1].content.trtPLU,&plu.address,0);
				Flash_flush_nvbuffer();
				if(Startup.menu_type)
				{
					if(tempList[maxIndex-i-1].content.trtPLU<MISC_PLU_WEIGHT)
					{
						addr = plu_get_field_point((INT16S)(plu.address-1),PTYPE_PLU_NAME1,(INT16S*)&sz); // Name
						Flash_sread(addr,(HUGEDATA INT8U*)pluname,20);
						blankFlag = IsBlankString((char *)pluname,20);
						if (blankFlag)
							strcpy(pluname,global_message[MSG_NO_PLUNAME]);
					}
					else
					{
						MiscNameCopy(pluname,tempList[maxIndex-i-1].content.trtPLU);
					}
					pluname[20]=0;
				}///////////////////////
				else
				{
					sprintf(pluname,"   %s%07ld",global_message[MSG_30_PLU],tempList[maxIndex-i-1].content.trtPLU);
				}
				strcat(gmsg,pluname);
			}
			else
				strset(gmsg,0x20,35);
			PutNSmallString((i%pageList)+1,0,gmsg,35);
			count++;
			if(count == pageList)	break;
		}
		for(;count<pageList ;count++)
		{
			strset(gmsg,0x20,35);
			PutNSmallString(count+1,0,gmsg,35);
		}
		xx = 0;

		if(Operation.operationClerk==OPER_NORMAL)
		{
			sprintf(gmsg,global_message[MSG_VOID_LIST],maxIndex);
			strcat(gmsg,global_message[MSG_VOID_LIST_2]);
		}
		else
		{
			sprintf(gmsg,"#%2d ",lastClerk);
			sprintf(tempStr,global_message[MSG_VOID_LIST],maxIndex);	// "VOID # [  ]   %-3d "
			strcat(gmsg,tempStr);
			strcat(gmsg,global_message[MSG_VOID_LIST_2]);	// "LIST"
			xx = 4;
		}
		str_fill(gmsg,35);
		if (!MsgRDisplayTime || !err)	// CJK070419 : net error 메세지를 유지하기 위해
		{
			PutNSmallString(0,0,gmsg,35);
		}
		err = OFF;

		old=DspStruct.Id1;
		DspLoadFont1(DSP_SMALL_FONT_ID);
		char_w = display_font_get_width();
		char_h = display_font_get_height();
#ifdef USE_ARAB_CONVERT
        result = keyin_ulong(KI_GRAPHIC, KI_FORM_ILNN, 0,(long*)&voidNumber, 2,
		     				0*char_h+PutXStringOffsetY, (xx+8)*char_w, 0,KI_NOPASS );
#else
		result = keyin_ulong(KI_GRAPHIC, KI_FORM_NORMAL, 0,(long*)&voidNumber, 3,
							0*char_h+PutXStringOffsetY, (xx+8)*char_w, 0,KI_NOPASS );
#endif
		keyin_clearcallback();
		DspLoadFont1(old);
#else
			if (viewIndex <= maxIndex)
			{
				if(tempList[viewIndex].delete)		strcpy(tempStr,global_message[MSG_29_DSP_VOID]);
				else				strset(tempStr,0x20,strlen(global_message[MSG_29_DSP_VOID]));
				temptrtvolume = tempList[viewIndex].content.trtVolume;					  //se-hyung 20071017
				if(tempList[viewIndex].content.trtStatus& FLAG_TRANS_NEGATIVE)			      //se-hyung 20071016
				{
					temptrtvolume = -tempList[viewIndex].content.trtVolume;
				}
				FloatToString(TO_STR,priceBuf,display_parameter.mode_len[DISPLAY_PRICE],DISPLAY_PRICE,&temptrtvolume,0,1);  //se-hyung 20071017

				sprintf(gmsg,"%3d,%s %s ",viewIndex+1,tempStr,priceBuf);	// Modified by CJK 20050728
				plu_table_search(tempList[viewIndex].content.trtDept,tempList[viewIndex].content.trtPLU,&plu.address,0);
				Flash_flush_nvbuffer();

				if(tempList[viewIndex].content.trtPLU<MISC_PLU_WEIGHT)
				{
					addr = plu_get_field_point((INT16S)(plu.address-1),PTYPE_PLU_NAME1,(INT16S*)&sz); // Name
					Flash_sread(addr,(HUGEDATA INT8U*)pluname,20);
					blankFlag = IsBlankString((char *)pluname,20);
					
					if (blankFlag)
						strcpy(pluname,global_message[MSG_NO_PLUNAME]);
				}
				else
				{
					MiscNameCopy(pluname,tempList[viewIndex].content.trtPLU);
				}
				pluname[20]=0;
			}
			else
			{
				memset(pluname,0x20,sizeof(pluname));
			}
			memset(tempStr,0x20,sizeof(pluname));
			memcpy(tempStr,pluname,strlen(pluname));
			disp_p.x = 0;
			disp_p.y = 0;
		Dsp_Write_String(disp_p, tempStr);
		
		if(Operation.operationClerk==OPER_NORMAL)
		{
			sprintf(tempStr,global_message[MSG_VOID_LIST],maxIndex);
		}
		else
		{
			sprintf(gmsg,"#%2d ",lastClerk);    
			sprintf(tempStr,global_message[MSG_VOID_LIST],maxIndex);
			strcat(gmsg,tempStr);
			strcat(gmsg,global_message[MSG_VOID_LIST_2]);
		}
		str_fill(gmsg,35);
		display_string(DISPLAY_WEIGHT, (INT8U*)global_message[MSG_VOID_LIST]);
		VFD7_Diffuse();
		
		while(!KEY_Read());
		
		result = KeyDrv.CnvCode;			
#endif
		if(result==KP_ESC)
		{
			BuzOn(1);
			ClearDisp7();
			display_lcd_clear();
			break;
		}
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
		if(voidNumber && result==KP_ENTER)// || result==KP_SAVE))
		{
			if(tempList[voidNumber-1].delete)
			{
				if(ClerkVoidTransaction(lastClerk, (INT16S)tempList[voidNumber-1].content.trtSerialNo, OFF,status_scale.scaleid, 0) == 0)
					tempList[voidNumber-1].delete=OFF;
				else
					err = ON;
			}
			else
			{
				if(ClerkVoidTransaction(lastClerk, (INT16S)tempList[voidNumber-1].content.trtSerialNo, ON,status_scale.scaleid, 0) == 0)
					tempList[voidNumber-1].delete=ON;
				else
					err = ON;
			}
		}
		switch(result)
		{
				case KP_ARROW_UP:
				case KP_PAGE_UP:
					viewList-=pageList;
					break;
				case KP_ARROW_DN:
				case KP_PAGE_DN:
					if(viewList+pageList < maxIndex)
						viewList+=pageList;
					break;
		}
		if(viewList<0)	viewList=0;
		if(maxIndex<=viewList)	viewList=maxIndex-1;
#else
		switch(result)
		{
				case KP_ARROW_UP:
				case KP_PAGE_UP:
					BuzOn(1);
					if(viewIndex < maxIndex)
						viewIndex++;
					break;
				case KP_ARROW_DN:
				case KP_PAGE_DN:
					BuzOn(1);
					viewIndex--;
					break;
			default:
				BuzOn(2);
				break;
				
		}
		if(viewIndex<0)	viewIndex=0;
		if(maxIndex<=viewIndex)	viewIndex=maxIndex;
#endif
	}
	
	ClearAndStart();
	
	ENDVOID:
		NetClerkLockRelease((INT8U)lastClerk);  //SG060711
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
	PutXStringOffsetX = 0;
	PutXStringOffsetY = 0;
#endif
}

void keyapp_lastvoid(void)	// add-up+clerk(CJK070424)	// only add-up
{
	//TRANSACT_CHAIN  *cp;
	INT16U h_index;
	STRUCT_TOTAL_PRICE totalData;
	INT16U lastClerk;
	INT16S	ret;
	
	lastClerk = status_scale.clerkid;
	
	ret = NetClerkLock(lastClerk);
	
	if (ret == 1 || ret == 2)
	{
		h_index = get_nvram_lparam(NVRAM_TRANS_HEAD+lastClerk*4);
		if (h_index)
		{
			ClerkTotalPrice(0, lastClerk, &totalData);	// mode:2 => 0
			if (ClerkVoidTransaction(lastClerk, (INT16S)(totalData.TotalCount+totalData.VoidCount), ON, status_scale.scaleid, 0) == 0)
			{
				caption_split_by_code(0xc044,&cap,0);//"LAST PLU VOIDED"
				DisplayMsg(cap.form);
			}
			else
			{
				BuzOn(2);
			}
		}
		else
		{
			DisplayMsg((char *)global_message[MSG_NOSALEDATA]);
		}
	}
	else if (ret == 0) return;
	NetClerkLockRelease((INT8U)lastClerk);
	
}

//extern void KEY_Set_Shift(INT8U onoff);

// for Return
extern TRANSACT_CHAIN  TrChain; //SG070213
extern TRANSACTION_RECORD_SUB_TYPE Trans_sub;
extern TR_ADDITION_DATA_TYPE TrReturn_add;
/////////////// 
extern void	InsertTransactionRecord(INT8U trtType,INT8U trtStatus,INT8U trtStatus2,INT16S operator,INT8U dept,INT32U pluNumber,INT8U pluType,INT8U group,PRICE_TYPE *pPrice,TRANSACTION_RECORD_TYPE* ptrans,TRANSACTION_RECORD_SUB_TYPE* ptrans_sub,long rprice );
extern INT16S	LocalClerkSummaries(INT8U scaleNum,INT8U operator,TRANSACTION_RECORD_TYPE* pTrans,TRANSACTION_RECORD_SUB_TYPE* pTrans_sub,TR_ADDITION_DATA_TYPE* pTrans_add);

void makeReturnTrData(PRICE_TYPE *pPrice) //insert transaction data to "TrChain.content"
{
	INT8U trtType,pluType;
	INT8U	operator=status_scale.clerkid;
	//INT8U	scaleNum=status_scale.scaleid;
	INT8U	trtStatus;
	INT8U	trtStatus2;
	//INT16S stat;
	
	if(Operation.operationClerk == OPER_NORMAL)	operator = 0;
	
	trtType = TRANS_TYPE_NORMAL;
	if(Operation.operationMode == MODE_REPACK)	trtType = TRANS_TYPE_REPACK;
	if(Operation.operationMode == MODE_SELF)	trtType = TRANS_TYPE_SELF;
	
	trtStatus = FLAG_TRANS_RETURN;
	if(Operation.transAddup)		trtStatus |= FLAG_TRANS_ADD;
	if(Operation.wPrepack)			trtStatus |= FLAG_TRANS_PREPACK;
	if(Operation.printON)			trtStatus |= FLAG_TRANS_LABEL;
	if(Operation.negative)			trtStatus |= FLAG_TRANS_NEGATIVE;
	if(Operation.override && status_scale.cur_pluid<MISC_PLU_WEIGHT && Price.UnitPrice[PRICE_ORIGIN] != status_scale.Plu.unitprice)
		trtStatus |= FLAG_TRANS_OVERRIDE;
	
	trtStatus2 = 0;
	if (ethernet_list.status == 4)	trtStatus2 |= FLAG2_TRANS_LOCAL;	// CJK080320
	
	pluType = PluType();
	memset(&TrChain, 0 , sizeof(TrChain));
	InsertAdditionalData(&TrReturn_add);
	InsertTransactionRecord(trtType,trtStatus,trtStatus2,operator,status_scale.Plu.deptid,status_scale.cur_pluid,pluType,status_scale.Plu.group,pPrice,&TrChain.content,&Trans_sub,0);
	TrChain.next = 0;
	TrChain.flag = ON;
	//stat = LocalClerkSummaries(scaleNum,operator,&TrChain.content);
	
}

//keyin_setcallback(keyapp_common_disc);
//keyin_anycheck_on=1;
//keyin_escape_clear=1;
void keyapp_callback_salemode(void)
{
	sale_adm_proc();
	CurPluWeightUnit = ADM_GetData.CurUnit;
	sale_calcprice();
	//sale_display_date(OFF);
	//sale_display_total(1);
#if !defined(_USE_LCD20848_) && !defined(_USE_LCD32_)
	sale_display_update(UPDATE_PRICE);
#endif
	sale_display_proc(ON);
}

void AddReturnTrData(void)
{
	INT8U	operator=status_scale.clerkid;
	INT8U	scaleNum=status_scale.scaleid;
	
	LocalClerkSummaries(scaleNum, operator, &TrChain.content, &Trans_sub, &TrReturn_add);
}

extern long InternalTaxPrice(long  taxRate,long totalPrice);
void keyapp_return(void)
{
	//INT16S	index;
	char	priceBuf[15];
	long	tPrice,tWeight,tQty;
	INT16S decpoint;
	INT16U	result;
	INT16S	okFlag=ON;
	INT8U	byte;
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
	char	old;
	char 	char_w, char_h;
#endif
	//	INT16S	pDecimal;
	char	cPriceStr[30];
	//	INT8U	state;
	INT8U	ucTemp;
	INT8U	printReturn;
	INT8U   flag1, flag2;
#ifdef USE_KOR_TRACE
	INT8U	kr_flag=0;
#endif
	POINT disp_p;
	
	disp_p.x = 0;
	disp_p.y = 0;
	
	//	INT8U	Discount_flag;
	//	Discount_flag = get_global_bparam(GLOBAL_SALE_SETUP_FLAG7);
	
	if(Operation.negative) {
		DisplayMsg(global_message[MSG_24_C04C]);
		return;
	}
	byte = get_global_bparam(GLOBAL_SALE_SETUP_FLAG2);
	if(OnOff(byte&0x04)) {   //option:
		KEY_Set_Shift(3);// 3 = auto change
		return;
	}
	if(!status_scale.cur_pluid)	return;
	if(!GlbOper.XZreport)
	{
		//DisplayMsg("No X report");
		return;
	}
	
	//SG071113 
	if(PluType()==PLU_BY_WEIGHT) //무게상품일경우 안정될때까지 기다려야 함
	{
		while(!ADM_GetData.Stable){
			ProcessAll();
		}
	}
	
	KEY_SetMode(1);
	flag2=0;
	if(Price.DisplayedPrice) {
		tPrice = Price.DisplayedPrice;
	} else {
		flag2=1;
		decpoint=DecimalPointPrice();
		tPrice = 0;
		tWeight = 0;
		tQty = 0;
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
		old=DspStruct.Id1;
		DspLoadFont1(DSP_SMALL_FONT_ID);
		char_w = display_font_get_width();
		char_h = display_font_get_height();
		caption_split_by_code(0xc018,&cap,1);//strcpy(gmsg,"PRICE        [         ]");
		PutString(DSP_MSG_Y, DSP_MSG_X, cap.form, DSP_MSG_FONT_ID, DSP_MSG_MAGY, DSP_MSG_MAGX, MAX_MSG_CHAR);
		result = keyin_ulong(KI_GRAPHIC, KI_FORM_NORMAL, decpoint,(long*)&tPrice, 8,
								DSP_MSG_Y+0*char_h,  DSP_MSG_X+15*char_w, 0,KI_NOPASS );
#else
		caption_split_by_code(0xc018,&cap,0);//strcpy(gmsg,"PRICE        [         ]");
		ClearDisp7();
		display_string(DISPLAY_UNITPRICE, (INT8U*)cap.form);
		VFD7_Diffuse();
		result = keyin_ulong(KI_TOTAL, KI_FORM_NORMAL, decpoint,(long*)&tPrice, 8, 
				0,  0, 0,KI_NOPASS );
#endif
		if (result==KP_ESC) goto NEXT;
		if(PluType()==PLU_BY_WEIGHT) {
			decpoint=DecimalPointWeight();
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
			caption_split_by_code(0xc019,&cap,1);//strcpy(gmsg,"WEIGHT          [      ]");
			PutString(DSP_MSG_Y, DSP_MSG_X, cap.form, DSP_MSG_FONT_ID, DSP_MSG_MAGY, DSP_MSG_MAGX, MAX_MSG_CHAR);
			result = keyin_ulong(KI_GRAPHIC, KI_FORM_NORMAL, decpoint,(long*)&tWeight, 6,
									DSP_MSG_Y+0*char_h,  DSP_MSG_X+17*char_w, 0,KI_NOPASS );
#else
			caption_split_by_code(0xc019,&cap,0);//strcpy(gmsg,"WEIGHT          [      ]");
			ClearDisp7();
			display_string(DISPLAY_UNITPRICE, (INT8U*)cap.form);
			VFD7_Diffuse();
			result = keyin_ulong(KI_TOTAL, KI_FORM_NORMAL, decpoint,(long*)&tWeight, 6, 
					0,  0, 0,KI_NOPASS );
#endif
		} else {
			caption_split_by_code(0xc01a,&cap,1);//strcpy(gmsg,"QUANTITY            [  ]");
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
			PutString(DSP_MSG_Y, DSP_MSG_X, cap.form, DSP_MSG_FONT_ID, DSP_MSG_MAGY, DSP_MSG_MAGX, MAX_MSG_CHAR);
			result = keyin_ulong(KI_GRAPHIC, KI_FORM_NORMAL, 0,(long*)&tQty, 2,
									DSP_MSG_Y+0*char_h,  DSP_MSG_X+21*char_w, 0,KI_NOPASS );
#else
			ClearDisp7();
			display_string(DISPLAY_UNITPRICE, (INT8U*)cap.form);
			VFD7_Diffuse();
			result = keyin_ulong(KI_TOTAL, KI_FORM_NORMAL, 0,(long*)&tQty, 2, 
					0,  0, 0,KI_NOPASS );
#endif
		}
		NEXT:
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
				DspLoadFont1(old);
#endif
				if (result==KP_ESC) goto RETURN_CANCEL;
		//	}
		//	if(!Price.DisplayedPrice) { // 입력값이면
		//		pDecimal = (INT16S)get_global_bparam(GLOBAL_AREA_PRICE_DECIMALPOINT);
		if(PluType()==PLU_BY_WEIGHT) {
			//			if(!tWeight)	okFlag=OFF;  // 무게값에 0 을 입력해도 리턴 가능하게 수정
			Price.Weight[PRICE_RESULT]	= tWeight;
			
		} else {
			if(!tQty)	okFlag=OFF;
			Price.Qty[PRICE_RESULT]		= (INT16S)tQty;
		}
		
		Price.TotalPrice[PRICE_RESULT]	= tPrice;
		Price.DisplayedPrice = tPrice;
		
		//Price.TaxPrice = longmuldiv(Price.TotalPrice[PRICE_RESULT],Price.TaxRate,10000);
		// tax
		if(GlbOper.priceAddExTax == 0)
			Price.TaxPrice = longmuldiv(Price.DisplayedPrice, Price.TaxRate,10000);
		else if(GlbOper.priceAddExTax == 1)
			Price.TaxPrice = InternalTaxPrice(Price.TaxRate, Price.DisplayedPrice);
		
	}								   
	if(!tPrice)	okFlag = OFF;
	//Dsp_Fill_Buffer(0);
	//Dsp_Diffuse();
	display_lcd_clear();
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
	PutXStringOffsetX = DSP_SALE_INFO_AREA_X1;
	PutXStringOffsetY = DSP_SALE_INFO_AREA_Y1;
#endif
	ClearPayment(TRANS_TYPE_PAY_TOTAL_NOPAY);	//Added by JJANG 20080619 return시 초기화 안되는 문제 수정

//Added by JJANG 071123 저울나라에서 반품요청을 받아들여야 반품행위와 리포트 갱신을 한다.
#ifdef USE_KOR_TRACE
   if(okFlag)
   {
	   kr_flag = keyapp_ktr_req_returnid();
	   if(!kr_flag) okFlag = OFF;
   }
#endif

	if(okFlag) 
	{
		
		makeReturnTrData(&Price); // CJK080327
		//ProcessAll(); //SG071113
		
		//		SaleAcctLocalAddTransaction(status_scale.Plu.deptid,status_scale.cur_pluid,&Price,FLAG_TRANS_RETURN);
		//		FloatToString(TO_STR,priceBuf,8,DISPLAY_PRICE,&tPrice,0);
		//ORG		FloatToString(TO_STR,priceBuf,8,DISPLAY_PRICE,&Price.DisplayedPrice,0);
		FloatToString(TO_STR,priceBuf,8,DISPLAY_PRICE,&Price.TotalPrice[PRICE_RESULT],0,1); //Modified by JJANG 20080104 무조건 RESULT값을 찍도록!
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
		GetPriceString(cPriceStr,priceBuf,ON,PRICE_PRINTER);
		sprintf(gmsg,global_message[MSG_RETURN],cPriceStr);
		PutNMediumString(1,0,gmsg,20);
#else
		display_string_pos(10, (INT8U*)global_message[MSG_RETURN]);
		display_string(DISPLAY_PRICE, (INT8U*)priceBuf);
		VFD7_Diffuse();
#endif
		Cash_open();	// Modified by CJK 20051110
		
		//SG070214
		ucTemp = get_global_bparam(GLOBAL_SALE_SETUP_FLAG5);
		printReturn = (ucTemp>>5)&0x01; 
		
		flag1 =ON;
		if(printReturn)
		{
			////SGTEST SG071113 /////////////////////////////////////////////////////////
			if(flag2) status_scale.Weight = Price.Weight[PRICE_RESULT];
			sale_calcprice();
			if(flag2) Price.TotalPrice[PRICE_RESULT]=tPrice;
			sale_display_update(0x0fff);
			sale_display_proc(OFF);
			
			SaleInsertLabelData();
			LabelData.Price.Weight[PRICE_RESULT] = -LabelData.Price.Weight[PRICE_RESULT];
			LabelData.Price.TotalPrice[PRICE_RESULT] = -LabelData.Price.TotalPrice[PRICE_RESULT];
			LabelData.Price.DisplayedPrice = -LabelData.Price.DisplayedPrice;
			LabelData.Price.Qty[PRICE_RESULT] = -LabelData.Price.Qty[PRICE_RESULT];
			
			StartPrintFlag = ON;
			//Modified by JJANG 20080619 clerk 0 --> status_scale.clerkid
			flag1 =Prt_PrintStart(PRT_ITEM_RETURN_MODE, PrtStruct.Mode, status_scale.clerkid, OFF, 0, 0);
			PrintListWaitEnd();
			//SaleAcctLocalAddTransaction(status_scale.Plu.deptid,status_scale.cur_pluid,&Price,0);
			StartPrintFlag = OFF;
			/////////////////////////////////////////////////////////////
			
		}
		if(flag1 == 1)
		{
			//makeReturnTrData(&Price); //SG070703
			AddReturnTrData(); // CJK080327
		}
	} 
	else 
	{
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
		strcpy(gmsg,"");
		PutNSmallString(0,0,gmsg,33);
		caption_split_by_code(0xc801,&cap,0);//strcpy(gmsg,"Check return value");
		PutNMediumString(1, 0,cap.form, 20);
#else
		caption_split_by_code(0xc801,&cap,0);//strcpy(gmsg,"Check return value");
		Dsp_Write_String(disp_p, cap.form);
#endif
	}
	Key_DelaySec(10,0);
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
	PutXStringOffsetX = 0;
	PutXStringOffsetY = 0;
#endif
	RETURN_CANCEL:
#if !defined(_USE_LCD20848_) && !defined(_USE_LCD32_)
				display_string_pos_clear(5,20);
#endif
	KEY_SetMode(0);
	ClearAndStart();
}

void keyapp_cleardiscount(void)
{
	if (DiscSale.saleDiscountType!=DISCOUNT_NO) {
		DiscSale.discIndexData.discTargetType=0;
		DiscSale.saleGlobalOk         =OFF;
		DiscSale.saleDiscountType     =DISCOUNT_NO;
		DiscSale.saleDiscFSP	      =0;
		Price.UnitPrice[PRICE_RESULT] =Price.UnitPrice[PRICE_ORIGIN];
		Price.TotalPrice[PRICE_RESULT]=Price.TotalPrice[PRICE_ORIGIN];
		//DEL070705		sale_display_discount(OFF);
		sale_display_update(0x0fff);
		//DEL070706		sale_display_total(0);
	}
}

INT16S key_common_disc1=0;
INT16S key_common_disc2=0;

void keyapp_common_disc(long tPrice)
{
	INT8U	flag_tmp;
    char	old;
#ifdef USE_PRT_CHNIA_DISCOUNT_RATE
	long chnPer;
#endif
    old=DspStruct.Id1;
	
	DiscSale.saleGlobalOk = ON;
	DiscSale.saleDiscountType = DISCOUNT_GLOBAL;
	if(status_scale.cur_pluid < MISC_PLU_WEIGHT)
	{
		Operation.override = OFF;
		Netweight_flag= OFF;
		SetMaxRide(PluType());
		SetCurRide(PluType());
	}
	
	DiscSale.discIndexData.discTargetType = DISC_TARGET_TPRICE;
	flag_tmp=get_global_bparam(GLOBAL_SALE_SETUP_FLAG2); // Parameter 585
	if(OnOff(flag_tmp&0x08)) {	// u.p discount
		DiscSale.discIndexData.discPriceType = key_common_disc1;
		if(Operation.fixedPrice) DiscSale.discIndexData.discPriceType =key_common_disc2;
	} else {
		DiscSale.discIndexData.discPriceType = key_common_disc2;
	}
#ifdef USE_EDIT_TOTALPRICE
	if (key_common_disc1==DISC_PRICE_UPRICE || key_common_disc1==DISC_PRICE_USPECIAL || key_common_disc1==DISC_PRICE_EDITTOTAL) {
#else
  #ifdef USE_TOPMART_DEFAULT
	if (key_common_disc1==DISC_PRICE_UPRICE || key_common_disc1==DISC_PRICE_USPECIAL || key_common_disc1==DISC_PRICE_TOTALTOUNIT) { //sewon 170118
  #else
	if (key_common_disc1==DISC_PRICE_UPRICE || key_common_disc1==DISC_PRICE_USPECIAL) {
  #endif
#endif
		DiscSale.discIndexData.discTarget[0] = 0;//tPrice;//Price.TotalPrice[PRICE_RESULT];
		DiscSale.discIndexData.discTarget[1] = 0;//tPrice;//Price.TotalPrice[PRICE_RESULT];
		DiscSale.discIndexData.discPrice[0] = tPrice;
		DiscSale.discIndexData.discPrice[1] = tPrice;
	} else {
		DiscSale.discIndexData.discTarget[0] = 0;//1;
		DiscSale.discIndexData.discTarget[1] = 0;//1;
#ifdef USE_PRT_CHNIA_DISCOUNT_RATE
		chnPer = 100 - tPrice;
		DiscSale.discIndexData.discPrice[0] = chnPer*100;
		DiscSale.discIndexData.discPrice[1] = chnPer*100;
#else
		DiscSale.discIndexData.discPrice[0] = tPrice*100;
		DiscSale.discIndexData.discPrice[1] = tPrice*100;
#endif
	}
	//DEL071204	sale_adm_proc();
	//DEL071204	CurPluWeightUnit = ADM_GetData.CurUnit;
	//DEL071204	sale_calcprice();
	//DEL071204	sale_display_total(1);
	keyapp_callback_salemode();
	if (tPrice==0l) {
		DiscSale.saleGlobalOk = OFF;
		DiscSale.saleDiscountType = DISCOUNT_NO;
	}
    DspLoadFont1(old);
}

void keyapp_common_disccancel(void)
{
	DiscSale.saleGlobalOk = OFF;
	DiscSale.saleDiscountType = DISCOUNT_NO;
	//DEL071204	sale_adm_proc();
	//DEL071204	CurPluWeightUnit = ADM_GetData.CurUnit;
	//DEL071204	sale_calcprice();
	//DEL071204	sale_display_total(1);
	keyapp_callback_salemode();
}

void keyapp_minus_disc(void)
{
	long	result;
	long	tPrice;
	INT16S decpoint=DecimalPointPrice();
	char	old;
	INT8U	uctemp;
	INT8U   old_key;
	char    str[36];
	char    char_w;
	CAPTION_STRUCT	cap;
#ifdef USE_PRICE_CHANGEABLE_CHECK
	INT16U plu_addr;
	INT16S size;
	INT8U priceChangeFlag;
	INT8U string_buf[20];
#endif

	if(!status_scale.cur_pluid) {
		BuzOn(2);            
		return;
	}
#ifdef USE_PRICE_CHANGEABLE_CHECK
	plu_table_search((INT8U)status_scale.departmentid, status_scale.cur_pluid,&plu_addr,0);
	plu_get_field(plu_addr-1,  PTYPE_TAX_NO, (INT8U *)&priceChangeFlag,   &size,sizeof(priceChangeFlag));
	if (priceChangeFlag)
	{
		BuzOn(2);
		sprintf(string_buf,"가격변경 제한");
		display_message_page_mid_delay(string_buf,10);
		return;
	}
#endif

	old_key = KEY_GetMode();
	KEY_SetMode(1);
	
	//CheckZeroTotalPrice();
	Operation.specialPriceDisc = OFF;
	
	
	tPrice = 0;
	
	old=DspStruct.Id1;
	DspLoadFont1(DSP_MSG_FONT_ID);
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
	caption_split_by_code(0xc01d,&cap,0);
#ifdef HEBREW_FONT
	strcat(cap.form, ")-(");
#else
	strcat(cap.form, "(-)");
#endif
#else
	caption_split_by_code(0xc01c,&cap,0);
#endif
    if(decpoint)
    	cap.input_length = get_global_bparam(GLOBAL_DISPLAY_UNITPRICELENGTH)+1;//unit price digit + decimal point
    else
       cap.input_length = get_global_bparam(GLOBAL_DISPLAY_UNITPRICELENGTH);      
	caption_adjust(&cap, NULL);
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
	PutString(DSP_MSG_Y, DSP_MSG_X, cap.form, DSP_MSG_FONT_ID, DSP_MSG_MAGY, DSP_MSG_MAGX, MAX_MSG_CHAR);
#else
	display_clear(DISPLAY_WEIGHT);
	display_string(DISPLAY_WEIGHT,(INT8U *)cap.form);
	VFD7_Diffuse();
#endif
	DspLoadFont1(DSP_MSG_FONT_ID);
	char_w = display_font_get_width();
	
	key_common_disc1=DISC_PRICE_UPRICE;
	key_common_disc2=DISC_PRICE_TPRICE;
#if !defined(USE_SINGLE_LINE_GRAPHIC)
	keyin_setcallback(keyapp_common_disc);
#endif
	keyin_setanycheck(keyapp_common_disc);
	//keyin_anycheck_on=1;
	keyin_escape_clear=1;
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
	#ifdef USE_ARAB_CONVERT
		result = keyin_ulong(KI_GRAPHIC, KI_FORM_ILNN, decpoint,(long*)&tPrice, cap.input_length, DSP_MSG_Y, DSP_MSG_X+cap.input_x*char_w, 0,KI_NOPASS );
	#else
		result = keyin_ulong(KI_GRAPHIC, KI_FORM_NORMAL, decpoint,(long*)&tPrice, cap.input_length, DSP_MSG_Y, DSP_MSG_X+cap.input_x*char_w, 0,KI_NOPASS );
	#endif
#else
	#ifdef USE_ARAB_CONVERT
		result = keyin_ulong(KI_UNIT, KI_FORM_ILNN, decpoint,(long*)&tPrice, cap.input_length, DSP_MSG_Y, DSP_MSG_X+cap.input_x*char_w, 0,KI_NOPASS );
	#else
		result = keyin_ulong(KI_UNIT, KI_FORM_NORMAL, decpoint,(long*)&tPrice, cap.input_length, DSP_MSG_Y, DSP_MSG_X, 0,KI_NOPASS );
	#endif
#endif
	keyin_escape_clear=0;
	keyin_clearcallback();
	DspLoadFont1(old);
	if(result==KP_ENTER || result==KP_SAVE)
	{
		keyapp_common_disc(tPrice);
		Operation.transactionOK = OFF;
#if defined(USE_GSMART_BARCODE) || defined(USE_BARCODE_DISCOUNT_RATE)		
  #ifdef USE_BESTCO_DISCOUNT_DIVISION
	status_scale.discountflag = 40;
  #else
	status_scale.discountflag = 1;
  #endif /* USE_BESTCO_DISCOUNT_DIVISION */
#endif
	} else {
		keyapp_common_disccancel();
	}
	//DEL070705	display_lcd_clear();
	//DEL070705	sale_display_discount(ON);
	memset(str, 0x20, MAX_MSG_CHAR);
	str[MAX_MSG_CHAR] = 0;
	PutString(DSP_MSG_Y, DSP_MSG_X, str, DSP_MSG_FONT_ID, DSP_MSG_MAGY, DSP_MSG_MAGX, MAX_MSG_CHAR);
	sale_display_update(0x0fff);
	
	if (result==KP_ENTER || result==KP_SAVE){
		uctemp = get_global_bparam(GLOBAL_SALE_SETUP_FLAG6);
		if(!(uctemp&0x01))
			keyapp_print(OFF);
	}	
	
	if (result==KP_CLEAR || result==KP_ESC) {
		if ((Operation.fixedAction==2) && (Operation.fixedPrice)) {
			keyapp_fixed_price(OFF,2);
		}
	}
	KEY_SetMode(old_key);
}

void keyapp_sale_percent(void)
{
	long	result;
	long	tPer;
	char	old;
	INT8U   uctemp;
	INT8U   old_key;
	char    str[36];
	char    char_w;
	CAPTION_STRUCT	cap;
#ifdef USE_PRICE_CHANGEABLE_CHECK
	INT16U plu_addr;
	INT16S size;
	INT8U priceChangeFlag;
	INT8U string_buf[20];
#endif

	if (!status_scale.cur_pluid) {
		BuzOn(2);            
		return;
	}
#ifdef USE_PRICE_CHANGEABLE_CHECK
	plu_table_search((INT8U)status_scale.departmentid, status_scale.cur_pluid,&plu_addr,0);
	plu_get_field(plu_addr-1,  PTYPE_TAX_NO, (INT8U *)&priceChangeFlag,   &size,sizeof(priceChangeFlag));
	if (priceChangeFlag)
	{
		BuzOn(2);
		sprintf(string_buf,"가격변경 제한");
		display_message_page_mid_delay(string_buf,10);
		return;
	}
#endif

	old_key = KEY_GetMode();
	KEY_SetMode(1);
	
	//CheckZeroTotalPrice();
	Operation.specialPriceDisc = OFF;
	
	
	tPer = 0;
	
	old=DspStruct.Id1;
	DspLoadFont1(DSP_MSG_FONT_ID);
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
	caption_split_by_code(0xc01d,&cap,0);
#ifdef HEBREW_FONT
	strcat(cap.form, ")%(");
#else
	strcat(cap.form, "(%)");
#endif
#else	
	caption_split_by_code(0xc01d,&cap,0);
#endif
	cap.input_length = 2;
	caption_adjust(&cap, NULL);
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
	PutString(DSP_MSG_Y, DSP_MSG_X, cap.form, DSP_MSG_FONT_ID, DSP_MSG_MAGY, DSP_MSG_MAGX, MAX_MSG_CHAR);
#else
	display_clear(DISPLAY_WEIGHT);
	display_string(DISPLAY_WEIGHT,(INT8U *)cap.form);
	VFD7_Diffuse();
#endif
	DspLoadFont1(DSP_MSG_FONT_ID);
	char_w = display_font_get_width();
	
	key_common_disc1=DISC_PRICE_UPERCENT;
	key_common_disc2=DISC_PRICE_PERCENT;
#if !defined(USE_SINGLE_LINE_GRAPHIC)
	keyin_setcallback(keyapp_common_disc);
#endif
	keyin_setanycheck(keyapp_common_disc);
	//keyin_anycheck_on=1;
	keyin_escape_clear=1;
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
	#ifdef USE_ARAB_CONVERT
		result = keyin_ulong(KI_GRAPHIC, KI_FORM_ILNN, 0,(long*)&tPer, cap.input_length, DSP_MSG_Y,  DSP_MSG_X+cap.input_x*char_w, 0,KI_NOPASS );
	#else
		result = keyin_ulong(KI_GRAPHIC, KI_FORM_NORMAL, 0,(long*)&tPer, cap.input_length, DSP_MSG_Y,  DSP_MSG_X+cap.input_x*char_w, 0,KI_NOPASS );
	#endif
#else
	#ifdef USE_ARAB_CONVERT
		result = keyin_ulong(KI_UNIT, KI_FORM_ILNN, 0,(long*)&tPer, cap.input_length, DSP_MSG_Y,  DSP_MSG_X+cap.input_x*char_w, 0,KI_NOPASS );
	#else
		result = keyin_ulong(KI_UNIT, KI_FORM_NORMAL, 0,(long*)&tPer, cap.input_length, DSP_MSG_Y,  DSP_MSG_X, 0,KI_NOPASS );
	#endif
#endif
	keyin_escape_clear=0;
	keyin_clearcallback();
	DspLoadFont1(old);
	if(result==KP_ENTER || result==KP_SAVE)
	{
		keyapp_common_disc(tPer);
		Operation.transactionOK = OFF; //SG060419
#if defined(USE_GSMART_BARCODE) || defined(USE_PRT_CHNIA_DISCOUNT_RATE)
  #ifdef USE_BESTCO_DISCOUNT_DIVISION
	status_scale.discountflag = 40;
	status_scale.percent_discount_flag = 1;
  #else
	status_scale.discountflag = 2;
  #endif /* USE_BESTCO_DISCOUNT_DIVISION */
		status_scale.discount_rate = (INT8U)tPer;
#endif
	} else {
		keyapp_common_disccancel();
	}
	//sale_pluclear();
	//DEL070705	display_lcd_clear();
	//DEL070705	sale_display_discount(ON);	//clear
	memset(str, 0x20, 35);
	str[35] = 0;
	//PutNSmallString(0, 0, str, 35);
	PutString(DSP_MSG_Y, DSP_MSG_X, str, DSP_MSG_FONT_ID, DSP_MSG_MAGY, DSP_MSG_MAGX, MAX_MSG_CHAR);
	sale_display_update(0x0fff);
	
	if (result==KP_ENTER || result==KP_SAVE){
		uctemp = get_global_bparam(GLOBAL_SALE_SETUP_FLAG6); //SG060419
		if(!(uctemp&0x01))
			keyapp_print(OFF);
	}	
	
	if (result==KP_CLEAR || result==KP_ESC) {
		if ((Operation.fixedAction==2) && (Operation.fixedPrice)) {
			keyapp_fixed_price(OFF,2);
		}
	}
	KEY_SetMode(old_key);
}

void keyapp_sale_specialprice(void)
{
	long	result;
	long	tPrice;
	char	old;
	INT8U   uctemp;
	INT8U   old_key;
	char    str[36];
	char    char_w;
	INT16S	decpoint = DecimalPointPrice();
	CAPTION_STRUCT	cap;
	
	if (!status_scale.cur_pluid) {
		BuzOn(2);            
		return;
	}
	
	old_key = KEY_GetMode();
	KEY_SetMode(1);
	
	//CheckZeroTotalPrice();
	Operation.specialPriceDisc = OFF;
	
	tPrice = 0;
	
	old=DspStruct.Id1;
	DspLoadFont1(DSP_MSG_FONT_ID);
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
	caption_split_by_code(0xc01d,&cap,0);
#ifdef HEBREW_FONT
	strcat(cap.form, ")%(");
#else
	strcat(cap.form, "(Special Unit Price)");
#endif
#else		
	caption_split_by_code(0xc01e,&cap,0);
#endif
	cap.input_length = 7;
	caption_adjust(&cap, NULL);
	display_clear(DISPLAY_WEIGHT);
	display_string(DISPLAY_WEIGHT,(INT8U *)cap.form);
	VFD7_Diffuse();	
	DspLoadFont1(DSP_MSG_FONT_ID);
	char_w = display_font_get_width();
	
	key_common_disc1=DISC_PRICE_USPECIAL;
	key_common_disc2=DISC_PRICE_TSPECIAL;
#if !defined(USE_SINGLE_LINE_GRAPHIC)
	keyin_setcallback(keyapp_common_disc);
#endif
	keyin_setanycheck(keyapp_common_disc);
	//keyin_anycheck_on=1;
	keyin_escape_clear=1;
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
	#ifdef USE_ARAB_CONVERT
		result = keyin_ulong(KI_GRAPHIC, KI_FORM_ILNN, 0,(long*)&tPrice, cap.input_length, DSP_MSG_Y,  DSP_MSG_X+cap.input_x*char_w, 0,KI_NOPASS );
	#else
		result = keyin_ulong(KI_GRAPHIC, KI_FORM_NORMAL, decpoint,(long*)&tPrice, cap.input_length, DSP_MSG_Y,  DSP_MSG_X, 0,KI_NOPASS );
	#endif
#else
	#ifdef USE_ARAB_CONVERT
		result = keyin_ulong(KI_UNIT, KI_FORM_ILNN, 0,(long*)&tPrice, cap.input_length, DSP_MSG_Y,  DSP_MSG_X+cap.input_x*char_w, 0,KI_NOPASS );
	#else
		result = keyin_ulong(KI_UNIT, KI_FORM_NORMAL, decpoint,(long*)&tPrice, cap.input_length, DSP_MSG_Y,  DSP_MSG_X, 0,KI_NOPASS );
	#endif
#endif
	keyin_escape_clear=0;
	keyin_clearcallback();
	DspLoadFont1(old);
	if(result==KP_ENTER || result==KP_SAVE)
	{
		keyapp_common_disc(tPrice);
		Operation.transactionOK = OFF; //SG060419
#if defined(USE_GSMART_BARCODE) || defined(USE_BARCODE_DISCOUNT_RATE)
		status_scale.discountflag = 3;
#endif
	} else {
		keyapp_common_disccancel();
	}
	//sale_pluclear();
	//DEL070705	display_lcd_clear();
	//DEL070705	sale_display_discount(ON);	//clear
	memset(str, 0x20, 35);
	str[35] = 0;
	//PutNSmallString(0, 0, str, 35);
	PutString(DSP_MSG_Y, DSP_MSG_X, str, DSP_MSG_FONT_ID, DSP_MSG_MAGY, DSP_MSG_MAGX, MAX_MSG_CHAR);
	sale_display_update(0x0fff);
	
	if (result==KP_ENTER || result==KP_SAVE){
		uctemp = get_global_bparam(GLOBAL_SALE_SETUP_FLAG6); //SG060419
		if(!(uctemp&0x01))
			keyapp_print(OFF);
	}	
	
	if (result==KP_CLEAR || result==KP_ESC) {
		if ((Operation.fixedAction==2) && (Operation.fixedPrice)) {
			keyapp_fixed_price(OFF,2);
		}
	}
	KEY_SetMode(old_key);
}

#ifdef USE_TOPMART_DEFAULT
void keyapp_sale_price_change_unitprice(void)
{
	long	result;
	long	tPrice;
	char	old;
	INT8U   uctemp;
	INT8U   old_key;
	char    str[36];
	char    char_w;
	CAPTION_STRUCT	cap;
	
	if (!status_scale.cur_pluid) {
		BuzOn(2);            
		return;
	}
	
	old_key = KEY_GetMode();
	KEY_SetMode(1);
	
	//CheckZeroTotalPrice();
	Operation.specialPriceDisc = OFF;
	
	tPrice = 0;
	
	old=DspStruct.Id1;
	DspLoadFont1(DSP_MSG_FONT_ID);
	
//	caption_split_by_code(0xc01e,&cap,1);//strcpy(gmsg,"DISCOUNT (%)        [  ]");
	//PutNSmallString(0,0,global_message[MSG_DC_PERCENT],35);
	caption_split_by_code(0xc01d,&cap,0);
	sprintf(cap.form, "Change price : ");
	cap.input_length = 6;
	caption_adjust(&cap, NULL);
	PutString(DSP_MSG_Y, DSP_MSG_X, cap.form, DSP_MSG_FONT_ID, DSP_MSG_MAGY, DSP_MSG_MAGX, MAX_MSG_CHAR);
	//old=DspStruct.Id1;
	DspLoadFont1(DSP_MSG_FONT_ID);
	char_w = display_font_get_width();
	
	//key_common_disc1=DISC_PRICE_USPECIAL;
	//key_common_disc2=DISC_PRICE_TSPECIAL;
	key_common_disc1 = DISC_PRICE_TOTALTOUNIT;
	key_common_disc2 = DISC_PRICE_TOTALTOUNIT;
	keyin_setcallback(keyapp_common_disc);
	keyin_setanycheck(keyapp_common_disc);
	//keyin_anycheck_on=1;
	keyin_escape_clear=1;
	
	result = keyin_ulong(KI_GRAPHIC, KI_FORM_NORMAL, 0,(long*)&tPrice, cap.input_length, DSP_MSG_Y,  DSP_MSG_X+cap.input_x*char_w, 0,KI_NOPASS );
     
	keyin_escape_clear=0;
	keyin_clearcallback();
	DspLoadFont1(old);
	if(result==KP_ENTER || result==KP_SAVE)
	{
		keyapp_common_disc(tPrice);
		Operation.transactionOK = OFF; //SG060419
#ifdef USE_GSMART_BARCODE
		status_scale.discountflag = 3;
#endif
	} else {
		keyapp_common_disccancel();
	}
	//sale_pluclear();
//DEL070705	display_lcd_clear();
//DEL070705	sale_display_discount(ON);	//clear
	memset(str, 0x20, 35);
	str[35] = 0;
	//PutNSmallString(0, 0, str, 35);
	PutString(DSP_MSG_Y, DSP_MSG_X, str, DSP_MSG_FONT_ID, DSP_MSG_MAGY, DSP_MSG_MAGX, MAX_MSG_CHAR);
	sale_display_update(0x0fff);
	
	if (result==KP_ENTER || result==KP_SAVE)
	{
		uctemp = get_global_bparam(GLOBAL_SALE_SETUP_FLAG6); //SG060419
		if(!(uctemp&0x01)) keyapp_print(OFF);
	}	
	
	if (result==KP_CLEAR || result==KP_ESC) 
	{
		if ((Operation.fixedAction==2) && (Operation.fixedPrice)) 
		{
			keyapp_fixed_price(OFF,2);
		}
	}
	KEY_SetMode(old_key);
}
#endif
#ifdef USE_EDIT_TOTALPRICE
void keyapp_sale_edit_totalprice(void)
{
	long	result;
	long	tPrice;
	char	old;
	INT8U   uctemp;
	INT8U   old_key;
	char    str[36];
	char    char_w;
	CAPTION_STRUCT	cap;
#ifdef USE_PRICE_CHANGEABLE_CHECK
	INT16U plu_addr;
	INT16S size;
	INT8U priceChangeFlag;
	INT8U string_buf[20];
#endif


	if (!status_scale.cur_pluid) {
		BuzOn(2);
		return;
	}
	
#ifdef USE_PRICE_CHANGEABLE_CHECK
	plu_table_search((INT8U)status_scale.departmentid, status_scale.cur_pluid,&plu_addr,0);
	plu_get_field(plu_addr-1,  PTYPE_TAX_NO, (INT8U *)&priceChangeFlag,   &size,sizeof(priceChangeFlag));
	if (priceChangeFlag)
	{
		BuzOn(2);
		sprintf(string_buf,"가격변경 제한");
		display_message_page_mid_delay(string_buf,10);
		return;
	}
#endif


	old_key = KEY_GetMode();
	KEY_SetMode(1);

	Operation.specialPriceDisc = OFF;

	tPrice = 0;

	old=DspStruct.Id1;
	DspLoadFont1(DSP_MSG_FONT_ID);

	caption_split_by_code(0xc01d,&cap,0);
	sprintf(cap.form, "Change price : ");
	cap.input_length = 6;
	caption_adjust(&cap, NULL);
	PutString(DSP_MSG_Y, DSP_MSG_X, cap.form, DSP_MSG_FONT_ID, DSP_MSG_MAGY, DSP_MSG_MAGX, MAX_MSG_CHAR);
	DspLoadFont1(DSP_MSG_FONT_ID);
	char_w = display_font_get_width();

	key_common_disc1 = DISC_PRICE_EDITTOTAL;
	key_common_disc2 = DISC_PRICE_EDITTOTAL;
	keyin_setcallback(keyapp_common_disc);
	keyin_setanycheck(keyapp_common_disc);
	keyin_escape_clear=1;

	result = keyin_ulong(KI_GRAPHIC, KI_FORM_NORMAL, 0,(long*)&tPrice, cap.input_length, DSP_MSG_Y,  DSP_MSG_X+cap.input_x*char_w, 0,KI_NOPASS );

	keyin_escape_clear=0;
	keyin_clearcallback();
	DspLoadFont1(old);
	if(result==KP_ENTER || result==KP_SAVE)
	{
		keyapp_common_disc(tPrice);
		Operation.transactionOK = OFF;
#ifdef USE_GSMART_BARCODE
		status_scale.discountflag = 3;
#endif
	} else {
		keyapp_common_disccancel();
	}

	memset(str, 0x20, 35);
	str[35] = 0;
	PutString(DSP_MSG_Y, DSP_MSG_X, str, DSP_MSG_FONT_ID, DSP_MSG_MAGY, DSP_MSG_MAGX, MAX_MSG_CHAR);
	sale_display_update(0x0fff);

	if (result==KP_ENTER || result==KP_SAVE)
	{
		uctemp = get_global_bparam(GLOBAL_SALE_SETUP_FLAG6);
		if(!(uctemp&0x01)) keyapp_print(OFF);
	}

	if (result==KP_CLEAR || result==KP_ESC)
	{
		if ((Operation.fixedAction==2) && (Operation.fixedPrice))
		{
			keyapp_fixed_price(OFF,2);
		}
	}
	KEY_SetMode(old_key);
}

#endif

#if defined(USE_FIXED_PERCENT_KEY) || defined(USE_FIXED_PERCENT_KEY_SET)
void keyapp_sale_fixedpercent(long tPer)
{
	long	result;
	
	if (!status_scale.cur_pluid) {
		BuzOn(2);            
		return;
	}
	Operation.specialPriceDisc = OFF;
	key_common_disc1=DISC_PRICE_UPERCENT;
	key_common_disc2=DISC_PRICE_PERCENT;

	keyapp_common_disc(tPer);

	Operation.transactionOK = OFF; //SG060419
	sale_display_update(0x0fff);	//Discount sign
#if defined(USE_GSMART_BARCODE) || defined(USE_PRT_CHNIA_DISCOUNT_RATE)
	status_scale.discountflag = 2;
	status_scale.discount_rate = (INT8U)tPer;
#endif
}
#endif

#ifdef USE_CHN_FONT
char DisplayMsg_CantChangeType[] = {"Can't Change PLU Type"};
#else
char DisplayMsg_CantChangeType[] = {"상품종류 변경 불가"};
#endif

extern INT16S GetDiscountInfo(INT16S pluType);
extern void	sale_display_fixed_price(INT16S display);

void keyapp_fixed_price(INT16S beep,INT16S mode)	// nonfood모드로
{
	INT8U	pluType,byte;
#ifdef USE_PRICE_CHANGE_PROHIBITED
  #ifdef USE_KOREAN_ERROR_MSG
	INT8U string_buf[20];
	sprintf(string_buf,"가격변경 제한");
	display_message_page_mid_delay(string_buf,10);
  #endif	
	BuzOn(2);
	return;
#endif

#ifdef DISABLE_PLUTYPE_CHANGE
	display_message_page_mid(DisplayMsg_CantChangeType);
	return;
#endif
	if(beep)
	{
		BuzOn(1);
		if(CheckCanadianTareAction())
		{
			MsgCanadianTare();
			return;
		}
		
	}
	if(!status_scale.cur_pluid)	return;
#ifdef USE_FIXED_PLUTYPE//USE_EMART_NETWORK
	if (status_scale.Plu.disable_typechange)	// 변경불가 플래그가 있으면
	{
		//DisplayMsg(global_message[MSG_CANT_CHANGE_TYPE]);
		display_message_page_mid(DisplayMsg_CantChangeType);
		return;
	}
#endif
	if(MISC_PLU_WEIGHT <= status_scale.cur_pluid)	return;
	pluType = PluType();
	if(Operation.fixedPrice) {
		Operation.fixedPrice = 0;
		GetDiscountInfo(pluType);
		//DEL070705		MsgDisplayTime = 1;
		//DEL070705		sale_display_discount(OFF);
		//DEL070705		MsgDisplayTime = 0;
		Operation.fixedAction=0;
	} else {
		Operation.fixedPrice = 1;
		Operation.fixedAction=(INT8U)mode;
	}
	if(Startup.country==COUNTRY_KR) {	// Added by CJK 20060508 : Only Korea
		if (mode!=2) {	// 정가키를 눌렀을 때만
			if (GlbOper.saveChangeType) {
				byte = OnOff(Operation.fixedPrice);
				plu_set_field(status_scale.Plu.address-1, PTYPE_FIXED_PRICE,&byte);// Save
				if (network_status.service_flag&0x20)	// send plu
				{
					commun_net_mask(0, &status_scale.Plu.address, 0xff, 1);
				}
			}
			status_scale.Plu.fixed_price = Operation.fixedPrice;
		}
	}
	sale_display_update(UPDATE_CENTER|UPDATE_WEIGHT|UPDATE_UNIT|UPDATE_PRICE|UPDATE_TARE);
	//DEL070706	sale_display_fixed_price(OFF);
	SetMaxRide(pluType);
	SetCurRide(pluType);
}

void keyapp_type(INT16S keyFlag)
{
	INT8U	pluType;
	CAPTION_STRUCT cap;
	INT8U flagSkipSettingRide = OFF;
#ifdef USE_FIXED_PLUTYPE
	INT8U pluFixedCntType;
#endif
#ifdef USE_DIVIDING_FUNCTION
	if(status_scale.divisor) return;
#endif
	if(keyFlag)
	{
		//BuzOn(1);
#ifdef DISABLE_PLUTYPE_CHANGE
		display_message_page_mid(DisplayMsg_CantChangeType);
		return;
#endif
		
		if(CheckCanadianTareAction())
		{
			MsgCanadianTare();
			return;
		}
		//if(DiscSale.saleDiscountType == DISCOUNT_INDEX)
		if(Operation.discountInfo)
		{
			caption_split_by_code(0xC80E,&cap,0);
			DisplayMsg(cap.form);//"Discount PLU"
			return;
		}
		if(!status_scale.cur_pluid)	return;
#ifdef USE_FIXED_PLUTYPE
		if(status_scale.Plu.disable_typechange && (status_scale.Plu.ptype == PLU_BY_COUNT))
		{
			pluFixedCntType = ON;	//plu_callbykey 에서 모든 FIXED PLU TYPE을 NON FIXED 로 만들기 때문에 구분하기 위함. 
		}
		else
		{
			pluFixedCntType = OFF;
		}
		if (status_scale.Plu.disable_typechange && !pluFixedCntType)	// 변경 불가 플래그가 있으면 
		{
			//DisplayMsg(global_message[MSG_CANT_CHANGE_TYPE]);//"Discount PLU"
			display_message_page_mid(DisplayMsg_CantChangeType);
			return;
		}
#endif
		if(MISC_PLU_WEIGHT <=status_scale.cur_pluid)	return;
		//if(PluType() == PLU_FIXED)	return;// fixed price key로 풀어라
		//if(Operation.fixedPrice)	return;
		
		//if(!permission_check(PMS_KEY_TYPE_CHANGE))	return;
		DiscSale.saleDiscountType=DISCOUNT_NO;
		DiscSale.saleDiscFSP =0;
	}
	if(Operation.fixedPrice) {
		keyapp_fixed_price(OFF,0);
		ChangePluType(PLU_BY_COUNT);
	} else {
		switch(PluType())
		{
			case PLU_BY_WEIGHT:
				if(PluType() == status_scale.Plu.ptype)	// by-weight plu
				{
					//if(GlbOper.enableByCount)
					//	ChangePluType(PLU_BY_PCS);
					//else
					ChangePluType(PLU_BY_COUNT);
					status_scale.cur_qty = 1;
					status_scale.cur_pcs = 1;
				}
				else
				{
					ChangePluType(status_scale.Plu.ptype);
				}
				
				break;
#ifdef USE_FIXED_PLUTYPE
			case PLU_BY_PCS:
				if (status_scale.cur_ride < 3)	// 2:change unit count, 3:change unit price
				{
					status_scale.cur_ride++;
					flagSkipSettingRide = ON;
				}
				else ChangePluType(PLU_BY_WEIGHT);
				break;
			case PLU_BY_COUNT:
				if (status_scale.cur_ride < 2)	// 2:change unit count
				{
					status_scale.cur_ride++;
					flagSkipSettingRide = ON;
				}
				else 
				{
					if(!pluFixedCntType)
					{
						ChangePluType(PLU_BY_WEIGHT);
					}
				}
				break;
#else
			case PLU_BY_PCS:
			case PLU_BY_COUNT:
				if(PluType() == status_scale.Plu.ptype)	// by-pcs plu or by-count plu
				{
					ChangePluType(PLU_BY_WEIGHT);
				}
				else					// by-weight plu
				{
					ChangePluType(status_scale.Plu.ptype);
				}
				break;
#endif
		}
	}
	pluType = PluType();
	if (!flagSkipSettingRide)
	{
		SetMaxRide(pluType);
		SetCurRide(pluType);
	}
	GetDiscountInfo(pluType);
	sale_display_update(0xfff);
	sale_display_tare_eu();
	//strcpy(gmsg,"keyapp_type");
	//PutNSmallString(1,0,gmsg,30);
}

void keyapp_edit_sellbydate(void)
{
#ifdef _USE_LCD20848_//ck 
	Dsp_Clear();
#endif
	Dsp_ChangeMode(DSP_PGM_MODE);
	plu_edit_sellbydate(status_scale.departmentid,status_scale.cur_pluid);
	Dsp_ChangeMode(DSP_SALE_MODE);
}

// keyapp_origin()
void keyapp_edit_origin(void)
{
#ifdef _USE_LCD20848_//ck
	Dsp_Clear();
#endif
	Dsp_ChangeMode(DSP_PGM_MODE);
	plu_edit_origin((INT8U)status_scale.departmentid,status_scale.cur_pluid);
	Dsp_ChangeMode(DSP_SALE_MODE);
}

// keyapp_pluitem
void keyapp_edit_item(void)
{
#ifdef _USE_LCD20848_//ck
	Dsp_Clear();
#endif
	Dsp_ChangeMode(DSP_PGM_MODE);
	plu_edit_item((INT8U)status_scale.departmentid,status_scale.cur_pluid);
	Dsp_ChangeMode(DSP_SALE_MODE);
	keyapp_pluno((INT8U)status_scale.departmentid,status_scale.cur_pluid,OFF);	// Added by CJK 20060607
}

void keyapp_edit_price(void)
{
#ifdef _USE_LCD20848_//ck
	Dsp_Clear();
#endif
	Dsp_ChangeMode(DSP_PGM_MODE);
	plu_edit_price((INT8U)status_scale.departmentid,status_scale.cur_pluid);
	Dsp_ChangeMode(DSP_SALE_MODE);
	keyapp_pluno((INT8U)status_scale.departmentid,status_scale.cur_pluid,OFF);	// Added by CJK 20060607
}

//SG060309
void keyapp_edit_packeddate(void)
{
	POINT disp_p;
	char format[34], ret_string[20],s_date[20],s_time[20];
	INT8U date_form;		//  Yxxx xxxx  :  Y = Year format
	//  xMxx xxxx  :  M = month format
	//  xxDD xxxx	: DD = Date format
	//  xxxx xTTTT :  T = Time format
	//	INT8U key_mode, sep_date, sep_time,date_type;
	INT8U key_mode, date_type;    
	INT16S w,m1,m2,m3;
	//	INT16U  i16umax, i16umin;
	INT16U  max_id, cur_id, i,  result, line_id;
	INT16U  page_max, page_cur;
	INT16U  year,month,day;
	RTC_STRUCT time_str;
	INT8U   old_font;
	//	INT16U  cap_id[]={0x3841,0x3842};
	INT16U  cap_id[]={0x3841};
	CAPTION_STRUCT sub_cap;
	char   str[48];
	char   string_buf[50];
#ifdef USE_PERSIAN_CALENDAR
	INT16U perCentury;
#endif
	
	//	max_id   = 2;
	max_id   = 1;
	
	page_max = max_id/Startup.line_max;
	if (max_id%Startup.line_max) page_max++;
	
	key_mode=KEY_GetMode();
	KEY_SetMode(1);
	old_font=DspStruct.Id1;
	DspLoadFont1(DSP_MENU_FONT_ID);
	date_form = get_global_bparam(GLOBAL_DATETIME_MODE);
	//	sep_date  = get_global_bparam(GLOBAL_DATE_SEPARATOR);
	//	sep_time  = get_global_bparam(GLOBAL_TIME_SEPARATOR);
	
	date_type = ((date_form>>4)& 0x03);
	
	cur_id     = 0;
	page_cur   = 0;
	line_id    = 0;
	disp_p = point(0, 0);
	PAGE_VIEW:
				
				Report_GetCurTime(&time_str,1); //SG060310
	
	year  = (INT16U)time_str.year;
	month = (INT16U)time_str.month; 
	day   = (INT16U)time_str.date;
	
	switch (date_type) {
		default:
		case 0:	caption_split_by_code(0x384A, &sub_cap,0); // 0
		sprintf(s_date,"%02d.%02d.%02d",year,month,day);
		break;
		case 1:						 // 1
			caption_split_by_code(0x384B, &sub_cap,0);
			sprintf(s_date,"%02d.%02d.%02d",month,day,year);
			break;
		case 2:
		case 3:	caption_split_by_code(0x384D, &sub_cap,0);
		sprintf(s_date,"%02d.%02d.%02d",day,month,year);
		break;
	}
	sprintf(s_time,"%02d:%02d:%02d",mPackDate.hour,mPackDate.min,mPackDate.sec);
	for (i=0; i<=Startup.line_max; i++) {
		cur_id = (page_cur*Startup.line_max) + i;
		if (cur_id>=max_id) break;
		if (i==0) {
			//display_title_page_change(0x3743, page_cur+1, page_max);
			Dsp_Fill_Buffer(0);
			sprintf(string_buf, "   ");
			caption_get_name(0x3743, string_buf+3);
			menu_writetitle_str(&string_buf[3]);
			menu_writepage(1, 1);
			Dsp_Diffuse();
		}
		caption_split_by_code(cap_id[cur_id],&cap,0);
		if ((cur_id==0) || (cur_id==1)) {
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
			strcat((char *)cap.form,"[");
#else
			strcat((char *)cap.form,"  ");
#endif
			cap.input_x = strlen(cap.form);
			if (cur_id==0) strcat((char *)cap.form,s_date);
			else           strcat((char *)cap.form,s_time);
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
			strcat((char *)cap.form,"]");
			strcpy(str,cap.form);
			if (cur_id==0) {
				strcat(str,sub_cap.form);
			}
#else
			if (cur_id==0) {
				Dsp_Write_String(disp_p,sub_cap.form);
			}
			display_string_pos((DISPLAY_START_POS_WEIGHT+strlen(&string_buf[3])), (INT8U*)cap.form);
#endif
		} 
		else 
		{
			caption_adjust(&cap,NULL);
			strcpy(str,cap.form);
		}
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
		disp_p = point((i+Startup.menu_type)*Startup.menu_ygap, 0);
		Dsp_Write_String(disp_p,(HUGEDATA char *)str);
		Dsp_Diffuse();
#else
		VFD7_Diffuse();
#endif
		strcpy(format,"99.99.99");
		switch (cur_id) {
			case 0: sprintf(ret_string,"%s",s_date); break;
			case 1: sprintf(ret_string,"%s",s_time); break;
		}
		switch (cap.input_itype) {
			//			case 'N': result = keyin_ulong(KI_GRAPHIC, KI_FORM_NORMAL, 0,(long*)&ret_long,
			//							cap.input_dlength, (i+Startup.menu_type)*Startup.menu_ygap, cap.input_x*Startup.menu_xgap, 0,KI_PASS );
			//					   keyin_clearcallback();
			//				   break;
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
			case 'S': result = keyin_string(KI_FREE_MODE, KI_GRAPHIC,NULL, (HUGEDATA INT8U*)ret_string,cap.input_length,
						cap.input_dlength, (i+Startup.menu_type)*Startup.menu_ygap,cap.input_x*Startup.menu_xgap, 0, 0, KI_PASS);
				break;
			case 'D':
			case 'T': result = keyin_string(KI_IP_MODE, KI_GRAPHIC,(HUGEDATA INT8U*)format, (HUGEDATA INT8U*)ret_string,8,8,
						(i+Startup.menu_type)*Startup.menu_ygap,cap.input_x*Startup.menu_xgap,	0, 0, KI_PASS);
			break;
#else
			case 'S': result = keyin_string(KI_FREE_MODE, KI_TOTAL,NULL, (HUGEDATA INT8U*)ret_string,cap.input_length,
					cap.input_dlength, (i+Startup.menu_type)*Startup.menu_ygap,cap.input_x*Startup.menu_xgap, 0, 0, KI_PASS);
			break;
			case 'D':
			case 'T': result = keyin_string(KI_IP_MODE, KI_TOTAL,(HUGEDATA INT8U*)format, (HUGEDATA INT8U*)ret_string,8,8,
					(i+Startup.menu_type)*Startup.menu_ygap,cap.input_x*Startup.menu_xgap,	0, 0, KI_PASS);
			break;
#endif
		}
	}
	PAGE_INPUT:
				cur_id=(page_cur*Startup.line_max)+line_id;
	if (cur_id>=max_id) {
		line_id=max_id%Startup.line_max;
		cur_id =max_id-1;
	}
	caption_split_by_code(cap_id[cur_id],&cap,0);
	//	i16umax = (INT16U)cap.input_max;
	//	i16umin = (INT16U)cap.input_min;
	disp_p = point(0, 0);
	if ((cur_id==0) || (cur_id==1)) {
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
		strcat((char *)cap.form,"[");
		cap.input_x =  strlen((char *)cap.form);
#else
		strcat((char *)cap.form,"  ");
#endif
		if (cur_id==0) strcat((char *)cap.form,s_date);
		else           strcat((char *)cap.form,s_time);
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
		strcat((char *)cap.form,"]");
#else
		if (cur_id==0) {
			Dsp_Write_String(disp_p,sub_cap.form);
		}
		display_string_pos((DISPLAY_START_POS_WEIGHT+strlen(&string_buf[3])), (INT8U*)cap.form);
#endif
	} 
	else 
	{
		caption_adjust(&cap,NULL);
	}
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
	disp_p = point((line_id+Startup.menu_type)*Startup.menu_ygap, 0);
	Dsp_Write_String(disp_p,(HUGEDATA char *)cap.form);
	Dsp_Diffuse();
#else		
	VFD7_Diffuse();
#endif
	strcpy(format,"99.99.99");
	
	PAGE_KI_IN:
				switch (cur_id) {
					case 0:	sprintf(ret_string,"%s",s_date); break;
					case 1:	sprintf(ret_string,"%s",s_time); break;
				}
				switch (cap.input_itype) 
				{
					/*
					 case 'N': result = keyin_ulong(KI_GRAPHIC, 
					 KI_FORM_NORMAL, 
					 0,
					 (long*)&ret_long,
					 cap.input_dlength, 
					 (line_id+Startup.menu_type)*Startup.menu_ygap, 
					 cap.input_x*Startup.menu_xgap, 
					 0,
					 KI_NOPASS );
					 keyin_clearcallback();
					 if(result == KP_ESC)	goto RET_ESC;
					 
					 cap.input_max = i16umax;
					 cap.input_min = i16umin;
					 
					 if( input_range_check(ret_long, cap.input_min, cap.input_max) )
					 {
					 ret_long=ret_long%100;
					 if (cur_id>=2) date_temp=(INT8U)ret_long;
					 switch (cur_id) {
					 case 2: date_form &= 0xbf;
					 if (date_temp) date_form|=0x40;
					 break; // Month Format
					 case 3: date_form &= 0x7f;
					 if (date_temp) date_form|=0x80;
					 break; // Year  Format
					 case 4: date_form &= 0xf0;
					 date_form |= date_temp;
					 break;    // Time Format
					 }
					 } 
					 else 
					 {
					 error_display_form1(0, cap.input_min, cap.input_max);
					 goto PAGE_VIEW;
					 }
					 break;
					 */
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
					case 'S': result = keyin_string(KI_FREE_MODE, KI_GRAPHIC,NULL, (HUGEDATA INT8U*)ret_string,cap.input_length,
						cap.input_dlength, (line_id+Startup.menu_type)*Startup.menu_ygap,cap.input_x*Startup.menu_xgap, 0, 0, KI_NOPASS);
#else
					case 'S': result = keyin_string(KI_FREE_MODE, KI_TOTAL,NULL, (HUGEDATA INT8U*)ret_string,cap.input_length,
							cap.input_dlength, (line_id+Startup.menu_type)*Startup.menu_ygap,cap.input_x*Startup.menu_xgap, 0, 0, KI_NOPASS);
#endif
					if(result == KP_ESC)	goto RET_ESC;
					switch(cur_id)
					{
						case 5: break;//sep_date = ret_string[0]; break;
						case 6: break;//sep_time = ret_string[0]; break;
					}
					break;
					case 'D':
					case 'T':
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
						result = keyin_string(KI_IP_MODE, KI_GRAPHIC,(HUGEDATA INT8U*)format, (HUGEDATA INT8U*)ret_string,8,8,
			 					(line_id+Startup.menu_type)*Startup.menu_ygap,cap.input_x*Startup.menu_xgap, 0, 0, KI_NOPASS);
#else
						result = keyin_string(KI_IP_MODE, KI_TOTAL,(HUGEDATA INT8U*)format, (HUGEDATA INT8U*)ret_string,8,8,
								(line_id+Startup.menu_type)*Startup.menu_ygap,cap.input_x*Startup.menu_xgap, 0, 0, KI_NOPASS);
#endif
						if(result == KP_ESC)	goto RET_ESC;
						if (cap.input_itype=='T') {
							if( !(input_time_check_str((char*)&ret_string,0)) ) {
								BuzOn(2);
								goto PAGE_KI_IN;
							}
						} else {
							for(w=0; w<8; w++) ret_string[w] -= '0';
							m1 = ret_string[0]*10 + ret_string[1];
							m2 = ret_string[3]*10 + ret_string[4];
							m3 = ret_string[6]*10 + ret_string[7];
							switch (date_type) {
								case 0: year=(INT16U)m1; month=(INT16U)m2; day=(INT16U)m3;break;
								case 1: year=(INT16U)m3; month=(INT16U)m1; day=(INT16U)m2; break;
								case 2:
								case 3: year=(INT16U)m3; month=(INT16U)m2; day=(INT16U)m1; break;
							}
							mPackDate.year =(INT8U)year ;
							mPackDate.month=(INT8U)month;
							mPackDate.date =(INT8U)day  ;
							
						}
						break;
				}
				switch(result) {
					case KP_ARROW_DN:
					case KP_ENTER:
						if (cur_id<max_id-1) {
							if (line_id<Startup.line_max-1) {
								line_id++;
								goto PAGE_INPUT;
							} else {
								if (page_cur<page_max-1) {
									line_id=0;
									page_cur++;
									goto PAGE_VIEW;
								}
							}
						}
						else
						{
							if (result == KP_ENTER)
								goto RET_END;
						}
						goto PAGE_INPUT;
					case KP_PAGE_DN:
						if (page_cur<page_max-1) 
						{
							page_cur++;
							goto PAGE_VIEW;
						}
						goto PAGE_VIEW;
					case KP_ARROW_UP:
						if (line_id>0) {
							line_id--;
							goto PAGE_INPUT;
						} else {
							if (page_cur>0) {
								page_cur--;
								line_id=Startup.line_max-1;
								goto PAGE_VIEW;
							}
							else
								goto PAGE_INPUT;
						}
					case KP_PAGE_UP:
						if(page_cur>0) {
							page_cur--;
							goto PAGE_VIEW;
						}
						goto PAGE_VIEW;
					case KP_SAVE:
						goto RET_END;
					default:
						goto PAGE_KI_IN;
				}
				goto RET_ESC;
				RET_END:
						
						if(!mPackDate.deltadays){
#ifdef USE_PERSIAN_CALENDAR
							RTC_Convert_Persian_Read();
#else
							RTC_CLK_Burst_Read();
#endif
							
#ifdef USE_PERSIAN_CALENDAR
							if(RTCStruct.year < 0x5a) 	perCentury = 1400; //90년 이전: 1400년대, 90년 이후: 1300년대
							else perCentury = 1300;
#else
							year = (INT16U)(bcd2hex(RTCStruct.year) + 2000);
#endif
							month = (INT16U)bcd2hex(RTCStruct.month);
							day = (INT16U)bcd2hex(RTCStruct.date);
#ifdef USE_PERSIAN_CALENDAR
							mPackDate.deltadays = date_getdeltadays(mPackDate.year+perCentury,mPackDate.month,mPackDate.date,
									year,month,day);
#else
							mPackDate.deltadays = date_getdeltadays(mPackDate.year+2000,mPackDate.month,mPackDate.date,
									year,month,day);
#endif
							if(mPackDate.deltadays)
								display_date_change(DISPLAY_DATE_DAY,ON);		       				
						}else {
							mPackDate.deltadays=0;
						}
				
				
				RET_ESC:
#if !defined(_USE_LCD20848_) && !defined(_USE_LCD32_)
				display_string_pos_clear(DISPLAY_START_POS_WEIGHT,20);
#endif
				KEY_SetMode(key_mode);
				DspLoadFont1(old_font);
				return;
}

void keyapp_edit_packdate(void)
{
	if(mPackDate.deltadays){
		mPackDate.deltadays = 0;
		return;
	}
#ifdef _USE_LCD20848_//ck
	Dsp_Clear();
#endif
	Dsp_ChangeMode(DSP_PGM_MODE);
	keyapp_edit_packeddate();
	Dsp_ChangeMode(DSP_SALE_MODE);
#ifndef USE_TOPMART_DEFAULT //sewon 170120
	sale_pluclear(OFF);
#endif
	//	display_lcd_clear();
	sale_display_update(0x0fff);
}

//SG060227
extern void transaction_sale_store(TRANSACTION_RECORD_TYPE *pTrans, TRANSACTION_RECORD_SUB_TYPE *pTrans_sub, TR_ADDITION_DATA_TYPE* pTrans_add, UNITPRICE_CHANGE_TYPE *pChgPrice, INT8U mode, INT8U sendflag);
void keyapp_save_price(void)
{
#ifdef USE_EMART_CHANGE_DATA_AT_NON_SALE
	UNITPRICE_CHANGE_TYPE	chgprice;
	//TRANSACTION_RECORD_TYPE pTrans;
	TR_ADDITION_DATA_TYPE trans_add;
	
	//INT8U	chg_flag;
	INT8U 	Reason_code;
	INT8U	ret_value = 0;
	INT8U	changed = OFF;
	INT8U	changeFlag;
#endif
	
	//chg_flag = get_global_bparam(GLOBAL_SALE_SETUP_FLAG8) & 0x40;	//parameter 723
	
	//	char tmpStr[64];
	//	sprintf(tmpStr,"SGTest\r\n");
	//	MsgOut(tmpStr);
#ifdef USE_PRICE_CHANGE_PROHIBITED
  #ifdef USE_KOREAN_ERROR_MSG
		INT8U string_buf[20];
		sprintf(string_buf,"가격변경 제한");
		display_message_page_mid_delay(string_buf,10);
  #endif
		BuzOn(2);
		return;
#endif
	//	if(GlbOper.saveChangePrice && status_scale.cur_pluid<MISC_PLU_WEIGHT)
	if(status_scale.cur_pluid && status_scale.cur_pluid<MISC_PLU_WEIGHT)
	{
		if(Price.UnitPrice[PRICE_ORIGIN] != status_scale.Plu.unitprice)
		{
#ifdef USE_EMART_CHANGE_DATA_AT_NON_SALE
			if (UseEmartNetwork)
			{
				ret_value = GetCheckAddTransactionData(0,1,AUDIT_MODE_CIRCULAR);		//mode : sale, count : 1, Circular
				if (ret_value);
				else return; 
			}
#endif
			
#ifdef USE_EMART_CHANGE_DATA_AT_NON_SALE			
			changed = OFF;
#endif
			if(Operation.saveZeroOverride)// zero 일때도 save
			{
				if(Price.UnitPrice[PRICE_ORIGIN])
				{
					save_override_unit_price();
#ifdef USE_EMART_CHANGE_DATA_AT_NON_SALE
					changed = ON;
#endif
				}
			}
			else
			{
				if(status_scale.Plu.unitprice)
				{
					save_override_unit_price();
#ifdef USE_EMART_CHANGE_DATA_AT_NON_SALE
					changed = ON;
#endif
				}
			}
#ifdef USE_EMART_CHANGE_DATA_AT_NON_SALE
			if (ret_value && changed)
			{
				Reason_code = 0;	//Fix
				changeFlag = GetChangeFlag(status_scale.Plu.ptype, status_scale.Plu.ptype, Price.UnitPrice[PRICE_ORIGIN], status_scale.Plu.unitprice, 0, 0, 0);	// plu type change는 무시
				InsertAdditionalData(&trans_add);
				InsertUnitPriceChangeRecord(status_scale.scaleid,status_scale.departmentid,status_scale.cur_pluid,
						status_scale.Plu.unitprice,Price.UnitPrice[PRICE_ORIGIN],Reason_code,status_scale.clerkid,changeFlag,&chgprice);
				transaction_sale_store(NULL, NULL, &trans_add, &chgprice, 0, 1);
			}
#endif
		}
		// change unit price
	}
}

void keyapp_edit_discount(void)
{
	if(!status_scale.cur_pluid)	return;
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
	Dsp_Clear();
#endif
	Dsp_ChangeMode(DSP_PGM_MODE);
	discount_edit_item((INT8U)status_scale.departmentid,status_scale.cur_pluid);
	Dsp_ChangeMode(DSP_SALE_MODE);
	keyapp_pluno((INT8U)status_scale.departmentid,status_scale.cur_pluid,OFF);	// Added by CJK 20060607
}

void keyapp_edit_delplu(void)
{
	//BuzOn(1);
	plu_edit_delete((INT8U)status_scale.departmentid,status_scale.cur_pluid);
}

void keyapp_edit_ingredient(void)
{
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
	Dsp_Clear();
#endif
	Dsp_ChangeMode(DSP_PGM_MODE);
	plu_edit_ingredient((INT8U)status_scale.departmentid,status_scale.cur_pluid);
	Dsp_ChangeMode(DSP_SALE_MODE);
}

void keyapp_edit_date_flag(void)
{
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
	Dsp_Clear();
#endif
	Dsp_ChangeMode(DSP_PGM_MODE);
	plu_edit_date_flag((INT8U)status_scale.departmentid,status_scale.cur_pluid);
	Dsp_ChangeMode(DSP_SALE_MODE);
}

void keyapp_chess_print(void)
{
	printer_chess();
}

void keyapp_test_print(void)
{
#ifdef _USE_LCD20848_//ck
	Dsp_Clear();
#endif
	Dsp_ChangeMode(DSP_PGM_MODE);
	printer_test();
	Dsp_ChangeMode(DSP_SALE_MODE);
}

void keyapp_edit_speedkey(void)
{
	Dsp_ChangeMode(DSP_PGM_MODE);
	plu_edit_speedkey(status_scale.cur_pluid);
	Dsp_ChangeMode(DSP_SALE_MODE);
}

void keyapp_edit_tax(void)	//Added by JJANG 20070619
{
#ifdef _USE_LCD20848_//ck
	Dsp_Clear();
#endif
#ifdef USE_TAX_RATE
	Dsp_ChangeMode(DSP_PGM_MODE);
	plu_edit_tax(&Price,(INT8U)status_scale.departmentid,status_scale.cur_pluid);
	Dsp_ChangeMode(DSP_SALE_MODE);
#endif
}

void keyapp_apply_global_tax(void)
{
#ifdef USE_TAX_RATE
	plu_apply_global_tax(&Price,(INT8U)status_scale.departmentid,status_scale.cur_pluid);
#endif
}



#ifdef USE_TRACEABILITY
void keyapp_edit_trace(void)		//insert by se-hyung for function traceability key 20070706
{
#ifdef _USE_LCD20848_//ck
	Dsp_Clear();
#endif
	Dsp_ChangeMode(DSP_PGM_MODE);
	plu_edit_traceability((INT8U)status_scale.departmentid,status_scale.cur_pluid);
	Dsp_ChangeMode(DSP_SALE_MODE);
}
#endif
extern PLU_BATCH_TYPE	Batch[MAX_PLU_BATCH];
extern INT8U	BatchCount,BatchIndex;
void keyapp_batch(INT8U beep)
{
	
	JOB_BATCH_STRUCT  batch_str;
	INT32U  addr_flash, addr;
	INT16U  read_code;
	INT16S   i;
	
	if(beep)
		BuzOn(1);
	if(status_scale.cur_pluid) {
		sale_pluclear(ON);
		sale_display_update(0x0fff);//061128
	}
	
	addr_flash =  DFLASH_BASE + FLASH_JOBBATCH_AREA;	
	
	BatchCount =0;
	BatchIndex =0;
	for(i=0; i<10; i++)	
	{
		addr =  addr_flash + JOB_BATCH_STRUCT_SIZE * i;
		read_code = Flash_wread(addr);
		if(read_code == (i+1))  
		{	
			Flash_sread(addr, (HUGEDATA INT8U*)&batch_str, JOB_BATCH_STRUCT_SIZE); 
			Batch[BatchIndex].dept = batch_str.deptNO;
			Batch[BatchIndex].plu =  batch_str.pluNO;
			Batch[BatchIndex].printCount = batch_str.quantity;
			BatchCount++;
			BatchIndex++;			
			
		}
	}
	BatchIndex = 0;
	
}

void keyapp_set_scan_clerk(void) //Added by JJANG 20071022 (function key #125)
{
	CAPTION_STRUCT 	cap;
	POINT 		disp_p;
	
	INT16U  result;
	long ret_long;
	
	if(status_scale.cur_pluid) return;	//PLU가 호출된 상태에서는 동작안함
	
	KEY_SetMode(1);		
#ifdef _USE_LCD20848_//ck
	Dsp_ChangeMode(DSP_PGM_MODE);
	display_lcd_clear_buf();
	Dsp_Clear();
	DspLoadFont1(DSP_MENU_FONT_ID);
#elif defined (_USE_LCD32_)
	Dsp_ChangeMode(DSP_PGM_MODE);
	display_lcd_clear_buf();
	DspLoadFont1(DSP_MENU_FONT_ID);
#else
	//	Dsp_ChangeMode(DSP_PGM_MODE);
	ClearDisp7();
#endif
	caption_split_by_code(0x1f01, &cap,0);
	caption_adjust(&cap,NULL);
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
  #ifdef USE_ARAB_CONVERT
    disp_p = point((Startup.menu_type)*Startup.menu_ygap,0);
    Convert_ArabPreview((INT8U *)cap.form, strlen(cap.form), 0, 0, 0); 
    arab_codetemp[strlen(cap.form)] = 0;
    disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
    Dsp_Write_1stringLong(disp_p, arab_codetemp);
  #else
	disp_p = point((Startup.menu_type)*Startup.menu_ygap,0);
	Dsp_Write_String(disp_p,(HUGEDATA char*)cap.form);
	Dsp_Diffuse();
  #endif
#else
	display_string(DISPLAY_UNITPRICE, (INT8U*)cap.form);
	VFD7_Diffuse();
#endif
	ret_long = 0;
	if (Scan_clerk) ret_long = Scan_clerk;
	
KI_IN:
#ifdef _USE_LCD32_
	keyin_setanycheck(keyapp_callback_salemode);
#endif
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
	result=keyin_ulong(KI_GRAPHIC, KI_FORM_NORMAL,  0,(long*)&ret_long, cap.input_dlength,
				(Startup.menu_type)*Startup.menu_ygap,  cap.input_x*Startup.menu_xgap, 0,KI_NOPASS );
#else
	result=keyin_ulong(KI_TOTAL, KI_FORM_NORMAL,  0,(long*)&ret_long, cap.input_dlength,
				(Startup.menu_type)*Startup.menu_ygap,  cap.input_x*Startup.menu_xgap, 0,KI_NOPASS );
#endif
#ifdef _USE_LCD32_
	keyin_clearcallback();
#endif
	
	if(result == KP_ESC) goto RET_ESC;
	cap.input_min = 0;
	if( !input_range_check(ret_long, cap.input_min, cap.input_max)) 
	{
		error_display_form1(0, cap.input_min, cap.input_max);
		goto KI_IN;
	} 
	else Scan_clerk = ret_long;
	switch(result) 
	{
		case KP_ENTER:
		case KP_SAVE:
			goto RET_END;
		default:
			BuzOn(2);
			goto KI_IN;
	}
	RET_END:
			Scan_clerk = ret_long;
	RET_ESC:
			//Dsp_Fill_Buffer(0);		// LCD Buffer clear
			//Dsp_Diffuse();
			//	display_lcd_clear();    
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
			Dsp_ChangeMode(DSP_SALE_MODE);
#else
			ClearDisp7();
#endif
	KEY_SetMode(0);
	sale_display_update(UPDATE_ALL|UPDATE_CLEARALL);
	return;
}
void kayapp_input_barcode(void)	 //Added by JJANG 20071022 (function key #126)
{
	CAPTION_STRUCT 	cap;
	POINT 		disp_p;
	
	INT16U  result;
	INT8U 	len,i;
	char 	ret_string[33];
#ifdef USE_TRACE_STANDALONE
	INT8U  bsIndex = 0, bsLeng = 0;
	if ((get_global_bparam(GLOBAL_SCANNER_FLAG1)&0x07) != 3)
	{
		if(status_scale.cur_pluid) return; //GS마트인 경우, 상품호출된 후에도 바코드 입력기능 허용함
	}
#else
	if(status_scale.cur_pluid) return;
#endif
#ifdef _USE_LCD20848_//ck
	Dsp_Clear();
#endif
	KEY_SetMode(1);
	Dsp_ChangeMode(DSP_PGM_MODE);
	
	display_lcd_clear_buf();
	
	DspLoadFont1(DSP_MENU_FONT_ID);
#if !defined(_USE_LCD20848_) && !defined(_USE_LCD32_)
	display_string_pos_clear(DISPLAY_START_POS_WEIGHT, 20);
#endif
	caption_split_by_code(0x146D, &cap,0);
	caption_adjust(&cap,NULL);
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
  #ifdef USE_ARAB_CONVERT
    disp_p = point((Startup.menu_type)*Startup.menu_ygap,0);
    Convert_ArabPreview((INT8U *)cap.form, strlen(cap.form), 0, 0, 0); 
    arab_codetemp[strlen(cap.form)] = 0;
    disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
    Dsp_Write_1stringLong(disp_p, arab_codetemp);
  #else
	disp_p = point((Startup.menu_type)*Startup.menu_ygap,0);
	Dsp_Write_String(disp_p,(HUGEDATA char*)cap.form);
	Dsp_Diffuse();
  #endif
#else
	display_string_pos(DISPLAY_START_POS_WEIGHT, (INT8U*)cap.form);
	VFD7_Diffuse();
#endif
	
	memset(ret_string, 0, sizeof(ret_string));	//초기화
	KI_IN:
			//keyin_setanycheck(keyapp_callback_salemode);
			//숫자만 입력 가능
			result = keyin_string(KI_SNUMBER, KI_GRAPHIC,0, (HUGEDATA INT8U*)ret_string, cap.input_length,
			cap.input_dlength, (Startup.menu_type)*Startup.menu_ygap, cap.input_x*Startup.menu_xgap, 0, 0, KI_NOPASS);
	//keyin_clearcallback();
	
	switch(result) 
	{
		case KP_ESC:
			goto RET_ESC; 
		case KP_ENTER:
		case KP_SAVE:
			goto RET_END;
		default:
			BuzOn(2);
			goto KI_IN;
	}
	RET_END:
			len=strlen(ret_string);	//Write Barcode
	// Deleted by CJK 090611 : 착한고기 무시
	////Added by JJANG 20081212 개체키 입력 시 임의로 바코드 앞에 '0'을 붙인다. 글자잘림 현상 방어
	//#ifdef USE_TRACE_STANDALONE
	//	if ((get_global_bparam(GLOBAL_SCANNER_FLAG1)&0x07) == 3)
	//	{
	//		getScannerMappingSymbolInfo(&bsIndex,&bsLeng,'X',0,0); //'X' : scan code
	//		for(i=0;i<bsIndex-1;i++)
	//		{
	//			PS2_Write('0'-0x30+KEY_NUM_0);
	//		}		
	//	}
	//#endif
	
	for(i=0;i<len;i++)
	{
		PS2_Write(ret_string[i]-0x30+KEY_NUM_0);
	}
	PS2_Write(KS_SCANPRINT);
RET_ESC:
#if !defined(_USE_LCD20848_) && !defined(_USE_LCD32_)
	display_string_pos_clear(DISPLAY_START_POS_WEIGHT, 20);
#endif
	Dsp_ChangeMode(DSP_SALE_MODE);
	KEY_SetMode(0);
	sale_display_update(UPDATE_ALL|UPDATE_CLEARALL);
	return;
}

//Added by JJANG 090819 (function key #138)
void kayapp_input_customer_no(void)
{
	CAPTION_STRUCT cap;
	INT8U	result;
	INT8U	old_font;
	INT8U	key_mode;
	INT32U	res;
	INT16U	customer_no = 0;
	INT16U	clerkid;
	STRUCT_TOTAL_PRICE totalData;
	
	key_mode=KEY_GetMode();
	KEY_SetMode(1);
	old_font=DspStruct.Id1;
	DspLoadFont1(DSP_MENU_FONT_ID);
	
	clerkid = status_scale.clerkid;
	ClerkTotalPrice(0, clerkid, &totalData);
	
	if (Operation.operationClerk == OPER_FLOATING)
	{
		if(ethernet_list.status==2) 
		{
			totalData.TotalCount = ClerkSaleAmount.totalCount;
			totalData.VoidCount =  ClerkSaleAmount.voidCount;
		}
		if (totalData.TotalCount || totalData.VoidCount)
		{
			BuzOn(2);
			caption_split_by_code(0xCA07, &cap, 0);	//CLERK IS OPENED!
			DisplayMsg(cap.form);
			goto RET_END;
		}
	}
	
	customer_no = get_nvram_wparam(NVRAM_CLERK_CUSTOMER_NO + clerkid*2);
	res = customer_no;
	
	Menu_Init();
	caption_split_by_code(0x3421, &cap,0);
	cap.input_dlength = 4;
	cap.input_length  = 4;
	caption_adjust(&cap,NULL);
	cap.input_max = 9999;
	Menu_InputCreate(1, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
			cap.input_max, cap.input_min, 0, &res, NULL, NULL, NULL);
	
	result = Menu_InputDisplay();
	if (result == MENU_RET_SAVE) 
	{
		if (Operation.operationClerk == OPER_FLOATING)
		{
			if (!customer_no) SetClerkCustomerNo(clerkid, (INT16U)res);
			else 
			{
				if (customer_no != (INT16U)res) BuzOn(2); 
			}
		}
		else //label mode
		{
			if (!customer_no) set_nvram_wparam(NVRAM_CLERK_CUSTOMER_NO + clerkid*2, (INT16U)res);
			else 
			{
				if (customer_no != (INT16U)res) BuzOn(2); 
			}
		}
	}
	RET_END:
			DspLoadFont1(old_font);
	KEY_SetMode(key_mode);
	sale_display_update(UPDATE_ALL|UPDATE_CLEARALL);
	return;
}

void kayapp_customer_close(void)
{
	INT16U	customer_no = 0;
	
	customer_no = get_nvram_wparam(NVRAM_CLERK_CUSTOMER_NO);
	
	if (!customer_no || Operation.operationClerk == OPER_FLOATING)
	{
		BuzOn(2);
		return;
	}
	else set_nvram_wparam(NVRAM_CLERK_CUSTOMER_NO, 0); 
}

//void kayapp_close_ticket(void)
//{
//	if(!status_scale.clerkid || Operation.operationClerk!=OPER_FLOATING)
//	{
//			BuzOn(2);
//			return;
//	}
//	
//	set_nvram_bparam(NVRAM_SLAVECHECK_FLAG, 1); 
//	while(!network_ClerkDeleteTransaction(status_scale.clerkid));
//	set_nvram_bparam(NVRAM_SLAVECHECK_FLAG, 0); 
//
//	ClerkDeleteTransaction(status_scale.clerkid);

//}

static INT32U lastTicketNumberEachClerk[MAX_CLERK_NO] = {0, };				// 해당 저울에서 클럭 별 마지막으로 Subtotal한 Ticket의 Number를 기억하기 위한 배열
INT8U checkLastTicketNumber(INT16U clerkid)
{
	INT8U ret = 0;
	INT32U tempTicketNumber = 0;
	INT8U tempMSG[50] = {0, };

	tempTicketNumber = get_TicketTransactionNumber(clerkid);				// Copy Ticket을 수행하려 하는 클럭의 마지막 
	if( lastTicketNumberEachClerk[clerkid] != tempTicketNumber )			// Ticket 넘버가 지금 저울에 저장된 번호와 일치하지 않으면 
	{
		sprintf( tempMSG,"Check Scale" );									// 오류 메세지를 화면에 표시하고
		DisplayMsg(tempMSG);												// 
		sale_display_update(UPDATE_ALL|UPDATE_CLEARALL);					// Copy Ticket을 수행하지 않고 해당 기능 종료
		BuzOnAdd(3);														// 
		flagcopy_ticket = OFF;												// 이유: Floating Clerk 모드에서 Subtotal시 적용된 
		ret = FALSE;														// 사항(할인 페이정보 등,,,)에 대해서는 Sync하지 않아 해당 저울에서만 Copy Ticket 하도록 제한
	}
	else
	{
		ret = TRUE;
	}
	
	return ret;
}

#define MAX_REOPEN_NUM	7
static INT32U issuedTicketNumberList[MAX_REOPEN_NUM] = {0, };
/**
********************************************************************************
* @brief       리오픈 요청이 들어왔을 때 해당 저울에서 발행한 티켓인지를 체크 하는 함수 
* @param     inputTicketNum: Reopen을 요청한 Ticket Num
* @return     TRUE(1): 해당저울에서 발행한 Ticket( Reopen 정상진행)
*			   FALSE(0): 해당 저울에서 발행하지 않은 Ticket에 대한 Reopen을 요청한 경우 err 처리 
* @remark    1) Ticket을 발행한 저울에서만 해당 Ticket을 Copy Ticket 할 수 있게 하기 위해 
*			   Subtotal 발행 시 lastTicketNumberEachClerk 배열에 마지막으로 발행한 티켓 번호 저장 
*			   2) 현재 저울에서 발행한 Ticket에 한해서만 Reopen을 가능하게 하기 위해 issuedTicketNumberList 배열에 티켓 번호 저장 
********************************************************************************
*/
INT8U checkReopneTicketNumber(INT32U inputTicketNum)
{
	INT8U ret = 0;
	INT8U tempMSG[50] = {0, };
	INT8U i = 0;
	
	for( i=0; i<MAX_REOPEN_NUM; i++ )
	{
		if( issuedTicketNumberList[i] == inputTicketNum )					// 해당 저울에서 발행한 Ticekt 넘버를 조사하여 
		{
			ret = TRUE;														// Reopen을 요청한 티켓이 해당 저울에서 발행한 경우 
			break;															// 정상적인 기존의 Reopen 절차를 수행 
		}
	}

	if( ret == 0 )
	{
		sprintf( tempMSG,"Check Scale" );									// 오류 메세지를 화면에 표시하고
		DisplayMsg(tempMSG);												// 
		sale_display_update(UPDATE_ALL|UPDATE_CLEARALL);					// Copy Ticket을 수행하지 않고 해당 기능 종료
		BuzOnAdd(3);														// 
		ret = FALSE;
	}
	
	return ret;
}

/**
********************************************************************************
* @brief       저울에서 발행한 영수증의 Ticket 넘버를 저장하는 함수 
* @param     clerkid: 지금 발행을 시도한 Clerk ID 
* @return     none
* @remark    1) Ticket을 발행한 저울에서만 해당 Ticket을 Copy Ticket 할 수 있게 하기 위해 
*			   Subtotal 발행 시 lastTicketNumberEachClerk 배열에 마지막으로 발행한 티켓 번호 저장 
*			   2) 현재 저울에서 발행한 Ticket에 한해서만 Reopen을 가능하게 하기 위해 issuedTicketNumberList 배열에 티켓 번호 저장 
********************************************************************************
*/
void setLastTicketNumber(INT16U clerkid)					// Subtotal 시 호출 되며 
{
	INT32U tempTicketNumber = 0;
	static INT8U idx = 0; 

	tempTicketNumber = get_TicketTransactionNumber(clerkid);				// 현재 발행하는 티켓넘버를 로딩 
	lastTicketNumberEachClerk[clerkid] = tempTicketNumber;					// lastTicketNumberEachClerk 배열에 저장 된 값을 갱신

	
	if( idx < MAX_REOPEN_NUM )
	{
		issuedTicketNumberList[idx] = tempTicketNumber;						// 해당 저울에서 발행한 티켓넘버를 저장해서
		idx++;																// Reopen 할 수 있는 목록을 갱신 (issuedTicketNumberList)
	}
	else
	{
		idx = 0;
		issuedTicketNumberList[idx] = tempTicketNumber;
	}
}

void keyapp_copy_ticket(void)
{
	INT16U	clerkid;
	STRUCT_TOTAL_PRICE totalData;
	
	if(Operation.operationClerk != OPER_FLOATING) 
	{
		flagcopy_ticket = OFF;												// keyapp_total에서 해당 플래그를 사용하기 때문에 함수 종료 전 플래그 값을 OFF
		return;
	}
	clerkid = status_scale.clerkid;
	NetClerkLock(clerkid);
	ClerkTotalPrice(0, clerkid, &totalData);	
	if(ethernet_list.status==2) 
	{
		totalData.TotalCount = ClerkSaleAmount.totalCount;
		totalData.VoidCount =  ClerkSaleAmount.voidCount;
	}
	if (totalData.TotalCount || totalData.VoidCount)
	{
		BuzOn(2);
		caption_split_by_code(0xCA07, &cap, 0);	//CLERK IS OPENED!
		DisplayMsg(cap.form);
		sale_display_update(UPDATE_ALL|UPDATE_CLEARALL);
		flagcopy_ticket = OFF;												// keyapp_total에서 해당 플래그를 사용하기 때문에 함수 종료 전 플래그 값을 OFF
		return;
	}

	if ( checkLastTicketNumber(clerkid) == FALSE )							// Copy Ticket을 요청한 Clerk의 Ticket이 해당 저울에서 발행되었는지를 체크 
		return;
	
	if (ethernet_list.status == 2)	//slave
	{
		network_reopen_start_send(0, clerkid, ON);			// TicketNum 0 이면 COPY TICKET
	}
	else
	{
		GetClerkReopenTransaction(0, clerkid);
	}
	sale_display_update(UPDATE_ALL|UPDATE_CLEARALL);
	keyapp_total(1);	//total direct print
	return;
}

//Added by JJANG 090908 (function key #140)
void kayapp_reopen_ticket(void)
{
	INT8U	old_font;
	INT8U	key_mode;
	INT8U	result;
	INT32U	res = 0;
	INT16U	clerkid;
	INT8U   error;
	//	INT8U	ret = 0;
	INT16U	customer_no = 0;
	char string_buf[50];
	STRUCT_TOTAL_PRICE totalData;
	
	if(Operation.operationClerk != OPER_FLOATING) return;
	
	key_mode=KEY_GetMode();
	KEY_SetMode(1);
	old_font=DspStruct.Id1;
	DspLoadFont1(DSP_MENU_FONT_ID);
	
	clerkid = status_scale.clerkid;
	ClerkTotalPrice(0, clerkid, &totalData);
	
	if(ethernet_list.status==2) 
	{
		totalData.TotalCount = ClerkSaleAmount.totalCount;
		totalData.VoidCount =  ClerkSaleAmount.voidCount;
	}
	
	if (totalData.TotalCount || totalData.VoidCount)
	{
		BuzOn(2);
		caption_split_by_code(0xCA07, &cap, 0);	//CLERK IS OPENED!
		DisplayMsg(cap.form);
		goto RET_END;
	}
	
	customer_no = get_nvram_wparam(NVRAM_CLERK_CUSTOMER_NO + clerkid*2);
	if (customer_no)
	{
		error = 4;
		caption_split_by_code(0xCA08, &cap, 0);	//CAN NOT REOPEN! (04:CUSTOMER NUMBER IS EXIST!)
#ifdef HEBREW_FONT
		sprintf(string_buf, "%s )%d(", cap.form, error);
#else
		sprintf(string_buf, "%s (%d)", cap.form, error);
#endif
		DisplayMsg(string_buf);
		goto RET_END;
	}
	
	
	Menu_Init();
	caption_split_by_code(0x3745, &cap, 0);
	caption_adjust(&cap,NULL);
	
	//	Menu_InputCreate(1, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
	//				cap.input_max, cap.input_min, 0, &res, NULL, NULL, NULL);
	Menu_InputCreate(1, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
			cap.input_max, 0, 0, &res, NULL, NULL, NULL);
	
	result = Menu_InputDisplay();
	if (result == MENU_RET_SAVE) 
	{
		if ( checkReopneTicketNumber(res) == FALSE )							// Reopen 을 요청한 Clerk의 Ticket이 해당 저울에서 발행되었는지를 체크 
		{
			goto RET_END;
		}
		
		if (ethernet_list.status == 2)	//slave
		{
			network_reopen_start_send(res, clerkid, OFF);			
		}
		else
		{
			error = GetClerkReopenTransaction(res, clerkid);
			if (error)
			{
				caption_split_by_code(0xCA08, &cap, 0);	//CAN NOT REOPEN!
#ifdef HEBREW_FONT
				sprintf(string_buf, "%s )%d(", cap.form, error);
#else
				sprintf(string_buf, "%s (%d)", cap.form, error);
#endif
				DisplayMsg(string_buf);
			}
			else
			{
				caption_split_by_code(0xCA09, &cap, 0);	//REOPEN OK!
				DisplayMsg(cap.form);
#if defined(USE_INDIA_FUNCTION) || defined(USE_SEND_TICKET_DATA)
				// Ticket Reopne 성공
				reopenCheck = ON;
#endif
			}
		}
	}
	RET_END:
			DspLoadFont1(old_font);
	KEY_SetMode(key_mode);
	sale_display_update(UPDATE_ALL|UPDATE_CLEARALL);
	return;
}

void keyapp_g_total()
{
	long	tPrice;
	INT16S	tCount;
	long	tWeight;
	INT16S	tQty;
	long	tNetWeight;
	long	tPerTWeight;
	long	tGrossWeight;
	char	priceBuf[15],tpriceStr[20];
	INT16U	tempKey;
	INT16U	clerkNum = status_scale.clerkid;
	INT8U result;
	CAPTION_STRUCT 	cap;
	PREPACK_STATUS_TYPE PrepackStatus;
	
	PutXStringOffsetX = DSP_SALE_INFO_AREA_X1;
	PutXStringOffsetY = DSP_SALE_INFO_AREA_Y1;
	//BuzOn(1);
	tPrice = get_nvram_lparam(NVRAM_GTOTAL_PRICE+clerkNum*4);
	tCount = get_nvram_wparam(NVRAM_GTOTAL_COUNT+clerkNum*2);
	tWeight = get_nvram_lparam(NVRAM_GTOTAL_WEIGHT+clerkNum*4);
	tQty = get_nvram_wparam(NVRAM_GTOTAL_QTY+clerkNum*2);
	
	tNetWeight = 0;
	tPerTWeight = 0;
	tGrossWeight = 0;
	if (!clerkNum)
	{
		tNetWeight = get_nvram_lparam(NVRAM_GTOTAL_NET_WEIGHT);
		tPerTWeight = get_nvram_lparam(NVRAM_GTOTAL_PER_TARE_WEIGHT);
		tGrossWeight = get_nvram_lparam(NVRAM_GTOTAL_GROSS_WEIGHT);
	}
	if (!tCount) {
		caption_split_by_code(0xc903,&cap,0);//"No grand total price"
		DisplayMsg(cap.form);
		Key_DelaySec(5,0);
		return;
	}
	ClearDisp7();
	display_lcd_clear();
	FloatToString(TO_STR,priceBuf,8,DISPLAY_PRICE,&tPrice,0,1);
	caption_split_by_code(0xc01f,&cap,0);//"G Total %s"
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
	cap.form[13] = 0;
	strcat(cap.form," %s");
	GetPriceString(tpriceStr,priceBuf,ON,PRICE_PRINTER);
	sprintf(gmsg,cap.form,tpriceStr);
	PutNMediumString(1, 0,gmsg, 25);
#else
	display_string_pos(10, (INT8U*)cap.form);
	display_string(DISPLAY_PRICE, (INT8U*)priceBuf);
	VFD7_Diffuse();
#endif
	tempKey = KeyWait(0,0,0,0,0);
	
	if(tempKey==KP_SAVE || tempKey==KP_ENTER || tempKey==KS_CLEAR || tempKey==KS_EXT_PRINT)
	{
		//DisplayMsg("PRINT GRAND TOTAL");
		if(tempKey!=KS_CLEAR)
		{
			result = PrintGrandTotal(clerkNum, tCount, tPrice, tWeight, tQty, tNetWeight, tPerTWeight, tGrossWeight);
		}
		else
			result = ON;
		if(result == ON) {	// Added by CJK 20040907
			if(tempKey!=KP_CLEAR)	Key_DelaySec(20,0);
			set_nvram_lparam(NVRAM_GTOTAL_PRICE+clerkNum*4, 0);
			set_nvram_wparam(NVRAM_GTOTAL_COUNT+clerkNum*2, 0);
			set_nvram_lparam(NVRAM_GTOTAL_WEIGHT+clerkNum*4, 0);
			set_nvram_wparam(NVRAM_GTOTAL_QTY+clerkNum*2, 0);
			if (!clerkNum)
			{
				set_nvram_lparam(NVRAM_GTOTAL_NET_WEIGHT, 0);
				set_nvram_lparam(NVRAM_GTOTAL_PER_TARE_WEIGHT, 0);
				set_nvram_lparam(NVRAM_GTOTAL_GROSS_WEIGHT, 0);
			}
			get_PrepackStatus(&PrepackStatus);
			PrepackStatus.OneItem_Grand = ON;
			PrepackStatus.PrevSubPluNo = 0;
			set_PrepackStatus(&PrepackStatus);
		}
	}
	Operation.salePluClearWait = ON;
	Operation.directClearWeight = ON;
#if !defined(_USE_LCD20848_) && !defined(_USE_LCD32_)
	display_string_pos_clear(5,20);
#endif
	PutXStringOffsetX = 0;
	PutXStringOffsetY = 0;
#if defined(USE_ARAB_FONT) || defined(HEBREW_FONT)
#ifndef _USE_LCD37D_40DOT_
	sale_pluclear(ON);
	sale_display_update(UPDATE_ALL|UPDATE_CLEARALL);
#endif
#endif
}

void keyapp_cash_open(INT16S beepFlag)
{
	INT16U temp;
	INT8U scaleNum;
	
	scaleNum = status_scale.scaleid;
	if (scaleNum > MAX_SCALE - 1) scaleNum = MAX_SCALE - 1;	// scale no 32~255는 scale 31로 정산함
	
	// input clerk number or password
	if(!Operation.openCash)
	{
		Cash_open();
		if(beepFlag)
		{
			//scale
			temp = get_nvram_wparam(NVRAM_CASH_SCALE+scaleNum*4);	// period 1
			temp++;
			set_nvram_wparam(NVRAM_CASH_SCALE+scaleNum*4, temp);
#ifndef DISABLE_X2Z2_REPORT
			temp = get_nvram_wparam(NVRAM_CASH_SCALE+scaleNum*4+2);	// period 2
			temp++;
			set_nvram_wparam(NVRAM_CASH_SCALE+scaleNum*4+2, temp);
#endif
			
			//clerk
			temp = get_nvram_wparam(NVRAM_CASH_CLERK+status_scale.clerkid*4);	// period 1
			temp++;
			set_nvram_wparam(NVRAM_CASH_CLERK+status_scale.clerkid*4, temp);
#ifndef DISABLE_X2Z2_REPORT
			temp = get_nvram_wparam(NVRAM_CASH_CLERK+status_scale.clerkid*4+2);	// period 2
			temp++;
			set_nvram_wparam(NVRAM_CASH_CLERK+status_scale.clerkid*4+2, temp);
#endif
		}
	}
}


void keyapp_pay(INT16S beep)
{
	//	INT16S print=OFF;
	//	char    old_font;
	INT8U	lastClerk=0;
	STRUCT_TOTAL_PRICE totalData;
	INT16S	ret;
#ifdef USE_TAX_RATE
	long priceInExtTax = 0;
#endif
	
	//if(beep) BuzOn(1);
#ifdef USE_INDIA_FUNCTION
	if (UseAreaScaleMode == 2) // 0:NormalScale(STD), 1:DualModeScale, 2:AreaScale
	{
		BuzOn(2);
		return;
	}
#endif
	
	lastClerk = status_scale.clerkid;
	
	//memset(&totalData, 0, sizeof(STRUCT_TOTAL_PRICE));
	//SG060706
	//        if(ethernet_list.status == 2) 
	//        {
	//		if( NetworkGetTransactionData() != 1 )  return;
	//	}
	
	ret = NetClerkLock(lastClerk);
	if (ret == 3) goto ESC_PAY;	// timeout
	else if (ret == 0) return;	// clerk lock
	
	//	if( !NetClerkLock(lastClerk)) 	goto ESC_PAY;		//Modified by JJANG 20080716
	//	if( NetClerkLock(lastClerk) ) //SG061129
	//	{
	
	if(lastClerk && !get_nvram_lparam(NVRAM_TRANS_HEAD+lastClerk*4)) {
		if(ethernet_list.status != 2)
			goto IGNORE;
	}
	
	ClerkTotalPrice(0, lastClerk, &totalData);
	
	//SG060706
	//SG061129		if(ethernet_list.status == 2) {
	//SG061129			totalData.TotalCount = ClerkSaleAmount.totalCount;
	//SG061129			totalData.TotalPrice = ClerkSaleAmount.totalPrice;
	//SG061129			totalData.VoidCount =  ClerkSaleAmount.voidCount;
	//SG061129			totalData.VoidPrice =  ClerkSaleAmount.voidPrice;
	//SG061129		}
	
	if(totalData.TotalCount+totalData.VoidCount<1) goto IGNORE;
	
	////	ClearDisp7();
	////	display_lcd_clear();
	////	old_font=DspStruct.Id1;
	////	DspLoadFont1(DSP_SYSTEM_FONT_ID);
	KEY_SetMode(1);
	CalcPay(lastClerk, totalData.TotalPrice, 0, beep); // beep : function key id
	KEY_SetMode(0);
	
	//	}
	goto ESC_PAY;
	//NetClerkLockRelease(lastClerk);  //SG061129
	
	////	DspLoadFont1(old_font);
	////	sale_pluclear(ON);
	////	display_lcd_clear();
	////	sale_display_update(0x0fff);
	//	return;
	IGNORE:
			DisplayMsg(global_message[MSG_NO_TOTAL]);
	ESC_PAY:
			NetClerkLockRelease(lastClerk);  //SG061129
	return;
	
}

void keyapp_edit_store(void)
{
	INT8U old_mode;

#ifdef _USE_LCD20848_//ck
	Dsp_Clear();
#endif
	old_mode = KEY_GetMode();
	KEY_SetMode(1);
	Dsp_ChangeMode(DSP_PGM_MODE);

	store_name_create();

	KEY_SetMode(old_mode);
	Dsp_Clear();
	Dsp_ChangeMode(DSP_SALE_MODE);
	sale_display_update(UPDATE_STATUS);
}

void keyapp_change_prefix(void)
{
	INT8U uctemp,swedish_barcode;
	INT8U buf[22];

	uctemp = get_global_bparam(GLOBAL_SALE_SETUP_FLAG2); 
	swedish_barcode = (uctemp>>6) & 0x03;

	swedish_barcode %= 2;
	swedish_barcode += 1;

	uctemp &= (~0xc0);
	uctemp |= ((swedish_barcode << 6) & 0xc0);
	set_global_bparam(GLOBAL_SALE_SETUP_FLAG2, uctemp);

	sprintf((char *)buf, "Swedish Auto Bar.:[%d]",swedish_barcode);
	DisplayMsg(buf);
	sale_display_update(UPDATE_STATUS);
}

#ifdef USE_LOT_NUMBER_FUNCTION	//(function key #147)
void keyapp_edit_lot(void)
{
	CAPTION_STRUCT	cap;
	INT8U key_mode;
	INT8U lot_str[20];
	INT8U form_string[20];
	INT8U string_buf[100];
	INT16U str_size;
	INT16U result;
	INT32U addr_start;
	
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
	Dsp_Clear();
#endif
	key_mode = KEY_GetMode();
	KEY_SetMode(1);
	Dsp_ChangeMode(DSP_PGM_MODE);

	Menu_Init();

	str_size = sizeof(lot_str);
	addr_start = DFLASH_BASE + FLASH_LOT_NUMBER;

	while(1)
	{
		Flash_sread(addr_start, (HUGEDATA INT8U *)lot_str, str_size);

		sprintf(cap.form, "LOT:");
		cap.input_length = 20;
		cap.input_dlength = 20;
		cap.input_itype = 'S';
		cap.input_max = 0;
		cap.input_min = 0;
		cap.inputtype = 0;
		cap.input_x = 5;

		caption_adjust(&cap,NULL);
		Menu_InputCreate(1, (HUGEDATA char *)cap.form, MENU_TYPE_STR, cap.input_x, cap.input_dlength, cap.input_length,
								cap.input_max, cap.input_min, 0, NULL, (HUGEDATA INT8U *)lot_str, NULL, NULL);

		result = Menu_InputDisplay();
		if(result == MENU_RET_SAVE)
		{
			Flash_swrite(addr_start, (HUGEDATA INT8U *)lot_str, str_size);
			memset(lot_str, 0x00, str_size);
			break;
		}
		else
		{
			break;
		}
	}
	KEY_SetMode(key_mode);
	Dsp_Clear();
	Dsp_ChangeMode(DSP_SALE_MODE);
	sale_display_update(UPDATE_STATUS);
}
#endif

void keyapp_change_keyset(INT16U dept_id)
{
    CAPTION_STRUCT  cap;
	POINT disp_p;
	INT16U result;
	INT8U old;
	INT32U speedkeyID = 1;
	
	if (dept_id>=100) return;
	if (dept_id==0) dept_id = (INT16U)get_net_wparam(NETWORK_DEPARTMENT_ID);
	if (status_scale.cur_pluid)
	{
		sale_display_update_error(0xC061,0,0);
		return;
	}

	old= DspStruct.DirectDraw;
	DspStruct.DirectDraw = 1;
	Dsp_Fill_Buffer(0);
	Dsp_Diffuse();
	caption_split_by_code(0x3161, &cap,0);
	caption_adjust(&cap,NULL);
	disp_p = point(Startup.menu_ygap,0);

#if defined(USE_ARAB_CONVERT)
	Convert_ArabPreview((INT8U *)cap.form, strlen((char *)cap.form), 0, 0, 0); 
	arab_codetemp[strlen((char *)cap.form)] = 0;
	disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
	Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
	Dsp_Write_String(disp_p,( char*)cap.form);
#endif

	keyin_escape_clear = 1;
	result = keyin_ulong(KI_GRAPHIC, KI_FORM_NORMAL, 0,(long*)&speedkeyID, 1, DSP_PLU_NAME_Y,cap.input_x*Startup.menu_xgap, 0,KI_NOPASS); 
	keyin_escape_clear = 0;
	
	switch (result)
	{
		case KP_SAVE :
		case KP_ENTER :
			if ((speedkeyID >= 1) && (speedkeyID <= 5))
			{
				status_scale.cur_speedkeyid = speedkeyID;
				BuzOn(1);
			}
			else
			{
				error_display_form1(0, cap.input_min, cap.input_max);
			}
			break;
		case KP_ESC :
		case KP_CLEAR :
		case KS_MULTI :
			BuzOn(1);
			break;
		default : 
			BuzOn(2);
			break;
	}

	DspStruct.DirectDraw = old;
	Dsp_Fill_Buffer(0);
	Dsp_Diffuse();
	return;
}

extern SALE_G_FLOAT_TYPE	GlbFloat;
extern SALE_G_ADD_TYPE		GlbAddup;


long total_minus_disc(long totalPrice)
{
	long	result;
	long	tPrice;
	INT16S     decpoint=DecimalPointPrice();
	char    old;
	long    ret;
	INT8U   old_key;
	char    char_w;
	CAPTION_STRUCT	cap;
	
	ret = 0;
	tPrice = 0;
	
	old_key = KEY_GetMode();
	KEY_SetMode(1);
	
	old=DspStruct.Id1;
	DspLoadFont1(DSP_MSG_FONT_ID);

#ifdef USE_ARAB_CONVERT
    strset(gmsg,0x20,35);
    PutNSmallString(0,0,gmsg,35);
#endif	
	//PutNSmallString(0,0,global_message[MSG_DC_MINUS],35);
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
	caption_split_by_code(0xc01d,&cap,0);
#ifdef HEBREW_FONT
	strcat(cap.form, ")-(");
#else
	strcat(cap.form, "(-)");
#endif
#else
	caption_split_by_code(0xc01c,&cap,0);
#endif
	//	strcat(cap.form, "(-)");
	cap.input_length = 7;
    cap.input_dlength = 7;
	caption_adjust(&cap, NULL);
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
    PutString(DSP_MSG_Y, DSP_MSG_X, cap.form, DSP_MSG_FONT_ID, DSP_MSG_MAGY, DSP_MSG_MAGX, MAX_MSG_CHAR);
#else
	display_clear(DISPLAY_WEIGHT);
	display_string(DISPLAY_WEIGHT,(INT8U *)cap.form);
#endif
	VFD7_Diffuse();
	//old=DspStruct.Id1;
	DspLoadFont1(DSP_MSG_FONT_ID);
	char_w = display_font_get_width();
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
  #ifdef USE_ARAB_CONVERT
    result = keyin_ulong(KI_GRAPHIC, KI_FORM_ILNN, decpoint,(long*)&tPrice, cap.input_length, DSP_MSG_Y,  DSP_MSG_X+cap.input_x*char_w, 0,KI_NOPASS );
  #else
	result = keyin_ulong(KI_GRAPHIC, KI_FORM_NORMAL, decpoint,(long*)&tPrice, cap.input_length, DSP_MSG_Y,  DSP_MSG_X+cap.input_x*char_w, 0,KI_NOPASS );
  #endif
#else
	result = keyin_ulong(KI_UNIT, KI_FORM_NORMAL, decpoint,(long*)&tPrice, cap.input_length, DSP_MSG_Y,  DSP_MSG_X+cap.input_x*char_w, 0,KI_NOPASS );
#endif
	DspLoadFont1(old);
	if(result==KP_ENTER || result==KP_SAVE)
	{
		if(0<totalPrice-tPrice)
		{
			ret = tPrice;
			//return	tPrice;
		}
	}
	
	KEY_SetMode(old_key);
	return ret;
}

long total_sale_percent(void)
{
	long result;
	long tPer;
	char old;
	long ret;
	INT8U old_key;
	char    char_w;
	CAPTION_STRUCT	cap;
	
	ret = 0;
	tPer = 0;
	
	old_key = KEY_GetMode();	
	KEY_SetMode(1);
	
	old=DspStruct.Id1;
	DspLoadFont1(DSP_MSG_FONT_ID);

#ifdef USE_ARAB_CONVERT
    strset(gmsg,0x20,35);
    PutNSmallString(0,0,gmsg,35);
#endif	
	//PutNSmallString(0,0,global_message[MSG_DC_PERCENT],35);
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
		caption_split_by_code(0xc01d,&cap,0);
#ifdef HEBREW_FONT
		strcat(cap.form, ")%(");
#else
		strcat(cap.form, "(%)");
#endif
#else	
	caption_split_by_code(0xc01d,&cap,0);
#endif
	cap.input_length = 2;
	caption_adjust(&cap, NULL);
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
    PutString(DSP_MSG_Y, DSP_MSG_X, cap.form, DSP_MSG_FONT_ID, DSP_MSG_MAGY, DSP_MSG_MAGX, MAX_MSG_CHAR);
#else
	display_clear(DISPLAY_WEIGHT);
	display_string(DISPLAY_WEIGHT,(INT8U *)cap.form);
#endif	
	VFD7_Diffuse();
	
	//old=DspStruct.Id1;
	DspLoadFont1(DSP_MSG_FONT_ID);
	char_w = display_font_get_width();
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
  #ifdef USE_ARAB_CONVERT
    result = keyin_ulong(KI_GRAPHIC, KI_FORM_ILNN, 0,(long*)&tPer, cap.input_length, DSP_MSG_Y,  DSP_MSG_X+cap.input_x*char_w, 0,KI_NOPASS );
  #else
	result = keyin_ulong(KI_GRAPHIC, KI_FORM_NORMAL, 0,(long*)&tPer, cap.input_length, DSP_MSG_Y,  DSP_MSG_X+cap.input_x*char_w, 0,KI_NOPASS );
  #endif
#else
	result = keyin_ulong(KI_UNIT, KI_FORM_NORMAL, 0,(long*)&tPer, cap.input_length, DSP_MSG_Y,  DSP_MSG_X+cap.input_x*char_w, 0,KI_NOPASS );
#endif
	DspLoadFont1(old);
	if(result==KP_ENTER || result==KP_SAVE)
	{
		ret = tPer;//return tPer;
	}
	//
	KEY_SetMode(old_key);
	return ret;
}

INT16S	LoadDefaultCurrency(void)
{
	if(CurrencyPrice.CurrencyUseDefault)//OPTION: default currency
	{
		if(CurrencyPrice.CurrencyIndex!=1)
			keyapp_currency(0,0,0,0,KS_CURRENCY_01);
		if(CurrencyPrice.CurrencyRate == 0)
			return 0;
	}
	return 1;
}

INT16S	ClearCurrencyPrice(void)
{
	CurrencyPrice.CurrencyKeyCode	= 0;
	CurrencyPrice.CurrencyIndex	= 0;
	////	CurrencyPrice.CurrencyPrice	= 0;
	////	CurrencyPrice.CurrencyUnitPrice	= 0;
	CurrencyPrice.CurrencyPriceH8	= 0;
	CurrencyPrice.CurrencyPriceL8	= 0;
	CurrencyPrice.CurrencyUnitPriceH8	= 0;
	CurrencyPrice.CurrencyUnitPriceL8	= 0;
	CurrencyPrice.CurrencyRate	= 0;
	
	LoadDefaultCurrency();
	
	return 1;
}

void	ComputeCurrency(INT16S dipslayMsg,INT16S y,INT16S x,long price,long unitprice)// display,y,x
{
	char	priceBuf[20];//, tempBuf[20];
	char 	dec_ch;
	
	if(CurrencyPrice.CurrencyUseDefault && !CurrencyPrice.CurrencyKeyCode)
	{
		CurrencyPrice.CurrencyKeyCode = KS_CURRENCY_01;
	}
	
	if(CurrencyPrice.CurrencyKeyCode)
	{
		keyapp_currency(0,0,price,unitprice,CurrencyPrice.CurrencyKeyCode);// beep off, display off
		if(dipslayMsg)
		{
			////			////if(CurrencyPrice.CurrencyPrice == 0xffffffff) priceBuf[0] = 0;
			////			////else {
			////				if(CurrencyPrice.CurrencyPriceDiv)
			////					FloatToString(TO_STR,priceBuf,9,DISPLAY_PRICE,&CurrencyPrice.CurrencyPrice,CurrencyPrice.CurrencyPriceDiv);
			////				else
			////					sprintf(priceBuf,"%ld",CurrencyPrice.CurrencyPrice);
			dec_ch = get_global_bparam(GLOBAL_DECIMAL_SIGN);
			format_value_2long((HUGEDATA INT8U *)priceBuf, CurrencyPrice.CurrencyPriceH8, CurrencyPrice.CurrencyPriceL8, CurrencyPrice.CurrencyPriceDiv, dec_ch);
			//				if(CurrencyPrice.CurrencyPriceH8) {
			//					sprintf(priceBuf, "%ld", CurrencyPrice.CurrencyPriceH8);
			//					len = strlen(priceBuf);
			//					if(CurrencyPrice.CurrencyPriceDiv) {
			//						//FloatToString(TO_STR,priceBuf+len,9,DISPLAY_PRICE,&CurrencyPrice.CurrencyPriceL8,CurrencyPrice.CurrencyPriceDiv);
			//						format_value(tempBuf,CurrencyPrice.CurrencyPriceL8, CurrencyPrice.CurrencyPriceDiv,'.');
			//						memset(priceBuf+len, '0', 9 - strlen(tempBuf));
			//						len += (9 - strlen(tempBuf));
			//						sprintf(priceBuf+len, "%s", tempBuf);
			//					} else {
			//						sprintf(priceBuf+len,"%08ld",CurrencyPrice.CurrencyPriceL8);
			//					}
			//				} else {
			//					if(CurrencyPrice.CurrencyPriceDiv)
			//						FloatToString(TO_STR,priceBuf,9,DISPLAY_PRICE,&CurrencyPrice.CurrencyPriceL8,CurrencyPrice.CurrencyPriceDiv);
			//					else
			//						sprintf(priceBuf,"%ld",CurrencyPrice.CurrencyPriceL8);
			//				}
			////			////}
			//			caption_split_by_code(0xc050,&cap,1);//"Total %s %s"
			sprintf(gmsg,global_message[MSG_CURRENCY_CHANGE],CurrencyPrice.CurrencyString1,str_trimleft(priceBuf));
			PutSmallString(y,x,gmsg);
		}
	}
}

void CheckGrandTotalStatus(void)
{
	PREPACK_STATUS_TYPE PrepackStatus;
	
	if (Operation.operationClerk != OPER_NORMAL) return;
	
	get_PrepackStatus(&PrepackStatus);
	if(PrepackStatus.PrevSubPluNo == 0 && PrepackStatus.OneItem_Sub) 
	{
		PrepackStatus.OneItem_Grand = ON;
	}
	else if(PrepackStatus.PrevSubPluNo != PrepackStatus.CurPluNo)
	{
		PrepackStatus.OneItem_Grand = OFF;
	}
	PrepackStatus.PrevSubPluNo = PrepackStatus.CurPluNo;
	PrepackStatus.PrevPluNo = 0;
	set_PrepackStatus(&PrepackStatus);
}

void CheckSubTotalStatus(void)
{
	PREPACK_STATUS_TYPE PrepackStatus;
	
	get_PrepackStatus(&PrepackStatus);
	PrepackStatus.CurDeptNo = status_scale.Plu.deptid;
	PrepackStatus.CurPluNo = status_scale.cur_pluid;
	if(PrepackStatus.PrevPluNo == 0) 
	{
		PrepackStatus.OneItem_Sub = ON;
	}
	else if(PrepackStatus.PrevPluNo != PrepackStatus.CurPluNo)
	{
		PrepackStatus.OneItem_Sub = OFF;
	}
	PrepackStatus.PrevPluNo = PrepackStatus.CurPluNo;
	set_PrepackStatus(&PrepackStatus);
}

extern long LocalPayTotalSummaries(TRANSACTION_PAY Pay,INT8U recvmode, INT8U reopen_flag);
//extern INT16S CreditAddPay(INT16S creditCustomerNo,TRANSACTION_PAY transPay,INT16S creditFlag);
extern	SALE_G_PRICE_TYPE	GlbPrice;
extern INT8U ClerkTransactionSaleStore(INT8U clerkNum);
//extern INT8U ClerkTransactionSaleSend(HUGEDATA COMM_BUF *CBuf, INT8U clerkNum);
extern INT8U CloseTicketWithoutPrint;
#ifdef USE_EFT_POS_PROTOCOL
extern INT8U PosProtocolSendFlag;
#endif
void	PrintTotalLabelTicket(INT8U trtType,INT16U lastClerk,long resultPrice,long payPrice, long savedPrice)
{
	////	INT16S totalCount;
	////	INT16S voidCount;
	////	long totalPrice;
	////	long voidPrice;
	INT8U	result;
	INT8U 	ret_value=0;
	INT16U 	bufdata_count=0;
	INT8U 	totalDisplayTime, i;
	INT8U	prt_total_conti;
	INT8U	escape_prt = 0;
#ifdef USE_DISCOUNTED_TAX_FOR_DISCOUNTED_TOTAL
	float discountRate;
#endif
	prt_total_conti = get_global_bparam(GLOBAL_PRINT_OPER_FLAG2) & 0x20;	//1872 Prt total label conti
	totalDisplayTime = get_global_bparam(GLOBAL_SALE_SETUP_FLAG12);
	totalDisplayTime = totalDisplayTime & 0x0f;
	totalDisplayTime *= 10;		//key delay sec 는 systimer100ms 사용 함. 
	ClerkTotalPrice(0, lastClerk, &TotalData);
	TotalData.SummaryTotalPrice = resultPrice;
	TotalData.SummarySavedPrice = savedPrice;
	TotalData.PayPrice = payPrice;
	TotalData.SummaryRound = transPay.summaryround;
	if(trtType == TRANS_TYPE_PAY_TOTAL_NOPAY)
	{
		TotalData.PayPrice = 0;
		DiscSale.saleCreditCustomer = 0;
	}
	/////////////////////////
	
	// Modified by CJK 20041112
	//result = PrintTotal(lastClerk, discountPer,discountPrice);
	transPay.trtType = trtType;// 일반적 total은 cash지불로 인정X
	transPay.scaleid = status_scale.scaleid;
	transPay.clerkid = lastClerk;
	transPay.summarydiscount = savedPrice;	//Added by JJANG 20080529
#ifdef USE_INDIA_FUNCTION
	transPay.ticketNo = get_TicketTransactionNumber(lastClerk);
	transPay.transCount= TotalData.TotalCount;
#endif
#ifdef USE_DISCOUNTED_TAX_FOR_DISCOUNTED_TOTAL
	if (TotalData.TotalPrice == 0) discountRate = 1;
	else
	{
		discountRate = (float)TotalData.SummaryTotalPrice / (float)TotalData.TotalPrice;
	}
	for (i = 0; i < MAX_TAX_NO; i++)
	{
		if (TotalData.taxPrice[i] > 0)
			TotalData.taxPrice[i] = ((float)TotalData.taxPrice[i] * discountRate + 0.5);
		else
			TotalData.taxPrice[i] = ((float)TotalData.taxPrice[i] * discountRate - 0.5);
		if (TotalData.taxGross[i] > 0)
			TotalData.taxGross[i] = ((float)TotalData.taxGross[i] * discountRate + 0.5);
		else
			TotalData.taxGross[i] = ((float)TotalData.taxGross[i] * discountRate - 0.5);
		transPay.taxPrice[i] = TotalData.taxPrice[i];
		transPay.taxGross[i] = TotalData.taxGross[i];
	}
	if (TotalData.InTaxPrice > 0)
		TotalData.InTaxPrice = ((float)TotalData.InTaxPrice * discountRate + 0.5);
	else
		TotalData.InTaxPrice = ((float)TotalData.InTaxPrice * discountRate - 0.5);
	if (TotalData.ExTaxPrice > 0)
		TotalData.ExTaxPrice= ((float)TotalData.ExTaxPrice * discountRate + 0.5);
	else
		TotalData.ExTaxPrice= ((float)TotalData.ExTaxPrice * discountRate - 0.5);
#endif
	//{
	//	INT16S i;
	//	for(i=0;i<MAX_PAID_TYPE;i++)
	//	{
	//	sprintf(gmsg,"[%d][%7ld]",i,transPay.paidprice[i]);
	//	PutSmallString(0,0,gmsg);
	//	Key_DelaySec(10,0);
	//	}
	//}
#ifdef USE_TAX_RATE
	TotalData.SummaryTotalPrice += TotalData.ExTaxPrice; 
#endif
	
	if(ethernet_list.status==4)	//Modified by JJANG 20080313
	{
        bufdata_count = GetClerkTransactionCount(lastClerk);
		ret_value = GetCheckAddTransactionData(0,bufdata_count,AUDIT_MODE_LINEAR);		//mode : sale
	}
	else ret_value = 1;
	
	if(ret_value)
	{
		if(CloseTicketWithoutPrint)
		{
			result = 1;
			//			caption_split_by_code(0x9C86, &cap, 0);		//TICKET CLOSED
			//			DisplayMsg(cap.form);
			BuzOn(1);					//Buzzer 거래 성공 타입으로 변경 필요(Long buzzer). 
			//		   	Key_DelaySec(20,0);
			CloseTicketWithoutPrint = OFF;
		}
		else
		{
			result = Prt_PrintStart(PRT_TOTAL_NORMAL_MODE, PrtStruct.Mode, lastClerk, ON, 0, 0);
		}
		
#ifdef USE_CHN_CART_SCALE
		result = 1; // 인쇄에러와 상관없이 정상처리
#else
		//Added by JJANG 20080131 프린트 출력 오류났을 때 에러메시지 출력 (print error, YES/NO?) 
		if(result == 2)	//paper end err
		{
			BuzOn(2);
			//VFD7_all_char(0x00);
			display_alloff_primary_indication();
			VFD7_Diffuse();
			if(display_message_printerr_page(0xCA01,0xCA02))	//Modified by JJANG 20080221 함수교체
			{
				set_nvram_bparam(NVRAM_SLAVECHECK_FLAG,1);
				result = ON;		
			}
		}
#endif
		if(result == 1) 
		{
			if (Operation.operationClerk == OPER_NORMAL)
			{
				ClerkGrandTotalSum(lastClerk, &TotalData);// , 0);	//CJK20040909
				CheckGrandTotalStatus();
			}
			//if(directPrint)	// DEL CJK070321
			//{
			LocalPayTotalSummaries(transPay,0,OFF);
			//}
#ifdef USE_TAX_RATE
			Get_PLUSaleSummaryRenewal(1, discountRate, lastClerk, status_scale.scaleid, OFF);
#endif
			ClerkTransactionSaleStore(lastClerk);
			//		network_ClerkDeleteTransaction(lastClerk);

#ifdef USE_EFT_POS_PROTOCOL
			INT8U use_eft_pos;
			use_eft_pos = (get_global_bparam(GLOBAL_SALE_SETUP_FLAG21) & 0x30) >> 4;	// EFT-POS PROTOCOL 전송방식 선택
			if(use_eft_pos != NOT_USE)
			{
				PosProtocolSendFlag = ON;
				transactionSendTicketData(lastClerk);
			}
#endif
#if defined(USE_INDIA_FUNCTION) || defined(USE_SEND_TICKET_DATA)
			Operation.transAddup = OFF;
            if(flagcopy_ticket == OFF) // not copy ticket
            {
  #ifdef USE_SEND_TICKET_DATA
    			if (GlbOper.realtimeTransactionType == 9)
                {
                    transactionSendTicketData(lastClerk);
                    TransactionSendWithoutVoid(lastClerk);
    			}
  #else
    			TransactionSendWithoutVoid(lastClerk);
  #endif
            }
			DeleteAdditionalData(lastClerk);
#endif
            if(flagcopy_ticket == ON)
            {
                flagcopy_ticket = OFF;
            }
			else									// Copy Ticket이 아닌 일반적인 상황에서 수행한 Total Print 동작의 경우,
			{
				setLastTicketNumber(lastClerk);		// lastTicketNumberEachClerk 배열의 값을 갱신 -> Copy Ticket 시 해당 저울에서 수행하였는지를 판단하기 위해서 
			}
			//Added by JJANG 20080130
			//Slave와 Master가 다시 연결될 때까지 기다렸다가 DATA전송
			set_nvram_bparam(NVRAM_SLAVECHECK_FLAG, 1); 
			while(!network_ClerkDeleteTransaction(lastClerk));
			set_nvram_bparam(NVRAM_SLAVECHECK_FLAG, 0); 
			
			ReopenedTransPay = transPay;
			ClerkDeleteTransaction(lastClerk);
			memset(&ReopenedTransPay, 0, sizeof(TRANSACTION_PAY));
			TareOper.addupWeight = 0;
			if(DiscSale.saleCreditCustomer && transPay.paidprice[PAY_CREDIT])
			{
				// Don't Erase! Need Check! It will be used later.(CJK)
				//CreditAddPay(DiscSale.saleCreditCustomer,transPay,1);
				///////
			}
			if(prt_total_conti && (PrtStruct.Mode == LABEL))		//Menu 1872 Print total label continuously
			{
				if(Startup.country != COUNTRY_KR)
				{
					caption_split_by_code(0x97A3, &cap, 0);	//"[ESC]=escape"
					PutSmallString(3,0,cap.form);
					Dsp_Diffuse();
				}
				while(!escape_prt) 
				{
					while(!KEY_Read());
					switch(KeyDrv.CnvCode) 
					{
						case KS_PRINT :
							result = Prt_PrintStart(PRT_TOTAL_NORMAL_MODE, PrtStruct.Mode, lastClerk, ON, 0, 0);
							if(result != 1) escape_prt = ON;
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
#ifdef USE_QR_ADDTOTAL
				// QrAddTotal Data 삭제
				nvram_set(NVRAM_QR_ADD_DATA, 0x00, 1000);
				set_nvram_bparam(NVRAM_QR_ADD_COUNT, 0);
// 실제 사용하는 바코드 폼의 길이에 따라 Add Total Count를 계산 
// NVRAM 영역, Floating Clerk Data Sync 등 추후 다시 적용 방안 검토 필요 
//				QR_addedMemUsage[status_scale.clerkid] = 0;
				TotalQRFlag = 0;
#endif
			}
			else
			{
#ifdef USE_QR_ADDTOTAL
				// QrAddTotal Data 삭제
				nvram_set(NVRAM_QR_ADD_DATA, 0x00, 1000);
				set_nvram_bparam(NVRAM_QR_ADD_COUNT, 0);
// 실제 사용하는 바코드 폼의 길이에 따라 Add Total Count를 계산 
// NVRAM 영역, Floating Clerk Data Sync 등 추후 다시 적용 방안 검토 필요 
//				QR_addedMemUsage[status_scale.clerkid] = 0;
				TotalQRFlag = 0;
#endif
#ifndef USE_CHN_CART_SCALE
				Key_DelaySec((INT16S)totalDisplayTime, 0);	//totalDisplayTime 0 인경우 키 입력시 까지 멈춰있음.
#endif
			}
		}
	}
}


void keyapp_total(INT16S directPrint)
{
	char priceBuf[15];
	char	tPriceStr[20];
	INT16S	tempKey;
	INT16U	lastClerk=0;
	long	resultPrice;	//rounded price
#ifdef USE_AU_ROUND
	long	notRoundedPrice;	//not rounded price
#endif
	static long	discountPrice_copyticket[MAX_CLERK_NO] = {0, };
	long	discountPrice=0;
	INT16S		discountPer=0;
	INT16S printFlag = OFF;
	INT8U  keyMode;
	char   tempBuf[50];
	STRUCT_TOTAL_PRICE totalData;
	long roundDifference;
	char weightBuf[15];	
	char w_sign1[5];
#if !defined(USE_SINGLE_LINE_GRAPHIC)
	INT8U	totalinfo_disp = 0;
#endif
	INT8U	v8_1;
	INT8U	dec_ch;
	INT8U	use_unit;
	INT16S	ret;
	INT32U curPluTemp = 0;
#ifdef USE_TAX_RATE
	long	discountTax = 0;
#endif
#ifdef USE_AU_ROUND
	INT8U	roundFlag = OFF;
#endif	

	lastClerk = status_scale.clerkid;
	ret = NetClerkLock(lastClerk);
	if (ret == 3) goto END_RELEASE;	// timeout
	else if (ret == 0) return;	// clerk lock
	
	PutXStringOffsetX = DSP_SALE_INFO_AREA_X1;
	PutXStringOffsetY = DSP_SALE_INFO_AREA_Y1;
	dec_ch  = get_global_bparam(GLOBAL_DECIMAL_SIGN);
#if !defined(USE_SINGLE_LINE_GRAPHIC)
	totalinfo_disp = get_global_bparam(GLOBAL_SALE_SETUP_FLAG7)& 0x10;
	if (ethernet_list.status == 2)
	{
		totalinfo_disp = 0;	// total weight 정보를 master로 부터 받을 수 없으므로 무게 정보 디스플레이 안함(필요시 수정)
	}
#endif
	
	memset(&totalData, 0, sizeof(STRUCT_TOTAL_PRICE));

#ifdef USE_AU_ROUND	
	if (transPay.summaryround)
	{
		roundFlag = ON; //summaryround 값이 있을 경우 플래그 ON
	}
#endif

	
	ClearCurrencyPrice();
	ClearPayment(TRANS_TYPE_PAY_TOTAL_NOPAY);
	// SummaryRound
	while(1)
	{
		ClerkTotalPrice(0, lastClerk, &totalData);
		
		if(totalData.TotalCount + totalData.VoidCount < 1)
		{
			if(lastClerk)
			{
#ifdef HEBREW_FONT
				strcpy(tempBuf,")%d#( ");
#else
				strcpy(tempBuf,"(#%d) ");
#endif
				strcat(tempBuf,global_message[MSG_NO_TOTAL]);//"No total price"
				sprintf(gmsg,tempBuf,lastClerk);
			}
			else
			{
				strcpy(gmsg,global_message[MSG_NO_TOTAL]);//"No total price"
			}
			DisplayMsg(gmsg);
			Key_DelaySec(5,0);
			goto END_RELEASE;
		}
		ClearDisp7();
		display_lcd_clear();
		///////////////////////////////////////////////////////////////
#if defined(USE_SINGLE_LINE_GRAPHIC)
		sprintf(tempBuf, "n%4d", totalData.TotalCount);
		display_string(DISPLAY_TARE, (INT8U*)tempBuf);
		
		use_unit = ADM_Status.UseUnit;
		if(use_unit == WEIGHT_UNIT_KG) {
			if(get_base_bparam(FLASH_ADC_WUNIT) == WEIGHT_UNIT_G) use_unit = WEIGHT_UNIT_G;
		}
		v8_1=get_decimal_pt(3, use_unit);	// UseUnit 사용
		format_value((HUGEDATA INT8U *)weightBuf, totalData.TotalWeight, v8_1, dec_ch);
		display_string(DISPLAY_WEIGHT, (INT8U*)weightBuf);
		
		caption_split_by_code(0xc013,&cap,0);
		display_string(DISPLAY_UNITPRICE, (INT8U*)cap.form);
		
		if(flagcopy_ticket != OFF){ // Copy Ticket
			discountPrice = discountPrice_copyticket;
		}
		else{
			if(discountPrice==0)discountPrice_copyticket = 0;
		}
		resultPrice = totalData.TotalPrice - discountPrice;
#ifdef USE_AU_ROUND
		notRoundedPrice = resultPrice;
#endif
		if(GlbPrice.roundApply == ROUND_TOTAL_OR_PLU)
		{
			resultPrice = PriceRounds(resultPrice,ROUND_TOTAL_OR_PLU,ON, &roundDifference);
			transPay.summaryround = roundDifference;
		}
		else if((GlbPrice.roundApply == ROUND_ONLY_TOTAL_SUM) && (PrtStruct.Mode == RECEIPT))
		{
			resultPrice = PriceRounds(resultPrice,ROUND_ONLY_TOTAL_SUM,ON, &roundDifference);
			transPay.summaryround = roundDifference;
		}
#ifdef USE_AU_ROUND	//호주의 경우 반올림 되지 않은 값을 display에 표시한다.
		FloatToString(TO_STR,priceBuf,8,DISPLAY_PRICE,&notRoundedPrice,0,1);
#else
		FloatToString(TO_STR,priceBuf,8,DISPLAY_PRICE,&resultPrice,0,1);
#endif
		display_string(DISPLAY_PRICE, (INT8U*)priceBuf);
		VFD7_Diffuse();
#ifdef USE_AU_ROUND		//호주의 경우 반올림 되지 않은 값을 display에 표시한다.
		ComputeCurrency(0,3,0,notRoundedPrice,0);// display,y,x
#else
		ComputeCurrency(0,3,0,resultPrice,0);// display,y,x
#endif
#else //#if defined(USE_SINGLE_LINE_GRAPHIC)
		///////////////////////////////////////////////////////////////
		sprintf(gmsg,global_message[MSG_TOTALCNT],totalData.TotalCount,totalData.VoidCount);//"Total Count %d, Void Count %d"
		if(lastClerk)
		{
			char	tempBuf[100];
#if defined(USE_ARAB_FONT) || defined(HEBREW_FONT)
			sprintf(tempBuf," %d #",lastClerk);
#else
			sprintf(tempBuf,"# %d ",lastClerk);
#endif
			strcat(tempBuf,gmsg);
			strcpy(gmsg,tempBuf);
		}
		PutNSmallString(0, 0,gmsg, 40);
		if(flagcopy_ticket != OFF){ // Copy Ticket
			discountPrice = discountPrice_copyticket[lastClerk];
		}
		else{
			if(discountPrice==0)discountPrice_copyticket[lastClerk] = 0;
		}
#ifdef USE_TAX_RATE
		if(!discountTax)
			discountTax = totalData.ExTaxPrice;
		resultPrice = totalData.TotalPrice - discountPrice + discountTax;
#else
		resultPrice = totalData.TotalPrice - discountPrice;
#endif
#ifdef USE_AU_ROUND
		notRoundedPrice = resultPrice;
#endif
		if(GlbPrice.roundApply == ROUND_TOTAL_OR_PLU)
		{
			resultPrice = PriceRounds(resultPrice,ROUND_TOTAL_OR_PLU,ON, &roundDifference);
			transPay.summaryround = roundDifference;
		}
		else if((GlbPrice.roundApply == ROUND_ONLY_TOTAL_SUM) && (PrtStruct.Mode == RECEIPT))
		{
			resultPrice = PriceRounds(resultPrice,ROUND_ONLY_TOTAL_SUM,ON, &roundDifference);
			transPay.summaryround = roundDifference;
		}
#ifdef USE_AU_ROUND	//호주의 경우 반올림 되지 않은 값을 display에 표시한다.
		FloatToString(TO_STR,priceBuf,8,DISPLAY_PRICE,&notRoundedPrice,0,1);
#else
		FloatToString(TO_STR,priceBuf,8,DISPLAY_PRICE,&resultPrice,0,1);
#endif
		GetPriceString(tPriceStr,priceBuf,ON,PRICE_PRINTER);
		if(totalinfo_disp)	
		{
			use_unit = ADM_Status.UseUnit;
			if(use_unit == WEIGHT_UNIT_KG) {
				if(get_base_bparam(FLASH_ADC_WUNIT) == WEIGHT_UNIT_G) use_unit = WEIGHT_UNIT_G;
			}
			v8_1=get_decimal_pt(3, use_unit);	// UseUnit 사용
			GetWeightSymbol(use_unit, w_sign1);
			format_value((HUGEDATA INT8U *)weightBuf, totalData.TotalWeight, v8_1, dec_ch);
			strcat(weightBuf, w_sign1);
			caption_split_by_code(0x2035,&cap,0);//"Weight" 
			sprintf(gmsg,"%s   %s", cap.form, weightBuf);
			PutSmallString(1,0,gmsg);

			caption_split_by_code(0xc013,&cap,0);//"SubTotal %s"
			strcat(cap.form," %s");
			sprintf(gmsg,cap.form,tPriceStr);
#ifdef USE_CL5200_DISPLAY
			PutNSmallString(2, 0,gmsg, 30);
#elif defined(USE_CL5500_DISPLAY)
			PutNMediumString(2, 0,gmsg, 30);
#endif
		}
		else 
		{
			caption_split_by_code(0xc013,&cap,0);//"SubTotal %s"
			strcat(cap.form," %s");
			sprintf(gmsg,cap.form,tPriceStr);
			PutNMediumString(1, 0,gmsg, 30);
#ifdef USE_AU_ROUND		//호주의 경우 반올림 되지 않은 값을 display에 표시한다.
            ComputeCurrency(1,3,0,notRoundedPrice,0);// display,y,x
        }
#else
            ComputeCurrency(1,3,0,resultPrice,0);// display,y,x
        }
#endif
		///////////////////////////////////////////////////////////////
#endif //#if defined(USE_SINGLE_LINE_GRAPHIC)
		
		if(directPrint)
		{
			tempKey = KS_SAVE;
			//if(flagcopy_ticket != OFF){ // Copy ticket
			//	flagcopy_ticket = OFF;
			//}
		}
		else
		{
			tempKey = KeyWait(KS_SALE_PER,KS_MINUS,KS_PAY,KS_VOID, KS_TTL);
		}
		if (get_global_bparam(GLOBAL_SALE_SETUP_FLAG4)&0x40)	//20090406 Parameter 724 : total에서 discount 동작안함
		{
			if(tempKey==KS_SALE_PER || tempKey==KS_MINUS) goto END_RELEASE;
		}
		else
		{
			if(tempKey==KS_SALE_PER)
			{
				discountPer    	= (INT16S)total_sale_percent();
				discountPrice	= CalcPercentPrice(totalData.TotalPrice,discountPer*100);
				discountPrice_copyticket[lastClerk]= discountPrice;
#ifdef USE_TAX_RATE
				discountTax = totalData.ExTaxPrice * discountPer / 100;
#endif
				continue;
			}
			if(tempKey==KS_MINUS)
			{
				discountPrice	= total_minus_disc(totalData.TotalPrice);
				discountPer = 0;
				discountPrice_copyticket[lastClerk]= discountPrice;
#ifdef USE_TAX_RATE
				discountTax = (float)discountPrice/(float)totalData.TotalPrice * totalData.ExTaxPrice + 0.5;
#endif
				continue;
			}
		}
		
		if(tempKey==KS_VOID)
		{
			keyMode=KEY_GetMode();
			KEY_SetMode(1);
			keyapp_void();
			KEY_SetMode(keyMode);
			continue;
		}
		if(tempKey==KS_SAVE || tempKey==KS_PRINT || tempKey==KS_EXT_PRINT)
		{
			printFlag = ON;
			break;
		}
		if(tempKey==KS_MULTI || tempKey==KS_CLEAR)
		{
			printFlag = OFF;
			break;
		}
		if((tempKey==KS_PAY) || ((tempKey >= KS_PAY_1) && (tempKey <= KS_PAY_7)))
		{
#ifdef USE_INDIA_FUNCTION
			if (UseAreaScaleMode == 2) // 0:NormalScale(STD), 1:DualModeScale, 2:AreaScale
			{
				BuzOn(2);
				continue;
			}
#endif
			break;
		}
		if(tempKey == KS_TTL)		//SUBTOTAL에서 TOTAL 키를 한번 더 누르면 Close ticket without print 됨.(독일요청)
		{
			if(get_global_bparam(GLOBAL_SALE_SETUP_FLAG15) & 0x80)	//M1873 "Close ticket without print: Y"로 설정 
			{
				printFlag = ON;
				CloseTicketWithoutPrint = ON;
				break;
			}
		}
	}
	if(tempKey==KP_SAVE || tempKey==KP_ENTER || tempKey==KS_PAY || ((tempKey >= KS_PAY_1) && (tempKey <= KS_PAY_7)) || printFlag || tempKey==KS_EXT_PRINT)
	{
		if (!directPrint)
		{
			if (NetClerkLock(lastClerk) != 3)	// 3:timeout
				ClerkTotalPrice(0, lastClerk, &totalData);
			else goto END_RELEASE;
		}
#ifdef USE_TAX_RATE
		if(!discountTax)
			discountTax = totalData.ExTaxPrice;
		resultPrice = totalData.TotalPrice - discountPrice + discountTax;
#else
		resultPrice = totalData.TotalPrice - discountPrice;
#endif
		if(GlbPrice.roundApply == ROUND_TOTAL_OR_PLU)
		{
			resultPrice = PriceRounds(resultPrice,ROUND_TOTAL_OR_PLU,ON, &roundDifference);
			transPay.summaryround = roundDifference;
		}
		else if((GlbPrice.roundApply == ROUND_ONLY_TOTAL_SUM) && (PrtStruct.Mode == RECEIPT))
		{
			resultPrice = PriceRounds(resultPrice,ROUND_ONLY_TOTAL_SUM,ON, &roundDifference);
			transPay.summaryround = roundDifference;
		}
		transPay.paidprice[PAY_CASH] = resultPrice;	// NoPay는 Cash로 간주
		
#if !defined(USE_SINGLE_LINE_GRAPHIC)
#ifdef USE_AU_ROUND		//호주의 경우 반올림 되지 않은 값을 display에 표시한다.
		ComputeCurrency(1,3,0,notRoundedPrice,0);// display,y,x
		ComputeCurrency(0,0,0,resultPrice,0);// display,y,x
#else
		ComputeCurrency(1,3,0,resultPrice,0);// display,y,x
#endif
#endif
		if(tempKey==KS_PAY)
		{
			KEY_SetMode(1);
			CalcPay(lastClerk, totalData.TotalPrice, discountPrice, 0);
			KEY_SetMode(0);
		}
		else if((tempKey >= KS_PAY_1) && (tempKey <= KS_PAY_7))
		{
			KEY_SetMode(1);
			CalcPay(lastClerk, totalData.TotalPrice, discountPrice, tempKey-KS_PAY_1+1);
			KEY_SetMode(0);
		}
		else
		{
#ifdef USE_TAX_RATE
			FloatToString(TO_STR,priceBuf,8,DISPLAY_PRICE,&resultPrice,0,1);
			GetPriceString(tPriceStr,priceBuf,ON,PRICE_PRINTER);
#endif
			caption_split_by_code(0xc022,&cap,0);//"Total %s"
#if defined(USE_SINGLE_LINE_GRAPHIC)
			display_clear(DISPLAY_UNITPRICE);
			display_string(DISPLAY_UNITPRICE, (INT8U*)cap.form);
			
			display_clear(DISPLAY_PRICE);
			display_string(DISPLAY_PRICE, (INT8U*)priceBuf);
			VFD7_Diffuse();
#else
			strcat(cap.form," %s");
			sprintf(gmsg,cap.form,tPriceStr);
  #ifdef USE_CL5500_DISPLAY
  			if(totalinfo_disp)
				PutNMediumString(2, 0,gmsg, 30);
			else
				PutNMediumString(1, 0,gmsg, 30);
  #elif defined(USE_CL5200_DISPLAY)
			PutNMediumString(1, 0,gmsg, 30);
  #endif
#endif
			Cash_open();	//transaction 종료시 cash drawer open
			GlbFlagPCnMasterConnection = NetCheckConnection();
#ifdef USE_TAX_RATE
			resultPrice -= discountTax;
#endif
#ifdef USE_AU_ROUND
			if(Operation.operationClerk != OPER_FLOATING)	//Total Label일 경우에 Display와 동일한 값을 출력
				PrintTotalLabelTicket(TRANS_TYPE_PAY_TOTAL_NOPAY,lastClerk,notRoundedPrice,0, discountPrice);
			else if(roundFlag == OFF)	//Round 처리 Flag가 OFF인 경우 Round 처리 안한 가격으로 출력
			{
				transPay.summaryround = 0;	
				PrintTotalLabelTicket(TRANS_TYPE_PAY_TOTAL_NOPAY,lastClerk,notRoundedPrice,0, discountPrice);
			}
			else
#endif
			PrintTotalLabelTicket(TRANS_TYPE_PAY_TOTAL_NOPAY,lastClerk,resultPrice,0, discountPrice);
			GlbFlagPCnMasterConnection = ON;
		}
	}
	END_RELEASE:
							curPluTemp = status_scale.cur_pluid;
	sale_pluclear(ON);
	sale_display_update(UPDATE_ALL|UPDATE_CLEARALL);
	PutXStringOffsetX = 0;
	PutXStringOffsetY = 0;
	if(Operation.wSave && Operation.wPrepack)	//save 와 prepack 동시 적용시 마지막 PLU 유지됨.
	{
		keyapp_pluno((INT8U)status_scale.departmentid,curPluTemp,OFF);
	}
}

void keyapp_add(INT16S beep)
{
	CAPTION_STRUCT cap;
	INT16U clerk_cnt;
	
	if(beep)	BuzOn(1);
	
	if(Operation.operationClerk==OPER_STD_CLERK || Operation.wPrepack)	// add 를 동작하지 않고 total sum에 대한 계산 flag를 설정한다
	{
		if(Operation.addStandatdClerk)
			Operation.addStandatdClerk = OFF;
		else
			Operation.addStandatdClerk = ON;
		if(Operation.addStandatdClerk) {
			caption_split_by_code(0xc040,&cap,0);
			//			strcpy(gmsg,"CLERK SUMMERY ON");
		} else {
			caption_split_by_code(0xc041,&cap,0);
			//			strcpy(gmsg,"CLERK SUMMERY OFF");
		}
		DisplayMsg(cap.form);
		return;
	}
	if(Operation.operationClerk==OPER_NORMAL && (Operation.wAuto || Operation.wPrepack))
		return;
	
#ifdef USE_QR_ADDTOTAL
	if(QrAddTotalFlag)
	{
		clerk_cnt = ClerkTransactionCnt(0);
		if(clerk_cnt >= MAX_QR_ADDCOUNT)
		{
			caption_split_by_code(0xC90D,&cap,0);
			DisplayMsg(cap.form);
			BuzOn(2);
			return;
		}
	}
#endif
	if(ClerkSearchEmpty() == -1) {
		caption_split_by_code(0xC90D,&cap,0);
		DisplayMsg(cap.form);
		Key_DelaySec(10,0);
		return;
	}
	//DipslayMsg("Add 1");
	if(!CheckPluPrint(0))	return;
	//DipslayMsg("Add 2");
	//if(!permission_check(PMS_ADDUP))	return;
	if(Operation.operationClerk == OPER_FLOATING)
	{
		caption_split_by_code(0xc904,&cap,0);
		DisplayMsg(cap.form);//"No sale in floating"
		return;
	}
	if(TareOper.addupWeightZero && PluType()==PLU_BY_WEIGHT)
	{
		if(ad_get_capa()< (TareOper.addupWeight+status_scale.Weight))
		{
			caption_split_by_code(0xc02a,&cap,0);
			DisplayMsg(cap.form);//"Overflow Addup weight"
			return;
		}
	}
	StartPrintFlag = ON;
	Operation.transAddup=ON;	// add
	//status_scale.clerkid = 0;
	status_scale.print_multiple=1;
	PrintLabelWait();
	Operation.directClearWeight = ON;
}

void display_message_page(char *str)
{
	POINT disp_p;
	char old_font;
	INT8U old_page;
	
	old_font = DspStruct.Id1;
	old_page = DspStruct.Page;
	Dsp_SetPage(DSP_ERROR_PAGE);
	DspLoadFont1(DSP_PLU_FONT_ID);
	Dsp_FontSize(DSP_PLU_NAME_MAGX, DSP_PLU_NAME_MAGY);
	
	//Dsp_Fill_Buffer(0);
	display_lcd_clear_buf();	// Clear buf PLU Name Area
	
	disp_p = point(DSP_PLU_NAME_Y, DSP_PLU_NAME_X);
#ifdef HEBREW_FONT
	Convert_HebrewPreview((INT8U *)str, strlen(str), 0, 0); 
	hebrew_codetemp[strlen(str)] = 0;
	disp_p.x = 202 - disp_p.x;
	Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
Convert_ArabPreview((INT8U *)str, strlen(str), 0, 0, 0); 
arab_codetemp[strlen(str)] = 0;
disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
	Dsp_Write_String(disp_p,(HUGEDATA char*)str);
#endif
	Dsp_Diffuse();
	display_lcd_diffuse();	// Redraw PLU Name Area
	Key_DelaySec(10, 0);
	Dsp_SetPage(old_page);
	Dsp_Diffuse();
	DspLoadFont1(old_font);
}

//extern INT16S FontSizeChange(INT16S fontSize);
void display_message_page_mid(char *str)
{
	POINT disp_p;
	char old_font;
	INT8U old_page;
	
#ifdef USE_CL5200_DISPLAY_DEFAULT  //_USE_LCD20848_
	Dsp_Clear();
#endif
	
	old_font = DspStruct.Id1;
	old_page = DspStruct.Page;
	Dsp_SetPage(DSP_ERROR_PAGE);
	FontSizeChange(FONT_MEDIUM);
	//DspLoadFont1(DSP_PLU_FONT_ID);
	//Dsp_FontSize(DSP_PLU_NAME_MAGX, DSP_PLU_NAME_MAGY);
	
	//Dsp_Fill_Buffer(0);
	display_lcd_clear_buf();	// Clear buf PLU Name Area
	
	disp_p = point(DSP_PLU_NAME_Y, DSP_PLU_NAME_X);
#ifdef HEBREW_FONT
	Convert_HebrewPreview((INT8U *)str, strlen(str), 0, 0); 
	hebrew_codetemp[strlen(str)] = 0;
	disp_p.x = 202 - disp_p.x;
	Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
	Convert_ArabPreview((INT8U *)str, strlen(str), 0, 0, 0); 
	arab_codetemp[strlen(str)] = 0;
	disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
	Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
	Dsp_Write_String(disp_p,(HUGEDATA char*)str);
#endif
	Dsp_Diffuse();
	display_lcd_diffuse();	// Redraw PLU Name Area
	Key_DelaySec(10, 0);
	Dsp_SetPage(old_page);
	Dsp_Diffuse();
	DspLoadFont1(old_font);
#ifdef USE_CL5200_DISPLAY_DEFAULT
	sale_display_update(UPDATE_ALL|UPDATE_CLEARALL);
#endif
}

void display_message_page_mid_delay(char *str, INT8U delay)
{
	POINT disp_p;
	char old_font;
	INT8U old_page;
	
	old_font = DspStruct.Id1;
	old_page = DspStruct.Page;
	Dsp_SetPage(DSP_ERROR_PAGE);
	FontSizeChange(FONT_MEDIUM);
	
	display_lcd_clear_buf();	// Clear buf PLU Name Area
	
	disp_p = point(DSP_PLU_NAME_Y, DSP_PLU_NAME_X);
#ifdef HEBREW_FONT
	Convert_HebrewPreview((INT8U *)str, strlen(str), 0, 0); 
	hebrew_codetemp[strlen(str)] = 0;
	disp_p.x = 202 - disp_p.x;
	Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
Convert_ArabPreview((INT8U *)str, strlen(str), 0, 0, 0); 
arab_codetemp[strlen(str)] = 0;
disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
	Dsp_Write_String(disp_p,(HUGEDATA char*)str);
#endif
	Dsp_Diffuse();
	display_lcd_diffuse();	// Redraw PLU Name Area
	Key_DelaySec(delay, 0);
	Dsp_SetPage(old_page);
	Dsp_Diffuse();
	DspLoadFont1(old_font);
}

//Added by JJANG 20080131 Print error Message
//Modified by JJANG 20080221 함수를 공용으로 사용할수 있게 변경.
INT8U display_message_printerr_page(INT16U L_msg, INT16U S_msg)
{
	Dsp_SetPage(DSP_ERROR_PAGE);
	Dsp_Fill_Buffer(0);
	
	caption_split_by_code(L_msg, &cap,0);
	//PutNLargeString(0,0,(char *)cap.form,20);
	PutNMediumString(0,0,(char *)cap.form,20);	// CL5200N의 경우 Large font 사용시 s_msg 내용이 표현 되지 않는 현상 발생.
	caption_split_by_code(S_msg, &cap,0);
	PutNSmallString(2,0,(char *)cap.form,35);	
	
	Dsp_Diffuse();
	while(1) 
	{
		while(!KEY_Read());
		switch(KeyDrv.CnvCode) 
		{
			case KEY_NUM_1 :
			case KP_ASC_Y :
				BuzOn(1);
				Dsp_SetPage(DSP_DEFAULT_PAGE);
				Dsp_Diffuse();
				return 1;
			case KEY_NUM_0 :
			case KP_ASC_N :
			case KP_ESC :
				BuzOn(1);
				Dsp_SetPage(DSP_DEFAULT_PAGE);
				Dsp_Diffuse();
				return 0;
			default :
				BuzOn(2);
				break;
		}
	}
}

//Added by JJANG 20090421, 문자열 받아서 찍어주는 함수 
INT8U display_message_check_page(char *S_msg1, char *S_msg2, INT8U mode)
{
	Dsp_SetPage(DSP_ERROR_PAGE);
	Dsp_Fill_Buffer(0);
	
	if (mode == 1)
	{
		PutNSmallString(1,0,(char *)S_msg1,35);
		PutNSmallString(2,0,(char *)S_msg2,35);
	}
	else if (mode == 2)
	{
		PutNMediumString(0,0,(char *)S_msg1,30);
		PutNMediumString(2,0,(char *)S_msg2,30);
	}  
	Dsp_Diffuse();
	while(1) 
	{
		while(!KEY_Read());
		switch(KeyDrv.CnvCode) 
		{
			case KEY_NUM_1 :
			case KP_ASC_Y :
				BuzOn(1);
				Dsp_SetPage(DSP_DEFAULT_PAGE);
				Dsp_Diffuse();
				return 1;
			case KEY_NUM_0 :
			case KP_ASC_N :
			case KP_ESC :
				BuzOn(1);
				Dsp_SetPage(DSP_DEFAULT_PAGE);
				Dsp_Diffuse();
				return 0;
			default :
				BuzOn(2);
				break;
		}
	}
}

#ifdef USE_HYUNDAI_CHECK_REASON_CODE
INT8U check_reason_code(INT8U code)
{
	INT8U result;
	result = FALSE;

	switch(code){
		case TRANS_PRICE_DC_EVENT_COMMON :
		case TRANS_PRICE_DC_EVENT_MEMBER :
		case TRANS_PRICE_DC_EVENT_STORE :
		case TRANS_PRICE_DC_EVENT_FRESHNESS_LOSS :
		case TRANS_PRICE_DC_EVENT_CLERK :
  #ifdef USE_REGIST_DISPOSAL
			if (Operation.RegistDisposalFlag) result = FALSE;
			else 
  #endif
				result = TRUE;
			break;
		case TRANS_DISPOSAL_RETURN :
		case TRANS_DISPOSAL_FRESHNESS_LOSS :
		case TRANS_SAMPLE :
  #ifdef USE_REGIST_DISPOSAL
			if (Operation.RegistDisposalFlag) result = TRUE;
			else 
  #endif
				result = FALSE;
			break;
		case TRANS_PRICE_DC_NONE :
		case TRANS_MEAT_SET :
		default : 
			break;
	}

	return result;
}
#endif

INT8U 	Send_reason_Code = 0;
INT8U display_reason_code_input_page(void)	//Added by JJANG 20090528
{
	INT8U   result,old_font,key_mode;
	INT32U	res = 0;
	INT8U	ret;
	INT8U old_page;
#ifdef USE_HYUNDAI_CHECK_REASON_CODE
	INT8U buf[20];

	old_page = DspStruct.Page;

	Dsp_Fill_Buffer(0);
	key_mode=KEY_GetMode();
	KEY_SetMode(1);
	old_font=DspStruct.Id1;
	DspLoadFont1(DSP_MENU_FONT_ID);

	while(1){
		Menu_Init();

		caption_split_by_code(0x1BD2, &cap,0);		
		cap.input_dlength = 1;
		cap.input_length = 1;
		cap.input_min = 0;
		cap.input_max = 9;
		res = status_scale.Plu.sales_msg;
		caption_adjust(&cap,NULL);
		Menu_InputCreate(1, ( char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
					cap.input_max, cap.input_min, 0, &res, NULL, NULL, NULL);

		result = Menu_InputDisplay();
		if (result == MENU_RET_SAVE)
		{
			if (check_reason_code(res)) {
				status_scale.Plu.sales_msg = res;
				ret = ON;
				break;
			}
			else {
				BuzOn(2);
				sprintf((char *)buf, "사유코드 확인!");
				display_message_page_mid((char *)buf);
				ret = OFF;
			}
		}
		else if (result == MENU_RET_ESC){
			ret = OFF;
			break;
		}
	}
#else	
	old_page = DspStruct.Page;
	
	Dsp_SetPage(DSP_ERROR_PAGE);
	Dsp_Fill_Buffer(0);
	
	key_mode=KEY_GetMode();
	KEY_SetMode(1);
	old_font=DspStruct.Id1;
	DspLoadFont1(DSP_MENU_FONT_ID);
	
	Menu_Init();
	
	caption_split_by_code(0x1BD2, &cap,0);
	caption_adjust(&cap,NULL);
	Menu_InputCreate(1, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
			cap.input_max, cap.input_min, 0, &res, NULL, NULL, NULL);
	
	result = Menu_InputDisplay();
	if (result == MENU_RET_SAVE) 
	{
		Send_reason_Code = (INT8U)res;	
		ret = ON;
	}
	else if (result == MENU_RET_ESC) ret = OFF;
#endif	
	Dsp_Fill_Buffer(0);
	Dsp_SetPage(old_page);
	Dsp_Diffuse();
	DspLoadFont1(old_font);
	KEY_SetMode(key_mode);
	sale_display_update(0x0fff);
	
	return ret;
}
#ifdef USE_CONTROL_CHANGING_PRICE

//divisor 1 : 쪼개기 없음, 2 : 1/2 쪼개기, 3 : 1/3 쪼개기, 4 : 1/4 쪼개기, else는 1로 고정됨.
//return 0: 매가 변경 허용, 1: 상한 넘김, 2: 하한 넘김
INT8U check_changed_price_range(INT8U divisor, long* upperPrice, long* lowerPrice)
{
	INT16U pluIndex;
	INT32U basePrice;
	INT16S size;
	INT8U splitTemp;
	plu_table_search(status_scale.departmentid, status_scale.cur_pluid, &pluIndex, 0);
	if(pluIndex == 0)
	{
		return 0;
	}
	basePrice = status_scale.Plu.special_price;	//special price를 기준 가격으로 사용. 
	if(!basePrice) return 0;		//기준가격이 없는 경우 매가 변경 허용?
	if(status_scale.Plu.ctrlPriceChange == 1)		//매가 통제 함.
	{
		if(divisor < 5 && divisor > 1)
		{
			splitTemp = divisor;
		}
		else
		{
			splitTemp = 1;
		}
		*upperPrice = ((100 + status_scale.Plu.upperLimit) * basePrice) / splitTemp / 100; //기준매가 소수점 절사함
		*lowerPrice = ((100 - status_scale.Plu.lowerLimit) * basePrice) / splitTemp / 100; //기준매가 소수점 절사함
		if(Price.UnitPrice[PRICE_RESULT] > *upperPrice)	
		{
			return 1;
		}
		else if(Price.UnitPrice[PRICE_RESULT] < *lowerPrice)
		{
			return 2;
		}
		else
		{
			return 0;	
		}
	}
	else	//매가 통제 안 함
	{
		return 0;
	}
}
#endif
INT8U MultiPrintFlag = OFF;
INT8U Multi_PeelEnable;
INT8U Multi_RewindEnable;
INT8U keyapp_print(INT16S beep)
{
	INT8U ret_value = 0;
	INT8U	changeFlag;
	char S_msg1[32];
	char S_msg2[32];
	char string_buf[50];
#if defined(USE_CHECK_CHANGE_REASON) || defined(USE_HYUNDAI_CHECK_REASON_CODE)
	INT8U askReason;
#endif
#ifdef USE_IMPORT_MEAT_PROTOCOL
#ifdef USE_EXT_CHECKING_HARMFULNESS
	INT8U use_rs232;
	INT8U flag;
	HUGEDATA COMM_BUF *CBuf;
	
	use_rs232 = OFF;
	flag=get_global_bparam(GLOBAL_SALE_SETUP_FLAG4);
	if( flag&0x80 ) use_rs232 = ON;
	if (!use_rs232) CBuf = &CommBufEthData[0];
	else CBuf = &CommBuf;
#endif
#endif
	
#ifdef USE_INDIA_FUNCTION
	if (UseAreaScaleMode == 2) // 0:NormalScale(STD), 1:DualModeScale, 2:AreaScale
	{
		BuzOn(2);
		return OFF;
	}
#endif
	
	if (beep)	BuzOn(1);
#ifdef USE_TRACE_STANDALONE
	if (FlagLotMultiStart) 
	{
    	Prt_LotList(0);
    	MultiIndex = 0;
		LotListMultiIndex = 0;
		FlagMultiStart = 0;//묶음장부 인쇄 후 복수개체 시작키로 초기화
		FlagLotMultiStart = OFF;
	    	return 0;
	}
#endif
	
	if(Startup.country == COUNTRY_US && !GlbPrint.printNoRewind)	//Added by JJANG 20090421, 미국전용, 여러장 발행 시 임시로 센서와 모터를 끌 수 있다.
	{
		if(Operation.multiply)
		{
			strcpy(S_msg1,"DO YOU WANT TO DISABLE PEEL OFF");
			strcpy(S_msg2,"& REWIND MOTOR? (1=Y/0=N)");
			ret_value = display_message_check_page(S_msg1,S_msg2,1);
			if (ret_value)
			{
				MultiPrintFlag = ON;
				Operation.rewindReady = ON;
				Multi_PeelEnable = PrtDrvStruct.PeelEnable;
				Multi_RewindEnable = PrtDrvStruct.RewindEnable;
				PrtDrvStruct.PeelEnable = DISABLE;
				PrtDrvStruct.RewindEnable = DISABLE;
			}
		}
	}
	if ((Operation.operationClerk==OPER_STD_CLERK && Operation.addStandatdClerk) || Operation.operationClerk==OPER_FLOATING)
	{
		if(Operation.multiply)	return 0xa1;
		keyapp_total(ON);
		return 0xa2;
	}
	else
	{
		if(PrtStruct.Mode == RECEIPT)//new
		{
#ifdef USE_CHN_CART_SCALE
			Operation.oneTouchTicketPrint = ON;
			keyapp_add(OFF);
			return 0;
#else
			caption_split_by_code(0xC80C,&cap,0);//"Select label mode"
			DisplayMsg(cap.form);
			return 0xa3;
#endif
		}
	}
	if(ADM_GetData.Overload || PercentTareOver) return 0xa4;// (For NMi) Added by CJK 20050713
	DirectWeightMode();
	DirectCountMode();  //SG070227
	
	if(!CheckPluPrint(0))	 return 0xa5;
	
#ifdef USE_IMPORT_MEAT_PROTOCOL
#ifdef USE_EXT_CHECKING_HARMFULNESS
	if(FlagDisplayIndividual)
	{
		if(command_import_meat_data(CBuf, use_rs232))
		{
			BuzOn(4);
			//sprintf(string_buf, "위해 쇠고기입니다");
			sprintf(string_buf, "담당자 확인 필요 상품");
			display_message_page_mid_delay(string_buf, 50);
			sale_display_indivFlag_set(INDIV_DISP_MODE_INDIV_NO, OFF);
			sale_display_update(UPDATE_ALL|UPDATE_CLEARALL);
			return OFF;
		}
	}	
#endif
#endif
#ifdef USE_CHECK_INDIV_INFO
	if(!CheckIndivInfo()) return OFF;
#endif
	//chg_flag = get_global_bparam(GLOBAL_SALE_SETUP_FLAG8) & 0x40;	//parameter 723
#ifdef USE_CHECK_CHANGE_REASON
	askReason = get_global_bparam(GLOBAL_SALE_SETUP_FLAG12) & 0x10;
	if(askReason)
	{
		changeFlag = GetChangeFlag(PluType(), status_scale.Plu.ptype, Price.UnitPrice[PRICE_RESULT], status_scale.Plu.unitprice, Operation.fixedPrice, status_scale.Plu.fixed_price, Price.TotalPrice[PRICE_SAVED]);
		if (UseEmartNetwork && changeFlag)
		{
			if(!display_reason_code_input_page()) return OFF;
		}
	}
#elif defined(USE_HYUNDAI_CHECK_REASON_CODE)
	askReason = get_global_bparam(GLOBAL_SALE_SETUP_FLAG12) & 0x10;
	if(askReason && (status_scale.Plu.sales_msg != 9) && !Operation.meatStorageLabelFlag)
	{
		changeFlag = GetChangeFlag(PluType(), status_scale.Plu.ptype, Price.UnitPrice[PRICE_RESULT], status_scale.Plu.unitprice, Operation.fixedPrice, status_scale.Plu.fixed_price, Price.TotalPrice[PRICE_SAVED]);
		if (changeFlag || Operation.RegistDisposalFlag) {
			if(!display_reason_code_input_page()) return OFF;
		}
	}
#endif
	
	Operation.transAddup = OFF;
	if (Operation.addStandatdClerk )
	{
		if(ClerkSearchEmpty() == -1) {
			caption_split_by_code(0xC90D,&cap,0);
			DisplayMsg(cap.form);
			Key_DelaySec(10,0);
			return 0xa7;
		}
		Operation.transAddup = ON;
	}
	if(Operation.multiply)	// Multi print
	{
		if (Operation.multiplyEdit && status_scale.print_multiple > 0)
		{
			Operation.multiplyEdit = OFF;
			//status_scale.print_multiple = status_scale.cur_keyvalue;
		}
		else
		{
			return 0xa6;
		}
	}
	else
	{
		if(GlbOper.autoDoublePrint && Operation.wAuto)//OPTION: double print
		{
			status_scale.print_multiple = GlbOper.autoDoublePrint;                     //yoo 080605
			Operation.autoDoublePrint = ON;
		}
		else
			status_scale.print_multiple = 1;
	}
	Operation.salePluClearWait = OFF;	// Added by CJK 20060425
	StartPrintFlag = ON;
	if(status_scale.print_multiple)
		PrintLabelWait();
	if(GlbPrint.printNoRewind && PrtFlashStruct.RewindEnable && Operation.multiply && 1 < status_scale.print_multiple)
	{
		char	string_buf[20];
		Operation.rewindReady = ON;
		sprintf(string_buf, "%cM", GS);
		PutDataRxBuf(&CommBufPrt,string_buf,2);
		string_buf[0] = OFF;
		PutDataRxBuf(&CommBufPrt,string_buf,1);
	}
	//	if (Operation.salePluClearWait==OFF) {
	//		keyapp_cleardiscount();
	//		if (status_scale.Plu.fixed_price==0) Operation.fixedPrice=0;
	//	}
	
	return 0;
}

extern INT8U	ClerkLastIndex;
extern INT8U	ClerkLastNumber;
extern INT32U	ClerkLastHead;
extern PRINT_OPER_TYPE		PrtOper;	// Added by CJK 20051226

//DEL070705extern void sale_display_multiply(INT16S dispFlsg);
void keyapp_multi(void)
{
	
#ifdef USE_KOR_TRACE	 //SG060410
	if(network_status.service_bflag2){
		BuzOn(2);
		return;
	}
#endif

	if(!Operation.multiply && Operation.negative)
	{
		caption_split_by_code(0xc909,&cap,0);
		DisplayMsg(cap.form);//"No X key on negative"
		return;
	}
	
	if(Operation.autoDoublePrint && Operation.wAuto)
	{
		Operation.multiply = OFF;
		caption_split_by_code(0xC90B,&cap,0);//"Double print(auto)"
		DisplayMsg(cap.form);
		return;
	}
	
	if(!GlbOper.byWeightMulti)
	{
		//if(!CheckPluPrint())	return;	// Deleted by CJK 20051031
		if(status_scale.cur_pluid && PluType()==PLU_BY_WEIGHT)
		{
			Operation.multiply = OFF;
			return;
		}
	}
	
	if(Operation.multiply) 
	{
		//		if(status_scale.print_multiple > 0) return;
		Operation.multiply = OFF;
		status_scale.print_multiple = 0;
		PrtDrvStruct.PrtLoadFormFlag=ON;
		label_load(status_scale.cur_labelid);
		Operation.salePrintWait = OFF;
		//DEL070705		sale_display_multiply(OFF);
		//DEL070705		sale_display_discount(ON);
	} 
	else 
	{
		if(!status_scale.cur_pluid)
		{
			////DEL090910//			INT8U	byte;
			////DEL090910
			////DEL090910			//byte = get_global_bparam(GLOBAL_REPORT_FLAG);//599 Print last ticket
			////DEL090910			//if(OnOff(byte&0x10) && ClerkLastIndex && ClerkLastNumber)	// floating
			////DEL090910#ifndef USE_TRACE_STANDALONE	// 개체기억기능 영역공유
			////DEL090910			if(status_scale.clerkid && PrtOper.PrtLastTicket)
			////DEL090910			{
			////DEL090910				if(ClerkLastIndex && (ClerkLastNumber == status_scale.clerkid))	// floating
			////DEL090910				{
			////DEL090910					//new
			////DEL090910					// Modified by CJK 20041112
			////DEL090910					//PrintClerkLastTicket();
			////DEL090910					Prt_PrintStart(PRT_TOTAL_LAST_MODE, PrtStruct.Mode, ClerkLastNumber, ON, 0, 0);
			////DEL090910				}
			////DEL090910				else
			////DEL090910				{
			////DEL090910					caption_split_by_code(0xC90E,&cap,0);//"Copy ticket is erased"
			////DEL090910					DisplayMsg(cap.form);
			////DEL090910				}
			////DEL090910			}
			////DEL090910#endif
			return ;
		}
		//sale_display_multiply(OFF);
		//DEL070705		sale_display_multiply(ON);
		
		//Added by JJANG 20080602
		//네덜란드 요청, floating clerk일 때, 'X' key 동작 안하도록 수정
		if(Operation.operationClerk == OPER_FLOATING)
		{
			Operation.multiply = OFF;
			return;
		}
		
		Operation.multiply = ON;
		Operation.multiplyEdit = ON;
		status_scale.cur_keyvalue=status_scale.print_multiple;
	}
	sale_display_update(UPDATE_TITLE);
}

void keyapp_sale_special(void)
{
	//BuzOn(1);
	
	if (status_scale.cur_pluid==0) return;
	if (GlbOper.useSpecialPriceForDisc) return;
#ifndef USE_TOPMART_STRUCTURE
	if(status_scale.Plu.special_price)
#endif
	{
		Operation.specialPrice = ON;
	}
	
	sale_display_update(UPDATE_TITLE);
	
}
#ifndef USE_PLU_MULTIPLE_PRICE	//Refefence PLU 기능 사용 안 함
#ifndef USE_AUSTRALIA_COOL_FUNCTION //Refefence PLU 기능 사용 안 함
void keyapp_reference(void)
{
	INT8U	dept;
	INT32U	plu;
	
	if (Startup.disable_dept) {
		status_scale.Plu.refer_dept=1;
	}
	if(status_scale.Plu.refer_dept && status_scale.Plu.refer_plu)
	{
		dept = status_scale.Plu.refer_dept;
		plu =  status_scale.Plu.refer_plu;
		sale_pluclear(ON);
		sale_display_update(0x0fff);//061128
		keyapp_pluno(dept,plu,OFF);
	}
	else
	{
		caption_split_by_code(0xc901,&cap,0);//"No reference"
		DisplayMsg(cap.form);
	}
}
#endif
#endif


void keyapp_sale_negative(void)
{
	//	char	priceBuf[15];
	
	//BuzOn(1);
	//if(Operation.operationClerk!=OPER_FLOATING)	return;
	if(!CheckPluPrint(0))	return;
	// if(!Operation.negative)
	//{
	Operation.negative = ON;
	caption_split_by_code(0xc02b,&cap,0);
	DisplayMsg(cap.form);//"Negative ON"
	//	}
}

extern long	WeightMaxDelta;
//extern INT8U PLU_scroll_onoff;
extern PLU_SCROLL_STRUCT PLU_SCROLL;
#ifdef USE_DONT_SAVE_VALIDDATE_SALEDATE
extern TEMP_SELLDATE_STRUCT	temp_selldate;
#endif

void sale_pluclear(INT16S keyFlag)
{
	//	if (status_scale.cur_pluid==0){
	//		status_scale.cur_qty      = 1;
	//		status_scale.cur_pcs      = 1;
	//	}
	
			if (Clerk_Subtotal_List == 1)
			{
				Clerk_Subtotal_List = 0; //Modify by se-hyung for Clerk Subtotal List 20070727
				sale_display_update(UPDATE_SUBTOTAL);
			}
	if (status_scale.clerkid) NetClerkLockRelease(status_scale.clerkid);
	
	status_scale.weight_stable = 0;
	WeightMaxDelta = 0;
	
	status_scale.cur_ride     = 0;
	status_scale.ride_max     = 1;
	
	status_scale.cur_pluid = 0;
	status_scale.flag_input &=0xf1;
	//DEL070130        status_scale.sale_type = 0;
	//DEL070130	status_scale.sale_value= 0;
	
	status_scale.cur_unitprice= 0;
	status_scale.cur_weightunit=get_defaultunit(CurPluWeightUnit);
	status_scale.cur_qty= 0;
	status_scale.cur_pcs= 0;
#ifdef USE_PERCENT_WEIGHT
	status_scale.cur_ptare= 0;
#endif
#if defined(USE_GSMART_BARCODE) || defined(USE_PRT_CHNIA_DISCOUNT_RATE)
  #ifdef USE_BESTCO_DISCOUNT_DIVISION
	status_scale.discountflag = 12;
	status_scale.percent_discount_flag = 0;
  #else
	status_scale.discountflag = 0;
  #endif /* USE_BESTCO_DISCOUNT_DIVISION */
	status_scale.discount_rate = 0;
#endif
	status_scale.divisor = 0;
	Operation.useMultipleUnits = 0;
	Price.TotalPrice[0]=0;
	Price.TotalPrice[1]=0;
	Price.TotalPrice[2]=0;
	Price.TaxRate	= 0;
	Price.TaxPrice	= 0;
	Price.TaxNumber	=0;
	Price.DisplayedPrice = 0;
	//Price.RoundedPrice = 0;
	Price.FixedDiscountPrice = 0;
	// quick plu에서 숫자 입력중 없는 PLU가 호출될 때, 
	// 단가가 0으로 되는것을 막기위해 InputQuickPluState가 0일때만 key값을 clear하게 함
	if(!InputQuickPluState) status_scale.cur_keyvalue=0;	
	status_scale.Plu.tare  = 0;
	if(status_scale.Plu.fixed_weight)
		status_scale.Weight=0;
	status_scale.Plu.fixed_weight = 0;
    status_scale.Plu.group = 0;
	
	status_scale.linked_plu[0] = 0;
	status_scale.linked_plu[1] = 0;
	
	//DEL051229	Operation.keyInWeight = OFF;
	Operation.specialPrice = OFF;
	Operation.specialPriceDisc = OFF;
	Operation.fixedPrice = OFF;
	Operation.negative = OFF;
	Operation.fixedDiscFlag = OFF;
	Operation.insertLabelData = OFF;
	Operation.discountInfo = OFF;
	Operation.fixedAction=0;
#ifdef USE_COPYKEY	
	flagcopykey = OFF;
#endif
	PLU_SCROLL.state = PLU_SCROLL_TURNOFF;
#ifdef USE_NHMART_SAFE_MEAT
	network_status.indivCallCts = 0;
#endif
	// qty, pcs, unitprice 편집시 blink. 숫자키를 누를 때 마다 값이 표시되도록 함
	Set7BlinkPrevTime = SysTimer100ms;
	Seg7BlinkOn = 0;
	
	ClearCurrencyPrice();
	// 무조건 off
	//if(Operation.operationMode != MODE_MANAGER)//OPTION: manager mode가 아니면
	{
		Operation.override = OFF;
		Netweight_flag=OFF;
	}
	//sale_discount();
	// batch 작업이 없을때 reference를 수행
#ifdef USE_CHN_EMART_TRACE
	CHNEmart_TraceSelectionOrder = 0;
	CHNEmart_CurTraceCode[0] = 0;
#endif
#ifndef USE_PLU_MULTIPLE_PRICE	//Refefence PLU 기능 사용 안 함
	if(keyFlag)
	{
		status_scale.Plu.coupled_dept = 0;
		status_scale.Plu.coupled_plu = 0;
		status_scale.Plu.refer_dept = 0;
		status_scale.Plu.refer_plu = 0;
	}
	
	if (Startup.disable_dept) {	// Added by CJK 20050809
		status_scale.Plu.coupled_dept = 1;
	}
#endif
	
	//SG070322	if(!BatchCount && status_scale.Plu.coupled_dept && status_scale.Plu.coupled_plu)
	//SG070322	{
	//SG070322		if(keyFlag)
	//SG070322		{
	//SG070322			BatchCount	= 0;
	//SG070322		}
	//SG070322		else
	//SG070322		{
	//SG070322			BatchCount	= 1;
	//SG070322			Batch[0].dept = status_scale.Plu.coupled_dept;
	//SG070322			Batch[0].plu =  status_scale.Plu.coupled_plu;
	//SG070322			BatchIndex = 0;
	//SG070322		}
	//SG070322	}
	
	TareClearPLU();
	// return key tare
	if(TareOper.saveKeyTareForPLU && TareOper.saveKeyTare)// continuous tare일때
	{
		keyapp_tare(TARE_TYPE_KEY,TareOper.saveKeyTare,OFF);
		TareOper.saveKeyTare = 0;
	}
	
	//no clerk mode에서는 plu clear 시점에서 level 권한 부여를 없앤다
	if(Operation.operationMode != MODE_MANAGER && Operation.operationClerk==OPER_NORMAL)
		logout();
	//display_alloff_primary_indication();// override
	sale_display_update(UPDATE_TARE|UPDATE_WEIGHT|UPDATE_UNIT|UPDATE_PRICE);
	//DEL070705	sale_display_discount(ON);
	display_tare(1);
	//sale_display_update(UPDATE_CENTER|UPDATE_TITLE|UPDATE_UNIT|UPDATE_PRICE|UPDATE_TARE);

#ifdef USE_KOR_TRACE
	//sale_display_kortrace(OFF);
	KorTrace_Flag.flagReceive = 0;
	sale_display_update(UPDATE_TITLE);	// CJK070705
#endif

#ifdef USE_TRACE_STANDALONE
	sale_display_indivFlag_set(INDIV_DISP_MODE_ALL, OFF);
	sale_display_update(UPDATE_TITLE);	// CJK070705
#elif defined(USE_CHN_CART_SCALE)
	// current trace clear
	memset((INT8U *)&CurCHNTrace, 0, sizeof(TRACE_STRUCT));
	CurCHNTraceRemainWeight = 0;
	CurCHNTraceLotNo = 0;
#endif
	
	InputQuickPluState = OFF;
	if (GlbOper.EnableQuickPlu) InputQuickPluState = ON;
	//sale_display_update(0x0fff);	// Del by CJK 20061128
	
	DispUpdateAll = 1;
#ifdef USE_DONT_SAVE_VALIDDATE_SALEDATE
	temp_selldate.onoff=OFF; 
#endif
#ifdef USE_PRICE_CHANGE_PROHIBITED
	PressedNumberKey_1000Digits = 0;
    status_scale.Plu.sales_msg = 0;
   // packed_date = 0; //영업사원 삭제 요청-> PLU 클리어되도 재포장일 유지 1회만 인쇄.
#endif
}

//DEL070705extern void	DispClearPeelHeadMsg(void);
extern char	PeelHeadMsgFlag;
extern long InventoryUnitWeight;
void keyapp_clear(void)
{
	INT8U smode,clearFlag=OFF;
	//BuzOn(1);
	
	//smode=(status_scale.flag_input>>1)&0x07;
	smode=PluType();
#if !defined(_USE_LCD20848_) && !defined(_USE_LCD32_)
	if (DateTimeDispTime) // date/time 표시되고 있을 때
	{
		DateTimeDispTime = 0;
		return;
	}
#endif
	if(BatchCount)	BatchCount=0;
	
	if (Operation.multiply || Operation.autoDoublePrint)
	{
		//sale_print_cancel();
		if (status_scale.print_multiple && KeyDrv.PrevType==KEY_TYPE_NUMERIC)
		{
			status_scale.print_multiple  = 0;
		}
		else
		{
			Operation.multiply=OFF;
			Operation.autoDoublePrint = OFF;
			//DEL070705	    	sale_display_multiply(OFF);
			status_scale.print_multiple  = 0;
#ifndef USE_PLU_MULTIPLE_PRICE	//Refefence PLU 기능 사용 안 함
			if(status_scale.Plu.refer_plu)	status_scale.Plu.refer_plu=0;
#endif
			ClearAndStart();
		}
		sale_display_update(UPDATE_TITLE|UPDATE_PRICE|UPDATE_MODE|UPDATE_STATUS);
		return;
	}
	else if(PeelHeadMsgFlag)
	{
		//DEL070705		DispClearPeelHeadMsg();
		CheckPeelHead(OFF, ON);
		Operation.salePrintWait=OFF;
		status_scale.print_multiple  = 0;
	}
	//RESTART:
	if (InputQuickPluState)
	{
		status_scale.cur_quickplu = 0;
		clearFlag=ON;
	}
	else if(Operation.fixedPrice)
	{
		if(Operation.override)
		{
			if(status_scale.cur_ride)
			{
				if (status_scale.cur_unitprice==0) clearFlag=ON;
				else status_scale.cur_unitprice=0;
			}
		}
		else
			clearFlag = ON;
		//DEL070130		if(clearFlag)
		//DEL070130		{
		//DEL070130//			ChangePluType(PLU_BY_WEIGHT);
		//DEL070130//			sale_display_update(UPDATE_WEIGHT);
		//DEL070130//			display_ulongsimple(DISPLAY_WEIGHT,Price.Weight[PRICE_RESULT]);
		//DEL070130			sale_pluclear(ON);
		//DEL070130			sale_display_update(0x0fff);//061128
		//DEL070130		}
	}
	else
	{
		switch (smode) {
			case 0:
				clearFlag = ON;
				//DEL070130				if(TareOper.canadianTare && !Operation.transactionOK && status_scale.Tare)
				//DEL070130				{
				//DEL070130					MsgCanadianTare();
				//DEL070130					return;
				//DEL070130				}
				//DEL070130				sale_pluclear(ON);
				//DEL070130				sale_display_update(0x0fff);//061128
				//DEL070130	//			sale_display_update(UPDATE_CENTER|UPDATE_TITLE|UPDATE_UNIT|UPDATE_PRICE|UPDATE_TARE);
				//DEL070130	//			if(status_scale.cur_keyvalue) {
				//DEL070130	//				status_scale.cur_keyvalue = 0;
				//DEL070130	//				sale_display_update(UPDATE_CENTER|UPDATE_UNIT|UPDATE_PRICE);
				//DEL070130	//			}
				//DEL070130				sale_display_update(UPDATE_MODE|UPDATE_STATUS|UPDATE_RIDE);
				break;
			case PLU_BY_WEIGHT:
				if(Operation.override)
				{
					Netweight_flag=OFF;
					if(status_scale.cur_ride)
					{
						if (status_scale.cur_unitprice==0) clearFlag=ON;
						else status_scale.cur_unitprice=0;
					}
					Operation.useMultipleUnits = 0;	// Added by CJK 20060412
					status_scale.cur_weightunit=get_defaultunit(ADM_GetData.CurUnit);
				}
				else
					clearFlag=ON;
				break;
			case PLU_BY_PCS: switch (status_scale.cur_ride) {
				case 1: if (status_scale.cur_qty==0) clearFlag=ON;
				else status_scale.cur_qty=0;
				break;
				case 2: if (status_scale.cur_pcs==0) clearFlag=ON;
				else status_scale.cur_pcs=0;
				break;
				case 3: if (status_scale.cur_unitprice==0) clearFlag=ON;
				else status_scale.cur_unitprice=0;
			}
			break;
			case PLU_BY_COUNT: switch (status_scale.cur_ride) {
				case 1: if (status_scale.cur_qty==0) clearFlag=ON;
				else status_scale.cur_qty=0;
				break;
				case 2: if (status_scale.cur_unitprice==0) clearFlag=ON;
				else status_scale.cur_unitprice=0;
			}
			
		}
	}
	//	sale_display_update(UPDATE_WEIGHT|UPDATE_MODE|UPDATE_UNIT);
	//	return;
	//SMODE_EXIT:
	if(clearFlag)
	{
		if(DiscSale.saleGlobalOk){
			keyapp_cleardiscount();
#ifndef USE_PLU_MULTIPLE_PRICE
			return;		//Direct Discount 경우 PLU 호출 한번에 취소되어야 함
#endif
		}
		
		if(TareOper.canadianTare && !Operation.transactionOK && status_scale.Tare)
		{
			MsgCanadianTare();
			return;
		}
#ifndef USE_PLU_MULTIPLE_PRICE	//Refefence PLU 기능 사용 안 함
		if(status_scale.Plu.refer_plu)	status_scale.Plu.refer_plu=0;
#endif
#ifdef USE_INVENTORY_LABEL
		InventoryUnitWeight = 0;
#endif

#ifdef USE_KOR_TRACE
		KorTrace_Flag.flagReceive = 0;
		//sale_display_kortrace(OFF);	// Added by CJK 20060310
		sale_display_update(UPDATE_TITLE);	// CJK070705
#endif
	
#ifdef USE_TRACE_STANDALONE
		sale_display_indivFlag_set(INDIV_DISP_MODE_ALL, OFF);
		sale_display_update(UPDATE_TITLE);	// CJK070705
#endif
		
		InputQuickPluState = OFF;
		if (GlbOper.EnableQuickPlu) InputQuickPluState = ON;
		sale_pluclear(ON);
#ifdef USE_TRACE_STANDALONE
		indivMultiAndLotIndexInit();
#endif
		sale_display_update(0x0fff);//061128
	}
	sale_display_update(UPDATE_WEIGHT|UPDATE_MODE|UPDATE_UNIT|UPDATE_STATUS);
	//	goto RESTART;
}

void keyapp_led(void)
{
}

INT16S keyapp_nzero(void)
{
	INT16S r,n;
	//BuzOn(1);
	n=(INT16S)get_global_bparam(GLOBAL_KEY_NUMBERZERO_N);
	if (n>4) n=4;
	r=power10(n); // <=1000
	if (r>10000) {
		r=100;
		n=2;
	}
	KeyDrv.CnvCode=r;
	//	key_numeric();
	//	for (i=0; i<n; i++) {
	//		KEY_Write(status_scale.code00,0);
	//	}
	return n;	// Added by CJK 20040722
}

void keyapp_zero(void)
{
#ifdef USE_SRILANKA_ZERO
	if(ad_set_zero() != 0) BuzOnAdd(1);
#else
	//BuzOn(1);
	ADM_SendCommand(ADM_CMD_SET_ZERO, 0, ADM_PARAM_OFF);	// Set Zero
#endif
}

//void keyapp_keytare(void)
//{
//	tare=0;
//	ADM_SendCommand(ADM_CMD_SET_TARE, 0, ADM_PARAM_OFF);	// Set Tare
//}

//INT8U preset_tare(long preset,INT8U reset)
//{
//	INT8U ret;
//	if (reset) {
//	    preset=0;
//	    ret=ad_set_presettare(preset);
//	} else {
//	    if (status_scale.Plu.tare) {
//		if (ad_get_tare_state()) return -1;
//		else {
//			ret=ad_set_presettare(preset);
//		}
//	    }
//	}
//	status_scale.Tare=preset;
//	keyapp_tare_error(ret);
//	return ret;
//}

void keyapp_tare_error(INT8U ret)
{
	INT16U capcode;
	
	if (ret==0) return;
	switch (ret) {
		case 1: capcode=0xE001; break;// Tare Overrange(+,-)
		case 2: capcode=0xE002; break;// Invalid Value
		case 3: capcode=0xE003; break;// Unstable
		case 4: capcode=0xE004; break;// Already Tare
		case 5: capcode=0xE005; break;// No Tare
		case 9: capcode=0xE009; break;// Communication Error
	}
	//		if ((keytare==0) && (ret==4)) goto END;
	BuzOn(2);
	sale_display_update_error(capcode,0,0);
}

//tare
INT16S TareClearKey(void)
{
	INT16S stat;
	
	if(TareOper.tareKey)
	{
		if(TareOper.flagAutoClearTare)	return ON;
		stat = keyapp_tare(TARE_TYPE_KEY,0, OFF);
		if(stat)	TareOper.flagAutoClearTare = ON;
		return ON;
	}
	
	return OFF;
}

INT16S TareClearWeight(void)
{
	INT16S stat;
	
	if(!TareOper.clearWeightTare)	return OFF;
	if(status_scale.cur_pluid)	return OFF;
	if(TareOper.canadianTare && !Operation.transactionOK)	return OFF;
	if(!status_scale.Tare)	TareOper.flagAutoClearTare = ON;
	if(TareOper.flagAutoClearTare)	return ON;
	
	if(TareOper.tareWeight && (TareOper.tareWeight+status_scale.Weight) == 0)
	{
		//keyapp_tare(TARE_TYPE_WEIGHT,status_scale.Weight, OFF);
		stat = keyapp_tare(TARE_TYPE_WEIGHT,0, OFF);
		if(stat)	TareOper.flagAutoClearTare = ON;
		return ON;
	}
	
	return OFF;
}

INT16S TareClearPLU(void)
{
	if(TareOper.tarePLU)
	{
		keyapp_tare(TARE_TYPE_PLU,0, OFF);
		return ON;
	}
	
	return OFF;
}

INT8U TareOverridePLUTare;
INT16S tare_operation(INT16S tareType, INT32U tareValue)
{
	INT8U ret;
	INT8U tareOK=OFF;
	TARE_OPER_TYPE	backTare;
	INT32U temp;   //se-hyung 20080325
	INT32U tare_auto_zero; //se-hyung 20080325
	long interval; //se-hyung 20080325
	long e_value;  //se-hyung 20080325
	
	long	curTareValue=0;
	INT8U cleartare;
	INT16S count;
	INT8U clerkTare = OFF;
#ifdef USE_KENYA_TARELIMIT
		INT32U tlimit = 0;
#endif
	
	ret=0;
	cleartare = OFF;
	tare_auto_zero = 0;  //se-hyung 20080325
	TareOverridePLUTare = OFF;
	
	backTare = TareOper;//data backup
	switch (tareType)
	{
		case TARE_TYPE_WEIGHT:
			
			break;
		case TARE_TYPE_KEY:
		case TARE_TYPE_PLU:
		case TARE_TYPE_PRESET:
			curTareValue = tareValue;
			break;
	}
	if (TareOper.addupWeightZero && Operation.operationClerk == OPER_FLOATING && status_scale.clerkid)
	{
		clerkTare = ON;
	}
	
	switch(tareType) {
		case TARE_TYPE_WEIGHT:
			
			interval = (long)ad_get_interval(0);	      //se-hyung 20080325
			interval = ad_cnv_adm2main(interval);
			temp = get_global_bparam(GLOBAL_TARE_SETITING);
			e_value = temp&0x07;
			e_value *= interval;
			temp >>= 3;
			tare_auto_zero = temp&0x01;
			
			if (TareOper.tareWeight) 
			{
				if (TareOper.tareWeight+status_scale.Weight >= -(e_value) && TareOper.tareWeight+status_scale.Weight <= e_value) // Gross Zero이면 se-hyung 20080325
				{
					cleartare = ON;
					break;	
				}
				if (clerkTare && TareOper.tareClerk)
				{
					break;
				}
				
				if (!TareOper.continuousTareUp && !TareOper.continuousTareDn) 	// One Time Tare이면
				{ 
					DisplayMsg(global_message[MSG_WAR_WEIGHTTARE]);//"Check weight tare"
					return OFF;
				}
				if (!TareOper.continuousTareDn && status_scale.Weight < 0) 
				{
					caption_split_by_code(0xC80A,&cap,0);
					DisplayMsg(cap.form);//"No down tare"
					return OFF;
				}
				if (!TareOper.continuousTareUp && status_scale.Weight > 0) 
				{
					caption_split_by_code(0xC80B,&cap,0);
					DisplayMsg(cap.form);//"No up tare"
					return OFF;
				}
				if (TareOper.continuousTareUp && TareOper.continuousTareDn) 
				{
					break;
				}
			}
			if (TareOper.tareKey) 
			{
				if (TareOper.tareKey+status_scale.Weight >= -(e_value) && TareOper.tareKey+status_scale.Weight <= e_value)
				{
					cleartare = ON;
					break;
				} 
				else 
				{
					if (clerkTare && TareOper.tareClerk)
					{
						break;
					}
					DisplayMsg(global_message[MSG_WAR_KEYTARE]);//"Check key tare"
					return OFF;
				}
			}
			if (TareOper.tarePLU) 
			{
				if (TareOper.useTareAfterPLUCall)
				{
					if (TareOper.tarePLU+status_scale.Weight >= -(e_value) && TareOper.tarePLU+status_scale.Weight <= e_value)
					{
						cleartare = ON;
						break;
					}
				}
				else
				{
					if (clerkTare && TareOper.tareClerk)
					{
						break;
					}
					DisplayMsg(global_message[MSG_WAR_PLUTARE]);//"Check PLU tare"
					return OFF;
				}
				TareOverridePLUTare = ON;
			}
			break;
		case TARE_TYPE_KEY:
			if (TareOper.tareWeight) 
			{
				if (curTareValue == 0) 
				{
					cleartare = ON;
					break;
				}
				if (clerkTare && TareOper.tareClerk)
				{
					break;
				}
				DisplayMsg(global_message[MSG_WAR_WEIGHTTARE]);//"Check weight tare"
				return OFF;
			}
			if (TareOper.tareKey) 
			{
				if (curTareValue == 0) 
				{
					cleartare = ON;
					break;
				}
				if (clerkTare && TareOper.tareClerk)
				{
					break;
				}
				if (!TareOper.continuousTareUp && !TareOper.continuousTareDn) 
				{
					DisplayMsg(global_message[MSG_WAR_KEYTARE]);//"Already key tare"
					return OFF;
				}
				if (!TareOper.continuousTareDn && curTareValue < TareOper.tareKey) 
				{
					caption_split_by_code(0xC80A,&cap,0);
					DisplayMsg(cap.form);//"No down tare"
					return OFF;
				}
				if (!TareOper.continuousTareUp && curTareValue > TareOper.tareKey) 
				{
					caption_split_by_code(0xC80B,&cap,0);
					DisplayMsg(cap.form);//"No up tare"
					return OFF;
				}
			}
			if (TareOper.tarePLU) 
			{
				if (TareOper.useTareAfterPLUCall)
				{
					if (curTareValue == 0) 
					{
						cleartare = ON;
						break;
					}
				}
				else
				{
					if (clerkTare && TareOper.tareClerk)
					{
						break;
					}
					DisplayMsg(global_message[MSG_WAR_PLUTARE]);//"Check PLU tare"
					return OFF;
				}
				TareOverridePLUTare = ON;
			}
			break;
		case TARE_TYPE_PLU:
			if (TareOper.tareWeight) 
			{
				if (TareOper.overridePluTare)	// PLU Tare 우선
				{
				}
				else
				{
					DisplayMsg(global_message[MSG_WAR_WEIGHTTARE]);//"Check weight tare"
					return OFF;
				}
			}
			if (TareOper.tareKey) 
			{
				if (TareOper.saveKeyTareForPLU) 	// Key Tare 저장 기능
				{
					TareOper.saveKeyTare = TareOper.tareKey;
				}
				else
				{
					if (TareOper.overridePluTare)	// PLU Tare 우선
					{
					}
					else
					{
						DisplayMsg(global_message[MSG_WAR_KEYTARE]);//"Check key tare"
						return OFF;
					}
				}
				
			}
			if (TareOper.tarePLU) 
			{
			}
			break;
		case TARE_TYPE_PRESET:	// 무조건 통과
			break;
	}
	
	if(cleartare == ON) {
		curTareValue = 0;
		tareType = TARE_TYPE_KEY;
	}
	count=0;
	while(1) {
#ifdef USE_KENYA_TARELIMIT
			switch(ADM_Status.Capa)
			{
				case 0:
					tlimit = 40;
					break;
				case 1:
					tlimit = 50;
					break;
				case 2:
					tlimit = 80;
					break;
				default:
					break;
			}

			if(tareType == TARE_TYPE_WEIGHT)
			{
				if(tlimit && status_scale.Weight >= tlimit)
					ret = 1;
				else
					ret = adm_set_weighttare(&curTareValue);
			}
			else
			{
				if(tlimit && curTareValue >= tlimit)
					ret = 1;
				else
					ret = ad_set_presettare(curTareValue);
			}
			if (5 < count++) {
				caption_split_by_code(0xE009,&cap,0);
				DisplayMsg(cap.form);//"Check AD Communication"
				break;
			}
#else
		if(tareType == TARE_TYPE_WEIGHT)
			ret = adm_set_weighttare(&curTareValue);
		else
			ret = ad_set_presettare(curTareValue);
		
		if (5 < count++) {
			caption_split_by_code(0xE009,&cap,0);
			DisplayMsg(cap.form);//"Check AD Communication"
			break;
		}
#endif
		
		if(ret == 9) {
			Key_DelaySec(1,0);
			continue;
		} else break;
	}
	if(!ret) {//good
		if(TareOverridePLUTare == ON) {
			tareType = TARE_TYPE_PLU;
		}
		TareOper.tareWeight = 0;
		TareOper.tareKey	= 0;
		TareOper.tarePLU	= 0;
		TareOper.tareClerk	= 0;
		switch(tareType) {
			case TARE_TYPE_WEIGHT:
				TareOper.tareWeight	= curTareValue;
				break;
			case TARE_TYPE_KEY:
			case TARE_TYPE_PRESET:
				TareOper.tareKey	= curTareValue;
				break;
			case TARE_TYPE_PLU:
				TareOper.tarePLU	= curTareValue;
				break;
		}
		// weight 업데이트가 tare의 업데이트 보다 늦어 Gross Weight 가 일시적으로 잘못 계산되는 문제 개선
		//  : Tare 변경시 ADM으로 부터 Weight가 업데이트 되기전 미리 계산해놓음
		if (!curTareValue)
		{
			status_scale.Weight = status_scale.Weight + status_scale.Tare;
		}
		else
		{
			status_scale.Weight = status_scale.Weight - curTareValue;
		}
		///////////////////////////////////////////////////////////////////////
		status_scale.Tare = curTareValue;
		if (clerkTare)
		{
			TareOper.tareClerk = curTareValue;
		}
	} else {//error
		TareOper = backTare;
		keyapp_tare_error(ret);
		return OFF;
	}
	if(tare_auto_zero && cleartare) keyapp_zero();
	return ON;
}

extern	INT8U	AutoClearTare;
INT16S keyapp_tare(INT16S tareType,INT32U tareValue, INT16S beepFlag)
{
	INT16S	stat;
	INT8U flag_tmp;
	INT8U chkUseKeyTareWithWeight;
	INT8U flag_numeric;
	INT8U chkUSENumericTareWithKEY;
	INT8U flag_weight;
	INT8U chkUSEWeightTareWithKEY;

#ifdef USE_WEIGHT_UNIT_500G
	long tempUprice;
#endif
	if(beepFlag)
	{
		AutoClearTare=OFF;
		if(!tareValue && !status_scale.Weight)	return OFF;
	}
	if(TareOper.canadianTare)
	{
		if(!Operation.transactionOK && status_scale.Tare)
		{
			MsgCanadianTare();
			return OFF;
		}
	}
	if(!tareType)// 키를 눌렀을때 판단
	{
		if(KeyDrv.PrevType == KEY_TYPE_NUMERIC && tareValue)
		{
			/* # Use Numeric Tare Block(P687)
			Use Numeric Tare Block 사용 시 P687이 [Y]일 경우
			: 숫자입력 용기설정을 제한한다.
			Use Numeric Tare Block 사용 시 P687이 [N]일 경우
			: 숫자입력 용기설정을 사용한다. */
			flag_numeric = get_global_bparam(GLOBAL_TARE_SETITING);
			chkUSENumericTareWithKEY = OnOff(flag_numeric&0x10);
			if (chkUSENumericTareWithKEY == ON)
			{
				BuzOn(2);
				return 0;
			}
			else
			{
				tareType=TARE_TYPE_KEY;
				
				// Use Key Tare With Weight(P795)
				flag_tmp=get_global_bparam(GLOBAL_SALE_SETUP_FLAG21);
				chkUseKeyTareWithWeight = OnOff(flag_tmp&0x40);
				if(chkUseKeyTareWithWeight == OFF)
				{
					if(0L < status_scale.Weight)
					{
						DisplayMsg((char *)global_message[MSG_23_C803]);//"Remove weight on tray"
						return OFF;
					}
				}
			}
		}
		else
		{
			/* # Use Weight Tare Block(P688)
			Use Weight Tare Block 사용 시 P688이 [Y]일 경우
			: 무게입력 용기설정을 제한한다.
			Use Weight Tare Block 사용 시 P688이 [N]일 경우
			: 무게입력 용기설정을 사용한다. */
			flag_weight = get_global_bparam(GLOBAL_TARE_SETITING);
			chkUSEWeightTareWithKEY = OnOff(flag_weight&0x20);
			if (chkUSEWeightTareWithKEY == ON)
			{
				BuzOn(2);
				return 0;
			}
			else
			{
				tareType=TARE_TYPE_WEIGHT;
				if (!ADM_GetData.Stable) return OFF;
			}
		}
	}
	
	stat=tare_operation(tareType,tareValue);
	if (tareValue) TareOper.flagAutoClearTare = OFF;
	if (stat) {
		if (status_scale.cur_pluid) {
			if (tareType == TARE_TYPE_KEY) 
			{
#ifdef	USE_WEIGHT_UNIT_500G
 #ifdef USE_UNIT_500G_PLU_500G_BASE
				if(status_scale.cur_weightunit == 500) {
					status_scale.cur_unitprice = status_scale.Plu.unitprice;
				} else if(status_scale.cur_weightunit == 1000 && PluType()== PLU_BY_WEIGHT) {
					tempUprice =  longmuldiv(status_scale.Plu.unitprice,2*status_scale.cur_weightunit,power10(3));
					status_scale.cur_unitprice = (INT32U) tempUprice;
				} else {
					status_scale.cur_unitprice = status_scale.Plu.unitprice;
				}
 #else
				if(status_scale.cur_weightunit == 500 && PluType()== PLU_BY_WEIGHT) 
				{
					tempUprice =  longmuldiv(status_scale.Plu.unitprice,status_scale.cur_weightunit,power10(3));
					status_scale.cur_unitprice=(INT32U)tempUprice;
				} 
				else 
				{
					status_scale.cur_unitprice=status_scale.Plu.unitprice;
				}
 #endif
#else
				status_scale.cur_unitprice=status_scale.Plu.unitprice;
#endif
			}
		}
	}
	return stat;
}


#ifdef USE_TAX_RATE
INT16S GetTaxData(PRICE_TYPE *pPrice,INT8U taxNumber,INT8U misc_flag)		
{
	INT32U	flash_addr, addr; 
	TAX_STRUCT	tax;
	INT16S	globalFlag,globalId;
	INT8U flag, miscId;
	
	globalFlag = get_global_bparam(GLOBAL_TAX_FLAG);
	globalId   = get_global_bparam(GLOBAL_TAX_VALUE);
	miscId   = get_global_bparam(GLOBAL_MISC_TAX_VALUE);
	
	//ok
	//sprintf(gmsg,"GF %d GNo %d",globalFlag,globalId);
	//PutSmallString(3,0,gmsg,30);
	//Key_DelaySec(10);
	tax.tax_code = 0;
	
	pPrice->TaxNumber = 0;
	pPrice->ExTax = 0;
	pPrice->TaxRate = 0;
	
	if (!(status_scale.restrict&FUNC_TAX_TBL)) return 0;
	
	flag=0;
	if(misc_flag==0)	// global
	{
		if(globalFlag == 2) taxNumber = globalId;	// globla id 를 할당
		if(globalFlag && taxNumber) flag=1;
	}
	else if(misc_flag==1)	//MISC 
	{
		taxNumber = miscId;
		if(taxNumber) flag=1;
	}
	
	if(flag)
	{
		addr = DFLASH_BASE + FLASH_TAX_AREA;
		flash_addr = addr + (sizeof(TAX_STRUCT) * (taxNumber-1));
		Flash_sread(flash_addr, (HUGEDATA INT8U*)&tax, sizeof(TAX_STRUCT));
		if(taxNumber == tax.tax_code)
		{
			pPrice->TaxNumber = tax.tax_code;
			pPrice->ExTax = tax.tax_type;
			pPrice->TaxRate = (long)tax.tax_rate;
		}
	}
	return 1;
}
#endif
#define EMPTY_SPACE		"            " // 화면 지우기 위한 space 12개

INT32U keyapp_pluno(INT8U tDept,INT32U pluid, INT8U beep)
{
	INT8U  taretype,stype;
	INT8U  linkedTareType[MAX_LINKED_PLU];
	INT16U oldpluid;
	INT8U fw_temp; //se-hyung 20070920
	//DEL060829	INT16U wait, backflag;
	PLU_BASE Plu;
	INT16S sz,i,index;
	INT8U flagAllProcessUpdate;
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
	TARE_OPER_TYPE	tempTare;
#endif
#ifdef USE_WEIGHT_UNIT_500G
	long tempUprice;
#endif

#ifdef USE_KOR_TRACE
	INT8U retry_cnt,flagTemp,backflag;
	INT16U wait;
#endif

#ifdef USE_MULTI_TRACE_NO_TABLE
	INT8U ind_str2[INDIVIDUAL_NO_LEN];
	INT8U ind_str3[INDIVIDUAL_NO_LEN];
	INT8U sl_no2;
	INT8U sl_no3;
#endif
#ifdef USE_CHN_CART_SCALE
		INT16U lotRoom;
		INT8U vege_trace_head;
#endif
#ifdef USE_TRACE_STANDALONE
		static INT32U backupPluid; //csh 20140303
		INT16U indIdxNo;	
#endif
#ifdef USE_PLU_INDIV_INDEX
		INT8U indiv_index_flag;
#endif

	if (!pluid) //return 0;
	{
		if (beep)
		{
			sale_display_update_error(0xE401,2,pluid);
			if(oldpluid)
			{
				sale_pluclear(ON);
				sale_display_update(0x0fff);//061128
			}
		}
		return 0;
	}
	//	if (BatchCount) BatchCount=0;
#ifdef USE_LONGPRESS_KEY_OVERRIDE
    if(!keyLongkeyIn && beep)
        BuzOn(1);
#else
	if (beep)       BuzOn(1);
#endif
	
	if (Clerk_Subtotal_List == 1)
	{	
		Clerk_Subtotal_List = 0; //Modify by se-hyung for Clerk Subtotal List 20070727
		sale_display_update(UPDATE_SUBTOTAL);
	}
	if (status_scale.clerkid) NetClerkLockRelease(status_scale.clerkid);
	
	oldpluid=status_scale.cur_pluid;
	if(TareOper.canadianTare && oldpluid)
	{
		if(status_scale.Tare && !Operation.transactionOK)
		{
			MsgCanadianTare();
			return OFF;
		}
	}
	//if ((oldpluid) && (pluid==0)) {
	//	sale_display_update_error(0xF006,0);
	//	goto END;
	//}
	if(!tDept) tDept = status_scale.departmentid;
#ifdef USE_LOTTEMART_ITEMCODE13
	if(status_scale.cur_pluid != pluid)
	{
		sale_display_update(UPDATE_TITLE);
	}
#endif		
#ifdef USE_KOR_TRACE
	if(KorTrace_Flag.flagPrint!=2){
		KorTrace_Flag.flagReceive = 0;
		//sale_display_kortrace(OFF);
		sale_display_update(UPDATE_TITLE);	// CJK070705
	}
#endif

#ifdef USE_TRACE_STANDALONE
	//	sale_display_indivFlag_set(OFF);
	IndividualDisplayTimeFlag = OFF;
	FlagIndividualMultiRemain = OFF;
#endif
	flagAllProcessUpdate = OFF;
	
	if (plu_callbykey((INT16U)tDept,pluid,&Plu,&taretype)) {
#ifdef USE_DONT_SAVE_VALIDDATE_SALEDATE
		temp_selldate.onoff=OFF;
#endif
		stype=Plu.ptype;
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
		tempTare = TareOper;
#endif
		//		if(TareOper.tarePLU)	TareClearPLU();		//deleted by JJANG 20080212
		
		//SG060330
#ifdef USE_PLU_SCROLLING			// PLU 길이가 길 경우 스크롤 됨
		PLU_SCROLL.data_leng= strlen((char*)Plu.ItemName);
		if(PLU_SCROLL.onoff != PLU_SCROLL_OFF){
			if(PLU_SCROLL.data_leng > MAX_SIDE_CHAR) {
				PLU_SCROLL.state = PLU_SCROLL_TURNON;
			}
			else {
				PLU_SCROLL.state = PLU_SCROLL_TURNOFF;
			}
			memset(PLU_SCROLL.msg,0,sizeof(PLU_SCROLL.msg));
			strcpy(PLU_SCROLL.msg,Plu.ItemName);
			strcat(PLU_SCROLL.msg,EMPTY_SPACE);
			PLU_SCROLL.pt=0;
			PLU_SCROLL.speed_timer = SysTimer100ms;
		}
#endif

#ifdef USE_KOR_TRACE
		if(KorTrace_Flag.flagPrint==2){
			Plu.unitprice = KorTrace_ReceiveCmd.unitprice;
		}
#endif
		if (stype==1)
		{
#ifndef USE_PRT_CONVERT_WEIGHT_KG_G
			if(GlbOper.EnableKgLb) 
			{
				if((ADM_GetData.CurUnit == WEIGHT_UNIT_KG || ADM_GetData.CurUnit == WEIGHT_UNIT_G)) 
				{
					if(Plu.unit_kglb == WEIGHT_UNIT_LB) 
					{
						if(GlbOper.EnableAutoConversionKgLb) 
						{
							adm_set_unit_cur(WEIGHT_UNIT_LB);
							LoadWeighingInfo();
							flagAllProcessUpdate = ON;
						} 
						else 
						{
							DisplayMsg(global_message[MSG_NOT_ALLOW_KGLB]);//"kg<->lb Not Allowed"
					    	return 0;
						}
					}
				} 
				else 
				{		// lb
					if(Plu.unit_kglb == WEIGHT_UNIT_KG || Plu.unit_kglb == WEIGHT_UNIT_G) 
					{
						if(GlbOper.EnableAutoConversionKgLb) 
						{
							if(get_base_bparam(FLASH_ADC_WUNIT) == WEIGHT_UNIT_G)
								adm_set_unit_cur(WEIGHT_UNIT_G);
							else 
								adm_set_unit_cur(WEIGHT_UNIT_KG);
							LoadWeighingInfo();
							flagAllProcessUpdate = ON;
						} 
						else 
						{
							DisplayMsg(global_message[MSG_NOT_ALLOW_KGLB]);//"kg<->lb Not Allowed"
					    	return 0;
						}
					}
				}
			}
#endif
			if(TareOper.tarePLU)	TareClearPLU();	//Added by JJANG 20080212 TARE값 사라지는 현상해결  
			
			fw_temp = get_global_bparam(GLOBAL_SALE_SETUP_FLAG4)&0x20;	//se-hyung 20070920
			if (fw_temp == 0)						//se-hyung 20070920
			{
#ifdef USE_USA_FIXEDWEIGHT_UNIT_OZ
				if(Plu.fixed_weight > ad_get_capa())
#else
					if(Plu.fixed_weight > ad_get_capa() || !ad_check_weight_value(Plu.fixed_weight))
#endif
					{
						DisplayMsg(global_message[MSG_INVALID_FIXEDWEIGHT]);//"Invalid Fixed Weight"
						return 0;
					}
			}
			if(Plu.tare)
			{
				if (!keyapp_tare(TARE_TYPE_PLU,Plu.tare,OFF))
				{					
					if(TareOper.saveKeyTareForPLU && TareOper.saveKeyTare)
					{
						keyapp_tare(TARE_TYPE_KEY,TareOper.saveKeyTare,OFF);
						TareOper.saveKeyTare = 0;
					}
					else
					{	
						return 0;
					}
				}
			}
			else
			{
				if(TareOper.saveKeyTareForPLU && TareOper.saveKeyTare)
				{
					keyapp_tare(TARE_TYPE_KEY,TareOper.saveKeyTare,OFF);
					TareOper.saveKeyTare = 0;
				}
			}
		}
		status_scale.cur_pluid=pluid;
		//if (taretype==3) {
		//	status_scale.tare=status_scale.Plu.tare;
		//}
		memcpy((char *)&(status_scale.Plu),(char *)&Plu,sizeof(PLU_BASE));
#ifndef USE_PLU_MULTIPLE_PRICE	//Refefence PLU 기능 사용 안 함
		plu_get_field(Plu.address-1,PTYPE_NO_OF_LINK_PLU,(INT8U *)&status_scale.linked_count,&sz,sizeof(status_scale.linked_count));
		if(status_scale.linked_count) {
			if (Startup.disable_dept) {
				status_scale.linked_dept[0]=1;
				status_scale.linked_dept[1]=1;
			} else {
				plu_get_field(Plu.address-1,PTYPE_LINK1_DEPT_NO,(INT8U *)&status_scale.linked_dept[0],&sz,sizeof(status_scale.linked_dept[0]));
				plu_get_field(Plu.address-1,PTYPE_LINK2_DEPT_NO,(INT8U *)&status_scale.linked_dept[1],&sz,sizeof(status_scale.linked_dept[1]));
			}
			plu_get_field(Plu.address-1,PTYPE_LINK1_PLU_NO,(INT8U *)&status_scale.linked_plu[0],&sz,sizeof(status_scale.linked_plu[0]));
			plu_get_field(Plu.address-1,PTYPE_LINK2_PLU_NO,(INT8U *)&status_scale.linked_plu[1],&sz,sizeof(status_scale.linked_plu[1]));
		}
		for(i=0;i<MAX_LINKED_PLU;i++) {
			if(i<status_scale.linked_count)	{
				if(!plu_callbykey((INT16U)status_scale.linked_dept[i],status_scale.linked_plu[i],&status_scale.LinkedPlu[i],&linkedTareType[i]))
				{
					status_scale.linked_plu[i]=0;
				}
			}
			else
			{
				status_scale.linked_plu[i]=0;
			}
		}
#endif
		// 단가가 0 이면 자동 override
		//OPTION: unit price zero override
		//change
		Operation.override=OFF;
		Netweight_flag=OFF;
		//VFD7_display_override_clear();
		Operation.specialPrice=OFF;
		if(!status_scale.Plu.unitprice && Operation.unitpriceZeroOverride)	Operation.override=ON;
		
		//if(Operation.operationMode == MODE_MANAGER)	Operation.override=ON;
		stype<<=1;
		status_scale.flag_input&=0xf1;
		status_scale.flag_input|=stype;
		stype>>=1;
		///
#ifdef USE_PCS_FONT5_TO_TRACE_CODE	// USE PCS, PLUFONT5 field for Trace CODE 
		status_scale.cur_qty	  =1;
#else
		status_scale.cur_qty	  =status_scale.Plu.pcs;
#endif
		status_scale.cur_pcs      =status_scale.Plu.pcs;
#ifdef USE_PERCENT_WEIGHT
		status_scale.cur_ptare	=status_scale.Plu.tare_percent;
#endif
#ifdef USE_WEIGHT_UNIT_500G
 #ifdef USE_UNIT_500G_PLU_500G_BASE
		if(status_scale.cur_weightunit == 500) {
			status_scale.cur_unitprice = status_scale.Plu.unitprice;
		} else if(status_scale.cur_weightunit == 1000 && PluType()== PLU_BY_WEIGHT) {
			tempUprice =  longmuldiv(status_scale.Plu.unitprice,2*status_scale.cur_weightunit,power10(3));
			status_scale.cur_unitprice = (INT32U) tempUprice;
		} else {
			status_scale.cur_unitprice = status_scale.Plu.unitprice;
		}
 #else
		if(status_scale.cur_weightunit == 500 && PluType()== PLU_BY_WEIGHT)
		{				
			tempUprice =  longmuldiv(status_scale.Plu.unitprice,status_scale.cur_weightunit,power10(3));
			status_scale.cur_unitprice=(INT32U)tempUprice;	//
		}
		else
		{
			status_scale.cur_unitprice=status_scale.Plu.unitprice;	//
		}
 #endif
#else
		status_scale.cur_unitprice=status_scale.Plu.unitprice;	//
#endif
		status_scale.cur_weightunit=status_scale.Plu.weightunit;	// Added by CJK 20050927
		Operation.fixedPrice = status_scale.Plu.fixed_price;
		status_scale.divisor = 0;
#ifdef USE_NHMART_SAFE_MEAT
		network_status.indivCallCts = 0;
#endif
		SetMaxRide(stype);
		SetCurRide(stype);
#ifdef USE_TAX_RATE        
		GetTaxData(&Price,status_scale.Plu.tax_id,0);		
		for(index=0;index<MAX_LINKED_PLU;index++)
		{
			GetTaxData(&LinkedPrice[index],status_scale.LinkedPlu[index].tax_id,0);
		}
#endif        
		///
		if(status_scale.cur_unitprice)
		{
			Operation.specialPriceDisc = OFF;
#ifdef USE_TOPMART_STRUCTURE
			GetDiscountInfo(stype);
			if(DiscSale.saleIndexOk && DiscSale.discIndexData.discPriceType==DISC_PRICE_FIXED)
			{
				Price.FixedDiscountPrice = DiscSale.discIndexData.discPrice[0];
			}
#else //USE_TOPMART_STRUCTURE
			if (GlbOper.useSpecialPriceForDisc && status_scale.Plu.special_price) 
			{
				Operation.specialPriceDisc = ON;
				key_common_disc1=DISC_PRICE_UPRICE;
				key_common_disc2=DISC_PRICE_TPRICE;
				keyapp_common_disc(status_scale.Plu.special_price);
			}
  #ifdef USE_PLU_MULTIPLE_PRICE
		else if(status_scale.Plu.first_price || status_scale.Plu.second_price)
		{
			if(PluType()==PLU_BY_WEIGHT)
			{
				multi_plu_disc(DISC_TARGET_WEIGHT, status_scale.Plu.unitprice, status_scale.Plu.first_target, status_scale.Plu.first_price, status_scale.Plu.second_target, status_scale.Plu.second_price);
			}
			else if(PluType()==PLU_BY_COUNT)
			{
				multi_plu_disc(DISC_TARGET_COUNT, status_scale.Plu.unitprice, status_scale.Plu.first_target, status_scale.Plu.first_price, status_scale.Plu.second_target, status_scale.Plu.second_price);
			}
		}
  #endif
			else
			{
				GetDiscountInfo(stype);
				if(DiscSale.saleIndexOk && DiscSale.discIndexData.discPriceType==DISC_PRICE_FIXED)
				{
					//status_scale.cur_unitprice = DiscSale.discIndexData.discPrice[0];
					Price.FixedDiscountPrice = DiscSale.discIndexData.discPrice[0];
				}
			}
#endif // USE_TOPMART_STRUCTURE
		}
#ifdef USE_CHN_EMART_TRACE
//#ifdef USE_CHN_EMART_BIRDS_TRACE
			if(status_scale.cur_pluid == pre_pluid) Pluchangeflag = OFF;
			else Pluchangeflag = ON;
			pre_pluid = status_scale.cur_pluid;
//#endif
		chn_emart_trace_direct_call_traceNo(0);	// 최근 trace code 호출
#endif

#ifdef USE_KOR_TRACE
		//sale_display_kortrace(OFF);
		if(network_status.service_bflag2){
			if(KorTrace_Flag.flagPrint!=2){

				KorTrace_Flag.flagCommSuccess = 0;
				flagTemp =0;
				retry_cnt=KOR_TRACE_RETRY_CNT;
#ifdef USE_KOR_TRACE_RS232
				keyapp_ktr_req_id();
				backflag = flagTempAllowSale;
				flagTempAllowSale = ON;
				wait = SysTimer100ms;
				while(1){
					network_common_trans();
					if(KorTrace_Flag.flagCommSuccess){
						flagTemp=1;
						break;
					}
					if((INT16U)(SysTimer100ms-wait) > KOR_TRACE_TIMEOUT) break;
				}
				flagTempAllowSale = backflag;
#else
				while(retry_cnt>0){

					if (Ethernet_Server(CLIENT_OPEN, 0, CLIENT_NORMAL)){
						keyapp_ktr_req_id();

						backflag = flagTempAllowSale;
						flagTempAllowSale = ON;
						wait = SysTimer100ms;
						while(1){
							network_common_trans();
							if(KorTrace_Flag.flagCommSuccess){
								flagTemp =1;
								break;
							}
							if((INT16U)(SysTimer100ms-wait) > KOR_TRACE_TIMEOUT) break;
						}

//SGTest------
//sprintf(MsgBuf, "[Timeout]wait=%04x, sys100ms=%04x, sys-wait=%04x \r\n",wait,SysTimer100ms,SysTimer100ms-wait);
//MsgOut(MsgBuf);
//------------
						flagTempAllowSale = backflag;

					}
					Ethernet_Server(CLIENT_CLOSE, 0, CLIENT_NORMAL); //SG060404

					if(flagTemp) break;
					retry_cnt--;

					//Delay_100ms(2);

				} //while(retry_cnt>0)
#endif
				if(!KorTrace_Flag.flagCommSuccess){
					BuzOn(4);
					KorTrace_CommErrorMsg();
					sale_pluclear(ON); 	//SG060405
					sale_display_update(0x0fff);//061128

				}

			}
		}
#endif
		
#ifdef USE_TRACE_STANDALONE
		memset(&CalledTraceStatus, 0, sizeof(CalledTraceStatus));
		
#ifdef USE_PLU_INDIV_INDEX
				//m1873 "이력번호 인덱스 사용 - Y"일 때 PLU에 저장된 이력 번호 호출
				indiv_index_flag = get_global_bparam(GLOBAL_SALE_SETUP_FLAG20) & 0x40;
				if(indiv_index_flag)
				{
					CalledTraceStatus.indivStr.index = Plu.indive_no;
				}
				else	
#endif
				{
#ifdef USE_MULTI_TRACE_NO_TABLE

		//		// 개체플래그가 0일 때 
		//		if (!status_scale.Plu.trace_flag && !plu_check_inhibit_ptype(PTYPE_TRACE_FLAG))
		//		{
		//			if (SaveTraceabilityNo != 2)	// 전체개체가 아닐 때
		//			{
		//				// 기억정보 초기화
		//				TNT_SetTraceNoTable((INT8U)status_scale.Plu.deptid, status_scale.cur_pluid, NULL, NULL, NULL, 0, 0, 0, 0, 0, 0, 0);
		//			}
		//		}
		//		else
		{
			indIdxNo = CalledTraceStatus.indivStr.index;	// 농협 idx 999 때문에 2byte로 임시변환
			//CalledTraceStatus.indivStr.individualNO[0] = 0;
			ind_str2[0] = 0;
			ind_str3[0] = 0;
			sl_no2 = 0;
			sl_no3 = 0;
			TNT_GetTraceNoTable((INT8U)status_scale.Plu.deptid, status_scale.cur_pluid, &indIdxNo, CalledTraceStatus.indivStr.individualNO, ind_str2, ind_str3, &CalledTraceStatus.slPlace, &sl_no2, &sl_no3, &CalledTraceStatus.meatUse, &CalledTraceStatus.meatPart, &CalledTraceStatus.indivStr.lotFlag, &CalledTraceStatus.gradeNo);		
			CalledTraceStatus.indivStr.index = indIdxNo;
			if (ind_str2[0] && !FlagIndividualMultiStart)	// 2번째 개체가 존재하면 
			{
				FlagIndividualMultiRemain = ON;
				MultiIndex = 0;
				memset(IndivdualMultiData, 0, sizeof(TRACE_INDIVIDUAL_MULTI_DATA) * MAX_INDIVIAL_MULTI_DATA_CNT);
				
				memcpy(IndivdualMultiData[MultiIndex].indivNoStr, CalledTraceStatus.indivStr.individualNO, INDIVIDUAL_NO_LEN);
				IndivdualMultiData[MultiIndex].slaughterHouse = CalledTraceStatus.slPlace;
				MultiIndex++;
				
				memcpy(IndivdualMultiData[MultiIndex].indivNoStr, ind_str2, INDIVIDUAL_NO_LEN);
				IndivdualMultiData[MultiIndex].slaughterHouse = sl_no2;
				MultiIndex++;
				
				if (ind_str3[0])
				{
					memcpy(IndivdualMultiData[MultiIndex].indivNoStr, ind_str3, INDIVIDUAL_NO_LEN);
					IndivdualMultiData[MultiIndex].slaughterHouse = sl_no3;
					MultiIndex++;
				}
				
				// 최종 데이터를 디스플레이에 표시함
				memcpy(CalledTraceStatus.indivStr.individualNO, IndivdualMultiData[MultiIndex-1].indivNoStr, INDIVIDUAL_NO_LEN);
				CalledTraceStatus.slPlace = IndivdualMultiData[MultiIndex-1].slaughterHouse;
			}
		}
#else
		//		// 개체플래그가 0일 때 
		//		if (!status_scale.Plu.trace_flag && !plu_check_inhibit_ptype(PTYPE_TRACE_FLAG))
		//		{
		//			if (SaveTraceabilityNo != 2)	// 전체개체가 아닐 때
		//			{
		//				// 기억정보 초기화
		//				SetIndividual_All((INT8U)status_scale.Plu.deptid, status_scale.cur_pluid, NULL, 0, 0, 0, 0, 0);
		//			}
		//		}
		//		else
		{
			indIdxNo = CalledTraceStatus.indivStr.index;
			TNT_GetTraceNoTable_Single((INT8U)status_scale.Plu.deptid, status_scale.cur_pluid, &indIdxNo, CalledTraceStatus.indivStr.individualNO, &CalledTraceStatus.slPlace, &CalledTraceStatus.meatUse, &CalledTraceStatus.meatPart, &CalledTraceStatus.indivStr.lotFlag, &CalledTraceStatus.gradeNo);
			CalledTraceStatus.indivStr.index = indIdxNo;
			// "USE_MULTI_TRACE_NO_TABLE" 모드가 아닐 경우, gradeNo는 기억 안됨
		}
#endif
	}
		switch (GlbOper.TraceabilityCallType)
		{
			case 0:	// 표시안함, 개체 유지
#ifdef USE_MULTI_TRACE_NO_TABLE
				FlagIndividualMultiRemain = OFF;
#endif
				CalledTraceStatus.indivStr.index = 0;	// 표시 안함
				CalledTraceStatus.slPlace = 0;
				CalledTraceStatus.meatUse = 0;
#ifdef USE_TRACE_MEATPART
				CalledTraceStatus.meatPart = 0;
#endif                
#ifdef USE_TRACE_MEATGRADE
				CalledTraceStatus.gradeNo = 0;
#endif
				break;
			case 1:	// 개체번호 + 등급
				if (!SaveTraceabilityNo)	// 일반개체일 때
				{
					CalledTraceStatus.indivStr.index = 0;
#ifdef USE_TRACE_MEATGRADE
					CalledTraceStatus.gradeNo = 0;
#endif
				}
				CalledTraceStatus.slPlace = 0;
				CalledTraceStatus.meatUse = 0;
#ifdef USE_TRACE_MEATPART
				CalledTraceStatus.meatPart = 0;
#endif
				break;
			case 2:	// 개체번호 + 등급 + 용도 
				CalledTraceStatus.slPlace = 0;
#ifdef USE_MULTI_TRACE_NO_TABLE
				if (FlagIndividualMultiRemain)
				{
					IndivdualMultiData[1].slaughterHouse = 0;
					IndivdualMultiData[2].slaughterHouse = 0;
				}
#endif
#ifdef USE_TRACE_MEATPART
				CalledTraceStatus.meatPart = 0;
#endif
				break;
			case 3:	// 개체번호 + 등급 + 도축장
				CalledTraceStatus.meatUse = 0;
#ifdef USE_TRACE_MEATPART
				CalledTraceStatus.meatPart = 0;
#endif
				break;
			case 4:	// 개체번호 + 등급 + 용도 + 도축장 + 부위
			case 5:
				break;
			case 6:
				if (!(backupPluid == status_scale.cur_pluid)||(SpeedKeyPressed == 1))
				{
					CalledTraceStatus.indivStr.index = 0;
					CalledTraceStatus.slPlace = 0;
					CalledTraceStatus.meatUse = 0;
#ifdef USE_MULTI_TRACE_NO_TABLE
					FlagIndividualMultiRemain = OFF;
#endif
					
#ifdef USE_TRACE_MEATPART
					CalledTraceStatus.meatPart = 0;
#endif				  
#ifdef USE_TRACE_MEATGRADE
					CalledTraceStatus.gradeNo = 0;
#endif
					
					backupPluid = 0;
					SpeedKeyPressed = 0;
				}
				
				backupPluid = status_scale.cur_pluid;
				break;
		}
		
		if (GlbOper.TraceabilityCallType)
		{
			keyapp_call_individual(CalledTraceStatus.indivStr.index, OFF);
#ifdef USE_LOTTEMART_MEATSCANNER
			if(CurTraceStatus.meatscannerflag)
			{
				sale_display_indivFlag_set(INDIV_DISP_MODE_INDIV_NO, ON);
			}
#endif			
			if (CalledTraceStatus.meatUse)
			{
				CurTraceStatus.meatUse = CalledTraceStatus.meatUse;
				sale_display_indivFlag_set(INDIV_DISP_MODE_SPECIALUSE, ON);
			}
			else
			{
				sale_display_indivFlag_set(INDIV_DISP_MODE_SPECIALUSE, OFF);
			}
			if (CalledTraceStatus.slPlace)
			{
				CurTraceStatus.slPlace = CalledTraceStatus.slPlace;
				sale_display_indivFlag_set(INDIV_DISP_MODE_SLAUGHTHOUSE, ON);
			}
			else
			{
				sale_display_indivFlag_set(INDIV_DISP_MODE_SLAUGHTHOUSE, OFF);
			}
#ifdef USE_TRACE_MEATPART
			if (CalledTraceStatus.meatPart)
			{
				CurTraceStatus.meatPart = CalledTraceStatus.meatPart;
				sale_display_indivFlag_set(INDIV_DISP_MODE_MEATPART, ON);
			}
			else
			{
				sale_display_indivFlag_set(INDIV_DISP_MODE_MEATPART, OFF);
			}
#endif            
		}
#ifdef USE_TRACE_MEATGRADE
  #ifdef USE_DISCLOSURE_MEATGRADE
		if (CalledTraceStatus.gradeNo)
		{
			CurTraceStatus.gradeNo = CalledTraceStatus.gradeNo; // 기억된 등급을 호출
			sale_display_indivFlag_set(INDIV_DISP_MODE_MEATGRADE, ON);
		}
		else if (status_scale.Plu.group)
		{
			CurTraceStatus.gradeNo = status_scale.Plu.group;	// PLU 등급을 호출
			sale_display_indivFlag_set(INDIV_DISP_MODE_MEATGRADE, ON);
		}
		else
		{
			sale_display_indivFlag_set(INDIV_DISP_MODE_MEATGRADE, OFF);
		}
		trace_meatgrade_getGradeText(CurTraceStatus.gradeNo, CurTraceStatus.curGradeText);	// 등급Text삭제
  #else
		if (!plu_check_inhibit_ptype(PTYPE_GROUP_NO))	// PLU 등급을 사용할 때
		{
			if (status_scale.Plu.group)
			{
				CurTraceStatus.gradeNo = status_scale.Plu.group;	// PLU 등급을 호출
				//trace_meatgrade_getGradeText(CurTraceStatus.gradeNo, CurTraceStatus.curGradeText);
				sale_display_indivFlag_set(INDIV_DISP_MODE_MEATGRADE, ON);
			}
			else
			{
				sale_display_indivFlag_set(INDIV_DISP_MODE_MEATGRADE, OFF);
			}
		}
		else		// PLU 등급을 사용하지 않을 때
		{
			if (GlbOper.TraceabilityCallType)	// 개체기억기능 사용시 and 0이 아닐 경우
			{
				if (CalledTraceStatus.gradeNo)
				{
					CurTraceStatus.gradeNo = CalledTraceStatus.gradeNo;	// 기억된 등급을 호출
					//trace_meatgrade_getGradeText(CurTraceStatus.gradeNo, CurTraceStatus.curGradeText);
					sale_display_indivFlag_set(INDIV_DISP_MODE_MEATGRADE, ON);
				}
				else
				{
					sale_display_indivFlag_set(INDIV_DISP_MODE_MEATGRADE, OFF);
				}
			}
		}
		trace_meatgrade_getGradeText(CurTraceStatus.gradeNo, CurTraceStatus.curGradeText);	// 등급Text삭제
  #endif		
#endif // USE_TRACE_MEATGRADE
		//sprintf(MsgBuf, "[G2](%s)ind_no=%d\r\n", CalledTraceStatus.indivStr.individualNO, CurTraceStatus.indivStr.index);
		//MsgOut(MsgBuf);
#elif defined(USE_CHN_CART_SCALE)
		CurCHNTraceLotNo = 1;
		if (Operation.traceCodeType == CHN_TRACE_CODE_TYPE_VEGETABLE)
		{
			vege_trace_head = get_nvram_bparam(NVRAM_VEGE_TRACE_HEAD + (status_scale.cur_pluid - 1));
			lotRoom = (VEGE_TRACE_LOT_COUNT + vege_trace_head - 1 - (CurCHNTraceLotNo - 1)) % VEGE_TRACE_LOT_COUNT;
			vege_trace_load(&CurCHNTrace, status_scale.cur_pluid, lotRoom);
			CurCHNTraceRemainWeight = vege_trace_remainWeight_load(status_scale.cur_pluid, lotRoom);
		}
		else
		{
			lotRoom = CurCHNTraceLotNo - 1;
			meat_trace_load(&CurCHNTrace, lotRoom);
			CurCHNTraceRemainWeight = meat_trace_remainWeight_load(lotRoom);
		}
#endif
		//#ifdef USE_TRACE_STANDALONE
		//		keyapp_get_individual();
		//#endif 
		
		// Delete CJK080708 : SAVE mode일 때도 transactionOK가 OFF가 되도록 수정. PLU 한번만 출력 기능은 SAVE mode일때는 동작안함
		//		if(Operation.transactionOK && Operation.wSave)	//Modified by JJANG 20080422 PARAMETER 580 관련 버그수정
		//		{
		//			if(pluid != oldpluid) 
		//			{
		//				Operation.transactionOK = OFF;
		//			}
		//		}
		//		else 
		Operation.transactionOK = OFF;	// PLU 호출시 transactionOK는 항상 OFF
		sale_display_update(UPDATE_WEIGHT|UPDATE_RIDE);
	} else {
		sale_display_update_error(0xE401,2,pluid);	// Modified by CJK 20051124
		if(oldpluid)
		{
			sale_pluclear(ON);
			sale_display_update(0x0fff);//061128
		}
		pluid = 0;
	}
	//#ifdef USE_TRACE_STANDALONE
	//	FlagIndividualMultiRemain = OFF;
	//#endif
	TareOper.flagAutoClearTare = OFF;
	Operation.negative = OFF;
	
	sale_display_update(UPDATE_MODE|UPDATE_CENTER|UPDATE_WEIGHT|UPDATE_UNIT|UPDATE_PRICE|UPDATE_TARE);
	
//SG060330
#ifdef USE_KOR_TRACE
	if(network_status.service_bflag2 && KorTrace_Flag.flagPrint!=2){
//		ProcessAll();
	}
#endif	
	
	//////고려///////////
	if(Operation.wPrepack || Operation.wAuto || flagAllProcessUpdate)
	{
		//if(Operation.operationMode != MODE_SELF)  //SG060926
		ProcessAll();
	}
	
	sale_display_tare_eu();
	
	Operation.salePluClearWait = OFF;	// 판매후 PLU 삭제 선택시 오동작 방지	// Added by CJK 20060315
	DispUpdateAll = 1;
	return pluid;
}

INT16U 	DoublePLUkeyTime=0;
INT16U	DoublePLUPrevSkey=0;
//#define DOUBLEKEY_TIME_100MS	3
INT16U  keyapp_doubleplukey(INT16U skeyplu, INT8U settingKey)
{
	INT32U lp;
	INT16U spd=0;
	INT16S i;
	
	if (SysTimer100ms>DoublePLUkeyTime)
		spd=SysTimer100ms-DoublePLUkeyTime;
	else
		spd=SysTimer100ms+0xffff-DoublePLUkeyTime+1;
	//	if ((spd<=1) || (spd>DOUBLEKEY_TIME_100MS))	DoublePLUPrevSkey=0;
	if ((spd<=1) || (spd>Operation.doubleKeyTime100ms))	DoublePLUPrevSkey=0;
	
	if (DoublePLUPrevSkey == skeyplu)
	{
		if (settingKey)
		{
			if (KeyDrv.Type == KEY_TYPE_SPEEDPLU)
			{
				if (Startup.scale_type==1) skeyplu += 48;
#if defined(USE_CL5200_KEY)
				else if (Startup.scale_type==6) skeyplu += 54;
#endif
				else skeyplu += 72;
			}
		}
		else
		{
			if (Operation.doubleDatekey)
			{
				if (skeyplu == KS_EDT_SELL)
				{
					if (DoublePLUPrevSkey == KS_EDT_SELL) 
					{
						skeyplu = KS_EDT_PACKDATE;
					}
				}
				else if (skeyplu == KS_DATETIME) 
				{
					if (DoublePLUPrevSkey == KS_DATETIME) 
					{
						skeyplu = KS_EDIT_DATETIME;
					}
				}
			}
			if (Operation.doublePLUkey == 1)
			{
				if (KeyDrv.Type == KEY_TYPE_SPEEDPLU)
				{
					//if (DoublePLUPrevSkey == skeyplu)
					{
						if(KeyDrv.DeviceType == 0) {//Scale key //HYP 20051221
							lp =FLASH_KEY_AREA+DFLASH_BASE;
							if(!KeyDrv.Shifted)
								skeyplu = Flash_wread(lp+KeyDrv.RawCode*2 + KEY_MAX_TABLE_SIZE);
							else
								skeyplu = Flash_wread(lp+KeyDrv.RawCode*2);
						} else {//ps/2 key
							lp = FLASH_PSKEY_AREA + DFLASH_BASE;
							for (i=0; i<PS2_KEY_MAX_NUM; i++) {
								if(skeyplu == Flash_wread(lp + 2*i))	break;
							}
							if(i != PS2_KEY_MAX_NUM) {
								lp = FLASH_PSKEY_SHIFT_AREA + DFLASH_BASE;
								skeyplu = Flash_wread(lp + 2*i);
							}
						}
					}
				}
			}
		}
		DoublePLUPrevSkey = 0;
	}
	else
	{
		DoublePLUPrevSkey = skeyplu;
	}
	DoublePLUkeyTime  = SysTimer100ms;
	return skeyplu;
}

INT16U keyapp_togleplukey(INT16U skeyplu, INT8U settingKey)
{
	INT32U lp;
	INT16S i;
	
	if (DoublePLUPrevSkey == skeyplu)
	{
		if (settingKey)
		{
			if (KeyDrv.Type == KEY_TYPE_SPEEDPLU)
			{
				if (Startup.scale_type==1) skeyplu += 48;
#if defined(USE_CL5200_KEY)
				else if (Startup.scale_type==6) skeyplu += 54;
#endif
				else skeyplu += 72;
			}
		}
		else
		{
			if (Operation.doublePLUkey == 2)
			{
				//if (KeyDrv.Type == KEY_TYPE_SPEEDPLU)
				{
					//if (DoublePLUPrevSkey == skeyplu)
					{
						if(KeyDrv.DeviceType == 0) {//Scale key //HYP 20051221
							lp =FLASH_KEY_AREA+DFLASH_BASE;
							if(!KeyDrv.Shifted)
								skeyplu = Flash_wread(lp+KeyDrv.RawCode*2 + KEY_MAX_TABLE_SIZE);
							else
								skeyplu = Flash_wread(lp+KeyDrv.RawCode*2);
						} else {//ps/2 key
							lp = FLASH_PSKEY_AREA + DFLASH_BASE;
							for (i=0; i<PS2_KEY_MAX_NUM; i++) {
								if(skeyplu == Flash_wread(lp + 2*i))	break;
							}
							if(i != PS2_KEY_MAX_NUM) {
								lp = FLASH_PSKEY_SHIFT_AREA + DFLASH_BASE;
								skeyplu = Flash_wread(lp + 2*i);
							}
						}
					}
				}
			}
		}
		DoublePLUPrevSkey = 0;
	}
	else
	{
		DoublePLUPrevSkey = skeyplu;
	}
	//sprintf(MsgBuf, "Prev=%04x, Cur=%04x\r\n", DoublePLUPrevSkey, skeyplu);
	//MsgOut(MsgBuf);
	return skeyplu;
}

INT32U keyapp_plukey(INT16U skeyplu)
{
	INT32U pluid;
	INT16U keyplu;
	//	INT8U  check;
	//	INT32U key_addr;
#ifdef USE_LONGPRESS_KEY_OVERRIDE
    if(!keyLongkeyIn)
#endif	
	BuzOn(1);
	
	if(Operation.operationMode == MODE_SELF)
	{
		if (!ADM_GetData.Stable) return 0;
		//SG060920		if(status_scale.weight_min > status_scale.Weight)	return 0;
	}
	
	//	if(Operation.doublePLUkey)
	//	{
	//		if(!DoublePLUkeyTime)	DoublePLUPrevSkey=0;
	//		if(KeyDrv.PrevType != KEY_TYPE_SPEEDPLU)	DoublePLUPrevSkey = 0;
	//		if(DoublePLUPrevSkey == skeyplu)
	//		{
	//			//DisplayMsg("double plu key");
	//			if(KeyDrv.DeviceType == 0) {//Scale key //HYP 20051221
	//				lp =FLASH_KEY_AREA+DFLASH_BASE;
	//				if(!KeyDrv.Shifted)
	//					skeyplu = Flash_wread(lp+KeyDrv.RawCode*2 + KEY_MAX_TABLE_SIZE);
	//				else
	//					skeyplu = Flash_wread(lp+KeyDrv.RawCode*2);
	//			} else {//ps/2 key
	//				lp = FLASH_PSKEY_AREA + DFLASH_BASE;
	//				for (i=0; i<PS2_KEY_MAX_NUM; i++) {
	//					if(skeyplu == Flash_wread(lp + 2*i))	break;
	//				}
	//				if(i != PS2_KEY_MAX_NUM) {
	//					lp = FLASH_PSKEY_SHIFT_AREA + DFLASH_BASE;
	//					skeyplu = Flash_wread(lp + 2*i);
	//				}
	//			}
	//		}
	//	}
	//	keyplu = skeyplu - KS_PLU_01;
	if (Operation.doublePLUkey == 2)
	{
		skeyplu = keyapp_togleplukey(skeyplu, OFF);
	}
	else	// Operation.doublePLUkey == 0 or 1
	{
		skeyplu = keyapp_doubleplukey(skeyplu, OFF);
	}
	keyplu = skeyplu - KS_PLU_01 + 1;
	//new
	//sprintf(gmsg,"PLU %d,%d, RAW %d",skeyplu,keyplu,KeyDrv.RawCode);
	//DisplayMsg(gmsg);
	//Key_DelaySec(10);
	//Operation.doublePLUkey = 1;
	if ((status_scale.cur_speedkeyid<1) || (status_scale.cur_speedkeyid>5)) {
		status_scale.cur_speedkeyid=1;
	}
	//key_addr =DFLASH_BASE + FLASH_KEY_SPEED_AREA;
	//key_addr+=(status_scale.cur_speedkeyid-1)*MAX_SPEEDKEY_SIZE;
	//key_addr+=keyplu*4;
	
	//pluid    =Flash_lread(key_addr);
	//	pluid = plu_get_speedkey(status_scale.cur_speedkeyid, keyplu);
	
	if((Operation.operationMode == MODE_SELF) && KeyDrv.DeviceType)
	{
		pluid = plu_get_speedkey(5, keyplu);
	}
	else
	{
		pluid = plu_get_speedkey(status_scale.cur_speedkeyid, keyplu);
	}
	
	
	
	
	pluid = keyapp_pluno(0,pluid,ON);
	
	if(Operation.operationMode == MODE_SELF) SelfserviceAutoCheck();	//Modified by JJANG 20070803
	
	
	/*	delete by JJANG 20070803
	 if(Operation.operationMode == MODE_SELF)
	 {
	 //SG060920		if(PluType()==PLU_BY_WEIGHT && CheckPluPrint())
	 if(!Operation.wAuto)	return pluid;		//Added JJANG 20070522
	 check=0;
	 if(status_scale.Weight <= 0 && Operation.wAuto) check =1;
	 if(CheckPluPrint(check))
	 {
	 //SG060920			caption_split_by_code(0x20e0,&cap,0);	//"Self service"
	 //SG060920  			DisplayMsg(&cap.form[2]);
	 if (PrtStruct.Mode==LABEL && (PluType() != PLU_BY_WEIGHT)) keyapp_print(OFF);	//Modified by JJANG 20070802 
	 if (PrtStruct.Mode==RECEIPT)
	 {
	 INT16S clerkid=0;
	 set_nvram_bparam(NVRAM_LASTCLERKNUMBER, (char)clerkid);
	 DirectWeightMode();
	 DirectCountMode();      //SG070227
	 SaleAcctLocalAddTransaction(status_scale.Plu.deptid,status_scale.cur_pluid,&Price,FLAG_TRANS_ADD);
	 keyapp_total(ON);
	 }
	 }
	 return pluid;
	 }
	 */
	//	if(DoublePLUPrevSkey == skeyplu)
	//		DoublePLUPrevSkey = 0;
	//	else
	//		DoublePLUPrevSkey = skeyplu;
	//	DoublePLUkeyTime = SysTimer100ms;
	return pluid;
}

//Added by JJANG 20070803 셀프서비스에서 개수상품 자동발행
void SelfserviceAutoCheck(void)
{
	INT8U  check;
	if(Operation.wAuto)
	{
		check=0;
		if(status_scale.Weight <= 0 && Operation.wAuto) check =1;
		if(CheckPluPrint(check))
		{
			if (PrtStruct.Mode==LABEL && (PluType() != PLU_BY_WEIGHT)) keyapp_print(OFF);	//Modified by JJANG 20070802 
		}
	}
}



INT16S input_date_check_pos;

void input_date_check_year_format(INT16S a)
{
	POINT 		disp_p;
	INT16S y_gab=8, x_gab=6;
	
	disp_p = point(input_date_check_pos*Startup.menu_ygap,17*Startup.menu_xgap);
	memset((INT8U *)cap.form,0x20,sizeof(cap.form));
	DspStruct.DirectDraw = 1;
	Dsp_Write_String(disp_p,(HUGEDATA char *)cap.form);
	
	// <CODE> JHC, 040420, -change caption no
	// [
	// $1: JHC, 040420, Modify, -184B~E To 183B~E
	switch (a) {
		case 1: 	caption_split_by_code(0x384A, &cap,0);	break;
		case 2: 	caption_split_by_code(0x384B, &cap,0);	break;
		case 3: 	caption_split_by_code(0x384C, &cap,0);	break;
		case 4: 	caption_split_by_code(0x384D, &cap,0);	break;
		default:
			memset((INT8U *)cap.form,0x20,sizeof(cap.form));
			break;
	}
	// ]
	DspStruct.DirectDraw = 1;
	Dsp_Write_String(disp_p,(HUGEDATA char *)cap.form);
}

#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
void keyapp_date(INT8U mode)
{
	POINT disp_p;
	char format[34], ret_string[20],s_date[20],s_time[20];

	INT8U date_form;
	INT8U date_temp, sep_date, sep_time,date_type;
	INT16S w,m1,m2,m3;
	INT16U  i16umax, i16umin;
	INT16U  max_id, cur_id, i, result, line_id;
	INT16U  page_max, page_cur;
	INT16U  year,month,day;
#ifdef USE_PERSIAN_CALENDAR
	INT16U	per_year,per_month,per_day,perCentury;
#endif
	long    ret_long;
	INT16U  cap_id[]={0x3841,0x3842,0x3843,0x3844,0x3845,0x3846,0x3847};
	CAPTION_STRUCT sub_cap;
	char    str[48];

	max_id   = 7;
	page_max = max_id/Startup.line_max;
	if (max_id%Startup.line_max) page_max++;

	date_form = get_global_bparam(GLOBAL_DATETIME_MODE);
	sep_date  = get_global_bparam(GLOBAL_DATE_SEPARATOR);
	sep_time  = get_global_bparam(GLOBAL_TIME_SEPARATOR);

	date_type = ((date_form>>4)& 0x03) + 1;
#ifdef USE_PERSIAN_CALENDAR
	RTC_Convert_Persian_Read();
#else
	RTC_CLK_Burst_Read();
#endif
#ifdef USE_MORNITORING_CNT
	Enable_operate_time_cnt = OFF;
#endif
	cur_id     = 0;
	page_cur   = 0;
	line_id    = 0;
PAGE_VIEW:
	year  =bcd2hex(RTCStruct.year);
	month =bcd2hex(RTCStruct.month);
	day   =bcd2hex(RTCStruct.date);
 	switch (date_type) {
		default:
		case 1:	caption_split_by_code(0x384A, &sub_cap,0); // 0
			sprintf(s_date,"%02d.%02d.%02d",year,month,day);
			break;
		case 2:						 // 1
			caption_split_by_code(0x384B, &sub_cap,0);
			sprintf(s_date,"%02d.%02d.%02d",month,day,year);
			break;
		case 3:
		case 4:	caption_split_by_code(0x384D, &sub_cap,0);
			sprintf(s_date,"%02d.%02d.%02d",day,month,year);
			break;
	}
	sprintf(s_time,"%02X:%02X:%02X",RTCStruct.hour,RTCStruct.min,RTCStruct.sec);
	Dsp_Fill_Buffer(0);
	display_title_page_change(0x1840, page_cur+1, page_max);
#ifdef USE_HIDE_DATETIME_MENU
	if(status_run.run_mode == RUN_CALIBRATION)
		menu_display_menucode(0x8700, 1);
#endif
	for (i=0; i<Startup.line_max; i++) {
		cur_id = (page_cur*Startup.line_max) + i;
		if (cur_id>=max_id) break;
		caption_split_by_code(cap_id[cur_id],&cap,0);
		if ((cur_id==0) || (cur_id==1)) {
#ifdef HEBREW_FONT
			strcat((char *)cap.form,"]");
#else
			strcat((char *)cap.form,"[");
#endif
#ifdef USE_THAI_FONT
			cap.input_x =  Cvt_ThaiStrLength((INT8U*)cap.form);
#else            
			cap.input_x =  strlen((char *)cap.form);
#endif
			if (cur_id==0) strcat((char *)cap.form,s_date);
			else           strcat((char *)cap.form,s_time);
#ifdef HEBREW_FONT
			strcat((char *)cap.form,"[");
#else
			strcat((char *)cap.form,"]");
#endif
			strcpy(str,cap.form);
			if (cur_id==0) {
				strcat(str,sub_cap.form);
			}
		}
		else
		{
			caption_adjust(&cap,NULL);
			strcpy(str,cap.form);
		}
		disp_p = point((i+Startup.menu_type)*Startup.menu_ygap, 0);
#ifdef HEBREW_FONT
		Convert_HebrewPreview((INT8U *)str, strlen(str), 0, 0);
		hebrew_codetemp[strlen(str)] = 0;
		disp_p.x = 202 - disp_p.x;
		Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
		Convert_ArabPreview((INT8U *)str, strlen(str), 0, 0, 0);
		arab_codetemp[strlen(str)] = 0;
		disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
		Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
		Dsp_Write_String(disp_p,( char *)str);
#endif
		strcpy(format,"99.99.99");
		switch (cur_id) {
			case 0: sprintf(ret_string,"%s",s_date); break;
			case 1: sprintf(ret_string,"%s",s_time); break;
			case 2: ret_long=(date_form>>6)& 0x01;
				break; // Month Format
			case 3: ret_long=(date_form>>7)& 0x01;
				break; // Year  Format
			case 4: ret_long=(date_form)& 0x07;
				break;    // Time Format
			case 5: ret_string[0]=sep_date;
				break;
			case 6: ret_string[0]=sep_time;
				break;
		}
		switch (cap.input_itype) {
#if defined(USE_ARAB_CONVERT) || defined(HEBREW_FONT)
			case 'N': result = keyin_ulong(KI_GRAPHIC, KI_FORM_ILNN, 0,(long*)&ret_long,
							cap.input_dlength, (i+Startup.menu_type)*Startup.menu_ygap, cap.input_x*Startup.menu_xgap, 0,KI_PASS );
#else
			case 'N': result = keyin_ulong(KI_GRAPHIC, KI_FORM_NORMAL, 0,(long*)&ret_long,
							cap.input_dlength, (i+Startup.menu_type)*Startup.menu_ygap, cap.input_x*Startup.menu_xgap, 0,KI_PASS );
#endif
					   keyin_clearcallback();
				   break;
			case 'S': result = keyin_string(KI_FREE_MODE, KI_GRAPHIC,NULL, ( INT8U*)ret_string,cap.input_length,
						cap.input_dlength, (i+Startup.menu_type)*Startup.menu_ygap,cap.input_x*Startup.menu_xgap, 0, 0, KI_PASS);
				break;
			case 'D':
			case 'T': result = keyin_string(KI_IP_MODE, KI_GRAPHIC,( INT8U*)format, ( INT8U*)ret_string,8,8,
						(i+Startup.menu_type)*Startup.menu_ygap,cap.input_x*Startup.menu_xgap,	0, 0, KI_PASS);
				break;
		}
	}
	Dsp_Diffuse();
PAGE_INPUT:
	cur_id=(page_cur*Startup.line_max)+line_id;
	if (cur_id>=max_id) {
		line_id=max_id%Startup.line_max;
		cur_id =max_id-1;
	}
	caption_split_by_code(cap_id[cur_id],&cap,0);

	i16umax = (INT16U)cap.input_max;
	i16umin = (INT16U)cap.input_min;

	if ((cur_id==0) || (cur_id==1)) {
#ifdef HEBREW_FONT
		strcat((char *)cap.form,"]");
#else
		strcat((char *)cap.form,"[");
#endif
#ifdef USE_THAI_FONT
		cap.input_x =  Cvt_ThaiStrLength((INT8U*)cap.form);
#else        
		cap.input_x =  strlen((char *)cap.form);
#endif        
		if (cur_id==0) strcat((char *)cap.form,s_date);
		else           strcat((char *)cap.form,s_time);
#ifdef HEBREW_FONT
		strcat((char *)cap.form,"[");
#else
		strcat((char *)cap.form,"]");
#endif
	}
	else
	{
		caption_adjust(&cap,NULL);
	}

	disp_p = point((line_id+Startup.menu_type)*Startup.menu_ygap, 0);
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
	Dsp_Write_String(disp_p,( char *)cap.form);
#endif
	Dsp_Diffuse();
	strcpy(format,"99.99.99");
	switch (cur_id) {
		case 0:	sprintf(ret_string,"%s",s_date); break;
		case 1:	sprintf(ret_string,"%s",s_time); break;
		case 2: ret_long=(date_form>>6)& 0x01;
			break; // Month Format
		case 3: ret_long=(date_form>>7)& 0x01;
			break; // Year  Format
		case 4: ret_long=(date_form)& 0x07;
			break;    // Time Format
		case 5: ret_string[0]=sep_date;
			break;
		case 6: ret_string[0]=sep_time;
			break;
	}

PAGE_KI_IN:
	switch (cap.input_itype)
	{
#if defined(USE_ARAB_CONVERT) || defined(HEBREW_FONT)
		case 'N': result = keyin_ulong(KI_GRAPHIC,
						   KI_FORM_ILNN,
						   0,
						   (long*)&ret_long,
						   cap.input_dlength,
						   (line_id+Startup.menu_type)*Startup.menu_ygap,
						   cap.input_x*Startup.menu_xgap,
						   0,
						   KI_NOPASS );
#else
		case 'N': result = keyin_ulong(KI_GRAPHIC,
					       KI_FORM_NORMAL,
					       0,
					       (long*)&ret_long,
					       cap.input_dlength,
					       (line_id+Startup.menu_type)*Startup.menu_ygap,
					       cap.input_x*Startup.menu_xgap,
					       0,
					       KI_NOPASS );
#endif
			  keyin_clearcallback();
			  if(result == KP_ESC)	goto RET_ESC;

			  cap.input_max = i16umax;
			  cap.input_min = i16umin;
			  
			  if( input_range_check(ret_long, cap.input_min, cap.input_max) )
			  {
			  	ret_long=ret_long%100;
			    	if (cur_id>=2) date_temp=(INT8U)ret_long;
			    	switch (cur_id) {
			  		case 2: date_form &= 0xbf;
			  		        if (date_temp) date_form|=0x40;
			  		        break; // Month Format
			  		case 3: date_form &= 0x7f;
			  			if (date_temp) date_form|=0x80;
			  			break; // Year  Format
			  		case 4: date_form &= 0xf0;
			  			date_form |= date_temp;
			  			break;    // Time Format
			    	}
			  }
			  else
			  {
			  		error_display_form1(0, cap.input_min, cap.input_max);
				  	goto PAGE_VIEW;
			  }
			  break;
		case 'S': result = keyin_string(KI_FREE_MODE, KI_GRAPHIC,NULL, ( INT8U*)ret_string,cap.input_length,
						cap.input_dlength, (line_id+Startup.menu_type)*Startup.menu_ygap,cap.input_x*Startup.menu_xgap, 0, 0, KI_NOPASS);
			  if(result == KP_ESC)	goto RET_ESC;
			  switch(cur_id)
			  {
			  case 5: sep_date = ret_string[0]; break;
			  case 6: sep_time = ret_string[0]; break;
			  }
			  break;
		case 'D':
		case 'T':
			  result = keyin_string(KI_IP_MODE, KI_GRAPHIC,( INT8U*)format, ( INT8U*)ret_string,8,8,
			 		(line_id+Startup.menu_type)*Startup.menu_ygap,cap.input_x*Startup.menu_xgap, 0, 0, KI_NOPASS);
			  if(result == KP_ESC)	goto RET_ESC;
			  if (cap.input_itype=='T') {
				  if( !(input_time_check_str((char*)&ret_string,1)) ) {
				 	BuzOn(2);
				 	goto PAGE_KI_IN;
				  }
			  } else {
				  for(w=0; w<8; w++) ret_string[w] -= '0';
				  m1 = ret_string[0]*10 + ret_string[1];
				  m2 = ret_string[3]*10 + ret_string[4];
				  m3 = ret_string[6]*10 + ret_string[7];
				  switch (date_type) {
					case 1: year=(INT16U)m1; month=(INT16U)m2; day=(INT16U)m3;break;
					case 2: year=(INT16U)m3; month=(INT16U)m1; day=(INT16U)m2; break;
					case 3:
					case 4: year=(INT16U)m3; month=(INT16U)m2; day=(INT16U)m1; break;
				  }
				  if (!date_checkday((short)year,(short)month,(short)day)) {
				 	BuzOn(2);
				 	goto PAGE_VIEW;
				  }
				  RTCStruct.year =hex2bcd((INT8U)year );
				  RTCStruct.month=hex2bcd((INT8U)month);
				  RTCStruct.date =hex2bcd((INT8U)day  );
			  }
			  break;
	}
	switch(result) {
		case KP_ARROW_DN:
		case KP_ENTER:
			if (cur_id<max_id-1) {
				if (line_id<Startup.line_max-1) {
					line_id++;
					goto PAGE_INPUT;
				} else {
					if (page_cur<page_max-1) {
						line_id=0;
						page_cur++;
						goto PAGE_VIEW;
					}
				}
			}
			else
			{
				if (result == KP_ENTER)
					goto RET_END;
			}
			goto PAGE_INPUT;
		case KP_PAGE_DN:
			if (page_cur<page_max-1)
			{
				page_cur++;
				goto PAGE_VIEW;
			}
			goto PAGE_VIEW;
			// ]
		case KP_ARROW_UP:
			if (line_id>0) {
				line_id--;
				goto PAGE_INPUT;
			} else {
				if (page_cur>0) {
					page_cur--;
					line_id=Startup.line_max-1;
					goto PAGE_VIEW;
				}
				else
					goto PAGE_INPUT;
			}
		case KP_PAGE_UP:
			if(page_cur>0) {
				page_cur--;
				goto PAGE_VIEW;
			}
			goto PAGE_VIEW;
		case KP_SAVE:
			goto RET_END;
		default:
			goto PAGE_KI_IN;
	}
RET_END:
#ifdef USE_PERSIAN_CALENDAR
	if(RTCStruct.year < 0x5a) 	perCentury = 1400; //90년 이전: 1400년대, 90년 이후: 1300년대
	else perCentury = 1300;

	RTCStruct.day = date_getweek((short)bcd2hex(RTCStruct.year)+perCentury,
		(short)bcd2hex(RTCStruct.month),(short)bcd2hex(RTCStruct.date));
	RTC_Convert_Persian_Write();
#else
	RTCStruct.day = date_getweek((short)bcd2hex(RTCStruct.year)+2000,
		(short)bcd2hex(RTCStruct.month),(short)bcd2hex(RTCStruct.date));
	RTC_CLK_Burst_Write();
#endif
#ifdef USE_MORNITORING_CNT
	rtc_set_pre_minute();
#endif
	if (mode==0) {
		set_global_bparam(GLOBAL_DATETIME_MODE, date_form);
		if(sep_date<0x21 || sep_date>0xFE) sep_date = 0x3A;
		set_global_bparam(GLOBAL_DATE_SEPARATOR, sep_date);
		if(sep_time<0x21 || sep_time>0xFE) sep_time = 0x3A;
		set_global_bparam(GLOBAL_TIME_SEPARATOR, sep_time);
	}
RET_ESC:
#ifdef USE_MORNITORING_CNT
	Enable_operate_time_cnt = ON;
#endif
	return;
}
#else
void keyapp_date(INT8U mode)
{
	char format[34], ret_string[20],s_date[20],s_time[20];
	char d_sep_str[4], t_sep_str[4];
	INT8U date_form;
	INT8U date_temp, sep_date, sep_time,date_type;
	//	INT16S w,m1,m2,m3;
	INT16U year,month,day;
	INT32U month_form, year_form, time_form;
	INT8U result;
	CAPTION_STRUCT sub_cap;
#ifdef USE_PERSIAN_CALENDAR
	INT16U perCentury;
#endif
	
	date_form = get_global_bparam(GLOBAL_DATETIME_MODE);
	//  Yxxx xxxx :  Y = Year format
	//  xMxx xxxx :  M = month format
	//  xxDD xxxx : DD = Date format
	//  xxxx xTTT :  T = Time format
	sep_date = '.'; //7-seg display용
	sep_time = '.'; //7-seg display용
	
	d_sep_str[0] = get_global_bparam(GLOBAL_DATE_SEPARATOR);
	d_sep_str[1] = 0;
	t_sep_str[0] = get_global_bparam(GLOBAL_TIME_SEPARATOR);
	t_sep_str[1] = 0;
	
	date_type = (date_form>>4)&0x03;
	month_form = (INT32U)((date_form>>6)&0x01); // Month Format
	year_form = (INT32U)((date_form>>7)&0x01); // Year  Format
	time_form = (INT32U)((date_form)&0x07); // Time Format
#ifdef USE_PERSIAN_CALENDAR
	RTC_Convert_Persian_Read();
#else
	RTC_CLK_Burst_Read();
#endif
	
	year  =bcd2hex(RTCStruct.year);
	month =bcd2hex(RTCStruct.month);
	day   =bcd2hex(RTCStruct.date);
	switch (date_type) {
		default:
		case 0:	caption_split_by_code(0x384A, &sub_cap,0); // 0
		sprintf(s_date,"%02d%c%02d%c%02d",year,sep_date,month,sep_date,day);
		break;
		case 1:						 // 1
			caption_split_by_code(0x384B, &sub_cap,0);
			sprintf(s_date,"%02d%c%02d%c%02d",month,sep_date,day,sep_date,year);
			break;
		case 2:
		case 3:	caption_split_by_code(0x384D, &sub_cap,0);
		sprintf(s_date,"%02d%c%02d%c%02d",day,sep_date,month,sep_date,year);
		break;
	}
	sprintf(s_time,"%02X%c%02X%c%02X",RTCStruct.hour,sep_date,RTCStruct.min,sep_date,RTCStruct.sec);
	
	
	Menu_Init();
	
	caption_split_by_code(0x1840, &cap,0);
	Menu_HeadCreate((HUGEDATA char *)cap.form);
#ifdef USE_HIDE_DATETIME_MENU
if(status_run.run_mode == RUN_CALIBRATION)
	menu_display_menucode(0x8700, 1);
#endif
	
	sprintf(format,"99%c99%c99", sep_date, sep_date);
	caption_split_by_code(0x3841, &cap,0); // date
	cap.input_length = 8;
	cap.input_dlength = 8;
	caption_adjust(&cap,NULL);
	cap.input_min = 1;
	Menu_InputCreate(1, (char *)cap.form, MENU_TYPE_DATE, cap.input_x, cap.input_dlength, cap.input_length,
			cap.input_max, cap.input_min, 0, NULL, (INT8U *)s_date, (INT8U *)format, NULL);
	Menu_SetGeneralFlag(1, date_type);
	Menu_SetFixedCharacter(1, sep_date);
	
	sprintf(format,"99%c99%c99", sep_time, sep_time);
	caption_split_by_code(0x3842, &cap,0); // time
	cap.input_length = 8;
	cap.input_dlength = 8;
	caption_adjust(&cap,NULL);
	cap.input_min = 1;
	Menu_InputCreate(2, (char *)cap.form, MENU_TYPE_TIME, cap.input_x, cap.input_dlength, cap.input_length,
			cap.input_max, cap.input_min, 0, NULL, (INT8U *)s_time, (INT8U *)format, NULL);
	Menu_SetFixedCharacter(2, sep_time);
	
	caption_split_by_code(0x3843, &cap,0); // month format(1=DEC,0=12)
	caption_adjust(&cap,NULL);
	Menu_InputCreate(3, (char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
			cap.input_max, cap.input_min, 0, &month_form, NULL, NULL, NULL);
	
	caption_split_by_code(0x3844, &cap,0); // year format(0=2001,1=01)
	caption_adjust(&cap,NULL);
	Menu_InputCreate(4, (char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
			cap.input_max, cap.input_min, 0, &year_form, NULL, NULL, NULL);
	
	caption_split_by_code(0x3845, &cap,0); // time format(0=24,1=12)
	caption_adjust(&cap,NULL);
	Menu_InputCreate(5, (char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
			cap.input_max, cap.input_min, 0, &time_form, NULL, NULL, NULL);
	
	caption_split_by_code(0x3846, &cap,0); // date seperate
	caption_adjust(&cap,NULL);
	Menu_InputCreate(6, (char *)cap.form, MENU_TYPE_STR, cap.input_x, cap.input_dlength, cap.input_length,
			cap.input_max, cap.input_min, 0, NULL, (INT8U *)d_sep_str, NULL, NULL);
	
	caption_split_by_code(0x3847, &cap,0); // time seperate
	caption_adjust(&cap,NULL);
	Menu_InputCreate(7, (char *)cap.form, MENU_TYPE_STR, cap.input_x, cap.input_dlength, cap.input_length,
			cap.input_max, cap.input_min, 0, NULL, (INT8U *)t_sep_str, NULL, NULL);
	
	
	result = Menu_InputDisplay();
	if(result == MENU_RET_SAVE) {
		
		date_form &= 0xbf;
		if (month_form) date_form|=0x40; // Month Format
		date_form &= 0x7f;
		if (year_form) date_form|=0x80; // Year  Format
		date_form &= 0xf8;
		date_form |= time_form; // Time Format
		
		input_day_check_str(date_type, s_date, 1);
		input_time_check_str(s_time, 1);
		
#ifdef USE_PERSIAN_CALENDAR
		RTC_Convert_Persian_Write();
#else
		RTC_CLK_Burst_Write();
#endif
#ifdef USE_MORNITORING_CNT
		rtc_set_pre_minute();
#endif
		if (mode==0) {
			set_global_bparam(GLOBAL_DATETIME_MODE, date_form);
			if(d_sep_str[0]<0x21 || d_sep_str[0]>0xFE) d_sep_str[0] = 0x3A;
			set_global_bparam(GLOBAL_DATE_SEPARATOR, d_sep_str[0]);
			if(t_sep_str[0]<0x21 || t_sep_str[0]>0xFE) t_sep_str[0] = 0x3A;
			set_global_bparam(GLOBAL_TIME_SEPARATOR, t_sep_str[0]);
		}
	}
}
#endif
void keyapp_edit_date(void)
{
	INT8U key_mode;
	INT8U old_font;
#ifdef _USE_LCD20848_//ck
	Dsp_Clear();
#endif		
	Dsp_ChangeMode(DSP_PGM_MODE);
	key_mode=KEY_GetMode();
	KEY_SetMode(1);
	old_font=DspStruct.Id1;
	DspLoadFont1(DSP_MENU_FONT_ID);
	
	keyapp_date(0);
	
	KEY_SetMode(key_mode);
	DspLoadFont1(old_font);
	Dsp_ChangeMode(DSP_SALE_MODE);
	
	sale_pluclear(OFF);
	sale_display_update(UPDATE_ALL|UPDATE_CLEARALL);
}

// Added by CJK
void keyapp_chg_prtmode(void)
{
	INT8U mode;
	CAPTION_STRUCT cap;
	
	//BuzOn(1);
	mode = PrtAutoChangeMode();
	
	if(mode == LABEL) {
		//sprintf(string_buf, "Changed to LABEL");
		caption_split_by_code(0x9c84,&cap,0);
	} else {
		//sprintf(string_buf, "Changed to RECEIPT");
		caption_split_by_code(0x9c85,&cap,0);
		//		DisplayMsg(cap.form);
	}
	DisplayMsg(cap.form);
	
	//display_message_page(string_buf);
}

//ticket mode = 1: floating & ticket 
//label mode = 2: label mode
extern void salemode_reset(void);
void keyapp_change_salemode(void)
{
	INT8U   uctemp;
	INT8U   mode1, mode2;
	
	uctemp=get_global_bparam(GLOBAL_SALERUN_MODE);
	mode1 = get_global_bparam(GLOBAL_CHANGE_MODE1);
	mode2 = 1;// floating & ticket mode
	
	if(uctemp == mode2)       // floating & ticket mode  --> mode1(label mode)
	{
		uctemp = mode1;
	}
	else if(uctemp == mode1)  // mode1(label mode) --> floating & ticket mode
	{
		uctemp = mode2;
	}
	else                  
	{
		uctemp = mode1;
	}
	
	set_global_bparam(GLOBAL_SALERUN_MODE, uctemp);
	salemode_reset();
	
	if(uctemp == 2 || uctemp == 3 || uctemp == 6)      // label mode or prepack mode
	{
		//sprintf(string_buf, "Changed to LABEL");
		caption_split_by_code(0x9c84,&cap,0);
	} 
	else if(uctemp ==1)  // ticket mode
	{
		//sprintf(string_buf, "Changed to RECEIPT");
		caption_split_by_code(0x9c85,&cap,0);
	}
	DisplayMsg(cap.form);
	
	//Initialize
	operation_load();
	logout();
	
}

void keyapp_set_packeddate(void)
{
	CAPTION_STRUCT cap;
	INT8U   select[32],prepackweek,uctemp1;
	INT32U	addr;
	INT32U  set_val[5];
	INT8U   result,old_font,key_mode;
	char	string_buf[50];
#ifdef _USE_LCD20848_//ck
	Dsp_Clear();
#endif		
	key_mode=KEY_GetMode();
	KEY_SetMode(1);
	Dsp_ChangeMode(DSP_PGM_MODE);
	old_font=DspStruct.Id1;
	DspLoadFont1(DSP_MENU_FONT_ID);
	
	addr = DFLASH_BASE;
	addr+= FLASH_GLOBAL_AREA;
	addr+= GLOBAL_PRT_INHIBIT;
	Flash_sread(addr, (HUGEDATA INT8U *)select, 32);
	set_val[0] = prtsel_get_table_buf(select, FIELD_ID_PACKED_DATE);
	set_val[1] = prtsel_get_table_buf(select, FIELD_ID_PACKED_TIME);
	set_val[2] = prtsel_get_table_buf(select, FIELD_ID_SELL_BY_DATE);
	set_val[3] = prtsel_get_table_buf(select, FIELD_ID_SELL_BY_TIME);
	
	uctemp1     = get_global_bparam(GLOBAL_SALE_SETUP_FLAG5);
	prepackweek = (uctemp1>>1)&0x03;
	set_val[4]  = prepackweek;
	
	//display_title_page_change(0x3878, 1, 1);
	Menu_Init();
	caption_split_by_code(18, &cap,0);
	sprintf(string_buf, "   %s", cap.form);
	Menu_HeadCreate((HUGEDATA char *)string_buf);
	
	caption_split_by_code(prtsel_get_menu_capcode(FIELD_ID_PACKED_DATE), &cap,0);
	caption_adjust(&cap,NULL);
	Menu_InputCreate(1, (HUGEDATA char *)cap.form, MENU_TYPE_YN, cap.input_x, cap.input_dlength, cap.input_length,
			cap.input_max, cap.input_min, 0, &set_val[0], NULL, NULL, NULL);
	
	caption_split_by_code(prtsel_get_menu_capcode(FIELD_ID_PACKED_TIME), &cap,0);
	caption_adjust(&cap,NULL);
	Menu_InputCreate(1, (HUGEDATA char *)cap.form, MENU_TYPE_YN, cap.input_x, cap.input_dlength, cap.input_length,
			cap.input_max, cap.input_min, 0, &set_val[1], NULL, NULL, NULL);
	
	caption_split_by_code(prtsel_get_menu_capcode(FIELD_ID_SELL_BY_DATE), &cap,0);
	caption_adjust(&cap,NULL);
	Menu_InputCreate(1, (HUGEDATA char *)cap.form, MENU_TYPE_YN, cap.input_x, cap.input_dlength, cap.input_length,
			cap.input_max, cap.input_min, 0, &set_val[2], NULL, NULL, NULL);
	
	caption_split_by_code(prtsel_get_menu_capcode(FIELD_ID_SELL_BY_TIME), &cap,0);
	caption_adjust(&cap,NULL);
	Menu_InputCreate(1, (HUGEDATA char *)cap.form, MENU_TYPE_YN, cap.input_x, cap.input_dlength, cap.input_length,
			cap.input_max, cap.input_min, 0, &set_val[3], NULL, NULL, NULL);
	
	caption_split_by_code(0x3878, &cap,0);
	caption_adjust(&cap,NULL);
	Menu_InputCreate(1, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
			cap.input_max, cap.input_min, 0, &set_val[4], NULL, NULL, NULL);
	
	result = Menu_InputDisplay();
	if(result == MENU_RET_SAVE) {
		prepackweek=(INT8U)set_val[4];
		uctemp1 &= (~0x06);
		uctemp1 |= ((prepackweek<<1)&0x06);
		set_global_bparam(GLOBAL_SALE_SETUP_FLAG5, uctemp1);
		uctemp1=(INT8U)set_val[0];
		prtsel_set_table_buf_pack(select, FIELD_ID_PACKED_DATE, uctemp1);
		uctemp1=(INT8U)set_val[1];
		prtsel_set_table_buf_pack(select, FIELD_ID_PACKED_TIME, uctemp1);
		uctemp1=(INT8U)set_val[2];
		prtsel_set_table_buf_pack(select, FIELD_ID_SELL_BY_DATE, uctemp1);
		uctemp1=(INT8U)set_val[3];
		prtsel_set_table_buf_pack(select, FIELD_ID_SELL_BY_TIME, uctemp1);
		Flash_swrite(addr, (HUGEDATA INT8U *)select, 32);
	}
	Dsp_Fill_Buffer(0);						// LCD Buffer clear
	DspLoadFont1(old_font);
	Dsp_Diffuse();
	Dsp_ChangeMode(DSP_SALE_MODE);
	KEY_SetMode(key_mode);
	sale_display_update(0x0fff);
}

//SG070723. set ticetnumber manually
void keyapp_set_ticketnumber(void)
{
	CAPTION_STRUCT 		cap;
	POINT 			disp_p;
	INT8U 	key_mode;
	long	ret_long;
	INT16U	result;
	char    old_font;
	
	if (ethernet_list.status != 1) //master only
	{ 
		BuzOn(2);
		return;
	}
#ifdef _USE_LCD20848_//ck
	Dsp_Clear();
#endif
	key_mode=KEY_GetMode();
	KEY_SetMode(1);
	Dsp_ChangeMode(DSP_PGM_MODE);
	
	old_font=DspStruct.Id1;
	DspLoadFont1(DSP_MENU_FONT_ID);
	
	display_title_page_change(0x3744, 1, 1);
	caption_split_by_code(0x3744, &cap,0);
	caption_adjust(&cap,NULL);
	cap.input_min = 1;
	
	disp_p = point( Startup.menu_type*Startup.menu_ygap, 0);
	Dsp_Write_String(disp_p, (HUGEDATA char*)&cap.form);
	Dsp_Diffuse();
	
	ret_long = get_nvram_lparam(NVRAM_COUNTER_TICKET);
	
	KI_IN:
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
			result = keyin_ulong(KI_GRAPHIC, 
			KI_FORM_NORMAL, 
			0,
			(long*)&ret_long, 
			cap.input_dlength,
			(Startup.menu_type)*Startup.menu_ygap,
			cap.input_x*Startup.menu_xgap, 
			0,
			KI_NOPASS);
#else			
			result = keyin_ulong(KI_DEFAULT, 
			KI_FORM_NORMAL, 
			0,
			(long*)&ret_long, 
			cap.input_dlength,
			(Startup.menu_type)*Startup.menu_ygap,
			cap.input_x*Startup.menu_xgap, 
			0,
			KI_NOPASS);
#endif
	if(result == KP_ESC)
		goto RET_ESC;
	
	if( !input_range_check(ret_long, cap.input_min, cap.input_max) ) 
	{
		error_display_form1(ret_long, cap.input_min, cap.input_max);
		goto KI_IN;
	} 
	
	switch(result) {
		case KP_ENTER:
		case KP_SAVE:
			set_nvram_lparam(NVRAM_COUNTER_TICKET, ret_long);
			break;
		default:
			BuzOn(2);
			goto KI_IN;
	}
	RET_ESC:
			Dsp_Fill_Buffer(0);					
	DspLoadFont1(old_font);
	Dsp_Diffuse();
	Dsp_ChangeMode(DSP_SALE_MODE);
	KEY_SetMode(key_mode);
	sale_display_update(0x0fff);
	
}

//SG060510
void keyapp_onoff_printdate(void)
{
	INT8U uctemp, uctemp2;
	
	uctemp = get_global_bparam(GLOBAL_SALE_SETUP_FLAG6);
	uctemp2 = (uctemp>>1)& 0x01;
	if(uctemp2){
		uctemp &= (~0x02);		
		DisplayMsg("PRT-DATE OFF");
	} else {
		uctemp |= 0x02;
		DisplayMsg("PRT-DATE ON");
	} 
	set_global_bparam(GLOBAL_SALE_SETUP_FLAG6, uctemp);
	
	////	Delay_100ms(5);
	////
	////	Dsp_Fill_Buffer(0);						// LCD Buffer clear
	//////	DspLoadFont1(old_font);
	////	Dsp_Diffuse();
	//////	KEY_SetMode(key_mode);
	////	sale_display_update(0x0fff);
	
}


extern SCANNER_DATA_STR   Scanner;
/*
 INT8U CheckScannerFormat(void)  //Added by JJANG 20071205
 {
 INT16U i;
 INT8U j;
 INT8U scannerBarForm[10][14];
 INT32U addr;
 INT8U  stemp[14];
 INT8U count;
 INT8U x_code=0;
 
 strncpy((char *)stemp,(char *)&Scanner.barcodeData,13);         //현재 스캔된 바코드
 
 addr = DFLASH_BASE  + FLASH_SCANNER_FORMAT;
 for(i=0;i<10;i++) 
 {
 Flash_sread(addr+(i*BAR_FORMAT_SIZE), scannerBarForm[i], 14);
 scannerBarForm[i][13] = 0;	 //EAN13 Only
 
 if(!scannerBarForm[i][0]) continue;     //중간에 포맷이 비어있을 경우
 else if(scannerBarForm[i][0]=='X') 
 {
 if(!x_code) x_code = i+1;
 } 
 
 count = 0;
 for(j=0;j<14;j++)	//현재 포맷의 숫자개수를 센다. MENU1464에 등록된 포맷
 {
 if(scannerBarForm[i][j]>=0x30 && scannerBarForm[i][j]<0x3A) count++;
 }
 if(count)               //숫자개수만큼만 비교한다. 
 {
 if(!strncmp((char *)scannerBarForm[i],(char *)stemp,count)) return i+1;
 }
 }
 if(x_code) return x_code;
 return 0;
 }
 */

/*
 //symbol의 시작위치와 개수를 알려준다.
 void getScannerMappingSymbolInfo(INT8U* index, INT8U* leng, char symbol, INT8U FormatNo, INT8U mode) //Added by JJANG 20071205
 {
 
 INT8U  flag, i;
 INT8U  scanForm[SCANNER_BARCODE_DATA_LEN + 1];
 INT8U  num_count=0;
 INT32U addr;
 INT16U len;
 
 if (mode)
 {
 len = SCANNER_BARCODE_DATA_LEN;
 addr = DFLASH_BASE  + FLASH_SCANNER_FORMAT;
 Flash_sread(addr+(FormatNo*BAR_FORMAT_SIZE), scanForm, len);
 }
 else
 {
 len = 32;	// 메모리가 32byte로 fix되어 있음
 get_global_sparam(GLOBAL_SCANNER_FORMAT, scanForm, len);
 }
 len = 13;
 scanForm[len] = 0;	 //EAN13 Only
 
 
 flag=0;
 *index=0;  *leng=0;
 for(i=0; i<len + 1; i++)     //EAN13 format only
 {
 if((scanForm[i] == symbol) && !flag) {flag =1; *index = i + 1;}
 if((scanForm[i] != symbol) && flag)  {*leng = 1 + i - (*index); break;}
 } 
 }
 */

/*
 //SG060122 
 void getScannerSymbolInfo(INT8U* index, INT8U* leng, char symbol)
 {
 
 INT8U  flag, i;
 INT8U  scanForm[32];
 
 get_global_sparam(GLOBAL_SCANNER_FORMAT, scanForm ,32);
 scanForm[13] = 0;	 //EAN13 Only
 
 flag=0;
 *index=0;  *leng=0;
 for(i=0; i<14; i++)  //EAN13 format only
 {
 if( (scanForm[i] == symbol) && !flag ){
 flag =1;
 *index = i + 1;
 }
 if( (scanForm[i] !=symbol) && flag ){
 *leng = 1 + i - (*index);
 break;
 }
 } 
 }
 */

/*
 INT32U scanner_get_weight(void)
 {
 INT8U i;
 INT8U wtStartIndex=0, wtLeng=0;
 INT32U wtValue;
 //	char temp[32];
 
 getScannerMappingSymbolInfo(&wtStartIndex,&wtLeng,'W',0,0); //'W' : Weight
 
 if ( wtStartIndex+wtLeng>14 ) return 0;   //EAN13 (13자리 수로 고정)
 
 if(wtLeng<=0 || wtLeng>13){
 //Error Message: Check Barcode Symbol
 BuzOn(3);
 return 0;
 }	
 
 wtValue = 0;
 for(i=wtStartIndex-1; i<(wtLeng+wtStartIndex-1); i++)
 {
 wtValue = wtValue*10 + (Scanner.barcodeData[i]-'0');
 }
 return wtValue;					  
 }
 */

extern INT8U scanner_get_data(STRUCT_STRFORM_PARAM *pBarParam, INT8U mode);

#define USE_PLU_NAME3_FOR_SCAN_TABLE

//Scan Format에서 'X','W','P'검출
void scanner_get_mapping_pluno(INT8U* deptNO, INT32U* pluNO, STRUCT_STRFORM_PARAM *pBarParam, INT32U* wValue, INT32U* pValue)
{
	INT16U cnt;
	INT16U usize;
	INT16S    str_size;
	SCANNER_CONV_TABLE_STR  scanConvTableStr;
	INT32U flash_addr, start_addr;
	PLU_BASE *pluBase;
	
	if (pBarParam->Scan_flag&BAR_SCANFLAG_WEIGHT)	//'W' : Weight
	{
		*wValue = pBarParam->Weight;
	}
	
	if (pBarParam->Scan_flag&BAR_SCANFLAG_FIXED_PRICE)	//'P' : Fixed Price
	{
		*pValue = pBarParam->TotalPrice;
	}
	
	if (pBarParam->Scan_flag&BAR_SCANFLAG_REMNANT)	//'X' : Remnant
	{
#ifdef USE_10000_SCANNER_CONVTABLE
		start_addr =  EXT_MEMCARD2_ADDR_X + FLASH_SCANNER_TABLE_AREA;
#else
		start_addr =  DFLASH_BASE + FLASH_SCANNER_TABLE_AREA;
#endif
		str_size = SCANNER_CONV_TABLE_STR_SIZE;
		
		cnt=0;
		while(1)
		{
			if(cnt > SCAN_BACODE_DATA_MAX_NUM) break;
			flash_addr = start_addr + (str_size * cnt);
			Flash_sread(flash_addr, (HUGEDATA INT8U *)&scanConvTableStr, str_size);
			scanConvTableStr.scanCode[13]=0;	// 13자리 제한
			if(!strcmp((char *)pBarParam->RemnantData,(char *)scanConvTableStr.scanCode ))
			{
				*deptNO = scanConvTableStr.dept_no;
				*pluNO = scanConvTableStr.plu_no;
				//break;
				return;
			}
			cnt++;
		}
		
#ifdef USE_PLU_NAME3_FOR_SCAN_TABLE
		// Scan Table 먼저 검색 후 PLU Name3 검색
		cnt=0;
		while(1)
		{
			if(cnt > MAX_PLU_NO) break;  
			plu_get_field(cnt, PTYPE_PLU_NAME3, (INT8U *)&scanConvTableStr.scanCode, (INT16S *)&usize, sizeof(scanConvTableStr.scanCode)); 
			scanConvTableStr.scanCode[13]=0;	// 13자리 제한
			if(!strcmp((char *)pBarParam->RemnantData,(char *)scanConvTableStr.scanCode ))
			{
				plu_get_field(cnt, PTYPE_DEPARTMENT_NO, (INT8U *)deptNO, (INT16S *)&usize, sizeof(pluBase->deptid)); 
				plu_get_field(cnt, PTYPE_PLU_NO, (INT8U *)pluNO, (INT16S *)&usize, sizeof(pluBase->PluCode)); 
				break;
			}
			cnt++;                
		}
#endif        
	}
}

/*
 void scanner_get_pluno(INT8U* deptNO, INT32U* pluNO)
 {
 INT8U  bsIndex, bsLeng;
 INT16U cnt;
 INT8U  stemp[14];
 INT16S    str_size;
 SCANNER_CONV_TABLE_STR  scanConvTableStr;
 INT32U flash_addr, start_addr;
 
 getScannerMappingSymbolInfo(&bsIndex,&bsLeng,'X',0,0); //'X' : scan code
 
 strncpy((char *)stemp,(char *)&Scanner.barcodeData[bsIndex-1],bsLeng);
 stemp[bsLeng]=0;
 
 start_addr =  DFLASH_BASE + FLASH_SCANNER_TABLE_AREA;
 str_size = SCANNER_CONV_TABLE_STR_SIZE;
 
 cnt=0;
 while(1){
 if(cnt > SCAN_BACODE_DATA_MAX_NUM) break;
 
 flash_addr = start_addr + (str_size * cnt);
 Flash_sread(flash_addr, (HUGEDATA INT8U *)&scanConvTableStr, str_size);
 scanConvTableStr.scanCode[13]=0;
 
 if(!strcmp((char *)stemp,(char *)scanConvTableStr.scanCode )){
 *deptNO = scanConvTableStr.dept_no;
 *pluNO = scanConvTableStr.plu_no;
 break;
 }
 cnt++;
 }
 }
 */

//#define MATCHING_FORMAT
INT8U scanner_get_data(STRUCT_STRFORM_PARAM *pBarParam, INT8U mode)
{
	INT8U i, len;
	INT8U inputbar_len;
#ifdef USE_LOTTEMART_MEATSCANNER
	INT8U scannerBarForm[8][SCANNER_BARCODE_DATA_LEN+1];
#else
	INT8U scannerBarForm[10][SCANNER_BARCODE_DATA_LEN+1];
#endif
	INT32U addr;
	INT8U  stemp[SCANNER_BAR_MAX_SIZE+1];
	INT8U ret;
	INT8U  count,j;
	INT8U modeScanner;
	
	modeScanner = get_global_bparam(GLOBAL_SCANNER_FLAG1);
	modeScanner = modeScanner&0x07;
	
	
	ret = OFF;
	memset(pBarParam, 0, sizeof(STRUCT_STRFORM_PARAM));
	
	strncpy((char *)stemp,(char *)&Scanner.barcodeData,SCANNER_BAR_MAX_SIZE);
	stemp[SCANNER_BAR_MAX_SIZE] = 0;
	inputbar_len = strlen((char *)stemp);
	if (mode)	// 10 format
	{
		addr = DFLASH_BASE  + FLASH_SCANNER_FORMAT;
#ifdef USE_LOTTEMART_MEATSCANNER
		for(i=0;i<8;i++)
#else
		for(i=0;i<10;i++)
#endif
		{
			Flash_sread(addr+(i*BAR_FORMAT_SIZE), scannerBarForm[i], SCANNER_BAR_MAX_SIZE);
			scannerBarForm[i][SCANNER_BAR_MAX_SIZE] = 0;
			len = strlen((char *)scannerBarForm[i]);
			if (modeScanner == 4)
			{
#ifdef USE_LOTTEMART_MEATSCANNER
				if ((inputbar_len >= SCANNER_BAR_MAX_SIZE) && (len == SCANNER_BARCODE_DATA_LEN))
				{
					for(j = 0; j < SCANNER_BARCODE_DATA_LEN; j++)
					{
						stemp[j] = stemp[j+15];
					}
					stemp[j] = 0;
					prt_scan_strform((char *)stemp, (char *)scannerBarForm[i], pBarParam);
					ret = ON;
					break;
				}
				else
				{
#endif
#ifdef USE_SETPRODUCT_SALE
				if (len == 18 && inputbar_len == 20 && scannerBarForm[i][0] != 'T')
				{
					for(j = 12; j < 18; j++)
					{
						stemp[j] = stemp[j+1];
					}
					stemp[j] = 0;
					prt_scan_strform((char *)stemp, (char *)scannerBarForm[i], pBarParam);
					ret = ON;
					break;
				}
				else
				{
#endif
#if defined(USE_IMPORT_LOTNO) && defined(USE_MANUAL_INPUT_IMPORT_LOTNO)
				if (len == 24 && inputbar_len == 24 && scannerBarForm[i][0] != 'T')
				{
					if(stemp[0] == 'A') continue;
					prt_scan_strform((char *)stemp, (char *)scannerBarForm[i], pBarParam);
					ret = ON;
					break;
				}
				else
				{
#endif
#ifdef USE_HYUNDAI_SET_FUNCTION
				if (len == 16 && inputbar_len == 20)
				{
					for(j = 0; j < 16; j++)
					{
						if(j < 12) stemp[j] = stemp[j+2];
						else stemp[j] = stemp[j+3];
					}
					stemp[j] = 0;
					prt_scan_strform((char *)stemp, (char *)scannerBarForm[i], pBarParam);
					ret = ON;
					break;
				}
				else
				{
#endif
				if (inputbar_len == len)
				{
#ifdef USE_EMART_PIG_TRACE
					if(len == SCANNER_BAR_MAX_SIZE && stemp[6] != 'A') continue; // 32자 Data 들어올 때 묶음번호(상품코드6자리 뒤)가 'A'로 시작하지 않는 경우,
#endif
					prt_scan_strform((char *)stemp, (char *)scannerBarForm[i], pBarParam);
					ret = ON;
					break;
				}
#ifdef USE_LOTTEMART_MEATSCANNER
				}
#endif
#ifdef USE_SETPRODUCT_SALE
			}
#endif
#if defined(USE_IMPORT_LOTNO) && defined(USE_MANUAL_INPUT_IMPORT_LOTNO)
			}
#endif
#ifdef USE_HYUNDAI_SET_FUNCTION
			}
#endif
			}
			else
			{
				if (inputbar_len >= len)

				{
					count = 0;
					for(j=0;j<SCANNER_BAR_MAX_SIZE;j++)	//현재 포맷의 숫자개수를 센다. MENU1464에 등록된 포맷
					{
						if (scannerBarForm[i][j] >= '0' && scannerBarForm[i][j] <= '9') count++;
						else break;
					}
					if (count)               //숫자개수만큼만 비교한다. 
					{
						if (strncmp((char *)scannerBarForm[i],(char *)stemp,count)) continue;
					}
					prt_scan_strform((char *)stemp, (char *)scannerBarForm[i], pBarParam);
					ret = ON;
					break;
				}
			}
		}
	}
	else	// 1 format(old)
	{
		get_global_sparam(GLOBAL_SCANNER_FORMAT, scannerBarForm[0], 32);	// 메모리가 32byte로 fix되어 있음
		scannerBarForm[0][SCANNER_BAR_MAX_SIZE] = 0;
		len = strlen((char *)scannerBarForm[0]);
		if (inputbar_len >= len)
		{
			prt_scan_strform((char *)stemp, (char *)scannerBarForm[0], pBarParam);
			ret = ON;
		}
	}
	
	return ret;
}

//Added by JJANG 20081212
#ifdef USE_TRACE_STANDALONE

// return value : 0 - 중복안됨, 1~3 - 중복되는 개체
INT8U compareMultiIndividual(INT8U *ind_no_str)
{
	INT8U i;
	
	for (i = 0; i < MultiIndex; i++)
	{
		if (strcmp((char*)ind_no_str, (char*)IndivdualMultiData[i].indivNoStr) == 0)
		{
			return i + 1;
		}
	}
	return 0;
}

#define LOTTEMART_ITEMCODE_SEARCH_BASE_PLUNO		1L

extern INT8U cksum_indivNo(char *buf);
extern INT8U command_ask_eco_barcode(char* barString);
INT8U scanner_interprete_data(STRUCT_STRFORM_PARAM *pBarParam)
{
	INT8U string_buf[50];
	INT16U ind_idx;
	INT8U lotFlag;
#ifdef USE_LOTTEMART_ITEMCODE13
	INT32U retPluNo;
	INT8U len;
#endif
#ifdef USE_LOTTEMART_MEATSCANNER
	INT8U j;
#endif
#ifdef USE_SCANNER_FOR_ECO_FRIENDLY
	INT8U ret = 0xff;
	POINT disp_p;
	char old_font;
	INT8U old_page;
#endif
	memset(&CalledTraceStatus, 0, sizeof(CalledTraceStatus));
	if (pBarParam->Scan_flag&BAR_SCANFLAG_PLUNO && !ScanFlag)	//pluno, when ScanFlag is SET
	{
		if (ModeIndiv == 5)
		{
			if (pBarParam->Scan_flag&BAR_SCANFLAG_TEXT && MultiIndex > 0)	// 개체스캔이 되고, 복수개체가 시작되면
			{
				if (status_scale.cur_pluid != pBarParam->PluNumber && FlagIndividualMultiStart)
				{
					BuzOn(2);
					sprintf((char*)string_buf, "상품코드가 다릅니다");
					display_message_page_mid((char*)string_buf);
					return OFF;
				}
			}
		}
#ifdef USE_SETPRODUCT_SALE
		if(pBarParam->PluNumber == 0) return OFF; 
#endif
#ifdef USE_HYUNDAI_SET_FUNCTION
		if(MeatSetSaleMode && Operation.saleStatus != SALE_SET_START) MeatSetSaleUseScanner = ON;
#endif
		keyapp_pluno(status_scale.departmentid, pBarParam->PluNumber, OFF);
	}
#ifdef USE_LOTTEMART_ITEMCODE13
	if (pBarParam->Scan_flag&BAR_SCANFLAG_ITEMCODE13)	//ITEMCODE13
	{

			len=strlen(pBarParam->ItemCode13); //Write Barcode

			retPluNo = lottemart_itemcode_plu_search(LOTTEMART_ITEMCODE_SEARCH_BASE_PLUNO, pBarParam->ItemCode13, len);
			//plu Call
			if(retPluNo)
			{
				keyapp_pluno(0,retPluNo,OFF);
			//Added by JJANG 20070803 셀프모드에서 개수 상품 발행시 자동 발행
				if((Operation.operationMode == MODE_SELF) && (KeyDrv.CnvCode == KS_PLU)) SelfserviceAutoCheck();
			}
			else
			{
				BuzOn(2);
			}
	}
#endif
#ifdef USE_LOTTEMART_MEATSCANNER
	if (pBarParam->Scan_flag&BAR_SCANFLAG_TEXT)
	{
		CurTraceStatus.no14indivflag = pBarParam->No14_indiv_flag;
		
		for(j = 0; j < 24 ; j++)
		{
			CurTraceStatus.indivno[j] = pBarParam->Indiv_no[j];
		}
		sale_display_indivFlag_set(INDIV_DISP_MODE_INDIV_NO, ON);
		sale_display_individual();
		CurTraceStatus.meatscannerflag = 1;
	}
#else

	if (pBarParam->Scan_flag&BAR_SCANFLAG_TEXT)	//Text
	{
		lotFlag = checkIndividualNo(pBarParam->Text);
		if (ModeIndiv == 2)	// 농협만 적용
			
		{
  #ifdef USE_TOPMART_STRUCTURE //sewon 161129
			if (!plu_check_inhibit_ptype(PTYPE_TR_COMP_FLAG)) //sewon 161121
  #else
			if (!plu_check_inhibit_ptype(PTYPE_TRACE_FLAG))
  #endif
			{
				if (status_scale.cur_pluid)
				{
#ifdef USE_TRACE_FLAG_0_CHECK
					if (!status_scale.Plu.trace_flag)
					{
						BuzOn(2);
						//sprintf((char*)string_buf, "개체이력대상이 아닙니다");
						sprintf((char*)string_buf, IndivMsg_NonTrace);
						display_message_page_mid((char*)string_buf);
						return OFF;
					}		
#endif		
				}
				else 
				{
					BuzOn(2);
					return OFF;
				}
				
			}
		}
		if (UseCheckIndivNo && !lotFlag)	// 묶음번호는 체크안함
		{
			if (cksum_indivNo((char *)pBarParam->Text) == FALSE) 
			{
				BuzOn(2);
				sprintf((char*)string_buf, "이력번호 오류!");
				display_message_page_mid((char*)string_buf);
				return OFF;
			}
		}  
		if (ModeIndiv == 5)
		{        
			if (compareMultiIndividual(pBarParam->Text) && FlagIndividualMultiStart)	// 복수개체중 중복이 있으면
			{
				BuzOn(2);
				//sprintf((char*)string_buf, "중복된 개체번호입니다");
				sprintf((char*)string_buf, IndivMsg_SameTraceNo);
				display_message_page_mid((char*)string_buf);
				return OFF;
			}
		}
		//CalledTraceStatus.indivStr.lotFlag = checkIndividualNo(pBarParam->Text);
		CalledTraceStatus.indivStr.lotFlag = lotFlag;
		memcpy(CalledTraceStatus.indivStr.individualNO, pBarParam->Text, sizeof(CalledTraceStatus.indivStr.individualNO));
#ifdef USE_EMART_PIG_TRACE
		if(strlen((char*)pBarParam->Text) == 24)
		{
			CalledTraceStatus.indivStr.individualNO[24] = 0;
			memcpy(ImportLotNumber, pBarParam->Text, 24);
			ImportLotNumberflag = 1;
			
		}
		else
		{
			CalledTraceStatus.indivStr.individualNO[INDIVIDUAL_NO_LEN] = 0;
			ImportLotNumberflag = 0;
		}
#else
		CalledTraceStatus.indivStr.individualNO[INDIVIDUAL_NO_LEN] = 0;
#endif
		individual_checkZero(CalledTraceStatus.indivStr.individualNO);
#ifdef USE_TOPMART_ASK_INDIV_INFO //sewon 170214
		if(strncmp(CalledTraceStatus.indivStr.individualNO,"777777777777",12))
		{
			ind_idx = individualData_search(CalledTraceStatus.indivStr.individualNO, 0);

			if(ind_idx == MAX_INDIVIDUAL_NO)
			{
				if (!keyapp_ask_indivInfo(CalledTraceStatus.indivStr.individualNO)) return OFF;
			}
		}// 이력번호가 777777777777 면 서버에서 체크 안함.
#endif
		ind_idx = individualData_search(CalledTraceStatus.indivStr.individualNO, 0);
		//keyapp_call_individual(ind_idx, OFF);
		if(!individual_call(ind_idx)){
			sale_pluclear(ON);
			return OFF;
		}
	}
#endif //USE_LOTTEMART_MEATSCANNER

#ifdef USE_LOTTEMART_MEATSCANNER
		if (pBarParam->Scan_flag&BAR_SCANFLAG_MEATUSE)	//Meat Use
		{
			CurTraceStatus.meatUseNo = pBarParam->Meat_use_no;
			//sale_display_indivFlag_set(INDIV_DISP_MODE_SPECIALUSE, ON);
			//sale_display_individual();
			CurTraceStatus.meatscannerflag = 1;
		}
#else
	if (pBarParam->Scan_flag&BAR_SCANFLAG_MEATUSE)	//Meat Use
	{
		CurTraceStatus.meatUse = pBarParam->MeatUseNo;
		sale_display_indivFlag_set(INDIV_DISP_MODE_SPECIALUSE, ON);
		sale_display_individual();
	}
#endif
	if (pBarParam->Scan_flag&BAR_SCANFLAG_ORIGIN)	//Origin
	{
		status_scale.Plu.origin= pBarParam->CountryNo;
		Operation.keyClear = OFF;
	}
#ifdef USE_TRACE_MEATPART
	if (pBarParam->Scan_flag&BAR_SCANFLAG_MEATPART)	//Meat Part
	{
		CurTraceStatus.meatPart = pBarParam->MeatPartNo;
		sale_display_indivFlag_set(INDIV_DISP_MODE_MEATPART, ON);
		sale_display_individual();
	}
#endif    
#ifdef USE_LOTTEMART_MEATSCANNER
	if (pBarParam->Scan_flag&BAR_SCANFLAG_MEATGRADE)	//Meat grade
	{
		CurTraceStatus.meatgradeNo = pBarParam->Meat_grade_no;
		trace_meatgrade_getGradeText(CurTraceStatus.meatgradeNo, CurTraceStatus.curGradeText);
		CurTraceStatus.meatscannerflag = 1;
	}
#else
  #ifdef USE_TRACE_MEATGRADE
    #ifdef USE_TRACE_SCANNER_FOR_GRADE
      #ifdef USE_DISCLOSURE_MEATGRADE
	if (pBarParam->Scan_flag&BAR_SCANFLAG_MEATGRADE)	//Meat Part
	{
		CurTraceStatus.gradeNo = pBarParam->MeatGradeNo;
		trace_meatgrade_getGradeText(CurTraceStatus.gradeNo, CurTraceStatus.curGradeText);
		sale_display_indivFlag_set(INDIV_DISP_MODE_MEATGRADE, ON);
		sale_display_individual();
	}
      #else
	if (pBarParam->Scan_flag&BAR_SCANFLAG_MEATGRADE)	//Meat Part
	{
		if (plu_check_inhibit_ptype(PTYPE_GROUP_NO))	// PLU 등급을 사용하지 않을 때 Scanner 등급 호출 가능
		{
			CurTraceStatus.gradeNo = pBarParam->MeatGradeNo;
			trace_meatgrade_getGradeText(CurTraceStatus.gradeNo, CurTraceStatus.curGradeText);
			sale_display_indivFlag_set(INDIV_DISP_MODE_MEATGRADE, ON);
			sale_display_individual();
		}
	}
      #endif //  USE_DISCLOSURE_MEATGRADE	
    #endif // USE_TRACE_SCANNER_FOR_GRADE
  #endif //USE_TRACE_MEATGRADE
#endif //USE_LOTTEMART_MEATSCANNER
#ifdef USE_LOTTEMART_MEATSCANNER
	if (pBarParam->Scan_flag&BAR_SCANFLAG_SL_NO)	//Slaughter House
	{
		CurTraceStatus.slPlace1= pBarParam->SlaughtHouseNo1;
		CurTraceStatus.meatscannerflag = 1;
	}
	if (pBarParam->Scan_flag&BAR_SCANFLAG_SL_NO2)	//Slaughter House2
	{
		CurTraceStatus.slPlace2= pBarParam->SlaughtHouseNo2;
		CurTraceStatus.meatscannerflag = 1;
	}
	
	if (pBarParam->Scan_flag&BAR_SCANFLAG_SL_NO3)	//Slaughter House3
	{
		CurTraceStatus.slPlace3= pBarParam->SlaughtHouseNo3;
		CurTraceStatus.meatscannerflag = 1;
	}
#else
	if (pBarParam->Scan_flag&BAR_SCANFLAG_SL_NO)	//Slaughter House
	{
  #ifdef USE_KOR_SLAUGHT_CODE_4DIGIT
		CurTraceStatus.slPlace = trace_slaughthouse_no2index(pBarParam->SlaughtHouseNo);
  #else
		CurTraceStatus.slPlace = pBarParam->SlaughtHouseNo;
  #endif
		sale_display_indivFlag_set(INDIV_DISP_MODE_SLAUGHTHOUSE, ON);
		sale_display_individual();
	}
#endif
	if (pBarParam->Scan_flag&BAR_SCANFLAG_WEIGHT)	//Weight
	{
		if (pBarParam->Weight)
		{
#ifdef USE_HYUNDAI_SET_FUNCTION
			if (MeatSetSaleMode) // 세트 판매 중 바코드 스캔하면 스캔해서 읽은 무게 전송(라벨 인쇄 X)
			{
				status_scale.Plu.fixed_weight = pBarParam->Weight;
				Operation.keyClear = OFF; 
			}
#else		
			status_scale.Plu.fixed_weight = pBarParam->Weight;
			//KEY_Write(keyin_getrawcode_bytype(0, KS_PRINT), 0);
			Operation.keyClear = OFF;
#endif
		}
	}
	if (pBarParam->Scan_flag&BAR_SCANFLAG_TARE_WEIGHT)	//Tare Weight
	{
		if (pBarParam->TareWeight)
		{
			status_scale.Plu.tare = pBarParam->TareWeight;
			Operation.keyClear = OFF;
		}
	}
#ifdef USE_SCANNER_FOR_ECO_FRIENDLY	
	if (pBarParam->Scan_flag & BAR_SCANFLAG_CODE_880)	//현대그린푸드 친환경 바코드	
	{		
		old_font = DspStruct.Id1;
		old_page = DspStruct.Page;
		Dsp_SetPage(DSP_ERROR_PAGE);
		FontSizeChange(FONT_MEDIUM);
		display_lcd_clear_buf();	// Clear buf PLU Name Area
		disp_p = point(DSP_PLU_NAME_Y, 10);
		Dsp_Write_String(disp_p, "조회 중입니다.");
		display_lcd_diffuse();	// Redraw PLU Name Area
		
		ret = command_ask_eco_barcode(pBarParam->code880);
		
		switch(ret)
		{
			case 0 : 
				BuzOn(2);
				sprintf((char*)string_buf, "인증 상품입니다.");
				break;
			case 1 : 
				BuzOn(3);
				sprintf((char*)string_buf, "인증상품이 아닙니다.");
				break;
			case 2 : 
				BuzOn(3);
				sprintf((char*)string_buf, "DATA 없음");
				break;
			case 3 :
				BuzOn(3);
				sprintf((char*)string_buf, "DB 접속 오류");
				break;
			default : 
				BuzOn(3);
				sprintf((char*)string_buf, "조회 오류");
				break;
		}
		display_lcd_clear_buf();	// Clear buf PLU Name Area
		Dsp_Write_String(disp_p, string_buf);
		display_lcd_diffuse();	// Redraw PLU Name Area
		Key_DelaySec(30, 0);
		Dsp_SetPage(old_page);
		Dsp_Diffuse();
		DspLoadFont1(old_font);
		return ON;
	}
#endif
#ifdef USE_HYUNDAI_SET_FUNCTION
	if (pBarParam->Scan_flag & BAR_SCANFLAG_RECEIPT_NO)
	{
		if(MeatSetSaleMode)
			memcpy(ReceiptData, pBarParam->ReceiptNo, strlen(pBarParam->ReceiptNo));
		else
			memset(ReceiptData, 0x00, sizeof(ReceiptData));
	}
#endif	
	if (FlagIndividualMultiStart)//복수개체 입력
	{
		if (MultiIndex < MAX_INDIVIAL_MULTI_DATA_CNT)
		{
			memcpy(IndivdualMultiData[MultiIndex].indivNoStr, CurTraceStatus.indivStr.individualNO, INDIVIDUAL_NO_LEN);
#ifdef USE_TOPMART_STRUCTURE //sewon 170105
			IndivdualMultiData[MultiIndex].slaughterHouse = CurTraceStatus.indivStr.slaughtNo;
#else
			IndivdualMultiData[MultiIndex].slaughterHouse = CurTraceStatus.slPlace;
#endif
			MultiIndex++;
#ifdef USE_EMART_NETWORK
			FlagIndividualMultiStart = OFF;
			FlagIndividualMultiRemain = ON;
#endif
		}
		else
		{
			BuzOn(2);
		}
	}
	else
	{
		FlagIndividualMultiRemain = OFF;
		MultiIndex = 0;
		memset(IndivdualMultiData, 0, sizeof(TRACE_INDIVIDUAL_MULTI_DATA) * MAX_INDIVIAL_MULTI_DATA_CNT);
	}
	if (FlagLotMultiStart)	//장부묶음 입력
	{
		if (LotListMultiIndex < MAX_LOT_MULTI_DATA_CNT)
		{
			memcpy(LotMultiData[LotListMultiIndex].indivNoStr, CurTraceStatus.indivStr.individualNO, INDIVIDUAL_NO_LEN);
			LotMultiData[LotListMultiIndex].pluNo = status_scale.cur_pluid;
			LotMultiData[LotListMultiIndex].deptNo = status_scale.departmentid;
			LotMultiData[LotListMultiIndex].slaughterNo = CurTraceStatus.slPlace;
			LotListMultiIndex++;
		}
		else
		{
			BuzOn(2);
		}
	}
	return ON;
}
#endif

void keyapp_scanprint(void)
{
	INT32U sCode=0;
	INT8U  ucTemp;
	INT8U  modeScanner;
	INT8U  deptno;
	INT32U pluno;
	INT32U T_wValue;        //tichet
	INT32U T_pValue;        //tichet
	INT8U  Scanform;	//parameter 558 : yes
	STRUCT_STRFORM_PARAM bar_param;
#ifdef USE_KOR_TRACE
	INT16U len;
#endif
#ifdef USE_TRACE_STANDALONE
	INT16U indivIdx;
#endif
	INT8U  ret;
	INT8U  keycode;
#ifdef USE_SETPRODUCT_SALE
	INT8U  buf[20];
#endif
	
	ucTemp = get_global_bparam(GLOBAL_SCANNER_FLAG1);
	modeScanner = ucTemp&0x07;
	Scanform = ucTemp&0x10;	// 0 : 1-format, 1 : 10-formats
	
	//PS2에 Barcode Scanner를 연결하여 사용 시 
	if(UseScanner)
	{
		deptno=0; pluno=0;
		T_wValue=0; T_pValue=0;
		
		ret = scanner_get_data(&bar_param, Scanform);
		if (!ret)
		{
			BuzOn(2);
			return;
		}
		if(modeScanner == 3) 
		{
#ifdef USE_TRACE_STANDALONE
			if (bar_param.Scan_flag&BAR_SCANFLAG_REMNANT)	// 'X' : Remnant
			{
				CalledTraceStatus.indivStr.lotFlag = checkIndividualNo(bar_param.RemnantData);
				memcpy(CalledTraceStatus.indivStr.individualNO, bar_param.RemnantData, sizeof(CalledTraceStatus.indivStr.individualNO));
				CalledTraceStatus.indivStr.individualNO[INDIVIDUAL_NO_LEN] = 0;
				individual_checkZero(CalledTraceStatus.indivStr.individualNO);
				indivIdx = individualData_search(CalledTraceStatus.indivStr.individualNO, 0);
				individual_call(indivIdx);
			}
#endif
		}
		else if (modeScanner == 4) 
		{
#ifdef USE_TRACE_STANDALONE
			ret = scanner_interprete_data(&bar_param);
#ifdef USE_SETPRODUCT_SALE
			if(SetproductSaleMode)
			{
				if (!ret)
				{
					BuzOn(2);
					sprintf((char *)buf, "스캔 오류");
					display_message_page_mid((char *)buf);
					return;
				}
				if (!bar_param.Weight)
				{
					BuzOn(2);
					sprintf((char *)buf, "무게 스캔 오류");
					display_message_page_mid((char *)buf);
					return;
				}
				if (bar_param.PluNumber)
				{
					KEY_Write(keyin_getrawcode_bytype(0, KS_PRINT), 0, 0);
					Operation.keyClear = OFF;
					setproduct_scan_flag = ON;
				}
			}
#endif
			if (!ret) return;
#endif
#ifdef USE_KOR_TRACE
			if (bar_param.Scan_flag&BAR_SCANFLAG_TEXT)	// 'T' : Text
			{
				memset(&KorTrace_Flag.CurBarData[0], 0, sizeof(KorTrace_Flag.CurBarData));
				len = strlen(bar_param.Text);
				memset(&KorTrace_Flag.CurBarData[0], 0x30, 5);
				memcpy((char *)&KorTrace_Flag.CurBarData[5], &bar_param.Text[0], len - KORTR_PROCESS_NO_LEN);
				KorTrace_Flag.CurBarData[37] = '-';
				sprintf((char *)&KorTrace_Flag.CurBarData[38], &bar_param.Text[len - KORTR_PROCESS_NO_LEN], KORTR_PROCESS_NO_LEN);
				if (network_status.service_bflag2)
				{
					keyapp_ktr_req_nextid(KORTR_REQ_FIXED_ID);
				}
				else
				{
					KorTrace_Flag.flagReceive = 1;
				}
				kortrace_make_display_data(KorTrace_Flag.display_data, KorTrace_Flag.CurBarData, sizeof(KorTrace_Flag.display_data));
			}
#endif
		}
		else
		{
			//if(!Scanform) scanner_get_pluno(&deptno, &pluno);
			//else scanner_get_mapping_pluno(&deptno, &pluno, &T_wValue, &T_pValue);  //LRE + Tichet   
			scanner_get_mapping_pluno(&deptno, &pluno, &bar_param, &T_wValue, &T_pValue);   
#ifdef USE_GERMANY_TARE_SCANNER
			pluno = bar_param.PluNumber;
			deptno = bar_param.DeptId;
			if(!deptno)deptno = status_scale.departmentid;
			if(bar_param.TareWeight)
			{
				keyapp_tare(TARE_TYPE_KEY,bar_param.TareWeight, OFF);
			} 
#else
			if(!deptno || !pluno) return;
#endif			
			
			if (keyapp_pluno(deptno, pluno, OFF))
			{
				if (modeScanner == 0)      //PLU 호출 만 함
				{
				}
				else if (modeScanner == 1)	//PLU 호출 후 라벨 인쇄 (일반 모드).짐판위의 weight 적용
				{
					
					if (bar_param.TotalPrice)
					{
						status_scale.cur_unitprice = bar_param.TotalPrice;
						keyapp_fixed_price(0,0);                                                
					}
					if(get_global_bparam(GLOBAL_SALERUN_MODE)==1)   // Ticket mode
					{
						if (bar_param.Weight)
						{
							status_scale.Plu.fixed_weight = bar_param.Weight;                                                      
						}
						
				        if (Scan_clerk == 0)	//ticket MODE 일때, 특정 점원을 선택하여 사용함 
				        {
				        	HiddenClerkNumber = 0;
				        	//PS2_Write(KS_CLERK_CALL);	//입력한 값이 없으면 Clerk을 호출한다. 
							KEY_Write(keyin_getrawcode_bytype(0, KS_CLERK_CALL), 0, 0);
							Operation.keyClear = OFF;
						}
				        else	//입력된 점원에게 상품을 판매한다. 수정필요.
						{
							//PS2_Write(Scan_clerk+KS_CLERK_01-1);	//입력된 점원에게 상품을 판매한다. 수정필요.
							KEY_Write(keyin_getrawcode_bytype(0, Scan_clerk+KS_CLERK_01-1), 0, 0);
							Operation.keyClear = OFF;
						}
					}
					else	//라벨모드
					{
						//PS2_Write(KS_PRINT);  //라벨모드에서 바로 출력함
						KEY_Write(keyin_getrawcode_bytype(0, KS_PRINT), 0, 0);
						Operation.keyClear = OFF;
					}
				}
				else if (modeScanner == 2)	//PLU 호출 후 라벨 인쇄	(Prepack).barcode의 weight 적용
				{
					if (bar_param.Weight)
					{
						status_scale.Plu.fixed_weight = bar_param.Weight;
						KEY_Write(keyin_getrawcode_bytype(0, KS_PRINT), 0, 0);
						Operation.keyClear = OFF; 
						/*
						 status_scale.Weight = T_wValue;
						 
						 sale_calcprice();
						 sale_display_update(0x0fff);
						 sale_display_proc(OFF);
						 
						 SaleInsertLabelData();
						 StartPrintFlag = ON;
						 Prt_PrintStart(PRT_ITEM_NORMAL_MODE, PrtStruct.Mode, 0, OFF, 0, 0);
						 PrintListWaitEnd();
						 SaleAcctLocalAddTransaction(status_scale.Plu.deptid,status_scale.cur_pluid,&Price,0);
						 StartPrintFlag = OFF;
						 
						 sale_pluclear(ON);      
						 sale_display_update(0x0fff);
						 */
					}
				}
			}
		} 
	}
	//PS2에 KEYBOARD 연결하여 사용시 "PRINT" 키로 사용
	else PS2_Write(KS_PRINT);  
}

INT16U LabelNumber = 0;		//임시 라벨 포맷 번호. 
INT16U BarcodeNumber = 0;	//임시 바코드 포맷 번호. 
void keyapp_labelformat_change(INT8U keyNum)
{
	INT32U addr;
	INT32U addr_global;
	CAPTION_STRUCT	cap;	
	INT8U   common;
	INT8U	s_flag;
	INT16S i;
	char temp[30];
	
	//#ifdef USE_TRACE_STANDALONE
	LABEL_KEY_STRUCT	label;
	INT8U	strSize;
	INT8U	common_bar;
#ifdef USE_LOTTESUPER_DEFAULT
	INT8U barcodefmtNo_forWeightPLU;
	INT8U barcodefmtNo_forCountPLU;
#endif
	
	strSize = sizeof(LABEL_KEY_STRUCT);
		s_flag = get_global_bparam(GLOBAL_SALE_SETUP_FLAG9)&0x80;//parameter 725
	
	addr_global = DFLASH_BASE;
	addr_global += FLASH_GLOBAL_AREA;
	
	addr = FLASH_LABEL_KEY_TABLE_AREA + DFLASH_BASE;
	addr += (keyNum-1) * strSize;
	label.fmtNo = Flash_wread(addr);
	label.barcodefmtNo = Flash_wread(addr+2);
	
	caption_split_by_code(0x2018,&cap,0); //plu label no.
	if((label.fmtNo < cap.input_min) || (label.fmtNo > cap.input_max) || (label.fmtNo == 0))
	{
		BuzOn(2);
		return;
	}
	caption_split_by_code(0x352B,&cap,0); //barcode no.
	if((label.barcodefmtNo < cap.input_min) || (label.barcodefmtNo > cap.input_max) || (label.barcodefmtNo == 0))
	{
		BuzOn(2);
		return;
	}
#ifndef USE_EMART_BACK_LABEL
	if (s_flag)	//20090416 Added JJANG 라벨 1회 변경
	{		
		if (label.fmtNo != 0) LabelNumber = label.fmtNo;
		if (label.barcodefmtNo != 0) BarcodeNumber = label.barcodefmtNo;
	}
	else
	{
		if (label.fmtNo != 0)
		{
			common = get_global_bparam(GLOBAL_SALE_SETUP_FLAG3)&0x80; //라벨 일괄설정
			if(common)
			{
				for(i=0; i<4; i++)  {
					addr = addr_global + GLOBAL_LABEL_STANDARD;
					addr = addr + i*2;
					if(i==3) addr = addr_global + GLOBAL_LABEL_FSP;
					Flash_wwrite(addr, label.fmtNo);
				}			
			}
			else  //change standard label only
			{
				addr = addr_global + GLOBAL_LABEL_STANDARD;
				Flash_wwrite(addr, label.fmtNo);	
			}
		}
		
		if (label.barcodefmtNo != 0)
		{
			common_bar = get_global_bparam(GLOBAL_SALE_SETUP_FLAG3)&0x40;//바코드 일괄설정		
			if (common_bar) 
			{	
				for(i = 0; i < 9; i++)  
				{
					addr = addr_global + GLOBAL_BARCODE_STD_WID;
					addr = addr + i;
					Flash_bwrite(addr, (INT8U)label.barcodefmtNo);
				}
			}
			else 
			{
  #ifdef USE_LOTTESUPER_DEFAULT
				if(keyNum == 1) // 일반라벨 -> 상품 종류별로 다른 포맷을 매칭 (무게 or 개수)
				{
					addr = FLASH_LABEL_KEY_TABLE_AREA + DFLASH_BASE;
					addr += 2 * strSize;  // (keyNum-1) * strSize -> 3번째 키값에 들어 있는 포맷 (일반_무게)
					barcodefmtNo_forWeightPLU = Flash_wread(addr+2);

					addr = FLASH_LABEL_KEY_TABLE_AREA + DFLASH_BASE;
					addr += 3 * strSize;  // (keyNum-1) * strSize -> 4번째 키값에 들어 있는 포맷 (일반_무게)
					barcodefmtNo_forCountPLU = Flash_wread(addr+2);

					// 상품 종류별 바코드 형식 매핑
					set_global_bparam(GLOBAL_BARCODE_STD_WID, barcodefmtNo_forWeightPLU);		// 무게 바코드
					set_global_bparam(GLOBAL_BARCODE_STD_CID, barcodefmtNo_forCountPLU);		// 개수 바코드
					set_global_bparam(GLOBAL_BARCODE_STD_NID, barcodefmtNo_forWeightPLU);		// 수량 바코드
					set_global_bparam(GLOBAL_BARCODE_STD_FIX, barcodefmtNo_forWeightPLU);		// 정가 바코드
					set_global_bparam(GLOBAL_BARCODE_NON_WID, barcodefmtNo_forWeightPLU);		// 오픈 무게 바코드 (Non-PLU)
					set_global_bparam(GLOBAL_BARCODE_NON_CID, barcodefmtNo_forCountPLU);		// 오픈 개수 바코드 (Non-PLU)
					set_global_bparam(GLOBAL_BARCODE_NON_NID, barcodefmtNo_forWeightPLU);		// 오픈 수량 바코드 (Non-PLU)
					set_global_bparam(GLOBAL_BARCODE_TOTAL_ID, barcodefmtNo_forWeightPLU);		// 합산판매 바코드
					set_global_bparam(GLOBAL_BARCODE_TOTAL_ID2, barcodefmtNo_forWeightPLU);		// 합산판매 바코드2
				}
				else	// 일반라벨이 아닐 때 (보관라벨) 바코드 형식 - 일괄 적용
				{
					for(i=0; i<9; i++)
					{
						set_global_bparam(GLOBAL_BARCODE_STD_WID+i, (INT8U)label.barcodefmtNo);
					}
				}
  #else
				addr = addr_global + GLOBAL_BARCODE_STD_WID;
				Flash_bwrite(addr, (INT8U)label.barcodefmtNo);
    #ifdef USE_LOTTEMART_DEFAULT
				addr = addr_global + GLOBAL_BARCODE_STD_FIX;
				Flash_bwrite(addr, (INT8U)label.barcodefmtNo);
    #endif
  #endif // USE_LOTTESUPER_DEFAULT
			}
		}
	}
	
	sprintf(temp,"L#:%2d, B#:%2d", label.fmtNo, label.barcodefmtNo);
	DisplayMsg(temp); 
  #ifdef USE_HYUNDAI_SET_FUNCTION
	if(label.fmtNo == get_labelid_from_labelkey_num(MEAT_STORAGE_LABEL_KEY_NUM)) {
		Operation.meatStorageLabelFlag = ON;
	}
	else{
		Operation.meatStorageLabelFlag = OFF;
	}
  #endif
#else
	Operation.BackLabeledFlag = Operation.BackLabeledFlag^1;
	if (Operation.BackLabeledFlag)
	{
		if (label.fmtNo != 0) LabelNumber = label.fmtNo;
		if (label.barcodefmtNo != 0) BarcodeNumber = label.barcodefmtNo;
		sprintf((char *)temp, "후방라벨");
		display_message_page_mid((char *)temp);
	}
	else
	{
		LabelNumber = 0;
		BarcodeNumber = 0;
		sprintf((char *)temp, "일반라벨");
		display_message_page_mid((char *)temp);
	}
	sale_display_update(UPDATE_MODE);
#endif
	//#else
	//	INT16U  labelNum;	 	
	//	s_flag = get_global_bparam(GLOBAL_SALE_SETUP_FLAG9)&0x80;
	//
	//	addr_global = DFLASH_BASE;
	//	addr_global += FLASH_GLOBAL_AREA;
	//
	//	addr = FLASH_LABEL_KEY_TABLE_AREA + DFLASH_BASE;
	//	addr += (keyNum-1) * 2;
	//	labelNum = Flash_wread(addr);
	//
	//	caption_split_by_code(0x2012,&cap,0); //plu label no.
	//	if(labelNum<cap.input_min || labelNum > cap.input_max)
	//	{
	//		BuzOn(2);
	//		return;
	//	}
	//
	//	if (s_flag)	//20090416 Added JJANG 라벨 1회 변경
	//	{
	//		LabelNumber = labelNum;
	//		//BarcodeNumber = label.barcodefmtNo;
	//	}
	//	else
	//	{
	//		common = get_global_bparam(GLOBAL_SALE_SETUP_FLAG3)&0x80;
	//		if(common)
	//		{
	//			for(i=0; i<4; i++)  {
	//				addr = addr_global + GLOBAL_LABEL_STANDARD;
	//				addr = addr + i*2;
	//				if(i==3) addr = addr_global + GLOBAL_LABEL_FSP;
	//				Flash_wwrite(addr, labelNum);
	//			}
	//		}
	//		else  //change standard label only
	//		{
	//			addr = addr_global + GLOBAL_LABEL_STANDARD;
	//			Flash_wwrite(addr, labelNum);	
	//		}
	//	}
	//
	//	sprintf(temp,"Label NO: #%d", labelNum);
	//	DisplayMsg(temp); 
	//#endif
}

extern void global_set_pluinhibit(INT8U x, INT8U y);
void keyapp_apply_global_label_barcode(void)
{
	//	CAPTION_STRUCT	cap;	
	//	INT8U	s_flag;
	char temp[64];
	INT8U g_label;
	
	//	s_flag = get_global_bparam(GLOBAL_SALE_SETUP_FLAG9)&0x80;
	
	//	if (s_flag)	//20090416 Added JJANG 라벨 1회 변경
	//	{		
	//		if (label.fmtNo != 0) LabelNumber = label.fmtNo;
	//		if (label.barcodefmtNo != 0) BarcodeNumber = label.barcodefmtNo;
	//	}
	//	else
	//	{
	g_label = get_global_bparam(GLOBAL_LABEL_PRIORITY);
	g_label ^= 1;
	set_global_bparam(GLOBAL_LABEL_PRIORITY, g_label);	// label priority
	set_global_bparam(GLOBAL_BARCODE_PRIORITY, g_label);	// barcode priority
	//	}
	
	global_set_pluinhibit(VERIFY_FUNC,0);
#ifdef USE_TRACE_STANDALONE
	if (g_label)
	{
		sprintf(temp, "공통라벨 사용");
	}
	else
	{
		sprintf(temp, "공통라벨 해제");
	}
	display_message_page_mid(temp);
#else
	if (g_label)
	{
		sprintf(temp, "Global Label");
	}
	else
	{
		sprintf(temp, "PLU Label");
	}
	DisplayMsg(temp); 
#endif
}

#ifdef USE_CHN_CART_SCALE
#ifdef USE_CHN_IO_BOARD
void keyapp_usb_save_log(void)
{
	INT16U len;
	INT8U ret;
	INT8U res_code;
	INT16U ind_idx;
	INT8U string_buf[50];
	
	POINT disp_p;
	char old_font;
	INT8U old_page;
	COMM_BUF *CBuf;
	INT32U pluno;
	INT8U  error;
	INT16U head, tail;
	INT16U log_num, temp_tail;
	
	CBuf = &CommBufUsbMem;
	
	old_font = DspStruct.Id1;
	old_page = DspStruct.Page;
	Dsp_SetPage(DSP_ERROR_PAGE);
	FontSizeChange(FONT_MEDIUM);
	display_lcd_clear_buf();  // Clear buf PLU Name Area
	
	disp_p = point(DSP_PLU_NAME_Y, DSP_PLU_NAME_X);
	sprintf((char*)string_buf, "USB SAVE LOG");
	
	Dsp_Write_String(disp_p,(HUGEDATA char*)string_buf);
	Dsp_Diffuse();
	//display_lcd_diffuse();	// Redraw PLU Name Area
	
	tail = get_nvram_wparam(NVRAM_SEND_REMAINDED_TAIL);
	head = get_nvram_wparam(NVRAM_SEND_REMAINDED_HEAD);
	
	//commun_send_cmd(CBuf, RFID_CMD_READ_DATA, NULL);
	log_num = 0;
	temp_tail = tail + log_num;
	temp_tail %= Startup.max_send_buf;
	usbmem_send_log_to_usb(CBuf, tail, log_num);
	
	network_timeout_reset(NETWORK_TIMEOUT_TIMESYNC);
	network_counter_reset(NETWORK_TIMEOUT_TIMESYNC);
	
	Delay_100ms(2);
	while(1)
	{
		if (network_timeout_check(50,NETWORK_TIMEOUT_TIMESYNC)) 
		{
			if (network_counter_check(0,NETWORK_TIMEOUT_TIMESYNC)) 
			{
				usbmem_send_log_to_usb(CBuf, tail, log_num);
			} 
			else 
			{
				BuzOn(3);
				sprintf((char*)string_buf, "FAIL..");
				display_message_page_mid((char*)string_buf);
				break;
			}
		}
		ret = usbmem_interpreter(CBuf);
		// 빠져나가는 조건
		if (ret == USBMEM_COM_OK)
		{
			if (usbmem_RcvCmd[0] == 'N' && usbmem_RcvCmd[1] == 'u')
			{
				memcpy((INT8U *)&pluno, usbmem_RcvBuffer, 4);
				memcpy((INT8U *)&error, &usbmem_RcvBuffer[4], 1);
				sprintf(string_buf, "[FAIL] LOG=%ld, ERR=%d", pluno, error);
				display_message_page_mid((char*)string_buf);
				break;
			}
			else if (usbmem_RcvCmd[0] == 'G' && usbmem_RcvCmd[1] == 'u')
			{
				memcpy((INT8U *)&pluno, usbmem_RcvBuffer, 4);
				memcpy((INT8U *)&error, &usbmem_RcvBuffer[4], 1);
				sprintf(string_buf, "[G%04d] PLU = %5ld", log_num, pluno);
				//display_message_page_mid((char*)string_buf);
				Dsp_Write_String(disp_p,(HUGEDATA char*)string_buf);
				Dsp_Diffuse();
				
				// next log
				log_num++;
				temp_tail = tail + (log_num - 1);	// log_num=0:file생성, log_num=1:가장오래된log
				temp_tail %= Startup.max_send_buf;
				if (temp_tail == head) break;	// complete
				
				usbmem_send_log_to_usb(CBuf, tail, log_num);
				network_timeout_reset(NETWORK_TIMEOUT_TIMESYNC);
				network_counter_reset(NETWORK_TIMEOUT_TIMESYNC);
			}
		}
		if(KEY_IsKey()) 
		{
			KEY_InKey();
			BuzOn(1);
			break;
		}
	}
	Dsp_SetPage(old_page);
	Dsp_Diffuse();
	DspLoadFont1(old_font);
}
#endif
#endif

#ifdef USE_TRACE_STANDALONE
//[start]gm 090408
void keyapp_specialuse(INT16U keyNum)
{
	INT16U  specialuseNum;	
	INT32U	addr;
	//	INT8U	buf1[52];
	//	INT8U	buf2[52];
	
	specialuseNum = keyNum - KS_SPECIALUSE_01 + 1;
	if(specialuseNum < 1 || specialuseNum > 100)
	{
		BuzOn(2);
		return;
	}
	BuzOn(1);
	
	//	memset(buf1, 0, sizeof(TRACE_SPECIALUSE_STRUCT));
	//	memset(buf2, 0, sizeof(TRACE_SPECIALUSE_STRUCT));
	
	addr =  DFLASH_BASE + FLASH_TRACE_SPECIALUSE_AREA;
	addr += sizeof(TRACE_SPECIALUSE_STRUCT) * (INT32U)(specialuseNum-1);
	
	if (Flash_wread(addr) != specialuseNum)
	{
		BuzOn(2);
		return;
	}
	//	addr += 2; //index no
	//
	//	Flash_sread(addr, (HUGEDATA INT8U *)buf1, sizeof(TRACE_SPECIALUSE_STRUCT)-2);
	//
	//	sprintf((char *)buf2, "용도:%s", buf1);
	//	display_message_page((char *)buf2);
	
	CurTraceStatus.meatUse = (INT8U)specialuseNum;
	sale_display_indivFlag_set(INDIV_DISP_MODE_SPECIALUSE, ON);
	sale_display_individual();
}
//[end]gm 090408
#endif

void keyapp_weight_reset(void)
{
	//	INT8U i;
	//	POINT p;
	//	char string_buf[25];
	//	INT16U x, y;
	
	status_run.run_mode = RUN_RESTART; 
	//for(i = 0; i < 2; i++)
	{
		display_allon_primary_indication();
		VFD7_Diffuse();
		LCD_Check(0xff,0x00);
		Delay_100ms(8);
		display_alloff_primary_indication();
		VFD7_Diffuse();
		LCD_Check(0x00,0xff);
		Delay_100ms(8);
	}
	display_alloff_primary_indication();
	VFD7_Diffuse();
	LCD_Check(0xff,0x00);
	Delay_100ms(10);
	//	DspLoadFont1(DSP_PLU_FONT_ID);
	//sprintf(string_buf, "저울 재시작");
	//	Dsp_Fill_Buffer(0);
	//	x = (LCD_X_MARGIN-display_font_get_height())/2;
	//	y = (LCD_Y_MARGIN-display_font_get_width()*strlen(string_buf))/2;
	//	p = point((INT8U)x, y);
	//	Dsp_Write_String(p, string_buf);
	//	Dsp_Diffuse();
}


void keyapp_check_production(void)
{
	display_alloff_primary_indication();
	VFD7_Diffuse();
	Dsp_ChangeMode(DSP_PGM_MODE);
	Report_SendingTransaction();
	Dsp_ChangeMode(DSP_SALE_MODE);
	sale_display_update(0x0fff);
}

void keyapp_delete_production(void)
{
	display_alloff_primary_indication();
	VFD7_Diffuse();
	Dsp_ChangeMode(DSP_PGM_MODE);
	Report_ClearSendingTransaction();
	Dsp_ChangeMode(DSP_SALE_MODE);
	sale_display_update(0x0fff);
}

#ifdef USE_KOR_TRACE
////////////////////////////////
// Korean Traceability
// Added by CJK 20060306

//INT8U use_kor_traceability = ON;

//SG070201
INT8U keyapp_ktr_print(void) //when press "PRINT" key
{
	INT8U   backflag, flagTemp;
	INT16U  wait;
	INT8U   retry_cnt;
	INT8U	ret =1;
	INT16U	id;

#ifdef USE_HANAPOS_PROTOCOL
	if(Operation.fixedPrice) id = KORTR_REQ_FPPRINT_ID;
	else id = KORTR_REQ_SCALE_SALE_ID;
#else
	if(Operation.fixedPrice) id = KORTR_REQ_FPPRINT_ID;
	else id = KORTR_REQ_PRINT_ID;
#endif
	if(network_status.service_bflag2)
	{
		if(status_scale.cur_pluid){

			if(PluType() == PLU_BY_WEIGHT && !ADM_GetData.Stable) return 0;

#ifdef USE_KOR_TRACE_RS232
			keyapp_ktr_req_print(id);
			
			KorTrace_Flag.flagCommSuccess = 0;
			backflag = flagTempAllowSale;
			flagTempAllowSale = ON;
			wait = SysTimer100ms;
			while(1){
				network_common_trans();
				if(KorTrace_Flag.flagCommSuccess){
					flagTemp=1;
					break;
				}
				if((INT16U)(SysTimer100ms-wait) > KOR_TRACE_TIMEOUT) break;
			}
			flagTempAllowSale = backflag;

#else
			retry_cnt=KOR_TRACE_RETRY_CNT;
			KorTrace_Flag.flagCommSuccess = 0;
			flagTemp =0;
			while(retry_cnt>0){

				if (Ethernet_Server(CLIENT_OPEN, 0, CLIENT_SCALE_POS)){

					keyapp_ktr_req_print(id);

					KorTrace_Flag.flagCommSuccess = 0;
					backflag = flagTempAllowSale;
					flagTempAllowSale = ON;
					wait = SysTimer100ms;
					while(1){
						network_common_trans();
						if(KorTrace_Flag.flagCommSuccess){
							flagTemp=1;

							//SG070201:
							//Get parameter value
							//if(Don't memorize)
							//{
							//	memset((char *)KorTrace_Flag.memberCode, 0, sizeof(KorTrace_Flag.memberCode));
							//}
							break;
						}
						if((INT16U)(SysTimer100ms-wait) > KOR_TRACE_TIMEOUT) break;
					}
					flagTempAllowSale = backflag;
				}
				Ethernet_Server(CLIENT_CLOSE, 0, CLIENT_SCALE_POS);
				//Delay_100ms(2);
				if(flagTemp) break;
				retry_cnt--;

			} // while(retry_cnt>0)
#endif
			if(!KorTrace_Flag.flagCommSuccess){
				BuzOn(4);
				KorTrace_CommErrorMsg();
			}
		}
		else
		{
		       BuzOn(3);
		}
	}
	else
	{
		keyapp_print(OFF);
	}

	return ret;

}

//SG060329
void keyapp_ktr_req_print(INT16U command)  //0x0201,0x020C
{
	if(!network_status.service_bflag2){
		BuzOn(2);
		return;
	}
	if(!status_scale.cur_pluid) {
		BuzOn(2);
		return;
	}
//	if (Ethernet_Server(CLIENT_OPEN, 0, CLIENT_NORMAL)) {  //SG060404
#ifdef USE_KOR_TRACE_RS232
		kortrace_com_req_command(&CommBuf, command);
#else
		kortrace_com_req_command(&CommBufEthData[0], command);
#endif
//		Ethernet_Server(CLIENT_CLOSE, 0, CLIENT_NORMAL);
//	}
}
/*
//SG060327
void keyapp_ktr_req_print(INT16U command)  //0x0201, 0x0207
{
	if(!network_status.service_bflag2){
		BuzOn(2);
		return;
	}
	if(!status_scale.cur_pluid) {
		BuzOn(2);
		return;
	}
	if (Ethernet_Server(CLIENT_OPEN, 0, CLIENT_NORMAL)) {
		//kortrace_com_print_id(&CommBufEthData[0]);
		kortrace_com_req_command(&CommBufEthData[0], command);
		Ethernet_Server(CLIENT_CLOSE, 0, CLIENT_NORMAL);
	}
}
*/

void keyapp_ktr_req_id(void)
{
//	if(!network_status.service_bflag2){
//		BuzOn(2);
//		return;
//	}

	if(!status_scale.cur_pluid) {
		BuzOn(2);
		return;
	}
//	if (Ethernet_Server(CLIENT_OPEN, 0, CLIENT_NORMAL))
//	{
#ifdef USE_KOR_TRACE_RS232
		kortrace_com_req_command(&CommBuf, KORTR_REQ_FIRST_ID);
#else
		kortrace_com_req_command(&CommBufEthData[0], KORTR_REQ_FIRST_ID);
#endif
//		Ethernet_Server(CLIENT_CLOSE, 0, CLIENT_NORMAL);
//	}
}


void keyapp_ktr_req_nextid(INT16U command)
{
	INT8U backflag,retry_cnt,flagTemp;
	INT16U wait;
//	if(!use_kor_traceability){
	if(!network_status.service_bflag2){
		BuzOn(2);
		return;
	}
	//if(!status_scale.cur_pluid || KorTrace_Flag.flagReceive != 1) {
	if(!status_scale.cur_pluid || KorTrace_Flag.flagReceive != 1) {
		BuzOn(2);
		return;
	}

	KorTrace_Flag.flagCommSuccess = 0;
	retry_cnt = KOR_TRACE_RETRY_CNT;
	flagTemp =0;
#ifdef USE_KOR_TRACE_RS232
	kortrace_com_req_command(&CommBuf, command);

	backflag = flagTempAllowSale;
	flagTempAllowSale = ON;
	wait = SysTimer100ms;
	while(1){
		network_common_trans();
		if(KorTrace_Flag.flagCommSuccess){
			flagTemp=1;
			break;
		}
		if((INT16U)(SysTimer100ms-wait) > KOR_TRACE_TIMEOUT) break;
	}
	flagTempAllowSale = backflag;	
#else
	while(retry_cnt>0){

		if (Ethernet_Server(CLIENT_OPEN, 0, CLIENT_SCALE_POS)){

			kortrace_com_req_command(&CommBufEthData[0], command);

			backflag = flagTempAllowSale;
			flagTempAllowSale = ON;
			wait = SysTimer100ms;
			while(1){
				network_common_trans();
				if(KorTrace_Flag.flagCommSuccess){
					flagTemp=1;
					break;
				}
				if((INT16U)(SysTimer100ms-wait) > KOR_TRACE_TIMEOUT) break;
			}
			flagTempAllowSale = backflag;
		}

		Ethernet_Server(CLIENT_CLOSE, 0, CLIENT_SCALE_POS);
		//Delay_100ms(2);

		if(flagTemp) break;
		retry_cnt--;

	} //while(retry_cnt>0)
#endif
	if(!KorTrace_Flag.flagCommSuccess){
		BuzOn(4);
		KorTrace_CommErrorMsg();  //SG060404
	}

}
void keyapp_ktr_req_noneid(void)
{
	KorTrace_Flag.flagReceive = 0;		// CJK070705
	//sale_display_kortrace(OFF);
	sale_display_update(UPDATE_TITLE);	// CJK070705

/*
	if(!network_status.service_bflag2){
		BuzOn(2);
		return;
	}
	if(!status_scale.cur_pluid || KorTrace_Flag.flagReceive != 1) {
		BuzOn(2);
		return;
	}
	if (Ethernet_Server(CLIENT_OPEN, 0, CLIENT_NORMAL))
	{
		kortrace_com_req_command(&CommBufEthData[0], KORTR_REQ_NONE_ID);
		Ethernet_Server(CLIENT_CLOSE, 0, CLIENT_NORMAL);
	}
*/
}

INT8U keyapp_ktr_req_returnid(void)	//Added by JJANG 071123
{
	INT8U backflag,retry_cnt,flagTemp;
	INT16U wait;
	if(!network_status.service_bflag2)
	{
		BuzOn(2);
		return 0;
	}
//	if(!status_scale.cur_pluid || KorTrace_Flag.flagReceive != 1) deleted by JJANG 20080108
//	{
//		BuzOn(2);
//		return 0;
//	}
	KorTrace_Flag.flagCommSuccess = 0;
	retry_cnt = KOR_TRACE_RETRY_CNT;
	flagTemp =0;
#ifdef USE_KOR_TRACE_RS232
	kortrace_com_req_command(&CommBuf, KORTR_REQ_RETURN_ID);
	backflag = flagTempAllowSale;
	flagTempAllowSale = ON;
	wait = SysTimer100ms;
	while(1)
	{
		network_common_trans();
		if(KorTrace_Flag.flagCommSuccess)break;
		if((INT16U)(SysTimer100ms-wait) > KOR_TRACE_TIMEOUT) break;
	}
	flagTempAllowSale = backflag;
#else
	while(retry_cnt>0)
	{
		if (Ethernet_Server(CLIENT_OPEN, 0, CLIENT_SCALE_POS))
		{
			kortrace_com_req_command(&CommBufEthData[0], KORTR_REQ_RETURN_ID);
			backflag = flagTempAllowSale;
			flagTempAllowSale = ON;
			wait = SysTimer100ms;
			while(1)
			{
				network_common_trans();
				if(KorTrace_Flag.flagCommSuccess)
				{
					flagTemp=1;
					break;
				}
				if((INT16U)(SysTimer100ms-wait) > KOR_TRACE_TIMEOUT) break;
			}
			flagTempAllowSale = backflag;
		}
		Ethernet_Server(CLIENT_CLOSE, 0, CLIENT_SCALE_POS);
		if(flagTemp) break;
		retry_cnt--;
	} //while(retry_cnt>0)
#endif
	if(!KorTrace_Flag.flagCommSuccess)
	{
		BuzOn(4);
		KorTrace_CommErrorMsg();  //SG060404
		return 0;
	}
	return 1;
}

/*
void keyapp_ktr_req_compid(void)
{
	INT8U backflag,retry_cnt;
	INT16U wait;

//	if(!use_kor_traceability){
	if(!network_status.service_bflag2){
		BuzOn(2);
		return;
	}
	if(!status_scale.cur_pluid || KorTrace_Flag.flagReceive != 1) {
		BuzOn(2);
		return;
	}

//	for(retry_cnt=KOR_TRACE_RETRY_CNT;retry_cnt>0;retry_cnt--){

		KorTrace_Flag.flagCommSuccess = 0;

		if (Ethernet_Server(CLIENT_OPEN, 0, CLIENT_NORMAL)){

			kortrace_com_req_command(&CommBufEthData[0], KORTR_REQ_COMPLETE_ID);

			KorTrace_Flag.flagCommSuccess = 0;
			backflag = flagTempAllowSale;
			flagTempAllowSale = ON;
			wait = SysTimer100ms;
			while(1){
				network_common_trans();
				if(KorTrace_Flag.flagCommSuccess){
					retry_cnt=0;
					break;
				}
				if(SysTimer100ms-wait > KOR_TRACE_TIMEOUT) break;
			}
			flagTempAllowSale = backflag;
		}
		Ethernet_Server(CLIENT_CLOSE, 0, CLIENT_NORMAL);
		//Delay_100ms(2);
//	} //for(tetry_cnt..)

	if(!KorTrace_Flag.flagCommSuccess){
		BuzOn(4);
		KorTrace_CommErrorMsg();  //SG060404
	}
}
*/


/*
INT8U keyapp_ktr_req_printid(void)
{
//	if(!use_kor_traceability){
	if(!network_status.service_bflag2){
		BuzOn(2);
		return 0;
	}
	if(!status_scale.cur_pluid) {
		BuzOn(2);
		return 0;
	}
	if (Ethernet_Server(CLIENT_OPEN, 0, CLIENT_NORMAL))
	{
		//kortrace_com_req_complete_id(&CommBufEthData[0]);
		kortrace_com_req_command(&CommBufEthData[0], KORTR_REQ_PRINT_ID);
		return 1;
		Ethernet_Server(CLIENT_CLOSE, 0, CLIENT_NORMAL);
	}
	return 2;
}
*/

void KorTrace_CommErrorMsg(void)
{
//SG060404
//DEL071206	INT8U cur_page;
//DEL071206	cur_page=DspStruct.Page;
	//VFD7_save_restore(VFD_BACKUP);
	display_save_restore(DSP_BACKUP);
	display_alloff_primary_indication();
	VFD7_Diffuse();
//DEL071206	Dsp_SetPage(DSP_COMM_PAGE);
//DEL071206	Dsp_Fill_Buffer(0);
//DEL071206	Dsp_Diffuse();
	Dsp_Clear();
	strcpy(gmsg,"----");
	display_string(DISPLAY_TARE     ,(INT8U*)gmsg);
	display_string(DISPLAY_PRICE,(INT8U*)gmsg);
	strcpy(gmsg,"Err");
	display_string(DISPLAY_UNITPRICE,(INT8U*)gmsg);
	strcpy(gmsg,"COMM");
	display_string(DISPLAY_WEIGHT   ,(INT8U*)gmsg);
	VFD7_Diffuse();

	Delay_100ms(5);

//DEL071206	Dsp_SetPage(cur_page);
	Dsp_Diffuse();
	if (status_run.run_mode!=RUN_SALE) {
		//VFD7_save_restore(VFD_RESTORE);
		display_save_restore(DSP_RESTORE);
	}
	sale_display_update(0x0fff);
}

//SG062425
void keyapp_ktr_manual_oper(void)
{
	//TRACEABILITY_STRUCT	traceability_str;
	INT8U set_val2[21];
	INT8U result,key_mode;
	CAPTION_STRUCT cap;
	INT32U	Flash_addr;
	//INT16U  str_size;
	char old_font;
	INT32U sell_by_date;
	INT16U len;

	key_mode=KEY_GetMode();
	KEY_SetMode(1);
	old_font=DspStruct.Id1;
	DspLoadFont1(DSP_MENU_FONT_ID);
	//display_title_page_change(0x126B, 1, 1);


	Flash_addr = DFLASH_BASE + FLASH_NUTRIFACT_AREA;	//국내에서 사용안하는 FLASH MEMORY 구역을 사용.
	//str_size = TRACEABILITY_STRUCT_SIZE;
	Flash_sread(Flash_addr, ( INT8U *)set_val2, sizeof(set_val2));
	//memset((char *)set_val2,0,sizeof(set_val2));
	//memcpy((char *)set_val2,traceability_str.contents,sizeof(set_val2));

	Menu_Init();

	caption_split_by_code(0x126B, &cap,0);
	Menu_HeadCreate(( char *)cap.form);

	caption_split_by_code(0x126B, &cap, 0);
	caption_adjust(&cap,NULL);
	Menu_InputCreate(1, ( char *)cap.form, MENU_TYPE_STR, cap.input_x, cap.input_dlength, cap.input_length,
						cap.input_max, cap.input_min,0,NULL, set_val2, NULL,NULL);

	result = Menu_InputDisplay();
	if(result == MENU_RET_SAVE) {
		//memset((char *)&traceability_str,0,sizeof(TRACEABILITY_STRUCT));
		//memcpy((char *)traceability_str.contents,set_val2,16);
		//traceability_str.code =1;
		Flash_swrite(Flash_addr, ( INT8U *)&set_val2, sizeof(set_val2));
//		memcpy((char *)KorTrace_Flag.bufferManualMode, (char *)traceability_str.contents,16);
//		KorTrace_Flag.bufferManualMode[17]=0;

		memset(&KorTrace_ReceiveData, 0, sizeof(KORTRACE_RECEIVE_DATA_TYPE));
//		if( isdigit(set_val2[0]) ){
		if (set_val2[0]){
			sprintf((char *)&KorTrace_ReceiveData.traceid_barcode[0],"%05d",0);
			sprintf((char *)&KorTrace_ReceiveData.traceid_barcode[5],"%s",set_val2);
			sprintf((char *)&KorTrace_ReceiveData.traceid_barcode[37],"-%04d",0);	//KORTR_PROCESS_NO_LEN
			sprintf((char *)KorTrace_ReceiveData.traceid_printdata[0],"%.20s",set_val2);
		}

		kortrace_com_parse_common((INT8U *)&KorTrace_ReceiveData,KORTR_REQ_PRINT_ID);
		plu_get_field(status_scale.Plu.address-1, PTYPE_SELLBY_DATE, (INT8U *)&sell_by_date, (INT16S *)&len,sizeof(sell_by_date));	// sell by date
		KorTrace_ReceiveData.sellbydate	= sell_by_date;
		//KorTrace_ReceiveData.cmd
		//KorTrace_ReceiveData.ret_cmd
		KorTrace_Flag.flagPrint=1;
		KorTrace_Flag.flagReceive =1;

/*
//		memcpy(param->Text, (INT8U *)KorTrace_Flag.barcode_data,sizeof(KorTrace_Flag.barcode_data));
		memset((char *)KorTrace_Flag.barcode_data,0,sizeof(KorTrace_Flag.barcode_data));
		memset((char *)KorTrace_Flag.display_data,0,sizeof(KorTrace_Flag.display_data));
		memcpy((char *)KorTrace_Flag.barcode_data, (char *)traceability_str.contents,16);
		memcpy((char *)KorTrace_Flag.display_data,(char *)traceability_str.contents,16);
*/
	}

	DspLoadFont1(old_font);
	Dsp_Fill_Buffer(0);						// LCD Buffer clear
	Dsp_Diffuse();
	KEY_SetMode(key_mode);
	sale_display_update(0x0fff);

}

void keyapp_ktr_member(void)
{
	long	result;
	char	old;
	char 	temp[20];
	char	message[50];
	char 	form[4];
	INT8U   old_key;
	INT8U   len;


	if(!status_scale.cur_pluid) 	return;

	old_key = KEY_GetMode();
	KEY_SetMode(1);

	sprintf(message,"MEMBER CODE:[               ]");
	PutNSmallString(0,0,message,35);

	old=DspStruct.Id1;
	DspLoadFont1(DSP_SMALL_FONT_ID);
	len = sizeof(KorTrace_Flag.memberCode);
	memcpy(temp, (char *)KorTrace_Flag.memberCode, len );
	temp[len] =0;
	strcpy(form,"9\0");
//	result = keyin_string(KI_BARCODE_FORM, KI_GRAPHIC,( INT8U *)form, ( INT8U*)temp,
//			15, 15, 0*8, 13*6, 0, 0, KI_NOPASS);
	result = keyin_string(KI_SNUMBER, KI_GRAPHIC, 0, ( INT8U*)temp,
			15, 15, 0*8, 13*6, 0, 0, KI_NOPASS);

//	result=keyin_string(KI_SNUMBER, KI_GRAPHIC,0,
//		( INT8U*)ret_string, length,dlength,
//		(start_line)*Startup.menu_ygap, sl*Startup.menu_xgap, 0, 0, flag);


	DspLoadFont1(old);

	if(result==KP_ENTER || result==KP_SAVE)
	{
		//Save Member Code and Display
		memcpy((char *)KorTrace_Flag.memberCode, temp, len);
	}
//	else
//	{
//	}

	display_lcd_clear();
	//sale_display_Member(ON);
	sale_display_update(0x0fff);

	KEY_SetMode(old_key);
}

#endif

#ifdef USE_LOTTEMART_ITEMCODE13
#define LOTTEMART_ITEMCODE_SEARCH_BASE_PLUNO		1L
void keyapp_lottemart_itemcode13_pluCall(void)	 //Added by wcm 20131025 (function key #177)
{
	CAPTION_STRUCT	cap;
	POINT		disp_p;

	INT16U	result;
	INT8U	len,i;
	char	ret_string[31];
	INT32U retPluNo;

	if(status_scale.cur_pluid) return;

	KEY_SetMode(1);
	Dsp_ChangeMode(DSP_PGM_MODE);

	display_lcd_clear_buf();

	DspLoadFont1(DSP_MENU_FONT_ID);
	//	DspLoadFont1(DSP_SMALL_FONT_ID);

	sprintf(cap.form,"판매코드");
	cap.input_length = 13;
	cap.input_dlength = 13;
	cap.input_itype = 'S';
	cap.input_max = 0;
	cap.input_min = 0;
	cap.inputtype = 0;
	cap.input_x = 8;

	caption_adjust(&cap,NULL);
	disp_p = point((Startup.menu_type)*Startup.menu_ygap,0);
	Dsp_Write_String(disp_p,(HUGEDATA char*)cap.form);
	Dsp_Diffuse();

	memset(ret_string, 0, sizeof(ret_string));	//초기화
	KI_IN:
	keyin_setanycheck(keyapp_callback_salemode);
	//숫자만 입력 가능
	result = keyin_string(KI_SNUMBER, KI_GRAPHIC,0, (HUGEDATA INT8U*)ret_string, cap.input_length, 
					  cap.input_dlength, (Startup.menu_type)*Startup.menu_ygap, cap.input_x*8, 1, 0, KI_NOPASS);
	keyin_clearcallback();

	switch(result) 
	{
		case KP_ESC:
			goto RET_ESC; 
		case KP_ENTER:
		case KP_SAVE:
			goto RET_END;
		default:
			BuzOn(2);
			goto KI_IN;
	}
	RET_END:
	len=strlen(ret_string); //Write Barcode

	retPluNo = lottemart_itemcode_plu_search(LOTTEMART_ITEMCODE_SEARCH_BASE_PLUNO, ret_string, len);
	//plu Call
	if(retPluNo)
	{
		keyapp_pluno(0,retPluNo,OFF); 
	//Added by JJANG 20070803 셀프모드에서 개수 상품 발행시 자동 발행
		if((Operation.operationMode == MODE_SELF) && (KeyDrv.CnvCode == KS_PLU)) SelfserviceAutoCheck();	
	}
	else
	{
		BuzOn(2);
	}

	RET_ESC:
	Dsp_ChangeMode(DSP_SALE_MODE);
	KEY_SetMode(0);
	sale_display_update(UPDATE_ALL|UPDATE_CLEARALL);
	return;
}

#define LOTTEMART_SEARCH_ITEMCODE_MAX	999999L
INT32U lottemart_itemcode_plu_search(INT32U pluBaseNo, INT8U *barString, INT8U len)
{
	INT32U cur_id, key, pluextbar_offset, addr;
	INT16U dept_no, pluextbar, xsize;
	INT8U extBarStr[21], stype;

	pluextbar_offset = plu_get_ptype_point(49,&stype,&xsize);

	dept_no = status_scale.departmentid;
	if ((dept_no < 1) && (dept_no > 99)) dept_no = 1;

	cur_id = pluBaseNo;

	do {
		key = plu_table_seq_search(dept_no, &cur_id, LOTTEMART_SEARCH_ITEMCODE_MAX);//cur_id에서 가장 근접한 PLU search, key는 Address, cur_id는 PLU_No

		if(key)
		{
			addr = plu_calc_addr_by_key(key-1);
			extBarStr[0] = 0;
			if(xsize)
			{
				Flash_sread(addr + pluextbar_offset,(HUGEDATA INT8U*)extBarStr, 20);
			}	
	//string compare
			if (strcmp(barString,extBarStr)==0) return cur_id;
		}
		else 
		{
			return 0L;
		}
		cur_id++;
	} while(1);
}
#endif	//#ifdef USE_LOTTEMART_ITEMCODE13
#ifdef USE_LOTTEMART_DEFAULT
void keyapp_lottemart_online(void)
{

	if(OnlineFlag)
	{
		OnlineFlag = OFF;
		DisplayMsg("ONLINE MODE OFF");
		Operation.salePluClearWait = ON;
	}
	else
	{
		OnlineFlag = ON;
		DisplayMsg("ONLINE MODE ON");
	}
}
#endif
#ifdef USE_SETPRODUCT_SALE
long cmpnt_Prdct_No = 3;
long current_sale_No = 0;
INT8U setproduct_PrtFlag = OFF;
INT8U setproduct_scan_flag = OFF;
void keyapp_setproduct_sale(void)
{
	POINT disp_p;
	INT8U buf[20];
	INT16U result;
	INT8U i=0;

	sale_pluclear(ON);	
	if(SetproductSaleMode)
	{
		SetproductSaleMode = OFF;
		cmpnt_Prdct_No = 3;
		current_sale_No = 0;
		setproduct_PrtFlag = OFF;
		sprintf((char *)buf, "세트상품 판매 종료");
		display_message_page_mid((char *)buf);
		display_lcd_clear_buf();
		Dsp_Diffuse();
		ClerkDeleteTransaction(status_scale.clerkid);
		DeleteAdditionalData(status_scale.clerkid);
		return;
	}
	else
	{
		nonweighing_mode = OFF; //세트 상품 판매 모드에서 미계량 라벨 사용 불가. 세트 상품 판매 시작시 자동 해제.
		SetproductSaleMode = ON;
		setproduct_sale_state = SETPRODUCT_SALE_START;
		sprintf((char *)buf, "세트상품 판매 시작");
		display_message_page_mid((char *)buf);
		display_lcd_clear_buf();
		Dsp_Diffuse();

		disp_p = point(DSP_PLU_NAME_Y, DSP_PLU_NAME_X);
		Dsp_SetPage(DSP_ERROR_PAGE);
		FontSizeChange(FONT_MEDIUM);

		while(1)
		{
			sprintf((char *)buf, "내품 수 : [%3d]",cmpnt_Prdct_No);
			Dsp_Write_String(disp_p,(char *)buf);

			result = keyin_ulong(KI_GRAPHIC, KI_FORM_NORMAL, 0,(long*)&cmpnt_Prdct_No, 2, DSP_PLU_NAME_Y,DSP_PLU_NAME_X+8*12, 0,KI_NOPASS);
			if(cmpnt_Prdct_No != 0 && cmpnt_Prdct_No < 100)	break;
			else
			{
				sprintf((char *)buf, "범위 1-99");
				display_message_page_mid((char *)buf);
			}
		}
		display_lcd_clear_buf();
		Dsp_Diffuse();
	}
}
void keyapp_setproduct_clear(void)
{
	INT8U buf[20];
	
	if(!SetproductSaleMode)
		return;
	keyapp_clear();
	current_sale_No = 0;
	setproduct_sale_state = SETPRODUCT_SALE_START;
	ClerkDeleteTransaction(status_scale.clerkid);
	DeleteAdditionalData(status_scale.clerkid);
	sprintf((char *)buf, "내품구성 취소");
	display_message_page_mid((char *)buf);
}
void setproduct_components_sale(void)
{
	char string_buf[50];
	INT8U delay_time = 5;
	
	if(!setproduct_PrtFlag && (setproduct_sale_state == SETPRODUCT_SALE_COMPONENTS))
	{
		current_sale_No++;
		//display_lcd_clear();
		sprintf(string_buf, "%2d중%2d개 판매", (INT16U)cmpnt_Prdct_No,(INT16U)current_sale_No);
		if(setproduct_scan_flag == ON) delay_time = 10;
		display_message_page_mid_delay(string_buf,delay_time);
		if(current_sale_No == cmpnt_Prdct_No)
		{
			current_sale_No = 0;
			setproduct_sale_state = SETPRODUCT_PRINT_SETLABEL;
			if(Operation.multiply)
			{
				Operation.multiply = OFF;
				status_scale.print_multiple = 1;
				PrtDrvStruct.PrtLoadFormFlag=ON;
				label_load(status_scale.cur_labelid);
				Operation.salePrintWait = OFF;
			}
		}
	}
}
INT8U nonweighing_mode = OFF;
void keyapp_nonweighing_mode(void)
{
	INT8U buf[50];
	
	if(SetproductSaleMode)
	{
		BuzOn(2);
		sprintf((char *)buf, "미계량 라벨 설정불가");
		display_message_page_mid((char *)buf);
		return;
	}

	if(nonweighing_mode)
	{
		sprintf((char *)buf, "미계량 라벨 해제");
		display_message_page_mid((char *)buf);
		nonweighing_mode = OFF;
	}
	else
	{
		sprintf((char *)buf, "미계량 라벨 설정");
		display_message_page_mid((char *)buf);
		nonweighing_mode = ON;
	}
}
#endif // USE_SETPRODUCT_SALE

#ifdef USE_HYUNDAI_SET_FUNCTION
void hyundai_set_clear(void)
{
	sale_pluclear(ON);
	ClearPluAux();
	memset(ReceiptData, 0x00, sizeof(ReceiptData));
	MeatSetSaleMode = OFF;
	Operation.saleStatus = SALE_NORMAL;
	MeatSetCompNum = 0;

	ClerkDeleteTransaction(status_scale.clerkid);
	DeleteAdditionalData(status_scale.clerkid);
}

void input_receipt_number(void)
{
	CAPTION_STRUCT 	cap;
	INT8U receiptNoStr[23];
	INT8U datalen = 0;
	INT8U scanflag = OFF;
	INT8U result,old_font,key_mode;
	INT8U string_buf[50];
	POINT disp_p;

	display_lcd_clear_buf();
	Dsp_Diffuse();
	
  	key_mode=KEY_GetMode();
	KEY_SetMode(1);
	old_font=DspStruct.Id1;
	DspLoadFont1(DSP_MENU_FONT_ID);

	memset(ReceiptData, 0x00, sizeof(ReceiptData));
	memset(receiptNoStr, 0, sizeof(receiptNoStr));
RET_RE:	
	sprintf(cap.form, "영수증번호");
	cap.input_length = 22;
	cap.input_dlength = 22;
	cap.input_itype = 'S';
	cap.input_max = 0;
	cap.input_min = 0;
	cap.inputtype = 0;
	cap.input_x = 8;
	caption_adjust(&cap,NULL);
	disp_p = point((Startup.menu_type)*Startup.menu_ygap,0);
	Dsp_Write_String(disp_p,(char *)cap.form);

KI_IN:
	KeyDrv.FirstKey = 1; // key clear
	FuncKeyInputFlag = OFF;
	result = keyin_string(KI_SNUMBER, KI_GRAPHIC,0, ( INT8U*)receiptNoStr, cap.input_length,
					  cap.input_dlength, (Startup.menu_type)*Startup.menu_ygap, cap.input_x*8, 1, 0, KI_NOPASS);
	FuncKeyInputFlag = ON;

	switch(result)
	{
		case KP_ESC:
		case KP_ENTER:
		case KP_SAVE:
			goto RET_CHECK;
		case KS_SCANPRINT:
			keyapp_scanprint();
			scanflag = ON;
			KeyDrv.FirstKey = 1; // key clear
			if(strlen((char *)ReceiptData) == 22) // 영수증 22자리 정상 스캔
			{ 
				BuzOn(1);
				goto RET_EXIT;
			}
			else // 영수증 스캔 오류 
				goto RET_CHECK;
		default:
			BuzOn(2);
			goto KI_IN;
	}
RET_CHECK:
	datalen = strlen((char *)receiptNoStr);

	if (datalen == 22) { // 22자리 맞게 입력한 경우
		memcpy(ReceiptData, receiptNoStr, sizeof(receiptNoStr));
	}
	else if (datalen == 0 && !scanflag) {  // 데이터 입력 없이 확인 버튼 누른 경우 (스캔도 안 한 상태. ReceiptData 는 빈값으로 설정)
		memset(ReceiptData, 0x00, sizeof(ReceiptData));
	}
	else { // 영수증 번호 잘 못 입력
		BuzOn(2);
		scanflag = OFF;
		sprintf((char *)string_buf, "영수증 번호 확인!");
		display_message_page_mid((char *)string_buf);
		display_lcd_clear_buf();
		Dsp_Diffuse();
		goto RET_RE;
	}

RET_EXIT:	

	display_lcd_clear_buf();
	Dsp_Diffuse();
	DspLoadFont1(old_font);
	KEY_SetMode(key_mode);
}

void keyapp_hyundai_meat_set_start(void)
{
	INT8U buf[20];
	INT8U old_page;
	char old_font;

	if (MeatSetSaleMode || Operation.meatStorageLabelFlag) {
		BuzOn(2);
		return;
	}

	old_font = DspStruct.Id1;
	old_page = DspStruct.Page;

	if (status_scale.cur_pluid) {
		sprintf((char *)buf, "세트판매시작");
		display_message_page_mid((char *)buf);
	}
	else {
		BuzOn(2);
		sprintf((char *)buf, "세트상품확인");
		display_message_page_mid((char *)buf);
		display_lcd_clear_buf();
		sale_pluclear(ON);
		return;
	}
	
	MeatSetSaleMode = ON;
	MeatSetSaleUseScanner = OFF;
	Operation.saleStatus = SALE_SET_START;
	MeatSetPluId = status_scale.Plu.PluCode;	
	input_receipt_number();
	sale_pluclear(ON);
	ClearPluAux();

	display_lcd_clear_buf();
	Dsp_Diffuse();
	Dsp_SetPage(old_page);
	DspLoadFont1(old_font);
}

void keyapp_hyundai_meat_set_input_price(void)
{
	CAPTION_STRUCT cap;
	INT32U total_price = 0;
	INT8U str[20];
	INT16U result, SetSrialNum;
	INT16U PrtResult = OFF;
	INT8U old_page, key_mode, old_status;
	char old_font;

	if(!MeatSetCompNum) {
		BuzOn(2);
		sprintf(str, "판매 개수 확인!");
		display_message_page_mid_delay(str,10);
		return;
	}
	
	old_status = Operation.saleStatus;
	old_font = DspStruct.Id1;
	old_page = DspStruct.Page;
	key_mode=KEY_GetMode();
	KEY_SetMode(1);
	Dsp_SetPage(DSP_ERROR_PAGE);
	FontSizeChange(FONT_MEDIUM);
	
	while(1)
	{
		Menu_Init();

		sprintf(cap.form, "세트가격");
		cap.input_length = 7;
		cap.input_dlength = 7;
		cap.input_itype = 'N';
		cap.input_max = 9999999L;
		cap.input_min = 0L;
		cap.inputtype = 0;
		cap.input_x = 7;

		caption_adjust(&cap,NULL);
		Menu_InputCreate(1, ( char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
					cap.input_max, cap.input_min, 0, &total_price, NULL, NULL, NULL);
		Menu_SetSymbol(1, 11);
		result = Menu_InputDisplay();
		if(result == MENU_RET_SAVE) 
		{
			if(total_price == 0) {
				sprintf((char *)str, "가격 확인 필요!");
				display_message_page_mid((char *)str);
			}
			else 
			{
				Operation.saleStatus = PRINT_SET_LABEL;
				MeatSetSaleUseScanner = OFF;
				MeatSetPrice = total_price;
				sale_pluclear(ON);	// 현재 호출된 PLU Clear
				ClearPluAux();		// status_scale 에 연결된 PLU 데이터 삭제 
				keyapp_pluno(0,MeatSetPluId,ON); // 세트 PLU 호출
				keyapp_fixed_price(OFF,0);		 // Fixed Price 판매 처리 
				status_scale.cur_unitprice = MeatSetPrice; // 세트 금액을 총액으로 인쇄
				ProcessAll();
				status_scale.Plu.sales_msg = TRANS_MEAT_SET;

				StartPrintFlag = ON;
				Report_GetCurTime(&SetSaleRTC, 0);
				PrtResult = LabelAndTransaction();
				StartPrintFlag = OFF;
				if(PrtResult == ON)
				{					
					MeatSetLabelTransFlag = ON;
					increase_setproduct_number();
					TransactionSendWithoutVoid(status_scale.clerkid);
					hyundai_set_clear();
					MeatSetPrice = 0;
					break;
				}
				else
				{
					BuzOn(2);
					sale_display_msg2();
					MeatSetPrice = 0;
					break;
				}
			}
		}
		else if(result == MENU_RET_ESC || result == MENU_RET_EXIT) {
			Operation.saleStatus = old_status;
			sale_pluclear(ON);
			ClearPluAux();
			display_lcd_clear();
			break;
		}
		else {
			break;
		}
	}

	Dsp_SetPage(old_page);
	DspLoadFont1(old_font);
	KEY_SetMode(key_mode);
	sale_display_update(0x0fff);
	//if (PrtResult == ON) hyundai_set_clear();
}

void keyapp_hyundai_meat_set_cancel(void)
{
	INT8U buf[20];
	INT8U old_page;
	char old_font;

	if (!MeatSetSaleMode) {
		BuzOn(2);
		return;
	}

	old_font = DspStruct.Id1;
	old_page = DspStruct.Page;
	sprintf((char *)buf, "세트상품 구성 취소");
	display_message_page_mid((char *)buf);
	display_lcd_clear_buf();
	Dsp_Diffuse();
	
	hyundai_set_clear();
	
	Dsp_SetPage(old_page);
	DspLoadFont1(old_font);
}
#endif

#ifdef USE_REGIST_DISPOSAL
void keyapp_hyundai_disposal(void)
{
	INT8U buf[20];

	if (status_scale.cur_pluid) {
		BuzOn(2);
		return;
	}

	if(Operation.RegistDisposalFlag)
	{
		Operation.RegistDisposalFlag = OFF;
		sale_display_disposal(Operation.RegistDisposalFlag);
	}
	else
	{
		Operation.RegistDisposalFlag = ON;
		sale_display_disposal(Operation.RegistDisposalFlag);
	}
}
#endif

//#ifdef USE_QR_ADDTOTAL
// 실제 사용하는 바코드 폼의 길이에 따라 Add Total Count를 계산 
// NVRAM 영역, Floating Clerk Data Sync 등 추후 다시 적용 방안 검토 필요 
//	INT16U QR_addedMemUsage[MAX_CLERK_NO] = {0, };
//	INT8U check_MAX_Transaction(INT16U clerk_cnt)
//	{
//		INT8U ret;
//		INT32U curCalledPLU = 0;
//		INT16U barId = 0;
//		INT8U bar_form[BARCODE_FORMAT_LEN+1] = {0, };
//		INT8U barcode_type = 0;
//		INT8U curBarLength = 0;
//		
//		INT16U pluAddr = 0;
//		INT16U curDeptId = 0;
//		INT8U curPLUType = 0;
//		INT16S sz = 0;
//	
//		INT16U maxMem_QrBuf = 0;
//	
//		INT8U tmpStr[100] = {0, };
//	
//		ret = FALSE;
//	
//	
//		// M1264(QR Code Config)에 설정된 값을 전체 QR 버퍼 사이즈에 반영 
//		maxMem_QrBuf = MAX_BAR_DATA_BUF_SIZE_QR;
//		maxMem_QrBuf -= strlen(QrCfg_OpeningCommand);
//		maxMem_QrBuf -= strlen(QrCfg_PosId);
//		maxMem_QrBuf -= strlen(QrCfg_ClosingCommand);
//		
//		// 현재 호출된 상품의 바코드 정보 추출( PLU No -> PLU Type, PLU Addr, -> Bar ID -> Barform의 길이 추출 )
//		curCalledPLU = status_scale.cur_pluid;
//		curDeptId = status_scale.departmentid;
//		plu_table_search(curDeptId, curCalledPLU, &pluAddr, 0);
//		plu_get_field(pluAddr-1, PTYPE_PLU_TYPE, (INT8U *)&curPLUType, (INT16S *)&sz, sizeof(curPLUType));
//		barId = prt_get_plu_barid(curCalledPLU, pluAddr, curPLUType, 1);
//		if (barId<1 || barId>99) barId=0;
//		get_barcodeformat(barId,(char *)bar_form, &barcode_type);
//		curBarLength = strlen(bar_form); // 1520에서 설정된 PLU 별 bar form의 길이 추출 
//		
//		// M1264(QR Code Config)에 설정된 값 반영 
//		curBarLength += strlen(QrCfg_EanPrefix);
//		curBarLength += strlen(QrCfg_EanSuffix);
//	
//		// 추출한 바코드 정보를 바탕으로 사용량 계산
//		QR_addedMemUsage[status_scale.clerkid] += curBarLength;
//	
//		if( QR_addedMemUsage[status_scale.clerkid] >= maxMem_QrBuf-2 )
//		{
//			QR_addedMemUsage[status_scale.clerkid] -= curBarLength;
//			ret = FALSE;
//		}
//		else
//			ret = TRUE;
//		
//		sprintf(tmpStr, "cur_PLU_ID: %d\r\ncur_CLERK_ID: %d\r\ncurBarLength: %d\r\naddedMemUsage: %d\r\nmaxMem: %d\r\n", curCalledPLU, status_scale.clerkid, curBarLength, QR_addedMemUsage[status_scale.clerkid], maxMem_QrBuf);
//		MsgOut(tmpStr);
//		
//		return ret;
//	}
//#endif


INT8U check_status_weight(INT8U check)
{
	INT8U ret;

	if(Operation.fixedPrice || status_scale.Plu.fixed_weight)	//Modified by JJANG	20070703  "PARAMETER 610"
	{
		ret = 0;
		if(!GlbOper.PrintFixedItemAlways && (status_scale.Weight > 0))	ret = 3;	
	}
	else
	{ 
		ret = 1;
		if(PluType() == PLU_BY_WEIGHT)		//무게 상품			    
		{
			if(status_scale.Weight >= status_scale.weight_min)
			{
				ret = 0;
			}
			else if(status_scale.Weight > 0)  // 2006.03.03 [bmlim
			{
				/*
					* Prepack, Auto 상태 && 상품을 올려 놓으면 프린트를 한 번 자동으로 수행하는데,
					* 이 때 상품을 제거하지 않은 상태에서 바로 Print를 누르면 Parameter 594의 값에
					* 상관없이 프린트가 수행되는 부분은 현재 처리하지 않았음
					* - 이유 : 현재 adm & stable 상태에서도 이 함수에 접근하고,
					*          Print 키를 눌러도 이 함수에 접근하는데, 이 둘을 구분하려면
					*          이 함수의 매개변수를 추가하여 구분자를 넘기는 등 
					*          기존 함수의 변경이 필요할 수도 있기 때문
					*/
				if (Operation.wPrepack && GlbOper.PrintUnderMinWeightOnPrepack) {
					ret = 0;
				} else if (Operation.wAuto && GlbOper.PrintUnderMinWeightOnAUTO) {
					ret = 0;
				} else {
					if (GlbOper.PrintUnderMinWeight) {
						ret = 0;
					}
				}
			}
			else	//	weight == 0
			{
				ret = 4;
				if(check) ret = 0;
				CheckZeroTotalPrice();
				if(Operation.fixedPrice || status_scale.Plu.fixed_weight) ret = 0;
#ifdef USE_EMART_NETWORK
				if (PluType() != PLU_BY_WEIGHT) ret = 0;	// 무게에서 개수로 자동 변경되었을 경우
#endif
			}
		}
		else					//개수 상품
		{
			ret = 3;
			if(status_scale.Weight > 0)
			{
				if(Operation.operationMode == MODE_SELF) ret = 0; //SG060920	
				if(GlbOper.byCountWeight) ret = 0;
			}
			else	ret = 0;
		}
	}

	return ret;
}

INT8U disp_error_weight_msg(INT8U status)
{
	INT8U tmp;

	switch(status)
	{
		case 1 :
			BuzOn(4);
			DisplayMsg(global_message[MSG_MINWEIGHT]);//"Check minimum weight"
			return OFF;
		case 2 :
			BuzOn(4);
			DisplayMsg(global_message[MSG_26_C804]);//"Check price"
			return OFF;
		case 3 :
			BuzOn(4);
			DisplayMsg(global_message[MSG_23_C803]);//"Remove weight on tray"
			return OFF;
		case 4 :
			BuzOn(4);
			tmp = get_global_bparam(GLOBAL_SALE_SETUP_FLAG6);
			tmp = (tmp>>3)&0x01;
			if(tmp)  //big size error message
			{
				display_message_page(global_message[MSG_CHECK_WEIGHT]);
			}
			else
			{
				DisplayMsg(global_message[MSG_CHECK_WEIGHT]);//"Check weight"
			}		       
			return OFF;
	}
	return ON;
}