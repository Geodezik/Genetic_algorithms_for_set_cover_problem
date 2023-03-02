#include "Genetic.hpp"
#include <vector>

namespace Greedy {
    class GreedyAlgorithm;
}

class Greedy::GreedyAlgorithm {
    std::vector<int> columns;

    int get_column_score(BooleanMatrix::BooleanMatrix& M, int j);
    int get_argmax_score(int *scores, int n);
public:
    GreedyAlgorithm();
    void fit(BooleanMatrix::BooleanMatrix& M);
}