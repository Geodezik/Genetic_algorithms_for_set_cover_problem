#ifndef GREEDY_H
#define GREEDY_H

#include <vector>
#include "BooleanMatrix.hpp"
#include <random>

namespace Greedy {
    class GreedyAlgorithm;
}

class Greedy::GreedyAlgorithm {
    std::vector<int> columns;

    int get_column_score(BooleanMatrix::BooleanMatrix& M, int j, bool* row_is_covered);
    int get_argmax_score(int *scores, int n);
    void update(BooleanMatrix::BooleanMatrix& M, int j, bool* row_is_covered, int &not_covered_counter);
public:
    GreedyAlgorithm();
    void fit(BooleanMatrix::BooleanMatrix& M);
    void print_solution(BooleanMatrix::BooleanMatrix& M);
};

#endif