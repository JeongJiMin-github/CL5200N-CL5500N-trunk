/*****************************************************************************
|*			
|*  Copyright		:	(c) 2002 CAS
|*  Filename		:	commbuf.c
|*  Version		:	0.1
|*  Programmer(s)	:	Hong Yi-Phyo (HYP)
|*  Created		:	2002/12/10
|*  Description		:	CLP Printer communication buffer routine    
|*				
****************************************************************************/

#include "globals.h"
#include "commbuf.h"
#include "prt_common.h"
#include "initial.h"

HUGEDATA COMM_BUF CommBuf;	 //PC
HUGEDATA COMM_BUF CommBufAD;	 //AD
#ifndef CPU_PIC32MZ
HUGEDATA COMM_BUF CommBufKey;	 //Key
HUGEDATA COMM_BUF CommBufUsb;	 //USB
#endif
HUGEDATA COMM_BUF CommBufEth;	 //Parallel	(Ethernet)
HUGEDATA COMM_BUF CommBufPrt;	 //Printer Buffer
//HYP 20041213
HUGEDATA COMM_BUF CommBufEthData[ETH_BUF_MAX_NUM]; 
HUGEDATA char CommTxBuf1Memory[COM_MAX_TX_BUF_HOST_SIZE];
HUGEDATA char CommRxBuf1Memory[COM_MAX_RX_BUF_HOST_SIZE];
HUGEDATA char CommTxBuf2Memory[COM_MAX_TX_BUF_AD_SIZE];
HUGEDATA char CommRxBuf2Memory[COM_MAX_RX_BUF_AD_SIZE];
HUGEDATA char CommTxBuf3Memory[COM_MAX_TX_BUF_ETH_SIZE];
HUGEDATA char CommRxBuf3Memory[COM_MAX_RX_BUF_ETH_SIZE];
HUGEDATA char CommTxBuf4Memory[COM_MAX_TX_BUF_PRT_SIZE];
HUGEDATA char CommRxBuf4Memory[COM_MAX_RX_BUF_PRT_SIZE];
#ifndef CPU_PIC32MZ
HUGEDATA char CommTxBuf5Memory[COM_MAX_TX_BUF_KEY_SIZE];
HUGEDATA char CommRxBuf5Memory[COM_MAX_RX_BUF_KEY_SIZE];
HUGEDATA char CommTxBuf6Memory[COM_MAX_TX_BUF_USB_SIZE];
HUGEDATA char CommRxBuf6Memory[COM_MAX_RX_BUF_USB_SIZE];
#endif
HUGEDATA char CommTxBuf10Memory[ETH_BUF_MAX_NUM][COM_MAX_TX_BUF_ETHDATA_SIZE];
HUGEDATA char CommRxBuf10Memory[ETH_BUF_MAX_NUM][COM_MAX_RX_BUF_ETHDATA_SIZE];

#ifdef USE_CHN_IO_BOARD
HUGEDATA COMM_BUF CommBufRFID;	 //RFID
HUGEDATA char CommTxBuf11Memory[COM_MAX_TX_BUF_RFID_SIZE];
HUGEDATA char CommRxBuf11Memory[COM_MAX_RX_BUF_RFID_SIZE];
HUGEDATA COMM_BUF CommBufGPRS;	 //GPRS
HUGEDATA char CommTxBuf12Memory[COM_MAX_TX_BUF_GPRS_SIZE];
HUGEDATA char CommRxBuf12Memory[COM_MAX_RX_BUF_GPRS_SIZE];
HUGEDATA COMM_BUF CommBufUsbMem;	 //USB Mem
HUGEDATA char CommTxBuf13Memory[COM_MAX_TX_BUF_USBMEM_SIZE];
HUGEDATA char CommRxBuf13Memory[COM_MAX_RX_BUF_USBMEM_SIZE];
#endif

_inline void Inline_PutCharTxBuf(HUGEDATA COMM_BUF *CBuf, char p)
{
	(*CBuf).TxBuf[(*CBuf).TxBufInPtr] = p;
	switch ((*CBuf).Type) {
		case COMM_TYPE_HOST:
			if ((*CBuf).TxBufInPtr >= (COM_MAX_TX_BUF_HOST_SIZE-1)) (*CBuf).TxBufInPtr = 0;
			else (*CBuf).TxBufInPtr++;
			break;
		case COMM_TYPE_AD:
			if ((*CBuf).TxBufInPtr >= (COM_MAX_TX_BUF_AD_SIZE-1)) (*CBuf).TxBufInPtr = 0;
			else (*CBuf).TxBufInPtr++;
			break;
		case COMM_TYPE_ETH:
			if ((*CBuf).TxBufInPtr >= (COM_MAX_TX_BUF_ETH_SIZE-1)) (*CBuf).TxBufInPtr = 0;
			else (*CBuf).TxBufInPtr++;
			break;
		case COMM_TYPE_PRT:
			if ((*CBuf).TxBufInPtr >= (COM_MAX_TX_BUF_PRT_SIZE-1)) (*CBuf).TxBufInPtr = 0;
			else (*CBuf).TxBufInPtr++;
			break;
#ifndef CPU_PIC32MZ
		case COMM_TYPE_KEY:
			if ((*CBuf).TxBufInPtr >= (COM_MAX_TX_BUF_KEY_SIZE-1)) (*CBuf).TxBufInPtr = 0;
			else (*CBuf).TxBufInPtr++;
			break;			
		case COMM_TYPE_USB:
			if ((*CBuf).TxBufInPtr >= (COM_MAX_TX_BUF_USB_SIZE-1)) (*CBuf).TxBufInPtr = 0;
			else (*CBuf).TxBufInPtr++;
			break;			
#endif
#ifdef USE_CHN_IO_BOARD
		case COMM_TYPE_RFID:
			if ((*CBuf).TxBufInPtr >= (COM_MAX_TX_BUF_RFID_SIZE-1)) (*CBuf).TxBufInPtr = 0;
			else (*CBuf).TxBufInPtr++;
			break;
		case COMM_TYPE_GPRS:
			if ((*CBuf).TxBufInPtr >= (COM_MAX_TX_BUF_GPRS_SIZE-1)) (*CBuf).TxBufInPtr = 0;
			else (*CBuf).TxBufInPtr++;
			break;
		case COMM_TYPE_USBMEM:
			if ((*CBuf).TxBufInPtr >= (COM_MAX_TX_BUF_USBMEM_SIZE-1)) (*CBuf).TxBufInPtr = 0;
			else (*CBuf).TxBufInPtr++;
			break;
#endif
			//HYP 20041213
		case COMM_TYPE_ETHDATA:
			if ((*CBuf).TxBufInPtr >= (COM_MAX_TX_BUF_ETHDATA_SIZE-1)) (*CBuf).TxBufInPtr = 0;
			else (*CBuf).TxBufInPtr++;
			break;
	}
}


