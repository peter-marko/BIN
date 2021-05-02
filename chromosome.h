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

double fRand(double fMin, double fMax);

class chromosome {
    gate* random_gate(int position);
    uint64_t get_value(int idx, uint64_t in[WORD_LENGTH]);
    UINT get_value(int idx, UINT in);
    void setUsedTrue(int idx);
    int countUsed();
    double getErr(uint64_t out, uint64_t expOut, int bitPos);
    
public:
    void print(std::ofstream &evalOut, std::ofstream &circuitOut, table tab);
    void dominated(chromosome &other);
    gate mChromosome[MAT_HEIGHT * MAT_WIDTH];   // chromosome information
    double mFitness;            // fitness of this chromosome
    double mErr;
    int mGates;
    double mCrowdD;
    int mDomination;

    chromosome ();
    void init();
    void simulate(uint64_t in[WORD_LENGTH], uint64_t out[WORD_LENGTH]);
    gate at(int idx);
    void fitness(table tab);
    void mutate();
    UINT used_gates_cnt();
};

#endif