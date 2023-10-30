#include "Greedy.hpp"
#include "BooleanMatrix.hpp"
#include <iostream>

using namespace Greedy;

int main()
{
    int m = 10;
    int n = 10;
    double p = 0.05;

    std::random_device rd{};
    std::mt19937 rng{rd()};
    rng.seed(317);
    std::bernoulli_distribution bd(p);
    std::uniform_int_distribution<> uid(1, n - 1);

    bool Matrix[m][n] = {
        {0, 1, 0, 1, 1, 0, 1, 0, 1, 0},
        {1, 0, 0, 0, 0, 0, 0, 0, 1, 0},
        {0, 0, 0, 0, 0, 1, 0, 0, 1, 0},
        {0, 1, 0, 1, 1, 0, 0, 0, 1, 0},
        {0, 0, 1, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 1, 0, 0, 0, 0, 1, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 1, 0},
        {1, 0, 0, 0, 0, 0, 0, 1, 0, 0},
        {0, 0, 0, 0, 0, 0, 1, 1, 0, 1},
        {0, 0, 0, 0, 0, 0, 0, 1, 0, 1}
    };

    BooleanMatrix::BooleanMatrix M(m, n);
    for(int i = 0; i < m; i++) {
        for(int j = 0; j < n; j++) {
            //M[i][j] = static_cast<bool> (bd(rng));
            M[i][j] = Matrix[i][j];
        }
        //M[i][uid(rng)] = true;
    }

    std::vector<int> features = {1, 1, 1, 1, 1, 2, 2, 2, 2, 3};

    EncodingGreedyAlgorithm A = EncodingGreedyAlgorithm(features);
    A.fit(M);
    A.print_solution(M);
    A.analyze();
    A.print_fit_stats(M);

    return 0;
}