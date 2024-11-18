/*****************************************************************************
|*
|*  Copyright		:	(c) 2011 CAS
|*  Filename		:	cl5kj_interpreter.c
|*  Version			:	0.1
|*  Programmer(s)	:	Kang kiho (KKH)
|*  Created			:	2011/04/13
|*  Description		:	cl5000j protocol interpreter
|*
****************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "globals.h"
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
#include "main.h"
#include "cal.h"
#include "prt_cmdtest.h"
#include "programmode.h"
//#include "file.h"
#include "rtc.h"
#include "debug.h"
#include "common.h"
#include "mode_commun.h"
#include "plu_manager.h"
#include "plu_device.h"
#include "initial.h"
#include "flash_app.h"
#include "comm_app.h"
#include "caption_app.h"
#include "menu.h"
#include "prt_font.h"
#include "dsp_app.h"
#include "commbuf.h"
#include "ethernet.h"
#include "ethernet_comm.h"
#include "mode_error.h"
#include "global_set.h"
#include "discount.h"
#include "mode_sale.h"
#include "network_command.h"
#include "origin.h"
#include "global_set.h"
#include "plu_edit.h"
#include "department.h"
#include "tare_file.h"
#include "tax.h"
#include "pcs_unit.h"
#include "barcode_file.h"
#include "workplace.h"
#include "nutrition_fact.h"
#include "traceability.h"
#include "mode_calibration.h"
#include "sale_report.h"
#include "customer_app.h"
#include "shopname.h"
#include "clerk_app.h"
#include "key_typedef.h"
#include "currency.h"
#include "key_function.h"
#include "mode_sale.h"
#include "prt_app.h"
#include "ingredient.h"
#include "initial_glb.h"
#include "prt_advertise.h"
#include "network_set.h"
#include "cl5kj_interpreter.h"
#include "menu_code_text1.h"
#include "flash_app.h"
#include "scale_config.h"

#ifdef USE_CHN_CART_SCALE
#include "traceability_chn.h"
#endif

#ifdef USE_PROTOCOL_CL5000JR

//variable
#define MAX_SIZE_CommMsgBuf	1500
char CommMsgBuf[MAX_SIZE_CommMsgBuf+32];

INT8U readSavedData(HUGEDATA COMM_BUF *CBuf, INT8U *buffer, INT16U *sLength);
INT8U writeReceivedData(HUGEDATA COMM_BUF *CBuf, INT8U *buffer, INT16U *sLength);
BOOLEAN findNextPacketHeader(HUGEDATA COMM_BUF *CBuf, INT8U *data, INT16U length);
#ifdef USE_CL5J_REALTIME_TRNASFER
INT8U process_response_good(COMM_BUF *CBuf, INT8U *buffer, INT16U *sLength);
#endif
ROMDATA INT8U maxDate[6]={99, 12, 31, 24, 60, 60};

/*
INT8U commData_Checksum(INT8U *str, INT16U plen)
{
	INT16U i;
	INT8U ck;

	ck=0;
	for(i=0; i<plen; i++) ck+=*(str++);

	return ck;
}
*/
/**
* @brief	패킷 구문 분석 함수
* @param	*CBuf    통신 데이터 구조체
* @param	trans    전송 여부
* @return  void
*/
extern INT16S CheckRxBufCountInt(HUGEDATA COMM_BUF *CBuf);
extern void GetDataRxBuf(HUGEDATA COMM_BUF *CBuf, HUGEDATA char *p, INT16U count);
extern INT8U commun_write_plu_data(HUGEDATA COMM_BUF *CBuf,INT8U *str,INT16S leng); //SG071204
extern INT8U commun_delete_plu_data(HUGEDATA COMM_BUF *CBuf,INT8U *str, INT16S leng); //SG080111
extern void commun_send_plu_data(HUGEDATA COMM_BUF *CBuf,INT32U nth); //SG080107
extern INT32U ing_calc_addr_by_key(INT16U ingid);
extern INT8U sendPluData(HUGEDATA INT8U *buffer, INT16U *sLength, INT32U nth, INT8U opCode);
extern INT16U get_new_labeladdress(INT16U id,INT8U chk);
extern INT16U get_new_bitmapaddress(INT16U id,INT8U chk);

typedef struct{
	INT32U Width;		// unit : mm
	INT32U Height;		// unit : mm
	INT32U FormSize;
	INT8U reserved[4];
	char  Name[16];
} CL5KJ_FORM_HEAD_STRUCT;

INT8U labelbmp_save(INT16U recv_id, INT16U offset, INT8U *str, INT16S leng, INT8U opCode)
{
	INT32U addr,saddr,dw;
	INT16U recv_w,recv_h,recv_sz,data_offset;
	INT16U nth_addr;
	CL5KJ_FORM_HEAD_STRUCT *cl5kjFHStruct;
	INT16U *recv_wp,*recv_hp;

	if (opCode == DATA_LABEL_CL55)
	{
		data_offset = 32;
	}
	else//DATA_BITMAP
	{
		saddr=FLASH_BITMAP_AREA;
		data_offset = 4;
	}
	saddr += DFLASH_BASE;

	nth_addr=999;
	if (opCode == DATA_LABEL_CL55)
	{
		nth_addr= get_new_labeladdress(recv_id,0);
	}
	else
	{
		nth_addr= get_new_bitmapaddress(recv_id,0);
	}

	if (nth_addr==999) return ERROR_DATA_FULL;//memory full

	if (opCode == DATA_LABEL_CL55)
	{
		addr = get_addr_from_label_idx(nth_addr, LABEL_AREA_IMAGE);
	}
	else
	{
		addr  = (INT32U)nth_addr;
		addr *= BITMAP_MAX_SIZE;
		addr += saddr + BITMAP_IMAGE;
	}

	if (offset == 0)
	{
		Flash_swrite(addr,(HUGEDATA INT8U *)&str[data_offset],leng-data_offset);
		addr  = saddr;
		if (opCode == DATA_LABEL_CL55)
		{
			struct LABEL_HEADER_INFO info;

			memset(&info, 0, sizeof(struct LABEL_HEADER_INFO));

			cl5kjFHStruct = (CL5KJ_FORM_HEAD_STRUCT *)&str[0];
			recv_w   =(INT16U)cl5kjFHStruct->Width;
			recv_h   =(INT16U)cl5kjFHStruct->Height;
			recv_sz  =(INT16U)cl5kjFHStruct->FormSize;

			addr = get_addr_from_label_idx(nth_addr, LABEL_AREA_HEADER);

			info.label_id = recv_id;
			info.img_w = recv_w;
			info.img_h = recv_h;
			strncpy(info.img_name, cl5kjFHStruct->Name, LABEL_HEADER_IMG_NAME_SIZE);
			info.img_size = recv_sz;

			save_label_header(addr, &info, L_H_ALL_BIT);
		}
		else
		{
			recv_wp  = (INT16U *)str;
			recv_hp  = (INT16U *)(str + 2);

			//dw = recv_w>>3;
			//if (recv_w%8) dw++;
			//recv_sz  =(INT16U)(dw*recv_h);

			addr += nth_addr*BITMAP_INFO_SIZE;
			Flash_wwrite(addr,recv_id);
			addr+=2;
			Flash_wwrite(addr,*recv_wp);
			addr+=2;
			Flash_wwrite(addr,*recv_hp);
		}
	}
	else
	{
		Flash_swrite(addr+offset-data_offset,(HUGEDATA INT8U *)&str[0],leng);
	}

	return 0;
}

