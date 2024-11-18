/*****************************************************************************
|*			
|*  Copyright		:	(c) 2002 CAS
|*  Filename		:	bar_hri.c
|*  Version		:	0.1
|*  Programmer(s)	:	Hong Yi-Phyo (HYP)
|*  Created		:	2002/11/11
|*  Description		:	    
|*				
****************************************************************************/

#include <stdio.h>
#include <string.h>
#include "globals.h"
#include "commbuf.h"
#include "prt_common.h"
#include "bar_common.h"
#include "prt_font.h"
#ifdef USE_LOGISTICS_BARCODE
#include "initial.h"
#endif

void BarHriCtrlStrConv(HUGEDATA char *dest,HUGEDATA char *src)
{
	HUGEDATA char *p1, *p2;

	p1 = dest;
	p2 = src;

	while(*p2 != '\0') {
//HYP 20040621
		//FONT_CON_CH > 0x20
//		if (*p2 == FONT_CON_CH) {
//			*p1 = FONT_CON_CH;
//			p1++;
//			*p1 = '0';
//		} else {
//			*p1 = *p2;
//		}
		//FONT_CON_CH < 0x20
		*p1 = *p2;

		p1++;
		p2++;
	}
	*p1 = '\0';
}

void BarEanHriStrConv(HUGEDATA char *dest1,HUGEDATA char *dest2,HUGEDATA char *src)
{
	HUGEDATA char *p1, *p2, *p3;
	char dest2_flag;

	p1 = dest1;
	p2 = dest2;
	p3 = src;
	dest2_flag = 0;

	while(*p3 != '\0') {
		if (*p3 == ' ') { //0x20
			dest2_flag = 1;
			*p1 = '\0';
		} else {
			if (dest2_flag == 0) {
				*p1 = *p3;
				p1++;
			} else {
				*p2 = *p3;
				p2++;	
			}
		}
		p3++;
	}
	*p2 = '\0';
}


void Prt_strcpy(HUGEDATA char *dest, HUGEDATA char *src, char num)
{
	char i;
	HUGEDATA char *d, *s;
	
	d = dest;
	s = src;
	i = 0;

	while(i < num) {
		*d = *s;
		d++;
		s++;
		i++;
	}
	*d = '\0';
}

void Prt_strsum(HUGEDATA char *dest, HUGEDATA char *src)
{
	HUGEDATA char *d, *s;
	
	d = dest;
	s = src;

	while(*d != '\0') {
		d++;
	}

	while(*s != 0) {
		*d = *s;
		d++;
		s++;
	}
	*d = '\0';
}