void CommBufInit(HUGEDATA COMM_BUF *CBuf, INT8U type, INT8U num)
{
	(*CBuf).Txing = 0;
	(*CBuf).Rxing = 0;
	(*CBuf).TxBufOutPtr = 0;
	(*CBuf).TxBufInPtr = (*CBuf).TxBufOutPtr;
	(*CBuf).RxBufOutPtr = 0;
	(*CBuf).RxBufInPtr = (*CBuf).RxBufOutPtr;
	switch (type) {
		case COMM_TYPE_HOST:
			(*CBuf).Type = 1;
			(*CBuf).TxBuf = CommTxBuf1Memory;
			(*CBuf).RxBuf = CommRxBuf1Memory;
			break;
		case COMM_TYPE_AD:
			(*CBuf).Type = 2;
			(*CBuf).TxBuf = CommTxBuf2Memory;
			(*CBuf).RxBuf = CommRxBuf2Memory;
			break;
		case COMM_TYPE_ETH:
			(*CBuf).Type = 3;
			(*CBuf).TxBuf = CommTxBuf3Memory;
			(*CBuf).RxBuf = CommRxBuf3Memory;
			break;
		case COMM_TYPE_PRT:
			(*CBuf).Type = 4;
			(*CBuf).TxBuf = CommTxBuf4Memory;
			(*CBuf).RxBuf = CommRxBuf4Memory;
			break;
#ifndef CPU_PIC32MZ
		case COMM_TYPE_KEY:
			(*CBuf).Type = 5;
			(*CBuf).TxBuf = CommTxBuf5Memory;
			(*CBuf).RxBuf = CommRxBuf5Memory;
			break;
		case COMM_TYPE_USB:
			(*CBuf).Type = 6;
			(*CBuf).TxBuf = CommTxBuf6Memory;
			(*CBuf).RxBuf = CommRxBuf6Memory;
			break;
#endif
#ifdef USE_CHN_IO_BOARD
		case COMM_TYPE_RFID:
			(*CBuf).Type = 11;
			(*CBuf).TxBuf = CommTxBuf11Memory;
			(*CBuf).RxBuf = CommRxBuf11Memory;
			break;
		case COMM_TYPE_GPRS:
			(*CBuf).Type = 12;
			(*CBuf).TxBuf = CommTxBuf12Memory;
			(*CBuf).RxBuf = CommRxBuf12Memory;
			break;
		case COMM_TYPE_USBMEM:
			(*CBuf).Type = 13;
			(*CBuf).TxBuf = CommTxBuf13Memory;
			(*CBuf).RxBuf = CommRxBuf13Memory;
			break;
#endif
			//HYP 20041213
		case COMM_TYPE_ETHDATA:
			(*CBuf).Type = 10;
			(*CBuf).TxBuf = &(CommTxBuf10Memory[num][0]);
			(*CBuf).RxBuf = &(CommRxBuf10Memory[num][0]);
			(*CBuf).SocketID=num;//0=client, 1~max=server
			break;
	}
}

void FlushTxBuf(HUGEDATA COMM_BUF *CBuf)
{
	(*CBuf).Txing = 0;
	(*CBuf).TxBufInPtr = 0;
	(*CBuf).TxBufInPtr = (*CBuf).TxBufOutPtr;
}

void FlushRxBuf(HUGEDATA COMM_BUF *CBuf)
{
	(*CBuf).Rxing = 0;
	(*CBuf).RxBufInPtr = 0;
	(*CBuf).RxBufInPtr = (*CBuf).RxBufOutPtr;
}

void PutCStrTxBuf(HUGEDATA COMM_BUF *CBuf, const char *p)
{
  ROMDATA char *s;

    s = (ROMDATA char*)p;
    while (*s != '\0') {
	    Inline_PutCharTxBuf(CBuf, *s);
        s++;
    }
}

void PutStrTxBuf(HUGEDATA COMM_BUF *CBuf, HUGEDATA char *p)
{
    while (*p != '\0') {
	    Inline_PutCharTxBuf(CBuf, *p);
        p++;
    }
}

