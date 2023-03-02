#include "Genetic.hpp"
#include <vector>

namespace Greedy {
    class GreedyAlgorithm;
}

class Greedy::GreedyAlgorithm {
    std::vector<int> columns;
public:
    GreedyAlgorithm();
    void fit(BooleanMatrix::BooleanMatrix& M);
}