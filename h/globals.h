/*****************************************************************************
|*			
|*  Copyright		:	(c) 2002 CAS
|*  Filename		:	globals.h
|*  Version			:	0.1
|*  Programmer(s)	:	Hong Yi-Phyo (HYP)
|*  Created			:	2002/11/11
|*  Description		:	CLP Printer global variable define    
|*				
****************************************************************************/
//�׽�Ʈ �귣ġ
#ifndef _GLOBALS_H
#define _GLOBALS_H

#include "app.h"

#define	_USE_SERIAL_   //use 232 port  only CLP Printer 

//////////////////////////////////////////////////////////////////////////
// Compile Condition START (Single choice in one conditon)
//////////////////////////////////////////////////////////////////////////
/*
********************************************************************************
* 			                   Compile Condition 1
* 			                   < Model Type >
********************************************************************************
*/
#define CL5200N_BP     //CL5200N-BP
//#define CL5500N_BPRH
//#define CL5500N_D
/********************************************************************************/


/*
********************************************************************************
* 			                   Compile Condition 2-1
* 			                   < Nation Define >
* 		< Dealer Code >
* 		Used Code : A     D E F G H I J K L M N   P Q R S T U V W X Y Z "TR" "SM" "PT" "DM" "TA" "SK" "KR" "SR" "PL" "LG" "VL" "GA"
* 		              a b c d e   g h i   k l m n o p   r s t u v    y z "sp" "kr"
* 		Remained Code :   B C                        O                    
* 		                              j         o    q             x 
********************************************************************************
*/

//------- ���� �߿��� ��� -------//
//#define USE_STANDARD							// ����

//------- ��Ÿ �߰����� ���� Ȯ�� �ʿ��� �߿��� -------//
//#define USE_ARAB								// (A) �ƶ� (��Ʈ)
//#define USE_EUROPE_WITHOUT_TICKET				// (u) Ƽ�ϱ���� ������� �ʴ� ���� ���� �߿���
//#define USE_RUSSIA							// (R) ���þ�

//------- ���� �߿��� (���ĺ�, �������� ����) -------//
//#define USE_DISCLOSURE_MEAT_PRICE 			// (g) ��깰 ������ (������ ��ü�̷� base)
//#define USE_EMART           					// (E) E-Mart - �̸�Ʈ�� ��� V3.03.7���� (E)������  (m)������ �����Ͽ� (E)���� �� ����
//#define USE_GSMART							// (G) GS ��Ʈ ����
//#define USE_GSMART_TRACE    					// (M) GS ��Ʈ ��� ����
//#define USE_HYUNDAI       					// (H) �����ȭ�� ����
#define USE_IMPORT_MEAT						// (I) ������ ��ü�̷� (������ ���� ��ü)
//#define USE_IMPORT_MEAT_8LINE					// (I8) ������ + ��ǰ�� 8�� ����
//#define USE_KOREA_CL5200     					// (k) CL5200 ���� ���� (�̷°��� ��� ����)
//#define USE_LOTTEMART       					// (J) Lotte Mart(���� ������ + GS1 DataBar Expand)
//#define USE_LOTTESUPER						// (U) Lotte Super(���� ���������� �Ļ�)
//#define USE_LOTTESUPER_8LINE 					// (W) Lotte Super ��ǰ�� 8��(����ī ���)
//#define USE_NHMART          					// (N) ���� ����
//#define USE_SCALE_POS       					// (T) ���ﳪ�� �̷����� => CL5200J���� ���ŵ�
//#define USE_SHINSEGAE_MEAT					// (S) �ż��� ��ȭ��
//#define USE_TOPMART							// (Y) ��������  TOP mart

//------- �ؿ� �߿��� (���ĺ� ����) -------//
//#define USE_ARAB_EMIRATES						// (a) �ƶ����̷���Ʈ
//#define USE_CANADA_LABEL						// (L) ĳ���� �� ���� ���(200mm)
//#define USE_CHINA_BAONENG 					// (X) CL5200N �߱� ���� ���� ��ü
//#define USE_CHINA_BAONENG_COUNTRYGARDEN 		// (w) CL5200N �߱� ���� Country Garden
//#define USE_CHINA_CARRFOUR	    			// (f) CL5200N �߱� Carrfour (����)
//#define USE_CHINA_HUARENWANGJI				// (d) CN5200N �߱� HUARENWANGJI (ȭ�����)
//#define USE_CHINESE_FONT_BIG5					// TR tranditional chinese (big5)
//#define USE_CHINESE_FONT_GB2312				// SM simple chinese (gb2312)
//#define USE_CHN_NEW_STANDARDMARKET			// (a) �߱� ǥ��ȭ����(2013������,���÷�����������) "/Branch/CarScale_RemoteDisplay/"
//#define USE_CHN_STANDARDMARKET				// (c) �߱�ǥ��ȭ����
//#define USE_CHN_TRACE_STANDARD				// (F) chinese trace standard (gb2312) <= chn E-Mart
//#define USE_GBR_HMART							// (P) ���� H-MART ����(�ѱ���Ʈ)
//#define USE_GEO_AGROHUB_MART					// GA �׷����� Agrohub ��Ʈ ���� ������ �Ļ�(���� Sell by Date ��� ����)
//#define USE_GLOBAL_KOREAN_MART    			// KR �ؿܿ� �ִ� ���� ��Ʈ ����. �⺻������ ���� ��� �߿����ε�, PLU �̸��� �ѱ۷� ǥ��, �޴� ���� ����� ǥ�� (���� ���)
//#define USE_HEBREW							// (h) ���긮�� (�̽���)
//#define USE_INDIA_DADUS						// DM �ε�(DADUS MITHAI VATIKA)
//#define USE_INDIA_TATA						// TA �ε� TATA Group ���� �߿���
//#define USE_INDONESIA_EMPORIUM				// (e) �ε��׽þ� �������� ����
//#define USE_MANAGE_INVENTORY					// (i) ���� ������ �� ���� ���.
//#define USE_MON_EMART							// (g) CL5500 ���� �̸�Ʈ
//#define USE_NINGBO_RETAIL_STANDARDMARKET		// (z) �߱��׺�ǥ��ȭ����(�Ҹű��) "/Branch/CarScale_Ningbo_Retail"
//#define USE_NINGBO_STANDARDMARKET				// (b) �߱��׺�ǥ��ȭ���� "/Branch/CarScale_Ningbo"
//#define USE_PERSIA							// (p) Persian(�̶�)
//#define USE_POLAND_LOGISTICS					// PL ������ ���� ����(LOGISTICS PLU)
//#define USE_POLAND_STOKROTKA_MART				// SK ������ STOKROTKA ü�θ�Ʈ ���� �߿��� (������ ���� ��� + ��Ʈ ���� �Ķ����, �޴� �⺻�� ����)
//#define USE_RSA_SPAR_MART         			// SR ���ư� SPAR ü�θ�Ʈ ���� ���
//#define USE_RUSSIA_INDIRECT					// (Q) ���þ�(Indirect Ingredient Version)
//#define USE_RUSSIA_LOGISTICS					// (l) ���þ� ���� ����
//#define USE_RUSSIA_SPAR         				// (sp) ���þ� SPAR ��Ʈ
//#define USE_SAUDI								// PT �ƶ���(����ƶ���)
//#define USE_SRILANKA							// (K) ������ī
//#define USE_TURKEY_QR_BARCODE					// (t) ��Ű ��û���� ���� (�Է��� URL�� QR ���ڵ�� ����ϴ� ���)
//#define USE_UKRAINE							// (Z) ��ũ���̳�
//#define USE_UN_KOR_MART						// kr �ؿܿ� �ִ� ���θ�Ʈ ����. �⺻������ ���� ���� ��� �߿����ε�, PLU �̸��� �ѱ۷� ǥ��, �޴� ���� ����� ǥ�� (���� ���� ���)
//#define USE_UN_LOGISTICS						// LG ���� standard_Logistics PLU ���� �߿��� (PLU 8,000��)
//#define USE_VIETNAM							// (V) ��Ʈ��
//#define USE_VIETNAM_EMART						// (v) �̸�Ʈ ��Ʈ��
//#define USE_VIETNAM_LOTTEMART					// VL �Ե���Ʈ ��Ʈ��

