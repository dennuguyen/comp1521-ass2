/**
 * @author Dan Nguyen (z5206032)
 * 
 * Simple MIPS emulator called SMIPS for COMP1521 which interprets hexadecimal
 * encoded instructions.
 * 
 * The design choices made in this assignment are intended for future scale-up
 * for more tools and features.
 * 
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
 * These two parses can be combined into a single parse called 'backpatching'.
 * 
 * 
 * SMIPS outputs:
 *  1. the instruction to each instruction opcode
 *  2. output produced by syscalls
 *  3. register values when program terminates
 * 
 * Instruction format: 000000 00000 00000 00000 00000 000000
 * Resources:
 *  - http://max.cs.kzoo.edu/cs230/Resources/MIPS/MachineXL/InstructionFormats.html instruction formatting
 *  - https://opencores.org/projects/plasma/opcodes opcodes
 *  - https://www.slideshare.net/tagbagtroj/mips-opcodes MIPS opcodes
 *  - http://www.drdobbs.com/the-new-c-x-macros/184401387 X macros
 *  - https://uweb.engr.arizona.edu/~ece369/Resources/spim/MIPSReference.pdf MIPS reference
 *  - https://web.stanford.edu/class/cs143/materials/SPIM_Manual.pdf SPIM manual
 *  - https://www.doc.ic.ac.uk/lab/secondyear/spim/node20.html floating point instructions
 * 
 * 
 * @todo
 *  - branching functions
 *  - check if unsigned functions are correct
 *  - error handling
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define NUM_REGISTERS 60
#define BUFFER 4096
#define MAX_INSTRUCTIONS 1000
#define MAX_MEMORY 65536

/******************************************************************************
 *                                X MACROS                                    *
 ******************************************************************************/

/**
 * @def REGISTER_TABLE
 * @brief X macro for registers to store enumerated number, enumerated name
 * and name as string.
 * 
 * @param REG_NUM Number of register
 * @param REG_NAME Name of register as enum
 * @param STR Name of register as string
 */
#define REGISTER_TABLE    \
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
 * @def R_TYPE_TABLE
 * @brief X macro for R-type instructions to store its enumerated name, funct
 * code, name as string, function pointer.
 * 
 * @param NAME Name of instruction as enum
 * @param FUNCT Funct value of instruction
 * @param STR Name of instruction as string
 * @param FUNC_PTR Function pointer to instruction
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
 * @def I_TYPE_TABLE
 * @brief X macro for I-type instructions to store its enumerated name, funct
 * code, name as string, function pointer.
 * 
 * @param NAME Name of instruction as enum
 * @param OP Op value of instruction
 * @param STR Name of instruction as string
 * @param FUNC_PTR Function pointer to instruction
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
 * @def J_TYPE_TABLE
 * @brief X macro for J-type instructions to store its enumerated name, funct
 * code, name as string, function pointer.
 * 
 * @param NAME Name of instruction as enum
 * @param OP Op value of instruction
 * @param STR Name of instruction as string
 * @param FUNC_PTR Function pointer to instruction
 */
#define J_TYPE_TABLE             \
    _J(J, 0b000010, "j", MIPS_j) \
    _J(JAL, 0b000011, "jal", MIPS_jal)

/**
 * @def P_TYPE_TABLE
 * @brief X macro for P-type instructions to store its enumerated name, funct
 * code, name as string, function pointer.
 * 
 * @param NAME Name of instruction as enum
 * @param FUNCT Funct value of instruction
 * @param STR Name of instruction as string
 * @param FUNC_PTR Function pointer to instruction
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

#define _X(REG_NUM, REG_NAME, STR) REG_NUM,
/**
 * @enum reg_num_t
 * @brief Extract `REG_NUM` from `REGISTER_TABLE` and fill enumerated type
 * `reg_num_t`.
 */
typedef enum reg_num_t
{
    REGISTER_TABLE
} reg_num_t;
#undef _X

#define _X(REG_NUM, REG_NAME, STR) REG_NAME,
/**
 * @enum reg_num_t
 * @brief Extract `REG_NAME` from `REGISTER_TABLE` and fill enumerated type
 * `reg_num_t`.
 */
typedef enum reg_name_t
{
    REGISTER_TABLE
} reg_name_t;
#undef _X

#define _X(REG_NUM, REG_NAME, STR) STR,
/**
 * @brief Returns the register from `REGISTER_TABLE` as a string.
 * 
 * @param key Key is an enumerated type
 */
