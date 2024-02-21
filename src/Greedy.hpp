#ifndef GREEDY_H
#define GREEDY_H

#include <vector>
#include <map>
#include <ctime>
#include <fstream>
#include <random>
#include <algorithm>
#include <boost/dynamic_bitset.hpp>
#include "BooleanMatrix.hpp"

namespace Greedy {
    class GreedyAlgorithm;
    class EncodingGreedyAlgorithm;
}

class Greedy::GreedyAlgorithm {
protected:
    boost::dynamic_bitset<> columns;
    double fit_time;

    int get_column_score(BooleanMatrix::BooleanMatrix& M, int j, boost::dynamic_bitset<>& row_is_covered);
    int get_argmax_score(std::vector<int> &scores, int n);
    void update(BooleanMatrix::BooleanMatrix& M, int j, boost::dynamic_bitset<>& row_is_covered, int &not_covered_counter);

public:
    void fit(BooleanMatrix::BooleanMatrix& M);
    void analyze();
    void print_solution(std::string filename, int n);
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