char GetCharTxBuf(HUGEDATA COMM_BUF *CBuf)
{
	char c;
	
	c = (*CBuf).TxBuf[(*CBuf).TxBufOutPtr];
	switch ((*CBuf).Type) {
		case COMM_TYPE_HOST:
			if ((*CBuf).TxBufOutPtr >= (COM_MAX_TX_BUF_HOST_SIZE-1)) (*CBuf).TxBufOutPtr = 0;
			else (*CBuf).TxBufOutPtr++;
			break;
		case COMM_TYPE_AD:
			if ((*CBuf).TxBufOutPtr >= (COM_MAX_TX_BUF_AD_SIZE-1)) (*CBuf).TxBufOutPtr = 0;
			else (*CBuf).TxBufOutPtr++;
			break;
		case COMM_TYPE_ETH:
			if ((*CBuf).TxBufOutPtr >= (COM_MAX_TX_BUF_ETH_SIZE-1)) (*CBuf).TxBufOutPtr = 0;
			else (*CBuf).TxBufOutPtr++;
			break;
		case COMM_TYPE_PRT:
			if ((*CBuf).TxBufOutPtr >= (COM_MAX_TX_BUF_PRT_SIZE-1)) (*CBuf).TxBufOutPtr = 0;
			else (*CBuf).TxBufOutPtr++;
			break;
#ifndef CPU_PIC32MZ
		case COMM_TYPE_KEY:
			if ((*CBuf).TxBufOutPtr >= (COM_MAX_TX_BUF_KEY_SIZE-1)) (*CBuf).TxBufOutPtr = 0;
			else (*CBuf).TxBufOutPtr++;
			break;
		case COMM_TYPE_USB:
			if ((*CBuf).TxBufOutPtr >= (COM_MAX_TX_BUF_USB_SIZE-1)) (*CBuf).TxBufOutPtr = 0;
			else (*CBuf).TxBufOutPtr++;
			break;
#endif
#ifdef USE_CHN_IO_BOARD
		case COMM_TYPE_RFID:
			if ((*CBuf).TxBufOutPtr >= (COM_MAX_TX_BUF_RFID_SIZE-1)) (*CBuf).TxBufOutPtr = 0;
			else (*CBuf).TxBufOutPtr++;
			break;
		case COMM_TYPE_GPRS:
			if ((*CBuf).TxBufOutPtr >= (COM_MAX_TX_BUF_GPRS_SIZE-1)) (*CBuf).TxBufOutPtr = 0;
			else (*CBuf).TxBufOutPtr++;
			break;
		case COMM_TYPE_USBMEM:
			if ((*CBuf).TxBufOutPtr >= (COM_MAX_TX_BUF_USBMEM_SIZE-1)) (*CBuf).TxBufOutPtr = 0;
			else (*CBuf).TxBufOutPtr++;
			break;
#endif
		//HYP 20041213
		case COMM_TYPE_ETHDATA:
			if ((*CBuf).TxBufOutPtr >= (COM_MAX_TX_BUF_ETHDATA_SIZE-1)) (*CBuf).TxBufOutPtr = 0;
			else (*CBuf).TxBufOutPtr++;
			break;
	}
	return c;
}

void RewindTxBuf(HUGEDATA COMM_BUF *CBuf)
{
	switch ((*CBuf).Type) {
		case COMM_TYPE_HOST:
			if ((*CBuf).TxBufOutPtr <= 0) (*CBuf).TxBufOutPtr = COM_MAX_TX_BUF_HOST_SIZE-1;
			else (*CBuf).TxBufOutPtr--;
			break;
		case COMM_TYPE_AD:
			if ((*CBuf).TxBufOutPtr <= 0) (*CBuf).TxBufOutPtr = COM_MAX_TX_BUF_AD_SIZE-1;
			else (*CBuf).TxBufOutPtr--;
			break;
		case COMM_TYPE_ETH:
			if ((*CBuf).TxBufOutPtr <= 0) (*CBuf).TxBufOutPtr = COM_MAX_TX_BUF_ETH_SIZE-1;
			else (*CBuf).TxBufOutPtr--;
			break;
		case COMM_TYPE_PRT:
			if ((*CBuf).TxBufOutPtr <= 0) (*CBuf).TxBufOutPtr = COM_MAX_TX_BUF_PRT_SIZE-1;
			else (*CBuf).TxBufOutPtr--;
			break;
#ifndef CPU_PIC32MZ
		case COMM_TYPE_KEY:
			if ((*CBuf).TxBufOutPtr <= 0) (*CBuf).TxBufOutPtr = COM_MAX_TX_BUF_KEY_SIZE-1;
			else (*CBuf).TxBufOutPtr--;
			break;
		case COMM_TYPE_USB:
			if ((*CBuf).TxBufOutPtr <= 0) (*CBuf).TxBufOutPtr = COM_MAX_TX_BUF_USB_SIZE-1;
			else (*CBuf).TxBufOutPtr--;
			break;
#endif
#ifdef USE_CHN_IO_BOARD
		case COMM_TYPE_RFID:
			if ((*CBuf).TxBufOutPtr <= 0) (*CBuf).TxBufOutPtr = COM_MAX_TX_BUF_RFID_SIZE-1;
			else (*CBuf).TxBufOutPtr--;
			break;
		case COMM_TYPE_GPRS:
			if ((*CBuf).TxBufOutPtr <= 0) (*CBuf).TxBufOutPtr = COM_MAX_TX_BUF_GPRS_SIZE-1;
			else (*CBuf).TxBufOutPtr--;
			break;
		case COMM_TYPE_USBMEM:
			if ((*CBuf).TxBufOutPtr <= 0) (*CBuf).TxBufOutPtr = COM_MAX_TX_BUF_USBMEM_SIZE-1;
			else (*CBuf).TxBufOutPtr--;
			break;
#endif
		case COMM_TYPE_ETHDATA:
			if ((*CBuf).TxBufOutPtr <= 0) (*CBuf).TxBufOutPtr = COM_MAX_TX_BUF_ETHDATA_SIZE-1;
			else (*CBuf).TxBufOutPtr--;
			break;
	}
}

