#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "globals.h"
#include "common.h"
#include "initial.h"


INT8U ctoh(INT8U c)
{
	if ((c>='a') && (c<='f')) c=c-'a'+10;
	else if ((c>='A') && (c<='F')) c=c-'A'+10;
	else c=c-'0';
	return c;
}

INT8U ctod(INT8U c)
{
	if ((c>='0') && (c<='9')) c=c-'0';
	else c=0;
	return c;
}

//long  _atol(char *s)
//{
//	long v,mask;
//	INT16S l,i;
//	l=strlen(s);
//	mask=1;
//	v=0;
//	for (i=l-1; i>=0; i--) {
//		if (!isdigit(s[i])) break;
//		v+=(INT16S)(s[i]-'0')*mask;
//		mask*=10;
//	}
//	return v;
//}

//INT16S  _atoi(char *s)
//{
//	INT16S v,mask;
//	INT16S l,i;
//	l=strlen(s);
//	mask=1;
//	v=0;
//	for (i=l-1; i>=0; i--) {
//		if (!isdigit(s[i])) break;
//		v+=(long)(s[i]-'0')*mask;
//		mask*=10;
//	}
//	return v;
//}

INT8U tohex(INT8U cc)
{
	byte c1;
	if (cc>=10) c1=cc+'A'-10;
	else c1=cc+'0';
	return c1;
}

INT8U checkblank(char *str,INT16S size)
{
	INT16S i;
	INT8U clr;

	clr=0;
	if (size==0) {
		size=strlen(str);
	}
	for (i=0; i<size; i++) {
		if ((INT8U)str[i]>0x20) { clr=1; break; }
	}
	return clr;
}

void tohex2(INT8U cc,char *str)
{
	str[0]=tohex(cc>>4  );
	str[1]=tohex(cc&0x0f);
}

void tohex4(INT16U cc,char *str)
{
	str[0]=tohex((INT8U)((cc>>12)&0x0f));
	str[1]=tohex((INT8U)((cc>> 8)&0x0f));
	str[2]=tohex((INT8U)((cc>> 4)&0x0f));
	str[3]=tohex((INT8U)(cc&0x0f));
}

INT8U ishex(INT8U cc)
{
	if ((cc>='0') && (cc<='9')) return 1;
	if ((cc>='A') && (cc<='F')) return 1;
	if ((cc>='a') && (cc<='f')) return 1;    
	return 0;
}

#ifdef USE_EUCKR_UTF8_CONVERT
INT8U isURL_unreserved(INT8U cc)
{
	if ((cc>='0') && (cc<='9')) return 1;
	if ((cc>='A') && (cc<='Z')) return 1;
	if ((cc>='a') && (cc<='z')) return 1;    
	if (cc=='-')return 1;
	if (cc=='_')return 1;
	if (cc=='.')return 1;	
	if (cc=='~')return 1;		
	return 0;
}

#endif

INT16S IsBlankString(char *p,INT16S max)
{
	INT16S i;

	for(i=0;i<max;i++)
	{
		if (p[i] == 0)		return ON;	// blank
		if ((unsigned char)p[i]>0x20)	break; 	// no blank
	}
	return OFF;
}
							
INT8U hexstrtobyte(char *str)
{
	INT8U header = 0;
	INT8U i;

	for(i = 0; i < 2; i++)
	{
		header <<= 4;

		if(str[i] >= 'A' && str[i] <= 'F')
		{
			header |= (str[i] - 'A'	+ 10);
		}
		else if(str[i]	>= '0' && str[i] <= '9')
		{
			header |= (str[i] - '0');
		}
	}
	return header;
}

long _labs(long a,long b)
{
	if (a>b) return (a-b);
	return (b-a);
}

INT32U power10(INT16S n)
{
	switch (n) {
	    case 0: return 1L;
	    case 1: return 10L;
	    case 2: return 100L;
	    case 3: return 1000L;
	    case 4: return 10000L;
	    case 5: return 100000L;
	    case 6: return 1000000L;
	    case 7: return 10000000L;
	    case 8: return 100000000L;
	    case 9: return 1000000000L;
	    default : return 1000000L;
	}
}

