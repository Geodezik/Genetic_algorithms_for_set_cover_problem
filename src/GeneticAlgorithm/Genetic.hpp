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
    class BaseGeneticAlgorithm;
    class CoverageGeneticAlgorithm;

    class BaseIndividual;
    class CoverageIndividual;
    std::ostream& operator<<(std::ostream& os, const BaseIndividual& I);
};

class Genetic::BaseIndividual {
    bool zero_gen;
public:
    std::vector<bool> genotype;

    BaseIndividual(std::vector<bool>& genotype, bool first_gen=false, double p=1.0);

    int size();
    bool is_coverage(BooleanMatrix::BooleanMatrix& M);
    bool is_from_zero_gen();
    double fitness(BooleanMatrix::BooleanMatrix& M);

    friend Genetic::BaseGeneticAlgorithm;
    friend std::ostream& Genetic::operator<<(std::ostream& os, const Genetic::BaseIndividual& I);
};

class Genetic::CoverageIndividual: public BaseIndividual {
public:
    CoverageIndividual(std::vector<bool>& genotype, bool first_gen=false, double p=1.0);
};

class Genetic::BaseGeneticAlgorithm {
protected:
    std::vector<BaseIndividual> population;
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
    BaseGeneticAlgorithm(int population_size, int extended_population_size,
                     double mutation_proba, int max_iter = 100, std::string task = "min_bool_coverings_1");
    BaseIndividual one_point_crossover(BaseIndividual s1, BaseIndividual s2);
    void fit(BooleanMatrix::BooleanMatrix& M, int verbose = 2, bool finishing_message = true);
    void print_stats(std::vector<double>& scores, std::vector<int>& argbest, int iteration, int verbose);

    // TO IMPLEMENT
    virtual void create_zero_generation(int genotype_len) = 0;
    virtual BaseIndividual crossover(BaseIndividual& parent1, BaseIndividual& parent2) = 0;
    virtual void mutate(std::vector<BaseIndividual>& individual_vector, double mutation_proba, int parameter) = 0;
    virtual void selection(std::vector<BaseIndividual>& extended_population, std::vector<double>& scores, int iteration, int verbose) = 0;

    BaseIndividual& get_best_individual();
    void print_individuals();
    void print_solution(BooleanMatrix::BooleanMatrix& M);
    void analyze_solution(BooleanMatrix::BooleanMatrix& M);
    void print_fit_stats(BooleanMatrix::BooleanMatrix& M, std::string filename="results.txt");
};

class Genetic::CoverageGeneticAlgorithm: public Genetic::BaseGeneticAlgorithm {
public:
    CoverageGeneticAlgorithm(int population_size, int extended_population_size,
                     double mutation_proba, int max_iter = 100, std::string task = "min_bool_coverings_1");

    void create_zero_generation(int genotype_len);
    BaseIndividual crossover(BaseIndividual& parent1, BaseIndividual& parent2);
    void mutate(std::vector<BaseIndividual>& individual_vector, double mutation_proba, int parameter);
    void selection(std::vector<BaseIndividual>& extended_population, std::vector<double>& scores, int iteration, int verbose);
};

#endif
