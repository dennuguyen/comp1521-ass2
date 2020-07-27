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
 *                                X MACROS                                    *
 ******************************************************************************/

/* Registers */
#define REG_NUM_TABLE  \
    X($0, $zero, "$0") \
    X($1, $at, "$1")   \
    X($2, $v0, "$2")   \
    X($3, $v1, "$3")   \
    X($4, $a0, "$4")   \
    X($5, $a1, "$5")   \
    X($6, $a2, "$6")   \
    X($7, $a3, "$7")   \
    X($8, $t0, "$8")   \
    X($9, $t1, "$9")   \
    X($10, $t2, "$10") \
    X($11, $t3, "$11") \
    X($12, $t4, "$12") \
    X($13, $t5, "$13") \
    X($14, $t6, "$14") \
    X($15, $t7, "$15") \
    X($16, $s0, "$16") \
    X($17, $s1, "$17") \
    X($18, $s2, "$18") \
    X($19, $s3, "$19") \
    X($20, $s4, "$20") \
    X($21, $s5, "$21") \
    X($22, $s6, "$22") \
    X($23, $s7, "$23") \
    X($24, $t8, "$24") \
    X($25, $t9, "$25") \
    X($26, $k0, "$26") \
    X($27, $k1, "$27") \
    X($28, $gp, "$28") \
    X($29, $sp, "$29") \
    X($30, $fa, "$30") \
    X($31, $ra, "$31")

/* R-type instructions */
#define R_TYPE_TABLE                              \
    R(ADD, 0b100000, "add", MIPS_add)             \
    R(ADDU, 0b100001, "addu", MIPS_addu)          \
    R(AND, 0b100100, "and", MIPS_and)             \
    R(BREAK, 0b001101, "break", MIPS_break)       \
    R(DIV, 0b011010, "div", MIPS_div)             \
    R(DIVU, 0b011011, "divu", MIPS_divu)          \
    R(JALR, 0b001001, "jalr", MIPS_jalr)          \
    R(JR, 0b001000, "jr", MIPS_jr)                \
    R(MFHI, 0b010000, "mfhi", MIPS_mfhi)          \
    R(MFLO, 0b010010, "mflo", MIPS_mflo)          \
    R(MTHI, 0b010001, "mthi", MIPS_mthi)          \
    R(MTLO, 0b010011, "mtlo", MIPS_mtlo)          \
    R(MUL, 0b011100, "mul", MIPS_mul)             \
    R(MULT, 0b011000, "mult", MIPS_mult)          \
    R(MULTU, 0b011001, "multu", MIPS_multu)       \
    R(NOR, 0b100111, "nor", MIPS_nor)             \
    R(OR, 0b100101, "or", MIPS_or)                \
    R(SLL, 0b000000, "sll", MIPS_sll)             \
    R(SLLV, 0b000100, "sllv", MIPS_sllv)          \
    R(SLT, 0b101010, "slt", MIPS_slt)             \
    R(SLTU, 0b101011, "sltu", MIPS_sltu)          \
    R(SRA, 0b000011, "sra", MIPS_sra)             \
    R(SRAV, 0b000111, "srav", MIPS_srav)          \
    R(SRL, 0b000010, "srl", MIPS_srl)             \
    R(SRLV, 0b000110, "srlv", MIPS_srlv)          \
    R(SUB, 0b100010, "sub", MIPS_sub)             \
    R(SUBU, 0b100011, "subu", MIPS_subu)          \
    R(SYSCALL, 0b001100, "syscall", MIPS_syscall) \
    R(XOR, 0b100110, "xor", MIPS_xor)

