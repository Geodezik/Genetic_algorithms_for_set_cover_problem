#ifndef GREEDY_H
#define GREEDY_H

#include <vector>
#include <ctime>
#include <fstream>
#include <random>
#include "BooleanMatrix.hpp"

namespace Greedy {
    class GreedyAlgorithm;
}

class Greedy::GreedyAlgorithm {
    std::vector<int> columns;
    double fit_time;

    int get_column_score(BooleanMatrix::BooleanMatrix& M, int j, bool* row_is_covered);
    int get_argmax_score(int *scores, int n);
    void update(BooleanMatrix::BooleanMatrix& M, int j, bool* row_is_covered, int &not_covered_counter);
public:
    GreedyAlgorithm();
    void fit(BooleanMatrix::BooleanMatrix& M);
    void print_solution(BooleanMatrix::BooleanMatrix& M);
    void analyze();
    void print_fit_stats(BooleanMatrix::BooleanMatrix& M, std::string filename="results.txt");
};

#endif