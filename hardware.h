#pragma once

#include "utils.h"

/**
 * MIPS data types
 */
typedef uint32_t word_t; // Size of word.
typedef uint16_t half_t; // Size of half.
typedef uint8_t byte_t;  // Size of byte.

/**
 * @union reg_t
 * @brief Registers are initialised to either wd or fl which are `reg_t`.
 */
typedef union reg_t
{
    __int32_t wd; // Size of a MIPS register is 32 bits.
    float fl;     // Size of a MIPS register is 32 bits.
} reg_t;

/**
 * @struct REGISTER
 * @brief A MIPS register has a `name` and `value` of size 'reg_t'.
 */
typedef struct REGISTER
{
    reg_name_t name; // Name of register
    reg_t value;     // Value of register
} REGISTER;

/**
 * @struct CPU
 * @brief A MIPS CPU has a program counter, registers and cache.
 */
typedef struct CPU
{
    unsigned int pc;              // Program Counter
    REGISTER *reg[NUM_REGISTERS]; // Array of CPU registers
    int cache[MAX_INSTR];         // Cache to store programs
} CPU;

REGISTER *init_reg(reg_name_t name);
CPU *init_CPU();
void free_reg(REGISTER *reg);
void free_CPU(CPU *cpu);