/* I-type instructions */
#define I_TYPE_TABLE                         \
    I(ADDI, 0b001000, "addi", MIPS_addi)     \
    I(ADDIU, 0b001001, "addiu", MIPS_addiu)  \
    I(ANDI, 0b001100, "andi", MIPS_andi)     \
    I(BEQ, 0b000100, "beq", MIPS_beq)        \
    I(BGEZ, 0b000001, "bgez", MIPS_bgez)     \
    I(BGTZ, 0b000111, "bgtz", MIPS_bgtz)     \
    I(BLEZ, 0b000110, "blez", MIPS_blez)     \
    /*I(BLTZ, 0b000001, "bltz", MIPS_bltz)*/ \
    I(BNE, 0b000101, "bne", MIPS_bne)        \
    I(LB, 0b100000, "lb", MIPS_lb)           \
    I(LH, 0b100001, "lh", MIPS_lh)           \
    I(LUI, 0b001111, "lui", MIPS_lui)        \
    I(LW, 0b100011, "lw", MIPS_lw)           \
    I(ORI, 0b001101, "ori", MIPS_ori)        \
    I(SB, 0b101000, "sb", MIPS_sb)           \
    I(SLTI, 0b001010, "slti", MIPS_slti)     \
    I(SH, 0b101001, "sh", MIPS_sh)           \
    I(SW, 0b101011, "sw", MIPS_sw)           \
    I(XORI, 0b001110, "xori", MIPS_xori)

/* J-type instructions */
#define J_TYPE_TABLE            \
    J(J, 0b000010, "j", MIPS_j) \
    J(JAL, 0b000011, "jal", MIPS_jal)

/******************************************************************************
 *                          REGISTER CONVENTIONS                              *
 ******************************************************************************/

/**
 * MIPS data types
 */
typedef __uint32_t word_t; // Size of word.
typedef __uint16_t half_t; // Size of half.
typedef __uint8_t byte_t;  // Size of byte.

/**
 * MIPS has Hi and Lo registers.
 */
typedef enum reg_bin_t
{
    LO,
    HI
} reg_bin_t;

/**
 * MIPS has 32 registers numbers.
 */
#define X(REG_NUM, REG_NAME, STR) REG_NUM,
typedef enum reg_num_t
{
    REG_NUM_TABLE
} reg_num_t;
#undef X

/**
 * MIPS registers have conventional names.
 */
#define X(REG_NUM, REG_NAME, STR) REG_NAME,
typedef enum reg_name_t
{
    REG_NUM_TABLE
} reg_name_t;
#undef X

/**
 * Returns the register from REG_NUM_TABLE as a string.
 */
#define X(REG_NUM, REG_NAME, STR) STR,
char *REG_NUM_STR(int key)
{
    char *_REG_NUM_STR[] = {REG_NUM_TABLE};
    return _REG_NUM_STR[key];
}
#undef X

/******************************************************************************
 *                           OPCODE INTERPRETATION                            *
 ******************************************************************************/

/**
 * R-type instructions are encoded by funct and have 0000000 opcode.
 */
#define R(NAME, NUM, STR, FUNC_PTR) NAME = NUM,
typedef enum R_t
{
    R_TYPE_TABLE
} R_t;
#undef R

/**
 * I-type instructions are encoded by opcode.
 */
#define I(NAME, NUM, STR, FUNC_PTR) NAME = NUM,
typedef enum I_t
{
    I_TYPE_TABLE
} I_t;
#undef I

/**
 * J-type instructions are encoded by opcode.
 */
#define J(NAME, NUM, STR, FUNC_PTR) NAME = NUM,
typedef enum J_t
{
    J_TYPE_TABLE
} J_t;
#undef J

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
 * Returns the R-type instruction from R_TYPE_TABLE as a string.
 */
#define R(NAME, NUM, STR, FUNC_PTR) [NAME] = STR,
char *R_STR(int key)
{
    char *_R_STR[] = {R_TYPE_TABLE};
    return _R_STR[key];
}
#undef R

/**
 * Returns the I-type instruction from I_TYPE_TABLE as a string.
 */
#define I(NAME, NUM, STR, FUNC_PTR) [NAME] = STR,
char *I_STR(int key)
{
    char *_I_STR[] = {I_TYPE_TABLE};
    return _I_STR[key];
}
#undef I