char *REG_NUM_STR(int key)
{
    char *_REG_NUM_STR[] = {REGISTER_TABLE};
    return _REG_NUM_STR[key];
}
#undef _X

/******************************************************************************
 *                           OPCODE INTERPRETATION                            *
 ******************************************************************************/

#define _R(NAME, FUNCT, STR, FUNC_PTR) NAME = FUNCT,
/**
 * @enum R_t
 * @brief Enumerate `NAME` by its `FUNCT` value from `R_TYPE_TABLE`.
 */
typedef enum R_t
{
    R_TYPE_TABLE
} R_t;
#undef _R

#define _I(NAME, OP, STR, FUNC_PTR) NAME = OP,
/**
 * @enum I_t
 * @brief Enumerate `NAME` by its `OP` value from `I_TYPE_TABLE`.
 */
typedef enum I_t
{
    I_TYPE_TABLE
} I_t;
#undef _I

#define _J(NAME, OP, STR, FUNC_PTR) NAME = OP,
/**
 * @enum J_t
 * @brief Enumerate `NAME` by its `OP` value from `J_TYPE_TABLE`.
 */
typedef enum J_t
{
    J_TYPE_TABLE
} J_t;
#undef _J

#define _P(NAME, FUNCT, STR, FUNC_PTR) NAME = FUNCT,
/**
 * @enum P_t
 * @brief Enumerate `NAME` by its `FUNCT` value from `P_TYPE_TABLE`.
 */
typedef enum P_t
{
    P_TYPE_TABLE
} P_t;
#undef _P

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

/**
 * @brief Extracts the bit fields from an encoded instruction for an R-type
 * instruction.
 * 
 * @param instr_code Encoded MIPS instruction
 * @return R_FORMAT
 */
R_FORMAT extract_R_FORMAT(int instr_code)
{
    R_FORMAT instr; //< dadas
    instr.op = (instr_code >> 26);
    instr.rs = ((instr_code >> 21) & 0x1F);
    instr.rt = ((instr_code >> 16) & 0x1F);
    instr.rd = ((instr_code >> 11) & 0x1F);
    instr.shamt = ((instr_code >> 6) & 0x1F);
    instr.funct = (instr_code & 0x3F);
    return instr;
}

/**
 * @brief Extracts the bit fields from an encoded instruction for an I-type
 * instruction.
 * 
 * @param instr_code Encoded MIPS instruction
 * @return I_FORMAT
 */
I_FORMAT extract_I_FORMAT(int instr_code)
{
    I_FORMAT instr;
    instr.op = (instr_code >> 26);
    instr.rs = ((instr_code >> 21) & 0x1F);
    instr.rt = ((instr_code >> 16) & 0x1F);
    instr.imm = (instr_code & 0xFFFF);
    return instr;
}

/**
 * @brief Extracts the bit fields from an encoded instruction for a J-type
 * instruction.
 *
 * @param instr_code Encoded MIPS instruction
 * @return J_FORMAT 
 */
J_FORMAT extract_J_FORMAT(int instr_code)
{
    J_FORMAT instr;
    instr.op = (instr_code >> 26);
    instr.addr = (instr_code & 0x3FFFFFF);
    return instr;
}

/**
 * @brief Check if opcode is in R format by comparing its op value against
 * 0b000000.
 * 
 * @param instr_code Encoded MIPS instruction
 * @return true 
 * @return false 
 */
bool is_R_FORMAT(int instr_code)
{
    R_FORMAT instr = extract_R_FORMAT(instr_code);
    if (instr.op == 0b000000)
        return true;
    return false;
}

/**
 * @brief Check if instruction is in J format by comparing its op value against
 * known J-instructions.
 * 
 * @param instr_code Encoded MIPS instruction
 * @return true 
 * @return false 
 */
bool is_J_FORMAT(int instr_code)
{
    J_FORMAT instr = extract_J_FORMAT(instr_code);
    if (instr.op == J || instr.op == JAL)
        return true;
    return false;
}

/**
 * @brief Check if opcode is a pseudo instruction by comparing its funct value
 * against known pseudo instructions.
 * 
 * @param instr_code Encoded MIPS instruction
 * @return true 
 * @return false 
 */
