#pragma once

#include "hardware.h"

void MIPS_add(CPU *cpu, REGISTER *rs, REGISTER *rt, REGISTER *rd, int shamt, int funct);
void MIPS_addi(CPU *cpu, REGISTER *rs, REGISTER *rt, int imm);
void MIPS_addiu(CPU *cpu, REGISTER *rs, REGISTER *rt, unsigned int imm);
void MIPS_and(CPU *cpu, REGISTER *rs, REGISTER *rt, REGISTER *rd, int shamt, int funct);
void MIPS_andi(CPU *cpu, REGISTER *rs, REGISTER *rt, int imm);
void MIPS_addu(CPU *cpu, REGISTER *rs, REGISTER *rt, REGISTER *rd, int shamt, int funct);
void MIPS_beq(CPU *cpu, REGISTER *rs, REGISTER *rt, int imm);
void MIPS_bgez(CPU *cpu, REGISTER *rs, REGISTER *rt, int imm);
void MIPS_bgtz(CPU *cpu, REGISTER *rs, REGISTER *rt, int imm);
void MIPS_blez(CPU *cpu, REGISTER *rs, REGISTER *rt, int imm);
void MIPS_bltz(CPU *cpu, REGISTER *rs, REGISTER *rt, int imm);
void MIPS_bne(CPU *cpu, REGISTER *rs, REGISTER *rt, int imm);
void MIPS_break(CPU *cpu, REGISTER *rs, REGISTER *rt, REGISTER *rd, int shamt, int funct);
void MIPS_div(CPU *cpu, REGISTER *rs, REGISTER *rt, REGISTER *rd, int shamt, int funct);
void MIPS_divu(CPU *cpu, REGISTER *rs, REGISTER *rt, REGISTER *rd);
void MIPS_j(CPU *cpu, REGISTER *addr);
void MIPS_jal(CPU *cpu, REGISTER *addr);
void MIPS_jalr(CPU *cpu, REGISTER *rs, REGISTER *rt, REGISTER *rd);
void MIPS_jr(CPU *cpu, REGISTER *rs, REGISTER *rt, REGISTER *rd, int shamt, int funct);
void MIPS_lb(CPU *cpu, REGISTER *rs, REGISTER *rt, int imm);
void MIPS_lh(CPU *cpu, REGISTER *rs, REGISTER *rt, int imm);
void MIPS_lui(CPU *cpu, REGISTER *rs, REGISTER *rt, int imm);
void MIPS_lw(CPU *cpu, REGISTER *rs, REGISTER *rt, int imm);
void MIPS_mfc0();
void MIPS_mfhi(CPU *cpu, REGISTER *rs, REGISTER *rt, REGISTER *rd, int shamt, int funct);
void MIPS_mflo(CPU *cpu, REGISTER *rs, REGISTER *rt, REGISTER *rd, int shamt, int funct);
void MIPS_mtc0();
void MIPS_mthi(CPU *cpu, REGISTER *rs, REGISTER *rt, REGISTER *rd, int shamt, int funct);
void MIPS_mtlo(CPU *cpu, REGISTER *rs, REGISTER *rt, REGISTER *rd, int shamt, int funct);
void MIPS_mult(CPU *cpu, REGISTER *rs, REGISTER *rt, REGISTER *rd, int shamt, int funct);
void MIPS_multu(CPU *cpu, REGISTER *rs, REGISTER *rt, REGISTER *rd, int shamt, int funct);
void MIPS_mul(CPU *cpu, REGISTER *rs, REGISTER *rt, REGISTER *rd, int shamt, int funct);
void MIPS_nor(CPU *cpu, REGISTER *rs, REGISTER *rt, REGISTER *rd, int shamt, int funct);
void MIPS_or(CPU *cpu, REGISTER *rs, REGISTER *rt, REGISTER *rd, int shamt, int funct);
void MIPS_ori(CPU *cpu, REGISTER *rs, REGISTER *rt, int imm);
void MIPS_sb(CPU *cpu, REGISTER *rs, REGISTER *rt, int imm);
void MIPS_sh(CPU *cpu, REGISTER *rs, REGISTER *rt, int imm);
void MIPS_sll(CPU *cpu, REGISTER *rs, REGISTER *rt, REGISTER *rd, int shamt, int funct);
void MIPS_sllv(CPU *cpu, REGISTER *rs, REGISTER *rt, REGISTER *rd, int shamt, int funct);
void MIPS_slt(CPU *cpu, REGISTER *rs, REGISTER *rt, REGISTER *rd, int shamt, int funct);
void MIPS_slti(CPU *cpu, REGISTER *rs, REGISTER *rt, int imm);
void MIPS_sltiu(CPU *cpu, REGISTER *rs, REGISTER *rt, unsigned int imm);
void MIPS_sltu(CPU *cpu, REGISTER *rs, REGISTER *rt, REGISTER *rd, int shamt, int funct);
void MIPS_sra(CPU *cpu, REGISTER *rs, REGISTER *rt, REGISTER *rd, int shamt, int funct);
void MIPS_srav(CPU *cpu, REGISTER *rs, REGISTER *rt, REGISTER *rd, int shamt, int funct);
void MIPS_srl(CPU *cpu, REGISTER *rs, REGISTER *rt, REGISTER *rd, unsigned int shamt, int funct);
void MIPS_srlv(CPU *cpu, REGISTER *rs, REGISTER *rt, REGISTER *rd, unsigned int shamt, int funct);
void MIPS_sub(CPU *cpu, REGISTER *rs, REGISTER *rt, REGISTER *rd, int shamt, int funct);
void MIPS_subu(CPU *cpu, REGISTER *rs, REGISTER *rt, REGISTER *rd, int shamt, int funct);
void MIPS_sw(CPU *cpu, REGISTER *rs, REGISTER *rt, int imm);
void MIPS_syscall(CPU *cpu, REGISTER *rs, REGISTER *rt, REGISTER *rd, int shamt, int funct);
void MIPS_xor(CPU *cpu, REGISTER *rs, REGISTER *rt, REGISTER *rd);
void MIPS_xori(CPU *cpu, REGISTER *rs, REGISTER *rt, int imm);
