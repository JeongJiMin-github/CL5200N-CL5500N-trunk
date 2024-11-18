/*****************************************************************************
|*			
|*  Copyright		:	(c) 2004 CAS
|*  Filename		:	sale_customer.c
|*  Version		:	0.1
|*  Modified		:	2004/02/11
|*  Modified		:	2004/09/01
|*  Description		:	
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

//CREDIT CJK070321/// CUSTOMER CREDIT PROCESS
//CREDIT CJK070321#ifndef TRANS_CREDIT_PAY_DEFINE
//CREDIT CJK070321#define TRANS_CREDIT_PAY_DEFINE
//CREDIT CJK070321struct TRANS_CREDIT_PAY
//CREDIT CJK070321{
//CREDIT CJK070321	unsigned char flag;		//memory 관리 flag
//CREDIT CJK070321	unsigned char delete;		//리스트별 지줄내역
//CREDIT CJK070321	unsigned char paid;		//외상 입금 flag
//CREDIT CJK070321	long	dateTime[2];		//long type
//CREDIT CJK070321	TRANSACTION_PAY	pay;		// each Payment
//CREDIT CJK070321	struct TRANS_CREDIT_PAY	*next;
//CREDIT CJK070321};
//CREDIT CJK070321typedef	struct TRANS_CREDIT_PAY	TRANS_CREDIT_PAY;
//CREDIT CJK070321#endif
//CREDIT CJK070321
//CREDIT CJK070321#define	NVRAM_CREDIT_CUSTOMER_HEAD	200000L	//4byte * 100명
//CREDIT CJK070321#define	NVRAM_CREDIT_CUSTOMER_DATA	200400L	//temp
//CREDIT CJK070321#define	CREDIT_MEMORY_MAX_SIZE		100000L	//bytes
//CREDIT CJK070321#define	CREDIT_MEMORY_RECORD_SIZE	100L	//bytes
//CREDIT CJK070321#define	CREDIT_MAX_TRANSACTION		(CREDIT_MEMORY_MAX_SIZE/CREDIT_MEMORY_RECORD_SIZE)
//CREDIT CJK070321
//CREDIT CJK070321#define MAX_CREDIT_NO    	100		// INPUT 1~99
//CREDIT CJK070321
//CREDIT CJK070321extern void	ClearPayment(INT8U trtType);
//CREDIT CJK070321extern void	ClerkGetName(INT16U clerkid ,char* p);
//CREDIT CJK070321extern long LocalPayTotalSummaries(TRANSACTION_PAY Pay,INT8U recvmode);
//CREDIT CJK070321//extern INT16S DisplayMsg(char *msg);
//CREDIT CJK070321extern INT16S PutSmallString(INT16S x, INT16S y,char *data);
//CREDIT CJK070321
//CREDIT CJK070321INT32U get_nvram_credit_addr(INT32U base,INT16U a)
//CREDIT CJK070321{
//CREDIT CJK070321	INT32U addr;
//CREDIT CJK070321	addr = a*CREDIT_MEMORY_RECORD_SIZE;
//CREDIT CJK070321	addr+= base+NVRAM_BASE;
//CREDIT CJK070321
//CREDIT CJK070321	return addr;
//CREDIT CJK070321}
//CREDIT CJK070321
//CREDIT CJK070321int	CreditSearchEmpty(void)
//CREDIT CJK070321{
//CREDIT CJK070321	INT16S i;
//CREDIT CJK070321	TRANS_CREDIT_PAY *pCredit;
//CREDIT CJK070321	for(i=0;i<CREDIT_MAX_TRANSACTION;i++)
//CREDIT CJK070321	{
//CREDIT CJK070321		pCredit = (TRANS_CREDIT_PAY *)get_nvram_credit_addr(NVRAM_CREDIT_CUSTOMER_DATA, i);
//CREDIT CJK070321		if(!pCredit->flag)
//CREDIT CJK070321			return i;//empty index
//CREDIT CJK070321	}
//CREDIT CJK070321	return -1;
//CREDIT CJK070321}
//CREDIT CJK070321
//CREDIT CJK070321int	CreditAddData(TRANS_CREDIT_PAY* pCredit,TRANSACTION_PAY transPay,INT16S creditFlag)
//CREDIT CJK070321{
//CREDIT CJK070321	INT16S firstFlag=ON;
//CREDIT CJK070321
//CREDIT CJK070321	pCredit->pay = transPay;// copy payment
//CREDIT CJK070321	pCredit->flag = ON;
//CREDIT CJK070321	pCredit->delete = OFF;
//CREDIT CJK070321	if(creditFlag)
//CREDIT CJK070321		pCredit->paid = OFF;
//CREDIT CJK070321	else
//CREDIT CJK070321		pCredit->paid = ON;
//CREDIT CJK070321
//CREDIT CJK070321	return ON;
//CREDIT CJK070321}
//CREDIT CJK070321
//CREDIT CJK070321int CreditAddPay(INT16S creditCustomerNo,TRANSACTION_PAY transPay,INT16S creditFlag)
//CREDIT CJK070321{
//CREDIT CJK070321	TRANS_CREDIT_PAY *pCredit;
//CREDIT CJK070321	INT16S	index;
//CREDIT CJK070321	INT32U  head_addr;
//CREDIT CJK070321
//CREDIT CJK070321  	if(MAX_CREDIT_NO-1 < creditCustomerNo)
//CREDIT CJK070321  	{ 
//CREDIT CJK070321		return 0;	// data error
//CREDIT CJK070321  	}                              
//CREDIT CJK070321	index = CreditSearchEmpty();
//CREDIT CJK070321	if(index == -1) return 0;
//CREDIT CJK070321
//CREDIT CJK070321	pCredit = (TRANS_CREDIT_PAY *)get_nvram_lparam(NVRAM_CREDIT_CUSTOMER_HEAD+creditCustomerNo*4);
//CREDIT CJK070321	if(!pCredit)
//CREDIT CJK070321	{
//CREDIT CJK070321		head_addr = get_nvram_credit_addr(NVRAM_CREDIT_CUSTOMER_DATA, index);
//CREDIT CJK070321		set_nvram_lparam(NVRAM_CREDIT_CUSTOMER_HEAD+creditCustomerNo*4, head_addr);
//CREDIT CJK070321		pCredit = (TRANS_CREDIT_PAY *)head_addr;
//CREDIT CJK070321	}
//CREDIT CJK070321	else
//CREDIT CJK070321	{
//CREDIT CJK070321		for(;;)
//CREDIT CJK070321		{
//CREDIT CJK070321			if (pCredit == pCredit->next) pCredit->next=0;
//CREDIT CJK070321			if (pCredit->next)
//CREDIT CJK070321			{
//CREDIT CJK070321				pCredit=pCredit->next;
//CREDIT CJK070321			}
//CREDIT CJK070321			else
//CREDIT CJK070321			{
//CREDIT CJK070321				pCredit->next = (TRANS_CREDIT_PAY *)get_nvram_credit_addr(NVRAM_CREDIT_CUSTOMER_DATA, index);
//CREDIT CJK070321				pCredit = pCredit->next;
//CREDIT CJK070321				break;
//CREDIT CJK070321			}
//CREDIT CJK070321		}
//CREDIT CJK070321	}
//CREDIT CJK070321	CreditAddData(pCredit,transPay,creditFlag);
//CREDIT CJK070321	return ON;
//CREDIT CJK070321}
//CREDIT CJK070321
//CREDIT CJK070321int CreditClearAllData(void) 
//CREDIT CJK070321{   
//CREDIT CJK070321	INT16S i;
//CREDIT CJK070321	TRANS_CREDIT_PAY *pCredit;
//CREDIT CJK070321
//CREDIT CJK070321	for(i=0; i<CREDIT_MAX_TRANSACTION; i++) 
//CREDIT CJK070321	{
//CREDIT CJK070321		  pCredit = (TRANS_CREDIT_PAY *)get_nvram_credit_addr(NVRAM_CREDIT_CUSTOMER_DATA, i);
//CREDIT CJK070321		  pCredit->flag = 0;
//CREDIT CJK070321		  pCredit->delete = 0;
//CREDIT CJK070321		  pCredit->next = NULL;
//CREDIT CJK070321	} // of for
//CREDIT CJK070321
//CREDIT CJK070321	for(i=0; i<CREDIT_MAX_TRANSACTION; i++) 
//CREDIT CJK070321	{
//CREDIT CJK070321		set_nvram_lparam(NVRAM_CREDIT_CUSTOMER_HEAD+i*4, NULL);
//CREDIT CJK070321	}
//CREDIT CJK070321    return 0;
//CREDIT CJK070321}
//CREDIT CJK070321
//CREDIT CJK070321int CreditDeleteCustomer(unsigned char creditCustomerNo)
//CREDIT CJK070321{ 
//CREDIT CJK070321	TRANS_CREDIT_PAY  *cp;
//CREDIT CJK070321	TRANS_CREDIT_PAY  *cpNext;
//CREDIT CJK070321	INT16S i=0;
//CREDIT CJK070321 
//CREDIT CJK070321  	if(MAX_CREDIT_NO-1 < creditCustomerNo)	return 0;
//CREDIT CJK070321
//CREDIT CJK070321	cp = (TRANS_CREDIT_PAY *)get_nvram_lparam(NVRAM_CREDIT_CUSTOMER_HEAD+creditCustomerNo*4);
//CREDIT CJK070321	if(!cp)	return -1;
//CREDIT CJK070321
//CREDIT CJK070321  	for(;cp;)
//CREDIT CJK070321	{
//CREDIT CJK070321		cpNext = cp->next;
//CREDIT CJK070321		cp->next = NULL;
//CREDIT CJK070321		cp->flag = OFF;
//CREDIT CJK070321		cp=cpNext;
//CREDIT CJK070321	}
//CREDIT CJK070321	set_nvram_lparam(NVRAM_CREDIT_CUSTOMER_HEAD+creditCustomerNo*4, 0 );
//CREDIT CJK070321
//CREDIT CJK070321  	return 1;
//CREDIT CJK070321} //
//CREDIT CJK070321
//CREDIT CJK070321int CreditDeleteForPrice(unsigned char creditCustomerNo,long payPrice, INT16S deleteFlag)
//CREDIT CJK070321{ 
//CREDIT CJK070321	TRANS_CREDIT_PAY  *cp;
//CREDIT CJK070321	TRANS_CREDIT_PAY  *cpNext;
//CREDIT CJK070321	long	creditPrice;
//CREDIT CJK070321	INT16S i=0;
//CREDIT CJK070321 
//CREDIT CJK070321  	if(MAX_CREDIT_NO-1 < creditCustomerNo)	return 0;
//CREDIT CJK070321
//CREDIT CJK070321	cp = (TRANS_CREDIT_PAY *)get_nvram_lparam(NVRAM_CREDIT_CUSTOMER_HEAD+creditCustomerNo*4);
//CREDIT CJK070321	if(!cp)	return -1;
//CREDIT CJK070321
//CREDIT CJK070321	creditPrice = 0;
//CREDIT CJK070321  	for(;cp && payPrice;)
//CREDIT CJK070321	{
//CREDIT CJK070321		if(cp->delete)
//CREDIT CJK070321		{
//CREDIT CJK070321			cp = cp->next;
//CREDIT CJK070321			continue;
//CREDIT CJK070321		}
//CREDIT CJK070321
//CREDIT CJK070321		if(cp->pay.paidprice[PAY_CREDIT]<=payPrice)
//CREDIT CJK070321		{
//CREDIT CJK070321			payPrice -= cp->pay.paidprice[PAY_CREDIT];
//CREDIT CJK070321		}
//CREDIT CJK070321		else
//CREDIT CJK070321		{
//CREDIT CJK070321			cp->pay.paidprice[PAY_CREDIT] -= payPrice;
//CREDIT CJK070321			payPrice = 0;
//CREDIT CJK070321		}
//CREDIT CJK070321		if(!payPrice)	break;
//CREDIT CJK070321
//CREDIT CJK070321		cpNext = cp->next;
//CREDIT CJK070321		cp->next = NULL;
//CREDIT CJK070321		if(deleteFlag)
//CREDIT CJK070321			cp->flag = OFF;		// delete memory
//CREDIT CJK070321		else
//CREDIT CJK070321			cp->delete = ON;	// only mark
//CREDIT CJK070321		cp=cpNext;
//CREDIT CJK070321	}
//CREDIT CJK070321	if(deleteFlag)
//CREDIT CJK070321	{
//CREDIT CJK070321		if(cp)	// change start point
//CREDIT CJK070321			set_nvram_lparam(NVRAM_CREDIT_CUSTOMER_HEAD+creditCustomerNo*4,(long)cp );
//CREDIT CJK070321		else	// clear
//CREDIT CJK070321			set_nvram_lparam(NVRAM_CREDIT_CUSTOMER_HEAD+creditCustomerNo*4, 0 );
//CREDIT CJK070321	}
//CREDIT CJK070321
//CREDIT CJK070321  	return 1;
//CREDIT CJK070321} //
//CREDIT CJK070321
//CREDIT CJK070321int CreditTotalPrice(unsigned char creditCustomer,INT16S *totalCount, long *totalPrice,INT16S *receiveCount, long *receivePrice)
//CREDIT CJK070321{ 
//CREDIT CJK070321	TRANS_CREDIT_PAY  *cp;
//CREDIT CJK070321	INT16S	i;
//CREDIT CJK070321
//CREDIT CJK070321	*totalCount =0;
//CREDIT CJK070321	*totalPrice=0;
//CREDIT CJK070321	*receiveCount =0;
//CREDIT CJK070321	*receivePrice=0;
//CREDIT CJK070321
//CREDIT CJK070321	cp = (TRANS_CREDIT_PAY *)get_nvram_lparam(NVRAM_CREDIT_CUSTOMER_HEAD+creditCustomer*4);
//CREDIT CJK070321  	for(;cp;)
//CREDIT CJK070321	{
//CREDIT CJK070321		if(!cp->paid && !cp->delete)	// 외상거래 금액
//CREDIT CJK070321		{
//CREDIT CJK070321			(*totalCount)++;
//CREDIT CJK070321			(*totalPrice) += cp->pay.paidprice[PAY_CREDIT];
//CREDIT CJK070321		}
//CREDIT CJK070321		if(cp->paid)			// 외상 받은 금액
//CREDIT CJK070321		{
//CREDIT CJK070321			(*receiveCount)++;
//CREDIT CJK070321			for(i=0;i<PAY_CREDIT;i++)
//CREDIT CJK070321				(*receivePrice) += cp->pay.paidprice[i];
//CREDIT CJK070321		}
//CREDIT CJK070321		cp = cp->next;
//CREDIT CJK070321	}
//CREDIT CJK070321  	return ON;
//CREDIT CJK070321} //
//CREDIT CJK070321
//CREDIT CJK070321#define	MAX_CHANGE		15
//CREDIT CJK070321extern	SALE_G_PRICE_TYPE	GlbPrice;
//CREDIT CJK070321extern	INT16S		ChgIndex;
//CREDIT CJK070321extern	long	ChgRemainedPrice[];
//CREDIT CJK070321extern	long	ChgPaidPrice[];
//CREDIT CJK070321extern TRANSACTION_PAY transPay;
//CREDIT CJK070321extern char gmsg[];
//CREDIT CJK070321extern DISC_SALE_TYPE		DiscSale;
//CREDIT CJK070321extern CAPTION_STRUCT cap;
//CREDIT CJK070321extern INT8U	DataSearchFlag;
//CREDIT CJK070321extern TARE_OPER_TYPE		TareOper;
//CREDIT CJK070321extern void plucall_payment(INT16S num);
//CREDIT CJK070321extern void DispSaleCustomer(INT16S num);
//CREDIT CJK070321
//CREDIT CJK070321long	CreditReceiveDsp(INT8U clerk,INT16S totalCount, long totalPrice,INT16S receiveCount, long receivePrice,INT16S printAll)
//CREDIT CJK070321{
//CREDIT CJK070321	char	ch_rprice[20],priceBuf[15];
//CREDIT CJK070321	long	rprice,rTotalPrice,PrevPrice;
//CREDIT CJK070321	INT16S	i,payType,items;
//CREDIT CJK070321	INT16S	decpoint;
//CREDIT CJK070321	long	tempLong,payPrice;
//CREDIT CJK070321	INT16U  result;
//CREDIT CJK070321	INT32U  addr_global;
//CREDIT CJK070321	INT16S	priceLength=8;
//CREDIT CJK070321	INT8U   ret,roundFlag=OFF,roundApplyFlag=OFF;
//CREDIT CJK070321	char	tempBuf[30];
//CREDIT CJK070321	long	originPrice;
//CREDIT CJK070321	INT16S     max_pay;
//CREDIT CJK070321	long    sTotalPrice;
//CREDIT CJK070321
//CREDIT CJK070321	rprice=0L;
//CREDIT CJK070321	rTotalPrice=0;
//CREDIT CJK070321	PrevPrice=0;
//CREDIT CJK070321	payType=0;
//CREDIT CJK070321	items=0;
//CREDIT CJK070321	payPrice=0;
//CREDIT CJK070321	
//CREDIT CJK070321	DiscSale.saleCreditCustomer = 0;
//CREDIT CJK070321	max_pay=MAX_PAID_TYPE;
//CREDIT CJK070321	if (!(status_scale.restrict&FUNC_CUSTOM)) max_pay=6;
//CREDIT CJK070321	strset(gmsg,0x20,40);
//CREDIT CJK070321	PutSmallString(2,0,gmsg);
//CREDIT CJK070321	caption_split_by_code(0xc010,&cap,1);//"Customer     [  ]"
//CREDIT CJK070321	PutSmallString(2,0,cap.form);
//CREDIT CJK070321	keyin_setcallback(DispSaleCustomer);
//CREDIT CJK070321	DspStruct.DirectDraw = 1;
//CREDIT CJK070321	tempLong = (long)DiscSale.saleCreditCustomer;
//CREDIT CJK070321	result = keyin_ulong(KI_GRAPHIC, KI_FORM_NORMAL, 0,(long*)&tempLong, 2, 
//CREDIT CJK070321	     					2*8,  14*6, 0,KI_NOPASS );
//CREDIT CJK070321	keyin_clearcallback();
//CREDIT CJK070321	DiscSale.saleCreditCustomer = (INT8U)tempLong;
//CREDIT CJK070321	if(!DataSearchFlag || !DiscSale.saleCreditCustomer)
//CREDIT CJK070321	{
//CREDIT CJK070321//		caption_split_by_code(0xc047,&cap,1);//"No data"
//CREDIT CJK070321		DisplayMsg(global_message[MSG_NOSALEDATA]);
//CREDIT CJK070321		return 0;
//CREDIT CJK070321	}
//CREDIT CJK070321
//CREDIT CJK070321//INT16S CreditTotalPrice(unsigned char creditCustomer,INT16S *totalCount, long *totalPrice,INT16S *receiveCount, long *receivePrice)
//CREDIT CJK070321	CreditTotalPrice(DiscSale.saleCreditCustomer,&totalCount,&totalPrice,&receiveCount,&receivePrice);
//CREDIT CJK070321	if(!totalCount && !totalPrice)
//CREDIT CJK070321	{
//CREDIT CJK070321//		caption_split_by_code(0xc047,&cap,1);//"No data"
//CREDIT CJK070321		DisplayMsg(global_message[MSG_NOSALEDATA]);
//CREDIT CJK070321		return 0;
//CREDIT CJK070321	}
//CREDIT CJK070321
//CREDIT CJK070321	addr_global = DFLASH_BASE;
//CREDIT CJK070321	addr_global += FLASH_GLOBAL_AREA;
//CREDIT CJK070321	decpoint=DecimalPointPrice();
//CREDIT CJK070321
//CREDIT CJK070321	for(i=0;i<MAX_PAID_TYPE;i++)
//CREDIT CJK070321		transPay.paidprice[i]=0;
//CREDIT CJK070321
//CREDIT CJK070321	ChgIndex=0;
//CREDIT CJK070321	originPrice = totalPrice;
//CREDIT CJK070321	ChgRemainedPrice[0]=originPrice;
//CREDIT CJK070321	transPay.change=0L;
//CREDIT CJK070321
//CREDIT CJK070321	gmsg[0]=0;
//CREDIT CJK070321	rprice=totalPrice;
//CREDIT CJK070321	payPrice = rprice;
//CREDIT CJK070321	ClearPayment(TRANS_TYPE_PAY_TOTAL_PAY);
//CREDIT CJK070321
//CREDIT CJK070321	while (1)
//CREDIT CJK070321	{
//CREDIT CJK070321		items=totalCount;
//CREDIT CJK070321		if(Operation.operationClerk != OPER_NORMAL)	// floating clerk
//CREDIT CJK070321		{
//CREDIT CJK070321			ClerkGetName(clerk ,tempBuf);
//CREDIT CJK070321			caption_split_by_code(0xc012,&cap,0);
//CREDIT CJK070321			sprintf(gmsg,cap.form, clerk,tempBuf);
//CREDIT CJK070321			PutSmallString(0,0,gmsg);
//CREDIT CJK070321		}
//CREDIT CJK070321		FloatToString(TO_STR,priceBuf,priceLength,DISPLAY_PRICE,&originPrice,0);
//CREDIT CJK070321		caption_split_by_code(0xc013,&cap,0);//"SUBTOTAL"
//CREDIT CJK070321		PutSmallString(1,0,cap.form);
//CREDIT CJK070321		GetPriceString(tempBuf,priceBuf,OFF,PRICE_DISPLAY); // currency와 함께
//CREDIT CJK070321		PutSmallString(1,13,tempBuf);
//CREDIT CJK070321
//CREDIT CJK070321		//DisplayPaidTotal(rTotalPrice);
//CREDIT CJK070321		caption_split_by_code(0xc014,&cap,1);//"Payment      [ ]"
//CREDIT CJK070321		PutSmallString(2,0,cap.form);
//CREDIT CJK070321
//CREDIT CJK070321		keyin_setcallback(plucall_payment);
//CREDIT CJK070321		DspStruct.DirectDraw = 1;
//CREDIT CJK070321		tempLong = (long)payType;
//CREDIT CJK070321		result = keyin_ulong(KI_GRAPHIC, KI_FORM_NORMAL, 0,(long*)&tempLong, 1, 
//CREDIT CJK070321		     					2*8,  14*6, 0,KI_NOPASS );
//CREDIT CJK070321		payType = (INT8U)tempLong;
//CREDIT CJK070321		keyin_clearcallback();
//CREDIT CJK070321		if(result==KP_ESC || PAY_CREDIT<=payType) {
//CREDIT CJK070321			ChgIndex=0;
//CREDIT CJK070321//DEL060829			//return 0;
//CREDIT CJK070321			break;
//CREDIT CJK070321		}
//CREDIT CJK070321
//CREDIT CJK070321		PrevPrice=rprice;
//CREDIT CJK070321		roundApplyFlag = OFF;
//CREDIT CJK070321			
//CREDIT CJK070321		if(!roundApplyFlag) {
//CREDIT CJK070321			ChgRemainedPrice[0] = originPrice;
//CREDIT CJK070321		}
//CREDIT CJK070321		rprice = ChgRemainedPrice[0] - rTotalPrice;
//CREDIT CJK070321		payPrice = rprice;
//CREDIT CJK070321		if(payType < max_pay)
//CREDIT CJK070321		{
//CREDIT CJK070321			caption_split_by_code(0xc000+payType,&cap,1);
//CREDIT CJK070321			PutSmallString(3,0,cap.form);
//CREDIT CJK070321			result = keyin_ulong(KI_GRAPHIC, KI_FORM_NORMAL, decpoint,(long*)&payPrice, 10, 
//CREDIT CJK070321			     					3*8,  14*6, 0,KI_NOPASS );
//CREDIT CJK070321		}
//CREDIT CJK070321		if(result==KP_ARROW_UP)	continue;
//CREDIT CJK070321		if(result==KP_PAGE_UP )	continue;
//CREDIT CJK070321		if(result==KP_PAGE_DN ) continue;
//CREDIT CJK070321		if(result==KP_ESC ) {
//CREDIT CJK070321			ChgIndex=0;
//CREDIT CJK070321//DEL060829			return 0;
//CREDIT CJK070321			break;
//CREDIT CJK070321		}
//CREDIT CJK070321		if(max_pay < payType)	continue;
//CREDIT CJK070321		if(payType==PAY_CARD)	// credit card
//CREDIT CJK070321		{
//CREDIT CJK070321			if(rprice<payPrice) {
//CREDIT CJK070321				payPrice=PrevPrice;
//CREDIT CJK070321				continue;
//CREDIT CJK070321			}
//CREDIT CJK070321		}
//CREDIT CJK070321
//CREDIT CJK070321		if((result==KP_ENTER || result==KP_SAVE) && !transPay.change)
//CREDIT CJK070321		{
//CREDIT CJK070321			if(rprice)
//CREDIT CJK070321			{
//CREDIT CJK070321				long	roundPrice = 0;
//CREDIT CJK070321
//CREDIT CJK070321				ChgPaidPrice[ChgIndex] = payPrice;
//CREDIT CJK070321				transPay.paidprice[payType]+=payPrice;
//CREDIT CJK070321				rTotalPrice = 0L;
//CREDIT CJK070321				for(i=0;i<=ChgIndex;i++)
//CREDIT CJK070321					rTotalPrice += ChgPaidPrice[i];
//CREDIT CJK070321
//CREDIT CJK070321				if(rprice <= payPrice)
//CREDIT CJK070321					ChgRemainedPrice[ChgIndex+1] = 0L;
//CREDIT CJK070321				else
//CREDIT CJK070321					ChgRemainedPrice[ChgIndex+1] = rprice - payPrice;
//CREDIT CJK070321
//CREDIT CJK070321				transPay.change = 0;
//CREDIT CJK070321				if(rprice < payPrice)
//CREDIT CJK070321					transPay.change = payPrice - rprice;
//CREDIT CJK070321
//CREDIT CJK070321				if(MAX_CHANGE<=ChgIndex)
//CREDIT CJK070321				{
//CREDIT CJK070321					//DisplayGmsg(1891);//
//CREDIT CJK070321				}
//CREDIT CJK070321				else
//CREDIT CJK070321					ChgIndex++;
//CREDIT CJK070321
//CREDIT CJK070321			}
//CREDIT CJK070321			if(transPay.change)
//CREDIT CJK070321			{
//CREDIT CJK070321				char	tempBuf[40];
//CREDIT CJK070321
//CREDIT CJK070321				strset(gmsg,0x20,40);
//CREDIT CJK070321				PutSmallString(2,0,gmsg);
//CREDIT CJK070321				PutSmallString(3,0,gmsg);
//CREDIT CJK070321
//CREDIT CJK070321				caption_split_by_code(0xc015,&cap,0);//"RECEIVE"
//CREDIT CJK070321				PutSmallString(2,0,cap.form);
//CREDIT CJK070321				FloatToString(TO_STR,ch_rprice,priceLength,DISPLAY_PRICE,&rTotalPrice,0);
//CREDIT CJK070321				GetPriceString(tempBuf,ch_rprice,OFF,PRICE_DISPLAY);
//CREDIT CJK070321				PutSmallString(2,13,tempBuf);
//CREDIT CJK070321
//CREDIT CJK070321				caption_split_by_code(0xc016,&cap,0);//strcpy(gmsg,"CHANGE");
//CREDIT CJK070321				PutSmallString(3,0,cap.form);
//CREDIT CJK070321				FloatToString(TO_STR,ch_rprice,priceLength,DISPLAY_PRICE,&transPay.change,0);
//CREDIT CJK070321				GetPriceString(tempBuf,ch_rprice,OFF,PRICE_DISPLAY);
//CREDIT CJK070321				PutSmallString(3,13,tempBuf);
//CREDIT CJK070321			}
//CREDIT CJK070321		}
//CREDIT CJK070321		if(rprice <= payPrice)
//CREDIT CJK070321		{
//CREDIT CJK070321			INT16U	lastClerk;
//CREDIT CJK070321			//SALES_SUMMARY_TYPE *tp;
//CREDIT CJK070321			//INT16S	j;
//CREDIT CJK070321			Cash_open();
//CREDIT CJK070321			lastClerk = clerk;//LastClerk();
//CREDIT CJK070321			transPay.scaleid=(INT8U)status_scale.scaleid;
//CREDIT CJK070321			transPay.clerkid=clerk;
//CREDIT CJK070321			transPay.trtType=TRANS_TYPE_PAY_TOTAL_PAY;
//CREDIT CJK070321			sTotalPrice=LocalPayTotalSummaries(transPay,0);
//CREDIT CJK070321			if(printAll)
//CREDIT CJK070321			{
//CREDIT CJK070321				INT16U	gapPrintTime;
//CREDIT CJK070321				gapPrintTime = SysTimer100ms;
//CREDIT CJK070321
//CREDIT CJK070321				ComputeCurrency(0,0,0,sTotalPrice,0);// display,y,x
//CREDIT CJK070321				memset((INT8U*)&TotalData, 0, sizeof(STRUCT_TOTAL_PRICE));
//CREDIT CJK070321				TotalData.VoidCount = 0;
//CREDIT CJK070321				TotalData.TotalCount = totalCount;
//CREDIT CJK070321				TotalData.VoidPrice = 0;
//CREDIT CJK070321				TotalData.TotalPrice = originPrice;
//CREDIT CJK070321				TotalData.TotalWeight = 0;
//CREDIT CJK070321				TotalData.TotalQuantity = 0;
//CREDIT CJK070321				TotalData.SummaryTotalPrice = sTotalPrice;
//CREDIT CJK070321				TotalData.PayPrice = rTotalPrice;
//CREDIT CJK070321				ret = Prt_PrintStart(PRT_TOTAL_NORMAL_MODE, PrtStruct.Mode, lastClerk, ON, 0, 0);
//CREDIT CJK070321				if(ret == OFF) 
//CREDIT CJK070321				{
//CREDIT CJK070321//DEL060829					return 0;
//CREDIT CJK070321					break;
//CREDIT CJK070321				}
//CREDIT CJK070321				Key_DelaySec(20,gapPrintTime);
//CREDIT CJK070321				TareOper.addupWeight = 0;
//CREDIT CJK070321
//CREDIT CJK070321			// Don't Erase! Need Check! It will be used later.(CJK)
//CREDIT CJK070321				//CreditAddPay(DiscSale.saleCreditCustomer,transPay,0);
//CREDIT CJK070321			/////////
//CREDIT CJK070321
//CREDIT CJK070321				//network_ClerkDeleteTransaction(lastClerk);
//CREDIT CJK070321				//ClerkDeleteTransaction(lastClerk);
//CREDIT CJK070321			}
//CREDIT CJK070321			//transaction_pay_send(&CommBufEth,1);
//CREDIT CJK070321
//CREDIT CJK070321			return 1L;
//CREDIT CJK070321		}
//CREDIT CJK070321	}
//CREDIT CJK070321	return 0L;
//CREDIT CJK070321}
