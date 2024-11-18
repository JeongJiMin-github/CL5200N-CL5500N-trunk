/*****************************************************************************
|*		       
|*  Copyright		:	(c) 2003 CAS
|*  Filename		:	inventory.c
|*  Version		:	1.0
|*  Programmer(s)		:	cjk
|*  Created		:	2012/12/07
|*  Description		:	
|*              				
****************************************************************************/

#include <stdio.h>
#include <string.h>
#include "globals.h"
#include "initial.h"
#include "inventory.h"
#include "caption_app.h"
#include "mode_sale.h"
#include "menu.h"
#include "sale_report.h"
#include "commbuf.h"
#include "plu_device.h"
#include "prt_font.h"
#include "dsp_common.h"
#include "key.h"
#include "pgm_app.h"
#include "plu_edit.h"
#include "main.h"
#include "adm.h"
#include "key_typedef.h"
#include "prt_common.h"
#include "prtdrv.h"
#include "adm_app.h"
#include "plu_manager.h"
#include "dsp_app.h"
#include "key_app.h"
#include "flash_app.h"
#include "common.h"
#include "key_function.h"
#include "cursor.h"

#ifdef USE_INDIA_FUNCTION


INT8U ReadPluInventory(INT16U deptno, INT32U pluno, INT32S *inventory)
{
	PLU_SALES_SUMMARY_TYPE* tp;
	INT16U index;

	if(GlbOper.PluReportType) return OFF; 
	tp = FindPluSummary(0, pluno, deptno, 0, &index);
	if(tp == NULL) return OFF;
	*inventory = tp->pstInventory;

	return ON;
}

INT8U ReadPluInventoryBagWt(INT16U deptno, INT32U pluno, INT32S *inv, INT32S *bagWt)
{
	PLU_SALES_SUMMARY_TYPE* tp;
	INT16U index;

	if(GlbOper.PluReportType) return OFF; 
	tp = FindPluSummary(0, pluno, deptno, 0, &index);
	if(tp == NULL) return OFF;

	*inv = tp->pstInventory;
	*bagWt = tp->pstInvBagWt;

	return ON;
}

INT8U ReadPluBagWt(INT16U deptno, INT32U pluno, INT32S *bagWt)
{
	PLU_SALES_SUMMARY_TYPE* tp;
	INT16U index;

	if(GlbOper.PluReportType) return OFF; 
	tp = FindPluSummary(0, pluno, deptno, 0, &index);
	if(tp == NULL) return OFF;

	if(tp->pstInvBagWt == 0)
		return OFF;
	*bagWt = tp->pstInvBagWt;

	return ON;
}

INT8U ReadPluInventoryAll(INT16U deptno, INT32U pluno, INT32S *inv, INT32S *bagWt, INT32S *grossInv)
{
	PLU_SALES_SUMMARY_TYPE* tp;
	INT16U index;

	if(GlbOper.PluReportType) return OFF; 
	tp = FindPluSummary(0, pluno, deptno, 0, &index);
	if(tp == NULL) return OFF;

	*inv = tp->pstInventory;
	*bagWt = tp->pstInvBagWt;
	*grossInv = tp->pstInvGross;

	return ON;
}


INT8U WritePluInventoryBagWt(INT16U deptno, INT32U pluno, INT32S inventory, INT32S bagWt)
{
	PLU_SALES_SUMMARY_TYPE* tp;
	INT16U index;

	if(GlbOper.PluReportType) return OFF; 
	tp = FindPluSummary(1, pluno, deptno, 0, &index);
	if(tp == NULL) return OFF;

	tp->pstInventory = inventory;
	tp->pstInvBagWt = bagWt;
	set_nvram_plu_summary(NVRAM_PLU_SUMMARY, index, tp);

	return ON;
}


