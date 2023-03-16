#ifndef GENETIC_H
#define GENETIC_H

#include <iostream>
#include <fstream>
#include <vector>
#include <random>
#include <numeric>
#include <algorithm>
#include <ctime>
#include "BooleanMatrix.hpp"

namespace Genetic {
    class Individual;
    class GeneticAlgorithm;
    std::ostream& operator<<(std::ostream& os, const Individual& I);
};

class Genetic::Individual {
    std::vector<bool> genotype;
    bool first_gen;

public:
    Individual(std::vector<bool> genotype, bool first_gen=false, double p=1.0);

    int size();
    bool is_coverage(BooleanMatrix::BooleanMatrix& M);
    double fitness(BooleanMatrix::BooleanMatrix& M);

    friend Genetic::GeneticAlgorithm;
    friend std::ostream& Genetic::operator<<(std::ostream& os, const Genetic::Individual& I);
};

class Genetic::GeneticAlgorithm {
    std::vector<Individual> population;
    std::vector<int> scores;

    std::mt19937 rng;

    int max_iter;
    int population_size;
    int extended_population_size;
    double mutation_proba;

    double fit_time;

    template <typename T>
    std::vector<int> argsort(const std::vector<T> &v);

public:
    GeneticAlgorithm(int population_size, int extended_population_size, int genotype_len,
                     double mutation_proba, int max_iter = 100, std::string task = "min_bool_coverings_1");
    Individual one_point_crossover(Individual s1, Individual s2);
    void fit(BooleanMatrix::BooleanMatrix& M, int verbose = 2, bool finishing_message = true);
    std::vector<bool> get_best_individual();
    void print_individuals();
    void print_solution(BooleanMatrix::BooleanMatrix& M);
    void analyze_solution(BooleanMatrix::BooleanMatrix& M);
    void print_fit_stats(BooleanMatrix::BooleanMatrix& M, std::string filename="results.txt");
};

#endif
