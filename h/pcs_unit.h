#ifndef _PCS_UNIT_H_1
#define _PCS_UNIT_H_1

#ifdef USE_GSMART_PLU
 #define		PCS_UNIT_STRUCT_SIZE	 8
#else
 #define		PCS_UNIT_STRUCT_SIZE	 18
#endif

#endif

  
#ifndef _PCS_UNIT_H_2
#define _PCS_UNIT_H_2

extern 	void	pcsunit_create(void);		// command code  0x1311
extern  void    pcsunit_delete(void);		// command code  0x1312
extern  void 	pcsunit_list(void);			// command code  0x1313
#ifdef USE_GSMART_PLU
extern  void 	pcsunit_default_KR(void);
#endif

#endif 



