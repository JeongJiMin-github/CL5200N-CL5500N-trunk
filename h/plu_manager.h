#include <stdio.h>
#include "globals.h"
#include "mode_main.h"

extern INT16S  plu_get_recordsize(void);
extern INT16S  plu_get_ptype_str_point(INT16U cur_ptype);
extern INT32U plu_get_nth_point(INT16U nth,INT16U *xsize);
// extern INT16S  plu_get_nth_point(INT16U nth,INT8U *stype,INT16U *xsize);
// Speed 2004.11
// extern void plu_save_deptid_com(INT8U m,INT32U pnt,INT32U data,INT8U stype,INT16U usize);
// extern void plu_save_deptid_com(INT32U pnt,INT32U data,INT8U stype,INT16U usize);

extern void   plu_get_field(INT16U id,INT16U ptype,INT8U *buffer,INT16S *usize,INT16S sz);
extern INT32U plu_get_field_point(INT16S id,INT16U ptype,INT16S *usize);
extern INT32U plu_get_field_point_nvram(INT16S id,INT16U ptype,INT16S *usize);
extern INT8U  plu_set_field(INT16S id,INT16U ptype,INT8U *buffer);
extern INT8U plu_callbykey(INT16U deptno,INT32U pluno, PLU_BASE *plu,INT8U *taretype);
//extern INT8U plu_callbykey_fromid(INT16S key,INT16S id,INT32U *data,INT16U *sz);
extern INT8U plu_callbykey_from_fid(INT32U key,INT16S id,INT32U *data,INT16U *sz);
extern INT16U plu_get_ptype_from_fid(INT16S id);
extern INT16U plu_get_item_position_from_ptype(INT16S cur_ptype);
extern INT8U  plu_get_stype_from_fid(INT16S id);
extern INT8U  plu_get_stype_fromid(INT16U id);
extern INT16U plu_get_number(void);
extern INT32U plu_get_next_exist_address(INT16U nth); // n번째 저장된 PLU의 물리적 주소공간 의 몇번째 인지 ?
extern INT8U  plu_get_writestate(INT16U id);
extern INT32U plu_struct_addr_byid(INT16U id);
extern INT32U plu_struct_addr_nameid(INT16U id,INT32U *addr);
extern INT8U  plu_remotecall(INT16U deptno,INT32U pluno,INT8U mode);
extern void plu_save_deptid_com_type(INT8U m,INT32U pnt,INT32U data,INT8U stype,INT16U usize);

#ifdef USE_INDIA_FUNCTION
extern INT8U UseItemCode13Digit;
#endif
extern INT8U NvramTempBuffer[3][2500];