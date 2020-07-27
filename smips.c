/**
 * Simple MIPS emulator called SMIPS for COMP1521 which interprets hexadecimal
 * encoded instructions.
 * 
 * SMIPS outputs:
 *  - the instruction to each instruction opcode
 *  - output produced by syscalls
 *  - register values when program terminates
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
 *  - https://uweb.engr.arizona.edu/~ece369/Resources/spim/MIPSReference.pdf
 * 
 * TODO:
 *  - branching functions
 *  - check if unsigned functions are correct
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NUM_REGISTERS 34
#define RAM_BUFFER 65536
#define MAX_LINE 128
#define SPACE 3

/******************************************************************************
 *                                X MACROS                                    *
 ******************************************************************************/

/* Registers */
#define REG_NUM_TABLE     \
    _X($0, $zero, "$0")   \
    _X($1, $at, "$1")     \
    _X($2, $v0, "$2")     \
    _X($3, $v1, "$3")     \
    _X($4, $a0, "$4")     \
    _X($5, $a1, "$5")     \
    _X($6, $a2, "$6")     \
    _X($7, $a3, "$7")     \
    _X($8, $t0, "$8")     \
    _X($9, $t1, "$9")     \
    _X($10, $t2, "$10")   \
    _X($11, $t3, "$11")   \
    _X($12, $t4, "$12")   \
    _X($13, $t5, "$13")   \
    _X($14, $t6, "$14")   \
    _X($15, $t7, "$15")   \
    _X($16, $s0, "$16")   \
    _X($17, $s1, "$17")   \
    _X($18, $s2, "$18")   \
    _X($19, $s3, "$19")   \
    _X($20, $s4, "$20")   \
    _X($21, $s5, "$21")   \
    _X($22, $s6, "$22")   \
    _X($23, $s7, "$23")   \
    _X($24, $t8, "$24")   \
    _X($25, $t9, "$25")   \
    _X($26, $k0, "$26")   \
    _X($27, $k1, "$27")   \
    _X($28, $gp, "$28")   \
    _X($29, $sp, "$29")   \
    _X($30, $fa, "$30")   \
    _X($31, $ra, "$31")   \
    _X(LO, Lo, "Lo")      \
    _X(HI, Hi, "Hi")      \
    _X($f0, _34, "$f0")   \
    _X($f1, _35, "$f1")   \
    _X($f2, _36, "$f2")   \
    _X($f4, _37, "$f4")   \
    _X($f5, _38, "$f5")   \
    _X($f6, _39, "$f6")   \
    _X($f7, _40, "$f7")   \
    _X($f8, _41, "$f8")   \
    _X($f9, _42, "$f9")   \
    _X($f10, _43, "$f10") \
    _X($f12, _44, "$f12") \
    _X($f13, _45, "$f13") \
    _X($f14, _46, "$f14") \
    _X($f16, _47, "$f16") \
    _X($f17, _48, "$f17") \
    _X($f18, _49, "$f18") \
    _X($f20, _50, "$f20") \
    _X($f21, _51, "$f21") \
    _X($f22, _52, "$f22") \
    _X($f23, _53, "$f23") \
    _X($f24, _54, "$f24") \
    _X($f25, _55, "$f25") \
    _X($f26, _56, "$f26") \
    _X($f27, _57, "$f27") \
    _X($f28, _58, "$f28") \
    _X($f29, _59, "$f29") \
    _X($f30, _60, "$f30")

/**
 * R-type instructions
 * R (NAME, FUNCT, STR, FUNC_PTR)
 */
