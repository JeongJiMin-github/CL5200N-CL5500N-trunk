#ifndef _DEPARTMENT_H_1
#define _DEPARTMENT_H_1

#pragma pack(2)
typedef struct	{
	INT16U	_code;
	INT16U	_speedkey;
	INT8U	_description[28];
} DEPARTMENT_STRUCT;
#pragma pack()
#define	GROUP_STRUCT_SIZE 32
#endif
  
#ifndef _DEPARTMENT_H_2
#define _DEPARTMENT_H_2

extern 	void	department_create(void);		// command code  0x1311
extern  void    department_delete(void);		// command code  0x1312
extern  void 	department_list(void);			// command code  0x1313

#ifndef USE_TRACE_MEATGRADE
extern 	void	group_create(void);		// command code  0x1321
extern  void    group_delete(void);		// command code  0x1322
extern  void 	group_list(void);			// command code  0x1323
#endif

extern  BOOLEAN	check_dept_exist(INT16U dept_no);
extern void set_department_speedkey(INT16U dept_no);
#endif 



