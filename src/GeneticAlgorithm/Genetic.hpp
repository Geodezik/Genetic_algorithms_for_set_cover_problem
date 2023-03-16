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
    class BaseGeneticAlgorithm;
    class CoverageGeneticAlgorithm;
    std::ostream& operator<<(std::ostream& os, const Individual& I);
};

class Genetic::Individual {
    std::vector<bool> genotype;
    bool first_gen;

public:
    Individual(std::vector<bool>& genotype, bool first_gen=false, double p=1.0);

    int size();
    bool is_coverage(BooleanMatrix::BooleanMatrix& M);
    double fitness(BooleanMatrix::BooleanMatrix& M);

    friend Genetic::BaseGeneticAlgorithm;
    friend std::ostream& Genetic::operator<<(std::ostream& os, const Genetic::Individual& I);
};

class Genetic::BaseGeneticAlgorithm {
    std::vector<Individual> population;
    std::vector<int> scores;

    std::mt19937 rng;

    int max_iter;
    int population_size;
    int extended_population_size;
    double mutation_proba;

    int m;
    int n;

    double fit_time;

    template <typename T>
    std::vector<int> argsort(const std::vector<T> &v);

public:
    BaseGeneticAlgorithm(int population_size, int extended_population_size, int genotype_len,
                     double mutation_proba, int max_iter = 100, std::string task = "min_bool_coverings_1");
    Individual one_point_crossover(Individual s1, Individual s2);
    void fit(BooleanMatrix::BooleanMatrix& M, int verbose = 2, bool finishing_message = true);
    void print_stats(std::vector<double>& scores, std::vector<int>& argbest, int iteration, int verbose);

    // TO IMPLEMENT
    void create_zero_generation(int genotype_len);
    Individual crossover(Individual& parent1, Individual& parent2);
    void mutate(std::vector<Individual>& individual_vector, double mutation_proba, int parameter);
    void selection(std::vector<Individual>& extended_population, std::vector<double>& scores, int iteration, int verbose);

    Individual& get_best_individual();
    void print_individuals();
    void print_solution(BooleanMatrix::BooleanMatrix& M);
    void analyze_solution(BooleanMatrix::BooleanMatrix& M);
    void print_fit_stats(BooleanMatrix::BooleanMatrix& M, std::string filename="results.txt");
};

#endif