BOOLEAN CheckTxBuf(HUGEDATA COMM_BUF *CBuf)
{
	if ((*CBuf).TxBufInPtr != (*CBuf).TxBufOutPtr)	{
		return TRUE;
	}
	return FALSE;
}

BOOLEAN CheckRxBuf(HUGEDATA COMM_BUF *CBuf)
{
	if ((*CBuf).RxBufInPtr != (*CBuf).RxBufOutPtr) {
		return TRUE;
	} 
	return FALSE;
}

//BOOLEAN CheckRxBufCount(HUGEDATA COMM_BUF *CBuf, INT16U count)
INT16S CheckRxBufCountInt(HUGEDATA COMM_BUF *CBuf)
{
	INT16S sz;
	if ((*CBuf).RxBufInPtr > (*CBuf).RxBufOutPtr) {
		return ((*CBuf).RxBufInPtr - (*CBuf).RxBufOutPtr);
	} else if ((*CBuf).RxBufInPtr < (*CBuf).RxBufOutPtr) {
		switch ((*CBuf).Type) {
			case COMM_TYPE_HOST: sz = COM_MAX_RX_BUF_HOST_SIZE; break;
			case COMM_TYPE_AD  : sz = COM_MAX_RX_BUF_AD_SIZE;   break;
			case COMM_TYPE_ETH : sz = COM_MAX_RX_BUF_ETH_SIZE;  break;
			case COMM_TYPE_PRT : sz = COM_MAX_RX_BUF_PRT_SIZE;  break;
#ifndef CPU_PIC32MZ
			case COMM_TYPE_KEY : sz = COM_MAX_RX_BUF_KEY_SIZE;  break;
			case COMM_TYPE_USB : sz = COM_MAX_RX_BUF_USB_SIZE;  break;
#endif
#ifdef USE_CHN_IO_BOARD
			case COMM_TYPE_RFID: sz = COM_MAX_RX_BUF_RFID_SIZE;  break;
			case COMM_TYPE_GPRS: sz = COM_MAX_RX_BUF_GPRS_SIZE;  break;
			case COMM_TYPE_USBMEM: sz = COM_MAX_RX_BUF_USBMEM_SIZE;  break;
#endif
			case COMM_TYPE_ETHDATA: sz = COM_MAX_RX_BUF_ETHDATA_SIZE; break;
		}
		return ((*CBuf).RxBufInPtr + sz - (*CBuf).RxBufOutPtr);
	} 
	return 0;
}

BOOLEAN CheckRxBufCount(HUGEDATA COMM_BUF *CBuf, INT16U count)
{
	if ((*CBuf).RxBufInPtr > (*CBuf).RxBufOutPtr) {
		if(((*CBuf).RxBufInPtr - (*CBuf).RxBufOutPtr) >= count) {
			return TRUE;
		} else {
			return FALSE;
		}
	} else if ((*CBuf).RxBufInPtr < (*CBuf).RxBufOutPtr) {
		switch ((*CBuf).Type) {
			case COMM_TYPE_HOST:
				if(((*CBuf).RxBufInPtr + COM_MAX_RX_BUF_HOST_SIZE - (*CBuf).RxBufOutPtr) >= count) {
					return TRUE;
				} else {
					return FALSE;
				}		
			case COMM_TYPE_AD:
				if(((*CBuf).RxBufInPtr + COM_MAX_RX_BUF_AD_SIZE - (*CBuf).RxBufOutPtr) >= count) {
					return TRUE;
				} else {
					return FALSE;
				}		
			case COMM_TYPE_ETH:
				if(((*CBuf).RxBufInPtr + COM_MAX_RX_BUF_ETH_SIZE - (*CBuf).RxBufOutPtr) >= count) {
					return TRUE;
				} else {
					return FALSE;
				}		
			case COMM_TYPE_PRT:
				if(((*CBuf).RxBufInPtr + COM_MAX_RX_BUF_PRT_SIZE - (*CBuf).RxBufOutPtr) >= count) {
					return TRUE;
				} else {
					return FALSE;
				}
#ifndef CPU_PIC32MZ
			case COMM_TYPE_KEY:
				if(((*CBuf).RxBufInPtr + COM_MAX_RX_BUF_KEY_SIZE - (*CBuf).RxBufOutPtr) >= count) {
					return TRUE;
				} else {
					return FALSE;
				}
			case COMM_TYPE_USB:
				if(((*CBuf).RxBufInPtr + COM_MAX_RX_BUF_USB_SIZE - (*CBuf).RxBufOutPtr) >= count) {
					return TRUE;
				} else {
					return FALSE;
				}
#endif
#ifdef USE_CHN_IO_BOARD
			case COMM_TYPE_RFID:
				if(((*CBuf).RxBufInPtr + COM_MAX_RX_BUF_RFID_SIZE - (*CBuf).RxBufOutPtr) >= count) {
					return TRUE;
				} else {
					return FALSE;
				}
			case COMM_TYPE_GPRS:
				if(((*CBuf).RxBufInPtr + COM_MAX_RX_BUF_GPRS_SIZE - (*CBuf).RxBufOutPtr) >= count) {
					return TRUE;
				} else {
					return FALSE;
				}
			case COMM_TYPE_USBMEM:
				if(((*CBuf).RxBufInPtr + COM_MAX_RX_BUF_USBMEM_SIZE - (*CBuf).RxBufOutPtr) >= count) {
					return TRUE;
				} else {
					return FALSE;
				}
#endif
			//HYP 20041213
			case COMM_TYPE_ETHDATA:
				if(((*CBuf).RxBufInPtr + COM_MAX_RX_BUF_ETHDATA_SIZE - (*CBuf).RxBufOutPtr) >= count) {
					return TRUE;
				} else {
					return FALSE;
				}						
		}
	} 
	return FALSE;
}