//------- ��� ���Ļ� �� �̻�� �߿��� -------//
//#define USE_BELARUS							// (r) ����罺 (2024�� ���� ���� ��� X )
//#define USE_CHINA_ALIBABA						// (l) CL5200N �˸��ٹ� ��ǰ��ü ���� (2024�� ���� ���� ��� X )
//#define USE_MANAGE_INVENTORY_CART				// (y) ���������� ��� �߰��� īƮ ���� ���� (2024�� ���� ���� ��� X)
//#define USE_EMART_CL5200     					// (m) CL5200 �̸�Ʈ (V3.03.7���� ��� ����) - �̸�Ʈ�� ��� V3.03.7���� (E)������  (m)������ �����Ͽ� (E)���� �� ����
//#define USE_NHMART_SAFE     					// (n) ���� �Ƚ� �ѿ�
//#define USE_IMPORT_MEAT_CART					// (s) ������ ���� ��ü(CART)(Pb Bat)
/********************************************************************************/


/*
********************************************************************************
* 			                   Compile Condition 2-2
* 			                   < Country Define >
* 							< ���� ���(USE_STANDARD) ���� ���� >
* 							  ������ ���� ������ ��� �ּ�ó��
********************************************************************************
*/
#ifdef USE_STANDARD					//����
/********************************************************************************/

/********************************************************************************/
//#define COUNTRY_AUSTRALIA			// ȣ��
//#define COUNTRY_AZERBAIJAN		// ������������
//#define COUNTRY_BULGARIA			// �Ұ�����
//#define COUNTRY_CANADA			// ĳ����
//#define COUNTRY_CHINA				// �߱�
//#define COUNTRY_COSTARICA			// �ڽ�Ÿ��ī
//#define COUNTRY_DOMINICA			// ���̴�ī
//#define COUNTRY_EASTERN_EUROPE	// CodePage1250 ������������ ��� ����մϴ�(�����ڵ� "EU").
//#define COUNTRY_GEORGIA			// �׷�����
//#define COUNTRY_GERMANY			// ����
//#define COUNTRY_GREECE			// �׸���
//#define COUNTRY_HUNGARY			// �밡��
//#define COUNTRY_INDIA				// �ε�
//#define COUNTRY_INDONESIA			// �ε��׽þ� Lotte-Mart
//#define COUNTRY_KENYA				// �ɳ�
//#define COUNTRY_LATVIA			// ��Ʈ���
//#define COUNTRY_LITHUANIA			// �����ƴϾ�
//#define COUNTRY_MAKEDONIA			// ���ɵ��Ͼ�
//#define COUNTRY_MALAYSIA_CN		// �����̽þ� chinese simplyfied version(���� ĸ��: GB2312 FONT)
//#define COUNTRY_MEXICO			// �߽���
//#define COUNTRY_MONGOL			// ���� ����
//#define COUNTRY_NETHERLAND		// �״�����
//#define COUNTRY_POLAND			// ������
//#define COUNTRY_SLOVENIA			// ���κ��Ͼ�
//#define COUNTRY_SOUTHAFRICA		// ���ư�
//#define COUNTRY_SWEDEN			// ������
//#define COUNTRY_THAILAND			// �±�
//#define COUNTRY_TURKEY			// ��Ű
//#define COUNTRY_USA				// �̱�
//#define COUNTRY_VENEZUELA			// ���׼�����

#endif /* USE_STANDARD */
/********************************************************************************/


