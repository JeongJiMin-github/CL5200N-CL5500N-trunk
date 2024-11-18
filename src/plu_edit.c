/*****************************************************************************
|*  Copyright		:	(c) 2004 CAS
|*  Filename		:	plu_edit.c
|*  Version		:	v 0.9
|*  Programmer(s)	:	getwing (Jae-Ok Kim)
|*  Created		:	2004/03/18
|*  edit		:	2004.05.28 CJK,LBM
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
#include "prt_common.h"
#include "prt_interpreter.h"
#include "prt_cmd.h"
#include "prt_list.h"
#include "dsp_vfd7.h"
#include "adm.h"
#include "key.h"
#include "key_typedef.h"
#include "main.h"
#include "cal.h"
#include "debug.h"
#include "menu.h"
#include "initial.h"
#include "common.h"
#include "mode_main.h"
#include "mode_error.h"
#include "plu_device.h"
#include "plu_manager.h"
#include "flash_app.h"
#include "caption_app.h"
#include "key_app.h"
#include "dsp_app.h"
#include "ingredient.h"
#include "discount.h"
#include "plu_auxcall.h"
#include "pgm_app.h"
#include "adm_app.h"
#include "department.h"
#include "plu_edit.h"
#include "prt_cmdtest.h"
#include "network_command.h"
#include "ethernet_comm.h"
#include "clerk_app.h"
#include "login.h"
#include "origin.h"
#include "global_set.h"
#include "mode_commun.h"
#include "initial_glb.h"
#include "tax.h"		//Added by JJANG 20070619
#include "traceability.h"	//insert by se-hyung for function traceability key 20070706
#include "nutrition_fact.h"

extern INT8U g_i8uInputDeptReturn;
extern INT16S    		plu_record_size;
extern INT16U 		plu_n_field;
extern long		speedkey_set_number;
extern long		speedkey_plu_number;
extern long		speedkey_key_number;
extern INT16S		Misc_tax;
extern INT16S		plucall_plu_position_x, plucall_plu_position_y;
extern INT16S		keycall_key_position_y;
extern long             skeyin_define;
extern INT16U		callback_dept_no;
extern INT8U 		temp_callback_discount_flag;
extern BOOLEAN		keyin_get_speed_key;
extern PRICE_TYPE 	Price;
extern char gmsg[];
INT8U	temp_plu_input_action_key=0;
extern INT8U   plu_base_change;
extern  NETWORK_STATUS network_status;
void    temp_number_of_linked_plu(INT16S linked_num);
extern  long str_2sellbydate(char *str);
extern  void date_2str(INT32U date,char *str);
extern INT32U plu_table_seq_search(INT16S deptno,INT32U *pluno,INT32U max_id);
extern INT16U plu_ptype_reset(void);
extern void plucall_fixed(long num);
extern void keyapp_feed(INT16S beepFlag);
extern void Flash_flush_nvbuffer_c2(void);
extern void transaction_sale_store(TRANSACTION_RECORD_TYPE *pTrans, TRANSACTION_RECORD_SUB_TYPE *pTrans_sub, TR_ADDITION_DATA_TYPE* pTrans_add, UNITPRICE_CHANGE_TYPE *pChgPrice, INT8U mode, INT8U sendflag);


INT32U plu_get_display_info(INT16U id,CAPTION_STRUCT *cap,INT8U x)
{
	INT32U pnt_dept,vdefault;
	char   str[64];
	INT16S    w;
	INT8U  p;
	INT8U  selldate_flag;
#ifdef USE_THAI_FONT    
	INT8U dspstrlen;
#endif

	vdefault=plu_struct_addr_nameid(id,&pnt_dept);
	if (vdefault==0) {
		cap->code=0xffff;
		strcpy(cap->form,"no plu str");
		return 0;
	}
	str[0]=(INT32U)Flash_bread(pnt_dept+PLUSTR_STYPE_POINT);
	if (str[0]!='S') cap->input_itype  = 'N';
	else             cap->input_itype  = 'S';
	Flash_sread(vdefault,(HUGEDATA INT8U*)str, PLUSTR_NAME_SIZE);
	vdefault    = Flash_lread(pnt_dept+PLUSTR_VDEFAULT_POINT);
	cap->input_dlength=(INT16U)Flash_bread(pnt_dept+PLUSTR_LENGTH_POINT);
	cap->input_max    =(INT32U)Flash_lread(pnt_dept+PLUSTR_MAXVALUE_POINT);
	cap->input_min    = 1;
	//cap->input_min = vdefault;	// CJK071020
	cap->input_length = cap->input_dlength;
	memcpy(cap->form,str,32); cap->form[31]=0;
#if !defined(USE_SINGLE_LINE_GRAPHIC)
	if (!x) {
#ifdef HEBREW_FONT
		strcat((char *)cap->form,":]");
#else
		strcat((char *)cap->form,":[");
#endif
	}
  #ifdef USE_THAI_FONT
		dspstrlen = Cvt_ThaiStrLength((INT8U*)cap->form);
  #endif  
#endif
	cap->input_x = strlen((char *)cap->form);
	p=0;
	if ((id==5) || (id==100) || (id==13))     {
		cap->input_dlength=get_global_bparam(GLOBAL_DISPLAY_WEIGHTLENGTH);
		//p = get_global_bparam(GLOBAL_DISPLAY_WEIGHTDECIMAL);
		p = get_decimal_pt(6, 0);
	} else if (id==6){
		cap->input_dlength=get_global_bparam(GLOBAL_DISPLAY_UNITPRICELENGTH);
		p = get_global_bparam(GLOBAL_AREA_PRICE_DECIMALPOINT);
	} else if (id==25) {
		cap->input_dlength=4;
		if (MAX_INGREDIENT_NO < 1000) cap->input_dlength=3;
	} else if (id==16) {	// sell by date
#ifdef USE_SELLBYDATE_CHANGE
		selldate_flag = 1;
#else
		selldate_flag = get_global_bparam(GLOBAL_SALEDATE_TYPE);
#endif
		if(selldate_flag) {
			cap->input_dlength=6;
			cap->input_length =6;
			cap->input_max    =999999;
		}
	}
#if !defined(USE_SINGLE_LINE_GRAPHIC)
	if (!x) {
		for (w=0; w<cap->input_dlength; w++) cap->form[w+cap->input_x]=0x20;
		w=cap->input_x+cap->input_dlength;
		if (p) cap->form[w++]=0x20;
#ifdef HEBREW_FONT
		cap->form[w++]='[';
#else
		cap->form[w++]=']';
#endif
		cap->form[w  ]=0;
	}
	
  #ifdef USE_THAI_FONT
    cap->input_x = dspstrlen;
  #endif    
#endif
	return vdefault;

}

INT8U plu_input_department(INT16U *deptno,INT32U _PACKED_DEF *pluno, INT8U dc)
{
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
	POINT	disp_p;
	char	str[64],format[64];
	INT16S		dlength[2],length;
	INT16U	ptype[2];
	INT16S		xpos[ 2];
	INT16S		sl,disp_length, q,result, dept_code,start_line;
	INT32U	addr;
	long	pnt_dept[2],pnt_name[2],vdefault[2],ret_long;//,maxvalue[2];
	INT8U	backup_directdraw;

	backup_directdraw = DspStruct.DirectDraw;
	DspStruct.DirectDraw = 0;
	//start_line=1;
	//if (Startup.menu_type==0) start_line=0;
	start_line = Startup.menu_type;
	pnt_name[0]=(long)plu_struct_addr_nameid(1,(INT32U *)&pnt_dept[0]);
	pnt_name[1]=(long)plu_struct_addr_nameid(2,(INT32U *)&pnt_dept[1]);
	if (Startup.disable_dept) {
		(*deptno)=1;
	}
	if (*deptno) vdefault[0]=*deptno;
	if (*pluno ) vdefault[1]= *pluno;
	else {	// Added by CJK 20051026
		vdefault[1]=get_nvram_lparam(NVRAM_PLU_LASTNUMBER)+1;
//SG070315		if (vdefault[1]>=99999L) vdefault[1]=1L;
		if (vdefault[1]>MAX_PLU_NUMBER) vdefault[1]=1L;
	}

	for (q=0; q<2; q++) {
		Flash_sread(pnt_name[q],(HUGEDATA INT8U*)str, PLUSTR_NAME_SIZE);
		ptype[q]	= Flash_wread(pnt_dept[q]+PLUSTR_PTYPE_POINT);
		//vdefault[q] = Flash_lread(pnt_dept[q]+PLUSTR_VDEFAULT_POINT);
		//maxvalue[q] = Flash_lread(pnt_dept[q]+PLUSTR_MAXVALUE_POINT);
		length		= Flash_bread(pnt_dept[q]+PLUSTR_LENGTH_POINT);
#ifdef USE_ARAB_CONVERT
		sl = Convert_ArabStrLength(str);
#elif defined(USE_THAI_FONT)
        sl = Cvt_ThaiStrLength((INT8U*)str); 
#else
		sl=strlen(str);
#endif
		disp_length=Dsp_GetChars();
		dlength[q]=length;
		if (length+sl+4>disp_length) dlength[q]=disp_length-sl-4;
		switch(q) {
			case 0: //sprintf(format,": [%c%ds]",'%',dlength[q]); break;
#ifdef HEBREW_FONT
			case 1: sprintf(format,":]%c%ds[",'%',dlength[q]); break;
#else
			case 1: sprintf(format,":[%c%ds]",'%',dlength[q]); break;
#endif
		}
		if (Startup.disable_dept) {
			if (q==0) goto DM1;
		}
		sprintf(&str[strlen(str)],format," ");
		disp_p = point((q+start_line)*Startup.menu_ygap, 0);
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
		Dsp_Write_String(disp_p, str);
#endif
	   DM1:
		if (q==0) {
			if (vdefault[0]) ret_long = vdefault[0];
		} else {
			if (vdefault[1]==0) {
				vdefault[1] = get_nvram_lparam(NVRAM_PLU_LASTNUMBER)+1;
//SG070315				if (vdefault[1]>=99999L) vdefault[1]=1L;
				if (vdefault[1]>MAX_PLU_NUMBER) vdefault[1]=1L;
				//ret_long++;
				//if (ret_long>=99999L) ret_long=1L;
			}
			ret_long = vdefault[1];
		}
		//sl+=3;
		sl+=2;	// 3 => 2 : space 1칸 제거
		switch(q) {
			case 0: xpos[q]=sl;//*Startup.menu_xgap;
				//plucall_department_position=(strlen(str)+1);
				//plucall_department_pos_y=start_line;
				CallbackEditPosition_x = strlen(str) + 1;
				CallbackEditPosition_y = start_line;
				keyin_setcallback(plucall_department);
				break;
			case 1: xpos[q]=(sl);//*Startup.menu_xgap;
				//plucall_plu_position_x=(strlen(str)+1);
				//plucall_plu_position_y=start_line+1;
				CallbackEditPosition_x = strlen(str) + 1;
				CallbackEditPosition_y = start_line + 1;
				if (dc) {
					keyin_setcallback(plucall_plu_dcnumber);
				} else {
					keyin_setcallback(plucall_plu_number);
				}
				break;
		}
		// ]
		if (Startup.disable_dept) {
			if (q==0) goto DM2;
		}
#if defined(USE_ARAB_CONVERT) || defined(HEBREW_FONT)
		result = keyin_ulong(KI_GRAPHIC, KI_FORM_ILNN,
			0, (long*)&ret_long, dlength[q], (q+start_line)*Startup.menu_ygap, xpos[q]*Startup.menu_xgap, 0,KI_PASS);
#else
		result = keyin_ulong(KI_GRAPHIC, KI_FORM_NORMAL,
			0, (long*)&ret_long, dlength[q], (q+start_line)*Startup.menu_ygap, xpos[q]*Startup.menu_xgap, 0,KI_PASS);
#endif
	DM2:	keyin_clearcallback();
	}	
	Dsp_Diffuse();
	DspStruct.DirectDraw = 1;
//	if (Startup.disable_dept) *deptno=1;
//	if (*deptno) vdefault[0]=*deptno;
//	if (*pluno ) vdefault[1]= *pluno;
//	else {	// Added by CJK 20051026
//		vdefault[1]=get_nvram_lparam(NVRAM_PLU_LASTNUMBER)+1;
//		if (vdefault[1]>=99999L) vdefault[1]=1L;
//	}
	q=0;
	while (1) {
PAGE_KI_IN:
		if (Startup.disable_dept) {
			if (q==0) {
				vdefault[0]= 1;
				ret_long   = 1;
				result	   = KP_ENTER;
				goto INPUTSKIP;
			}
		}
		if (ptype[q]==1)
		{
			CallbackEditPosition_x = xpos[q] + dlength[q] + 2;
			CallbackEditPosition_y = (q+start_line);
			keyin_setcallback(plucall_department);
		}
		else 
		{
			CallbackEditPosition_x = xpos[q] + dlength[q] + 2;
			CallbackEditPosition_y = (q+start_line);
			if (dc) {
				keyin_setcallback(plucall_plu_dcnumber);
			} else {
				keyin_setcallback(plucall_plu_number);
			}
		}
		ret_long=vdefault[q];
#if defined(USE_ARAB_CONVERT) || defined(HEBREW_FONT)
		result = keyin_ulong(KI_GRAPHIC, KI_FORM_ILNN,
				0, (long*)&ret_long, dlength[q], (q+start_line)*Startup.menu_ygap, xpos[q]*Startup.menu_xgap, 0,KI_NOPASS);
#else
		result = keyin_ulong(KI_GRAPHIC, KI_FORM_NORMAL,
				0, (long*)&ret_long, dlength[q], (q+start_line)*Startup.menu_ygap, xpos[q]*Startup.menu_xgap, 0,KI_NOPASS);
#endif
		keyin_clearcallback();
INPUTSKIP:
		if (result==KP_ESC) 
		{
			DspStruct.DirectDraw = backup_directdraw;
			return 2;
		}
		vdefault[q]=ret_long;
		if (q==0) {
			addr =	DFLASH_BASE + FLASH_DEPARTMENT_AREA;
			addr = addr + DEPARTMENT_STRUCT_SIZE * (ret_long-1);
			dept_code = Flash_wread(addr);
			if(dept_code != ret_long) {
				 BuzOn(2);
				 goto PAGE_KI_IN;
			}
		}
		switch (result) {
			case KP_SAVE:
				if (q==0) {
					*deptno = (INT16U)vdefault[0];						
					*pluno	= (INT32U)vdefault[1];
					if (*deptno < 1) *deptno = 1;
					if (*pluno	< 1) *pluno = 1;
					goto END;
				}
			case KP_ENTER:
			case KP_ARROW_DN:
			case KP_PAGE_DN : if (q==1) {
						*deptno = (INT16U)vdefault[0];						
						*pluno	= (INT32U)vdefault[1];
						if (!g_i8uInputDeptReturn || result == KP_ENTER)
							goto END;
						else {
							BuzOn(2);
							goto PAGE_KI_IN;
						}

					  } else {
						//if (vdefault[1]==0) {
						//	vdefault[1]=get_nvram_lparam(NVRAM_PLU_LASTNUMBER)+1;
						//	if (vdefault[1]>=99999L) vdefault[1]=1L;
						//}
						q++;
					  }
					  break;
			case KP_ARROW_UP: 
			case KP_PAGE_UP:  if (q==0) 
					  {
						BuzOn(2); 
						goto PAGE_KI_IN;
					  }
					  else if (q==1) {
						if (Startup.disable_dept) {
							BuzOn(2); 
							goto PAGE_KI_IN;
						}
						q--;
					  }
					  break;
			// ]
			case KP_ESC 	: 
				DspStruct.DirectDraw = backup_directdraw;
				return 2;
		}
	}
END:
	DspStruct.DirectDraw = backup_directdraw;
	return result;
#else
	CAPTION_STRUCT 		cap;
	INT16U 	result;

	INT32U	dept_no;
	INT32U	plu_no;

	dept_no = (INT32U)*deptno;
	plu_no  = *pluno;
	if (*pluno ) plu_no = *pluno;
	else {
		plu_no = get_nvram_lparam(NVRAM_PLU_LASTNUMBER) + 1;
		if (plu_no > MAX_PLU_NUMBER) plu_no = 1L;
	}

	Menu_Init();

	//caption_split_by_code(0x1110, &cap,0);
	//Menu_HeadCreate((HUGEDATA char *)cap.form);

	if (Startup.disable_dept)
	{
		dept_no = 1;
	}
	else
	{
		plu_get_display_info(PTYPE_DEPARTMENT_NO, &cap, 0);
		Menu_InputCreate(1, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
						cap.input_max, cap.input_min, 0, &dept_no, NULL, NULL, plucall_department);
	}

	plu_get_display_info(PTYPE_PLU_NO, &cap, 0);
	Menu_InputCreate(2, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
					cap.input_max, cap.input_min, 0, &plu_no, NULL, NULL, plucall_plu_number);
	Menu_AddKeyFunction(2, KP_TEST, display_remain_plu);

	result = Menu_InputDisplay();
	if(result == MENU_RET_SAVE)
	{
		*deptno = dept_no;
		*pluno = plu_no;
		result = KP_ENTER;
	}
	else if (result == MENU_RET_ESC)
	{
		result = 2;
	}
	else if (result == MENU_RET_EXIT)
	{
		result = 2;
	}

	return result;
#endif
}

void display_remain_plu()
{
	POINT disp_p;
	long rem;
	char disp_string[64];

	rem = MAX_PLU_NO-plu_rem_number_get();
	sprintf(disp_string,"REMAIN:%05d",rem);
	disp_p.x = 0;
	disp_p.y = 0;
#ifdef HEBREW_FONT
		Convert_HebrewPreview((INT8U *)disp_string, strlen(disp_string), 0, 0);
		hebrew_codetemp[strlen(disp_string)] = 0;
		disp_p.x = 202 - disp_p.x;
		Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#else
	Dsp_Write_String(disp_p, disp_string);
#endif
//	display_string(DISPLAY_UNITPRICE, (INT8U *)disp_string);
	VFD7_Diffuse();
	Delay_100ms(10); // "remain PLU 표시를 위해 1초 지연
}

INT16U plu_input_department_only(INT16U *deptno, INT8U allow)
{
	POINT 	disp_p;
	INT16S  	result, dept_code,start_line;
	INT32U  addr;
	long 	vdefault, ret_long;
	CAPTION_STRUCT 	cap;
	INT8U 	backup_directdraw;

	backup_directdraw = DspStruct.DirectDraw;
	DspStruct.DirectDraw = 0;

	//start_line=1;
	start_line = Startup.menu_type;
	//if (Startup.menu_type==0) start_line=0;
	if (Startup.disable_dept) {
		(*deptno)=1;
		return KP_ENTER;
	}
	vdefault=plu_get_display_info(PTYPE_DEPARTMENT_NO,&cap,0);
#if defined(USE_SINGLE_LINE_GRAPHIC)
	display_clear(DISPLAY_UNITPRICE);
	display_string(DISPLAY_UNITPRICE,(INT8U *)cap.form);
#else
	disp_p = point((start_line)*Startup.menu_ygap, 0);
#ifdef HEBREW_FONT
	Convert_HebrewPreview((INT8U *)cap.form, strlen((char *)cap.form), 0, 0);
	hebrew_codetemp[strlen((char *)cap.form)] = 0;
	disp_p.x = 202 - disp_p.x;
	Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
	Convert_ArabPreview((INT8U *)cap.form, strlen((char *)cap.form), 0, 0, 0);
	arab_codetemp[strlen((char *)cap.form)] = 0;
	disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
	Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
	Dsp_Write_String(disp_p,(HUGEDATA char*)cap.form);
#endif
#endif

	ret_long = 1;
	if (*deptno) ret_long = *deptno;
#ifdef USE_SINGLE_LINE_GRAPHIC
	CallbackEditPosition_x = 0;
	CallbackEditPosition_y = 0;
#else
  #ifdef USE_ARAB_CONVERT
    CallbackEditPosition_x = 0;
	CallbackEditPosition_y = start_line-1; // plucall_department 에서 y 좌표값 +2를 해주기 대문에, 여기서 -1 을 해줘야 다음줄에 인쇄 
  #else
	CallbackEditPosition_x = (strlen((char *)cap.form)+1);
	CallbackEditPosition_y = start_line;
  #endif
#endif
	keyin_setcallback(plucall_department);
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
	result = keyin_ulong(KI_GRAPHIC, KI_FORM_NORMAL,
			     0, (long*)&ret_long, cap.input_dlength, (start_line)*Startup.menu_ygap,cap.input_x*Startup.menu_xgap, 0,KI_PASS);
#else
	result = keyin_ulong(KI_DEFAULT, KI_FORM_NORMAL,
			     0, (long*)&ret_long, cap.input_dlength, (start_line)*Startup.menu_ygap,cap.input_x*Startup.menu_xgap, 0,KI_PASS);
#endif
	keyin_clearcallback();
	Dsp_Diffuse();
	DspStruct.DirectDraw = 1;

	if (*deptno) ret_long=(*deptno);
	else ret_long=vdefault;
	while (1) {
PAGE_KI_IN:
#ifdef USE_SINGLE_LINE_GRAPHIC
	CallbackEditPosition_x = 0;
	CallbackEditPosition_y = 0;
#else
  #ifdef USE_ARAB_CONVERT
    CallbackEditPosition_x = 0;
	CallbackEditPosition_y = start_line-1;
  #else
	CallbackEditPosition_x = (strlen((char *)cap.form)+1);
	CallbackEditPosition_y = start_line;
  #endif
#endif
		keyin_setcallback(plucall_department);
#if defined(USE_ARAB_CONVERT) || defined(HEBREW_FONT)
		result = keyin_ulong(KI_GRAPHIC, KI_FORM_ILNN,
				0, (long*)&ret_long,cap.input_dlength, (start_line)*Startup.menu_ygap,cap.input_x*Startup.menu_xgap, 0,KI_NOPASS);
#else
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
		result = keyin_ulong(KI_GRAPHIC, KI_FORM_NORMAL,
				0, (long*)&ret_long,cap.input_dlength, (start_line)*Startup.menu_ygap,cap.input_x*Startup.menu_xgap, 0,KI_NOPASS);
#else
		result = keyin_ulong(KI_DEFAULT, KI_FORM_NORMAL,
				0, (long*)&ret_long,cap.input_dlength, (start_line)*Startup.menu_ygap,cap.input_x*Startup.menu_xgap, 0,KI_NOPASS);
#endif
#endif
		keyin_clearcallback();
		if (result==KP_ESC)
		{
			DspStruct.DirectDraw = backup_directdraw;
			return 2;
		}
		if (!allow) {
			addr =  DFLASH_BASE + FLASH_DEPARTMENT_AREA;
 			addr = addr + DEPARTMENT_STRUCT_SIZE * (ret_long-1);
			dept_code = Flash_wread(addr);
			if (dept_code != ret_long) {
				 BuzOn(2);	goto PAGE_KI_IN;
			}
		}
		switch (result) {
			case KP_ARROW_DN:
			case KP_PAGE_DN:
					// <CODE> JHC, 040702, ADD
					// [
					if (g_i8uInputDeptReturn)
					{
						BuzOn(2);
						goto PAGE_KI_IN;
					}
					// ]
			case KP_ENTER:
			case KP_SAVE:
					*deptno = (INT16U)ret_long;
					DspStruct.DirectDraw = backup_directdraw;
					return result;
			case KP_ARROW_UP:
			case KP_PAGE_UP:  BuzOn(2); goto PAGE_KI_IN;
			case KP_ESC     :
				DspStruct.DirectDraw = backup_directdraw;
				return 2;
			default: BuzOn(2); break;
		}
	}
}



INT32U MenuPrevPluType = 0;
INT32U MenuPrevUnitWeight = 0;
INT32U MenuPrevNoLinkPlu = 0;
void callback_end_plutype(INT32U num)
{
	INT8U id_unitweight;//5
	INT8U id_tareno;//12
	INT8U id_tare;//13
	INT8U id_pcs;//14
	INT8U id_qtyno;//15
	INT8U id_pertarelimit;//23
	INT8U id_pertare;//24
	INT8U id_fixedweight;//100
#ifdef USE_PLU_MULTIPLE_PRICE
	INT8U id_fTarget;//69
	INT8U id_sTarget;//65
	INT8U id_fPrice;//68
	INT8U id_sPrice;//66
#endif
	INT8U use_memberprice_flag;

	use_memberprice_flag = get_global_bparam(GLOBAL_SALE_SETUP_FLAG18) & 0x80;	//parameter 729

	id_unitweight = Menu_GetMenuId(5);
	id_tareno = Menu_GetMenuId(12);
	id_tare = Menu_GetMenuId(13);
	id_pcs = Menu_GetMenuId(14);
	id_qtyno = Menu_GetMenuId(15);
	id_pertarelimit = Menu_GetMenuId(23);
	id_pertare = Menu_GetMenuId(24);
	id_fixedweight = Menu_GetMenuId(100);
#ifdef USE_PLU_MULTIPLE_PRICE
	id_fTarget = Menu_GetMenuId(69);
	id_sTarget = Menu_GetMenuId(65);
	id_fPrice = Menu_GetMenuId(68);
	id_sPrice = Menu_GetMenuId(66);
#endif
	switch(num) {
		case PLU_BY_WEIGHT:
			if(id_unitweight != 0xff)   Menu[id_unitweight].inhibit = OFF;
			if(id_tareno != 0xff)       Menu[id_tareno].inhibit = OFF;
			if(id_tare != 0xff)         Menu[id_tare].inhibit = OFF;
#ifdef USE_PCS_FONT5_TO_TRACE_CODE	// USE PCS, PLUFONT5 field for Trace CODE 
			if(id_pcs != 0xff)			Menu[id_pcs].inhibit = OFF;
#else
			if(id_pcs != 0xff)			Menu[id_pcs].inhibit = ON;
#endif
			if(id_qtyno != 0xff)        Menu[id_qtyno].inhibit = ON;
			if(id_pertarelimit != 0xff) Menu[id_pertarelimit].inhibit = OFF;
			if(id_pertare != 0xff)      Menu[id_pertare].inhibit = OFF;
			if(id_fixedweight != 0xff)  Menu[id_fixedweight].inhibit = OFF;
#ifdef USE_PLU_MULTIPLE_PRICE
			if(id_fTarget != 0xff)	Menu[id_fTarget].inhibit = OFF;	//해당 필드 표시
			if(id_sTarget != 0xff)	Menu[id_sTarget].inhibit = OFF;
			if(id_fPrice != 0xff)  Menu[id_fPrice].inhibit = OFF;
			if(id_sPrice != 0xff)  Menu[id_sPrice].inhibit = OFF;
#endif
			//temp_plu_inhibit_list[14]=14;
			//temp_plu_inhibit_list[15]=15;
			////plu_tare_inhibit();
			//sale_flag=get_global_bparam(GLOBAL_SALE_SETUP_FLAG2);
			//if (sale_flag&0x02) {
			//	temp_plu_inhibit_list[12]=0;
			//	temp_plu_inhibit_list[13]=13;
			//} else {
			//	temp_plu_inhibit_list[12]=12;
			//	temp_plu_inhibit_list[13]=0;
			//}
			break;
		case PLU_BY_PCS:
			if(id_unitweight != 0xff)   Menu[id_unitweight].inhibit = ON;
			if(id_tareno != 0xff)       Menu[id_tareno].inhibit = ON;
			if(id_tare != 0xff)
			{
#ifndef USE_SPAR_MEMBER_PRICE
				if(use_memberprice_flag) Menu[id_tare].inhibit = OFF;
				else Menu[id_tare].inhibit = ON;
#else
                Menu[id_tare].inhibit = ON;
#endif
			}
			if(id_pcs != 0xff)          Menu[id_pcs].inhibit = OFF;
			if(id_qtyno != 0xff)        Menu[id_qtyno].inhibit = OFF;
			if(id_pertarelimit != 0xff) Menu[id_pertarelimit].inhibit = ON;
			if(id_pertare != 0xff)      Menu[id_pertare].inhibit = ON;
			if(id_fixedweight != 0xff)  Menu[id_fixedweight].inhibit = OFF;
#ifdef USE_PLU_MULTIPLE_PRICE
			if(id_fTarget != 0xff)  Menu[id_fTarget].inhibit = ON;	//해당 필드 표시X
			if(id_sTarget != 0xff)  Menu[id_sTarget].inhibit = ON;
			if(id_fPrice != 0xff)  Menu[id_fPrice].inhibit = ON;
			if(id_sPrice != 0xff)  Menu[id_sPrice].inhibit = ON;
#endif
			//temp_plu_inhibit_list[5] = 5;	//id_unitweight
			//temp_plu_inhibit_list[12]=12;	//id_tareno
			//temp_plu_inhibit_list[13]=13;	//id_tare
			//temp_plu_inhibit_list[14]=0;	//id_pcs
			//temp_plu_inhibit_list[15]=0;	//id_qtyno
			//temp_plu_inhibit_list[23]=23;	//id_pertarelimit
			//temp_plu_inhibit_list[24]=24;	//id_pertare
			//temp_plu_inhibit_list[100]=100;//id_fixedweight
			break;
		case PLU_BY_COUNT:
			if(id_unitweight != 0xff)   Menu[id_unitweight].inhibit = ON;
			if(id_tareno != 0xff)       Menu[id_tareno].inhibit = ON;
			if(id_tare != 0xff)
			{
#ifndef USE_SPAR_MEMBER_PRICE
				if(use_memberprice_flag) Menu[id_tare].inhibit = OFF;
				else Menu[id_tare].inhibit = ON;
#else
                Menu[id_tare].inhibit = ON;
#endif
			}
#ifdef USE_PCS_FONT5_TO_TRACE_CODE	// USE PCS, PLUFONT5 field for Trace CODE 
			if(id_pcs != 0xff)			Menu[id_pcs].inhibit = OFF;
#else
			if(id_pcs != 0xff)			Menu[id_pcs].inhibit = ON;
#endif
			if(id_qtyno != 0xff)        Menu[id_qtyno].inhibit = OFF;
			if(id_pertarelimit != 0xff) Menu[id_pertarelimit].inhibit = ON;
			if(id_pertare != 0xff)      Menu[id_pertare].inhibit = ON;
			if(id_fixedweight != 0xff)  Menu[id_fixedweight].inhibit = OFF;
#ifdef USE_PLU_MULTIPLE_PRICE
			if(id_fTarget != 0xff)	Menu[id_fTarget].inhibit = OFF;	//해당 필드 표시
			if(id_sTarget != 0xff)	Menu[id_sTarget].inhibit = OFF;
			if(id_fPrice != 0xff)  Menu[id_fPrice].inhibit = OFF;
			if(id_sPrice != 0xff)  Menu[id_sPrice].inhibit = OFF;
#endif
			//temp_plu_inhibit_list[5] = 5;
			//temp_plu_inhibit_list[12]=12;
			//temp_plu_inhibit_list[13]=13;
			//temp_plu_inhibit_list[14]=14;	// PCS
			//temp_plu_inhibit_list[15]= 0;	// PCS Unit
			//temp_plu_inhibit_list[23]=23;
			//temp_plu_inhibit_list[24]=24;
			//temp_plu_inhibit_list[100]=100;
			break;
	}

	if (num != MenuPrevPluType) MenuRefresh = ON;
	MenuPrevPluType = num;
}

void callback_end_unitweight(INT32U num)
{
	INT8U new_id;
	INT8U id_fweight;
	INT8U id_tare;

	if(GlbOper.EnableKgLb) {
		get_weightunit_and_id_fromid(num, &new_id, &CurPluWeightUnit);
	}

	id_fweight = Menu_GetMenuId(100);
	id_tare = Menu_GetMenuId(13);

	if(id_fweight != 0xff) Menu[id_fweight].symbol = CurPluWeightUnit + 1;
	if(id_tare != 0xff)    Menu[id_tare].symbol = CurPluWeightUnit + 1;

	if (num != MenuPrevUnitWeight) MenuRefresh = ON;
	MenuPrevUnitWeight = num;
}

#ifndef USE_PLU_MULTIPLE_PRICE	//Refefence PLU 기능 사용 안 함
void callback_end_numlinkplu(INT32U num)
{
	INT8U id_linkdpt1;
	INT8U id_linkplu1;
	INT8U id_linkdpt2;
	INT8U id_linkplu2;

	//temp_number_of_linked_plu(ret_long);

	id_linkdpt1 = Menu_GetMenuId(61);
	id_linkplu1 = Menu_GetMenuId(65);
	id_linkdpt2 = Menu_GetMenuId(62);
	id_linkplu2 = Menu_GetMenuId(66);

	switch(num)  {
		case 0:
			if(id_linkdpt1 != 0xff) Menu[id_linkdpt1].inhibit = ON;
			if(id_linkplu1 != 0xff) Menu[id_linkplu1].inhibit = ON;
			if(id_linkdpt2 != 0xff) Menu[id_linkdpt2].inhibit = ON;
			if(id_linkplu2 != 0xff) Menu[id_linkplu2].inhibit = ON;
//DEL071022			temp_plu_inhibit_list[61]=61;
//DEL071022			temp_plu_inhibit_list[65]=65;
//DEL071022			temp_plu_inhibit_list[62]=62;
//DEL071022			temp_plu_inhibit_list[66]=66;
			break;
		case 1:
			if(id_linkdpt1 != 0xff) Menu[id_linkdpt1].inhibit = OFF;
			if(id_linkplu1 != 0xff) Menu[id_linkplu1].inhibit = OFF;
			if(id_linkdpt2 != 0xff) Menu[id_linkdpt2].inhibit = ON;
			if(id_linkplu2 != 0xff) Menu[id_linkplu2].inhibit = ON;
//DEL071022			temp_plu_inhibit_list[61]=0;
//DEL071022			temp_plu_inhibit_list[65]=0;
//DEL071022			temp_plu_inhibit_list[62]=62;
//DEL071022			temp_plu_inhibit_list[66]=66;
			break;
		case 2:
			if(id_linkdpt1 != 0xff) Menu[id_linkdpt1].inhibit = OFF;
			if(id_linkplu1 != 0xff) Menu[id_linkplu1].inhibit = OFF;
			if(id_linkdpt2 != 0xff) Menu[id_linkdpt2].inhibit = OFF;
			if(id_linkplu2 != 0xff) Menu[id_linkplu2].inhibit = OFF;
//DEL071022			temp_plu_inhibit_list[61]=0;
//DEL071022			temp_plu_inhibit_list[65]=0;
//DEL071022			temp_plu_inhibit_list[62]=0;
//DEL071022			temp_plu_inhibit_list[66]=0;
 			break;
	}
	if (Startup.disable_dept) {
		if(id_linkdpt1 != 0xff) Menu[id_linkdpt1].inhibit = ON;
		if(id_linkdpt2 != 0xff) Menu[id_linkdpt2].inhibit = ON;
//DEL071022		temp_plu_inhibit_list[61]=61;
//DEL071022		temp_plu_inhibit_list[62]=62;
	}

	if (num != MenuPrevNoLinkPlu) MenuRefresh = ON;
	MenuPrevNoLinkPlu = num;
}
#endif



INT8U callback_check_weightvalue(INT32U num)
{
	INT8U temp;
	INT8U result;

	temp = get_global_bparam(GLOBAL_SALE_SETUP_FLAG4)&0x20;     //PARAM 618
	result = ON;
	if (temp == 0)
	{
	     	if(num > ad_get_capa() || !ad_check_weight_value(num)) {
			BuzOn(2);
			//error_display_invalidkey();
			result = OFF;
		}
	}

	return result;
}

extern void PrintListWaitEnd(void);
extern INT16S plu_type_narray(INT16S m,INT16S *ret);
extern INT8U LabelIdSetPosition;
#ifdef USE_ARAB_CONVERT
#define PLU_EDIT_STR_LEN 21
#else
#define PLU_EDIT_STR_LEN 15
#endif
INT8U plu_input(INT32U key,INT16U deptno,INT32U pluno, INT16S maxid, INT16S *enable_list, INT16S page_offset, INT16S saveflag)
{
	CAPTION_STRUCT 	cap;
	INT16S result, cur_id;
	INT32U pnt;
	INT8U stype;
	INT16U usize;
	INT8U temp;
	HUGEDATA INT8U *data_point;
	INT8U   selldate_flag;   //   0 = number of day,1 = Infut format is mm.dd.yy
	INT8U  dlength, dpoint;//, ki_rotate_flag=0;
	INT16S    sl,disp_length;
//	INT8U	lineno;
	INT16U  length;
	INT16U	ptype, uitemp;
	INT32U	vdefault, minvalue, maxvalue;
	INT32U npnt;
	INT16S     ret_temp;
	char str[64], format[64];
	INT16U properties;
//	INT32U addr_global;
	void (*callback_func)();
	INT8U	fw_temp; //se-hyung 20070920
	INT8U temp_byte;
	INT16U temp_word;
	INT32U temp_long;
	INT8U plulen;
	//TRANSACTION_RECORD_TYPE pTrans;

	//INT8U	chg_flag;
	INT8U	use_memberprice_flag;
	INT8U	use_packingprice_flag;
#ifdef USE_EMART_CHANGE_DATA_AT_NON_SALE
	UNITPRICE_CHANGE_TYPE chgprice;
	TR_ADDITION_DATA_TYPE trans_add;
	INT32U	originUnitPrice;
	INT32U	resultUnitPrice;
	INT8U	originPluType;
	INT8U	resultPluType;
	INT8U	originFixedPrice;
	INT8U	resultFixedPrice;
	INT8U 	Reason_code;
	INT16U	plu_addr;
	INT8U	changeFlag;
	INT8U	ret_value = 0;
	INT16S    	size;
#endif
	INT8U ucTemp, nutri_type=0;
#ifdef USE_DIRECT_INGREDIENT
	INT8U ret;
	HUGEDATA INT8U *dmsg;
	HUGEDATA INT8U *nv_point;
	INT16U ingred_no[DIRECT_INGREDIENT_BLOCK_NUM];
#endif
#ifdef USE_PCS_FONT5_TO_TRACE_CODE	// USE PCS, PLUFONT5 field for Trace CODE 
	INT8U ret;
	INT8U *dmsg;
	INT8U *nv_point;
	INT8U temp_font5;
	INT16U temp_pcs;
	INT32U temp_tracecode;	
#endif
	INT32U point;

	//chg_flag = get_global_bparam(GLOBAL_SALE_SETUP_FLAG8) & 0x40;	//parameter 723
	use_memberprice_flag = get_global_bparam(GLOBAL_SALE_SETUP_FLAG18) & 0x80;	//parameter 729
	use_packingprice_flag = get_global_bparam(GLOBAL_SALE_SETUP_FLAG19) & 0x02;	//parameter 743
#ifdef USE_EMART_CHANGE_DATA_AT_NON_SALE
	originUnitPrice = 0;
	originPluType = 0;
	originFixedPrice = 0;
	if (UseEmartNetwork)
	{
		plu_table_search((INT8U)deptno, pluno,&plu_addr,0);
		if (plu_addr)
		{
			plu_get_field(plu_addr-1, PTYPE_PRICE, (INT8U *)&originUnitPrice, &size,sizeof(originUnitPrice));
			plu_get_field(plu_addr-1, PTYPE_PLU_TYPE, (INT8U *)&originPluType, &size,sizeof(originPluType));
		#ifdef USE_FIXED_PLUTYPE//USE_EMART_NETWORK
			if (originPluType >= PLU_BY_WEIGHT_FIX && originPluType <= PLU_BY_PCS_FIX)
			{
				originPluType -= 3;
			}
		#endif
			plu_get_field(plu_addr-1, PTYPE_FIXED_PRICE, (INT8U *)&originFixedPrice, &size,sizeof(originFixedPrice));
		}
	}
#endif

//	addr_global = DFLASH_BASE;
//	addr_global += FLASH_GLOBAL_AREA;
//DEL071023	clear_temp_inhibit_list();
	MenuPrevPluType = 0;
	MenuPrevUnitWeight = 0;
	MenuPrevNoLinkPlu = 0xff;
	selldate_flag = get_global_bparam(GLOBAL_SALEDATE_TYPE);

	//Menu_Init();
	Menu_Init_WithPage(page_offset);
	MenuNumLinePerPage = 1;
	MenuDefaultCallbackMode = 1;	// 1:below

#ifdef USE_SINGLE_LINE_GRAPHIC
	caption_split_by_code(0x1120, &cap,0);
	Menu_HeadCreate((HUGEDATA char *)cap.form);
#else
	plu_get_display_info(PTYPE_PLU_NO, &cap, 1);
	sprintf(str, "   %s %ld", cap.form, pluno);// 3자리 skip
	Menu_HeadCreate(str);
#endif

	for (cur_id = 0; cur_id < maxid; cur_id++)
	{
		uitemp = plu_get_item_position_from_ptype(enable_list[cur_id]);
		pnt   = DFLASH_BASE;
		pnt  += FLASH_PLU_STRUCT_AREA;
		pnt  += PLU_STRUCT_POINT;
		pnt  += PLU_STRUCT_SIZE * uitemp;
		npnt  = DFLASH_BASE;
		npnt += FLASH_PLU_STRUCT_CAPTION_AREA;
		npnt += PLUSTR_NAME_SIZE * uitemp;
		Flash_sread(npnt ,(HUGEDATA INT8U*)str,PLUSTR_NAME_SIZE);
		ptype    = Flash_wread(pnt+PLUSTR_PTYPE_POINT);
		vdefault = Flash_lread(pnt+PLUSTR_VDEFAULT_POINT);
		maxvalue = Flash_lread(pnt+PLUSTR_MAXVALUE_POINT);
	   	stype    = Flash_bread(pnt+PLUSTR_STYPE_POINT);
		length   = Flash_bread(pnt+PLUSTR_LENGTH_POINT);
		//lineno   = Flash_bread(pnt+PLUSTR_LINENO_POINT);
		dpoint   = Flash_bread(pnt+PLUSTR_DPOINT_POINT);
		usize    = Flash_wread(pnt+PLUSTR_USIZE_POINT);
		minvalue = vdefault;

		properties = 0;
		dpoint = 0;
		callback_func = NULL;
#ifdef USE_EMART_NETWORK
		if((ptype & 0x7fff) == PTYPE_SPECIAL_PRICE)	//special price
		{
			sprintf(str, "기준매가");
		}
		if((ptype & 0x7fff) == PTYPE_TAX_NO)	//Tax No.
		{
			sprintf(str, "쪼개기 가능 여부");
			maxvalue = 1;
			length = 1;
		}
		if((ptype & 0x7fff) == PTYPE_NUTRITION_NO)	//Nutrition No.
		{
			sprintf(str, "매가 변경 통제값");
			maxvalue = 19999;
			length = 5;
		}
#endif
#ifdef USE_PRICE_CHANGEABLE_CHECK
		if((ptype & 0x7fff) == PTYPE_TAX_NO)	//Tax No.
		{
			sprintf(str, "가격변경제한");
			maxvalue = 1;
			length = 1;
		}
#endif
		point   = plu_get_nth_point((INT16U)uitemp,(INT16U *)&usize);
		data_point = &NvramTempBuffer[0][point];
		switch (ptype&0x7fff) {
			case PTYPE_PLU_PREFIX:
				if (Startup.country==COUNTRY_KR) {
					usize =  2;
					stype = 'W';
					maxvalue=99;
					vdefault=0;
				}
				break;
			case PTYPE_UNIT_WEIGHT:
				vdefault=(INT32U)get_global_bparam(GLOBAL_DEFAULTUNIT);
				minvalue=1;
				maxvalue=1;
				if(GlbOper.EnableKgLb) maxvalue=2;
				//MenuPrevUnitWeight = *data_point;	// 1byte : Menu의 EndFunction에서 참고
				callback_func = plucall_unitweight;
				break;
			case PTYPE_PLU_TYPE:
				temp = *data_point;
				maxvalue=(INT32U)plu_type_narray((INT16S)temp, &ret_temp);
				if(!input_range_check((long)ret_temp, minvalue, maxvalue)) {
					ret_temp = (INT8U)vdefault;
				}
				*data_point = (INT8U)ret_temp;
				//MenuPrevPluType = *data_point;	// 1byte : Menu의 EndFunction에서 참고
				callback_func = plucall_plutype;
				break;
			case PTYPE_TAX_NO:
				//plucall_taxcode_pos_y = 1;	//JJANG 20070612	TAX_CODE 줄 바꿈
#ifdef USE_TAX_RATE
				callback_func = plucall_taxcode;
#endif
				break;
			case PTYPE_ORIGIN_NO:
				maxvalue=MAX_ORIGIN_NO;
				length  =ilog((INT32U)maxvalue);
				callback_func = plucall_origin;
				break;
			case PTYPE_INGREDIENT_NO:
#if defined(USE_9999_INDIRECT_INGREDIENT) || defined(USE_INGREDIENT_CODE_4DIGIT)
				maxvalue = MAX_INGREDIENT_INDEX_NUMBER;
#else
				maxvalue=MAX_INGREDIENT_NO;
#endif
				length  =ilog((INT32U)maxvalue);
				callback_func = plucall_ingredient;
				break;
			case PTYPE_PRICE:
#ifdef USE_PLU_MULTIPLE_PRICE
			case PTYPE_LINK2_PLU_NO:	// Second Price
			case PTYPE_COUPLE_PLU_NO:	// First Price
#endif
#ifndef USE_TOPMART_STRUCTURE 
			case PTYPE_SPECIAL_PRICE:
#endif
				length  = display_parameter.mode_len[DISPLAY_UNITPRICE];
				maxvalue = power10(length)-1;
				dpoint = display_parameter.mode_dec[DISPLAY_UNITPRICE];
				if (dpoint) {
					length++;
				}
				break;
#ifdef USE_PLU_MULTIPLE_PRICE
			case PTYPE_LINK1_PLU_NO:	// Second Target
			case PTYPE_REF_PLU_NO:	// First Target
				length = 5;
				maxvalue = power10(length)-1;
				dpoint = 0;
				break;
#endif
			case PTYPE_TARE_VALUE:
#ifndef USE_SPAR_MEMBER_PRICE
				if(use_memberprice_flag)
				{
					caption_split_by_code(0x4730, &cap,0);
					sprintf(str, cap.form);
					length	= display_parameter.mode_len[DISPLAY_UNITPRICE];
					maxvalue = power10(length)-1;
					dpoint = display_parameter.mode_dec[DISPLAY_UNITPRICE];
					if (dpoint) {
						length++;
					}
				}
				else
#endif
				{
		   		fw_temp = get_global_bparam(GLOBAL_SALE_SETUP_FLAG4)&0x20;	 //se-hyung 20070920
				if (fw_temp)							 //Modify by se-hyung 고정무게 입력시 캐파 무시, e값 무시 20070920
				{
		      	    		length = 6;						 
				}
				else
				{
					length = display_parameter.mode_len[DISPLAY_WEIGHT];
				}
					maxvalue = power10(length)-1;
                    dpoint = get_decimal_pt(6, 0);
                    if (dpoint) {
                        length++;
                    }
				}
				break;
            case PTYPE_FIXED_WEIGHT:
				if (use_packingprice_flag)
				{
					caption_split_by_code(0x9307, &cap,0);
					sprintf(str, cap.form);
					length	= display_parameter.mode_len[DISPLAY_UNITPRICE];
					maxvalue = power10(length)-1;
					dpoint = display_parameter.mode_dec[DISPLAY_UNITPRICE];
					if (dpoint) {
						length++;
					}
				}
				else
				{
					fw_temp = get_global_bparam(GLOBAL_SALE_SETUP_FLAG4)&0x20;	 //se-hyung 20070920
					if (fw_temp)							 //Modify by se-hyung 고정무게 입력시 캐파 무시, e값 무시 20070920
					{
						length = 6;
					}
					else
					{
						length = display_parameter.mode_len[DISPLAY_WEIGHT];
					}
					maxvalue = power10(length)-1;
#ifdef USE_INVENTORY_LABEL
					dpoint = get_global_bparam(GLOBAL_AREA_PRICE_DECIMALPOINT);
#elif defined (USE_USA_FIXEDWEIGHT_UNIT_OZ)
					dpoint = 0; //oz decimal point
#else
					dpoint = get_decimal_pt(6, 0);
#endif
					if (dpoint) {
						length++;
					}
				}
				break;
			case PTYPE_PERCENT_TARE_LIMIT:
			case PTYPE_PERCENT_TARE:
				dpoint = 2;
				break;
			case PTYPE_NO_OF_LINK_PLU:	// # of LinkPLU
#ifdef USE_AUSTRALIA_COOL_FUNCTION
				continue;
#else
				//MenuPrevNoLinkPlu = *data_point;	// 1byte : Menu의 EndFunction에서 참고
				break;
#endif
			case PTYPE_PLU_NAME2:	// Name 2
			case PTYPE_PLU_NAME3:	// Name 3
				properties = MENU_P_USE_LINEFEED;
				break;
			case PTYPE_SELLBY_DATE:	// sellbydate
#ifdef 	USE_SELLBYDATE_CHANGE
				selldate_flag = 1;
#endif
				if(selldate_flag==1) {
					 maxvalue = 999999;
					 length  = 6;
				}
				stype = 'N';	// string타입으로 입력
				break;
			case PTYPE_INGREDIENT_TEXT:	// Direct Ingredient
#ifdef USE_DIRECT_INGREDIENT
				length = DEFAULT_INGREDIENT_TEXT_ALL_SIZE;//length = 300;
				data_point += (4 + 2*DIRECT_INGREDIENT_BLOCK_NUM) ;
				stype = 'I';	// Ingredient 전용 타입
#else
				length = DEFAULT_INGREDIENT_TEXT_SIZE - 4;//length = 300;
				data_point += 6;
				stype = 'S';
#endif
				break;
#ifdef USE_PCS_FONT5_TO_TRACE_CODE	// USE PCS, PLUFONT5 field for Trace CODE 
			case PTYPE_PIECE: // Direct Ingredient
				sprintf(str, "품목 코드");
				maxvalue = 999999;
				length = 6;
				stype = 'L';
				data_point = ( INT8U *)(get_temp_area(0) + (INT32U)plu_record_size + 4);
				break;
			case PTYPE_PLU_NAME5_FONT:
				continue;
#endif
			case PTYPE_TARE_NO:	// Tare No.
				callback_func = plucall_tareid;
				break;
			case PTYPE_QTYUNIT_NO:	// Qty Unit No.
				callback_func = plucall_pcscode;
				break;
			case PTYPE_LABEL_MSG_NO:	// Sales Msg No.
#ifndef USE_HYUNDAI_CHECK_REASON_CODE
				callback_func = plucall_label_message;
#endif
				break;
			case PTYPE_FIXED_PRICE:	// Use Fixed Price Type
				callback_func = plucall_fixed;
				break;
			case PTYPE_DEPARTMENT_NO:		// Department
				callback_func = plucall_department;
				break;
			case PTYPE_GROUP_NO:		// Group No.
#if (defined(USE_TRACE_STANDALONE) && defined(USE_TRACE_MEATGRADE))
				callback_func = trace_meatgrade_call_back;
#else
				callback_func = plucall_group;
#endif
				break;
			case PTYPE_TRACE_NO:	// Traceability No.
#ifdef USE_TRACEABILITY //gm 090407, memory 충돌 방지
				callback_func = plucall_traceability;
#endif
				break;
			case PTYPE_NUTRITION_NO:	// Nutrifact No.
#ifdef USE_NUTRITION_FACT
				ucTemp = get_global_bparam(GLOBAL_SALE_SETUP_FLAG9);
				nutri_type = ucTemp&0x0f;
#ifdef COUNTRY_USA
				nutri_type = NUTRI_TYPE_2COL_USA;
#endif						
				if(nutri_type == NUTRI_TYPE_2COL_6DIG_AUS || nutri_type == NUTRI_TYPE_2COL_6DIG_AUS_ACTUAL_WEIGHT
					|| nutri_type == NUTRI_TYPE_6DIG_CZE)
				{
					maxvalue = MAX_NUTRIFACT2_NO;
				}
#ifdef USE_EU_NUTRITIONFACT
				else if (nutri_type == NUTRI_TYPE_42ITEM_5DIG_EU_ACTUAL_WEIGHT)
				{
					maxvalue = MAX_NUTRIFACT3_NO;
				}
#endif
#ifdef USE_ADDITIONAL_NUTRIENT
				else if(nutri_type == NUTRI_TYPE_2COL_USA || nutri_type == NUTRI_TYPE_TOTALFAT_CAN)
					maxvalue = MAX_NUTRIFACT4_NO;
#endif
				else maxvalue = MAX_NUTRIFACT_NO;
				callback_func = plucall_nutrifact;
#endif
				break;
#ifndef USE_TOPMART_STRUCTURE
			case PTYPE_AUX_BARCODE_NO:	// Barcode2 No.
#endif
			case PTYPE_BARCODE_NO:	// Barcode No.
				callback_func = plucall_barcodefile;
				break;
#ifndef USE_PLU_MULTIPLE_PRICE	//Refefence PLU 기능 사용 안 함
#ifdef USE_AUSTRALIA_COOL_FUNCTION
			case PTYPE_REF_DEPT_NO:	// Reference PLU Dept
				minvalue = 0;						
				maxvalue = 5;
				length = 1; 			
				callback_func = plucall_CoOL_type;
				break;
			case PTYPE_BONUS: // Bonus point
			case PTYPE_LINK1_DEPT_NO:	// Link PLU1 Dept
			case PTYPE_LINK2_DEPT_NO:	// Link PLU2 Dept
			case PTYPE_COUPLE_DEPT_NO:	// Coupled PLU Dept 						
				continue;
#else
			case PTYPE_LINK1_DEPT_NO:	// Link PLU1 Dept
			case PTYPE_LINK2_DEPT_NO:	// Link PLU2 Dept
			case PTYPE_COUPLE_DEPT_NO:	// Coupled PLU Dept
			case PTYPE_REF_DEPT_NO:	// Reference PLU Dept
				callback_func = plucall_department;
				break;
#endif
#endif
			case PTYPE_LABEL_NO:	// Label No.
#ifndef USE_TOPMART_STRUCTURE
			case PTYPE_AUX_LABEL_NO:	// Aux. Label No.
#endif
				callback_func = plucall_label_id;
				break;
#ifndef USE_PLU_MULTIPLE_PRICE	//Refefence PLU 기능 사용 안 함
#ifdef USE_AUSTRALIA_COOL_FUNCTION
			case PTYPE_REF_PLU_NO:	// Reference PLU No.
				minvalue = 0;						
				maxvalue = 100;
				length = 3;
				break;
			case PTYPE_LINK1_PLU_NO:	// Link PLU1 No.
			case PTYPE_LINK2_PLU_NO:	// Link PLU2 No.
			case PTYPE_COUPLE_PLU_NO:	// Coupled PLU No.
				continue;
#else
			case PTYPE_LINK1_PLU_NO:	// Link PLU1 No.
			case PTYPE_LINK2_PLU_NO:	// Link PLU2 No.
			case PTYPE_COUPLE_PLU_NO:	// Coupled PLU No.
			case PTYPE_REF_PLU_NO:	// Reference PLU No.
				callback_func = plucall_plu_number;
				break;
#endif
#endif
#ifdef USE_SPAR_MEMBER_PRICE
            case PTYPE_MEMBER_PRICE:	// member price spar exclusive
                caption_split_by_code(0x4730, &cap,0);
				sprintf(str, cap.form);
				length	= display_parameter.mode_len[DISPLAY_UNITPRICE];
				maxvalue = power10(length)-1;
				dpoint = display_parameter.mode_dec[DISPLAY_UNITPRICE];
				if (dpoint) {
					length++;
				}
                break;
#endif
#if defined (USE_RUSSIA_LOGISTICS_PLU) || defined (USE_EUR_LOGISTICS_PLU)
			case PTYPE_GTIN_TYPE:	// PTYPE_GTIN_TYPE
				callback_func = plucall_gtintype;
				break;
#endif

		}
		plulen = PLU_EDIT_STR_LEN;
		if (IsHanCharMid(str, PLU_EDIT_STR_LEN)==CHAR_HAN_MID)
		{
			plulen++;
		}
		str[plulen] = 0;
#ifdef USE_ARAB_CONVERT
		sl = Convert_ArabStrLength(str);
#elif defined(USE_THAI_FONT)
        sl = Cvt_ThaiStrLength((INT8U*)str);
#else
		sl=strlen(str);
#endif
		disp_length=Dsp_GetChars();
		dlength=length;
#ifdef USE_SINGLE_LINE_GRAPHIC
		if (dlength>disp_length) dlength=disp_length;
#else
		if (dlength+sl+3>disp_length) dlength=disp_length-sl-3;
#ifdef HEBREW_FONT
		sprintf(format,":]%%%ds[",dlength);
#else
		sprintf(format,":[%%%ds]",dlength);
#endif
		sprintf(&str[strlen(str)],format," ");
#endif

		switch (stype) {
			case 'S':
				//keyin_string_set(1);
				Menu_InputCreate(ptype, (HUGEDATA char *)str, MENU_TYPE_STR, sl+2, dlength, length,
							0, 0, 0, NULL, (HUGEDATA INT8U*)data_point, NULL, NULL);
				//if(properties) Menu_SetProperties(ptype, properties);	// Menu_InputCreate 다음에 위치할것
#ifdef USE_CHINA_PINYIN
				properties = properties | MENU_P_CHINA_PINY;
#endif
				Menu_SetProperties(ptype, properties|MENU_P_EXCLUDE_NULL);	// Menu_InputCreate 다음에 위치할것
				//keyin_string_set(0);
				break;
			case 'W':
				memcpy(&temp_word, data_point, 2);
				if(!input_range_check(temp_word, minvalue, maxvalue)) {
					temp_word = vdefault;
					memcpy(data_point, &temp_word, 2);
				}
				Menu_InputCreate(ptype, (HUGEDATA char *)str, MENU_TYPE_NUM_2BYTE, sl+2, dlength, length,
							maxvalue, minvalue, dpoint, (HUGEDATA INT32U*)data_point, NULL, NULL, callback_func);
				break;
			case 'L':
				memcpy(&temp_long, data_point, 4);
				if(!input_range_check(temp_long, minvalue, maxvalue)) {
					temp_long = vdefault;
					memcpy(data_point, &temp_long, 4);
				}
				Menu_InputCreate(ptype, (HUGEDATA char *)str, MENU_TYPE_NUM, sl+2, dlength, length,
							maxvalue, minvalue, dpoint, (HUGEDATA INT32U*)data_point, NULL, NULL, callback_func);
				break;
			case 'M':
			case 'B':
				memcpy(&temp_byte, data_point, 1);
				if(!input_range_check(temp_byte, minvalue, maxvalue)) {
					temp_word = vdefault;
					memcpy(data_point, &temp_byte, 1);
				}
				Menu_InputCreate(ptype, (HUGEDATA char *)str, MENU_TYPE_NUM_1BYTE, sl+2, dlength, length,
							maxvalue, minvalue, dpoint, (HUGEDATA INT32U*)data_point, NULL, NULL, callback_func);
				break;
			case 'N':
				Menu_InputCreate(ptype, (HUGEDATA char *)str, MENU_TYPE_DATENUM, sl+2, dlength, length,
							maxvalue, minvalue, dpoint, (HUGEDATA INT32U*)data_point, NULL, NULL, callback_func);
				break;
#ifdef USE_DIRECT_INGREDIENT
			case 'I':
#ifdef USE_AUSTRALIA_COOL_FUNCTION
				Menu_InputCreate(ptype, (HUGEDATA char *)str, MENU_TYPE_STR, sl+2, dlength, DEFAULT_INGREDIENT_TEXT_ALL_SIZE,
								maxvalue, minvalue, 0, NULL, (HUGEDATA INT8U*)data_point, NULL, NULL);						
				Menu_SetProperties(ptype, MENU_P_USE_LINEFEED|MENU_P_DSP_POSITION|MENU_P_PAGEUPDN_KEY);
#else
				caption_split_by_code(0x131B, &cap,0);
				cap.input_dlength = Dsp_GetChars();
				cap.input_dlength -= strlen(cap.form);
				cap.input_dlength -= 2;
				caption_adjust(&cap,NULL);
				Menu_InputCreate(ptype, (HUGEDATA char *)cap.form, MENU_TYPE_STR, cap.input_x, cap.input_dlength, DEFAULT_INGREDIENT_TEXT_ALL_SIZE,
								cap.input_max, cap.input_min, 0, NULL, (HUGEDATA INT8U*)data_point, NULL, NULL);
//				Menu_SetProperties(ptype, MENU_P_USE_LINEFEED|MENU_P_DSP_POSITION|MENU_P_PAGEUPDN_KEY|MENU_P_CHECK_BLANKS);
				Menu_SetProperties(ptype, MENU_P_USE_LINEFEED|MENU_P_DSP_POSITION|MENU_P_PAGEUPDN_KEY);
//				Menu_InputCreate(ptype, (HUGEDATA char *)str, MENU_TYPE_STR, sl+2, dlength, length,
//							0, 0, 0, NULL, (HUGEDATA INT8U*)data_point, NULL, NULL);
//				Menu_SetProperties(ptype, properties|MENU_P_EXCLUDE_NULL);	// Menu_InputCreate 다음에 위치할것
				//keyin_string_set(0);
#endif
				break;
#endif
/*
			case 'T':
			case 'D':
				memcpy((INT8U *)str,(INT8U *)data_point,3);
				sprintf((char*)format,"99.99.99");
				sprintf((char*)ret_string,"%02d.%02d.%02d",str[0],str[1],str[2]);
				dlength=length=strlen(format);
				Menu_InputCreate(1, (HUGEDATA char *)str, MENU_TYPE_IP, sl+3, dlength, length,
									maxvalue, cap.input_min, 0, (HUGEDATA INT32U*)ret_string, NULL, format, NULL);
				break;
*/
		}


		switch(ptype)
		{
#ifndef USE_SRILANKA_CERTI_DEFAULT
			case PTYPE_PRICE: // Price
#endif	// USE_SRILANKA_CERTI_DEFAULT
#ifndef USE_TOPMART_STRUCTURE
			case PTYPE_SPECIAL_PRICE: // SpecialPrice
#endif
#ifdef	USE_PLU_MULTIPLE_PRICE
			case PTYPE_LINK2_PLU_NO:	// Second Price
			case PTYPE_COUPLE_PLU_NO:	// First Price
#endif
				Menu_SetSymbol(ptype, 11);
				break;
			case PTYPE_TARE_VALUE:
#ifndef USE_SPAR_MEMBER_PRICE
				if(use_memberprice_flag)
				{
					Menu_SetSymbol(ptype, 11);
				}
				else
#endif
				{
				Menu_SetSymbol(ptype, CurPluWeightUnit+1);	//1:kg, 2:lb, 3:g, 11:$
				Menu_AddCheckFunction(ptype, callback_check_weightvalue);
				}
				break;
			case PTYPE_FIXED_WEIGHT: // Fixed Weight
				if(use_packingprice_flag)
				{
					Menu_SetSymbol(ptype, 11);	//1:kg, 2:lb, 3:g, 11:$
				}
				else
				{
#ifdef USE_INVENTORY_LABEL
					Menu_SetSymbol(ptype, CurPluWeightUnit+1);	//1:kg, 2:lb, 3:g, 11:$
#elif defined(USE_USA_FIXEDWEIGHT_UNIT_OZ)
					Menu_SetSymbol(ptype, 4);	//1:kg, 2:lb, 3:g, 4:oz, 11:$, 41: oz
#else
					Menu_SetSymbol(ptype, CurPluWeightUnit+1);	//1:kg, 2:lb, 3:g, 11:$
					Menu_AddCheckFunction(ptype, callback_check_weightvalue);
#endif
				}
				break;
			case PTYPE_PLU_TYPE:	// Plu Type
				Menu_AddEndFunction(ptype, callback_end_plutype);
				break;
			case PTYPE_UNIT_WEIGHT: // UnitWeight
				Menu_AddEndFunction(ptype, callback_end_unitweight);
				break;
#ifndef USE_PLU_MULTIPLE_PRICE	//Refefence PLU 기능 사용 안 함
			case PTYPE_NO_OF_LINK_PLU: // Linked plu
				Menu_AddEndFunction(ptype, callback_end_numlinkplu);
				break;
#endif
#ifdef USE_AUSTRALIA_COOL_FUNCTION
			case PTYPE_REF_PLU_NO:	// Contains (%)
				Menu_SetSymbol(ptype, 31);
				break;
#endif
#ifdef USE_SPAR_MEMBER_PRICE
            case PTYPE_MEMBER_PRICE:    // member price spar exclusive
                Menu_SetSymbol(ptype, 11); // 11 : currency symbol 
                break;
#endif
 		}

	}
	if (saveflag)
	{
		result = MENU_RET_SAVE;
	}
	else
	{
		Menu_return_last_item_arrow_dn = 1;
		Menu_return_first_item_arrow_up = 1;
		result = Menu_InputDisplay();
		Menu_return_last_item_arrow_dn = 0;
		Menu_return_first_item_arrow_up = 0;
	}
	if(result == MENU_RET_SAVE || result == MENU_RET_DN)
	{
		pnt=plu_get_ptype_point(PTYPE_DEPARTMENT_NO,&stype,&usize);
		plu_save_deptid(pnt,(INT32U)deptno,stype,usize);
		pnt=plu_get_ptype_point(PTYPE_PLU_NO,&stype,&usize);
		plu_save_deptid(pnt,(INT32U)pluno,stype,usize);

#ifdef USE_DIRECT_INGREDIENT
		//ret = ingred_text_restore_temp(0,(INT8U)deptno,pluno);  //SG070730
#ifdef CPU_PIC32MZ
		dmsg = (INT8U *)(NvramTempBuffer[0] + (INT32U)plu_record_size + 4);
#else
		dmsg = (HUGEDATA INT8U *)(get_temp_area(0) + (INT32U)plu_record_size + 4);
#endif
		if(strlen(dmsg)==0)
		{
			ingredient_text_table_delete(key);
		}
		else
		{
			nv_point = (HUGEDATA INT8U *)(get_temp_area(0) + plu_get_ptype_point(PTYPE_INGREDIENT_TEXT,NULL,(INT16U *)&usize));
			memcpy((INT8U*)&ingred_no[0], nv_point, 2*DIRECT_INGREDIENT_BLOCK_NUM);
			ret = ingredient_text_write(&ingred_no[0], dmsg, 0, DEFAULT_INGREDIENT_TEXT_ALL_SIZE, 1);
			memcpy(nv_point, (INT8U *)&ingred_no[0], 2*DIRECT_INGREDIENT_BLOCK_NUM);
			if(!ret)//direct ingredient memory full
			{
				error_display_caption(0xF035); //data full!!
				//if(result == 2) cur_id--;
				//continue;
			return 0;
			}
		}
#endif

#ifdef USE_PCS_FONT5_TO_TRACE_CODE	// USE PCS, PLUFONT5 field for Trace CODE 
		dmsg = ( INT8U *)(get_temp_area(0) + (INT32U)plu_record_size + 4);
		memcpy(&temp_tracecode, dmsg, 4);
		temp_pcs = (INT16U)(temp_tracecode/100);
		temp_font5= (INT8U)(temp_tracecode%100);		
		nv_point = ( INT8U *)(get_temp_area(0) + plu_get_ptype_point(PTYPE_PIECE,NULL,(INT16U *)&usize));
		memcpy(nv_point, &temp_pcs, 2);
		nv_point = ( INT8U *)(get_temp_area(0) + plu_get_ptype_point(PTYPE_PLU_NAME5_FONT,NULL,(INT16U *)&usize));		
		memcpy(nv_point, &temp_font5, 1);
#endif

#ifdef USE_EMART_CHANGE_DATA_AT_NON_SALE
		if (UseEmartNetwork)
		{
			pnt=plu_get_ptype_point(PTYPE_PRICE,&stype,&usize);
			plu_temp_get(0,pnt,(char *)&resultUnitPrice,usize);
			pnt=plu_get_ptype_point(PTYPE_PLU_TYPE,&stype,&usize);
			plu_temp_get(0,pnt,(char *)&resultPluType,usize);
		#ifdef USE_FIXED_PLUTYPE//USE_EMART_NETWORK
			if (resultPluType >= PLU_BY_WEIGHT_FIX && resultPluType <= PLU_BY_PCS_FIX)
			{
				resultPluType -= 3;
			}
		#endif
			pnt=plu_get_ptype_point(PTYPE_FIXED_PRICE,&stype,&usize);
			plu_temp_get(0,pnt,(char *)&resultFixedPrice,usize);

			changeFlag = GetChangeFlag(resultPluType, originPluType, resultUnitPrice, originUnitPrice, resultFixedPrice, originFixedPrice, 0);
			if (changeFlag)
			{
				ret_value = GetCheckAddTransactionData(0,1,AUDIT_MODE_CIRCULAR);		//mode : sale, count : 1, Circular
				if(ret_value)
				{
					Reason_code = 0;	//Fix
					InsertAdditionalData(&trans_add);
					InsertUnitPriceChangeRecord(status_scale.scaleid,deptno,pluno,
							originUnitPrice,resultUnitPrice,Reason_code,status_scale.clerkid,changeFlag,&chgprice);
					transaction_sale_store(NULL, NULL, &trans_add, &chgprice, 0, 1);
				}
				else return KP_ESC;
			}
		}
#endif

		plu_restore_temp(0,(long)key-1);
//DEL_MASK			         commun_plumask(key,0xff,3);
		if (network_status.service_flag&0x20)	// send plu
		{
			commun_net_mask(0, (INT16U *)&key, 0xff, 1);
		}
		plu_table_save((unsigned char)deptno,pluno);


				 //set_nvram_lparam(NVRAM_PLU_LASTNUMBER,pluno);
#ifdef USE_DIRECT_INGREDIENT
		Flash_flush_nvbuffer_c2(); //SG070810
#endif
		Flash_flush_nvbuffer();
		temp=get_global_bparam(GLOBAL_PRINT_OPER_FLAG2);
		if(temp&0x01)
		{
			if(PrtStruct.Mode == RECEIPT)
			{
				print_plu_contents(pluno, deptno, enable_list, maxid);
			}
			else
			{
				plu_base_change=1;
				Prt_PrintStart(PRT_ITEM_TEST_MODE, LABEL, 0, ON, deptno, pluno);
				plu_base_change=0;
			}
		}
//DEL_MASK		if (send_plu_modify(1,(INT8U)deptno,pluno)) {
//DEL_MASK			plu_modify_set(1,(INT8U)deptno,pluno);
//DEL_MASK		}

		return KP_SAVE;
	}
	else if (result == MENU_RET_UP)
	{
		return KP_ARROW_UP;
	}
	else if (result == MENU_RET_ESC)
	{
		return KP_ESC;
	}
	return result;
}

