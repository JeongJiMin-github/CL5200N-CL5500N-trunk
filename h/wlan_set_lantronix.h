
#define WLTX_CMD_NODE_RESET	0x03
#define WLTX_CMD_EXT_VERSION	0xf4
#define WLTX_CMD_FW_VERSION	0xf6
#define WLTX_CMD_GET_CONFIG	0xf8
#define WLTX_CMD_SET_CONFIG	0xfa
#define WLTX_CMD_SET_IP		0xfc
#define WLTX_CMD_SET_CONFIG_A	0xfd
#define WLTX_CMD_GET_CONINFO 0xf1

#define WLTX_CMD_GET_CONFIG0	0xe0
#define WLTX_CMD_GET_CONFIG1	0xe1
#define WLTX_CMD_GET_CONFIG2	0xe2
#define WLTX_CMD_GET_CONFIG3	0xe3
#define WLTX_CMD_GET_CONFIG4	0xe4
#define WLTX_CMD_GET_CONFIG5	0xe5
#define WLTX_CMD_GET_CONFIG6	0xe6
#define WLTX_CMD_GET_CONFIG7	0xe7
#define WLTX_CMD_GET_CONFIG8	0xe8
#define WLTX_CMD_GET_CONFIG9	0xe9

#define WLTX_CMD_SET_CONFIG0	0xc0
#define WLTX_CMD_SET_CONFIG1	0xc1
#define WLTX_CMD_SET_CONFIG2	0xc2
#define WLTX_CMD_SET_CONFIG3	0xc3
#define WLTX_CMD_SET_CONFIG4	0xc4
#define WLTX_CMD_SET_CONFIG5	0xc5
#define WLTX_CMD_SET_CONFIG6	0xc6
#define WLTX_CMD_SET_CONFIG7	0xc7
#define WLTX_CMD_SET_CONFIG8	0xc8
#define WLTX_CMD_SET_CONFIG9	0xc9


#define WLAN_CONTROL_PORT_LANTRONIX	0x77fe
#define WLAN_CONTROL_PORT_TOBNET	0x78fe
#pragma pack(2)
typedef struct {
	INT8U IP[4]; 
	INT8U reserved; 
	INT8U flagByte;
	INT8U noHostBitsForSubnet;
	INT8U tcpKeepaliveValidRange;
	INT8U telnetConfigPswd[4];
	INT8U gatewayIP[4];
	INT8U ch1Parameter[48];
	INT8U ch2Parameter[48];
	INT8U DHCPDevName[8];
} WLAN_SETUP_RECORD0; //120 byte

typedef struct {
	INT8U reserved1;
	INT8U AESKey1[16];
	INT8U reserved2[32];
	INT8U AESKeyLength;
	INT8U AESKey2[16];
	INT8U reserved3[60];
} WLAN_SETUP_RECORD1; //126 byte

typedef struct {
	INT8U reserved1[77]; 
	INT8U speedNDuplex; 
	INT8U reserved2[48];
} WLAN_SETUP_RECORD3; //126 byte

typedef struct {
	INT8U reserved1[10]; 
	INT8U country; 
	INT8U reserved2[115];
} WLAN_SETUP_RECORD4; //126 byte

typedef struct {
	INT8U wifiSettings; 
	INT8U adHocSettings; 
	INT8U securitySettings;
	INT8U SSID[33];
	INT8U reserved1[33];
	INT8U groupEncryptionMethod;
	INT8U keyLength;
	INT8U keyType;
	INT8U txKeyIndex;
	INT8U roaming;
	INT8U reserved2[52];
} WLAN_SETUP_RECORD8; //126 byte

typedef struct {
	INT8U reserved1[7];
	INT8U EncryptionKey[64];
	INT8U reserved2[55];
} WLAN_SETUP_RECORD9; //126 byte

typedef struct {
	INT8U IP[4]; 
	INT8U reserved1[2];
	INT8U subnet[4]; 
	INT8U reserved2[2];
	INT8U gatewayIP[4]; 
	INT8U reserved3[104];
} WLAN_SETUP_RECORD0_TOBNET; //120 byte

typedef struct {
	INT8U securitySuite;
	INT8U authenticationMethod;
	INT8U cipherType;
	INT8U SSID[33];
	INT8U keyType;
	INT8U txKeyIndex;
	INT8U encryptionKey[64];
	INT8U operationMode;
	INT8U channel;
	INT8U reserved[22];
} WLAN_SETUP_RECORD8_TOBNET; //126 byte

typedef struct {
	INT8U fwversion[20];
	INT8U ssid[32];
	INT8U bssid[6];
	INT8U rsimac[6];
	INT8U rssiVal;
	INT8U connState;
	INT8U channel;
	INT8U reserved[59];	
} ap_info; //126byte

#pragma pack()


extern INT8U wlan_SetIP_Lantronix(INT8U *ip, INT8U *gateway, INT8U *subnet);
extern INT8U wlan_SetIP_Tobnet(INT8U *ip, INT8U *gateway, INT8U *subnet);
extern INT8U wlan_CheckIP_Tobnet(INT8U *ip, INT8U *gateway, INT8U *subnet);
extern INT8U wlan_SetEthernetSpeed_Lantronix(INT8U speed);
extern INT8U wlan_SetAESKey_Lantronix(INT8U keyLength, INT8U *aesKey);
extern INT8U wlan_SetWiFiSetting_Lantronix(INT8U *ssid, INT8U wep, INT8U keyIdx, INT8U keyLen, INT8U opmode);
extern INT8U wlan_SetWiFiSetting_Tobnet(INT8U *ssid, INT8U wep, INT8U keyIdx, INT8U keyLen, INT8U *key, INT8U opmode);
extern INT8U wlan_CheckWiFiSetting_Tobnet(INT8U *ssid, INT8U wep, INT8U keyIdx, INT8U keyLen, INT8U *key);
extern INT8U wlan_SetCountry_Lantronix(void);
extern INT8U wlan_SetEncriptionKey_Lantronix(INT8U *key);
extern INT8U wlan_CheckVer_Bridge(INT8U wtype, INT8U *ver_str, INT8U retry);
extern ap_info *wlan_Check_ConnInfo_Bridge(void);
extern void wlan_RebootModule(INT8U wtype);
extern void wlan_SetNetwork(INT8U *ip, INT8U *gw, INT8U *sn);

