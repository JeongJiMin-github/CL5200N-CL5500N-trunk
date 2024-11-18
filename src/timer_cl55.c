/*****************************************************************************
|*
|*  Copyright		:	(c) 2002 CAS
|*  Filename		:	timer.c
|*  Version			:	0.1
|*  Programmer(s)	:	Hong Yi-Phyo (HYP)
|*  Created			:	2002/11/11
|*  Description		:	CLP Printer timer routine
|*
****************************************************************************/
#include <stdio.h>
#include "globals.h"
#include "commbuf.h"
#include "prt_common.h"
#include "prtdrv.h"
#include "bar_common.h"
#include "timer.h"
#include "prt_render.h"
#include "prt_font.h"
#include "sensor.h"
#include "key.h"
#include "main.h"

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include "system_config.h"
#include "system/system.h"
#include "peripheral/peripheral.h"
#include "system/devcon/sys_devcon.h"
#include "bsp_config.h"
#include "wdt.h"

#include "prt_cnvtable.h"
#ifdef USE_SRAM_SPI_COMM
#include "spi_sram.h"
#endif

/*
 *   Timer_Rate = Osc/(N*(65536-Timer_Reload_Value)
 *   Osc = 30 Mhz
 *   N   = prescale value = 4 (SCR)
 *
 *   Reload_Value = 65536 - Osc/(N*Timer_Rate)
 */
#define PRESCALE_VALUE		1			/* change for non-default Prescale value (SCR register)*/
#define OSC					96000000	/* change for different oscillator frequency */
#define MAX_TIMER_RATE		OSC/PRESCALE_VALUE
#define MIN_TIMER_RATE		OSC/(PRESCALE_VALUE*65536)
#define MIN_RELOAD_VALUE	0
#define MAX_RELOAD_VALUE	65535


/* Erase non-use variable HYP 20060412
volatile _bitbyte	TphBitByte;
_bit     TphBit0  _atbit( TphBitByte, 0 );
_bit     TphBit1  _atbit( TphBitByte, 1 );
_bit     TphBit2  _atbit( TphBitByte, 2 );
_bit     TphBit3  _atbit( TphBitByte, 3 );
_bit     TphBit4  _atbit( TphBitByte, 4 );
_bit     TphBit5  _atbit( TphBitByte, 5 );
_bit     TphBit6  _atbit( TphBitByte, 6 );
_bit     TphBit7  _atbit( TphBitByte, 7 );
*/

extern INT8U Inline_Ror8(INT8U ch, INT8U count);
extern INT8U Inline_Rol8(INT8U ch, INT8U count);

void Inline_Tph_Data_Tx(INT8U tph_buf)
{
}

void Inline_Tph_Data_Tx_Rev(INT8U tph_buf)
{
}

void Inline_DIS_Data_Tx(INT16U dis_buf)
{
}

void Inline_DIS_Data_Tx_Rev(INT8U dis_buf)
{
}

void Inline_Tph_Data_Cpy_Rev(INT8U *dst, INT8U *src, INT16U cnt)
{
	INT16U i;

	for (i = 0; i < cnt; i++)
	{
		dst[i] = CnvTableMirror[src[cnt-i-1]];
	}
}

//#pragma optimize 2
					/*		100 200 300 400 500 600 700 800 800 800	*/
ROMDATA INT16U Timer0ExtReloadVal[10] = {8, 8, 8, 	8, 	8, 	8, 	8, 	8, 	8, 	8};	 //100~800 Hz

#define SENSOR_ON_DELAY	4
#define SENSOR_ON_DELAY_MOTOR	10       //HYP
#define SENSOR_OFF_DELAY_MOTOR	10

//HYP 20040401
#define HEADUP_PEEL_SEN_NOT_CONTROLLED

//#define USE_REWIND_WAIT//HYP 20060123
#ifdef USE_REWIND_WAIT
INT16U RewindWaitCount = 0;
#define REWIND_WAIT_COUNT	40 //0.125*40 = 5mm
#endif

#ifdef PRT_REVERSE_FEED
//HYP 20060309
//if power is on or head up is on, Disable reverse feed one time.
INT8U ReverseFeedEnable = OFF;
#endif

#ifdef USE_CHECK_OUTOFPAPER_ON_LABEL
INT8U PaperEndCount = 0;
#define PAPEREND_CHECK_LEN (8*5)	// 5mm (0.125mm * 8 = 1mm)
#endif
extern HUGEDATA INT8U	tph_tx_memory[56];
//extern void dma_renew(void);
void __attribute__((optimize("-O0"))) prt_delay(INT8U cnt)
{
    INT8U i;
    for(i = 0; i < cnt; i++);
}

void SPISendByte(INT8U data)
{
    PLIB_SPI_BufferWrite(SPI_ID_2, data);

	while(!PLIB_SPI_TransmitBufferIsEmpty(SPI_ID_2));
}

void SPISendStr(INT8U *data, INT16U len)
{
    INT8U i = 0;

    for( i=0; i<len; i++)
    {
        PLIB_SPI_BufferWrite(SPI_ID_2, data[i]);

	    while(!PLIB_SPI_TransmitBufferIsEmpty(SPI_ID_2));
    }
}

//void dma_send()
//{
//    PLIB_DMA_Enable(DMA_ID_0);
//    PLIB_DMA_ChannelXSourceStartAddressSet(DMA_ID_0, DMA_CHANNEL_0, (uint32_t)tph_tx_memory);
//    PLIB_DMA_ChannelXDestinationStartAddressSet(DMA_ID_0, DMA_CHANNEL_0, (uint32_t)&SPI2BUF);
//    PLIB_DMA_ChannelXSourceSizeSet(DMA_ID_0, DMA_CHANNEL_0, TPH_ROW_BYTE);
//    PLIB_DMA_ChannelXDestinationSizeSet(DMA_ID_0, DMA_CHANNEL_0, 1);
//    PLIB_DMA_ChannelXCellSizeSet(DMA_ID_0, DMA_CHANNEL_0, 1);
//    PLIB_DMA_ChannelXAutoDisable( DMA_ID_0, DMA_CHANNEL_0);
//    PLIB_DMA_ChannelXTriggerEnable(DMA_ID_0, DMA_CHANNEL_0, DMA_CHANNEL_TRIGGER_TRANSFER_START);
//    PLIB_DMA_ChannelXStartIRQSet(DMA_ID_0, DMA_CHANNEL_0, DMA_TRIGGER_SPI_2_TRANSMIT);
//    PLIB_DMA_StartTransferSet (DMA_ID_0, DMA_CHANNEL_0);
//    PLIB_DMA_ChannelXEnable(DMA_ID_0, DMA_CHANNEL_0);
//
//    while(1)
//    {
//        if(PLIB_DMA_ChannelXBusyIsBusy( DMA_ID_0,DMA_CHANNEL_0) == true)
//        {
//            ;
//        }
//        else
//        {
//            //PLIB_DMA_StartTransferSet ( DMA_ID_0, DMA_CHANNEL_0 );
//            PLIB_DMA_ChannelXDisable(DMA_ID_0, DMA_CHANNEL_0);
//            break;
//        }
//    }
//}

