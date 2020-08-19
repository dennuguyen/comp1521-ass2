#include <stdio.h>
#include <stdlib.h>

#include "hardware.h"

/**
 * @brief Initialise a register given a register name.
 *
 * @param name Name of register
 * @return REGISTER*
 */
REGISTER *init_reg(reg_name_t name)
{
    REGISTER *reg = malloc(sizeof(REGISTER));
    if (reg == NULL)
    {
        fprintf(stderr, "ERROR: Failed to allocate memory for Register\n");
        exit(EXIT_FAILURE);
    }

    reg->name = name;

    if ($zero <= name && name <= Hi)
        reg->value.wd = 0;
    else
        reg->value.fl = 0.0;

    return reg;
}

/**
 * @brief Initialise the CPU and its registers.
 *
 * @return CPU*
 */
CPU *init_CPU()
{
    CPU *cpu = malloc(sizeof(CPU));
    if (cpu == NULL)
    {
        fprintf(stderr, "ERROR: Failed to allocate memory for CPU\n");
        exit(EXIT_FAILURE);
    }

    cpu->pc = 0;

    for (int i = 0; i < NUM_REGISTERS; i++)
        cpu->reg[i] = init_reg(i);

    for (int i = 0; i < MAX_INSTR; i++)
        cpu->cache[i] = 0;

    return cpu;
}

/**
 * @brief Destroy the register.
 *
 * @param reg Register to be destroyed
 */
void free_reg(REGISTER *reg)
{
    free(reg);
    reg = NULL;
}

/**
 * @brief Destroy the CPU by freeing all its registers and then itself.
 *
 * @param cpu CPU to be destroyed
 */
void free_CPU(CPU *cpu)
{
    for (int i = 0; i < NUM_REGISTERS; i++)
        free_reg(cpu->reg[i]);

    free(cpu);
    cpu = NULL;
}
