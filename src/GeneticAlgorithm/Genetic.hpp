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

namespace BCGA {
    class BaseBCGA;
    class SotnezovBCGA;
    class BinaryIndividual;
    enum class OutputMode {Silent, Normal, Max};
    namespace GlobalSettings {
        #include "GlobalSettings.cfg"
    }
    std::ostream& operator<<(std::ostream& os, const BinaryIndividual& I);
};

class BCGA::BinaryIndividual {
    bool zero_gen;
public:
    std::vector<bool> genotype;

    BinaryIndividual(std::vector<bool>& genotype, bool zero_gen=false);

    int size();
    bool is_from_zero_gen();

    friend BCGA::BaseBCGA;
    friend std::ostream& BCGA::operator<<(std::ostream& os, const BCGA::BinaryIndividual& individual);
};

class BCGA::BaseBCGA {
protected:
    std::vector<BinaryIndividual> population;
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
    OutputMode verbose = OutputMode::Normal;

    template <typename T>
    std::vector<int> argsort(const std::vector<T> &v);

public:
    BaseBCGA(int population_size, int extended_population_size, double mutation_proba, int max_iter = 100,
             int seed = -1, OutputMode verbose = OutputMode::Normal);
    void fit(BooleanMatrix::BooleanMatrix& M);
    void print_stats(std::vector<int>& argbest, int iteration);

    // TO IMPLEMENT
    virtual void create_zero_generation(BooleanMatrix::BooleanMatrix& M, int genotype_len) = 0;
    virtual void get_parent_indices(int& p1, int& p2) = 0;
    virtual BinaryIndividual crossover(BinaryIndividual& parent1, BinaryIndividual& parent2) = 0;
    virtual void mutate(BooleanMatrix::BooleanMatrix& M, double mutation_proba, int parameter) = 0;
    virtual double fitness(BooleanMatrix::BooleanMatrix& M, BinaryIndividual& individual) = 0;
    virtual void selection(int iteration) = 0;

    BinaryIndividual& get_best_individual();
    void create_matrix(int m, int n);
    void print_individuals();
    void print_solution(BooleanMatrix::BooleanMatrix& M);
    void analyze_solution(BooleanMatrix::BooleanMatrix& M);
    void analyze_alikeness(int t);
    void print_fit_stats(BooleanMatrix::BooleanMatrix& M, std::string filename = GlobalSettings::default_out_filename);

    ~BaseBCGA() {};
};

class BCGA::SotnezovBCGA: public BCGA::BaseBCGA {
    int scores_sum = 0;
    int unluck_counter = 0;

    int K = 100;
    float C = 0.01;
public:
    SotnezovBCGA(int population_size, int K = 100, float C = 0.01, int max_iter = 100, int seed = -1, OutputMode verbose = OutputMode::Normal);

    void optimize_covering(BooleanMatrix::BooleanMatrix& M, std::vector<bool>& columns);
    std::vector<bool> get_covered_rows(BooleanMatrix::BooleanMatrix& M, std::vector<bool> columns);
    int get_maxscore_column(BooleanMatrix::BooleanMatrix& M, std::vector<bool>& covered_rows, std::vector<bool>& columns, int row);

    void create_zero_generation(BooleanMatrix::BooleanMatrix& M, int genotype_len);
    void get_parent_indices(int& p1, int& p2);
    BinaryIndividual crossover(BinaryIndividual& parent1, BinaryIndividual& parent2);
    void mutate(BooleanMatrix::BooleanMatrix& M, double mutation_proba, int parameter);
    double fitness(BooleanMatrix::BooleanMatrix& M, BinaryIndividual& individual);
    void selection(int iteration);
};

#endif