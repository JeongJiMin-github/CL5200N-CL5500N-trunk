/**
* @file dsp_lcd7_25d.c
* @attention (c) 2008 CAS
* @version 0.1
* @author 홍의표
* @date 2008/10/31
* @brief lcd(2line,25digit,double body용)에 디스플레이 하기 위한 모듈
*/

//#include "91x_rtc.h"
//#include "91x_lib.h"
#include "globals.h"
#include "dsp_app.h"


#ifdef _USE_LCD_7_25_

#define MINUS_SEG             0x02//2*pos
#define SPACE_SEG             0x08//2*pos
#define DOT_SEG               0x04//0x08//2*pos+1
#define COMMA_SEG             0x08//0x10//2*pos+1
#define TRIANGLE_SEG          0x40//yoo test

#define SPA_SEG               0x00
#define ALL_SEG               0x7f
#define CURSOR_SEG			  0x08
#define OTHER_SEG             0x18

#define CURSOR_CHAR			  0x01


#define DSP_STR_BUF_SIZE	16

#define DSP_MAX_DIGIT   26
#define DSP_MAX_CHAR	12
#define PRICE_DIGIT     7

#define WEIGHT_DIGIT          5
#define UNIT_DIGIT			6
#define PT_DIGIT              4

#define PT_DISPLAY_START      1//0//4
#define WEIGHT_DISPLAY_START  13//5//5
#define UNIT_DISPLAY_START    7//11//6

#define DIFFUSE_ALL		0
#define DIFFUSE_7SEG	1
#define DIFFUSE_12CHAR	2

#define PRICE_DISPLAY_START   19//17//7
#define PMSG_DIGIT		12
#define PMSG_DISPLAY_START   24//12

#define DOT_TYPE_COMMA	0x80


/** @brief Turn off both system oscillator and Lcd bias generator*/
#define HT162X_SYS_DIS	0x00
/** @brief Turn on system oscillator*/
#define HT162X_SYS_EN	0x01
/** @brief Turn off Lcd display*/
#define HT162X_OFF		0x02
/** @brief Turn on Lcd display*/
#define HT162X_ON		0x03
/** @brief Disable time base output*/
#define HT162X_TIMER_DIS	0x04
/** @brief Disable WDT time-out flag output*/
#define HT162X_WDT_DIS	0x05
/** @brief Enable time base output*/
#define HT162X_TIMER_EN	0x06
/** @brief Enable WDT time-out flag output*/
#define HT162X_WDT_EN	0x07
/** @brief Turn off tone outputs*/
#define HT162X_TONE_OFF	0x08
/** @brief Clear the contents of the time base generator*/
#define HT162X_CLR_TIMER	0x0d
/** @brief Clear the contents of WDT stage*/
#define HT162X_CLR_WDT	0x0f
/** @brief System clock source, on-chip RC oscillator*/
#define HT162X_RC_32K	0x18
/** @brief System clock source, external clock source*/
#define HT162X_EXT_32K	0x1c
/** @brief Tone frequency output : 4kHz*/
#define HT162X_TONE_4K	0x40
/** @brief Tone frequency output : 2kHz*/
#define HT162X_TONE_2K	0x60
/** @brief Disable IRQ output*/
#define HT162X_IRQ_DIS	0x80
/** @brief Enable IRQ output*/
#define HT162X_IRQ_EN	0x88
/** @brief Time base clock output : 1Hz.   The WDT time-out flag after : 4s*/
#define HT162X_F1		0xa0
/** @brief Time base clock output : 2Hz.   The WDT time-out flag after : 2s*/
#define HT162X_F2		0xa1
/** @brief Time base clock output : 4Hz.   The WDT time-out flag after : 1s*/
#define HT162X_F4		0xa2
/** @brief Time base clock output : 8Hz.   The WDT time-out flag after : 1/2s*/
#define HT162X_F8		0xa3
/** @brief Time base clock output : 16Hz.  The WDT time-out flag after : 1/4s*/
#define HT162X_F16		0xa4
/** @brief Time base clock output : 32Hz.  The WDT time-out flag after : 1/8s*/
#define HT162X_F32		0xa5
/** @brief Time base clock output : 64Hz.  The WDT time-out flag after : 1/16s*/
#define HT162X_F64		0xa6
/** @brief Time base clock output : 128Hz. The WDT time-out flag after : 1/32s*/
#define HT162X_F128		0xa7
/** @brief Test mode, user don't use*/
#define HT162X_TEST		0xe0
/** @brief Normal mode*/
#define HT162X_NORMAL	0xe3