/**
 * Returns the J-type instruction from J_TYPE_TABLE as a string.
 */
#define J(NAME, NUM, STR, FUNC_PTR) [NAME] = STR,
char *J_STR(int key)
{
    char *_J_STR[] = {J_TYPE_TABLE};
    return _J_STR[key];
}
#undef J

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
    REGISTER *hi;                 // HI register
    REGISTER *lo;                 // LO register
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
    cpu->hi = init_reg(HI);
    cpu->lo = init_reg(LO);

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

typedef void *R_funct_ptr_t(CPU *, REGISTER *, REGISTER *, REGISTER *);
typedef void *I_funct_ptr_t(CPU *, REGISTER *, REGISTER *, int);
typedef void *J_funct_ptr_t(CPU *, REGISTER *);

/* R-type instructions */
void MIPS_add(CPU *cpu, REGISTER *d, REGISTER *s, REGISTER *t) { d->value = s->value + t->value; }
void MIPS_and(CPU *cpu, REGISTER *d, REGISTER *s, REGISTER *t) { d->value = s->value & t->value; }
void MIPS_addu(CPU *cpu, REGISTER *d, REGISTER *s, REGISTER *t) { d->value = s->value + t->value; }
void MIPS_break(CPU *cpu, REGISTER *d, REGISTER *s, REGISTER *t) { cpu->pc = d->value; }
void MIPS_div(CPU *cpu, REGISTER *d, REGISTER *s, REGISTER *t)
{
    cpu->hi = d->value % s->value;
    cpu->lo = d->value / s->value;
}
void MIPS_divu(CPU *cpu, REGISTER *d, REGISTER *s, REGISTER *t)
{
    cpu->hi = d->value % s->value;
    cpu->lo = d->value / s->value;
}
void MIPS_jalr(CPU *cpu, REGISTER *d, REGISTER *s, REGISTER *t)
{
    d->value = cpu->pc;
    cpu->pc = s->value;
}
void MIPS_jr(CPU *cpu, REGISTER *d, REGISTER *s, REGISTER *t) { cpu->pc = d->value; }
void MIPS_mfhi(CPU *cpu, REGISTER *d, REGISTER *s, REGISTER *t) { d->value = cpu->hi; }
void MIPS_mflo(CPU *cpu, REGISTER *d, REGISTER *s, REGISTER *t) { d->value = cpu->lo; }
void MIPS_mthi(CPU *cpu, REGISTER *d, REGISTER *s, REGISTER *t) { cpu->hi = s->value; }
void MIPS_mtlo(CPU *cpu, REGISTER *d, REGISTER *s, REGISTER *t) { cpu->lo = s->value; }
void MIPS_mul(CPU *cpu, REGISTER *d, REGISTER *s, REGISTER *t) { d->value = s->value * t->value; }
void MIPS_mult(CPU *cpu, REGISTER *d, REGISTER *s, REGISTER *t) { cpu->hi = cpu->lo = s->value * t->value; }
void MIPS_multu(CPU *cpu, REGISTER *d, REGISTER *s, REGISTER *t) { cpu->hi = cpu->lo = s->value * t->value; }
void MIPS_nor(CPU *cpu, REGISTER *d, REGISTER *s, REGISTER *t) { d->value = ~(s->value | t->value); }
void MIPS_or(CPU *cpu, REGISTER *d, REGISTER *s, REGISTER *t) { d->value = s->value | t->value; }
void MIPS_sll(CPU *cpu, REGISTER *d, REGISTER *s, int I) { d->value = s->value << I; }
void MIPS_sllv(CPU *cpu, REGISTER *d, REGISTER *s, REGISTER *t) { d->value = s->value << t->value; }
void MIPS_slt(CPU *cpu, REGISTER *d, REGISTER *s, REGISTER *t) { d->value = s->value < t->value ? 1 : 0; }
void MIPS_sltu(CPU *cpu, REGISTER *d, REGISTER *s, REGISTER *t) { d->value = s->value < t->value; }
void MIPS_sra(CPU *cpu, REGISTER *d, REGISTER *s, REGISTER *t) { d->value = s->value >> t->value; }
void MIPS_srav(CPU *cpu, REGISTER *d, REGISTER *s, REGISTER *t) { d->value = s->value >> t->value; }
void MIPS_srl(CPU *cpu, REGISTER *d, REGISTER *s, int I) { d->value = s->value >> I; }
void MIPS_srlv(CPU *cpu, REGISTER *d, REGISTER *s, REGISTER *t) { d->value = s->value >> t->value; }
void MIPS_sub(CPU *cpu, REGISTER *d, REGISTER *s, REGISTER *t) { d->value = s->value - t->value; }
void MIPS_subu(CPU *cpu, REGISTER *d, REGISTER *s, REGISTER *t) { d->value = s->value - t->value; }
void MIPS_syscall(CPU *cpu) { printf("%s", syscall(cpu)); }
void MIPS_xor(CPU *cpu, REGISTER *d, REGISTER *s, REGISTER *t) { d->value = s->value ^ t->value; }