/*
********************************************************************************
*                   CL5200N ��ǰ ���� Define (Hardewrare ����)
********************************************************************************
*/
#ifdef CL5200N_BP
	#define MODEL_NAME "CL5200N"
    #define CPU_PIC32MZ
    #define USE_USB_DATA_ROM_UPDATE //USB memory�� ����� DATAROM update
    #define USE_CL5200_KEY // P:72Ű, B:54Ű, H:72Ű ���
    #define USE_ONLY_1EA_PRT_RENDERING_BUF //printer buffer 1EA�� ��� (�޸� Ȯ��)
    #define NOT_USE_NETSET_LOCK
    //----------   CL5200N �߰� ���  ----------//
    #define CL5200_PROJECT                          // [ CL5200N ]  CL5200N ��Ÿ �߰� ���
	#define USE_INTERNAL_ADC                        // [ CL5200N ]  EFG CPU ���� ADC ���
	#define USE_SRAM_SPI_COMM                       // [ CL5200N ]  Serial SRAM SPI ��� ���� ���
	#define USE_SRAM_PRT_RENERING_BUF               // [ CL5200N ]  Printer Buffer�� Serial SRAM���� �̵� (���� �޸� Ȯ��)
	#define USE_CL5200_DISPLAY                      // [ CL5200N ]  CL5200 Display ����
	#define USE_KOR_LABELFORM31
	#define USE_PRINT_TEMP_SENSOR					// [ CL5200N ]  Temp Sensor Display ǥ�� �߰�
	#define USE_EXTEND_8M_DFLASH  					// [ CL5200N ]  Data Flash 8MB Ȯ�� ���� Ȱ���� ���� PLU ���� Ȯ�� (!!����!! ���� �Ļ� �� �ش� ���� Memory Structure ���� �ʿ�!!!)
	//	#define USE_EXTEND_NUMBER_OF_PLU_FOR_8M_DFLASH	// [ CL5200N ]	Data Flash 8MB Ȯ�� ���� Ȱ���� ���� PLU 12,000�� Ȯ�� ( Defalt�� : Disable ) (��������, Stack Memory Size �������� ���� PLU�� �ִ� 12,000�� ���� Ȯ�尡��)
	#define USE_EXTEND_CAPNTION_AREA                // [ CL5200N ]  Data Flash 8MB Ȯ�� ���� Ȱ���� ���� Caption 200�� Ȯ�� (!!����!! ���� �Ļ� �� �ش� ���� Memory Structure ���� �ʿ�!!!)
	#define CONTINEOUS_PRT_TEST						// ü�� ���� ���� �׽�Ʈ(QC�����)
	#define USE_USB_ZIG_TEST						// USB TEST (MENU 1867/MENU 8570���� USB Read/Write ���� Ȯ��)
	#define USE_DATETIME_ZIG_TEST					// RTC TEST (MENU 1867/MENU 8570���� RTC Read/Write, RTC Battery ���� Ȯ��)
	#define USE_DISPLAY_COMM_SPEED_SETTING			// �跮��忡�� ȭ���� ��� �����ϰ� �־, PLU ���� �ӵ� �������� ���� ���� (P770 �߰�)
    #define USE_DSP_USB_FW_UPDATE_VERSION           // USB FW ������Ʈ �� ���� �� ���� ǥ��
    #define USE_DSP_MESSAGE_FOR_MEM_CLEAR           // Memory clear ���� �޼��� ǥ��
	//#define USE_AUTO_FIND_VERSION					// ���� ���� �ڵ����� ã�ƿ��� ���
    //----------   CL5200N Test�� ���  ----------//
//	#define USE_CPU_LOAD_TEST                       // CPU Load Test - �ֱ������� NVRAM Read/Write, �ֱ����� ���� ���� �μ� 
//	#define USE_TEST_AUTO_LABEL_PRINT				// ���� ON �� ü�� �� 1�� ���� �׽�Ʈ
//	#define USE_CL5200N_SFLASH_DEBUGGING			// Flash Memory Read/Write Test (����̹� ���� �� ���)
//	#define USE_BUZZER_OFF_TEST                     // ���� ���� ���Ұ�(���� �� ����� ��� �϶� �Ҹ����°� ����)
//	#define USE_TEST_TMP_PRTBUF_MAX_SIZE			// SRMA�� ������ ���۷� ��� �� ���� ��(�����Ҵ�) �ִ밪�� �˾ƺ��� ���� �׽�Ʈ
//  #define USE_TEST_PRT_RENDERING_TIME				// SRMA�� ������ ���۷� ��� �� ���� ���۰� ������ �ð� ���̸� �˾ƺ��� ���� �׽�Ʈ
	//----------   ����� �޸� ������ ��ȸ ���   ----------//
	#define USE_CHECK_MEMORY_SIZE // ����� �޸� ������ ��ȸ ��� M1866
	//----------- 4-8MByte Ȯ�念�� ��� ���----------//
	#define USE_8M_EXPAND_BACKUP_RESTORE
	//----------- Scale Display(USE_CL5200_DISPLAY) ----------//
	#define _USE_LCD_7_20848_               // Graphic LCD : 7-seg part (208x48)
    #define _USE_LCD20848_                  // Graphic LCD : graphic part (208x48)
	#define USE_CL5200_DISPLAY_DEFAULT      // CL5200 display bug ���� ����
	#define USE_DISPLAY_OPT_CL5200	        // CL5200 Diplay �ӵ� ����
#endif // CL5200N_B
/********************************************************************************/