INT16U CheckRxBufChars(HUGEDATA COMM_BUF *CBuf)
{
	if ((*CBuf).RxBufInPtr > (*CBuf).RxBufOutPtr) {
		return (*CBuf).RxBufInPtr - (*CBuf).RxBufOutPtr;
	} else if ((*CBuf).RxBufInPtr < (*CBuf).RxBufOutPtr) {
		switch ((*CBuf).Type) {
			case COMM_TYPE_HOST:
				return (*CBuf).RxBufInPtr + COM_MAX_RX_BUF_HOST_SIZE - (*CBuf).RxBufOutPtr;		
			case COMM_TYPE_AD:
				return (*CBuf).RxBufInPtr + COM_MAX_RX_BUF_AD_SIZE - (*CBuf).RxBufOutPtr;		
			case COMM_TYPE_ETH:
				return (*CBuf).RxBufInPtr + COM_MAX_RX_BUF_ETH_SIZE - (*CBuf).RxBufOutPtr;		
			case COMM_TYPE_PRT:
				return (*CBuf).RxBufInPtr + COM_MAX_RX_BUF_PRT_SIZE - (*CBuf).RxBufOutPtr;
#ifndef CPU_PIC32MZ
			case COMM_TYPE_KEY:
				return (*CBuf).RxBufInPtr + COM_MAX_RX_BUF_KEY_SIZE - (*CBuf).RxBufOutPtr;
			case COMM_TYPE_USB:
				return (*CBuf).RxBufInPtr + COM_MAX_RX_BUF_USB_SIZE - (*CBuf).RxBufOutPtr;
#endif
#ifdef USE_CHN_IO_BOARD
			case COMM_TYPE_RFID:
				return (*CBuf).RxBufInPtr + COM_MAX_RX_BUF_RFID_SIZE - (*CBuf).RxBufOutPtr;
			case COMM_TYPE_GPRS:
				return (*CBuf).RxBufInPtr + COM_MAX_RX_BUF_GPRS_SIZE - (*CBuf).RxBufOutPtr;
			case COMM_TYPE_USBMEM:
				return (*CBuf).RxBufInPtr + COM_MAX_RX_BUF_USBMEM_SIZE - (*CBuf).RxBufOutPtr;
#endif
			//HYP 20041213
			case COMM_TYPE_ETHDATA:
				return (*CBuf).RxBufInPtr + COM_MAX_RX_BUF_ETHDATA_SIZE - (*CBuf).RxBufOutPtr;						
		}
	} 
	return 0;
}
//HYP 20041213
INT16U CheckTxBufChars(HUGEDATA COMM_BUF *CBuf)
{
	if ((*CBuf).TxBufInPtr > (*CBuf).TxBufOutPtr) {
		return (*CBuf).TxBufInPtr - (*CBuf).TxBufOutPtr;
	} else if ((*CBuf).TxBufInPtr < (*CBuf).TxBufOutPtr) {
		switch ((*CBuf).Type) {
			case COMM_TYPE_HOST:
				return (*CBuf).TxBufInPtr + COM_MAX_TX_BUF_HOST_SIZE - (*CBuf).TxBufOutPtr;		
			case COMM_TYPE_AD:
				return (*CBuf).TxBufInPtr + COM_MAX_TX_BUF_AD_SIZE - (*CBuf).TxBufOutPtr;		
			case COMM_TYPE_ETH:
				return (*CBuf).TxBufInPtr + COM_MAX_TX_BUF_ETH_SIZE - (*CBuf).TxBufOutPtr;		
			case COMM_TYPE_PRT:
				return (*CBuf).TxBufInPtr + COM_MAX_TX_BUF_PRT_SIZE - (*CBuf).TxBufOutPtr;
#ifndef CPU_PIC32MZ
			case COMM_TYPE_KEY:
				return (*CBuf).TxBufInPtr + COM_MAX_TX_BUF_KEY_SIZE - (*CBuf).TxBufOutPtr;
			case COMM_TYPE_USB:
				return (*CBuf).TxBufInPtr + COM_MAX_TX_BUF_USB_SIZE - (*CBuf).TxBufOutPtr;
#endif
#ifdef USE_CHN_IO_BOARD
			case COMM_TYPE_RFID:
				return (*CBuf).TxBufInPtr + COM_MAX_TX_BUF_RFID_SIZE - (*CBuf).TxBufOutPtr;
			case COMM_TYPE_GPRS:
				return (*CBuf).TxBufInPtr + COM_MAX_TX_BUF_GPRS_SIZE - (*CBuf).TxBufOutPtr;
			case COMM_TYPE_USBMEM:
				return (*CBuf).TxBufInPtr + COM_MAX_TX_BUF_USBMEM_SIZE - (*CBuf).TxBufOutPtr;
#endif
			//HYP 20041213
			case COMM_TYPE_ETHDATA:
				return (*CBuf).TxBufInPtr + COM_MAX_TX_BUF_ETHDATA_SIZE - (*CBuf).TxBufOutPtr;						
		}
	} 
	return 0;
}

