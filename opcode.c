#include "opcode.h"

#define _R(NAME, FUNCT, STR, FUNC_PTR) [NAME] = STR,
/**
 * @brief Returns the R-type instruction from `R_TYPE_TABLE` as a string.
 *
 * @param key Key is an enumerated type
 * @return char*
 */
char *R_STR(int key)
{
    char *_R_STR[] = { R_TYPE_TABLE };
    return _R_STR[key];
}
#undef _R

#define _I(NAME, OP, STR, FUNC_PTR) [NAME] = STR,
/**
 * @brief Returns the I-type instruction from `I_TYPE_TABLE` as a string.
 *
 * @param key Key is an enumerated type
 * @return char*
 */
char *I_STR(int key)
{
    char *_I_STR[] = { I_TYPE_TABLE };
    return _I_STR[key];
}
#undef _I

#define _J(NAME, OP, STR, FUNC_PTR) [NAME] = STR,
/**
 * @brief Returns the J-type instruction from `J_TYPE_TABLE` as a string.
 *
 * @param key Key is an enumerated type
 * @return char*
 */
char *J_STR(int key)
{
    char *_J_STR[] = { J_TYPE_TABLE };
    return _J_STR[key];
}
#undef _J

#define _P(NAME, FUNCT, STR, FUNC_PTR) [NAME] = STR,
/**
 * @brief Returns the pseudo instruction from `P_TYPE_TABLE` as a string.
 *
 * @param key Key is an enumerated type
 * @return char*
 */
char *P_STR(int key)
{
    char *_P_STR[] = { P_TYPE_TABLE };
    return _P_STR[key];
}
#undef _P

/**
 * @brief Extracts the bit fields from an encoded instruction for an R-type
 * instruction.
 *
 * @param instr_code Encoded MIPS instruction
 * @return R_FORMAT
 */
R_FORMAT extract_R_FORMAT(int instr_code)
{
    R_FORMAT instr;
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
    // I_FORMAT instr = extract_I_FORMAT(instr_code);
    // for (int i = 0; i < I_SIZE; i++)
    //     if(instr.op == I_)

    if (!is_R_FORMAT(instr_code) &&
        !is_J_FORMAT(instr_code) &&
        !is_P_FORMAT(instr_code))
        return true;
    return false;
}