/*
********************************************************************************
*                   CL5500N ��ǰ ���� Define (Hardewrare ����)
********************************************************************************
*/
#if defined (CL5500N_BPRH) || defined (CL5500N_D)
	#define MODEL_NAME "CL5500N"
    #define CPU_PIC32MZ
    #define USE_USB_DATA_ROM_UPDATE //USB memory�� ����� DATAROM update
    //#define USE_CL5200_KEY // P:72Ű, B:54Ű, H:72Ű ���
    #define USE_ONLY_1EA_PRT_RENDERING_BUF //printer buffer 1EA�� ��� (�޸� Ȯ��)
    #define NOT_USE_NETSET_LOCK
    //----------   CL5200N �߰� ���  ----------//
    #define CL5500_PROJECT                          // [ CL5200N ]  CL5200N ��Ÿ �߰� ���
	#define USE_INTERNAL_ADC                        // [ CL5200N ]  EFG CPU ���� ADC ���
	#define USE_SRAM_SPI_COMM                       // [ CL5200N ]  Serial SRAM SPI ��� ���� ���
	#define USE_SRAM_PRT_RENERING_BUF               // [ CL5200N ]  Printer Buffer�� Serial SRAM���� �̵� (���� �޸� Ȯ��)
	#define USE_CL5500_DISPLAY                      // [ CL5500N ]  CL5500 Display
	#define USE_KOR_LABELFORM31
	#define USE_PRINT_TEMP_SENSOR					// [ CL5200N ]  Temp Sensor Display ǥ�� �߰�
	#define USE_EXTEND_8M_DFLASH  		// [ CL5200N ]  Data Flash 8MB Ȯ�� ���� Ȱ���� ���� PLU ���� Ȯ�� (!!����!! ���� �Ļ� �� �ش� ���� Memory Structure ���� �ʿ�!!!)
	//	#define USE_EXTEND_NUMBER_OF_PLU_FOR_8M_DFLASH	// [ CL5200N ]	Data Flash 8MB Ȯ�� ���� Ȱ���� ���� PLU 12,000�� Ȯ�� ( Defalt�� : Disable ) (��������, Stack Memory Size �������� ���� PLU�� �ִ� 12,000�� ���� Ȯ�尡��)
	#define USE_EXTEND_CAPNTION_AREA                // [ CL5200N ]  Data Flash 8MB Ȯ�� ���� Ȱ���� ���� Caption 200�� Ȯ�� (!!����!! ���� �Ļ� �� �ش� ���� Memory Structure ���� �ʿ�!!!)
	#define CONTINEOUS_PRT_TEST						// ü�� ���� ���� �׽�Ʈ(QC�����)
	#define USE_USB_ZIG_TEST						// USB TEST (MENU 1867/MENU 8570���� USB Read/Write ���� Ȯ��)
	#define USE_DATETIME_ZIG_TEST					// RTC TEST (MENU 1867/MENU 8570���� RTC Read/Write, RTC Battery ���� Ȯ��)
	#define USE_DSP_USB_FW_UPDATE_VERSION           // USB FW ������Ʈ �� ���� �� ���� ǥ��
    #define USE_DSP_MESSAGE_FOR_MEM_CLEAR           // Memory clear ���� �޼��� ǥ��
	//#define USE_AUTO_FIND_VERSION					// ���� ���� �ڵ����� ã�ƿ��� ���
    //----------   CL5200N Test�� ���  ----------//
//	#define USE_CPU_LOAD_TEST                       // CPU Load Test - �ֱ������� NVRAM Read/Write, �ֱ����� ���� ���� �μ� 
//	#define USE_TEST_AUTO_LABEL_PRINT				// ���� ON �� ü�� �� 1�� ���� �׽�Ʈ
//	#define USE_CL5200N_SFLASH_DEBUGGING			// Flash Memory Read/Write Test (����̹� ���� �� ���)
//	#define USE_BUZZER_OFF_TEST                     // ���� ���� ���Ұ�(���� �� ����� ��� �϶� �Ҹ����°� ����)
//	#define USE_TEST_TMP_PRTBUF_MAX_SIZE			// SRMA�� ������ ���۷� ��� �� ���� ��(�����Ҵ�) �ִ밪�� �˾ƺ��� ���� �׽�Ʈ
//  #define USE_TEST_PRT_RENDERING_TIME				// SRMA�� ������ ���۷� ��� �� ���� ���۰� ������ �ð� ���̸� �˾ƺ��� ���� �׽�Ʈ
	//----------   CL5500N-BPRH �߰����  ----------//
	#define DISABLE_ADM_SND_WT_WHILE_USB_COMM	// USB ������ �д� ���� ADM WT ���� �ߴ�(USB �ν� ���� ���)
    //----------   CL5500N ������ ��ī ----------//
	#ifdef CL5500N_BPRH
	#define _CL5500_PRINTER_MECH	//CL5500 Printer
	#elif defined (CL5500N_D)
	#define _CL5500_D_PRINTER_MECH	//CL5500-D Printer
	#endif
    //----------   ���ݱ��            ----------//
//	    #define USE_REMOTE_FUNCTION
    #ifdef USE_REMOTE_FUNCTION
		#define USE_EXTEND_FLASH // NVRAM 8MB Ȯ��
	    #define USE_REMOTE_FW_DOWNLOAD // ���� �߿��� �ٿ�ε� ���
	    #define USE_REMOTE_ROM_UPDATE // ���� ���÷� ������Ʈ ��� 'USE_CHK_LABEL_CLR_WHEN_USB_UPDATE'����� ���� �ʿ�    
	#endif
	//----------   ����� �޸� ������ ��ȸ ���   ----------//
	#define USE_CHECK_MEMORY_SIZE // ����� �޸� ������ ��ȸ ��� M1866
	//----------- 4-8MByte Ȯ�念�� ��� ���----------//
	#define USE_8M_EXPAND_BACKUP_RESTORE
	//----------- Scale Display(USE_CL5500_DISPLAY) ----------//
	#ifdef CL5500N_BPRH
        #define _USE_LCD_7_24_    //CL5500 B,P,R,S,H : 7-seg LCD(24digits), CL5000JC
        #define _USE_LCD32_       //Graphic LCD(202x32)
    #elif defined (CL5500N_D)
        #define _USE_LCD_7_25_    //CL5500-D : 7-seg LCD(25digits)
        #define _USE_LCD32_       //Graphic LCD(202x32)
	#endif
#endif // CL5500N_BPRHD
/********************************************************************************/


/*
********************************************************************************
*                   ETC Compiler Option define
********************************************************************************
*/
#ifndef CPU_PIC32MZ
	#define _PACKED_DEF	__packed
	#define __no_init   __no_init
#else
	#define _PACKED_DEF	//__attribute__ ((packed))
	#define __no_init