INT8U hex2bcd(INT8U cc)		// 0x0d -> 0x13
{
	byte c1,c2;
	c1=cc/10;
	c2=cc%10;
	c1<<=4;
	c1|=c2;
	return c1;
}

INT8U bcd2hex(INT8U cc)		// 0x13 -> 0x0d
{
	INT8U c1,c2;
	c1=cc&0x0f;
	c2=cc>>4;
	c2=c2*10+c1;
	return c2;
}

INT16U hex2bcd_word(INT16U cc)
{
	INT16U c1;
	c1 = cc/100;
	c1 = hex2bcd((INT8U)c1);
	c1 *= 0x100;
	c1 += hex2bcd((INT8U)(cc%100));
	return c1;
}

INT8U format_long(INT8U *dest,long v,INT8U count)
{
	byte cnt,i,c1;
	cnt=count-1;
	for (i=0; i<count; i++) {
		c1=(byte)(v%10);
		dest[cnt--]=c1+'0';
		v=v/10;
	}
	return count;
}

void  str_fill(char *str,INT16S l)
{
	INT16S i,sl,q;
	sl=strlen(str);
	if (sl==0) q=0;
	else q=sl;
	for (i=q; i<l; i++) {
		str[i]=0x20;
	}
	str[l]=0;
}

INT16S ilog(INT32U m)
{
	if (m<10) return 1;
	if (m<100) return 2;
	if (m<1000) return 3;
	if (m<10000) return 4;
	if (m<100000l) return 5;
	if (m<1000000l) return 6;
	if (m<10000000l) return 7;
	if (m<100000000l) return 8;
	if (m<1000000000l) return 9;
	return 0;
}

INT8U format_value(HUGEDATA INT8U *dest,long data,INT8U digit, char decimal_ch)
{
	long m;
	INT8U i,x,q;//,neg;
	INT16S k;
	// 1000단위 구분자 사용
	INT8U dec_len;
	INT8U decPtFlag;
	INT8U segmentChar;
	INT8U useSeperator;

	m=1000000000l;
	q=1;
	k=0;

	decPtFlag = 0;	     
	dec_len = ilog(data);
	useSeperator = get_global_bparam(GLOBAL_PRICESIGN_1000);
	useSeperator &= 0x01;
	if(useSeperator)	// 1000단위 구분자 사용
	{
 #if defined(COUNTRY_INDONESIA) || defined(USE_INDONESIA_EMPORIUM)
		segmentChar = '.';
 #else
		segmentChar = get_global_bparam(GLOBAL_SALE_SETUP_FLAG20) & 0x01;	//parameter 764

	 	if(segmentChar == 1)
			segmentChar = '.';
		else
			segmentChar = ',';
 #endif
	}
	
	if(data < 0) {
		data = -data;
//		neg = 1;
		dest[k++]='-';
	}

	for (i=0; i<10; i++) {
		x=(char)(data/m);
		if (q) {
			if (x) { dest[k++]=x+'0'; q=0; }
		} else {
			dest[k++]=x+'0';
		}
		if ((digit==(9-i)) && (digit)) {
			if (q) dest[k++]='0';
			q=0;
		if(useSeperator)	// 1000단위 구분자 사용
			decPtFlag = 1;

			dest[k++]=decimal_ch;//'.';
		}

		if (useSeperator && decPtFlag == 0 && dec_len > (9-i) && ( (9-i) == (3+digit) || (9-i) == (3*2+digit)) )	// 1000단위 구분자 사용
		{
			dest[k++] = segmentChar;
		}

		data%=m;
		m=m/10l;
	}
	if (q) dest[k++]='0';
	dest[k]=0;
	return k;
}