#define R_TYPE_TABLE                         \
    _R(ADD, 0b100000, "add", MIPS_add)       \
    _R(ADDU, 0b100001, "addu", MIPS_addu)    \
    _R(AND, 0b100100, "and", MIPS_and)       \
    _R(BREAK, 0b001101, "break", MIPS_break) \
    _R(DIV, 0b011010, "div", MIPS_div)       \
    _R(DIVU, 0b011011, "divu", MIPS_divu)    \
    _R(JALR, 0b001001, "jalr", MIPS_jalr)    \
    _R(JR, 0b001000, "jr", MIPS_jr)          \
    _R(MFHI, 0b010000, "mfhi", MIPS_mfhi)    \
    _R(MFLO, 0b010010, "mflo", MIPS_mflo)    \
    _R(MTHI, 0b010001, "mthi", MIPS_mthi)    \
    _R(MTLO, 0b010011, "mtlo", MIPS_mtlo)    \
    _R(MULT, 0b011000, "mult", MIPS_mult)    \
    _R(MULTU, 0b011001, "multu", MIPS_multu) \
    _R(NOR, 0b100111, "nor", MIPS_nor)       \
    _R(OR, 0b100101, "or", MIPS_or)          \
    _R(SLL, 0b000000, "sll", MIPS_sll)       \
    _R(SLLV, 0b000100, "sllv", MIPS_sllv)    \
    _R(SLT, 0b101010, "slt", MIPS_slt)       \
    _R(SLTU, 0b101011, "sltu", MIPS_sltu)    \
    _R(SRA, 0b000011, "sra", MIPS_sra)       \
    _R(SRAV, 0b000111, "srav", MIPS_srav)    \
    _R(SRL, 0b000010, "srl", MIPS_srl)       \
    _R(SRLV, 0b000110, "srlv", MIPS_srlv)    \
    _R(SUB, 0b100010, "sub", MIPS_sub)       \
    _R(SUBU, 0b100011, "subu", MIPS_subu)    \
    _R(XOR, 0b100110, "xor", MIPS_xor)

/**
 * I-type instructions
 * I (NAME, OP, STR, FUNC_PTR)
 */
#define I_TYPE_TABLE                         \
    _I(ADDI, 0b001000, "addi", MIPS_addi)    \
    _I(ADDIU, 0b001001, "addiu", MIPS_addiu) \
    _I(ANDI, 0b001100, "andi", MIPS_andi)    \
    _I(BEQ, 0b000100, "beq", MIPS_beq)       \
    _I(BGEZ, 0b000001, "bgez", MIPS_bgez)    \
    _I(BGTZ, 0b000111, "bgtz", MIPS_bgtz)    \
    _I(BLEZ, 0b000110, "blez", MIPS_blez)    \
    /*I(BLTZ, 0b000001, "bltz", MIPS_bltz)*/ \
    _I(BNE, 0b000101, "bne", MIPS_bne)       \
    _I(LB, 0b100000, "lb", MIPS_lb)          \
    _I(LH, 0b100001, "lh", MIPS_lh)          \
    _I(LUI, 0b001111, "lui", MIPS_lui)       \
    _I(LW, 0b100011, "lw", MIPS_lw)          \
    _I(ORI, 0b001101, "ori", MIPS_ori)       \
    _I(SB, 0b101000, "sb", MIPS_sb)          \
    _I(SLTI, 0b001010, "slti", MIPS_slti)    \
    _I(SLTIU, 0b001011, "sltiu", MIPS_sltiu) \
    _I(SH, 0b101001, "sh", MIPS_sh)          \
    _I(SW, 0b101011, "sw", MIPS_sw)          \
    _I(XORI, 0b001110, "xori", MIPS_xori)

/**
 * J-type instructions
 * J (NAME, OP, STR, FUNC_PTR)
 */
#define J_TYPE_TABLE             \
    _J(J, 0b000010, "j", MIPS_j) \
    _J(JAL, 0b000011, "jal", MIPS_jal)

/**
 * Pseudo instructions
 * P (NAME, FUNCT, STR, FUNC_PTR)
 */
#define P_TYPE_TABLE                   \
    _P(MUL, 0b000010, "mul", MIPS_mul) \
    _P(SYSCALL, 0b001100, "syscall", MIPS_syscall)

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
 * MIPS has 32 registers numbers + Lo + Hi.
 */
