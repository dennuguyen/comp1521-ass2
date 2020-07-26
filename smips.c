/**
 * Simple MIPS emulator called SMIPS for COMP1521 which interprets hexadecimal
 * encoded instructions.
 * 
 * SMIPS outputs:
 *  - the instruction to each instruction opcode
 *  - output produced by syscalls
 *  - register values when program terminates
 * 
 * SMIPS currently supports the following subset of MIPS instructions:
 *  - add  $d, $s, $t
 *  - sub  $d, $s, $t
 *  - and  $d, $s, $t
 *  - or   $d, $s, $t
 *  - slt  $d, $s, $t
 *  - mul  $d, $s, $t
 *  - beq  $s, $t, I
 *  - bne  $s, $t, I
 *  - addi $t, $s, I
 *  - slti $t, $s, I
 *  - andi $t, $s, I
 *  - ori  $t, $s, I
 *  - lui  $t, I
 *  - syscall (1, 10, 11)
 * 
 * Author: Dan Nguyen (z5206032)
 * 
 * Trivia:
 *  - Opcode format: 000000 00000 00000 00000 00000 000000
 * 
 * Resources:
 *  - http://max.cs.kzoo.edu/cs230/Resources/MIPS/MachineXL/InstructionFormats.html
 *  - https://opencores.org/projects/plasma/opcodes
 *  - https://www.slideshare.net/tagbagtroj/mips-opcodes
 *  - http://www.drdobbs.com/the-new-c-x-macros/184401387
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NUM_REGISTERS 32
#define RAM_BUFFER 65536
#define MAX_LINE 128

/******************************************************************************
 *                              MIPS CONVENTIONS                              *
 ******************************************************************************/

/**
 * MIPS data types
 */
typedef __uint32_t word_t; // Size of word.
typedef __uint16_t half_t; // Size of half.
typedef __uint8_t byte_t;  // Size of byte.

/**
 * MIPS has 32 registers numbers.
 */
typedef enum reg_num_t
{
    $0,
    $1,
    $2,
    $3,
    $4,
    $5,
    $6,
    $7,
    $8,
    $9,
    $10,
    $11,
    $12,
    $13,
    $14,
    $15,
    $16,
    $17,
    $18,
    $19,
    $20,
    $21,
    $22,
    $23,
    $24,
    $25,
    $26,
    $27,
    $28,
    $29,
    $30,
    $31,
} reg_num_t;

/**
 * MIPS registers have conventional names.
 */
typedef enum reg_name_t
{
    $zero,
    $at,
    $v0,
    $v1,
    $a0,
    $a1,
    $a2,
    $a3,
    $t0,
    $t1,
    $t2,
    $t3,
    $t4,
    $t5,
    $t6,
    $t7,
    $s0,
    $s1,
    $s2,
    $s3,
    $s4,
    $s5,
    $s6,
    $s7,
    $t8,
    $t9,
    $k0,
    $k1,
    $gp,
    $sp,
    $fa,
    $ra,
} reg_name_t;

// typedef enum fl_reg_name
// {
//     $f0,
//     $f1,
//     $f2,
//     $f4,
//     $f5,
//     $f6,
//     $f7,
//     $f8,
//     $f9,
//     $f10,
//     $f12,
//     $f13,
//     $f14,
//     $f16,
//     $f17,
//     $f18,
//     $f20,
//     $f21,
//     $f22,
//     $f23,
//     $f24,
//     $f25,
//     $f26,
//     $f27,
//     $f28,
//     $f29,
//     $f30,
// } fl_reg_name;

// typedef enum bin_reg_name
// {
//     Lo,
//     Hi
// } bin_reg_name;

/**
 * Designated initialiser to allow "reverse lookup" into reg_num_t enums.
 * 
 * Returns the register from reg_num_t as a string.
 */
char *REG_NUM_STR[] = {
    [$0] = "$0",
    [$1] = "$1",
    [$2] = "$2",
    [$3] = "$3",
    [$4] = "$4",
    [$5] = "$5",
    [$6] = "$6",
    [$7] = "$7",
    [$8] = "$8",
    [$9] = "$9",
    [$10] = "$10",
    [$11] = "$11",
    [$12] = "$12",
    [$13] = "$13",
    [$14] = "$14",
    [$15] = "$15",
    [$16] = "$16",
    [$17] = "$17",
    [$18] = "$18",
    [$19] = "$19",
    [$20] = "$20",
    [$21] = "$21",
    [$22] = "$22",
    [$23] = "$23",
    [$24] = "$24",
    [$25] = "$25",
    [$26] = "$26",
    [$27] = "$27",
    [$28] = "$28",
    [$29] = "$29",
    [$30] = "$30",
    [$31] = "$31",
};

