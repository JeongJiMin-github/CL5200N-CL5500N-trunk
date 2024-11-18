#ifndef _WORKPLACE_H_1
#define _WORKPLACE_H_1


/*	// Modified by CJK 20060412
typedef	struct  {
		INT16U	code;
		INT8U	text[28];
		INT16U	country_number;
		}WORKPLACE_STRUCT;
#define	SLAUGHTERHOUSE_STRUCT_SIZE		32
#define CUTTINGHALL_STRUCT_SIZE			32
*/
typedef	struct  {
		INT16U	code;
		INT8U	text[18];
		INT16U	country_number;
}WORKPLACE_STRUCT;
#define	SLAUGHTERHOUSE_STRUCT_SIZE		22
#define SLAUGHTERHOUSE_COUNTRY_OFFSET	20
#define CUTTINGHALL_STRUCT_SIZE			22
#define CUTTINGHALL_COUNTRY_OFFSET		20

#endif

  
#ifndef _WORKPLACE_H_2
#define _WORKPLACE_H_2

extern 	void	slaughter_create(void);				// command code  0x1371
extern  void    slaughter_delete(void);				// command code  0x1372
extern  void 	slaughter_list(void);				// command code  0x1373

extern 	void	cuttinghall_create(void);		// command code  0x1381
extern  void    cuttinghall_delete(void);		// command code  0x1382
extern  void 	cuttinghall_list(void);			// command code  0x1383


#endif 
