extern char  date_getweek(short year,short month,short day );
extern short date_checkleap(short n ); // n=year
extern short date_getdays(short y,short m,short d);	// 1..365
extern void  date_getnextday(short *y,short *m,short *d,short ds);
extern void  date_getbeforeday(short *y,short *m,short *d,short ds);
extern short date_getyeardays(short y );
extern short date_getmonthdays(short y,short m);
extern char  date_checkday(short year,short month,short day);
extern short date_getdeltadays(short y1,short m1,short d1,short y2,short m2,short d2);