#endif //#ifndef CPU_PIC32MZ
#define PACK_2			#pragma pack(2)
#define PACK_4			#pragma pack(4)
#define PACK_CLOSE		#pragma pack()
//#define DISCOUNT_MAX_NUM_200	<= ���� ���� ó�� �ȵ�. �� �����ؾ���

#ifndef USE_INTERNAL_ADC
	#define USE_EXTERNAL_ADC    // [CL5200 Renewal] �ܺ� ADC ���
#endif
/********************************************************************************/


/*
********************************************************************************
*                   < Common Definition - �� ���� ���� >
********************************************************************************
*/
//#define USE_CERT_APPLY						// ������ �߿��� ����� ����
//#define USE_USA_NTEP_SETTING					// �̱� NTEP ������ �� ���� ����
#define USE_ALLOW_CAL_MODE						//8000 + 'menu'key�� ���� CAL ��� ���� ����
												// ���������� define�� ������� ���� �Ұ�.
//#define _USE_UNDER_SELFKEY_ 					//BODY�� SELFKEY PAD ����
#define USE_CHECK_WLAN_STATUS_LANTRONIX			// LANTRONIX CHECK ���(SCAN SSID, ���� ��)
#define USE_UNDER_LIMIT_MINUS20D				// -20d �̸� ����ǥ������ ��� (OIML2006)
//#define USE_ONLY_LABEL						// for CL5200 Label version
#define USE_CTS_FUNCTION 						// Calibration Tracebility System
//#define USE_MANAGE_LICENSE					// License ���� ���
#define USE_KOR_LABELFORM31 					// 31�� �� ���� ���
#define USE_FUNCTION_KEY_BLOCK 					// Ű�е� ������ ���Ű onoff ��뿩�� �����ϴ� ���
//#define USE_CHN_CERTIFICATION					//�߱� �����㰡�� ���� ���. �����߿��� ���۽� �ּ� ó��
#define DISPLAY_REVISION_INFO   				// Revision ���� ��¿��� �����ϴ� ������
#define USE_CL5200_DISP_CONTINUOUS_REFRESH		// Display Refresh Parameter(737)�� ���� ȭ�� ���� �ֱ� ����(1: 2s-100ms / 0: X-1,000ms)
#define USE_ADM_AUTO_RESTORE					// ADM error(���� 03, 04) �� �� �ڵ����� init �����ϵ��� ����(��������)
//#define USE_CHK_LABEL_CLR_WHEN_USB_UPDATE		// USB�� �� ������Ʈ�� �� �������� ������ ����
#define USE_PARTIAL_MEM_INIT 					// �޸� �κ� �ʱ�ȭ
#define USE_INIT_MASK 							// �޸� �ʱ�ȭ ���
#define USE_MORNITORING_CNT 					// Menu 1879 ����͸� ������ ���

#define USE_QR_BARCODE							// QR ���ڵ�
#define USE_BARCODE_CODE128X					// CODE128X ��� 
#define USE_STANDARD_BARCODE					// GS1 DataBar ������
/********************************************************************************/


/*
********************************************************************************
*                   <FONT(Windows codepage)>
********************************************************************************
*/
#define CODEPAGE_1250	1
#define CODEPAGE_1251	2
#define CODEPAGE_1252	3
#define CODEPAGE_1253	4
#define CODEPAGE_1254	5
#define CODEPAGE_1255	6
#define CODEPAGE_1256	7
#define CODEPAGE_1257	8
#define CODEPAGE_1258	9
#define CODEPAGE_1252_ARM   10 // Armenia
#define CODEPAGE_1251_GEO   11 // Georgia
#define CODEPAGE_1250_POL   12 // POLAND
#define CODEPAGE_0936_CHN   13 // Simplified chinese
#define CODEPAGE_0949_KOR   14
#define CODEPAGE_1252_IND   15
#define CODEPAGE_1251_MNG   16 // Mongolia
#define CODEPAGE_1250_HUN   17 // Hungary
#define CODEPAGE_1251_KAZ   18 // Kazakhstan
#define CODEPAGE_1257_BAL   19 // Baltic
#define CODEPAGE_1258_VNM	20 // Vietnam
#define CODEPAGE_0874_ENG	21 // Thailand
#define CODEPAGE_1256_IRN   22 // Persian
#define CODEPAGE_0950_CHN	23 // Traditional Chinese
/********************************************************************************/


/*
********************************************************************************
*                   ������� ������ ��� difine
********************************************************************************
*/
#ifdef USE_STANDARD
//----------   STANDARD_DIRECT_INGREDIENT  ----------//
	#ifdef COUNTRY_USA
		#include "./Nation/USA_UN_D.h"
	#elif defined COUNTRY_CANADA
		#include "./Nation/CAN_UN_D.h"
	#elif defined COUNTRY_AUSTRALIA
		#include "./Nation/AUS_UN_D.h"
	#elif defined COUNTRY_SLOVENIA
		#include "./Nation/SVN_UN_D.h"
	#elif defined COUNTRY_DOMINICA
		#include "./Nation/DOM_UN_D.h"
