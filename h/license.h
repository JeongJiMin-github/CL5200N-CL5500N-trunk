/*****************************************************************************
|*		       
|*  Copyright		:	(c) 2013 CAS
|*  Filename		:	license.h
|*  Version		:	1.0
|*  Programmer(s)		:	cjk
|*  Created		:	2013/02/18
|*  Description		:	
|*              				
****************************************************************************/
	
#define KEY_SCALE "434C355363616C65"

#define CODE_FUNC1 "43568B9AFFD0209E"
#define CODE_FUNC2 "D687F5D2C13A9807"
#define CODE_FUNC3 "AE0F2EB9A142D441"
#define CODE_FUNC4 "C5F1E10B9F2E028D"
#define CODE_FUNC5 "FAD81B611C9580B0"
#define CODE_FUNC6 "438D510F9EAEA0B2"
#define CODE_FUNC7 "EF37281CED5FB0D2"
#define CODE_FUNC8 "81D155BF3F377E7C"


#define LIC_SCALE_HW_ID_SIZE	8
#define LIC_FUNCTION_NUMBER	4
#define LIC_FUNCTION_CODE_SIZE	8
#define LIC_KEY_SIZE		8
#define LIC_STRUCT_SIZE	(LIC_SCALE_HW_ID_SIZE + LIC_FUNCTION_NUMBER * LIC_FUNCTION_CODE_SIZE + 40) // reserve 40bytes

typedef struct
{
	INT8U ScaleHWID_E[LIC_SCALE_HW_ID_SIZE];
	INT8U funcEnable[LIC_FUNCTION_NUMBER];
} LICENSE_STRUCT;

extern LICENSE_STRUCT Lic;


extern void lic_manage(void);
extern INT8U lic_check_validation_scaleHWID(void);
extern INT8U lic_check_function_license(INT8U fnId, INT8U *funcCode_E);

