/**
 * @brief Simple MIPS emulator called SMIPS.
 * @author Dan Nguyen
 *
 * SMIPS outputs:
 *  1. the instruction to each instruction opcode
 *  2. output produced by syscalls
 *  3. changed register values when program terminates
 *
 * Some random info:
 *
 * Assemblers parse the file twice: first to get the lexemes and store the
 * information into its symbol table, then the second to produce the machine
 * code.
 *
 * On the first parse, assemblers must determine the instruction size of
 * variable-length instructions to allocate to memory. However MIPS assemblers
 * use 'cursory examination' due to their fixed-length instructions.
 *
 * These two parses can be combined into a single parse with a technique called
 * 'backpatching'.
 *
 * Instruction format: 000000 00000 00000 00000 00000 000000
 *
 * Resources:
 *  - http://max.cs.kzoo.edu/cs230/Resources/MIPS/MachineXL/InstructionFormats.html :: instruction formatting
 *  - https://opencores.org/projects/plasma/opcodes                                 :: opcodes
 *  - https://www.slideshare.net/tagbagtroj/mips-opcodes                            :: MIPS opcodes
 *  - http://www.drdobbs.com/the-new-c-x-macros/184401387                           :: X macros
 *  - https://uweb.engr.arizona.edu/~ece369/Resources/spim/MIPSReference.pdf        :: MIPS reference
 *  - https://web.stanford.edu/class/cs143/materials/SPIM_Manual.pdf                :: SPIM manual
 *  - https://www.doc.ic.ac.uk/lab/secondyear/spim/node20.html                      :: floating point instructions
 *  - http://matthews.sites.truman.edu/files/2019/11/pseudoinstructions.pdf         :: pseudo instructions
 *
 * @todo
 *  - check if unsigned functions are correct
 *  - implement common pseudo instructions
 *      - lw, lb, sw, sb, li, move, la, blt, ble, bgt, bge
 *  - stack frames
 *  - assembly parser
 *  - resolve cases where some instructions have common codes
 *  - syscall
 *  - redo is format functions
 */

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "functions.h"
#include "hardware.h"
#include "hashtable.h"
#include "opcode.h"
#include "utils.h"

#define BUFFER 4096

/**
 * @brief Print bits from MSB to LSB.
 *
 * @param value Value of bit
 * @param n_bits Number of bits to be printed
 */
void print_bits(__uint64_t value, int n_bits)
{
    for (int i = n_bits - 1; i >= 0; i--)
    {
        int bit = (value >> i) & 1;
        printf("%d", bit);
    }
    printf("\n");
}

/**
 * @brief Print non-zero registers, $0 - $31.
 *
 * @param cpu Pointer to instantiation of CPU
 */
void print_registers(CPU *cpu)
{
    printf("Registers After Execution\n");

    for (int i = 0; i < NUM_REGISTERS; i++)
        if (cpu->reg[i]->value.wd != 0 && $0 <= i && i <= $31)
            printf("%-3s = %d\n",
                REG_NUM_STR[cpu->reg[i]->name],
                cpu->reg[i]->value.wd);
}

/**
 * @brief Check if instruction code is valid. If it is not valid, print the
 * file, line number and instruction then exit.
 *
 * @param file File name
 * @param instr_code Encoded MIPS instruction
 * @param i Line number
 */
void check_valid_instruction(char *file, int instr_code, int i)
{
    if (!is_P_FORMAT(instr_code) &&
        !is_R_FORMAT(instr_code) &&
        !is_I_FORMAT(instr_code) &&
        !is_J_FORMAT(instr_code))
    {
        printf("%s:%d: invalid instruction code: %.6d\n", file, i, instr_code);
        exit(EXIT_FAILURE);
    }
}

/**
 * @brief Prints to stdout an equivalent Assembly instruction for a given
 * encoded instruction.
 *
 * @param cpu Pointer to instantiation of CPU
 * @param instr_code Encoded MIPS instruction
 */
void print_instruction_by_format(CPU *cpu, int instr_code)
{
    if (is_P_FORMAT(instr_code))
    {
        R_FORMAT instr = extract_R_FORMAT(instr_code);
        if (instr.funct == SYSCALL)
        {
            printf("%s", P_STR[instr.funct]);
        }
        else
        {
            printf("%-4s %s, %s, %s",
                P_STR[instr.funct],
                REG_NUM_STR[instr.rd],
                REG_NUM_STR[instr.rs],
                REG_NUM_STR[instr.rt]);
        }
    }
    else if (is_R_FORMAT(instr_code))
    {
        R_FORMAT instr = extract_R_FORMAT(instr_code);
        printf("%-4s %s, %s, %s",
            R_STR[instr.funct],
            REG_NUM_STR[instr.rd],
            REG_NUM_STR[instr.rs],
            REG_NUM_STR[instr.rt]);
    }
    else if (is_I_FORMAT(instr_code))
    {
        I_FORMAT instr = extract_I_FORMAT(instr_code);
        if (instr.op == BEQ || instr.op == BNE)
        {
            printf("%-4s %s, %s, %d",
                I_STR[instr.op],
                REG_NUM_STR[instr.rs],
                REG_NUM_STR[instr.rt],
                instr.imm);
        }
        else if (instr.op == LUI)
        {
            printf("%-4s %s, %d",
                I_STR[instr.op],
                REG_NUM_STR[instr.rt],
                instr.imm);
        }
        else
        {
            printf("%-4s %s, %s, %d",
                I_STR[instr.op],
                REG_NUM_STR[instr.rt],
                REG_NUM_STR[instr.rs],
                instr.imm);
        }
    }
    else if (is_J_FORMAT(instr_code))
    {
        J_FORMAT instr = extract_J_FORMAT(instr_code);
        printf("%-4s %d", J_STR[instr.op], instr.addr);
    }
}