/*
void Prt_strcpy(HUGEDATA char *dest, HUGEDATA char *src)
{
	HUGEDATA char *d, *s;
	
	d = dest;
	s = src;

	while(*s != '\0') {
		*d = *s;
		d++;
		s++;
	}
	*d = '\0';
}
*/

		
void BarHri(HUGEDATA char *HriData, short pw) 
{
	short	bx,by,slen;
	HUGEDATA char *TmpHriData;
	char	binterch_sp;
	char bar_data_buf[MAX_BAR_DATA_BUF_SIZE];
	INT8U id1_backup;
	char magx_backup, magy_backup;
#ifdef USE_LOGISTICS_BARCODE
	INT8U BarTypeCode128Flag = OFF;
#endif
#ifdef USE_SETPRODUCT_SALE
	INT8U i;
#endif

	bx = PrtStruct.x;
	by = PrtStruct.y;
	StrStruct.StrHeight = BarStruct.CharHeight;
//	StrStruct.StrWidth = BarStruct.p2 - BarStruct.p1;
	StrStruct.StrWidth = pw;

//Insert Fixed Font 20031218
//	if(BarStruct.UseOcrb) {
//		binterch_sp = StrStruct.InterChSp;
//		id1_backup = StrStruct.Id1;
//		magx_backup = StrStruct.Magx;
//		magy_backup = StrStruct.Magy;
//		FontLoadOcrB(); ////////
//	}	
	binterch_sp = StrStruct.InterChSp;
	magx_backup = StrStruct.Magx;
	magy_backup = StrStruct.Magy;
//HYP 20050528 Erase
/*
	if(BarStruct.UseOcrb) {
		id1_backup = StrStruct.Id1;
		FontLoadOcrB(); ////////
	} else {
		if(StrStruct.UseFixedWidth == 1) {
			if(FontLoadHri() != 0) return;
		}
	}
*/
	if(pw == -1) return ;
	if(!BarStruct.PrtHri) return;

	switch(BarStruct.BarType) {
		case I2OF5: 
		case S2OF5:
		case IATA2OF5:
		case M2OF5: 
		case POSTNET: 
			slen=strlen(HriData);
			if (!BarStruct.PrtCheckHri && BarStruct.CheckType) {
				*(HriData + slen - 1)=NUL;
			}
			TmpHriData = HriData;
			break;
		case CODE39: 
		case CODE39FULL: 
			slen=strlen(HriData);
			if(!BarStruct.PrtStartStopHri) {
				if (!BarStruct.PrtCheckHri && BarStruct.CheckType) {
					*(HriData + slen - 1)=NUL;
				}
				TmpHriData = HriData;
			} else {
				TmpHriData = bar_data_buf;
				TmpHriData[0] = '*';
				if (!BarStruct.PrtCheckHri && BarStruct.CheckType) {
					*(HriData + slen - 1)=NUL;
					Prt_strcpy(&TmpHriData[1], HriData, slen - 1);
					TmpHriData[slen] = '*';
					TmpHriData[slen+1] = NUL;
				} else {
					Prt_strcpy(&TmpHriData[1], HriData, slen);
					TmpHriData[slen+1] = '*';
					TmpHriData[slen+2] = NUL;
				}
			}
			break;
		case CODABAR: 
			slen=strlen(HriData);
			if(BarStruct.PrtStartStopHri) {
				if (!BarStruct.PrtCheckHri && BarStruct.CheckType) {
					*(HriData + slen - 2)=*(HriData + slen - 1);
					*(HriData + slen - 1)=NUL;
				}
				TmpHriData = HriData;
			} else {
				TmpHriData = bar_data_buf;
				if (!BarStruct.PrtCheckHri && BarStruct.CheckType) {
					*(HriData + slen - 2)=NUL;
					Prt_strcpy(TmpHriData, &HriData[1], slen - 2);
				} else {
					*(HriData + slen - 1)=NUL;
					Prt_strcpy(TmpHriData, &HriData[1], slen - 1);
				}
			}
			break;
		case MSI: 
		case CODE11:
			slen=strlen(HriData);
			if (!BarStruct.PrtCheckHri && BarStruct.CheckType) {
				if(BarStruct.CheckType > 2) {
					*(HriData + slen - 2)=NUL;
				} else {
					*(HriData + slen - 1)=NUL;
				}
			}
			TmpHriData = HriData;
			break;
		case PLESSEY:
			slen=strlen(HriData);
			if (!BarStruct.PrtCheckHri && BarStruct.CheckType) {
				*(HriData + slen - 2)=NUL;
				*(HriData + slen - 1)=NUL;
			}
			TmpHriData = HriData;
			break;
#ifdef USE_LOGISTICS_BARCODE
		case CODE128A:
		case CODE128B:
		case CODE128C:
		case CODE128X:
			if(BarStruct.BarWidthMag==1)
			{
				StrStruct.LineSp = 0;			
				StrStruct.StrHeight = 16 * 2;
				BarTypeCode128Flag = ON;
			}
			BarStruct.UseOcrb = 0;
			TmpHriData = HriData;							
			break;
		case RSSEXP:
			StrStruct.LineSp = 0;
			slen=strlen(HriData);
			if(BarStruct.BarWidthMag==1) slen  /= 18;
			if(BarStruct.BarWidthMag==2) slen  /= 38;
			if(BarStruct.BarWidthMag==3) slen  /= 57;
			if(BarStruct.BarWidthMag==4) slen  /= 76;
			StrStruct.StrHeight = 16 * (slen + 1);
			BarStruct.UseOcrb = 0;
			TmpHriData = HriData;			
			break;
#endif			
		default:
			TmpHriData = HriData;
	}
#ifdef USE_SETPRODUCT_SALE
	if(strlen(HriData) == 20)
	{
		for(i=6; i<20; i++)
		{
			if(i<12)
			{
				TmpHriData[i] = TmpHriData[i-6];
				TmpHriData[i-6] = 0x20;
			}
			else
				TmpHriData[i] = 0x20;
		}
	}
		
#endif
//HYP 20050528 Insert
#ifdef USE_ARAB_TICKET_PRT
    id1_backup = StrStruct.Id1;
	FontLoadOcrB(pw,strlen(TmpHriData));
#else
	if(BarStruct.UseOcrb) {
		id1_backup = StrStruct.Id1;
		FontLoadOcrB(pw,strlen(TmpHriData)); ////////
	} else {
		if(StrStruct.UseFixedWidth == 1) {
			if(FontLoadHri(pw,strlen(TmpHriData)) != 0) return;
		}
	}
#endif	
	switch (PrtStruct.Rot + BarStruct.BarHriPos*2) {
		case DEG_0:
		case DEG_360:
			PrtStruct.y = by + BarStruct.FullHeight - BarStruct.CharHeight;
			PrtStruct.x = bx + BarStruct.QuietZone;
			FontStrOut(TmpHriData);
			break;
		case DEG_180:
		case DEG_540:
#ifdef USE_LOGISTICS_BARCODE
			if(BarStruct.BarWidthMag==1 && BarTypeCode128Flag == ON)//HRI 높이 2줄 고정
				PrtStruct.y = by - (BarStruct.CharHeight + 1); // 2줄 인쇄할 좌표
			else
#endif
			PrtStruct.y = by;
			PrtStruct.x = bx + BarStruct.QuietZone;	 
			FontStrOut(TmpHriData);
			break;
		case DEG_90:
		case DEG_450:
#ifdef USE_LOGISTICS_BARCODE
			if(BarStruct.BarWidthMag==1 && BarTypeCode128Flag == ON)//HRI 높이 2줄 고정
				PrtStruct.x = bx - (BarStruct.CharHeight + 1); // 2줄 인쇄할 자표
			else
#endif
			PrtStruct.x = bx;
			PrtStruct.y = by + BarStruct.QuietZone;
			FontStrOut(TmpHriData);
			break;
		case DEG_270:
		case DEG_630:
			PrtStruct.x = bx + BarStruct.FullHeight - BarStruct.CharHeight + 1;
			PrtStruct.y = by + BarStruct.QuietZone;
			FontStrOut(TmpHriData);
			break;
	}
//Insert Fixed Font 20031218
//	if(BarStruct.UseOcrb) {
//		FontLoadFont1(id1_backup);
//		StrStruct.InterChSp = binterch_sp;
//		StrStruct.Magx = magx_backup;
//		StrStruct.Magy = magy_backup;
//	}
#ifdef USE_ARAB_TICKET_PRT
       FontLoadFont1(id1_backup);
#else
	if(BarStruct.UseOcrb) {
		FontLoadFont1(id1_backup);
	}
#endif
	StrStruct.InterChSp = binterch_sp;
	StrStruct.Magx = magx_backup;
	StrStruct.Magy = magy_backup;

	PrtStruct.x = bx;
	PrtStruct.y = by;
}
#ifdef USE_EAN13
void BarHriEan13(HUGEDATA char *HriData) 
{
	short	bx,by;
	HUGEDATA INT8U *fontw_ptr;
	char wbits;
	char binterch_sp;
	char bar_data_buf[10];
	INT8U id1_backup;
	char magx_backup, magy_backup;
//	char string_buf[50];

	bx = PrtStruct.x;
	by = PrtStruct.y;
	StrStruct.StrHeight = BarStruct.CharHeight;
	StrStruct.StrWidth = BarStruct.p2 - BarStruct.p1;

	if(BarStruct.GuardType != NO_GUARD) {
		binterch_sp = StrStruct.InterChSp;
		id1_backup = StrStruct.Id1;
		magx_backup = StrStruct.Magx;
		magy_backup = StrStruct.Magy;
		FontLoadOcrB(0,0); ////////
		switch (PrtStruct.Rot) {
			case DEG_0:
				fontw_ptr = FontStruct.PtrWidth1 + (HriData[0]-FontStruct.FirstCode1);
				wbits = *(fontw_ptr) * StrStruct.Magx;

				PrtStruct.x = bx - wbits - BarStruct.HriGapEan + BarStruct.QuietZone;
				PrtStruct.y = by + BarStruct.FullHeight - BarStruct.CharHeight;
				FontCharOut(HriData[0], PrtStruct.Rot);
			
				PrtStruct.x = bx + BarStruct.p1 + BarStruct.QuietZone;
				Prt_strcpy(bar_data_buf,&HriData[1],6);
				FontStrOut(bar_data_buf);
				PrtStruct.x = bx +BarStruct.p3 + BarStruct.QuietZone;	 
				Prt_strcpy(bar_data_buf,&HriData[7],6);
				FontStrOut(bar_data_buf);
				break;
			case DEG_180:
				PrtStruct.x = bx + BarStruct.p5 + BarStruct.HriGapEan + BarStruct.QuietZone;
				PrtStruct.y = by;
				FontCharOut(HriData[0], PrtStruct.Rot);
	
				PrtStruct.x = bx + BarStruct.p5 - BarStruct.p2 + BarStruct.QuietZone;
				Prt_strcpy(bar_data_buf,&HriData[1],6);
				FontStrOut(bar_data_buf);
	
				PrtStruct.x = bx + BarStruct.p5 - BarStruct.p4 + BarStruct.QuietZone;	 
				Prt_strcpy(bar_data_buf,&HriData[7],6);
				FontStrOut(bar_data_buf);
				break;
			case DEG_90:
				fontw_ptr = FontStruct.PtrWidth1 + (HriData[0]-FontStruct.FirstCode1);
				wbits = *(fontw_ptr) * StrStruct.Magx;

				PrtStruct.x = bx;
				PrtStruct.y = by - wbits - BarStruct.HriGapEan + BarStruct.QuietZone;
				FontCharOut(HriData[0], PrtStruct.Rot);
			
				PrtStruct.y = by + BarStruct.p1 + BarStruct.QuietZone;
				Prt_strcpy(bar_data_buf,&HriData[1],6);
				FontStrOut(bar_data_buf);
	
				PrtStruct.y = by +BarStruct.p3 + BarStruct.QuietZone;	 
				Prt_strcpy(bar_data_buf,&HriData[7],6);
				FontStrOut(bar_data_buf);
				break;
			case DEG_270:
				PrtStruct.x = bx + BarStruct.FullHeight - BarStruct.CharHeight + 1;
				PrtStruct.y = by + BarStruct.p5 + BarStruct.HriGapEan + BarStruct.QuietZone;
				FontCharOut(HriData[0], PrtStruct.Rot);
			
				PrtStruct.y = by + BarStruct.p5 - BarStruct.p2 + BarStruct.QuietZone;
				Prt_strcpy(bar_data_buf,&HriData[1],6);
				FontStrOut(bar_data_buf);
	
				PrtStruct.y = by + BarStruct.p5 - BarStruct.p4 + BarStruct.QuietZone;	 
				Prt_strcpy(bar_data_buf,&HriData[7],6);
				FontStrOut(bar_data_buf);
				break;
		}
		FontLoadFont1(id1_backup);
		StrStruct.InterChSp = binterch_sp;
		StrStruct.Magx = magx_backup;
		StrStruct.Magy = magy_backup;
	} else {
		BarHri(HriData, BarStruct.p5);
	}
	PrtStruct.x = bx;
	PrtStruct.y = by;
}
#endif //#ifdef USE_EAN13
#if defined(USE_EAN13P2_UPCAP2_P5_EAN8P2_P5) || defined(USE_EAN13SUP5)
void BarHriEan13A(HUGEDATA char *HriData, HUGEDATA char *HriData2) 
{
	short	bx,by;
	HUGEDATA INT8U *fontw_ptr;
	char wbits;
	char binterch_sp;
	char bar_data_buf[10];
	INT8U id1_backup;
	char magx_backup, magy_backup;

	bx = PrtStruct.x;
	by = PrtStruct.y;
	StrStruct.StrHeight = BarStruct.CharHeight;
	StrStruct.StrWidth = BarStruct.p2 - BarStruct.p1;

	if(BarStruct.GuardType != NO_GUARD) {
		binterch_sp = StrStruct.InterChSp;
		id1_backup = StrStruct.Id1;
		magx_backup = StrStruct.Magx;
		magy_backup = StrStruct.Magy;
		FontLoadOcrB(0,0); ////////
		switch (PrtStruct.Rot) {
			case DEG_0:
				fontw_ptr = FontStruct.PtrWidth1 + (HriData[0]-FontStruct.FirstCode1);
				wbits = *(fontw_ptr) * StrStruct.Magx;
				
				PrtStruct.x = bx - wbits - BarStruct.HriGapEan+ BarStruct.QuietZone;
				PrtStruct.y = by + BarStruct.FullHeight - BarStruct.CharHeight;
				FontCharOut(HriData[0], PrtStruct.Rot);
					
				PrtStruct.x = bx + BarStruct.p1+ BarStruct.QuietZone;
				Prt_strcpy(bar_data_buf,&HriData[1],6);
				FontStrOut(bar_data_buf);
		
				PrtStruct.x = bx +BarStruct.p3+ BarStruct.QuietZone;	 
				Prt_strcpy(bar_data_buf,&HriData[7],6);
				FontStrOut(bar_data_buf);
		
				//sup
				PrtStruct.y = by;
				StrStruct.StrWidth = BarStruct.x2 - BarStruct.x1;
				PrtStruct.x = bx + BarStruct.x1+ BarStruct.QuietZone;
				FontStrOut(HriData2);
				
				break;
			case DEG_180:
				PrtStruct.x = bx + BarStruct.x2 + BarStruct.HriGapEan+ BarStruct.QuietZone;
				PrtStruct.y = by;
				FontCharOut(HriData[0], PrtStruct.Rot);
				
				PrtStruct.x = bx + BarStruct.x2 - BarStruct.p2+ BarStruct.QuietZone;
				Prt_strcpy(bar_data_buf,&HriData[1],6);
				FontStrOut(bar_data_buf);
			
				PrtStruct.x = bx + BarStruct.x2 - BarStruct.p4+ BarStruct.QuietZone;	 
				Prt_strcpy(bar_data_buf,&HriData[7],6);
				FontStrOut(bar_data_buf);
				
				//sup
 				PrtStruct.y = by + BarStruct.FullHeight - BarStruct.CharHeight;
				StrStruct.StrWidth = BarStruct.x2 - BarStruct.x1;
				PrtStruct.x = bx+ BarStruct.QuietZone;
				FontStrOut(HriData2);
			
				break;
			case DEG_90:
				fontw_ptr = FontStruct.PtrWidth1 + (HriData[0]-FontStruct.FirstCode1);
				wbits = *(fontw_ptr) * StrStruct.Magx;
				
				PrtStruct.x = bx;
				PrtStruct.y = by - wbits - BarStruct.HriGapEan+ BarStruct.QuietZone;
				FontCharOut(HriData[0], PrtStruct.Rot);
				
				PrtStruct.y = by + BarStruct.p1+ BarStruct.QuietZone;
				Prt_strcpy(bar_data_buf,&HriData[1],6);
				FontStrOut(bar_data_buf);
	
				PrtStruct.y = by +BarStruct.p3+ BarStruct.QuietZone;	 
				Prt_strcpy(bar_data_buf,&HriData[7],6);
				FontStrOut(bar_data_buf);

				//sup
				PrtStruct.x = bx + BarStruct.FullHeight - BarStruct.CharHeight;
				StrStruct.StrWidth = BarStruct.x2 - BarStruct.x1;
				PrtStruct.y = by + BarStruct.x1+ BarStruct.QuietZone;
				FontStrOut(HriData2);
	
				break;
			case DEG_270:
				PrtStruct.x = bx + BarStruct.FullHeight - BarStruct.CharHeight + 1;
				PrtStruct.y = by + BarStruct.x2 + BarStruct.HriGapEan+ BarStruct.QuietZone;
				FontCharOut(HriData[0], PrtStruct.Rot);
			
				PrtStruct.y = by + BarStruct.x2 - BarStruct.p2+ BarStruct.QuietZone;
				Prt_strcpy(bar_data_buf,&HriData[1],6);
				FontStrOut(bar_data_buf);
	
				PrtStruct.y = by + BarStruct.x2 - BarStruct.p4+ BarStruct.QuietZone;	 
				Prt_strcpy(bar_data_buf,&HriData[7],6);
				FontStrOut(bar_data_buf);

				//sup
				PrtStruct.x = bx;
				PrtStruct.y = by+ BarStruct.QuietZone;
 				StrStruct.StrWidth = BarStruct.x2 - BarStruct.x1;
				FontStrOut(HriData2);
	
				break;
		}
		FontLoadFont1(id1_backup);
		StrStruct.InterChSp = binterch_sp;
		StrStruct.Magx = magx_backup;
		StrStruct.Magy = magy_backup;
	} else {
		Prt_strsum(HriData,HriData2);
		BarHri(HriData, BarStruct.x2);
	}
	PrtStruct.x = bx;
	PrtStruct.y = by;
}
#endif //#if defined(USE_EAN13P2_UPCAP2_P5_EAN8P2_P5) || defined(USE_EAN13SUP5)
#ifdef USE_EAN8
void BarHriEan8(HUGEDATA char *HriData) 
{
	short	bx,by;
	char	binterch_sp;
	char bar_data_buf[10];
	INT8U id1_backup;
	char magx_backup, magy_backup;

	bx = PrtStruct.x;
	by = PrtStruct.y;
	StrStruct.StrHeight = BarStruct.CharHeight;
	StrStruct.StrWidth = BarStruct.p2 - BarStruct.p1;

	if(BarStruct.GuardType != NO_GUARD) {
		binterch_sp = StrStruct.InterChSp;
		id1_backup = StrStruct.Id1;
		magx_backup = StrStruct.Magx;
		magy_backup = StrStruct.Magy;
		FontLoadOcrB(0,0); ////////
		switch (PrtStruct.Rot) {
			case DEG_0:
				PrtStruct.y = by + BarStruct.FullHeight - BarStruct.CharHeight;
				PrtStruct.x = bx + BarStruct.p1+ BarStruct.QuietZone;
				Prt_strcpy(bar_data_buf,&HriData[0],4);
				FontStrOut(bar_data_buf);
	
				PrtStruct.x = bx +BarStruct.p3+ BarStruct.QuietZone;	 
				Prt_strcpy(bar_data_buf,&HriData[4],4);
				FontStrOut(bar_data_buf);
				break;
			case DEG_180:
				PrtStruct.y = by;
				PrtStruct.x = bx + BarStruct.p5 - BarStruct.p2+ BarStruct.QuietZone;
				Prt_strcpy(bar_data_buf,&HriData[0],4);
				FontStrOut(bar_data_buf);
	
				PrtStruct.x = bx + BarStruct.p5 - BarStruct.p4+ BarStruct.QuietZone;	 
				Prt_strcpy(bar_data_buf,&HriData[4],4);
				FontStrOut(bar_data_buf);
				break;
			case DEG_90:
				PrtStruct.x = bx;
				PrtStruct.y = by + BarStruct.p1+ BarStruct.QuietZone;
				Prt_strcpy(bar_data_buf,&HriData[0],4);
				FontStrOut(bar_data_buf);
	
				PrtStruct.y = by +BarStruct.p3+ BarStruct.QuietZone;	 
				Prt_strcpy(bar_data_buf,&HriData[4],4);
				FontStrOut(bar_data_buf);
				break;
			case DEG_270:
				PrtStruct.x = bx + BarStruct.FullHeight - BarStruct.CharHeight + 1;
				PrtStruct.y = by + BarStruct.p5 - BarStruct.p2+ BarStruct.QuietZone;
				Prt_strcpy(bar_data_buf,&HriData[0],4);
				FontStrOut(bar_data_buf);
	
				PrtStruct.y = by + BarStruct.p5 - BarStruct.p4+ BarStruct.QuietZone;	 
				Prt_strcpy(bar_data_buf,&HriData[4],4);
				FontStrOut(bar_data_buf);
				break;
		}
		FontLoadFont1(id1_backup);
		StrStruct.InterChSp = binterch_sp;
		StrStruct.Magx = magx_backup;
		StrStruct.Magy = magy_backup;
	} else {
		BarHri(HriData, BarStruct.p5);
	}
	PrtStruct.x = bx;
	PrtStruct.y = by;
}
#endif //#ifdef USE_EAN8
#ifdef USE_EAN13P2_UPCAP2_P5_EAN8P2_P5
void BarHriEan8A(HUGEDATA char *HriData, HUGEDATA char *HriData2) 
{
	short	bx,by;
	char	binterch_sp;
	char bar_data_buf[10];
	INT8U id1_backup;
	char magx_backup, magy_backup;

	bx = PrtStruct.x;
	by = PrtStruct.y;
	StrStruct.StrHeight = BarStruct.CharHeight;
	StrStruct.StrWidth = BarStruct.p2 - BarStruct.p1;

	if(BarStruct.GuardType != NO_GUARD) {
		binterch_sp = StrStruct.InterChSp;
		id1_backup = StrStruct.Id1;
		magx_backup = StrStruct.Magx;
		magy_backup = StrStruct.Magy;
		FontLoadOcrB(0,0); ////////
		switch (PrtStruct.Rot) {
			case DEG_0:
				PrtStruct.y = by + BarStruct.FullHeight - BarStruct.CharHeight;
				PrtStruct.x = bx + BarStruct.p1+ BarStruct.QuietZone;
				Prt_strcpy(bar_data_buf,&HriData[0],4);
				FontStrOut(bar_data_buf);
		
				PrtStruct.x = bx +BarStruct.p3+ BarStruct.QuietZone;	 
				Prt_strcpy(bar_data_buf,&HriData[4],4);
				FontStrOut(bar_data_buf);
				//sup
				PrtStruct.y = by;
				StrStruct.StrWidth = BarStruct.x2 - BarStruct.x1;
				PrtStruct.x = bx + BarStruct.x1+ BarStruct.QuietZone;
				FontStrOut(HriData2);
				break;
			case DEG_180:
				PrtStruct.y = by;
				PrtStruct.x = bx + BarStruct.x2 - BarStruct.p2+ BarStruct.QuietZone;
				Prt_strcpy(bar_data_buf,&HriData[0],4);
				FontStrOut(bar_data_buf);
		
				PrtStruct.x = bx + BarStruct.x2 - BarStruct.p4+ BarStruct.QuietZone;	 
				Prt_strcpy(bar_data_buf,&HriData[4],4);
				FontStrOut(bar_data_buf);
				//sup
 				PrtStruct.y = by + BarStruct.FullHeight - BarStruct.CharHeight;
				StrStruct.StrWidth = BarStruct.x2 - BarStruct.x1;
				PrtStruct.x = bx+ BarStruct.QuietZone;
				FontStrOut(HriData2);
				break;
			case DEG_90:
				PrtStruct.x = bx;
				PrtStruct.y = by + BarStruct.p1+ BarStruct.QuietZone;
				Prt_strcpy(bar_data_buf,&HriData[0],4);
				FontStrOut(bar_data_buf);
			
				PrtStruct.y = by +BarStruct.p3+ BarStruct.QuietZone;	 
				Prt_strcpy(bar_data_buf,&HriData[4],4);
				FontStrOut(bar_data_buf);
				//sup
				PrtStruct.x = bx + BarStruct.FullHeight - BarStruct.CharHeight;
				StrStruct.StrWidth = BarStruct.x2 - BarStruct.x1;
				PrtStruct.y = by + BarStruct.x1+ BarStruct.QuietZone;
				FontStrOut(HriData2);
				break;
			case DEG_270:
				PrtStruct.x = bx + BarStruct.FullHeight - BarStruct.CharHeight + 1;
				PrtStruct.y = by + BarStruct.x2 - BarStruct.p2+ BarStruct.QuietZone;
				Prt_strcpy(bar_data_buf,&HriData[0],4);
				FontStrOut(bar_data_buf);
			
				PrtStruct.y = by + BarStruct.x2 - BarStruct.p4+ BarStruct.QuietZone;	 
				Prt_strcpy(bar_data_buf,&HriData[4],4);
				FontStrOut(bar_data_buf);
				//sup
				PrtStruct.x = bx;
				PrtStruct.y = by+ BarStruct.QuietZone;
 				StrStruct.StrWidth = BarStruct.x2 - BarStruct.x1;
				FontStrOut(HriData2);
				break;
		}
		FontLoadFont1(id1_backup);
		StrStruct.InterChSp = binterch_sp;
		StrStruct.Magx = magx_backup;
		StrStruct.Magy = magy_backup;
	} else {
		Prt_strsum(HriData,HriData2);
		BarHri(HriData, BarStruct.x2);
	}
	PrtStruct.x = bx;
	PrtStruct.y = by;

}
#endif//#ifdef USE_EAN13P2_UPCAP2_P5_EAN8P2_P5
#ifdef USE_UPCA
void BarHriUpca(HUGEDATA char *HriData) 
{
	short	bx,by;
	HUGEDATA INT8U *fontw_ptr;
	char wbits;
	char	binterch_sp;
	char bar_data_buf[10];
	INT8U id1_backup;
	char magx_backup, magy_backup;

	bx = PrtStruct.x;
	by = PrtStruct.y;
	StrStruct.StrHeight = BarStruct.CharHeight;
	StrStruct.StrWidth = BarStruct.p2 - BarStruct.p1;

	if(BarStruct.GuardType != NO_GUARD) {
		binterch_sp = StrStruct.InterChSp;
		id1_backup = StrStruct.Id1;
		magx_backup = StrStruct.Magx;
		magy_backup = StrStruct.Magy;
		FontLoadOcrB(0,0); ////////
		switch (PrtStruct.Rot) {
			case DEG_0:
				fontw_ptr = FontStruct.PtrWidth1 + (HriData[0]-FontStruct.FirstCode1);
				wbits = *(fontw_ptr) * StrStruct.Magx;

				PrtStruct.x = bx - wbits - BarStruct.HriGapEan+ BarStruct.QuietZone;
				PrtStruct.y = by + BarStruct.FullHeight - BarStruct.CharHeight;
				FontCharOut(HriData[0], PrtStruct.Rot);
				
				PrtStruct.x = bx + BarStruct.p1+ BarStruct.QuietZone;
				Prt_strcpy(bar_data_buf,&HriData[1],5);
				FontStrOut(bar_data_buf);
	
				PrtStruct.x = bx +BarStruct.p3+ BarStruct.QuietZone;	 
				Prt_strcpy(bar_data_buf,&HriData[6],5);
				FontStrOut(bar_data_buf);
	
				PrtStruct.x = bx + BarStruct.p5 + BarStruct.HriGapEan+ BarStruct.QuietZone;
				FontCharOut(HriData[11], PrtStruct.Rot);
				break;
			case DEG_180:
				PrtStruct.x = bx + BarStruct.p5 + BarStruct.HriGapEan+ BarStruct.QuietZone;
				PrtStruct.y = by;
				FontCharOut(HriData[0], PrtStruct.Rot);
			
				PrtStruct.x = bx + BarStruct.p5 - BarStruct.p2+ BarStruct.QuietZone;
				Prt_strcpy(bar_data_buf,&HriData[1],5);
				FontStrOut(bar_data_buf);
	
				PrtStruct.x = bx + BarStruct.p5 - BarStruct.p4+ BarStruct.QuietZone;	 
				Prt_strcpy(bar_data_buf,&HriData[6],5);
				FontStrOut(bar_data_buf);

				fontw_ptr = FontStruct.PtrWidth1 + (HriData[11]-FontStruct.FirstCode1);
				wbits = *(fontw_ptr) * StrStruct.Magx;
			
				PrtStruct.x = bx + BarStruct.p5 - BarStruct.p5 - wbits - BarStruct.HriGapEan+ BarStruct.QuietZone;
				FontCharOut(HriData[11], PrtStruct.Rot);
				break;
			case DEG_90:
				fontw_ptr = FontStruct.PtrWidth1 + (HriData[0]-FontStruct.FirstCode1);
				wbits = *(fontw_ptr) * StrStruct.Magx;
				
				PrtStruct.x = bx;
				PrtStruct.y = by - wbits - BarStruct.HriGapEan+ BarStruct.QuietZone;
				FontCharOut(HriData[0], PrtStruct.Rot);
				
				PrtStruct.y = by + BarStruct.p1+ BarStruct.QuietZone;
				Prt_strcpy(bar_data_buf,&HriData[1],5);
				FontStrOut(bar_data_buf);
		
				PrtStruct.y = by +BarStruct.p3+ BarStruct.QuietZone;	 
				Prt_strcpy(bar_data_buf,&HriData[6],5);
				FontStrOut(bar_data_buf);

				PrtStruct.y = by + BarStruct.p5 +BarStruct.HriGapEan+ BarStruct.QuietZone;
				FontCharOut(HriData[11], PrtStruct.Rot);
				break;
			case DEG_270:
				PrtStruct.x = bx + BarStruct.FullHeight - BarStruct.CharHeight + 1;
				PrtStruct.y = by + BarStruct.p5 + BarStruct.HriGapEan+ BarStruct.QuietZone;
				FontCharOut(HriData[0], PrtStruct.Rot);
			
				PrtStruct.y = by + BarStruct.p5 - BarStruct.p2+ BarStruct.QuietZone;
				Prt_strcpy(bar_data_buf,&HriData[1],5);
				FontStrOut(bar_data_buf);
	
				PrtStruct.y = by + BarStruct.p5 - BarStruct.p4+ BarStruct.QuietZone;	 
				Prt_strcpy(bar_data_buf,&HriData[6],5);
				FontStrOut(bar_data_buf);

				fontw_ptr = FontStruct.PtrWidth1 + (HriData[11]-FontStruct.FirstCode1);
				wbits = *(fontw_ptr) * StrStruct.Magx;
			
				PrtStruct.y = by - wbits - BarStruct.HriGapEan+ BarStruct.QuietZone;
				FontCharOut(HriData[11], PrtStruct.Rot);
				break;
		}
		FontLoadFont1(id1_backup);
		StrStruct.InterChSp = binterch_sp;
		StrStruct.Magx = magx_backup;
		StrStruct.Magy = magy_backup;
	} else {
		BarHri(HriData, BarStruct.p5);
	}
	PrtStruct.x = bx;
	PrtStruct.y = by;
}
#endif //#ifdef USE_UPCA
#ifdef USE_EAN13P2_UPCAP2_P5_EAN8P2_P5
void BarHriUpcaA(HUGEDATA char *HriData, HUGEDATA char *HriData2) 
{
	short	bx,by;
	HUGEDATA INT8U *fontw_ptr;
	char wbits;
	char	binterch_sp;
	char bar_data_buf[10];
	INT8U id1_backup;
	char magx_backup, magy_backup;

	bx = PrtStruct.x;
	by = PrtStruct.y;
	StrStruct.StrHeight = BarStruct.CharHeight;
	StrStruct.StrWidth = BarStruct.p2 - BarStruct.p1;

	if(BarStruct.GuardType != NO_GUARD) {
		binterch_sp = StrStruct.InterChSp;
		id1_backup = StrStruct.Id1;
		magx_backup = StrStruct.Magx;
		magy_backup = StrStruct.Magy;
		FontLoadOcrB(0,0); ////////
		switch (PrtStruct.Rot) {
			case DEG_0:
				fontw_ptr = FontStruct.PtrWidth1 + (HriData[0]-FontStruct.FirstCode1);
				wbits = *(fontw_ptr) * StrStruct.Magx;

				PrtStruct.x = bx - wbits - BarStruct.HriGapEan+ BarStruct.QuietZone;
				PrtStruct.y = by + BarStruct.FullHeight - BarStruct.CharHeight;
				FontCharOut(HriData[0], PrtStruct.Rot);
				
				PrtStruct.x = bx + BarStruct.p1+ BarStruct.QuietZone;
				Prt_strcpy(bar_data_buf,&HriData[1],5);
				FontStrOut(bar_data_buf);
		
				PrtStruct.x = bx +BarStruct.p3+ BarStruct.QuietZone;	 
				Prt_strcpy(bar_data_buf,&HriData[6],5);
				FontStrOut(bar_data_buf);

				PrtStruct.x = bx + BarStruct.p5 + BarStruct.HriGapEan+ BarStruct.QuietZone;
				FontCharOut(HriData[11], PrtStruct.Rot);
				//sup
				PrtStruct.y = by;
				StrStruct.StrWidth = BarStruct.x2 - BarStruct.x1;
				PrtStruct.x = bx + BarStruct.x1+ BarStruct.QuietZone;
				FontStrOut(HriData2);
				break;
			case DEG_180:
				PrtStruct.x = bx + BarStruct.x2 + BarStruct.HriGapEan+ BarStruct.QuietZone;
				PrtStruct.y = by;
				FontCharOut(HriData[0], PrtStruct.Rot);
				
				PrtStruct.x = bx + BarStruct.x2 - BarStruct.p2+ BarStruct.QuietZone;
				Prt_strcpy(bar_data_buf,&HriData[1],5);
				FontStrOut(bar_data_buf);
		
				PrtStruct.x = bx + BarStruct.x2 - BarStruct.p4+ BarStruct.QuietZone;	 
				Prt_strcpy(bar_data_buf,&HriData[6],5);
				FontStrOut(bar_data_buf);
		
				fontw_ptr = FontStruct.PtrWidth1 + (HriData[11]-FontStruct.FirstCode1);
				wbits = *(fontw_ptr) * StrStruct.Magx;

				PrtStruct.x = bx + BarStruct.x2 - BarStruct.p5 - wbits - BarStruct.HriGapEan+ BarStruct.QuietZone;
				FontCharOut(HriData[11], PrtStruct.Rot);
				//sup
	 			PrtStruct.y = by + BarStruct.FullHeight - BarStruct.CharHeight;
				StrStruct.StrWidth = BarStruct.x2 - BarStruct.x1;
				PrtStruct.x = bx+ BarStruct.QuietZone;
				FontStrOut(HriData2);
				break;
			case DEG_90:
				fontw_ptr = FontStruct.PtrWidth1 + (HriData[0]-FontStruct.FirstCode1);
				wbits = *(fontw_ptr) * StrStruct.Magx;

				PrtStruct.x = bx;
				PrtStruct.y = by - wbits - BarStruct.HriGapEan+ BarStruct.QuietZone;
				FontCharOut(HriData[0], PrtStruct.Rot);
			
				PrtStruct.y = by + BarStruct.p1+ BarStruct.QuietZone;
				Prt_strcpy(bar_data_buf,&HriData[1],5);
				FontStrOut(bar_data_buf);
	
				PrtStruct.y = by +BarStruct.p3+ BarStruct.QuietZone;	 
				Prt_strcpy(bar_data_buf,&HriData[6],5);
				FontStrOut(bar_data_buf);

				PrtStruct.y = by + BarStruct.p5 +BarStruct.HriGapEan+ BarStruct.QuietZone;
				FontCharOut(HriData[11], PrtStruct.Rot);
				//sup
				PrtStruct.x = bx + BarStruct.FullHeight - BarStruct.CharHeight;
				StrStruct.StrWidth = BarStruct.x2 - BarStruct.x1;
				PrtStruct.y = by + BarStruct.x1+ BarStruct.QuietZone;
				FontStrOut(HriData2);
				break;
			case DEG_270:
				PrtStruct.x = bx + BarStruct.FullHeight - BarStruct.CharHeight + 1;
				PrtStruct.y = by + BarStruct.x2 + BarStruct.HriGapEan+ BarStruct.QuietZone;
				FontCharOut(HriData[0], PrtStruct.Rot);
				
				PrtStruct.y = by + BarStruct.x2 - BarStruct.p2+ BarStruct.QuietZone;
				Prt_strcpy(bar_data_buf,&HriData[1],5);
				FontStrOut(bar_data_buf);
			
				PrtStruct.y = by + BarStruct.x2 - BarStruct.p4+ BarStruct.QuietZone;	 
				Prt_strcpy(bar_data_buf,&HriData[6],5);
				FontStrOut(bar_data_buf);
			
				fontw_ptr = FontStruct.PtrWidth1 + (HriData[11]-FontStruct.FirstCode1);
				wbits = *(fontw_ptr) * StrStruct.Magx;
			
				PrtStruct.y = by + BarStruct.x2 - BarStruct.p5 - wbits - BarStruct.HriGapEan+ BarStruct.QuietZone;
				FontCharOut(HriData[11], PrtStruct.Rot);
				//sup
				PrtStruct.x = bx;
				PrtStruct.y = by+ BarStruct.QuietZone;
 				StrStruct.StrWidth = BarStruct.x2 - BarStruct.x1;
				FontStrOut(HriData2);
				break;
		}
		FontLoadFont1(id1_backup);
		StrStruct.InterChSp = binterch_sp;
		StrStruct.Magx = magx_backup;
		StrStruct.Magy = magy_backup;
	} else {
		Prt_strsum(HriData,HriData2);
		BarHri(HriData, BarStruct.x2);
	}
	PrtStruct.x = bx;
	PrtStruct.y = by;
}
#endif//#ifdef USE_EAN13P2_UPCAP2_P5_EAN8P2_P5