INT8U WritePluBagWt(INT16U deptno, INT32U pluno, INT32S bagWt)
{
	PLU_SALES_SUMMARY_TYPE* tp;
	INT16U index;

	if(GlbOper.PluReportType) return OFF; 
	tp = FindPluSummary(1, pluno, deptno, 0, &index);
	if(tp == NULL) return OFF;

	tp->pstInvBagWt = bagWt;
	set_nvram_plu_summary(NVRAM_PLU_SUMMARY, index, tp);

	return ON;
}

INT8U WritePluInventoryAll(INT16U deptno, INT32U pluno, INT32S inventory, INT32S bagWt, INT32S grossInv)
{
	PLU_SALES_SUMMARY_TYPE* tp;
	INT16U index;

	if(GlbOper.PluReportType) return OFF; 
	tp = FindPluSummary(1, pluno, deptno, 0, &index);
	if(tp == NULL) return OFF;

	tp->pstInventory = inventory;
	tp->pstInvBagWt = bagWt;
	tp->pstInvGross = grossInv;
	set_nvram_plu_summary(NVRAM_PLU_SUMMARY, index, tp);

	return ON;
}


INT8U AddPluInventory(INT16U deptno, INT32U pluno, INT32S inventory)
{
	PLU_SALES_SUMMARY_TYPE* tp;
	INT16U index;

	if(GlbOper.PluReportType) return OFF; 
	tp = FindPluSummary(1, pluno, deptno, 0, &index);
	if(tp == NULL) return OFF;

	tp->pstInventory += inventory;
	tp->pstInvGross += inventory;
	set_nvram_plu_summary(NVRAM_PLU_SUMMARY, index, tp);


	return ON;
}


INT8U AddPluInventoryBagWt(INT16U deptno, INT32U pluno, INT32S inventory, INT32S bagWt)
{
	PLU_SALES_SUMMARY_TYPE* tp;
	INT16U index;

	if(GlbOper.PluReportType) return OFF; 
	tp = FindPluSummary(1, pluno, deptno, 0, &index);
	if(tp == NULL) return OFF;

	tp->pstInventory += inventory;
	tp->pstInvBagWt = bagWt;
	tp->pstInvGross += inventory;
	set_nvram_plu_summary(NVRAM_PLU_SUMMARY, index, tp);


	return ON;
}


// check : INVENTORY_DEDUCT = deduction, INVENTORY_CHECK = check
INT8U DeductPluInventory(INT16U deptno, INT32U pluno, INT8U orgType, INT8U newType, INT32S weight, INT32S qty, INT8U check)
{
	PLU_SALES_SUMMARY_TYPE* tp;
	INT16U index;
	INT32S weightQty;
	INT32S bagWt;

	if(GlbOper.PluReportType) return OFF; 
	tp = FindPluSummary(1, pluno, deptno, 0, &index);
	if(tp == NULL) return OFF;

	weight = ad_cnv_weight_by_gram(weight);
	bagWt = tp->pstInvBagWt;
	if (bagWt == 0) bagWt = 1;

	if (orgType == PLU_BY_WEIGHT)
	{
		if (newType == PLU_BY_WEIGHT)
		{
			weightQty = weight;
		}
		else
		{
			weightQty = qty * bagWt; // qty->weight
		}
	}
	else
	{
		if (newType == PLU_BY_WEIGHT)
		{
			weightQty = weight * 10L / bagWt; // weight -> qty
			weightQty += 5L; // round 0.5
			weightQty /= 10L;
		}
		else
		{
			weightQty = qty;
		}
	}
	if (check == INVENTORY_CHECK)
	{
		if (weightQty > tp->pstInventory) return OFF;
		return ON;
	}
/*	if (tp->pstInventory > weightQty) 
	{
		tp->pstInventory -= weightQty;
	}
	else
	{
		tp->pstInventory = 0;
	}
*/
	tp->pstInventory -= weightQty; // 인도 요청사항으로 -값도 허용 하기로 함 
	set_nvram_plu_summary(NVRAM_PLU_SUMMARY, index, tp);

	return ON;
}



