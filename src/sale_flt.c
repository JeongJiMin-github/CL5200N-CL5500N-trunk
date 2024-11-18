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
#include "plu_manager.h"
#include "caption_app.h"
#include "dsp_app.h"
#include "rtc_app.h"
#include "plu_auxcall.h"
#include "discount.h"
#include "menu.h"
#include "sale_report.h"
#include "cashdrawer.h"
#include "clerk_app.h"
#include "mode_sale.h"
#include "customer_app.h"
#include "ethernet_comm.h"
#include "network_command.h"
#include "traceability.h"
#ifdef USE_CHN_CART_SCALE
#include "traceability_chn.h"
#endif
#ifdef USE_INDIA_FUNCTION
#include "inventory.h"
#include "plu_device.h"
#endif

extern CAPTION_STRUCT cap;
extern PRICE_TYPE Price;
extern char gmsg[];
extern INT32U NVRAM_CLEARK_LASTSALE_PNT;
//extern PREPACK_STATUS_TYPE PrepackStatus;
extern INT8U GlbFlagPCnMasterConnection;
extern INT32U MAX_PLU_REPORT_NO;

extern INT8U flagcopy_ticket;
extern INT8U PrintTotal(INT8U clerkid, INT16S discountPer,long discountPrice);
extern INT16S PutSmallString(INT16S y, INT16S x,char *data);
extern void customer_read(INT16U code, CUSTOMER_STRUCT *custom);
INT8U selectpayment(INT8U preKey);						//modify by se-hyung for select payment 20070530
extern  INT16S	PutNSmallString(INT16S y, INT16S x,char *data, INT16S n);
extern long PriceRounds(long tprice, INT16S roundType, INT16S assignFlag, long *roundDifference);
extern void transaction_sale_store(TRANSACTION_RECORD_TYPE *pTrans, TRANSACTION_RECORD_SUB_TYPE *pTrans_sub, TR_ADDITION_DATA_TYPE* pTrans_add, UNITPRICE_CHANGE_TYPE *pChgPrice, INT8U mode, INT8U sendflag);
extern long PayChangeSummary(TRANSACTION_PAY trans_pay);
extern INT8U network_remotecounter(void);
extern  void PrintTotalLabelTicket(INT8U trtType,INT16U lastClerk,long resultPrice,long payPrice, long savedPrice);
extern INT16S NetClerkLock(INT8U operator);
extern INT8U NetClerkLockRelease(INT8U operator);
extern INT16S NetCheckConnection(void);

//#ifdef USE_TICKETCLOSE_BEFORE_ZREPORT 
//extern void CheckClerkTransaction_Print(void);
//#endif
INT16S	ReadSystemNewDate(void)
{
	INT8U day;

	day = get_nvram_bparam(NVRAM_OLDDAY);
	if(day != RTCStruct.date)
	{
		//OldDay = (INT8U)RTCStruct.date;
		set_nvram_bparam(NVRAM_OLDDAY, RTCStruct.date);
		return 1;
	}
	return 0;
}

INT16S	ReadSystemNewDateTicket(void)
{
	INT8U day;

	day = get_nvram_bparam(NVRAM_OLDDAYTICKET);
	if(day != RTCStruct.date)
	{
		//OldDayTicket = (INT8U)RTCStruct.date;
		set_nvram_bparam(NVRAM_OLDDAYTICKET, RTCStruct.date);
		return 1;
	}
	return 0;
}

INT16S	ReadSystemNewDateReopenTicket(void)
{
	INT8U day;

	day = get_nvram_bparam(NVRAM_LAST_REOPEN_DAY);
	if(day != RTCStruct.date)
	{
		//OldDayReopen = (INT8U)RTCStruct.date;
		set_nvram_bparam(NVRAM_LAST_REOPEN_DAY, RTCStruct.date);
		return 1;
	}
	return 0;
}

long	GetTransactionCounter(void)
{
	INT32S count;
	INT8U readDate;

	readDate = (get_global_bparam(GLOBAL_PRICESIGN_1000) >> 5) & 0x01;
	count = get_nvram_lparam(NVRAM_COUNTER_TRANSACTION);
	
	if((readDate & ReadSystemNewDate()) || count < 1L || count > 999999L)
	{
		count = 1L;
		set_nvram_lparam(NVRAM_COUNTER_TRANSACTION, count);
	}
	return count;
}

void	IncreaseTransactionCounter(void)
{
	INT32S count;

	//CounterTransaction++;
	count = get_nvram_lparam(NVRAM_COUNTER_TRANSACTION);
	count++;
	set_nvram_lparam(NVRAM_COUNTER_TRANSACTION, count);
}
	

long	GetTicketCounter(void)
{
	char str_form[32];
	INT16U i, form_cnt, reset;
	INT32U ticket_form;
	INT32S count;

	form_cnt = 0;
	ticket_form = 1;	
	count = get_nvram_lparam(NVRAM_COUNTER_TICKET);
	
	get_global_sparam(GLOBAL_TICKET_SERIAL_FORM,(INT8U *)str_form,32);
	
	for(i = 0; i <= 32; i++)
	{
		if(str_form[i] == 'A')
			form_cnt++;
	}

	for(i = 0; i < form_cnt; i++)
	{
		ticket_form *= 10;
	}

	reset = count / ticket_form;
	
	if(ReadSystemNewDateTicket() || (count < 1L) || reset != 0)
	{
		count = 1L;
		set_nvram_lparam(NVRAM_COUNTER_TICKET, count);
	}
	return count;
}

void	IncreaseTicketCounter(void)
{
	INT32S count;
	//CounterTicket++;
	count = get_nvram_lparam(NVRAM_COUNTER_TICKET);
	count++;
	set_nvram_lparam(NVRAM_COUNTER_TICKET, count);
}

INT16S ClearSummaryRecord(SALES_SUMMARY_TYPE* tp,INT16S counter)
{
	INT16S i;

	if ( (counter<0) || (counter>1) ) // if (counter==9)
	{
		for (i=0;i<MAX_REPORT_PERIOD;i++)
		{
			tp->sstTransactions[i]=0L;
			tp->sstVolume[i]=0L;
			tp->sstWeight[i]=0l;
			tp->sstQty[i]=0l;
		}
	}
	else
	{
		tp->sstTransactions[counter]=0L;
		tp->sstVolume[counter]=0L;
		tp->sstWeight[counter]=0L;
		tp->sstQty[counter]=0L;
	}
	return 0;
}	

INT16S ClearPluSummaryRecord(PLU_SALES_SUMMARY_TYPE* tp,INT16S counter)
{
	INT16S i;

	if( (counter<0) || (counter>1) ) // if (counter==9)
	{
		if (counter == 99)	// Clear All 일때
		{
			tp->pstPLU= 0;
			tp->pstDept= 0;
			tp->pstPrev=0; 
			tp->pstNext= 0;
		}
		for (i=0;i<MAX_REPORT_PERIOD;i++)
		{
			//tp->pstTransactions[i]=0L;
			tp->pstTransactions[i]=0;
			tp->pstVolume[i]=0L;
			tp->pstWeight[i]=0L;
			tp->pstQty[i]=0L;
			tp->pstPrepackWeight[i] = 0;
			tp->pstPrepackVolume[i] = 0;
#if !(ROM_COUNTRY == ROM_RUS)
#if !defined(USE_INDIA_FUNCTION)
			tp->pstCountOfLabelVolume[i] = 0;
			tp->pstCountOfLabel[i] = 0;
#endif
			tp->pstTax[i] = 0;
#endif
		}
	}
	else
	{
		//tp->pstTransactions[counter]=0L;
		tp->pstTransactions[counter]=0;
		tp->pstVolume[counter]=0L;
		tp->pstWeight[counter]=0L;
		tp->pstQty[counter]=0L;
		tp->pstPrepackWeight[counter] = 0;
		tp->pstPrepackVolume[counter] = 0;
#if !(ROM_COUNTRY == ROM_RUS)
#if !defined(USE_INDIA_FUNCTION)
		tp->pstCountOfLabelVolume[counter] = 0;
		tp->pstCountOfLabel[counter] = 0;
#endif
		tp->pstTax[counter] = 0;
#endif
	}
	return 0;
}	

void clear_nvram_summary(INT32U saddr, INT32U size)
{
	nvram_set(saddr,0x00,size);
}

INT16S ClearPluCounter(PLU_SALES_SUMMARY_TYPE *tp)
{
	tp->pstCountNumber = 0L;
	return 0;
}

//#ifdef USE_ZREPORT_COUNTER
//void ClearZreportCounter()
//{
//	CounterTicketGroupZ[0]	=1L;
//	CounterTicketGroupZ[1]	=1L;	
//}
//#endif
INT16S ClearSalesSummaries(INT16S counter)
{
	INT16S i;
	SALES_SUMMARY_TYPE* tp;
	SALES_SUMMARY_TYPE sst;
	PLU_SALES_SUMMARY_TYPE* tp1;
	PLU_SALES_SUMMARY_TYPE pst;
#if ((defined(USE_REPORT_TAX) && defined(USE_TAX_RATE)) || defined(USE_REPORT_PAY))
	INT16S j;
#endif
//	INT16U prev_Time;

	tp = &sst;
	tp1 = &pst;

//	prev_Time = SysTimer1_25ms;
	//ClearSummaryRecord(tp,counter);
#ifdef DISABLE_X2Z2_REPORT
	clear_nvram_summary(NVRAM_DEPT_SUMMARY,(NVRAM_SALE_SUMMARY_SIZE*MAX_DEPT_NO));
	clear_nvram_summary(NVRAM_SCALE_SUMMARY,(NVRAM_SALE_SUMMARY_SIZE*MAX_SCALE));
	clear_nvram_summary(NVRAM_GROUP_SUMMARY,(NVRAM_SALE_SUMMARY_SIZE*MAX_GROUP));
	clear_nvram_summary(NVRAM_CLERK_SUMMARY,(NVRAM_SALE_SUMMARY_SIZE*MAX_CLERK_NO));
	clear_nvram_summary(NVRAM_HOURLY_SUMMARY,(NVRAM_SALE_SUMMARY_SIZE*25));
			
	get_nvram_summary_addr(NVRAM_TOTAL_SUMMARY, 0, tp);
	ClearSummaryRecord(tp,counter);
	set_nvram_summary_addr(NVRAM_TOTAL_SUMMARY, 0, tp);
	if(!GlbOper.PluReportType)
	{
		for (i=0;i< MAX_PLU_REPORT_NO;i++)
		{
			get_nvram_plu_summary(NVRAM_PLU_SUMMARY, i, tp1);
			ClearPluSummaryRecord(tp1,counter);
			set_nvram_plu_summary(NVRAM_PLU_SUMMARY, i, tp1);
		}
	}
	if (counter==99)
	{
		INT16S	scale;
		INT32U	addr;
#if (defined(USE_REPORT_TAX) && defined(USE_TAX_RATE))
		
		clear_nvram_summary(NVRAM_TAX,(MAX_SCALE*MAX_TAX_NO*4)+(MAX_TAX_NO*4+4));
		clear_nvram_summary(NVRAM_TAX_GROSS,(MAX_SCALE*MAX_TAX_NO*4)+(MAX_TAX_NO*4+4));
#endif
		for(scale=0;scale<MAX_SCALE;scale++)
		{
	
			set_nvram_wparam(NVRAM_LAST_TRANS_COUNT + 2 * scale, 0xffff);
		}
		for(scale=0;scale<MAX_SCALE;scale++)
		{
			set_nvram_wparam(NVRAM_LAST_TRANS_PAY + 2 * scale, 0xffff);
		}
#ifdef USE_MAX_SCALE_NO_255
		for(scale=0;scale<256-MAX_SCALE;scale++)	
		{
			set_nvram_wparam(NVRAM_LAST_TRANS_SALE2 + 2 * scale, 0xffff);
			set_nvram_wparam(NVRAM_LAST_TRANS_PAY2 + 2 * scale, 0xffff);
		}
#endif        

		set_nvram_lparam(NVRAM_COUNTER_TRANSACTION, 1);
		set_nvram_lparam(NVRAM_COUNTER_TICKET, 1);
		set_nvram_lparam(NVRAM_COUNTER_GROUP_Z+4*0, 1L);
		set_nvram_lparam(NVRAM_COUNTER_GROUP_Z+4*1, 1L);
		set_nvram_bparam(NVRAM_OLDDAY, 0);
		set_nvram_bparam(NVRAM_OLDDAYTICKET, 0);
		set_nvram_bparam(NVRAM_LAST_REOPEN_DAY, 0);

		set_nvram_wparam(NVRAM_PLUSUM_HEADINDEX, 1);
		set_nvram_wparam(NVRAM_PLUSUM_TAILINDEX, 1);
		set_nvram_wparam(NVRAM_PLUSUM_LRU, 0);
		set_nvram_wparam(NVRAM_PLUSUM_MRU, 0);

	} 
	else 
	{
#if (defined(USE_REPORT_TAX) && defined(USE_TAX_RATE))
		
		clear_nvram_summary(NVRAM_TAX,(MAX_SCALE*MAX_TAX_NO*4)+(MAX_TAX_NO*4+counter*4));
		clear_nvram_summary(NVRAM_TAX_GROSS,(MAX_SCALE*MAX_TAX_NO*4)+(MAX_TAX_NO*4+counter*4));
#endif
	}
	
	clear_nvram_summary(NVRAM_CUSTOM_SCALE,(NVRAM_SALE_SUMMARY_SIZE*MAX_SCALE));
	clear_nvram_summary(NVRAM_CUSTOM_CLERK,(NVRAM_SALE_SUMMARY_SIZE*MAX_SCALE));
	clear_nvram_summary(NVRAM_CUSTOM_CREDIT,(NVRAM_SALE_SUMMARY_SIZE*MAX_CLERK_NO));
	clear_nvram_summary(NVRAM_CUSTOM_HOUR,(NVRAM_SALE_SUMMARY_SIZE*25));
	clear_nvram_summary(NVRAM_REPACK_SCALE,(NVRAM_SALE_SUMMARY_SIZE*MAX_SCALE));
	clear_nvram_summary(NVRAM_REPACK_CLERK,(NVRAM_SALE_SUMMARY_SIZE*MAX_CLERK_NO));
	clear_nvram_summary(NVRAM_OVERRIDE_SCALE,(NVRAM_SALE_SUMMARY_SIZE*MAX_SCALE));
	clear_nvram_summary(NVRAM_OVERRIDE_CLERK,(NVRAM_SALE_SUMMARY_SIZE*MAX_CLERK_NO));
	clear_nvram_summary(NVRAM_VOID_SCALE,(NVRAM_SALE_SUMMARY_SIZE*MAX_SCALE));
	clear_nvram_summary(NVRAM_VOID_CLERK,(NVRAM_SALE_SUMMARY_SIZE*MAX_CLERK_NO));
	clear_nvram_summary(NVRAM_RETURN_SCALE,(NVRAM_SALE_SUMMARY_SIZE*MAX_SCALE));
	clear_nvram_summary(NVRAM_RETURN_CLERK,(NVRAM_SALE_SUMMARY_SIZE*MAX_CLERK_NO));
	
#if !(ROM_COUNTRY == ROM_RUS)	// REPORT
	clear_nvram_summary(NVRAM_RETURN_DEPT,(NVRAM_SALE_SUMMARY_SIZE*MAX_DEPT_NO));
#endif

	clear_nvram_summary(NVRAM_DISCOUNT_SCALE,(NVRAM_SALE_SUMMARY_SIZE*MAX_SCALE));
	clear_nvram_summary(NVRAM_DISCOUNT_CLERK,(NVRAM_SALE_SUMMARY_SIZE*MAX_CLERK_NO));
#if !(ROM_COUNTRY == ROM_RUS)	// REPORT
	clear_nvram_summary(NVRAM_DISCOUNT_CREDIT,(NVRAM_SALE_SUMMARY_SIZE*100));
#endif

	clear_nvram_summary(NVRAM_PREPACK_SCALE,(NVRAM_SALE_SUMMARY_SIZE*MAX_SCALE));
	clear_nvram_summary(NVRAM_PREPACK_CLERK,(NVRAM_SALE_SUMMARY_SIZE*MAX_CLERK_NO));
	clear_nvram_summary(NVRAM_PREPACK_HOUR,(NVRAM_SALE_SUMMARY_SIZE*25));
	clear_nvram_summary(NVRAM_PREPACK_DEPT,(NVRAM_SALE_SUMMARY_SIZE*MAX_DEPT_NO));
	clear_nvram_summary(NVRAM_PREPACK_GROUP,(NVRAM_SALE_SUMMARY_SIZE*100));

	for(i=0;i<MAX_SCALE;i++) 
	{
		if(1<counter)
		{
			set_nvram_wparam(NVRAM_CASH_SCALE+i*4, 0);
			set_nvram_wparam(NVRAM_CASH_SCALE+i*4+2, 0);
		}
		else
			set_nvram_wparam(NVRAM_CASH_SCALE+i*4+counter*2, 0);
	}
	for(i=0;i<MAX_CLERK_NO;i++) 
	{
		if(1<counter)
		{
			set_nvram_wparam(NVRAM_CASH_CLERK+i*4, 0);
			set_nvram_wparam(NVRAM_CASH_CLERK+i*4+2, 0);
		}
		else
			set_nvram_wparam(NVRAM_CASH_CLERK+i*4+counter*2, 0);

	}
#ifdef USE_REPORT_PAY	//#if !(ROM_COUNTRY == ROM_RUS)	// REPORT

	clear_nvram_summary(NVRAM_PAY_SCALE,(NVRAM_SALE_SUMMARY_SIZE*MAX_PAID_TYPE*MAX_SCALE));
	clear_nvram_summary(NVRAM_PAY_CLERK,(NVRAM_SALE_SUMMARY_SIZE*MAX_PAID_TYPE*MAX_SCALE));

	clear_nvram_summary(NVRAM_CHANGE_SCALE,(NVRAM_SALE_SUMMARY_SIZE*MAX_SCALE));
	clear_nvram_summary(NVRAM_CHANGE_CLERK,(NVRAM_SALE_SUMMARY_SIZE*MAX_CLERK_NO));
#endif
	clear_nvram_summary(NVRAM_ROUND_TOTAL_SCALE,(NVRAM_SALE_SUMMARY_SIZE*MAX_SCALE));
	clear_nvram_summary(NVRAM_ROUND_TOTAL_CLERK,(NVRAM_SALE_SUMMARY_SIZE*MAX_CLERK_NO));
	clear_nvram_summary(NVRAM_NEGATIVE_CLERK,(NVRAM_SALE_SUMMARY_SIZE*MAX_CLERK_NO));

	if (Startup.use_ext_nvram == 2 && counter == 99)	
	{
		ClearNVExtPLUSummaries();
	}
#else //USE To X2,Z2 Report
	for (i=0;i< MAX_SCALE;i++)
	{
		//tp = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_SCALE_SUMMARY, i);
		get_nvram_summary_addr(NVRAM_SCALE_SUMMARY, i, tp);
		ClearSummaryRecord(tp,counter);
		set_nvram_summary_addr(NVRAM_SCALE_SUMMARY, i, tp);
	}
	for (i=0;i< MAX_DEPT_NO;i++)
	{
		//tp = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_DEPT_SUMMARY, i);
		get_nvram_summary_addr(NVRAM_DEPT_SUMMARY, i, tp);
		ClearSummaryRecord(tp,counter);
		set_nvram_summary_addr(NVRAM_DEPT_SUMMARY, i, tp);
	}
	for (i=0;i< MAX_GROUP;i++)
	{
		//tp = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_GROUP_SUMMARY, i);
		get_nvram_summary_addr(NVRAM_GROUP_SUMMARY, i, tp);
		ClearSummaryRecord(tp,counter);
		set_nvram_summary_addr(NVRAM_GROUP_SUMMARY, i, tp);
	}
	for (i=0;i< MAX_CLERK_NO;i++)
	{
		//tp = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_CLERK_SUMMARY, i);
		get_nvram_summary_addr(NVRAM_CLERK_SUMMARY, i, tp);
		ClearSummaryRecord(tp,counter);
		set_nvram_summary_addr(NVRAM_CLERK_SUMMARY, i, tp);
	}
	for (i=0;i< 24+1;i++)
	{
		//tp = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_HOURLY_SUMMARY, i);
		get_nvram_summary_addr(NVRAM_HOURLY_SUMMARY, i, tp);
		ClearSummaryRecord(tp,counter);
		set_nvram_summary_addr(NVRAM_HOURLY_SUMMARY, i, tp);
	}
	//tp = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_TOTAL_SUMMARY, 0);
	get_nvram_summary_addr(NVRAM_TOTAL_SUMMARY, 0, tp);
	ClearSummaryRecord(tp,counter);
	set_nvram_summary_addr(NVRAM_TOTAL_SUMMARY, 0, tp);
	if(!GlbOper.PluReportType)
	{
		ClearPluSummaryRecord(tp1,counter);
		for (i=0;i< MAX_PLU_REPORT_NO;i++)
		{
			//tp1 = (PLU_SALES_SUMMARY_TYPE *)get_nvram_plu_summary(NVRAM_PLU_SUMMARY, i);
			get_nvram_plu_summary(NVRAM_PLU_SUMMARY, i, tp1);
			ClearPluSummaryRecord(tp1,counter);
			set_nvram_plu_summary(NVRAM_PLU_SUMMARY, i, tp1);
		}
	}
	if (counter==99)
	{
		INT16S	scale;
		INT32U	addr;
#if (defined(USE_REPORT_TAX) && defined(USE_TAX_RATE))
		// CL5200J을 위해 NVRAM_TAX와 NVRAM_TAX_GROSS의 for문을 분리함(CJK)
		//  - flash의 cache 스위칭을 최소화 시키기 위함
		for(scale=0;scale<MAX_SCALE;scale++)	
		{
			for (i=0;i<MAX_TAX_NO;i++)	// id=10(room no. 9)은 Tax free. 초기화시 포함되야함
			{
	   			for (j=0;j<MAX_REPORT_PERIOD;j++) {
					addr = (scale*MAX_TAX_NO*MAX_REPORT_PERIOD*4)+(i*MAX_REPORT_PERIOD*4+j*4);
					set_nvram_lparam(addr+NVRAM_TAX, 0);
				}
			}
		}
		for(scale=0;scale<MAX_SCALE;scale++)	
		{
			for (i=0;i<MAX_TAX_NO;i++)	// id=10(room no. 9)은 Tax free. 초기화시 포함되야함
			{
	   			for (j=0;j<MAX_REPORT_PERIOD;j++) {
					addr = (scale*MAX_TAX_NO*MAX_REPORT_PERIOD*4)+(i*MAX_REPORT_PERIOD*4+j*4);
					set_nvram_lparam(addr+NVRAM_TAX_GROSS, 0);
				}
			}
		}
#endif
		for(scale=0;scale<MAX_SCALE;scale++)
		{
			//PrevTransactionSaleTail[scale] = 0xffff;
			set_nvram_wparam(NVRAM_LAST_TRANS_COUNT + 2 * scale, 0xffff);
		}
		for(scale=0;scale<MAX_SCALE;scale++)
		{
			//PrevTransactionPayTail[scale] = 0xffff;
			set_nvram_wparam(NVRAM_LAST_TRANS_PAY + 2 * scale, 0xffff);
		}
#ifdef USE_MAX_SCALE_NO_255
		for(scale=0;scale<256-MAX_SCALE;scale++)	
		{
			//PrevTransactionSaleTail2[scale] = 0xffff;
			set_nvram_wparam(NVRAM_LAST_TRANS_SALE2 + 2 * scale, 0xffff);
			//PrevTransactionPayTail2[scale] = 0xffff;
			set_nvram_wparam(NVRAM_LAST_TRANS_PAY2 + 2 * scale, 0xffff);
		}
#endif        
		//CounterTransaction	=1L;
		set_nvram_lparam(NVRAM_COUNTER_TRANSACTION, 1);
		//CounterTicket		=1L;
		set_nvram_lparam(NVRAM_COUNTER_TICKET, 1);
//#ifndef USE_ZREPORT_COUNTER
		//CounterTicketGroupZ[0]	=1L;
		//CounterTicketGroupZ[1] 	=1L;
		set_nvram_lparam(NVRAM_COUNTER_GROUP_Z+4*0, 1L);
		set_nvram_lparam(NVRAM_COUNTER_GROUP_Z+4*1, 1L);
//#endif
		//OldDay		= 0;
		set_nvram_bparam(NVRAM_OLDDAY, 0);
		//OldDayTicket	= 0;
		set_nvram_bparam(NVRAM_OLDDAYTICKET, 0);
		//OldDayReopen    = 0;
		set_nvram_bparam(NVRAM_LAST_REOPEN_DAY, 0);

		set_nvram_wparam(NVRAM_PLUSUM_HEADINDEX, 1);
		set_nvram_wparam(NVRAM_PLUSUM_TAILINDEX, 1);
		set_nvram_wparam(NVRAM_PLUSUM_LRU, 0);
		set_nvram_wparam(NVRAM_PLUSUM_MRU, 0);

	} 
	else 
	{
#if (defined(USE_REPORT_TAX) && defined(USE_TAX_RATE))
		INT16S	scale;
		INT32U	addr;
		for(scale=0;scale<MAX_SCALE;scale++)
		{
	 		for (j=0;j<MAX_TAX_NO;j++)	// id=10(room no. 9)은 Tax free. 초기화시 포함되야함
			{
				addr = (scale*MAX_TAX_NO*MAX_REPORT_PERIOD*4)+(j*MAX_REPORT_PERIOD*4+counter*4);
				set_nvram_lparam(addr+NVRAM_TAX, 0);	
			}
		}
		for(scale=0;scale<MAX_SCALE;scale++)
		{
	 		for (j=0;j<MAX_TAX_NO;j++)	// id=10(room no. 9)은 Tax free. 초기화시 포함되야함
			{
				addr = (scale*MAX_TAX_NO*MAX_REPORT_PERIOD*4)+(j*MAX_REPORT_PERIOD*4+counter*4);
				set_nvram_lparam(addr+NVRAM_TAX_GROSS, 0);
			}
		}
#endif
	}
	for(i=0;i<MAX_SCALE;i++) 
	{
		//tp = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_CUSTOM_SCALE, i);
		get_nvram_summary_addr(NVRAM_CUSTOM_SCALE, i, tp);
		ClearSummaryRecord(tp,counter);
		set_nvram_summary_addr(NVRAM_CUSTOM_SCALE, i, tp);
	}
	for(i=0;i<MAX_CLERK_NO;i++) 
	{
		//tp = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_CUSTOM_CLERK, i);
		get_nvram_summary_addr(NVRAM_CUSTOM_CLERK, i, tp);
		ClearSummaryRecord(tp,counter);
		set_nvram_summary_addr(NVRAM_CUSTOM_CLERK, i, tp);
	}
	for(i=0;i<MAX_CLERK_NO;i++) 
	{
		//tp = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_CUSTOM_CREDIT, i);
		get_nvram_summary_addr(NVRAM_CUSTOM_CREDIT, i, tp);
		ClearSummaryRecord(tp,counter);
		set_nvram_summary_addr(NVRAM_CUSTOM_CREDIT, i, tp);
	}
	for(i=0;i<24+1;i++) 
	{
		//tp = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_CUSTOM_HOUR, i);
		get_nvram_summary_addr(NVRAM_CUSTOM_HOUR, i, tp);
		ClearSummaryRecord(tp,counter);
		set_nvram_summary_addr(NVRAM_CUSTOM_HOUR, i, tp);
	}

	for(i=0;i<MAX_SCALE;i++) 
	{
		//tp = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_REPACK_SCALE, i);
		get_nvram_summary_addr(NVRAM_REPACK_SCALE, i, tp);
		ClearSummaryRecord(tp,counter);
		set_nvram_summary_addr(NVRAM_REPACK_SCALE, i, tp);
	}
	for(i=0;i<MAX_CLERK_NO;i++) 
	{
		//tp = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_REPACK_CLERK, i);
		get_nvram_summary_addr(NVRAM_REPACK_CLERK, i, tp);
		ClearSummaryRecord(tp,counter);
		set_nvram_summary_addr(NVRAM_REPACK_CLERK, i, tp);
	}
	for(i=0;i<MAX_SCALE;i++) 
	{
		//tp = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_OVERRIDE_SCALE, i);
		get_nvram_summary_addr(NVRAM_OVERRIDE_SCALE, i, tp);
		ClearSummaryRecord(tp,counter);
		set_nvram_summary_addr(NVRAM_OVERRIDE_SCALE, i, tp);
	}
	for(i=0;i<MAX_CLERK_NO;i++) 
	{
		//tp = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_OVERRIDE_CLERK, i);
		get_nvram_summary_addr(NVRAM_OVERRIDE_CLERK, i, tp);
		ClearSummaryRecord(tp,counter);
		set_nvram_summary_addr(NVRAM_OVERRIDE_CLERK, i, tp);
	}

	for(i=0;i<MAX_SCALE;i++) 
	{
		//tp = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_VOID_SCALE, i);
		get_nvram_summary_addr(NVRAM_VOID_SCALE, i, tp);
		ClearSummaryRecord(tp,counter);
		set_nvram_summary_addr(NVRAM_VOID_SCALE, i, tp);
	}
	for(i=0;i<MAX_CLERK_NO;i++) 
	{
		//tp = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_VOID_CLERK, i);
		get_nvram_summary_addr(NVRAM_VOID_CLERK, i, tp);
		ClearSummaryRecord(tp,counter);
		set_nvram_summary_addr(NVRAM_VOID_CLERK, i, tp);
	}
	for(i=0;i<MAX_SCALE;i++) 
	{
		//tp = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_RETURN_SCALE, i);
		get_nvram_summary_addr(NVRAM_RETURN_SCALE, i, tp);
		ClearSummaryRecord(tp,counter);
		set_nvram_summary_addr(NVRAM_RETURN_SCALE, i, tp);
	}
	for(i=0;i<MAX_CLERK_NO;i++) 
	{
		//tp = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_RETURN_CLERK, i);
		get_nvram_summary_addr(NVRAM_RETURN_CLERK, i, tp);
		ClearSummaryRecord(tp,counter);
		set_nvram_summary_addr(NVRAM_RETURN_CLERK, i, tp);
	}
#if !(ROM_COUNTRY == ROM_RUS)	// REPORT
	for(i=0;i<MAX_DEPT_NO;i++) 
	{
		//tp = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_RETURN_DEPT, i);
		get_nvram_summary_addr(NVRAM_RETURN_DEPT, i, tp);
		ClearSummaryRecord(tp,counter);
		set_nvram_summary_addr(NVRAM_RETURN_DEPT, i, tp);
	}
#endif

	for(i=0;i<MAX_SCALE;i++) 
	{
		//tp = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_DISCOUNT_SCALE, i);
		get_nvram_summary_addr(NVRAM_DISCOUNT_SCALE, i, tp);
		ClearSummaryRecord(tp,counter);
		set_nvram_summary_addr(NVRAM_DISCOUNT_SCALE, i, tp);
	}
	for(i=0;i<MAX_CLERK_NO;i++) 
	{
		//tp = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_DISCOUNT_CLERK, i);
		get_nvram_summary_addr(NVRAM_DISCOUNT_CLERK, i, tp);
		ClearSummaryRecord(tp,counter);
		set_nvram_summary_addr(NVRAM_DISCOUNT_CLERK, i, tp);
	}
#if !(ROM_COUNTRY == ROM_RUS)	// REPORT
	for(i=0;i<99+1;i++) 
	{
		//tp = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_DISCOUNT_CREDIT, i);
		get_nvram_summary_addr(NVRAM_DISCOUNT_CREDIT, i, tp);
		ClearSummaryRecord(tp,counter);
		set_nvram_summary_addr(NVRAM_DISCOUNT_CREDIT, i, tp);
	}
#endif

	for(i=0;i<MAX_SCALE;i++) 
	{
		//tp = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_PREPACK_SCALE, i);
		get_nvram_summary_addr(NVRAM_PREPACK_SCALE, i, tp);
		ClearSummaryRecord(tp,counter);
		set_nvram_summary_addr(NVRAM_PREPACK_SCALE, i, tp);
	}
	for(i=0;i<MAX_CLERK_NO;i++) 
	{
		//tp = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_PREPACK_CLERK, i);
		get_nvram_summary_addr(NVRAM_PREPACK_CLERK, i, tp);
		ClearSummaryRecord(tp,counter);
		set_nvram_summary_addr(NVRAM_PREPACK_CLERK, i, tp);
	}
	for(i=0;i<24+1;i++) 
	{
		//tp = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_PREPACK_HOUR, i);
		get_nvram_summary_addr(NVRAM_PREPACK_HOUR, i, tp);
		ClearSummaryRecord(tp,counter);
		set_nvram_summary_addr(NVRAM_PREPACK_HOUR, i, tp);
	}
	for(i=0;i<MAX_DEPT_NO;i++) 
	{
		//tp = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_PREPACK_DEPT, i);
		get_nvram_summary_addr(NVRAM_PREPACK_DEPT, i, tp);
		ClearSummaryRecord(tp,counter);
		set_nvram_summary_addr(NVRAM_PREPACK_DEPT, i, tp);
	}
	for(i=0;i<99+1;i++) 
	{
		//tp = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_PREPACK_GROUP, i);
		get_nvram_summary_addr(NVRAM_PREPACK_GROUP, i, tp);
		ClearSummaryRecord(tp,counter);
		set_nvram_summary_addr(NVRAM_PREPACK_GROUP, i, tp);
	}

	for(i=0;i<MAX_SCALE;i++) 
	{
		if(1<counter)
		{
			set_nvram_wparam(NVRAM_CASH_SCALE+i*4, 0);
			set_nvram_wparam(NVRAM_CASH_SCALE+i*4+2, 0);
		}
		else
			set_nvram_wparam(NVRAM_CASH_SCALE+i*4+counter*2, 0);
	}
	for(i=0;i<MAX_CLERK_NO;i++) 
	{
		if(1<counter)
		{
			set_nvram_wparam(NVRAM_CASH_CLERK+i*4, 0);
			set_nvram_wparam(NVRAM_CASH_CLERK+i*4+2, 0);
		}
		else
			set_nvram_wparam(NVRAM_CASH_CLERK+i*4+counter*2, 0);

	}
#ifdef USE_REPORT_PAY	//#if !(ROM_COUNTRY == ROM_RUS)	// REPORT
	for(i=0;i<MAX_SCALE;i++)
		for(j=0;j<MAX_PAID_TYPE;j++) 
		{
			//tp = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_PAY_SCALE, i*(MAX_PAID_TYPE)+j);
			get_nvram_summary_addr(NVRAM_PAY_SCALE, i*(MAX_PAID_TYPE)+j, tp);
			ClearSummaryRecord(tp,counter);
			set_nvram_summary_addr(NVRAM_PAY_SCALE, i*(MAX_PAID_TYPE)+j, tp);
		}
	for(i=0;i<MAX_CLERK_NO;i++)
		for(j=0;j<MAX_PAID_TYPE;j++) 
		{
			//tp = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_PAY_CLERK, i*(MAX_PAID_TYPE)+j);
			get_nvram_summary_addr(NVRAM_PAY_CLERK, i*(MAX_PAID_TYPE)+j, tp);
			ClearSummaryRecord(tp,counter);
			set_nvram_summary_addr(NVRAM_PAY_CLERK, i*(MAX_PAID_TYPE)+j, tp);
		}

	for(i=0;i<MAX_SCALE;i++) 
	{
			//tp = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_CHANGE_SCALE, i);
			get_nvram_summary_addr(NVRAM_CHANGE_SCALE, i, tp);
			ClearSummaryRecord(tp,counter);
			set_nvram_summary_addr(NVRAM_CHANGE_SCALE, i, tp);
	}
	for(i=0;i<MAX_CLERK_NO;i++) 
	{
			//tp = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_CHANGE_CLERK, i);
			get_nvram_summary_addr(NVRAM_CHANGE_CLERK, i, tp);
			ClearSummaryRecord(tp,counter);
			set_nvram_summary_addr(NVRAM_CHANGE_CLERK, i, tp);
	}
#endif
	for(i=0;i<MAX_SCALE;i++) 
	{
			//tp = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_ROUND_TOTAL_SCALE, i);
			get_nvram_summary_addr(NVRAM_ROUND_TOTAL_SCALE, i, tp);
			ClearSummaryRecord(tp,counter);
			set_nvram_summary_addr(NVRAM_ROUND_TOTAL_SCALE, i, tp);
	}
	for(i=0;i<MAX_CLERK_NO;i++) 
	{
			//tp = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_ROUND_TOTAL_CLERK, i);
			get_nvram_summary_addr(NVRAM_ROUND_TOTAL_CLERK, i, tp);
			ClearSummaryRecord(tp,counter);
			set_nvram_summary_addr(NVRAM_ROUND_TOTAL_CLERK, i, tp);
	}
	for(i=0;i<MAX_CLERK_NO;i++) 
	{
			//tp = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_NEGATIVE_CLERK, i);
			get_nvram_summary_addr(NVRAM_NEGATIVE_CLERK, i, tp);
			ClearSummaryRecord(tp,counter);
			set_nvram_summary_addr(NVRAM_NEGATIVE_CLERK, i, tp);
	}

	if (Startup.use_ext_nvram == 2 && counter == 99)	
	{
		ClearNVExtPLUSummaries();
	}
//	sprintf(MsgBuf, "time=%d\r\n", (INT16U)(SysTimer1_25ms-prev_Time));
//	MsgOut(MsgBuf);
#endif
	return 0;
}

