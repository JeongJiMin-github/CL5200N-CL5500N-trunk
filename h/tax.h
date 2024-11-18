#ifndef _TAX_H_1
#define _TAX_H_1


#define		TAX_STRUCT_SIZE			8
#pragma pack(2)
typedef struct
{
	INT16U		tax_code;
	INT16U		tax_type;
	INT32U		tax_rate;
}	TAX_STRUCT;
#pragma pack()
#endif
  
#ifndef _TAX_H_2
#define _TAX_H_2

#ifdef USE_TAX_RATE
extern 	void	tax_create(void);		// command code  0x1221
extern  void    tax_delete(void);		// command code  0x1222
extern  void 	tax_list(void);			// command code  0x1223
extern  void callback_tax_type(long num);
#endif

#endif 









