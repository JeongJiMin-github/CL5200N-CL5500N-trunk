#include <stdio.h>
#include "globals.h"
#include "initial.h"
#include "mode_commun.h"
#include "common.h"

#include "rfid_drv_rfid522b.h"

#ifdef USE_RFID_TAG_READER_RFID_522_B
// Use RFID_522_B(China)

INT8U rfid_default_password[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
void commun_send_read_card_no(HUGEDATA COMM_BUF *CBuf)
{
	INT8U string_buf[100];
	INT8U i;
	INT8U bcc;
	
	i = 0;
	string_buf[i++] = 0x02;
	string_buf[i++] = 0x05;
	string_buf[i++] = RFID_CMD_READ_CARD_NO;
	bcc = gen_bcc(&string_buf[1], 2);
	string_buf[i++] = bcc;
	string_buf[i++] = 0x03;

	commun_outleng(CBuf, (char *)string_buf, i);
}

void commun_send_read_data(HUGEDATA COMM_BUF *CBuf, INT8U *password)
{
	INT8U string_buf[100];
	INT8U i;
	INT8U j;
	INT8U bcc;
	INT8U blockno;
	
	blockno = 0x04;	// example
	
	i = 0;
	string_buf[i++] = 0x02;
	string_buf[i++] = 0x0c;
	string_buf[i++] = RFID_CMD_READ_DATA;
	for (j = 0; j < 6; j++)
	{
		string_buf[i++] = password[j];
	}
	string_buf[i++] = blockno;
	bcc = gen_bcc(&string_buf[1], 2+6+1);
	string_buf[i++] = bcc;
	string_buf[i++] = 0x03;

	commun_outleng(CBuf, (char *)string_buf, i);
}

void commun_send_write_data(HUGEDATA COMM_BUF *CBuf, INT8U *password, INT8U *wr_data)
{
	INT8U string_buf[100];
	INT8U i;
	INT8U j;
	INT8U bcc;
	INT8U blockno;
	
	blockno = 0x04;	// example
	
	i = 0;
	string_buf[i++] = 0x02;
	string_buf[i++] = 0x1c;
	string_buf[i++] = RFID_CMD_WRITE_DATA;
	for (j = 0; j < 6; j++)
	{
		string_buf[i++] = password[j];
	}
	string_buf[i++] = blockno;
	for (j = 0; j < 16; j++)
	{
		string_buf[i++] = wr_data[j];
	}
	bcc = gen_bcc(&string_buf[1], 2+6+1+16);
	string_buf[i++] = bcc;
	string_buf[i++] = 0x03;

	commun_outleng(CBuf, (char *)string_buf, i);
}

void commun_send_change_password(HUGEDATA COMM_BUF *CBuf, INT8U *password, INT8U *new_password)
{
	INT8U string_buf[100];
	INT8U i;
	INT8U j;
	INT8U bcc;
	INT8U blockno;
	
	blockno = 0x04;	// example
	
	i = 0;
	string_buf[i++] = 0x02;
	string_buf[i++] = 0x12;
	string_buf[i++] = RFID_CMD_CHANGE_PASSWD;
	for (j = 0; j < 6; j++)
	{
		string_buf[i++] = password[j];
	}
	string_buf[i++] = blockno;
	for (j = 0; j < 6; j++)
	{
		string_buf[i++] = new_password[j];
	}
	bcc = gen_bcc(&string_buf[1], 2+6+1+6);
	string_buf[i++] = bcc;
	string_buf[i++] = 0x03;

	commun_outleng(CBuf, (char *)string_buf, i);
}


void commun_send_cmd(HUGEDATA COMM_BUF *CBuf, INT8U cmd, INT8U *wr_data1)
{
	FlushRxBuf(CBuf);
	RFID_RcvState = 0;
	memset(RFID_RcvBuffer, 0, sizeof(RFID_RcvBuffer));
	RFID_RcvCmd = 0;
	RFID_RcvResult = FALSE;
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
/*
		case RFID_CMD_BEEP_ON:
			commun_send_read_data(CBuf, rfid_default_password);
			break;
		case RFID_CMD_BEEP_OFF:
			commun_send_read_data(CBuf, rfid_default_password);
			break;
		case RFID_CMD_BEEP_ERR_ON:
			commun_send_read_data(CBuf, rfid_default_password);
			break;
		case RFID_CMD_BEEP_ERR_OFF:
			commun_send_read_data(CBuf, rfid_default_password);
			break;
*/
	}
}


