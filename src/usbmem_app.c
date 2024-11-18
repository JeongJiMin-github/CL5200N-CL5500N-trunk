#include <stdio.h>
#include <string.h>
#include "globals.h"
#include "app.h"
#include "initial.h"
#include "mode_main.h"
#include "mode_commun.h"
#include "rtc.h"
#include "main.h"
#include "common.h"

#include "usbmem_app.h"
#include "adm.h"
#include "dsp_app.h"
#include "plu_device.h"
#include "plu_manager.h"

#ifdef USE_CHN_CART_SCALE
#ifdef USE_CHN_IO_BOARD


#define USB_LOG_NUM_SIZE        4
#define USB_LOG_FILENAME_SIZE   12
#define USB_LOG_DATETIME_SIZE   6
#define USB_LOG_DATA_SIZE       110
#define USB_LOG_TOTAL_SIZE      (USB_LOG_NUM_SIZE+USB_LOG_FILENAME_SIZE+USB_LOG_DATETIME_SIZE+USB_LOG_DATA_SIZE)

#define USE_LOG_TITLE_NAME_SIZE         45
#define USE_LOG_TITLE_DATETIME_SIZE     26


void usbmem_send_log_to_usb(HUGEDATA COMM_BUF *CBuf, INT16U tail, INT16U num)
{
	INT8U str[200];
	INT8U checksum;
	INT8U i;
	INT32U addr;
	INT16U len;
	INT8U weightStr[10];
	INT8U unitStr[10];
	INT8U totalStr[10];
	RTC_TIME time;	// local(non BCD)
	RTC_DATE date;	// local(non BCD)
	REPORT_LOG_STRUCT rep_log_temp;
	INT8U decimal, w_decimal, dec_ch;
	INT8U buffer[60];
	INT16U plu_key;
	INT16U name_len;
	long  mul;
	INT8U temp;
	INT16U temp_tail;
	INT32U temp_num;
	
	//LOG_UPLOAD_INF logUpload;
	decimal=get_global_bparam(GLOBAL_AREA_PRICE_DECIMALPOINT);
	dec_ch =get_global_bparam(GLOBAL_DECIMAL_SIGN);
	w_decimal=get_decimal_pt(3, (INT16U)ADM_Status.UseUnit);
	
	len = 0;
	//header
	str[0] = 'W';
	str[1] = 'u'; //opcode for usb host 
	str[2] = (status_scale.scaleid)&0x00ff;
	str[3] = ((status_scale.scaleid)&0xff00) >> 8;
	str[4] = 0;
	str[5] = 0;
	str[6] = ',';
	str[7] = USB_LOG_TOTAL_SIZE;
	str[8] = 0;
	str[9] = ':';
	len = 10;
	
	//report number: 4 byte
	//sprintf((char *)&str[len], "%04ld", num);
	temp_num = (INT32U)num;
	memcpy((char *)&str[len], (char *)&temp_num, 4);
	len += 4;
	
	//file name: 12 byte
	RTC_CLK_Burst_Read();
	date.year    = bcd2hex(RTCStruct.year);
	date.month   = bcd2hex(RTCStruct.month);
	date.day     = bcd2hex(RTCStruct.date);
	time.hours   = bcd2hex(RTCStruct.hour);
	time.minutes = bcd2hex(RTCStruct.min);
	time.seconds = bcd2hex(RTCStruct.sec);
	sprintf((char *)&str[len], "%03dL%02d%02d.txt", get_net_wparam(NETWORK_STORE_ID), date.month, date.day);
	len += 12;
	
	//date: 6 byte
	str[26] = date.year;
	str[27] = date.month;
	str[28] = date.day;
	str[29] = time.hours;
	str[30] = time.minutes;
	str[31] = time.seconds;
	len += 6; //32
        
    //data: 110 byte
	if (num == 0) //start
	{
		//(1)date/time, (2)trace code, (3)PLU name, (4)PLU no, (5)shop no, (6)unit price, (7)weight, (8)total price  
		sprintf((char *)&str[len], "ÈÕÆÚ\t×·ËÝÂë\tÆ·Ãû\t´úÂë\tÌ¯Î»ºÅ\tµ¥¼Û\tÖØÁ¿\t½ð¶î\r\n");
		len += USE_LOG_TITLE_NAME_SIZE;
		
		sprintf((char *)&str[len], "ÈÕÆÚ\t20%02d/%02d/%02d %02d:%02d:%02d\r\n",  date.year, date.month, date.day, 
						   time.hours, time.minutes, time.seconds); 
		len += USE_LOG_TITLE_DATETIME_SIZE;
		
		memset(&str[len], 0x20, USB_LOG_DATA_SIZE - USE_LOG_TITLE_NAME_SIZE - USE_LOG_TITLE_DATETIME_SIZE - 2);
		len += (USB_LOG_DATA_SIZE - USE_LOG_TITLE_NAME_SIZE - USE_LOG_TITLE_DATETIME_SIZE - 2);
		
		str[len++] = ASC_CR;
		str[len++] = LF;
	}
	else
	{
		//Scale Capa¿¡ µû¸¥ ¹«°Ô°ªÀ» "g" ´ÜÀ§·Î Àü¼Û ÇÏ°íÀÚ ÇÒ¶§ 
		temp = get_global_bparam(GLOBAL_SALE_SETUP_FLAG7);
		temp = (temp >> 6) & 0x03;
		mul = 1L;
		if(temp > 0) mul = power10(temp);
	
		//tail + num
		temp_tail = tail + (num - 1);
		temp_tail %= Startup.max_send_buf;
		
		addr = (INT32U)temp_tail; 
		addr *= MAX_TRANSACTION_TYPE7_SALE_SIZE;
		addr += Startup.addr_send_buf;
		get_nvram_sparam(addr, (INT8U *)&rep_log_temp, sizeof(REPORT_LOG_STRUCT));
		rep_log_temp.weightOrcount *= mul;
		
		//maddr = LOG_BASE_ADDR + ((num-1)*LOG_STRUCTURE_SIZE);
		//Eeprom_sread(maddr, (INT8U *)&rep_log_temp, LOG_STRUCTURE_SIZE);
// For Test
/*
		rep_log_temp.pluNo = 1;       // 4
		rep_log_temp.itemNo = 1;      // 4
		rep_log_temp.saleType = 0;     // 1 add, void
		rep_log_temp.pluType = 1;      // 1
		rep_log_temp.ticketNo = 3;    // 4
		strcpy(rep_log_temp.traceCode, "123456789012");
		rep_log_temp.year = 12;         // 1
		rep_log_temp.month = 2;        // 1
		rep_log_temp.day = 15;          // 1
		rep_log_temp.hours = 19;        // 1
		rep_log_temp.minutes = 27;      // 1
		rep_log_temp.seconds = 59;      // 1
		rep_log_temp.weightOrcount = 345;// 4
		rep_log_temp.uprice = 12003;      // 4
		rep_log_temp.tprice = 123456;
*/
////////
	
		//transaction date
		sprintf((char *)&str[len], "20%02d/%02d/%02d %02d:%02d:%02d",  rep_log_temp.year, rep_log_temp.month, rep_log_temp.day, 
						   rep_log_temp.hours, rep_log_temp.minutes, rep_log_temp.seconds); 
		len += 19;
		str[len++] = '\t';
		
		//trace code
		memcpy(&str[len], rep_log_temp.traceCode, TRACE_CODE_SIZE);
		len += TRACE_CODE_SIZE;
		str[len++] = '\t';
		
		//plu name
		//art2buf(&artbuf1, rep_log_temp.pluNo);
		plu_table_search(status_scale.departmentid, rep_log_temp.pluNo, &plu_key,0);
		memset(&buffer[0], 0, 55);
		plu_get_field(plu_key-1, PTYPE_PLU_NAME1, (INT8U *)&buffer[0], (INT16S *)&name_len, 55); // name1
		memcpy(&str[len], buffer, MAX_ARTNAME);
		len += MAX_ARTNAME;
		str[len++] = '\t';
		
		//plu number
		sprintf((char *)&str[len], "%4ld", rep_log_temp.pluNo);
		len += 4;
		str[len++] = '\t';
		
		//booth number
		sprintf((char *)&str[len], "%4d", get_net_wparam(NETWORK_STORE_ID));
		len += 4;
		str[len++] = '\t';
		
		//unit price
		//digit_to_string(unitStr, rep_log_temp.uprice, pricedot, UNIT_DIGIT);
		//PrtMakePriceString(unitStr, rep_log_temp.uprice, decimal, dec_ch, 6);
		format_value(unitStr, rep_log_temp.uprice, decimal, dec_ch);
		sprintf((char *)&str[len], "%8s", unitStr);
		len += 8;
		str[len++] = '\t';
		
		//weight 
		//digit_to_string(weightStr, rep_log_temp.weightOrcount, kgdot, WEIGHT_DIGIT);
		//PrtMakeWeightString(weightStr, rep_log_temp.weightOrcount, w_decimal, dec_ch, 5);
		format_value(weightStr, rep_log_temp.weightOrcount, w_decimal, dec_ch);
		sprintf((char *)&str[len], "%8s", weightStr);
		len += 8;
		str[len++] = '\t';
		
		//total price 
		//digit_to_string(totalStr, rep_log_temp.tprice, pricedot, TOTAL_DIGIT);
		//PrtMakePriceString(totalStr, rep_log_temp.tprice, decimal, dec_ch, 7);;
		format_value(totalStr, rep_log_temp.tprice, decimal, dec_ch);
		sprintf((char *)&str[len], "%8s", totalStr);
		len += 8;
		
		str[len++] = ASC_CR;
		str[len++] = LF;
	}

	str[len++]=':';
	
	checksum = 0;
	for(i = 2; i < len; i++) checksum += str[i];
	
	str[len++]=checksum;
	str[len++]=ASC_CR;
	str[len] = 0;


	FlushRxBuf(CBuf);
	commun_outleng(CBuf, (char *)str, len);
}


