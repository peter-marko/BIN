#include <iostream>
#include <algorithm>
#include <time.h>

#include "chromosome.h"
#include "params.h"

using namespace std;

/**
 * \brief Initialize population with chromosomes
 * \param population [out] Empty vector where new chromosomes are written
 */
void create_population(vector<chromosome> *population) {
    for (int i = 0; i < POP_SIZE; i++) {
        population->push_back(chromosome());
    }
}

/**
 * \brief Calculate fitness of each memeber of population
 * \param population [in/out] Chromosomes which fitness should be calculated
 * \param tab Table of correct results e.g for sin(x)
 */
void population_fitness(vector<chromosome> *population, table tab) {
    for (UINT i = 0; i < population->size(); i++) {
        population->at(i).fitness(tab);
    }
}

/**
 * \brief Generate and return random double in range fMin, fMax
 */
double fRand(double fMin, double fMax) {
    double f = (double)rand() / RAND_MAX;
    return fMin + f * (fMax - fMin);
}

/**
 * \brief Stochastic algorithm for selection of good chromosomes
 * \param aPopulation [in] Current population
 * \param total_fitness Sum of all fitness values from population
 */
chromosome roulette(vector<chromosome> *aPopulation, double total_fitness) {
    double r = fRand(0, total_fitness);
    double fitness_sum = 0;
    for (int i = 0; i < POP_SIZE; i++) {
        fitness_sum += aPopulation->at(i).mFitness;
        if (r < fitness_sum) {
            return(aPopulation->at(i));
        }
    }
    return(aPopulation->at(0));
}

/**
 * \brief Create new population from existing one using roulette algorithm
 * \param aPopulation [in/out] current population, new population will be stored here
 * \param aTotalBest best chromosome evere seen
 * \param aTab Table of correct results e.g. for sin(x)
 * \return Fitness of best chromosome in current population
 */
double updatePopulation(vector<chromosome> * aPopulation, chromosome *aTotalBest, table aTab) {
    vector<chromosome> new_pop;
    double total_fitness = 0;
    chromosome best = *aPopulation->begin();
    double total_cost = 0;
    double total_gate_cnt = 0;
    for (chromosome ch : *aPopulation) {
        if (ch.mFitness > best.mFitness) {
            best = ch;
        }
        total_fitness += ch.mFitness;
        total_cost += ch.mCost;
        total_gate_cnt += ch.gate_cnt();
    }
    if (best.mFitness > aTotalBest->mFitness) {
        *aTotalBest = best;
    }
    printf("%g,%g,%u,%g\n", best.mCost, total_cost / POP_SIZE, best.gate_cnt(), total_gate_cnt / POP_SIZE);
    if (best.mFitness > 0.9) {
        return best.mFitness;
    }
    
    for (int i = 0; i < POP_SIZE - ELITISM; i++) {
        chromosome ch = roulette(aPopulation, total_fitness);
        ch.mutate();
        new_pop.push_back(ch);
    }
    for (int i = 0; i < ELITISM; i++) {
        chromosome bestChild(best);
        bestChild.mutate();
        new_pop.push_back(bestChild);
    }
    population_fitness(& new_pop, aTab);
    *aPopulation = new_pop;
    return best.mFitness;
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Error : missing argument\n usage ./exec /path/to/eval_out.csv /path/to/circuit_out.csv\n");
        return(1);
    }
    srand( (unsigned)time(NULL) );
    vector<chromosome> population;
    create_population(&population);
    chromosome best_chromosome = population[0];

    table tab = table();
    
    for (int iteration = 0; iteration < ITERATIONS; iteration++) {
        population_fitness(&population, tab);
        updatePopulation(&population, &best_chromosome, tab);
    }
    best_chromosome.print(argv[1], argv[2], tab);
    
}