char GetCharRxBuf(HUGEDATA COMM_BUF *CBuf)
{
	char c;

	c = (*CBuf).RxBuf[(*CBuf).RxBufOutPtr];
	switch ((*CBuf).Type) {
		case COMM_TYPE_HOST:
			if ((*CBuf).RxBufOutPtr >= (COM_MAX_RX_BUF_HOST_SIZE-1)) (*CBuf).RxBufOutPtr = 0;
			else (*CBuf).RxBufOutPtr++;
			break;
		case COMM_TYPE_AD:
			if ((*CBuf).RxBufOutPtr >= (COM_MAX_RX_BUF_AD_SIZE-1)) (*CBuf).RxBufOutPtr = 0;
			else (*CBuf).RxBufOutPtr++;
			break;
		case COMM_TYPE_ETH:
			if ((*CBuf).RxBufOutPtr >= (COM_MAX_RX_BUF_ETH_SIZE-1)) (*CBuf).RxBufOutPtr = 0;
			else (*CBuf).RxBufOutPtr++;
			break;
		case COMM_TYPE_PRT:
			if ((*CBuf).RxBufOutPtr >= (COM_MAX_RX_BUF_PRT_SIZE-1)) (*CBuf).RxBufOutPtr = 0;
			else (*CBuf).RxBufOutPtr++;
			break;
#ifndef CPU_PIC32MZ
		case COMM_TYPE_KEY:
			if ((*CBuf).RxBufOutPtr >= (COM_MAX_RX_BUF_KEY_SIZE-1)) (*CBuf).RxBufOutPtr = 0;
			else (*CBuf).RxBufOutPtr++;
			break;
		case COMM_TYPE_USB:
			if ((*CBuf).RxBufOutPtr >= (COM_MAX_RX_BUF_USB_SIZE-1)) (*CBuf).RxBufOutPtr = 0;
			else (*CBuf).RxBufOutPtr++;
			break;
#endif
#ifdef USE_CHN_IO_BOARD
		case COMM_TYPE_RFID:
			if ((*CBuf).RxBufOutPtr >= (COM_MAX_RX_BUF_RFID_SIZE-1)) (*CBuf).RxBufOutPtr = 0;
			else (*CBuf).RxBufOutPtr++;
			break;
		case COMM_TYPE_GPRS:
			if ((*CBuf).RxBufOutPtr >= (COM_MAX_RX_BUF_GPRS_SIZE-1)) (*CBuf).RxBufOutPtr = 0;
			else (*CBuf).RxBufOutPtr++;
			break;
		case COMM_TYPE_USBMEM:
			if ((*CBuf).RxBufOutPtr >= (COM_MAX_RX_BUF_USBMEM_SIZE-1)) (*CBuf).RxBufOutPtr = 0;
			else (*CBuf).RxBufOutPtr++;
			break;
#endif
		case COMM_TYPE_ETHDATA:
			if ((*CBuf).RxBufOutPtr >= (COM_MAX_RX_BUF_ETHDATA_SIZE-1)) (*CBuf).RxBufOutPtr = 0;
			else (*CBuf).RxBufOutPtr++;
			break;
	}
	return c;
}

void PutCharRxBuf(HUGEDATA COMM_BUF *CBuf, char c)
{
	(*CBuf).RxBuf[(*CBuf).RxBufInPtr] = c;
	switch ((*CBuf).Type) {
		case COMM_TYPE_HOST:
			if ((*CBuf).RxBufInPtr >= (COM_MAX_RX_BUF_HOST_SIZE-1)) (*CBuf).RxBufInPtr = 0;
			else (*CBuf).RxBufInPtr++;
			break;
		case COMM_TYPE_AD:
			if ((*CBuf).RxBufInPtr >= (COM_MAX_RX_BUF_AD_SIZE-1)) (*CBuf).RxBufInPtr = 0;
			else (*CBuf).RxBufInPtr++;
			break;
		case COMM_TYPE_ETH:
			if ((*CBuf).RxBufInPtr >= (COM_MAX_RX_BUF_ETH_SIZE-1)) (*CBuf).RxBufInPtr = 0;
			else (*CBuf).RxBufInPtr++;
			break;
		case COMM_TYPE_PRT:
			if ((*CBuf).RxBufInPtr >= (COM_MAX_RX_BUF_PRT_SIZE-1)) (*CBuf).RxBufInPtr = 0;
			else (*CBuf).RxBufInPtr++;
			break;
#ifndef CPU_PIC32MZ
		case COMM_TYPE_KEY:
			if ((*CBuf).RxBufInPtr >= (COM_MAX_RX_BUF_KEY_SIZE-1)) (*CBuf).RxBufInPtr = 0;
			else (*CBuf).RxBufInPtr++;
			break;
		case COMM_TYPE_USB:
			if ((*CBuf).RxBufInPtr >= (COM_MAX_RX_BUF_USB_SIZE-1)) (*CBuf).RxBufInPtr = 0;
			else (*CBuf).RxBufInPtr++;
			break;			
#endif
#ifdef USE_CHN_IO_BOARD
		case COMM_TYPE_RFID:
			if ((*CBuf).RxBufInPtr >= (COM_MAX_RX_BUF_RFID_SIZE-1)) (*CBuf).RxBufInPtr = 0;
			else (*CBuf).RxBufInPtr++;
			break;
		case COMM_TYPE_GPRS:
			if ((*CBuf).RxBufInPtr >= (COM_MAX_RX_BUF_GPRS_SIZE-1)) (*CBuf).RxBufInPtr = 0;
			else (*CBuf).RxBufInPtr++;
			break;
		case COMM_TYPE_USBMEM:
			if ((*CBuf).RxBufInPtr >= (COM_MAX_RX_BUF_USBMEM_SIZE-1)) (*CBuf).RxBufInPtr = 0;
			else (*CBuf).RxBufInPtr++;
			break;
#endif
		case COMM_TYPE_ETHDATA:
			if ((*CBuf).RxBufInPtr >= (COM_MAX_RX_BUF_ETHDATA_SIZE-1)) (*CBuf).RxBufInPtr = 0;
			else (*CBuf).RxBufInPtr++;
			break;
	}
}

