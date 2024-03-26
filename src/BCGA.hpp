#ifndef BCGA_H
#define BCGA_H

#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <random>
#include <numeric>
#include <algorithm>
#include <ctime>
#include <chrono>
#include <stdexcept>
#include <boost/dynamic_bitset.hpp>
#include "BooleanMatrix.hpp"

namespace BCGA {
    class BaseBCGA;
    class SotnezovBCGA;
    class CODE3;
    class GENCODE;
    class GENCODE_plus;
    class BinaryIndividual;

    enum class OutputMode {Silent, Normal, Max};
    enum class Fitness {CovLen, MaxBinsNum, Mixed};
    enum class RankType{ElementWise, GroupWise, Sum};

    namespace GlobalSettings {
        #include "GlobalSettings.cfg"
    }
    std::ostream& operator<<(std::ostream& os, const BinaryIndividual& I);
};

class BCGA::BinaryIndividual {
protected:
    bool zero_gen;
public:
    boost::dynamic_bitset<> genotype;

    BinaryIndividual(boost::dynamic_bitset<>& genotype, bool zero_gen=false);

    int size();
    bool is_from_zero_gen();

    friend BCGA::BaseBCGA;
    friend std::ostream& BCGA::operator<<(std::ostream& os, const BCGA::BinaryIndividual& individual);
};

class BCGA::BaseBCGA {
protected:
    std::vector<BinaryIndividual> population;
    std::vector<double> scores;
    std::mt19937 rng;

    int max_iter;
    int iteration;
    int population_size;
    int extended_population_size;
    //int best_score = -1;
    int best_index = -1;
    bool is_fitted = false;
    double mutation_proba;

    int m;
    int n;

    double fit_time;
    OutputMode verbose = OutputMode::Normal;

    // TO IMPLEMENT
    virtual void create_zero_generation(BooleanMatrix::BooleanMatrix& M, int genotype_len) = 0;
    virtual void get_parent_indices(int& p1, int& p2) = 0;
    virtual BinaryIndividual crossover(BinaryIndividual& parent1, BinaryIndividual& parent2) = 0;
    virtual void mutate(BooleanMatrix::BooleanMatrix& M, double mutation_proba) = 0;
    virtual double fitness(BooleanMatrix::BooleanMatrix& M, BinaryIndividual& individual) = 0;
    virtual void selection() = 0;

    virtual void update_scores(BooleanMatrix::BooleanMatrix& M);
    virtual void check_compatibility() {};
public:
    double best_score = -1;

    BaseBCGA(int population_size, int extended_population_size, double mutation_proba, int max_iter = 100,
             int seed = -1, OutputMode verbose = OutputMode::Normal);

    void fit(BooleanMatrix::BooleanMatrix& M);
    void print_stats(std::vector<int>& argbest);

    BinaryIndividual& get_best_individual();
    void print_solution(std::string filename);

    virtual void analyze_solution(BooleanMatrix::BooleanMatrix& M) {};

    ~BaseBCGA() {};
};

class BCGA::SotnezovBCGA: public BCGA::BaseBCGA {
protected:
    std::vector<int> column_scores;
    std::vector<int> apriori_queue;
    double scores_sum = 0;

    int K = 100;
    double C = 0.01;

    std::vector<int> argsort(const std::vector<int> &v);

    void create_zero_generation(BooleanMatrix::BooleanMatrix& M, int genotype_len);
    void get_parent_indices(int& p1, int& p2);
    BinaryIndividual crossover(BinaryIndividual& parent1, BinaryIndividual& parent2);
    void mutate(BooleanMatrix::BooleanMatrix& M, double mutation_proba);
    double fitness(BooleanMatrix::BooleanMatrix& M, BinaryIndividual& individual);
    void selection();

