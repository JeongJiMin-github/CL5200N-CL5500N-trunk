
#define USBMEM_COM_NONE	0
#define USBMEM_COM_OK	1
#define USBMEM_COM_ERR_HEADER_COMMA	101
#define USBMEM_COM_ERR_HEADER_COLON	102
#define USBMEM_COM_ERR_TAIL_COLON	103
#define USBMEM_COM_ERR_CHECKSUM		104
#define USBMEM_COM_ERR_CR		105

extern INT8U usbmem_RcvBuffer[256];
extern INT8U usbmem_RcvCmd[2];
extern INT8U usbmem_RcvResult;

extern void usbmem_send_log_to_usb(HUGEDATA COMM_BUF *CBuf, INT16U tail, INT16U num);
extern INT8U usbmem_interpreter(HUGEDATA COMM_BUF *CBuf);

extern INT8U usbmem_save_report(INT8U *filename, INT8U *str);