#ifndef PRICE_DISPLAY_DEF
#define PRICE_DISPLAY	0
#define PRICE_PRINTER	1
#endif


#define	TARE_TYPE_WEIGHT	1
#define	TARE_TYPE_KEY		2
#define	TARE_TYPE_PLU		3
#define TARE_TYPE_PRESET	4	//one-time tare와 무관// CJK080322


#ifndef _LABEL_KEY_DEF
#define _LABEL_KEY_DEF
#pragma pack(2)
typedef struct 
{
	INT16U	fmtNo;
	INT16U  barcodefmtNo;
	INT8U	reserved[2];
} LABEL_KEY_STRUCT;
#pragma pack()
#endif

//Clerk key action in label mode, Parameter 708
#define CLERK_KEY_ADD_FUNCTION	0
#define CLERK_KEY_DISABLED		1
#define CLERK_KEY_RESERVED1		2
#define CLERK_KEY_RESERVED2		3

//Return data of checked individual info (in keyapp_print function)
#define INVALID_INDIV_NO 	0x04
#define INVALID_SLAUGHTERHOUSE 	0x02
#define INVALID_GRADE 		0x01

#ifdef USE_TRACE_NUM_CHECK_FUNCTION
  #define TRACE_KOR_BEEF  1
  #define TRACE_KOR_PORK  2
  #define TRACE_KOR_NULL  3
#endif

#ifdef USE_QR_ADDTOTAL
  #ifdef USE_INDIA_DEFAULT
	  #define MAX_QR_ADDCOUNT 30	// 인도 현지 요청에 의한 QR Add Total 최대 Transaction 수 확장
  #else
	  #define MAX_QR_ADDCOUNT 15 // 30 -> 15 CL5200N 30Bytes 기준 15개 transaction (buf 500)
  #endif // USE_INDIA_DEFAULT
// ADD COUNT 증가 시 MAX_BAR_DATA_BUF_SIZE_QR 수정 필요
// 실제 사용하는 바코드 폼의 길이에 따라 Add Total Count를 계산 
// NVRAM 영역, Floating Clerk Data Sync 등 추후 다시 적용 방안 검토 필요 
//extern INT8U check_MAX_Transaction(INT16U clerk_cnt);
#endif // USE_QR_ADDTOTAL
extern INT16S CheckPluPrint(INT8U check);

extern INT16S   keyapp_tare(INT16S tareType,INT32U tare,INT16S beepFlag);
extern void   operation_load(void);
extern INT8U  preset_tare(long preset,INT8U reset);
extern void   keyapp_tare_error(INT8U ret);
extern INT32U keyapp_pluno(INT8U tDept,INT32U pluid,INT8U beep);
extern INT32U keyapp_plukey(INT16U keyplu);
extern INT16U call_clerk(INT16U skeyclerk);
extern INT16U keyapp_clerk(INT16U keyclerk);
extern INT16U keyapp_customer(INT16U keycustomer);
extern void   keyapp_directtare(INT16U skeytareid);
extern void   keyapp_led(void);
extern void   keyapp_clear(void);
// extern void   input_date_check_year_format(INT16S a); // Call Back Function
extern void   keyapp_date(INT8U mode);
extern void keyapp_edit_date(void);
extern void keyapp_toggle_weightunit(void);
extern void   keyapp_datetime(void);
extern void   keyapp_for(void);
extern void   keyapp_save(void);
extern void   keyapp_auto(void);
extern void   keyapp_self(INT16S Flag);
extern void   keyapp_prepack(void);
extern void   keyapp_non_function(void);
extern void   keyapp_misc_byweight(INT16S beep);
extern void   keyapp_misc_bycount(void);
extern void   keyapp_misc_bypcs(INT16S beep);
extern void   keyapp_menu(void);
extern INT8U  key_selfmenu(void);
extern INT8U  keyapp_print(INT16S beep);
//extern void   keyapp_feed(void);
extern void   keyapp_feed(INT16S beepFlag);

extern void   keyapp_multi(void);
extern void   keyapp_zero(void);
extern void   keyapp_shift(void);
extern INT16S    keyapp_nzero(void);
extern void prepack_onoff(INT8U mode);	// mode=0 , Toggle, mode=1, ON, mode=2 : Off

