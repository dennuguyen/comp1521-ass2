/**
 * Simple MIPS emulator called SMIPS for COMP1521.
 * 
 * SMIPS outputs:
 *  -> the instruction to each instruction code
 *  -> output produced by syscalls
 *  -> register values when program terminates
 * 
 * SMIPS currently supports the following subset of MIPS instructions:
 *  -> add  $d, $s, $t
 *  -> sub  $d, $s, $t
 *  -> and  $d, $s, $t
 *  -> or   $d, $s, $t
 *  -> slt  $d, $s, $t
 *  -> mul  $d, $s, $t
 *  -> beq  $s, $t, I
 *  -> bne  $s, $t, I
 *  -> addi $t, $s, I
 *  -> slti $t, $s, I
 *  -> andi $t, $s, I
 *  -> ori  $t, $s, I
 *  -> lui  $t, I
 *  -> syscall
 * 
 * Style:
 *  -> snake_case for variable and function naming
 *  -> CamelCase for struct and class naming
 * 
 * Author: Dan Nguyen (z5206032)
 */

#include <stdio.h>
#include <stdlib.h>

#define NUM_REGISTERS 32
#define RAM_BUFFER 1008
#define LO 0
#define HI !(LO)

typedef __uint32_t reg_t; // Size of a MIPS register is 32 bits.

/**
 * MIPS data types
 */
typedef __uint32_t word_t; // Size of word.
typedef __uint16_t half_t; // Size of half.
typedef __uint8_t byte_t;  // Size of byte.

/**
 * MIPS has 32 registers numbers.
 */
typedef enum reg_num
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
} reg_num;

/**
 * MIPS registers have conventional names.
 */
typedef enum reg_name
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
} reg_name;

/**
 * A MIPS register has a register name and value of size 'reg_t'.
 */
typedef struct Register
{
    reg_name name;
    reg_t value;
} Register;

/**
 * A MIPS CPU has a program counter, 32 x 32-bit general purpose registers,
 * 16 x 64-bit floating point registers, Hi and Lo registers.
 */
typedef struct CPU
{
    unsigned char pc;             // Program Counter
    Register *reg[NUM_REGISTERS]; // Array of CPU registers
} CPU;

/**
 * Initialise the register given a register name.
 */
Register *init_reg(reg_name name)
{
    Register *reg = malloc(sizeof(Register));
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
 * Destroy the register.
 */
void free_reg(Register *reg)
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

/**
 * Destroy the RAM.
 */
void free_RAM(int *ram)
{
    free(ram);
    ram = NULL;
}

/**
 * Emulate MIPS add.
 */
void add(CPU *cpu, Register *d, Register *s, Register *t)
{
    d->value = s->value + t->value;
}

/**
 * Emulate MIPS sub.
 */
void sub(CPU *cpu, Register *d, Register *s, Register *t)
{
    d->value = s->value - t->value;
}

/**
 * Emulate MIPS and.
 */
void and (CPU * cpu, Register *d, Register *s, Register *t)
{
    d->value = s->value & t->value;
}

/**
 * Emulate MIPS or.
 */
void or (CPU * cpu, Register *d, Register *s, Register *t)
{
    d->value = s->value | t->value;
}

/**
 * Emulate MIPS slt.
 */
void slt(CPU *cpu, Register *d, Register *s, Register *t)
{
    d->value = s->value < t->value ? 1 : 0;
}

/**
 * Emulate MIPS mul.
 */
void mul(CPU *cpu, Register *d, Register *s, Register *t)
{
    d->value = s->value * t->value;
}

/**
 * Emulate MIPS beq.
 */
void beq(CPU *cpu, Register *s, Register *t, int I)
{
    if (s->value == t->value)
        cpu->pc += I;
}

/**
 * Emulate MIPS bne.
 */
void bne(CPU *cpu, Register *s, Register *t, int I)
{
    if (s->value != t->value)
        cpu->pc += I;
}

/**
 * Emulate MIPS addi.
 */
void addi(CPU *cpu, Register *t, Register *s, int I)
{
    t->value = s->value + I;
}

/**
 * Emulate MIPS slti.
 */
void slti(CPU *cpu, Register *t, Register *s, int I)
{
    t->value = s->value < I;
}

/**
 * Emulate MIPS andi.
 */
void andi(CPU *cpu, Register *t, Register *s, int I)
{
    t->value = s->value & I;
}

/**
 * Emulate MIPS ori.
 */
void ori(CPU *cpu, Register *t, Register *s, int I)
{
    t->value = s->value | I;
}

/**
 * Emulate MIPS ori.
 */
void lui(CPU *cpu, Register *t, int I)
{
    t->value = I << 16;
}

/**
 * Emulate MIPS syscall.
 */
void syscall(CPU *cpu, Register *a0, Register *a1, Register *a2, Register *a3, Register *v0)
{
    switch (v0->value)
    {
    case 1:
        printf("%d", a0->value);
        break;
    case 2:
        break;
    case 3:
        break;
    case 4:
        printf("%s", &(a0->value));
        break;
    case 5:
        scanf("%d", &(v0->value));
        break;
    case 6:
        scanf("%f", &(v0->value));
        break;
    case 7:
        scanf("%lf", &(v0->value));
        break;
    case 8:
        fgets(a0->value, a1->value, stdin);
        break;
    case 9:
        break;
    case 10:
        exit(EXIT_SUCCESS);
        break;
    case 11:
        printf("%c", a0->value);
        break;
    default:
        fprintf(stderr, "wth");
    }
}

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

    printf("Output\n");

    printf("Registers After Execution\n");

    free_CPU(cpu);
    free_RAM(ram);
    fclose(f);

    return EXIT_SUCCESS;
}