void CL5000JRCommInterpreter(HUGEDATA COMM_BUF *CBuf)
{
	PACKET_HEADER *header;
	PACKET_TAIL *tail;
	INT16U length;
	INT8U  checksum;
	INT16U i;
	INT8U errNo;
	INT16U rxSize;
	INT16U sendLength;
	INT16U check_end_pos;
	INT8U opCode;
	INT16U fullLength;

	sendLength = 0;

	rxSize = CheckRxBufCountInt(CBuf);//GetRxBufSize(CBuf);
	GetDataRxBuf(CBuf, CommMsgBuf, rxSize);

	if ((rxSize) < sizeof(PACKET_HEADER))
	{
		return;
	}

	header = (PACKET_HEADER *)CommMsgBuf;
	opCode = header->opCode[1];

	fullLength = header->length;
	length = fullLength;

	if (header->opCode[1] == DATA_PLU)
	{
		if (fullLength > PLU_STORAGE_SIZE + DIRECT_INGREDIENT_BLOCK_NUM_CL5KJ_BLOCK_SIZE1 + 1)//PLU_HOST_STORAGE_SIZE + PLU_STORAGE_SIZE * 3 + 1) in case of CL5KJ
		{
			length = PLU_STORAGE_SIZE + DIRECT_INGREDIENT_BLOCK_NUM_CL5KJ_BLOCK_SIZE1 + 1;//PLU_HOST_STORAGE_SIZE + PLU_STORAGE_SIZE * 3 + 1;
		}
	}
	else if ( header->opCode[1] == DATA_PLU2)
	{
#if defined USE_DMSG_1050_BYTE
		if (fullLength >DIRECT_INGREDIENT_BLOCK_NUM_CL5KJ_BLOCK_SIZE1 + 4)//PLU_STORAGE_SIZE * 3 + sizeof(INT32U))
		{
			length = fullLength - DIRECT_INGREDIENT_BLOCK_NUM_CL5KJ_BLOCK_SIZE1 ;//PLU_STORAGE_SIZE * 3 ;
		}
#elif defined USE_DMSG_2100_BYTE
		switch(header->address)
		{
			case 0:
				if (fullLength >DIRECT_INGREDIENT_BLOCK_NUM_CL5KJ_BLOCK_SIZE1 + 4)//PLU_STORAGE_SIZE * 3 + sizeof(INT32U))
				{
					length = fullLength - DIRECT_INGREDIENT_BLOCK_NUM_CL5KJ_BLOCK_SIZE1 ;//PLU_STORAGE_SIZE * 3 ;
				}
				if (fullLength > DIRECT_INGREDIENT_BLOCK_NUM_CL5KJ_BLOCK_SIZE1 + DIRECT_INGREDIENT_BLOCK_NUM_CL5KJ_BLOCK_SIZE2 +4)
				{
					length = DIRECT_INGREDIENT_BLOCK_NUM_CL5KJ_BLOCK_SIZE2+5 ;//PLU_STORAGE_SIZE * 4 +  sizeof(INT32U) +1 (null);
				}
				break;
			case 1:
				if (fullLength >DIRECT_INGREDIENT_BLOCK_NUM_CL5KJ_BLOCK_SIZE1 + DIRECT_INGREDIENT_BLOCK_NUM_CL5KJ_BLOCK_SIZE2 + 4)//PLU_STORAGE_SIZE * 7 + sizeof(INT32U))
				{
					length = fullLength - (DIRECT_INGREDIENT_BLOCK_NUM_CL5KJ_BLOCK_SIZE1+DIRECT_INGREDIENT_BLOCK_NUM_CL5KJ_BLOCK_SIZE2) ;//PLU_STORAGE_SIZE * 7 ;
				}
				if (fullLength > DIRECT_INGREDIENT_BLOCK_NUM_CL5KJ_BLOCK_SIZE1 + 2*DIRECT_INGREDIENT_BLOCK_NUM_CL5KJ_BLOCK_SIZE2 +4)
				{
					length = DIRECT_INGREDIENT_BLOCK_NUM_CL5KJ_BLOCK_SIZE2+5 ;//PLU_STORAGE_SIZE * 4 +  sizeof(INT32U) +1 (null);
				}
				break;
			case 2:
				if (fullLength >DIRECT_INGREDIENT_BLOCK_NUM_CL5KJ_BLOCK_SIZE1 + 2*DIRECT_INGREDIENT_BLOCK_NUM_CL5KJ_BLOCK_SIZE2 + 4)//PLU_STORAGE_SIZE * 11 + sizeof(INT32U))
				{
					length = fullLength - (DIRECT_INGREDIENT_BLOCK_NUM_CL5KJ_BLOCK_SIZE1+2*DIRECT_INGREDIENT_BLOCK_NUM_CL5KJ_BLOCK_SIZE2) ;//PLU_STORAGE_SIZE * 11 ;
				}
				if (fullLength > 2*DIRECT_INGREDIENT_BLOCK_NUM_CL5KJ_BLOCK_SIZE1 + 2*DIRECT_INGREDIENT_BLOCK_NUM_CL5KJ_BLOCK_SIZE2 +4)
				{
					length = DIRECT_INGREDIENT_BLOCK_NUM_CL5KJ_BLOCK_SIZE1+5 ;//PLU_STORAGE_SIZE * 3 +  sizeof(INT32U) +1 (null);
				}
				break;
		}
#endif
	}
#ifdef USE_RUSSIA_LOGISTICS_PLU
	else if (header->opCode[1] == DATA_LOGISTICS_PLU)
	{
		if (fullLength > LOGISTIC_PLU_STORAGE_SIZE)//PLU_HOST_STORAGE_SIZE + PLU_STORAGE_SIZE * 3 + 1) in case of CL5KJ
		{
			length = LOGISTIC_PLU_STORAGE_SIZE;
		}
	}
	else if ( header->opCode[1] == DATA_LOGISTICS_INGREDIENT)
	{
		if (fullLength > (header->address) * DIRECT_INGREDIENT_BLOCK_NUM_CL5KJ_BLOCK_SIZE2 + 4)
		{
			length = fullLength - (header->address) * DIRECT_INGREDIENT_BLOCK_NUM_CL5KJ_BLOCK_SIZE2 ;
		}
		if (fullLength > (header->address+1) * DIRECT_INGREDIENT_BLOCK_NUM_CL5KJ_BLOCK_SIZE2 + 4)
		{
			length = DIRECT_INGREDIENT_BLOCK_NUM_CL5KJ_BLOCK_SIZE2+5 ;
		}
	}
#endif
/*
	length = header->length;
	if (header->opCode[1] == DATA_PLU)
	{
		if (length > (PLU_HOST_STORAGE_SIZE + DMSG_HOST_STORAGE_SIZE))
		{
			errNo = ERROR_DATA;
			goto SEND_ERR;
		}
	}
*/
	if (header->colon != ':')							// 패킷 header 확인
	{
		goto ERR_HANDLER;
	}
	else if (header->delilimeter != ',')
	{
		goto ERR_HANDLER;
	}
	else if (('W' != header->opCode[0]) && ('R' != header->opCode[0]) && ('G' != header->opCode[0]) && ('N' != header->opCode[0]))
	{
		goto ERR_HANDLER;
	}

	if (length > COM_MAX_RX_BUF_HOST_SIZE)
	{
		goto ERR_HANDLER_CHECKSUM;                 //poland err
	}

	if (rxSize < length + sizeof(PACKET_HEADER) + sizeof(PACKET_TAIL))
	{
		return;
	}

	tail = (PACKET_TAIL *)&CommMsgBuf[length + sizeof(PACKET_HEADER)];

	if (tail->colon != ':')							// 패킷 tail 확인
	{
		goto ERR_HANDLER_CHECKSUM;
	}
	else if (tail->cr != 0x0D)//CR)
	{
		goto ERR_HANDLER_CHECKSUM;
	}

	if(header->opCode[1] != DATA_RTT) // RTT 응답의 경우 chcecksum 체크 안 함(현재 CL5000JR 코드에서 체크 안 함) 
	{
		checksum = 0;

		i = 2;
		check_end_pos = length + sizeof(PACKET_HEADER);//data 마지막 까지
		while(i < check_end_pos)
		{
			checksum += CommMsgBuf[i++];
		}

		checksum += ':';//tail ':' 까지 checksum 계산

		if (tail->checksum != checksum)
		{
			goto ERR_HANDLER_CHECKSUM;
		}
	}

	//TO DO : CommMsgBuf를 Input, Output 모두 사용하고 있음, 무결성 검사 필요함
	if (header->opCode[0] == 'W')
	{
		errNo = writeReceivedData(CBuf, (INT8U *)CommMsgBuf, &sendLength);
	}
	else if (header->opCode[0] == 'R')
	{
		errNo = readSavedData(CBuf, (INT8U *)CommMsgBuf, &sendLength);
	}
  #ifdef USE_CL5J_REALTIME_TRNASFER
	else if (header->opCode[0] == 'G')
	{
		errNo = process_response_good(CBuf, (INT8U *)CommMsgBuf, &sendLength);
	} 
	else if (header->opCode[0] == 'N')
	{
		errNo = ERROR_RECEIVE;
	}
  #endif
	else
	{
		errNo = ERROR_INTERPRETER;
	}

	if (errNo == 0 || errNo == LONG_EXE_ERASE_ALL_PLU)
	{
		WindRxBuf(CBuf, length + sizeof(PACKET_HEADER) + sizeof(PACKET_TAIL));
		PutDataTxBuf(CBuf, CommMsgBuf, sendLength);
		if (errNo == LONG_EXE_ERASE_ALL_PLU)
		{
			network_tx_proc();
			//plu_init(); need replacing
		}
		return;
	}

SEND_ERR:
	CommMsgBuf[0]='N';
	CommMsgBuf[1]=opCode;
	if (opCode != DATA_FLASH && opCode != DATA_EEPROM && opCode != DATA_RAM)
	{
		CommMsgBuf[2]=0;//cstruct.scaleAddr; need replacing
		CommMsgBuf[3]=0;
		CommMsgBuf[4]=0;
		CommMsgBuf[5]=0;
	}
	CommMsgBuf[6]=',';
	CommMsgBuf[7]=5;
	CommMsgBuf[8]=0;
	CommMsgBuf[9]=':';
//  기존에 받은 번호 그대로 반환하거나
//	해석기에서 modify 했다고 가정함
//	CommMsgBuf[10]=0;
//	CommMsgBuf[11]=0;
//	CommMsgBuf[12]=0;
//	CommMsgBuf[13]=0;
	CommMsgBuf[14]=errNo;
	CommMsgBuf[15]=':';

	checksum = 0;

	for(i=2; i<16; i++) checksum += CommMsgBuf[i];

	CommMsgBuf[16]=checksum;
	CommMsgBuf[17]=0x0D;//CR;

	FlushRxBuf(CBuf);
	PutDataTxBuf(CBuf, CommMsgBuf, 18);
	return;

ERR_HANDLER:
	if (TRUE == findNextPacketHeader(CBuf, (INT8U *)CommMsgBuf, rxSize))
	{
		return;
	}
	else
	{
ERR_HANDLER_CHECKSUM:
  #ifdef USE_CL5J_REALTIME_TRNASFER
		if(header->opCode[0] == 'G' || header->opCode[0] == 'N')
		{
			FlushRxBuf(CBuf);
			return;
		}
  #endif
		errNo = ERROR_RECEIVE;
		goto SEND_ERR;
	}
}

/**
*  @brief		패킷 데이터에 잘못된 데이터가 들어 왔을 때 다음 패킷의 헤더를 찾는다.
*  @param		*CBuf	통신 버퍼 구조체
*  @param		*data	패킷 데이터
*  @param		length	들어온 패킷의 길이
*  @return		다음 헤더 찾기 성공 여부
*/
BOOLEAN findNextPacketHeader(COMM_BUF *CBuf, INT8U *data, INT16U length)
{
	INT16U i;
	PACKET_HEADER *header;

	for (i = 0; i <= length - sizeof(PACKET_HEADER); i++)
	{
		header = (PACKET_HEADER *)(data + i);

		if (':' == header->colon) {
			if (',' == header->delilimeter) {
				if (('W' == header->opCode[0]) || ('R' == header->opCode[0])) {
					WindRxBuf(CBuf, i);
					return TRUE;
				}
			}
		}
	}
	WindRxBuf(CBuf, i);
	//return FALSE; //cause send many error protocol
	return TRUE;
}
/*
INT32U ing_calc_addr_by_key(INT16U ingid)
{
	INT32U pnt;
	pnt =ingid;
	if (ingid>=MAX_INDIRECT_INGREDIENT_small)
	{
		pnt-=MAX_INDIRECT_INGREDIENT_small;
		pnt*=INGREDIENT_STRUCT_SIZE;
		pnt+=FLASH_ING_EXT_AREA_PNT;
	}
	else
	{
		pnt*=INGREDIENT_STRUCT_SIZE;
		pnt+=FLASH_ING_NOR_AREA_PNT;
	}
	return pnt;
}
*/