INT8U format_value_2long(HUGEDATA INT8U *dest, long data_h8, long data_l8, INT8U decimal, char decimal_ch)
{
	char temp_buf[20];
	INT8U len;

	len=0;
	if(data_h8) {
		format_value((HUGEDATA INT8U *)dest, data_h8, 0, decimal_ch);
		len = strlen((char *)dest);
		format_value((HUGEDATA INT8U *)temp_buf, data_l8, decimal, decimal_ch);
		if(decimal) {
			memset(dest+len, '0', 9 - strlen(temp_buf));
			len += (9 - strlen(temp_buf));
			sprintf((char *)&dest[len], "%s", temp_buf);
		} else 
			sprintf((char *)&dest[len],"%08ld", data_l8);
	} else {
		format_value((HUGEDATA INT8U *)dest, data_l8, decimal, decimal_ch);
	}
	return len;
}

//void format_fixed(INT8U *dest,long v,INT8U digit,INT8U leng)
void format_fixed(HUGEDATA INT8U *dest,long v,INT8U digit,INT8U leng, char decimal_ch)
{
	INT8U cnt, i, neg;
	byte c1,r;
	INT8U j,temp;

	neg = 0;
	if(v < 0) {
		v = -v;
		neg = 1;
	}

	cnt=(char)leng-1;
	r=0;
	for (i=0; i<leng; i++) {
	    c1 =(byte)(v%10);
	    dest[cnt--]=c1+'0';
	    v=v/10;
	    if (r) r=2;
	    if ((digit-1)==i) {
			dest[cnt--]=decimal_ch;//'.';
			r=1;
	    }
	    if (v==0) {
			if (digit) {
				if (r==2) break;
			} else break;
	    }
	}
	if ((r==1) && (cnt)) dest[cnt--]='0';
	if (neg == 1) 
	{
		if(cnt>0) dest[cnt--] = '-';
		//else dest[0] = '-';
		else {
			for(i=0; i<leng; i++) dest[i] = '-';
			cnt = 0xff;  
		}
	}
	if(cnt == 0xff) return;
	temp = (INT16S)cnt;
	for (j=0; j<=temp; j++) dest[j]=0x20; 
	
//@	char cnt,i;
//@	byte c1,r;
//@	cnt=(char)leng-1;
//@	r=0;
//@	for (i=0; i<leng; i++) {
//@	    c1 =(byte)(v%10);
//@	    dest[(INT16S)(cnt--)]=c1+'0';
//@	    v=v/10;
//@	    if (r) r=2;
//@	    if ((digit-1)==i) {
//@			dest[(INT16S)(cnt--)]=decimal_ch;//'.';
//@			r=1;
//@	    }
//@	    if (v==0) {
//@			if (digit) {
//@				if (r==2) break;
//@			} else break;
//@	    }
//@	}
//@	if ((r==1) && (cnt)) dest[(INT16S)(cnt--)]='0';
//@	for (i=0; i<=cnt; i++) dest[(INT16S)i]=0x20;
}

/*
void format_fixed(HUGEDATA INT8U *dest,long v,INT8U digit,INT8U leng, char decimal_ch)
{
	char cnt,i, neg;
	byte c1,r;

	neg = 0;
	if(v < 0) {
		v = -v;
		neg = 1;
	}

	cnt=(char)leng-1;
	r=0;
	for (i=0; i<leng; i++) {
	    c1 =(byte)(v%10);
	    dest[(INT16S)(cnt--)]=c1+'0';
	    v=v/10;
	    if (r) r=2;
	    if ((digit-1)==i) {
			dest[(INT16S)(cnt--)]=decimal_ch;//'.';
			r=1;
	    }
	    if (v==0) {
			if (digit) {
				if (r==2) break;
			} else break;
	    }
	}
	if ((r==1) && (cnt)) dest[(INT16S)(cnt--)]='0';
	if (neg == 1) {
		if(cnt>0) dest[(INT16S)(cnt--)] = '-';
		//else dest[0] = '-';
		else {
			for(i=0; i<leng; i++) dest[i] = '-';
			cnt = -1;
		}
	}
	for (i=0; i<leng; i++) dest[i]=0x20;
	
//@	char cnt,i;
//@	byte c1,r;
//@	cnt=(char)leng-1;
//@	r=0;
//@	for (i=0; i<leng; i++) {
//@	    c1 =(byte)(v%10);
//@	    dest[(INT16S)(cnt--)]=c1+'0';
//@	    v=v/10;
//@	    if (r) r=2;
//@	    if ((digit-1)==i) {
//@			dest[(INT16S)(cnt--)]=decimal_ch;//'.';
//@			r=1;
//@	    }
//@	    if (v==0) {
//@			if (digit) {
//@				if (r==2) break;
//@			} else break;
//@	    }
//@	}
//@	if ((r==1) && (cnt)) dest[(INT16S)(cnt--)]='0';
//@	for (i=0; i<=cnt; i++) dest[(INT16S)i]=0x20;
}
*/