void ClearNVExtPLUSummaries(void)
{
	//memset((HUGEDATA INT8U *)(NVEXT2_REPORT1_PLU_UPDATE_DATE+NVRAM_BASE), 0, 8);
	nvram_set(NVEXT2_REPORT1_PLU_UPDATE_DATE, 0, 8);
	//memset((HUGEDATA INT8U *)(NVEXT2_REPORT2_PLU_UPDATE_DATE+NVRAM_BASE), 0, 8);
	nvram_set(NVEXT2_REPORT2_PLU_UPDATE_DATE, 0, 8);
	//memset((HUGEDATA INT8U *)(NVEXT2_REPORT_PLU_UPDATE_DATE+NVRAM_BASE), 0, 4);
	nvram_set(NVEXT2_REPORT_PLU_UPDATE_DATE, 0, 4);
}

//Added by JJANG 20080519
//Parameter 625 : 2 && 확장메모리 有
//Back Up PLU REPORT
void PLUDailyReportBackup(INT8U rType)
{
	HUGEDATA INT16U *dest;
	HUGEDATA INT16U *p;
	INT32U i;

	//p = (HUGEDATA INT16U *)(NVRAM_PLU_SUMMARY+NVRAM_BASE);
	Report_SetPLUReportDate(rType);	//write, Back up time <- current time

	if (rType == 1)
	{
		//dest = (NVEXT2_REPORT1_PLU_SUMMARY+NVRAM_BASE);
		//memcpy((HUGEDATA INT8U *)(NVEXT2_REPORT1_PLUSUM_HEADINDEX+NVRAM_BASE), (INT8U *)(NVRAM_PLUSUM_HEADINDEX+NVRAM_BASE), 2);
		nvram_cpy((NVEXT2_REPORT1_PLUSUM_HEADINDEX), (NVRAM_PLUSUM_HEADINDEX), 2);
		//memcpy((HUGEDATA INT8U *)(NVEXT2_REPORT1_PLUSUM_TAILINDEX+NVRAM_BASE), (INT8U *)(NVRAM_PLUSUM_TAILINDEX+NVRAM_BASE), 2);
		nvram_cpy((NVEXT2_REPORT1_PLUSUM_TAILINDEX), (NVRAM_PLUSUM_TAILINDEX), 2);
		//memcpy((HUGEDATA INT8U *)(NVEXT2_REPORT1_PLUSUM_LRU+NVRAM_BASE), (INT8U *)(NVRAM_PLUSUM_LRU+NVRAM_BASE), 2);
		nvram_cpy((NVEXT2_REPORT1_PLUSUM_LRU), (NVRAM_PLUSUM_LRU), 2);
		//memcpy((HUGEDATA INT8U *)(NVEXT2_REPORT1_PLUSUM_MRU+NVRAM_BASE), (INT8U *)(NVRAM_PLUSUM_MRU+NVRAM_BASE), 2);
		nvram_cpy((NVEXT2_REPORT1_PLUSUM_MRU), (NVRAM_PLUSUM_MRU), 2);
		//for (i=0; i<(MAX_PLU_NO*NVRAM_PLU_SALE_SUMMARY_SIZE)/2; i++)
		//{
		//	*dest++ = *p++;
		//}
		nvram_cpy((NVEXT2_REPORT1_PLU_SUMMARY), (NVRAM_PLU_SUMMARY), (MAX_PLU_NO*NVRAM_PLU_SALE_SUMMARY_SIZE)/2);
	}
	else if (rType == 2)
	{
		//dest = (NVEXT2_REPORT2_PLU_SUMMARY+NVRAM_BASE);
		//memcpy((HUGEDATA INT8U *)(NVEXT2_REPORT2_PLUSUM_HEADINDEX+NVRAM_BASE), (INT8U *)(NVRAM_PLUSUM_HEADINDEX+NVRAM_BASE), 2);
		nvram_cpy((NVEXT2_REPORT2_PLUSUM_HEADINDEX), (NVRAM_PLUSUM_HEADINDEX), 2);
		//memcpy((HUGEDATA INT8U *)(NVEXT2_REPORT2_PLUSUM_TAILINDEX+NVRAM_BASE), (INT8U *)(NVRAM_PLUSUM_TAILINDEX+NVRAM_BASE), 2);
		nvram_cpy((NVEXT2_REPORT2_PLUSUM_TAILINDEX), (NVRAM_PLUSUM_TAILINDEX), 2);
		//memcpy((HUGEDATA INT8U *)(NVEXT2_REPORT2_PLUSUM_LRU+NVRAM_BASE), (INT8U *)(NVRAM_PLUSUM_LRU+NVRAM_BASE), 2);
		nvram_cpy((NVEXT2_REPORT2_PLUSUM_LRU), (NVRAM_PLUSUM_LRU), 2);
		//memcpy((HUGEDATA INT8U *)(NVEXT2_REPORT2_PLUSUM_MRU+NVRAM_BASE), (INT8U *)(NVRAM_PLUSUM_MRU+NVRAM_BASE), 2);
		nvram_cpy((NVEXT2_REPORT2_PLUSUM_MRU), (NVRAM_PLUSUM_MRU), 2);
		//for (i=0; i<(MAX_PLU_NO*NVRAM_PLU_SALE_SUMMARY_SIZE)/2; i++)
		//{
		//	*dest++ = *p++;
		//}
		nvram_cpy((NVEXT2_REPORT2_PLU_SUMMARY), (NVRAM_PLU_SUMMARY), (MAX_PLU_NO*NVRAM_PLU_SALE_SUMMARY_SIZE)/2);
	}
}


//Added by JJANG 20080519
//Parameter 625 : 2 && 확장메모리 有
//Check : 저울켰을 때, PLU 판매 시
void ReportUseDailyReportBackUp(void)
{
	INT8U  rType = 0;

	if (Startup.use_ext_nvram != 2)	return;
	if (!Report_GetPLUReportDate())		//날짜가 다를시에 백업 실행
	{
		rType = get_nvram_bparam(NVEXT2_REPORT_PLU_BACKUP_INDEX);
		PLUDailyReportBackup(rType+1);	//ext_report 1, ext_report 2	번갈아 가며 백업 실행
		rType = rType^1;
		set_nvram_bparam(NVEXT2_REPORT_PLU_BACKUP_INDEX, rType);
	}
}

INT16S InitSalesSummaryRecord(PLU_SALES_SUMMARY_TYPE*  pplu,INT16U index, INT8U pluType,INT32U id,INT16U Dept,INT16U prev,INT16U next)
{
	INT16S i;
	
	pplu->pstPLUtype= pluType;
	pplu->pstPLU= id;
	pplu->pstDept= Dept;
	pplu->pstCountNumber = 0;
	for (i=0;i<MAX_REPORT_PERIOD;i++)
	{
		pplu->pstTransactions[i]=0L;
		pplu->pstVolume[i]=0L;
		pplu->pstWeight[i]=0L;
		pplu->pstQty[i]=0L;
		pplu->pstPrepackWeight[i] = 0;
		pplu->pstPrepackVolume[i] = 0;
#if (!(ROM_COUNTRY == ROM_RUS) && !defined(USE_INDIA_FUNCTION))
		pplu->pstCountOfLabelVolume[i] = 0;
		pplu->pstCountOfLabel[i] = 0;
		pplu->pstTax[i] = 0;
#endif
	}
#ifdef USE_INDIA_FUNCTION
	pplu->pstInventory = 0;
	pplu->pstInvGross = 0;
	pplu->pstInvBagWt = 0;
#endif
	
	pplu->pstPrev= prev; 
	pplu->pstNext= next; 
	// put new PLU at tail of MRU list
	////PluSummaryLRU= index;
	set_nvram_wparam(NVRAM_PLUSUM_LRU, index);
	return 0;
}

PLU_SALES_SUMMARY_TYPE pst;
PLU_SALES_SUMMARY_TYPE pstNew;
PLU_SALES_SUMMARY_TYPE pstPrev;
PLU_SALES_SUMMARY_TYPE* FindPluSummary(INT16S mode,INT32U id,INT16U dept,INT8U pluType,INT16U *pindex)
{
	PLU_SALES_SUMMARY_TYPE*  pplu;
	PLU_SALES_SUMMARY_TYPE*  ppluNew;
	PLU_SALES_SUMMARY_TYPE*  plu_summary;
	INT16U index,prev,next,NewIndex;
	INT16U tail_index;

	if(GlbOper.PluReportType) return NULL;
// for now, linear search,
	*pindex=0;
	////index= PluSummaryHeadIndex;
	index = get_nvram_wparam(NVRAM_PLUSUM_HEADINDEX);
	while (1)
	{
		if (index > MAX_PLU_REPORT_NO)
		{	
			return NULL;
		}
		//pplu = (PLU_SALES_SUMMARY_TYPE *)get_nvram_plu_summary(NVRAM_PLU_SUMMARY, index);
		get_nvram_plu_summary(NVRAM_PLU_SUMMARY, index, &pst);
		pplu = &pst;
		if ((id==pplu->pstPLU) && (dept==pplu->pstDept))
		{
				*pindex=index;
				return pplu; //found it

		}
		if ((id < pplu->pstPLU) || (index >=get_nvram_wparam(NVRAM_PLUSUM_TAILINDEX)) || pplu->pstNext==0)	
		{
			// not found
			if ((get_nvram_wparam(NVRAM_PLUSUM_TAILINDEX) >= MAX_PLU_REPORT_NO) || mode==0/*find only*/)
			{
				if (mode==1)
				{
				}
				return NULL;
			}
		// create a new PLU record
			NewIndex= get_nvram_wparam(NVRAM_PLUSUM_TAILINDEX);
			//ppluNew = (PLU_SALES_SUMMARY_TYPE *)get_nvram_plu_summary(NVRAM_PLU_SUMMARY, NewIndex);
			get_nvram_plu_summary(NVRAM_PLU_SUMMARY, NewIndex, &pstNew);
			ppluNew = &pstNew;
			if (id < pplu->pstPLU) // insert new record
			{
				prev= pplu->pstPrev; 
				next= index;
				if(index == get_nvram_wparam(NVRAM_PLUSUM_HEADINDEX))
				{
					set_nvram_wparam(NVRAM_PLUSUM_HEADINDEX, NewIndex);
					prev=0;
				}

			}else  // append new record
			{
				if(NewIndex == get_nvram_wparam(NVRAM_PLUSUM_HEADINDEX))
					prev=0;
				else
					prev= index; 
				next= 0;
			}
			
			InitSalesSummaryRecord(ppluNew,NewIndex,pluType,id,dept,prev,next);
		// adjust old record's pointer
			if (id < pplu->pstPLU) // insert new record
			{
				if (pplu->pstPrev !=0) {//not first record
					//plu_summary = (PLU_SALES_SUMMARY_TYPE *)get_nvram_plu_summary(NVRAM_PLU_SUMMARY, pplu->pstPrev);
					get_nvram_plu_summary(NVRAM_PLU_SUMMARY, pplu->pstPrev, &pstPrev);
					plu_summary = &pstPrev;
					plu_summary->pstNext = NewIndex;
					set_nvram_plu_summary(NVRAM_PLU_SUMMARY, pplu->pstPrev, &pstPrev);
				}
				pplu->pstPrev= NewIndex;
				set_nvram_plu_summary(NVRAM_PLU_SUMMARY, index, &pst);
			}
			else //append
			{
				if (index != NewIndex) // not first record
				{
					pplu->pstNext= NewIndex;
					set_nvram_plu_summary(NVRAM_PLU_SUMMARY, index, &pst);
				}
			}
			tail_index = get_nvram_wparam(NVRAM_PLUSUM_TAILINDEX);
			++tail_index;
			set_nvram_wparam(NVRAM_PLUSUM_TAILINDEX, tail_index);
			*pindex= NewIndex;
			return ppluNew;
		}
		index= pplu->pstNext;
	}
}