extern void keyapp_in_weight(void);
#ifndef USE_PLU_MULTIPLE_PRICE	//Refefence PLU 기능 사용 안 함
extern void keyapp_reference(void);
#endif
extern void keyapp_label_ticket(void);
//extern void KEY_TypeNumeric(void);
//extern void KEY_TypeSpeedPlu(void);
// extern void KEY_TypeClerk(void);
extern void   key_numeric(void);
extern void   key_function(void);
extern void   keyapp_unitchange(INT16S id);
extern INT16U keyapp_currency(INT16S beep,INT16S displayMsg,long currPrice,long currUnitPrice,INT16U skeycurrency);
extern INT16U keyapp_department(INT16U skeydept);
extern INT16U keyapp_call_clerk(void);
extern void   keyapp_call_tare(INT16U tare_id);
extern void keyapp_call_department(INT16U dept_id,INT8U fg);
extern void keyapp_override(void);
extern void keyapp_add(INT16S beep);
extern void	keyapp_lastvoid(void);

extern void keyapp_callback_salemode(void);
extern void keyapp_void(void);
extern void keyapp_total(INT16S directPrint);
extern void keyapp_return(void);
extern void keyapp_pay(INT16S beep);
extern void keyapp_cash_open(INT16S beepFlag);

extern void keyapp_minus_disc(void);
extern void keyapp_fixed_price(INT16S beep,INT16S mode);
extern void keyapp_type(INT16S keyFlag);
// extern void keyapp_selldate(void);
extern void keyapp_edit_origin(void);	// keyapp_origin
extern void keyapp_edit_item(void);		// keyapp_pluitem
extern void keyapp_edit_sellbydate(void);
extern void keyapp_edit_delplu(void);
extern void keyapp_chess_print(void);
extern void keyapp_test_print(void);
extern void keyapp_edit_discount(void);
extern void keyapp_edit_speedkey(void);
extern void keyapp_edit_price(void);
extern void keyapp_save_price(void);
extern void keyapp_edit_packdate(void); //SG060309
extern void keyapp_edit_ingredient(void);
extern void keyapp_call_lmsg(void);
extern void keyapp_checkin(void);
extern void keyapp_external_print(void);
#if defined(USE_RFID_TAG_READER) || defined(USE_RFID_TAG_READER_RFID_522_B)
extern void keyapp_send_call_tag(void);
#endif
extern void keyapp_call_packed_date(void);
extern void keyapp_edit_date_flag(void);
extern void keyapp_edit_tax(void);
extern void keyapp_apply_global_tax(void);
extern void keyapp_edit_trace(void);
extern void keyapp_set_ticketnumber(void);
extern void keyapp_set_scan_clerk(void);
extern void kayapp_input_barcode(void);
extern void kayapp_input_customer_no(void);
extern void kayapp_customer_close(void);
extern void kayapp_reopen_ticket(void);
extern void kayapp_close_ticket(void);
extern void keyapp_copy_ticket(void);
extern void keyapp_batch(INT8U beep);
extern void keyapp_g_total(void);
extern void sale_pluclear(INT16S keyFlag);
extern void keyapp_chg_prtmode(void);			// Added by CJK
extern void keyapp_change_salemode(void);
extern void	ComputeCurrency(INT16S dipslayMsg,INT16S y,INT16S x,long price,long unitprice);
extern void LoadWeighingInfo(void);
extern void CheckGrandTotalStatus(void);
extern void CheckSubTotalStatus(void);
extern void SelfserviceAutoCheck(void);
extern void display_message_page_mid(char *str);
extern void display_message_page_mid_delay(char *str, INT8U delay);
#ifdef USE_LOTTEMART_ITEMCODE13
extern void keyapp_lottemart_itemcode13_pluCall(void);
extern INT32U lottemart_itemcode_plu_search(INT32U pluBaseNo, INT8U *barString, INT8U len);
#endif
#ifdef USE_LOTTEMART_DEFAULT
extern void keyapp_lottemart_online(void);
#endif
#ifdef USE_LOTTEMART_MEATSCANNER
extern void lottemart_meatscanner_initial(void);
#endif
extern void keyapp_apply_global_label_barcode(void);
#ifdef USE_CHN_IO_BOARD
extern void keyapp_usb_save_log(void);
#endif
#ifdef USE_SETPRODUCT_SALE
extern void keyapp_setproduct_sale(void);
extern void keyapp_setproduct_clear(void);
extern void setproduct_components_sale(void);
#endif