#define _X(REG_NUM, REG_NAME, STR) REG_NUM,
typedef enum reg_num_t
{
    REG_NUM_TABLE
} reg_num_t;
#undef _X

/**
 * MIPS registers have conventional names.
 */
#define _X(REG_NUM, REG_NAME, STR) REG_NAME,
typedef enum reg_name_t
{
    REG_NUM_TABLE
} reg_name_t;
#undef _X

/**
 * Returns the register from REG_NUM_TABLE as a string.
 */
#define _X(REG_NUM, REG_NAME, STR) STR,
char *REG_NUM_STR(int key)
{
    char *_REG_NUM_STR[] = {REG_NUM_TABLE};
    return _REG_NUM_STR[key];
}
#undef _X

/******************************************************************************
 *                           OPCODE INTERPRETATION                            *
 ******************************************************************************/

/**
 * R-type instructions are encoded by funct and have 0000000 opcode.
 */
#define _R(NAME, FUNCT, STR, FUNC_PTR) NAME = FUNCT,
typedef enum R_t
{
    R_TYPE_TABLE
} R_t;
#undef _R

/**
 * I-type instructions are encoded by opcode.
 */
#define _I(NAME, OP, STR, FUNC_PTR) NAME = OP,
typedef enum I_t
{
    I_TYPE_TABLE
} I_t;
#undef _I

/**
 * J-type instructions are encoded by opcode.
 */
#define _J(NAME, OP, STR, FUNC_PTR) NAME = OP,
typedef enum J_t
{
    J_TYPE_TABLE
} J_t;
#undef _J

/**
 * Pseudo instructions are encoded by opcode.
 */
#define _P(NAME, FUNCT, STR, FUNC_PTR) NAME = FUNCT,
typedef enum P_t
{
    P_TYPE_TABLE
} P_t;
#undef _P

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
 * Check if opcode is in R format by comparing its op value against 0b000000.
 */
bool is_R_FORMAT(int opcode)
{
    R_FORMAT instr = extract_R_FORMAT(opcode);
    if (instr.op == 0b000000)
        return true;
    return false;
}

/**
 * Check if instruction is in J format by comparing its op value against known
 * J-instructions.
 */
bool is_J_FORMAT(int opcode)
{
    J_FORMAT instr = extract_J_FORMAT(opcode);
    if (instr.op == J || instr.op == JAL)
        return true;
    return false;
}

/**
 * Check if opcode is a pseudo instruction by comparing its funct value
 * against known pseudo instructions.
 */
bool is_P_FORMAT(int opcode)
{
    R_FORMAT instr = extract_R_FORMAT(opcode);
    if (instr.funct == MUL || instr.funct == SYSCALL)
        return true;
    return false;
}

/**
 * Check if instruction is in I format if it does not match any other
 * instruction criteria.
 */
bool is_I_FORMAT(int opcode)
{
    if (!is_R_FORMAT(opcode) && !is_J_FORMAT(opcode) && !is_P_FORMAT(opcode))
        return true;
    return false;
}

/**
 * Returns the R-type instruction from R_TYPE_TABLE as a string.
 */
#define _R(NAME, FUNCT, STR, FUNC_PTR) [NAME] = STR,
char *R_STR(int key)
{
    char *_R_STR[] = {R_TYPE_TABLE};
    return _R_STR[key];
}
#undef _R

/**
 * Returns the I-type instruction from I_TYPE_TABLE as a string.
 */
#define _I(NAME, OP, STR, FUNC_PTR) [NAME] = STR,
char *I_STR(int key)
{
    char *_I_STR[] = {I_TYPE_TABLE};
    return _I_STR[key];
}
#undef _I

/**
 * Returns the J-type instruction from J_TYPE_TABLE as a string.
 */
#define _J(NAME, OP, STR, FUNC_PTR) [NAME] = STR,
char *J_STR(int key)
{
    char *_J_STR[] = {J_TYPE_TABLE};
    return _J_STR[key];
}
#undef _J