/* I-type instructions */
void MIPS_addi(CPU *cpu, REGISTER *t, REGISTER *s, int I) { t->value = s->value + I; }
void MIPS_addiu(CPU *cpu, REGISTER *t, REGISTER *s, int I) { t->value = s->value + I; }
void MIPS_andi(CPU *cpu, REGISTER *t, REGISTER *s, int I) { t->value = s->value & I; }
void MIPS_beq(CPU *cpu, REGISTER *s, REGISTER *t, int I)
{
    if (s->value == t->value)
        cpu->pc += I;
}
void MIPS_bgez(CPU *cpu, REGISTER *d, REGISTER *s, int I)
{
    if (d->value >= 0)
        cpu->pc += I;
}
void MIPS_bgtz(CPU *cpu, REGISTER *d, REGISTER *s, int I)
{
    if (d->value > 0)
        cpu->pc += I;
}
void MIPS_blez(CPU *cpu, REGISTER *d, REGISTER *s, int I)
{
    if (d->value <= 0)
        cpu->pc += I;
}
void MIPS_bltz(CPU *cpu, REGISTER *d, REGISTER *s, int I)
{
    if (d->value < 0)
        cpu->pc += I;
}
void MIPS_bne(CPU *cpu, REGISTER *s, REGISTER *t, int I)
{
    if (s->value != t->value)
        cpu->pc += I;
}
void MIPS_lb(CPU *cpu, REGISTER *d, REGISTER *s, int I) { d->value = *(byte_t *)(s->value); }
void MIPS_lh(CPU *cpu, REGISTER *d, REGISTER *s, int I) { d->value = *(half_t *)(s->value); }
void MIPS_lui(CPU *cpu, REGISTER *t, int I) { t->value = I << 16; }
void MIPS_lw(CPU *cpu, REGISTER *d, REGISTER *s, int I) { d->value = *(word_t *)(s->value); }
void MIPS_ori(CPU *cpu, REGISTER *t, REGISTER *s, int I) { t->value = s->value | I; }
void MIPS_sb(CPU *cpu, REGISTER *d, REGISTER *s, int I) { *(byte_t *)s->value = d->value; }
void MIPS_slti(CPU *cpu, REGISTER *t, REGISTER *s, int I) { t->value = s->value < I; }
void MIPS_sh(CPU *cpu, REGISTER *d, REGISTER *s, int I) { *(half_t *)s->value = d->value; }
void MIPS_sw(CPU *cpu, REGISTER *d, REGISTER *s, int I) { *(word_t *)s->value = d->value; }
void MIPS_xori(CPU *cpu, REGISTER *d, REGISTER *s, int I) { d->value = s->value ^ I; }