// CL5000  : 1-B, 2-P, 3-H, 4-S,Ext_S(100key), 5-D, 6-B(CL5200)
// CL5000J : 1-B, 2-P, 3-H, 4-S, 5-Ext_S(100key), 6-D
					//CL5KJ		//CL5K
INT16U keyTypeConvTable_CL5KJ[] = {	0,		// 0 : NONE
					1,		// 1 : B-type
					2,		// 2 : P,R-type
					3,		// 3 : H-type
					4,		// 4 : S-type
					6,		// 5 : D-type
					1		// 6 : CL5200 B-type

				      //5,		// 4 : S-type(ext_self100key)
};


INT8U writeReceivedData(COMM_BUF *CBuf, INT8U *buffer, INT16U *sLength)
{
	INT32U number,rp;
	//INT32U address;
	//INT32U maddr, maddr_max, maddr_max_limit;
	INT8U checksum;
#ifdef USE_CHN_CART_SCALE
	INT8U *dataPtr;
#endif
	INT16U i;
	PLU_INF *plu;
    char   temp[512];
	//INT32U int32uBuf;
	lpDATA_HEADER dataHeader;

	INT8U opCode;
	INT8U *data;
	INT16U length;
	PACKET_HEADER *header;

	//INT8U ptype;
    //INT8U pleng, option;
	INT8U errNo;

	INT8U long_exe_flag;
    //INT8U msgLenFlag;

	//INT16U id;
	INT16U sz;
	INT32U p;
	BARCODE_STRUCT  barcode;
	SCROLL_MESSAGE_STRUCT	scroll;
	STORE_NAME_STRUCT store;
	MENU_TYPE1_STRUCT menu_type1_str;
	RTC_STRUCT	RTCStruct;
	RTC_DATE	date;
	RTC_TIME	time;

	INT32U pluno;
	INT32U deptno;
#ifdef USE_DIRECT_INGREDIENT
	INT8U ing_ret;
	INT16U iSize;
	INT32U dmsg_addr;
	INT16U idx;
	INT32U key;
	INT32U fl_pnt;
	INT8U being;
	INT16U dmsgNo[DIRECT_INGREDIENT_BLOCK_NUM];
	INT32U ingredient_address;
	INT16U total_block2;
#endif
	INT32U  addr;
#ifdef USE_CHN_CART_SCALE
	//TRACE_STRUCT vege_trace_new;
	//TRACE_STRUCT vege_trace_old;
	TRACE_STRUCT trace_new[4];
	TRACE_STRUCT trace_old[4];
	TRACE_STRUCT trace_cur[4];
	INT32S remainWeight_new[4];
	INT32S remainWeight_old[4];
	INT32S remainWeight_cur[4];
	MEAT_COMM_TRACE_STRUCT *meatTraceTmp;// from PC
	VEGE_COMM_TRACE_STRUCT *vegeTraceTmp;// from PC
	INT16U plu_save_no;
	INT8U name[MAX_ARTNAME + 1];
//	INT8U trace_code[TRACE_CODE_SIZE + 1];
	INT16U lot_no;
	INT8U flag;
	INT8U vege_trace_head;
	INT32U offset;
	INT32U retVal;
	INT16U cnt;
	INT16U j;
	VEGE_STOCK_INF *stock;
#endif

	p=0;

	long_exe_flag = 0;

	header = (PACKET_HEADER *)buffer;
	opCode = header->opCode[1];
	data = &buffer[sizeof(PACKET_HEADER)];
	length = header->length;
	//address = header->address;

#ifdef USE_CHN_CART_SCALE
	dataPtr = data + sizeof(INT32U);
#endif
	memcpy(&number, data, sizeof(INT32U));

	switch(opCode)
	{
		case DATA_PLU://not use (slow)
			plu = (PLU_INF *)data;
			number = plu->pluno + plu->deptNo * 1000000L;
			if (plu->pluno > PLU_NUM_MAX_RANGE || plu->pluno == 0)
			{
				if (plu->deptNo == 0 && plu->pluno == 0 && length == 6)
				{
					long_exe_flag = LONG_EXE_ERASE_ALL_PLU;
					break;
				}
				else
				{
					*((INT32U _PACKED_DEF *)data) = number;
					return ERROR_NUMBER_OVER;
				}
			}

			if (plu->deptNo > 99 || plu->deptNo == 0)
			{
				*((INT32U _PACKED_DEF *)data) = number;
				return ERROR_NUMBER_OVER;
			}

			if (length == 6)//plu->deptNo | plu->no
			{
				//plu_erase(plu->deptNo, plu->no);
				commun_delete_plu_data(CBuf, data, length);
				break;
			}
			else if (length < PLU_HOST_STORAGE_SIZE || length > (PLU_HOST_STORAGE_SIZE + DMSG_HOST_STORAGE_SIZE))//except dmsg number
			{
				*((INT32U _PACKED_DEF *)data) = number;//error 처리시 data number 확인을 위해 input buffer 수정
				return ERROR_DATA;
			}
			//msgLenFlag = 0;
			//ptype = plu_save(plu, &data[PLU_HOST_STORAGE_SIZE], length - PLU_HOST_STORAGE_SIZE, 1, msgLenFlag);
            //ptype = 0;//check cl5000j code
            //if (ptype)
			//{
			//	*((INT32U _PACKED_DEF *)data) = number;
			//	if (ptype == 0xfe) return ERROR_DMSG_FULL;
			//	return ERROR_DATA_FULL;
			//}
    		errNo = commun_write_plu_data(CBuf, data, length);
			if(errNo) return errNo;
			break;
#ifdef USE_DIRECT_INGREDIENT
		case DATA_PLU2:
			ingredient_address = header->address;
		  	deptno = number / 1000000;
			pluno = number %1000000;
			//number = plu->pluno + plu->deptNo * 1000000L;
			if (pluno > PLU_NUM_MAX_RANGE || pluno == 0)
			{
				*((INT32U _PACKED_DEF *)data) = number;
				return ERROR_NUMBER_OVER;
			}

			if (deptno > 99 || deptno == 0)
			{
				*((INT32U _PACKED_DEF *)data) = number;
				return ERROR_NUMBER_OVER;
			}

			if (length < (DIRECT_INGREDIENT_BLOCK_NUM_CL5KJ_BLOCK_SIZE1+1+4) || length > (2*DIRECT_INGREDIENT_BLOCK_NUM_CL5KJ_BLOCK_SIZE1+2*DIRECT_INGREDIENT_BLOCK_NUM_CL5KJ_BLOCK_SIZE2+1+4))
			{
				*((INT32U _PACKED_DEF *)data) = number;//error 처리시 data number 확인을 위해 input buffer 수정
				return ERROR_DATA;
			}
#if defined USE_DMSG_1050_BYTE
			length -= DIRECT_INGREDIENT_BLOCK_NUM_CL5KJ_BLOCK_SIZE1;
#elif defined USE_DMSG_2100_BYTE
			switch(ingredient_address)
			{
				case 0:
					length -= DIRECT_INGREDIENT_BLOCK_NUM_CL5KJ_BLOCK_SIZE1;
					break;
				case 1:
					length -= DIRECT_INGREDIENT_BLOCK_NUM_CL5KJ_BLOCK_SIZE1+DIRECT_INGREDIENT_BLOCK_NUM_CL5KJ_BLOCK_SIZE2;
					break;
				case 2:
					length -= DIRECT_INGREDIENT_BLOCK_NUM_CL5KJ_BLOCK_SIZE1+2*DIRECT_INGREDIENT_BLOCK_NUM_CL5KJ_BLOCK_SIZE2;
					break;
			}
#endif
			if (length > 0)
			{
				total_block2 = (length - 1) / (DEFAULT_INGREDIENT_TEXT_SIZE - 2);
				total_block2++;
			}
			else
			{
				total_block2 = 0;
			}
///////////////////////////////////////////////////////////////////////////////////////
			being = plu_table_search((INT8U)deptno,pluno,(INT16U *)&idx,0);
			if (!being)
			{
				return ERROR_DATA;
			}
			else
			{
				key = (INT32U)idx;
			}

			fl_pnt = plu_calc_addr_by_key(key-1);
			fl_pnt+=3;
#ifdef USE_RUSSIA_LOGISTICS_PLU
			dmsg_addr = fl_pnt + LOGISTIC_PLU_STORAGE_SIZE;
#else
			dmsg_addr = fl_pnt + PLU_STORAGE_SIZE;
#endif
			iSize = PLU_STORAGE_SIZE;

			Flash_sread(dmsg_addr, (HUGEDATA INT8U *)&dmsgNo[0], 2*DIRECT_INGREDIENT_BLOCK_NUM);
#if defined USE_DMSG_1050_BYTE
			ing_ret = ingredient_text_write(&dmsgNo[0], &data[4], DIRECT_INGREDIENT_BLOCK_NUM_CL5KJ_BLOCK_SIZE1, DIRECT_INGREDIENT_BLOCK_NUM_CL5KJ_BLOCK_SIZE2, total_block2);
#elif defined USE_DMSG_2100_BYTE
			switch(ingredient_address)
			{
				case 0 :
					ing_ret = ingredient_text_write(&dmsgNo[0], &data[4], DIRECT_INGREDIENT_BLOCK_NUM_CL5KJ_BLOCK_SIZE1, DIRECT_INGREDIENT_BLOCK_NUM_CL5KJ_BLOCK_SIZE2, total_block2);
					break;
				case 1 :
					ing_ret = ingredient_text_write(&dmsgNo[0], &data[4], DIRECT_INGREDIENT_BLOCK_NUM_CL5KJ_BLOCK_SIZE1+DIRECT_INGREDIENT_BLOCK_NUM_CL5KJ_BLOCK_SIZE2, DIRECT_INGREDIENT_BLOCK_NUM_CL5KJ_BLOCK_SIZE2, total_block2);
					break;
				case 2 :
					ing_ret = ingredient_text_write(&dmsgNo[0], &data[4], DIRECT_INGREDIENT_BLOCK_NUM_CL5KJ_BLOCK_SIZE1+2*DIRECT_INGREDIENT_BLOCK_NUM_CL5KJ_BLOCK_SIZE2, DIRECT_INGREDIENT_BLOCK_NUM_CL5KJ_BLOCK_SIZE1, total_block2);
					break;
			}
#endif
//			ing_ret = ingredient_text_write(&dmsgNo[0], &data[4], DIRECT_INGREDIENT_BLOCK_NUM_CL5KJ_BLOCK_SIZE1, DIRECT_INGREDIENT_BLOCK_NUM_CL5KJ_BLOCK_SIZE2);
			Flash_swrite(dmsg_addr, (HUGEDATA INT8U *)&dmsgNo[0], 2*DIRECT_INGREDIENT_BLOCK_NUM);
			if (!ing_ret) return ERROR_DMSG_FULL;	//direct ingredient memory full

			if (network_status.service_flag&0x20)	// send plu
			{
				if (ethernet_list.status == 1)
				{
					commun_net_mask(0, (INT16U *)&key, 0xff, 1);
				}
			}
			plu_table_save((INT8U)deptno,pluno);
///////////////////////////////////////////////////////////////////////////////////////
			break;
#endif
#ifdef USE_RUSSIA_LOGISTICS_PLU
		case DATA_LOGISTICS_INGREDIENT:
			ingredient_address = header->address;
			deptno = number / 1000000;
			pluno = number %1000000;
			//number = plu->pluno + plu->deptNo * 1000000L;
			if (pluno > PLU_NUM_MAX_RANGE || pluno == 0)
			{
				*((INT32U _PACKED_DEF *)data) = number;
				return ERROR_NUMBER_OVER;
			}

			if (deptno > 99 || deptno == 0)
			{
				*((INT32U _PACKED_DEF *)data) = number;
				return ERROR_NUMBER_OVER;
			}

			if (length < (1+4) || length > (4*DIRECT_INGREDIENT_BLOCK_NUM_CL5KJ_BLOCK_SIZE2))
			{
				*((INT32U _PACKED_DEF *)data) = number;//error 처리시 data number 확인을 위해 input buffer 수정
				return ERROR_DATA;
			}
			length -= ingredient_address * DIRECT_INGREDIENT_BLOCK_NUM_CL5KJ_BLOCK_SIZE2;
			being = plu_table_search((INT8U)deptno,pluno,(INT16U *)&idx,0);
			if (!being)
			{
				return ERROR_DATA;
			}
			else
			{
				key = (INT32U)idx;
			}

			fl_pnt = plu_calc_addr_by_key(key-1);
			fl_pnt+=3;
			dmsg_addr = fl_pnt + LOGISTIC_PLU_STORAGE_SIZE;
			iSize = LOGISTIC_PLU_STORAGE_SIZE;

			Flash_sread(dmsg_addr, (HUGEDATA INT8U *)&dmsgNo[0], 2*DIRECT_INGREDIENT_BLOCK_NUM);

			ing_ret = ingredient_text_write(&dmsgNo[0], &data[4], ingredient_address * DIRECT_INGREDIENT_BLOCK_NUM_CL5KJ_BLOCK_SIZE2, DIRECT_INGREDIENT_BLOCK_NUM_CL5KJ_BLOCK_SIZE2, total_block2);

			Flash_swrite(dmsg_addr, (HUGEDATA INT8U *)&dmsgNo[0], 2*DIRECT_INGREDIENT_BLOCK_NUM);
			if (!ing_ret) return ERROR_DMSG_FULL;	//direct ingredient memory full

			if (network_status.service_flag&0x20)	// send plu
			{
				if (ethernet_list.status == 1)
				{
					commun_net_mask(0, (INT16U *)&key, 0xff, 1);
				}
			}

			plu_table_save((INT8U)deptno,pluno);
///////////////////////////////////////////////////////////////////////////////////////
			break;
#endif

        case DATA_SHOP:
			if (number > SHOP_QTY_MAX || number == 0) return ERROR_NUMBER_OVER;
#ifdef USE_RUSSIA_DEFAULT
			length += 70; // text[80] -> text[150]
#endif
			if (length == sizeof(INT32U))
			{
//				bar_erase(number);
				sz=SHOPNAME_STRUCT_SIZE;
				p=FLASH_SHOPNAME_AREA;
				//if (id>max_id) return 0x99;
				if(p)
				{
					memset(temp,0,sz);
//					memset(temp,0,128);
					//p+=DFLASH_BASE;
					rp=0;
					if(number)//id
					{
						rp = (number-1);//id
						rp*= sz;
					}
					p+=rp;
					//Flash_swrite(p,temp,128);
				}
				break;
			}
			else if (length != FLASH_SCALE_SHOP_HOST_SIZE + sizeof(INT32U))
			{
				return ERROR_DATA;
			}


			memset(&store,0,sizeof(store));
			//desc1[25]=0;
			//telep[19]=0;
			//descL[79]=0;

			store.code = (INT16U)number;

			//strcpy((char *)store.name,(char *)desc1);
			//strcpy((char *)store.text,(char *)descL);
			//strcpy((char *)store.tel_no,(char *)telep);
			memcpy(store.name,data+4,26);
			memcpy(store.tel_no,data+30,20);
#ifdef USE_RUSSIA_DEFAULT
			memcpy(store.text,data+50,150);
#else
			memcpy(store.text,data+50,80);
#endif

			memcpy(temp,&store,sizeof(store));
			sz   = sizeof(store);
			p   = number-1;
			p *= sizeof(store);
			p += FLASH_SHOPNAME_AREA;
			break;

		case DATA_BARCODE:
			if (number > BAR_QTY_MAX || number == 0) return ERROR_NUMBER_OVER;
			if (length == sizeof(INT32U))
			{
//				bar_erase(number);
				sz=BARCODE_STRUCT_SIZE;
				p=FLASH_BARCODE_FORMAT_AREA;
				//if (id>max_id) return 0x99;
				if(p)
				{
					memset(temp,0,32);
					//p+=DFLASH_BASE;
					rp=0;
					if(number)//id
					{
						rp = (number-1);//id
						rp*= sz;
					}
					p+=rp;
					//Flash_swrite(p,temp,2);

				}
				break;
			}
			else if (length != BAR_HOST_STORAGE_SIZE + sizeof(INT32U))
			{
				return ERROR_DATA;
			}

			//bar_save(number, dataPtr);
	        memset(&barcode,0,sizeof(barcode)); // Barcode Format
		    barcode.barcode_type= *((INT8U*)(data + sizeof(INT32U)));//barcode type
		    barcode.barcode_num = (INT16U)number;//barcode number
			memcpy(barcode.barcode_format,data+5,BARCODE_FORMAT_LEN);
		    sz=BARCODE_STRUCT_SIZE;
		    memcpy(temp,&barcode,sizeof(barcode));
		    p =(number-1)*sz;//id
		    p+=FLASH_BARCODE_FORMAT_AREA;
			break;

		case DATA_GROUP:
			if (number > MAX_GROUP || number == 0) return ERROR_NUMBER_OVER;
			if (length == sizeof(INT32U))
			{
				sz=GROUP_STRUCT_SIZE;
				p=FLASH_SUB_DEPARTMENT_AREA;
				if(p)
				{
					memset(temp,0,32);
					//p+=DFLASH_BASE;
					rp=0;
					if(number)//id
					{
						rp = (number-1);//id
						rp*= sz;
					}
					p+=rp;
					//Flash_swrite(p,temp,2);

				}
				break;
			}
			else if (length != GROUP_HOST_STORAGE_SIZE + sizeof(INT32U))
			{
				return ERROR_DATA;
			}
			menu_type1_str.code = (INT16U)number;
			memcpy(menu_type1_str.text, data+4, GROUP_STRUCT_SIZE-2);
			memcpy(&temp, &menu_type1_str, sizeof(menu_type1_str));

			sz = GROUP_STRUCT_SIZE;
			p =(number-1)*sz;
			p+= FLASH_SUB_DEPARTMENT_AREA;
			break;

		case DATA_SCMSG:
			if (number > SCMSG_QTY_MAX || number == 0) return ERROR_NUMBER_OVER;

			if (length == sizeof(INT32U))
			{
				sz=SCMSG_STORAGE_SIZE;
				p=FLASH_SCROLL_AREA;
				if(p)
				{
					memset(temp,0,32);
					rp=0;
					if(number)//id
					{
						rp = (number-1);//id
						rp*= sz;
					}
					p+=rp;
				}
				break;
			}
			else if (length != SCMSG_HOST_STORAGE_SIZE + sizeof(INT32U))
			{
				return ERROR_DATA;
			}

			//scmsg_save(number, dataPtr);
			memset(&scroll,0,sizeof(scroll));
			memcpy(&scroll.text,data+8,80);
			scroll.use = *((INT8U*)(data + 6));
			scroll.stype =1;
			scroll.week = 0xff;

			memcpy(temp,&scroll,sizeof(scroll));
			sz   = sizeof(scroll);
			p   = number-1;
			p *= sizeof(scroll);
			p += FLASH_SCROLL_AREA;
			//p+=DFLASH_BASE;
			break;

		case DATA_IMESSAGE:
			if (number > MAX_INGREDIENT_NO || number == 0) return ERROR_NUMBER_OVER;
			if (length == sizeof(INT32U))
			{
				sz=INGREDIENT_STRUCT_SIZE-2;
				p=FLASH_INGREDIENT_AREA;
				if(p)
				{
					memset(temp,0,400);
					rp=0;
					if(number)//id
					{
						rp = (number-1);//id
						rp*= sz;
					}
					p+=rp;
					//Flash_swrite(p,temp,2);
				}
				break;
			}
			else if (length != (INGREDIENT_STRUCT_SIZE-2) + sizeof(INT32U))
			{
				return ERROR_DATA;
			}

			memset(temp,0,INGREDIENT_STRUCT_SIZE);
			memcpy(temp,&number,2);	// Group
		    memcpy(&temp[2],data+4,400);

			sz = INGREDIENT_STRUCT_SIZE;
		    p =(number-1)*sz;
		    p+= FLASH_INGREDIENT_AREA;
		    break;

		case DATA_DATE:
			for(i = 0; i < 6; i++)
			{
				if(data[i] > maxDate[i]) return ERROR_NUMBER_OVER;
			}
			RTCStruct.year = hex2bcd(data[0]);
			RTCStruct.month = hex2bcd(data[1]);
			RTCStruct.date = hex2bcd(data[2]);
			RTCStruct.hour = hex2bcd(data[3]);
			RTCStruct.min = hex2bcd(data[4]);
			RTCStruct.sec = hex2bcd(data[5]);

			date.day = bcd2hex(RTCStruct.date); 						 /* set day */
			date.weekday = bcd2hex(RTCStruct.day);						/* set weekday */
			date.month = bcd2hex(RTCStruct.month); 						/* set month */
			date.year = bcd2hex(RTCStruct.year);							/* set year */
			date.century = 21;						/* set century */
			time.hours = bcd2hex(RTCStruct.hour);						/* set hours */
			time.minutes = bcd2hex(RTCStruct.min);						 /* set minutes */
			time.seconds = bcd2hex(RTCStruct.sec);						/* set seconds */
			time.milliseconds = 0;					/* set milliseconds */

			/*set time & date*/
			RTC_SetDate(date);						/* set date in RTC */
			RTC_SetTime(time);						/* set time in RTC registers */
			break;

		case DATA_KEY:
			dataHeader = (lpDATA_HEADER)data;
			if (dataHeader->No > 5 || dataHeader->No == 0) return ERROR_NUMBER_OVER;

			if (dataHeader->No != 5) {//KEY_TABLE_NUM_SELF
#ifdef _USE_UNDER_SELFKEY_
				if (dataHeader->Offset != (INT16U)keyTypeConvTable_CL5KJ[4]) return ERROR_NUMBER_OVER;	// self type
#else
				if (dataHeader->Offset != (INT16U)keyTypeConvTable_CL5KJ[Startup.scale_type]) return ERROR_NUMBER_OVER;// 1-B, 2-P, 3-H, 4-S, 5-D, 6-B(CL5200)
#endif
			}

			if (length > (144*4) + sizeof(INT32U)) return ERROR_DATA;

			addr = DFLASH_BASE + FLASH_KEY_SPEED_AREA;
			addr += (INT32U)(dataHeader->No - 1) * MAX_SPEEDKEY_SIZE;
			Flash_swrite(addr, data+4, (144*4));
			break;

		case DATA_LABEL_CL55:
		case DATA_BITMAP:
			dataHeader = (lpDATA_HEADER)data;
			if(length == 0)
			{
				label_load(status_scale.cur_labelid);
				break;
			}
			errNo = labelbmp_save(dataHeader->No, dataHeader->Offset, data+4, length-4, opCode);
			if (errNo != 0) return errNo;
			break;

/*
    case DATA_PARAMETER:
			switch(number)
			{
				default://MAC Download시 사용하고 있음
					//if (number >= 100 && number <= 999) {
					i = scale_parameter(number, 0, &ptype, &option, &pleng, P_CHECK);
					if (option & PARAM_USE_IN_COMM_MODE){
						if (i == FALSE) return ERROR_NUMBER_OVER;
						if (ptype == 'S') {
							if (length != (sizeof(INT32U) + pleng)) return ERROR_DATA;
							scale_parameter(number, (INT32U *)dataPtr, 0, 0, 0, P_WRITE);
						} else {
							if (length != 2 * sizeof(INT32U)) return ERROR_DATA;
							scale_parameter(number, (INT32U *)dataPtr, 0, 0, 0, P_WRITE);
						}
					} else {
						return ERROR_NUMBER_OVER;
					}
					break;
			}
			break;
*/

#ifdef USE_CHN_CART_SCALE
		case DATA_MEAT_TRACE:
			if (number > MAX_SCALE || number == 0) return ERROR_NUMBER_OVER; //check scale id range 4 digit number
			if (length > (sizeof(MEAT_COMM_TRACE_STRUCT) + sizeof(INT32U)))//124 byte
			{
				return ERROR_DATA;
			}

			meatTraceTmp = (MEAT_COMM_TRACE_STRUCT *)dataPtr;

			//1. temp_old buf load trace code from memory.
			for (i = 0; i < 4; i++)
			{
				meat_trace_load(&trace_old[i], i);
				remainWeight_old[i] = meat_trace_remainWeight_load(i);
			}

			//2. temp_new buf save trace code from PC
			for (i = 0; i < 4; i++)
			{
				trace_new[i].dt[0] = meatTraceTmp->unit[i].dt[0];
				trace_new[i].dt[1] = meatTraceTmp->unit[i].dt[1];
				trace_new[i].dt[2] = meatTraceTmp->unit[i].dt[2];
				trace_new[i].dt[3] = meatTraceTmp->unit[i].dt[3];
				trace_new[i].dt[4] = meatTraceTmp->unit[i].dt[4];
				trace_new[i].dt[5] = meatTraceTmp->unit[i].dt[5];
				memcpy(trace_new[i].code, meatTraceTmp->unit[i].code, TRACE_CODE_SIZE);
				trace_new[i].inputWeight = meatTraceTmp->unit[i].inputWeight;
				remainWeight_new[i] = meatTraceTmp->unit[i].inputWeight;
			}

			//3. compare trace code
			cnt = 0;
			for (i = 0; i < 4; i++)//new
			{
				flag = 0;//NOT_EQUAL;
				for (j = 0; j < 4; j++)//old
				{
					if (trace_compare_data(trace_new[i].code, trace_old[j].code, TRACE_CODE_SIZE)) //return : NOT_EQUAL=0, EQUAL=1
					{
						flag = 1;//EQUAL;
						break;
					}
				}
				if (flag == 0)
				{
					remainWeight_cur[cnt] = remainWeight_new[i];
					memcpy(&trace_cur[cnt], &trace_new[i], sizeof(TRACE_STRUCT));
					cnt++;
				}
			}

			//4. trace code sorting based on datetime
			//make sorting table


			//5. save new trace code
			for (i = 0; i < cnt; i++)
			{
				//offset = MEAT_REMAIN_WEIGHT_SIZE * i;
				//addr = NVRAM_BASE + NVRAM_MEAT_REMAIN_WEIGHT + offset;
				//memcpy((INT8U *)addr, (INT8U *)&remainWeight_cur[i], MEAT_REMAIN_WEIGHT_SIZE);
				meat_trace_save(&trace_cur[i], i);
				meat_trace_remainWeight_save(i, remainWeight_cur[i]);
			}
			for (i = cnt; i < 4; i++)
			{
				meat_trace_save(&trace_old[i-cnt], i);
				meat_trace_remainWeight_save(i, remainWeight_old[i-cnt]);
			}
			break;
		case DATA_VEGE_TRACE:
			if (number > MAX_SCALE || number == 0) return ERROR_NUMBER_OVER;
			if (length > (sizeof(VEGE_COMM_TRACE_STRUCT) + sizeof(INT32U)))
			{
				return ERROR_DATA;
			}
			vegeTraceTmp = (VEGE_COMM_TRACE_STRUCT *)dataPtr;

			pluno = vegeTraceTmp->commodityCode;

			vege_trace_head = get_nvram_bparam(NVRAM_VEGE_TRACE_HEAD + (pluno-1));
			if (vege_trace_head >= VEGE_TRACE_LOT_COUNT)
			{
				vege_trace_head = 0;
				set_nvram_bparam(NVRAM_VEGE_TRACE_HEAD + (pluno-1), vege_trace_head);
			}


			//1. copy trace info from PC
			memcpy(trace_new[0].code, vegeTraceTmp->code, TRACE_CODE_SIZE);
			trace_new[0].dt[0] = vegeTraceTmp->dt[0];
			trace_new[0].dt[1] = vegeTraceTmp->dt[1];
			trace_new[0].dt[2] = vegeTraceTmp->dt[2];
			trace_new[0].dt[3] = vegeTraceTmp->dt[3];
			trace_new[0].dt[4] = vegeTraceTmp->dt[4];
			trace_new[0].dt[5] = vegeTraceTmp->dt[5];
			trace_new[0].inputWeight = vegeTraceTmp->inputWeight;
			remainWeight_new[0] = vegeTraceTmp->inputWeight;

			//2. search trace code
			flag = 0;//NOT_EQUAL;
			for (lot_no = 0; lot_no< VEGE_TRACE_LOT_COUNT; lot_no++)
			{
				//load trace vegetabl lot table
				i = (VEGE_TRACE_LOT_COUNT + vege_trace_head - 1 - lot_no) % VEGE_TRACE_LOT_COUNT;
				vege_trace_load(&trace_old[0], pluno, i);
				if (trace_compare_data(vegeTraceTmp->code, trace_old[0].code, TRACE_CODE_SIZE)) //return : NOT_EQUAL=0, EQUAL=1
				{
					flag = 1;
					break;
				}
			}


			//3. change plu name
			plu_table_search(status_scale.departmentid, pluno, &plu_save_no, 0);
			if (plu_save_no)
			{
				memcpy(name, vegeTraceTmp->name, MAX_ARTNAME);//30 byte
				name[MAX_ARTNAME] = 0;
				plu_set_field(plu_save_no-1, PTYPE_PLU_NAME1, (INT8U*)&name);
			}

			//4. save trace code
			if (flag == 1)	// 기존 list에 있을경우
			{
				retVal = lot_no + 1;	// 회신할 lot no
			}
			else
			{
				// save remainWeight & tracecode
				//offset = VEGE_REMAIN_WEIGHT_SIZE * (VEGE_TRACE_LOT_COUNT * (pluno-1) + vege_trace_head);
				//addr = NVRAM_BASE + NVRAM_VEGE_REMAIN_WEIGHT + offset;
				//memcpy((INT8U *)addr, (INT8U *)&remainWeight_new[0], VEGE_REMAIN_WEIGHT_SIZE);
				vege_trace_save(&trace_new[0], pluno, vege_trace_head);
				vege_trace_remainWeight_save(pluno, vege_trace_head, remainWeight_new[0]);

				vege_trace_head++;
				if (vege_trace_head >= VEGE_TRACE_LOT_COUNT) vege_trace_head = 0;
				set_nvram_bparam(NVRAM_VEGE_TRACE_HEAD + (pluno-1), vege_trace_head);
				retVal = 1;	// 가장 최근 lot no 회신
			}
			number = retVal;

			break;
		case DATA_BOOTH :
			if(number > BOOTH_NO_MAX || number == 0) return ERROR_NUMBER_OVER;
			set_net_wparam(NETWORK_STORE_ID, (INT16U)number);
			break;
		case DATA_ACCOUNT:
			if (number == 100000000)
			{
				transaction_log_delete();
			}
			else if (number == 300000000)
			{
				stock = (VEGE_STOCK_INF *)dataPtr;
				vege_trace_clear_stock(stock->commodityCode, stock->dt, stock->salePercent);
			}
			else return ERROR_NUMBER_OVER;
			break;
#endif
#ifdef USE_REMOTE_KEY
		case DATA_REMOTE_KEY:
			if (number > 0x00 && number < 0x80)
			{
				KEY_Write((INT8U)number, 1, 0);
			}
			else return ERROR_NUMBER_OVER;
			break;
#endif
		default:
		return ERROR_INTERPRETER;
	}

	if (p)
	{
	    p+=DFLASH_BASE;
	    Flash_swrite(p,(HUGEDATA INT8U *)temp,(INT16U)sz);
//	    commun_recv_monitor('T',(INT16S)part,(INT16S)id);
	}

	checksum = 0;

	buffer[0]='G';
	buffer[1]=opCode;
	if (opCode != DATA_FLASH && opCode != DATA_EEPROM && opCode != DATA_RAM)
	{
		buffer[2]=status_scale.scaleid;
		buffer[3]=0;
		buffer[4]=0;
		buffer[5]=0;
	}
	buffer[6]=',';
	buffer[7] = 4;
	buffer[8] = 0;
	buffer[9]=':';
	memcpy(&buffer[10], &number, sizeof(INT32U));
	buffer[14]=':';

	for(i=2; i<15; i++)
		checksum += buffer[i];

	buffer[15]=checksum;
	buffer[16]=0x0D;//CR;

	*sLength = 17;

    return long_exe_flag;
}