extern SALE_G_OPER_TYPE	GlbOper;
#ifdef USE_SETPRODUCT_SALE	
INT8U set_sale_started = OFF;    
#endif
void	InsertTransactionRecord(INT8U trtType,INT8U trtStatus,INT8U trtStatus2,INT16S operator,INT8U dept,INT32U pluNumber,INT8U pluType,INT8U group,PRICE_TYPE *pPrice,TRANSACTION_RECORD_TYPE* ptrans,TRANSACTION_RECORD_SUB_TYPE* ptrans_sub,long rprice)
{
	INT8U	Discount_flag;
#ifdef USE_SETPRODUCT_SALE	
	INT16U setNumber;
#endif
	Discount_flag = get_global_bparam(GLOBAL_SALE_SETUP_FLAG7);

	//InsertAdditionalData(ptrans_add);

	ptrans->trtType = trtType;
	ptrans->trtStatus = trtStatus;
	ptrans->trtStatus2 = trtStatus2;	// CJK080320

	ptrans->trtScaleId = status_scale.scaleid;
	ptrans->trtPLUtype= pluType;
#ifdef USE_SETPRODUCT_SALE
	if(SetproductSaleMode)
		ptrans->trtPLUtype=PLU_BY_WEIGHT;
#endif
	if (ptrans->trtPLUtype == PLU_BY_WEIGHT)
	{
#ifdef USE_GERMANY_TICKET_DISCOUNT			
		ptrans->trtDiscountSavedvalue = Price.Weight[PRICE_SAVED];
		ptrans->trtWeight = Price.Weight[PRICE_RESULT];
#else
		if(!(Discount_flag&0x01)){
			ptrans->trtWeight = Price.Weight[PRICE_RESULT];
		}
		else ptrans->trtWeight = Price.Weight[PRICE_ORIGIN];
#endif
#ifdef USE_HYUNDAI_SET_FUNCTION
		if(Operation.saleStatus == PRINT_SET_LABEL)
			ptrans->trtWeight=0;
#endif
		if (Operation.fixedPrice) {
			ptrans->trtWeight=0;
		}
		ptrans->trtQty = 0;
#ifndef USE_TARE_IN_TICKET
		ptrans->trtPCS = 0;
#else
		ptrans->trtPCS = status_scale.Tare;
#endif
		ptrans->trtMultiple=ADM_GetData.CurUnit;	// Modified by CJK 20060510
	}
	else// if (pscb->scbPLUtype== PLU_BY_PCS)
	{
		ptrans->trtWeight = 0;
		ptrans->trtQty = Price.Qty[PRICE_RESULT];
#ifdef USE_GERMANY_TICKET_DISCOUNT		
		ptrans->trtDiscountSavedvalue = Price.Qty[PRICE_SAVED];		
#endif
		ptrans->trtPCS = Price.UnitCount[PRICE_RESULT];
		ptrans->trtMultiple=ADM_Status.UseUnit;	// Modified by CJK 20060510
	}

	ptrans->trtGroup= group;
	ptrans->trtOperator= operator ;
	ptrans->trtPLU= pluNumber;
	if(MISC_PLU_WEIGHT<=pluNumber)	dept=status_scale.departmentid;  //SG070604	
	ptrans->trtDept= dept;
	ptrans->trtCode= status_scale.Plu.ItemCode;
#ifdef USE_SETPRODUCT_SALE
	if(SetproductSaleMode) 
	{
		if (ptrans->trtStatus & 0x80)
			ptrans->trtPLU = 289467;
		else {
			pPrice->TotalPrice[PRICE_RESULT] = 0;
			pPrice->UnitPrice[PRICE_RESULT] = 0;
		}
		if(set_sale_started == OFF) {
			set_sale_started = ON;
			setNumber = get_setproduct_number();
			setNumber++;
			if(setNumber > 9999)
				setNumber = 1;
			set_setproduct_number(setNumber);
		}
		ptrans->trtCode= status_scale.scaleid * 10000L + get_setproduct_number();
	}
#endif

#ifdef USE_INDIA_FUNCTION
	if (UseItemCode13Digit)
	{
		ptrans->trtCode = 0;
	}
#endif
	if(!(Discount_flag&0x01)) ptrans->trtUnitPrice = pPrice->UnitPrice[PRICE_RESULT];
	else ptrans->trtUnitPrice = pPrice->UnitPrice[PRICE_ORIGIN];
	if(!rprice)
	{
		ptrans->trtVolume= pPrice->TotalPrice[PRICE_RESULT];	//Modified by JJANG 20080104 무조건 RESULT값을 찍도록!
	}
	else
	{
		ptrans->trtVolume= rprice;
	}

	ptrans->trtDiscountVolume= pPrice->TotalPrice[PRICE_SAVED];

	ptrans->trtTransactionCounter = GetTransactionCounter();
	ptrans->trtTicketNumber	= get_TicketTransactionNumber(operator);

#ifdef USE_TAX_RATE    
	ptrans->trtTaxNumber=pPrice->TaxNumber;
	ptrans->trtTax=pPrice->TaxPrice;
	ptrans->trtTaxType = pPrice->ExTax;
#else
	ptrans->trtTaxNumber=0;
	ptrans->trtTax=0;
	ptrans->trtTaxType = 0;
#endif    
	ptrans->trtCreditCustomerNo = get_nvram_wparam(NVRAM_CLERK_CUSTOMER_NO + operator*2);

#ifdef USE_TRACE_STANDALONE
	ptrans->trtIndividualIndex = CurTraceStatus.indivStr.index%100;
	ptrans->trtStatus2 |= FLAG2_UPPER_NIBBLE_MASK&((CurTraceStatus.indivStr.index/100%10)<<4);	// 개체인덱스의 100의자리만 저장 (농협 개체 999개 지원)
	if (CurTraceStatus.indivStr.lotFlag) ptrans->trtIndividualIndex |= 0x80;	// 상위1bit : 개체or 묶
	else                                 ptrans->trtIndividualIndex &= (~0x80);
// 추가 transaction buffer
	memcpy(ptrans_sub->indivNo, CurTraceStatus.indivStr.individualNO, sizeof(ptrans_sub->indivNo));	
/////////////////
#elif defined(USE_CHN_CART_SCALE)
	ptrans->trtIndividualIndex = CurCHNTraceLotNo;
	memcpy(ptrans->trtTraceCode, CurCHNTrace.code, sizeof(ptrans->trtTraceCode));	
#elif defined(USE_CHN_EMART_TRACE)
	memcpy(ptrans_sub->indivNo, CHNEmart_CurTraceCode, sizeof(ptrans_sub->indivNo));
#else
	ptrans->trtIndividualIndex = 0;
#endif
#ifdef USE_HYUNDAI_CHECK_REASON_CODE
	ptrans->trtReaseonCode = status_scale.Plu.sales_msg;
#endif
#ifdef USE_CL5J_REALTIME_TRNASFER
	ptrans->trtTareWeight = status_scale.Tare;
#endif
}

void InsertUnitPriceChangeRecord(INT8U scaleNum,INT8U dept,INT32U pluNumber,INT32U OriginUnitPrice,INT32U ResultUnitPrice,INT8U Reason_code,INT8U operator,INT8U changeFlag,UNITPRICE_CHANGE_TYPE *chgprice)
{
	chgprice->trtScaleId = scaleNum; 
	chgprice->trtDept = dept;
	chgprice->trtPLUNo = pluNumber;
#ifdef USE_DIVIDING_FUNCTION
	if(status_scale.divisor)
	{
		OriginUnitPrice /= status_scale.divisor;
	}
#endif
	chgprice->trtOriginUnitPrice = OriginUnitPrice;
	chgprice->trtResultUnitPrice = ResultUnitPrice;
	chgprice->trtReasonCode = Reason_code;
	chgprice->trtClerkId = operator;
	chgprice->trtChangeFlag = changeFlag;
}

void InsertAdditionalData(TR_ADDITION_DATA_TYPE* pTrans_add)
{
#ifdef USE_PERSIAN_CALENDAR
	RTC_Convert_Persian_Read();
#else
	RTC_CLK_Burst_Read();
#endif
	pTrans_add->adtDate[0] = bcd2hex(RTCStruct.year);
	pTrans_add->adtDate[1] = bcd2hex(RTCStruct.month);
	pTrans_add->adtDate[2] = bcd2hex(RTCStruct.date);
	pTrans_add->adtDate[3] = bcd2hex(RTCStruct.hour);
	pTrans_add->adtDate[4] = bcd2hex(RTCStruct.min);
	pTrans_add->adtDate[5] = bcd2hex(RTCStruct.sec);
	pTrans_add->adtPrefix = status_scale.Plu.prefix;
	memcpy(pTrans_add->adtPluName, status_scale.Plu.ItemName, sizeof(pTrans_add->adtPluName));
	pTrans_add->adtPluName[sizeof(pTrans_add->adtPluName)-1] = 0;
	pTrans_add->adtItemCode = status_scale.Plu.ItemCode;
#ifdef USE_INDIA_FUNCTION
	if (UseItemCode13Digit)
	{
		pTrans_add->adtItemCode = 0;
	}
#endif
	if(get_nvram_bparam(NVRAM_BUSINESS_DATE_FLAG))
	{
		pTrans_add->adtSaleDate[0] = bcd2hex(get_nvram_bparam(NVRAM_BUSINESS_DATE+2));
		pTrans_add->adtSaleDate[1] = bcd2hex(get_nvram_bparam(NVRAM_BUSINESS_DATE+1));
		pTrans_add->adtSaleDate[2] = bcd2hex(get_nvram_bparam(NVRAM_BUSINESS_DATE));
	}
	else
	{
		pTrans_add->adtSaleDate[0] = bcd2hex(RTCStruct.year);
		pTrans_add->adtSaleDate[1] = bcd2hex(RTCStruct.month);
		pTrans_add->adtSaleDate[2] = bcd2hex(RTCStruct.date);
	}
#ifdef USE_IMPORT_MEAT_PROTOCOL
    #ifdef USE_TRACE_MEATPART
	if (CurTraceStatus.meatPart) pTrans_add->Reserved = CurTraceStatus.meatPart;
    #else
	if (CurTraceStatus.meatUse) pTrans_add->Reserved = CurTraceStatus.meatUse;
    #endif
	else pTrans_add->Reserved = 11;	//부위, 기타코드 BF011
#endif
}

void InsertAddWeightData(PRICE_TYPE *pPrice)
{
	INT32U w_value;
	
	w_value = get_nvram_lparam(NVRAM_SUBTOTAL_NET_WEIGHT);
	w_value += pPrice->NetWeight;
	set_nvram_lparam(NVRAM_SUBTOTAL_NET_WEIGHT, w_value);

#ifndef USE_PERCENT_WEIGHT
	w_value = get_nvram_lparam(NVRAM_SUBTOTAL_PER_TARE_WEIGHT);
	w_value += pPrice->PercentTareWeight;
	set_nvram_lparam(NVRAM_SUBTOTAL_PER_TARE_WEIGHT, w_value);
#endif

	w_value = get_nvram_lparam(NVRAM_SUBTOTAL_GROSS_WEIGHT);
	w_value += pPrice->GrossWeight;
	set_nvram_lparam(NVRAM_SUBTOTAL_GROSS_WEIGHT, w_value);
}

#ifdef USE_HYUNDAI_MEAT_FUNCTION
void InsertBarcodetoTransactionRecord(TRANSACTION_RECORD_TYPE* ptrans)
{
	INT8U len;
	len = strlen((char*)BarcodeData);
	if (len > sizeof(ptrans->trtBarcodeStr)) len = sizeof(ptrans->trtBarcodeStr);
	memcpy(ptrans->trtBarcodeStr, BarcodeData, len);
	ptrans->trtBarcodeStr[len] = 0;
}
#endif

INT16S AddSummaryData(SALES_SUMMARY_TYPE *sp,INT16S index, long aprice,long weight,long qty, INT8U reopen_flag)
{
	if(reopen_flag)
	{
		if(1<index)
		{
			for(index=0;index<MAX_REPORT_PERIOD;index++)
			{
				sp->sstTransactions[index]--;
				sp->sstVolume[index]-=aprice;
				sp->sstWeight[index]-=weight;
				sp->sstQty[index]-=qty;
			}
			
		}
		else
		{
			sp->sstTransactions[index]--;
			sp->sstVolume[index]-=aprice;
			sp->sstWeight[index]-=weight;
			sp->sstQty[index]-=qty;
		}

	}
	else
	{
		if(1<index)
		{
			for(index=0;index<MAX_REPORT_PERIOD;index++)
			{
				sp->sstTransactions[index]++;
				sp->sstVolume[index]+=aprice;
				sp->sstWeight[index]+=weight;
				sp->sstQty[index]+=qty;
			}
			
		}
		else
		{
			sp->sstTransactions[index]++;
			sp->sstVolume[index]+=aprice;
			sp->sstWeight[index]+=weight;
			sp->sstQty[index]+=qty;
		}
	}
	return 0;
}


void ClerkGrandTotalSum(INT8U clerkNum, STRUCT_TOTAL_PRICE *totalData)
{
	INT16U temp;
	INT32U ltemp;
	INT32U addr;

	if (Operation.operationClerk != OPER_NORMAL) return;

	addr = NVRAM_GTOTAL_PRICE+clerkNum*4;
	ltemp = get_nvram_lparam(addr);
	ltemp += totalData->SummaryTotalPrice;
	set_nvram_lparam(addr, ltemp);

	addr = NVRAM_GTOTAL_COUNT+clerkNum*2;
	temp = get_nvram_wparam(addr);
	temp += totalData->TotalCount;
	//temp++;
	set_nvram_wparam(addr, temp);

	addr = NVRAM_GTOTAL_WEIGHT+clerkNum*4;
	ltemp = get_nvram_lparam(addr);
	ltemp += totalData->TotalWeight;
	set_nvram_lparam(addr, ltemp);

	addr = NVRAM_GTOTAL_QTY+clerkNum*2;
	temp = get_nvram_wparam(addr);
	temp += totalData->TotalQuantity;
	set_nvram_wparam(addr, temp);

	if (!clerkNum)
	{
		ltemp = get_nvram_lparam(NVRAM_GTOTAL_NET_WEIGHT);
		ltemp += totalData->TotalNetWeight;
		set_nvram_lparam(NVRAM_GTOTAL_NET_WEIGHT, ltemp);

		ltemp = get_nvram_lparam(NVRAM_GTOTAL_PER_TARE_WEIGHT);
		ltemp += totalData->TotalPerTareWeight;
		set_nvram_lparam(NVRAM_GTOTAL_PER_TARE_WEIGHT, ltemp);

		ltemp = get_nvram_lparam(NVRAM_GTOTAL_GROSS_WEIGHT);
		ltemp += totalData->TotalGrossWeight;
		set_nvram_lparam(NVRAM_GTOTAL_GROSS_WEIGHT, ltemp);
	}

}
long LocalPayTotalSummaries(TRANSACTION_PAY Pay,INT8U recvmode, INT8U reopen_flag)
{
	SALES_SUMMARY_TYPE* sale_summary;
	SALES_SUMMARY_TYPE sst;
	TRANSACTION_PAY *pPay;
	INT16S i;
	long summary=0;
	INT16U	bufdata_count=0;
	INT8U	ret_value=0;
	INT16U	customer_no = 0;

	INT32U clerk_addr;				//insert by se-hyung for training key 20070627
	CLERK_STRUCT clerk_st;				//insert by se-hyung for training key 20070627
	INT8U scaleNum;
#ifdef USE_DISCOUNTED_TAX_FOR_DISCOUNTED_TOTAL
	long total_tax;
	INT8U tax_type;
    INT32U tempAddr;
	INT8U j;
#endif

	sale_summary = &sst;

	clerk_addr = Pay.clerkid;			//insert by se-hyung for training key 20070627
	clerk_addr *= CLERK_STRUCT_SIZE;		//insert by se-hyung for training key 20070627
	clerk_addr += FLASH_CLERK_AREA;			//insert by se-hyung for training key 20070627
   	get_base_sparam( clerk_addr, (INT8U *)&clerk_st, CLERK_STRUCT_SIZE);   //insert by se-hyung for training key 20070627	

 	if(clerk_st.training == 1)	return 0; //insert by se-hyung for training key 20070627  training 모드일때 정산 안함.
 	if(flagcopy_ticket)			return 0; // Copy Ticket 시 Report 관련 동작을 수행하지 않음 


	pPay = &Pay;
	if(Operation.operationMode != MODE_REPACK)
	{
		if (!recvmode) {
			//Modified by JJANG 20080314
			if(ethernet_list.status==4)
			{
			        bufdata_count = GetClerkTransactionCount(pPay->clerkid);		
				ret_value = GetCheckAddTransactionData(1,bufdata_count,AUDIT_MODE_LINEAR);		//mode : pay
			}
			else ret_value = 1;
			if(ret_value)
			{
#ifdef USE_INDIA_FUNCTION
				if (ethernet_list.status == 3 && UseAreaScaleMode == 2)
				{
					transaction_pay_store_type8(pPay);
				}
				else
				{
					transaction_pay_store(pPay);
					if (Operation.operationClerk == OPER_FLOATING && ethernet_list.status == 2) 
					{	// CJK080307
						transaction_pay_send(&(CommBufEthData[0]),1);
					}
				}
#else
				transaction_pay_store(pPay);
				if (Operation.operationClerk == OPER_FLOATING && ethernet_list.status == 2) 
				{	// CJK080307
					transaction_pay_send(&(CommBufEthData[0]),1);
				}
#endif
			}
			else return 0;

		}
	}
	if(ethernet_list.status == 2)     return 0;

	customer_no = get_nvram_wparam(NVRAM_CLERK_CUSTOMER_NO + pPay->clerkid*2);
	if(customer_no)     return 0;

	scaleNum = pPay->scaleid;
	if (pPay->scaleid > MAX_SCALE - 1) scaleNum = MAX_SCALE - 1;
	Get_SalesSummaryRenewal(1, pPay->summarydiscount, pPay->clerkid, scaleNum, reopen_flag);	//Added by JJANG 20080416
	switch(pPay->trtType)
	{
		case TRANS_TYPE_PAY_TOTAL_NOPAY:// normal total
		case TRANS_TYPE_PAY_TOTAL_PAY:// pay total
			// clerk total pay[MAX_PAID_TYPE] & scale total pay[MAX_PAID_TYPE] summary 
			for(i=0;i<MAX_PAID_TYPE;i++)
			{
				if(Operation.operationMode!=MODE_REPACK && pPay->paidprice[i])
				{
#ifdef USE_REPORT_PAY	//#if !(ROM_COUNTRY == ROM_RUS)	// REPORT
					//sale_summary = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_PAY_CLERK, pPay->clerkid*(MAX_PAID_TYPE)+i);
					get_nvram_summary_addr(NVRAM_PAY_CLERK, pPay->clerkid*(MAX_PAID_TYPE)+i, sale_summary);
					AddSummaryData(sale_summary,2,pPay->paidprice[i],0l,0l,reopen_flag);
					set_nvram_summary_addr(NVRAM_PAY_CLERK, pPay->clerkid*(MAX_PAID_TYPE)+i, sale_summary);
					//sale_summary = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_PAY_SCALE, scaleNum*(MAX_PAID_TYPE)+i);
					get_nvram_summary_addr(NVRAM_PAY_SCALE, scaleNum*(MAX_PAID_TYPE)+i, sale_summary);
					AddSummaryData(sale_summary,2,pPay->paidprice[i],0l,0l,reopen_flag);
					set_nvram_summary_addr(NVRAM_PAY_SCALE, scaleNum*(MAX_PAID_TYPE)+i, sale_summary);
#endif
				}
				summary += pPay->paidprice[i];

			}
			// clerk & scale change summary
			if(pPay->change)
			{
				if(Operation.operationMode!=MODE_REPACK)
				{
#ifdef USE_REPORT_PAY	//#if !(ROM_COUNTRY == ROM_RUS)	// REPORT
					//sale_summary = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_CHANGE_CLERK, pPay->clerkid);
					get_nvram_summary_addr(NVRAM_CHANGE_CLERK, pPay->clerkid, sale_summary);
					AddSummaryData(sale_summary,2,pPay->change,0l,0l,reopen_flag);
					set_nvram_summary_addr(NVRAM_CHANGE_CLERK, pPay->clerkid, sale_summary);
					//sale_summary = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_CHANGE_SCALE, scaleNum);
					get_nvram_summary_addr(NVRAM_CHANGE_SCALE, scaleNum, sale_summary);
					AddSummaryData(sale_summary,2,pPay->change,0l,0l,reopen_flag);
					set_nvram_summary_addr(NVRAM_CHANGE_SCALE, scaleNum, sale_summary);
#endif
				}
				summary -= pPay->change;
			}
			// clerk & scale pay total round summary
			if(Operation.operationMode!=MODE_REPACK && pPay->summaryround)
			{
				//sale_summary = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_ROUND_TOTAL_CLERK, pPay->clerkid);
				get_nvram_summary_addr(NVRAM_ROUND_TOTAL_CLERK, pPay->clerkid, sale_summary);
				AddSummaryData(sale_summary,2,pPay->summaryround,0l,0l,reopen_flag);
				set_nvram_summary_addr(NVRAM_ROUND_TOTAL_CLERK, pPay->clerkid, sale_summary);
				//sale_summary = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_ROUND_TOTAL_SCALE, scaleNum);
				get_nvram_summary_addr(NVRAM_ROUND_TOTAL_SCALE, scaleNum, sale_summary);
				AddSummaryData(sale_summary,2,pPay->summaryround,0l,0l,reopen_flag);
				set_nvram_summary_addr(NVRAM_ROUND_TOTAL_SCALE, scaleNum, sale_summary);
			}
			////////////////////////////////////////
			// Total Discount양에 대한 Summary 필요 
			//  pPay->summarydiscount
			////////////////////////////////////////
			break;
		default:
			break;
	}
#ifdef USE_DISCOUNTED_TAX_FOR_DISCOUNTED_TOTAL
	for(tax_type = 0; tax_type < MAX_TAX_NO; tax_type++)
	{
		for (j = 0; j < MAX_REPORT_PERIOD; j++)
		{
			tempAddr = (scaleNum * MAX_TAX_NO * MAX_REPORT_PERIOD * 4) + (tax_type * MAX_REPORT_PERIOD * 4 + j * 4);
			total_tax = get_nvram_lparam(tempAddr+NVRAM_TAX);	//TAX Price Amount
			if(reopen_flag)
			{
				total_tax -= pPay->taxPrice[tax_type];
			}
			else
			{
				total_tax += pPay->taxPrice[tax_type];
			}
			set_nvram_lparam(tempAddr+NVRAM_TAX, total_tax);
			total_tax = get_nvram_lparam(tempAddr+NVRAM_TAX_GROSS);	// TAX Gross Amount
			if(reopen_flag)
			{
				total_tax -= pPay->taxGross[tax_type];
			}
			else
			{
				total_tax += pPay->taxGross[tax_type];
			}
			set_nvram_lparam(tempAddr+NVRAM_TAX_GROSS, total_tax);
		}
	}
#endif

	return summary;
}

