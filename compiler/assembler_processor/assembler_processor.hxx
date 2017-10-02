enum ARG_TYPES {
    ARG_NUMBER,
    ARG_REG,
    ARG_STACK,
    ARG_LABEL
};

enum ASM_CMDS {
#define DEF_CMD(cmdname, argqt, code) \
	ASM_##cmdname,
#include "cmds.hxx"
#undef DEF_CMD
	ASM_DEFAULT
};

enum {MAX_ARG_QT = 4};

