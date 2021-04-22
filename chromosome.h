#ifndef CHROMOSOME_H
#define CHROMOSOME_H

#include <vector>
#include <math.h>
#include <fstream>
#include <iostream>
#include <bitset>
#include <string.h>
#include <random>

#include "params.h"
#include "gate.h"
#include "table.h"

#define NUM_SIZE 8

class chromosome {
    gate* random_gate(int position);
    uint64_t get_value(int idx, uint64_t in[WORD_LENGTH]);
    UINT get_value(int idx, UINT in);
    void setUsedTrue(int idx);
    int countUsed();
    double getErr(uint64_t out, uint64_t expOut, int bitPos);
    UINT  mGate_cnt;

public:
    UINT gate_cnt();
    void print(char *pathEval, char *pathCircuit, table tab);
    gate mChromosome[MAT_HEIGHT * MAT_WIDTH];   // chromosome information
    double mFitness;            // fitness of this chromosome
    double mCost;               // cost of solution proposed by this chromosome

    chromosome ();
    void simulate(uint64_t in[WORD_LENGTH], uint64_t out[WORD_LENGTH]);
    gate at(int idx);
    void fitness(table tab);
    void mutate();
    UINT used_gates_cnt();
};

#endif