void PutDataRxBuf(HUGEDATA COMM_BUF *CBuf, HUGEDATA char *p, INT16U count)
{
    while (count != 0) {
	PutCharRxBuf(CBuf, *p);
        count--;
	p++;
    }
}

void PutDataTxBuf(HUGEDATA COMM_BUF *CBuf, HUGEDATA char *p, INT16U count)
{
    while (count != 0) {
	Inline_PutCharTxBuf(CBuf, *p);
	count--;
	p++;
    }
}

void GetDataTxBuf(HUGEDATA COMM_BUF *CBuf, HUGEDATA char *p, INT16U count)
{
    while (count != 0) {
	    *p = GetCharTxBuf(CBuf);
	    count--;
	    p++;
    }
}

#ifdef USE_PROTOCOL_CL5000JR
void GetDataRxBuf(HUGEDATA COMM_BUF *CBuf, char *data, INT16U size)
{
	INT16U dataSize;
	INT16U maxRxBufSize;
		
	switch ((*CBuf).Type) {
		case COMM_TYPE_HOST:
			maxRxBufSize = COM_MAX_RX_BUF_HOST_SIZE;
			break;
#ifndef CPU_PIC32MZ
		case COMM_TYPE_USB:
			maxRxBufSize = COM_MAX_RX_BUF_USB_SIZE;
			break;			
#endif
		case COMM_TYPE_ETHDATA:
			maxRxBufSize = COM_MAX_RX_BUF_ETHDATA_SIZE;
			break;
		default:
			return;
	}
	
	if ((CBuf->RxBufOutPtr + size) > maxRxBufSize)
	{
		dataSize = maxRxBufSize - CBuf->RxBufOutPtr;
		memcpy(data, &CBuf->RxBuf[CBuf->RxBufOutPtr], dataSize);
		memcpy(&data[dataSize], CBuf->RxBuf, size - dataSize);
	}
	else
	{
		memcpy(data, &CBuf->RxBuf[CBuf->RxBufOutPtr], size);
	}
}
#endif//#ifdef USE_PROTOCOL_CL5000JR

//Use only COMM_TYPE_ETHDATA 
void GetDataEthDataTxBuf(HUGEDATA COMM_BUF *CBuf, HUGEDATA char *p, INT16U count) 
{
	INT16U i;
//	INT8U sum;

//	sum = 0;
	for (i=0; i<count; i++) {
		*p = (*CBuf).TxBuf[(*CBuf).TxBufOutPtr];
		p++;
		if ((*CBuf).TxBufOutPtr >= (COM_MAX_TX_BUF_ETHDATA_SIZE-1)) (*CBuf).TxBufOutPtr = 0;
		else (*CBuf).TxBufOutPtr++;
	}
}

//Use only COMM_TYPE_ETHDATA
void GetDataEthDataRxBuf(HUGEDATA COMM_BUF *CBuf, HUGEDATA char *p, INT16U count)
{
	INT16U i;
//	INT8U sum;

//	sum = 0;
	for (i=0; i<count; i++) {
		*p = (*CBuf).RxBuf[(*CBuf).RxBufOutPtr];
		p++;
		if ((*CBuf).RxBufOutPtr >= (COM_MAX_RX_BUF_ETHDATA_SIZE-1)) (*CBuf).RxBufOutPtr = 0;
		else (*CBuf).RxBufOutPtr++;
	}
}

//Use only COMM_TYPE_ETHDATA 
INT8U PutDataEthTxBuf(HUGEDATA COMM_BUF *CBuf,INT16U count) 
{
	INT16U i;
	INT8U sum;
	INT8U byte;

	sum = 0;
	for (i=0; i<count; i++) {
//		byte = GetCharTxBuf(CBuf);
////////////////HYP 20051012
		byte = (*CBuf).TxBuf[(*CBuf).TxBufOutPtr];
		if ((*CBuf).TxBufOutPtr >= (COM_MAX_TX_BUF_ETHDATA_SIZE-1)) (*CBuf).TxBufOutPtr = 0;
		else (*CBuf).TxBufOutPtr++;
/////////////////////////////////////////////////////////////////
		sum = sum + byte;
		CommBufEth.TxBuf[CommBufEth.TxBufInPtr] = byte;
		if (CommBufEth.TxBufInPtr >= (COM_MAX_TX_BUF_ETH_SIZE-1)) CommBufEth.TxBufInPtr = 0;
		else CommBufEth.TxBufInPtr++;
	}
	return sum;
}

//Use only COMM_TYPE_ETHDATA 
INT8U GetDataEthRxBuf(HUGEDATA COMM_BUF *CBuf,INT16U count) 
{
	INT16U i;
	INT8U sum;
	INT8U byte;
		       
	sum = 0;
	for (i=0; i<count; i++) {
		byte = CommBufEth.RxBuf[CommBufEth.RxBufOutPtr];
		if (CommBufEth.RxBufOutPtr >= (COM_MAX_RX_BUF_ETH_SIZE-1)) CommBufEth.RxBufOutPtr = 0;
		else CommBufEth.RxBufOutPtr++;
		sum = sum + byte;
//	 	PutCharRxBuf(CBuf, byte);
//////////////////HYP 20051012
		(*CBuf).RxBuf[(*CBuf).RxBufInPtr] = byte;
		if ((*CBuf).RxBufInPtr >= (COM_MAX_RX_BUF_ETHDATA_SIZE-1)) (*CBuf).RxBufInPtr = 0;
		else (*CBuf).RxBufInPtr++;
/////////////////////////////////////////////////////////////
 	}
	return sum;
}


