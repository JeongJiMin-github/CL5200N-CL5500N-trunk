#include <string.h>
#ifndef BYTE_H
#define BYTE_H
typedef unsigned char byte;
#endif

#define MUL_DEV(a, b, c) (a*b+c/2)/c	//사용시 c에 0이 들어가지 않도록 주의

extern INT8U checkblank(char *str,INT16S sz);
extern INT8U ctoh(INT8U c);
extern INT8U ctod(INT8U c);
extern INT8U tohex(INT8U cc);
extern INT8U ishex(INT8U cc);
#ifdef USE_EUCKR_UTF8_CONVERT
extern INT8U isURL_unreserved(INT8U cc);
#endif
extern long _labs(long a,long b);
extern INT16S   ilog(INT32U m);
// extern INT16S  _atoi(char *s);
// extern long _atol(char *s);
extern INT8U hextoc(INT8U cc);
//DEL20060310extern INT8U hextobyte(INT8U c1,INT8U c2);
extern INT8U hexstrtobyte(char *str);
extern INT8U hex2bcd(INT8U hex );
extern INT8U bcd2hex(INT8U bcd );
extern void tohex2(INT8U cc,char *str);
extern void tohex4(INT16U cc,char *str);
extern void str_trim(char *str);
extern INT8U format_long(INT8U *dest,long v,INT8U count);
//extern void  format_fixed(INT8U *dest,long v,INT8U digit,INT8U leng);
extern void format_fixed(HUGEDATA INT8U *dest,long v,INT8U digit,INT8U leng, char decimal_ch);
extern void format_fixed_hex(HUGEDATA INT8U *dest,INT32U v,INT8U leng);
extern INT8U format_value(HUGEDATA INT8U *dest,long data,INT8U digit, char decimal_ch);
extern INT8U format_value_2long(HUGEDATA INT8U *dest, long data_h8, long data_l8, INT8U digit, char decimal_ch);
extern INT8U _isdigit(INT8U ch);
extern void  strset(char *str,INT8U ch,INT16S sz);
extern INT16U hex2bcd_word(INT16U cc);
extern void  binary2str(INT8U x,INT8U *str);
extern INT8U str2binary(INT8U *str);
extern void _strfill(INT8U *str,INT8U fl,INT16S n);
extern INT32U power10(INT16S n);
extern INT8U checksum_price_weight1(long v);
extern INT8U checksum_price_weight2(long v);
extern void _strupr(char *str);
extern INT8U str2byte_n(char * str, INT8U size);
extern INT32U str2long_n(INT8U *str, INT8U size);
extern char* str_trimleft(char* str);
extern INT8U gen_bcc(INT8U *buffer,INT16S leng);
extern void  str_fill(char *str,INT16S l);
extern INT16S IsBlankString(char *p,INT16S max);
extern void hex2hexstr(INT8U *src, INT8U *dst, INT16U len);
extern void hexstr2hex(INT8U *src, INT8U *dst, INT16U hex_len);
extern void safe_strncpy(char *dst, const char *src, size_t size);
#ifdef USE_USA_FIXEDWEIGHT_UNIT_OZ
extern INT32S LbtoOz(INT32S lb, INT16U rate, INT8U decimal);
extern INT32S OztoLb(INT32S oz, INT16U rate, INT8U decimal);
#endif

