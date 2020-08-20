#pragma once

#include "hardware.h"

// Lists
extern int R_LIST[];
extern int I_LIST[];
extern int J_LIST[];
extern int P_LIST[];

// Size of lists
extern const int n_R_LIST;
extern const int n_I_LIST;
extern const int n_J_LIST;
extern const int n_P_LIST;

// Function pointer lists
extern void (*R_FUNCT_PTR[])(CPU *, REGISTER *rs, REGISTER *rt, REGISTER *rd, int shamt, int funct);
extern void (*I_FUNCT_PTR[])(CPU *, REGISTER *rs, REGISTER *rt, int imm);
extern void (*J_FUNCT_PTR[])(CPU *, REGISTER *addr);
extern void (*P_FUNCT_PTR[])(CPU *, REGISTER *rs, REGISTER *rt, REGISTER *rd, int shamt, int funct);

// String lists
extern char *REG_NUM_STR[];
extern char *R_STR[];
extern char *I_STR[];
extern char *J_STR[];
extern char *P_STR[];
