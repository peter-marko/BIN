#include "chromosome.h"

double popcount(UINT n) {
    std::bitset<sizeof(UINT)> b(n);
    return b.count();
}

chromosome::chromosome() {
    std::random_device rd; // obtain a random number from hardware
    std::mt19937 gen(rd()); // seed the generator
    mGate_cnt = UNDEFINED;
    for (int i = 0; i < MAT_HEIGHT * MAT_WIDTH; i++) {
        gate g(i, gen);
        mChromosome[i] = g;
    }
    mFitness = 0;
}

/**
 * \brief Get value from vector of gates
 * \param idx Position of value in vector
 */
gate chromosome::at(int idx) {
    return mChromosome[idx - WORD_LENGTH];
}

template<class T>
inline std::string format_binary(T x)
{
    char b[sizeof(T)*8+1] = {0};

    for (size_t z = 0; z < sizeof(T)*8; z++)
        b[sizeof(T)*8-1-z] = ((x>>z) & 0x1) ? '1' : '0';

    return std::string(b);
}

template<class T>
inline UINT bit_cnt(T x)
{
    UINT res = 0;
    for (size_t z = 0; z < sizeof(T)*8; z++)
        res += ((x>>z) & 0x1) ? 1 : 0;

    return res;
}


double getErr(uint64_t out, uint64_t expOut, int bitPos) {
    // return pow(2, bitPos) * bit_cnt(out ^ expOut);
    return bit_cnt(out ^ expOut);
}

/**
 * \brief Print chromosome details to two CSV files
 * \param pathEval Path to evaluation CSV, file will contain on each line:
 *      - Value from circuit for line number as input
 *      - Value from table for line number as input
 * \param pathCircuit Path to circuit CSV, file will contain gate on each line (if gate not used => line is empty):
 *      - Index of first input
 *      - Index of second input
 *      - Name of function used
 * \param tab Table of correct values for UINT inputs
 */
void chromosome::print(char *pathEval, char *pathCircuit, table tab) {
    printf("todo chromosome::print() %g %g\n", mFitness, mCost);
    std::ofstream evalOut;
    evalOut.open(pathEval, std::ios::out | std::ios::trunc );
    std::ofstream circuitOut;
    circuitOut.open(pathCircuit, std::ios::out | std::ios::trunc );
    

    uint64_t out[4][WORD_LENGTH] = {0};
    for (int i = 0; i < 4; i++) {
        simulate(tab.mTableIn[i], out[i]);
    }
    for (int i = 0; i <= U_MAX; i++) {
        evalOut << std::to_string(i) << "," << std::to_string(get(out, i)) << "," << std::to_string(get(tab.mTableOut, i)) << "," << format_binary(get(out, i)) << "," << format_binary(get(tab.mTableOut, i)) << "\n";
    }
    
    // int idx = 0;
    // for (gate g: mChromosome) {
    //     if (mUsedGates[idx]) {
    //         g.print(circuitOut);
    //     }
    //     circuitOut << "\n";
    //     idx++;
    // }

}

/**
 * \brief Recursive function for evaluation of boolean value for one gate
 * \param idx Index of gate in chromosome we wish to evaluate
 * \param in Input of whole circuit
 * \return Binary output of gate
 */
uint64_t chromosome::get_value(int idx, uint64_t in[WORD_LENGTH]) {
    if (idx < WORD_LENGTH) {
        return in[idx];
    }
    gate gate = at(idx);
    uint64_t a = get_value(gate.in_1, in);
    uint64_t b = get_value(gate.in_2, in);
    return gate.exec(a, b);
}


/**
 * \brief Recursive function for evaluation of boolean value for one gate
 * \param idx Index of gate in chromosome we wish to evaluate
 * \param in Input of whole circuit
 * \return Binary output of gate
 */
UINT chromosome::get_value(int idx, UINT in) {
    if (idx < WORD_LENGTH) {
        return (in >> idx) & 1;
    }
    gate gate = at(idx);
    if (gate.value != UNDEFINED) {
        return gate.value;
    }
    UINT a = get_value(gate.in_1, in);
    UINT b = get_value(gate.in_2, in);
    return gate.exec(a, b);
}

/**
 * \brief Count used gates
 * \return Number of gates in use
 */
UINT chromosome::gate_cnt(){
    if (mGate_cnt != UNDEFINED)
        return mGate_cnt;
    UINT cnt = 42;
    // for (int i = 0; i < GATE_LIMIT; i++) {
    //     UINT is_used = mUsedGates[i];
    //     cnt += is_used;
    // }
    return cnt;
}

/**
 * \brief Calculate fitness of chromosome as sum of squares
 * \param tab Table of expected values
 */
void chromosome::fitness(table tab) {
    double err = 0;
    for (int i = 0; i < 4; i++) {
        uint64_t out[WORD_LENGTH] = {0};
        simulate(tab.mTableIn[i], out);
        // printf("simulated out %lu %lu %lu %lu %lu %lu %lu %lu\n", out[0], out[1], out[2], out[3], out[4 + 0], out[4 + 1], out[4 + 2], out[4 + 3]);
        for (UINT j = 0; j < WORD_LENGTH; j++) {
            err += getErr(out[j], tab.mTableOut[i][j], j);
        }
    }

    mCost = err; // * (1 + gate_cnt() * FEW_GATES_IMPORTANCE);
    mFitness = 1 / (mCost + 1);
}

/**
 * \brief Simulate circuit for specified input
 * \param in Input of circuit
 * \return Value of output of circuit
 */
void chromosome::simulate(uint64_t in[WORD_LENGTH], uint64_t out[WORD_LENGTH]) {
    for (int j = 0; j < MAT_HEIGHT; j++) {
        int idx = (MAT_WIDTH - 1) * MAT_HEIGHT + j;
        out[j] = get_value(idx, in);
    }
}

/**
 * \brief Mutate some gates in chromosome
 */
void chromosome::mutate() {
    for (int i = 0; i < N_MUT; i++) {
        int mutIdx = rand() % (MAT_HEIGHT * MAT_WIDTH);
        mChromosome[mutIdx].mutate(mutIdx);
    }
}