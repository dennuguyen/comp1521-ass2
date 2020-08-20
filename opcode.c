#include "hashtable.h"
#include "opcode.h"
#include "utils.h"

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
 * @brief Check if opcode is in R-format by comparing its funct value against
 * known R-instructions.
 *
 * @param instr_code Encoded MIPS instruction
 * @return true
 * @return false
 */
bool is_R_FORMAT(int instr_code)
{
    R_FORMAT instr = extract_R_FORMAT(instr_code);
    for (int i = 0; i < n_R_LIST; i++)
        if (instr.op == 0b000000 && instr.funct == R_LIST[i])
            return true;
    return false;
}

/**
 * @brief Check if opcode is in I-format by comparing its op value against
 * known I-instructions.
 *
 * @param instr_code Encoded MIPS instruction
 * @return true
 * @return false
 */
bool is_I_FORMAT(int instr_code)
{
    I_FORMAT instr = extract_I_FORMAT(instr_code);
    for (int i = 0; i < n_I_LIST; i++)
        if (instr.op == I_LIST[i])
            return true;
    return false;
}

/**
 * @brief Check if instruction is in J-format by comparing its op value against
 * known J-instructions.
 *
 * @param instr_code Encoded MIPS instruction
 * @return true
 * @return false
 */
bool is_J_FORMAT(int instr_code)
{
    J_FORMAT instr = extract_J_FORMAT(instr_code);
    for (int i = 0; i < n_J_LIST; i++)
        if (instr.op == J_LIST[i])
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
    for (int i = 0; i < n_P_LIST; i++)
        if (instr.funct == P_LIST[i])
            return true;
    return false;
}
