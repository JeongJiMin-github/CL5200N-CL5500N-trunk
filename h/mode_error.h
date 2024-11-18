#ifndef _DSP_VFD7_H
#include "dsp_vfd7.h"
#endif

#ifndef _FONT_H
#include "prt_font.h"
#endif

#ifndef _DSP_COMMON_H
#include "dsp_common.h"
#endif

extern void message_error(POINT disp_p);
extern void mode_error(void);
extern void mode_error_disp(INT16U errcode,INT8U act);
//DEL071206extern void warning_message(INT16U err,char *msg);
