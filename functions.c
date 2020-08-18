#include <stdio.h>

#include "functions.h"
#include "utils.h"

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
    void *_R_FUNC[] = { R_TYPE_TABLE };
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
    void *_I_FUNC[] = { I_TYPE_TABLE };
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
    void *_J_FUNC[] = { J_TYPE_TABLE };
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
    void *_P_FUNC[] = { P_TYPE_TABLE };
    return _P_FUNC[key];
}
#undef _P

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
        cpu->pc += imm - 1;
}

void MIPS_bgez(CPU *cpu, REGISTER *rs, REGISTER *rt, int imm)
{
    if (rs->value.wd >= 0)
        cpu->pc += imm - 1;
}

void MIPS_bgtz(CPU *cpu, REGISTER *rs, REGISTER *rt, int imm)
{
    if (rs->value.wd > 0)
        cpu->pc += imm - 1;
}

void MIPS_blez(CPU *cpu, REGISTER *rs, REGISTER *rt, int imm)
{
    if (rs->value.wd <= 0)
        cpu->pc += imm - 1;
}

void MIPS_bltz(CPU *cpu, REGISTER *rs, REGISTER *rt, int imm)
{
    if (rs->value.wd < 0)
        cpu->pc += imm - 1;
}

void MIPS_bne(CPU *cpu, REGISTER *rs, REGISTER *rt, int imm)
{
    if (rs->value.wd != rt->value.wd)
        cpu->pc += imm - 1;
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

void MIPS_j(CPU *cpu, REGISTER *addr)
{
    cpu->pc = addr->value.wd;
}

void MIPS_jal(CPU *cpu, REGISTER *addr)
{
    cpu->pc = addr->value.wd;
}

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
    // case 2:
    //     printf("%lf", cpu->reg[$f12]->value.fl);
    //     break;
    // case 3:
    //     printf("%lf", cpu->reg[$f12]->value.fl);
    //     printf("%lf", cpu->reg[$f13]->value.fl);
    //     break;
    // case 4:
    //     printf("%s", (char *)(__intptr_t)(cpu->reg[$v0]->value.wd));
    //     break;
    // case 5:
    //     scanf("%d", &(cpu->reg[$v0]->value.wd));
    //     break;
    // case 6:
    //     scanf("%f", &(cpu->reg[$f0]->value.fl));
    //     break;
    // case 7:
    //     scanf("%f", &(cpu->reg[$f0]->value.fl));
    //     scanf("%f", &(cpu->reg[$f1]->value.fl));
    //     break;
    // case 8:
    //     fgets((char *)(__intptr_t)cpu->reg[$a0]->value.wd,
    //         cpu->reg[$a1]->value.wd,
    //         stdin);
    //     break;
    // case 9:
    //     cpu->reg[$v0]->value.wd = (__intptr_t)sbrk(cpu->reg[$a0]->value.wd);
    //     break;
    case 10:
        cpu->pc = MAX_INSTR;
        break;
    case 11:
        printf("%c", cpu->reg[$a0]->value.wd);
        break;
    default:
        printf("Unknown system call: %d\n", cpu->reg[$v0]->value.wd);
        cpu->pc = MAX_INSTR;
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