void format_fixed_hex(HUGEDATA INT8U *dest,INT32U v,INT8U leng)
{
	char cnt,i;
	byte c1;

	cnt=(char)leng-1;
	for (i=0; i<leng; i++) {
	    c1 =(byte)(v%16);
		if(c1 > 9) dest[(INT16S)(cnt--)]=c1-10+'A';
		else       dest[(INT16S)(cnt--)]=c1+'0';
	    v=v/16;
	    if (v==0) {
			break;
	    }
	}
	for (i=0; i<=cnt; i++) dest[(INT16S)i]=0x20;
}


void _strfill(INT8U *str,INT8U fl,INT16S n)
{
	INT16S i;
	for (i=0; i<n; i++) {
		if (str[i]<0x20) {
			str[i]=fl;
		}
	}
}

INT8U _isdigit(INT8U ch)
{
	if ((ch>=48) && (ch<=57)) return -1;
	return 0;
}

void strset(char *str,INT8U ch,INT16S sz)
{
	INT16S i;
	for (i=0; i<sz; i++) str[i]=ch;
	str[sz]=0;
}

void binary2str(INT8U x,INT8U *str)
{
	INT8U mask;
	INT16S   i;

	mask=0x80;
	for (i=0; i<8; i++) {
		str[i]='0';
		if (mask&x) str[i]='1';
		mask>>=1;
	}
	str[i]=0;
}

INT8U str2binary(INT8U *str)
{
	INT8U mask,ret;
	INT16S   i;

	mask=0x80;
	ret =0;
	for (i=0; i<8; i++) {
		if (str[i]=='1') {
			ret|=mask;
		}
		mask>>=1;
	}
	return ret;
}

INT8U checksum_weight_5_pls[10]={0,5,1,6,2,7,3,8,4,9};
INT8U checksum_weight_5_min[10]={0,5,9,4,8,3,7,2,6,1};
INT8U checksum_weight_2_min[10]={0,2,4,6,8,9,1,3,5,7};
INT8U checksum_weight_3_zro[10]={0,3,6,9,2,5,8,1,4,7};
INT8U checksum_revers_5_min[10]={0,9,7,5,3,1,8,6,4,2};	// modified 2009.04.17, V1.70.1
//INT8U checksum_revers_5_min[10]={0,9,7,5,3,2,8,6,4,2};

INT8U checksum_price_weight1(long v)
{
	INT8U p1,p2,p3,p4;
	INT16S temp;

	if (v>=10000) return 0;

	p1 = checksum_weight_2_min[(v/1000)  ]; temp = v%1000;
	p2 = checksum_weight_2_min[(temp/100)]; temp = v%100;
	p3 = checksum_weight_3_zro[(temp/10) ]; temp = v%10;
	p4 = checksum_weight_5_min[temp      ];
	
	temp = ( (p1+p2+p3+p4)*3)%10 ;
	return (INT8U)temp;
}

INT8U checksum_price_weight2(long v)
{
	INT8U p0, p1, p2, p3, p4;
	INT16S   temp;

	if (v>=100000l) return 0;

	p0 = checksum_weight_5_pls[(INT16S)(v/10000)  ]; temp = v%10000;
	p1 = checksum_weight_2_min[(INT16S)(temp/1000)]; temp = v%1000;
	p2 = checksum_weight_5_min[(INT16S)(temp/100) ]; temp = v%100;
	p3 = checksum_weight_5_pls[(INT16S)(temp/10)  ]; temp = v%10;
	p4 = checksum_weight_2_min[temp];

	temp = checksum_revers_5_min[(10-((p0+p1+p2+p3+p4)%10))%10];
	return (INT8U)temp;
}