void DeletePluInventory(INT16U deptno, INT32U pluno)
{
	PLU_SALES_SUMMARY_TYPE* tp;
	INT16U index;

	tp = FindPluSummary(0, pluno, deptno, 0, &index);
	if(tp == NULL) return;

	//ClearPluCounter(tp);
	tp->pstInventory = 0;
	tp->pstInvBagWt = 0;
	tp->pstInvGross = 0;
	set_nvram_plu_summary(NVRAM_PLU_SUMMARY, index, tp);

}

void DeletePluInventoryAll(void)
{
	PLU_SALES_SUMMARY_TYPE pst;
	INT16U i;

	for (i=0;i< MAX_PLU_REPORT_NO;i++)
	{
		//tp = (PLU_SALES_SUMMARY_TYPE *)get_nvram_plu_summary(NVRAM_PLU_SUMMARY, i);
		get_nvram_plu_summary(NVRAM_PLU_SUMMARY, i, &pst);
		pst.pstInventory = 0;
		pst.pstInvBagWt = 0;
		pst.pstInvGross = 0;
		set_nvram_plu_summary(NVRAM_PLU_SUMMARY, i, &pst);
	}


}


void inventory_input(void)
{
	CAPTION_STRUCT cap;
	INT16U	result, old_font, key_mode;	    
	 
	INT16U  deptno;
	INT32U  pluno;
	INT32S  add_inventory;
	INT32S  inventory;
	INT16U  plu_addr;
	INT8U   plu_type;
	INT16U  size;
	INT8U   dec;
	INT8U   symbol;
	INT8U buffer[10];
	INT8U cur_id = 0;
	INT8U buf_size;


	key_mode=KEY_GetMode();
	KEY_SetMode(1);
	old_font=DspStruct.Id1;
	DspLoadFont1(DSP_MENU_FONT_ID);


	deptno = status_scale.departmentid;
	pluno = 1;
	while(1)
	{
		add_inventory = 0;
		
		Dsp_Fill_Buffer(0);
		display_title_page_change(0x1181, 1, 2);
		result=(INT16S)plu_input_department((INT16U *)&deptno, (INT32U *)&pluno, 0);

		if (result==2) goto RET_ESC;

		plu_table_search((INT8U)deptno, pluno, &plu_addr, 0);
		if (!plu_addr) {
			BuzOn(2);
			continue;
		}
		plu_get_field(plu_addr-1, PTYPE_PLU_TYPE, (INT8U *)&plu_type, (INT16S *)&size, sizeof(plu_type));
		
		if (plu_type == PLU_BY_WEIGHT)
		{
			symbol = CurPluWeightUnit + 1;
			dec = get_decimal_pt(6, 0);
			buf_size = 8;
		}
		else
		{
			symbol = 21;
			dec = 0;
			buf_size = 7;
		}
		
		if (!ReadPluInventory(deptno, pluno, &inventory))
		{
			inventory = 0;
		}

		while(1)
		{
			caption_split_by_code(0x1c61, &cap,0);
			
			display_clear(DISPLAY_UNITPRICE);
			display_string(DISPLAY_UNITPRICE,(INT8U *)cap.form);
			display_clear(DISPLAY_PRICE);
			format_fixed((HUGEDATA INT8U *)buffer, inventory, dec, buf_size, KeyInSet.char_decimal);
			buffer[buf_size] = '\0';
			display_string(DISPLAY_PRICE,(INT8U *)buffer);
			Dsp_Diffuse();

			while(!KEY_Read());
			switch(KeyDrv.Type) {
				case KEY_TYPE_FUNCTION :
					switch(KeyDrv.CnvCode) {
						case KP_ENTER :
							cur_id = 1;
							BuzOn(1);
							break;
						case KP_ESC :
							BuzOn(1);
							return;
						default :
							BuzOn(2);
							break;
					}
					break;
				default :
					BuzOn(2);
					break;
			}
			if (cur_id == 1)
				break;
		}

		Menu_Init_WithPage(1);
		caption_split_by_code(0x1181, &cap,0);
		Menu_HeadCreate((HUGEDATA char *)cap.form);

		caption_split_by_code(0x1c62, &cap,0);
		caption_adjust(&cap,NULL);
		Menu_InputCreate(1, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
							cap.input_max, cap.input_min, dec, (INT32U*)&add_inventory, NULL, NULL, NULL);
		Menu_SetSymbol(1, symbol);

		caption_split_by_code(0x1c61, &cap,0);
		caption_adjust(&cap,NULL);
		Menu_InputCreate(2, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
							cap.input_max, cap.input_min, dec, (INT32U*)&inventory, NULL, NULL, NULL);
		Menu_SetDisable(2, ON);


		result = Menu_InputDisplay();
		if(result == MENU_RET_SAVE) 
		{
			AddPluInventory(deptno, pluno, add_inventory);
		}
		else if(result == MENU_RET_ESC)
		{
			break;
		}
	}

RET_ESC:
	Dsp_Fill_Buffer(0);						// LCD Buffer clear
	DspLoadFont1(old_font);
	Dsp_Diffuse();
	KEY_SetMode(key_mode);
	sale_display_update(0x0fff);

}

