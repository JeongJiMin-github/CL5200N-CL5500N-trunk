#include <stdio.h>
#include "globals.h"
#include "common_sunday.h"

char date_checkday(short year,short month,short day)
{	// ======================================================
	// Check Date Value : return (1) is OK else return (0)
	// ======================================================
	short mday;
	if (month>12) return 0;
	mday=date_getmonthdays(year,month);
	if (day<=0) return 0;
	if (day>mday) return 0;
	return 1;
}

char date_getweek(short year,short month,short day )
{	// year=2001,month=1..12 day=1..
    short c,y,w;
#ifdef USE_PERSIAN_CALENDAR
	if(month<7) month=month*3;		// 1~6월 : 31일 (3의 배수)
	else if(month<=12) month=month*2;	//7~12월 : 30일 (2의 배수)
	
	c = year/4;	//윤년
	w= (INT16S)year + (INT16S)(c*2) - (INT16S)c + (INT16S)month + day - 1 + 4;	//매년 요일 1증가, 윤년 경우 2증가
	c = w%7;	//0: sun, 1: mon, 2: tue ~ 6: sat
#else
    if (month<3) { month=month+12; year--; }
    c=(INT16S) year / 100;
    y=      year % 100;

    w=(INT16S)(c>>2) - (c<<1) + y + (INT16S)(y>>2) +
      (INT16S)( 26*(month+1)/10) + day - 1;
	c = w%7;
	if (c<0) c=7+c;
#endif
        return (char)c;
}

short date_checkleap(short n ) // n=year
{
        INT16S a,c,y,test;

        a=(INT16S)n/4000;
        c=(INT16S) ( (n-4000*a)/100 );
        y=n-4000*a-100*c;
        test=n % 4;
        if (y==0) {
//		if (c==0) return 0;
		if ((c>0) && (test==0) ) return 1;   /* Leap year */
//		else return 0;
	}
        if (y>0 ) {
		if (test==0) return 1;             /* Leap year */
//		else return 0;
	}
        return 0;
}

short date_getdays(short y,short m,short d)	// 1..365
{
        short total,i;

        m     = m-1;
        total = 0;
        for (i=0; i<m; i++) total+=(INT16S)date_getmonthdays(y,i+1);
        total+=d;
        return (total);
}

void date_getnextday(short *y,short *m,short *d,short ds)
{
	short qd,rm;
        qd = *d +ds;
        rm = date_getmonthdays( *y , *m );
        if (qd>rm) {
        M1:     qd=qd-rm;
                (*m)++;
                if ( (*m)>12) { *m = 1; (*y)++; }
                rm = date_getmonthdays( *y , *m );
                if (qd>rm) goto M1;
        }
        *d = qd;
}

void date_getbeforeday(short *y,short *m,short *d,short ds)
{
        INT16S qd,rm;

        qd = *d - ds;
        if (qd<1) {
        M1:     (*m)--;
                if ((*m)<1) { (*m)=12; (*y)--; }
                rm=date_getmonthdays( *y,*m);
                qd = qd + rm;
                if (qd<1) goto M1;
        }
        *d = qd;
}

short date_getyeardays(short y )
{
        return (date_checkleap(y)+365);
}

short date_getmonthdays(short y,short m)
{
        INT16S r;
#ifdef USE_PERSIAN_CALENDAR
        if(m<=6) m=31;
        else if(m==12) m=29+date_checkleap(y+1);
        else m=30;
#else
        m--;
        if (m<=6) {
                r=(m+1)%2;
        } else  r=m%2;
        if (m==1)
                m=28+date_checkleap(y);
        else    m=30+r;
#endif
        return m;
}

//SG060310
short date_getdeltadays(short y1,short m1,short d1,short y2,short m2,short d2)
{
	//date : Temporary date,   date2 : basic date
	short i;
//DEL060829	short k, days1, days2;
	short deltadays;

	deltadays =0;
	if(y1==y2){
		deltadays = date_getdays(y1,m1,d1) - date_getdays(y2,m2,d2);
	}else if(y1>y2){
		deltadays = deltadays + date_getdays(y1,m1,d1) + date_getyeardays(y2) - date_getdays(y2,m2,d2);
		for(i=0;i<(y1-y2-1);i++){
			deltadays += date_getyeardays(i+y2+1);	
		}
	}else {
		deltadays = deltadays + date_getdays(y2,m2,d2) + date_getyeardays(y1) - date_getdays(y1,m1,d1);
		for(i=0;i<(y2-y1-1);i++){
			deltadays += date_getyeardays(i+y1+1);	
		}
		deltadays = (-deltadays);
	}

	return deltadays;
}