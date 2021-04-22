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
void create_population(chromosome population[POP_SIZE]) {
    for (int i = 0; i < POP_SIZE; i++) {
        population[i] = chromosome();
    }
}

/**
 * \brief Calculate fitness of each memeber of population
 * \param population [in/out] Chromosomes which fitness should be calculated
 * \param tab Table of correct results e.g for sin(x)
 */
void population_fitness(chromosome population[POP_SIZE], table tab) {
    for (UINT i = 0; i < POP_SIZE; i++) {
        population[i].fitness(tab);
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
chromosome roulette(chromosome aPopulation[POP_SIZE], double total_fitness) {
    double r = fRand(0, total_fitness);
    double fitness_sum = 0;
    for (int i = 0; i < POP_SIZE; i++) {
        fitness_sum += aPopulation[i].mFitness;
        if (r < fitness_sum) {
            return(aPopulation[i]);
        }
    }
    return(aPopulation[0]);
}

/**
 * \brief Create new population from existing one using roulette algorithm
 * \param aPopulation [in/out] current population, new population will be stored here
 * \param aTab Table of correct results e.g. for sin(x)
 * \return Fitness of best chromosome in current population
 */
chromosome updatePopulation(chromosome aPopulation[POP_SIZE], table aTab) {
    double total_fitness = 0;
    chromosome new_pop[POP_SIZE];
    chromosome best = aPopulation[0];
    chromosome prevBest = aPopulation[1];
    double total_cost = 0;
    double total_gate_cnt = 0;
    for (int i = 0; i < POP_SIZE; i++) {
        chromosome ch = aPopulation[i];
        if (ch.mFitness >= best.mFitness && i != 1) {
            prevBest = best;
            best = ch;
            printf("fitness %g\n", best.mFitness);
        }
        total_fitness += ch.mFitness;
        total_cost += ch.mCost;
        total_gate_cnt += ch.gate_cnt();
    }
    if (best.mFitness > 0.9) {
        return best;
    }

    
    new_pop[0] = best;
    new_pop[1] = prevBest;
    // printf("%g, %g,%g,%u,%g\n", aTotalBest->mFitness, new_pop[0].mFitness, total_cost / POP_SIZE, best.gate_cnt(), total_gate_cnt / POP_SIZE);
    for (int i = 2; i < POP_SIZE - 1; i++) {
        chromosome ch = new_pop[0];
        ch.mutate();
        new_pop[i] = ch;
    }
    
    chromosome ch = prevBest;
    ch.mutate();
    new_pop[POP_SIZE - 1] = ch;
    population_fitness(new_pop, aTab);
    memcpy(aPopulation, new_pop, sizeof(chromosome) * POP_SIZE);
    // aPopulation = new_pop;
    return best;
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Error : missing argument\n usage ./exec /path/to/eval_out.csv /path/to/circuit_out.csv\n");
        return(1);
    }
    srand( (unsigned)time(NULL) );
    chromosome population[POP_SIZE];
    create_population(population);
    chromosome best_chromosome = population[0];

    table tab = table();
    
    for (int iteration = 0; iteration < ITERATIONS; iteration++) {
        population_fitness(population, tab);
        best_chromosome = updatePopulation(population, tab);
    }
    best_chromosome.print(argv[1], argv[2], tab);
    
}