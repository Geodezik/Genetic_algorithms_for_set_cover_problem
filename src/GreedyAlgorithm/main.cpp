#include "Greedy.hpp"
#include "BooleanMatrix.hpp"
#include <iostream>

using namespace Greedy;

int main()
{
    int SEED = 318;
    int m = 2500;
    int n = 2500;

    double p = 0.001;
    std::random_device rd{};
    std::mt19937 rng{rd()};
    rng.seed(SEED);
    std::bernoulli_distribution d(p);

    bool Matrix[m][n] = {
        {0, 1, 1, 1, 1, 0, 1, 0, 1, 0},
        {1, 0, 1, 0, 0, 0, 1, 0, 1, 0},
        {0, 0, 1, 0, 0, 1, 1, 0, 1, 0},
        {0, 1, 1, 1, 1, 0, 0, 0, 1, 0},
        {0, 0, 1, 0, 1, 0, 1, 0, 0, 0},
        {0, 0, 1, 1, 1, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 1, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 1, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 1, 0, 0, 1},
        {0, 0, 0, 0, 0, 0, 0, 1, 0, 1}
    };

    BooleanMatrix::BooleanMatrix M(m, n);
    for(int i = 0; i < m; i++) {
        for(int j = 0; j < n; j++) {
            //M[i][j] = static_cast<bool> (d(rng));
            //M[i][j] = Matrix[i][j];
            M[i][j] = (i == j);
        }
    }

    GreedyAlgorithm A = GreedyAlgorithm();
    A.fit(M);
    //A.print_solution(M);

    return 0;
}