bool is_P_FORMAT(int instr_code)
{
    R_FORMAT instr = extract_R_FORMAT(instr_code);
    if (instr.funct == MUL || instr.funct == SYSCALL)
        return true;
    return false;
}

/**
 * @brief Check if instruction is in I format if it does not match any other
 * instruction criteria.
 * 
 * @param instr_code Encoded MIPS instruction
 * @return true 
 * @return false 
 */
bool is_I_FORMAT(int instr_code)
{
    if (!is_R_FORMAT(instr_code) &&
        !is_J_FORMAT(instr_code) &&
        !is_P_FORMAT(instr_code))
        return true;
    return false;
}

#define _R(NAME, FUNCT, STR, FUNC_PTR) [NAME] = STR,
/**
 * @brief Returns the R-type instruction from `R_TYPE_TABLE` as a string.
 * 
 * @param key Key is an enumerated type
 * @return char* 
 */
char *R_STR(int key)
{
    char *_R_STR[] = {R_TYPE_TABLE};
    return _R_STR[key];
}
#undef _R

#define _I(NAME, OP, STR, FUNC_PTR) [NAME] = STR,
/**
 * @brief Returns the I-type instruction from `I_TYPE_TABLE` as a string.
 * 
 * @param key 
 * @return char* 
 */
char *I_STR(int key)
{
    char *_I_STR[] = {I_TYPE_TABLE};
    return _I_STR[key];
}
#undef _I

#define _J(NAME, OP, STR, FUNC_PTR) [NAME] = STR,
/**
 * @brief Returns the J-type instruction from `J_TYPE_TABLE` as a string.
 * 
 * @param key 
 * @return char* 
 */
char *J_STR(int key)
{
    char *_J_STR[] = {J_TYPE_TABLE};
    return _J_STR[key];
}
#undef _J

#define _P(NAME, FUNCT, STR, FUNC_PTR) [NAME] = STR,
/**
 * @brief Returns the pseudo instruction from `P_TYPE_TABLE` as a string.
 * 
 * @param key 
 * @return char* 
 */
char *P_STR(int key)
{
    char *_P_STR[] = {P_TYPE_TABLE};
    return _P_STR[key];
}
#undef _P

/******************************************************************************
 *                               HARDWARE                                     *
 ******************************************************************************/

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
 * @brief A MIPS CPU has a program counter, registers and memory.
 */
typedef struct CPU
{
    unsigned int pc;              // Program Counter
    REGISTER *reg[NUM_REGISTERS]; // Array of CPU registers
    int memory[MAX_INSTRUCTIONS]; // Memory to store programs
} CPU;

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

    for (int i = 0; i < MAX_INSTRUCTIONS; i++)
        cpu->memory[i] = 0;

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

/******************************************************************************
 *                              MIPS FUNCTIONS                                *
 ******************************************************************************/

typedef void *R_funct_ptr_t(CPU *, REGISTER *rs, REGISTER *rt, REGISTER *rd, int shamt, int funct);
typedef void *I_funct_ptr_t(CPU *, REGISTER *rs, REGISTER *rt, int imm);
typedef void *J_funct_ptr_t(CPU *, REGISTER *addr);

void MIPS_add(CPU *cpu, REGISTER *rs, REGISTER *rt, REGISTER *rd, int shamt, int funct)
{
    rd->value.wd = rs->value.wd + rt->value.wd;
}

void MIPS_addi(CPU *cpu, REGISTER *rs, REGISTER *rt, int imm)
{
    rt->value.wd = rs->value.wd + imm;
}

void MIPS_addiu(CPU *cpu, REGISTER *rs, REGISTER *rt, unsigned int imm)
{
    rt->value.wd = rs->value.wd + imm;
}

void MIPS_and(CPU *cpu, REGISTER *rs, REGISTER *rt, REGISTER *rd, int shamt, int funct)
{
    rd->value.wd = rs->value.wd & rt->value.wd;
}

void MIPS_andi(CPU *cpu, REGISTER *rs, REGISTER *rt, int imm)
{
    rt->value.wd = rs->value.wd & imm;
}

void MIPS_addu(CPU *cpu, REGISTER *rs, REGISTER *rt, REGISTER *rd, int shamt, int funct)
{
    rd->value.wd = rs->value.wd + rt->value.wd;
}

void MIPS_beq(CPU *cpu, REGISTER *rs, REGISTER *rt, int imm)
{
    if (rs->value.wd == rt->value.wd)
        cpu->pc += imm;
}

