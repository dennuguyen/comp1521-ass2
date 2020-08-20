#include "functions.h"
#include "hashtable.h"
#include "utils.h"

#define SIZEOF(list) (sizeof(list) / sizeof(list[0]))

#define _R(NAME, FUNCT, STR, FUNC_PTR) [NAME] = FUNCT,
const int NUM_R_INSTR = SIZEOF((int[]) { R_TYPE_TABLE });
int R_LIST[] = { [0 ... NUM_R_INSTR] = -1, R_TYPE_TABLE };
#undef _R

#define _I(NAME, OP, STR, FUNC_PTR) [NAME] = OP,
const int NUM_I_INSTR = SIZEOF((int[]) { I_TYPE_TABLE });
int I_LIST[] = { [0 ... NUM_I_INSTR] = -1, I_TYPE_TABLE };
#undef _I

#define _J(NAME, OP, STR, FUNC_PTR) [NAME] = OP,
const int NUM_J_INSTR = SIZEOF((int[]) { J_TYPE_TABLE });
int J_LIST[] = { [0 ... NUM_J_INSTR] = -1, J_TYPE_TABLE };
#undef _J

#define _P(NAME, FUNCT, STR, FUNC_PTR) [NAME] = FUNCT,
const int NUM_P_INSTR = SIZEOF((int[]) { P_TYPE_TABLE });
int P_LIST[] = { [0 ... NUM_P_INSTR] = -1, P_TYPE_TABLE };
#undef _P

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

#define _X(REG_NUM, REG_NAME, NUM_STR, NAME_STR) NUM_STR,
char *REG_NUM_STR[] = { REGISTER_TABLE };
#undef _X

#define _X(REG_NUM, REG_NAME, NUM_STR, NAME_STR) NAME_STR,
char *REG_NAME_STR[] = { REGISTER_TABLE };
#undef _X

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