/********************************************************************************/
	#elif defined COUNTRY_GERMANY
		#include "./Nation/DEU_UN.h"
	#elif defined COUNTRY_INDIA
		#include "./Nation/IND_UN.h"
	#elif defined COUNTRY_CHINA
		#include "./Nation/CHN_UN.h"
	#elif defined COUNTRY_INDONESIA			/* Indonesia Lotte Mart */
		#include "./Nation/IDN_UN.h"
	#elif defined COUNTRY_GEORGIA
		#include "./Nation/GEO_UN.h"
	#elif defined COUNTRY_MALAYSIA_CN
		#include "./Nation/MYS_UN_CN.h"
	#elif defined COUNTRY_TURKEY
		#include "./Nation/TUR_UN.h"
	#elif defined COUNTRY_AZERBAIJAN
		#include "./Nation/AZE_UN.h"
	#elif defined COUNTRY_MEXICO
		#include "./Nation/MEX_UN.h"
	#elif defined COUNTRY_POLAND
		#include "./Nation/POL_UN.h"
	#elif defined COUNTRY_MAKEDONIA
		#include "./Nation/MKD_UN.h"
	#elif defined COUNTRY_GREECE			/* for CL5200 for greece �ǸŽ� label�� ����Ʈ�� ticket���� ��°��� */
		#include "./Nation/GRC_UN.h"
	#elif defined COUNTRY_KENYA
		#include "./Nation/KEN_UN.h"
	#elif defined COUNTRY_VENEZUELA
		#include "./Nation/VEN_UN.h"
	#elif defined COUNTRY_HUNGARY
		#include "./Nation/HUN_UN.h"
	#elif defined COUNTRY_LATVIA
		#include "./Nation/LVA_UN.h"
	#elif defined COUNTRY_LITHUANIA
		#include "./Nation/LTU_UN.h"
	#elif defined COUNTRY_SOUTHAFRICA
		#include "./Nation/ZAF_UN.h"
	#elif defined COUNTRY_NETHERLAND
		#include "./Nation/NLD_UN.h"
	#elif defined COUNTRY_EASTERN_EUROPE
		// ROMANIA
		// CROATIA
		#include "./Nation/EUR_UN_EASTERN.h"
	#elif defined COUNTRY_THAILAND
		#include "./Nation/THA_UN.h"
	#elif defined COUNTRY_BULGARIA
		#include "./Nation/BGR_UN.h"
	#elif defined COUNTRY_MONGOL
		#include "./Nation/MNG_UN.h"
	#elif defined COUNTRY_COSTARICA
		#include "./Nation/CRI_UN.h"
	#elif defined COUNTRY_SWEDEN
		#include "./Nation/SWE_UN.h"
	#else
		// else�� ó���Ǵ� ���� ���(Codepage�� �ٸ� ����)
		// COUNTRY_GERMANY_BIZERBA
		// COUNTRY_UNITED_KINGDOM
		#define FONT_CODEPAGE CODEPAGE_1252
	#endif
//////////////////////////////////////////////////////////////////////////////////
/*				< ���� ������� �� Direct Ingredient ���� ������� >			*/
	#include "./Nation/UN_function.h"
//////////////////////////////////////////////////////////////////////////////////
#endif  // #ifdef USE_STANDARD
/********************************************************************************/


/*
********************************************************************************
*                   ���� ������ ��� difine
********************************************************************************
*/
#ifdef USE_UN_LOGISTICS
	#include "./Nation/UN_LG.h"
#endif

#ifdef USE_POLAND_LOGISTICS
	#include "./Nation/POL_PL.h"
#endif

#ifdef USE_POLAND_STOKROTKA_MART
 	#include "./Nation/POL_SK.h"
#endif

#ifdef USE_CHINA_ALIBABA
	#include "./Nation/CHN_l.h"
#endif

#ifdef USE_CHINA_HUARENWANGJI
	#include "./Nation/CHN_d.h"
#endif

#ifdef USE_CHINA_CARRFOUR
	#include "./Nation/CHN_f.h"
#endif

#ifdef USE_CHINA_BAONENG
	#include "./Nation/CHN_X_D.h"
#endif

#ifdef USE_CHINA_BAONENG_COUNTRYGARDEN
	#include "./Nation/CHN_w_D.h"
#endif

#ifdef USE_NHMART
	#include "./Nation/KOR_NHMART_N.h"
#endif

#ifdef USE_NHMART_SAFE
	#include "./Nation/KOR_NHMART_SAFE_n.h"
#endif

#ifdef USE_GSMART_TRACE
	#include "./Nation/KOR_GSMART_TRACE_M.h"
#endif

#ifdef USE_GSMART
	#include "./Nation/KOR_GSMART_G.h"
#endif

#ifdef USE_HYUNDAI
	#include "./Nation/KOR_HYUNDAI_H.h"
#endif

#ifdef USE_SCALE_POS
	#include "./Nation/KOR_SCALE_POS_T.h"
#endif

#ifdef USE_EMART
	#include "./Nation/KOR_EMART_E.h"
#endif

#ifdef USE_DISCLOSURE_MEAT_PRICE
	#include "./Nation/KOR_DISCLOSURE_MEAT_PRICE_g.h"
#endif

#ifdef USE_RUSSIA_INDIRECT
	#include "./Nation/RUS_Q.h"
#endif

#ifdef USE_RUSSIA_LOGISTICS
	#include "./Nation/RUS_l_D.h"
#endif

#ifdef USE_RUSSIA_SPAR
	#include "./Nation/RUS_SP_D.h"
#endif

#ifdef USE_UKRAINE
	#include "./Nation/UKR_Z_D.h"
#endif

#ifdef USE_UN_KOR_MART
	#include "./Nation/UN_KOR_MART_kr.h"
#endif

#ifdef USE_BELARUS
	#include "./Nation/BLR_r_D.h"
#endif

#ifdef USE_VIETNAM
	#include "./Nation/VNM_V.h"
#endif

#ifdef USE_VIETNAM_EMART
	#include "./Nation/VNM_EMART_v.h"
#endif

#ifdef USE_VIETNAM_LOTTEMART
	#include "./Nation/VNM_LOTTEMART_VL.h"
#endif

#ifdef USE_IMPORT_MEAT
	#include "./Nation/KOR_IMPORT_MEAT_I.h"
#endif

#ifdef USE_SHINSEGAE_MEAT
	#include "./Nation/KOR_SHINSEGAE_MEAT_S.h"
#endif

#ifdef USE_IMPORT_MEAT_CART
	#include "./Nation/KOR_IMPORT_MEAT_CART_s.h"
#endif

#ifdef USE_LOTTEMART
	#include "./Nation/KOR_LOTTEMART_J.h"
#endif

#ifdef USE_LOTTESUPER
	#include "./Nation/KOR_LOTTESUPER_U.h"
#endif

#ifdef USE_LOTTESUPER_8LINE
	#include "./Nation/KOR_LOTTESUPER_8LINE_W.h"
#endif

#ifdef USE_CANADA_LABEL
	#include "./Nation/CAN_LABEL_L.h"
