#ifndef GENETIC_H
#define GENETIC_H

#include <iostream>
#include <fstream>
#include <vector>
#include <random>
#include <numeric>
#include <algorithm>
#include <ctime>
#include <stdexcept>
#include "BooleanMatrix.hpp"

namespace Genetic {
    class BaseGeneticAlgorithm;
    class CoverageGeneticAlgorithm;
    class SotnezovGeneticAlgorithm;

    class Individual;
    std::ostream& operator<<(std::ostream& os, const Individual& I);
};

class Genetic::Individual {
    bool zero_gen;
public:
    std::vector<bool> genotype;

    Individual(std::vector<bool>& genotype, bool zero_gen=false);

    int size();
    bool is_from_zero_gen();

    friend Genetic::BaseGeneticAlgorithm;
    friend std::ostream& Genetic::operator<<(std::ostream& os, const Genetic::Individual& I);
};

class Genetic::BaseGeneticAlgorithm {
protected:
    std::vector<Individual> population;
    std::vector<int> scores;

    std::mt19937 rng;

    int max_iter;
    int population_size;
    int extended_population_size;
    int best_score = -1;
    int best_index = -1;
    double mutation_proba;

    int m;
    int n;

    double fit_time;

    template <typename T>
    std::vector<int> argsort(const std::vector<T> &v);

public:
    BaseGeneticAlgorithm(int population_size, int extended_population_size, double mutation_proba, int max_iter = 100);
    void fit(BooleanMatrix::BooleanMatrix& M, int verbose = 2, bool finishing_message = true);
    void print_stats(std::vector<int>& argbest, int iteration, int verbose);

    // TO IMPLEMENT
    virtual void create_zero_generation(BooleanMatrix::BooleanMatrix& M, int genotype_len) = 0;
    virtual void get_parent_indices(int& p1, int& p2) = 0;
    virtual Individual crossover(Individual& parent1, Individual& parent2) = 0;
    virtual void mutate(BooleanMatrix::BooleanMatrix& M, double mutation_proba, int parameter) = 0;
    virtual double fitness(BooleanMatrix::BooleanMatrix& M, Individual& individual) = 0;
    virtual void selection(int iteration, int verbose) = 0;

    Individual& get_best_individual();
    void create_matrix(int m, int n);
    void print_individuals();
    void print_solution(BooleanMatrix::BooleanMatrix& M);
    void analyze_solution(BooleanMatrix::BooleanMatrix& M);
    void analyze_alikeness(int t);
    void print_fit_stats(BooleanMatrix::BooleanMatrix& M, std::string filename="results.txt");

    ~BaseGeneticAlgorithm() {};
};

class Genetic::CoverageGeneticAlgorithm: public Genetic::BaseGeneticAlgorithm {
public:
    CoverageGeneticAlgorithm(int population_size, int extended_population_size, double mutation_proba, int max_iter = 100);

    void create_zero_generation(BooleanMatrix::BooleanMatrix& M, int genotype_len);
    void get_parent_indices(int& p1, int& p2);
    Individual crossover(Individual& parent1, Individual& parent2);
    void mutate(BooleanMatrix::BooleanMatrix& M, double mutation_proba, int parameter);
    double fitness(BooleanMatrix::BooleanMatrix& M, Individual& individual);
    void selection(int iteration, int verbose);
};

class Genetic::SotnezovGeneticAlgorithm: public Genetic::BaseGeneticAlgorithm {
    int scores_sum = 0;
    int unluck_counter = 0;
public:
    SotnezovGeneticAlgorithm(int population_size, int max_iter = 100);

    void optimize_covering(BooleanMatrix::BooleanMatrix& M, std::vector<bool>& columns);
    std::vector<bool> get_covered_rows(BooleanMatrix::BooleanMatrix& M, std::vector<bool> columns);
    int get_maxscore_column(BooleanMatrix::BooleanMatrix& M, std::vector<bool>& covered_rows, std::vector<bool>& columns, int row);

    void create_zero_generation(BooleanMatrix::BooleanMatrix& M, int genotype_len);
    void get_parent_indices(int& p1, int& p2);
    Individual crossover(Individual& parent1, Individual& parent2);
    void mutate(BooleanMatrix::BooleanMatrix& M, double mutation_proba, int parameter);
    double fitness(BooleanMatrix::BooleanMatrix& M, Individual& individual);
    void selection(int iteration, int verbose);
};

#endif