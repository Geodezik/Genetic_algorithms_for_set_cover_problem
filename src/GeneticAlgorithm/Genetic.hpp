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
    double mutation_proba;

    int m;
    int n;

    double fit_time;

    template <typename T>
    std::vector<int> argsort(const std::vector<T> &v);

public:
    BaseGeneticAlgorithm(int population_size, int extended_population_size, double mutation_proba, int max_iter = 100);
    void fit(BooleanMatrix::BooleanMatrix& M, int verbose = 2, bool finishing_message = true);
    void print_stats(std::vector<double>& scores, std::vector<int>& argbest, int iteration, int verbose);

    // TO IMPLEMENT
    virtual void create_zero_generation(int genotype_len) = 0;
    virtual Individual crossover(Individual& parent1, Individual& parent2) = 0;
    virtual void mutate(std::vector<Individual>& individual_vector, double mutation_proba, int parameter) = 0;
    virtual double fitness(Individual& individual, BooleanMatrix::BooleanMatrix& M) = 0;
    virtual void selection(std::vector<Individual>& extended_population, std::vector<double>& scores, int iteration, int verbose) = 0;

    Individual& get_best_individual();
    void print_individuals();
    void print_solution(BooleanMatrix::BooleanMatrix& M);
    void analyze_solution(BooleanMatrix::BooleanMatrix& M);
    void print_fit_stats(BooleanMatrix::BooleanMatrix& M, std::string filename="results.txt");
};

class Genetic::CoverageGeneticAlgorithm: public Genetic::BaseGeneticAlgorithm {
public:
    CoverageGeneticAlgorithm(int population_size, int extended_population_size, double mutation_proba, int max_iter = 100);

    void create_zero_generation(int genotype_len);
    Individual crossover(Individual& parent1, Individual& parent2);
    void mutate(std::vector<Individual>& individual_vector, double mutation_proba, int parameter);
    double fitness(Individual& individual, BooleanMatrix::BooleanMatrix& M);
    void selection(std::vector<Individual>& extended_population, std::vector<double>& scores, int iteration, int verbose);
};

//TODO: Sotnezov genetic algorithm

#endif