INT8U RFID_RcvBuffer[256];
INT8U RFID_RcvCmd;
INT8U RFID_RcvResult;
INT8U RFID_RcvState;
INT8U rfid_interpreter(HUGEDATA COMM_BUF *CBuf)
{
	INT8U *rcv_buf;
	INT8U r_data;
	static INT8U r_len;
	INT8U r_cnt;
	INT8U ret;
	static INT8U bcc;
	static INT8U block_no;
	static INT8U status;
	static INT8U length;
	static INT8U cmd;
	static INT8U buf_idx;
	//INT8U rcv_bcc;

	if (!CheckRxBuf(CBuf))
	{
		return 0;
	}
	ret = 0;
	//bcc = 0;
	//block_no = 0;
	//status = 0;
	//r_len = 0;
	//length = 0;
	//cmd = 0;
	
	rcv_buf = &RFID_RcvBuffer[0];
	RFID_RcvResult = TRUE;

	r_cnt = CheckRxBufCountInt(CBuf);
	
	while(r_cnt)
	{
		r_data = GetCharRxBuf(CBuf);
		
		switch (RFID_RcvState)
		{
			case 0:	// rcv STX
				if (r_data == STX)
				{
					RFID_RcvState = 1;
				}
				else
				{
					//ret = RCV_NO_START;
				}
				bcc = 0;
				block_no = 0;
				status = 0;
				r_len = 0;
				length = 0;
				cmd = 0;
				r_len = 1;
				buf_idx = 0;
				break;
			case 1:	// rcv length
				length = r_data;
				bcc ^= r_data;
				r_len++;
				RFID_RcvState = 2;
				break;
			case 2:	// rcv cmd
				cmd = r_data;
				bcc ^= r_data;
				r_len++;
				buf_idx = 0;
				switch(cmd)
				{
					case RFID_CMD_READ_CARD_NO: // 0xA0
						if (length == 7) RFID_RcvState = 90;	// fail
						else RFID_RcvState = 10;
						break;
					case RFID_CMD_READ_DATA: //0xA1
						if (length == 7) RFID_RcvState = 90;	// fail
						else RFID_RcvState = 20;
						break;
					case RFID_CMD_WRITE_DATA: // 0xA2
						RFID_RcvState = 30;
						break;
					case RFID_CMD_CHANGE_PASSWD: // 0xA3
						RFID_RcvState = 40;
						break;
					case RFID_CMD_BEEP_ON: // 0xA6
						if (length == 7) RFID_RcvState = 90;	// fail
						else RFID_RcvState = 50;
						break;
					case RFID_CMD_BEEP_OFF: // 0xA7
						if (length == 7) RFID_RcvState = 90;	// fail
						else RFID_RcvState = 60;
						break;
					case RFID_CMD_BEEP_ERR_ON: // 0xA8
						if (length == 7) RFID_RcvState = 90;	// fail
						else RFID_RcvState = 70;
						break;
					case RFID_CMD_BEEP_ERR_OFF: // 0xA9
						if (length == 7) RFID_RcvState = 90;	// fail
						else RFID_RcvState = 80;
						break;
					default:
						ret = RCV_DATA_INVALID;
						RFID_RcvState = 0;
						break;
				}
				break;
	
			case 10: // read card no(0xA0)
			case 50: // beep on
			case 60: // beep err on
				rcv_buf[buf_idx++] = r_data;
				bcc ^= r_data;
				r_len++;
				if (buf_idx == 4) RFID_RcvState++;
				break;
			case 11: // read card no(0xA0)
			case 51: // beep on
			case 61: // beep err on
				bcc ^= r_data;
				r_len++;
				if (r_data == 0x00) RFID_RcvState = 99;
				else
				{
					ret = RCV_DATA_INVALID;
					RFID_RcvState = 0;
				}
				break;
			
			case 20: // read data(0xA1)
				rcv_buf[buf_idx++] = r_data;
				bcc ^= r_data;
				r_len++;
				if (buf_idx == 16) RFID_RcvState = 21;
				break;
			case 21:
				block_no = r_data;
				bcc ^= r_data;
				r_len++;
				RFID_RcvState = 99;
				break;
			
			case 30: // write data(0xA2)
			case 40: // change password(0xA3)
			case 90: // Fail(0xA0, 0xA1, 0xA6, 0xA7, 0xA8, 0xA9)
				status = r_data;
				bcc ^= r_data;
				r_len++;
				RFID_RcvState = 31;
				break;
			case 31: // write data(0xA2)
			case 41: // change password(0xA3)
			case 91: // Fail(0xA0, 0xA1, 0xA6, 0xA7, 0xA8, 0xA9)
				block_no = r_data;
				bcc ^= r_data;
				r_len++;
				RFID_RcvState = 99;
				break;
				
			case 99: // rcv CRC
				bcc ^= r_data;
				r_len++;
				if (r_len == length - 1)
				{
					if (bcc == 0) // bcc ok
					{
						RFID_RcvState = 100;
					}
					else // bcc fail
					{
						ret = RCV_BCC_FAIL;
						RFID_RcvState = 0;
					}
				}
				else
				{
					ret = RCV_LENGTH_ERR;
					RFID_RcvState = 0;
				}
				break;
			case 100:
				if (r_data == ETX)
				{
					ret = RCV_OK;
					if (status)
					{
						if (status == 0x06) RFID_RcvResult = TRUE;
						else RFID_RcvResult = FALSE;
					}
					RFID_RcvCmd = cmd;
				}
				RFID_RcvState = 0;
				break;
			default:
				RFID_RcvState = 0;
				break;
		}
		r_cnt--;
		if (ret) break;
	}


	return ret;
}
#endif