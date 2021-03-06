#ifndef PARAMS_H
#define PARAMS_H

#include <stdio.h>
#include <inttypes.h>

#define WORD_LENGTH 8       // length of input and output word, e.g. 8 bits
#define POP_SIZE 25         // number of chromosomes in population
#define N_MUT 7
#define ITERATIONS 300000     // number of generations
#define MAT_WIDTH 7
#define MAT_HEIGHT 8
#define L_BACK 8
#define P_FIT 0.75

typedef uint8_t UINT;
typedef UINT BOOL;

#define U_MAX 0xff
#define UNDEFINED U_MAX
enum func {AND, NAND, OR, NOR, XOR, NUM_FUNCS};

#endif