/*
void commun_send_cmd(HUGEDATA COMM_BUF *CBuf, INT8U cmd, INT8U *wr_data1)
{
	switch(cmd)
	{
		case RFID_CMD_READ_CARD_NO:
			commun_send_read_card_no(CBuf);
			break;
		case RFID_CMD_READ_DATA:
			commun_send_read_data(CBuf, rfid_default_password);
			break;
		case RFID_CMD_WRITE_DATA:
			commun_send_write_data(CBuf, rfid_default_password, wr_data1);
			break;
		case RFID_CMD_CHANGE_PASSWD:
			commun_send_change_password(CBuf, rfid_default_password, wr_data1);
			break;

	}
}
*/


INT8U usbmem_RcvBuffer[256];
INT8U usbmem_RcvCmd[2];
INT8U usbmem_RcvResult;
INT8U usbmem_interpreter(HUGEDATA COMM_BUF *CBuf)
{
	INT8U r_data;
	INT8U r_cnt;
	INT8U ret;
	static INT8U buf_idx;
	static INT8U rcv_buf[32];	// data length´Â 4~5bytes
	static INT8U rcv_state;
	static INT8U bcc;
	static INT8U opCode[2];
	static INT32U address = 0;
	static INT16U data_length = 0;

	if (!CheckRxBuf(CBuf))
	{
		return 0;
	}
	//rcv_state = 0;
	ret = USBMEM_COM_NONE;
	//bcc = 0;
	
	//rcv_buf = &usbmem_RcvBuffer[0];
	usbmem_RcvResult = FALSE;

	r_cnt = CheckRxBufCountInt(CBuf);

//sprintf(MsgBuf, "rcv_cnt=%d, st=%d\r\n", r_cnt, rcv_state);
//MsgOut(MsgBuf);	
	while(r_cnt)
	{
		r_data = GetCharRxBuf(CBuf);
		
		switch (rcv_state)
		{
			case 0:	// rcv opcode1
				if (r_data != 'G' && r_data != 'N') break;
				opCode[0] = r_data;
				bcc = 0;
				address = 0;
				data_length = 0;
				buf_idx = 0;
				rcv_state = 1;
				break;
			case 1:	// rcv opcode2
				if (r_data != 'u')
				{
					rcv_state = 0;
					break;
				}
				opCode[1] = r_data;
				bcc = 0;
				rcv_state = 2;
				break;
			case 2: // address(4bytes)
				rcv_buf[buf_idx++] = r_data;
				bcc += r_data;
				if (buf_idx == 4)
				{
					memcpy((INT8U *)&address, rcv_buf, 4);
					rcv_state++;
				}
				break;
			case 3: // ','(comma)
				if (r_data == ',') rcv_state++;
				else
				{
					ret = USBMEM_COM_ERR_HEADER_COMMA;
					rcv_state = 0;
				}
				bcc += r_data;
				buf_idx = 0;
				break;
			case 4: // size(2bytes)
				rcv_buf[buf_idx++] = r_data;
				bcc += r_data;
				if (buf_idx == 2)
				{
					memcpy((INT8U *)&data_length, rcv_buf, 2);
					rcv_state++;
				}
				break;
			case 5: // ':'(colon)
				if (r_data == ':') rcv_state++;
				else
				{
					ret = USBMEM_COM_ERR_HEADER_COLON;
					rcv_state = 0;
				}
				bcc += r_data;
				buf_idx = 0;
				break;
			case 6: // data
				rcv_buf[buf_idx++] = r_data;
				bcc += r_data;
				if (buf_idx == data_length)
				{
					rcv_state++;
				}
				break;
			case 7: // ':'(colon)
				if (r_data == ':') rcv_state++;
				else
				{
					ret = USBMEM_COM_ERR_TAIL_COLON;
					rcv_state = 0;
				}
				bcc += r_data;
				break;
			case 8: // checksum
				if (r_data == bcc) rcv_state++;
				else
				{
					ret = USBMEM_COM_ERR_CHECKSUM;
					rcv_state = 0;
				}
				break;
			case 9: // CR
				if (r_data == ASC_CR)	// OK
				{
					ret = USBMEM_COM_OK;
					usbmem_RcvCmd[0] = opCode[0];
					usbmem_RcvCmd[1] = opCode[1];
					memcpy(usbmem_RcvBuffer, rcv_buf, data_length);
					usbmem_RcvResult = TRUE;
					rcv_state = 0;
				}
				else
				{
					ret = USBMEM_COM_ERR_CR;
					rcv_state = 0;
				}
				break;
				
			default:
				rcv_state = 0;
				break;
		}
//sprintf(MsgBuf, "[%d]%02x ", rcv_state, r_data);
//MsgOut(MsgBuf);	
		r_cnt--;
		if (ret) break;
	}
//sprintf(MsgBuf, "<%d>\r\n", ret);
//MsgOut(MsgBuf);	
	return ret;
}

#endif
#endif

extern INT8U UsbdeviceIsConnected;
INT8U usbmem_save_report(INT8U *filename, INT8U *str)
{
	SYS_FS_HANDLE fileHandle;
	INT8U fullname[256];

	if(UsbdeviceIsConnected)
	{
		sprintf((char*)fullname, "/mnt/myDrive1/%s", filename);
		fileHandle = SYS_FS_FileOpen((char*)fullname, (SYS_FS_FILE_OPEN_APPEND_PLUS));
		if(fileHandle == SYS_FS_HANDLE_INVALID)
		{
			/* Could not open the file. Error out*/
			return OFF;
		}
		else
		{
			if (SYS_FS_FileWrite( fileHandle, (const void *) str, strlen((char*)str) ) == -1)
			{
				/* Write was not successful. Close the file
				 * and error out.*/
				SYS_FS_FileClose(fileHandle);
				return OFF;
			}                
			SYS_FS_FileClose(fileHandle);
		}
	}
	else
	{
		return OFF;
	}
	return ON;
}