INT8U conPortStatus = 0;
INT8U datPortStatus = 0;

void ht162XWriteCommand(INT8U datcom);
void ht162XTranCommand(INT8U command);      
void ht162XTranData(INT8U disdata);
void ht162XWriteAllData(INT8U type);

INT8U DSP_Memory[DSP_MAX_DIGIT*2+DSP_MAX_CHAR*5];
INT8U DSP_Memory_blink[DSP_MAX_DIGIT*2+DSP_MAX_CHAR*5];
INT8U DSP_Memory_capture[DSP_MAX_DIGIT*2+DSP_MAX_CHAR*5];

INT8U	DspPtrFont1;

INT8U capture_flag;
INT8U blink_flag;
INT8U blink_spos;
INT8U blink_epos;

const INT8U ucNumSegTable[] = {    /* Decimal number */
	0x6F,        /* 0 */
	0x60,        /* 1 */
	0x5E,        /* 2 */
	0x7C,        /* 3 */
	0x71,        /* 4 */
	0x3D,        /* 5 */
	0x3F,        /* 6 */
	0x69,        /* 7 */
	0x7F,        /* 8 */
	0x7D         /* 9 */
};

const INT8U ucAlphaSegTable[] = {
	0x7B,        /* A */
	0x37,        /* B */
	0x0F,        /* C */
	0x76,        /* D */
	0x1F,        /* E */
	0x1B,        /* F */
	0x2F,        /* G */
	0x73,        /* H */
	0x20,        /* I */
	0x66,        /* J */
	0x13,        /* K */
	0x07,        /* L */
	0x3A,        /* M */
	0x32,        /* N */
	0x36,        /* O */
	0x5B,        /* P */
	0x79,        /* Q */
	0x12,        /* R */
	0x3D,        /* S */
	0x17,        /* T */
	0x67,        /* U */
	0x26,        /* V */
	0x2E,        /* W */
	0x1C,        /* X */
	0x75,        /* Y */
	0x5E        /* Z */
};

INT8U *DSP_buf_ptr;


#define DSP_ON	1
#define DSP_OFF	0

#define WMSG_DIGIT			UNIT_DIGIT
#define WMSG_DISPLAY_START	UNIT_DISPLAY_START
#define UMSG_DIGIT			PRICE_DIGIT
#define UMSG_DISPLAY_START	PRICE_DISPLAY_START


const char typeDp1[13][6] = {"%0ld", "%1ld", "%2ld", "%3ld", "%4ld", "%5ld", "%6ld", "%7ld", "%8ld", "%9ld", "%10ld", "%11ld", "%12ld"};
const char typeDp2[13][7] = {"%00ld", "%01ld", "%02ld", "%03ld", "%04ld", "%05ld", "%06ld", "%07ld", "%08ld", "%09ld", "%010ld", "%011ld", "%012ld"};
const char typeDpHex[13][6] = {"%00x", "%01x", "%02x", "%03x", "%04x", "%05x", "%06x", "%07x", "%08x", "%09x", "%10x", "%11x", "%12x"};



INT16U DSP7_DATA;
extern void SPILCDWrite_2Byte(INT16U data);

void DISPLAY_DATA(unsigned char data)
{
	INT16U DSP7_TEMP_DATA = 0;       
	
        DSP7_TEMP_DATA = data;
        DSP7_DATA = (DSP7_TEMP_DATA<<8)| (DSP7_DATA & 0x00FF);
        //Inline_DIS_Data_Tx(DSP7_DATA);
	SPILCDWrite_2Byte(DSP7_DATA);
	Delay_10();
	
}

void DISPLAY_CON(unsigned char data)
{
        INT16U DSP7_TEMP_DATA = 0;		 
		
	DSP7_TEMP_DATA = (data|0x20);
	DSP7_DATA = (DSP7_TEMP_DATA)| (DSP7_DATA & 0xFF00);
	//Inline_DIS_Data_Tx(DSP7_DATA);
	SPILCDWrite_2Byte(DSP7_DATA);
	Delay_10();

}