/******************************************************************************
 *                           OPCODE INTERPRETATION                            *
 ******************************************************************************/

/**
 * R-type instructions are encoded by funct and have 0000000 opcode.
 */
typedef enum funct_t
{
    ADD = 0b100000,
    ADDU = 0b100001,
    AND = 0b100100,
    BREAK = 0b001101,
    DIV = 0b011010,
    DIVU = 0b011011,
    JALR = 0b001001,
    JR = 0b001000,
    MFHI = 0b010000,
    MFLO = 0b010010,
    MTHI = 0b010001,
    MTLO = 0b010011,
    MULT = 0b011000,
    MULTU = 0b011001,
    NOR = 0b100111,
    OR = 0b100101,
    SLL = 0b000000,
    SLLV = 0b000100,
    SLT = 0b101010,
    SLTU = 0b101011,
    SRA = 0b000011,
    SRAV = 0b000111,
    SRL = 0b000010,
    SRLV = 0b000110,
    SUB = 0b100010,
    SUBU = 0b100011,
    SYSCALL = 0b001100,
    XOR = 0b100110
} funct_t;

/**
 * I-type and J-type instructions are encoded by opcode.
 */
typedef enum op_t
{
    /* I-type instructions */
    ADDI = 0b001000,
    ADDIU = 0b001001,
    ANDI = 0b001100,
    BEQ = 0b000100,
    BGEZ = 0b000001,
    BGTZ = 0b000111,
    BLEZ = 0b000110,
    BLTZ = 0b000001,
    BNE = 0b000101,
    LB = 0b100000,
    LBU = 0b100100,
    LH = 0b100001,
    LHU = 0b100101,
    LUI = 0b001111,
    LW = 0b100011,
    LWCL = 0b110001,
    ORI = 0b001101,
    SB = 0b101000,
    SLTI = 0b001010,
    SLTIU = 0b001011,
    SH = 0b101001,
    SW = 0b101011,
    SWCL = 0b111001,
    XORI = 0b001110,

    /* J-type instructions */
    J = 0b000010,
    JAL = 0b000011
} op_t;

/**
 * R-type format.
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
 * I-type format.
 */
typedef struct I_FORMAT
{
    unsigned op : 6;
    unsigned rs : 5;
    unsigned rt : 5;
    signed imm : 16;
} I_FORMAT;

/**
 * J-type format.
 */
typedef struct J_FORMAT
{
    unsigned op : 6;
    unsigned addr : 26;
} J_FORMAT;

/**
 * Extract the bit fields from the opcode for an R-type instruction.
 */
R_FORMAT extract_R_FORMAT(int opcode)
{
    R_FORMAT instr;
    instr.op = (opcode >> 26);
    instr.rs = ((opcode >> 21) & 0x1F);
    instr.rt = ((opcode >> 16) & 0x1F);
    instr.rd = ((opcode >> 11) & 0x1F);
    instr.shamt = ((opcode >> 6) & 0x1F);
    instr.funct = (opcode & 0x3F);
    return instr;
}

/**
 * Extract the bit fields from the opcode for an I-type instruction.
 */
I_FORMAT extract_I_FORMAT(int opcode)
{
    I_FORMAT instr;
    instr.op = (opcode >> 26);
    instr.rs = ((opcode >> 21) & 0x1F);
    instr.rt = ((opcode >> 16) & 0x1F);
    instr.imm = (opcode & 0xFFFF);
    return instr;
}

/**
 * Extract the bit fields from the opcode for a J-type instruction.
 */
J_FORMAT extract_J_FORMAT(int opcode)
{
    J_FORMAT instr;
    instr.op = (opcode >> 26);
    instr.addr = (opcode & 0x3FFFFFF);
    return instr;
}

/**
 * Check if opcode is in R format.
 */
bool is_R_FORMAT(int opcode)
{
    if ((opcode >> 26) == 0b000000)
        return true;
    return false;
}

/**
 * Check if instruction is in J format.
 */
bool is_J_FORMAT(int opcode)
{
    if ((opcode >> 26) == J || (opcode >> 26) == JAL)
        return true;
    return false;
}

/**
 * Check if instruction is in I format.
 */
bool is_I_FORMAT(int opcode)
{
    if (!is_R_FORMAT(opcode) && !is_J_FORMAT(opcode))
        return true;
    return false;
}

/**
 * Designated initialiser to allow "reverse lookup" into func_t enums.
 * 
 * Returns an R-type instruction from func_t as a string.
 */