INT8U IsHeadupState(void)
{
    INT8U headup;
    headup = HEADUP_PORT;//port open = 0, close = 1
#if defined(_CL5500_PRINTER_MECH) || defined(_CL5500_D_PRINTER_MECH)
    if (!headup)
    {
        return 0; //open
    }
    else
    {
        return 1; //close
    }
#else    
    if (!headup)
    {
        return 1; //open
    }
    else
    {
        return 0; //close
    }
#endif
}

void Timer0Int(void)
{
	static INT8U State_Delay = 0;//HYP 20040223 //HYP 20090116 change name
	INT8U tph_buf;
	HUGEDATA INT8U	*tph_memory;
	HUGEDATA INT8U	*tph_memory_reverse;
#ifdef USE_SRAM_PRT_RENERING_BUF
    INT32U pSram = 0;
    INT8U *pTempSramBuff;
    INT32U tempSize = 0;
#endif // USE_SRAM_PRT_RENERING_BUF

	INT8U column_byte_count, sm_signal;
	INT32U timerVal;

	switch (PrtDrvStruct.PrtState)
	{
		case PRT_STATE0E_HOLD_MOTOR:
			if(State_Delay == 0)
			{
				State_Delay = SENSOR_OFF_DELAY_MOTOR;//same as start delay
				break;
			}
			else
			{
				State_Delay--;
				if(State_Delay != 0)
				{
					break;
				}
			}
			PrtDrvStop();
			break;

		case PRT_STATE_CSS:
			PrtDrvStruct.PrtState = PRT_STATE_CS;
			break;

		case PRT_STATE_CS:
			if(State_Delay == 0)
			{
				State_Delay = SENSOR_ON_DELAY;
				break;
			}
			else
			{
				State_Delay--;
				if(State_Delay != 0)
				{
					break;
				}
			}

#ifdef PEEL_SENSOR_REFLECTIVE
			if (SensorStruct.PeelFilVal < SensorStruct.PeelThreshold)
			{
#else
			if (SensorStruct.PeelFilVal > SensorStruct.PeelThreshold)
			{
#endif
				PrtDrvStruct.HangingFormFlag = OFF;
			}
			else
			{
//HYP 20040524
#ifdef HEADUP_PEEL_SEN_NOT_CONTROLLED
				if(PrtDrvStruct.PeelEnable == ENABLE)
				{
					PrtDrvStruct.HangingFormFlag = ON;
				}
				else
				{
					PrtDrvStruct.HangingFormFlag = OFF;
				}
#else
				PrtDrvStruct.HangingFormFlag = ON;
#endif//HEADUP_PEEL_SEN_NOT_CONTROLLED
			}
			if (HEADUP)
			{
				PrtDrvStruct.HeadUpFlag = ON;
			}
			else
			{
				PrtDrvStruct.HeadUpFlag = OFF;
			}

			//HYP 20101021
			if (SensorStruct.GapFilVal > SensorStruct.GapOutOfPaperThreshold) {
				PrtDrvStruct.GapSensorStatus = GAP_STATUS_OUTOFPAPER;
			} else if (SensorStruct.GapFilVal >= SensorStruct.GapThreshold) {
				PrtDrvStruct.GapSensorStatus = GAP_STATUS_GAP;
			} else {
				PrtDrvStruct.GapSensorStatus = GAP_STATUS_LABEL;
			}

			PrtDrvStruct.PrtState = PRT_STATE0I;
			PEEL_OFF;
			GAP_OFF;
			break;

		case PRT_STATE_RW:
			if (HEADUP)
			{
				PrtDrvStruct.HeadUpFlag = ON;
				PrtDrvStruct.RewindVariable = REWIND_LENGTH;
				PRT_M_EN_PORT_HIGH;
#ifndef _CL3000_PRINTER_MECH
				PRT_R_EN_PORT_HIGH;
#endif
				PrtDrvStruct.PrtState = PRT_STATE0I;
				break;
			}
			else
			{
				PrtDrvStruct.HeadUpFlag = OFF;
			}
			if(PrtDrvStruct.RewindEnable == ENABLE)
			{
				PrtDrvStruct.RewindStepMotorSignal = Inline_Ror8(PrtDrvStruct.RewindStepMotorSignal,1);
				STEP_MOTOR_CON((PrtDrvStruct.RewindStepMotorSignal | 0xf0) & 0xf3);	//L6219
				if(PrtDrvStruct.RewindVariable == 0)
				{
					PrtDrvStruct.PrtState = PRT_STATE0I;
					PrtDrvStruct.RewindVariable = REWIND_LENGTH;
					STEP_MOTOR_CON(0xff);	//L6219
				}
				else
				{
					PrtDrvStruct.RewindVariable--;
				}
			}
			else
			{
				PrtDrvStruct.PrtState = PRT_STATE0I;
			}
			break;

		case PRT_STATE_TM:
			if(PrtDrvStruct.SysTimer != 0xffff)
			{
				PrtDrvStruct.SysTimer++;
			}
			break;

#ifdef PRT_REVERSE_FEED
		case PRT_STATE_RFS:
			if(ReverseFeedEnable == OFF)
			{
				ReverseFeedEnable = ON;
//				PrtDrvStruct.PrtState = PRT_STATE0I;
				PrtDrvStruct.PrtState = PRT_STATE_RF;
				break;
			}
#endif
		case PRT_STATE_RFFS:
		case PRT_STATE_FFS:
		case PRT_STATE_LFS:
		case PRT_STATE_LCS:
		case PRT_STATE_SCS:
		case PRT_STATE0S:
#ifdef HEADUP_PEEL_SEN_NOT_CONTROLLED
			PrtDrvStruct.HeadUpFlag = OFF;
#else
			if (HEADUP)
			{
				PrtDrvStruct.HeadUpFlag = ON;
				PrtDrvStruct.LabelCopyCount = 1;
				PrtDrvStruct.PrtState = PRT_STATE0E;
				break;
			}
			else
			{
				PrtDrvStruct.HeadUpFlag = OFF;
			}
#endif //HEADUP_PEEL_SEN_NOT_CONTROLLED
			PEEL_ON;
			GAP_ON;
			TPH_ON;
			if(State_Delay == 0)
			{
				State_Delay = SENSOR_ON_DELAY_MOTOR;
				sm_signal = (PrtDrvStruct.MainStepMotorSignal | 0x0f) & 0x3f;
				if(PrtDrvStruct.RewindEnable == ENABLE)
				{
					sm_signal = sm_signal & ((PrtDrvStruct.RewindStepMotorSignal | 0xf0) & 0xf3);
				}
				STEP_MOTOR_CON(sm_signal);	//L6219
#ifdef USE_REWIND_WAIT
				RewindWaitCount = REWIND_WAIT_COUNT;//HYP 20060123
#endif
				break;
			}
			else
			{
				State_Delay--;
				if(State_Delay != 0)
				{
					if (State_Delay == 1)
					{
						switch (PrtDrvStruct.PrtState)
						{
#ifdef PRT_REVERSE_FEED
							case PRT_STATE_RFS:
								PrtDrvStruct.MainStepMotorSignal = Inline_Rol8(PrtDrvStruct.MainStepMotorSignal,1);
								if(PrtDrvStruct.RewindEnable == ENABLE)
								{
									PrtDrvStruct.RewindStepMotorSignal = Inline_Rol8(PrtDrvStruct.RewindStepMotorSignal,1);
								}
								sm_signal = (PrtDrvStruct.MainStepMotorSignal | 0x0f) & 0x3f;
								if(PrtDrvStruct.RewindEnable == ENABLE)
								{
									sm_signal = sm_signal & ((PrtDrvStruct.RewindStepMotorSignal | 0xf0) & 0xf3);
								}
								STEP_MOTOR_CON(sm_signal);	//L6219
								break;
#endif//#ifdef PRT_REVERSE_FEED
							case PRT_STATE_RFFS:
							case PRT_STATE_FFS:
							case PRT_STATE_LFS:
							case PRT_STATE_LCS:
							case PRT_STATE_SCS:
								/* Step motor drive 1 step */
								PrtDrvStruct.MainStepMotorSignal = Inline_Ror8(PrtDrvStruct.MainStepMotorSignal,1);
								if(PrtDrvStruct.RewindEnable == ENABLE)
								{
#ifdef USE_REWIND_WAIT	//HYP 20060123
									if(RewindWaitCount == 0)
									{
										PrtDrvStruct.RewindStepMotorSignal = Inline_Ror8(PrtDrvStruct.RewindStepMotorSignal,1);
									}
									else
									{
										RewindWaitCount--;
									}
#else
									PrtDrvStruct.RewindStepMotorSignal = Inline_Ror8(PrtDrvStruct.RewindStepMotorSignal,1);
#endif
								}
								sm_signal = (PrtDrvStruct.MainStepMotorSignal | 0x0f) & 0x3f;
								if(PrtDrvStruct.RewindEnable == ENABLE)
								{
									sm_signal = sm_signal & ((PrtDrvStruct.RewindStepMotorSignal | 0xf0) & 0xf3);
								}
								STEP_MOTOR_CON(sm_signal);	//L6219
								break;
						}
					}
					break;
				}
			}
			PrtDrvStruct.GapTraceFlag = 0;
			PrtDrvStruct.FindGapFeedCount = 0;
			PrtDrvStruct.FindLabelFeedCount = 0;
			switch (PrtDrvStruct.PrtState)
			{
				case PRT_STATE_RFFS:
					PrtDrvStruct.ReceiptFeedVariable = PrtDrvStruct.ReceiptFeedHeight;
					PrtDrvStruct.PrtState = PRT_STATE_RFF;
					break;
				case PRT_STATE_FFS:
					PrtDrvStruct.PrtState = PRT_STATE_FF;
					if(PrtStruct.Mode == LABEL)
					{
						if(PrtDrvStruct.PrePrintMode == ON)
						{
							if(PrtDrvStruct.PrePrtState == PRE_PRT_STATE0)
							{
								if(PrtDrvStruct.LabelGapMode == GAP) PrtDrvStruct.FormFeedVariable = 80;//10 mm feed and find gap
								else PrtDrvStruct.FormFeedVariable = PrtDrvStruct.FormFeedLength-PrtDrvStruct.PrePrintLength-1;
							}
							else//PRE_PRT_STATE1FF
							{
								PrtDrvStruct.FormFeedVariable = PrtDrvStruct.PrePrintLength;
							}
						}
						else
						{
							if(PrtDrvStruct.LabelGapMode == GAP) PrtDrvStruct.FormFeedVariable = 80;//10 mm feed and find gap
						}
					}
					break;
				case PRT_STATE_LFS:
					PrtDrvStruct.PrtState = PRT_STATE_LF;
					break;
#ifdef PRT_REVERSE_FEED
				case PRT_STATE_RFS:
					PrtDrvStruct.PrtState = PRT_STATE_RF;
					break;
#endif
				case PRT_STATE0S:
#ifdef USE_MORNITORING_CNT
                    PrtTphUseData();
#endif
					PrtDrvStruct.PrtState = PRT_STATE1;
					break;
				case PRT_STATE_LCS:
					PrtDrvStruct.PrtState = PRT_STATE_LC;
					SensorStruct.TestGapStartFlag = 0;
					SensorStruct.TestLabelStartFlag = 0;
					if(SensorStruct.GapFilVal >= SensorStruct.GapThreshold)//gap
					{
						SensorStruct.TestGapFlag = 1;
					}
					else//label
					{
						SensorStruct.TestGapFlag = 0;
					}
					SensorStruct.TestPreGapFlag = SensorStruct.TestGapFlag;
					break;
				case PRT_STATE_SCS:
					PrtDrvStruct.PrtState = PRT_STATE_SC;
					SensorStruct.MaxGapValue = 0;
					SensorStruct.MinGapValue = 0xff;
					SensorStruct.MaxPeelValue = 0;
					SensorStruct.MinPeelValue = 0xff;
					SensorStruct.TestGapStartFlag = 0;
					SensorStruct.TestLabelStartFlag = 0;
					if(SensorStruct.GapFilVal >= SensorStruct.GapThreshold)//gap
					{
						SensorStruct.TestGapFlag = 1;
					}
					else//label
					{
						SensorStruct.TestGapFlag = 0;
					}
					SensorStruct.TestPreGapFlag = SensorStruct.TestGapFlag;
					break;
			}
			break;
		case PRT_STATE0E:
			if (PrtDrvStruct.PrtDirection == NON_INVERT)
			{
				PrtDrvStruct.TphMemRowCount = 0;
			}
			else
			{
				PrtDrvStruct.TphMemRowCount = PrtDrvStruct.TphMemMaxRowNum - 1;
			}
			if(PrtDrvStruct.LabelCopyCount != 0)
			{
				PrtDrvStruct.LabelCopyCount--;
			}
			if (PrtDrvStruct.LabelCopyCount == 0)
			{
				if(PrtStruct.Mode == RECEIPT)
				{
					PrtStruct.ReceiptModeState = RM_INIT;
					PrtDrvStruct.TphMemMaxRowNum = StrStruct.LineSp;
                #ifdef USE_SRAM_PRT_RENERING_BUF
                    SramPrtMemory = SramTphMemoryA + StrStruct.LineSp*TPH_ROW_BYTE;//start ptr
                #else //  USE_SRAM_PRT_RENERING_BUF
					PrtStruct.PrtMemory = PrtDrvStruct.TphMemoryA + StrStruct.LineSp*TPH_ROW_BYTE;//start ptr
				#endif // USE_SRAM_PRT_RENERING_BUF
                    PrtStruct.PrintableHeight = MAX_RECEIPT_LEN;
#ifdef USE_FORMFEED_WITH_PRE_PRINT //HYP 20060428
					PrtDrvTphMemoryClear(OFF);
#else
					PrtDrvTphMemoryClear();
#endif
					PrtStruct.PrintableHeight = PrtStruct.PrintableHeight - StrStruct.LineSp;
					PrtStruct.PrintHeight = PrtStruct.PrintableHeight - PrtStruct.TMargin - PrtStruct.BMargin;
				}
				else
				{
					if((PrtDrvStruct.PrePrintMode == ON) && (PrtDrvStruct.PrePrtState != PRE_PRT_STATE0))
					{
						if((PrtDrvStruct.OutOfPaperFlag == OFF) && (PrtDrvStruct.GapSenErrFlag == OFF))
						{
							switch(PrtDrvStruct.PrePrtState)
							{
								case PRE_PRT_STATE1PR:
									PrtDrvStruct.PrtState = PRT_STATE0S;
									break;
								case PRE_PRT_STATE1FF:
									PrtDrvStruct.PrtState = PRT_STATE_FFS;
									break;
								default:
									PrtDrvStruct.PrePrtState = PRE_PRT_STATE0;
									break;
							}
						}
						else
						{
							PrtDrvStruct.PrePrtState = PRE_PRT_STATE0;
						}
					}
					else
					{//PrtDrvStruct.PrePrtState == state0
						if((PrtDrvStruct.PrtLoadFormFlag == ON) && (PrtDrvStruct.HeadUpFlag == OFF) && (PrtDrvStruct.HangingFormFlag == OFF))
						{
#ifdef USE_ONLY_1EA_PRT_RENDERING_BUF
                            PrtDrvStruct.ExistBackGround = OFF;
#endif
							if(PrtDrvStruct.ExistBackGround == ON)
							{
#ifdef USE_FORMFEED_WITH_PRE_PRINT //HYP 20060428
								PrtDrvTphMemoryCopy((HUGEDATA INT16U *)PrtDrvStruct.TphMemoryA,(HUGEDATA INT16U *)PrtDrvStruct.TphMemoryB,ON);
#else
                            #ifdef USE_SRAM_PRT_RENERING_BUF
                                SramTphMemoryA = SramTphMemoryB;
                            #else // USE_SRAM_PRT_RENERING_BUF
                                PrtDrvTphMemoryCopy((HUGEDATA INT16U *)PrtDrvStruct.TphMemoryA,(HUGEDATA INT16U *)PrtDrvStruct.TphMemoryB);
                            #endif // USE_SRAM_PRT_RENERING_BUF
#endif
							}
							else
							{
#ifdef USE_FORMFEED_WITH_PRE_PRINT //HYP 20060428
								PrtDrvTphMemoryClear(ON);
#else
								PrtDrvTphMemoryClear();
#endif
							}
						}
					}
				}
				if((PrtDrvStruct.HeadUpFlag == OFF) && (PrtDrvStruct.HangingFormFlag == OFF))
				{
					PrtDrvStruct.PrtCompleteFlag = ON;
				}
				if (PrtDrvStruct.PrtState == PRT_STATE0E)
				{
					PrtDrvStruct.PrtState = PRT_STATE0E_HOLD_MOTOR;
				}
				else
				{
					PrtDrvStop();
				}
			}
			else
			{
				if((PrtDrvStruct.OutOfPaperFlag == OFF) && (PrtDrvStruct.GapSenErrFlag == OFF))
				{
					PrtDrvStruct.PrtState = PRT_STATE0S;
				}
				else
				{
					PrtDrvStruct.PrtState = PRT_STATE0E;
					PrtDrvStruct.LabelCopyCount = 1;
				}
			}
			break;
#ifdef PRT_REVERSE_FEED
		case PRT_STATE_RF:	//not complete this mode
			PrtDrvStruct.MainStepMotorSignal = Inline_Rol8(PrtDrvStruct.MainStepMotorSignal,1);
			if(PrtDrvStruct.RewindEnable == ENABLE)
			{
				PrtDrvStruct.RewindStepMotorSignal = Inline_Rol8(PrtDrvStruct.RewindStepMotorSignal,1);
			}
			sm_signal = (PrtDrvStruct.MainStepMotorSignal | 0x0f) & 0x3f;
			if(PrtDrvStruct.RewindEnable == ENABLE)
			{
				sm_signal = sm_signal & ((PrtDrvStruct.RewindStepMotorSignal | 0xf0) & 0xf3);
			}
			STEP_MOTOR_CON(sm_signal);	//L6219
			// gap rewind
			if (SensorStruct.GapBufInPtr == 0) SensorStruct.GapBufInPtr = (GAP_SENSOR_BUF_SIZE-1);
			else SensorStruct.GapBufInPtr--;
			if (SensorStruct.GapBufOutPtr == 0) SensorStruct.GapBufOutPtr = (GAP_SENSOR_BUF_SIZE-1);
			else SensorStruct.GapBufOutPtr--;

			if (PrtDrvStruct.ReverseFeedVariable <= 0)
			{
				PrtDrvStruct.PrtState = PRT_STATE0E_HOLD_MOTOR;
				PrtDrvStruct.ReverseFeedVariable = PrtDrvStruct.ReverseFeedLength; //temp linefeed
				break;
			}
			else
			{
				PrtDrvStruct.ReverseFeedVariable--;
				break;
			}
#endif//#ifdef PRT_REVERSE_FEED

		case PRT_STATE4:
			if((PrtStruct.Mode == LABEL) && (PrtDrvStruct.PrePrintMode == ON))
			{
				if(PrtDrvStruct.PrePrtState == PRE_PRT_STATE0) goto STATE4_PRE;
			}
		case PRT_STATE_RFF:
		case PRT_STATE_LF:
		case PRT_STATE_FF:
		case PRT_STATE_LC:
		case PRT_STATE_SC:
		case PRT_STATEB:
		case PRT_STATE3:
		case PRT_STATE5:
			/* Step motor drive 1 step */
			PrtDrvStruct.MainStepMotorSignal = Inline_Ror8(PrtDrvStruct.MainStepMotorSignal,1);
			if(PrtDrvStruct.RewindEnable == ENABLE)
			{
#ifdef USE_REWIND_WAIT	//HYP 20060123
				if(RewindWaitCount == 0)
				{
					PrtDrvStruct.RewindStepMotorSignal = Inline_Ror8(PrtDrvStruct.RewindStepMotorSignal,1);
				}
				else
				{
					RewindWaitCount--;
				}
#else
				PrtDrvStruct.RewindStepMotorSignal = Inline_Ror8(PrtDrvStruct.RewindStepMotorSignal,1);
#endif
			}
			sm_signal = (PrtDrvStruct.MainStepMotorSignal | 0x0f) & 0x3f;
			if(PrtDrvStruct.RewindEnable == ENABLE)
			{
				sm_signal = sm_signal & ((PrtDrvStruct.RewindStepMotorSignal | 0xf0) & 0xf3);
			}
			STEP_MOTOR_CON(sm_signal);	//L6219
			//Gap update
			SensorStruct.GapBuf[SensorStruct.GapBufInPtr] = SensorStruct.GapFilVal;
			if (SensorStruct.GapBufInPtr >= (GAP_SENSOR_BUF_SIZE-1)) SensorStruct.GapBufInPtr = 0;
			else SensorStruct.GapBufInPtr++;
			if (SensorStruct.GapBufOutPtr >= (GAP_SENSOR_BUF_SIZE-1)) SensorStruct.GapBufOutPtr = 0;
			else SensorStruct.GapBufOutPtr++;

#ifdef USE_CHECK_OUTOFPAPER_ON_LABEL
			// check paper end
			if (PrtStruct.Mode == LABEL)
			{
				if (SensorStruct.GapOutOfPaperThresholdOnLabel > 0)
				{
					if (SensorStruct.GapFilVal > SensorStruct.GapOutOfPaperThresholdOnLabel) {
						if (++PaperEndCount >= PAPEREND_CHECK_LEN)	// 5mm
						{
							PrtDrvStruct.OutOfPaperFlag = ON;
							PrtDrvStruct.PrtState = PRT_STATE0E;
							PaperEndCount = 0;
							break;
						}
					}
					else
					{
						PaperEndCount = 0;
					}
				}
			}
#endif
			if (PrtDrvStruct.PrtState == PRT_STATE_LF)
			{
				if (PrtDrvStruct.LineFeedVariable <= 0)
				{
					PrtDrvStruct.PrtState = PRT_STATE0E;
					PrtDrvStruct.LineFeedVariable = PrtDrvStruct.LineFeedLength; //temp linefeed
					break;
				}
				else
				{
					PrtDrvStruct.LineFeedVariable--;
					break;
				}
			}
			//HYP 20040330
			if (PrtDrvStruct.PrtState == PRT_STATE_RFF)
			{
				if (PrtDrvStruct.ReceiptFeedVariable == 0)
				{
					PrtDrvStruct.PrtState = PRT_STATE0E;
					break;
				}
				else
				{
					PrtDrvStruct.ReceiptFeedVariable--;
					break;
				}
			}
			if (PrtDrvStruct.PrtState == PRT_STATE_FF)
			{
				if (PrtDrvStruct.FormFeedVariable <= 1)	   //PRT_STATE5(1 step)
				{
					if (PrtDrvStruct.LabelGapMode)
					{
						PrtDrvStruct.PrtState = PRT_STATE5;
						if((PrtStruct.Mode == LABEL) && (PrtDrvStruct.PrePrintMode == ON))
						{
							if(PrtDrvStruct.PrePrtState == PRE_PRT_STATE0)
							{
#ifdef USE_FORMFEED_WITH_PRE_PRINT//HYP 20060428
								PrtDrvStruct.PrePrtState = PRE_PRT_STATE1PR;
#else
								PrtDrvStruct.PrePrtState = PRE_PRT_STATE1FF;
#endif
							}
							else
							{//state1FF
								PrtDrvStruct.PrtState = PRT_STATE0E;
								PrtDrvStruct.PrePrtState = PRE_PRT_STATE0;
							}
						}
						PrtDrvStruct.FormFeedVariable = PrtDrvStruct.FormFeedLength;
						break;//HYP 20050321
					}
					else
					{
						PrtDrvStruct.PrtState = PRT_STATE0E;
						if((PrtStruct.Mode == LABEL) && (PrtDrvStruct.PrePrintMode == ON))
						{
							if(PrtDrvStruct.PrePrtState == PRE_PRT_STATE0)
							{
#ifdef USE_FORMFEED_WITH_PRE_PRINT//HYP 20060428
								PrtDrvStruct.PrePrtState = PRE_PRT_STATE1PR;
#else
								PrtDrvStruct.PrePrtState = PRE_PRT_STATE1FF;
#endif
							}
							else
							{//state1FF
								PrtDrvStruct.PrePrtState = PRE_PRT_STATE0;
							}
						}
						PrtDrvStruct.FormFeedVariable = PrtDrvStruct.FormFeedLength;
						break;
					}
				}
				else
				{
					PrtDrvStruct.FormFeedVariable--;
					break;
				}
			}
			if (PrtDrvStruct.PrtState == PRT_STATE_LC)
			{
				PrtDrvStruct.FindGapFeedCount++;
				if((SensorStruct.TestLabelStartFlag == 2 && SensorStruct.TestGapStartFlag == 2) || (PrtDrvStruct.FindGapFeedCount > MAX_CHECK_FEED_LEN))
				{
					PrtStruct.LabelHeight = SensorStruct.TestLabelLength;
					PrtDrvStruct.GapLength = SensorStruct.TestGapLength;
//sprintf(MsgBuf, "tll=%d,tgl=%d,fgfc=%d\r\n", SensorStruct.TestLabelLength, SensorStruct.TestGapLength, PrtDrvStruct.FindGapFeedCount);//CJK0416
//PutStrTxBuf(&CommBuf, MsgBuf);
					if (PrtDrvStruct.LabelGapMode)
					{
						PrtDrvStruct.PrtState = PRT_STATE5;
					}
					else
					{
						PrtDrvStruct.PrtState = PRT_STATE0E;
					}
					if((PrtStruct.Mode == LABEL) && (PrtDrvStruct.PrePrintMode == ON))
					{
						PrtDrvStruct.PrePrtState = PRE_PRT_STATE1FF;
					}
				}
				break;
			}

			if (PrtDrvStruct.PrtState == PRT_STATE_SC)
			{
				PrtDrvStruct.FindGapFeedCount++;
				//HYP 20040330
				if((PrtDrvStruct.LabelGapMode == NO_GAP) && (PrtDrvStruct.FindGapFeedCount > MAX_CHECK_FEED_LEN_NO_GAP))
				{
					SensorStruct.GapThreshold = SensorStruct.MinGapValue + (SensorStruct.MaxGapValue - SensorStruct.MinGapValue)/2;
					SensorStruct.PeelThreshold = SensorStruct.MinPeelValue + (SensorStruct.MaxPeelValue - SensorStruct.MinPeelValue)/2;
					PrtDrvStruct.PrtState = PRT_STATE0E;
					break;
				}
				if((SensorStruct.TestLabelStartFlag == 2 && SensorStruct.TestGapStartFlag == 2) || (PrtDrvStruct.FindGapFeedCount > MAX_CHECK_FEED_LEN))
				{
					SensorStruct.GapThreshold = SensorStruct.MinGapValue + (SensorStruct.MaxGapValue - SensorStruct.MinGapValue)/2;
					SensorStruct.PeelThreshold = SensorStruct.MinPeelValue + (SensorStruct.MaxPeelValue - SensorStruct.MinPeelValue)/2;
					if (PrtDrvStruct.LabelGapMode)
					{
						PrtDrvStruct.PrtState = PRT_STATE5;
					}
					else
					{
						PrtDrvStruct.PrtState = PRT_STATE0E;
					}
					if((PrtStruct.Mode == LABEL) && (PrtDrvStruct.PrePrintMode == ON))
					{
						PrtDrvStruct.PrePrtState = PRE_PRT_STATE1FF;
					}
				}
				break;
			}
STATE5://HYP 20060410 (change prt flow)
			if (PrtDrvStruct.PrtState == PRT_STATE5)
			{
				PrtDrvStruct.FindGapFeedCount++;
				if((PrtStruct.Mode == LABEL) && (PrtDrvStruct.PrePrintMode == ON) && (PrtDrvStruct.PrePrtState == PRE_PRT_STATE0))
				{
					PrtDrvStruct.PrtState = PRT_STATE0E;
					//Don't check gap
				}
				else//PrePrtState == state1 or state1ff
				{
					if(	PrtDrvStruct.LabelGapMode)
					{
						if(PrtDrvStruct.FindGapFeedCount < MAX_CHECK_FEED_LEN)
						{
							if(PrtDrvStruct.FindLabelFeedCount < PrtDrvStruct.TphMemMaxRowNum + PrtDrvStruct.GapLength*2)
							{
								if (SensorStruct.GapBuf[SensorStruct.GapBufOutPtr] > SensorStruct.GapThreshold)
								{
									PrtDrvStruct.GapTraceFlag = 1;
									PrtDrvStruct.FindLabelFeedCount++;
								}
								else if(PrtDrvStruct.GapTraceFlag == 1 )
								{
									PrtDrvStruct.OutOfPaperFlag = OFF;
									PrtDrvStruct.GapSenErrFlag = OFF;
									PrtDrvStruct.PrtState = PRT_STATE0E;
									//HYP 20060426 for smooth movement in pre-print mode
									if((PrtStruct.Mode == LABEL) && (PrtDrvStruct.PrePrintMode == ON))
									{
										if(PrtDrvStruct.PrePrtState == PRE_PRT_STATE1FF)
										{
											PrtDrvStruct.PrtState = PRT_STATE_FF;
											PrtDrvStruct.FormFeedVariable = PrtDrvStruct.PrePrintLength;
										}
										else if(PrtDrvStruct.PrePrtState == PRE_PRT_STATE1PR)
										{
											if (PrtDrvStruct.PrtDirection == NON_INVERT)
											{
												PrtDrvStruct.TphMemRowCount = 0;
											}
											else
											{
												PrtDrvStruct.TphMemRowCount = PrtDrvStruct.TphMemMaxRowNum - 1;
											}
											PrtDrvStruct.PrtState = PRT_STATE1;
											goto STATE1;
										}
									}
									//////////////////////////////////////////////////////
								}
							}
							else
							{
								PrtDrvStruct.PrtState = PRT_STATE0E;
								PrtDrvStruct.OutOfPaperFlag = ON;/////
							}
						}
						else
						{
							PrtDrvStruct.PrtState = PRT_STATE0E;
							PrtDrvStruct.GapSenErrFlag = ON;
						}
					}
					else
					{
						PrtDrvStruct.PrtState = PRT_STATE0E;
					}
				}
				break;
			}
			if (PrtDrvStruct.PrtState == PRT_STATEB)
			{
				PrtDrvStruct.PrtState = PRT_STATE2;
				if(PrtDrvStruct.PrePrtState != PRE_PRT_STATE1PR)
				{
					break;
				}
			}

STATE4_PRE://HYP 20060426 Rechange Flow for pre=print

		case PRT_STATE2:
			//LATCH > Min 100 ns
			PRT_LAT_PORT_LOW;
			prt_delay(20);
			PRT_LAT_PORT_HIGH;
			PRT_STB1_PORT_HIGH;
			PRT_STB2_PORT_LOW;

			timerVal = 390625 * (PrtDrvStruct.PCATimerReloadValue2 >> 1) / 1000000; // 100MHz PBclk / 256 = 390,625Hz
			PLIB_TMR_Period16BitSet(TMR_ID_6, (INT16U)timerVal);
			PLIB_TMR_Start(TMR_ID_6);

			if(PrtDrvStruct.PrtState == PRT_STATE4) {
				//HYP 20040330
				//PrtDrvStruct.PrtState = PRT_STATE5;
				if((PrtStruct.Mode == LABEL) && (PrtDrvStruct.LabelGapMode == NO_GAP)) {
					PrtDrvStruct.ReceiptFeedVariable = PrtDrvStruct.GapLength;
					PrtDrvStruct.PrtState = PRT_STATE_RFF;
				} else {
					PrtDrvStruct.PrtState = PRT_STATE5;
					goto STATE5;//HYP 20060410 Insert (change prt flow)
				}
//				goto STATE5;
				break;
			}

		case PRT_STATE1:
STATE1:
			//Check peel (state1)
			if (PrtDrvStruct.PrtState == PRT_STATE1)
			{
				if((PrtStruct.Mode == LABEL) && (PrtDrvStruct.PrePrintMode == ON) && (PrtDrvStruct.PrePrtState == PRE_PRT_STATE1PR))
				{
					//none
				}
				else
				{//Receipt or PrePrintMode == OFF or (PrePrintMode == ON && PrePrtState == state0)
					if((PrtStruct.Mode == LABEL) && (PrtDrvStruct.PrePrintMode == ON) && (PrtDrvStruct.PrePrtState == PRE_PRT_STATE0))
					{
						if (PrtDrvStruct.PrtDirection == NON_INVERT)
						{
							PrtDrvStruct.TphMemRowCount = PrtDrvStruct.PrePrintLength;
						}
						else
						{
							PrtDrvStruct.TphMemRowCount = PrtDrvStruct.TphMemMaxRowNum - PrtDrvStruct.PrePrintLength - 1;
						}
					}
//HYP 20040401
#ifdef HEADUP_PEEL_SEN_NOT_CONTROLLED
					PrtDrvStruct.HangingFormFlag = OFF;
#else

#ifdef PEEL_SENSOR_REFLECTIVE
					if ((PrtDrvStruct.PeelEnable == DISABLE) || (SensorStruct.PeelFilVal < SensorStruct.PeelThreshold))
					{
#else
					if ((PrtDrvStruct.PeelEnable == DISABLE) || (SensorStruct.PeelFilVal > SensorStruct.PeelThreshold))
					{
#endif//PEEL_SENSOR_REFLECTIVE
						PrtDrvStruct.HangingFormFlag = OFF;
					}
					else
					{
						PrtDrvStruct.HangingFormFlag = ON;
						PrtDrvStruct.LabelCopyCount = 1;
						PrtDrvStruct.PrtState = PRT_STATE0E;
						break;
					}
#endif //HEADUP_PEEL_SEN_NOT_CONTROLLED
				}
			}
			/* TPH 1 row data */
			if (PrtDrvStruct.PrtDirection == NON_INVERT)
			{
            #ifdef USE_SRAM_PRT_RENERING_BUF
                pSram = SramTphMemory + (PrtDrvStruct.TphMemRowCount*TPH_ROW_BYTE);
                tempSize = TPH_ROW_BYTE;
                pTempSramBuff = (char *)malloc(tempSize);
                SramReadData( pSram, pTempSramBuff, tempSize );
                SPISendStr( pTempSramBuff, TPH_ROW_BYTE );
                free( pTempSramBuff );
            #else // USE_SRAM_PRT_RENERING_BUF
                tph_memory = &PrtDrvStruct.TphMemory[PrtDrvStruct.TphMemRowCount*TPH_ROW_BYTE];
				for (column_byte_count = 0 ; column_byte_count < (TPH_ROW_BYTE/8) ; column_byte_count++)
				{
					tph_buf = *tph_memory++;
					SPISendByte(tph_buf); //1_
					tph_buf = *tph_memory++;
					SPISendByte(tph_buf); //2_
					tph_buf = *tph_memory++;
					SPISendByte(tph_buf); //3_
					tph_buf = *tph_memory++;
					SPISendByte(tph_buf); //4_
					tph_buf = *tph_memory++;
					SPISendByte(tph_buf); //5
					tph_buf = *tph_memory++;
					SPISendByte(tph_buf); //6
					tph_buf = *tph_memory++;
					SPISendByte(tph_buf); //7
					tph_buf = *tph_memory++;
					SPISendByte(tph_buf); //8
				}
            #endif // USE_SRAM_PRT_RENERING_BUF
			}
			else
			{
//dma mode
//			    //tph_memory = &PrtDrvStruct.TphMemory[PrtDrvStruct.TphMemRowCount*TPH_ROW_BYTE];
//                            tph_memory = &testPattern;
//                            if ((PrtDrvStruct.TphMemRowCount%16) >= 0 && (PrtDrvStruct.TphMemRowCount%16) < 8)
//                            {
//                                memcpy(tph_tx_memory, tph_memory, TPH_ROW_BYTE);
//                            }
//                            else
//                            {
//                                memcpy(tph_tx_memory, &tph_memory[1], TPH_ROW_BYTE);
//                            }
//                            dma_send();
//spi mode
            #ifdef USE_SRAM_PRT_RENERING_BUF
                pSram = SramTphMemory + (PrtDrvStruct.TphMemRowCount*TPH_ROW_BYTE);
                tempSize = TPH_ROW_BYTE;
                pTempSramBuff = (char *)malloc(tempSize);
                SramReadData( pSram, pTempSramBuff, tempSize );
                Inline_Tph_Data_Cpy_Rev(tph_tx_memory, pTempSramBuff, TPH_ROW_BYTE);
                SPISendStr( tph_tx_memory, TPH_ROW_BYTE );
                free( pTempSramBuff );
            #else // USE_SRAM_PRT_RENERING_BUF
                tph_memory = &PrtDrvStruct.TphMemory[PrtDrvStruct.TphMemRowCount*TPH_ROW_BYTE];
				Inline_Tph_Data_Cpy_Rev(tph_tx_memory, tph_memory, TPH_ROW_BYTE);
				tph_memory_reverse = tph_tx_memory;
				for (column_byte_count = 0 ; column_byte_count < (TPH_ROW_BYTE/8) ; column_byte_count++)
				{
					tph_buf = *tph_memory_reverse++;
					SPISendByte(tph_buf); //1
					tph_buf = *tph_memory_reverse++;
					SPISendByte(tph_buf); //2
					tph_buf = *tph_memory_reverse++;
					SPISendByte(tph_buf); //3
					tph_buf = *tph_memory_reverse++;
					SPISendByte(tph_buf); //4
					tph_buf = *tph_memory_reverse++;
					SPISendByte(tph_buf); //5
					tph_buf = *tph_memory_reverse++;
					SPISendByte(tph_buf); //6
					tph_buf = *tph_memory_reverse++;
					SPISendByte(tph_buf); //7
					tph_buf = *tph_memory_reverse++;
					SPISendByte(tph_buf); //8
				}
            #endif // USE_SRAM_PRT_RENERING_BUF
			}
			if (PrtDrvStruct.PrtDirection == NON_INVERT)
			{
				PrtDrvStruct.TphMemRowCount++;
			}
			else
			{
				PrtDrvStruct.TphMemRowCount--;
			}

			if (PrtDrvStruct.PrtState == PRT_STATE1)
			{
  				PrtDrvStruct.PrtState = PRT_STATEB;
				break;
			}
			if (PrtDrvStruct.PrtState == PRT_STATE2)
			{
				PrtDrvStruct.PrtState = PRT_STATE3;
				break;
			}
			//state 3
			if (PrtDrvStruct.PrtDirection == NON_INVERT)
			{
				if((PrtStruct.Mode == LABEL) && (PrtDrvStruct.PrePrintMode == ON) && (PrtDrvStruct.PrePrtState == PRE_PRT_STATE1PR))
				{
					if (PrtDrvStruct.TphMemRowCount >= PrtDrvStruct.PrePrintLength )
					{
						PrtDrvStruct.PrtState = PRT_STATE4;
						PrtDrvStruct.PrePrtState = PRE_PRT_STATE0;
						break;
					}
				}
				else//PrePrtState == state0
				{
					if (PrtDrvStruct.TphMemRowCount >= PrtDrvStruct.TphMemMaxRowNum )
					{
						if((PrtStruct.Mode == LABEL) && (PrtDrvStruct.PrePrintMode == ON) && (PrtDrvStruct.PrePrtState == PRE_PRT_STATE0))
						{
							PrtDrvStruct.PrePrtState = PRE_PRT_STATE1PR;
						}
						PrtDrvStruct.PrtState = PRT_STATE4;
						break;
					}
				}
			}
			else
			{
				if((PrtStruct.Mode == LABEL) && (PrtDrvStruct.PrePrintMode == ON) && (PrtDrvStruct.PrePrtState == PRE_PRT_STATE1PR))
				{
					if ((PrtDrvStruct.TphMemRowCount +1) <= (PrtDrvStruct.TphMemMaxRowNum - PrtDrvStruct.PrePrintLength - 1))
					{	  //0 line print
						PrtDrvStruct.PrtState = PRT_STATE4;
						PrtDrvStruct.PrePrtState = PRE_PRT_STATE0;
						break;
					}
				}
				else
				{//PrePrtState == state0
					if ((PrtDrvStruct.TphMemRowCount) <= 0)
					{	  //0 line print
						if((PrtStruct.Mode == LABEL) && (PrtDrvStruct.PrePrintMode == ON) && (PrtDrvStruct.PrePrtState == PRE_PRT_STATE0))
						{
							PrtDrvStruct.PrePrtState = PRE_PRT_STATE1PR;
						}
						PrtDrvStruct.PrtState = PRT_STATE4;
						break;
					}
				}
			}
			break;
		default:
			PrtDrvStruct.PrtState = PRT_STATE0I;
			PrtDrvStruct.PrePrtState = PRE_PRT_STATE0;
#ifdef TIMER0_ONOFF_CONTROL
			PrtDrvStruct.Timer0OnOff = OFF;
#endif
			break;
	}//switch(PrtDrvStruct.PrtState)

			PrtDrvStruct.SysTimer++;
			SysTimer1_25ms++;
			KEY_Scan();
			WDT_Timer_Clear();
			if(!(PrtDrvStruct.SysTimer % PrtDrvStruct.SysTimerNum10ms)) {
				PrtDrvStruct.Timer10ms++;
				SysTimer10ms++;
				if(!(PrtDrvStruct.Timer10ms%6000)) { // 1 min
#ifdef USE_MORNITORING_CNT
					SysOperatingTime = 1;
#endif
					SysTimerMin++;
				}
				if(!(PrtDrvStruct.Timer10ms%100)) { // 1 sec
					SysTimerForDHCP++;//DHCP LEASE TIME CHECK NO
				}
				if(!(PrtDrvStruct.Timer10ms%10)) { //100 ms
					SysTimer100ms++;
				}
				if(!(PrtDrvStruct.Timer10ms%5)){ // 50 ms
					if(Buzzer.Times && Buzzer.Status == 0) {
						BUZZER_HIGH;
						Buzzer.Times--;
						Buzzer.Status = 1;
					}
					else if(Buzzer.Status) {
						BUZZER_LOW;
						Buzzer.Status = 0;
					}
				}
			}
	if(PrtDrvStruct.PrtState != PRT_STATE0I)
	{
		AdInt();
	}
}

void PCATimerInt(void)
{
	INT32U timerVal;
#ifdef TPH_STB_ON_AT_THE_SAME_TIME
	   	PCATIMER_STB_STOP;
		/* STB is off */
		TPHSTB1 = 0;
		TPHSTB2 = 0;
		PrtDrvStruct.StbState = STB_STATE0;
#else
	if (PrtDrvStruct.StbState == STB_STATE0) {
		PLIB_TMR_Stop(TMR_ID_6);
		timerVal = 390625 * (PrtDrvStruct.PCATimerReloadValue >> 1) / 1000000; // 100MHz PBclk / 256 = 390,625Hz
		PLIB_TMR_Period16BitSet(TMR_ID_6, (INT16U)timerVal);

		PLIB_TMR_Start(TMR_ID_6);
		PRT_STB1_PORT_LOW;
		PRT_STB2_PORT_HIGH;
		PrtDrvStruct.StbState = STB_STATE1;
	} else if (PrtDrvStruct.StbState == STB_STATE1) {
		PLIB_TMR_Stop(TMR_ID_6);
		timerVal = 390625 * (PrtDrvStruct.PCATimerReloadValue2 >> 1) / 1000000; // 100MHz PBclk / 256 = 390,625Hz
		PLIB_TMR_Period16BitSet(TMR_ID_6, (INT16U)timerVal);
		PLIB_TMR_Start(TMR_ID_6);
		PRT_STB1_PORT_HIGH;
		PRT_STB2_PORT_LOW;
		PrtDrvStruct.StbState = STB_STATE2;
	} else if (PrtDrvStruct.StbState == STB_STATE2) {
		PLIB_TMR_Stop(TMR_ID_6);
		timerVal = 390625 * (PrtDrvStruct.PCATimerReloadValue >> 1) / 1000000; // 100MHz PBclk / 256 = 390,625Hz
		PLIB_TMR_Period16BitSet(TMR_ID_6, (INT16U)timerVal);
		PLIB_TMR_Start(TMR_ID_6);
		PRT_STB1_PORT_LOW;
		PRT_STB2_PORT_HIGH;
		PrtDrvStruct.StbState = STB_STATE3;
	} else {
		//PCATIMER_STB_STOP;
		PLIB_TMR_Stop(TMR_ID_6);
		//TIM_CounterCmd(TIM1, TIM_STOP);
		/* STB is off */
		PRT_STB1_PORT_LOW;
		PRT_STB2_PORT_LOW;
		PrtDrvStruct.StbState = STB_STATE0;
	}
#endif //#ifdef TPH_STB_ON_AT_THE_SAME_TIME
}

void Timer0Start(void)
{
	PLIB_TMR_Start(TMR_ID_8);
}