/**
********************************************************************************
* @brief    HT162X IC에 command 전송 함수
* @param    datcom : command\n
* @return   none
* @remark   
********************************************************************************
*/
void ht162XWriteCommand(INT8U datcom)         
{
    DISPLAY_CON(0x0E);
	DISPLAY_DATA(0x01);
	Delay_10();
	DISPLAY_DATA(0x03);
    Delay_10();
	DISPLAY_DATA(0x00);
	Delay_10();
	DISPLAY_DATA(0x02);
	Delay_10();
	DISPLAY_DATA(0x00);
	Delay_10();
	DISPLAY_DATA(0x02);
	Delay_10();

    ht162XTranCommand(datcom);

    DISPLAY_DATA(0x00);
	Delay_10();
	DISPLAY_DATA(0x02);
	Delay_10();   

    DISPLAY_CON(0x1E);  //serial control  //DISPLAY_CON(0x2E);

}

/**
********************************************************************************
* @brief    HT162X IC에 command byte 전송 함수
* @param    command : command\n
* @return   none
* @remark   
********************************************************************************
*/
void ht162XTranCommand(INT8U command)      
{
    INT8U k;
    
    for (k = 0; k < 8; k++)
	{

		if ((command & 0x80) == 0)
		{
				DISPLAY_DATA(0x00);
				Delay_10();
				DISPLAY_DATA(0x02);
				Delay_10();
		}
		else
		{
				DISPLAY_DATA(0x00);
				Delay_10();
				DISPLAY_DATA(0x03);
				Delay_10();

		}
		command <<= 1;
	}  
}

/**
********************************************************************************
* @brief    HT162X IC에 data byte 전송 함수
* @param    disdata : data\n
* @return   none
* @remark   
********************************************************************************
*/
void ht162XTranData(INT8U disdata)       
{
    INT8U k;
    
	for (k = 0; k < 8; k++)
	{
		if ((disdata & 0x01) == 0)
		{
			DISPLAY_DATA(0x00);
			Delay_10();
			DISPLAY_DATA(0x02);
			Delay_10();

		}
		else
		{
			DISPLAY_DATA(0x00);
			Delay_10();
			DISPLAY_DATA(0x03);
			Delay_10();

		}
		disdata >>= 1;
	}  
}

/**
********************************************************************************
* @brief    HT162X IC에 dispTransPage의 Image를 Display Driver IC로 전송
* @param    disdata : data\n
* @return   none
* @remark   
********************************************************************************
*/
void ht162XWriteAllData(INT8U type)         
{
    INT8U i;
	INT8U *dispBufPtr;

	if (type == DIFFUSE_12CHAR) {
		dispBufPtr = DSP_buf_ptr + DSP_MAX_DIGIT + 13;
		goto SEND_ONLY_CHAR; //only char
	}

       DISPLAY_CON(0x0E);
	DISPLAY_DATA(0x01);
	Delay_10();
	DISPLAY_DATA(0x03);
       Delay_10();
	DISPLAY_DATA(0x00);
	Delay_10();
	DISPLAY_DATA(0x02);
	Delay_10();
 
       ht162XTranCommand(0x80); //Write from memory address 0

    dispBufPtr = DSP_buf_ptr;
	for (i = 0; i < DSP_MAX_DIGIT + 13; i++)
	{   
        ht162XTranData(*dispBufPtr);
		dispBufPtr++;
	}
	
	DISPLAY_CON(0x1E);    //serial control  //DISPLAY_CON(0x2E);
	Delay_10();


	if (type == DIFFUSE_7SEG) return; //only 7 segment
SEND_ONLY_CHAR:
	return;
}

void DSP_LCD7_diffuse_by_type(INT8U type)
{
	//esd protection
	if(type == 0) {
		ht162XWriteCommand(HT162X_ON);	
		ht162XWriteCommand(HT162X_SYS_EN); /* Turn on system oscillator*/	
	}
	ht162XWriteAllData(type);
}

void DSP_LCD7_diffuse(void)
{
//	ht162XWriteAllData(DIFFUSE_ALL);
	DSP_LCD7_diffuse_by_type(DIFFUSE_ALL);
}
void DSP_clear_all(void)
{
	memset(DSP_Memory, 0x00, sizeof(DSP_Memory));
	DSP_LCD7_diffuse();
}

void VFD7_write_byte(char Data)
{

}        

