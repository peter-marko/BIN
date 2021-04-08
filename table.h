#ifndef TABLE_H
#define TABLE_H

#include <vector>
#include <math.h>
#include <string>
#include "params.h"

void insert(uint64_t table[4][8], uint64_t idx, UINT bit, uint64_t value);
UINT get(uint64_t table[4][8], int idx);

class table
{
private:
public:
    uint64_t mTableOut[4][8] = {0};
    uint64_t mTableIn[4][8] = {0};
    table();
    // UINT at(UINT idx);
    // UINT fitness(UINT in, UINT out, UINT out_arr[WORD_LENGTH]);
    static UINT double_to_uint8(double in);
    static double uint_to_double(UINT in);
};


#endif