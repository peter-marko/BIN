#include "chromosome.h"

double popcount(UINT n) {
    std::bitset<sizeof(UINT)> b(n);
    return b.count();
}

/**
 * \brief Generate and return random double in range fMin, fMax
 */
double fRand(double fMin, double fMax) {
    double f = (double)rand() / RAND_MAX;
    return fMin + f * (fMax - fMin);
}


chromosome::chromosome() {
    std::random_device rd; // obtain a random number from hardware
    std::mt19937 gen(rd()); // seed the generator
    for (int i = 0; i < MAT_HEIGHT * MAT_WIDTH; i++) {
        gate g(i, gen);
        mChromosome[i] = g;
    }
    mFitness = 0;
}

void chromosome::init() {
    mDomination = 0;
    mCrowdD = 0; //MAT_HEIGHT * MAT_WIDTH;
}

void chromosome::dominated(chromosome &other) {
    if ((mErr >= other.mErr && mGates > other.mGates) ||
        (mErr > other.mErr && mGates >= other.mGates))
    {
        mDomination++;
    }
}

/**
 * \brief Get value from vector of gates
 * \param idx Position of value in vector
 */
gate chromosome::at(int idx) {
    return mChromosome[idx - WORD_LENGTH];
}

void chromosome::setUsedTrue(int idx) {
    mChromosome[idx].mUsed = true; 
}

int chromosome::countUsed() {
    int sum = 0;
    for (int i = 0; i < MAT_HEIGHT * MAT_WIDTH; i++) {
        sum += mChromosome[i].mUsed;
    }
    return sum;
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


double chromosome::getErr(uint64_t out, uint64_t expOut, int bitPos) {
    return pow(2, bitPos) * bit_cnt(out ^ expOut);
    // return (bitPos * bitPos + 1) * bit_cnt(out ^ expOut);
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
void chromosome::print(std::ofstream &evalOut, std::ofstream &circuitOut, table tab) {
    

    uint64_t out[4][WORD_LENGTH] = {0};
    for (int i = 0; i < 4; i++) {
        simulate(tab.mTableIn[i], out[i]);
    }
    for (int i = 0; i <= U_MAX; i++) {
        evalOut << std::to_string(i) << "," << std::to_string(get(out, i)) << "," << std::to_string(get(tab.mTableOut, i)) << "," << format_binary(get(out, i)) << "," << format_binary(get(tab.mTableOut, i)) << "\n";
    }
    
    int idx = 0;
    for (gate g: mChromosome) {
        g.print(circuitOut);
        idx++;
    }
    circuitOut << "\n";
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
    setUsedTrue(idx);
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
 * \brief Calculate fitness of chromosome as sum of squares
 * \param tab Table of expected values
 */
void chromosome::fitness(table tab) {
    double err = 0;
    for (int i = 0; i < 4; i++) {
        uint64_t out[WORD_LENGTH] = {0};
        simulate(tab.mTableIn[i], out);
        for (UINT j = 0; j < WORD_LENGTH; j++) {
            err += getErr(out[j], tab.mTableOut[i][j], j);
        }
    }

    mErr = err;
    mGates = countUsed();
    mFitness = 1 / (mErr + 1);
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