/*****************************************************************************
|*			
|*  Copyright		:	(c) 2002 CAS
|*  Filename		:	timer.h
|*  Version			:	0.1
|*  Programmer(s)	:	Hong Yi-Phyo (HYP)
|*  Created			:	2002/11/11
|*  Description		:	CLP Printer timer routine header    
|*				
****************************************************************************/


#ifndef _TIMER_H
#define _TIMER_H

#define PRT_STATE0I	0	  //idle
#define PRT_STATE0S	1	  //ready & start
#define PRT_STATEB	50	  //remove back lash1
#define PRT_STATE1	2	  //data
#define PRT_STATE2	3	  //latch,stb,data
#define PRT_STATE3	4	  //step,latch,stb,data
#define PRT_STATE4	5	  //step,latch,stb
#define PRT_STATE5	6	  //step
#define PRT_STATE0E	7	  //stop & end
#define PRT_STATE0E_HOLD_MOTOR	8
											
#define PRT_STATE_FFS	10	  //form feed
#define PRT_STATE_FF	11	  //form feed
#define PRT_STATE_LFS	20	  //line feed
#define PRT_STATE_LF	21	  //line feed
#define PRT_STATE_RFS	30	  //reverse feed
#define PRT_STATE_RF	31	  //reverse feed
#define PRT_STATE_LCS	40	  //label and gap check
#define PRT_STATE_LC	41	  //label and gap check
#define PRT_STATE_SCS	42	  //gap peel sensor check
#define PRT_STATE_SC	43	  //gap peel sensor check

#define PRT_STATE_TM	60	//user timer

#define PRT_STATE_RW	70	//rewind motor

#define PRT_STATE_CSS	80	//sensor check (peel-off and head-up)
#define PRT_STATE_CS	81

//HYP 20040330
#define PRT_STATE_RFFS	90
#define PRT_STATE_RFF	91	//Receipt Feed

#define STB_STATE0	0	  
#define STB_STATE1	1
#define STB_STATE2	2	  
#define STB_STATE3	3

#define PRE_PRT_STATE0		0
#define PRE_PRT_STATE1PR	1
#define PRE_PRT_STATE1FF	2

#define TPH_MAX_BUFFER	MAX_FORM_LEN * TPH_ROW_BYTE

	#ifndef USE_SRAM_PRT_RENERING_BUF
#ifdef USE_ONLY_1EA_PRT_RENDERING_BUF
extern HUGEDATA INT8U TphBufC[MAX_FORM_LEN][TPH_ROW_BYTE];
#else
extern HUGEDATA INT8U TphBufC[2][MAX_FORM_LEN][TPH_ROW_BYTE];
#endif
	#endif

extern void TimerInit(void);
extern void Timer0Enable(void);
extern void Timer0Start(void);
extern void Timer0Int(void);
extern void PCATimerEnable(void);
extern void PCATimerInt(void);

#endif /* _TIMER_H */