/**
 * Returns the pseudo instruction from P_TYPE_TABLE as a string.
 */
#define _P(NAME, FUNCT, STR, FUNC_PTR) [NAME] = STR,
char *P_STR(int key)
{
    char *_P_STR[] = {P_TYPE_TABLE};
    return _P_STR[key];
}
#undef _P

/******************************************************************************
 *                               HARDWARE                                     *
 ******************************************************************************/

typedef __int32_t reg_t; // Size of a MIPS register is 32 bits.

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

typedef void *R_funct_ptr_t(CPU *, REGISTER *rs, REGISTER *rt, REGISTER *rd, int shamt, int funct);
typedef void *I_funct_ptr_t(CPU *, REGISTER *rs, REGISTER *rt, int imm);
typedef void *J_funct_ptr_t(CPU *, REGISTER *addr);

/* Syscall */
char *syscall(CPU *cpu)
{
    char *str;
    switch (cpu->reg[$v0]->value)
    {
    case 1:
        sprintf(str, "%d", cpu->reg[$a0]->value);
        break;
    // case 2:
    //     sprintf(str, "%f", cpu->reg[$f12]->value);
    //     break;
    // case 3:
    //     sprintf(str, "%lf", cpu->reg[$f12]->value);
    //     break;
    // case 4:
    //     sprintf(str, "%s", cpu->reg[$a0]->value);
    //     break;
    case 5:
        scanf("%d", &(cpu->reg[$v0]->value));
        break;
    // case 6:
    //     scanf("%f", &(cpu->reg[$v0]->value));
    //     break;
    // case 7:
    //     scanf("%lf", &(cpu->reg[$v0]->value));
    //     break;
    // case 8:
    //     fgets(cpu->reg[$a0]->value, cpu->reg[$a1]->value, stdin);
    //     break;
    // case 9:
    //     realloc(cpu->reg[$v0]->value, cpu->reg[$a0]->value);
    //     break;
    case 10:
        exit(EXIT_SUCCESS);
        break;
    case 11:
        sprintf(str, "%c", cpu->reg[$a0]->value);
        break;
    default:
        sprintf(str, "Unknown system call: %d\n", cpu->reg[$v0]->value);
    }

    return str;
}