//extern void ClearDisp7(void);
//extern INT16S DisplayMsg(char *msg);
//extern INT16S DisplayMsgRight(char *msg);
//extern INT16S DisplayMsgShort(char *msg);
#ifdef USE_TRACE_STANDALONE
extern void keyapp_check_trace(void);
extern INT8U SaveTraceabilityNo;
#endif
extern INT16U LabelNumber;
extern INT16U BarcodeNumber;

#ifdef USE_KOR_TRACE
// Added by CJK 20060310
extern void keyapp_ktr_req_id(void);
extern void keyapp_ktr_req_nextid(INT16U command);
extern void keyapp_ktr_req_noneid(void);
extern void keyapp_ktr_req_compid(void);
extern void keyapp_ktr_req_print(INT16U command);
extern INT8U keyapp_ktr_req_returnid(void);
extern INT8U keyapp_ktr_req_printid(void);
extern void KorTrace_CommErrorMsg(void);
extern void keyapp_ktr_manual_oper(void);
extern void keyapp_ktr_member(void);
extern INT8U keyapp_ktr_print(void);
  #ifdef USE_NHMART_SAFE_MEAT
extern TRACE_INDIVIDUAL_IDENTITY indivInfo;	//저장되기 전 임시 개체 정보.
  #endif
//#define    	KOR_TRACE_RETRY_CNT	 	3	// Comm retry
#define    	KOR_TRACE_RETRY_CNT	 	1	//
//#define		KOR_TRACE_TIMEOUT		20  // 3 second (100ms x 30)
#define     KOR_TRACE_TIMEOUT		50  // 5 second (061113)
#endif
#ifdef USE_TRACE_NUM_CHECK_FUNCTION
  extern INT8U Trace_Num_Check(void);
#endif
extern void keyapp_edit_store(void);
extern void keyapp_change_prefix(void);
#ifdef USE_LOT_NUMBER_FUNCTION
extern void keyapp_edit_lot(void);
#endif
#ifdef USE_HYUNDAI_SET_FUNCTION
extern void keyapp_hyundai_meat_set_start(void);
extern void keyapp_hyundai_meat_set_input_price(void);
extern void keyapp_hyundai_meat_set_cancel(void);
#endif
#ifdef USE_REGIST_DISPOSAL
extern void keyapp_hyundai_disposal(void);
#endif
#ifdef USE_LOTTEMART_MEATSCANNER
#define BAR_FORMAT_SIZE	50
#else
#define BAR_FORMAT_SIZE	40
#endif

#ifdef USE_QR_ADDTOTAL
//extern INT8U AddSaleFlag;
extern INT8U QrAddCount;
extern INT8U QrAddTotalFlag;
#endif

#ifdef USE_TOPMART_DEFAULT //sewon 170116
extern INT16U InputQty;
extern INT8U InputQtyFlag;
extern INT8U PackdatePrintFlag;
#endif

#ifdef USE_EMART_PIG_TRACE
extern char ImportLotNumber[24];
extern INT8U ImportLotNumberflag;

#endif

#ifdef USE_LOTTEMART_DEFAULT
extern INT8U OnlineFlag;
#endif

#ifdef USE_SETPRODUCT_SALE
extern long cmpnt_Prdct_No;
extern long current_sale_No;
extern INT8U setproduct_PrtFlag;
extern INT8U SetproductSaleMode;
extern INT8U setproduct_scan_flag;
extern INT8U nonweighing_mode;
#endif

#ifdef USE_CHN_EMART_TRACE//USE_CHN_EMART_BIRDS_TRACE
extern INT8U Pluchangeflag; //20140912
#endif

extern INT8U check_status_weight(INT8U check);
extern INT8U disp_error_weight_msg(INT8U status);

#define CAPTION_MAX_SIZE 32
#define CAPTION_DISP_SIZE 32