void MIPS_bgez(CPU *cpu, REGISTER *rs, REGISTER *rt, int imm)
{
    if (rs->value.wd >= 0)
        cpu->pc += imm;
}

void MIPS_bgtz(CPU *cpu, REGISTER *rs, REGISTER *rt, int imm)
{
    if (rs->value.wd > 0)
        cpu->pc += imm;
}

void MIPS_blez(CPU *cpu, REGISTER *rs, REGISTER *rt, int imm)
{
    if (rs->value.wd <= 0)
        cpu->pc += imm;
}

void MIPS_bltz(CPU *cpu, REGISTER *rs, REGISTER *rt, int imm)
{
    if (rs->value.wd < 0)
        cpu->pc += imm;
}

void MIPS_bne(CPU *cpu, REGISTER *rs, REGISTER *rt, int imm)
{
    if (rs->value.wd != rt->value.wd)
        cpu->pc += imm;
}

void MIPS_break(CPU *cpu, REGISTER *rs, REGISTER *rt, REGISTER *rd, int shamt, int funct)
{
    cpu->pc = rd->value.wd;
}

void MIPS_div(CPU *cpu, REGISTER *rs, REGISTER *rt, REGISTER *rd, int shamt, int funct)
{
    cpu->reg[HI]->value.wd = rs->value.wd % rt->value.wd;
    cpu->reg[LO]->value.wd = rs->value.wd / rt->value.wd;
}

void MIPS_divu(CPU *cpu, REGISTER *rs, REGISTER *rt, REGISTER *rd)
{
    cpu->reg[HI]->value.wd = rs->value.wd % rt->value.wd;
    cpu->reg[LO]->value.wd = rs->value.wd / rt->value.wd;
}

void MIPS_j(CPU *cpu, REGISTER *addr) {} // TODO:

void MIPS_jal(CPU *cpu, REGISTER *addr) {} // TODO:

void MIPS_jalr(CPU *cpu, REGISTER *rs, REGISTER *rt, REGISTER *rd)
{
    rd->value.wd = cpu->pc;
    cpu->pc = rs->value.wd;
}

void MIPS_jr(CPU *cpu, REGISTER *rs, REGISTER *rt, REGISTER *rd, int shamt, int funct)
{
    cpu->pc = rs->value.wd;
}

void MIPS_lb(CPU *cpu, REGISTER *rs, REGISTER *rt, int imm)
{
    rt->value.wd = rs->value.wd;
}

void MIPS_lh(CPU *cpu, REGISTER *rs, REGISTER *rt, int imm)
{
    rt->value.wd = rs->value.wd;
}

void MIPS_lui(CPU *cpu, REGISTER *rs, REGISTER *rt, int imm)
{
    rt->value.wd = imm << 16U;
}

void MIPS_lw(CPU *cpu, REGISTER *rs, REGISTER *rt, int imm)
{
    rt->value.wd = rs->value.wd;
}

void MIPS_mfhi(CPU *cpu, REGISTER *rs, REGISTER *rt, REGISTER *rd, int shamt, int funct)
{
    rd->value.wd = cpu->reg[HI]->value.wd;
}

void MIPS_mflo(CPU *cpu, REGISTER *rs, REGISTER *rt, REGISTER *rd, int shamt, int funct)
{
    rd->value.wd = cpu->reg[LO]->value.wd;
}

void MIPS_mthi(CPU *cpu, REGISTER *rs, REGISTER *rt, REGISTER *rd, int shamt, int funct)
{
    cpu->reg[HI]->value.wd = rd->value.wd;
}

void MIPS_mtlo(CPU *cpu, REGISTER *rs, REGISTER *rt, REGISTER *rd, int shamt, int funct)
{
    cpu->reg[LO]->value.wd = rd->value.wd;
}

void MIPS_mult(CPU *cpu, REGISTER *rs, REGISTER *rt, REGISTER *rd, int shamt, int funct)
{
    cpu->reg[HI]->value.wd = cpu->reg[LO]->value.wd = rs->value.wd * rt->value.wd;
}

void MIPS_multu(CPU *cpu, REGISTER *rs, REGISTER *rt, REGISTER *rd, int shamt, int funct)
{
    cpu->reg[HI]->value.wd = cpu->reg[LO]->value.wd = rs->value.wd * rt->value.wd;
}

