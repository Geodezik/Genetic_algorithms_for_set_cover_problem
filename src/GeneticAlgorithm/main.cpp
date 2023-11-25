#include "BCGA.hpp"
#include "BooleanMatrix.hpp"
#include <iostream>
#include <random>
#include <numeric>

using namespace BCGA;

int main()
{
    int m = 10;
    int n = 10;
    double p = 0.002;
    int seed = 317;

    int K = 8;
    float C = 1.0;
    int population_size = 1;
    int max_iter = 150;

    std::random_device rd{};
    std::mt19937 rng{rd()};
    rng.seed(seed);
    std::bernoulli_distribution bd(p);
    std::uniform_int_distribution<> uid(1, n - 1);

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

    BooleanMatrix::BooleanMatrix M(m, n);
    for(int i = 0; i < m; i++) {
        for(int j = 0; j < n; j++) {
            //M[i][j] = static_cast<bool> (bd(rng));
            M[i][j] = Matrix[i][j];
        }
        //M[i][uid(rng)] = true;
    }

    std::vector<int> groups_idx = {0, 5, 9};

    EncodingSotnezovBCGA A = EncodingSotnezovBCGA(population_size, groups_idx, Fitness::CovLen, K, C, max_iter, seed, OutputMode::Max);
    A.fit(M);
    A.print_solution(M);

    return 0;
}