void _strupr(char *str)
{
#ifndef USE_GEORGIA_DEFAULT
	INT16S i;
	for (i=0; i<strlen(str); i++) {
		if ((str[i]>='a') && (str[i]<='z')) str[i]=str[i]-32;
	}
#endif
}

INT8U str2byte_n(char * str, INT8U size)
{
	INT8U i;
	INT8U val;
	val = 0;
	for(i = 0; i < size; i++) {
		if(str[i] == 0) break;
		else if(str[i] == ' ') continue;
		val *= 10;
		val += (str[i] - '0');
	}
	return val;
}

INT32U str2long_n(INT8U *str, INT8U size)
{
	INT8U i;
	INT32U val;

	val = 0;
	for(i = 0; i < size; i++)
	{
		if (!isdigit(str[i])) 
		{
			val = 0;
			break;
		}
		val *= 10;
		val += (str[i] - '0');
	}
	return val;
}
		
char* str_trimleft(char* str)
{
	char *p;

	p=str;
	for (;*p==' ';p++) {
		if (*p==0) break;
	}
	return p;
}

void str_trim(char *str)
{
	INT16S i,l;
	l=strlen(str);
	for (i=l-1; i>=0; i--) if ((INT8U)str[i]<=0x20) str[i]=0; else break;
}

INT8U gen_bcc(INT8U *buffer,INT16S leng)
{
	INT8U bcc;
	INT16S   i;

	bcc = 0;
	for (i=0; i<leng; i++) bcc^=buffer[i];
	return bcc;
}


void hex2hexstr(INT8U *src, INT8U *dst, INT16U len)
{
	INT16U i;
	
	for (i  = 0; i < len; i++)
	{
		tohex2(src[i], (char*)&dst[i*2]);
	}
}


void hexstr2hex(INT8U *src, INT8U *dst, INT16U hex_len)
{
	INT16U i;
	
	for (i  = 0; i < hex_len; i++)
	{
		dst[i] = hexstrtobyte((char*)&src[i*2]);
	}
}

void safe_strncpy(char *dst, const char *src, size_t size)
{   
	dst[size-1] = '\0';
	if((size > 0) && (src == NULL)) 
		dst[0] = 0;
	else
		strncpy(dst,src,size-1);
	return;
}
int safe_strncat(char *dst, const char *src, size_t size)
{
	char *d = dst;
	const char *s = src;
	int n = size;
	int dlen;
	
	while (n-- != 0 && *d != '\0')
		d++;
	dlen = d - dst;
	n = size - dlen;
 
	if (n == 0)
		return (dlen + strlen(s));
	while (*s != '\0')
	{
		if (n != 1)
		{
			*d++ = *s;
			n--;
		}
		s++;
	}
	*d = '\0';
	return (dlen + (s - src));
}

#ifdef USE_USA_FIXEDWEIGHT_UNIT_OZ
INT32S LbtoOz(INT32S lb, INT16U rate, INT8U decimal)
{
	INT32S temp_lb;	
	INT32S temp_oz;	
	
	if(lb<0){
		lb=0;
	}
	temp_lb = lb / power10(decimal);
	temp_oz = lb % power10(decimal);
	temp_oz = temp_oz * rate;
	temp_oz = temp_oz * 10 + 5;
	temp_oz = temp_oz / power10(decimal+1);
	temp_oz = temp_lb * rate + temp_oz;
	return temp_oz;
}
INT32S OztoLb(INT32S oz, INT16U rate, INT8U decimal)
{
	INT32S temp_lb;	

	if(oz<0){
		oz=0;
	}
	temp_lb = oz * power10(decimal);
	temp_lb /= rate;
	return temp_lb;
}
#endif