void inventory_edit(void)
{
	CAPTION_STRUCT cap;
	INT16U	result, old_font, key_mode;	    
	 
	INT16U  deptno;
	INT32U  pluno;
	INT32S  inventory;
	INT32S  bagWeight;
	INT32S  grossInv;
	INT16U  plu_addr;
	INT8U   plu_type;
	INT16U  size;
	INT8U   dec;
	INT8U   wt_dec;
	INT8U   symbol;

	wt_dec = get_decimal_pt(6, 0);

	key_mode=KEY_GetMode();
	KEY_SetMode(1);
	old_font=DspStruct.Id1;
	DspLoadFont1(DSP_MENU_FONT_ID);


	deptno = status_scale.departmentid;
	pluno = 1;
	while(1)
	{    
		Dsp_Fill_Buffer(0);
		display_title_page_change(0x1181, 1, 2);
		result=(INT16S)plu_input_department((INT16U *)&deptno, (INT32U *)&pluno, 0);

		if (result==2) goto RET_ESC;

		plu_table_search((INT8U)deptno, pluno, &plu_addr, 0);
		if (!plu_addr) {
			BuzOn(2);
			continue;
		}
		plu_get_field(plu_addr-1, PTYPE_PLU_TYPE, (INT8U *)&plu_type, (INT16S *)&size, sizeof(plu_type));
		
		if (plu_type == PLU_BY_WEIGHT)
		{
			symbol = CurPluWeightUnit + 1;
			dec = wt_dec;
		}
		else
		{
			symbol = 21;
			dec = 0;
		}

		if (!ReadPluInventoryAll(deptno, pluno, &inventory, &bagWeight, &grossInv))
		{
			inventory = 0;
			bagWeight = 0;
			grossInv = 0;
		}

		Menu_Init_WithPage(2);
		caption_split_by_code(0x1182, &cap,0);
		Menu_HeadCreate((HUGEDATA char *)cap.form);

		caption_split_by_code(0x1c62, &cap,0);
		caption_adjust(&cap,NULL);
		Menu_InputCreate(1, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
							cap.input_max, cap.input_min, dec, (INT32U*)&inventory, NULL, NULL, NULL);
		Menu_SetSymbol(1, symbol);

		caption_split_by_code(0x1c63, &cap,0);
		caption_adjust(&cap,NULL);
		Menu_InputCreate(2, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
							cap.input_max, cap.input_min, wt_dec, (INT32U*)&bagWeight, NULL, NULL, NULL);

//		caption_split_by_code(0x1c64, &cap,0);
//		caption_adjust(&cap,NULL);
//		Menu_InputCreate(3, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
//							cap.input_max, cap.input_min, dec, &grossInv, NULL, NULL, NULL);
//		Menu_SetSymbol(3, symbol);
//		Menu_SetDisable(3, ON);


		result = Menu_InputDisplay();
		if(result == MENU_RET_SAVE) 
		{
			WritePluInventoryBagWt(deptno, pluno, inventory, bagWeight);
		}
		else if(result == MENU_RET_ESC)
		{
			break;
		}
	}

RET_ESC:
	Dsp_Fill_Buffer(0);						// LCD Buffer clear
	DspLoadFont1(old_font);
	Dsp_Diffuse();
	KEY_SetMode(key_mode);
	sale_display_update(0x0fff);	
}
#ifndef DISABLE_LIST_MENU
extern INT8U print_inventory_list(INT16S dept_no, long start_id, long end_id);
void inventory_list(void)
{
	CAPTION_STRUCT 	cap;
	POINT 		disp_p;
	INT8U	lcd_size;
	char	buffer[64];
	char	plu_name[40];
	INT8U	end_flag, input_line, plu_type;
	INT16S result, dept_no=1;
	INT32U  key;
	long    cur_id;
	INT16S     i,line,line_max;
	long	ret_long, plu_no;
	long	plu_num[2],line_id[4],nn[2];	// start_num = plu_num[0], end_num=plu_num[1];	
	INT32U	addr;
	INT16U	pluname_offset, plutype_offset, pluno_offset;
	INT16U xsize[3];
	INT8U   stype;
	INT16U  input_cap[]={0x3151,0x3152,0x1E02}; // "start PLU No", "end PLU no", 
	INT16S 	linenum;
	INT32U  input_min, input_max;
	INT8U   w_symbol[5];
	INT8U   p_symbol[5];
	INT8U   inventory_str[50];
	INT8U   string_buf[50];
	INT32S  inventory;
	INT8U   dec;
	INT8U   dec_ch;

	caption_split_by_code(input_cap[0], &cap,0);
	caption_adjust(&cap,NULL);
	input_min = cap.input_min;
	input_max = cap.input_max;

	dept_no=status_scale.departmentid;
	if ((dept_no<1) && (dept_no>99)) dept_no=1;

	lcd_size = Dsp_GetChars();
	pluname_offset=plu_get_ptype_point(PTYPE_PLU_NAME1,&stype,&xsize[0]);
	plutype_offset=plu_get_ptype_point(PTYPE_PLU_TYPE,&stype,&xsize[1]);
	pluno_offset  =plu_get_ptype_point(PTYPE_PLU_NO,&stype,&xsize[2]);
	plu_num[0] = input_min;
	plu_num[1] = input_max;


	dec = get_decimal_pt(3, (INT16U)ADM_Status.UseUnit);
	dec_ch = get_global_bparam(GLOBAL_DECIMAL_SIGN);
	GetWeightSymbol(ADM_Status.UseUnit, (char *)w_symbol);
	get_global_sparam(GLOBAL_PCS_SYMBOL, (HUGEDATA INT8U *)p_symbol, 4);


LIST_START:
	end_flag=1;
	Dsp_Fill_Buffer(0);
PAGE1_VIEW:
	display_title_page_change(0x1183,1,2);
	result=plu_input_department_only((INT16U *)&dept_no, 0);
	if (result==2) goto RET_ESC;
PAGE2_VIEW:
	Dsp_Fill_Buffer(0);
	display_title_page_change(0x1183, 2, 2);
	linenum = Startup.line_max;
	if (linenum > 3) linenum = 3;
	for (i = 0; i < linenum; i++)  {
		caption_split_by_code(input_cap[i], &cap,0);
		if (i!=2) caption_adjust(&cap,NULL);
		disp_p = point((i+Startup.menu_type)*Startup.menu_ygap,0);
		Dsp_Write_String(disp_p,(HUGEDATA char*)cap.form);
	}
	Dsp_Diffuse();
	input_line = 0;
PAGE2_KI_IN:
	for(i=input_line; i<2; i++)  {
		ret_long = plu_num[i];
		caption_split_by_code(input_cap[i], &cap,0);
		caption_adjust(&cap,NULL);
		disp_p = point((i+Startup.menu_type)*Startup.menu_ygap,0);
		Dsp_Write_String(disp_p,(HUGEDATA char*)cap.form);
		result=keyin_ulong(KI_GRAPHIC, KI_FORM_NORMAL,	0,(long*)&ret_long, cap.input_dlength,
			(i+Startup.menu_type)*Startup.menu_ygap,  cap.input_x*Startup.menu_xgap, 0,KI_NOPASS );
		nn[i]=ret_long;
		if(result == KP_ESC)
			goto RET_ESC;
		if ( ret_long > cap.input_max || ret_long < cap.input_min ) {
			BuzOn(2);
			error_display_form1(0, cap.input_min, cap.input_max);		
			goto PAGE2_KI_IN;	
		}
		plu_num[i] = ret_long;
		switch (result){
			case KP_ARROW_UP:  
				if(i == 0)	
					goto PAGE1_VIEW;
				input_line = 0;
				goto PAGE2_VIEW;

			case KP_PAGE_UP:
				goto PAGE1_VIEW;
			case KP_SAVE:	
				i = 2;
				plu_num[0] = input_min;
				plu_num[1] = input_max;
				nn[0] = plu_num[0];
				nn[1] = plu_num[1];
				break;

			case KP_ENTER:
			case KP_ARROW_DN:	   
				break;
					   				  				
		     	default:	
		     		BuzOn(2);	goto PAGE2_KI_IN;		     	 	
		} //switch (result)
	}//for(i=1; i<3; i++)
	key=plu_table_seq_search((INT16S)dept_no,(INT32U *)&plu_num[0],(INT32U)plu_num[1]);
	if(key==0) {
		plu_num[0]=nn[0];
		plu_num[1]=nn[1];
		error_display_caption(0xE00C);
		goto LIST_START;
	}
	cur_id   = plu_num[0];
READ_START:
	end_flag = 0;
	for (line=0; line<Startup.line_max; line++) {
		key=plu_table_seq_search(dept_no,(INT32U *)&cur_id,(INT32U)plu_num[1]);
 		if(key>0) {
			if (line == 0)  {
				Dsp_Fill_Buffer(0);
				if (Startup.menu_type) {
					caption_split_by_code(0x71FF, &cap,0);
					disp_p = point((Startup.menu_type-1) * Startup.menu_ygap, 2 * Startup.menu_xgap);
					Dsp_Write_String(disp_p,(HUGEDATA char*)cap.form);
				}
				line_id[0] = cur_id;
			}
			else if (line == 1) {
				line_id[1] = cur_id;
			}
			line_max      = line;
			addr = plu_calc_addr_by_key((long)(key-1));
			plu_name[0] = 0;
			plu_type = 0;
			plu_no = 0;
			if (xsize[0]) {
#if defined(USE_ARAB_FONT) || defined(HEBREW_FONT)
				Flash_sread(addr+pluname_offset,(HUGEDATA INT8U*)plu_name, 34);
				plu_name[34] = 0;
#else              
				Flash_sread(addr+pluname_offset,(HUGEDATA INT8U*)plu_name, 20);
#endif 
#if !defined(USE_ARAB_FONT) && !defined(HEBREW_FONT)
				plu_name[20] = 0;                
#endif                
			}
			if (xsize[1]) {
				plu_type = Flash_bread(addr+plutype_offset);
			}
			if (xsize[2]) {
				plu_no   = Flash_lread(addr+pluno_offset);
			}
#ifdef USE_INDIA_FUNCTION
			if (!ReadPluInventory(dept_no, cur_id, &inventory))
			{
				inventory = 0;
			}
			if (plu_type == PLU_BY_WEIGHT)
			{
				format_fixed((HUGEDATA INT8U *)string_buf, inventory, dec, 10, dec_ch);
				string_buf[10] = 0;
				sprintf((char*)inventory_str, "%s%s  ", string_buf, w_symbol);
			}
			else
			{
				sprintf((char*)inventory_str, " %9ld%s ", inventory, p_symbol);
			}
#endif
			if (plu_type==0) plu_type=1;

			if (Startup.menu_type)
			{
				sprintf(buffer, "%-5ld %1d", plu_no, plu_type);
#ifdef HEBREW_FONT
				Convert_HebrewPreview((INT8U *)plu_name, strlen(plu_name), 0, 0);								
				hebrew_codetemp[25-8] = 0;
#elif defined(USE_ARAB_FONT)	//기존 아랍ROM 적용되어 있음
				Convert_ArabPreview((INT8U *)plu_name, strlen(plu_name), 0, 0, 0);								
				arab_codetemp[25-8] = 0;
#endif
			}
			else
			{	   
				sprintf(buffer, "%-5ld %s %s", plu_no, inventory_str, plu_name);
			}
			buffer[lcd_size] = 0;
			disp_p = point((line+Startup.menu_type)*Startup.menu_ygap, 0);
#ifdef HEBREW_FONT
			disp_p.x += (25-1)*8;//(strlen(buffer_tmp)-1)*8;
			Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_FONT)	//기존 아랍ROM 적용되어 있음
			disp_p.x += (25-1)*8;//(strlen(buffer_tmp)-1)*8;
			Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
			Dsp_Write_String(disp_p,(HUGEDATA char *)buffer);
#endif
		} else {
			//line_id[line]=0;
			if(line == 0) {
				error_display_caption(0xE00E);
				end_flag=1;
				//break;
			}
			break;
		}
		if (cur_id>=plu_num[1]) {
			end_flag=1;
			break;
		}
		cur_id++;
	}
	Dsp_Diffuse();