void plu_new(void)
{
	INT8U  	ret, saveflag;
	INT16U 	deptno ;
	INT32U 	pluno;
	INT16U  key, editing_key;
	long    rem;
	INT8U 	new_wunit_id, wunit_id, backup_weightunit;
	INT16S 	usize;
	INT16S enable_list[128];
	INT16S maxid;
	INT16U  uitemp;
#ifdef USE_DIRECT_INGREDIENT
	HUGEDATA INT8U *nv_point;
	HUGEDATA INT8U *dmsg;
	INT16U ingred_no[DIRECT_INGREDIENT_BLOCK_NUM];
#endif
#ifdef USE_PCS_FONT5_TO_TRACE_CODE	// USE PCS, PLUFONT5 field for Trace CODE 
	INT8U *nv_point;
	INT8U *dmsg;
	INT32U temp_tracecode;		
	INT16U temp_pcs;	
	INT8U temp_font5;		
#endif
	char disp_string[64];
	INT8U disp_remplu_flag = 1;
	INT8U tarelength;
	CAPTION_STRUCT cap;
#if defined(USE_PRICE_CHANGE_PROHIBITED) || defined(USE_PROHIBITE_PRICE_CHANGE_IN_PLU_MENU)
	plu_inhibit_bycode(PTYPE_PRICE,1);
#endif
				
	deptno = status_scale.departmentid; 
	maxid = read_enable_list(enable_list);
	editing_key = 0;
DEPT_PLU_IN:
	pluno=0;
REPT:
	Dsp_Fill_Buffer(0);
	rem = MAX_PLU_NO-plu_rem_number_get();
	if (rem<0) rem=0;

	menu_display_menucode(CurMenuCode, 1);

	tarelength=get_global_bparam(GLOBAL_DISPLAY_TARELENGTH);

#if defined(USE_SINGLE_LINE_GRAPHIC)
	if (disp_remplu_flag)
	{
		display_remain_plu();
		disp_remplu_flag = 0;
	}
	caption_split_by_code(0x1120, &cap,0);
	sprintf(disp_string, "%s", cap.form);
	menu_writetitle_str(disp_string);
	menu_writepage(maxid-1, 1);
#else
	sprintf(disp_string,"%05d",rem);
	if(tarelength)
		display_string(DISPLAY_UNITPRICE, (INT8U *)disp_string);
	else
		display_string(DISPLAY_PRICE, (INT8U *)disp_string);
	VFD7_Diffuse();
	caption_split_by_code(0x1120, &cap,0);
	sprintf(disp_string, "%s", cap.form);
	menu_writetitle_str(disp_string);
	menu_writepage(maxid-1, 1);
#endif

	ret = plu_input_department(&deptno, &pluno, 0);
	if(ret==2) {
		goto RET_ESC;
	}
	if (deptno<1) {
		goto RET_ESC;
	}
	plu_table_search((INT8U)deptno, pluno,&key,0);
	if (key==0) {
	    if (network_status.remote_plu) {
		plu_remotecall(deptno,pluno,0);
		display_clear(DISPLAY_PRICE);
		plu_table_search((INT8U)deptno, pluno,&key,0);
	    }
	}
	if (key==0) {
		key=plu_address_get_new();
		if (key==0L) {
			error_display_caption(0xF035);
			goto REPT;
		}
		plu_temp_clear(0);
#ifdef CPU_PIC32MZ
		plu_temp_init();
#else
		uitemp = plu_get_item_position_from_ptype(PTYPE_UNIT_WEIGHT);	//Added by JJANG
		plu_temp_init(uitemp);				//Added by JJANG 20070712
#endif
	} else {
		if(editing_key != key)	// 현재 진행중인 Temp mem의 PLU가 아닐경우 Temp mem 복사
			plu_move_temp(0,(long)key-1);
	}
	if (pluno > 0) {
		backup_weightunit = CurPluWeightUnit;
		plu_get_field(key-1, PTYPE_UNIT_WEIGHT,(INT8U *)&wunit_id, &usize, sizeof(wunit_id));	// UnitWeight
		get_weightunit_and_id_fromid(wunit_id, &new_wunit_id, &CurPluWeightUnit);

#ifdef USE_DIRECT_INGREDIENT
#ifdef CPU_PIC32MZ
		dmsg = ( INT8U *)(NvramTempBuffer[0] + plu_record_size + 4);
		nv_point = ( INT8U *)(NvramTempBuffer[0] + plu_get_ptype_point(PTYPE_INGREDIENT_TEXT,NULL,(INT16U *)&usize));
#else
		dmsg = (HUGEDATA INT8U *)(get_temp_area(0) + plu_record_size + 4);
		nv_point = (HUGEDATA INT8U *)(get_temp_area(0) + plu_get_ptype_point(PTYPE_INGREDIENT_TEXT,NULL,(INT16U *)&usize));
#endif
		memcpy((INT8U *)&ingred_no[0], nv_point, 2*DIRECT_INGREDIENT_BLOCK_NUM);
		ingredient_text_read(&ingred_no[0], dmsg, 0, DEFAULT_INGREDIENT_TEXT_ALL_SIZE);
		memcpy(nv_point, (INT8U *)&ingred_no[0], 2*DIRECT_INGREDIENT_BLOCK_NUM);
/*
		nv_point = (HUGEDATA INT8U*)get_temp_area(0);
		nv_point = (HUGEDATA INT8U*)((INT32U)nv_point + (INT32U)plu_get_ptype_point(PTYPE_INGREDIENT_TEXT,NULL,(INT16U *)&usize));
		memcpy((INT8U*)&ingredTextNo, nv_point,2);
		if( (ingredTextNo>0) && (ingredTextNo<=MAX_INGREDIENT_TEXT_NUMBER))
		{
			ingred_text_move_temp(0,ingredTextNo);
		}
		else
		{
			ingredTextNo =0;
			memset(nv_point, 0, 2);
			ingred_text_temp_clear(0);
		}
*/
#endif
#ifdef USE_PCS_FONT5_TO_TRACE_CODE	// USE PCS, PLUFONT5 field for Trace CODE 
		dmsg = ( INT8U *)(get_temp_area(0) + plu_record_size + 4);
		nv_point = ( INT8U *)(get_temp_area(0) + plu_get_ptype_point(PTYPE_PIECE,NULL,(INT16U *)&usize));
		memcpy(&temp_pcs, nv_point, 2);
		nv_point = ( INT8U *)(get_temp_area(0) + plu_get_ptype_point(PTYPE_PLU_NAME5_FONT,NULL,(INT16U *)&usize));		
		memcpy(&temp_font5, nv_point, 1);
		temp_tracecode = temp_pcs * 100 + temp_font5;
		memcpy(dmsg, &temp_tracecode, 4);
#endif
		saveflag = 0;
		if(ret == KP_SAVE) saveflag = 1;

		ret = plu_input(key,deptno,pluno,maxid,enable_list,1,saveflag);
		CurPluWeightUnit = backup_weightunit;	// 복귀
		switch(ret)  {
			case  KP_ESC:
				goto RET_ESC;
			case  KP_ARROW_DN:  			// Arrow Down (at end)
			case  KP_SAVE:
				set_nvram_lparam(NVRAM_PLU_LASTNUMBER,pluno);
				//pluno=0;	//중복
				editing_key = 0;
				goto DEPT_PLU_IN;
			case  KP_ARROW_UP:			// Arrow Up (at start)
				editing_key = key;	// 현재 진행중인 Temp mem의 PLU를 유지하기 위해
				//goto DEPT_PLU_IN;
				goto REPT;
		}
	} else {
	}
RET_ESC:
//	Dsp_Fill_Buffer(0);   // LCD Buffer clear
//	Dsp_Diffuse();
#if defined(USE_PRICE_CHANGE_PROHIBITED) || defined(USE_PROHIBITE_PRICE_CHANGE_IN_PLU_MENU)
	plu_inhibit_bycode(PTYPE_PRICE,0);
#endif
	memset(gmsg,0x20,5);
	gmsg[5]=0;
	display_string(DISPLAY_UNITPRICE, (INT8U *)gmsg);
	VFD7_Diffuse();
	return;
}