void VFD7_write_cmd(char Data, INT8U stb)	// Local Command
{     
	
} 
 

void VFD7_display_On(void)
{
	
} 

void VFD7_display_Off(void)
{
	
}

void VFD7_display_bright(INT8U bright)//1~4 level
{

}


void VFD7_Init(void)
{
        //DSP_Init(); 
	DSP_buf_ptr = DSP_Memory;

//	DSP_SET_RD2_HIGH;

	ht162XWriteCommand(HT162X_ON);	
	ht162XWriteCommand(HT162X_SYS_EN); /* Turn on system oscillator*/	
	ht162XWriteCommand(HT162X_TONE_OFF); /* Turn off tone outputs */		
	ht162XWriteCommand(HT162X_RC_32K); /* System clock source, on-chip RC oscillator*/
	ht162XWriteCommand(HT162X_TIMER_DIS);
	ht162XWriteCommand(HT162X_WDT_DIS);
	//ht162XWriteCommand(HT162X_BIAS_1_3);	
	ht162XWriteCommand(HT162X_IRQ_DIS);	/* Disable IRQ output */	
	ht162XWriteCommand(HT162X_NORMAL);	/* Normal mode*/

	DSP_clear_all();
	ht162XWriteAllData(DIFFUSE_ALL);
}

void VFD7_display_char(INT8U nDigitPos, char cData)
{
	//DSP_set_char(nDigitPos, cData);

	INT8U backup_triangle;
	INT8U pos, pos0, pos1,pos2;
	INT8U fData,tempdata;

	if (nDigitPos < (INT8U)DSP_MAX_DIGIT) {  //DSP_MAX_DIGIT
		pos0 = nDigitPos*3;
		pos1 = pos0 + 1;//second byte
		pos2 = pos1 + 1;

		//DSP_Memory[pos1] = DSP_Memory[pos1] & ~COMMA_SEG;
    DSP_Memory[pos1] = DSP_Memory[pos1];// & ~COMMA_SEG;  yoo 100107
		backup_triangle = DSP_Memory[pos2] & TRIANGLE_SEG;

		if( cData >= '0' && cData <= '9' ) {
			fData = ucNumSegTable[cData-'0'];
		}
		else if ( cData >= 'a' && cData <= 'z' ) {
			fData = ucAlphaSegTable[cData-'a'];
		}
		else if( cData >= 'A' && cData <= 'Z' ) {
			fData = ucAlphaSegTable[cData-'A'];
		}
		else if( cData == '_') {
			fData = SPACE_SEG;
		} else if( cData == '-') {
			fData = 0x10;
		} else if( cData == 0x20) {
			fData = SPA_SEG;
		}
		else if(cData == CURSOR_CHAR) {
			fData = CURSOR_SEG;
		}
		else if (cData == '\0') {
			fData = '\0';
		}
		else {
			fData = OTHER_SEG;
		}
		if( cData == '.') {
			if(nDigitPos < 13){
			DSP_Memory[pos2] = DSP_Memory[pos2] | DOT_SEG;
			}else{
				DSP_Memory[pos2-39] =  DSP_Memory[pos2-39]  | 0x20;
				}
		} else if( cData == ',') {
		      if(nDigitPos < 13){
			DSP_Memory[pos2] = DSP_Memory[pos2] | DOT_SEG;
			DSP_Memory[pos2] = DSP_Memory[pos2] | COMMA_SEG;
			}else{
			DSP_Memory[pos2-39] =  DSP_Memory[pos2-39]  | 0x20;
			DSP_Memory[pos2-39] =  DSP_Memory[pos2-39]  | 0x40;
			}
		} else {
		      if(nDigitPos < 13){
	DSP_Memory[pos0] = DSP_Memory[pos0] & 0x38;
	DSP_Memory[pos1] = DSP_Memory[pos1] & 0x78;
	DSP_Memory[pos2] = DSP_Memory[pos2] & 0x71;    
			DSP_Memory[pos0] =  DSP_Memory[pos0] | (0x07 & fData);// | backup_triangle;
			DSP_Memory[pos1] =  DSP_Memory[pos1] | (0x07 & (fData>>3));
			DSP_Memory[pos2] =  DSP_Memory[pos2] | (0x02 & (fData>>5));
			}else{
	DSP_Memory[pos0-39] = DSP_Memory[pos0-39] & 0x07;
	DSP_Memory[pos1-39] = DSP_Memory[pos1-39] & 0x47;
	DSP_Memory[pos2-39] = DSP_Memory[pos2-39] & 0x0F;                       
		    tempdata = 0x07 & fData; 
			DSP_Memory[pos0-39] =  DSP_Memory[pos0-39] | (tempdata<<3);// | backup_triangle;
			tempdata =  0x07 & (fData>>3);
			DSP_Memory[pos1-39] =  DSP_Memory[pos1-39] | (tempdata<<3);
			tempdata =   0x01 & (fData>>6);
			DSP_Memory[pos2-39] =  DSP_Memory[pos2-39] | (tempdata<<4);
			}			      	
		}

		if (FALSE == blink_flag) return;
		
		if ((nDigitPos >= blink_spos) && (nDigitPos <= blink_epos))
		{
			DSP_Memory_blink[pos0] = 0;
			DSP_Memory_blink[pos0] = DSP_Memory[pos0] & TRIANGLE_SEG;	//CJK070215
			DSP_Memory_blink[pos1] = 0;
		} else {
			DSP_Memory_blink[pos0] = DSP_Memory[pos0];
			DSP_Memory_blink[pos1] = DSP_Memory[pos1];
		}
	} else if (nDigitPos < (INT8U)(DSP_MAX_DIGIT + (DSP_MAX_CHAR/2))) {
		pos0 = DSP_MAX_DIGIT*2 + (nDigitPos - DSP_MAX_DIGIT)*5*2;
		for (pos = 0; pos < 5; pos++) {
			//DSP_Memory[pos0] = Font_bread(DspPtrFont1 + (INT8U)cData*6+pos);//font ID = 2. 8*6 with rotate 90 deg (range 0x00~0xFF)
			DSP_Memory_blink[pos0] = DSP_Memory[pos0];
			pos0 = pos0 + 2;
		}
	} else if (nDigitPos < (INT8U)(DSP_MAX_DIGIT + DSP_MAX_CHAR)) {
		pos0 = DSP_MAX_DIGIT*2 + (DSP_MAX_CHAR - (nDigitPos - DSP_MAX_DIGIT))*5*2 - 1;
		for (pos = 0; pos < 5; pos++) {
			//DSP_Memory[pos0] = Font_bread(DspPtrFont1 + (INT8U)cData*6+pos);//font ID = 2. 8*6 with rotate 90 deg (range 0x00~0xFF)
			DSP_Memory_blink[pos0] = DSP_Memory[pos0];
			pos0 = pos0 - 2;
		}
	} else return;
	
}