INT32U bar_upload(INT32U roomNo)
{
	INT32U existNo;
	INT32U i;
	INT16U sz;
	INT32U p;
	BARCODE_STRUCT  barcode;

	existNo = 0;

	for (i = 1; i <= BAR_QTY_MAX; i++)
	{
			sz=BARCODE_STRUCT_SIZE;
		    p =(i-1)*sz;
		    p+=FLASH_BARCODE_FORMAT_AREA;
		    p+=DFLASH_BASE;
			Flash_sread(p,(HUGEDATA INT8U*)&barcode,sz);
			if (i == barcode.barcode_num) existNo++;
			if (roomNo == existNo) return i;
	}
	return 0;
}

INT8U bar_load(INT32U barNo, INT8U *src)
{
	INT16U sz;
	INT32U p;
	BARCODE_STRUCT  barcode;

	sz=BARCODE_STRUCT_SIZE;
    p =(barNo-1)*sz;
    p+=FLASH_BARCODE_FORMAT_AREA;
    p+=DFLASH_BASE;
    Flash_sread(p,(HUGEDATA INT8U*)&barcode,sz);

	src[0] = barcode.barcode_type;
	memcpy(src + 1,barcode.barcode_format,BARCODE_FORMAT_LEN);
	return 1; //인터프리터 에서만 사용하므로 존재여부 확인하지 않음 무조근 1 반환(이전 함수에서 존재 여부 확인함)
}