// TODO: prototype def
void MIPS_mul(CPU *cpu, REGISTER *rs, REGISTER *rt, REGISTER *rd, int shamt, int funct)
{
    // rd->value.wd = rs->value.wd * rt->value.wd;
    MIPS_mult(cpu, rs, rt, rd, shamt, funct);
    MIPS_mflo(cpu, rs, rt, rd, shamt, funct);
}

void MIPS_nor(CPU *cpu, REGISTER *rs, REGISTER *rt, REGISTER *rd, int shamt, int funct)
{
    rd->value.wd = ~(rs->value.wd | rt->value.wd);
}

void MIPS_or(CPU *cpu, REGISTER *rs, REGISTER *rt, REGISTER *rd, int shamt, int funct)
{
    rd->value.wd = rs->value.wd | rt->value.wd;
}

void MIPS_ori(CPU *cpu, REGISTER *rs, REGISTER *rt, int imm)
{
    rt->value.wd = rs->value.wd | imm;
}

void MIPS_sb(CPU *cpu, REGISTER *rs, REGISTER *rt, int imm)
{
    rs->value.wd = rt->value.wd;
}

void MIPS_sh(CPU *cpu, REGISTER *rs, REGISTER *rt, int imm)
{
    rs->value.wd = rt->value.wd;
}

void MIPS_sll(CPU *cpu, REGISTER *rs, REGISTER *rt, REGISTER *rd, int shamt, int funct)
{
    rd->value.wd = rt->value.wd << shamt;
}

void MIPS_sllv(CPU *cpu, REGISTER *rs, REGISTER *rt, REGISTER *rd, int shamt, int funct)
{
    rd->value.wd = rt->value.wd << rs->value.wd;
}

void MIPS_slt(CPU *cpu, REGISTER *rs, REGISTER *rt, REGISTER *rd, int shamt, int funct)
{
    rd->value.wd = rs->value.wd < rt->value.wd ? 1 : 0;
}

void MIPS_slti(CPU *cpu, REGISTER *rs, REGISTER *rt, int imm)
{
    rt->value.wd = rs->value.wd < imm ? 1 : 0;
}

void MIPS_sltiu(CPU *cpu, REGISTER *rs, REGISTER *rt, unsigned int imm)
{
    rt->value.wd = rs->value.wd < imm ? 1 : 0;
}

void MIPS_sltu(CPU *cpu, REGISTER *rs, REGISTER *rt, REGISTER *rd, int shamt, int funct)
{
    rd->value.wd = rs->value.wd < rt->value.wd ? 1 : 0;
}

void MIPS_sra(CPU *cpu, REGISTER *rs, REGISTER *rt, REGISTER *rd, int shamt, int funct)
{
    rd->value.wd = rt->value.wd >> shamt;
}

void MIPS_srav(CPU *cpu, REGISTER *rs, REGISTER *rt, REGISTER *rd, int shamt, int funct)
{
    rd->value.wd = rt->value.wd >> rs->value.wd;
}

void MIPS_srl(CPU *cpu, REGISTER *rs, REGISTER *rt, REGISTER *rd, unsigned int shamt, int funct)
{
    rd->value.wd = rs->value.wd >> shamt;
}

void MIPS_srlv(CPU *cpu, REGISTER *rs, REGISTER *rt, REGISTER *rd, unsigned int shamt, int funct)
{
    rd->value.wd = rt->value.wd >> rs->value.wd;
}

void MIPS_sub(CPU *cpu, REGISTER *rs, REGISTER *rt, REGISTER *rd, int shamt, int funct)
{
    rd->value.wd = rs->value.wd - rt->value.wd;
}

void MIPS_subu(CPU *cpu, REGISTER *rs, REGISTER *rt, REGISTER *rd, int shamt, int funct)
{
    rd->value.wd = rs->value.wd - rt->value.wd;
}

void MIPS_sw(CPU *cpu, REGISTER *rs, REGISTER *rt, int imm)
{
    rs->value.wd = rt->value.wd;
}

/**
 * @brief Emulation of syscall function which checks `$v0` to set syscall
 * behaviour and arguments `$a0`, `$a1`, `$a2`, `$a3`.
 * 
 * @param cpu Pointer to instantiation of CPU
 */