KEY_WAIT:
	while (1) {
		while(!KEY_Read());		BuzOn(1);
		if(KeyDrv.CnvCode == KP_ESC)	goto RET_ESC;
		switch(KeyDrv.CnvCode) {
			case KP_ARROW_UP: cur_id=line_id[0];
					cur_id--;
					if (cur_id<=0) break;
					key=plu_table_prev_search(dept_no,(INT32U *)&cur_id,(INT32U)plu_num[0]);
					if (cur_id<plu_num[0]) cur_id=plu_num[0];
					if (key) goto READ_START;
					break;
			case KP_PAGE_UP :
					cur_id = line_id[0];
					for (line=0; line<Startup.line_max; line++) {
						cur_id--;
						key=plu_table_prev_search(dept_no,(INT32U *)&cur_id,(INT32U)plu_num[0]);
					}
					if (cur_id<plu_num[0]) cur_id=plu_num[0];
					goto READ_START;
			case KP_PAGE_DN :
			case KP_ENTER   :
					if(end_flag == 1)  {
						error_display_caption(0xE00E); 
						goto KEY_WAIT;
					} else {
//						cur_id ++;
						goto  	READ_START;
					}
			case KP_ARROW_DN:
					//cur_id=line_id[line_max];
					if(line_max < 1) {
						error_display_caption(0xE00E); 
						goto KEY_WAIT;
					}						
					cur_id=line_id[1];
					if (cur_id<=0) cur_id=1;
//					cur_id++;
					key=plu_table_seq_search(dept_no,(INT32U *)&cur_id,(INT32U)plu_num[1]);
					if (key) goto READ_START;
					break;
			case KP_TARE:	
			case KP_TEST:	
					print_inventory_list(dept_no, plu_num[0], plu_num[1]);
					goto KEY_WAIT;
			case KP_FEED:	keyapp_feed(OFF);
					goto KEY_WAIT;
			default 	:
					BuzOn(1);
					goto KEY_WAIT;
		}
	}
RET_ESC:
	Dsp_Fill_Buffer(0);
	Dsp_Diffuse();
}
#endif

#endif