INT32U group_upload(INT32U roomNo)
{
	INT32U existNo;
	INT32U i;
	INT16U sz;
	INT32U p;
	char   temp[32];

	existNo = 0;

	for (i = 1; i <= GROUP_QTY_MAX; i++)
	{
			sz = GROUP_STRUCT_SIZE;
		    p =(i-1)*sz;
		    p+=FLASH_SUB_DEPARTMENT_AREA;
		    p+=DFLASH_BASE;
			Flash_sread(p,(HUGEDATA INT8U*)temp,sz);
			if (i == temp[0]) existNo++;
			if (roomNo == existNo) return i;
	}
	return 0;
}

INT8U group_load(INT32U groupNo, INT8U *src)
{
	INT16U sz;
	INT32U p;
	char   temp[32];

	sz = GROUP_STRUCT_SIZE;
    p =(groupNo-1)*sz;
    p+=FLASH_SUB_DEPARTMENT_AREA;
    p+=DFLASH_BASE;

	Flash_sread(p,(HUGEDATA INT8U*)temp,sz);

	memcpy(src,temp+2,30);
	return 1; //인터프리터 에서만 사용하므로 존재여부 확인하지 않음 무조근 1 반환(이전 함수에서 존재 여부 확인함)
}

INT32U scmsg_upload(INT32U roomNo)
{
	INT32U existNo;
	INT32U i;
	INT16U sz;
	INT32U p;
	SCROLL_MESSAGE_STRUCT	scroll;

	existNo = 0;

	for (i = 1; i <= MAX_SCROLL_NO; i++)
	{
			sz=sizeof(scroll);
		    p =(i-1)*sz;
		    p+=FLASH_SCROLL_AREA;
		    p+=DFLASH_BASE;
			Flash_sread(p,(HUGEDATA INT8U*)&scroll,sz);
			if (scroll.stype >= 1 && scroll.stype <=3) existNo++;
			if (roomNo == existNo) return i;
	}
	return 0;
}

