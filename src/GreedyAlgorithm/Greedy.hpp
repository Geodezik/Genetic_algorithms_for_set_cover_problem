#ifndef GREEDY_H
#define GREEDY_H

#include <vector>
#include <map>
#include <ctime>
#include <fstream>
#include <random>
#include <algorithm>
#include "BooleanMatrix.hpp"

namespace Greedy {
    class GreedyAlgorithm;
    class EncodingGreedyAlgorithm;
}

class Greedy::GreedyAlgorithm {
protected:
    std::vector<bool> columns;
    double fit_time;

    int get_column_score(BooleanMatrix::BooleanMatrix& M, int j, std::vector<bool> &row_is_covered);
    int get_argmax_score(std::vector<int> &scores, int n);
    void update(BooleanMatrix::BooleanMatrix& M, int j, std::vector<bool> &row_is_covered, int &not_covered_counter);

public:
    void fit(BooleanMatrix::BooleanMatrix& M);
    void print_solution(BooleanMatrix::BooleanMatrix& M);
    void analyze();
    void print_fit_stats(BooleanMatrix::BooleanMatrix& M, std::string filename="results.txt");
};

class Greedy::EncodingGreedyAlgorithm: public Greedy::GreedyAlgorithm {
protected:
    std::vector<int> features;
    std::map<int, int> group_counters;

    void clear_counters();
    void optimize_covering(BooleanMatrix::BooleanMatrix& M);

    template <typename T>
    std::vector<int> argsort(const std::vector<T> &v);
public:
    EncodingGreedyAlgorithm(std::vector<int> features);
    void fit(BooleanMatrix::BooleanMatrix& M);
    void analyze();
};

#endif