#ifdef USE_UPCE0_UPCE1
void BarHriUpce(HUGEDATA char *HriData) 
{
	short	bx,by;
	HUGEDATA INT8U *fontw_ptr;
	char wbits;
	char	binterch_sp;
	char bar_data_buf[10];
	INT8U id1_backup;
	char magx_backup, magy_backup;

	bx = PrtStruct.x;
	by = PrtStruct.y;
	StrStruct.StrHeight = BarStruct.CharHeight;
	StrStruct.StrWidth = BarStruct.p2 - BarStruct.p1;

	if(BarStruct.GuardType != NO_GUARD) {
		binterch_sp = StrStruct.InterChSp;
		id1_backup = StrStruct.Id1;
		magx_backup = StrStruct.Magx;
		magy_backup = StrStruct.Magy;
		FontLoadOcrB(0,0); ////////
		switch (PrtStruct.Rot) {
			case DEG_0:
				fontw_ptr = FontStruct.PtrWidth1 + ('0'-FontStruct.FirstCode1);
				wbits = *(fontw_ptr) * StrStruct.Magx;

				PrtStruct.x = bx - wbits - BarStruct.HriGapEan+ BarStruct.QuietZone;
				PrtStruct.y = by + BarStruct.FullHeight - BarStruct.CharHeight;
				FontCharOut('0', PrtStruct.Rot);
				
				PrtStruct.x = bx + BarStruct.p1+ BarStruct.QuietZone;
				Prt_strcpy(bar_data_buf,&HriData[0],6);
				FontStrOut(bar_data_buf);

				PrtStruct.x = bx + BarStruct.p3 + BarStruct.HriGapEan+ BarStruct.QuietZone;
				FontCharOut(HriData[6], PrtStruct.Rot);
				break;
			case DEG_180:
				PrtStruct.x = bx + BarStruct.p3 + BarStruct.HriGapEan+ BarStruct.QuietZone;
				PrtStruct.y = by;
				FontCharOut('0', PrtStruct.Rot);
				
				PrtStruct.x = bx + BarStruct.p5 - BarStruct.p2+ BarStruct.QuietZone;	 
				Prt_strcpy(bar_data_buf,&HriData[0],6);
				FontStrOut(bar_data_buf);
	
				fontw_ptr = FontStruct.PtrWidth1 + (HriData[6]-FontStruct.FirstCode1);
				wbits = *(fontw_ptr) * StrStruct.Magx;
				
				PrtStruct.x = bx - wbits - BarStruct.HriGapEan+ BarStruct.QuietZone;
				FontCharOut(HriData[6], PrtStruct.Rot);
				break;
			case DEG_90:
				fontw_ptr = FontStruct.PtrWidth1 + ('0'-FontStruct.FirstCode1);
				wbits = *(fontw_ptr) * StrStruct.Magx;
	
				PrtStruct.x = bx;
				PrtStruct.y = by - wbits - BarStruct.HriGapEan+ BarStruct.QuietZone;
				FontCharOut('0', PrtStruct.Rot);
			
				PrtStruct.y = by + BarStruct.p1+ BarStruct.QuietZone;
				Prt_strcpy(bar_data_buf,&HriData[0],6);
				FontStrOut(bar_data_buf);

				PrtStruct.y = by + BarStruct.p3 +BarStruct.HriGapEan+ BarStruct.QuietZone;
				FontCharOut(HriData[6], PrtStruct.Rot);
				break;
			case DEG_270:
				PrtStruct.x = bx + BarStruct.FullHeight - BarStruct.CharHeight + 1;
				PrtStruct.y = by + BarStruct.p3 + BarStruct.HriGapEan+ BarStruct.QuietZone;
				FontCharOut('0', PrtStruct.Rot);
			
				PrtStruct.y = by + BarStruct.p5 - BarStruct.p2+ BarStruct.QuietZone;
				Prt_strcpy(bar_data_buf,&HriData[0],6);
				FontStrOut(bar_data_buf);
				
				fontw_ptr = FontStruct.PtrWidth1 + (HriData[6]-FontStruct.FirstCode1);
				wbits = *(fontw_ptr) * StrStruct.Magx;
			
				PrtStruct.y = by - wbits - BarStruct.HriGapEan+ BarStruct.QuietZone;
				FontCharOut(HriData[6], PrtStruct.Rot);
				break;
		}
		FontLoadFont1(id1_backup);
		StrStruct.InterChSp = binterch_sp;
		StrStruct.Magx = magx_backup;
		StrStruct.Magy = magy_backup;
	} else {
		BarHri(HriData, BarStruct.p5);
	}
	PrtStruct.x = bx;
	PrtStruct.y = by;
}

