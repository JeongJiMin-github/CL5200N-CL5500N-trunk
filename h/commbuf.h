/*****************************************************************************
|*			
|*  Copyright		:	(c) 2002 CAS
|*  Filename		:	commbuf.h
|*  Version			:	0.1
|*  Programmer(s)	:	Hong Yi-Phyo (HYP)
|*  Created			:	2002/12/10
|*  Description		:	CLP Printer communication buffer routine header    
|*				
****************************************************************************/
#ifndef COMMBUF_STRUCT_DEFINE
#define COMMBUF_STRUCT_DEFINE
#pragma pack(4)
typedef struct {
	INT8U	Type;

	HUGEDATA char *TxBuf;
	INT16U MaxTxBufSize;
	INT16U TxBufInPtr;
	INT16U TxBufOutPtr;
	BOOLEAN	Txing;

	HUGEDATA char *RxBuf;
	INT16U MaxRxBufSize;
	INT16U RxBufInPtr;
	INT16U RxBufOutPtr;
	BOOLEAN	Rxing;

	INT8U  SocketID; //0=client, 1~max=server
	INT8U  DestIP[4];
	INT16U DestPort;
} COMM_BUF;
#pragma pack()
#endif

#define _COMMBUF_DEFINE_ALL
#ifndef _COMMBUF_H
#define _COMMBUF_H
#ifdef USE_DMSG_2100_BYTE
#ifdef USE_LABEL_LENGTH_120MM
#define COM_MAX_TX_BUF_HOST_SIZE	1500
#define COM_MAX_RX_BUF_HOST_SIZE	1500
#else
#define COM_MAX_TX_BUF_HOST_SIZE	3000
#define COM_MAX_RX_BUF_HOST_SIZE	3000
#endif
#define COM_MAX_TX_BUF_AD_SIZE		100
#define COM_MAX_RX_BUF_AD_SIZE		100
#ifdef USE_LABEL_LENGTH_120MM
#define COM_MAX_TX_BUF_ETH_SIZE		2000
#define COM_MAX_RX_BUF_ETH_SIZE		3000
#else
#define COM_MAX_TX_BUF_ETH_SIZE		3000
#define COM_MAX_RX_BUF_ETH_SIZE		3000
#endif
#define COM_MAX_TX_BUF_PRT_SIZE		4100
#define COM_MAX_RX_BUF_PRT_SIZE		4100
#define COM_MAX_TX_BUF_KEY_SIZE		512
#define COM_MAX_RX_BUF_KEY_SIZE		512
#define COM_MAX_RX_BUF_USB_SIZE		1500//gm usb
#define COM_MAX_TX_BUF_USB_SIZE		1500//gm usb
#else
#define COM_MAX_TX_BUF_HOST_SIZE	1500
#define COM_MAX_RX_BUF_HOST_SIZE	1500
#define COM_MAX_TX_BUF_AD_SIZE		100
#define COM_MAX_RX_BUF_AD_SIZE		100
#define COM_MAX_TX_BUF_ETH_SIZE		2000
#define COM_MAX_RX_BUF_ETH_SIZE		2000
#define COM_MAX_TX_BUF_PRT_SIZE		4100
#define COM_MAX_RX_BUF_PRT_SIZE		4100
#define COM_MAX_TX_BUF_KEY_SIZE		512
#define COM_MAX_RX_BUF_KEY_SIZE		512
#define COM_MAX_RX_BUF_USB_SIZE		1500//gm usb
#define COM_MAX_TX_BUF_USB_SIZE		1500//gm usb
#endif
//HYP 20041213
#define COM_MAX_TX_BUF_ETHDATA_SIZE		1500
#define COM_MAX_RX_BUF_ETHDATA_SIZE		1500

#ifdef USE_CHN_IO_BOARD
#define COM_MAX_RX_BUF_RFID_SIZE	100
#define COM_MAX_TX_BUF_RFID_SIZE	100
#define COM_MAX_RX_BUF_GPRS_SIZE	100
#define COM_MAX_TX_BUF_GPRS_SIZE	100
#define COM_MAX_RX_BUF_USBMEM_SIZE	512
#define COM_MAX_TX_BUF_USBMEM_SIZE	512
#endif

