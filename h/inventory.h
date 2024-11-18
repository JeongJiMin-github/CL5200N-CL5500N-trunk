
#ifdef USE_INDIA_FUNCTION

#define INVENTORY_DEDUCT 0
#define INVENTORY_CHECK  1


extern INT8U ReadPluInventory(INT16U deptno, INT32U pluno, INT32S *inventory);
extern INT8U ReadPluInventoryBagWt(INT16U deptno, INT32U pluno, INT32S *inv, INT32S *bagWt);
extern INT8U ReadPluBagWt(INT16U deptno, INT32U pluno, INT32S *bagWt);
extern INT8U ReadPluInventoryAll(INT16U deptno, INT32U pluno, INT32S *inv, INT32S *bagWt, INT32S *grossInv);
extern INT8U WritePluInventoryBagWt(INT16U deptno, INT32U pluno, INT32S inventory, INT32S bagWt);
extern INT8U WritePluBagWt(INT16U deptno, INT32U pluno, INT32S bagWt);
extern INT8U WritePluInventoryAll(INT16U deptno, INT32U pluno, INT32S inventory, INT32S bagWt, INT32S grossInv);
extern INT8U AddPluInventory(INT16U deptno, INT32U pluno, INT32S inventory);
extern INT8U AddPluInventoryBagWt(INT16U deptno, INT32U pluno, INT32S inventory, INT32S bagWt);
extern INT8U DeductPluInventory(INT16U deptno, INT32U pluno, INT8U orgType, INT8U newType, INT32S weight, INT32S qty, INT8U check);
extern void DeletePluInventory(INT16U deptno, INT32U pluno);
extern void DeletePluInventoryAll(void);
extern void inventory_input(void);
extern void inventory_edit(void);
extern void inventory_list(void);

#endif