INT8U scmsg_load(INT32U scgNo, INT8U *src)
{
	INT16U sz;
	INT32U p;
	SCROLL_MESSAGE_STRUCT	scroll;

	sz=sizeof(scroll);
    p =(scgNo-1)*sz;
    p+=FLASH_SCROLL_AREA;
    p+=DFLASH_BASE;
    Flash_sread(p,(HUGEDATA INT8U*)&scroll,sz);

	memset(src, 0, sizeof(scroll));
	src[2] = scroll.use;
	memcpy(src + 4,scroll.text,80);
	return 1; //인터프리터 에서만 사용하므로 존재여부 확인하지 않음 무조근 1 반환(이전 함수에서 존재 여부 확인함)
}

INT32U shop_upload(INT32U roomNo)
{
	INT32U existNo;
	INT32U i;
	INT16U sz;
	INT16U code;
	INT32U p;
	STORE_NAME_STRUCT	store;

	existNo = 0;

	for (i = 1; i <= SHOP_QTY_MAX; i++)
	{
		sz = sizeof(store);
		p =(i-1)*sz;
		p+=FLASH_SHOPNAME_AREA;
		p+=DFLASH_BASE;
		code = Flash_wread(p);//store.code
		if(code != 0) existNo++;
		if (roomNo == existNo) return i;
	}
	return 0;
}

INT8U shop_load(INT32U scgNo, INT8U *src)
{
	INT16U sz;
	INT32U p;
	STORE_NAME_STRUCT	store;

	sz=sizeof(store);
    p =(scgNo-1)*sz;
    p+=FLASH_SHOPNAME_AREA;
    p+=DFLASH_BASE;
    Flash_sread(p,(HUGEDATA INT8U*)&store,sz);
	memset(src, 0, SHOPNAME_STRUCT_SIZE);

//	store.code = (INT16U)scgNo;

	memcpy(src,(INT8U _PACKED_DEF *)store.name,26);
	memcpy(src+26,(INT8U _PACKED_DEF *)store.tel_no,20);
#ifdef USE_RUSSIA_DEFAULT
	memcpy(src+46,(INT8U _PACKED_DEF *)store.text,150);
#else
	memcpy(src+46,(INT8U _PACKED_DEF *)store.text,80);
#endif
	return 1; //인터프리터 에서만 사용하므로 존재여부 확인하지 않음 무조근 1 반환(이전 함수에서 존재 여부 확인함)
}

INT32U imessage_upload(INT32U roomNo)
{
	INT32U existNo;
	INT32U i;
	INT16U sz;
	INT32U p;
	char   temp[512];

	existNo = 0;

	for (i = 1; i <= MAX_INGREDIENT_NO; i++)
	{
			sz = INGREDIENT_STRUCT_SIZE;
		    p =(i-1)*sz;
		    p+=FLASH_INGREDIENT_AREA;
		    p+=DFLASH_BASE;
			Flash_sread(p,(HUGEDATA INT8U*)temp,sz);
			if (i == temp[0]) existNo++;
			if (roomNo == existNo) return i;
	}
	return 0;
}

INT8U imessage_load(INT32U groupNo, INT8U *src)
{
	INT16U sz;
	INT32U p;
	char   temp[512];

	sz = INGREDIENT_STRUCT_SIZE;
    p =(groupNo-1)*sz;
    p+=FLASH_INGREDIENT_AREA;
    p+=DFLASH_BASE;

	Flash_sread(p,(HUGEDATA INT8U*)temp,sz);

	memcpy(src,temp+2,400);
	return 1; //인터프리터 에서만 사용하므로 존재여부 확인하지 않음 무조근 1 반환(이전 함수에서 존재 여부 확인함)
}