char *R_STR[] = {
    [ADD] = "add",
    [ADDU] = "addu",
    [AND] = "and",
    [BREAK] = "break",
    [DIV] = "div",
    [DIVU] = "divu",
    [JALR] = "jalr",
    [JR] = "jr",
    [MFHI] = "mfhi",
    [MFLO] = "mflo",
    [MTHI] = "mthi",
    [MTLO] = "mtlo",
    [MULT] = "mult",
    [MULTU] = "multu",
    [NOR] = "nor",
    [OR] = "or",
    [SLL] = "sll",
    [SLLV] = "sllv",
    [SLT] = "slt",
    [SLTU] = "sltu",
    [SRA] = "sra",
    [SRAV] = "srav",
    [SRL] = "srl",
    [SRLV] = "srlv",
    [SUB] = "sub",
    [SUBU] = "subu",
    [SYSCALL] = "syscall",
    [XOR] = "xor"};

/**
 * Designated initialiser to allow "reverse lookup" into op_t enums.
 * 
 * Returns an I-type instruction from op_t as a string.
 */
char *I_STR[] = {
    [ADDI] = "addi",
    [ADDIU] = "addiu",
    [ANDI] = "andi",
    [BEQ] = "beq",
    [BGEZ] = "bgez",
    [BGTZ] = "bgtz",
    [BLEZ] = "blez",
    // [BLTZ] = "bltz",
    [BNE] = "bne",
    [LB] = "lb",
    [LBU] = "lbu",
    [LH] = "lh",
    [LHU] = "lhu",
    [LUI] = "lui",
    [LW] = "lw",
    [LWCL] = "LWCL",
    [ORI] = "ori",
    [SB] = "sb",
    [SLTI] = "slti",
    [SLTIU] = "sltiu",
    [SH] = "sh",
    [SW] = "sw",
    [SWCL] = "swcl",
    [XORI] = "xori"};

/**
 * Designated initialiser to allow "reverse lookup" into op_t enums.
 * 
 * Returns an J-type instruction from op_t as a string.
 */
char *J_STR[] = {
    [J] = "j",
    [JAL] = "jal"};

/******************************************************************************
 *                               HARDWARE                                     *
 ******************************************************************************/

typedef __uint32_t reg_t; // Size of a MIPS register is 32 bits.

/**
 * A MIPS register has a register name and value of size 'reg_t'.
 */
typedef struct REGISTER
{
    reg_name_t name; // Name of register
    reg_t value;     // Value of register
} REGISTER;

/**
 * A MIPS CPU has a program counter, 32 x 32-bit general purpose registers,
 * 16 x 64-bit floating point registers, Hi and Lo registers.
 */
typedef struct CPU
{
    unsigned char pc;             // Program Counter
    REGISTER *reg[NUM_REGISTERS]; // Array of CPU registers
} CPU;

/**
 * Initialise the register given a register name.
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
    reg->value = 0;

    return reg;
}

/**
 * Initialise the RAM.
 */
int *init_RAM()
{
    int *ram = malloc(sizeof(RAM_BUFFER));
    if (ram == NULL)
    {
        fprintf(stderr, "ERROR: Failed to allocate memory for RAM\n");
        exit(EXIT_FAILURE);
    }

    return ram;
}

/**
 * Initialise the CPU.
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

    return cpu;
}

/**
 * Destroy the register.
 */
void free_reg(REGISTER *reg)
{
    free(reg);
    reg = NULL;
}

/**
 * Destroy the RAM.
 */
void free_RAM(int *ram)
{
    free(ram);
    ram = NULL;
}

/**
 * Destroy the CPU.
 */
void free_CPU(CPU *cpu)
{
    for (int i = 0; i < NUM_REGISTERS; i++)
        free_reg(cpu->reg[i]);

    free(cpu);
    cpu = NULL;
}

/******************************************************************************
 *                              MIPS FUNCTIONS                                *
 ******************************************************************************/

/**
 * Emulate MIPS add.
 */
void add(CPU *cpu, REGISTER *d, REGISTER *s, REGISTER *t)
{
    d->value = s->value + t->value;
}

/**
 * Emulate MIPS and.
 */
void and (CPU * cpu, REGISTER *d, REGISTER *s, REGISTER *t)
{
    d->value = s->value & t->value;
}

/**
 * Emulate MIPS sub.
 */
void sub(CPU *cpu, REGISTER *d, REGISTER *s, REGISTER *t)
{
    d->value = s->value - t->value;
}

/**
 * Emulate MIPS or.
 */
