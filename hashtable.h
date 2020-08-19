#pragma once

#include "functions.h"
#include "hardware.h"
#include "utils.h"

#define _X(REG_NUM, REG_NAME, STR) STR,
char *REG_NUM_STR[] = { REGISTER_TABLE };
#undef _X

#define _R(NAME, FUNCT, STR, FUNC_PTR) [NAME] = FUNC_PTR,
void (*R_FUNCT_PTR[])(CPU *, REGISTER *rs, REGISTER *rt, REGISTER *rd, int shamt, int funct) = { R_TYPE_TABLE };
#undef _R

#define _I(NAME, OP, STR, FUNC_PTR) [NAME] = FUNC_PTR,
void (*I_FUNCT_PTR[])(CPU *, REGISTER *rs, REGISTER *rt, int imm) = { I_TYPE_TABLE };
#undef _I

#define _J(NAME, OP, STR, FUNC_PTR) [NAME] = FUNC_PTR,
void (*J_FUNCT_PTR[])(CPU *, REGISTER *addr) = { J_TYPE_TABLE };
#undef _J

#define _P(NAME, FUNCT, STR, FUNC_PTR) [NAME] = FUNC_PTR,
void (*P_FUNCT_PTR[])(CPU *, REGISTER *rs, REGISTER *rt, REGISTER *rd, int shamt, int funct) = { P_TYPE_TABLE };
#undef _P

#define _R(NAME, FUNCT, STR, FUNC_PTR) [NAME] = STR,
char *R_STR[] = { R_TYPE_TABLE };
#undef _R

#define _I(NAME, OP, STR, FUNC_PTR) [NAME] = STR,
char *I_STR[] = { I_TYPE_TABLE };
#undef _I

#define _J(NAME, OP, STR, FUNC_PTR) [NAME] = STR,
char *J_STR[] = { J_TYPE_TABLE };
#undef _J

#define _P(NAME, FUNCT, STR, FUNC_PTR) [NAME] = STR,
char *P_STR[] = { P_TYPE_TABLE };
#undef _P