/* R-type instructions */
void MIPS_add(CPU *cpu, REGISTER *rs, REGISTER *rt, REGISTER *rd, int shamt, int funct) { rd->value = rs->value + rt->value; }
void MIPS_and(CPU *cpu, REGISTER *rs, REGISTER *rt, REGISTER *rd, int shamt, int funct) { rd->value = rs->value & rt->value; }
void MIPS_addu(CPU *cpu, REGISTER *rs, REGISTER *rt, REGISTER *rd, int shamt, int funct) { rd->value = rs->value + rt->value; }
void MIPS_break(CPU *cpu, REGISTER *rs, REGISTER *rt, REGISTER *rd, int shamt, int funct) { cpu->pc = rd->value; }
void MIPS_div(CPU *cpu, REGISTER *rs, REGISTER *rt, REGISTER *rd, int shamt, int funct)
{
    cpu->reg[HI]->value = rs->value % rt->value;
    cpu->reg[LO]->value = rs->value / rt->value;
}
void MIPS_divu(CPU *cpu, REGISTER *rs, REGISTER *rt, REGISTER *rd)
{
    cpu->reg[HI]->value = rs->value % rt->value;
    cpu->reg[LO]->value = rs->value / rt->value;
}
void MIPS_jalr(CPU *cpu, REGISTER *rs, REGISTER *rt, REGISTER *rd)
{
    rd->value = cpu->pc;
    cpu->pc = rs->value;
}
void MIPS_jr(CPU *cpu, REGISTER *rs, REGISTER *rt, REGISTER *rd, int shamt, int funct) { cpu->pc = rs->value; }
void MIPS_mfhi(CPU *cpu, REGISTER *rs, REGISTER *rt, REGISTER *rd, int shamt, int funct) { rd->value = cpu->reg[HI]->value; }
void MIPS_mflo(CPU *cpu, REGISTER *rs, REGISTER *rt, REGISTER *rd, int shamt, int funct) { rd->value = cpu->reg[LO]->value; }
void MIPS_mthi(CPU *cpu, REGISTER *rs, REGISTER *rt, REGISTER *rd, int shamt, int funct) { cpu->reg[HI]->value = rd->value; }
void MIPS_mtlo(CPU *cpu, REGISTER *rs, REGISTER *rt, REGISTER *rd, int shamt, int funct) { cpu->reg[LO]->value = rd->value; }
void MIPS_mult(CPU *cpu, REGISTER *rs, REGISTER *rt, REGISTER *rd, int shamt, int funct) { cpu->reg[HI]->value = cpu->reg[LO]->value = rs->value * rt->value; }
void MIPS_multu(CPU *cpu, REGISTER *rs, REGISTER *rt, REGISTER *rd, int shamt, int funct) { cpu->reg[HI]->value = cpu->reg[LO]->value = rs->value * rt->value; }
void MIPS_nor(CPU *cpu, REGISTER *rs, REGISTER *rt, REGISTER *rd, int shamt, int funct) { rd->value = ~(rs->value | rt->value); }
void MIPS_or(CPU *cpu, REGISTER *rs, REGISTER *rt, REGISTER *rd, int shamt, int funct) { rd->value = rs->value | rt->value; }
void MIPS_sll(CPU *cpu, REGISTER *rs, REGISTER *rt, REGISTER *rd, int shamt, int funct) { rd->value = rt->value << shamt; }
void MIPS_sllv(CPU *cpu, REGISTER *rs, REGISTER *rt, REGISTER *rd, int shamt, int funct) { rd->value = rt->value << rs->value; }
void MIPS_slt(CPU *cpu, REGISTER *rs, REGISTER *rt, REGISTER *rd, int shamt, int funct) { rd->value = rs->value < rt->value ? 1 : 0; }
void MIPS_sltu(CPU *cpu, REGISTER *rs, REGISTER *rt, REGISTER *rd, int shamt, int funct) { rd->value = rs->value < rt->value ? 1 : 0; }
void MIPS_sra(CPU *cpu, REGISTER *rs, REGISTER *rt, REGISTER *rd, int shamt, int funct) { rd->value = rt->value >> shamt; }
void MIPS_srav(CPU *cpu, REGISTER *rs, REGISTER *rt, REGISTER *rd, int shamt, int funct) { rd->value = rt->value >> rs->value; }
void MIPS_srl(CPU *cpu, REGISTER *rs, REGISTER *rt, REGISTER *rd, unsigned int shamt, int funct) { rd->value = rs->value >> shamt; }
void MIPS_srlv(CPU *cpu, REGISTER *rs, REGISTER *rt, REGISTER *rd, unsigned int shamt, int funct) { rd->value = rt->value >> rs->value; }
void MIPS_sub(CPU *cpu, REGISTER *rs, REGISTER *rt, REGISTER *rd, int shamt, int funct) { rd->value = rs->value - rt->value; }
void MIPS_subu(CPU *cpu, REGISTER *rs, REGISTER *rt, REGISTER *rd, int shamt, int funct) { rd->value = rs->value - rt->value; }
void MIPS_xor(CPU *cpu, REGISTER *rs, REGISTER *rt, REGISTER *rd) { rd->value = rs->value ^ rt->value; }