    virtual void optimize_covering(BooleanMatrix::BooleanMatrix& M, boost::dynamic_bitset<>& columns);
    virtual std::vector<bool> get_covered_rows(BooleanMatrix::BooleanMatrix& M, boost::dynamic_bitset<>& columns);
    virtual void restore_solution(BooleanMatrix::BooleanMatrix& M, boost::dynamic_bitset<>& columns);
    virtual void add_maxscore_column(BooleanMatrix::BooleanMatrix& M, std::vector<bool>& covered_rows, boost::dynamic_bitset<>& columns, int row, int from, int to);
public:
    SotnezovBCGA(int population_size, int K = 100, double C = 0.01, int max_iter = 100, int seed = -1, OutputMode verbose = OutputMode::Normal);
    void print_columns_to_file(std::string filename);
};

class BCGA::GENCODE: public BCGA::SotnezovBCGA {
protected:
    Fitness fit_function = Fitness::CovLen;
    std::vector<int> groups_idx;
    std::vector<int> group_counters;
    std::vector<int> columns_groups;

    std::vector<int> conditional_argsort(const std::vector<int> &v, const std::vector<double> &c);
    std::vector<int> enc_conditional_argsort(const std::vector<int> &v);

    void check_compatibility();
    void fill_counters(BooleanMatrix::BooleanMatrix& M, boost::dynamic_bitset<>& columns);

    std::vector<double> build_decreasing_counters(boost::dynamic_bitset<>& columns, std::vector<int>& columns_argsort);
    void optimize_covering(BooleanMatrix::BooleanMatrix& M, boost::dynamic_bitset<>& columns);
    void restore_solution(BooleanMatrix::BooleanMatrix& M, boost::dynamic_bitset<>& columns);
    double covlen_fitness(BooleanMatrix::BooleanMatrix& M, BinaryIndividual& individual);
    double maxbinsnum_fitness(BooleanMatrix::BooleanMatrix& M, BinaryIndividual& individual);
    double mixed_fitness(BooleanMatrix::BooleanMatrix& M, BinaryIndividual& individual);
    double fitness(BooleanMatrix::BooleanMatrix& M, BinaryIndividual& individual);
public:
    GENCODE(int population_size, std::vector<int> groups_idx, Fitness optimize = Fitness::CovLen, int K = 100, double C = 0.01,
                         int max_iter = 100, int seed = -1,  OutputMode verbose = OutputMode::Normal);

    void analyze_solution(BooleanMatrix::BooleanMatrix& M);
};

class BCGA::CODE3: public BCGA::GENCODE {
protected:
    void optimize_covering(BooleanMatrix::BooleanMatrix& M, boost::dynamic_bitset<>& columns);
    double fitness(BooleanMatrix::BooleanMatrix& M, BinaryIndividual& individual);

public:
    CODE3(int population_size, std::vector<int> groups_idx, Fitness optimize = Fitness::CovLen, int K = 100, double C = 0.01,
                         int max_iter = 100, int seed = -1,  OutputMode verbose = OutputMode::Normal);
};

class BCGA::GENCODE_plus: public BCGA::GENCODE {
    std::vector<double> columns_ranks;
    std::vector<double> individual_ranks;

    RankType rank_type;
    double best_rank;
    double alpha;
    int norank_iter;

    BinaryIndividual crossover(BinaryIndividual& parent1, BinaryIndividual& parent2);
    double rank(BinaryIndividual& individual);

    void update_scores(BooleanMatrix::BooleanMatrix& M);
    void create_zero_generation(BooleanMatrix::BooleanMatrix& M, int genotype_len);
    void selection();
public:
    GENCODE_plus(int population_size, std::vector<int> groups_idx, std::vector<double> ranks, RankType rank_type = RankType::ElementWise, Fitness optimize = Fitness::CovLen,
                     int K = 100, double C = 0.01, double alpha = 0.2, int max_iter = 100, int norank_iter = 0, int seed = -1,  OutputMode verbose = OutputMode::Normal);
};

#endif