TRANSACTION_PAY transPay;
INT16S LocalPluTotalSummaries(INT8U scale,TRANSACTION_RECORD_TYPE* pTrans,TRANSACTION_RECORD_SUB_TYPE* pTrans_sub,TR_ADDITION_DATA_TYPE* pTrans_add,INT8U recvmode,INT8U onlyvoid)
{
	INT16S i,j,overflow;
	unsigned char clerk;
	CLERK_STRUCT clerk_st;				   //insert by se-hyung for training key 20070627
	INT32U clerk_addr;				   //insert by se-hyung for training key 20070627
	SALES_SUMMARY_TYPE* tp;
	SALES_SUMMARY_TYPE sst;
	PLU_SALES_SUMMARY_TYPE* tp1;
	//PLU_SALES_SUMMARY_TYPE* plu_summary;
	SALES_SUMMARY_TYPE* sale_summary;
	SALES_SUMMARY_TYPE sst1;
	INT16U Hour,index;
	long	trtVolume,trtDiscountedVolume,trtWeight,trtQty,trtTax;
	long	transaction;  
#if (defined(USE_REPORT_TAX) && defined(USE_TAX_RATE))
	long total_tax;
#endif
	long weight_useunit;
	INT8U ret;
//	INT8U temp;   //SG071009
	long volume;
//	INT8U	ret_value = 0;
	INT16U	customer_no = 0;
#ifdef USE_TRACE_STANDALONE
	INT16U indivIdxNo;
#endif
#ifdef USE_INDIA_FUNCTION
	INT16U plu_addr;
	INT16U sz;
	INT8U  pluOrgType;
#endif

    tp = &sst;
    sale_summary = &sst1;

	if (scale > MAX_SCALE - 1) scale = MAX_SCALE - 1;	// scale no 32~255는 scale 31로 정산함
    
	if (!recvmode) 
	{
#ifdef USE_EMART_NETWORK
#ifdef USE_EMART_BACK_LABEL
		if (Operation.checkin || Operation.BackLabeledFlag)	//생산등록 ON or 후방라벨 발행.
#else
		if (Operation.checkin)	//생산등록 ON
#endif
#endif
		{	// Emart가 아닐경우 항상 수행
			pTrans->trtStatus2 |= FLAG2_TRANS_CHECKIN;
#ifdef USE_IMPORT_MEAT_PROTOCOL
			if (GlbOper.realtimeTransactionType == 6)
			{
				if (FlagDisplayIndividual) transaction_sale_store(pTrans, pTrans_sub, pTrans_add, NULL, 0, 0);
			}
			else
			{
  #ifndef USE_LOTTEMART_PROTOCOL
    #ifdef USE_HYUNDAI_SET_FUNCTION
  				if(!MeatSetSaleMode)
    #endif
				transaction_sale_store(pTrans, pTrans_sub, pTrans_add, NULL, 0, 0);
  #endif
			}
#else // USE_IMPORT_MEAT_PROTOCOL
  #ifndef USE_LOTTEMART_PROTOCOL
    #ifdef USE_SETPRODUCT_SALE
			if (!SetproductSaleMode)
    #endif
			transaction_sale_store(pTrans, pTrans_sub, pTrans_add, NULL, 0, 0);
    #ifdef USE_HANAPOS_PROTOCOL
			memcpy(&pTrans_Data, pTrans, sizeof(TRANSACTION_RECORD_TYPE));
			memcpy(&pTrans_add_Data, pTrans_add, sizeof(TR_ADDITION_DATA_TYPE));
    #endif // USE_HANAPOS_PROTOCOL
  #endif // USE_LOTTEMART_PROTOCOL
#endif // USE_IMPORT_MEAT_PROTOCOL
		}
		if (Operation.operationClerk == OPER_FLOATING && ethernet_list.status == 2)
		{
			ret = transaction_sale_send(&(CommBufEthData[0]));	// ret=0:ok, 1:timeout, 2:memory full, 3:send data error, 4:etc(setting err)
			if (ret) return ret;
		}
#ifdef USE_TRACE_STANDALONE
		if (GlbOper.UseSendTraceInformation) trace_information_send(&(CommBufEthData[0]), pTrans);
#endif
	}

	if(!GlbOper.XZreport)		return 0;
	if(!GlbOper.PrepackReport && (pTrans->trtStatus&FLAG_TRANS_PREPACK))	return 0;

	if(ethernet_list.status==2)     return 0;  //SG060615. slave 모드 일때에는 정산 안함
	
	customer_no = get_nvram_wparam(NVRAM_CLERK_CUSTOMER_NO + pTrans->trtOperator*2);
	if(customer_no)     return 0;

	clerk_addr = pTrans->trtOperator;		//insert by se-hyung for training key 20070627
	clerk_addr *= CLERK_STRUCT_SIZE;		//insert by se-hyung for training key 20070627
	clerk_addr += FLASH_CLERK_AREA;			//insert by se-hyung for training key 20070627
   	get_base_sparam( clerk_addr, (INT8U *)&clerk_st, CLERK_STRUCT_SIZE);   //insert by se-hyung for training key 20070627	

 	if(clerk_st.training == 1)	return 0; //insert by se-hyung for training key 20070627  training 모드일때 정산 안함.


	weight_useunit = ad_cnv_weight_unit(pTrans->trtWeight, ADM_GetData.CurUnit, ADM_Status.UseUnit);

	if(!(pTrans->trtType == TRANS_TYPE_REPACK) && (pTrans->trtStatus & FLAG_TRANS_VOID))
	{
		//sale_summary = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_VOID_SCALE, scale);
		get_nvram_summary_addr(NVRAM_VOID_SCALE, scale, sale_summary);
		AddSummaryData(sale_summary,2,pTrans->trtVolume,weight_useunit,pTrans->trtQty,OFF);
		set_nvram_summary_addr(NVRAM_VOID_SCALE, scale, sale_summary);

		//sale_summary = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_VOID_CLERK, pTrans->trtOperator);
		get_nvram_summary_addr(NVRAM_VOID_CLERK, pTrans->trtOperator, sale_summary);
		AddSummaryData(sale_summary,2,pTrans->trtVolume,weight_useunit,pTrans->trtQty,OFF);
		set_nvram_summary_addr(NVRAM_VOID_CLERK, pTrans->trtOperator, sale_summary);
	}
	if(recvmode && onlyvoid) return 0;	// CJK080311
	if(!(pTrans->trtType == TRANS_TYPE_REPACK) && (pTrans->trtStatus & FLAG_TRANS_NOVOID))
	{
		//sale_summary = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_VOID_SCALE, scale);
		get_nvram_summary_addr(NVRAM_VOID_SCALE, scale, sale_summary);
		AddSummaryData(sale_summary,2,-(pTrans->trtVolume),-(weight_useunit),-(pTrans->trtQty),OFF);
		set_nvram_summary_addr(NVRAM_VOID_SCALE, scale, sale_summary);

		//sale_summary = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_VOID_CLERK, pTrans->trtOperator);
		get_nvram_summary_addr(NVRAM_VOID_CLERK, pTrans->trtOperator, sale_summary);
		AddSummaryData(sale_summary,2,-(pTrans->trtVolume),-(weight_useunit),-(pTrans->trtQty),OFF);
		set_nvram_summary_addr(NVRAM_VOID_CLERK, pTrans->trtOperator, sale_summary);
	}

	if(!(pTrans->trtType == TRANS_TYPE_REPACK) && (pTrans->trtStatus & FLAG_TRANS_RETURN))
	{
		//sale_summary = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_RETURN_SCALE, scale);
		get_nvram_summary_addr(NVRAM_RETURN_SCALE, scale, sale_summary);
		AddSummaryData(sale_summary,2,pTrans->trtVolume,weight_useunit,pTrans->trtQty,OFF);
		set_nvram_summary_addr(NVRAM_RETURN_SCALE, scale, sale_summary);

		//sale_summary = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_RETURN_CLERK, pTrans->trtOperator);
		get_nvram_summary_addr(NVRAM_RETURN_CLERK, pTrans->trtOperator, sale_summary);
		AddSummaryData(sale_summary,2,pTrans->trtVolume,weight_useunit,pTrans->trtQty,OFF);
		set_nvram_summary_addr(NVRAM_RETURN_CLERK, pTrans->trtOperator, sale_summary);

#if !(ROM_COUNTRY == ROM_RUS)	// REPORT
		//sale_summary = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_RETURN_DEPT, pTrans->trtDept);
		get_nvram_summary_addr(NVRAM_RETURN_DEPT, pTrans->trtDept, sale_summary);
		AddSummaryData(sale_summary,2,pTrans->trtVolume,weight_useunit,pTrans->trtQty,OFF);
		set_nvram_summary_addr(NVRAM_RETURN_DEPT, pTrans->trtDept, sale_summary);
#endif
	}
	if(pTrans->trtStatus&FLAG_TRANS_NEGATIVE || pTrans->trtStatus&FLAG_TRANS_VOID || pTrans->trtStatus&FLAG_TRANS_RETURN)
	{
		if(pTrans->trtStatus&FLAG_TRANS_NEGATIVE && pTrans->trtStatus&FLAG_TRANS_VOID)
			transaction=1;
		else
			transaction=-1;
	}
	else
	{
		transaction=1;
	}
	trtVolume			=transaction*pTrans->trtVolume;
	trtDiscountedVolume		=transaction*pTrans->trtDiscountVolume;
	trtWeight			=transaction*weight_useunit;
	trtQty				=transaction*pTrans->trtQty;
#ifdef USE_TAX_RATE    
	trtTax				=transaction*pTrans->trtTax;
#endif    
	// transaction function으로 합류해야 함

	//OPTION: report, repack mode
	if (pTrans->trtType == TRANS_TYPE_REPACK)
	{
		//sale_summary = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_REPACK_SCALE, scale);
		get_nvram_summary_addr(NVRAM_REPACK_SCALE, scale, sale_summary);
		AddSummaryData(sale_summary,2,trtVolume,trtWeight,trtQty,OFF);
		set_nvram_summary_addr(NVRAM_REPACK_SCALE, scale, sale_summary);

		//sale_summary = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_REPACK_CLERK, pTrans->trtOperator);
		get_nvram_summary_addr(NVRAM_REPACK_CLERK, pTrans->trtOperator, sale_summary);
		AddSummaryData(sale_summary,2,trtVolume,trtWeight,trtQty,OFF);
		set_nvram_summary_addr(NVRAM_REPACK_CLERK, pTrans->trtOperator, sale_summary);

		return 0;
	}

	if(pTrans->trtType == TRANS_TYPE_SELF)
	{
	}

#ifdef USE_TRACE_STANDALONE
	indivIdxNo = (pTrans->trtIndividualIndex&0x7f)+(pTrans->trtStatus2>>4%10)*100;
	RemainWeightSummary(trtWeight, indivIdxNo);
#elif defined(USE_CHN_CART_SCALE)
	trace_remainWeightSummary(pTrans->trtPLU, pTrans->trtIndividualIndex, trtWeight);
#elif defined(USE_INDIA_FUNCTION)
	//if (UseInventoryMangement)
	{
		plu_table_search(pTrans->trtDept, pTrans->trtPLU, &plu_addr, 0);
		plu_get_field(plu_addr-1, PTYPE_PLU_TYPE,(INT8U *)&pluOrgType, &sz, sizeof(pluOrgType));
		DeductPluInventory(pTrans->trtDept, pTrans->trtPLU, pluOrgType, pTrans->trtPLUtype, trtWeight, trtQty, INVENTORY_DEDUCT);
	}
#endif

	overflow=0;
	for(j=0; j<12; ++j) 
	{
    	switch(j) 
	   	{
			case 0: // add transaction to Dept summary
				//tp = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_DEPT_SUMMARY, pTrans->trtDept);
				get_nvram_summary_addr(NVRAM_DEPT_SUMMARY, pTrans->trtDept, tp);
				break;
			case 1: // add transaction to Scale Total summary
				//tp = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_SCALE_SUMMARY, scale);
				get_nvram_summary_addr(NVRAM_SCALE_SUMMARY, scale, tp);
				break;
			case 2: // add transaction to Group summary
				//tp = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_GROUP_SUMMARY, pTrans->trtGroup);
				get_nvram_summary_addr(NVRAM_GROUP_SUMMARY, pTrans->trtGroup, tp);
				break;
			case 3: //  add transaction to Clerk summary
				if(pTrans->trtOperator < MAX_CLERK_NO) 
				{
					clerk= pTrans->trtOperator; 
					//tp = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_CLERK_SUMMARY, clerk);
					get_nvram_summary_addr(NVRAM_CLERK_SUMMARY, clerk, tp);
				}
				else 
					continue;
				break;				
			case 4: // add transaction to Hour summary
	        		Hour= bcd2hex(RTCStruct.hour); 
				//tp = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_HOURLY_SUMMARY, Hour);
				get_nvram_summary_addr(NVRAM_HOURLY_SUMMARY, Hour, tp);
	       			break;
			case 5: // add transaction to global summary
//				tp = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_TOTAL_SUMMARY, 0);
//				break;
				continue;
			case 6: // add transaction to global summary
				//tp = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_CUSTOM_SCALE, scale);
				get_nvram_summary_addr(NVRAM_CUSTOM_SCALE, scale, tp);
				break;
			case 7: // add transaction to global summary
				//tp = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_CUSTOM_CLERK, clerk);
				get_nvram_summary_addr(NVRAM_CUSTOM_CLERK, clerk, tp);
				break;
			case 8: // add transaction to global summary
				//tp = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_CUSTOM_HOUR, Hour);
				get_nvram_summary_addr(NVRAM_CUSTOM_HOUR, Hour, tp);
				break;
			case 9: // add transaction to global summary
				//tp = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_DISCOUNT_SCALE, scale);
				get_nvram_summary_addr(NVRAM_DISCOUNT_SCALE, scale, tp);
				break;
			case 10: // add transaction to global summary
				//tp = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_DISCOUNT_CLERK, clerk);
				get_nvram_summary_addr(NVRAM_DISCOUNT_CLERK, clerk, tp);
				break;
			case 11: // add transaction to global summary
#if !(ROM_COUNTRY == ROM_RUS)
// 현재 사용 안함
//				tp = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_DISCOUNT_CREDIT, pTrans->trtCreditCustomerNo);
//				break;
				continue;
#else
				continue;
#endif

			default:  break;
	   	} // of switch(j)
		volume = trtVolume;
//		if(6<=j && j<=8)	// discount
		if(9<=j && j<=11)	// discount
		{
			if(!trtDiscountedVolume)	continue;
			if(j==11 && !pTrans->trtCreditCustomerNo)	break;
			//trtVolume = trtDiscountedVolume;		// sale price -> saved price(origin price - trtVolume) se-hyung 20080131
			volume = trtDiscountedVolume;
		}
//		if(j==9 && !pTrans->trtCreditCustomerNo)	break;
		if((j >= 6 && j <= 8) && !pTrans->trtCreditCustomerNo)  continue;

		for (i = 0; i < MAX_REPORT_PERIOD; i++)	
	   	{
	    		if ((tp->sstVolume[i]+trtVolume) >MAX_SALES_VOLUME)
				++overflow;
			else
			{
				tp->sstTransactions[i] += transaction;
				tp->sstVolume[i] += volume;	// CJK080213
				tp->sstWeight[i] += trtWeight;
				tp->sstQty[i]    += trtQty;
			}
	   	}
		switch(j)
	   	{
			case 0: // add transaction to Dept summary
				set_nvram_summary_addr(NVRAM_DEPT_SUMMARY, pTrans->trtDept, tp);
				break;
			case 1: // add transaction to Scale Total summary
				set_nvram_summary_addr(NVRAM_SCALE_SUMMARY, scale, tp);
				break;
			case 2: // add transaction to Group summary
				set_nvram_summary_addr(NVRAM_GROUP_SUMMARY, pTrans->trtGroup, tp);
				break;
			case 3: //  add transaction to Clerk summary
				if(pTrans->trtOperator < MAX_CLERK_NO)
				{
					clerk= pTrans->trtOperator;
					set_nvram_summary_addr(NVRAM_CLERK_SUMMARY, clerk, tp);
				}
				else
					continue;
				break;
			case 4: // add transaction to Hour summary
				Hour= bcd2hex(RTCStruct.hour);
				set_nvram_summary_addr(NVRAM_HOURLY_SUMMARY, Hour, tp);
				break;
			case 5: // add transaction to global summary
				continue;
			case 6: // add transaction to global summary
				set_nvram_summary_addr(NVRAM_CUSTOM_SCALE, scale, tp);
				break;
			case 7: // add transaction to global summary
				set_nvram_summary_addr(NVRAM_CUSTOM_CLERK, clerk, tp);
				break;
			case 8: // add transaction to global summary
				set_nvram_summary_addr(NVRAM_CUSTOM_HOUR, Hour, tp);
				break;				
			case 9: // add transaction to global summary
				set_nvram_summary_addr(NVRAM_DISCOUNT_SCALE, scale, tp);
				break;
			case 10: // add transaction to global summary
				set_nvram_summary_addr(NVRAM_DISCOUNT_CLERK, clerk, tp);
				break;
			case 11: // add transaction to global summary
				continue;
			default:  break;
	   	} // of switch(j)
	}//for()
	

//	if(pTrans->trtStatus & FLAG_TRANS_NEGATIVE)
//	{
//		//tp = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_NEGATIVE_CLERK, clerk);
//		get_nvram_summary_addr(NVRAM_NEGATIVE_CLERK, clerk, tp);
//		for (i = 0; i < MAX_REPORT_PERIOD; i++)	
//	   	{
//			tp->sstTransactions[i] -= transaction;
//			tp->sstVolume[i] -= trtVolume;
//			tp->sstWeight[i] -= trtWeight;
//			tp->sstQty[i]    -= trtQty;
//	   	}
//		set_nvram_summary_addr(NVRAM_NEGATIVE_CLERK, clerk, tp);
//	}


	if(pTrans->trtStatus&FLAG_TRANS_OVERRIDE)// && pscb->scbSelectedPLU<PLUID_MISC_BY_WEIGHT)
	{
		//sale_summary = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_OVERRIDE_SCALE, scale);
		get_nvram_summary_addr(NVRAM_OVERRIDE_SCALE, scale, sale_summary);
		AddSummaryData(sale_summary,2,trtVolume,trtWeight,trtQty,OFF);
		set_nvram_summary_addr(NVRAM_OVERRIDE_SCALE, scale, sale_summary);
		//sale_summary = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_OVERRIDE_CLERK, clerk);
		get_nvram_summary_addr(NVRAM_OVERRIDE_CLERK, clerk, sale_summary);
		AddSummaryData(sale_summary,2,trtVolume,trtWeight,trtQty,OFF);
		set_nvram_summary_addr(NVRAM_OVERRIDE_CLERK, clerk, sale_summary);
	}							 
	if(pTrans->trtStatus&FLAG_TRANS_PREPACK)
	{
		//sale_summary = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_PREPACK_SCALE, scale);
		get_nvram_summary_addr(NVRAM_PREPACK_SCALE, scale, sale_summary);
		AddSummaryData(sale_summary,2,trtVolume,trtWeight,trtQty,OFF);
		set_nvram_summary_addr(NVRAM_PREPACK_SCALE, scale, sale_summary);
		//sale_summary = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_PREPACK_CLERK, clerk);
		get_nvram_summary_addr(NVRAM_PREPACK_CLERK, clerk, sale_summary);
		AddSummaryData(sale_summary,2,trtVolume,trtWeight,trtQty,OFF);
		set_nvram_summary_addr(NVRAM_PREPACK_CLERK, clerk, sale_summary);
		//sale_summary = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_PREPACK_HOUR, Hour);
		get_nvram_summary_addr(NVRAM_PREPACK_HOUR, Hour, sale_summary);
		AddSummaryData(sale_summary,2,trtVolume,trtWeight,trtQty,OFF);
		set_nvram_summary_addr(NVRAM_PREPACK_HOUR, Hour, sale_summary);
		//sale_summary = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_PREPACK_DEPT, pTrans->trtDept);
		get_nvram_summary_addr(NVRAM_PREPACK_DEPT, pTrans->trtDept, sale_summary);
		AddSummaryData(sale_summary,2,trtVolume,trtWeight,trtQty,OFF);
		set_nvram_summary_addr(NVRAM_PREPACK_DEPT, pTrans->trtDept, sale_summary);
		//sale_summary = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_PREPACK_GROUP, pTrans->trtGroup);
		get_nvram_summary_addr(NVRAM_PREPACK_GROUP, pTrans->trtGroup, sale_summary);
		AddSummaryData(sale_summary,2,trtVolume,trtWeight,trtQty,OFF);
		set_nvram_summary_addr(NVRAM_PREPACK_GROUP, pTrans->trtGroup, sale_summary);
	}
#ifndef USE_DISCOUNTED_TAX_FOR_DISCOUNTED_TOTAL
#if (defined(USE_REPORT_TAX) && defined(USE_TAX_RATE))
	// Tax
	//if (pTrans->trtTaxNumber && trtTax)	// gross amount(tax 포함 금액)를 정산하기 위해 조건 제거함
	{
		INT16S taxcode;
		INT32U tempAddr;
		taxcode= pTrans->trtTaxNumber;
		if (taxcode == 0 || taxcode > MAX_TAX_NO)	taxcode = MAX_TAX_NO;	// tax id가 0인경우 gross amount를 10번째 방에 누적한다.

		for (j=0;j<MAX_REPORT_PERIOD;j++)
		{
			tempAddr = (scale*MAX_TAX_NO*MAX_REPORT_PERIOD*4)+((taxcode-1)*MAX_REPORT_PERIOD*4+j*4);
			total_tax = get_nvram_lparam(tempAddr+NVRAM_TAX);	// Modified by CJK 20050903
			total_tax += trtTax;
			set_nvram_lparam(tempAddr+NVRAM_TAX, total_tax);
			total_tax = get_nvram_lparam(tempAddr+NVRAM_TAX_GROSS);	// TAX Gross Amount
			total_tax += trtVolume;
			set_nvram_lparam(tempAddr+NVRAM_TAX_GROSS, total_tax);
		}
	}
#endif
#endif
	// add transaction to PLU summary
	if(!GlbOper.PluReportType)
	{
		tp1= FindPluSummary(1,pTrans->trtPLU,pTrans->trtDept,pTrans->trtPLUtype,&index);
		if(tp1!=NULL)	
		{
			tp1->pstCountNumber++;
			for (i=0; i< MAX_REPORT_PERIOD;i++)	
			{
		    		if ((MAX_SALES_VOLUME-pTrans->trtVolume) < tp1->pstVolume[i])
					++overflow;
				else
				{
					tp1->pstTransactions[i] += transaction; 	
					tp1->pstVolume[i] += trtVolume;
					if(pTrans->trtPLUtype == PLU_BY_WEIGHT)
						tp1->pstWeight[i] += trtWeight;
					else
						tp1->pstQty[i] += trtQty;
#if !(ROM_COUNTRY == ROM_RUS)
#ifdef USE_TAX_RATE                    
					tp1->pstTax[i] += trtTax;
#endif                    
#endif
				}

#if (!(ROM_COUNTRY == ROM_RUS) && !defined(USE_INDIA_FUNCTION))
		   		if(pTrans->trtStatus&FLAG_TRANS_LABEL) 
				{
					//plu_summary = (PLU_SALES_SUMMARY_TYPE *)get_nvram_plu_summary(NVRAM_PLU_SUMMARY, index);
					tp1->pstCountOfLabel[i]++;
					tp1->pstCountOfLabelVolume[i] += trtVolume;
				}
#endif
				if (pTrans->trtStatus&FLAG_TRANS_PREPACK)
				{
					//plu_summary = (PLU_SALES_SUMMARY_TYPE *)get_nvram_plu_summary(NVRAM_PLU_SUMMARY, index);
					if(tp1->pstPLUtype == PLU_BY_WEIGHT)
						tp1->pstPrepackWeight[i] += trtWeight;
					tp1->pstPrepackVolume[i] += trtVolume;
				}
			} // of for
			set_nvram_plu_summary(NVRAM_PLU_SUMMARY, index, tp1);
	  	} // of if(tp!=NULLP)
	}
	
	if(pTrans->trtStatus & FLAG_TRANS_NEGATIVE) //memory 수명관계로 위치 이동.
	{
		//tp = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_NEGATIVE_CLERK, clerk);
		get_nvram_summary_addr(NVRAM_NEGATIVE_CLERK, clerk, tp);
		for (i = 0; i < MAX_REPORT_PERIOD; i++)	
	   	{
			tp->sstTransactions[i] -= transaction;
			tp->sstVolume[i] -= trtVolume;
			tp->sstWeight[i] -= trtWeight;
			tp->sstQty[i]    -= trtQty;
	   	}
		set_nvram_summary_addr(NVRAM_NEGATIVE_CLERK, clerk, tp);
	}
	
	if (overflow)
	{
		caption_split_by_code(0xe201,&cap,0);//"Check sales summary overflow"
		DisplayMsg(cap.form);
	}
	return 0;
} // of INT16S AddLocalTransactionToSummaries(...)

