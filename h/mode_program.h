#ifndef MODE_PROGRAM_DEF1
#define MODE_PROGRAM_DEF1

#define MODECODE_MASK		0xF000
#define MASTERCODE_MASK		0x0F00
#define SUBCODE_MASK		0x00F0
#define ENDCODE_MASK		0x000F

extern void mode_program(void);
extern void mode_return(INT16U code);
#endif
