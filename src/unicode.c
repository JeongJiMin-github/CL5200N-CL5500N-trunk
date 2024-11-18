#include <stdio.h>
#include "globals.h"
#include "bar_common.h"
#ifdef USE_KOR_FONT
#include "char_euckr_to_unicode.h"
#endif
#ifdef USE_GB2312_CHN_FONT
#include "char_gb2312_to_unicode.h"
#endif



/**
 * Encode a code point using UTF-8
 * 
 * @param out - output buffer (min 5 characters), will be 0-terminated
 * @param utf - code point 0-0x10FFFF
 * @return number of bytes on success, 0 on failure (also produces U+FFFD, which uses 3 bytes)
 */
INT16U utf8_encode(INT8U *out, INT16U utf)
{
  if (utf <= 0x7F) {
    // Plain ASCII
    out[0] = (INT8U) utf;
    out[1] = 0;
    return 1;
  }
  else if (utf <= 0x07FF) {
    // 2-byte unicode
    out[0] = (INT8U) (((utf >> 6) & 0x1F) | 0xC0);
    out[1] = (INT8U) (((utf >> 0) & 0x3F) | 0x80);
    out[2] = 0;
    return 2;
  }
  else if (utf <= 0xFFFF) {
    // 3-byte unicode
    out[0] = (INT8U) (((utf >> 12) & 0x0F) | 0xE0);
    out[1] = (INT8U) (((utf >>  6) & 0x3F) | 0x80);
    out[2] = (INT8U) (((utf >>  0) & 0x3F) | 0x80);
    out[3] = 0;
    return 3;
  }
//  else if (utf <= 0x10FFFF) {
//    // 4-byte unicode
//    out[0] = (INT8U) (((utf >> 18) & 0x07) | 0xF0);
//    out[1] = (INT8U) (((utf >> 12) & 0x3F) | 0x80);
//    out[2] = (INT8U) (((utf >>  6) & 0x3F) | 0x80);
//    out[3] = (INT8U) (((utf >>  0) & 0x3F) | 0x80);
//    out[4] = 0;
//    return 4;
//  }
  else { 
    // error - use replacement character
    out[0] = (INT8U) 0xEF;  
    out[1] = (INT8U) 0xBF;
    out[2] = (INT8U) 0xBD;
    out[3] = 0;
    return 0;
  }
}

#ifdef USE_KOR_FONT
INT16U convert_euckr_to_unicode(INT8U *src, INT16U *dst)
{
	INT16U i;
	INT16U wd;
	INT8U c;
	INT16U len;
	
	len = 0;
	
	while(1)
	{
		c = *src++;
		if (c == 0) break;
		if (c < 0x80)
		{
			*dst++ = (INT16U)c;
			len++;
			continue;
		}
		if (c >= 0xb0 && c <= 0xc8)
		{
			wd = (INT16U)c << 8;
			wd += (*src++); 
			for (i = 0; i < sizeof(euckr_to_unicode)/sizeof(INT16U)/2; i++)
			{
				if (wd == euckr_to_unicode[i * 2])
				{
					*dst++ = euckr_to_unicode[i * 2 + 1];
					len++;
					continue;
				}
			}
		}
	}
	return len;
}

INT16U convert_euckr_to_utf8(INT8U *src)
{
	INT16U unicode[MAX_BAR_DATA_BUF_SIZE_QR];
	INT16U pos, len;
	INT16U i;
	
	len = convert_euckr_to_unicode(src, unicode);
	pos = 0;
	for (i = 0; i < len; i++)
	{
		pos += utf8_encode(src+pos, unicode[i]);
	}
	*(src+pos) = 0;
	return pos;
}
#endif


#ifdef USE_GB2312_CHN_FONT
INT16U convert_gb2312_to_unicode(INT8U *src, INT16U *dst)
{
	INT16U i;
	INT16U wd;
	INT8U c;
	INT16U len;
	
	len = 0;
	
	while(1)
	{
		c = *src++;
		if (c == 0) break;
		if (c < 0x80)
		{
			*dst++ = (INT16U)c;
			len++;
			continue;
		}
		if (c >= 0xb0 && c <= 0xd7)
		{
			wd = (INT16U)c << 8;
			wd += (*src++); 
			for (i = 0; i < sizeof(gb2312_to_unicode)/sizeof(INT16U)/2; i++)
			{
				if (wd == gb2312_to_unicode[i * 2])
				{
					*dst++ = gb2312_to_unicode[i * 2 + 1];
					len++;
					continue;
				}
			}
		}
	}
	return len;
}

INT16U convert_gb2312_to_utf8(INT8U *src)
{
	INT16U unicode[MAX_BAR_DATA_BUF_SIZE_QR];
	INT16U pos, len;
	INT16U i;
	
	len = convert_gb2312_to_unicode(src, unicode);
	pos = 0;
	for (i = 0; i < len; i++)
	{
		pos += utf8_encode(src+pos, unicode[i]);
	}
	*(src+pos) = 0;
	return pos;
}
#endif