extern INT16U transaction_sale_make_data_type7(char *buffer, INT16U tail);
INT8U readSavedData(COMM_BUF *CBuf, INT8U *buffer, INT16U *sLength)
{
	INT32U number;
	INT16U packetLen;
	INT32U maddr;
	//INT32U address;
	INT8U checksum;
	INT16U dataLen,sz;
	INT8U *dataPtr;
	INT16U i;
	lpDATA_HEADER dataHeader;
	INT32U int32uBuf,p;
	//PLU_INDEX pluIndex;
	VERSION_STRUCT *verBufPtr;

	INT8U opCode;
	INT8U *data;
	PACKET_HEADER _PACKED_DEF *header;
	BARCODE_STRUCT  barcode;

	//INT8U ptype, pleng;

	SCALE_STATUS_STRUCT *pScaleStatus;

    PLU_INF pluCommData;
//    CLERK_INF clerkCommData;
//    TAX_INF taxCommData;
#ifdef USE_DIRECT_INGREDIENT
	//INT32U pluno;
	//INT32U deptno;
	INT8U errNo;
	INT32U ingredient_address;
#endif
	INT32U  addr_key;
	FW_VERSION fversion;
#ifdef USE_CHN_CART_SCALE
	INT16U head;
	INT16U tail;
	INT16U temp_tail;
#endif

	version_get(&fversion);

	header = (PACKET_HEADER *)buffer;
	opCode = header->opCode[1];
	data = &buffer[sizeof(PACKET_HEADER)];
	//address = header->address;

	dataPtr = data;
	memcpy(&number, data, 4);

	switch (opCode)
	{
		case DATA_PLU: // PLU //not use (slow)
			dataPtr += sizeof(INT32U);
			dataLen = PLU_HOST_STORAGE_SIZE;

			int32uBuf = number/1000000L;//dept num

			if (int32uBuf == 0)	//room number
			{
				if (number == 0 || number > PLU_QTY_MAX) return ERROR_NUMBER_OVER;//room number
				//maddr = FLASH_SCALE_PLU_INDEX_BASE + sizeof(PLU_INDEX) * (INT32U)(number - 1);
				//ExMem_sread(maddr, (INT8U *)&pluIndex, sizeof(PLU_INDEX));

				//if (pluIndex.DeptPluNo == 0) return ERROR_DATA_END;//end
			}
			else	//plu number
			{
				if (int32uBuf > 99) return ERROR_NUMBER_OVER;//dept number

				number = number % 1000000L;//plu number
				if (number == 0) return ERROR_NUMBER_OVER;

				//if (plu_search(int32uBuf, number, &i, &pluIndex.Offset) == 0) {// plu number의 경우
				//	return ERROR_DATA_END;//end
				//}
				//not applied now 20110413
				*sLength = 0;
				return 0;
			}
			commun_send_plu_data(CBuf, number);
			*sLength = 0;
			return 0;
			//maddr = FLASH_SCALE_PLU_BASE + PLU_STORAGE_SIZE * (INT32U)pluIndex.Offset;
/*
            //art2buf(&artbuf1, number); //loadd plu data
			if (opCode == DATA_PLU)
			{
               //plu copy func.
                memset((char *)&pluCommData, 0, dataLen);

                pluCommData.deptNo = 1;     //default (CT100 don't use, but CL-Works need default.)
                pluCommData.no = number;    //plu number is same index which plu saved buffer position
                pluCommData.unitPrice = artbuf1.price;
                pluCommData.icode = artbuf1.cod;
                pluCommData.groupNo = (INT16U)artbuf1.group;
                pluCommData.taxNo = artbuf1.tax;              //CL5000J's protocol don't have item
                pluCommData.traceNo = artbuf1.traceNo;
                if (artbuf1.f.noweight) pluCommData.type = 2;
                else pluCommData.type = 1;
                pluCommData.fixedPrice = artbuf1.f.fixprice;    //CL5000J's protocol don't have item
                pluCommData.barNo1 = artbuf1.f.printbar;        //CL5000J's protocol don't have item
                memcpy(pluCommData.name1, artbuf1.name, 31);    //PLU NAME SIZE = 31
                pluCommData.sDate = (INT32U)artbuf1.valid;

				memcpy(dataPtr, (char *)&pluCommData, dataLen);
				dataLen += sizeof(INT32U);
			}
*/
			break;
#ifdef USE_DIRECT_INGREDIENT
		case DATA_PLU2:
			dataPtr += sizeof(INT32U);
			int32uBuf = number/1000000L;//dept num

			if (int32uBuf == 0)	//dept num
			{
				if (number == 0 || number > PLU_QTY_MAX) return ERROR_NUMBER_OVER;//room number
				errNo = sendPluData(buffer, &dataLen, number, opCode);
				if (errNo != 0)
				{
					return errNo;
				}
				*sLength = dataLen;
				return 0;
			}
			else	//plu number
			{
				if (int32uBuf > 99) return ERROR_NUMBER_OVER;//dept number

				//number = number % 1000000L;//plu number
				if ((number % 1000000L) == 0) return ERROR_NUMBER_OVER;

				errNo = sendPluData(buffer, &dataLen, number, opCode);
				if (errNo != 0)
				{
					return errNo;
				}
				*sLength = dataLen;
				return 0;
			}
			break;
#endif
#ifdef USE_RUSSIA_LOGISTICS_PLU
		case DATA_LOGISTICS_INGREDIENT:
			ingredient_address = header->address;
			dataPtr += sizeof(INT32U);
			int32uBuf = number/1000000L;//dept num

			if (int32uBuf == 0) //dept num
			{
				if (number == 0 || number > PLU_QTY_MAX) return ERROR_NUMBER_OVER;//room number
				errNo = sendPluData(buffer, &dataLen, number, opCode);
				if (errNo != 0)
				{
					return errNo;
				}
				*sLength = dataLen;
				return 0;
			}
			else	//plu number
			{
				if (int32uBuf > 99) return ERROR_NUMBER_OVER;//dept number

				//number = number % 1000000L;//plu number
				if ((number % 1000000L) == 0) return ERROR_NUMBER_OVER;

				errNo = sendPluData(buffer, &dataLen, number, opCode);
				if (errNo != 0)
				{
					return errNo;
				}
				*sLength = dataLen;
				return 0;
			}
			break;
#endif
		case DATA_SHOP:
			if (number > SHOP_QTY_MAX || number == 0) return ERROR_NUMBER_OVER;

			int32uBuf = shop_upload(number);
			if (int32uBuf == 0) return ERROR_DATA_END;

			if (shop_load(int32uBuf, dataPtr + sizeof(INT32U)))
			{
				memcpy(dataPtr, &int32uBuf, sizeof(INT32U));
				dataLen = FLASH_SCALE_SHOP_HOST_SIZE + sizeof(INT32U);
			}
			else return ERROR_DATA_NON;
			break;

		case DATA_BARCODE:
			if (number > BAR_QTY_MAX || number == 0) return ERROR_NUMBER_OVER;

			int32uBuf = bar_upload(number);

			if (int32uBuf == 0) return ERROR_DATA_END;

			if (bar_load(int32uBuf, dataPtr + sizeof(INT32U)))
			{
				memcpy(dataPtr, &int32uBuf, sizeof(INT32U));
				dataLen = BAR_HOST_STORAGE_SIZE + sizeof(INT32U);
			}
			else return ERROR_DATA_NON;
			break;

		case DATA_GROUP:
			if (number > GROUP_QTY_MAX || number == 0) return ERROR_NUMBER_OVER;
			int32uBuf = group_upload(number);

			if (int32uBuf == 0) return ERROR_DATA_END;

			if (group_load(int32uBuf, dataPtr + sizeof(INT32U)))
			{
				memcpy(dataPtr, &int32uBuf, sizeof(INT32U));
				dataLen = GROUP_HOST_STORAGE_SIZE + sizeof(INT32U);
			}
			else return ERROR_DATA_NON;
			break;

		case DATA_SCMSG:
			if (number > BAR_QTY_MAX || number == 0) return ERROR_NUMBER_OVER;

			int32uBuf = scmsg_upload(number);

			if (int32uBuf == 0) return ERROR_DATA_END;

			if (scmsg_load(int32uBuf, dataPtr + sizeof(INT32U)))
			{
				memcpy(dataPtr, &int32uBuf, sizeof(INT32U));
				dataLen = SCMSG_HOST_STORAGE_SIZE + sizeof(INT32U);
			}
			else return ERROR_DATA_NON;
			break;

		case DATA_VERSION:
			//Eeprom_sread(EEPROM_BASE,(INT8U *)dataPtr,sizeof(fw_version));
			verBufPtr = (VERSION_STRUCT *)dataPtr;
			verBufPtr->Class = 0;
			verBufPtr->ModelNum = 5500;// CL5500 = 5500, CT100 = 100, CL5000J = 5010
			verBufPtr->Type = (INT8U)keyTypeConvTable_CL5KJ[Startup.scale_type]; // 1-B, 2-P, 3-H, 4-S, 5-D, 6-B(CL5200)
			verBufPtr->ScaleId = status_scale.scaleid;
			verBufPtr->MainVersion = ((fversion.version-'0')*10)+(fversion.reversion[0]-'0');
			verBufPtr->SubVersion = fversion.reversion[1]-'0';
			verBufPtr->DataVersion = ((Startup.version[0]-'0')*10)+(Startup.version[1]-'0');
			verBufPtr->CountryNum = 0;
			verBufPtr->CountryCode[0] = 'R';//COUNTRY_CODE0;
			verBufPtr->CountryCode[1] = 'U';//COUNTRY_CODE1;
			dataLen = sizeof(VERSION_STRUCT);
			break;

		case DATA_STATUS:
			pScaleStatus = (SCALE_STATUS_STRUCT *)dataPtr;
			if (ADM_GetData.Overload) pScaleStatus->btLoadFlag = 2;		//0:Zero, 1:Non Zero, 2:Overload
			else if (ADM_GetData.Zero) pScaleStatus->btLoadFlag = 0;
			else pScaleStatus->btLoadFlag = 1;
			pScaleStatus->btStableFlag = ADM_GetData.Stable;			//0:Unstable, 1: Stable
			pScaleStatus->btTareFlag = ADM_GetData.Tare;				//0:No tare, 1:tare, 2:Percent tare(TO DO)
			pScaleStatus->btDualFlag = ADM_Status.Dual+1;								//2: dual range
			pScaleStatus->btWeightUnit = ADM_GetData.CurUnit;			//0:kg, 1:lb, 2:g
			pScaleStatus->btWeightDecimalPoint = Flash_bread(DFLASH_BASE + FLASH_GLOBAL_AREA + GLOBAL_DISPLAY_WEIGHTDECIMAL);
			pScaleStatus->btPriceDecimalPoint = Flash_bread(DFLASH_BASE + FLASH_GLOBAL_AREA + GLOBAL_AREA_PRICE_DECIMALPOINT);
			pScaleStatus->btReserved = 0;
			pScaleStatus->dwCurTare = status_scale.Tare;
			pScaleStatus->iCurWeight = status_scale.Weight;
			pScaleStatus->dwUnitPrice = status_scale.UnitPrice;
			pScaleStatus->dwTotalPrice = status_scale.TotalPrice;
			pScaleStatus->dwPLUno = status_scale.cur_pluid;
			pScaleStatus->wdDeptNo = status_scale.departmentid;
			dataLen = sizeof(SCALE_STATUS_STRUCT);
			break;

		case DATA_IMESSAGE:
			if (number > MAX_INGREDIENT_NO || number == 0) return ERROR_NUMBER_OVER;
			int32uBuf = imessage_upload(number);

			if (int32uBuf == 0) return ERROR_DATA_END;

			if (imessage_load(int32uBuf, dataPtr + sizeof(INT32U)))
			{
				memcpy(dataPtr, &int32uBuf, sizeof(INT32U));
				dataLen = INGREDIENT_STRUCT_SIZE-2 + sizeof(INT32U);
			}
			else return ERROR_DATA_NON;
			break;

		case DATA_KEY: // Key
			dataHeader = (lpDATA_HEADER)data;
			if (dataHeader->No > 5 || dataHeader->No == 0) return ERROR_NUMBER_OVER;
			// CL5000  : 1-B, 2-P, 3-H, 4-S,Ext_S(100key), 5-D, 6-B(CL5200)
			// CL5000J : 1-B, 2-P, 3-H, 4-S, 5-Ext_S(100key), 6-D
			if (dataHeader->No == 5) {//KEY_TABLE_NUM_SELF) {
				dataHeader->Offset = (INT16U)5; // 5-Ext_S(100key)
			} else {
				dataHeader->Offset = (INT16U)keyTypeConvTable_CL5KJ[Startup.scale_type]; // 1-B, 2-P, 3-H, 4-S, 5-D, 6-B(CL5200)
			}

			addr_key = DFLASH_BASE + FLASH_KEY_SPEED_AREA;
			addr_key += (INT32U)(dataHeader->No - 1) * MAX_SPEEDKEY_SIZE;
			Flash_sread(addr_key, dataPtr+4, (144*4));
			dataLen = (144*4) + sizeof(INT32U);
			break;
#ifdef USE_CHN_CART_SCALE
		case DATA_ACCOUNT: // account
			dataPtr += sizeof(INT32U); // log report number
			if (number >= ACC_LOG_TOTAL && number < (ACC_LOG_TOTAL + Startup.max_send_buf))
			{
				head = get_nvram_wparam(NVRAM_SEND_REMAINDED_HEAD);
				tail = get_nvram_wparam(NVRAM_SEND_REMAINDED_TAIL);
				//if ((number-ACC_LOG_TOTAL) >= head) return ERROR_DATA_END;
				temp_tail = tail + (number - ACC_LOG_TOTAL);
				temp_tail %= Startup.max_send_buf;
				if (head > tail)
				{
					if (temp_tail >= head || temp_tail < tail) return ERROR_DATA_END;
				}
				else
				{
					if (temp_tail >= head && temp_tail < tail) return ERROR_DATA_END;
				}

				dataLen = transaction_sale_make_data_type7(dataPtr, temp_tail);
				dataLen += sizeof(INT32U);
			}
			else
			{
				return ERROR_NUMBER_OVER;//room number
			}
			break;
		case DATA_BOOTH :
			int32uBuf = get_net_wparam(NETWORK_STORE_ID);
			if(int32uBuf > BOOTH_NO_MAX || int32uBuf == 0) return ERROR_NUMBER_OVER;

			*((INT32U _PACKED_DEF *)dataPtr) = int32uBuf;
			dataLen = sizeof(INT32U);
			break;
#endif

/*
		case DATA_LABEL_CL55:
		case DATA_BITMAP:
			break;
*/
/*
		case DATA_PARAMETER: // Parameter
			dataPtr += sizeof(INT32U);

			switch(number)
			{
				default://MAC Download시 사용하고 있음
					if (number >= 100 && number <= 999) {
						i = scale_parameter(number, 0, &ptype, 0, 0, P_CHECK);
						if (i == FALSE) return ERROR_NUMBER_OVER;

						if (ptype == 'S') {
							scale_parameter(number, (INT32U *)dataPtr, 0, 0, &pleng, P_READ);
							dataPtr += pleng;
							dataLen = sizeof(INT32U) + pleng;
						} else {
							scale_parameter(number, (INT32U *)dataPtr, 0, 0, 0, P_READ);
							dataPtr += sizeof(INT32U);
							dataLen = 2*sizeof(INT32U);
						}
					} else {
						return ERROR_NUMBER_OVER;
					}
					break;
			}
			if (number < 100) {
				Flash_sread(maddr, dataPtr, dataLen);
				dataLen += sizeof(INT32U);
			}
			break;
*/
#ifdef USE_REMOTE_DISP
		case DATA_REMOTE_DISP:
#ifdef _USE_LCD20848_ //ck
		//DspBuf[DspStruct.Page][1248]
			if (number > 4) return ERROR_NUMBER_OVER;
			if (number == 4) dataLen = 224;
			else dataLen = 256;
			dataPtr += sizeof(INT32U);
			memcpy(dataPtr, &(DspBuf[DspStruct.Page][number * 256]), dataLen);
			dataLen = dataLen + sizeof(INT32U);
#endif
			break;
#endif
		default :
			return ERROR_INTERPRETER;
	}

	buffer[0] = 'W';
	buffer[1] = opCode;
	if (opCode != DATA_FLASH && opCode != DATA_EEPROM && opCode != DATA_RAM)
	{
		buffer[2]=status_scale.scaleid;
		buffer[3]=0;
		buffer[4]=0;
		buffer[5]=0;
	}
	buffer[6] = ',';
	buffer[7] = (INT8U)dataLen;
	buffer[8] = (INT8U)(dataLen >> 8);
	buffer[9]=':';
	packetLen = sizeof(PACKET_HEADER) + dataLen;
	buffer[packetLen++] = ':';

	checksum = 0;
	for(i=2; i<packetLen; i++)
		checksum += buffer[i];

	buffer[packetLen++] = checksum;
	buffer[packetLen++] = 0x0D;//CR;

	*sLength = packetLen;

	return 0;
}

  #ifdef USE_CL5J_REALTIME_TRNASFER