#ifndef DISABLE_LIST_MENU
void plu_list(void)
{
	CAPTION_STRUCT 	cap;
	POINT 		disp_p;
	INT8U	lcd_size;
	char	buffer[64];
#if defined(USE_ARAB_FONT) || defined(HEBREW_FONT)
	char	plu_name[40];
#else
	char	plu_name[21];
#endif
	INT8U	end_flag, input_line, plu_type;
	INT16S result, dept_no=1;
//	INT16U	list_title_cap;
//	INT16U  plu_data_size;
	INT32U  key;
	long    cur_id;
	INT16S     i,line,line_max;
	long	ret_long, plu_no;
	long	plu_num[2],line_id[4],nn[2];	// start_num = plu_num[0], end_num=plu_num[1];
	INT32U	addr;
//	INT32U  addr_plu_str;
	INT16U	pluname_offset, plutype_offset, pluno_offset;
	INT16U xsize[3];
	INT8U   stype;
	INT16U  input_cap[]={0x3151,0x3152,0x1E02};
	INT16S 	linenum;
	INT32U  input_min, input_max;

//	list_title_cap = 0x1158;
	caption_split_by_code(input_cap[0], &cap,0);
	caption_adjust(&cap,NULL);
	input_min = cap.input_min;
	input_max = cap.input_max;

	//addr_plu_str = DFLASH_BASE + FLASH_PLU_STRUCT_AREA;
	//addr_plu_str += PLU_STRUCT_POINT;
	dept_no=status_scale.departmentid;
	if ((dept_no<1) && (dept_no>99)) dept_no=1;

	lcd_size = Dsp_GetChars();
	pluname_offset=plu_get_ptype_point(PTYPE_PLU_NAME1,&stype,&xsize[0]);
	plutype_offset=plu_get_ptype_point(PTYPE_PLU_TYPE,&stype,&xsize[1]);
	pluno_offset  =plu_get_ptype_point(PTYPE_PLU_NO,&stype,&xsize[2]);
	//plu_data_size=plu_record_size;
	plu_num[0] = input_min;
	plu_num[1] = input_max;
LIST_START:
	end_flag=1;
	Dsp_Fill_Buffer(0);
PAGE1_VIEW:
	display_title_page_change(0x1150,1,2);
	result=plu_input_department_only((INT16U *)&dept_no, 0);
	if (result==2) goto RET_ESC;
PAGE2_VIEW:
	Dsp_Fill_Buffer(0);
	display_title_page_change(0x1150, 2, 2);
	linenum = Startup.line_max;
	if (linenum > 3) linenum = 3;
	for (i = 0; i < linenum; i++)  {
		caption_split_by_code(input_cap[i], &cap,0);
		if (i!=2) caption_adjust(&cap,NULL);
		disp_p = point((i+Startup.menu_type)*Startup.menu_ygap,0);
#ifdef HEBREW_FONT
		Convert_HebrewPreview((INT8U *)cap.form, strlen(cap.form), 0, 0);
		hebrew_codetemp[strlen(cap.form)] = 0;
		disp_p.x = 202 - disp_p.x;
		Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
		Convert_ArabPreview((INT8U *)cap.form, strlen((char *)cap.form), 0, 0, 0);
		arab_codetemp[strlen((char *)cap.form)] = 0;
		disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
		Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
		Dsp_Write_String(disp_p,(HUGEDATA char*)cap.form);
#endif
	}
	Dsp_Diffuse();
	input_line = 0;
PAGE2_KI_IN:
	for(i=input_line; i<2; i++)  {
		ret_long = plu_num[i];
		caption_split_by_code(input_cap[i], &cap,0);
		caption_adjust(&cap,NULL);
		disp_p = point((i+Startup.menu_type)*Startup.menu_ygap,0);
#ifdef HEBREW_FONT
		Convert_HebrewPreview((INT8U *)cap.form, strlen(cap.form), 0, 0);
		hebrew_codetemp[strlen(cap.form)] = 0;
		disp_p.x = 202 - disp_p.x;
		Dsp_Write_1stringLong(disp_p, hebrew_codetemp);

		result=keyin_ulong(KI_GRAPHIC, KI_FORM_ILNN,	0,(long*)&ret_long, cap.input_dlength,
			(i+Startup.menu_type)*Startup.menu_ygap,  cap.input_x*Startup.menu_xgap, 0,KI_NOPASS );
#elif defined(USE_ARAB_CONVERT)
		Convert_ArabPreview((INT8U *)cap.form, strlen((char *)cap.form), 0, 0, 0);
		arab_codetemp[strlen((char *)cap.form)] = 0;
		disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
		Dsp_Write_1stringLong(disp_p, arab_codetemp);

		result=keyin_ulong(KI_GRAPHIC, KI_FORM_ILNN,	0,(long*)&ret_long, cap.input_dlength,
			(i+Startup.menu_type)*Startup.menu_ygap,  cap.input_x*Startup.menu_xgap, 0,KI_NOPASS );
#else
		Dsp_Write_String(disp_p,(HUGEDATA char*)cap.form);
		result=keyin_ulong(KI_GRAPHIC, KI_FORM_NORMAL,	0,(long*)&ret_long, cap.input_dlength,
			(i+Startup.menu_type)*Startup.menu_ygap,  cap.input_x*Startup.menu_xgap, 0,KI_NOPASS );
#endif
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
#ifdef HEBREW_FONT
					Convert_HebrewPreview((INT8U *)cap.form, strlen(cap.form), 0, 0);
					hebrew_codetemp[strlen((char*)cap.form)] = 0;
					disp_p.x = 202 - disp_p.x;
					Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
					Convert_ArabPreview((INT8U *)cap.form, strlen((char *)cap.form), 0, 0, 0);
					arab_codetemp[strlen((char *)cap.form)] = 0;
					disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
					Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
					Dsp_Write_String(disp_p,(HUGEDATA char*)cap.form);
#endif
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
			if (plu_type==0) plu_type=1;

			sprintf(buffer, "%-5ld %1d %s", plu_no, plu_type,plu_name);
#ifdef USE_CL5200_DISPLAY_DEFAULT
			buffer[lcd_size+1] = 0;//PLU목록에서 PLU 이름 디스플레이 버그 수정
#else
			buffer[lcd_size] = 0;
#endif
			disp_p = point((line+Startup.menu_type)*Startup.menu_ygap, 0);
#ifdef USE_ARAB_FONT //list, 기존 아랍ROM 적용되어 있음
 #ifdef USE_ENG_BASED_ARAB_SUPPORT
			Convert_ArabPreview((INT8U *)buffer, strlen((char *)buffer), 0, 0, 1); 
			eng_codetemp[strlen((char *)buffer)] = 0;
			Dsp_Write_1stringLong(disp_p, eng_codetemp);
 #else
			Convert_ArabPreview((INT8U *)buffer, strlen(buffer), 0, 0, 0);
			arab_codetemp[lcd_size] = 0;
			disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
			Dsp_Write_1stringLong(disp_p, arab_codetemp);
 #endif
#elif defined(HEBREW_FONT)
			Convert_HebrewPreview((INT8U *)buffer, strlen(buffer), 0, 0);
			hebrew_codetemp[lcd_size] = 0;
			disp_p.x = 202 - disp_p.x;
			Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
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
			case KP_TEST:
					if(PrtStruct.Mode == RECEIPT) {
						print_plu_list(dept_no, plu_num[0], plu_num[1]);
					} else {
						print_plu_list_forlabel(dept_no, plu_num[0], plu_num[1]);
					}
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
#endif //#ifndef DISABLE_LIST_MENU


void plu_copy(void)
{
	plu_copy_move_manager(0x1141, 0);
}

void plu_move(void)
{
	plu_copy_move_manager(0x1143, 1);
}

void callback_copy_move_msg(long num)
{
	CAPTION_STRUCT cap;
	POINT disp_p;
	disp_p = point(CallbackEditPosition_y*Startup.menu_ygap, CallbackEditPosition_x*Startup.menu_xgap);
	memset((INT8U*)&cap.form, 0x20, sizeof(cap.form));
	cap.form[13]=0;
	Dsp_Write_String(disp_p,(HUGEDATA char *)cap.form);

	caption_split_by_code(0xF03C, &cap,0);	//"PLU exist !!"
	Dsp_Write_String(disp_p,(HUGEDATA char *)cap.form);
}

void plu_copy_move_manager(INT16U code, char flag)
{
	CAPTION_STRUCT 	cap;//,cap_plu;
	INT16S 	i;
	INT16U	result, key;
	INT32U  input_max[2], input_min[2];
	INT16U  input_len[2];
	INT32U	origin_data[3], destination_data[2];
	INT16U  cap1_id[] = {0x3141, 0x3142};
	INT16U  cap2_id[] = {0x3143, 0x3144};
	INT32U  ret_long;

	for(i = 0; i < 2; i++) {
		origin_data[i] = 1;
		destination_data[i] = 1;
	}
	origin_data[2] = 1;

	plu_get_display_info(PTYPE_DEPARTMENT_NO,&cap,0);
	input_max[0]=cap.input_max;
	input_min[0]=cap.input_min;
	input_len[0]=cap.input_dlength;

	plu_get_display_info(PTYPE_PLU_NO,&cap,0);
	input_max[1]=cap.input_max;
	input_min[1]=cap.input_min;
	input_len[1]=cap.input_dlength;

//modified by JJANG 20081121
LIST_START:
	Menu_Init();
#if !defined(USE_SINGLE_LINE_GRAPHIC)
	MenuNumLinePerPage = 2;
#endif

	caption_split_by_code(code, &cap,0);
	Menu_HeadCreate((HUGEDATA char *)cap.form);

	if (!Startup.disable_dept)
	{
		caption_split_by_code(cap1_id[0], &cap,0);
		cap.input_max=input_max[0];
		cap.input_min=input_min[0];
		cap.input_length=input_len[0];
		cap.input_dlength=input_len[0];
		caption_adjust(&cap, NULL);
		Menu_InputCreate(1, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
							cap.input_max, cap.input_min, 0, &origin_data[0], NULL, NULL, plucall_department);
	}

	caption_split_by_code(cap1_id[1], &cap,0);
	cap.input_max=input_max[1];
	cap.input_min=input_min[1];
	cap.input_length=input_len[1];
	cap.input_dlength=input_len[1];
	caption_adjust(&cap, NULL);
	Menu_InputCreate(2, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
						cap.input_max, cap.input_min, 0, &origin_data[1], NULL, NULL, plucall_plu_number);

	if (!Startup.disable_dept)
	{
		caption_split_by_code(cap2_id[0], &cap,0);
		cap.input_max=input_max[0];
		cap.input_min=input_min[0];
		cap.input_length=input_len[0];
		cap.input_dlength=input_len[0];
		caption_adjust(&cap, NULL);
		Menu_InputCreate(3, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
							cap.input_max, cap.input_min, 0, &destination_data[0], NULL, NULL, plucall_department);
	}

	caption_split_by_code(cap2_id[1], &cap,0);
	cap.input_max=input_max[1];
	cap.input_min=input_min[1];
	cap.input_length=input_len[1];
	cap.input_dlength=input_len[1];
	caption_adjust(&cap, NULL);
	Menu_InputCreate(4, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
						cap.input_max, cap.input_min, 0, &destination_data[1], NULL, NULL, plucall_plu_number);

	result = Menu_InputDisplay();
	if (result == MENU_RET_SAVE)
	{
		plu_table_search(origin_data[0], origin_data[1],&key,0);	// 복사할 PLU 유무 check
		if (key==0 )
		{
			error_display_caption(0xE00C);
			goto  LIST_START;
		}

		plu_table_search((unsigned char)destination_data[0],(long)destination_data[1],&key,0); //복사할 위치에 PLU유무 체크
		if (key==0) //PLU 없으면 바로 저장
		{
			goto RET_SAVE;
		}
		else
		{
			Menu_Init_WithPage(2);

			caption_split_by_code(code, &cap,0);
			Menu_HeadCreate((HUGEDATA char *)cap.form);
#ifdef _USE_LCD32_
			caption_split_by_code(0xf03c,&cap,0);
			Menu_InputCreate(1, ( char *)cap.form, MENU_TYPE_FIX, cap.input_x, cap.input_dlength, cap.input_length, 
								cap.input_max, cap.input_min, 0, NULL, NULL, NULL, NULL);            

   			caption_split_by_code(0x9500,&cap,0);
			caption_adjust(&cap, NULL);
			Menu_InputCreate(2, (HUGEDATA char *)cap.form, MENU_TYPE_YN, cap.input_x, cap.input_dlength, cap.input_length,
								cap.input_max, cap.input_min, 0, &ret_long, NULL, NULL, NULL);

#else
			caption_split_by_code(0x9500,&cap,0);
			caption_adjust(&cap, NULL);
			Menu_InputCreate(1, (HUGEDATA char *)cap.form, MENU_TYPE_YN, cap.input_x, cap.input_dlength, cap.input_length,
								cap.input_max, cap.input_min, 0, &ret_long, NULL, NULL, callback_copy_move_msg);
#endif            
			result = Menu_InputDisplay();
			if (result == MENU_RET_SAVE && ret_long == 1)
			{
				goto RET_SAVE;
			}
			else return;
		}
RET_SAVE:
		result = plu_copy_move_device((INT16S)origin_data[0], origin_data[1], (INT16S)destination_data[0], destination_data[1], flag);
		switch (result)
		{
		     case 0: error_display_caption(0xE401); break;
		     case 9: error_display_caption(0xF035); break;
		}
	}

}

/*
void plu_copy_move_manager(INT16U code, char flag)
{
	CAPTION_STRUCT 	cap;//,cap_plu;
	POINT 		disp_p;
//	INT8U	x_gab=6, y_gab=8;

//	char	str[200];
	INT16S 	i;
	INT16U	input_line, result, key, dept_code;
	long    input_max[2],input_min[2];
	INT16S     input_len[2];
	long	origin_data[3],  destination_data[2];
	long	ret_long;
	INT16U  cap1_id[]={0x3141,0x3142};
	INT16U  cap2_id[]={0x3143,0x3144};
//DEL060829	INT32U 	addr_cap;
	INT32U  addr;

	for(i=0; i<2; i++) {
		origin_data[i] = 1;
		destination_data[i]=1;
	}
	origin_data[2]=1;
	input_line = 0;
	plu_get_display_info(PTYPE_DEPARTMENT_NO,&cap,0);
	input_max[0]=cap.input_max;
	input_min[0]=cap.input_min;
	input_len[0]=cap.input_dlength;

	plu_get_display_info(PTYPE_PLU_NO,&cap,0);
	input_max[1]=cap.input_max;
	input_min[1]=cap.input_min;
	input_len[1]=cap.input_dlength;
PAGE1_VIEW:
	Dsp_Fill_Buffer(0);
	display_title_page_change(code, 1, 2);

	for(i=0; i<2; i++)  {
		ret_long = origin_data[i];
		caption_split_by_code(cap1_id[i],&cap,0);
		cap.input_max=input_max[i];
		cap.input_min=input_min[i];
		cap.input_length=input_len[i];
		cap.input_dlength=input_len[i];
		caption_adjust(&cap,NULL);
		if ((Startup.disable_dept) && (i==0)) goto NEXT_1;

		disp_p = point((i+Startup.menu_type)*Startup.menu_ygap, 0);
		Dsp_Write_String(disp_p,(HUGEDATA char *)cap.form);
		switch(i) {
			case 0:
				//plucall_department_position = strlen((char*)cap.form)+1;
				//plucall_department_pos_y=Startup.menu_type;
				CallbackEditPosition_x = (strlen((char *)cap.form)+1);
				CallbackEditPosition_y = Startup.menu_type;
				keyin_setcallback(plucall_department);
				break;
			case 1:
				//plucall_plu_position_x = strlen((char*)cap.form)+1;
				//plucall_plu_position_y = i+Startup.menu_type;
				CallbackEditPosition_x = (strlen((char *)cap.form)+1);
				CallbackEditPosition_y = Startup.menu_type + i;
				callback_dept_no = origin_data[0];
				keyin_setcallback(plucall_plu_number);
				break;
			case 2:
				break;
		}
		result=keyin_ulong(KI_GRAPHIC, KI_FORM_NORMAL,	0,(long*)&ret_long, cap.input_dlength,
			(i+Startup.menu_type)*Startup.menu_ygap,  cap.input_x*Startup.menu_xgap, 0,KI_PASS );
		keyin_clearcallback();
NEXT_1:;
	}
	Dsp_Diffuse();
	for(i=input_line; i<2; i++) {
		if ((Startup.disable_dept) && (i==0)) {
			origin_data[0]=1;
			i=1;
		};
		ret_long = origin_data[i];
		caption_split_by_code(cap1_id[i],&cap,0);
		cap.input_max=input_max[i];
		cap.input_min=input_min[i];
		cap.input_length=input_len[i];
		cap.input_dlength=input_len[i];
		caption_adjust(&cap,NULL);
		disp_p = point((i+Startup.menu_type)*Startup.menu_ygap, 0);
		Dsp_Write_String(disp_p,(HUGEDATA char *)cap.form);
PAGE1_KI_IN:
		switch(i) {
			case 0:
				CallbackEditPosition_x = (strlen((char *)cap.form)+1);
				CallbackEditPosition_y = Startup.menu_type;
				keyin_setcallback(plucall_department);
				break;
			case 1:
				callback_dept_no=origin_data[0];
			 	//plucall_plu_position_y = i+Startup.menu_type;
			 	//plucall_plu_position_x = strlen((char *)cap.form)+1;
				CallbackEditPosition_x = (strlen((char *)cap.form)+1);
				CallbackEditPosition_y = Startup.menu_type + i;
				keyin_setcallback(plucall_plu_number);
				break;
		}
		result=keyin_ulong(KI_GRAPHIC, KI_FORM_NORMAL,	0,(long*)&ret_long, cap.input_dlength,
			(i+Startup.menu_type)*Startup.menu_ygap,  cap.input_x*Startup.menu_xgap, 0,KI_NOPASS );
		keyin_clearcallback();
		if(result==KP_ESC || result==KP_PAGE_UP)  goto RET_ESC;
		if(cap.input_max < ret_long || cap.input_min > ret_long) {
			BuzOn(2);
			goto PAGE1_KI_IN;
		}
		origin_data[i] = ret_long;

		if(i==0) {
			addr =  DFLASH_BASE + FLASH_DEPARTMENT_AREA;
	 		addr = addr + DEPARTMENT_STRUCT_SIZE * (origin_data[0]-1);
			dept_code = Flash_wread(addr);
			if(dept_code != ret_long) {
				 BuzOn(2);	goto PAGE1_KI_IN;
			}
		}

		switch (result) {
       			case KP_ARROW_DN:
    			case KP_ENTER:
    				break;

	  	  	case KP_ARROW_UP:
				switch(i) {
					case 0:	goto RET_ESC;
					case 1:	input_line=0;	goto PAGE1_VIEW;
					case 2: input_line=1; 	goto PAGE1_VIEW;
				}
				break;

			case KP_PAGE_DN:
				input_line = 0;
		       		goto PAGE2_VIEW;

			case KP_SAVE:
				goto RET_END;

			default:	BuzOn(2);
					goto PAGE1_KI_IN;
		}
	}//page1_end

	input_line =0;
PAGE2_VIEW:
	key = plu_table_search(origin_data[0], origin_data[1],&key,0);
	if (key==0 ) {
		error_display_caption(0xE00C);
		i = 1;
		goto PAGE1_KI_IN;
	}
	Dsp_Fill_Buffer(0);
	display_title_page_change(code, 2, 2);
	for(i=0; i<2; i++)  {
		if ((Startup.disable_dept) && (i==0)) {
			destination_data[0]=1;
			i=1;
		};
		ret_long = destination_data[i];
		caption_split_by_code(cap2_id[i],&cap,0);
		cap.input_max    =input_max[i];
		cap.input_min    =input_min[i];
		cap.input_length =input_len[i];
		cap.input_dlength=input_len[i];
		caption_adjust(&cap,NULL);
		disp_p = point((i+Startup.menu_type)*Startup.menu_ygap, 0);
		Dsp_Write_String(disp_p,(HUGEDATA char *)cap.form);
		switch(i) {
			case 0:
				//plucall_department_position = strlen((char*)cap.form)+1;
				//plucall_department_pos_y=Startup.menu_type;
				CallbackEditPosition_x = (strlen((char *)cap.form)+1);
				CallbackEditPosition_y = Startup.menu_type;
				break;
			case 1:
				callback_dept_no=destination_data[0];
				//plucall_plu_position_x = strlen((char*)cap.form)+1;
				//plucall_plu_position_y = (i+Startup.menu_type);
				CallbackEditPosition_x = (strlen((char *)cap.form)+1);
				CallbackEditPosition_y = Startup.menu_type + i;
				keyin_setcallback(plucall_plu_number);
				break;
		}
		result=keyin_ulong(KI_GRAPHIC, KI_FORM_NORMAL,	0,(long*)&ret_long, cap.input_dlength,
					   (i+Startup.menu_type)*Startup.menu_ygap,  cap.input_x*Startup.menu_xgap, 0,KI_PASS );
		keyin_clearcallback();
	}
	Dsp_Diffuse();
	for(i=input_line; i<2; i++) {
		if ((Startup.disable_dept) && (i==0)) {
			destination_data[0]=1;
			i=1;
		};
		ret_long = destination_data[i];
		caption_split_by_code(cap2_id[i],&cap,0);
		cap.input_max=input_max[i];
		cap.input_min=input_min[i];
		cap.input_length=input_len[i];
		cap.input_dlength=input_len[i];
		caption_adjust(&cap,NULL);
		disp_p = point((i+Startup.menu_type)*Startup.menu_ygap, 0);
		Dsp_Write_String(disp_p,(HUGEDATA char *)cap.form);
PAGE2_KI_IN:
		if ((Startup.disable_dept) && (i==0)) {
			destination_data[0]=1;
			i=1;
		};
		switch(i) {
			case 0:
				CallbackEditPosition_x = strlen((char *)cap.form) + 1;
				CallbackEditPosition_y = Startup.menu_type;
				keyin_setcallback(plucall_department);
				break;
			case 1:	callback_dept_no=destination_data[0];
				CallbackEditPosition_x = strlen((char *)cap.form) + 1;
				CallbackEditPosition_y = Startup.menu_type + 1;
				keyin_setcallback(plucall_plu_number);
				break;
		}
		result=keyin_ulong(KI_GRAPHIC, KI_FORM_NORMAL,	0,(long*)&ret_long, cap.input_dlength,
					   (i+Startup.menu_type)*Startup.menu_ygap,  cap.input_x*Startup.menu_xgap, 0,KI_NOPASS );
		keyin_clearcallback();

		if(result==KP_ESC)  goto RET_ESC;
		if(cap.input_max < ret_long || cap.input_min > ret_long) {
			BuzOn(2);
			goto PAGE2_KI_IN;
		}
		destination_data[i] = ret_long;
		if(i==0) {
			addr =  DFLASH_BASE + FLASH_DEPARTMENT_AREA;
	 		addr = addr + DEPARTMENT_STRUCT_SIZE * (destination_data[0]-1);
			dept_code = Flash_wread(addr);
			if(dept_code != ret_long) {
				 BuzOn(2);
				 goto PAGE2_KI_IN;
			}
		}
		switch (result) {
       			case KP_ARROW_DN:
    			case KP_ENTER:
    				break;

	  	  	case KP_ARROW_UP:
				switch(i) {
					case 0:	input_line=1; goto PAGE1_VIEW;
					case 1:	input_line=0; goto PAGE2_VIEW;
				}
				break;

			case KP_PAGE_UP:
				input_line =0 ;
				goto PAGE1_VIEW;

			case KP_PAGE_DN:
			case KP_SAVE:
				goto RET_END;

			default:	BuzOn(2);
					goto PAGE2_KI_IN;

		}
	}
	if (result == KP_ARROW_DN) {
		i = 1;
		goto PAGE2_KI_IN;
	}

RET_END:
	plu_table_search((unsigned char)destination_data[0],(long)destination_data[1],&key,0);
	if (key==0) {
		goto RET_SAVE;
	} else {
		Dsp_Fill_Buffer(0);
		display_title_page_change(code, 3, 3);
		disp_p=point(Startup.menu_type*Startup.menu_ygap, 0*Startup.menu_xgap);
//		addr_cap = caption_search_by_code(0xF03c);
//		memcpy((INT8U*)&cap, (INT8U*)addr_cap,	 sizeof(cap));
		caption_split_by_code(0xf03c,&cap,0);
		Dsp_Write_String(disp_p,(HUGEDATA char*)cap.form);

		disp_p=point((Startup.menu_type+1)*Startup.menu_ygap, 0*Startup.menu_xgap);
//		addr_cap = caption_search_by_code(0x9500);
//		memcpy((INT8U*)&cap, (INT8U*)addr_cap,	 sizeof(cap));
		caption_split_by_code(0x9500,&cap,0);
		caption_adjust(&cap,NULL);
		Dsp_Write_String(disp_p,(HUGEDATA char*)cap.form);
		Dsp_Diffuse();
PAGE3_KI_IN:
		ret_long = 1;
		result=keyin_yesno(KI_GRAPHIC,(long *)&ret_long,(1+Startup.menu_type)*Startup.menu_ygap,cap.input_x*Startup.menu_xgap,KI_NOPASS);
	        if(result==KP_ESC)  goto RET_ESC;
		if((ret_long<0) || (ret_long>1)) {
			BuzOn(2);
			goto PAGE2_KI_IN;
		}
		switch (result) {
	  	  	case KP_ARROW_UP:
			case KP_PAGE_UP:
				input_line=0;
				goto PAGE2_VIEW;
			case KP_ENTER:
			case KP_SAVE :
				if(ret_long==1)	goto RET_SAVE;
				else		goto RET_ESC;
			default:	BuzOn(2);
					goto PAGE3_KI_IN;

		}
	}
RET_SAVE:
	result=plu_copy_move_device((INT16S)origin_data[0], origin_data[1], (INT16S)destination_data[0], destination_data[1], flag);
	switch (result) {
	     case 0: error_display_caption(0xE401); break;
	     case 9: error_display_caption(0xF035); break;
	}
RET_ESC:
	Dsp_Fill_Buffer(0);						// LCD Buffer clear
	Dsp_Diffuse();
 	return;
}
*/

void plu_del_all(void)
{
// bmlim -- 2004.05.22
	INT32U value;
	CAPTION_STRUCT cap;
	INT8U result;
	POINT disp_p;
	INT8U xret;

	Menu_Init();
	caption_split_by_code(0x1149, &cap,0);
	Menu_HeadCreate((HUGEDATA char *)cap.form);

	value=0;
	caption_split_by_code(0x9500, &cap,0);
	caption_adjust(&cap,NULL);
	Menu_InputCreate(1, (HUGEDATA char *)cap.form, MENU_TYPE_YN, cap.input_x, cap.input_dlength, cap.input_length,
						cap.input_max, cap.input_min, 0, &value, NULL, NULL, NULL);
	result = Menu_InputDisplay();

	if(result != MENU_RET_SAVE) goto RET_ESC;
	if (value) {
		xret=get_dc_any_plu_no(0);
		if (xret) {
			Dsp_Fill_Buffer(0);
			caption_split_by_code(0xf051, &cap,0);
			disp_p = point( (Startup.menu_type)*Startup.menu_ygap, 0);
#ifdef USE_ARAB_CONVERT
            Convert_ArabPreview((INT8U *)cap.form, strlen(cap.form), 0, 0, 0); 
			arab_codetemp[strlen(cap.form)] = 0;
			disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
			Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
			Dsp_Write_String(disp_p,(HUGEDATA char *)cap.form);
#endif
			caption_split_by_code(0xf03e, &cap,0);
			disp_p = point( (Startup.menu_type+1)*Startup.menu_ygap, 0);
#ifdef USE_ARAB_CONVERT
            Convert_ArabPreview((INT8U *)cap.form, strlen(cap.form), 0, 0, 0); 
            arab_codetemp[strlen(cap.form)] = 0;
            disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
            Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
			Dsp_Write_String(disp_p,(HUGEDATA char *)cap.form);
#endif
			Dsp_Diffuse();
			Delay_100ms(20);
			goto RET_ESC;
		}
		//plu_all_delete(NULL,9);
		plu_all_delete(NULL,10);
	}
RET_ESC:;
// -------------------
}

void plu_del_by_plu_no(void)	  // function code : 1161
{
//	char	str[64];
	INT8U  	ret;
	INT16U 	deptno ;
	INT32U 	pluno;

	deptno = status_scale.departmentid;

	pluno=1;
	Dsp_Fill_Buffer(0);

PAGE_KI_IN:
	display_title_page_change(0x1147,1,1);
	// <CODE> JHC, 040702, Modify, - KP_ARROW_DN : 1 = NoReturn, 0 = Return
	// [
	g_i8uInputDeptReturn = 1;
	ret = plu_input_department(&deptno, &pluno, 0);
	g_i8uInputDeptReturn = 0;
	// ]
	if(ret==2) {
		goto RET_ESC;
	}
	else if (ret == KP_ARROW_DN ||  ret == KP_PAGE_DN)
		goto PAGE_KI_IN;

	if (deptno<1) {
		goto RET_ESC;
	}
	plu_delete(deptno, pluno,0);
//	if (network_status.service_flag&0x0004) {
//		if ((ethernet_list.status==1) ||
//		    ((ethernet_list.status==2) && (network_status.service_flag&0x10)))
//		command_set(0x11,0,deptno,pluno,0,0,NULL);
//	}
RET_ESC:
	Dsp_Fill_Buffer(0);   // LCD Buffer clear
	Dsp_Diffuse();
	return;
}

void plu_del_by_department(void)		// function code : 1162
{
	INT16U ret,xret;
//	INT32U ret_long;
	INT16U deptno;
	CAPTION_STRUCT cap;
	POINT  disp_p;
//	unsigned char temp[64];	//SGDebug

	if (Startup.disable_dept) return;
	Dsp_Fill_Buffer(0);						// LCD Buffer clear
	deptno=1;

PAGE_KI_IN:
	display_title_page_change(0x1148,1,1);
	// <CODE> JHC, 040702, Modify
	// [
	g_i8uInputDeptReturn = 1;
	ret = plu_input_department_only(&deptno, 0);
	g_i8uInputDeptReturn = 0;
	// ]
	//ret_long = (INT32U)deptno;

	if(ret==2) {
		goto RET_ESC;
	}
	else if (ret == KP_ARROW_DN ||
		 ret == KP_PAGE_DN)
	{
		goto PAGE_KI_IN;
	}
	if (deptno<1) {
		goto RET_ESC;
	}
	xret=get_dc_any_plu_no(deptno);
	if (xret) {
		Dsp_Fill_Buffer(0);
		caption_split_by_code(0xf051, &cap,0);
		disp_p = point( (Startup.menu_type)*Startup.menu_ygap, 0);
		Dsp_Write_String(disp_p,(HUGEDATA char *)cap.form);
		caption_split_by_code(0xf03e, &cap,0);
		disp_p = point( (Startup.menu_type+1)*Startup.menu_ygap, 0);
		Dsp_Write_String(disp_p,(HUGEDATA char *)cap.form);
		Dsp_Diffuse();
		Delay_100ms(20);
		goto RET_ESC;
	}

	plu_delete_department((unsigned char)deptno);

//SGDebug-S
//	sprintf(temp,"SG:ret=%d, depno=%d\r\n",ret,deptno);
//	MsgOut(temp);
//SGDebug-E

//	if (network_status.service_flag&0x0004) {
//		if ((ethernet_list.status==1) ||
//		    ((ethernet_list.status==2) && (network_status.service_flag&0x10)))
//		command_set(0x12,0,(unsigned char)ret_long,0,0,0,NULL);
//	}
RET_ESC:
	return;
}

//DEL071022void plu_tare_inhibit(void)
//DEL071022{
//DEL071022	INT8U sale_flag;
//DEL071022	sale_flag=get_global_bparam(GLOBAL_SALE_SETUP_FLAG2);
//DEL071022	if (sale_flag&0x02) {
//DEL071022		temp_plu_inhibit_list[12]=0;
//DEL071022		temp_plu_inhibit_list[13]=13;
//DEL071022	} else {
//DEL071022		temp_plu_inhibit_list[12]=12;
//DEL071022		temp_plu_inhibit_list[13]=0;
//DEL071022	}
//DEL071022}

//void plu_tare_inhibit_set(void)
//{
//	INT8U sale_flag;
//	sale_flag=get_global_bparam(GLOBAL_SALE_SETUP_FLAG2);
//	if (sale_flag&0x02) {
//		if (status_scale.restrict&FUNC_TARE_TBL) plu_inhibit_bycode(12,0);
//		else                                     plu_inhibit_bycode(12,1);
//		if ((!plu_check_inhibit_ptype(PTYPE_TARE_VALUE)) && (!plu_check_inhibit_ptype(PTYPE_TARE_NO))) {
//			plu_inhibit_bycode(PTYPE_TARE_VALUE,1);
//		}
//	} else {
//		plu_inhibit_bycode(PTYPE_TARE_NO,1);
//		if ((!plu_check_inhibit_ptype(PTYPE_TARE_VALUE)) && (!plu_check_inhibit_ptype(PTYPE_TARE_NO))) {
//			plu_inhibit_bycode(PTYPE_TARE_VALUE,0);
//		}
//	}
//}

INT16S plu_type_narray(INT16S m,INT16S *ret)
{
	INT16S r;
	INT8U flag;
	flag=get_global_bparam(GLOBAL_SALE_SETUP_FLAG3);
#ifdef USE_EMART_NETWORK
	r=6;
#else
	r=3;
#endif
	if (!(flag&0x01) ) r--;
/*
	*ret=m;
	if (m) {
		if (!(flag&0x01)) {
			if (m>1) (*ret)++;
		}
	}
*/
	*ret=m;
	if (m) {
		if (!(flag&0x01)) {
			if (m == PLU_BY_PCS) *ret=PLU_BY_COUNT;
#ifdef USE_FIXED_PLUTYPE//USE_EMART_NETWORK
			else if (m == PLU_BY_PCS_FIX) *ret=PLU_BY_COUNT_FIX;
#endif
		}
	}
	return r;
}

/*
INT16S plu_type_narray_reverse(INT16S m,INT16S *ret)
{
	INT16S r;
	INT8U flag;
	flag=get_global_bparam(GLOBAL_SALE_SETUP_FLAG3);
	r=3;
	if (!(flag&0x01) ) r--;
	*ret=m;
	if (m) {
		if (!(flag&0x01)) {
			if (m>2) (*ret)=2;
		}
	}
	return r;
}
*/



INT8U plu_inhibit_impossible(INT16U ptype)
{
	INT8U ret;
	INT8U tare_flag;

	ptype&=0x7fff;
	ret=0;
	// RET=1 은 Disable 할수 없다.
	// RET=2 는 초기화시 Global, Parameter 의 영향을 받는 것
	switch (ptype) {
		case PTYPE_DEPARTMENT_NO: ret=1; break;
		case PTYPE_PLU_NO: ret=1; break;
		case PTYPE_PLU_TYPE: ret=1; break;
		case PTYPE_UNIT_WEIGHT: ret=2; break;
		case PTYPE_PRICE: ret=1; break;
		case PTYPE_PLU_NAME1: ret=1; break;
		case PTYPE_TARE_NO: if (!(status_scale.restrict&FUNC_TARE_TBL)) {
				ret=2; break;
			 }
			 tare_flag=get_global_bparam(GLOBAL_SALE_SETUP_FLAG2);
			 if (tare_flag&0x02) {
				ret=0;
			 } else {
				ret=2;
			 }
			 break;
		case PTYPE_TARE_VALUE: tare_flag=get_global_bparam(GLOBAL_SALE_SETUP_FLAG2);
			 if (tare_flag&0x02) {
				ret=2;
			 } else {
				ret=0;
			 }
			 break;
		case PTYPE_PIECE: ret=1; break;
		case PTYPE_QTYUNIT_NO: ret=1; break;
#ifndef USE_PLU_MULTIPLE_PRICE	//Refefence PLU 기능 사용 안 함
#ifdef USE_AUSTRALIA_COOL_FUNCTION
		case PTYPE_LINK1_DEPT_NO: ret=1; break;
		case PTYPE_LINK2_DEPT_NO: ret=1; break;
		case PTYPE_LINK1_PLU_NO: ret=1; break;
		case PTYPE_LINK2_PLU_NO: ret=1; break;
		case PTYPE_COUPLE_DEPT_NO: ret=1; break;
		case PTYPE_COUPLE_PLU_NO: ret=1; break; 	
		case PTYPE_INGREDIENT_TEXT: ret=1; break;
		case PTYPE_NO_OF_LINK_PLU: ret=1; break;
#else
		case PTYPE_LINK1_DEPT_NO: ret=1; break;
		case PTYPE_LINK2_DEPT_NO: ret=1; break;
		case PTYPE_LINK1_PLU_NO: ret=1; break;
		case PTYPE_LINK2_PLU_NO: ret=1; break;
		case PTYPE_COUPLE_DEPT_NO:
		case PTYPE_REF_DEPT_NO: if (Startup.disable_dept) ret=2; break;
#endif
#endif
		case PTYPE_LABEL_NO: ret=2; break;
		case PTYPE_ORIGIN_NO: if (!(status_scale.restrict&FUNC_ORG_TBL)) ret=2; break;
		case PTYPE_TAX_NO: if (!(status_scale.restrict&FUNC_TAX_TBL)) ret=2; break;
#ifdef USE_DISCLOSURE_MEATGRADE	// 개체 및 PLU 등급 동시 사용, 개체가 우선순위를 가짐.
		case PTYPE_GROUP_NO: ret=1; break;
#else
		case PTYPE_GROUP_NO: if (!(status_scale.restrict&FUNC_GROUP  )) ret=2; break;
#endif
		case PTYPE_TRACE_NO: if (!(status_scale.restrict&FUNC_TRACE  )) ret=2; break;
		case PTYPE_NUTRITION_NO: if (!(status_scale.restrict&FUNC_NUT_TBL)) ret=2; break;
#ifdef USE_AUSTRALIA_COOL_FUNCTION
		case PTYPE_BONUS: ret=1; break;
#else
		case PTYPE_BONUS: if (!(status_scale.restrict&FUNC_CUSTOM )) ret=2; break;
#endif
		case PTYPE_BARCODE_NO: ret=2; break;
#ifdef USE_PCS_FONT5_TO_TRACE_CODE	// USE PCS, PLUFONT5 field for Trace CODE 
		case PTYPE_PLU_NAME5_FONT: ret=1; break;
#endif
	}
	return ret;
}

//void plu_ptype_enable_code(void)
//{
//	status_scale.restrict=get_global_wparam(GLOBAL_ACTION_ENABLE);
//	Startup.disable_dept=1;
//	if (status_scale.restrict&FUNC_DEPTNO) Startup.disable_dept=0;
//	if (status_scale.restrict&FUNC_ORG_TBL ) {
//		plu_inhibit_bycode( 55,0);
//	} else {
//		plu_inhibit_bycode( 55,1);
//	}
//	if (status_scale.restrict&FUNC_TAX_TBL ) {
//		plu_inhibit_bycode(  8,0);
//	} else {
//		plu_inhibit_bycode(  8,1);
//	}
//	if (status_scale.restrict&FUNC_TARE_TBL) {
//		plu_inhibit_bycode( 12,0);
//	}
//	if (status_scale.restrict&FUNC_TRACE   ) {
//		plu_inhibit_bycode( 35,0);
//	} else {
//		plu_inhibit_bycode( 35,1);
//	}
//	if (status_scale.restrict&FUNC_NUT_TBL ) {
//		plu_inhibit_bycode( 70,0);
//	} else {
//		plu_inhibit_bycode( 70,1);
//	}
//	if (status_scale.restrict&FUNC_CUSTOM  ) {
//		plu_inhibit_bycode( 50,0);
//	} else {
//		plu_inhibit_bycode( 50,1);
//	}
//	if (Startup.disable_dept) {
//		plu_inhibit_bycode(71,1);
//		plu_inhibit_bycode(64,1);
//		plu_inhibit_bycode(61,1);
//		plu_inhibit_bycode(62,1);
//	} else {
////		plu_inhibit_bycode(71,0);
////		plu_inhibit_bycode(64,0);
////		plu_inhibit_bycode(61,0);
////		plu_inhibit_bycode(62,0);
//	}
//	plu_inhibit_bycode(1,0);
//}

void plu_inhibit_step(void)
{
	CAPTION_STRUCT	cap;
	char	buffer[36], string_buf[36];
	long	ret_ptype;
	INT16S     k,sx;
	INT8U	str_cnt;
	INT16U	maxid, i, result, lcd_size;
	INT8U   line_id[128];
	INT32U  set_val[128];
	INT32U	addr_ptype, addr; //, addr_cap;
	INT32U  addr_name, naddr;
	INT16U ptype;
#ifdef USE_THAI_FONT
	INT8U	dsp_str_len;
#endif

	addr_ptype = DFLASH_BASE + FLASH_PLU_STRUCT_AREA;
	addr_ptype += PLU_STRUCT_POINT;
	addr_ptype += PLUSTR_PTYPE_POINT;
	addr_name   = DFLASH_BASE + FLASH_PLU_STRUCT_CAPTION_AREA;
	if( !permission_check(PMS_KEY_INHIBIT,0)) return;
	lcd_size = Dsp_GetChars();

	k = 0;
	for (i=0; i<plu_n_field; i++) {
		addr = addr_ptype + PLU_STRUCT_SIZE * i;
		ret_ptype = Flash_wread(addr);
		if (plu_inhibit_impossible(ret_ptype)) continue;
		ptype = ret_ptype&0x7fff;
#ifdef USE_CHINA_CARRFOUR_DEFAULT
		if (ptype == PTYPE_IMAGE_NO) continue; //image unuse
#else
  #ifndef USE_TOPMART_STRUCTURE
		if (ptype == PTYPE_IMAGE_NO || ptype == PTYPE_BONUS) continue; //image, bonus unuse
  #endif
#endif
#ifndef USE_TRACEABILITY
		if (ptype == PTYPE_TRACE_NO) continue;
#endif
#if defined(USE_USA_INGREDIENT) || defined(USE_NORTH_AMERICA_INGREDIENT)
		// pass ingredient no. step
		if (ptype == PTYPE_INGREDIENT_NO) continue;
#endif
#ifdef USE_CHINA_DEFAULT
		if(ptype == PTYPE_SELLBY_DATE_FLAG) continue;
		if(ptype == PTYPE_SELLBY_TIME_FLAG) continue;
		if(ptype == PTYPE_PACKCED_DATE_FLAG) continue;
		if(ptype == PTYPE_PACKCED_TIME_FLAG) continue;

		if(ptype == PTYPE_PLU_NAME1_FONT) continue;
		if(ptype == PTYPE_PLU_NAME2_FONT) continue;
		if(ptype == PTYPE_PLU_NAME3_FONT) continue;
		if(ptype == PTYPE_PLU_NAME4_FONT) continue;
		if(ptype == PTYPE_PLU_NAME5_FONT) continue;
		if(ptype == PTYPE_PLU_NAME6_FONT) continue;
		if(ptype == PTYPE_PLU_NAME7_FONT) continue;
		if(ptype == PTYPE_PLU_NAME8_FONT) continue;
#endif
		line_id[k] = (INT8U)i;
		if (ret_ptype&0x8000) set_val[k] = 0;	// No(inhibit)
		else set_val[k] = 1;			// Yes(inhibit)
		k++;
	}
	maxid = k;

	Menu_Init();

	caption_split_by_code(0x1144, &cap,0);
	Menu_HeadCreate((HUGEDATA char *)cap.form);

	for(i = 0; i < maxid; i++)  {
		str_cnt = line_id[i];
		if(str_cnt >= plu_n_field) break;
		naddr = addr_name + PLUSTR_NAME_SIZE * (INT32U)str_cnt;
		Flash_sread(naddr, (HUGEDATA INT8U*)&string_buf, PLUSTR_NAME_SIZE);
#ifdef USE_PERCENT_WEIGHT	//1144에서 reserved 안보이도록 처리
		if(strcmp(string_buf,"reserved") ==0 ) continue;
#endif
#ifdef USE_SRILANKA_CERTI
		if(strcmp(string_buf,"FixedWeight") == 0 ) continue;
		if(strcmp(string_buf,"Prefix") == 0 ) continue;
		if(strcmp(string_buf,"Use Fixed Price Type") == 0 ) continue;
		if(strcmp(string_buf,"SpecialPrice") == 0 ) continue;
		if(strcmp(string_buf,"Tare") == 0 ) continue;
		if(strcmp(string_buf,"%Tare") == 0 ) continue;
		if(strcmp(string_buf,"Tare % limit") == 0 ) continue;
#endif
#ifdef USE_RUSSIA_LOGISTICS_PLU	//1144에서 reserved 안보이도록 처리
		if(strcmp(string_buf,"Reserved2") ==0 ) continue;
		if(strcmp(string_buf,"Reserved3") ==0 ) continue;
#endif
#ifdef USE_PRICE_CHANGEABLE_CHECK
		addr = addr_ptype + PLU_STRUCT_SIZE * str_cnt;
		ret_ptype = Flash_wread(addr);
		if(ret_ptype == PTYPE_TAX_NO) sprintf(string_buf,"가격변경제한");
#endif
#ifdef HEBREW_FONT
		sprintf(buffer, "%-14s: ]", string_buf);
#else
		sprintf(buffer, "%-14s: [", string_buf);
#endif
#ifdef USE_ARAB_CONVERT
		sx = Convert_ArabStrLength((char *)buffer);
#elif defined(USE_THAI_FONT)
        dsp_str_len = Cvt_ThaiStrLength((INT8U*)buffer); 
#else
		sx = strlen((char *)buffer);
#endif
#ifdef HEBREW_FONT
		sprintf(&buffer[sx]," [");
#else
		sprintf(&buffer[strlen((char *)buffer)]," ]");
#endif
		buffer[lcd_size] = 0;
#ifdef USE_THAI_FONT
		sx = dsp_str_len; 
#endif  
		Menu_InputCreate(2, (HUGEDATA char *)buffer, MENU_TYPE_YN, sx, 1, 1,
							1, 0, 0, &set_val[i], NULL, NULL, NULL);
	}

	result = Menu_InputDisplay();
	if (result == MENU_RET_SAVE)
	{
		for(i = 0; i < maxid; i++)  {
			str_cnt = line_id[i];
			if (str_cnt >= plu_n_field) break;
			plu_set_usefield(str_cnt+1, set_val[i]);
		}
		plu_ptype_reset();
		// 최적화 수정 필요(CJK) ////////////////////
		// plu_check_inhibit_ptype() 에서 plu_ptype_reset()에서의 결과(plu_global[p].inhibit)를
		// 재사용하기 때문에 먼저 수행하여 검사한뒤
		// 아래 루틴이 끝 나면 다시 한번 수행해야 한다.
#ifndef USE_PLU_MULTIPLE_PRICE	//Refefence PLU 기능 사용 안 함
#ifndef USE_AUSTRALIA_COOL_FUNCTION
		if (plu_check_inhibit_ptype(PTYPE_NO_OF_LINK_PLU)) {
			plu_inhibit_bycode(PTYPE_LINK1_DEPT_NO,1);
			plu_inhibit_bycode(PTYPE_LINK2_DEPT_NO,1);
			plu_inhibit_bycode(PTYPE_LINK1_PLU_NO,1);
			plu_inhibit_bycode(PTYPE_LINK2_PLU_NO,1);
		} else {
			plu_inhibit_bycode(PTYPE_LINK1_DEPT_NO,0);
			plu_inhibit_bycode(PTYPE_LINK2_DEPT_NO,0);
			plu_inhibit_bycode(PTYPE_LINK1_PLU_NO,0);
			plu_inhibit_bycode(PTYPE_LINK2_PLU_NO,0);
		}
#endif
#endif
		plu_ptype_reset();
	}
	return;
}


INT8U plu_inhibit_bycode(INT16U code,INT8U onoff)
{
	INT32U addr,addr_p;
	INT16U ptype,stype;
	INT8U  ret;
	INT16S i;
	addr = DFLASH_BASE + FLASH_PLU_STRUCT_AREA;
	addr += PLU_STRUCT_POINT;
	ret=0;
	if(!plu_n_field)
	{
		plu_n_field = plu_struct_get_nfield();
	}
	for (i=0; i<plu_n_field; i++) {
		addr_p = i * PLU_STRUCT_SIZE;
		addr_p+= addr;
		ptype = Flash_wread(addr_p + PLUSTR_PTYPE_POINT);
		stype = ptype;
		ptype = ptype & 0x7fff;
		if (code==0xff) {
			if (onoff) ptype=ptype|0x8000;
			if (stype!=ptype) {
				Flash_wwrite(addr_p + PLUSTR_PTYPE_POINT,ptype);
				ret=1;
			}
		} else {
			if (ptype==code) {
				if (onoff) ptype=ptype|0x8000;
				if (stype!=ptype) {
					Flash_wwrite(addr_p + PLUSTR_PTYPE_POINT,ptype);
					ret=1;
				}
				break;
			}
		}
	}
	return ret;
}

void plu_set_usefield(INT16U index, INT8U use)	// use = 0:inhibit, 1:not inhibit
{
	INT32U	addr_plu_struct;
	INT16U	ptype;

	addr_plu_struct  = DFLASH_BASE;
	addr_plu_struct += FLASH_PLU_STRUCT_AREA;
	addr_plu_struct += PLU_STRUCT_POINT;
	addr_plu_struct += PLUSTR_PTYPE_POINT;

	if(index <=0) return;
	addr_plu_struct += (PLU_STRUCT_SIZE * (index-1));
	ptype = Flash_wread(addr_plu_struct);
	if (use)
	{
		ptype = ptype & 0x7fff;		// enable(not inhibit)
	}
	else
	{
		ptype = ptype | 0x8000;		// disable(inhibit)
	}
	Flash_wwrite(addr_plu_struct, ptype);
}


void plu_inhibit_callback(INT16S num)
{
	POINT 			disp_p;
	INT8U	buffer[34];
	INT32U	addr;
	INT32U  naddr;
//	INT16U	maxid;
	INT16U	ret_val;

	//maxid = plu_n_field;

	disp_p = point((1+Startup.menu_type)*Startup.menu_ygap, 3*Startup.menu_xgap);

	memset((INT8U *)&buffer,0x20, sizeof(buffer) );
	buffer[33] = 0;
	DspStruct.DirectDraw = 1;
	Dsp_Write_String(disp_p,(HUGEDATA char *)buffer);

	ret_val = plu_get_ptype_str_point(num);
	num     = ret_val;
	if(ret_val == 0) {
		return;
	}
       	addr  = DFLASH_BASE;
	addr += FLASH_PLU_STRUCT_AREA;
	addr += PLU_STRUCT_POINT;
	addr += PLU_STRUCT_SIZE * (num-1);
       	naddr  = DFLASH_BASE;
	naddr += FLASH_PLU_STRUCT_CAPTION_AREA;
	naddr += PLUSTR_NAME_SIZE * (num-1);

	Flash_sread(naddr,(HUGEDATA INT8U*)buffer, 34);
	buffer[33] = 0;

	DspStruct.DirectDraw = 1;
	DspStruct.Reverse = 1;
	Dsp_Write_String(disp_p,(HUGEDATA char *)buffer);
	DspStruct.Reverse = 0;

}

void plu_unit_price_change(void)
{
	plu_edit_price_sub(0,0,0);
}


INT16U Menu_CallbackDeptNo = 0;
INT32U Menu_CallbackPluNo = 0;
void callback_end_chageunit_dept(INT32U num)
{
	Menu_CallbackDeptNo = num;
}


void callback_end_chageunit_plu(INT32U num)
{
	INT8U	unitprice_id;
	INT8U	pcs_id;
	INT32U	plu_unitprice;
	INT16U	plu_pcs;
	INT8U 	plu_ptype;
	INT16U	plu_addr;
	INT16S	size, pcs_size;

	unitprice_id = Menu_GetMenuId(6);
	if(unitprice_id == 0xff) return;

	pcs_id = Menu_GetMenuId(14);
	if(pcs_id == 0xff) return;

	Menu_CallbackPluNo = num;

	plu_table_search((INT8U)Menu_CallbackDeptNo, num, &plu_addr, 0);
	if (plu_addr == 0)
	{
		plu_ptype = 1;
		plu_unitprice = 0;
		plu_pcs = 1;
//		Menu[unitprice_id].disable = ON;
//		Menu[pcs_id].disable       = ON;
		Menu[unitprice_id].inhibit = ON;
		Menu[pcs_id].inhibit       = ON;
	}
	else
	{
		plu_get_field(plu_addr-1, PTYPE_PLU_TYPE, (INT8U *)&plu_ptype,     &size,sizeof(plu_ptype));
		plu_get_field(plu_addr-1, PTYPE_PRICE, (INT8U *)&plu_unitprice, &size,sizeof(plu_unitprice));
		plu_get_field(plu_addr-1, PTYPE_PIECE, (INT8U *)&plu_pcs,       &pcs_size,sizeof(plu_pcs));

#ifdef USE_FIXED_PLUTYPE//USE_EMART_NETWORK
		if (plu_ptype != PLU_BY_PCS && plu_ptype != PLU_BY_PCS_FIX)
#else
		if (plu_ptype != PLU_BY_PCS)
#endif
		{
			Menu[unitprice_id].inhibit = OFF;
			Menu[pcs_id].inhibit       = ON;
			*Menu[unitprice_id].pval = plu_unitprice;
		}
		else
		{
			Menu[unitprice_id].inhibit = OFF;
			Menu[pcs_id].inhibit       = OFF;
			*Menu[unitprice_id].pval = plu_unitprice;
			//*Menu[pcs_id].pval = plu_pcs;
			memcpy((void *)Menu[pcs_id].pval, (void *)&plu_pcs, pcs_size);
		}
	}

	MenuRefresh = ON;
}

INT8U plu_edit_price_sub(INT8U dept,INT32U iplu,INT8U mode)
{
	CAPTION_STRUCT 		cap;
	//TRANSACTION_RECORD_TYPE pTrans;
	INT16S    	size, decpoint;
	INT16U 	result;
	INT8U	ret_value = 0;

	INT32U	plu_unitprice;
	INT16U	plu_pcs;
	INT8U 	plu_ptype;
#ifdef USE_EMART_CHANGE_DATA_AT_NON_SALE
	UNITPRICE_CHANGE_TYPE chgprice;
	TR_ADDITION_DATA_TYPE trans_add;
	INT32U 	Reason_code = 0;
	//INT8U	chg_flag;
	INT32U	OriginUnitPrice;
	INT32U	ResultUnitPrice;
	INT8U	changeFlag;
#endif
//	INT16S	stat;

	INT32U	dept_no;
	INT32U	plu_no;
	INT16U	plu_addr;


	//chg_flag = get_global_bparam(GLOBAL_SALE_SETUP_FLAG8) & 0x40;	//parameter 723
	if (!mode) {
		dept_no = status_scale.departmentid;
		plu_no = 1;
	}  else {
		dept_no = (INT32U)dept;
		plu_no  = iplu;
	}
	if (Startup.disable_dept)
	{
		dept_no = 1;
	}

	plu_table_search((INT8U)dept_no, plu_no,&plu_addr,0);
	if (plu_addr == 0)
	{
		plu_ptype = 1;
		plu_unitprice = 0;
		plu_pcs = 1;
	}
	else
	{
		plu_get_field(plu_addr-1, PTYPE_PLU_TYPE, (INT8U *)&plu_ptype,     &size,sizeof(plu_ptype));
		plu_get_field(plu_addr-1, PTYPE_PRICE, (INT8U *)&plu_unitprice, &size,sizeof(plu_unitprice));
		plu_get_field(plu_addr-1, PTYPE_PIECE, (INT8U *)&plu_pcs,       &size,sizeof(plu_pcs));
	}

	Menu_Init();
#if !defined(USE_SINGLE_LINE_GRAPHIC)
	MenuNumLinePerPage = 2;
#endif
	caption_split_by_code(0x1110, &cap,0);
	Menu_HeadCreate((HUGEDATA char *)cap.form);

	if (!mode)
	{
		if (Startup.disable_dept)
		{
			Menu_CallbackDeptNo = 1;
		}
		else
		{
			plu_get_display_info(PTYPE_DEPARTMENT_NO, &cap, 0);
			Menu_InputCreate(1, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
							cap.input_max, cap.input_min, 0, &dept_no, NULL, NULL, plucall_department);

			Menu_AddEndFunction(1, callback_end_chageunit_dept);
		}
		plu_get_display_info(PTYPE_PLU_NO, &cap, 0);
		Menu_InputCreate(2, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
						cap.input_max, cap.input_min, 0, &plu_no, NULL, NULL, plucall_plu_number);

		Menu_AddEndFunction(2, callback_end_chageunit_plu);
	}
	plu_get_display_info(PTYPE_PRICE, &cap, 0);
#if !defined(_USE_LCD20848_) && !defined(_USE_LCD32_)
	cap.input_dlength  = display_parameter.mode_len[DISPLAY_UNITPRICE];
	cap.input_max = power10(cap.input_dlength)-1;
	decpoint = display_parameter.mode_dec[DISPLAY_UNITPRICE];
#else
	decpoint = get_global_bparam(GLOBAL_AREA_PRICE_DECIMALPOINT);
#endif
	if (decpoint) 
	{
		cap.input_dlength++;
		cap.input_length = cap.input_dlength;
	}
#ifdef USE_PRICE_UNIT_8DIGIT
	cap.input_max = 99999999L;
#elif defined(USE_PRICE_UNIT_9DIGIT)
	cap.input_max = 999999999L;
#endif
	Menu_InputCreate(6, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
					cap.input_max, cap.input_min, decpoint, &plu_unitprice, NULL, NULL, NULL);
	Menu_SetSymbol(6, 11);

	plu_get_display_info(PTYPE_PIECE, &cap, 0);
	Menu_InputCreate(14, (HUGEDATA char *)cap.form, MENU_TYPE_NUM_2BYTE, cap.input_x, cap.input_dlength, cap.input_length,
					cap.input_max, cap.input_min, 0, (HUGEDATA INT32U*)&plu_pcs, NULL, NULL, NULL);
	Menu_SetSymbol(14, 21);

#ifdef USE_FIXED_PLUTYPE//USE_EMART_NETWORK
	if (plu_ptype == PLU_BY_PCS || plu_ptype == PLU_BY_PCS_FIX)
#else
	if (plu_ptype == PLU_BY_PCS)
#endif
	{
		Menu_SetInhibit(6, OFF);
		Menu_SetInhibit(14, OFF);
	}
	else
	{
		Menu_SetInhibit(6, OFF);
		Menu_SetInhibit(14, ON);
	}

#ifdef USE_EMART_CHANGE_DATA_AT_NON_SALE
	if (mode && UseEmartNetwork)
	{
		caption_split_by_code(0x1BD2, &cap,0);
		caption_adjust(&cap,NULL);
		Menu_InputCreate(15, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
					cap.input_max, cap.input_min, 0, &Reason_code, NULL, NULL, NULL);
	}
#endif

	result = Menu_InputDisplay();
	if(result == MENU_RET_SAVE)
	{
		//Modified by JJANG 20090113
		//기존 버그, menu 1110에서 단가를 변경할 경우 무조건 1번 PLU만 변경되는 현상 수정
		plu_table_search((INT8U)dept_no, plu_no,&plu_addr,0);
#ifdef USE_EMART_CHANGE_DATA_AT_NON_SALE
		if (UseEmartNetwork)
		{
			plu_get_field(plu_addr-1, PTYPE_PRICE, (INT8U *)&OriginUnitPrice, &size,sizeof(OriginUnitPrice));
			plu_get_field(plu_addr-1, PTYPE_PLU_TYPE, (INT8U *)&plu_ptype,     &size,sizeof(plu_ptype));

			ResultUnitPrice = plu_unitprice;
			if (!mode)
			{
				status_scale.departmentid = dept_no;
				status_scale.cur_pluid = plu_no;
				Reason_code = 0;
			}

			changeFlag = GetChangeFlag(plu_ptype, plu_ptype, ResultUnitPrice, OriginUnitPrice, 0, 0, 0);
			if (changeFlag)
			{
				ret_value = GetCheckAddTransactionData(0,1,AUDIT_MODE_CIRCULAR);		//mode : sale, count : 1, Linear
				if (ret_value)
				{
//					Reason_code = 0;	//Fix
					InsertAdditionalData(&trans_add);
					InsertUnitPriceChangeRecord(status_scale.scaleid,status_scale.departmentid,status_scale.cur_pluid,
							OriginUnitPrice,ResultUnitPrice,Reason_code,status_scale.clerkid,changeFlag,&chgprice);
					transaction_sale_store(NULL, NULL, &trans_add, &chgprice, 0, 1);
				}
				else return MENU_RET_ESC;
			}
		}
#endif
		plu_set_field(plu_addr-1, PTYPE_PRICE, (INT8U *)&plu_unitprice);
		plu_set_field(plu_addr-1, PTYPE_PIECE, (INT8U *)&plu_pcs);
		//plu_set_field(plu_addr-1, PTYPE_PLU_TYPE, (INT8U *)&plu_ptype);
		if (network_status.service_flag&0x20)	// send plu
		{
			commun_net_mask(0, &plu_addr, 0xff, 1);
		}
	}

	return 0;
}


INT16S	read_enable_list(INT16S *enable_list)
{
	INT16S i, j=0;
	INT16S itemp;

	INT32U	addr;

	addr  = DFLASH_BASE;
	addr += FLASH_PLU_STRUCT_AREA;
	addr += PLU_STRUCT_POINT;

	for(i=0; i<plu_n_field; i++) {
		itemp = Flash_wread(addr+PLUSTR_PTYPE_POINT);
		if( (itemp&0x8000) == 0) {
			if (get_global_bparam(GLOBAL_SALE_SETUP_FLAG3)&0x20) {
				if (itemp==26) goto NEXT; // Fixed Price
			}
#ifdef USE_DIRECT_INGREDIENT
			if (MAX_INGREDIENT_TEXT_NUMBER == 0)
			{
				if (itemp == 99) goto NEXT;	// direct ingredient
			}
#endif
			if (!((itemp==1) || (itemp==2)))
			 	enable_list[j++] = (itemp&0x7fff);
		}
	NEXT:	addr += PLU_STRUCT_SIZE;
	}

	return j;
}

INT8U plu_set_speedkey(INT16U setNo, INT16U keyNo, INT32U pluNo)
{
	INT32U  addr_key;

	if(setNo == 0 || setNo > 5)
	{
		return FALSE;
	}
	if(keyNo == 0 || keyNo > MAX_SPEEDKEY_QTY)
	{
		return FALSE;
	}
	if(pluNo == 0 || pluNo > MAX_PLU_NUMBER)
	{
		return FALSE;
	}
	addr_key = DFLASH_BASE + FLASH_KEY_SPEED_AREA;
	addr_key+= (INT32U)(setNo - 1) * MAX_SPEEDKEY_SIZE;
	addr_key+= (INT32U)(keyNo - 1) * 4;
	Flash_lwrite(addr_key, pluNo);

	return TRUE;
}

INT32U plu_get_speedkey(INT16U setNo, INT16U keyNo)
{
	INT32U  addr_key;
	INT32U  pluNo;

	if(setNo == 0 || setNo > 5)
	{
		return 0;
	}
	if(keyNo == 0 || keyNo > MAX_SPEEDKEY_QTY)
	{
		return 0;
	}
	addr_key = DFLASH_BASE + FLASH_KEY_SPEED_AREA;
	addr_key+= (INT32U)(setNo - 1) * MAX_SPEEDKEY_SIZE;
	addr_key+= (INT32U)(keyNo - 1) * 4;
	pluNo = Flash_lread(addr_key);

	return pluNo;
}
extern INT32U PrevSpeedKeyPluNo;
extern INT16U PrevKeySetNo;
extern INT16U PrevKeyNo;
#define M_ID_SPDKEY_SET_NO         1
#define M_ID_SPDKEY_KEY_NO         2
#define M_ID_SPDKEY_PLU_NO         3

void callback_speedKeySetNo(long num)
{
	if (num < 1 || num > 5) num = 1;
	speedkey_set_number = num;
}


void callback_speedKeyNo(long plunum)
{
	INT8U buffer[34];
	INT8U id_plu_no;

	if (KeyDrv.Type == KEY_TYPE_SPEEDPLU)
	{
		if (PrevSpeedKeyPluNo) { // 직전 스피드키 저장(스피드키+숫자키 일때만)
			plu_set_speedkey(PrevKeySetNo, PrevKeyNo, PrevSpeedKeyPluNo);
			PrevSpeedKeyPluNo = 0;
		}
		PrevKeyNo = (INT16U)speedkey_key_number;
		PrevKeySetNo = (INT16U)speedkey_set_number;
		caption_get_name(0x3162, (char*)buffer);
		buffer[3] = 0;
		sprintf((char*)&buffer[3],"%03d", speedkey_key_number);
		display_string(DISPLAY_UNITPRICE, buffer);
		VFD7_Diffuse();

		id_plu_no = Menu_GetMenuId(M_ID_SPDKEY_PLU_NO);
		if (id_plu_no != 0xff) {
			*Menu[id_plu_no].pval = plunum;
		}
	}
	plucall_plu_number(plunum);
}

void	plu_speed_key_define(void)
{
	CAPTION_STRUCT 		cap;
	POINT	disp_p;
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
	CAPTION_STRUCT 	   cap1;
	char 	input_line;
	INT16S 	i, result,start_line;
	long  	ret_long;
	long	input_value[3];
	INT16U  cap_no[2]={ 0x3162,2 };	// 2번째 값은 PLU Structure 에서 얻는다.
	INT8U 	backup_directdraw;
	INT8U   pluno_cap_len;
	INT8U	string_buf[50];

	PrevSpeedKeyPluNo = 0;
	PrevKeySetNo = 0;
	PrevKeyNo = 0;

	caption_split_by_code(2, &cap1,0);
	caption_adjust(&cap1,NULL);
	input_value[1] = 1;
	input_value[0] = (long)status_scale.cur_speedkeyid;
	start_line = Startup.menu_type;
PAGE1_VIEW:
	if (Startup.disable_dept) {
		input_value[0]=1;
		speedkey_set_number=input_value[0];
		goto PAGE1SKIP;
	}
	backup_directdraw = DspStruct.DirectDraw;
	DspStruct.DirectDraw = 0;

	Dsp_Fill_Buffer(0);
	display_title_page_change(0x1160, 1, 2);
	caption_split_by_code(0x3161, &cap,0);
	caption_adjust(&cap,NULL);
	disp_p = point((start_line+1)*Startup.menu_ygap,0); 
#ifdef HEBREW_FONT
	Convert_HebrewPreview((INT8U *)cap.form, strlen(cap.form), 0, 0); 
	hebrew_codetemp[strlen(cap.form)] = 0;
	disp_p.x = 202 - disp_p.x;
	Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
	Convert_ArabPreview((INT8U *)cap.form, strlen((char *)cap.form), 0, 0, 0); 
	arab_codetemp[strlen((char *)cap.form)] = 0;
	disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
	Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
	Dsp_Write_String(disp_p,(HUGEDATA char*)cap.form);
#endif
	Dsp_Diffuse();

PAGE1_KI_IN:
	DspStruct.DirectDraw = 1;
	ret_long = input_value[0];
	if(ret_long<1 || ret_long>5)
		ret_long = 1;

	result = keyin_ulong(KI_GRAPHIC, KI_FORM_NORMAL, 0,(long*)&ret_long, cap.input_length, 
    				(start_line+1)*Startup.menu_ygap,  cap.input_x*Startup.menu_xgap, 0,KI_NOPASS );
	if(result == KP_ESC) goto RET_ESC;
	if(ret_long < cap.input_min || ret_long>cap.input_max) {
		BuzOn(2);
		error_display_form1(0, cap.input_min, cap.input_max);
		goto PAGE1_KI_IN;
	} else {
		input_value[0] = ret_long;
		speedkey_set_number = input_value[0];
	}
	switch(result) {
		case KP_ARROW_DN:
		case KP_ENTER:
		case KP_PAGE_DN: break;
		default        : BuzOn(2);
				 goto PAGE1_KI_IN;
	}
PAGE1SKIP:
	input_line = 1;
PAGE2_VIEW:
	DspStruct.DirectDraw = 0;
	Dsp_Fill_Buffer(0);
	display_title_page_change(0x1160, 2, 2);
	for(i=0; i<2; i++)  {
		caption_split_by_code(cap_no[i],&cap,0);
		caption_adjust(&cap,NULL);
		disp_p = point( (i+start_line)*Startup.menu_ygap,0);
#ifdef USE_ARAB_CONVERT
		Convert_ArabPreview((INT8U *)cap.form, strlen((char *)cap.form), 0, 0, 0); 
		arab_codetemp[strlen((char *)cap.form)] = 0;
		disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
		Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
		Dsp_Write_String(disp_p,(HUGEDATA char*)cap.form);
#endif
		if (i == 1) {
			input_value[2] = plu_get_speedkey((INT16U)input_value[0], (INT16U)input_value[1]);
#ifdef USE_THAI_FONT
			pluno_cap_len = Cvt_ThaiStrLength((INT8U*)cap.form);
#else
			pluno_cap_len = strlen((char *)cap.form);
#endif
		}
		ret_long = input_value[i+1];
		if( !(input_range_check(ret_long, cap.input_min, cap.input_max)) ) {
			input_value[1] = 1;
//			input_value[2] = 1;
			ret_long = 1;			
		} 
		if (i == 1) {
			callback_dept_no=status_scale.departmentid;
		 	//plucall_plu_position_x = strlen((char*)cap.form)+1; 
		 	//plucall_plu_position_y = start_line+i;
			CallbackEditPosition_x = (pluno_cap_len + 1);
			CallbackEditPosition_y = start_line + i;
		 	keyin_setcallback(plucall_plu_number);
		} else {
		 	keycall_key_position_y = start_line+i;
		}
		result = keyin_ulong(KI_GRAPHIC, KI_FORM_NORMAL, 0,(long*)&ret_long, cap.input_length, 
	     	     (i+start_line)*Startup.menu_ygap,  cap.input_x*Startup.menu_xgap, 0,KI_PASS );
		keyin_clearcallback();
	}
	Dsp_Diffuse();
	DspStruct.DirectDraw = 1;
	for(i=input_line; i<2; i++)  {
		caption_split_by_code(cap_no[i], &cap,0);
		caption_adjust(&cap,NULL);
PAGE2_KI_IN:	ret_long = input_value[i+1];
		if (!(input_range_check(ret_long, cap.input_min, cap.input_max))) {
			ret_long = 1;
		}
		switch(i) {
			case 0:	keyin_get_speed_key=1;
				KEY_SetMode(2);
				speedkey_set_number = input_value[0]; 
				CallbackEditPosition_x = pluno_cap_len + 1;
				CallbackEditPosition_y = Startup.menu_type + 1;	// speed key 다음줄. PLU no 위치
				keyin_setcallback(keycall_key_number);
				break;
			case 1:	keyin_get_speed_key=2;
				KEY_SetMode(2);
				skeyin_define=input_value[1]; 
				CallbackEditPosition_x = pluno_cap_len + 1;
				CallbackEditPosition_y = Startup.menu_type + 1;
			 	keyin_setcallback(plucall_plu_number);
				break;
		}
#ifdef USE_ARAB_CONVERT
        result = keyin_ulong(KI_GRAPHIC,
                     KI_FORM_ILNN, 
                     0,
                     (long*)&ret_long, 
                     cap.input_length, 
                         (i+start_line)*Startup.menu_ygap,  
                         cap.input_x*Startup.menu_xgap, 
                         0,
                         KI_NOPASS );
#else
		result = keyin_ulong(KI_GRAPHIC,
				     KI_FORM_NORMAL, 
				     0,
				     (long*)&ret_long,
				     cap.input_length,
			     	     (i+start_line)*Startup.menu_ygap,  
			     	     cap.input_x*Startup.menu_xgap, 
			     	     0,
			     	     KI_NOPASS);
#endif
		keyin_get_speed_key=0;
		KEY_SetMode(1);
		keyin_clearcallback();
		if (i == 0) {
			if (speedkey_plu_number > 0)
				input_value[2] = speedkey_plu_number;
		}
		if (i == 1) {
			input_value[1] = skeyin_define;
		}
		if(result == KP_ESC) goto RET_ESC;
//		if( input_range_check(ret_long, cap.input_min, cap.input_max) ) {
		if( input_range_check(ret_long, 0, cap.input_max) ) { 		 //SG060510
			input_value[i+1] = ret_long;
		} else {
			error_display_form1(0, cap.input_min, cap.input_max);
			goto PAGE2_KI_IN;
		}
										   
		switch(result) {
			case    KP_ESC:
				goto RET_ESC;
			case 	KP_ARROW_RI:
				if (input_value[i+1] < cap.input_max)
					input_value[i+1]++;
				goto PAGE2_KI_IN;
			case 	KP_ARROW_LF:
				if (input_value[i+1] > cap.input_min)
					input_value[i+1]--;
				goto PAGE2_KI_IN;
			case	KP_ARROW_DN:
			case 	KP_ENTER:
					if(i==1) goto RET_END;
					break;

			case	KP_ARROW_UP:	
					switch(i) {
						case 0:
							goto PAGE1_VIEW;
						case 1:
							input_line = 0;
							goto PAGE2_VIEW;
						}
			case 	KP_SAVE:	
					goto RET_END;

			default:		
					BuzOn(2);						
					goto PAGE2_KI_IN;
		} // switch(result);
	} // end page2_input
RET_END:
	if(	plu_set_speedkey((INT16U)input_value[0], (INT16U)input_value[1], input_value[2]))
	{
		Dsp_Fill_Buffer(0);
		sprintf(string_buf,"key saved");
		disp_p = point(Startup.menu_ygap,0);
		Dsp_Write_String(disp_p, string_buf);
		Dsp_Diffuse();
		Delay_100ms(3);
	}
	else 
	{
		Dsp_Fill_Buffer(0);
		sprintf(string_buf,"key setting error");
		disp_p = point(Startup.menu_ygap,0);
		Dsp_Write_String(disp_p, string_buf);
		Dsp_Diffuse();
		Delay_100ms(3);
	}
	input_line = 1;
	goto PAGE2_VIEW;
RET_ESC:
	DspStruct.DirectDraw = backup_directdraw;
#else
	INT16U 	result;

	INT32U speedKeySet;
	INT32U speedKeyNo;
	INT32U pluNo;
	INT8U	string_buf[50];


	speedKeyNo = 1;
	speedKeySet = (INT32U)status_scale.cur_speedkeyid;

	if (Startup.disable_dept)
	{
		speedKeySet = 1;
	}
	if (speedKeySet < 1 || speedKeySet > 5)
	{
		speedKeySet = 1;
	}

	speedkey_set_number=speedKeySet;
	speedkey_key_number=speedKeyNo;

	pluNo = plu_get_speedkey((INT16U)speedKeySet, (INT16U)speedKeyNo);
	callback_dept_no=status_scale.departmentid;

	Menu_Init();

	caption_split_by_code(0x1160, &cap,0);
	Menu_HeadCreate(( char *)cap.form);

	if (!Startup.disable_dept)
	{
		caption_split_by_code(0x3161, &cap, 0); // "Speed Key Set No"
		caption_adjust(&cap, NULL);
		Menu_InputCreate(M_ID_SPDKEY_SET_NO, ( char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
						cap.input_max, cap.input_min, 0, &speedKeySet, NULL, NULL, NULL);
		Menu_AddEndFunction(M_ID_SPDKEY_SET_NO, callback_speedKeySetNo);
	}

	plu_get_display_info(PTYPE_PLU_NO, &cap, 1); // "PLU NO"
	caption_get_name(0x3162, cap.form);
	cap.form[3] = 0;
	sprintf(&cap.form[3],"%03d", speedkey_key_number);
	cap.input_min = 0; // 0일때, 취소,이동, 실행이 가능하도록 함
	caption_adjust(&cap, NULL);
	Menu_InputCreate(M_ID_SPDKEY_PLU_NO, ( char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
					cap.input_max, cap.input_min, 0, &pluNo, NULL, NULL, callback_speedKeyNo);
	Menu_SetKeyMode(M_ID_SPDKEY_PLU_NO, 2);

	result = Menu_InputDisplay();
	if(result == MENU_RET_SAVE)
	{
		if( plu_set_speedkey((INT16U)speedKeySet, (INT16U)speedkey_key_number, pluNo)) //callback_speedKeyNo()에서 speedkey가 설정되며, 최종 speedkey는 이곳에서 설정됨
		{
			Dsp_Fill_Buffer(0);
			sprintf(string_buf,"key saved");
			disp_p = point(0,0);
			Dsp_Write_String(disp_p, string_buf);
			Dsp_Diffuse();
			Delay_100ms(10);
		}
		else
		{
			Dsp_Fill_Buffer(0);
			sprintf(string_buf,"key setting error");
			disp_p = point(0,0);
			Dsp_Write_String(disp_p, string_buf);
			Dsp_Diffuse();
			Delay_100ms(10);
		}
	}
	speedkey_set_number = 0;
#endif
	return;
}

void plu_edit_price(INT8U deptno,INT32U pluno)
{
	INT16U 	ret_pass;
	INT8U key_mode;
//	INT32U key;
	char old_font;

	if (deptno==0) return;
	if (pluno==0) return;
//	if (pluno>=100000) return;
	if (pluno>MAX_PLU_NUMBER) return;

	key_mode=KEY_GetMode();
	KEY_SetMode(1);

	ret_pass = permission_check(PMS_PLU_PRICE_CHANGE,0);
	old_font=DspStruct.Id1;
	if(!ret_pass) {
		goto RET_ESC;
	}
	DspLoadFont1(DSP_MENU_FONT_ID);

	plu_edit_price_sub(deptno,pluno,1);
RET_ESC:
	sale_display_update(0x0fff);
//RET_END:
	DspLoadFont1(old_font);
	KEY_SetMode(key_mode);
	return;
}


void plu_edit_item(INT8U deptno,INT32U pluno)
{
	INT16U 	ret_pass;
	INT8U key_mode;
	INT16U key;
	char old_font;
	INT8U 	new_wunit_id, wunit_id, backup_weightunit;
	INT16S 	usize;
	INT16S enable_list[128];
	INT16S maxid;

	if (deptno==0) return;
	if (pluno==0) return;
//	if (pluno>=100000) return;
	if (pluno>MAX_PLU_NUMBER) return;

	key_mode=KEY_GetMode();
	KEY_SetMode(1);

	maxid = read_enable_list(enable_list);
//	ret_pass = check_clerk_permission(status_scale.clerkid,  PLU_CHANGE_H_KEY);
	ret_pass = permission_check(PMS_PLU_ITEM_CHANGE,0);
	old_font=DspStruct.Id1;
	if(!ret_pass) {
		goto RET_ESC;
	}
	DspLoadFont1(DSP_MENU_FONT_ID);
	plu_table_search(deptno,pluno,&key,0);
	if (key==0) {
		goto RET_END;
	} else {
		plu_move_temp(0,(long)(key-1));
	}
	if (pluno > 0) {
		backup_weightunit = CurPluWeightUnit;
		plu_get_field(key-1, PTYPE_UNIT_WEIGHT,(INT8U *)&wunit_id, &usize, sizeof(wunit_id));	// UnitWeight
		get_weightunit_and_id_fromid(wunit_id, &new_wunit_id, &CurPluWeightUnit);
		plu_input((INT32U)key,(INT16U)deptno,pluno,maxid,enable_list,0,0);
		CurPluWeightUnit = backup_weightunit;
		goto RET_ESC;
	}

RET_ESC:
	//sale_display_update(0x0fff);
RET_END:
	Dsp_Fill_Buffer(0);						// LCD Buffer clear
	DspLoadFont1(old_font);
	Dsp_Diffuse();
	KEY_SetMode(key_mode);
	sale_display_update(0x0fff);
	return;
}

void plu_edit_origin(INT8U deptno,INT32U pluno)
{
	CAPTION_STRUCT 		cap;
	INT8U 	key_mode;
	INT16S 	size;
	INT16U	result, origin;
	INT16U	plu_save_no;
	char    old_font;
	INT32U  set_val;

	if (deptno==0) return;
	if (pluno==0) return;
	if (pluno>MAX_PLU_NUMBER) return;
	if ((status_scale.restrict&FUNC_ORG_TBL)==0) {
		BuzOn(2);
		return;
	}
	plu_table_search(deptno, pluno,&plu_save_no,0);
	if (plu_save_no==0) {
		return;
	}
	plu_get_field(plu_save_no-1, PTYPE_ORIGIN_NO, (INT8U*)&origin, &size,sizeof(origin));
	set_val = origin;

	key_mode=KEY_GetMode();
	KEY_SetMode(1);

	old_font=DspStruct.Id1;
	DspLoadFont1(DSP_MENU_FONT_ID);

	Menu_Init();

	caption_split_by_code(0x1BB0, &cap,0);
	Menu_HeadCreate((HUGEDATA char *)cap.form);

	plu_get_display_info(PTYPE_ORIGIN_NO, &cap, 0);
	cap.input_min = 0;
	Menu_InputCreate(1, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
					cap.input_max, cap.input_min, 0, &set_val, NULL, NULL, plucall_origin);


	result = Menu_InputDisplay();
	if(result == MENU_RET_SAVE)
	{
		origin = set_val;
		plu_set_field(plu_save_no-1, PTYPE_ORIGIN_NO, (INT8U*)&origin);
		if (network_status.service_flag&0x20)	// send plu
		{
			commun_net_mask(0, &plu_save_no, 0xff, 1);
		}
	}
	DspLoadFont1(old_font);
	KEY_SetMode(key_mode);
	sale_display_update(0x0fff);
}

//Added by JJANG 20081818 ingredient edit key
void plu_edit_ingredient(INT8U deptno,INT32U pluno)
{
	CAPTION_STRUCT 		cap;
	INT8U 	key_mode;
	INT16S 	size;
	INT16U	result, ingredient_data;
	INT16U	plu_save_no;
	char    old_font;
	INT32U  set_val;

	if (deptno==0) return;
	if (pluno==0) return;
	if (pluno>MAX_PLU_NUMBER) return;

	plu_table_search(deptno, pluno,&plu_save_no,0);
	if (plu_save_no==0) {
		return;
	}

	plu_get_field(plu_save_no-1, PTYPE_INGREDIENT_NO, (INT8U*)&ingredient_data, &size,sizeof(ingredient_data));
	set_val = ingredient_data;

	key_mode=KEY_GetMode();
	KEY_SetMode(1);

	old_font=DspStruct.Id1;
	DspLoadFont1(DSP_MENU_FONT_ID);

	Menu_Init();

	caption_split_by_code(0x1BD1, &cap,0);
	Menu_HeadCreate((HUGEDATA char *)cap.form);

	plu_get_display_info(PTYPE_INGREDIENT_NO, &cap, 0);
	cap.input_min = 0;
	Menu_InputCreate(1, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
					cap.input_max, cap.input_min, 0, &set_val, NULL, NULL, plucall_ingredient);

	result = Menu_InputDisplay();
	if(result == MENU_RET_SAVE)
	{
		ingredient_data = set_val;
		plu_set_field(plu_save_no-1, PTYPE_INGREDIENT_NO, (INT8U*)&ingredient_data);
		if (network_status.service_flag&0x20)	// send plu
		{
			commun_net_mask(0, &plu_save_no, 0xff, 1);
		}
	}
	DspLoadFont1(old_font);
	KEY_SetMode(key_mode);
	sale_display_update(0x0fff);
}

//Added by JJANG 20090405 유효일, 가공일 인쇄 여부 세팅
void plu_edit_date_flag(INT8U deptno,INT32U pluno)
{
	CAPTION_STRUCT	cap;
	INT8U 	packdate_flag;
	INT8U 	packtime_flag;
	INT8U	selldate_flag;
	INT8U	selltime_flag;
	INT16U	plu_save_no;
	INT16U	result;
	char    old_font;
	INT8U 	key_mode;
	INT16S 	size;
	INT32U  set_val[4];


	if (deptno==0) return;
	if (pluno==0) return;
	if (pluno>MAX_PLU_NUMBER) return;

	plu_table_search(deptno, pluno,&plu_save_no,0);
	if (plu_save_no==0) {
		return;
	}

	plu_get_field(plu_save_no-1, PTYPE_PACKCED_DATE_FLAG, (INT8U*)&packdate_flag, &size,sizeof(packdate_flag));
	set_val[0] = packdate_flag;
	plu_get_field(plu_save_no-1, PTYPE_PACKCED_TIME_FLAG, (INT8U*)&packtime_flag, &size,sizeof(packtime_flag));
	set_val[1] = packtime_flag;
	plu_get_field(plu_save_no-1, PTYPE_SELLBY_DATE_FLAG, (INT8U*)&selldate_flag, &size,sizeof(selldate_flag));
	set_val[2] = selldate_flag;
	plu_get_field(plu_save_no-1, PTYPE_SELLBY_TIME_FLAG, (INT8U*)&selltime_flag, &size,sizeof(selltime_flag));
	set_val[3] = selltime_flag;

	key_mode=KEY_GetMode();
	KEY_SetMode(1);

	old_font=DspStruct.Id1;
	DspLoadFont1(DSP_MENU_FONT_ID);

	Menu_Init();

	plu_get_display_info(PTYPE_PACKCED_DATE_FLAG, &cap, 0);
	cap.input_min = 0;
	Menu_InputCreate(1, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
					cap.input_max, cap.input_min, 0, &set_val[0], NULL, NULL, NULL);
	plu_get_display_info(PTYPE_PACKCED_TIME_FLAG, &cap, 0);
	cap.input_min = 0;
	Menu_InputCreate(2, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
					cap.input_max, cap.input_min, 0, &set_val[1], NULL, NULL, NULL);
	plu_get_display_info(PTYPE_SELLBY_DATE_FLAG, &cap, 0);
	cap.input_min = 0;
	Menu_InputCreate(3, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
					cap.input_max, cap.input_min, 0, &set_val[2], NULL, NULL, NULL);
	plu_get_display_info(PTYPE_SELLBY_TIME_FLAG, &cap, 0);
	cap.input_min = 0;
	Menu_InputCreate(4, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
					cap.input_max, cap.input_min, 0, &set_val[3], NULL, NULL, NULL);

	result = Menu_InputDisplay();
	if(result == MENU_RET_SAVE)
	{
		packdate_flag = set_val[0];
		plu_set_field(plu_save_no-1, PTYPE_PACKCED_DATE_FLAG, (INT8U*)&packdate_flag);
		packtime_flag = set_val[1];
		plu_set_field(plu_save_no-1, PTYPE_PACKCED_TIME_FLAG, (INT8U*)&packtime_flag);
		selldate_flag = set_val[2];
		plu_set_field(plu_save_no-1, PTYPE_SELLBY_DATE_FLAG, (INT8U*)&selldate_flag);
		selltime_flag = set_val[3];
		plu_set_field(plu_save_no-1, PTYPE_SELLBY_TIME_FLAG, (INT8U*)&selltime_flag);
		if (network_status.service_flag&0x20)	// send plu
		{
			commun_net_mask(0, &plu_save_no, 0xff, 1);
		}
	}
	DspLoadFont1(old_font);
	KEY_SetMode(key_mode);
	sale_display_update(0x0fff);
}

void plu_edit_delete(INT8U xdeptno,INT32U xpluno)
{
//	INT16U 	deptno ;
//	INT32U 	pluno;
//	char  old_font;
//	INT8U  ret_pass;
//	INT16S    ret;

//	deptno = (INT16U)xdeptno;
//	pluno  = (INT32U)xpluno;
/*
	ret_pass = permission_check(PMS_PLU_ITEM_CHANGE);
	old_font=DspStruct.Id1;
	if(!ret_pass) {
		goto RET_ESC;
	}
	DspLoadFont1(DSP_MENU_FONT_ID);
	Dsp_Fill_Buffer(0);
	Dsp_Diffuse();

PAGE_KI_IN:
	display_title_page_change(0x1147,1,1);
	// <CODE> JHC, 040702, Modify, - KP_ARROW_DN : 1 = NoReturn, 0 = Return
	// [
	g_i8uInputDeptReturn = 1;
	ret = plu_input_department(&deptno, &pluno);
	g_i8uInputDeptReturn = 0;
	// ]
	if(ret==2) {
		goto RET_ESC;
	}
	else if (ret == KP_ARROW_DN ||  ret == KP_PAGE_DN)
		goto PAGE_KI_IN;

	if (deptno<1) {
		goto RET_ESC;
	}
	plu_delete(deptno, pluno,0);
	if (network_status.service_flag&0x0004) {
		if ((ethernet_list.status==1) ||
		    ((ethernet_list.status==2) && (network_status.service_flag&0x10)))
		command_set(0x11,0,deptno,pluno,0,0,NULL);
	}
RET_ESC:
	Dsp_Fill_Buffer(0);   // LCD Buffer clear
	Dsp_Diffuse();
	DspLoadFont1(old_font);
*/
}

extern INT8U plu_edit_speedkey_sub(INT16U set,INT32U pluno);

void plu_edit_speedkey(INT32U pluno)
{
	INT16U set;
	char old_font;
	INT8U 	key_mode;

	key_mode=KEY_GetMode();
	KEY_SetMode(1);
	old_font=DspStruct.Id1;
	DspLoadFont1(DSP_MENU_FONT_ID);
	set=status_scale.cur_speedkeyid;
	plu_edit_speedkey_sub(set,pluno);
	DspLoadFont1(old_font);
	KEY_SetMode(key_mode);
	sale_display_update(0x0fff);
}

INT8U plu_edit_speedkey_sub(INT16U set,INT32U pluno)
{
	CAPTION_STRUCT 	   cap;
	POINT 		   disp_p;
	INT16S 	i, result;
	INT16S     nspeedkey;
	long  	ret_long;
	char    buffer[32];
	INT32U  cur_plu,chk_plu,saved_plu;
	INT16U  skey;

	Dsp_Fill_Buffer(0);
	display_title_page_change(0x1160, 1, 1);

	cur_plu = pluno;
	if ((set<1) || (set>5)) set=1;
	speedkey_set_number=set;

	plu_get_display_info(PTYPE_PLU_NO,&cap,1);

	sprintf(buffer,"%s %6d",cap.form,cur_plu);
	disp_p = point(0,0);
	Dsp_Write_String(disp_p,(HUGEDATA char*)buffer);
	Dsp_Diffuse();

	nspeedkey = MAX_SPEEDKEY_SIZE/4;
	skey=0;
	if (set==0) set=1;
	if (set>5 ) set=1;
	for (i=0; i<nspeedkey; i++) {
		chk_plu = plu_get_speedkey(set, i+1);
		if (chk_plu==cur_plu) {
			skey=i+1;
			break;
		}
	}
PAGE_VIEW:
	caption_split_by_code(0x3162,&cap,0); // 0x116B+i, &cap);
	caption_adjust(&cap,NULL);
	disp_p = point(0,0);
	display_clear(DISPLAY_PRICE);
	display_string_pos(12, (INT8U*)cap.form);
	VFD7_Diffuse();
	ret_long = skey;
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
	result = keyin_ulong(KI_GRAPHIC, KI_FORM_NORMAL, 0,(long*)&ret_long, cap.input_length, 
						(1+Startup.menu_type)*Startup.menu_ygap,  cap.input_x*Startup.menu_xgap, 0,KI_PASS );
#else
	result = keyin_ulong(KI_TOTAL, KI_FORM_NORMAL, 0,(long*)&ret_long, cap.input_length,
			     	     0,  0, 0,KI_PASS );
#endif
	Dsp_Diffuse();
PAGE_KI_IN:
	ret_long = skey;
	if( !(input_range_check(ret_long, cap.input_min, cap.input_max)) ) {
		ret_long = 1;
	}
	callback_dept_no=status_scale.departmentid;
	keyin_get_speed_key=1;
	KEY_SetMode(2);
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
	//keyin_setcallback(keycall_direct_edit_number);
	result = keyin_ulong(KI_GRAPHIC,
			KI_FORM_NORMAL,
			0,
			(long*)&ret_long,
			cap.input_length,
				(1+Startup.menu_type)*Startup.menu_ygap,
				cap.input_x*Startup.menu_xgap,
				0,
				KI_NOPASS );
#else
	result = keyin_ulong(KI_TOTAL, 
			KI_FORM_NORMAL, 
			0,
			(long*)&ret_long,
			cap.input_length,
			0,
			0,
			0,
			KI_NOPASS );
#endif
	saved_plu=speedkey_plu_number;
	keyin_get_speed_key=0;
	KEY_SetMode(1);
	if(result == KP_ESC) goto RET_ESC;
	if( input_range_check(ret_long, cap.input_min, cap.input_max) ) {
			skey  = ret_long;
	} else {
		error_display_form1(0, cap.input_min, cap.input_max);

		plu_get_display_info(PTYPE_PLU_NO,&cap,1);
#if !defined(_USE_LCD20848_) && !defined(_USE_LCD32_)
		sprintf(buffer,"%s %6d",cap.form,cur_plu);
		disp_p = point(0,0);
		display_lcd_clear();
		Dsp_Write_String(disp_p,(HUGEDATA char*)buffer);
		Dsp_Diffuse();
#endif
		goto PAGE_KI_IN;
	}
	switch(result) {
		case 	KP_ARROW_RI:
			if (skey < cap.input_max)
				skey++;
			goto PAGE_KI_IN;
		case 	KP_ARROW_LF:
			if (skey > cap.input_min)
				skey--;
			goto PAGE_KI_IN;
		case	KP_ARROW_DN:
		case 	KP_ENTER:
		case 	KP_SAVE:
			goto RET_END;
		case	KP_ARROW_UP:
			//return 2;
		default:
			BuzOn(2);
			goto PAGE_KI_IN;
	}
RET_END:
	if (saved_plu) {
		BuzOn(2);
		goto PAGE_VIEW;
	}
	if (skey<1) return 1;
	if (cur_plu<1) return 1;
	plu_set_speedkey(set, skey, cur_plu);
	return 1;
RET_ESC:
	Dsp_Fill_Buffer(0);			// LCD Buffer clear
	Dsp_Diffuse();
	display_string_pos_clear(5,20);
 	return 1;
}

#ifdef USE_DONT_SAVE_VALIDDATE_SALEDATE
extern TEMP_SELLDATE_STRUCT temp_selldate;
#endif

void plu_edit_sellbydate(INT8U deptno,INT32U pluno)
{
	CAPTION_STRUCT 		cap;
	INT8U   key_mode;//, selldate_flag;
	INT16S 	size;
	INT16U	result;
#ifdef USE_GSMART_PLU
	INT16U	sell_by_time;
#else
	INT8U	sell_by_time;
#endif
	INT16U	plu_save_no;
	INT32U	sell_by_date;
	char    old_font;
	INT8U   use_selldate, use_selltime;
	if (deptno==0) return;
	if (pluno==0) return;
	if (pluno>MAX_PLU_NUMBER) return;

	if( !permission_check(PMS_KEY_DATE_CHANGE,0)) return;
	use_selldate = 1;
	use_selltime = 1;
	plu_get_ptype_point(PTYPE_SELLBY_DATE,&key_mode,&result);
	if (result == 0) use_selldate = 0;	// 없거나 inhibit일때
	plu_get_ptype_point(PTYPE_SELLBY_TIME,&key_mode,&result);
	if (result == 0) use_selltime = 0;	// 없거나 inhibit일때
	if ((use_selldate == 0) && (use_selltime == 0)) {
		BuzOn(2);
		return;
	}
//	selldate_flag = get_global_bparam(GLOBAL_SALEDATE_TYPE);

	plu_table_search(deptno, pluno,(INT16U *)&plu_save_no,0);
	if (plu_save_no==0) {
		return;
	}

	key_mode=KEY_GetMode();
	KEY_SetMode(1);

	plu_get_field(plu_save_no-1, PTYPE_SELLBY_DATE, (INT8U*)&sell_by_date, &size,sizeof(sell_by_date));
	plu_get_field(plu_save_no-1, PTYPE_SELLBY_TIME, (INT8U*)&sell_by_time, &size,sizeof(sell_by_time));

	old_font=DspStruct.Id1;
	DspLoadFont1(DSP_MENU_FONT_ID);


	Menu_Init();

	caption_split_by_code(0x1BA0, &cap,0);
	Menu_HeadCreate((HUGEDATA char *)cap.form);

	if (use_selldate)
	{
		//caption_split_by_code(1, &cap,0);
		//caption_adjust(&cap,NULL);
		plu_get_display_info(PTYPE_SELLBY_DATE, &cap, 0);
		cap.input_min = 0;
//		if(selldate_flag) {
//			cap.input_dlength=6;
//			cap.input_length =6;
//			cap.input_min    =0;
//			cap.input_max    =999999;
//		}
		Menu_InputCreate(1, (HUGEDATA char *)cap.form, MENU_TYPE_DATENUM, cap.input_x, cap.input_dlength, cap.input_length,
						cap.input_max, cap.input_min, 0, &sell_by_date, NULL, NULL, NULL);
	}

	if (use_selltime)
	{
		plu_get_display_info(PTYPE_SELLBY_TIME, &cap, 0);
		cap.input_min = 0;
#ifdef USE_GSMART_PLU
		Menu_InputCreate(2, (HUGEDATA char *)cap.form, MENU_TYPE_NUM_2BYTE, cap.input_x, cap.input_dlength, cap.input_length,
						cap.input_max, cap.input_min, 0, (HUGEDATA INT32U*)&sell_by_time, NULL, NULL, NULL);
#else
		Menu_InputCreate(2, (HUGEDATA char *)cap.form, MENU_TYPE_NUM_1BYTE, cap.input_x, cap.input_dlength, cap.input_length,
						cap.input_max, cap.input_min, 0, (HUGEDATA INT32U*)&sell_by_time, NULL, NULL, NULL);
#endif
	}

	result = Menu_InputDisplay();
	if(result == MENU_RET_SAVE)
	{
#ifdef USE_DONT_SAVE_VALIDDATE_SALEDATE  // 유효일, 유효기간 저장하지 않음
		temp_selldate.date = sell_by_date;
		temp_selldate.time = sell_by_time;
 		temp_selldate.onoff = ON;
		temp_selldate.pluno= pluno;
		temp_selldate.deptno=deptno;
#else
		plu_set_field(plu_save_no-1, PTYPE_SELLBY_DATE, (INT8U*)&sell_by_date);
		plu_set_field(plu_save_no-1, PTYPE_SELLBY_TIME, (INT8U*)&sell_by_time);
		if (network_status.service_flag&0x20)	// send plu
		{
			commun_net_mask(0, &plu_save_no, 0xff, 1);
		}
#endif
	}
	DspLoadFont1(old_font);
	KEY_SetMode(key_mode);
	sale_display_update(0x0fff);

}

#ifdef USE_TAX_RATE
//function key 91 EDIT TAX
void plu_edit_tax(PRICE_TYPE *pPrice,INT8U deptno,INT32U pluno)	//Added by JJANG 20070619
{
	CAPTION_STRUCT	cap;
	TAX_STRUCT	tax;
	INT8U 	key_mode;
	INT16S 	size;
	INT16U	result;
	INT8U   tax_code;
	INT16U	plu_save_no;
	char    old_font;
	INT32U  set_val;
 	INT32U	flash_addr, addr;
	INT8U 	tax_flag;

	if (deptno==0) return;
	if (pluno==0) return;
	if (pluno>MAX_PLU_NUMBER) return;
	if ((status_scale.restrict&FUNC_TAX_TBL)==0) {
		BuzOn(2);
		return;
	}

	tax_flag = get_global_bparam(GLOBAL_TAX_FLAG);			//menu 1541
	if(pluno<MISC_PLU_WEIGHT)	//PLU일때
	{
		if(!tax_flag) return;
		else if (tax_flag == 2)	tax_code  = get_global_bparam(GLOBAL_TAX_VALUE);	//Get GLOBAL TAX NUMBER
		else
		{
			plu_table_search(deptno, pluno,&plu_save_no,0);
			if (plu_save_no==0) return;
			plu_get_field(plu_save_no-1, PTYPE_TAX_NO, (INT8U*)&tax_code, &size,sizeof(tax_code));
		}
	}
	else				//MISC PLU 일 때
	{
		tax_code  = get_global_bparam(GLOBAL_MISC_TAX_VALUE);	//Get MISC TAX NUMBER
		if(tax_code==0) return;
	}
	set_val = tax_code;

	key_mode=KEY_GetMode();
	KEY_SetMode(1);

	old_font=DspStruct.Id1;
	DspLoadFont1(DSP_MENU_FONT_ID);


	Menu_Init();

	caption_split_by_code(0x1BC0, &cap,0);
	Menu_HeadCreate((HUGEDATA char *)cap.form);

	//caption_split_by_code(1, &cap,0);
	//caption_adjust(&cap,NULL);
	plu_get_display_info(PTYPE_TAX_NO, &cap, 0);
	cap.input_min = 0;
	Menu_InputCreate(1, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
					cap.input_max, cap.input_min, 0, &set_val, NULL, NULL, plucall_taxcode);


	result = Menu_InputDisplay();
	if(result == MENU_RET_SAVE)
	{
		tax_code = set_val;
		//plu_set_field(plu_save_no-1, PTYPE_TAX_NO, (INT8U*)&tax_code);	// 추후 Tax의 저장기능 확인 필요
		//commun_plumask(plu_save_no,0xff,3);
		addr = DFLASH_BASE + FLASH_TAX_AREA;
		flash_addr = addr + (sizeof(TAX_STRUCT) * (set_val-1));
		Flash_sread(flash_addr, (HUGEDATA INT8U*)&tax, sizeof(TAX_STRUCT));
		if(set_val == tax.tax_code)
		{
			pPrice->TaxNumber = tax.tax_code;
			pPrice->ExTax = tax.tax_type;
			pPrice->TaxRate = (long)tax.tax_rate;
		}
	}
	DspLoadFont1(old_font);
	KEY_SetMode(key_mode);
	sale_display_update(0x0fff);

}

void plu_apply_global_tax(PRICE_TYPE *pPrice,INT8U deptno,INT32U pluno)
{
	INT8U 	tax_flag;
	INT8U   tax_code;
	INT32U	flash_addr, addr;
	TAX_STRUCT	tax;
	CAPTION_STRUCT cap;

	if (deptno==0) return;
	if (pluno==0) return;
	if (pluno>MAX_PLU_NUMBER) return;
	if ((status_scale.restrict&FUNC_TAX_TBL)==0)
	{
		BuzOn(2);
		return;
	}
	tax_flag = get_global_bparam(GLOBAL_TAX_FLAG);			//menu 1541
	if(pluno<MISC_PLU_WEIGHT)	//PLU일때
	{
		if (tax_flag == 1)	//use plu tax only
		{
				tax_code  = get_global_bparam(GLOBAL_TAX_VALUE);		//Get GLOBAL TAX NUMBER
		}
		else	//use global or not use tax
		{
			return;
		}

	}
	else				//MISC PLU 일 때
	{
		tax_code  = get_global_bparam(GLOBAL_MISC_TAX_VALUE);	//Get MISC TAX NUMBER
		if(tax_code==0) return;
	}

	caption_split_by_code(0x1BD3,&cap,0);		//"Global Tax applied"
	DisplayMsg(cap.form);

	addr = DFLASH_BASE + FLASH_TAX_AREA;
	flash_addr = addr + (sizeof(TAX_STRUCT) * (tax_code-1));
	Flash_sread(flash_addr, (HUGEDATA INT8U*)&tax, sizeof(TAX_STRUCT));
	if(tax_code == tax.tax_code)
	{
		pPrice->TaxNumber = tax.tax_code;
		pPrice->ExTax = tax.tax_type;
		pPrice->TaxRate = (long)tax.tax_rate;
	}
}

#endif

#ifdef USE_TRACEABILITY
void plu_edit_traceability(INT8U deptno,INT32U pluno)
{
	CAPTION_STRUCT 		cap;
	INT8U 	key_mode;
	INT16S 	size;
	INT16U	result, trace;
	INT16U	plu_save_no;
	char    old_font;
	INT32U  set_val;

	if (deptno==0) return;
	if (pluno==0) return;
	if (pluno>MAX_PLU_NUMBER) return;
	if ((status_scale.restrict&FUNC_TRACE)==0) {
		BuzOn(2);
		return;
	}
	plu_table_search(deptno, pluno,&plu_save_no,0);
	if (plu_save_no==0) {
		return;
	}
	plu_get_field(plu_save_no-1, PTYPE_TRACE_NO, (INT8U*)&trace, &size,sizeof(trace));
	set_val = trace;

	key_mode=KEY_GetMode();
	KEY_SetMode(1);

	old_font=DspStruct.Id1;
	DspLoadFont1(DSP_MENU_FONT_ID);


	Menu_Init();

	caption_split_by_code(0x1BD0, &cap,0);
	Menu_HeadCreate((HUGEDATA char *)cap.form);

	//caption_split_by_code(1, &cap,0);
	//caption_adjust(&cap,NULL);
	plu_get_display_info(PTYPE_TRACE_NO, &cap, 0);
	cap.input_min = 0;
	Menu_InputCreate(1, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
					cap.input_max, cap.input_min, 0, &set_val, NULL, NULL, plucall_traceability);


	result = Menu_InputDisplay();
	if(result == MENU_RET_SAVE)
	{
		trace = set_val;
		plu_set_field(plu_save_no-1, PTYPE_TRACE_NO, (INT8U*)&trace);
		if (network_status.service_flag&0x20)	// send plu
		{
			commun_net_mask(0, &plu_save_no, 0xff, 1);
		}
//DEL_MASK		commun_plumask(plu_save_no,0xff,3);
	}
	DspLoadFont1(old_font);
	KEY_SetMode(key_mode);
	sale_display_update(0x0fff);

}
#endif


void save_override_unit_price(void)
// Override 시 Save
{

	INT8U fixedprice=0;
	INT16S  sz=0;
	INT16U pcs = 1;
	if( PluType() != status_scale.Plu.ptype ) return; //SG061016
	if(status_scale.divisor)
	{
		return;
	}
	if (status_scale.cur_pluid) {
		plu_get_field(status_scale.Plu.address-1, PTYPE_FIXED_PRICE,(INT8U *)&fixedprice,&sz,sizeof(status_scale.Plu.fixed_price));// fixed price
	}
	if( fixedprice != status_scale.Plu.fixed_price) return; //SG061016

	if(PluType()==PLU_BY_PCS)
		pcs = Price.UnitCount[PRICE_ORIGIN];
	save_changed_unitprice(status_scale.departmentid,
			       status_scale.cur_pluid,
			       pcs,
			       Price.UnitPrice[PRICE_ORIGIN]);
}


void save_plu_type_change(void)
{
	INT8U  plu_type;
	INT16U pcs = 1;
	INT16U dc_no;

	if (get_global_bparam(GLOBAL_SALE_SAVE_FLAG)&0x04) return;

	plu_type = PluType();
	dc_no    = _get_dc_number((INT16U)status_scale.departmentid,(INT32U)status_scale.cur_pluid);
	if (dc_no) {
		error_display_caption(0xF03E);
		return;
	}
	if(plu_type == PLU_BY_PCS)
		pcs = Price.UnitCount[PRICE_ORIGIN];
	save_plu_type_changed_data(plu_type,
				   status_scale.departmentid,
				   status_scale.cur_pluid,
				   pcs);
}


void save_changed_unitprice(INT16U deptno,INT32U pluno, INT16U pcs, long unitprice)
{
	INT8U	uctemp;
	INT16U	key;

	if(deptno<1 || deptno>99)	return;
//SG070315	if(pluno<1 || pluno>99999L)	return;
	if(pluno<1 || pluno>MAX_PLU_NUMBER)	return;

//	key = plu_table_search((INT8U)deptno, pluno);
	plu_table_search((INT8U)deptno, pluno,&key,0);
	if(key<1) {
		BuzOn(2);
		return;
	}
	uctemp = get_global_bparam(GLOBAL_SALE_SAVE_FLAG);
//	if( !(uctemp&0x01)) {
	if( !(uctemp&0x01) && !(KeyDrv.CnvCode==KS_SAVEPRICE )) {  //SG060227
								   //KS_가격 저장 키가 할당 후 수정해야...
		return;
	} else {
		plu_set_field(key-1, PTYPE_PIECE, (INT8U *)&pcs);
		plu_set_field(key-1, PTYPE_PRICE, (INT8U *)&unitprice);
		status_scale.Plu.unitprice = unitprice;
		if (network_status.service_flag&0x20)	// send plu
		{
			commun_net_mask(0, &key, 0xff, 1);
		}
//		if (network_status.service_flag&0x0004) {
//			if ((ethernet_list.status==1) ||
//			    ((ethernet_list.status==2) && (network_status.service_flag&0x10)))
//			       	command_set(0x23,0x00,(INT8U)deptno,(INT32U)pluno,pcs,unitprice,NULL);
//		}
	}
}

void save_plu_type_changed_data(INT8U plu_type, INT16U deptno,INT32U pluno,INT16U pcs)
{
	INT8U	uctemp,btareid;
	INT16U	key,wtarep;
	INT32U  ltare;

	if(deptno<1 || deptno>99)	return;
//SG070315	if(pluno<1 || pluno>99999)	return;
	if(pluno<1 || pluno>MAX_PLU_NUMBER) 	return;

//	key = plu_table_search(deptno, pluno);
	plu_table_search(deptno, pluno,&key,0);
	uctemp = get_global_bparam(GLOBAL_SALE_SAVE_FLAG);
	if(key==0)
		return;

	if( !(uctemp&0x02) ) {
		return;
	} else {
		btareid=0;
		wtarep =0;
		ltare  =0;
		plu_set_field(key-1, PTYPE_PLU_TYPE, (INT8U *)&plu_type);
		plu_set_field(key-1, PTYPE_PIECE, (INT8U *)&pcs);
		plu_set_field(key-1, PTYPE_TARE_NO, (INT8U *)&btareid);
		plu_set_field(key-1, PTYPE_TARE_VALUE, (INT8U *)&ltare);
		plu_set_field(key-1, PTYPE_PERCENT_TARE_LIMIT, (INT8U *)&wtarep);
		plu_set_field(key-1, PTYPE_PERCENT_TARE, (INT8U *)&wtarep);
		if (network_status.service_flag&0x20)	// send plu
		{
			commun_net_mask(0, &key, 0xff, 1);
		}
//		if (network_status.service_flag&0x0004) {
//			if ((ethernet_list.status==1) ||
//			    ((ethernet_list.status==2) && (network_status.service_flag&0x10)))
//			       	command_set(0x22,0x00,(INT8U)deptno,(INT32U)pluno,plu_type,(INT32U)pcs,NULL);
//		}
	}
}

void plu_mgr_trancnt(void)
{
	INT8U  	ret, result;
	INT16U 	deptno ;
	INT32U 	pluno;
	CAPTION_STRUCT cap;
	POINT disp_p;
	BASIC_REPORT_TYPE dummy;
//CJK061012	INT16U zero_code;
	INT16U idx;

	if(GlbOper.PluReportType) return;

//SG061009	zero_code = keyin_getrawcode_bytype(0, KS_ZERO);
//CJK061012	zero_code = keyin_getrawcode_bytype(1, KP_ZERO);

	deptno = status_scale.departmentid;

	pluno=1;
	Dsp_Fill_Buffer(0);
	Dsp_Diffuse();

PAGE_KI_IN:
	// <CODE> JHC, 040621, add
	// [
//@@	if (!Startup.menu_type)
//@@		menu_writepage(2, 1);
	// ]
	display_title_page_change(0x1145,1,2);
	ret = plu_input_department(&deptno, &pluno, 0);
	if(ret==2) {
		goto RET_ESC;
	}
	if (!plu_table_search(deptno,pluno,&idx,0)) {
		goto PAGE_KI_IN;
	}
	if (deptno<1) {
		goto PAGE_KI_IN;
	}
	Dsp_Fill_Buffer(0);
	display_title_page_change(0x1145,2,2);
	ret=DisplayPluSalesSummary(1, deptno, deptno, pluno, pluno,1, &dummy);

#if !defined(USE_SINGLE_LINE_GRAPHIC)
	caption_split_by_code(0x1aaa, &cap,0);	//"TEST = clear count"
	disp_p = point((Startup.menu_type?3:1)*Startup.menu_ygap, Startup.menu_xgap);
#ifdef HEBREW_FONT
		Convert_HebrewPreview((INT8U *)cap.form, strlen(cap.form), 0, 0);
		hebrew_codetemp[strlen(cap.form)] = 0;
		disp_p.x = 202 - disp_p.x;
		Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
		Convert_ArabPreview((INT8U *)cap.form, strlen((char *)cap.form), 0, 0, 0);
		arab_codetemp[strlen((char *)cap.form)] = 0;
		disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
		Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
		Dsp_Write_String(disp_p,(HUGEDATA char *)cap.form);
#endif
#endif

	Dsp_Diffuse();
	while(1) {
		if(KEY_Read()) {
			switch(KeyDrv.CnvCode) {
				// <CODE> JHC, 040621, ADD
				// [
				case KP_ARROW_UP:
				case KP_PAGE_UP:
					BuzOn(1);
					goto PAGE_KI_IN;
				// ]
				case KP_ESC :
					BuzOn(1);
					goto RET_ESC;
				case KP_TEST :
					BuzOn(1);
					ClearPluTransactionCount(deptno, pluno);
					goto RET_ESC;
				case KP_ZERO :
					BuzOn(1);
					result = Report_Confirm(NULL);
					if(result == ON) {
						ClearPluTransactionCountAll();
						goto RET_ESC;
					}
					else
						goto PAGE_KI_IN;
					break;
				default :
					BuzOn(2);
					break;
			}
		}
	}
RET_ESC:
	Dsp_Fill_Buffer(0);   // LCD Buffer clear
	Dsp_Diffuse();
	return;
}