#endif

#ifdef USE_SRILANKA
	#include "./Nation/LKA_K.h"
#endif

#ifdef USE_CHINESE_FONT_BIG5
	#include "./Nation/CHN_FONT_BIG5_TR_D.h"
#endif

#ifdef USE_CHINESE_FONT_GB2312
	#include "./Nation/CHN_FONT_GB2312_D.h"
#endif

#ifdef USE_CHN_TRACE_STANDARD	//USE_CHN_EMART
	#include "./Nation/CHN_TRACE_STANDARD_F.h"
#endif

#ifdef USE_GBR_HMART
	#include "./Nation/GBR_HMART_P.h"
#endif

#ifdef USE_GEO_AGROHUB_MART
	#include "./Nation/GEO_AGROHUB_MART_GA.h"
#endif

#ifdef USE_GLOBAL_KOREAN_MART
	#include "./Nation/GLOBAL_KOR_MART_KR.h"
#endif

#ifdef USE_ARAB_EMIRATES
  	#include "./Nation/ARE_a.h"
#endif

#ifdef USE_PERSIA
  	#include "./Nation/IRN_p.h"
#endif

#ifdef USE_HEBREW
  	#include "./Nation/ISR_HEBREW_h.h"
#endif

#ifdef USE_SAUDI   // �ƶ������ + ���� Direct Ingredient
	#include "./Nation/SAU_PT_D.h"
#endif

#ifdef USE_CHN_STANDARDMARKET
	#include "./Nation/CHN_STANDARDMARKET_c.h"
#endif

#ifdef USE_MANAGE_INVENTORY
	#include "./Nation/MANAGE_INVENTORY_i.h"
#endif

#ifdef USE_MANAGE_INVENTORY_CART
	#include "./Nation/MANAGE_INVENTORY_CART_y.h"
#endif

#ifdef USE_KOREA_CL5200
	#include "./Nation/KOR_CL5200_k.h"
#endif

#ifdef USE_EMART_CL5200
	#include "./Nation/KOR_EMART_CL5200_m.h"
#endif

#ifdef USE_INDONESIA_EMPORIUM
	#include "./Nation/IDN_EMPORIUM_e_D.h"
#endif

#ifdef USE_MON_EMART
	#include "./Nation/MNG_EMART_g.h"
#endif

#ifdef USE_TURKEY_QR_BARCODE
	#include "./Nation/TUR_QR_BARCODE_t_D.h"
#endif

#ifdef USE_TOPMART
	#include "./Nation/KOR_TOPMART_Y.h"
#endif

#ifdef USE_INDIA_DADUS
	#include "./Nation/IND_DADUS_DM.h"
#endif

#ifdef USE_INDIA_TATA
	#include "./Nation/IND_TATA_TA.h"
#endif

#ifdef USE_RSA_SPAR_MART
	#include "./Nation/ZAF_SPAR_MART_SR_D.h"
#endif

#ifdef USE_IMPORT_MEAT_8LINE
	#include "./Nation/KOR_IMPORT_MEAT_8LINE_I8.h"
#endif
//////////////////////////////////////////////////////////////////////////////////
/*								< ���þ� �߿��� >								*/
#ifdef USE_RUSSIA
	#include "./Nation/RUS_function.h"

/*						< ���þ� ���� Ư�� ����(Country) ���� >						*/
//#define COUNTRY_RUSSIA 			// Default
//#define COUNTRY_ARMENIA
//#define COUNTRY_KAZAKHSTAN
//#define COUNTRY_LITHUANIA
//#define COUNTY_UZBEKISTAN

  #ifdef COUNTRY_RUSSIA
	#include "./Nation/RUS_R_D.h"
  #elif defined COUNTRY_ARMENIA
	#include "./Nation/ARM_R_D.h"
  #elif defined COUNTRY_KAZAKHSTAN
	#include "./Nation/KAZ_R_D.h"
  #elif defined COUNTRY_LITHUANIA
	#include "./Nation/LTU_R_D.h"
  #elif defined COUNTY_UZBEKISTAN
	#include "./Nation/UZB_R_D.h"
  #else
 	#define FONT_CODEPAGE CODEPAGE_1251
  #endif
#endif
//////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////
/*							< �ƶ� ��Ʈ ��� �߿��� >							*/
#ifdef USE_ARAB
	#include "./Nation/ARAB_FONT_function_A.h"

/*						< �ƶ� ��Ʈ Ư�� ����(Country) ���� >						*/
//#define COUNTRY_LEBANON

  #ifdef COUNTRY_LEBANON
  	#include "./Nation/LBN_A.h"
  #endif //COUNTRY_LEBANON
#endif //USE_ARAB
//////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////
/*							< ����(Ƽ�� ��� X) ��� �߿��� >							*/
#ifdef USE_EUROPE_WITHOUT_TICKET
	#include "./Nation/UN_function.h"

/*					< ����(Ƽ�� ��� X) Ư�� ����(Country) ���� >						*/
//#define COUNTRY_GERMANY

  #ifdef COUNTRY_GERMANY
	#include "./Nation/DEU_EUR_u.h"
  #endif // COUNTRY_GERMANY
#endif
//////////////////////////////////////////////////////////////////////////////////


/********************************************************************************/


/*
********************************************************************************
* 			                   ���÷��� ����
*  < ����ϰ��� �ϴ� DIGIT�� ������Ͽ��� �Է¹޾� ǥ���ϴ� ���ǹ��� �ڵ����� ���� >
********************************************************************************
*/
#ifdef CL5200N_BP
  #if defined(USE_PRICE_UNIT_7DIGIT) || defined(USE_PRICE_TOTAL_7DIGIT)	// �ܰ�, �Ѿ� 7�ڸ� ����
	#define USE_DISPLAY_5200_7DIGIT	//5200 lcd_7_20848���� 7�ڸ� ǥ��, 7�ڸ� ����� ��� parameter 997 Display style 3(Tare ��ǥ�� )���� ���� �Ұ�c
  #endif

  #if defined(USE_PRICE_UNIT_9DIGIT) || defined(USE_PRICE_TOTAL_9DIGIT)	// �ܰ�, �Ѿ� 9�ڸ� ����
    #define USE_DISPLAY_STYLE_3DIGIT		// Tare�� ������ �߷�/�ܰ�/�Ѿ� ǥ��
  #endif
