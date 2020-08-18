#pragma once

#include <stdbool.h>

#include "utils.h"

/**
 * @struct R_FORMAT
 * @brief Struct with bit fields to store `op`, `rs`, `rt`, `rd`, `shamt`,
 * `funct`.
 */
typedef struct R_FORMAT
{
    unsigned op : 6;
    unsigned rs : 5;
    unsigned rt : 5;
    unsigned rd : 5;
    unsigned shamt : 5;
    unsigned funct : 6;
} R_FORMAT;

/**
 * @struct I_FORMAT
 * @brief Struct with bit fields to store `op`, `rs`, `rt`, `imm`.
 */
typedef struct I_FORMAT
{
    unsigned op : 6;
    unsigned rs : 5;
    unsigned rt : 5;
    signed imm : 16;
} I_FORMAT;

/**
 * @struct J_FORMAT
 * @brief Struct with bit fields to store `op`, `addr`.
 */
typedef struct J_FORMAT
{
    unsigned op : 6;
    unsigned addr : 26;
} J_FORMAT;

char *R_STR(int key);
char *I_STR(int key);
char *J_STR(int key);
char *P_STR(int key);
R_FORMAT extract_R_FORMAT(int instr_code);
I_FORMAT extract_I_FORMAT(int instr_code);
J_FORMAT extract_J_FORMAT(int instr_code);
bool is_R_FORMAT(int instr_code);
bool is_J_FORMAT(int instr_code);
bool is_P_FORMAT(int instr_code);
bool is_I_FORMAT(int instr_code);