void or (CPU * cpu, REGISTER *d, REGISTER *s, REGISTER *t)
{
    d->value = s->value | t->value;
}

/**
 * Emulate MIPS slt.
 */
void slt(CPU *cpu, REGISTER *d, REGISTER *s, REGISTER *t)
{
    d->value = s->value < t->value ? 1 : 0;
}

/**
 * Emulate MIPS mul.
 */
void mul(CPU *cpu, REGISTER *d, REGISTER *s, REGISTER *t)
{
    d->value = s->value * t->value;
}

/**
 * Emulate MIPS beq.
 */
void beq(CPU *cpu, REGISTER *s, REGISTER *t, int I)
{
    if (s->value == t->value)
        cpu->pc += I;
}

/**
 * Emulate MIPS bne.
 */
void bne(CPU *cpu, REGISTER *s, REGISTER *t, int I)
{
    if (s->value != t->value)
        cpu->pc += I;
}

/**
 * Emulate MIPS addi.
 */
void addi(CPU *cpu, REGISTER *t, REGISTER *s, int I)
{
    t->value = s->value + I;
}

/**
 * Emulate MIPS slti.
 */
void slti(CPU *cpu, REGISTER *t, REGISTER *s, int I)
{
    t->value = s->value < I;
}

/**
 * Emulate MIPS andi.
 */
void andi(CPU *cpu, REGISTER *t, REGISTER *s, int I)
{
    t->value = s->value & I;
}

/**
 * Emulate MIPS ori.
 */
void ori(CPU *cpu, REGISTER *t, REGISTER *s, int I)
{
    t->value = s->value | I;
}

/**
 * Emulate MIPS ori.
 */
void lui(CPU *cpu, REGISTER *t, int I)
{
    t->value = I << 16;
}

/**
 * Emulate MIPS syscall.
 */
void syscall(CPU *cpu, REGISTER *a0, REGISTER *a1, REGISTER *a2, REGISTER *a3, REGISTER *v0)
{
    switch (v0->value)
    {
    case 1:
        printf("%d", a0->value);
        break;
    // case 2:
    //     break;
    // case 3:
    //     break;
    // case 4:
    //     printf("%s", &(a0->value));
    //     break;
    // case 5:
    //     scanf("%d", &(v0->value));
    //     break;
    // case 6:
    //     scanf("%f", &(v0->value));
    //     break;
    // case 7:
    //     scanf("%lf", &(v0->value));
    //     break;
    // case 8:
    //     fgets(a0->value, a1->value, stdin);
    //     break;
    // case 9:
    //     break;
    case 10:
        exit(EXIT_SUCCESS);
        break;
    case 11:
        printf("%c", a0->value);
        break;
    default:
        fprintf(stderr, "ERROR: undefined register $v0 has %d\n", v0->value);
        exit(EXIT_FAILURE);
    }
}

/******************************************************************************
 *                                  MAIN                                      *
 ******************************************************************************/

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
    int *ram = init_RAM();

    printf("Program\n");
    char line[MAX_LINE];
    for (int i = 0; fgets(line, sizeof(line), f); i++)
    {
        int opcode = (int)strtol(line, NULL, 16);

        if (is_R_FORMAT(opcode))
        {
            R_FORMAT instr = extract_R_FORMAT(opcode);
            if (instr.funct == SYSCALL)
                printf("\t%d: %s\n", i, R_STR[instr.funct]);
            else
                printf("\t%d: %s\t%s %s %s\n", i, R_STR[instr.funct], REG_NUM_STR[instr.rd], REG_NUM_STR[instr.rs], REG_NUM_STR[instr.rt]);
        }
        else if (is_I_FORMAT(opcode))
        {
            I_FORMAT instr = extract_I_FORMAT(opcode);
            printf("\t%d: %s\t%s %s %d\n", i, I_STR[instr.op], REG_NUM_STR[instr.rt], REG_NUM_STR[instr.rs], instr.imm);
        }
        else if (is_J_FORMAT(opcode))
        {
            J_FORMAT instr = extract_J_FORMAT(opcode);
            printf("\t%d: %s\n", i, J_STR[instr.op]);
        }
    }

    printf("Output\n");

    printf("Registers After Execution\n");
    for (int i = 0; i < NUM_REGISTERS; i++)
        if (cpu->reg[i]->value != 0)
            printf("%s = %d\n", REG_NUM_STR[cpu->reg[i]->name], cpu->reg[i]->value);

    free_CPU(cpu);
    free_RAM(ram);
    fclose(f);

    return EXIT_SUCCESS;
}
