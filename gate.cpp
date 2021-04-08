
#include "gate.h"

gate::gate(int position, std::mt19937 aGen) {
    int in_range = position + WORD_LENGTH;
    in_1 = rand() % in_range;
    in_2 = rand() % in_range;
    func = rand() % NUM_FUNCS;
    value = 0;
    used = 0;
    gen = aGen;
}

gate::gate() {
    value = 0;
    err = 0;
    used = 0;
}

gate::gate(gate *g) {
    in_1 = g->in_1;
    in_2 = g->in_2;
    func = g->func;
    value = 0;
    err = 0;
    used = 0;
}

void gate::print(std::ofstream &circuitOut) {
    
    circuitOut << std::to_string(in_1) << "," << std::to_string(in_2) << ",";
    switch (func) {
        case AND:
            circuitOut << "AND";
            break;
        case NAND:
            circuitOut << "NAND";
            break;
        case OR:
            circuitOut << "OR";
            break;
        case NOR:
            circuitOut << "NOR";
            break;
        case XOR:
            circuitOut << "XOR";
            break;
    }
}

uint64_t gate::set_val(uint64_t val) {
    value = val;
    return val;
}

uint64_t gate::exec(uint64_t a, uint64_t b) {
    switch (func) {
        case AND:
            return set_val(a & b);
        case NAND:
            return set_val(~(a & b));
        case OR:
            return set_val(a | b);
        case NOR:
            return set_val(~(a | b));
        case XOR:
            return set_val(a ^ b);
    }
    return 424242424242;
}

void gate::mutate(int position) {
    int max = (position / MAT_HEIGHT) * MAT_HEIGHT  + WORD_LENGTH;
    int min = max - MAT_HEIGHT * L_BACK;
    min = (min < 0) ? 0 : min;

    std::uniform_int_distribution<> distr(min, max - 1);
    switch (rand() % 3)
    {
    case 0:
        in_1 = distr(gen);
        break;
    case 1:
        in_2 = distr(gen);
        break;
    case 2:
        func = rand() % NUM_FUNCS;
        break;
    }
}