void MIPS_syscall(CPU *cpu, REGISTER *rs, REGISTER *rt, REGISTER *rd, int shamt, int funct)
{
    switch (cpu->reg[$v0]->value.wd)
    {
    case 1:
        printf("%d", cpu->reg[$a0]->value.wd);
        break;
    case 2:
        printf("%lf", cpu->reg[$f12]->value.fl);
        break;
    case 3:
        printf("%lf", cpu->reg[$f12]->value.fl);
        printf("%lf", cpu->reg[$f13]->value.fl);
        break;
    case 4:
        printf("%s", (char *)(__intptr_t)(cpu->reg[$v0]->value.wd));
        break;
    case 5:
        scanf("%d", &(cpu->reg[$v0]->value.wd));
        break;
    case 6:
        scanf("%f", &(cpu->reg[$f0]->value.fl));
        break;
    case 7:
        scanf("%f", &(cpu->reg[$f0]->value.fl));
        scanf("%f", &(cpu->reg[$f1]->value.fl));
        break;
    case 8:
        fgets((char *)(__intptr_t)cpu->reg[$a0]->value.wd,
              cpu->reg[$a1]->value.wd,
              stdin);
        break;
    case 9:
        cpu->reg[$v0]->value.wd = (__intptr_t)sbrk(cpu->reg[$a0]->value.wd);
        break;
    case 10:
        cpu->pc = MAX_INSTRUCTIONS;
        break;
    case 11:
        printf("%c", cpu->reg[$a0]->value.wd);
        break;
    default:
        printf("Unknown system call: %d\n", cpu->reg[$v0]->value.wd);
        cpu->pc = MAX_INSTRUCTIONS;
    }
}

void MIPS_xor(CPU *cpu, REGISTER *rs, REGISTER *rt, REGISTER *rd)
{
    rd->value.wd = rs->value.wd ^ rt->value.wd;
}

void MIPS_xori(CPU *cpu, REGISTER *rs, REGISTER *rt, int imm)
{
    rt->value.wd = rs->value.wd ^ imm;
}

#define _R(NAME, FUNCT, STR, FUNC_PTR) [NAME] = FUNC_PTR,
/**
 * @brief Returns the function pointer to an R-type instruction from
 * `R_TYPE_TABLE`
 * 
 * @param key Key is an enumerated type
 * @return R_funct_ptr_t* 
 */
R_funct_ptr_t *R_FUNCT_PTR(int key)
{
    void *_R_FUNC[] = {R_TYPE_TABLE};
    return _R_FUNC[key];
}
#undef _R

#define _I(NAME, OP, STR, FUNC_PTR) [NAME] = FUNC_PTR,
/**
 * @brief Returns the function pointer to an I-type instruction from
 * `I_TYPE_TABLE`
 * 
 * @param key Key is an enumerated type
 * @return I_funct_ptr_t* 
 */
I_funct_ptr_t *I_FUNCT_PTR(int key)
{
    void *_I_FUNC[] = {I_TYPE_TABLE};
    return _I_FUNC[key];
}
#undef _I

#define _J(NAME, OP, STR, FUNC_PTR) [NAME] = FUNC_PTR,
/**
 * @brief Returns the function pointer to a J-type instruction from
 * `J_TYPE_TABLE`
 * 
 * @param key Key is an enumerated type
 * @return J_funct_ptr_t* 
 */
J_funct_ptr_t *J_FUNCT_PTR(int key)
{
    void *_J_FUNC[] = {J_TYPE_TABLE};
    return _J_FUNC[key];
}
#undef _J

#define _P(NAME, FUNCT, STR, FUNC_PTR) [NAME] = FUNC_PTR,
/**
 * @brief Returns the function pointer to a pseudo instruction from
 * `P_TYPE_TABLE`
 * 
 * @param key Key is an enumerated type
 * @return R_funct_ptr_t* 
 */
R_funct_ptr_t *P_FUNCT_PTR(int key)
{
    void *_P_FUNC[] = {P_TYPE_TABLE};
    return _P_FUNC[key];
}
#undef _P

