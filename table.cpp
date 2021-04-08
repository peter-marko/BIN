#include "table.h"

template<class T>
inline std::string format_binary(T x)
{
    char b[sizeof(T)*8+1] = {0};

    for (size_t z = 0; z < sizeof(T)*8; z++)
        b[sizeof(T)*8-1-z] = ((x>>z) & 0x1) ? '1' : '0';

    return std::string(b);
}

void insert(uint64_t table[4][8], uint64_t idx, UINT bit, uint64_t value) {
    uint64_t atIdx = (value & (1 << bit)) >> bit;
    uint64_t cur = atIdx << (idx % 64);
    table[3 - idx / 64][bit] |= cur;
}

UINT get(uint64_t table[4][8], int idx) {
    uint64_t res = 0;
    for (int i =0; i < 8; i++) {
        uint64_t cell = table[3 - idx / 64][i];
        uint64_t shift = idx % 64;
        uint64_t one = 1;
        uint64_t cur = (cell & (one << shift)) >> shift;
        res |= cur << i;
    }
    return res;
}


table::table() {
    for (uint64_t i = 0; i <= U_MAX; i++) {
        double d = uint_to_double(i);
        d = sin(d);
        UINT out = double_to_uint8(d);
        for (UINT j = 0; j < 8; j++) {
            insert(mTableOut, i, j, out);
            insert(mTableIn, i, j, i);
        }
    }
    // for (int i = 0; i < 8; i++) {
    //     for (int j = 0; j < 4; j++) {
    //         printf( "%s", format_binary(mTableIn[j][i]).c_str() );
    //     }
    //     printf("\n");
    // }
    // for (int i = 0; i <= U_MAX; i++) {
    //     printf("get %u %u\n", i,  get(mTableOut, i));
    // }
}

// UINT table::at(UINT idx) {
//     return array[idx];
// }

UINT bit_count(UINT n, UINT out_arr[WORD_LENGTH]) {
    UINT counter = 0;
    UINT i = 0;
    while(n) {
        if (n % 2) {
            counter++;
            out_arr[i]++;
        }
        n >>= 1;
        i++;
    }
    // printf("counter %u\n", counter);
    return counter;
 }

// UINT table::fitness(UINT in, UINT out, UINT out_arr[WORD_LENGTH]) {
//     UINT searched = array.at(in);
//     UINT un_matched_bits = searched ^ out;
//     return bit_count(un_matched_bits, out_arr);
// }

UINT table::double_to_uint8(double in) {
    in += 1;
    in /= 2;
    in *= U_MAX;
    return (UINT) in;
}

double table::uint_to_double(UINT in) {
    return (2*M_PI *in + 0.5) / U_MAX;
}