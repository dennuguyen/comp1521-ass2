#include "functions.h"
#include "hashtable.h"
#include "utils.h"

#define _R(NAME, FUNCT, STR, FUNC_PTR) [NAME] = FUNCT,
static int _R_LIST[] = { R_TYPE_TABLE };
const int n_R_LIST = sizeof(_R_LIST) / sizeof(_R_LIST[0]);
int R_LIST[] = { [0 ... n_R_LIST - 1] = -1, R_TYPE_TABLE };
#undef _R

#define _I(NAME, OP, STR, FUNC_PTR) [NAME] = OP,
static int _I_LIST[] = { I_TYPE_TABLE };
const int n_I_LIST = sizeof(_I_LIST) / sizeof(_I_LIST[0]);
int I_LIST[] = { [0 ... n_I_LIST - 1] = -1, I_TYPE_TABLE };
#undef _I

#define _J(NAME, OP, STR, FUNC_PTR) [NAME] = OP,
static int _J_LIST[] = { J_TYPE_TABLE };
const int n_J_LIST = sizeof(_J_LIST) / sizeof(_J_LIST[0]);
int J_LIST[] = { [0 ... n_J_LIST - 1] = -1, J_TYPE_TABLE };
#undef _J

#define _P(NAME, FUNCT, STR, FUNC_PTR) [NAME] = FUNCT,
static int _P_LIST[] = { P_TYPE_TABLE };
const int n_P_LIST = sizeof(_P_LIST) / sizeof(_P_LIST[0]);
int P_LIST[] = { [0 ... n_P_LIST - 1] = -1, P_TYPE_TABLE };
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

#define _X(REG_NUM, REG_NAME, STR) STR,
char *REG_NUM_STR[] = { REGISTER_TABLE };
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