void VFD7_display_ucomma(INT8U nDigitPos, INT8U onoff)
{
	
}

void VFD7_display_triangle(INT8U nDigitPos, INT8U onoff)
{
	//DSP_set_triangle(nDigitPos,onoff);
#ifdef USE_PB_BATTERY
	if(nDigitPos > SIGN_BAT3) return;
#else
	if(nDigitPos >= DSP_MAX_DIGIT) return;
#endif

	if(nDigitPos == SIGN_TARE){
		if (onoff) DSP_Memory[1] |= 0x04;    // display_sign_noflush(SIGN_TARE,1);
		else DSP_Memory[1] &= 0xFB;   //display_sign_noflush(SIGN_TARE,0);
	} 
	else if(nDigitPos == SIGN_ZERO){	
		if (onoff) DSP_Memory[2] |= 0x04;    //			display_sign_noflush(SIGN_ZERO,1);
		else DSP_Memory[2] &= 0xFB;    //	  //			display_sign_noflush(SIGN_ZERO,0);
	}
	else if(nDigitPos == SIGN_STABLE){		
		if(onoff) DSP_Memory[0] |= 0x04;    //			display_sign_noflush(SIGN_STABLE,1);
		else  DSP_Memory[0] &= 0xFB;   //			display_sign_noflush(SIGN_STABLE,0);
	}      
	else if(nDigitPos == SIGN_MASTER){		
		if(onoff) DSP_Memory[1] |= 0x01;    //			master signal
		else  DSP_Memory[1] &= 0xFE;   //	
	}      
	else if(nDigitPos == SIGN_CONNECT){		
		if(onoff) DSP_Memory[0] |= 0x01;    //			slave connection signal
		else  DSP_Memory[0] &= 0xFE;   //			
	}     		
	else if(nDigitPos == SIGN_ETH_LED_LINK){		
		if(onoff) DSP_Memory[2] |= 0x08;    // ANT Sign			
		else  DSP_Memory[2] &= 0xF7;   //			
	}   
#ifdef USE_PB_BATTERY
	else if(nDigitPos == SIGN_BAT1){		
		if(onoff) DSP_Memory[0] |= 0x02;    // TEST ONLY: BAT1 Signal
		else  DSP_Memory[0] &= 0xFD;   //			
	}
	else if(nDigitPos == SIGN_BAT2){		
		if(onoff) DSP_Memory[1] |= 0x02;    // TEST ONLY: BAT2 Signal
		else  DSP_Memory[1] &= 0xFD;   //			
	}
	else if(nDigitPos == SIGN_BAT3){		
		if(onoff) DSP_Memory[2] |= 0x02;    // TEST ONLY: BAT3 Signal
		else  DSP_Memory[2] &= 0xFD;   //			
	}
#endif
	//  else if(nDigitPos == SIGN_ETH_LED_LINK){		
	//		  if(onoff) DSP_Memory[1] |= 0x02;    // TEST ONLY: BAT2 Signal
	//		  else  DSP_Memory[1] &= 0xFD;   //			
	//  }
	else{
		if(onoff){
			if(nDigitPos < 13)DSP_Memory[nDigitPos*3+2] |= 1;//TRIANGLE_SEG;
			else DSP_Memory[(nDigitPos*3 - 39)+1] |= 0x40;//TRIANGLE_SEG;
		}else {
			if(nDigitPos < 13)DSP_Memory[nDigitPos*3+2] &= 0xFE;//(~TRIANGLE_SEG);
			else DSP_Memory[(nDigitPos*3 - 39)+1] &= 0xBF;//(~TRIANGLE_SEG);
		}
	}
}

