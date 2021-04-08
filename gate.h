#ifndef GATE_H
#define GATE_H

#include <stdlib.h>
#include <math.h>
#include <fstream>
#include <random>
#include "params.h"

class gate {
    uint64_t set_val(uint64_t val);
public:
    int in_1;
    int in_2;
    int func;
    UINT used;
    uint64_t value;
    double err;
    std::mt19937 gen;
    
    gate();
    gate(int position, std::mt19937 gen);
    gate(gate *g);
    void print(std::ofstream &circuitOut);
    uint64_t exec(uint64_t a, uint64_t b);
    void mutate(int position);
};
#endif