/*
#ifdef USE_TICKETCLOSE_BEFORE_ZREPORT 
extern INT16S Prt_CalcTotalPrice(INT8U addr_mode, INT8U ClerkNum, INT8U ret_chk);
extern INT8U ClerkTransactionSaleStore(INT8U clerkNum);
extern void	ClearPayment(INT8U trtType);
void CheckClerkTransaction_Print(void)
{
	INT8U ClerkNum;
	INT16U	waitingTime;
	INT8U	result;
	INT8U	ret_value=0;
	INT16U	bufdata_count=0;
	INT8U	totalDisplayTime, i;
	INT8U	prt_total_conti;
	INT8U	escape_prt = 0;
#ifdef USE_DISCOUNTED_TAX_FOR_DISCOUNTED_TOTAL
	float discountRate;
#endif

	for(ClerkNum=0;ClerkNum<MAX_CLERK_NO;ClerkNum++)
	{
		prt_total_conti = get_global_bparam(GLOBAL_PRINT_OPER_FLAG2) & 0x20;	//1872 Prt total label conti
		totalDisplayTime = get_global_bparam(GLOBAL_SALE_SETUP_FLAG12);
		totalDisplayTime = totalDisplayTime & 0x0f;
		totalDisplayTime *= 10; 	//key delay sec 는 systimer100ms 사용 함. 
		ClerkTotalPrice(0, ClerkNum, &TotalData);
		TotalData.SummaryTotalPrice = TotalData.TotalPrice;
		TotalData.SummarySavedPrice = 0;
		TotalData.SummaryRound = transPay.summaryround;
		TotalData.PayPrice = 0;
					
		waitingTime = SysTimer100ms;
	
		//result = PrintTotal(lastClerk, discountPer,discountPrice);
		transPay.trtType = TRANS_TYPE_PAY_TOTAL_NOPAY;// 일반적 total은 cash지불로 인정X
		transPay.scaleid = status_scale.scaleid;
		transPay.clerkid = ClerkNum;
		transPay.paidprice[PAY_CASH] = TotalData.TotalPrice;			
		transPay.summarydiscount = 0;	//Added by JJANG 20080529
#ifdef USE_DISCOUNTED_TAX_FOR_DISCOUNTED_TOTAL
		if (TotalData.TotalPrice == 0) discountRate = 1;
		else
		{
			discountRate = (float)TotalData.SummaryTotalPrice / (float)TotalData.TotalPrice;
		}
		for (i = 0; i < MAX_TAX_NO; i++)
		{
			if (TotalData.taxPrice[i] > 0)
				TotalData.taxPrice[i] = (long)(((float)TotalData.taxPrice[i] * discountRate + 0.5));
			else
				TotalData.taxPrice[i] = (long)(((float)TotalData.taxPrice[i] * discountRate - 0.5));
			if (TotalData.taxGross[i] > 0)
				TotalData.taxGross[i] = (long)(((float)TotalData.taxGross[i] * discountRate + 0.5));
			else
				TotalData.taxGross[i] = (long)(((float)TotalData.taxGross[i] * discountRate - 0.5));
			transPay.taxPrice[i] = TotalData.taxPrice[i];
			transPay.taxGross[i] = TotalData.taxGross[i];
		}
		if (TotalData.InTaxPrice > 0)
			TotalData.InTaxPrice = (long)(((float)TotalData.InTaxPrice * discountRate + 0.5));
		else
			TotalData.InTaxPrice = (long)(((float)TotalData.InTaxPrice * discountRate - 0.5));
		if (TotalData.ExTaxPrice > 0)
			TotalData.ExTaxPrice= (long)(((float)TotalData.ExTaxPrice * discountRate + 0.5));
		else
			TotalData.ExTaxPrice= (long)(((float)TotalData.ExTaxPrice * discountRate - 0.5));
#endif
		if(ethernet_list.status==4) //Modified by JJANG 20080313
		{
				bufdata_count = GetClerkTransactionCount(ClerkNum);
			ret_value = GetCheckAddTransactionData(0,bufdata_count,AUDIT_MODE_LINEAR);		//mode : sale
		}
		else ret_value = 1;
		
		if(ret_value)
		{
			{
				result = Prt_PrintStart(PRT_TOTAL_NORMAL_MODE, PrtStruct.Mode, ClerkNum, ON, 0, 0);
			}
		
			//Added by JJANG 20080131 프린트 출력 오류났을 때 에러메시지 출력 (print error, YES/NO?) 
			if(result == 2) //paper end err
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
			if(result == 1) 
			{
				ClerkGrandTotalSum(ClerkNum, &TotalData);// , 0);	//CJK20040909
				CheckGrandTotalStatus();
				LocalPayTotalSummaries(transPay,0,OFF);
				ClerkTransactionSaleStore(ClerkNum);
				//Slave와 Master가 다시 연결될 때까지 기다렸다가 DATA전송
				set_nvram_bparam(NVRAM_SLAVECHECK_FLAG, 1); 
				while(!network_ClerkDeleteTransaction(ClerkNum));
				set_nvram_bparam(NVRAM_SLAVECHECK_FLAG, 0); 
				ReopenedTransPay = transPay;
				ClerkDeleteTransaction(ClerkNum);
				memset(&ReopenedTransPay, 0, sizeof(TRANSACTION_PAY));
				TareOper.addupWeight = 0;
			}
		}
	}
}
#endif
*/

char ClerkClearTransaction(void) 
{
	INT16S i;
	TRANSACT_CHAIN *tc;
	TRANSACT_CHAIN tcs;
	PREPACK_STATUS_TYPE PrepackStatus;

	////ClerkTotalTransaction = 0; 
//	set_nvram_bparam(NVRAM_CLERK_TOTALTRANSACTION, 0);  //미사용

	nvram_set(NVRAM_TRANS_DATA, 0, MAX_TRANSACTION_SALE_SIZE*MAX_ADD_TRANSACTION); // 72*399

//	set_nvram_bparam(NVRAM_LASTCLERKNUMBER, 0);  //미사용

	nvram_set(NVRAM_TRANS_HEAD, 0, (4+4+2+4+2)*MAX_CLERK_NO); //16*100

	// cache를 통한 flash write 속도를 위해 위의 for문과 분리
	nvram_set(NVRAM_CLERK_CUSTOMER_NO, 0, 2*MAX_CLERK_NO); // 2*100

	set_nvram_lparam(NVRAM_GTOTAL_NET_WEIGHT, 0);      
	set_nvram_lparam(NVRAM_GTOTAL_PER_TARE_WEIGHT, 0);      
	set_nvram_lparam(NVRAM_GTOTAL_GROSS_WEIGHT, 0);      
	set_nvram_lparam(NVRAM_SUBTOTAL_NET_WEIGHT, 0);      
	set_nvram_lparam(NVRAM_SUBTOTAL_PER_TARE_WEIGHT, 0);      
	set_nvram_lparam(NVRAM_SUBTOTAL_GROSS_WEIGHT, 0);      

	//get_PrepackStatus(&PrepackStatus);
	memset(&PrepackStatus, 0, sizeof(PREPACK_STATUS_TYPE));
	set_PrepackStatus(&PrepackStatus);

	//memset((void *)(NVRAM_CLEARK_LASTSALE+NVRAM_BASE), 0, MAX_TRANSACTION_SALE_SIZE*MAX_REOPEN_SALE*MAX_REOPEN_HEAD);
	nvram_set(NVRAM_CLEARK_LASTSALE, 0, MAX_TRANSACTION_SALE_SIZE*MAX_REOPEN_SALE*MAX_REOPEN_HEAD);

	return 0;
} // of char InitClerkQueue(void)

INT16S	ClerkSearchEmpty(void)
{
	INT16S i;
	TRANSACT_CHAIN *tc;
	TRANSACT_CHAIN tcs;

	for(i=0;i<MAX_ADD_TRANSACTION;i++)
	{
		//tc = (TRANSACT_CHAIN *)get_nvram_trans_addr(NVRAM_TRANS_DATA, i);
		get_nvram_trans_addr(NVRAM_TRANS_DATA, i, &tcs);
		tc = &tcs;
		if(!tc->flag)
			return i + 1;//empty index. 0을 사용하지 않기 위해 1 증가
	}
	return -1;
}

INT16S	ClerkSearchEmptyCount(void)
{
	INT16S i;
	INT16S count;
	TRANSACT_CHAIN tcs;

	count = 0;
	for(i=0;i<MAX_ADD_TRANSACTION;i++)
	{
		//tc = (TRANSACT_CHAIN *)get_nvram_trans_addr(NVRAM_TRANS_DATA, i);
		get_nvram_trans_addr(NVRAM_TRANS_DATA, i, &tcs);
		if(!tcs.flag) count++;
	}
	return count;
}

INT16S	ClerkAddData(TRANSACT_CHAIN* cp,TRANSACTION_RECORD_TYPE* trans)
{
//	INT16S firstFlag=ON;

	cp->content = *trans;// copy data
	cp->flag=ON;
	cp->delete=OFF;
	return ON;
}

//#define MAX_CLERK_TRANSACTION 49
extern NETWORK_CLERK_SALE_AMOUNT ClerkSaleAmount;

// return value = 0:ok, 0x11:exceed clerk no, 0x12:memory full, 0x13:same transaction 
// network 1 : recieve, reopen
//	   0 : new data, master only
INT16S ClerkAddTransaction(INT8U ClerkNum,TRANSACTION_RECORD_TYPE  *CurrentTrans, INT8U network, INT32U TicketNo)
{ 
	TRANSACT_CHAIN  *cp;
	TRANSACT_CHAIN  tc;
	INT16U h_index, next;
	INT32U  head_addr;// ,dmax,dmin;
	INT16S	index;
	INT8U   serialno;
	INT8U clerk_cnt;
	INT16U	max_trans;

  	if (ClerkNum >= MAX_CLERK_NO ) { 
		return 0x11;
  	}

	if (!network && Operation.operationClerk == OPER_FLOATING && ethernet_list.status == 2)	// slave일 때 저장하지 않음	
	{
		CurrentTrans->trtSerialNo = ClerkSaleAmount.totalCount + ClerkSaleAmount.voidCount + 1;
		return 0;
	}
	index = ClerkSearchEmpty();
	if (index == -1) {
		caption_split_by_code(0xC90D,&cap,0);
		DisplayMsg(cap.form);
		return 0x12;
	}

	if (ClerkNum) max_trans = MAX_CLERK_TRANSACTION;
	else max_trans = MAX_ADD_TRANSACTION;
#ifdef USE_QR_ADDTOTAL
//	if (QrAddTotalFlag) max_trans = MAX_QR_ADDCOUNT - 1;
	if (QrAddTotalFlag) max_trans = MAX_QR_ADDCOUNT;
#endif

	//cp = (TRANSACT_CHAIN *)get_nvram_lparam(NVRAM_TRANS_HEAD+ClerkNum*4);
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
	if (!cp)
	{
		//head_addr = get_nvram_trans_addr(NVRAM_TRANS_DATA, index);
		//cp = (TRANSACT_CHAIN *)head_addr;
		set_nvram_lparam(NVRAM_TRANS_HEAD+ClerkNum*4, index);
		next = index;
		get_nvram_trans_addr(NVRAM_TRANS_DATA, next - 1, &tc);
		cp = &tc;

		if (TicketNo) set_TicketTransactionNumber(ClerkNum, TicketNo);	//reopen시 동일 ticket number를 주기위해
		else
		{
			set_TicketTransactionNumber(ClerkNum, GetTicketCounter()); // floating clerk에서는 Master로부터 부여받츰
			IncreaseTicketCounter();
		}
		serialno=0;
	}
	else
	{
		serialno=0;
		clerk_cnt=0;
		for(;;)
		{
			if (next == cp->next) cp->next=0;
			if (cp->next)
			{
				//cp=cp->next;
				next = cp->next;
				get_nvram_trans_addr(NVRAM_TRANS_DATA, next - 1, &tc);
				cp = &tc;
				if (serialno<cp->content.trtSerialNo) serialno=cp->content.trtSerialNo;
				
				//20090908, Clerk별 최대 판매개수 50개로 제한
				clerk_cnt++;
				if (clerk_cnt + 1 >= max_trans)
				{
					if (!network)
					{
#ifdef USE_QR_ADDTOTAL
						if(QrAddTotalFlag)
						{
							caption_split_by_code(0xC90D,&cap,0);
							DisplayMsg(cap.form);
//							if(PrtStruct.Mode == LABEL)     AddSaleFlag = 0;
							BuzOn(2);
							return 0x16;
						}
#endif
						caption_split_by_code(0xCA06,&cap,0);	//MAX TRNASACTION IS 50!
						DisplayMsg(cap.form);
					}					
					return 0x16;
				}
			}
			else
			{
				if (network && cp->content.trtSerialNo == CurrentTrans->trtSerialNo) 
				{
					return 0x14;	// 마지막 데이터가 새로 추가되는 것과 동일한 no일 때(다른 scale로 부터 중복)
				}
				//cp->next = (TRANSACT_CHAIN *)get_nvram_trans_addr(NVRAM_TRANS_DATA, index);
				cp->next = index;
				set_nvram_trans_addr(NVRAM_TRANS_DATA, next - 1, cp);
				if (serialno<cp->content.trtSerialNo) serialno=cp->content.trtSerialNo;
				next = index;
				get_nvram_trans_addr(NVRAM_TRANS_DATA, next - 1, &tc);
				cp = &tc;
				break;
			}
		}
	}

	CurrentTrans->trtTicketNumber = get_TicketTransactionNumber(CurrentTrans->trtOperator);

	if (!network)
	{
		serialno ++;
		CurrentTrans->trtSerialNo=serialno;
	}
	cp->content = *CurrentTrans;// copy data
	cp->flag=ON;
	if (CurrentTrans->trtStatus&FLAG_TRANS_VOID) cp->delete=ON;
	else cp->delete=OFF;
	set_nvram_trans_addr(NVRAM_TRANS_DATA, next - 1, &tc);

  	return 0;
}

INT16U ClerkTransactionCnt(INT16U ClerkNum)
{
	TRANSACT_CHAIN  tc;
	INT16U h_index, next;
	INT16U clerk_cnt = 0;
	INT16U i;

	h_index = get_nvram_lparam(NVRAM_TRANS_HEAD+ClerkNum*4);
	if (h_index) {
		clerk_cnt++;
		next = h_index;
		get_nvram_trans_addr(NVRAM_TRANS_DATA, h_index-1, &tc);
	}
	else
		return 0;

	for (i = 0; i <= MAX_ADD_TRANSACTION; i++)
	{
		if (next == tc.next) tc.next=0;
		if (tc.next) {
			next = tc.next;
			get_nvram_trans_addr(NVRAM_TRANS_DATA, next-1, &tc);
			clerk_cnt++;
		}
		else
			return clerk_cnt;
	}
}
#if !defined(_USE_LCD20848_) && !defined(_USE_LCD32_)
extern void sale_display_add_msg(INT8U transaction, INT32S price);
#endif
void DisplayClerkTotalInfo(INT8U ClerkNum)     
{
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
	STRUCT_TOTAL_PRICE totalData;
	char priceBuffer1[40];
	char priceBuffer2[40];
	char priceBuffer3[40];
	char w_sign1[5];//, w_sign2[5];
	INT8U	ret;
	INT8U	dec_ch;
	INT8U	totalinfo_disp = 0;
	INT8U	v8_1;//, v8_2;
//	long	tWeight=0;
	INT8U	use_unit;


  	ret = get_global_bparam(GLOBAL_SALE_SETUP_FLAG4)&0x10;

	dec_ch  = get_global_bparam(GLOBAL_DECIMAL_SIGN);
	totalinfo_disp = get_global_bparam(GLOBAL_SALE_SETUP_FLAG7)& 0x10;
	if (ethernet_list.status == 2)
	{
		totalinfo_disp = 0;	// total weight 정보를 master로 부터 받을 수 없으므로 무게 정보 디스플레이 안함(필요시 수정)
	}

	if(ret)
	{
		if (ClerkNum && !status_scale.cur_pluid)
		{
			Clerk_Subtotal_List = 1;
			sale_display_update(UPDATE_SUBTOTAL);
			NetClerkLock(ClerkNum);
		}
	}
	else
	{
		if (Operation.operationClerk == OPER_FLOATING && ethernet_list.status == 2)
		{
			totalData.TotalCount = ClerkSaleAmount.totalCount;
			totalData.VoidCount  = ClerkSaleAmount.voidCount;
			totalData.TotalPrice = ClerkSaleAmount.totalPrice;
		}
		else
		{
			ClerkTotalPrice(0, ClerkNum, &totalData);
		}

		if (ClerkNum && !status_scale.cur_pluid)
		{
			FloatToString(TO_STR,priceBuffer1,8,DISPLAY_PRICE,&totalData.TotalPrice,0,1);
			GetPriceString(priceBuffer2,priceBuffer1,ON,PRICE_DISPLAY);

			//Added by JJANG 20080327 , parameter 568 : Y  무게정보 화면에 디스플레이
			if(totalinfo_disp)
			{
				use_unit = ADM_Status.UseUnit;
				if(use_unit == WEIGHT_UNIT_KG) {
					if(get_base_bparam(FLASH_ADC_WUNIT) == WEIGHT_UNIT_G) use_unit = WEIGHT_UNIT_G;
				}
				v8_1=get_decimal_pt(3, use_unit);	// UseUnit 사용
				GetWeightSymbol(use_unit, w_sign1);
				format_value((HUGEDATA INT8U *)priceBuffer3, totalData.TotalWeight, v8_1, dec_ch);
				strcat(priceBuffer3, w_sign1);
				//PrtGenWeightStr(priceBuffer3, totalData.TotalWeight, 5, w_sign1, w_sign2, v8_1, v8_2);
				// Ex)  #1, 27, $ 234.44, 55.000kg
				sprintf(gmsg,"#%d, %d, %s, %s", ClerkNum, totalData.TotalCount+totalData.VoidCount,priceBuffer2, priceBuffer3);
				DisplayWeightMsg(gmsg);
			}
			else
			{
				caption_split_by_code(0xc022,&cap,0);//"Total"
				sprintf(gmsg,"[%s] %d, %s", cap.form, totalData.TotalCount+totalData.VoidCount,priceBuffer2);
				DisplayMsg(gmsg);
			}
		}
	}
#else
	if (ClerkNum && !status_scale.cur_pluid) 
	{
		sale_display_add_msg(OFF, 0);
		VFD7_Diffuse();
		Msg7SegDispTime = SysTimer100ms;
	}
#endif
}

void DisplayClerkAddTransaction(INT8U ClerkNum, INT32S price)	// Clerk 판매 정보 디스플레이
{
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
	STRUCT_TOTAL_PRICE totalData;

	INT8U msg_size;

	msg_size=get_global_bparam(GLOBAL_SALE_SETUP_FLAG8);
	msg_size=(msg_size>>3)&0x01;


	if (Operation.operationClerk == OPER_FLOATING && ethernet_list.status == 2)
	{
		totalData.TotalCount = ClerkSaleAmount.totalCount + 1;
		totalData.VoidCount  = ClerkSaleAmount.voidCount;
	}
	else
	{
		ClerkTotalPrice(0, ClerkNum, &totalData);
	}

	if (ClerkNum)
	{
		sprintf(gmsg,global_message[MSG_TRANSINFO], ClerkNum, totalData.TotalCount+totalData.VoidCount);
		if(msg_size) DisplayMsgTransaction(gmsg);	//Modified by JJANG 20080220 PARAMETER 613
		else DisplayMsgShort(gmsg);
	}
	else
	{
	}
#else
	sale_display_add_msg(ON, price);
	VFD7_Diffuse();
	Msg7SegDispTime = SysTimer100ms;
#endif
}

