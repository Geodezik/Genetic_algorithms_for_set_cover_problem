#include "Greedy.hpp"
#include "BooleanMatrix.hpp"
#include <iostream>

using namespace Greedy;

int main()
{
    int m = 2500;
    int n = 10000;

    double p = 10.0 / m;
    std::random_device rd{};
    std::mt19937 rng{rd()};
    rng.seed(317);
    std::bernoulli_distribution bd(p);
    std::uniform_int_distribution<> uid(1, m - 1);

    /*
    bool Matrix[m][n] = {
        {0, 1, 1, 1, 1, 0, 1, 0, 1, 0},
        {1, 0, 1, 0, 0, 0, 1, 0, 1, 0},
        {0, 0, 1, 0, 0, 1, 1, 0, 1, 0},
        {0, 1, 1, 1, 1, 0, 0, 0, 1, 0},
        {0, 0, 1, 0, 1, 0, 1, 0, 1, 0},
        {0, 0, 1, 1, 1, 0, 0, 0, 1, 0},
        {0, 0, 0, 0, 0, 0, 1, 0, 1, 0},
        {0, 0, 0, 0, 0, 0, 1, 0, 1, 0},
        {0, 0, 0, 0, 0, 0, 1, 0, 1, 0},
        {0, 0, 0, 0, 0, 0, 0, 1, 0, 0}
    };
    */

    BooleanMatrix::BooleanMatrix M(m, n);
    for(int i = 0; i < m; i++) {
        for(int j = 0; j < n; j++) {
            M[i][j] = static_cast<bool> (bd(rng));
            //M[i][j] = Matrix[i][j];
        }
        M[i][uid(rng)] = true;
    }

    GreedyAlgorithm A = GreedyAlgorithm();
    A.fit(M);
    //A.print_solution(M);
    A.analyze();
    A.print_fit_stats(M);

    return 0;
}