static inline void __display_string_reverse(char *dataString, char size, INT8U mode)
{
	INT16S start, end;
	start = display_parameter.mode_pos[mode];
	end = start + display_parameter.mode_len[mode] - 1;

	while (size && start <= end) {
		char ch = dataString[--size];

		if (ch == '.' || ch == ',') {
			if (size - 1 >= 0) {
				VFD7_display_char(end, dataString[size - 1]);
				size--;
			}
		}
		VFD7_display_char(end, ch);
		end--;
	}

	while(start <= end)
	{
		VFD7_display_char(end, '\0');
		end--;
	}
}

static inline void __display_string(char *dataString, INT8U nDigitPos, char size, INT8U type_7seg)
{
	INT8U i,j;

	i=nDigitPos;
	j=0; 
//    if ((nDigitPos+size)>=DSP_MAX_DIGIT) return;
	while((*(dataString) != '\0') && (j<size))
	{
		VFD7_display_char(i, *dataString);

		if (type_7seg) {
			if(*(dataString+1) != '\0') 
			{
				if(*(dataString+1) == '.' || *(dataString+1) == ',') 
				{
					VFD7_display_char(i, *(dataString+1)); 
					dataString++;
				}
			}
		}

		dataString++;
		i++;
		j++;
	}   
	//clear remain area
	while(j<size)
	{
		VFD7_display_char(i, '\0');
		i++;
		j++;
	}
//	DSP_LCD7_diffuse();//comm speed slow
}

void VFD7_display_string( char *dataString, INT8U nDigitPos, char size, INT8U mode, BOOLEAN reverse)
{
	//DSP_str_common(( char *)dataString, nDigitPos, size);
	INT8U i,j;
	INT8U type_7seg;

	
	if (nDigitPos < (INT8U)DSP_MAX_DIGIT) type_7seg = 1;
	else type_7seg = 0;

	i=nDigitPos;
	j=0; 

	if (reverse && mode != DISPLAY_UNDEFINED) {
		__display_string_reverse(dataString, size, mode);
	} else {
		__display_string(dataString, nDigitPos, size, type_7seg);
	}
}

void VFD7_Diffuse(void)
{
	DSP_LCD7_diffuse();	
	
}

void VFD7_Fill_Buffer(unsigned char x)
{
	//DSP_LCD7_Fill_Buffer(x);
	memset(DSP_Memory, x, sizeof(DSP_Memory));
}

void VFD7_save_restore(INT8U x)
{
}

#endif