void RewindRxBuf(HUGEDATA COMM_BUF *CBuf, short num)
{
	short i;

	for (i = 0; i<num; i++) {
		switch ((*CBuf).Type) {
			case COMM_TYPE_HOST:
				if ((*CBuf).RxBufOutPtr <= 0) (*CBuf).RxBufOutPtr = COM_MAX_RX_BUF_HOST_SIZE-1;
				else (*CBuf).RxBufOutPtr--;
				break;
			case COMM_TYPE_AD:
				if ((*CBuf).RxBufOutPtr <= 0) (*CBuf).RxBufOutPtr = COM_MAX_RX_BUF_AD_SIZE-1;
				else (*CBuf).RxBufOutPtr--;
				break;
			case COMM_TYPE_ETH:
				if ((*CBuf).RxBufOutPtr <= 0) (*CBuf).RxBufOutPtr = COM_MAX_RX_BUF_ETH_SIZE-1;
				else (*CBuf).RxBufOutPtr--;
				break;
			case COMM_TYPE_PRT:
				if ((*CBuf).RxBufOutPtr <= 0) (*CBuf).RxBufOutPtr = COM_MAX_RX_BUF_PRT_SIZE-1;
				else (*CBuf).RxBufOutPtr--;
				break;
#ifndef CPU_PIC32MZ
			case COMM_TYPE_KEY:
				if ((*CBuf).RxBufOutPtr <= 0) (*CBuf).RxBufOutPtr = COM_MAX_RX_BUF_KEY_SIZE-1;
				else (*CBuf).RxBufOutPtr--;
				break;
			case COMM_TYPE_USB:
				if ((*CBuf).RxBufOutPtr <= 0) (*CBuf).RxBufOutPtr = COM_MAX_RX_BUF_USB_SIZE-1;
				else (*CBuf).RxBufOutPtr--;
				break;
#endif
#ifdef USE_CHN_IO_BOARD
			case COMM_TYPE_RFID:
				if ((*CBuf).RxBufOutPtr <= 0) (*CBuf).RxBufOutPtr = COM_MAX_RX_BUF_RFID_SIZE-1;
				else (*CBuf).RxBufOutPtr--;
				break;
			case COMM_TYPE_GPRS:
				if ((*CBuf).RxBufOutPtr <= 0) (*CBuf).RxBufOutPtr = COM_MAX_RX_BUF_GPRS_SIZE-1;
				else (*CBuf).RxBufOutPtr--;
				break;
			case COMM_TYPE_USBMEM:
				if ((*CBuf).RxBufOutPtr <= 0) (*CBuf).RxBufOutPtr = COM_MAX_RX_BUF_USBMEM_SIZE-1;
				else (*CBuf).RxBufOutPtr--;
				break;
#endif
			case COMM_TYPE_ETHDATA:
				if ((*CBuf).RxBufOutPtr <= 0) (*CBuf).RxBufOutPtr = COM_MAX_RX_BUF_ETHDATA_SIZE-1;
				else (*CBuf).RxBufOutPtr--;
				break;
		}
	}
}

#ifdef USE_PROTOCOL_CL5000JR
void WindRxBuf(HUGEDATA COMM_BUF *CBuf, short num)
{
	INT16U maxRxBufSize;
		
	switch ((*CBuf).Type) {
		case COMM_TYPE_HOST:
			maxRxBufSize = COM_MAX_RX_BUF_HOST_SIZE;
			break;
#ifndef CPU_PIC32MZ
		case COMM_TYPE_USB:
			maxRxBufSize = COM_MAX_RX_BUF_USB_SIZE;
			break;			
#endif
		case COMM_TYPE_ETHDATA:
			maxRxBufSize = COM_MAX_RX_BUF_ETHDATA_SIZE;
			break;
		default:
			return;
	}
	
	CBuf->RxBufOutPtr += num;
	CBuf->RxBufOutPtr %= maxRxBufSize;
}
#else
void WindRxBuf(HUGEDATA COMM_BUF *CBuf, short num)
{
	short i;

	for (i = 0; i<num; i++) {
		GetCharRxBuf(CBuf);
	}
}
#endif//#ifdef USE_PROTOCOL_CL5000JR

void PutStrTxBuf_copy(HUGEDATA COMM_BUF *CBuf_dst, HUGEDATA COMM_BUF *CBuf_src)
{
    char ch;

    while (CheckRxBuf(CBuf_src)) {
	    ch = GetCharRxBuf(CBuf_src);
	    Inline_PutCharTxBuf(CBuf_dst, ch);
    }
}

void PutStrTx2Buf_copy(HUGEDATA COMM_BUF *CBuf_dst, HUGEDATA COMM_BUF *CBuf_src)
{
    char ch;

    while (CheckTxBuf(CBuf_src)) {
	    ch = GetCharTxBuf(CBuf_src); 
	    Inline_PutCharTxBuf(CBuf_dst, ch);
    }
}


void PutStrRxBuf_copy(HUGEDATA COMM_BUF *CBuf_dst, HUGEDATA COMM_BUF *CBuf_src)
{
    char ch;

    while (CheckRxBuf(CBuf_src)) {
	    ch = GetCharRxBuf(CBuf_src); 
	    PutCharRxBuf(CBuf_dst, ch);
    }
}	 