/* J-type instructions */
void MIPS_j(CPU *cpu, REGISTER *d) {}
void MIPS_jal(CPU *cpu, REGISTER *d) {}

/* Syscall */
char *syscall(CPU *cpu)
{
    char *str = NULL;
    switch (cpu->reg[$v0]->value)
    {
    case 1:
        snprintf(str, 8, "%d", cpu->reg[$a0]->value);
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
        snprintf(str, 8, "%c", cpu->reg[$a0]->value);
        break;
    default:
        snprintf(str, 8, "Unknown system call: %d\n", cpu->reg[$v0]->value);
    }

    return str;
}

/**
 * Returns the function pointer to an R-type instruction from R_TYPE_TABLE
 * given a matching key.
 */
#define R(NAME, NUM, STR, FUNC_PTR) [NAME] = FUNC_PTR,
R_funct_ptr_t *execute_R_instr(int key)
{
    void *_R_FUNC[] = {R_TYPE_TABLE};
    return _R_FUNC[key];
}
#undef R

/**
 * Returns the function pointer to an I-type instruction from I_TYPE_TABLE
 * given a matching key.
 */
#define I(NAME, NUM, STR, FUNC_PTR) [NAME] = FUNC_PTR,
I_funct_ptr_t *execute_I_instr(int key)
{
    void *_I_FUNC[] = {I_TYPE_TABLE};
    return _I_FUNC[key];
}
#undef I

/**
 * Returns the function pointer to a J-type instruction from J_TYPE_TABLE
 * given a matching key.
 */
#define J(NAME, NUM, STR, FUNC_PTR) [NAME] = FUNC_PTR,
J_funct_ptr_t *execute_J_instr(int key)
{
    void *_J_FUNC[] = {J_TYPE_TABLE};
    return _J_FUNC[key];
}
#undef J

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
    char line[MAX_LINE], buffer[MAX_LINE];
    int buffer_inc = 0;
    for (int i = 0; fgets(line, sizeof(line), f); i++)
    {
        int opcode = (int)strtol(line, NULL, 16);

        if (is_R_FORMAT(opcode))
        {
            R_FORMAT instr = extract_R_FORMAT(opcode);
            if (instr.funct == SYSCALL)
            {
                printf("\t%d: %s\n", i, R_STR(instr.funct));
                buffer_inc += snprintf(buffer + buffer_inc, 8, "%s", syscall(cpu));
            }
            else
            {
                printf("\t%d: %s\t%s %s %s\n", i, R_STR(instr.funct), REG_NUM_STR(instr.rd), REG_NUM_STR(instr.rs), REG_NUM_STR(instr.rt));
                execute_R_instr(instr.funct)(cpu, cpu->reg[instr.rd], cpu->reg[instr.rs], cpu->reg[instr.rt]);
            }
        }
        else if (is_I_FORMAT(opcode))
        {
            I_FORMAT instr = extract_I_FORMAT(opcode);
            printf("\t%d: %s\t%s %s %d\n", i, I_STR(instr.op), REG_NUM_STR(instr.rt), REG_NUM_STR(instr.rs), instr.imm);
            execute_I_instr(instr.op)(cpu, cpu->reg[instr.rt], cpu->reg[instr.rs], instr.imm);
        }
        else if (is_J_FORMAT(opcode))
        {
            J_FORMAT instr = extract_J_FORMAT(opcode);
            printf("\t%d: %s\n", i, J_STR(instr.op));
            execute_J_instr(instr.op)(cpu, cpu->reg[instr.addr]);
        }
    }

    printf("Output\n");
    printf("%s", buffer);

    printf("Registers After Execution\n");
    for (int i = 0; i < NUM_REGISTERS; i++)
        if (cpu->reg[i]->value != 0)
            printf("%s = %d\n", REG_NUM_STR(cpu->reg[i]->name), cpu->reg[i]->value);

    free_CPU(cpu);
    free_RAM(ram);
    fclose(f);

    return EXIT_SUCCESS;
}
