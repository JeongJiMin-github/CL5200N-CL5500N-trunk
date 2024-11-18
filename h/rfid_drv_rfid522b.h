#ifdef USE_RFID_TAG_READER_RFID_522_B
// Use RFID_522_B(China)

#define RFID_CMD_READ_CARD_NO	0xA0
#define RFID_CMD_READ_DATA	0xA1
#define RFID_CMD_WRITE_DATA	0xA2
#define RFID_CMD_CHANGE_PASSWD	0xA3
#define RFID_CMD_BEEP_ON	0xA6
#define RFID_CMD_BEEP_OFF	0xA7
#define RFID_CMD_BEEP_ERR_ON	0xA8
#define RFID_CMD_BEEP_ERR_OFF	0xA9

#define RCV_OK            1
#define RCV_NO_START      2
#define RCV_DATA_INVALID  3
#define RCV_BCC_FAIL      4
#define RCV_LENGTH_ERR    5

extern INT8U rfid_default_password[6];
extern INT8U RFID_RcvBuffer[256];
extern INT8U RFID_RcvCmd;
extern INT8U RFID_RcvResult;
extern INT8U RFID_RcvState;

extern void commun_send_read_card_no(HUGEDATA COMM_BUF *CBuf);
extern void commun_send_read_data(HUGEDATA COMM_BUF *CBuf, INT8U *password);
extern void commun_send_write_data(HUGEDATA COMM_BUF *CBuf, INT8U *password, INT8U *wr_data);
extern void commun_send_change_password(HUGEDATA COMM_BUF *CBuf, INT8U *password, INT8U *new_password);
extern void commun_send_cmd(HUGEDATA COMM_BUF *CBuf, INT8U cmd, INT8U *wr_data1);
extern INT8U rfid_interpreter(HUGEDATA COMM_BUF *CBuf);

#endif