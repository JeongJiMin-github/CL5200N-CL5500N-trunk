#ifndef CLERK_STRUCT_DEFINE
#define CLERK_STRUCT_DEFINE
#include "clerk_app.h"
#endif

// extern INT8U system_password(void);
extern INT8U login_clerk(INT16U clerkid);
extern INT8U LastClerk(void);
// extern INT8U password_input(char *input_pass, char dsp_reload);
extern INT8U logout(void);
extern INT8U check_clerk_permission(INT16U clerk_id,  INT8U func_code);
//extern INT8U permission_check(INT16U clerkid,INT8U func_code);
extern INT8U permission_check(INT8U func_code, INT8U login_chk);
extern INT8U permission_check_sub(INT8U func_code, INT8U login_chk);
extern INT8U password_input_clerk(CLERK_STRUCT* clerkp, char dsp_reload);
extern INT8U permission_check_user(INT8U userLevel, INT8U func_code);
extern INT16S   LoadClerk(CLERK_STRUCT* clerkp,INT16U logOperator);