/******************************************************************************
 *                                  MAIN                                      *
 ******************************************************************************/

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
    for (int i = 0; i < NUM_REGISTERS; i++)
        if (cpu->reg[i]->value.wd != 0 && $0 <= i && i <= $31) // 1521 spim
            printf("%-3s = %d\n",
                   REG_NUM_STR(cpu->reg[i]->name),
                   cpu->reg[i]->value.wd);
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
            printf("%s", P_STR(instr.funct));
        }
        else
        {
            printf("%-4s %s, %s, %s",
                   P_STR(instr.funct),
                   REG_NUM_STR(instr.rd),
                   REG_NUM_STR(instr.rs),
                   REG_NUM_STR(instr.rt));
        }
    }
    else if (is_R_FORMAT(instr_code))
    {
        R_FORMAT instr = extract_R_FORMAT(instr_code);
        printf("%-4s %s, %s, %s",
               R_STR(instr.funct),
               REG_NUM_STR(instr.rd),
               REG_NUM_STR(instr.rs),
               REG_NUM_STR(instr.rt));
    }
    else if (is_I_FORMAT(instr_code))
    {
        I_FORMAT instr = extract_I_FORMAT(instr_code);
        if (instr.op == BEQ || instr.op == BNE)
        {
            printf("%-4s %s, %s, %d",
                   I_STR(instr.op),
                   REG_NUM_STR(instr.rs),
                   REG_NUM_STR(instr.rt), instr.imm);
        }
        else if (instr.op == LUI)
        {
            printf("%-4s %s, %d",
                   I_STR(instr.op),
                   REG_NUM_STR(instr.rt),
                   instr.imm);
        }
        else
        {
            printf("%-4s %s, %s, %d",
                   I_STR(instr.op),
                   REG_NUM_STR(instr.rt),
                   REG_NUM_STR(instr.rs),
                   instr.imm);
        }
    }
    else if (is_J_FORMAT(instr_code))
    {
        J_FORMAT instr = extract_J_FORMAT(instr_code);
        printf("%-4s %d", J_STR(instr.op), instr.addr);
    }
}

/**
 * @brief Carry out the CPU's processes.
 * 
 * @param cpu Pointer to instantiation of CPU
 * @param instr_code Encoded MIPS instruction
 */
bool processes(CPU *cpu, int instr_code)
{
    bool jumped = false;
    if (is_P_FORMAT(instr_code))
    {
        R_FORMAT instr = extract_R_FORMAT(instr_code);
        P_FUNCT_PTR(instr.funct)
        (cpu,
         cpu->reg[instr.rs],
         cpu->reg[instr.rt],
         cpu->reg[instr.rd],
         instr.shamt,
         instr.funct);
    }
    else if (is_R_FORMAT(instr_code))
    {
        R_FORMAT instr = extract_R_FORMAT(instr_code);
        R_FUNCT_PTR(instr.funct)
        (cpu,
         cpu->reg[instr.rs],
         cpu->reg[instr.rt],
         cpu->reg[instr.rd],
         instr.shamt,
         instr.funct);
    }
    else if (is_I_FORMAT(instr_code))
    {
        I_FORMAT instr = extract_I_FORMAT(instr_code);
        I_FUNCT_PTR(instr.op)
        (cpu,
         cpu->reg[instr.rs],
         cpu->reg[instr.rt],
         instr.imm);

        if (instr.op == BEQ || instr.op == BNE)
            jumped = true;
    }
    else if (is_J_FORMAT(instr_code))
    {
        J_FORMAT instr = extract_J_FORMAT(instr_code);
        J_FUNCT_PTR(instr.op)
        (cpu, cpu->reg[instr.addr]);
    }

    // Clean up registers
    cpu->reg[$zero]->value.wd = 0;

    return jumped;
}

/**
 * @brief There are two parses over the hexadecimal string to get the print then
 * the execution.
 * 
 * @param f Stream of encoded MIPS instructions
 * @param cpu Pointer to instantiation of CPU
 */
void hexadecimal_parser(FILE *f, CPU *cpu)
{
    char line[BUFFER];

    // Load program into memory
    for (int i = 0; fgets(line, sizeof(line), f); i++)
    {
        int instr_code = (int)strtol(line, NULL, 16);

        printf("%3d: ", i);
        print_instruction_by_format(cpu, instr_code);
        printf("\n");

        cpu->memory[i] = instr_code;
    }

    printf("Output\n");

    // Execute the program loaded in memory
    for (cpu->pc = 0; cpu->pc < MAX_INSTRUCTIONS;)
        if (!processes(cpu, cpu->memory[cpu->pc]))
            cpu->pc++;
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

    printf("Program\n");
    hexadecimal_parser(f, cpu);

    printf("Registers After Execution\n");
    print_registers(cpu);

    free_CPU(cpu);
    fclose(f);

    return EXIT_SUCCESS;
}