#if defined(USE_INDIA_FUNCTION) || defined(USE_SEND_TICKET_DATA) || defined(USE_SETPRODUCT_SALE) || defined(USE_HYUNDAI_SET_FUNCTION)
void StoreAdditionalData(TR_ADDITION_DATA_TYPE *pTrans_add, INT8U ClerkNum)
{
	TRANS_ADD_CHAIN  *AddChain;
	TRANS_ADD_CHAIN  TransAddChain;
	INT32U  head_addr;// ,dmax,dmin;
	INT16S	index;
	INT8U   serialno;
	INT8U clerk_cnt;
	INT16U	max_trans;
	INT16U h_index;

  	if (ClerkNum >= MAX_CLERK_NO ) { 
		return;
  	}
	index = ClerkSearchEmpty()-1;//Transaction_chain structure에 저장되는 transaction들과 저장 순서 일치.
	if (index == -1) {
		caption_split_by_code(0xC90D,&cap,0);
		DisplayMsg(cap.form);
		return;
	}

	if (ClerkNum) max_trans = MAX_CLERK_TRANSACTION;
	else max_trans = MAX_ADD_TRANSACTION;

	h_index = get_nvram_lparam(NVRAM_TRANS_ADD_HEAD+ClerkNum*4);
	if (h_index)
	{
		get_nvram_transAdd_addr(NVRAM_TRANS_ADD, h_index-1, &TransAddChain);
		AddChain = &TransAddChain;
	}
	else {
		AddChain = 0;
	}
	if (!AddChain)
	{
		get_nvram_transAdd_addr(NVRAM_TRANS_ADD, index-1, &TransAddChain);
		AddChain = &TransAddChain;
		set_nvram_lparam(NVRAM_TRANS_ADD_HEAD+ClerkNum*4, index);
		h_index = index;
	}
	else
	{
		for(;;)
		{
			if (h_index == AddChain->next) AddChain->next=0;
			if (AddChain->next)
			{
				h_index = AddChain->next;
				get_nvram_transAdd_addr(NVRAM_TRANS_ADD, h_index-1, &TransAddChain);
				AddChain = &TransAddChain;

				clerk_cnt++;
				if (clerk_cnt >= max_trans) break;
			}
			else
			{
				AddChain->next = index;
				set_nvram_transAdd_addr(NVRAM_TRANS_ADD, h_index-1, &TransAddChain);
				h_index = index;
				get_nvram_transAdd_addr(NVRAM_TRANS_ADD, h_index - 1, &TransAddChain);
				AddChain = &TransAddChain;
				break;
			}
		}
	}
	
	AddChain->addtion_data = *pTrans_add;// copy data
	set_nvram_transAdd_addr(NVRAM_TRANS_ADD, h_index-1, &TransAddChain);

  	return;
}
extern INT8U set_sale_started;
void DeleteAdditionalData(INT8U ClerkNum)
{
		TRANS_ADD_CHAIN	*AddChain;
		TRANS_ADD_CHAIN	TransAddChain;
		INT16U h_index, next, current;
		INT16S i=0;
		
#ifdef USE_SETPRODUCT_SALE
		set_sale_started = OFF;
#endif
		if(MAX_CLERK_NO-1 < ClerkNum)	return;
	
		h_index = get_nvram_lparam(NVRAM_TRANS_ADD_HEAD+ClerkNum*4);
		if (h_index)
		{
			current = h_index;
			get_nvram_transAdd_addr(NVRAM_TRANS_ADD, h_index-1, &TransAddChain);
			AddChain = &TransAddChain;
		}
		else {
			AddChain = 0;
		}
		if(!AddChain) return;

		for(;AddChain;) {
			if(i<MAX_LAST_TRANSACTION && ClerkNum) {
				i++;
			} else {
				//DisplayMsg("Over clerk back data");
			}
			next = AddChain->next;
			AddChain->next = NULL;
			set_nvram_transAdd_addr(NVRAM_TRANS_ADD, current-1, &TransAddChain);
			if (!next) break;
			get_nvram_transAdd_addr(NVRAM_TRANS_ADD, next-1, &TransAddChain);
			AddChain = &TransAddChain;
			current = next;
		}
		set_nvram_lparam(NVRAM_TRANS_ADD_HEAD+ClerkNum*4, 0 );

		return;
}

void ClearAdditionalData(void) 
{   
	INT16S i;
	TRANS_ADD_CHAIN *AddChain;

	nvram_set(NVRAM_TRANS_ADD, 0, MAX_TRANSACTION_ADD_SIZE*MAX_ADD_TRANSACTION);

	for(i=0; i<MAX_CLERK_NO; i++) {
		set_nvram_lparam(NVRAM_TRANS_ADD_HEAD+i*4, NULL);
	}
#if !defined(USE_SETPRODUCT_SALE) && !defined(USE_HYUNDAI_SET_FUNCTION)
    set_nvram_bparam(NVRAM_QR_ADD_DATA, 0x00);
	set_nvram_bparam(NVRAM_QR_ADD_COUNT, 0);
// 실제 사용하는 바코드 폼의 길이에 따라 Add Total Count를 계산 
// NVRAM 영역, Floating Clerk Data Sync 등 추후 다시 적용 방안 검토 필요 
//	QR_addedMemUsage[status_scale.clerkid] = 0;
#endif
	return;
} // of char InitClerkQueue(void)
#endif


INT16S	LocalClerkSummaries(INT8U scaleNum,INT8U operator,TRANSACTION_RECORD_TYPE* pTrans,TRANSACTION_RECORD_SUB_TYPE* pTrans_sub,TR_ADDITION_DATA_TYPE* pTrans_add)
{
	INT16S stat;

	stat = 0;
	if(pTrans->trtStatus&FLAG_TRANS_ADD) {	//ADD or FLOATING	// stat=0:ok, 1:exceed clerk no, 2:memory full 
		stat = ClerkAddTransaction(operator,pTrans,0,0);
	}
	if(stat == 0) {
		stat = LocalPluTotalSummaries(scaleNum,pTrans,pTrans_sub,pTrans_add,0,0);	// stat=0:ok, 1:timeout, 2:memory full
		if (stat == 0 && pTrans->trtStatus&FLAG_TRANS_ADD)	//ok
		{
			DisplayClerkAddTransaction(operator, pTrans->trtVolume);	// Added by CJK 20061129
		}
	}
	if (stat == 0) return 1;
	return 0;
}

extern PRICE_TYPE LinkedPrice[MAX_LINKED_PLU];
extern INT8U Send_reason_Code;
#ifdef USE_HYUNDAI_MEAT_FUNCTION
extern INT8U generate_barcode_string(INT8U deptno, long pluno, INT8U plutype);
#endif
INT16S	SaleAcctLocalAddTransaction(INT8U dept_id,INT32U plu_id,PRICE_TYPE *pPrice,INT8U trtStatus)
{
	TRANSACTION_RECORD_TYPE  currentTrans;
	TRANSACTION_RECORD_SUB_TYPE  currentTrans_sub;
	TR_ADDITION_DATA_TYPE currentTrans_add;
	UNITPRICE_CHANGE_TYPE	chgprice;
	INT8U trtType,pluType;
	INT8U	operator=status_scale.clerkid;
	INT8U	scaleNum=status_scale.scaleid;
	INT16S	index,stat;
	INT8U	trtStatus2;
	//INT8U   chg_flag;
	INT8U	changeFlag;
	INT8U   ret_value;

	//chg_flag = get_global_bparam(GLOBAL_SALE_SETUP_FLAG8) & 0x40;	//parameter 723

	if(Operation.operationClerk == OPER_NORMAL)	operator = 0;

	trtType = TRANS_TYPE_NORMAL;
	if(Operation.operationMode == MODE_REPACK)	trtType = TRANS_TYPE_REPACK;
	if(Operation.operationMode == MODE_SELF)	trtType = TRANS_TYPE_SELF;
#ifdef USE_EMART_BACK_LABEL
	if(Operation.BackLabeledFlag)
	{
		trtType |= TRANS_TYPE_BACK_LABEL_SALE;
	}
	else
	{
		trtType &= ~TRANS_TYPE_BACK_LABEL_SALE;
	}
#endif
#ifdef USE_SETPRODUCT_SALE
	if(SetproductSaleMode)
	{
		trtType |= TRANS_TYPE_SETPRODUCT;
	}
	else
	{
		trtType &= ~TRANS_TYPE_SETPRODUCT;
	}

#endif
#ifdef USE_NHMART_SAFE_MEAT
	if(Operation.OfflineMode)
	{
		trtType |= TRANS_TYPE_OFFLINE;
	}
	else
	{
		trtType &= ~TRANS_TYPE_OFFLINE;
	}
#endif
#ifdef USE_SEND_TICKET_DATA
	trtType |= 0x20;
#endif
#ifdef USE_HYUNDAI_MEAT_FUNCTION
	if(Operation.meatStorageLabelFlag && !Operation.RegistDisposalFlag){ // 보관 라벨을 발행하는 경우에는 대면 제작 세트 판매 중이더라도, 보관 라벨 타입으로 전송
		trtType = TRANS_TYPE_MEAT_STORAGE_LABEL;
	}
	else if(MeatSetSaleMode) { 
		trtType = TRNAS_TYPE_SETPRODUCT; //대면 제작 세트 판매 중에는 세트판매 타입으로 전송
	}
	else trtType = TRNAS_TYPE_NORMAL;
#endif

	if(Operation.transAddup)		trtStatus |= FLAG_TRANS_ADD;
	if(Operation.wPrepack)			trtStatus |= FLAG_TRANS_PREPACK;
	if(Operation.printON)			trtStatus |= FLAG_TRANS_LABEL;
	if(Operation.negative)			trtStatus |= FLAG_TRANS_NEGATIVE;
	if(Operation.override && plu_id<MISC_PLU_WEIGHT && Price.UnitPrice[PRICE_ORIGIN] != status_scale.Plu.unitprice)
		trtStatus |= FLAG_TRANS_OVERRIDE;

	trtStatus2 = 0;
	if (ethernet_list.status == 4)	trtStatus2 |= FLAG2_TRANS_LOCAL;	// CJK080320
#ifdef USE_SETPRODUCT_SALE
	if(SetproductSaleMode && setproduct_PrtFlag)
		trtStatus |= FLAG_TRANS_NOVOID; //농협은
	else
		trtStatus &= ~FLAG_TRANS_NOVOID; //
#endif

	pluType = PluType();
	InsertAdditionalData(&currentTrans_add);
	InsertTransactionRecord(trtType,trtStatus,trtStatus2,operator,dept_id,plu_id,pluType,status_scale.Plu.group,pPrice,&currentTrans,&currentTrans_sub,0);
#ifdef USE_HYUNDAI_MEAT_FUNCTION
	generate_barcode_string(dept_id, plu_id, pluType);
	InsertBarcodetoTransactionRecord(&currentTrans);
#endif
	if (!status_scale.clerkid && pluType == PLU_BY_WEIGHT) InsertAddWeightData(pPrice);
	changeFlag = GetChangeFlag(PluType(), status_scale.Plu.ptype, Price.UnitPrice[PRICE_RESULT], status_scale.Plu.unitprice, Operation.fixedPrice, status_scale.Plu.fixed_price, Price.TotalPrice[PRICE_SAVED]);
#ifdef USE_SETPRODUCT_SALE
	if(SetproductSaleMode) changeFlag = FLAG3_TRANS_NONE;
#endif
	if (UseEmartNetwork && changeFlag)
	{
		ret_value = GetCheckAddTransactionData(0,1,AUDIT_MODE_CIRCULAR);		//mode : sale, count : 1, Circular
		if(ret_value)
		{
			InsertUnitPriceChangeRecord(status_scale.scaleid,status_scale.departmentid,status_scale.cur_pluid,
					status_scale.Plu.unitprice,Price.UnitPrice[PRICE_RESULT],Send_reason_Code,status_scale.clerkid,changeFlag,&chgprice);

			transaction_sale_store(&currentTrans, &currentTrans_sub, &currentTrans_add, &chgprice, 0, 1);
		}
	}

	stat = LocalClerkSummaries(scaleNum,operator,&currentTrans,&currentTrans_sub,&currentTrans_add);
#if defined(USE_INDIA_FUNCTION) || defined(USE_SEND_TICKET_DATA)
	if (!RealtimeAddDataFlag)
	{
	if(currentTrans.trtStatus&FLAG_TRANS_ADD && stat == 1)
		StoreAdditionalData(&currentTrans_add,operator);
	}
#endif
#ifdef USE_SETPRODUCT_SALE
	if(SetproductSaleMode)
	{
		stat = ClerkAddTransaction(operator,&currentTrans,0,0);
		if(!stat) stat = ON;
		StoreAdditionalData(&currentTrans_add,status_scale.clerkid);
	}
#endif
#ifndef USE_PLU_MULTIPLE_PRICE	//Refefence PLU 기능 사용 안 함
	for(index=0;stat && index<MAX_LINKED_PLU;index++)
	{
		if(status_scale.linked_dept[index] && status_scale.linked_plu[index] && LinkedPrice[index].TotalPrice[PRICE_RESULT])
		{
			pluType = status_scale.LinkedPlu[index].ptype;
			InsertTransactionRecord(trtType,trtStatus,trtStatus2,operator,status_scale.linked_dept[index],status_scale.linked_plu[index],pluType,status_scale.LinkedPlu[index].group,&LinkedPrice[index],&currentTrans,NULL,0);
			LocalClerkSummaries(scaleNum,operator,&currentTrans,&currentTrans_sub,&currentTrans_add);
		}
	}
#endif
#ifdef USE_HYUNDAI_SET_FUNCTION
	if(MeatSetSaleMode)
	{
		ClerkAddTransaction(operator,&currentTrans,0,0);
		//StoreAdditionalData(&currentTrans_add,status_scale.clerkid);
	}
#endif
	if (stat == ON) IncreaseTransactionCounter();

	return stat;
}

INT32U	ClerkLastHead=0;