#define COMM_TYPE_HOST	1
#define COMM_TYPE_AD	2
#define COMM_TYPE_ETH	3
#define COMM_TYPE_PRT	4
#define COMM_TYPE_KEY	5
#define COMM_TYPE_USB	6 //gm usb
//HYP 20041213
#define COMM_TYPE_ETHDATA	10
#ifdef USE_CHN_IO_BOARD
#define COMM_TYPE_RFID		11
#define COMM_TYPE_GPRS		12
#define COMM_TYPE_USBMEM	13
#endif

#define ETH_BUF_MAX_NUM		6	//ETH_DEFAULT_CLI_MAX_NUM_4_SRV+1, UDP buffer +1

extern HUGEDATA COMM_BUF CommBuf;  //Host
extern HUGEDATA COMM_BUF CommBufAD; //AD
extern HUGEDATA COMM_BUF CommBufEth; //parallel
#ifndef CPU_PIC32MZ
extern HUGEDATA COMM_BUF CommBufKey; //Key
extern HUGEDATA COMM_BUF CommBufUsb; //USB
#endif
extern HUGEDATA COMM_BUF CommBufPrt; //Printer
extern HUGEDATA COMM_BUF CommBufEthData[ETH_BUF_MAX_NUM];
#ifdef USE_CHN_IO_BOARD
extern HUGEDATA COMM_BUF CommBufRFID;
extern HUGEDATA COMM_BUF CommBufGPRS;
extern HUGEDATA COMM_BUF CommBufUsbMem;
#endif

extern void CommBufInit(HUGEDATA COMM_BUF *CBuf, INT8U type, INT8U num);
extern void FlushTxBuf(HUGEDATA COMM_BUF *CBuf);
extern void FlushRxBuf(HUGEDATA COMM_BUF *CBuf);
extern void PutCStrTxBuf(HUGEDATA COMM_BUF *CBuf, const char *p);
extern void PutStrTxBuf(HUGEDATA COMM_BUF *CBuf, HUGEDATA char *p);
extern void PutDataRxBuf(HUGEDATA COMM_BUF *CBuf, HUGEDATA char *p, INT16U count);
extern void PutDataTxBuf(HUGEDATA COMM_BUF *CBuf, HUGEDATA char *p, INT16U count);
extern void GetDataTxBuf(HUGEDATA COMM_BUF *CBuf, HUGEDATA char *p, INT16U count);
extern void GetDataEthDataTxBuf(HUGEDATA COMM_BUF *CBuf, HUGEDATA char *p, INT16U count);
extern void GetDataEthDataRxBuf(HUGEDATA COMM_BUF *CBuf, HUGEDATA char *p, INT16U count);
extern INT8U PutDataEthTxBuf(HUGEDATA COMM_BUF *CBuf,INT16U count);
extern INT8U GetDataEthRxBuf(HUGEDATA COMM_BUF *CBuf,INT16U count);
extern void PutCharRxBuf(HUGEDATA COMM_BUF *CBuf, char);
extern char GetCharTxBuf(HUGEDATA COMM_BUF *CBuf);
extern char GetCharRxBuf(HUGEDATA COMM_BUF *CBuf);
extern BOOLEAN CheckRxBuf(HUGEDATA COMM_BUF *CBuf);
extern BOOLEAN CheckTxBuf(HUGEDATA COMM_BUF *CBuf);
extern BOOLEAN CheckRxBufCount(HUGEDATA COMM_BUF *CBuf, INT16U count);
extern INT16U CheckRxBufChars(HUGEDATA COMM_BUF *CBuf);
//HYP 20041213
extern INT16U CheckTxBufChars(HUGEDATA COMM_BUF *CBuf);
extern void RewindRxBuf(HUGEDATA COMM_BUF *CBuf, short num);
extern void WindRxBuf(HUGEDATA COMM_BUF *CBuf, short num);
extern void RewindTxBuf(HUGEDATA COMM_BUF *CBuf);
extern void PutStrTxBuf_copy(HUGEDATA COMM_BUF *CBuf_dst, HUGEDATA COMM_BUF *CBuf_src);
extern void PutStrTx2Buf_copy(HUGEDATA COMM_BUF *CBuf_dst, HUGEDATA COMM_BUF *CBuf_src);
extern void PutStrRxBuf_copy(HUGEDATA COMM_BUF *CBuf_dst, HUGEDATA COMM_BUF *CBuf_src);
extern INT16S CheckRxBufCountInt(HUGEDATA COMM_BUF *CBuf);
#endif /* _COMMBUF_H */