/* I-type instructions */
void MIPS_addi(CPU *cpu, REGISTER *rs, REGISTER *rt, int imm) { rt->value = rs->value + imm; }
void MIPS_addiu(CPU *cpu, REGISTER *rs, REGISTER *rt, unsigned int imm) { rt->value = rs->value + imm; }
void MIPS_andi(CPU *cpu, REGISTER *rs, REGISTER *rt, int imm) { rt->value = rs->value & imm; }
void MIPS_beq(CPU *cpu, REGISTER *rs, REGISTER *rt, int imm)
{
    if (rs->value == rt->value)
        cpu->pc += imm;
}
void MIPS_bgez(CPU *cpu, REGISTER *rs, REGISTER *rt, int imm)
{
    if (rs->value >= 0)
        cpu->pc += imm;
}
void MIPS_bgtz(CPU *cpu, REGISTER *rs, REGISTER *rt, int imm)
{
    if (rs->value > 0)
        cpu->pc += imm;
}
void MIPS_blez(CPU *cpu, REGISTER *rs, REGISTER *rt, int imm)
{
    if (rs->value <= 0)
        cpu->pc += imm;
}
void MIPS_bltz(CPU *cpu, REGISTER *rs, REGISTER *rt, int imm)
{
    if (rs->value < 0)
        cpu->pc += imm;
}
void MIPS_bne(CPU *cpu, REGISTER *rs, REGISTER *rt, int imm)
{
    if (rs->value != rt->value)
        cpu->pc += imm;
}
void MIPS_lb(CPU *cpu, REGISTER *rs, REGISTER *rt, int imm) { rt->value = rs->value; }
void MIPS_lh(CPU *cpu, REGISTER *rs, REGISTER *rt, int imm) { rt->value = rs->value; }
void MIPS_lui(CPU *cpu, REGISTER *rs, REGISTER *rt, int imm) { rt->value = imm << 16U; }
void MIPS_lw(CPU *cpu, REGISTER *rs, REGISTER *rt, int imm) { rt->value = rs->value; }
void MIPS_ori(CPU *cpu, REGISTER *rs, REGISTER *rt, int imm) { rt->value = rs->value | imm; }
void MIPS_sb(CPU *cpu, REGISTER *rs, REGISTER *rt, int imm) { rs->value = rt->value; }
void MIPS_slti(CPU *cpu, REGISTER *rs, REGISTER *rt, int imm) { rt->value = rs->value < imm ? 1 : 0; }
void MIPS_sltiu(CPU *cpu, REGISTER *rs, REGISTER *rt, unsigned int imm) { rt->value = rs->value < imm ? 1 : 0; }
void MIPS_sh(CPU *cpu, REGISTER *rs, REGISTER *rt, int imm) { rs->value = rt->value; }
void MIPS_sw(CPU *cpu, REGISTER *rs, REGISTER *rt, int imm) { rs->value = rt->value; }
void MIPS_xori(CPU *cpu, REGISTER *rs, REGISTER *rt, int imm) { rt->value = rs->value ^ imm; }

/* J-type instructions */
void MIPS_j(CPU *cpu, REGISTER *addr) {}
void MIPS_jal(CPU *cpu, REGISTER *addr) {}

/* Pseudo instructions */
void MIPS_mul(CPU *cpu, REGISTER *rs, REGISTER *rt, REGISTER *rd, int shamt, int funct)
{
    // rd->value = rs->value * rt->value;
    MIPS_mult(cpu, rs, rt, rd, shamt, funct);
    MIPS_mflo(cpu, rs, rt, rd, shamt, funct);
}
void MIPS_syscall(CPU *cpu) { printf("%s", syscall(cpu)); }

/**
 * Returns the function pointer to an R-type instruction from R_TYPE_TABLE
 * given a matching key.
 */
#define _R(NAME, FUNCT, STR, FUNC_PTR) [NAME] = FUNC_PTR,
R_funct_ptr_t *execute_R_instr(int key)
{
    void *_R_FUNC[] = {R_TYPE_TABLE};
    return _R_FUNC[key];
}
#undef _R

/**
 * Returns the function pointer to an I-type instruction from I_TYPE_TABLE
 * given a matching key.
 */
#define _I(NAME, OP, STR, FUNC_PTR) [NAME] = FUNC_PTR,
I_funct_ptr_t *execute_I_instr(int key)
{
    void *_I_FUNC[] = {I_TYPE_TABLE};
    return _I_FUNC[key];
}
#undef _I

