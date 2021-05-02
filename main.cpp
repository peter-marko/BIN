#include <iostream>
#include <algorithm>
#include <time.h>
#include <map>

#include "chromosome.h"
#include "params.h"

using namespace std;


table tab = table();

/**
 * \brief Initialize population with chromosomes
 * \param population [out] Empty vector where new chromosomes are written
 */
void create_population(vector<chromosome> *population) {
    for (int i = 0; i < POP_SIZE; i++) {
        chromosome ch;
        ch.fitness(tab);
        population->push_back(ch);
    }
}

void printPopulation(vector<chromosome> &population) {
    for (chromosome ch : population) {
        printf("(%d, %d %g), ", ch.mDomination, ch.mGates, ch.mErr);
    }
    printf("\n");
}

bool dominationCmp(chromosome a, chromosome b) {
    return (a.mDomination < b.mDomination);
}

bool distanceCmp(chromosome a, chromosome b) {
    return (a.mCrowdD > b.mCrowdD);
}

bool gatesCmp(chromosome a, chromosome b) {
    return (a.mGates < b.mGates);
}

bool errCmp(chromosome a, chromosome b) {
    return (a.mErr < b.mErr);
}

void crowdingDistanceAssignment(vector<chromosome> &nonDomSet, double gatesRange, double errRange) {
    size_t size = nonDomSet.size();
    if (size == 0)
        return;
    int end = size - 1;

    // objective mGates
    sort(nonDomSet.begin(), nonDomSet.end(), gatesCmp);
    nonDomSet[0].mCrowdD = INFINITY;
    nonDomSet[end].mCrowdD = INFINITY;
    for (int i = 1; i < end; i++) {
        // double prev = nonDomSet[i].mCrowdD;
        nonDomSet[i].mCrowdD += (nonDomSet[i + 1].mGates - nonDomSet[i - 1].mGates) / gatesRange;
    }
    
    // objective mErr
    sort(nonDomSet.begin(), nonDomSet.end(), errCmp);
    for (int i = 1; i < end; i++) {
        // double prev = nonDomSet[i].mCrowdD;
        nonDomSet[i].mCrowdD += (nonDomSet[i + 1].mErr - nonDomSet[i - 1].mErr) / errRange;
        // printf(" %g + (%g - %g) / %g = %g\n", prev, nonDomSet[i + 1].mErr, nonDomSet[i - 1].mErr, errRange, nonDomSet[i].mCrowdD);
    }
}

void setMinMax(double cur, double &min, double &max) {
    min = (cur < min) ? cur : min;
    max = (cur > max) ? cur : max;
}

map<int, vector<chromosome>> nonDominatedSort(vector<chromosome> &aPopulation,
    double &gatesRange, double &errRange)
{
    map<int, vector<chromosome>> nonDomSets;
    double minGates = INFINITY;
    double maxGates = 0;
    double minErr = INFINITY;
    double maxErr = 0;
    size_t i = 0;
    size_t tmpPopSize = aPopulation.size();
    int deduplicitSize = 5;
    bool allGates[deduplicitSize][MAT_HEIGHT * MAT_WIDTH] = {false};
    for (chromosome &ch : aPopulation) {
        ch.init();
        setMinMax(ch.mGates, minGates, maxGates);
        setMinMax(ch.mErr, minErr, maxErr);
        for (size_t j = 0; j < tmpPopSize; j++) {
            if (i != j) {
                ch.dominated(aPopulation[j]);
            }
        }
        int dominated = ch.mDomination;
        if ( nonDomSets.find(dominated) == nonDomSets.end() ) {
            nonDomSets[dominated] = vector<chromosome> ();
        }
        if (dominated >= deduplicitSize || !allGates[dominated][ch.mGates]) {
            if (dominated < deduplicitSize) {
                allGates[dominated][ch.mGates] = true;
            }
            nonDomSets[dominated].push_back(ch);
        }
        // else {
        //     printf("ee %d\n", nonDomSets[dominated].size());
        // }
        i++;
    }
    gatesRange = maxGates - minGates;
    errRange = maxErr - minErr;

    return nonDomSets;
}

void createOffspring(vector<chromosome> &aPopulation) {
    for (int i = 0; i < POP_SIZE; i++) {
        int idxA = rand() % POP_SIZE;
        int idxB = rand() % POP_SIZE;
        double r = fRand(0, 1);
        chromosome winner;
        if (aPopulation[idxA].mDomination < aPopulation[idxB].mDomination) {
            winner = (r < P_FIT) ? aPopulation[idxA] : aPopulation[idxB];
        } else {
            winner = (r < P_FIT) ? aPopulation[idxB] : aPopulation[idxA];
        }
        winner.mutate();
        winner.fitness(tab);
        aPopulation.push_back(winner);
    }
}

/**
 * \brief Create new population from existing one using roulette algorithm
 * \param aPopulation [in/out] current population, new population will be stored here
 */
void updatePopulation(vector<chromosome> &aPopulation) {
    createOffspring(aPopulation);
    double gatesRange, errRange;
    map<int, vector<chromosome>> nonDomSets = nonDominatedSort(aPopulation, gatesRange, errRange);
    vector<chromosome> new_pop;
    new_pop.reserve(POP_SIZE);
    for (auto set : nonDomSets) {
        vector<chromosome> &curSet = set.second;
        
        // printf("set %d\n", set.first);
        // for (chromosome ch : curSet) {
        //     printf("(%d, %g %g), ", ch.mDomination, ch.mGates, ch.mErr);
        // }
        // printf("\n");
        if (new_pop.size() + curSet.size() <= POP_SIZE) {
            new_pop.insert(new_pop.end(), curSet.begin(), curSet.end());
        } else {
            crowdingDistanceAssignment(curSet, gatesRange, errRange);
            sort(curSet.begin(), curSet.end(), distanceCmp);
            int dstIdx = new_pop.size();
            for (chromosome ch : curSet) {
                if (dstIdx >= POP_SIZE)
                    break;
                new_pop[dstIdx] = ch;
                dstIdx++;
            }
            break;
        }
    }
    
    
    for (int i = 0; i < POP_SIZE; i++) {
        aPopulation[i] = new_pop[i];
    }
    aPopulation.erase(aPopulation.begin() + POP_SIZE,aPopulation.end());
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Error : missing argument\n usage ./exec /path/to/eval_out.csv /path/to/circuit_out.csv\n");
        return(1);
    }
    srand( (unsigned)time(NULL) );
    vector<chromosome> population;
    create_population(&population);
    double gatesRange, errRange;
    map<int, vector<chromosome>> nonDomSets = nonDominatedSort(population, gatesRange, errRange);
    
    for (int i = 0; i < ITERATIONS; i++) {
        updatePopulation(population);
        if (i % 1000 == 0) {
            printPopulation(population);
        }
    }
    printPopulation(population);
    
    
    std::ofstream evalOut;
    evalOut.open(argv[1], std::ios::out | std::ios::trunc );
    std::ofstream circuitOut;
    circuitOut.open(argv[2], std::ios::out | std::ios::trunc );
    for (chromosome ch : population) {
        ch.print(evalOut, circuitOut, tab);
    }    
}