INT8U process_response_good(COMM_BUF* CBuf, INT8U* buffer, INT16U* sLength)
{
	INT32U number = 0;
	INT8U *dataPtr;
	INT16U i;
	INT8U opCode;
	INT8U *data;
	PACKET_HEADER *header;
	
	header = (PACKET_HEADER *)buffer;
	opCode = header->opCode[1];
	data = &buffer[sizeof(PACKET_HEADER)];

	dataPtr = data;
	memcpy(&number, data, sizeof(INT32U)); 
	
	network_status.rcv_tail = number;
	network_status.send_trans_sale = EM_NET_SEND_TRAN_OK;
	
	return 0;
}

INT16U CL5_JR_transaction_sale_make_data(char *buffer, INT8U code, INT16U head, INT16U tail, INT16U *max_cnt)
{
	INT16U cnt;
	INT32U addr;
	INT8U temp1;
	long  mul;
	TRANSACTION_SALE_DATA_TYPE trans_sdt;
	TRANSACTION_CHANGE_DATA_TYPE trans_cdt;
	INT16S   slen;
	INT8U bcc;
	INT8U tflag, checksum;
	INT16U plu_key;
	INT16U len;
	int i, length;
	INT32U tail_num;
	RTT_STRUCT rtt_data;

	slen = 0;

	//Scale Capa에 따른 무게값을 "g" 단위로 전송 하고자 할때 
	temp1=get_global_bparam(GLOBAL_SALE_SETUP_FLAG7);
	temp1=(temp1>>6)&0x03;
	mul = 1L;
	if(temp1 > 0) mul = power10(temp1);

	cnt = 1;

	addr=tail; 
	addr%=Startup.max_send_buf;
	addr*=MAX_TRANSACTION_TYPE4_SALE_SIZE;//sizeof(TRANSACTION_RECORD_TYPE); 
	addr+=Startup.addr_send_buf;

	sprintf(buffer, "Wr");
	slen += strlen(buffer);
	for(i = 0; i < 4; i++)
		buffer[slen++] = 0;
	buffer[slen++] = ',';
	length = 4 + sizeof(RTT_STRUCT);
	buffer[slen++] = length & 0xff;
	buffer[slen++] = (length >> 8) & 0xff;
	buffer[slen++] = ':';
	tail = tail+cnt;
	for(i = 0; i < sizeof(INT32U); i++) {
		buffer[slen++] = (tail >> (8 * i)) & 0xff;
	}
	
	get_nvram_sparam(addr,(INT8U *)&trans_sdt,sizeof(TRANSACTION_SALE_DATA_TYPE));
	trans_sdt.sdtWeight *= mul;
	
	memset(&rtt_data, 0, sizeof(rtt_data));
	rtt_data.StoreNo = get_net_wparam(NETWORK_STORE_ID);
	rtt_data.DeptNo = trans_sdt.sdtDept;
	rtt_data.GroupNo = trans_sdt.sdtGroup;
	rtt_data.ScaleId = trans_sdt.sdtScaleId;
	if(trans_sdt.sdtStatus == FLAG_TRANS_ADD || trans_sdt.sdtStatus == FLAG_TRANS_LABEL)
		rtt_data.saleType = ACC_ADD;
	else if(trans_sdt.sdtStatus == FLAG_TRANS_VOID)
		rtt_data.saleType = ACC_VOID;
	else if(trans_sdt.sdtStatus == FLAG_TRANS_RETURN)
		rtt_data.saleType = ACC_RET;
	else
		rtt_data.saleType = ACC_NO;
	rtt_data.Plutype = trans_sdt.sdtPLUtype;
	rtt_data.ClerkNo = trans_sdt.sdtOperator;
	rtt_data.PluNo = trans_sdt.sdtPLUNo;
	rtt_data.Weight = trans_sdt.sdtWeight;
	rtt_data.Quantity = trans_sdt.sdtQty;
	rtt_data.PCS = trans_sdt.sdtPCS;
	rtt_data.TareWeight = trans_sdt.sdtTareWeight;
	rtt_data.UnitPrice = trans_sdt.sdtUnitPrice;
	rtt_data.TotalPrice = trans_sdt.sdtTotalPrice;
	rtt_data.ItemCode = trans_sdt.sdtItemCode;
	for(i = 0; i < sizeof(rtt_data.Date); i++) 
		rtt_data.Date[i] = trans_sdt.sdtDate[i];
	safe_strncpy(rtt_data.BarcodeStr, trans_sdt.sdtBarcodeStr, sizeof(rtt_data.BarcodeStr));
	rtt_data.LabelTranNo = trans_sdt.sdtTransactionCounter;
	
	plu_table_search(trans_sdt.sdtDept, trans_sdt.sdtPLUNo, &plu_key,0);
	plu_get_field(plu_key-1, PTYPE_PLU_NAME1, rtt_data.PLU_name, (INT16S *)&len, sizeof(rtt_data.PLU_name)); // name1
	
	memcpy(&buffer[slen],&rtt_data,sizeof(rtt_data));
	slen += sizeof(rtt_data);
	buffer[slen++] = ':';

	checksum = 0;
	for(i=2; i<slen; i++)
		checksum += buffer[i];

	buffer[slen++] = checksum;
	buffer[slen++] = 0x0D;	//CR;
	
	*max_cnt = cnt;
	return slen;
}
  #endif
#endif//#ifdef USE_PROTOCOL_CL5000JR