#endif
/********************************************************************************/


/*
********************************************************************************
* 			                   ��ǰ ���� ����
*               < ������ �ش� ���ǹ��� Ȱ��ȭ ���� �ʿ��� ��� ��� >
********************************************************************************
*/
#ifdef USE_CERT_APPLY
	#define USE_CHECK_CAL_SW_TO_UPDATE_FW		//������ CAL����ġ�� ���� firmware ������Ʈ �� �� �ֵ��� ��.(#define USE_OIML_SETTING ��� ����)
	#define USE_CONTINUOUS_PRINT_DELAY_2000ms		// EMC�׽�Ʈ �� �μ� �ð� ���� ����(2000ms)
	#define USE_CERT_DISP_CONTINUOUS_REFRESH		// ���� Sample �� ȭ�� ���� ���� �߿��� 
#endif
/********************************************************************************/


/*
********************************************************************************
* 			                     ��Ʈ ����
*   < ������Ͽ� FONT_CODEPAGE�� ���ǵǾ� ���� �ʴٸ� �ش� ���ǹ��� ����Ʈ�� ���� >
********************************************************************************
*/
#ifndef FONT_CODEPAGE
#define FONT_CODEPAGE CODEPAGE_1252
#endif
/********************************************************************************/


/////////////////////////////Compile Condition END/////////////////////////////////

//#define USE_KEY_BUZZER	// Buzzer�� Key�� ���� ������ �︮���� ��

#define ROMDATA	const
#define HUGEDATA
#define _huge
#define _rom const
#define _inline
#define NEARDATA

//#define NDEBUG //debug message to output window


#define ON		1
#define OFF		0
#define IDLE	2

#define	DISABLE		0
#define	ENABLE		1
#define YES		1
#define NO		0
#if !defined(TRUE)
  #define	TRUE	(!0)
  #define	FALSE	0
#endif

// <Flash memory Type> //
#define EN25QH32A		0 // ���� (Eon)
#define SST26VF0XXB     1 // ��ü �� ( Microchip - SST26VF032B[4M] or SST26VF064B[8M] )
//#define SST26VF032B		1 // ��ü �� (Microchp)


#define NUL		0x00
#define SOH		0x01
#define	STX		0x02
#define ETX	   	0x03
#define EOT		0x04
#define ENQ		0x05
#define ACK		0x06
#define BEL		0x07
#define BS 		0x08
#define HT 	   	0x09
#define LF 		0x0A
#define VT 		0x0B
#define FF 		0x0C
#define _CR 	0x0D
#define SO		0x0E
#define SI		0x0F
#define DLE		0x10
#define DC1		0x11
#define DC2		0x12
#define DC3		0x13
#define DC4		0x14
#define NAK		0x15
#define SYN		0x16
#define ETB		0x17

#define EM		0x19
#define SUB		0x1A
#define _ESC	0x1B   		//091207 yoo test
#define FC 		0x1C
#define GS 		0x1D
#define __RS 	0x1E		//QR encode lib conflict
#define US 		0x1F
#define DEL 	0x7F

typedef unsigned char BOOLEAN; /* Logical data type (TRUE or FALSE) */
typedef unsigned char INT8U; /* Unsigned 8 bit value */
typedef signed char INT8S; /* Signed 8 bit value */
typedef unsigned short INT16U; /* Unsigned 16 bit value */
typedef signed short INT16S; /* Signed 16 bit value */
typedef unsigned long INT32U; /* Unsigned 32 bit value */
typedef signed long INT32S; /* Signed 32 bit value */
typedef unsigned long long INT64U; /* Unsigned 64 bit value (if available)*/
//typedef signed xxxxx INT64S; /* Signed 64 bit value (if available)*/
typedef float FP32; /* 32 bit, single prec. floating-point */
typedef double FP64; /* 64 bit, double prec. floating-point */

#endif /* _GLOBALS_H */
#ifndef _FW_VERSION_DEFINE
#define _FW_VERSION_DEFINE

#define FW_BOOT_VERSION_ROM_ADDR	0x21fff0
#define FW_VERSION_ROM_ADDR	 		0x07fff0

#pragma pack(2)
typedef struct {
	char model[2];
	char year[2];
	char month[2];	// 6
	char country[3];//
	char keytype;	// 10
	char subversion[2];	//vendor[2];		// Modified by CJK 20061121
	char version;
	char reversion[2];
	char testversion;	// CJK080321
} FW_VERSION;

typedef struct {
	char dealer[4];	// include NULL
	char country[3];	// include NULL
	char calpassword[6];	// Cal Password version & init default
	char reserved[3];
} FW_VERSION2;
#pragma pack()

#ifdef USE_MORNITORING_CNT
#pragma pack(2)
typedef struct {
    INT32U TotalCnt;
    INT32U AfterCnt;
    INT32U InspectDate;
    INT32U Reserved;
} REMOTE_STRUCT;
#pragma pack()
#endif

extern void version_get(FW_VERSION *fw);
extern void version2_get(FW_VERSION2 *fw);
#endif

#ifndef _ROM_COUNTRY_DEFINE
#define _ROM_COUNTRY_DEFINE

#define ROM_STD	0
#define ROM_RUS	1

#if defined(USE_RUSSIA) || defined(USE_RUSSIA_INDIRECT) || defined(USE_BELARUS) || defined(USE_UKRAINE) || defined(USE_RUSSIA_LOGISTICS) || defined(USE_RUSSIA_SPAR)
  #define ROM_COUNTRY ROM_RUS
#else
  #define ROM_COUNTRY ROM_STD
#endif
#endif

#if defined(USE_MANAGE_LICENSE)		// License ���� ���
#define USE_DES_ENCRYPTION
#endif

//#define TEST_REPORT