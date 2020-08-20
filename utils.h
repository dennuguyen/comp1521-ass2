#pragma once

#include <stdint.h>

#define NUM_REGISTERS 60
#define BUFFER 4096
#define MAX_INSTR 1000
#define MAX_MEMORY 65536

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