/**
 * Returns the function pointer to a J-type instruction from J_TYPE_TABLE
 * given a matching key.
 */
#define _J(NAME, OP, STR, FUNC_PTR) [NAME] = FUNC_PTR,
J_funct_ptr_t *execute_J_instr(int key)
{
    void *_J_FUNC[] = {J_TYPE_TABLE};
    return _J_FUNC[key];
}
#undef _J

/**
 * Returns the function pointer to a pseudo instruction from P_TYPE_TABLE
 * given a matching key.
 */
#define _P(NAME, FUNCT, STR, FUNC_PTR) [NAME] = FUNC_PTR,
R_funct_ptr_t *execute_P_instr(int key)
{
    void *_P_FUNC[] = {P_TYPE_TABLE};
    return _P_FUNC[key];
}
#undef _P

/******************************************************************************
 *                                  MAIN                                      *
 ******************************************************************************/

/**
 * Print bits from MSB to LSB.
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
 * Print non-zero registers.
 */
void print_registers(CPU *cpu)
{
    for (int i = 0; i < NUM_REGISTERS; i++)
        if (cpu->reg[i]->value != 0)
            printf("%s%*c= %d\n", REG_NUM_STR(cpu->reg[i]->name), 2, ' ', cpu->reg[i]->value);
}

/**
 * Parser.
 */
void parser(FILE *f, CPU *cpu, char *buffer)
{
    char line[MAX_LINE];
    for (int i = 0, buffer_inc = 0; fgets(line, sizeof(line), f); i++)
    {
        int opcode = (int)strtol(line, NULL, 16);

        printf("%*d: ", SPACE, i);

        if (is_P_FORMAT(opcode))
        {
            R_FORMAT instr = extract_R_FORMAT(opcode);
            if (instr.funct == SYSCALL)
            {
                printf("%s", P_STR(instr.funct));
                buffer_inc += snprintf(buffer + buffer_inc, MAX_LINE, "%s", syscall(cpu));
            }
            else
            {
                printf("%s %*s, %s, %s", P_STR(instr.funct), SPACE, REG_NUM_STR(instr.rd), REG_NUM_STR(instr.rs), REG_NUM_STR(instr.rt));
                execute_P_instr(instr.funct)(cpu, cpu->reg[instr.rs], cpu->reg[instr.rt], cpu->reg[instr.rd], instr.shamt, instr.funct);
            }
        }
        else if (is_R_FORMAT(opcode))
        {
            R_FORMAT instr = extract_R_FORMAT(opcode);
            printf("%s %*s, %s, %s", R_STR(instr.funct), SPACE, REG_NUM_STR(instr.rd), REG_NUM_STR(instr.rs), REG_NUM_STR(instr.rt));
            execute_R_instr(instr.funct)(cpu, cpu->reg[instr.rs], cpu->reg[instr.rt], cpu->reg[instr.rd], instr.shamt, instr.funct);
        }
        else if (is_I_FORMAT(opcode))
        {
            I_FORMAT instr = extract_I_FORMAT(opcode);
            printf("%s %*s, %s, %d", I_STR(instr.op), SPACE, REG_NUM_STR(instr.rt), REG_NUM_STR(instr.rs), instr.imm);
            execute_I_instr(instr.op)(cpu, cpu->reg[instr.rs], cpu->reg[instr.rt], instr.imm);
        }
        else if (is_J_FORMAT(opcode))
        {
            J_FORMAT instr = extract_J_FORMAT(opcode);
            printf("%s", J_STR(instr.op));
            execute_J_instr(instr.op)(cpu, cpu->reg[instr.addr]);
        }

        printf("\n");
    }
}

/**
 * Main.
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

    printf("Program\n");
    char buffer[MAX_LINE];
    parser(f, cpu, buffer);

    printf("Output\n");
    fprintf(stdout, "%s", buffer);

    printf("Registers After Execution\n");
    print_registers(cpu);

    free_CPU(cpu);
    fclose(f);

    return EXIT_SUCCESS;
}