/**
 * @brief Carry out the CPU's processes.
 *
 * @param cpu Pointer to instantiation of CPU
 * @param instr_code Encoded MIPS instruction
 */
void processes(CPU *cpu, int instr_code)
{
    if (is_P_FORMAT(instr_code))
    {
        R_FORMAT instr = extract_R_FORMAT(instr_code);
        (*P_FUNCT_PTR[instr.funct])(cpu,
            cpu->reg[instr.rs],
            cpu->reg[instr.rt],
            cpu->reg[instr.rd],
            instr.shamt,
            instr.funct);
    }
    else if (is_R_FORMAT(instr_code))
    {
        R_FORMAT instr = extract_R_FORMAT(instr_code);
        (*R_FUNCT_PTR[instr.funct])(cpu,
            cpu->reg[instr.rs],
            cpu->reg[instr.rt],
            cpu->reg[instr.rd],
            instr.shamt,
            instr.funct);
    }
    else if (is_I_FORMAT(instr_code))
    {
        I_FORMAT instr = extract_I_FORMAT(instr_code);
        (*I_FUNCT_PTR[instr.op])(cpu,
            cpu->reg[instr.rs],
            cpu->reg[instr.rt],
            instr.imm);
    }
    else if (is_J_FORMAT(instr_code))
    {
        J_FORMAT instr = extract_J_FORMAT(instr_code);
        (*J_FUNCT_PTR[instr.op])(cpu, cpu->reg[instr.addr]);
    }
    else
    {
        printf("Invalid instruction code: %.6d\n", instr_code);
        exit(EXIT_FAILURE);
    }

    // Clean up registers
    cpu->reg[$zero]->value.wd = 0;
}

/**
 * @brief
 *
 * @param f Stream of encoded MIPS instructions
 * @param cpu Pointer to instantiation of CPU
 * @param file Name of instruction file
 * @param j Instruction counter
 */
void assembly_loader(FILE *f, CPU *cpu, char *file, int *j)
{
    char line[BUFFER];
    for (int i = 0; fgets(line, sizeof(line), f) && *j < MAX_INSTR; i++, (*j)++)
    {
        int instr_code = (int)strtol(line, NULL, 16);
        // while ()
        // {

        // }

        check_valid_instruction(file, instr_code, i);
        cpu->cache[i] = instr_code;
    }
}

/**
 * @brief
 *
 * @param f Stream of encoded MIPS instructions
 * @param cpu Pointer to instantiation of CPU
 * @param file Name of instruction file
 * @param j Instruction counter
 */
void hexadecimal_loader(FILE *f, CPU *cpu, char *file, int *j)
{
    char line[BUFFER];
    for (int i = 0; fgets(line, sizeof(line), f) && *j < MAX_INSTR; i++, (*j)++)
    {
        int instr_code = (int)strtol(line, NULL, 16);
        check_valid_instruction(file, instr_code, i);
        cpu->cache[i] = instr_code;
    }
}

/**
 * @brief
 *
 * @param f Stream of encoded MIPS instructions
 * @param cpu Pointer to instantiation of CPU
 * @param file Name of instruction file
 */
void parser(FILE *f, CPU *cpu, char *file)
{
    int j = 0; // Counter for number of instructions loaded

    // Check file type and load program into cache
    char *file_type = strrchr(file, '.');
    if (strncmp(file_type, ".s", 3) == 0)
    {
        assembly_loader(f, cpu, file, &j);
    }
    else if (strncmp(file_type, ".hex", 5) == 0)
    {
        hexadecimal_loader(f, cpu, file, &j);
    }
    else
    {
        fprintf(stderr, "ERROR: Incorrect file, type %s\n", file);
        exit(EXIT_FAILURE);
    }

    printf("Program\n");

    // Print program instructions
    for (int i = 0; i < j; i++)
    {
        printf("%3d: ", i);
        print_instruction_by_format(cpu, cpu->cache[i]);
        printf("\n");
    }

    printf("Output\n");

    // Execute the program loaded in cache while PC is in [0, j)
    for (cpu->pc = 0; 0 <= cpu->pc && cpu->pc < j; cpu->pc++)
        processes(cpu, cpu->cache[cpu->pc]);
}

/**
 * @brief Main function.
 *
 * @param argv
 * @param argc
 * @return int
 */
int main(int argv, char *argc[])
{
    if (argv != 2)
    {
        fprintf(stderr, "ERROR: Given %d arguments instead of 2\n", argv);
        exit(EXIT_FAILURE);
    }

    FILE *f = fopen(argc[1], "r");
    if (f == NULL)
    {
        fprintf(stderr, "ERROR: Failed to open %s\n", argc[1]);
        exit(EXIT_FAILURE);
    }

    CPU *cpu = init_CPU();

    parser(f, cpu, argc[1]);
    print_registers(cpu);

    free_CPU(cpu);
    fclose(f);
    // for (int i = 0; i < NUM_R_INSTR; i++)
    //     printf("%d ", R_LIST[i]);
    // printf("\n");

    // for (int i = 0; i < NUM_I_INSTR; i++)
    //     printf("%d ", I_LIST[i]);
    // printf("\n");

    // for (int i = 0; i < NUM_J_INSTR; i++)
    //     printf("%d ", J_LIST[i]);
    // printf("\n");

    // for (int i = 0; i < NUM_P_INSTR; i++)
    //     printf("%d ", P_LIST[i]);
    // printf("\n");

    return EXIT_SUCCESS;
}