void BarHriUpceA(HUGEDATA char *HriData, HUGEDATA char *HriData2) 
{
	short	bx,by;
	HUGEDATA INT8U *fontw_ptr;
	char wbits;
	char	binterch_sp;
	char bar_data_buf[10];
	INT8U id1_backup;
	char magx_backup, magy_backup;

	bx = PrtStruct.x;
	by = PrtStruct.y;
	StrStruct.StrHeight = BarStruct.CharHeight;
	StrStruct.StrWidth = BarStruct.p2 - BarStruct.p1;

	if(BarStruct.GuardType != NO_GUARD) {
		binterch_sp = StrStruct.InterChSp;
		id1_backup = StrStruct.Id1;
		magx_backup = StrStruct.Magx;
		magy_backup = StrStruct.Magy;
		FontLoadOcrB(0,0); ////////
		switch (PrtStruct.Rot) {
			case DEG_0:
				fontw_ptr = FontStruct.PtrWidth1 + ('0'-FontStruct.FirstCode1);
				wbits = *(fontw_ptr) * StrStruct.Magx;

				PrtStruct.x = bx - wbits - BarStruct.HriGapEan+ BarStruct.QuietZone;
				PrtStruct.y = by + BarStruct.FullHeight - BarStruct.CharHeight;
				FontCharOut('0', PrtStruct.Rot);
				
				PrtStruct.x = bx + BarStruct.p1+ BarStruct.QuietZone;
				Prt_strcpy(bar_data_buf,&HriData[0],6);
				FontStrOut(bar_data_buf);
		
				PrtStruct.x = bx + BarStruct.p3 + BarStruct.HriGapEan+ BarStruct.QuietZone;
				FontCharOut(HriData[6], PrtStruct.Rot);
				//sup
				PrtStruct.y = by;
				StrStruct.StrWidth = BarStruct.x2 - BarStruct.x1;
				PrtStruct.x = bx + BarStruct.x1+ BarStruct.QuietZone;
				FontStrOut(HriData2);
 				break;
			case DEG_180:
				PrtStruct.x = bx + BarStruct.x2 + BarStruct.HriGapEan+ BarStruct.QuietZone;
				PrtStruct.y = by;
				FontCharOut('0', PrtStruct.Rot);
			
				PrtStruct.x = bx + BarStruct.x2 - BarStruct.p2+ BarStruct.QuietZone;	 
				Prt_strcpy(bar_data_buf,&HriData[0],6);
				FontStrOut(bar_data_buf);

				fontw_ptr = FontStruct.PtrWidth1 + (HriData[6]-FontStruct.FirstCode1);
				wbits = *(fontw_ptr) * StrStruct.Magx;
			
				PrtStruct.x = bx + BarStruct.x2 - BarStruct.p3 - wbits - BarStruct.HriGapEan+ BarStruct.QuietZone;
				FontCharOut(HriData[6], PrtStruct.Rot);
				//sup
	 			PrtStruct.y = by + BarStruct.FullHeight - BarStruct.CharHeight;
				StrStruct.StrWidth = BarStruct.x2 - BarStruct.x1;
				PrtStruct.x = bx+ BarStruct.QuietZone;
				FontStrOut(HriData2);
				break;
			case DEG_90:
				fontw_ptr = FontStruct.PtrWidth1 + ('0'-FontStruct.FirstCode1);
				wbits = *(fontw_ptr) * StrStruct.Magx;

				PrtStruct.x = bx;
				PrtStruct.y = by - wbits - BarStruct.HriGapEan+ BarStruct.QuietZone;
				FontCharOut('0', PrtStruct.Rot);
				
				PrtStruct.y = by + BarStruct.p1+ BarStruct.QuietZone;
				Prt_strcpy(bar_data_buf,&HriData[0],6);
				FontStrOut(bar_data_buf);
				
				PrtStruct.y = by + BarStruct.p3 +BarStruct.HriGapEan+ BarStruct.QuietZone;
				FontCharOut(HriData[6], PrtStruct.Rot);
				//sup
				PrtStruct.x = bx + BarStruct.FullHeight - BarStruct.CharHeight;
				StrStruct.StrWidth = BarStruct.x2 - BarStruct.x1;
				PrtStruct.y = by + BarStruct.x1+ BarStruct.QuietZone;
				FontStrOut(HriData2);
				break;
			case DEG_270:
				PrtStruct.x = bx + BarStruct.FullHeight - BarStruct.CharHeight + 1;
				PrtStruct.y = by + BarStruct.x2 + BarStruct.HriGapEan+ BarStruct.QuietZone;
				FontCharOut('0', PrtStruct.Rot);
				
				PrtStruct.y = by + BarStruct.x2 - BarStruct.p2+ BarStruct.QuietZone;
				Prt_strcpy(bar_data_buf,&HriData[0],6);
				FontStrOut(bar_data_buf);
		
				fontw_ptr = FontStruct.PtrWidth1 + (HriData[6]-FontStruct.FirstCode1);
				wbits = *(fontw_ptr) * StrStruct.Magx;
				
				PrtStruct.y = by + BarStruct.x2 - BarStruct.p3 - wbits - BarStruct.HriGapEan+ BarStruct.QuietZone;
				FontCharOut(HriData[6], PrtStruct.Rot);
				//sup
				PrtStruct.x = bx;
				PrtStruct.y = by+ BarStruct.QuietZone;
 				StrStruct.StrWidth = BarStruct.x2 - BarStruct.x1;
				FontStrOut(HriData2);
				break;
		}
		FontLoadFont1(id1_backup);
		StrStruct.InterChSp = binterch_sp;
		StrStruct.Magx = magx_backup;
		StrStruct.Magy = magy_backup;
	} else {
		Prt_strsum(HriData,HriData2);
		BarHri(HriData, BarStruct.x2);
	}
	PrtStruct.x = bx;
	PrtStruct.y = by;
}
#endif//#ifdef USE_UPCE0_UPCE1
				