extern TRANSACT_CHAIN  TrChain;
INT16S ClerkTotalPrice(INT8U mode, unsigned char ClerkNum, STRUCT_TOTAL_PRICE *totalData)
{ 
	TRANSACT_CHAIN  *cp;
	TRANSACT_CHAIN  tc;
	INT16U h_index, next;
	INT8U lastSerialNo;
	long weight;
#ifdef USE_TAX_RATE
    INT8U taxNum;
#endif
	lastSerialNo=0;
        memset(totalData, 0, sizeof(STRUCT_TOTAL_PRICE));
 
	if ((mode == 0) || (mode==2)) 
	{
		//cp = (TRANSACT_CHAIN *)get_nvram_lparam(NVRAM_TRANS_HEAD+ClerkNum*4);
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
		//cp = (TRANSACT_CHAIN *)(ClerkLastHead + (NVRAM_CLEARK_LASTSALE_PNT));
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
	else if (mode ==3) //SG070214
	{
		cp = (TRANSACT_CHAIN *)&TrChain;
	}

  	for(;cp;)
	{
		if(cp->delete)
		{
			if(cp->content.trtStatus & FLAG_TRANS_NEGATIVE)
			{
				(totalData->VoidCount)++;
				(totalData->VoidPrice) -= cp->content.trtVolume;
			}
			else
			{
				(totalData->VoidCount)++;
				(totalData->VoidPrice) += cp->content.trtVolume;
			}
		}
		else
		{
#ifdef USE_TAX_RATE
			if((cp->content.trtTaxNumber == 0) || (cp->content.trtTaxNumber > MAX_TAX_NO))
			{
				taxNum = MAX_TAX_NO;
			}
			else
			{
				taxNum = cp->content.trtTaxNumber;
			}
#endif
			weight = ad_cnv_weight_unit(cp->content.trtWeight, cp->content.trtMultiple, ADM_Status.UseUnit);	// CJK080219
			//if( cp->content.trtStatus & FLAG_TRANS_NEGATIVE) 
			if( (cp->content.trtStatus & FLAG_TRANS_NEGATIVE) || (cp->content.trtStatus & FLAG_TRANS_RETURN) )   //SG070214
			{
				(totalData->TotalCount)++;
				(totalData->TotalPrice) -= cp->content.trtVolume;
				(totalData->TotalSavedPrice) -= cp->content.trtDiscountVolume;	//Added by JJANG 20071228 
				lastSerialNo = cp->content.trtSerialNo;
				//(totalData->TotalWeight) -= cp->content.trtWeight;
				(totalData->TotalWeight) -= weight;
				(totalData->TotalQuantity) -= cp->content.trtQty;
#ifdef USE_TAX_RATE
				(totalData->taxPrice[taxNum-1]) -= cp->content.trtTax;
				(totalData->taxGross[taxNum-1]) -= cp->content.trtVolume;
				if(cp->content.trtTaxType == 0) 
					totalData->InTaxPrice -= cp->content.trtTax;
				else 
					totalData->ExTaxPrice -= cp->content.trtTax;
#endif
			}
			else
			{
				(totalData->TotalCount)++;
				(totalData->TotalPrice) += cp->content.trtVolume;
				(totalData->TotalSavedPrice) += cp->content.trtDiscountVolume;	//Added by JJANG 20071228 개별 SavedPrice를 합산한다. 
				lastSerialNo   = cp->content.trtSerialNo;
				//(totalData->TotalWeight) += cp->content.trtWeight;
				(totalData->TotalWeight) += weight;
				(totalData->TotalQuantity) += cp->content.trtQty;
#ifdef USE_TAX_RATE
				(totalData->taxPrice[taxNum-1]) += cp->content.trtTax;
				(totalData->taxGross[taxNum-1]) += cp->content.trtVolume;
				if(cp->content.trtTaxType == 0) 
					totalData->InTaxPrice += cp->content.trtTax;
				else 
					totalData->ExTaxPrice += cp->content.trtTax;
#endif
			}
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
	if (!ClerkNum)
	{
		totalData->TotalNetWeight = get_nvram_lparam(NVRAM_SUBTOTAL_NET_WEIGHT);
		totalData->TotalPerTareWeight = get_nvram_lparam(NVRAM_SUBTOTAL_PER_TARE_WEIGHT); 
		totalData->TotalGrossWeight = get_nvram_lparam(NVRAM_SUBTOTAL_GROSS_WEIGHT); 
	}
	if (mode==2) {
		totalData->TotalCount = lastSerialNo;
	}
  	return ON;
} //

INT16U GetClerkTransactionCount(INT16U clerkNum)
{
	TRANSACT_CHAIN  *cp;
	TRANSACT_CHAIN  tc;
	INT16U h_index, next;
	INT16U count=0;
	//cp = (TRANSACT_CHAIN *)get_nvram_lparam(NVRAM_TRANS_HEAD+clerkNum*4);
	h_index = get_nvram_lparam(NVRAM_TRANS_HEAD+clerkNum*4);
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

  	for(;cp;) 
  	{
		if (count>=MAX_ADD_TRANSACTION) break;
		if (!cp) break;
		//cp = cp->next;
		count++;
		if (!(cp->next)) break;
		next = cp->next;
		get_nvram_trans_addr(NVRAM_TRANS_DATA, next - 1, &tc);
		cp = &tc;
	}
	return count;
}

#define NVRAM_CLERK_REOPEN_PAY		NVRAM_CLEARK_LASTSALE + (MAX_TRANSACTION_SALE_SIZE * MAX_REOPEN_SALE * MAX_REOPEN_HEAD) 
void SetClerkReopenTransaction(INT16U ClerkNum, TRANSACTION_PAY pay)
{
	INT16U head;
	INT32U addr;
	INT32U pay_addr;
	INT16U i;
	TRANSACT_CHAIN  *cp;
	TRANSACT_CHAIN tc;
	INT16U h_index, next;

	if(ReadSystemNewDateReopenTicket())	//delete reopen data
	{
		//memset((void *)(NVRAM_CLEARK_LASTSALE+NVRAM_BASE), 0, MAX_TRANSACTION_SALE_SIZE*MAX_REOPEN_SALE*MAX_REOPEN_HEAD);
		nvram_set(NVRAM_CLEARK_LASTSALE, 0, MAX_TRANSACTION_SALE_SIZE*MAX_REOPEN_SALE*MAX_REOPEN_HEAD);
		//memset((void *)(NVRAM_CLERK_REOPEN_PAY+NVRAM_BASE), 0, sizeof(TRANSACTION_PAY)*MAX_REOPEN_HEAD);
		nvram_set(NVRAM_CLERK_REOPEN_PAY, 0, sizeof(TRANSACTION_PAY)*MAX_REOPEN_HEAD);
	}
			     
	head = get_nvram_wparam(NVRAM_REOPEN_HEAD);
	cp = (TRANSACT_CHAIN *)get_nvram_lparam(NVRAM_TRANS_HEAD+ClerkNum*4);
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

	addr = head; 
	addr *= MAX_REOPEN_SALE;
	addr *= MAX_TRANSACTION_SALE_SIZE;
	addr += NVRAM_CLEARK_LASTSALE;

	pay_addr = head;
	pay_addr *= sizeof(TRANSACTION_PAY);
	pay_addr += NVRAM_CLERK_REOPEN_PAY;

	i = 0;
	//memset((void *)(addr+NVRAM_BASE), 0, MAX_TRANSACTION_SALE_SIZE*MAX_REOPEN_SALE); 
	nvram_set(addr, 0, MAX_TRANSACTION_SALE_SIZE*MAX_REOPEN_SALE);
	//memset((void *)(pay_addr+NVRAM_BASE), 0, sizeof(TRANSACTION_PAY));
	nvram_set(pay_addr, 0, sizeof(TRANSACTION_PAY));

	//transaction data
  	for(;cp;) 
  	{
		if (i>=MAX_REOPEN_SALE) break;
		if (!cp) break;
		set_nvram_sparam(addr,(INT8U *)&(cp->content), sizeof(TRANSACTION_RECORD_TYPE));
		//cp = cp->next;
		if (!(cp->next)) break;
		next = cp->next;
		get_nvram_trans_addr(NVRAM_TRANS_DATA, next - 1, &tc);
		cp = &tc;
		i++;
		addr += MAX_TRANSACTION_SALE_SIZE;
	}
	//pay
	if (pay.trtType)
	{
		set_nvram_sparam(pay_addr,(INT8U *)&pay, sizeof(TRANSACTION_PAY));
	}

	head++;
	if (head >= MAX_REOPEN_HEAD) head = 0;
	set_nvram_wparam(NVRAM_REOPEN_HEAD, head);
}

//INT32U CheckReopenData(INT32U TicketNum, INT16U ClerkNum, INT8U *ticket_cnt, INT8U *error)
INT32U CheckReopenData(INT32U *pTicketNum, INT16U ClerkNum, INT8U *ticket_cnt, INT8U *error)
{
	INT32U addr;
	INT32U pay_addr;
	INT16U i;
	INT32U ret;
	INT16U head;
	INT16U temp_head;
	INT8U flag;
	INT16U	customer_no = 0;

	TRANSACTION_RECORD_TYPE trans;
	TRANSACTION_PAY ptrans;

	head = get_nvram_wparam(NVRAM_REOPEN_HEAD);
	if (head == 0) temp_head = MAX_REOPEN_HEAD;
	else temp_head = head;
	temp_head--;

	*ticket_cnt = 0;
	*error = 0;
	ret = 0;
	flag = OFF;

	if (*pTicketNum ==0)	flagcopy_ticket = ON;		// Slave에서 Master로 Copy Ticket 요청 시 Ticket Number를 0으로 하여 Reopen과 동일 한 요청을 함
	else					flagcopy_ticket = OFF;		// 요청한 Ticket Number로 Copy Ticket인지 일반적인 Reopen 요청인지를 구분 
	//각 티켓의 첫번째 Transaction만 Serch    
	for (i = 0; i < MAX_REOPEN_HEAD; i++)
	{
		addr = temp_head; 
		addr *= MAX_REOPEN_SALE;
		addr *= MAX_TRANSACTION_SALE_SIZE;
		addr += NVRAM_CLEARK_LASTSALE;
		
		pay_addr = temp_head;
		pay_addr *= sizeof(TRANSACTION_PAY);
		pay_addr += NVRAM_CLERK_REOPEN_PAY;

		get_nvram_sparam(addr, (INT8U *)&trans, sizeof(TRANSACTION_RECORD_TYPE));
		
		if (*pTicketNum ==0)
		{
			if( trans.trtOperator == ClerkNum)
			{
				*pTicketNum = trans.trtTicketNumber;
				flag = ON;
				break;
			}
		}
		else if (trans.trtTicketNumber == *pTicketNum)
		{
			 flag = ON;
			 break;
		}

		if (temp_head == 0) temp_head = MAX_REOPEN_HEAD;
		temp_head--;
	}
	if (flag) 
	{
		if (trans.trtOperator == ClerkNum)
		{
			ret = addr;
			customer_no = trans.trtCreditCustomerNo;
			for (i = 0; i < MAX_REOPEN_SALE; i++)
			{
				get_nvram_sparam(addr, (INT8U *)&trans, sizeof(TRANSACTION_RECORD_TYPE));
				//data 유무 check
				if (trans.trtPLU) (*ticket_cnt)++;
				addr += MAX_TRANSACTION_SALE_SIZE;
			}

			if (customer_no) set_nvram_wparam(NVRAM_CLERK_CUSTOMER_NO + ClerkNum*2, customer_no);

			get_nvram_sparam(pay_addr, (INT8U *)&ptrans, sizeof(TRANSACTION_PAY));
			if (ptrans.clerkid == ClerkNum)
			{
				LocalPayTotalSummaries(ptrans, 1, ON);
			}
		}
		else *error = 2; //CAN NOT REOPEN! (02:WRONG CLERK NUMBER!)
	}
	else *error = 1; //CAN NOT REOPEN! (01:TICKET IS NOT FOUND!)

	return ret;
}

INT8U GetClerkReopenTransaction(INT32U TicketNum, INT16U ClerkNum)
{
	INT32U addr;
	INT16U res_cnt;
	INT8U ticket_cnt;
	INT8U i;
	INT8U error;
	INT8U stat;
	INT32U org_addr;
	TRANSACTION_RECORD_TYPE trans;
	error = 0;
	res_cnt = ClerkSearchEmptyCount();

	addr = CheckReopenData(&TicketNum, ClerkNum, &ticket_cnt, &error);
	org_addr = addr;
	if (addr)
	{
		if (ticket_cnt > res_cnt) 
		{
			error = 3; //CAN NOT REOPEN! (03:MEMORY FULL!) 
			set_nvram_wparam(NVRAM_CLERK_CUSTOMER_NO, 0);	//Reopen 실패하면 다시 초기화
		}
		else 
		{
			for (i = 0; i < ticket_cnt; i++)
			{
				get_nvram_sparam(addr, (INT8U *)&trans, sizeof(TRANSACTION_RECORD_TYPE));
				stat = ClerkAddTransaction(ClerkNum, &trans, 1, TicketNum);
				addr += MAX_TRANSACTION_SALE_SIZE;
			}
			if (!stat) nvram_set(org_addr, 0, MAX_TRANSACTION_SALE_SIZE*MAX_REOPEN_SALE); //REOPEN 후 기존 reopen한 ticket 삭제
		}
	}
	return error;
}

TRANSACTION_PAY ReopenedTransPay;
// =================================================
// NV RAM
// =================================================
// #define	MAX_LAST_TRANSACTION	MAX_ADD_TRANSACTION	// Modified by CJK 20040721
INT8U	ClerkLastIndex=0;
INT8U	ClerkLastNumber=0;
INT16S ClerkDeleteTransaction(unsigned char ClerkNum)
{ 
	TRANSACT_CHAIN  *cp;
	TRANSACT_CHAIN  tc;
	INT16U h_index, next, current;
	INT16S i=0;

  	if(MAX_CLERK_NO-1 < ClerkNum)	return 0;
	if (TareOper.addupWeightZero && Operation.operationClerk == OPER_FLOATING)
	{
		if (TareOper.tareClerk)
		{
			keyapp_tare(TARE_TYPE_PRESET, 0, OFF);
		}
		ClearClerkTare(ClerkNum);
	}

	//cp = (TRANSACT_CHAIN *)get_nvram_lparam(NVRAM_TRANS_HEAD+ClerkNum*4);
	h_index = get_nvram_lparam(NVRAM_TRANS_HEAD+ClerkNum*4);
	if(!h_index)	return -1;
	current = h_index;
	get_nvram_trans_addr(NVRAM_TRANS_DATA, h_index - 1, &tc);
	cp = &tc;
	ClerkLastNumber = ClerkNum;
	ClerkLastHead = (INT32U)h_index;
#ifndef USE_TRACE_STANDALONE	// 개체기억기능 영역공유
	if (ethernet_list.status != 2) SetClerkReopenTransaction(ClerkNum, ReopenedTransPay); // not slave
//	set_nvram_sparam(NVRAM_CLEARK_LASTSALE,(INT8U *)(NVRAM_BASE+NVRAM_TRANS_DATA), MAX_TRANSACTION_SALE_SIZE*MAX_LAST_TRANSACTION);
#endif
  	for(;cp;) {
		if(i<MAX_LAST_TRANSACTION && ClerkNum) {
			i++;
		} else {
			//DisplayMsg("Over clerk back data");
		}
		next = cp->next;
		cp->next = 0;
		cp->flag = OFF;
		set_nvram_trans_addr(NVRAM_TRANS_DATA, current - 1, &tc);
		if (!next) break;
		get_nvram_trans_addr(NVRAM_TRANS_DATA, next - 1, &tc);
		cp = &tc;
		current = next;
	}
	ClerkLastIndex = i;
	set_nvram_lparam(NVRAM_TRANS_HEAD+ClerkNum*4, 0 );
	set_nvram_wparam(NVRAM_CLERK_CUSTOMER_NO + ClerkNum*2, 0);

	if (!ClerkNum)
	{
		set_nvram_lparam(NVRAM_SUBTOTAL_NET_WEIGHT, 0);      
		set_nvram_lparam(NVRAM_SUBTOTAL_PER_TARE_WEIGHT, 0);      
		set_nvram_lparam(NVRAM_SUBTOTAL_GROSS_WEIGHT, 0); 
	}
  	return 1;
}

///////////////////////////////////
// return value 
//  0x00:ok, 
//  0x01:timeout, (master에서 사용X)
//  0x02:memory full, (master에서 사용X) 
//  0x03:잘못된 데이터 전송, (master에서 사용X) 
//  0x11:clerk no 초과, 
//  0x23:해당index없음, 
//  0x24:중복설정
INT16S ClerkVoidTransaction(unsigned char clerkNum, INT16S index, INT8U deleteFlag, INT8U scaleid, INT8U recvmode)  //SG060615
{ 
	TRANSACT_CHAIN  *cp;
	TRANSACT_CHAIN  tc;
	INT16U h_index, next;
	INT16S	i,searchFlag=OFF;
	INT8U   stat;
	INT8U   backup_status;
	INT8U   backup_delete;
 
  	if(MAX_CLERK_NO-1 < clerkNum) {
		return 0x11;//OFF;
  	}                              
	//cp = (TRANSACT_CHAIN *)get_nvram_lparam(NVRAM_TRANS_HEAD+clerkNum*4);
	h_index = (INT16U)get_nvram_lparam(NVRAM_TRANS_HEAD+clerkNum*4);
	if(!h_index)	return 0x23;//OFF;
	next = h_index;
	get_nvram_trans_addr(NVRAM_TRANS_DATA, h_index - 1, &tc);
	cp = &tc;
	for (i=0;;) {
		i++;
		if (cp->content.trtSerialNo==index) {
			searchFlag=ON;
			break;
		}
		if (cp->next)
		{
			//cp=cp->next;
			next = cp->next;
			get_nvram_trans_addr(NVRAM_TRANS_DATA, next - 1, &tc);
			cp = &tc;
        }
		else
			break;
	}
	if(!searchFlag)
	{
		return 0x23;//OFF;
	}

	backup_status = cp->content.trtStatus;
	backup_delete = cp->delete;
	if(cp->delete != deleteFlag)
		cp->delete = deleteFlag;
	else
		return 0x24;//OFF;
  
	if(deleteFlag) {
		cp->content.trtStatus&=(~FLAG_TRANS_NOVOID);
		cp->content.trtStatus|=FLAG_TRANS_VOID;
	} else {
		cp->content.trtStatus&=(~FLAG_TRANS_VOID  );
		cp->content.trtStatus|=FLAG_TRANS_NOVOID;
	}

	stat = LocalPluTotalSummaries(scaleid, &cp->content, NULL, NULL, recvmode, 0);		// stat=0:ok, 1:timeout, 2:memory full, 3:send data error// SG060615
	if (stat) 
	{
		cp->content.trtStatus = backup_status;
		cp->delete = backup_delete;
		set_nvram_trans_addr(NVRAM_TRANS_DATA, next - 1, &tc);
		return stat;//OFF;
	}
	set_nvram_trans_addr(NVRAM_TRANS_DATA, next - 1, &tc);
  	return 0;//ON;
} //

INT16S ClerkSearchVoidTransaction(INT16U dept, INT32U plu, INT16U clerkNum, INT8U deleteFlag, long *totalPrice)
{ 
	TRANSACT_CHAIN  *cp;
	TRANSACT_CHAIN tc;
    INT16U h_index, next;
	INT16S	i=0, searchFlag=OFF;
 
  	if(MAX_CLERK_NO-1 < clerkNum)
  	{ 
		return -1;
  	}                              
  	////cp = HeadOfClerk[clerkNum]; 
	//cp = (TRANSACT_CHAIN *)get_nvram_lparam(NVRAM_TRANS_HEAD+clerkNum*4);
	h_index = (INT16U)get_nvram_lparam(NVRAM_TRANS_HEAD+clerkNum*4);
	if(!h_index)	return -1;
	get_nvram_trans_addr(NVRAM_TRANS_DATA, h_index - 1, &tc);
	cp = &tc;
	next = h_index;
	if(!plu) return -1;

	for(i=0;cp;i++)//0번에서부터 선택 void list
	{
		//if(dept && plu && plu == cp->content.trtPLU && dept==cp->content.trtDept)	// Modified by CJK 20050310
		if(dept && plu && plu == cp->content.trtPLU && dept==cp->content.trtDept && cp->delete != deleteFlag)
		{
			if(*totalPrice)
			{
				if(*totalPrice == cp->content.trtVolume)
				{
					searchFlag = ON;
					break;
				}
			}
			else
			{
				searchFlag = ON;
				break;
			}
		}
		//cp=cp->next;
		if (!cp->next)
		{
            cp = 0;
			i++;
            break;
		}
		next = cp->next;
		get_nvram_trans_addr(NVRAM_TRANS_DATA, next - 1, &tc);
		cp = &tc;
	}
	if(dept && plu)
	{
		if(!searchFlag)	return -1;
	}
	if(cp && cp->delete != deleteFlag)
	{
		cp->delete = deleteFlag;
		if(cp->content.trtStatus & FLAG_TRANS_VOID)
			cp->content.trtStatus&=(~FLAG_TRANS_VOID);
		else
			cp->content.trtStatus|=FLAG_TRANS_VOID;
		*totalPrice = cp->content.trtVolume;
		set_nvram_trans_addr(NVRAM_TRANS_DATA, next - 1, &tc);
		LocalPluTotalSummaries(status_scale.scaleid,&cp->content,NULL,NULL,0,0); // search void
	}
	else
	{
		return -1;
	}
	return i;	//deleted index
}

INT16S	SizeOfStringToNull(char *p)
{
	INT16S	xx;
	for(xx=0;*p!=(char)NUL;xx++,p++);
	return xx;
}

//INT16S	DisplayPaidTotal(long rTotalPrice)
//{
//	return 0;
//}

//INT16S		EditCreditSale(INT16S x,long currPrice)
//{
//	return 0;
//}

// decFlag(0~1) : 7-Segment 디스플레이 출력시 dot 길이 제외(1), 그 외의 경우 dot 포함(0), Full Graphic LCD: 1 로 설정해야 함
void	FloatToString(INT16S toString,char *str,INT16S totalLength,INT16S mode,long *longNum,INT16S decimalLength, INT8U decFlag)
{
	char	typeLongRight[][7]={"%1ld","%2ld","%3ld","%4ld","%5ld","%6ld","%7ld","%8ld","%9ld","%10ld"};
	char	typeFloat[][6][12]={
		{"%1ld%c%1ld","%0ld%c%02ld","%0ld%c%03ld","%0ld%c%04ld","%0ld%c%05ld","%0ld%c%06ld"},	//3.1
		{"%2ld%c%1ld","%1ld%c%02ld","%0ld%c%03ld","%0ld%c%04ld","%0ld%c%05ld","%0ld%c%06ld"},	//4.1
		{"%3ld%c%1ld","%2ld%c%02ld","%1ld%c%03ld","%0ld%c%04ld","%0ld%c%05ld","%0ld%c%06ld"},	//5.1
		{"%4ld%c%1ld","%3ld%c%02ld","%2ld%c%03ld","%1ld%c%04ld","%0ld%c%05ld","%0ld%c%06ld"},	//6.1
		{"%5ld%c%1ld","%4ld%c%02ld","%3ld%c%03ld","%2ld%c%04ld","%1ld%c%05ld","%0ld%c%06ld"},	//7.1
		{"%6ld%c%1ld","%5ld%c%02ld","%4ld%c%03ld","%3ld%c%04ld","%2ld%c%05ld","%1ld%c%06ld"},	//8.1
		{"%7ld%c%1ld","%6ld%c%02ld","%5ld%c%03ld","%4ld%c%04ld","%3ld%c%05ld","%2ld%c%06ld"},	//9.1
		{"%8ld%c%1ld","%7ld%c%02ld","%6ld%c%03ld","%5ld%c%04ld","%4ld%c%05ld","%3ld%c%06ld"}	//10.1
		};
	char 	decimalSign;
///////////////////////////
//	unsigned long	setFloatLong[]={1L,10L,100L,1000L,10000L,100000L,1000000L,10000000L,100000000L,1000000000L};
	unsigned long	intNum,floatNum,i;
	char	buf[20];
	if(!decimalLength)	
		decimalLength = display_parameter.mode_dec[mode];
	decimalSign = display_parameter.mode_fix[mode];
	if(toString)
	{
		if(decimalLength)
		{
			intNum		=labs(*longNum)/power10(decimalLength);
			floatNum	=labs(*longNum)%power10(decimalLength);
			// Create Format
#ifndef _USE_LCD32_           
			if (decFlag)
				sprintf(buf,typeFloat[totalLength-2][decimalLength-1],intNum,decimalSign,floatNum);
			else
#endif                
				sprintf(buf,typeFloat[totalLength-3][decimalLength-1],intNum,decimalSign,floatNum);
			if(*longNum<0L)	{
				for(i=0;i<totalLength;i++) {		//se-hyung 20071017
					if(*(buf+i)!=' ') {		
						break;
					}
				}
				if(!i)
				{
					*str='-';
					strcpy(str+1,buf);
				}
				else
				{
 					*(buf+i-1)='-';			 //se-hyung 20071017     
				       	strcpy(str,buf);
				}
			}	 
			else
			{
				strcpy(str,buf);
			}
		}
		else
			sprintf(str,typeLongRight[totalLength-1],*longNum);

	}
	else
	{
		if(decimalLength)
		{
			sscanf(str,typeFloat[totalLength-3][decimalLength-1],&intNum,&decimalSign,&floatNum);
			*longNum=intNum*power10(decimalLength)+floatNum;
		}
		else
			sscanf(str,typeLongRight[totalLength],longNum);
	}
}

extern DISC_SALE_TYPE		DiscSale;

#define	MAX_CHANGE		15
INT16S		ChgIndex;
void	ClearPayment(INT8U trtType)
{
	INT16S	i;

	transPay.trtType = trtType;
	transPay.scaleid = 0;
	transPay.clerkid = 0;
	for(i=0;i<MAX_PAID_TYPE;i++)	transPay.paidprice[i] = 0;
	transPay.change = 0;
	transPay.summaryround = 0;
	transPay.summarydiscount = 0;	//Added by JJANG 20080512
}

INT16S CheckBoundry(CAPTION_STRUCT* p,INT16S num)
{
	if(p->input_min && num<p->input_min)
	{
		return ON;
	}
	if (p->input_max && p->input_max<num)
	{
		return ON;
	}
	return OFF;
}

void plucall_payment(long num)
{	
	INT16S     max_num;
//	INT8U	y_gab=8;				// 폰트의 y 크기
//	INT8U	x_gab=6;				// 폰트의 x 크기
    INT16S x_pos;

#ifdef USE_ARAB_CONVERT
    x_pos = 16;
#else
    x_pos = 18;
#endif
	max_num=MAX_PAID_TYPE;
	if (!(status_scale.restrict&FUNC_CUSTOM)) max_num=6;
	if(num < max_num)
		caption_split_by_code(0xc000+num,&cap,0);
	else
	{
        caption_split_by_code(0xE102,&cap,0);
        BuzOn(2);
    }
#ifdef _USE_LCD20848_//ck
	PutNSmallString(1,x_pos,(char *)cap.form,20);
#else
	PutNSmallString(2,18,(char *)cap.form,20);
#endif
}

char*	GetCurrency(char* pCurr,INT8U prt_or_display)
{
#ifdef USE_EXTEND_PRICE_PRIMARYSIGN
	if (prt_or_display) {
		get_global_sparam(GLOBAL_AREA_PRICE_PRIMARYSIGN,(INT8U *)pCurr,5);
	} else {
		get_global_sparam(GLOBAL_DISPLAY_PRICE_PRIMARYSIGN,(INT8U *)pCurr,5);
	}
	*(pCurr+5)=0;
#else
	if (prt_or_display) {
		get_global_sparam(GLOBAL_AREA_PRICE_PRIMARYSIGN,(INT8U *)pCurr,4);
	} else {
		get_global_sparam(GLOBAL_DISPLAY_PRICE_PRIMARYSIGN,(INT8U *)pCurr,4);
	}
	*(pCurr+4)=0;
#endif
	str_trimleft(pCurr);
	str_trim(pCurr);
	return pCurr;
}

INT16S	GetPriceString(char *rString,char *priceBuf, INT16S trim,INT8U prt_or_display)
{
#ifdef USE_EXTEND_PRICE_PRIMARYSIGN
	char	currency[6];
#else
	char	currency[5];
#endif
	INT16S i;
	char currency_type;

	currency_type = get_global_bparam(GLOBAL_AREA_PRICE_TYPE);
	currency_type &= 0x0f;
	if ((currency_type > 3) || (currency_type < 0)) currency_type = 1;
#ifdef USE_EXTEND_PRICE_PRIMARYSIGN
	if (prt_or_display) {
		get_global_sparam(GLOBAL_AREA_PRICE_PRIMARYSIGN,(INT8U *)currency,5);
	} else {
		get_global_sparam(GLOBAL_DISPLAY_PRICE_PRIMARYSIGN,(INT8U *)currency,5);
	}		
	currency[5]=0;
#else
	if (prt_or_display) {
		get_global_sparam(GLOBAL_AREA_PRICE_PRIMARYSIGN,(INT8U *)currency,4);
	} else {
		get_global_sparam(GLOBAL_DISPLAY_PRICE_PRIMARYSIGN,(INT8U *)currency,4);
	}		
	currency[4]=0;
#endif
	for(i=3;0<i;i--)
	{
		if(currency[i]==' ')	currency[i]=0;
	}
	if(trim) priceBuf=str_trimleft(priceBuf);

	switch (currency_type) {
		case 0: 					// 12.34
			sprintf(rString, "%s", priceBuf);
			break;
		case 3: 					// $ 12.34 c
		case 1: 					// $ 12.34
			sprintf(rString, "%s %s", currency, priceBuf);	
			break;
		case 2: 					// 12.34 $
			sprintf(rString, "%s %s", priceBuf, currency);
			break;
	}

	return 0;
}

INT16S	DecimalPointPrice(void)
{
	INT32U pnt;
	INT16S	decpoint;

	pnt = DFLASH_BASE + FLASH_GLOBAL_AREA;
	decpoint = Flash_bread(pnt+GLOBAL_AREA_PRICE_DECIMALPOINT);
	return decpoint;
}
INT16S	DecimalPointWeight(void)
{
	INT32U pnt;
	INT16S	decpoint;

	pnt = DFLASH_BASE + FLASH_GLOBAL_AREA;
	decpoint = Flash_bread(pnt+GLOBAL_DISPLAY_WEIGHTDECIMAL);
	return decpoint;
}

void	ClerkGetName(INT16U clerkid ,char* p)
{
	CLERK_STRUCT clerk;
	INT32U pnt;

	if(!clerkid)
	{
		strcpy(p,"");
		return;
	}
	pnt = clerkid;//status_scale.clerkid;
	pnt *= CLERK_STRUCT_SIZE;
	pnt += FLASH_CLERK_AREA;
	get_base_sparam(pnt, (INT8U*) &clerk, CLERK_STRUCT_SIZE);
	memcpy(p,clerk.nickname,8);
	*(p+8) = 0;

	return;
}
// extern void ClearLCD();
extern void keyapp_total(INT16S directPrint);
extern TARE_OPER_TYPE		TareOper;

//long PayPrice, ChangePrice;

INT8U	DataSearchFlag=OFF;
void DispSaleCustomer(long num)
{	
//DEL070709	POINT 			disp_p;
//	INT8U	y_gab=8;				// 폰트의 y 크기
//	INT8U	x_gab=6;				// 폰트의 x 크기
	CUSTOMER_STRUCT custom;


//DEL070709	disp_p.x = 2;
//DEL070709	disp_p.y = 20;

	//"Cash        "
	//"Pin/chip    "
	//"Check       "
	//"Credit card "
	//"Credit note "
	//"Coupon      "
	//"Bonus point "
	//"Credit      "
	customer_read(num, &custom);
	if(num != custom.code)
	{
		caption_split_by_code(0xc908,&cap,0);//"Not found"
		strcpy(gmsg,cap.form);
		DataSearchFlag = OFF;
	}
	else
	{
		strncpy(gmsg,(char *)custom.name,24);
		DataSearchFlag = ON;
	}
#ifdef	_USE_LCD20848_//ck
	PutNSmallString(1,18,gmsg,20);
#elif defined(_USE_LCD32_)
	PutNSmallString(2,18,gmsg,20);
#else
	POINT disp_p;
	disp_p.x = 0;
	disp_p.y = 0;
	display_lcd_clear();
	Dsp_Write_String(disp_p, gmsg);
#endif
}
extern	SALE_G_PRICE_TYPE	GlbPrice;
INT8U CloseTicketWithoutPrint = OFF;
#define DIPS_WEIGHT_POS   5
long	CalcPay(INT8U clerk, long originPrice, long savedPrice, INT8U paymentID)
{

	char	ch_rprice[20],priceBuf[15];
	long	rprice=0L;	// 현재 잔액
#ifdef USE_AU_ROUND
    long    notRoundedPrice;
#endif
	long 	tempPrice = 0;	//호주 round 기능 관련 추가. credit 일 때는  round 사용 안함.
	long  	rTotalPrice=0;	// 지불 총액
	INT16S	i,payType=0;
	INT16S	decpoint;
	long	tempLong;
	long	payPrice=0;	// 현재 지불입력액
 	INT16U  result;
	INT16S	priceLength = display_parameter.mode_len[DISPLAY_PRICE];
	char	tempBuf[40];
	INT16S     max_pay;
	long	ret;
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
	char	old_font;
	char	char_w, char_h;
	INT8U   check=0;
	INT8U   rtnum=0;
	char    flagPay=FALSE;
#endif
	long    roundDifference;
#ifdef USE_TAX_RATE    
	INT16S 	saveTax=GlbFloat.printTaxInfo; // 1762의 tax 프린트 세팅을 tax on off 키를 사용하여 변경하여도 함수 밖으로 나가면 기존의 세팅을 계속 유지 하기 위해 저장해두는 변수.
#endif
	INT8U	customerFlag=0;
	INT8S	paypos=0;
#ifdef USE_TAX_RATE
	STRUCT_TOTAL_PRICE totalData;
	long	discountTax = 0;
#endif
	INT8U   customerNum;
	POINT disp_p;

#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
	PutXStringOffsetX = DSP_SALE_INFO_AREA_X1;
	PutXStringOffsetY = DSP_SALE_INFO_AREA_Y1;
#else
	disp_p.x = 0;
	disp_p.y = 0;
#endif
	decpoint=DecimalPointPrice();

	max_pay = MAX_PAID_TYPE;
	if (!(status_scale.restrict&FUNC_CUSTOM)) max_pay=6;
	gmsg[0]=0;
	rprice = originPrice - savedPrice;
#ifdef USE_TAX_RATE
	ClerkTotalPrice(0, clerk, &totalData);
	if(savedPrice == 0)
		discountTax = totalData.ExTaxPrice;
	else
	{
		discountTax = (float)savedPrice / (float)originPrice * totalData.ExTaxPrice + 0.5;
	}
	rprice += discountTax;
#endif
	ClearPayment(TRANS_TYPE_PAY_TOTAL_PAY);
#ifdef USE_AU_ROUND
    notRoundedPrice = rprice;
#endif
	if(GlbPrice.roundApply == ROUND_TOTAL_OR_PLU)
	{
		rprice = PriceRounds(rprice,ROUND_TOTAL_OR_PLU  ,ON, &roundDifference);
		transPay.summaryround = roundDifference;
	}
	else if((GlbPrice.roundApply == ROUND_ONLY_TOTAL_SUM) && (PrtStruct.Mode == RECEIPT))
	{
		rprice = PriceRounds(rprice,ROUND_ONLY_TOTAL_SUM,ON, &roundDifference);
		transPay.summaryround = roundDifference;
	}

	ClearDisp7();
	display_lcd_clear();
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
	old_font=DspStruct.Id1;
	DspLoadFont1(DSP_SMALL_FONT_ID);
	char_w = display_font_get_width();
	char_h = display_font_get_height();
#endif
	ret = 0;

	if((get_global_bparam(GLOBAL_SALE_SETUP_FLAG20)>>5)&0x01)	//734
			payType = PAY_CARD; //EFT POS
	else
		payType = (INT16S)paymentID;
	
#ifdef _USE_LCD20848_//ck
	Dsp_Clear();
	paypos-=1;
#endif

	while (1)
	{
		//items=totalCount;
#ifndef USE_SINGLE_LINE_GRAPHIC
		if(Operation.operationClerk != OPER_NORMAL)	// clerk
		{
			ClerkGetName(clerk ,tempBuf);
			caption_split_by_code(0xc012,&cap,0);	//"Clerk(%d): %s"
			sprintf(gmsg,cap.form, clerk,tempBuf);
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
			PutSmallString(paypos,0,gmsg);
#else
			Dsp_Write_String(disp_p, gmsg);
#endif
			Delay_100ms(7);
		}
#endif
#ifdef USE_AU_ROUND
		FloatToString(TO_STR,priceBuf,priceLength,DISPLAY_PRICE,&notRoundedPrice,0,1);
#else
		FloatToString(TO_STR,priceBuf,priceLength,DISPLAY_PRICE,&rprice,0,1);
#endif
#if !defined(_USE_LCD20848_) && !defined(_USE_LCD32_)
		display_string_pos_clear(DIPS_WEIGHT_POS,20);
#endif
		caption_split_by_code(0xc013,&cap,0);//"SUBTOTAL"
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
		PutSmallString(paypos+1,0,cap.form);
		GetPriceString(tempBuf,priceBuf,OFF,PRICE_DISPLAY); // currency와 함께
		PutSmallString(paypos+1,13,tempBuf);

		//DisplayPaidTotal(rTotalPrice);
		strset(gmsg,0x20,40);					//insert by se-hyung for select payment 20070530
 		PutSmallString(paypos+2,0,gmsg);			 	//insert by se-hyung for select payment 20070530
		caption_split_by_code(0xc014,&cap,1);//"Payment      [ ]"
		PutSmallString(paypos+2,0,cap.form);
		strset(gmsg,0x20,40);					//insert by se-hyung for select payment 20070530  전화면 잔상 없애기 위해
		PutSmallString(paypos+3,0,gmsg);				//insert by se-hyung for select payment 20070530  전화면 잔상 없애기 위해
		caption_split_by_code(0xc00C,&cap,0);//"SELECT PAYMENT"//insert by se-hyung for select payment 20070530
		PutSmallString(paypos+3,0,cap.form);			       //insert by se-hyung for select payment 20070530

		keyin_setcallback(plucall_payment);
		DspStruct.DirectDraw = 1;
		tempLong = (long)payType;

		if (check==1)						//insert by se-hyung for select payment 20070530
		{							//insert by se-hyung for select payment 20070530
		  tempLong = (long)rtnum;				//insert by se-hyung for select payment 20070530
		  check = 0;						//insert by se-hyung for select payment 20070530
		}							//insert by se-hyung for select payment 20070530		 }

        if(flagPay)
        {
            result = keyin_ulong(KI_GRAPHIC, KI_FORM_NORMAL, 0,(long*)&tempLong, 1,
                                    (paypos+2)*char_h+PutXStringOffsetY,  14*char_w+PutXStringOffsetX, 0,KI_NOPASS );
        }
        else
        {   flagPay = TRUE;
            result = keyin_ulong(KI_GRAPHIC, KI_FORM_NORMAL, 0,(long*)&tempLong, 1,
                                    (paypos+2)*char_h+PutXStringOffsetY,  14*char_w+PutXStringOffsetX, 0,KI_PASS );
            result=KP_ENTER;
        }
		payType = (INT8U)tempLong;
		keyin_clearcallback();

		if (result==KP_TEST)   					//insert by se-hyung for select payment 20070530
		{ 		       					//insert by se-hyung for select payment 20070530
		   rtnum = selectpayment(rtnum);			//insert by se-hyung for select payment 20070530
 		   check = 1;						//insert by se-hyung for select payment 20070530
		   display_lcd_clear();					//insert by se-hyung for select payment 20070530
		 }		       					//insert by se-hyung for select payment 20070530

	     	if(max_pay <= payType)	continue;

	     	if(result==KP_ESC)
		{
			break;
		}
                if( !(result==KP_ENTER || result==KP_SAVE || result==KP_ARROW_DN) ) continue;   			//modify by se-hyung for select payment 20070530
#else
		display_string_pos(DIPS_WEIGHT_POS, (INT8U*)cap.form);
		display_string(DISPLAY_PRICE, (INT8U*)priceBuf); //Subtotal Price display
		VFD7_Diffuse();
#endif
PAYMENT:
		if(payType !=PAY_CREDIT)
		{
#ifdef USE_AU_ROUND
			if(payType == PAY_CARD || payType==PAY_CREDIT)
			{
				payPrice = notRoundedPrice;
			}
			else
			{
				payPrice = rprice;
			}
#else
			payPrice = rprice;
#endif
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
			strset(gmsg,0x20,40);				//insert by se-hyung for select payment 20070530
			PutSmallString((paypos+3),0,gmsg);			//insert by se-hyung for select payment 20070530
			    caption_split_by_code(0xc000+payType,&cap,0);
			str_fill((char *)cap.form,14);
			PutSmallString((paypos+3),0,cap.form);
#ifdef USE_ARAB_CONVERT
            result = keyin_ulong(KI_GRAPHIC, KI_FORM_ILNN, decpoint,(long*)&payPrice, 10,
						(paypos+3)*char_h+PutXStringOffsetY,  14*char_w+PutXStringOffsetX, 0,KI_NOPASS );
#else
			result = keyin_ulong(KI_GRAPHIC, KI_FORM_NORMAL, decpoint,(long*)&payPrice, 10,
						(paypos+3)*char_h+PutXStringOffsetY,  14*char_w+PutXStringOffsetX, 0,KI_NOPASS );
#endif
		} else { // credit type


			DiscSale.saleCreditCustomer = 0;
			strset(gmsg,0x20,40);
			PutSmallString((paypos+2),0,gmsg);
			caption_split_by_code(0xc010,&cap,1);//"Customer	 [	]"

			PutSmallString((paypos+2),0,cap.form);
			keyin_setcallback(DispSaleCustomer);
			DspStruct.DirectDraw = 1;
			tempLong = (long)DiscSale.saleCreditCustomer;
#ifdef USE_ARAB_CONVERT
            result = keyin_ulong(KI_GRAPHIC, KI_FORM_ILNN, 0,(long*)&tempLong, 2,
			     					(paypos+2)*char_h+PutXStringOffsetY,  14*char_w+PutXStringOffsetX, 0,KI_NOPASS );
#else
			result = keyin_ulong(KI_GRAPHIC, KI_FORM_NORMAL, 0,(long*)&tempLong, 2,
									(paypos+2)*char_h+PutXStringOffsetY,  14*char_w+PutXStringOffsetX, 0,KI_NOPASS );
#endif
			keyin_clearcallback();
			DiscSale.saleCreditCustomer = (INT8U)tempLong;
			if(!DataSearchFlag)
			{
				BuzOn(2);					 //insert by se-hyung for select payment 20070530
				continue;					 //insert by se-hyung for select payment 20070530
							}
			if(!DiscSale.saleCreditCustomer)
					result=KP_ESC;
			else
			{
				strset(gmsg,0x20,40);				//insert by se-hyung for select payment 20070530  전화면 잔상 없애기 위해
				PutSmallString((paypos+3),0,gmsg);			//insert by se-hyung for select payment 20070530  전화면 잔상 없애기 위해
				caption_split_by_code(0xc000+PAY_CREDIT,&cap,0);//"CREDIT SALES [		  ]"
				PutSmallString((paypos+3),0,cap.form);
#ifdef USE_ARAB_CONVERT
                result = keyin_ulong(KI_GRAPHIC, KI_FORM_ILNN, decpoint,(long*)&payPrice, 10,
				     					(paypos+3)*char_h+PutXStringOffsetY,  14*char_w+PutXStringOffsetX, 0,KI_NOPASS );
#else
				result = keyin_ulong(KI_GRAPHIC, KI_FORM_NORMAL, decpoint,(long*)&payPrice, 10,
										(paypos+3)*char_h+PutXStringOffsetY,  14*char_w+PutXStringOffsetX, 0,KI_NOPASS );
#endif
				keyin_clearcallback();
			}
		}
		if(result==KP_ARROW_UP)	continue;
		if(result==KP_PAGE_UP) 	continue;
		if(result==KP_PAGE_DN) 	continue;
		if(result==KP_ESC )    {
		       continue;
		}
#else
			caption_split_by_code(0xc000+payType,&cap,0);
			str_fill((char *)cap.form,14);
			Dsp_Write_String(disp_p, cap.form);
			result = keyin_ulong(KI_TOTAL, KI_FORM_NORMAL, decpoint,(long*)&payPrice, 10, 
								 0,  0, 0,KI_NOPASS );
		}
		else //credit sale type
		{
			if (!customerFlag)
			{
				DiscSale.saleCreditCustomer = 0;
				ClearDisp7();
				display_string_pos_clear(DIPS_WEIGHT_POS,20);
				caption_split_by_code(0xc010,&cap,0);//"Customer     [  ]"
				display_string_pos(DIPS_WEIGHT_POS, (INT8U*)cap.form);
				keyin_setcallback(DispSaleCustomer);
				DspStruct.DirectDraw = 1;
				tempLong = (long)DiscSale.saleCreditCustomer;
				result = keyin_ulong(KI_TOTAL, KI_FORM_NORMAL, 0,(long*)&tempLong, 2, 
									 0,  0, 0,KI_NOPASS );
				keyin_clearcallback();
			}
			else
			{
				DiscSale.saleCreditCustomer = customerNum;
			
				display_lcd_clear();
				caption_split_by_code(0xc013,&cap,0);//"SUBTOTAL"
				display_string_pos(DIPS_WEIGHT_POS, (INT8U*)cap.form);
				caption_split_by_code(0xc000+PAY_CREDIT,&cap,0);//"CREDIT SALES [         ]"
				Dsp_Write_String(disp_p, cap.form);
					
				result = keyin_ulong(KI_TOTAL, KI_FORM_NORMAL, decpoint,(long*)&payPrice, 10, 
									 0,  0, 0,KI_NOPASS );
				keyin_clearcallback();
			}
			if (result == KP_ENTER)
			{
				if (!customerFlag)
				{
					if (!DataSearchFlag || (tempLong == 0))
					{
						BuzOn(2);
						continue;
					}
					else
					{
						customerNum = (INT8U)tempLong;
						customerFlag = 1;
						result = 0;
					}
				}
			}
		}

		if ((result == KP_PAGE_UP) || (result == KP_ARROW_UP))
		{
			if (payType == PAY_CREDIT)
			{
				customerFlag = 0;
				customerNum = 0;
			}
			payType--;
			if (payType < 0) payType = max_pay - 1;
			continue;
		}
		if ((result == KP_PAGE_DN) || (result == KP_ARROW_DN))
		{
			if(payType == PAY_CREDIT)
			{
				customerFlag = 0;
				customerNum = 0;
			}
			payType++;
			if (payType >= max_pay) payType = 0;
			continue;
		}
		if (result == KP_ESC)
		{
			display_string_pos_clear(DIPS_WEIGHT_POS,20);
			ClearDisp7();
			break;
		}
#ifdef USE_TAX_RATE
		if (result == KP_TEST)
		{
			if (GlbFloat.printTaxInfo == 0)		//tax print off 일 경우 들어옴 
			{
				GlbFloat.printTaxInfo = 1;
			}
			else			      		//tax print on 일 경우 들어옴 
			{
				GlbFloat.printTaxInfo = 0;
			}
			
			display_lcd_clear();
			
			if (GlbFloat.printTaxInfo == 0)
			{
				caption_split_by_code(0xc00A,&cap,0);
			}
			else
			{
				caption_split_by_code(0xc00B,&cap,0);
			}
			Dsp_Write_String(disp_p, cap.form);
			Delay_100ms(5);
			continue;
		}
#endif
#endif
		if(payType==PAY_CARD || payType==PAY_CREDIT)	// credit card / credit sale
		{
#ifdef USE_AU_ROUND	//australia 의 경우 card 사용시 round off 안함
			if(notRoundedPrice < payPrice)
#else
			if(rprice<payPrice)
#endif
			{
				continue;
			}
		}

		if( !(result==KP_ENTER || result==KP_SAVE || result==KP_TEST || result == KP_CLOSE_TICKET)) goto PAYMENT;

		if((result==KP_ENTER || result==KP_SAVE || result==KP_TEST || result == KP_CLOSE_TICKET) && !transPay.change)
		{
			if(payPrice)
			{
				transPay.paidprice[payType] += payPrice;
			}
		}

// Result : Display & Print ///////////////
#ifdef USE_AU_ROUND	//australia 의 경우 card 사용시 round off 안함
		if(payType==PAY_CARD || payType==PAY_CREDIT)	// credit card / credit sale
		{
			tempPrice = notRoundedPrice;
		}
		else
		{
			tempPrice = rprice;
		}
#else
		tempPrice = rprice;
#endif
		if (tempPrice <= payPrice)
		{
#ifdef USE_AU_ROUND	//australia 의 경우 card 사용시 round off 안함
			if(payType==PAY_CARD || payType==PAY_CREDIT)	// credit card / credit sale
			{
				transPay.summaryround = 0;
			}
#endif
			rTotalPrice = 0L;
			for (i = 0; i < max_pay;i++)
			{
				rTotalPrice += transPay.paidprice[i];
			}
			transPay.change = payPrice - tempPrice;

#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
			strset(gmsg,0x20,40);
			PutSmallString((paypos+2),0,gmsg);
			PutSmallString((paypos+3),0,gmsg);

			caption_split_by_code(0xc015,&cap,0);//"RECEIVE"
			PutSmallString((paypos+2),0,cap.form);
			FloatToString(TO_STR,ch_rprice,priceLength,DISPLAY_PRICE,&rTotalPrice,0,1);
			GetPriceString(tempBuf,ch_rprice,OFF,PRICE_DISPLAY);
			PutSmallString((paypos+2),13,tempBuf);

			caption_split_by_code(0xc016,&cap,0);//strcpy(gmsg,"CHANGE");
			PutSmallString((paypos+3),0,cap.form);
			FloatToString(TO_STR,ch_rprice,priceLength,DISPLAY_PRICE,&transPay.change,0,1);
			GetPriceString(tempBuf,ch_rprice,OFF,PRICE_DISPLAY);
			PutSmallString((paypos+3),13,tempBuf);
#else
			display_lcd_clear();
			display_string_pos_clear(DIPS_WEIGHT_POS,20);
			
			caption_split_by_code(0xc016,&cap,0);//strcpy(gmsg,"CHANGE");
			display_string_pos(DIPS_WEIGHT_POS+5, (INT8U*)cap.form);
			
//			caption_split_by_code(0x9120,&cap,0);//press any key
//			Dsp_Write_String(disp_p, cap.form);
			
			FloatToString(TO_STR,ch_rprice,priceLength,DISPLAY_PRICE,&transPay.change,0,1);
			display_string(DISPLAY_PRICE, (INT8U*)ch_rprice); //Subtotal Price display
			VFD7_Diffuse();
#endif
#ifdef USE_PERSIA_DEFAULT
			if(payType==PAY_CASH || payType==PAY_CHECK || payType==PAY_COUPON)	// credit card / credit sale
			{
				Cash_open();
			}
#else
			Cash_open();
#endif
			ComputeCurrency(0, 0, 0, rTotalPrice - transPay.change, 0);// display,y,x

			GlbFlagPCnMasterConnection = NetCheckConnection(); //SG070703
			if(result == KP_CLOSE_TICKET)
			{
				CloseTicketWithoutPrint = ON;
			}
#ifdef USE_TAX_RATE
			PrintTotalLabelTicket(TRANS_TYPE_PAY_TOTAL_PAY, clerk, rTotalPrice - transPay.change - discountTax, rTotalPrice, savedPrice);
#else
			PrintTotalLabelTicket(TRANS_TYPE_PAY_TOTAL_PAY, clerk, rTotalPrice - transPay.change, rTotalPrice, savedPrice);
#endif
			GlbFlagPCnMasterConnection = ON; //SG070703
			ret = 1;
			break;
			//return 1L;
		}
		rprice -= payPrice;
#ifdef USE_AU_ROUND
		notRoundedPrice -= payPrice;
#endif
//////////////////////////////
	}

#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
	DspLoadFont1(old_font);
	sale_pluclear(ON);
	display_lcd_clear();
#else
	sale_pluclear(ON);
	display_lcd_clear();
	display_string_pos_clear(DIPS_WEIGHT_POS,20);
#endif
	sale_display_update(0x0fff);
#ifdef USE_TAX_RATE    
	GlbFloat.printTaxInfo=saveTax; // 항상 1762의 세팅값을 유지해 주기 위해  (Tax print on, off)
#endif
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
	PutXStringOffsetX = 0;
	PutXStringOffsetY = 0;
#endif
	return ret;
}

long PayChangeSummary(TRANSACTION_PAY trans_pay)
{
	LocalPayTotalSummaries(trans_pay,1,OFF);

	return 0;
}

extern void send_clerk_sale_data(HUGEDATA COMM_BUF *CBuf,ETH_CONNECT_LIST eth,INT8U ClerkNum,INT16S id,TRANSACT_CHAIN *xtrans);

INT16S Network_ClerkTransaction(HUGEDATA COMM_BUF *CBuf,ETH_CONNECT_LIST eth,unsigned char ClerkNum,INT16S id)
// =====================================================================
// Clerk Login 시 Master 가 Slave 로 id 번째의 정보를 Slave로 보낸다.
// =====================================================================
{ 
	TRANSACT_CHAIN  *cp;
	TRANSACT_CHAIN tc;
	INT16U h_index, next;
	INT16S i=0;
 	INT8U send_ok;

  	if(MAX_CLERK_NO-1 < ClerkNum)	return 0;
	//cp = (TRANSACT_CHAIN *)get_nvram_lparam(NVRAM_TRANS_HEAD+ClerkNum*4);
	h_index = (INT16U)get_nvram_lparam(NVRAM_TRANS_HEAD+ClerkNum*4);
	if (h_index)
	{
		get_nvram_trans_addr(NVRAM_TRANS_DATA, h_index - 1, &tc);
		cp = &tc;
	}
	else
	{
		cp = 0;
	}
	send_ok=0;
	if (!cp) goto ENDDATA;
	//get_nvram_trans_addr(NVRAM_TRANS_DATA, h_index - 1, &tc);
	//cp = &tc;
  	for(;cp;) {
		if (i>=500) break;
		if (i==id) {
			if (!cp) goto ENDDATA;
			send_clerk_sale_data(CBuf,eth,ClerkNum,id,cp);
			send_ok=1;
			break;
		}
		if (!cp) goto ENDDATA;
		next = cp->next;
		//cp     = cpNext;
		i++;
		if (!next) break;
		get_nvram_trans_addr(NVRAM_TRANS_DATA, next - 1, &tc);
		cp = &tc;
	}
// 전송완료이면 현재 저울의 내용을 삭제
// - Protocol 변경으로 미사용
	if (send_ok==0) goto ENDDATA;
	return 0;
ENDDATA:
	send_clerk_sale_data(CBuf,eth,ClerkNum,id,NULL);
	return 1;
}

//insert INT8U selectpayment(INT8U preKey) by se-hyung for select payment & Tax print on, off 20070530
INT8U selectpayment(INT8U preKey)
{
	INT8U selectNum;
	INT8U i;
    INT8U max_num;
	INT8U isBreak;
	char tempBuf[40];
	isBreak = 0;	 //se-hyung 20071017
#ifdef USE_ARAB_CONVERT
    INT8U displayLen = 14;
#else
    INT8U displayLen = 18;
#endif
	
 	display_lcd_clear();
    max_num = MAX_PAID_TYPE;
    if (!(status_scale.restrict&FUNC_CUSTOM)) max_num = 6;


        while (1)
        {	
 		INT16U capCode = 0xc000;
		INT8U indexCap = 0;
		for (i = 0; i < 4; i++)
		{
            if(indexCap < max_num) // indexCap starts 1
            {
    			caption_split_by_code(capCode+indexCap,&cap,0);
    			sprintf(tempBuf, "%d.%s",indexCap, cap.form);
    			PutSmallString(i,1,tempBuf);
    			indexCap++;

    			if (indexCap < MAX_PAID_TYPE )
    			{
    				caption_split_by_code(capCode+indexCap,&cap,0);
    				sprintf(tempBuf, "%d.%s",indexCap, cap.form);
    				PutSmallString(i,displayLen,tempBuf);
    			}
#ifdef USE_TAX_RATE            
    			else if (indexCap == MAX_PAID_TYPE)
    			{
    				if (GlbFloat.printTaxInfo == 0)
    				{
    					caption_split_by_code(0xc00A,&cap,0);
    				}
    				else
    				{
    					strset(gmsg,0x20,40);				//insert by se-hyung for select payment 20070530  전화면 잔상 없애기 위해
    					PutSmallString(3,26,gmsg);			//insert by se-hyung for select payment 20070530  전화면 잔상 없애기 위해
    					caption_split_by_code(0xc00B,&cap,0);
    				}
    				sprintf(tempBuf, "%d.%s",indexCap, cap.form);
    				PutSmallString(i,displayLen,tempBuf);	
    			}
#endif  
            }
			indexCap++;
		}


		while (!KEY_Read());	     //se-hyung 20071017
 		{
			BuzOn(1);
			isBreak = 1;
			switch (KeyDrv.CnvCode)
			{
				case KS_CLEAR:					
				case KP_SAVE:
				case KP_ESC:
				case KP_ENTER:
				case KP_ARROW_DN:
					selectNum=preKey;
					break;

				case KEY_NUM_0:
				case KEY_NUM_1:
				case KEY_NUM_2:
				case KEY_NUM_3:
				case KEY_NUM_4:
				case KEY_NUM_5:
				case KEY_NUM_6:
				case KEY_NUM_7:
				      selectNum	= KeyDrv.CnvCode;
				      break;
#ifdef USE_TAX_RATE	
				case KEY_NUM_8: 
				{
					if (GlbFloat.printTaxInfo == 0)		//tax print off 일 경우 들어옴 
					{
						GlbFloat.printTaxInfo = 1;
		       	 		}
					else			      		//tax print on 일 경우 들어옴 
					{
						GlbFloat.printTaxInfo = 0;
		      			}
					isBreak = 0;
					break;
				}
#endif
				case KEY_NUM_9: 
					BuzOn(2);
					isBreak = 0;
					break;
   
				default:
					BuzOn(2);
					isBreak = 0;
					break;
 			}
	       }
	       if (isBreak)
	       {
	       		break;
	       }
	       else 
	       {
	       		continue;	
	       }
	}
	return selectNum;
}











